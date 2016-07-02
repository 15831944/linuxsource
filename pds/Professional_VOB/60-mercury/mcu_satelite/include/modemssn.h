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

******************************************************************************/

// mtssn.h: interface for the CMtSsnInst class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MODEMSSN_H_)
#define _MODEMSSN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vctopo.h"
#include "evmcu.h"
#include "modemcmdproxy.h"


extern TMcuTopo	g_atMcuTopo[MAXNUM_TOPO_MCU];
extern u8		g_wMcuTopoNum;
extern TMtTopo		g_atMtTopo[MAXNUM_TOPO_MT];	//����ȫ����MT������Ϣ
extern u16		g_wMtTopoNum;		//����ȫ����MT��Ŀ

//���ն�roundTripDelay���ֵ���룩
#define TIMERESULT	        5
#define TIMESTATUS          15
#define MODE_RECEIVE        1
#define MODE_SEND           2
#define MODE_STOPSEND       4
#define MODE_RELEASE        16
#define MODE_SENDSRC        32
#define MODE_REVSRC         64


//�Ự��ʱ������ϵ��

class CModemSsnInst : public CInstance  
{
public:
	enum 
	{ 
		STATE_IDLE		= 0,
		STATE_CONNECTED	= 1,	//���ӽ�����δ�������״̬
		STATE_RECEIVE	= 2,	//����״̬
		STATE_SENDRECEIVE	= 3,	//���ͽ���״̬
		STATE_SEND = 4
	};

	enum
	{
		TIMER_CHECK_RESULT,	//������ȴ������
		TIMER_CHECK_STATUS
	};

protected:
	ModemCmdProxy m_cModem;


	u32		m_dwMtNode;		//��Ӧ�ն˽ڵ��
	u16		m_wMtId;		//��Ӧ�ն˺�
	u32		m_dwIpAddr;
	u16		m_wPort;
	u8		m_byType;
	u8      m_bySetMode;
	u8      m_byConfIdx;

	u32		m_dwRecvMode;
	u32		m_dwSendMode;
	u32		m_dwSendFrq;
	u32     m_dwRecFrq;
	u32     m_dwSendRate;
	u32     m_dwRecRate;
	BOOL	m_bCheckResult;
	BOOL	m_bCheckStatus;

public:
	void Reset()
	{
		m_dwMtNode = 0;		//��Ӧ�ն˽ڵ��
		m_wMtId = 0;		//��Ӧ�ն˺�
		m_dwIpAddr = 0;
		m_wPort = 0;
		m_byType = 0;
		m_bySetMode = 0;
		
		m_dwRecvMode = 0;
		m_dwSendMode = 0;
		m_dwSendFrq = 0;
		m_dwRecFrq = 0;
		m_dwSendRate = 0;
		m_dwRecRate = 0;
		m_bCheckStatus = FALSE;
		m_bCheckResult = FALSE;
	}
	void ProcMcuModeSetSrc( const CMessage *pcMsg );
	void ProcModemMcuRegReq( const CMessage * pcMsg );
	void ProcMcuModemReceive( const CMessage * pcMsg );
	void ProcMcuModemSend( const CMessage * pcMsg );
	void ProcMcuModemStopSend( const CMessage * pcMsg );	
	void ProcMcuModemRelease( const CMessage * pcMsg );
	void ProcMcuModemSetBitRate( const CMessage * pcMsg );
	void PostMessage2Conf( u16 wInstanceId, u16 wEvent,const CServMsg & cServMsg );
		
	void CheckResult( const CMessage * pcMsg );
	void CheckStatus( const CMessage * pcMsg );
	void DaemonProcAppTaskRequest( const CMessage * pcMsg );
	void DaemonProcPowerOn( const CMessage * pcMsg, CApp * pcApp );
	void InstanceEntry( CMessage * const pcMsg );
	void DaemonInstanceEntry( CMessage * const pcMsg, CApp * pcApp );
	void InstanceDump( u32 param = 0 );

	void MdmssnLog(s8 * pszFmt, ...);
	CModemSsnInst();
	virtual ~CModemSsnInst();
};


class CModemConfig
{
public:
	BOOL SendMsgToModemSsn( u16 wMtId, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	void BroadcastToAllModemSsn( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	CModemConfig();
	virtual ~CModemConfig();
};

typedef zTemplate< CModemSsnInst, MAXNUM_MCU_MT+MAXNUM_MCU_MODEM+1, CModemConfig, sizeof( u8 ) > CModemSsnApp;

extern CModemSsnApp	g_cModemSsnApp;	//�¼�MT�ỰӦ��ʵ��


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
inline void CModemConfig::BroadcastToAllModemSsn( u16 wEvent, u8 * const pbyMsg, u16 wLen )
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
inline BOOL CModemConfig::SendMsgToModemSsn( u16 wMtId, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	if( wMtId == NULL )	//��������IDLEʵ��
	{
		::OspPost( MAKEIID( AID_MCU_MODEMSSN, CInstance::PENDING ), wEvent, pbyMsg, wLen );
		return( TRUE );
	}
	else if( wMtId >= MAXNUM_MCU_MT+16 )
	{
		return( FALSE );
	}
	else	//����Alias
	{
		OspPost( MAKEIID( AID_MCU_MODEMSSN, wMtId ), wEvent, pbyMsg, wLen );
		return( TRUE );
	}
}


API void pmdmmsg(void);
API void npmdmmsg(void);


#endif

