#include"stdint.h"
#include"Sensor.h"

sHum hum_temp = {0,0};
sRTC rtc = {5,21};
sTimer timer = {0,0};
uint32_t lux = 0;
uint8_t Dec_to_Bcd(uint8_t decimal);
uint8_t Bcd_to_Dec(uint8_t bcd);
void Read_Humtemp();//read in humidity and temperature
void Read_RTC();//read data from rtc
void Write_RTC();//write data to rtc
void Read_Lux(uint8_t gain, uint8_t tInt);//read in lux value
void Sensor_Init();//intialize all the I2C sensors

uint8_t Dec_to_Bcd(uint8_t decimal)
{
	uint8_t temp = 0;
	temp = (decimal/10) << 4;
	decimal = temp + decimal % 10;
	return decimal;
}

uint8_t Bcd_to_Dec(uint8_t bcd)
{
	uint8_t temp = 0;
	temp = ((bcd & 0xF0) >> 4) * 10;
	bcd = temp + (bcd & 0x0F);
	return bcd;
}

void Sensor_Init()
{
	//RTC initialization
	I2C_Write(SLAVE_ADDR_RTC, 0x00, Dec_to_Bcd(0));//assume start form 0 seconds.
	I2C_Write(SLAVE_ADDR_RTC, 0x01, Dec_to_Bcd(21));
	I2C_Write(SLAVE_ADDR_RTC, 0x02, Dec_to_Bcd(5));
    __delay_cycles(5000);
	//light sensor initialization
//    I2C_Write(SLAVE_ADDR_LIGHT, 0x80, 0x03);//power up the light sensor
//	I2C_Write(SLAVE_ADDR_LIGHT, 0x81, 0x12);//high gain, 402ms
}
void Read_Humtemp()
{
	uint8_t data[2] = {0,0};
	uint16_t humidityCount = 0, temperatureCount = 0;
	uint32_t humudity_temp = 0, temperature_temp = 0;
	I2C_Read(SLAVE_ADDR_HUM, 0xE5, 2, data);
	humidityCount = (uint16_t)((uint16_t)data[0] << 8 | data[1]);
//		temp = (((humidityCount << 7) - (humidityCount << 1) - humidityCount)) >> 8;
//		humidity = (temp >> 8) - 6;
	humudity_temp = (((uint32_t)humidityCount) * 125) / 65536;
	hum_temp.humidity = (uint16_t)humudity_temp - 6;
	I2C_Read(SLAVE_ADDR_HUM, 0xE0, 2, data);
	temperatureCount = (uint16_t)((uint16_t)data[0] << 8 | data[1]);
	temperature_temp = (((uint32_t)temperatureCount) * 17572) / 65536;
	hum_temp.temperature = ((uint16_t)temperature_temp - 4685)/100;
}

void Read_RTC()
{
	uint8_t data[] = {0,0,0};
	I2C_Read(SLAVE_ADDR_RTC, 0x00, 3, data);
//	I2C_Read(SLAVE_ADDR_RTC, 0x02, 1, &rtc_hour_temp);
	rtc.minute = Bcd_to_Dec(data[1]);
	rtc.hour = Bcd_to_Dec(data[2]);
}

void Write_RTC()
{
	__bic_SR_register(GIE);//disable interrupt to ensure that the data is writting into the rtc immediately.
	I2C_Write(SLAVE_ADDR_RTC, 0x00, 0);//assume start form 0 seconds.
	I2C_Write(SLAVE_ADDR_RTC, 0x01, Dec_to_Bcd(rtc.minute));
	I2C_Write(SLAVE_ADDR_RTC, 0x02, Dec_to_Bcd(rtc.hour));
	__bis_SR_register(GIE);
}

void Read_Lux(uint8_t gain, uint8_t tInt)
{
	uint32_t chScale;
	uint32_t channel1;
	uint32_t channel0;
	uint8_t low, high;
	uint16_t cha0, cha1;
	switch (tInt)
	{
		case 0: // 13.7 msec
			chScale = CHSCALE_TINT0;
			break;
		case 1: // 101 msec
			chScale = CHSCALE_TINT1;
			break;
		default: // assume no scaling
			chScale = (1 << CH_SCALE);
			break;
	}
	// scale if gain is NOT 16X
	if (!gain) chScale = chScale << 4; // scale 1X to 16X
//	I2C_Write(SLAVE_ADDR_LIGHT, 0x80, 0x03);//power up the light sensor
//	I2C_Write(SLAVE_ADDR_LIGHT, 0x81, 0x12);//high gain, 402ms
	I2C_Read(SLAVE_ADDR_LIGHT, 0x8C, 1, &low);
	I2C_Read(SLAVE_ADDR_LIGHT, 0x8D, 1, &high);
	cha0 = high << 8 + low;
	I2C_Read(SLAVE_ADDR_LIGHT, 0x8E, 1, &low);
	I2C_Read(SLAVE_ADDR_LIGHT, 0x8F, 1, &high);
	cha1 = high << 8  + low;
	// scale the channel values
	channel0 = (cha0 * chScale) >> CH_SCALE;
	channel1 = (cha1 * chScale) >> CH_SCALE;
	// find the ratio of the channel values (Channel1/Channel0)
	// protect against divide by zero
	uint32_t ratio1 = 0;
	if (channel0 != 0)
		ratio1 = (channel1 << (RATIO_SCALE+1)) / channel0;
	// round the ratio value
	uint32_t ratio = (ratio1 + 1) >> 1;
	uint16_t b,m;
	if ((ratio >= 0) && (ratio <= K1T))
	{b=B1T; m=M1T;}
	else if (ratio <= K2T)
	{b=B2T; m=M2T;}
	else if (ratio <= K3T)
	{b=B3T; m=M3T;}
	else if (ratio <= K4T)
	{b=B4T; m=M4T;}
	else if (ratio <= K5T)
	{b=B5T; m=M5T;}
	else if (ratio <= K6T)
	{b=B6T; m=M6T;}
	else if (ratio <= K7T)
	{b=B7T; m=M7T;}
	else if (ratio > K8T)
	{b=B8T; m=M8T;}
	uint32_t temp;
	temp = channel0 * b - channel1 * m;
	// do not allow negative lux value
	if (temp < 0) temp = 0;
	// round lsb (2^(LUX_SCALE))
	temp += 1 << 13;
	// strip off fractional portion
	lux = temp >> LUX_SCALE;
}
