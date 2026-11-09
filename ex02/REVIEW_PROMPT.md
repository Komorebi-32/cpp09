# Review prompt: ex02 (PmergeMe) against the 42 cpp09 subject

Review my `ex02` implementation against `../en.subject.cpp09.pdf` (read pages 3-7 for general/module rules and pages 13-15 for ex02).
Do not rewrite my code. Report findings, ranked by severity, each with `file:line`, a failing input, expected vs. actual behaviour.
Build it (`make re`), then actually run the edge cases below; do not guess.
Reference material in this folder: `Merge_Insertion_sort (3).pdf`, `Merge insert 1.png`, `Merge Insert 2.png`, `ex02 subject`.

## 1. Subject specs (check each, answer PASS / FAIL / PARTIAL)

- Directory `ex02/`; files `Makefile`, `main.cpp`, `PmergeMe.{cpp,hpp}`; program name `PmergeMe`.
- Takes a positive integer sequence as arguments (`./PmergeMe 3 5 9 7 4`).
- Sorts with the **Ford-Johnson merge-insertion** algorithm (Knuth, TAOCP vol. 3, p. 184). It must be the real algorithm, not `std::sort`, not plain merge sort, not plain insertion sort. Verify:
  - pairing + recursive sort of the larger elements of each pair,
  - the pend/main-chain construction and the leftover (odd) element handling,
  - insertion order driven by **Jacobsthal numbers** (1, 3, 5, 11, 21, 43, ...), inserting each group in descending index order,
  - binary search bounded to the correct sub-range (the bound is `b_i`'s partner `a_i`; not a search over the whole chain),
  - the number of comparisons: instrument or count them and compare with the theoretical worst case `F(n) = sum_{k=1..n} ceil(log2(3k/4))` (n=5 -> 7, n=21 -> 66, n=1000 about 8530). Anything noticeably above that means the insertion order or search bounds are wrong.
- Errors -> message on **stderr** (subject example: `./PmergeMe "-1" "2"` prints `Error`).
- Output on stdout, exactly these 4 lines in order:
  1. `Before:` + unsorted sequence
  2. `After:` + sorted sequence
  3. time taken with container #1 (name the container in the message)
  4. time taken with container #2 (name the container in the message)
- Time format is free but precision must be high enough to see the difference between the two containers (microseconds with decimals, like the example). The timing must include data management (container filling/parsing into the container) as well as sorting.
- Must handle at least **3000 different integers** (test 3000, and also 10000, 100000).
- **Two different containers**, and neither may be one used in ex00 (`std::map`) or ex01. Check with `grep -rn "#include <" src inc` and say which are the two used and which are already burned.
- Subject advice: one implementation per container instead of a single generic function. State whether the code duplicates (acceptable) or uses a template (also acceptable if both containers are really exercised and timed fairly).
- Duplicates handling is left to my discretion: confirm the behaviour is deliberate, consistent and documented (error vs. accepted and sorted stably).
- No `<algorithm>` (so no `std::sort`, `std::lower_bound`, `std::upper_bound`, `std::swap` from `<algorithm>`, `std::min/max`, `std::find`, `std::reverse` ...). `<utility>` for `std::pair`/`std::swap` in C++98 should be judged carefully: tell me what is included and whether anything transitively requires `<algorithm>`. Also no C++11 (`auto`, `nullptr`, range-for, `std::chrono`, `std::stoi`, `to_string`, brace-init), no `using namespace`, no `friend`, no `*printf`/`*alloc`/`free`.

## 2. General rules (module 09)

- `c++ -Wall -Wextra -Werror`, still compiles with `-std=c++98`.
- Makefile: rules `$(NAME)`, `all`, `clean`, `fclean`, `re`; must not relink on a second `make`.
- Orthodox Canonical Form for the class(es).
- No function implementation in headers (except templates); include guards; headers self-contained.
- No leaks / invalid accesses: run `valgrind --leak-check=full --error-exitcode=1` on small and large inputs.

## 3. Edge cases to run

Arguments:
- no argument (expected: error on stderr, or empty-sequence handling that is clearly defined)
- one element `5`; two elements `2 1` and `1 2`; three `3 1 2`
- already sorted, reverse sorted, all equal `7 7 7 7`, duplicates mixed
- odd and even counts, and sizes right at Jacobsthal boundaries: n = 2, 3, 4, 5, 6, 11, 12, 21, 22, 43, 44, 85, 86
- `0` (positive? decide: is 0 accepted? the subject says *positive*; state what the code does)
- negative `-1`, `"-1" "2"` (must print `Error`), `+5`, `5+`, `1.5`, `abc`, `""`, `" "`, `"1 2 3"` as ONE quoted argument (should it be split or rejected? say what the code does), `0x10`, `1e3`
- `INT_MAX` = 2147483647 (valid), `2147483648` (overflow int -> Error?), `4294967296`, 20+ digit numbers, leading zeros `007`
- whitespace inside tokens, empty-string argument among valid ones

Scale / robustness:
- `./PmergeMe $(shuf -i 1-100000 -n 3000 | tr "\n" " ")` -> sorted, both timings printed
- 10000 and 100000 unique values; also 3000 values with many duplicates
- Output of line 2 is truly sorted: verify with `sort -n -c`, and that it is a permutation of the input (same multiset)
- Large inputs: `Before:` / `After:` lines may be truncated with `[...]` as in the subject example, but only if that is intentional; for small inputs everything is printed
- Argument list too long for the shell (not a bug, just note limits)
- Deterministic: run 20 random inputs of random sizes 1..500 against `sort -n` and compare. Write the loop as a script and show it.

## 4. Algorithm review (the hard part)

For each container version:
- Does the recursion actually recurse on the **larger elements of the pairs** and keep the pair association (`b_i` knows its `a_i`) through the recursion? How is the main chain handed from level N-1 up to level N, and how are the partners re-attached after the recursive sort?
- Is the leftover (odd) element inserted last, in the correct range?
- Jacobsthal sequence generation: correct values, no overflow, and correct handling when the last group is incomplete (indices beyond the pend size).
- Insertion bound: when inserting `b_k`, the search range must end right before `a_k`'s current position, which moves as elements are inserted. Is that position tracked correctly?
- Complexity: no hidden O(n^2) beyond what the container's insertion costs imply (`std::vector::insert` shifting vs. `std::deque`/`std::list`). Does the timing difference between the two containers make sense?
- Is the sort stable w.r.t. duplicates, or at least correct with duplicates?
- Any off-by-one in the binary search (`lo`, `hi` inclusive vs. exclusive)?

## 5. Timing check

- Is the clock started before the container is filled and stopped after the sort completes, for each container separately?
- Is the same input used for both containers?
- Is the clock choice appropriate in C++98 (`clock()`, `gettimeofday`, `clock_gettime`)? Is the unit printed (us) correct (conversion from `CLOCKS_PER_SEC` etc.)?
- Is the time per container non-zero and plausible for 3000 elements (tens to hundreds of microseconds)?

## 6. Output format

1. Table: spec item -> PASS/FAIL/PARTIAL -> evidence.
2. Table: edge case -> input -> expected -> actual -> verdict.
3. Comparison-count table (n, measured comparisons, theoretical F(n)) for both containers.
4. Ranked list of bugs/risks (critical = fails the subject, wrong sort, crash, or not really Ford-Johnson; minor = style/robustness).
5. Container verdict: which two containers are used, confirm they are different from ex00 and ex01.
