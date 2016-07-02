/*****************************************************************************
   ģ����      : mpu2BoardAgent
   �ļ���      : mpu2agent.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �������
   ����        : zhouyiliang
   �汾        : V4.7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/

#include "mpu2agent.h"
#include "boardguardbasic.h"

CBrdAgentApp	g_cBrdAgentApp;

SEMHANDLE       g_semMPU;

BOOL32 CBoardConfig::ReadConfig()
{
    BOOL32 bRet = CBBoardConfig::ReadConnectMcuInfo();
    if ( !bRet )
    {
		printf("[ReadConfig]CBBoardConfig::ReadConnectMcuInfo() error\n");
        return bRet;
    }
	
	u8 byState = 0;

#ifdef _LINUX12_
	u8 byEthChoice = GetBrdEthChoice();
	OspDelay(3000);
	if( ERROR == BrdGetEthLinkStat(byEthChoice, &byState) )
	{
		LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS," Call BrdGetEthLinkStat(%u) ERROR!\n", byEthChoice);
		printf(" Call BrdGetEthLinkStat(%u) ERROR!\n", byEthChoice);
		return FALSE;
	}

	//TODO ������������ʵ������ȷ��[12/15/2011 chendaiwei]
	char pszEthName[] = "ethernet";

	if( byState == 0)
	{
		BrdInterfaceShutdown(pszEthName,(byEthChoice^0x01));
		BrdInterfaceNoShutdown(pszEthName,byEthChoice);
		
		LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS, "[ReadConfig]Expected Eth choice%d is down, Need to up and reboot\n",byEthChoice );		
		printf("[ReadConfig]Expected Eth choice%d is down, Need to up and reboot\n",byEthChoice );		
		BrdHwReset();
	}
	else
	{
		BrdInterfaceShutdown(pszEthName,(byEthChoice^0x01));
	}

#endif

	s8    achProfileName[64] = {0};
    memset((void*)achProfileName, 0x0, sizeof(achProfileName));

#ifdef _WIN32  //win32 �µĲ�ۺ�ȡ�����ļ�

	sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
		
	//Layer
	s32  sdwLayer = 0;
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Layer, 0, &sdwLayer );
	if( !bRet )  
	{
		LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_Layer );
		return FALSE;
	}
	
    //Slot
    s32 sdwSlot = 0;
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Slot, 0, &sdwSlot );
	if( !bRet )  
	{
		LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_Slot );
		return FALSE;
	}
	
    //Type
    s32 sdwType = 0;
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Type, 0, &sdwType );
	if( !bRet )  
	{
		LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_Type );
		return FALSE;
	}
	
	TBrdPos tBrdPos;
	tBrdPos.byBrdID = (u8)sdwType;
	tBrdPos.byBrdLayer = (u8)sdwLayer;   	
	tBrdPos.byBrdSlot = (u8)sdwSlot;
	g_cBrdAgentApp.SetBrdPosition(tBrdPos);
	
	//BrdIp
	u32 dwBrdIpAddr = 0;
	
	s8 achBrdIpAddr[64] = {0};
	
	bRet = GetRegKeyString( achProfileName, SECTION_BoardConfig, KEY_BoardIpAddr, "0.0.0.0", achBrdIpAddr, 64 );
	
	if( !bRet )  
	{
		LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_BoardIpAddr );
		return FALSE;
	}
	
	dwBrdIpAddr = ntohl(inet_addr(achBrdIpAddr));
	
	g_cBrdAgentApp.SetBrdIpAddr(dwBrdIpAddr);
    
	
	s32 sdwPID = 0;
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_BrdType, 0, &sdwPID );
	if( !bRet )  
	{
		LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_BrdType );
		return FALSE;
	}
	//����brdtype
    TBrdPos tBdPos = g_cBrdAgentApp.GetBrdPosition();
	tBdPos.byBrdID = (u8)sdwPID;
	g_cBrdAgentApp.SetBrdPosition(tBdPos);


#endif


	
	//�������ļ��Ƿ�ģ��mpu
    s32 nSimuMpu = 0;
	memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);
	BOOL32 bResult = GetRegKeyInt( achProfileName, SECTION_BoardSystem, KEY_IsSimuMpu,
		0, &nSimuMpu );
	
	if ( !bResult )
	{
		printf("Wrong profile while reading %s%s!\n", SECTION_BoardSystem, KEY_IsSimuMpu );	
		LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,"Wrong profile while reading %s%s!\n", SECTION_BoardSystem, KEY_IsSimuMpu );
	}


	if ( 1 == nSimuMpu ) 
	{
		TBrdPos tSimMpuBrdPos = g_cBrdAgentApp.GetBrdPosition();
		tSimMpuBrdPos.byBrdID = BRD_TYPE_MPU;
		g_cBrdAgentApp.SetBrdPosition(tSimMpuBrdPos);
	}
    return TRUE;
}




API BOOL InitBrdAgent()
{
    BOOL bResult;

	static u16 wFlag = 0;
	if ( wFlag != 0 )
	{
		LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "[mpu2] Already Inited!\n" );
		return TRUE;
	}

    LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS, "[mpu2] mpu BrdInit() here!\n" );
    if ( BrdInit() )
    {
        LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,  "[mpu2][error] mpu BrdInit() failed!\n" );
        return FALSE;
    }
    else
    {
        LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS,"[mpu2] mpu BrdInit() succeed!\n" );
    }
    
    ::OspInit( TRUE, BRD_TELNET_PORT );
    
    //Osp telnet ��ʼ��Ȩ [11/28/2006-zbq]
#ifndef WIN32
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
#endif

	wFlag++;

    if ( !OspSemBCreate(&g_semMPU) )
	{
         LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "[mpu][error] OspSemBCreate failed\n" );
		return FALSE;
	}

	bResult = OspSemTake( g_semMPU );
    if ( bResult == FALSE )
    {
         LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,  "[mpu][error] OspSemTake(g_semMMP) error!\n" );
    }
	
	BOOL32 bAutoTest = FALSE;
#ifdef _LINUX_
	bAutoTest = BrdGetE2promTestFlag();
#else
	s8    achProfileName[64] = {0};
	memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	
	s32  dwTest = 0;
	BOOL32 bRet = GetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_IsAutoTest, 0, &dwTest );
	if( !bRet )  
	{
		 LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_IsAutoTest );
	}
	bAutoTest = (dwTest != 0) ? TRUE: FALSE;
#endif
	printf("bAutoTest is: %d\n", bAutoTest);
	LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS, "bAutoTest is: %d\n", bAutoTest);
	
	g_cBrdAgentApp.SetIsAutoTest( bAutoTest );
	
	if( bAutoTest )		//����������·��
	{
		u32 dwBrdIpAddr = 0;
#ifdef _LINUX_
		BrdGetE2promIpAddr(&dwBrdIpAddr);//�����ڼ�IP��E2PROM�ж�ȡ
		
		if(dwBrdIpAddr == 0)
		{
			 LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,"Read Board Ip from E2PROM error!\n" );
			return FALSE;
		}
		
		LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS, "Board Ip(network sequence) while in AutoTest is: %x\n", dwBrdIpAddr);
		
		//zhouyiliang 20121112 ����������0����Ϊ׼����������1����	
	
		//����ip
		//�ж���E0 or E1. (���ڶ����Ļ�����0��Ϊ׼)
//		u8 byId = 0;
// 		u8 byState = 0;
// 		u8 byEthId = ~0;
// 		for(u8 byId = 0; byId < 2; byId ++)
// 		{
// 			if( ERROR == BrdGetEthLinkStat(byId, &byState) )
// 			{
// 				 LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS," Call BrdGetEthLinkStat(%u) ERROR!\n", byId);
// 				byEthId = 0;
// 				break;
// 			}
// 			printf("BrdGetEthLinkStat(%d),byState=%d\n",byId,byState);
// 			if( byState == 1 && byEthId == (u8)~0)
// 			{
// 				
// 				byEthId = byId;
// 				printf("use eth:%d\n",byEthId);
// 			}
// 			BrdDelEthParam(byId);	//��ȫ������Է���ip�費��
// 		}
		
        TBrdEthParam tEtherPara;
        tEtherPara.dwIpAdrs = dwBrdIpAddr;
        tEtherPara.dwIpMask = inet_addr("255.0.0.0");
        memset( tEtherPara.byMacAdrs, 0, sizeof(tEtherPara.byMacAdrs) );
		//zhouyiliang 20121112 ����������0����Ϊ׼
        if( ERROR == BrdSetEthParam( 0, Brd_SET_IP_AND_MASK, &tEtherPara ) )
        {
             LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,"Set Eth0 IP failed\n");
			 printf("Error:Set Eth0 IP failed\n");
            //return FALSE;
        }
		else
		{
			 LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS,"Set Eth0 Ip successfully!\n");
			 printf("Set Eth0 Ip successfully!\n");
		}
#else
		s8    achDefStr[] = "10.1.1.2";
		s8    achReturn[MAX_VALUE_LEN + 1] ;
		memset(achReturn, '\0', sizeof(achReturn));
		bResult = GetRegKeyString( achProfileName, SECTION_BoardDebug, KEY_BoardIpAddr,
			achDefStr, achReturn, MAX_VALUE_LEN + 1 );
		if ( !bResult )
		{
			 LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,"Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_BoardIpAddr );
			memcpy(achReturn, achDefStr, sizeof(achDefStr)) ;
		}
		dwBrdIpAddr = ntohl(INET_ADDR(achReturn));
#endif
		g_cBrdAgentApp.SetBrdIpAddr(dwBrdIpAddr);
		
		 LogPrint(LOG_LVL_DETAIL,MID_PUB_ALWAYS,"return true dircectly here due to the purpose of autotest!!!!!\n");
		return TRUE;
	}

	//�õ�������Ϣ
    bResult = g_cBrdAgentApp.ReadConfig();
    if( bResult != TRUE )
    {
         LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "[mpu][error] ReadConfig failed.\n" );
		 printf("[mpu][error] ReadConfig failed.\n" );
        return FALSE;
    }

	printf("[mpu2]after ReadConfig begin createapp .\n" );
	//�����������Ӧ��
	g_cBrdAgentApp.CreateApp( "MpuAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );
	//������������Ӧ��
	g_cBrdGuardApp.CreateApp( "MpuGuard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);
 
	printf("[mpu2]after ReadConfig  createapp succ. \n" );
	//�������ʼ����
	CBBoardConfig *pBBoardConfig = (CBBoardConfig*)&g_cBrdAgentApp;
	OspPost( MAKEIID(AID_MCU_BRDAGENT, 1), OSP_POWERON, &pBBoardConfig, sizeof(pBBoardConfig) );
	// ��������ģ�鿪ʼ����
	// �����������浥��������Ϣ��������ݲ�ͬ���������в�ͬ�澯 [12/1/2011 chendaiwei]
	u8 byBrdId = pBBoardConfig->GetBrdPosition().byBrdID;
	OspPost( MAKEIID(AID_MCU_BRDGUARD,0,0), BOARD_GUARD_POWERON,&byBrdId,sizeof(u8));
	//wait here
	bResult = OspSemTake( g_semMPU);
	printf("[mpu2]after ReadConfig take sem g_semMPU succ\n" );
    if ( bResult == FALSE )
    {
		printf("[mpu2][error] BrdAgent: OspSemTake(g_semMMP) error!\n" );
    
         LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "[mpu2][error] BrdAgent: OspSemTake(g_semMMP) error!\n" );
    }
	
	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0 );	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0 );
	printf("[mpu2 InitBrdAgent] over init success!\n" );
	return TRUE;
}

/*lint -save -e714*/
API u32 BrdGetDstMcuNode()
{
    return g_cBrdAgentApp.m_dwDstMcuNodeA;
}

API u32 BrdGetDstMcuNodeB()
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
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
void CBoardAgent::InstanceEntry( CMessage * const pcMsg )
{
	if ( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,  "[Mpu][error] the pointer can not be NULL!\n" );
		return;
	}
	LogPrint(LOG_LVL_DETAIL,MID_PUB_ALWAYS, "[mpu] %u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event) );
	switch( pcMsg->event )
	{

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
		// ProcBoardConfigModify( pcMsg );
		break;


	default:
		CBBoardAgent::InstanceEntry( pcMsg );
		break;
	}
	
	return;
}

/*====================================================================
    ������      ��ProcBoardGetConfigAck
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
void CBoardAgent::ProcBoardGetConfigAck( CMessage * const pcMsg )
{
    if( NULL == pcMsg )
    {
         LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "[mpu][error] ProcBoardGetConfigAck pointer is Null\n" );
        return;
    }
    TMpuEBapEVpuConfigAck tMpuEBapEVpuConfigAck;
    memcpy( &tMpuEBapEVpuConfigAck, pcMsg->content, sizeof(TMpuEBapEVpuConfigAck) );

//	if ( 0 != g_cBrdAgentApp.GetEqpId() && 
//		 (g_cBrdAgentApp.GetEqpId() != tMpuEBapEVpuConfigAck.GetEqpId()) )
//	{
//		BrdHwReset();
//	}

	 LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS,"[ProcBoardGetConfigAck] Mpu2's WorkMode is %d!\n ", tMpuEBapEVpuConfigAck.GetEqpType());

    // ����EqpType��Workmode����EqpId
	u8 byEqpNum = 0;
	u8 byEqpType = 0;
	if ( TYPE_MPUSVMP == tMpuEBapEVpuConfigAck.GetEqpType() || TYPE_MPU2VMP_BASIC == tMpuEBapEVpuConfigAck.GetEqpType()
		|| TYPE_MPU2VMP_ENHANCED == tMpuEBapEVpuConfigAck.GetEqpType() )
    {
         byEqpType = EQP_TYPE_VMP;
     }
     else if ( TYPE_EVPU == tMpuEBapEVpuConfigAck.GetEqpType() )
     {
         byEqpType = EQP_TYPE_VMP;
     }
     else if ( TYPE_MPUBAS == tMpuEBapEVpuConfigAck.GetEqpType() 
 				|| TYPE_MPUBAS_H == tMpuEBapEVpuConfigAck.GetEqpType()
				|| TYPE_MPU2BAS_ENHANCED == tMpuEBapEVpuConfigAck.GetEqpType() 
 				|| TYPE_MPU2BAS_BASIC == tMpuEBapEVpuConfigAck.GetEqpType()
			)
     {
         byEqpType = EQP_TYPE_BAS;
     }
     else if ( TYPE_EBAP == tMpuEBapEVpuConfigAck.GetEqpType() )
     {
         byEqpType = EQP_TYPE_BAS;
     }
     else
     {
          LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "[mpu][error] tMpuEBapEVpuConfigAck.GetEqpType() wrong!\n" );
     }
	if (tMpuEBapEVpuConfigAck.GetEqpId() != 0 ) 
	{
		TEqpBasicCfg tCfg;
		tCfg.byEqpId =tMpuEBapEVpuConfigAck.GetEqpId();
		tCfg.dwConnectIP = g_cBrdAgentApp.GetMpcIpA();
		tCfg.dwConnectIpB = g_cBrdAgentApp.GetMpcIpB();
		tCfg.wConnectPort = g_cBrdAgentApp.GetMpcPortA();
		tCfg.wConnectPortB = g_cBrdAgentApp.GetMpcPortB();
		tCfg.wMcuId = LOCAL_MCUID;
		tCfg.dwLocalIP = g_cBrdAgentApp.GetBrdIpAddr();
		tCfg.wRcvStartPort =  tMpuEBapEVpuConfigAck.GetEqpRcvStartPort();
		tCfg.wRcvStartPortB = tMpuEBapEVpuConfigAck.GetEqpRcvStartPort();
		
		tCfg.byEqpType = byEqpType;
		g_cBrdAgentApp.SetMpu2EqpCfg(0,tCfg);
		byEqpNum++;
	}
	if(tMpuEBapEVpuConfigAck.GetEqpId2() != 0)
	{
		TEqpBasicCfg tCfg;
		tCfg.byEqpId =tMpuEBapEVpuConfigAck.GetEqpId2();
		tCfg.dwConnectIP = g_cBrdAgentApp.GetMpcIpA();
		tCfg.dwConnectIpB = g_cBrdAgentApp.GetMpcIpB();
		tCfg.wConnectPort = g_cBrdAgentApp.GetMpcPortA();
		tCfg.wConnectPortB = g_cBrdAgentApp.GetMpcPortB();
		tCfg.wMcuId = LOCAL_MCUID;
		tCfg.dwLocalIP = g_cBrdAgentApp.GetBrdIpAddr();
		tCfg.wRcvStartPort =  tMpuEBapEVpuConfigAck.GetEqpRcvStartPort2();
		tCfg.wRcvStartPortB = tMpuEBapEVpuConfigAck.GetEqpRcvStartPort2();
		tCfg.byEqpType = byEqpType;
		g_cBrdAgentApp.SetMpu2EqpCfg(1,tCfg);
		byEqpNum++;
	}
	if ( 0 != byEqpNum) 
	{
		g_cBrdAgentApp.SetMpu2EqpNum(byEqpNum);
		g_cBrdAgentApp.SetMpu2WorkMode(tMpuEBapEVpuConfigAck.GetEqpType());
	}
	
	LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS,"[ProcBoardGetConfigAck] Mpu2's eqpNum: %d!\n ", byEqpNum);

#ifndef _WIN32
	BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON );// ��MLINK�Ƶ���
#endif
    // �ͷ��ź���
    OspSemGive( g_semMPU );

	KillTimer( BRDAGENT_GET_CONFIG_TIMER );
	NEXTSTATE( STATE_NORMAL );
	/*
	u8 abyBuf[32];
	//TODO ����汾�Ŵ�ͳһ���壬�Ƿ��ú궨��[12/13/2011 chendaiwei]
	u32 dwSoftWareVersion = 1212;
	TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );
	memcpy(abyBuf+sizeof(TBrdPos),&dwSoftWareVersion,sizeof(u32));

	//֪ͨMPC�����������汾����Ϣ [12/13/2011 chendaiwei]
	PostMsgToManager( BOARD_MPC_SOFTWARE_VERSION_NOTIFY, abyBuf, 
		sizeof(u32)+sizeof(TBrdPos) );
	*/

	return;
}

/*lint -save -e715*/
/*====================================================================
    ������      ��ProcBoardGetConfigNAck
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
void CBoardAgent::ProcBoardGetConfigNAck( CMessage* const pcMsg )
{
     LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "[mpu][error] ProcBoardGetConfigNAck\n" );
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

		if (g_cBrdAgentApp.m_bPrintBrdLog)
		{
			LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS, "CBoardAgent::ProcLedStatusReq: Led:\n");
			
			for (u32 dwIndex = 0; dwIndex < m_tLedState.dwLedNum; ++dwIndex)
			{
				 LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS," %d ", m_tLedState.nlunion.byLedNo[dwIndex]);
			}
			LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS,"\n");
		}	
		PostMsgToManager( BOARD_MPC_LED_STATUS_ACK, abyBuf, 
			              sizeof(tBrdPos)+sizeof(u8)+ byCount * sizeof(u8) );
		break;
		
	default:
		 LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
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
    10/05/13    4.6         lukunpeng       �޸�
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
					
	PostMsgToManager( BOARD_MPC_LED_STATUS_NOTIFY, abyBuf, 
		sizeof(TBrdPos)+sizeof(u8)+byCount * sizeof(u8) );
#endif
	
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
    11/12/14    4.7         �´�ΰ        ����
====================================================================*/
/*void CBoardAgent::ProcGetVersionReq( CMessage* const pcMsg )
{
	s8 abyBuf[MAX_SOFT_VER_LEN+1+sizeof(TBrdPos)] = {0};


	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:		
		{	
			//ȡ�汾��Ϣ
			memset(abyBuf, 0, sizeof(abyBuf) );

			TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
			memcpy(abyBuf, &tBrdPos,sizeof(tBrdPos) );
			
			//TODO ����汾�Ŵ�ͳһ���壬�Ƿ��ú궨��[12/13/2011 chendaiwei]
			sprintf(abyBuf+sizeof(tBrdPos),"softwareversin:mpu24.7");

			//���͸�MANAGER��Ӧ
			PostMsgToManager( BOARD_MPC_GET_VERSION_ACK,(u8*)abyBuf, sizeof(abyBuf) );
		}
		break;
		
	default:
		LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}*/

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

}
/*lint -restore*/