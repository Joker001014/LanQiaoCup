#include "STC15F2K60S2.h"

#define u8 unsigned char
#define u16 unsigned int

u8 dspbuf[8] = {10,10,10,10,10,10,10,10};  
u8 code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
	             // 0     1    2    3    4    5    6    7    8    9    10  11   12   13   14   15   16  17   18   19   20       
u8 dspcom = 0;
bit key_flag = 0;
	
void door(u8 choose,input)
{
	P2 = (P2 & 0x1f) | choose;
	P0 = input;
	P2 &= 0x1f;	
}

void num_close()
{
	dspbuf[0] = 10;
	dspbuf[1] = 10;
	dspbuf[2] = 10;
	dspbuf[3] = 10;
	dspbuf[4] = 10;
	dspbuf[5] = 10;
	dspbuf[6] = 10;
	dspbuf[7] = 10;
}

void all_init()
{
 	door(0x80,0xff);
	door(0xa0,0xaf);
	num_close();
}

void Timer0Init(void)		//2毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x40;		//设置定时初值
	TH0 = 0xA2;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	
	ET0 = 1;
	EA = 1;
}

void display()
{			  
	door(0xe0,0xff);

	door(0xc0,0x01<<dspcom);
	door(0xe0,tab[dspbuf[dspcom++]]);
	
	if(dspcom >= 8)
		dspcom = 0;
}

//2*4矩阵键盘
u8 keypress = 0,keyvalue = 0xff,keyread = 0;
u8 Read_key()
{
    u8 key_m,cal;
    
    P32 = 0;P33 = 0;P34 = 1;P35 = 1;P42 = 1;P44 = 1;
  	P36 = P42;P37 = P44;
    key_m = P3&0xf0;

    if(key_m != 0xf0) 
        keypress++;
    else
        keypress = 0; 
    
    if(keypress == 3)
    {
		  keypress = 0;
	  	keyread = 1;
		  switch(key_m)
			{
					case 0x70:cal = 0;break;                
					case 0xb0:cal = 1;break;    
					case 0xd0:cal = 2;break;    
					case 0xe0:cal = 3;break;
			}
        
        P32 = 1;P33 = 1;P34 = 0;P35 = 0;P42 = 0;P44 = 0;
        key_m = P3&0x0c;
        
			switch(key_m)
			{
					case 0x08:keyvalue = (4*cal+5);break;    
					case 0x04:keyvalue = (4*cal+4);break;
			}
    }

    P32 = 1;P33 = 1;P34 = 0;P35 = 0;P42 = 0;P44 = 0;
		key_m = P3&0x0c;
	
    if((keyread) && (key_m == 0x0c))
    {
        keyread = 0;
        return keyvalue;	
    }
    return 0xff;  
}
//2*2矩阵键盘
//u8 Read_key()
//{
//    u8 key_m,cal;
//    
//		P42 = 1;P44 = 1;P33 = 0;P32 = 0;
//  	P36 = P42;P37 = P44;
//    key_m = P3&0xc0;

//    if(key_m != 0xc0) 
//        keypress++;
//   else
//        keypress = 0; 
//    
//    if(keypress == 3)
//    {
//		  keypress = 0;
//	  	keyread = 1;
//		  switch(key_m)
//			{
//					case 0x40:cal = 0;break;                
//					case 0x80:cal = 1;break;    
//			}
//        
//			P42 = 0;P44 = 0;P33 = 1;P32 = 1;
//			key_m = P3&0x0c;
//        
//			switch(key_m)
//			{
//					case 0x08:keyvalue = (4*cal+5);break;    
//					case 0x04:keyvalue = (4*cal+4);break;
//			}
//    }

//    P42 = 0;P44 = 0;P33 = 1;P32 = 1;
//		key_m = P3&0x0c;

//    if((keyread) && (key_m == 0x0c))
//    {
//        keyread = 0;
//        return keyvalue;	
//    }
//    return 0xff;  
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{ 
	u8 key_re;
	
	Timer0Init();
	all_init();

 	while(1)
	{
		if(key_flag)			
		{
			key_flag = 0;
			key_re = Read_key();
			
			if(key_re != 0xff)
			{
				switch(key_re)
				{
					case 7:door(0x80,0xff);break;
					case 6:door(0x80,0x00);break;
					case 5:door(0x80,0xaa);break;
					case 4:door(0x80,0x55);break;
					
					case 11:dspbuf[3] = 0;break;
					case 10:dspbuf[2] = 0;break;
					case 9:dspbuf[1] = 0;break;
					case 8:dspbuf[0] = 0;break;
					
					case 15:dspbuf[7] = 0;break;
					case 14:dspbuf[6] = 0;break;
					case 13:dspbuf[5] = 0;break;
					case 12:dspbuf[4] = 0;break;
					
					case 19:dspbuf[6] = 10;dspbuf[7] = 10;break;
					case 18:dspbuf[4] = 10;dspbuf[5] = 10;break;
					case 17:dspbuf[2] = 10;dspbuf[3] = 10;break;
					case 16:dspbuf[0] = 10;dspbuf[1] = 10;break;
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void timer0() interrupt 1
{
	static u8  t_20ms = 0;
	
	display();

	t_20ms++;

	if(t_20ms >= 10)
	{
		t_20ms = 0;
		key_flag = 1;
	}
}