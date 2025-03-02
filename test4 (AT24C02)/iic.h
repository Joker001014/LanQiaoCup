#ifndef _IIC_H
#define _IIC_H

//void IIC_Start(void); 
//void IIC_Stop(void);  
//bit IIC_WaitAck(void);  
//void IIC_SendAck(bit ackbit); 
//void IIC_SendByte(unsigned char byt); 
//unsigned char IIC_RecByte(void); 

//unsigned char PCF8591_ADC(unsigned char addr);
//void PCF8591_DAC(unsigned char date);
//unsigned char EEPROM_Read(unsigned char addr);
//void EEPROM_Write(unsigned char addr,date);
void EEPROM_Read(unsigned char addr,unsigned char *p,unsigned char lens);
void EEPROM_Write(unsigned char addr,unsigned char *p,unsigned char lens);

#endif