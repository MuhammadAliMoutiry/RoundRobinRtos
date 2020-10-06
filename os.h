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
#endif