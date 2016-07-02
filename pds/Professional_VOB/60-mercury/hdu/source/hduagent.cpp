/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : hduagent.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �������
   ����        :  ���ֱ�
   �汾        : V4.6  Copyright(C) 2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
  2008/12/19   4.6         ���ֱ�        ����
******************************************************************************/

#include "hduagent.h"
// #ifdef _LINUX_
// #include "brdwrapper.h"
// #endif

CHduBrdAgentApp	g_cBrdAgentApp;	//�������Ӧ��ʵ��

static SEMHANDLE       g_semHdu;

void CHduBoardAgent::InstanceEntry(CMessage * const pcMsg)
{
	if ( NULL == pcMsg )
	{
		OspPrintf( TRUE, FALSE, "[Hdu] the pointer can not be NULL!\n" );
		return;
	}
	
	OspPrintf( TRUE, FALSE, " %u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event) );

	switch (pcMsg->event)
	{
		// [6/2/2010 xliang] comment the followinf cases, use default.
//	    case OSP_POWERON:                   //��������
//         case BRDAGENT_CONNECT_MANAGERA_TIMER:       // ���ӹ������ʱ
// 		case BRDAGENT_CONNECT_MANAGERB_TIMER:       // ���ӹ������ʱ
// 		case BRDAGENT_REGISTERA_TIMER:		        // ע��ʱ�䳬ʱ
// 		case BRDAGENT_REGISTERB_TIMER:		        // ע��ʱ�䳬ʱ
// 		case MPC_BOARD_REG_ACK:                     //ע��Ӧ����Ϣ
// 		case MPC_BOARD_REG_NACK:			        //ע���Ӧ����Ϣ
// 		case BRDAGENT_GET_CONFIG_TIMER:             //�ȴ�����Ӧ����Ϣ��ʱ
// 		case MPC_BOARD_ALARM_SYNC_REQ:              //�������ĸ澯ͬ������
// 		case MPC_BOARD_GET_VERSION_REQ:             //��ȡ����İ汾��Ϣ
// 		case MPC_BOARD_GET_MODULE_REQ:              //��ȡ�����ģ����Ϣ
// 		case MPC_BOARD_TIME_SYNC_CMD:               //����ʱ��ͬ������
// 		case MPC_BOARD_SELF_TEST_CMD:               //�����Բ�����
// 		case MPC_BOARD_BIT_ERROR_TEST_CMD:          //���������������
// 		case MPC_BOARD_GET_STATISTICS_REQ:          //��ȡ�����ͳ����Ϣ
// 		case MPC_BOARD_UPDATE_SOFTWARE_CMD:			//������������
// 		// �����Ǹ澯����������Ϣ
// 	    case SVC_AGT_MEMORY_STATUS:                 //�ڴ�״̬�ı�
// 		case SVC_AGT_FILESYSTEM_STATUS:             //�ļ�ϵͳ״̬�ı�
// 		// �����ǲ�������Ϣ
// 	    case BOARD_MPC_CFG_TEST:
// 		case BOARD_MPC_GET_ALARM:
// 		case BOARD_MPC_MANAGERCMD_TEST:
// 		case MPC_BOARD_RESET_CMD:                   //������������
// 		case OSP_DISCONNECT:
// 		    CBBoardAgent::InstanceEntry(pcMsg);
// 			if ( pcMsg->event == OSP_POWERON )
// 			{
// 				g_cBrdAgentApp.SetHduMode( HDU_OUTPUTMODE_BOTH );
// 			}
// 		    break;
		
	    case MPC_BOARD_GET_CONFIG_ACK:       //ȡ������ϢӦ����Ϣ
		    ProcBoardGetConfigAck( pcMsg );
		    break;
		case MPC_BOARD_GET_CONFIG_NACK:      //ȡ������Ϣ��Ӧ��
		    ProcBoardGetConfigNAck( pcMsg );
		    break;

		default:
			CBBoardAgent::InstanceEntry(pcMsg); // [6/2/2010 xliang] use base class
			if ( pcMsg->event == OSP_POWERON )
			{
				g_cBrdAgentApp.SetHduMode( HDU_OUTPUTMODE_BOTH );
 			}
			break;
			//OspPrintf(TRUE, FALSE, "[CHduBoardAgent]InstanceEntry  have no this message! pcMsg-->event = %d\n", pcMsg->event);

	}
}

void  CHduBoardAgent::ProcBoardGetConfigAck( CMessage* const pcMsg )  //ȡ������ϢӦ����Ϣ
{
	if( NULL == pcMsg )
    {
        OspPrintf( TRUE, FALSE, "ProcBoardGetConfigAck pointer is Null\n" );
        return;
    }
    
// 	u8 *pbyHduNum;
// 	pbyHduNum = (u8*)pcMsg->content;
	TEqpHduEntry *ptHduEntry = (TEqpHduEntry*)(pcMsg->content + sizeof(u8));
	if ( (0 != g_cBrdAgentApp.GetEqpId()) &&
		(g_cBrdAgentApp.GetEqpId() != ptHduEntry->GetEqpId()) )
	{
		BrdHwReset();
	}
    g_cBrdAgentApp.SetEqpId( ptHduEntry->GetEqpId() );
	g_cBrdAgentApp.SetStartMode(ptHduEntry->GetStartMode());
	printf("[CHduBoardAgent::ProcBoardGetConfigAck]EqpId = %d , StartMode = %d\n", 
														ptHduEntry->GetEqpId(),
														ptHduEntry->GetStartMode());
    
    BOOL32 bResult = OspSemGive(g_semHdu);
	if (!bResult)
	{
		OspPrintf(TRUE, FALSE, "[CHduBoardAgent] OspSemGive fail! in ProcBoardGetConfigAck()\n");
	}
	
	KillTimer( BRDAGENT_GET_CONFIG_TIMER );
	NEXTSTATE( STATE_NORMAL );
}

void  CHduBoardAgent::ProcBoardGetConfigNAck( CMessage* const pcMsg )    //ȡ������Ϣ��Ӧ��
{
	if (NULL == pcMsg)
	{
		OspPrintf(TRUE, FALSE, "[CHduBoardAgent:ProcBoardGetConfigNAck]pcMsg is Null\n");
		return;
	}
	OspPrintf(TRUE, FALSE, "[CHduBoardAgent]The function ProcBoardGetConfigNAck() is Null\n");
}


BOOL32 CHduBoardConfig::ReadConfig()
{
    BOOL32 bRet = CBBoardConfig::ReadConnectMcuInfo();
    if (!bRet)
    {
		OspPrintf(TRUE, FALSE, "[CHduBoardConfig]ReadConnectMcuInfo fail!\n");
        return bRet;
    }
	
	//���ȡ�����ļ�
	s8    achProfileName[64] = {0};
	memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	//Layer
	s32  sdwLayer = 0;
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Layer, 0, &sdwLayer );
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_Layer );
		return FALSE;
	}
	TBrdPos tPos = g_cBrdAgentApp.GetBrdPosition();
	tPos.byBrdLayer = (u8)sdwLayer;   
	g_cBrdAgentApp.SetBrdPosition(tPos);
	
#ifdef _WIN32  //win32 �µĲ�ۺ�ȡ�����ļ�
	//IsAutoTest
	/*	s32  sdwTest = 0;
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_IsAutoTest, 0, &sdwTest );
	if( !bRet )  
	{
	printf( "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_IsAutoTest );
	return FALSE;
	}
	g_cBrdAgentApp.SetIsTest( (BOOL)sdwTest );
	*/	
	
    //Slot
    s32 sdwSlot = 0;
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Slot, 0, &sdwSlot );
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_Slot );
		return FALSE;
	}
	
    //Type
    s32 sdwType = 0;
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Type, 0, &sdwType );
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_Type );
		return FALSE;
	}
	
	TBrdPos tBrdPos;
	tBrdPos.byBrdID = (u8)sdwType;
	tBrdPos.byBrdLayer = (u8)sdwLayer;   	
	tBrdPos.byBrdSlot = (u8)sdwSlot;
	g_cBrdAgentApp.SetBrdPosition(tBrdPos);
	
	//BrdIp
	u32 dwBrdIpAddr = 0;
	
	s8 achBrdIpAddr[MAXLEN_IPADDR_BRD_HDU] = {0};
	
	bRet = GetRegKeyString( achProfileName, SECTION_BoardConfig, KEY_BoardIpAddr, "0.0.0.0", achBrdIpAddr, MAXLEN_IPADDR_BRD_HDU );
	
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_BoardIpAddr );
		return FALSE;
	}
	
	dwBrdIpAddr = ntohl(inet_addr(achBrdIpAddr));
	
	g_cBrdAgentApp.SetBrdIpAddr(dwBrdIpAddr);
    
	
	s32 sdwPID = 0;
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_BrdType, 0, &sdwPID );
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_BrdType );
		return FALSE;
	}
	//����brdtype
    TBrdPos tBdPos = g_cBrdAgentApp.GetBrdPosition();
	tBdPos.byBrdID = (u8)sdwPID;
	g_cBrdAgentApp.SetBrdPosition(tBdPos);
#endif
	
    return TRUE;
}

static void AgentAPIEnableInLinux()
{
#ifdef _LINUX_
    
#endif
}

API BOOL InitBrdAgent(void)
{
    BOOL bResult;

	static u16 wFlag = 0;
	if( wFlag != 0 )
	{
		printf("[BrdAgent] Already Inited!\n" );
		return TRUE;
	}
	
   	wFlag++;

#ifndef _VXWORKS_
    if( ERROR == BrdInit() )
    {
        printf("[BrdAgent] BrdInit fail.\n");
        return FALSE;
    }
#endif

    ::OspInit(TRUE, BRD_TELNET_PORT);
    
    //Osp telnet ��ʼ��Ȩ [11/28/2006-zbq]
#ifndef WIN32
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
#endif
/*lint -save -e750*/
#undef OSPEVENT
#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
    
#ifdef _EV_AGTSVC_H_
#undef _EV_AGTSVC_H_
//#include "evagtsvc.h"
#define _EV_AGTSVC_H_
#else
#include "evagtsvc.h"
#undef _EV_AGTSVC_H_
#endif    


	// ����API
	//lint -save -e522
	AgentAPIEnableInLinux();
	//lint -restore

    if ( !OspSemBCreate(&g_semHdu) )
	{
        printf( "[InitHduAgent] OspSemBCreate failed\n" );
		return FALSE;
	}
	else
	{
		printf("[InitHduAgent] OspSemBCreate sucessful!\n");
	}

	bResult = OspSemTake( g_semHdu );
    if ( !bResult )
    {
		printf("BrdAgent: OspSemTake(g_semHdu) error!");
    }


	BOOL32 bAutoTest = FALSE;
#ifdef _LINUX_
	bAutoTest = BrdGetE2promTestFlag();
#endif
	printf("bAutoTest is: %d\n", bAutoTest);
	g_cBrdAgentApp.SetIsTest( bAutoTest );

	if(g_cBrdAgentApp.GetIsTest())		//����������·��
	{
		u32 dwBrdIpAddr = 0;
#ifdef _LINUX_
		BrdGetE2promIpAddr(&dwBrdIpAddr);//�����ڼ�IP��E2PROM�ж�ȡ
		
		if(dwBrdIpAddr == 0)
		{
			printf( "Read Board Ip from E2PROM error!\n" );
			return FALSE;
		}
		
		printf( "Board Ip(network sequence) while in AutoTest is: %x\n", dwBrdIpAddr);
		
		//����ip
        TBrdEthParam tEtherPara;
        tEtherPara.dwIpAdrs = dwBrdIpAddr;
        tEtherPara.dwIpMask = inet_addr("255.0.0.0");
        memset( tEtherPara.byMacAdrs, 0, sizeof(tEtherPara.byMacAdrs) );
        if( ERROR == BrdSetEthParam( 0, Brd_SET_IP_AND_MASK, &tEtherPara ) )
        {
            printf("Set Eth 0 IP failed\n");
            //return FALSE;
        }
		
#endif
		g_cBrdAgentApp.SetBrdIpAddr(dwBrdIpAddr);

		printf("return true dircectly here due to the purpose of autotest!!!!!\n");
		return TRUE;
	}

	//�õ�������Ϣ
    bResult = g_cBrdAgentApp.ReadConfig();
    if( !bResult )
    {
        printf("[BrdAgent] ReadConfig failed.\n");
        return FALSE;
    }
	else
	{
		printf("[BrdAgent] ReadConfig succeed.\n");
	}

	//�����������Ӧ��
	g_cBrdAgentApp.CreateApp( "HduAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );

	//�������ʼ����
	CBBoardConfig *pBBoardConfig = (CBBoardConfig*)&g_cBrdAgentApp;
	OspPost( MAKEIID(AID_MCU_BRDAGENT, 1), OSP_POWERON, &pBBoardConfig, sizeof(pBBoardConfig) );

	//wait here
	bResult = OspSemTake( g_semHdu);
    if ( !bResult )
    {
		printf("BrdAgent: OspSemTake(g_semHdu) error!\n");
        OspPrintf( TRUE, FALSE, "BrdAgent: OspSemTake(g_semHdu) error!\n" );
    }
	
	printf("[InitBrdAgent] going to the end of the init!!!!!\n");
	
	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0);


	return TRUE;
}


u32 BrdGetDstMcuNode(void)
{
    return g_cBrdAgentApp.m_dwDstMcuNodeA;
}

u32 BrdGetDstMcuNodeB(void)
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}

u8 BrdGetHduModel(void)
{
    return g_cBrdAgentApp.GetHduMode();
}
