	
实验目的:
	学习摄像头的使用
	
硬件资源:

	1,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)
	2,实验平台自带3.5 寸TFTLCD模块(通过FSMC驱动)
	3,SD卡,通过SDIO连接 
	4,外部中断8(PA8,用于检测OV7670的帧信号)
	5,  OV7670摄像头模块,连接关系为:
		OV7670模块 ----------- STM32实验平台
		 OV_D0~D7  -----------   PC0~7 
		 OV_SCL    ------------  PD3 
		 OV_SDA    ------------  PD6 
		 OV_VSYNC  ------------  PA8   
		 FIFO_RRST -----------   PG14 
		 FIFO_OE   -----------   PG15
		 FIFO_WRST ------------  PG13
		 FIFO_WEN  ------------  PB3  
		 FIFO_RCLK ------------  PB4 
		
实验现象:
	本实验开机的时候先初始化OV7670，在初始化成功之后，就一直在屏幕显示OV7670拍到的内容。 

注意事项:
	1,4.3寸和7寸屏需要比较大电流,请用外部电源适配器(推荐外接12V 2A电源).
	2,本例程在LCD_Init函数里面(在ILI93xx.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!! 

参考资料：
        手册：《STM32实验指导书》。

 基于ALIENTEK例程完成	
-------------------------------------------------------------------------------------------
