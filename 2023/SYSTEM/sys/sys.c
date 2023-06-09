
#include "sys.h"

/*
24MHz主频下 
在示波器下看
一个语句的时间大概是 0.8us
*/



//2微秒	延时
//此函数是以2us的倍数增加
//t  2us的倍数
void delay2us(u16 us)
 {
  u16 i;
  u8 m;
  for(i=0;i<us;i++)
  for(m=0;m<3;m++);
 }
 
void delay_ms(u16 ms)
{
 u16 m;
 for(m=0;m<ms;m++)
 delay4us(250);
} 

//为兼容原程序
void delay_us(u16 us)
 {
  u16 i;
  u8 m;
  for(i=0;i<us;i++)
  for(m=0;m<1;m++);
 }
 
 
 
 //4微秒	延时
//此函数是以4us的倍数增加
//t  4us的倍数
void delay4us(u16 us)
 {
  u16 i;
  u8 m;
  for(i=0;i<us;i++)
  for(m=0;m<5;m++);
 }

 
 //1毫秒 延时
//ms 1ms的倍数
void delay1ms(u16 ms)
{
 u16 m;
 for(m=0;m<ms;m++)
 delay4us(250);
} 

 
void Delay100ms(void)	//@24.000MHz   
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 10;
	j = 31;
	k = 147;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
