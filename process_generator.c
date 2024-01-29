

/******************************************************************************
 *
 * Module: process_generator
 *
 * File Name: process_generator.c
 *
 * Description: C file for the process generator
 *
 *This file handles all the requirments of the process generator (refer to the PDF)
 *
 *******************************************************************************/

#include "headers.h"

#include "process_generator.h"

/*******************************************************************************
 *                      Functions definitions                                    *
 *******************************************************************************/
/*
 * Description : Function to clear all resources

 */

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0);
    // To Do : call the Destructor of the process queue
    Queue_destroy(&process_queue);
}

/*******************************************************************************
 *                      main                                      *
 *******************************************************************************/
//
int main(int argc, char *argv[])
{

    // initialization of the msg queue
    key_t key_id;
    int send_val;
    key_id = ftok("keyfile", 65);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);

    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }

    int stat_loc;
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    FILE *input_file;

    
    input_file = fopen(argv[1], "r");
    if (!input_file)
    {
        printf("couldn't open the input file \"processes.txt\"\n");
        exit(-1);
    }

    // skipping the first line
    fscanf(input_file, "%*[^\n]\n");

    process_par p;

    processNumbers = 0;
    // printf("%d %d %d %d",p.processNumber,p.arrival_time,p.runtime,p.priority);
    while (fscanf(input_file, "%d %d %d %d %d", &p.processNumber, &p.arrival_time, &p.runtime, &p.priority, &p.memSize) == 5)
    {
        process_par *ptr = malloc(sizeof(process_par));
        *ptr = p;
        Queue_push(&process_queue, (void *)ptr);
        printf("\n");
        processNumbers++;
    }

    fclose(input_file);

 

    SCHEDULING_ALGORITHM scheduling_algorithm = 0;
    int x = atoi(argv[3]);
    scheduling_algorithm = x;

    char* memoryAlgorithm;

    int time_chunk = -1;
    if (scheduling_algorithm == RR)
    {
        //printf("Enter the time chunk for each process to run RR algorithm:");
        time_chunk = atoi(argv[5]);
        memoryAlgorithm = (argv[7]);
    } else {
        memoryAlgorithm = (argv[7]);
    }

    
    

    // 3. Initiate and create the scheduler and clock processes.
    printf("Number of proccess =%d\nAlgorithm :%d\n", processNumbers, scheduling_algorithm);
    int Clock_pid = fork();
    if (Clock_pid < 0)
    {
        perror("Error in forking the Clock Process\n");
        exit(1);
    }
    if (Clock_pid == 0)
    {                                         // child process
        char *Clock_file[] = {"./clk", NULL}; // arguments for execv
        execv(Clock_file[0], Clock_file);     // execute the child process
        fprintf(stderr, "Exec failed\n");     // execv only returns if it fails
        exit(1);
    }
    
    int Scheduler_pid = fork();
    if (Scheduler_pid < 0)
    {
        perror("Error in forking the Scheduler Process\n");
        exit(1);
    }
    if (Scheduler_pid == 0)
    {                                      // child process
        char *buff = malloc(sizeof(char)); // we need to free these memory
        char *len = malloc(sizeof(char));
        char *timeChunk = malloc(sizeof(char));
        char *memo = malloc(sizeof(char));
        int x = scheduling_algorithm;
        snprintf(buff, sizeof(buff), "%d", x);
        snprintf(len, sizeof(len), "%d", processNumbers);
        snprintf(timeChunk, sizeof(timeChunk), "%d", time_chunk);
        snprintf(memo, sizeof(memo), "%d", memo);
        char *Scheduler_file[] = {"./scheduler", buff, len, timeChunk,memo, memoryAlgorithm, NULL}; // arguments for execv
        
        execv(Scheduler_file[0], Scheduler_file); // execute the child process
        fprintf(stderr, "Exec failed\n");         // execv only returns if it fails
        exit(1);
    }

    // 4. Use this function after creating the clock process to initialize clock
    initClk();

    // To get time use this
    // int x = getClk();
    // printf("current time is %d\n", x);

    // TODO Generation Main Loop
    int lastClk = -1;
    while (!isEmpty(&process_queue))
    {
        // if(lastClk==getClk()){
        //     continue;
        // }
        int cur_time = getClk();
        p = *((process_par *)Queue_peek(&process_queue));
        //    6. Send the information to the scheduler at the appropriate time.
        if (cur_time >= p.arrival_time)
        {
            //    send process to scheduler
            //            // use message queue don't forget the flag !IPC_NOWAIT
            msgbuff sentProcess;
            sentProcess.process = p;
            sentProcess.mtype = 7; /* arbitrary value */
            send_val = msgsnd(msgq_id, &sentProcess, sizeof(sentProcess.process), !IPC_NOWAIT);
            if (send_val == -1)
                perror("Errror in send");
            Queue_pop(&process_queue);
        }
        else
        {
            lastClk = cur_time;
        }
    }

    // 7. Clear clock resources

    /*Don't uncomment the destroyclk there is an exception thrown*/
    /*To Do: need to hndle the Exception */
    // destroyClk(true);

    waitpid(Scheduler_pid, &stat_loc, WUNTRACED);
    // TODO clear the resources
}
