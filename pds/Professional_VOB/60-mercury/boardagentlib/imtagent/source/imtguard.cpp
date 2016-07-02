/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : imtguard.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ����״̬���
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/25  1.0         jianghy       ����
   2004/12/05  3.5         �� ��        �½ӿڵ��޸�
******************************************************************************/

#include "imtguard.h"
#include "evagtsvc.h"
#include "mcuconst.h"
#include "imtagent.h"

CBoardGuardApp	g_cBrdGuardApp;   //���������Ӧ��ʵ��

//���캯��
CBoardGuard::CBoardGuard()
{

#ifndef WIN32
#ifdef IMT
	memset( &m_tLedState, 0, sizeof(m_tLedState) );
	memset( &m_tImtBrdAlarm, 0, sizeof(m_tImtBrdAlarm) );
#endif
#endif

	return;
}

//��������
CBoardGuard::~CBoardGuard()
{
	return;
}


/*====================================================================
    ������      ��InstanceEntry
    ����        ��ʵ����Ϣ������ں���������override
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBoardGuard::InstanceEntry(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
	{
		log(LOGLVL_EXCEPTION, "CBoardGuard: The received msg's pointer in the msg entry is NULL!");
		return;
	}

	switch( pcMsg->event )
	{
	case BOARD_GUARD_POWERON:    //������Ϣ
		ProcGuardPowerOn( pcMsg );
		break;

	case BRDAGENT_SCAN_STATE_TIMER:		 //ɨ�趨ʱ������
		ProcGuardStateScan( pcMsg );
		break;

	default:
		break;
	}	
	return;
}


/*====================================================================
    ������      ��ProcGuardPowerOn
    ����        ��GUARDģ��������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBoardGuard::ProcGuardPowerOn(CMessage* const pcMsg)
{
	SetTimer( BRDAGENT_SCAN_STATE_TIMER, SCAN_STATE_TIME_OUT );
	return;
}


/*====================================================================
    ������      ��ProcGuardStateScan
    ����        ������״̬�Ķ�ʱɨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
    04/12/05    3.5         �� ��        �½ӿڵ��޸�
====================================================================*/
void CBoardGuard::ProcGuardStateScan(CMessage* const pcMsg)
{
#ifndef WIN32
#ifdef IMT
	/*���ȸ澯*/
	//TBrdIMTAlarmAll tBrdAlarm;
    TBrdIMTAlarmAll * ptBrdAlarm;
	u8 byBuf[2];
	
    TBrdAlarmState tBrdAlarmState;

	//BrdIMTAlarmStateScan( &tBrdAlarm );
    BrdAlarmStateScan( &tBrdAlarmState );
    ptBrdAlarm = &tBrdAlarmState.nlunion.tBrdIMTAlarmAll;

	if( ptBrdAlarm->bAlarmDSP1FanStop != m_tImtBrdAlarm.bAlarmDSP1FanStop )
	{
		m_tImtBrdAlarm.bAlarmDSP1FanStop = ptBrdAlarm->bAlarmDSP1FanStop;

		byBuf[0] = 0;
		if( m_tImtBrdAlarm.bAlarmDSP1FanStop == TRUE )
		{
			byBuf[1] = 1;
		}
		else
		{
			byBuf[1] = 0;
		}
		post( MAKEIID(AID_MCU_BRDAGENT, 1), MCU_BRD_FAN_STATUS, byBuf, sizeof(byBuf) );
	}

	if( ptBrdAlarm->bAlarmDSP2FanStop != m_tImtBrdAlarm.bAlarmDSP2FanStop )
	{
		m_tImtBrdAlarm.bAlarmDSP2FanStop = ptBrdAlarm->bAlarmDSP2FanStop;

		byBuf[0] = 1;
		if( m_tImtBrdAlarm.bAlarmDSP2FanStop == TRUE )
		{
			byBuf[1] = 1;
		}
		else
		{
			byBuf[1] = 0;
		}
		post( MAKEIID(AID_MCU_BRDAGENT, 1), MCU_BRD_FAN_STATUS, byBuf, sizeof(byBuf) );
	}
	
	if( ptBrdAlarm->bAlarmDSP3FanStop != m_tImtBrdAlarm.bAlarmDSP3FanStop )
	{
		m_tImtBrdAlarm.bAlarmDSP3FanStop = ptBrdAlarm->bAlarmDSP3FanStop;

		byBuf[0] = 1;
		if( m_tImtBrdAlarm.bAlarmDSP3FanStop == TRUE )
		{
			byBuf[1] = 1;
		}
		else
		{
			byBuf[1] = 0;
		}
		post( MAKEIID(AID_MCU_BRDAGENT, 1), MCU_BRD_FAN_STATUS, byBuf, sizeof(byBuf) );
	}
	
	/*��״̬*/    
    TBrdLedState tBrdLedState;
	TBrdIMTLedStateDesc * ptLedState;

	//BrdIMTQueryLedState( &tLedState );
    BrdQueryLedState( &tBrdLedState );
    ptLedState = &tBrdLedState.nlunion.tBrdIMTLedState;
	if( memcmp( ptLedState, &m_tLedState, sizeof(TBrdIMTLedStateDesc)) != 0 )
	{
		memcpy( &m_tLedState, ptLedState, sizeof(TBrdIMTLedStateDesc) );
		post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_LED_STATUS, &m_tLedState, sizeof(TBrdIMTLedStateDesc) );
	}
#endif
#endif	
	//�������ö�ʱ��
	SetTimer( BRDAGENT_SCAN_STATE_TIMER, SCAN_STATE_TIME_OUT );

	return;
}




