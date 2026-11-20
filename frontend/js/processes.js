/* Process Management page: CRUD against the C backend (linked list + hash table). */
const PM = { editing: null, list: [] };

function validateProcess(pid, a, b, p, existing) {
  if (!/^[A-Za-z0-9_-]{1,15}$/.test(pid)) return 'PID must be 1-15 characters: letters, digits, _ or -';
  if (existing && existing.includes(pid)) return `PID "${pid}" already exists`;
  if (!Number.isInteger(a) || a < 0 || a > 10000) return 'Arrival time must be a whole number 0-10000';
  if (!Number.isInteger(b) || b < 1 || b > 10000) return 'Burst time must be a whole number 1-10000';
  if (!Number.isInteger(p) || p < 0 || p > 999) return 'Priority must be a whole number 0-999';
  return null;
}

async function pmLoad() {
  const q = $('#pm-search').value.trim();
  const d = await api('GET', '/api/processes' + (q ? '?q=' + encodeURIComponent(q) : ''));
  PM.list = d.processes; PM.total = d.total;
  $('#pm-table').innerHTML = `<tr><th>PID</th><th>Arrival</th><th>Burst</th><th>Priority</th><th></th></tr>` +
    (d.processes.map(p => `<tr><td><b>${esc(p.pid)}</b></td><td>${p.arrival}</td><td>${p.burst}</td><td>${p.priority}</td>
      <td><button class="mini ghost" data-edit="${esc(p.pid)}">Edit</button> <button class="mini danger" data-del="${esc(p.pid)}">Delete</button></td></tr>`).join('') ||
      `<tr><td colspan="5" class="null">No processes${q ? ' match the search' : ' yet - add one above or load the sample dataset'}.</td></tr>`);
  document.dispatchEvent(new CustomEvent('store-changed', { detail: d }));
}

function pmReset() {
  PM.editing = null;
  $('#pm-title').textContent = 'Add process'; $('#pm-save').textContent = 'Add';
  $('#pm-pid').disabled = false; $('#pm-cancel').hidden = true; $('#pm-pid').value = '';
}

async function pmSave() {
  const pid = $('#pm-pid').value.trim(), a = Number($('#pm-arrival').value), b = Number($('#pm-burst').value), p = Number($('#pm-priority').value);
  const err = validateProcess(pid, a, b, p, PM.editing ? null : PM.list.map(x => x.pid));
  if (err) return toast(err, true);
  const body = { arrival: a, burst: b, priority: p };
  try {
    if (PM.editing) await api('PUT', '/api/processes/' + encodeURIComponent(PM.editing), body);
    else await api('POST', '/api/processes', { pid, ...body });
    toast(PM.editing ? 'Process updated' : 'Process added');
    pmReset(); await pmLoad();
    $('#pm-pid').value = ''; 
  } catch (e) { toast(e.message, true); }
}

function initProcesses() {
  $('#pm-save').onclick = pmSave;
  $('#pm-cancel').onclick = pmReset;
  $('#pm-search').oninput = () => pmLoad().catch(e => toast(e.message, true));
  $('#pm-sample').onclick = async () => { try { await api('POST', '/api/sample', { deps: 0 }); pmReset(); await pmLoad(); toast('Sample dataset loaded'); } catch (e) { toast(e.message, true); } };
  $('#pm-clear').onclick = async () => { if (!confirm('Delete all processes?')) return; try { await api('POST', '/api/reset', {}); await pmLoad(); } catch (e) { toast(e.message, true); } };
  $('#pm-table').onclick = async ev => {
    const del = ev.target.dataset.del, ed = ev.target.dataset.edit;
    try {
      if (del) { await api('DELETE', '/api/processes/' + encodeURIComponent(del)); toast('Deleted ' + del); await pmLoad(); }
      if (ed) {
        const p = PM.list.find(x => x.pid === ed);
        PM.editing = ed; $('#pm-title').textContent = 'Edit process ' + ed; $('#pm-save').textContent = 'Save changes';
        $('#pm-pid').value = p.pid; $('#pm-pid').disabled = true; $('#pm-cancel').hidden = false;
        $('#pm-arrival').value = p.arrival; $('#pm-burst').value = p.burst; $('#pm-priority').value = p.priority;
        window.scrollTo({ top: 0, behavior: 'smooth' });
      }
    } catch (e) { toast(e.message, true); }
  };
}
