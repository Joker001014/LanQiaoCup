#include "STC15F2K60S2.h"
#include "stdio.h"

#define u8 unsigned char
#define u16 unsigned int
u8 Uart_rec[10] = "",Uart_out[15] = "",Uart_count = 0;
u8 dat_1 = 20,dat_3 = 35,dat_4 = 30;
float dat_2 = 24.32;
	
//����1��8λ�Զ���װ
void UartInit(void)		//4800bps@12.000MHz
{
	PCON &= 0x7F;		//�����ʲ�����
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�����ʱ��1ģʽλ
	TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TL1 = 0xB2;		//�趨��ʱ��ֵ
	TH1 = 0xB2;		//�趨��ʱ����װֵ
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1
	
	ES = 1;
	EA = 1;
}

//�������ݺ���
void Uart_send(u8 *str)
{
	u8 *s;
	
	s = str;
	// ���������͵��ַ���
	while(*s != '\0')
	{
		SBUF = *s;	// �������ݴ��� SBUF ������
		while(!TI);		// �ȴ����ͣ����ͽ��� TI = 1 ����ѭ��
		TI = 0;		// ��� TI ��־
		s++;		// ָ����һλ
	}
}

// ���ڴ�����
void Uart_pro()
{
	if(Uart_count > 0)
	{
		if(Uart_rec[Uart_count-1] == '\n')
		{
			//Ϊ�������鸳ֵ
			 if((Uart_rec[0]=='S') && (Uart_rec[1]=='T')) 
				 sprintf(Uart_out, "$%02u,%04.2f\r\n",(unsigned int)dat_1,(float)dat_2);    //ע���sprintf���͵���int�����ݣ�Ҫ��unsigned int
	     else if((Uart_rec[0]=='P') && (Uart_rec[1]=='A') && (Uart_rec[2]=='R') && (Uart_rec[3]=='A')) 
				 sprintf(Uart_out, "#%02u,%02u\r\n",(unsigned int)dat_3, (unsigned int)dat_4); 
	     else
	       sprintf(Uart_out, "ERROR\r\n");        //��ͬʱ��"\r\n"���ܻ���
			 //��������
			 Uart_send(Uart_out); 
	     Uart_count = 0;
		}
	}
}

void main()
{
	UartInit();
	
	while(1)
	{
		Uart_pro();
	}
}

// �����жϷ�����
void Uart() interrupt 4
{
	// ���յ�˭
	if(RI)
	{
		RI = 0;				// ��־����
		Uart_rec[Uart_count++] = SBUF;       	// ���������ݴ��� Uart_rec ���飬���� Uart_count ͳ�ƽ��ո���+1
	}
}


