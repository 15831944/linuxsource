/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : criGuard.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ����״̬���
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/25  1.0         jianghy       ����
   2004/11/11  3.5         �� ��         �½ӿڵ��޸�
******************************************************************************/
#include "criguard.h"
#include "evagtsvc.h"
#include "mcuconst.h"
#include "criagent.h"


CBoardGuardApp	g_cBrdGuardApp;   //���������Ӧ��ʵ��

//���캯��
CBoardGuard::CBoardGuard()
{
	m_byCriAlarm = 0;
#ifndef WIN32
	memset( &m_tLedState, 0, sizeof(m_tLedState) );
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
    04/11/11    3.5         �� ��         �½ӿڵ��޸�
====================================================================*/
void CBoardGuard::ProcGuardStateScan(CMessage* const pcMsg)
{
#ifndef WIN32
	u8 byOldCriAlarm;

    TBrdCRIAlarmAll * ptCriAlarm;
    TBrdAlarmState tBrdAlarmState;

    TBrdCRILedStateDesc * ptLedState;
    TBrdLedState tBrdLedState;

	log( LOGLVL_DEBUG2, "Receive Scan Board Message \n");

    BrdAlarmStateScan( &tBrdAlarmState );
    ptCriAlarm = &tBrdAlarmState.nlunion.tBrdCRIAlarmAll;

	byOldCriAlarm = m_byCriAlarm;
	if( ptCriAlarm->bAlarmModuleOffLine != ((m_byCriAlarm & 0x01) == 0x01) )
	{
		m_byCriAlarm ^= 0x01;
	}
    // ���ߣ�ptCriAlarm->bAlarmModuleOffLineΪTRUE
    
	if( byOldCriAlarm != m_byCriAlarm )
	{
		post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_MODULE_STATUS, &m_byCriAlarm, sizeof(m_byCriAlarm) );
	}

    BrdQueryLedState( &tBrdLedState );
    ptLedState = &tBrdLedState.nlunion.tBrdCRILedState;
	if( memcmp( ptLedState, &m_tLedState, sizeof(TBrdCRILedStateDesc)) != 0 )
	{
		memcpy( &m_tLedState, ptLedState, sizeof(TBrdCRILedStateDesc) );
		post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_LED_STATUS, &m_tLedState, sizeof(TBrdCRILedStateDesc) );
	}

#endif	
	//�������ö�ʱ��
	SetTimer( BRDAGENT_SCAN_STATE_TIMER, SCAN_STATE_TIME_OUT );

	return;
}




