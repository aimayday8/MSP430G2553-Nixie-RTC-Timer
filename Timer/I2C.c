//I2C Driver.
#include"I2C.h"


void I2C_Init(uint8_t devAddr);
void I2C_transmit_Init(uint8_t slave_address,uint8_t regAddr);
void I2C_Read(uint8_t slave_address,uint8_t regAddr, uint8_t len, uint8_t* data);
void I2C_Write(uint8_t slave_address,uint8_t regAddr, uint8_t data);

void I2C_Init(uint8_t devAddr)
{
    UCB0CTL1 |= UCSWRST; // Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC; // I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST; // Use SMCLK, keep SW reset
    UCB0BR0 = 12; // fSCL = 1Mhz/10 = ~100kHz
    UCB0BR1 = 0;
    UCB0I2CSA = devAddr; // Slave Address is 069h
    P1SEL |=  BIT6 + BIT7; // Assign I2C pins to USCI_B0 // Assign Uart pins to USCI_A0
    P1SEL2 |= BIT6 + BIT7; // Assign I2C pins to USCI_B0 // Assign Uart pins to USCI_A0
    UCB0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
//    IE2 |= UCB0RXIE + UCB0TXIE; // Enable RX and TX interrupt
}
void I2C_transmit_Init(uint8_t slave_address, uint8_t regAddr)
{
    while (UCB0CTL1 & UCTXSTP); // Ensure stop condition got sent
    UCB0I2CSA = slave_address;                   // Slave Address is 05fh
    UCB0CTL1 |= UCTR + UCTXSTT; // I2C start condition with UCTR flag for transmit
    UCB0TXBUF = regAddr; //write registerAddr in TX buffer
    while((UCB0CTL1 & UCTXSTT)); // // Wait for start condition and data to send
}
void I2C_Read(uint8_t slave_address, uint8_t regAddr, uint8_t len, uint8_t* data)
{
    uint8_t i = 0;
    I2C_transmit_Init(slave_address, regAddr);
    UCB0CTL1 &= ~UCTR ; // Clear I2C TX flag for receive
    UCB0CTL1 |= UCTXSTT;             // I2C TX, start condition
    while (UCB0CTL1 & UCTXSTT);             // Wait for start condition and data to send
    if(len == 1){
        UCB0CTL1 |= UCTXSTP;             // I2C TX, stop condition
        while (!(IFG2 & UCB0RXIFG));
        *data = UCB0RXBUF;
    }
    else{
        for(i = 0; i < len; i++){
            if(i == (len - 1))
                UCB0CTL1 |= UCTXSTP;             // I2C TX, stop condition
            while (!(IFG2 & UCB0RXIFG));
            data[i] = UCB0RXBUF;
        }
    }
}

void I2C_Write(uint8_t slave_address, uint8_t regAddr, uint8_t data)
{
    I2C_transmit_Init(slave_address, regAddr);
//    while (IFG2 & UCB0TXIFG);
    while (!(IFG2 & UCB0TXIFG));
    UCB0TXBUF = data; //write registerAddr in TX buffer
//    while (IFG2 & UCB0TXIFG);
    while (!(IFG2 & UCB0TXIFG));
    UCB0CTL1 |= UCTXSTP; // I2C stop condition
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
}
