#include "STC15F2K60S2.h"

#define u8 unsigned char
#define u16 unsigned int

u8 dspbuf[8] = {10,10,10,10,10,10,10,10};  
u8 code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
    
u8 dspcom = 0;
bit key_flag = 0,NE555_flag = 0;
	
void door(u8 choose,input)
{
	P2 = (P2 & 0x1f) | choose;
	P0 = input;
	P2 &= 0x1f;	
}

void clo_num()
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
	clo_num();
}

void display()
{			  
	door(0xe0,0xff);

	door(0xc0,0x01<<dspcom);
	door(0xe0,tab[dspbuf[dspcom++]]);
	
	if(dspcom >= 8)
		dspcom = 0;
}

u8 keypress = 0,keyvalue = 0xff,keyread = 0;
u8 Read_key()
{
    u8 key_m,cal;
    
    P3 = 0xf0;P42 = 1;P44 = 1;
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
        
        P3 = 0x0f;P42 = 0;P44 = 0;
        key_m = P3&0x0f;
        
			switch(key_m)
					{
							case 0x0e:keyvalue = (4*cal+7);break;                
							case 0x0d:keyvalue = (4*cal+6);break;    
							case 0x0b:keyvalue = (4*cal+5);break;    
							case 0x07:keyvalue = (4*cal+4);break;
					}
    }

    P3 = 0x0f;P42 = 0;P44 = 0;
    key_m = P3&0x0f;
	
    if((keyread) && (key_m == 0x0f))
    {
        keyread = 0;
        return keyvalue;	
    }
    return 0xff; 
}

void Timer_Init(void)		//2毫秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	//△注意：需要用 |=
	TMOD |= 0x05;		//定时器T1(00：自动重装载)  计数器T0(01：16位不重装，TL0、TH0全用)    0000 0101(第3、6位，0：定时，1：计数)
	TL1 = 0x40;		//设置定时初值
	TH1 = 0xA2;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	
  TR0 = 1;        //开计数器0
	ET0 = 1;
	ET1 = 1;
	EA = 1;         //打开总中断        
}

///////////////////////也可拆成两个函数/////////////////////
//void Timer0Init(void)    //计数器
//{
//    AUXR |= 0x80;
//		TMOD |= 0x05;    
//    TL0 = 0x00;        //设置计数初值
//    TH0 = 0x00;        //设置计数初值
//    TF0 = 0;
//    TR0 = 0;
//    ET0 = 0;
//}
//void Timer1Init(void)        //2毫秒@12.000MHz
//{
//    AUXR |= 0x40;        //定时器时钟1T模式
//    TMOD &= 0x0F;        //设置定时器模式
//    TL1 = 0x40;        //设置定时初值
//    TH1 = 0xA2;        //设置定时初值
//    TF1 = 0;        //清除TF1标志
//    TR1 = 1;        //定时器1开始计时
//	
//    ET1 = 1;
//		EA  = 1;
//}
///////////////////////也可拆成两个函数/////////////////////

//////////////////////当定时器1用于串口时，可用定时器2定时/////////////////////
//void Timer5Init(void)		//2毫秒@12.000MHz
//{
//	AUXR |= 0x04;		//定时器时钟1T模式
//	T2L = 0x40;		//设置定时初值
//	T2H = 0xA2;		//设置定时初值
//	AUXR |= 0x10;		//定时器2开始计时

//	IE2 |= 0x04;   //△
//	EA = 1;
//}
//////////////////////当定时器1用于串口时，可用定时器2定时/////////////////////

//短接signal和P34，改变Rb3可改变频率
void main()          //****************************************************************************************************
{
	u8 key_re,NE555_w = 0;
	u16 freq = 0;        //定义频率
	
	all_init();
	Timer_Init();
//	Timer0Init();
//	Timer1Init();
//	Timer5Init();
	
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
					case 7:NE555_w ^= 1;break;   //用不了num_close()
					case 6:;break;
					case 5:;break;
					case 4:door(0x80,0x55);break;
				}
			}
		}
		
		if(NE555_flag)
		{
			NE555_flag = 0;
			TR0 = 0;      //关计数器0
//			TR1 = 0;      //关定时器1
			
			freq = TL0+TH0*256;
			
			TH0 = 0;      //清空计数
			TL0 = 0;      //清空计数
			
			TR0 = 1;      //打开计数器0
//			TR1 = 1;      //打开计数器1
		}
		
		if((key_flag) && (NE555_w))
		{
			key_flag = 0;
			
			dspbuf[0] = freq/10000;
			dspbuf[1] = freq%10000/1000;
			dspbuf[2] = freq%1000/100;
			dspbuf[3] = freq%100/10;
			dspbuf[4] = freq%10;
		}
	}
}

void timer1() interrupt 3
{
	static u8 t_20ms = 0;
	static u16 t_1000ms = 0;

	display();
	
	t_20ms++;
	t_1000ms++;
	
	if(t_20ms >= 10)
	{
		t_20ms = 0;
		key_flag = 1;
	}
	
	if(t_1000ms >= 500)
	{
		t_1000ms = 0;
		NE555_flag = 1;
	}
}

//void timer2() interrupt 12
//{
//	static u8 t_20ms = 0;
//	static u16 t_1000ms = 0;

//	display();
//	
//	t_20ms++;
//	t_1000ms++;
//	
//	if(t_20ms >= 10)
//	{
//		t_20ms = 0;
//		key_flag = 1;
//	}
//	
//	if(t_1000ms >= 500)
//	{
//		t_1000ms = 0;
//		NE555_flag = 1;
//	}
//}

