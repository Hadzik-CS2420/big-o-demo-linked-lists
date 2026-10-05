// ============================================================================
// Big O Demo: Linked Lists vs. Arrays -- Space Complexity
// ============================================================================
// All three structures are O(n) space. What differs is the constant: how many
// bytes each element really costs, and how many separate trips to the heap it
// takes to hold n of them.
//
//   dynamic array  -- one block of ints. 4 bytes each, plus up to 2x empty
//                     slack after doubling.
//   singly linked  -- one Node per element: an int AND a next pointer.
//   doubly linked  -- one DoublyNode per element: an int, next AND prev.
//
// Sizes are one past a power of two -- the array's emptiest moment, right after
// doubling -- so this is the array's worst case, and the lists still lose.
//
// The node sizes below come from sizeof, which is what your compiler actually
// lays out (including padding). They do NOT include the bookkeeping the heap
// adds to every separate allocation -- typically another 8 to 16 bytes each.
// That cost falls almost entirely on the lists, which allocate once per node.
//
// Not graded -- run it, read the output, and open charts_space.html.
// ============================================================================

#include "DynamicArray.h"
#include "bench.h"
#include "lists.h"

namespace {

const std::vector<int> SIZES = {1025, 2049, 4097, 8193, 16385};

struct SpaceRow {
    std::string metric, structure;
    int n;
    double value;
    std::string unit, note;
};

}  // namespace

int main() {
    std::cout << "============================================================\n";
    std::cout << "  Big O Demo: Linked Lists vs. Arrays -- Space\n";
    std::cout << "============================================================\n\n";
    std::cout << "On this machine:  sizeof(int) = " << sizeof(int)
              << "   sizeof(Node) = " << sizeof(Node)
              << "   sizeof(DoublyNode) = " << sizeof(DoublyNode) << "\n";
    std::cout << "  A Node is an int plus one pointer; a DoublyNode adds a second.\n";
    std::cout << "  Pointers are " << sizeof(void*) << " bytes here, so each pointer "
              << "costs more than the int it sits beside.\n\n";

    std::vector<SpaceRow> rows;
    const std::string TOTAL = "memory to hold n ints";
    const std::string PER = "bytes per element";
    const std::string ALLOCS = "heap allocations to build it";

    std::cout << std::setw(8) << "n"
              << std::setw(12) << "array B"
              << std::setw(12) << "singly B"
              << std::setw(12) << "doubly B"
              << std::setw(10) << "arr B/el"
              << std::setw(10) << "sgl B/el"
              << std::setw(10) << "dbl B/el"
              << std::setw(10) << "arr new"
              << std::setw(10) << "list new" << "\n";
    std::cout << std::string(94, '-') << "\n";

    for (int n : SIZES) {
        DynamicArray a(Growth::Double);
        for (int i = 0; i < n; ++i) a.push_back(i);

        double arr_b = static_cast<double>(a.capacity()) * sizeof(int);
        double sll_b = static_cast<double>(n) * sizeof(Node);
        double dll_b = static_cast<double>(n) * sizeof(DoublyNode);
        // The array calls new[] once per resize; a list calls new once per node.
        double arr_allocs = static_cast<double>(a.reallocations());
        double list_allocs = static_cast<double>(n);

        std::cout << std::setw(8) << n << std::fixed << std::setprecision(0)
                  << std::setw(12) << arr_b << std::setw(12) << sll_b << std::setw(12) << dll_b
                  << std::setprecision(1)
                  << std::setw(10) << arr_b / n << std::setw(10) << sll_b / n << std::setw(10) << dll_b / n
                  << std::setprecision(0)
                  << std::setw(10) << arr_allocs << std::setw(10) << list_allocs << "\n";

        rows.push_back({TOTAL, "dynamic array", n, arr_b, "bytes",
                        "All three are O(n) -- the slopes differ"});
        rows.push_back({TOTAL, "singly linked", n, sll_b, "bytes", ""});
        rows.push_back({TOTAL, "doubly linked", n, dll_b, "bytes", ""});

        rows.push_back({PER, "dynamic array", n, arr_b / n, "bytes per element",
                        "Even at its emptiest the array costs less per element than either list"});
        rows.push_back({PER, "singly linked", n, sll_b / n, "bytes per element", ""});
        rows.push_back({PER, "doubly linked", n, dll_b / n, "bytes per element", ""});

        rows.push_back({ALLOCS, "dynamic array", n, arr_allocs, "allocations",
                        "The array allocates about log2(n) times; a list allocates once per node"});
        rows.push_back({ALLOCS, "singly or doubly linked", n, list_allocs, "allocations", ""});
    }

    std::cout << "\nWhat to notice:\n";
    std::cout << "  - All three grow in a straight line with n: O(n) space.\n";
    std::cout << "  - Per element, the lists pay for their pointers. A doubly linked\n";
    std::cout << "    list spends more on prev and next than on the data itself.\n";
    std::cout << "  - The array here is at its worst -- half empty, just after doubling\n";
    std::cout << "    -- and it is still the smallest.\n";
    std::cout << "  - A list makes a separate heap allocation for every node. Each one\n";
    std::cout << "    carries hidden bookkeeping not counted above, and scatters the\n";
    std::cout << "    nodes across memory. The array allocates only once per resize.\n";

    std::string repo_dir = REPO_DIR;
    std::ofstream csv(repo_dir + "/results_space.csv");
    csv << "metric,structure,n,value,unit,note\n";
    for (const auto& row : rows) {
        csv << row.metric << "," << row.structure << "," << row.n << "," << std::fixed
            << std::setprecision(2) << row.value << "," << row.unit << "," << row.note << "\n";
    }
    csv.close();
    std::cout << "\n  Results written to results_space.csv -- generating charts...\n";
    make_charts(repo_dir, "--space");
    return 0;
}
