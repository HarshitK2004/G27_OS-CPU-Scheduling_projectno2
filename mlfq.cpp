/*
 * Javadala Mathew
 * 21JE0425
 * mlfq.cpp - Multilevel Feedback Queue Scheduler
 * 3 priority queues: Q0 (RR quantum=2), Q1 (RR quantum=4), Q2 (FCFS)
 * Priority boost every 15 time units to prevent starvation.
 * Processes are demoted when they exhaust their time quantum.
 */

#include "common.h"

SimResult runMLFQ(vector<Process> procs, int num_cpus) {
    SimResult res;
    res.name = "Multilevel Feedback Queue (MLFQ)";
    res.cpus = num_cpus;

    int n = (int)procs.size();
    const int NUM_Q = 3;
    const int QUANTA[] = {2, 4, 100};
    const int BOOST_INTERVAL = 15;

    resetProcs(procs);

    deque<int> queues[3];
    vector<int> cpu_proc(num_cpus, -1);
    vector<int> cpu_q_left(num_cpus, 0);
    res.gantt.resize(num_cpus);

    int completed = 0, time = 0;
    int max_time = 0;
    for (auto& p : procs) max_time += p.burst;
    max_time += procs.back().arrival + 20;

    res.events.push_back(BLD + "MLFQ Config: Q0(RR q=2), Q1(RR q=4), Q2(FCFS), Boost every 15" + RST);

    while (completed < n && time < max_time) {
        // 1. Arrivals
        for (int i = 0; i < n; i++) {
            if (procs[i].arrival == time) {
                queues[0].push_back(i);
                procs[i].queue_level = 0;
                res.events.push_back("t=" + to_string(time) + ": P" +
                    to_string(procs[i].id) + " arrived -> Q0");
            }
        }

        // 2. Priority Boost
        if (time > 0 && time % BOOST_INTERVAL == 0) {
            bool boosted = false;
            for (int q = 1; q < NUM_Q; q++) {
                while (!queues[q].empty()) {
                    int idx = queues[q].front();
                    queues[q].pop_front();
                    procs[idx].queue_level = 0;
                    queues[0].push_back(idx);
                    boosted = true;
                }
            }
            if (boosted)
                res.events.push_back("t=" + to_string(time) +
                    ": ** PRIORITY BOOST ** all promoted to Q0");
        }

        // 3. Completions and quantum expiry
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
                int nq = min(procs[pi].queue_level + 1, NUM_Q - 1);
                string msg = "t=" + to_string(time) + ": P" +
                    to_string(procs[pi].id) + " quantum expired -> ";
                if (nq != procs[pi].queue_level)
                    msg += "demoted to Q" + to_string(nq);
                else
                    msg += "stays in Q" + to_string(nq);
                res.events.push_back(msg);
                procs[pi].queue_level = nq;
                queues[nq].push_back(pi);
                cpu_proc[c] = -1;
            }
        }

        // 4. Assign idle CPUs from highest-priority non-empty queue
        for (int c = 0; c < num_cpus; c++) {
            if (cpu_proc[c] != -1) continue;
            for (int q = 0; q < NUM_Q; q++) {
                if (!queues[q].empty()) {
                    int pi = queues[q].front();
                    queues[q].pop_front();
                    cpu_proc[c] = pi;
                    cpu_q_left[c] = QUANTA[q];
                    if (procs[pi].start_time == -1) {
                        procs[pi].start_time = time;
                    }
                    break;
                }
            }
        }

        // 5. Execute one time unit
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
