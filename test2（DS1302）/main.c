#include "STC15F2K60S2.h"
#include "ds1302.h"

#define u8 unsigned char
#define u16 unsigned int


u8 dspbuf[8] = {10,10,10,10,10,10,10,10};  
u8 code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x89,0xc1,0xbf};
	             //  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7 ,  8 ,  9 , �� , 0. , 1. , 2. , 3. , 4. , 5. , 6. , 7. , 8. , 9. ,  H ,  U ,  -  ; 
u8 time_tab[3] = {23,59,50};
u8 dspcom = 0;
bit clock_flag = 0,key_flag = 0;

void Timer0Init(void)		//2����@12.000MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x40;		//���ö�ʱ��ֵ
	TH0 = 0xA2;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	
	ET0 = 1;		//����ʱ�ж�
	EA= 1;			//�����ж�
}

void door(u8 choose,u8 input)	   //573��������װ�ɺ���
{
	P2 = (P2 & 0x1f) | choose;
	P0 = input;
	P2 &= 0x1f;	
}

void clo_num()        //�������
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

void all_init()	     //�صơ��������������
{
 	door(0x80,0xff);
	door(0xa0,0xaf);
	clo_num();
}

void display()		//�������ʾ����
{			  
	door(0xe0,0xff);		//����

	door(0xc0,0x01<<dspcom);
	door(0xe0,tab[dspbuf[dspcom++]]);
	
	if(dspcom >= 8)     //or dspcom &= 0x07;
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
	
    if((keyread == 1) && (key_m == 0x0f))        //���ּ��
    {
        keyread = 0;
        return keyvalue;	
    }
    
    return 0xff;  
}

void main()		//`````````````````````````````````````````````````````````````````````````````````````������
{
  u8 key_re,clock_w;
	
	all_init();
	Timer0Init();
	
//	DS1302_init(0x23,0x59,0x55);
	
	DS1302_write(time_tab);
	
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
					case 10:door(0xa0,0xaf);break;
					case 11:door(0xa0,0xef);break;
					case 9:clock_w = 1;break;
					case 8:clock_w = 0;
					       clo_num();break;
					case 15:door(0xa0,0xbf);break;
					case 14:door(0xa0,0xaf);break;
					case 13:time_tab[0]++;
					        if(time_tab[0] == 24)
										time_tab[0] = 0;
									DS1302_write(time_tab);break;
					case 12:time_tab[0]--;
					        if(time_tab[0] <= 0)
										time_tab[0] = 23;
									DS1302_write(time_tab);break;
					case 19:door(0x80,0xfb);break;	
					case 18:door(0x80,0xf7);break;
					case 17:door(0x80,0xef);break;
					case 16:door(0x80,0xdf);break;
					default:break;
				}
			}
		}
		
		if((clock_flag) && (clock_w))
		{
			clock_flag = 0;
			DS1302_read();
			
			dspbuf[0] = hour/16;
			dspbuf[1] = hour%16;
			dspbuf[2] = 23;
			dspbuf[3] = minute/16;
			dspbuf[4] = minute%16;
			dspbuf[5] = 23;
			dspbuf[6] = second/16;
			dspbuf[7] = second%16;
			
			time_tab[0] = hour/16*10+hour%16;
			time_tab[1] = minute/16*10+minute%16;
			time_tab[2] = second/16*10+second%16;
		}
	}
}


void timer0() interrupt 1	  //````````````````��ʱ��0�ж�
{
	static u8  t_20ms = 0,t_50ms = 0;   //MAX = 500 ms

	//����Ҫ����װ��ֵ
	
	display();		//�������ʾ����������

	t_20ms++;
	t_50ms++;

	if(t_20ms >= 10)	//ÿ20msɨ��һ�ΰ���
	{
		t_20ms = 0;
		key_flag = 1;
	}

	if(t_50ms >= 25)
	{
	  t_50ms = 0;
		clock_flag = 1;
	}
}