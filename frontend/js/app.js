/* Navigation, dashboard and start-up. */
const State = { algo: '-', last: null, count: 0 };

const OVERVIEW = [
  ['FCFS', 'Runs processes in order of arrival.', 'Linked-list FIFO queue'],
  ['SJF', 'Shortest burst first, non-preemptive.', 'Min-heap (burst time)'],
  ['SRTF', 'Shortest remaining time, preemptive.', 'Min-heap (remaining time)'],
  ['Round Robin', 'Fixed time quantum, fair rotation.', 'Circular queue'],
  ['Priority (non-preemptive)', 'Highest priority first; lower number = higher priority.', 'Priority queue (min-heap)'],
  ['Priority (preemptive)', 'A higher-priority arrival pre-empts the CPU.', 'Priority queue (min-heap)']
];
const DS_TABLE = [
  ['Singly linked list', 'Dynamic process store: insert, delete, search, traversal', 'linkedlist.c'],
  ['Hash table (chaining)', 'O(1) PID lookup, collision handling by chaining', 'hashtable.c'],
  ['Queue (linked list)', 'FCFS ready queue', 'queue.c'],
  ['Circular queue (array)', 'Round Robin ready queue', 'queue.c'],
  ['Binary min-heap', 'SJF / SRTF / priority selection; insert, extract-min', 'heap.c'],
  ['Stack (linked list)', 'CPU execution history (push on dispatch, peek = last process)', 'stack.c'],
  ['Directed graph (adjacency list)', 'Process dependencies, topological sort, cycle detection', 'graph.c']
];

function updateDashboard() {
  const s = State.last;
  const items = [['Processes', State.count], ['Selected algorithm', State.algo], ['Avg waiting time', s ? fmt(s.avgWait) : '-'],
    ['Avg turnaround time', s ? fmt(s.avgTat) : '-'], ['CPU utilisation', s ? fmt(s.cpuUtil) + '%' : '-'], ['Throughput', s ? fmt(s.throughput, 4) : '-']];
  $('#dash-stats').innerHTML = items.map(([l, v]) => `<div class="stat"><div class="v" style="${String(v).length > 12 ? 'font-size:.95rem' : ''}">${esc(v)}</div><div class="l">${l}</div></div>`).join('');
}

function showPage(p) {
  $$('#nav button').forEach(b => b.classList.toggle('active', b.dataset.page === p));
  $$('.page').forEach(s => s.classList.toggle('active', s.id === 'page-' + p));
  if (p === 'visualizer') vizStatic();
  if (p === 'processes') pmLoad().catch(() => {});
}

document.addEventListener('store-changed', ev => { State.count = ev.detail.total; updateDashboard(); });

window.addEventListener('DOMContentLoaded', async () => {
  $('#nav').onclick = e => { if (e.target.dataset.page) showPage(e.target.dataset.page); };
  $('#dash-algos').innerHTML = OVERVIEW.map(([n, d, ds]) => `<div class="card"><h3>${n}</h3><p>${d}</p><p><b>Data structure:</b> ${ds}</p></div>`).join('');
  $('#dash-ds').innerHTML = '<tr><th>Data structure</th><th>Used for</th><th>C file</th></tr>' + DS_TABLE.map(r => `<tr>${r.map(c => `<td>${c}</td>`).join('')}</tr>`).join('');
  initProcesses(); initScheduling(); initVisualizer(); initCompare();
  updateDashboard();
  try { await pmLoad(); } catch (e) { toast(e.message, true); }
});
