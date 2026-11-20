# Scheduling test cases (expected outputs)
All values below were checked by an independent tick-by-tick reference simulator and are asserted by `tests/test_scheduler.c`.
Gantt notation: `PID[start-end]`. Priority rule: lower number = higher priority unless noted. CPU utilisation = busy time / makespan (time 0 to last completion).

### textbook / fcfs
Processes (PID, arrival, burst, priority): P1(0,8,3), P2(1,4,1), P3(2,9,4), P4(3,5,2)

- Gantt: P1[0-8] P2[8-12] P3[12-21] P4[21-26]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 8 | 8 | 0 | 0 |
| P2 | 12 | 11 | 7 | 7 |
| P3 | 21 | 19 | 10 | 10 |
| P4 | 26 | 23 | 18 | 18 |

- Avg waiting 8.75, avg turnaround 15.25, CPU utilisation 100.00%, makespan 26

### textbook / sjf
Processes (PID, arrival, burst, priority): P1(0,8,3), P2(1,4,1), P3(2,9,4), P4(3,5,2)

- Gantt: P1[0-8] P2[8-12] P4[12-17] P3[17-26]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 8 | 8 | 0 | 0 |
| P2 | 12 | 11 | 7 | 7 |
| P3 | 26 | 24 | 15 | 15 |
| P4 | 17 | 14 | 9 | 9 |

- Avg waiting 7.75, avg turnaround 14.25, CPU utilisation 100.00%, makespan 26

### textbook / srtf
Processes (PID, arrival, burst, priority): P1(0,8,3), P2(1,4,1), P3(2,9,4), P4(3,5,2)

- Gantt: P1[0-1] P2[1-5] P4[5-10] P1[10-17] P3[17-26]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 17 | 17 | 9 | 0 |
| P2 | 5 | 4 | 0 | 0 |
| P3 | 26 | 24 | 15 | 15 |
| P4 | 10 | 7 | 2 | 2 |

- Avg waiting 6.50, avg turnaround 13.00, CPU utilisation 100.00%, makespan 26

### textbook / prio_np
Processes (PID, arrival, burst, priority): P1(0,8,3), P2(1,4,1), P3(2,9,4), P4(3,5,2)

- Gantt: P1[0-8] P2[8-12] P4[12-17] P3[17-26]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 8 | 8 | 0 | 0 |
| P2 | 12 | 11 | 7 | 7 |
| P3 | 26 | 24 | 15 | 15 |
| P4 | 17 | 14 | 9 | 9 |

- Avg waiting 7.75, avg turnaround 14.25, CPU utilisation 100.00%, makespan 26

### textbook / prio_p
Processes (PID, arrival, burst, priority): P1(0,8,3), P2(1,4,1), P3(2,9,4), P4(3,5,2)

- Gantt: P1[0-1] P2[1-5] P4[5-10] P1[10-17] P3[17-26]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 17 | 17 | 9 | 0 |
| P2 | 5 | 4 | 0 | 0 |
| P3 | 26 | 24 | 15 | 15 |
| P4 | 10 | 7 | 2 | 2 |

- Avg waiting 6.50, avg turnaround 13.00, CPU utilisation 100.00%, makespan 26

### textbook / rr q=2
Processes (PID, arrival, burst, priority): P1(0,8,3), P2(1,4,1), P3(2,9,4), P4(3,5,2)

- Gantt: P1[0-2] P2[2-4] P3[4-6] P1[6-8] P4[8-10] P2[10-12] P3[12-14] P1[14-16] P4[16-18] P3[18-20] P1[20-22] P4[22-23] P3[23-25] P3[25-26]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 22 | 22 | 14 | 0 |
| P2 | 12 | 11 | 7 | 1 |
| P3 | 26 | 24 | 15 | 2 |
| P4 | 23 | 20 | 15 | 5 |

- Avg waiting 12.75, avg turnaround 19.25, CPU utilisation 100.00%, makespan 26

### all_at_zero / fcfs
Processes (PID, arrival, burst, priority): P1(0,6,2), P2(0,8,1), P3(0,7,3), P4(0,3,2)

- Gantt: P1[0-6] P2[6-14] P3[14-21] P4[21-24]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 6 | 6 | 0 | 0 |
| P2 | 14 | 14 | 6 | 6 |
| P3 | 21 | 21 | 14 | 14 |
| P4 | 24 | 24 | 21 | 21 |

- Avg waiting 10.25, avg turnaround 16.25, CPU utilisation 100.00%, makespan 24

### all_at_zero / sjf
Processes (PID, arrival, burst, priority): P1(0,6,2), P2(0,8,1), P3(0,7,3), P4(0,3,2)

- Gantt: P4[0-3] P1[3-9] P3[9-16] P2[16-24]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 9 | 9 | 3 | 3 |
| P2 | 24 | 24 | 16 | 16 |
| P3 | 16 | 16 | 9 | 9 |
| P4 | 3 | 3 | 0 | 0 |

- Avg waiting 7.00, avg turnaround 13.00, CPU utilisation 100.00%, makespan 24

### all_at_zero / srtf
Processes (PID, arrival, burst, priority): P1(0,6,2), P2(0,8,1), P3(0,7,3), P4(0,3,2)

- Gantt: P4[0-3] P1[3-9] P3[9-16] P2[16-24]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 9 | 9 | 3 | 3 |
| P2 | 24 | 24 | 16 | 16 |
| P3 | 16 | 16 | 9 | 9 |
| P4 | 3 | 3 | 0 | 0 |

- Avg waiting 7.00, avg turnaround 13.00, CPU utilisation 100.00%, makespan 24

### all_at_zero / prio_np
Processes (PID, arrival, burst, priority): P1(0,6,2), P2(0,8,1), P3(0,7,3), P4(0,3,2)

- Gantt: P2[0-8] P1[8-14] P4[14-17] P3[17-24]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 14 | 14 | 8 | 8 |
| P2 | 8 | 8 | 0 | 0 |
| P3 | 24 | 24 | 17 | 17 |
| P4 | 17 | 17 | 14 | 14 |

- Avg waiting 9.75, avg turnaround 15.75, CPU utilisation 100.00%, makespan 24

### all_at_zero / prio_p
Processes (PID, arrival, burst, priority): P1(0,6,2), P2(0,8,1), P3(0,7,3), P4(0,3,2)

- Gantt: P2[0-8] P1[8-14] P4[14-17] P3[17-24]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 14 | 14 | 8 | 8 |
| P2 | 8 | 8 | 0 | 0 |
| P3 | 24 | 24 | 17 | 17 |
| P4 | 17 | 17 | 14 | 14 |

- Avg waiting 9.75, avg turnaround 15.75, CPU utilisation 100.00%, makespan 24

### all_at_zero / rr q=2
Processes (PID, arrival, burst, priority): P1(0,6,2), P2(0,8,1), P3(0,7,3), P4(0,3,2)

- Gantt: P1[0-2] P2[2-4] P3[4-6] P4[6-8] P1[8-10] P2[10-12] P3[12-14] P4[14-15] P1[15-17] P2[17-19] P3[19-21] P2[21-23] P3[23-24]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 17 | 17 | 11 | 0 |
| P2 | 23 | 23 | 15 | 2 |
| P3 | 24 | 24 | 17 | 4 |
| P4 | 15 | 15 | 12 | 6 |

- Avg waiting 13.75, avg turnaround 19.75, CPU utilisation 100.00%, makespan 24

### simultaneous / fcfs
Processes (PID, arrival, burst, priority): A(2,3,2), B(2,3,1), C(2,1,1), D(2,3,1)

- Gantt: IDLE[0-2] A[2-5] B[5-8] C[8-9] D[9-12]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| A | 5 | 3 | 0 | 0 |
| B | 8 | 6 | 3 | 3 |
| C | 9 | 7 | 6 | 6 |
| D | 12 | 10 | 7 | 7 |

- Avg waiting 4.00, avg turnaround 6.50, CPU utilisation 83.33%, makespan 12

### simultaneous / sjf
Processes (PID, arrival, burst, priority): A(2,3,2), B(2,3,1), C(2,1,1), D(2,3,1)

- Gantt: IDLE[0-2] C[2-3] A[3-6] B[6-9] D[9-12]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| A | 6 | 4 | 1 | 1 |
| B | 9 | 7 | 4 | 4 |
| C | 3 | 1 | 0 | 0 |
| D | 12 | 10 | 7 | 7 |

- Avg waiting 3.00, avg turnaround 5.50, CPU utilisation 83.33%, makespan 12

### simultaneous / srtf
Processes (PID, arrival, burst, priority): A(2,3,2), B(2,3,1), C(2,1,1), D(2,3,1)

- Gantt: IDLE[0-2] C[2-3] A[3-6] B[6-9] D[9-12]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| A | 6 | 4 | 1 | 1 |
| B | 9 | 7 | 4 | 4 |
| C | 3 | 1 | 0 | 0 |
| D | 12 | 10 | 7 | 7 |

- Avg waiting 3.00, avg turnaround 5.50, CPU utilisation 83.33%, makespan 12

### simultaneous / prio_np
Processes (PID, arrival, burst, priority): A(2,3,2), B(2,3,1), C(2,1,1), D(2,3,1)

- Gantt: IDLE[0-2] B[2-5] C[5-6] D[6-9] A[9-12]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| A | 12 | 10 | 7 | 7 |
| B | 5 | 3 | 0 | 0 |
| C | 6 | 4 | 3 | 3 |
| D | 9 | 7 | 4 | 4 |

- Avg waiting 3.50, avg turnaround 6.00, CPU utilisation 83.33%, makespan 12

### simultaneous / prio_p
Processes (PID, arrival, burst, priority): A(2,3,2), B(2,3,1), C(2,1,1), D(2,3,1)

- Gantt: IDLE[0-2] B[2-5] C[5-6] D[6-9] A[9-12]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| A | 12 | 10 | 7 | 7 |
| B | 5 | 3 | 0 | 0 |
| C | 6 | 4 | 3 | 3 |
| D | 9 | 7 | 4 | 4 |

- Avg waiting 3.50, avg turnaround 6.00, CPU utilisation 83.33%, makespan 12

### simultaneous / rr q=2
Processes (PID, arrival, burst, priority): A(2,3,2), B(2,3,1), C(2,1,1), D(2,3,1)

- Gantt: IDLE[0-2] A[2-4] B[4-6] C[6-7] D[7-9] A[9-10] B[10-11] D[11-12]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| A | 10 | 8 | 5 | 0 |
| B | 11 | 9 | 6 | 2 |
| C | 7 | 5 | 4 | 4 |
| D | 12 | 10 | 7 | 5 |

- Avg waiting 5.50, avg turnaround 8.00, CPU utilisation 83.33%, makespan 12

### idle_gaps / fcfs
Processes (PID, arrival, burst, priority): P1(2,2,1), P2(8,3,2), P3(9,1,1)

- Gantt: IDLE[0-2] P1[2-4] IDLE[4-8] P2[8-11] P3[11-12]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 4 | 2 | 0 | 0 |
| P2 | 11 | 3 | 0 | 0 |
| P3 | 12 | 3 | 2 | 2 |

- Avg waiting 0.67, avg turnaround 2.67, CPU utilisation 50.00%, makespan 12

### idle_gaps / sjf
Processes (PID, arrival, burst, priority): P1(2,2,1), P2(8,3,2), P3(9,1,1)

- Gantt: IDLE[0-2] P1[2-4] IDLE[4-8] P2[8-11] P3[11-12]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 4 | 2 | 0 | 0 |
| P2 | 11 | 3 | 0 | 0 |
| P3 | 12 | 3 | 2 | 2 |

- Avg waiting 0.67, avg turnaround 2.67, CPU utilisation 50.00%, makespan 12

### idle_gaps / srtf
Processes (PID, arrival, burst, priority): P1(2,2,1), P2(8,3,2), P3(9,1,1)

- Gantt: IDLE[0-2] P1[2-4] IDLE[4-8] P2[8-9] P3[9-10] P2[10-12]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 4 | 2 | 0 | 0 |
| P2 | 12 | 4 | 1 | 0 |
| P3 | 10 | 1 | 0 | 0 |

- Avg waiting 0.33, avg turnaround 2.33, CPU utilisation 50.00%, makespan 12

### idle_gaps / prio_np
Processes (PID, arrival, burst, priority): P1(2,2,1), P2(8,3,2), P3(9,1,1)

- Gantt: IDLE[0-2] P1[2-4] IDLE[4-8] P2[8-11] P3[11-12]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 4 | 2 | 0 | 0 |
| P2 | 11 | 3 | 0 | 0 |
| P3 | 12 | 3 | 2 | 2 |

- Avg waiting 0.67, avg turnaround 2.67, CPU utilisation 50.00%, makespan 12

### idle_gaps / prio_p
Processes (PID, arrival, burst, priority): P1(2,2,1), P2(8,3,2), P3(9,1,1)

- Gantt: IDLE[0-2] P1[2-4] IDLE[4-8] P2[8-9] P3[9-10] P2[10-12]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 4 | 2 | 0 | 0 |
| P2 | 12 | 4 | 1 | 0 |
| P3 | 10 | 1 | 0 | 0 |

- Avg waiting 0.33, avg turnaround 2.33, CPU utilisation 50.00%, makespan 12

### idle_gaps / rr q=2
Processes (PID, arrival, burst, priority): P1(2,2,1), P2(8,3,2), P3(9,1,1)

- Gantt: IDLE[0-2] P1[2-4] IDLE[4-8] P2[8-10] P3[10-11] P2[11-12]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 4 | 2 | 0 | 0 |
| P2 | 12 | 4 | 1 | 0 |
| P3 | 11 | 2 | 1 | 1 |

- Avg waiting 0.67, avg turnaround 2.67, CPU utilisation 50.00%, makespan 12

### single / fcfs
Processes (PID, arrival, burst, priority): P1(3,5,1)

- Gantt: IDLE[0-3] P1[3-8]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 8 | 5 | 0 | 0 |

- Avg waiting 0.00, avg turnaround 5.00, CPU utilisation 62.50%, makespan 8

### single / sjf
Processes (PID, arrival, burst, priority): P1(3,5,1)

- Gantt: IDLE[0-3] P1[3-8]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 8 | 5 | 0 | 0 |

- Avg waiting 0.00, avg turnaround 5.00, CPU utilisation 62.50%, makespan 8

### single / srtf
Processes (PID, arrival, burst, priority): P1(3,5,1)

- Gantt: IDLE[0-3] P1[3-8]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 8 | 5 | 0 | 0 |

- Avg waiting 0.00, avg turnaround 5.00, CPU utilisation 62.50%, makespan 8

### single / prio_np
Processes (PID, arrival, burst, priority): P1(3,5,1)

- Gantt: IDLE[0-3] P1[3-8]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 8 | 5 | 0 | 0 |

- Avg waiting 0.00, avg turnaround 5.00, CPU utilisation 62.50%, makespan 8

### single / prio_p
Processes (PID, arrival, burst, priority): P1(3,5,1)

- Gantt: IDLE[0-3] P1[3-8]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 8 | 5 | 0 | 0 |

- Avg waiting 0.00, avg turnaround 5.00, CPU utilisation 62.50%, makespan 8

### single / rr q=2
Processes (PID, arrival, burst, priority): P1(3,5,1)

- Gantt: IDLE[0-3] P1[3-5] P1[5-7] P1[7-8]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 8 | 5 | 0 | 0 |

- Avg waiting 0.00, avg turnaround 5.00, CPU utilisation 62.50%, makespan 8

### long_burst / fcfs
Processes (PID, arrival, burst, priority): P1(0,500,2), P2(10,20,1), P3(20,3,3)

- Gantt: P1[0-500] P2[500-520] P3[520-523]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 500 | 500 | 0 | 0 |
| P2 | 520 | 510 | 490 | 490 |
| P3 | 523 | 503 | 500 | 500 |

- Avg waiting 330.00, avg turnaround 504.33, CPU utilisation 100.00%, makespan 523

### long_burst / sjf
Processes (PID, arrival, burst, priority): P1(0,500,2), P2(10,20,1), P3(20,3,3)

- Gantt: P1[0-500] P3[500-503] P2[503-523]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 500 | 500 | 0 | 0 |
| P2 | 523 | 513 | 493 | 493 |
| P3 | 503 | 483 | 480 | 480 |

- Avg waiting 324.33, avg turnaround 498.67, CPU utilisation 100.00%, makespan 523

### long_burst / srtf
Processes (PID, arrival, burst, priority): P1(0,500,2), P2(10,20,1), P3(20,3,3)

- Gantt: P1[0-10] P2[10-20] P3[20-23] P2[23-33] P1[33-523]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 523 | 523 | 23 | 0 |
| P2 | 33 | 23 | 3 | 0 |
| P3 | 23 | 3 | 0 | 0 |

- Avg waiting 8.67, avg turnaround 183.00, CPU utilisation 100.00%, makespan 523

### long_burst / prio_np
Processes (PID, arrival, burst, priority): P1(0,500,2), P2(10,20,1), P3(20,3,3)

- Gantt: P1[0-500] P2[500-520] P3[520-523]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 500 | 500 | 0 | 0 |
| P2 | 520 | 510 | 490 | 490 |
| P3 | 523 | 503 | 500 | 500 |

- Avg waiting 330.00, avg turnaround 504.33, CPU utilisation 100.00%, makespan 523

### long_burst / prio_p
Processes (PID, arrival, burst, priority): P1(0,500,2), P2(10,20,1), P3(20,3,3)

- Gantt: P1[0-10] P2[10-30] P1[30-520] P3[520-523]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 520 | 520 | 20 | 0 |
| P2 | 30 | 20 | 0 | 0 |
| P3 | 523 | 503 | 500 | 500 |

- Avg waiting 173.33, avg turnaround 347.67, CPU utilisation 100.00%, makespan 523

### long_burst / rr q=2
Processes (PID, arrival, burst, priority): P1(0,500,2), P2(10,20,1), P3(20,3,3)

- Gantt: P1[0-2] P1[2-4] P1[4-6] P1[6-8] P1[8-10] P2[10-12] P1[12-14] P2[14-16] P1[16-18] P2[18-20] P1[20-22] P3[22-24] P2[24-26] P1[26-28] P3[28-29] P2[29-31] P1[31-33] P2[33-35] P1[35-37] P2[37-39] P1[39-41] P2[41-43] P1[43-45] P2[45-47] P1[47-49] P2[49-51] P1[51-53] P1[53-55] P1[55-57] P1[57-59] P1[59-61] P1[61-63] P1[63-65] P1[65-67] P1[67-69] P1[69-71] P1[71-73] P1[73-75] P1[75-77] P1[77-79] P1[79-81] P1[81-83] P1[83-85] P1[85-87] P1[87-89] P1[89-91] P1[91-93] P1[93-95] P1[95-97] P1[97-99] P1[99-101] P1[101-103] P1[103-105] P1[105-107] P1[107-109] P1[109-111] P1[111-113] P1[113-115] P1[115-117] P1[117-119] P1[119-121] P1[121-123] P1[123-125] P1[125-127] P1[127-129] P1[129-131] P1[131-133] P1[133-135] P1[135-137] P1[137-139] P1[139-141] P1[141-143] P1[143-145] P1[145-147] P1[147-149] P1[149-151] P1[151-153] P1[153-155] P1[155-157] P1[157-159] P1[159-161] P1[161-163] P1[163-165] P1[165-167] P1[167-169] P1[169-171] P1[171-173] P1[173-175] P1[175-177] P1[177-179] P1[179-181] P1[181-183] P1[183-185] P1[185-187] P1[187-189] P1[189-191] P1[191-193] P1[193-195] P1[195-197] P1[197-199] P1[199-201] P1[201-203] P1[203-205] P1[205-207] P1[207-209] P1[209-211] P1[211-213] P1[213-215] P1[215-217] P1[217-219] P1[219-221] P1[221-223] P1[223-225] P1[225-227] P1[227-229] P1[229-231] P1[231-233] P1[233-235] P1[235-237] P1[237-239] P1[239-241] P1[241-243] P1[243-245] P1[245-247] P1[247-249] P1[249-251] P1[251-253] P1[253-255] P1[255-257] P1[257-259] P1[259-261] P1[261-263] P1[263-265] P1[265-267] P1[267-269] P1[269-271] P1[271-273] P1[273-275] P1[275-277] P1[277-279] P1[279-281] P1[281-283] P1[283-285] P1[285-287] P1[287-289] P1[289-291] P1[291-293] P1[293-295] P1[295-297] P1[297-299] P1[299-301] P1[301-303] P1[303-305] P1[305-307] P1[307-309] P1[309-311] P1[311-313] P1[313-315] P1[315-317] P1[317-319] P1[319-321] P1[321-323] P1[323-325] P1[325-327] P1[327-329] P1[329-331] P1[331-333] P1[333-335] P1[335-337] P1[337-339] P1[339-341] P1[341-343] P1[343-345] P1[345-347] P1[347-349] P1[349-351] P1[351-353] P1[353-355] P1[355-357] P1[357-359] P1[359-361] P1[361-363] P1[363-365] P1[365-367] P1[367-369] P1[369-371] P1[371-373] P1[373-375] P1[375-377] P1[377-379] P1[379-381] P1[381-383] P1[383-385] P1[385-387] P1[387-389] P1[389-391] P1[391-393] P1[393-395] P1[395-397] P1[397-399] P1[399-401] P1[401-403] P1[403-405] P1[405-407] P1[407-409] P1[409-411] P1[411-413] P1[413-415] P1[415-417] P1[417-419] P1[419-421] P1[421-423] P1[423-425] P1[425-427] P1[427-429] P1[429-431] P1[431-433] P1[433-435] P1[435-437] P1[437-439] P1[439-441] P1[441-443] P1[443-445] P1[445-447] P1[447-449] P1[449-451] P1[451-453] P1[453-455] P1[455-457] P1[457-459] P1[459-461] P1[461-463] P1[463-465] P1[465-467] P1[467-469] P1[469-471] P1[471-473] P1[473-475] P1[475-477] P1[477-479] P1[479-481] P1[481-483] P1[483-485] P1[485-487] P1[487-489] P1[489-491] P1[491-493] P1[493-495] P1[495-497] P1[497-499] P1[499-501] P1[501-503] P1[503-505] P1[505-507] P1[507-509] P1[509-511] P1[511-513] P1[513-515] P1[515-517] P1[517-519] P1[519-521] P1[521-523]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 523 | 523 | 23 | 0 |
| P2 | 51 | 41 | 21 | 0 |
| P3 | 29 | 9 | 6 | 2 |

- Avg waiting 16.67, avg turnaround 191.00, CPU utilisation 100.00%, makespan 523

### with_deps / fcfs
Processes (PID, arrival, burst, priority): P1(0,3,2), P2(0,2,1), P3(0,4,3), P4(1,2,1); dependencies: P1→P3, P2→P4

- Gantt: P1[0-3] P2[3-5] P3[5-9] P4[9-11]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 3 | 3 | 0 | 0 |
| P2 | 5 | 5 | 3 | 3 |
| P3 | 9 | 9 | 5 | 5 |
| P4 | 11 | 10 | 8 | 8 |

- Avg waiting 4.00, avg turnaround 6.75, CPU utilisation 100.00%, makespan 11

### with_deps / sjf
Processes (PID, arrival, burst, priority): P1(0,3,2), P2(0,2,1), P3(0,4,3), P4(1,2,1); dependencies: P1→P3, P2→P4

- Gantt: P2[0-2] P4[2-4] P1[4-7] P3[7-11]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 7 | 7 | 4 | 4 |
| P2 | 2 | 2 | 0 | 0 |
| P3 | 11 | 11 | 7 | 7 |
| P4 | 4 | 3 | 1 | 1 |

- Avg waiting 3.00, avg turnaround 5.75, CPU utilisation 100.00%, makespan 11

### with_deps / srtf
Processes (PID, arrival, burst, priority): P1(0,3,2), P2(0,2,1), P3(0,4,3), P4(1,2,1); dependencies: P1→P3, P2→P4

- Gantt: P2[0-2] P4[2-4] P1[4-7] P3[7-11]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 7 | 7 | 4 | 4 |
| P2 | 2 | 2 | 0 | 0 |
| P3 | 11 | 11 | 7 | 7 |
| P4 | 4 | 3 | 1 | 1 |

- Avg waiting 3.00, avg turnaround 5.75, CPU utilisation 100.00%, makespan 11

### with_deps / prio_np
Processes (PID, arrival, burst, priority): P1(0,3,2), P2(0,2,1), P3(0,4,3), P4(1,2,1); dependencies: P1→P3, P2→P4

- Gantt: P2[0-2] P4[2-4] P1[4-7] P3[7-11]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 7 | 7 | 4 | 4 |
| P2 | 2 | 2 | 0 | 0 |
| P3 | 11 | 11 | 7 | 7 |
| P4 | 4 | 3 | 1 | 1 |

- Avg waiting 3.00, avg turnaround 5.75, CPU utilisation 100.00%, makespan 11

### with_deps / prio_p
Processes (PID, arrival, burst, priority): P1(0,3,2), P2(0,2,1), P3(0,4,3), P4(1,2,1); dependencies: P1→P3, P2→P4

- Gantt: P2[0-2] P4[2-4] P1[4-7] P3[7-11]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 7 | 7 | 4 | 4 |
| P2 | 2 | 2 | 0 | 0 |
| P3 | 11 | 11 | 7 | 7 |
| P4 | 4 | 3 | 1 | 1 |

- Avg waiting 3.00, avg turnaround 5.75, CPU utilisation 100.00%, makespan 11

### with_deps / rr q=2
Processes (PID, arrival, burst, priority): P1(0,3,2), P2(0,2,1), P3(0,4,3), P4(1,2,1); dependencies: P1→P3, P2→P4

- Gantt: P1[0-2] P2[2-4] P1[4-5] P4[5-7] P3[7-9] P3[9-11]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 5 | 5 | 2 | 0 |
| P2 | 4 | 4 | 2 | 2 |
| P3 | 11 | 11 | 7 | 7 |
| P4 | 7 | 6 | 4 | 4 |

- Avg waiting 3.75, avg turnaround 6.50, CPU utilisation 100.00%, makespan 11

### textbook / rr q=1
Processes (PID, arrival, burst, priority): P1(0,8,3), P2(1,4,1), P3(2,9,4), P4(3,5,2)

- Gantt: P1[0-1] P2[1-2] P1[2-3] P3[3-4] P2[4-5] P4[5-6] P1[6-7] P3[7-8] P2[8-9] P4[9-10] P1[10-11] P3[11-12] P2[12-13] P4[13-14] P1[14-15] P3[15-16] P4[16-17] P1[17-18] P3[18-19] P4[19-20] P1[20-21] P3[21-22] P1[22-23] P3[23-24] P3[24-25] P3[25-26]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 23 | 23 | 15 | 0 |
| P2 | 13 | 12 | 8 | 0 |
| P3 | 26 | 24 | 15 | 1 |
| P4 | 20 | 17 | 12 | 2 |

- Avg waiting 12.50, avg turnaround 19.00, CPU utilisation 100.00%, makespan 26

### textbook / rr q=3
Processes (PID, arrival, burst, priority): P1(0,8,3), P2(1,4,1), P3(2,9,4), P4(3,5,2)

- Gantt: P1[0-3] P2[3-6] P3[6-9] P4[9-12] P1[12-15] P2[15-16] P3[16-19] P4[19-21] P1[21-23] P3[23-26]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 23 | 23 | 15 | 0 |
| P2 | 16 | 15 | 11 | 2 |
| P3 | 26 | 24 | 15 | 4 |
| P4 | 21 | 18 | 13 | 6 |

- Avg waiting 13.50, avg turnaround 20.00, CPU utilisation 100.00%, makespan 26

### textbook / rr q=4
Processes (PID, arrival, burst, priority): P1(0,8,3), P2(1,4,1), P3(2,9,4), P4(3,5,2)

- Gantt: P1[0-4] P2[4-8] P3[8-12] P4[12-16] P1[16-20] P3[20-24] P4[24-25] P3[25-26]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 20 | 20 | 12 | 0 |
| P2 | 8 | 7 | 3 | 3 |
| P3 | 26 | 24 | 15 | 6 |
| P4 | 25 | 22 | 17 | 9 |

- Avg waiting 11.75, avg turnaround 18.25, CPU utilisation 100.00%, makespan 26

### textbook / rr q=100
Processes (PID, arrival, burst, priority): P1(0,8,3), P2(1,4,1), P3(2,9,4), P4(3,5,2)

- Gantt: P1[0-8] P2[8-12] P3[12-21] P4[21-26]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 8 | 8 | 0 | 0 |
| P2 | 12 | 11 | 7 | 7 |
| P3 | 21 | 19 | 10 | 10 |
| P4 | 26 | 23 | 18 | 18 |

- Avg waiting 8.75, avg turnaround 15.25, CPU utilisation 100.00%, makespan 26

### textbook / prio_np high-number-first
Processes (PID, arrival, burst, priority): P1(0,8,3), P2(1,4,1), P3(2,9,4), P4(3,5,2)

- Gantt: P1[0-8] P3[8-17] P4[17-22] P2[22-26]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 8 | 8 | 0 | 0 |
| P2 | 26 | 25 | 21 | 21 |
| P3 | 17 | 15 | 6 | 6 |
| P4 | 22 | 19 | 14 | 14 |

- Avg waiting 10.25, avg turnaround 16.75, CPU utilisation 100.00%, makespan 26

### textbook / prio_p high-number-first
Processes (PID, arrival, burst, priority): P1(0,8,3), P2(1,4,1), P3(2,9,4), P4(3,5,2)

- Gantt: P1[0-2] P3[2-11] P1[11-17] P4[17-22] P2[22-26]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| P1 | 17 | 17 | 9 | 0 |
| P2 | 26 | 25 | 21 | 21 |
| P3 | 11 | 9 | 0 | 0 |
| P4 | 22 | 19 | 14 | 14 |

- Avg waiting 11.00, avg turnaround 17.50, CPU utilisation 100.00%, makespan 26

### simultaneous / prio_np high-number-first
Processes (PID, arrival, burst, priority): A(2,3,2), B(2,3,1), C(2,1,1), D(2,3,1)

- Gantt: IDLE[0-2] A[2-5] B[5-8] C[8-9] D[9-12]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| A | 5 | 3 | 0 | 0 |
| B | 8 | 6 | 3 | 3 |
| C | 9 | 7 | 6 | 6 |
| D | 12 | 10 | 7 | 7 |

- Avg waiting 4.00, avg turnaround 6.50, CPU utilisation 83.33%, makespan 12

### simultaneous / prio_p high-number-first
Processes (PID, arrival, burst, priority): A(2,3,2), B(2,3,1), C(2,1,1), D(2,3,1)

- Gantt: IDLE[0-2] A[2-5] B[5-8] C[8-9] D[9-12]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| A | 5 | 3 | 0 | 0 |
| B | 8 | 6 | 3 | 3 |
| C | 9 | 7 | 6 | 6 |
| D | 12 | 10 | 7 | 7 |

- Avg waiting 4.00, avg turnaround 6.50, CPU utilisation 83.33%, makespan 12

### simultaneous / rr q=1
Processes (PID, arrival, burst, priority): A(2,3,2), B(2,3,1), C(2,1,1), D(2,3,1)

- Gantt: IDLE[0-2] A[2-3] B[3-4] C[4-5] D[5-6] A[6-7] B[7-8] D[8-9] A[9-10] B[10-11] D[11-12]

| PID | Completion | Turnaround | Waiting | Response |
|---|---|---|---|---|
| A | 10 | 8 | 5 | 0 |
| B | 11 | 9 | 6 | 1 |
| C | 5 | 3 | 2 | 2 |
| D | 12 | 10 | 7 | 3 |

- Avg waiting 5.00, avg turnaround 7.50, CPU utilisation 83.33%, makespan 12
