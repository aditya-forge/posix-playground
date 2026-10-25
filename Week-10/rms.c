#include <stdio.h>
#include <math.h>
#include <string.h>

#define MAXT 10
#define MAXJ 3000

int n;
char id[MAXT][8];
int Cc[MAXT], Tp[MAXT], ph[MAXT];

typedef struct { int task, num, rel, rem, dl, fin, missed, remAtMiss; } Job;
Job jobs[MAXJ];
int nj = 0;

long gcd(long a, long b) { return b ? gcd(b, a % b) : a; }

int main(void) {
    scanf("%d", &n);
    for (int i = 0; i < n; i++) scanf("%7s %d %d %d", id[i], &Cc[i], &Tp[i], &ph[i]);

    // priority order: shorter period = higher priority (stable sort)
    for (int i = 1; i < n; i++) {
        char s[8]; int c = Cc[i], p = Tp[i], f = ph[i], j = i - 1;
        strcpy(s, id[i]);
        while (j >= 0 && Tp[j] > p) {
            strcpy(id[j + 1], id[j]); Cc[j + 1] = Cc[j]; Tp[j + 1] = Tp[j]; ph[j + 1] = ph[j];
            j--;
        }
        strcpy(id[j + 1], s); Cc[j + 1] = c; Tp[j + 1] = p; ph[j + 1] = f;
    }

    printf("Task   C    T  Phase (listed in priority order)\n");
    for (int i = 0; i < n; i++) printf("%-5s %2d %4d %6d\n", id[i], Cc[i], Tp[i], ph[i]);

    // ---------- Step 1: schedulability analysis ----------
    double U = 0;
    for (int i = 0; i < n; i++) U += (double)Cc[i] / Tp[i];
    double bound = n * (pow(2.0, 1.0 / n) - 1);

    printf("\n--- Schedulability analysis ---\n");
    printf("Utilization U = ");
    for (int i = 0; i < n; i++) printf("%d/%d%s", Cc[i], Tp[i], i < n - 1 ? " + " : "");
    printf(" = %.4f\n", U);
    printf("Liu-Layland bound n(2^(1/n)-1) = %.4f\n", bound);
    if (U > 1.0) printf("Utilization test: U > 1, definitely UNSCHEDULABLE\n");
    else if (U <= bound) printf("Utilization test: U <= bound, schedulability GUARANTEED\n");
    else printf("Utilization test: bound < U <= 1, NOT GUARANTEED (inconclusive, check RTA)\n");

    printf("\nResponse-time analysis:\n");
    int R[MAXT], allOk = 1;
    for (int i = 0; i < n; i++) {
        int r = Cc[i];
        printf("  %s: R = %d", id[i], r);
        for (;;) {
            int next = Cc[i];
            for (int j = 0; j < i; j++) next += ((r + Tp[j] - 1) / Tp[j]) * Cc[j];
            printf(" -> %d", next);
            if (next == r) break;
            r = next;
            if (r > Tp[i]) break;           // deadline miss
        }
        R[i] = r;
        int ok = r <= Tp[i];
        if (!ok) allOk = 0;
        printf(" => R = %d, deadline = %d : %s\n", r, Tp[i], ok ? "schedulable" : "MISSES DEADLINE");
    }
    printf("RTA conclusion: task set is %s\n", allOk ? "SCHEDULABLE" : "NOT schedulable");

    // ---------- Step 2: timeline simulation over one hyperperiod ----------
    long H = Tp[0], maxph = 0;
    for (int i = 1; i < n; i++) H = H / gcd(H, Tp[i]) * Tp[i];
    for (int i = 0; i < n; i++) if (ph[i] > maxph) maxph = ph[i];
    int simLen = (int)(H + maxph);

    for (int i = 0; i < n; i++) {
        int k = 1;
        for (int r = ph[i]; r < simLen; r += Tp[i], k++) {
            Job *j = &jobs[nj++];
            j->task = i; j->num = k; j->rel = r; j->rem = Cc[i];
            j->dl = r + Tp[i]; j->fin = -1; j->missed = 0;
        }
    }

    static char lab[20000][12];
    static int brk[20000];
    int missOrder[MAXJ], nm = 0;

    for (int t = 0; t < simLen; t++) {
        for (int k = 0; k < nj; k++)        // deadline check at time t
            if (!jobs[k].missed && jobs[k].rem > 0 && jobs[k].dl <= t) {
                jobs[k].missed = 1; jobs[k].remAtMiss = jobs[k].rem; missOrder[nm++] = k;
            }
        int pick = -1;
        for (int k = 0; k < nj; k++)
            if (jobs[k].rel <= t && jobs[k].rem > 0 &&
                (pick == -1 || jobs[k].task < jobs[pick].task)) pick = k;

        if (pick == -1) strcpy(lab[t], "IDLE");
        else sprintf(lab[t], "%s.%d", id[jobs[pick].task], jobs[pick].num);
        brk[t] = t == 0 || strcmp(lab[t], lab[t - 1]) != 0;
        if (pick != -1 && --jobs[pick].rem == 0) jobs[pick].fin = t + 1;
    }
    for (int k = 0; k < nj; k++)            // deadlines falling exactly at the end
        if (!jobs[k].missed && jobs[k].rem > 0 && jobs[k].dl <= simLen) {
            jobs[k].missed = 1; jobs[k].remAtMiss = jobs[k].rem; missOrder[nm++] = k;
        }

    printf("\n--- Simulation over hyperperiod H = %ld (simulated %d units) ---\n", H, simLen);
    printf("Timeline (start-job-end):\n");
    int col = 0;
    for (int u = 0; u < simLen; u++)
        if (brk[u]) {
            printf("%d-%s-", u, lab[u]);
            if (++col % 8 == 0) printf("\n");
        }
    printf("%d\n", simLen);

    printf("\nMax observed response time per task (simulation vs RTA):\n");
    for (int i = 0; i < n; i++) {
        int mx = 0;
        for (int k = 0; k < nj; k++)
            if (jobs[k].task == i && jobs[k].fin > 0 && jobs[k].fin - jobs[k].rel > mx)
                mx = jobs[k].fin - jobs[k].rel;
        printf("  %s: observed %d, RTA %d%s\n", id[i], mx, R[i],
               R[i] > Tp[i] ? " (RTA stopped once R exceeded the deadline)" : "");
    }

    printf("\nMissed deadlines:\n");
    if (nm == 0) printf("  None\n");
    for (int m = 0; m < nm; m++) {
        Job *j = &jobs[missOrder[m]];
        printf("  %s job %d (rel. %d) missed deadline t=%d, %d unit(s) left, ",
               id[j->task], j->num, j->rel, j->dl, j->remAtMiss);
        if (j->fin > 0) printf("finished late at t=%d\n", j->fin);
        else printf("unfinished at end\n");
    }
    return 0;
}
