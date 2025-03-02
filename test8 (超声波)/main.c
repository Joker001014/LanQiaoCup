#include "STC15F2K60S2.h" 

#define u8 unsigned char
#define u16 unsigned int

u8 dspbuf[8] = {10,10,10,10,10,10,10,10};  
u8 code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
	             // 0     1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16  17   18   19   20       
u8 dspcom = 0;
bit key_flag = 0;
	
//////////////////////////////////////////////////////////////////////////////////////////
sbit TX = P1^0;    // 发射引脚
sbit RX = P1^1;    // 接收引脚
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

void Timer1Init(void)		//2毫秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x40;		//设置定时初值
	TH1 = 0xA2;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	
	ET1 = 1;
	EA = 1;
}

//定时器0、12T时钟
void Timer0Init(void) // 12us@12.000MHz    频率：40KHZ,1/40000 = 25us，其中高电平12.5us，低电平12.5us，约12us
{ 
	AUXR &= 0x7F;    // 定时器时钟12T模式 △
	TMOD &= 0xF0;    // 设置定时器模式
	TL0 = 0xF4;      // 设置定时初值
	TH0 = 0xFF;      // 设置定时初值
	TF0 = 0;         // 清除TF0溢出标志位
	TR0 = 0;         // 关闭定时器0  △
} 

u8 Wave_recv(void) 
{
	 u8 distance,count = 10; 
	 
	 TX = 0;           // 发射引脚置0
	 TL0 = 0xF4;       // 设置定时初值
	 TH0 = 0xFF;       // 设置定时初值
	 TR0 = 1;          // 打开定时器 0
	
	 // TX 引脚发送 40kHz 方波信号驱动超声波发送探头
	 while(count--) 
	 {
		 while(!TF0);    // TF0溢出标志位为0是卡死，直至TF0满12us溢出，跳出循环
		 TX ^= 1;        // 发送信号翻转
		 TF0 = 0;        // 溢出标志位清零
	 }
	 
	 TR0 = 0;          // 关闭定时器0，将其再次赋初值
	 TL0 = 0;          // 设置定时初值
	 TH0 = 0;          // 设置定时初值
	 TR0 = 1;          // 打开定时器0
	 
	 while(RX && !TF0);  // 等待收到脉冲，收到脉冲时RX将产生低电平，将跳出循环。并开始计时，1us计数加一
	 TR0 = 0;            // 关闭定时器0，此时TL0、TH0被记录下来
	 
	 if(TF0)           // 测距大于1m时，发生溢出
	 { 
		 TF0 = 0;        // 标志位清零
		 distance = 255;   // 即0xff，表示超出测量范围
	 } 
	 else              // 计算距离
		distance = ((TH0<<8)+TL0)*0.017;   // t*0.000001(1us)*340/2(往返)*100(换算成厘米) = t * 0.017

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