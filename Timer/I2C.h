//I2C Driver.
#ifndef I2C_H_
#define I2C_H_

#include"stdint.h"
#include<msp430.h>


extern void I2C_Init(uint8_t devAddr);
extern void I2C_transmit_Init(uint8_t slave_address,uint8_t regAddr);
extern void I2C_Read(uint8_t slave_address,uint8_t regAddr, uint8_t len, uint8_t* data);
extern void I2C_Write(uint8_t slave_address,uint8_t regAddr, uint8_t data);

#endif                          /*I2C_H_ */
