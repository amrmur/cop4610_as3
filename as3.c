// Add more header files if required...
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_PROCESSES 100

/* ========================================================================================*/
// Structure to store process information
typedef struct
{ // Add more variables to store process information if required...
    int pid;
    int priority;
    int burst_time;
    int arrival_time;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
    int completion_time;
    bool is_completed;

} Process;

typedef struct Node
{
    Process *process;
    struct Node *next;
} Node;

typedef struct
{
    Node *front;
    Node *rear;
    int size;
} Queue;

/* ========================================================================================*/
// Add more Global variables if required...
Process processes[MAX_PROCESSES];
int num_processes = 0;

/* ========================================================================================*/
// Add more Function prototypes suitable your implementation if required...
void fcfs();                        // First Come First Serve Scheduling
void sjf_non_preemptive();          // Shortest Job First - Non-preemptive
void srt_preemptive();              // Shortest Remaining Time - Preemptive
void round_robin(int time_quantum); // Round Robin Scheduling
void priority_non_preemptive();     // Priority Scheduling - Non-preemptive
void read_processes_from_file(const char *filename); //Read and process the process-related information form `input.txt` file
void calculate_average_times();     // Calculate and display average waiting & turnaround times
void display_results();             // Display scheduling results
void display_process_info();        // Display process details before scheduling
void clear_input_buffer();          // Clears input buffer to prevent invalid input issues
void reset_process_states();        // Resets process variables before each scheduling run
int min(int a, int b);              // Utility function to find the minimum of two numbers

void initQueue(Queue *q);
bool isEmpty(Queue *q);
void enqueue(Queue *q, Process *p);
Process *dequeue(Queue *q);
void displayQueue(Queue *q);
void incrementWaitingTime(Queue *q);

/* ========================================================================================*/
// Main function
int main()
{
    // Add more variables suitable for your implementation if required...
    int choice;
    int time_quantum;
    char input[100]; // Buffer to store user input

    // Read process data from file
    read_processes_from_file("input.txt");

    // User-driven menu
    while (1)
    {
        printf("\n                 CPU Scheduling Algorithms\n");
        printf("|-----------------------------------------------------------|\n");
        printf("|   1. First-Come, First-Served (FCFS)                      |\n");
        printf("|   2. Shortest Job First (SJF) - Nonpreemptive             |\n");
        printf("|   3. Shortest Remaining Time (SRT) - Preemptive           |\n");
        printf("|   4. Round Robin (RR)                                     |\n");
        printf("|   5. Priority Scheduling - Nonpreemptive                  |\n");
        printf("|   0. Exit                                                 |\n");
        printf("|-----------------------------------------------------------|\n");

        printf("\nEnter your choice: ");
        if (!fgets(input, sizeof(input), stdin))
        {
            // Handle EOF (e.g., Ctrl+D)
            printf("\nExiting program.\n\n");
            break;
        }

        // Validate input: check if it's an integer
        if (sscanf(input, "%d", &choice) != 1)
        {
            printf("Invalid input. Please enter an integer between 0 and 5.\n");
            continue;
        }

        printf("\n");

        switch (choice)
        {
        case 1:
            fcfs();
            break;
        case 2:
            sjf_non_preemptive();
            break;
        case 3:
            srt_preemptive();
            break;
        case 4:
            printf("Enter time quantum for Round Robin Scheduling: ");
            printf("\n");
            scanf("%d", &time_quantum);
            clear_input_buffer(); // Clear the buffer after reading input
            round_robin(time_quantum);
            break;
        case 5:
            priority_non_preemptive();
            break;
        case 0:
            printf("Exiting program.\n\n");
            exit(0);
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

/* ========================================================================================*/
// Function to read processes from a file (PLEASE DONOT MODIFY THIS FUNCTION CODE!!!)
void read_processes_from_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Buffer to hold each line from the file
    char line[256];
    int line_number = 0;

    // Skip the first two lines (header and separator)
    while (line_number < 2 && fgets(line, sizeof(line), file))
    {
        line_number++;
    }

    // Read process data from the file
    while (fgets(line, sizeof(line), file))
    {
        // Skip lines with separators like "====" or "----"
        if (line[0] == '=' || line[0] == '-')
        {
            continue;
        }

        char pid_str[10]; // Buffer to store process name like "P1" or "1"
        int pid, priority, burst_time, arrival_time;

        // Read the process ID and other values
        if (sscanf(line, "%s %d %d %d", pid_str, &burst_time, &priority, &arrival_time) == 4)
        {
            // Extract numeric part from 'P1' or read directly if it's just '1'
            if (sscanf(pid_str, "P%d", &pid) != 1)
            { // Check if it starts with 'P'
                if (sscanf(pid_str, "%d", &pid) != 1)
                { // Otherwise, try reading as a number
                    printf("Invalid process ID format: %s (skipped)\n", pid_str);
                    continue; // Skip invalid process IDs
                }
            }

            // Store the process data
            processes[num_processes].pid = pid;
            processes[num_processes].priority = priority;
            processes[num_processes].burst_time = burst_time;
            processes[num_processes].arrival_time = arrival_time;
            processes[num_processes].remaining_time = burst_time; // Remaining time equals burst time initially
            processes[num_processes].waiting_time = 0;            // Initialize waiting time for aging
            num_processes++;                                      // Increment process count
        }
        else
        {
            printf("Invalid line format: %s (skipped)\n", line); // Handle invalid data lines
        }
    }

    // Close the file after reading
    fclose(file);

    // Display the loaded process information
    display_process_info();
}

/* ========================================================================================*/
// Function to reset process states
void reset_process_states()
{
    // Also reset other process information variables if you have added to the 'Process' structure...
    for (int i = 0; i < num_processes; i++)
    {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].completion_time = 0;
        processes[i].is_completed = false;
    }
}

/* ========================================================================================*/
// Function to clear the input buffer (PLEASE DONOT MODIFY THIS FUNCTION CODE!!!)
void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ; // Consume characters until newline or EOF
}

/* ========================================================================================*/
// Function to display the scheduling statistics
void display_process_info()
{
    // Print fetched values in a table format
    printf("\n\n             Process Scheduling Information\n");
    printf("----------------------------------------------------------\n");
    printf("  | %-5s | %-12s | %-12s | %-12s |\n", "PID", "Burst Time", "Priority", "Arrival Time");
    printf("----------------------------------------------------------\n");

    for (int i = 0; i < num_processes; i++)
    {
        printf("  | %-5d | %-12d | %-12d | %-12d |\n",
               processes[i].pid, processes[i].burst_time,
               processes[i].priority, processes[i].arrival_time);
    }

    printf("----------------------------------------------------------\n\n");
}

/* ========================================================================================*/
// Finding minmum of two numbers
int min(int a, int b)
{
    return (a < b) ? a : b;
}

/* ========================================================================================*/
// Calculate Average Waiting Time & Average Turnaround Time
void calculate_average_times()
{
    int total_waiting_time = 0, total_turnaround_time = 0;
    
    for(int i = 0; i < num_processes; i++){
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
    }

    printf("\nAverage Waiting Time: %.2f\n", (float)total_waiting_time / num_processes);
    printf("\nAverage Turnaround Time: %.2f\n", (float)total_turnaround_time / num_processes);
}

/* ========================================================================================*/
// Display results
void display_results()
{
    // Display Individual Process Turnaround Time & Waiting Time
    printf("\nProcess\t   Waiting Time\t    Turnaround Time\n");
    for (int i = 0; i < num_processes; i++)
    {
        printf("  P%d\t       %d\t\t %d\n", processes[i].pid, processes[i].waiting_time, processes[i].turnaround_time);
    }

    // Display Average Waiting Time & Average Turnaround Time
    calculate_average_times();
}

/* ========================================================================================*/
// First-Come, First-Served (FCFS) Scheduling
void fcfs()
{   

    // Reset process states before execution
    reset_process_states();

    // Implementation of rest of your code...
    int time = 0;
    int size = 0;
    Process* cur = NULL;
    int completed = num_processes;

    Queue q;
    initQueue(&q);
    while(completed != 0){
        while(size < num_processes && processes[size].arrival_time == time){
            enqueue(&q, &processes[size]);
            size += 1;
        }

        if(cur == NULL){
            cur = dequeue(&q);
        }
        cur->remaining_time -= 1;
        incrementWaitingTime(&q);
        
        if(cur->remaining_time == 0){
            completed -= 1;
            cur->completion_time = time;
            cur = NULL;
        }
        time += 1;
    }

    for(int i = 0; i < num_processes; i++){
        processes[i].turnaround_time = processes[i].waiting_time + processes[i].burst_time;
    }
    calculate_average_times();

    printf("***************************************************************************************\n\n");
    printf("FCFS Statistics...\n");
    display_results(); // Display Statistics
    printf("\n***************************************************************************************\n");
}

/* ========================================================================================*/
// Shortest Job First (SJF) - Non-Preemptive
void sjf_non_preemptive()
{
    // Reset process states
    reset_process_states();

    // Implementation of rest of your code...
     int current_time = 0;
    int completed = 0;
    
    while (completed < num_processes) {
        int shortest_job_index = -1;
        int shortest_burst_time = INT_MAX;
        
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= current_time && 
                !processes[i].is_completed && 
                processes[i].burst_time < shortest_burst_time) {
                shortest_burst_time = processes[i].burst_time;
                shortest_job_index = i;
            }
        }
        
        if (shortest_job_index == -1) {
            current_time++;
            continue;
        }
        
        Process *current_process = &processes[shortest_job_index];
        
        current_process->waiting_time = current_time - current_process->arrival_time;
        
        current_time += current_process->burst_time;
        
        current_process->completion_time = current_time;
        current_process->turnaround_time = current_process->completion_time - current_process->arrival_time;
        
        current_process->is_completed = true;
        completed++;
    }

    // Display results
    printf("***************************************************************************************\n\n");
    printf("SJF (Non-Preemptive) Statistics...\n");
    display_results();
    printf("\n***************************************************************************************\n\n");
}

/* ========================================================================================*/
// Shortest Remaining Time (SRT) - SJF Preemptive
void srt_preemptive()
{

    // Reset process states before execution
    reset_process_states();

    // Implementation of rest of your code...
    int current_time = 0;
    int completed = 0;
    int prev = -1;
    
    while (completed != num_processes)
    {
        int shortest_idx = -1;
        int shortest_time = INT_MAX;
        
        for (int i = 0; i < num_processes; i++)
        {
            if (processes[i].arrival_time <= current_time && 
                !processes[i].is_completed && 
                processes[i].remaining_time < shortest_time)
            {
                shortest_idx = i;
                shortest_time = processes[i].remaining_time;
            }
        }
        
        if (shortest_idx == -1)
        {
            current_time++;
            continue;
        }
        
        if (prev != shortest_idx && prev != -1)
        {
        }
        
        processes[shortest_idx].remaining_time--;
        prev = shortest_idx;
        
        for (int i = 0; i < num_processes; i++)
        {
            if (i != shortest_idx && 
                processes[i].arrival_time <= current_time && 
                !processes[i].is_completed)
            {
                processes[i].waiting_time++;
            }
        }
        
        if (processes[shortest_idx].remaining_time == 0)
        {
            processes[shortest_idx].is_completed = true;
            processes[shortest_idx].completion_time = current_time + 1;
            processes[shortest_idx].turnaround_time = 
                processes[shortest_idx].completion_time - processes[shortest_idx].arrival_time;
            
            completed++;
        }
        
        current_time++;
    }

    printf("***************************************************************************************\n\n");
    printf("SRT (Preemptive) Statistics...\n");
    display_results();
    printf("\n***************************************************************************************\n\n");
}

/* ========================================================================================*/
// Round Robin (RR) Scheduling
void round_robin(int time_quantum)
{

    // Reset process states before execution
    reset_process_states();

    // Implementation of rest of your code...
    int current_time = 0, completed = 0;
    while (completed < num_processes)
    {
        bool done = true;
        for (int i = 0; i < num_processes; i++)
        {
            if (processes[i].remaining_time > 0)
            {
                done = false;
                int execution_time = (processes[i].remaining_time > time_quantum) ? time_quantum : processes[i].remaining_time;
                processes[i].remaining_time -= execution_time;
                current_time += execution_time;
                if (processes[i].remaining_time == 0)
                {
                    processes[i].completion_time = current_time;
                    processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
                    processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
                    completed++;
                }
            }
        }
        if (done) break;
    }

    printf("***************************************************************************************\n\n");
    printf("RR Statistics (Time Quantum = %d)...\n", time_quantum);
    display_results(); // Display Statistics
    printf("\n***************************************************************************************\n");
}

/* ========================================================================================*/
// Priority Scheduling - Non-Preemptive
void priority_non_preemptive()
{

    // Reset process states before execution
    reset_process_states();

    // Implementation of rest of your code...

    printf("***************************************************************************************\n\n");
    printf("Priority (PR) - Nonpreemptive Statistics...\n");
    display_results(); // Display Statistics
    printf("\n***************************************************************************************\n");
}

void initQueue(Queue *q)
{
    q->front = q->rear = NULL;
    q->size = 0;
}

bool isEmpty(Queue *q)
{
    return q->size == 0;
}

void enqueue(Queue *q, Process *p)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (!newNode)
        return;

    newNode->process = p;
    newNode->next = NULL;

    if (q->rear == NULL)
        q->front = q->rear = newNode;
    else
    {
        q->rear->next = newNode;
        q->rear = newNode;
    }

    q->size++;
}

Process *dequeue(Queue *q)
{
    if (isEmpty(q))
        return NULL;

    Node *temp = q->front;
    Process *p = temp->process;
    q->front = q->front->next;

    if (q->front == NULL)
        q->rear = NULL;

    free(temp);
    q->size--;

    return p; 
}

void displayQueue(Queue *q)
{
    if (isEmpty(q))
    {
        printf("Queue is empty!\n");
        return;
    }

    Node *temp = q->front;
    printf("Process Queue:\n");
    while (temp)
    {
        printf("PID: %d, Priority: %d, Burst Time: %d, Arrival Time: %d\n",
               temp->process->pid, temp->process->priority, temp->process->burst_time, temp->process->arrival_time);
        temp = temp->next;
    }
    printf("\n");
}

void incrementWaitingTime(Queue *q)
{
    Node *temp = q->front;
    while (temp)
    {
        temp->process->waiting_time += 1;
        temp = temp->next;
    }
}
