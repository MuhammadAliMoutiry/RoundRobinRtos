#ifndef OS_H_
#define OS_H_

#include "os_types.h"
#define OS_TIME_1MS  50000
#define OS_TIME_2MS  2*OS_TIME_1MS
/*****************************************************************************/
/*           FIFO       */

#define OS_FIFO_SIZE  10


/*****************************************************************************/
/*           Function prototypes       */

void OS_Init(void);
void OS_Add_Thread( void (*OSTask0Ptr)(void),
	                  void (*OSTask1Ptr)(void),
		                void (*OSTask2Ptr)(void));
void OS_Launch(uint32 OSTimeSlice);
										
void OS_Semaphore_Init(int32 *SemaphoreCount , int32 SemaphoreVal);
										
void OS_SpinLock_Semaphore_Wait  (int32 *SemaphoreCount);
void OS_SpinLock_Semaphore_Signal(int32 *SemaphoreCount);

void OS_Cooperative_Semaphore_Wait  (int32 *SemaphoreCount);
void OS_Cooperative_Semaphore_Signal(int32 *SemaphoreCount);
	
void OS_Blocking_Semaphore_Wait  (int32 *SemaphoreCount);
void OS_Blocking_Semaphore_Signal(int32 *SemaphoreCount);		
										
void   OS_Send_mail(uint32 data);
uint32 OS_Receive_Mail(void);
										
void OS_Suspend(void);
void OS_Sleep(int32 SleepTimeMilliSec)	;
										
void   OS_FIFO_Init(void);
void   OS_FIFO_Put(uint32 data);
uint32 OS_FIFO_Get(void);										
#endif