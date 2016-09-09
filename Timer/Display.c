#include<msp430.h>
#include"stdint.h"
#include"Display.h"

void display(uint8_t second, uint8_t minute, uint8_t high_bit);
void display_off(void);

void display(uint8_t second, uint8_t minute, uint8_t high_bit)
{
	uint8_t lower = 0, higher = 0;
	if(high_bit){
			lower = second / 10;
			higher = minute / 10;
	}
	else{
			lower = second % 10;
			higher = minute % 10;
	}
	switch(lower)
	{
	case 0:
		//BCD 1,0000
		P3OUT &= ~(BCD1_A + BCD1_B + BCD1_C + BCD1_D);
		break;
	case 1:
		//BCD 1,1000
		P3OUT |= BCD1_A;
		P3OUT &= ~(BCD1_B + BCD1_C + BCD1_D);
		break;
	case 2:
		//BCD 1,0100
		P3OUT |= BCD1_B;
		P3OUT &= ~(BCD1_A + BCD1_C + BCD1_D);
		break;
	case 3:
		//BCD 1,1100
		P3OUT |= (BCD1_A + BCD1_B);
		P3OUT &= ~(BCD1_C + BCD1_D);
		break;
	case 4:
		//BCD 1,0010
		P3OUT |= BCD1_C;
		P3OUT &= ~(BCD1_A + BCD1_B + BCD1_D);
		break;
	case 5:
		//BCD 1,1010
		P3OUT |= (BCD1_A + BCD1_C);
		P3OUT &= ~(BCD1_B + BCD1_D);
		break;
	case 6:
		//BCD 1,0110
		P3OUT |= (BCD1_B + BCD1_C);
		P3OUT &= ~(BCD1_A + BCD1_D);
		break;
	case 7:
		//BCD 1,1110
		P3OUT |= (BCD1_A + BCD1_B + BCD1_C);
		P3OUT &= ~(BCD1_D);
		break;
	case 8:
		//BCD 1,0001
		P3OUT |= BCD1_D;
		P3OUT &= ~(BCD1_A + BCD1_B + BCD1_C);
		break;
	case 9:
		//BCD 1,1001
		P3OUT |= (BCD1_A + BCD1_D);
		P3OUT &= ~(BCD1_B + BCD1_C);
		break;
	default:
		break;
	}
	switch(higher)
	{
	case 0:
		//BCD 2,0000
		P3OUT &= ~(BCD2_A + BCD2_B + BCD2_C + BCD2_D);
		break;
	case 1:
		//BCD 2,1000
		P3OUT |= BCD2_A;
		P3OUT &= ~(BCD2_B + BCD2_C + BCD2_D);
		break;
	case 2:
		//BCD 2,0100
		P3OUT |= BCD2_B;
		P3OUT &= ~(BCD2_A + BCD2_C + BCD2_D);
		break;
	case 3:
		//BCD 2,1100
		P3OUT |= (BCD2_A + BCD2_B);
		P3OUT &= ~(BCD2_C + BCD2_D);
		break;
	case 4:
		//BCD 2,0010
		P3OUT |= BCD2_C;
		P3OUT &= ~(BCD2_A + BCD2_B + BCD2_D);
		break;
	case 5:
		//BCD 2,1010
		P3OUT |= (BCD2_A + BCD2_C);
		P3OUT &= ~(BCD2_B + BCD2_D);
		break;
	case 6:
		//BCD 2,0110
		P3OUT |= (BCD2_B + BCD2_C);
		P3OUT &= ~(BCD2_A + BCD2_D);
		break;
	case 7:
		//BCD 2,1110
		P3OUT |= (BCD2_A + BCD2_B + BCD2_C);
		P3OUT &= ~(BCD2_D);
		break;
	case 8:
		//BCD 2,0001
		P3OUT |= BCD2_D;
		P3OUT &= ~(BCD2_A + BCD2_B + BCD2_C);
		break;
	case 9:
		//BCD 2,1001
		P3OUT |= (BCD2_A + BCD2_D);
		P3OUT &= ~(BCD2_B + BCD2_C);
		break;
	default:
		break;
	}
}

void display_off(void)
{
	P3OUT |= (BCD_ALL);
}
