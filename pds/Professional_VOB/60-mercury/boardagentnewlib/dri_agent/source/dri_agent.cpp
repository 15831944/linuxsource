/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : driagent.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �������ʵ�֣���ɸ澯�������MANAGER�Ľ���
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/25  1.0         jianghy       ����
   2004/12/05  3.5         �� ��        �½ӿڵ��޸�
   2006/04/18  4.0         john         ���������������
******************************************************************************/
#include "osp.h"
#include "dri_agent.h"
#include "eqplib.h"

#ifdef _VXWORKS_
#include "timers.h"
#include <dirent.h>
#endif

CBrdAgentApp	g_cBrdAgentApp;	//�������Ӧ��ʵ��

extern SEMHANDLE g_semDRI;

// ���캯��
CBoardAgent::CBoardAgent()
{
	memset(&m_tBrdLedState, 0, sizeof(TBrdLedState));
	memset(m_byE1Alarm,0,sizeof(m_byE1Alarm));
    memset(m_wPortE1Num, 0, sizeof(m_wPortE1Num));
    memset(m_wLastE1BandWidth, 0, sizeof(m_wLastE1BandWidth));
	return;
}

// ��������
CBoardAgent::~CBoardAgent()
{
	return;
}

/*====================================================================
    ������      ��InstanceExit
    ����        ��ʵ���˳�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/20    1.0         jianghy       ����
====================================================================*/
void CBoardAgent::InstanceExit()
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
    03/08/20    1.0         jianghy       ����
====================================================================*/
void CBoardAgent::InstanceEntry(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
	{
		OspPrintf(TRUE, FALSE,"BoardAgent: The received msg's pointer in the msg entry is NULL!");
		return;
	}
	
	DriLog("%u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event));

    if ( MPC_BOARD_REG_ACK == pcMsg->event)
    {
        s8 achModCfgFimeName[MAX_PATH] = {0};
        sprintf( achModCfgFimeName, "%s/%s", DIR_CONFIG, FILE_MODCFG_INI);

        //��ʼ��������֤DRI2��ע��ɹ����ϱ�E1����
        //������ﲻ��ʼ��������Ϊ֮ǰ��ֵ,�����жϴ���δ�仯�������ϱ�
        for ( u8 abyLoop = 0; abyLoop < 8; abyLoop++ )
        {            
            m_wLastE1BandWidth[abyLoop] = 0;
            DriLog( "[InstanceEntry]Reset last e1 band width!%d\n" );
        }
        
        s32 nValue;
        GetRegKeyInt( achModCfgFimeName, "Dri2E1Check", "V0E1Num ", 0, &nValue);
        m_wPortE1Num[0] = (u8)nValue;
        GetRegKeyInt( achModCfgFimeName, "Dri2E1Check", "V1E1Num ", 0, &nValue);
        m_wPortE1Num[1] = (u8)nValue;
        GetRegKeyInt( achModCfgFimeName, "Dri2E1Check", "V2E1Num ", 0, &nValue);
        m_wPortE1Num[2] = (u8)nValue;
        GetRegKeyInt( achModCfgFimeName, "Dri2E1Check", "V3E1Num ", 0, &nValue);
        m_wPortE1Num[3] = (u8)nValue;
        GetRegKeyInt( achModCfgFimeName, "Dri2E1Check", "V4E1Num ", 0, &nValue);
        m_wPortE1Num[4] = (u8)nValue;
        GetRegKeyInt( achModCfgFimeName, "Dri2E1Check", "V5E1Num ", 0, &nValue);
        m_wPortE1Num[5] = (u8)nValue;
        GetRegKeyInt( achModCfgFimeName, "Dri2E1Check", "V6E1Num ", 0, &nValue);
        m_wPortE1Num[6] = (u8)nValue;
        GetRegKeyInt( achModCfgFimeName, "Dri2E1Check", "V7E1Num ", 0, &nValue);
        m_wPortE1Num[7] = (u8)nValue;

        GetRegKeyInt( achModCfgFimeName, "Dri2E1Check", "CheckE1Time", 500, &nValue);
        m_wCheckE1Time = (u16)nValue;

        for ( u8 byLoop = 0; byLoop < MAXNUM_E1PORT; byLoop++)
        {
            DriLog( "[InstanceEntry]port_%d E1 Num is %d\n", byLoop, m_wPortE1Num[byLoop]);
        }

        DriLog( "[InstanceEntry]set BRDAGENT_E1CHECK_TIMER interval is %d!\n", m_wCheckE1Time);
        SetTimer( BRDAGENT_E1CHECK_TIMER, m_wCheckE1Time);
         
    }
    
	switch( pcMsg->event )
	{
// 	case OSP_POWERON:            //��������
//     case BRDAGENT_CONNECT_MANAGERA_TIMER:
// 	case BRDAGENT_CONNECT_MANAGERB_TIMER:          //���ӹ������ʱ
//     case BRDAGENT_REGISTERA_TIMER:
// 	case BRDAGENT_REGISTERB_TIMER:		      //ע��ʱ�䳬ʱ
// 	case MPC_BOARD_REG_ACK:              //ע��Ӧ����Ϣ
// 	case MPC_BOARD_REG_NACK:			 //ע���Ӧ����Ϣ
// 	case BOARD_MPC_GET_CONFIG:           //�ȴ�����Ӧ����Ϣ��ʱ
// 	case BRDAGENT_GET_CONFIG_TIMER:		 //ȡ���ö�ʱ��
// 	case MPC_BOARD_ALARM_SYNC_REQ:       //�������ĸ澯ͬ������
// 	case MPC_BOARD_BIT_ERROR_TEST_CMD:   //���������������
// 	case MPC_BOARD_TIME_SYNC_CMD:        //����ʱ��ͬ������
// 	case MPC_BOARD_SELF_TEST_CMD:        //�����Բ�����
// 	case MPC_BOARD_RESET_CMD:            //��������������
// 	case MPC_BOARD_UPDATE_SOFTWARE_CMD:  //�����������
// 	case MPC_BOARD_GET_STATISTICS_REQ:   //��ȡ�����ͳ����Ϣ
// 	case MPC_BOARD_GET_VERSION_REQ:      //��ȡ����İ汾��Ϣ
// 	case MPC_BOARD_GET_MODULE_REQ:      //��ȡ�����ģ����Ϣ
// 	case OSP_DISCONNECT:
// 	//�����Ǹ澯����������Ϣ
// 	case SVC_AGT_MEMORY_STATUS:          //�ڴ�״̬�ı�
// 	case SVC_AGT_FILESYSTEM_STATUS:      //�ļ�ϵͳ״̬�ı�
// 	// �����ǲ�������Ϣ
// 	case BOARD_MPC_CFG_TEST:
// 	case BOARD_MPC_GET_ALARM:
// 	case BOARD_MPC_MANAGERCMD_TEST:
// 		CBBoardAgent::InstanceEntry(pcMsg);
// 		break;

	case BOARD_LINK_STATUS:
		ProcBoardLinkStatus( pcMsg );
		break;

	case BOARD_LED_STATUS:
		ProcBoardLedStatus( pcMsg );
		break;

	case MPC_BOARD_E1_LOOP_CMD:
		ProcE1LoopBackCmd( pcMsg );
		break;

	case MPC_BOARD_LED_STATUS_REQ:
		ProcLedStatusReq( pcMsg );
		break;

	case MPC_BOARD_GET_CONFIG_ACK:       //ȡ������ϢӦ����Ϣ
		ProcBoardGetConfigAck( pcMsg );
		break;
		
	case MPC_BOARD_GET_CONFIG_NACK:      //ȡ������Ϣ��Ӧ��
		ProcBoardGetConfigNAck( pcMsg );
		break;

	case MCU_BOARD_CONFIGMODIFY_NOTIF:
		ProcBoardConfigModify( pcMsg );
		break;

	case BOARD_TEMPERATURE_STATUS_NOTIF:
		ProcBoardTempStatusNotif(pcMsg);
		break;
		
	case BOARD_CPU_STATUS_NOTIF:
		ProcBoardCpuStatusNotif(pcMsg);
		break;
		
	case BOARD_MODULE_STATUS:
		ProcBoardE1AlarmNotif(pcMsg);
		break;
    case BRDAGENT_E1CHECK_TIMER:
        ProcBoardCheckE1TimerOut();
        break;


	default:
		CBBoardAgent::InstanceEntry(pcMsg);
		break;
	}
	
	return;
}

//ģ�ⵥ��BSP�ϵĺ��� ������
//#ifdef WIN32
//void BrdQueryPosition(TBrdPos* ptPosition)
//{
//	/*�����õ�ʱ���Ѿ�����*/
//	return ;
//}
//#endif

/*====================================================================
    ������      ��ProcBoardGetConfigNAck
    ����        ������ȡ������Ϣ��Ӧ����Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBoardAgent::ProcBoardGetConfigNAck( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_INIT:
		DriLog( "Receive Get Config NAck Message!\n ");

		break;
		
	default:
		OspPrintf( TRUE, FALSE, "Wrong state %u when in (ProcBoardGetConfigNAck)!srcnode.%d", CurState(),pcMsg!=NULL?pcMsg->srcnode:0 );
		break;
	}
	return;
}

/*====================================================================
    ������      ��ProcBoardGetConfigAck
    ����        ������ȡ������ϢӦ����Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
    04/11/11    3.5         �� ��         �½ӿڵ��޸�
====================================================================*/
void CBoardAgent::ProcBoardGetConfigAck( CMessage* const pcMsg )
{
	u8	byPeriCount;
	u8	byPeriType;
	u16	wLoop, wIndex;
	TEqpPrsEntry*	 ptPrsCfg = NULL;		//Prs��������Ϣ
    
	switch( CurState() ) 
	{
	case STATE_INIT:
		NEXTSTATE( STATE_NORMAL );
		byPeriCount = pcMsg->content[0];// �����������
        wIndex = 1;

		for( wLoop = 0; wLoop < byPeriCount; wLoop++ )
		{			
			byPeriType = pcMsg->content[wIndex++];// ������������
						
			switch( byPeriType ) // ���øõ�����Ӧ��������Ϣ
			{
			case EQP_TYPE_PRS:
				ptPrsCfg = (TEqpPrsEntry*)(pcMsg->content+wIndex);
                ptPrsCfg->SetMcuIp(g_cBrdAgentApp.GetMpcIpA());
				wIndex += sizeof(TEqpPrsEntry);
				g_cBrdAgentApp.SetPrsConfig(ptPrsCfg);
                brdagtlog( "[Dri] The Prs run on Dri\n");
				break;

			default:
				break;
			}
		}

//#ifndef WIN32
		
		if (1 == pcMsg->content[wIndex])// ���øõ�����Ӧ��������Ϣ
		{
            BrdSetE1SyncClkOutputState(E1_CLK_OUTPUT_ENABLE);
            BrdSelectE1NetSyncClk(pcMsg->content[wIndex+1]);
		}
		else
		{
            BrdSetE1SyncClkOutputState(E1_CLK_OUTPUT_DISABLE);
		}

		BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON );
//#endif
		// ����IP
		g_cBrdAgentApp.SetBrdIpAddr( ntohl(  *(u32*)&pcMsg->content[wIndex+2] ) );

		// �Ƿ�ʹ�ÿ��Ź�
		brdagtlog( "Set Reboot Option %d\n", pcMsg->content[ wIndex + 6 ] );
		if( 0 == pcMsg->content[ wIndex + 6 ] )
		{
//#ifndef WIN32
			SysRebootDisable( ); // ��ֹϵͳ�����쳣ʱ�Զ���λ
//#endif
		}
		else
		{
//#ifndef WIN32
			SysRebootEnable( ); // ����ϵͳ�����쳣ʱ�Զ���λ
//#endif
		}

		OspSemGive(g_semDRI);

		KillTimer( BOARD_MPC_GET_CONFIG );
		break;
		
	default:
		OspPrintf( TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    ������      ��ProcE1LoopBackCmd
    ����        ��MPC���͸������e1���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBoardAgent::ProcE1LoopBackCmd( CMessage* const pcMsg )
{
	u8 byCount = 0;
	u16 wLoop = 0;


	switch( CurState() ) 
	{
	case STATE_NORMAL:
//#ifndef WIN32
		// ��Ϣ��ʽ: �����õ�E1�� + ( e1�� + MODE )*�����õ�E1�� 
		byCount = pcMsg->content[0];
		for( wLoop = 0; wLoop < byCount; wLoop++ )
		{
			//BrdDRISetRelayLoop( pcMsg->content[wLoop*2+1], pcMsg->content[wLoop*2+2] );
            BrdSetE1RelayLoopMode( pcMsg->content[wLoop*2+1], pcMsg->content[wLoop*2+2] );
            DriLog( "ProcE1LoopBackCmd: test E1 loop back.");
		}
//#endif
		// ������
		TestingProcess( pcMsg );
		break;
		
	default:
		OspPrintf( TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    ������      ��ProcBoardLedStatus
    ����        ������״̬�ı���Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBoardAgent::ProcBoardLedStatus( CMessage* const pcMsg )
{
//#ifndef WIN32
	u8 abyBuf[32];
	u8  byCount;
	TBrdLedState tBrdLedState;
	memcpy(&tBrdLedState, pcMsg->content, sizeof(TBrdLedState));

	s8 abyLedState[256] = {0};

	abyLedState[255] = '\0';

	s8 *pBuff = abyLedState;

	pBuff += sprintf(pBuff, "[ProcBoardLedStatus] LedNum:%d [", tBrdLedState.dwLedNum);

	u32 dwIndex = 0;

	for (dwIndex = 0; dwIndex < tBrdLedState.dwLedNum; ++dwIndex)
	{
		if (dwIndex >= BRD_LED_NUM_MAX)
		{
			break;
		}

		pBuff += sprintf(pBuff, " %d ", tBrdLedState.nlunion.byLedNo[dwIndex]);
	}


	pBuff += sprintf(pBuff, "]\n");


	DriLog(abyLedState);

	if( memcmp(&m_tBrdLedState, &tBrdLedState, sizeof(m_tBrdLedState)) == 0 )
		return;

	memset(abyLedState, 0, 256);

	abyLedState[255] = '\0';

	pBuff = abyLedState;

	pBuff += sprintf(pBuff, "[ProcBoardLedStatus] LedNum:%d [", m_tBrdLedState.dwLedNum);
	
	for (dwIndex = 0; dwIndex < m_tBrdLedState.dwLedNum; ++dwIndex)
	{
		if (dwIndex >= BRD_LED_NUM_MAX)
		{
			break;
		}
		
		pBuff += sprintf(pBuff, " %d ", m_tBrdLedState.nlunion.byLedNo[dwIndex]);
	}
	
	pBuff += sprintf(pBuff, "]  diff\n");
	
	DriLog(abyLedState);

	memcpy(&m_tBrdLedState, &tBrdLedState, sizeof(m_tBrdLedState));
	
	TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );
	//byCount = sizeof(TBrdDRILedStateDesc);
	byCount = tBrdLedState.dwLedNum;

	memcpy(abyBuf+sizeof(TBrdPos), &byCount, sizeof(byCount) ); 
	memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8),  &m_tBrdLedState.nlunion, byCount * sizeof(u8));
					
	PostMsgToManager( BOARD_MPC_LED_STATUS_NOTIFY, abyBuf, 
		sizeof(TBrdPos)+sizeof(u8)+byCount * sizeof(u8) );
//#endif
}

/*====================================================================
    ������      ��ProcLedStatusReq
    ����        ��MPC���͸������ȡ���������״̬��Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
    04/11/11    3.5         �� ��         �½ӿڵ��޸�
====================================================================*/
void CBoardAgent::ProcLedStatusReq( CMessage* const pcMsg )
{
//#ifndef WIN32
	u8 abyBuf[32];
	u8  byCount;
    TBrdLedState tBrdLedState;

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
		{
			BrdQueryLedState( &tBrdLedState );
			memcpy(&m_tBrdLedState, &tBrdLedState, sizeof(m_tBrdLedState));

			s8 abyLedState[256] = {0};
			
			abyLedState[255] = '\0';

			s8 *pBuff = abyLedState;
			
			pBuff += sprintf(pBuff, "[ProcLedStatusReq] LedNum:%d [", tBrdLedState.dwLedNum);
			
			for (u32 dwIndex = 0; dwIndex < tBrdLedState.dwLedNum; ++dwIndex)
			{
				if (dwIndex >= BRD_LED_NUM_MAX)
				{
					break;
				}
				
				pBuff += sprintf(pBuff, " %d ", tBrdLedState.nlunion.byLedNo[dwIndex]);
			}
			
			
			pBuff += sprintf(pBuff, "]\n");
			
			
			DriLog(abyLedState);

			TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
			memcpy(abyBuf, &tBrdPos, sizeof(tBrdPos) );
			//byCount = sizeof(m_tLedState);
			byCount = m_tBrdLedState.dwLedNum;
			memcpy(abyBuf+sizeof(tBrdPos), &byCount, sizeof(byCount) ); 
			memcpy(abyBuf+sizeof(tBrdPos)+sizeof(byCount), &m_tBrdLedState.nlunion, byCount * sizeof(u8));
			
			PostMsgToManager( BOARD_MPC_LED_STATUS_ACK, abyBuf, 
				sizeof(tBrdPos) + sizeof(byCount) + sizeof(u8) * byCount);
		}
		break;
		
	default:
		OspPrintf( TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
//#endif
	return;
}

/*====================================================================
    ������      ��ProcBoardLinkStatus
    ����        ���ڴ�״̬�ı���Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBoardAgent::ProcBoardLinkStatus( CMessage* const pcMsg )
{
	u16 wLoop;
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];
    u16   dwE1AlarmCode[] = { ALARM_MCU_BRD_E1_RMF,  ALARM_MCU_BRD_E1_RCMF,
							   ALARM_MCU_BRD_E1_RUA1, ALARM_MCU_BRD_E1_RRA, 
							   ALARM_MCU_BRD_E1_LOC,  ALARM_MCU_BRD_E1_RLOS };
    u8   byE1AlarmBit[] = { 0x32,0x16,0x08, 0x04, 0x02, 0x01 };

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();

	switch( pcMsg->event )
	{
	case BOARD_LINK_STATUS:
        byAlarmObj[0] = ( u8 )pcMsg->content[0];
        byAlarmObj[1] = 0;

        for( wLoop = 0; wLoop < sizeof( dwE1AlarmCode ) / sizeof( dwE1AlarmCode[0] ); wLoop++ )
        {
            if( FindAlarm( dwE1AlarmCode[wLoop], ALARMOBJ_MCU_LINK, byAlarmObj, &tAlarmData ) )
            {
                if( ( pcMsg->content[1] & byE1AlarmBit[wLoop] ) == 0 )    // normal
                {
                    if( DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ) == FALSE )
                        brdagtlog("DriAgent: DeleteAlarm( AlarmCode = %lu, Object = %d --- Link ) failed!\n", 
                            tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0]);
                    else
					{
						tAlarmMsg.wEventId = MCU_AGT_LINK_STATUSCHANGE;
						tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
						tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
						tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
						tAlarmMsg.abyAlarmContent[3] = pcMsg->content[0];
						tAlarmMsg.abyAlarmContent[4] = pcMsg->content[1] & byE1AlarmBit[wLoop];
						
						memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
						
						memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
							&tAlarmMsg, sizeof(tAlarmMsg) );
						
						*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 
						
						PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
							sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );				
					}
                }
            }
            else        //no such alarm
            {
                if( ( pcMsg->content[1] & byE1AlarmBit[wLoop] ) != 0 )    //abnormal
                {
                    if( AddAlarm( dwE1AlarmCode[wLoop], ALARMOBJ_MCU_LINK, byAlarmObj, &tAlarmData ) == FALSE )
                        brdagtlog("DriAgent: AddAlarm( AlarmCode = %lu, Object = %d --- Link) failed!\n", 
                            dwE1AlarmCode[wLoop], byAlarmObj[0] );
                    else
					{						
						tAlarmMsg.wEventId = MCU_AGT_LINK_STATUSCHANGE;
						tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
						tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
						tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
						tAlarmMsg.abyAlarmContent[3] = pcMsg->content[0];
						tAlarmMsg.abyAlarmContent[4] = pcMsg->content[1] & byE1AlarmBit[wLoop];
						
						SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg);

						memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
						
						memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
							&tAlarmMsg, sizeof(tAlarmMsg) );
						
						*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 
						
						PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
							sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
					}
                }
            }
        }
        break;
	default:
		break;
	}
	
}
/*=============================================================================
�� �� ���� ProcBoardConfigModify
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage* const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/19   4.0			�ܹ��                  ����
=============================================================================*/
void CBoardAgent::ProcBoardConfigModify( CMessage* const pcMsg )
{
	if ( NULL == pcMsg )
	{
		OspPrintf(TRUE, FALSE, "[ProcBoardConfigModify] Receive null msg!\n");
		return;
	}

	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byLayer = 0;
	memcpy( &byLayer, cServMsg.GetMsgBody(), sizeof(u8) );
	u8 bySlot = 0;
	memcpy( &bySlot, cServMsg.GetMsgBody()+sizeof(u8), sizeof(u8) );

	if ( byLayer != g_cBrdAgentApp.GetBrdPosition().byBrdLayer
		|| bySlot != g_cBrdAgentApp.GetBrdPosition().byBrdSlot )
	{
		DriLog( "[ProcBoardConfigModify] Receive msg is not local board msg! Layer = %d, Slot = %d\n", byLayer, bySlot );
		return;
	}

	u8	byPeriCount;
	u8	byPeriType;
	u16	wLoop, wIndex;
	TEqpPrsEntry  *ptPrsCfg = NULL;		//Prs��������Ϣ 
	BOOL32 bIsCfgPrs = FALSE;
	
	u8 *pMsg = cServMsg.GetMsgBody()+sizeof(u8)+sizeof(u8);

	byPeriCount = pMsg[0];// �����������
    wIndex = 1;

	for( wLoop = 0; wLoop < byPeriCount; wLoop++ )
	{			
		byPeriType = pMsg[wIndex++];// ������������
					
		switch( byPeriType ) // ���øõ�����Ӧ��������Ϣ
		{
		case EQP_TYPE_PRS:
			bIsCfgPrs = TRUE;
			if ( !g_cBrdAgentApp.IsRunPrs() )
			{
				ptPrsCfg = (TEqpPrsEntry*)(pMsg+wIndex);
				ptPrsCfg->SetMcuIp(g_cBrdAgentApp.GetMpcIpA());
				wIndex += sizeof(TEqpPrsEntry);
				g_cBrdAgentApp.SetPrsConfig(ptPrsCfg);
				TPrsCfg tPrsCfg;
				g_cBrdAgentApp.GetPrsCfg(&tPrsCfg);
				if( !prsinit( &tPrsCfg ) )
				{
					DriLog( " [ProcBoardConfigModify]Start the prs failed!\n");
				}	
				else
				{
					DriLog( " [ProcBoardConfigModify]Start the prs OK!\n");
				}			
			}
			else
			{
				DriLog( "The board have started the Prs module, need reboot!\n" );
				return;
			}
			break;

		default:
			break;
		}
	}

	if ( !bIsCfgPrs && g_cBrdAgentApp.IsRunPrs() )
	{
		DriLog("[ProcBoardConfigModify] New cfg delete the prs, need reboot!\n");
		return;
	}

//#ifndef WIN32
	
	if (1 == pMsg[wIndex])// ���øõ�����Ӧ��������Ϣ
	{
        BrdSetE1SyncClkOutputState(E1_CLK_OUTPUT_ENABLE);
        BrdSelectE1NetSyncClk(pcMsg->content[wIndex+1]);
	}
	else
	{
        BrdSetE1SyncClkOutputState(E1_CLK_OUTPUT_DISABLE);
	}

//#endif
	// ����IP
	if ( g_cBrdAgentApp.GetBrdIpAddr() != ntohl(  *(u32*)&pMsg[wIndex+2] ) )
	{
		DriLog( "The board ip changed, need recfg!\n" );
	}

	// �Ƿ�ʹ�ÿ��Ź�
	brdagtlog( "Set Reboot Option %d\n", pMsg[ wIndex + 6 ] );
	if( 0 == pMsg[ wIndex + 6 ] )
	{
//#ifndef WIN32
		SysRebootDisable( ); // ��ֹϵͳ�����쳣ʱ�Զ���λ
//#endif
	}
	else
	{
//#ifndef WIN32
		SysRebootEnable( ); // ����ϵͳ�����쳣ʱ�Զ���λ
//#endif
	}
	return;
}

/*=============================================================================
�� �� ���� ProcBoardE1AlarmNotif
��    �ܣ� ����E1�澯����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage* const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/10/25   4.0		chendaiwei            ����
=============================================================================*/
void CBoardAgent::ProcBoardE1AlarmNotif(CMessage* const pcMsg)
{
	u8 byLoop;
	u8 byOldE1Alarm;
    TBrdAlarmState tBrdAlarmState = *(TBrdAlarmState*)(pcMsg->content);
    TBrdDRIAlarmAll * ptBrdE1Alarm = &tBrdAlarmState.nlunion.tBrdDRIAlarmAll;
	
	for( byLoop=0; byLoop < sizeof(TBrdDRIAlarmAll)/sizeof(TBrdE1AlarmDesc); byLoop++)
	{
		byOldE1Alarm = m_byE1Alarm[byLoop];
		
		//E1ʧͬ��
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RLOS != (BOOL32)((m_byE1Alarm[byLoop] & 0x01)==0x01) )
			m_byE1Alarm[byLoop] ^= 0x01;
		//E1ʧ�ز�
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RCL != (BOOL32)((m_byE1Alarm[byLoop] & 0x02)==0x02) )
			m_byE1Alarm[byLoop] ^= 0x02;
		//E1Զ�˸澯
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RRA != (BOOL32)((m_byE1Alarm[byLoop] & 0x04)==0x04) )
			m_byE1Alarm[byLoop] ^= 0x04;
		//E1ȫ1
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RUA1 != (BOOL32)((m_byE1Alarm[byLoop] & 0x08)==0x08) )
			m_byE1Alarm[byLoop] ^= 0x08;

		//CRC��֡����
// 		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RCMF != ((m_byE1Alarm[byLoop] & 0x10)==0x10) )
// 			m_byE1Alarm[byLoop] ^= 0x10;
// 		//��·��֡����
// 		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RMF != ((m_byE1Alarm[byLoop] & 0x20)==0x20) )
// 			m_byE1Alarm[byLoop] ^= 0x20;
		
		if( byOldE1Alarm != m_byE1Alarm[byLoop] )
		{
			u16 wLoop;
			TBoardAlarm tAlarmData;
			u8 byAlarmObj[2];
			TBoardAlarmMsgInfo tAlarmMsg;
			u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];
			u16   dwE1AlarmCode[] = { 
				/*ALARM_MCU_BRD_E1_RMF,  ALARM_MCU_BRD_E1_RCMF,*/
				ALARM_MCU_BRD_E1_RUA1, ALARM_MCU_BRD_E1_RRA, 
				ALARM_MCU_BRD_E1_LOC,  ALARM_MCU_BRD_E1_RLOS
			};
			u8   byE1AlarmBit[] = {/* 0x20,0x10,*/ 0x08, 0x04, 0x02, 0x01 };
			
			memset( &tAlarmData,0,sizeof(tAlarmData));
			memset( byAlarmObj, 0, sizeof(byAlarmObj) );
			memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
			memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );
			
			TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
			
			byAlarmObj[0] = byLoop; // E1��·ID [10/26/2011 chendaiwei]
			byAlarmObj[1] = 0;
			
			for( wLoop = 0; wLoop < sizeof( dwE1AlarmCode ) / sizeof( dwE1AlarmCode[0] ); wLoop++ )
			{
				if( FindAlarm( dwE1AlarmCode[wLoop], ALARMOBJ_MCU_LINK, byAlarmObj, &tAlarmData ) )
				{
					if( ( m_byE1Alarm[byLoop] & byE1AlarmBit[wLoop] ) == BRD_STATUS_NORMAL )    // normal
					{
						if( !DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ))
						{
							brdagtlog("DriAgent: DeleteAlarm( AlarmCode = %lu, Object = %d --- Link ) failed!\n", 
							tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0]);
						}
						else
						{
							tAlarmMsg.wEventId = MCU_AGT_LINK_STATUSCHANGE;
							tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
							tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
							tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
							tAlarmMsg.abyAlarmContent[3] = byLoop;
							tAlarmMsg.abyAlarmContent[4] = m_byE1Alarm[byLoop] & byE1AlarmBit[wLoop];
							
							memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
							
							memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
								&tAlarmMsg, sizeof(tAlarmMsg) );
							
							*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 
							
							PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
								sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );				
						}
					}
				}
				else        //no such alarm
				{
					if( ( m_byE1Alarm[byLoop] & byE1AlarmBit[wLoop] ) != BRD_STATUS_NORMAL )    //abnormal
					{
						if( !AddAlarm( dwE1AlarmCode[wLoop], ALARMOBJ_MCU_LINK, byAlarmObj, &tAlarmData ))
							
						{
							brdagtlog("DriAgent: AddAlarm( AlarmCode = %lu, Object = %d --- Link) failed!\n", dwE1AlarmCode[wLoop], byAlarmObj[0] );
						}
						else
						{						
							tAlarmMsg.wEventId = MCU_AGT_LINK_STATUSCHANGE;
							tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
							tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
							tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
							tAlarmMsg.abyAlarmContent[3] = byLoop;
							tAlarmMsg.abyAlarmContent[4] = m_byE1Alarm[byLoop] & byE1AlarmBit[wLoop];
							
							SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg);
							
							memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
							
							memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
								&tAlarmMsg, sizeof(tAlarmMsg) );
							
							*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 
							
							PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
								sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
						}
					}
				}
			}
		}
	}

	return;
}
/*=============================================================================
�� �� ���� ProcBoardCheckE1TimerOut
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/12/21   4.0		��ʤ��                  ����
=============================================================================*/
void CBoardAgent::ProcBoardCheckE1TimerOut( void )
{

#ifdef _LINUX12_

    DriLog( "[ProcBoardCheckE1TimerOut] BRDAGENT_E1CHECK_TIMER coming!\n");
    SetTimer( BRDAGENT_E1CHECK_TIMER, m_wCheckE1Time);

    BOOL32 bBWChanged = FALSE;
    u16 wCurE1BandWidth[MAXNUM_E1PORT];
    memset(wCurE1BandWidth, 0 , sizeof(wCurE1BandWidth));

    //��⵱ǰ���ж˿ڴ���
    for ( u8 byPortNo = 0; byPortNo < MAXNUM_E1PORT; byPortNo++)
    {
        wCurE1BandWidth[byPortNo] = (u16)BrdGetE1Bandwidth( "virtual", byPortNo);
        DriLog( "[ProcBoardCheckE1TimerOut]port_%d cur BandWidth is %d\n", byPortNo, wCurE1BandWidth[byPortNo] );
        
        if( wCurE1BandWidth[byPortNo] != m_wLastE1BandWidth[byPortNo])
        {
            DriLog( "[ProcBoardCheckE1TimerOut]E1 port_%d Band Width change from %d to %d!\n", 
                byPortNo, m_wLastE1BandWidth[byPortNo], wCurE1BandWidth[byPortNo]);
            bBWChanged = TRUE;
            m_wLastE1BandWidth[byPortNo] = wCurE1BandWidth[byPortNo];
        }
        
        //ת�������򣬷���MPC
        wCurE1BandWidth[byPortNo] = htons( wCurE1BandWidth[byPortNo] );
    }  

    //�������ı�֪ͨMPC
    if (bBWChanged)
    {
        DriLog( "[ProcBoardCheckE1TimerOut]notify mpc E1 Band Width changed!\n");
        //����ip(DRI2)
        u32 dwBrdIpAddr = g_cBrdAgentApp.GetBrdIpAddr();
        dwBrdIpAddr = htonl(dwBrdIpAddr);

        u8 abyBuf[ sizeof(wCurE1BandWidth) + sizeof(m_wPortE1Num) + sizeof(dwBrdIpAddr) + 10];
        memset( abyBuf, 0, sizeof(abyBuf));

        memcpy( abyBuf, wCurE1BandWidth, sizeof(wCurE1BandWidth));
        memcpy( abyBuf + sizeof(wCurE1BandWidth), m_wPortE1Num, sizeof(m_wPortE1Num));
        memcpy( abyBuf + sizeof(wCurE1BandWidth) + sizeof(m_wPortE1Num), &dwBrdIpAddr, sizeof(dwBrdIpAddr) );
        PostMsgToManager(BOARD_MPC_E1BANDWIDTH_NOTIF, abyBuf, sizeof(wCurE1BandWidth) + sizeof(m_wPortE1Num) + sizeof(dwBrdIpAddr));       
    }
#endif

    return;
}
/*lint -save -esym(530,argptr)*/
void DriLog(char * fmt, ...)
{
	if ( g_cBrdAgentApp.m_bPrintBrdLog )
	{
		OspPrintf(TRUE, FALSE, "[Dri]");
	}
    va_list argptr;
	va_start(argptr, fmt);
	g_cBrdAgentApp.brdlog(fmt, argptr);
	va_end(argptr);
    return;
}
/*lint -restore*/

API u32 BrdGetDstMcuNode( void )
{
    return g_cBrdAgentApp.m_dwDstMcuNodeA;
}

API u32 BrdGetDstMcuNodeB(void)
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}

API void noupdatesoft( void )
{
	g_cBrdAgentApp.m_bUpdateSoftBoard = FALSE;
}

API void updatesoft( void )
{
	g_cBrdAgentApp.m_bUpdateSoftBoard = TRUE;
}

API void pdrimsg(void)
{
    g_cBrdAgentApp.m_bPrintBrdLog = TRUE;
    return;
}

API void npdrimsg(void)
{
	g_cBrdAgentApp.m_bPrintBrdLog = FALSE;
	return;
}
API u8  GetBoardSlot()
{
	return g_cBrdAgentApp.GetBrdPosition().byBrdSlot;
}
