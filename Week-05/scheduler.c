#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Structure to store process information
typedef struct {
    char name[10];
    int arrival_time;
    int burst_time;
    int remaining_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
    int first_run;
} Process;


// Read processes from input file
Process *read_processes(const char *filename,int *count)
{
    FILE *file=fopen(filename,"r");

    // Check if file opened successfully
    if(file==NULL)
    {
        printf("Error: Could not open %s\n",filename);
        return NULL;
    }

    // Read number of processes
    if(fscanf(file,"%d",count)!=1 || *count<=0)
    {
        printf("Error: Invalid number of processes.\n");
        fclose(file);
        return NULL;
    }

    // Allocate memory for processes
    Process *processes=malloc((*count)*sizeof(Process));

    if(processes==NULL)
    {
        printf("Error: Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    // Read details of each process
    for(int i=0;i<*count;i++)
    {
        if(fscanf(file,"%9s %d %d",
                  processes[i].name,
                  &processes[i].arrival_time,
                  &processes[i].burst_time)!=3)
        {
            printf("Error: Invalid input at process %d.\n",i+1);
            free(processes);
            fclose(file);
            return NULL;
        }

        // Initially remaining time is equal to burst time
        processes[i].remaining_time=processes[i].burst_time;

        // Initialize calculated values
        processes[i].completion_time=0;
        processes[i].waiting_time=0;
        processes[i].turnaround_time=0;
        processes[i].response_time=0;
        processes[i].first_run=0;
    }

    fclose(file);

    return processes;
}


// Copy process array
void copy_processes(Process *destination,Process *source,int n)
{
    for(int i=0;i<n;i++)
    {
        destination[i]=source[i];
    }
}


// Calculate turnaround time and waiting time
void calculate_metrics(Process *processes,int n)
{
    for(int i=0;i<n;i++)
    {
        // Turnaround Time = Completion Time - Arrival Time
        processes[i].turnaround_time=
            processes[i].completion_time-processes[i].arrival_time;

        // Waiting Time = Turnaround Time - Burst Time
        processes[i].waiting_time=
            processes[i].turnaround_time-processes[i].burst_time;
    }
}


// Display scheduling results
void display_results(Process *processes,int n,const char *algorithm_name)
{
    double total_turnaround=0;
    double total_waiting=0;
    double total_response=0;

    printf("\n");
    printf("==============================================================\n");
    printf("%s SCHEDULING RESULTS\n",algorithm_name);
    printf("==============================================================\n");

    printf("Process | Arrival | Burst | Completion | Turnaround | Waiting | Response\n");
    printf("-----------------------------------------------------------------------\n");

    // Display every process
    for(int i=0;i<n;i++)
    {
        printf("%-7s | %7d | %5d | %10d | %10d | %7d | %8d\n",
               processes[i].name,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].completion_time,
               processes[i].turnaround_time,
               processes[i].waiting_time,
               processes[i].response_time);

        total_turnaround+=processes[i].turnaround_time;
        total_waiting+=processes[i].waiting_time;
        total_response+=processes[i].response_time;
    }

    printf("-----------------------------------------------------------------------\n");

    // Display average values
    printf("Average Turnaround Time : %.2f\n",total_turnaround/n);
    printf("Average Waiting Time    : %.2f\n",total_waiting/n);
    printf("Average Response Time   : %.2f\n",total_response/n);
}


// Sort processes according to arrival time
void sort_by_arrival(Process *processes,int n)
{
    for(int i=0;i<n-1;i++)
    {
        for(int j=0;j<n-i-1;j++)
        {
            if(processes[j].arrival_time>processes[j+1].arrival_time)
            {
                Process temp=processes[j];
                processes[j]=processes[j+1];
                processes[j+1]=temp;
            }
        }
    }
}


// FCFS Scheduling
void fcfs(Process *processes,int n)
{
    // Arrange processes according to arrival time
    sort_by_arrival(processes,n);

    int current_time=0;

    for(int i=0;i<n;i++)
    {
        // If CPU is idle, move time to arrival time
        if(current_time<processes[i].arrival_time)
        {
            current_time=processes[i].arrival_time;
        }

        // Response time
        processes[i].response_time=
            current_time-processes[i].arrival_time;

        processes[i].first_run=1;

        // Execute process completely
        current_time+=processes[i].burst_time;

        // Store completion time
        processes[i].completion_time=current_time;
    }

    calculate_metrics(processes,n);
}


// FCFS Gantt Chart
void print_fcfs_gantt(Process *processes,int n)
{
    printf("\nFCFS Gantt Chart:\n\n");

    printf("|");

    // Print process names
    for(int i=0;i<n;i++)
    {
        printf(" %s |",processes[i].name);
    }

    printf("\n");

    // Print starting time
    printf("%d",processes[0].arrival_time);

    // Print completion times
    for(int i=0;i<n;i++)
    {
        printf("    %d",processes[i].completion_time);
    }

    printf("\n");
}


// SJF Scheduling
void sjf(Process *processes,int n)
{
    int current_time=0;
    int completed=0;

    // Stores whether a process is completed
    bool *done=calloc(n,sizeof(bool));

    if(done==NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    while(completed<n)
    {
        int selected=-1;

        // Find shortest available process
        for(int i=0;i<n;i++)
        {
            if(!done[i] &&
               processes[i].arrival_time<=current_time)
            {
                // Select process with shortest burst time
                if(selected==-1 ||
                   processes[i].burst_time<processes[selected].burst_time)
                {
                    selected=i;
                }
            }
        }

        // If no process is available, move to next arrival
        if(selected==-1)
        {
            int next_arrival=-1;

            for(int i=0;i<n;i++)
            {
                if(!done[i])
                {
                    if(next_arrival==-1 ||
                       processes[i].arrival_time<next_arrival)
                    {
                        next_arrival=processes[i].arrival_time;
                    }
                }
            }

            current_time=next_arrival;
            continue;
        }

        // Calculate response time
        processes[selected].response_time=
            current_time-processes[selected].arrival_time;

        processes[selected].first_run=1;

        // Execute selected process completely
        current_time+=processes[selected].burst_time;

        // Store completion time
        processes[selected].completion_time=current_time;

        // Mark process as completed
        done[selected]=true;
        completed++;
    }

    free(done);

    calculate_metrics(processes,n);
}


// SJF Gantt Chart
void print_sjf_gantt(Process *processes,int n)
{
    bool *done=calloc(n,sizeof(bool));
    int *order=malloc(n*sizeof(int));

    if(done==NULL || order==NULL)
    {
        printf("Memory allocation failed.\n");
        free(done);
        free(order);
        return;
    }

    int current_time=0;
    int completed=0;
    int order_count=0;

    // Find actual SJF execution order
    while(completed<n)
    {
        int selected=-1;

        for(int i=0;i<n;i++)
        {
            if(!done[i] &&
               processes[i].arrival_time<=current_time)
            {
                if(selected==-1 ||
                   processes[i].burst_time<processes[selected].burst_time)
                {
                    selected=i;
                }
            }
        }

        // CPU is idle until next process arrives
        if(selected==-1)
        {
            int next_arrival=-1;

            for(int i=0;i<n;i++)
            {
                if(!done[i])
                {
                    if(next_arrival==-1 ||
                       processes[i].arrival_time<next_arrival)
                    {
                        next_arrival=processes[i].arrival_time;
                    }
                }
            }

            current_time=next_arrival;
            continue;
        }

        // Store actual execution order
        order[order_count]=selected;
        order_count++;

        current_time+=processes[selected].burst_time;

        done[selected]=true;
        completed++;
    }

    printf("\nSJF Gantt Chart:\n\n");

    printf("|");

    // Print actual SJF execution order
    for(int i=0;i<order_count;i++)
    {
        printf(" %s |",processes[order[i]].name);
    }

    printf("\n");

    // Print starting time
    printf("%d",processes[order[0]].arrival_time);

    // Print completion times
    for(int i=0;i<order_count;i++)
    {
        printf("    %d",processes[order[i]].completion_time);
    }

    printf("\n");

    free(order);
    free(done);
}


// SRTF Scheduling
void srtf(Process *processes,int n)
{
    int current_time=0;
    int completed=0;

    // Maximum number of Gantt segments
    int max_segments=n*100;

    int *gantt_process=
        malloc(max_segments*sizeof(int));

    int *gantt_start=
        malloc(max_segments*sizeof(int));

    int *gantt_end=
        malloc(max_segments*sizeof(int));

    int gantt_count=0;

    if(gantt_process==NULL ||
       gantt_start==NULL ||
       gantt_end==NULL)
    {
        printf("Memory allocation failed.\n");

        free(gantt_process);
        free(gantt_start);
        free(gantt_end);

        return;
    }

    // Find first arrival time
    int first_arrival=processes[0].arrival_time;

    for(int i=1;i<n;i++)
    {
        if(processes[i].arrival_time<first_arrival)
        {
            first_arrival=processes[i].arrival_time;
        }
    }

    current_time=first_arrival;

    int previous_process=-1;
    int segment_start=current_time;

    while(completed<n)
    {
        int selected=-1;

        // Find process with shortest remaining time
        for(int i=0;i<n;i++)
        {
            if(processes[i].arrival_time<=current_time &&
               processes[i].remaining_time>0)
            {
                if(selected==-1 ||
                   processes[i].remaining_time<
                   processes[selected].remaining_time)
                {
                    selected=i;
                }
                else if(processes[i].remaining_time==
                        processes[selected].remaining_time)
                {
                    // If equal, continue current process
                    if(previous_process!=-1 &&
                       processes[previous_process].remaining_time>0 &&
                       processes[previous_process].arrival_time<=current_time)
                    {
                        selected=previous_process;
                    }
                }
            }
        }

        // If no process is available, CPU remains idle
        if(selected==-1)
        {
            if(previous_process!=-1)
            {
                gantt_start[gantt_count]=segment_start;
                gantt_end[gantt_count]=current_time;
                gantt_process[gantt_count]=previous_process;
                gantt_count++;

                previous_process=-1;
            }

            current_time++;
            segment_start=current_time;
            continue;
        }

        // Check for process change
        if(previous_process!=selected)
        {
            if(previous_process!=-1)
            {
                gantt_start[gantt_count]=segment_start;
                gantt_end[gantt_count]=current_time;
                gantt_process[gantt_count]=previous_process;
                gantt_count++;
            }

            segment_start=current_time;
            previous_process=selected;
        }

        // Calculate response time on first CPU allocation
        if(!processes[selected].first_run)
        {
            processes[selected].response_time=
                current_time-processes[selected].arrival_time;

            processes[selected].first_run=1;
        }

        // Execute for one time unit
        processes[selected].remaining_time--;
        current_time++;

        // Process completed
        if(processes[selected].remaining_time==0)
        {
            processes[selected].completion_time=current_time;
            completed++;

            gantt_start[gantt_count]=segment_start;
            gantt_end[gantt_count]=current_time;
            gantt_process[gantt_count]=selected;
            gantt_count++;

            previous_process=-1;
            segment_start=current_time;
        }
    }

    calculate_metrics(processes,n);

    display_results(processes,n,"SRTF");

    // SRTF Gantt Chart
    printf("\nSRTF Gantt Chart:\n\n");

    printf("|");

    for(int i=0;i<gantt_count;i++)
    {
        printf(" %s |",
               processes[gantt_process[i]].name);
    }

    printf("\n");

    if(gantt_count>0)
    {
        printf("%d",gantt_start[0]);

        for(int i=0;i<gantt_count;i++)
        {
            printf("    %d",gantt_end[i]);
        }

        printf("\n");
    }

    free(gantt_process);
    free(gantt_start);
    free(gantt_end);
}


// Main function
int main(int argc,char *argv[])
{
    int n;

    // Check whether input filename is provided
    if(argc!=2)
    {
        printf("Usage: ./scheduler processes1.txt\n");
        return 1;
    }

    // Read the input file given in command line
    Process *original=
        read_processes(argv[1],&n);

    if(original==NULL)
    {
        return 1;
    }

    // Create separate copies for each algorithm
    Process *fcfs_processes=
        malloc(n*sizeof(Process));

    Process *sjf_processes=
        malloc(n*sizeof(Process));

    Process *srtf_processes=
        malloc(n*sizeof(Process));

    if(fcfs_processes==NULL ||
       sjf_processes==NULL ||
       srtf_processes==NULL)
    {
        printf("Memory allocation failed.\n");

        free(original);
        free(fcfs_processes);
        free(sjf_processes);
        free(srtf_processes);

        return 1;
    }

    // Copy original data
    copy_processes(fcfs_processes,original,n);
    copy_processes(sjf_processes,original,n);
    copy_processes(srtf_processes,original,n);

    // Run FCFS
    fcfs(fcfs_processes,n);
    display_results(fcfs_processes,n,"FCFS");
    print_fcfs_gantt(fcfs_processes,n);

    // Run SJF
    sjf(sjf_processes,n);
    display_results(sjf_processes,n,"SJF");
    print_sjf_gantt(sjf_processes,n);

    // Run SRTF
    srtf(srtf_processes,n);

    // Free allocated memory
    free(original);
    free(fcfs_processes);
    free(sjf_processes);
    free(srtf_processes);

    return 0;
}