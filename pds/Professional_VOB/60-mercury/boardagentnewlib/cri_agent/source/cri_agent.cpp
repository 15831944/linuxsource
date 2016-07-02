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
#include "eqplib.h"
#include "cri_agent.h"

#ifdef _VXWORKS_
#include "timers.h"
#include <dirent.h>
#endif

CBrdAgentApp	g_cBrdAgentApp;	//�������Ӧ��ʵ��

extern SEMHANDLE g_semCRI;

//���캯��
CBoardAgent::CBoardAgent()
{
    m_dwBrdSlaveSysNode = INVALID_NODE;
    m_dwBrdSlaveSysIId = INVALID_INS;
    memset( &m_tBrdSlaveSysLedState, 2, sizeof(TBrdLedState) );         // ��״̬ ��
#ifdef _LINUX12_   // Ŀǰ��brdwrapper.h����SWEthInfo
    memset( &m_tBrdSlaveSysEthPortState, 3, sizeof( SWEthInfo ) ); // ����״̬ δ֪
#endif //end _LINUX12_
	return;
}

//��������
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
		OspPrintf( TRUE, FALSE,  "[InstanceEntry] The received msg's pointer in the msg entry is NULL!");
		return;
	}

/*	CriLog("%u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event));*/    
    LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, 
        "[InstanceEntry] %u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event));

	switch( pcMsg->event )
	{
	// ����������Ϣ
// 	case OSP_POWERON:		                    //��������
// 	case BRDAGENT_CONNECT_MANAGERA_TIMER:       // ���ӹ������ʱ
//     case BRDAGENT_CONNECT_MANAGERB_TIMER:
// 	case BRDAGENT_REGISTERA_TIMER:		        // ע��ʱ�䳬ʱ
//     case BRDAGENT_REGISTERB_TIMER:		        // ע��ʱ�䳬ʱ
// 	case MPC_BOARD_REG_ACK:                     //ע��Ӧ����Ϣ
// 	case MPC_BOARD_REG_NACK:			        //ע���Ӧ����Ϣ
// 	case BRDAGENT_GET_CONFIG_TIMER:             //�ȴ�����Ӧ����Ϣ��ʱ
// 	// MPC������Ϣ
// 	case MPC_BOARD_ALARM_SYNC_REQ:              //�������ĸ澯ͬ������
// 	case MPC_BOARD_BIT_ERROR_TEST_CMD:          //���������������
// 	case MPC_BOARD_TIME_SYNC_CMD:               //����ʱ��ͬ������
// 	case MPC_BOARD_SELF_TEST_CMD:               //�����Բ�����
// 	case MPC_BOARD_RESET_CMD:                   //��������������
// 	case MPC_BOARD_UPDATE_SOFTWARE_CMD:         //�����������
// 	case MPC_BOARD_GET_STATISTICS_REQ:          //��ȡ�����ͳ����Ϣ
// 	case MPC_BOARD_GET_VERSION_REQ:             //��ȡ����İ汾��Ϣ
// 	case MPC_BOARD_GET_MODULE_REQ:              //��ȡ�����ģ����Ϣ	
// 	// �����Ǹ澯����������Ϣ
// 	case SVC_AGT_MEMORY_STATUS:                 //�ڴ�״̬�ı�
// 	case SVC_AGT_FILESYSTEM_STATUS:             //�ļ�ϵͳ״̬�ı�
// 	// �����ǲ�������Ϣ
// 	case BOARD_MPC_CFG_TEST:
// 	case BOARD_MPC_GET_ALARM:
// 	case BOARD_MPC_MANAGERCMD_TEST:
// 	// ����
// 	case OSP_DISCONNECT:
// 		CBBoardAgent::InstanceEntry(pcMsg);
// 		break;

	case MPC_BOARD_GET_CONFIG_ACK:              //ȡ������ϢӦ����Ϣ
		ProcBoardGetConfigAck( pcMsg );
		break;
		
	case MPC_BOARD_GET_CONFIG_NACK:             //ȡ������Ϣ��Ӧ��
		ProcBoardGetConfigNAck( pcMsg );
		break;

	case MPC_BOARD_LED_STATUS_REQ:
		ProcLedStatusReq( pcMsg );              // ȡ����״̬ [05/04/2012 liaokang]
		break;
        
    case MPC_BOARD_ETHPORT_STATUS_REQ:
        ProcEthPortStatusReq( pcMsg );          // ȡ����״̬ [05/04/2012 liaokang]
        break;

	case BOARD_LED_STATUS:
		ProcBoardLedStatus( pcMsg );            // ��ϵͳled״̬
		break;

    case BOARD_ETHPORT_STATUS:
        ProcBoardEthPortStatus( pcMsg );        // ��ϵͳ����״̬
		break;        

	case BOARD_MODULE_STATUS:
		ProcBoardModuleStatus( pcMsg );
		break;

	case MCU_BOARD_CONFIGMODIFY_NOTIF:
		ProcBoardConfigModify( pcMsg );
		break;

     // ֧�ִ�ϵͳ��IS2.2 8313��[05/04/2012 liaokang]
    case BRDSYS_SLAVE_MASTER_REG_REQ:
        ProcBrdSlaveSysRegReq( pcMsg );         // ��ϵͳ��IS2.2 8313�� ע��
        break;

    case BRDSYS_SLAVE_MASTER_LED_STATUS_NOTIFY:
        ProcBrdSlaveSysLedStatus( pcMsg );      // ��ϵͳ��IS2.2 8313�� led״̬
        break;
    
    case BRDSYS_SLAVE_MASTER_ETHPORT_STATUS_NOTIFY:
        ProcBrdSlaveSysEthPortStatus( pcMsg );  // ��ϵͳ��IS2.2 8313�� ����״̬
        break;

    case OSP_DISCONNECT: 
        ProcDisconnect( pcMsg );                // ��������
		break;

		//Is2.2��������������8313����[6/8/2013 chendaiwei]
#if defined(_IS22_)
	case MPC_BOARD_RESET_CMD:                   //��������������
		ProcBoardIS22ResetCmd( pcMsg );
		break;
#endif

	default:
		CBBoardAgent::InstanceEntry(pcMsg);
		break;
// 		OspPrintf( TRUE, FALSE,  "[InstanceEntry] receive unknown msg %d<%s> criAgent! \n",
//                                   pcMsg->event, OspEventDesc(pcMsg->event) );
	
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
    case STATE_NORMAL:
		break;
		
	default:
		OspPrintf(TRUE, FALSE,  "BoardAgent: Wrong state %u when receive get config Nack!srcNode.%d\n", 
			CurState(),pcMsg!=NULL?pcMsg->srcnode:0 );
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
	TEqpPrsEntry*			ptPrsCfg;		// Prs��������Ϣ

	switch( CurState() ) 
	{
	case STATE_INIT:
		NEXTSTATE( STATE_NORMAL );
		byPeriCount = pcMsg->content[0]; // �����������
		wIndex = 1;

		for( wLoop = 0; wLoop < byPeriCount; wLoop++ )
		{			
			byPeriType = pcMsg->content[wIndex++]; // ������������
			
			switch( byPeriType ) // ���øõ�����Ӧ��������Ϣ
			{
			case EQP_TYPE_PRS:
				ptPrsCfg = (TEqpPrsEntry*)(pcMsg->content+wIndex);
                ptPrsCfg->SetMcuIp(g_cBrdAgentApp.GetMpcIpA());
				wIndex += sizeof(TEqpPrsEntry);
				g_cBrdAgentApp.SetPrsConfig(ptPrsCfg);
                CriLog( " The Cri board run the Prs module\n");
				break;

			default:
				break;
			}
		}
		
		// �Ժ��ǵ���IP        
        CriLog( "g_cBrdAgentApp.SetBrdIpAddr(0x%x)-wIndex = %d\n",
                            ntohl(*(u32*)&pcMsg->content[wIndex+2]), wIndex);

		g_cBrdAgentApp.SetBrdIpAddr( ntohl(  *(u32*)&pcMsg->content[wIndex+2] ) );
		
		if( pcMsg->content[ wIndex + 6 ] == 0 )// �Ժ����Ƿ�ʹ�ÿ��Ź�
		{
			SysRebootDisable( );
		}
		else
		{
			SysRebootEnable( );
		}

		BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON );// ��MLINK�Ƶ���
		OspSemGive( g_semCRI );
		
		KillTimer( BRDAGENT_GET_CONFIG_TIMER );
		
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when receive get config Ack!\n", 
			CurState() );
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
    04/11/11    3.5         �� ��         �½ӿڵ��޸�
====================================================================*/
void CBoardAgent::ProcLedStatusReq( CMessage* const pcMsg )
{
    u8  abyLedBuf[ MAX_BOARD_LED_NUM + 1 ]={0};    // led BUF  ���32����
    u8  byLedCount = 0;                        // led����
    // abyBuf�ṹ��TBrdPos + byLedCount + abyLedBuf
    u8  abyBuf[ sizeof(TBrdPos) + sizeof(u8) + MAX_BOARD_LED_NUM + 1 ] = {0};  // ��ϢBUF
    u16 dwMsgLen = 0;                         // ��Ϣ����

    // ��ȡ��ϵͳled״̬
    TBrdLedState tBrdLedState;
    BrdQueryLedState( &tBrdLedState ); 
    memcpy( &m_tBrdMasterSysLedState, &tBrdLedState, sizeof(TBrdLedState) );
    
    // ������ݽṹ
    TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );                       

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
        {

#if (defined _LINUX_ && defined _LINUX12_) // CRI2 / IS2.2
        
        if ( BRD_TYPE_IS22 == tBrdPos.byBrdID ) // IS2.2
        {
            // led count �� BUF ����ϵͳ8548 �� ��ϵͳ8313��
            byLedCount = sizeof(TBrdIS3MPC8548LedDesc) + sizeof(TBrdIS3MPC8313LedDesc);
            memcpy(abyLedBuf, &m_tBrdMasterSysLedState.nlunion.tBrdIS2XMpc8548LedState, sizeof(TBrdIS3MPC8548LedDesc));
            memcpy(abyLedBuf + sizeof(TBrdIS3MPC8548LedDesc), &m_tBrdSlaveSysLedState.nlunion.tBrdIS2XMpc8313LedState, sizeof(TBrdIS3MPC8313LedDesc));
        }
        else //CRI2
        {
            // led count �� BUF
            byLedCount = sizeof(TBrdCRI2LedDesc);
            memcpy(abyLedBuf, &m_tBrdMasterSysLedState.nlunion.tBrdCRI2LedState, byLedCount);
        }

#else // CRI
        
        // led count �� BUF
        byLedCount = sizeof(TBrdCRILedStateDesc);
        memcpy(abyLedBuf, &m_tBrdMasterSysLedState.nlunion.tBrdCRILedState, byLedCount);

#endif

        memcpy(abyBuf+sizeof(TBrdPos), &byLedCount, sizeof(u8) );         // led����
        memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), abyLedBuf, byLedCount); // led BUF
        dwMsgLen = sizeof(TBrdPos) + sizeof(u8) + byLedCount;             // ��Ϣ����
        PostMsgToManager( BOARD_MPC_LED_STATUS_ACK, abyBuf, dwMsgLen);    // ACK

        // ��ӡled״̬
        s8 abyPrintLedState[256] = {0};        
        abyPrintLedState[255] = '\0';
        s8 *pBuff = abyPrintLedState;        
        pBuff += sprintf(pBuff, "[ProcLedStatusReq] Led Num:%d [", byLedCount);
        u32 dwIndex = 0;        
        for (dwIndex = 0; dwIndex < byLedCount; ++dwIndex)
        {
            pBuff += sprintf(pBuff, " %d ", *(abyLedBuf + dwIndex));
        }
        pBuff += sprintf(pBuff, "]\n");
        LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, abyPrintLedState );
   
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
    ������      ��ProcBoardLedStatus
    ����        ��Led״̬�ı���Ϣ����(��ϵͳ)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
    12/05/04    4.7         liaokang      ֧��IS2.2��ϵͳ
====================================================================*/
void CBoardAgent::ProcBoardLedStatus( CMessage* const pcMsg )
{
    // �Ƚ� ά����ϵͳled״̬
	TBrdLedState tLedState;
	memcpy( &tLedState, pcMsg->content, sizeof(TBrdLedState) );
    if( memcmp(&m_tBrdMasterSysLedState, &tLedState, sizeof(TBrdLedState)) == 0 )
    {
        return; 
    }
    memcpy( &m_tBrdMasterSysLedState, &tLedState, sizeof(TBrdLedState) );

    u8  abyLedBuf[ MAX_BOARD_LED_NUM + 1 ]={0};    // led BUF  ���32����
    u8  byLedCount = 0;                        // led����
    // abyBuf�ṹ��TBrdPos + byLedCount + abyLedBuf
    u8  abyBuf[ sizeof(TBrdPos) + sizeof(u8) + MAX_BOARD_LED_NUM + 1 ] = {0};  // ��ϢBUF
    u16 dwMsgLen = 0;                        // ��Ϣ����

    // ������ݽṹ
    TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) ); 

#if (defined _LINUX_ && defined _LINUX12_) // CRI2 / IS2.2

    if ( BRD_TYPE_IS22 == tBrdPos.byBrdID ) // IS2.2
    {
        // led count �� BUF ����ϵͳ8548 �� ��ϵͳ8313��
        byLedCount = sizeof(TBrdIS3MPC8548LedDesc) + sizeof(TBrdIS3MPC8313LedDesc);
        memcpy(abyLedBuf, &m_tBrdMasterSysLedState.nlunion.tBrdIS2XMpc8548LedState, sizeof(TBrdIS3MPC8548LedDesc));
        memcpy(abyLedBuf + sizeof(TBrdIS3MPC8548LedDesc), &m_tBrdSlaveSysLedState.nlunion.tBrdIS2XMpc8313LedState, sizeof(TBrdIS3MPC8313LedDesc));
    }
    else //CRI2
    {
        // led count �� BUF
        byLedCount = sizeof(TBrdCRI2LedDesc);
        memcpy(abyLedBuf, &m_tBrdMasterSysLedState.nlunion.tBrdCRI2LedState, byLedCount);
    }

#else // CRI

    // led count �� BUF
    byLedCount = sizeof(TBrdCRILedStateDesc);
    memcpy(abyLedBuf, &m_tBrdMasterSysLedState.nlunion.tBrdCRILedState, byLedCount);

#endif

    memcpy(abyBuf+sizeof(TBrdPos), &byLedCount, sizeof(u8) );         // led����
    memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), abyLedBuf, byLedCount); // led BUF
	dwMsgLen = sizeof(TBrdPos) + sizeof(u8) + byLedCount;             // ��Ϣ����
    PostMsgToManager( BOARD_MPC_LED_STATUS_NOTIFY, abyBuf, dwMsgLen);

    // ��ӡled״̬
    s8 abyPrintLedState[256] = {0};        
    abyPrintLedState[255] = '\0';
    s8 *pBuff = abyPrintLedState;        
    pBuff += sprintf(pBuff, "[ProcBoardLedStatus] Led Num:%d [", byLedCount);
    u32 dwIndex = 0;        
    for (dwIndex = 0; dwIndex < byLedCount; ++dwIndex)
    {
        pBuff += sprintf(pBuff, " %d ", *(abyLedBuf + dwIndex));
    }
    pBuff += sprintf(pBuff, "]\n");
    LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, abyPrintLedState );

    return;
}

/*====================================================================
������      ��ProcEthPortStatusReq
����        ��ȡ����״̬��Ŀǰ֧��IS2.2�������Ż����޸ģ�
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg, �������Ϣ
����ֵ˵��  ��void
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
12/05/04    4.7         liaokang       ����
====================================================================*/
void CBoardAgent::ProcEthPortStatusReq( CMessage* const pcMsg )
{
    /*
    ����������ʱֻ��IS2.2 ˫��״̬��MCU�������޸�����mcuagentһ���޸�
    NMS�� ˫�� ��ʾ��1 ��˫�� 2 ȫ˫�� 3 δ֪
    �ӿ�  ˫����ʾ�� 0 ��˫�� 1 ȫ˫��
    */
    
    u8  abyEthPortBuf[ MAX_BOARD_ETHPORT_NUM + 1 ]={0};    // EthPort BUF
    u8  byEthPortCount = 0;                            // EthPort����
    // abyBuf�ṹ��TBrdPos + byEthPortCount + EthPortBuf
    u8  abyBuf[ sizeof(TBrdPos) + sizeof(u8) + MAX_BOARD_ETHPORT_NUM + 1 ] = {0};  // ��ϢBUF
    u16 dwMsgLen = 0;                              // ��Ϣ����  
   
    // ȡǰ���ڣ�0 ������״̬
    u8  byEthId = 0;       // ����
    u8  byLink =0;         // link
    u8  byEthAutoNeg = 0;  // Э��״̬
    u8  byEthDuplex = 0;   // ˫��״̬
    u32 dwEthSpeed = 0;    // �ӿ�����Mbps
#ifdef _LINUX12_   // Ŀǰ��brdwrapper.h����TBrdEthInfo
    TBrdEthInfo  tBrdFrontEthPortState;
    // byLink: 0-link down, 1-link up �������ٵ��� BrdGetEthNegStat������ˢ�����ӡ
    BrdGetEthLinkStat( byEthId, &byLink );
    if ( 1 == byLink )
    {
        BrdGetEthNegStat( byEthId, &byEthAutoNeg, &byEthDuplex, &dwEthSpeed);
    }
    tBrdFrontEthPortState.Link = byLink;
    tBrdFrontEthPortState.AutoNeg = byEthAutoNeg;
    tBrdFrontEthPortState.Duplex = byEthDuplex;
    tBrdFrontEthPortState.Speed = dwEthSpeed;    
    memcpy( &m_tBrdMasterSysFrontEthPortState, &tBrdFrontEthPortState, sizeof(TBrdEthInfo) );
#endif //end _LINUX12_

    // ������ݽṹ
    TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) ); 

    switch( CurState() ) 
    {
    case STATE_INIT:
    case STATE_NORMAL:
        {
#if (defined _LINUX_ && defined _LINUX12_) // CRI2 / IS2.2

            if ( BRD_TYPE_IS22 == tBrdPos.byBrdID ) // IS2.2
            {
                // 8548 ǰ���� 1�� + 8313 ǰ���� 2�� + 8313 ������ 8��
                byEthPortCount = 1 + FRONT_PANE_ETH_MAXNUM + BACK_PANE_ETH_MAXNUM;
                u8 byDuplexToNms = 0; // �������ṩ��˫��״̬ת��Ϊ�������˫��״̬
                u8 byLoop = 0;
                // EthPort BUF
                // ǰ���ǰ����һ������ G2
                if ( 0 == m_tBrdMasterSysFrontEthPortState.Link )
                {
                    byDuplexToNms = 3;
                }
                else
                {
                    byDuplexToNms = m_tBrdMasterSysFrontEthPortState.Duplex + 1;
                }
                memcpy( abyEthPortBuf, &byDuplexToNms, sizeof(u8)); 
                
                // ǰ���ڶ������������� G0 G1
                for ( byLoop = 0; byLoop< FRONT_PANE_ETH_MAXNUM; byLoop++ )
                {
                    if ( 0 == m_tBrdSlaveSysEthPortState.front_eth_info[byLoop].Link )
                    {
                        byDuplexToNms = 3;
                    }
                    else
                    {
                        byDuplexToNms = m_tBrdSlaveSysEthPortState.front_eth_info[byLoop].Duplex + 1;
                    }        
                    memcpy( abyEthPortBuf + sizeof(u8) * ( 1 + byLoop ), &byDuplexToNms, sizeof(u8));
                }
                
                // �����link0-7
                for ( byLoop = 0; byLoop< BACK_PANE_ETH_MAXNUM; byLoop++ )
                {
                    if ( 0 == m_tBrdSlaveSysEthPortState.back_eth_info[byLoop].Link )
                    {
                        byDuplexToNms = 3;
                    }
                    else
                    {
                        byDuplexToNms = m_tBrdSlaveSysEthPortState.back_eth_info[byLoop].Duplex + 1;
                    }        
                    memcpy( abyEthPortBuf + sizeof(u8) * ( FRONT_PANE_ETH_MAXNUM + 1 + byLoop), &byDuplexToNms, sizeof(u8));
                } 
            }
            else //CRI2
            {
                // Ԥ������ʱ��������
            }

#else // CRI
            
            // Ԥ������ʱ��������

#endif

            memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );                       // ������ݽṹ
            memcpy(abyBuf+sizeof(TBrdPos), &byEthPortCount, sizeof(u8) );     // EthPort����
            memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), abyEthPortBuf, byEthPortCount); // EthPort BUF
            dwMsgLen = sizeof(TBrdPos) + sizeof(u8) + byEthPortCount;         // ��Ϣ���� 
            PostMsgToManager( BOARD_MPC_ETHPORT_STATUS_ACK, abyBuf, dwMsgLen);// ACK

            // ��ӡEthPort״̬
            s8 abyPrintEthPortState[256] = {0};        
            abyPrintEthPortState[255] = '\0';
            s8 *pBuff = abyPrintEthPortState;        
            pBuff += sprintf(pBuff, "[ProcEthPortStatusReq] EthPort Num:%d [", byEthPortCount);
            u32 dwIndex = 0;        
            for (dwIndex = 0; dwIndex < byEthPortCount; ++dwIndex)
            {
                pBuff += sprintf(pBuff, " %d ", *(abyEthPortBuf + dwIndex));
            }
            pBuff += sprintf(pBuff, "]\n");
            LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, abyPrintEthPortState );

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
������      ��ProcBoardEthPortStatus
����        ������״̬�ı䣨��ϵͳ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg, �������Ϣ
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
12/05/04    4.7         liaokang       ����
====================================================================*/
void CBoardAgent::ProcBoardEthPortStatus( CMessage* const pcMsg )
{
    /*
    ����������ʱֻ��IS2.2 ˫��״̬��MCU�������޸�����mcuagentһ���޸�
    NMS�� ˫�� ��ʾ��1 ��˫�� 2 ȫ˫�� 3 δ֪
    �ӿ�  ˫����ʾ�� 0 ��˫�� 1 ȫ˫��
    */

#ifdef _LINUX12_   // Ŀǰ��brdwrapper.h����TBrdEthInfo
    // �Ƚ� ά����ϵͳ����״̬
    TBrdEthInfo tBrdFrontEthPortState;
    memcpy( &tBrdFrontEthPortState, pcMsg->content, sizeof(TBrdEthInfo) );
    if( memcmp(&m_tBrdMasterSysFrontEthPortState, &tBrdFrontEthPortState, sizeof(TBrdEthInfo)) == 0 )
    {
        return; 
    }
    memcpy( &m_tBrdMasterSysFrontEthPortState, &tBrdFrontEthPortState, sizeof(TBrdEthInfo) );
#endif //end _LINUX12_
    
    u8  abyEthPortBuf[ MAX_BOARD_ETHPORT_NUM + 1 ]={0};    // EthPort BUF
    u8  byEthPortCount = 0;                            // EthPort����
    // abyBuf�ṹ��TBrdPos + byEthPortCount + EthPortBuf
    u8  abyBuf[ sizeof(TBrdPos) + sizeof(u8) + MAX_BOARD_ETHPORT_NUM + 1 ] = {0};  // ��ϢBUF
    u16 dwMsgLen = 0;                              // ��Ϣ����

    // ������ݽṹ
    TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
    memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) ); 
    
#if (defined _LINUX_ && defined _LINUX12_) // CRI2 / IS2.2
    
    if ( BRD_TYPE_IS22 == tBrdPos.byBrdID ) // IS2.2
    {
        // 8548 ǰ���� 1�� + 8313 ǰ���� 2�� + 8313 ������ 8��
        byEthPortCount = 1 + FRONT_PANE_ETH_MAXNUM + BACK_PANE_ETH_MAXNUM;
        u8 byDuplexToNms = 0; // �������ṩ��˫��״̬ת��Ϊ�������˫��״̬ 
        u8 byLoop = 0;
        // EthPort BUF
        // ǰ���ǰ����һ������ G2
        if ( 0 == m_tBrdMasterSysFrontEthPortState.Link )
        {
            byDuplexToNms = 3;
        }
        else
        {
            byDuplexToNms = m_tBrdMasterSysFrontEthPortState.Duplex + 1;
        }
        memcpy( abyEthPortBuf, &byDuplexToNms, sizeof(u8)); 
        
        // ǰ���ڶ������������� G0 G1
        for ( byLoop = 0; byLoop< FRONT_PANE_ETH_MAXNUM; byLoop++ )
        {
            if ( 0 == m_tBrdSlaveSysEthPortState.front_eth_info[byLoop].Link )
            {
                byDuplexToNms = 3;
            }
            else
            {
                byDuplexToNms = m_tBrdSlaveSysEthPortState.front_eth_info[byLoop].Duplex + 1;
            }        
            memcpy( abyEthPortBuf + sizeof(u8) * ( 1 + byLoop ), &byDuplexToNms, sizeof(u8));
        }
        
        // �����link0-7
        for ( byLoop = 0; byLoop< BACK_PANE_ETH_MAXNUM; byLoop++ )
        {
            if ( 0 == m_tBrdSlaveSysEthPortState.back_eth_info[byLoop].Link )
            {
                byDuplexToNms = 3;
            }
            else
            {
                byDuplexToNms = m_tBrdSlaveSysEthPortState.back_eth_info[byLoop].Duplex + 1;
            }        
            memcpy( abyEthPortBuf + sizeof(u8) * ( FRONT_PANE_ETH_MAXNUM + 1 + byLoop), &byDuplexToNms, sizeof(u8));
        } 

        memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );                       // ������ݽṹ
        memcpy(abyBuf+sizeof(TBrdPos), &byEthPortCount, sizeof(u8) );     // EthPort����
        memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), abyEthPortBuf, byEthPortCount); // EthPort BUF
        dwMsgLen = sizeof(TBrdPos) + sizeof(u8) + byEthPortCount;         // ��Ϣ���� 
        PostMsgToManager( BOARD_MPC_ETHPORT_STATUS_NOTIFY, abyBuf, dwMsgLen);// ACK

        // ��ӡEthPort״̬
        s8 abyPrintEthPortState[256] = {0};        
        abyPrintEthPortState[255] = '\0';
        s8 *pBuff = abyPrintEthPortState;        
        pBuff += sprintf(pBuff, "[ProcBoardEthPortStatus] EthPort Num:%d [", byEthPortCount);
        u32 dwIndex = 0;        
        for (dwIndex = 0; dwIndex < byEthPortCount; ++dwIndex)
        {
            pBuff += sprintf(pBuff, " %d ", *(abyEthPortBuf + dwIndex));
        }
        pBuff += sprintf(pBuff, "]\n");
        LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, abyPrintEthPortState );

    }
    else //CRI2
    {
        // Ԥ������ʱ��������
    }
    
#else // CRI

    // Ԥ������ʱ��������

#endif

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
void CBoardAgent::ProcBoardModuleStatus( CMessage* const pcMsg )
{

	TBoardAlarm tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	if( FindAlarm( ALARM_MCU_MODULE_OFFLINE, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) )
	{
		if( 0 == *( pcMsg->content ))      //normal
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
				tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );
			
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
		if( 1 == *( pcMsg->content ) )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_MODULE_OFFLINE, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) == FALSE )
			{
				brdagtlog( "BoardAgent: AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg );
			
				memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}

	return;
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
2007/9/17   4.0			�ܹ��                  ����
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
		CriLog( "[ProcBoardConfigModify] Receive msg is not local board msg! Layer = %d, Slot = %d\n", byLayer, bySlot );
		return;
	}

	u8	byPeriCount;
	u8	byPeriType;
	u16	wLoop, wIndex;
	TEqpPrsEntry* ptPrsCfg;		// Prs��������Ϣ
	BOOL32 bIsCfgPrs = FALSE;
	
	u8 *pMsg = cServMsg.GetMsgBody()+sizeof(u8)+sizeof(u8);

	byPeriCount = pMsg[0]; // �����������
	wIndex = 1;

	for( wLoop = 0; wLoop < byPeriCount; wLoop++ )
	{			
		byPeriType = pMsg[wIndex++]; // ������������
		
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
					CriLog( " [ProcBoardConfigModify]Start the prs failed!\n");
				}	
				else
				{
					CriLog( " [ProcBoardConfigModify]Start the prs OK!\n");
				}			
			}
			else
			{
				CriLog( "The board have started the Prs module, need reboot!\n" );
				return;
			}
			break;

		default:
			break;
		}
	}
	
	if ( !bIsCfgPrs && g_cBrdAgentApp.IsRunPrs() )
	{
		CriLog("[ProcBoardConfigModify] New cfg delete the prs, need reboot!\n");
		return;
	}

	// �Ժ��ǵ���IP        
    CriLog( "g_cBrdAgentApp.SetBrdIpAddr(0x%x)-wIndex = %d\n",
                        ntohl(*(u32*)&pMsg[wIndex+2]), wIndex);

	if ( g_cBrdAgentApp.GetBrdIpAddr() != ntohl(  *(u32*)&pMsg[wIndex+2] ) )
	{
		CriLog( "The Cri ip changed, need recfg!\n" );
		return;
	}
			
	if( pMsg[ wIndex + 6 ] == 0 )// �Ժ����Ƿ�ʹ�ÿ��Ź�
	{
		SysRebootDisable( );
	}
	else
	{
		SysRebootEnable( );
	}
		
	return;
}

/*====================================================================
������      ��ProcBrdSlaveSysRegReq
����        ����ϵͳ��IS2.2 8313�� ע������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg, �������Ϣ
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
12/05/04    4.7         liaokang       ����
====================================================================*/
void CBoardAgent::ProcBrdSlaveSysRegReq( CMessage* const pcMsg )
{  
    m_dwBrdSlaveSysNode = pcMsg->srcnode;
    m_dwBrdSlaveSysIId = pcMsg->srcid;

    // ���ö���������
	OspSetHBParam( m_dwBrdSlaveSysNode, 10, 3 );
    // ����node�����ж�ʱ��֪ͨ��AppId��InstId
    OspNodeDiscCBRegQ( m_dwBrdSlaveSysNode, GetAppID(), GetInsID() );

    PostMsgToBrdSlaveSys( BRDSYS_MASTER_SLAVE_REG_ACK );
}


/*====================================================================
������      ��ProcBrdSlaveSysLedStatus
����        ��led״̬������ϵͳ IS2.2 8313��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg, �������Ϣ
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
12/05/04    4.7         liaokang       ����
====================================================================*/
void CBoardAgent::ProcBrdSlaveSysLedStatus( CMessage* const pcMsg )
{
    // �Ƚ� ά����ϵͳled״̬
    TBrdLedState tLedState;
    memcpy( &tLedState, pcMsg->content, sizeof(TBrdLedState) );
    if( memcmp(&m_tBrdSlaveSysLedState, &tLedState, sizeof(TBrdLedState)) == 0 )
    {
        return; 
    }
    memcpy( &m_tBrdSlaveSysLedState, &tLedState, sizeof(TBrdLedState) );  
    
    u8  abyLedBuf[ MAX_BOARD_LED_NUM + 1 ]={0};    // led BUF  ���32����
    u8  byLedCount = 0;                            // led����
    // abyBuf�ṹ��TBrdPos + byLedCount + abyLedBuf
    u8  abyBuf[ sizeof(TBrdPos) + sizeof(u8) + MAX_BOARD_LED_NUM + 1 ] = {0};  // ��ϢBUF
    u16 dwMsgLen = 0;                        // ��Ϣ����
    
#if (defined _LINUX_ && defined _LINUX12_)
    // IS2.2
    TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
    memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );                       // ������ݽṹ
    byLedCount = sizeof(TBrdIS3MPC8548LedDesc) + sizeof(TBrdIS3MPC8313LedDesc);
    memcpy(abyBuf+sizeof(TBrdPos), &byLedCount, sizeof(u8) );         // led����
    memcpy(abyLedBuf, &m_tBrdMasterSysLedState.nlunion.tBrdIS2XMpc8548LedState, sizeof(TBrdIS3MPC8548LedDesc));
    memcpy(abyLedBuf + sizeof(TBrdIS3MPC8548LedDesc), &m_tBrdSlaveSysLedState.nlunion.tBrdIS2XMpc8313LedState, sizeof(TBrdIS3MPC8313LedDesc));
    memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), abyLedBuf, byLedCount); // led BUF
    dwMsgLen = sizeof(TBrdPos) + sizeof(u8) + byLedCount;             // ��Ϣ����
#endif
    PostMsgToManager( BOARD_MPC_LED_STATUS_NOTIFY, abyBuf, dwMsgLen);

    // ��ӡled״̬
    s8 abyPrintLedState[256] = {0};        
    abyPrintLedState[255] = '\0';
    s8 *pBuff = abyPrintLedState;        
    pBuff += sprintf(pBuff, "[ProcBrdSlaveSysLedStatus] Led Num:%d [", byLedCount);
    u32 dwIndex = 0;        
    for (dwIndex = 0; dwIndex < byLedCount; ++dwIndex)
    {
        pBuff += sprintf(pBuff, " %d ", *(abyLedBuf + dwIndex));
    }
    pBuff += sprintf(pBuff, "]\n");
    LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, abyPrintLedState );
    
    return;
}

/*====================================================================
������      ��ProcBrdSlaveSysEthPortStatus
����        ����ϵͳ��IS2.2 8313������״̬
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg, �������Ϣ
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
12/05/04    4.7         liaokang       ����
====================================================================*/
void CBoardAgent::ProcBrdSlaveSysEthPortStatus( CMessage* const pcMsg )
{
#ifdef _LINUX12_   // Ŀǰ��brdwrapper.h����SWEthInfo

    /*
    ����������ʱֻ��IS2.2 ˫��״̬��MCU�������޸�����mcuagentһ���޸�
    NMS�� ˫�� ��ʾ��1 ��˫�� 2 ȫ˫�� 3 δ֪
    �ӿ�  ˫����ʾ�� 0 ��˫�� 1 ȫ˫��
    */

    // �Ƚ� ά����ϵͳ����״̬
    SWEthInfo tEthPortState;
    memcpy( &tEthPortState, pcMsg->content, sizeof(SWEthInfo) );
    if( memcmp(&m_tBrdSlaveSysEthPortState, &tEthPortState, sizeof(SWEthInfo)) == 0 )
    {
        return; 
    }    
    memcpy( &m_tBrdSlaveSysEthPortState, &tEthPortState, sizeof(SWEthInfo) );

    u8  abyEthPortBuf[ MAX_BOARD_ETHPORT_NUM + 1 ]={0};    // EthPort BUF
    u8  byEthPortCount = 0;                            // EthPort����
    // abyBuf�ṹ��TBrdPos + byEthPortCount + EthPortBuf
    u8  abyBuf[ sizeof(TBrdPos) + sizeof(u8) + MAX_BOARD_ETHPORT_NUM + 1 ] = {0};  // ��ϢBUF
    u16 dwMsgLen = 0;                              // ��Ϣ����
    
    // ������ݽṹ
    TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
    memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) ); 
    
    // 8548 ǰ���� 1�� + 8313 ǰ���� 2�� + 8313 ������ 8��
    byEthPortCount = 1 + FRONT_PANE_ETH_MAXNUM + BACK_PANE_ETH_MAXNUM;
    u8 byDuplexToNms = 0; // �������ṩ��˫��״̬ת��Ϊ�������˫��״̬
    u8 byLoop = 0;
    // EthPort BUF
    // ǰ���ǰ����һ������ G2
    if ( 0 == m_tBrdMasterSysFrontEthPortState.Link )
    {
        byDuplexToNms = 3;
    }
    else
    {
        byDuplexToNms = m_tBrdMasterSysFrontEthPortState.Duplex + 1;
    }
    memcpy( abyEthPortBuf, &byDuplexToNms, sizeof(u8)); 
    
    // ǰ���ڶ������������� G0 G1
    for ( byLoop = 0; byLoop< FRONT_PANE_ETH_MAXNUM; byLoop++ )
    {
        if ( 0 == m_tBrdSlaveSysEthPortState.front_eth_info[byLoop].Link )
        {
            byDuplexToNms = 3;
        }
        else
        {
            byDuplexToNms = m_tBrdSlaveSysEthPortState.front_eth_info[byLoop].Duplex + 1;
        }        
        memcpy( abyEthPortBuf + sizeof(u8) * ( 1 + byLoop ), &byDuplexToNms, sizeof(u8));
    }

    // �����link0-7
    for ( byLoop = 0; byLoop< BACK_PANE_ETH_MAXNUM; byLoop++ )
    {
        if ( 0 == m_tBrdSlaveSysEthPortState.back_eth_info[byLoop].Link )
        {
            byDuplexToNms = 3;
        }
        else
        {
            byDuplexToNms = m_tBrdSlaveSysEthPortState.back_eth_info[byLoop].Duplex + 1;
        }        
        memcpy( abyEthPortBuf + sizeof(u8) * ( FRONT_PANE_ETH_MAXNUM + 1 + byLoop), &byDuplexToNms, sizeof(u8));
    } 
    
    memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );                       // ������ݽṹ
    memcpy(abyBuf+sizeof(TBrdPos), &byEthPortCount, sizeof(u8) );     // EthPort����
    memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), abyEthPortBuf, byEthPortCount); // EthPort BUF
    dwMsgLen = sizeof(TBrdPos) + sizeof(u8) + byEthPortCount;         // ��Ϣ���� 
    PostMsgToManager( BOARD_MPC_ETHPORT_STATUS_NOTIFY, abyBuf, dwMsgLen);

    // ��ӡEthPort״̬
    s8 abyPrintEthPortState[256] = {0};        
    abyPrintEthPortState[255] = '\0';
    s8 *pBuff = abyPrintEthPortState;        
    pBuff += sprintf(pBuff, "[ProcBrdSlaveSysEthPortStatus] EthPort Num:%d [", byEthPortCount);
    u32 dwIndex = 0;        
    for (dwIndex = 0; dwIndex < byEthPortCount; ++dwIndex)
    {
        pBuff += sprintf(pBuff, " %d ", *(abyEthPortBuf + dwIndex));
    }
    pBuff += sprintf(pBuff, "]\n");
    LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, abyPrintEthPortState ); 
    
#endif //end _LINUX12_

	return;
}

/*====================================================================
������      ��ProcDisconnect
����        ����������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg, �������Ϣ
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
12/05/04    4.7         liaokang       ����
====================================================================*/
void CBoardAgent::ProcDisconnect( CMessage* const pcMsg )
{
    u32 dwNode = *(u32*)pcMsg->content;
    if ( INVALID_NODE != dwNode )
    {
        OspDisconnectTcpNode(dwNode);
    }
    // ��ϵͳ��IS2.2 8313����������
    if( dwNode == m_dwBrdSlaveSysNode )
    {
        // ��մ�ϵͳ�������
        m_dwBrdSlaveSysNode = INVALID_NODE;
        m_dwBrdSlaveSysIId = INVALID_INS;
        memset( &m_tBrdSlaveSysLedState, 2, sizeof(TBrdLedState) );         // ��״̬ ��
#ifdef _LINUX12_   // Ŀǰ��brdwrapper.h����SWEthInfo
        memset( &m_tBrdSlaveSysEthPortState, 0, sizeof(SWEthInfo) );        // ����״̬
#endif //end _LINUX12_
        // ģ��mcu agentģ��brdmanager��������֪ͨmcu agentģ��brdmanager
        SetTimer( MPC_BOARD_LED_STATUS_REQ, 10 );    // ����led״̬
        SetTimer( MPC_BOARD_ETHPORT_STATUS_REQ, 10 );// ��������״̬
        LogPrint( LOG_LVL_KEYSTATUS, MID_MCULIB_MCUAGT, "[ProcDisconnect] Board Slave System disconnect!\n" );
    }
    else
    {
        CBBoardAgent::InstanceEntry( pcMsg );
    }
}

/*====================================================================
������      ��PostMsgToBrdSlaveSys
����        ������Ϣ����ϵͳ��IS2.2 8313��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u16 wEvent:   ��Ϣ��
              void * pMsg   ��Ϣ���� 
			  u16  wLen:    ��Ϣ���ݳ���
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
12/05/04    4.7         liaokang       ����
====================================================================*/
void CBoardAgent::PostMsgToBrdSlaveSys( u16 wEvent, void * pMsg, u16 wLen)
{
    if( OspIsValidTcpNode( m_dwBrdSlaveSysNode ) )
    {
        post( m_dwBrdSlaveSysIId, wEvent, pMsg, wLen, m_dwBrdSlaveSysNode );
        LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, 
            "[PostMsgToBrdSlaveSys]Post message %u(%s) to slave system!\n", wEvent, ::OspEventDesc( wEvent ));
    }
    else
    {
        LogPrint( LOG_LVL_ERROR, MID_MCULIB_MCUAGT,
            "[PostMsgToBrdSlaveSys]Post message %u(%s) to slave system ERROR!\n", wEvent, ::OspEventDesc( wEvent ));
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
void CBoardAgent::ProcBoardIS22ResetCmd( CMessage* const pcMsg )
{
    switch(CurState())
    {
    case STATE_IDLE:
    case STATE_INIT:
    case STATE_NORMAL:
		PostMsgToBrdSlaveSys(pcMsg->event);
        OspPrintf(TRUE, FALSE, "[ProcBoardIS22ResetCmd] send reboot cmd to 8313,Reboot 8548!\n");
        OspDelay(1000);		
        BrdHwReset();

        break;

    default:
        OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n",
                                pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState());
        break;
    }
	return;
}


/*lint -save -e765*/
API u32 BrdGetDstMcuNode( void )
{
    return g_cBrdAgentApp.m_dwDstMcuNodeA;
}

API u32 BrdGetDstMcuNodeB(void)
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}

/*lint -save -esym(530,argptr)*/
API void CriLog(char * fmt, ...)
{
	if ( g_cBrdAgentApp.m_bPrintBrdLog )
	{
		OspPrintf(TRUE, FALSE, "[Cri]");
	}
    va_list argptr;
	va_start(argptr, fmt);
	g_cBrdAgentApp.brdlog(fmt, argptr);
	va_end(argptr);
    return;
}

API void pcrimsg(void)
{
	g_cBrdAgentApp.m_bPrintBrdLog = TRUE;
}

API void npcrimsg( void )
{
	g_cBrdAgentApp.m_bPrintBrdLog = FALSE;
}

// API void nupdatesoft( void )
// {
// 	g_cBrdAgentApp.m_bUpdateSoftBoard = FALSE;
// }
// API void updatesoft( void )
// {
// 	g_cBrdAgentApp.m_bUpdateSoftBoard = TRUE;
// }
// API u8  GetBoardSlot()
// {
// 	return g_cBrdAgentApp.GetBrdPosition().byBrdSlot;
// }

/*lint -restore*/
