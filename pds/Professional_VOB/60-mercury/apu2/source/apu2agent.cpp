#include "apu2agent.h"
#include "boardguardbasic.h"

CBrdAgentApp	g_cBrdAgentApp;
static SEMHANDLE       g_semApu2;


/*====================================================================
������      ��ReadConfig
����        ����ȡApu2����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��True or False
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
BOOL32 CBoardConfig::ReadConfig()
{
    if (!CBBoardConfig::ReadConnectMcuInfo() )
    {
		printf("[ReadConfig]ReadConnectMcuInfo failed!!!\n");
        return FALSE;
    }
	else
	{
		printf("[ReadConfig]ReadConnectMcuInfo success\n");
	}

	s32 sdwSimuApuDefault = 0;
	s32 sdwQuaLvlDefault = 0;
	s32 sdwReturnValue = 0;
	s8  achProfileName[64] = {0};
	memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, "mcueqp.ini");
	BOOL32 bRet = TRUE;

	// ��ȡģ���
	bRet = GetRegKeyInt( achProfileName, SECTION_EqpMixer, KEY_IsSimuApu, sdwSimuApuDefault, &sdwReturnValue);
	if( bRet && sdwReturnValue != 0 )  
	{
		m_bIsSimuApu = TRUE;
		GetRegKeyInt( achProfileName, SECTION_EqpMixer, KEY_SimApuVer, 4504, &sdwReturnValue);
		m_wSimApuVer = (u16)sdwReturnValue;
	}
	else
	{
		m_bIsSimuApu = FALSE;
	}

	// ��ȡ��Ƶ�����ȼ�
	bRet = GetRegKeyInt( achProfileName, SECTION_EqpMixer , KEY_QualityLvl, sdwQuaLvlDefault, &sdwReturnValue );
	m_byQualityLvl = (u8)sdwReturnValue;

    return TRUE;
}

/*====================================================================
������      ��SetEqpEMixerEntry
����        ������Apu2Mixer����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��True or False
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
BOOL32 CBoardConfig::SetEqpEMixerEntry(u8 byMixIdx,TEqpMixerEntry tEqpMixerEntry) 
{ 
	m_tApu2Cfg.m_acMixerCfg[byMixIdx].byEqpId   = tEqpMixerEntry.GetEqpId();
	m_tApu2Cfg.m_acMixerCfg[byMixIdx].byEqpType = EQP_TYPE_MIXER;
	
	if ( TRUE == m_bIsSimuApu )
	{
		m_tApu2Cfg.m_acMixerCfg[byMixIdx].byEqpType		= EQP_TYPE_MIXER;
		m_tApu2Cfg.m_acMixerCfg[byMixIdx].m_byMemberNum = MAXNUM_APU_MIXING_MEMBER;
	}
	else
	{
		m_tApu2Cfg.m_acMixerCfg[byMixIdx].m_byMemberNum = tEqpMixerEntry.GetMaxChnInGrp();
	}
	
	m_tApu2Cfg.m_acMixerCfg[byMixIdx].wMcuId            = LOCAL_MCUID;
	m_tApu2Cfg.m_acMixerCfg[byMixIdx].wRcvStartPort     = tEqpMixerEntry.GetEqpRecvPort();
	strcpy(m_tApu2Cfg.m_acMixerCfg[byMixIdx].achAlias, tEqpMixerEntry.GetAlias());

	m_tApu2Cfg.m_acMixerCfg[byMixIdx].achAlias[MAXLEN_EQP_ALIAS-1] = '\0';
	m_tApu2Cfg.m_acMixerCfg[byMixIdx].m_byIsSimuApu = m_bIsSimuApu;
	m_tApu2Cfg.m_acMixerCfg[byMixIdx].m_wSimApuVer = m_wSimApuVer;
	m_tApu2Cfg.m_acMixerCfg[byMixIdx].m_byQualityLvl = m_byQualityLvl;
	return TRUE;
}

/*====================================================================
������      ��SetEqpBasEntry
����        ������Apu2Bas����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��True or False
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/28  2.0         ������          ����
====================================================================*/
BOOL32 CBoardConfig::SetEqpBasEntry(u8 byBasIdx, TEqpMpuBasEntry tEqpMpuBasEntry)
{
	m_tApu2Cfg.m_acBasCfg[byBasIdx].byEqpId = tEqpMpuBasEntry.GetEqpId();
	m_tApu2Cfg.m_acBasCfg[byBasIdx].byEqpType = EQP_TYPE_BAS;
	m_tApu2Cfg.m_acBasCfg[byBasIdx].wMcuId = LOCAL_MCUID;
	m_tApu2Cfg.m_acBasCfg[byBasIdx].wRcvStartPort = tEqpMpuBasEntry.GetEqpRecvPort();
	strcpy(m_tApu2Cfg.m_acBasCfg[byBasIdx].achAlias, tEqpMpuBasEntry.GetAlias());
	m_tApu2Cfg.m_acBasCfg[byBasIdx].achAlias[MAXLEN_EQP_ALIAS - 1] = '\0';
	m_tApu2Cfg.m_acBasCfg[byBasIdx].m_byQualityLvl = m_byQualityLvl;
	return TRUE;
}


/*====================================================================
������      ��SetApu2Num
����        ������Apu2Mixer������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 byMixerNum
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
void CBoardConfig::SetApu2MixerNum(u8 byMixerNum)
{
	m_tApu2Cfg.m_byMixerNum = min(MAXNUM_APU2_MIXER, byMixerNum);
}

/*====================================================================
������      ��SetApu2Num
����        ������Apu2Bas������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 byBasNum
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/28  2.0         ������        ����
====================================================================*/
void CBoardConfig::SetApu2BasNum(u8 byBasNum)
{
	m_tApu2Cfg.m_byBasNum = min(MAXNUM_APU2_BAS, byBasNum);
}

/*====================================================================
������      : GetApu2Cfg
����        ����ȡApu2����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��TApu2Cfg
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
TApu2EqpCfg CBoardConfig::GetApu2Cfg() 
{  
	return m_tApu2Cfg;
}

/*====================================================================
������      : SetProductTest
����        �������Ƿ���������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����BOOL32 bIsProduct  �Ƿ���������
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
void CBoardConfig::SetProductTest(BOOL32 bIsProduct)
{ 
	m_tApu2Cfg.m_bIsProductTest = bIsProduct;
}

/*====================================================================
������      : IsProductTest
����        ���ж��Ƿ���������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��True or False
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
BOOL32 CBoardConfig::IsProductTest()
{
	return m_tApu2Cfg.m_bIsProductTest;
}

/*====================================================================
������      : IsSimuApu
����        ���ж��Ƿ�ģ��apu
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��True or False
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
BOOL32 CBoardConfig::IsSimuApu()
{
	return m_bIsSimuApu;
}

/*====================================================================
������      : InitBrdAgent
����        �������ʼ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��True or False
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
API BOOL InitBrdAgent()
{
	static BOOL32 bIsBrdInitSuccessFul = FALSE;//���ʼ���Ƿ�ɹ�,��ֹ��ζ԰���г�ʼ������,��һ��ʱĬ�ϲ��ɹ�
	if ( bIsBrdInitSuccessFul )
	{
		printf( "[InitBrdAgent] Already Inited Successfully,So Return!\n" );
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[InitBrdAgent] Already Inited Successfully,So Return!\n");
		return TRUE;
	}
    if ( ::BrdInit() == ERROR )
    {
        printf( "[InitBrdAgent]BrdInit() failed,so return!\n" );
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[InitBrdAgent]BrdInit() failed,so return!\n");
        return FALSE;
    }
    else
    {
		bIsBrdInitSuccessFul = TRUE;
        printf( "[InitBrdAgent]BrdInit() succeed!\n" );
		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[InitBrdAgent]BrdInit() succeed!\n");
    }
    if ( ::IsOspInitd() )
    {
		printf( "[InitBrdAgent]Osp Has Been Inited!\n" );
		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[InitBrdAgent]Osp Has Been Inited!\n");
    }
	else
	{
		if (TRUE == ::OspInit( TRUE, BRD_TELNET_PORT ))
		{
			printf("OspInit success!\n");
		}
		else
		{
			printf("OspInit failed!\n");
		}
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
			LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[IsProductTest]Read Board Ip from E2PROM error! so do'not do product test\n" );
			return FALSE;
		}
		printf( "[IsProductTest]Board Ip(network sequence) while in ProductTest is: %x\n", dwBrdIpAddr);
		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[IsProductTest]Board Ip(network sequence) while in ProductTest is: %x\n", dwBrdIpAddr);
		
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
				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, " Call BrdGetEthLinkStat(%u) ERROR!\n", byId);

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
			LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[BrdSetEthParam]Set Eth %u IP failed\n",byEthId);
		}
		else
		{
			printf("[BrdSetEthParam]Set Eth %u Ip successfully!\n", byEthId);
			LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BrdSetEthParam]Set Eth %u Ip successfully!\n", byEthId);
		}

		g_cBrdAgentApp.SetProductTest(TRUE);

		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"starting product test  ......\n");
		printf("[PRODUCTTEST]starting product test  ......\n");
		return TRUE;
	}
#else
	s32 sdwDefault = 0;
	s32 sdwReturnValue = 0;
	s8  achProfileName[64] = {0};
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");

	// �ж��Ƿ������������
	if( ::GetRegKeyInt( achProfileName, "BoardDebug", "IsAutoTest", sdwDefault, &sdwReturnValue ) 
		&& sdwReturnValue != 0 )  
	{
		g_cBrdAgentApp.SetProductTest(TRUE);
		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"starting product test  ......\n");
		printf("starting product test  ......\n");

		return TRUE;
	}
#endif

    if ( !::OspSemBCreate(&g_semApu2) )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP,"[InitBrdAgent]OspSemBCreate() failed\n" );
        printf( "[InitBrdAgent]OspSemBCreate() failed\n" );
		return FALSE;
	}
	else
	{
		printf( "[InitBrdAgent]OspSemBCreate() success\n" );
	}

    if ( !::OspSemTake( g_semApu2 ) )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[InitBrdAgent]OspSemTake(g_semApu2) error!\n" );
		printf( "[InitBrdAgent]OspSemTake(g_semApu2) error!\n" );
		return FALSE;
    }
	else
	{
		printf( "[InitBrdAgent]OspSemTake(g_semApu2) success!\n" );
	}

	// ��ȡ������Ϣ
	if( !g_cBrdAgentApp.ReadConfig() )  
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[InitBrdAgent]ReadConfig error!\n" );
		printf( "[InitBrdAgent]ReadConfig() failed.!\n" );
		return FALSE;
	}
	else
	{
		printf("ReadConfig success!\n");
	}
	// �����������͵�������Ӧ��
	g_cBrdAgentApp.CreateApp( "Apu2Agent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT);
	g_cBrdGuardApp.CreateApp( "Apu2Guard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);

    TBrdPos tBoardPosition;
    tBoardPosition = g_cBrdAgentApp.GetBrdPosition();

	// �Ƿ�ģ��APU��Ϣ
	if ( g_cBrdAgentApp.IsSimuApu() )
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[InitBrdAgent]Apu2 SIMU APU!\n" );
		printf( "[InitBrdAgent]APU2 SIMU APU!\n" );
		tBoardPosition.byBrdID = BRD_TYPE_APU;
	}

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"**************info:byBrdID=[%d]byBrdLayer=[%d]byBrdSlot=[%d]**************\n",
											tBoardPosition.byBrdID,
											tBoardPosition.byBrdLayer,
											tBoardPosition.byBrdSlot);

	printf("**************info:byBrdID=[%d]byBrdLayer=[%d]byBrdSlot=[%d]**************\n",
						tBoardPosition.byBrdID,
						tBoardPosition.byBrdLayer,
						tBoardPosition.byBrdSlot);

    g_cBrdAgentApp.SetBrdPosition( tBoardPosition );

	CBBoardConfig *pBBoardConfig = (CBBoardConfig*)&g_cBrdAgentApp;

	OspPost( MAKEIID(AID_MCU_BRDAGENT, 1), OSP_POWERON, &pBBoardConfig, sizeof(pBBoardConfig) );

	u8 byBrdId = pBBoardConfig->GetBrdPosition().byBrdID;
	OspPost( MAKEIID(AID_MCU_BRDGUARD,0,0), BOARD_GUARD_POWERON,&byBrdId,sizeof(u8));
    
	if ( !OspSemTake( g_semApu2) )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[InitBrdAgent]OspSemTake(g_semApu2) error!\n" );
    }

	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0 );
	
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0 );

	return TRUE;
}

/*====================================================================
������      : BrdGetDstMcuNode
����        ����ȡMcuA->NodeId
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��u32 dwNodeId
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
API u32 BrdGetDstMcuNode()
{
    return g_cBrdAgentApp.m_dwDstMcuNodeA;
}


/*====================================================================
������      : BrdGetDstMcuNodeB
����        ����ȡMcuB->NodeId
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��u32 dwNodeId
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
API u32 BrdGetDstMcuNodeB()
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}

/*====================================================================
������      : InstanceEntry
����        ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
void CBoardAgent::InstanceEntry( CMessage * const pcMsg )
{
	if ( NULL == pcMsg )
	{
		OspPrintf( TRUE, FALSE, "[CBoardAgent::InstanceEntry]the pointer can not be NULL!\n" );
		return;
	}
	OspPrintf(TRUE, FALSE, "[CBoardAgent::InstanceEntry] %u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event));

	switch( pcMsg->event )
	{
	case MPC_BOARD_GET_CONFIG_ACK:         
		ProcBoardGetConfigAck( pcMsg );
		break;
		
	case MPC_BOARD_GET_CONFIG_NACK:      
		ProcBoardGetConfigNAck( pcMsg );
		break;

	case MPC_BOARD_LED_STATUS_REQ:
		ProcLedStatusReq( pcMsg );
		break;

	case BOARD_LED_STATUS:
		ProcBoardLedStatus( pcMsg );
		break;

	case MCU_BOARD_CONFIGMODIFY_NOTIF:
		ProcBoardConfigModify( pcMsg );
		break;

	default:
		CBBoardAgent::InstanceEntry( pcMsg );
		break;
	}
	return;
}


/*====================================================================
������      : ProcBoardGetConfigAck
����        �������ȡ������Ӧ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
2013/03/28	2.0			������			��������������
====================================================================*/
void CBoardAgent::ProcBoardGetConfigAck( CMessage * const pcMsg )
{
    if( NULL == pcMsg )
    {
        OspPrintf( TRUE, FALSE, "[ProcBoardGetConfigAck][error] Param:(CMessage * const pcMsg) Is NULL.\n" );
        return;
    }
	TEqpMixerEntry*	ptMixerCfg;	
	TEqpMpuBasEntry* ptBasCfg;
	if (pcMsg->content == NULL)
	{
		OspPrintf( TRUE, FALSE, "[ProcBoardGetConfigAck][error] Content Of Msg Is NULL.\n" );
		return;
	}

	u16 wIndex = 1;	
	u8 byPeriCount = pcMsg->content[0];	/*����������*/
	u8 byPeriType = 0;	/*��������*/
	u8 byMixerCount = 0;
	u8 byBasCount = 0;

	for (u8 wLoop = 0; wLoop < byPeriCount; wLoop++)
	{
		byPeriType = pcMsg->content[wIndex];
		wIndex += sizeof(u8);

		switch(byPeriType)
		{
		case EQP_TYPE_MIXER:
			ptMixerCfg = (TEqpMixerEntry*)(pcMsg->content + wIndex);
			//Apu2Mixer���ñ���
			if (byMixerCount <= MAXNUM_APU2_MIXER)
			{
				g_cBrdAgentApp.SetEqpEMixerEntry(byMixerCount, *ptMixerCfg);
				byMixerCount += 1;
			}
			wIndex += sizeof(TEqpMixerEntry);
			break;
		case EQP_TYPE_BAS:
			ptBasCfg = (TEqpMpuBasEntry*)(pcMsg->content + wIndex);
			//Apu2Bas���ñ���
			if (byBasCount <= MAXNUM_APU2_BAS)
			{
				g_cBrdAgentApp.SetEqpBasEntry(byBasCount, *ptBasCfg);
				byBasCount += 1;
			}
			wIndex += sizeof(TEqpMpuBasEntry);
			break;
		default:
			OspPrintf(TRUE, FALSE, "[ProcBoardGetConfigAck][error] The %d.st PeriType.%d Is Not MIXER Or BAS.\n", wIndex, byPeriType);
			break;
		}
	}
	//ͳ��Apu2Mixer����
	g_cBrdAgentApp.SetApu2MixerNum(byMixerCount);
	//ͳ��Apu2Bas����
	g_cBrdAgentApp.SetApu2BasNum(byBasCount);

    BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON );// ��MLINK�Ƶ���
    // �ͷ��ź���
    OspSemGive( g_semApu2 );
	KillTimer( BRDAGENT_GET_CONFIG_TIMER );
	NEXTSTATE( STATE_NORMAL );
	return;
}

/*====================================================================
������      : ProcBoardGetConfigNack
����        �������ȡ���þܾ�����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
void CBoardAgent::ProcBoardGetConfigNAck( CMessage* const pcMsg )
{
	if(NULL == pcMsg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[apu2][ProcBoardGetConfigNAck] pcMsg is null!\n" );
		return;
	}
    LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[apu2][error] ProcBoardGetConfigNAck\n" );
    return;
}

void CBoardAgent::ProcLedStatusReq( CMessage* const pcMsg )
{
	if (NULL == pcMsg)
	{
		OspPrintf(TRUE, FALSE, "[ProcLedStatusReq] pcMsg is null!\n");
		return;
	}
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
	2012/02/14  1.0         �ܼ���          ����
====================================================================*/
void CBoardAgent::ProcBoardLedStatus( CMessage* const pcMsg )
{
	if (NULL == pcMsg)
	{
		OspPrintf(TRUE, FALSE, "[ProcBoardLedStatus] pcMsg is null!\n");
		return;
	}
#ifndef WIN32
	u8 abyBuf[32];
	u8 byCount;

	//ָʾ��״̬
	TBrdLedState tLedState;
	memcpy( &tLedState, pcMsg->content, sizeof(TBrdLedState) );

	if( memcmp(m_tLedState.nlunion.byLedNo, tLedState.nlunion.byLedNo, tLedState.dwLedNum) == 0 )
		return;
	
	//��������ָʾ��״̬
	memcpy(&m_tLedState, &tLedState, sizeof(m_tLedState));

	TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );
	byCount = (u8)tLedState.dwLedNum;
	memcpy(abyBuf+sizeof(TBrdPos), &byCount, sizeof(u8) );	//ָʾ������
	memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), tLedState.nlunion.byLedNo, byCount);	//ָʾ��״̬

	if (g_cBrdAgentApp.m_bPrintBrdLog)
	{
		OspPrintf(TRUE, FALSE, "CBoardAgent::ProcLedStatusReq: Led:\n");
		
		for (u32 dwIndex = 0; dwIndex < m_tLedState.dwLedNum; ++dwIndex)
		{
			OspPrintf(TRUE, FALSE," %d ", m_tLedState.nlunion.byLedNo[dwIndex]);
		}
		OspPrintf(TRUE, FALSE, "\n");
	}
					
	PostMsgToManager( BOARD_MPC_LED_STATUS_NOTIFY, abyBuf, sizeof(TBrdPos)+sizeof(u8)+byCount * sizeof(u8) );
#endif
	
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
2012/02/14  1.0         �ܼ���                 ����
=============================================================================*/
void CBoardAgent::ProcBoardConfigModify( CMessage* const pcMsg )
{
	if (NULL == pcMsg)
	{
		OspPrintf(TRUE, FALSE, "[ProcBoardConfigModify] pcMsg is null!\n");
		return;
	}
}