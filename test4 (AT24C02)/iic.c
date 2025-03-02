/*
  程序说明: IIC总线驱动程序
  软件环境: Keil uVision 4.10 
  硬件环境: CT107单片机综合实训平台 8051，12MHz
  日    期: 2011-8-9
*/

#include "STC15F2K60S2.h"
#include "intrins.h"

#define DELAY_TIME 5

#define SlaveAddrW 0xA0
#define SlaveAddrR 0xA1

//总线引脚定义
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */

void IIC_Delay(unsigned char i)
{
    do{_nop_();}
    while(i--);        
}

//void Delay5ms()		//@12.000MHz
//{
//	unsigned char i, j;

//	i = 59;
//	j = 90;
//	do
//	{
//		while (--j);
//	} while (--i);
//}


//总线启动条件
void IIC_Start(void)
{
    SDA = 1;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 0;
    IIC_Delay(DELAY_TIME);
    SCL = 0;	
}

//总线停止条件
void IIC_Stop(void)
{
    SDA = 0;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//发送应答
void IIC_SendAck(bit ackbit)
{
    SCL = 0;
    SDA = ackbit;  					// 0：应答，1：非应答
    IIC_Delay(DELAY_TIME);
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SCL = 0; 
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//等待应答
bit IIC_WaitAck(void)
{
    bit ackbit;
	
    SCL  = 1;
    IIC_Delay(DELAY_TIME);
    ackbit = SDA;
    SCL = 0;
    IIC_Delay(DELAY_TIME);
    return ackbit;
}

//通过I2C总线发送数据
void IIC_SendByte(unsigned char byt)
{
    unsigned char i;

    for(i=0; i<8; i++)
    {
        SCL  = 0;
        IIC_Delay(DELAY_TIME);
        if(byt & 0x80) SDA  = 1;
        else SDA  = 0;
        IIC_Delay(DELAY_TIME);
        SCL = 1;
        byt <<= 1;
        IIC_Delay(DELAY_TIME);
    }
    SCL  = 0;  
}

//从I2C总线上接收数据
unsigned char IIC_RecByte(void)
{
    unsigned char i, da;
    for(i=0; i<8; i++)
    {   
    	SCL = 1;
	IIC_Delay(DELAY_TIME);
	da <<= 1;
	if(SDA) da |= 1;
	SCL = 0;
	IIC_Delay(DELAY_TIME);
    }
    return da;    
}

//unsigned char PCF8591_ADC(unsigned char addr)     //模拟量转化成数字量
//{
//	unsigned char date;
//	
//	IIC_Start();              //开启总线
//	IIC_SendByte(0x90);       //0x90: 1001 0000  向PCF8091写数据
//	IIC_WaitAck();            //等待应答
//	IIC_SendByte(addr);       //写数据
//	IIC_WaitAck();            //等待应答
//	IIC_Stop();               //关闭总线
//	
//	IIC_Start();              //开启总线
//	IIC_SendByte(0x91);       //0x91: 1001 0001  向PCF8091读数据
//	IIC_WaitAck();            //等待应答
//	date = IIC_RecByte();     //接受数据
//	IIC_SendAck(1);           //发送fei应答：已经读完数据
//	IIC_Stop();               //关闭总线
//	
//	return date;
//}

//void PCF8591_DAC(unsigned char date)     //数字量转化成模拟量
//{
//	IIC_Start();              //开启总线
//	IIC_SendByte(0x90);       //0x90: 1001 0000  向PCF8091写数据
//	IIC_WaitAck();            //等待应答
//	IIC_SendByte(0x40);       //告诉芯片要输出模拟量，即DA
//	IIC_WaitAck();            //等待应答
//	IIC_SendByte(date);       //
//	IIC_WaitAck();            //等待应答
//	IIC_Stop();               //关闭总线
//}

//void EEPROM_Write(unsigned char addr,date)     //向EEPROM存数据
//{
//	IIC_Start();              //开启总线
//	IIC_SendByte(0xa0);       //0xa0: 和地址为0xa0的芯片通信，即AT24C02
//	IIC_WaitAck();            //等待应答
//	IIC_SendByte(addr);       //输入数据要保存的地址
//	IIC_WaitAck();            //等待应答
//	IIC_SendByte(date);       //输入保存的数据
//	IIC_WaitAck();            //等待应答
//	IIC_Stop();               //关闭总线
//	Delay5ms();
//}

//unsigned char EEPROM_Read(unsigned char addr)     //从EEPROM读数据
//{
//	unsigned char date;
//	
//	IIC_Start();              //开启总线
//	IIC_SendByte(0xa0);       //0xa0: 和AT24C02芯片通信
//	IIC_WaitAck();            //等待应答
//	IIC_SendByte(addr);       //写通信地址
//	IIC_WaitAck();            //等待应答
//	IIC_Stop();               //关闭总线
//	
//	IIC_Start();              //开启总线
//	IIC_SendByte(0xa1);       //0xa1: 从AT24C02读数据
//	IIC_WaitAck();            //等待应答
//	date = IIC_RecByte();     //接受数据
//	IIC_SendAck(1);           //发送非应答：已经读完数据
//	IIC_Stop();               //关闭总线
//	
//	return date;
//}


void EEPROM_Write(unsigned char addr,unsigned char *p,unsigned char lens)     //向EEPROM存数据
{
	unsigned char i;
	
	IIC_Start();              //开启总线
	IIC_SendByte(0xa0);       //0xa0: 和地址为0xa0的芯片通信，即AT24C02
	IIC_WaitAck();            //等待应答
	IIC_SendByte(addr);       //输入数据要保存的地址
	IIC_WaitAck();            //等待应答
	for(i = 0;i<lens;i++)
	{
		IIC_SendByte(p[i]);       //输入保存的数据
	  IIC_WaitAck();            //等待应答
	}
	IIC_Stop();               //关闭总线
//	Delay5ms();
}

void EEPROM_Read(unsigned char addr,unsigned char *p,unsigned char lens)     //从EEPROM读数据
{
	unsigned char i;
	
	IIC_Start();              //开启总线
	IIC_SendByte(0xa0);       //0xa0: 和AT24C02芯片通信
	IIC_WaitAck();            //等待应答
	IIC_SendByte(addr);       //写通信地址
	IIC_WaitAck();            //等待应答
	IIC_Stop();               //关闭总线
	
	IIC_Start();              //开启总线
	IIC_SendByte(0xa1);       //0xa1: 从AT24C02读数据
	IIC_WaitAck();            //等待应答
	for(i=0;i<lens;i++)
	{
		p[i] = IIC_RecByte();     //接受数据
		IIC_SendAck(0);           //发送应答：未读完数据
	}
	IIC_SendAck(1);           //发送非应答：已经读完数据
	IIC_Stop();               //关闭总线
}