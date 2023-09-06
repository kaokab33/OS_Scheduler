#include "headers.h"

/*************Functions*************/
void highestPriorityFirst();
void shortestRemainnigTime();
void roundRobin();
void createprocess();
float calculateWaitSD();
void schedularchoose();
void signalFinish(int segnum);
// void finalize(int segnum);
/********************************/

/*************Variables*************/
struct PCB *currk;
struct PCB currProc, lastProc, temp;
int msgid, shmid2;
key_t key, key_sh;
FILE *schedulerFile;
FILE *schedulerFile2;
FILE *memFile;
PriorityQueue readyQueue;
PriorityQueue unallocatedProcess;
LinkedList holeList;
LinkedList processList;
char choice;
char memChoice;
int quantum;
int noProcess, totalProcesses;
int time;
int totalWTA;
int totalWT;
int counter;
int *waitingTimeArr;
int totalUsedTime;
int srtn_run;
int *shared;
int lastDQ = 0;
struct BuddyNode **memArray;
struct BuddyNode *root;
/********************************/

struct msgbuff
{
    long mtype;
    struct PCB recvpcd;
};

int main(int argc, char *argv[])
{
    printf("\nScheduler has started .. \n");
    insert(&holeList, 0, 1023, -1);
    root = newBuddyNode(1024, 1, 0, 1023); // Allocating memory of size 1024

    /***************Initiallizing IPCs***************/
    key_sh = ftok("tempfile", 's');
    shmid2 = shmget(key_sh, 4096, IPC_CREAT | 0666);
    key = ftok("tempfile", 'a');
    msgid = msgget(key, 0666 | IPC_CREAT);
    struct msgbuff recvmess;

    if (shmid2 == -1)
    {
        printf("\nError in creating shared memory\n");
        killpg(getpgrp(), SIGINT);
    }
    if (msgid == -1)
    {
        printf("\nError in creating msgQ\n");
        killpg(getpgrp(), SIGINT);
    }

    /***************Signal Handelers***************/
    signal(SIGUSR1, signalFinish);
    //   signal(SIGINT, finalize);

    /***************Initializations***************/
    choice = argv[1][0];
    quantum = atoi(argv[2]);
    noProcess = atoi(argv[3]);
    totalProcesses = noProcess;
    memChoice = argv[4][0];
    time = -1;

    shared = (int *)shmat(shmid2, (void *)0, 0);
    memArray = malloc((noProcess + 1) * sizeof(struct BuddyNode *));
    waitingTimeArr = malloc((noProcess + 1) * sizeof(int));
    setpqueue(&readyQueue);
    setpqueue(&unallocatedProcess);

    /***************Openning output files***************/
    schedulerFile = fopen("scheduler.log", "w");
    memFile = fopen("memory.log", "w");
    if (schedulerFile == NULL || memFile == NULL)
    {
        printf("\nCan't open output files\n");
        killpg(getpgrp(), SIGINT);
    }

    initClk();
    while (noProcess) // While there are processes
    {

        if (time != getClk()) // Is a cycle finished?
        {
            printf("\nCurrent time = %d\n", getClk());
            time = getClk();
            while (msgrcv(msgid, &recvmess, sizeof(recvmess.recvpcd), 0, IPC_NOWAIT) != -1) // Checking if there is processes to recieve
            {
                temp = recvmess.recvpcd; // a comment
                createprocess(temp);
            }
            schedularchoose();
        }
    }

    schedulerFile2 = fopen("scheduler.perf", "w");
    if (schedulerFile2 == NULL)
    {
        printf("\nCan't open output file\n");
        killpg(getpgrp(), SIGINT);
    }
    fprintf(schedulerFile2, "CPU utillization = %.2f %%", ((float)totalUsedTime) / getClk() * 100);
    fprintf(schedulerFile2, "\nAvg WTA = %.2f\n", ((float)totalWTA) / atoi(argv[3]));
    fprintf(schedulerFile2, "Avg Waiting = %.2f\n", ((float)totalWT) / counter);
    fprintf(schedulerFile2, "Std WTA = %.2f\n", calculateWaitSD());
    fclose(schedulerFile2);
    fclose(schedulerFile);
    fclose(memFile);
    freeList(&holeList);
    freeList(&processList);

    shmctl(shmid2, IPC_RMID, (struct shmid_ds *)0);
    msgctl(msgid, IPC_RMID, (struct msqid_ds *)0);
    for (int i = 0; i <= totalProcesses; i++)
    {
        //  free(memArray[i]);
    }
    free(memArray);
    free(waitingTimeArr);
    kill(getpgrp(), SIGINT);
    destroyClk(true);
    return 0;
}
void createprocess(struct PCB processTemp)
{
    if (memChoice == 'f') // First fit technique
    {
        LinkedListNode theNode = *holeAllocation(&holeList, processTemp.memsize);
        if (theNode.start == -1) // No memory available for this process now
        {
            enqueue(&unallocatedProcess, processTemp, processTemp.memsize);
            return;
        }
        theNode.procNumber = processTemp.id;
        insert(&processList, theNode.start, theNode.end, theNode.procNumber);
        fprintf(memFile, "At time %d allocated %d bytes for process %d from %d to %d\n", getClk(),
                processTemp.memsize, processTemp.id, theNode.start, theNode.end);
    }
    else // Buddyfit technique
    {
        struct BuddyNode *theNode = findBuddyNode(root, processTemp.memsize);

        if (theNode == NULL) // No memory available for this process now
        {
            enqueue(&unallocatedProcess, processTemp, processTemp.memsize);
            return;
        }
        theNode->processNumber = processTemp.id;
        memArray[processTemp.id] = theNode;
        fprintf(memFile, "At time %d allocated %d bytes for process %d from %d to %d\n", getClk(),
                processTemp.memsize, processTemp.id, theNode->start, theNode->ending);
    }

    switch (choice) // Enqueue according to the scheduling algo
    {
    case 'r':
        enqueue(&readyQueue, processTemp, 1000);
        break;
    case 'p':
        enqueue(&readyQueue, processTemp, processTemp.Priority);
        break;
    case 's':
        enqueue(&readyQueue, processTemp, processTemp.RemainingTime);
        break;

    default:
        break;
    }
}

void signalFinish(int segnum)
{
    if (choice == 's')
        return;
    if (readyQueue.head != NULL && choice != 'p')
    {
        readyQueue.head->pcb.RemainingTime = 0;
        if (choice == 'r')
        {
            currProc = readyQueue.head->pcb;
            currk->state = Stopped;
        }
    }

    if (choice != 's')
        currProc.state = Stopped;

    lastProc = currProc;
    totalUsedTime += lastProc.RunTime;
    lastProc.state = Stopped;
    lastProc.TA = getClk() - lastProc.ArrTime + 1;
    lastProc.WaitTime = lastProc.TA - lastProc.RunTime;
    lastProc.WTA = lastProc.TA / lastProc.RunTime;
    totalWTA += lastProc.WTA;
    totalWT += lastProc.WaitTime;
    lastProc.endTime = getClk();
    lastProc.RemainingTime = 0;
    waitingTimeArr[counter++] = lastProc.WaitTime;
    int total = lastProc.endTime - lastProc.startTime + 1;
    fprintf(schedulerFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",
            getClk(), lastProc.id, lastProc.ArrTime, total, lastProc.RemainingTime,
            lastProc.WaitTime, lastProc.TA, lastProc.WTA);

    noProcess--;
    lastDQ = getClk();

    if (memChoice == 'f')
    {
        LinkedListNode tempNode = *searchProcessNumber(&processList, lastProc.id);
        deleteLinkedListNode(&processList, &tempNode);
        holeFree(&holeList, &tempNode);
        fprintf(memFile, "At time %d freed %d bytes for process %d from %d to %d\n", getClk(),
                lastProc.memsize, lastProc.id, tempNode.start, tempNode.end);
        int prev = -1;
        while (!isEmpty(&unallocatedProcess))
        {
            if (prev == unallocatedProcess.head->pcb.id)
                break;
            struct PCB newPCB = dequeue(&unallocatedProcess);
            createprocess(newPCB);
            if (isEmpty(&unallocatedProcess))
                break;
            prev = unallocatedProcess.head->pcb.id;
        }
    }
    else
    {
        freeBuddyNode(memArray[lastProc.id]);
        fprintf(memFile, "At time %d freed %d bytes for process %d from %d to %d\n", getClk(),
                lastProc.memsize, lastProc.id, memArray[lastProc.id]->start, memArray[lastProc.id]->ending);
        int prev = -1;
        while (!isEmpty(&unallocatedProcess) && noProcess != 0)
        {
            if (prev == unallocatedProcess.head->pcb.id)
                break;
            struct PCB newPCB = dequeue(&unallocatedProcess);
            createprocess(newPCB);
            if (isEmpty(&unallocatedProcess))
                break;
            prev = unallocatedProcess.head->pcb.id;
        }
    }
    signal(SIGUSR1, signalFinish);
}

void highestPriorityFirst()
{
    if (currProc.state != Running) // If there is no process running now
    {
        if (readyQueue.count == 0 || noProcess == 0) // No processes to run
        {
            return;
        }
        while (getClk() == lastDQ)
            ;
        currProc = dequeue(&readyQueue); // Getting next process
        currProc.state = Running;
        char qr[5];
        currProc.startTime = getClk();
        currProc.WaitTime = getClk() - currProc.ArrTime - (currProc.RunTime - currProc.RemainingTime);

        fprintf(schedulerFile, "At time %d process %d started arr %d total %d remain %d wait %d \n",
                getClk(), currProc.id, currProc.ArrTime, 0, currProc.RemainingTime, currProc.WaitTime);
        int pid = fork(); // Forking new process
        if (pid == 0)
        {
            sprintf(qr, "%d", currProc.RunTime); // Convert integer to string
            execl("process.out", "", &qr, NULL);
        }
        else
        {
            currProc.PID = pid;
        }
    }
}

void schedularchoose()
{
    switch (choice)
    {
    case 'r':
        roundRobin();
        break;
    case 'p':
        highestPriorityFirst();
        break;
    case 's':
        shortestRemainnigTime();
        break;
    }
}
void roundRobin()
{

    if (isEmpty(&readyQueue))
        return;
    while (!isEmpty(&readyQueue) && readyQueue.head->pcb.state == Stopped)
    {
        dequeue(&readyQueue); // Removing finished processess from ready queue
    }
    if (isEmpty(&readyQueue))
        return;
    int currTime = getClk();
    currk = &readyQueue.head->pcb;
    if (readyQueue.head->pcb.state == Running) // There is a process running and we need to pause it
    {
        kill(currk->PID, SIGSTOP);
        if (currk->RemainingTime - *shared >= quantum) // Passed quantum ?
        {
            currk->RemainingTime = *shared;
            // currk->WaitTime = getClk() - currk->ArrTime - (currk->RunTime - currk->RemainingTime) +2;

            kill(currk->PID, SIGSTOP);
            lastDQ = currTime;
            currk->state = Waiting;

            fprintf(schedulerFile, "At time %d process %d stopped arr %d total %d remain %d wait %d \n",
                    currTime, currk->id, currk->ArrTime, currTime - currk->startTime + 1, currk->RemainingTime, currk->WaitTime);
            if (currk->state != Stopped && currk->RemainingTime != 0)
                enqueue(&readyQueue, dequeue(&readyQueue), 1000);
            else if (currk->state != Stopped)
            {
                raise(SIGUSR1);
                dequeue(&readyQueue);
            }
        }
        else // Qunatun not passed yet!
        {
            kill(currk->PID, SIGCONT);
            return;
        }
    }

    while (!isEmpty(&readyQueue) && readyQueue.head->pcb.state == Stopped)
    {
        dequeue(&readyQueue);
    }

    if (isEmpty(&readyQueue))
        return;

    while (lastDQ == getClk())
        ;
    currTime = getClk();
    currk = &readyQueue.head->pcb;
    if (currk->state == Waiting) // Continue a process
    {

        currk->WaitTime = currTime - currk->ArrTime - (currk->RunTime - currk->RemainingTime);
        fprintf(schedulerFile, "At time %d process %d resumed arr %d total %d remain %d wait %d \n",
                currTime, currk->id, currk->ArrTime, currTime - currk->startTime, currk->RemainingTime, currk->WaitTime);
        currk->state = Running;
        *shared = currk->RemainingTime;
        kill(currk->PID, SIGCONT);
        return;
    }

    else if (currk->state == NotStarted) // Forking new process
    {
        currk->WaitTime = currTime - currk->ArrTime - (currk->RunTime - currk->RemainingTime);
        fprintf(schedulerFile, "At time %d process %d started arr %d total %d remain %d wait %d \n",
                currTime, currk->id, currk->ArrTime, 0, currk->RemainingTime, currk->WaitTime);
        int pid = fork();
        if (pid == 0)
        {
            char qr[5];
            sprintf(qr, "%d", currk->RunTime); // Convert integer to string
            execl("process.out", "", qr, NULL);
        }
        else
        {
            currk->startTime = currTime;
            currk->state = Running;
            currk->PID = pid;
            *shared = currk->RemainingTime;
        }
    }
}

float calculateWaitSD()
{
    float sum = 0.0, mean, SD = 0.0;
    int i;
    for (i = 0; i < counter; ++i)
    {
        sum += waitingTimeArr[i];
    }
    mean = sum / 10;
    for (i = 0; i < counter; ++i)
    {
        SD += pow(waitingTimeArr[i] - mean, 2);
    }
    return sqrt(SD / 10);
}

void shortestRemainnigTime()
{
    if (currk != NULL)
    {
        currk->RemainingTime = (*shared);
    }
    if (isEmpty(&readyQueue))
        return;
    char qr[5];
    currProc = peek(&readyQueue);
    if (currk == NULL)
    {
        if (lastDQ == getClk()) // Same cycle
        {
            return;
        }
        currk = &readyQueue.head->pcb;
        if (currk->state == Waiting) // Continue a process
        {
            
            kill(currk->PID, SIGCONT);
            //  currk->RemainingTime = (*shared);
            currk->WaitTime = getClk() - currk->ArrTime - (currk->RunTime - currk->RemainingTime);
            fprintf(schedulerFile, "At time %d process %d resumed arr %d total %d remain %d wait %d \n",
                    getClk(), currk->id, currk->ArrTime, getClk() - currk->startTime + 1, currk->RemainingTime, currk->WaitTime);

            currk->state = Running;
        }
        else // Fork a new process
        {
            int pid = fork();
            if (pid == 0)
            {
                sprintf(qr, "%d", currk->RunTime); // Convert integer to string
                execl("process.out", "", qr, NULL);
                exit(0);
            }
            else
            {
                currk->RemainingTime = currk->RunTime;
                currk->startTime = getClk();
                currk->WaitTime = getClk() - currk->ArrTime - (currk->RunTime - currk->RemainingTime);
                fprintf(schedulerFile, "At time %d process %d started arr %d total %d remain %d wait %d \n",
                        getClk(), currk->id, currk->ArrTime, 0, currk->RemainingTime, currk->WaitTime);

                currk->state = Running;
                currk->PID = pid;
            }
        }
    }

    else if (readyQueue.head->pcb.id != currk->id) // Stopping currently running process
    {
        lastDQ = getClk();
        kill(currk->PID, SIGUSR2);
        kill(currk->PID, SIGSTOP);
        currk->RemainingTime = (*shared);
        currk->state = Waiting;
        // currk->WaitTime = getClk() - currk->ArrTime - (currk->RunTime - currk->RemainingTime) + 1;
        fprintf(schedulerFile, "At time %d process %d stopped arr %d total %d remain %d wait %d \n",
                getClk(), currk->id, currk->ArrTime, getClk() - currk->startTime + 1, currk->RemainingTime, currk->WaitTime);
        if (lastDQ == getClk())
        {
            currk = NULL;
            return;
        }

        currk = &readyQueue.head->pcb;
        if (currk->state == Waiting)
        {
            
            currk->state = Running;
            kill(currk->PID, SIGCONT);
            // currk->RemainingTime = (*shared);
            currk->WaitTime = getClk() - currk->ArrTime - (currk->RunTime - currk->RemainingTime) + 1;
            fprintf(schedulerFile, "At time %d process %d resumed arr %d total %d remain %d wait %d \n",
                    getClk(), currk->id, currk->ArrTime, getClk() - currk->startTime + 1, currk->RemainingTime, currk->WaitTime);
        }
        else
        {
            int pid = fork();
            if (pid == 0)
            {
                sprintf(qr, "%d", currk->RunTime); // Convert integer to string
                execl("process.out", "", qr, NULL);
                exit(0);
            }
            else
            {
                currk->RemainingTime = currk->RunTime;
                currk->startTime = getClk();
                currk->WaitTime = getClk() - currk->ArrTime - (currk->RunTime - currk->RemainingTime) + 1;
                fprintf(schedulerFile, "At time %d process %d started arr %d total %d remain %d wait %d \n",
                        getClk(), currk->id, currk->ArrTime, 0, currk->RemainingTime, currk->WaitTime);
                currk->state = Running;
                currk->PID = pid;
            }
        }
    }
    else if ((currk->RemainingTime = (*shared)) == 0)
    {
        currk->RemainingTime = 0;
        currProc = readyQueue.head->pcb;
        lastProc = currProc;
        totalUsedTime += lastProc.RunTime;
        lastProc.state = Stopped;
        lastProc.TA = getClk() - lastProc.ArrTime + 1;
        lastProc.WaitTime = lastProc.TA - lastProc.RunTime;
        lastProc.WTA = lastProc.TA / lastProc.RunTime;
        totalWTA += lastProc.WTA;
        totalWT += lastProc.WaitTime;
        lastProc.endTime = getClk();
        lastProc.RemainingTime = 0;
        waitingTimeArr[counter++] = lastProc.WaitTime;
        int total = lastProc.endTime - lastProc.startTime + 1;
        fprintf(schedulerFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",
                getClk(), lastProc.id, lastProc.ArrTime, total, lastProc.RemainingTime,
                lastProc.WaitTime, lastProc.TA, lastProc.WTA);

        noProcess--;
        lastDQ = getClk();
        dequeue(&readyQueue);
        currk = NULL;
    }
}
