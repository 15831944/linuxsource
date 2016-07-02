/*****************************************************************************
   ģ����      : mcu_new
   �ļ���      : mtssn.h
   ����ļ�    : mtssn.cpp
   �ļ�ʵ�ֹ���: �¼�MT�ỰӦ����ͷ�ļ�
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2002/07/25  0.9         ����        ����
   2002/01/07  1.0         ����        ʹ�ú�H323���и���
   2009/09/01  4.6         �ű���      ��ֲ��4.6����ƽ̨
******************************************************************************/

// mtssn.h: interface for the CMtSsnInst class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MTSSN_H__AB423FC4_7512_4B60_9269_742737980FBF__INCLUDED_)
#define AFX_MTSSN_H__AB423FC4_7512_4B60_9269_742737980FBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vctopo.h"
#include "evmcu.h"

#ifndef SOCKET 
#define SOCKET   int
#endif


//���ն�roundTripDelay���ֵ���룩
#define MAXDELAY_MT_ROUNDTRIPDELAY	10

//�Ự��ʱ������ϵ��
const u16 TIMER_MTSSN_COEFF		= 8;	//0.8

class CMtSsnInst : public CInstance  
{
public:
	enum 
	{ 
		STATE_IDLE		= 0,
#ifndef H323
		STATE_CONNECTED	= 1,	//���ӽ�����δ�������״̬
#endif
		STATE_CALLING	= 2,	//���������������ڵȴ��ն�Ӧ���
								//�յ��ն˼�������ȴ�ҵ���Ӧ
		STATE_NORMAL	= 3,	//���гɹ�MT�����������״̬
		STATE_WAITING	= 4		//�����ն������ȴ�״̬
	};

	enum
	{
		TIMER_WAIT_REPLY,		//�ȴ�Ӧ��
		TIMER_WAIT_CALL_RSP,	//�ȴ��ն˶Ի��������Ӧ���
								//�ȴ�ҵ����ն˼��������Ӧ��
		TIMER_ROUNDTRIPDELAY_REQ,		//��ʱ����roundTripDelayRequest
		TIMER_WAIT_ROUNDTRIPDELAY_RSP,	//�ȴ�����roundTripDelayResponse

		TIMER_RECEIVE_SIGNAL,	//��������ʱ��
	};

protected:
#ifndef H323
	u32		m_dwMtNode;		//��Ӧ�ն˽ڵ��

#else
	HCALL	m_hsCall;	//stack handle of call
#endif	
	u8		m_byTimeoutTimes;	//roundTripDelayRsp time out times
	BOOL	m_bGotRoundTripDelayRsp;	//whether got roundTripDelay

	u8		m_byMtId;		//��Ӧ�ն˺�
	CConfId	m_cConfId;	//�ն����������
	char	m_abySavedMsgHdr[SERV_MSGHEAD_LEN];	//����ԭ����ͷ
	u16		m_wSavedEvent;
    static SOCKET  m_hSocket;      //�ն˷���UDP���ư���SOCKET

#ifndef H323
	BOOL SendMsgToSubMt( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
#endif
	time_t timeold;
	u32    eventold;
public:
	void ProcMtMcuDaemonConfMsg( const CMessage * pcMsg );
	void ProcMtMcuMsg( const CMessage * pcMsg );
	void ProcMcuMtMsg( const CMessage * pcMsg );
	void ProcMtMcuResponse( const CMessage * pcMsg );
	void ProcMcuMtRequest( const CMessage * pcMsg );
	void ProcMtMcuGetMcuTopoReq( const CMessage * pcMsg );
	void ProcMtDisconnect( const CMessage * pcMsg );
	void ProcMtMcuRegReq( const CMessage * pcMsg );
	void ProcMtMcuReadyNotif( const CMessage * pcMsg );
	void ProcMcuMtDropMtCmd( const CMessage * pcMsg );
	void ProcMcuMtInviteMtReq( const CMessage * pcMsg );		//�����ն˼���
	void ProcMtMcuInviteMtRsp( const CMessage * pcMsg );		//�ն˶���������Ӧ��
	void ProcMtMcuApplyJoinRequest( const CMessage * pcMsg );	//�ն��������
	void ProcMcuMtApplyJoinRsp( const CMessage * pcMsg );		//mcu���ն˼��������Ӧ��
	void ProcMtMcuCreateConfRequest( const CMessage * pcMsg );	//�ն˴�������
	void ProcMcuMtCreateConfRsp( const CMessage * pcMsg );		//mcu���ն˻��鴴�������Ӧ��

	void ProcMcuMtOpenLogicChnReq( const CMessage * pcMsg );	//mcu���ն�˫��ͨ��
	void ProcMcuMtCloseLogicChnCmd( const CMessage * pcMsg );	//mcu�ر��ն�˫��ͨ��
	
	/*
	 *	Some Special Message For Satellite MCU
	 */
	void ProcMcuMtCapbilitySetNotif( const CMessage * pcMsg );
	void PostMessage2Conf( u16 wInstanceId, const CServMsg & cServMsg );
	BOOL FilterMessage( u32 eventid );
	/* For Message Transport 				*/
	SOCKET  CreateUdpSocket( u32 dwIpAddr, u16 wPort );
	BOOL DropSocketMulticast( SOCKET sock ,u32 dwMulticastIP, u32 dwIfIP  );
	BOOL JoinSocketMulticast( SOCKET sock ,u32 dwMulticastIP ,u32 dwIfIP );
	
	void ProcTimerWaitReply( const CMessage * pcMsg );

#ifdef H323
	void SendH323MsgToSubMt( const CMessage * pcMsg );
#endif
	void ProcTimerRoundTripDelayReq( const CMessage * pcMsg );
	void ProcTimerWaitRoundTripDelayRsp( const CMessage * pcMsg );
	void ProcMtMcuRoundTripDelayRsp( const CMessage * pcMsg );
	
	void ProcTimerWaitCallRsp( const CMessage * pcMsg );
	void DaemonProcAppTaskRequest( const CMessage * pcMsg );
	void DaemonProcPowerOn( const CMessage * pcMsg, CApp * pcApp );
	void InstanceEntry( CMessage * const pcMsg );
	void DaemonInstanceEntry( CMessage * const pcMsg, CApp * pcApp );
	void InstanceDump( u32 param = 0 );
	void InstanceExit();

	CMtSsnInst();
	virtual ~CMtSsnInst();

    void SendMsgToConf( u8 byConfIdx, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
    void SendMsgToDaemonConf( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
};


class CMtConfig
{
public:
	BOOL SendMsgToMtSsn( u8 byMtId, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	void BroadcastToAllMtSsn( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	BOOL SendMultiCastMsg( const CServMsg& cServMsg );
    
	CMtConfig();
	virtual ~CMtConfig();
};

typedef zTemplate< CMtSsnInst, MAXNUM_MCU_MT, CMtConfig, sizeof( u8 ) > CMtSsnApp;

extern CMtSsnApp	g_cMtSsnApp;	//�¼�MT�ỰӦ��ʵ��


/*====================================================================
    ������      ��BroadcastToAllMtSsn
    ����        ������Ϣ�������Ѿ��Ǽ����ӵ��¼��ն˶�Ӧ�ĻỰʵ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ�룬ȱʡΪNULL
				  u16 wLen, ��Ϣ���ȣ�ȱʡΪ0
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/28    1.0         LI Yi         ����
====================================================================*/
inline void CMtConfig::BroadcastToAllMtSsn( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	::OspPost( MAKEIID( AID_MCU_MTSSN, CInstance::EACH ), wEvent, pbyMsg, wLen );
}

/*====================================================================
    ������      ��SendMsgToMtSsn
    ����        ������Ϣ��ָ�����¼��ն˶�Ӧ�ĻỰʵ�����ն˺�Ϊ0ʱ����
				  ����IDLEʵ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, ָ�����ն˺ţ����ΪNULL��������һIDLEʵ��
				  u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ�룬ȱʡΪNULL
				  u16 wLen, ��Ϣ���ȣ�ȱʡΪ0
    ����ֵ˵��  ���ɹ�����TRUE�������¼��ն�δ�ǼǷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/28    1.0         LI Yi         ����
====================================================================*/
inline BOOL CMtConfig::SendMsgToMtSsn( u8 byMtId, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	if( byMtId == NULL )	//��������IDLEʵ��
	{
		::OspPost( MAKEIID( AID_MCU_MTSSN, CInstance::PENDING ), wEvent, pbyMsg, wLen );
		return( TRUE );
	}
	else if( byMtId >= MAXNUM_MCU_MT )
	{
		return( FALSE );
	}
	else	//����Alias
	{
//		::OspPost( (char*)&byMtId, sizeof( u8 ), AID_MCU_MTSSN, wEvent, pbyMsg, wLen );
		OspPost( MAKEIID( AID_MCU_MTSSN, byMtId ), wEvent, pbyMsg, wLen );
		return( TRUE );
	}
}

inline BOOL CMtConfig::SendMultiCastMsg( const CServMsg& cServMsg )
{
	::OspPost( MAKEIID( AID_MCU_MTSSN, CInstance::DAEMON ), MCU_MULTICAST_MSG, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	return TRUE;	
}

#endif // !defined(AFX_MTSSN_H__AB423FC4_7512_4B60_9269_742737980FBF__INCLUDED_)
