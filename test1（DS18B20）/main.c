#include "STC15F2K60S2.h"
#include "onewire.h"
#include "stdio.h"

#define u8 unsigned char
#define u16 unsigned int

u8 dspbuf[8] = {10,10,10,10,10,10,10,10};  
u8 code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x89,0xc1,0xbf};
u8 dspcom = 0;
bit key_flag = 0,temp_flag = 0,flag = 0,flag1 = 0,temp_w = 0;


void Timer0Init(void)		//2毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x40;		//设置定时初值
	TH0 = 0xA2;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	
	ET0 = 1;		//开定时中断
	EA= 1;			//开总中断
}

//void delay(float rate)  //延时函数：0.5s-0.5;1s-1;2s-2...
//{
//	u8 num;
//	while(1)
//	{
//		if(key_flag)
//		{
//			num++;
//			key_flag = 0;
//		}
//		if(num == 50*rate)
//		{
//			num = 0;
//			break;
//		}
//	}
//}

void door(u8 choose,u8 input)	   //573锁存器封装成函数
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

void all_init()	     //关灯、蜂鸣器、数码管
{
 	door(0x80,0xff);
	door(0xa0,0xaf);
	clo_num();
}

void display()		//数码管显示函数
{			  
	door(0xe0,0xff);		//消隐

	door(0xc0,0x01<<dspcom);
	door(0xe0,tab[dspbuf[dspcom++]]);
	
	if(dspcom >= 8)
		dspcom = 0;
}

//u8 Read_key()               //独立按键，接BTN
//{
//	u8 key_m;
//	P3 = 0x0f;
//	key_m = (P3 & 0x0f);
//	
//	if(key_m != 0x0f)
//		keypress++;
//	else
//		keypress = 0;
//	
//	if(keypress == 3)
//	{
//		keypress = 0;
//		keyread = 1;
//		switch(key_m)
//		{
//			case 0x0e:keyvalue = 7;break;
//			case 0x0d:keyvalue = 6;break;
//			case 0x0b:keyvalue = 5;break;
//			case 0x07:keyvalue = 4;break;
//	  }
//	}
//	
//		P3 = 0x0f;
//		key_m = (P3 & 0x0f);
//		
//		if((keyread==1) && (key_m==0x0f))
//		{
//			keyread = 0;
//			return keyvalue;
//		}
//	return 0xff;
//}

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
							case 0x0e:keyvalue = (4*cal+7);return keyvalue;	break;                
							case 0x0d:keyvalue = (4*cal+6);return keyvalue;	break;    
							case 0x0b:keyvalue = (4*cal+5);return keyvalue;	break;    
							case 0x07:keyvalue = (4*cal+4);return keyvalue;	break;
					}
    }

    P3 = 0x0f;P42=0;P44=0;
		P36=P42;P37=P44;			                //变量替换
    key_m = (P3&0x0f);
	
    if((keyread == 1) && (key_m == 0x0f))    //松手不检测
    {
        keyread = 0;
//        return keyvalue;	
			  temp_w = 0;
    }
    
    return 0xff;  
}

void datachange(long int temp)
{
	temp = temp*0.0625*10000;

	dspbuf[1] = temp/1000000;
	dspbuf[2] = temp%1000000/100000;
	dspbuf[3] = temp%100000/10000+11;
	dspbuf[4] = temp%10000/1000;
	dspbuf[5] = temp%1000/100;
	dspbuf[6] = temp%100/10;
	dspbuf[7] = temp%10;
}

void main()		//```````````````````````````````````````````````````````````````````主函数
{ 
	u8 key_re,num = 10;
//	u8 i;
//	u8 w = 0xff;
	
	Timer0Init();
	all_init();
	
//	for(i=0;i<=8;i++)   //流水灯
//	{
//		door(0x80,~(0x01<<i));
//		delay(0.2);
//	}
//	delay(1);
//	for(i=0;i<10;i++)
//	{
//		w = ~w;
//		door(0x80,~(0x01&w));
//		delay(1);
//	}

 	while(1)
	{
	  if((temp_flag) && (temp_w))
			{
				temp_flag = 0;
				datachange(rd_temperature());
			}
			
		if(!temp_w)
			clo_num();


		if(key_flag)			
		{
			key_flag = 0;
			key_re = Read_key();
			
			if(key_re != 0xff)
			{
				switch(key_re)
				{
					case 7:door(0x80,0x00);break;	//在这里放按键要执行的功能
					case 6:door(0x80,0xff);break;
					case 5:door(0x80,0xaa);break;
					case 4:door(0x80,0x55);break;
					case 11:door(0x80,0x0f);break;
					case 10:door(0x80,0xf0);break;
					case 9:door(0x80,0x0f);break;
					case 8:door(0x80,0xf0);break;
					case 15: break;
					case 14:door(0xa0,0xaf);break;
					case 13:temp_w = 1;break;
					case 12:temp_w = 0;
					        clo_num();break;
					case 19:door(0x80,0xfb);break;	//在这里放按键要执行的功能
					case 18:door(0x80,0xf7);break;
					case 17:door(0x80,0xef);break;
					case 16:door(0x80,0xdf);break;
					default:break;
				}
			}
		}
	}
}


void timer0() interrupt 1	  //````````````````定时器0中断
{
	static u8  t_20ms = 0,t_500ms = 0,t_50ms = 0;
	//不需要重载装载值
	
	display();		//数码管显示函数放里面

	t_20ms++;
	t_500ms++;				
	t_50ms++;

	if(t_20ms >= 10)	//每20ms扫描一次按键
	{
		t_20ms = 0;
		key_flag = 1;
	}

	if(t_500ms >= 250)
	{
		t_500ms = 0;
		flag = 1;
	}
	
	if(t_50ms >= 25)
	{
	  t_50ms = 0;
		temp_flag = 1;
	}
}

//void timer1() interrupt 3	  //````````````````定时器1中断
//{
//	static u8 t1_500ms = 0;
//	
//	t1_500ms++;
//	
//	if(t1_500ms >= 250)
//	{
//		t1_500ms = 0;
//		flag1 = 1;
//	}
//	
//	
//}






