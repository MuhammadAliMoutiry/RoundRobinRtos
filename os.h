#ifndef OS_H_
#define OS_H_

#include "os_types.h"
#define OS_TIME_1MS  50000
#define OS_TIME_2MS  2*OS_TIME_1MS


void OS_Init(void);
void OS_Add_Thread( void (*OSTask0Ptr)(void),
	                  void (*OSTask1Ptr)(void),
		                void (*OSTask2Ptr)(void));
void OS_Launch(uint32 OSTimeSlice);
										
void OS_Semaphore_Init(uint32 *SemaphoreCount , uint32 SemaphoreVal);
void OS_Semaphore_Wait(uint32 *SemaphoreCount);
void OS_Semaphore_Signal(uint32 *SemaphoreCount);
void OS_Send_mail(uint32 data);
uint32 OS_Receive_Mail(void);
										
#endif