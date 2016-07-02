/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : criagent.cpp
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
#include "evagtsvc.h"
#include "dscagent.h"

#ifdef _VXWORKS_
#include "brddrvlib.h"
#endif

#define DISCONNECT_MCUA    (u16)1
#define DISCONNECT_MCUB    (u16)2

#define UPDATE_FILEBUF_SIZE	 (u32)(100*1024)

CBrdAgentApp	g_cBrdAgentApp;	//�������Ӧ��ʵ��

extern SEMHANDLE g_semDSC;
BOOL32 g_bUpdateSoftBoard = TRUE;
BOOL32 g_bPrintDscLog = FALSE;

void Dsclog(char * fmt, ...)
{
    char PrintBuf[255];
    memset(PrintBuf, 0, sizeof(PrintBuf));
    int BufLen = 0;
    va_list argptr;
	if (g_bPrintDscLog)
	{
		BufLen = sprintf(PrintBuf, "[Dsc]:");
		va_start(argptr, fmt);
		BufLen += vsprintf(PrintBuf+BufLen, fmt, argptr);
		va_end(argptr);
		BufLen += sprintf(PrintBuf+BufLen, "\n");
		OspPrintf(TRUE, FALSE, PrintBuf);
	}
    return;
}

//���캯��
CBoardAgent::CBoardAgent()
{
	memset(&m_tBoardPosition, 0, sizeof(m_tBoardPosition) );
	m_bIsTest = FALSE;
    FreeDataA();
    FreeDataB();

	memset(m_achCurUpdateFileName, 0, sizeof(m_achCurUpdateFileName));
	memset(m_abyUpdateResult, 0, sizeof(m_abyUpdateResult));
	m_byUpdateFileNum = 0;
	m_byWaitTimes = 0;
	m_byErrTimes = 0;
	m_byBrdIdx = 0;
	m_byMcsSsnIdx = 0;
	m_hUpdateFile = NULL;
	m_pbyFileBuf = NULL;
	m_dwBufUsedSize = 0;
	return;
}

// ��������
CBoardAgent::~CBoardAgent()
{
	m_hUpdateFile = NULL;
	m_pbyFileBuf = NULL;
    return;
}

void CBoardAgent::FreeDataA(void)
{
	m_dwDstNode = INVALID_NODE;
	m_dwDstIId = INVALID_INS;
    return;
}

void CBoardAgent::FreeDataB(void)
{
    m_dwDstIIdB = INVALID_INS;
    m_dwDstNodeB = INVALID_NODE;
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
    03/08/20    1.0         jianghy       ����
====================================================================*/
void CBoardAgent::InstanceEntry(CMessage* const pcMsg)
{
	Dsclog( "Msg %u(%s) received!\n", pcMsg->event, ::OspEventDesc( pcMsg->event ) );

	if( NULL == pcMsg )
	{
		OspPrintf(TRUE, FALSE, "BoardAgent: The received msg's pointer in the msg entry is NULL!");
		return;
	}

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

    case BRDAGENT_GET_CONFIG_TIMER:     //�ȴ�����Ӧ����Ϣ��ʱ
        ProcGetConfigTimeOut( pcMsg );
		break;

    case MPC_BOARD_SETDSCINFO_REQ:          // ��������Ϣ֪ͨ
        ProcSetDscInfoReq(pcMsg);
        break;

	case MPC_BOARD_DSCGKINFO_UPDATE_CMD:	// DSC��GK info��������, zgc, 2007-07-21
		ProcBoardGkInfoUpdateCmd(pcMsg);
		break;

	case MCU_BOARD_DSCTELNETLOGININFO_UPDATE_CMD:	// DSC�ĸ���LoginInfo����, zgc, 2007-10-12
		ProcBoardLoginInfoUpdateCmd(pcMsg);
		break;

	case BOARD_MODULE_STATUS:
        ProcBoardModuleStatus(pcMsg);
        break;
        
    case BOARD_LED_STATUS:
        ProcBoardLedStatus(pcMsg);
        break;

    case MPC_BOARD_GET_MODULE_REQ:      //��ȡ�����ģ����Ϣ
		ProcGetModuleInfoReq( pcMsg );
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
		ProcRebootCmd( pcMsg );
		break;

	case MPC_BOARD_UPDATE_SOFTWARE_CMD:  //�����������
		ProcUpdateSoftwareCmd( pcMsg );
		break;

	case MPC_DSC_STARTUPDATE_SOFTWARE_REQ:	// ������������zgc, 2007-08-20
		ProcStartDscUpdateSoftwareReq( pcMsg );
		break;

	case MPC_DSC_UPDATEFILE_REQ:			// ���մ��������ļ����ݰ�, zgc, 2007-08-25
		ProcUpdateFileReq(pcMsg);
		break;

	case DSC_UPDATE_SOFTWARE_WAITTIMER:
		ProcUpdateSoftwareWaittimer(pcMsg);
		break;

	case OSP_DISCONNECT:
		ProcOspDisconnect( pcMsg );
		break;

    case BOARD_MPC_ALARM_SYNC_ACK:    
    case BOARD_MPC_LED_STATUS_ACK:
    case BOARD_MPC_GET_VERSION_ACK:
        break;

	default:
        log(LOGLVL_EXCEPTION, "BoardAgent: receive unknown msg %d<%s> in DscAgent InstanceEntry! \n",
                               pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
	
	return;
}

//ģ�ⵥ��BSP�ϵĺ��� ������
//#ifdef WIN32
//void BrdQueryPosition(TBrdPosition* ptPosition)
//{
//	/*�Ѿ��������ļ�����*/
// 	return ;
//}
//#endif

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

	// �ڲ���ʱҪ�����ڵ�
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");
	bResult = GetRegKeyInt( achProfileName, "BoardDebug", "IsTest", 0, &nValue );
	if( bResult == TRUE )  
	{
		m_bIsTest = (nValue != 0);
		if(m_bIsTest)
        {
			OspCreateTcpNode( 0, 4400 );
        }
	}

#ifdef WIN32
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
		OspPrintf( TRUE, TRUE, "[Dri] Wrong profile while reading %s!\n", "MpcIpAddrB" );

	}
    m_dwBoardIpAddr = ntohl( inet_addr( achReturn ) );
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
        dwMcuNode = OspConnectTcpNode( htonl(dwMpcIp), wMpcPort, 10, 3, 100);
        if( !OspIsValidTcpNode(dwMcuNode) )
        {
            bRet = FALSE;
            OspPrintf( TRUE, FALSE, "[Dsc] CreateTcpNode with Manager Failed: dwNode<%d>, MpcIp<0x%x@%d>!\n",
                                     dwMcuNode, dwMpcIp, wMpcPort );
        }
        else
        {
            OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());
            OspSetHBParam(dwMcuNode, g_cBrdAgentApp.m_wDiscHeartBeatTime, g_cBrdAgentApp.m_byDiscHeartBeatNum);
            
            g_cBrdAgentApp.m_dwLocalInnerIp = OspNodeLocalIpGet( dwMcuNode );   // ������

            OspPrintf(TRUE, FALSE, "[Dsc] connect to mcu success, dwMcuNode:%d, Mpc Ip: 0x%x, Local Ip: 0x%x, wDiscHeartBeatTime:%d, byDiscHeartBeatNum:%d\n",
                dwMcuNode, 
                dwMpcIp,
                ntohl(g_cBrdAgentApp.m_dwLocalInnerIp),
                g_cBrdAgentApp.m_wDiscHeartBeatTime, g_cBrdAgentApp.m_byDiscHeartBeatNum);
            

        }
        
    }
	return bRet;
}

/*=============================================================================
  �� �� ���� RegisterToMcuAgent
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::RegisterToMcuAgent(u32 dwDstNode)
{
    CServMsg  cReportMsg;
    Dsclog("[BOARD_MPC_REG]byBrdSlot = %d, byBrdLayer = %d, byBrdID = %d\n",
        m_tBoardPosition.byBrdSlot, m_tBoardPosition.byBrdLayer, m_tBoardPosition.byBrdID);
   
    cReportMsg.SetMsgBody((u8*)&m_tBoardPosition, sizeof(TBrdPosition));
    // guzh [1/17/2007] �йظ�IP��˵��:
    // DSC�� �û����õ�IP�� �� show run �������IP, 
    // �ڲ�IP ͨ�� secondary ��ʽ����,��MCU eth1 ͨ��
    // �����ϱ�����IP,Ӧ����MCU�����ļ�һ��
    cReportMsg.CatMsgBody((u8*)&m_dwBoardIpAddr, sizeof(u32));
    cReportMsg.CatMsgBody(&g_cBrdAgentApp.m_byChoice, sizeof(u8));

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
    07/03/27    4.0         ����        �޸�
====================================================================*/
void CBoardAgent::ProcBoardPowerOn( CMessage* const pcMsg )
{
	Dsclog("BoardAgent: Receive Board Power On Message! \n");

	switch( CurState() ) 
	{
	case STATE_IDLE:
		if(0 != g_cBrdAgentApp.m_dwMpcIpAddr)
        {
            SetTimer(BRDAGENT_CONNECT_MANAGERA_TIMER, POWEN_ON_CONNECT);
        }

        if(0 != g_cBrdAgentApp.m_dwMpcIpAddrB )
        {
            SetTimer(BRDAGENT_CONNECT_MANAGERB_TIMER, POWEN_ON_CONNECT);
        }


        BrdQueryPosition(&m_tBoardPosition);
        m_tBoardPosition.byBrdLayer = 0;
        m_tBoardPosition.byBrdSlot = 0;
#ifdef _VXWORKS_	
        TBrdEthParam tBrdEthParam;
        BrdGetEthParam(g_cBrdAgentApp.m_byChoice, &tBrdEthParam);
        m_dwBoardIpAddr = tBrdEthParam.dwIpAdrs;
        printf("[ProcBoardPowerOn] [in] byChoice :%d,  [out] BrdIpAddr :0x%x\n", g_cBrdAgentApp.m_byChoice,
               m_dwBoardIpAddr);
//#endif
//#ifdef _LINUX_
#else
        TBrdEthParamAll tBrdEthParamAll;
        BrdGetEthParamAll(g_cBrdAgentApp.m_byChoice, &tBrdEthParamAll);
        // guzh [1/17/2007] ��ʱȡ��0������ע��
        m_dwBoardIpAddr = tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs;
        printf("[ProcBoardPowerOn] [in] byChoice :%d,  [out] BrdIpAddr :0x%x\n", g_cBrdAgentApp.m_byChoice,
               m_dwBoardIpAddr);
#endif
//#ifdef WIN32
//        GetBoardCfgInfo();
//#endif
		break;

	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when in (ProcBoardPowerOn)!\n", CurState() );
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
	Dsclog("BoardAgent: Receive Connect Manager Time Out Message! \n");
    BOOL bRet = FALSE;
      
	switch( CurState() ) 
	{
	case STATE_IDLE:
    case STATE_INIT:
    case STATE_NORMAL:
		if(TRUE == bIsConnectA)
        {
            bRet = ConnectManager(m_dwDstNode, 
                    g_cBrdAgentApp.m_dwMpcIpAddr, g_cBrdAgentApp.m_wMpcPort);       //��MPC�ϵĹ����������
            if( TRUE == bRet)
            {
                KillTimer(BRDAGENT_CONNECT_MANAGERA_TIMER);
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
                    g_cBrdAgentApp.m_dwMpcIpAddrB, g_cBrdAgentApp.m_wMpcPortB);       //��MPC�ϵĹ����������
            if( TRUE == bRet)
            {
                KillTimer(BRDAGENT_CONNECT_MANAGERB_TIMER);
                SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT );
                g_cBrdAgentApp.m_dwDstMcuNodeB = m_dwDstNodeB;
            }
            else
            {
                SetTimer( BRDAGENT_CONNECT_MANAGERB_TIMER, CONNECT_MANAGER_TIMEOUT );
            }
        }

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
	
	Dsclog("[Dsc] Receive Register Manager Time Out Message! \n");

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
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when in (ProcBoardRegisterTimeOut)!\n", CurState() );
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
    case STATE_NORMAL:
		log( LOGLVL_EXCEPTION, "BoardAgent: Receive Register NAck Message!  \n");
		break;

	default:
		log( LOGLVL_EXCEPTION, "Wrong state %u when in (ProcBoardRegNAck)!\n", CurState() );
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

	Dsclog("[Dsc] Receive Register Ack Message! \n");

	switch( CurState() ) 
	{
	case STATE_INIT:
    case STATE_NORMAL:
        MsgRegAck(pcMsg);
		break;

	default:
		OspPrintf(TRUE, FALSE, "[Dsc] Wrong state %u when in (ProcBoardRegAck)!\n", CurState() );
		break;
	}
	return;

}

/*=============================================================================
  �� �� ���� MsgRegAck
  ��    �ܣ� ����ע��Ack
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::MsgRegAck(CMessage* const pcMsg )
{
    if(NULL == pcMsg)
    {
        OspPrintf(TRUE, FALSE, "[Dsc] the pointer can not be Null\n");
        return;
    }

    if(pcMsg->srcnode == m_dwDstNode)
    {
        m_dwDstIId = pcMsg->srcid;
        KillTimer(BRDAGENT_REGISTERA_TIMER);
    }
    else if(pcMsg->srcnode == m_dwDstNodeB)
    {
        m_dwDstIIdB = pcMsg->srcid;
        KillTimer(BRDAGENT_REGISTERB_TIMER);
    }

        
    if(STATE_INIT == CurState()) // ֻ��INIT״̬�²�ȡ����
    {
        PostMsgToManager( BOARD_MPC_GET_CONFIG, 
            (u8*)&m_tBoardPosition, sizeof(m_tBoardPosition) );		
        SetTimer( BRDAGENT_GET_CONFIG_TIMER, GET_CONFIG_TIMEOUT );
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
void CBoardAgent::ProcGetConfigTimeOut( CMessage* const pcMsg )
{

	Dsclog("BoardAgent: Receive Get Config Time Out Message! \n");

	switch( CurState() ) 
	{
	case STATE_INIT:
		// ���·���ȡ������Ϣ��Ϣ
		PostMsgToManager( BOARD_MPC_GET_CONFIG, (u8*)&m_tBoardPosition, sizeof(m_tBoardPosition) );		
		SetTimer( BRDAGENT_GET_CONFIG_TIMER, GET_CONFIG_TIMEOUT );	
		break;

	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when in (ProcGetConfigTimeOut)!\n", CurState() );
		break;
	}
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
		OspPrintf(TRUE, FALSE, "[Dsc] Receive Get Config NAck Message! \n");
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "[Dsc] Wrong state %u! (ProcBoardGetConfigNAck)\n", CurState() );
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
    if(NULL == pcMsg)
    {
        OspPrintf(TRUE, FALSE, "[Dsc] the pointer can not be Null\n");
        return;
    }

    Dsclog("Receive Get Config Ack Message! \n");

	switch( CurState() ) 
	{
	case STATE_INIT:
        MsgGetConfAck(pcMsg);
        break;
        
	default:
		OspPrintf(TRUE, FALSE, "Wrong state %u! (ProcBoardGetConfigAck)\n", CurState() );
		break;
	}
	return;
}

/*=============================================================================
  �� �� ���� MsgGetConfAck
  ��    �ܣ� ����ȡ����Ack��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::MsgGetConfAck(CMessage* const pcMsg)
{    
    KillTimer( BRDAGENT_GET_CONFIG_TIMER ); 
    
    /*
    // ������Ϣ�峤�ȱ���, ��ֹ�����쳣, zgc, 2008-07-15
    // ��������(u8)+��������(u8)+PRS������Ϣ(TEqpPrsEntry)+��ͬ����Ϣ(2*u8)
    // +����IP(u32)+���Ź���־(u8)+TDSCModuleInfo+GKIP(u32)+TLoginInfo
    // FIXME:����û�жԶ�����������д���
    u16 dwLocalMsgLength = ( sizeof(u8)+sizeof(u8)+sizeof(TEqpPrsEntry)+2*sizeof(u8)
        +sizeof(u32)+sizeof(u8)+sizeof(TDSCModuleInfo)+sizeof(u32)
        +sizeof(TLoginInfo) );
    if ( pcMsg->length != dwLocalMsgLength )
    {
        OspPrintf( TRUE, FALSE, "[MsgGetConfAck] Config msg body lenth(%d) is error! Should be %d!\n",
            pcMsg->length, dwLocalMsgLength );
        OspDisconnectTcpNode(m_dwDstNode);
        SetTimer( BRDAGENT_CONNECT_MANAGERA_TIMER, CONNECT_MANAGER_TIMEOUT );
        return;
    }
    */
    
	TEqpPrsEntry*	ptPrsCfg = NULL;	// Prs��������Ϣ
    u16	wIndex = 0;
    u16 byPeriType = 0;
    u8 byPeriCount = 0;
	u32 dwGKIp = 0;
    u16 wLoop = 0;
    BOOL32 bCfgModified = FALSE;
    u16 wLocalMsgLength = 0;
	CServMsg cServMsg;

	TDSCModuleInfo tOtherModuleInfo, tCurModuleInfo;
	TMINIMCUNetParamAll tNewParamAll, tCurParamAll;

	TLoginInfo tLoginInfo;
#ifdef _LINUX_
	TUserType emUserType;
#endif

	u32 dwGKIpAddr = 0;

    switch(CurState())
    {
    case STATE_INIT:
        byPeriCount = pcMsg->content[wIndex]; // �������
        wIndex ++;

        // ������Ϣ�峤�ȱ���, ��ֹ�����쳣, zgc, 2008-07-15
        // �������е����ݲ�һ�����ڣ���Ҫ�ж��Ƿ�������, zgc, 2008-10-09
        // ��������(u8)[+��������(u8)+PRS������Ϣ(TEqpPrsEntry)]+��ͬ����Ϣ(2*u8)
        // +����IP(u32)+���Ź���־(u8)+TDSCModuleInfo+GKIP(u32)+TLoginInfo
        // FIXME:����û�жԶ�����������д���
        if ( byPeriCount == 0 )
        { 
            wLocalMsgLength = ( sizeof(u8)+2*sizeof(u8)+sizeof(u32)+sizeof(u8)
                        +sizeof(TDSCModuleInfo)+sizeof(u32)
                        +sizeof(TLoginInfo) );
        }
        else
        {
            wLocalMsgLength = ( sizeof(u8)+sizeof(u8)+sizeof(TEqpPrsEntry)+2*sizeof(u8)
                        +sizeof(u32)+sizeof(u8)+sizeof(TDSCModuleInfo)+sizeof(u32)
                        +sizeof(TLoginInfo) );
        }
         
        if ( pcMsg->length != wLocalMsgLength )
        {
            OspPrintf( TRUE, FALSE, "[MsgGetConfAck] Config msg body lenth(%d) is error! Should be %d!\n",
                pcMsg->length, wLocalMsgLength );
            OspDisconnectTcpNode(m_dwDstNode);
            SetTimer( BRDAGENT_CONNECT_MANAGERA_TIMER, CONNECT_MANAGER_TIMEOUT );
            return;
        }

	    for( wLoop = 0; wLoop < byPeriCount; wLoop++ )
	    {			
		    byPeriType = pcMsg->content[wIndex++];  //  ��������
		    
            switch( byPeriType )	//  ���øõ�����Ӧ��������Ϣ
		    {
		    case EQP_TYPE_PRS:
			    ptPrsCfg = (TEqpPrsEntry*)(pcMsg->content+wIndex);
                ptPrsCfg->SetMcuIp(g_cBrdAgentApp.m_dwMpcIpAddr);
			    wIndex += sizeof(TEqpPrsEntry);
			    g_cBrdAgentApp.SetPrsConfig(ptPrsCfg);
			    break;

		    default:
			    break;
		    }
	    }
        // ������ͬ����Ϣ
        wIndex += 2*sizeof(u8);

	    // ����IP        
        printf("BoardAgent: g_cBrdAgentApp.SetBrdIpAddr(0x%x)-wIndex = %d\n",
                                 ntohl(*(u32*)&pcMsg->content[wIndex]), wIndex);        
    
	    g_cBrdAgentApp.SetBrdIpAddr( ntohl(  *(u32*)&pcMsg->content[wIndex] ) );
		
        wIndex += sizeof(u32);
		    
	    if( pcMsg->content[wIndex] == 0 )  // �Ƿ�ʹ�ÿ��Ź�
	    {
    #ifdef _VXWORKS_
		    SysRebootDisable( );
    #endif
    #ifdef _LINUX_
            SysRebootDisable();
    #endif
	    }
	    else
	    {
    #ifdef _VXWORKS_
		    SysRebootEnable( );
    #endif
    #ifdef _LINUX_
            SysRebootEnable();
    #endif
	    }

    #ifdef _VXWORKS_			
	    BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON ); // ��MLINK�Ƶ���
    #endif
    #ifdef  _LINUX_
        // BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON );
    #endif

        wIndex ++;
		
        // guzh [1/12/2007] ����������ģ�����Ϣ        
        memcpy(&tOtherModuleInfo, &(pcMsg->content[wIndex]), sizeof(TDSCModuleInfo) );

        wIndex += sizeof(TDSCModuleInfo);

		// ������DSCinfo, zgc, 2007-07-20	
		tOtherModuleInfo.GetCallAddrAll( tNewParamAll );

		if ( tNewParamAll.IsValid() )
		{
			Dsclog("[MsgGetConfAck] Save dscinfo from mcu to local!\n");
			g_cBrdAgentApp.SaveDscLocalInfoToNip( &tOtherModuleInfo );
			g_cBrdAgentApp.SaveDefaultGWToCfg( tNewParamAll );
		}

		// ȡGKIP��zgc, 2007-08-15
		dwGKIp = ntohl(  *(u32*)&pcMsg->content[wIndex] );
		g_cBrdAgentApp.SetGKIpAddr( dwGKIp );
		if ( !g_cBrdAgentApp.SaveGKInfoToCfg( g_cBrdAgentApp.GetGKIpAddr(), MCU_RAS_PORT ) ) // ����д��RAS�˿ں�Ϊ1719, zgc, 2007-07-21
		{
			OspPrintf(TRUE, FALSE, "[MsgGetConfAck] Save GK to cfg file error!\n");
		}
		wIndex += sizeof(u32);

		tLoginInfo = *(TLoginInfo*)&pcMsg->content[wIndex];
		wIndex += sizeof(TLoginInfo);
		// ȡ����LoginInfo���ȴ�NIP�ӿ�
		#ifndef WIN32
		#endif
		
		// ������, zgc, 2007-10-25
		if (g_bPrintDscLog)
		{
			OspPrintf(TRUE, FALSE, "[GetCfgAck] Msg len = %d\n", wIndex+1 );
			for ( s32 n = 0; n < wIndex+1; n++ )
			{
				OspPrintf(TRUE, FALSE, "%x ", pcMsg->content[n] );
				if ( (n+1)%20 == 0 )
				{
					OspPrintf(TRUE, FALSE, "\n" );
				}
			}
		}
		
		/*
		emUserType = USER_TYPE_TELNET_SUPER;
		if ( ERROR == BrdAddUser( (s8*)tLoginInfo.GetUser(), (s8*)tLoginInfo.GetPwd(), emUserType ) )
		{
			Dsclog( "[LoginInfoUpdate] Set login info failed!\n " );
		}
		*/
		
		bCfgModified = g_cBrdAgentApp.SetDscRunningModule( tOtherModuleInfo, FALSE );

//		bCfgModified |= g_cBrdAgentApp.WriteConnectMcuInfo();


		// �޸�mtadpport
		if ( tOtherModuleInfo.IsStartGk() )
		{
			//�ָ�ʹ��1720�˿�, zgc, 2007-10-11
			g_cBrdAgentApp.ModifyMtadpPort( RASPORT_GK, Q931PORT_NOGK );
		}
		else
		{
			g_cBrdAgentApp.ModifyMtadpPort( RASPORT_NOGK, Q931PORT_NOGK );
		}

		// �޸�DCS config, zgc, 2007-09-24
		if ( tOtherModuleInfo.IsStartDcs() )
		{
			TMINIMCUNetParam tNetParam;
			tNewParamAll.GetNetParambyIdx( 0, tNetParam );
			g_cBrdAgentApp.ModifyDcsConfig( tNetParam.GetIpAddr(), ntohl(g_cBrdAgentApp.GetConnectMcuIpAddr()), dwGKIp );
		}

		g_cBrdAgentApp.SetLastRunningModuleInfo( tOtherModuleInfo );
    
	    OspSemGive( g_semDSC );
		   	    
	    NEXTSTATE( STATE_NORMAL );

		printf("Current state.%d\n", CurState());
		
        if (bCfgModified)
        {
    #ifdef _LINUX_        
            OspDelay(1000);
            BrdHwReset();
    #endif
        }

        break;
    default:
        printf("[Dsc] Wrong state: %d when in MsgGetConfAck\n", CurState());
        break;
    }

	return;
}

/*====================================================================
    ������      ��ProcSetDscInfoReq
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/01/24    4.0         ����        ����
====================================================================*/
void CBoardAgent::ProcSetDscInfoReq( CMessage* const pcMsg )
{
	if(NULL == pcMsg)
    {
        OspPrintf(TRUE, FALSE, "[DSC][ProcSetDscInfoReq]The pointer can not be Null\n");
        return;
    }

    if (CurState() != STATE_NORMAL)
    {
        OspPrintf(TRUE, FALSE, "Wrong state %u! (ProcSetDscInfoReq)\n", CurState() );
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TDSCModuleInfo tNewInfo = *(TDSCModuleInfo*)(cServMsg.GetMsgBody());
    if (g_bPrintDscLog)
    {
        tNewInfo.Print();
    }
	TDSCModuleInfo tOldInfo = g_cBrdAgentApp.GetRunningModuleInfo();
	if (g_bPrintDscLog)
    {
        tOldInfo.Print();
    }
	
	u8 byCallAddrNum = tNewInfo.GetCallAddrNum();
    // ��Ҫ����1����ַ
	if ( 0 == byCallAddrNum || ETH_IP_MAXNUM-1 <= byCallAddrNum )
	{
		OspPrintf(TRUE, FALSE, "[ProcSetDscInfoReq] The call addr num.%d error!\n", byCallAddrNum );
		PostMsgToManager( BOARD_MPC_SETDSCINFO_NACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		return;
	}

	TMINIMCUNetParamAll tNewParamAll;
	tNewInfo.GetCallAddrAll( tNewParamAll );
	TMINIMCUNetParamAll tOldParamAll;
	tOldInfo.GetCallAddrAll( tOldParamAll );

	BOOL32 bCfgModified = FALSE;
	TMINIMCUNetParam tNetParam;
	tNewParamAll.GetNetParambyIdx( 0, tNetParam );
//#ifdef _LINUX_
	TBrdEthParam tEthParam;
	BrdGetEthParam( 0, &tEthParam );
	// ����׵�ַ�ı䣬��Ҫ����, zgc, 2007-09-24
	bCfgModified |= ( ntohl(tEthParam.dwIpAdrs) != tNetParam.GetIpAddr() ) ? TRUE : FALSE;
//#endif
	// ��m_dwBoardIpAddr��ͬ����Ҫ����, zgc, 2007-09-24
	bCfgModified |= ( ntohl(m_dwBoardIpAddr) != tNetParam.GetIpAddr() ) ? TRUE : FALSE;
	

	BOOL32 bSaveCallAddr = TRUE;
	if ( !tOldParamAll.IsEqualTo( tNewParamAll ) )	
	{
		bSaveCallAddr &= g_cBrdAgentApp.SaveDscLocalInfoToNip( &tNewInfo );
		bSaveCallAddr &= g_cBrdAgentApp.SaveDefaultGWToCfg( tNewParamAll );
		if ( !bSaveCallAddr )
		{
			OspPrintf( TRUE, FALSE, "[ProcSetDscInfoReq] Set new dscinfo error! Roll back!\n");
			bSaveCallAddr = TRUE;
			bSaveCallAddr &= g_cBrdAgentApp.SaveDscLocalInfoToNip( &tOldInfo );
			bSaveCallAddr &= g_cBrdAgentApp.SaveDefaultGWToCfg( tOldParamAll );
			if ( !bSaveCallAddr )
			{
				OspPrintf( TRUE, FALSE, "[ProcSetDscInfoReq] Roll back ERROR!\n");
			}
			PostMsgToManager( BOARD_MPC_SETDSCINFO_NACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			return;
		}
	}

	PostMsgToManager( BOARD_MPC_SETDSCINFO_ACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

	bCfgModified |= g_cBrdAgentApp.SetDscRunningModule( tNewInfo );
//	bCfgModified |= g_cBrdAgentApp.WriteConnectMcuInfo();	
	g_cBrdAgentApp.SetLastRunningModuleInfo( tNewInfo );

	// �޸�mtadpport
	if ( tNewInfo.IsStartGk() )
	{
		//�ָ�ʹ��1720�˿�, zgc, 2007-10-11
		g_cBrdAgentApp.ModifyMtadpPort( RASPORT_GK, Q931PORT_NOGK );
	}
	else
	{
		g_cBrdAgentApp.ModifyMtadpPort( RASPORT_NOGK, Q931PORT_NOGK );
	}
	
	// �޸�DCS config
	if ( tNewInfo.IsStartDcs() )
	{
		TMINIMCUNetParam tNetParam;
		tNewParamAll.GetNetParambyIdx( 0, tNetParam );
		g_cBrdAgentApp.ModifyDcsConfig( tNetParam.GetIpAddr() );
	}

	 if (bCfgModified)
	{
	#ifdef _LINUX_        
		OspDelay(1000);
		BrdHwReset();
	#endif
	}

}

/*=============================================================================
�� �� ���� ProcBoardGkInfoUpdateCmd
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage* const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/21   4.0			�ܹ��                  ����
=============================================================================*/
void CBoardAgent::ProcBoardGkInfoUpdateCmd( CMessage* const pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	u32 dwGKIp = *(u32*)(cServMsg.GetMsgBody() + sizeof(u8)*2);
	dwGKIp = ntohl(dwGKIp);
	g_cBrdAgentApp.SetGKIpAddr( dwGKIp );
	if ( !g_cBrdAgentApp.SaveGKInfoToCfg( g_cBrdAgentApp.GetGKIpAddr(), MCU_RAS_PORT ) ) // ����д��RAS�˿ں�Ϊ1719, zgc, 2007-07-21
	{
		OspPrintf(TRUE, FALSE, "[ProcBoardGkInfoUpdateCmd] Save GK to cfg file error!\n");
	}
	return;
}

/*=============================================================================
�� �� ���� ProcBoardGkInfoUpdateCmd
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage* const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/21   4.0			�ܹ��                  ����
=============================================================================*/
void CBoardAgent::ProcBoardLoginInfoUpdateCmd( CMessage* const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TLoginInfo tLoginInfo = *(TLoginInfo*)( cServMsg.GetMsgBody() + sizeof(u8)*2 );

	s8 achPwd[MAXLEN_PWD+1];
	memset(achPwd, 0, sizeof(achPwd));
	tLoginInfo.GetPwd(achPwd, sizeof(achPwd));
	if ( NULL == tLoginInfo.GetUser() || 0 == strlen(achPwd) )
	{
		printf( "[ProcBoardLoginInfoUpdateCmd] Login name or password is null! Error!\n" );
		return ;
	}
	
#ifdef _LINUX_
	TUserType emUserType = USER_TYPE_TELNET_SUPER;
	STATUS wRet = BrdAddUser( (s8*)tLoginInfo.GetUser(), achPwd, emUserType );
	if ( ERROR == wRet )
	{
		Dsclog( "[LoginInfoUpdate] Set login info failed!\n " );
	}
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
	if( OspIsValidTcpNode( m_dwDstNode ) )
	{
		Dsclog("[PostMsgToManager] MSG<%s> send!\n", ::OspEventDesc(wEvent) );
		post( m_dwDstIId, wEvent, pbyContent, wLen, m_dwDstNode );
	}
	else
	{
		Dsclog("[Dsc] the A's node is invalid\n");
        Dsclog("Board [%d,%d,%d] is offline now: Node=%u\n", 
			m_tBoardPosition.byBrdID, m_tBoardPosition.byBrdLayer, 
            m_tBoardPosition.byBrdSlot, m_dwDstNode);
	}
    
    if(OspIsValidTcpNode(m_dwDstNodeB))
    {
        post(m_dwDstIIdB, wEvent, pbyContent, wLen, m_dwDstNodeB);
    }
    else
    {
        Dsclog("[Dsc] the B's node is invalid\n");
        Dsclog("BoardAgent: Board [%d,%d,%d] is offline now: Node=%u\n", 
			m_tBoardPosition.byBrdID, m_tBoardPosition.byBrdLayer, 
            m_tBoardPosition.byBrdSlot, m_dwDstNodeB);
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
====================================================================*/
void CBoardAgent::ProcOspDisconnect( CMessage* const pcMsg )
{

	Dsclog("BoardAgent: Receive Osp Disconnect Message! \n");

    u32 dwNode = 0;
	switch( CurState() ) 
	{
    case STATE_IDLE:
	case STATE_INIT:
	case STATE_NORMAL:
        MsgDisconnectInfo(pcMsg);
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u! (ProcOspDisconnect)\n", CurState() );
		break;
	}
	return;
}

/*=============================================================================
  �� �� ���� MsgDisconnectInfo
  ��    �ܣ� ���������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::MsgDisconnectInfo(CMessage* const pcMsg)
{
    if(NULL == pcMsg)
    {
        OspPrintf(TRUE, FALSE, "[Dsc] the pointer can not be Null.(MsgDisconnectInfo)\n");
        return;
    }

    u16 wRet = 0;

    u32 dwNode = *(u32*)pcMsg->content;
    if ( INVALID_NODE != dwNode )
    {
        OspDisconnectTcpNode(dwNode);
    }

    if(dwNode == m_dwDstNode)
    {
        FreeDataA();
        g_cBrdAgentApp.m_dwDstMcuNode = INVALID_NODE;
        SetTimer( BRDAGENT_CONNECT_MANAGERA_TIMER, CONNECT_MANAGER_TIMEOUT );
    }
    else if(dwNode == m_dwDstNodeB)
    {
        FreeDataB();
        g_cBrdAgentApp.m_dwDstMcuNodeB = INVALID_NODE;
        SetTimer( BRDAGENT_CONNECT_MANAGERB_TIMER, CONNECT_MANAGER_TIMEOUT );
        wRet = DISCONNECT_MCUB;
    }

    if(INVALID_NODE == m_dwDstNode && INVALID_NODE == m_dwDstNodeB)
    {

    #ifdef _VXWORKS_
		BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF );
    #endif

    #ifdef _LINUX_
        /* add the linux interface */
        BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF );
    #endif
        NEXTSTATE( STATE_IDLE );
        OspDelay(5000);		
    }
    return;
}
/*=============================================================================
  �� �� ���� ProcBoardModuleStatus
  ��    �ܣ� ����ģ��澯
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::ProcBoardModuleStatus( CMessage* const pcMsg )
{
    return;
}

/*=============================================================================
  �� �� ���� ProcBoardLedStatus
  ��    �ܣ� ����Ƹ澯
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::ProcBoardLedStatus( CMessage* const pcMsg )
{
    return;
}

/*=============================================================================
  �� �� ���� ProcGetModuleInfoReq
  ��    �ܣ� ȡ����ģ����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::ProcGetModuleInfoReq( CMessage* const pcMsg )
{
    switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:		
		// ���͸�MANAGER��Ӧ
		PostMsgToManager( BOARD_MPC_GET_MODULE_ACK, (u8*)&m_tBoardPosition, sizeof(m_tBoardPosition) );
		break;
		
	default:
		log( LOGLVL_EXCEPTION, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*=============================================================================
  �� �� ���� ProcRebootCmd
  ��    �ܣ� reboot 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::ProcRebootCmd(CMessage* const pcMsg)
{	
	OspPrintf(TRUE, FALSE, "[Dsc] Receive reboot command.\n");
#ifndef WIN32	
	switch( CurState() )
	{
	case STATE_INIT:
	case STATE_NORMAL:
		
        OspDelay(2000);		
        BrdHwReset();
	
		break;
	default:
		break;
	}
#endif
	return;
}
/*=============================================================================
  �� �� ���� ProcBoardSelfTestCmd
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::ProcBoardSelfTestCmd(CMessage* const pcMsg)
{
	return;
}
/*=============================================================================
  �� �� ���� ProcBitErrorTestCmd
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::ProcBitErrorTestCmd(CMessage* const pcMsg )
{
	return;
}
/*=============================================================================
  �� �� ���� ProcTimeSyncCmd
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::ProcTimeSyncCmd(CMessage* const pcMsg )
{
#ifndef WIN32	    
	s8 achTemp[50];
    struct tm tmTime;

    switch( CurState() ) 
    {
    case STATE_INIT:
    case STATE_NORMAL:			
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
        if( ERROR == BrdTimeSet( &tmTime ) )
        {
            printf( "BrdTimeSet failed!\n" );
            return;
        }
	}
#endif
}
/*=============================================================================
  �� �� ���� ProcUpdateSoftwareCmd
  ��    �ܣ� �������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::ProcUpdateSoftwareCmd(CMessage* const pcMsg )
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
                    OspPrintf(TRUE, FALSE, "[ProcUpdateSoftwareCmd] invalid msgbody!!!\n");
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
                        OspPrintf(TRUE, FALSE, "[ProcUpdateBrdVersion] BrdUpdateAppImage failed.\n");
                    }
                    else
                    {
                        OspPrintf(TRUE, FALSE, "[ProcUpdateBrdVersion] BrdUpdateAppImage success.\n");
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
                            OspPrintf(TRUE, FALSE, "[ProcUpdateBrdVersion] BrdFpUpdateAuxData failed.\n");
                        }
                        else
                        {
                            OspPrintf(TRUE, FALSE, "[ProcUpdateBrdVersion] BrdFpUpdateAuxData success.\n");
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
		log( LOGLVL_EXCEPTION, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			 pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*=============================================================================
�� �� ���� ProcStartDscUpdateSoftwareReq
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage* const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/20   4.0			�ܹ��                  ����
=============================================================================*/
void CBoardAgent::ProcStartDscUpdateSoftwareReq( CMessage* const pcMsg )
{
	if ( pcMsg == NULL )
	{
		Dsclog( "[ProcStartDscUpdateSoftwareReq] The msg is NULL! Error!\n" );
		return;
	}

	CServMsg cMsg( pcMsg->content, pcMsg->length );
	if ( m_byMcsSsnIdx == 0 )
	{
		m_byBrdIdx = *cMsg.GetMsgBody();
		m_byMcsSsnIdx = *( cMsg.GetMsgBody()+sizeof(u8) );
		m_byUpdateFileNum = *( cMsg.GetMsgBody()+sizeof(u8)+sizeof(u8) );
		Dsclog("[ProcStartDscUpdateSoftwareReq] BrdIdx = %d, McsSsnIdx = %d, UpdateFileNum = %d\n",
			m_byBrdIdx, m_byMcsSsnIdx, m_byUpdateFileNum);
	}
	else
	{
		Dsclog("[ProcStartDscUpdateSoftwareReq] Other Mcs<McsSsn.%d> is updating now!\n", m_byMcsSsnIdx);
		PostMsgToManager(DSC_MPC_STARTUPDATE_SOFTWARE_NACK, pcMsg->content, pcMsg->length);
		return;
	}
	
	u8 *pMsgBuf = cMsg.GetMsgBody() + sizeof(u8)*3;
	u8 byFileNameLen = 0;
	memset( m_achCurUpdateFileName, 0, sizeof(m_achCurUpdateFileName) );
	memset(m_abyUpdateResult, 0, sizeof(m_abyUpdateResult));
	for ( u8 byLop = 0; byLop < m_byUpdateFileNum; byLop++ )
	{
		byFileNameLen = *pMsgBuf;
		pMsgBuf++;
		strncpy( m_achCurUpdateFileName[byLop], (s8*)pMsgBuf, byFileNameLen );
		m_achCurUpdateFileName[byLop][MAXLEN_MCU_FILEPATH-1] = '\0';
		pMsgBuf += byFileNameLen;
	}

	if ( !g_bUpdateSoftBoard )
	{
		PostMsgToManager(DSC_MPC_STARTUPDATE_SOFTWARE_NACK, pcMsg->content, pcMsg->length);
	}
	else
	{
		PostMsgToManager(DSC_MPC_STARTUPDATE_SOFTWARE_ACK, pcMsg->content, pcMsg->length);
		SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 1000);
	}

	return;
}

/*=============================================================================
�� �� ���� ProcUpdateFileReq
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage* const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/25   4.0			�ܹ��                  ����
=============================================================================*/
void CBoardAgent::ProcUpdateFileReq( CMessage* const pcMsg )
{
	if ( pcMsg == NULL )
	{
		Dsclog( "[ProcUpdateFileReq] The msg is NULL! Error!\n" );
		return;
	}

//	FILE *hFile = NULL;

	KillTimer(DSC_UPDATE_SOFTWARE_WAITTIMER);
	m_byWaitTimes = 0;

	CServMsg cMsg( pcMsg->content, pcMsg->length );
	u8 byBrdIdx = *( cMsg.GetMsgBody() );
	u8 byMcsSsnIdx = *( cMsg.GetMsgBody()+sizeof(u8) );
	TDscUpdateReqHead tDscUpdateReqHead = *(TDscUpdateReqHead*)( cMsg.GetMsgBody() + 2*sizeof(u8) );
	if ( strlen(m_tCurUpdateReq.m_acFileName) == 0 )
	{
		strncpy( m_tCurUpdateReq.m_acFileName, tDscUpdateReqHead.m_acFileName, MAXLEN_MCU_FILEPATH-1);
		m_tCurUpdateReq.m_acFileName[MAXLEN_MCU_FILEPATH-1] = 0;
	}
	//����뵱ǰ������ļ���֡��һ�£���ֱ�Ӷ������ط�����
	if ( (strcmp(tDscUpdateReqHead.m_acFileName, m_tCurUpdateReq.m_acFileName ) != 0
		|| tDscUpdateReqHead.GetReqFrmSN() != m_tCurUpdateReq.GetRspFrmSN()
		|| m_byBrdIdx != byBrdIdx
		|| m_byMcsSsnIdx != byMcsSsnIdx)
		&& 
		m_byErrTimes <= 3 )
	{
		Dsclog("[ProcUpdateFileReq] Req update pack is not according with waitting pack! NACK!\n");
		Dsclog("ReqFile : %s, WaitFile : %s\n",tDscUpdateReqHead.m_acFileName, m_tCurUpdateReq.m_acFileName);
		Dsclog("ReqFrmSN : %d, WaitFrmSN : %d\n", tDscUpdateReqHead.GetReqFrmSN(), m_tCurUpdateReq.GetRspFrmSN());
		Dsclog("ReqBrdIdx : %d, WaitBrdIdx : %d\n", byBrdIdx, m_byBrdIdx);
		Dsclog("ReqMcsSsnIdx : %d, WaitMcsSsnIdx : %d\n", byMcsSsnIdx, m_byMcsSsnIdx);
		NackUpdateFileReq();
		SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
		return;
	}
	else
	{
		if ( tDscUpdateReqHead.GetReqFrmSN() == 0 )
		{
			Dsclog("[ProcUpdateFileReq]ReqFileSize: %d\n", tDscUpdateReqHead.GetFileSize());
			// ���뻺��, zgc, 2007-10-18
			if ( NULL == m_pbyFileBuf )
			{
				m_pbyFileBuf = new u8[UPDATE_FILEBUF_SIZE];
				if ( NULL == m_pbyFileBuf )
				{
					Dsclog( "[ProcUpdateFileReq] Alloc mem failed! NACK!\n" );
					NackUpdateFileReq();
					SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
					return;
				}
				memset( m_pbyFileBuf, 0, UPDATE_FILEBUF_SIZE );
				m_dwBufUsedSize = 0;
			}
		}
		Dsclog("ReqFrmSize: %d,\tReqFrmSN: %d,\tMark: %d\n", tDscUpdateReqHead.GetFrmSize(), tDscUpdateReqHead.GetReqFrmSN(), tDscUpdateReqHead.m_byMark);
	}

	if ( m_byErrTimes > 3)
	{
		if ( strlen(m_tCurUpdateReq.m_acFileName) != 0 )
		{
		//	FILE *hFile = fopen( m_tCurUpdateReq.m_acFileName, "r" );
			if ( NULL == m_hUpdateFile )
			{
				m_hUpdateFile = fopen( m_tCurUpdateReq.m_acFileName, "r" );
			}
			//if ( hFile != NULL )
			if ( m_hUpdateFile != NULL )
			{
				//fclose(hFile);
				fclose(m_hUpdateFile);
				remove(m_tCurUpdateReq.m_acFileName);
			}
		}
		memset( &m_tCurUpdateReq, 0, sizeof(m_tCurUpdateReq) );
		NackUpdateFileReq();
		m_byErrTimes = 0;
		m_byMcsSsnIdx = 0;
		m_byBrdIdx = 0;
		m_byUpdateFileNum = 0;
		if ( NULL != m_hUpdateFile )
		{
			fclose( m_hUpdateFile );
			m_hUpdateFile = NULL;
		}
		if ( NULL != m_pbyFileBuf )
		{
			delete [] m_pbyFileBuf;
			m_pbyFileBuf = NULL;
		}
		m_dwBufUsedSize = 0;
		return;
	}

	// ���ԭ���ļ��򴴽��¿��ļ�
	m_byErrTimes = 0;
	if ( tDscUpdateReqHead.GetReqFrmSN() == 0 )
	{
		//hFile = fopen( tDscUpdateReqHead.m_acFileName, "w" );
		if ( NULL != m_hUpdateFile )
		{
			fclose(m_hUpdateFile);
			m_hUpdateFile = NULL;
		}
		m_hUpdateFile = fopen( tDscUpdateReqHead.m_acFileName, "w" );
		//if ( hFile == NULL )
		if ( NULL == m_hUpdateFile )
		{
			Dsclog( "[ProcUpdateFileReq] Open or creat file<%s> failed!\n", tDscUpdateReqHead.m_acFileName );
		//	memset( &m_tCurUpdateReq, 0, sizeof(m_tCurUpdateReq) );
			NackUpdateFileReq();
//			m_byMcsSsnIdx = 0;
//			m_byBrdIdx = 0;
//			m_byUpdateFileNum = 0;
			SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
			return;
		}
		//fclose( hFile );
		fclose( m_hUpdateFile );
		m_hUpdateFile = NULL;
	}

	//hFile = fopen( tDscUpdateReqHead.m_acFileName, "ab" );
	if ( NULL == m_hUpdateFile )
	{
		m_hUpdateFile = fopen( tDscUpdateReqHead.m_acFileName, "ab" );
		if ( NULL == m_hUpdateFile )
		{
			Dsclog( "[ProcUpdateFileReq] Open file<%s> failed! NACK!\n", tDscUpdateReqHead.m_acFileName );
			NackUpdateFileReq();
			SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
			return;
		}
	}
	u8 *pFileContent = cMsg.GetMsgBody() + 2*sizeof(u8) + sizeof(TDscUpdateReqHead);
	u32 dwFileContentLen = cMsg.GetMsgBodyLen() - 2*sizeof(u8) - sizeof(TDscUpdateReqHead);
	//fwrite( pFileContent, sizeof(u8), dwFileContentLen, hFile );
	//fclose( hFile );
	// ��д���棬д������д���ļ��У�zgc, 2007-10-18
	if ( m_dwBufUsedSize + dwFileContentLen > UPDATE_FILEBUF_SIZE )
	{
		if ( m_dwBufUsedSize == fwrite( m_pbyFileBuf, sizeof(u8), m_dwBufUsedSize, m_hUpdateFile ) )
		{
			memset( m_pbyFileBuf, 0, UPDATE_FILEBUF_SIZE );
			m_dwBufUsedSize = 0;
		}
		else
		{
			Dsclog("[ProcUpdateFileReq] Write File<%s> failed! NACK!\n", tDscUpdateReqHead.m_acFileName );
			NackUpdateFileReq();
			SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
			return;
		}
	}
	memcpy( m_pbyFileBuf+m_dwBufUsedSize, pFileContent, dwFileContentLen );
	m_dwBufUsedSize += dwFileContentLen;

	if ( tDscUpdateReqHead.m_byMark == 0 )
	{
		// ������һ֡
		u32 dwRspFrmSN = m_tCurUpdateReq.GetRspFrmSN() + 1;
		m_tCurUpdateReq.SetRspFrmSN(dwRspFrmSN);
		cMsg.SetMsgBody( (u8*)&byBrdIdx, sizeof(byBrdIdx) );
		cMsg.CatMsgBody( (u8*)&byMcsSsnIdx, sizeof(byMcsSsnIdx) );
		cMsg.CatMsgBody( (u8*)&m_tCurUpdateReq, sizeof(m_tCurUpdateReq) );
		PostMsgToManager( DSC_MPC_UPDATEFILE_ACK, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
		SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
	}
	else
	{
		// ���һ����д��, zgc, 2008-03-18
		if ( 0 != m_dwBufUsedSize )
		{
			if ( m_dwBufUsedSize == fwrite( m_pbyFileBuf, sizeof(u8), m_dwBufUsedSize, m_hUpdateFile ) )
			{
				memset( m_pbyFileBuf, 0, UPDATE_FILEBUF_SIZE );
				m_dwBufUsedSize = 0;
			}
			else
			{
				m_dwBufUsedSize -= dwFileContentLen;

				Dsclog("[ProcUpdateFileReq] Write File<%s> failed! NACK!\n", tDscUpdateReqHead.m_acFileName );
				NackUpdateFileReq();
				SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
				return;
			}
		}

		// ����У��
		FILE *hp = m_hUpdateFile;
		fseek( hp, 0, SEEK_END );
		u32 dwFileLen = ftell(hp);
		if ( tDscUpdateReqHead.GetFileSize() != dwFileLen )
		{
			Dsclog("[ProcUpdateFileReq] File(%s) recv filelen(%d) not equal to reqlen(%d)! NACK!\n",
				tDscUpdateReqHead.m_acFileName, dwFileLen, tDscUpdateReqHead.GetFileSize() );
			NackUpdateFileReq();
			SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
			return;
		}

		u8 byLop = 0;
		for ( byLop = 0; byLop < m_byUpdateFileNum; byLop++ )
		{
			if ( 0 == strcmp(tDscUpdateReqHead.m_acFileName, m_achCurUpdateFileName[byLop] ) )
			{
				m_abyUpdateResult[byLop] = 1;
				break;
			}
		}
		m_tCurUpdateReq.SetRspFrmSN(0);
		memset ( m_tCurUpdateReq.m_acFileName, 0, sizeof(m_tCurUpdateReq.m_acFileName) );
		if ( NULL != m_hUpdateFile )
		{
			fclose( m_hUpdateFile );
			m_hUpdateFile = NULL;
		}
		if ( NULL != m_pbyFileBuf )
		{
			delete [] m_pbyFileBuf;
			m_pbyFileBuf = NULL;
		}
		m_dwBufUsedSize = 0;
		cMsg.SetMsgBody( (u8*)&byBrdIdx, sizeof(byBrdIdx) );
		cMsg.CatMsgBody( (u8*)&byMcsSsnIdx, sizeof(byMcsSsnIdx) );
		cMsg.CatMsgBody( (u8*)&m_tCurUpdateReq, sizeof(m_tCurUpdateReq) );
		PostMsgToManager( DSC_MPC_UPDATEFILE_ACK, cMsg.GetServMsg(), cMsg.GetServMsgLen() );		
		for ( byLop = 0; byLop < m_byUpdateFileNum; byLop++ )
		{
			if ( m_abyUpdateResult[byLop] != 1 )
			{
				break;
			}
		}
		if ( byLop == m_byUpdateFileNum )
		{
			m_byMcsSsnIdx = 0;
			m_byBrdIdx = 0;
			m_byUpdateFileNum = 0;
			memset(m_abyUpdateResult, 0 , sizeof(m_abyUpdateResult));
			Dsclog("[ProcUpdateFileReq] Update complete successfully!\n");
		}
		else
		{
			SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
		}
		return;
	}
}

/*=============================================================================
�� �� ���� ProcUpdateSoftwareWaittimer
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage* const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/21   4.0			�ܹ��                  ����
=============================================================================*/
void CBoardAgent::ProcUpdateSoftwareWaittimer( CMessage* const pcMsg )
{
	KillTimer(DSC_UPDATE_SOFTWARE_WAITTIMER);
	m_byWaitTimes++;
	CServMsg cMsg;
	if ( m_byWaitTimes <= 3 && strlen(m_tCurUpdateReq.m_acFileName) != 0 )
	{
		NackUpdateFileReq();
		SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
	}
	else
	{
		m_byWaitTimes = 0;
		m_byErrTimes = 0;
		Dsclog( "[ProcDscUpDateRsp] Lost connect during update file<%s>!\n", m_tCurUpdateReq.m_acFileName );
		if ( strlen(m_tCurUpdateReq.m_acFileName) != 0 )
		{
			//FILE *hFile = fopen( m_tCurUpdateReq.m_acFileName, "r" );
			if ( NULL == m_hUpdateFile )
			{
				m_hUpdateFile = fopen( m_tCurUpdateReq.m_acFileName, "r" );
			}
			//if ( hFile != NULL )
			if ( m_hUpdateFile != NULL )
			{
				//fclose(hFile);
				fclose( m_hUpdateFile );
				remove(m_tCurUpdateReq.m_acFileName);
				m_hUpdateFile = NULL;
			}
		}
		memset( &m_tCurUpdateReq, 0, sizeof(m_tCurUpdateReq) );		

		NackUpdateFileReq();

		m_byMcsSsnIdx = 0;
		m_byBrdIdx = 0;
		m_byUpdateFileNum = 0;	
		memset( m_achCurUpdateFileName, 0, sizeof(m_achCurUpdateFileName) );
		memset( m_abyUpdateResult, 0, sizeof(m_abyUpdateResult) );
		if ( m_hUpdateFile != NULL )
		{
			fclose( m_hUpdateFile );
			m_hUpdateFile = NULL;
		}
		if ( m_pbyFileBuf != NULL )
		{
			delete [] m_pbyFileBuf;
			m_pbyFileBuf = NULL;
		}
		m_dwBufUsedSize = 0;
		return;
	}
}

/*=============================================================================
�� �� ���� NackUpdateFileReq
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/21   4.0			�ܹ��                  ����
=============================================================================*/
void CBoardAgent::NackUpdateFileReq(void)
{
	CServMsg cMsg;
	cMsg.SetMsgBody( (u8*)&m_byBrdIdx, sizeof(m_byBrdIdx) );
	cMsg.CatMsgBody( (u8*)&m_byMcsSsnIdx, sizeof(m_byMcsSsnIdx) );
	cMsg.CatMsgBody( (u8*)&m_tCurUpdateReq, sizeof(m_tCurUpdateReq) );
	PostMsgToManager( DSC_MPC_UPDATEFILE_NACK, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
	m_byErrTimes++;
}


//////////////////////////////////////////////////////////////////////////

API u32 BrdGetDstMcuNode( void )
{
    return g_cBrdAgentApp.m_dwDstMcuNode;
}

API u32 BrdGetDstMcuNodeB(void)
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}

API void noupdatesoft( void )
{
	g_bUpdateSoftBoard = FALSE;
}

API void updatesoft( void )
{
	g_bUpdateSoftBoard = TRUE;
}





















