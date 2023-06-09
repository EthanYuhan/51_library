#include "wave.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

#define Trig GPIO_Pin_9   //-->PB9
#define Echo GPIO_Pin_8   //-->PF8

float Distance;

void Wave_SRD_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitSture;
	EXTI_InitTypeDef  EXTI_InitSture;
	NVIC_InitTypeDef  NVIC_InitSture;
	//如果外部中断的话则一定使能AFIO复用功能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOF,ENABLE);
	
	
	//配置IO端口
	GPIO_InitSture.GPIO_Mode=GPIO_Mode_Out_PP;   //推挽输出模式  //-->PB9
	GPIO_InitSture.GPIO_Pin=Trig;                //将PE4于Trig相连
	GPIO_InitSture.GPIO_Speed=GPIO_Speed_50MHz;  
	GPIO_Init(GPIOB,&GPIO_InitSture);
	
	GPIO_InitSture.GPIO_Mode=GPIO_Mode_IPD;      //拉输入模式   //-->PF8
	GPIO_InitSture.GPIO_Pin=Echo;                //将PE6于Echo相连
	GPIO_InitSture.GPIO_Speed=GPIO_Speed_50MHz;  
	GPIO_Init(GPIOF,&GPIO_InitSture);
	
	
	
	//中断和8端口映射一起			//-->PF8
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource8);	
	
	
	//外部中断配置
	EXTI_InitSture.EXTI_Line=EXTI_Line8;
	EXTI_InitSture.EXTI_LineCmd=ENABLE;
	EXTI_InitSture.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitSture.EXTI_Trigger=EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitSture);
	
	
	//中断优先级管理
	NVIC_InitSture.NVIC_IRQChannel=EXTI9_5_IRQn;
	NVIC_InitSture.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitSture.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitSture.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitSture);
}

void EXTI9_5_IRQHandler(void)
{
	delay_us(10);
	
	
	if(EXTI_GetITStatus(EXTI_Line8)!=RESET)
	{
		TIM_SetCounter(TIM3,0);
		TIM_Cmd(TIM3,ENABLE);
		
		while(GPIO_ReadInputDataBit(GPIOF,Echo));  //等待低电平
		
		TIM_Cmd(TIM3,DISABLE);
		
		Distance=TIM_GetCounter(TIM3)*340/200.0;
		
		if(Distance>0)
		{
			printf("Distance:%f cm\r\n",Distance);
		}
			
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
}

void Wave_SRD_Strat(void)
{
	GPIO_SetBits(GPIOB,Trig);   //将Trig设置为高电平
	delay_us(20);               //持续大于10us触发，触发超声波模块工作
	GPIO_ResetBits(GPIOB,Trig); 
	
}











