Project Description: Operating System Simulator

Introduction:
The Operating System Simulator is a comprehensive project implemented in C++ on the Linux operating system. This simulator provides a platform to study and experiment with various aspects of operating systems, including CPU scheduling, semaphores, deadlocks (using the Bankers Algorithm), memory management, and page replacement algorithms. It aims to simulate the behavior and functionality of an operating system, allowing users to gain practical insights into these fundamental concepts.

Features:

CPU Scheduling:
The simulator implements different CPU scheduling algorithms, allowing users to explore their impact on process execution. It includes the following scheduling algorithms:
Multilevel Queue Scheduling: Three queues with different scheduling algorithms: priority scheduling, round-robin (RR), and shortest remaining job first (SRJF).
Multilevel Feedback Queue Scheduling: Three queues with different algorithms: first-come, first-served (FCFS), shortest job first (SJF), and round-robin (RR).
Longest Job First (LJF)
Lowest Feedback Ratio Next Scheduling (LFRN)
Users can customize parameters such as waiting time, burst time, and priority levels to observe the behavior of the scheduling algorithms.

Semaphores:
The simulator includes an implementation of the Dining Philosophers Problem using semaphores. This classic synchronization problem demonstrates how semaphores can be used to prevent deadlocks and ensure the fair allocation of resources. Users can observe the philosophers' behavior and understand how semaphores maintain synchronization and avoid deadlock scenarios.

Deadlock (Bankers Algorithm):
The simulator incorporates the Bankers Algorithm to handle deadlocks in a computer system. It ensures safe resource allocation to processes and avoids deadlock scenarios. Users can define the number of account holders and available resources, allowing the simulator to simulate the allocation process. This algorithm helps understand how operating systems manage resources efficiently to prevent deadlocks.

Memory Management:
The simulator provides support for two memory management schemes:

Two-Level Page-Table Scheme: Users can explore this scheme, which involves dividing memory into two levels for efficient page table lookup.
Hashed Page Table with TLB: This scheme utilizes a hashed page table combined with a Translation Lookaside Buffer (TLB). Users can analyze the efficiency of this approach for page table lookups.
Page Replacement:
The simulator implements the Second Chance Algorithm for page replacement. Users can observe the circular linked list data structure and how the algorithm determines which pages to replace based on a second chance bit. The simulator calculates page faults, page fault probability, and page fault percentage, providing insights into the performance of the page replacement algorithm.
Conclusion:
The Operating System Simulator is a comprehensive project that enables users to understand and experiment with essential operating system concepts. With features such as CPU scheduling, semaphores, deadlocks using the Bankers Algorithm, memory management, and page replacement, this simulator provides a valuable learning tool. By exploring and customizing different parameters and algorithms, users can gain practical insights into the behavior and efficiency of these fundamental operating system functionalities.
