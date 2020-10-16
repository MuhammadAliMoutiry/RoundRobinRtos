
#include "NVIC.h"
#include "PLL.h"
#include "os.h"


#define OS_NUM_THREADS       3
#define OS_THREAD_STACK_SIZE 100


typedef struct OSThreadTCB{
   uint32 *OSThreadSP;
	 int32  *OSThreadBlocked;
  // int32  OSThreadSleep  ;
   struct OSThreadTCB *OSNextThread;	
}OSThreadTCB_t;

OSThreadTCB_t OSThreadsTCBs[OS_NUM_THREADS];
uint32        OSThreadsStacks[OS_NUM_THREADS][OS_THREAD_STACK_SIZE];

OSThreadTCB_t *OSRunningThreadPtr;
// Function definitions in osasm.s
void   OS_ASM_Disable_Interrupts(void          );
void   OS_ASM_Enable_Interrupts (void          );
uint32 StartCritical            (void          );
void   EndCritical              (uint32 primask);
void   OS_Start                 (void          );   
/************************************************************************************************/
uint32  mail                 ;
int32   mailSendSemaphore =0 ;
int32   mailAckSemaphore  =0 ;
/************************************************************************************************/
/*           FIFO   Globals    */
/*****************************************************************************/
uint32 volatile *OSFIFOPutPtr ;
uint32 volatile *OSFIFOGetPtr ;
uint32 volatile OSFIFO[OS_FIFO_SIZE];
/* FIFO Semaphores */
int32 OSFIFOCurrentSize; // 0 means FIFO is empty
int32 OSFIFORoomLeft   ; // 0 means FIFO is FUll
int32 OSFIFOMutex      ; // execlusive access to FIFO
/*********************************************************************************/
/*
void OS_Timer0A_Handler(void){
 uint8 i;
	for(i=0 ; i<OS_NUM_THREADS ; i++){
		if(OSThreadsTCBs[i].OSThreadSleep){
			OSThreadsTCBs[i].OSThreadSleep--;
		}
	}
}
*/
void OS_Init(void){
  OS_ASM_Disable_Interrupts();
	PLL_Init();          // set processor clock to 50 MHz
	SYSTICK_Disable();   
  SYSTICK_Prioity_7(); // Assign lowest priority to SYSTICK

}

void OS_Thread_Stack_Init(uint8 threadIndx){
	OSThreadsTCBs[threadIndx].OSThreadSP = &OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -16];
	OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -1 ] = 0x01000000;  // thumb bit indicates thumb state
	OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -3 ] = 0x14141414;  // R14 ( LR ) 
	OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -4 ] = 0x12121212;  // R12
	OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -5 ] = 0x03030303;  // R3
	OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -6 ] = 0x02020202;  // R2
	OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -7 ] = 0x01010101;  // R1
	OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -8 ] = 0x00000000;  // R0
	OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -9 ] = 0x11111111;  // R11
	OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -10] = 0x10101010;  // R10
	OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -11] = 0x09090909;  // R9
	OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -12] = 0x08080808;  // R8
	OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -13] = 0x07070707;  // R7
	OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -14] = 0x06060606;  // R6
	OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -15] = 0x05050505;  // R5
	OSThreadsStacks[threadIndx][OS_THREAD_STACK_SIZE -16] = 0x04040404;  // R4	
}

void OS_Add_Thread( void (*OSTask0Ptr)(void),
	                  void (*OSTask1Ptr)(void),
		                void (*OSTask2Ptr)(void)){

  uint32 status =0;
  status = StartCritical();
	OS_Thread_Stack_Init(0);
	OS_Thread_Stack_Init(1);
	OS_Thread_Stack_Init(2);
	/** Assign The thread to PC*/										
	OSThreadsStacks[0][OS_THREAD_STACK_SIZE -2] = (uint32)OSTask0Ptr;
	OSThreadsStacks[1][OS_THREAD_STACK_SIZE -2] = (uint32)OSTask1Ptr;
	OSThreadsStacks[2][OS_THREAD_STACK_SIZE -2] = (uint32)OSTask2Ptr;
	
	OSThreadsTCBs[0].OSNextThread = &OSThreadsTCBs[1];
	OSThreadsTCBs[1].OSNextThread = &OSThreadsTCBs[2];
	OSThreadsTCBs[2].OSNextThread = &OSThreadsTCBs[0];
	
	OSRunningThreadPtr = &OSThreadsTCBs[0];
  //OSThreadsTCBs[0].OSThreadSleep   = 0;
  OSThreadsTCBs[0].OSThreadBlocked = 0;	
	//OSThreadsTCBs[1].OSThreadSleep   = 0;
  OSThreadsTCBs[1].OSThreadBlocked = 0;	
	//OSThreadsTCBs[2].OSThreadSleep   = 0;
  OSThreadsTCBs[2].OSThreadBlocked = 0;											
	EndCritical(status);
											
}
										
void OS_Launch(uint32 OSTimeSlice){
  NVIC_ST_RELOAD_R = OSTimeSlice - 1; // reload value
  NVIC_ST_CTRL_R = 0x00000007; // enable, core clock and interrupt arm
  
//	Timer0A_Init(OS_Timer0A_Handler ,OS_TIME_1MS);//interrupt every 1 ms
	
	OS_Start();   

}


void OS_Semaphore_Init(int32 *SemaphoreCount , int32 SemaphoreVal){
  *SemaphoreCount = SemaphoreVal;
}

/*****************************************************************************/
/*****************************************************************************/
void OS_SpinLock_Semaphore_Wait(int32 *SemaphoreCount){
  OS_ASM_Disable_Interrupts();
	while(*SemaphoreCount == 0){
		OS_ASM_Enable_Interrupts();
		OS_ASM_Disable_Interrupts();
	}
	(*SemaphoreCount) = (*SemaphoreCount) -1;
	OS_ASM_Enable_Interrupts();
}

void OS_SpinLock_Semaphore_Signal(int32 *SemaphoreCount){
  OS_ASM_Disable_Interrupts();
	(*SemaphoreCount) = (*SemaphoreCount) +1;
	OS_ASM_Enable_Interrupts();
}
/*****************************************************************************/
void OS_Cooperative_Semaphore_Wait(int32 *SemaphoreCount){
  OS_ASM_Disable_Interrupts();
	while(*SemaphoreCount == 0){
		OS_ASM_Enable_Interrupts();
		OS_Suspend();
		OS_ASM_Disable_Interrupts();
	}
	(*SemaphoreCount) = (*SemaphoreCount) -1;
	OS_ASM_Enable_Interrupts();
}

void OS_Cooperative_Semaphore_Signal(int32 *SemaphoreCount){
  OS_ASM_Disable_Interrupts();
	(*SemaphoreCount) = (*SemaphoreCount) +1;
	OS_ASM_Enable_Interrupts();
}
/****************************************************************************************/
void OS_Blocking_Semaphore_Wait(int32 *SemaphoreCount){
  OS_ASM_Disable_Interrupts();
	(*SemaphoreCount) = (*SemaphoreCount) -1;
	if(*SemaphoreCount < 0){
		OSRunningThreadPtr->OSThreadBlocked = SemaphoreCount;
		OS_ASM_Enable_Interrupts();
		OS_Suspend();
	}
	OS_ASM_Enable_Interrupts();
}

void OS_Blocking_Semaphore_Signal(int32 *SemaphoreCount){
  OSThreadTCB_t *pt;
	OS_ASM_Disable_Interrupts();
	(*SemaphoreCount) = (*SemaphoreCount) +1;
	if((*SemaphoreCount) <=0){
		pt = OSRunningThreadPtr->OSNextThread;
	 while(pt->OSThreadBlocked != SemaphoreCount){
			pt = pt->OSNextThread;
		}
		pt->OSThreadBlocked = 0;
	}
	OS_ASM_Enable_Interrupts();
}
/***************************************************************************************/
void OS_Send_mail(uint32 data){
   mail = data ;
	OS_Blocking_Semaphore_Signal(&mailSendSemaphore);
	OS_Blocking_Semaphore_Wait(&mailAckSemaphore);
}



uint32 OS_Receive_Mail(void){
  uint32 data;
  OS_Blocking_Semaphore_Wait(&mailSendSemaphore);
	data = mail ;
	OS_Blocking_Semaphore_Signal(&mailAckSemaphore);
  return data;
}
/********************************************************************/
void OS_Suspend(void){
  NVIC_ST_CURRENT_R = 0;
	NVIC_INT_CTRL_R   = 0x04000000;
}
/*
void OS_Sleep(int32 SleepTimeMilliSec){
  OSRunningThreadPtr->OSThreadSleep = SleepTimeMilliSec;
	OS_Suspend();
}*/
/*******************************************************************/
void OS_Schedular(void){
  OSRunningThreadPtr = OSRunningThreadPtr->OSNextThread;
	/*while((OSRunningThreadPtr->OSThreadBlocked)/*||(OSRunningThreadPtr->OSThreadSleep)){
		OSRunningThreadPtr = OSRunningThreadPtr ->OSNextThread;
	}*/
}
/**************************************************************************************/
/****************  FIFO FUNCTIONS *****************************************************/
/**************************************************************************************/
/*void OS_FIFO_Init(void){
	OSFIFOGetPtr = OSFIFOPutPtr = &OSFIFO[0];
	OS_Semaphore_Init(&OSFIFOCurrentSize , 0            );
	OS_Semaphore_Init(&OSFIFORoomLeft    , OS_FIFO_SIZE );
	OS_Semaphore_Init(&OSFIFOMutex       , 1            );
}

void OS_FIFO_Put(uint32 data){
	OS_Blocking_Semaphore_Wait(&OSFIFORoomLeft);
	OS_Blocking_Semaphore_Wait(&OSFIFOMutex);
	*OSFIFOPutPtr = data;
	OSFIFOPutPtr++;
	if(OSFIFOPutPtr == &OSFIFO[OS_FIFO_SIZE]){
	   OSFIFOPutPtr = &OSFIFO[0];
	}
	OS_Blocking_Semaphore_Signal(&OSFIFOMutex);
	OS_Blocking_Semaphore_Signal(&OSFIFOCurrentSize);
}

uint32 OS_FIFO_Get(void){
	uint32 data;
	OS_Blocking_Semaphore_Wait(&OSFIFOCurrentSize);
	OS_Blocking_Semaphore_Wait(&OSFIFOMutex);
	data = *OSFIFOGetPtr;
	OSFIFOGetPtr++;
	if(OSFIFOGetPtr == &OSFIFO[OS_FIFO_SIZE]){
	 OSFIFOGetPtr = &OSFIFO[0];
	}
	OS_Blocking_Semaphore_Signal(&OSFIFOMutex);
	OS_Blocking_Semaphore_Signal(&OSFIFORoomLeft);
	return data;
}*/
