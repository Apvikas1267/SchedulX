const url = require('url');
const { World, schedRun, ALGO_NAMES, ALGO_FCFS, ALGO_SJF, ALGO_SRTF, ALGO_RR, ALGO_PRIO_NP, ALGO_PRIO_P, djb2Hash } = require('./engine.js');

// Global in-memory world (mirrors C "World G")
const G = new World();

function parseBody(req) {
  return new Promise((resolve) => {
    if (req.method === 'GET' || req.method === 'DELETE' || req.method === 'HEAD') {
      return resolve({});
    }
    if (req.body && typeof req.body === 'object') return resolve(req.body);
    if (typeof req.body === 'string') {
      try { return resolve(Object.fromEntries(new URLSearchParams(req.body))); } catch (e) {}
    }
    let data = '';
    req.on('data', chunk => { data += chunk; });
    req.on('end', () => {
      if (!data) return resolve({});
      try {
        const parsed = Object.fromEntries(new URLSearchParams(data));
        resolve(parsed);
      } catch (e) {
        resolve({});
      }
    });
    // safety timeout for empty body
    setTimeout(() => resolve({}), 100);
  });
}

function parseProcs(str) {
  const procs = [];
  const entries = str.split(';').map(s => s.trim()).filter(Boolean);
  for (const entry of entries) {
    const parts = entry.split(',');
    if (parts.length !== 4) return { error: `Malformed process entry '${entry}'` };
    const [pid, aStr, bStr, prStr] = parts;
    const a = parseInt(aStr, 10);
    const b = parseInt(bStr, 10);
    const pr = parseInt(prStr, 10);
    if (isNaN(a) || isNaN(b) || isNaN(pr)) return { error: `Malformed process entry '${entry}'` };
    procs.push({ pid, arrival: a, burst: b, priority: pr, index: procs.length });
  }
  return { procs };
}

function parseDeps(str) {
  const deps = [];
  const entries = str.split(';').map(s => s.trim()).filter(Boolean);
  for (const entry of entries) {
    const parts = entry.split('>');
    if (parts.length !== 2) return { error: `Malformed dependency '${entry}'` };
    deps.push({ from: parts[0].trim(), to: parts[1].trim() });
  }
  return { deps };
}

module.exports = async (req, res) => {
  // Enable CORS
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
  if (req.method === 'OPTIONS') {
    res.statusCode = 200;
    return res.end();
  }

  const parsedUrl = url.parse(req.url, true);
  const pathname = parsedUrl.pathname.replace(/\/+$/, '') || '/';
  const query = parsedUrl.query || {};
  const method = req.method;
  const body = await parseBody(req);

  res.setHeader('Content-Type', 'application/json');

  const send = (status, obj) => {
    res.statusCode = status;
    res.end(JSON.stringify(obj));
  };
  const err = (status, msg) => send(status, { ok: false, error: msg });

  // 1. Health check
  if (pathname === '/api/health') {
    return send(200, { ok: true, service: 'SchedulX C backend (Vercel Serverless)' });
  }

  // 2. Processes list or add: /api/processes
  if (pathname === '/api/processes') {
    if (method === 'GET') {
      const q = (query.q || '').trim().toLowerCase();
      let list = G.list;
      if (q) list = list.filter(p => p.pid.toLowerCase().includes(q));
      return send(200, { ok: true, processes: list, count: list.length, total: G.list.length });
    }
    if (method === 'POST') {
      const pid = (body.pid || '').trim();
      const a = parseInt(body.arrival, 10);
      const b = parseInt(body.burst, 10);
      const pr = parseInt(body.priority, 10);
      if (!pid) return err(400, "Missing field 'pid'");
      if (isNaN(a) || isNaN(b) || isNaN(pr)) return err(400, 'Arrival, burst, and priority must be numbers');
      const addErr = G.addProcess(pid, a, b, pr);
      if (addErr) return err(400, addErr);
      return send(201, { ok: true, processes: G.list, count: G.list.length, total: G.list.length });
    }
    return err(405, 'Method not allowed');
  }

  // 3. Single process: /api/processes/:pid
  if (pathname.startsWith('/api/processes/')) {
    const pid = decodeURIComponent(pathname.substring('/api/processes/'.length));
    if (method === 'GET') {
      const p = G.ht.get(pid);
      if (!p) return err(404, 'Process not found');
      return send(200, { ok: true, process: p, bucket: djb2Hash(pid) });
    }
    if (method === 'PUT') {
      const a = parseInt(body.arrival, 10);
      const b = parseInt(body.burst, 10);
      const pr = parseInt(body.priority, 10);
      if (isNaN(a) || isNaN(b) || isNaN(pr)) return err(400, 'Arrival, burst, and priority must be numbers');
      const upErr = G.updateProcess(pid, a, b, pr);
      if (upErr) return err(400, upErr);
      return send(200, { ok: true, processes: G.list, count: G.list.length, total: G.list.length });
    }
    if (method === 'DELETE') {
      const delOk = G.deleteProcess(pid);
      if (!delOk) return err(404, 'Process not found');
      return send(200, { ok: true, processes: G.list, count: G.list.length, total: G.list.length });
    }
    return err(405, 'Method not allowed');
  }

  // 4. Sample dataset: /api/sample
  if (pathname === '/api/sample' && method === 'POST') {
    const withDeps = body.deps === '1' || body.deps === 1;
    G.loadSample(withDeps);
    return send(200, { ok: true, processes: G.list, count: G.list.length, total: G.list.length });
  }

  // 5. Reset: /api/reset
  if (pathname === '/api/reset' && method === 'POST') {
    G.clear();
    return send(200, { ok: true, processes: G.list, count: 0, total: 0 });
  }

  // 6. Dependencies: /api/deps
  if (pathname === '/api/deps') {
    if (method === 'GET') {
      return send(200, { ok: true, deps: G.deps });
    }
    const from = (method === 'POST' ? body.from : (query.from || body.from || '')).trim();
    const to = (method === 'POST' ? body.to : (query.to || body.to || '')).trim();
    if (!from || !to) return err(400, "Fields 'from' and 'to' are required");

    if (method === 'POST') {
      const depErr = G.addDep(from, to, true);
      if (depErr) return err(400, depErr);
      return send(201, { ok: true, deps: G.deps });
    }
    if (method === 'DELETE') {
      const rmOk = G.removeDep(from, to);
      if (!rmOk) return err(404, 'Dependency not found');
      return send(200, { ok: true, deps: G.deps });
    }
    return err(405, 'Method not allowed');
  }

  // 7. Graph: /api/graph
  if (pathname === '/api/graph' && method === 'GET') {
    const { arr, graph } = G.build();
    const topo = graph.topoSort();
    const cycle = topo.length < arr.length ? graph.findCycle() : null;
    return send(200, {
      ok: true,
      nodes: arr.map(p => ({
        pid: p.pid,
        indegree: graph.indeg[p.index],
        adj: graph.adj[p.index].map(idx => arr[idx].pid)
      })),
      hasCycle: cycle !== null && cycle.length > 0,
      topo: topo.map(idx => arr[idx].pid),
      cycle: cycle ? cycle.map(idx => arr[idx].pid) : []
    });
  }

  // 8. Hash table: /api/hash
  if (pathname === '/api/hash' && method === 'GET') {
    return send(200, { ok: true, table: G.ht.toJSON() });
  }

  // 9. Simulate: /api/simulate
  if (pathname === '/api/simulate' && method === 'POST') {
    const algo = (body.algo || '').trim();
    const quantum = parseInt(body.quantum || '2', 10);
    const high = body.prio === 'high';
    let procsToSchedule, graphToSchedule;

    if (body.procs) {
      const parsed = parseProcs(body.procs);
      if (parsed.error) return err(400, parsed.error);
      procsToSchedule = parsed.procs;

      const pmap = {};
      procsToSchedule.forEach((p, i) => { pmap[p.pid] = i; });
      graphToSchedule = new (require('./engine.js').Graph)(procsToSchedule.length);

      if (body.deps) {
        const parsedD = parseDeps(body.deps);
        if (parsedD.error) return err(400, parsedD.error);
        for (const d of parsedD.deps) {
          if (pmap[d.from] !== undefined && pmap[d.to] !== undefined) {
            graphToSchedule.addEdge(pmap[d.from], pmap[d.to]);
          }
        }
      }
    } else {
      const built = G.build();
      procsToSchedule = built.arr;
      graphToSchedule = built.graph;
    }

    if (!ALGO_NAMES[algo]) return err(400, 'Unknown algorithm');
    const result = schedRun(procsToSchedule, graphToSchedule, algo, quantum, high);
    if (!result.ok) return err(400, result.error);
    return send(200, result);
  }

  // 10. Compare: /api/compare
  if (pathname === '/api/compare' && method === 'POST') {
    const quantum = parseInt(body.quantum || '2', 10);
    const high = body.prio === 'high';
    const algos = [ALGO_FCFS, ALGO_SJF, ALGO_SRTF, ALGO_RR, ALGO_PRIO_NP, ALGO_PRIO_P];
    const results = [];

    for (const a of algos) {
      const built = G.build();
      const res = schedRun(built.arr, built.graph, a, quantum, high);
      if (!res.ok) return err(400, res.error);
      const s = res.stats;
      results.push({
        algo: a,
        name: ALGO_NAMES[a],
        avgWait: s.avgWait,
        avgTat: s.avgTat,
        avgResp: s.avgResp,
        cpuUtil: s.cpuUtil,
        throughput: s.throughput,
        totalTime: s.totalTime,
        busyTime: s.busyTime
      });
    }

    return send(200, {
      ok: true,
      quantum,
      count: G.list.length,
      results
    });
  }

  return err(404, 'Unknown API route');
};
