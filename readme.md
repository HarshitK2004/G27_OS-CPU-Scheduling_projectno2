# Advanced CPU Scheduling Algorithms for Multiprocessor Systems

**Operating Systems — Lab Project 2**

## Overview

This project implements and compares three advanced CPU scheduling algorithms designed for **multiprocessor systems**. The simulator allows users to configure the number of CPUs, define custom process sets, and visualize scheduling behavior through color-coded Gantt charts, per-process metrics tables, and side-by-side algorithm comparison with bar charts.

## Algorithms Implemented

### 1. Multilevel Feedback Queue (MLFQ)

A priority-based scheduler with three feedback queues:

| Queue | Policy | Time Quantum |
|-------|--------|-------------|
| Q0 | Round Robin | 2 |
| Q1 | Round Robin | 4 |
| Q2 | FCFS | — |

- New processes enter Q0 (highest priority).
- If a process exhausts its quantum without finishing, it is **demoted** to the next lower queue.
- A **priority boost** occurs every 15 time units — all queued processes are promoted back to Q0 to prevent starvation.
- Idle CPUs always pick from the highest-priority non-empty queue first.

### 2. Lottery Scheduler

A probabilistic scheduler based on randomized ticket allocation:

- Each process holds a configurable number of **lottery tickets**.
- When a CPU becomes idle, a random draw selects the next process, weighted by ticket count (more tickets = higher chance).
- Uses a **time quantum of 3** — after the quantum expires, the process returns to the ready pool and a new lottery is held.
- Provides **proportional-share fairness** without strict priority levels.

### 3. Earliest Deadline First (EDF)

A preemptive real-time scheduling algorithm:

- Each process has an **absolute deadline**.
- At every time step, the scheduler assigns the processes with the **earliest deadlines** to the available CPUs.
- A running process can be **preempted** if a newly arrived process has a tighter deadline.
- Implements **global EDF** — scheduling decisions are made across all CPUs simultaneously.

## Features

- **Multiprocessor support** — configurable number of CPUs (2, 3, 4, etc.)
- **Color-coded Gantt charts** — each process has a distinct background color for visual clarity
- **Per-process metrics** — arrival, burst, finish, turnaround, waiting, and response times
- **Summary metrics** — average turnaround time, average waiting time, average response time, CPU utilization, and throughput
- **Scheduling event logs** — detailed trace of arrivals, demotions, preemptions, lottery draws, and completions
- **Algorithm comparison** — tabular comparison highlighting the best algorithm per metric
- **Visual bar charts** — ASCII bar graphs comparing all three algorithms across five performance metrics
- **Sample and custom input** — built-in sample dataset of 6 processes or user-defined process sets

## Project Structure

```
├── common.h        Shared data structures, constants, ANSI colors, utility functions
├── mlfq.cpp        MLFQ algorithm implementation
├── lottery.cpp     Lottery scheduler implementation
├── edf.cpp         EDF algorithm implementation
├── main.cpp        Main program, menu, I/O, Gantt chart display, comparison
└── README.md       Project documentation
```

## Team Contributions

| File | Member | Responsibility |
|------|--------|----------------|
| `common.h` | Member 1 | Core structures (`Process`, `SimResult`), color codes, console setup, metrics calculation |
| `mlfq.cpp` | Member 2 | MLFQ scheduling — queue management, demotion logic, priority boost |
| `lottery.cpp` | Member 3 | Lottery scheduling — ticket allocation, random draw, quantum management |
| `edf.cpp` | Member 4 | EDF scheduling — deadline sorting, preemptive assignment, global scheduling |
| `main.cpp` | Member 5 | User interface, process input, Gantt chart rendering, metrics tables, comparison bar charts |
| `main.cpp` | Member 5 | User interface, process input, Gantt chart rendering, metrics tables, comparison bar charts |
| `readme.md` | Member 6 | Execution support(scheduler.exe) and overall readme compilation |
## How to Compile and Run

### Prerequisites

- A C++ compiler supporting C++11 (g++, MSVC, clang++)

### Compilation

```bash
g++ -o scheduler.exe main.cpp mlfq.cpp lottery.cpp edf.cpp -std=c++11
```

Or with MSVC:

```bash
cl /EHsc /std:c++17 main.cpp mlfq.cpp lottery.cpp edf.cpp /Fe:scheduler.exe
```

### Running

```bash
./scheduler.exe
```

## Usage

On launch, the program displays a menu:

```
  Select Scheduling Algorithm:

    1. Multilevel Feedback Queue (MLFQ)
    2. Lottery Scheduler
    3. Earliest Deadline First (EDF)
    4. Run All & Compare
    5. Exit
```

- Options **1–3** run a single algorithm and display its Gantt chart, metrics, and event log.
- Option **4** runs all three algorithms on the same process set and shows a full comparison with tables and bar charts.

### Process Input

You can either use the **built-in sample data** (6 processes) or enter **custom processes** by specifying:

| Field | Description |
|-------|-------------|
| Arrival Time | When the process enters the system |
| Burst Time | Total CPU time required |
| Tickets | Number of lottery tickets (used by Lottery Scheduler) |
| Deadline | Absolute deadline (used by EDF) |

### Sample Data

| PID | Arrival | Burst | Tickets | Deadline |
|-----|---------|-------|---------|----------|
| P1 | 0 | 7 | 30 | 18 |
| P2 | 1 | 5 | 20 | 12 |
| P3 | 3 | 8 | 40 | 28 |
| P4 | 4 | 3 | 10 | 10 |
| P5 | 6 | 6 | 25 | 22 |
| P6 | 8 | 4 | 15 | 16 |

## Performance Metrics

The following metrics are computed for each algorithm:

| Metric | Formula | Description |
|--------|---------|-------------|
| Turnaround Time | Finish − Arrival | Total time from arrival to completion |
| Waiting Time | Turnaround − Burst | Time spent waiting in ready queue |
| Response Time | First Execution − Arrival | Time from arrival to first CPU access |
| CPU Utilization | (Busy Time / Total Time) × 100 | Percentage of time CPUs are active |
| Throughput | Processes / Total Time | Number of processes completed per time unit |

## Sample Output

### Gantt Chart (MLFQ, 2 CPUs)

```
  Time: 0   1   2   3   4   5   6   7   8   9   10  11  12
        +---+---+---+---+---+---+---+---+---+---+---+---+
  CPU 0 |P1 |P1 |P1 |P1 |P1 |P1 |P5 |P5 |P6 |P6 |P3 |P3 |
        +---+---+---+---+---+---+---+---+---+---+---+---+
  CPU 1 | - |P2 |P2 |P3 |P3 |P4 |P4 |P2 |P2 |P2 |P4 |P5 |
        +---+---+---+---+---+---+---+---+---+---+---+---+
```

### Comparison Bar Chart

```
  Average Turnaround Time:
    MLFQ        [##############################] 10.67
    Lottery     [##########################    ]  9.33
    EDF         [######################        ]  7.67  <-- Best

  Average Waiting Time:
    MLFQ        [##############################]  5.17
    Lottery     [######################        ]  3.83
    EDF         [#############                 ]  2.17  <-- Best
```

## Notes

- The terminal must support **ANSI escape codes** for colored output. Modern Windows 10/11 terminals, Linux, and macOS terminals work out of the box.
- The Lottery Scheduler uses `rand()` seeded with system time, so results may vary between runs.
- Gantt charts automatically wrap to the next line for simulations longer than 25 time units.
- Array-based implementation is used throughout (no linked lists or complex data structures).
