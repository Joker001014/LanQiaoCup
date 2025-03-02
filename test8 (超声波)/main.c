#include "STC15F2K60S2.h" 

#define u8 unsigned char
#define u16 unsigned int

u8 dspbuf[8] = {10,10,10,10,10,10,10,10};  
u8 code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
	             // 0     1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16  17   18   19   20       
u8 dspcom = 0;
bit key_flag = 0;
	
//////////////////////////////////////////////////////////////////////////////////////////
sbit TX = P1^0;    // ��������
sbit RX = P1^1;    // ��������
//////////////////////////////////////////////////////////////////////////////////////////
	
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

//��ʱ��0��12Tʱ��
void Timer0Init(void) // 12us@12.000MHz    Ƶ�ʣ�40KHZ,1/40000 = 25us�����иߵ�ƽ12.5us���͵�ƽ12.5us��Լ12us
{ 
	AUXR &= 0x7F;    // ��ʱ��ʱ��12Tģʽ ��
	TMOD &= 0xF0;    // ���ö�ʱ��ģʽ
	TL0 = 0xF4;      // ���ö�ʱ��ֵ
	TH0 = 0xFF;      // ���ö�ʱ��ֵ
	TF0 = 0;         // ���TF0�����־λ
	TR0 = 0;         // �رն�ʱ��0  ��
} 

u8 Wave_recv(void) 
{
	 u8 distance,count = 10; 
	 
	 TX = 0;           // ����������0
	 TL0 = 0xF4;       // ���ö�ʱ��ֵ
	 TH0 = 0xFF;       // ���ö�ʱ��ֵ
	 TR0 = 1;          // �򿪶�ʱ�� 0
	
	 // TX ���ŷ��� 40kHz �����ź���������������̽ͷ
	 while(count--) 
	 {
		 while(!TF0);    // TF0�����־λΪ0�ǿ�����ֱ��TF0��12us���������ѭ��
		 TX ^= 1;        // �����źŷ�ת
		 TF0 = 0;        // �����־λ����
	 }
	 
	 TR0 = 0;          // �رն�ʱ��0�������ٴθ���ֵ
	 TL0 = 0;          // ���ö�ʱ��ֵ
	 TH0 = 0;          // ���ö�ʱ��ֵ
	 TR0 = 1;          // �򿪶�ʱ��0
	 
	 while(RX && !TF0);  // �ȴ��յ����壬�յ�����ʱRX�������͵�ƽ��������ѭ��������ʼ��ʱ��1us������һ
	 TR0 = 0;            // �رն�ʱ��0����ʱTL0��TH0����¼����
	 
	 if(TF0)           // ������1mʱ���������
	 { 
		 TF0 = 0;        // ��־λ����
		 distance = 255;   // ��0xff����ʾ����������Χ
	 } 
	 else              // �������
		distance = ((TH0<<8)+TL0)*0.017;   // t*0.000001(1us)*340/2(����)*100(���������) = t * 0.017

	 return distance; 
}

void main()
{
	u8 distance;
	
	all_init();
	Timer1Init();
	Timer0Init();
	
	while(1)
	{
		if(key_flag)
		{
			key_flag = 0;
			distance = Wave_recv();
			
			dspbuf[5] = distance/100;
			dspbuf[6] = distance%100/10;
			dspbuf[7] = distance%10;
		}
	}
}

void timer1() interrupt 3
{
	static u8 t_20ms = 0;
	
	display();
	
	t_20ms++;
	
	if(t_20ms >= 10)
	{
		t_20ms = 0;
		key_flag = 1;
	}
}