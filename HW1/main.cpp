#include <vector>
#include <iomanip>
#include <algorithm>
#include <iostream>

using namespace std;

class Process {
public:
    int pid;                  // Process ID
    int arrival_time;         // Arrival Time
    int burst_time;           // Burst Time
    int waiting_time;         // Waiting Time
    int turnaround_time;      // Turnaround Time
    int remaining_time;       // Remaining Time for preemptive scheduling

    Process(int id, int at, int bt) {
        pid = id;
        arrival_time = at;
        burst_time = bt;
        waiting_time = 0;
        turnaround_time = 0;
        remaining_time = bt; // Initialize remaining time
    }
};

// Function for First-Come, First-Serve (FCFS) Scheduling
void fcfs_scheduling(vector<Process>& processes) {
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });
    int current_time = 0;
    for (auto& process : processes) {
        if (current_time < process.arrival_time) {
            current_time = process.arrival_time;
        }
        process.waiting_time = current_time - process.arrival_time;
        process.turnaround_time = process.waiting_time + process.burst_time;
        current_time += process.burst_time;
    }
}

// Function for Shortest Job First (SJF) Scheduling
void sjf_scheduling(vector<Process>& processes) {
    int current_time = 0;
    vector<Process> completed_processes;

    while (completed_processes.size() < processes.size()) {
        vector<Process*> ready_queue;

        // Populate ready queue with processes that have arrived
        for (auto& process : processes) {
            if (process.arrival_time <= current_time &&
                find_if(completed_processes.begin(), completed_processes.end(), [&](const Process& p) {
                    return p.pid == process.pid;
                }) == completed_processes.end()) {
                ready_queue.push_back(&process);
            }
        }

        // Select the shortest job
        if (!ready_queue.empty()) {
            auto shortest_job = min_element(ready_queue.begin(), ready_queue.end(),
                [](Process* a, Process* b) {
                    return a->burst_time < b->burst_time;
                });
            Process* current_process = *shortest_job;

            // Calculate waiting and turnaround times
            current_process->waiting_time = current_time - current_process->arrival_time;
            current_process->turnaround_time = current_process->waiting_time + current_process->burst_time;
            current_time += current_process->burst_time;

            completed_processes.push_back(*current_process);
        } else {
            current_time++; // If no process is ready, advance time
        }
    }

    processes = completed_processes; // Update the processes list with completed processes
}

// Function for Shortest Remaining Time (SRT) Scheduling
void srt_scheduling(vector<Process>& processes) {
    int current_time = 0;
    int completed = 0;
    int n = processes.size();
    vector<bool> is_completed(n, false);

    while (completed < n) {
        vector<Process*> ready_queue;

        // Populate ready queue with processes that have arrived
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && !is_completed[i]) {
                ready_queue.push_back(&processes[i]);
            }
        }

        // Find the process with the shortest remaining time
        if (!ready_queue.empty()) {
            auto shortest = min_element(ready_queue.begin(), ready_queue.end(),
                [](Process* a, Process* b) {
                    return a->remaining_time < b->remaining_time;
                });

            Process* current_process = *shortest;
            current_process->remaining_time--;

            // Check if the process is completed
            if (current_process->remaining_time == 0) {
                is_completed[current_process->pid - 1] = true;
                completed++;
                current_process->turnaround_time = current_time + 1 - current_process->arrival_time;
                current_process->waiting_time = current_process->turnaround_time - current_process->burst_time;
            }
        }

        current_time++;
    }
}

// Function for Round Robin (RR) Scheduling
void round_robin_scheduling(vector<Process>& processes, int time_quantum) {
    int current_time = 0;
    vector<Process*> ready_queue;
    int completed_processes = 0;
    int n = processes.size();

    // Initialize remaining time before starting scheduling
    for (auto& process : processes) {
        process.remaining_time = process.burst_time;
    }

    // Initialize ready queue with processes that have arrived
    for (auto& process : processes) {
        if (process.arrival_time <= current_time) {
            ready_queue.push_back(&process);
        }
    }

    while (completed_processes < n) {
        for (size_t i = 0; i < ready_queue.size(); i++) {
            Process* process = ready_queue[i];

            if (process->remaining_time > 0) {
                // Determine time slice
                int time_slice = min(time_quantum, process->remaining_time);
                current_time += time_slice;
                process->remaining_time -= time_slice;

                // If process finishes within the time slice
                if (process->remaining_time == 0) {
                    completed_processes++;
                    process->turnaround_time = current_time - process->arrival_time;
                    process->waiting_time = process->turnaround_time - process->burst_time;
                }
            }
            // Move to the end of the queue if not finished
            if (process->remaining_time > 0) {
                ready_queue.push_back(process);
            }
            ready_queue.erase(ready_queue.begin() + i); // Remove the process from the current index
            i--; // Decrement i to stay on the current index after removal
        }

        // Add newly arrived processes to the queue
        for (auto& process : processes) {
            if (process.arrival_time <= current_time &&
                find_if(ready_queue.begin(), ready_queue.end(), [&](Process* p) {
                    return p->pid == process.pid;
                }) == ready_queue.end() &&
                process.remaining_time > 0) {
                ready_queue.push_back(&process);
            }
        }
    }
}

// Function to display results
void display_results(const string& algorithm_name, const vector<Process>& processes) {
    cout << "Results for " << algorithm_name << ":" << endl;

    // Display results
    cout << left << setw(15) << "Process ID"
         << setw(15) << "Arrival Time"
         << setw(15) << "Burst Time"
         << setw(15) << "Waiting Time"
         << setw(15) << "Turnaround Time" << endl;

    for (const auto& process : processes) {
        cout << setw(15) << process.pid
             << setw(15) << process.arrival_time
             << setw(15) << process.burst_time
             << setw(15) << process.waiting_time
             << setw(15) << process.turnaround_time << endl;
    }

    // Calculate and display the average waiting and turnaround times
    double avg_waiting_time = 0;
    double avg_turnaround_time = 0;

    for (const auto& process : processes) {
        avg_waiting_time += process.waiting_time;
        avg_turnaround_time += process.turnaround_time;
    }

    avg_waiting_time /= processes.size();
    avg_turnaround_time /= processes.size();

    cout << "Average Waiting Time: " << avg_waiting_time << endl;
    cout << "Average Turnaround Time: " << avg_turnaround_time << endl << endl;
}

int main() {
    // Initialize processes
    vector<Process> processes = {
        Process(1, 0, 8),
        Process(2, 1, 4),
        Process(3, 2, 9),
        Process(4, 3, 5)
    };

    // FCFS Scheduling
    vector<Process> fcfs_processes = processes;
    fcfs_scheduling(fcfs_processes);
    display_results("First-Come, First-Serve (FCFS)", fcfs_processes);

    // SJF Scheduling
    vector<Process> sjf_processes = processes;
    sjf_scheduling(sjf_processes);
    display_results("Shortest Job First (SJF)", sjf_processes);

    // SRT Scheduling
    vector<Process> srt_processes = processes;
    srt_scheduling(srt_processes);
    display_results("Shortest Remaining Time (SRT)", srt_processes);

    
    // Round Robin Scheduling
    int time_quantum = 3; // Example time quantum
    vector<Process> rr_processes = processes;
    round_robin_scheduling(rr_processes, time_quantum);
    display_results("Round Robin (RR)", rr_processes);

    return 0;
}
