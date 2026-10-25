#include <stdio.h>
#include <string.h>

#define MAXP 20
#define MAXT 5000

int n, C;
char pid[MAXP][8];
int at[MAXP], bt[MAXP], rem[MAXP], ct[MAXP];
char lab[MAXT][8];   // what the CPU did in each time unit
int brk[MAXT];       // 1 if a new Gantt segment starts at this unit
int running = -1;    // process on the CPU (-1 = none)

// ready = arrived, unfinished and not on the CPU
int pick_shortest(int t) {
    int best = -1;
    for (int i = 0; i < n; i++) {
        if (at[i] > t || rem[i] == 0 || i == running) continue;
        if (best == -1 || rem[i] < rem[best] ||
            (rem[i] == rem[best] && at[i] < at[best]))
            best = i;
    }
    return best;
}

void mark(int t, const char *name, int newSeg) {
    strcpy(lab[t], name);
    brk[t] = newSeg || t == 0 || strcmp(lab[t - 1], name) != 0;
}

int main(void) {
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%7s %d %d", pid[i], &at[i], &bt[i]);
        rem[i] = bt[i];
    }
    scanf("%d", &C);

    printf("Process  AT  BT\n");
    for (int i = 0; i < n; i++) printf("%-7s %3d %3d\n", pid[i], at[i], bt[i]);
    printf("Context switch overhead C = %d\n\n", C);

    int t = 0, done = 0, csLeft = 0, csCount = 0, csTime = 0, busy = 0;
    while (done < n) {
        if (csLeft > 0) {                          // CPU is switching
            mark(t, "CS", csLeft == C);
            csLeft--; csTime++; t++;
            if (csLeft == 0) running = pick_shortest(t);   // dispatch when the switch ends
            continue;
        }
        int best = pick_shortest(t);
        if (running == -1) {                       // CPU free
            if (best == -1) {                      // nothing to run: idle
                mark(t, "IDLE", 0);
                t++;
                continue;
            }
            csLeft = C; csCount++;                 // idle/finish -> CS
            continue;
        }
        // preemption test: remaining(curr) > remaining(new) + C
        if (best != -1 && rem[running] > rem[best] + C) {
            running = -1; csLeft = C; csCount++;   // preempted -> CS
            continue;
        }
        mark(t, pid[running], 0);                  // run for one unit
        rem[running]--; busy++; t++;
        if (rem[running] == 0) { ct[running] = t; done++; running = -1; }
    }

    printf("Gantt Chart:\n");
    for (int u = 0; u < t; u++)
        if (brk[u]) printf("%d-%s-", u, lab[u]);
    printf("%d\n\n", t);

    double sumTat = 0, sumWt = 0;
    printf("Process  AT  BT   CT  TAT   WT\n");
    for (int i = 0; i < n; i++) {
        int tat = ct[i] - at[i], wt = tat - bt[i];
        sumTat += tat; sumWt += wt;
        printf("%-7s %3d %3d %4d %4d %4d\n", pid[i], at[i], bt[i], ct[i], tat, wt);
    }
    printf("Average TAT = %.2f, Average WT = %.2f\n\n", sumTat / n, sumWt / n);
    printf("Total context switches = %d\n", csCount);
    printf("Total time on switches = %d\n", csTime);
    printf("Total busy time = %d\n", busy);
    printf("Total timeline length = %d\n", t);
    printf("CPU efficiency = %d / %d x 100 = %.2f%%\n", busy, t, 100.0 * busy / t);
    return 0;
}
