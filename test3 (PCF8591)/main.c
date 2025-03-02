#include "STC15F2K60S2.h"
#include "iic.h"

#define u8 unsigned char
#define u16 unsigned int

u8 dspbuf[8] = {10,10,10,10,10,10,10,10};  
u8 code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x89,0xc1,0xbf};
	             //  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7 ,  8 ,  9 , �� , 0. , 1. , 2. , 3. , 4. , 5. , 6. , 7. , 8. , 9. ,  H ,  U ,  -  ; 
	             //  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7 ,  8 ,  9 , 10 , 11 , 12 , 13 , 14 , 15 , 16 , 17 , 18 , 19 , 20 , 21 , 22 , 23  ;
u8 dspcom = 0,i;
bit key_flag = 0,iic_flag1 = 0,iic_flag2 = 0;


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

void Uart_init(void)		//9600bps@12.000MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ
	TL1 = 0xC7;		//�趨��ʱ��ֵ
	TH1 = 0xFE;		//�趨��ʱ��ֵ
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1
	EA = 1;
	ES = 1;
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
	
    if((keyread == 1) && (key_m == 0x0f))
    {
        keyread = 0;
        return keyvalue;	
    }
    
    return 0xff;  
}

void main()		//```````````````````````````````````````````````````````````````````������
{
	u8 key_re,iic_w = 0,vol = 0;
	int date_1,date_2;
	
	all_init();
	Uart_init();
	Timer0Init();
	
	
	PCF8591_DAC(200);
	
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
					case 9:;break;
					case 8:;break;
					case 15:door(0xa0,0xbf);break;
					case 14:door(0xa0,0xaf);break;
					case 13:door(0x80,0xfe);break;
					case 12:door(0x80,0xfd);break;
					case 19:door(0x80,0xfb);break;	
					case 18:vol +=10;break;
					case 17:iic_w = 1;;break;
					case 16:iic_w = 0;
					        clo_num();break;
					default:break;
				}
			}
		}
		
			if((iic_flag1) && (iic_w))
			{
//				PCF8591_DAC(i);
//				PCF8591_DAC(200);
				iic_flag1 = 0;
				date_1 = PCF8591_ADC(0);   //��Rb2��ѹ
				date_1 = 5*date_1/255.0*100;

				dspbuf[0] = date_1/100+11;
				dspbuf[1] = date_1%100/10;
				dspbuf[2] = date_1%10; 
			}
			
			if((iic_flag2) && (iic_w))
			{
				iic_flag2 = 0;
				date_2 = PCF8591_ADC(3);     //��A/D���Ų��ѹ
				date_2 = 5*date_2/255.0*100;
				
				dspbuf[5] = date_2/100+11;
				dspbuf[6] = date_2%100/10;
				dspbuf[7] = date_2%10;
			}
	}
}


void timer0() interrupt 1	  //````````````````��ʱ��0�ж�
{
	static u8  t_20ms = 0,t_100ms = 0;   //MAX = 500 ms

	//����Ҫ����װ��ֵ
	
	display();		//�������ʾ����������

	t_20ms++;
	t_100ms++;

	if(t_20ms >= 10)	//ÿ20msɨ��һ�ΰ���
	{
		t_20ms = 0;
		key_flag = 1;
	}
	
	if(t_100ms == 100)      
	{
		iic_flag1 = 1;
	}
	if(t_100ms == 200)
	{
		t_100ms = 0;
		iic_flag2 = 1;
	}
}

void Uart() interrupt 4
{
//	u8 i;
	
	if(RI)
	{
		RI = 0;
		i = SBUF;
//		PCF8591_DAC(i);
		SBUF = i;
		while(!TI);
		TI = 0;
	}
}