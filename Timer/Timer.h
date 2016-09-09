#ifndef TIMER_H_
#define TIMER_H_

#include "stdint.h"
#include "Sensor.h"

#define RTC						0xA0
#define RTC_SETTING 			0xA1
#define HUM_TEMP 				0xA2
#define TIMER_STATIC			0xA3
#define TIMER_SETTING 			0xA4
#define TIMER_COUNT_DOWN 		0xA5
#define TIMER_FLASHING 			0xA6

#define TIMER_1_SECONDS			3
#define TIMER_2_SECONDS			10

#define Timer_Sec_To_Cnt(sec)		(sec << 2)
// Set of timer flags
typedef union
{
    struct
    {
        uint8_t delay_over : 1;               // 1 = Timer delay over
        uint8_t minute_button_hold : 1;
        uint8_t second_button_hold : 1;
        uint8_t display_flash : 1;
        uint8_t alarm : 1;
        uint8_t rtc_second_read : 1;
        uint8_t ccr1 : 1;
        uint8_t ccr2 : 1;
    } flag;
    uint8_t all_flags;                        // Shortcut to all system flags (for reset)
} s_timer_flags;
extern volatile s_timer_flags timerflag;

typedef struct struct_timer_timeout
{
	uint8_t minute;
	uint8_t second;
	uint8_t minute_cnt;
	uint8_t second_cnt;
	uint8_t display;
}sTimerTimeout;
extern volatile sTimerTimeout timeout;

extern void Wdt_Config(uint8_t interval);
extern void Wdt_Stop();
extern void TimerA0_Config();
extern void TimerA1_Config();
extern void TimerA0_CCR2_Delay(uint8_t ms);//delay in milliseconds by using timer ccr2.
extern void TimerA0_CCR0_CountDown();

extern volatile uint16_t countdown_cnt;
extern volatile uint8_t display_status;

#endif                          /*Timer_H_ */
