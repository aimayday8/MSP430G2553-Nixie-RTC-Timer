#include<msp430.h>
#include"Timer.h"
#include"stdint.h"
#include"Button.h"
#include"Display.h"


void Wdt_Config(uint8_t interval);
void Wdt_Stop();
void TimerA0_Config();
void TimerA1_Config();
void TimerA0_CCR2_Delay(uint8_t ms);//delay in milliseconds by using timer ccr2.
void TimerA0_CCR0_CountDown();


volatile uint8_t wdt_interval = 0;
volatile uint8_t display_status = RTC;
volatile s_timer_flags timerflag;
volatile sTimerTimeout timeout  = {0,0,0,0,0};
volatile uint16_t countdown_cnt = 0;

void Wdt_Config(uint8_t interval)//to configure the WDT for every 1 second.
{
	wdt_interval = interval;
    WDTCTL = WDT_ADLY_1000;                    // (WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL) WDT 1000ms, ACLK, interval timer
    IE1 |= WDTIE;                             // Enable WDT interrupt
    __bis_SR_register(GIE);
}

void Wdt_Stop()
{
	wdt_interval = 0;
    IE1 &= ~WDTIE;              // Disable WDT interrupt
	WDTCTL = WDTPW | WDTHOLD;   // Stop 15seconds watchdog timer
}

void TimerA0_Config()
{
	TA0CCR1 = 50000;
	TA0CCR2 = 0;
	TA0CTL = TASSEL_2 + MC_2 + ID_2 + TAIE;                  // SMCLK/4, continuous mode,overflow enabled,250000 count for 1 second.
	TA0CCTL1 = CCIE;
	TA0CCTL2 = CCIE;
	__bis_SR_register(GIE);       // Enter LPM0 w/ interrupt
}

void TimerA1_Config()
{
	P2SEL |= (BIT1 + BIT5);            // P2.0, P2.2, P2.4 option select
	P2DIR |= (BIT1 + BIT5);
	TA1CCR0 = 1350;//100HZ
	TA1CCR1 = 1000;//on time
	TA1CCR2 = 350;//off time
	TA1CCTL1 = OUTMOD_6 | CCIE;//Set/Reset
	TA1CCTL2 = OUTMOD_2 | CCIE;//Reset/Set
	TA1CTL = TASSEL_2 + MC_3 + ID_2 + TAIE;                  // SMCLK/4, up mode,overflow enabled,250000 count for 1 second.
	TA1CCTL0 = CCIE;

	__bis_SR_register(GIE);       // Enter LPM0 w/ interrupt
}
void TimerA0_CCR0_CountDown()
{
    // Exit immediately if Timer0 not running - otherwise we'll get stuck here
    if ((TA0CTL & (BIT4 | BIT5)) == 0)
        return;
    // Disable timer interrupt
    TA0CCTL0 &= ~CCIE;
    TA0CCR0 += 62500;
    // Reset IRQ flag
    TA0CCTL0 &= ~CCIFG;
    // Enable timer interrupt
    TA0CCTL0 |= CCIE;
    __bis_SR_register(GIE);       // Enter LPM0 w/ interrupt
}
void TimerA0_CCR2_Delay(uint8_t ms)
{
    // Exit immediately if Timer0 not running - otherwise we'll get stuck here
    if ((TA0CTL & (BIT4 | BIT5)) == 0)
        return;

    // Disable timer interrupt
    TA0CCTL2 &= ~CCIE;

    // Clear delay_over flag
    timerflag.flag.delay_over = 0;

    // Add delay to current timer value
    // To make sure this value is correctly read
	
    // Update CCR
    TA0CCR2 += ((ms << 8) - (ms << 2) - (ms << 1));

    // Reset IRQ flag
    TA0CCTL2 &= ~CCIFG;

    // Enable timer interrupt
    TA0CCTL2 |= CCIE;
    __bis_SR_register(GIE);       // Enter LPM0 w/ interrupt
    // Wait for timer IRQ
    while (1)
    {
        if (timerflag.flag.delay_over)
            break;
    }
    __bis_SR_register(GIE);       // Enter LPM0 w/ interrupt
}

// Timer A0 CCR0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
{
	switch(countdown_cnt)
	{
	case 0:
		TA0CCTL0 &= ~CCIE;
	    TA0CCTL0 &= ~CCIFG;// Reset IRQ flag
	    timer.second = 0;
	    timer.minute = 0;
	    timerflag.flag.alarm = 1;
	    display_status = TIMER_FLASHING;
	    Wdt_Config(30);//start 30 seconds wdt timer for no action period detection
		break;
	default:
		TA0CCR0 += 62500;
		countdown_cnt--;
		if((countdown_cnt % 4) == 0){
			if(timer.second == 0){
				timer.second = 59;
				timer.minute--;
			}
			else
				timer.second--;
		}
		break;
	}
}

// Timer A0 CCR1, CCR2 and overflow interrupt service routine
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1(void)
{
  switch( TA0IV )
  {
  case  2:// CCR1 for 1 seconds display update and button long press
	// Disable IE
	TA0CCTL1 &= ~CCIE;
	// Reset IRQ flag
	TA0CCTL1 &= ~CCIFG;
	// Add 1 sec to TACCR0 register (IRQ will be asserted at 0x7FFF and 0xFFFF = 1 sec intervals)
	TA0CCR1 += 50000;
	// Enable IE
	TA0CCTL1 |= CCIE;
	//minute button long press
	if(BUTTON_MINUTE_IS_PRESSED){
		timeout.minute++;
		if(timeout.minute == TIMER_2_SECONDS){
			button.flag.minute_long = 1;//long button press for minute button
			timeout.minute = 0;
		}
	}
	else{
		timeout.minute = 0;
		button.flag.minute_long = 0;//button released, clear long press flag
		Wdt_Config(15);
	}
	//second button long press
	if(BUTTON_SECOND_IS_PRESSED){
		timeout.second++;
		if(timeout.second == TIMER_2_SECONDS){
			button.flag.second_long = 1;//long button press for minute button
			timeout.second = 0;
		}
	}
	else{
		timeout.second = 0;
		button.flag.second_long = 0;//button released, clear long press flag
		Wdt_Config(15);
	}
	//this is after the long press of minute button,if it's still holded, it is effective every 1 second until it's released
	if(button.flag.minute_long){
		timeout.minute_cnt++;
		if(timeout.minute_cnt == TIMER_1_SECONDS){
			timeout.minute_cnt = 0;
			timerflag.flag.minute_button_hold = 1;
		}
	}
	else timeout.minute_cnt = 0;
	//this is after the long press of second button,if it's still holded, it is effective every 1 second until it's released
	if(button.flag.second_long){
		timeout.second_cnt++;
		if(timeout.second_cnt == TIMER_1_SECONDS){
			timeout.second_cnt = 0;
			timerflag.flag.second_button_hold = 1;
		}
	}
	else timeout.second_cnt = 0;
	//this is where the display will flash, basicly on/off every 1 seconds
	if((display_status == RTC_SETTING) || (display_status == TIMER_FLASHING)){
		timeout.display++;
		if(timeout.display == TIMER_1_SECONDS){
			timeout.display = 0;
			timerflag.flag.display_flash ^= 1;
		}
	}
	else timeout.display = 0;
  	break;
  case  4://CCR2 for delay in milliseconds
	  TA0CCTL2 &= ~CCIE;
      TA0CCTL2 &= ~CCIFG;// Reset IRQ flag
      timerflag.flag.delay_over = 1;
	  break;
  case 10: break;                           // overflow for timer count down
 }
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0(void)
{
	timerflag.flag.ccr1 = 1;
}

#pragma vector=TIMER1_A1_VECTOR
__interrupt void Timer1_A1(void)
{
	switch( TA1IV )
	{
	case 2://CCR1
		//update display higher bit
		if(timerflag.flag.ccr1){
			timerflag.flag.ccr1 = 0;
//			display(timer.second, timer.minute,1);
			if((display_status == RTC) || (display_status == RTC_SETTING)){
				if(display_status == RTC_SETTING){
					if(timerflag.flag.display_flash)//
						display_off();
					else
						display(rtc.minute, rtc.hour,1);
				}
				else
					display(rtc.minute, rtc.hour,1);
			}
			else if(display_status == HUM_TEMP){
				display(hum_temp.temperature, hum_temp.humidity,1);
			}
			else{
				if(display_status == TIMER_FLASHING){
					if(timerflag.flag.display_flash)
						display_off();
					else
						display(timer.second, timer.minute,1);
				}
				else
					display(timer.second, timer.minute,1);
	    	}
		}
		break;
	case 4://CCR2 used to check rtc second register
		//update display lower bit
		if(timerflag.flag.ccr2){
			timerflag.flag.ccr2 = 0;
//			display(timer.second, timer.minute,0);
			if((display_status == RTC) | (display_status == RTC_SETTING)){
				if(display_status == RTC_SETTING){
					if(timerflag.flag.display_flash)//
						display_off();
					else
						display(rtc.minute, rtc.hour,0);
				}
				else
					display(rtc.minute, rtc.hour,0);
			}
			else if(display_status == HUM_TEMP){
				display(hum_temp.temperature, hum_temp.humidity,0);
			}
			else{
				if(display_status == TIMER_FLASHING){
					if(timerflag.flag.display_flash)
						display_off();
					else
						display(timer.second, timer.minute,0);
				}
				else
					display(timer.second, timer.minute,0);
	    	}
		}
		break;
	case 10://Overflow
		timerflag.flag.rtc_second_read = 1;
		timerflag.flag.ccr2 = 1;
		break;
	default :
		break;
	}
}

#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
	if(wdt_interval > 0)
		wdt_interval--;
	if(wdt_interval == 0){
		display_status = RTC;//timer up, no action and go back to rtc mode.
		WDTCTL = WDTPW | WDTHOLD;   // Stop 15seconds watchdog timer
	    IE1 &= ~WDTIE;              // Disable WDT interrupt
	}
	P2OUT ^= BIT2;
}
