#include "STC15F2K60S2.h"

#define u8 unsigned char
#define u16 unsigned int
	
u8 dspbuf[8] = {10,10,10,10,10,10,10,10};
u8 code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbf,0xc6};
u8 pwm_plus = 0,dspcom = 0;
bit key_flag = 0,led_flag = 0;
sbit pwm = P3^4;

void Timer0Init(void)		//100΢��@12.000MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x50;		//���ö�ʱ��ֵ
	TH0 = 0xFB;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	
	ET0 = 1;
	EA = 1;
}

void Timer1Init(void)		//2����@12.000MHz
{
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x40;		//���ö�ʱ��ֵ
	TH1 = 0xA2;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	
	ET1 = 1;
	EA = 1;
}


void door(u8 choose,u8 input)	   //573��������װ�ɺ���
{
	P2 = (P2 & 0x1f) | choose;
	P0 = input;
	P2 &= 0x1f;	
}

void all_init()	     //�صơ��������������
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
u8 Read_key(void)		                              	//������̣���KBD
{
    u8 key_m,cal;
    
    P3 = 0xf0;P42=1;P44=1;
  	P36=P42;P37=P44;			   //�����滻
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
				P36=P42;P37=P44;			   //�����滻
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
		P36=P42;P37=P44;			                //�����滻
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

//ע�⣬����˳���ʽ��д��pwm_plusΪ2,ռ�ձȾ���20%����ͨ�����Լ��
//��ΪP34�����õ��ˣ����Ի���һ����������
void timer0() interrupt 1
{
	static tt = 0;
	
	if(tt >= pwm_plus)       //�õ��Կ��Է��֣�ռ�ձȰٷ�Ϊ50
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

	if(t_20ms >= 10)	//ÿ20msɨ��һ�ΰ���
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


