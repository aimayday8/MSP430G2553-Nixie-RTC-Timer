//Pin Button Driver
#ifndef BUTTON_H_
#define BUTTON_H_

#include"stdint.h"
#include<msp430.h>

/*******************************************PORTS_H_*********************************************/
// Pin macro for 5 buttons
#define BUTTONS_IN              				(P1IN)

#define BUTTON_MINUTE							(BIT0)		//P1.0
#define BUTTON_SECOND							(BIT1) 		//P1.1
#define BUTTON_TIMER_ONOFF						(BIT2) 		//P1.2
#define BUTTON_RTC								(BIT3)		//P1.3
#define BUTTON_HUMIDITY 						(BIT4) 			//P1.4
#define ALL_BUTTONS 							(BUTTON_MINUTE + BUTTON_SECOND + BUTTON_TIMER_ONOFF + BUTTON_RTC + BUTTON_HUMIDITY)

// Macro for button IRQ
#define IRQ_TRIGGERED(flags, bit)               ((flags & bit) == bit)
#define BUTTON_MINUTE_IS_PRESSED          		((BUTTONS_IN | ~BUTTON_MINUTE) == ~BUTTON_MINUTE)
#define BUTTON_SECOND_IS_PRESSED           		((BUTTONS_IN | ~BUTTON_SECOND) == ~BUTTON_SECOND)
#define BUTTON_TIMER_ONOFF_IS_PRESSED           ((BUTTONS_IN | ~BUTTON_TIMER_ONOFF) == ~BUTTON_TIMER_ONOFF)
#define BUTTON_RTC_IS_PRESSED          			((BUTTONS_IN | ~BUTTON_RTC) == ~BUTTON_RTC)
#define BUTTON_HUMIDITY_IS_PRESSED     			((BUTTONS_IN | ~BUTTON_HUMIDITY) == ~BUTTON_HUMIDITY)


// Set of button flags
typedef union
{
    struct
    {
        // Manual button events
        uint16_t minute : 1;           // Short minute button press
        uint16_t second : 1;            // Short second button press
        uint16_t timer : 1;             // Short timer on/off button press
        uint16_t rtc : 1;      // Short rtc button press
        uint16_t hum : 1;           // Short humidity button press
        uint16_t minute_long : 1;      // Long minute button press
        uint16_t second_long : 1;       // Long second button press
        uint16_t minute_not_long : 1;  // Between short and long STAR button press
        uint16_t second_not_long : 1;   // Between short and long NUM button press
    } flag;
    uint16_t all_flags;              // Shortcut to all display flags (for reset)
} s_button_flags;
extern volatile s_button_flags button;

extern void Port_Init(void);// to initialize all I/O ports

#endif                          /*BUTTON_H_ */
