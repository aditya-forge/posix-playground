/*
 * priority_non_preemptive.c
 *
 * Simulates Non-Preemptive Priority CPU Scheduling.
 * Priority convention: LOWER number = HIGHER priority (priority 1 runs before priority 5).
 * Ties are broken by arrival time, then by process name (FCFS-style).
 *
 * Input file format (processes_priority.txt):
 *   <n>
 *   <name> <arrival_time> <burst_time> <priority>   (repeated n times)
 *
 * Compile : gcc priority_non_preemptive.c -o priority_non_preemptive
 * Run     : ./priority_non_preemptive [input_file]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100

typedef struct {
    char name[10];
    int arrival_time;
    int burst_time;
    int remaining_time;   /* unused here, kept for a shared Process layout      */
    int priority;         /* lower number = higher priority                    */
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
    int first_run;
    int in_queue;         /* unused here, kept for a shared Process layout      */
    int completed;
} Process;

typedef struct {
    char name[10];
    int start;
    int end;
} GanttEntry;

/* ---------- Helper functions ---------- */

Process *read_processes(const char *filename, int *count) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Could not open %s\n", filename);
        return NULL;
    }

    int n;
    fscanf(fp, "%d", &n);

    Process *processes = malloc(sizeof(Process) * n);
    for (int i = 0; i < n; i++) {
        fscanf(fp, "%s %d %d %d",
               processes[i].name,
               &processes[i].arrival_time,
               &processes[i].burst_time,
               &processes[i].priority);
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].completion_time = 0;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].response_time = 0;
        processes[i].first_run = 0;
        processes[i].in_queue = 0;
        processes[i].completed = 0;
    }

    fclose(fp);
    *count = n;
    return processes;
}

void reset_metrics(Process *processes, int n) {
    for (int i = 0; i < n; i++) {
        processes[i].completion_time = 0;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].response_time = 0;
        processes[i].first_run = 0;
        processes[i].completed = 0;
        processes[i].remaining_time = processes[i].burst_time;
    }
}

void calculate_metrics(Process *processes, int n, double *avg_tat,
                        double *avg_wt, double *avg_rt) {
    double sum_tat = 0, sum_wt = 0, sum_rt = 0;
    for (int i = 0; i < n; i++) {
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
        sum_tat += processes[i].turnaround_time;
        sum_wt += processes[i].waiting_time;
        sum_rt += processes[i].response_time;
    }
    *avg_tat = sum_tat / n;
    *avg_wt = sum_wt / n;
    *avg_rt = sum_rt / n;
}

void display_gantt_chart(GanttEntry *gantt, int gantt_count, int context_switches) {
    printf("\nGantt Chart for Non-Preemptive Priority:\n|");
    for (int i = 0; i < gantt_count; i++)
        printf(" %s |", gantt[i].name);
    printf("\n%-4d", gantt[0].start);
    for (int i = 0; i < gantt_count; i++)
        printf("%5d", gantt[i].end);
    printf("\n\nContext Switches : %d\n", context_switches);
}

void display_results(Process *processes, int n, const char *algorithm_name,
                      GanttEntry *gantt, int gantt_count, int context_switches) {
    double avg_tat, avg_wt, avg_rt;
    calculate_metrics(processes, n, &avg_tat, &avg_wt, &avg_rt);

    printf("==========================================\n");
    printf("%s\n", algorithm_name);
    printf("==========================================\n");
    printf("Process | Arrival | Burst | Priority | Completion | Turnaround | Waiting | Response\n");
    printf("--------|---------|-------|----------|------------|------------|---------|---------\n");
    for (int i = 0; i < n; i++) {
        printf("%-7s | %-7d | %-5d | %-8d | %-10d | %-10d | %-7d | %-8d\n",
               processes[i].name, processes[i].arrival_time, processes[i].burst_time,
               processes[i].priority, processes[i].completion_time,
               processes[i].turnaround_time, processes[i].waiting_time,
               processes[i].response_time);
    }

    printf("\nAverage Turnaround Time : %.2f\n", avg_tat);
    printf("Average Waiting Time    : %.2f\n", avg_wt);
    printf("Average Response Time   : %.2f\n", avg_rt);
    printf("Total Context Switches  : %d\n", context_switches);

    display_gantt_chart(gantt, gantt_count, context_switches);
}

/* ---------- Scheduling ---------- */

void run_non_preemptive_priority(Process *p, int n) {
    int current_time = 0;
    int completed = 0;
    int context_switches = 0;
    int last_process = -1;

    GanttEntry gantt[MAX_PROCESSES];
    int gantt_count = 0;

    while (completed < n) {
        int selected = -1;
        for (int i = 0; i < n; i++) {
            if (p[i].completed || p[i].arrival_time > current_time)
                continue;
            if (selected == -1 ||
                p[i].priority < p[selected].priority ||
                (p[i].priority == p[selected].priority &&
                 p[i].arrival_time < p[selected].arrival_time) ||
                (p[i].priority == p[selected].priority &&
                 p[i].arrival_time == p[selected].arrival_time &&
                 strcmp(p[i].name, p[selected].name) < 0)) {
                selected = i;
            }
        }

        if (selected == -1) {
            current_time++;
            continue;
        }

        if (last_process != -1 && last_process != selected)
            context_switches++;

        strcpy(gantt[gantt_count].name, p[selected].name);
        gantt[gantt_count].start = current_time;

        p[selected].response_time = current_time - p[selected].arrival_time;
        current_time += p[selected].burst_time;
        p[selected].completion_time = current_time;

        gantt[gantt_count].end = current_time;
        gantt_count++;

        p[selected].completed = 1;
        completed++;
        last_process = selected;
    }

    display_results(p, n, "NON-PREEMPTIVE PRIORITY SCHEDULING RESULTS",
                     gantt, gantt_count, context_switches);
}

int main(int argc, char *argv[]) {
    const char *filename = (argc > 1) ? argv[1] : "processes_priority.txt";
    int n;

    Process *p = read_processes(filename, &n);
    if (p == NULL) return 1;

    run_non_preemptive_priority(p, n);

    free(p);
    return 0;
}
