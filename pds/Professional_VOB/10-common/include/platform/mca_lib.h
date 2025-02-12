/*****************************************************************************
模块名      : MCA
文件名      : mca_lib.h
相关文件    : 
文件实现功能: mca lib head file
作者        : Allen Wang
版本        : V1.0  Copyright(C) 1995-2012 KEDACOM, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
3/16/2012  01.0         	Allen Wang	Creat	
3/23/2012  01.1         	Allen Wang	修改命名规范
4/17/2010  01.2 		Allen Wang      修改相应实现及注释
******************************************************************************/

#ifndef __MCA_LIB_
#define __MCA_LIB_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdlib.h>
#include "kdvtype.h"

#define MCA_MAX_CHIP_NUM				10		/*系统支持的最大的芯片数目*/
#define MCA_DATA_BUFF_MAX_NUM			100		/*数据BUFF最大个数*/
#define MCA_CMD_BUFF_MAX_NUM			32		/*信令BUFF最大个数*/

#define MCA_DATA_BUF_VAILD				1		/*系统使能数据BUFF传输*/
#define MCA_DATA_BUF_INVAILD			0		/*系统不使能数据BUFF传输*/


#define MCA_RCV_DABUF_FROM_USER		1		/*数据BUFF由用户提供*/
#define MCA_RCV_DABUF_FROM_MCA		0		/*数据BUFF由MCA提供*/

#define MCA_INIT_RESTART_STRA			1		/*复位策略，如果有一个芯片未成功启动，则复位整个系统*/
#define MCA_INIT_NORMAL_STRA			0		/*普通策略*/

#define MCA_MSG_DESC_VAILD				0x5a5a5a5a


#define MCA_CHIP_DM8168					1
#define MCA_CHIP_MPC8377				2
#define MCA_CHIP_P2020					3
#define MCA_CHIP_C6678					4


#define ERR_MCAINIT						-233		/*MCA初始化错误*/
#define ERR_MCA_MSG_PEER_FULL			-234		/*对端buff满*/
#define ERR_MCA_MSG_CHANNEL_BUSY		-235		/*消息通道忙*/
#define ERR_MCA_MSG_NO_MSG			-236		/*没有信息可读*/
#define ERR_MCA_NO_MORE_DATABUF		-238		/*没有buff可读*/
#define ERR_MCA_ERR_PARM				-239		/*参数错误*/
#define ERR_MCA_ERR_CALL				-240		/*错误的调用、请参照API说明*/
#define ERR_MCA_ERR_DMA_TRANS			-241		/*dma传输错误*/
#define ERR_MCA_ERR_CACHE_FLUSH			-242		/*dma传输错误*/
#define ERR_MCA_ERR_EDMA_2DFILL			-243		/*dma传输错误*/


/*dma传输刷cache方式*/
#define 	SRC_NONFLUSH_DST_NONCOHERENT 0
#define 	SRC_FLUSH_DST_NONCOHERENT	1
#define 	SRC_NONFLUSH_DST_COHERENT	2
#define 	SRC_FLUSH_DST_COHERENT		3



typedef int  HMcaObj;

typedef void ( *TEPPrtFunc )(u32 dwEpid, u8 * pbyPrtBuff);

enum McaBufType
{
	MCA_BUF_CMD =0,
	MCA_BUF_DTA,
};

typedef struct ChipSet
{
	u32 dwType;		/*DM8168,MPC8377,P2020,C6678 etc...*/
	u32 dwIndex;	/*表示同类型芯片的索引*/
	u32 dwChipid;	/*唯一的chipid*/
} TChipSet;

typedef struct BufferDesc
{
	u32 dwPhysAddr;
	u32 dwVirAddr;
	u32 dwLen;
}TBufferDesc;


typedef struct McaParam
{
	u32 dwDBufVaild;	/*是否启用裸数据传送，MCA_DATA_BUF_VAILD表示启用*/								
	u32 dwRcvDBufStra;	/*由用户提供接收裸数据buffer 还是mca提供，如果用户提供的话，
						必须是物理地址连续的buffer，且4K对齐， 为MCA_RCV_DABUF_FROM_USER表示用户提供*/
	u32 dwRcvDBufNum;	/*用户提供的buffer个数，rcv_bufstra为1有效,<10*/
	TBufferDesc	 tRcvBuffer[MCA_DATA_BUFF_MAX_NUM];	/*用户提供的buffer ，填入地址和长度*/
	u32 dwMaxCmdSize;	/*信令包的最大长度，buffer由mca提供，为节约内存，不要超过128K, 大小4K对齐*/
	u32 dwMaxDataSize;	/*数据包的最大长度，buffer由用户或者mca提供，最大8M*/
	u32 dwCmdBufNum;	/*缓存的信令包个数，默认32个*/	
	u32 dwInitStra;	/*如果其中1个EP没有启动成功，
						是否需要复位整个系统，MCA_INIT_RESTART_STRA表示复位，MCA_INIT_NORMAL_STRA表示不复位RC有效*/
}TMcaParam;	

typedef struct McaCapability
{
	u32 dwChipid;		/*当前芯片ID，RC为0，其他为EP，可能不连续*/
	u32 dwChipNum;	/*整个系统芯片个数*/	
	TChipSet tMcaChip[MCA_MAX_CHIP_NUM];	/*系统中的各个芯片*/
}TMcaCapability;


/*====================================================================
    函数名      : McaGetCapbility
    功能        ：获取mca能力集,只能在open之后调用
    输入参数说明：cab: 指向能力集结构体的指针
    返回值说明  ：0，或者错误号
---------------------------------------------------------------------*/
s32 McaGetCapbility(HMcaObj * ptObj, TMcaCapability * ptCap);


/*====================================================================
    函数名      : McaOpen
    功能        ：打开mca
    输入参数说明：param:参数指针
    返回值说明  ：HMcaObj 指针，或者NULL
---------------------------------------------------------------------*/
HMcaObj * McaOpen(TMcaParam * ptParam);


/*====================================================================
    函数名      : McaClose
    功能        ：关闭mca
    输入参数说明：obj: 指向由mca_open打开的句柄
    返回值说明  ：0，或者错误号
---------------------------------------------------------------------*/
s32  McaClose(HMcaObj * ptObj);


/*====================================================================
    函数名      : McaReinit
    功能        ：重新初始化MCA，目前只有dwDBufVaild，dwRcvDBufStra，dwRcvDBufNum
				tRcvBuffer 参数生效
    输入参数说明：obj: 指向由mca_open打开的句柄
    					      ptParam:参数指针
    返回值说明  ：0，或者错误号
---------------------------------------------------------------------*/
s32  McaReinit(HMcaObj * ptObj, TMcaParam * ptParam);


/*==============================================================================
    函数名      : McaReadCmdmsg
    功能        :读取一条信令消息
    输入参数说明: ptObj:       控制句柄
                  pbyBuf:       [I]用户分配的Buf，用来存放读到的数据;
                  dwSize:      [IO]用户分配，输入时指定用户的Buf的大小，如果足够大，
                  		则读取成功
                  nTimeout:     [I]超时值: 0=立即返回, -1=永远等待, >0=等待毫秒数                  			
    返回值说明  : 0，或者错误号或者实际读出长度
------------------------------------------------------------------------------*/
s32 McaReadCmdmsg(HMcaObj * ptObj, u8 *pbyBuf, u32 dwSize, s32 nTimeout);


/*==============================================================================
    函数名      : McaWriteCmdmsg
    功能        :写入一条信令消息
    输入参数说明: ptObj:       控制句柄
    		dwChipid:	[O]发往 信息的chipid
                  pbyBuf:       [O]用户分配的Buf，用来存放发送的数据;
                  dwSize:      [IO]用户分配，输入时指定用户的Buf的大小
    返回值说明  : 0，或者错误号
------------------------------------------------------------------------------*/
s32 McaWriteCmdmsg(HMcaObj * ptObj, u32 dwChipid, u8 *pbyBuf, u32 dwSize);


/*==============================================================================
    函数名      : McaReadDataMsg
    功能        :读取一条data消息，主要用户裸数据传输
    输入参数说明: ptObj:       控制句柄
                  pdwVirtaddr:		指向虚拟地址的指针，如果该虚拟地址为0，表示使用预先
                  		定义好的buf，读取成功后，驱动会将虚拟地址填进去，如果
                  		不为0，驱动会拷贝数据进该虚拟地址
                  pdwSize:      [IO]用户分配，指向大小的指针，输入时指定用户的Buf的大小，
                  		如果足够大则读取成功，返回时驱动会将数据大小填进去
		  pdwPhyaddr: 指向物理地址的指针，如果是预先分配好的地址，读取成功后
		  		会将物理地址填进去,如果不是用预先分配的buf，则不用关心		  
                  nTimeout:     [I]超时值: 0=立即返回, -1=永远等待, >0=等待毫秒数                  			
    返回值说明  : 0，或者错误号
------------------------------------------------------------------------------*/
s32 McaReadDataMsg(HMcaObj * ptObj, u32 *pdwVirtaddr, u32 *pdwPhyaddr,u32 *pdwSize,  s32 nTimeout);


/*==============================================================================
    函数名      : McaReadDataMsgDone
    功能        :读取的一条data消息已经处理完毕，这个地址可以用来进行
    			下一条数据的接收，只有在用户提供裸数据buf的时候有用
    输入参数说明: ptObj:       控制句柄
                  pbyBuf:       [I]Buf地址，虚拟地址，只在用户buf使能的时候有用
    返回值说明  : 0，或者错误号
------------------------------------------------------------------------------*/
s32 McaReadDataMsgDone(HMcaObj * ptObj, u8 *pbyBuf);


/*==============================================================================
    函数名      : McaWriteDataMsg
    功能        :写一条数据消息，主要用于用户裸数据写
    输入参数说明: ptObj:       控制句柄
                  pbyBuf:       [O]用户分配的Buf，用来存放发送的数据;
                  dwChipid:  发往信息的chipid
                  dwPhyaddr:buf的物理地址，如果是物理地址连续且4K对齐，可以DMA传输
                  dwSize:      [IO]用户分配，输入时指定用户的Buf的大小
                  nTimeout:     [I]超时值: 0=立即返回, -1=永远等待, >0=等待毫秒数
     	 	       		作用:	在对端无法接收的情况，为减少DMA资源浪费，
     	 	       		本端不会发送数据
    返回值说明  : 0，或者错误号
------------------------------------------------------------------------------*/
s32 McaWriteDataMsg(HMcaObj * ptObj, u32 dwChipid, u8 *pbyBuf, u32 dwPhyaddr, u32 dwSize, s32 nTimeout);


/*==============================================================================
    函数名      : mca_inc_heartbeat
    功能        : 增加当前的心跳数，供别人查询
    输入参数说明:  ptObj:       控制句柄
    返回值说明  : 0，或者错误号
------------------------------------------------------------------------------*/
s32 McaIncHeartbeat(HMcaObj * ptObj);


/*==============================================================================
    函数名      : mca_get_heartbeat
    功能        : 查询指定心跳计数，如果不再累加，说明通信已经挂死
    输入参数说明:  ptObj:       控制句柄
    					dwChipid:	需要查询的芯片的id
    返回值说明  : 心跳，或者错误号
------------------------------------------------------------------------------*/
u32 McaGetHeartbeat(HMcaObj * ptObj, u32 dwChipid);

/*==============================================================================
    函数名      : McaGetReservedMem
    功能        : 获取保留的物理地址连续的内存，必须在MCAopen之后调用
    			  且只能调用一次，获取的mem是带cache的mem
    输入参数说明: 
    					dwPhyAddr:	获取reserved mem的物理地址，传入时写NULL
    					dwVirAddr:	获取reserved mem的虚拟地址，传入时写NULL
    					dwLen:	需要获取的长度
    返回值说明  : 0，或者错误号
------------------------------------------------------------------------------*/
u32 McaGetReservedMem(u32 *dwPhyAddr, u32 *dwVirAddr, u32 dwLen);

/*==============================================================================
    函数名      : McaDmaTransfer
    功能        : 进行dma传输，传输之前默认会刷一次cache
    输入参数说明: 
    					dwSrcPhyAddr:		源物理地址
    					dwSrcVirtAddr:		源虚拟地址
    					dwDstPhyAddr:	目的物理地址
    					dwDstVirtAddr:		目的虚拟地址
    					dwLen:	传输长度，4K对齐
    					dwCacheMode:	刷cache方式:
    									SRC_FLUSH_DST_NONCOHERENT:	刷源地址cache
    									SRC_NONFLUSH_DST_COHERENT:	同步目的地址cache
    									SRC_NONFLUSH_DST_NONCOHERENT :	皆不需要
    									SRC_FLUSH_DST_COHERENT:皆需要    									
    返回值说明  : 0，或者错误号
------------------------------------------------------------------------------*/
u32 McaDmaTransfer(u32 dwSrcPhyAddr, u32 dwSrcVirtAddr, u32 dwDstPhyAddr, u32 dwDstVirtAddr, u32 dwLen, u32 dwCacheMode);


/*====================================================================
    函数名      : McaEdmaFill2D
    功能        ：Ti81xx Edma 2D Fill
    输入参数说明：pixPhyAddr: 源物理地址
    			  dstPhyAddr: 目的物理地址
    			  width:	  2D区域的宽
    			  height:	  2D区域的高
    			  pitch:	  目标同步长度
    			  bpp:		  像素字节数
    返回值说明  ：0，或者错误号
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/05/24    1.0        周新星        创建
====================================================================*/
u32 McaEdmaFill2D(u32 pixPhyAddr, u32 dstPhyAddr, u32 width, u32 height, u32 pitch, u32 bpp);


/*==============================================================================
    函数名      : McaCacheFlush
    功能        : 刷一次cache
    输入参数说明: 
    					VirtAddr:		孕槟獾刂�
    					dwLen:	传输长度，4K对齐
    					dwCacheMode:	刷cache方式:
    									SRC_FLUSH_DST_NONCOHERENT:	刷源地址cache
    									SRC_NONFLUSH_DST_COHERENT:	同步目的地址cache
    返回值说明  : 0，或者错误号
------------------------------------------------------------------------------*/
u32 McaCacheFlush(u32 VirtAddr, u32 dwLen, u32 dwCacheMode);


/*==============================================================================
    函数名      : McaPrtEnable
    功能        : 允许对应chipid的打印信息打印到RC，只能RC调用
    输入参数说明: dwChipid:	需要查询的芯片的id
    返回值说明  : 无
------------------------------------------------------------------------------*/
void McaPrtEnable(u32 dwChipid);


/*==============================================================================
    函数名      : McaPrtDisable
    功能        : 不允许对应chipid的打印信息打印到RC，只能RC调用
    输入参数说明: dwChipid:	芯片的id
    返回值说明  : 无
------------------------------------------------------------------------------*/
void McaPrtDisable(u32 dwChipid);


/*==============================================================================
    函数名      : McaPrtFuncRegister
    功能        : 业务注册打印函数
    输入参数说明: pfHookFunc--业务的打印函数
    返回值说明  : 无
------------------------------------------------------------------------------*/
void McaPrtFuncRegister(TEPPrtFunc pfHookFunc);


/*==============================================================================
    函数名      : McaPrtStart
    功能        : rc端启动log监控线程
    输入参数说明: ptObj 控制句柄
    返回值说明  : 无
------------------------------------------------------------------------------*/
void McaPrtStart(HMcaObj *ptObj);


/*==============================================================================
    函数名      : McaEpPrintf
    功能        : ep打印函数，该函数可以将打印输出至控制台及ep的log ring buffer
    输入参数说明: ptObj:       控制句柄
		  fmt, ...:  格式通配及参数
    返回值说明  : 无
------------------------------------------------------------------------------*/
void McaEpPrintf(HMcaObj *ptObj, const char *fmt, ...);

#ifdef __cplusplus 
}
#endif /* __cplusplus */


#endif // __MCA_LIB_


