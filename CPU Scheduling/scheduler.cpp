/*
 * Name: Batul Zamin
 * Class: Operating Systems
 * Program: A C program to implement 3 scheduling algorithms: 
            Round Robin (RR), Shortest Job First (SJF) and Shortest Remaining Time First (SRTF).
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <queue>
#include <map>

using namespace std;
const int MAX_PROCESSES = 8;

// Structure to store process information
struct Process {
	char process_id;
	int burst_time;
	int arrival_time;
	int completion_time;
};

// Helper Function for sort function to compare 2 processes
bool compare2ProcessRR(Process p1, Process p2) {
	// if process arrive at same time, sort alphabetically on process ID else sort on arrival time
	if (p1.arrival_time == p2.arrival_time)
		return p1.process_id < p2.process_id; 
	return p1.arrival_time < p2.arrival_time; 
}

// Sorts the Process array
void sortProcesses(Process p[], int n) {
	sort(p, p + n, compare2ProcessRR);
}

// Structure maintaining the priority ready queue using burst time
struct compareBurstTime {
	int operator()(const Process& p1, const Process& p2) {
		// if burst time is same, give more priority to already running process else alphabetically
		if (p1.burst_time == p2.burst_time) {
			// (completion time -1 indicates the process was running previously but not completed)
			if (p1.completion_time == -1) 
				return false;
			else if (p2.completion_time == -1) 
				return true;
			return p1.process_id > p2.process_id;
		}
		// if burst time is not same, more priority to less burst time process.
		return p1.burst_time > p2.burst_time;
	}
};

// Function to push process from job queue to ready queue at given time
// Helper for Round Robin
void pushToReadyQueue(queue<Process>& job_queue, queue<Process>& ready_queue, int time) {
	while (!job_queue.empty()) {
		if (time == job_queue.front().arrival_time) {
			ready_queue.push(job_queue.front());
			job_queue.pop();
		}
		else break;
	}
}

// Function to push process from job queue to  priority ready queue at given time
// Helper for SJF and SRTF
void pushToPriorityReadyQueue(queue<Process>& job_queue, 
	priority_queue<Process, vector<Process>, compareBurstTime>& pri_ready_queue, int time) {
	while (!job_queue.empty()) {
		if (job_queue.front().arrival_time <= time) {
			pri_ready_queue.push(job_queue.front());
			job_queue.pop();
		}
		else break;
	}
}

// Function to display vertical Gantt Chart for RR
void ganttChartRR(queue<Process> job_queue, map<char, Process>& p_map, int total_time) {
	// local variables
	int time_quantum = 3, ctime, timer = 0, process_start_time = 0;
	queue <Process> ready_queue;
	// push the processes at time 0 to ready queue
	pushToReadyQueue(job_queue, ready_queue, 0);
	for (ctime = 0; ctime < total_time; ctime++) {
		if (!ready_queue.empty()) {
			ready_queue.front().burst_time--; // run front of ready queue
			timer++; 
			// Print the process with its start time and reason for context switch
			if (ready_queue.front().burst_time == 0) { // process terminated 
				cout << process_start_time << " " << ready_queue.front().process_id << " Process terminated" << endl;
				process_start_time = ctime + 1;
				p_map[ready_queue.front().process_id].completion_time = ctime + 1;
				ready_queue.pop();
				timer = 0;
			}
			else if (timer == time_quantum) { // quantum expired
				cout << process_start_time << " " << ready_queue.front().process_id << " Quantum expired" << endl;
				ready_queue.push(ready_queue.front());
				ready_queue.pop();
				process_start_time = ctime + 1;
				timer = 0;
			}
		}
		// push the processes at ctime to ready queue
		pushToReadyQueue(job_queue, ready_queue, ctime);
	}
	cout << ctime << " Complete" << endl << endl;
}

// Function to display vertical Gantt Chart for SJF
void ganttChartSJF(queue<Process> job_queue, map<char, Process>& p_map) {
	// local variables
	int ctime = job_queue.front().arrival_time;
	priority_queue<Process, vector<Process>, compareBurstTime> pri_ready_queue;
	pushToPriorityReadyQueue(job_queue, pri_ready_queue, 0);
	// while priority queue is not empty keep running processes
	while (!pri_ready_queue.empty()) {
		cout << ctime << " " << pri_ready_queue.top().process_id << " Process terminated" << endl;
		ctime += pri_ready_queue.top().burst_time;
		p_map[pri_ready_queue.top().process_id].completion_time = ctime;
		pri_ready_queue.pop();
		// push processes from job queue to priority queue if they come before ctime
		pushToPriorityReadyQueue(job_queue, pri_ready_queue, ctime);
	}
	cout << ctime << " Complete" << endl << endl;
}

// Function to display vertical Gantt Chart for SRTF
void ganttChartSRTF(queue<Process> job_queue, map<char, Process>& p_map, int total_time) {
	// local variables
	bool running = true; // to mark if process was running just before checking all processes in ready queue
	int process_start_time = 0, ctime;
	priority_queue<Process, vector<Process>, compareBurstTime> pri_ready_queue;
	pushToPriorityReadyQueue(job_queue, pri_ready_queue, 0);
	for (ctime = 0; ctime < total_time; ctime++) {
		Process pw = pri_ready_queue.top();
		// push processes from job queue to priority queue if they come before or at ctime
		pushToPriorityReadyQueue(job_queue, pri_ready_queue, ctime);
		if (!pri_ready_queue.empty()) {
			// if process was running but context switched
			if (pri_ready_queue.top().process_id != pw.process_id && running) {
				cout << process_start_time << " " << pw.process_id
					<< " Process preempted by process with shorter burst time" << endl;
				process_start_time = ctime;
			}
			// set pw to most recent top of priority queue, decrement burst time, mark process incomplete (-1)
			pw = pri_ready_queue.top(); 
			pw.burst_time--;
			pw.completion_time = -1;
			// push pw to priority queue and pop the previous top
			pri_ready_queue.pop();
			pri_ready_queue.push(pw);
			running = true;
			// Print the process with its start time and reason for context switch
			if (pri_ready_queue.top().burst_time == 0) {
				cout << process_start_time << " " << pri_ready_queue.top().process_id << " Process terminated" << endl;
				process_start_time = ctime + 1;
				p_map[pri_ready_queue.top().process_id].completion_time = ctime + 1;
				pri_ready_queue.pop();
				running = false;
			}
		}
	}
	cout << ctime << " Complete" << endl << endl;
}

// Function to print the turnaround and waiting time statistics of the processes for RR, SJF and SRTF
// Turnaround time = Completion time - Arrival time
// Waiting time = Turnaround time - Burst time
void turnaroundAndWaitingTime(map<char, Process> p_map, Process p[MAX_PROCESSES], int i) {
	int turnaround_time = 0, waiting_time = 0;
	float total_turnaround_time = 0.0, total_waiting_time = 0.0;
	cout << "************  Summary  Statistics  ************" << endl;
	cout << "-----------------------------------------------" << endl;
	cout << "| Process ID | Turnaround Time | Waiting Time |" << endl;
	cout << "-----------------------------------------------" << endl;

	for (int a = 0; a < i; a++) {
		turnaround_time = p_map[p[a].process_id].completion_time - p[a].arrival_time;
		waiting_time = turnaround_time - p[a].burst_time;
		cout << "| " << p[a].process_id << right << setw(12) << "| "
			<< left << setw(16) << setfill(' ') << turnaround_time << "| "
			<< left << setw(13) << setfill(' ') << waiting_time << "|" << endl;
		total_turnaround_time += turnaround_time;
		total_waiting_time += waiting_time;
	}
	cout << "-----------------------------------------------" << endl;
	cout << "| Average    | " << left << setw(16) << setfill(' ') << total_turnaround_time / i << "| "
		<< left << setw(13) << setfill(' ') << total_waiting_time / i << "|" << endl;
	cout << "-----------------------------------------------" << endl;
}

// Main Function 
int main(int argc, char** argv)
{
	cout << "Welcome to bzscheduler." << endl; // Welcome message
	// Read input file 
	ifstream in_file(argv[1]); 
	if (!in_file) {
		cout << "Error! Could not open input file." << endl;
		exit(-1);
	}
	// Declare local variables
	Process p[MAX_PROCESSES], job[MAX_PROCESSES];
	map<char, Process> p_map;
	queue <Process> job_queue;
	int i = 0, total_time = 0, num_processes;

	// Load processes in: p, job and p_map
	while (in_file >> p[i].process_id >> p[i].burst_time >> p[i].arrival_time) {
		job[i].process_id = p[i].process_id;
		job[i].burst_time = p[i].burst_time;
		job[i].arrival_time = p[i].arrival_time;
		p_map[p[i].process_id] = p[i];
		total_time += p[i].burst_time;
		i++;
	}
	num_processes = i;
	sortProcesses(job, num_processes); // sort processes
	// Push all processes to Job queue
	for (int x = 0; x < num_processes; x++) 
		job_queue.push(job[x]);

	/****************************************************************************************************/                                                                                                                                                                                                                                                                                                                                                                                           
	/* Round Robin Scheduling Algorithm */
	cout << "\n* Round Robin Scheduling Algorithm *\n" << endl;
	/* Gannt Chart */
	ganttChartRR(job_queue, p_map, total_time);
	/* Turnaround time And Waiting time */
	turnaroundAndWaitingTime(p_map, p, num_processes);

	/****************************************************************************************************/
	/* Non-Preemptive Shortest Job First Scheduling Algorithm */
	cout << "\n* Non-Preemptive Shortest Job First Scheduling Algorithm *\n" << endl;
	/* Gannt Chart */
	ganttChartSJF(job_queue, p_map);
	/* Turnaround time And Waiting time */
	turnaroundAndWaitingTime(p_map, p, i);

	/****************************************************************************************************/
	/* Shortest Remaining Time First Scheduling Algorithm */
	cout << "\n* Shortest Remaining Time First Scheduling Algorithm *\n" << endl;
	/* Gannt Chart */
	ganttChartSRTF(job_queue, p_map, total_time);
	/* Turnaround time And Waiting time */
	turnaroundAndWaitingTime(p_map, p, i);

	/****************************************************************************************************/
	return 0;
}
