
#include "sys.h"
#include "lcd.h"

//LCD的画笔颜色和背景色	   
u16 POINT_COLOR=0x0000;	//画笔颜色
u16 BACK_COLOR=0xFFFF;  //背景色 
//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

void LCD_write(u8 HVAL,u8 LVAL)
{
	LCD_CS = 0;
	LCD_WR = 0;
	LCD_DataPortH = HVAL;
	LCD_DataPortL = LVAL;
	LCD_WR = 1;
	LCD_CS = 1;
}

u16 LCD_read(void)
{
	u16 d;
	LCD_CS = 0;
	LCD_RD = 0;
	delay_us(1); //delay 1 us
	d = LCD_DataPortH;
	d = (d<<8)|LCD_DataPortL;
	LCD_RD = 1;
  LCD_CS = 1;
	return d;
}
/*****************************************************************************
 * @name       :void LCD_WR_REG(u16 Reg)	
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_REG(u16 Reg)	 
{	
	LCD_RS=0;
	LCD_write((Reg>>8)&0xFF,Reg&0xFF);
} 

/*****************************************************************************
 * @name       :void LCD_WR_DATA(u16 Data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_DATA(u16 Data)
{
	LCD_RS=1;
	LCD_write((Data>>8)&0xFF,Data&0xFF);
}

/*****************************************************************************
 * @name       :u16 LCD_RD_DATA(void)
 * @date       :2018-11-13 
 * @function   :Read an 16-bit value from the LCD screen
 * @parameters :None
 * @retvalue   :read value
******************************************************************************/
u16 LCD_RD_DATA(void)
{
	LCD_RS=1;
	return LCD_read();
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA_16Bit(u16 Data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :Data:Data to be written
 * @retvalue   :None
******************************************************************************/	 
void LCD_WR_DATA_16Bit(u16 Data)
{
	LCD_RS=1;
	#if LCD_USE8BIT_MODEL
	LCD_write(0,(Data>>8)&0xFF);
	LCD_write(0,Data&0xFF);
	#else
	LCD_write((Data>>8)&0xFF,Data&0xFF);
	#endif
}

u16 Color_To_565(u8 r, u8 g, u8 b)
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

/*****************************************************************************
 * @name       :u16 Lcd_ReadData_16Bit(void)
 * @date       :2018-11-13 
 * @function   :Read an 16-bit value from the LCD screen
 * @parameters :None
 * @retvalue   :read value
******************************************************************************/	
u16 Lcd_RD_DATA_16Bit(void)
{
	u16 r,g,b;
	LCD_RS = 1;
	LCD_CS = 0;
	
	//dummy Data
	LCD_RD = 0;
	delay_us(1); //delay 1us
	r = LCD_DataPortH;
	r = (r<<8)|LCD_DataPortL;
	LCD_RD = 1;

	//8bit:red Data
	//16bit:red and green Data
	LCD_RD = 0;
	delay_us(1); //delay 1us
	r = LCD_DataPortH;
	r = (r<<8)|LCD_DataPortL;
	LCD_RD = 1;
	
	//8bit:green Data
	//16bit:blue Data
	LCD_RD = 0;
	delay_us(1); //delay 1us
	g = LCD_DataPortH;
	g = (g<<8)|LCD_DataPortL;
	LCD_RD = 1;
	
	#if LCD_USE8BIT_MODEL	
	LCD_RD = 0;
	delay_us(1); //delay 1us
	b = LCD_DataPortH;
	b = (b<<8)|LCD_DataPortL;
	LCD_RD = 1;
	#else
	b = g>>8;
	g = r&0xFF;
	r = r>>8;
	#endif
	LCD_CS = 1;
	return Color_To_565(r, g, b);
}

/*****************************************************************************
 * @name       :void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
 * @date       :2018-08-09 
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{
  LCD_WR_REG(LCD_Reg);
	LCD_WR_DATA(LCD_RegValue);
}

/*****************************************************************************
 * @name       :u16 LCD_ReadReg(u16 LCD_Reg)
 * @date       :2018-11-13 
 * @function   :read value from specially registers
 * @parameters :LCD_Reg:Register address
 * @retvalue   :read value
******************************************************************************/
void LCD_ReadReg(u16 LCD_Reg,u8 *Rval,int n)
{
	LCD_WR_REG(LCD_Reg);
	LCD_DataPortH = 0xFF; //拉高P2
	LCD_DataPortL = 0xFF; //拉高P0
	while(n--)
	{		
		*(Rval++) = LCD_RD_DATA();
	}
	LCD_DataPortH = 0xFF; //拉高P2
	LCD_DataPortL = 0xFF; //拉高P0
}

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_WriteRAM_Prepare(void)
{
 	LCD_WR_REG(lcddev.wramcmd);	  
}

/*****************************************************************************
 * @name       :void LCD_ReadRAM_Prepare(void)
 * @date       :2018-11-13 
 * @function   :Read GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 
void LCD_ReadRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.rramcmd);
}

/*****************************************************************************
 * @name       :void LCD_Clear(u16 Color)
 * @date       :2018-08-09 
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
******************************************************************************/	
void LCD_Clear(u16 Color)
{
	u16 i,j;
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);	
  for(i=0;i<lcddev.width;i++)
	{
	  for (j=0;j<lcddev.height;j++)
	  {
		LCD_WR_DATA_16Bit(Color);
	  }
	}
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(u16 x,u16 y)
 * @date       :2018-08-09 
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/	
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetWindows(x,y,x,y);//设置光标位置 
	LCD_WR_DATA_16Bit(POINT_COLOR); 	    
} 	 


/*****************************************************************************
 * @name       :u16 LCD_ReadPoint(u16 x,u16 y)
 * @date       :2018-11-13 
 * @function   :Read a pixel color value at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :the read color value
******************************************************************************/	
u16 LCD_ReadPoint(u16 x,u16 y)
{
	u16 color;
	if(x>=lcddev.width||y>=lcddev.height)
	{
		return 0;	//超过了范围,直接返回	
	}
	LCD_SetCursor(x,y);//设置光标位置 
	LCD_ReadRAM_Prepare();
	LCD_DataPortH = 0xFF; //拉高P2
	LCD_DataPortL = 0xFF; //拉高P0
	
	color = Lcd_RD_DATA_16Bit();
	
	LCD_DataPortH = 0xFF; //拉高P2
	LCD_DataPortL = 0xFF; //拉高P0
	return color;
}

/*****************************************************************************
 * @name       :void LCD_Set_GPIO(void)
 * @date       :2018-11-13 
 * @function   :Set the gpio to push-pull mode
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
/*
void LCD_Set_GPIO(void)
{
  //P0口设置成推挽输出
	p0M0=0xff;
	p0M1=0x00;

	//P1口设置成推挽输出
  p1M0=0xff;
	p1M1=0x00;

	//P2口设置成推挽输出
	P2M0=0xff;
	P2M1=0x00;
}
*/

/*****************************************************************************
 * @name       :void LCD_Reset(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_Reset(void)
{
	delay_ms(50);	
	LCD_RESET=0;
	delay_ms(50);
	LCD_RESET=1;
	delay_ms(50);
}

/*****************************************************************************
 * @name       :void LCD_Init(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 	 
void LCD_Init(void)
{
//	LCD_Set_GPIO();
	LCD_Reset(); //初始化之前复位
//************* ILI9488初始化**********//	
	LCD_WR_REG(0XF7);
	LCD_WR_DATA(0xA9);
	LCD_WR_DATA(0x51);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x82);

	LCD_WR_REG(0xC0);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x09);

	LCD_WR_REG(0xC1);
	LCD_WR_DATA(0x41);

	LCD_WR_REG(0XC5);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x80);

	LCD_WR_REG(0xB1);
	LCD_WR_DATA(0xB0);
	LCD_WR_DATA(0x11);

	LCD_WR_REG(0xB4);
	LCD_WR_DATA(0x02);

	LCD_WR_REG(0xB6);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x22);

	LCD_WR_REG(0xB7);
	LCD_WR_DATA(0xc6);

	LCD_WR_REG(0xBE);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x04);

	LCD_WR_REG(0xE9);
	LCD_WR_DATA(0x00);

	LCD_WR_REG(0x36);
	LCD_WR_DATA(0x08);

	LCD_WR_REG(0x3A);
	LCD_WR_DATA(0x55);

	LCD_WR_REG(0xE0);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x17);
	LCD_WR_DATA(0x0B);
	LCD_WR_DATA(0x41);
	LCD_WR_DATA(0x89);
	LCD_WR_DATA(0x4B);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x0E);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0x1B);
	LCD_WR_DATA(0x0F);

	LCD_WR_REG(0XE1);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x17);
	LCD_WR_DATA(0x1A);
	LCD_WR_DATA(0x04);
	LCD_WR_DATA(0x0E);
	LCD_WR_DATA(0x06);
	LCD_WR_DATA(0x2F);
	LCD_WR_DATA(0x45);
	LCD_WR_DATA(0x43);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x32);
	LCD_WR_DATA(0x36);
	LCD_WR_DATA(0x0F);

	LCD_WR_REG(0x11);
	delay_ms(120);
	LCD_WR_REG(0x29);

	//设置LCD属性参数
	LCD_direction(USE_HORIZONTAL);//设置LCD显示方向 
	//LCD_BL=1;//点亮背光	  51核心板tft屏幕背光常亮
}
 
/*****************************************************************************
 * @name       :void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
 * @date       :2018-08-09 
 * @function   :Setting LCD display window
 * @parameters :xStar:the bebinning x coordinate of the LCD display window
								yStar:the bebinning y coordinate of the LCD display window
								xEnd:the endning x coordinate of the LCD display window
								yEnd:the endning y coordinate of the LCD display window
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{	
	LCD_WR_REG(lcddev.setxcmd);	
	LCD_WR_DATA(xStar>>8);
	LCD_WR_DATA(0x00FF&xStar);		
	LCD_WR_DATA(xEnd>>8);
	LCD_WR_DATA(0x00FF&xEnd);

	LCD_WR_REG(lcddev.setycmd);	
	LCD_WR_DATA(yStar>>8);
	LCD_WR_DATA(0x00FF&yStar);		
	LCD_WR_DATA(yEnd>>8);
	LCD_WR_DATA(0x00FF&yEnd);	

	LCD_WriteRAM_Prepare();	//开始写入GRAM				
}   

/*****************************************************************************
 * @name       :void LCD_SetCursor(u16 Xpos, u16 Ypos)
 * @date       :2018-08-09 
 * @function   :Set coordinate value
 * @parameters :Xpos:the  x coordinate of the pixel
								Ypos:the  y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);	
} 

/*****************************************************************************
 * @name       :void LCD_direction(u8 direction)
 * @date       :2018-08-09 
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0-0 degree
                          1-90 degree
													2-180 degree
													3-270 degree
 * @retvalue   :None
******************************************************************************/ 
void LCD_direction(u8 direction)
{ 
			lcddev.setxcmd=0x2A;
			lcddev.setycmd=0x2B;
			lcddev.wramcmd=0x2C;
			lcddev.rramcmd=0x2E;
	switch(direction){		  
		case 0:		
			lcddev.dir=0;
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;		
			LCD_WriteReg(0x36,(1<<3));
		break;
		case 1:
			lcddev.dir=1;
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(1<<3)|(1<<6)|(1<<5));
		break;
		case 2:	
			lcddev.dir=2;
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;	
			LCD_WriteReg(0x36,(1<<3)|(1<<7)|(1<<6));
		break;
		case 3:
			lcddev.dir=3;
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(1<<3)|(1<<5)|(1<<7));
		break;	
		default:break;
	}		
	
}	 

/*****************************************************************************
 * @name       :u16 LCD_Read_ID(void)
 * @date       :2018-11-13 
 * @function   :Read ID
 * @parameters :None
 * @retvalue   :ID value
******************************************************************************/ 
u16 LCD_Read_ID(void)
{
	u8 val[4] = {0};
	LCD_ReadReg(0xD3,val,4);
	return (val[2]<<8)|val[3];
}



