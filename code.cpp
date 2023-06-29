#include<iostream>
#include<cstdlib>
#include<list>
#include<queue>
#include<string>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

using namespace std;

#define max 10
int bowl = 20;

int num;
sem_t forks[max];
sem_t temp;
pthread_t philosophers[max];

//----- Part 1 -------------------------------->
class Process {
public:
	int processID;
	int arrival;
	int burst;
	int queue;
	int priority;
	Process() {
		processID = -1;
		arrival = 0;
		burst = 0;
		queue = 0;
		priority = 0;
	}
};

struct CompareProcess {
	bool operator()(const Process& p1, const Process& p2) {
		if (p1.arrival == p2.arrival) {
			return p1.processID > p2.processID; // Use processID as tiebreaker
		}
		return p1.arrival > p2.arrival; // Sort based on arrival time
	}
};

void sortQueue(std::queue<Process>& q) {
	std::priority_queue<Process, std::vector<Process>, CompareProcess> sortedQueue;

	while (!q.empty()) {
		sortedQueue.push(q.front());
		q.pop();
	}

	while (!sortedQueue.empty()) {
		q.push(sortedQueue.top());
		sortedQueue.pop();
	}
}

void take_input_multi(int* (& all_info)[3], int& num, Process& prcs, queue<Process>& all, bool prior) {
	cout << "\nEnter number of processes > ";
	cin >> num;

	for (int i = 0; i < 3; i++) {
		all_info[i] = new int[num];
	}

	for (int i = 0; i < num; i++) {

		prcs.processID = i + 1;

		do {
			cout << "\nEnter Arrival time for Process ID. " << i + 1 << " > ";
			cin >> prcs.arrival;
		} while (prcs.arrival < 0);
		all_info[0][i] = prcs.arrival;

		do {
			cout << "\nEnter Burst time for Process ID. " << i + 1 << " > ";
			cin >> prcs.burst;
		} while (prcs.burst < 0);
		all_info[1][i] = prcs.burst;

		do {
			cout << "\nEnter Queue no. for Process ID. " << i + 1 << " > ";
			cin >> prcs.queue;
		} while (prcs.queue < 1 || prcs.queue>3);

		if (prcs.queue == 1 && prior) {
			do {
				cout << "\nEnter Priority for Process ID. " << i + 1 << " > ";
				cin >> prcs.priority;
			} while (prcs.priority < 0);
		}

		all.push(prcs);
	}

	sortQueue(all);
}

void display_output(int** all_info, int& num) {
	cout << "\n\nPrcs ID   Arrival   Burst   Completion   Waiting   Turnaround\n";
	for (int i = 0; i < num; i++) {
		cout << "  " << i + 1 << "          " << all_info[0][i] << "        " << all_info[1][i] << "        " << all_info[2][i];
		cout << "            " << all_info[2][i] - all_info[1][i] - all_info[0][i] << "           " << all_info[2][i] - all_info[0][i] << endl;
	}
}

void multi_level() {
	int num;
	Process prcs, previous, rr_prev;
	queue<Process> all, q1, q2, q3;

	int* all_info[3];

	take_input_multi(all_info,num, prcs, all,1);

	cout << "\n\nGantt Chart > ";

	int finished = 0;
	int time = 0, rr_time = 0;
	bool check = 1;

	while (!all.empty() || !q1.empty() || !q2.empty() || !q3.empty()) {
		check = 1;
		while (check && all.size() > 0) {
			prcs = all.front();
			if (prcs.arrival == time) {
				all.pop();
				switch (prcs.queue) {
				case 1:
					q1.push(prcs);
					break;
				case 2:
					q2.push(prcs);
					break;
				case 3:
					q3.push(prcs);
					break;
				default:
					perror("error in queue no.");
				}
			}
			else
				check = 0;
		}

		if (q1.size() > 0) { // q1 implementation (Priority)
			int highest_pr = -1;

			for (int i = 0; i < q1.size(); i++) {

				if (q1.front().priority > highest_pr) {
					prcs = q1.front();
					highest_pr = prcs.priority;
				}
				if (previous.processID == q1.front().processID && q1.front().priority >= highest_pr) {
					prcs = q1.front();
					highest_pr = prcs.priority;
				}
				q1.push(q1.front());
				q1.pop();
			}

			//----- to remove (which we are processing) process from queue
			for (int i = 0; i < q1.size(); i++) {

				if (q1.front().processID == prcs.processID) {
					q1.pop();
					break;
				}
				else {
					q1.push(q1.front());
					q1.pop();
				}
			}

			if (previous.processID != prcs.processID) {
				previous = prcs;
				cout << " " << time << " |P" << prcs.processID << "|";
			}
			prcs.burst--;
			if (prcs.burst > 0) {
				q1.push(prcs);
			}
			else {
				finished++;
				all_info[2][prcs.processID - 1]=time+1;
				if (finished == num)
					cout << " " << time + 1;
			}

		}
		else {
			if (q2.size() > 0) { // q2 implementation (RR, 2 time)

				if (rr_prev.burst > 0 && rr_time < 2) {
					prcs = rr_prev;
				}
				else {
					prcs = q2.front();
					rr_time = 0;
				}

				//----- to remove (which we are processing) process from queue
				for (int i = 0; i < q2.size(); i++) {

					if (q2.front().processID == prcs.processID) {
						q2.pop();
						break;
					}
					else {
						q2.push(q2.front());
						q2.pop();
					}
				}

				if (previous.processID != prcs.processID) {
					previous = prcs;
					cout << " " << time << " |P" << prcs.processID << "|";
				}
				prcs.burst--;
				rr_time++;
				rr_prev = prcs;
				if (prcs.burst > 0) {
					q2.push(prcs);
				}
				else {
					finished++;
					all_info[2][prcs.processID - 1] = time + 1;
					if (finished == num)
						cout << " " << time + 1;
				}


			}
			else {
				if (q3.size() > 0) { // q3 implementation (SRJF)

					int lowest_rj = 10000000;

					for (int i = 0; i < q3.size(); i++) {

						if (q3.front().burst < lowest_rj) {
							prcs = q3.front();
							lowest_rj = prcs.burst;
						}
						if (previous.processID == q3.front().processID && q3.front().burst <= lowest_rj) {
							prcs = q3.front();
							lowest_rj = prcs.burst;
						}
						q3.push(q3.front());
						q3.pop();
					}


					//----- to remove (which we are processing) process from queue
					for (int i = 0; i < q3.size(); i++) {

						if (q3.front().processID == prcs.processID) {
							q3.pop();
							break;
						}
						else {
							q3.push(q3.front());
							q3.pop();
						}
					}

					if (previous.processID != prcs.processID) {
						previous = prcs;
						cout << " " << time << " |P" << prcs.processID << "|";
					}
					prcs.burst--;
					if (prcs.burst > 0) {
						q3.push(prcs);
					}
					else {
						finished++;
						all_info[2][prcs.processID - 1] = time + 1;
						if (finished == num)
							cout << " " << time + 1;
					}


				}
				else {
					cout << " " << time;
				}
			}
		}
		time++;
	}
	display_output(all_info, num);
}

void multi_level_feedback() {
	int num;
	Process prcs, previous, rr_prev,srj_prev;
	queue<Process> all, q1, q2, q3;
	int* all_info[3];

	take_input_multi(all_info, num, prcs, all, 0);

	cout << "\n\nGantt Chart > ";

	int finished = 0;
	int time = 0, rr_time = 0, fcfs_time = 0, srj_time = 0;
	bool check = 1;

	while (!all.empty() || !q1.empty() || !q2.empty() || !q3.empty()) {
		check = 1;

		while (check && all.size() > 0) {
			prcs = all.front();
			if (prcs.arrival <= time) {
				all.pop();
				switch (prcs.queue) {
				case 1:
					q1.push(prcs);
					break;
				case 2:
					q2.push(prcs);
					break;
				case 3:
					q3.push(prcs);
					break;
				default:
					perror("error in queue no.");
				}
			}
			else
				check = 0;
		}

		if (q1.size() > 0) { // q1 implementation (FCFS, 3 time)
			
			prcs = q1.front();
			q1.pop();

			cout << " " << time << " |P" << prcs.processID << "|";
			while (fcfs_time<3 && prcs.burst>0) {
				fcfs_time++;
				time++;
				prcs.burst--;
			}

			previous = prcs;
			fcfs_time = 0;
			time--;

			if (prcs.burst > 0) {
				q2.push(prcs);
			}
			else {
				finished++;
				all_info[2][prcs.processID - 1] = time + 1;
				if (finished == num) {
					cout << " " << time+1;
				}
			}
		}
		else {
			if (q2.size() > 0) { // q2 implementation (SRJ, 3 time)
				int shortest_job = 100000000;

				if (srj_prev.burst > 0 && srj_time < 3) {
					prcs = srj_prev;
				}
				else {
					for (int i = 0; i < q2.size(); i++) {

						if (q2.front().burst < shortest_job) {
							prcs = q2.front();
							shortest_job = prcs.burst;
						}
						q2.push(q2.front());
						q2.pop();
					}
					srj_time = 0;
				}

				//----- to remove (which we are processing) process from queue
				for (int i = 0; i < q2.size(); i++) {

					if (q2.front().processID == prcs.processID) {
						q2.pop();
						break;
					}
					else {
						q2.push(q2.front());
						q2.pop();
					}
				}

				if (previous.processID != prcs.processID || srj_prev.processID != prcs.processID) {
					previous = prcs;
					cout << " " << time << " |P" << prcs.processID << "|";
				}
				prcs.burst--;
				srj_time++;
				srj_prev = prcs;
				previous = prcs;

				if (srj_time == 3) {
					if (prcs.burst > 0) {
						q3.push(prcs);
					}
					else {
						finished++;
						all_info[2][prcs.processID - 1] = time + 1;
						if (finished == num)
							cout << " " << time + 1;
					}

				}
				else {
					if (prcs.burst > 0) {
						q2.push(prcs);
					}
					else {
						finished++;
						all_info[2][prcs.processID - 1] = time + 1;
						if (finished == num)
							cout << " " << time + 1;
					}
				}
			}
			else {
				if (q3.size() > 0) { // q3 implementation (RR, 2 time)

					bool print_time = 0;

					if (rr_prev.burst > 0 && rr_time < 2) {
						prcs = rr_prev;
					}
					else {
						int shortest_job = 10000000;
						for (int i = 0; i < q3.size(); i++) {

							if (q3.front().burst < shortest_job) {
								prcs = q3.front();
								shortest_job = prcs.burst;
							}
							q3.push(q3.front());
							q3.pop();
						}
						print_time = 1;
						rr_time = 0;
					}

					//----- to remove (which we are processing) process from queue
					for (int i = 0; i < q3.size(); i++) {

						if (q3.front().processID == prcs.processID) {
							q3.pop();
							break;
						}
						else {
							q3.push(q3.front());
							q3.pop();
						}
					}

					if (previous.processID != prcs.processID || rr_prev.processID!=prcs.processID || print_time) {
						previous = prcs;
						cout << " " << time << " |P" << prcs.processID << "|";
					}
					prcs.burst--;
					rr_time++;
					rr_prev = prcs;
					previous = prcs;
					if (prcs.burst > 0) {
						q3.push(prcs);
					}
					else {
						finished++;
						all_info[2][prcs.processID - 1] = time + 1;
						if (finished == num)
							cout << " " << time + 1;
					}

				}
				else {
					cout << " " << time;
				}
			}
		}
		time++;
	}

	display_output(all_info, num);

}

void take_input_simple(int* (&all_info)[3], int& num, Process& prcs, queue<Process>& all) {
	cout << "\nEnter number of processes > ";
	cin >> num;

	for (int i = 0; i < 3; i++) {
		all_info[i] = new int[num];
	}

	for (int i = 0; i < num; i++) {

		prcs.processID = i + 1;

		do {
			cout << "\nEnter Arrival time for Process ID. " << i + 1 << " > ";
			cin >> prcs.arrival;
		} while (prcs.arrival < 0);
		all_info[0][i] = prcs.arrival;

		do {
			cout << "\nEnter Burst time for Process ID. " << i + 1 << " > ";
			cin >> prcs.burst;
		} while (prcs.burst < 0);
		all_info[1][i] = prcs.burst;

		all.push(prcs);
	}
	sortQueue(all);
}

void longest_job() {
	int num;
	
	Process prcs;
	queue<Process> all, ready;
	int* all_info[3];

	take_input_simple(all_info, num, prcs, all);
	
	cout << "\n\nGantt Chart > ";

	int finished = 0;
	bool check = 1;
	int time = 0;

	while (!all.empty() || !ready.empty()) {
		check = 1;
		while (check && all.size() > 0) {
			prcs = all.front();
			if (prcs.arrival <= time) {
				ready.push(prcs);
				all.pop();
			}
			else
				check = 0;
		}

		int longest_job = 0;
		if (ready.size() > 0) {
			for (int i = 0; i < ready.size(); i++) {

				if (ready.front().burst > longest_job) {
					prcs = ready.front();
					longest_job = prcs.burst;
				}
				ready.push(ready.front());
				ready.pop();
			}

			//----- to remove (which we are processing) process from queue
			for (int i = 0; i < ready.size(); i++) {

				if (ready.front().processID == prcs.processID) {
					ready.pop();
					break;
				}
				else {
					ready.push(ready.front());
					ready.pop();
				}
			}

			cout << " " << time << " |P" << prcs.processID << "|";
			while (prcs.burst > 0) {
				time++;
				prcs.burst--;
			}
			finished++;
			all_info[2][prcs.processID - 1] = time + 1;
			if (finished == num) {
				cout << " " << time;
			}
		}
		else {
			cout << " " << time;
			time++;
		}

	}

	display_output(all_info, num);
}

void lowest_feedback_ratio() {
	int num;
	Process prcs;
	queue<Process> all, ready;
	int* all_info[3];
	
	take_input_simple(all_info, num, prcs, all);

	cout << "\n\nGantt Chart > ";

	int finished = 0;
	bool check = 1;
	int time = 0;

	while (!all.empty() || !ready.empty()) {
		check = 1;
		while (check && all.size() > 0) {
			prcs = all.front();
			if (prcs.arrival <= time) {
				ready.push(prcs);
				all.pop();
			}
			else
				check = 0;
		}

		int low_feedback = 10000000;
		int cur_feed = 0;
		if (ready.size() > 0) {
			for (int i = 0; i < ready.size(); i++) {
				prcs = ready.front();
				cur_feed = ((time - prcs.arrival) + prcs.burst) / prcs.burst;
				if (cur_feed < low_feedback) {
					prcs = ready.front();
					low_feedback = cur_feed;
				}
				ready.push(ready.front());
				ready.pop();
			}

			//----- to remove (which we are processing) process from queue
			for (int i = 0; i < ready.size(); i++) {

				if (ready.front().processID == prcs.processID) {
					ready.pop();
					break;
				}
				else {
					ready.push(ready.front());
					ready.pop();
				}
			}

			cout << " " << time << " |P" << prcs.processID << "|";
			while (prcs.burst > 0) {
				time++;
				prcs.burst--;
			}
			finished++;
			all_info[2][prcs.processID - 1] = time + 1;
			if (finished == num) {
				cout << " " << time;
			}
		}
		else {
			cout << " " << time;
			time++;
		}

	}
	display_output(all_info, num);
}

void CPU_scheduling() {
	int opt = 0;
	cout << "-------------------------------------\n----- Welcome to CPU Scheduling -----\n-------------------------------------\n";
	do {
		cout << "\n-------------------------------------";
		cout << "\nEnter Where you want to go: \n1. Multi-Level Queue\n2. Multi-Level Feedback Queue\n3. Longest Job First\n4. Lowest Feedback Ratio Next\n0. <-Back\n";
		cout << "Enter your Choice > ";
		cin >> opt;
		cout << "-------------------------------------";
		switch (opt) {
		case 1:
			multi_level();
			break;
		case 2:
			multi_level_feedback();
			break;
		case 3:
			longest_job();
			break;
		case 4:
			lowest_feedback_ratio();
			break;
		case 0:
			break;
		default:
			cout << "\nPlease Enter any Valid Choice!";
		}

	} while (opt != 0);
}

//----- Part 2 -------------------------------->

void* philosopher(void* arg) {
	int id = *(int*)arg;
	int left_fork = id;
	int right_fork = (id + 1) % num;

	while (bowl > 0) {
		if (id != num) {
			sem_wait(&forks[left_fork]);
			sem_wait(&forks[right_fork]);
		}
		else {
			sem_wait(&forks[right_fork]);
			sem_wait(&forks[left_fork]);
		}
		cout << "Philosopher " << id + 1 << " takes fork " << left_fork << " and " << right_fork << endl;
		cout << "Philosopher " << id + 1 << " is eating." << endl;

		bowl--;
		sleep(1.5);

		// Release forks
		sem_post(&forks[left_fork]);
		sem_post(&forks[right_fork]);

		cout << "\nPhilosopher " << id + 1 << " is thinking." << endl;
		sleep(1.5);
	}

	pthread_exit(NULL);
}

void Semaphores() {
	cout << "-----------------------------------\n----- Welcome to Dining's Philospher -----\n-------------------------------------\n";
	int i = 0;
	int id = 0;
   
	cout << "\nEnter Number of Philosophers > ";
	cin >> num;

	for (i = 0; i < num; i++) {
		sem_init(&forks[i], 0, 1);
	}

	for (i = 0; i < num; i++) {
		id = i;
		pthread_create(&philosophers[i], NULL, philosopher, &id);
	}

	for (i = 0; i < num; i++) {
		pthread_join(philosophers[i], NULL);
	}
	cout << "\nBowl Ended!\n";
	for (i = 0; i < num; i++) {
		sem_destroy(&forks[i]);
	}
}

//----- Part 3 -------------------------------->

void take_bankers_input(int& processes, int& resources, int*& total_resources, int*& available, int**& maxreq, int**& allocated, int**& need) {
	cout << "\nEnter the number of Processes for Bankers Algorithm > ";
	cin >> processes;

	cout << "\nEnter the number of resources > ";
	cin >> resources;

	total_resources = new int[resources];
	cout << "\nEnter the total quantity for each resource: ";
	for (int i = 0; i < resources; i++) {
		cout << "\nEnter quantity for " << i + 1 << " resource > ";
		cin >> total_resources[i];
	}

	cout << endl << "\nEnter Info for Max Table below: ";

	maxreq = new int* [processes];
	for (int i = 0; i < processes; i++) {
		maxreq[i] = new int[resources];

		cout << "\nFor process " << i + 1 << ": ";
		for (int j = 0; j < resources; j++) {
			cout << "Enter Max requirment of resource " << j + 1 << " > ";
			cin >> maxreq[i][j];
		}
	}


	cout << endl << "\nEnter Info for Allocated Table below: ";

	allocated = new int* [processes];
	for (int i = 0; i < processes; i++) {
		allocated[i] = new int[resources];

		cout << "\nFor process " << i + 1 << ": ";
		for (int j = 0; j < resources; j++) {
			cout << "Enter Allocated no. of resource " << j + 1 << " > ";
			cin >> allocated[i][j];
		}
	}

	need = new int* [processes];
	//	cout << endl << "Need table working below" << endl;
	for (int i = 0; i < processes; i++) {
		need[i] = new int[resources];
		for (int j = 0; j < resources; j++) {
			need[i][j] = maxreq[i][j] - allocated[i][j];
		}
	}

	// printing tables
	cout << "All 3 tables\n<Max>  <Allocated>  <Need>" << endl;

	int k = 0;
	int l = 0;

	for (int i = 0; i < processes; i++) {
		for (int j = 0; j < resources; j++) {
			cout << maxreq[i][j] << " ";
		}
		cout << "     ";
		for (k = 0; k < resources; k++) {
			cout << allocated[i][k] << " ";
		}
		cout << "     ";
		for (l = 0; l < resources; l++) {
			cout << need[i][l] << " ";
		}
		cout << endl;
	}

	int* sumup;
	sumup = new int[resources];// to sumup each allocated resource and then subtract it from max to get available

	for (int j = 0; j < resources; j++) { // iterate through each column of the matrix
		sumup[j] = 0;
		for (int i = 0; i < processes; i++) {		// iterate through each row of the column

			sumup[j] += allocated[i][j];// add the current element to the column sum
		}
	}

	available = new int[resources];

	for (int i = 0; i < resources; i++) {
		available[i] = total_resources[i] - sumup[i];
	}

	int flag = 1;
	for (int i = 0; i < resources; i++) {
		if (available[i] <= 0) {
			cout << "\nNo availability for " << i + 1 << " resource!";
			flag = 0;
			break;
		}
	}
	if (flag == 1) {
		cout << "\nThe availabe resources are > ";
		for (int j = 0; j < resources; j++) { // iterate through each column of the matrix
			cout << available[j] << "  ";
		}
	}
}

void del_arrays(int processes, int resources, int** maxreq, int** allocated, int** need, int* available, int* total_resources) {
	for (int i = 0; i < processes; i++) {
		delete[] maxreq[i];
		delete[]allocated[i];
		delete[]need[i];
	}
	delete[] maxreq;
	delete[]allocated;
	delete[]need;
	delete[]available;
	delete[]total_resources;
}

bool safetyalgo(int processes, int resources, int** maxreq, int** allocated, int** need, int* available) {
	bool* processend = new bool[processes];
	int* safesequence = new int[processes];
	int* avail = new int[resources];
	int i = 0;



	for (i = 0; i < resources; i++) {
		avail[i] = available[i];
	}

	for (i = 0; i < processes; i++) {
		processend[i] = false;
	}


	int n = 0;
	while (n < processes)
	{
		bool found = false;
		for (int i = 0; i < processes; i++)
		{
			if (processend[i] == false)
			{
				int j;
				for (j = 0; j < resources; j++)
					if (need[i][j] > avail[j])
						break;

				if (j == resources)
				{
					for (int k = 0; k < resources; k++)
						avail[k] += allocated[i][k];

					processend[i] = true;
					safesequence[n++] = i;
					found = true;

				}
			}
		}

		if (found == false) {
			delete[] processend;
			delete[] safesequence;
			delete[] avail;
			return false;
		}
	}

	i = 0;
	cout << "\nThe safe sequence is > ";
	for (i = 0; i < processes; i++) {
		cout << "P" << safesequence[i] << " ";
	}
	cout << endl;
	delete[] processend;
	delete[] safesequence;
	delete[] avail;
	return true;
}

void resoursereq() {
	int processes, resources;
	int* total_resources, * available, ** maxreq, ** allocated, ** need;
	take_bankers_input(processes, resources, total_resources, available, maxreq, allocated, need);

	int rfp;//resuest for process;
	cout << "\nThe availability of resources before request > ";
	for (int j = 0; j < resources; j++) { // iterate through each column of the matrix
		cout << available[j] << " ";
	}

	do {
		cout << "\nEnter the Process you want to Request for Resource > ";
		cin >> rfp;

	} while (rfp > processes || rfp <= 0);

	cout << "Enter the request" << endl;
	int* request = new int[resources];
	for (int i = 0; i < resources; i++) {
		cout << "For Resource" << i + 1 << endl;
		cin >> request[i];
	}
	cout << "\nYour request > ";
	for (int j = 0; j < resources; j++) { // iterate through each column of the matrix
		cout << request[j] << " ";
	}

	bool condition = true;
	for (int i = 0; i < resources; i++) {
		if (request[i] > need[rfp][i]) {//rpf used for specified process
			condition = false;
			break;
		}
		if (request[i] > available[i])  // 2nd condition
			condition = false;
		break;
	}

	if (condition == true) {
		for (int i = 0; i < resources; i++) {
			available[i] -= request[i];
			allocated[rfp][i] += request[i];
			need[rfp][i] -= request[i];
		}
		cout << "\nNew calculations for process after request made" << endl;
		cout << "Available Resources \n";
		for (int i = 0; i < resources; i++) {
			cout << available[i];
		}
		cout << "\nNew Allocation \n";
		for (int i = 0; i < resources; i++) {
			cout << allocated[rfp][i];
		}
		cout << "\nNew Need \n";
		for (int i = 0; i < resources; i++) {
			cout << need[rfp][i];
		}


		if (safetyalgo(processes, resources, maxreq, allocated, need, available)) {
			cout << "Request successfull" << endl;

		}
		else {
			cout << "Resource request denied." << endl;
			for (int i = 0; i < resources; i++) {
				available[i] += request[i];
				allocated[rfp][i] -= request[i];
				need[rfp][i] += request[i];
			}
		}
		del_arrays(processes, resources, maxreq, allocated, need, available, total_resources);
	}
	else {
		del_arrays(processes, resources, maxreq, allocated, need, available, total_resources);
		cout << "Resource request denied." << endl;
	}
}

void Bankers() {
	int opt = 0;
	cout << "-------------------------------------\n----- Welcome to Banker's Algo -----\n-------------------------------------\n";
	do {
		cout << "\n-------------------------------------";
		cout << "\nEnter Where you want to go: \n1. Safety Algo\n2. Resourse request\n0. <-Back\n";
		cout << "Enter your Choice > ";
		cin >> opt;
		cout << "-------------------------------------";
		switch (opt) {
		case 1:
			int processes, resources;
			int* total_resources, * available, ** maxreq, ** allocated, ** need;
			take_bankers_input(processes, resources, total_resources, available, maxreq, allocated, need);
			if (safetyalgo(processes, resources, maxreq, allocated, need, available))
				cout << "\nSystem is in Safe State!";
			else {
				cout << "\nSystem is not in Safe State!";
			}
			del_arrays(processes, resources, maxreq, allocated, need, available, total_resources);
			break;
		case 2:
			resoursereq();
			break;
		case 0:
			break;
		default:
			cout << "\nPlease Enter any Valid Choice!";
		}
	} while (opt != 0);
}

//----- Part 4 -------------------------------->
int getProcessInfo(list<int>& free_frames) {
	int size;
	do {
		cout << "\nEnter the Size of Process (from 1 to 10000) >";
		cin >> size;
	} while (size < 0 || size>10000);

	if (size / 10 > free_frames.size()) {
		cout << "Not Enough Space in Memory";
		return 0;
	}

	int no_of_pages = size / 10;
	if (size % 10 > 0)
		no_of_pages++;
	return no_of_pages;
}

bool addProcess(char Memory[], list<int>& free_frames, int** inner_tables, int& inner_idx) {
	int no_of_pages = getProcessInfo(free_frames);
	if (no_of_pages == 0) {
		return 0;
	}

	string data = "..........";
	cout << "\nEnter any 10 letters that you want to place into Memory as Process's Data >";
	cin >> data;

	inner_tables[inner_idx] = new int[no_of_pages + 1];
	inner_tables[inner_idx][no_of_pages] = -1; // ending delimiter
	int fr_no;
	for (int i = 0; i < no_of_pages; i++) {
		fr_no = free_frames.front();
		free_frames.pop_front();
		inner_tables[inner_idx][i] = fr_no; //storing frame no. into page table

		for (int x = 0; x < data.size(); x++) { //writing process data into respective frame
			Memory[fr_no * 10 + x] = data[x];
		}
	}
	inner_idx++;
	return true;
}

int getTableSize(int* table) {
	int i = 0;
	for (i = 0; table[i] != -1; i++) { ; }
	return i;
}

void two_level_page_table() {
	//2,2,1 --> p1, p2, d
	char Memory[10000] = { '0' }; //--- initializing Memory
	for (int i = 0; i < 10000; i++) {
		Memory[i] = '-';
	}

	list<int> free_frames; //--- storing free frames
	for (int i = 0; i < 1000; i++) {
		free_frames.push_back(i);
	}
	int outer_table[100]; //--- declaring tables, and indexes
	int* inner_tables[100];
	for (int i = 0; i < 100; i++) {
		outer_table[i] = -1;
	}
	int inner_idx = 0;
	int opr = 0;
	int address = -1;
	do {
		cout << "\n-----------------------------------";
		cout << "\nWhat Operation you want to Perfrom.\n1. Add a Process\n2. Read Data\n3. End a Process\n0. Exit\nChoose your option> ";
		cin >> opr;
		cout << "-----------------------------------";
		switch (opr) {
		case 1:
			if (addProcess(Memory, free_frames, inner_tables, inner_idx)) {
				for (int i = 0; i < 100; i++) {
					if (outer_table[i] == -1) {
						outer_table[i] = inner_idx - 1;
						cout << "\nProcess Added Successfully! At outer table address " << i;
						break;
					}
					if (i == 99) {
						cout << "\nNo Space in Outer table!";
					}
				}
			}

			break;
		case 2:
			do {
				cout << "\nEnter a logical address of exact 6 letters to read data (first 2 letter for outer table, next 3 for inner table and last offset) >";
				cin >> address;
			} while (address < 0 || address>999999);

			if (outer_table[address / 10000] == -1) {
				cout << "\nThere's No Data at this Given Logical Address!";
			}
			else if ((address % 10000) / 10 >= getTableSize(inner_tables[outer_table[address / 10000]])) {
				cout << "\nThere's No Data at this Given Logical Address!";
			}
			else {
				cout << "\nData at " << inner_tables[outer_table[address / 10000]][(address % 10000) / 10] * 10 + (address % 10) << " physical address : " << Memory[inner_tables[outer_table[address / 10000]][(address % 10000) / 10] * 10 + (address % 10)];
			}

			break;
		case 3:
			do {
				cout << "\nEnter the Outer Page Table Address of 2 digits >";
				cin >> address;
			} while (address < 0 || address>99);

			if (outer_table[address] == -1) {
				cout << "\nNo Page Table at this Address!";
			}
			else {
				for (int i = 0; i < getTableSize(inner_tables[outer_table[address]]); i++) {
					int frame_no = inner_tables[outer_table[address]][i];
					free_frames.push_back(frame_no); // adding free frame back to the list
					for (int x = 0; x < 10; x++) {
						Memory[frame_no * 10 + x] = '.';
					}
				}
				delete[] inner_tables[outer_table[address]];
				inner_tables[outer_table[address]] = NULL;
				outer_table[address] = -1;
			}
			break;
		case 0:
			break;
		default:
			cout << "\nPlease Enter Valid Option!";
			break;
		}
	} while (opr != 0);

}

struct HashedNode {
	int page_no;
	int frame_no;
	void insert(int p, int f) {
		page_no = p;
		frame_no = f;
	}
};

int hash_func(int virtual_page_num, int hash_table_size) {
	int hash = 5381;
	hash = ((hash << 5) + hash) + virtual_page_num;
	return hash % hash_table_size;
}

bool addHashedProcess(char Memory[], list<int>& free_frames, list<HashedNode>** page_tables, int& table_idx) {
	int no_of_pages = getProcessInfo(free_frames);
	if (no_of_pages == 0) {
		return 0;
	}

	string data = "..........";
	cout << "\nEnter any 10 letters that you want to place into Memory as Process's Data >";
	cin >> data;
	page_tables[table_idx] = new list<HashedNode>[100];

	int fr_no;
	HashedNode page;
	for (int i = 0; i < no_of_pages; i++) {
		fr_no = free_frames.front();
		free_frames.pop_front();
		page.insert(i, fr_no);

		int hash_idx = hash_func(i, 100);

		page_tables[table_idx][hash_idx].push_back(page); // inserting into list

		for (int x = 0; x < data.size(); x++) { //writing process data into respective frame
			Memory[fr_no * 10 + x] = data[x];
		}
	}
	table_idx++;
	return true;
}

int get_hashed_physical_address(list<HashedNode>** page_tables, int log_address) {
	int physical = 0;
	if (page_tables[log_address / 10000] == NULL) {
		return -1;
	}
	else {
		int frame = -1, index = hash_func((log_address % 10000) / 10, 100);
		HashedNode front;
		for (int i = 0; i < page_tables[log_address / 10000][index].size(); i++) {
			front = page_tables[log_address / 10000][index].front();
			page_tables[log_address / 10000][index].pop_front();
			page_tables[log_address / 10000][index].push_back(front);

			if (front.page_no == (log_address % 10000) / 10) {
				frame = front.frame_no;
				break;
			}
		}
		if (frame == -1) {
			return -1;
		}
		else
			return frame * 10 + (log_address % 10);
	}
}

void hashed_page_table() {
	//frame size 10 bytes
	char Memory[10000] = { '0' }; //--- initializing Memory
	for (int i = 0; i < 10000; i++) {
		Memory[i] = '-';
	}

	list<int> free_frames; //--- storing free frames
	for (int i = 0; i < 1000; i++) {
		free_frames.push_back(i);
	}

	list<HashedNode>* page_tables[10];
	for (int i = 0; i < 10; i++) {
		page_tables[i] = NULL;
	}
	int table_idx = 0;

	int TLB[10][2], TLB_idx = 0;
	for (int i = 0; i < 10; i++) {
		TLB[i][0] = -1;
	}

	int opr = 0;
	int address = -1;
	bool TLB_hit = 0;
	do {
		cout << "\n-----------------------------------";
		cout << "\nWhat Operation you want to Perfrom.\n1. Add a Process\n2. Read Data\n3. End a Process\n0. Exit\nChoose your option> ";
		cin >> opr;
		cout << "-----------------------------------";
		switch (opr) {
		case 1:
			if (addHashedProcess(Memory, free_frames, page_tables, table_idx))
				cout << "\nProcess Added Successfully! At Page Table address " << table_idx - 1 << endl;

			break;
		case 2:
			do {
				cout << "\nEnter a logical address of exact 5 letters to read data (first letter for table idx, next 3 for page no. and last offset) >";
				cin >> address;
			} while (address < 0 || address>99999);

			//---------- TLB Check
			TLB_hit = 0;
			for (int i = 0; i < 10; i++) {
				if (TLB[i][0] == address) {
					cout << "\nTLB HIT! Data : " << (char)TLB[i][1] << endl;
					TLB_hit = 1;
					break;
				}
			}
			if (!TLB_hit) {
				int physical_add = get_hashed_physical_address(page_tables, address);
				if (physical_add == -1) {
					cout << "\nInvalid Logical Address!\n";
				}
				else {
					cout << "\nData at " << physical_add << " physical address : " << Memory[physical_add] << endl;
					if (TLB_idx == 10)
						TLB_idx = 0;
					TLB[TLB_idx][0] = address;
					TLB[TLB_idx++][1] = Memory[physical_add];
				}
			}
			break;
		case 3:
			do {
				cout << "\nEnter the Outer Page Table Address of 1 digits >";
				cin >> address;
			} while (address < 0 || address>9);

			if (page_tables[address] == NULL) {
				cout << "\nNo Page Table at this Address!\n";
			}
			else {
				HashedNode page;
				for (int i = 0; i < 100; i++) { // 100 is the size of Hashed page table
					for (int j = 0; j < page_tables[address][i].size(); j++) {
						page = page_tables[address][i].front();
						page_tables[address][i].pop_front();
						int frame_no = page.frame_no;
						for (int x = 0; x < 10; x++) {
							Memory[frame_no * 10 + x] = '.';
						}
					}
				}
				delete[] page_tables[address];
				page_tables[address] = NULL;
				for (int i = 0; i < 10; i++) {
					if (TLB[i][0] / 1000 == address)
						TLB[i][0] = -1;
				}
			}
			break;
		case 0:
			break;
		default:
			cout << "\nPlease Enter Valid Option!\n";
			break;
		}
	} while (opr != 0);

}

void Memory_Management() {
	int opt = 0;
	cout << "----------------------------------------\n----- Welcome to Memory Management -----\n----------------------------------------\n";
	do {
		cout << "\n---------------------------------------";
		cout << "\nEnter Where you want to go: \n1. Two-Level Paging\n2. Hashed Page Tables\n0. <-Back\n";
		cout << "Enter your Choice > ";
		cin >> opt;
		cout << "---------------------------------------";
		switch (opt) {
		case 1:
			two_level_page_table();
			break;
		case 2:
			hashed_page_table();
			break;
		case 0:
			break;
		default:
			cout << "\nPlease Enter any Valid Choice!";
		}

	} while (opt != 0);
}

//----- Part 5 -------------------------------->
class Node {
public:
	int data;
	bool chance;
	Node* next;
	Node(int data_, bool ch) {
		data = data_;
		chance = ch;
		next = NULL;
	}
};

class CircularLinkedList {
private:
	Node* tail;
	Node* traverse;
public:
	CircularLinkedList() {
		tail = NULL;
		traverse = NULL;
	}
	void insert(int data) {
		Node* new_node = new Node(data, 0);
		if (tail == NULL) {
			tail = new_node;
			new_node->next = new_node;
		}
		else {
			new_node->next = tail->next;
			tail->next = new_node;
			tail = new_node;
		}
		traverse = tail->next;
	}

	bool search(int data) {
		Node* current = tail->next;
		do {
			if (current->data == data) {
				current->chance = 1;
				return true;
			}
			current = current->next;
		} while (current != tail->next);
		return false;
	}

	void replace(int data, int& faults) {

		if (search(data)) {
			cout << "\n*--- Page already present in RAM ---*\n";
			return;
		}
		else {
			cout << "\n*--- Page Fault ---*\n";
			while (traverse->chance == 1) {
				traverse->chance = 0;
				traverse = traverse->next;
			}
			traverse->data = data;
			traverse->chance = 1;
			traverse = traverse->next;
			faults++;
		}
	}

	void display() {
		Node* current = tail->next;

		cout << " RAM:> ";
		do {
			if (current->data == -1) {
				cout << "NULL  ";
			}
			else
				cout << current->data << "(" << current->chance << ")" << "  ";
			current = current->next;
		} while (current != tail->next);
		cout << endl;
	}

	// Destructor
	~CircularLinkedList() {
		if (tail == NULL) {
			return;
		}
		Node* current = tail->next;
		while (current != tail) {
			Node* temp = current;
			current = current->next;
			delete temp;
		}
		delete tail;
		tail = NULL;
	}
};

void Page_Replacement() {
	int RAM_Size, Pages, Page_no, faults = 0;;
	cout << "\n Enter RAM Size (No. of Frames) >";

	cin >> RAM_Size;
	cout << "\n Enter Total No. of Pages >";
	cin >> Pages;

	CircularLinkedList RAM;

	for (int i = 0; i < RAM_Size; i++) {
		RAM.insert(-1); //allocating memory to RAM
	}

	for (int i = 0; i < Pages; i++) {
		cout << "\n Enter Page Number >";
		cin >> Page_no;
		RAM.replace(Page_no, faults);
		RAM.display();
	}
	cout << "\nNo. of Page Faults:> " << faults;
	cout << "\nProbability of Page Faults:> " << (float)faults / Pages;
	cout << "\nPercentage of Page Faults:> " << (float)faults / Pages * 100;
	cout << "\n\n";
}

int main() {

	int opt = 0;
		do {
		system("clear");
	    cout << "--------------------------------------\n------- Welcome to OS Simulator ------\n--------------------------------------\n";
		cout << "\n--------------------------------------------------------------------------";
		cout << "\nEnter Where you want to go: \n1. CPU Scheduling\n2. Dining Philosopher\n3. Banker's Algo\n4. Memory Management\n5. Page Replacement\n0. Exit\n";
		cout << "Enter your Choice > ";
		cin >> opt;
		cout << "--------------------------------------------------------------------------\n";
		switch (opt) {
		case 1:
			system("clear");
			CPU_scheduling();
			break;
		case 2:
			system("clear");
			Semaphores();
			break;
		case 3:
			system("clear");
			Bankers();
			break;
		case 4:
    		system("clear");
			Memory_Management();
			break;
		case 5:
			system("clear");
			Page_Replacement();
			break;
		case 0:
			break;
		default:
			cout << "\nPlease Enter any Valid Choice!";
		}

	} while (opt != 0);

	return 0;
}