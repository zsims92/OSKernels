/*
 * Main.c
 *
 *  Created on: Oct 18, 2013
 *      Author: Zach Sims and Elliot Mitchell
 */
#include <stdio.h>
#include <malloc.h>
#include "sys/alt_alarm.h"
#include "alt_types.h"
#include "../main.h"

TCB threads[NUM_THREADS];
TCB* currThread = NULL;
int* mainSP;
int* mainFP;
int first = 1;

void mythread(int thread_id){
	// The declaration of j as an integer was added on 10/24/2011
	int i, j, n;

	n = (thread_id % 2 == 0)? 10: 15;
	for (i = 0; i < n; i++){
		printf("This is message %d of thread #%d.\n", i, thread_id);
		for (j = 0; j < MAX; j++);
	}
}

void mythread_scheduler(int sp, int fp){
	DISABLE_INTERRUPTS();
	int dc;
	if(first){
		mainSP = (int)sp;
		mainFP = (int)fp;
		first = 0;
	}
	else{
		currThread->sp = (int)sp;
		currThread->fp = (int)fp;
	}

	int id;
	//This determines which thread to run or which thread is running
	//Uses a basic FIFO thread order
	if(runnable_threads > 0){
		if(currThread == NULL){
			id = 0;
		}
		else{
			id = currThread->id;
			id = (id + 1) % NUM_THREADS;
			while(currThread->status == DONE){
				id = (id + 1) % NUM_THREADS;
			}
		}
		currThread = &threads[id];
	}
	//If there are threads to be ran
	if(runnable_threads > 0){
		 asm("add r4, r0, %0": "=r"(dc): "r"(currThread->sp));
		 asm("add r5, r0, %0": "=r"(dc): "r"(currThread->fp));
	}
	else{//There are no mor threads to run
		 printf("Interrupted by the DE2 timer!\n");
	}
	ENABLE_INTERRUPTS();
}

alt_u32 mythread_handler (void * param_list){
	global_flag = 1;
	return ALARMTICKS(35); //Using the defined function above
}

void newTCB(TCB* tcb, int i){
	//Determing the addres of mythread
	//So the context of new thread can
	//be changed to start at mythread
	int x = (int)mythread;
	int y = (int)destroy;
	//Create a context to hold the context
	//of the thread
	tcb->context = malloc(sizeof(int) * 40);
	//Create a stack to hold the context
	//of the prototype_os()
	tcb->context[20] = y;
	tcb->context[25] = i;
	tcb->context[38] = x;
	tcb->sp = &tcb->context[39];
	tcb->fp = &tcb->context[39];
	printf("%d %d %d\n", &tcb->context[21], tcb->sp, tcb->fp);
}

void mythread_join(int i){
	//Tell the scheduler that a thread is available
	runnable_threads++;
	//Set that thread to ready
	threads[i].status = READY;
}

void mythread_create(int i){
	//Creates a new thread with
	//the given ID
	TCB tcb;
	newTCB(&tcb, i);
	threads[i] = tcb;
	//Sets the thread off until
	//mythread_join is used
	//on this thread
	threads[i].status = DONE;
}
/*
 * This function will clean up the current TCB and
 * then return to the original function unless it
 * get interrupted.  It alerts the user that the program
 * is here first.  It then loads the context of the
 * prototype_os here to return to it until another interrupt
 * Finally it free the context and stack of the currentTCB
 * and turns the thread off *
 * */
void destroy(){
	printf("In destroy function\n");

	runnable_threads -= 1;
	currThread->status = DONE;
	free(currThread->context);
}

void prototype_os()
{
	int i;
	for(i=0; i<NUM_THREADS; i++){
		mythread_create(i);
	}

	for(i=0; i<NUM_THREADS; i++){
		mythread_join(i);
	}

	//Set the timer to be 1 second
	alt_alarm_start(&alarm, 10, mythread_handler, NULL);

	while (1)
	{
		printf ("This is the prototype os for my exciting CSE351 course projects!\n");
		int j=0;
		//MAX is the amount of wait time before the next printf above
		for (j = 0 ; j < MAX; j++);
	}
}

int main(){
	prototype_os();
	return 0;
}

