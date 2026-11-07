# Week 12: Deadlock Avoidance — Banker's Algorithm Simulator

**Name:** Aditya Kumar  
**Registration No:** AP24110010333  
**Branch/Section:** BTech CSE - C  
**Institution:** SRM University AP  
**Course:** Operating Systems (CSE302)  
**Environment:** Ubuntu Linux / WSL (`aditya-forge@LAPTOP-184D82EB`)  
**Compiler:** GCC (`-std=c99 -Wall -Wextra -Werror -pedantic`)

---

## Overview

In **Week 08** and **Week 09**, we examined race conditions in concurrent execution and implemented mutual exclusion using hardware atomic primitives (`test_and_set` and `compare_and_swap`).

In **Week 11**, we elevated concurrency synchronization using **POSIX semaphores**, where we also demonstrated how out-of-order resource acquisition directly induces **deadlock** through circular waiting. While static acquisition ordering can prevent deadlock by convention, practical operating systems require dynamic mechanisms to manage multiple competing processes requesting arbitrary instances of multiple resource types.

In **Week 12**, we implement **Dijkstra's Banker's Algorithm** for dynamic deadlock avoidance. Rather than restricting concurrent access or detecting deadlocks post-facto, the Banker's Algorithm simulates resource allocation states in advance:
1. **Safety Algorithm (`is_safe`)**: Determines whether a system state has at least one valid execution sequence $\langle P_0, P_1, \dots, P_{n-1} \rangle$ where each process can satisfy its maximum potential claim with currently available and accumulated resources.
2. **Resource-Request Algorithm (`try_request`)**: Evaluates incoming dynamic requests. A request is tentatively allocated and tested against the safety algorithm. If the resulting state is safe, the request is permanently committed (`GRANTED`); if unsafe, it is completely **rolled back** (`DENIED: would lead to unsafe state`) to maintain systemic stability.
3. **Rigorous Input Validation**: Robust token-by-token parsing ensuring non-numeric inputs, dimension violations, negative resources, and claims exceeding declarations are rejected with error diagnostic messages before program execution.

---

## Included Files

| File | Description |
|---|---|
| `bankers.c` | Production-grade C99 implementation of the Banker's Algorithm (Safety & Resource-Request with full state rollback) |
| `Makefile` | Build automation with strict compiler flags (`-std=c99 -Wall -Wextra -Werror -pedantic`), automated test harness, and clean target |
| `test_results.log` | Consolidated test execution logs covering all 15 benchmark and validation test runs |
| `Bankers_Report.pdf` | Detailed laboratory report covering deadlock avoidance theory, data structure design, and algorithmic analysis |
| `tests/` | Comprehensive test suite containing standard benchmark workloads and edge-case validation scenarios |

---

## Build and Run

### Compilation
Build the binary with strict standard conformance:
```bash
make
# or directly:
gcc -std=c99 -Wall -Wextra -Werror -pedantic -o bankers bankers.c
```

### Execution
Run with standard input redirection:
```bash
./bankers < tests/test1_sample.txt
```

### Automated Testing
Run the test suite across all benchmark inputs:
```bash
make test
```

### Cleanup
Remove compiled binaries:
```bash
make clean
```

---

## 1. Design & Algorithmic Architecture

1. **Dynamic Matrix Allocation**: System state is tracked using dynamically allocated 2D matrices (`Allocation`, `Max`, `Need = Max - Allocation`) and a 1D vector (`Available`), allocated via `alloc_matrix()` and cleanly deallocated on all execution paths through a unified cleanup label in `main()`.
2. **Strict Token-by-Token Input Parsing**: Input is parsed sequentially using `strtol()`. Malformed dimensions, negative/out-of-range resource quantities, `Allocation > Max` inconsistencies, unrecognized query types, invalid process indices, and unexpected trailing tokens are caught early and reported to `stderr` with exit status `1`.
3. **Safety Verification (`is_safe()`)**:
   - Initializes a working resource vector $\text{Work} = \text{Available}$ and a boolean vector $\text{Finish}[i] = \text{false}$.
   - Performs iterative passes over all processes $i \in [0, N-1]$ to find an unfinished process whose $\text{Need}_i \le \text{Work}$.
   - Upon finding such a process, simulates completion by returning its held resources: $\text{Work} = \text{Work} + \text{Allocation}_i$, marking $\text{Finish}[i] = \text{true}$.
   - If all processes finish, outputs `SAFE:` followed by the execution sequence. If a pass yields no progress while processes remain, outputs `UNSAFE`. The underlying system state is never mutated during safety checks.
4. **Tentative Allocation & Rollback (`try_request()`)**:
   - Validates that $\text{Request}_i \le \text{Need}_i$ (rejecting claims beyond declared maximum).
   - Validates that $\text{Request}_i \le \text{Available}$ (rejecting requests with insufficient physical units).
   - Tentatively updates:
     $$\text{Available} \leftarrow \text{Available} - \text{Request}_i$$
     $$\text{Allocation}_i \leftarrow \text{Allocation}_i + \text{Request}_i$$
     $$\text{Need}_i \leftarrow \text{Need}_i - \text{Request}_i$$
   - Executes `is_safe()`. If safe, the transaction commits (`GRANTED`). If unsafe, state changes are rolled back to their exact previous values (`DENIED: would lead to unsafe state`).
5. **Decoupled Error & Status Codes**: Helper routines communicate state transitions using discrete status enums (`REQ_GRANTED`, `REQ_EXCEEDS_MAX`, `REQ_INSUFFICIENT`, `REQ_UNSAFE`, `REQ_ERROR`), keeping algorithmic logic cleanly separated from user I/O formatting.
6. **Sanitizer & Memory Safety**: Validated under AddressSanitizer (`ASan`), UndefinedBehaviorSanitizer (`UBSan`), and LeakSanitizer (`LSan`) with zero memory leaks, zero buffer overflows, and zero undefined behavior warnings.

---

## 2. Test Cases & Verification

### Test 1: Standard Assignment Benchmark
Validates baseline safety sequence and valid request granting:
- **Input**: `tests/test1_sample.txt` ($N=5, M=3$)
- **Queries**: Initial safety query followed by process 1 requesting $(1, 0, 2)$
```text
$ ./bankers < tests/test1_sample.txt
SAFE: 1 3 4 0 2

GRANTED
```

### Test 2: Safe Request Grant with State Update
Validates state progression after an approved allocation:
- **Input**: `tests/test2_safe_grant.txt` ($N=4, M=2$)
- **Queries**: Initial safety query, $P_3$ requests $(1, 1)$, subsequent safety query
```text
$ ./bankers < tests/test2_safe_grant.txt
SAFE: 1 2 3 0

GRANTED

SAFE: 3 1 2 0
```

### Test 3: Requests Exceeding Maximum Claim
Validates rejection when process asks for more than its declared maximum:
- **Input**: `tests/test3_exceeds_claim.txt` ($N=5, M=3$)
- **Queries**: $P_0$ requests $(8, 0, 0)$ ($\text{Need}_0 = 7$), $P_2$ requests $(0, 0, 5)$ ($\text{Need}_2 = 0$)
```text
$ ./bankers < tests/test3_exceeds_claim.txt
DENIED: exceeds maximum claim

DENIED: exceeds maximum claim

SAFE: 1 3 4 0 2
```

### Test 4: Resource Scarcity & Unsafe State Avoidance (Rollback Verification)
Validates denial when resources are insufficient or would induce deadlock:
- **Input**: `tests/test4_unsafe_request.txt` ($N=5, M=3$)
- **Queries**: $P_1$ requests $(1, 0, 2)$ (Granted); $P_4$ requests $(3, 3, 0)$ (Insufficient); $P_0$ requests $(0, 2, 0)$ (Fits Available but unsafe -> Rollback)
```text
$ ./bankers < tests/test4_unsafe_request.txt
GRANTED

DENIED: insufficient resources

DENIED: would lead to unsafe state

SAFE: 1 3 4 0 2
```

### Test 5: All-Zero Null Request
A null request $(0, 0, 0)$ is legal, verifies safety, and leaves system state unchanged:
```text
$ ./bankers < tests/test5_zero_request.txt
GRANTED

SAFE: 1 3 4 0 2

GRANTED
```

### Test 6: Inherently Unsafe Initial System State
Validates system behavior when initialized in an unsafe configuration with Available $(1, 1)$:
```text
$ ./bankers < tests/test6_unsafe_state.txt
UNSAFE

DENIED: would lead to unsafe state
```

---

## 3. Input Validation & Fault Tolerance

The simulator rejects improper inputs on `stderr` with exit code `1` before producing stdout output:

| Test File | Condition Tested | Exit Code | Diagnostic Message (`stderr`) |
|---|---|:---:|---|
| `bad1_alloc_exceeds_max.txt` | Allocation exceeds declared Max | 1 | `Error: Allocation[0][0] is greater than Max[0][0]` |
| `bad2_negative_available.txt` | Negative available resource count | 1 | `Error: Available[0] = -1 is out of range [0, 1000]` |
| `bad3_too_few_tokens.txt` | Truncated input stream | 1 | `Error: input ended early, missing Q` |
| `bad4_extra_tokens.txt` | Extraneous tokens after last query | 1 | `Error: unexpected extra input 'S' after the last query` |
| `bad5_unknown_query.txt` | Unsupported query opcode | 1 | `Error: unknown query 'X' (expected S or R)` |
| `bad6_process_out_of_range.txt` | Out-of-bounds process identifier | 1 | `Error: process index of request = 5 is out of range [0, 1]` |
| `bad7_zero_processes.txt` | Process count below valid range | 1 | `Error: N = 0 is out of range [1, 20]` |
| `bad8_non_numeric.txt` | Floating point or non-integer token | 1 | `Error: '1.5' is not a valid integer (Available[0])` |
| `bad9_negative_request.txt` | Negative resource request value | 1 | `Error: request amount = -1 is out of range [0, 1000]` |

---

## 4. Why Rollback on Failed Request is Essential

In the Banker's Algorithm, request safety cannot be evaluated in the abstract without tentatively modifying the system state ($\text{Available}$, $\text{Allocation}$, and $\text{Need}$). Consequently, when `is_safe()` determines that granting the request leaves the system without any valid execution sequence (an unsafe state), **the tentative grant must be strictly and accurately reverted**.

If rollback were omitted:
1. **Persistent Phantom Allocation**: Resources would remain credited to the requesting process even though the transaction was reported as `DENIED`.
2. **Artificial Starvation & Deadlock Vulnerability**: Depleted $\text{Available}$ pools would cause subsequent legitimate requests to be falsely denied (`DENIED: insufficient resources`).
3. **Invalid Subsequent Safety Queries**: Any following `S` queries would execute against a corrupted state, falsely diagnosing deadlock or computing invalid safe sequences.

Test 4 explicitly validates the rollback invariant: after $P_0$'s request is denied due to unsafe consequences, a subsequent `S` query confirms the state remains completely intact with sequence `SAFE: 1 3 4 0 2`.

---

## 5. Memory Safety & Strict Compliance

- **Compiler Compliance**: Compiled under `gcc -std=c99 -Wall -Wextra -Werror -pedantic` with zero warnings and zero diagnostic suppressions.
- **Sanitizers**: All 15 benchmark workloads and error scenarios executed cleanly under:
  ```bash
  gcc -std=c99 -fsanitize=address,undefined -Wall -Wextra -Werror -pedantic -o bankers bankers.c
  ```
  Resulting in 0 memory leaks, 0 heap-buffer-overflows, and 0 undefined behavior triggers.
