#include "beep.h"
//////////////////////////////////////////////////////////////////////////////////	 

//蜂鸣器驱动代码
								  
////////////////////////////////////////////////////////////////////////////////// 	   

//初始化PA15为输出口.并使能这个口的时钟		    
//蜂鸣器初始化
void BEEP_Init(void)
{
	 
 GPIO_InitTypeDef  GPIO_InitStructure; 
	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);    //打开GPIOA的AFIO时钟
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能GPIOA端口时钟
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 //BEEP-->PA15端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);	 //根据参数初始化GPIOA.15
	
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//禁用JTAG口调试，PA15作为普通IO
 
 GPIO_SetBits(GPIOA,GPIO_Pin_15);//输出1，关闭蜂鸣器输出

}
