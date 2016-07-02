/*****************************************************************************
   ģ����      : Board Manager
   �ļ���      : brdmanager.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ����������ʵ��
   ����        : 
   �汾        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/11/20  3.0         jianghy     ����
   2004/11/10  3.6         libo        �½ӿ��޸�
   2004/11/29  3.6         libo        ��ֲ��Linux
   2005/08/17  4.0         liuhuiyun   4.0
   2006/04/30  4.0		   liuhuiyun   �Ż�
******************************************************************************/
#include "brdmanager.h"
#include "configureagent.h"
#include "agentinterface.h"
#include "commonlib.h"

//extern u8 g_byEthBak;
extern u8 g_byPrintLvl;
/*lint -save -esym(526,g_cMcuAgent)*/
extern CAgentInterface g_cMcuAgent;
/*lint -restore*/
CBrdManagerApp	g_cBrdManagerApp; // �������Ӧ��

CBoardManager::CBoardManager()
{
    m_dwBrdIId  = INVALID_INS;
    m_dwBrdNode = INVALID_NODE;
    memset( &m_tBrdPosition, 0, sizeof(m_tBrdPosition));    
}
CBoardManager::~CBoardManager()
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
void CBoardManager::InstanceEntry(CMessage* const pcMsg)
{
	Agtlog( LOG_VERBOSE, "Msg %u(%s) received!\n", pcMsg->event, ::OspEventDesc( pcMsg->event ) );

	if( NULL == pcMsg )
	{
		Agtlog(LOG_ERROR, "[InstanceEntry] BoardManager: The received msg's pointer is NULL!");
		return;
	}
    
	switch( pcMsg->event )
	{
	case BOARD_MPC_REG:					// ����ע����Ϣ
		ProcBoardRegMsg( pcMsg );
		break;

	case BOARD_MPC_GET_CONFIG:			// ����ȡ������Ϣ��Ϣ
		ProcBoardGetConfigMsg( pcMsg );
		break;

	case BOARD_MPC_ALARM_SYNC_ACK:		// �澯ͬ����Ӧ
		ProcBoardAlarmSyncAck( pcMsg );
		break;

	case BOARD_MPC_ALARM_NOTIFY:		// ����澯�����ϱ�
		ProcBoardAlarmNotify( pcMsg );
		break;

	case BOARD_MPC_LED_STATUS_NOTIFY:	// ����״̬�ϱ�
	case BOARD_MPC_LED_STATUS_ACK:		// ȡ����
		ProcBoardLedNotify( pcMsg );
		break;
    
    //  ���� ����״̬���� [2012/05/04 liaokang] 
    case BOARD_MPC_ETHPORT_STATUS_NOTIFY:	// ����״̬�ϱ�
    case BOARD_MPC_ETHPORT_STATUS_ACK:		// ȡ����״̬
        ProcBoardEthPortNotify( pcMsg );
		break;

	case BOARD_MPC_GET_VERSION_ACK:		// ȡ����İ汾��Ϣ��Ӧ
		ProcBoardGetVersionAck( pcMsg );
		break;

	case BOARD_MPC_GET_MODULE_ACK:		// ȡ�����ģ����Ϣ��Ӧ
		ProcBoardGetModuleAck( pcMsg );
		break;   

	case BOARD_MPC_UPDATE_NOTIFY:
		ProcBoardUpdateRsp( pcMsg );
		break;

	/*case BOARD_MPC_SOFTWARE_VERSION_NOTIFY:
		ProcBoardSoftwareVerNotify(pcMsg);
		break;
	*/

#ifdef _MINIMCU_
	case BOARD_MPC_SETDSCINFO_ACK:
	case BOARD_MPC_SETDSCINFO_NACK:
		ProcBoardSetDscInfoRsp( pcMsg );	// ����DSC info����Ļ�Ӧ������, zgc, 2007-07-17
		break;

	case DSC_MPC_STARTUPDATE_SOFTWARE_ACK:
	case DSC_MPC_STARTUPDATE_SOFTWARE_NACK:
		ProcDscStartUpdateSoftwareRsp(pcMsg);
		break;

	case DSC_MPC_UPDATEFILE_ACK:
	case DSC_MPC_UPDATEFILE_NACK:
		ProcDscMpcUpdateFileRsp(pcMsg);
		break;
#endif

	case MCU_BOARD_CONFIGMODIFY_NOTIF:
		ProcBoardConfigModifyNotif(pcMsg);
		break;

	case MPC_BOARD_DISCONNECT_CMD:
		ProcMpcBrdDisconnectCmd();
		break;

	// �����Ǵ�MPC�ϲ㷢���һЩ���ʹ�ñ�������(��֮���ܷ�����)
	case MPC_BOARD_BIT_ERROR_TEST_CMD:			// �������
	case MPC_BOARD_TIME_SYNC_CMD:				// ʱ��ͬ��
	case MPC_BOARD_SELF_TEST_CMD:				// �Բ�
	case MPC_BOARD_RESET_CMD:					// ���帴λ
	case MPC_BOARD_GET_STATISTICS_REQ:			// ȡ����ͳ����Ϣ
	case MPC_BOARD_GET_MODULE_REQ:				// ȡ�����ģ����Ϣ
	case MPC_BOARD_E1_LOOP_CMD:					// E1����
	case MPC_BOARD_UPDATE_SOFTWARE_CMD:			// �������
	case MPC_BOARD_SETDSCINFO_REQ:				// ����DSC info, zgc, 2007-07-17
	case MPC_BOARD_DSCGKINFO_UPDATE_CMD:		// ����DSC GKINFO, zgc, 2007-07-21
	case MPC_DSC_STARTUPDATE_SOFTWARE_REQ:		// DSC����������, zgc, 2007-08-21
	case MPC_DSC_UPDATEFILE_REQ:				// MCU�����������ݰ�, zgc, 2007-08-25
	case MCU_BOARD_DSCTELNETLOGININFO_UPDATE_CMD: // MCU����DSC����LoginInfo, zgc, 2007-10-12
		ProcMpcCommand( pcMsg );
		break;       
	case OSP_DISCONNECT:						// Osp����
		ProcBoardDisconnect( pcMsg );
		break;
    case BOARD_MPC_E1BANDWIDTH_NOTIF:
        ProcBoardE1BandWidthNotif( pcMsg );
        break;

	default:
		break;
	}

	return;
}

/*====================================================================
    ������      ��ProcBoardRegMsg
    ����        ���������ע����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBoardManager::ProcBoardRegMsg(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
    {
        Agtlog(LOG_ERROR, "[BoardManager] The pointer can not be Null\n");
        return;
    }

    // ��Ϣ��ṹ��TBoardPosition��dwBrdIp(u32)��byChoice(u8)+dwBrdNode(u32)+dwBrdInstId(u32)+byOsType(u8) (����Daemon)
	TBrdStatus tBrdStatus;
	u32 dwIp = 0;
    u32 dwGetFromAgentIp = 0;
	u8  byLoop = 0;
    s8  achBuf[256];
    BOOL32 bRet = FALSE;
	u16 wRet = 0;
    time_t      tCurTime;
    struct tm  *ptTime = NULL;

    CServMsg  cServMsg(pcMsg->content, pcMsg->length);
//	TBrdPosition tBrdPos = *(TBrdPosition*)cServMsg.GetMsgBody();
// [pengjie 2010/3/23] �������ϲ�mcu����Ľӿ���ȡ�忨λ�ã���Ϊ����඼�����TBrdPos
	TBrdPos tMcuBrdPos = *(TBrdPos*)cServMsg.GetMsgBody();
    u32 dwRevBrdIp = *(u32*)(cServMsg.GetMsgBody() + sizeof(TBrdPos));
    u8 byChoice    = *(u8*) (cServMsg.GetMsgBody() + sizeof(TBrdPos) + sizeof(u32));

    m_dwBrdNode    = *(u32*)(cServMsg.GetMsgBody() + sizeof(TBrdPos) + sizeof(u32)+sizeof(u8) );
    m_dwBrdIId     = *(u32*)(cServMsg.GetMsgBody() + sizeof(TBrdPos) + sizeof(u32)+sizeof(u8) + sizeof(u32) );
    m_byOsType     = *(u8*)( cServMsg.GetMsgBody() + sizeof(TBrdPos) + sizeof(u32)+sizeof(u8) + sizeof(u32) + sizeof(u32) );

	LogPrint(LOG_ERROR,MID_MCU_CFG,"[ProcBoardRegMsg]Brd<%d,%d> ostype == %d\n",tMcuBrdPos.byBrdLayer,tMcuBrdPos.byBrdSlot,m_byOsType);

    TBoardInfo tBrdInfo;
	u8  byBrdType = BRD_TYPE_UNKNOW;            // jlb ����������ͣ��Ա�Hdu���Ӳ���ָʾ�ƣ�hduû��ָʾ��
#ifdef _MINIMCU_
	u32 dwDscBrdIp = 0;
	u32 dwDscMcuIp = 0;
	u32 dwIpMaskLocal = 0;
	u32 dwDscConfigIp = 0;
	
	u32 dwHduBrdIp = 0;
	u32 dwHduMcuIp = 0;
	u32 dwHduConfigIp = 0;
	BOOL32 bIsConfiged = TRUE;
#endif

//#ifndef WIN32
	STATUS nRet = OK;
//#endif

	// [pengjie 2010/3/23] TBrdPos�ṹ��ת��
	TBrdPosition tBrdWrapPos;
	tBrdWrapPos.byBrdID    = tMcuBrdPos.byBrdID;
	tBrdWrapPos.byBrdLayer = tMcuBrdPos.byBrdLayer;
	tBrdWrapPos.byBrdSlot  = tMcuBrdPos.byBrdSlot;

	//lukunpeng��ʱ�޸ģ��ȴ�mcs������Ϻ�ɾ��(2009/10/23)
	//����mcs��û�ж�dri16��������������£���ʱ�޸�����
	g_cCfgParse.AdjustConfigedBrdDRI16(tBrdWrapPos);

    switch( CurState() ) 
	{
    case STATE_WAITREG:
    {
		// ��÷���ע���DSC����ڲ�IP, ���ӵ�MCU���ڲ�IP�Լ�IP MASK
		// zgc, 2007-03-09        
#ifdef _MINIMCU_
		if( BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/ == tBrdWrapPos.byBrdID 
			|| BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/ == tBrdWrapPos.byBrdID 
			|| BRD_TYPE_DSC/*KDV8000B_MODULE*/ == tBrdWrapPos.byBrdID )
		{
			dwDscBrdIp = OspNodeIpGet(m_dwBrdNode);
			dwDscMcuIp = OspNodeLocalIpGet(m_dwBrdNode);

			Agtlog( LOG_INFORM, "DscBrdIp = 0x%x, DscMcuIp = 0x%x, RevBrdIp = 0x%x\n", ntohl(dwDscBrdIp), ntohl(dwDscMcuIp), ntohl(dwRevBrdIp) );
		}
		if ( BRD_TYPE_HDU/*BRD_HWID_KDM200_HDU*/ == tBrdWrapPos.byBrdID 
			|| BRD_TYPE_HDU_L/*BRD_HWID_KDM200_HDU*/ == tBrdWrapPos.byBrdID
			|| BRD_TYPE_HDU2 == tBrdWrapPos.byBrdID
			|| BRD_TYPE_HDU2_L == tBrdWrapPos.byBrdID
			|| BRD_TYPE_HDU2_S == tBrdWrapPos.byBrdID)
		{
			dwHduBrdIp = OspNodeIpGet(m_dwBrdNode);
			dwHduMcuIp = OspNodeLocalIpGet(m_dwBrdNode);
			Agtlog( LOG_INFORM, "HduBrdIp = 0x%x, HduMcuIp = 0x%x, RevBrdIp = 0x%x\n", ntohl(dwHduBrdIp), ntohl(dwHduMcuIp), ntohl(dwRevBrdIp) );
		}
#endif // ifdef _MINIMCU_

#ifndef _MINIMCU_
		// 1.�Ƿ������˸õ���
		//[2011/01/25 zhushz]IS2.x����֧��
		//  [12/4/2010 chendaiwei]���IS22������⴦���ֶ�д�����PRS�������ļ�
		if(BRD_TYPE_IS22 == tBrdWrapPos.byBrdID)
		{
			// �޸ĵ������ñ�
			u8 byBrdNum = 0;
			TBoardInfo atBrdInfo[MAX_BOARD_NUM];
			wRet = g_cCfgParse.ReadBrdTable(&byBrdNum, atBrdInfo);
			if ( wRet != SUCCESS_AGENT )
			{
				Agtlog(LOG_ERROR, "[ProcBoardRegMsg] Read Brd Table Error!\n" );
				post( m_dwBrdIId, MPC_BOARD_REG_NACK, NULL, 0, m_dwBrdNode );
				::OspDisconnectTcpNode( m_dwBrdNode );
				ClearInst();
			}
			else
			{
				Agtlog(LOG_INFORM, "[ProcBoardRegMsg] Configed Brd num is %d\n", byBrdNum );
				for ( byLoop = 0; byLoop < byBrdNum; byLoop++ )
				{
					if( BRD_TYPE_IS22 == atBrdInfo[byLoop].GetType() 
						&& atBrdInfo[byLoop].GetLayer() == tBrdWrapPos.byBrdLayer
						&& atBrdInfo[byLoop].GetSlot() == tBrdWrapPos.byBrdSlot )
					{
						// ���浥����Ϣ
						atBrdInfo[byLoop].SetBrdIp( ntohl(dwRevBrdIp) );
						atBrdInfo[byLoop].SetPortChoice( byChoice );
						atBrdInfo[byLoop].SetOSType( m_byOsType );
						atBrdInfo[byLoop].SetAlias( g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID) );

						// ����ǵ�һ��ע�ᣬ����prs��Ϣ
						TEqpPrsInfo atPrsInfo[MAXNUM_PERIEQP];
						memset( atPrsInfo, 0, sizeof(atPrsInfo) );
						u8 byPrsNum = 0;
						wRet = g_cCfgParse.ReadPrsTable(&byPrsNum, atPrsInfo);
						if( SUCCESS_AGENT != wRet )
						{
							Agtlog(LOG_ERROR, "[ProcBoardRegMsg] Read Prs Table Error!\n" );
						}
						else
						{
							u8 byPrsLoop = 0;
							for ( byPrsLoop = 0; byPrsLoop < byPrsNum; byPrsLoop++ )
							{
								if ( atPrsInfo[byPrsLoop].GetRunBrdId() == g_cCfgParse.GetBrdId(tBrdWrapPos) )
								{
									//  [12/4/2010 chendaiwei]IS2.2����ߵ������PRS��Ϣ����Ϣ��Ȼ����
									if ( atPrsInfo[byPrsLoop].GetIpAddr() != ntohl(dwRevBrdIp) )
									{
										atPrsInfo[byPrsLoop].SetIpAddr( ntohl(dwRevBrdIp) );
									}
									break;
								}
							}

							// ���δ�ҵ�������is2.2��δ����prs��Ϣ��������
							if( byPrsLoop == byPrsNum )
							{
								u8 byPRSPeriId = 0;
								byPRSPeriId = g_cCfgParse.GetIdlePRSEqpId();
								if(byPRSPeriId != 0)
								{
									atBrdInfo[byLoop].SetPeriId( &byPRSPeriId, 1 );
									g_cCfgParse.WriteBrdTable( byBrdNum, atBrdInfo );

									atPrsInfo[byPrsLoop].SetFirstTimeSpan(DEF_FIRSTTIMESPAN_PRS);
									atPrsInfo[byPrsLoop].SetSecondTimeSpan(DEF_SECONDTIMESPAN_PRS);
									atPrsInfo[byPrsLoop].SetThirdTimeSpan(DEF_THIRDTIMESPAN_PRS);
									atPrsInfo[byPrsLoop].SetRejectTimeSpan(DEF_LOSETIMESPAN_PRS);
									s8 asPRSAlias[15] = {0};
									sprintf(asPRSAlias,"IS22_PRS_%d", atBrdInfo[byLoop].GetBrdId());
									atPrsInfo[byPrsLoop].SetAlias(asPRSAlias);
									atPrsInfo[byPrsLoop].SetEqpId(byPRSPeriId);
									atPrsInfo[byPrsLoop].SetEqpRecvPort(PRS_EQP_STARTPORT);
									atPrsInfo[byPrsLoop].SetIpAddr(ntohl(dwRevBrdIp) );
									// ����MCS���治�����õĶ˿���
									atPrsInfo[byPrsLoop].SetMcuRecvPort(PRS_MCU_STARTPORT+(byPRSPeriId-1-PRSID_MIN)*PRS_MCU_PORTSPAN);
									atPrsInfo[byPrsLoop].SetRunBrdId(atBrdInfo[byLoop].GetBrdId());
									atPrsInfo[byPrsLoop].SetSwitchBrdId(atBrdInfo[byLoop].GetBrdId());

									wRet = g_cCfgParse.WritePrsTable( byPrsNum + 1, atPrsInfo );
									if( SUCCESS_AGENT != wRet )
									{
										Agtlog(LOG_ERROR, "[ProcBoardRegMsg] Write Prs Table Error!\n" );
									}
								}
							}
						}
						break;
					}
				}
			}
		} //  [12/4/2010 chendaiwei]����IS2.2�����ע�����⴦��

		if( !g_cCfgParse.IsConfigedBrd( tBrdWrapPos ) )
		{
            OspPrintf(TRUE, FALSE, "BoardManager: Board<%d,%d:%s> not configed!\n", 
                                   tBrdWrapPos.byBrdLayer,tBrdWrapPos.byBrdSlot,
                                   g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID));
            Agtlog(LOG_ERROR, "BoardManager: Board<%d,%d:%s> not configed!\n", 
				               tBrdWrapPos.byBrdLayer,tBrdWrapPos.byBrdSlot,
                               g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID));
			post( m_dwBrdIId, MPC_BOARD_REG_NACK, NULL, 0, m_dwBrdNode );
			::OspDisconnectTcpNode( m_dwBrdNode );
			ClearInst();
			return;
		}
#else
		// 8000B ��MDSC��HDSC��DCS����Ҫͨ��У�� [10/11/2011 chendaiwei]
		if( !g_cCfgParse.IsConfigedBrd( tBrdWrapPos ) 
			&& (BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/ != tBrdWrapPos.byBrdID) 
			&& (BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/ != tBrdWrapPos.byBrdID)
			&& (BRD_TYPE_DSC/*KDV8000B_MODULE*/ != tBrdWrapPos.byBrdID) )
		{
            OspPrintf( TRUE, FALSE, "BoardManager: Board<%d,%d:%s> not configed!\n", 
                tBrdWrapPos.byBrdLayer,tBrdWrapPos.byBrdSlot,
				g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID));
            Agtlog(LOG_ERROR, "BoardManager: Board<%d,%d:%s> not configed!\n", 
				tBrdWrapPos.byBrdLayer,tBrdWrapPos.byBrdSlot,
				g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID));
			post( m_dwBrdIId, MPC_BOARD_REG_NACK, NULL, 0, m_dwBrdNode );
			::OspDisconnectTcpNode( m_dwBrdNode );
			ClearInst();
			return;
		}

#endif
        // ��ȡ������Ϣ
		if( g_cCfgParse.IsConfigedBrd( tBrdWrapPos ) )
        {
			g_cCfgParse.GetBrdCfgById( g_cCfgParse.GetBrdId(tBrdWrapPos), &tBrdInfo );
            byBrdType = tBrdInfo.GetType();
#ifdef _MINIMCU_
			dwDscConfigIp = tBrdInfo.GetBrdIp();
			dwHduConfigIp = tBrdInfo.GetBrdIp();
#endif
			Agtlog(LOG_VERBOSE, "[ProcBoardRegMsg] The %s[0x%x] is already configed! Configed ip is 0x%x\n", 
								g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID),
								ntohl(dwRevBrdIp),
								tBrdInfo.GetBrdIp());
			
		}
		else
		{
			Agtlog(LOG_VERBOSE, "[ProcBoardRegMsg] The %s[0x%x] is not configed!\n", 
								g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID),
								ntohl(dwRevBrdIp));
		}


#ifndef _MINIMCU_
		// 2.IP��ַ(������)�Ƿ�һ��
        dwGetFromAgentIp = htonl(tBrdInfo.GetBrdIp());
        if( dwGetFromAgentIp != dwRevBrdIp ) 
		{
			// �澯
			tBrdStatus.byLayer = tBrdWrapPos.byBrdLayer;
			tBrdStatus.bySlot  = tBrdWrapPos.byBrdSlot;
			tBrdStatus.byType  = tBrdWrapPos.byBrdID;
			tBrdStatus.byStatus = BOARD_STATUS_CFGCONFLICT;
            tBrdStatus.byOsType = 0;

            // FIXME: ����� CONFCONFLICT ״̬��ת�������������Ϊ BOARD_UNKNOW
            //        �Ƿ���Կ����� mcuconst.h ���һ�� BOARD_CFGCONFILCT, �һ����
            //        �յ���״̬�󽫸�λ�õĵ�����ʾΪ��ɫ��ֱ�����������ڻ�ø��õĽ��潻��Ч�� 2006-11-03
            post(MAKEIID(AID_MCU_AGENT, 1), MCU_AGT_BOARD_STATUSCHANGE, 
				                (u8*)&tBrdStatus, sizeof(tBrdStatus) ); 

            OspPrintf( TRUE, FALSE, "BoardManager: Board<%d,%d:%s> config conflict!, RcvIp: %0x, CfgIp: 0x%x\n", 
                tBrdWrapPos.byBrdLayer,tBrdWrapPos.byBrdSlot,
                g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID),
                               dwRevBrdIp, dwGetFromAgentIp);
            Agtlog(LOG_WARN, "BoardManager: Board<%d,%d:%s> config conflict!, RcvIp: %0x, CfgIp: 0x%x\n", 
				               tBrdWrapPos.byBrdLayer,tBrdWrapPos.byBrdSlot,
                               g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID),
                               dwRevBrdIp, dwGetFromAgentIp );
			post( m_dwBrdIId, MPC_BOARD_REG_NACK, NULL, 0, m_dwBrdNode );
			::OspDisconnectTcpNode( m_dwBrdNode );
            ClearInst();

			return;	
		}
#else
		// �Ƿ�����MPC�ڿ�IP����ע��, zgc, 2007-07-18
		if( BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/ == tBrdWrapPos.byBrdID 
		 || BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/ == tBrdWrapPos.byBrdID 
		 || BRD_TYPE_DSC/*KDV8000B_MODULE*/ == tBrdWrapPos.byBrdID )
		{
#ifdef _LINUX_
			TBrdEthParamAll tBrdEthParamAll;
			memset( &tBrdEthParamAll, 0 ,sizeof(TBrdEthParamAll) );
			nRet = AgtGetBrdEthParamAll( 1, &tBrdEthParamAll ); 
			u8 byIpNum = tBrdEthParamAll.dwIpNum;
			if( 0 < byIpNum )
			{
				for( byLoop = 0; byLoop < byIpNum; byLoop++ )
				{
					if( dwDscMcuIp == tBrdEthParamAll.atBrdEthParam[byLoop].dwIpAdrs )
					{
						dwIpMaskLocal = tBrdEthParamAll.atBrdEthParam[byLoop].dwIpMask;
						break;
					}
				}
				if ( byLoop == byIpNum )
				{
                    OspPrintf( TRUE, FALSE, "BoardManager: Board<%d,%d:%s> not local dsc!, Remote Ip: 0x%x \n", 
                                tBrdWrapPos.byBrdLayer,tBrdWrapPos.byBrdSlot,
                                g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID),
                                dwDscBrdIp);
					Agtlog(LOG_VERBOSE, "BoardManager: Board<%d,%d:%s> not local dsc!, Remote Ip: 0x%x \n", 
				               tBrdWrapPos.byBrdLayer,tBrdWrapPos.byBrdSlot,
                               g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID),
                               dwDscBrdIp );
					post( m_dwBrdIId, MPC_BOARD_REG_NACK, NULL, 0, m_dwBrdNode );
					::OspDisconnectTcpNode( m_dwBrdNode );
					ClearInst();
					return;
				}
				// �޸ĵ������ñ�ʹDSC�������IP��DSC��ѡ���IPһ��, zgc, 2007-07-19
				if ( g_cCfgParse.IsConfigedBrd( tBrdWrapPos ) 
					&& htonl(tBrdInfo.GetBrdIp()) != dwRevBrdIp )
				{
					Agtlog( LOG_INFORM, "BoardManager: change board<0x%x> ip to be 0x%x\n", tBrdInfo.GetBrdIp(), ntohl(dwRevBrdIp) );
					tBrdInfo.SetBrdIp( ntohl(dwRevBrdIp) );
					g_cCfgParse.SetBrdCfgById( g_cCfgParse.GetBrdId(tBrdWrapPos), &tBrdInfo );
				}
			}
			else
			{
				Agtlog(LOG_WARN, "BoardManager: MPC not config inner Ip!\n");
				post( m_dwBrdIId, MPC_BOARD_REG_NACK, NULL, 0, m_dwBrdNode );
				::OspDisconnectTcpNode( m_dwBrdNode );
				ClearInst();
				return;
			}
#endif
#ifdef _VXWORKS_
			TBrdEthParam tBrdEthParam;
			memset( &tBrdEthParam, 0, sizeof(TBrdEthParam) );
			nRet = AgtGetBrdEthParam( 1, &tBrdEthParam ); 
			if ( dwDscMcuIp != tBrdEthParam.dwIpAdrs && tBrdEthParam.dwIpAdrs != 0)
			{
                OspPrintf( TRUE, FALSE, "BoardManager: Board<%d,%d:%s> not local dsc!, Remote Ip: 0x%x\n", 
                            tBrdWrapPos.byBrdLayer,tBrdWrapPos.byBrdSlot,
                            g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID),
                            dwDscBrdIp);
				Agtlog(LOG_WARN, "BoardManager: Board<%d,%d:%s> not local dsc!, Remote Ip: 0x%x\n", 
				               tBrdWrapPos.byBrdLayer,tBrdWrapPos.byBrdSlot,
                               g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID),
                               dwDscBrdIp);
				post( m_dwBrdIId, MPC_BOARD_REG_NACK, NULL, 0, m_dwBrdNode );
				::OspDisconnectTcpNode( m_dwBrdNode );
				ClearInst();
				return;
			}
			else if ( 0 == tBrdEthParam.dwIpAdrs )
			{
				Agtlog(LOG_WARN, "BoardManager: MPC not config inner Ip!\n");
				post( m_dwBrdIId, MPC_BOARD_REG_NACK, NULL, 0, m_dwBrdNode );
				::OspDisconnectTcpNode( m_dwBrdNode );
				ClearInst();
				return;
			}
			else
			{
				dwIpMaskLocal = tBrdEthParam.dwIpMask;
				// �޸ĵ������ñ�ʹDSC�������IP��DSC��ѡ���IPһ��, zgc, 2007-07-19
				if ( g_cCfgParse.IsConfigedBrd( tBrdWrapPos )
					&& htonl(tBrdInfo.GetBrdIp()) != dwRevBrdIp )
				{
					tBrdInfo.SetBrdIp( ntohl(dwRevBrdIp) );
					g_cCfgParse.SetBrdCfgById( g_cCfgParse.GetBrdId(tBrdWrapPos), &tBrdInfo );
				}
			}
#endif
#ifdef WIN32
			dwIpMaskLocal = INET_ADDR( DEFAULT_INNERIPMASK );
			// �޸ĵ������ñ�ʹDSC�������IP��DSC��ѡ���IPһ��, zgc, 2007-07-19
			if ( g_cCfgParse.IsConfigedBrd( tBrdWrapPos )
				&& htonl(tBrdInfo.GetBrdIp()) != dwRevBrdIp )
			{
				tBrdInfo.SetBrdIp( ntohl(dwRevBrdIp) );
				g_cCfgParse.SetBrdCfgById( g_cCfgParse.GetBrdId(tBrdWrapPos), &tBrdInfo );
			}
#endif
		}
		else if ( BRD_TYPE_HDU/*BRD_HWID_KDM200_HDU*/ == tBrdWrapPos.byBrdID 
					||  BRD_TYPE_HDU_L/*BRD_HWID_KDM200_HDU*/ == tBrdWrapPos.byBrdID
					||  BRD_TYPE_HDU2 == tBrdWrapPos.byBrdID
					|| BRD_TYPE_HDU2_L == tBrdWrapPos.byBrdID
					|| BRD_TYPE_HDU2_S == tBrdWrapPos.byBrdID)
		{
#ifdef _LINUX_

#endif
		}
		else
		{
		#ifndef WIN32
            OspPrintf( TRUE, FALSE, "BoardManager: Board<%d,%d:%s> not dsc brd!, Remote Ip: 0x%x\n", 
                        tBrdWrapPos.byBrdLayer,tBrdWrapPos.byBrdSlot,
                        g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID),
						dwDscBrdIp);
			Agtlog(LOG_WARN, "BoardManager: Board<%d,%d:%s> not dsc brd!, Remote Ip: 0x%x\n", 
							   tBrdWrapPos.byBrdLayer,tBrdWrapPos.byBrdSlot,
							   g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID),
							   dwDscBrdIp );
			post( m_dwBrdIId, MPC_BOARD_REG_NACK, NULL, 0, m_dwBrdNode );
			::OspDisconnectTcpNode( m_dwBrdNode );
			ClearInst();
			return;
		#endif
		}
#endif // #ifndef _MINIMCU_

        // 3.�õ����Ƿ��Ѿ�ע���
        bRet = g_cBrdManagerApp.IsRegedBoard(tBrdWrapPos, m_dwBrdNode);
        if( bRet )
        {
            OspPrintf( TRUE, FALSE, "BoardManager: Board<%d,%d:%s> already regiestered or disconnect not received !\n", 
                        tBrdWrapPos.byBrdLayer, tBrdWrapPos.byBrdSlot,
                        g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID));
            Agtlog(LOG_WARN, "BoardManager: Board<%d,%d:%s> already regiestered or disconnect not received !\n", 
				               tBrdWrapPos.byBrdLayer, tBrdWrapPos.byBrdSlot,
                               g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID) );
            
            ::OspDisconnectTcpNode( m_dwBrdNode );
            ClearInst();
			return;			
        }

#ifdef _MINIMCU_
		if ( !g_cCfgParse.IsConfigedBrd( tBrdWrapPos ) )
		{



			bIsConfiged = FALSE;
			// �޸ĵ������ñ�, zgc, 2007-07-30
			u8 byBrdNum = 0;
			TBoardInfo atBrdInfo[MAX_BOARD_NUM];
			wRet = g_cCfgParse.ReadBrdTable(&byBrdNum, atBrdInfo);
			if ( wRet != SUCCESS_AGENT )
			{
				Agtlog(LOG_ERROR, "[ProcBoardRegMsg] Read Brd Table Error!\n" );
			}
			else
			{
				Agtlog(LOG_INFORM, "[ProcBoardRegMsg] Configed Brd num is %d\n", byBrdNum );
				for ( byLoop = 0; byLoop < byBrdNum; byLoop++ )
				{
					if ( 
						(BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/ == atBrdInfo[byLoop].GetType() 
					  || BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/ == atBrdInfo[byLoop].GetType()
					  || BRD_TYPE_DSC/*KDV8000B_MODULE*/ == atBrdInfo[byLoop].GetType())
					  &&
						(BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/ == tBrdWrapPos.byBrdID 
					  || BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/ == tBrdWrapPos.byBrdID
					  || BRD_TYPE_DSC/*KDV8000B_MODULE*/ == tBrdWrapPos.byBrdID)
					  &&
						 tBrdWrapPos.byBrdID != atBrdInfo[byLoop].GetType() )
					{
						atBrdInfo[byLoop].SetBrdId( byLoop+1 );
						atBrdInfo[byLoop].SetType( tBrdWrapPos.byBrdID );
						atBrdInfo[byLoop].SetLayer( tBrdWrapPos.byBrdLayer );
						atBrdInfo[byLoop].SetSlot( tBrdWrapPos.byBrdSlot );
						atBrdInfo[byLoop].SetBrdIp( ntohl(dwRevBrdIp) );
						atBrdInfo[byLoop].SetPortChoice( byChoice );
						atBrdInfo[byLoop].SetOSType( m_byOsType );
						atBrdInfo[byLoop].SetAlias( g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID) );					
						break;
					}
				}
				if ( byLoop == byBrdNum )
				{
					atBrdInfo[byLoop].SetBrdId( byLoop+1 );
					atBrdInfo[byLoop].SetType( tBrdWrapPos.byBrdID );
					atBrdInfo[byLoop].SetLayer( tBrdWrapPos.byBrdLayer );
					atBrdInfo[byLoop].SetSlot( tBrdWrapPos.byBrdSlot );
					atBrdInfo[byLoop].SetBrdIp( ntohl(dwRevBrdIp) );
					atBrdInfo[byLoop].SetPortChoice( byChoice );
					atBrdInfo[byLoop].SetOSType( m_byOsType );
					atBrdInfo[byLoop].SetAlias( g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID) );
					byBrdNum++;
				}
				g_cCfgParse.WriteBrdTable( byBrdNum, atBrdInfo );
				memcpy( &tBrdInfo, &atBrdInfo[byLoop], sizeof(TBoardInfo) );
			}			
		}

		// �޸�prs�����а�IP����Ϊ��IP����DSC��ע��ǰ�Ͷ���ģ����MCU�����ļ���ԭ����
		// DSC��IP��ע��������IP��һ�£��ᵼ��DSC��ע��ɹ���PRSע��ʧ�ܣ��������DSC�巴����������
		// zgc, 2007-08-17
		TEqpPrsInfo atPrsInfo[MAXNUM_PERIEQP];
		memset( atPrsInfo, 0, sizeof(atPrsInfo) );
		u8 byPrsNum = 0;
		wRet = g_cCfgParse.ReadPrsTable(&byPrsNum, atPrsInfo);
		if( SUCCESS_AGENT != wRet )
		{
			Agtlog(LOG_ERROR, "[ProcBoardRegMsg] Read Prs Table Error!\n" );
		}
		else
		{
			if ( byPrsNum > 0 )
			{
				for ( byLoop = 0; byLoop < byPrsNum; byLoop++ )
				{
					if ( atPrsInfo[byLoop].GetRunBrdId() == g_cCfgParse.GetBrdId(tBrdWrapPos) )
					{
						if ( atPrsInfo[byLoop].GetIpAddr() != ntohl(dwRevBrdIp) )
						{
							atPrsInfo[byLoop].SetIpAddr( ntohl(dwRevBrdIp) );
						}
						break;
					}
				}
				wRet = g_cCfgParse.WritePrsTable( byPrsNum, atPrsInfo );
				if( SUCCESS_AGENT != wRet )
				{
					Agtlog(LOG_ERROR, "[ProcBoardRegMsg] Write Prs Table Error!\n" );
				}
			}
		}

		//����network�е�GKIpAddr
		TNetWorkInfo  tNetWorkInfo;
		g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
		if ( tNetWorkInfo.GetGkIp() > 0 )
		{
			if ( dwDscConfigIp == tNetWorkInfo.GetGkIp()
				&& tNetWorkInfo.GetGkIp() != ntohl(dwRevBrdIp) )
			{
				Agtlog( LOG_INFORM, "[ProcBoardRegMsg] Modify GK ip, to be 0x%x!\n", ntohl(dwRevBrdIp) );
				tNetWorkInfo.SetGKIp( ntohl(dwRevBrdIp) );
			}
			//����RRQMtadpIp
			if ( dwDscConfigIp == tNetWorkInfo.GetRRQMtadpIp()
				&& tNetWorkInfo.GetRRQMtadpIp() != ntohl(dwRevBrdIp) )
			{
				Agtlog( LOG_INFORM, "[ProcBoardRegMsg] Modify RRQMtadp ip, to be 0x%x!\n", ntohl(dwRevBrdIp) );
				tNetWorkInfo.SetRRQMtadpIp( ntohl(dwRevBrdIp) );
			}
			g_cCfgParse.SetNetWorkInfo( &tNetWorkInfo );
		}
#endif

        // 4.��ʽ���ӵ��壺һϵ�б���
        g_cBrdManagerApp.AddBoard(tBrdWrapPos, m_dwBrdIId, m_dwBrdNode);

        // ����״̬,���ĵ��������״̬�ȡ�
        memcpy( &m_tBrdPosition, &tBrdWrapPos, sizeof(tBrdWrapPos) );
        
        if( tBrdInfo.GetPortChoice() != byChoice )
        {
            Agtlog(LOG_WARN, "BoardManager: The Board<%d,%d:%s> eth choice is different.\n",
                             tBrdWrapPos.byBrdLayer, tBrdWrapPos.byBrdSlot,
                             g_cCfgParse.GetBrdTypeStr(tBrdWrapPos.byBrdID) );

			tBrdInfo.SetPortChoice(byChoice);

			g_cCfgParse.SetBrdCfgById( g_cCfgParse.GetBrdId(tBrdWrapPos), &tBrdInfo );

        }
        
        // �޸ĵ�������״̬
        g_cCfgParse.ChangeBoardStatus( tBrdWrapPos, BOARD_STATUS_INLINE );

		// ֪ͨǰ̨����Ҫ�ָ��澯��
		tBrdStatus.byLayer = m_tBrdPosition.byBrdLayer;
		tBrdStatus.bySlot = m_tBrdPosition.byBrdSlot;
		tBrdStatus.byType = m_tBrdPosition.byBrdID;
		tBrdStatus.byStatus = BOARD_STATUS_INLINE; //  0��������1��������2�����ó�ͻ 
        tBrdStatus.byOsType = m_byOsType;
		post( MAKEIID(AID_MCU_AGENT, 1), MCU_AGT_BOARD_STATUSCHANGE, (u8*)&tBrdStatus, sizeof(tBrdStatus) );	

		// ��������ACK
		if(255 == m_tBrdPosition.byBrdID ) // ����
		{
			post( m_dwBrdIId, MPC_BOARD_REG_ACK, &m_tBrdPosition.byBrdSlot, 
				           sizeof(m_tBrdPosition.byBrdSlot), m_dwBrdNode );
		}
		else
		{
			BOOL32 bMpcEnable = g_cCfgParse.IsMpcActive();
			TBrdRegAck tRegAck;
			TMPCInfo tMpcInfo;
			g_cCfgParse.GetMPCInfo( &tMpcInfo );
			if ( tMpcInfo.GetOtherMpcIp() != 0 )
			{
				tRegAck.SetOtherMpcIp( tMpcInfo.GetOtherMpcIp() );
				tRegAck.SetOtherMpcPort( tMpcInfo.GetOtherMpcPort() );
			}
			else
			{
				tRegAck.SetOtherMpcIp( 0 );
				tRegAck.SetOtherMpcPort( 0 );
			}
			Agtlog(LOG_INFORM, " RegAck: Other mpc ip is 0x%x, Other mpc port is %d\n", tRegAck.GetOtherMpcIp(), tRegAck.GetOtherMpcPort());
			cServMsg.SetMsgBody((u8*)&bMpcEnable, sizeof(bMpcEnable));
			cServMsg.CatMsgBody((u8*)&tRegAck, sizeof(tRegAck));
			post( m_dwBrdIId, MPC_BOARD_REG_ACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwBrdNode );
		}

        ::OspSetHBParam(m_dwBrdNode, 
                        g_tAgentDebugInfo.GetBoardDiscTimeSpan(), 
                        g_tAgentDebugInfo.GetBoardDiscTimes() );
        ::OspNodeDiscCBRegQ( m_dwBrdNode, GetAppID(), GetInsID() );	// ע���������ʵ��

#ifdef _MINIMCU_
        // guzh [1/24/2007] �����8000B xDSC��,����������õ��ڲ�IP
        // guzh [5/11/2007] ����ͨ���ڲ�IP����ʱ�޸�
        if ( m_tBrdPosition.byBrdID == BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/ ||
             m_tBrdPosition.byBrdID == BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/ || 
             m_tBrdPosition.byBrdID == BRD_TYPE_DSC/*KDV8000B_MODULE*/  
			 )
        {
			TDSCModuleInfo tDscInfo;
			g_cCfgParse.GetDSCInfo( &tDscInfo );

			tDscInfo.SetDscInnerIp( ntohl(dwDscBrdIp) );
			tDscInfo.SetMcuInnerIp( ntohl(dwDscMcuIp) );
			tDscInfo.SetInnerIpMask( ntohl(dwIpMaskLocal) );

			g_cCfgParse.SetLastDscInfo( &tDscInfo );
			g_cCfgParse.SetDSCInfo( &tDscInfo,	TRUE );
//#ifndef WIN32
			// �����ڲ�·��
			TBrdIpRouteParam tBrdIpRouteParam;
			u32 dwIpMask = INET_ADDR(DEFAULT_INNERIPMASK);

			tBrdIpRouteParam.dwDesIpMask = 0xFFFFFFFF;
			tBrdIpRouteParam.dwDesIpNet = dwRevBrdIp;
			tBrdIpRouteParam.dwGwIpAdrs = dwDscBrdIp;

			nRet = BrdAddOneIpRoute( &tBrdIpRouteParam );
			if ( ERROR == nRet )
			{
				Agtlog( LOG_ERROR, "[ProcBoardRegMsg] Add Ip routes error! DesIpNet.0x%x, DesIpMask.0x%x, GwIp.0x%x\n", 
						tBrdIpRouteParam.dwDesIpNet,
						tBrdIpRouteParam.dwDesIpMask,
						tBrdIpRouteParam.dwGwIpAdrs);
			}
			BrdSaveNipConfig();

//#endif
			cServMsg.SetMsgBody((u8*)&bIsConfiged, sizeof(BOOL32));
			post( MAKEIID(AID_MCU_AGENT, 1), AGT_SVC_DSCREGSUCCEED_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
        }        
#endif
        
        // ������Ϣ��֤�ɹ��󣬻�ȡ�����һЩ��Ϣ		
        // 1: ͬ���澯
        PostMsgToBoard( MPC_BOARD_ALARM_SYNC_REQ, NULL, 0 );
        
        // 2: ʱ��ͬ��        
        tCurTime = time( NULL );
        ptTime = localtime( &tCurTime );
        sprintf( (s8*)achBuf, "%4.4u%2.2u%2.2u%2.2u%2.2u%2.2u", ptTime->tm_year + 1900, 			
                 ptTime->tm_mon + 1, ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );		
        PostMsgToBoard( MPC_BOARD_TIME_SYNC_CMD,(u8*)achBuf, strlen(achBuf) );
        
        // 3: ȡ�汾��Ϣ
        PostMsgToBoard( MPC_BOARD_GET_VERSION_REQ, NULL, 0 );
        
        // 4: ȡģ����Ϣ
        PostMsgToBoard( MPC_BOARD_GET_MODULE_REQ, NULL, 0 );
        
        // 5: ȡ����״̬
		if (BRD_TYPE_UNKNOW != byBrdType
			&& BRD_TYPE_HDU/*BRD_HWID_KDM200_HDU*/ != byBrdType
			&& BRD_TYPE_HDU_L != byBrdType
			&& BRD_TYPE_HDU2 != byBrdType
			&& BRD_TYPE_HDU2_L != byBrdType
			&& BRD_TYPE_HDU2_S != byBrdType)                      //hdu����û��ָʾ��
		{
			PostMsgToBoard( MPC_BOARD_LED_STATUS_REQ, NULL, 0 );
		}

        // 6: ȡ����״̬ [2012/05/04 liaokang]
        if ( BRD_TYPE_IS22 == byBrdType ) // IS2.2
        {
            PostMsgToBoard( MPC_BOARD_ETHPORT_STATUS_REQ, NULL, 0 );
		}

		NEXTSTATE( STATE_INIT );
		break;
    }
	default:
		Agtlog(LOG_ERROR, "[ProcBoardRegMsg] Wrong message %u(%s) received in current state %u!\n", 
			                pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        Agtlog(LOG_ERROR, "[ProcBoardRegMsg] for node: %d, ip:0x%0x, Inst: %d\n", 
                            m_dwBrdNode, ntohl(OspNodeIpGet(m_dwBrdNode)), GetInsID());
		break;
	}

	return;
}

/*=============================================================================
  �� �� ���� ClearInst
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CBoardManager::ClearInst( void )
{
	m_dwBrdIId = INVALID_INS;
	if( INVALID_NODE != m_dwBrdNode )
	{
		OspDisconnectTcpNode( m_dwBrdNode );
	}
    
    if (g_cBrdManagerApp.IsRegedBoard(m_tBrdPosition))
    {
        g_cBrdManagerApp.DelBoard(m_tBrdPosition);
    }	
	m_dwBrdNode = INVALID_NODE;

	memset( &m_tBrdPosition, 0, sizeof(m_tBrdPosition));

    DeleteAlias();
    
    NextState(STATE_IDLE);
}


/*====================================================================
    ������      ��Generate
    ����        ��������������Ϣ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    06/08/11    4.0         ����        ����
====================================================================*/
u16 CEqpConfigBuf::Generate()
{
    //��Ϣ�ṹ  �����ϵ�������� + ��������������Ϣ  + (DRI DTI)��ͬ������ + ����IP + ���Ź�
    // guzh [1/12/2007] 8000B ��ĩβ���� TDSCModuleInfo ��Ϣ�ṹ

    memset( m_abyBuf, 0, sizeof(m_abyBuf) );
    u16 wPtr = 0;
    // ��������
    m_abyBuf[wPtr++] = m_byCfgNum;
    
    u8 byPeriType;
    for ( u8 byLoop = 0; byLoop < m_byCfgNum; byLoop ++)
    {
        byPeriType = m_utCfg[byLoop].byEqpType;
        // �������ͣ�Ȼ�������Ӧ�ṹ
        m_abyBuf[wPtr++] = byPeriType;
		switch( byPeriType )
		{
		case EQP_TYPE_TVWALL:
			memcpy(m_abyBuf+wPtr, &m_utCfg[byLoop].tTVWallCfg, sizeof(TEqpTVWallEntry));
			wPtr += sizeof(TEqpTVWallEntry);

			break;
			
		case EQP_TYPE_MIXER:
		case 12: //[2/14/2012 chendaiwei]ԭEQP_TYPE_EMIXERֵΪ12���˴�����V4R7��ǰEAPU	
			memcpy(m_abyBuf+wPtr, &m_utCfg[byLoop].tMixerCfg, sizeof(TEqpMixerEntry));
			wPtr += sizeof(TEqpMixerEntry);

			break;
			
		case EQP_TYPE_VMP:
			memcpy(m_abyBuf+wPtr, &m_utCfg[byLoop].tVMPCfg, sizeof(TEqpVMPEntry));
			wPtr += sizeof(TEqpVMPEntry);

			break;

        case EQP_TYPE_VMPTW:
			memcpy(m_abyBuf+wPtr, &m_utCfg[byLoop].tMpwCfg, sizeof(TEqpMPWEntry));
			wPtr += sizeof(TEqpMPWEntry);

            break;

		case EQP_TYPE_BAS:

			if(m_utCfg[byLoop].bySubEqpType == TYPE_APU2BAS )
			{
				memcpy(m_abyBuf+wPtr, &m_utCfg[byLoop].tMpuBasCfg, sizeof(TEqpMpuBasEntry));
				wPtr += sizeof(TEqpMpuBasEntry);
			}
			else
			{
				memcpy(m_abyBuf+wPtr, &m_utCfg[byLoop].tBasCfg, sizeof(TEqpBasEntry));
				wPtr += sizeof(TEqpBasEntry);
			}

			break;

		case EQP_TYPE_PRS:
			memcpy(m_abyBuf+wPtr, &m_utCfg[byLoop].tPrsCfg, sizeof(TEqpPrsEntry));
			wPtr += sizeof(TEqpPrsEntry);

			break;            
			
		case EQP_TYPE_HDU:
// 		case EQP_TYPE_HDU_H:
// 		case EQP_TYPE_HDU_L:
			memcpy(m_abyBuf+wPtr, &m_utCfg[byLoop].tHduCfg, sizeof(TEqpHduEntry));
			wPtr += sizeof(TEqpHduEntry);
    		break;            

        default:
            break;
        }
    }

    // ��ͬ����Ϣ
    m_abyBuf[wPtr++] = m_byUseNetSync;
    if (m_byUseNetSync)
    {
        m_abyBuf[wPtr++] = m_byNetSyncE1Chnl;
    }
    else
    {
        wPtr ++;
    }
    // ����IP
    memcpy( m_abyBuf+wPtr, &m_dwIp, sizeof(u32) );
    wPtr += sizeof(u32);

    //���Ź�
    m_abyBuf[wPtr++] = m_byUseWatchDog;

    // guzh [1/12/2007]  8000B ��ĩβ���� TDSCModuleInfo ��Ϣ�ṹ
#ifdef _MINIMCU_
    memcpy( m_abyBuf+wPtr, &m_tDscModule, sizeof(m_tDscModule) );
    wPtr += sizeof(m_tDscModule);
#endif

	// zgc [15/08/2007] 8000B/C��ĩβ����GKIP
#ifdef _MINIMCU_
	memcpy( m_abyBuf+wPtr, &m_dwGKIp, sizeof(m_dwGKIp) );
	wPtr += sizeof(m_dwGKIp);
#endif

	// zgc 2007-10-12 8000B/C��ĩβ����TLoginInfo
#ifdef _MINIMCU_
	memcpy( m_abyBuf+wPtr, &m_tLoginInfo, sizeof(m_tLoginInfo) );
	wPtr += sizeof(m_tLoginInfo);
#endif

    // ���س���
    return wPtr;
}


/*====================================================================
    ������      ��GetWatchDogOption
    ����        ����ȡWATCH DOG������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
u8 CBoardManager::GetWatchDogOption( )
{
	s8   achFullName[64];
	u32  dwWDEnable;
	sprintf( achFullName, "%s/%s", DIR_CONFIG, MCUDEBUGFILENAME );
	GetRegKeyInt( achFullName, "mcuWatchDog", "mcuWatchDogEnable", 1, ( s32* )&dwWDEnable );

	return (u8) dwWDEnable;
}

/*=============================================================================
�� �� ���� PackEqpConfig
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CEqpConfigBuf &cEqpCfgBuf
           TBoardInfo tBrdInfo
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/18   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CBoardManager::PackEqpConfig(CEqpConfigBuf &cEqpCfgBuf, TBoardInfo tBrdInfo)
{
	TEqpTVWallEntry tTVWallCfg;
	TEqpMixerEntry tMixerCfg;
	TEqpBasEntry tBasCfg;
	TEqpVMPEntry tVMPCfg;
    TEqpMPWEntry tMpwCfg;
	TEqpPrsEntry tPrsCfg;
	//4.6�汾 �¼����� jlb
	TEqpHduEntry tHduCfg;
    //TEqpDvmpBasicEntry tDvmpBasicCfg;
	//TEqpMpuBasEntry tMpuBasCfg;
	//TEqpEbapEntry tEbapCfg;
	//TEqpEvpuEntry tEvpuCfg;

	u8 byPeriCount = 0;
	u8 abyPeriId[MAXNUM_BRD_EQP] = {0};
	u8 byPeriType = 0;
	u8 byLoop = 0;

	//��Ϣ�ṹ  �����ϵ�������� + ��������������Ϣ  + (DRI DTI)��ͬ������ + ����IP + ���Ź�
    tBrdInfo.GetPeriId( abyPeriId, &byPeriCount );
    if ( byPeriCount > MAXNUM_BRD_EQP )
    {
        Agtlog( LOG_ERROR, "[PackEqpConfig]Peri id over flow, %d !\n\n", byPeriCount );
        return FALSE;
    }

    // �Ƿ��ÿ��Ź�
    cEqpCfgBuf.SetUseWatchDog( GetWatchDogOption() );
    // �����а�Ip��ַ
    cEqpCfgBuf.SetIpAddr(  htonl(tBrdInfo.GetBrdIp()));

    // guzh [1/17/2007] ����� 8000B�����xDSC���ʵ��������IP���ظ���
    //cCfgBuf.SetIpAddr( OspNodeIpGet(pcMsg->srcnode) );

    // ������Ϣ
	for( byLoop = 0; byLoop < byPeriCount; byLoop ++ )
	{
		// ͨ������ID�õ�������
        u32 dwPeriIp = 0;
        u16 wRet = g_cCfgParse.GetPriInfo( abyPeriId[byLoop], &dwPeriIp, &byPeriType );
        if( SUCCESS_AGENT != wRet )
        {
            Agtlog(LOG_ERROR, "[PackEqpConfig] The Peri Id %d for<%d,%d:%s>is not exist\n",
                               abyPeriId[byLoop], tBrdInfo.GetLayer(), 
                               tBrdInfo.GetSlot(), g_cCfgParse.GetBrdTypeStr(tBrdInfo.GetType()) );
            return FALSE;
        }

		// �ҵ���Ӧ��������Ϣ
		switch( byPeriType )
		{
		case EQP_TYPE_TVWALL:
			if( g_cCfgParse.GetTvCfgToBrd( abyPeriId[byLoop], &tTVWallCfg ) )
			{
                cEqpCfgBuf.AddEqpCfg(tTVWallCfg);
			}
			else
			{
				Agtlog( LOG_ERROR, "[PackEqpConfig] Get TVWall Config Error!\n"); 
				return FALSE;					
			}
			break;
			
		case EQP_TYPE_MIXER:
			if( g_cCfgParse.GetMixCfgToBrd( abyPeriId[byLoop], &tMixerCfg ) )
			{
				u8 byMixerSubType = UNKONW_MIXER;
				if(g_cMcuAgent.GetMixerSubTypeByEqpId(tMixerCfg.GetEqpId(),byMixerSubType))
				{
					if(byMixerSubType == EAPU_MIXER)
					{
						tMixerCfg.SetType(12); //[2/14/2012 chendaiwei]ԭEQP_TYPE_EMIXERֵΪ12���˴�����V4R7��ǰEAPU
					}
				}
                cEqpCfgBuf.AddEqpCfg(tMixerCfg);
			}
			else
			{
				Agtlog( LOG_ERROR, "[PackEqpConfig] Get Mixer Config Error!\n"); 
				return FALSE;					
			}
			break;
			
		case EQP_TYPE_VMP:
            if ( BRD_TYPE_MPU/*BRD_HWID_DSL8000_MPU*/ != tBrdInfo.GetType() &&
                 BRD_HWID_EBAP        != tBrdInfo.GetType() &&
                 BRD_HWID_EVPU        != tBrdInfo.GetType() &&
				 BRD_TYPE_MPU2   != tBrdInfo.GetType() &&
				 BRD_TYPE_MPU2ECARD   != tBrdInfo.GetType())
            {
				if( g_cCfgParse.GetVmpCfgToBrd( abyPeriId[byLoop], &tVMPCfg ) )
			    {
                    cEqpCfgBuf.AddEqpCfg(tVMPCfg);
			    }
			    else
			    {
				    Agtlog( LOG_ERROR, "[PackEqpConfig] Get VMP Config Error!\n"); 
				    return FALSE;					
			    }
            }
			break;

        case EQP_TYPE_VMPTW:
            if( g_cCfgParse.GetMpwCfgToBrd(abyPeriId[byLoop], &tMpwCfg) )
            {
                cEqpCfgBuf.AddEqpCfg(tMpwCfg);
            }
            else
            {
                Agtlog( LOG_ERROR, "[PackEqpConfig] Get Mpw Config Error!\n"); 
				return FALSE;
            }
            break;

		case EQP_TYPE_BAS:
			if( BRD_TYPE_APU2 == tBrdInfo.GetType())
			{
				TEqpMpuBasEntry tApu2BasCfg;
				if(g_cCfgParse.GetMpuBasEntry(abyPeriId[byLoop],tApu2BasCfg))
				{
					cEqpCfgBuf.AddEqpCfg(tApu2BasCfg);
				}
				else
				{
					Agtlog( LOG_ERROR, "[PackEqpConfig] Get apu2 Bas Config Error!\n"); 
					return FALSE;					
				}
			}
            else if ( BRD_TYPE_MPU/*BRD_HWID_DSL8000_MPU*/ != tBrdInfo.GetType() &&
                 BRD_HWID_EBAP        != tBrdInfo.GetType() &&
                 BRD_HWID_EVPU        != tBrdInfo.GetType() &&
				 BRD_TYPE_MPU2   != tBrdInfo.GetType() &&
				 BRD_TYPE_MPU2ECARD   != tBrdInfo.GetType())
            {
			if( g_cCfgParse.GetBasCfgToBrd( abyPeriId[byLoop], &tBasCfg ) )
			{
                cEqpCfgBuf.AddEqpCfg(tBasCfg);
			}
			else
			{
				Agtlog( LOG_ERROR, "[PackEqpConfig] Get Bas Config Error!\n"); 
				return FALSE;					
			}
            }
			break;

		case EQP_TYPE_PRS:
			if( g_cCfgParse.GetPrsCfgToBrd( abyPeriId[byLoop], &tPrsCfg ) )
			{
                cEqpCfgBuf.AddEqpCfg(tPrsCfg);
			}
			else
			{
				Agtlog( LOG_ERROR, "[PackEqpConfig] Get Prs Config Error!\n"); 
				return FALSE;					
			}
			break;
        //4.6�汾 �¼� jlb
		case EQP_TYPE_HDU:
// 		case EQP_TYPE_HDU_H:
// 		case EQP_TYPE_HDU_L:
// 		case EQP_TYPE_HDU2:
			if( g_cCfgParse.GetHduCfgToBrd( abyPeriId[byLoop], &tHduCfg ) )
			{
				switch(tHduCfg.GetStartMode())
				{
				case STARTMODE_HDU_L:
					tHduCfg.SetType(14); //ԭ���壬�����ϰ汾HDU�汾��#define EQP_TYPE_HDU_L 14
					break;
				case STARTMODE_HDU_H:
					tHduCfg.SetType(13); //ԭ���壬�����ϰ汾HDU�汾��#define EQP_TYPE_HDU_H 13
					break;
				case STARTMODE_HDU2:
					tHduCfg.SetType(15); //ԭ���壬�����ϰ汾HDU�汾��#define EQP_TYPE_HDU2 15
					break;
				case STARTMODE_HDU2_L:
					tHduCfg.SetType(12); //ԭ���壬�����ϰ汾HDU�汾��#define EQP_TYPE_HDU2_L 12
					break;
				default:
					tHduCfg.SetType(EQP_TYPE_HDU);
					break;
				}

                cEqpCfgBuf.AddEqpCfg(tHduCfg);
			}
			else
			{
				Agtlog( LOG_ERROR, "[PackEqpConfig] Get Hdu Config Error!\n"); 
				return FALSE;					
			}
			break;
			
		default:  
			Agtlog( LOG_ERROR, "[PackEqpConfig] Get Peri Type Error!\n"); 
			return FALSE;
		}
	}
			
// �������� DTI DRI����ͬ������
#ifndef WIN32
	switch( tBrdInfo.GetType() )
	{
	case BRD_TYPE_DTI/*DSL8000_BRD_DTI*/:
	case BRD_TYPE_DRI/*DSL8000_BRD_DRI*/:
		
	//  [1/21/2011 chendaiwei]֧��DRI2
	case BRD_TYPE_DRI2:
	case BRD_TYPE_16E1/*DSL8000_BRD_16E1*/:
	case BRD_TYPE_DSI/*DSL8000_BRD_DSI*/:
		{
			// �Ƿ�����ͬ��ʹ�õĵ���
			TNetSyncInfo tNetSyncInfo;
			g_cCfgParse.GetNetSyncInfo(&tNetSyncInfo);
			if( NETSYNTYPE_TRACE == tNetSyncInfo.GetMode())
			{
				u8 byBrdId = tNetSyncInfo.GetDTSlot();
				u8 byE1Index = tNetSyncInfo.GetE1Index();
				if( byBrdId == tBrdInfo.GetSlot() )
				{
					cEqpCfgBuf.SetUseNetSync( TRUE, byE1Index );
				}
			}                
		}
		break;
	default:
		break;
	}
#endif

    // 8000B ����DSC������Ϣ
#ifdef _MINIMCU_
    TDSCModuleInfo tInfo;
    g_cCfgParse.GetDSCInfo(&tInfo);
	tInfo.Print();
    cEqpCfgBuf.SetDscModuleInfo( tInfo );
#endif

#ifdef _MINIMCU_
	TNetWorkInfo  tNetWorkInfo;
	g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
	u32 dwGKIpAddr = tNetWorkInfo.GetGkIp();
	OspPrintf(TRUE, FALSE, "[GETCONFIG] Gk ip = 0x%x\n", dwGKIpAddr );
	cEqpCfgBuf.SetGKIp( dwGKIpAddr );
#endif  

#ifdef _MINIMCU_
	//ȡ����LoginInfo, �ȴ�NIP�ӿ�, zgc, 2007-10-12
#ifndef WIN32	
#endif
	TLoginInfo tLoginInfo;		//zgc, 2007-10-12
	cEqpCfgBuf.SetLoginInfo( tLoginInfo );
#endif	 

	return TRUE;
}


/*====================================================================
    ������      ��GetMpuEBapEVpuEqpInfo
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
    06/08/11    4.0         ����        �������
====================================================================*/
void CBoardManager::GetMpuEBapEVpuEqpInfo( TBoardInfo &tBoardInfo, TMpuEBapEVpuConfigAck &tMpuEBapEVpuConfigAck )
{
	u8 byPeriCount = 0;
	u8 abyPeriId[MAXNUM_BRD_EQP] = {0};
	tBoardInfo.GetPeriId( abyPeriId, &byPeriCount );

    if ( g_cCfgParse.IsSVmp(abyPeriId[0]) )
    {
        TEqpSvmpInfo tEqpSvmpInfo;
        g_cCfgParse.GetEqpSvmpCfgById( abyPeriId[0], &tEqpSvmpInfo );
        tMpuEBapEVpuConfigAck.SetEqpRcvStartPort( tEqpSvmpInfo.GetEqpRecvPort() );

        tMpuEBapEVpuConfigAck.SetEqpType( tEqpSvmpInfo.GetVmpType() );
        tMpuEBapEVpuConfigAck.SetEqpId( abyPeriId[0] );
    }
	
    if ( g_cCfgParse.IsMpuBas(abyPeriId[0]) )
    {
		//��ȡ�����ļ���mpubas������ģʽ
		TEqpMpuBasEntry tMpuBasEntry;
		if (!g_cCfgParse.GetMpuBasEntry(abyPeriId[0], tMpuBasEntry))
		{
			OspPrintf(TRUE, FALSE, "[GetMpuEBapEVpuEqpInfo] GetMpuBasEntry failed <EqpId:%d>!\n", abyPeriId[0]);
		}
        tMpuEBapEVpuConfigAck.SetEqpType( tMpuBasEntry.GetStartMode()/*TYPE_MPUBAS*/ );
        tMpuEBapEVpuConfigAck.SetEqpId( abyPeriId[0] );
		
        TEqpMpuBasInfo tEqpMpuBasInfo;
        g_cCfgParse.GetEqpMpuBasCfgById( abyPeriId[0], &tEqpMpuBasInfo );
        tMpuEBapEVpuConfigAck.SetEqpRcvStartPort( tEqpMpuBasInfo.GetEqpRecvPort() );
    }

	if (byPeriCount > 1)
	{
		if ( g_cCfgParse.IsSVmp(abyPeriId[1]) )
		{
			TEqpSvmpInfo tEqpSvmpInfo;
			g_cCfgParse.GetEqpSvmpCfgById( abyPeriId[1], &tEqpSvmpInfo );
			tMpuEBapEVpuConfigAck.SetEqpRcvStartPort2( tEqpSvmpInfo.GetEqpRecvPort() );
			
			tMpuEBapEVpuConfigAck.SetEqpType( tEqpSvmpInfo.GetVmpType() );
			tMpuEBapEVpuConfigAck.SetEqpId2( abyPeriId[1] );
		}

		if ( g_cCfgParse.IsMpuBas(abyPeriId[1]) )
		{
			//��ȡ�����ļ���mpubas������ģʽ
			TEqpMpuBasEntry tMpuBasEntry;
			if (!g_cCfgParse.GetMpuBasEntry(abyPeriId[1], tMpuBasEntry))
			{
				OspPrintf(TRUE, FALSE, "[GetMpuEBapEVpuEqpInfo] GetMpuBasEntry failed <EqpId:%d>!\n", abyPeriId[0]);
			}
			tMpuEBapEVpuConfigAck.SetEqpType( tMpuBasEntry.GetStartMode()/*TYPE_MPUBAS*/ );
			tMpuEBapEVpuConfigAck.SetEqpId2( abyPeriId[1] );
			
			TEqpMpuBasInfo tEqpMpuBasInfo;
			g_cCfgParse.GetEqpMpuBasCfgById( abyPeriId[1], &tEqpMpuBasInfo );
			tMpuEBapEVpuConfigAck.SetEqpRcvStartPort2( tEqpMpuBasInfo.GetEqpRecvPort() );
		}
	}
}

/*====================================================================
    ������      ��ProcBoardGetConfigMsg
    ����        ���������ȡ������Ϣ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
    06/08/11    4.0         ����        �������
====================================================================*/
void CBoardManager::ProcBoardGetConfigMsg(CMessage* const pcMsg)
{
    if( NULL == pcMsg )
    {
        Agtlog(LOG_ERROR, "[BoardManager] The pointer can not be Null\n");
        return;
    }

    TBrdPos	*ptBrdPos = (TBrdPos*)pcMsg->content;
    TBrdPosition tBrdPosition;
	memset(&tBrdPosition, 0x0, sizeof(tBrdPosition));
	tBrdPosition.byBrdID = ptBrdPos->byBrdID;
	tBrdPosition.byBrdLayer = ptBrdPos->byBrdLayer;
	tBrdPosition.byBrdSlot = ptBrdPos->byBrdSlot;

    CEqpConfigBuf cCfgBuf;

	u8 byPeriCount = 0;
    u8 abyPeriId[MAXNUM_BRD_EQP] = {0};
	u8 byPeriType = 0;
	u8 byLoop = 0;

	//��Ϣ�ṹ  �����ϵ�������� + ��������������Ϣ  + (DRI DTI)��ͬ������ + ����IP + ���Ź�
	switch( CurState() ) 
	{
	case STATE_INIT:
    {
		// ��֤�Ƿ�ͱ���ĵ�����Ϣһ��
//        if ( 0 != memcmp( &tBrdPosition, &m_tBrdPosition, sizeof(TBrdPosition) ) )
		if( tBrdPosition.byBrdID    != m_tBrdPosition.byBrdID ||
			tBrdPosition.byBrdLayer != m_tBrdPosition.byBrdLayer ||
			tBrdPosition.byBrdSlot  != m_tBrdPosition.byBrdSlot )
		{
            Agtlog(LOG_ERROR, "[ProcBoardGetConfigMsg] Board <%d,%d:%s> get config conflict with <%d,%d:%s>!\n", 
				               tBrdPosition.byBrdLayer, tBrdPosition.byBrdSlot,
                               g_cCfgParse.GetBrdTypeStr(tBrdPosition.byBrdID),
                               m_tBrdPosition.byBrdLayer, m_tBrdPosition.byBrdSlot,
                               g_cCfgParse.GetBrdTypeStr(m_tBrdPosition.byBrdID));
            
			PostMsgToBoard( MPC_BOARD_GET_CONFIG_NACK, NULL, 0 );
			::OspDisconnectTcpNode( pcMsg->srcnode );
			return;			
		}

        TBoardInfo tBrdInfo;
        u16 wRet = g_cCfgParse.GetBrdCfgById( g_cCfgParse.GetBrdId(m_tBrdPosition), &tBrdInfo );
        if ( SUCCESS_AGENT != wRet )
        {
            Agtlog(LOG_ERROR, "[ProcBoardGetConfigMsg] GetBrdCfgById failed !\n");
            return;
        }

		PackEqpConfig( cCfgBuf, tBrdInfo );
	
        //  ��������
        u16 wLen = 0;
        u8* pbyBuf = cCfgBuf.GetBuffer(wLen);
        if ( BRD_TYPE_MPU/*BRD_HWID_DSL8000_MPU*/ != tBrdInfo.GetType() &&
             BRD_HWID_EBAP        != tBrdInfo.GetType() &&
             BRD_HWID_EVPU        != tBrdInfo.GetType() &&
			 BRD_TYPE_MPU2   != tBrdInfo.GetType() &&
			 BRD_TYPE_MPU2ECARD   != tBrdInfo.GetType() )
        {
			if ( BRD_TYPE_HDU/*BRD_HWID_KDM200_HDU*/ != tBrdInfo.GetType() 
				 && BRD_TYPE_HDU_L != tBrdInfo.GetType()
				 && BRD_TYPE_HDU2 != tBrdInfo.GetType()
				 && BRD_TYPE_HDU2_L != tBrdInfo.GetType()
				 && BRD_TYPE_HDU2_S != tBrdInfo.GetType())
			{
				PostMsgToBoard(MPC_BOARD_GET_CONFIG_ACK, pbyBuf, wLen );
			}
			else
			{
				u8 byLen = 0;
				u8 abyBuffer[1024];
				memset( abyBuffer, 0x0, sizeof(abyBuffer) );
                abyBuffer[byLen++] = cCfgBuf.GetEqpCfgNum();
				for (s32 nIndex=0; nIndex<cCfgBuf.GetEqpCfgNum(); nIndex++)
				{
                    memcpy( &abyBuffer[byLen], &cCfgBuf.m_utCfg[nIndex].tHduCfg, sizeof(TEqpHduEntry) );
				    byLen += sizeof(TEqpHduEntry);
				}
		        PostMsgToBoard( MPC_BOARD_GET_CONFIG_ACK, abyBuffer, sizeof(abyBuffer) ); 
			}
        }
        else
        {
            TMpuEBapEVpuConfigAck tMpuEBapEVpuConfigAck;
            memset( &tMpuEBapEVpuConfigAck, 0, sizeof(TMpuEBapEVpuConfigAck) );
            GetMpuEBapEVpuEqpInfo( tBrdInfo, tMpuEBapEVpuConfigAck );
            PostMsgToBoard( MPC_BOARD_GET_CONFIG_ACK, (u8*)&tMpuEBapEVpuConfigAck, sizeof(TMpuEBapEVpuConfigAck) );
        }

	    // ������, zgc, 2007-10-25
		if ( g_byPrintLvl > LOG_INFORM )
		{
			OspPrintf(TRUE, FALSE, "[GetCfgMsg] CfgBuf len = %d\n", wLen );
			for ( s32 n = 0; n < wLen; n++ )
			{
				OspPrintf(TRUE, FALSE, "%x ", *(pbyBuf+n) );
				if ( (n+1)%20 == 0 )
				{
					OspPrintf(TRUE, FALSE, "\n" );
				}
			} 
		}
                
		// ����NORMAL״̬
		NEXTSTATE( STATE_NORMAL );
		break;
    }
	default:
		Agtlog(LOG_WARN, "[ProcBoardGetConfigMsg] Wrong message %u(%s) received in current state %u!\n", 
			    pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        Agtlog(LOG_WARN, "[ProcBoardGetConfigMsg] the node: %d, Ip: %0x, the inst: %d\n", 
                pcMsg->srcnode, OspNodeIpGet(pcMsg->srcnode), GetInsID());
		break;
	}

	return;
}


/*====================================================================
    ������      ��ProcBoardAlarmSyncAck
    ����        ��������ĸ澯ͬ��Ӧ����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBoardManager::ProcBoardAlarmSyncAck(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
    {
        Agtlog(LOG_ERROR, "[ProcBoardAlarmSyncAck] The pointer can not be Null\n");
        return;
    }
    u16 wLoop = 0;
	// ����TBrdPosition
//	TBrdPosition *ptBrdPos = (TBrdPosition*)pcMsg->content;
	TBrdPos *ptBrdPosChange = (TBrdPos*)pcMsg->content;
	TBrdPosition tBrdPos;
	tBrdPos.byBrdID    = ptBrdPosChange->byBrdID;
	tBrdPos.byBrdLayer = ptBrdPosChange->byBrdLayer;
	tBrdPos.byBrdSlot  = ptBrdPosChange->byBrdSlot;
	// ���Ϊ�澯�����
	u16 wCount = ntohs( *(u16*)(pcMsg->content + sizeof(TBrdPos)) );
	// �����Ϊ�澯��
	TBoardAlarmMsgInfo* ptBoardAlarm = (TBoardAlarmMsgInfo*)
		( pcMsg->content + sizeof(TBrdPos) + sizeof(wCount) );
	
	switch( CurState() ) 
	{
    case STATE_INIT:
	case STATE_NORMAL:
		// ��֤�Ƿ�ͱ���ĵ�����Ϣһ��
//        if ( 0 != memcmp( ptBrdPos, &m_tBrdPosition, sizeof(TBrdPosition) ) )		
		if( tBrdPos.byBrdID    != m_tBrdPosition.byBrdID ||
			tBrdPos.byBrdLayer != m_tBrdPosition.byBrdLayer ||
			tBrdPos.byBrdSlot  != m_tBrdPosition.byBrdSlot )
        {
            Agtlog(LOG_ERROR, "[ProcBoardAlarmSyncAck] Board <%d,%d:%s> Alarm Ack Conflict!\n", 
			                   tBrdPos.byBrdLayer,tBrdPos.byBrdSlot,
                               g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID));
			::OspDisconnectTcpNode( pcMsg->srcnode );
			return;			
		}
		
		if( g_cCfgParse.HasSnmpNms() )
		{
			// ���澯���е�ÿһ����뵽MPC�ĸ澯����
			for( wLoop = 0; wLoop<wCount; wLoop++)
			{
				post( MAKEIID(AID_MCU_AGENT, 1), ntohs( ptBoardAlarm->wEventId ), 
					  ptBoardAlarm->abyAlarmContent,sizeof(ptBoardAlarm->abyAlarmContent) );
				ptBoardAlarm++;
			}
		}
		break;

	default:
		Agtlog(LOG_WARN, "[ProcBoardAlarmSyncAck]  Wrong message %u(%s) received in current state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}		
	return;
}


/*====================================================================
    ������      ��ProcBoardGetVersionAck
    ����        ��ȡ����İ汾��ϢӦ����Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBoardManager::ProcBoardGetVersionAck(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
    {
        Agtlog(LOG_ERROR, "[ProcBoardGetVersionAck] The pointer can not be Null\n");
        return;
    }
    // ����TBrdPosition
//	TBrdPosition *ptBrdPos = (TBrdPosition*)pcMsg->content;

	// [pengjie 2010/3/25]
	TBrdPos *ptBrdPosChange = (TBrdPos*)pcMsg->content;
	TBrdPosition tBrdPos;
	tBrdPos.byBrdID    = ptBrdPosChange->byBrdID;
	tBrdPos.byBrdLayer = ptBrdPosChange->byBrdLayer;
	tBrdPos.byBrdSlot  = ptBrdPosChange->byBrdSlot;

	Agtlog( LOG_VERBOSE, "[McuAgt_Debug][ProcBoardGetVersionAck] id: %d, Layer: %d, Slot: %d \n",
			ptBrdPosChange->byBrdID,  ptBrdPosChange->byBrdLayer, ptBrdPosChange->byBrdSlot );
	Agtlog( LOG_VERBOSE, "[McuAgt_Debug][ProcBoardGetVersionAck] change to, id: %d, Layer: %d, Slot: %d \n",
		tBrdPos.byBrdID, tBrdPos.byBrdLayer, tBrdPos.byBrdSlot);
	

	// ��Ϣ�ṹΪ�汾��Ϣ(�ַ���,��ʽ: ģ����:�汾|ģ����:�汾|...)
	u8* pbyVersion = pcMsg->content + sizeof(TBrdPos) ;

    u8 byBrdId = g_cCfgParse.GetBrdId(m_tBrdPosition);
    TEqpBrdCfgEntry tBrdInfo;
    memset( &tBrdInfo, 0, sizeof(tBrdInfo) );
    g_cCfgParse.GetBrdInfoById( byBrdId, &tBrdInfo );
    
	switch( CurState() ) 
	{
    case STATE_INIT:
	case STATE_NORMAL:
		// ��֤�Ƿ�ͱ���ĵ�����Ϣһ��
//        if ( 0 != memcmp( ptBrdPos, &m_tBrdPosition, sizeof(TBrdPosition)) )
		if( tBrdPos.byBrdID    != m_tBrdPosition.byBrdID ||
			tBrdPos.byBrdLayer != m_tBrdPosition.byBrdLayer ||
			tBrdPos.byBrdSlot  != m_tBrdPosition.byBrdSlot )
		{
            Agtlog( LOG_ERROR, "BoardManager: Board <%d,%d:%s> Version Ack Conflict!\n", 
				                tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                                g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID));
            Agtlog( LOG_ERROR, "BoardManager: m_tBrdPosition: <%d,%d:%s> !\n", 
				                m_tBrdPosition.byBrdLayer, m_tBrdPosition.byBrdSlot,
                                g_cCfgParse.GetBrdTypeStr(m_tBrdPosition.byBrdID));
			::OspDisconnectTcpNode( pcMsg->srcnode );
			return;			
		}

		// ֱ�ӽ��汾��Ϣ��ŵ�������ؽṹ
        tBrdInfo.SetVersion( (s8*)pbyVersion );
        g_cCfgParse.SetBrdInfoById( byBrdId, &tBrdInfo );
		break;

	default:
		Agtlog(LOG_WARN, "[ProcBoardGetVersionAck] Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	Agtlog( LOG_VERBOSE, "[ProcBoardGetVersionAck] TBrdVersion : %s\n",
		tBrdInfo.GetVersion());

	return;
}

/*====================================================================
    ������      ��ProcBoardGetModuleAck
    ����        ��ȡ�����ģ����ϢӦ����Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBoardManager::ProcBoardGetModuleAck(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
    {
        Agtlog(LOG_ERROR, "[ProcBoardGetModuleAck] The pointer can not be Null\n");
        return;
    }
    // ����TBrdPosition
//	TBrdPosition *ptBrdPos = (TBrdPosition*)pcMsg->content;

	// [pengjie 2010/3/25]
	TBrdPos *ptBrdPosChange = (TBrdPos*)pcMsg->content;
	TBrdPosition tBrdPos;
	tBrdPos.byBrdID    = ptBrdPosChange->byBrdID;
	tBrdPos.byBrdLayer = ptBrdPosChange->byBrdLayer;
	tBrdPos.byBrdSlot  = ptBrdPosChange->byBrdSlot;

	Agtlog( LOG_VERBOSE, "[McuAgt_Debug][ProcBoardGetModuleAck] id: %d, Layer: %d, Slot: %d \n",
		ptBrdPosChange->byBrdID,  ptBrdPosChange->byBrdLayer, ptBrdPosChange->byBrdSlot );
	Agtlog( LOG_VERBOSE, "[McuAgt_Debug][ProcBoardGetModuleAck] change to, id: %d, Layer: %d, Slot: %d \n",
		tBrdPos.byBrdID, tBrdPos.byBrdLayer, tBrdPos.byBrdSlot);


	// ��Ϣ�ṹΪʹ�õ�ģ��� ��ʽ:[M1|M2|..]
	u8* pbyModule = pcMsg->content + sizeof(TBrdPos) ;

    u8 byBrdId = g_cCfgParse.GetBrdId(m_tBrdPosition);
    TEqpBrdCfgEntry tBrdInfo;
    memset( &tBrdInfo, 0, sizeof(tBrdInfo) );
    g_cCfgParse.GetBrdInfoById( byBrdId, &tBrdInfo );

	switch( CurState() ) 
	{
    case STATE_INIT:
	case STATE_NORMAL:
		// ��֤�Ƿ�ͱ���ĵ�����Ϣһ��
//        if ( 0 != memcmp( ptBrdPos, &m_tBrdPosition, sizeof(TBrdPosition)) )	
		if( tBrdPos.byBrdID    != m_tBrdPosition.byBrdID ||
			tBrdPos.byBrdLayer != m_tBrdPosition.byBrdLayer ||
			tBrdPos.byBrdSlot  != m_tBrdPosition.byBrdSlot )		
        {
            Agtlog(LOG_ERROR, "[ProcBoardGetModuleAck] Board <%d,%d:%s> Module Ack Conflict!\n", 
				               tBrdPos.byBrdLayer,tBrdPos.byBrdSlot,
                               g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID));
			Agtlog( LOG_ERROR, "BoardManager: m_tBrdPosition: <%d,%d:%s> !\n", 
				               m_tBrdPosition.byBrdLayer, m_tBrdPosition.byBrdSlot,
                                g_cCfgParse.GetBrdTypeStr(m_tBrdPosition.byBrdID));
			::OspDisconnectTcpNode( pcMsg->srcnode );
			return;			
		}

		// ֱ�ӽ��汾��Ϣ��ŵ�������ؽṹ
        tBrdInfo.SetModule( (s8*)pbyModule );
        g_cCfgParse.SetBrdInfoById( byBrdId, &tBrdInfo );
		break;

	default:
		Agtlog(LOG_WARN, "[ProcBoardGetModuleAck] Wrong message %u(%s) received in current state %u!\n", 
			               pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*=============================================================================
  �� �� ���� ProcBoardUpdateRsp
  ��    �ܣ� ������������Ӧ( 0:ʧ�ܣ�1:���� )
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardManager::ProcBoardUpdateRsp(CMessage* const pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    u8 byBrdId   = *(u8*)cServMsg.GetMsgBody();
    u8 byFileNum = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8));
    u8 *pbyResult = (u8*)(cServMsg.GetMsgBody() + sizeof(u8) * 2);
    
    // ���ܷ���ĵ�������
    if ( UPDATE_NMS == cServMsg.GetChnIndex() )
    {
        TBoardInfo tBrdInfo;
        g_cCfgParse.GetBrdCfgById( byBrdId, &tBrdInfo );

        // ����LINUX�ĵ���״̬����
        if ( OS_TYPE_LINUX != tBrdInfo.GetOSType() ) 
        {
            Agtlog(LOG_INFORM, "[ProcBoardUpdateRsp] brd<%d,%d:%s> is not linux, ignore !\n",
                                tBrdInfo.GetLayer(), tBrdInfo.GetSlot(), g_cCfgParse.GetBrdTypeStr(tBrdInfo.GetType()) );
            return;
        }

        u16 wMsgLen = 0;
        u16 wOffSet = 0;
        s8  achFileName[256];
        s8  achMsgBody[256];
        memset(achMsgBody, 0, sizeof(achMsgBody));
        s8* lpszMsgBody = &achMsgBody[0];

        u8  byLen = 0;

        wMsgLen = cServMsg.GetMsgBodyLen() - (2+byFileNum);
        memcpy(lpszMsgBody, cServMsg.GetMsgBody()+(2+byFileNum), wMsgLen);
        
        while (byFileNum-- > 0) 
        {                
            memset( achFileName, 0, sizeof(achFileName) );
            byLen = *lpszMsgBody;
            if (byLen + wOffSet > wMsgLen)
            {
                Agtlog(LOG_ERROR, "[ProcBoardUpdateRsp] invalid msgbody, byLen: %d, wOffSet: %d, wMsgLen: %d!\n",
                                   byLen, wOffSet, wMsgLen );
                break;
            }
            else
            {
                Agtlog(LOG_INFORM, "[ProcBoardUpdateRsp] msgbody valid, byLen: %d, wOffSet: %d, wMsgLen: %d!\n",
                                    byLen, wOffSet, wMsgLen );
            }
            
            s8 *pszFileName = lpszMsgBody+sizeof(u8);
            memcpy(achFileName, pszFileName, byLen+1);
            achFileName[byLen] = '\0';
            wOffSet = sizeof(byLen) + byLen;
            lpszMsgBody += wOffSet;
            Agtlog(LOG_INFORM, "[ProcBoardUpdateRsp] filename: %s\n", achFileName);

            u8 byStatus = SNMP_FILE_UPDATE_INPROCESS;
            if ( *pbyResult == 0 )
            {
                byStatus = SNMP_FILE_UPDATE_FAILED;
            }
            else
            {
                byStatus = SNMP_FILE_UPDATE_SUCCEED;
            }
            pbyResult ++;

            //�ж��Ƿ�image�ļ�
            s32 nNameOff = strlen(achFileName) - strlen(LINUXAPP_POSTFIX);
            if (nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, LINUXAPP_POSTFIX) )
            {
                g_cBrdManagerApp.SetImageFileStatus(m_tBrdPosition, byStatus);
            }
            else
            {
                //�ж��Ƿ�os
                nNameOff = strlen(achFileName) - strlen(LINUXOS_POSTFIX);
                if (nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, LINUXOS_POSTFIX) )
                {
                    g_cBrdManagerApp.SetOSFileStatus(m_tBrdPosition, byStatus);
                }   
                else
                {
					
					//�ж��Ƿ�bin
					nNameOff = strlen(achFileName) - strlen(LINUXAPP_POSTFIX_BIN);
					if ( nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, LINUXAPP_POSTFIX_BIN) )
					{
						g_cBrdManagerApp.SetBinFileStatus(m_tBrdPosition, byStatus);
					}
					else
					{
						Agtlog(LOG_ERROR, "[ProcBoardUpdateRsp] not supported file type.\n" );
					}
                }
            }
        }
    }
    else
    {
        // VX,LINUX,WIN32����һ�£�u8(brd ID)+u8(FileNum)+u8[FileNum](�ɹ����) [06/11/24-zbq]
        u8 abyStrBuf[256];
        memset( abyStrBuf, 0, sizeof(abyStrBuf));
        ChangeNumBufToStrBuf(cServMsg.GetMsgBody(), byFileNum + 2, abyStrBuf);
        cServMsg.SetMsgBody(abyStrBuf, byFileNum + 2);

        while ( byFileNum -- > 0)
        {
            Agtlog( LOG_INFORM, "Recv board<%d,%d:%s> upate result: %d\n", 
                                 m_tBrdPosition.byBrdLayer, 
                                 m_tBrdPosition.byBrdSlot, 
                                 g_cCfgParse.GetBrdTypeStr(m_tBrdPosition.byBrdID), *pbyResult-'0');
            pbyResult += sizeof(u8);
        }
        SendMsgToMcuCfg( AGT_SVC_UPDATEBRDVERSION_NOTIF, 
                         cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }
        
	return;
}

/*====================================================================
    ������      ��ProcBoardE1BandWidthNotif
    ����        ��������E1����ָʾ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    11/12/21    1.0         ��ʤ��          ����
====================================================================*/
void CBoardManager::ProcBoardE1BandWidthNotif( CMessage* const pcMsg )
{

    //CServMsg cServMsg( pcMsg->content, pcMsg->length );
    //u16 wDriId = cServMsg.GetSrcDriId();

    Agtlog(LOG_WARN, "[ProcBoardE1BandWidthNotif] Dri2 E1 Band Width Change!\n");

    u16 wBandWidthEachPort[MAXNUM_E1PORT];
    memset(wBandWidthEachPort, 0,  sizeof(wBandWidthEachPort));
    memcpy( wBandWidthEachPort, pcMsg->content, sizeof(wBandWidthEachPort));

    u8 byPortE1Num[MAXNUM_E1PORT];
    memset(byPortE1Num, 0, sizeof(byPortE1Num));
    memcpy( byPortE1Num, pcMsg->content + sizeof(wBandWidthEachPort), sizeof(byPortE1Num));

    u32 dwDri2Ip = *(u32*)(pcMsg->content + sizeof(wBandWidthEachPort) + sizeof(byPortE1Num));

    u8 byBuf[sizeof(wBandWidthEachPort) + sizeof(byPortE1Num) + sizeof(dwDri2Ip) + 10];
    memset(byBuf, 0, sizeof(byBuf));
    memcpy( byBuf, wBandWidthEachPort, sizeof(wBandWidthEachPort));
    memcpy( byBuf + sizeof(wBandWidthEachPort), byPortE1Num, sizeof(byPortE1Num));
    memcpy( byBuf + sizeof(wBandWidthEachPort) + sizeof(byPortE1Num), &dwDri2Ip, sizeof(dwDri2Ip));
    OspPost(MAKEIID(AID_MCU_VC, CInstance::DAEMON/*EACH*/), AGT_SVC_E1BANDWIDTH_NOTIF, byBuf, sizeof(wBandWidthEachPort) + sizeof(byPortE1Num) + sizeof(dwDri2Ip));
    
//     TDri2E1Cfg tDri2E1Cfg[8];
//     g_cCfgParse.GetDri2E1Cfg( tDri2E1Cfg );
//     
//     for ( u8 byInx = 0; byInx < 8; byInx++)
//     {
//         if ( tDri2E1Cfg[byInx].GetDriIp() == dwDri2Ip)
//         {
//             u8 byBuf[sizeof(u32) + sizeof(u16) + 10];
//             memset(byBuf, 0, sizeof(byBuf));
//             u32 dwIpAddr = tDri2E1Cfg[byInx].GetIpAddr();
//             dwIpAddr = htonl(dwIpAddr);
//             u16 wBandWidth = htons(wBandWidthEachPort[byInx]);
//             //u8 byE1Num = byPortE1Num[byInx];
//             memcpy( byBuf, &dwIpAddr, sizeof(u32));
//             memcpy( byBuf + sizeof(u32), &wBandWidth, sizeof(u16));
//             memcpy( byBuf + sizeof(u32) + sizeof(u16), &byPortE1Num[byInx], sizeof(u8));
// 
//             OspPost(MAKEIID(AID_MCU_VC, CInstance::EACH), AGT_SVC_E1BANDWIDTH_NOTIF, byBuf, sizeof(u32) + sizeof(u16) + sizeof(u8));
// 
//         }
//     }

    return;
}


#ifdef _MINIMCU_
/*=============================================================================
�� �� ���� ProcBoardSetDscInfoRsp
��    �ܣ� ����DSC info����Ļ�Ӧ������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage* const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/17   4.0			�ܹ��                  ����
=============================================================================*/
void CBoardManager::ProcBoardSetDscInfoRsp( CMessage* const pcMsg )
{
	post( MAKEIID(AID_MCU_AGENT, 1), pcMsg->event, pcMsg->content, pcMsg->length );
}

/*=============================================================================
�� �� ���� ProcDscStartUpdateSoftwareRsp
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
void CBoardManager::ProcDscStartUpdateSoftwareRsp( CMessage* const pcMsg )
{
	if ( pcMsg->event == DSC_MPC_STARTUPDATE_SOFTWARE_ACK )
	{
		post( MAKEIID(AID_MCU_CONFIG, 1), AGT_SVC_STARTUPDATEDSCVERSION_ACK, pcMsg->content, pcMsg->length );
	}
	else
	{
		post( MAKEIID(AID_MCU_CONFIG, 1), AGT_SVC_STARTUPDATEDSCVERSION_NACK, pcMsg->content, pcMsg->length );
	}
}

/*=============================================================================
�� �� ���� ProcDscMpcUpdateFileRsp
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
void CBoardManager::ProcDscMpcUpdateFileRsp( CMessage* const pcMsg )
{
	if ( pcMsg->event == DSC_MPC_UPDATEFILE_ACK )
	{
		post( MAKEIID(AID_MCU_CONFIG, 1), AGT_SVC_DSCUPDATEFILE_ACK, pcMsg->content, pcMsg->length );
	}
	else
	{
		post( MAKEIID(AID_MCU_CONFIG, 1), AGT_SVC_DSCUPDATEFILE_NACK, pcMsg->content, pcMsg->length );
	}
}
#endif

/*=============================================================================
�� �� ���� ProcBoardConfigModifyNotif
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
void CBoardManager::ProcBoardConfigModifyNotif( CMessage* const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byLayer = *(u8*)( cServMsg.GetMsgBody() );
	u8 bySlot = *(u8*)( cServMsg.GetMsgBody() + sizeof(u8) );
	u8 byType = *(u8*)( cServMsg.GetMsgBody() + sizeof(u8)*2 );
	TBrdPosition tBrdPos;
	tBrdPos.byBrdLayer = byLayer;
	tBrdPos.byBrdSlot = bySlot;
	tBrdPos.byBrdID = byType;
	if ( g_cBrdManagerApp.IsRegedBoard( tBrdPos ) )
	{
		CEqpConfigBuf cCfgBuf;

		TBoardInfo tBrdInfo;
		g_cCfgParse.GetBrdCfgById( g_cCfgParse.GetBrdId( tBrdPos ), &tBrdInfo );
		PackEqpConfig( cCfgBuf, tBrdInfo );
        u16 wLen = 0;
        u8* pbyBuf = cCfgBuf.GetBuffer(wLen);

		cServMsg.SetMsgBody( (u8*)&byLayer, sizeof(byLayer) );
		cServMsg.CatMsgBody( (u8*)&bySlot, sizeof(bySlot) );
		cServMsg.CatMsgBody( (u8*)pbyBuf, wLen );
		PostMsgToBoard( MCU_BOARD_CONFIGMODIFY_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	}
}

/*=============================================================================
�� �� ���� ProcMpcBrdDisconnectCmd
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage* const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/28   4.0			�ܹ��                  ����
=============================================================================*/
void CBoardManager::ProcMpcBrdDisconnectCmd( void )
{
	ClearInst();
}

/*====================================================================
    ������      ��ProcMpcCommand
    ����        �����ϲ�ҵ��Ĺ�����Ϣ���͵�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBoardManager::ProcMpcCommand(CMessage* const pcMsg)
{
    if( NULL == pcMsg )
    {
        Agtlog(LOG_ERROR, "[ProcMpcCommand] The pointer can not be Null\n");
        return;
    }

    switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
		PostMsgToBoard( pcMsg->event, pcMsg->content, pcMsg->length );
		break;

	default:
		Agtlog(LOG_ERROR, "[ProcMpcCommand] Wrong message %u(%s) received in current state %u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    ������      ��PostMsgToBoard
    ����        ��������Ϣ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
BOOL32 CBoardManager::PostMsgToBoard( u16 wEvent, u8 * const pbyContent, u16 wLen  )
{
	BOOL32 bRet = FALSE;
    if( OspIsValidTcpNode( m_dwBrdNode ) )
	{
        // ���岻��Ӧȡ������Ϣ
		if( !g_cCfgParse.IsMpcActive() &&
            (wEvent == MPC_BOARD_GET_CONFIG_ACK || MPC_BOARD_GET_CONFIG_NACK == wEvent) )
        {
            Agtlog(LOG_VERBOSE, "[PostMsgToBoard] this SLV board not rsp to get cfg req !\n");
            return TRUE;
        }
        s32 nRet = post( m_dwBrdIId, wEvent, pbyContent, wLen, m_dwBrdNode );
        if ( 0 != nRet ) 
        {
            Agtlog(LOG_ERROR, "[PostMsgToBoard] bMST.%d post msg%d<%s> failed !\n",
                               g_cCfgParse.IsMpcActive(), wEvent, OspEventDesc(wEvent));
        }
        else
        {
            Agtlog(LOG_VERBOSE, "[PostMsgToBoard] bMST.%d post msg%d<%s> succeed !\n",
                                g_cCfgParse.IsMpcActive(), wEvent, OspEventDesc(wEvent));
            bRet = TRUE;
        }
	}
	else  
	{
#ifdef _TEST_	// ��Ԫ����ʱ��ſ�������
		post( m_dwBrdIId, wEvent, pbyContent, wLen, m_dwBrdNode );
        
        Agtlog(LOG_INFORM, "[PostMsgToBoard] Board <%d,%d:%s> is offline now: Node=%u, CurState=%u, InstId=%u\n", 
					        m_tBrdPosition.byBrdLayer,
					        m_tBrdPosition.byBrdSlot,
                            g_cCfgParse.GetBrdTypeStr(m_tBrdPosition.byBrdID),
					        m_dwBrdNode, CurState(), GetInsID() );
		bRet = FALSE;
#endif
	}
    return bRet;
}

/*====================================================================
    ������      ��ProcBoardLedNotify
    ����        ������ĵ�״ָ̬ʾ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBoardManager::ProcBoardLedNotify(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
    {
        Agtlog(LOG_INFORM, "[ProcBoardLedNotify] The pointer can not be Null\n");
        return;
    }
    // ����TBrdPosition
//	TBrdPosition *ptBrdPos = (TBrdPosition*)pcMsg->content;
	TBrdPos *ptBrdPosChange = (TBrdPos*)pcMsg->content;

// [pengjie 2010/3/25]
	TBrdPosition tBrdPos;
	tBrdPos.byBrdID    = ptBrdPosChange->byBrdID;
	tBrdPos.byBrdLayer = ptBrdPosChange->byBrdLayer;
	tBrdPos.byBrdSlot  = ptBrdPosChange->byBrdSlot;


	// ���Ƹ���
	u8 byCount = *(pcMsg->content + sizeof(TBrdPos));
	// ���Ϊ������״̬
	u8* pbyLedState = pcMsg->content + sizeof(TBrdPos) + sizeof(u8);

    u8 byBrdId = g_cCfgParse.GetBrdId( m_tBrdPosition );

    TEqpBrdCfgEntry tBrdCfg;
    memset( &tBrdCfg, 0, sizeof(tBrdCfg) );
    g_cCfgParse.GetBrdInfoById( byBrdId, &tBrdCfg );

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
        {
		    // ��֤�Ƿ�ͱ���ĵ�����Ϣһ��
//            if ( 0 != memcmp( ptBrdPos, &m_tBrdPosition, sizeof(TBrdPosition)) )
			if( tBrdPos.byBrdID    != m_tBrdPosition.byBrdID ||
				tBrdPos.byBrdLayer != m_tBrdPosition.byBrdLayer ||
			    tBrdPos.byBrdSlot  != m_tBrdPosition.byBrdSlot )
		    {
                Agtlog(LOG_INFORM, "[ProcBoardLedNotify] Board <%d,%d:%s> Alarm Ack Conflict!\n", 
				                    tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                                    g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID));
			    ::OspDisconnectTcpNode( pcMsg->srcnode );
			    return;			
		    }

			s8 abyLedState[256] = {0};
			
			abyLedState[255] = '\0';
			
			s8 *pBuff = abyLedState;
			
			pBuff += sprintf(pBuff, "[ProcBoardLedNotify] Led Num:%d [", byCount);
			
			u32 dwIndex = 0;
			
			for (dwIndex = 0; dwIndex < byCount; ++dwIndex)
			{
				if (dwIndex >= MAX_BOARD_LED_NUM)
				{
					break;
				}
				
				pBuff += sprintf(pBuff, " %d ", *(pbyLedState + dwIndex));
			}
			
			
			pBuff += sprintf(pBuff, "]\n");
			
			
			Agtlog(LOG_VERBOSE, abyLedState);

            tBrdCfg.SetPanelLed((s8 *)pbyLedState);
            g_cCfgParse.SetBrdInfoById( byBrdId, &tBrdCfg );

            post( MAKEIID(AID_MCU_AGENT, 1), EV_BOARD_LED_ALARM, &tBrdCfg, sizeof(tBrdCfg) ); // ������Ϣ����
        }
        break;

	default:
		Agtlog(LOG_WARN, "[ProcBoardLedNotify] Wrong message %u(%s) received in current state %u!\n", 
			              pcMsg->event, ::OspEventDesc(pcMsg->event), CurState() );
		break;
	}
	return;	
}

/*====================================================================
    ������      ��ProcBoardEthPortNotify
    ����        �����������״̬֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    12/05/04    4.7         liaokang       ����
====================================================================*/
void CBoardManager::ProcBoardEthPortNotify(CMessage* const pcMsg )
{
    if( NULL == pcMsg )
    {
        Agtlog(LOG_INFORM, "[ProcBoardEthPortNotify] The pointer can not be Null\n");
        return;
    }

    TBrdPos *ptBrdPosChange = (TBrdPos*)pcMsg->content;

    TBrdPosition tBrdPos;
    tBrdPos.byBrdID    = ptBrdPosChange->byBrdID;
    tBrdPos.byBrdLayer = ptBrdPosChange->byBrdLayer;
    tBrdPos.byBrdSlot  = ptBrdPosChange->byBrdSlot;
    
    // ���ڸ���
    u8 byCount = *(pcMsg->content + sizeof(TBrdPos));
    // ��������״̬
    u8* pbyEthPortState = pcMsg->content + sizeof(TBrdPos) + sizeof(u8);
    
    u8 byBrdId = g_cCfgParse.GetBrdId( m_tBrdPosition );
    
    TEqpBrdCfgEntry tBrdCfg;
    memset( &tBrdCfg, 0, sizeof(tBrdCfg) );
    g_cCfgParse.GetBrdInfoById( byBrdId, &tBrdCfg );
    
    switch( CurState() ) 
    {
    case STATE_INIT:
    case STATE_NORMAL:
        {
            // ��֤�Ƿ�ͱ���ĵ�����Ϣһ��
            //            if ( 0 != memcmp( ptBrdPos, &m_tBrdPosition, sizeof(TBrdPosition)) )
            if( tBrdPos.byBrdID    != m_tBrdPosition.byBrdID ||
                tBrdPos.byBrdLayer != m_tBrdPosition.byBrdLayer ||
                tBrdPos.byBrdSlot  != m_tBrdPosition.byBrdSlot )
            {
                Agtlog(LOG_INFORM, "[ProcBoardEthPortNotify] Board <%d,%d:%s> Alarm Ack Conflict!\n", 
                    tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                    g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID));
                ::OspDisconnectTcpNode( pcMsg->srcnode );
                return;			
            }
            
            s8 abyNetPortState[256] = {0};
            
            abyNetPortState[255] = '\0';
            
            s8 *pBuff = abyNetPortState;
            
            pBuff += sprintf(pBuff, "[ProcBoardEthPortNotify] EthPort Num:%d [", byCount);
            
            u32 dwIndex = 0;
            
            for (dwIndex = 0; dwIndex < byCount; ++dwIndex)
            {
                if (dwIndex >= MAX_BOARD_ETHPORT_NUM)
                {
                    break;
                }
                
                pBuff += sprintf(pBuff, " %d ", *(pbyEthPortState + dwIndex));
            }
            
            
            pBuff += sprintf(pBuff, "]\n");
            
            Agtlog(LOG_VERBOSE, abyNetPortState);
            
            tBrdCfg.SetEthPort((s8 *)pbyEthPortState);
            g_cCfgParse.SetBrdInfoById( byBrdId, &tBrdCfg );
            
            post( MAKEIID(AID_MCU_AGENT, 1), EV_BOARD_ETHPORT_ALARM, &tBrdCfg, sizeof(tBrdCfg) ); // ������Ϣ����
        }
        break;
        
    default:
        Agtlog(LOG_WARN, "[ProcBoardEthPortNotify] Wrong message %u(%s) received in current state %u!\n", 
            pcMsg->event, ::OspEventDesc(pcMsg->event), CurState() );
        break;
    }
	return;	

}

/*====================================================================
    ������      ��ProcBoardSoftwareVerNotify
    ����        �����������汾֪ͨ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	2011/12/13   4.7        �´�ΰ        ����
====================================================================*/
/*void CBoardManager::ProcBoardSoftwareVerNotify(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
    {
        Agtlog(LOG_INFORM, "[ProcBoardSoftwareVerNotify] The pointer can not be Null\n");
        return;
    }

	TBrdPos *ptBrdPosChange = (TBrdPos*)pcMsg->content;

	TBrdPosition tBrdPos;
	tBrdPos.byBrdID    = ptBrdPosChange->byBrdID;
	tBrdPos.byBrdLayer = ptBrdPosChange->byBrdLayer;
	tBrdPos.byBrdSlot  = ptBrdPosChange->byBrdSlot;
	u32 dwSoftwareVersion = *(u32*)(pcMsg->content+sizeof(TBrdPos));

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
        {
		    // ��֤�Ƿ�ͱ���ĵ�����Ϣһ��
			if( tBrdPos.byBrdID    != m_tBrdPosition.byBrdID ||
				tBrdPos.byBrdLayer != m_tBrdPosition.byBrdLayer ||
			    tBrdPos.byBrdSlot  != m_tBrdPosition.byBrdSlot )
		    {
                Agtlog(LOG_INFORM, "[ProcBoardSoftwareVerNotify] Board <%d,%d:%s> software version notify error!\n", 
				                    tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                                    g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID));
			    ::OspDisconnectTcpNode( pcMsg->srcnode );
			    return;			
		    }
			
            post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_BRD_SOFTWARE_VERSION_NOTIFY, pcMsg->content, sizeof(TBrdPos)+sizeof(u32) ); // ������Ϣ����
        }
        break;

	default:
		Agtlog(LOG_WARN, "[ProcBoardSoftwareVerNotify] Wrong message %u(%s) received in current state %u!\n", 
			              pcMsg->event, ::OspEventDesc(pcMsg->event), CurState() );
		break;
	}
	return;	
}
*/

/*====================================================================
    ������      ��ProcBoardAlarmNotify
    ����        ������ĸ澯ָʾ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBoardManager::ProcBoardAlarmNotify(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
    {
        Agtlog(LOG_ERROR, "[ProcBoardAlarmNotify] The pointer can not be Null\n");
        return;
    }
    
    u16 wLoop = 0;
	// ����TBrdPosition
//	TBrdPosition *ptBrdPos = (TBrdPosition*)pcMsg->content;

	// [pengjie 2010/3/25]
	TBrdPos *ptBrdPosChange = (TBrdPos*)pcMsg->content;
	TBrdPosition tBrdPos;
	tBrdPos.byBrdID    = ptBrdPosChange->byBrdID;
	tBrdPos.byBrdLayer = ptBrdPosChange->byBrdLayer;
	tBrdPos.byBrdSlot  = ptBrdPosChange->byBrdSlot;


	// ���Ϊ�澯�����
	u16 wCount = ntohs( *(u16*)(pcMsg->content + sizeof(TBrdPos)) );
	// �����Ϊ�澯��
	TBoardAlarmMsgInfo* ptBoardAlarm = (TBoardAlarmMsgInfo*)
		( pcMsg->content + sizeof(TBrdPos) + sizeof(wCount) );

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
		// ��֤�Ƿ�ͱ���ĵ�����Ϣһ��
//        if ( 0 != memcmp( ptBrdPos, &m_tBrdPosition, sizeof(TBrdPosition)) )
		if( tBrdPos.byBrdID    != m_tBrdPosition.byBrdID ||
			tBrdPos.byBrdLayer != m_tBrdPosition.byBrdLayer ||
			tBrdPos.byBrdSlot  != m_tBrdPosition.byBrdSlot )		
        {
            Agtlog(LOG_ERROR, "[ProcBoardAlarmNotify] Board <%d,%d:%s> Alarm Ack Conflict!\n", 
				               tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                               g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID));
			::OspDisconnectTcpNode( pcMsg->srcnode );
			return;			
		}
		
		if( g_cCfgParse.HasSnmpNms() )
		{
			// ���澯���е�ÿһ����뵽MPC�ĸ澯����
			for( wLoop = 0; wLoop < wCount; wLoop++)
			{
				post( MAKEIID(AID_MCU_AGENT, 1), ntohs( ptBoardAlarm->wEventId ), 
					  ptBoardAlarm->abyAlarmContent,sizeof(ptBoardAlarm->abyAlarmContent) );
				ptBoardAlarm++;
			}
		}
		break;

	default:
		Agtlog(LOG_WARN, "[ProcBoardAlarmNotify] Wrong message %u(%s) received in current state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;	
}


// /*====================================================================
//     ������      ��ProcBoardGetStatisticsAck
//     ����        �������ͳ����ϢӦ����Ϣ
//     �㷨ʵ��    ��
//     ����ȫ�ֱ�����
//     �������˵����CMessage * const pcMsg, �������Ϣ
//     ����ֵ˵��  ��
// ----------------------------------------------------------------------
//     �޸ļ�¼    ��
//     ��  ��      �汾        �޸���        �޸�����
//     03/08/19    1.0         jianghy       ����
// ====================================================================*/
// void CBoardManager::ProcBoardGetStatisticsAck(CMessage* const pcMsg)
// {
// 	// ��Ϣ�ṹ��Ϊ����λ�á�ͳ����Ϣ�ṹ(���������ͳ����Ϣ���ܲ�һ����Ҫ�ֱ���)
// 
// 	return;
// }


/*====================================================================
    ������      ��ProcBoardDisconnect
    ����        ��OSP�͵���Ͽ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBoardManager::ProcBoardDisconnect( CMessage* const pcMsg )
{
	if( NULL == pcMsg )
    {
        Agtlog(LOG_ERROR, "[ProcBoardDisconnect] The pointer can not be Null\n");
        return;
    }

    switch( CurState() ) 
    {
    case STATE_INIT:
    case STATE_WAITREG:
    case STATE_NORMAL:	
        {
            Agtlog(LOG_ERROR, "[ProcBoardDisconnect] Board<%d,%d:%s> Disconnected��Inst.%d, State.%d!\n", 
                               m_tBrdPosition.byBrdLayer, m_tBrdPosition.byBrdSlot,
                               g_cCfgParse.GetBrdTypeStr(m_tBrdPosition.byBrdID), GetInsID(), CurState());
            
            g_cCfgParse.ChangeBoardStatus( m_tBrdPosition, BOARD_STATUS_DISCONNECT );
            
            g_cBrdManagerApp.DelBoard(m_tBrdPosition);
            
            CServMsg cServMsg;
            cServMsg.SetMsgBody((u8*)&m_tBrdPosition.byBrdLayer, sizeof(u8) );
            cServMsg.CatMsgBody((u8*)&m_tBrdPosition.byBrdSlot, sizeof(u8) );
            cServMsg.CatMsgBody((u8*)&m_tBrdPosition.byBrdID, sizeof(u8) );
            
            TBrdStatus tBrdStatus;
            // ֪ͨǰ̨����Ҫ�澯��
            tBrdStatus.byLayer = m_tBrdPosition.byBrdLayer;
            tBrdStatus.bySlot = m_tBrdPosition.byBrdSlot;
            tBrdStatus.byType = m_tBrdPosition.byBrdID;
            tBrdStatus.byStatus = BOARD_STATUS_DISCONNECT;  // = 1; (2005-11-26)
            tBrdStatus.byOsType = 0;
            post( MAKEIID(AID_MCU_AGENT, 1), MCU_AGT_BOARD_STATUSCHANGE, (u8*)&tBrdStatus, sizeof(tBrdStatus) ); 
            
            // �������������
            ClearInst();            
        }
		break;

	default:
		Agtlog(LOG_ERROR, "[ProcBoardDisconnect] Wrong message %u(%s) received in current Inst.%d state %u!\n", 
			               pcMsg->event, ::OspEventDesc(pcMsg->event), GetInsID(), CurState() );
		break;
	}	
	return ;
}

/*#if defined(_8KI_) && defined(_LINUX_)
/*=============================================================================
�� �� ���� ProcDetectEthTimer
��    �ܣ� �������״̬�������߶Ͽ����������л�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2010/04/02  4.6         Ѧ��        ����
=============================================================================
void CBoardManager::ProcDetectEthTimer(CMessage * const pcMsg )
{
	BOOL32 bShift = FALSE;		//�Ƿ������л�
	BOOL32 bRecOrg = FALSE;
	u8 byState0 = OK;
	u8 byState1 = OK;

	if( ERROR == BrdGetEthLinkStat(0,&byState0))
	{
		Agtlog(LOG_WARN,"[ProcDetectEthTimer]BrdGetEthLinkStat 0 failed!\n");
	}
	
	if(ERROR == BrdGetEthLinkStat(1,&byState1))
	{
		Agtlog(LOG_WARN,"[ProcDetectEthTimer]BrdGetEthLinkStat 1 failed!\n");
	}

	u8 byCurEthIdx = g_cMcuAgent.GetInterface();
	
	u8  byStatChk = (byCurEthIdx == 0) ? byState0: byState1;
	u8	byIdBak = (byCurEthIdx == 0) ? 1: 0;

	// [7/8/2010 xliang] ����B(��Գ�ʼ�����������ڵ�MAC��������ͬ��, ��down��up����)
	if( byStatChk != 1 )
	{
		if((byIdBak == 1 && byState1 == 1) || (byIdBak == 0 && byState0 == 1))
		{
			//TODO ��������ȷ��down������byCurEthIdx��up����byIdBak[5/8/2012 chendaiwei]
			if(ERROR == BrdInterfaceShutdown("fastethnet",byCurEthIdx))
			{
				Agtlog(LOG_WARN,"Failed shutdown eth%d!\n",byCurEthIdx);
			}
			
			if(ERROR == BrdInterfaceNoShutdown("fastethernet",byIdBak))
			{
				Agtlog(LOG_WARN,"Failed to up eth%d!\n",byIdBak);
			}
			
			//TODO �Ƿ���Ҫ�������غ�·�ɣ��ڰ汾�л���
			
			Agtlog(LOG_LVL_KEYSTATUS,  "network shift from eth%d to eth%d!\n", byCurEthIdx, byIdBak);
			printf( "network shift from eth%d to eth%d!\n", byCurEthIdx, byIdBak);
			
			g_cMcuAgent.SetInterface(byIdBak);
			post( MAKEIID(AID_MCU_CONFIG,1), AGT_MCU_CFG_ETHCHANGE_NOTIF);

		}
	}
}
#endif*/

/*====================================================================
	������  ��DaemonInstanceEntry
	����    ��DAEMON ʵ����Ϣ������ں���  !����ʱʹ��!!!
	����    ����Ϣ
	���    ����
	����ֵ  ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void CBoardManager::DaemonInstanceEntry(CMessage* const pcMsg, CApp* pcApp)
{
	if( NULL == pcMsg )
    {
        Agtlog(LOG_ERROR, "[ProcBoardDisconnect] The pointer can not be Null\n");
        return;
    }
    
    switch(pcMsg->event) 
	{
	case BOARD_MPC_CFG_TEST:
		ProcDaemonGetConfig();
		break;

	case BOARD_MPC_MANAGERCMD_TEST:
		ProcDaemonManagerCmd();
		break;

    case BOARD_AGENT_POWERON:
        DaemonProcDaemonPowerOn(pcApp);
        break;

    case BOARD_MPC_REG:   // ����ע����Ը���Daemon������
        DaemonProcBrdRegMsg(pcMsg, pcApp);
        break;

	// ���Խӿ�
	case TEST_BOARDMANAGERCMD_DUMPINST:
		DaemonDumpInst( pcApp );
		break;

/*#if defined(_8KI_) && defined(_LINUX_)
	case EV_ETH_STATDETECT_TIME:
		ProcDetectEthTimer( pcMsg );
		break;
#endif*/

	default:
		OspPrintf(TRUE, FALSE,  "BoardManager: [DaemonInstanceEntry] Wrong message %u(%s) received \n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ) );		
		break;
	}
	return;
}

/*====================================================================
	������  ��ProcDaemonGetConfig
	����    ������MSG�еĵ����ʶ��������Ӧ������  !����ʱʹ��!!!
	����    ����Ϣ
	���    ����
	����ֵ  ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void CBoardManager::ProcDaemonGetConfig( void )
{
/*
	if( NULL == pcMsg )
    {
        OspPrintf( TRUE, FALSE, "[BoardManager] The pointer can not be Null\n");
        return;
    }

    TBrdPosition *ptBrdPos = (TBrdPosition*)pcMsg->content;
	TEqpRecEntry tRecCfg;
	TEqpTVWallEntry tTVWallCfg;
	u8 byPeriCount;
	u8 abyPeriId[5];
	u8 byPeriType;
	u8 abyBuf[128];
    u16 wIndex = 0;
    u16 wLoop = 0;

#ifdef _TEST_
    OspPrintf(TRUE, FALSE, "[] Get configure \n");
#endif
    
	memset(abyBuf,0,sizeof(abyBuf));
		
	if( ptBrdPos->byBrdID == 255 )  // ����(���ܲ���Ҫ)
	{
		switch( ptBrdPos->byBrdLayer )  // ��������
		{
		case  EQP_TYPE_RECORDER:
			// �Ӵ����ȡ������Ϣ
			if( g_cCfgParse.GetRecCfgToBrd( ptBrdPos->byBrdSlot, &tRecCfg ) )
			{
				PackRecorderCfg(&tRecCfg);
				SetSyncAck( (u8*)&tRecCfg, sizeof(tRecCfg) );
			}
			else
			{
				SetSyncAck( NULL, 0 );
				return;					
			}
			break;
			
		default:
			SetSyncAck( NULL, 0 );
			break;
		}
	}
	else    // ����
	{			
		// ��ͨ���㡢�ۡ����͵õ�����ID
		byPeriCount = g_cCfgParse.GetBrdPriId( ptBrdPos->byBrdLayer, ptBrdPos->byBrdSlot, 
			ptBrdPos->byBrdID, abyPeriId, sizeof(abyPeriId) );

#ifdef _TEST_
        OspPrintf(TRUE, FALSE, "byPeriCount = %x\n", byPeriCount);
#endif
		
        if( 0 == byPeriCount )// ������û����������
		{
#ifndef WIN32
			switch( ptBrdPos->byBrdID )
			{
			case DSL8000_BRD_DTI:
			case DSL8000_BRD_DRI:
				// �Ƿ�����ͬ��ʹ�õĵ���
				if( NETSYNTYPE_VIBRATION == g_cCfgParse.GetNetSyncMode())
				{
					abyBuf[0] = 0 ;
					SetSyncAck( (u8*)&abyBuf[0], sizeof(abyBuf[0]) );
				}
				else
				{
					u8 byBrdId = g_cCfgParse.GetNetDTSlot();
                    u8 byE1Index = g_cCfgParse.GetNetE1Idx();

					if( byBrdId==ptBrdPos->byBrdSlot )
					{
						abyBuf[0] = 1;
						abyBuf[1] = byE1Index;
						SetSyncAck( (u8*)abyBuf, 2 );
					}
					else
					{
						abyBuf[0] = 0 ;
						SetSyncAck( (u8*)&abyBuf[0], sizeof(abyBuf[0]) );
					}
				}
				break;
				
			case DSL8000_BRD_CRI:
			case DSL8000_BRD_VAS:
			case DSL8000_BRD_MMP:
			case DSL8000_BRD_DIC:
				SetSyncAck( NULL, 0 );
				break;
				
			default:
				SetSyncAck( NULL, 0 );
				return;
			}
#else
			SetSyncAck( NULL, 0 );
#endif
		}
		else// ����������������
		{
			memset(abyBuf, 0, sizeof(abyBuf) );
		    // ���Ǹ���
		    abyBuf[0] = byPeriCount;
			// BUF�Ѿ�ʹ�õĳ���
			wIndex = 1;

		    for( wLoop = 0; wLoop < byPeriCount; wLoop++ )
    		{
			    // ͨ������ID�õ�������
    			byPeriType = g_cCfgParse.GetPriType( abyPeriId[wLoop] );
    			// �ҵ���Ӧ��������Ϣ
    			switch( byPeriType )
    			{
    			case EQP_TYPE_TVWALL:
    				if( g_cCfgParse.GetTvCfgToBrd( 33, &tTVWallCfg ) )
    				{
                       OspPrintf(TRUE, FALSE, "mcueqpTVWallEntIpAddr = 0x%x\n", tTVWallCfg.dwMcueqpTVWallEntIpAddr);
                       OspPrintf(TRUE, FALSE, "mcueqpTVWallEntConnMcuIp = 0x%x\n", tTVWallCfg.dwMcueqpTVWallEntConnMcuIp);
                       OspPrintf(TRUE, FALSE, "mcueqpTVWallEntConnMcuPort = 0x%x\n", ntohs(tTVWallCfg.wMcueqpTVWallEntConnMcuPort));
                       OspPrintf(TRUE, FALSE, "mcueqpTVWallEntVideoStartRecvPort = 0x%x\n", ntohs(tTVWallCfg.wMcueqpTVWallEntVideoStartRecvPort));

	    				PackTVWallCfg( &tTVWallCfg );
	    				SetSyncAck( (u8*)&tTVWallCfg, sizeof(tTVWallCfg));
		    		}
		    		else
		    		{
			    		SetSyncAck( NULL, 0 );
				    	return;					
			     	}
			    	break;
				
		
				
		    	default:  
		    		SetSyncAck( NULL, 0 );
			    	return;
		    	}
			
	    	}
	    }
    }		*/

	return;
}

/*====================================================================
	������  ��ProcDaemonManagerCmd
	����    ������MSG�еĵ����ʶ��������Ӧ������  !����ʱʹ��!!!
	����    ����Ϣ
	���    ����
	����ֵ  ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void CBoardManager::ProcDaemonManagerCmd( void )
{
/*
	if( NULL == pcMsg )
    {
        OspPrintf( TRUE, FALSE, "[BoardManager] The pointer can not be Null\n");
        return;
    }

    // ��Ϣ��ΪҪת���ĵ���Ĳ�(1B)����(1B)������(1B)����Ϣ��(2B ������)����Ϣ���ݳ���(1B)����Ϣ����(nB)
	TBrdPosition tBrdPos;
	u16 wEvent;
	u32 dwRet;

	// Ҫ���͵ļ���
	memcpy(&tBrdPos, pcMsg->content, sizeof(tBrdPos) );

	// ��Ϣ��
	wEvent = ntohs( *(u16*)(pcMsg->content+3) );

	// ���͵�ָ���ĵ���
/ *	bRet = PostMsgToBrdMgr(tBrdPos, wEvent, pcMsg->content+6, 
		pcMsg->content[5] );* /
    
    dwRet = post( (s8*)&tBrdPos, sizeof(tBrdPos), AID_MCU_BRDMGR, wEvent, pcMsg->content+6,
		                                                                pcMsg->content[5], 0);	*/

	return ;
}

/*=============================================================================
  �� �� ���� ProcDaemonPowerOn
  ��    �ܣ� ʵ����ʼ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
             CApp* pApp
  �� �� ֵ�� void 
=============================================================================*/
void CBoardManager::DaemonProcDaemonPowerOn( CApp* pApp)
{
    for(u8 byLoop = 0; byLoop < MAX_BOARD_NUM; byLoop++)
    {
        CBoardManager* pInst = NULL;
        pInst = (CBoardManager*)pApp->GetInstance(byLoop+1);
        pInst->NextState(STATE_IDLE);
    }
    return;
}

/*=============================================================================
  �� �� ���� DaemonProcBrdRegMsg
  ��    �ܣ� ������ע��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardManager::DaemonProcBrdRegMsg(CMessage* const pMsg, CApp* pcApp)
{
    if(NULL == pMsg)
    {
        Agtlog(LOG_ERROR, "[DaemonProcBrdRegMsg] The msg can not be Null\n");
        return;
    }

    // ��Ϣ��ṹ��TBoardPosition��dwBrdIp(u32)��byChoice(u8) + byOsType(u8) (���Ե���)
    CServMsg  cSrevMsg(pMsg->content, pMsg->length);
//	TBrdPosition tBrdPos = *(TBrdPosition*)cSrevMsg.GetMsgBody();
	TBrdPos tBrdPosChange = *(TBrdPos*)cSrevMsg.GetMsgBody();
    u32 dwBoardIp = *(u32*)(cSrevMsg.GetMsgBody() + sizeof(TBrdPos));
    u8  byChoice = *(u8*)(cSrevMsg.GetMsgBody() + sizeof(TBrdPos) + sizeof(u32));
    u8  byOsType = *(u8*)(cSrevMsg.GetMsgBody() + sizeof(TBrdPos) + sizeof(u32) + sizeof(u8));
    
    u32 dwBrdInstId = pMsg->srcid;
    u32 dwBrdNode = pMsg->srcnode;

	// [pengjie 2010/3/24] �ײ�ṹ��ת��
	TBrdPosition tBrdPos;
	tBrdPos.byBrdID    = tBrdPosChange.byBrdID;
	tBrdPos.byBrdLayer = tBrdPosChange.byBrdLayer;
	tBrdPos.byBrdSlot  = tBrdPosChange.byBrdSlot;

    // guzh [2008/08/01] �ǻ��򵥰崦��
    if (tBrdPos.byBrdLayer == BRD_LAYER_CUSTOM)
    {
        // ֱ�ӻ�Ӧ����Ҫ����Ϣ���ɣ�McuAgent����������������Mcu EqpSsn
        BOOL32 bMpcEnable = g_cCfgParse.IsMpcActive();
        TBrdRegAck tRegAck;
        TMPCInfo tMpcInfo;
        g_cCfgParse.GetMPCInfo( &tMpcInfo );
        if ( tMpcInfo.GetOtherMpcIp() != 0 )
        {
            tRegAck.SetOtherMpcIp( tMpcInfo.GetOtherMpcIp() );
            tRegAck.SetOtherMpcPort( tMpcInfo.GetOtherMpcPort() );
        }
        else
        {
            tRegAck.SetOtherMpcIp( 0 );
            tRegAck.SetOtherMpcPort( 0 );
        }
        Agtlog(LOG_INFORM, " DeamonRegAck: Other mpc ip is 0x%x, Other mpc port is %d\n", tRegAck.GetOtherMpcIp(), tRegAck.GetOtherMpcPort());

        CServMsg cServMsg;
        cServMsg.SetMsgBody((u8*)&bMpcEnable, sizeof(bMpcEnable));
        cServMsg.CatMsgBody((u8*)&tRegAck, sizeof(tRegAck));
		post( dwBrdInstId, MPC_BOARD_REG_ACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), dwBrdNode );        

        return;
    }


    
    // ��Ϣ��ṹ��TBoardPosition��dwBrdIp(u32)��byChoice(u8)+dwBrdNode(u32)+dwBrdInstId(u32) + byOsType(u8)
    CServMsg cRegMsg;
    cRegMsg.SetMsgBody((u8*)&tBrdPosChange, sizeof(TBrdPos));
    cRegMsg.CatMsgBody((u8*)&dwBoardIp, sizeof(u32));
    cRegMsg.CatMsgBody((u8*)&byChoice, sizeof(u8));
    cRegMsg.CatMsgBody((u8*)&dwBrdNode, sizeof(u32));
    cRegMsg.CatMsgBody((u8*)&dwBrdInstId, sizeof(u32));
    cRegMsg.CatMsgBody((u8*)&byOsType, sizeof(u8));
    
	u8 byDstInstId = g_cCfgParse.GetBrdInstIdByPos( tBrdPos );
	if ( byDstInstId == 0 || byDstInstId > MAX_BOARD_NUM )
	{
		Agtlog(LOG_ERROR, "[DaemonProcBrdRegMsg] Brd<%d,%d> get wrong inst.%d!\n", 
					  tBrdPos.byBrdLayer, tBrdPos.byBrdSlot, byDstInstId );
		return;
	}
	CBoardManager* pInst = (CBoardManager*)pcApp->GetInstance(byDstInstId);

	OspPrintf(TRUE,FALSE,"[DaemonProcBrdRegMsg]Brd<%d,%d> get inst.%d success!\n", 
					  tBrdPos.byBrdLayer, tBrdPos.byBrdSlot, byDstInstId);

	if ( STATE_IDLE == pInst->CurState() ) 
	{
		pInst->NextState(STATE_WAITREG);
		post( MAKEIID(AID_MCU_BRDMGR, byDstInstId), BOARD_MPC_REG, 
			   cRegMsg.GetServMsg(), cRegMsg.GetServMsgLen(), 0 );
	}
	else
	{
		Agtlog(LOG_ERROR, "[DaemonProcBrdRegMsg] The Brd<%d,%d,%s>'s pos is not IDLE which is impossible, check it.\n", 
            tBrdPos.byBrdLayer, tBrdPos.byBrdSlot, g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );
	}
    return;
}

// /*=============================================================================
//   �� �� ���� DaemonProcBrdDisconectMsg
//   ��    �ܣ� ���������(����������������Ӧ��ʵ����������һ����Daem������)
//   �㷨ʵ�֣� 
//   ȫ�ֱ����� 
//   ��    ���� CMessage* const pMsg
//   �� �� ֵ�� void 
// =============================================================================*/
// void CBoardManager::DaemonProcBrdDisconectMsg(CMessage* const pMsg)
// {
//     return;
// }

/*=============================================================================
  �� �� ���� DaemonDumpInst
  ��    �ܣ� ��ӡʵ����Ϣ(���Խӿ�)
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  CMessage* const pMsg
             CApp* pApp
  �� �� ֵ�� void 
=============================================================================*/
void CBoardManager::DaemonDumpInst(CApp* pApp)
{
	CBoardManager* pInst = NULL;
	for(u8 byLoop = 0; byLoop < MAX_BOARD_NUM; byLoop++)
    {
        pInst = (CBoardManager*)pApp->GetInstance(byLoop+1);
		if( NULL != pInst )
		{
			switch( pInst->CurState() )
			{
			case STATE_IDLE:
				OspPrintf(TRUE, FALSE, "[!debug!] Instance %d state: %d. (IDLE)\n",byLoop+1, pInst->CurState() );
				break;
			case STATE_WAITREG:
				OspPrintf(TRUE, FALSE, "[!debug!] Instance %d state: %d. (WAITREG)\n",byLoop+1, pInst->CurState() );
				break;
			case STATE_INIT:
				OspPrintf(TRUE, FALSE, "[!debug!] Instance %d state: %d. (INIT)\n",byLoop+1, pInst->CurState() );
				break;
			case STATE_NORMAL:
				OspPrintf(TRUE, FALSE, "[!debug!] Instance %d state: %d. (NORMAL)\n",byLoop+1, pInst->CurState() );
				break;
			default:
				OspPrintf(TRUE, FALSE, "[!debug!] Instance %d state: %d. (unknow)\n",byLoop+1, pInst->CurState() );
				break;
			}	
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[!debug!] Instance %d is null. (unknow)\n",byLoop+1 );
		}
    }
	return;
}


//////////////////////////////////////////////////////////////////////////
// 
//                         CRegedBoard ��
// 
//////////////////////////////////////////////////////////////////////////

CRegedBoard::CRegedBoard()
{
    for(u8 byLoop = 0; byLoop < MAX_BOARD_NUM; byLoop++)
    {
        m_tBrdReged[byLoop].bReg = FALSE;
        m_tBrdReged[byLoop].dwNode = INVALID_NODE;
        m_tBrdReged[byLoop].dwDstInsId = INVALID_INS;
        m_tBrdReged[byLoop].tBrdPos.byBrdID = 0;
        m_tBrdReged[byLoop].tBrdPos.byBrdLayer = 0;
        m_tBrdReged[byLoop].tBrdPos.byBrdSlot = 0;
    }
    m_wRegedBrdNum = 0;

	if( !OspSemBCreate(&m_hBoardManager) )  // �������
    {
        OspSemDelete( m_hBoardManager );
        Agtlog(LOG_ERROR, "[CRegedBoard] Create m_hBoardManager failed\n");
    }
}

CRegedBoard::~CRegedBoard()
{
	if( m_hBoardManager )
	{
		OspSemDelete( m_hBoardManager );
		m_hBoardManager = NULL;
	}
}
/*=============================================================================
  �� �� ���� IsRegedBoard
  ��    �ܣ� �ж�ָ�������Ƿ�ע��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TBrdPosition tBrdPos
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CRegedBoard::IsRegedBoard(TBrdPosition tBrdPos, u32 dwNode)
{
    BOOL32 bReged = FALSE;

    for(u8 byLoop = 0; byLoop < MAX_BOARD_NUM; byLoop++)
    {
        if( 0 == memcmp( &tBrdPos, &m_tBrdReged[byLoop].tBrdPos, sizeof(tBrdPos) ) )
        {
            if ( TRUE == m_tBrdReged[byLoop].bReg )
            {
                Agtlog( LOG_INFORM, "[IsRegedBoard] Board<%d,%d:%s> has been registered ! dwNode.%d\n",
                                     tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                                     g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID),dwNode );
                bReged = TRUE;
                break;
            }
        }
    }
    if ( !bReged )
    {
        Agtlog( LOG_VERBOSE, "[IsRegedBoard] Boadd<%d,%d:%s> has not been registered !\n",
                              tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                              g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );
    }
    return bReged;
}

/*=============================================================================
  �� �� ���� AddBoard
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TBrdPosition tBrdPos
             u16 wInsId
             u32 dwNode
  �� �� ֵ�� u16  
=============================================================================*/
/*lint -save -esym(1788,cSemOpt)*/
u16 CRegedBoard::AddBoard(TBrdPosition tBrdPos, u32 dwInsId, u32 dwNode)
{
	ENTER( m_hBoardManager );
	
	u16 wRet = SUCCESS_AGENT;
	u8 byLoop = 0;
	u8 byIdleInx = MAX_BOARD_NUM;
    for( byLoop = 0; byLoop < MAX_BOARD_NUM; byLoop++ )
    {
        if( TRUE == m_tBrdReged[byLoop].bReg &&
            0 == memcmp( &tBrdPos, &m_tBrdReged[byLoop].tBrdPos, sizeof(tBrdPos) ) )
        {
            return ERR_AGENT_BOARDDEXIST;
        }

		if( !m_tBrdReged[byLoop].bReg )
		{
			if( MAX_BOARD_NUM == byIdleInx ) // first time
			{
				byIdleInx = byLoop;
			}
		}
    }

	if( MAX_BOARD_NUM != byIdleInx )
	{
		m_tBrdReged[byIdleInx].bReg = TRUE;
		m_tBrdReged[byIdleInx].tBrdPos.byBrdLayer = tBrdPos.byBrdLayer;
		m_tBrdReged[byIdleInx].tBrdPos.byBrdSlot = tBrdPos.byBrdSlot;
		m_tBrdReged[byIdleInx].tBrdPos.byBrdID = tBrdPos.byBrdID;
		m_tBrdReged[byIdleInx].dwNode = dwNode;
		m_tBrdReged[byIdleInx].dwDstInsId = dwInsId;
		m_wRegedBrdNum++;

        Agtlog( LOG_INFORM, "[AddBoard] Brd<%d,%d:%s> has been add succeed !\n",
                             tBrdPos.byBrdLayer, tBrdPos.byBrdSlot, 
                             g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );
	}
	else
	{
		wRet = ERR_AGENT_BOARDDEXIST;
	}

/*
    for(byLoop = 0; byLoop < MAX_BOARD_NUM; byLoop++)
    {
        if(FALSE == m_tBrdReged[byLoop].bReg)
        {
            m_tBrdReged[byLoop].bReg = TRUE;
            m_tBrdReged[byLoop].tBrdPos.byBrdLayer = tBrdPos.byBrdLayer;
            m_tBrdReged[byLoop].tBrdPos.byBrdSlot = tBrdPos.byBrdSlot;
            m_tBrdReged[byLoop].tBrdPos.byBrdID = tBrdPos.byBrdID;
            m_tBrdReged[byLoop].dwNode = dwNode;
            m_tBrdReged[byLoop].dwDstInsId = dwInsId;
            m_wRegedBrdNum++;
            break;
        }
    }
*/
    return wRet;
}

/*=============================================================================
  �� �� ���� DelBoard
  ��    �ܣ� ɾ��һ�鵥��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TBrdPosition tBrdPos
   �� �� ֵ�� u16  
=============================================================================*/
u16 CRegedBoard::DelBoard(TBrdPosition tBrdPos)
{
	ENTER( m_hBoardManager );

    for(u8 byLoop = 0; byLoop < MAX_BOARD_NUM; byLoop++)
    {
        if( 0 == memcmp( &tBrdPos, &m_tBrdReged[byLoop].tBrdPos, sizeof(tBrdPos) ) )
        {
            m_tBrdReged[byLoop].bReg = FALSE;
            m_tBrdReged[byLoop].tBrdPos.byBrdLayer = 0;
            m_tBrdReged[byLoop].tBrdPos.byBrdSlot = 0;
            m_tBrdReged[byLoop].tBrdPos.byBrdID = 0;
            m_tBrdReged[byLoop].dwNode = INVALID_NODE;
            m_tBrdReged[byLoop].dwDstInsId = INVALID_INS;
            m_wRegedBrdNum--;

			
            Agtlog( LOG_INFORM, "[DelBoard] Brd<%d,%d:%s> has been del succeed !\n",
				tBrdPos.byBrdLayer, tBrdPos.byBrdSlot, 
                                 g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );
            break;

        }
    }
    return SUCCESS_AGENT;
}
/*lint -restore*/

/*=============================================================================
  �� �� ���� GetImageFileStatus
  ��    �ܣ� ���ĳ����� .image �ļ�������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TBrdPosition tBrdPos
  �� �� ֵ�� u8
=============================================================================*/
u8 CRegedBoard::GetImageFileStatus( TBrdPosition tBrdPos )
{
    BOOL32 bGet = FALSE;
    u8     byStatus  = SNMP_FILE_UPDATE_INIT;
    s32    nIndex = 0;

    switch( tBrdPos.byBrdID )
    {
	//  [1/21/2011 chendaiwei]֧��MPC2
    case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:
	case BRD_TYPE_MPC2:
        {
            TMPCInfo tMPCInfo;
            g_cCfgParse.GetMPCInfo( &tMPCInfo );
            if ( tBrdPos.byBrdLayer == tMPCInfo.GetLocalLayer() &&
                 tBrdPos.byBrdSlot  == tMPCInfo.GetLocalSlot() )
            {
                byStatus = m_abyMpcFileStatus[0];
                bGet = TRUE;
            }
        }
    	break;

    default:
        for( ; nIndex < MAX_BOARD_NUM; nIndex ++ )
        {
            if ( 0 == memcmp( &tBrdPos, &m_tBrdReged[nIndex].tBrdPos, sizeof(tBrdPos) ) )
            {
                byStatus = m_tBrdReged[nIndex].m_abyBrdFileStatus[0];
                Agtlog(LOG_INFORM, "[GetImageFileStatus] get Brd<%d,%d:%s> image status: %d\n",
                                    tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                                    g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID), byStatus );
                bGet = TRUE;
                
            }                
        }
        break;
    }
    if ( !bGet )
    {
        Agtlog(LOG_ERROR, "[GetImageFileStatus] board<%d,%d:%s> is unexist !\n",
                            tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                            g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );
        byStatus = 255;
    }
    
    return byStatus;
}

/*=============================================================================
  �� �� ���� GetBinFileStatus
  ��    �ܣ� ���ĳ����� .bin �ļ�������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TBrdPosition tBrdPos
  �� �� ֵ�� u8
-----------------------------------------------------------------------
    ����        �汾      ������        �޸���     �� ��
  2009/03/18    4.6       ���ֱ�                   �� ��
=============================================================================*/
u8 CRegedBoard::GetBinFileStatus( TBrdPosition tBrdPos )
{
    BOOL32 bGet = FALSE;
    u8     byStatus  = SNMP_FILE_UPDATE_INIT;
    s32    nIndex = 0;

    switch( tBrdPos.byBrdID )
    {
	//  [1/21/2011 chendaiwei]֧��MPC2
    case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:		
	case BRD_TYPE_MPC2:
        {
            TMPCInfo tMPCInfo;
            g_cCfgParse.GetMPCInfo( &tMPCInfo );
            if ( tBrdPos.byBrdLayer == tMPCInfo.GetLocalLayer() &&
                 tBrdPos.byBrdSlot  == tMPCInfo.GetLocalSlot() )
            {
                byStatus = m_abyMpcFileStatus[0];
                bGet = TRUE;
            }
        }
    	break;

    default:
        for( ; nIndex < MAX_BOARD_NUM; nIndex ++ )
        {
            if ( 0 == memcmp( &tBrdPos, &m_tBrdReged[nIndex].tBrdPos, sizeof(tBrdPos) ) )
            {
                byStatus = m_tBrdReged[nIndex].m_abyBrdFileStatus[0];
                Agtlog(LOG_INFORM, "[GetBinFileStatus] get Brd<%d,%d:%s> bin status: %d\n",
                                    tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                                    g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID), byStatus );
                bGet = TRUE;
                
            }                
        }
        break;
    }
    if ( !bGet )
    {
        Agtlog(LOG_ERROR, "[GetBinFileStatus] board<%d,%d:%s> is unexist !\n",
                            tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                            g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );
        byStatus = 255;
    }
    
    return byStatus;
}


/*=============================================================================
  �� �� ���� SetBinFileStatus
  ��    �ܣ� ����ĳ����� .bin �ļ�������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TBrdPosition tBrdPos
             u8           byStatus
  �� �� ֵ�� void
-----------------------------------------------------------------------
    ����        �汾      ������        �޸���     ����
  2009/03/18    4.6       ���ֱ�                   ����
=============================================================================*/
void CRegedBoard::SetBinFileStatus(TBrdPosition tBrdPos, u8 byStatus)
{
    BOOL32 bSet   = FALSE;
    s32    nIndex = 0;

    switch( tBrdPos.byBrdID )
    {

	//  [1/21/2011 chendaiwei]֧��MPC2
    case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:		
	case BRD_TYPE_MPC2:
        {
            TMPCInfo tMPCInfo;
            g_cCfgParse.GetMPCInfo( &tMPCInfo );
            if ( tBrdPos.byBrdLayer == tMPCInfo.GetLocalLayer() &&
                 tBrdPos.byBrdSlot  == tMPCInfo.GetLocalSlot() )
            {
                m_abyMpcFileStatus[0] = byStatus;
                bSet = TRUE;
            }
        }
    	break;
    default:
        for( ; nIndex < MAX_BOARD_NUM; nIndex ++ )
        {
        if ( 0 == memcmp( &tBrdPos, &m_tBrdReged[nIndex].tBrdPos, sizeof(tBrdPos) ) )            
        {
                m_tBrdReged[nIndex].m_abyBrdFileStatus[0] = byStatus;
                Agtlog(LOG_INFORM, "[SetBinFileStatus] set Brd<%d,%d:%s> bin status: %d\n",
                                    tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                                    g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID), byStatus );
                bSet = TRUE;
                break;
            }
        }
        break;
    }
    if ( !bSet )
    {
        Agtlog(LOG_ERROR, "[SetBinFileStatus] board<%d,%d:%s> is unexist !\n",
                            tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                            g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );
    }
    return;
}



/*=============================================================================
  �� �� ���� SetImageFileStatus
  ��    �ܣ� ����ĳ����� .image �ļ�������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TBrdPosition tBrdPos
             u8           byStatus
  �� �� ֵ�� void
=============================================================================*/
void CRegedBoard::SetImageFileStatus(TBrdPosition tBrdPos, u8 byStatus)
{
    BOOL32 bSet   = FALSE;
    s32    nIndex = 0;

    switch( tBrdPos.byBrdID )
    {
	//  [1/21/2011 chendaiwei]֧��MPC2
    case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:		
	case BRD_TYPE_MPC2:
        {
            TMPCInfo tMPCInfo;
            g_cCfgParse.GetMPCInfo( &tMPCInfo );
            if ( tBrdPos.byBrdLayer == tMPCInfo.GetLocalLayer() &&
                 tBrdPos.byBrdSlot  == tMPCInfo.GetLocalSlot() )
            {
                m_abyMpcFileStatus[0] = byStatus;
                bSet = TRUE;
            }
        }
    	break;
    default:
        for( ; nIndex < MAX_BOARD_NUM; nIndex ++ )
        {
        if ( 0 == memcmp( &tBrdPos, &m_tBrdReged[nIndex].tBrdPos, sizeof(tBrdPos) ) )            
        {
                m_tBrdReged[nIndex].m_abyBrdFileStatus[0] = byStatus;
                Agtlog(LOG_INFORM, "[SetImageFileStatus] set Brd<%d,%d:%s> image status: %d\n",
                                    tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                                    g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID), byStatus );
                bSet = TRUE;
                break;
            }
        }
        break;
    }
    if ( !bSet )
    {
        Agtlog(LOG_ERROR, "[SetImageFileStatus] board<%d,%d:%s> is unexist !\n",
                            tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                            g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );
    }
    return;
}

/*=============================================================================
  �� �� ���� GetOSFileStatus
  ��    �ܣ� ���ĳ����� .linux �ļ�������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TBrdPosition tBrdPos
  �� �� ֵ�� u8
=============================================================================*/
u8 CRegedBoard::GetOSFileStatus( TBrdPosition tBrdPos )
{
    BOOL32 bGet = FALSE;
    u8     byStatus  = SNMP_FILE_UPDATE_INIT;
    s32    nIndex = 0;

    switch( tBrdPos.byBrdID )
    {
	//  [1/21/2011 chendaiwei]֧��MPC2
    case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:		
	case BRD_TYPE_MPC2:
        {
            TMPCInfo tMPCInfo;
            g_cCfgParse.GetMPCInfo( &tMPCInfo );
            if ( tBrdPos.byBrdLayer == tMPCInfo.GetLocalLayer() &&
                 tBrdPos.byBrdSlot  == tMPCInfo.GetLocalSlot() )
            {
                byStatus = m_abyMpcFileStatus[1];
                bGet = TRUE;
            }
        }
        break;
    default:
        for( ; nIndex < MAX_BOARD_NUM; nIndex ++ )
        {
            if ( 0 == memcmp( &tBrdPos, &m_tBrdReged[nIndex].tBrdPos, sizeof(tBrdPos) ) )
            {
                byStatus = m_tBrdReged[nIndex].m_abyBrdFileStatus[1];
                Agtlog(LOG_INFORM, "[GetOSFileStatus] get Brd<%d,%d:%s> IOS status: %d\n",
                                    tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                                    g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID), byStatus );
                bGet = TRUE;
                break;
            }
        }
        break;
    }
    if ( !bGet )
    {
        Agtlog(LOG_ERROR, "[GetOSFileStatus] reged board<%d,%d:%s> is unexist !\n",
                            tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                            g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );
        byStatus = 255;
    }
    return byStatus;
}

/*=============================================================================
  �� �� ���� SetOSFileStatus
  ��    �ܣ� ����ĳ����� .linux �ļ�������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TBrdPosition tBrdPos
             u8           byStatus
  �� �� ֵ�� void
=============================================================================*/
void CRegedBoard::SetOSFileStatus( TBrdPosition tBrdPos, u8 byStatus )
{
    BOOL32 bSet = FALSE;
    s32    nIndex = 0;
    
    switch( tBrdPos.byBrdID )
    {
	//  [1/21/2011 chendaiwei]֧��MPC2
    case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:		
	case BRD_TYPE_MPC2:
        {
            TMPCInfo tMPCInfo;
            g_cCfgParse.GetMPCInfo( &tMPCInfo );
            if ( tBrdPos.byBrdLayer == tMPCInfo.GetLocalLayer() &&
                tBrdPos.byBrdSlot   == tMPCInfo.GetLocalSlot() )
            {
                m_abyMpcFileStatus[1] = byStatus;
                bSet = TRUE;
            }
            break;
        }

    default:
        for( ; nIndex < MAX_BOARD_NUM; nIndex ++ )
        {
            if ( 0 == memcmp( &tBrdPos, &m_tBrdReged[nIndex].tBrdPos, sizeof(tBrdPos) ) )
            {
                m_tBrdReged[nIndex].m_abyBrdFileStatus[1] = byStatus;
                Agtlog(LOG_INFORM, "[SetOSFileStatus] set Brd<%d,%d:%s> IOS status: %d\n",
                                    tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                                    g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID), byStatus );
                bSet = TRUE;
                break;
            }
        }
        break;
    }
    if ( !bSet )
    {
        Agtlog(LOG_ERROR, "[SetOSFileStatus] reged board<%d,%d:%s> is unexist !\n",
                            tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                            g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );
    }
    return;
}

/*=============================================================================
  �� �� ���� ShowRegedBoard
  ��    �ܣ� ��ʾ�Ѿ�ע�ᵥ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CRegedBoard::ShowRegedBoard(void)
{
    OspPrintf(TRUE, FALSE, "The registered board num: %d\n", m_wRegedBrdNum);
    OspPrintf(TRUE, FALSE, "------------------------------------------------\n");
    OspPrintf(TRUE, FALSE, "No.\tLayer\tSlot\tID(TYPE)\tIpAddr\n");
    for(u8 byLoop = 0 ;byLoop < MAX_BOARD_NUM; byLoop++)
    {
        if( m_tBrdReged[byLoop].bReg )
        {
            OspPrintf( TRUE, FALSE, "%d\t%d\t%d\t%d(%s)\t%x\n", 
                        byLoop + 1,
                        m_tBrdReged[byLoop].tBrdPos.byBrdLayer,
                        m_tBrdReged[byLoop].tBrdPos.byBrdSlot, 
                        m_tBrdReged[byLoop].tBrdPos.byBrdID,
                        g_cCfgParse.GetBrdTypeStr(m_tBrdReged[byLoop].tBrdPos.byBrdID),
					    OspNodeIpGet(m_tBrdReged[byLoop].dwNode) );
        }
    }
    OspPrintf(TRUE, FALSE, "------------------------------------------------\n");
    return;
}

/*=============================================================================
  �� �� ���� ShowSemHandle
  ��    �ܣ� ��ʾ���ź������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CRegedBoard::ShowSemHandle(void)
{
    OspPrintf( TRUE, FALSE, "Alarm Table Handle: 0x%x \n", m_hBoardManager );
    return;
}

/*=============================================================================
  �� �� ���� AgtQueryBrdPosition
  ��    �ܣ� ��ѯMPC�忨�Ĳ�ۼ�ID��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� [IN/OUT]TBrdPosition *ptBrdPosition
  �� �� ֵ�� s32 (ERROR OK)
=============================================================================*/
//֧��8000E ��װbrdwrapper����Ҫ�ĵ�����ز�ѯ�ӿ�
s32 AgtQueryBrdPosition(TBrdPosition *ptBrdPosition)
{
	if (NULL == ptBrdPosition)
	{
		OspPrintf(TRUE, FALSE, "[AgtQueryBrdPosition]bad input param");
		return ERROR;
	}

	u32 wResult = OK;
#if defined(_MINIMCU_) || defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	ptBrdPosition->byBrdLayer = 0;
	ptBrdPosition->byBrdSlot  = 0;
	ptBrdPosition->byBrdID    = BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;
#else
	wResult = BrdQueryPosition(ptBrdPosition);
#endif

	return wResult;
}
/*=============================================================================
  �� �� ���� AgtGetBrdEthParamAll
  ��    �ܣ� ��Զ��IP��ַ�����,��ȡһ�����������е���̫��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� [IN]     u8 byEthId: ָ�����ں�
             [IN/OUT]TBrdEthParamAll *ptBrdEthParamAll: ��̫������
  �� �� ֵ�� s32 (ERROR OK)
=============================================================================*/
s32 AgtGetBrdEthParamAll(u8 byEthId, TBrdEthParamAll *ptBrdEthParamAll)
{
	if (NULL == ptBrdEthParamAll)
	{
		OspPrintf(TRUE, FALSE, "[AgtGetBrdEthParamAll]bad input param");
		return ERROR;
	}

	s32 nResult = OK;                                  
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	TMcuBrdEthParamAll tMcuEthParamAll;
	if(GetAdapterIp(byEthId, &tMcuEthParamAll) != TRUE)
	{
		nResult = ERROR;
	}
	else
	{
		ptBrdEthParamAll->dwIpNum = tMcuEthParamAll.dwIpNum;
		for (u8 byIdx = 0; byIdx < tMcuEthParamAll.dwIpNum; byIdx++)
		{
			if (byIdx >= IP_ADDR_MAX_NUM)
			{
				break;
			}
			ptBrdEthParamAll->atBrdEthParam[byIdx].dwIpAdrs = tMcuEthParamAll.atBrdEthParam[byIdx].dwIpAdrs;
			ptBrdEthParamAll->atBrdEthParam[byIdx].dwIpMask = tMcuEthParamAll.atBrdEthParam[byIdx].dwIpMask;
			memcpy(ptBrdEthParamAll->atBrdEthParam[byIdx].byMacAdrs, tMcuEthParamAll.atBrdEthParam[byIdx].byMacAdrs, 
				   sizeof(tMcuEthParamAll.atBrdEthParam[byIdx].byMacAdrs));
		}
	}
#else
	nResult = BrdGetEthParamAll(byEthId, ptBrdEthParamAll);
#endif

	return nResult;
}
/*=============================================================================
  �� �� ���� AgtGetBrdEthParam
  ��    �ܣ� ��ȡָ������MPC����ʹ�õ�IP��ַ������8000EΪ���0��ַ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� [IN]     u8 byEthId: ָ�����ں�
             [IN/OUT]TBrdEthParam *ptBrdEthParam: ��̫������
  �� �� ֵ�� s32 (ERROR OK)
=============================================================================*/
s32 AgtGetBrdEthParam(u8 byEthId, TBrdEthParam *ptBrdEthParam)
{
	if (NULL == ptBrdEthParam)
	{
		OspPrintf(TRUE, FALSE, "[AgtGetBrdEthParam]bad input param");
		return ERROR;
	}

	s32 nResult = OK;
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	TMcuBrdEthParamAll tBrdEthParamAll;
	if(GetAdapterIp(byEthId, &tBrdEthParamAll) != TRUE)
	{
		nResult = ERROR;
	}
	else
	{
		ptBrdEthParam->dwIpAdrs = tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs;

		ptBrdEthParam->dwIpMask = tBrdEthParamAll.atBrdEthParam[0].dwIpMask;        // С����(������)
        ptBrdEthParam->dwIpMask = htonl( ptBrdEthParam->dwIpMask );

		memcpy(ptBrdEthParam->byMacAdrs, tBrdEthParamAll.atBrdEthParam[0].byMacAdrs, 
			sizeof(tBrdEthParamAll.atBrdEthParam[0].byMacAdrs));
	}
#else
	nResult = BrdGetEthParam(byEthId, ptBrdEthParam);    // �����(������)
#endif
	return nResult;
}

/*=============================================================================
  �� �� ���� AgtGetMpcGateway
  ��    �ܣ� ��ȡMpc�������IP��ַ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwIpGateway
  �� �� ֵ�� u32 
  ----------------------------------------------------------------------------
  �޸ļ�¼��
  ��   ��        �汾        �޸���        �޸�����
  2011/01/12     5.0         ������          ����
=============================================================================*/
u32 AgtGetMpcGateway( u32 dwIfIndex )
{
	u32 dwIpGateway = 0;

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	//zjj20120515 8000G��8000Hͬһ��ʱ��ֻ��һ�������ڹ�����
	//����ֻ�ܻ��һ������ڵ���Ϣ�����Ҿ��ǵ�ǰ���������ڣ�����ֱ��ȡ����0��������Ϣ
	TNetAdaptInfoAll tNetAdapterInfoAll;
	if ( GetNetAdapterInfo( &tNetAdapterInfoAll ) )
	{
		TNetAdaptInfo *ptNetAdapt = NULL;
		
#if defined(_8KH_)
		if(g_cCfgParse.Is8000HmMcu())
		{
			for(u8 byIdx = 0; byIdx <tNetAdapterInfoAll.m_tLinkNetAdapt.Length(); byIdx++)
			{
				ptNetAdapt = tNetAdapterInfoAll.m_tLinkNetAdapt.GetValue(byIdx);
				 
				//8000H-Mʼ��ȡ����1������[5/6/2013 chendaiwei]
				if(ptNetAdapt != NULL && ptNetAdapt->GetAdaptIdx() == 1)
				{
					dwIpGateway = ptNetAdapt->GetDefGWIpAddr();    // С����(������)

					break;
				}
			}
		}
		else
#endif
		{
			ptNetAdapt = tNetAdapterInfoAll.m_tLinkNetAdapt.GetValue(0);
			
			if (ptNetAdapt != NULL)
			{
				dwIpGateway = ptNetAdapt->GetDefGWIpAddr();    // С����(������)
    		}
		}
	}
#else
	dwIpGateway = BrdGetDefGateway();   // �����(������)
#endif
	
	Agtlog(LOG_LVL_DETAIL,"[AgtGetMpcGateway] dwIfIndex.%d\n",dwIfIndex);

	return dwIpGateway;
}
// END OF FILE
