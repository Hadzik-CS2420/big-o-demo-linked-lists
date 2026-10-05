# Big O Demo: Linked Lists vs. Arrays

**Not graded** -- this is a runnable demonstration, not an assignment.

## What This Shows

Times the same six operations on a dynamic array, a singly linked list and a doubly linked
list, at doubling sizes (1,000 to 16,000). It puts numbers behind CT 08, CT 09, CT 10 and
Lab 4 -- and behind the question Module 4 keeps asking: *which structure, for which job?*

| Operation | Dynamic array | Singly linked | Doubly linked | Why |
|-----------|---------------|---------------|---------------|-----|
| `push_front` | O(n) | O(1) | O(1) | An array shifts everything right; a list links one node in front |
| `push_back` | O(1) amortized | **O(n)** | O(1) | Singly linked walks to its last node; `tail_` fixes that |
| `pop_front` | O(n) | O(1) | O(1) | An array shifts everything left; a list moves `head_` |
| `pop_back` | O(1) | **O(n)** | O(1) | Singly linked needs the trailing-pointer walk; `tail_->prev` is one step |
| `get(i)` | **O(1)** | O(n) | O(n) | An array computes the address; a list has to walk there |
| `contains` | O(n) | O(n) | O(n) | A missing value means checking every element |

No structure wins every row. The fronts belong to lists, random access belongs to arrays,
and `tail_` is what makes a doubly linked list good at both ends.

`contains` is O(n) for all three, but run it and compare the actual times: the array is
usually several times faster. Its elements sit next to each other in memory, and the CPU's
cache rewards that. Big O says how cost *grows*; it does not say which one is fastest.

The space demo shows what the pointers cost:

| | Dynamic array | Singly linked | Doubly linked |
|-|---------------|---------------|---------------|
| Bytes per element (64-bit) | 4 to 8 | 16 -- an `int` and a `next` | 24 -- an `int`, `next` and `prev` |
| Heap allocations to hold n | about log&#8322;(n) | n | n |

All three are O(n) space. The space demo uses sizes one past a power of two -- the array's
emptiest moment, right after doubling -- so it is the array's worst case, and it is still
the smallest. The node sizes come from `sizeof` and do not include the bookkeeping the heap
adds to every separate allocation, which makes the lists cost more still.

## How to Run

```bash
cmake -B build
cmake --build build
./build/big-o-demo-linked-lists          # time:  prints tables, writes results.csv, makes charts.html
./build/big-o-demo-linked-lists-space    # space: prints tables, writes results_space.csv, makes charts_space.html
```

Each program finishes by running `graph.py`, which turns its CSV into charts. Open
`charts.html` or `charts_space.html` in a browser. `graph.py` needs nothing installed --
only Python itself -- so it works in the Dev Container, on Windows and on a Mac.

The time demo takes a few seconds: the singly linked `push_back` and `pop_back` at
16,000 elements walk about 128 million nodes each.

## Reading the Output

Each size is **double** the one before it, so the `growth` column tells you the complexity:

| growth | means | because |
|--------|-------|---------|
| ~1x | O(1) | doubling n did not change the cost per operation |
| ~2x | O(n) | doubling n doubled the cost per operation |

The smallest sizes are noisy. Trust the pattern across the larger rows. Every measurement is
run three times and the fastest run is kept.

## Files

| File | Purpose |
|------|---------|
| `src/lists.h` | Minimal singly and doubly linked lists -- the CT 08 - CT 10 designs, plus `get(i)` |
| `src/DynamicArray.h` | The growable array from the Module 3 demo, for comparison |
| `src/bench.h` | Timing, table printing, CSV writing and chart generation shared by both programs |
| `src/main.cpp` | The time demo |
| `src/space_main.cpp` | The space demo |
| `graph.py` | Turns `results.csv` / `results_space.csv` into HTML charts with a data table under each one |

`DynamicArray.h`, `bench.h` and `graph.py` are identical to the copies in
`module3-pointers-arrays/assignments/big-o-demo-arrays/` -- change one, change both.
