// ============================================================================
// Shared benchmark helpers for the Big O demos
// ============================================================================
#pragma once

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using Clock = std::chrono::steady_clock;

// Returns elapsed time in microseconds.
template <typename Func>
double time_us(Func&& func) {
    auto start = Clock::now();
    func();
    auto end = Clock::now();
    return std::chrono::duration<double, std::micro>(end - start).count();
}

// Writing a result here stops the compiler deciding a loop does nothing and
// deleting it -- a timed loop whose answer is never used can vanish entirely.
inline volatile long long sink = 0;

inline void print_header(const std::string& title) {
    std::cout << "\n--- " << title << " ---\n";
    std::cout << std::setw(12) << "n"
              << std::setw(18) << "us per op"
              << std::setw(12) << "growth" << "\n";
    std::cout << std::string(42, '-') << "\n";
}

inline void print_row(int n, double us, double prev_us) {
    std::cout << std::setw(12) << n
              << std::setw(18) << std::fixed << std::setprecision(4) << us;
    if (prev_us > 0) {
        std::cout << std::setw(11) << std::setprecision(1) << (us / prev_us) << "x";
    }
    std::cout << "\n";
}

struct BenchResult {
    std::string operation;
    std::string structure;
    std::string complexity;
    int n;
    double time_us;
    std::string note;   // no commas -- it goes straight into the CSV
};

// Runs graph.py next to the CSV. python3 inside the Dev Container and on a
// Mac; the py launcher on Windows, where python3 is often not on the PATH.
inline void make_charts(const std::string& repo_dir, const std::string& flag) {
#ifdef _WIN32
    std::string py = "py -3";
#else
    std::string py = "python3";
#endif
    std::string cmd = py + " \"" + repo_dir + "/graph.py\" " + flag;
    std::cout << std::flush;   // our output first, then graph.py's
    if (std::system(cmd.c_str()) != 0) {
        std::cout << "  (Could not run graph.py automatically -- run it yourself:\n"
                  << "   python3 graph.py " << flag << ")\n";
    }
}

inline void write_time_csv(const std::string& path, const std::vector<BenchResult>& results) {
    std::ofstream csv(path);
    csv << "operation,structure,complexity,n,time_us,note\n";
    for (const auto& r : results) {
        csv << r.operation << "," << r.structure << "," << r.complexity << ","
            << r.n << "," << std::fixed << std::setprecision(5) << r.time_us
            << "," << r.note << "\n";
    }
}
