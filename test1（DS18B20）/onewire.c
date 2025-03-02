/*
  ����˵��: ��������������
  �������: Keil uVision 4.10 
  Ӳ������: CT107��Ƭ���ۺ�ʵѵƽ̨(�ⲿ����12MHz) STC89C52RC��Ƭ��
  ��    ��: 2011-8-9
*/
#include "STC15F2K60S2.h"

#define u8 unsigned char
#define u16 unsigned int

sbit DQ = P1^4;  //�����߽ӿ�

void Delay_OneWire(u16 t)  //STC89C52RC
{
	u8 num;
	while(t--)
		for(num = 0;num <= 12;num++);
}

//ͨ����������DS18B20дһ���ֽ�
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(5);
}

//��DS18B20��ȡһ���ֽ�
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5);
	}
	return dat;
}

//DS18B20�豸��ʼ��
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	DQ = 1;
  	Delay_OneWire(12);
  	DQ = 0;
  	Delay_OneWire(80);
  	DQ = 1;
  	Delay_OneWire(10); 
    initflag = DQ;     
  	Delay_OneWire(5);
  	return initflag;
}


int rd_temperature(void)
{
	int temp = 0;
	unsigned char tmh,tml;
	
  init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	
	tml=Read_DS18B20();
	tmh=Read_DS18B20();
	
	temp = tmh;
	temp <<= 8;
	temp |= tml;
	
	return temp;
}

//void datachange(long int temp)
//{
//	temp = temp*0.0625*10000;

//	dspbuf[1] = temp/1000000;
//	dspbuf[2] = temp%1000000/100000;
//	dspbuf[3] = temp%100000/10000+11;
//	dspbuf[4] = temp%10000/1000;
//	dspbuf[5] = temp%1000/100;
//	dspbuf[6] = temp%100/10;
//	dspbuf[7] = temp%10;
//}




