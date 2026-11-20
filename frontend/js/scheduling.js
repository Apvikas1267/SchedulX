/* CPU Scheduling page: one independent section per algorithm. All scheduling is done by the C backend. */
const ALGOS = [
  { id: 'fcfs', tab: 'FCFS', title: 'First Come First Served (FCFS)', kind: 'np', ds: 'linked-list FIFO queue',
    how: '<b>How it works:</b> processes join a FIFO ready queue in the order they become ready and each one runs to completion (non-preemptive). Simple and fair in arrival order, but a long job at the front makes everybody behind it wait (the <i>convoy effect</i>). <b>Data structure:</b> a queue implemented with a singly linked list (enqueue at rear, dequeue at front).' },
  { id: 'sjf', tab: 'SJF', title: 'Shortest Job First (non-preemptive)', kind: 'np', ds: 'min-heap on burst time',
    how: '<b>How it works:</b> whenever the CPU becomes free, the ready process with the <u>smallest burst time</u> is chosen and runs to completion. Gives the minimum average waiting time among non-preemptive algorithms but can starve long jobs. Ties: earlier arrival, then input order. <b>Data structure:</b> a binary min-heap keyed on burst time (insert = sift-up, extract-min = sift-down).' },
  { id: 'srtf', tab: 'SRTF', title: 'Shortest Remaining Time First (preemptive SJF)', kind: 'preempt', ds: 'min-heap on remaining time',
    how: '<b>How it works:</b> the running process is pre-empted as soon as a newly arrived process has a <u>smaller remaining time</u>. Completion time is recorded only when the process finishes its last unit. Red edges in the Gantt chart mark pre-emptions. <b>Data structure:</b> min-heap keyed on remaining burst time; the root is always the process to run.' },
  { id: 'rr', tab: 'Round Robin', title: 'Round Robin (RR)', kind: 'rr', ds: 'circular queue', quantum: true,
    how: '<b>How it works:</b> each process gets the CPU for at most one <u>time quantum</u>. If it is not finished it goes to the rear of the queue; processes that arrive during a slice are queued <i>before</i> the pre-empted one. Good response time, more context switches for small quantum. <b>Data structure:</b> an array-based circular queue (front/rear wrap around with modulo arithmetic).' },
  { id: 'priority', tab: 'Priority', title: 'Priority Scheduling', kind: 'np', ds: 'min-heap on priority', priority: true,
    how: '<b>How it works:</b> the ready process with the highest priority runs first. By default a <u>lower number means higher priority</u> (change it below). In <i>preemptive</i> mode a higher-priority arrival interrupts the running process; in <i>non-preemptive</i> mode it waits for the CPU to be free. Equal priority: earlier arrival first. Low-priority jobs may starve. <b>Data structure:</b> priority queue = binary min-heap keyed on priority.' }
];
const SCHED = {};

function panelHtml(a) {
  const id = a.id;
  return `<div class="panel" id="panel-${id}" hidden>
  <div class="card">
    <h2>${a.title}</h2>
    <div class="explain">${a.how}</div>
    <h3>Processes for this algorithm</h3>
    <div class="form-row">
      <label>PID<input data-f="pid" maxlength="15" placeholder="P1" style="width:90px"></label>
      <label>Arrival<input data-f="arrival" type="number" min="0" value="0" style="width:90px"></label>
      <label>Burst<input data-f="burst" type="number" min="1" value="4" style="width:90px"></label>
      <label>Priority<input data-f="priority" type="number" min="0" value="1" style="width:90px"></label>
      <button data-act="add" class="primary">Add</button>
      <button data-act="import" class="ghost">Use Process Management data</button>
      <button data-act="sample" class="ghost">Sample</button>
      <button data-act="clear" class="danger">Clear</button>
    </div>
    <div class="scroll-x"><table class="tbl" data-role="ds"></table></div>
    <h3 style="margin-top:14px">Settings</h3>
    <div class="form-row">
      ${a.quantum ? `<label>Time quantum<input data-f="quantum" type="number" min="1" value="2" style="width:90px"></label>` : ''}
      ${a.priority ? `<label>Mode<select data-f="mode"><option value="prio_np">Non-preemptive</option><option value="prio_p">Preemptive</option></select></label>` : ''}
      ${a.priority ? `<label>Priority rule<select data-f="prio"><option value="low">lower number = higher priority</option><option value="high">higher number = higher priority</option></select></label>` : ''}
      <label style="flex-direction:row;align-items:center;gap:6px;font-size:.85rem"><input type="checkbox" data-f="deps" style="min-width:0"> apply dependencies from the DSA Visualizer</label>
      <button data-act="run" class="primary">&#9654; Run simulation</button>
    </div>
  </div>
  <div data-role="out"></div></div>`;
}

function renderDs(id) {
  const P = SCHED[id], root = $('#panel-' + id);
  $('[data-role=ds]', root).innerHTML = '<tr><th>PID</th><th>Arrival</th><th>Burst</th><th>Priority</th><th></th></tr>' +
    (P.data.map((p, i) => `<tr><td><b>${esc(p.pid)}</b></td><td>${p.arrival}</td><td>${p.burst}</td><td>${p.priority}</td><td><button class="mini danger" data-rm="${i}">remove</button></td></tr>`).join('') ||
      '<tr><td colspan="5" class="null">No processes - add some or import them.</td></tr>');
}

async function runPanel(a) {
  const P = SCHED[a.id], root = $('#panel-' + a.id), out = $('[data-role=out]', root);
  if (!P.data.length) return toast('Add at least one process first', true);
  const f = n => { const e = $(`[data-f=${n}]`, root); return e ? e.value : null; };
  const algo = a.priority ? f('mode') : a.id;
  const params = { algo, procs: P.data.map(p => [p.pid, p.arrival, p.burst, p.priority].join(',')).join(';') };
  if (a.quantum) params.quantum = f('quantum');
  if (a.priority) params.prio = f('prio');
  try {
    if ($('[data-f=deps]', root).checked) {
      const d = await api('GET', '/api/deps'), pids = new Set(P.data.map(p => p.pid));
      const s = d.deps.filter(e => pids.has(e.from) && pids.has(e.to)).map(e => e.from + '>' + e.to).join(';');
      if (s) params.deps = s;
    }
    const r = await api('POST', '/api/simulate', params);
    const kind = algo === 'rr' ? 'rr' : (algo === 'srtf' || algo === 'prio_p') ? 'preempt' : 'np';
    out.innerHTML = `<div class="card"><h3>Gantt chart - ${esc(r.algoName)}${a.quantum ? ', quantum ' + r.quantum : ''}${a.priority ? ' (' + esc(r.priorityRule) + ')' : ''}</h3><div data-g></div></div>
      <div class="card"><h3>Scheduling statistics</h3>${statCards(r.stats)}${resultTable(r.processes)}
      <p class="hint">Turnaround = Completion - Arrival &nbsp;|&nbsp; Waiting = Turnaround - Burst &nbsp;|&nbsp; Response = First start - Arrival &nbsp;|&nbsp;
      CPU utilisation = busy time / total time (0 to last completion) &nbsp;|&nbsp; Throughput = completed / total time.${params.deps ? '<br>Dependency order (topological sort): <b>' + r.topo.map(esc).join(' → ') + '</b>' : ''}</p></div>`;
    renderGantt($('[data-g]', out), r.gantt, kind);
    State.algo = r.algoName; State.last = r.stats; updateDashboard();
  } catch (e) { out.innerHTML = `<div class="err">${esc(e.message)}</div>`; toast(e.message, true); }
}

function initScheduling() {
  const tabs = $('#sched-tabs'), panels = $('#sched-panels');
  ALGOS.forEach((a, i) => {
    SCHED[a.id] = { data: [] };
    const b = document.createElement('button'); b.textContent = a.tab; b.dataset.tab = a.id; tabs.appendChild(b);
    panels.insertAdjacentHTML('beforeend', panelHtml(a));
    const root = $('#panel-' + a.id);
    root.onclick = async ev => {
      const act = ev.target.dataset.act, rm = ev.target.dataset.rm, P = SCHED[a.id];
      if (rm !== undefined) { P.data.splice(+rm, 1); renderDs(a.id); }
      if (act === 'add') {
        const pid = $('[data-f=pid]', root).value.trim(), n = k => Number($(`[data-f=${k}]`, root).value);
        const err = validateProcess(pid, n('arrival'), n('burst'), n('priority'), P.data.map(p => p.pid));
        if (err) return toast(err, true);
        P.data.push({ pid, arrival: n('arrival'), burst: n('burst'), priority: n('priority') });
        $('[data-f=pid]', root).value = ''; renderDs(a.id);
      }
      if (act === 'clear') { P.data = []; renderDs(a.id); }
      if (act === 'import') { try { const d = await api('GET', '/api/processes'); P.data = d.processes; renderDs(a.id); toast('Imported ' + d.count + ' processes'); } catch (e) { toast(e.message, true); } }
      if (act === 'sample') { P.data = SAMPLE.map(p => ({ ...p })); renderDs(a.id); }
      if (act === 'run') runPanel(a);
    };
    renderDs(a.id);
  });
  tabs.onclick = ev => { if (ev.target.dataset.tab) showTab(ev.target.dataset.tab); };
  showTab('fcfs');
}
const SAMPLE = [{ pid: 'P1', arrival: 0, burst: 8, priority: 3 }, { pid: 'P2', arrival: 1, burst: 4, priority: 1 }, { pid: 'P3', arrival: 2, burst: 9, priority: 4 }, { pid: 'P4', arrival: 3, burst: 5, priority: 2 }, { pid: 'P5', arrival: 30, burst: 3, priority: 1 }];

function showTab(id) {
  $$('#sched-tabs button').forEach(b => b.classList.toggle('active', b.dataset.tab === id));
  $$('.panel').forEach(p => p.hidden = p.id !== 'panel-' + id);
}

/* give every section the stored dataset the first time it arrives */
let schedSeeded = false;
document.addEventListener('store-changed', ev => {
  if (schedSeeded || !SCHED.fcfs) return;
  schedSeeded = true;
  ALGOS.forEach(a => { SCHED[a.id].data = ev.detail.processes.map(p => ({ ...p })); renderDs(a.id); });
});
