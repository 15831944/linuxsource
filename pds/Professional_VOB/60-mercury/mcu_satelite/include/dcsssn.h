/*****************************************************************************
   ģ����      : mcu
   �ļ���      : dcsssn.h
   ����ļ�    : dcsssn.cpp
   �ļ�ʵ�ֹ���: MCU��T120����Ӧ��ͷ�ļ�
   ����        : �ű���
   �汾        : V4.0  Copyright(C) 2005-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/11/28  4.0         �ű���        ����
******************************************************************************/
#ifndef _MCUDCS_H_
#define _MCUDCS_H_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mcudcspdu.h"

class CDcsSsnInst : public CInstance
{
	enum
	{
		STATE_T120_IDLE,
		STATE_T120_NORMAL
	};

public:
	u32		m_dwDcsNode;			//DCS��OSP���ӽ���
	u32		m_dwDcsIId;				//ͨѶDCSʵ����
	u32		m_dwMcuIId;				//ͨѶMCUʵ����
	u32		m_dwDcsIp;				//DCS�ĵ�ַ
	u16		m_wDcsPort;				//DCS�Ķ˿�
	
public:
	void	DaemonInstanceEntry( CMessage *const pcMsg, CApp * pcApp );	
	void	DaemonProcDcsMcuRegReq( CMessage *const pcMsg, CApp *pcApp );
    void    DaemonProcAppTaskRequest( const CMessage * pcMsg );
    
	void	InstanceEntry( CMessage *const pcMsg );
	void	ProcDcsMcuMsg( CMessage *const pcMsg );
	void	ProcMcuDcsMsg( CMessage *const pcMsg );
	void	ProcDcsMcuRegisterReq( CMessage *const pcMsg ); 
	void	ProcMcuDcsCreateConfReq( CMessage *const pcMsg );
	void	ProcDcsMcuCreateConfRsp( CMessage *const pcMsg );
	void	ProcMcuDcsReleaseConfReq( CMessage *const pcMsg );
	void	ProcDcsMcuReleaseConfRsp( CMessage *const pcMsg );
	void	ProcMcuDcsAddMtReq( CMessage *const pcMsg );
	void	ProcMcuDcsDelMtReq(	CMessage *const pcMsg );
	void	ProcDcsMcuAddMtRsp( CMessage *const pcMsg );
	void	ProcDcsMcuDelMtRsp( CMessage *const pcMsg );
	void	ProcDcsMcuMtOnlineNotify ( CMessage *const pcMsg );
	void	ProcDcsMcuMtOfflineNotify( CMessage *const pcMsg );    
	void	ProcDcsDisconnect( CMessage *const pcMsg );
    void    ProcMSStateExchageNotify( CMessage *const pcMsg );
    void    ProcDcsMcuGetMsStatusReq( CMessage *const pcMsg );
	void	ClearInst();
	
    void    InstanceDump( u32 dwParam = 0 );
protected:
	BOOL32	SendMsgToDcs( u16 wEvent, u8 *const pbyMsg = NULL, u16 wLen = 0 );
	BOOL32	SendMsgToInst( s32 nInst,  u16 wEvent, u8 *const pbyMsg = NULL, u16 wLen = 0 );
	
	void	McuDcsRegNack( CMcuDcsRegNackPdu cRegNackPdu, CNetDataCoder cRegCoder, 
		                   u8 achRegNackData[sizeof(CMcuDcsRegNackPdu)], u16 wErrorType, u16 wEvent );
	void	McuDcsRegAck(  CMcuDcsRegAckPdu cRegAckPdu, CNetDataCoder cRegCoder, 
						   u8 achRegAckData[sizeof(CMcuDcsRegAckPdu)], u16 wEvent );
public:
	CDcsSsnInst();
	virtual ~CDcsSsnInst();
};

class CDcsConfig
{
public:
	u32 m_adwDcsIp[MAXNUM_MCU_DCS];		//ע��������DCS��ַ����
	
public:
	BOOL32 SendMsgToMcuConf( u8 byConfIdx, u16 wEvent, u8 *const pbyMsg  = NULL, u16 wLen  = 0 );
	BOOL32 SendMsgToMcuConf( CServMsg &cServMsg );
	BOOL32 SendMsgToMcuDaemon( CServMsg &cServMsg );
    void BroadcastToAllDcsSsn( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );

public:	
	CDcsConfig();
	virtual ~CDcsConfig();
};

inline BOOL32 MSGBODY_LEN_GE(CServMsg &cServMsg, u16 wLen);

typedef zTemplate < CDcsSsnInst, MAXNUM_MCU_DCS, CDcsConfig > CDcsSsnApp;
extern CDcsSsnApp g_cDcsSsnApp;

#endif    // _MCUDCSSSN_H_
