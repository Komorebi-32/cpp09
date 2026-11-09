# Review prompt: ex01 (RPN) against the 42 cpp09 subject

Review my `ex01` implementation against `../en.subject.cpp09.pdf` (read pages 3-7 for general/module rules and pages 11-12 for ex01).
Do not rewrite my code. Report findings, ranked by severity, each with `file:line`, a failing input, expected vs. actual behaviour.
Build it (`make re`), then actually run the edge cases below; do not guess.

## 1. Subject specs (check each, answer PASS / FAIL / PARTIAL)

- Directory `ex01/`; files `Makefile`, `main.cpp`, `RPN.{cpp,hpp}`; program name `RPN`.
- Takes ONE argument: the RPN expression. Wrong argc (0 or 2+) -> error on **stderr**.
- Numbers passed as input are always < 10 (single digit). Intermediate results and the final result are NOT bound by this rule (e.g. `9 9 * 9 *` = 729 must work).
- Operators `+ - / *` all handled. Brackets and decimals need not be handled and must yield `Error`.
- Correct result printed on **stdout**, newline-terminated.
- Any error -> message on **stderr** (the subject example prints `Error`), non-zero exit is sensible.
- Subject examples must match exactly:
  - `"8 9 * 9 - 9 - 9 - 4 - 1 +"` -> `42`
  - `"7 7 * 7 -"` -> `42`
  - `"1 2 * 2 / 2 * 2 4 - +"` -> `0`
  - `"(1 + 1)"` -> `Error`
- At least one STL container used. **A container used in ex00 (`std::map`) must NOT be used here.** Verify with `grep -rn "#include <" src inc`. This container is also burned for ex02.
- No `<algorithm>`, no C++11, no `using namespace`, no `friend`, no `*printf`/`*alloc`/`free`.

## 2. General rules (module 09)

- `c++ -Wall -Wextra -Werror`, still compiles with `-std=c++98`.
- Makefile: rules `$(NAME)`, `all`, `clean`, `fclean`, `re`; must not relink on a second `make`.
- Orthodox Canonical Form (default ctor, copy ctor, copy assignment, dtor) unless the class is justified as non-instantiable. Check that copies behave correctly (deep state).
- No function implementation in headers (except templates); include guards; headers self-contained.
- No memory leaks / invalid accesses: run `valgrind --leak-check=full --error-exitcode=1` on every case below.

## 3. Edge cases to run (expected = `Error` on stderr unless stated)

Parsing / tokens:
- empty string `""`, only spaces `"   "`, tabs/newlines inside the string
- single number `"5"` (decide: valid result `5`, or error? State what the code does and whether it is defensible. A stack holding exactly one value at the end is a valid RPN result)
- two numbers, no operator `"1 2"`; operator only `"+"`; operator first `"+ 1 2"`
- too few operands `"1 +"`, `"1 2 + +"`; too many operands left `"1 2 3 +"`
- multi-digit number `"10 2 +"`, `"12"`; negative `"-1 2 +"`; `"1 -2 +"`; decimal `"1.5 2 +"`
- tokens glued together `"12+"`, `"1 2+"`, `"1 2 ++"`, `"1 2 +3 *"`
- unknown tokens `"a"`, `"1 2 x"`, `"1 2 %"`, `"(1 + 1)"`, `"1 2 + )"`
- leading/trailing/multiple whitespace `"  1   2  +  "` (should work)
- `"+ -"` type strings where a sign could be misread as part of a number

Arithmetic:
- division by zero `"1 0 /"` and `"0 0 /"` -> Error, no crash, no SIGFPE
- integer division semantics and negative results: `"1 2 -"` -> `-1`, `"7 2 /"`, `"1 2 /"`, `"0 7 /"`, `"1 7 - 2 /"` (negative division/truncation: confirm it is consistent and documented)
- **overflow**: `"9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 *"` etc. Which type holds the stack (`int`, `long`, `long long`)? Is overflow detected (INT_MIN / -1 included) or does it silently wrap (UB for signed ints)? There is a test script in `ex01/tests/test_RPN_overflow.sh`: read it, run it, and judge whether it covers the real boundaries (`INT_MAX + 1`, `INT_MIN - 1`, `INT_MIN / -1`, multiplication overflow, long chains).
- very long expression (10k+ tokens): no stack overflow, no quadratic slowness.

Process level:
- no argument, 2+ arguments, argument starting with `-`
- output only on stdout for results, only on stderr for errors (check with `./RPN "..." 2>/dev/null` and `>/dev/null`)
- exit codes: 0 on success, non-zero on error

## 4. Code-quality checks

- Is partial state cleaned between calls (if the class is reusable)? Does an error mid-way leave the container dirty?
- Are `pop`/`top` ever called on an empty container?
- Are `char` values passed to `isdigit`/`isspace` cast to `unsigned char`?
- Any `atoi`/`strtol` use without validating the full token?
- Is the error path a clean exception caught in `main`, rather than `exit()` scattered in the code?

## 5. Output format

1. Table: spec item -> PASS/FAIL/PARTIAL -> evidence.
2. Table: edge case -> input -> expected -> actual -> verdict.
3. Ranked list of bugs/risks (critical = fails the subject or crashes; minor = style/robustness).
4. Container-reuse verdict (which container is used here, confirm it differs from ex00 and note what remains for ex02).
