/* Shared helpers: talking to the C backend, escaping, colours, toast. */
const API_BASE = location.protocol === 'file:' ? 'http://localhost:8080' : '';

async function api(method, path, params) {
  const opt = { method };
  if (params) {
    opt.headers = { 'Content-Type': 'application/x-www-form-urlencoded' };
    opt.body = new URLSearchParams(params).toString();
  }
  let res;
  try { res = await fetch(API_BASE + path, opt); }
  catch (e) { setStatus(false); throw new Error('Cannot reach the C backend. Is schedulx.exe running?'); }
  setStatus(true);
  let data;
  try { data = await res.json(); } catch (e) { throw new Error('Invalid response from backend'); }
  if (!data.ok) throw new Error(data.error || 'Request failed');
  return data;
}

function setStatus(on) {
  const el = document.getElementById('backend-status');
  el.className = 'status ' + (on ? 'on' : 'off');
  el.textContent = on ? 'C backend: connected' : 'C backend: offline';
}

const $ = (s, r = document) => r.querySelector(s);
const $$ = (s, r = document) => [...r.querySelectorAll(s)];
const esc = s => String(s).replace(/[&<>"']/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[c]));

const PALETTE = ['#74c0fc', '#ffa94d', '#8ce99a', '#f783ac', '#b197fc', '#ffe066', '#66d9e8', '#ff8787', '#a9e34b', '#e599f7', '#96f2d7', '#fcc2d7'];
const colorMap = new Map();
function colorFor(pid) {
  if (!colorMap.has(pid)) colorMap.set(pid, PALETTE[colorMap.size % PALETTE.length]);
  return colorMap.get(pid);
}

let toastTimer;
function toast(msg, bad) {
  const t = $('#toast');
  t.textContent = msg; t.className = 'show' + (bad ? ' bad' : '');
  clearTimeout(toastTimer); toastTimer = setTimeout(() => t.className = '', 2800);
}

const fmt = (x, d = 2) => Number(x).toFixed(d);

/* Gantt chart. kind: 'np' | 'preempt' | 'rr'. */
function renderGantt(el, gantt, kind) {
  if (!gantt.length) { el.innerHTML = ''; return; }
  const last = gantt.length - 1;
  const segs = gantt.map((g, i) => {
    const dur = g.end - g.start;
    let mark = '';
    if (!g.idle && g.rem > 0) mark = kind === 'rr' ? 'quantum' : 'preempt';
    const tip = g.idle ? `CPU idle ${g.start}-${g.end}` : `${g.pid}: ${g.start}-${g.end}` + (g.rem > 0 ? ` (${g.rem} left)` : ' (finished)');
    return `<div class="seg ${g.idle ? 'idle' : ''} ${mark}" style="flex:${dur} 1 0;${g.idle ? '' : 'background:' + colorFor(g.pid)}" title="${esc(tip)}">
      <span>${g.idle ? 'IDLE' : esc(g.pid)}</span><span class="dur">${dur}</span>
      <span class="t0">${g.start}</span>${i === last ? `<span class="t1">${g.end}</span>` : ''}</div>`;
  }).join('');
  const legend = kind === 'preempt' ? '<span><i style="background:var(--bad)"></i>red edge = process was pre-empted</span>'
    : kind === 'rr' ? '<span><i style="background:var(--warn)"></i>orange edge = time quantum expired, process re-queued</span>' : '';
  el.innerHTML = `<div class="gantt-scroll"><div class="gantt">${segs}</div></div>
    <div class="legend"><span><i style="background:#e9ecef"></i>idle CPU</span>${legend}<span>number inside a block = duration</span></div>`;
}

function statCards(s) {
  const items = [['Avg waiting', fmt(s.avgWait)], ['Avg turnaround', fmt(s.avgTat)], ['Avg response', fmt(s.avgResp)],
    ['CPU utilisation', fmt(s.cpuUtil) + '%'], ['Throughput', fmt(s.throughput, 4) + ' /unit'], ['Total time', s.totalTime]];
  return '<div class="stats">' + items.map(([l, v]) => `<div class="stat"><div class="v">${v}</div><div class="l">${l}</div></div>`).join('') + '</div>';
}

function resultTable(procs) {
  return `<div class="scroll-x"><table class="tbl"><tr><th>PID</th><th>Arrival</th><th>Burst</th><th>Priority</th><th>First start</th><th>Completion</th><th>Turnaround</th><th>Waiting</th><th>Response</th></tr>` +
    procs.map(p => `<tr><td><b style="border-left:10px solid ${colorFor(p.pid)};padding-left:6px">${esc(p.pid)}</b></td><td>${p.arrival}</td><td>${p.burst}</td><td>${p.priority}</td><td>${p.start}</td><td>${p.completion}</td><td>${p.turnaround}</td><td>${p.waiting}</td><td>${p.response}</td></tr>`).join('') + '</table></div>';
}
