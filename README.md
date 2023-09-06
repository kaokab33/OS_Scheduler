# OS_Scheduler <img src="https://github.com/YaraHisham61/OS_Scheduler/assets/88517271/6c99eaf7-4d14-42d4-8844-d9992bb5990a" width="30" height="30" />
A simulation of scheduling processes by the operating system using C language and IPCs

## <img align= center width=50px height=50px src="https://thumbs.gfycat.com/HeftyDescriptiveChimneyswift-size_restricted.gif"> Table of Contents

- [Overview](#Overview)
- [System Description](#sysdes) 
- [Data Structure](#ds)
- [How To Run](#run)
- [Output](#out)
- [Contributors](#contributors)

<hr style="background-color: #4b4c60"></hr>


## <img src="https://github.com/YaraHisham61/OS_Scheduler/assets/88517271/34402b46-ead8-415c-96f6-a21dce7fa14d" width="30" height="30" /> Overview <a name = "Overview"></a>
This is our project for the Operating Systems course at Cairo University. The project simulates the work of the operating system scheduler with the processes in the system using
3 different algorithms to run the processes which are:
1. Non-preemptive Highest Priority First (HPF)
2. Shortest Remaining time Next (SRTN)
3. Round Robin (RR)

While it uses 2 different algorithms for memory allocations which are:
1. First Fit.
2. Buddy memory allocation.

## <img src="https://github.com/YaraHisham61/OS_Scheduler/assets/88517271/d8e6c9f3-9ba5-4d9e-a108-7d9a95989812" width="30" height="30" /> System Description <a name = "sysdes"></a>
![image](https://github.com/YaraHisham61/OS_Scheduler/assets/88517271/a97a24d6-2438-431e-b097-3efc7c6f8b43)

## <img src="https://github.com/YaraHisham61/OS_Scheduler/assets/88517271/41cd74fb-7e37-492e-b15d-5f54bccfd43e" width="30" height="30" /> Data Structures <a name = "ds"></a>
* Priority queue
   - Ready queue: To hold the processes PCBs arranged according to the scheduling algorithm.

      | Scheduling algorithm 	| Priority queue key |
      |:----------------------|:-------------------|
      | Highest priority first	| Processes priority |
      | Shortest remaining time next | The remaining time (initially it’s the process runtime)|
      | Round robin |	Arriving time |
      
    - Unallocated Processes: To hold the process that can’t be currently allocated due to memory limits
* Linked list
  - Used in first fit memory management algorithm
  - Hole linked list: To hold the free blocks of memory and their location
  - Processes linked list: To hold the allocated processes address space

* Binary search tree
  - Buddy lists: To hold memory addresses and specify whether they are allocated or not
 

## <img src="https://github.com/YaraHisham61/OS_Scheduler/assets/88517271/1c40c081-3619-449b-a9d7-605fc7b2baca" width="30" height="30" />  How To Run <a name = "run"></a>
1) Clone the project
```
git clone https://github.com/YaraHisham61/OS_Scheduler
 ```
 
2) Go to the project directory
  ```
  cd code
  ```
  
3) Compile your project using the command:
 ```
 make  
 ```
4) Choose the scheduling algorithm by modifying Makefile
 ```
 -sch 1 : HPF
 -sch 2 : SRTN
 -sch 3 : RR
 ```

5) Choose the memory allocation algorithm by modifying Makefile
 ```
 -mem 1: First fit
 -mem 2: Buddy
 ```
6) Modify process.txt to the desired test case
7) Run your project using the command:
 ```
make run
 ```

 ## <img src="https://github.com/YaraHisham61/OS_Scheduler/assets/88517271/52e64c12-9638-45ba-9040-d6545e67a1df" width="35" height="35" /> Outputs Files <a name = "out"></a>
 * scheduler.log
 * scheduler.perf
 * memory.log

  ## <img src="https://github.com/YaraHisham61/OS_Scheduler/assets/88517271/859c6d0a-d951-4135-b420-6ca35c403803" width="30" height="30" /> Contributors <a name = "contributors"></a>
- [@AhmedSamy02](https://github.com/AhmedSamy02)
- [@kaokab33](https://github.com/kaokab33)
- [@nancyalgazzar](https://github.com/nancyalgazzar)
- [@YaraHisham61](https://github.com/YaraHisham61)
