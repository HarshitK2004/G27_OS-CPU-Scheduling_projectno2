/*
 * common.h - Shared Data Structures, Constants & Utilities
 * Team Member 1: Core project foundation
 */

#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <string>
#include <numeric>
#include <climits>
#include <sstream>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// ======================== ANSI Color Codes ========================
const string RST = "\033[0m";
const string BLD = "\033[1m";
const string DIM = "\033[2m";

const string FG[] = {
    "\033[91m", "\033[92m", "\033[93m", "\033[94m", "\033[95m",
    "\033[96m", "\033[31m", "\033[32m", "\033[33m", "\033[34m"
};
const string BG[] = {
    "\033[41;97m", "\033[42;97m", "\033[43;30m", "\033[44;97m", "\033[45;97m",
    "\033[46;30m", "\033[101;30m", "\033[102;30m", "\033[103;30m", "\033[104;97m"
};
const int NCLR = 10;

inline string fgc(int pid) { return FG[(pid - 1) % NCLR]; }
inline string bgc(int pid) { return BG[(pid - 1) % NCLR]; }

// ======================== Data Structures ========================
struct Process {
    int id, arrival, burst, remaining;
    int tickets;
    int deadline;

    int start_time;
    int finish_time;
    int turnaround;
    int wait_time;
    int response_time;
    bool done;
    int queue_level;
    int quantum_left;

    Process()
        : id(0), arrival(0), burst(0), remaining(0),
          tickets(10), deadline(0),
          start_time(-1), finish_time(-1), turnaround(0),
          wait_time(0), response_time(-1), done(false),
          queue_level(0), quantum_left(0) {}
};

struct SimResult {
    string name;
    int cpus;
    int total_time;
    vector<vector<int>> gantt;
    vector<Process> procs;
    double avg_tat, avg_wt, avg_rt, cpu_util, throughput;
    vector<string> events;
};

// ======================== Console Setup ========================
inline void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD m = 0;
    GetConsoleMode(h, &m);
    SetConsoleMode(h, m | 0x0004);
#endif
    srand((unsigned)time(nullptr));
}

// ======================== Display Helpers ========================
inline void printLine(int w, char c = '=') {
    cout << "  " << string(w, c) << "\n";
}

inline void printHeader(const string& title, int w = 62) {
    cout << "\n";
    printLine(w);
    int pad = (w - (int)title.size()) / 2;
    cout << "  " << string(pad, ' ') << BLD << title << RST << "\n";
    printLine(w);
}

inline void printSubHeader(const string& title) {
    cout << "\n  " << BLD << ">> " << title << RST << "\n";
}

// ======================== Process Reset ========================
inline void resetProcs(vector<Process>& procs) {
    for (auto& p : procs) {
        p.remaining = p.burst;
        p.start_time = -1;
        p.finish_time = -1;
        p.turnaround = 0;
        p.wait_time = 0;
        p.response_time = -1;
        p.done = false;
        p.queue_level = 0;
        p.quantum_left = 0;
    }
}

// ======================== Metrics Calculation ========================
inline void calcMetrics(SimResult& r) {
    int n = (int)r.procs.size();
    double sT = 0, sW = 0, sR = 0;
    for (auto& p : r.procs) {
        p.turnaround = p.finish_time - p.arrival;
        p.wait_time = p.turnaround - p.burst;
        p.response_time = p.start_time - p.arrival;
        sT += p.turnaround;
        sW += p.wait_time;
        sR += p.response_time;
    }
    int busy = 0;
    for (int c = 0; c < r.cpus; c++)
        for (int t = 0; t < r.total_time; t++)
            if (r.gantt[c][t] != -1) busy++;

    r.avg_tat = sT / n;
    r.avg_wt = sW / n;
    r.avg_rt = sR / n;
    r.cpu_util = 100.0 * busy / (r.cpus * r.total_time);
    r.throughput = (double)n / r.total_time;
}

// ======================== Algorithm Declarations ========================
SimResult runMLFQ(vector<Process> procs, int num_cpus);
SimResult runLottery(vector<Process> procs, int num_cpus);
SimResult runEDF(vector<Process> procs, int num_cpus);

#endif
