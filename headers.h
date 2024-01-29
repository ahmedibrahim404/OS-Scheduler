
#ifndef headers
#define headers

#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define true 1
#define false 0

#define SHKEY 300

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
typedef short bool;

typedef struct
{
	int processNumber;
	int arrival_time;
	int runtime;
	int priority;
	int memSize;
} process_par;

typedef enum
{
	HPF,
	SRTN,
	RR
} SCHEDULING_ALGORITHM;
typedef struct
{
	long mtype;
	process_par process;
} msgbuff;

// Define the queue node structure
struct node
{
	void *data;
	struct node *next;
};

// Define the queue structure
typedef struct
{
	struct node *front;
	struct node *rear;
} queue;

/*Priority Queue implementation with heap     */

/*******************************************************************************
 *                        Global Variables                                   *
 *******************************************************************************/
int total_time;
int original_process_numbers;
///==============================
// don't mess with this variable//
int *shmaddr; //
//===============================

/*******************************************************************************
 *                      Functions  definitions                                   *
 *******************************************************************************/

// Description:
// for each data type used in this project as a queue we define a print function
// to keep the generic of the queue implementation
// refer to the Queue_print
void Print_int(void *data)
{
	printf("%d", *((int *)data));
}

/*******************************************************************************
 *                     type generic queue implementation                                   *
 *******************************************************************************/

// Initialize the queue
void Queue_init(queue *q)
{
	q->front = NULL;
	q->rear = NULL;
}

int isEmpty(queue *q)
{
	return q->front == NULL && q->rear == NULL;
}			

// Add an item to the queue
void Queue_push(queue *q, void *value)
{
	struct node *new_node = (struct node *)malloc(sizeof(struct node));
	if (new_node == NULL)
	{
		fprintf(stderr, "Error: Memory allocation failed\n");
		exit(1);
	}
	new_node->data = value;
	new_node->next = NULL;
	if (q->front == NULL)
	{
		q->front = new_node;
	}
	else
	{
		q->rear->next = new_node;
	}
	q->rear = new_node;
}

// Remove an item from the queue
void *Queue_pop(queue *q)
{
	if (q->front == NULL)
	{
		fprintf(stderr, "Error: Queue is empty\n");
		exit(1);
	}
	void *data = q->front->data;
	struct node *temp = q->front;
	q->front = q->front->next;
	free(temp);
	if (q->front == NULL)
	{
		q->rear = NULL;
	}
	return data;
}
void *Queue_peek(queue *q)
{
	if (!q->front)
	{
		return NULL;
	}
	return q->front->data;
}
// Print the contents of the queue
void Queue_print(queue *q, void (*ptr)(void *))
{
	struct node *current_node = q->front;
	while (current_node != NULL)
	{
		ptr(current_node->data);
		printf(" ");
		current_node = current_node->next;
	}
	printf("\n");
}

void Queue_destroy(queue *q)
{
	struct node *current_node = q->front;
	while (current_node != NULL)
	{
		struct node *next_node = current_node->next;
		// free_func(current_node->data);
		free(current_node);
		current_node = next_node;
	}
	q->front = NULL;
	q->rear = NULL;
}

/*******************************************************************************
 *                      clock functions                                   *
 *******************************************************************************/

int getClk()
{
	return *shmaddr;
}

///*
// * All process call this function at the beginning to establish communication between them and the clock module.
// * Again, remember that the clock is only emulation!
//*/
void initClk()
{
	int shmid = shmget(SHKEY, 4, 0444);
	while ((int)shmid == -1)
	{
		// Make sure that the clock exists
		printf("Wait! The clock not initialized yet!\n");
		sleep(1);
		shmid = shmget(SHKEY, 4, 0444);
	}
	shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
 */

void destroyClk(bool terminateAll)
{
	// printf("NOO");
	shmdt(shmaddr);
	if (terminateAll)
	{
		killpg(getpgrp(), SIGINT);
	}
}
#endif