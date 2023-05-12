#include "recorder.h"
#include "calendar.h"
#include "audioplay.h"
#include "vs10xx.h" 
#include "spi.h"
#include "settings.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//APP-录音机 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 

#define RECORDER_TITLE_COLOR   	0XFFFF		//录音机标题颜色	
#define RECORDER_TITLE_BKCOLOR	0X0000		//录音机标题背景色	

#define RECORDER_VU_BKCOLOR    	0X39C7		//VU Meter背景色	
#define RECORDER_VU_L1COLOR    	0X07FF		//VU Meter L1色	
#define RECORDER_VU_L2COLOR    	0xFFE0		//VU Meter L2色	
#define RECORDER_VU_L3COLOR    	0xF800		//VU Meter L3色	

#define RECORDER_TIME_COLOR    	0X07FF		//时间颜色
#define RECORDER_MAIN_BKCOLOR	0X18E3		//主背景色

//窗体内嵌字颜色
#define RECORDER_INWIN_FONT_COLOR		0X736C		//0XAD53		


u8*const RECORDER_DEMO_PIC="1:/SYSTEM/APP/RECORDER/Demo.bmp";		//demo图片路径 	      
u8*const RECORDER_RECR_PIC="1:/SYSTEM/APP/RECORDER/RecR.bmp";		//录音 松开
u8*const RECORDER_RECP_PIC="1:/SYSTEM/APP/RECORDER/RecP.bmp";		//录音 按下
u8*const RECORDER_PAUSER_PIC="1:/SYSTEM/APP/RECORDER/PauseR.bmp";	//暂停 松开
u8*const RECORDER_PAUSEP_PIC="1:/SYSTEM/APP/RECORDER/PauseP.bmp";	//暂停 按下
u8*const RECORDER_STOPR_PIC="1:/SYSTEM/APP/RECORDER/StopR.bmp";		//停止 松开
u8*const RECORDER_STOPP_PIC="1:/SYSTEM/APP/RECORDER/StopP.bmp";		//停止 按下    
//麦克风增益设置
const u8* recoder_set_tbl[GUI_LANGUAGE_NUM]={"麦克风增益设置","麥克風增益設置","MIC GAIN SET"};	   
//录音提示信息
u8*const recorder_remind_tbl[3][GUI_LANGUAGE_NUM]=
{
"是否保存该录音文件?","是否保存該錄音文件?","Do you want to save?", 
{"请先停止录音!","請先停止錄音!","Please stop REC first!",},	  
{"内存不够!!","內存不夠!!","Out of memory!",},	 	 
};	
//00级功能选项表标题
u8*const recorder_modesel_tbl[GUI_LANGUAGE_NUM]=
{
"录音设置","錄音設置","Recorder Set",
};	

//VS1053的WAV录音有bug,这个plugin可以修正这个问题 							    
const u16 wav_plugin[40]=/* Compressed plugin */ 
{ 
0x0007, 0x0001, 0x8010, 0x0006, 0x001c, 0x3e12, 0xb817, 0x3e14, /* 0 */ 
0xf812, 0x3e01, 0xb811, 0x0007, 0x9717, 0x0020, 0xffd2, 0x0030, /* 8 */ 
0x11d1, 0x3111, 0x8024, 0x3704, 0xc024, 0x3b81, 0x8024, 0x3101, /* 10 */ 
0x8024, 0x3b81, 0x8024, 0x3f04, 0xc024, 0x2808, 0x4800, 0x36f1, /* 18 */ 
0x9811, 0x0007, 0x0001, 0x8028, 0x0006, 0x0002, 0x2a00, 0x040e,  
}; 
//进入ADPCM 录音模式
//agc:0,自动增益.1024相当于1倍,512相当于0.5倍,最大值65535=64倍		  
void recorder_enter_rec_mode(void)
{
	//如果是IMA ADPCM,采样率计算公式如下:
 	//采样率=CLKI/256*d;	
	//假设d=0,并2倍频,外部晶振为12.288M.那么Fc=(2*12288000)/256*6=16Khz
	//如果是线性PCM,采样率直接就写采样值 
   	VS_WR_Cmd(SPI_BASS,0x0000);     	
	VS_WR_Cmd(SPI_AICTRL0,8000);	//设置采样率,设置为8Khz	
	VS_WR_Cmd(SPI_AICTRL2,0);		//设置增益最大值,0,代表最大值65536=64X
 	VS_WR_Cmd(SPI_AICTRL3,6);		//左通道(MIC单声道输入)
	VS_WR_Cmd(SPI_CLOCKF,0X2000);	//设置VS10XX的时钟,MULT:2倍频;ADD:不允许;CLK:12.288Mhz
	VS_WR_Cmd(SPI_MODE,0x1804);		//MIC,录音激活    
 	delay_ms(5);					//等待至少1.35ms 
 	VS_Load_Patch((u16*)wav_plugin,40);//VS1053的WAV录音需要plugin
}  
//设置VS1053的AGC
//agc:0,自动增益.1~64,表示1~64倍		  
void recoder_set_agc(u16 agc)
{
	if(agc>=64)agc=65535;
	else agc*=1024;
	VS_WR_Cmd(SPI_AICTRL1,agc);	//设置增益 	
}
//初始化WAV头.
//wavhead:wav文件头结构体
void recorder_wav_init(__WaveHeader* wavhead) 	   
{
	wavhead->riff.ChunkID=0X46464952;	//"RIFF"
	wavhead->riff.ChunkSize=0;			//还未确定,最后需要计算
	wavhead->riff.Format=0X45564157; 	//"WAVE"
	wavhead->fmt.ChunkID=0X20746D66; 	//"fmt "
	wavhead->fmt.ChunkSize=16; 			//大小为16个字节
	wavhead->fmt.AudioFormat=0X01; 		//0X01,表示PCM;0X01,表示IMA ADPCM
 	wavhead->fmt.NumOfChannels=1;		//单声道
 	wavhead->fmt.SampleRate=8000;		//采样率,单位:Hz,
 	wavhead->fmt.ByteRate=wavhead->fmt.SampleRate*2;//字节速率=采样率*通道数*(ADC位数/8),后续确定
 	wavhead->fmt.BlockAlign=2;			//块大小=通道数*(ADC位数/8)
 	wavhead->fmt.BitsPerSample=16;		//16位PCM
   	wavhead->data.ChunkID=0X61746164;	//"data"
 	wavhead->data.ChunkSize=0;			//数据大小,还需要计算  
} 
//读VS10XX的数据         
//address：寄存器地址
//返回值：读到的值
//注意不要用倍速读取,会出错
u16 recorder_read_data(u8 address)
{ 
	u16 temp=0;    	 
    while(VS_DQ==0);//非等待空闲状态 	
	VS_XDCS=1;       
	VS_XCS=0;        
	VS_SPI_ReadWriteByte(VS_READ_COMMAND);	//发送VS10XX的读命令
	VS_SPI_ReadWriteByte(address);       	//地址
	temp=VS_SPI_ReadWriteByte(0xff); 		//读取高字节
	temp=temp<<8;
	temp+=VS_SPI_ReadWriteByte(0xff); 		//读取低字节
	VS_XCS=1;     
   return temp; 
}   
//电平阀值表
const u16 vu_val_tbl[10]={3000,4500,6500,9000,11000,14000,18000,22000,27000,32000};
//从信号电平得到vu表征值
//signallevel:信号电平
//返回值:vu值
u8 recorder_vu_get(u16 signallevel)
{
	u8 i;
	for(i=10;i>0;i--)
	{
		if(signallevel>=vu_val_tbl[i-1])break;
	}
	return i; 
}
//显示VU Meter
//level:0~10;
//x,y:坐标
void recorder_vu_meter(u16 x,u16 y,u8 level)
{
	u8 i;
	u16 vucolor=RECORDER_VU_L1COLOR;
 	if(level>10)return ;
	if(level==0)
	{
		gui_fill_rectangle(x,y,218,10,RECORDER_VU_BKCOLOR);	//填充背景色
		return;
	}   
	for(i=0;i<level;i++)
	{
		if(i==9)vucolor=RECORDER_VU_L3COLOR;
		else if(i>5)vucolor=RECORDER_VU_L2COLOR;
		gui_fill_rectangle(x+22*i,y,20,10,vucolor);	//填充背景色
	}
	if(level<10)gui_fill_rectangle(x+level*22,y,218-level*22,10,RECORDER_VU_BKCOLOR);	//填充背景色	 
}   
//显示录音时长
//显示尺寸为:150*60
//x,y:地址
//tsec:秒钟数.
void recorder_show_time(u16 x,u16 y,u32 tsec)
{
	u8 min;
	if(tsec>=60*100)min=99;
	else min=tsec/60;	 
	gui_phy.back_color=RECORDER_MAIN_BKCOLOR;
	gui_show_num(x,y,2,RECORDER_TIME_COLOR,60,min,0X80);	//XX					   
	gui_show_ptchar(x+60,y,lcddev.width,lcddev.height,0,RECORDER_TIME_COLOR,60,':',0);	//":" 
	gui_show_num(x+90,y,2,RECORDER_TIME_COLOR,60,tsec%60,0X80);	//XX	    	    
} 
//显示名字
//x,y:坐标(不要从0开始)
//name:名字
void recorder_show_name(u16 x,u16 y,u8 *name)
{
 	gui_fill_rectangle(x-1,y-1,lcddev.width,13,RECORDER_MAIN_BKCOLOR);	//填充背景色
 	gui_show_ptstrwhiterim(x,y,lcddev.width,y+12,0,BLACK,WHITE,12,name); 	  
}
//显示采样率
//x,y:坐标(不要从0开始)
//samplerate:采样率
void recorder_show_samplerate(u16 x,u16 y,u16 samplerate)
{
	u8 *buf=0;
	float temp;
	temp=(float)samplerate/1000; 
	buf=gui_memin_malloc(60);//申请内存
	if(buf==0)return;
	if(samplerate%1000)sprintf((char*)buf,"%.1fKHz",temp);//有小数点
	else sprintf((char*)buf,"%dKHz",samplerate/1000);  
 	gui_fill_rectangle(x,y,42,12,RECORDER_MAIN_BKCOLOR);//填充背景色
	gui_show_string(buf,x,y,42,12,12,RECORDER_INWIN_FONT_COLOR);//显示agc  	  
	gui_memin_free(buf);//释放内存 
}
//加载录音机主界面UI	  
void recorder_load_ui(void)
{	
 	gui_fill_rectangle(0,0,lcddev.width,gui_phy.tbheight,RECORDER_TITLE_BKCOLOR);				//填充背景色
	gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,RECORDER_TITLE_COLOR,gui_phy.tbfsize,(u8*)APP_MFUNS_CAPTION_TBL[11][gui_phy.language]);//显示标题
 	gui_fill_rectangle(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight,RECORDER_MAIN_BKCOLOR);//填充底色 
 	minibmp_decode((u8*)RECORDER_DEMO_PIC,(lcddev.width-100)/2,100+(lcddev.height-320)/2,100,100,0,0);//解码100*100的图片DEMO
	recorder_vu_meter((lcddev.width-218)/2,(lcddev.height-320)/2+200+5,0);				//显示vu meter;
	app_gui_tcbar(0,lcddev.height-gui_phy.tbheight,lcddev.width,gui_phy.tbheight,0x01);	//上分界线
}

//通过时间获取文件名
//仅限在SD卡保存,不支持FLASH DISK保存
//组合成:形如"0:RECORDER/REC20120321210633.wav"的文件名
void recorder_new_pathname(u8 *pname)
{	  
	RTC_Get();
	if(gui_phy.memdevflag&(1<<0))sprintf((char*)pname,"0:RECORDER/REC%04d%02d%02d%02d%02d%02d.wav",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);		//首选保存在SD卡
}
//显示AGC大小
//x,y:坐标
//agc:增益值 0~15,0,自动增益;1~15,放大倍数
void recorder_show_agc(u16 x,u16 y,u8 agc)
{ 
	gui_phy.back_color=APP_WIN_BACK_COLOR;					//设置背景色为底色
	gui_fill_rectangle(x,y,24,12,APP_WIN_BACK_COLOR);		//填充背景色 
	if(agc==0)
	{
   		gui_show_string("AUTO",x,y,24,12,12,RECORDER_INWIN_FONT_COLOR);//显示agc
	}else
	{
		gui_show_num(x,y,2,RECORDER_INWIN_FONT_COLOR,12,agc,0X80);	//显示数值	
	}
}	
//agc设置界面.固定尺寸:180*122    
//x,y:左上角坐标
//agc:自动增益指针,范围:0~15,0,自动增益;1~15对应1~15倍
//caption:窗口名字				  
//返回值:0,成功设置;
//    其他,不进行设置
u8 recorder_agc_set(u16 x,u16 y,u8 *agc,u8*caption)
{
 	u8 rval=0,res;
   	_window_obj* twin=0;			//窗体
 	_btn_obj * rbtn=0;				//取消按钮		  
 	_btn_obj * okbtn=0;				//确定按钮		  
	_progressbar_obj * agcprgb;		//AGC设置进度条
 	u8 tempagc=*agc;

  	twin=window_creat(x,y,180,122,0,1|1<<5,16);			//创建窗口 　
 	agcprgb=progressbar_creat(x+10,y+52,160,15,0X20);	//创建进度条
	if(agcprgb==NULL)rval=1;
  	okbtn=btn_creat(x+20,y+82,60,30,0,0x02);		//创建按钮
 	rbtn=btn_creat(x+20+60+20,y+82,60,30,0,0x02);	//创建按钮
	if(twin==NULL||rbtn==NULL||okbtn==NULL||rval)rval=1;
	else
	{
		//窗口的名字和背景色
		twin->caption=caption;
		twin->windowbkc=APP_WIN_BACK_COLOR;	
		//返回按钮的颜色    
		rbtn->bkctbl[0]=0X8452;		//边框颜色
		rbtn->bkctbl[1]=0XAD97;		//第一行的颜色				
		rbtn->bkctbl[2]=0XAD97;		//上半部分颜色
		rbtn->bkctbl[3]=0X8452;		//下半部分颜色
		okbtn->bkctbl[0]=0X8452;	//边框颜色
		okbtn->bkctbl[1]=0XAD97;	//第一行的颜色				
		okbtn->bkctbl[2]=0XAD97;	//上半部分颜色
		okbtn->bkctbl[3]=0X8452;	//下半部分颜色 
		agcprgb->totallen=15;		//最大AGC为15
	   	agcprgb->curpos=tempagc;	//当前尺寸 
		rbtn->caption=(u8*)GUI_CANCEL_CAPTION_TBL[gui_phy.language];	//标题为取消
 		okbtn->caption=(u8*)GUI_OK_CAPTION_TBL[gui_phy.language];		//标题为确定
  		window_draw(twin);				//画出窗体
		btn_draw(rbtn);					//画按钮
		btn_draw(okbtn);				//画按钮
		progressbar_draw_progressbar(agcprgb);
   		gui_show_string("AGC:",x+10,y+38,24,12,12,RECORDER_INWIN_FONT_COLOR);//显示SIZE
		recorder_show_agc(x+10+24,y+38,tempagc);
 		while(rval==0)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
			delay_ms(1000/OS_TICKS_PER_SEC);		//延时一个时钟节拍
			if(system_task_return)break;			//TPAD返回
			res=btn_check(rbtn,&in_obj);			//取消按钮检测
			if(res&&((rbtn->sta&0X80)==0))			//有有效操作
			{
				rval=1;
				break;//退出
			}
			res=btn_check(okbtn,&in_obj); 			//确认按钮检测
			if(res&&((okbtn->sta&0X80)==0))			//有有效操作
			{
				rval=0XFF;		   
				break;//确认了
			}	  	  
			res=progressbar_check(agcprgb,&in_obj);
 			if(res&&(tempagc!=agcprgb->curpos))	//进度条改动了
			{
				tempagc=agcprgb->curpos;		//保存最新的结果  
 				recorder_show_agc(x+10+24,y+38,tempagc);
				recoder_set_agc(tempagc);		//设置增益 
			} 		
 		}
	}
	window_delete(twin);							//删除窗口
	btn_delete(rbtn);								//删除按钮	
	btn_delete(okbtn);								//删除按钮	
	progressbar_delete(agcprgb);//删除进度条  
	system_task_return=0;		 
	if(rval==0XFF)
	{
		*agc=tempagc;
		return 0;
 	}
	return rval;
}

//录音机
//所有录音文件,均保存在SD卡RECORDER文件夹内.
u8 recorder_play(void)
{
	u8 res;
	u8 rval=0;
	__WaveHeader *wavhead=0;
	FIL* f_rec=0;			//录音文件	 
	u32 sectorsize=0;
 	_btn_obj * rbtn=0;		//取消按钮		  
 	_btn_obj * mbtn=0;		//选项按钮	
 	_btn_obj * recbtn=0;	//录音按钮		  
 	_btn_obj * stopbtn=0;	//停止录音按钮  

 	u16 *pset_bkctbl=0;		//设置时背景色指针
	u32 recsec=0;			//录音时间
	
 	u8 *recbuf;				//数据内存
	u8 *pname=0; 	
	u16 w;
	u16 idx=0;
	
	u8 timecnt=0;
	u8 vulevel=0;
 	short tempval;
	u8 temp;
	u16 maxval=0;
	
	u8 rec_sta=0;			//录音状态
							//[7]:0,没有开启录音;1,已经开启录音;
							//[6:1]:保留
							//[0]:0,正在录音;1,暂停录音;  
  
	u8 recagc=4;			//默认增益为4倍 
	
  	f_rec=(FIL *)gui_memin_malloc(sizeof(FIL));			//开辟FIL字节的内存区域  
 	wavhead=(__WaveHeader*)gui_memin_malloc(sizeof(__WaveHeader));//开辟__WaveHeader字节的内存区域 
	pname=gui_memin_malloc(60);							//申请30个字节内存,类似"0:RECORDER/REC20120321210633.wav" 
	recbuf=gui_memin_malloc(512);						//申请512字节内存
	pset_bkctbl=gui_memex_malloc(180*272*2);			//为设置时的背景色表申请内存
	if(!recbuf||!f_rec||!wavhead||!pname||!pset_bkctbl)rval=1;
 	else
	{
		//加载字体
		res=f_open(f_rec,(const TCHAR*)APP_ASCII_S6030,FA_READ);//打开文件夹
		if(res==FR_OK)
		{
			asc2_s6030=(u8*)gui_memex_malloc(f_rec->fsize);	//为大字体开辟缓存地址
			if(asc2_s6030==0)rval=1;
			else 
			{
				res=f_read(f_rec,asc2_s6030,f_rec->fsize,(UINT*)&br);	//一次读取整个文件
 			}
		} 
		if(res)rval=1; 
		recorder_load_ui();			//装载主界面
		rbtn=btn_creat(lcddev.width-2*gui_phy.tbfsize-8-1,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建文字按钮
		mbtn=btn_creat(0,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建文字按钮
		recbtn=btn_creat((lcddev.width-96)/3,(lcddev.height-320)/2+215+18,48,48,0,1);	//创建图片按钮
		stopbtn=btn_creat((lcddev.width-96)*2/3+48,(lcddev.height-320)/2+215+18,48,48,0,1);//创建图片按钮
		if(!rbtn||!mbtn||!recbtn||!stopbtn)rval=1;//没有足够内存够分配	
		else
		{																				
			rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//返回 
			rbtn->font=gui_phy.tbfsize;//设置新的字体大小	 	 
			rbtn->bcfdcolor=WHITE;	//按下时的颜色
			rbtn->bcfucolor=WHITE;	//松开时的颜色
			
			mbtn->caption=(u8*)GUI_OPTION_CAPTION_TBL[gui_phy.language];//返回 
			mbtn->font=gui_phy.tbfsize;//设置新的字体大小		 
			mbtn->bcfdcolor=WHITE;	//按下时的颜色
			mbtn->bcfucolor=WHITE;	//松开时的颜色
			
			recbtn->picbtnpathu=(u8*)RECORDER_RECR_PIC;
			recbtn->picbtnpathd=(u8*)RECORDER_PAUSEP_PIC;
			recbtn->bcfucolor=0X0001;//不填充背景
			recbtn->bcfdcolor=0X0001;//不填充背景
			recbtn->sta=0;	 
			
			stopbtn->picbtnpathu=(u8*)RECORDER_STOPR_PIC;
			stopbtn->picbtnpathd=(u8*)RECORDER_STOPP_PIC;
			stopbtn->bcfucolor=0X0001;//不填充背景
			stopbtn->bcfdcolor=0X0001;//不填充背景
			recbtn->sta=0;	 
		} 
	}
	if(rval==0)
	{    
		if(gui_phy.memdevflag&(1<<0))f_mkdir("0:RECORDER");	//强制创建文件夹,给录音机用
 		btn_draw(rbtn);	
		btn_draw(mbtn);	
		btn_draw(recbtn);
		recbtn->picbtnpathu=(u8*)RECORDER_PAUSER_PIC;
		recbtn->picbtnpathd=(u8*)RECORDER_RECP_PIC;	  
		btn_draw(stopbtn);
		if(audiodev.status&(1<<7))		//当前在放歌??必须停止
		{
			audio_stop_req(&audiodev);	//停止音频播放
			audio_task_delete();		//删除音乐播放任务.
		}
		rec_sta=0;
		recsec=0; 
		recorder_enter_rec_mode();		//进入录音模式,并设置AGC
		recorder_show_samplerate((lcddev.width-218)/2,(lcddev.height-320)/2+200+5-15,8000);//显示采样率
		recoder_set_agc(recagc); 		//设置增益
   		while(VS_RD_Reg(SPI_HDAT1)>>8);	//等到buf 较为空闲再开始    		
		recorder_show_time((lcddev.width-150)/2,40+(lcddev.height-320)/2,recsec);//显示时间
 	   	while(rval==0)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值    
 			if(system_task_return)break;			//TPAD返回
			res=btn_check(rbtn,&in_obj); 			//检查返回按钮
			if(res&&((rbtn->sta&(1<<7))==0)&&(rbtn->sta&(1<<6)))break;//返回按钮
  			res=btn_check(mbtn,&in_obj); 			//检查设置按钮
			if(res&&((mbtn->sta&(1<<7))==0)&&(mbtn->sta&(1<<6)))
			{   
				app_read_bkcolor((lcddev.width-180)/2,(lcddev.height-272)/2,180,272,pset_bkctbl);//读取背景色
 				res=recorder_agc_set((lcddev.width-180)/2,(lcddev.height-122)/2,&recagc,(u8*)recoder_set_tbl[gui_phy.language]);//设置AGC
 				recoder_set_agc(recagc); 			//设置增益 
 				app_recover_bkcolor((lcddev.width-180)/2,(lcddev.height-272)/2,180,272,pset_bkctbl);//恢复背景色  				  
 			}
  			res=btn_check(recbtn,&in_obj); 			//检查录音按钮
			if(res&&((recbtn->sta&(1<<7))==0)&&(recbtn->sta&(1<<6)))
			{  
				if(rec_sta&0X01)//原来是暂停,继续录音
				{
					rec_sta&=0XFE;//取消暂停
					recbtn->picbtnpathu=(u8*)RECORDER_RECR_PIC;
					recbtn->picbtnpathd=(u8*)RECORDER_PAUSEP_PIC;
				}else if(rec_sta&0X80)//已经在录音了,暂停
				{
					rec_sta|=0X01;//暂停
					recbtn->picbtnpathu=(u8*)RECORDER_PAUSER_PIC;
					recbtn->picbtnpathd=(u8*)RECORDER_RECP_PIC;
				}else	//还没开始录音 
				{
 					rec_sta|=0X80;	//开始录音
					sectorsize=0;	//文件大小设置为0
					recbtn->picbtnpathu=(u8*)RECORDER_RECR_PIC;
					recbtn->picbtnpathd=(u8*)RECORDER_PAUSEP_PIC;	 	 
			 		pname[0]='\0';					//添加结束符
					recorder_new_pathname(pname);	//得到新的名字
					recorder_show_name(2,gui_phy.tbheight+4,pname);//显示名字
			 		recorder_wav_init(wavhead);		//初始化wav数据	
					res=f_open(f_rec,(const TCHAR*)pname, FA_CREATE_ALWAYS|FA_WRITE); 
					if(res)//文件创建失败
					{
						rec_sta=0;//创建文件失败,不能录音
						rval=0XFE;//提示是否存在SD卡
					}else res=f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//写入头数据
				}								  
			}
  			res=btn_check(stopbtn,&in_obj); 			//检查停止按钮
			if(res&&((recbtn->sta&(1<<7))==0)&&(recbtn->sta&(1<<6)))
			{
				if(rec_sta&0X80)//有录音
				{ 
					wavhead->riff.ChunkSize=sectorsize*512+36;	//整个文件的大小-8;
			   		wavhead->data.ChunkSize=sectorsize*512;		//数据大小
					f_lseek(f_rec,0);							//偏移到文件头.
			  		f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//写入头数据
					f_close(f_rec);
					sectorsize=0; 
					recbtn->picbtnpathu=(u8*)RECORDER_RECR_PIC;
					recbtn->picbtnpathd=(u8*)RECORDER_PAUSEP_PIC;
					btn_draw(recbtn);	
					recbtn->picbtnpathu=(u8*)RECORDER_PAUSER_PIC;
					recbtn->picbtnpathd=(u8*)RECORDER_RECP_PIC;	  
				}
				rec_sta=0;
				recsec=0;
				recorder_show_name(2,gui_phy.tbheight+4,"");				//显示名字
				recorder_show_time((lcddev.width-150)/2,40+(lcddev.height-320)/2,recsec);//显示时间
  			}
///////////////////////////////////////////////////////////
//读取数据			
			SPI1_SetSpeed(SPI_BaudRatePrescaler_16);		//设置到低速模式,4.5Mhz  
	  		w=recorder_read_data(SPI_HDAT1);	
			if((w>=256)&&(w<896))
			{
 				idx=0;				   	 
	  			while(idx<512) 	//一次读取512字节
				{	 
		 			w=recorder_read_data(SPI_HDAT0);
					tempval=(short)w;
					if(tempval<0)tempval=-tempval;
					if(maxval<tempval)maxval=tempval;	    
	 				recbuf[idx++]=w&0XFF;
					recbuf[idx++]=w>>8;
				}	 				      
				timecnt++;
				if((timecnt%2)==0)
				{	 
					temp=recorder_vu_get(maxval);
					if(temp>vulevel)vulevel=temp;
					else if(vulevel)vulevel--;
					recorder_vu_meter((lcddev.width-218)/2,(lcddev.height-320)/2+200+5,vulevel);//显示vu meter;
					maxval=0;							  
	 			}  
				if(rec_sta==0X80)//已经在录音了
				{
	 				res=f_write(f_rec,recbuf,512,&bw);//写入文件
					if(res)
					{
						printf("err:%d\r\n",res);
						printf("bw:%d\r\n",bw);
						break;//写入出错.	  
					}
					sectorsize++;//扇区数增加1,约为32ms	 
				}			
			}else delay_ms(1000/OS_TICKS_PER_SEC);					//延时一个时钟节拍
///////////////////////////////////////////////////////////// 
 			if(recsec!=((sectorsize*512)/wavhead->fmt.ByteRate))	//录音时间显示
			{
				recsec=(sectorsize*512)/wavhead->fmt.ByteRate;		//录音时间
				recorder_show_time((lcddev.width-150)/2,40+(lcddev.height-320)/2,recsec);//显示时间
			}
		}	 	
	}else
	{
		window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)recorder_remind_tbl[2][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//内存错误
		delay_ms(2000);
	}	
	if(rval==0XFE)				//创建文件失败了,需要提示是否存在SD卡
	{
		window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)APP_CREAT_ERR_MSG_TBL[gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//提示SD卡是否存在
		delay_ms(2000);			//等待2秒钟
 	}
	if(rec_sta&0X80)			//如果正在录音,则提示保存这个录音文件
	{
 		res=window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,"",(u8*)recorder_remind_tbl[0][gui_phy.language],12,0,0X03,0);
		if(res==1)//需要保存
		{ 
			wavhead->riff.ChunkSize=sectorsize*512+36;	//整个文件的大小-8;
	   		wavhead->data.ChunkSize=sectorsize*512;		//数据大小
			f_lseek(f_rec,0);							//偏移到文件头.
	  		f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//写入头数据
			f_close(f_rec);
		}
	} 
	VS_HD_Reset();	//硬复位VS1003
	//释放内存
 	gui_memin_free(recbuf);
 	gui_memin_free(f_rec);
	gui_memin_free(wavhead);  
	gui_memin_free(pname);  
	gui_memex_free(pset_bkctbl);
	gui_memex_free(asc2_s6030);
	asc2_s6030=0;				//清零
	btn_delete(rbtn);	 
	btn_delete(mbtn);	 
	btn_delete(recbtn);	 
	btn_delete(stopbtn);	 
	return rval;
}
			   			   


































