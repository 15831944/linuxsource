/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : dsiguard.cpp
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

#include "dsiguard.h"
#include "evagtsvc.h"
#include "mcuconst.h"
#include "dsiagent.h"


CBoardGuardApp	g_cBrdGuardApp;   //���������Ӧ��ʵ��

//���캯��
CBoardGuard::CBoardGuard()
{
	memset(m_byE1Alarm, 0, sizeof(m_byE1Alarm));
#ifndef WIN32
	memset(&m_tLedState, 0, sizeof(m_tLedState));
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

    04/11/11    3.5         �� ��          �½ӿڵ��޸�
====================================================================*/
void CBoardGuard::ProcGuardStateScan(CMessage* const pcMsg)
{
#ifndef WIN32
	u8 byLoop;
    TBrdDSIAlarmAll * ptBrdE1Alarm;
	u8 byOldE1Alarm;
	u8 byBuf[2];
	TBrdDSILedStateDesc * ptLedState;

    TBrdAlarmState tBrdAlarmState;
    TBrdLedState tBrdLedState;

	log( LOGLVL_DEBUG2, "Receive Scan Board Message \n");

    BrdAlarmStateScan( &tBrdAlarmState );
    ptBrdE1Alarm = &tBrdAlarmState.nlunion.tBrdDSIAlarmAll;

	for( byLoop=0; byLoop<4; byLoop++)
	{
		byOldE1Alarm = m_byE1Alarm[byLoop];
		
		//E1ʧͬ��
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RLOS != ((m_byE1Alarm[byLoop] & 0x01)==0x01) )
			m_byE1Alarm[byLoop] ^= 0x01;
		//E1ʧ�ز�
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RCL != ((m_byE1Alarm[byLoop] & 0x02)==0x02) )
			m_byE1Alarm[byLoop] ^= 0x02;
		//E1Զ�˸澯
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RRA != ((m_byE1Alarm[byLoop] & 0x04)==0x04) )
			m_byE1Alarm[byLoop] ^= 0x04;
		//E1ȫ1
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RUA1 != ((m_byE1Alarm[byLoop] & 0x08)==0x08) )
			m_byE1Alarm[byLoop] ^= 0x08;
		//CRC��֡����
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RCMF != ((m_byE1Alarm[byLoop] & 0x10)==0x10) )
			m_byE1Alarm[byLoop] ^= 0x10;
		//��·��֡����
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RMF != ((m_byE1Alarm[byLoop] & 0x20)==0x20) )
			m_byE1Alarm[byLoop] ^= 0x20;
		
		if( byOldE1Alarm != m_byE1Alarm[byLoop] )
		{
			byBuf[0] = byLoop;
			byBuf[1] = m_byE1Alarm[byLoop];
			post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_LINK_STATUS, byBuf, sizeof(byBuf) );
		}

        BrdQueryLedState( &tBrdLedState );
        ptLedState = &tBrdLedState.nlunion.tBrdDSILedState;
		if( memcmp( ptLedState, &m_tLedState, sizeof(TBrdDRILedStateDesc)) != 0 )
		{
			memcpy( &m_tLedState, ptLedState, sizeof(TBrdDRILedStateDesc) );
			post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_LED_STATUS, &m_tLedState, sizeof(TBrdDRILedStateDesc) );
		}
	}
#endif	
	//�������ö�ʱ��
	SetTimer( BRDAGENT_SCAN_STATE_TIMER, SCAN_STATE_TIME_OUT );

	return;
}

