/*****************************************************************************
   ģ����      : TvWall
   �ļ���      : TWInst.cpp
   ����ļ�    : TWInst.h
   �ļ�ʵ�ֹ���: ����ǽ����ʵ��
   ����        : ������
   �汾        : V1.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/11/10  2.0         ������      ����
   2005/01/12  3.6         libo        ���Ӽ���
   2006/03/21  4.0         �ű���      �����Ż�
******************************************************************************/
#include "twinst.h"
#include "evtvwall.h"
#include "evmcueqp.h"
#include "errtw.h"
#include "boardagent.h"
#include "mcuver.h"
/*lint -save -e843 -e714 -e765*/
#ifdef WIN32
static char	CFG_FILE[] = "conf/mcueqp.ini";
#else
static char	CFG_FILE[] = "/usr/etc/config/conf/mcueqp.ini";
#endif
static char    SECTION_DEBUG[] = "EQPDEBUG";
s32  g_nlog = 0;

static void tvlog( s8* pszFmt, ...)
{
	/*lint -save -e438 -e737*/
    s8 achPrintBuf[KDV_MAX_PATH];
    s32  nBufLen = 0;
    va_list argptr;
	if( g_nlog == 1 )
	{		  
		nBufLen = sprintf( achPrintBuf, "[TvWall]:" ); 
		va_start( argptr, pszFmt );
		//nBufLen += vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
		vsnprintf(achPrintBuf + nBufLen, KDV_MAX_PATH - nBufLen - 1, pszFmt, argptr);
		va_end(argptr); 
		OspPrintf( TRUE, FALSE, achPrintBuf ); 
	}
    return;
	/*lint -restore*/
}

CTWInst::CTWInst()
{
    m_byTvNum = 0;
    m_byTvModel = 0;
    memset(&m_tTWStatus, 0, sizeof(m_tTWStatus));
    memset(&m_tCfg, 0, sizeof(m_tCfg));
    memset(&m_tPrsTimeSpan, 0, sizeof(m_tPrsTimeSpan));
}

CTWInst::~CTWInst()
{
}

/*====================================================================
    ������      ��InstanceEntry
    ����        ����Ϣ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pMsg - ��Ϣָ��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
    05/01/12    3.6         Li Bo         �����޸�
====================================================================*/
void CTWInst::InstanceEntry(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);

    tvlog("Recv msg %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event));

    switch(pMsg->event) 
    {
    //�������ϵ��ʼ��
    case EV_TVWALL_INIT:
        if (!Init(pMsg))
        {
            OspPrintf(TRUE, FALSE, "[TW] Init the TvWall Error\n");
            return;
        }
        break;

    // ����
    case EV_TVWALL_CONNECT:
        ProcConnectTimeOut(TRUE);
        break;

    case EV_TVWALL_CONNECTB:
        ProcConnectTimeOut(FALSE);
        break;

    //�������:
    case MCU_EQP_ROUNDTRIP_REQ:
        SendMsgToMcu(EQP_MCU_ROUNDTRIP_ACK, cServMsg);
        break;

	//MCU ֪ͨ�Ự ����MPCB ��Ӧʵ��, zgc, 2007/04/30
	case MCU_EQP_RECONNECTB_CMD:
		ProcReconnectBCmd( pMsg );
		break;

    // ע����Ϣ
    case EV_TVWALL_REGISTER:  	
        ProcRegisterTimeOut(TRUE);
        break;
    case EV_TVWALL_REGISTERB:  	
        ProcRegisterTimeOut(FALSE);
        break;

    // MCU ע��Ӧ����Ϣ
    case MCU_TVWALL_REG_ACK:
        MsgRegAckProc(pMsg);
        break;

    // MCU�ܾ�ע��
    case MCU_TVWALL_REG_NACK:
        MsgRegNackProc(pMsg);
        break;

    //OSP ������Ϣ
    case OSP_DISCONNECT:
        MsgDisconnectProc(pMsg);
        break;

    //��ʼ��ʾ
    case MCU_TVWALL_START_PLAY_REQ:
        ProcStartPlayMsg(pMsg);
        break;

    //ֹͣ��ʾ
    case MCU_TVWALL_STOP_PLAY_REQ:
        ProcStopPlayMsg(pMsg);
        break;

    // ����Qosֵ
    case MCU_EQP_SETQOS_CMD:
        ProcSetQosInfo(pMsg);
        break;

	case EV_TVWALL_GETMSSTATUS_TIMER:
	case MCU_EQP_GETMSSTATUS_ACK:
		ProcGetMsStatusMsg(pMsg);
		break;

    //������ʾ
    case EV_TVWALL_CONFIGSHOW:
        ConfigShow();
        break;

    //״̬��ʾ
    case EV_TVWALL_STATUSSHOW:
        StatusShow();
        break;
	default:
		{
			OspPrintf(TRUE, FALSE, "[TW] Rcv unknow message. msg id= %d.\n", pMsg->event);
		}
		break;
    }
}

/*=============================================================================
  �� �� ���� ProcConnectTimeOut
  ��    �ܣ� ����ע�ᳬʱ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� BOOL32 bIsConnectA
  �� �� ֵ�� void 
=============================================================================*/
void CTWInst::ProcConnectTimeOut(BOOL32 bIsConnectA)
{
    BOOL32 bRet = FALSE;
    if(TRUE == bIsConnectA)
    {
        bRet = ConnectMcu(bIsConnectA, g_cTvWallApp.m_dwMcuNode);
        if(TRUE == bRet)
        {
            SetTimer(EV_TVWALL_REGISTER, TV_REGISTER_TIMEOUT);
        }
        else
        {
            SetTimer(EV_TVWALL_CONNECT, TV_CONNETC_TIMEOUT);
        }
    }
    else
    {
        bRet = ConnectMcu(bIsConnectA, g_cTvWallApp.m_dwMcuNodeB);
        if(TRUE == bRet)
        {
            SetTimer(EV_TVWALL_REGISTERB, TV_REGISTER_TIMEOUT);
        }
        else
        {
            SetTimer(EV_TVWALL_CONNECTB, TV_CONNETC_TIMEOUT);
        }
    }
    return;
}

/*====================================================================
    ������      ��ConnectMcu
    ����        ������MCU
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
====================================================================*/
BOOL32 CTWInst::ConnectMcu(BOOL32 bIsConnectA, u32& dwMcuNode)
{
    BOOL32 bRet = TRUE;
    if( !OspIsValidTcpNode(dwMcuNode) )
    {
        if(TRUE == bIsConnectA)
        {    
            dwMcuNode = BrdGetDstMcuNode(); 
        }
        else
        {   
            dwMcuNode = BrdGetDstMcuNodeB(); 
        }

        if (::OspIsValidTcpNode(dwMcuNode))
        {
            tvlog("Connect Mcu%d Success!\n", m_tCfg.wMcuId);
            ::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());   //��ʵ�����������Ϣ
        }
        else 
        {
            OspPrintf(TRUE, FALSE, "[TW]Connect to Mcu%d failed,retry after 5s!\n", m_tCfg.wMcuId);
            bRet = FALSE;
        }
    }

    return bRet;
}

/*=============================================================================
  �� �� ���� ProcRegisterTimeOut
  ��    �ܣ� ����ע�ᳬʱ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� BOOL32 bIsRegisterA
  �� �� ֵ�� void 
=============================================================================*/
void CTWInst::ProcRegisterTimeOut(BOOL32 bIsRegisterA)
{
    if(TRUE == bIsRegisterA)
    {
        Register(bIsRegisterA, g_cTvWallApp.m_dwMcuNode);
        SetTimer(EV_TVWALL_REGISTER, TV_REGISTER_TIMEOUT);
    }
    else
    {
        Register(bIsRegisterA, g_cTvWallApp.m_dwMcuNodeB);
        SetTimer(EV_TVWALL_REGISTERB, TV_REGISTER_TIMEOUT);
    }
    return;
}
/*====================================================================
    ������      ��Register
    ����        ����MCUע��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
====================================================================*/
void CTWInst::Register(BOOL32 bIsRegisterA, u32 dwMcuNode)
{
    CServMsg cSvrMsg;
    TPeriEqpRegReq tReg;

    tReg.SetEqpId(m_tCfg.byEqpId);
    tReg.SetEqpType(m_tCfg.byEqpType);
    tReg.SetChnnlNum(m_byTvNum);
    tReg.SetEqpAlias(m_tCfg.achAlias);
    tReg.SetPeriEqpIpAddr(m_tCfg.dwLocalIP);
    tReg.SetStartPort(m_tCfg.wRcvStartPort);
    tReg.SetVersion(DEVVER_TW);
    
    if(TRUE == bIsRegisterA)
    {
        tReg.SetMcuId((u8)m_tCfg.wMcuId );
    }
    else
    {
        tReg.SetMcuId((u8)m_tCfg.wMcuId );
    }

    cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
        
    post(MAKEIID(AID_MCU_PERIEQPSSN, m_tCfg.byEqpId),
                TVWALL_MCU_REG_REQ,
                cSvrMsg.GetServMsg(), 
                cSvrMsg.GetServMsgLen(), 
                dwMcuNode);
	    
    tvlog("Regist to MCU%d\t, Id=%u !(%d PICs)\n",
                m_tCfg.wMcuId,
				m_tCfg.byEqpId,
				m_byTvNum );
	return;
}

/*====================================================================
    ������      ��MsgRegAckProc
    ����        ��ע��Ӧ����Ϣ�Ĵ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
====================================================================*/
void CTWInst::MsgRegAckProc(CMessage* const pMsg)
{
    if(g_cTvWallApp.m_dwMcuNode == pMsg->srcnode)
    {
        g_cTvWallApp.m_dwMcuIId = pMsg->srcid;
        g_cTvWallApp.m_byRegAckNum ++;
        KillTimer(EV_TVWALL_REGISTER);
        tvlog("Register Mcu%d  success!\n", m_tCfg.wMcuId); // mcu id ͳһΪ192
    }
    else if(g_cTvWallApp.m_dwMcuNodeB == pMsg->srcnode)
    {
        g_cTvWallApp.m_dwMcuIIdB = pMsg->srcid;
        g_cTvWallApp.m_byRegAckNum++;
        KillTimer(EV_TVWALL_REGISTERB);
        tvlog("Register Mcu%d  success!\n", m_tCfg.wMcuId);
    }

	CServMsg cServMsg(pMsg->content, pMsg->length);
    TPeriEqpRegAck tRegAck = *(TPeriEqpRegAck*)cServMsg.GetMsgBody();
    // guzh [6/12/2007] У��Ự����
    if ( g_cTvWallApp.m_dwMpcSSrc == 0 )
    {
        g_cTvWallApp.m_dwMpcSSrc = tRegAck.GetMSSsrc();
    }
    else
    {
        // �쳣������Ͽ������ڵ�
        if ( g_cTvWallApp.m_dwMpcSSrc != tRegAck.GetMSSsrc() )
        {
            OspPrintf(TRUE, FALSE, "[MsgRegAckProc] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
                       g_cTvWallApp.m_dwMpcSSrc, tRegAck.GetMSSsrc());
            if ( OspIsValidTcpNode(g_cTvWallApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cTvWallApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cTvWallApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cTvWallApp.m_dwMcuNodeB);
            }  
			return;
        }
    }
   
    if(FIRST_REGACK == g_cTvWallApp.m_byRegAckNum)
    {
        NEXTSTATE(u32(eNORMAL));//ʵ������ NORMAL ״̬
        m_tTWStatus.byChnnlNum = m_byTvNum ;
        
        m_tPrsTimeSpan = *(TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck));
    }

    SendNotify();

    return;
}

/*====================================================================
    ������      ��MsgRegNackProc
    ����        ��ע��ܾ���Ϣ�Ĵ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
====================================================================*/
void CTWInst::MsgRegNackProc(CMessage* const pMsg)
{
    if(pMsg->srcnode == g_cTvWallApp.m_dwMcuNode)
    {
        OspPrintf(TRUE, FALSE, "[TW] 1 TV Wall registe be refused by Mcu%d\n", m_tCfg.wMcuId);
    }
    else if(pMsg->srcnode == g_cTvWallApp.m_dwMcuNodeB)
    {
        OspPrintf(TRUE, FALSE, "[TW] 2 TV Wall registe be refused by Mcu%d\n", m_tCfg.wMcuId);
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[TvWall] Recv unknown register Nack message\n");
    }
    return;
}

/*====================================================================
    ������      ��SendMsgToMcu
    ����        ����MCU������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
====================================================================*/
BOOL CTWInst::SendMsgToMcu(u16 wEvent, CServMsg const &cServMsg)
{
    if (g_cTvWallApp.m_bEmbed || OspIsValidTcpNode(g_cTvWallApp.m_dwMcuNode))
    {
        post(g_cTvWallApp.m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cTvWallApp.m_dwMcuNode);
        tvlog("Send Message %u(%s) to Mcu%d\n",wEvent, ::OspEventDesc(wEvent), m_tCfg.wMcuId);
       
    }
    
    if( g_cTvWallApp.m_bEmbedB || OspIsValidTcpNode(g_cTvWallApp.m_dwMcuNodeB))
    {
        post(g_cTvWallApp.m_dwMcuIIdB, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cTvWallApp.m_dwMcuNodeB);
        tvlog("Send Message %u(%s) to Mcu%d\n", wEvent, ::OspEventDesc(wEvent), m_tCfg.wMcuId);
    }
    else
    {
/*       
		OspPrintf(TRUE, FALSE, "[TW] Send Message failed %u(%s), since disconnected with Mcu%d or Mcu%d.\n",
					wEvent,
					::OspEventDesc(wEvent),
					m_tCfg.wMcuId,
					m_tCfg.wMcuId);
        return FALSE;
*/
    }
    return TRUE;
}

/*====================================================================
    ������      ��SendNotify
    ����        ������ǽ״̬�ϱ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
====================================================================*/
void CTWInst::SendNotify()
{
	TPeriEqpStatus tEqpStatus;
	tEqpStatus.m_byOnline = TRUE;
	
	tEqpStatus.m_tStatus.tTvWall = m_tTWStatus;
	tEqpStatus.SetMcuEqp((u8)m_tCfg.wMcuId, m_tCfg.byEqpId, m_tCfg.byEqpType);

    tEqpStatus.SetAlias(m_tCfg.achAlias);

	CServMsg cServMsg;
	cServMsg.SetMsgBody((u8*)&tEqpStatus, sizeof(tEqpStatus));
	SendMsgToMcu(TVWALL_MCU_STATUS_NOTIF, cServMsg);
}

/*====================================================================
    ������      ��MsgDisconnectProc
    ����        ��������Ϣ�Ĵ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
====================================================================*/
void CTWInst::MsgDisconnectProc(CMessage* const pMsg)
{
    u32 dwDisNode = *(u32*)pMsg->content;

    if (INVALID_NODE != dwDisNode)
    {
        OspDisconnectTcpNode(dwDisNode);
    }

    if(dwDisNode == g_cTvWallApp.m_dwMcuNode)
    {
        g_cTvWallApp.FreeStatusDataA();
        SetTimer(EV_TVWALL_CONNECT, TV_CONNETC_TIMEOUT);
    }
    else if(dwDisNode == g_cTvWallApp.m_dwMcuNodeB)
    {
        g_cTvWallApp.FreeStatusDataB();
        SetTimer(EV_TVWALL_CONNECTB, TV_CONNETC_TIMEOUT);
    }
	else
	{
		OspPrintf(TRUE, FALSE, "[MsgDisconnectProc] g_cTvWallApp.m_dwMcuNode.%d, g_cTvWallApp.m_dwMcuNodeB.%d, dwDisNode.%d!\n",
				g_cTvWallApp.m_dwMcuNode, g_cTvWallApp.m_dwMcuNodeB, dwDisNode);

		printf("[MsgDisconnectProc] g_cTvWallApp.m_dwMcuNode.%d, g_cTvWallApp.m_dwMcuNodeB.%d, dwDisNode.%d!\n",
				g_cTvWallApp.m_dwMcuNode, g_cTvWallApp.m_dwMcuNodeB, dwDisNode);
	}

	// ������һ����������Mcuȡ��������״̬���ж��Ƿ�ɹ�
	if (INVALID_NODE != g_cTvWallApp.m_dwMcuNode || INVALID_NODE != g_cTvWallApp.m_dwMcuNodeB)
	{
		if (OspIsValidTcpNode(g_cTvWallApp.m_dwMcuNode))
		{
			post( g_cTvWallApp.m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cTvWallApp.m_dwMcuNode );     
        
			OspPrintf(TRUE, FALSE, "[MsgDisconnectProc] GetMsStatusReq. McuNode.A\n");
		}
		else if (OspIsValidTcpNode(g_cTvWallApp.m_dwMcuNodeB))
		{
			post( g_cTvWallApp.m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cTvWallApp.m_dwMcuNodeB );        

			OspPrintf(TRUE, FALSE, "[MsgDisconnectProc] GetMsStatusReq. McuNode.B\n");
		}

		SetTimer(EV_TVWALL_GETMSSTATUS_TIMER, WAITING_MSSTATUS_TIMEOUT);
		return;
	}
	
	// SetTimer(EV_TVWALL_CONNECT, 5*1000);    // 5���������MCU����
    if(INVALID_NODE == g_cTvWallApp.m_dwMcuNode && INVALID_NODE == g_cTvWallApp.m_dwMcuNodeB)
    {
        g_cTvWallApp.m_byRegAckNum = 0;

		g_cTvWallApp.m_dwMpcSSrc = 0;
		
		//ʵ���������״̬
        NEXTSTATE(u32(eIDLE)); 
		
		//ֹͣ����ͨ��
		StopAllChannels();

		//��յ���ǽͨ��״̬�������ܵ���ǽ�����ģʽ 2006-10-18
		u8 byOutputMode = m_tTWStatus.byOutputMode;
		memset(&m_tTWStatus, 0, sizeof(m_tTWStatus));
		m_tTWStatus.byOutputMode = byOutputMode;
    }
    return;
}

/*=============================================================================
�� �� ���� ProcReconnectBCmd
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/4/30   4.0			�ܹ��                  ����
=============================================================================*/
void CTWInst::ProcReconnectBCmd( CMessage * const pcMsg )
{
	if( pcMsg == NULL )
	{
		return;
	}
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u32 dwMpcBIp = *(u32*)cServMsg.GetMsgBody();
	dwMpcBIp = ntohl(dwMpcBIp);
	m_tCfg.dwConnectIpB = dwMpcBIp;
	
	// ���������, �ȶϿ�
	if( OspIsValidTcpNode( g_cTvWallApp.m_dwMcuNodeB ) )
	{
		OspDisconnectTcpNode( g_cTvWallApp.m_dwMcuNodeB );
	}

	g_cTvWallApp.FreeStatusDataB();
    if(0 != m_tCfg.dwConnectIpB && !g_cTvWallApp.m_bEmbedB )// ����Mcu
    {
        SetTimer(EV_TVWALL_CONNECTB, TV_CONNETC_TIMEOUT );
    }
	else
	{
		SetTimer(EV_TVWALL_REGISTERB, TV_REGISTER_TIMEOUT);
	}
}
/*=============================================================================
  �� �� ���� StopAllChannels
  ��    �ܣ� ֹͣ����ͨ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CTWInst::StopAllChannels(void)
{
	for(u8 byChlLp = 0; byChlLp < m_byTvNum; byChlLp++)
    {
        Stop(byChlLp); // ֹͣͨ��
        m_tTWStatus.atVideoMt[byChlLp].SetNull();
    }
	return;
}

/*=============================================================================
  �� �� ���� ProcGetMsStatusMsg
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CTWInst::ProcGetMsStatusMsg(CMessage* const pcMsg)
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	if( MCU_EQP_GETMSSTATUS_ACK == pcMsg->event ) // Mcu����Ӧ
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer(EV_TVWALL_GETMSSTATUS_TIMER);
        tvlog("[ProcGetMsStatusMsg]. receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                  ptMsStatus->IsMsSwitchOK());

        if(ptMsStatus->IsMsSwitchOK()) // �����ɹ�
        {
            bSwitchOk = TRUE;
        }
	}

	// ����ʧ�ܻ��߳�ʱ
	if( !bSwitchOk )
	{
		//20110914 zjl ��������ʧ�� ����״̬  �ٶ��� ������
		tvlog("[ProcGetMsStatusMsg] StopAllChannels!\n");	
		StopAllChannels();
        NEXTSTATE(u32(eIDLE)); // ʵ���������״̬		
        g_cTvWallApp.m_byRegAckNum = 0;
		g_cTvWallApp.m_dwMpcSSrc = 0; 

		if (OspIsValidTcpNode(g_cTvWallApp.m_dwMcuNode))
		{
			tvlog("[ProcGetMsStatusMsg] OspDisconnectTcpNode A!\n");	
			OspDisconnectTcpNode(g_cTvWallApp.m_dwMcuNode);
		}
		if (OspIsValidTcpNode(g_cTvWallApp.m_dwMcuNodeB))
		{
			tvlog("[ProcGetMsStatusMsg] OspDisconnectTcpNode B!\n");	
			OspDisconnectTcpNode(g_cTvWallApp.m_dwMcuNodeB);
		}

		if( INVALID_NODE == g_cTvWallApp.m_dwMcuNode)// �п��ܲ�����������Connect�������
		{
			tvlog("[ProcGetMsStatusMsg] EV_TVWALL_CONNECT!\n");	
			SetTimer(EV_TVWALL_CONNECT, TV_CONNETC_TIMEOUT);
		}
		if( INVALID_NODE == g_cTvWallApp.m_dwMcuNodeB)
		{
			tvlog("[ProcGetMsStatusMsg] EV_TVWALL_CONNECTB!\n");	
			SetTimer(EV_TVWALL_CONNECTB, TV_CONNETC_TIMEOUT);
		}

        
	}
	return;
}
/*====================================================================
    ������      ��SetEncryptParam
    ����        ��������Ƶ���ܲ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/22    3.6         Li Bo         ����
====================================================================*/
void CTWInst::SetEncryptParam(u8 byChnIdx, TMediaEncrypt*  ptMediaEncryptVideo,
                              TDoublePayload* ptDoublePayloadVideo)
{
    u16 wRet;
    u8  abyKeyBuf[MAXLEN_KEY];
	memset( abyKeyBuf, 0, sizeof(abyKeyBuf) );
	s32 nKeySize;
    u8  byDecryptMode;

    //����-��Ƶ
    byDecryptMode = ptMediaEncryptVideo->GetEncryptMode();
    if (byDecryptMode == CONF_ENCRYPTMODE_NONE)
    {
        // zbq [10/13/2007] FEC֧��
        u8 byPayLoadValue;
        if (ptDoublePayloadVideo->GetActivePayload() == MEDIA_TYPE_FEC)
        {
            byPayLoadValue = MEDIA_TYPE_FEC;
        }
        else if (ptDoublePayloadVideo->GetRealPayLoad() == MEDIA_TYPE_H264)
        {
            byPayLoadValue = MEDIA_TYPE_H264;
        }
        else if (ptDoublePayloadVideo->GetRealPayLoad() == MEDIA_TYPE_H263PLUS)
        {
            byPayLoadValue = MEDIA_TYPE_H263PLUS;
        }
        else
        {
            byPayLoadValue = 0;
        }

        wRet = m_acDecoder[byChnIdx].SetVideoActivePT(byPayLoadValue, byPayLoadValue);
        if (CODEC_NO_ERROR != wRet)
	    {
		    OspPrintf(TRUE, FALSE, "[TvWall] Grp.%d call \"SetVideoActivePT\" failed.\n", GetInsID());
		    return;
	    }

		wRet = m_acDecoder[byChnIdx].SetVidDecryptKey((s8*)NULL, (u16)0, byDecryptMode);
		if (CODEC_NO_ERROR != wRet)
		{
			OspPrintf(TRUE, FALSE, "[TvWall] SetVidDecryptKey failed err=%d!\n", wRet);
			return;
		}
    }
	else if (byDecryptMode == CONF_ENCRYPTMODE_DES || byDecryptMode == CONF_ENCRYPTMODE_AES)
    {
        tvlog("[TvWall]����-��Ƶ\n");

        //�ϲ����²�ı����ģʽ��һ��
        if (byDecryptMode == CONF_ENCRYPTMODE_DES)
        {
            byDecryptMode = DES_ENCRYPT_MODE;
        }
        else if (byDecryptMode == CONF_ENCRYPTMODE_AES)
        {
            byDecryptMode = AES_ENCRYPT_MODE;
        }

        // zbq [10/13/2007] FEC֧��
        u8 byRealPayload = ptDoublePayloadVideo->GetRealPayLoad();
        u8 byActivePayload = ptDoublePayloadVideo->GetActivePayload();
        if ( MEDIA_TYPE_FEC == byActivePayload )
        {
            byRealPayload = MEDIA_TYPE_FEC;
        }

        wRet = m_acDecoder[byChnIdx].SetVideoActivePT(byActivePayload,
                                                      byRealPayload );
        if (CODEC_NO_ERROR != wRet)
	    {
		    OspPrintf(TRUE, FALSE, "[TvWall] Grp.%d call \"SetVideoActivePT\" failed.\n", GetInsID());
		    return;
	    }

        ptMediaEncryptVideo->GetEncryptKey(abyKeyBuf, &nKeySize);
	
		wRet = m_acDecoder[byChnIdx].SetVidDecryptKey((s8*)abyKeyBuf, (u16)nKeySize, byDecryptMode);
		if (CODEC_NO_ERROR != wRet)
		{
			OspPrintf(TRUE, FALSE, "[TvWall] SetVidDecryptKey failed err=%d!\n", wRet);
			return;
		}
    }
}

/*=============================================================================
  �� �� ���� SetAudEnctypt
  ��    �ܣ� ������Ƶ���ܲ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChnIdx
             TMediaEncrypt*  ptMediaEncryptVideo
             TDoublePayload* ptDoublePayloadVideo
  �� �� ֵ�� BOOL 
=============================================================================*/
BOOL32 CTWInst::SetAudEnctypt(u8 byChnIdx, TMediaEncrypt*  ptMediaEncryptVideo, TDoublePayload* ptDoublePayloadVideo)
{
    if(NULL == ptMediaEncryptVideo || NULL == ptDoublePayloadVideo)
    {
        OspPrintf(TRUE, FALSE, "[TvWall] The pointer cannot be Null (SetAudEnctypt)\n");
        return FALSE;
    }
    
    u16 wRet;
    u8  abyKeyBuf[MAXLEN_KEY];
	s32 wKeySize;
    u8  byDecryptMode;

    //����-��Ƶ
    byDecryptMode = ptMediaEncryptVideo->GetEncryptMode();
    
    OspPrintf(TRUE, FALSE, "[TvWall] Audio PayLoad: EncryptMode: %d, ActivePayload: %d, RealPayLoad: %d\n", 
                                                   byDecryptMode, ptDoublePayloadVideo->GetActivePayload(),
                                                   ptDoublePayloadVideo->GetRealPayLoad());
	// ���ò�����Ϊ���ܺͲ�����
    if (byDecryptMode == CONF_ENCRYPTMODE_NONE)
    {
        // zbq [10/13/2007] FEC֧��
        u8 byPayLoadValue = 0;

        if (ptDoublePayloadVideo->GetActivePayload() == MEDIA_TYPE_FEC)
        {
            byPayLoadValue = MEDIA_TYPE_FEC;
        }
        
        wRet = m_acDecoder[byChnIdx].SetAudioActivePT(byPayLoadValue, byPayLoadValue);
        if (CODEC_NO_ERROR != wRet)
	    {
		    OspPrintf(TRUE, FALSE, "[TvWall] Grp.%d call \"SetVideoActivePT\" failed.\n", GetInsID());
		    return FALSE;
	    }

		wRet = m_acDecoder[byChnIdx].SetAudDecryptKey((s8*)NULL, (u16)0, byDecryptMode);
		if (CODEC_NO_ERROR != wRet)
		{
			OspPrintf(TRUE, FALSE, "[TvWall] SetVidDecryptKey failed err=%d!\n", wRet);
			return FALSE;
		}
    }
	else if (byDecryptMode == CONF_ENCRYPTMODE_DES || byDecryptMode == CONF_ENCRYPTMODE_AES)
    {
        tvlog("[TvWall]����-��Ƶ\n");

        //�ϲ����²�ı����ģʽ��һ��
        if (byDecryptMode == CONF_ENCRYPTMODE_DES)
        {
            byDecryptMode = DES_ENCRYPT_MODE;
        }
        else if (byDecryptMode == CONF_ENCRYPTMODE_AES)
        {
            byDecryptMode = AES_ENCRYPT_MODE;
        }
        
        // zbq [10/13/2007] FEC֧��
        u8 byRealPayload = ptDoublePayloadVideo->GetRealPayLoad();
        u8 byActivePayload = ptDoublePayloadVideo->GetActivePayload();
        if ( MEDIA_TYPE_FEC == byActivePayload )
        {
            byRealPayload = MEDIA_TYPE_FEC;
        }

        wRet = m_acDecoder[byChnIdx].SetAudioActivePT(byActivePayload,
                                                      byRealPayload);
        if (CODEC_NO_ERROR != wRet)
	    {
		    OspPrintf(TRUE, FALSE, "[TvWall]  Grp.%d call \"SetVideoActivePT\" failed.\n", GetInsID());
		    return FALSE;
	    }

        ptMediaEncryptVideo->GetEncryptKey(abyKeyBuf, &wKeySize);
	
		wRet = m_acDecoder[byChnIdx].SetAudDecryptKey((s8*)abyKeyBuf, (u16)wKeySize, byDecryptMode);
		if (CODEC_NO_ERROR != wRet)
		{
			OspPrintf(TRUE, FALSE, "[TvWall] SetVidDecryptKey failed err=%d!\n", wRet);
			return FALSE;
		}
    }
    return TRUE;
}
/*====================================================================
    ������      ��ProcStartPlayMsg
    ����        ����ʼ������Ϣ�Ĵ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
    05/01/12    3.6         Li Bo         �����޸�
====================================================================*/
void CTWInst::ProcStartPlayMsg(CMessage* const pMsg)
{
    PCHECK(pMsg);
    CServMsg cServMsg(pMsg->content, pMsg->length);

    // ��ǰ������ͨ����
    u8 byChnIdx = cServMsg.GetChnIndex();

    if (byChnIdx >= m_byTvNum)  // �ж�ͨ�����Ƿ�Ϸ�
    {
        cServMsg.SetErrorCode(ERR_TW_INVALID_CHANNL);
        SendMsgToMcu(TVWALL_MCU_START_PLAY_NACK, cServMsg);
        log(LOGLVL_EXCEPTION, "[TW] Mcu Want %d Paly Nack it!\n", byChnIdx);
        return;
    }

    TMt tSrcMt;
    TTvWallStartPlay tTvWallStartPlay;
    TMediaEncrypt    tMediaEncrypt;
    TDoublePayload   tDoublePayload, tAudPayLoad;

    tTvWallStartPlay = *(TTvWallStartPlay*)cServMsg.GetMsgBody();
    tDoublePayload   = *(TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(TTvWallStartPlay));
    if(TW_OUTPUTMODE_BOTH == m_byTvModel)
    {
        tAudPayLoad = *(TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(TTvWallStartPlay) + sizeof(TDoublePayload));
    }
	// MCUǰ�����, zgc, 2007-09-27
	TCapSupportEx tCapSupportEx = *(TCapSupportEx*)(cServMsg.GetMsgBody() + sizeof(TTvWallStartPlay) + sizeof(TDoublePayload)*2 );
	if( g_nlog == 1 )
	{
		tCapSupportEx.Print();
	}
	
	TTransportAddr tAudRemoteAddr;
	tAudRemoteAddr.SetNull();
	if (cServMsg.GetMsgBodyLen() > (sizeof(TTvWallStartPlay) + sizeof(TDoublePayload) * 2 + sizeof(TCapSupportEx)) &&
		tTvWallStartPlay.IsNeedByPrs())
	{
		tAudRemoteAddr = *(TTransportAddr*)(cServMsg.GetMsgBody() + sizeof(TTvWallStartPlay) + 
											sizeof(TDoublePayload) * 2 + sizeof(TCapSupportEx));
		tvlog("[ProcStartPlayMsg] AudRemoteAddr<%x, %d>!\n", tAudRemoteAddr.GetIpAddr(), tAudRemoteAddr.GetPort());
	}

    tSrcMt = tTvWallStartPlay.GetMt();

    tMediaEncrypt = tTvWallStartPlay.GetVideoEncrypt();

    tvlog("********************************************************\n");
    tvlog("tSrcMt.GetMcuId() = %d,tSrcMt.GetEqpId() = %d\n",
                        tSrcMt.GetMcuId(),
						tSrcMt.GetEqpId());
    tvlog("GetRealPayLoad() = %d, GetActivePayload() = %d\n",
                        tDoublePayload.GetRealPayLoad(), 
                        tDoublePayload.GetActivePayload());
    s32 nLen;
    u8  achKey[20];
    memset((s8*)achKey, '\0', sizeof(achKey));
    tMediaEncrypt.GetEncryptKey(achKey, &nLen);
    tvlog("GetEncryptMode() = %d, key = %s, len = %d\n",
                        tMediaEncrypt.GetEncryptMode(), achKey, nLen);
    tvlog("********************************************************\n");

    // ���ü��ܲ���
    SetEncryptParam(byChnIdx, &tMediaEncrypt, &tDoublePayload);
    if(TW_OUTPUTMODE_BOTH == m_byTvModel)
    {
        SetAudEnctypt(byChnIdx, &tMediaEncrypt, &tAudPayLoad); // ��Ƶ
    }
    if (Play(byChnIdx))
    {
        m_tTWStatus.atVideoMt[byChnIdx].SetMt(tSrcMt);
        SendMsgToMcu(TVWALL_MCU_START_PLAY_ACK, cServMsg);
        SendNotify();
    }
    else
    {
        cServMsg.SetErrorCode(ERR_TW_STARTPLAY_FAIL);
        SendMsgToMcu(TVWALL_MCU_START_PLAY_NACK, cServMsg);
    }

    u16 wRet;
    u32 dwRtcpBackIp;
    u16 wRtcpBackPort;
    TLocalNetParam tNetParm;
    
    memset(&tNetParm, 0, sizeof(tNetParm));
    tTvWallStartPlay.GetVidRtcpBackAddr(dwRtcpBackIp, wRtcpBackPort);

	//��Ƶ��Ĭ�ϵ�
    if (tTvWallStartPlay.IsNeedByPrs())
    {
        tNetParm.m_dwRtcpBackAddr = dwRtcpBackIp;
        tNetParm.m_wRtcpBackPort  = wRtcpBackPort;
    }
    else
    {
        tNetParm.m_dwRtcpBackAddr = 0;
        tNetParm.m_wRtcpBackPort  = 0;
    }
    tNetParm.m_tLocalNet.m_dwRTPAddr  = m_tCfg.dwLocalIP;
    tNetParm.m_tLocalNet.m_wRTPPort   = m_tCfg.wRcvStartPort + byChnIdx*PORTSPAN;
    tNetParm.m_tLocalNet.m_dwRTCPAddr = m_tCfg.dwLocalIP;
    tNetParm.m_tLocalNet.m_wRTCPPort  = m_tCfg.wRcvStartPort + 1 + byChnIdx*PORTSPAN;
    wRet = m_acDecoder[byChnIdx].SetVideoNetRcvParam(&tNetParm);
    if (CODEC_NO_ERROR != wRet)
    {
        log(LOGLVL_EXCEPTION, "[TW] Chn.%d SetVideoNetRcvParam failed err=%d!\n", byChnIdx, wRet);
        return;
    }

    //��Ƶ + ��Ƶ
    if(TW_OUTPUTMODE_BOTH == m_byTvModel)
    {
        memset(&tNetParm, 0, sizeof(tNetParm));
		//tTvWallStartPlay.GetAudRtcpBackAddr(dwRtcpBackIp, wRtcpBackPort);

        if (tTvWallStartPlay.IsNeedByPrs())
        {
            tNetParm.m_dwRtcpBackAddr = tAudRemoteAddr.GetIpAddr();
            tNetParm.m_wRtcpBackPort  = tAudRemoteAddr.GetPort();
        }
        else
        {
            tNetParm.m_dwRtcpBackAddr = 0;
            tNetParm.m_wRtcpBackPort  = 0;
        }
        tNetParm.m_tLocalNet.m_dwRTPAddr  = m_tCfg.dwLocalIP;
        tNetParm.m_tLocalNet.m_wRTPPort   = m_tCfg.wRcvStartPort + 2 + byChnIdx*PORTSPAN;
        tNetParm.m_tLocalNet.m_dwRTCPAddr = m_tCfg.dwLocalIP;
        tNetParm.m_tLocalNet.m_wRTCPPort  = m_tCfg.wRcvStartPort + 3 + byChnIdx*PORTSPAN;
        wRet = m_acDecoder[byChnIdx].SetAudioNetRcvParam(&tNetParm);
        if (CODEC_NO_ERROR != wRet)
        {
            log(LOGLVL_EXCEPTION, "[TW] Chn.%d SetVideoNetRcvParam failed err=%d!\n", byChnIdx, wRet);
            return;
        }
    }
    
	//�趪���ش�����
	TNetRSParam tNetRSParam;
	if (tTvWallStartPlay.IsNeedByPrs())
	{		
		tNetRSParam.m_wFirstTimeSpan  = m_tPrsTimeSpan.m_wFirstTimeSpan;
		tNetRSParam.m_wSecondTimeSpan = m_tPrsTimeSpan.m_wSecondTimeSpan;
		tNetRSParam.m_wThirdTimeSpan  = m_tPrsTimeSpan.m_wThirdTimeSpan;
		tNetRSParam.m_wRejectTimeSpan = m_tPrsTimeSpan.m_wRejectTimeSpan;
		m_acDecoder[byChnIdx].SetNetFeedbackVideoParam(tNetRSParam, TRUE);
	}
	else
	{
		tNetRSParam.m_wFirstTimeSpan  = m_tPrsTimeSpan.m_wFirstTimeSpan;
		tNetRSParam.m_wSecondTimeSpan = m_tPrsTimeSpan.m_wSecondTimeSpan;
		tNetRSParam.m_wThirdTimeSpan  = m_tPrsTimeSpan.m_wThirdTimeSpan;
		tNetRSParam.m_wRejectTimeSpan = m_tPrsTimeSpan.m_wRejectTimeSpan;
		m_acDecoder[byChnIdx].SetNetFeedbackVideoParam(tNetRSParam, FALSE);
	}
    return;
}

/*====================================================================
    ������      ��ProcStopPlayMsg
    ����        ��ֹͣ������Ϣ�Ĵ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
====================================================================*/
void CTWInst::ProcStopPlayMsg(CMessage* const pMsg)
{
    PCHECK(pMsg);
    CServMsg cServMsg(pMsg->content, pMsg->length);
	
	// ��ǰ������ͨ����
	u8 byChnIdx = cServMsg.GetChnIndex(); 

	if (byChnIdx >= m_byTvNum)  // �ж�ͨ�����Ƿ�Ϸ�
	{	
		cServMsg.SetErrorCode(ERR_TW_INVALID_CHANNL);
		SendMsgToMcu(TVWALL_MCU_STOP_PLAY_NACK, cServMsg);
		OspPrintf(TRUE, FALSE, "[TW] Mcu Want %d Stop Nack it!\n", byChnIdx);
		return;
	}


	if (Stop(byChnIdx))
	{
        OspPrintf(TRUE, FALSE, "[TvWall] Stop channel: %d success\n", byChnIdx); 
		 m_tTWStatus.atVideoMt[byChnIdx].SetNull();
		 SendMsgToMcu(TVWALL_MCU_STOP_PLAY_ACK, cServMsg);
		 SendNotify();
	}
	else
	{
		cServMsg.SetErrorCode(ERR_TW_STOPPLAY_FAIL);
		SendMsgToMcu(TVWALL_MCU_STOP_PLAY_NACK, cServMsg);
		OspPrintf(TRUE, FALSE, "[TW]Chn.%d Call Stop failed!\n", byChnIdx);
		return;
	}

	TNetRSParam tNetRSParam;
	tNetRSParam.m_wFirstTimeSpan  = m_tPrsTimeSpan.m_wFirstTimeSpan;
	tNetRSParam.m_wSecondTimeSpan = m_tPrsTimeSpan.m_wSecondTimeSpan;
	tNetRSParam.m_wThirdTimeSpan  = m_tPrsTimeSpan.m_wThirdTimeSpan;
	tNetRSParam.m_wRejectTimeSpan = m_tPrsTimeSpan.m_wRejectTimeSpan;
	m_acDecoder[byChnIdx].SetNetFeedbackVideoParam(tNetRSParam, FALSE);
    return;
}


/*====================================================================
    ������      ��Init
    ����        ����ʼ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����ptCfg �� ���ò���
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
====================================================================*/
BOOL CTWInst::Init(CMessage* const pMsg)
{

    tvlog("[tv] init the Decoder\n");
    if( NULL == pMsg)
    {
        OspPrintf(TRUE, FALSE, "The pointer cannot be Null\n");
        return FALSE;
    }

    m_byTvModel = BrdGetTvwallModel(); // ����ǽ�Ƿ�֧����Ƶ
    
    if(TW_OUTPUTMODE_BOTH == m_byTvModel)
    {
        m_tTWStatus.byOutputMode = TW_OUTPUTMODE_BOTH;
    }
    else
    {
        m_tTWStatus.byOutputMode = TW_OUTPUTMODE_VIDEO;
    }

    tvlog("[TvWall] The Tvwall model: %d\n (0: no, 1: Audio, 2: Video, 3: Both)\n", m_byTvModel);

    s32 nChnIdx = 0;

    memcpy(&m_tCfg, pMsg->content, sizeof(m_tCfg));
    m_byTvNum = (u8)m_tCfg.wMAPCount;

    for (nChnIdx=0; nChnIdx<MAXNUM_TVWALL_CHNNL; nChnIdx++)
    {
        m_tTWStatus.atVideoMt[nChnIdx].SetNull();
    }

    if (!InitDecoder()) // ��ʼ��������
    {
        OspPrintf(TRUE, FALSE, "[TvWall] Fail to inital decorder\n");
		printf("[TvWall] Fail to inital decorder\n");
		return FALSE;
    }

    if (m_tCfg.dwLocalIP == m_tCfg.dwConnectIP)// �ж��Ƿ���Ƕ��( ������ǰ�汾���Ժ���ȥ�� )
    {
        g_cTvWallApp.m_dwMcuNode = 0;
        g_cTvWallApp.m_bEmbed = TRUE;
        SetTimer(EV_TVWALL_REGISTER, TV_REGISTER_TIMEOUT);
    }

    if(m_tCfg.dwLocalIP == m_tCfg.dwConnectIpB) // �ж��Ƿ���Ƕ
    {
        g_cTvWallApp.m_bEmbedB = TRUE;
        g_cTvWallApp.m_dwMcuNodeB = 0;
        SetTimer(EV_TVWALL_REGISTERB, TV_REGISTER_TIMEOUT);
    }

#ifndef WIN32 
	if(0 != m_tCfg.dwConnectIP && FALSE == g_cTvWallApp.m_bEmbed ) // ����Mcu
#else
	if( 0 != m_tCfg.dwConnectIP )
#endif    
    {
        SetTimer(EV_TVWALL_CONNECT, TV_CONNETC_TIMEOUT );
    }
	else
	{
		printf("m_tCfg.dwConnectIPA = 0!\n");
	}

#ifndef WIN32
    if(0 != m_tCfg.dwConnectIpB && FALSE == g_cTvWallApp.m_bEmbedB )// ����Mcu
#else
	if( 0 != m_tCfg.dwConnectIpB )
#endif
    {
        SetTimer(EV_TVWALL_CONNECTB, TV_CONNETC_TIMEOUT );
    }
	else
	{
		printf("m_tCfg.dwConnectIPB = 0!\n");
	}

    return TRUE;
}

/*====================================================================
    ������      ��InitDecoder
    ����        ����ʼ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
====================================================================*/
BOOL CTWInst::InitDecoder()
{
    u16 wRet;
    THardCodecInitParm tParm;

    tvlog("[TW] Now Start Init Decodesr, the Max Channel: %d\n", m_byTvNum);

	u32 dwBlueTimeSpan = g_cTvWallApp.GetCfgWait2BlueTimeSpan();

    //nChnIdx��MAPIDһһ��Ӧ
    for(u32 nChnIdx = 0; nChnIdx < m_byTvNum; nChnIdx++)
    {
        memset(&tParm, 0, sizeof(tParm));

        tParm.dwCoreSpd = m_tCfg.m_atMap[nChnIdx].dwCoreSpd;
        tParm.dwMemSize = m_tCfg.m_atMap[nChnIdx].dwMemSize;
        tParm.dwMemSpd  = m_tCfg.m_atMap[nChnIdx].dwMemSpd;

        tParm.dwMaxVidDecNum = 1;
        tParm.dwMaxVidEncNum = 0;

        if(TW_OUTPUTMODE_BOTH == m_byTvModel)
        {
            tParm.dwMaxAudDecNum = 1;   // ��Ƶ����
            tParm.dwMaxAudEncNum = 1;
        }
        else
        {
            tParm.dwMaxAudDecNum = 0;   // ��Ƶ����
            tParm.dwMaxAudEncNum = 0;
        }


        tvlog("Map param: Channl: %d, Map: %d Spd: %d MemSize: %d MemSpd: %d, Port: %d\n",
                            nChnIdx, m_tCfg.m_atMap[nChnIdx].byMapId, tParm.dwCoreSpd, 
                             tParm.dwMemSize, tParm.dwMemSpd, m_tCfg.m_atMap[nChnIdx].dwPort);

        wRet = HardCodecInit(m_tCfg.m_atMap[nChnIdx].byMapId, &tParm, TRUE); // Ӳ����ʼ��
        if (wRet != CODEC_NO_ERROR)
        {
            OspPrintf(TRUE, FALSE, "[TW] Init Codec map.%d fialed (%d)!\n", nChnIdx, wRet);
			printf("[TW] Init Codec map.%d fialed (%d)!\n", nChnIdx, wRet);

            return FALSE;
        }

        TVideoDecoder tVidDec;
        memset(&tVidDec, 0, sizeof(tVidDec));
        tVidDec.m_dwCpuId               = m_tCfg.m_atMap[nChnIdx].byMapId;
        tVidDec.m_nVideoPlayBufNum      = 12;
        tVidDec.m_nVideoStartPlayBufNum = 4;

        tVidDec.m_dwCoreSpd = m_tCfg.m_atMap[nChnIdx].dwCoreSpd;;
        tVidDec.m_dwMemSpd  = m_tCfg.m_atMap[nChnIdx].dwMemSpd;
        tVidDec.m_dwMemSize = (64<<20); // m_tCfg.m_atMap[nChnIdx].dwMemSize;

        tvlog("Video param: Map: %d Spd: %d MemSize: %d MemSpd: %d, Port: %d\n",
                    tVidDec.m_dwCpuId, tVidDec.m_dwCoreSpd, tVidDec.m_dwMemSize, 
                    tVidDec.m_dwMemSpd, tVidDec.m_byPort);
        
        TAudioDecoder tAudDec;
        memset(&tAudDec, 0, sizeof(TAudioDecoder));
        tAudDec.m_dwCpuId = m_tCfg.m_atMap[nChnIdx].byMapId;
        tAudDec.m_nAudioPlayBufNum = 8; // 8
        tAudDec.m_nAudioStartPlayBufNum = 1; // 1

        tAudDec.m_dwMemSize = (64<<20); // m_tCfg.m_atMap[nChnIdx].dwMemSize;
        tAudDec.m_dwCoreSpd = m_tCfg.m_atMap[nChnIdx].dwCoreSpd;
        tAudDec.m_dwMemSpd = m_tCfg.m_atMap[nChnIdx].dwMemSpd;

        tvlog("Audio param: Map: %d Spd: %d MemSize: %d MemSpd: %d, Port: %d\n",
                    tAudDec.m_dwCpuId, tAudDec.m_dwCoreSpd, tAudDec.m_dwMemSize, 
                                                        tAudDec.m_dwMemSpd, tAudDec.m_byPort);

        if(TW_OUTPUTMODE_BOTH == m_byTvModel)
        {
            wRet = m_acDecoder[nChnIdx].CreateDecoder(&tVidDec, &tAudDec);
        }
        else
        {
            wRet = m_acDecoder[nChnIdx].CreateDecoder(&tVidDec, NULL);
        }
        if (CODEC_NO_ERROR != wRet)
        {
            OspPrintf(TRUE, FALSE, "[TW] Ch.%d CreateDecoder failed err=%d!\n", nChnIdx, wRet);
			printf("[TW] Ch.%d CreateDecoder failed err=%d!\n", nChnIdx, wRet);

            return FALSE;
        }

        TLocalNetParam tNetParm;
        memset(&tNetParm, 0, sizeof(tNetParm));
        tNetParm.m_dwRtcpBackAddr = 0;
        tNetParm.m_wRtcpBackPort  = 0;
        tNetParm.m_tLocalNet.m_dwRTPAddr  = m_tCfg.dwLocalIP;
        tNetParm.m_tLocalNet.m_wRTPPort   = m_tCfg.wRcvStartPort + (u16)nChnIdx*PORTSPAN;
        tNetParm.m_tLocalNet.m_dwRTCPAddr = m_tCfg.dwLocalIP;
        tNetParm.m_tLocalNet.m_wRTCPPort  = m_tCfg.wRcvStartPort + (u16)nChnIdx*PORTSPAN + 1;

        tvlog("Set Channel: %d Video Rcv Param Ip: %0x, Port:%d\n",
            nChnIdx, m_tCfg.dwLocalIP, m_tCfg.wRcvStartPort + nChnIdx*PORTSPAN);

        wRet = m_acDecoder[nChnIdx].SetVideoNetRcvParam(&tNetParm);//��Ƶ���ܲ���
        if (CODEC_NO_ERROR != wRet)
        {
            OspPrintf(TRUE, FALSE, "[TW] Chn.%d SetVideoNetRcvParam failed err: %d!\n", nChnIdx, wRet);
			printf("[TW] Chn.%d SetVideoNetRcvParam failed err: %d!\n", nChnIdx, wRet);
            return FALSE;
        }

        wRet = m_acDecoder[nChnIdx].StartRcvVideo();
        if (CODEC_NO_ERROR != wRet)
        {
            OspPrintf(TRUE, FALSE, "[TW] Channel.%d StartRcvVideo failed err: %d!\n", nChnIdx, wRet);
			printf("[TW] Channel.%d StartRcvVideo failed err: %d!\n", nChnIdx, wRet);
            return FALSE;
        }

        tvlog("[TW] Init Video Decoder: %d Success!\n", nChnIdx);

        m_acDecoder[nChnIdx].SetDisplayMode(PIP_REMOTE_ONLY);// ��ʾģʽ
        if(TW_OUTPUTMODE_BOTH == m_byTvModel)
        {
            // ������Ƶ�˿ڵķ���
            TLocalNetParam tAudNetParam;
            memset(&tAudNetParam, 0, sizeof(TLocalNetParam));
            tAudNetParam.m_dwRtcpBackAddr = 0;
            tAudNetParam.m_wRtcpBackPort  = 0;
            tAudNetParam.m_tLocalNet.m_dwRTPAddr  = m_tCfg.dwLocalIP;  
            tAudNetParam.m_tLocalNet.m_wRTPPort   = (m_tCfg.wRcvStartPort+(u16)nChnIdx*PORTSPAN) + 2; // ��Ƶ�˿�
            tAudNetParam.m_tLocalNet.m_dwRTCPAddr = m_tCfg.dwLocalIP; 
            tAudNetParam.m_tLocalNet.m_wRTCPPort  = (m_tCfg.wRcvStartPort+(u16)nChnIdx*PORTSPAN) + 3; 
    
            tvlog("Set Channel: %d Audio Rcv Param Ip: %0x, Port:%d\n",
                        nChnIdx, m_tCfg.dwLocalIP, m_tCfg.wRcvStartPort+nChnIdx*PORTSPAN + 2 );
    
            wRet = m_acDecoder[nChnIdx].SetAudioNetRcvParam(&tAudNetParam);   // ��Ƶ���ܲ���
            if (CODEC_NO_ERROR != wRet)
            {
                OspPrintf(TRUE, FALSE, "[TW] Chn.%d SetAudioNetRcvParam failed err: %d!\n", nChnIdx, wRet);
				printf("[TW] Chn.%d SetAudioNetRcvParam failed err: %d!\n", nChnIdx, wRet);
                return FALSE;
            }

            wRet = m_acDecoder[nChnIdx].StartRcvAudio();    // ��ʼ������Ƶ
            if (CODEC_NO_ERROR != wRet)
            {
                OspPrintf(TRUE, FALSE, "[TW] Chn.%d StartRcvVideo failed err=%d!\n", nChnIdx, wRet);
				printf("[TW] Chn.%d StartRcvVideo failed err=%d!\n", nChnIdx, wRet);
                return FALSE;
            }
        }
		::VidSetBlueWaitTime(nChnIdx, dwBlueTimeSpan);
    }
    return TRUE;
}

/*====================================================================
    ������      ��Play
    ����        ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����nChnnl ͨ����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
====================================================================*/
BOOL CTWInst::Play(s32 nChnnl)
{

    tvlog("[TvWall] Process to Start Channel: %d\n", nChnnl);

    BOOL bRet = TRUE;
    if(nChnnl >= m_byTvNum) // ͨ����ʼ�Ŵ�0��ʼ
    {
        OspPrintf(TRUE, FALSE, "[TW] Chnnal Indx = %d the Max is %d!\n", m_byTvNum);
        return FALSE;
    }

    u16 wRet = m_acDecoder[nChnnl].StartVideoDec(); // ��ʼ��Ƶ����
    if(CODEC_NO_ERROR != wRet)
    {
        OspPrintf(TRUE, FALSE, "[TW] StartVideoDec failed err: %d!\n", wRet);
        bRet = FALSE;
    }
    m_acDecoder[nChnnl].SetDisplayMode(PIP_REMOTE_ONLY);
    if(TW_OUTPUTMODE_BOTH == m_byTvModel)
    {
        wRet = m_acDecoder[nChnnl].StartAudioDec(); // ��ʼ��Ƶ����
        if(CODEC_NO_ERROR != wRet)
        {
            OspPrintf(TRUE, FALSE, "[TW] StartAudioDec failed err: %d!\n", wRet);
            bRet = FALSE;
        }
    }
    return bRet;
}

/*====================================================================
    ������      ��Stop
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����nChnnl ͨ����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
====================================================================*/
BOOL CTWInst::Stop(s32 nChnnl)
{

    tvlog("[TvWall] Process to stop Channel: %d\n", nChnnl);

    BOOL bRet = TRUE;
    if (nChnnl >= m_byTvNum)
	{
		OspPrintf(TRUE, FALSE, "[TW] Chnnal Indx = %d the Max is %d!\n", m_byTvNum);
		return FALSE;
	}

	u16 wRet;
	wRet = m_acDecoder[nChnnl].StopVideoDec();// ֹͣ����
	if (CODEC_NO_ERROR != wRet)
	{
		OspPrintf(TRUE, FALSE, "[TW] StopVideoDec failed err: %d!\n", wRet);
		bRet = FALSE;
	}

    if(TW_OUTPUTMODE_BOTH == m_byTvModel)
    {

        wRet = m_acDecoder[nChnnl].StopAudioDec();//��Ƶ
	    if (CODEC_NO_ERROR != wRet)
	    {
		    OspPrintf(TRUE, FALSE, "[TW] StopAudioDec failed err: %d!\n", wRet);
		    bRet = FALSE;
	    }
    }
	return bRet;
}

/*=============================================================================
  �� �� ���� ProcSetQosInfo
  ��    �ܣ� ����Qosֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CTWInst::ProcSetQosInfo(CMessage* const pMsg)
{
    if(NULL == pMsg)
    {
        OspPrintf(TRUE, FALSE, "[Mixer] The Mix's Qos infomation is Null\n");
        return;
    }

    TMcuQosCfgInfo  tQosInfo;
    CServMsg cServMsg(pMsg->content, pMsg->length);
    tQosInfo = *(TMcuQosCfgInfo*)cServMsg.GetMsgBody();

    u8 byQosType = tQosInfo.GetQosType();
    u8 byAudioValue = tQosInfo.GetAudLevel();
    u8 byVideoValue = tQosInfo.GetVidLevel();
    u8 byDataValue = tQosInfo.GetDataLevel();
    u8 byIpPriorValue = tQosInfo.GetIpServiceType();
    if(QOSTYPE_IP_PRIORITY == byQosType)
    {
        byAudioValue = (byAudioValue << 5);
        byVideoValue = (byVideoValue << 5);
        byDataValue = (byDataValue << 5);
        ComplexQos(byAudioValue, byIpPriorValue);
        ComplexQos(byVideoValue, byIpPriorValue);
        ComplexQos(byDataValue, byIpPriorValue);
    }
    else
    {
        byAudioValue = (byAudioValue << 2);
        byVideoValue = (byVideoValue << 2);
        byDataValue = (byDataValue << 2);
    }

    /*s32 nRet = */SetMediaTOS((s32)byAudioValue, QOS_AUDIO);
    /*nRet = */SetMediaTOS((s32)byVideoValue, QOS_VIDEO);
    /*nRet = */SetMediaTOS((s32)byDataValue, QOS_DATA);

    return;
}

/*=============================================================================
  �� �� ���� ComplexQos
  ��    �ܣ� �󸴺�Qosֵ(�����㷨�μ������ĵ�)
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8& byValue
             u8 byPrior
  �� �� ֵ�� void 
=============================================================================*/
void CTWInst::ComplexQos(u8& byValue, u8 byPrior)
{
    u8 byLBit = ((byPrior & 0x08) >> 3);
    u8 byRBit = ((byPrior & 0x04) >> 1);
    u8 byTBit = ((byPrior & 0x02)<<1);
    u8 byDBit = ((byPrior & 0x01)<<3);
    
    byValue = byValue + ((byDBit + byTBit + byRBit + byLBit)*2);
    return;
}
/*====================================================================
    ������      ��ConfigShow
    ����        ��TvWall������ʾ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
    05/01/12    3.6         Li Bo         ��ǿ״̬��ʾ����
====================================================================*/
void CTWInst::ConfigShow()
{
    u16 wLoop;

    OspPrintf(TRUE, FALSE, "Local IP : %x\n", m_tCfg.dwLocalIP);
    OspPrintf(TRUE, FALSE, "MCU IP : %x\n", m_tCfg.dwConnectIP);
    OspPrintf(TRUE, FALSE, "MCU ID : %d\n", m_tCfg.wMcuId);
    OspPrintf(TRUE, FALSE, "MCU Port : %u\n", m_tCfg.wConnectPort);
    OspPrintf(TRUE, FALSE, "Eqp ID : %u\n", m_tCfg.byEqpId);
    OspPrintf(TRUE, FALSE, "Eqp Alias : %s\n", m_tCfg.achAlias);
    OspPrintf(TRUE, FALSE, "Rcv Start Port : %u\n", m_tCfg.wRcvStartPort);

    OspPrintf(TRUE ,FALSE ,"Map Nub : %d\n", m_byTvNum);
	OspPrintf(TRUE, FALSE, "Blue Screen Time(ms): %u\n", g_cTvWallApp.GetCfgWait2BlueTimeSpan() );
    for(wLoop = 0; wLoop < m_byTvNum; wLoop++)
    {
        OspPrintf(TRUE, FALSE, "Map.%d CoreSpeed=%d MemSize =%d MemSpeed=%d\n",
                                wLoop,
                                m_tCfg.m_atMap[wLoop].dwCoreSpd,  
                                m_tCfg.m_atMap[wLoop].dwMemSize, 
                                m_tCfg.m_atMap[wLoop].dwMemSpd);
    }

    OspPrintf(TRUE, FALSE, "Decoder Nub : %d\n", m_tCfg.wMAPCount);
    for(wLoop = 0; wLoop < m_tCfg.wMAPCount; wLoop++)
    {
        OspPrintf(TRUE, FALSE, "\nDecoder.%d \nVideo : MapId=%d MapPort=%d BufSize=%d\n",
                                wLoop,
                                m_tCfg.m_atMap[wLoop].byMapId,
                                m_tCfg.m_atMap[wLoop].dwPort,
                                m_tCfg.m_atMap[wLoop].dwMemSize);
    }
}

/*====================================================================
    ������      ��StatusShow
    ����        ��TvWall״̬��ʾ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/10    2.x         ������        ����
    05/01/12    3.6         Li Bo         ��ǿ״̬��ʾ����
====================================================================*/
void CTWInst::StatusShow()
{
	OspPrintf(TRUE, FALSE, "\n\t KDV TV Wall \n");
	OspPrintf(TRUE, FALSE, "Channel Num : %d \n\n", m_tTWStatus.byChnnlNum);
	for (s32 nLoop=0; nLoop<m_byTvNum; nLoop++)
	{
		if (m_tTWStatus.atVideoMt[nLoop].GetMcuId())
		{
			OspPrintf(TRUE, FALSE, "\t ------------------------------\n");
			OspPrintf(TRUE, FALSE, "\t Mcu.%d Mt.%d\n",m_tTWStatus.atVideoMt[nLoop].GetMcuId(),
                                    m_tTWStatus.atVideoMt[nLoop].GetMtId());
		}
		else
		{
			OspPrintf(TRUE, FALSE, "\t ------------------------------\n");
			OspPrintf(TRUE, FALSE, "\t IDLE Channel %d\n",nLoop);
		}
	}
	OspPrintf(TRUE, FALSE, "\n");
	return;
}

CTvWallCfg::CTvWallCfg()
{  
	m_dwMcuIId  = INVALID_INS;
    m_dwMcuNode = INVALID_NODE;
	m_dwMcuIIdB  = INVALID_INS;
    m_dwMcuNodeB = INVALID_NODE;
    m_bEmbedB = FALSE;
    m_bEmbed  = FALSE;
    m_byRegAckNum = 0;
	m_dwMpcSSrc = 0;
}

CTvWallCfg::~CTvWallCfg()
{
}

/*=============================================================================
  �� �� ���� FreeStatusDataA
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CTvWallCfg::FreeStatusDataA()
{
    m_dwMcuIId  = INVALID_INS;
    m_dwMcuNode = INVALID_NODE;
    return;
}

/*=============================================================================
  �� �� ���� FreeStatusDataB
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CTvWallCfg::FreeStatusDataB()
{
    m_dwMcuIIdB  = INVALID_INS;
    m_dwMcuNodeB = INVALID_NODE;
    return;
}
/*=============================================================================
  �� �� ���� GetCfgWait2BlueTimeSpan
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
u32 CTvWallCfg::GetCfgWait2BlueTimeSpan()
{
	/*lint -save -e569*/
    s32 nValue;
    if (!::GetRegKeyInt(CFG_FILE, SECTION_DEBUG, "Wait2BlueScrTimeSpan", 3000, &nValue))    
    {
        nValue = 3000;
    }
    if (nValue == 0)
	{
        nValue = 0xFF000000;
	}
	
    return (u32)nValue;
	/*lint -restore*/
}
