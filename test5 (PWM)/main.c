#include "STC15F2K60S2.h"

#define u8 unsigned char
#define u16 unsigned int
	
u8 dspbuf[8] = {10,10,10,10,10,10,10,10};
u8 code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbf,0xc6};
u8 pwm_plus = 0,dspcom = 0;
bit key_flag = 0,led_flag = 0;
sbit pwm = P3^4;

void Timer0Init(void)		//100微秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x50;		//设置定时初值
	TH0 = 0xFB;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	
	ET0 = 1;
	EA = 1;
}

void Timer1Init(void)		//2毫秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x40;		//设置定时初值
	TH1 = 0xA2;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	
	ET1 = 1;
	EA = 1;
}


void door(u8 choose,u8 input)	   //573锁存器封装成函数
{
	P2 = (P2 & 0x1f) | choose;
	P0 = input;
	P2 &= 0x1f;	
}

void all_init()	     //关灯、蜂鸣器、数码管
{
 	door(0x80,0xff);
	door(0xa0,0xaf);
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
u8 Read_key(void)		                              	//矩阵键盘，接KBD
{
    u8 key_m,cal;
    
    P3 = 0xf0;P42=1;P44=1;
  	P36=P42;P37=P44;			   //变量替换
    key_m = (P3 & 0xf0);

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
        
        P3 = 0x0f;P42=0;P44=0;
				P36=P42;P37=P44;			   //变量替换
        key_m = (P3 & 0x0f);
        
			switch(key_m)
					{
							case 0x0e:keyvalue = (4*cal+7);break;                
							case 0x0d:keyvalue = (4*cal+6);break;    
							case 0x0b:keyvalue = (4*cal+5);break;    
							case 0x07:keyvalue = (4*cal+4);break;
					}
    }

    P3 = 0x0f;P42=0;P44=0;
		P36=P42;P37=P44;			                //变量替换
    key_m = (P3&0x0f);
	
    if((keyread == 1) && (key_m == 0x0f))
    {
        keyread = 0;
        return keyvalue;	
    }
    
    return 0xff;  
}

void main()      //***************************************************************************************************
{
	u8 key_re,mode = 0,led_put = 0;
	
	all_init();
	Timer0Init();
	Timer1Init();
	
	while(1)
	{
		dspbuf[6] = pwm_plus/10;
		dspbuf[7] = pwm_plus%10;
		
		if(key_flag)			
		{
			key_flag = 0;
			key_re = Read_key();
			
			if(key_re != 0xff)
			{
				switch(key_re)
				{
					case 12:pwm_plus+=2;
					        if(pwm_plus > 10)
										pwm_plus = 0;
									break;
				}
			}
		}
	}
}

//注意，按此顺序格式书写，pwm_plus为2,占空比就是20%，可通过调试检查
//因为P34按键用到了，所以会用一个脉冲跳变
void timer0() interrupt 1
{
	static tt = 0;
	
	if(tt >= pwm_plus)       //用调试可以发现，占空比百分为50
	{
		pwm = 0;
		door(0x80,0xff);   
	}
	else
	{
		pwm = 1;
		door(0x80,0x00);
	}
	
	tt++;
	
	if(tt >= 10)
		tt = 0;
}

void timer1() interrupt 3
{
	static u8  t_20ms = 0;   //MAX = 500 ms
	static int t_1s;
	
	display();

	t_20ms++;
	t_1s++;

	if(t_20ms >= 10)	//每20ms扫描一次按键
	{
		t_20ms = 0;
		key_flag = 1;
	}
	
	if(t_1s >= 500)
	{
		t_1s = 0;
		led_flag = 1;
	}
}


