
#include "NVIC.h"
#include "PLL.h"
#include "os.h"

#define OS_NUM_THREADS       3
#define OS_THREAD_STACK_SIZE 100


typedef struct OSThreadTCB{
   uint32 *OSThreadSP;
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
uint32 mail;
uint32 mailSendSemaphore =0 ;
uint32 mailAckSemaphore  =0 ;
/************************************************************************************************/
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
	EndCritical(status);
											
}
										
void OS_Launch(uint32 OSTimeSlice){
  NVIC_ST_RELOAD_R = OSTimeSlice - 1; // reload value
  NVIC_ST_CTRL_R = 0x00000007; // enable, core clock and interrupt arm
  OS_Start();   

}


void OS_Semaphore_Init(uint32 *SemaphoreCount , uint32 SemaphoreVal){
  *SemaphoreCount = SemaphoreVal;
}

void OS_Semaphore_Wait(uint32 *SemaphoreCount){
  OS_ASM_Disable_Interrupts();
	while(*SemaphoreCount == 0){
		OS_ASM_Enable_Interrupts();
		OS_ASM_Disable_Interrupts();
	}
	(*SemaphoreCount) = (*SemaphoreCount) -1;
	OS_ASM_Enable_Interrupts();
}

void OS_Semaphore_Signal(uint32 *SemaphoreCount){
  OS_ASM_Disable_Interrupts();
	(*SemaphoreCount) = (*SemaphoreCount) +1;
	OS_ASM_Enable_Interrupts();
}

void OS_Send_mail(uint32 data){
   mail = data ;
	OS_Semaphore_Signal(&mailSendSemaphore);
	OS_Semaphore_Wait(&mailAckSemaphore);
}

uint32 OS_Receive_Mail(void){
 uint32 data;
 OS_Semaphore_Wait(&mailSendSemaphore);
	data = mail ;
	OS_Semaphore_Signal(&mailAckSemaphore);
  return data;
}

