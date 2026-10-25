#include <stdio.h>
#include <string.h>

#define MAXP 20
#define MAXQ 5000
#define MAXS 5000

int n;
char pid[MAXP][8];
int at[MAXP], bt[MAXP], rem[MAXP], ct[MAXP], admitted[MAXP];
int q[MAXQ], head = 0, tail = 0;                    // ready queue
int sgS[MAXS], sgE[MAXS], sgP[MAXS], sgN = 0;       // Gantt segments (-1 = idle)
// cycle table
int cycTq[MAXS], cycN[MAXS], cycMem[MAXS][MAXP], cycRem[MAXS][MAXP];

void seg(int s, int e, int p) {
    if (sgN > 0 && sgP[sgN - 1] == p && sgE[sgN - 1] == s) { sgE[sgN - 1] = e; return; }
    sgS[sgN] = s; sgE[sgN] = e; sgP[sgN] = p; sgN++;
}

// put every process that has arrived by time t at the back of the queue
void admit(int t) {
    for (;;) {
        int b = -1;
        for (int i = 0; i < n; i++)
            if (!admitted[i] && at[i] <= t && (b == -1 || at[i] < at[b])) b = i;
        if (b == -1) return;
        admitted[b] = 1;
        q[tail++] = b;
    }
}

int median_of_queue(void) {
    int a[MAXP], m = tail - head;
    for (int k = 0; k < m; k++) a[k] = rem[q[head + k]];
    for (int i = 0; i < m; i++)                     // sort
        for (int j = i + 1; j < m; j++)
            if (a[j] < a[i]) { int x = a[i]; a[i] = a[j]; a[j] = x; }
    if (m % 2) return a[m / 2];
    return (a[m / 2 - 1] + a[m / 2] + 1) / 2;       // even count: mean of middle two, rounded up
}

int main(void) {
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%7s %d %d", pid[i], &at[i], &bt[i]);
        rem[i] = bt[i];
    }
    printf("Process  AT  BT\n");
    for (int i = 0; i < n; i++) printf("%-7s %3d %3d\n", pid[i], at[i], bt[i]);
    printf("\n");

    int t = 0, done = 0, cycles = 0, prevTq = 1, idled = 0;
    while (done < n) {
        admit(t);
        if (head == tail) {                         // queue empty: wait for next arrival
            int nx = -1;
            for (int i = 0; i < n; i++)
                if (!admitted[i] && (nx == -1 || at[i] < nx)) nx = at[i];
            seg(t, nx, -1);
            t = nx; idled = 1;
            continue;
        }
        int N = tail - head, tq;
        if (cycles == 0) tq = median_of_queue();    // first cycle: median burst
        else if (idled) tq = prevTq;                // queue was empty: keep previous TQ
        else {                                      // ceil(average / 2)
            int sum = 0;
            for (int k = head; k < tail; k++) sum += rem[q[k]];
            tq = (sum + 2 * N - 1) / (2 * N);
        }
        if (tq < 1) tq = 1;
        idled = 0;

        cycTq[cycles] = tq; cycN[cycles] = N;
        for (int k = 0; k < N; k++) {
            cycMem[cycles][k] = q[head + k];
            cycRem[cycles][k] = rem[q[head + k]];
        }
        cycles++;

        for (int k = 0; k < N; k++) {               // one pass over the N processes
            int i = q[head++];
            int run = rem[i] < tq ? rem[i] : tq;
            seg(t, t + run, i);
            t += run; rem[i] -= run;
            admit(t);                               // arrivals go behind the queue
            if (rem[i] > 0) q[tail++] = i;          // preempted: back of the queue
            else { ct[i] = t; done++; }
        }
        prevTq = tq;
    }

    printf("Gantt Chart:\n");
    for (int s = 0; s < sgN; s++)
        printf("%d-%s-", sgS[s], sgP[s] < 0 ? "IDLE" : pid[sgP[s]]);
    printf("%d\n\n", t);

    printf("Cycle | TQ | Processes in RQ      | Remaining times\n");
    for (int c = 0; c < cycles; c++) {
        char names[100] = "[", rems[100] = "[";
        for (int k = 0; k < cycN[c]; k++) {
            char tmp[16];
            strcat(names, pid[cycMem[c][k]]);
            sprintf(tmp, "%d", cycRem[c][k]); strcat(rems, tmp);
            if (k < cycN[c] - 1) { strcat(names, ", "); strcat(rems, ", "); }
        }
        strcat(names, "]"); strcat(rems, "]");
        printf("%5d | %2d | %-20s | %s\n", c + 1, cycTq[c], names, rems);
    }

    double sumTat = 0, sumWt = 0;
    printf("\nProcess  AT  BT   CT  TAT   WT\n");
    for (int i = 0; i < n; i++) {
        int tat = ct[i] - at[i], wt = tat - bt[i];
        sumTat += tat; sumWt += wt;
        printf("%-7s %3d %3d %4d %4d %4d\n", pid[i], at[i], bt[i], ct[i], tat, wt);
    }
    printf("Average TAT = %.2f, Average WT = %.2f\n", sumTat / n, sumWt / n);
    return 0;
}
