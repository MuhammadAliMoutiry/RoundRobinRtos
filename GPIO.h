
#ifndef GPIO_H_
#define GPIO_H_

//define easy to read names for registers
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))

#define LED_OFF          0x00
#define LED_RED          0x02
#define LED_BLUE         0x04
#define LED_PINK         0x06
#define LED_GREEN        0x08
#define LED_YELLOW       0x0A
#define LED_SKY_BLUE     0x0C
#define LED_WHITE        0x0E

//   Global Variables

//   Function Prototypes
void PortF_Init(void);		
void Delay(void);
void led(unsigned long val);
void delay_100ms(int i);
// 3. Subroutines Section

#endif
