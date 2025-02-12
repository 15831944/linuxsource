/******************************************************************************
模块名  ： DM6437_VIDCAP_DRV
文件名  ： dm6437_vidcap_api.h
相关文件： dm6437_vidcap_api.c
文件实现功能：
作者    ：张方明
版本    ：1.0.0.0.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
04/22/2008  1.0         张方明      创建
******************************************************************************/
#ifndef __DM6437_VIDCAP_API_H
#define __DM6437_VIDCAP_API_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* runtime include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "dm6437_drv_common.h"

/****************************** 模块的版本号命名规定 *************************
总的结构：mn.mm.ii.cc.tttt
     如  Osp 1.1.7.20040318 表示
模块名称Osp
模块1版本
接口1版本
实现7版本
04年3月18号提交

版本修改记录：
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.1.20080602
增加功能：创建
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.2.20080604
增加功能：使用自己的IOM封装
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.3.20080701
增加功能：根据第5版接口fpga调节偏移
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.4.20080704
增加功能：调节编码端6437偏移
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.5.20080707
增加功能：修正6437采集1080i图像偏上造成底部有绿条问题
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.6.20080710
增加功能：修正6437采集分量的D1图像奇偶场反问题
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.7.20080718
增加功能：修正6437采集关闭后重新打开时buf乱掉引起图像抖的问题
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.8.20080723
增加功能：修正6437采集D1从HDMI输出无图像的问题
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.9.20080830
增加功能：支持YUV16格式的VGA图像采集
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.10.20081022
增加功能：支持DVI接口
修改缺陷：无
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.11.20081121
增加功能：支持HD dsp0 vga采集
修改缺陷：无
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.12.20090216
增加功能：支持WXGA60 WXGA75采集
修改缺陷：无
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.13.20090427
增加功能：获得丢帧数
修改缺陷：中断函数中加入丢帧的保护
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.14.20090512
增加功能：支持60/1.001 24/1.001 30/1.001帧率的视频输入
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.15.20090723
增加功能：重新配置了vga参数
修改缺陷：无
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.16.20090728
增加功能：按照VESA标准修正了的所有VGA分辨率的偏移参数
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.17.20090803
增加功能：按照CEA861标准修正了的所有HD/SD分辨率的偏移参数
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.18.20090819
增加功能：重新配置了vga参数
修改缺陷：无
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.19.20090902
增加功能：无
修改缺陷：修改了7810上dsp1采集1080p的参数
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.20.20090908
增加功能：增加采集和播放模块同采同播同步设置接口
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.21.20090909
增加功能：增加采集状态统计接口;增加GetBuf时超时为0立即返回功能
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.22.20091028
增加功能：支持720P25/30采集
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.23.20091104
增加功能：重新配置了7810的video参数
修改缺陷：无
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM6437_VIDCAP_DRV 1.1.24.20091204
增加功能：完善了外同步576i和480i的配置
修改缺陷：无
提交人：张方明

****************************************************************************/
/* 版本号定义 */
#define VER_DM6437_VIDCAP_DRV         (const s8*)"DM6437_VIDCAP_DRV 1.1.24.20091204" 

/* 极限值定义 */
#define DM6437_VIDCAP_DEV_MAX_NUM        1      /* 目前一个DM6437上最大支持1个视频采集端口 */
#define DM6437_VIDCAP_BUF_MAX_NUM        32     /* 目前最多允许分配的BUF个数 */

/* Dm6437VidCapCtrl操作码定义 */
#define DM6437_VIDCAP_SET_BRIGHT         0      /* 亮度，  范围0x00~0xff */
#define DM6437_VIDCAP_SET_CONTRAST       1      /* 对比度，范围0x00~0xff */
#define DM6437_VIDCAP_SET_SATURATION     2      /* 饱和度，范围0x00~0xff */
#define DM6437_VIDCAP_SET_HUE            3      /* 色度，  范围0x00~0xff */
#define DM6437_VIDCAP_GET_LOST_FRAME     4      /* 获得丢帧数 */
#define DM6437_VIDCAP_GET_STAT           5      /* 获取采集状态统计; pArgs为(TDm6437VidCapStat *) */

/* 类型定义 */
typedef void * HDm6437VidCapDev;


/* 视频采集IO设备创建的参数结构定义 */
typedef struct{
    u32       dwVidInIntf;                              /* 视频输入接口，见: 视频接口宏定义 */
    TVidInfo  tVidInfo;                                 /* 视频信号信息 */
    TFBufDesc *pBufDescs;                               /* 指向用户分配的FBufDesc结构变量数组的首地址 */
    u32       dwFBufNum;                                /* 要分配的FBufDesc的个数，范围：0~DM6437_VIDCAP_BUF_MAX_NUM-1 */
    u16	      wWidthMax;                                /* 最大视频宽，像素为单位, 用于分配Buf */
    u16	      wHeightMax;                               /* 最大视频高，行为单位, 用于分配Buf
                                                           注意：由于用户会经常打开关闭设备，因此驱动中分配Buf时仅在第一次打开
                                                           设备时按照最大分辨率来分配Buf，后面打开设备继续沿用该Buf，这样可以避免
                                                           内存碎片过多引起分配失败问题 */
} TDm6437VidCapDevParam;

/* 视频采集IO设备状态统计结构定义 */
typedef struct{
    u32  dwUsrGetBufs;                          /* 用户累计取走的Buf个数 */
    u32  dwUsrPutBufs;                          /* 用户累计放入的Buf个数 */
    u32  dwWaitCapBufs;                         /* 驱动层等待采集数据的Buf个数,含当前正在采的那一帧 */
    u32  dwCapedBufs;                           /* 驱动层采集完毕等待用户取的Buf个数 */
    u32  dwDrvLostFrames;                       /* 驱动层丢弃的帧数 */
} TDm6437VidCapStat;

/*==============================================================================
    函数名      : Dm6437VidCapGetStd
    功能        ：查询指定视频源的制式
    输入参数说明：dwVidInIntf: 视频输入接口，见: 视频接口宏定义
                  ptVidInfo:   存放视频信号信息的指针，用户分配
    返回值说明  ：错误返回DM6437_DRV_FAILURE或错误码；成功返回DM6437_DRV_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm6437VidCapGetStd(u32 dwVidInIntf, TVidInfo *ptVidInfo);

/*==============================================================================
    函数名      : Dm6437VidCapOpen
    功能        ：视频采集IO设备打开
    输入参数说明：dwDevId: 0~DM6437_VIDCAP_DEV_MAX_NUM-1;
                  ptParam: 打开的参数
    返回值说明  ：错误返回NULL；成功返回控制句柄
------------------------------------------------------------------------------*/
HDm6437VidCapDev Dm6437VidCapOpen(u32 dwDevId, TDm6437VidCapDevParam *ptParam);

/*==============================================================================
    函数名      : Dm6437VidCapClose
    功能        ：视频采集IO设备关闭。
    输入参数说明：hVidCapDev: Dm6437VidCapOpen函数返回的句柄;
    返回值说明  ：错误返回DM6437_DRV_FAILURE或错误码；成功返回DM6437_DRV_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm6437VidCapClose(HDm6437VidCapDev hVidCapDev);

/*==============================================================================
    函数名      : Dm6437VidCapFBufGet
    功能        ：取一个视频采集BUF，用户可以多次调用取多个BUF。
    输入参数说明：hVidCapDev: 调用Dm6437VidCapOpen函数返回的句柄;
                  pBufDesc: 用户分配并传入指针，驱动将数据BUF信息拷贝给用户
                  nTimeoutMs: -1=wait forever; 0=no wait;其他正值为超时毫秒数
    返回值说明  ：错误返回DM6437_DRV_FAILURE或错误码；成功返回DM6437_DRV_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm6437VidCapFBufGet(HDm6437VidCapDev hVidCapDev, TFBufDesc *pBufDesc, l32 nTimeoutMs);

/*==============================================================================
    函数名      : Dm6437VidCapFBufPut
    功能        ：归还一个视频BUF
    输入参数说明：hVidCapDev: 调用Dm6437VidCapOpen函数返回的句柄;
                  pBufDesc: 用户调用Dm6437VidCapFBufGet得到的BufDesc信息
    返回值说明  ：错误返回DM6437_DRV_FAILURE或错误码；成功返回DM6437_DRV_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm6437VidCapFBufPut(HDm6437VidCapDev hVidCapDev, TFBufDesc *pBufDesc);

/*==============================================================================
    函数名      : Dm6437VidCapCtrl
    功能        ：视频采集IO设备控制，目前定义了
                    DM6437_VIDCAP_SET_BRIGHT:       pArgs为u8 *
                    DM6437_VIDCAP_SET_CONTRAST:     pArgs为u8 *
                    DM6437_VIDCAP_SET_SATURATION:   pArgs为u8 *
                    DM6437_VIDCAP_SET_HUE:          pArgs为u8 *
                  ......
    输入参数说明：hVidCapDev: 调用Dm6437VidCapOpen函数返回的句柄;
                 nCmd: 操作码；pArgs: 参数指针
    返回值说明  ：错误返回DM6437_DRV_FAILURE或错误码；成功返回DM6437_DRV_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm6437VidCapCtrl(HDm6437VidCapDev hVidCapDev, l32 nCmd, void *pArgs);

/*====================================================================
    函数名      : Dm6437VidCapGetVer
    功能        ：模块版本号查询。
    输入参数说明：pchVer： 给定的存放版本信息的buf指针
                  dwBufLen：给定buf的长度
    返回值说明  ：版本的实际字符串长度。小于0为出错;
                 如果实际字符串长度大于dwBufLen，赋值为0
--------------------------------------------------------------------*/
l32 Dm6437VidCapGetVer(s8 *pchVer, u32 dwBufLen);

#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif /* __DM6437_VIDCAP_API_H */
