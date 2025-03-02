/*
  ����˵��: IIC������������
  �������: Keil uVision 4.10 
  Ӳ������: CT107��Ƭ���ۺ�ʵѵƽ̨ 8051��12MHz
  ��    ��: 2011-8-9
*/

#include "STC15F2K60S2.h"
#include "intrins.h"

#define DELAY_TIME 5

#define SlaveAddrW 0xA0
#define SlaveAddrR 0xA1

//�������Ŷ���
sbit SDA = P2^1;  /* ������ */
sbit SCL = P2^0;  /* ʱ���� */

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


//������������
void IIC_Start(void)
{
    SDA = 1;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 0;
    IIC_Delay(DELAY_TIME);
    SCL = 0;	
}

//����ֹͣ����
void IIC_Stop(void)
{
    SDA = 0;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//����Ӧ��
void IIC_SendAck(bit ackbit)
{
    SCL = 0;
    SDA = ackbit;  					// 0��Ӧ��1����Ӧ��
    IIC_Delay(DELAY_TIME);
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SCL = 0; 
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//�ȴ�Ӧ��
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

//ͨ��I2C���߷�������
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

//��I2C�����Ͻ�������
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

//unsigned char PCF8591_ADC(unsigned char addr)     //ģ����ת����������
//{
//	unsigned char date;
//	
//	IIC_Start();              //��������
//	IIC_SendByte(0x90);       //0x90: 1001 0000  ��PCF8091д����
//	IIC_WaitAck();            //�ȴ�Ӧ��
//	IIC_SendByte(addr);       //д����
//	IIC_WaitAck();            //�ȴ�Ӧ��
//	IIC_Stop();               //�ر�����
//	
//	IIC_Start();              //��������
//	IIC_SendByte(0x91);       //0x91: 1001 0001  ��PCF8091������
//	IIC_WaitAck();            //�ȴ�Ӧ��
//	date = IIC_RecByte();     //��������
//	IIC_SendAck(1);           //����feiӦ���Ѿ���������
//	IIC_Stop();               //�ر�����
//	
//	return date;
//}

//void PCF8591_DAC(unsigned char date)     //������ת����ģ����
//{
//	IIC_Start();              //��������
//	IIC_SendByte(0x90);       //0x90: 1001 0000  ��PCF8091д����
//	IIC_WaitAck();            //�ȴ�Ӧ��
//	IIC_SendByte(0x40);       //����оƬҪ���ģ��������DA
//	IIC_WaitAck();            //�ȴ�Ӧ��
//	IIC_SendByte(date);       //
//	IIC_WaitAck();            //�ȴ�Ӧ��
//	IIC_Stop();               //�ر�����
//}

//void EEPROM_Write(unsigned char addr,date)     //��EEPROM������
//{
//	IIC_Start();              //��������
//	IIC_SendByte(0xa0);       //0xa0: �͵�ַΪ0xa0��оƬͨ�ţ���AT24C02
//	IIC_WaitAck();            //�ȴ�Ӧ��
//	IIC_SendByte(addr);       //��������Ҫ����ĵ�ַ
//	IIC_WaitAck();            //�ȴ�Ӧ��
//	IIC_SendByte(date);       //���뱣�������
//	IIC_WaitAck();            //�ȴ�Ӧ��
//	IIC_Stop();               //�ر�����
//	Delay5ms();
//}

//unsigned char EEPROM_Read(unsigned char addr)     //��EEPROM������
//{
//	unsigned char date;
//	
//	IIC_Start();              //��������
//	IIC_SendByte(0xa0);       //0xa0: ��AT24C02оƬͨ��
//	IIC_WaitAck();            //�ȴ�Ӧ��
//	IIC_SendByte(addr);       //дͨ�ŵ�ַ
//	IIC_WaitAck();            //�ȴ�Ӧ��
//	IIC_Stop();               //�ر�����
//	
//	IIC_Start();              //��������
//	IIC_SendByte(0xa1);       //0xa1: ��AT24C02������
//	IIC_WaitAck();            //�ȴ�Ӧ��
//	date = IIC_RecByte();     //��������
//	IIC_SendAck(1);           //���ͷ�Ӧ���Ѿ���������
//	IIC_Stop();               //�ر�����
//	
//	return date;
//}


void EEPROM_Write(unsigned char addr,unsigned char *p,unsigned char lens)     //��EEPROM������
{
	unsigned char i;
	
	IIC_Start();              //��������
	IIC_SendByte(0xa0);       //0xa0: �͵�ַΪ0xa0��оƬͨ�ţ���AT24C02
	IIC_WaitAck();            //�ȴ�Ӧ��
	IIC_SendByte(addr);       //��������Ҫ����ĵ�ַ
	IIC_WaitAck();            //�ȴ�Ӧ��
	for(i = 0;i<lens;i++)
	{
		IIC_SendByte(p[i]);       //���뱣�������
	  IIC_WaitAck();            //�ȴ�Ӧ��
	}
	IIC_Stop();               //�ر�����
//	Delay5ms();
}

void EEPROM_Read(unsigned char addr,unsigned char *p,unsigned char lens)     //��EEPROM������
{
	unsigned char i;
	
	IIC_Start();              //��������
	IIC_SendByte(0xa0);       //0xa0: ��AT24C02оƬͨ��
	IIC_WaitAck();            //�ȴ�Ӧ��
	IIC_SendByte(addr);       //дͨ�ŵ�ַ
	IIC_WaitAck();            //�ȴ�Ӧ��
	IIC_Stop();               //�ر�����
	
	IIC_Start();              //��������
	IIC_SendByte(0xa1);       //0xa1: ��AT24C02������
	IIC_WaitAck();            //�ȴ�Ӧ��
	for(i=0;i<lens;i++)
	{
		p[i] = IIC_RecByte();     //��������
		IIC_SendAck(0);           //����Ӧ��δ��������
	}
	IIC_SendAck(1);           //���ͷ�Ӧ���Ѿ���������
	IIC_Stop();               //�ر�����
}