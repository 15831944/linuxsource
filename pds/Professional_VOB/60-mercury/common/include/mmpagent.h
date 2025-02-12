/*****************************************************************************
   模块名      : Board Agent
   文件名      : mmpagent.h
   相关文件    : 
   文件实现功能: 单板启动及相关普通函数定义
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/25  1.0         jianghy       创建
   2004/11/11  3.5         李 博         新接口的修改
******************************************************************************/
#ifndef MMPAGENT_H
#define MMPAGENT_H

#include "osp.h"
#include "kdvsys.h"
#include "agtcomm.h"
#include "mcuagtstruct.h"
#include "mmpconfig.h"

#ifdef _VXWORKS_
#include "brddrvLib.h"
#endif

// MPC2 支持
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

#define BOARD_AGENT_MAX_INSTANCE	1		// 定义单板管理程序最大的实例数目
#define POWEN_ON_CONNECT            200
#define CONNECT_MANAGER_TIMEOUT		2000	// 重新连接MPC上管理程序的定时器时长(2秒)
#define REGISTER_TIMEOUT			4000	// 注册等待定时器时长(4秒)
#define GET_CONFIG_TIMEOUT			3000	// 取配置信息等待定时器时长(3秒)
#define SCAN_STATE_TIME_OUT			5000	// 定时扫描单板状态定时器时长(5s)
#define  MAX_TIME_SPAN_REBOOT	    3		// 最大重启时间间隔(分钟)
#define	MAXNUM_BOARD_ALARM			80		// 单板告警表的最大长度

//告警结构
typedef struct
{
	u32	dwBoardAlarmSerialNo;			//告警序列号
    u32 dwBoardAlarmCode;				//告警码
    u8  byBoardAlarmObjType;			//告警对象类型
    u8	byBoardAlarmObject[5];		    //告警对象
	TBoardAlarmMsgInfo tBoardAlarm;         //状态消息和结构
    BOOL  	bExist;						    //该告警是否有效
}TBoardAlarm;

//在WINDOWS下模拟使用
#ifdef WIN32
typedef struct
{
    u8 byBrdID;                      // 板子ID号 
    u8 byBrdLayer;                   // 板子所在层号
    u8 byBrdSlot;                    // 板子所在槽位号
}TBrdPosition;
#else
/*
#ifndef MMP
typedef struct
{
    u8 byBrdID;                      // 板子ID号 
    u8 byBrdLayer;                   // 板子所在层号
    u8 byBrdSlot;                    // 板子所在槽位号
}TBrdPosition;
#endif
*/
#endif 


class CBoardAgent:public CInstance
{
	enum 
	{ 
		STATE_IDLE,			//空闲
		STATE_INIT,			//初始化
		STATE_NORMAL,		//工作
	};

public:
	CBoardAgent();
	~CBoardAgent();

protected:
	//实例消息处理入口函数，必须override
	void InstanceEntry(CMessage* const pMsg);

	//退出时调用
	void InstanceExit();

	//OSP断链
	void ProcOspDisconnect( CMessage* const pcMsg );

	//以下是和MANAGER的消息处理
	//注册应答消息
	void ProcBoardRegAck( CMessage* const pcMsg );	
	//注册否定应答消息
	void ProcBoardRegNAck( CMessage* const pcMsg );
	//取配置信息消息定时器到期处理
	void ProcGetConfigTimeOut( CMessage* const pcMsg );
	//配置应答消息
	void ProcBoardGetConfigAck( CMessage* const pcMsg );
	//配置否定应答消息
	void ProcBoardGetConfigNAck( CMessage* const pcMsg );
	//告警同步请求
	void ProcAlarmSyncReq( CMessage* const pcMsg );
	//启动消息
	void ProcBoardPowerOn( CMessage* const pcMsg );
	//连接超时
	void ProcBoardConnectManagerTimeOut( BOOL32 bIsConnectA );
	//等待注册超时
	void ProcBoardRegisterTimeOut( BOOL32 bIsConnectA );
	
	//取单板软件版本消息处理
	void ProcGetVersionReq( CMessage* const pcMsg );
	//取单板的模块信息消息处理
	void ProcGetModuleInfoReq( CMessage* const pcMsg );
	//单板复位消息处理
	void ProcBoardResetCmd( CMessage* const pcMsg );
	//单板时间同步消息处理
	void ProcTimeSyncCmd( CMessage* const pcMsg );
	//单板自测消息处理
	void ProcBoardSelfTestCmd( CMessage* const pcMsg );
	//单板误码测试消息处理
	void ProcBitErrorTestCmd( CMessage* const pcMsg );
	//软件版本更新消息
	void ProcUpdateSoftwareCmd( CMessage* const pcMsg );
	//取单板的统计信息消息处理
	void ProcGetStatisticsReq( CMessage* const pcMsg );
	//取面板灯状态消息处理
	void ProcLedStatusReq( CMessage* const pcMsg );

	//以下是状态消息处理
	//内存状态
	void ProcBoardMemeryStatus( CMessage* const pcMsg );
	//文件系统状态
	void ProcBoardFileSystemStatus( CMessage* const pcMsg );
	//模块信息处理
	void ProcBoardModuleStatus( CMessage* const pcMsg );
	//灯状态改变
	void ProcBoardLedStatus( CMessage* const pcMsg );

protected:
	//测试用消息
	void ProcBoardGetCfgTest( CMessage* const pcMsg );
	void ProcBoardGetAlarm( CMessage* const pcMsg );
	void ProcBoardGetLastManagerCmd( CMessage* const pcMsg );
	void TestingProcess( CMessage* const pcMsg );

	
protected:
	//取单板的要连接的MPC的配置信息
	BOOL GetBoardCfgInfo();
	//连接管理程序
	BOOL ConnectManager(u32& dwMcuNode, u32 dwMpcIp, u16 wMpcPort);
	//发送消息给管理程序
	BOOL PostMsgToManager( u16  wEvent, u8 * const pbyContent, u16  wLen );
	//告警列表中增加相应的告警信息
    BOOL AddAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData );
	//告警列表中删除相应的告警信息
    BOOL DeleteAlarm( u32 dwSerialNo );	
	//告警列表中获取相应的告警信息
    BOOL FindAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData );
	//设置引起该告警的消息信息
	BOOL SetAlarmMsgInfo( u32 dwSerialNo, TBoardAlarmMsgInfo* const ptMsg );
    
    void RegisterToMcuAgent(u32 dwDstNode);
    void PostGetCfgMsg( u16 wEvent, u8 * const pbyContent, u16 wLen );

    void FreeDataA(void);
    void FreeDataB(void);
	// 比较两个时间是否超过指定间隔
	BOOL32 Compare2TimeIsValid(struct tm* ptTimeA, struct tm* ptTimeB); 
private:
	TBrdPosition	m_tBoardPosition;    // 单板位置，包括层、槽、类型
	u32	m_dwDstNode;    // 节点号
	u32	m_dwDstIId;		// 目的实例号
private:
    u32 m_dwDstNodeB;   // 节点号
    u32 m_dwDstIIdB;    // 目的实例号

private:
	time_t  m_tLastDisTimeA; // Mpc A 上次断链的时间
	time_t  m_tLastDisTimeB; // Mpc B 上次断链的时间
    u32 m_dwBoardIpAddr; // look 
    
    u8  m_byConnectNum;
#ifndef WIN32
#ifdef MMP
	TBrdMMPLedStateDesc m_tLedState;
#endif
#endif
	TBoardAlarm  m_atBoardAlarmTable[ MAXNUM_BOARD_ALARM ];  /*告警表*/
	u32			 m_dwBoardAlarmTableMaxNo;  /* 告警表中告警的最大编号*/
	//测试用
	BOOL    m_bIsTest;
	u16 	m_wLastEvent;
	u8	m_abyLastEventPara[20];
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CMmpConfig, 0 > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	//单板代理应用实例

API BOOL InitMmpAgent();

#endif  /* MMPAGENT_H */
