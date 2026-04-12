/*
 * edf.cpp - Earliest Deadline First Scheduler (Preemptive)
 * Team Member 4: EDF real-time scheduling algorithm implementation
 *
 * Global preemptive EDF across all CPUs.
 * At each time step, processes with the earliest deadlines
 * are assigned to CPUs. Running processes can be preempted
 * if a newly arrived process has a tighter deadline.
 */

#include "common.h"

SimResult runEDF(vector<Process> procs, int num_cpus) {
    SimResult res;
    res.name = "Earliest Deadline First (EDF)";
    res.cpus = num_cpus;

    int n = (int)procs.size();
    resetProcs(procs);

    vector<int> ready;
    vector<int> cpu_proc(num_cpus, -1);
    res.gantt.resize(num_cpus);

    int completed = 0, time = 0;
    int max_time = 0;
    for (auto& p : procs) max_time += p.burst;
    max_time += procs.back().arrival + 20;

    res.events.push_back(BLD + "EDF Config: Preemptive, global scheduling across CPUs" + RST);

    while (completed < n && time < max_time) {
        // 1. Arrivals
        for (int i = 0; i < n; i++) {
            if (procs[i].arrival == time) {
                ready.push_back(i);
                res.events.push_back("t=" + to_string(time) + ": P" +
                    to_string(procs[i].id) + " arrived (deadline=" +
                    to_string(procs[i].deadline) + ")");
            }
        }

        // 2. Completions
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
            }
        }

        // 3. Collect all candidates (ready pool + currently running)
        vector<int> candidates;
        for (int idx : ready) candidates.push_back(idx);
        for (int c = 0; c < num_cpus; c++)
            if (cpu_proc[c] != -1) candidates.push_back(cpu_proc[c]);
        ready.clear();

        sort(candidates.begin(), candidates.end(), [&](int a, int b) {
            if (procs[a].deadline != procs[b].deadline)
                return procs[a].deadline < procs[b].deadline;
            return procs[a].id < procs[b].id;
        });

        // 4. Assign top N candidates to CPUs, rest go back to ready pool
        vector<int> prev_cpu = cpu_proc;
        for (int c = 0; c < num_cpus; c++) cpu_proc[c] = -1;

        int assign_count = min(num_cpus, (int)candidates.size());
        for (int i = 0; i < assign_count; i++) {
            cpu_proc[i] = candidates[i];
            if (procs[candidates[i]].start_time == -1)
                procs[candidates[i]].start_time = time;
        }
        for (int i = assign_count; i < (int)candidates.size(); i++)
            ready.push_back(candidates[i]);

        // Log preemptions
        for (int c = 0; c < num_cpus; c++) {
            int old_p = prev_cpu[c];
            if (old_p == -1) continue;
            bool still_running = false;
            for (int c2 = 0; c2 < num_cpus; c2++)
                if (cpu_proc[c2] == old_p) { still_running = true; break; }
            if (!still_running && !procs[old_p].done)
                res.events.push_back("t=" + to_string(time) + ": P" +
                    to_string(procs[old_p].id) + " PREEMPTED (deadline=" +
                    to_string(procs[old_p].deadline) + ")");
        }

        // 5. Execute one time unit
        for (int c = 0; c < num_cpus; c++) {
            int pi = cpu_proc[c];
            res.gantt[c].push_back(pi == -1 ? -1 : procs[pi].id);
            if (pi != -1)
                procs[pi].remaining--;
        }
        time++;
    }

    res.total_time = time;
    res.procs = procs;
    calcMetrics(res);
    return res;
}
