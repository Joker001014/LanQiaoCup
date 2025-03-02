#ifndef _IIC_H
#define _IIC_H

#include "STC15F2K60S2.h"
#include "intrins.h"

sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */
//void IIC_Start(void); 
//void IIC_Stop(void);  
//bit IIC_WaitAck(void);  
//void IIC_SendAck(bit ackbit); 
//void IIC_SendByte(unsigned char byt); 
//unsigned char IIC_RecByte(void); 

unsigned char PCF8591_ADC(unsigned char addr);
void PCF8591_DAC(unsigned char date);


#endif