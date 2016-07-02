/*****************************************************************************
   ģ����      : Board Agent Basic
   �ļ���      : boardagentbasic.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �������ʵ�֣���ɸ澯�������MANAGER�Ľ���
   ����        : �ܹ��
   �汾        : V4.0  Copyright(C) 2001-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2007/08/20  4.0         �ܹ��       ����
******************************************************************************/

#include "boardagentbasic.h"
#include "mcuver.h"

#ifdef _VXWORKS_
#include "timers.h"
#include <dirent.h>
#endif


//���캯��
CBBoardAgent::CBBoardAgent()
{
	m_pBBoardConfig = NULL;
	m_byIsMpu2SimuMpu = 0;
	m_byIsAnyBrdRegSuccess = 0;
	m_byRegBrdTypeFlagA = 0;
	m_byRegBrdTypeFlagB = 0;
	return;
}

//��������
CBBoardAgent::~CBBoardAgent()
{
	m_pBBoardConfig = NULL;
	m_byIsMpu2SimuMpu = 0;
	m_byIsAnyBrdRegSuccess = 0;
	m_byRegBrdTypeFlagA = 0;
	m_byRegBrdTypeFlagB = 0;
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
    2007/08/20  4.0         �ܹ��       ����
====================================================================*/
void CBBoardAgent::InstanceExit()
{
	BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF );
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
void CBBoardAgent::InstanceEntry(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
	{
		OspPrintf( TRUE, FALSE,  "[InstanceEntry] The received msg's pointer in the msg entry is NULL!");
		return;
	}

	if ( pcMsg->event != OSP_POWERON )
	{
		brdagtlog( "[InstanceEntry] Msg.%d<%s> received!\n", pcMsg->event, ::OspEventDesc(pcMsg->event) );
	}
	
	switch( pcMsg->event )
	{
	case OSP_POWERON:                   //��������
        ProcBoardPowerOn( pcMsg ) ;
		break;
	case BRDAGENT_CONNECT_MANAGER_TIMER:
		ProcBoardConnectManagerTimeOut();
		break;


	case BRDAGENT_REGISTERA_TIMER:		        // ע��ʱ�䳬ʱ
		ProcBoardRegisterTimeOut( TRUE );
		break;

    case BRDAGENT_REGISTERB_TIMER:		        // ע��ʱ�䳬ʱ
		ProcBoardRegisterTimeOut( FALSE );
		break;

	case MPC_BOARD_REG_ACK:                     //ע��Ӧ����Ϣ
		ProcBoardRegAck( pcMsg );
		break;
		
	case MPC_BOARD_REG_NACK:			        //ע���Ӧ����Ϣ
		ProcBoardRegNAck( pcMsg );
		break;

	case BRDAGENT_GET_CONFIG_TIMER:             //�ȴ�����Ӧ����Ϣ��ʱ
		ProcGetConfigTimeOut( pcMsg );
		break;

	case MPC_BOARD_ALARM_SYNC_REQ:              //�������ĸ澯ͬ������
		ProcAlarmSyncReq( pcMsg );
		break;

	case MPC_BOARD_GET_VERSION_REQ:             //��ȡ����İ汾��Ϣ
		ProcGetVersionReq( pcMsg );
		break;

	case MPC_BOARD_GET_MODULE_REQ:              //��ȡ�����ģ����Ϣ
		ProcGetModuleInfoReq( pcMsg );
		break;

	case MPC_BOARD_TIME_SYNC_CMD:               //����ʱ��ͬ������
		ProcTimeSyncCmd( pcMsg );
		break;

	case MPC_BOARD_SELF_TEST_CMD:               //�����Բ�����
		ProcBoardSelfTestCmd( pcMsg );
		break;

	case MPC_BOARD_BIT_ERROR_TEST_CMD:          //���������������
		ProcBitErrorTestCmd( pcMsg );
		break;

	case MPC_BOARD_GET_STATISTICS_REQ:          //��ȡ�����ͳ����Ϣ
		ProcGetStatisticsReq( pcMsg );
		break;

	case MPC_BOARD_UPDATE_SOFTWARE_CMD:			//������������
		ProcUpdateSoftwareCmd( pcMsg );
		break;

	// �����Ǹ澯����������Ϣ
	case SVC_AGT_MEMORY_STATUS:                 //�ڴ�״̬�ı�
		ProcBoardMemeryStatus( pcMsg );
		break;

	case SVC_AGT_FILESYSTEM_STATUS:             //�ļ�ϵͳ״̬�ı�
		ProcBoardFileSystemStatus( pcMsg );
		break;

	// �����ǲ�������Ϣ
	case BOARD_MPC_CFG_TEST:
		ProcBoardGetCfgTest( pcMsg );
		break;

	case BOARD_MPC_GET_ALARM:
		ProcBoardGetAlarm( pcMsg );
		break;

	case BOARD_MPC_MANAGERCMD_TEST:
		ProcBoardGetLastManagerCmd( pcMsg );
		break;

	case MPC_BOARD_RESET_CMD:                   //��������������
		ProcBoardResetCmd( pcMsg );
		break;

	case OSP_DISCONNECT:
		ProcOspDisconnect( pcMsg );
		break;

	case BOARD_TEMPERATURE_STATUS_NOTIF:
		ProcBoardTempStatusNotif(pcMsg);
		break;
		
	case BOARD_CPU_STATUS_NOTIF:
		ProcBoardCpuStatusNotif(pcMsg);
		break;

	default:
		OspPrintf( TRUE, FALSE,  "[InstanceEntry] receive unknown msg %d<%s>! \n",
                                  pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
	return;
}

/*====================================================================
    ������      ��ConnectManager
    ����        ����MPC�ϵĹ����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bIsNodeA ����mpc A/B, TRUE:A, FALSE:B
    ����ֵ˵��  �����ӳɹ�����TRUE������ʧ�ܻ�������Ѿ������򷵻�FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2007/08/20  4.0         �ܹ��        ����
	2010/06/02	4.6         xueliang	  add filter
	20110105    4.6         pengjie       �ӿڲ�������
====================================================================*/
BOOL32 CBBoardAgent::ConnectManager(BOOL32 bIsNodeA)
{
	if( !IsNeedConnectNode(bIsNodeA) )
	{
		return FALSE;
	}

	u32 dwMpcIp = 0;
	u16 wMpcPort = 0;
	if( bIsNodeA )
	{
		dwMpcIp = m_pBBoardConfig->GetMpcIpA();
		wMpcPort = m_pBBoardConfig->GetMpcPortA();
	}
	else
	{
		dwMpcIp = m_pBBoardConfig->GetMpcIpB();
		wMpcPort = m_pBBoardConfig->GetMpcPortB();
	}
	
	u32 dwMcuNode = OspConnectTcpNode( htonl(dwMpcIp), wMpcPort, 10, 3, 100);
	if( !OspIsValidTcpNode(dwMcuNode) )
	{	
		OspPrintf(TRUE, FALSE, "[BoardAgent] CreateTcpNode Failed MpcIp.%x,MpcPort.%d!\n", dwMpcIp, wMpcPort);
	}
	else
	{
		OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());
		OspSetHBParam(dwMcuNode, m_pBBoardConfig->m_wDiscHeartBeatTime, m_pBBoardConfig->m_byDiscHeartBeatNum);
		if( bIsNodeA )
		{
			m_pBBoardConfig->m_dwDstMcuNodeA = dwMcuNode;
		}
		else
		{
			m_pBBoardConfig->m_dwDstMcuNodeB = dwMcuNode;
		}
		brdagtlog( "[BoardAgent] connect Success MpcIp.%x, Port.%d, Node:%d, wDiscHeartBeatTime:%d, byDiscHeartBeatNum:%d\n",
			dwMpcIp, wMpcPort, dwMcuNode, m_pBBoardConfig->m_wDiscHeartBeatTime, m_pBBoardConfig->m_byDiscHeartBeatNum);
		return TRUE;
	} 

	return FALSE;
}

/*====================================================================
    ������      IsNeedConnectNode
    ����        ���ж��Ƿ���Ҫ���ӵ�MPC A/B
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bIsNodeA  TRUE:Mpc A; FLASE:Mpc B
    ����ֵ˵��  ��False:����Ҫ����;True:��Ҫ��������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	20110105    4.6         pengjie       create
====================================================================*/
BOOL32 CBBoardAgent::IsNeedConnectNode( BOOL32 bIsNodeA )
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(IsNeedConnectNode)\n");
		return FALSE;
	}

	if( bIsNodeA == TRUE ) // Mpc A
	{
		if( m_pBBoardConfig->GetMpcIpA() != 0 && m_pBBoardConfig->GetMpcPortA() != 0 &&
			!OspIsValidTcpNode(m_pBBoardConfig->m_dwDstMcuNodeA) )
		{
			return TRUE;
		}
	}
	else // Mpc B
	{
		if( m_pBBoardConfig->GetMpcIpB() != 0 && m_pBBoardConfig->GetMpcPortB() != 0 &&
			!OspIsValidTcpNode(m_pBBoardConfig->m_dwDstMcuNodeB) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*=============================================================================
  �� �� ���� RegisterToMcuAgent
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CBBoardAgent::RegisterToMcuAgent(u32 dwDstNode)
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(RegisterToMcuAgent)\n");
		return;
	}

	OspPrintf(TRUE, FALSE, "[RegisterToMcuAgent] Register to Node.%d\n", dwDstNode );

    CServMsg  cReportMsg;
    u32 dwBrdIpAddr = htonl(m_pBBoardConfig->m_dwBrdIpAddr);

	TBrdPos tTmpPos;
	memcpy(&tTmpPos,&m_pBBoardConfig->m_tBoardPosition,sizeof(tTmpPos));
	
	OspPrintf(TRUE, FALSE, "[RegisterToMcuAgent] before adjust,brdId.0x%x\n",tTmpPos.byBrdID);
	printf("[RegisterToMcuAgent]before adjust,brdId.0x%x\n",tTmpPos.byBrdID);
	
	//ֻ��MPC A��ʱ�л�board type[2/1/2013 chendaiwei]
	if(m_byIsMpu2SimuMpu && !m_byIsAnyBrdRegSuccess)
	{
		if(dwDstNode == m_pBBoardConfig->m_dwDstMcuNodeA)
		{
			if(m_byRegBrdTypeFlagA== 0)
			{
				tTmpPos.byBrdID = BRD_TYPE_MPU;
				m_byRegBrdTypeFlagA = 1;
				printf("[RegisterToMcuAgent](1)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
				OspPrintf(TRUE, FALSE, "[RegisterToMcuAgent](1)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
			}
			else if(m_byRegBrdTypeFlagA == 1)
			{
				tTmpPos.byBrdID = 0x42;/*BRD_HWID_DSL8000_MPU*/
				m_byRegBrdTypeFlagA = 0;
				printf("[RegisterToMcuAgent](2)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
				OspPrintf(TRUE, FALSE, "[RegisterToMcuAgent](2)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
			}
		}
		else if(dwDstNode == m_pBBoardConfig->m_dwDstMcuNodeB)
		{
			if(m_byRegBrdTypeFlagB== 0)
			{
				tTmpPos.byBrdID = BRD_TYPE_MPU;
				m_byRegBrdTypeFlagB = 1; 
				printf("[RegisterToMcuAgent](3)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
				OspPrintf(TRUE, FALSE, "[RegisterToMcuAgent](3)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
			}
			else if(m_byRegBrdTypeFlagB == 1)
			{
				tTmpPos.byBrdID = 0x42;/*BRD_HWID_DSL8000_MPU*/
				m_byRegBrdTypeFlagB = 0;
				printf("[RegisterToMcuAgent](4)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
				OspPrintf(TRUE, FALSE, "[RegisterToMcuAgent](4)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
			}
		}
	}
	
	OspPrintf(TRUE, FALSE, "[RegisterToMcuAgent] after adjust,brdId.0x%x,m_byIsMpu2SimuMpu<%d>,m_byIsAnyBrdRegSuccess<%d>,m_byRegBrdTypeFlagA<%d>,m_byRegBrdTypeFlagB<%d>\n",
		tTmpPos.byBrdID,m_byIsMpu2SimuMpu,m_byIsAnyBrdRegSuccess,m_byRegBrdTypeFlagA,m_byRegBrdTypeFlagB);
	printf("[RegisterToMcuAgent] after adjust,brdId.0x%x,m_byIsMpu2SimuMpu<%d>,m_byIsAnyBrdRegSuccess<%d>,m_byRegBrdTypeFlagA<%d>,m_byRegBrdTypeFlagB<%d>\n",
		tTmpPos.byBrdID,m_byIsMpu2SimuMpu,m_byIsAnyBrdRegSuccess,m_byRegBrdTypeFlagA,m_byRegBrdTypeFlagB);
	
    cReportMsg.SetMsgBody((u8*)&tTmpPos, sizeof(TBrdPos));
    cReportMsg.CatMsgBody((u8*)&dwBrdIpAddr, sizeof(u32));
    cReportMsg.CatMsgBody(&m_pBBoardConfig->m_byBrdEthChoice, sizeof(u8));
	  
    // xsl [8/14/2006] ���Ӱ汾�����ϱ�
    u8  byOsType = 0;
#if defined WIN32
    byOsType = OS_TYPE_WIN32;
#elif defined _LINUX_
    byOsType = OS_TYPE_LINUX;
#else
    byOsType = OS_TYPE_VXWORKS;
#endif
    cReportMsg.CatMsgBody((u8*)&byOsType, sizeof(u8));

    post( MAKEIID(AID_MCU_BRDMGR, CInstance::DAEMON), BOARD_MPC_REG, 
            cReportMsg.GetServMsg(), cReportMsg.GetServMsgLen(), dwDstNode );

    return;
}

/*====================================================================
    ������      ��ProcBoardPowerOn
    ����        ������������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
	10/11/25	4.6			xliang	      recode
====================================================================*/
void CBBoardAgent::ProcBoardPowerOn( CMessage* const pcMsg )
{
	if ( NULL == pcMsg )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(ProcBoardPowerOn)\n");
		return;
	}
	
	memcpy( &m_pBBoardConfig, pcMsg->content, sizeof(CBBoardConfig*));
	if ( 0 == m_pBBoardConfig )
	{
		printf("[PowerOn] Config class point is zero!\n");
		return;
	}

	//�ж��Ƿ���MPU2ģ��MPU[2/5/2013 chendaiwei]
	if(m_pBBoardConfig->m_tBoardPosition.byBrdID == BRD_TYPE_MPU)
	{
#ifdef _LINUX12_

		TBrdE2PromInfo tBrdE2PromInfo;
		memset( &tBrdE2PromInfo, 0x0, sizeof(tBrdE2PromInfo) );
		s32 nRet = BrdGetE2PromInfo(&tBrdE2PromInfo); 
		if (ERROR == nRet)
		{
			printf("[BoardAgent][ProcBoardPowerOn] BrdGetE2PromInfo error!\n");
			return;
		}

		if(BRD_PID_KDV8000A_MPU2 == tBrdE2PromInfo.dwProductId)
		{
			m_byIsMpu2SimuMpu = 1;
			m_byIsAnyBrdRegSuccess = 0;
			printf("[ProcBoardPowerOn] Mpu2 simu MPU\n");
			OspPrintf(TRUE,FALSE,"[ProcBoardPowerOn] Mpu2 simu MPU\n");
		}
#endif
	}
	
    // zw 20081114 �������TRUEΪFALSE
	BOOL32 bRet = FALSE;
	switch( CurState() ) 
	{
	case STATE_IDLE:
		bRet = ProcConnect();
		if ( bRet == TRUE )
		{
			NEXTSTATE( STATE_INIT );
		}
		break;

	default:
		printf( "[PowerOn] Wrong state %u when in (ProcBoardPowerOn)!\n", CurState() );
		break;
	}
	return;
}

/*====================================================================
    ������      ProcConnect
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/06/01    4.6         xueliang       ����
	20110106    4.6         pengjie        �߼�����
====================================================================*/
BOOL32 CBBoardAgent::ProcConnect()
{
	if( ConnectManager(TRUE) ) // ������mpc A
	{
		brdagtlog("SetTimer BRDAGENT_REGISTERB_TIMER after connect node A ok!\n");
		SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT);
		return TRUE;
	}

	if( ConnectManager(FALSE) ) // ������mpc B
	{
		brdagtlog("SetTimer BRDAGENT_REGISTERB_TIMER after connect node B ok!\n");
		SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT);
		return TRUE;
	}

	if( IsNeedConnectNode(TRUE) || IsNeedConnectNode(FALSE) )
	{
		SetTimer( BRDAGENT_CONNECT_MANAGER_TIMER, CONNECT_MANAGER_TIMEOUT );
	}

	return FALSE;
}

/*====================================================================
    ������      ��ProcBoardConnectManagerTimeOut
    ����        ������MANAGER��ʱ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/06/01    4.6         xueliang       ����
====================================================================*/
void CBBoardAgent::ProcBoardConnectManagerTimeOut( void )
{
    BOOL bRet = FALSE;
    
	switch( CurState() ) 
	{
	case STATE_IDLE:
    case STATE_INIT:
    case STATE_NORMAL:
		bRet =  ProcConnect();
		/*
		if ( IsConnectA )
        {
			bRet = ConnectManager(m_pBBoardConfig->m_dwDstMcuNodeA, 
                m_pBBoardConfig->GetMpcIpA(), m_pBBoardConfig->GetMpcPortA());       //��MPC�ϵĹ����������
			if( TRUE == bRet)
			{
				SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT );
			}
			else 
			{
				//������IpB����
				bRet = ConnectManager(m_pBBoardConfig->m_dwDstMcuNodeA, 
					m_pBBoardConfig->GetMpcIpB(), m_pBBoardConfig->GetMpcPortB());

				if( bRet )
				{	
					// [5/26/2010 xliang] ipA��ipB������ɫ
					u32 dwTmpIp = m_pBBoardConfig->GetMpcIpA();
					u16 wTmpPort = m_pBBoardConfig->GetMpcPortA();
					m_pBBoardConfig->SetMpcIpA(m_pBBoardConfig->GetMpcIpB());
					m_pBBoardConfig->SetMpcPortA(m_pBBoardConfig->GetMpcPortB());
					m_pBBoardConfig->SetMpcIpB(dwTmpIp);
					m_pBBoardConfig->SetMpcPortB(wTmpPort);

					SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT );
				}
				else
				{
					SetTimer( BRDAGENT_CONNECT_MANAGERA_TIMER, CONNECT_MANAGER_TIMEOUT );
				}
			}
			
		}
		else
		{
			bRet = ConnectManager(m_pBBoardConfig->m_dwDstMcuNodeB, 
                m_pBBoardConfig->GetMpcIpB(), m_pBBoardConfig->GetMpcPortB());       //��MPC�ϵĹ����������
			if( TRUE == bRet)
			{
				SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT );
			}
			else
			{
				SetTimer( BRDAGENT_CONNECT_MANAGERB_TIMER, CONNECT_MANAGER_TIMEOUT );
			}
		}
		*/
        if(TRUE == bRet && STATE_IDLE == CurState())
        {
            NEXTSTATE( STATE_INIT );
        }
		break;

	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when in (ProcBoardConnectManagerTimeOut)!\n", CurState() );
		break;
	}
	return;
}

/*====================================================================
    ������      ��ProcBoardConnectManagerTimeOut
    ����        ������MANAGER��ʱ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
// void CBBoardAgent::ProcBoardConnectManagerTimeOut( BOOL32 IsConnectA )
// {
//     BOOL bRet = FALSE;
//     
// 	switch( CurState() ) 
// 	{
// 	case STATE_IDLE:
//     case STATE_INIT:
//     case STATE_NORMAL:
// 		if ( IsConnectA )
//         {
// 			bRet = ConnectManager(m_pBBoardConfig->m_dwDstMcuNodeA, 
//                 m_pBBoardConfig->GetMpcIpA(), m_pBBoardConfig->GetMpcPortA());       //��MPC�ϵĹ����������
// 			if( TRUE == bRet)
// 			{
// 				SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT );
// 			}
// 			else 
// 			{
// 				//������IpB����
// 				bRet = ConnectManager(m_pBBoardConfig->m_dwDstMcuNodeA, 
// 					m_pBBoardConfig->GetMpcIpB(), m_pBBoardConfig->GetMpcPortB());
// 
// 				if( bRet )
// 				{	
// 					// [5/26/2010 xliang] ipA��ipB������ɫ
// 					u32 dwTmpIp = m_pBBoardConfig->GetMpcIpA();
// 					u16 wTmpPort = m_pBBoardConfig->GetMpcPortA();
// 					m_pBBoardConfig->SetMpcIpA(m_pBBoardConfig->GetMpcIpB());
// 					m_pBBoardConfig->SetMpcPortA(m_pBBoardConfig->GetMpcPortB());
// 					m_pBBoardConfig->SetMpcIpB(dwTmpIp);
// 					m_pBBoardConfig->SetMpcPortB(wTmpPort);
// 
// 					SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT );
// 				}
// 				else
// 				{
// 					SetTimer( BRDAGENT_CONNECT_MANAGERA_TIMER, CONNECT_MANAGER_TIMEOUT );
// 				}
// 			}
// 		}
// 		else
// 		{
// 			bRet = ConnectManager(m_pBBoardConfig->m_dwDstMcuNodeB, 
//                 m_pBBoardConfig->GetMpcIpB(), m_pBBoardConfig->GetMpcPortB());       //��MPC�ϵĹ����������
// 			if( TRUE == bRet)
// 			{
// 				SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT );
// 			}
// 			else
// 			{
// 				SetTimer( BRDAGENT_CONNECT_MANAGERB_TIMER, CONNECT_MANAGER_TIMEOUT );
// 			}
// 		}
//  
//         if(TRUE == bRet && STATE_IDLE == CurState())
//         {
//             NEXTSTATE( STATE_INIT );
//         }
// 		break;
// 
// 	default:
// 		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when in (ProcBoardConnectManagerTimeOut)!\n", CurState() );
// 		break;
// 	}
// 	return;
// }



/*====================================================================
    ������      ��ProcBoardRegisterTimeOut
    ����        ���ȴ�ע�ᳬʱ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcBoardRegisterTimeOut( BOOL32 bIsConnectA )
{

	switch( CurState() )
	{
	case STATE_INIT:
    case STATE_NORMAL:
        if(TRUE == bIsConnectA)
        {
            RegisterToMcuAgent(m_pBBoardConfig->m_dwDstMcuNodeA);
            SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT );
        }
        else
        {
            RegisterToMcuAgent(m_pBBoardConfig->m_dwDstMcuNodeB);
            SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT );
        }
		break;

	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when in (ProcBoardRegisterTimeOut)!\n", 
            CurState() );
		break;
	}
	return;
}

/*====================================================================
    ������      ��ProcBoardRegNAck
    ����        ������ע����Ϣ�ķ�Ӧ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcBoardRegNAck( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_INIT:
    case STATE_NORMAL:
		break;

	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %d when receive RegNack!srcNodeId.%d\n", 
			CurState(),(pcMsg != NULL)?pcMsg->srcnode:0 );
		break;
	}
	return;

}

/*====================================================================
    ������      ��ProcBoardRegAck
    ����        ������ע����Ϣ��Ӧ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcBoardRegAck( CMessage* const pcMsg )
{

	if(NULL == pcMsg)
    {
        OspPrintf(TRUE, FALSE, "[Cri] The pointer cannot be Null. (ProcBoardRegAck)\n");
        return;
    }

	switch( CurState() ) 
	{
	case STATE_INIT:
    case STATE_NORMAL:
        MsgRegAck(pcMsg );	
		break;
       
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when receive Reg ack!\n", 
			CurState() );
		break;
	}
	return;

}

/*=============================================================================
  �� �� ���� UpdateIpPort
  ��    �ܣ� ����IP��port
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwIp (host sequence)
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  10/06/02    4.6         xueliang      ����
==============================================================================*/
void CBBoardAgent::UpdateIpPort(u32 dwIp, u16 wPort, BOOL32 bUpdateA)
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(UpdateIpPort)\n");
		return;
	}

	if( bUpdateA )
	{
		m_pBBoardConfig->SetMpcIpA(dwIp);
		m_pBBoardConfig->SetMpcPortA(wPort);
	}
	else
	{
		m_pBBoardConfig->SetMpcIpB(dwIp);
		m_pBBoardConfig->SetMpcPortB(wPort);
	}
	
	return;
}

/*=============================================================================
�� �� ���� ProcBoardTempStatusNotif
��    �ܣ� �����¶�״̬�仯ʱ�����͵����¶�״̬��MPC
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage* const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/10/25   4.0		chendaiwei            ����
=============================================================================*/
void CBBoardAgent::ProcBoardTempStatusNotif(CMessage* const pcMsg)
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];
	
	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );
	
	TBrdPos tBrdPos;
	memset(&tBrdPos,0,sizeof(tBrdPos));
	if( m_pBBoardConfig != NULL )
	{
		tBrdPos = m_pBBoardConfig->GetBrdPosition();
	}
		
    if( FindAlarm( ALARM_MCU_BRD_TEMP_ABNORMAL, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) )
    {
        if( pcMsg->content[0] == BRD_STATUS_NORMAL )    // normal
        {
            if( !DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ))
			{
                brdagtlog("brdAgent: DeleteAlarm( AlarmCode = %lu) failed!\n", tAlarmData.dwBoardAlarmCode);
			}
            else
			{
				tAlarmMsg.wEventId = SVC_AGT_BRD_TEMPERATURE_STATUS_NOTIFY;
				tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = pcMsg->content[0];
				tAlarmMsg.abyAlarmContent[4] = 0;
				
				memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );
				
				// �澯����� [10/25/2011 chendaiwei]
				*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 
				
				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );				
			}
        }
    }
    else //no such alarm
    {
        if(  pcMsg->content[1] == BRD_STATUS_ABNORMAL )    //abnormal
        {
            if( !AddAlarm( ALARM_MCU_BRD_TEMP_ABNORMAL, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ))
			{
                brdagtlog("brdAgent: AddAlarm( AlarmCode = %lu) failed!\n", ALARM_MCU_BRD_TEMP_ABNORMAL);
			}
            else
			{						
				tAlarmMsg.wEventId = SVC_AGT_BRD_TEMPERATURE_STATUS_NOTIFY;
				tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = pcMsg->content[0];
				tAlarmMsg.abyAlarmContent[4] = 0;
				
				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg);
				
				memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );
				
				// �澯����� [10/25/2011 chendaiwei]
				*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 
				
				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
        }
    }

	return;
}

/*=============================================================================
�� �� ���� ProcBoardCpuStatusNotif
��    �ܣ� ����CPUռ�����쳣�����л�ʱ�����͸澯��Ϣ��MPC
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage* const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/10/25   4.0		chendaiwei            ����
=============================================================================*/
void CBBoardAgent::ProcBoardCpuStatusNotif(CMessage* const pcMsg)
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];
	
	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );
	
	TBrdPos tBrdPos;
	memset(&tBrdPos,0,sizeof(tBrdPos));
	if( m_pBBoardConfig != NULL )
	{
		tBrdPos = m_pBBoardConfig->GetBrdPosition();
	}
		
    if( FindAlarm( ALARM_MCU_BRD_CPU_ABNORMAL, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) )
    {
        if( pcMsg->content[0] == BRD_STATUS_NORMAL )    // normal
        {
            if( !DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ))
			{
                brdagtlog("brdAgent: DeleteAlarm( AlarmCode = %lu) failed!\n", tAlarmData.dwBoardAlarmCode);
			}
            else
			{
				tAlarmMsg.wEventId = SVC_AGT_BRD_CPU_STATUS_NOTIF;
				tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = pcMsg->content[0];
				tAlarmMsg.abyAlarmContent[4] = 0;
				
				memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );
				
				// �澯����� [10/25/2011 chendaiwei]
				*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 
				
				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );				
			}
        }
    }
    else //no such alarm
    {
        if(  pcMsg->content[1] == BRD_STATUS_ABNORMAL )    //abnormal
        {
            if( !AddAlarm( ALARM_MCU_BRD_CPU_ABNORMAL, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ))
			{
				brdagtlog("brdAgent: AddAlarm( AlarmCode = %lu) failed!\n", ALARM_MCU_BRD_CPU_ABNORMAL);
			}
            else
			{						
				tAlarmMsg.wEventId = SVC_AGT_BRD_CPU_STATUS_NOTIF;
				tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = pcMsg->content[0];
				tAlarmMsg.abyAlarmContent[4] = 0;
				
				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg);
				
				memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );
				
				// �澯����� [10/25/2011 chendaiwei]
				*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 
				
				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
        }
    }
	
	return;
}

/*=============================================================================
  �� �� ���� MsgRegAck
  ��    �ܣ� ע��ɹ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  10/06/02    4.6         xueliang      ����
==============================================================================*/
void CBBoardAgent::MsgRegAck(CMessage* const pcMsg )
{
    if(NULL == pcMsg || NULL == m_pBBoardConfig)
    {
        OspPrintf(TRUE, FALSE, "The pointer can not be Null.(MsgRegAck)\n");
		return;
    }

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	BOOL32 bIsMpcActive = TRUE;
	TBrdRegAck tBrdRegAck;
	memcpy( &bIsMpcActive, cServMsg.GetMsgBody(), sizeof(BOOL32));
	memcpy( &tBrdRegAck, cServMsg.GetMsgBody()+sizeof(BOOL32), sizeof(TBrdRegAck) );

	printf("The other MPC is 0x%x:%d\n", tBrdRegAck.GetOtherMpcIp(), tBrdRegAck.GetOtherMpcPort());

	// ˫����־
	BOOL32 bIsDoubleLink = ( 0 != tBrdRegAck.GetOtherMpcIp() ) ? TRUE : FALSE;
	BOOL32 bSrcNodeA = TRUE;
	BOOL32 bNetParamChanged = FALSE;

	if(m_byIsMpu2SimuMpu)
	{
		printf("[MsgRegAck]m_byIsAnyBrdRegSuccess.%d m_byRegBrdTypeFlagA.%d m_byRegBrdTypeFlagB.%d\n",m_byIsAnyBrdRegSuccess,
			m_byRegBrdTypeFlagA,m_byRegBrdTypeFlagB);
		OspPrintf(TRUE,FALSE,"[MsgRegAck]m_byIsAnyBrdRegSuccess.%d m_byRegBrdTypeFlagA.%d m_byRegBrdTypeFlagB.%d\n",m_byIsAnyBrdRegSuccess,
			m_byRegBrdTypeFlagA,m_byRegBrdTypeFlagB);
	}

	// record IId
	if(pcMsg->srcnode == m_pBBoardConfig->m_dwDstMcuNodeA)
    {
		m_pBBoardConfig->m_dwDstMcuIIdA = pcMsg->srcid;

		if(m_byIsMpu2SimuMpu && !m_byIsAnyBrdRegSuccess)
		{
			m_byIsAnyBrdRegSuccess = 1;
			if(m_byRegBrdTypeFlagA == 0)
			{
				m_pBBoardConfig->m_tBoardPosition.byBrdID = 0x42;
			}
			else
			{
				m_pBBoardConfig->m_tBoardPosition.byBrdID = BRD_TYPE_MPU;
			}
			
			printf("[MsgRegAck]m_pBBoardConfig->m_tBoardPosition.byBrdId adjust to 0x%x\n",m_pBBoardConfig->m_tBoardPosition.byBrdID);
			OspPrintf(TRUE,FALSE,"[MsgRegAck]m_pBBoardConfig->m_tBoardPosition.byBrdId adjust to 0x%x\n",m_pBBoardConfig->m_tBoardPosition.byBrdID);
		}
	}
	else if(pcMsg->srcnode == m_pBBoardConfig->m_dwDstMcuNodeB)
    {
        m_pBBoardConfig->m_dwDstMcuIIdB = pcMsg->srcid;
		bSrcNodeA = FALSE;
		if(m_byIsMpu2SimuMpu && !m_byIsAnyBrdRegSuccess)
		{
			m_byIsAnyBrdRegSuccess = 1;
			if(m_byRegBrdTypeFlagB == 0)
			{
				m_pBBoardConfig->m_tBoardPosition.byBrdID = 0x42;
			}
			else
			{
				m_pBBoardConfig->m_tBoardPosition.byBrdID = BRD_TYPE_MPU;
			}
			
			printf("[MsgRegAck]m_pBBoardConfig->m_tBoardPosition.byBrdId adjust to 0x%x\n",m_pBBoardConfig->m_tBoardPosition.byBrdID);
			OspPrintf(TRUE,FALSE,"[MsgRegAck]m_pBBoardConfig->m_tBoardPosition.byBrdId adjust to 0x%x\n",m_pBBoardConfig->m_tBoardPosition.byBrdID);
		}
    }
	
	u16 wRegEvent = (bSrcNodeA ? BRDAGENT_REGISTERA_TIMER: BRDAGENT_REGISTERB_TIMER);
	KillTimer(wRegEvent);

	if( bIsDoubleLink )
	{
		u32 dwAlternativeNode	= bSrcNodeA ? m_pBBoardConfig->m_dwDstMcuNodeB: m_pBBoardConfig->m_dwDstMcuNodeA;
		u32 dwAlternativeIp		= bSrcNodeA ? m_pBBoardConfig->GetMpcIpB(): m_pBBoardConfig->GetMpcIpA();
		u16 wAlternativePort	= bSrcNodeA ? m_pBBoardConfig->GetMpcPortB(): m_pBBoardConfig->GetMpcPortA();

		if ( dwAlternativeIp != tBrdRegAck.GetOtherMpcIp() 
			|| wAlternativePort != tBrdRegAck.GetOtherMpcPort() )
		{
			OspPrintf(TRUE,FALSE,"[MsgRegAck]localothermpcip = %d,regack othermpcip:%d localothermpcport:%d,regack othermpcport:%d\n",dwAlternativeIp,tBrdRegAck.GetOtherMpcIp(),
				wAlternativePort,tBrdRegAck.GetOtherMpcPort());
			BOOL32 bNoSrcNodeA = !bSrcNodeA;
			UpdateIpPort(tBrdRegAck.GetOtherMpcIp(), tBrdRegAck.GetOtherMpcPort(), bNoSrcNodeA);
			bNetParamChanged = TRUE;
		}

		if( !OspIsValidTcpNode(dwAlternativeNode))
		{
			//if the alternative Node is invalid, then connect it using the Ip passed by Mcu
			ProcConnect();
		}
		else
		{	
			if(bNetParamChanged)
			{
				//if the alternative node is valid but the ip info doesn't match the ip passed by Mcu,
				//we disconnect it and reconnect using new ip.
				if ( !OspDisconnectTcpNode( dwAlternativeNode ) )
				{
					brdagtlog("[MsgRegAck] Disconnect alternative node.%d failed!\n", dwAlternativeNode);
					return;
				}
				OspPrintf(TRUE,FALSE,"[MsgRegAck]NetParamChanged,Disconnect!\n");
				ProcConnect();
			}
			else
			{
				//do nothing
			}
		}
	}

/*	
    if(pcMsg->srcnode == m_pBBoardConfig->m_dwDstMcuNodeA)
    {
		m_pBBoardConfig->m_dwDstMcuIIdA = pcMsg->srcid;
		KillTimer(BRDAGENT_REGISTERA_TIMER);
		
		if ( bIsDoubleLink )
		{
			// ��¼��һ��MPC��IP��PORT
			// [5/26/2010 xliang] ������Ϣ�е�����Ϊ׼
			if ( m_pBBoardConfig->GetMpcIpB() != tBrdRegAck.GetOtherMpcIp() 
				|| m_pBBoardConfig->GetMpcPortB() != tBrdRegAck.GetOtherMpcPort() )
			{
				m_pBBoardConfig->SetMpcIpB(tBrdRegAck.GetOtherMpcIp());
				m_pBBoardConfig->SetMpcPortB(tBrdRegAck.GetOtherMpcPort());
				bNetParamChanged = TRUE;
			}
			// ��ʼ����һ��彨��
			if ( ( bNetParamChanged 
				|| m_pBBoardConfig->m_dwDstMcuNodeB == INVALID_NODE
				|| m_pBBoardConfig->m_dwDstMcuIIdB == INVALID_INS)
				&& m_pBBoardConfig->GetMpcIpB() != 0
				)
			{
				// �ȶϿ��ϵ�TCP��
				if ( OspIsValidTcpNode(m_pBBoardConfig->m_dwDstMcuNodeB) )
				{
					if ( !OspDisconnectTcpNode( m_pBBoardConfig->m_dwDstMcuNodeB ) )
					{
						brdagtlog("[MsgRegAck] Disconnect old nodeB is failed!\n");
						return;
					}
				}
				
				bRet = ConnectManager(m_pBBoardConfig->m_dwDstMcuNodeB, 
						m_pBBoardConfig->GetMpcIpB(), m_pBBoardConfig->GetMpcPortB());       //��MPC�ϵĹ����������
				if( TRUE == bRet)
				{
					SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT );
				}
				else
				{
					SetTimer( BRDAGENT_CONNECT_MANAGERB_TIMER, CONNECT_MANAGER_TIMEOUT );
				}
			}
		} // if ( bIsDoubleLink )
    }
    else if(pcMsg->srcnode == m_pBBoardConfig->m_dwDstMcuNodeB)
    {
        m_pBBoardConfig->m_dwDstMcuIIdB = pcMsg->srcid;
        KillTimer(BRDAGENT_REGISTERB_TIMER);
    }
*/ 
	if( STATE_INIT == CurState() )
	{
        //guzh [2008/08/01] �Ǳ�׼����ע����̽���
        if (m_pBBoardConfig->m_tBoardPosition.byBrdLayer == BRD_LAYER_CUSTOM)
        {
            NEXTSTATE(STATE_NORMAL);
        }
        else
        {
            PostMsgToManager( BOARD_MPC_GET_CONFIG, (u8*)&m_pBBoardConfig->m_tBoardPosition, sizeof(TBrdPos) );		
            SetTimer( BRDAGENT_GET_CONFIG_TIMER, GET_CONFIG_TIMEOUT );	
        }
	}

    return;
}

/*====================================================================
    ������      ��ProcGetConfigTimeOut
    ����        ��ȡ�����ļ���ʱ����ʱ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcGetConfigTimeOut( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_INIT:
		// ���·���ȡ������Ϣ��Ϣ
		PostMsgToManager( BOARD_MPC_GET_CONFIG, (u8*)&m_pBBoardConfig->m_tBoardPosition, sizeof(TBrdPos) );		
		SetTimer( BRDAGENT_GET_CONFIG_TIMER, GET_CONFIG_TIMEOUT );	
		break;

	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when receive get configure!srcnode.%d\n", 
			CurState(), pcMsg != NULL?pcMsg->srcnode:0 );
		break;
	}
	return;
}

/*====================================================================
    ������      ��ProcAlarmSyncReq
    ����        ��MPC���͸�����ĸ澯ͬ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcAlarmSyncReq( CMessage* const pcMsg )
{
	if (NULL == m_pBBoardConfig)
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(ProcAlarmSyncReq)\n");
		return;
	}

	u16 wLoop;
	u16 wCount=0, wLen;
	u8 abyAlarmBuf[ MAXNUM_BOARD_ALARM*sizeof(TBoardAlarmMsgInfo) + 10 ];
	TBoardAlarmMsgInfo* pAlarmMsgInfo;

	memset(abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	switch( CurState() ) 
	{
    case STATE_INIT:
	case STATE_NORMAL:
		// �����Լ��������Ϣ
		memcpy(abyAlarmBuf, &m_pBBoardConfig->m_tBoardPosition, sizeof(TBrdPos) );

		// ɨ���Լ��ĸ澯��
		pAlarmMsgInfo = (TBoardAlarmMsgInfo*)(abyAlarmBuf + sizeof(TBrdPos) + sizeof(u16));
		for( wLoop = 0; wLoop < m_dwBoardAlarmTableMaxNo; wLoop++ )
		{
			if( m_atBoardAlarmTable[wLoop].bExist )
			{
				memcpy(pAlarmMsgInfo, &m_atBoardAlarmTable[wLoop].tBoardAlarm, sizeof(TBoardAlarmMsgInfo) );
				pAlarmMsgInfo++;
				wCount++;
			}
		}

		// �澯����
		*(u16*)( abyAlarmBuf + sizeof(TBrdPos) ) = htons(wCount); 
		wLen = sizeof(TBrdPos) + sizeof(u16) + wCount*sizeof(TBoardAlarmMsgInfo);

		PostMsgToManager( BOARD_MPC_ALARM_SYNC_ACK, abyAlarmBuf, wLen );
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    ������      ��ProcGetVersionReq
    ����        ��MPC���͸�����Ļ�ȡ�汾��Ϣ��Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcGetVersionReq( CMessage* const pcMsg )
{
	if (NULL == m_pBBoardConfig)
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(ProcGetVersionReq)\n");
		return;
	}

	s8 abyBuf[128];
	u16 wLen;

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
		//ȡ�汾��Ϣ
		{
		memset(abyBuf, 0, sizeof(abyBuf) );

		TBrdPos tBrdPosition = m_pBBoardConfig->GetBrdPosition();
		memcpy(abyBuf, &tBrdPosition,sizeof(tBrdPosition) );
		
		s8 achVersionBuf[128] = {0};
		strcpy(achVersionBuf, KDV_MCU_PREFIX);
		s8 achMon[16] = {0};
		u32 byDay = 0;
		u32 byMonth = 0;
		u32 wYear = 0;
		s8 achFullDate[24] = {0};
		
		s8 achDate[32] = {0};
		sprintf(achDate, "%s", __DATE__);
		StrUpper(achDate);
		sscanf(achDate, "%s %d %d", achMon, &byDay, &wYear );
		
		if ( 0 == strcmp( achMon, "JAN") )		 
			byMonth = 1;
		else if ( 0 == strcmp( achMon, "FEB") )
			byMonth = 2;
		else if ( 0 == strcmp( achMon, "MAR") )
			byMonth = 3;
		else if ( 0 == strcmp( achMon, "APR") )		 
			byMonth = 4;
		else if ( 0 == strcmp( achMon, "MAY") )
			byMonth = 5;
		else if ( 0 == strcmp( achMon, "JUN") )
			byMonth = 6;
		else if ( 0 == strcmp( achMon, "JUL") )
			byMonth = 7;
		else if ( 0 == strcmp( achMon, "AUG") )
			byMonth = 8;
		else if ( 0 == strcmp( achMon, "SEP") )		 
			byMonth = 9;
		else if ( 0 == strcmp( achMon, "OCT") )
			byMonth = 10;
		else if ( 0 == strcmp( achMon, "NOV") )
			byMonth = 11;
		else if ( 0 == strcmp( achMon, "DEC") )
			byMonth = 12;
		else
			byMonth = 0;
		
		if ( byMonth != 0 )
		{
			sprintf(achFullDate, "%04d%02d%02d", wYear, byMonth, byDay);
			sprintf(achVersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
		}
		else
		{
			// for debug information
			sprintf(achVersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
		}

		wLen = sprintf(abyBuf+sizeof(tBrdPosition),achVersionBuf);
   
		/*
		switch(tBrdPosition.byBrdID)
		{
			//bas vmp
		case BRD_TYPE_MPU:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_MPU);
			break;
		case BRD_TYPE_MPU2:
		case BRD_TYPE_MPU2ECARD:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_MPU2);
			break;
		case BRD_TYPE_APU:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_MIXER);
			break;
			
			//mixer
		case BRD_TYPE_APU2:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_APU2);
			break;
		case BRD_TYPE_EAPU:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_EMIXER);
			break;
			
			//tvwall
		case BRD_TYPE_DEC5:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_TW);
			break;
		case BRD_TYPE_HDU:
		case BRD_TYPE_HDU_L:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_HDU);
			break;
		case BRD_TYPE_HDU2:
		//case BRD_TYPE_HDU2_L:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_HDU2);
			break;
			
			//cri,dri..
		case BRD_TYPE_CRI:
		case BRD_TYPE_CRI2:
		case BRD_TYPE_IS21:
		case BRD_TYPE_IS22:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_CRIAGENT);
			break;
		case BRD_TYPE_DRI:
		case BRD_TYPE_DRI2:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_DRIAGENT);
			break;
			
		default:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),"Hardware %d| FPGA %d",
				BrdQueryHWVersion(), BrdQueryFPGAVersion() );
			break;
		}*/

		// [pengjie 2010/3/25]
		OspPrintf( TRUE, FALSE, "[Brd_Debug][ProcGetModuleInfoReq] id: %d, Layer: %d, Slot: %d \n",
			tBrdPosition.byBrdID,  tBrdPosition.byBrdLayer, tBrdPosition.byBrdSlot );
		printf( "[Brd_Debug][ProcGetModuleInfoReq] id: %d, Layer: %d, Slot: %d \n",
			tBrdPosition.byBrdID,  tBrdPosition.byBrdLayer, tBrdPosition.byBrdSlot );

		//���͸�MANAGER��Ӧ
		PostMsgToManager( BOARD_MPC_GET_VERSION_ACK, (u8*)abyBuf, wLen+sizeof(TBrdPos)+1 );
		break;
		}
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}


/*====================================================================
    ������      ��ProcGetModuleInfoReq
    ����        ��MPC���͸������ȡ�����ģ����Ϣ��Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcGetModuleInfoReq( CMessage* const pcMsg )
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(ProcGetModuleInfoReq)!\n");
		return;
	}

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:		
		//���͸�MANAGER��Ӧ
		{
		TBrdPos tBrdPosition = m_pBBoardConfig->GetBrdPosition();
		PostMsgToManager( BOARD_MPC_GET_MODULE_ACK, (u8*)&tBrdPosition, sizeof(tBrdPosition) );
		// [pengjie 2010/3/25]
		OspPrintf( TRUE, FALSE, "[Brd_Debug][ProcGetModuleInfoReq] id: %d, Layer: %d, Slot: %d \n",
			tBrdPosition.byBrdID,  tBrdPosition.byBrdLayer, tBrdPosition.byBrdSlot );
		printf( "[Brd_Debug][ProcGetModuleInfoReq] id: %d, Layer: %d, Slot: %d \n",
			tBrdPosition.byBrdID,  tBrdPosition.byBrdLayer, tBrdPosition.byBrdSlot );

		break;
		}
		
	default:
		log( LOGLVL_EXCEPTION, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    ������      ��ProcBoardResetCmd
    ����        ��MPC���͸�����������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcBoardResetCmd( CMessage* const pcMsg )
{

    switch(CurState())
    {
    case STATE_IDLE:
    case STATE_INIT:
    case STATE_NORMAL:
        OspDelay(1000);		
        BrdHwReset();
        OspPrintf(TRUE, FALSE, "[ProcBoardResetCmd] Reboot local brd...\n");
        break;

    default:
        OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n",
                                pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState());
        break;
    }
	return;
}

/*====================================================================
    ������      ��ProcUpdateSoftwareCmd
    ����        ��MPC���͸������������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcUpdateSoftwareCmd( CMessage* const pcMsg )
{
	if( NULL == pcMsg->content )
	{
		OspPrintf(TRUE, FALSE, "[ProcUpdateSoftwareCmd] Error input parameter .\n");
		return;
	}	

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8 byBrdId = *(u8*)cServMsg.GetMsgBody();
    u8 byNum   = *(u8*)(cServMsg.GetMsgBody()+sizeof(u8));
    u8 byTmpNum = byNum;

    u8 abyRet[MAXNUM_FILE_UPDATE];  // �������ļ���Ӧ�ĳɹ������1-�ɹ���0-ʧ�� (��Ի��)
    memset(abyRet, 1, sizeof(abyRet));

    u8 byFileNum = 0;      // �Ѿ��������ļ���
	brdagtlog( "BoardAgent: Rcv Update Software Msg: byBrdId.%d byNum.%d\n", byBrdId, byNum);
	
	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
		if( m_pBBoardConfig->m_bUpdateSoftBoard )
		{
			s8 achFileName[256];
            s8 achTmpName[256];
            s8 *lpMsgBody = (s8*)(cServMsg.GetMsgBody()+sizeof(u8)+sizeof(u8));
			u8  byLen = 0;
            u16 wOffSet = 0;

			while( byTmpNum-- > 0 )
			{
                
                
				memset( achFileName, 0, sizeof(achFileName) );
                byLen = *lpMsgBody;
                if (byLen + wOffSet > cServMsg.GetMsgBodyLen()-2)
                {
                    OspPrintf(TRUE, FALSE, "[ProcUpdateSoftwareCmd] invalid msgbody!!!\n");
                    abyRet[byFileNum] = 0;
                    break;
                }
                memcpy(achFileName, lpMsgBody+sizeof(u8), byLen);
                wOffSet = sizeof(byLen) + byLen;
                lpMsgBody += wOffSet;
                brdagtlog( "[ProcUpdateSoftwareCmd] filename: %s\n", achFileName);
				
				// �����ϴ��ļ���
				memcpy(achTmpName, achFileName, sizeof(achFileName));

                // �ж��Ƿ��д��ļ���
				FILE *hR = fopen( achTmpName, "r" );
				if( NULL == hR )
				{
                    abyRet[byFileNum] = 0;
					continue;
				}
				else
				{
					fclose( hR );
				}
				
                //vx �¸���
#ifdef _VXWORKS_    
				//ȥ����׺
				achFileName[strlen(achFileName) - strlen("upd")] = 0;
				brdagtlog( "[ProcUpdateSoftwareCmd] updating file name is :%s\n", achFileName);
				
				s8 achOldFile[KDV_MAX_PATH];
				memset(achOldFile, 0, sizeof(achOldFile));
				memcpy(achOldFile, achFileName, strlen(achFileName));
								
				hR = fopen( achOldFile, "r" );
				if( NULL != hR )
				{
					fclose( hR );
					strcat(achOldFile, "old");
					
					s32 nRet = rename( achFileName, achOldFile );
					if( ERROR == nRet )
					{
                        abyRet[byFileNum] = 0;
						brdagtlog( "[ProcUpdateSoftwareCmd] fail to rename file %s to %s.\n", achFileName, achOldFile);
					}
					else// ���ļ�����
					{
						nRet = rename(achTmpName, achFileName);
						if( ERROR == nRet )
						{
                            abyRet[byFileNum] = 0;
							brdagtlog( "[ProcUpdateSoftwareCmd] fail to rename file %s to %s.\n", achTmpName, achFileName);						
						}
						
						remove( achOldFile );
					}
				}
				else
				{
					// ���ļ�����
					rename(achTmpName, achFileName);
				}
#elif defined _LINUX_
                //���õײ�ӿ�appupdate...
                s32 nNameOffBin = strlen(achFileName) - strlen(LINUXAPP_POSTFIX_BIN);
                s32 nNameOffImage = strlen(achFileName) - strlen(LINUXAPP_POSTFIX);
				s32 nNameOffLinux = strlen(achFileName) - strlen(LINUXOS_POSTFIX);
                
				printf("[ProcUpdateSoftwareCmd] start update, file.%s\n", achFileName+nNameOffBin);
				brdagtlog("[ProcUpdateSoftwareCmd] start update, file.%s\n", achFileName+nNameOffBin);
		#ifdef _LINUX12_				
				//�ж��Ƿ�bin�ļ�
				if ( nNameOffBin > 0 && 0 == strcmp(achFileName+nNameOffBin, LINUXAPP_POSTFIX_BIN) )
				{
					printf("[ProcUpdateSoftwareCmd] update ram file.%s as bin\n", achFileName);
					brdagtlog("[ProcUpdateSoftwareCmd] update ram file.%s as bin\n", achFileName);
					
					s32 nRetCode = BrdCheckUpdatePackage(achFileName);
					if( nRetCode != OK )
					{
						LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,"[ProcUpdateSoftwareCmd] Board<T:%d,L:%d,S:%d>check package[%s] failed,error.%d!\n",
							m_pBBoardConfig->GetBrdPosition().byBrdID,
							m_pBBoardConfig->GetBrdPosition().byBrdLayer,
							m_pBBoardConfig->GetBrdPosition().byBrdSlot,
							achFileName, nRetCode);

						abyRet[byFileNum] = 0;
						if( CHK_PID_FAILURE == nRetCode )
						{
							abyRet[byFileNum] = 2;
						}

						//return;
						continue;
					}

					TBrdPos tBrdPosion = m_pBBoardConfig->GetBrdPosition();					
					if (BRD_TYPE_IS22 == tBrdPosion.byBrdID)
					{//IS2.2��BrdAllSysUpdate����
						if ( OK != BrdAllSysUpdate(achFileName, 1) )
						{
							abyRet[byFileNum] = 0;
							printf("[ProcUpdateBrdVersion] BrdAllSysUpdate failed.\n");
							brdagtlog( "[ProcUpdateBrdVersion] BrdUpdateAppBin failed.\n");
						}
						else
						{
							printf("[ProcUpdateBrdVersion] BrdAllSysUpdate success.\n");
							brdagtlog( "[ProcUpdateBrdVersion] BrdUpdateAppBin success.\n");
						}
					}
					else
					{//��������BrdSysUpdate()����
						if ( SYSTEM_UPDATE_SUCCESS != BrdSysUpdate(achFileName) )
						{
							abyRet[byFileNum] = 0;
							printf("[ProcUpdateBrdVersion] BrdUpdateAppBin failed.\n");
							brdagtlog( "[ProcUpdateBrdVersion] BrdUpdateAppBin failed.\n");
						}
						else
						{
							printf( "[ProcUpdateBrdVersion] BrdUpdateAppBin success.\n");
							brdagtlog( "[ProcUpdateBrdVersion] BrdUpdateAppBin success.\n");
						}
					}					
				}

				//�ж��Ƿ�image�ļ�
                else 
		#endif					
				if (nNameOffImage > 0 && 0 == strcmp(achFileName+nNameOffImage, LINUXAPP_POSTFIX) )
                {
					printf("[ProcUpdateSoftwareCmd] update ram file.%s as image\n", achFileName);
					brdagtlog("[ProcUpdateSoftwareCmd] update ram file.%s as image\n", achFileName);

                    if ( OK != BrdUpdateAppImage(achFileName) )
                    {
                        abyRet[byFileNum] = 0;
                        brdagtlog( "[ProcUpdateBrdVersion] BrdUpdateAppImage failed.\n");
                    }
                    else
                    {
                        brdagtlog( "[ProcUpdateBrdVersion] BrdUpdateAppImage success.\n");
                    }
                }
                else if (nNameOffLinux > 0 && 0 == strcmp(achFileName+nNameOffLinux, LINUXOS_POSTFIX) )
                {
					printf("[ProcUpdateSoftwareCmd] update ram file.%s as os\n", achFileName);
					brdagtlog("[ProcUpdateSoftwareCmd] update ram file.%s as os\n", achFileName);

					if ( OK != BrdFpUpdateAuxData(achFileName) )
					{
						abyRet[byFileNum] = 0;
						brdagtlog( "[ProcUpdateBrdVersion] BrdFpUpdateAuxData failed.\n");
					}
					else
					{
						brdagtlog( "[ProcUpdateBrdVersion] BrdFpUpdateAuxData success.\n");
					}
				}   
				else
				{
					brdagtlog( "[ProcUpdateBrdVersion] not supported file type.\n");
				}
              
#endif
				byFileNum ++;
///FIXME: WIndows����                
			}
		}

        // ������������������Ϣ��ȫ���ķ��أ�ͳһ��MCU������
        {
            u8 * pbyMsg   = cServMsg.GetMsgBody() + sizeof(u8) * 2;
            u8   byMsgLen = cServMsg.GetMsgBodyLen() - 2;
            
            CServMsg cServ(pcMsg->content, pcMsg->length);
            cServ.SetMsgBody(&byBrdId,sizeof(u8));
            cServ.CatMsgBody(&byNum,  sizeof(u8));
            cServ.CatMsgBody(abyRet, byNum);
            cServ.CatMsgBody(pbyMsg, byMsgLen);
            
            PostMsgToManager( BOARD_MPC_UPDATE_NOTIFY, cServ.GetServMsg(), cServ.GetServMsgLen());
            
            // [zw] [09/10/2008] �����ɹ���������
            if ( UPDATE_NMS == cServMsg.GetChnIndex() )
            {
                s32 nIndex = 0;
                for( ; nIndex < byNum; nIndex ++ )
                {
                    if ( abyRet[nIndex] == 0 )
                    {
                        brdagtlog("[ProcUpdateBrdVersion] file.%d update failed !\n", nIndex);
                        return;
                    }
                }
                BrdHwReset();
            }
        }
		break;
		
	default:
		OspPrintf( TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			 pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    ������      ��ProcTimeSyncCmd
    ����        ��MPC���͸������ʱ��ͬ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcTimeSyncCmd( CMessage* const pcMsg )
{
    s8 achTemp[50];
#ifdef WIN32 
    SYSTEMTIME  systemTime;
#else
    struct tm tmTime;
#endif

    switch( CurState() ) 
    {
    case STATE_INIT:
    case STATE_NORMAL:		
    #ifndef WIN32		
        achTemp[4] = '\0';
        memcpy( achTemp, pcMsg->content, 4 );
        tmTime.tm_year = atoi( achTemp ) - 1900;
        achTemp[2] = '\0';
        memcpy( achTemp, pcMsg->content + 4, 2 );
        tmTime.tm_mon = atoi( achTemp ) - 1;
        memcpy( achTemp, pcMsg->content + 6, 2 );
        tmTime.tm_mday = atoi( achTemp );
        memcpy( achTemp, pcMsg->content + 8, 2 );
        tmTime.tm_hour = atoi( achTemp );
        memcpy( achTemp, pcMsg->content + 10, 2 );
        tmTime.tm_min = atoi( achTemp );
        memcpy( achTemp, pcMsg->content + 12, 2 );
        tmTime.tm_sec = atoi( achTemp );

        /* set clock chip time */
        if( BrdTimeSet( &tmTime ) == ERROR )
        {
            printf( "BrdTimeSet failed!\n" );
            return;
        }
    #else
        achTemp[4] = '\0';
        memcpy( achTemp, pcMsg->content, 4 );
        systemTime.wYear = atoi( achTemp );
        achTemp[2] = '\0';
        memcpy( achTemp, pcMsg->content + 4, 2 );
        systemTime.wMonth = atoi( achTemp );
        memcpy( achTemp, pcMsg->content + 6, 2 );
        systemTime.wDay = atoi( achTemp );
        memcpy( achTemp, pcMsg->content + 8, 2 );
        systemTime.wHour = atoi( achTemp );
        memcpy( achTemp, pcMsg->content + 10, 2 );
        systemTime.wMinute = atoi( achTemp );
        memcpy( achTemp, pcMsg->content + 12, 2 );
        systemTime.wSecond = atoi( achTemp );	 

        SetLocalTime( &systemTime );	  
    #endif
        //������
        TestingProcess( pcMsg );
        break;

    default:
        OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        break;
    }

    return;
}

/*====================================================================
    ������      ��ProcBoardSelfTestCmd
    ����        ��MPC���͸�������Բ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcBoardSelfTestCmd( CMessage* const pcMsg )
{
	switch( CurState() ) 
	{
	case STATE_NORMAL:
		//������
		TestingProcess( pcMsg );
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*=============================================================================
�� �� ���� TestingProcess
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage* const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/27   4.0			�ܹ��                  ����
=============================================================================*/
void CBBoardAgent::TestingProcess( CMessage* const pcMsg )
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(TestingProcess)\n");
		return;
	}

	if( m_pBBoardConfig->m_bIsTest == FALSE )
		return ;
	
	m_pBBoardConfig->m_wLastEvent = pcMsg->event;
	if(pcMsg->content != NULL)
	{
		if( pcMsg->length < sizeof(m_pBBoardConfig->m_abyLastEventPara) )
        {
			memcpy(m_pBBoardConfig->m_abyLastEventPara, pcMsg->content, pcMsg->length );
        }
		else
        {
			memcpy(m_pBBoardConfig->m_abyLastEventPara, pcMsg->content, sizeof(m_pBBoardConfig->m_abyLastEventPara) );
        }
	}
	else
	{
		memset(m_pBBoardConfig->m_abyLastEventPara, 0, sizeof(m_pBBoardConfig->m_abyLastEventPara) );
	} 
}

/*====================================================================
    ������      ��ProcBitErrorTestCmd
    ����        ��MPC���͸������������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcBitErrorTestCmd( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_NORMAL:
		//������
		TestingProcess( pcMsg );
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    ������      ��ProcGetStatisticsReq
    ����        ��MPC���͸������ȡ�����ͳ����Ϣ��Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcGetStatisticsReq( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_NORMAL:
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    ������      ��PostMsgToManager
    ����        ��������Ϣ��MPC�ĵ���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wEvent: ��Ϣ��
				  u8 * const pbyContent: ��Ϣ���� 
				  u16 wLen: ��Ϣ���ݳ���
    ����ֵ˵��  ���ɹ�����TRUE����֮����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
BOOL32 CBBoardAgent::PostMsgToManager( u16 wEvent, u8 * const pbyContent, u16 wLen )
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(PostMsgToManager)!\n");
		return FALSE;
	}

	if( OspIsValidTcpNode( m_pBBoardConfig->m_dwDstMcuNodeA ) )
	{
		post( m_pBBoardConfig->m_dwDstMcuIIdA, wEvent, pbyContent, wLen, m_pBBoardConfig->m_dwDstMcuNodeA );
		brdagtlog("Post message %u(%s) to Board manager A!\n", wEvent, ::OspEventDesc( wEvent ));
	}
	else
	{
		brdagtlog("BoardAgent: Post message %u(%s) to Board manager A ERROR!\n", wEvent, ::OspEventDesc( wEvent ));
	}

    if(OspIsValidTcpNode(m_pBBoardConfig->m_dwDstMcuNodeB))
    {
        post( m_pBBoardConfig->m_dwDstMcuIIdB, wEvent, pbyContent, wLen, m_pBBoardConfig->m_dwDstMcuNodeB );
		brdagtlog(" Post message %u(%s) to Board manager B!\n", wEvent, ::OspEventDesc( wEvent ));
    }
    else
    {
		brdagtlog( "BoardAgent: Post message %u(%s) to Board manager B ERROR!\n", wEvent, ::OspEventDesc( wEvent ));
    }
    return TRUE;
}

/*====================================================================
    ������      ��ProcOspDisconnect
    ����        ��OSP�͵���Ͽ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
    04/11/11    3.5         �� ��         �½ӿڵ��޸�
	10/06/01	4.6			xueliang	  ����
====================================================================*/
void CBBoardAgent::ProcOspDisconnect( CMessage* const pcMsg )
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(ProcOspDisconnect)!\n");
		return;
	}

    u32 dwNode = *(u32*)pcMsg->content;
    if ( INVALID_NODE != dwNode )
    {
        OspDisconnectTcpNode(dwNode);
    }
    
    if(dwNode == m_pBBoardConfig->m_dwDstMcuNodeA)
    {
        FreeDataA();
    }
    else if(dwNode == m_pBBoardConfig->m_dwDstMcuNodeB)
    {
        FreeDataB();
    }

	printf("[ProcOspDisconnect]m_pBBoardConfig->m_dwDstMcuNodeA.%d m_pBBoardConfig->m_dwDstMcuNodeB.%d brdId.%d\n",m_pBBoardConfig->m_dwDstMcuNodeA,
		m_pBBoardConfig->m_dwDstMcuNodeB,m_pBBoardConfig->m_tBoardPosition.byBrdID);
	OspPrintf(TRUE,FALSE,"[ProcOspDisconnect]m_pBBoardConfig->m_dwDstMcuNodeA.%d m_pBBoardConfig->m_dwDstMcuNodeB.%d brdId.%d\n",m_pBBoardConfig->m_dwDstMcuNodeA,
		m_pBBoardConfig->m_dwDstMcuNodeB,m_pBBoardConfig->m_tBoardPosition.byBrdID);
	
	if(m_pBBoardConfig->m_dwDstMcuNodeA == INVALID_NODE
		&& m_pBBoardConfig->m_dwDstMcuNodeB == INVALID_NODE
		&& m_byIsAnyBrdRegSuccess)
	{
		printf("[ProcOspDisconnect]m_byIsAnyBrdRegSuccess set to 0!\n");
		OspPrintf(TRUE,FALSE,"[ProcOspDisconnect]m_byIsAnyBrdRegSuccess set to 0!\n");
		m_byIsAnyBrdRegSuccess = 0;
	}

	SetTimer( BRDAGENT_CONNECT_MANAGER_TIMER, CONNECT_MANAGER_TIMEOUT );
	
	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:

        if(INVALID_NODE == m_pBBoardConfig->m_dwDstMcuNodeA && INVALID_NODE == m_pBBoardConfig->m_dwDstMcuNodeB )
        {

		    BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF );
            NEXTSTATE( STATE_IDLE );
            OspDelay(5*1000);
        }
        break;
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when in (ProcOspDisconnect)!\n", CurState() );
		break;
	}

	return;
}

/*====================================================================
    ������      ��ProcBoardMemeryStatus
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
void CBBoardAgent::ProcBoardMemeryStatus( CMessage* const pcMsg )
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[ALARM_MCU_OBJECT_LENGTH];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	if( FindAlarm( ALARM_MCU_MEMORYERROR, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) )
	{
		if( *( pcMsg->content ) == 0 )      //normal
		{
			if( DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ) == FALSE )
			{
				brdagtlog( "DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
				tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0], 
				tAlarmData.byBoardAlarmObject[1] );
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_pBBoardConfig->m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_pBBoardConfig->m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_pBBoardConfig->m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );
			
				memcpy(abyAlarmBuf, &m_pBBoardConfig->m_tBoardPosition,sizeof(TBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(TBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(TBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(TBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	else        //no such alarm
	{
		if( *( pcMsg->content ) == 1 )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_MEMORYERROR, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) == FALSE )
			{
				brdagtlog( "AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_pBBoardConfig->m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_pBBoardConfig->m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_pBBoardConfig->m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg);

				memcpy(abyAlarmBuf, &m_pBBoardConfig->m_tBoardPosition,sizeof(TBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(TBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(TBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(TBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	return;
}

/*====================================================================
    ������      ��ProcBoardModuleStatus
    ����        ������ģ��״̬�澯
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcBoardModuleStatus( CMessage* const pcMsg )
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[ALARM_MCU_OBJECT_LENGTH];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	if( FindAlarm( ALARM_MCU_MODULE_OFFLINE, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) )
	{
		if( 0 == *( pcMsg->content ))      //normal
		{
			if( DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ) == FALSE )
			{
				brdagtlog( "DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
				tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0], 
				tAlarmData.byBoardAlarmObject[1] );
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_pBBoardConfig->m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_pBBoardConfig->m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_pBBoardConfig->m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );
			
				memcpy(abyAlarmBuf, &m_pBBoardConfig->m_tBoardPosition,sizeof(TBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(TBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(TBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(TBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	else        //no such alarm
	{
		if( 1 == *( pcMsg->content ) )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_MODULE_OFFLINE, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) == FALSE )
			{
				brdagtlog( "AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_pBBoardConfig->m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_pBBoardConfig->m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_pBBoardConfig->m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg );
			
				memcpy(abyAlarmBuf, &m_pBBoardConfig->m_tBoardPosition,sizeof(TBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(TBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(TBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(TBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	return;
}

/*====================================================================
    ������      ��ProcBoardFileSystemStatus
    ����        ���ļ�ϵͳ״̬�ı���Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcBoardFileSystemStatus( CMessage* const pcMsg )
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[ALARM_MCU_OBJECT_LENGTH];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	if( FindAlarm( ALARM_MCU_FILESYSTEMERROR, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) )
	{
		if( *( pcMsg->content ) == 0 )      //normal
		{
			if( DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ) == FALSE )
			{
				brdagtlog( "DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
				tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0], 
				tAlarmData.byBoardAlarmObject[1] );
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_pBBoardConfig->m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_pBBoardConfig->m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_pBBoardConfig->m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );
			
				memcpy(abyAlarmBuf, &m_pBBoardConfig->m_tBoardPosition,sizeof(TBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(TBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(TBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(TBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	else        //no such alarm
	{
		if( *( pcMsg->content ) == 1 )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_FILESYSTEMERROR, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) == FALSE )
			{
				brdagtlog( " AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_pBBoardConfig->m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_pBBoardConfig->m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_pBBoardConfig->m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg );
			
				memcpy(abyAlarmBuf, &m_pBBoardConfig->m_tBoardPosition,sizeof(TBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(TBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(TBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(TBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	return;
}


/*====================================================================
    ������      ��AddAlarm
    ����        ����澯�������Ӹ澯
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����DWORD dwAlarmCode, �澯��
                  u8 byObjType, �澯��������
                  u8 byObject[], �澯����
                  TBoardAlarm *ptData, ���صĸ澯��Ϣ
    ����ֵ˵��  ���ɹ�����TRUE������FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
BOOL32 CBBoardAgent::AddAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData )
{
    u16    wLoop;

	/*�ҵ�һ���ձ���*/
    for( wLoop = 0; wLoop < m_dwBoardAlarmTableMaxNo; wLoop++ )
    {
        if( !m_atBoardAlarmTable[wLoop].bExist )
        {
            break;
        }
    }
    
    if( wLoop >= sizeof( m_atBoardAlarmTable ) / sizeof( m_atBoardAlarmTable[0] ) )
    {
        return( FALSE );
    }

	/*��д�ø澯����*/
	u8 byLen1 = ALARM_MCU_OBJECT_LENGTH;
	u8 bylen2 = sizeof( m_atBoardAlarmTable[0].byBoardAlarmObject );
    m_atBoardAlarmTable[wLoop].dwBoardAlarmCode = dwAlarmCode;
    m_atBoardAlarmTable[wLoop].byBoardAlarmObjType = byObjType;
    memcpy( m_atBoardAlarmTable[wLoop].byBoardAlarmObject, byObject, min(byLen1, bylen2) );
    m_atBoardAlarmTable[wLoop].bExist = TRUE;
	
    // copy
    ptData->dwBoardAlarmSerialNo = wLoop + 1;
    ptData->dwBoardAlarmCode = m_atBoardAlarmTable[wLoop].dwBoardAlarmCode;
    ptData->byBoardAlarmObjType = m_atBoardAlarmTable[wLoop].byBoardAlarmObjType;
    memcpy( ptData->byBoardAlarmObject, m_atBoardAlarmTable[wLoop].byBoardAlarmObject, sizeof( ptData->byBoardAlarmObject ) );
	
    // increase maxno
    if( wLoop == m_dwBoardAlarmTableMaxNo )
    {
        m_dwBoardAlarmTableMaxNo++;
    }

	return TRUE;
}

/*====================================================================
    ������      ��SetAlarmMsgInfo
    ����        ����������ø澯����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����DWORD dwSerialNo: �澯�����к�
	              CMessage* const pcMsg: ����ø澯����Ϣ
    ����ֵ˵��  ���ɹ�����TRUE������FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
BOOL32 CBBoardAgent::SetAlarmMsgInfo( u32 dwSerialNo, TBoardAlarmMsgInfo* const ptMsg )
{
	m_atBoardAlarmTable[dwSerialNo - 1].tBoardAlarm = *ptMsg;

	return ( TRUE );
}

/*====================================================================
    ������      ��DeleteAlarm
    ����        ���Ӹ澯����ɾ���澯
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����DWORD dwSerialNo: �澯�����к�
    ����ֵ˵��  ���ɹ�����TRUE������FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
BOOL32 CBBoardAgent::DeleteAlarm( u32 dwSerialNo )
{	
    s32   dwLoop;

    m_atBoardAlarmTable[dwSerialNo - 1].bExist = FALSE;
        
    if( dwSerialNo == m_dwBoardAlarmTableMaxNo )     //the last one
    {
        for( dwLoop = m_dwBoardAlarmTableMaxNo - 1; dwLoop >= 0; dwLoop-- )
        {
            if( m_atBoardAlarmTable[dwLoop].bExist )
            {
                break;
            }
        }
        m_dwBoardAlarmTableMaxNo = dwLoop + 1;  //reset MaxNo
    }

    return( TRUE );
}

/*====================================================================
    ������      ��FindAlarm
    ����        ���Ӹ澯����Ҹ澯
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����DWORD dwAlarmCode:  �澯��
				  u8 byObjType: �澯��������
				  u8 byObject[]: �澯����
				  TBoardAlarm *ptData: ���صĸ澯��Ϣ
    ����ֵ˵��  ���ɹ�����TRUE������FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
BOOL32 CBBoardAgent::FindAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData )
{
    u16    wLoop;
    BOOL    bFind = FALSE;

	/*���Ҹ澯���е�ÿһ��*/
    for( wLoop = 0; wLoop < m_dwBoardAlarmTableMaxNo; wLoop++ )
    {
        if( !m_atBoardAlarmTable[wLoop].bExist ) /*������ڣ���������һ��*/
        {
            continue;
        }

		/*�澯��͸澯��������Ҫһ��*/
        if( dwAlarmCode == m_atBoardAlarmTable[wLoop].dwBoardAlarmCode 
            && byObjType == m_atBoardAlarmTable[wLoop].byBoardAlarmObjType )
        {
			/*�澯����Ҫһ��*/
            switch( byObjType )
            {
			case ALARMOBJ_MCU:
				bFind = TRUE;
				break;

            case ALARMOBJ_MCU_LINK:
                if( byObject[0] == m_atBoardAlarmTable[wLoop].byBoardAlarmObject[0] 
                    && byObject[1] == m_atBoardAlarmTable[wLoop].byBoardAlarmObject[1] )
                {
                    bFind = TRUE;
                }
                break;

            default:
                brdagtlog( " Wrong AlarmObjType in FindAlarm()!\n" );
                break;
            }  //end switch
        
            if( bFind ) 
				break;
        }   //end if
    }

    if( bFind )
    {
        //copy
		ptData->dwBoardAlarmSerialNo = wLoop+1;
        ptData->dwBoardAlarmCode = m_atBoardAlarmTable[wLoop].dwBoardAlarmCode;
        ptData->byBoardAlarmObjType = m_atBoardAlarmTable[wLoop].byBoardAlarmObjType;
        memcpy( ptData->byBoardAlarmObject, m_atBoardAlarmTable[wLoop].byBoardAlarmObject, 
            sizeof( ptData->byBoardAlarmObject ) );
    }

    return( bFind );
}

/*====================================================================
    ������      ��ProcBoardGetCfgTest
    ����        ������ʱȡ���õ���Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage* const pcMsg 
    ����ֵ˵��  ���ɹ�����TRUE������FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcBoardGetCfgTest( CMessage* const pcMsg )
{
/*	STRUCT_mcueqpMpEntry tMpCfg;

	tMpCfg = g_cBrdAgentApp.m_tMpConfig;

	tMpCfg.mcueqpMpEntConnMcuPort = htons(tMpCfg.mcueqpMpEntConnMcuPort);
	tMpCfg.mcueqpMpEntConnMcuIp = htonl(tMpCfg.mcueqpMpEntConnMcuIp);
	tMpCfg.mcueqpMpEntIpAddr = htonl(tMpCfg.mcueqpMpEntIpAddr);
	
	SetSyncAck( &tMpCfg, sizeof(tMpCfg) );*/
	OspPrintf(TRUE,FALSE,"[ProcBoardGetCfgTest] srcnode.%d\n",pcMsg!=NULL ? pcMsg->srcnode:0);
	return ;
}
/*====================================================================
    ������      ��ProcBoardGetAlarm
    ����        ������ʱȡ�澯��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage* const pcMsg 
    ����ֵ˵��  ���ɹ�����TRUE������FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcBoardGetAlarm( CMessage* const pcMsg )
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(ProcBoardGetAlarm).srcnode.%d\n",pcMsg!=NULL?pcMsg->srcnode:0);
		return;
	}

	u16 wLoop;
	u16 wCount=0;
	u8 abyAlarmBuf[ 1024];
	u8* pbyBuf;

	//ɨ���Լ��ĸ澯��
	pbyBuf = abyAlarmBuf + sizeof(u16);
	for( wLoop=0; wLoop<m_dwBoardAlarmTableMaxNo; wLoop++ )
	{
		if( m_atBoardAlarmTable[wLoop].bExist )
		{
			*(u32*)pbyBuf =  htonl( m_atBoardAlarmTable[wLoop].dwBoardAlarmCode );
			pbyBuf += sizeof(m_atBoardAlarmTable[wLoop].dwBoardAlarmCode);
			*pbyBuf = m_atBoardAlarmTable[wLoop].byBoardAlarmObjType;
			pbyBuf ++;
			*pbyBuf = m_pBBoardConfig->m_tBoardPosition.byBrdLayer;
			pbyBuf++;
			*pbyBuf = m_pBBoardConfig->m_tBoardPosition.byBrdSlot;
			pbyBuf++;
			*pbyBuf = m_pBBoardConfig->m_tBoardPosition.byBrdID;
			pbyBuf++;
			memcpy( pbyBuf, m_atBoardAlarmTable[wLoop].byBoardAlarmObject, 2 );
			pbyBuf += 2;

			wCount++;
		}
	}
	
	//�澯����
	*(u16*)abyAlarmBuf = htons(wCount); 

	SetSyncAck( abyAlarmBuf, pbyBuf - abyAlarmBuf );
	return ;

}

/*====================================================================
    ������      ��ProcBoardGetLastManagerCmd
    ����        ������ʱȡ�澯��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage* const pcMsg 
    ����ֵ˵��  ���ɹ�����TRUE������FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBBoardAgent::ProcBoardGetLastManagerCmd( CMessage* const pcMsg )
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(ProcBoardGetLastManagerCmd).srcnode.%d\n",pcMsg!=NULL?pcMsg->srcnode:0);
		return;
	}

	u8 abyBuf[128];

	//ֱ�ӷ������һ����Ϣ�������Ϣ
	*(u16*)abyBuf = htons(m_pBBoardConfig->m_wLastEvent);
	memcpy(abyBuf+2, m_pBBoardConfig->m_abyLastEventPara, sizeof(m_pBBoardConfig->m_abyLastEventPara) );
	SetSyncAck( abyBuf, sizeof(u16)+sizeof(m_pBBoardConfig->m_abyLastEventPara) );
	return;
}

/*=============================================================================
�� �� ���� FreeDataA
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/28   4.0			�ܹ��                  ����
=============================================================================*/
void CBBoardAgent::FreeDataA(void)
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(FreeDataA)\n");
		return;
	}

	m_pBBoardConfig->m_dwDstMcuNodeA = INVALID_NODE;
	m_pBBoardConfig->m_dwDstMcuIIdA  = INVALID_INS;
    return;
}

/*=============================================================================
�� �� ���� FreeDataB
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/28   4.0			�ܹ��                  ����
=============================================================================*/
void CBBoardAgent::FreeDataB(void)
{
    if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(FreeDataB)\n");
		return;
	}

    m_pBBoardConfig->m_dwDstMcuNodeB = INVALID_NODE;
    m_pBBoardConfig->m_dwDstMcuIIdB  = INVALID_INS;
    return;
}

/*=============================================================================
�� �� ���� brdagtlog
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� char * fmt
           ...
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/28   4.0			�ܹ��                  ����
=============================================================================*/
/*lint -save -e438 -e530*/
void CBBoardAgent::brdagtlog(char * fmt, ...)
{
    if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(brdagtlog)\n");
		return;
	}

	va_list argptr;
	va_start(argptr, fmt);
    m_pBBoardConfig->brdlog(fmt, argptr);
	va_end(argptr);
}
/*lint -restore*/

/*====================================================================
������      : AddBrdAgtEventStr
����        : ��ӵ��������Ϣ�ַ���
�㷨ʵ��    : 
����ȫ�ֱ���: ��
�������˵��: 
����ֵ˵��  : ��
----------------------------------------------------------------------
�޸ļ�¼    :
��  ��      �汾        �޸���      �޸�����
2012/05/05  4.7         liaokang      ����
====================================================================*/
void AddBrdAgtEventStr()
{    
/*lint -save -e537*/
#ifdef OSPEVENT
#undef OSPEVENT
#endif
#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )

#ifdef _EV_AGTSVC_H_
    #undef _EV_AGTSVC_H_
    #include "evagtsvc.h"
#else
#include "evagtsvc.h"
#endif
/*lint -restore*/
}

// API void byebye()
// {
//     OspPost( MAKEIID(AID_MCU_BRDAGENT,1), MPC_BOARD_RESET_CMD );
//     
// }
