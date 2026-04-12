/*
 * main.cpp - Program Entry, I/O, Visualization & Comparison

 *
 * Compile all files together:
 *   g++ -o scheduler.exe main.cpp mlfq.cpp lottery.cpp edf.cpp -std=c++11
 */

#include "common.h"

// ======================== Input Functions ========================
vector<Process> getSampleProcesses() {
    const int N = 6;
    int ids[]      = {1,  2,  3,  4,  5,  6};
    int arrivals[] = {0,  1,  3,  4,  6,  8};
    int bursts[]   = {7,  5,  8,  3,  6,  4};
    int tickets[]  = {30, 20, 40, 10, 25, 15};
    int deadlines[]= {18, 12, 28, 10, 22, 16};

    vector<Process> procs(N);
    for (int i = 0; i < N; i++) {
        procs[i].id       = ids[i];
        procs[i].arrival  = arrivals[i];
        procs[i].burst    = bursts[i];
        procs[i].remaining= bursts[i];
        procs[i].tickets  = tickets[i];
        procs[i].deadline = deadlines[i];
    }
    return procs;
}

vector<Process> getCustomProcesses() {
    int n;
    cout << "\n  Enter number of processes: ";
    cin >> n;
    vector<Process> procs(n);
    cout << "  For each process enter: ArrivalTime  BurstTime  Tickets  Deadline\n";
    for (int i = 0; i < n; i++) {
        procs[i].id = i + 1;
        cout << "  P" << i + 1 << ": ";
        cin >> procs[i].arrival >> procs[i].burst >> procs[i].tickets >> procs[i].deadline;
        procs[i].remaining = procs[i].burst;
    }
    return procs;
}

void showProcessTable(const vector<Process>& procs) {
    printSubHeader("Process Table");
    cout << "  +-----+---------+-------+---------+----------+\n";
    cout << "  | PID | Arrival | Burst | Tickets | Deadline |\n";
    cout << "  +-----+---------+-------+---------+----------+\n";
    for (auto& p : procs) {
        cout << "  | " << fgc(p.id) << "P" << left << setw(2) << p.id << RST
             << " |   " << setw(4) << right << p.arrival
             << "  | " << setw(4) << p.burst
             << "  |   " << setw(4) << p.tickets
             << "  |    " << setw(4) << p.deadline << "  |\n";
    }
    cout << "  +-----+---------+-------+---------+----------+\n";
}

// ======================== Gantt Chart Display ========================
void printGanttChart(const SimResult& r) {
    printSubHeader("Gantt Chart - " + r.name);

    int T = r.total_time;
    const int MAX_PER_ROW = 25;

    for (int start = 0; start < T; start += MAX_PER_ROW) {
        int end = min(start + MAX_PER_ROW, T);

        cout << "\n  Time: ";
        for (int t = start; t < end; t++)
            cout << left << setw(4) << t;
        cout << " " << end << "\n";

        for (int c = 0; c < r.cpus; c++) {
            cout << "        ";
            for (int t = start; t < end; t++) cout << "+---";
            cout << "+\n";

            cout << "  CPU " << c << " ";
            for (int t = start; t < end; t++) {
                int pid = r.gantt[c][t];
                if (pid == -1) {
                    cout << "|" << DIM << " - " << RST;
                } else {
                    ostringstream oss;
                    oss << "P" << pid;
                    string label = oss.str();
                    if ((int)label.size() < 3) label += string(3 - label.size(), ' ');
                    cout << "|" << bgc(pid) << label << RST;
                }
            }
            cout << "|\n";
        }
        cout << "        ";
        for (int t = start; t < end; t++) cout << "+---";
        cout << "+\n";
    }

    cout << "\n  " << BLD << "Legend: " << RST;
    for (auto& p : r.procs)
        cout << bgc(p.id) << " P" << p.id << " " << RST << " ";
    cout << DIM << " - = idle" << RST << "\n";
}

// ======================== Metrics Table ========================
void printMetricsTable(const SimResult& r) {
    printSubHeader("Per-Process Metrics");
    cout << "  +-----+---------+-------+--------+-----------+----------+----------+\n";
    cout << "  | PID | Arrival | Burst | Finish | Turnaround|  Waiting | Response |\n";
    cout << "  +-----+---------+-------+--------+-----------+----------+----------+\n";
    for (auto& p : r.procs) {
        cout << "  | " << fgc(p.id) << "P" << left << setw(2) << p.id << RST
             << " |   " << right << setw(4) << p.arrival
             << "  | " << setw(4) << p.burst
             << "  | " << setw(5) << p.finish_time
             << "  |     " << setw(4) << p.turnaround
             << "  |    " << setw(4) << p.wait_time
             << "  |    " << setw(4) << p.response_time << "  |\n";
    }
    cout << "  +-----+---------+-------+--------+-----------+----------+----------+\n";

    printSubHeader("Summary Metrics");
    cout << fixed << setprecision(2);
    cout << "  Avg Turnaround Time : " << BLD << r.avg_tat << RST << "\n";
    cout << "  Avg Waiting Time    : " << BLD << r.avg_wt << RST << "\n";
    cout << "  Avg Response Time   : " << BLD << r.avg_rt << RST << "\n";
    cout << "  CPU Utilization     : " << BLD << r.cpu_util << "%" << RST << "\n";
    cout << "  Throughput          : " << BLD << r.throughput << " proc/unit" << RST << "\n";
}

// ======================== Event Log ========================
void printEvents(const SimResult& r) {
    if (r.events.empty()) return;
    printSubHeader("Scheduling Events");
    for (auto& e : r.events)
        cout << "    " << e << "\n";
}

// ======================== Comparison Display ========================
void printBar(double val, double max_val, int width, const string& color) {
    int filled = (max_val > 0) ? (int)round(val / max_val * width) : 0;
    filled = min(filled, width);
    cout << color;
    for (int i = 0; i < filled; i++) cout << "#";
    cout << RST;
    for (int i = filled; i < width; i++) cout << " ";
}

void printComparison(const vector<SimResult>& results) {
    printHeader("ALGORITHM COMPARISON", 62);

    int w = 62;
    cout << "\n  " << BLD << left << setw(12) << "Metric" << right;
    for (auto& r : results)
        cout << " | " << setw(12) << r.name.substr(0, 12);
    cout << " | Best" << RST << "\n";
    cout << "  " << string(w, '-') << "\n";

    struct Row { string label; vector<double> vals; bool lower_better; };
    vector<Row> rows = {
        {"Avg TAT",     {}, true},
        {"Avg Wait",    {}, true},
        {"Avg Response", {}, true},
        {"CPU Util(%)", {}, false},
        {"Throughput",  {}, false}
    };
    for (auto& r : results) {
        rows[0].vals.push_back(r.avg_tat);
        rows[1].vals.push_back(r.avg_wt);
        rows[2].vals.push_back(r.avg_rt);
        rows[3].vals.push_back(r.cpu_util);
        rows[4].vals.push_back(r.throughput);
    }

    for (auto& row : rows) {
        int best_idx = 0;
        for (int i = 1; i < (int)row.vals.size(); i++) {
            if (row.lower_better) {
                if (row.vals[i] < row.vals[best_idx]) best_idx = i;
            } else {
                if (row.vals[i] > row.vals[best_idx]) best_idx = i;
            }
        }
        cout << "  " << left << setw(12) << row.label << right;
        for (int i = 0; i < (int)row.vals.size(); i++) {
            cout << " | ";
            if (i == best_idx)
                cout << BLD << "\033[92m" << setw(12) << fixed << setprecision(2)
                     << row.vals[i] << RST;
            else
                cout << setw(12) << fixed << setprecision(2) << row.vals[i];
        }
        cout << " | " << results[best_idx].name.substr(0, 8) << "\n";
    }
    cout << "  " << string(w, '-') << "\n";

    string bar_colors[] = {"\033[91m", "\033[93m", "\033[96m"};
    string metric_labels[] = {
        "Average Turnaround Time", "Average Waiting Time",
        "Average Response Time", "CPU Utilization (%)", "Throughput"
    };

    printHeader("VISUAL COMPARISON (BAR CHART)", 62);

    for (int m = 0; m < 5; m++) {
        cout << "\n  " << BLD << metric_labels[m] << RST << ":\n";
        double mx = *max_element(rows[m].vals.begin(), rows[m].vals.end());
        if (mx < 0.001) mx = 1;
        for (int i = 0; i < (int)results.size(); i++) {
            string short_name = results[i].name.substr(0, 10);
            cout << "    " << left << setw(11) << short_name << " [";
            string clr = bar_colors[i % 3];
            printBar(rows[m].vals[i], mx, 30, clr);
            cout << "] " << fixed << setprecision(2) << rows[m].vals[i];

            int best_idx = 0;
            for (int j = 1; j < (int)rows[m].vals.size(); j++) {
                if (rows[m].lower_better) {
                    if (rows[m].vals[j] < rows[m].vals[best_idx]) best_idx = j;
                } else {
                    if (rows[m].vals[j] > rows[m].vals[best_idx]) best_idx = j;
                }
            }
            if (i == best_idx) cout << " " << BLD << "\033[92m<-- Best" << RST;
            cout << "\n";
        }
    }
}

// ======================== Display Single Result ========================
void displayResult(SimResult& r) {
    printGanttChart(r);
    printMetricsTable(r);
    printEvents(r);
}

// ======================== Main ========================
int main() {
    setupConsole();

    vector<Process> procs;
    int num_cpus = 2;
    bool running = true;

    while (running) {
        printHeader("ADVANCED CPU SCHEDULING - MULTIPROCESSOR SYSTEM", 56);
        cout << "\n  " << BLD << "Select Scheduling Algorithm:" << RST << "\n\n";
        cout << "    1. Multilevel Feedback Queue (MLFQ)\n";
        cout << "    2. Lottery Scheduler\n";
        cout << "    3. Earliest Deadline First (EDF)\n";
        cout << "    4. Run All & Compare\n";
        cout << "    5. Exit\n";
        cout << "\n  Choice: ";

        int choice;
        cin >> choice;

        if (choice == 5) { running = false; continue; }
        if (choice < 1 || choice > 4) {
            cout << "  Invalid choice.\n";
            continue;
        }

        cout << "\n  " << BLD << "Process Input:" << RST << "\n";
        cout << "    1. Use sample data (6 processes)\n";
        cout << "    2. Enter custom processes\n";
        cout << "  Choice: ";
        int inp;
        cin >> inp;

        if (inp == 1) procs = getSampleProcesses();
        else          procs = getCustomProcesses();

        cout << "\n  Number of CPUs (e.g. 2, 3, 4): ";
        cin >> num_cpus;
        if (num_cpus < 1) num_cpus = 1;

        showProcessTable(procs);

        if (choice == 1) {
            SimResult r = runMLFQ(procs, num_cpus);
            printHeader(r.name, 56);
            displayResult(r);
        } else if (choice == 2) {
            SimResult r = runLottery(procs, num_cpus);
            printHeader(r.name, 56);
            displayResult(r);
        } else if (choice == 3) {
            SimResult r = runEDF(procs, num_cpus);
            printHeader(r.name, 56);
            displayResult(r);
        } else {
            vector<SimResult> results;

            SimResult r1 = runMLFQ(procs, num_cpus);
            printHeader(r1.name, 56);
            displayResult(r1);
            results.push_back(r1);

            SimResult r2 = runLottery(procs, num_cpus);
            printHeader(r2.name, 56);
            displayResult(r2);
            results.push_back(r2);

            SimResult r3 = runEDF(procs, num_cpus);
            printHeader(r3.name, 56);
            displayResult(r3);
            results.push_back(r3);

            printComparison(results);
        }

        cout << "\n  Press Enter to continue...";
        cin.ignore(10000, '\n');
        cin.get();
    }

    cout << "\n  Goodbye!\n";
    return 0;
}
