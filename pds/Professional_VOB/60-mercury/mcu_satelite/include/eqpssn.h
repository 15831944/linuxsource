/*****************************************************************************
   ģ����      : mcu_new
   �ļ���      : eqpssn.h
   ����ļ�    : eqpssn.cpp
   �ļ�ʵ�ֹ���: ����ỰӦ����ͷ�ļ�
   ����        : Ѧ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2002/08/19  0.9         Ѧ����      ����
   2002/08/20  0.9         LI Yi       ��Ӵ���
******************************************************************************/

#ifndef _MCU_EQPSSN_H
#define _MCU_EQPSSN_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEqpSsnInst : public CInstance  
{
	enum 
	{ 
		STATE_IDLE,
		STATE_NORMAL
	};

public:
	u32		m_dwEqpNode;	//��Ӧ����ڵ��
	u32		m_dwEqpIId;		//�����IId
	u32     m_dwEqpIpAddr;  //����IP��ַ
	u8		m_byEqpId;		//����ID
	u8		m_byEqpType;	//��������
	char    m_achAlias[MAXLEN_EQP_ALIAS];     //�������

protected:
	BOOL32 SendMsgToPeriEqp( u16 wEvent, u8 * const pbyMsg, u16 wLen );

public:
	void ProcEqpDisconnect( const CMessage * pcMsg );
	void ProcEqpMcuRegMsg( const CMessage * pcMsg );
	void ProcEqpToMcuMsg( const CMessage * pcMsg );
	void ProcMcuToEqpMsg( const CMessage * pcMsg );
	void ProcEqpToMcuDaemonConfMsg( const CMessage * pcMsg );
	void DaemonProcAppTaskRequest( const CMessage * pcMsg );
	void InstanceEntry( CMessage * const pcMsg );
	void DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp );
	void InstanceDump( u32 param = 0 );
	void ProcEqpGetMsStatusReq(CMessage* const pcMsg); // ��������ȡ��������״̬
	void ProcMcuDisconnectEqp(CMessage* const pcMsg);	

    //qos msg
    void ProcMcuEqpSetQos(CServMsg &cMsg);
	
	CEqpSsnInst();
	virtual ~CEqpSsnInst();
};


class CEqpConfig
{
public:

public:
	BOOL32 SendMsgToPeriEqpSsn( u8 byPeriEqpId, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	void   BroadcastToAllPeriEqpSsn( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );

	CEqpConfig();
	virtual ~CEqpConfig();
};

typedef zTemplate< CEqpSsnInst, MAXNUM_MCU_PERIEQP, CEqpConfig, sizeof( u8 ) > CEqpSsnApp;

extern CEqpSsnApp	g_cEqpSsnApp;	//����ỰӦ��ʵ��


#endif

// END OF FILE
