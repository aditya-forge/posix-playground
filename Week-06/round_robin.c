/*
 * round_robin.c
 *
 * Simulates Round Robin CPU Scheduling with a configurable time quantum.
 * Each ready process gets at most one quantum of CPU time per turn; if it
 * doesn't finish, it is preempted and re-enqueued after any processes that
 * arrived during its turn (newly arrived processes join the queue before
 * the just-preempted process, per the assignment spec).
 *
 * Input file format (processes_rr.txt):
 *   <n>
 *   <name> <arrival_time> <burst_time>   (repeated n times)
 *   <time_quantum>
 *
 * Compile : gcc round_robin.c -o round_robin
 * Run     : ./round_robin [input_file] [override_quantum]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100
#define MAX_GANTT (MAX_PROCESSES * 200)

typedef struct {
    char name[10];
    int arrival_time;
    int burst_time;
    int remaining_time;
    int priority;          /* unused in RR, kept for a shared Process layout */
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
    int first_run;
    int in_queue;
    int completed;
} Process;

typedef struct {
    int process_indices[MAX_PROCESSES];
    int front;
    int rear;
    int count;
} Queue;

typedef struct {
    char name[10];
    int start;
    int end;
} GanttEntry;

/* ---------- Queue operations ---------- */

void init_queue(Queue *q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
}

int is_empty(Queue *q) {
    return q->count == 0;
}

int is_full(Queue *q) {
    return q->count == MAX_PROCESSES;
}

void enqueue(Queue *q, int process_index) {
    if (is_full(q)) return;
    q->process_indices[q->rear] = process_index;
    q->rear = (q->rear + 1) % MAX_PROCESSES;
    q->count++;
}

int dequeue(Queue *q) {
    if (is_empty(q)) return -1;
    int value = q->process_indices[q->front];
    q->front = (q->front + 1) % MAX_PROCESSES;
    q->count--;
    return value;
}

/* ---------- Helper functions ---------- */

Process *read_processes(const char *filename, int *count, int *quantum) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Could not open %s\n", filename);
        return NULL;
    }

    int n;
    fscanf(fp, "%d", &n);

    Process *processes = malloc(sizeof(Process) * n);
    for (int i = 0; i < n; i++) {
        fscanf(fp, "%s %d %d",
               processes[i].name,
               &processes[i].arrival_time,
               &processes[i].burst_time);
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].completion_time = 0;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].response_time = 0;
        processes[i].first_run = 0;
        processes[i].in_queue = 0;
        processes[i].completed = 0;
    }
    fscanf(fp, "%d", quantum);

    fclose(fp);
    *count = n;
    return processes;
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

void display_gantt_chart(GanttEntry *gantt, int gantt_count) {
    printf("\nGantt Chart for Round Robin:\n|");
    for (int i = 0; i < gantt_count; i++)
        printf(" %s |", gantt[i].name);
    printf("\n%-4d", gantt[0].start);
    for (int i = 0; i < gantt_count; i++)
        printf("%5d", gantt[i].end);
    printf("\n");
}

void display_results(Process *processes, int n, int quantum,
                      GanttEntry *gantt, int gantt_count, int context_switches) {
    double avg_tat, avg_wt, avg_rt;
    calculate_metrics(processes, n, &avg_tat, &avg_wt, &avg_rt);

    printf("==========================================\n");
    printf("ROUND ROBIN SCHEDULING RESULTS (Quantum = %d)\n", quantum);
    printf("==========================================\n");
    printf("Process | Arrival | Burst | Completion | Turnaround | Waiting | Response\n");
    printf("--------|---------|-------|------------|------------|---------|---------\n");
    for (int i = 0; i < n; i++) {
        printf("%-7s | %-7d | %-5d | %-10d | %-10d | %-7d | %-8d\n",
               processes[i].name, processes[i].arrival_time, processes[i].burst_time,
               processes[i].completion_time, processes[i].turnaround_time,
               processes[i].waiting_time, processes[i].response_time);
    }

    printf("\nAverage Turnaround Time : %.2f\n", avg_tat);
    printf("Average Waiting Time    : %.2f\n", avg_wt);
    printf("Average Response Time   : %.2f\n", avg_rt);
    printf("Total Context Switches  : %d\n", context_switches);

    display_gantt_chart(gantt, gantt_count);
}

/* ---------- Scheduling ---------- */

void run_round_robin(Process *p, int n, int quantum) {
    Queue q;
    init_queue(&q);

    int current_time = 0, completed = 0;
    int last_process = -1, context_switches = 0;

    GanttEntry gantt[MAX_GANTT];
    int gantt_count = 0;

    /* enqueue every process that has already arrived at time 0 */
    for (int i = 0; i < n; i++) {
        if (p[i].arrival_time <= current_time && !p[i].in_queue) {
            enqueue(&q, i);
            p[i].in_queue = 1;
        }
    }

    while (completed < n) {
        if (is_empty(&q)) {
            current_time++;
            for (int i = 0; i < n; i++) {
                if (p[i].remaining_time > 0 && p[i].arrival_time <= current_time && !p[i].in_queue) {
                    enqueue(&q, i);
                    p[i].in_queue = 1;
                }
            }
            continue;
        }

        int current = dequeue(&q);
        p[current].in_queue = 0;

        if (last_process != -1 && last_process != current)
            context_switches++;

        if (!p[current].first_run) {
            p[current].response_time = current_time - p[current].arrival_time;
            p[current].first_run = 1;
        }

        int start = current_time;
        int run = (quantum < p[current].remaining_time) ? quantum : p[current].remaining_time;
        p[current].remaining_time -= run;
        current_time += run;

        strcpy(gantt[gantt_count].name, p[current].name);
        gantt[gantt_count].start = start;
        gantt[gantt_count].end = current_time;
        gantt_count++;

        /* newly arrived processes join the queue before the preempted process */
        for (int i = 0; i < n; i++) {
            if (p[i].remaining_time > 0 && p[i].arrival_time <= current_time &&
                !p[i].in_queue && i != current) {
                enqueue(&q, i);
                p[i].in_queue = 1;
            }
        }

        if (p[current].remaining_time == 0) {
            p[current].completion_time = current_time;
            completed++;
        } else {
            enqueue(&q, current);
            p[current].in_queue = 1;
        }

        last_process = current;
    }

    display_results(p, n, quantum, gantt, gantt_count, context_switches);
}

int main(int argc, char *argv[]) {
    const char *filename = (argc > 1) ? argv[1] : "processes_rr.txt";
    int n, quantum;

    Process *p = read_processes(filename, &n, &quantum);
    if (p == NULL) return 1;

    if (argc > 2) quantum = atoi(argv[2]);  /* allow overriding the quantum for testing */

    run_round_robin(p, n, quantum);

    free(p);
    return 0;
}
