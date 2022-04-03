/*
 * main.c 
 * 
 * Copyright 2022 raja <raja@raja-IdeaPad-Gaming-3-15IMH05>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 
Embedded simple task

Please write a simple program in C language using FreeRTOS system and its API.
Setup

Please find the below instruction as a example setup for the task. It is project with FreeRTOS emulator dedicated for Linux OS distributions. Feel free to use it for completion of the coding challenge if feasible:

    Download and unzip project with FreeRTOS emulator.
    Install required dependencies for Linux:

sudo apt-get install libc6-dev-i386

    Modify main source file Project/main.c in order to create required program.
    Build program:

make

    Run program:

./sim_FreeRTOS

Behavior

Program should follows below rules:

    There is an account balance with money (it cannot store negative value), can be in two states:

    a) lock - doesn't allow to change balance

    b) unlock - allows to change balance

    Every operation - adding/subtracting - on the account should produce the log record of current balance and its state (lock/unlock), print them to the console.

    There are three asynchronous task working in parallel:

    a) First - every 3 seconds adds 300 money to the account.

    b) Second - every 5 seconds takes 500 money from the account, if there is enough money to collect.

    c) Third - every 30 second lock account for 10 seconds, after that unlock it again.
*/
/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"		/* RTOS firmware */
#include "task.h"			/* Task */
#include "timers.h"
#include "queue.h"
#include "semphr.h" // include semaphore!

#pragma pack(1)
struct stateAndBalance
{
	uint8_t state;
	uint64_t balance;
}sab;

typedef enum stateOfAcc { lock, unlock };

static const char *stateOfAcc[] = { "lock", "unlock" };

void vApplicationIdleHook(void);


//void vTask(void* parameter);

/*
 * @brief: Step a: Rule 3- Declare three tasks 
*/ 
void vFirstTask(void* parameter);
void vSecondTask(void* parameter);
void vThirdTask(void* parameter);


static SemaphoreHandle_t bin_sem;
static SemaphoreHandle_t mutex;
/* --------------------------------------------- */
/*             Main application                  */
/* --------------------------------------------- */

int main ( void )
{
	//xTaskCreate( vTask, "Task 1", 1000, "Inne", 1, NULL );

	//Some initializations
	sab.balance = 0 ; 
	sab.state = unlock;
	
	//Initialize a semaphore before starting tasks
	
	//bin_sem = xSemaphoreCreateBinary();
	
	 // Create mutex before starting tasks
  mutex = xSemaphoreCreateMutex();


	//Step a: Rule 3 - There are three asynchronous task working in parallel. 
	//They are of equal priority
	xTaskCreate( vFirstTask, "Task 1", 1024, NULL, 1, NULL );
	xTaskCreate( vSecondTask,"Task 2", 1024, NULL, 1, NULL);
	xTaskCreate( vThirdTask, "Task 3", 1024, NULL, 1, NULL );
	//xSemaphoreTake(bin_sem, 10);

	// start the scheduler
	vTaskStartScheduler();
	return 0;
}

/*
 * @brief: vFirstTask - This is the first task. 
 * every 3 seconds adds 300 money to the account.
 */ 

void vFirstTask(void* parameter)
{
	while(1)
	{
		printf("Task 1\n");
		
  // Release the binary semaphore
  //xSemaphoreGive(bin_sem);
	if(sab.state == unlock)
	{
	printf("Current Balance = %d\n", sab.balance);
	sab.balance =  sab.balance + 300;
	printf("New Balance = %d\n", sab.balance);
	}
	vTaskDelay(pdMS_TO_TICKS(3000)); // The first task should run every 3 seconds
	}
}

/*
 * @brief: Step a: Rule 3- Declare three tasks 
 */ 
void vSecondTask(void* parameter)
{
	while(1)
	{
	printf("Task 2 - sub 500\n");
	   
	if(sab.balance >= 500 && sab.state == unlock)
	{
    printf("Current Balance = %d\n", sab.balance);
	sab.balance =  sab.balance - 500;
	printf("New Balance = %d\n", sab.balance);
	}
	//xSemaphoreGive(mutex);

    
	vTaskDelay(pdMS_TO_TICKS(5000)); // The second task should run every 5 seconds
	}
}

/*
 * @brief: Step a: Rule 3- Declare three tasks 
 */ 
void vThirdTask(void* parameter)
{
	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(30000)); // The third task should run every 30 seconds
	 printf("Task 3 - locked\n");
		if (xSemaphoreTake(mutex, 0) == pdTRUE) 
	   {
		   //locking is the critical section
	 sab.state = lock;
	 vTaskDelay(pdMS_TO_TICKS(10000)); // Keep it locked for 10 seconds.
     sab.state = unlock; // Now unlock it
	 printf("Task 3 - unlocked\n");
	 // Give mutex after critical section
      xSemaphoreGive(mutex);
    }
	}
}

#if 0
void vTask(void* parameter) 
{
    char *title = parameter;

    while(1){
        printf("%s: Please fill me\n", title);
		vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#endif

/* --------------------------------------------- */
/*        Function required by FreeRTOS          */
/* --------------------------------------------- */

void vApplicationIdleHook(void)
{
	// printf("Idle\r\n");
}

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
 	taskENTER_CRITICAL();
	{
        printf("[ASSERT] %s:%lu\n", pcFileName, ulLine);
        fflush(stdout);
	}
	taskEXIT_CRITICAL();
	exit(-1);
}
