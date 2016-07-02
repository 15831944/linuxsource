/*****************************************************************************
   ģ����      : MpwLib�໭�渴�ϵ���ǽ
   �ļ���      : mpwinst.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ��Ϣ�࣬ʵ����Mcuҵ��Ľ���
   ����        : john
   �汾        : V4.0  Copyright(C) 2001-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/09/25  1.0         john        ����
   2005/03/21  4.0         �ű���      �����Ż�������֧��
******************************************************************************/
#include "mpwinst.h"
#include "evmpw.h"
#include "boardagent.h"
#include "mcuver.h"

CMpwApp g_cMpwApp;

s32 g_nMpwlog = 0;

//#define _TEST_   // ����ʱ�õ�

CMpwInst::CMpwInst()
{
    m_bDbVid = FALSE;
    m_dwLastFUPTick = 0;
    m_cConfId.SetNull();
    memset( &m_cHardMulPic, 0, sizeof(m_cHardMulPic) );
    memset( &m_tRtcpAddr, 0, sizeof(TNetAddress) );
	memset( &m_tParam, 0, sizeof(CKDVVMPParam) );
    memset( &m_tCapSupportEx, 0, sizeof(m_tCapSupportEx));
/*    for( s32 nIndex = 0; nIndex < 2; nIndex ++ )
    {
        memset( &m_tParam, 0, sizeof(CKDVVMPParam) );
    }
*/
    for (u8 byChnlNo = 0; byChnlNo < MAXNUM_VMP_MEMBER; byChnlNo++)
    {
        m_bAddVmpChannel[byChnlNo] = FALSE;
    }
    memset(&m_tSndAddr, 0, sizeof(m_tSndAddr));
}

CMpwInst::~CMpwInst()
{
    m_cHardMulPic.Quit();
}

void CMpwInst::InstanceEntry( CMessage *const pMsg )
{
    if(NULL == pMsg)
    {
        OspPrintf(TRUE, FALSE, "[Mpw] the pointer can not be Null\n");
        return;
    }

    CServMsg cServMsg(pMsg->content, pMsg->length);
    switch(pMsg->event)
    {
    //-------  �ڲ���ʱ�� --------
    // ����MCU.A
    case EV_MPW_CONNECT_TIMER:
        ProcConnectTimeOut(TRUE);
        break;

    // ����MCU.B
    case EV_MPW_CONNECT_TIMERB:
        ProcConnectTimeOut(FALSE);
        break;

    // ��MCU.Aע��
    case EV_MPW_REGISTER_TIMER:
        ProcRegisterTimeOut(TRUE);
        break;
        
    // ��MCU.Bע��
    case EV_MPW_REGISTER_TIMERB:
        ProcRegisterTimeOut(FALSE);
        break;

    // ����ؼ�֡
    case EV_MPW_NEEDIFRAME_TIMER:
        //MsgNeedIFrameProc();
        break;
	
    // ��������
    case OSP_DISCONNECT:
        ProcDisconnect(pMsg);
        break;

    // ��ӦMcu̽����Ϣ
    case MCU_EQP_ROUNDTRIP_REQ:
        SendMsgToMcu(EQP_MCU_ROUNDTRIP_ACK, &cServMsg);
        break;
        
    // ------ �ⲿ��Ϣ --------
    // ��ʼ��
    case EV_VMPTW_INIT:
        Init(pMsg);
        break;
        
    // ע��ɹ�
    case MCU_VMPTW_REGISTER_ACK:
        MsgRegAckProc(pMsg);
        break;

    //  ע��ʧ��   
    case MCU_VMPTW_REGISTER_NACK:
        MsgRegNackProc(pMsg);
        break;

    // ��ʼ����
    case MCU_VMPTW_STARTVIDMIX_REQ:
        MsgStartVidMixProc(pMsg);
        break;

    // ֹͣ����
    case MCU_VMPTW_STOPVIDMIX_REQ:
        MsgStopVidMixProc(pMsg);
        break;

    // ���ĸ��ϲ���
    case MCU_VMPTW_CHANGEVIDMIXPARAM_REQ:
        MsgChangeMixParamProc(pMsg);
        break;

    // ��ѯ����
    case MCU_VMPTW_GETVIDMIXPARAM_REQ:
        MsgGetMixParam(pMsg);
        break;

   // ���ļ��ܲ���
    case MCU_VMPTW_UPDATAVMPENCRYPTPARAM_REQ:
        UpdataEncryptParam(pMsg);
        break;
	// ȡ��������״̬
	case EV_MPW_GETMSSTATUS_TIMER:
	case MCU_EQP_GETMSSTATUS_ACK:
		ProcGetMsStatusRsp(pMsg);
		break;

    // ����Qosֵ
    case MCU_EQP_SETQOS_CMD:
        ProcSetQosInfo(pMsg);
        break;

    // ��ʾ״̬
    case EV_VMPTW_DISPLAYALL:
        MpwStatusShow();
        break; 
        
    default:
        mpwlog("Recv undefined message: %d<%s> in InstanceEntry !\n", pMsg->event, OspEventDesc(pMsg->event) );
        break;
    }
    return;
}

/*=============================================================================
  �� �� ���� Init
  ��    �ܣ� ��ʼ��������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::Init(CMessage* const pMsg)
{
    if (NULL == pMsg->content)
    {
        OspPrintf(TRUE, FALSE, "[Mpw]Init params cannot be NULL!\n");
        return;
    }

    u32 dwTotalMapNum = 0;
    TVmpCfg tMpwCfg;
    memset(&tMpwCfg, 0, sizeof(TVmpCfg));
    memcpy(&tMpwCfg, pMsg->content, sizeof(TVmpCfg));

#ifdef _TEST_
    OspPrintf(TRUE, FALSE, "[Mpw] The Video num: %d\n", tMpwCfg.byDbVid);
#endif

    dwTotalMapNum = tMpwCfg.wMAPCount;

    g_cMpwApp.m_tCfg.ReadConfigureFile(); // ��ȡ�����ļ�
    g_cMpwApp.m_tCfg.m_tMulPicParam.byType = GetFavirateStyle(dwTotalMapNum, g_cMpwApp.m_byChannelnum);

    g_cMpwApp.m_tCfg.m_tMulPicParam.dwTotalMapNum = dwTotalMapNum;
    g_cMpwApp.m_tCfg.m_tMulPicParam.dwCoreSpd     = tMpwCfg.m_atMap[0].dwCoreSpd;
    g_cMpwApp.m_tCfg.m_tMulPicParam.dwMemSpd      = tMpwCfg.m_atMap[0].dwMemSpd;
    g_cMpwApp.m_tCfg.m_tMulPicParam.dwMemSize     = tMpwCfg.m_atMap[0].dwMemSize;

    g_cMpwApp.m_tCfg.dwMcuIP        = tMpwCfg.dwConnectIP;
    g_cMpwApp.m_tCfg.wMcuPort       = tMpwCfg.wConnectPort;
    g_cMpwApp.m_tCfg.wMcuId         = tMpwCfg.wMcuId;

    g_cMpwApp.m_tCfg.byEqpType      = tMpwCfg.byEqpType;
    g_cMpwApp.m_tCfg.byEqpId        = tMpwCfg.byEqpId;
    g_cMpwApp.m_tCfg.dwLocalIp      = tMpwCfg.dwLocalIP;
    g_cMpwApp.m_tCfg.wRecvStartPort = tMpwCfg.wRcvStartPort;
    
    g_cMpwApp.m_tCfg.m_wMcuIdB   = tMpwCfg.wMcuId;
    g_cMpwApp.m_tCfg.m_dwMcuIpB  = tMpwCfg.dwConnectIpB;  // ������Ҫ
    g_cMpwApp.m_tCfg.m_wMcuPortB = tMpwCfg.wConnectPortB;

    memcpy(g_cMpwApp.m_tCfg.szAlias, tMpwCfg.achAlias, MAXLEN_ALIAS);
    g_cMpwApp.m_tCfg.szAlias[MAXLEN_ALIAS] = '\0';

    memset(m_tSndAddr, 0, sizeof(m_tSndAddr));

    // �����õ�����Ϣ
    if (FALSE == g_cMpwApp.m_tCfg.ParseParam())
    {
        OspPrintf(TRUE, FALSE, "[Mpw] some param for inital are error, please check them!\n");
        return;
    }

    // Ӳ����ʼ��
    g_cMpwApp.m_tCfg.m_tStatus.m_byVMPStyle = g_cMpwApp.m_tCfg.m_tMulPicParam.byType;
    s32 nRetval = m_cHardMulPic.Initialize(g_cMpwApp.m_tCfg.m_tMulPicParam, m_bDbVid);
    if (HARD_MULPIC_OK == nRetval)
    {
        OspPrintf(TRUE, FALSE, "[Mpw]Init %s Success!\n", g_cMpwApp.m_tCfg.szAlias);
        if (g_cMpwApp.m_tCfg.dwLocalIp == g_cMpwApp.m_tCfg.dwMcuIP)
        {
            g_cMpwApp.m_bEmbed = TRUE;
            g_cMpwApp.m_dwMcuNode = 0;
            SetTimer(EV_MPW_REGISTER_TIMER, MPW_REGISTER_TIMEOUT);
        }
        else if( g_cMpwApp.m_tCfg.dwLocalIp == g_cMpwApp.m_tCfg.m_dwMcuIpB)
        {
            g_cMpwApp.m_bEmbedB = TRUE;
            g_cMpwApp.m_dwMcuNodeB = 0;
            SetTimer(EV_MPW_REGISTER_TIMERB, MPW_REGISTER_TIMEOUT);
        }
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[Mpw]Init Mpw alias(%s) failed Error code: %d!\n", g_cMpwApp.m_tCfg.szAlias, nRetval);
        return;
    }

    m_cHardMulPic.SetBackBoardOut(TRUE);

    mpwlog("[Mpw] Follow is the init params we give:\n");
    mpwlog("\tThe type(defined in kdvmulpic.h,<=11):%u\n", g_cMpwApp.m_tCfg.m_tMulPicParam.byType);
    mpwlog("\tTotal use map number(<=5):%u\n", g_cMpwApp.m_tCfg.m_tMulPicParam.dwTotalMapNum);
    mpwlog("\tCore Speed(0(default) or other(392 maybe)) :%u\n", g_cMpwApp.m_tCfg.m_tMulPicParam.dwCoreSpd);
    mpwlog("\tMem Speed(0(default) or other(131 maybe)) :%u\n", g_cMpwApp.m_tCfg.m_tMulPicParam.dwMemSpd);
    mpwlog("\tMem Size (Must have,normal is 64M):%u\n", g_cMpwApp.m_tCfg.m_tMulPicParam.dwMemSize);
    mpwlog("\tDouble Video(0 one video ,1 two video) :%u\n", m_bDbVid);

    NEXTSTATE(IDLE);

    if(0 != g_cMpwApp.m_tCfg.dwMcuIP && FALSE == g_cMpwApp.m_bEmbed )
    {
        SetTimer(EV_MPW_CONNECT_TIMER, MPW_CONNECT_TIMEOUT );  // to be connect 
    }

    if(0 != g_cMpwApp.m_tCfg.m_dwMcuIpB && FALSE == g_cMpwApp.m_bEmbedB)
    {
        SetTimer(EV_MPW_CONNECT_TIMERB, MPW_CONNECT_TIMEOUT );  // to be connect 
    }

    return;
}

/*=============================================================================
  �� �� ���� ProcConnectTimeOut
  ��    �ܣ� �������ӳ�ʱ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� BOOL32 bIsConnectA
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::ProcConnectTimeOut(BOOL32 bIsConnectA)
{
    BOOL32 bRet = FALSE;
    if(TRUE == bIsConnectA)
    {
        bRet = ConnectToMcu(bIsConnectA, g_cMpwApp.m_dwMcuNode);
        if( TRUE == bRet )
        {
            SetTimer(EV_MPW_REGISTER_TIMER, MPW_REGISTER_TIMEOUT);
        }
        else
        {
            SetTimer(EV_MPW_CONNECT_TIMER, MPW_CONNECT_TIMEOUT);
        }
    }
    else
    {
        bRet = ConnectToMcu(bIsConnectA, g_cMpwApp.m_dwMcuNodeB);
        if( TRUE == bRet )
        {
            SetTimer(EV_MPW_REGISTER_TIMERB, MPW_REGISTER_TIMEOUT);
        }
        else
        {
            SetTimer(EV_MPW_CONNECT_TIMERB, MPW_CONNECT_TIMEOUT);
        }
    }
    return;
}
/*=============================================================================
  �� �� ���� ConnectToMcu
  ��    �ܣ� ����Mcu
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
BOOL32 CMpwInst::ConnectToMcu(BOOL32 bIsConnectA, u32& dwMcuNode)
{
    BOOL32 bRet = TRUE;
    if( !OspIsValidTcpNode(dwMcuNode))
    {
        if(TRUE == bIsConnectA)
        {  
            dwMcuNode = BrdGetDstMcuNode();  
        }
        else
        {  
            dwMcuNode = BrdGetDstMcuNodeB(); 
        }

        if ( OspIsValidTcpNode(dwMcuNode) )
        {
            ::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());

            mpwlog("[Mpw] Connect MCU Success,node is %u!\n", dwMcuNode);
        }
        else
        {
            //����ʧ��
            OspPrintf(TRUE, FALSE, "[Mpw] Fail to Connect MCU%u faield\n", g_cMpwApp.m_tCfg.wMcuId);
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
void CMpwInst::ProcRegisterTimeOut(BOOL32 bIsRegisterA)
{
    if(TRUE == bIsRegisterA)
    {
        RegisterToMcu(bIsRegisterA, g_cMpwApp.m_dwMcuNode);
        SetTimer(EV_MPW_REGISTER_TIMER, MPW_REGISTER_TIMEOUT);
    }
    else
    {
        RegisterToMcu(bIsRegisterA, g_cMpwApp.m_dwMcuNodeB);
        SetTimer(EV_MPW_REGISTER_TIMERB, MPW_REGISTER_TIMEOUT);
    }
    return;
}

/*=============================================================================
  �� �� ���� RegisterToMcu
  ��    �ܣ� ��Mcuע��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::RegisterToMcu(BOOL32 bIsRegisterA, u32 dwMcuNode)
{
    CServMsg cSvrMsg;
    TPeriEqpRegReq tReg;
    memset(&tReg, 0, sizeof(tReg));

    tReg.SetEqpId(g_cMpwApp.m_tCfg.byEqpId);   	
    tReg.SetEqpType(g_cMpwApp.m_tCfg.byEqpType);
    tReg.SetEqpAlias(g_cMpwApp.m_tCfg.szAlias);
    tReg.SetPeriEqpIpAddr(g_cMpwApp.m_tCfg.dwLocalIp);
    tReg.SetStartPort(g_cMpwApp.m_tCfg.wRecvStartPort);
    tReg.SetChnnlNum(g_cMpwApp.m_byChannelnum);   // ͨ����
    tReg.SetVersion(DEVVER_MPW);
    
    if(TRUE == bIsRegisterA)
    {
        tReg.SetMcuId((u8)g_cMpwApp.m_tCfg.wMcuId);
    }
    else
    {
        tReg.SetMcuId((u8)g_cMpwApp.m_tCfg.wMcuId);
    }

    cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
        
    post(MAKEIID(AID_MCU_PERIEQPSSN, g_cMpwApp.m_tCfg.byEqpId), VMPTW_MCU_REGISTER_REQ,
            cSvrMsg.GetServMsg(), cSvrMsg.GetServMsgLen(), dwMcuNode);

    return;
}

/*=============================================================================
  �� �� ���� MsgRegAckProc
  ��    �ܣ� ע��Ack��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::MsgRegAckProc(CMessage* const pMsg)
{
    
    if (NULL == pMsg->content)
    {
        OspPrintf(TRUE, FALSE, "[Mpw] the message can not be Null\n");
        return;
    }
    
    TPeriEqpRegAck tRegAck;
    CServMsg cServMsg(pMsg->content, pMsg->length);
    tRegAck = *(TPeriEqpRegAck*)cServMsg.GetMsgBody();

    if( pMsg->srcnode == g_cMpwApp.m_dwMcuNode)
    {
        g_cMpwApp.m_dwMcuIId = pMsg->srcid;
        g_cMpwApp.m_byRegAckNum++;
        KillTimer(EV_MPW_REGISTER_TIMER);
    }
    else if(pMsg->srcnode == g_cMpwApp.m_dwMcuNodeB)
    {
        g_cMpwApp.m_dwMcuIIdB = pMsg->srcid;
        g_cMpwApp.m_byRegAckNum++;
        KillTimer(EV_MPW_REGISTER_TIMERB);
    }

	// guzh [6/12/2007] У��Ự����
    if ( g_cMpwApp.m_dwMpcSSrc == 0 )
    {
        g_cMpwApp.m_dwMpcSSrc = tRegAck.GetMSSsrc();
    }
    else
    {
        // �쳣������Ͽ������ڵ�
        if ( g_cMpwApp.m_dwMpcSSrc != tRegAck.GetMSSsrc() )
        {
            OspPrintf(TRUE, FALSE, "[MsgRegAckProc] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
                      g_cMpwApp.m_dwMpcSSrc, tRegAck.GetMSSsrc());
            if ( OspIsValidTcpNode(g_cMpwApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cMpwApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cMpwApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cMpwApp.m_dwMcuNodeB);
            }      
            return;
        }
    }

    if(FIRST_REGACK == g_cMpwApp.m_byRegAckNum)
    {
        // guzh [10/9/2006] �������ƶ�������
        // FIXME: ע��: MCU�����ᷢ�͸õ�ַ��mpw�����иõ�ַ��ʹ�ö�û������
        m_tRtcpAddr.dwIp = htonl(tRegAck.GetMcuIpAddr());
        m_tRtcpAddr.wPort = htons(tRegAck.GetMcuStartPort());

        // FIXME: �˴�����û�д���prs������

        NEXTSTATE(NORMAL);
    }
    
    SendStatusMsg(1, 0, 0/*always*/); 
    return;
}

/*=============================================================================
  �� �� ���� MsgRegNackProc
  ��    �ܣ� ע�ᱻ�ܾ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::MsgRegNackProc(CMessage* const pMsg)
{
    if(pMsg->srcnode == g_cMpwApp.m_dwMcuNode)
    {
        OspPrintf(TRUE, FALSE, "[Mpw] Recv A board's register Nack message\n");
    }
    else if(pMsg->srcnode == g_cMpwApp.m_dwMcuNodeB)
    {
        OspPrintf(TRUE, FALSE, "[Mpw] Recv B board's register Nack message\n");
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[Mpw] Unknown register Nack message\n");
    }
    return;
}

/*=============================================================================
  �� �� ���� MsgStartVidMixProc
  ��    �ܣ� ��ʼ��Ƶ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::MsgStartVidMixProc(CMessage* const pMsg)
{
    if (NULL == pMsg->content)
    {
        OspPrintf(TRUE, FALSE, "[Mpw] the pointer can not be Null\n");
        CServMsg cServMsg;
        SendMsgToMcu(VMPTW_MCU_STARTVIDMIX_NACK, &cServMsg);
        return;
    }

    CServMsg cServMsg(pMsg->content, pMsg->length);
    memcpy(&m_tParam, cServMsg.GetMsgBody(), sizeof(CKDVVMPParam));
	
    // MCUǰ�����, zgc, 2007-09-27
	TCapSupportEx tCapSupportEx = *(TCapSupportEx*)(cServMsg.GetMsgBody()+sizeof(CKDVVMPParam)+sizeof(u8));
	if( g_nMpwlog == 1 )
	{
		tCapSupportEx.Print();
	}
    m_tCapSupportEx = tCapSupportEx;

    m_cConfId = cServMsg.GetConfId();
    
    // ״̬�����
    switch(CurState())
    {
    case NORMAL:
        break;

    default:
        OspPrintf(TRUE, FALSE, "[Mpw]Cannot start merge while in state:%d\n", CurState());
        return;
    }

    // ���õ�һ·��Ƶ����������п�ʼ
    g_cMpwApp.m_tCfg.GetDefaultParam(m_tParam.m_byEncType, g_cMpwApp.m_tCfg.m_tVideoEncParam[0]);    // 
    g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_wVideoHeight = m_tParam.m_wVideoHeight;
    g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_wVideoWidth  = m_tParam.m_wVideoWidth;
    g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_byEncType    = MEDIA_TYPE_H261; // m_tParam.m_byEncType;

    // ����Ĭ��ֵ
    if (m_tParam.m_wBitRate == 0)
    {
        m_tParam.m_wBitRate = 1200;
    }
    g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_wBitRate = m_tParam.m_wBitRate;

    if (g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_dwSndNetBand == 0)
    {
        g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_dwSndNetBand = (u32)(m_tParam.m_wBitRate*2+100);
    }

    // ��ӡ��ʼ�Ĳ���
    mpwlog("\tBitRate: %u \n\tVideoHeight: %u \n\tVideoWidth: %u\n", m_tParam.m_wBitRate, 
                                                      m_tParam.m_wVideoHeight, 
                                                      m_tParam.m_wVideoWidth);

    mpwlog("Follows are the params we used to start the Mpw:\n\n");
    mpwlog("/*------------------- First video channel ---------------------------*/\n");
    mpwlog("\tEncType: \t%u \n\tRcMode: \t%u \n\tMaxKeyFrameInterval: \t%d \n\tMaxQuant: \t%u \n\tMinQuant: \t%u \n\tEncryptType: \t%d \n",
                               g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_byEncType, g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_byRcMode,
                               g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_byMaxKeyFrameInterval, g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_byMaxQuant,
                               g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_byMinQuant, m_tParam.m_tVideoEncrypt[0].GetEncryptMode());
    mpwlog("\tBitRate: \t%u\n\tSndNetBand: \t%u \n\tFrameRate: \t%u \n\tImageQulity: \t%u \n\tVideoWidth: \t%u \n\tVideoHeight: \t%u\n",
                               g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_wBitRate, g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_dwSndNetBand,
                               g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_byFrameRate, g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_byImageQulity,
                               g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_wVideoWidth, g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_wVideoHeight);
    mpwlog("\tSend IP: 0x%x and Port: %u\n", m_tSndAddr[0].dwIp, m_tSndAddr[0].wPort);
    
    mpwlog("/*------------------- Second video channel --------------------------*/\n");
    mpwlog("\tEncType: \t%u \n\tRcMode: \t%u \n\tMaxKeyFrameInterval: \t%u \n\tMaxQuant: \t%u \n\tMinQuant: \t%u \n\tEncryptType: \t%d \n",
                               g_cMpwApp.m_tCfg.m_tVideoEncParam[1].m_byEncType, g_cMpwApp.m_tCfg.m_tVideoEncParam[1].m_byRcMode,
                               g_cMpwApp.m_tCfg.m_tVideoEncParam[1].m_byMaxKeyFrameInterval, g_cMpwApp.m_tCfg.m_tVideoEncParam[1].m_byMaxQuant,
                               g_cMpwApp.m_tCfg.m_tVideoEncParam[1].m_byMinQuant, m_tParam.m_tVideoEncrypt[1].GetEncryptMode());
    mpwlog("\tBitRate: \t%u \n\tSndNetBand: \t%u \n\tFrameRate: \t%u \n\tImageQulity: \t%u \n\tVideoWidth: \t%u \n\tVideoHeight: \t%u\n",
                               g_cMpwApp.m_tCfg.m_tVideoEncParam[1].m_wBitRate, g_cMpwApp.m_tCfg.m_tVideoEncParam[1].m_dwSndNetBand,
                               g_cMpwApp.m_tCfg.m_tVideoEncParam[1].m_byFrameRate, g_cMpwApp.m_tCfg.m_tVideoEncParam[1].m_byImageQulity,
                               g_cMpwApp.m_tCfg.m_tVideoEncParam[1].m_wVideoWidth, g_cMpwApp.m_tCfg.m_tVideoEncParam[1].m_wVideoHeight);
    mpwlog("\tSend Ip: 0x%x and Port: %u\n", m_tSndAddr[1].dwIp, m_tSndAddr[1].wPort);
    mpwlog( "/*-------------------------------------------------------------------*/\n\n");

    u8 byStyle = ConvertVc2Hard(m_tParam.m_byVMPStyle); // ��ǰ�Ǽ�����

    mpwlog("Convert Mpw Params! (before style = %d, after style = %d)\n",
                                                                    m_tParam.m_byVMPStyle, byStyle);


    if (byStyle != g_cMpwApp.m_tCfg.m_tStatus.m_byVMPStyle) // ����ǰ��һ��
    {
        m_cHardMulPic.SetMulPicType(byStyle);  
    }
    //zbq[01/03/2008]���������������
    if ( g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_byEncType == MEDIA_TYPE_H264 &&
        (g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_wVideoHeight > 288 ||
         g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_wVideoWidth  > 352 ))
    {
        mpwlog("[StartVidMixProc] Video.0 format has been adjusted due to Height.%d, Width.%d\n",
                g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_wVideoHeight, g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_wVideoWidth);
        g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_wVideoHeight = 288;
        g_cMpwApp.m_tCfg.m_tVideoEncParam[0].m_wVideoWidth  = 352;
    }

    s32 nRet = m_cHardMulPic.StartMerge(g_cMpwApp.m_tCfg.m_tVideoEncParam, m_tSndAddr);  // ��ʼ����(����ָ����ַ)
    if (HARD_MULPIC_ERROR == nRet)
    {
        OspPrintf(TRUE, FALSE, "[Mpw]Fail to call StartMerge(), Error code: %d\n", nRet);
        cServMsg.SetMsgBody(NULL, 0);
        cServMsg.SetErrorCode(1);
        SendMsgToMcu(VMPTW_MCU_STARTVIDMIX_NACK, &cServMsg);
        return;
    }
    else
    {
        SendMsgToMcu(VMPTW_MCU_STARTVIDMIX_ACK, &cServMsg);
    }

    cServMsg.SetMsgBody(NULL, 0);

    mpwlog("StartWork, style is %u!\n", byStyle);
    
    // �ȿ�ʼ���ϣ�������ͨ��
    if (TRUE == StartHardwareMix(m_tParam))  // ����ͨ��
    {
        cServMsg.SetErrorCode(0);
        mpwlog("StartHardwareMix succeed !\n");
    }
    else
    {
        cServMsg.SetErrorCode(1);
        mpwlog("StartHardwareMix failed !\n");
    }

    u8 byType = MPW_ID;
    cServMsg.SetMsgBody(&byType, sizeof(byType));
    SendMsgToMcu(VMPTW_MCU_STARTVIDMIX_NOTIF, &cServMsg);

    SendStatusMsg(1, 1, 0/*always*/); 
    
    return;
}

/*=============================================================================
  �� �� ���� MsgStopVidMixProc
  ��    �ܣ� ֹͣ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::MsgStopVidMixProc(CMessage* const pMsg)
{
    if(NULL == pMsg)
    {
        OspPrintf(TRUE, FALSE, "[Mpw] The pointer cannot be Null (MsgStopVidMixProc)\n");
        CServMsg cServMsg;
        SendMsgToMcu(VMPTW_MCU_STOPVIDMIX_NACK, &cServMsg);
        return;
    }

    CServMsg cServMsg(pMsg->content, pMsg->length);

    switch(CurState())
    {
    case RUNNING:
        break;
    default:
        OspPrintf(TRUE, FALSE, "[Mpw]Error State(stop mix): %d\n", CurState());
        return;
    }

    // ֹͣ����
    int nRetCode = m_cHardMulPic.StopMerge();
    if (HARD_MULPIC_OK != nRetCode)
    {
        cServMsg.SetErrorCode(1);
        cServMsg.SetMsgBody(NULL, 0);
        SendMsgToMcu(VMPTW_MCU_STOPVIDMIX_NACK, &cServMsg);
        return;
    }
    else 
    {
        cServMsg.SetErrorCode(0);
        cServMsg.SetMsgBody(NULL, 0);
        SendMsgToMcu(VMPTW_MCU_STOPVIDMIX_ACK, &cServMsg);
        
        for(u8 byChnNo = 0; byChnNo < g_cMpwApp.m_tCfg.m_tStatus.m_byMemberNum; byChnNo++)
        {
            m_cHardMulPic.RemoveChannel(byChnNo); // �Ƴ�ͨ��
            m_bAddVmpChannel[byChnNo] = FALSE;
            m_cHardMulPic.SetVideoActivePT(byChnNo, 0, 0);
            m_cHardMulPic.SetVidDecryptKey(byChnNo, NULL, 0, 0);
            m_cHardMulPic.SetVidEncryptPT(byChnNo, 0);
            m_cHardMulPic.SetVidEncryptKey(byChnNo, NULL, 0, 0);      
        }

        memset(&m_tParam, 0, sizeof(CKDVVMPParam));
        NEXTSTATE(NORMAL);
    }

    cServMsg.SetMsgBody(NULL, 0);
    u8 byType = MPW_ID;
    cServMsg.SetMsgBody(&byType, sizeof(byType));
    SendMsgToMcu(VMPTW_MCU_STOPVIDMIX_NOTIF, &cServMsg);
    SendStatusMsg(1, 0, 0/*always*/);
    return;
}

/*=============================================================================
  �� �� ���� MsgChangeMixParamProc
  ��    �ܣ� �޸ĸ��ϲ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::MsgChangeMixParamProc(CMessage* const pMsg)
{
    
    if(NULL == pMsg)
    {
        OspPrintf(TRUE, FALSE, "[Mpw] The pointer cannot be Null! (MsgChangeMixParamProc)\n");
        CServMsg cServMsg;
        SendMsgToMcu(VMPTW_MCU_CHANGEVIDMIXPARAM_NACK, &cServMsg);
        return;
    }

    CServMsg cServMsg(pMsg->content, pMsg->length);
    CKDVVMPParam tParam;

    switch(CurState())
    {
    case RUNNING:
        break;
    default:
        OspPrintf(TRUE, FALSE, "[Mpw]Error State: %d (MsgChangeMixParamProc)\n", CurState());
        return;
    }

    // cServMsg.GetMsgBody((u8*)&tParam, 2*sizeof(CKDVVMPParam));
    memcpy(&m_tParam, cServMsg.GetMsgBody(), sizeof(CKDVVMPParam));
    memcpy(&tParam, &m_tParam, sizeof(CKDVVMPParam));
   
    mpwlog("EncType= %d, m_byMemberNum= %d, m_byVMPStyle= %d\n", 
                    tParam.m_byEncType, tParam.m_byMemberNum, tParam.m_byVMPStyle);

    mpwlog("m_wBitRate= %d, m_wVideoHeight= %d, m_wVideoWidth= %d\n", 
                    tParam.m_wBitRate, tParam.m_wVideoHeight, tParam.m_wVideoWidth);

    BOOL32 bCheckRet = AdjustMpwStyle(tParam);
    if (TRUE == bCheckRet)
    {
        cServMsg.SetErrorCode(0); //ok
        SendMsgToMcu(VMPTW_MCU_CHANGEVIDMIXPARAM_ACK, &cServMsg);
    }
    else
    {
        cServMsg.SetErrorCode(1); //error
        SendMsgToMcu(VMPTW_MCU_CHANGEVIDMIXPARAM_NACK, &cServMsg);
    }

    cServMsg.SetMsgBody(NULL, 0);
    u8 byType = MPW_ID;
    cServMsg.SetMsgBody(&byType, sizeof(u8));
    SendMsgToMcu(VMPTW_MCU_CHANGESTATUS_NOTIF, &cServMsg);

    SendStatusMsg(1, 1, 0/*always*/);

    u8 byMaxMemNum = GetMpwNumOfStyle(tParam.m_byVMPStyle);
    mpwlog("The Num of pictures: %d\n", byMaxMemNum);

    return;
}

/*=============================================================================
  �� �� ���� AdjustMpwStyle
  ��    �ܣ� �����ϳɷ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CKDVVMPParam& tParam
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMpwInst::AdjustMpwStyle(CKDVVMPParam& tParam)
{
    s32 nRet = 0;
    s32 nErrNum = 0;
    TNetAddress tNetAddr;
    memset(&tNetAddr, 0, sizeof(TNetAddress));
    TNetAddress tNetRtcpAddr;
    memset(&tNetRtcpAddr, 0, sizeof(TNetAddress));

    BOOL bChangeStyle = FALSE;
    u8   byStyle = ConvertVc2Hard(tParam.m_byVMPStyle);
    
    mpwlog("The UpLevel style: %d, HardStyle: %d\n", tParam.m_byVMPStyle, byStyle);

    // tNetAddr.dwIp = g_cMpwApp.m_tCfg.dwMcuIP;
    tNetAddr.dwIp = 0;
    tNetRtcpAddr.dwIp = m_tRtcpAddr.dwIp;

    BOOL32 bRet = TRUE;
    if (byStyle != g_cMpwApp.m_tCfg.m_tStatus.m_byVMPStyle) // ��ԭ�з��Ƚ�
    {
        bChangeStyle = TRUE;
        mpwlog("Change Mpw Style to %u!\n", byStyle);

        // �����Ƴ����е�ͨ��
        mpwlog("The g_cMpwApp.m_tCfg.m_tStatus.m_byMemberNum= %d\n", g_cMpwApp.m_tCfg.m_tStatus.m_byMemberNum);
        for (u8 byChnNo = 0; byChnNo < g_cMpwApp.m_tCfg.m_tStatus.m_byMemberNum; byChnNo++)
        {
            if (m_bAddVmpChannel[byChnNo] == TRUE)
            {
                m_bAddVmpChannel[byChnNo] = FALSE;
                nRet = m_cHardMulPic.RemoveChannel(byChnNo); // �Ƴ�����ͨ��
                if(HARD_MULPIC_OK != nRet)
                {
                    bRet = FALSE;
                    mpwlog("Remove channel: %u, return code: %d.\n", byChnNo, nRet);
                }
                      
            }
        }
        
        nRet = m_cHardMulPic.SetMulPicType(byStyle); // ���ø��Ϸ��
        if (HARD_MULPIC_OK != nRet)
        {          
            OspPrintf(TRUE, FALSE, "[MPw] Fail to Change style, Error code: %d !\n", nRet);
            return FALSE;
        }
        mpwlog("Total channel is %u!\n", tParam.m_byMemberNum);

    }

    g_cMpwApp.m_tCfg.m_tStatus.m_byVMPStyle = byStyle;
    g_cMpwApp.m_tCfg.m_tStatus.m_byMemberNum = tParam.m_byMemberNum;

    // ���ü��ܲ���
    //SetEncryptParam(0, &tParam.m_tVideoEncrypt[0], GetActivePayload(tParam.m_byEncType));
    SetEncryptParam(0, &tParam.m_tVideoEncrypt[0], &tParam.m_tDoublePayload[0]);

    for(u8 byChnNo = 0; byChnNo < g_cMpwApp.m_tCfg.m_tStatus.m_byMemberNum; byChnNo++)
    {
        SetDecryptParam(byChnNo, &tParam.m_tVideoEncrypt[byChnNo], &tParam.m_tDoublePayload[byChnNo]);

        mpwlog("RealPayLoad: %d, ActivePayload: %d.\n",
                        tParam.m_tDoublePayload[byChnNo].GetRealPayLoad(), 
                        tParam.m_tDoublePayload[byChnNo].GetActivePayload());

        if ((TRUE == m_bAddVmpChannel[byChnNo]) && (tParam.m_atMtMember[byChnNo].IsNull())) // �����ͬ������Ա��ͬ
        {
            m_bAddVmpChannel[byChnNo] = FALSE;
            nRet = m_cHardMulPic.RemoveChannel(byChnNo); // ������ԭ�з����ͬ����
            mpwlog("Remove Channel: %d, return Code: %d\n", byChnNo, nRet);
        }

        if ( ( m_bAddVmpChannel[byChnNo] == FALSE) &&
             ( tParam.m_tDoublePayload[byChnNo].GetRealPayLoad() != 0 ||
               tParam.m_tDoublePayload[byChnNo].GetActivePayload() != 0 ) ) 
        {
            // ���ͨ��
            tNetAddr.wPort = htons(g_cMpwApp.m_tCfg.wRecvStartPort + byChnNo*PORTSPAN);
            tNetRtcpAddr.wPort = htons(GetSendRtcpPort(byChnNo));
            nRet = m_cHardMulPic.AddChannel(byChnNo, tNetAddr, tNetRtcpAddr);
            if (HARD_MULPIC_OK != nRet)
            {
                nErrNum++;
            }
            else
            {
                m_bAddVmpChannel[byChnNo] = TRUE;
            }
            mpwlog("Add channel: %u, return Code: %d!\n", byChnNo, nRet);
        }
    }
    return (0 == nErrNum);
}

/*=============================================================================
  �� �� ���� MsgGetMixParam
  ��    �ܣ� ȡ�ϳɲ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::MsgGetMixParam(CMessage* const pMsg)
{
    CServMsg cServMsg((u8*)&(g_cMpwApp.m_tCfg.m_tStatus), sizeof(g_cMpwApp.m_tCfg.m_tStatus));
    SendMsgToMcu(VMPTW_MCU_GETVIDMIXPARAM_ACK, &cServMsg);
    return;
}

/*=============================================================================
  �� �� ���� MsgSetBitRate
  ��    �ܣ� ��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::MsgSetBitRate(CMessage* const pMsg)
{
    if(NULL == pMsg)
    {
        OspPrintf(TRUE, FALSE, "[Mpw] The pointer cannot be Null (MsgSetBitRate)\n");
        CServMsg cServMsg;
        SendMsgToMcu(VMPTW_MCU_SETCHANNELBITRATE_NACK, &cServMsg);
        return;
    }
    CServMsg cServMsg(pMsg->content, pMsg->length);
   
    u16 wBitrate;
    cServMsg.GetMsgBody((u8*)&wBitrate, sizeof(u16));
    u8 byChannelnum = cServMsg.GetChnIndex();
    s32 nRet = 0;
    BOOL32 bRet = TRUE;
    if (FIRST_CHANNEL == byChannelnum)// �Ƿ�˫����
    {
        nRet = m_cHardMulPic.ChangeBitRate(FALSE, wBitrate);
        if(HARD_MULPIC_OK != nRet)
        {
            bRet = FALSE;
        }
    }
    if (SECOND_CHANNEL == byChannelnum)
    {
        bRet = m_cHardMulPic.ChangeBitRate(TRUE, wBitrate);
        if(HARD_MULPIC_OK != nRet)
        {
            bRet = FALSE;
        }
    }
    if(TRUE == bRet)
    {
        SendMsgToMcu(VMPTW_MCU_SETCHANNELBITRATE_ACK, &cServMsg);
    }
    else
    {
        SendMsgToMcu(VMPTW_MCU_SETCHANNELBITRATE_NACK, &cServMsg);
    }
    return;
}

/*=============================================================================
  �� �� ���� UpdataEncryptParam
  ��    �ܣ� �޸�ͨ���ļ��ܲ�����ָ��ͨ��û��ʱ������һ��ͨ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::UpdataEncryptParam(CMessage * const pMsg)
{
    if(NULL == pMsg)
    {
        OspPrintf(TRUE, FALSE, "[Mpw] The pointer cannot be Null. (UpdataEncryptParam)\n");
        return;
    }

    u8  byChannelNo = 0;
    TMediaEncrypt  tVideoEncrypt;
    TDoublePayload tDoublePayload;

    CServMsg cServMsg(pMsg->content, pMsg->length);

    byChannelNo    = *(u8 *)(cServMsg.GetMsgBody());
    tVideoEncrypt  = *(TMediaEncrypt *)(cServMsg.GetMsgBody() + sizeof(u8));
    tDoublePayload = *(TDoublePayload *)(cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TMediaEncrypt));

    mpwlog("byChannelNo: %d\n", byChannelNo);
    mpwlog("ActivePayload: %d, RealPayLoad: %d\n", tDoublePayload.GetActivePayload(),
                                                 tDoublePayload.GetRealPayLoad() );

    if (!m_bAddVmpChannel[byChannelNo] &&
        (tDoublePayload.GetRealPayLoad() != 0 ||
        tDoublePayload.GetActivePayload() != 0))
    {
        s32 nRet;
        TNetAddress tNetAddr;
        TNetAddress tNetRtcpAddr;
        // ���ͨ��
        // tNetAddr.dwIp = g_cMpwApp.m_tCfg.dwMcuIP;
        tNetAddr.dwIp = 0;
        tNetRtcpAddr.dwIp = m_tRtcpAddr.dwIp;
        tNetAddr.wPort = htons(g_cMpwApp.m_tCfg.wRecvStartPort + byChannelNo*PORTSPAN);
        tNetRtcpAddr.wPort = htons(GetSendRtcpPort(byChannelNo));
        nRet = m_cHardMulPic.AddChannel(byChannelNo, tNetAddr, tNetRtcpAddr);
        if (HARD_MULPIC_OK == nRet)
        {
            m_bAddVmpChannel[byChannelNo] = TRUE;
        }
    
    }

    if(TRUE == m_bAddVmpChannel[byChannelNo] )
    {
        SetDecryptParam(byChannelNo, &tVideoEncrypt, &tDoublePayload);
    }

    cServMsg.SetMsgBody(NULL, 0);
    u8 byType = MPW_ID;
    cServMsg.SetMsgBody(&byType, sizeof(byType));
    SendMsgToMcu(VMPTW_MCU_CHANGESTATUS_NOTIF, &cServMsg);
    SendStatusMsg(1, 1, 0/*always*/);
    return;
}

/*=============================================================================
  �� �� ���� MsgFastUpdatePic
  ��    �ܣ� ����ؼ�֡����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::MsgFastUpdatePic(void)
{
    u32 dwTimeInterval = 1*OspClkRateGet();

    u32 dwCurTick = OspTickGet();	
    if( dwCurTick - m_dwLastFUPTick > dwTimeInterval)
    {
        m_dwLastFUPTick = dwCurTick;
        m_cHardMulPic.SetFastUpdata();
    }
    return;
}

/*=============================================================================
  �� �� ���� MsgNeedIFrameProc
  ��    �ܣ� �����Ƿ���Ҫ�ؼ�֡
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::MsgNeedIFrameProc(void)
{
    CServMsg cServMsg;
    cServMsg.SetConfId(m_cConfId);
    TKdvDecStatis tDecStatis;

    for (u8 byChnNo = 0; byChnNo < g_cMpwApp.m_byChannelnum; byChnNo++)
    {
        if (TRUE == m_bAddVmpChannel[byChnNo])
        {
            memset(&tDecStatis, 0, sizeof(TKdvDecStatis));
            m_cHardMulPic.GetVidRecvStatis(byChnNo, tDecStatis);
            if (tDecStatis.m_bVidCompellingIFrm)
            {
                cServMsg.SetChnIndex(byChnNo);
                SendMsgToMcu(VMPTW_MCU_NEEDIFRAME_CMD, &cServMsg);
                mpwlog("Mpw channel: %d request iframe!\n", byChnNo);
            }
        }
    }

    SetTimer(EV_MPW_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);
    return;
}

/*=============================================================================
  �� �� ���� MpwStatusShow
  ��    �ܣ� ��ʾ״̬��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::MpwStatusShow(void)
{
    TMulPicStatus tMulPicStatus;
    TMulPicStatis tMulPicStatis;
    memset(&tMulPicStatus, 0, sizeof(tMulPicStatus));
    memset(&tMulPicStatis, 0, sizeof(tMulPicStatis));

    m_cHardMulPic.GetStatus(tMulPicStatus);
    m_cHardMulPic.GetStatis(tMulPicStatis);

    ::OspPrintf(TRUE, FALSE, "\t===========The state of Mpw is below================\n");

    ::OspPrintf(TRUE, FALSE, "\tIDLE: %d, NORMAL: %d, RUNNING: %d, \n\tMpw Current State: %d\n",
	                          IDLE, NORMAL, RUNNING, CurState());

    ::OspPrintf(TRUE, FALSE, "\t===========The status of Mpw is below===============\n");
    ::OspPrintf(TRUE, FALSE, "\tMerge: \t%d \n\tStyle: \t%d \n\tMapNum: \t%d \n\tCurChannel: \t%d\n",
                              tMulPicStatus.bMergeStart,
                              tMulPicStatus.byType,
                              tMulPicStatus.dwTotalMapNum,
                              tMulPicStatus.byCurChnNum);

    ::OspPrintf(TRUE, FALSE, "\tSend Ip Address: 0x%x, Port: %d\n", 
                ntohl(tMulPicStatus.atNetSndAddr.dwIp), tMulPicStatus.atNetSndAddr.wPort);
 
    s32 nLoop = 0;
    for (nLoop = 0; nLoop < tMulPicStatus.byCurChnNum; nLoop++)
    {    
        ::OspPrintf(TRUE, FALSE, "\tReceive Channel: %d Ip Address: 0x%x @ Port: %d\n",
                    nLoop, ntohl(tMulPicStatus.atNetRcvAddr[nLoop].dwIp), tMulPicStatus.atNetRcvAddr[nLoop].wPort);
    }
    ::OspPrintf(TRUE, FALSE, "\t=============The Video Encoding Params==============\n");
    ::OspPrintf(TRUE, FALSE, "\tEncType: \t%d \n\tComMode: \t%d \n\tKeyFrameInter: \t%d \n\tMaxQuant: \t%d \n\tMinQuant: \t%d\t\n",
                              tMulPicStatus.tVideoEncParam.m_byEncType,
                              tMulPicStatus.tVideoEncParam.m_byRcMode,
                              tMulPicStatus.tVideoEncParam.m_byMaxKeyFrameInterval,
                              tMulPicStatus.tVideoEncParam.m_byMaxQuant,
                              tMulPicStatus.tVideoEncParam.m_byMinQuant);

    ::OspPrintf(TRUE, FALSE, "\tBitRate: \t%d \n\tSndBandWith: \t%d \n\tFrameRate: \t%d \n\tImageQuality: \t%d \n\tVideoWidth: \t%d \n\tVideoHeight: \t%d\t\n",
                              tMulPicStatus.tVideoEncParam.m_wBitRate,
                              tMulPicStatus.tVideoEncParam.m_dwSndNetBand,
                              tMulPicStatus.tVideoEncParam.m_byFrameRate,
                              tMulPicStatus.tVideoEncParam.m_byImageQulity,
                              tMulPicStatus.tVideoEncParam.m_wVideoWidth,
                              tMulPicStatus.tVideoEncParam.m_wVideoHeight);

    ::OspPrintf(TRUE, FALSE, "\t==================Network Statics===================\n");
    ::OspPrintf(TRUE, FALSE, "\tSendBitRate: %d \n\tSendPackNum: %d \n\tLostPackNum: %d\t\n",
                              tMulPicStatis.m_dwSendBitRate,
                              tMulPicStatis.m_dwSendPackNum,
                              tMulPicStatis.m_dwSendDiscardPackNum);

    for(nLoop = 0; nLoop < tMulPicStatus.byCurChnNum; nLoop++)
    {
        ::OspPrintf(TRUE, FALSE, "\tReceive Channel: %d BitRate: %d\t RecvPackNum: %d\t LostPackNum: %d\n",nLoop,
                                  tMulPicStatis.atMulPicChnStatis[nLoop].m_dwRecvBitRate,
                                  tMulPicStatis.atMulPicChnStatis[nLoop].m_dwRecvPackNum,
                                  tMulPicStatis.atMulPicChnStatis[nLoop].m_dwRecvLosePackNum);
    }
    return;
}

/*=============================================================================
  �� �� ���� SendStatusMsg
  ��    �ܣ� �ϱ�״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byOnline���Ƿ�����
             u8 byState���Ƿ���ʹ��
             u8 byStyle����ʱû��ʹ��
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::SendStatusMsg(u8 byOnline, u8 byState, u8 byStyle)
{
    CServMsg cServMsg;
    TPeriEqpStatus tMpwStatus;
    memset(&tMpwStatus,0,sizeof(TPeriEqpStatus));

    tMpwStatus.m_byOnline = byOnline;
    tMpwStatus.SetMcuEqp((u8)g_cMpwApp.m_tCfg.wMcuId, g_cMpwApp.m_tCfg.byEqpId, g_cMpwApp.m_tCfg.byEqpType);
    tMpwStatus.m_tStatus.tVmp.m_byUseState = byState;
    tMpwStatus.m_tStatus.tVmp.m_tVMPParam.SetVMPStyle(g_cMpwApp.m_tCfg.m_tStatus.m_byVMPStyle);
    tMpwStatus.SetAlias(g_cMpwApp.m_tCfg.szAlias);
    tMpwStatus.m_tStatus.tVmp.m_byChlNum = g_cMpwApp.m_byChannelnum;

    u8 byType = MPW_ID;
    cServMsg.SetMsgBody((u8*)&(tMpwStatus), sizeof(TPeriEqpStatus));
    cServMsg.CatMsgBody(&byType, sizeof(byType));
    SendMsgToMcu(VMPTW_MCU_VMPTWSTATUS_NOTIF, &cServMsg);
    return;
}

/*=============================================================================
  �� �� ���� ProcDisconnect
  ��    �ܣ� �������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::ProcDisconnect(CMessage *const pMsg)
{

    if(NULL == pMsg)
    {
        OspPrintf(TRUE, FALSE, "[Mpw] The pointer cannot be Null. (ProcDisconnect)");
        return;
    }

    mpwlog("Disconnect with Mcu, the node: %d\n",pMsg->srcnode);
    mpwlog("The A's Node: %d, B's Node: %d\n", g_cMpwApp.m_dwMcuNode, g_cMpwApp.m_dwMcuNodeB);
    
    u32 dwDisNode = *(u32*)pMsg->content;
    
    if ( INVALID_NODE != dwDisNode )
    {
        OspDisconnectTcpNode( dwDisNode );
    }
    if( dwDisNode == g_cMpwApp.m_dwMcuNode)
    {
        g_cMpwApp.FreeStatusDataA();
        OspPrintf(TRUE, FALSE, "[Mpw] Disconnect with A Node: %d\n", g_cMpwApp.m_dwMcuNode);
        SetTimer(EV_MPW_CONNECT_TIMER, MPW_CONNECT_TIMEOUT);
    }
    else if (dwDisNode == g_cMpwApp.m_dwMcuNodeB)
    {
        g_cMpwApp.FreeStatusDataB();
        SetTimer(EV_MPW_CONNECT_TIMERB, MPW_CONNECT_TIMEOUT);
        OspPrintf(TRUE, FALSE, "[Mpw] Disconnect with B Node: %d\n", g_cMpwApp.m_dwMcuNodeB);
    }

	// ������һ����������Mcuȡ��������״̬���ж��Ƿ�ɹ�
	if (INVALID_NODE != g_cMpwApp.m_dwMcuNode || INVALID_NODE != g_cMpwApp.m_dwMcuNodeB)
	{
		if (OspIsValidTcpNode(g_cMpwApp.m_dwMcuNode))
		{
			post( g_cMpwApp.m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMpwApp.m_dwMcuNode );     
        
			OspPrintf(TRUE, FALSE, "[MsgDisconnectProc] GetMsStatusReq. McuNode.A\n");
		}
		else if (OspIsValidTcpNode(g_cMpwApp.m_dwMcuNodeB))
		{
			post( g_cMpwApp.m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMpwApp.m_dwMcuNodeB );        

			OspPrintf(TRUE, FALSE, "[DaemonProcMcuDisconnect] GetMsStatusReq. McuNode.B\n");
		}

		SetTimer(EV_MPW_GETMSSTATUS_TIMER, WAITING_MSSTATUS_TIMEOUT);
		return;
	}
	
    if(INVALID_NODE == g_cMpwApp.m_dwMcuNode && INVALID_NODE == g_cMpwApp.m_dwMcuNodeB)
    {
		StopAndClearInstStatus();
    }
    return;
}

/*=============================================================================
  �� �� ���� ProcGetMsStatusRsp
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::ProcGetMsStatusRsp(CMessage* const pMsg)
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg(pMsg->content, pMsg->length);
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event )
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer(EV_MPW_GETMSSTATUS_TIMER);
        mpwlog("[DeamonProcGetMsStatus]. receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                  ptMsStatus->IsMsSwitchOK());

        if(ptMsStatus->IsMsSwitchOK()) // �����ɹ�
        {
            bSwitchOk = TRUE;
        }
	}

	// ����ʧ�ܻ��߳�ʱ
	if( !bSwitchOk )
	{
		StopAndClearInstStatus();
		if( INVALID_NODE == g_cMpwApp.m_dwMcuNode)// �п��ܲ�����������Connect�������
		{
			SetTimer(EV_MPW_CONNECT_TIMER, MPW_CONNECT_TIMEOUT);
		}
		if( INVALID_NODE == g_cMpwApp.m_dwMcuNodeB)
		{
			SetTimer(EV_MPW_CONNECT_TIMERB, MPW_CONNECT_TIMEOUT);
		}
	}
	return;
}

/*=============================================================================
  �� �� ���� StopAndClearInstStatus
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::StopAndClearInstStatus(void)
{
    s32 nRetCode = m_cHardMulPic.StopMerge();
    OspPrintf(TRUE, FALSE, "[Mpw]: Stop mpw to merge, the return code: %d!\n", nRetCode);
    g_cMpwApp.m_byRegAckNum = 0;
	g_cMpwApp.m_dwMpcSSrc = 0;
	
	memset( m_bAddVmpChannel, 0, sizeof(m_bAddVmpChannel) );

	NEXTSTATE(IDLE);
	return;
}

/*=============================================================================
  �� �� ���� StartHardwareMix
  ��    �ܣ� ���Ӻϳ�ͨ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CKDVVMPParam& tParam
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMpwInst::StartHardwareMix( CKDVVMPParam& tParam )
{
    s32 nRet = 0;
    u8  byErrNum = 0; 
    TNetAddress tNetAddr;
    memset(&tNetAddr, 0, sizeof(TNetAddress));
    TNetAddress tNetRtcpAddr;
    memset(&tNetRtcpAddr, 0, sizeof(TNetAddress));
   
    u8 byStyle = ConvertVc2Hard(tParam.m_byVMPStyle);

    g_cMpwApp.m_tCfg.m_tStatus.m_byVMPStyle =  byStyle; // �ϳɷ��
    g_cMpwApp.m_tCfg.m_tStatus.m_byMemberNum = tParam.m_byMemberNum; // ����ϳɵĳ�Ա��

    tNetAddr.dwIp = 0;  // ����
    tNetRtcpAddr.dwIp = m_tRtcpAddr.dwIp; // Rtcp

    //zbq [10/13/2007] FEC ֧��
    SetEncryptParam(0, &tParam.m_tVideoEncrypt[0], &tParam.m_tDoublePayload[0]); // ���õ�һ·�������

    u8 byFECType = m_tCapSupportEx.GetVideoFECType();
    BOOL32 bFECEnable = FECTYPE_NONE != byFECType ? TRUE : FALSE;
    m_cHardMulPic.SetVidFecEnable(0, bFECEnable);
    mpwlog("[StartHardwareMix] Fec Enable.%d, FecMode.%d\n", bFECEnable, byFECType);

    for(u8 byChnNo = 0; byChnNo < tParam.m_byMemberNum; byChnNo++)
    {
        SetDecryptParam(byChnNo, &tParam.m_tVideoEncrypt[byChnNo], &tParam.m_tDoublePayload[byChnNo]); // ���ý������

        mpwlog("RealPayLoad: %d, ActivePayload: %d.\n",
                tParam.m_tDoublePayload[byChnNo].GetRealPayLoad(), 
                tParam.m_tDoublePayload[byChnNo].GetActivePayload());

        if ( tParam.m_tDoublePayload[byChnNo].GetRealPayLoad()   != 0 || 
             tParam.m_tDoublePayload[byChnNo].GetActivePayload() != 0 )
        {
            tNetAddr.wPort = htons(g_cMpwApp.m_tCfg.wRecvStartPort + PORTSPAN * byChnNo );
            tNetRtcpAddr.wPort = htons(GetSendRtcpPort(byChnNo)); ;  // Rtcp ������Dri��
            nRet = m_cHardMulPic.AddChannel(byChnNo, tNetAddr, tNetRtcpAddr); // tNetAddr�����ؽ��ܵ�ַ��tNetRtcpAddr������Rtcp���ĵ�ַ      
            if (HARD_MULPIC_OK != nRet)
            {
                byErrNum++;
                mpwlog("Fail to add channel: %u!\n", byChnNo);
            }
            else
            {
                m_bAddVmpChannel[byChnNo] = TRUE;
                mpwlog("Success to add channel: %u!\n", byChnNo);
            }
       
        }
    }

    NEXTSTATE(RUNNING);
    return (0 == byErrNum);
}

/*=============================================================================
  �� �� ���� SetEncryptParam
  ��    �ܣ� ���ñ�������еļ��ܲ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChnNo
             TMediaEncrypt *  ptVideoEncrypt
             u8 byDoublePayload
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::SetEncryptParam(u8 byChnNo, TMediaEncrypt*  ptVideoEncrypt, TDoublePayload* ptDoublePayload)
{
    u16 wRet;
    u8  abyKeyBuf[MAXLEN_KEY];
    s32 wKeySize;
    u8  byEncryptMode;
    u8  byPayLoadValue = 0;
    
    //����-��Ƶ
    byEncryptMode = ptVideoEncrypt->GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
        // zbq [10/13/2007] FEC ֧�֣�������0��FECý�����;���
        /*if ( MEDIA_TYPE_H264 == ptDoublePayload->GetRealPayLoad() )
        {
            byPayLoadValue = MEDIA_TYPE_H264;
        }
        else if ( MEDIA_TYPE_H263PLUS == ptDoublePayload->GetRealPayLoad() )
        {
            byPayLoadValue = MEDIA_TYPE_H263PLUS;
        }
        else
        {
            byPayLoadValue = 0;
        }
        wRet = m_cHardMulPic.SetVideoActivePT( byChnNo, byPayLoadValue, byPayLoadValue );
        if ( CODEC_NO_ERROR != wRet )
        {
            mpwlog( "Grp.%d call \"SetVideoActivePT\" failed.\n", GetInsID());
            return;
        }*/
        wRet = m_cHardMulPic.SetVidEncryptPT( byChnNo, 0 );
        if ( CODEC_NO_ERROR != wRet )
        {
            mpwlog( "SetVidEncryptPT failed err=%d!\n", wRet );
            return;
        }
        wRet = m_cHardMulPic.SetVidEncryptKey( byChnNo, (s8*)NULL, (u16)0, byEncryptMode);
        if ( CODEC_NO_ERROR != wRet )
        {
            mpwlog( "SetVidEncryptKey failed err=%d!\n", wRet );
            return;
        }
    }
    else if ( CONF_ENCRYPTMODE_DES == byEncryptMode || 
              CONF_ENCRYPTMODE_AES == byEncryptMode )
    {        
        //�ϲ����²�ı����ģʽ��һ��
        if ( CONF_ENCRYPTMODE_DES == byEncryptMode )
        {
            byEncryptMode = DES_ENCRYPT_MODE;
        }
        else if ( CONF_ENCRYPTMODE_AES == byEncryptMode )
        {
            byEncryptMode = AES_ENCRYPT_MODE;
        }
        /*
        wRet = m_cHardMulPic.SetVideoActivePT( byChnNo, 
                                               ptDoublePayload->GetActivePayload(),
                                               ptDoublePayload->GetRealPayLoad() );
        if ( CODEC_NO_ERROR != wRet )
        {
            mpwlog( "Grp.%d call \"SetVideoActivePT\" failed.\n", GetInsID() );
            return;
        }*/
        wRet = m_cHardMulPic.SetVidEncryptPT( byChnNo, ptDoublePayload->GetActivePayload() );
        if ( CODEC_NO_ERROR != wRet )
        {
            mpwlog( "SetVidEncryptPT failed err=%d!\n", wRet);
            return;
        }

        ptVideoEncrypt->GetEncryptKey( abyKeyBuf, &wKeySize );
        
        wRet = m_cHardMulPic.SetVidEncryptKey( byChnNo, (s8*)abyKeyBuf, (u16)wKeySize, byEncryptMode );
        if ( CODEC_NO_ERROR != wRet )
        {
            mpwlog( "SetVidEncryptKey failed err=%d!\n", wRet);
            return;
        }
    }
    return;
}

/*=============================================================================
  �� �� ���� SetDecryptParam
  ��    �ܣ� ���ñ�������еĽ��ܲ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChnNo
             TMediaEncrypt *  ptVideoEncrypt
             TDoublePayload * ptDoublePayload
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::SetDecryptParam(u8 byChnNo, TMediaEncrypt*  ptVideoEncrypt, TDoublePayload* ptDoublePayload)
{
    u16 wRet;
    u8  byDecryptMode = 0;
    s32 nKenLen = 0;
    u8  abyKeyBuf[MAXLEN_KEY];
    memset(abyKeyBuf, '\0', sizeof(abyKeyBuf));
    u8 byPayLoadValue = 0;  
    
    byDecryptMode = ptVideoEncrypt->GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byDecryptMode ) 
    {
        // zbq [10/13/2007] FEC֧��
        u8 byPayLoadValue = 0;
        if (MEDIA_TYPE_FEC == ptDoublePayload->GetActivePayload())
        {
            byPayLoadValue = MEDIA_TYPE_FEC;
        }
        else if (MEDIA_TYPE_H264 == ptDoublePayload->GetRealPayLoad())
        {
            byPayLoadValue = MEDIA_TYPE_H264;
        }
        else if (MEDIA_TYPE_H263PLUS == ptDoublePayload->GetRealPayLoad())
        {
            byPayLoadValue = MEDIA_TYPE_H263PLUS;
        }

        wRet = m_cHardMulPic.SetVideoActivePT( byChnNo, byPayLoadValue, byPayLoadValue );
        if ( CODEC_NO_ERROR != wRet )
        {
            mpwlog( "Grp.%d call \"SetVideoActivePT\" failed.\n", GetInsID());
            return;
        }        
        wRet = m_cHardMulPic.SetVidDecryptKey( byChnNo, (s8*)NULL, (u16)0, byDecryptMode);
        if ( CODEC_NO_ERROR != wRet )
        {
            mpwlog( "SetVidDecryptKey failed err=%d!\n", wRet );
            return;
        }
    }
    else if ( CONF_ENCRYPTMODE_AES == byDecryptMode ||
              CONF_ENCRYPTMODE_DES == byDecryptMode )
    {
        //�ϲ����²�ı����ģʽ��һ��
        if ( CONF_ENCRYPTMODE_DES == byDecryptMode )
        {
            byDecryptMode = DES_ENCRYPT_MODE;
        }
        else if ( CONF_ENCRYPTMODE_AES == byDecryptMode )
        {
            byDecryptMode = AES_ENCRYPT_MODE;
        }

        // zbq [10/13/2007] FEC֧��
        u8 byRealPayload = ptDoublePayload->GetRealPayLoad();
        u8 byActivePayload = ptDoublePayload->GetActivePayload();
        if ( MEDIA_TYPE_FEC == byActivePayload )
        {
            byRealPayload = MEDIA_TYPE_FEC;
        }
        wRet = m_cHardMulPic.SetVideoActivePT( byChnNo, byActivePayload,
                                                        byRealPayload );
        if ( CODEC_NO_ERROR != wRet )
        {
            mpwlog( "Grp.%d call \"SetVideoActivePT\" failed.\n", GetInsID() );
            return;
        }
        ptVideoEncrypt->GetEncryptKey( abyKeyBuf, &nKenLen );
        
        wRet = m_cHardMulPic.SetVidDecryptKey( byChnNo, (s8*)abyKeyBuf, (u16)nKenLen, byDecryptMode );
        if ( CODEC_NO_ERROR != wRet )
        {
            mpwlog( "SetVidDecryptKey failed err=%d!\n", wRet);
            return;
        }
    }
    return;
}

/*=============================================================================
  �� �� ���� SendMsgToMcu
  ��    �ܣ� ����Ϣ��Mcu
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16  wEvent
             CServMsg* const pcServMsg
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMpwInst::SendMsgToMcu(u16  wEvent, CServMsg* const pcServMsg)
{
  
    if (g_cMpwApp.m_bEmbed || OspIsValidTcpNode(g_cMpwApp.m_dwMcuNode))
    {
        post(g_cMpwApp.m_dwMcuIId, wEvent, pcServMsg->GetServMsg(), pcServMsg->GetServMsgLen(), g_cMpwApp.m_dwMcuNode);
    }

    if( g_cMpwApp.m_bEmbedB || OspIsValidTcpNode(g_cMpwApp.m_dwMcuNodeB))
    {
        post(g_cMpwApp.m_dwMcuIIdB, wEvent, pcServMsg->GetServMsg(), pcServMsg->GetServMsgLen(), g_cMpwApp.m_dwMcuNodeB);
    }
    return TRUE;
}
/*=============================================================================
  �� �� ���� GetMpwNumOfStyle
  ��    �ܣ� ȡ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMpwStyle
  �� �� ֵ�� u8 
=============================================================================*/
u8 CMpwInst::GetMpwNumOfStyle(u8 byMpwStyle)
{
	u8   byMaxMemNum = 0;
	switch( byMpwStyle ) 
	{
	case VMP_STYLE_ONE:
		byMaxMemNum = 1;
		break;
        
	case VMP_STYLE_VTWO:
	case VMP_STYLE_HTWO:
		byMaxMemNum = 2;
		break;

	case VMP_STYLE_THREE:
		byMaxMemNum = 3;
		break;

	case VMP_STYLE_FOUR:
		byMaxMemNum = 4;
		break;

	case VMP_STYLE_SIX:
		byMaxMemNum = 6;
		break;

	case VMP_STYLE_EIGHT:
		byMaxMemNum = 8;
		break;

	case VMP_STYLE_NINE:
		byMaxMemNum = 9;
		break;

	case VMP_STYLE_TEN:
		byMaxMemNum = 10;
		break;

	case VMP_STYLE_THIRTEEN:
		byMaxMemNum = 13;
		break;

	case VMP_STYLE_SIXTEEN:
		byMaxMemNum = 16;
		break;

	case VMP_STYLE_SPECFOUR:
		 byMaxMemNum = 4;
		break;

	case VMP_STYLE_SEVEN:
		 byMaxMemNum = 7;
		 break;
	}
	return byMaxMemNum;
}

/*=============================================================================
  �� �� ���� GetActivePayload
  ��    �ܣ� ȡ��̬�غ�ֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byRealPayloadType
  �� �� ֵ�� u8  
=============================================================================*/
u8  CMpwInst::GetActivePayload(u8 byRealPayloadType)
{
    u8 byAPayload;
	switch(byRealPayloadType)
	{
	case MEDIA_TYPE_PCMA: 
        byAPayload = ACTIVE_TYPE_PCMA;
        break;

	case MEDIA_TYPE_PCMU: 
        byAPayload = ACTIVE_TYPE_PCMU;
        break;

	case MEDIA_TYPE_G721: 
        byAPayload = ACTIVE_TYPE_G721;
        break;

	case MEDIA_TYPE_G722: 
        byAPayload = ACTIVE_TYPE_G722;
        break;

	case MEDIA_TYPE_G7231: 
        byAPayload = ACTIVE_TYPE_G7231;
        break;

	case MEDIA_TYPE_G728:  
        byAPayload = ACTIVE_TYPE_G728;
        break;

	case MEDIA_TYPE_G729:  
        byAPayload = ACTIVE_TYPE_G729;
        break;

//	case MEDIA_TYPE_G7221C: 
//        byAPayload = ACTIVE_TYPE_G7221;
//        break;

	case MEDIA_TYPE_H261:  
        byAPayload = ACTIVE_TYPE_H261;
        break;

	case MEDIA_TYPE_H262:  
        byAPayload = ACTIVE_TYPE_H262;
        break;

	case MEDIA_TYPE_H263:  
        byAPayload = ACTIVE_TYPE_H263;
        break;

	default:
        byAPayload = byRealPayloadType;
        break;
	}
	return byAPayload;
}

/*=============================================================================
  �� �� ���� GetFavirateStyle
  ��    �ܣ� ��Map���ĵ�ͨ���������غϳɷ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwTotalmap
             u8& byChannels
  �� �� ֵ�� u8 
=============================================================================*/
u8 CMpwInst::GetFavirateStyle(u32 dwTotalmap, u8& byChannels)
{
    u8 byType = 0;
    switch(dwTotalmap)
    {
    case 1:
    case 2:
    case 3:
        byType = 3;
        byChannels = 4;
        break;
    case 4:
        byChannels = 9;
        byType = 10;
        break;
    case 5:
        byChannels = 16;
        byType = 13;
        break;
    default:
        byChannels = 4;
        byType = 3;
        break;
    }
    return byType;
}

/*=============================================================================
  �� �� ���� ConvertVc2Hard
  ��    �ܣ� ��������ת��Ϊ�ײ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byOldstyle:��Ӧ����ϵĻ�����
  �� �� ֵ�� u8���ϳ�ʱ�Ļ�����
=============================================================================*/
u8 CMpwInst::ConvertVc2Hard(u8 byOldstyle)
{
    u8 byStyle;
    switch ( byOldstyle )
    {
    case VMP_STYLE_ONE:  // һ����
        byStyle = MULPIC_TYPE_ONE;  // ������
        break;

    case VMP_STYLE_VTWO: // �����棺���ҷ�
        byStyle = MULPIC_TYPE_VTWO;  // �����棺���ҷ�
        break;

    case VMP_STYLE_HTWO:  // ������: һ��һС
        byStyle = MULPIC_TYPE_ITWO; // �����棺 һ��һС������ֳ�9�ݷ��ͣ�
        break;

    case VMP_STYLE_THREE: // ������
        byStyle = MULPIC_TYPE_THREE; // ������
        break;

    case VMP_STYLE_FOUR: // �Ļ���
        byStyle = MULPIC_TYPE_FOUR;  // �Ļ���
        break;

    case VMP_STYLE_SIX: //������
        byStyle = MULPIC_TYPE_SIX; //������
        break;

    case VMP_STYLE_EIGHT: //�˻���
        byStyle = MULPIC_TYPE_EIGHT; //�˻���
        break;
    case VMP_STYLE_NINE: //�Ż���
        byStyle = MULPIC_TYPE_NINE; //�Ż���
        break;

    case VMP_STYLE_TEN: //ʮ����
        byStyle = MULPIC_TYPE_TEN; //ʮ���棺�����С������ֳ�4�ݷ��ͣ�
        break;

    case VMP_STYLE_THIRTEEN:  //ʮ������
        byStyle = MULPIC_TYPE_THIRTEEN; //ʮ������
        break;

    case VMP_STYLE_SIXTEEN: //ʮ������
        byStyle = MULPIC_TYPE_SIXTEEN; //ʮ������
        break;
        
    case VMP_STYLE_SPECFOUR://�����Ļ��� 
        byStyle = MULPIC_TYPE_SFOUR; //�����Ļ��棺һ����С������ֳ�4�ݷ��ͣ�
        break;

    case VMP_STYLE_SEVEN: //�߻���
        byStyle = MULPIC_TYPE_SEVEN; //�߻��棺������С������ֳ�4�ݷ��ͣ�
        break;

    default:
        byStyle = MULPIC_TYPE_VTWO;    // Ĭ�������棺���ҷ�
        break;
    }
    return byStyle;
}

void CMpwInst::mpwlog( s8* pszFmt, ...)
{
    s8 achPrintBuf[255];
    s32  nBufLen = 0;
    va_list argptr;
	if( g_nMpwlog == 1 )
	{		  
		nBufLen = sprintf( achPrintBuf, "[Mpw]: " ); 
		va_start( argptr, pszFmt );
		nBufLen += vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
		va_end(argptr); 
		OspPrintf( TRUE, FALSE, achPrintBuf ); 
	}
    return;
}


/*=============================================================================
  �� �� ���� ProcSetQosInfo
  ��    �ܣ� ����Qosֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::ProcSetQosInfo(CMessage* const pMsg)
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

    s32 nRet = SetMediaTOS((s32)byAudioValue, QOS_AUDIO);
    nRet = SetMediaTOS((s32)byVideoValue, QOS_VIDEO);
    nRet = SetMediaTOS((s32)byDataValue, QOS_DATA);

    return;
}

/*=============================================================================
  �� �� ���� ComplexQos
  ��    �ܣ� �󸴺�Qosֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8& byValue
             u8 byPrior
  �� �� ֵ�� void 
=============================================================================*/
void CMpwInst::ComplexQos(u8& byValue, u8 byPrior)
{
    u8 byLBit = ((byPrior & 0x08) >> 3);
    u8 byRBit = ((byPrior & 0x04) >> 1);
    u8 byTBit = ((byPrior & 0x02)<<1);
    u8 byDBit = ((byPrior & 0x01)<<3);
    
    byValue = byValue + ((byDBit + byTBit + byRBit + byLBit)*2);
    return;
}


/*=============================================================================
�� �� ���� GetSendRtcpPort
��    �ܣ� ��ȡͨ��rtcp�˿ڣ�������
�㷨ʵ�֣� ����ϳɽ�������ͨ����mp��RTCP�˿ڷ���(����ԭ��2...9, ��Ϊ0��1ΪMP���Ͷ˿�)
ȫ�ֱ����� 
��    ���� u8 byChanNo
�� �� ֵ�� u16 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/6/3  4.0			������                  ����
=============================================================================*/
u16 CMpwInst::GetSendRtcpPort(u8 byChanNo)
{
    if (byChanNo >= 8)
    {
        return (m_tRtcpAddr.wPort + 4 + byChanNo); 
    }
    else
    {
        return (m_tRtcpAddr.wPort + 2 + byChanNo); 
    }      
}

CMpwConfig::CMpwConfig()
{
    m_bEmbed  = FALSE;
    m_bEmbedB = FALSE;
    FreeStatusDataA();
    FreeStatusDataB();
    m_byRegAckNum  = 0;
	m_dwMpcSSrc    = 0;
    m_byChannelnum = 0;
    memset( &m_tCfg, 0, sizeof(CMpwCfg) );
}

CMpwConfig::~CMpwConfig()
{
}

/*=============================================================================
  �� �� ���� FreeStatusDataA
  ��    �ܣ� ���A��״̬��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMpwConfig::FreeStatusDataA()
{
    m_dwMcuIId  = INVALID_INS;
    m_dwMcuNode = INVALID_NODE;
    return;
}

/*=============================================================================
  �� �� ���� FreeStatusDataB
  ��    �ܣ�  ���B��״̬��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMpwConfig::FreeStatusDataB()
{
    m_dwMcuIIdB  = INVALID_INS;
    m_dwMcuNodeB = INVALID_NODE;
    return;
}
