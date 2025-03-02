#include "STC15F2K60S2.h"
#include "iic.h"

#define u8 unsigned char
#define u16 unsigned int
#define u32 unsigned long int


u8 dspbuf[8] = {10,10,10,10,10,10,10,10};
u8 code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x89,0xc1,0xbf};
	             //  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7 ,  8 ,  9 , 灭 , 0. , 1. , 2. , 3. , 4. , 5. , 6. , 7. , 8. , 9. ,  H ,  U ,  -  ; 
	             //  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7 ,  8 ,  9 , 10 , 11 , 12 , 13 , 14 , 15 , 16 , 17 , 18 , 19 , 20 , 21 , 22 , 23  ;
u8 dspcom = 0;
bit key_flag = 0,EEPROM_flag = 0;


void Timer0Init(void)		//2毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x40;		//设置定时初值
	TH0 = 0xA2;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	
	ET0 = 1;		//开定时中断
	EA= 1;			//开总中断
}

void door(u8 choose,u8 input)	   //573锁存器封装成函数
{
	P2 = (P2 & 0x1f) | choose;
	P0 = input;
	P2 &= 0x1f;	
}

void clo_num()        //关数码管
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

void all_init()	     //关灯、蜂鸣器、数码管
{
 	door(0x80,0xff);
	door(0xa0,0xaf);
	clo_num();
}

void display()		//数码管显示函数
{			  
	door(0xe0,0xff);		//消隐

	door(0xc0,0x01<<dspcom);
	door(0xe0,tab[dspbuf[dspcom++]]);
	
	if(dspcom >= 8)     //or dspcom &= 0x07;
		dspcom = 0;
}

u8 keypress = 0,keyvalue = 0xff,keyread = 0;
u8 Read_key(void)		                              	//矩阵键盘，接KBD
{
    u8 key_m,cal;
    
    P3 = 0xf0;P42=1;P44=1;
  	P36=P42;P37=P44;			   //变量替换
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
				P36=P42;P37=P44;			   //变量替换
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
		P36=P42;P37=P44;			                //变量替换
    key_m = (P3&0x0f);
	
    if((keyread == 1) && (key_m == 0x0f))
    {
        keyread = 0;
        return keyvalue;	
    }
    
    return 0xff;  
}

//float第N个字节转换成uint8
u8 Float2Uint8(float num,u8 N)
{
    u8* temp;
    temp = (u8 *)&num;
    return *(temp+N);
}

//float转换为uint32
u32 Float2Uint32(float dat)
{
    u32 temp = 0;
		u8 i;
    for(i=0;i<4;i++)
        temp |= Float2Uint8(dat,i)<<(i*8);
    return temp;
}

float Uint82Float(u8* p)
{
    float temp;
    *((u8*)&temp) = *p;
    *((u8*)&temp+1) = *(p+1);
    *((u8*)&temp+2) = *(p+2);
    *((u8*)&temp+3) = *(p+3);
    return temp;
}
//uint32转float
float Uint322Float(u32 dat)
{
    float temp;
    *((u8*)&temp) = dat&0x00ff;
    *((u8*)&temp+1) = (dat>>8)&0x00ff;
    *((u8*)&temp+2) = (dat>>16)&0x00ff;
    *((u8*)&temp+3) = (dat>>24)&0x00ff;

    return temp;
}

u8 num = 50;
void main()		//```````````````````````````````````````````````````````````````````主函数
{
	u8 key_re,date,EEPROM_w = 0;
	u8 str[8] = {0,1,3,1,4,5,2,0},lens = 4;
	u8 put[8] = {1,1,1,1,1,1,1,1};
	float dat = 12.345,dat_show;
	u8 input[4],ouput[4];
	u32 temp_test = 0,temp1,show;
	
	all_init();
	Timer0Init();
	
	
	EEPROM_Write(1,str,8);    //地址0不行

	
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
					case 7:EEPROM_Read(1,put,8);;break;	
					case 6:door(0x80,0xff);break;
					case 5:
							temp1 = dat*1000;
							*(u32 *)input = temp1;
							EEPROM_Write(0,input,4);
					break;
					case 4:
//						*(u32 *)input = (u32 *)&dat;
//						temp_test = input[0]*256*256*256+input[1]*256*256+input[2]*256+input[3];
						
//						temp_test = Float2Uint32(dat);
//						dat_show = Uint322Float(temp_test);
//						show = dat_show*1000;
					
							EEPROM_Read(0,ouput,4);
							show = ouput[0]*256*256*256+ouput[1]*256*256+ouput[2]*256+ouput[3];
					break;
					case 10:door(0xa0,0xaf);break;
					case 11:door(0xa0,0xef);break;
					case 9:;break;
					case 8:;break;
//					case 15:EEPROM_Write(1,num);
//				        	break;
//					case 14:date = EEPROM_Read(1);break;
//					case 13:num++;EEPROM_Write(0,num);break;
//					case 12:num--;EEPROM_Write(0,num);break;
					case 19:EEPROM_w = 1;break;	
					case 18:EEPROM_w = 0;
					        clo_num();break;
					case 17:dspbuf[6] = num%100/10;
				          dspbuf[7] = num%10;     break;
					case 16:door(0x80,0xdf);break;
					default:break;
				}
			}
		}
//		
//		if(EEPROM_flag)
//			{
//				EEPROM_flag =0;
//				date = EEPROM_Read(0);
//				
//				dspbuf[5] = date/100;
//				dspbuf[6] = date%100/10;
//				dspbuf[7] = date%10;
//			}
		
//			dspbuf[0] = temp_test/100000000;
//			dspbuf[1] = temp_test%10000000/1000000;
//			dspbuf[2] = temp_test%1000000/100000;
//			dspbuf[3] = temp_test%100000/10000;
//			dspbuf[4] = temp_test%10000/1000;
//			dspbuf[5] = temp_test%1000/100;
//			dspbuf[6] = temp_test%100/10;
//			dspbuf[7] = temp_test%10;
		

			dspbuf[0] = show/100000000;
			dspbuf[1] = show%10000000/1000000;
			dspbuf[2] = show%1000000/100000;
			dspbuf[3] = show%100000/10000;
			dspbuf[4] = show%10000/1000;
			dspbuf[5] = show%1000/100;
			dspbuf[6] = show%100/10;
			dspbuf[7] = show%10;
		
			if(EEPROM_flag)
			{
				EEPROM_flag =0;
//				EEPROM_Read(2,put,8);
				
				dspbuf[0] = put[0];
				dspbuf[1] = put[1];
				dspbuf[2] = put[2];
				dspbuf[3] = put[3];
				dspbuf[4] = put[4];
				dspbuf[5] = put[5];
				dspbuf[6] = put[6];
				dspbuf[7] = put[7];
			}
	}
}


void timer0() interrupt 1	  //````````````````定时器0中断
{
	static u8  t_20ms = 0,t_50ms = 0;   //MAX = 500 ms

	//不需要重载装载值
	
	display();		//数码管显示函数放里面

	t_20ms++;
	t_50ms++;

	if(t_20ms >= 10)	//每20ms扫描一次按键
	{
		t_20ms = 0;
		key_flag = 1;
	}		
	
	if(t_50ms >= 50)
	{
	  t_50ms = 0;
		EEPROM_flag = 1;
	}
}