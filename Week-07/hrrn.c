/*
 * hrrn.c
 *
 * Highest Response Ratio Next (HRRN) CPU Scheduling
 * ---------------------------------------------------
 * HRRN is a non-preemptive scheduling algorithm. At every scheduling
 * decision point it looks at every process that has already arrived but
 * not yet run, computes each one's Response Ratio, and dispatches the
 * process with the highest ratio to run to completion.
 *
 *      Response Ratio = (Waiting Time + Burst Time) / Burst Time
 *                      = 1 + (Waiting Time / Burst Time)
 *
 * A process that has waited a long time accumulates a growing Waiting
 * Time, which pushes its ratio up over time even if its burst is long
 * (this is the "aging" effect that prevents starvation). A short burst
 * time also inflates the ratio, so HRRN naturally favors short jobs the
 * way SJF does, but without permanently starving long ones.
 *
 * Tie-breaking, when two ready processes have the same response ratio:
 *   1. Prefer the smaller burst time.
 *   2. If still tied, prefer the smaller arrival time.
 *   3. If still tied, prefer the smaller PID.
 *
 * Input file format (processes_hrrn.txt):
 *   <n>
 *   <name> <arrival_time> <burst_time>   (repeated n times)
 *
 * Compile : gcc hrrn.c -o hrrn
 * Run     : ./hrrn [input_file]
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 100

typedef struct {
    char name[20];
    int pid;
    int arrival_time;
    int burst_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    int response_time;
    int start_time;
    int completed;
} Process;

/*
 * response_ratio()
 * -----------------
 * Computes the Response Ratio of process p if it were dispatched right
 * now (at current_time). Waiting time is how long the process has sat
 * in the ready queue so far; dividing (waiting + burst) by burst turns
 * that raw wait into a ratio that both short jobs and long-waiting jobs
 * can win on.
 */
double response_ratio(Process *p, int current_time)
{
    int waiting_time = current_time - p->arrival_time;
    return (double)(waiting_time + p->burst_time) / p->burst_time;
}

/*
 * select_process()
 * -----------------
 * Scans every process that has arrived (arrival_time <= current_time)
 * and is not yet completed, and returns the index of the one with the
 * highest response ratio. Ties are resolved, in order, by: smaller
 * burst time, then smaller arrival time, then smaller PID. Returns -1
 * if no process is currently available to run (meaning the CPU is idle
 * until the next arrival).
 */
int select_process(Process p[], int n, int current_time)
{
    int selected = -1;
    double highest_rr = -1.0;

    for (int i = 0; i < n; i++) {
        if (p[i].completed == 0 && p[i].arrival_time <= current_time) {
            double rr = response_ratio(&p[i], current_time);

            if (selected == -1 || rr > highest_rr) {
                selected = i;
                highest_rr = rr;
            } else if (rr == highest_rr) {
                if (p[i].burst_time < p[selected].burst_time) {
                    selected = i;
                } else if (p[i].burst_time == p[selected].burst_time) {
                    if (p[i].arrival_time < p[selected].arrival_time) {
                        selected = i;
                    } else if (p[i].arrival_time == p[selected].arrival_time &&
                               p[i].pid < p[selected].pid) {
                        selected = i;
                    }
                }
            }
        }
    }
    return selected;
}

/*
 * print_gantt_chart()
 * ---------------------
 * Prints the execution order as a simple text Gantt chart, with the
 * start time of the first process followed by the completion time of
 * every process in the order they ran.
 */
void print_gantt_chart(Process p[], int order[], int count)
{
    printf("\nGantt Chart :\n\n");
    printf("|");
    for (int i = 0; i < count; i++) {
        printf(" %s |", p[order[i]].name);
    }
    printf("\n");
    printf("%d", p[order[0]].start_time);
    for (int i = 0; i < count; i++) {
        printf(" %d", p[order[i]].completion_time);
    }
    printf("\n");
}

/*
 * display_results()
 * -------------------
 * Prints the PID / AT / BT / CT / TAT / WT / RT table and the average
 * turnaround, waiting, and response times, exactly as required by the
 * assignment's output format.
 */
void display_results(Process p[], int n)
{
    double total_tat = 0;
    double total_wt = 0;
    double total_rt = 0;

    printf("\n");
    printf("PID   AT   BT   CT   TAT   WT   RT\n");
    for (int i = 0; i < n; i++) {
        printf("%-5d %-4d %-4d %-4d %-5d %-4d %-4d\n",
               p[i].pid,
               p[i].arrival_time,
               p[i].burst_time,
               p[i].completion_time,
               p[i].turnaround_time,
               p[i].waiting_time,
               p[i].response_time);

        total_tat += p[i].turnaround_time;
        total_wt += p[i].waiting_time;
        total_rt += p[i].response_time;
    }

    printf("\n");
    printf("Average Turnaround Time : %.2f\n", total_tat / n);
    printf("Average Waiting Time    : %.2f\n", total_wt / n);
    printf("Average Response Time   : %.2f\n", total_rt / n);
}

/*
 * hrrn()
 * -------
 * Runs the HRRN simulation to completion. If no process is ready at the
 * current time, the clock jumps forward to the next arrival instead of
 * ticking one unit at a time (there is nothing useful to do in between).
 * Since HRRN is non-preemptive, Response Time and Waiting Time are equal
 * for every process (both measure the gap between arrival and the single
 * point the process starts running).
 */
void hrrn(Process p[], int n)
{
    int current_time = 0;
    int completed = 0;
    int order[MAX_PROCESSES];
    int order_count = 0;

    while (completed < n) {
        int index = select_process(p, n, current_time);

        if (index == -1) {
            /* CPU is idle: jump straight to the next arrival */
            int next_arrival = -1;
            for (int i = 0; i < n; i++) {
                if (p[i].completed == 0) {
                    if (next_arrival == -1 || p[i].arrival_time < next_arrival) {
                        next_arrival = p[i].arrival_time;
                    }
                }
            }
            current_time = next_arrival;
            continue;
        }

        order[order_count] = index;
        order_count++;

        p[index].start_time = current_time;
        p[index].response_time = p[index].start_time - p[index].arrival_time;
        current_time += p[index].burst_time;
        p[index].completion_time = current_time;
        p[index].turnaround_time = p[index].completion_time - p[index].arrival_time;
        p[index].waiting_time = p[index].turnaround_time - p[index].burst_time;
        p[index].completed = 1;
        completed++;
    }

    print_gantt_chart(p, order, order_count);
    display_results(p, n);
}

int main(int argc, char *argv[])
{
    Process p[MAX_PROCESSES];
    int n;
    FILE *file;

    const char *filename = (argc > 1) ? argv[1] : "processes_hrrn.txt";

    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open %s\n", filename);
        return 1;
    }

    if (fscanf(file, "%d", &n) != 1) {
        printf("Error: Invalid input file (could not read process count)\n");
        fclose(file);
        return 1;
    }

    if (n <= 0 || n > MAX_PROCESSES) {
        printf("Error: Invalid number of processes (%d). Must be between 1 and %d.\n",
               n, MAX_PROCESSES);
        fclose(file);
        return 1;
    }

    for (int i = 0; i < n; i++) {
        if (fscanf(file, "%19s %d %d",
                   p[i].name, &p[i].arrival_time, &p[i].burst_time) != 3) {
            printf("Error: Invalid or missing data for process %d\n", i + 1);
            fclose(file);
            return 1;
        }

        /* Reject impossible/negative values instead of silently miscomputing. */
        if (p[i].arrival_time < 0) {
            printf("Error: Process %s has a negative arrival time (%d)\n",
                   p[i].name, p[i].arrival_time);
            fclose(file);
            return 1;
        }
        if (p[i].burst_time <= 0) {
            printf("Error: Process %s has a non-positive burst time (%d)\n",
                   p[i].name, p[i].burst_time);
            fclose(file);
            return 1;
        }

        p[i].pid = i + 1;
        p[i].completion_time = 0;
        p[i].turnaround_time = 0;
        p[i].waiting_time = 0;
        p[i].response_time = 0;
        p[i].start_time = 0;
        p[i].completed = 0;
    }
    fclose(file);

    hrrn(p, n);
    return 0;
}
