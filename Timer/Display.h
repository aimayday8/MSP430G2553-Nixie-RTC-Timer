#ifndef DISPLAY_H_
#define DISPLAY_H_
#include"stdint.h"
//macro for two BCD driver
#define BCD1_A 									(BIT0)
#define BCD1_B 									(BIT1)
#define BCD1_C 									(BIT2)
#define BCD1_D 									(BIT3)
#define BCD2_A 									(BIT4)
#define BCD2_B 									(BIT5)
#define BCD2_C 									(BIT6)
#define BCD2_D 									(BIT7)
#define BCD_ALL									(BCD1_A + BCD1_B + BCD1_C + BCD1_D + BCD2_A + BCD2_B + BCD2_C + BCD2_D)

extern void display(uint8_t second, uint8_t minute, uint8_t high_bit);
extern void display_off(void);
#endif			/*DISPLAY_H_*/
