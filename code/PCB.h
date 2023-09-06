#ifndef pcbn
#define pcbn

typedef short STATE;
#define NotStarted 0
#define Waiting 1
#define Running 2
#define Stopped 3

// defining the PCB struct (Process Control Block)
struct PCB
{
    int id;            // process id
    int ArrTime;       // time of arrival
    int RunTime;       // running time (in state of running it must be devremented)
    int Priority;      // priority (the less the number the higher the priority)
    int WaitTime;      // time spent waiting
    int state;         // ready or waiting or running or terminated
    int TA;            // turnaround time
    int RemainingTime; // Remaining time
    int startTime;     // Starting time
    int endTime;       // Finishing time
    double WTA;        // weighted tur arround time
    int PID;           // pid of the actual created process
    int memsize;       // process size
};

// sets the main prameters of the pcb
//  at the initialization the pcb hasn't arrived
void setPCB(struct PCB *pcb, int ID, int ARR, int RUN, int Pr, int size)
{
    pcb->id = ID;
    pcb->ArrTime = ARR;
    pcb->RunTime = RUN;
    pcb->RemainingTime = pcb->RunTime;
    pcb->Priority = Pr;
    pcb->RemainingTime = RUN;
    pcb->state = NotStarted;
    pcb->WaitTime = 0;
    pcb->memsize = size;
}
#endif