/*****************************************************************************
   ģ����      : Board Guard
   �ļ���      : BoardGuard.cpp
   ����ļ�    : brdguard.h
   �ļ�ʵ�ֹ���: ������������ʵ�֣�����״̬���
   ����        : jianghy
   �汾        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���          �޸�����
   2003/08/25  1.0         jianghy           ����
   2004/11/10  3.6         libo           �½ӿ��޸�
   2004/11/29  3.6         libo           ��ֲ��Linux
   2005/08/17  4.0         liuhuiyun      4.0
   2006/04/30  4.0		   liuhuiyun      ���Ӷ�Trap���͵ı���
   2011/05/03  4.0         miaoqingsong   ���MPC2�澯֧��
******************************************************************************/
#include "brdguard.h"
#include "agtcomm.h"
#include "configureagent.h"
#include "agentinterface.h"

// #ifdef _LINUX_
// #include <sys/sysinfo.h>
// #endif

/*lint -save -esym(714,g_bAlarmState)*/
// MCU������Ӧ��ʵ��
CBoardGuardApp	g_cBrdGuardApp;
//BOOL32          g_bAlarmState = FALSE;      // ȫ�ֱ��������ڱ���»����Դ�ͷ��ȸ澯״̬

// ���캯��
CBoardGuard::CBoardGuard()
{
	m_byPowerStatus = 0;
	m_byNetSyncStatus = 0;
	m_byPowerFanStatus = 0;
	m_byBoxFanStatus = 0;           
	m_byMPCCpuStatus = 0;
	m_byMPC2TempStatus = 0;
	m_byPowerTempStatus = 0;
	m_byMPCMemoryStatus = 0;
	m_dwSDHStatus = 0;
	m_byOldRemoteMpcState = 255;
	m_b8KASlamOn = FALSE;
#ifdef _8KI_
	m_dwCpuFan = 0;
	m_byCpuTemp = 0;
#endif
}

// ��������
CBoardGuard::~CBoardGuard()
{
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
		OspPrintf(TRUE, FALSE,  "[CBoardGuard]: Error input parameter.!");
		return;
	}

	switch( pcMsg->event )
	{
	case BOARD_GUARD_POWERON:       //������Ϣ
		ProcGuardPowerOn();
		break;

	case MCUAGENT_SCAN_STATE_TIMER:	//ɨ�趨ʱ������
		ProcGuardStateScan();
		break;

    case AGT_SVC_REBOOT:            // ������mcuҵ��������
        ProcReboot(pcMsg);
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
void CBoardGuard::ProcGuardPowerOn()
{
	SetTimer( MCUAGENT_SCAN_STATE_TIMER, SCAN_STATE_TIME_OUT ); // �ȴ�ϵͳ��̬

	return;
}

/*=============================================================================
  �� �� ���� ProcReboot
  ��    �ܣ� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardGuard::ProcReboot(CMessage* const pMsg)
{
    if(NULL != pMsg)
    {

#if ( defined(_8KE_) || defined(_8KH_) || defined(_8KI_)) && defined(_LINUX_)
		//��ҵ�������������
		McuAgentQuit(FALSE);

	#ifndef _8KH_
		OspQuit();
	#endif
		
		OspDelay(100);     

//		s8    achProfileName[MAXLEN_MCU_FILEPATH] = {0};
//		sprintf(achProfileName, "%s/%s", DIR_DATA, RUNSTATUS_8KE_CHKFILENAME);
//		s32 dwRunSt = 0;
//		SetRegKeyInt( achProfileName, SECTION_RUNSTATUS, KEY_MCU8KE, dwRunSt );
		//execute reboot
		s8   chCmdline[256] =  {0};   
		sprintf(chCmdline,  "reboot");
		system(chCmdline);

#elif defined(_LINUX_)	
        printf("[ProcReboot] Reboot the mcu\n");

        OspQuit();
        
        OspDelay( 1000 );

        BrdHwReset(); // restart
#else
#endif
#ifdef WIN32
        OspPrintf(TRUE, FALSE, "[ProcReboot] Recv reboot message\n");
#endif
    }
    return;
}
/*==============================================================================================
    ������      ��ProcGuardStateScan
    ����        ������״̬�Ķ�ʱɨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
------------------------------------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
    04/11/10    3.6         libo          �½ӿڵ��޸�
	06/04/28    4.0         john          ���Ӷ��Ƿ������ܷ��������ж�
	10/12/15    4.0         ������        ���ӻ�����ȡ�MPC���ڴ��CPUʹ���ʡ�MPC2���¶��쳣�澯
	11/05/03    4.0         ������        ���ӵ�Դ�͵�Դ�����쳣�澯
================================================================================================*/
void CBoardGuard::ProcGuardStateScan(void)
{

#ifndef WIN32
    TBrdLedState tBrdLedState;

#ifndef _MINIMCU_

    TBrdAlarmState      tBrdAlarm;
    TBrdMPCAlarmVeneer* ptMpcAlarm = NULL;
	TBrdMPCAlarmSDH*    ptSDHAlarm = NULL;
	TPowerStatus        tPowerStatus;
	TPowerFanStatus     tPowerFanStatus;
	
	u8 byNetSyncMode;
	u32 dwOldSDHStatus;
		
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );

    u8 byRet = BrdMPCQueryAnotherMPCState();

	TBrdStatus tBrdStatus;
	if( tMPCInfo.GetIsHaveOtherMpc() )  // ��ֹԶ��Mpc������ʱ����һ����澯
	{
		if( byRet != m_byOldRemoteMpcState ) // ״̬�ı�
		{
			//  [1/21/2011 chendaiwei]֧��MPC2
#ifdef _LINUX12_
			tBrdStatus.byType = BRD_TYPE_MPC2;
#else
            tBrdStatus.byType   = BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;
#endif
			tBrdStatus.byLayer  = tMPCInfo.GetOtherMpcLayer();
			tBrdStatus.bySlot   = tMPCInfo.GetOtherMpcSlot(); 
			tBrdStatus.byStatus = g_cCfgParse.BrdStatusHW2Agt(byRet);
            tBrdStatus.byOsType = tMPCInfo.GetOSType();

			post( MAKEIID(AID_MCU_AGENT, 1), MCU_AGT_BOARD_STATUSCHANGE, 
				                 (u8*)&tBrdStatus, sizeof(tBrdStatus) );

            tMPCInfo.SetOtherMpcStatus( BOARD_STATUS_INLINE );
            g_cCfgParse.SetMPCInfo( tMPCInfo );
			m_byOldRemoteMpcState = g_cCfgParse.BrdStatusHW2Agt(byRet);
			Agtlog(LOG_VERBOSE, "[ProcGuardStateScan] The Remote Mpc(%d, %d)'s state is changed (%d ---0: In, 1: Out).\n", 
                                                       tMPCInfo.GetOtherMpcLayer(), tMPCInfo.GetOtherMpcSlot(), byRet );
		}
	}

#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)

	// ɨ�赥���״̬
    if( g_cCfgParse.IsMpcActive() )
    {   
	    // MPC�澯
        BrdAlarmStateScan(&tBrdAlarm);

	    // ��48V��Դ״̬��ת
        ptMpcAlarm = &tBrdAlarm.nlunion.tBrdMPCAlarmAll.tBrdMPCAlarmVeneer;
		Agtlog(LOG_INFORM,"[ProcGuardStateScan]MPCBrdState:Power.bAlarmPowerDC48VLDown:%d,\
			Power.bAlarmPowerDC5VLDown:%d,Power.bAlarmPowerDC48VRDown:%d,Power.bAlarmPowerDC5VRDown:%d\n",
			ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC48VLDown,
			ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC5VLDown,
			ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC48VRDown,
			ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC5VRDown);
		Agtlog(LOG_INFORM,"[ProcGuardStateScan]MPCBrdState:PowerFan.bAlarmPowerFanLLDown:%d,\
			PowerFan.bAlarmPowerFanLRDown:%d,PowerFan.bAlarmPowerFanRLDown:%d,PowerFan.bAlarmPowerFanRRDown:%d\n",
			ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanLLDown,
			ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanLRDown,
			ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanRLDown,
			ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanRRDown);

	    if (ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC48VLDown != (BOOL32)((m_byPowerStatus & 0x08) == 0x08))
	    {
		    m_byPowerStatus ^= 0x08;

		    tPowerStatus.bySlot = POWER_LEFT;
		    tPowerStatus.byType = POWER_48V;
		    tPowerStatus.byStatus = m_byPowerStatus & 0x08;
			post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_STATUS, &tPowerStatus, sizeof(tPowerStatus) );
	    }

        // ��5v
	    if (ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC5VLDown != (BOOL32)((m_byPowerStatus & 0x04) == 0x04))
	    {
		    m_byPowerStatus ^= 0x04;

		    tPowerStatus.bySlot = POWER_LEFT;
		    tPowerStatus.byType = POWER_5V;
		    tPowerStatus.byStatus = m_byPowerStatus & 0x04;
			post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_STATUS, &tPowerStatus, sizeof(tPowerStatus) );
	    }

	    //��48v
	    if (ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC48VRDown != (BOOL32)((m_byPowerStatus & 0x02) == 0x02))
	    {
		    //״̬��ת
		    m_byPowerStatus ^= 0x02;

		    tPowerStatus.bySlot = POWER_RIGHT;
		    tPowerStatus.byType = POWER_48V;
		    tPowerStatus.byStatus = m_byPowerStatus & 0x02;
			post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_STATUS, &tPowerStatus, sizeof(tPowerStatus) );
	    }

	    // ��5v
	    if (ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC5VRDown != (BOOL32)((m_byPowerStatus & 0x01) == 0x01)) 
	    {
		    //״̬��ת
		    m_byPowerStatus ^= 0x01;

		    tPowerStatus.bySlot = POWER_RIGHT;
		    tPowerStatus.byType = POWER_5V;
		    tPowerStatus.byStatus = m_byPowerStatus & 0x01;
			post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_STATUS, &tPowerStatus, sizeof(tPowerStatus) );
	    }

	    // ��Դ����״̬
	    // ���Դ�����ͣת
	    if (ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanLLDown != (BOOL32)((m_byPowerFanStatus & 0x08) == 0x08))
	    {
		    m_byPowerFanStatus ^= 0x08;

		    tPowerFanStatus.bySlot = POWER_LEFT;
		    tPowerFanStatus.byFanPos = FAN_LEFT;
		    tPowerFanStatus.byStatus = m_byPowerFanStatus & 0x08;
			post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_FAN_STATUS, &tPowerFanStatus, sizeof(tPowerFanStatus) );
	    }

	    // ���Դ�ҷ���ͣת
	    if (ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanLRDown != (BOOL32)((m_byPowerFanStatus & 0x04) == 0x04))
	    {
		    m_byPowerFanStatus ^= 0x04;

		    tPowerFanStatus.bySlot = POWER_LEFT;
		    tPowerFanStatus.byFanPos = FAN_RIGHT;
		    tPowerFanStatus.byStatus = m_byPowerFanStatus & 0x04;
            post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_FAN_STATUS, &tPowerFanStatus, sizeof(tPowerFanStatus) );
	    }

	    // �ҵ�Դ�����ͣת
	    if (ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanRLDown != (BOOL32)((m_byPowerFanStatus & 0x02) == 0x02))
	    {
		    // ״̬��ת
		    m_byPowerFanStatus ^= 0x02;

		    tPowerFanStatus.bySlot = POWER_RIGHT;
		    tPowerFanStatus.byFanPos = FAN_LEFT;
		    tPowerFanStatus.byStatus = m_byPowerFanStatus & 0x02;
            post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_FAN_STATUS, &tPowerFanStatus, sizeof(tPowerFanStatus) );
	    }

	    // �ҵ�Դ�ҷ���ͣת
	    if (ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanRRDown != (BOOL32)((m_byPowerFanStatus & 0x01) == 0x01)) 
	    {
		    // ״̬��ת
		    m_byPowerFanStatus ^= 0x01;

		    tPowerFanStatus.bySlot = POWER_RIGHT;
		    tPowerFanStatus.byFanPos = FAN_RIGHT;
		    tPowerFanStatus.byStatus = m_byPowerFanStatus & 0x01;
            post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_FAN_STATUS, &tPowerFanStatus, sizeof(tPowerFanStatus) );
	    }

		BOOL32 bCur8KASlamOn = FALSE;
		//�жϵ�ǰ��Դ�������Ƿ�����쳣
		if ( (BOOL32)(m_byPowerStatus & 0x0F)  || (BOOL32)(m_byPowerFanStatus & 0x0F) )
		{
			bCur8KASlamOn = TRUE;
		}
		//Ҫ����alarm���ҵ�ǰ��û����
		if ( TRUE == bCur8KASlamOn && FALSE == m_b8KASlamOn )
		{
// 			BrdLedStatusSet(LED_BOARD_LED_SALM, BRD_LED_ON);   // ��������澯��
// 			BrdMPCLedBoardSpeakerSet(LED_BOARD_SPK_ON);        // ������������
			m_b8KASlamOn = TRUE;
			Agtlog(LOG_INFORM,"[ProcGuardStateScan] Turn On  SAlarm !\n");
		}
		else if ( FALSE == bCur8KASlamOn && TRUE == m_b8KASlamOn )//Ҫ��alarm���ҵ�ǰ�������ŵ�
		{
			BrdLedStatusSet(LED_BOARD_LED_SALM, BRD_LED_OFF);      // �������澯��
 			BrdMPCLedBoardSpeakerSet(LED_BOARD_SPK_OFF);           // �رջ��������
			m_b8KASlamOn = FALSE;
			Agtlog(LOG_INFORM,"[ProcGuardStateScan] Turn Off  SAlarm !\n");
		}


	    
       // ��ͬ��, �ڸ��ٵ�ʱ�����
	    byNetSyncMode = BrdMPCQueryNetSyncMode();
	    if( byNetSyncMode != SYNC_MODE_FREERUN )
	    {
		    if (ptMpcAlarm->bAlarmNetClkLockFailed != (BOOL32)((m_byNetSyncStatus & 0x01) == 0x01))
		    {
			    // ��ת
			    m_byNetSyncStatus ^= 0x01;
				// ֪ͨ����
				post( MAKEIID(AID_MCU_AGENT, 1), MCU_AGT_SYNCSOURCE_STATUSCHANGE, 
						         &m_byNetSyncStatus, sizeof(m_byNetSyncStatus) );
		    }
	    }

	    // SDH�澯
        ptSDHAlarm = &tBrdAlarm.nlunion.tBrdMPCAlarmAll.tBrdMPCAlarmSDH;
	    dwOldSDHStatus = m_dwSDHStatus;

	    // ���źŶ�ʧ
	    if (ptSDHAlarm->bAlarmLOS != (BOOL32)((m_dwSDHStatus & 0x00000001) == 0x00000001))
		    m_dwSDHStatus ^= 0x00000001;

	    // ֡��ʧ
	    if (ptSDHAlarm->bAlarmLOF != (BOOL32)((m_dwSDHStatus & 0x00000002) == 0x00000002))
		    m_dwSDHStatus ^= 0x00000002;

	    // ֡ʧ��
	    if (ptSDHAlarm->bAlarmOOF != (BOOL32)((m_dwSDHStatus & 0x00000004) == 0x00000004))
		    m_dwSDHStatus ^= 0x00000004;

	    // ��֡��ʧ
	    if (ptSDHAlarm->bAlarmLOM != (BOOL32)((m_dwSDHStatus & 0x00000008) == 0x00000008))
		    m_dwSDHStatus ^= 0x00000008;

	    // ����Ԫָ�붪ʧ
	    if (ptSDHAlarm->bAlarmAU_LOP != (BOOL32)((m_dwSDHStatus & 0x00000010) == 0x00000010))
		    m_dwSDHStatus ^= 0x00000010;

	    // ���ö�Զ��ȱ��ָʾ
	    if (ptSDHAlarm->bAlarmMS_RDI != (BOOL32)((m_dwSDHStatus & 0x00000020) == 0x00000020))
		    m_dwSDHStatus ^= 0x00000020;

	    // ���öα���ָʾ
	    if (ptSDHAlarm->bAlarmMS_AIS != (BOOL32)((m_dwSDHStatus & 0x00000040) == 0x00000040))
		    m_dwSDHStatus ^= 0x00000040;

	    // �߽�ͨ��Զ��ȱ��ָʾ
	    if (ptSDHAlarm->bAlarmHP_RDI != (BOOL32)((m_dwSDHStatus & 0x00000080) == 0x00000080))
		    m_dwSDHStatus ^= 0x00000080;

	    // �߽�ͨ������ָʾ
	    if (ptSDHAlarm->bAlarmHP_AIS != (BOOL32)((m_dwSDHStatus & 0x00000100) == 0x00000100))
		    m_dwSDHStatus ^= 0x00000100;

	    // ������ͨ���ټ��ֽڲ�ƥ��
	    if (ptSDHAlarm->bAlarmRS_TIM != (BOOL32)((m_dwSDHStatus & 0x00000200) == 0x00000200))
		    m_dwSDHStatus ^= 0x00000200;

	    // �߽�ͨ���ټ��ֽڲ�ƥ��
	    if (ptSDHAlarm->bAlarmHP_TIM != (BOOL32)((m_dwSDHStatus & 0x00000400) == 0x00000400))
		    m_dwSDHStatus ^= 0x00000400;

	    // �߽�ͨ���źű���ֽ�δװ��
	    if (ptSDHAlarm->bAlarmHP_UNEQ != (BOOL32)((m_dwSDHStatus & 0x00000800) == 0x00000800))
		    m_dwSDHStatus ^= 0x00000800;

	    // �߽�ͨ���źű���ֽڲ�ƥ��
	    if (ptSDHAlarm->bAlarmHP_PLSM != (BOOL32)((m_dwSDHStatus & 0x00001000) == 0x00001000))
		    m_dwSDHStatus ^= 0x00001000;

	    // ֧·��Ԫָ�붪ʧ
	    if (ptSDHAlarm->bAlarmTU_LOP != (BOOL32)((m_dwSDHStatus & 0x00002000) == 0x00002000))
		    m_dwSDHStatus ^= 0x00002000;

	    // �ͽ�ͨ��Զ��ȱ��ָʾ
	    if (ptSDHAlarm->bAlarmLP_RDI != (BOOL32)((m_dwSDHStatus & 0x00004000) == 0x00004000))
		    m_dwSDHStatus ^= 0x00004000;

	    // �ͽ�ͨ������ָʾ
	    if (ptSDHAlarm->bAlarmLP_AIS != (BOOL32)((m_dwSDHStatus & 0x00008000) == 0x00008000))
		    m_dwSDHStatus ^= 0x00008000;

	    // �ͽ�ͨ���ټ��ֽڲ�ƥ��
	    if (ptSDHAlarm->bAlarmLP_TIM != (BOOL32)((m_dwSDHStatus & 0x00010000) == 0x00010000))
		    m_dwSDHStatus ^= 0x00010000;

	    // �ͽ�ͨ���źű���ֽڲ�ƥ��
	    if (ptSDHAlarm->bAlarmLP_PLSM != (BOOL32)((m_dwSDHStatus & 0x00020000) == 0x00020000))
		    m_dwSDHStatus ^= 0x00020000;
	    // ��״̬�ı�
	    if (dwOldSDHStatus != m_dwSDHStatus)
	    {		
			post( MAKEIID(AID_MCU_AGENT), SVC_AGT_SDH_STATUS, &m_dwSDHStatus, sizeof(m_dwSDHStatus) );
	    }
    }
#endif // end !_8KE_
#endif // !_MINIMCU_


#if defined(_8KI_) && defined(_LINUX_)
	TBrdCtrlInfo tBreCtrlInfo;
	memset( &tBreCtrlInfo,0,sizeof(tBreCtrlInfo) );
	u8 byOldCpuFanState = m_dwCpuFan >= 100 ? 0 : 1;//����ת�ٴ���100Ϊ����ֵ
	u8 byOldCpuTempState = m_byCpuTemp <= 85 ? 0 : 1;//cpu�¶�С�ڵ���85��Ϊ����ֵ
	if( OK != GetBrdCtrlInfo(&tBreCtrlInfo) )
	{
		Agtlog(LOG_ERROR, "[ProcGuardStateScan] BrdQueryLedState failed !\n");
	}

	
#else
	// led status
    if (OK != BrdQueryLedState(&tBrdLedState)) 
    {
        Agtlog(LOG_ERROR, "[ProcGuardStateScan] BrdQueryLedState failed !\n");
    }
#endif//end defined(_8KI_)
    else
    {
#if defined(_8KI_) && defined(_LINUX_)
		// ��ʼ��tBrdLedState��Ϣ
		memset(&tBrdLedState, 0, sizeof(tBrdLedState));
		tBrdLedState.dwLedNum = sizeof(TBrdMPCLedStateDesc);
		memset(&tBrdLedState.nlunion.tBrdMPCLedState, BRD_LED_OFF, sizeof(tBrdLedState.nlunion.tBrdMPCLedState));//��ʼ��Ϊ����
	
		tBrdLedState.nlunion.tBrdMPCLedState.byLedRUN = ( tBreCtrlInfo.dwRUNLedStat <= 0 ) ? 0xff : (u8)tBreCtrlInfo.dwRUNLedStat;		
		tBrdLedState.nlunion.tBrdMPCLedState.byLedALM = ( tBreCtrlInfo.dwALMLedStat <= 0 ) ? 0xff : (u8)tBreCtrlInfo.dwALMLedStat;		
		u8 byCurCpuFanState = tBreCtrlInfo.dwCpuFanRPM >= 100 ? 0 : 1;		
		if( byOldCpuFanState != byCurCpuFanState )
		{
			post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_CPU_FAN_STATUS, &byCurCpuFanState, sizeof(byCurCpuFanState));
		}
		m_dwCpuFan = tBreCtrlInfo.dwCpuFanRPM;
		u8 byCurCpuTempState = tBreCtrlInfo.dwCpuTemp <= 85 ? 0 : 1;	
		if( byOldCpuTempState != byCurCpuTempState )
		{
			post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_CPUTEMP_STATUS, &byCurCpuTempState, sizeof(byCurCpuTempState));
		}
		m_byCpuTemp = tBreCtrlInfo.dwCpuTemp;
#endif
        if (0 != memcmp(&tBrdLedState, &m_tBrdLedState, sizeof(TBrdLedState))  )
        {
            Agtlog(LOG_INFORM, "[ProcGuardStateScan] tBrdLedState not equal to m_tBrdLedState !\n");
            
            memcpy( &m_tBrdLedState, &tBrdLedState, sizeof(TBrdLedState) );
		    post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_LED_STATUS, &m_tBrdLedState, sizeof(m_tBrdLedState));
        }
        else
        {
            Agtlog(LOG_VERBOSE, "[ProcGuardStateScan] tBrdLedState equal to m_tBrdLedState !\n");
		} 

        s8  abyBuf[256] = {'\0'};
        s8 *pBuff = abyBuf;
        u8  abyLedBuf[ MAX_BOARD_LED_NUM + 1 ]={0};    // led BUF  ���32����
        u8  byLedCount = 0;                            // led����
#ifdef _LINUX12_
        byLedCount = sizeof(TBrdMPC2LedDesc);
        memcpy( abyLedBuf, &m_tBrdLedState.nlunion.tBrdMPC2LedState, byLedCount);
#else
        byLedCount = sizeof(TBrdMPCLedStateDesc);
        memcpy( abyLedBuf, &m_tBrdLedState.nlunion.tBrdMPCLedState, byLedCount);
#endif
        pBuff += sprintf(pBuff, "[ProcGuardStateScan] Led Num:%d [", byLedCount );
        u8 byIdx = 0;    
        for( byIdx = 0; byIdx < byLedCount; byIdx++)
        {
            if ( byIdx >= MAX_BOARD_LED_NUM )
            {
                break;
            }        
            pBuff += sprintf(pBuff, " %d ", abyLedBuf[byIdx] );
        } 
        pBuff += sprintf(pBuff, "]\n");
        Agtlog(LOG_VERBOSE, abyBuf);
    }


// #endif // !_MINIMCU_

// #ifdef _MINIMCU_
//     // led status
//     if (OK != BrdQueryLedState(&tBrdLedState)) 
//     {
//         Agtlog(LOG_VERBOSE, "[ProcGuardStateScan] BrdQueryLedState failed !\n");
//     }
//     else
//     {        	
//         if (0 != memcmp(&tBrdLedState, &m_tBrdLedState, sizeof(TBrdLedState))  )
//         {
//             Agtlog(LOG_WARN, "[ProcGuardStateScan] tBrdLedState not equal to m_tBrsLedState !\n");
// 
//             memcpy( &m_tBrdLedState, &tBrdLedState, sizeof(TBrdLedState) );
// 		    post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_LED_STATUS, &m_tBrdLedState, sizeof(m_tBrdLedState));
//         }
//         else
//         {
// 			Agtlog(LOG_VERBOSE, "[ProcGuardStateScan] tBrdLedState equal to m_tBrsLedState !\n");
//         } 
//     }
// #endif // _MINIMCU_
//Ŀǰ����״̬��ȡ�ݲ�֧�֣�����ˢ�����ӡ[1/25/2013 chendaiwei]
// #ifdef _LINUX12_
// 	
// 	// [miaoqingsong 20101214 add] MCU8000A��������쳣�澯�ϱ�  
// 	TBoxFanStatus       tBoxFanStatus;              
// 	u8                  byFanId;                                     
// 	u16                 wFanSpeed;
// 	
// 	for ( byFanId = 0; byFanId < MCU8000ABOXFAN_NUM; byFanId++ )
// 	{
// 		u32 dwRet = BrdMPCFanBrdGetFanSpeed( byFanId, &wFanSpeed );
// 		
// 		if ( OK == dwRet )
// 		{
// 			if ( (wFanSpeed < MIN_MCU8000ABOXFAN_SPEED) && ((((unsigned int)1<<byFanId) & m_byBoxFanStatus) == 0) )
// 			{
// 				tBoxFanStatus.bySlot = byFanId / 2;
// 				tBoxFanStatus.byFanPos = byFanId % 2 + 1;
// 				tBoxFanStatus.byStatus = 1;
// 				
// 				post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_BOX_FAN_STATUS, &tBoxFanStatus, sizeof(tBoxFanStatus) );
// 				
// 				m_byBoxFanStatus |= (unsigned int)1 << byFanId;
// 			}
// 			else
// 			{
// 				if ( 0 < (((unsigned int)1<<byFanId) & m_byBoxFanStatus) ) 
// 					
// 				{
// 					tBoxFanStatus.bySlot = byFanId / 2;
// 					tBoxFanStatus.byFanPos = byFanId % 2 + 1;
// 					tBoxFanStatus.byStatus = 0;
// 					
// 					post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_BOX_FAN_STATUS, &tBoxFanStatus, sizeof(tBoxFanStatus) );
// 					
// 					m_byBoxFanStatus ^= (unsigned int)1 << byFanId;      
// 				}
// 			}
// 		}
// 		else
// 		{
// 			Agtlog( LOG_ERROR, "[BrdMPCFanBrdGetFanSpeed] get BoxFan's speed failed!\n");
// 		}
// 	}
// #endif  // _LINUX12_

#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
	
	// [2010/12/15 miaoqingsong add] MPC���ڴ�ʹ���ʸ澯�ϱ����ܣ����ڴ�ʹ���ʴ��ڵ���85%ʱ�ϱ��澯
    u32 dwByteFree = 0;
    u32 dwBytesAlloc = 1;
	TMPCMemoryStatus tMpcMemoryStatus;
    memset( &tMpcMemoryStatus, 0, sizeof(tMpcMemoryStatus) );
    if(OK == BrdGetMemInfo( &dwByteFree, &dwBytesAlloc) )
	{   
        // [20120718 liaokang] ��ֹ���
        dwByteFree = dwByteFree>>20;       // byte ----> MB   ��ֹ���
        dwBytesAlloc = dwBytesAlloc>>20;   // byte ----> MB   ��ֹ���

		tMpcMemoryStatus.byMemoryAllocRate = dwBytesAlloc * 100 / (dwBytesAlloc + dwByteFree);
		
		if ( (tMpcMemoryStatus.byMemoryAllocRate > 85) && (m_byMPCMemoryStatus == 0) )    // �ڴ�ʹ���ʳ���85%
		{
			Agtlog( LOG_WARN,"[ProcGuardStateScan] The Mpc's Memory is not enough: %d\n", tMpcMemoryStatus.byMemoryAllocRate);
			
			tMpcMemoryStatus.byMemoryStatus = 1;
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_MPCMEMORY_STATUS, &tMpcMemoryStatus, sizeof(tMpcMemoryStatus));
			m_byMPCMemoryStatus = 1;
		}
		
		if ( (tMpcMemoryStatus.byMemoryAllocRate <= 85) && (m_byMPCMemoryStatus == 1) )
		{
			tMpcMemoryStatus.byMemoryStatus = 0;
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_MPCMEMORY_STATUS, &tMpcMemoryStatus, sizeof(tMpcMemoryStatus));
			m_byMPCMemoryStatus = 0;
		}
        Agtlog( LOG_VERBOSE,"[ProcGuardStateScan] Free Mem:%u(MB), Alloc Mem:%u(MB), Rate:%u, Status:%u!\n", 
        dwByteFree, dwBytesAlloc, tMpcMemoryStatus.byMemoryAllocRate, m_byMPCMemoryStatus );
	}
	else
	{
		Agtlog( LOG_ERROR, "[BrdGetMemInfo] Error Get Memory Percentage.\n");
	}

	// [2010/12/15 miaoqingsong add] MPC��CPUռ���ʸ澯�ϱ����ܣ���Cpuռ���ʴ��ڵ���85%ʱ�ϱ��澯
	u8             byCpuIdleRate = 0;
	TOspCpuInfo    tCpuInfo;
	TMPCCpuStatus  tMpcCpuStatus;
    memset(&tMpcCpuStatus, 0, sizeof(tMpcCpuStatus) );
	
	if ( OspGetCpuInfo(&tCpuInfo) )
    {
		byCpuIdleRate = tCpuInfo.m_byIdlePercent;
        tMpcCpuStatus.byCpuAllocRate = 100 - byCpuIdleRate;
		       
		if ( (tMpcCpuStatus.byCpuAllocRate > 85) && (BOOL32)(m_byMPCCpuStatus == 0) )    // Cpuռ���ʳ���85%
        {           
			tMpcCpuStatus.byCpuStatus = 1;
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_CPU_STATUS, &tMpcCpuStatus, sizeof(tMpcCpuStatus));
			m_byMPCCpuStatus = 1;
        }
        
		if ( (tMpcCpuStatus.byCpuAllocRate <= 85) && (BOOL32)( m_byMPCCpuStatus == 1) )
		{
			tMpcCpuStatus.byCpuStatus = 0;
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_CPU_STATUS, &tMpcCpuStatus,sizeof(tMpcCpuStatus));
			m_byMPCCpuStatus = 0;
		} 
        Agtlog( LOG_VERBOSE,"[ProcGuardStateScan] Cpu Rate:%u, Status:%u!\n", 
        tMpcCpuStatus.byCpuAllocRate, m_byMPCCpuStatus);
    }
	else
	{
		Agtlog( LOG_ERROR, "[OspGetCpuInfo] Error Get Cpu Percentage.\n");
	}

#ifdef _LINUX12_
	// [2011/01/13 miaoqingsong add] MPC2���¶��쳣�澯�ϱ�����
    lm75 tSensor;
	TMPC2TempStatus  tMpc2TempStatus;

    u32 dwReturn = BrdGetSensorStat( &tSensor );
    tMpc2TempStatus.dwMpc2Temp = tSensor.temp;

	if ( OK == dwReturn )
	{
		if ( (tMpc2TempStatus.dwMpc2Temp > 70) && (BOOL32)(m_byMPC2TempStatus == 0) )    // MPC2���¶ȳ���70��
		{
			Agtlog(LOG_WARN,"[ProcGuardStateScan] The Mpc2's temperature is high enough: %d\n", tMpc2TempStatus.dwMpc2Temp);

            tMpc2TempStatus.byMpc2TempStatus = 1;
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_MPC2TEMP_STATUS, &tMpc2TempStatus, sizeof(tMpc2TempStatus));
			m_byMPC2TempStatus = 1;
		}

		if ( (tMpc2TempStatus.dwMpc2Temp <= 70) && (BOOL32)(m_byMPC2TempStatus == 1) )
		{
			tMpc2TempStatus.byMpc2TempStatus = 0;
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_MPC2TEMP_STATUS, &tMpc2TempStatus, sizeof(tMpc2TempStatus));
			m_byMPC2TempStatus = 0;
		}
	}
	else
	{
		Agtlog( LOG_ERROR, "[BrdGetSensorStat] Error Get Mpc2 temperature.\n");
	}
	
	// [2010/12/16 miaoqingsong add] ��Դ���¶��쳣�澯�ϱ�����
	TPowerBrdTempStatus    tPowerBrdTempStatus;
	u8                     byPowerBrdTemp = 0;
	
	u32 dwRet = BrdMPCLedBrdTempCheck( &byPowerBrdTemp );

	if ( OK == dwRet )
	{
		if( 1 == (byPowerBrdTemp & 0x01) )              // byPowerBrdTemp: 0x01λ��ʾ���Դ״̬��0���¶�������1���¶��쳣
		{
			if ( m_byPowerTempStatus == 0 )
			{
				tPowerBrdTempStatus.bySlot = POWER_LEFT;
				tPowerBrdTempStatus.byPowerBrdTempStatus = 1;
				OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_POWERTEMP_STATUS, &tPowerBrdTempStatus, sizeof(tPowerBrdTempStatus) );
			    m_byPowerTempStatus = 1;
			}
			
		}
		else
		{
			if ( m_byPowerTempStatus == 1 )
			{
				tPowerBrdTempStatus.bySlot = POWER_LEFT;
				tPowerBrdTempStatus.byPowerBrdTempStatus = 0;
				OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_POWERTEMP_STATUS, &tPowerBrdTempStatus, sizeof(tPowerBrdTempStatus) );
			    m_byPowerTempStatus = 0;
			}
		}

		if ( 2 == (byPowerBrdTemp & 0x02) )              // byPowerBrdTemp: 0x02λ��ʾ�ҵ�Դ״̬��0���¶�������1���¶��쳣
		{
			if ( m_byPowerTempStatus == 0 )
			{
				tPowerBrdTempStatus.bySlot = POWER_RIGHT;
				tPowerBrdTempStatus.byPowerBrdTempStatus = 1;
				OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_POWERTEMP_STATUS, &tPowerBrdTempStatus, sizeof(tPowerBrdTempStatus) );
			    m_byPowerTempStatus = 1;
			}
			
		}
		else
		{
			if ( m_byPowerTempStatus == 1 )
			{
				tPowerBrdTempStatus.bySlot = POWER_RIGHT;
				tPowerBrdTempStatus.byPowerBrdTempStatus = 0;
				OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_POWERTEMP_STATUS, &tPowerBrdTempStatus, sizeof(tPowerBrdTempStatus) );
			    m_byPowerTempStatus = 0;
			}
		}
	}
	else
	{
		Agtlog( LOG_ERROR, "[BrdMPCLedBrdTempCheck] Error Get Power's board temperature!\n");
	}
#endif // end _LINUX12_
#endif // end !_8KE_
	
	// �������ö�ʱ��
	SetTimer( MCUAGENT_SCAN_STATE_TIMER, SCAN_STATE_TIME_OUT );

#endif // !(_VXWORKS_ || _LINUX_)

    return;
}

/*lint -restore*/
// END OF FILE
