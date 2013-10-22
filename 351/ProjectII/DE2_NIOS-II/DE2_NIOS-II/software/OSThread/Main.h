/*
 * Main.h
 *
 *  Created on: Oct 18, 2013
 *      Author: Zach
 */

#ifndef MAIN_H_
#define MAIN_H_

#define NUM_THREADS 8
//Defined a macro to calculate the time in seconds
#define ALARMTICKS(x) ((alt_ticks_per_second()*(x))/10)
//MAX value to wait before normal OS operation
#define MAX 100000
// disable an interrupt

#define DISABLE_INTERRUPTS() { asm("wrctl status, zero"); }
#define ENABLE_INTERRUPTS() { asm("movi et, 1");asm("wrctl status, et");}

int runnable_threads;
static alt_alarm alarm;
int global_flag;

typedef enum{
	READY=1,
	RUNNING,
	DONE,
	PAUSE
} THREAD_STATUS;

typedef struct{
	int id;
	THREAD_STATUS status;
	int* stack;
	int* context;  //Should be 7 registers
} TCB;

#endif /* MAIN_H_ */
