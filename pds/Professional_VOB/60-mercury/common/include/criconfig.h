/*****************************************************************************
   模块名      : Board Agent
   文件名      : criagent.h
   相关文件    : 
   文件实现功能: 相关配置函数定义
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/25  1.0         jianghy       创建
   2004/11/11  3.5         李 博         新接口的修改
******************************************************************************/
#ifndef CRICONFIG_H
#define CRICONFIG_H

#include "mcuconst.h"
#include "vccommon.h"
#include "mcuagtstruct.h"
#include "eqpcfg.h"
#include "agtcomm.h"

///*------------------------------------------------------------- 
//*包重传器表常量 
//*-------------------------------------------------------------*/
//#define SECTION_mcueqpPrsConfig			(const s8*)"mcueqpPrsConfig"
//#define KEY_mcueqpPrsId					(const s8*)"mcueqpPrsId"
//#define KEY_mcueqpPrsMCUStartPort		(const s8*)"mcueqpPrsMCUStartPort"
//#define KEY_mcueqpPrsSwitchBrdId		(const s8*)"mcueqpPrsSwitchBrdId"
//#define KEY_mcueqpPrsAlias				(const s8*)"mcueqpPrsAlias"
//#define KEY_mcueqpPrsRunningBrdId		(const s8*)"mcueqpPrsIpAddr"
//#define KEY_mcueqpPrsStartPort			(const s8*)"mcueqpPrsStartPort"
//#define KEY_mcueqpPrsFirstTimeSpan		(const s8*)"mcueqpPrsFirstTimeSpan"
//#define KEY_mcueqpPrsSecondTimeSpan		(const s8*)"mcueqpPrsSecondTimeSpan"
//#define KEY_mcueqpPrsThirdTimeSpan      (const s8*)"mcueqpPrsThirdTimeSpan"
//#define KEY_mcueqpPrsRejectTimeSpan		(const s8*)"mcueqpPrsRejectTimeSpan"


class CCriConfig
{
public:
	CCriConfig();
	~CCriConfig();
    friend class CBoardAgent;
	/*====================================================================
	功能：获取单板的槽位号
	参数：无
	返回值：ID值，0表示失败
	====================================================================*/
	u8	GetBoardId();

	/*====================================================================
	功能：获取要连接的MCU的IP地址(网络序)
	参数：无
	返回值：MCU的IP地址(网络序)，0表示失败
	====================================================================*/
	u32 GetConnectMcuIpAddr();

	/*====================================================================
	功能：获取连接的MCU的端口号
	参数：无
	返回值：MCU的端口号，0表示失败
	====================================================================*/
	u16  GetConnectMcuPort();
	
	/*====================================================================
	功能：得到单板IP地址
	参数：无
	返回值：单板IP地址(网络序)
	====================================================================*/
	u32 GetBrdIpAddr( );

	/*====================================================================
	功能：是否运行Prs
	参数：无
	返回值：运行返回TRUE，反之FALSE
	====================================================================*/
	BOOL IsRunPrs();

	/*====================================================================
	功能：获取prs配置信息
	参数：无
	返回值：MCU的端口号，0表示失败
	====================================================================*/
	BOOL GetPrsCfg( TPrsCfg* ptCfg );

	/*====================================================================
	功能：设置单板IP地址
	参数：无
	返回值：TRUE/FALSE
	====================================================================*/
	BOOL SetBrdIpAddr( u32 dwIp );

	/*====================================================================
	功能：设置PRS的配置信息
	参数：无
	返回值：TRUE/FALSE
	====================================================================*/
	BOOL SetPrsConfig( TEqpPrsEntry *ptPrsCfg );
	

	/*====================================================================
	功能：从文件读配置信息
	参数：无
	返回值：TRUE/FALSE
	====================================================================*/
	BOOL ReadConnectMcuInfo();
    
    u32  GetMpcBIp(void);
    u16  GetMpcBPort(void);
public:
    u32	 m_dwDstMcuNode;
    u32  m_dwDstMcuNodeB;
    
    u16  m_wDiscHeartBeatTime;
    u8   m_byDiscHeartBeatNum;

private:
	BOOL    bIsRunPrs;      //是否运行PRS
	TPrsCfg m_prsCfg;
	u32	    m_dwMpcIpAddr;
	u16 	m_wMpcPort;
    u32     m_dwMpcIpAddrB;
    u16     m_wMpcPortB;

	u8	m_byBrdId;
	u32 m_dwBrdIpAddr;
    u8 m_byChoice;
};

#endif /* CRICONFIG_H */
