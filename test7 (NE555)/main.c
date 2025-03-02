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

void Timer_Init(void)		//2����@12.000MHz
{
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	//��ע�⣺��Ҫ�� |=
	TMOD |= 0x05;		//��ʱ��T1(00���Զ���װ��)  ������T0(01��16λ����װ��TL0��TH0ȫ��)    0000 0101(��3��6λ��0����ʱ��1������)
	TL1 = 0x40;		//���ö�ʱ��ֵ
	TH1 = 0xA2;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	
  TR0 = 1;        //��������0
	ET0 = 1;
	ET1 = 1;
	EA = 1;         //�����ж�        
}

///////////////////////Ҳ�ɲ����������/////////////////////
//void Timer0Init(void)    //������
//{
//    AUXR |= 0x80;
//		TMOD |= 0x05;    
//    TL0 = 0x00;        //���ü�����ֵ
//    TH0 = 0x00;        //���ü�����ֵ
//    TF0 = 0;
//    TR0 = 0;
//    ET0 = 0;
//}
//void Timer1Init(void)        //2����@12.000MHz
//{
//    AUXR |= 0x40;        //��ʱ��ʱ��1Tģʽ
//    TMOD &= 0x0F;        //���ö�ʱ��ģʽ
//    TL1 = 0x40;        //���ö�ʱ��ֵ
//    TH1 = 0xA2;        //���ö�ʱ��ֵ
//    TF1 = 0;        //���TF1��־
//    TR1 = 1;        //��ʱ��1��ʼ��ʱ
//	
//    ET1 = 1;
//		EA  = 1;
//}
///////////////////////Ҳ�ɲ����������/////////////////////

//////////////////////����ʱ��1���ڴ���ʱ�����ö�ʱ��2��ʱ/////////////////////
//void Timer5Init(void)		//2����@12.000MHz
//{
//	AUXR |= 0x04;		//��ʱ��ʱ��1Tģʽ
//	T2L = 0x40;		//���ö�ʱ��ֵ
//	T2H = 0xA2;		//���ö�ʱ��ֵ
//	AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ

//	IE2 |= 0x04;   //��
//	EA = 1;
//}
//////////////////////����ʱ��1���ڴ���ʱ�����ö�ʱ��2��ʱ/////////////////////

//�̽�signal��P34���ı�Rb3�ɸı�Ƶ��
void main()          //****************************************************************************************************
{
	u8 key_re,NE555_w = 0;
	u16 freq = 0;        //����Ƶ��
	
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
					case 7:NE555_w ^= 1;break;   //�ò���num_close()
					case 6:;break;
					case 5:;break;
					case 4:door(0x80,0x55);break;
				}
			}
		}
		
		if(NE555_flag)
		{
			NE555_flag = 0;
			TR0 = 0;      //�ؼ�����0
//			TR1 = 0;      //�ض�ʱ��1
			
			freq = TL0+TH0*256;
			
			TH0 = 0;      //��ռ���
			TL0 = 0;      //��ռ���
			
			TR0 = 1;      //�򿪼�����0
//			TR1 = 1;      //�򿪼�����1
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

