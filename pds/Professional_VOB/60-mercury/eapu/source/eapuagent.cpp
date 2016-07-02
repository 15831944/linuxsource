#include "eapuagent.h"

#include "boardguardbasic.h"

CBrdAgentApp	g_cBrdAgentApp;

static SEMHANDLE       g_semEAPU;

BOOL32 CBoardConfig::ReadConfig()
{
	// ��ȡ���ӵ�MCU��Ϣ
    BOOL32 bRet = CBBoardConfig::ReadConnectMcuInfo();
    if ( !bRet )
    {
        return bRet;
    }

	s32 sdwDefault = 0;
	s32 sdwReturnValue = 0;
	s8  achProfileName[64] = {0};
	memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, "mcueqp.ini");
	bRet = GetRegKeyInt( achProfileName, SECTION_EqpMixer, KEY_IsSimuApu, sdwDefault, &sdwReturnValue);
	if( bRet && sdwReturnValue != 0 )  
	{
		m_bIsSimuApu = TRUE;
	}
	else
	{
		m_bIsSimuApu = FALSE;
	}
    return TRUE;
}
#ifdef _LUNUX_
void AgentAPIEnableInLinux()
{
}
#endif
API BOOL InitBrdAgent()
{
	static BOOL32 bIsBrdInitSuccessFul = FALSE;//���ʼ���Ƿ�ɹ�,��ֹ��ζ԰���г�ʼ������,��һ��ʱĬ�ϲ��ɹ�
	if ( bIsBrdInitSuccessFul )
	{
		printf( "[InitBrdAgent] Already Inited Successfully,So Return!\n" );
		return TRUE;
	}
    if ( ::BrdInit() == ERROR )
    {
        printf( "[InitBrdAgent]BrdInit() failed,so return!\n" );
        return FALSE;
    }
    else
    {
		bIsBrdInitSuccessFul = TRUE;
        printf( "[InitBrdAgent]BrdInit() succeed!\n" );
    }
    if ( ::IsOspInitd() )
    {
		printf( "[InitBrdAgent]Osp Has Been Inited!\n" );
    }
	else
	{
		::OspInit( TRUE, BRD_TELNET_PORT );
	}
	
#ifndef WIN32
	OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );

    // �ж��Ƿ������������
	if( BrdGetE2promTestFlag() )		
	{
		u32 dwBrdIpAddr = 0;
		if( ::BrdGetE2promIpAddr(&dwBrdIpAddr) == ERROR)//�����ڼ�IP��E2PROM�ж�ȡ
		{
			printf( "[IsProductTest]Read Board Ip from E2PROM error! so do'not do product test\n" );
			return FALSE;
		}
		printf( "[IsProductTest]Board Ip(network sequence) while in ProductTest is: %x\n", dwBrdIpAddr);
		
		//����ip
		//�ж���E0 or E1. (���ڶ����Ļ�����0��Ϊ׼)
		u8 byId = 0;
		u8 byState = 0;
		u8 byEthId = ~0;
		for(; byId < 2; byId ++)
		{
			if( OK != ::BrdGetEthLinkStat(byId, &byState) )
			{
				printf(" Call BrdGetEthLinkStat(%u) ERROR!\n", byId);
				byEthId = 0;
				break;
			}
			if( byState == 1 && byEthId == (u8)~0)
			{
				byEthId = byId;
			}
			::BrdDelEthParam(byId);	//��ȫ������Է���ip�費��
		}
		
		TBrdEthParam tEtherPara;
		tEtherPara.dwIpAdrs = dwBrdIpAddr;
		tEtherPara.dwIpMask = inet_addr("255.0.0.0");
		memset( tEtherPara.byMacAdrs, 0, sizeof(tEtherPara.byMacAdrs) );
		printf("[BrdSetEthParam]Set Eth(%d)  IP(%d)(%x) \n",byEthId,dwBrdIpAddr,dwBrdIpAddr);
		if( ERROR == ::BrdSetEthParam( byEthId, Brd_SET_IP_AND_MASK, &tEtherPara ) )
		{
			printf("[BrdSetEthParam]Set Eth %u IP failed\n",byEthId);
		}
		else
		{
			printf("[BrdSetEthParam]Set Eth %u Ip successfully!\n", byEthId);
		}

		g_cBrdAgentApp.SetProductTest(TRUE);
		OspPrintf(TRUE,FALSE,"starting product test  ......\n");
		printf("starting product test  ......\n");
		return TRUE;
	}
#else
	s32 sdwDefault = 0;
	s32 sdwReturnValue = 0;
	s8  achProfileName[64] = {0};
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");

	// �ж��Ƿ������������
	if( ::GetRegKeyInt( achProfileName, "BoardDebug", "IsTest", sdwDefault, &sdwReturnValue ) 
		&& sdwReturnValue != 0 )  
	{
		g_cBrdAgentApp.SetProductTest(TRUE);
		OspPrintf(TRUE,FALSE,"starting product test  ......\n");
		printf("starting product test  ......\n");

		return TRUE;
	}
#endif

    if ( !::OspSemBCreate(&g_semEAPU) )
	{
		OspPrintf( TRUE, FALSE,"[InitBrdAgent]OspSemBCreate() failed\n" );
        printf( "[InitBrdAgent]OspSemBCreate() failed\n" );
		return FALSE;
	}
    if ( !::OspSemTake( g_semEAPU ) )
    {
        OspPrintf( TRUE, FALSE, "[InitBrdAgent]OspSemTake(g_semEAPU) error!\n" );
		printf( "[InitBrdAgent]OspSemTake(g_semEAPU) error!\n" );
		return FALSE;
    }

	// ��ȡ������Ϣ
	if( !g_cBrdAgentApp.ReadConfig() )  
	{
		OspPrintf( TRUE, FALSE, "[InitBrdAgent]ReadConfig error!\n" );
		printf( "[InitBrdAgent]ReadConfig() failed.!\n" );
		return FALSE;
	}

	// �����������͵�������Ӧ��
	g_cBrdAgentApp.CreateApp( "EapuAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );
	g_cBrdGuardApp.CreateApp( "EapuGuard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);

    TBrdPos tBoardPosition;
    tBoardPosition = g_cBrdAgentApp.GetBrdPosition();
	
	// �Ƿ�ģ��APU��Ϣ
	if ( g_cBrdAgentApp.IsSimuApu() )
	{
		OspPrintf( TRUE, FALSE, "[InitBrdAgent]EAPU SIMU APU!\n" );
		printf( "[InitBrdAgent]EAPU SIMU APU!\n" );
		tBoardPosition.byBrdID = BRD_TYPE_APU;
	}

	OspPrintf(TRUE,FALSE,"**************info:byBrdID=[%d]byBrdLayer=[%d]byBrdSlot=[%d]**************\n",tBoardPosition.byBrdID,
		tBoardPosition.byBrdLayer,tBoardPosition.byBrdSlot);
	printf("**************info:byBrdID=[%d]byBrdLayer=[%d]byBrdSlot=[%d]**************\n",tBoardPosition.byBrdID,
		tBoardPosition.byBrdLayer,tBoardPosition.byBrdSlot);
    g_cBrdAgentApp.SetBrdPosition( tBoardPosition );

	CBBoardConfig *pBBoardConfig = (CBBoardConfig*)&g_cBrdAgentApp;
	// �������ʼ����
	OspPost( MAKEIID(AID_MCU_BRDAGENT, 1), OSP_POWERON, &pBBoardConfig, sizeof(pBBoardConfig) );
	// ��������ģ�鿪ʼ����
	// �����������浥��������Ϣ��������ݲ�ͬ���������в�ͬ�澯 [12/1/2011 chendaiwei]
	u8 byBrdId = pBBoardConfig->GetBrdPosition().byBrdID;
	OspPost( MAKEIID(AID_MCU_BRDGUARD,0,0), BOARD_GUARD_POWERON,&byBrdId,sizeof(u8));
    
	if ( !OspSemTake( g_semEAPU) )
    {
        OspPrintf( TRUE, FALSE, "[InitBrdAgent]OspSemTake(g_semEAPU) error!\n" );
    }
	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0 );	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0 );
	return TRUE;
}


API u32 BrdGetDstMcuNode()
{
    return g_cBrdAgentApp.m_dwDstMcuNodeA;
}

API u32 BrdGetDstMcuNodeB()
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}
void CBoardAgent::InstanceEntry( CMessage * const pcMsg )
{
	if ( NULL == pcMsg )
	{
		OspPrintf( TRUE, FALSE, "[InstanceEntry]the pointer can not be NULL!\n" );
		return;
	}

	OspPrintf( TRUE, FALSE, "[InstanceEntry] %u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event) );

	switch( pcMsg->event )
	{
// 	case OSP_POWERON:                      //��������
// 	case BRDAGENT_CONNECT_MANAGERA_TIMER:  //���ӹ������ʱ
//     case BRDAGENT_CONNECT_MANAGERB_TIMER:  //���ӹ������ʱ
// 	case BRDAGENT_REGISTERA_TIMER:		   //ע��ʱ�䳬ʱ
//     case BRDAGENT_REGISTERB_TIMER:		   //ע��ʱ�䳬ʱ
// 	case MPC_BOARD_REG_ACK:                //ע��Ӧ����Ϣ
// 	case MPC_BOARD_REG_NACK:			   //ע���Ӧ����Ϣ
// 	case BRDAGENT_GET_CONFIG_TIMER:        //�ȴ�����Ӧ����Ϣ��ʱ
// 	case MPC_BOARD_ALARM_SYNC_REQ:         //�������ĸ澯ͬ������
// 	case MPC_BOARD_BIT_ERROR_TEST_CMD:     //���������������
// 	case MPC_BOARD_TIME_SYNC_CMD:          //����ʱ��ͬ������
// 	case MPC_BOARD_SELF_TEST_CMD:          //�����Բ�����
// 	case MPC_BOARD_RESET_CMD:              //��������������
// 	case MPC_BOARD_UPDATE_SOFTWARE_CMD:    //�����������
// 	case MPC_BOARD_GET_STATISTICS_REQ:     //��ȡ�����ͳ����Ϣ
// 	case MPC_BOARD_GET_VERSION_REQ:        //��ȡ����İ汾��Ϣ
// 	case OSP_DISCONNECT:
// 	//�����Ǹ澯����������Ϣ
// 	case SVC_AGT_MEMORY_STATUS:            //�ڴ�״̬�ı�
// 	case SVC_AGT_FILESYSTEM_STATUS:        //�ļ�ϵͳ״̬�ı�
// 	//�����ǲ�������Ϣ
// 	case BOARD_MPC_CFG_TEST:
// 	case BOARD_MPC_GET_ALARM:
// 	case BOARD_MPC_MANAGERCMD_TEST:
// 	case MPC_BOARD_GET_MODULE_REQ:         //��ȡ�����ģ����Ϣ
// 		CBBoardAgent::InstanceEntry( pcMsg );
// 		break;

	case MPC_BOARD_GET_CONFIG_ACK:         //ȡ������ϢӦ����Ϣ
		ProcBoardGetConfigAck( pcMsg );
		break;
		
	case MPC_BOARD_GET_CONFIG_NACK:        //ȡ������Ϣ��Ӧ��
		ProcBoardGetConfigNAck( pcMsg );
		break;

	case MPC_BOARD_LED_STATUS_REQ:
		ProcLedStatusReq( pcMsg );
		break;

	case BOARD_LED_STATUS:
		ProcBoardLedStatus( pcMsg );
		break;

	case MCU_BOARD_CONFIGMODIFY_NOTIF:
		ProcBoardConfigModify();
		break;

	default:
		CBBoardAgent::InstanceEntry( pcMsg );
		break;
//         OspPrintf( TRUE, FALSE, "[InstanceEntry][error] BoardAgent: receive unknown msg %d<%s> in mmpAgent InstanceEntry! \n",
//                               pcMsg->event, OspEventDesc(pcMsg->event) );
// 		break;
	}
	
	return;
}

void CBoardAgent::ProcBoardGetConfigAck( CMessage * const pcMsg )
{
    if( NULL == pcMsg )
    {
        OspPrintf( TRUE, FALSE, "[ProcBoardGetConfigAck][error] ProcBoardGetConfigAck pointer is Null\n" );
        return;
    }
    u8	byPeriCount;
	u8	byPeriType;
	u16	wLoop, wIndex;
	TEqpMixerEntry*	ptMixerCfg;		//MIXER��������Ϣ
	if (pcMsg->content == NULL)
	{
		OspPrintf( TRUE, FALSE, "[ProcBoardGetConfigAck][error] pcMsg->content == NULL\n" );
		return;
	}
	byPeriCount = pcMsg->content[0]; // �������
	g_cBrdAgentApp.SetEMixerNum(byPeriCount);
	wIndex = 1;
	for( wLoop = 0; wLoop < min(MAXNUM_EAPU_MIXER,byPeriCount); wLoop++ )
	{			
		byPeriType = pcMsg->content[wIndex++];  // ��������
		switch( byPeriType )                    // ���øõ�����Ӧ��������Ϣ
		{
			case EQP_TYPE_MIXER: //��������Դ��EAPU�п���ģ��APU[2/14/2012 chendaiwei]
			case 12: //  [2/14/2012 chendaiwei]ԭEQP_TYPE_EMIXERֵΪ12,V4R7 MCU�����ϵ�EAPU���Իᷢ��ֵ12��EAPU�塣����������EAPU�����EAPU�塣
				ptMixerCfg = (TEqpMixerEntry*)(pcMsg->content + wIndex);
				wIndex += sizeof(TEqpMixerEntry);
				g_cBrdAgentApp.SetEqpEMixerEntry(wLoop,*ptMixerCfg);
				break;
			default:
				OspPrintf( TRUE, FALSE, "*******unknown type[%d]*****\n",byPeriType);
		}
	}
    BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON );// ��MLINK�Ƶ���
    // �ͷ��ź���
    OspSemGive( g_semEAPU );
	KillTimer( BRDAGENT_GET_CONFIG_TIMER );
	NEXTSTATE( STATE_NORMAL );
	return;
}

void CBoardAgent::ProcBoardGetConfigNAck(CMessage* const pcMsg)
{
    OspPrintf( TRUE, FALSE, "[eapu][error] ProcBoardGetConfigNAck(%d)\n",pcMsg != NULL ? pcMsg->srcnode : 0 );
    return;
}

void CBoardAgent::ProcLedStatusReq( CMessage* const pcMsg )
{
#ifndef WIN32
	u8 abyBuf[32];
	u8 byCount;
    TBrdLedState tBrdLedState;

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
        BrdQueryLedState( &tBrdLedState );
        memcpy(&m_tLedState, &tBrdLedState, sizeof(tBrdLedState));
		
        TBrdPos tBrdPos;
        tBrdPos = g_cBrdAgentApp.GetBrdPosition();

		memcpy(abyBuf, &tBrdPos, sizeof(tBrdPos) );
		byCount = (u8)m_tLedState.dwLedNum;
		memcpy(abyBuf+sizeof(tBrdPos), &byCount, sizeof(byCount) ); 
		memcpy(abyBuf+sizeof(tBrdPos)+sizeof(byCount), m_tLedState.nlunion.byLedNo, byCount);
		
		PostMsgToManager( BOARD_MPC_LED_STATUS_ACK, abyBuf, 
			              sizeof(tBrdPos)+sizeof(u8)+ byCount * sizeof(u8) );
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
#endif
	OspPrintf(TRUE, FALSE, "BoardAgent:message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
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
#ifndef WIN32
	u8 abyBuf[32];
	u8 byCount;

	TBrdLedState tLedState;
	memcpy( &tLedState, pcMsg->content, sizeof(TBrdLedState) );

	if( memcmp(m_tLedState.nlunion.byLedNo, tLedState.nlunion.byLedNo, tLedState.dwLedNum) == 0 )
		return;

	memcpy(&m_tLedState, &tLedState, sizeof(m_tLedState));

	TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );
	byCount = (u8)tLedState.dwLedNum;
	memcpy(abyBuf+sizeof(TBrdPos), &byCount, sizeof(u8) ); 
	memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), tLedState.nlunion.byLedNo, byCount);

	if (g_cBrdAgentApp.m_bPrintBrdLog)
	{
		OspPrintf(TRUE, FALSE, "CBoardAgent::ProcLedStatusReq: Led:\n");
		
		for (u32 dwIndex = 0; dwIndex < m_tLedState.dwLedNum; ++dwIndex)
		{
			OspPrintf(TRUE, FALSE," %d ", m_tLedState.nlunion.byLedNo[dwIndex]);
		}
		OspPrintf(TRUE, FALSE, "\n");
	}
					
	PostMsgToManager( BOARD_MPC_LED_STATUS_NOTIFY, abyBuf, 
		sizeof(TBrdPos)+sizeof(u8)+byCount * sizeof(u8) );
#endif
	OspPrintf(TRUE, FALSE, "BoardAgent:message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
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
void CBoardAgent::ProcBoardConfigModify()
{

}