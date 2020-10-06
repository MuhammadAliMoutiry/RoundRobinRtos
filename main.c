

#include <stdint.h>
#include "os.h"
#include "GPIO.h"
#define TIMESLICE               OS_TIME_2MS    // thread switch time in system time units

uint32_t Count1;   // number of times thread1 loops
uint32_t Count2;   // number of times thread2 loops
uint32_t Count3;   // number of times thread3 loops
#define GPIO_PORTD1             (*((volatile uint32_t *)0x40007008))
#define GPIO_PORTD2             (*((volatile uint32_t *)0x40007010))
#define GPIO_PORTD3             (*((volatile uint32_t *)0x40007020))
#define GPIO_PORTD_DIR_R        (*((volatile uint32_t *)0x40007400))
#define GPIO_PORTD_AFSEL_R      (*((volatile uint32_t *)0x40007420))
#define GPIO_PORTD_DEN_R        (*((volatile uint32_t *)0x4000751C))
#define GPIO_PORTD_AMSEL_R      (*((volatile uint32_t *)0x40007528))
#define GPIO_PORTD_PCTL_R       (*((volatile uint32_t *)0x4000752C))
#define SYSCTL_RCGCGPIO_R       (*((volatile uint32_t *)0x400FE608))
#define SYSCTL_RCGCGPIO_R3      0x00000008  // GPIO Port D Run Mode Clock
                                            // Gating Control
#define SYSCTL_PRGPIO_R         (*((volatile uint32_t *)0x400FEA08))
#define SYSCTL_PRGPIO_R3        0x00000008  // GPIO Port D Peripheral Ready

void Task1(void){
  Count1 = 0;
  for(;;){
    Count1++;
    GPIO_PORTD1 ^= 0x02;      // toggle PD1
		delay_100ms(5);
  }
}
void Task2(void){
  Count2 = 0;
  for(;;){
    Count2++;
    GPIO_PORTD2 ^= 0x04;      // toggle PD2
		delay_100ms(5);
  }
}
void Task3(void){
  Count3 = 0;
  for(;;){
    Count3++;
    GPIO_PORTD3 ^= 0x08;      // toggle PD3
		delay_100ms(5);

  }
}
int main(void){
  OS_Init();           // initialize, disable interrupts, 50 MHz
  SYSCTL_RCGCGPIO_R |= 0x08;            // activate clock for Port D
  while((SYSCTL_PRGPIO_R&0x08) == 0){}; // allow time for clock to stabilize
  GPIO_PORTD_DIR_R |= 0x0E;             // make PD3-1 out
  GPIO_PORTD_AFSEL_R &= ~0x0E;          // disable alt funct on PD3-1
  GPIO_PORTD_DEN_R |= 0x0E;             // enable digital I/O on PD3-1
                                        // configure PD3-1 as GPIO
  GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFF000F)+0x00000000;
  GPIO_PORTD_AMSEL_R &= ~0x0E;          // disable analog functionality on PD3-1
  OS_Add_Thread(&Task1, &Task2, &Task3);
  OS_Launch(TIMESLICE); // doesn't return, interrupts enabled in here
  return 0;             // this never executes
}
