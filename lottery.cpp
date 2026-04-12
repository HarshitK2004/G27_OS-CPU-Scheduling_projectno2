/*
 * lottery.cpp - Lottery Scheduler
 * Team Member 3: Lottery scheduling algorithm implementation
 *
 * Ticket-weighted random selection with time quantum of 3.
 * Each process holds tickets proportional to its priority.
 * A random lottery draw decides which process runs on each idle CPU.
 */

#include "common.h"

SimResult runLottery(vector<Process> procs, int num_cpus) {
    SimResult res;
    res.name = "Lottery Scheduler";
    res.cpus = num_cpus;

    int n = (int)procs.size();
    const int QUANTUM = 3;
    resetProcs(procs);

    vector<int> ready;
    vector<int> cpu_proc(num_cpus, -1);
    vector<int> cpu_q_left(num_cpus, 0);
    res.gantt.resize(num_cpus);

    int completed = 0, time = 0;
    int max_time = 0;
    for (auto& p : procs) max_time += p.burst;
    max_time += procs.back().arrival + 20;

    res.events.push_back(BLD + "Lottery Config: Time Quantum = " +
        to_string(QUANTUM) + ", ticket-weighted random selection" + RST);

    while (completed < n && time < max_time) {
        // 1. Arrivals
        for (int i = 0; i < n; i++) {
            if (procs[i].arrival == time) {
                ready.push_back(i);
                res.events.push_back("t=" + to_string(time) + ": P" +
                    to_string(procs[i].id) + " arrived (" +
                    to_string(procs[i].tickets) + " tickets)");
            }
        }

        // 2. Completions and quantum expiry
        for (int c = 0; c < num_cpus; c++) {
            int pi = cpu_proc[c];
            if (pi == -1) continue;

            if (procs[pi].remaining == 0) {
                procs[pi].done = true;
                procs[pi].finish_time = time;
                completed++;
                cpu_proc[c] = -1;
                res.events.push_back("t=" + to_string(time) + ": P" +
                    to_string(procs[pi].id) + " completed on CPU " + to_string(c));
            } else if (cpu_q_left[c] == 0) {
                ready.push_back(pi);
                cpu_proc[c] = -1;
            }
        }

        // 3. Lottery draw for each idle CPU
        for (int c = 0; c < num_cpus; c++) {
            if (cpu_proc[c] != -1 || ready.empty()) continue;

            int total = 0;
            for (int idx : ready) total += procs[idx].tickets;
            int winning = rand() % total;

            int cumul = 0;
            for (int j = 0; j < (int)ready.size(); j++) {
                cumul += procs[ready[j]].tickets;
                if (cumul > winning) {
                    int pi = ready[j];
                    ready.erase(ready.begin() + j);
                    cpu_proc[c] = pi;
                    cpu_q_left[c] = QUANTUM;
                    if (procs[pi].start_time == -1)
                        procs[pi].start_time = time;
                    res.events.push_back("t=" + to_string(time) +
                        ": Lottery draw -> P" + to_string(procs[pi].id) +
                        " wins (ticket " + to_string(winning) +
                        "/" + to_string(total) + ") -> CPU " + to_string(c));
                    break;
                }
            }
        }

        // 4. Execute one time unit
        for (int c = 0; c < num_cpus; c++) {
            int pi = cpu_proc[c];
            res.gantt[c].push_back(pi == -1 ? -1 : procs[pi].id);
            if (pi != -1) {
                procs[pi].remaining--;
                cpu_q_left[c]--;
            }
        }
        time++;
    }

    res.total_time = time;
    res.procs = procs;
    calcMetrics(res);
    return res;
}
