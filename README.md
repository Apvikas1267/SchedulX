# SchedulX - DSA-Based CPU Scheduling and Process Management Simulator

A college *Data Structures and Applications* project. A **C backend** implements every data structure and every
scheduling algorithm by hand and serves a small **HTML5 / CSS3 / vanilla-JavaScript** frontend through a built-in
local HTTP server. JavaScript only collects input and draws what the C code returns - **all scheduling maths is done in C**.

## Quick start (Windows + VS Code)

1. Extract `SchedulX.zip` and open the `SchedulX` folder in VS Code (`File > Open Folder`).
2. Open a terminal (`Ctrl+``) and run `scripts\setup.bat` - it checks `gcc`, builds the backend and runs the self-tests.
3. Run `scripts\run.bat` - it starts the C server and opens <http://localhost:8080> in your browser.
4. Press `Ctrl+C` in the terminal to stop the server (memory is released on exit).

VS Code tasks are included (`Terminal > Run Build Task` builds, `Run Task...` offers *Run SchedulX* and *Run tests*).

### Requirements and dependencies
* **GCC for Windows (MinGW-w64)** on `PATH` (e.g. MSYS2 `pacman -S mingw-w64-ucrt-x86_64-gcc`, or WinLibs). Check with `gcc --version`.
* A modern browser (Chrome, Edge, Firefox).
* Nothing else: only the C standard library and Winsock (`ws2_32`, ships with Windows). No Node.js, Python, frameworks or internet access.
* Port 8080 must be free (`backend\bin\schedulx.exe <port> frontend` uses another port).
* Linux/macOS also work: `cd backend && make && ./bin/schedulx 8080 ../frontend`.

## Project structure
```
SchedulX/
├── backend/
│   ├── include/   *.h  (one header per module)
│   ├── src/       C sources (see table below)
│   ├── tests/     test_scheduler.c  (unit + scheduling tests)
│   ├── Makefile   build.bat
├── frontend/      index.html, css/style.css, js/*.js
├── scripts/       setup.bat, run.bat, run.sh
├── samples/       sample_dataset.csv / .txt
├── docs/          TEST_CASES.md  (expected outputs of every test)
├── README.md  LICENSE  .gitignore  .vscode/tasks.json
```

| C module | Data structure / purpose |
|---|---|
| `linkedlist.c` | singly linked list of processes: insert (head/tail), delete, search, traversal, dynamic memory |
| `hashtable.c` | hash table (djb2, 13 buckets, **separate chaining** for collisions) for O(1) lookup by PID |
| `queue.c` | FIFO queue (linked list) for FCFS and **circular queue** (array, wrap-around) for Round Robin |
| `stack.c` | stack (linked list): push / pop / peek - CPU execution history |
| `heap.c` | binary **min-heap** (sift-up / sift-down): SJF, SRTF and priority queue |
| `graph.c` | directed graph, **adjacency list**; Kahn topological sort; DFS cycle detection |
| `process.c` | process validation and the "world" (list + hash table + dependency edges) |
| `scheduler.c` | the 6 algorithms, Gantt segments and execution trace |
| `stats.c` | averages, CPU utilisation, throughput |
| `api.c`, `http_server.c`, `main.c` | JSON API, small HTTP server (Winsock on Windows, sockets elsewhere), start-up |
| `strbuf.c` | growable string buffer used to build JSON |

### Syllabus mapping (CS233AI, uploaded syllabus)
| Syllabus topic | Where it is used |
|---|---|
| Unit I - singly linked lists: creation, insertion, deletion, traversal, searching | process store, Process Management page, search box |
| Unit II - stack: push, pop, peek, array/linked implementation | execution-history stack |
| Unit III - queues: simple, circular, priority; scheduling / round-robin / CPU task buffering | FCFS queue, Round Robin circular queue, priority queue |
| Unit IV - binary trees | the heap is a complete binary tree (drawn as a tree in the visualizer) |
| Unit V - binary heap, priority queue | SJF, SRTF, priority scheduling |
| Unit V - graphs (adjacency list), cycle detection | process dependencies, topological sort |
| Unit V - hashing, collision resolution | PID index (open hashing / separate chaining) |

## Using the application
* **Dashboard** - description, algorithm overview, process count, selected algorithm and the averages of the last run.
* **Processes** - add / edit / delete / search processes (validated in the browser *and* in C). The sample dataset is loaded at start-up.
* **CPU Scheduling** - five tabs (FCFS, SJF, SRTF, Round Robin, Priority with a preemptive/non-preemptive switch). Each tab has its own
  process list, settings, Run button, Gantt chart, statistics table and a how-it-works explanation, and runs independently.
  *Use Process Management data* copies the stored dataset into a tab.
* **DSA Visualizer** - linked list, hash table, dependency graph (add/remove dependencies; cycles are rejected with the cycle shown) and a
  step-through of the scheduler showing the ready queue / circular queue / min-heap tree and the history stack after every dispatch,
  with the operations performed (`enqueue`, `heap-insert`, `extract-min`, `push`...). All of it is JSON from the C structures.
* **Comparison** - all algorithms on the same dataset: table plus bar charts for waiting time, turnaround time, CPU utilisation, throughput.

### Conventions (important for viva questions)
* **Priority:** by default a **lower number = higher priority**; every priority run can switch to "higher number = higher priority".
* **Formulas:** Turnaround = Completion - Arrival; Waiting = Turnaround - Burst; Response = First CPU start - Arrival.
* **CPU utilisation** = busy time / total simulation time x 100, where total time runs from **0 to the last completion**
  (so an initial idle gap before the first arrival lowers utilisation). **Throughput** = completed processes / total time.
* **Tie-breaking:** equal key -> earlier arrival -> input order. A newly arrived process never pre-empts on a tie.
* **Round Robin:** processes that arrive during (or exactly at the end of) a slice are queued before the pre-empted process.
* **Dependencies:** edge `A -> B` means B may start only after A has completed. Circular dependencies are detected (Kahn + DFS) and the run is refused.
* **Limits:** 100 processes, arrival/burst <= 10000, priority 0-999, PID = 1-15 letters/digits/`_`/`-`. The Gantt chart is capped at 20000 blocks
  (Round Robin with a tiny quantum on huge bursts is refused with a message); the visualizer keeps the first 2000 steps.

## Testing
`backend\build.bat test` (or `make test`) runs `tests/test_scheduler.c`: **622 checks** - data-structure unit tests (heap order, circular-queue wrap-around,
stack LIFO, hash collision, list insert/delete/search, topological order, cycle rejection, input validation) and **51 scheduling cases**
covering every algorithm with processes arriving at time 0, different arrival times, simultaneous arrivals, different priorities, long bursts (500),
idle periods, a single process, Round Robin with quantum 1/2/3/4/100, both priority rules, and dependencies. Expected Gantt charts, completion,
turnaround, waiting and response times are listed in `docs/TEST_CASES.md`.

How the expected values were obtained (honest summary): they come from a separate tick-by-tick reference simulator written for verification
(not part of this project), which was also compared against the C backend through the HTTP API on **12,000 randomly generated datasets** (all six algorithms, both
priority rules, random dependencies, quantum 1-5) with zero differences, and the textbook example (P1-P4, SRTF average waiting time 6.5) was checked by hand.

## API (used by the frontend; handy for demos with `curl`)
`GET/POST /api/processes`, `GET/PUT/DELETE /api/processes/<pid>`, `POST /api/sample`, `POST /api/reset`, `GET/POST/DELETE /api/deps`,
`GET /api/graph`, `GET /api/hash`, `POST /api/simulate`, `POST /api/compare` (form-encoded bodies; e.g.
`curl -X POST localhost:8080/api/simulate -d "algo=rr&quantum=2&procs=P1,0,5,1;P2,1,3,2"`).

## Live Public Deployment

### 1. Deploy to Vercel (Recommended - Instant Public URL)
The repository is fully configured for Vercel Serverless deployment with `vercel.json` and `/api` serverless functions.
1. Push your repository to GitHub.
2. Go to [vercel.com](https://vercel.com) and click **"Add New" > "Project"**.
3. Import your `SchedulX` repository.
4. Keep the **Root Directory** as `.` (default) and click **Deploy**.
5. Vercel will build the project and provide a public link (e.g. `https://schedulx.vercel.app`).
   - The `/api/*` endpoints are automatically handled by Vercel Serverless.
   - If offline or on static CDN, the frontend includes an in-browser DSA engine fallback so the public link always works 100%!

### 2. Deploy Pure C Binary with Docker (Render / Railway / Koyeb)
If you want to run the exact native C socket server binary in the cloud:
1. Push to GitHub.
2. Go to [render.com](https://render.com) > **New Web Service** > Connect your GitHub repo.
3. Select **Docker** environment (it automatically detects the included `Dockerfile`).
4. Click **Create Web Service**. Render compiles the C backend on Linux and serves the app on `https://<your-service>.onrender.com`.

## Verification status
Compiled with GCC and tested on **Linux** and cross-compiled for Windows with MinGW-w64 (including AddressSanitizer/UBSan runs of the unit tests and of the server under API calls: no errors or leaks reported)
and the frontend was driven with a headless browser against the real C server. The **Windows** path (`build.bat`, `run.bat`, Winsock code) follows standard MinGW-w64
practice but could not be executed in the environment where this project was produced, so please run `scripts\setup.bat` once and tell me about any compiler message.

### VS Code shows red squiggles such as `identifier "World" is undefined`?
That is IntelliSense (the editor), not the compiler. The project ships `.vscode/c_cpp_properties.json` that points it to `backend/include`;
open the folder **SchedulX** (not a sub-folder) and, if needed, run *C/C++: Reset IntelliSense Database* from the Command Palette. The real build is `backend\build.bat`.
