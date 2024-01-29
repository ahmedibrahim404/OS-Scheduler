/******************************************************************************
 *
 * Module: scheduler
 *
 * File Name: scheduler.h
 *
 * Description: header file for the scheduler
 *
 *******************************************************************************/

#ifndef scheduler
#define scheduler
#include "headers.h"
#define MAXSIZE 200
#define NOMOREPROCESS -1
#define TOTAL_MEMORY_SIZE 1024
#define NOTALLOCATED -1
#define ALLOCATED 1
#define MAX_PROCESSNUMBER 5000
#define PROCESS_WAITING -3
#define PROCESS_NOTWAITING -4
/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
    RUNNING,
    WAITING,
    FINISHED,
    NOTSTARTED
} PROCESS_STATUS;
typedef enum
{
    PROCESS_STARTED,
    PROCESS_STOPPED,
    PROCESS_RESUMED,
    PROCESS_FINISHED
} EVENT_TYPE;

typedef struct
{
    int pid; // process pid that returned from the fork
    PROCESS_STATUS status;
    int arrival_time;
    int excutionTime;
    int remainingTime;
    int waitingTime;
    int startTime;
    int finishTime;
    int priority;
    int lastTimeStopped; // indicator to calculate the waiting time for each process
    int lastTimeStartted;
    int TA;
    int WTA;
    int memSize;
    int memStart;
} PCB;

// 2 4 1 0 5
// 7 8 10 11
// Queue<processPar>

typedef struct
{
    int processNumber;
    EVENT_TYPE type;
    int waitingTime;
    int remainingTime;
    int time;
} Event;

typedef enum
{
    FIRSTFIT,
    BUDDYMEMORY
} Memory_Algorithm;

typedef enum
{
    ALLOCATION_EVENT,
    DEALLOCATION_EVENT
} MEMORY_EVENT_TYPE;

typedef struct
{
    int processNumber;
    int clock;
    int startAddress;
    int endAddress;
    int memSize;
    MEMORY_EVENT_TYPE type;
} Memory_Event;

int curSize = 0;
process_par heap[MAXSIZE];

/*******************************************************************************
 *                     global variables                                    *
 *******************************************************************************/
SCHEDULING_ALGORITHM Scheduler;
int processNumbers; /*Number of processes*/
int timeChunk;      /*special variable for the case of Round Robin time slite*/
int msgq_id;        /*msg queue to communicate between the process_generator and the scheduler*/
msgbuff recProcess;
int rec_val;
int stat_loc;
PCB processTable[MAXSIZE];
short flag = false;
int index = 1;         // current index in the processTable
short lastProcessFlag; // a flag that indicate the last process that is to be recieved
// it is updated in the scheduler_recieveNewProcess
// when the scheduler recieves a process with id = -1 just an indicator

queue g_eventQueue;
queue g_memEventQueue;
queue tempQ; // thi is RR Queue
// memory variables

short memory[TOTAL_MEMORY_SIZE];
short memoryAlgorithm = BUDDYMEMORY;
short waitingMemoryList[MAX_PROCESSNUMBER];
process_par waitingProcesses[MAX_PROCESSNUMBER];
int nowRun = -1;

// 1 ->>> is alocated , -1 ->> is free
/*******************************************************************************
 *                     priority Queue implementation                                     *
 *******************************************************************************/

// priority queue functions

// auxiliary fucnctions

static int parent(int i)
{
    return (i - 1) / 2;
}

static int left(int i)
{
    return 2 * i + 1;
}

static int right(int i)
{
    return 2 * i + 2;
}
int memoo3;
// 1--> 1 > 2
// 0--> 1 == 2
// -1--> 1 < 2
static int compare(int idx1, int idx2, SCHEDULING_ALGORITHM s)
{
    int val1 = 0, val2 = 0;
    if (s == HPF)
    {
        val1 = heap[idx1].priority;
        val2 = heap[idx2].priority;
    }
    else if (s == SRTN)
    {
        val1 = processTable[idx1].remainingTime;
        val2 = processTable[idx2].remainingTime;
        // printf("%d %d\n",val1,val2);
    }

    if (val1 > val2)
        return 1;
    if (val1 == val2)
        return 0;
    if (val1 < val2)
        return -1;
}

static void heapify(int root, SCHEDULING_ALGORITHM s)
{
    int l = left(root), r = right(root);
    int mn = root;
    if (l < curSize && compare(l, mn, s) == -1)
        mn = l;
    if (r < curSize && compare(r, mn, s) == -1)
        mn = r;

    if (mn != root)
    {
        process_par tmp = heap[root];
        heap[root] = heap[mn];
        heap[mn] = tmp;
        heapify(mn, s);
    }
}

// main functions

process_par top()
{
    return heap[0];
}

void pop(SCHEDULING_ALGORITHM s)
{
    if (curSize == 1)
    {
        curSize--;
        return;
    }
    heap[0] = heap[curSize - 1];
    curSize--;
    heapify(0, s);
}

void push(process_par newP, SCHEDULING_ALGORITHM s)
{
    curSize++;
    heap[curSize - 1] = newP;
    int pos = curSize - 1;
    while (pos > 0 && compare(pos, parent(pos), s) == -1)
    {
        process_par tmp = heap[parent(pos)];
        heap[parent(pos)] = heap[pos];
        heap[pos] = tmp;
        pos = parent(pos);
    }
}

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description : Function to initalize the Scheduler
 inputs: 1-Number of processes
         2-Scheduling algorithm
 */
void Scheduler_init(int count, SCHEDULING_ALGORITHM s, int chunk);
/**

*/
// void Scheduler_processStop();
/**
 * Descritption:checks if there is a new process arrived from the process
 * generator and if so it pushes it in your data structure passed
 */
void Scheduler_recieveNewProcess(void *container);

/**
 * Fork a new process and give its parameters
 * called when we run the proecss for the first time
 */
int Scheduler_processStart(process_par *newProcess);
/**
 * Resume a stopped Process
 * inpts : the pid of the process  (pid is the id that returnd when forking a process)
 * don't conflict between the pid and the id of the text file
 */
void Scheduler_processResume(int processNumber);
/**
 * stop a running Process
 * inpts : the pid of the process  (pid is the id that returnd when forking a process)
 * don't conflict between the pid and the id of the text file
 */
void Scheduler_processStop(int processNumber);

void Scheduler_generateOutputFiles();

void Scheduler_processFinishHandler(int signum);

/*******************************************************************************
 *                    First_Fit Algorithm functuins prototype                                  *
 *******************************************************************************/
void FirstFit_init();
short FirstFit_allocateNewProcess(int, int);
short FirstFit_deAllocateProcess(int);

struct memory_block
{

    bool is_allocated;
    bool is_split;
    int size;
    int processNumber;
    int startMem;

} mem[2047];

void buddy_goBuild(int idx, int pw, int start)
{
    if (pw < 0)
        return;

    mem[idx].size = 1 << pw;
    mem[idx].startMem = start;
    mem[idx].is_allocated = mem[idx].is_split = 0;
    mem[idx].processNumber = -1;

    buddy_goBuild(left(idx), pw - 1, start);
    buddy_goBuild(right(idx), pw - 1, start + mem[idx].size / 2);
}

int buddy_allocate(int idx, int reqSize, int pid)
{
    if (reqSize > mem[idx].size)
        return -1;
    if (!mem[idx].is_split && !mem[idx].is_allocated && reqSize <= mem[idx].size && reqSize > mem[idx].size / 2)
    {
        printf("Allocate memory from %d to %d for process %d %d\n", mem[idx].startMem, mem[idx].startMem + mem[idx].size - 1, pid, reqSize);
        mem[idx].is_allocated = 1;
        mem[idx].processNumber = pid;
        return mem[idx].startMem;
    }
    if (mem[idx].is_allocated)
        return -1;
    int ans = buddy_allocate(left(idx), reqSize, pid);
    if (ans == -1)
        ans = buddy_allocate(right(idx), reqSize, pid);
    if (ans != -1)
        mem[idx].is_split = 1;
    return ans;
}

int buddy_deallocate(int idx, int pid, int sz)
{

    if(sz < 0) return 0;


    if (mem[idx].is_allocated)
    {
        if (mem[idx].processNumber == pid)
        {
            mem[idx].is_allocated = 0;
            return 0;
        }
        else
        {
            return 1;
        }
    }

    if (buddy_deallocate(left(idx),  pid, sz-1) == 0 && buddy_deallocate(right(idx), pid, sz-1) == 0)
    {
        mem[idx].is_allocated = mem[idx].is_split = 0;
        return 0;
    }


    return 1;
}

void MEMORY_pushAllocateEvent(int processNum);
void MEMORY_pushDeAllocateEvent(int processNum);
void Memory_generateMemoryFile();

/*******************************************************************************
 *                      Main Algorithms                                   *
 *******************************************************************************/

void Scheduler_HPF();
void Scheduler_SRTN();
void Scheduler_RR();

#endif
