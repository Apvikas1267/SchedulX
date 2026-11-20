(function (root, factory) {
  if (typeof module === 'object' && module.exports) {
    module.exports = factory();
  } else {
    root.SchedulXEngine = factory();
  }
})(typeof self !== 'undefined' ? self : this, function () {
  const HT_SIZE = 13;
  const PID_LEN = 16;
  const MAX_TIME_VALUE = 10000;
  const MAX_PROCS = 100;
  const MAX_DEPS = 200;
  const MAX_SEGMENTS = 20000;
  const MAX_TRACE = 2000;

  const ALGO_FCFS = 'fcfs';
  const ALGO_SJF = 'sjf';
  const ALGO_SRTF = 'srtf';
  const ALGO_RR = 'rr';
  const ALGO_PRIO_NP = 'prio_np';
  const ALGO_PRIO_P = 'prio_p';

  const ALGO_NAMES = {
    fcfs: 'First Come First Served (FCFS)',
    sjf: 'Shortest Job First (non-preemptive)',
    srtf: 'Shortest Remaining Time First (preemptive)',
    rr: 'Round Robin',
    prio_np: 'Priority Scheduling (non-preemptive)',
    prio_p: 'Priority Scheduling (preemptive)'
  };

  function djb2Hash(str) {
    let hash = 5381;
    for (let i = 0; i < str.length; i++) {
      hash = (((hash << 5) + hash) + str.charCodeAt(i)) >>> 0;
    }
    return hash % HT_SIZE;
  }

  class HashTable {
    constructor() {
      this.size = HT_SIZE;
      this.count = 0;
      this.buckets = Array.from({ length: HT_SIZE }, () => []);
    }
    put(key, val) {
      const b = djb2Hash(key);
      for (const node of this.buckets[b]) {
        if (node.key === key) return -1;
      }
      this.buckets[b].unshift({ key, val }); // collision handling: chain at head
      this.count++;
      return 0;
    }
    get(key) {
      const b = djb2Hash(key);
      for (const node of this.buckets[b]) {
        if (node.key === key) return node.val;
      }
      return null;
    }
    remove(key) {
      const b = djb2Hash(key);
      const chain = this.buckets[b];
      for (let i = 0; i < chain.length; i++) {
        if (chain[i].key === key) {
          chain.splice(i, 1);
          this.count--;
          return true;
        }
      }
      return false;
    }
    clear() {
      for (let i = 0; i < HT_SIZE; i++) this.buckets[i] = [];
      this.count = 0;
    }
    collisions() {
      let c = 0;
      for (let i = 0; i < HT_SIZE; i++) {
        if (this.buckets[i].length > 1) c += this.buckets[i].length - 1;
      }
      return c;
    }
    toJSON() {
      return {
        size: this.size,
        count: this.count,
        collisions: this.collisions(),
        loadFactor: Number((this.count / this.size).toFixed(3)),
        buckets: this.buckets.map((chain, index) => ({
          index,
          chain: chain.map(n => n.key)
        }))
      };
    }
  }

  class MinHeap {
    constructor(cap, mode) {
      this.cap = cap;
      this.mode = mode; // 'burst' | 'remaining' | 'priority'
      this.a = [];
    }
    heapKey(p) {
      if (this.mode === 'burst') return p.burst;
      if (this.mode === 'remaining') return p.remaining;
      return p.pkey;
    }
    less(a, b) {
      const ka = this.heapKey(a), kb = this.heapKey(b);
      if (ka !== kb) return ka < kb;
      if (a.arrival !== b.arrival) return a.arrival < b.arrival;
      return a.index < b.index;
    }
    insert(p) {
      if (this.a.length >= this.cap) return -1;
      let i = this.a.length;
      this.a.push(p);
      while (i > 0) {
        const parent = Math.floor((i - 1) / 2);
        if (!this.less(this.a[i], this.a[parent])) break;
        const t = this.a[i]; this.a[i] = this.a[parent]; this.a[parent] = t;
        i = parent;
      }
      return 0;
    }
    extractMin() {
      if (this.a.length === 0) return null;
      const root = this.a[0];
      const last = this.a.pop();
      if (this.a.length > 0) {
        this.a[0] = last;
        let i = 0;
        while (true) {
          const l = 2 * i + 1, r = l + 1;
          let m = i;
          if (l < this.a.length && this.less(this.a[l], this.a[m])) m = l;
          if (r < this.a.length && this.less(this.a[r], this.a[m])) m = r;
          if (m === i) break;
          const t = this.a[i]; this.a[i] = this.a[m]; this.a[m] = t;
          i = m;
        }
      }
      return root;
    }
    peek() { return this.a.length > 0 ? this.a[0] : null; }
    empty() { return this.a.length === 0; }
    toJSON() {
      return {
        kind: 'heap',
        mode: this.mode,
        items: this.a.map(p => ({ pid: p.pid, key: this.heapKey(p) }))
      };
    }
  }

  class CircularQueue {
    constructor(cap) {
      this.cap = cap;
      this.slot = new Array(cap).fill(null);
      this.front = 0;
      this.size = 0;
    }
    enqueue(p) {
      if (this.size === this.cap) return -1;
      this.slot[(this.front + this.size) % this.cap] = p;
      this.size++;
      return 0;
    }
    dequeue() {
      if (this.size === 0) return null;
      const p = this.slot[this.front];
      this.slot[this.front] = null;
      this.front = (this.front + 1) % this.cap;
      this.size--;
      return p;
    }
    peek() { return this.size > 0 ? this.slot[this.front] : null; }
    empty() { return this.size === 0; }
    toJSON() {
      return {
        kind: 'circular',
        cap: this.cap,
        front: this.front,
        size: this.size,
        slots: this.slot.map(p => (p ? p.pid : null))
      };
    }
  }

  class FIFOQueue {
    constructor() { this.items = []; }
    enqueue(p) { this.items.push(p); return 0; }
    dequeue() { return this.items.length > 0 ? this.items.shift() : null; }
    peek() { return this.items.length > 0 ? this.items[0] : null; }
    empty() { return this.items.length === 0; }
    toJSON() {
      return {
        kind: 'queue',
        items: this.items.map(p => p.pid)
      };
    }
  }

  class Stack {
    constructor() { this.items = []; }
    push(pid, time) { this.items.unshift({ pid, time }); return 0; }
    pop() { return this.items.length > 0 ? this.items.shift() : null; }
    peek() { return this.items.length > 0 ? this.items[0] : null; }
    empty() { return this.items.length === 0; }
    toJSON(limit = 12) {
      return this.items.slice(0, limit).map(n => ({ pid: n.pid, time: n.time }));
    }
  }

  class Graph {
    constructor(n) {
      this.n = n;
      this.adj = Array.from({ length: n }, () => []);
      this.indeg = new Array(n).fill(0);
    }
    addEdge(u, v) {
      if (u < 0 || v < 0 || u >= this.n || v >= this.n || u === v) return -1;
      if (this.adj[u].includes(v)) return -1;
      this.adj[u].unshift(v); // C prepends to adjacency list
      this.indeg[v]++;
      return 0;
    }
    topoSort() {
      const deg = [...this.indeg];
      const q = [];
      for (let i = 0; i < this.n; i++) {
        if (deg[i] === 0) q.push(i);
      }
      const order = [];
      let head = 0;
      while (head < q.length) {
        const u = q[head++];
        order.push(u);
        for (const v of this.adj[u]) {
          if (--deg[v] === 0) q.push(v);
        }
      }
      return order;
    }
    findCycle() {
      const color = new Array(this.n).fill(0);
      const stack = [];
      let cyclePath = [];

      const dfs = (u) => {
        color[u] = 1;
        stack.push(u);
        for (const v of this.adj[u]) {
          if (color[v] === 1) {
            const idx = stack.indexOf(v);
            cyclePath = stack.slice(idx);
            cyclePath.push(v);
            return true;
          }
          if (color[v] === 0 && dfs(v)) return true;
        }
        color[u] = 2;
        stack.pop();
        return false;
      };

      for (let i = 0; i < this.n; i++) {
        if (color[i] === 0 && dfs(i)) return cyclePath;
      }
      return null;
    }
  }

  function statsCompute(arr) {
    let wait = 0, tat = 0, resp = 0;
    let last = 0, busy = 0, done = 0;
    for (const p of arr) {
      if (!p.done) continue;
      done++;
      wait += p.waiting;
      tat += p.turnaround;
      resp += p.response;
      busy += p.burst;
      if (p.completion > last) last = p.completion;
    }
    return {
      completed: done,
      busy_time: busy,
      total_time: last,
      avg_wait: done ? wait / done : 0,
      avg_tat: done ? tat / done : 0,
      avg_resp: done ? resp / done : 0,
      cpu_util: last ? (100.0 * busy) / last : 0,
      throughput: last ? done / last : 0
    };
  }

  function schedRun(arr, graph, algo, quantum = 2, highIsHigh = false) {
    const n = arr.length;
    if (n <= 0) return { ok: false, error: 'No processes to schedule' };
    if (algo === ALGO_RR && (quantum < 1 || quantum > MAX_TIME_VALUE)) {
      return { ok: false, error: `Time quantum must be between 1 and ${MAX_TIME_VALUE}` };
    }

    const topo = graph.topoSort();
    if (topo.length < n) {
      const cycle = graph.findCycle();
      let msg = 'Circular dependency detected';
      if (cycle && cycle.length) {
        msg += ': ' + cycle.map(idx => arr[idx].pid).join(' -> ');
      }
      return { ok: false, error: msg };
    }

    const indeg = [...graph.indeg];
    const readyAt = arr.map(p => p.arrival);
    for (let i = 0; i < n; i++) {
      const p = arr[i];
      p.remaining = p.burst;
      p.pkey = highIsHigh ? -p.priority : p.priority;
      p.start_time = -1;
      p.completion = 0;
      p.turnaround = 0;
      p.waiting = 0;
      p.response = 0;
      p.admitted = false;
      p.done = false;
    }

    let ready;
    if (algo === ALGO_FCFS) ready = new FIFOQueue();
    else if (algo === ALGO_RR) ready = new CircularQueue(n + 1);
    else if (algo === ALGO_SJF) ready = new MinHeap(n, 'burst');
    else if (algo === ALGO_SRTF) ready = new MinHeap(n, 'remaining');
    else ready = new MinHeap(n, 'priority');

    const hist = new Stack();
    const seg = [];
    const trace = [];
    let ops = [];
    let t = 0, done = 0, fail = null;
    let lastProc = null;

    function op(s) { ops.push(s); }

    function readyInsert(p) {
      if (algo === ALGO_FCFS) { ready.enqueue(p); op(`enqueue ${p.pid}`); }
      else if (algo === ALGO_RR) { ready.enqueue(p); op(`cq-enqueue ${p.pid}`); }
      else { ready.insert(p); op(`heap-insert ${p.pid}`); }
    }

    function readyPop() {
      if (algo === ALGO_FCFS) { const p = ready.dequeue(); op(`dequeue ${p.pid}`); return p; }
      if (algo === ALGO_RR) { const p = ready.dequeue(); op(`cq-dequeue ${p.pid}`); return p; }
      const p = ready.extractMin(); op(`extract-min ${p.pid}`); return p;
    }

    function admit() {
      const cand = [];
      for (let i = 0; i < n; i++) {
        const p = arr[i];
        if (!p.admitted && indeg[i] === 0 && readyAt[i] <= t) {
          cand.push({ ready: readyAt[i], arrival: p.arrival, index: i, p });
        }
      }
      if (cand.length > 1) {
        cand.sort((x, y) => {
          if (x.ready !== y.ready) return x.ready - y.ready;
          if (x.arrival !== y.arrival) return x.arrival - y.arrival;
          return x.index - y.index;
        });
      }
      for (const c of cand) {
        c.p.admitted = true;
        readyInsert(c.p);
      }
    }

    function nextArrival() {
      let best = -1;
      for (let i = 0; i < n; i++) {
        const p = arr[i];
        if (!p.admitted && indeg[p.index] === 0 && p.arrival > t) {
          if (best < 0 || p.arrival < best) best = p.arrival;
        }
      }
      return best;
    }

    function addSegment(p, start, end) {
      if (end <= start) return;
      if (p && algo !== ALGO_RR && seg.length > 0) {
        const l = seg[seg.length - 1];
        if (!l.idle && l.end === start && l.pid === p.pid) {
          l.end = end;
          l.rem = p.remaining;
          return;
        }
      }
      if (seg.length >= MAX_SEGMENTS) {
        fail = 'Too many Gantt segments; use a larger time quantum';
        return;
      }
      seg.push({
        pid: p ? p.pid : 'IDLE',
        idle: !p,
        start,
        end,
        rem: p ? p.remaining : 0
      });
    }

    function snapshot(p) {
      if (trace.length >= MAX_TRACE) return;
      trace.push({
        time: t,
        pid: p.pid,
        ops: ops.join('; '),
        ready: ready.toJSON(),
        stack: hist.toJSON(12)
      });
      ops = [];
    }

    function dispatch(p) {
      if (p.start_time < 0) p.start_time = t;
      hist.push(p.pid, t);
      op(`push ${p.pid}`);
      snapshot(p);
      lastProc = p;
    }

    function complete(p) {
      p.completion = t;
      p.turnaround = p.completion - p.arrival;
      p.waiting = p.turnaround - p.burst;
      p.response = p.start_time - p.arrival;
      p.done = true;
      done++;
      for (const to of graph.adj[p.index]) {
        indeg[to]--;
        if (readyAt[to] < t) readyAt[to] = t;
      }
    }

    function idleJump() {
      const nt = nextArrival();
      if (nt < 0) {
        fail = 'Deadlock: remaining processes can never become ready';
        return false;
      }
      addSegment(null, t, nt);
      t = nt;
      return true;
    }

    if (algo === ALGO_SRTF || algo === ALGO_PRIO_P) {
      while (done < n && !fail) {
        admit();
        if (ready.empty()) {
          lastProc = null;
          if (!idleJump()) break;
          continue;
        }
        const p = ready.peek();
        if (p !== lastProc) dispatch(p);
        const nt = nextArrival();
        let run = p.remaining;
        if (nt >= 0 && nt - t < run) run = nt - t;
        t += run;
        p.remaining -= run;
        addSegment(p, t - run, t);
        if (p.remaining === 0) {
          ready.extractMin();
          op(`extract-min ${p.pid} (finished)`);
          complete(p);
          lastProc = null;
        }
      }
    } else {
      while (done < n && !fail) {
        admit();
        if (ready.empty()) {
          if (!idleJump()) break;
          continue;
        }
        const p = readyPop();
        dispatch(p);
        let run = p.remaining;
        if (algo === ALGO_RR && run > quantum) run = quantum;
        t += run;
        p.remaining -= run;
        addSegment(p, t - run, t);
        if (p.remaining === 0) complete(p);
        if (algo === ALGO_RR) {
          admit();
          if (p.remaining > 0) readyInsert(p);
        }
      }
    }

    if (fail) return { ok: false, error: fail };

    const st = statsCompute(arr);
    return {
      ok: true,
      algo,
      algoName: ALGO_NAMES[algo],
      quantum,
      priorityRule: highIsHigh ? 'higher number = higher priority' : 'lower number = higher priority',
      processes: arr.map(p => ({
        pid: p.pid,
        arrival: p.arrival,
        burst: p.burst,
        priority: p.priority,
        start: p.start_time,
        completion: p.completion,
        turnaround: p.turnaround,
        waiting: p.waiting,
        response: p.response
      })),
      gantt: seg,
      stats: {
        avgWait: st.avg_wait,
        avgTat: st.avg_tat,
        avgResp: st.avg_resp,
        cpuUtil: st.cpu_util,
        throughput: st.throughput,
        totalTime: st.total_time,
        busyTime: st.busy_time,
        completed: st.completed
      },
      topo: topo.map(idx => arr[idx].pid),
      trace
    };
  }

  // World class representing the C "World G"
  class World {
    constructor() {
      this.list = []; // singly linked list order
      this.ht = new HashTable();
      this.deps = [];
      this.loadSample(false);
    }
    clear() {
      this.list = [];
      this.ht.clear();
      this.deps = [];
    }
    addProcess(pid, a, b, pr) {
      if (!/^[A-Za-z0-9_-]{1,15}$/.test(pid)) return 'Process ID must be 1-15 characters: letters, digits, \'_\' or \'-\'';
      if (a < 0 || a > MAX_TIME_VALUE) return `Arrival time must be between 0 and ${MAX_TIME_VALUE}`;
      if (b < 1 || b > MAX_TIME_VALUE) return `Burst time must be between 1 and ${MAX_TIME_VALUE}`;
      if (pr < 0 || pr > 999) return 'Priority must be between 0 and 999';
      if (this.list.length >= MAX_PROCS) return `At most ${MAX_PROCS} processes are supported`;
      if (this.ht.get(pid)) return `Process ID '${pid}' already exists`;

      const p = { pid, arrival: a, burst: b, priority: pr };
      this.list.push(p);
      this.ht.put(pid, p);
      return null;
    }
    updateProcess(pid, a, b, pr) {
      const p = this.ht.get(pid);
      if (!p) return `Process '${pid}' not found`;
      if (a < 0 || a > MAX_TIME_VALUE) return `Arrival time must be between 0 and ${MAX_TIME_VALUE}`;
      if (b < 1 || b > MAX_TIME_VALUE) return `Burst time must be between 1 and ${MAX_TIME_VALUE}`;
      if (pr < 0 || pr > 999) return 'Priority must be between 0 and 999';
      p.arrival = a;
      p.burst = b;
      p.priority = pr;
      return null;
    }
    deleteProcess(pid) {
      if (!this.ht.get(pid)) return false;
      this.deps = this.deps.filter(d => d.from !== pid && d.to !== pid);
      this.ht.remove(pid);
      this.list = this.list.filter(p => p.pid !== pid);
      return true;
    }
    addDep(from, to, checkCycle = true) {
      if (!this.ht.get(from) || !this.ht.get(to)) return `Unknown process in dependency ${from} -> ${to}`;
      if (from === to) return 'A process cannot depend on itself';
      if (this.deps.some(d => d.from === from && d.to === to)) return `Dependency ${from} -> ${to} already exists`;
      if (this.deps.length >= MAX_DEPS) return 'Too many dependencies';

      this.deps.push({ from, to });
      if (checkCycle) {
        const { arr, graph } = this.build();
        const cycle = graph.findCycle();
        if (cycle && cycle.length) {
          this.deps.pop();
          return 'Circular dependency detected: ' + cycle.map(i => arr[i].pid).join(' -> ');
        }
      }
      return null;
    }
    removeDep(from, to) {
      const idx = this.deps.findIndex(d => d.from === from && d.to === to);
      if (idx >= 0) {
        this.deps.splice(idx, 1);
        return true;
      }
      return false;
    }
    loadSample(withDeps = false) {
      this.clear();
      const S = [
        { pid: 'P1', a: 0, b: 8, p: 3 },
        { pid: 'P2', a: 1, b: 4, p: 1 },
        { pid: 'P3', a: 2, b: 9, p: 4 },
        { pid: 'P4', a: 3, b: 5, p: 2 },
        { pid: 'P5', a: 30, b: 3, p: 1 }
      ];
      for (const s of S) this.addProcess(s.pid, s.a, s.b, s.p);
      if (withDeps) {
        this.addDep('P2', 'P4', true);
        this.addDep('P1', 'P3', true);
      }
    }
    build() {
      const arr = this.list.map((p, i) => ({ ...p, index: i }));
      const g = new Graph(arr.length);
      const map = {};
      arr.forEach((p, i) => { map[p.pid] = i; });
      for (const d of this.deps) {
        if (map[d.from] !== undefined && map[d.to] !== undefined) {
          g.addEdge(map[d.from], map[d.to]);
        }
      }
      return { arr, graph: g };
    }
  }

  return {
    HT_SIZE,
    PID_LEN,
    HashTable,
    MinHeap,
    CircularQueue,
    FIFOQueue,
    Stack,
    Graph,
    World,
    schedRun,
    statsCompute,
    djb2Hash,
    ALGO_NAMES,
    ALGO_FCFS,
    ALGO_SJF,
    ALGO_SRTF,
    ALGO_RR,
    ALGO_PRIO_NP,
    ALGO_PRIO_P
  };
});
