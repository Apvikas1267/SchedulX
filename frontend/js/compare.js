/* Algorithm comparison: same dataset, all six algorithms, numbers from the C scheduler. */
function barGroup(title, rows, key, higherBetter, unit) {
  const vals = rows.map(r => r[key]), best = higherBetter ? Math.max(...vals) : Math.min(...vals), max = Math.max(...vals) || 1;
  return `<div class="card"><h3>${title}</h3><div class="bars">` + rows.map(r =>
    `<div class="bar-row ${r[key] === best ? 'best' : ''}"><span class="nm">${esc(r.short)}</span><div class="b" style="width:${Math.max(1, r[key] / max * 60)}%"></div><span class="val">${fmt(r[key], key === 'throughput' ? 4 : 2)}${unit}</span></div>`).join('') +
    '</div><p class="hint">green = best</p></div>';
}

async function cmpRun() {
  const out = $('#cmp-out');
  try {
    const r = await api('POST', '/api/compare', { quantum: $('#cmp-q').value, prio: $('#cmp-prio').value });
    const short = { fcfs: 'FCFS', sjf: 'SJF', srtf: 'SRTF', rr: 'Round Robin (q=' + r.quantum + ')', prio_np: 'Priority (non-pre.)', prio_p: 'Priority (preemptive)' };
    const rows = r.results.map(x => ({ ...x, short: short[x.algo] }));
    out.innerHTML = `<div class="card"><h3>Comparison table (${r.count} processes)</h3><div class="scroll-x"><table class="tbl"><tr><th>Algorithm</th><th>Avg waiting</th><th>Avg turnaround</th><th>Avg response</th><th>CPU util %</th><th>Throughput</th><th>Total time</th></tr>` +
      rows.map(x => `<tr><td><b>${esc(x.short)}</b></td><td>${fmt(x.avgWait)}</td><td>${fmt(x.avgTat)}</td><td>${fmt(x.avgResp)}</td><td>${fmt(x.cpuUtil)}</td><td>${fmt(x.throughput, 4)}</td><td>${x.totalTime}</td></tr>`).join('') + '</table></div></div>' +
      '<div class="two-col">' + barGroup('Average waiting time (lower is better)', rows, 'avgWait', false, '') + barGroup('Average turnaround time (lower is better)', rows, 'avgTat', false, '') +
      barGroup('CPU utilisation (higher is better)', rows, 'cpuUtil', true, '%') + barGroup('Throughput (higher is better)', rows, 'throughput', true, '') + '</div>';
  } catch (e) { out.innerHTML = `<div class="err">${esc(e.message)}</div>`; }
}
function initCompare() { $('#cmp-run').onclick = cmpRun; }
