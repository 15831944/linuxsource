/******************************************************************************
模块名      ： audcodec_common              
文件名      ： audcodec_common.h
相关文件    ：无
文件实现功能： 编解码函数参数声明等
---------------------------------------------------------------------------------------
修改记录:
日  期                  版本            修改人     走读人        修改内容
02/22/2008              1.0             陆志洋                    创建
**************************************************************************************/
#ifndef _AUDCODEC_COMMON_H_
#define _AUDCODEC_COMMON_H_
#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */

#define AUDIO_SUCCESS          0        //成功返回值        
#define AUDIO_FAILURE         (-1)      //失败返回值
#define AUDIO_VER_LEN          128      //版本号需要的最小内存空间大小

#define TAudioEncStatusParam TAudioEncParam
#define TAudioDecStatusParam TAudioDecParam

//音频采样率模式
typedef enum
{
	SAMPLE_RATE_8K = 8000,         //采样率为8kHz
	SAMPLE_RATE_16K = 16000,       //采样率为16kHz
	SAMPLE_RATE_24K = 24000,       //采样率为24kHz
	SAMPLE_RATE_32K = 32000,       //采样率为32kHz
	SAMPLE_RATE_48K = 48000        //采样率为48kHz
}TEnumAudSampleRate;

//音频采样率索引
typedef enum
{
	  SAMPLE_RATE96000_INDEX = 0, //采样率为96kHz的索引是0
		SAMPLE_RATE88200_INDEX,     //采样率为88.2kHz的索引是1
		SAMPLE_RATE64000_INDEX,     //采样率为64kHz的索引是2
		SAMPLE_RATE48000_INDEX,     //采样率为48kHz的索引是3
		SAMPLE_RATE44100_INDEX,     //采样率为44.1kHz的索引是4
		SAMPLE_RATE32000_INDEX,     //采样率为32kHz的索引是5
		SAMPLE_RATE24000_INDEX,     //采样率为24kHz的索引是6  
		SAMPLE_RATE22050_INDEX,     //采样率为22.05kHz的索引是7
		SAMPLE_RATE16000_INDEX,     //采样率为16kHz的索引是8
		SAMPLE_RATE12000_INDEX,     //采样率为12kHz的索引是9
		SAMPLE_RATE11025_INDEX,     //采样率为11.025kHz的索引是10 
		SAMPLE_RATE8000_INDEX      //采样率为8kHz的索引是11
}TEnumAudSampleRateIndex;

//声道模式
typedef enum
{
	CH_MONO = 1,                  //单声道
	CH_STEREO                     //双声道
}TEnumAudChannel;

//样本位宽模式
typedef enum
{
	BIT_WIDTH_16 = 16,            //样本位宽为16bit
	BIT_WIDTH_32 = 32             //样本位宽为32bit
}TEnumAudBitWidth;

//比特率模式
typedef enum
{
	AACLC_MO_16K_48K = 48000,//命名方式：算法_声道_采样率_比特率
	AACLC_MO_32K_72K = 72000,
	AACLC_MO_48K_96K = 96000,
	AACLC_ST_16K_48K = 48000,
	AACLC_ST_32K_72K = 72000,
	AACLC_ST_48K_96K = 96000,
}TEnumAudBitRate;

//编码器初始化参数结构体
typedef struct
{
    l32 l32Channel;     //声道模式（TEnumAudChannel）
    l32 l32BitWidth;    //样本位宽模式（TEnumAudBitWidth）
    l32 l32SampleRate;  //音频采样率模式（TEnumAudSampleRate）
    l32 l32BitRate;     //比特率模式(TEnumAudBitRate)
    l32 l32FrameLen;    //编码一帧样本数
    u32 u32FrameHeadFlag; //每帧码流是否带头信息(1：有，0：否)
    u32 u32Reserved;    //保留参数
}TAudioEncParam;

//编码输入结构体
typedef struct
{
    u8 *pu8InBuf;      //指向编码器输入缓存指针
    u32 u32InBufSize;  //输入缓存字节数 
    u8 *pu8OutBuf;     //指向编码器输出缓存指针
    u32 u32OutBufSize; //输出缓存字节数  
    u32 u32Reserved;   //保留参数
}TAudEncInput;

//编码输出结构体
typedef struct
{       
    u32  u32OutBitStreamSize;   //输出码流的字节数
    u32  u32StatisCycles;       //编码一帧效率统计
    u32  u32Reserved;           //保留参数
}TAudEncOutput;

//解码器初始化参数结构体                  
typedef struct                                  
{                                               
    l32 l32BitRate;     //比特率模式(TEnumAudBitRate)
    u32 u32FrameHeadFlag; //每帧码流是否带头信息(1：有，0：否)
    u32 u32Reserved;    //保留参数                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
}TAudioDecParam;        

//解码器输入结构体                                                 
typedef struct                                 
{                                              
    u8 *pu8InBuf;            //指向解码器输入缓存指针   
    u32 u32InBufSize;        //输入缓存字节数                                                                                                                                                                                                                                                                                      捍嬷刚�

    u8 *pu8OutBuf;           //指向解码器输出缓存指针 
    u32 u32OutBufSize;       //输出缓存字节数                                                                                                                                                                                                                                                                                           
    u32 u32InBitStreamSize;  //每帧码流的字节数   
    u32 u32SampelFreIndex;   //每帧采样率索引
    u32 u32Channels;         //每帧通道数                                                                                                                                                                                                                                                                        淖纸谑�

    u32 u32Reserved;         //保留参数                                                                                                                                                                                                                                                                                                
}TAudDecInput;      

//解码器输出结构体
typedef struct
{               
    u32  u32OutFrameLen ; //每帧输出样本数
    u32  u32StatisCycles; //解码一帧效率统计
    u32  u32Reserved;     //保留参数
}TAudDecOutput; 

//错误码定义
#define ERR_AACLCENC  (15 << 24)    //AACLC编码子模块    
#define ERR_AACLCDEC  (16 << 24)    //AACLC解码子模块
                                                                                                                    
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif


