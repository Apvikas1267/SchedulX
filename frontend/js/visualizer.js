/* DSA Visualizer: draws structures exactly as returned by the C backend. */
const VIZ = { sim: null, step: 0, timer: null };

function listHtml(procs) {
  if (!procs.length) return '<span class="null">head → NULL</span>';
  return '<div class="nodes"><span class="arrow">head</span>' + procs.map(p =>
    `<span class="arrow">→</span><div class="node"><b>${esc(p.pid)}</b><small>AT ${p.arrival} BT ${p.burst} PR ${p.priority}</small></div>`).join('') + '<span class="arrow">→</span><span class="null">NULL</span></div>';
}

function hashHtml(t) {
  return `<p class="hint">size ${t.size}, items ${t.count}, collisions ${t.collisions}, load factor ${t.loadFactor} &nbsp;(hash = djb2 mod ${t.size})</p>` +
    t.buckets.map(b => `<div class="bucket"><span class="ix">${b.index}</span>${b.chain.length ? b.chain.map((k, i) => (i ? '<span class="arrow">→</span>' : '') + `<span class="chip">${esc(k)}</span>`).join('') : '<span class="null">empty</span>'}</div>`).join('');
}

function graphSvg(g) {
  const n = g.nodes.length;
  if (!n) return '<p class="null">No processes.</p>';
  const W = 480, H = 300, cx = W / 2, cy = H / 2, rx = W / 2 - 45, ry = H / 2 - 40, pos = {};
  g.nodes.forEach((nd, i) => { const a = 2 * Math.PI * i / n - Math.PI / 2; pos[nd.pid] = { x: cx + rx * Math.cos(a), y: cy + ry * Math.sin(a) }; });
  const cyc = new Set(); for (let i = 0; i + 1 < g.cycle.length; i++) cyc.add(g.cycle[i] + '>' + g.cycle[i + 1]);
  let edges = '';
  g.nodes.forEach(nd => nd.adj.forEach(to => {
    const a = pos[nd.pid], b = pos[to], d = Math.hypot(b.x - a.x, b.y - a.y) || 1, r = 22;
    edges += `<line x1="${a.x + (b.x - a.x) / d * r}" y1="${a.y + (b.y - a.y) / d * r}" x2="${b.x - (b.x - a.x) / d * (r + 4)}" y2="${b.y - (b.y - a.y) / d * (r + 4)}" stroke="${cyc.has(nd.pid + '>' + to) ? '#e03131' : '#3b5bdb'}" stroke-width="2" marker-end="url(#ah)"/>`;
  }));
  const nodes = g.nodes.map(nd => `<g><circle cx="${pos[nd.pid].x}" cy="${pos[nd.pid].y}" r="22" fill="${colorFor(nd.pid)}" stroke="#333"/><text x="${pos[nd.pid].x}" y="${pos[nd.pid].y + 4}" text-anchor="middle" font-size="13" font-weight="600">${esc(nd.pid)}</text></g>`).join('');
  return `<svg viewBox="0 0 ${W} ${H}" width="100%" style="max-width:520px"><defs><marker id="ah" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" orient="auto"><path d="M0 0L10 5L0 10z" fill="#3b5bdb"/></marker></defs>${edges}${nodes}</svg>`;
}

async function vizStatic() {
  try {
    const [pl, hs, gr, dp] = await Promise.all([api('GET', '/api/processes'), api('GET', '/api/hash'), api('GET', '/api/graph'), api('GET', '/api/deps')]);
    $('#viz-list').innerHTML = listHtml(pl.processes);
    $('#viz-hash').innerHTML = hashHtml(hs.table);
    $('#viz-graph').innerHTML = graphSvg(gr);
    const opts = gr.nodes.map(n => `<option>${esc(n.pid)}</option>`).join('');
    const f = $('#dep-from').value, t = $('#dep-to').value;
    $('#dep-from').innerHTML = opts; $('#dep-to').innerHTML = opts;
    if (f) $('#dep-from').value = f; if (t) $('#dep-to').value = t;
    $('#viz-graph-info').innerHTML = '<h4>Adjacency list</h4>' + (gr.nodes.map(n => `<div class="bucket"><b style="width:44px">${esc(n.pid)}</b>→ ${n.adj.length ? n.adj.map(x => `<span class="chip">${esc(x)}</span>`).join(' ') : '<span class="null">NULL</span>'} <small class="hint">in-degree ${n.indegree}</small></div>`).join('') || '<span class="null">empty</span>') +
      '<h4>Topological order (Kahn)</h4>' + (gr.hasCycle ? `<div class="err">Cycle detected: ${gr.cycle.map(esc).join(' → ')}</div>` : `<div>${gr.topo.map(x => `<span class="chip">${esc(x)}</span>`).join(' → ') || '-'}</div>`) +
      '<h4>Dependencies</h4>' + (dp.deps.map(e => `<div class="bucket"><span class="chip">${esc(e.from)}</span> → <span class="chip">${esc(e.to)}</span> <button class="mini danger" data-rmdep="${esc(e.from)}|${esc(e.to)}">remove</button></div>`).join('') || '<span class="null">none (processes are independent)</span>');
  } catch (e) { toast(e.message, true); }
}

function readyHtml(r) {
  if (r.kind === 'queue') {
    return `<p class="hint">Linked-list FIFO queue (front → rear)</p>` + (r.items.length ? '<div class="nodes"><span class="arrow">front</span>' + r.items.map(x => `<span class="arrow">→</span><div class="node"><b>${esc(x)}</b></div>`).join('') + '<span class="arrow">← rear</span></div>' : '<span class="null">empty queue</span>');
  }
  if (r.kind === 'circular') {
    const rear = r.size ? (r.front + r.size - 1) % r.cap : -1;
    return `<p class="hint">Circular array queue: capacity ${r.cap}, front = ${r.front}, size = ${r.size}, rear = ${rear < 0 ? '-' : rear}</p><div class="slots" style="padding-top:18px;padding-bottom:20px">` +
      r.slots.map((s, i) => `<div class="slot ${s ? 'filled' : ''}">${s ? esc(s) : ''}<small>[${i}]</small>${r.size && i === r.front ? '<span class="tag">FRONT</span>' : ''}${i === rear && rear !== r.front ? '<span class="tag" style="left:auto;right:2px">REAR</span>' : ''}</div>`).join('') + '</div>';
  }
  /* heap */
  const it = r.items;
  if (!it.length) return '<span class="null">heap is empty</span>';
  const levels = Math.floor(Math.log2(it.length)) + 1, W = Math.max(380, 2 ** (levels - 1) * 70), H = levels * 62 + 10, p = i => { const l = Math.floor(Math.log2(i + 1)), pos = i + 1 - 2 ** l; return { x: (pos + .5) / 2 ** l * W, y: 28 + l * 58 }; };
  let s = '';
  it.forEach((_, i) => { if (i) { const a = p(i), b = p(Math.floor((i - 1) / 2)); s += `<line x1="${a.x}" y1="${a.y}" x2="${b.x}" y2="${b.y}" stroke="#98a2b3"/>`; } });
  it.forEach((x, i) => { const a = p(i); s += `<circle cx="${a.x}" cy="${a.y}" r="22" fill="${i === 0 ? '#ffe066' : colorFor(x.pid)}" stroke="#333"/><text x="${a.x}" y="${a.y - 2}" text-anchor="middle" font-size="12" font-weight="600">${esc(x.pid)}</text><text x="${a.x}" y="${a.y + 11}" text-anchor="middle" font-size="10">${x.key}</text>`; });
  return `<p class="hint">Min-heap on <b>${r.mode}</b> (yellow = root = next process). Array: [${it.map(x => esc(x.pid) + ':' + x.key).join(', ')}]</p><svg viewBox="0 0 ${W} ${H}" width="${W}" height="${H}">${s}</svg>`;
}

function renderStep() {
  const t = VIZ.sim ? VIZ.sim.trace : [];
  if (!t.length) { $('#viz-step-info').textContent = 'Run a simulation to see the scheduler step by step.'; $('#viz-ready').innerHTML = ''; $('#viz-stack').innerHTML = ''; return; }
  const s = t[VIZ.step];
  $('#step-range').max = t.length - 1; $('#step-range').value = VIZ.step;
  $('#viz-step-info').innerHTML = `Step <b>${VIZ.step + 1}/${t.length}</b>${t.length >= 2000 ? ' (first 2000 steps)' : ''} - time <b>${s.time}</b>: CPU dispatched <b>${esc(s.pid)}</b>` + (s.ops ? `<div class="ops">${esc(s.ops)}</div>` : '');
  $('#viz-ready-title').textContent = 'Ready structure after the dispatch';
  $('#viz-ready').innerHTML = readyHtml(s.ready);
  $('#viz-stack').innerHTML = s.stack.length ? '<div class="stack">' + s.stack.map(e => `<div><span>${esc(e.pid)}</span><small>t=${e.time}</small></div>`).join('') + '</div>' : '<span class="null">empty</span>';
}

async function vizRun() {
  const algo = $('#viz-algo').value;
  try {
    const r = await api('POST', '/api/simulate', { algo, quantum: $('#viz-q').value, prio: $('#viz-prio').value });
    VIZ.sim = r; VIZ.step = 0; stopPlay();
    renderGantt($('#viz-gantt'), r.gantt, algo === 'rr' ? 'rr' : (algo === 'srtf' || algo === 'prio_p') ? 'preempt' : 'np');
    renderStep();
    State.algo = r.algoName; State.last = r.stats; updateDashboard();
  } catch (e) { toast(e.message, true); VIZ.sim = null; renderStep(); }
}

function stopPlay() { clearInterval(VIZ.timer); VIZ.timer = null; $('#step-play').textContent = '▶ Play'; }

function initVisualizer() {
  $('#viz-run').onclick = vizRun;
  $('#step-prev').onclick = () => { if (VIZ.sim && VIZ.step > 0) { VIZ.step--; renderStep(); } };
  $('#step-next').onclick = () => { if (VIZ.sim && VIZ.step < VIZ.sim.trace.length - 1) { VIZ.step++; renderStep(); } };
  $('#step-range').oninput = e => { VIZ.step = +e.target.value; renderStep(); };
  $('#step-play').onclick = () => {
    if (VIZ.timer) return stopPlay();
    if (!VIZ.sim) return;
    $('#step-play').textContent = '❚❚ Pause';
    VIZ.timer = setInterval(() => { if (VIZ.step < VIZ.sim.trace.length - 1) { VIZ.step++; renderStep(); } else stopPlay(); }, 900);
  };
  $('#dep-add').onclick = async () => {
    try { await api('POST', '/api/deps', { from: $('#dep-from').value, to: $('#dep-to').value }); toast('Dependency added'); vizStatic(); }
    catch (e) { toast(e.message, true); vizStatic(); }
  };
  $('#dep-sample').onclick = async () => { try { await api('POST', '/api/sample', { deps: 1 }); await pmLoad(); toast('Sample processes with dependencies P1→P3, P2→P4 loaded'); vizStatic(); } catch (e) { toast(e.message, true); } };
  $('#viz-graph-info').onclick = async ev => {
    const d = ev.target.dataset.rmdep; if (!d) return;
    const [f, t] = d.split('|');
    try { await api('DELETE', `/api/deps?from=${encodeURIComponent(f)}&to=${encodeURIComponent(t)}`); vizStatic(); } catch (e) { toast(e.message, true); }
  };
}
document.addEventListener('store-changed', () => { if ($('#page-visualizer').classList.contains('active')) vizStatic(); });
