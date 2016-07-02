/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : mmpagent.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �������ʵ�֣���ɸ澯�������MANAGER�Ľ���
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/25  1.0         jianghy       ����
   2004/11/11  3.5         �� ��         �½ӿڵ��޸�
   2006/04/18  4.0         john         ���������������
******************************************************************************/
#include "osp.h"
#include "evagtsvc.h"
#include "mcuconst.h"
#include "mmpagent.h"
#include "mcuagtstruct.h"
#include "mcustruct.h"


#ifdef _VXWORKS_
#include "brddrvLib.h"
#include "timers.h"
#include <dirent.h>
#endif

// MPC2 ֧��
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

extern SEMHANDLE g_semMMP;
BOOL32 g_bUpdateSoftBoard = TRUE;   // �����������
CBrdAgentApp	g_cBrdAgentApp;	//�������Ӧ��ʵ��

s32 g_nLogLevel = 0;
/*=============================================================================
  �� �� ���� brdagtlog
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� char* fmt
             ...
  �� �� ֵ�� void 
=============================================================================*/
void brdagtlog(char* fmt, ...)
{
    char PrintBuf[255];
    int BufLen = 0;
    va_list argptr;
	if( 1 == g_nLogLevel)
	{		  
		BufLen = sprintf(PrintBuf,"[Mmp]:"); 
		va_start(argptr, fmt);
		BufLen += vsprintf(PrintBuf+BufLen, fmt, argptr); 
		va_end(argptr); 
		BufLen += sprintf(PrintBuf+BufLen, "\n"); 
		OspPrintf(1, 0, PrintBuf); 
	}
    return;
}

// ���캯��
CBoardAgent::CBoardAgent()
{
	memset(&m_tBoardPosition, 0, sizeof(m_tBoardPosition) );

    FreeDataA();
    FreeDataB();

	m_bIsTest = FALSE;
	memset( m_atBoardAlarmTable, 0, sizeof(m_atBoardAlarmTable) );
	m_dwBoardAlarmTableMaxNo = 0;

	//������
	m_wLastEvent = 0;
	memset( m_abyLastEventPara, 0, sizeof(m_abyLastEventPara) );
    m_byConnectNum = 0;
	return;
}

//��������
CBoardAgent::~CBoardAgent()
{
	return;
}

/*=============================================================================
  �� �� ���� FreeDataA
  ��    �ܣ� ���A���������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::FreeDataA(void)
{
	m_dwDstNode = INVALID_NODE;
	m_dwDstIId = INVALID_INS;

    return;
}

/*=============================================================================
  �� �� ���� FreeDataB
  ��    �ܣ� ���B���������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::FreeDataB(void)
{
    m_dwDstNodeB = INVALID_NODE;
	m_dwDstIIdB = INVALID_INS;

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
#ifndef WIN32
#ifdef MMP
	BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF );
#endif
#endif
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
void CBoardAgent::InstanceEntry(CMessage * const pcMsg)
{
	if( NULL == pcMsg )
	{
		OspPrintf(TRUE, FALSE, "[Mmp] the pointer can not be NULL!\n");
		return;
	}

	brdagtlog(" %u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event));

	switch( pcMsg->event )
	{
	case BOARD_AGENT_POWERON:            //��������
        ProcBoardPowerOn( pcMsg ) ;
		break;

	case BRDAGENT_CONNECT_MANAGERA_TIMER:          //���ӹ������ʱ
		ProcBoardConnectManagerTimeOut( TRUE );
		break;

    case BRDAGENT_CONNECT_MANAGERB_TIMER:          //���ӹ������ʱ
		ProcBoardConnectManagerTimeOut( FALSE );
		break;

	case BRDAGENT_REGISTERA_TIMER:		         //ע��ʱ�䳬ʱ
		ProcBoardRegisterTimeOut( TRUE );
		break;

    case BRDAGENT_REGISTERB_TIMER:		         //ע��ʱ�䳬ʱ
		ProcBoardRegisterTimeOut( FALSE );
		break;

	case MPC_BOARD_REG_ACK:              //ע��Ӧ����Ϣ
		ProcBoardRegAck( pcMsg );
		break;
		
	case MPC_BOARD_REG_NACK:			 //ע���Ӧ����Ϣ
		ProcBoardRegNAck( pcMsg );
		break;

	case MPC_BOARD_GET_CONFIG_ACK:       //ȡ������ϢӦ����Ϣ
		ProcBoardGetConfigAck( pcMsg );
		break;
		
	case MPC_BOARD_GET_CONFIG_NACK:      //ȡ������Ϣ��Ӧ��
		ProcBoardGetConfigNAck( pcMsg );
		break;

	case BRDAGENT_GET_CONFIG_TIMER:               //�ȴ�����Ӧ����Ϣ��ʱ
		ProcGetConfigTimeOut( pcMsg );
		break;

	case MPC_BOARD_ALARM_SYNC_REQ:       //�������ĸ澯ͬ������
		ProcAlarmSyncReq( pcMsg );
		break;

	case MPC_BOARD_LED_STATUS_REQ:
		ProcLedStatusReq( pcMsg );
		break;

	case MPC_BOARD_BIT_ERROR_TEST_CMD:   //���������������
		ProcBitErrorTestCmd( pcMsg );
		break;

	case MPC_BOARD_TIME_SYNC_CMD:        //����ʱ��ͬ������
		ProcTimeSyncCmd( pcMsg );
		break;

	case MPC_BOARD_SELF_TEST_CMD:        //�����Բ�����
		ProcBoardSelfTestCmd( pcMsg );
		break;

	case MPC_BOARD_RESET_CMD:            //��������������
		ProcBoardResetCmd( pcMsg );
		break;

	case MPC_BOARD_UPDATE_SOFTWARE_CMD:  //�����������
		ProcUpdateSoftwareCmd( pcMsg );
		break;

	case MPC_BOARD_GET_STATISTICS_REQ:   //��ȡ�����ͳ����Ϣ
		ProcGetStatisticsReq( pcMsg );
		break;

	case MPC_BOARD_GET_VERSION_REQ:      //��ȡ����İ汾��Ϣ
		ProcGetVersionReq( pcMsg );
		break;

	case MPC_BOARD_GET_MODULE_REQ:      //��ȡ�����ģ����Ϣ
		ProcGetModuleInfoReq( pcMsg );
		break;

	case OSP_DISCONNECT:
		ProcOspDisconnect( pcMsg );
		break;

	//�����Ǹ澯����������Ϣ
	case SVC_AGT_MEMORY_STATUS:          //�ڴ�״̬�ı�
		ProcBoardMemeryStatus( pcMsg );
		break;

	case SVC_AGT_FILESYSTEM_STATUS:      //�ļ�ϵͳ״̬�ı�
		ProcBoardFileSystemStatus( pcMsg );
		break;

	case BOARD_LED_STATUS:
		ProcBoardLedStatus( pcMsg );
		break;

	//�����ǲ�������Ϣ
	case BOARD_MPC_CFG_TEST:
		ProcBoardGetCfgTest( pcMsg );
		break;

	case BOARD_MPC_GET_ALARM:
		ProcBoardGetAlarm( pcMsg );
		break;

	case BOARD_MPC_MANAGERCMD_TEST:
		ProcBoardGetLastManagerCmd( pcMsg );
		break;

	default:
        OspPrintf(TRUE, FALSE, "BoardAgent: receive unknown msg %d<%s> in mmpAgent InstanceEntry! \n",
                              pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
	
	return;
}

//ģ�ⵥ��BSP�ϵĺ��� ������
#ifdef WIN32
void BrdQueryPosition(TBrdPosition* ptPosition)
{
	return ;
}
#else
#ifndef MMP
void BrdQueryPosition(TBrdPosition* ptPosition)
{
	return ;
}
#endif
#endif


/*====================================================================
    ������      ��GetBoardCfgInfo
    ����        ����ȡҪ���ӵ�MPC��IP��ַ��PORT
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ���ɹ�����TRUE����֮����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/20    1.0         jianghy       ����
====================================================================*/
BOOL CBoardAgent::GetBoardCfgInfo()
{
    s8   achProfileName[32];
    BOOL bResult;
    s8   achDefStr[] = "Cannot find the section or key";
    s8   achReturn[MAX_VALUE_LEN + 1];
    s32  nValue;
    s8   achIpAddr[32];
    memset(achIpAddr, '\0', sizeof(achIpAddr));

	// �ڲ���ʱҪ�����ڵ�
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");
	bResult = GetRegKeyInt( achProfileName, "BoardDebug", "IsTest", 0, &nValue );
	if( bResult == TRUE )  
	{
		m_bIsTest = (nValue != 0);
		if(m_bIsTest)
        {
		    OspCreateTcpNode( 0, 4400 );
            OspPrintf(TRUE, FALSE, "[MmpAgent]OspCreateTcpNode( 0, 4400 )!\n");
        }
	}

#ifdef WIN32
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Layer", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "Layer" );
		return( FALSE );
	}
	m_tBoardPosition.byBrdLayer = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Slot", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "Slot" );
		return( FALSE );
	}
	m_tBoardPosition.byBrdSlot = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Type", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "Type" );
		return( FALSE );
	}
	m_tBoardPosition.byBrdID = (u8)nValue;
    
    memset(achReturn, '\0', sizeof(achReturn));
    bResult = GetRegKeyString( achProfileName, "BoardConfig", "BoardIpAddr", 
                                            achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[Mmp] Wrong profile while reading %s!\n", "BoardIpAddr" );

	}
    m_dwBoardIpAddr = inet_addr( achReturn );  // ������

#else
#ifndef MMP
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Layer", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "Layer" );
		return( FALSE );
	}
	m_tBoardPosition.byBrdLayer = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Slot", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "Slot" );
		return( FALSE );
	}
	m_tBoardPosition.byBrdSlot = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Type", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "Type" );
		return( FALSE );
	}
	m_tBoardPosition.byBrdID = (u8)nValue;

    memset(achReturn, '\0', sizeof(achReturn));
    bResult = GetRegKeyString( achProfileName, "BoardConfig", "BoardIpAddr", 
                                            achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[Mmp] Wrong profile while reading %s!\n", "BoardIpAddr" );
        m_dwBoardIpAddr = 0;
	}
    else
    {
        m_dwBoardIpAddr = ntohl( INET_ADDR( achReturn ) );
    }
#endif
#endif

	return TRUE;
}

/*====================================================================
    ������      ��ConnectManager
    ����        ����MPC�ϵĹ����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ���ɹ�����TRUE����֮����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/20    1.0         jianghy       ����
====================================================================*/
BOOL CBoardAgent::ConnectManager(u32& dwMcuNode, u32 dwMpcIp, u16 wMpcPort)
{
    BOOL bRet = TRUE;
    if( !OspIsValidTcpNode(dwMcuNode) )  // δ����������
    {
        dwMcuNode = OspConnectTcpNode( htonl(dwMpcIp), wMpcPort, 10, 3, 1000);// ����Mcu ����
        if( !OspIsValidTcpNode(dwMcuNode) )
        {
            brdagtlog( "CreateTcpNode with Manager Failed, the dst Ip: %0x, port: %d\n", 
                                                dwMpcIp, wMpcPort);
            bRet = FALSE;
        }
        else
        {
            OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());
            OspSetHBParam(dwMcuNode, g_cBrdAgentApp.m_wDiscHeartBeatTime, g_cBrdAgentApp.m_byDiscHeartBeatNum);            
            brdagtlog( "Connect with Manager(0x%0x:%d) Successfully, dwMcuNode:%d, HeartBeatTime:%d, HeartBeatNum:%d\n", 
                                       dwMpcIp, wMpcPort, dwMcuNode, g_cBrdAgentApp.m_wDiscHeartBeatTime, g_cBrdAgentApp.m_byDiscHeartBeatNum);
        }
    }
	return bRet;
}

/*=============================================================================
  �� �� ���� RegisterToMcuAgent
  ��    �ܣ� ���������ע��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::RegisterToMcuAgent(u32 dwDstNode)
{
    CServMsg  cReportMsg; 
    cReportMsg.SetMsgBody((u8*)&m_tBoardPosition, sizeof(TBrdPosition));
    cReportMsg.CatMsgBody((u8*)&m_dwBoardIpAddr, sizeof(u32));
    cReportMsg.CatMsgBody(&g_cBrdAgentApp.m_byChoice, sizeof(u8));
        
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

    // ��������ע��
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
====================================================================*/
void CBoardAgent::ProcBoardPowerOn( CMessage* const pcMsg )
{
    
    u8 byBrdId = 0;
    u8 byHWVer = 0;

	switch( CurState() ) 
	{
	case STATE_IDLE:
        if(0 != g_cBrdAgentApp.dwMpcIpAddr)
        {
            SetTimer(BRDAGENT_CONNECT_MANAGERA_TIMER, POWEN_ON_CONNECT);
        }
		if(0 != g_cBrdAgentApp.dwMpcIpAddrB)
        {
            SetTimer(BRDAGENT_CONNECT_MANAGERB_TIMER, POWEN_ON_CONNECT);
        }
        

#ifndef WIN32

        BrdQueryPosition( &m_tBoardPosition );

        TBrdEthParam tBrdEthParam; 
        BrdGetEthParam(g_cBrdAgentApp.m_byChoice, &tBrdEthParam);
        m_dwBoardIpAddr = tBrdEthParam.dwIpAdrs;  // ������

        byBrdId = BrdGetBoardID();
        byHWVer = BrdQueryHWVersion();
        if((BRD_TYPE_DEC5/*DSL8000_BRD_DEC5*/ == byBrdId) && (byHWVer >= 7))
        {
            g_cBrdAgentApp.m_byTvWallModel = TW_OUTPUTMODE_BOTH;
        }
        else if((BRD_TYPE_DEC5/*DSL8000_BRD_DEC5*/ == byBrdId) && (byHWVer < 7))
        {
            g_cBrdAgentApp.m_byTvWallModel = TW_OUTPUTMODE_VIDEO;
        }
#endif
#ifdef WIN32
        GetBoardCfgInfo(); // win32��ȡ�����ۺ�
#endif  
        break;

	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    ������      ��ProcBoardConnectManagerTimeOut
    ����        ������MANAGER��ʱ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBoardAgent::ProcBoardConnectManagerTimeOut( BOOL32 bIsConnectA )
{
    
    BOOL bRet = FALSE;
	switch( CurState() ) 
	{
	case STATE_IDLE:
    case STATE_INIT:
    case STATE_NORMAL:
        if(TRUE == bIsConnectA)
        {
            bRet = ConnectManager(m_dwDstNode, 
                    g_cBrdAgentApp.dwMpcIpAddr, g_cBrdAgentApp.wMpcPort); //��MPC�ϵĹ����������
            if( TRUE == bRet)
            {
                SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT );
                g_cBrdAgentApp.m_dwDstMcuNode = m_dwDstNode;
            }
            else
            {
                SetTimer( BRDAGENT_CONNECT_MANAGERA_TIMER, CONNECT_MANAGER_TIMEOUT );
            }
        }
		else
        {
            bRet = ConnectManager(m_dwDstNodeB, 
                    g_cBrdAgentApp.dwMpcIpAddrB, g_cBrdAgentApp.wMpcPortB); //��MPC�ϵĹ����������
            if( TRUE == bRet)
            {
                SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT );
                g_cBrdAgentApp.m_dwDstMcuNodeB = m_dwDstNodeB;
            }
            else
            {
                SetTimer( BRDAGENT_CONNECT_MANAGERB_TIMER, CONNECT_MANAGER_TIMEOUT );
            }
        }
		    
        if( bRet == TRUE && STATE_IDLE == CurState())
        {
            NEXTSTATE( STATE_INIT );
        }
		break;

	default:
		OspPrintf(TRUE, FALSE, "[Mmp] Wrong state %u (ProcBoardConnectManagerTimeOut)!\n", CurState() );
		break;
	}
	return;
}

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
void CBoardAgent::ProcBoardRegisterTimeOut( BOOL32 bIsConnectA )
{
	// OspPrintf(TRUE, FALSE, "[Mmp] Receive Register Manager Time Out Message! \n");

	switch( CurState() ) 
	{
	case STATE_INIT:
    case STATE_NORMAL:
        if(TRUE == bIsConnectA)
        {
            RegisterToMcuAgent(m_dwDstNode);
            SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT );
        }
        else
        {
            RegisterToMcuAgent(m_dwDstNodeB);
            SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT );
        }
		break;

	default:
		OspPrintf(TRUE, FALSE, "[Mmp] Wrong state %u (ProcBoardRegisterTimeOut)!\n", CurState() );
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
void CBoardAgent::ProcBoardRegNAck( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_INIT:
		if( pcMsg->srcnode == m_dwDstNode)
        {
            brdagtlog( "BoardAgent: Receive Register NAck Message from A board!\n");
        }
        if( pcMsg->srcnode == m_dwDstNodeB)
        {
            brdagtlog( "BoardAgent: Receive Register NAck Message from B board!\n");
        }
		break;

	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
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
void CBoardAgent::ProcBoardRegAck( CMessage* const pcMsg )
{
	// OspPrintf(TRUE, FALSE, "[Mmp] Receive Register Ack Message! \n");

	switch( CurState() ) 
	{
	case STATE_INIT:
    case STATE_NORMAL:
		if( pcMsg->srcnode == m_dwDstNode )
        {
	    	m_dwDstIId = pcMsg->srcid; // ��¼�Է�ʵ����
            KillTimer(BRDAGENT_REGISTERA_TIMER);

        }
        else if( pcMsg->srcnode == m_dwDstNodeB)
        {
            m_dwDstIIdB = pcMsg->srcid;
            KillTimer(BRDAGENT_REGISTERB_TIMER);
        }

		break;

	default:
		OspPrintf( TRUE, FALSE, "[Mmp] Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
    
    if(STATE_INIT == CurState())
    {
        PostGetCfgMsg( BOARD_MPC_GET_CONFIG, 
            (u8*)&m_tBoardPosition, sizeof(m_tBoardPosition) ); //���·���ȡ������Ϣ��Ϣ
        SetTimer( BRDAGENT_GET_CONFIG_TIMER, GET_CONFIG_TIMEOUT );
    }

	return;

}

/*=============================================================================
  �� �� ���� PostGetCfgMsg
  ��    �ܣ� ��ȡ������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u16 wEvent
             u8 * const pbyContent
             u16 wLen
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::PostGetCfgMsg( u16 wEvent, u8 * const pbyContent, u16 wLen )
{

    if( OspIsValidTcpNode( m_dwDstNodeB))
    {
        s32 nRet = post( m_dwDstIIdB, wEvent, pbyContent, wLen, m_dwDstNodeB );
        if ( OSP_OK != nRet )
		    brdagtlog( "[PostGetCfgMsg] node.B:%d, ret.%d!\n", m_dwDstNodeB, nRet);
    }
    else
    {
        // OspPrintf(TRUE, FALSE, "[PostGetCfgMsg] node.B:%d invalid!\n", m_dwDstNodeB );
    }
    
    if( OspIsValidTcpNode( m_dwDstNode ))
	{
	    s32 nRet = post( m_dwDstIId, wEvent, pbyContent, wLen, m_dwDstNode );
        if ( OSP_OK != nRet )
            brdagtlog( "[PostGetCfgMsg] node.A:%d, ret.%d!\n", m_dwDstNode, nRet);
	}
    else
    {
        // OspPrintf(TRUE, FALSE, "[PostGetCfgMsg] node.A:%d invalid!\n", m_dwDstNode );
    }

    SetTimer( BRDAGENT_GET_CONFIG_TIMER, GET_CONFIG_TIMEOUT );
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
void CBoardAgent::ProcGetConfigTimeOut( CMessage* const pcMsg )
{
	PostGetCfgMsg( BOARD_MPC_GET_CONFIG, (u8*)&m_tBoardPosition, sizeof(m_tBoardPosition) ); //���·���ȡ������Ϣ��Ϣ
	return;
}


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
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
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

    04/11/11    3.5         �� ��          �½ӿڵ��޸�
====================================================================*/
void CBoardAgent::ProcBoardGetConfigAck( CMessage* const pcMsg )
{
	u8	byPeriCount;
	u8	byPeriType;
	u16	wLoop, wIndex;
	TEqpMixerEntry*		ptMixerCfg;		//MIXER��������Ϣ
	TEqpTVWallEntry*	ptTVWallCfg;	//TVWALL��������Ϣ
	TEqpBasEntry*		ptBasCfg;		//BAS��������Ϣ
	TEqpVMPEntry*		ptVMPCfg;		//VMP��������Ϣ
	TEqpPrsEntry*		ptPrsCfg;		//Prs��������Ϣ
    TEqpMPWEntry*		ptMpwCfg;
    	
	switch( CurState() ) 
	{
	case STATE_INIT:
//    case STATE_NORMAL: // ��Mcu���ú�Mcu������������ʱMmp״̬ΪNormal
		
		byPeriCount = pcMsg->content[0]; // �����������
		wIndex = 1;

		for( wLoop = 0; wLoop < byPeriCount; wLoop++ )
		{			
			byPeriType = pcMsg->content[wIndex++]; // ��������
			
            switch( byPeriType ) // ���øõ�����Ӧ��������Ϣ
			{
			case EQP_TYPE_TVWALL:
				ptTVWallCfg = (TEqpTVWallEntry*)(pcMsg->content+wIndex);
				wIndex += sizeof(TEqpTVWallEntry);
				g_cBrdAgentApp.SetTVWallConfig(ptTVWallCfg);
				break;
				
			case EQP_TYPE_MIXER:
				ptMixerCfg = (TEqpMixerEntry*)(pcMsg->content+wIndex);
				wIndex += sizeof(TEqpMixerEntry);
				g_cBrdAgentApp.SetMixerConfig(ptMixerCfg);
				break;
				
			case EQP_TYPE_BAS:
				ptBasCfg = (TEqpBasEntry*)(pcMsg->content+wIndex);
				wIndex += sizeof(TEqpBasEntry);
				g_cBrdAgentApp.SetBasConfig(ptBasCfg);
				break;
				
			case EQP_TYPE_VMP:
				ptVMPCfg = (TEqpVMPEntry*)(pcMsg->content+wIndex);
				wIndex += sizeof(TEqpVMPEntry);
				g_cBrdAgentApp.SetVMPConfig(ptVMPCfg);
				break;

            case EQP_TYPE_VMPTW:
                ptMpwCfg = (TEqpMPWEntry*)(pcMsg->content+wIndex);
                wIndex += sizeof(TEqpMPWEntry);
                g_cBrdAgentApp.SetMpwConfig(ptMpwCfg);
                break;

			case EQP_TYPE_PRS:
				ptPrsCfg = (TEqpPrsEntry*)(pcMsg->content+wIndex);
                ptPrsCfg->SetMcuIp(g_cBrdAgentApp.dwMpcIpAddr);
				wIndex += sizeof(TEqpPrsEntry);
				g_cBrdAgentApp.SetPrsConfig(ptPrsCfg);
				break;
			}
		}
		
#ifndef WIN32
#ifdef MMP
		BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON );
#endif
#endif

		OspSemGive( g_semMMP );

		// �Ժ����Ƿ�ʹ�ÿ��Ź�
		if( pcMsg->content[ wIndex + 6 ] == 0 )
		{
#ifndef WIN32
			SysRebootDisable( );
#endif
		}
		else
		{
#ifndef WIN32
			SysRebootEnable( );
#endif
		}

		KillTimer( BRDAGENT_GET_CONFIG_TIMER );
		NEXTSTATE( STATE_NORMAL );
		break;
		
	default:
		OspPrintf( TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
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
void CBoardAgent::ProcAlarmSyncReq( CMessage* const pcMsg )
{
	u16 wLoop;
	u16 wCount=0, wLen;
	u8 abyAlarmBuf[ MAXNUM_BOARD_ALARM*sizeof(TBoardAlarmMsgInfo) + 10 ];
	TBoardAlarmMsgInfo* pAlarmMsgInfo;

	memset(abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	switch( CurState() ) 
	{
    case STATE_INIT:
	case STATE_NORMAL:
		//�����Լ��������Ϣ
		memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );

		//ɨ���Լ��ĸ澯��
		pAlarmMsgInfo = (TBoardAlarmMsgInfo*)(abyAlarmBuf + sizeof(m_tBoardPosition) + sizeof(u16));
		for( wLoop=0; wLoop<m_dwBoardAlarmTableMaxNo; wLoop++ )
		{
			if( m_atBoardAlarmTable[wLoop].bExist )
			{
				memcpy(pAlarmMsgInfo, &m_atBoardAlarmTable[wLoop].tBoardAlarm, sizeof(TBoardAlarmMsgInfo) );
				pAlarmMsgInfo++;
				wCount++;
			}
		}

		//�澯����
		*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(wCount); 
		wLen = sizeof(m_tBoardPosition) + sizeof(u16) + wCount*sizeof(TBoardAlarmMsgInfo);

		PostMsgToManager( BOARD_MPC_ALARM_SYNC_ACK, abyAlarmBuf, wLen );
		break;
		
	default:
		OspPrintf( TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

#ifdef WIN32
//ģ��BSP����
u8 BrdQueryHWVersion()
{
	return 1;
}
u8 BrdQueryFPGAVersion()
{
	return 1;
}

#else
/*
#ifndef MMP
//ģ��BSP����
u8 BrdQueryHWVersion()
{
	return 1;
}
u8 BrdQueryFPGAVersion()
{
	return 1;
}
#endif
*/
#endif

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
void CBoardAgent::ProcGetVersionReq( CMessage* const pcMsg )
{
	s8 abyBuf[128];
	u16 wLen;

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
		//ȡ�汾��Ϣ
		memset(abyBuf, 0, sizeof(abyBuf) );

		memcpy(abyBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );

		wLen = sprintf(abyBuf+sizeof(m_tBoardPosition),"Hardware %d| FPGA %d ",
			BrdQueryHWVersion(), BrdQueryFPGAVersion() );

		//���͸�MANAGER��Ӧ
		PostMsgToManager( BOARD_MPC_GET_VERSION_ACK, (u8*)&abyBuf, wLen+sizeof(m_tBoardPosition)+1 );
		break;
		
	default:
		OspPrintf( TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
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
void CBoardAgent::ProcGetModuleInfoReq( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:		
		//���͸�MANAGER��Ӧ
		PostMsgToManager( BOARD_MPC_GET_MODULE_ACK, (u8*)&m_tBoardPosition, sizeof(m_tBoardPosition) );
		break;
		
	default:
		OspPrintf( TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
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
void CBoardAgent::TestingProcess( CMessage* const pcMsg )
{
	if( m_bIsTest == FALSE )
    {
		return;
    }
	
	m_wLastEvent = pcMsg->event;
	if(pcMsg->content != NULL)
	{
		if( pcMsg->length < sizeof(m_abyLastEventPara) )
        {
			memcpy(m_abyLastEventPara, pcMsg->content, pcMsg->length );
        }
		else
        {
			memcpy(m_abyLastEventPara, pcMsg->content, sizeof(m_abyLastEventPara) );
        }
	}
	else
	{
		memset(m_abyLastEventPara, 0, sizeof(m_abyLastEventPara) );
	} 
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
void CBoardAgent::ProcBoardResetCmd( CMessage* const pcMsg )
{

    switch( CurState() )
    {
    case STATE_INIT:
    case STATE_NORMAL:
    #ifndef WIN32
        OspDelay(2000);
        BrdHwReset();
    #endif
        //������
        //TestingProcess( pcMsg );
        break;

    default:
        OspPrintf( TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
                                pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        break;
    }
    return;
}

/*====================================================================
    ������      ��MoveFilesToTarget
    ����        ����FTPĿ¼�е��ļ����������ƶ�����ͬ���ļ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����s8* pbyFileName:���ƶ����ļ����б�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
/*
void MoveFilesToTarget( s8* pbyFileName )
{
#ifndef WIN32

#ifndef WIN32
	DIR *ptDir;
#endif
	BOOL bNext;
	s8 *lpszBgn, *lpszEnd;
	s8 achFileName[256];
	s8 achSrcFullName[256];
	s8 achDstFullName[256];
	
	memset( achFileName, 0, sizeof(achFileName) );
	memset( achSrcFullName, 0, sizeof(achSrcFullName) );
	memset( achDstFullName, 0, sizeof(achDstFullName) );
	
	// move files in sequence
	bNext = TRUE;
	lpszBgn = pbyFileName;
	while( bNext )
	{
		// find the first end
		lpszEnd = strchr( lpszBgn, (int)CHAR_FTPFILE_SEPARATOR );
		if( lpszEnd == NULL )		// end 
		{
			bNext = FALSE;
			lpszEnd = lpszBgn + strlen( lpszBgn ) - 1;
		}
		else
		{
			lpszEnd = lpszEnd - 1;
		}
		
		// find the first type separator
		lpszEnd = (s8*)memchr( lpszBgn, (int)CHAR_FTPFILETYPE_SEPARATOR, lpszEnd - lpszBgn );
		if( lpszEnd == NULL )
			return;
		memcpy( achFileName, lpszBgn, lpszEnd - lpszBgn );
		achFileName[lpszEnd - lpszBgn] = '\0';
		sprintf( achSrcFullName, "%s/%s", DIR_FTP, achFileName );
		
		// analyze the file type
		switch( lpszEnd[1] )
		{
		case CHAR_EXE_TYPE:
		case CHAR_EXE_TYPE + 'a' - 'A':
#ifndef WIN32
			if( ( ptDir = opendir( ( s8* )PATH_BIN ) ) == NULL )
				mkdir( ( s8* )PATH_BIN );
			else
				closedir( ptDir );
#endif
			sprintf( achDstFullName, "%s/%s", PATH_BIN, achFileName );
			break;
		case CHAR_RES_TYPE:
		case CHAR_RES_TYPE + 'a' - 'A':
#ifndef WIN32
			if( ( ptDir = opendir( ( s8* )PATH_RES ) ) == NULL )
				mkdir( ( s8* )PATH_RES );
			else
				closedir( ptDir );
#endif
			sprintf( achDstFullName, "%s/%s", PATH_RES, achFileName );
			break;
		case CHAR_CFG_TYPE:
		case CHAR_CFG_TYPE + 'a' - 'A':
		default:
#ifndef WIN32
			if( ( ptDir = opendir( ( s8* )DIR_CONFIG ) ) == NULL )
				mkdir( ( s8* )DIR_CONFIG );
			else
				closedir( ptDir );
#endif

			sprintf( achDstFullName, "%s/%s", DIR_CONFIG, achFileName );
			break;
		}
		
		// copy the file from ftp directory to specified directory
#ifndef WIN32
		if( mv( (s8*)achSrcFullName, (s8*)achDstFullName ) == ERROR )
		{
			OspPrintf( TRUE, FALSE, "move file from %s to %s fail!\n", achSrcFullName, achDstFullName );
			return;
		}
#endif		
		lpszBgn = lpszEnd + 3;
	}
	return;
#endif
}
*/
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
	06/04/19    4.0         john          �޸�������ʽ
====================================================================*/
void CBoardAgent::ProcUpdateSoftwareCmd( CMessage* const pcMsg )
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
	OspPrintf(TRUE, FALSE, "BoardAgent: Rcv Update Software Msg: byBrdId.%d byNum.%d\n", byBrdId, byNum);
	
	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
		if( g_bUpdateSoftBoard )
		{
			s8 achFileName[256];
            s8 achTmpName[256];
            s8 *lpMsgBody = (s8*)(cServMsg.GetMsgBody()+sizeof(u8)+sizeof(u8));
			u8  byLen = 0;
            u16 wOffSet = 0;

			while( byTmpNum-- > 0 )
			{
                byFileNum ++;
                
				memset( achFileName, 0, sizeof(achFileName) );
                byLen = *lpMsgBody;
                if (byLen + wOffSet > cServMsg.GetMsgBodyLen()-2)
                {
                    brdagtlog( "[ProcUpdateSoftwareCmd] invalid msgbody, byLen:%d, offset:%d, wMsgLen:%d!!!\n",
                                            byLen, wOffSet, cServMsg.GetMsgBodyLen()-2);
                    abyRet[byFileNum] = 0;
                    break;
                }
                memcpy(achFileName, lpMsgBody+sizeof(u8), byLen);
                wOffSet = sizeof(byLen) + byLen;
                lpMsgBody += wOffSet;
                OspPrintf(TRUE, FALSE, "[ProcUpdateSoftwareCmd] filename: %s\n", achFileName);
				
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
				OspPrintf(TRUE, FALSE, "[ProcUpdateSoftwareCmd] updating file name is :%s\n", achFileName);
				
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
						OspPrintf(TRUE, FALSE, "[ProcUpdateSoftwareCmd] fail to rename file %s to %s.\n", achFileName, achOldFile);
					}
					else// ���ļ�����
					{
						nRet = rename(achTmpName, achFileName);
						if( ERROR == nRet )
						{
                            abyRet[byFileNum] = 0;
							OspPrintf(TRUE, FALSE, "[ProcUpdateSoftwareCmd] fail to rename file %s to %s.\n", achTmpName, achFileName);						
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
                //�ж��Ƿ�image�ļ�
                s32 nNameOff = strlen(achFileName) - strlen(LINUXAPP_POSTFIX);
                if (nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, LINUXAPP_POSTFIX) )
                {
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
                else
                {
                    //�ж��Ƿ�os
                    nNameOff = strlen(achFileName) - strlen(LINUXOS_POSTFIX);
                    if (nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, LINUXOS_POSTFIX) )
                    {
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
                        OspPrintf(TRUE, FALSE, "[ProcUpdateBrdVersion] not supported file type.\n");
                    }
                }
#endif
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
        }
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
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
void CBoardAgent::ProcTimeSyncCmd( CMessage * const pcMsg )
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
        OspPrintf( TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
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
void CBoardAgent::ProcBoardSelfTestCmd( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_NORMAL:
		//������
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
void CBoardAgent::ProcBitErrorTestCmd( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_NORMAL:
		//������
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
void CBoardAgent::ProcGetStatisticsReq( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_NORMAL:
		break;
		
	default:
		OspPrintf( TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
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

    04/11/11    3.5         �� ��          �½ӿڵ��޸�
====================================================================*/
void CBoardAgent::ProcLedStatusReq( CMessage* const pcMsg )
{
#ifndef WIN32
#ifdef MMP
	u8  abyBuf[32];
	u8  byCount;
    TBrdLedState tBrdLedState;

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
        BrdQueryLedState( &tBrdLedState );
        memcpy(&m_tLedState, &tBrdLedState.nlunion.tBrdMMPLedState, sizeof(TBrdMMPLedStateDesc));
		
		memcpy(abyBuf, &m_tBoardPosition, sizeof(m_tBoardPosition) );
		byCount = sizeof(m_tLedState);
		memcpy(abyBuf+sizeof(m_tBoardPosition), &byCount, sizeof(byCount) ); 
		memcpy(abyBuf+sizeof(m_tBoardPosition)+sizeof(byCount), &m_tLedState, sizeof(m_tLedState));
		
		PostMsgToManager( BOARD_MPC_LED_STATUS_ACK, abyBuf, 
			sizeof(m_tBoardPosition)+sizeof(u8)+sizeof(m_tLedState) );
		break;
		
	default:
		OspPrintf( TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
#endif
#endif
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
BOOL CBoardAgent::PostMsgToManager( u16 wEvent, u8 * const pbyContent, u16 wLen )
{
    if( OspIsValidTcpNode( m_dwDstNode ))
	{
        s32 nRet = post( m_dwDstIId, wEvent, pbyContent, wLen, m_dwDstNode );
        if ( OSP_OK != nRet )
            brdagtlog( "[PostMsgToManager] Post message %u(%s) to nodeA: %d, ret: %d!\n", 
                                wEvent, ::OspEventDesc( wEvent ),  m_dwDstNode, nRet );
	}
	else
	{
        // OspPrintf( TRUE, FALSE, "[PostMsgToManager] m_dwDstNodeA.%d is invalid !\n", m_dwDstNode );
	}

    if( OspIsValidTcpNode( m_dwDstNodeB))
    {
        s32 nRet = post( m_dwDstIIdB, wEvent, pbyContent, wLen, m_dwDstNodeB );
        if ( OSP_OK != nRet )
            brdagtlog( "[PostMsgToManager] Post message %u(%s) to nodeB: %d, ret: %d!\n", 
                                wEvent, ::OspEventDesc( wEvent ),  m_dwDstNodeB, nRet );
    }
    else
    {
        // OspPrintf( TRUE, FALSE, "[PostMsgToManager] m_dwDstNodeB.%d is invalid !\n", m_dwDstNodeB );
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

    04/11/11    3.5         �� ��          �½ӿڵ��޸�
====================================================================*/
void CBoardAgent::ProcOspDisconnect( CMessage* const pcMsg )
{
	
    u32 dwDisNode = *(u32*)pcMsg->content;
    if (INVALID_NODE != dwDisNode)
    {
        OspDisconnectTcpNode(dwDisNode);
    }

    if(m_dwDstNode == dwDisNode)
    {
        FreeDataA();
        g_cBrdAgentApp.m_dwDstMcuNode = INVALID_NODE;
        SetTimer( BRDAGENT_CONNECT_MANAGERA_TIMER, CONNECT_MANAGER_TIMEOUT );
        brdagtlog( " Disconnect with A's Node: %d\n", m_dwDstNode);
		m_tLastDisTimeA = time(0); // ȡ��ǰʱ��
		
    }
    else if( dwDisNode == m_dwDstNodeB)
    {
        FreeDataB();
        g_cBrdAgentApp.m_dwDstMcuNodeB = INVALID_NODE;
        SetTimer( BRDAGENT_CONNECT_MANAGERB_TIMER, CONNECT_MANAGER_TIMEOUT );
        brdagtlog( " Disconnect with B's Node: %d\n", m_dwDstNodeB);
		m_tLastDisTimeB = time(0); // ȡ��ǰʱ��
    }

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:

        if(INVALID_NODE == m_dwDstNode && INVALID_NODE == m_dwDstNodeB)
        {
    #ifndef WIN32
    #ifdef MMP
		    BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF ); // ָʾ�ƹر�
    #endif
    #endif
            NEXTSTATE( STATE_IDLE );
            OspDelay(5*1000);
        }
		else
		{
///////////////////////////////////////////////////////////////////////
//////////////   �Ծ���ʱ�����ж�Mpc���Ⱥ�����
/*			
			struct tm   *ptTimeA;
			ptTimeA = localtime(&m_tLastDisTimeA); // ת��Ϊ��׼ʱ��
			struct tm   *ptTimeB;
			ptTimeB = localtime(&m_tLastDisTimeB);
			BOOL32 bRet = Compare2TimeIsValid(ptTimeA, ptTimeB);
			if( bRet )
			{
#ifndef WIN32
#ifdef MMP
		    BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF ); // ָʾ�ƹر�
#endif
#endif
            // NEXTSTATE( STATE_IDLE );
			reboot(6); // ��������
			}
*/
////////////////////////////////////////////////////////////////////////
		}
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}
/*=============================================================================
  �� �� ���� Compare2TimeIsValid
  ��    �ܣ� �Ƚ�����ʱ���Ƿ񳬹�ָ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� struct tm* ptTimeA
             struct tm* ptTimeB
  �� �� ֵ�� BOOL32:TRUE: ���� FALSE:û�г���
=============================================================================*/
BOOL32 CBoardAgent::Compare2TimeIsValid(struct tm* ptTimeA, struct tm* ptTimeB)
{
	BOOL32 bRet = TRUE;
	if( ptTimeA->tm_year != ptTimeB->tm_year )
	{
		bRet = FALSE;
	}
	else if (ptTimeA->tm_mon != ptTimeB->tm_mon)
	{
		bRet = FALSE;
	}
	else if(ptTimeA->tm_mday != ptTimeB->tm_mday)
	{
		bRet = FALSE;
	}
	else if(ptTimeA->tm_hour != ptTimeB->tm_hour)
	{
		bRet = FALSE;
	}
	else if( abs(ptTimeA->tm_min - ptTimeB->tm_min) > MAX_TIME_SPAN_REBOOT)
	{
		bRet = FALSE;
	}
	else
	{
		bRet = TRUE;  // ���뼶����3�������ڣ���Ϊ���Ⱥ���������Mpc
	}
	return bRet;
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
void CBoardAgent::ProcBoardMemeryStatus( CMessage* const pcMsg )
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	if( FindAlarm( ALARM_MCU_MEMORYERROR, ALARMOBJ_MCU_SOFTWARE, byAlarmObj, &tAlarmData ) )
	{
		if( *( pcMsg->content ) == 0 )      //normal
		{
			if( DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ) == FALSE )
			{
				brdagtlog( "BoardAgent: DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
				tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0], 
				tAlarmData.byBoardAlarmObject[1] );
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );
			
				memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
				memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

                OspPrintf(TRUE, TRUE, "PostMsgToManager( BOARD_MPC_ALARM_NOTIFY00\n");

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	else        //no such alarm
	{
		if( *( pcMsg->content ) == 1 )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_MEMORYERROR, ALARMOBJ_MCU_SOFTWARE, byAlarmObj, &tAlarmData ) == FALSE )
			{
				brdagtlog( "BoardAgent: AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg);

				memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
				memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

                OspPrintf(TRUE, TRUE, "PostMsgToManager( BOARD_MPC_ALARM_NOTIFY\n");

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg) );
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
void CBoardAgent::ProcBoardModuleStatus( CMessage* const pcMsg )
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	if( FindAlarm( ALARM_MCU_MODULE_OFFLINE, ALARMOBJ_MCU_MODULE, byAlarmObj, &tAlarmData ) )
	{
		if( *( pcMsg->content ) == 0 )      //normal
		{
			if( DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ) == FALSE )
			{
				brdagtlog( "BoardAgent: DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
				tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0], 
				tAlarmData.byBoardAlarmObject[1] );
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );
			
				memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
				memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	else        //no such alarm
	{
		if( *( pcMsg->content ) == 1 )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_MODULE_OFFLINE, ALARMOBJ_MCU_MODULE, byAlarmObj, &tAlarmData ) == FALSE )
			{
				brdagtlog( "BoardAgent: AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg );
			
				memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
				memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
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
#ifndef WIN32
#ifdef MMP
	u8 abyBuf[32];
	u8 byCount;
	TBrdMMPLedStateDesc* ptLedState;
	ptLedState = (TBrdMMPLedStateDesc*)pcMsg->content;

	if( memcmp(&m_tLedState, ptLedState, sizeof(*ptLedState)) == 0 )
    {
		return; 
    }

	memcpy(abyBuf, &m_tBoardPosition, sizeof(m_tBoardPosition) );
	byCount = sizeof(*ptLedState);
	memcpy(abyBuf+sizeof(m_tBoardPosition), &byCount, sizeof(byCount) ); 
	memcpy(abyBuf+sizeof(m_tBoardPosition)+sizeof(byCount), ptLedState, sizeof(*ptLedState));
					
	PostMsgToManager( BOARD_MPC_LED_STATUS_NOTIFY, abyBuf, 
		sizeof(m_tBoardPosition)+sizeof(u8)+sizeof(*ptLedState) );
#endif
#endif
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
void CBoardAgent::ProcBoardFileSystemStatus( CMessage* const pcMsg )
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	if( FindAlarm( ALARM_MCU_FILESYSTEMERROR, ALARMOBJ_MCU_SOFTWARE, byAlarmObj, &tAlarmData ) )
	{
		if( *( pcMsg->content ) == 0 )      //normal
		{
			if( DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ) == FALSE )
			{
				brdagtlog( "BoardAgent: DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
				tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0], 
				tAlarmData.byBoardAlarmObject[1] );
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );
			
				memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
				memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	else        //no such alarm
	{
		if( *( pcMsg->content ) == 1 )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_FILESYSTEMERROR, ALARMOBJ_MCU_SOFTWARE, byAlarmObj, &tAlarmData ) == FALSE )
			{
				brdagtlog( "BoardAgent: AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg );
			
				memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
				memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg) );
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
BOOL CBoardAgent::AddAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData )
{
    u16    wLoop;

	/*�ҵ�һ���ձ���*/
    for( wLoop = 0; wLoop < m_dwBoardAlarmTableMaxNo; wLoop++ )
    {
        if( !m_atBoardAlarmTable[wLoop].bExist )
            break;
    }
    
    if( wLoop >= sizeof( m_atBoardAlarmTable ) / sizeof( m_atBoardAlarmTable[0] ) )
    {
        return( FALSE );
    }

	/*��д�ø澯����*/
    m_atBoardAlarmTable[wLoop].dwBoardAlarmCode = dwAlarmCode;
    m_atBoardAlarmTable[wLoop].byBoardAlarmObjType = byObjType;
    memcpy( m_atBoardAlarmTable[wLoop].byBoardAlarmObject, byObject, 
        sizeof( m_atBoardAlarmTable[0].byBoardAlarmObject ) );
    m_atBoardAlarmTable[wLoop].bExist = TRUE;
	
    // copy
    ptData->dwBoardAlarmSerialNo = wLoop + 1;
    ptData->dwBoardAlarmCode = m_atBoardAlarmTable[wLoop].dwBoardAlarmCode;
    ptData->byBoardAlarmObjType = m_atBoardAlarmTable[wLoop].byBoardAlarmObjType;
    memcpy( ptData->byBoardAlarmObject, m_atBoardAlarmTable[wLoop].byBoardAlarmObject, 
        sizeof( ptData->byBoardAlarmObject ) );
	
    // increase maxno
    if( wLoop == m_dwBoardAlarmTableMaxNo )
        m_dwBoardAlarmTableMaxNo++;

     return( TRUE );
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
BOOL CBoardAgent::SetAlarmMsgInfo( u32 dwSerialNo, TBoardAlarmMsgInfo* const ptMsg )
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
BOOL CBoardAgent::DeleteAlarm( u32 dwSerialNo )
{	
    u32   dwLoop;

    m_atBoardAlarmTable[dwSerialNo - 1].bExist = FALSE;
        
    if( dwSerialNo == m_dwBoardAlarmTableMaxNo )     //the last one
    {
        for( dwLoop = m_dwBoardAlarmTableMaxNo - 1; dwLoop >= 0; dwLoop-- )
        {
            if( m_atBoardAlarmTable[dwLoop].bExist )
                break;
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
BOOL CBoardAgent::FindAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData )
{
    u16    wLoop;
    BOOL    bFind = FALSE;

	/*���Ҹ澯���е�ÿһ��*/
    for( wLoop = 0; wLoop < m_dwBoardAlarmTableMaxNo; wLoop++ )
    {
		/*������ڣ���������һ��*/
        if( !m_atBoardAlarmTable[wLoop].bExist )
            continue;

		/*�澯��͸澯��������Ҫһ��*/
        if( dwAlarmCode == m_atBoardAlarmTable[wLoop].dwBoardAlarmCode 
            && byObjType == m_atBoardAlarmTable[wLoop].byBoardAlarmObjType )
        {
			/*�澯����Ҫһ��*/
            switch( byObjType )
            {
			case ALARMOBJ_MCU_SOFTWARE:
			case ALARMOBJ_MCU_MODULE:
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
#ifdef _DEBUG
                brdagtlog( "BoardAgent: Wrong AlarmObjType in FindAlarm()!\n" );
#endif
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
void CBoardAgent::ProcBoardGetCfgTest( CMessage* const pcMsg )
{
	TEqpTVWallEntry tTVWallCfg;
/*	STRUCT_mcueqpMixerEntry_mem		tMixerCfg;
	STRUCT_mcueqpBasEntry_mem		tBasCfg;
	STRUCT_mcueqpVMPEntry_mem		tVMPCfg;
*/
	//switch(  g_cBrdAgentApp.byPeriType )
	//{
	//case EQP_TYPE_TVWALL:
		tTVWallCfg = g_cBrdAgentApp.m_tTVWallCfg;

		tTVWallCfg.SetIpAddr(tTVWallCfg.GetIpAddr());
		tTVWallCfg.SetMcuIp(tTVWallCfg.GetMcuIp());
		tTVWallCfg.SetMcuPort(tTVWallCfg.GetMcuPort());
		tTVWallCfg.SetEqpRecvPort(tTVWallCfg.GetEqpRecvPort());
        OspPrintf(1, 1, "mcueqpTVWallEntIpAddr = 0x%x\n", tTVWallCfg.GetIpAddr());
        OspPrintf(1, 1, "mcueqpTVWallEntConnMcuIp = 0x%x\n", tTVWallCfg.GetMcuIp());
        OspPrintf(1, 1, "mcueqpTVWallEntConnMcuPort = 0x%x\n", tTVWallCfg.GetMcuPort());
        OspPrintf(1, 1, "mcueqpTVWallEntVideoStartRecvPort = 0x%x\n", tTVWallCfg.GetEqpRecvPort());
		SetSyncAck(&tTVWallCfg, sizeof(tTVWallCfg));
		//break;
/*
	case EQP_TYPE_MIXER:
		tMixerCfg = g_cBrdAgentApp.m_tMixerCfg;
		tMixerCfg.mcueqpMixerEntPort = htons(tMixerCfg.mcueqpMixerEntPort);
		tMixerCfg.mcueqpMixerEntIpAddr = htonl(tMixerCfg.mcueqpMixerEntIpAddr);
		tMixerCfg.mcueqpMixerEntMixStartPort = htons(tMixerCfg.mcueqpMixerEntMixStartPort);
		tMixerCfg.mcueqpMixerEntConnMcuPort = htons(tMixerCfg.mcueqpMixerEntConnMcuPort);
		tMixerCfg.mcueqpMixerEntConnMcuIp = htonl(tMixerCfg.mcueqpMixerEntConnMcuIp);
		SetSyncAck(&tMixerCfg, sizeof(tMixerCfg));

		break;

	case EQP_TYPE_VMP:
		tVMPCfg = g_cBrdAgentApp.m_tVMPCfg;
		tVMPCfg.mcueqpVMPEntPort = htons(tVMPCfg.mcueqpVMPEntPort);
		tVMPCfg.mcueqpVMPEntIpAddr = htonl(tVMPCfg.mcueqpVMPEntIpAddr);
		tVMPCfg.mcueqpVMPEntRecvStartPort = htons(tVMPCfg.mcueqpVMPEntRecvStartPort);
		tVMPCfg.mcueqpVMPEntConnMcuPort = htons(tVMPCfg.mcueqpVMPEntConnMcuPort);
		tVMPCfg.mcueqpVMPEntConnMcuIp = htonl(tVMPCfg.mcueqpVMPEntConnMcuIp);
		SetSyncAck(&tVMPCfg, sizeof(tVMPCfg));

		break;

	case EQP_TYPE_BAS:
		tBasCfg = g_cBrdAgentApp.m_tBasCfg;
		tBasCfg.mcueqpBasEntPort = htons(tBasCfg.mcueqpBasEntPort);
		tBasCfg.mcueqpBasEntIpAddr = htonl(tBasCfg.mcueqpBasEntIpAddr);
		tBasCfg.mcueqpBasEntRecvStartPort = htons(tBasCfg.mcueqpBasEntRecvStartPort);
		tBasCfg.mcueqpBasEntConnMcuPort = htons(tBasCfg.mcueqpBasEntConnMcuPort);
		tBasCfg.mcueqpBasEntConnMcuIp = htonl(tBasCfg.mcueqpBasEntConnMcuIp);
		SetSyncAck(&tBasCfg, sizeof(tBasCfg));
		break;

	default:
		break;
	}*/
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
void CBoardAgent::ProcBoardGetAlarm( CMessage* const pcMsg )
{
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
			*pbyBuf = m_tBoardPosition.byBrdLayer;
			pbyBuf++;
			*pbyBuf = m_tBoardPosition.byBrdSlot;
			pbyBuf++;
			*pbyBuf = m_tBoardPosition.byBrdID;
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
void CBoardAgent::ProcBoardGetLastManagerCmd( CMessage* const pcMsg )
{
	u8 abyBuf[128];

	//ֱ�ӷ������һ����Ϣ�������Ϣ
	*(u16*)abyBuf = htons(m_wLastEvent);
	memcpy(abyBuf+2, m_abyLastEventPara, sizeof(m_abyLastEventPara) );
	SetSyncAck( abyBuf, sizeof(u16)+sizeof(m_abyLastEventPara) );
	return;
}

API u32 BrdGetDstMcuNode( void )
{
    return g_cBrdAgentApp.m_dwDstMcuNode;
}

API u32 BrdGetDstMcuNodeB( void )
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}
API u8 BrdGetTvwallModel(void)
{
    return g_cBrdAgentApp.m_byTvWallModel;
}

API void noupdatesoft( void )
{
	g_bUpdateSoftBoard = FALSE;
}

API void updatesoft( void )
{
	g_bUpdateSoftBoard = TRUE;
}

// test update file
/*
API void ud( void )
{
	::OspPost( MAKEIID( AID_MCU_BRDAGENT, 1), BOARD_MPC_UPDATE_SOFTWARE_CMD, ".\\kdvvmp.outudp", sizeof(".\\kdvvmp.outudp"));
}
*/
