#include"lcd12864.h"

extern uchar code CharCodeLine1[];       //第一行显示字符
extern uchar code CharCodeLine2[];         //第二行显示字符

void Roll(void){  //卷动显示两行字	
	uchar i;	
	//为显示上半屏第一行字符做准备，地址0xa0  
	//详细参考文章：https://wenku.baidu.com/view/375ec764cc22bcd127ff0c9a.html	
	LCD12864_WriteCmd(0xa0);	
	while(CharCodeLine1[i]!='\0'){
		LCD12864_WriteData(CharCodeLine1[i]);
		i++;
	}
	i=0;
	//为显示上半屏第二行字符做准备，地址0xb0  	
	LCD12864_WriteCmd(0xb0);    
	while(CharCodeLine2[i]!='\0')
	{
		LCD12864_WriteData(CharCodeLine2[i]);
		i++;
	}
	
		for(i=0;i<33;i++)    //上半屏卷动显示
	{
			LCD12864_VerticalRoll(i);
			Delay100ms();	 //每行高16个像素，两行32像素，0.1秒卷动1像素，两行字显示结束共需要3.2秒
	}
	LCD12864_WriteCmd(0x30); //恢复基本指令集
}

/*******************************************************************************
* 函 数 名         : LCD12864_Delay1ms
* 函数功能		   : 延时1MS
* 输    入         : c
* 输    出         : 无
*******************************************************************************/

void LCD12864_Delay1ms(uint c)
{
    uchar a,b;
	for(; c>0; c--)
	{
	    for(b=199; b>0; b--)
		{
	        for(a=1; a>0; a--);
		}
	}
}

/*******************************************************************************
* 函 数 名         : LCD12864_WriteCmd
* 函数功能		   : 写命令
* 输    入         : cmd
* 输    出         : 无
*******************************************************************************/

void LCD12864_WriteCmd(uchar cmd)
{
	uchar i;
	i = 0;
	LCD12864_Delay1ms(20); //等待忙完
	LCD12864_RS = 0;     //选择命令
	LCD12864_RW = 0;     //选择写入
	LCD12864_EN = 0;     //初始化使能端
	LCD12864_DATAPORT = cmd;   //放置数据
	LCD12864_EN = 1;		   //写时序
	LCD12864_Delay1ms(5);
	LCD12864_EN = 0;    					
}

/*******************************************************************************
* 函 数 名         : LCD12864_WriteData
* 函数功能		   : 写数据
* 输    入         : dat
* 输    出         : 无
*******************************************************************************/

void LCD12864_WriteData(uchar dat)
{
	uchar i;
	i = 0;
	LCD12864_Delay1ms(20);  //等待忙完

	LCD12864_RS = 1;     //选择数据
	LCD12864_RW = 0;     //选择写入
	LCD12864_EN = 0;     //初始化使能端

	LCD12864_DATAPORT = dat;   //放置数据

	LCD12864_EN = 1;		   //写时序
	LCD12864_Delay1ms(5);
	LCD12864_EN = 0;    								
}

/*******************************************************************************
* 函 数 名         : LCD12864_ReadData
* 函数功能		   : 读取数据
* 输    入         : 无
* 输    出         : 读取到的8位数据
*******************************************************************************/
uchar LCD12864_ReadData(void)
{
	uchar i, readValue;

	i = 0;
	
	LCD12864_Delay1ms(20);  //等待忙完

	LCD12864_RS = 1;       //选择命令
	LCD12864_RW = 1;
	LCD12864_EN = 0;
	LCD12864_Delay1ms(1);  //等待

	LCD12864_EN = 1;
	LCD12864_Delay1ms(1);
	readValue = LCD12864_DATAPORT;
	LCD12864_EN = 0;

	return readValue;
}


/*******************************************************************************
* 函 数 名         : LCD12864_Init
* 函数功能		       : 初始化LCD12864
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void LCD12864_Init()
{

	LCD12864_WriteCmd(0x30);  //选择基本指令操作
	LCD12864_WriteCmd(0x0c);  //显示开，关光标
	LCD12864_WriteCmd(0x01);  //清除LCD12864的显示内容
}

/*******************************************************************************
* 函 数 名         : LCD12864_ClearScreen
* 函数功能		   :    在画图模式下，LCD12864的01H命令不能清屏，所以要自己写一个清
*                  * 屏函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void LCD12864_ClearScreen(void)
{
	uchar i,j;

	LCD12864_WriteCmd(0x34);	  //开启拓展指令集

	for(i=0;i<32;i++)			  //因为LCD有纵坐标32格所以写三十二次
	{
		LCD12864_WriteCmd(0x80+i);		  //先写入纵坐标Y的值
		LCD12864_WriteCmd(0x80);		  //再写入横坐标X的值
		for(j=0;j<32;j++)		  //横坐标有16位，每位写入两个字节的的数据，也
		{						  //就写入32次以为当写入两个字节之后横坐标会自
			LCD12864_WriteData(0xFF);	  //动加1，所以就不用再次写入地址了。
		}
	}

	LCD12864_WriteCmd(0x36);	 //0x36扩展指令里面打开绘图显示
	LCD12864_WriteCmd(0x30);	 //恢复基本指令集
}

/*******************************************************************************
* 函 数 名         : LCD12864_SetWindow
* 函数功能		   : 设置在基本指令模式下设置显示坐标。注意：x是设置行，y是设置列
* 输    入         : x, y
* 输    出         : 无
*******************************************************************************/
void LCD12864_SetWindow(uchar x, uchar y)
{
	uchar pos;

	if(x == 0)	   // 第一行的地址是80H
	{
		x = 0x80;
	}
		
	else if(x == 1)  //第二行的地址是90H
	{
		x = 0x90;	
	}
	
	else if(x == 2)  //第三行的地址是88H
	{
		x = 0x88;
	}
	
	else if(x == 3)
	{
		x = 0x98;
	}
	
	pos = x + y;
	LCD12864_WriteCmd(pos);
}


/*-----------------------------------------------------------------------------------------
函数原型：void LCD12864_VerticalRoll(uchar N)
函数功能：将DDRAM内容垂直卷动N个像素的距离
入口参数：uchar N_Pixel：卷动的距离大小（单位为像素），范围为0~33（实际上可以是0~63，但33~
                         63实际意义不大）
出口参数：无
返回参数：无
注意事项：将N_Pixel设为33可将DDRAM地址0xa0~0xbf的内容完全切换到屏幕上，N为0则显示DDRAM地址
          0x80~0x9f的内容（复位默认状态）
-----------------------------------------------------------------------------------------*/
void LCD12864_VerticalRoll(uchar N_Pixel)
{
    LCD12864_WriteCmd(0x34);//允许绘图模式（开启扩展指令集模式）
    LCD12864_WriteCmd(0x03);//允许输入卷动位址
    LCD12864_WriteCmd(0x40|N_Pixel);//上卷N行（像素）
}

/*--------------------------------------------------------------*/
//液晶定位写入数据一个字节
//液晶规划:
//x: 0 - 7	(页)
//y: 0 -127	(列)
void LCD_write_dat_pos(unsigned char x, unsigned char y, unsigned char dat)
{
	LCD12864_SetWindow(x, y);
	LCD12864_WriteCmd(dat);
}

/*--------------------------------------------------------------*/
//液晶定位读出数据一个字节
//液晶规划:
//x: 0 - 7	(页)
//y: 0 -127	(列)
unsigned char LCD_read_dat_pos(unsigned char x, unsigned char y)
{
	unsigned char read_dat;

	LCD12864_SetWindow(x, y);
	read_dat = LCD12864_ReadData();		//dummy读
	read_dat = LCD12864_ReadData();		//数据有效

	return (read_dat);	
}

/*--------------------------------------------------------------*/
//画点函数
//x:	0 - 127		横坐标
//y:	0 - 63		列坐标	
//attr:		=1,		画点	
//			=0,		消点
void LCD12864_pixel(unsigned char x, unsigned char y, unsigned char attr)
{
	unsigned char pixel_dat, temp_dat, yy;
	
	yy = y >> 3;						//划分到页
	pixel_dat = LCD_read_dat_pos(yy, x);//读出数据
	temp_dat = 0x01 << (y & 0x07);		//页中的点
	if(attr) pixel_dat |= temp_dat;		//画点
	else	 pixel_dat &= ~temp_dat;	//消点
	LCD_write_dat_pos(yy, x, pixel_dat);//写入组合数据
}


/*--------------------------------------------------------------*/
//画横线
//y:	0 - 63		列坐标	
//attr:		=1,		画线
//			=0,		消线
void LCD_line_h(unsigned char y, unsigned char attr)
{
	unsigned char i;

	for(i = 0; i < 128; i++) LCD12864_pixel(i, y, attr);
}
