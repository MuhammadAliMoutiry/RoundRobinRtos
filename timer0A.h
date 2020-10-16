#ifndef TIMER0A_H_ // do not include more than once
#define TIMER0A_H_
#include <stdint.h>
// ***************** Timer0A_Init ****************
// Activate Timer0A interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer0A_Init(void(*task)(void), uint32_t period);

#endif // __TIMER0AINTS_H__