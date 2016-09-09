#include"stdint.h"
#include<msp430.h>
#include"Button.h"
#include"Timer.h"
#include"Display.h"
volatile s_button_flags button;
void Port_Init(void);// to initialize all I/O ports

void Port_Init(void)
{
	//Button pin configuration
	P1DIR &= ~ALL_BUTTONS;//Input direction
	P1REN |= ALL_BUTTONS;//Pull up/down resistor enabled
	P1OUT |= ALL_BUTTONS;//select pull up resistor
	P1IES |= ALL_BUTTONS;//high to low transition interrupt
	P1IFG = 0;//clear interrupt flag
	P1IE |= ALL_BUTTONS;//Interrupt enabled

	P3SEL &= ~(BCD_ALL);
	P3DIR |= (BCD_ALL);//output direction
	P3OUT |= (BCD_ALL);
	button.all_flags = 0;
}

//make interrupt service routine short, just modify the display status and let the main program
//to do the rest.
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	uint8_t int_enable = 0;
	button.all_flags = 0;
	if((P1IFG & ALL_BUTTONS) != 0){
        // Disable PORT1 IRQ
	    __bic_SR_register(GIE);       // disable global interrupt
	    int_enable = P1IE;
        P1IE = 0x00;
        __bis_SR_register(GIE);       // enable global interrupt
//        TimerA0_CCR2_Delay(5);	//delay for 5ms
	}

    if(IRQ_TRIGGERED(P1IFG,BUTTON_MINUTE)){//Button minute pressed
    	if(BUTTON_MINUTE_IS_PRESSED){
    		button.flag.minute = 1;
    	}
    }
    if(IRQ_TRIGGERED(P1IFG,BUTTON_SECOND)){//Button minute pressed
    	if(BUTTON_SECOND_IS_PRESSED){
    		button.flag.second = 1;
    	}
    }
    if(IRQ_TRIGGERED(P1IFG,BUTTON_TIMER_ONOFF)){//Button minute pressed
    	if(BUTTON_TIMER_ONOFF_IS_PRESSED){
    		button.flag.timer = 1;
    	}
    }
    if(IRQ_TRIGGERED(P1IFG,BUTTON_RTC)){//Button minute pressed
    	if(BUTTON_RTC_IS_PRESSED){
    		button.flag.rtc = 1;
    	}
    }
    if(IRQ_TRIGGERED(P1IFG,BUTTON_HUMIDITY)){//Button minute pressed
    	if(BUTTON_HUMIDITY_IS_PRESSED){
    		button.flag.hum = 1;
    	}
    }
    // Reenable PORT2 IRQ
    __bic_SR_register(GIE);       // disable global interrupt
    P1IFG = 0x00;
    P1IE = int_enable;
    __bis_SR_register(GIE);       // enable global interrupt
}
