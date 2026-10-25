#include <stdio.h>

#define MAXP 20

int n;
char pid[MAXP][8];
int at[MAXP], bt[MAXP], pr[MAXP], ct[MAXP], fin[MAXP], trig[MAXP];

int main(void) {
    int thr, order[MAXP], oc = 0;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) scanf("%7s %d %d %d", pid[i], &at[i], &bt[i], &pr[i]);
    scanf("%d", &thr);                      // 0 = use 2 x max(BT)

    int maxbt = 0;
    for (int i = 0; i < n; i++) if (bt[i] > maxbt) maxbt = bt[i];
    if (thr <= 0) thr = 2 * maxbt;

    printf("Process  AT  BT  PR\n");
    for (int i = 0; i < n; i++) printf("%-7s %3d %3d %3d\n", pid[i], at[i], bt[i], pr[i]);
    printf("Starvation threshold = %d\n\n", thr);

    int t = 0, done = 0, decision = 0;
    while (done < n) {
        int el[MAXP], ne = 0, starved = 0;
        for (int i = 0; i < n; i++)
            if (!fin[i] && at[i] <= t) { el[ne++] = i; if (t - at[i] > thr) starved = 1; }

        if (ne == 0) {                      // CPU idle until next arrival
            int nx = -1;
            for (int i = 0; i < n; i++)
                if (!fin[i] && (nx == -1 || at[i] < nx)) nx = at[i];
            printf("t = %d: no process is ready, CPU idle until t = %d\n\n", t, nx);
            t = nx;
            continue;
        }

        decision++;
        printf("Decision %d at t = %d\n", decision, t);
        printf("  Eligible: ");
        for (int k = 0; k < ne; k++) printf("%s%s", pid[el[k]], k < ne - 1 ? ", " : "\n");

        int best = -1;
        double bestv = -1;
        for (int k = 0; k < ne; k++) {
            int i = el[k];
            int w = t - at[i];
            double rr = (double)(w + bt[i]) / bt[i];
            double weight = 1.0 + (pr[i] - 1) * 0.2;
            double arr = (double)(w + bt[i]) / (bt[i] * weight);
            int isStarving = w > thr;
            if (isStarving) trig[i] = 1;

            if (starved) {
                printf("  %s: W=%d, RR=(%d+%d)/%d = %.3f%s\n", pid[i], w, w, bt[i], bt[i], rr,
                       isStarving ? " <-- waiting > threshold" : "");
                // only the starving processes compete
                if (isStarving && (best == -1 || rr > bestv ||
                    (rr == bestv && at[i] < at[best]))) { best = i; bestv = rr; }
            } else {
                printf("  %s: W=%d, weight=%.1f, ARR=(%d+%d)/(%d*%.1f) = %.3f\n", pid[i], w,
                       weight, w, bt[i], bt[i], weight, arr);
                if (best == -1 || arr > bestv ||
                    (arr == bestv && at[i] < at[best])) { best = i; bestv = arr; }
            }
        }

        const char *why = starved ? "Anti-starvation: Highest RR"
                        : (ne == 1 ? "Only eligible process" : "Highest ARR");
        printf("  Chosen : %s (%s)\n\n", pid[best], why);

        order[oc++] = best;
        t += bt[best];
        ct[best] = t; fin[best] = 1; done++;
    }

    printf("Order of execution: ");
    for (int k = 0; k < oc; k++) printf("%s%s", pid[order[k]], k < oc - 1 ? " -> " : "\n\n");

    double sumTat = 0, sumWt = 0;
    printf("Process  AT  BT  PR   CT  TAT   WT\n");
    for (int i = 0; i < n; i++) {
        int tat = ct[i] - at[i], wt = tat - bt[i];
        sumTat += tat; sumWt += wt;
        printf("%-7s %3d %3d %3d %4d %4d %4d\n", pid[i], at[i], bt[i], pr[i], ct[i], tat, wt);
    }
    printf("Average TAT = %.2f, Average WT = %.2f\n\n", sumTat / n, sumWt / n);

    printf("Processes that triggered anti-starvation: ");
    int any = 0;
    for (int i = 0; i < n; i++) if (trig[i]) { printf("%s%s", any ? ", " : "", pid[i]); any = 1; }
    printf("%s\n", any ? "" : "None");
    return 0;
}
