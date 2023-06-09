#include "motor.h"
#include "delay.h"
#include "lcd.h"

//////////////////////////////////////////////////////////////////////////////////	 

//步进电机驱动代码	   
//详细请参考https://blog.csdn.net/nimbrethil/article/details/97618772
								  
////////////////////////////////////////////////////////////////////////////////// 

     
//步进电机 IO初始化
void MOTOR_Init(void)
{
	
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启引脚相关的GPIO外设时钟*/
		RCC_APB2PeriphClockCmd( IN1_GPIO_CLK | IN2_GPIO_CLK | IN3_GPIO_CLK | IN4_GPIO_CLK , ENABLE);
		
	 /*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = IN1_GPIO_PIN;	
		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		/*调用库函数，初始化GPIO*/
		GPIO_Init(IN1_GPIO_PORT, &GPIO_InitStructure);	
		
		/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = IN2_GPIO_PIN;
		/*调用库函数，初始化GPIO*/
		GPIO_Init(IN2_GPIO_PORT, &GPIO_InitStructure);
		
		/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = IN3_GPIO_PIN;
		/*调用库函数，初始化GPIOF*/
		GPIO_Init(IN3_GPIO_PORT, &GPIO_InitStructure);
		
		/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = IN4_GPIO_PIN;
		/*调用库函数，初始化GPIOF*/
		GPIO_Init(IN4_GPIO_PORT, &GPIO_InitStructure);
		
		//所有引脚初始化为低电平
    IN1_LOW;
    IN2_LOW;
    IN3_LOW;
    IN4_LOW;		

}



void stepper(uint8_t dir,int speed)
{
	if(dir == Pos)
	{
		IN1_HIGH;
		delay_ms(speed);
		IN1_LOW;
		IN2_HIGH;
		delay_ms(speed);
		IN2_LOW;
		IN3_HIGH;
		delay_ms(speed);
		IN3_LOW;
		IN4_HIGH;
		delay_ms(speed);
		IN4_LOW;
	}
	else
	{
		IN1_HIGH;
		delay_ms(speed);
		IN1_LOW;
		IN4_HIGH;
		delay_ms(speed);
		IN4_LOW;
		IN3_HIGH;
		delay_ms(speed);
		IN3_LOW;
		IN2_HIGH;
		delay_ms(speed);
		IN2_LOW;
	}
}


void MotorCWRound(u8 n)  //顺时针转圈
{
	u8 i,j,k;
	
	LCD_ShowString(114,70,240,24,24,"^");
	
	for(k=0;k<n;k++)
	{
		for(j=0;j<64;j++) ////减速比64，码盘旋转1周
			{
				for(i=0;i<8;i++)//电机内齿轮转360度
				{
					stepper(Pos,5);    //顺时针转 
				}
			}
	}
}

void MotorCCWRound(u8 n)  //逆时针转圈
{
	u8 i,j,k;
	
	LCD_ShowString(114,70,240,24,24,"v");
	
	for(k=0;k<n;k++)
	{
		for(j=0;j<64;j++) ////减速比64，码盘旋转1周
			{
				for(i=0;i<8;i++)//电机内齿轮转360度
				{
					stepper(Neg,5);    //逆时针转 
				}
			}
	}

}


void MotorStop(void) //停止转动
{
	IN1_LOW;
	IN2_LOW;
	IN3_LOW;
	IN4_LOW;

}








 
