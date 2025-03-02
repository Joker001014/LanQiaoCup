/*
  程序说明: DS1302驱动程序
  软件环境: Keil uVision 4.10 
  硬件环境: CT107单片机综合实训平台 8051，12MHz
  日    期: 2011-8-9
*/

#include "STC15F2K60S2.h"
#include "intrins.h"

sbit SCK=P1^7;		
sbit SDA=P2^3;		
sbit RST = P1^3;   // DS1302复位												

void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK=0;
		SDA=temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	return (temp);			
}

//void DS1302_init(unsigned char hour,minute,second)
//{
//	Write_Ds1302_Byte(0x8e,0x00);          //关闭写保护功能
//	Write_Ds1302_Byte(0x84,hour);
//	Write_Ds1302_Byte(0x82,minute);
//	Write_Ds1302_Byte(0x80,second);
//	Write_Ds1302_Byte(0x8e,0x80);          //打开写保护功能
//}	

//unsigned char hour,minute,second;
//void DS1302_read()
//{
//	
//	hour = Read_Ds1302_Byte(0x85);
//	minute = Read_Ds1302_Byte(0x83);
//	second = Read_Ds1302_Byte(0x81);
//	
//}

void DS1302_write(unsigned char *time_tab)
{
	Write_Ds1302_Byte(0x8e,0x00);
	Write_Ds1302_Byte(0x84,((time_tab[0]/10)<<4)+time_tab[0]%10);
	Write_Ds1302_Byte(0x82,((time_tab[1]/10)<<4)+time_tab[1]%10);
	Write_Ds1302_Byte(0x80,((time_tab[2]/10)<<4)+time_tab[2]%10);
	Write_Ds1302_Byte(0x8e,0x80);
}

unsigned char hour,minute,second;
void DS1302_read()
{
	hour = Read_Ds1302_Byte(0x85);
	minute = Read_Ds1302_Byte(0x83);
	second = Read_Ds1302_Byte(0x81);
}

//if((clock_flag) && (clock_w))
//{
//	clock_flag = 0;
//	DS1302_read();
//	
//	dspbuf[0] = hour/16;
//	dspbuf[1] = hour%16;
//	dspbuf[2] = 23;
//	dspbuf[3] = minute/16;
//	dspbuf[4] = minute%16;
//	dspbuf[5] = 23;
//	dspbuf[6] = second/16;
//	dspbuf[7] = second%16;
//	
//	time_tab[0] = hour/16*10+hour%16;
//	time_tab[1] = minute/16*10+minute%16;
//	time_tab[2] = second/16*10+second%16;
//}
