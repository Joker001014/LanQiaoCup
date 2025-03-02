#include "STC15F2K60S2.h"

#define u8 unsigned char
#define u16 unsigned int
	
u8 dspbuf[8] = {10,10,10,10,10,10,10,10};  
u8 code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
	             // 0     1    2    3    4    5    6    7    8    9    10  11   12   13   14   15   16  17   18   19   20       
u8 dspcom = 0,send_dat = 0;
bit key_flag = 0;

void door(u8 choose,input)
{
	P2 = (P2 & 0x1f) | choose;
	P0 = input;
	P2 &= 0x1f;	
}

void display()
{			  
	door(0xe0,0xff);

	door(0xc0,0x01<<dspcom);
	door(0xe0,tab[dspbuf[dspcom++]]);
	
	if(dspcom >= 8)
		dspcom = 0;
}

void Timer0Init(void)		//2����@12.000MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x40;		//���ö�ʱ��ֵ
	TH0 = 0xA2;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	
	ET0 = 1;
	EA = 1;
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

//����1��8λ�Զ�����
void Uart_init(void)		//9600bps@12.000MHz
{
	PCON &= 0x7F;		//�����ʲ�����
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�����ʱ��1ģʽλ
	TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TL1 = 0xD9;		//�趨��ʱ��ֵ
	TH1 = 0xD9;		//�趨��ʱ����װֵ
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ
	ES = 1;
	EA = 1;
}

///ע�⣺�������ǵ�ѡ��12MHz
void main()
{
	u8 key_re;
	
	 Timer0Init();
   Uart_init();
	
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
					case 7:send_dat += 5;break;
					case 4:send_dat -= 5;break;
				}
			}
		 }
	 }
}

void Uart() interrupt 4   //�����жϺ�������4
{
	u8 i = 0;
	
	if(RI)       //��������
	{
		RI=0;
		i=SBUF;           //��SBUF�������ж�������
		dspbuf[7] = i;    //��ʾ��������
//		PCF8591_DAC(i);
	}
}

void timer0() interrupt 1
{	
	static u8  t_20ms = 0,t_50ms = 0;
	
	display();

	t_20ms++;
	t_50ms++;

	if(t_20ms >= 10)
	{
		t_20ms = 0;
		key_flag = 1;
	}
	
	if(t_50ms >= 25)
	{
		t_50ms = 0;
		//��������
		SBUF = send_dat;  //����ܻ�������������
		while(!TI);
		TI=0;
	}
}


