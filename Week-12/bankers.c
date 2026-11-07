/*
 * bankers.c  --  Banker's Algorithm simulator (deadlock avoidance)
 *
 * Reads a system state (Available, Allocation, Max) from stdin, then answers
 * a list of queries:
 *     S                      safety check
 *     R i r0 r1 ... r(M-1)   resource request from process i
 *
 * Build : gcc -std=c99 -Wall -Wextra -Werror -pedantic -o bankers bankers.c
 * Run   : ./bankers < input.txt
 *
 * Helpers never call exit(); they return status codes and main() decides
 * what to do, so every error path can free its memory.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N   20
#define MAX_M   10
#define MAX_Q   100
#define MAX_VAL 1000

/* status codes returned by try_request() */
#define REQ_GRANTED      0
#define REQ_EXCEEDS_MAX  1
#define REQ_INSUFFICIENT 2
#define REQ_UNSAFE       3
#define REQ_ERROR       (-1)

/* ------------------------------------------------------------------ */
/* Matrix helpers                                                      */
/* ------------------------------------------------------------------ */

void free_matrix(int **m, int rows)
{
    if (m == NULL) return;
    for (int r = 0; r < rows; r++) free(m[r]);   /* free(NULL) is harmless */
    free(m);
}

/* rows x cols matrix of zeros, or NULL if any allocation fails */
int **alloc_matrix(int rows, int cols)
{
    int **mat = calloc((size_t)rows, sizeof(int *));
    if (mat == NULL) return NULL;
    for (int r = 0; r < rows; r++) {
        mat[r] = calloc((size_t)cols, sizeof(int));
        if (mat[r] == NULL) {
            free_matrix(mat, rows);
            return NULL;
        }
    }
    return mat;
}

/* ------------------------------------------------------------------ */
/* Core algorithms                                                     */
/* ------------------------------------------------------------------ */

/*
 * Need = Max - Allocation.
 * Returns 0 on success. If some Allocation[i][j] > Max[i][j] the state is
 * invalid and the function returns (i * m + j) + 1 so main() can report
 * exactly which entry is wrong.
 */
int compute_need(int **alloc, int **max, int **need, int n, int m)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (alloc[i][j] > max[i][j]) return i * m + j + 1;
            need[i][j] = max[i][j] - alloc[i][j];
        }
    }
    return 0;
}

/*
 * Safety algorithm.
 * Work starts as a copy of Available. Repeatedly scan the processes in
 * index order; an unfinished process whose Need <= Work can run to
 * completion, so it returns its Allocation to Work and is appended to the
 * safe sequence. Stop when everything has finished (safe) or a full pass
 * finishes nobody (unsafe).
 *
 * Returns 1 = safe (sequence stored in seq_out if it is not NULL),
 *         0 = unsafe, -1 = out of memory.
 * The state (alloc, need, avail) is never modified.
 */
int is_safe(int **alloc, int **need, int *avail, int n, int m, int *seq_out)
{
    int *work = malloc((size_t)m * sizeof(int));
    int *finish = calloc((size_t)n, sizeof(int));
    int count = 0, progress = 1;

    if (work == NULL || finish == NULL) {
        free(work);
        free(finish);
        return -1;
    }
    memcpy(work, avail, (size_t)m * sizeof(int));

    while (count < n && progress) {
        progress = 0;
        for (int i = 0; i < n; i++) {
            int can_run = 1;
            if (finish[i]) continue;
            for (int j = 0; j < m; j++) {
                if (need[i][j] > work[j]) { can_run = 0; break; }
            }
            if (can_run) {
                for (int j = 0; j < m; j++) work[j] += alloc[i][j];
                finish[i] = 1;
                if (seq_out != NULL) seq_out[count] = i;
                count++;
                progress = 1;
            }
        }
    }

    free(work);
    free(finish);
    return count == n;
}

/*
 * Resource-request algorithm for process i.
 *   1. req > Need[i]      -> REQ_EXCEEDS_MAX   (checked first)
 *   2. req > Available    -> REQ_INSUFFICIENT
 *   3. otherwise pretend to grant, run the safety check:
 *        safe   -> keep the new state, REQ_GRANTED
 *        unsafe -> undo the pretend grant exactly, REQ_UNSAFE
 * *granted is set to 1 only when the request is committed.
 * Returns REQ_ERROR on out-of-memory (state is restored first).
 */
int try_request(int **alloc, int **need, int *avail, int i, const int *req,
                int n, int m, int *granted)
{
    int safe;

    *granted = 0;
    for (int j = 0; j < m; j++)
        if (req[j] > need[i][j]) return REQ_EXCEEDS_MAX;
    for (int j = 0; j < m; j++)
        if (req[j] > avail[j]) return REQ_INSUFFICIENT;

    /* tentative grant */
    for (int j = 0; j < m; j++) {
        avail[j]    -= req[j];
        alloc[i][j] += req[j];
        need[i][j]  -= req[j];
    }

    safe = is_safe(alloc, need, avail, n, m, NULL);
    if (safe == 1) {
        *granted = 1;
        return REQ_GRANTED;
    }

    /* rollback: put the state back exactly as it was */
    for (int j = 0; j < m; j++) {
        avail[j]    += req[j];
        alloc[i][j] -= req[j];
        need[i][j]  += req[j];
    }
    return (safe == 0) ? REQ_UNSAFE : REQ_ERROR;
}

/* ------------------------------------------------------------------ */
/* Input parsing                                                       */
/* ------------------------------------------------------------------ */

/* Reads one whole-token integer in [lo, hi]; prints an error and returns -1 on failure. */
static int read_int(int *out, long lo, long hi, const char *what)
{
    char tok[64];
    char *end;
    long v;

    if (scanf("%63s", tok) != 1) {
        fprintf(stderr, "Error: input ended early, missing %s\n", what);
        return -1;
    }
    errno = 0;
    v = strtol(tok, &end, 10);
    if (*end != '\0' || errno == ERANGE) {
        fprintf(stderr, "Error: '%s' is not a valid integer (%s)\n", tok, what);
        return -1;
    }
    if (v < lo || v > hi) {
        fprintf(stderr, "Error: %s = %ld is out of range [%ld, %ld]\n", what, v, lo, hi);
        return -1;
    }
    *out = (int)v;
    return 0;
}

/* Reads an n x m matrix of values in [0, MAX_VAL]. */
static int read_matrix(int **mat, int n, int m, const char *name)
{
    char what[64];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            sprintf(what, "%s[%d][%d]", name, i, j);
            if (read_int(&mat[i][j], 0, MAX_VAL, what) < 0) return -1;
        }
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* main                                                                */
/* ------------------------------------------------------------------ */

int main(void)
{
    int n = 0, m = 0, q = 0;
    int **alloc = NULL, **max = NULL, **need = NULL;
    int *avail = NULL, *seq = NULL;
    int qtype[MAX_Q];              /* 0 = safety check, 1 = request */
    int qproc[MAX_Q];
    int qreq[MAX_Q][MAX_M];
    int status = EXIT_FAILURE;
    char extra[64];
    int bad;

    /* ---- read and validate everything before printing anything ---- */
    if (read_int(&n, 1, MAX_N, "N") < 0) goto cleanup;
    if (read_int(&m, 1, MAX_M, "M") < 0) goto cleanup;

    alloc = alloc_matrix(n, m);
    max   = alloc_matrix(n, m);
    need  = alloc_matrix(n, m);
    avail = calloc((size_t)m, sizeof(int));
    seq   = calloc((size_t)n, sizeof(int));
    if (!alloc || !max || !need || !avail || !seq) {
        fprintf(stderr, "Error: out of memory\n");
        goto cleanup;
    }

    for (int j = 0; j < m; j++) {
        char what[64];
        sprintf(what, "Available[%d]", j);
        if (read_int(&avail[j], 0, MAX_VAL, what) < 0) goto cleanup;
    }
    if (read_matrix(alloc, n, m, "Allocation") < 0) goto cleanup;
    if (read_matrix(max, n, m, "Max") < 0) goto cleanup;

    bad = compute_need(alloc, max, need, n, m);
    if (bad != 0) {
        fprintf(stderr, "Error: Allocation[%d][%d] is greater than Max[%d][%d]\n",
                (bad - 1) / m, (bad - 1) % m, (bad - 1) / m, (bad - 1) % m);
        goto cleanup;
    }

    if (read_int(&q, 0, MAX_Q, "Q") < 0) goto cleanup;
    for (int k = 0; k < q; k++) {
        char tok[64];
        if (scanf("%63s", tok) != 1) {
            fprintf(stderr, "Error: input ended early, expected query %d of %d\n", k + 1, q);
            goto cleanup;
        }
        if (strcmp(tok, "S") == 0) {
            qtype[k] = 0;
        } else if (strcmp(tok, "R") == 0) {
            qtype[k] = 1;
            if (read_int(&qproc[k], 0, n - 1, "process index of request") < 0) goto cleanup;
            for (int j = 0; j < m; j++) {
                if (read_int(&qreq[k][j], 0, MAX_VAL, "request amount") < 0) goto cleanup;
            }
        } else {
            fprintf(stderr, "Error: unknown query '%s' (expected S or R)\n", tok);
            goto cleanup;
        }
    }
    if (scanf("%63s", extra) == 1) {
        fprintf(stderr, "Error: unexpected extra input '%s' after the last query\n", extra);
        goto cleanup;
    }

    /* ---- answer the queries ---- */
    for (int k = 0; k < q; k++) {
        if (qtype[k] == 0) {
            int r = is_safe(alloc, need, avail, n, m, seq);
            if (r < 0) { fprintf(stderr, "Error: out of memory\n"); goto cleanup; }
            if (r == 1) {
                printf("SAFE:");
                for (int i = 0; i < n; i++) printf(" %d", seq[i]);
                printf("\n");
            } else {
                printf("UNSAFE\n");
            }
        } else {
            int granted;
            int r = try_request(alloc, need, avail, qproc[k], qreq[k], n, m, &granted);
            if (r == REQ_ERROR) { fprintf(stderr, "Error: out of memory\n"); goto cleanup; }
            if (r == REQ_GRANTED)           printf("GRANTED\n");
            else if (r == REQ_EXCEEDS_MAX)  printf("DENIED: exceeds maximum claim\n");
            else if (r == REQ_INSUFFICIENT) printf("DENIED: insufficient resources\n");
            else                            printf("DENIED: would lead to unsafe state\n");
        }
        printf("\n");
    }
    status = EXIT_SUCCESS;

cleanup:
    free_matrix(alloc, n);
    free_matrix(max, n);
    free_matrix(need, n);
    free(avail);
    free(seq);
    return status;
}
