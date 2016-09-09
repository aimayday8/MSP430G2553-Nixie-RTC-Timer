//Parameters for light sensor, humidity sensor and RTC

#ifndef SENSOR_H_
#define SENSOR_H_
#include"stdint.h"
#include"I2C.h"

//#define DEC_TO_BCD(dec)			((dec/10) << 4 + dec % 10)
//#define BCD_TO_DEC(bcd)			(((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F))
#define SLAVE_ADDR_LIGHT 		0x39// light sensor
#define SLAVE_ADDR_RTC			0x68//RTC
#define SLAVE_ADDR_HUM			0x40//Humidity sensor

/*macro for lux data conversion start*/
#define LUX_SCALE 14
//#define LUX_SCALE 14 // scale by 2^14
#define RATIO_SCALE 9 // scale ratio by 2^9
#define CH_SCALE 10 // scale channel values by 2^10
#define CHSCALE_TINT0 0x7517 // 322/11 * 2^CH_SCALE
#define CHSCALE_TINT1 0x0fe7 // 322/81 * 2^CH_SCALE

#define K1T 0x0040 // 0.125 * 2^RATIO_SCALE
#define B1T 0x01f2 // 0.0304 * 2^LUX_SCALE
#define M1T 0x01be // 0.0272 * 2^LUX_SCALE

#define K2T 0x0080 // 0.250 * 2^RATIO_SCALE
#define B2T 0x0214 // 0.0325 * 2^LUX_SCALE
#define M2T 0x02d1 // 0.0440 * 2^LUX_SCALE

#define K3T 0x00c0 // 0.375 * 2^RATIO_SCALE
#define B3T 0x023f // 0.0351 * 2^LUX_SCALE
#define M3T 0x037b // 0.0544 * 2^LUX_SCALE

#define K4T 0x0100 // 0.50 * 2^RATIO_SCALE
#define B4T 0x0270 // 0.0381 * 2^LUX_SCALE
#define M4T 0x03fe // 0.0624 * 2^LUX_SCALE

#define K5T 0x0138 // 0.61 * 2^RATIO_SCALE
#define B5T 0x016f // 0.0224 * 2^LUX_SCALE
#define M5T 0x01fc // 0.0310 * 2^LUX_SCALE

#define K6T 0x019a // 0.80 * 2^RATIO_SCALE
#define B6T 0x00d2 // 0.0128 * 2^LUX_SCALE
#define M6T 0x00fb // 0.0153 * 2^LUX_SCALE

#define K7T 0x029a // 1.3 * 2^RATIO_SCALE
#define B7T 0x0018 // 0.00146 * 2^LUX_SCALE
#define M7T 0x0012 // 0.00112 * 2^LUX_SCALE

#define K8T 0x029a // 1.3 * 2^RATIO_SCALE
#define B8T 0x0000 // 0.000 * 2^LUX_SCALE
#define M8T 0x0000 // 0.000 * 2^LUX_SCALE
/*macro for lux data conversion end*/

typedef struct humiditytemperature
{
//	uint8_t hum_raw[2];//raw data for humidity
//	uint8_t temp_raw[2];//raw data for humidity
	uint16_t humidity;//humidity data in percent
	uint16_t temperature;//temperature data in percent
}sHum;
typedef struct realtimeclock
{
	uint8_t hour;//data for rtc hour
	uint8_t minute;//data for rtc minute
}sRTC;
typedef struct countdowntimer
{
	uint8_t minute;//timer minute count
	uint8_t second;//timer second count
}sTimer;
extern sHum hum_temp;
extern sRTC rtc;
extern sTimer timer;
extern uint8_t Dec_to_Bcd(uint8_t decimal);
extern uint8_t Bcd_to_Dec(uint8_t bcd);
extern void Read_Humtemp();//read in humidity and temperature
extern void Read_RTC();//read data from rtc
extern void Write_RTC();//write data to rtc
extern void Read_Lux(uint8_t gain, uint8_t tInt);//read in lux value
extern void Sensor_Init();//intialize all the I2C sensors
//extern void Calculate_lux(uint16_t ch0, uint16_t ch1, uint8_t gain, uint8_t tInt);

#endif					/*Sensor_H_*/
