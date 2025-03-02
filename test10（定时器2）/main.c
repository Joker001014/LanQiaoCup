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

//定时器2
void Timer2Init(void)		//2毫秒@12.000MHz
{
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x40;		//设置定时初值
	T2H = 0xA2;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时

	IE2 |= 0x04;   //△
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

void main()		//```````````````````````````````````````````````````````````````````主函数
{ 
	u8 key_re;
	
	Timer2Init();
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
					case 7:door(0x80,0x00);break;
					case 6:door(0x80,0xff);break;
					case 5:door(0x80,0xaa);break;
					case 4:door(0x80,0x55);break;
				}
			}
		}
	}
}

void timer2() interrupt 12
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