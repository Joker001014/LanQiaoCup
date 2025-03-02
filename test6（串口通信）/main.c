#include "STC15F2K60S2.h"
#include "stdio.h"

#define u8 unsigned char
#define u16 unsigned int
u8 Uart_rec[10] = "",Uart_out[15] = "",Uart_count = 0;
u8 dat_1 = 20,dat_3 = 35,dat_4 = 30;
float dat_2 = 24.32;
	
//串口1、8位自动重装
void UartInit(void)		//4800bps@12.000MHz
{
	PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TL1 = 0xB2;		//设定定时初值
	TH1 = 0xB2;		//设定定时器重装值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
	
	ES = 1;
	EA = 1;
}

//发送数据函数
void Uart_send(u8 *str)
{
	u8 *s;
	
	s = str;
	// 遍历待发送的字符串
	while(*s != '\0')
	{
		SBUF = *s;	// 发送内容存入 SBUF 缓冲区
		while(!TI);		// 等待发送，发送结束 TI = 1 跳出循环
		TI = 0;		// 清除 TI 标志
		s++;		// 指向下一位
	}
}

// 串口处理函数
void Uart_pro()
{
	if(Uart_count > 0)
	{
		if(Uart_rec[Uart_count-1] == '\n')
		{
			//为发送数组赋值
			 if((Uart_rec[0]=='S') && (Uart_rec[1]=='T')) 
				 sprintf(Uart_out, "$%02u,%04.2f\r\n",(unsigned int)dat_1,(float)dat_2);    //注意△sprintf传送的是int型数据，要用unsigned int
	     else if((Uart_rec[0]=='P') && (Uart_rec[1]=='A') && (Uart_rec[2]=='R') && (Uart_rec[3]=='A')) 
				 sprintf(Uart_out, "#%02u,%02u\r\n",(unsigned int)dat_3, (unsigned int)dat_4); 
	     else
	       sprintf(Uart_out, "ERROR\r\n");        //△同时有"\r\n"才能换行
			 //发送数据
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

// 串口中断服务函数
void Uart() interrupt 4
{
	// 接收到谁
	if(RI)
	{
		RI = 0;				// 标志清零
		Uart_rec[Uart_count++] = SBUF;       	// 将接收数据存入 Uart_rec 数组，并将 Uart_count 统计接收个数+1
	}
}


