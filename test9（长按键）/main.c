#include "STC15F2K60S2.h"

#define u8 unsigned char
#define u16 unsigned int

u8 dspbuf[8] = {10,10,10,10,10,10,10,10};  
u8 code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
	             // 0     1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16  17   18   19   20       
u8 dspcom = 0;
bit key_flag = 0;
	
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

void display()
{			  
	door(0xe0,0xff);

	door(0xc0,0x01<<dspcom);
	door(0xe0,tab[dspbuf[dspcom++]]);
	
	if(dspcom >= 8)
		dspcom = 0;
}

//u8 keypress = 0,keyread = 0,keyvalue = 0xff,keypress_long = 0,keyread_long = 0,keyvalue_long = 0xff;
//u8 Read_key()
//{
//    u8 key_m,cal;
//    
//    P3 = 0xf0;P42 = 1;P44 = 1;
//  	P36 = P42;P37 = P44;
//    key_m = P3&0xf0;

//    if(key_m != 0xf0) 
//		{
//      keypress++;
//			keypress_long++;
//		}
//    else
//        keypress = 0; 
//    
//    if(keypress == 3)
//    {
//		  keypress = 0;
//	  	keyread = 1;
//		  switch(key_m)
//        {
//            case 0x70:cal = 0;break;                
//            case 0xb0:cal = 1;break;    
//            case 0xd0:cal = 2;break;    
//            case 0xe0:cal = 3;break;
//        }
//        
//      P3 = 0x0f;P42 = 0;P44 = 0;
//      key_m = P3&0x0f;
//        
//			switch(key_m)
//			{
//					case 0x0e:keyvalue = (4*cal+7);
//										if(keypress_long >= 50)
//										{
//											keyread_long = 1;
//											keypress_long = 0;
//											keyvalue_long = keyvalue+20;
//										}
//										break;
//					case 0x0d:keyvalue = (4*cal+6);
//										if(keypress_long >= 50)
//										{
//											keyread_long = 1;
//											keypress_long = 0;
//											keyvalue_long = keyvalue+20;
//										}
//										break;
//					case 0x0b:keyvalue = (4*cal+5);
//										if(keypress_long >= 50)
//										{
//											keyread_long = 1;
//											keypress_long = 0;
//											keyvalue_long = keyvalue+20;
//										}
//										break;
//					case 0x07:keyvalue = (4*cal+4);
//										if(keypress_long >= 50)
//										{
//											keyread_long = 1;
//											keypress_long = 0;
//											keyvalue_long = keyvalue+20;
//										}
//										break;
//			}
//    }
//		
//    P3 = 0x0f;P42 = 0;P44 = 0;
//    key_m = P3&0x0f;
//	
//    if((keyread) && (key_m == 0x0f))
//    {
//      keyread = 0;
//			if(keyread_long)
//			{
//				keyread_long = 0;
//				return keyvalue_long;
//			}
//			else
//			{
//				keypress_long = 0;
//			  return keyvalue;
//			}
//		}
//    return 0xff;
//}

u8 keypress = 0,keyvalue = 0xff,keyread = 0,key_state = 0,key_200ms = 0;
u8 Read_key()
{
	u8 key_m,cal;
	
	P3 = 0xf0;P42 = 1;P44 = 1;
	P36 = P42;P37 = P44;
	key_m = P3&0xf0;

	switch(key_state)
	{
		case 0:
			if(key_m != 0xf0)     //需要放状态1里，防止松手时keypress清零了，导致又返回了短按键
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
					case 0x0e:keyvalue = (4*cal+7);key_state = 1;break;                
					case 0x0d:keyvalue = (4*cal+6);key_state = 1;break;    
					case 0x0b:keyvalue = (4*cal+5);key_state = 1;break;    
					case 0x07:keyvalue = (4*cal+4);key_state = 1;break;
				}
			}
			break;
		case 1:
			//松手检测
			P3 = 0x0f;P42 = 0;P44 = 0;
			key_m = P3&0x0f;
			if(key_m == 0x0f)   //松手则进入
			{
				key_state = 0;
				if(keypress <= 25)  //时间短则返回短按键
					return keyvalue;
			}
			
			//未松手则累加
			(keypress <= 25)?keypress++:key_200ms++;    //相当于<=25为短按键，>=35为长按键
			
			if(key_200ms >= 10)
			{
				key_200ms = 0;
				return keyvalue+20;
			}
		break;
	}
	return 0xff;
}

void main()		//```````````````````````````````````````````````````````````````````主函数
{ 
	u8 key_re;
	u8 dat = 50;
	
	Timer1Init();
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
					//短按键
					case 15:dspbuf[0] = 8;break;
					case 14:door(0x80,0x00);break;
					case 13:dspbuf[6] = 8;break;
					case 12:dspbuf[6] = 10;break;
					//长按键
					case 35:dspbuf[1] = 8;break;
					case 34:door(0x80,0xff);break;
					case 33:dspbuf[7] = 8;break;
					case 32:dspbuf[7] = 10;break;
					
					case 9:dat++;break;
					case 8:dat--;break;
					case 29:dat++;break;
					case 28:dat--;break;
				}
			}
		}
		
		dspbuf[4] = dat/10;
		dspbuf[5] = dat%10;
	}
}

void timer0() interrupt 3
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