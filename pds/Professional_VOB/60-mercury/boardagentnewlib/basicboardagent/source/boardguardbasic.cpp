/*****************************************************************************
   ģ����      : Board Agent Basic
   �ļ���      : boardguardbasic.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ������������ʵ��
   ����        : �ܹ��
   �汾        : V4.0  Copyright(C) 2001-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2007/08/20  4.0         �ܹ��       ����
******************************************************************************/
#include "boardguardbasic.h"

CBoardGuardApp	g_cBrdGuardApp;

//���캯��
CBBoardGuard::CBBoardGuard()
{
	memset( &m_tBrdAlarmState, 0, sizeof(m_tBrdAlarmState) );
	memset( &m_tBrdLedState, 0, sizeof(m_tBrdLedState) );
	m_byBrdTempStatus = 0;
	m_byBrdCpuStatus = 0;
	m_byBrdId = 0;

	return;
}

//��������
CBBoardGuard::~CBBoardGuard()
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
    2007/08/20  4.0         �ܹ��       ����
====================================================================*/
void CBBoardGuard::InstanceEntry(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
	{
		OspPrintf(TRUE, FALSE, "CBoardGuard: The received msg's pointer in the msg entry is NULL!");
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
    2007/08/20  4.0         �ܹ��       ����
====================================================================*/
void CBBoardGuard::ProcGuardPowerOn(CMessage* const pcMsg)
{
	m_byBrdId = *(u8*)pcMsg->content;
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
    2007/08/20  4.0         �ܹ��       ����
====================================================================*/
void CBBoardGuard::ProcGuardStateScan(CMessage* const pcMsg)
{
    TBrdAlarmState tBrdAlarmState;
    TBrdLedState tBrdLedState;

	log( LOGLVL_DEBUG2, "Receive Scan Board Message \n");

    if( OK == BrdAlarmStateScan( &tBrdAlarmState ))
	{
		if( memcmp( &tBrdAlarmState, &m_tBrdAlarmState, sizeof(TBrdAlarmState)) != 0 )
		{
			memcpy( &m_tBrdAlarmState, &tBrdAlarmState, sizeof(TBrdAlarmState) );
			post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_MODULE_STATUS, &m_tBrdAlarmState, sizeof(TBrdAlarmState) );
		}
	}
	else
	{
		OspPrintf(TRUE,FALSE,"[ProcGuardStateScan] Error Get board alarm state.srcnode.%d\n",pcMsg!=NULL?pcMsg->srcnode:0);
	}

    // ɨ��led״̬
    BrdQueryLedState( &tBrdLedState );
	if( memcmp( &tBrdLedState, &m_tBrdLedState, sizeof(TBrdLedState)) != 0 )
	{
		memcpy( &m_tBrdLedState, &tBrdLedState, sizeof(TBrdLedState) );
		post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_LED_STATUS, &m_tBrdLedState, sizeof(TBrdLedState) );
	}


    // ���°忨�������ϱ�����״̬���Ժ���𲽷ſ���[2012/05/30 liaokang]
    if( m_byBrdId == BRD_TYPE_IS22 )
	{
        // ȡǰ���ڣ�0 ������״̬  
        u8  byEthId = 0;       // ����
        u8  byLink =0;         // link
        u8  byEthAutoNeg = 0;  // Э��״̬
        u8  byEthDuplex = 0;   // ˫��״̬
        u32 dwEthSpeed = 0;    // �ӿ�����Mbps
#ifdef _LINUX12_   // Ŀǰ��brdwrapper.h����TBrdEthInfo
        TBrdEthInfo  tBrdFrontEthPortState;
        // byLink: 0-link down, 1-link up �������ٵ��� BrdGetEthNegStat������һֱˢ�����ӡ
        BrdGetEthLinkStat( byEthId, &byLink );
        if ( 1 == byLink )
        {
            BrdGetEthNegStat( byEthId, &byEthAutoNeg, &byEthDuplex, &dwEthSpeed);
        }
        tBrdFrontEthPortState.Link = byLink;
        tBrdFrontEthPortState.AutoNeg = byEthAutoNeg;
        tBrdFrontEthPortState.Duplex = byEthDuplex;
        tBrdFrontEthPortState.Speed = dwEthSpeed;
        if( memcmp( &tBrdFrontEthPortState, &m_tBrdFrontEthPortState, sizeof(TBrdEthInfo)) != 0 )
        {
            memcpy( &m_tBrdFrontEthPortState, &tBrdFrontEthPortState, sizeof(TBrdEthInfo) );
            post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_ETHPORT_STATUS, &m_tBrdFrontEthPortState, sizeof(TBrdEthInfo) );
        }
#endif
    }

	/*���°忨����,��Ҫ�ϱ��¶Ⱥ�CPU�쳣���Ժ���𲽷ſ���[10/27/2011 chendaiwei]	*/
	if(m_byBrdId == BRD_TYPE_DRI || 
		m_byBrdId == BRD_TYPE_DRI2 || 
		m_byBrdId == BRD_TYPE_MPU2 ||
		m_byBrdId == BRD_TYPE_MPU2ECARD ||
		m_byBrdId == BRD_TYPE_HDU2 ||
		m_byBrdId == BRD_TYPE_HDU2_L ||
		m_byBrdId == BRD_TYPE_HDU2_S)
	{
	#ifdef _LINUX12_
		tempalarm tSensoralarm;
		memset(&tSensoralarm,0,sizeof(tSensoralarm));
		if ( OK == get_lm75temp_alarm(&tSensoralarm) )
		{
			if ( (tSensoralarm.temp_alarm == BRD_STATUS_ABNORMAL) && (m_byBrdTempStatus == BRD_STATUS_NORMAL) )    
			{
				OspPrintf(TRUE,FALSE,"[ProcGuardStateScan] The board's temperature is high enough\n");
				
				m_byBrdTempStatus = BRD_STATUS_ABNORMAL;
				post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_TEMPERATURE_STATUS_NOTIF, &m_byBrdTempStatus, sizeof(m_byBrdTempStatus) );
			}
			
			if ( (tSensoralarm.temp_alarm == BRD_STATUS_NORMAL) && (m_byBrdTempStatus == BRD_STATUS_ABNORMAL))
			{
				m_byBrdTempStatus = BRD_STATUS_NORMAL;
				post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_TEMPERATURE_STATUS_NOTIF, &m_byBrdTempStatus, sizeof(m_byBrdTempStatus) );
			}
		}
		else
		{
			OspPrintf(TRUE,FALSE,"[ProcGuardStateScan] Error Get board temperature.\n");
		}
	#endif
		
		// ��ȡCPUռ������Ϣ [10/25/2011 chendaiwei]
		u8			   byCpuAllocRate = 0;
		TOspCpuInfo    tCpuInfo;
		
		if ( OspGetCpuInfo(&tCpuInfo) )
		{
			byCpuAllocRate = 100 - tCpuInfo.m_byIdlePercent;
			
			if ( byCpuAllocRate > BRD_CPU_THRESHOLD && m_byBrdCpuStatus == BRD_STATUS_NORMAL)    // Cpuռ���ʳ���85%
			{
				OspPrintf(TRUE,FALSE,"[ProcGuardStateScan] The Board's cpu is not enough: %d\n", byCpuAllocRate);
				m_byBrdCpuStatus = BRD_STATUS_ABNORMAL;
				post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_CPU_STATUS_NOTIF, &m_byBrdCpuStatus, sizeof(m_byBrdCpuStatus) );
			}
        
			if ( (byCpuAllocRate <= BRD_CPU_THRESHOLD) && ( m_byBrdCpuStatus == BRD_STATUS_ABNORMAL) )
			{
				m_byBrdCpuStatus = BRD_STATUS_NORMAL;
				post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_CPU_STATUS_NOTIF, &m_byBrdCpuStatus, sizeof(m_byBrdCpuStatus) );
			}       
		}
		else
		{
			OspPrintf(TRUE,FALSE,"[ProcGuardStateScan] Error Get Board Cpu Percentage.\n");
		}	
	}

	//�������ö�ʱ��
	SetTimer( BRDAGENT_SCAN_STATE_TIMER, SCAN_STATE_TIME_OUT );

	return;
}
