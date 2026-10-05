// ============================================================================
// Big O Demo: Linked Lists -- Time Complexity
// ============================================================================
// Times the same operations on three structures, at doubling sizes, so you
// can SEE which one each operation favors:
//
//                     dynamic array      singly linked     doubly linked
//   push_front        O(n)  shift        O(1)              O(1)
//   push_back         O(1)  amortized    O(n)  walk        O(1)  tail_
//   pop_front         O(n)  shift        O(1)              O(1)
//   pop_back          O(1)               O(n)  walk        O(1)  tail_->prev
//   get(i)            O(1)  arithmetic   O(n)  walk        O(n)  walk
//   contains          O(n)               O(n)              O(n)
//
// No structure wins every row. That table is the whole point of Module 4.
//
// Sizes double each step, so read the 'growth' column like this:
//   ~1x  -> O(1)   doubling n did not change the cost per operation
//   ~2x  -> O(n)   doubling n doubled the cost per operation
//
// Not graded -- run it, read the output, and open charts.html.
// ============================================================================

#include "DynamicArray.h"
#include "bench.h"
#include "lists.h"

#include <algorithm>

namespace {

const std::vector<int> SIZES = {1000, 2000, 4000, 8000, 16000};
constexpr int RUNS = 3;   // each measurement is repeated; the fastest run counts

template <typename Func>
double best_us(Func&& func) {
    double best = 1e300;
    for (int r = 0; r < RUNS; ++r) best = std::min(best, func());
    return best;
}

// One structure's measurement: microseconds PER OPERATION at size n.
using Measure = double (*)(int);

struct Row {
    const char* structure;
    const char* complexity;
    Measure measure;
};

void run(std::vector<BenchResult>& results, const std::string& op,
         const std::string& note, std::initializer_list<Row> rows) {
    for (const Row& row : rows) {
        print_header(op + " -- " + row.structure + " " + row.complexity);
        double prev = 0;
        for (int n : SIZES) {
            double us = best_us([&] { return row.measure(n); });
            print_row(n, us, prev);
            results.push_back({op, row.structure, row.complexity, n, us, note});
            prev = us;
        }
    }
}

// Builds a structure with n elements using push_back (or the array's own).
template <typename T> void fill(T& s, int n) { for (int i = 0; i < n; ++i) s.push_back(i); }
template <> void fill(SinglyLinkedList& s, int n) { for (int i = n - 1; i >= 0; --i) s.push_front(i); }

// ── push_front ─────────────────────────────────────────────────────────────
double arr_push_front(int n) {
    DynamicArray a(Growth::Double);
    double t = time_us([&] { for (int i = 0; i < n; ++i) a.insert_front(i); });
    sink = static_cast<long long>(a.size());
    return t / n;
}
template <typename L> double list_push_front(int n) {
    double t;
    { L l; t = time_us([&] { for (int i = 0; i < n; ++i) l.push_front(i); }); sink = static_cast<long long>(l.size()); }
    return t / n;
}

// ── push_back ──────────────────────────────────────────────────────────────
double arr_push_back(int n) {
    double t;
    { DynamicArray a(Growth::Double); t = time_us([&] { for (int i = 0; i < n; ++i) a.push_back(i); }); sink = static_cast<long long>(a.size()); }
    return t / n;
}
template <typename L> double list_push_back(int n) {
    double t;
    { L l; t = time_us([&] { for (int i = 0; i < n; ++i) l.push_back(i); }); sink = static_cast<long long>(l.size()); }
    return t / n;
}

// ── pop_front / pop_back: start full, empty it ─────────────────────────────
double arr_pop_front(int n) {
    DynamicArray a(Growth::Double); fill(a, n);
    double t = time_us([&] { for (int i = 0; i < n; ++i) a.remove_front(); });
    return t / n;
}
double arr_pop_back(int n) {
    DynamicArray a(Growth::Double); fill(a, n);
    double t = time_us([&] { for (int i = 0; i < n; ++i) a.pop_back(); });
    return t / n;
}
template <typename L> double list_pop_front(int n) {
    L l; fill(l, n);
    double t = time_us([&] { for (int i = 0; i < n; ++i) l.pop_front(); });
    return t / n;
}
template <typename L> double list_pop_back(int n) {
    L l; fill(l, n);
    double t = time_us([&] { for (int i = 0; i < n; ++i) l.pop_back(); });
    return t / n;
}

// ── get(i) at the middle -- the worst spot for either end of a doubly list ─
constexpr int GETS = 200;
double arr_get(int n) {
    DynamicArray a(Growth::Double); fill(a, n);
    long long sum = 0;
    double t = time_us([&] { for (int k = 0; k < GETS; ++k) sum += a.at(n / 2); });
    sink = sum;
    return t / GETS;
}
template <typename L> double list_get(int n) {
    L l; fill(l, n);
    long long sum = 0;
    double t = time_us([&] { for (int k = 0; k < GETS; ++k) sum += l.get(n / 2); });
    sink = sum;
    return t / GETS;
}

// ── contains, for a value that is never there ──────────────────────────────
constexpr int SEARCHES = 200;
double arr_contains(int n) {
    DynamicArray a(Growth::Double); fill(a, n);
    int found = 0;
    double t = time_us([&] { for (int k = 0; k < SEARCHES; ++k) found += a.contains(-1); });
    sink = found;
    return t / SEARCHES;
}
template <typename L> double list_contains(int n) {
    L l; fill(l, n);
    int found = 0;
    double t = time_us([&] { for (int k = 0; k < SEARCHES; ++k) found += l.contains(-1); });
    sink = found;
    return t / SEARCHES;
}

using SLL = SinglyLinkedList;
using DLL = DoublyLinkedList;

}  // namespace

int main() {
    std::cout << "============================================================\n";
    std::cout << "  Big O Demo: Linked Lists vs. Arrays -- Time\n";
    std::cout << "============================================================\n";
    std::cout << "\nEach size is double the one before it. Watch 'growth':\n";
    std::cout << "  ~1x  the cost per operation did not change   -> O(1)\n";
    std::cout << "  ~2x  the cost per operation doubled with n   -> O(n)\n";
    std::cout << "Small sizes are noisy; trust the pattern across the bigger rows.\n";

    std::vector<BenchResult> r;

    run(r, "push_front", "An array shifts everything right; a list just links one node in front",
        {{"dynamic array", "O(n)", arr_push_front},
         {"singly linked", "O(1)", list_push_front<SLL>},
         {"doubly linked", "O(1)", list_push_front<DLL>}});

    run(r, "push_back", "A singly linked list has to walk to its last node; tail_ fixes that",
        {{"dynamic array", "O(1) amortized", arr_push_back},
         {"singly linked", "O(n)", list_push_back<SLL>},
         {"doubly linked", "O(1)", list_push_back<DLL>}});

    run(r, "pop_front", "An array shifts everything left; a list just moves head_",
        {{"dynamic array", "O(n)", arr_pop_front},
         {"singly linked", "O(1)", list_pop_front<SLL>},
         {"doubly linked", "O(1)", list_pop_front<DLL>}});

    run(r, "pop_back", "Singly linked needs the trailing-pointer walk; tail_->prev makes it one step",
        {{"dynamic array", "O(1)", arr_pop_back},
         {"singly linked", "O(n)", list_pop_back<SLL>},
         {"doubly linked", "O(1)", list_pop_back<DLL>}});

    run(r, "get(i) at the middle", "An array computes the address; a list has to walk there",
        {{"dynamic array", "O(1)", arr_get},
         {"singly linked", "O(n)", list_get<SLL>},
         {"doubly linked", "O(n)", list_get<DLL>}});

    run(r, "contains (not found)", "A missing value means checking every element in all three",
        {{"dynamic array", "O(n)", arr_contains},
         {"singly linked", "O(n)", list_contains<SLL>},
         {"doubly linked", "O(n)", list_contains<DLL>}});

    std::cout << "\nWhat to notice:\n";
    std::cout << "  - The front belongs to lists: push_front and pop_front stay flat\n";
    std::cout << "    for both lists while the array's cost doubles with n.\n";
    std::cout << "  - The back is where tail_ earns its keep: singly linked push_back\n";
    std::cout << "    and pop_back grow with n; doubly linked stays flat.\n";
    std::cout << "  - get(i) belongs to arrays. A list cannot jump to position i --\n";
    std::cout << "    even a doubly linked list still walks.\n";
    std::cout << "  - contains is O(n) for all three, but compare the actual numbers:\n";
    std::cout << "    the array is usually faster, because its elements sit next to\n";
    std::cout << "    each other in memory and the CPU's cache loves that.\n";

    std::string repo_dir = REPO_DIR;
    write_time_csv(repo_dir + "/results.csv", r);
    std::cout << "\n  Results written to results.csv -- generating charts...\n";
    make_charts(repo_dir, "");
    return 0;
}
