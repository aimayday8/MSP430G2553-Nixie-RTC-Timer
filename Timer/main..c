#include <msp430.h>
#include "stdint.h"
#include"Sensor.h"
#include"Timer.h"
#include"Button.h"
#include"I2C.h"
/*
 * main.c
 */

int main(void) {
	uint8_t rtc_second_temp = 0;
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    BCSCTL1 = CALBC1_1MHZ; // Set DCO to 1Mhz
    DCOCTL = CALDCO_1MHZ;
    Port_Init();// to initialize all I/O ports
    P2SEL &= ~(BIT2);// + BIT1 + BIT5);
    P2DIR |= (BIT2);// + BIT1 + BIT5);
    P2OUT &= ~(BIT2);// + BIT1 + BIT5);
    I2C_Init(SLAVE_ADDR_RTC);
    Sensor_Init();//initialize all the i2c sensors
    TimerA0_Config();
    TimerA1_Config();
    while(1){
    	if(button.flag.minute){
    		button.flag.minute = 0;
    		if(display_status == RTC_SETTING){
    			Wdt_Stop();
    			if(rtc.hour < 23)
    				rtc.hour ++;
    			else
    				rtc.hour = 0;
    			Wdt_Config(15);//restart wdt timer after button pressed
    		}
            else if(display_status == TIMER_STATIC){
            	Wdt_Stop();
                display_status = TIMER_SETTING;
                if(timer.minute < 99)
                	timer.minute++;
                else timer.minute = 0;
                Wdt_Config(15);
            }
            else if(display_status == TIMER_SETTING){
            	Wdt_Stop();
                if(timer.minute < 99)
                     timer.minute++;
                else timer.minute = 0;
                Wdt_Config(15);
            }
    	}
    	if(button.flag.minute_long){//long button press
    		Wdt_Stop();
    		if(timerflag.flag.minute_button_hold){
    			timerflag.flag.minute_button_hold = 0;
    			if((display_status == TIMER_STATIC) || (display_status == TIMER_SETTING)){
    				if(timer.minute < 95)
    					timer.minute += 5;
    				else timer.minute = 0;
    			}
    		}
    	}
    	if(button.flag.second){
    		button.flag.second = 0;
    		if(display_status == RTC_SETTING){
    			Wdt_Stop();
    	        if(rtc.minute < 59)
    	        	rtc.minute++;
    	        else rtc.minute = 0;
    	        Wdt_Config(15);
    		}
    	    else if(display_status == TIMER_STATIC){
    	    	Wdt_Stop();
    	        display_status = TIMER_SETTING;
    	        if(timer.second < 59)
    	        	timer.second++;
    	        else timer.second = 0;
    	        Wdt_Config(15);
    	    }
    	    else if(display_status == TIMER_SETTING){
    	    	Wdt_Stop();
    	        if(timer.second < 59)
    	        	timer.second++;
    	        else timer.second = 0;
    	        Wdt_Config(15);
    	    }
    	}
    	if(button.flag.second_long){
    		Wdt_Stop();
    		if(timerflag.flag.second_button_hold){
    			timerflag.flag.second_button_hold = 0;
    			if(display_status == RTC_SETTING){
    				if(rtc.minute < 55)
    					rtc.minute += 5;
    				else rtc.minute = 0;
    			}
    			else if((display_status == TIMER_STATIC) || (display_status == TIMER_SETTING)){
    				if(timer.second < 55)
    					timer.second += 5;
    				else timer.second = 0;
    			}
    		}
    	}
    	if(button.flag.timer){
    		button.flag.timer = 0;
            if(display_status == RTC){//was in RTC mode, change to timer mode
            	display_status = TIMER_STATIC;//change to static 0000 mode for setting
            	timer.second = 0;
            	timer.minute = 0;
                Wdt_Config(15);//start 15 seconds wdt timer for no action period detection
            }
            else if(display_status == TIMER_STATIC){//was in timer static mode,turn off timer and go back to rtc
                Wdt_Stop();
            	display_status = RTC;
            }
            else if(display_status == TIMER_SETTING){
                Wdt_Stop();
                display_status = TIMER_COUNT_DOWN;//was in timer setting mode, finished setting and start timer
                uint16_t total_seconds = (((timer.minute << 6) - (timer.minute << 2)) + timer.second);//minute * (64 - 4) + seconds
                //Timer_Sec_To_Cnt(total_seconds, &countdown_cnt , &countdown_base);
                countdown_cnt = Timer_Sec_To_Cnt(total_seconds);
                TimerA0_CCR0_CountDown();
            }
            else if(display_status == TIMER_COUNT_DOWN){
                display_status = TIMER_STATIC;//was in timer count down, stop it and go to all 0 static mode.
                TA0CCTL0 &= ~CCIE;
                TA0CCTL0 &= ~CCIFG;// Reset IRQ flag
            	timer.second = 0;
            	timer.minute = 0;
                Wdt_Config(15);//start 15 seconds wdt timer for no action period detection
            }
            else if(display_status == TIMER_FLASHING){
            	Wdt_Stop();
                display_status = TIMER_STATIC;//was in timer finished and blinking buzzer mode, stop it and go back to static
                //add in function to stop the buzzer.
            	timer.second = 0;
            	timer.minute = 0;
                Wdt_Config(15);//start 15 seconds wdt timer for no action period detection
            }
    	}
    	if(button.flag.rtc){
    		button.flag.rtc = 0;
            if(display_status == RTC){//was in rtc mode, now change to rtc setting, blink
                display_status = RTC_SETTING;
                Wdt_Config(15);///start 15 seconds wdt timer for no action period detection
            }
            else if(display_status == RTC_SETTING){
            	Wdt_Stop();
            	display_status = RTC;//was in setting mode, go back to rtc mode
            	Write_RTC();
            }
    	}
/*    	if(button.flag.hum){
    		button.flag.hum = 0;
            if(display_status == RTC){//only works when rtc is displayed
            	display_status = HUM_TEMP;
            	Read_Humtemp();
                Wdt_Config(5);//to start 5 secs count timer interrupt to go back to rtc mode.
            }
    	}*/
    	if(timerflag.flag.alarm){
    		timerflag.flag.alarm = 0;
    		//add in the alarm funtion
    	}
    	if(timerflag.flag.rtc_second_read){//reading rtc data every 1 second.
    		timerflag.flag.rtc_second_read = 0;
    		I2C_Read(SLAVE_ADDR_RTC, 0x00, 1, &rtc_second_temp);
    		if((Bcd_to_Dec(rtc_second_temp)) == 0){
    			Read_RTC();
    		}
    	}
    }
    return 0;
}



/*
 * I2C_Init(SLAVE_ADDR_LIGHT);
I2C_Write(SLAVE_ADDR_LIGHT, 0x80, 0x03);
I2C_Write(SLAVE_ADDR_LIGHT, 0x81, 0x12);
I2C_Read(SLAVE_ADDR_LIGHT, 0x8C, 1, &low);
I2C_Read(SLAVE_ADDR_LIGHT, 0x8D, 1, &high);
cha0 = 256 * high + low;
I2C_Read(SLAVE_ADDR_LIGHT, 0x8E, 1, &low);
I2C_Read(SLAVE_ADDR_LIGHT, 0x8F, 1, &high);
cha1 = 256 * high + low;
Calculate_lux(cha0, cha1, 1, 2);*/


/*    	switch(display_status)
    	{
    		case RTC:
    			break;
    		case RTC_SETTING:
    			break;
    		case HUM_TEMP:
    			break;
    		case TIMER_STATIC:
    			break;
    		case TIMER_SETTING:
    			break;
    		case TIMER_COUNT_DOWN:
    			break;
    		case TIMER_FLASHING:
    			break;
    		default:
    			break;
    	}*/
