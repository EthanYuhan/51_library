


#include "stc15f2k60s2.h"
#include "vs1053.h"
#include "sys.h"
#include "mp3player.h"




//音乐频谱分析补丁  必须加载此补丁才能正常显示音乐频谱
//code u16 VS1053_PATCH[] = {
//0x0007,0x0001,0x8d00,0x0006,0x0004,0x2803,0x5b40,0x0000,0x0024,0x0007,0x0001,0x8d02,
//0x0006,0x00d6,0x3e12,0xb817,0x3e12,0x3815,0x3e05,0xb814,0x3615,0x0024,0x0000,0x800a,
//0x3e10,0x3801,0x0006,0x0800,0x3e10,0xb803,0x0000,0x0303,0x3e11,0x3805,0x3e11,0xb807,
//0x3e14,0x3812,0xb884,0x130c,0x3410,0x4024,0x4112,0x10d0,0x4010,0x008c,0x4010,0x0024,
//0xf400,0x4012,0x3000,0x3840,0x3009,0x3801,0x0000,0x0041,0xfe02,0x0024,0x2903,0xb480,
//0x48b2,0x0024,0x36f3,0x0844,0x6306,0x8845,0xae3a,0x8840,0xbf8e,0x8b41,0xac32,0xa846,
//0xffc8,0xabc7,0x3e01,0x7800,0xf400,0x4480,0x6090,0x0024,0x6090,0x0024,0xf400,0x4015,
//0x3009,0x3446,0x3009,0x37c7,0x3009,0x1800,0x3009,0x3844,0x48b3,0xe1e0,0x4882,0x4040,
//0xfeca,0x0024,0x5ac2,0x0024,0x5a52,0x0024,0x4cc2,0x0024,0x48ba,0x4040,0x4eea,0x4801,
//0x4eca,0x9800,0xff80,0x1bc1,0xf1eb,0xe3e2,0xf1ea,0x184c,0x4c8b,0xe5e4,0x48be,0x9804,
//0x488e,0x41c6,0xfe82,0x0024,0x5a8e,0x0024,0x525e,0x1b85,0x4ffe,0x0024,0x48b6,0x41c6,
//0x4dd6,0x48c7,0x4df6,0x0024,0xf1d6,0x0024,0xf1d6,0x0024,0x4eda,0x0024,0x0000,0x0fc3,
//0x2903,0xb480,0x4e82,0x0024,0x4084,0x130c,0x0006,0x0500,0x3440,0x4024,0x4010,0x0024,
//0xf400,0x4012,0x3200,0x4024,0xb132,0x0024,0x4214,0x0024,0xf224,0x0024,0x6230,0x0024,
//0x0001,0x0001,0x2803,0x54c9,0x0000,0x0024,0xf400,0x40c2,0x3200,0x0024,0xff82,0x0024,
//0x48b2,0x0024,0xb130,0x0024,0x6202,0x0024,0x003f,0xf001,0x2803,0x57d1,0x0000,0x1046,
//0xfe64,0x0024,0x48be,0x0024,0x2803,0x58c0,0x3a01,0x8024,0x3200,0x0024,0xb010,0x0024,
//0xc020,0x0024,0x3a00,0x0024,0x36f4,0x1812,0x36f1,0x9807,0x36f1,0x1805,0x36f0,0x9803,
//0x36f0,0x1801,0x3405,0x9014,0x36f3,0x0024,0x36f2,0x1815,0x2000,0x0000,0x36f2,0x9817,
//0x0007,0x0001,0x8d6d,0x0006,0x01f6,0x3613,0x0024,0x3e12,0xb817,0x3e12,0x3815,0x3e05,
//0xb814,0x3645,0x0024,0x0000,0x800a,0x3e10,0xb803,0x3e11,0x3805,0x3e11,0xb811,0x3e14,
//0xb813,0x3e13,0xf80e,0x4182,0x384d,0x0006,0x0912,0x2803,0x6105,0x0006,0x0451,0x0006,
//0xc352,0x3100,0x8803,0x6238,0x1bcc,0x0000,0x0024,0x2803,0x7705,0x4194,0x0024,0x0006,
//0x0912,0x3613,0x0024,0x0006,0x0411,0x0000,0x0302,0x3009,0x3850,0x0006,0x0410,0x3009,
//0x3840,0x0000,0x1100,0x2914,0xbec0,0xb882,0xb801,0x0000,0x1000,0x0006,0x0810,0x2915,
//0x7ac0,0xb882,0x0024,0x3900,0x9bc1,0x0006,0xc351,0x3009,0x1bc0,0x3009,0x1bd0,0x3009,
//0x0404,0x0006,0x0451,0x2803,0x66c0,0x3901,0x0024,0x4448,0x0402,0x4294,0x0024,0x6498,
//0x2402,0x001f,0x4002,0x6424,0x0024,0x0006,0x0411,0x2803,0x6611,0x0000,0x03ce,0x2403,
//0x764e,0x0000,0x0013,0x0006,0x1a04,0x0006,0x0451,0x3100,0x8024,0xf224,0x44c5,0x4458,
//0x0024,0xf400,0x4115,0x3500,0xc024,0x623c,0x0024,0x0000,0x0024,0x2803,0x7691,0x0000,
//0x0024,0x4384,0x184c,0x3100,0x3800,0x2915,0x7dc0,0xf200,0x0024,0x003f,0xfec3,0x4084,
//0x4491,0x3113,0x1bc0,0xa234,0x0024,0x0000,0x2003,0x6236,0x2402,0x0000,0x1003,0x2803,
//0x6fc8,0x0000,0x0024,0x003f,0xf803,0x3100,0x8024,0xb236,0x0024,0x2803,0x75c0,0x3900,
//0xc024,0x6236,0x0024,0x0000,0x0803,0x2803,0x7208,0x0000,0x0024,0x003f,0xfe03,0x3100,
//0x8024,0xb236,0x0024,0x2803,0x75c0,0x3900,0xc024,0x6236,0x0024,0x0000,0x0403,0x2803,
//0x7448,0x0000,0x0024,0x003f,0xff03,0x3100,0x8024,0xb236,0x0024,0x2803,0x75c0,0x3900,
//0xc024,0x6236,0x0402,0x003f,0xff83,0x2803,0x75c8,0x0000,0x0024,0xb236,0x0024,0x3900,
//0xc024,0xb884,0x07cc,0x3900,0x88cc,0x3313,0x0024,0x0006,0x0491,0x4194,0x2413,0x0006,
//0x04d1,0x2803,0x9755,0x0006,0x0902,0x3423,0x0024,0x3c10,0x8024,0x3100,0xc024,0x4304,
//0x0024,0x39f0,0x8024,0x3100,0x8024,0x3cf0,0x8024,0x0006,0x0902,0xb884,0x33c2,0x3c20,
//0x8024,0x34d0,0xc024,0x6238,0x0024,0x0000,0x0024,0x2803,0x8dd8,0x4396,0x0024,0x2403,
//0x8d83,0x0000,0x0024,0x3423,0x0024,0x34e4,0x4024,0x3123,0x0024,0x3100,0xc024,0x4304,
//0x0024,0x4284,0x2402,0x0000,0x2003,0x2803,0x8b89,0x0000,0x0024,0x3423,0x184c,0x34f4,
//0x4024,0x3004,0x844c,0x3100,0xb850,0x6236,0x0024,0x0006,0x0802,0x2803,0x81c8,0x4088,
//0x1043,0x4336,0x1390,0x4234,0x0024,0x4234,0x0024,0xf400,0x4091,0x2903,0xa480,0x0003,
//0x8308,0x4336,0x1390,0x4234,0x0024,0x4234,0x0024,0x2903,0x9a00,0xf400,0x4091,0x0004,
//0x0003,0x3423,0x1bd0,0x3404,0x4024,0x3123,0x0024,0x3100,0x8024,0x6236,0x0024,0x0000,
//0x4003,0x2803,0x85c8,0x0000,0x0024,0xb884,0x878c,0x3900,0x8024,0x34e4,0x4024,0x3123,
//0x0024,0x31e0,0x8024,0x6236,0x0402,0x0000,0x0024,0x2803,0x8b88,0x4284,0x0024,0x0000,
//0x0024,0x2803,0x8b95,0x0000,0x0024,0x3413,0x184c,0x3410,0x8024,0x3e10,0x8024,0x34e0,
//0xc024,0x2903,0x4080,0x3e10,0xc024,0xf400,0x40d1,0x003f,0xff44,0x36e3,0x048c,0x3100,
//0x8024,0xfe44,0x0024,0x48ba,0x0024,0x3901,0x0024,0x0000,0x00c3,0x3423,0x0024,0xf400,
//0x4511,0x34e0,0x8024,0x4234,0x0024,0x39f0,0x8024,0x3100,0x8024,0x6294,0x0024,0x3900,
//0x8024,0x0006,0x0411,0x6894,0x04c3,0xa234,0x0403,0x6238,0x0024,0x0000,0x0024,0x2803,
//0x9741,0x0000,0x0024,0xb884,0x90cc,0x39f0,0x8024,0x3100,0x8024,0xb884,0x3382,0x3c20,
//0x8024,0x34d0,0xc024,0x6238,0x0024,0x0006,0x0512,0x2803,0x9758,0x4396,0x0024,0x2403,
//0x9703,0x0000,0x0024,0x0003,0xf002,0x3201,0x0024,0xb424,0x0024,0x0028,0x0002,0x2803,
//0x9605,0x6246,0x0024,0x0004,0x0003,0x2803,0x95c1,0x4434,0x0024,0x0000,0x1003,0x6434,
//0x0024,0x2803,0x9600,0x3a00,0x8024,0x3a00,0x8024,0x3213,0x104c,0xf400,0x4511,0x34f0,
//0x8024,0x6294,0x0024,0x3900,0x8024,0x36f3,0x4024,0x36f3,0xd80e,0x36f4,0x9813,0x36f1,
//0x9811,0x36f1,0x1805,0x36f0,0x9803,0x3405,0x9014,0x36f3,0x0024,0x36f2,0x1815,0x2000,
//0x0000,0x36f2,0x9817,0x0007,0x0001,0x1868,0x0006,0x0010,0x0032,0x004f,0x007e,0x00c8,
//0x013d,0x01f8,0x0320,0x04f6,0x07e0,0x0c80,0x13d8,0x1f7f,0x3200,0x4f5f,0x61a8,0x0000,
//0x0007,0x0001,0x8e68,0x0006,0x0054,0x3e12,0xb814,0x0000,0x800a,0x3e10,0x3801,0x3e10,
//0xb803,0x3e11,0x7806,0x3e11,0xf813,0x3e13,0xf80e,0x3e13,0x4024,0x3e04,0x7810,0x449a,
//0x0040,0x0001,0x0003,0x2803,0xa344,0x4036,0x03c1,0x0003,0xffc2,0xb326,0x0024,0x0018,
//0x0042,0x4326,0x4495,0x4024,0x40d2,0x0000,0x0180,0xa100,0x4090,0x0010,0x0fc2,0x4204,
//0x0024,0xbc82,0x4091,0x459a,0x0024,0x0000,0x0054,0x2803,0xa244,0xbd86,0x4093,0x2403,
//0xa205,0xfe01,0x5e0c,0x5c43,0x5f2d,0x5e46,0x020c,0x5c56,0x8a0c,0x5e53,0x5e0c,0x5c43,
//0x5f2d,0x5e46,0x020c,0x5c56,0x8a0c,0x5e52,0x0024,0x4cb2,0x4405,0x0018,0x0044,0x654a,
//0x0024,0x2803,0xb040,0x36f4,0x5810,0x0007,0x0001,0x8e92,0x0006,0x0080,0x3e12,0xb814,
//0x0000,0x800a,0x3e10,0x3801,0x3e10,0xb803,0x3e11,0x7806,0x3e11,0xf813,0x3e13,0xf80e,
//0x3e13,0x4024,0x3e04,0x7810,0x449a,0x0040,0x0000,0x0803,0x2803,0xaf04,0x30f0,0x4024,
//0x0fff,0xfec2,0xa020,0x0024,0x0fff,0xff02,0xa122,0x0024,0x4036,0x0024,0x0000,0x1fc2,
//0xb326,0x0024,0x0010,0x4002,0x4326,0x4495,0x4024,0x40d2,0x0000,0x0180,0xa100,0x4090,
//0x0010,0x0042,0x4204,0x0024,0xbc82,0x4091,0x459a,0x0024,0x0000,0x0054,0x2803,0xae04,
//0xbd86,0x4093,0x2403,0xadc5,0xfe01,0x5e0c,0x5c43,0x5f2d,0x5e46,0x0024,0x5c56,0x0024,
//0x5e53,0x5e0c,0x5c43,0x5f2d,0x5e46,0x0024,0x5c56,0x0024,0x5e52,0x0024,0x4cb2,0x4405,
//0x0010,0x4004,0x654a,0x9810,0x0000,0x0144,0xa54a,0x1bd1,0x0006,0x0413,0x3301,0xc444,
//0x687e,0x2005,0xad76,0x8445,0x4ed6,0x8784,0x36f3,0x64c2,0xac72,0x8785,0x4ec2,0xa443,
//0x3009,0x2440,0x3009,0x2741,0x36f3,0xd80e,0x36f1,0xd813,0x36f1,0x5806,0x36f0,0x9803,
//0x36f0,0x1801,0x2000,0x0000,0x36f2,0x9814,0x0007,0x0001,0x8ed2,0x0006,0x000e,0x4c82,
//0x0024,0x0000,0x0024,0x2000,0x0005,0xf5c2,0x0024,0x0000,0x0980,0x2000,0x0000,0x6010,
//0x0024,0x000a,0x0001,0x0d00,
//};
//
////FFT
//#define FFT_BANDS			14			//14个频段
//// 新的频率值
//code u16 VS_NEW_BANDS_FREQ_TBL[FFT_BANDS]={80,300,800,1270,2016,3200,4500,6000,7500,9000,11000,13000,15000,20000};
//


//=====================================================================
// 名称：Mp3Player_Init()  
// 功能：播放器初始化
// 入口参数：无	 
// 出口参数：1:初始化成功, 0:初始化失败
// 说明：    
//=====================================================================
u8 Mp3Player_Init(void)
{
    u16 ret;
		
    VS_HD_Reset();          // 硬复位    	 ok 
    ret = VS_Ram_Test();    // 存储器测试
    if(ret != 0X83FF && ret != 0x807F)return 0;
    VS_Set_Vol(120);        // 设置音量
    VS_Sine_Test();         // 正弦波测试
	VS_HD_Reset();          // 硬复位
	VS_Soft_Reset();        // 软复位
    return 1;
}



//// 重设频谱频率
//void SetBands(void)
//{
//    VS_Load_Patch((u16*)VS1053_PATCH,1000); 
////    VS_Set_Bands((u16*)VS_NEW_BANDS_FREQ_TBL,FFT_BANDS);//重设频谱频率
//
//}


















