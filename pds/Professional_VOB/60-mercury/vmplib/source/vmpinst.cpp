/*****************************************************************************
   ģ����      : ���渴����(Video Multiplexer)ʵ��
   �ļ���      : vmpInst.cpp
   ����ʱ��    : 2003�� 12�� 4��
   ʵ�ֹ���    : include : /include/VMPInst.h
   ����        : zhangsh
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   03/12/4	   v1.0	      zhangsh	    create
   05/02/27    3.6        libo          �����޸�
   2006/03/21  4.0        �ű���        �����Ż�
******************************************************************************/
#include "vmpinst.h"
#include "eqpcfg.h"
#include "boardagent.h"
//#include "codeclib.h"
#include "mcuver.h"

s32 g_nVmplog = 0;
/*lint -save -esym(530, argptr)*/
void vmplog( s8* pszFmt, ...)
{
    s8 achPrintBuf[255];
    s32  nBufLen = 0;
    va_list argptr;
	if( g_nVmplog == 1 )
	{		  
		nBufLen = sprintf( achPrintBuf, "[VMP]: " ); 
		va_start( argptr, pszFmt );
		nBufLen += vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
		va_end(argptr); 
		OspPrintf( TRUE, FALSE, achPrintBuf ); 
	}
    return;
}
/*lint -restore*/
CVMPInst::CVMPInst()
{
    m_bDbVid = FALSE;
    m_byMaxChannelNum    = 0;
    m_dwLastFUPTick = 0;
    memset( &m_tPrsTimeSpan, 0, sizeof(TPrsTimeSpan) );
    for (u8 byChnlNo = 0; byChnlNo < MAXNUM_SDVMP_MEMBER; byChnlNo++)
    {
        m_bAddVmpChannel[byChnlNo] = FALSE;
    }
}
/*lint -save -e1551*/
CVMPInst::~CVMPInst()
{
    m_cHardMulPic.Quit();
}
/*lint -restore*/
/*====================================================================
	����  : InstanceEntry
	����  : ��Ϣ�ַ�
	����  : CMessage
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void CVMPInst::InstanceEntry(CMessage *const pMsg)
{
    if (NULL == pMsg)
    {
        return;
    }
    CServMsg cServMsg(pMsg->content, pMsg->length);

    if(EV_VMP_NEEDIFRAME_TIMER != pMsg->event )
    {
        vmplog("Message %u(%s) received in InstanceEntry\n", pMsg->event, ::OspEventDesc(pMsg->event));
    }
    switch(pMsg->event)
    {
    //--------�ڲ���Ϣ------------
    // ��������
    case OSP_DISCONNECT:
        Disconnect(pMsg);
        break;
	// ����
    case EV_VMP_CONNECT_TIMER:
        ProcConnectTimeOut(TRUE);
        break;

    case EV_VMP_CONNECT_TIMERB:
        ProcConnectTimeOut(FALSE);
        break;
	// ע��
    case EV_VMP_REGISTER_TIMER:
        ProcRegisterTimeOut(TRUE);
        break;

    case EV_VMP_REGISTER_TIMERB:
        ProcRegisterTimeOut(FALSE);
        break;
	
    //--------�ⲿ��Ϣ------------
    // ������ʼ��
    case EV_VMP_INIT:
        Init(pMsg);
        break;

	//MCU ֪ͨ�Ự ����MPCB ��Ӧʵ��, zgc, 2007/04/30
	case MCU_EQP_RECONNECTB_CMD:
		ProcReconnectBCmd( pMsg );
		break;

    // ��ʾ���е�״̬��ͳ����Ϣ
    case EV_VMP_DISPLAYALL:
        StatusShow();
        break;

	case EV_VMP_SHOWPARAM:
		ParamShow();
		break;

    // ά����Ϣ
    case MCU_EQP_ROUNDTRIP_REQ:
        SendMsgToMcu(EQP_MCU_ROUNDTRIP_ACK, &cServMsg);
        break;

    // ע��Ӧ����Ϣ
    case MCU_VMP_REGISTER_ACK:
        MsgRegAckProc(pMsg);
        break;

    // ע��ܾ���Ϣ
    case MCU_VMP_REGISTER_NACK:
        MsgRegNackProc(pMsg);
        break;

    // ��ʼ����
    case MCU_VMP_STARTVIDMIX_REQ:
        MsgStartVidMixProc(pMsg);
        break;

    // ֹͣ����
    case MCU_VMP_STOPVIDMIX_REQ:
        MsgStopVidMixProc(pMsg);
        break;

    // �����ı�
    case MCU_VMP_CHANGEVIDMIXPARAM_CMD:
        MsgChangeVidMixParamProc(pMsg);
        break;

    // ��ѯ����
    case MCU_VMP_GETVIDMIXPARAM_REQ:
        MsgGetVidMixParamProc(pMsg);
        break;

    // ����ͨ������
    case MCU_VMP_SETCHANNELBITRATE_REQ:
        MsgSetBitRate(pMsg);
        break;

    // ����Qosֵ
    case MCU_EQP_SETQOS_CMD:
        ProcSetQosInfo(pMsg);
        break;

    // ���ı������
    case MCU_VMP_UPDATAVMPENCRYPTPARAM_REQ:
        MsgUpdataVmpEncryptParamProc(pMsg);
        break;

    // ǿ�ƹؼ�֡
    case MCU_VMP_FASTUPDATEPIC_CMD:
        MsgFastUpdatePicProc();
        break;

    // �޸ĸ��ϸ������(�߿򡢱���)
    case MCU_VMP_SETSTYLE_CMD:
        ProcSetStyleMsg(pMsg);
        break;

	// ֹͣMAP�Ĺ���, zgc, 2007/04/24
	case MCU_VMP_STOPMAP_CMD:
		ProcStopMapCmd(pMsg);
		break;

	// ����Ƿ���Ҫ�ؼ�֡
    case EV_VMP_NEEDIFRAME_TIMER:
        MsgTimerNeedIFrameProc();
        break;

	// ȡ��������״̬
	case TIME_GET_MSSTATUS:
	case MCU_EQP_GETMSSTATUS_ACK:
		ProcGetMsStatusRsp(pMsg);
		break;
        
    case EV_VMP_BACKBOARD_OUT_CMD:
        ProcSetBackBoardOutCmd(pMsg);
        break;

    // δ�������Ϣ
    default:
        log(LOGLVL_EXCEPTION, "[Error]Unexcpet Message %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event));
        break;
    }
}

/*=============================================================================
  �� �� ���� GetFavirateStyle
  ��    �ܣ� ��Map���õ�ͨ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 totalmap
             u8 &channel
  �� �� ֵ�� u8 
=============================================================================*/
u8 CVMPInst::GetFavirateStyle(u32 dwTotalmap, u8 &byChannel)
{
    u8 byType;

    switch(dwTotalmap) // Map��Ŀ
    {
    case 1:
    case 2:
    case 3:
        byChannel = 4; // ���ͨ����
        byType = MULPIC_TYPE_FOUR;
        break;
    case 4:
        byChannel = 9;
        byType = MULPIC_TYPE_NINE;
        break;
    case 5:
        byChannel = 16;
        byType = MULPIC_TYPE_SIXTEEN;
        break;
    default:
        byChannel = 4;
        byType = MULPIC_TYPE_FOUR;
        break;
    }

    return byType;
}

/*=============================================================================
  �� �� ���� ConvertVcStyle2HardStyle
  ��    �ܣ� �ɻ�ط��õ��ײ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 oldstyle
  �� �� ֵ�� u8 
=============================================================================*/
u8 CVMPInst::ConvertVcStyle2HardStyle(u8 byOldstyle)
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
		{
			if( TRUE == m_bDbVid)
			{
				byStyle = MULPIC_TYPE_VTWO;
				OspPrintf( TRUE, FALSE, "Vmp has Double Enc, Change VmpStyle to MULPIC_TYPE_VTWO !\n" );
			}
			else
			{
				byStyle = MULPIC_TYPE_ITWO; // �����棺 һ��һС������ֳ�9�ݷ��ͣ�
			}
		}
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

/*=============================================================================
  �� �� ���� GetVmpNumOfStyle
  ��    �ܣ� �ɷ��õ���Ա��Ŀ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byVMPStyle
  �� �� ֵ�� u8 
=============================================================================*/
u8 CVMPInst::GetVmpNumOfStyle(u8 byVMPStyle)
{
	u8   byMaxMemNum = 0;

	switch( byVMPStyle ) 
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
	default:
		byMaxMemNum = 0;
		break;
	}

	return byMaxMemNum;
}

/*====================================================================
	����  : Init
	����  : ��MCUע��
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void CVMPInst::Init(CMessage *const pMsg)
{
    //����Ϣ�еõ���Ϣ
    if (NULL == pMsg->content)
    {
        OspPrintf(TRUE, FALSE, "[Error]Init params cannot be NULL!\n");
        return;
    }

    
    TVmpCfg tVmpCfg;
    memset(&tVmpCfg, 0, sizeof(TVmpCfg));

    memcpy(&tVmpCfg, pMsg->content, sizeof(TVmpCfg));
	
    if ( 2 == tVmpCfg.byDbVid ) 
    {
        m_bDbVid = TRUE;// ����·����
    }
    else
    {
        m_bDbVid = FALSE;
    }

	u32 dwTotalMapNum;
    if ( m_bDbVid )
    {
        dwTotalMapNum = tVmpCfg.wMAPCount - 1; // Ԥ��һ��Map����
    }
    else
    {
        dwTotalMapNum = tVmpCfg.wMAPCount;
    }

    m_tCfg.m_tMulPicParam.byType = GetFavirateStyle(dwTotalMapNum, m_byMaxChannelNum);

    m_tCfg.m_tMulPicParam.dwTotalMapNum = dwTotalMapNum;
    m_tCfg.m_tMulPicParam.dwCoreSpd     = tVmpCfg.m_atMap[0].dwCoreSpd;
    m_tCfg.m_tMulPicParam.dwMemSpd      = tVmpCfg.m_atMap[0].dwMemSpd;
    m_tCfg.m_tMulPicParam.dwMemSize     = tVmpCfg.m_atMap[0].dwMemSize;
	// Mcu��Ϣ
    m_tCfg.dwMcuIP        = tVmpCfg.dwConnectIP;
    m_tCfg.wMcuPort       = tVmpCfg.wConnectPort;
    m_tCfg.byEqpType      = tVmpCfg.byEqpType;
    m_tCfg.byEqpId        = tVmpCfg.byEqpId;
    m_tCfg.dwLocalIp      = tVmpCfg.dwLocalIP;
    m_tCfg.wRecvStartPort = tVmpCfg.wRcvStartPort;
    m_tCfg.byMcuId        = (u8)tVmpCfg.wMcuId;
    m_tCfg.m_dwMcuIpB     = tVmpCfg.dwConnectIpB;
    m_tCfg.m_wMcuPortB    = tVmpCfg.wConnectPortB;
    m_tCfg.m_dwMcuIdB     = (u32)tVmpCfg.wMcuId;
    m_tCfg.m_wRecvStartPortB = tVmpCfg.wRcvStartPortB;

    memcpy(m_tCfg.szAlias, tVmpCfg.achAlias, MAXLEN_ALIAS);
    m_tCfg.szAlias[MAXLEN_ALIAS] = '\0';

    // �����õ�����Ϣ
    if ( !m_tCfg.ParseParam() )
    {
        OspPrintf(TRUE, FALSE, "[Error]Read basic params(MUST) failed,some in brdcfg.ini at VMP,others at mcucfg.ini at MCU!\n");
        OspPrintf(TRUE, FALSE, " They come from mmpagent,you can type command MmpAgentSelfTest at VMP to check!\n");
        return;
    }

    // ��ʼ��
    m_tCfg.m_tStatus.m_byVMPStyle = m_tCfg.m_tMulPicParam.byType;
	
    s32 nRetval = m_cHardMulPic.Initialize(m_tCfg.m_tMulPicParam, m_bDbVid); // ��ʼ��Map
    if ( HARD_MULPIC_OK == nRetval )
    {
        vmplog("Init %s Success!\n", m_tCfg.szAlias);
        if (m_tCfg.dwLocalIp == m_tCfg.dwMcuIP)
        {
            g_cVMPApp.m_bEmbed = TRUE;
            g_cVMPApp.m_dwMcuNode = 0;
            SetTimer(EV_VMP_REGISTER_TIMER, VMP_REGISTER_TIMEOUT);
        }
        if(m_tCfg.dwLocalIp == m_tCfg.m_dwMcuIpB)
        {
            g_cVMPApp.m_bEmbedB = TRUE;
            g_cVMPApp.m_dwMcuNodeB = 0;
            SetTimer(EV_VMP_REGISTER_TIMERB, VMP_REGISTER_TIMEOUT);
        }
    }
    else
    {
        OspPrintf(TRUE, FALSE, "Fail to inital the Hard Multi-picture.(Init). errcode.%d\n", nRetval);
        return;
    }

    if(0 != m_tCfg.dwMcuIP && FALSE == g_cVMPApp.m_bEmbed) // ����Mcu
    {
        SetTimer(EV_VMP_CONNECT_TIMER, VMP_CONNETC_TIMEOUT);
    }

    if(0 != m_tCfg.m_dwMcuIpB && FALSE == g_cVMPApp.m_bEmbedB )
    {
        SetTimer(EV_VMP_CONNECT_TIMERB, VMP_CONNETC_TIMEOUT);
    }

    //�������
	//    m_cHardMulPic.SetBackBoardOut(TRUE);
	//  [1/11/2010 pengjie] Modify �������ͨ�������ļ�����
	BOOL32 bIsNeedBackBoardOut = m_tCfg.ReadIsNeedBackBrdOut();
	m_cHardMulPic.SetBackBoardOut( bIsNeedBackBoardOut );
	printf( "[Vpu_Cfg][Init] SetBackBoardOut: %d \n ", bIsNeedBackBoardOut);
	// End
   
	// ��ʼ����ͼ�ӿ�
	// m_cDrawInterface.Initalize();

	// ������Ϣ
	m_tCfg.PrintMulPicParam();
    // guzh [10/18/2007] 
    m_tCfg.ReadBitrateCheatValues();

    vmplog("\t Is Double Video(0 one video ,1 two video) :%u\n", m_bDbVid);

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
void CVMPInst::ProcConnectTimeOut(BOOL32 bIsConnectA)
{
    BOOL32 bRet = FALSE;
    if( bIsConnectA ) // ����
    {
        bRet = ConnectMcu(bIsConnectA, g_cVMPApp.m_dwMcuNode);
        if( bRet )
        { 
            SetTimer(EV_VMP_REGISTER_TIMER, VMP_REGISTER_TIMEOUT); 
        }
        else
        {
            SetTimer(EV_VMP_CONNECT_TIMER, VMP_CONNETC_TIMEOUT);
        }
    }
    else
    {
        bRet = ConnectMcu(bIsConnectA, g_cVMPApp.m_dwMcuNodeB);
        if( bRet )
        { 
            SetTimer(EV_VMP_REGISTER_TIMERB, VMP_REGISTER_TIMEOUT); 
        }
        else
        {
            SetTimer(EV_VMP_CONNECT_TIMERB, VMP_CONNETC_TIMEOUT);
        }
    }
    return;
}

/*=============================================================================
  �� �� ���� ConnectMcu
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� BOOL32 bIsConnectA
             u32& dwMcuNode
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CVMPInst::ConnectMcu(BOOL32 bIsConnectA, u32& dwMcuNode)
{
    BOOL32 bRet = TRUE;

    if( !OspIsValidTcpNode(dwMcuNode))
    {
        if( bIsConnectA )
        {  
            dwMcuNode = BrdGetDstMcuNode();  
        }
        else
        {  
            dwMcuNode = BrdGetDstMcuNodeB(); 
        }

        if (OspIsValidTcpNode(dwMcuNode))
        {
            ::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());
            vmplog("[Info] Connect to Mcu Success,node is %u!\n", dwMcuNode);
        }
        else
        {
            //����ʧ��
            OspPrintf(TRUE, FALSE, "[Error Vmp] Fail to Connect Mcu A\n");
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
void CVMPInst::ProcRegisterTimeOut(BOOL32 bIsRegisterA)
{
    if(TRUE == bIsRegisterA)
    {
        Register(bIsRegisterA, g_cVMPApp.m_dwMcuNode);
        SetTimer(EV_VMP_REGISTER_TIMER, VMP_REGISTER_TIMEOUT);
    }
    else
    {
        Register(bIsRegisterA, g_cVMPApp.m_dwMcuNodeB);
        SetTimer(EV_VMP_REGISTER_TIMERB, VMP_REGISTER_TIMEOUT);
    }
    return;
}

/*====================================================================
	������  ��Register
	����    ����MCUע�ắ��
	����    ����
	���    ����
	����ֵ  ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   03/12/4	   v1.0	      zhangsh	    create
====================================================================*/
void CVMPInst::Register(BOOL32 bIsRegisterA, u32 dwMcuNode)
{
    CServMsg cSvrMsg;
    TPeriEqpRegReq tReg;
    memset(&tReg, 0, sizeof(tReg));
    
    tReg.SetEqpId(m_tCfg.byEqpId);
    tReg.SetEqpType(m_tCfg.byEqpType);
    tReg.SetEqpAlias(m_tCfg.szAlias);
    tReg.SetPeriEqpIpAddr(m_tCfg.dwLocalIp);
    tReg.SetStartPort(m_tCfg.wRecvStartPort);
    tReg.SetChnnlNum(m_byMaxChannelNum);
    tReg.SetVersion(DEVVER_VMP);

    if(TRUE == bIsRegisterA)
    {
        tReg.SetMcuId(m_tCfg.byMcuId);
    }
    else
    {
        tReg.SetMcuId(m_tCfg.byMcuId);
    }
        
    cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
      
    post( MAKEIID( AID_MCU_PERIEQPSSN, m_tCfg.byEqpId ), VMP_MCU_REGISTER_REQ,
          cSvrMsg.GetServMsg(), cSvrMsg.GetServMsgLen(), dwMcuNode );

    return;
}

/*====================================================================
	������  ��MsgRegAckProc
	����    ��MCUע��Ӧ����Ϣ������
	����    ����Ϣ
	���    ����
	����ֵ  ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   03/12/4	   v1.0	      zhangsh	    create
====================================================================*/
void CVMPInst::MsgRegAckProc(CMessage* const pMsg)
{
    
    vmplog("[Info] Register OK!\n");

    if (NULL == pMsg->content)
    {
        OspPrintf(TRUE, FALSE, "[Vmp] The pointer cannot be Null. (MsgRegAckProc)\n");
        return;
    }

    CServMsg cServMsg(pMsg->content, pMsg->length);
    TPeriEqpRegAck tRegAck = *(TPeriEqpRegAck*)cServMsg.GetMsgBody();

    // ȡ����
    if( pMsg->srcnode == g_cVMPApp.m_dwMcuNode)
    {
        g_cVMPApp.m_dwMcuIId = pMsg->srcid; 
        g_cVMPApp.m_byRegAckNum++;
        KillTimer(EV_VMP_REGISTER_TIMER);
        vmplog("register to mcu.A succeed !\n");
    }
    else if(pMsg->srcnode == g_cVMPApp.m_dwMcuNodeB)
    {
        g_cVMPApp.m_dwMcuIIdB = pMsg->srcid;        
        g_cVMPApp.m_byRegAckNum++;
        KillTimer(EV_VMP_REGISTER_TIMERB);
        vmplog("register to mcu.B succeed !\n");
    }

	
	// guzh [6/12/2007] У��Ự����
    if ( g_cVMPApp.m_dwMpcSSrc == 0 )
    {
        g_cVMPApp.m_dwMpcSSrc = tRegAck.GetMSSsrc();
    }
    else
    {
        // �쳣������Ͽ������ڵ�
        if ( g_cVMPApp.m_dwMpcSSrc != tRegAck.GetMSSsrc() )
        {
            OspPrintf(TRUE, FALSE, "[MsgRegAckProc] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
                      g_cVMPApp.m_dwMpcSSrc, tRegAck.GetMSSsrc());
            if ( OspIsValidTcpNode(g_cVMPApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cVMPApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cVMPApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cVMPApp.m_dwMcuNodeB);
            }
			return;
        }
    }


    vmplog("[Info] Get Send IP and Port from MCU are Follow(Network):\n");
    vmplog("[Info] First Channel Send IP:0x%x\tPort:%u\n",  m_tSndAddr[0].dwIp, m_tSndAddr[0].wPort);
    vmplog("[Info] Second Channel Send IP:0x%x\tPort:%u\n", m_tSndAddr[1].dwIp, m_tSndAddr[1].wPort);


    if( FIRST_REGACK == g_cVMPApp.m_byRegAckNum )
    {
        // mcu��˿�
		m_tSndAddr[0].dwIp  = htonl(tRegAck.GetMcuIpAddr());  // Rtp(Dri ��)
        m_tSndAddr[0].wPort = htons(tRegAck.GetMcuStartPort());
        m_tSndAddr[1].dwIp  = htonl(tRegAck.GetMcuIpAddr());  // (Dri ��) ˫��ʱ��
        m_tSndAddr[1].wPort = htons(tRegAck.GetMcuStartPort()) + PORTSPAN;
		
		m_tPrsTimeSpan = *(TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck));
		// ���MTU�Ĵ�С, zgc, 2007-04-02
		m_wMTUSize = *(u16*)( cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan) );
		m_wMTUSize = ntohs( m_wMTUSize );
		vmplog( "[RegAck] The net MTU size is : %d\n", m_wMTUSize );
		//SetVideoSendPacketLen( (s32)m_wMTUSize );
		
        // xsl [8/15/2006] ����һ�����ֵ����ֹ�ײ��ڴ�����ʧ��
        if (m_tPrsTimeSpan.m_wRejectTimeSpan > DEF_LOSETIMESPAN_PRS)
        {
            m_tPrsTimeSpan.m_wRejectTimeSpan = DEF_LOSETIMESPAN_PRS;
        }
        NEXTSTATE(NORMAL);
        SetTimer(EV_VMP_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);
        vmplog("[Info] The Vmp state be setted Normal\n");
    }

    SendStatusChangeMsg(1, 0, 0/*always*/);     //Tell MCU My Current Status
    return ;
}

/*====================================================================
	������  ��MsgRegNackProc
	����    ��MCUע��ܾ���Ϣ������
	����    ����Ϣ
	���    ����
	����ֵ  ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    03/12/4	   v1.0	      zhangsh	    create
====================================================================*/
void CVMPInst::MsgRegNackProc(CMessage* const pMsg)
{
	if( NULL == pMsg )
	{
		OspPrintf(TRUE, FALSE, "[MsgRegNackProc] pMsg is null\n");
	}
	// ��ʱ������������
    OspPrintf(TRUE, FALSE, "[Info] Register to MCU be refused .retry...\n");
    return;
}

/*====================================================================
	������  ��SendMsgToMcu
	����    ����MCU������Ϣ
	����    ��wEvent - �¼�
			  
	���    ����
	����ֵ  ���ɹ����� TRUE�����򷵻�FALSE
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   03/12/4	   v1.0	      zhangsh	    create
====================================================================*/
BOOL CVMPInst::SendMsgToMcu(u16 wEvent, CServMsg* const pcServMsg)
{
    vmplog("[Send] Message %u(%s).\n", wEvent, ::OspEventDesc(wEvent));

    if(g_cVMPApp.m_bEmbed || OspIsValidTcpNode(g_cVMPApp.m_dwMcuNode))
    {
        post(g_cVMPApp.m_dwMcuIId, wEvent, pcServMsg->GetServMsg(), pcServMsg->GetServMsgLen(), g_cVMPApp.m_dwMcuNode);
    }

    if( g_cVMPApp.m_bEmbedB || OspIsValidTcpNode(g_cVMPApp.m_dwMcuNodeB))
    {
        post(g_cVMPApp.m_dwMcuIIdB, wEvent, pcServMsg->GetServMsg(), pcServMsg->GetServMsgLen(), g_cVMPApp.m_dwMcuNodeB);
    }

    return TRUE;
}

/*====================================================================
	������  ��Disconnect
	����    ����������
	����    ����
	���    ����
	����ֵ  ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   03/12/4	   v1.0	      zhangsh	    create
====================================================================*/
void CVMPInst::Disconnect(CMessage *const pMsg )
{
//    s32 nRet = 0;
    u32 dwDisNode = *(u32 * )pMsg->content;
    
    if (INVALID_NODE != dwDisNode)
    {
        OspDisconnectTcpNode(dwDisNode);
    }

    if( dwDisNode == g_cVMPApp.m_dwMcuNode )
    {
        g_cVMPApp.FreeStatusDataA();
        SetTimer(EV_VMP_CONNECT_TIMER, VMP_CONNETC_TIMEOUT);
        vmplog("[Vmp] Disconnect with A's Node: %d\n", g_cVMPApp.m_dwMcuNode);
    }
    else if (dwDisNode == g_cVMPApp.m_dwMcuNodeB)
    {
        g_cVMPApp.FreeStatusDataB();
        SetTimer(EV_VMP_CONNECT_TIMERB, VMP_CONNETC_TIMEOUT);
        vmplog("[Vmp] Disconnect with B's Node: %d\n", g_cVMPApp.m_dwMcuNodeB);
    }
	
	// ������һ����������Mcuȡ��������״̬���ж��Ƿ�ɹ�
	if (INVALID_NODE != g_cVMPApp.m_dwMcuNode || INVALID_NODE != g_cVMPApp.m_dwMcuNodeB)
	{
		if (OspIsValidTcpNode(g_cVMPApp.m_dwMcuNode))
		{
			// �Ƿ�����ʱһ���Mcu�ȶ����ٷ�
			post( g_cVMPApp.m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cVMPApp.m_dwMcuNode );     
			OspPrintf(TRUE, FALSE, "[MsgDisconnectProc] GetMsStatusReq. McuNode.A\n");
		}
		else if (OspIsValidTcpNode(g_cVMPApp.m_dwMcuNodeB))
		{
			post( g_cVMPApp.m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cVMPApp.m_dwMcuNodeB );        
			OspPrintf(TRUE, FALSE, "[MsgDisconnectProc] GetMsStatusReq. McuNode.B\n");
		}
		// �ȴ�ָ��ʱ��
		SetTimer(TIME_GET_MSSTATUS, WAITING_MSSTATUS_TIMEOUT);
		return;
	}
	
    if(INVALID_NODE == g_cVMPApp.m_dwMcuNode && INVALID_NODE == g_cVMPApp.m_dwMcuNodeB)
    {
		ClearCurrentInst();
    }
    return ;
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
void CVMPInst::ProcReconnectBCmd( CMessage * const pcMsg )
{
	if( pcMsg == NULL )
	{
		return;
	}
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u32 dwMpcBIp = *(u32*)cServMsg.GetMsgBody();
	dwMpcBIp = ntohl(dwMpcBIp);
	m_tCfg.m_dwMcuIpB = dwMpcBIp;
	
	// ���������, �ȶϿ�
	if( OspIsValidTcpNode( g_cVMPApp.m_dwMcuNodeB ) )
	{
		OspDisconnectTcpNode( g_cVMPApp.m_dwMcuNodeB );
	}

	g_cVMPApp.FreeStatusDataB();
    if(0 != m_tCfg.m_dwMcuIpB && FALSE == g_cVMPApp.m_bEmbedB )
    {
        SetTimer(EV_VMP_CONNECT_TIMERB, VMP_CONNETC_TIMEOUT);
    }
	else
	{
		SetTimer(EV_VMP_REGISTER_TIMERB, VMP_REGISTER_TIMEOUT);
	}
}
/*=============================================================================
  �� �� ���� ClearCurrentInst
  ��    �ܣ� ��յ�ǰʵ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CVMPInst::ClearCurrentInst(void)
{
	switch(CurState())//��ͨ��
    {
    case NORMAL:
    case RUNNING:
        {
            s32 nRet = m_cHardMulPic.StopMerge();
            KillTimer(EV_VMP_NEEDIFRAME_TIMER);
            vmplog("[VMP Stop]:Stop work, the return code: %d!\n", nRet);
        }
        break;
	default:
		break;
    }

    NEXTSTATE(IDLE);
    g_cVMPApp.m_byRegAckNum = 0;

	g_cVMPApp.m_dwMpcSSrc = 0;
	
	memset( m_bAddVmpChannel, 0, sizeof(m_bAddVmpChannel) );

    vmplog("[Vmp] Disconnect with A and B board\n");
	return;
}

/*=============================================================================
  �� �� ���� ProcGetMsStatusRsp
  ��    �ܣ� ����ȡ��������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CVMPInst::ProcGetMsStatusRsp(CMessage* const pMsg)
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg(pMsg->content, pMsg->length);
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event ) // Mcu��Ӧ��Ϣ
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer(TIME_GET_MSSTATUS);
        vmplog("[DeamonProcGetMsStatus]. receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                  ptMsStatus->IsMsSwitchOK());

        if(ptMsStatus->IsMsSwitchOK()) // �����ɹ�
        {
            bSwitchOk = TRUE;
        }
	}
	// ����ʧ�ܻ��߳�ʱ
	if( !bSwitchOk )
	{
		ClearCurrentInst();
		if( INVALID_NODE == g_cVMPApp.m_dwMcuNode )
		{
			SetTimer(EV_VMP_CONNECT_TIMER, VMP_CONNETC_TIMEOUT);
		}
		if( INVALID_NODE == g_cVMPApp.m_dwMcuNodeB )
		{
			SetTimer(EV_VMP_CONNECT_TIMERB, VMP_CONNETC_TIMEOUT);
		}
	}
	return;
}

/*====================================================================
	������  :MsgStartVidMixProc
	����    :��ʼ���渴����Ϣ������
	����    :��Ϣ
	���    :��
	����ֵ  :��
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void CVMPInst::MsgStartVidMixProc(CMessage * const pMsg)
{
    if (NULL == pMsg->content)
    {
        OspPrintf(TRUE, FALSE, "[VMP] The pointer cannot be Null (MsgStartVidMixProc)\n");
        return;
    }
    
    CServMsg cServMsg(pMsg->content, pMsg->length);

    CKDVVMPParam tParam[2];    // ������Ƶͨ��
    memset(tParam, 0, 2 * sizeof(CKDVVMPParam));
    memcpy((u8*)tParam, cServMsg.GetMsgBody(), 2 * sizeof(CKDVVMPParam));

    u8 byNeedPrs = *(cServMsg.GetMsgBody() + 2 * sizeof(CKDVVMPParam));
    m_tStyleInfo = *(TVmpStyleCfgInfo*)(cServMsg.GetMsgBody() + 2 * sizeof(CKDVVMPParam) + sizeof(u8));

	TCapSupportEx tCapSupportEx = *(TCapSupportEx*)(cServMsg.GetMsgBody() + 2 * sizeof(CKDVVMPParam) + sizeof(u8) + sizeof(TVmpStyleCfgInfo));
    m_tCapSupportEx = tCapSupportEx;
	if( g_nVmplog == 1 )
	{
		tCapSupportEx.Print();
	}
    
    switch(CurState())
    {
    case NORMAL:
        break;

    default:
        OspPrintf(TRUE, FALSE, "[Error]Cannot start merge while in state:%d\n", CurState());
        return ;
    }

	// ��ӡ MCU ����Ĳ���, zgc, 2008-03-19
	vmplog("Follows are the params that mcu send to the vmp:\n\n");
	vmplog("CHNNL1: Type=%d BitRate=%u VideoHeight: %u VideoWidth: %u \n",
		tParam[0].m_byEncType, tParam[0].m_wBitRate, 
		tParam[0].m_wVideoHeight, tParam[0].m_wVideoWidth );
	vmplog("CHNNL2: Type=%d BitRate=%u VideoHeight: %u VideoWidth: %u \n",
		tParam[1].m_byEncType, tParam[1].m_wBitRate, 
		tParam[1].m_wVideoHeight, tParam[1].m_wVideoWidth );
	vmplog("/*---------------------------------------------------------*/\n\n");
	
	// ����Ϊ��ʱ��, �� ���õ�һ·��Ƶ������� ���Ƶ�����, zgc, 2008-03-19
    if ( 0 == tParam[0].m_wBitRate )
    {
        tParam[0].m_wBitRate = 1200;
    }
	
	// �Եڶ�·�������ʽ��б���, zgc, 2008-03-19
	if ( 0 == tParam[1].m_wBitRate )
	{
		tParam[1].m_wBitRate = MIN_BITRATE_OUTPUT;
	}
	
	// zgc, 2008-03-19, �������
    memcpy(m_tParam, tParam, 2 * sizeof(CKDVVMPParam));
    
    if( !SetVmpAttachStyle( m_tStyleInfo ) )
    {
        OspPrintf(TRUE, FALSE, "[Info] Fail to set vmp attach style(MsgStartVidMixProc)\n");
        cServMsg.SetMsgBody(NULL, 0);
        cServMsg.SetErrorCode(1);
        SendMsgToMcu(VMP_MCU_STARTVIDMIX_NACK, &cServMsg);
        return;
    }
	
	// zgc, 2008-03-19, ��������
	if ( m_tCfg.IsEnableBitrateCheat() )
    {        
        m_tParam[0].m_wBitRate = m_tCfg.GetDecRateDebug( m_tParam[0].m_wBitRate );
        vmplog("[MsgStartVidMixProc] New Enc Bitrate: %d\n", m_tParam[0].m_wBitRate);
		
        if ( m_bDbVid )
        {
            m_tParam[1].m_wBitRate = m_tCfg.GetDecRateDebug( m_tParam[1].m_wBitRate );
            vmplog("[MsgStartVidMixProc] New 2nd Enc Bitrate: %d\n", m_tParam[1].m_wBitRate);
        }
    }


    // ���õ�һ·��Ƶ����������п�ʼ
    m_tCfg.GetDefaultParam(m_tParam[0].m_byEncType, m_tCfg.m_tVideoEncParam[0]);
    m_tCfg.m_tVideoEncParam[0].m_wVideoHeight = m_tParam[0].m_wVideoHeight;
    m_tCfg.m_tVideoEncParam[0].m_wVideoWidth  = m_tParam[0].m_wVideoWidth;
    m_tCfg.m_tVideoEncParam[0].m_byEncType    = m_tParam[0].m_byEncType;

    m_tCfg.m_tVideoEncParam[0].m_wBitRate = m_tParam[0].m_wBitRate;
    if (m_tCfg.m_tVideoEncParam[0].m_dwSndNetBand == 0)
    {
		// ���ʹ�����Ȼʹ�ó�ʼ���ʽ�������, zgc, 2008-03-19
        m_tCfg.m_tVideoEncParam[0].m_dwSndNetBand = (u32)(tParam[0].m_wBitRate*2+100);
    }

    // guzh [4/9/2007] ֧����������
	// �����Ѿ����׹������ﲻ����Ҫ����, zgc, 2008-03-19
	/*
    if ( m_tCfg.IsEnableBitrateCheat() )
    {
        m_tCfg.m_tVideoEncParam[0].m_wBitRate = m_tCfg.GetDecRateDebug( m_tCfg.m_tVideoEncParam[0].m_wBitRate );
    }
    */

    if ( m_bDbVid )
    {
		// ���õڶ�·��Ƶ
		if ( MEDIA_TYPE_NULL == m_tParam[1].m_byEncType )
		{
			// Ĭ������, zgc, 2008-03-19
			m_tCfg.m_tVideoEncParam[1].m_byEncType = MEDIA_TYPE_MP4;
		}
		else
        {
			m_tCfg.m_tVideoEncParam[1].m_byEncType = m_tParam[1].m_byEncType;
		}

        // ���õڶ�·��Ƶ
        m_tCfg.GetDefaultParam(m_tParam[1].m_byEncType, m_tCfg.m_tVideoEncParam[1]);
        m_tCfg.m_tVideoEncParam[1].m_wVideoHeight = m_tParam[1].m_wVideoHeight;
        m_tCfg.m_tVideoEncParam[1].m_wVideoWidth  = m_tParam[1].m_wVideoWidth;
        //m_tCfg.m_tVideoEncParam[1].m_byEncType    = tParam[1].m_byEncType;
        m_tCfg.m_tVideoEncParam[1].m_wBitRate     = m_tParam[1].m_wBitRate;
        if ( 0 == m_tCfg.m_tVideoEncParam[1].m_dwSndNetBand )
        {
			// ���ʹ�����Ȼʹ�ó�ʼ���ʽ�������, zgc, 2008-03-19
            m_tCfg.m_tVideoEncParam[1].m_dwSndNetBand = (u32)(tParam[1].m_wBitRate*2+100);
        }
    }
    else
    {
        // �����һ·�򽫵ڶ�·���
        memset(&m_tCfg.m_tVideoEncParam[1], 0, sizeof(m_tCfg.m_tVideoEncParam[1]));
        memset(&m_tSndAddr[1], 0, sizeof(m_tSndAddr[1]));
    }

    // ��ӡ��ʼ�Ĳ���
    vmplog("Follows are the params we used to start the vmp:\n\n");
    vmplog( "/*----------  First video channel  ----------*/\n");
	m_tCfg.PrintEncParam( 0 );
	vmplog("\tSend IP: 0x%x and Port: %u\n", m_tSndAddr[0].dwIp, m_tSndAddr[0].wPort);
	
    vmplog("/*----------  Second video channel  ---------*/\n");
	m_tCfg.PrintEncParam( 1 );
    vmplog("\tSend IP: 0x%x and Port: %u\n", m_tSndAddr[1].dwIp, m_tSndAddr[1].wPort);
    vmplog("/*--------------------------------------------*/\n\n");

	// ���ת��
    u8 byStyle = ConvertVcStyle2HardStyle( tParam[0].m_byVMPStyle );
    vmplog("[Param]:Change VMP's style. (before style: %d, after style: %d)\n", tParam[0].m_byVMPStyle, byStyle);

//     if ( byStyle != m_tCfg.m_tStatus.m_byVMPStyle )
//     {
        m_cHardMulPic.SetMulPicType( byStyle ); // ���ķ��
        vmplog("[Info]now we change the vmp's style to user command\n");
//     }
    
    vmplog("[Info]m_tCfg.m_tStatus.m_byVMPStyle: %d\n", m_tCfg.m_tStatus.m_byVMPStyle);

    vmplog("[StartMix] PrsTimeSpan: Fst.%d, Snd.%d, Third.%d, Reject.%d\n",
            ntohs(m_tPrsTimeSpan.m_wFirstTimeSpan), ntohs(m_tPrsTimeSpan.m_wSecondTimeSpan),
            ntohs(m_tPrsTimeSpan.m_wThirdTimeSpan), ntohs(m_tPrsTimeSpan.m_wRejectTimeSpan) );
    

    TNetAddress tSndLocalAddr[2];
    tSndLocalAddr[0].dwIp = 0;
    tSndLocalAddr[0].wPort = m_tCfg.wRecvStartPort - PORTSPAN;
    tSndLocalAddr[1].dwIp = 0;
    tSndLocalAddr[1].wPort = m_tCfg.wRecvStartPort - 2*PORTSPAN;
    m_cHardMulPic.SetNetSndLocalAddr(tSndLocalAddr);
    
    //zbq[01/03/2008]���������������
    if ( m_tCfg.m_tVideoEncParam[0].m_byEncType == MEDIA_TYPE_H264 &&
         (m_tCfg.m_tVideoEncParam[0].m_wVideoHeight > 288 ||
          m_tCfg.m_tVideoEncParam[0].m_wVideoWidth  > 352 ))
    {
        vmplog("[StartVidMixProc] Video.0 format has been adjusted due to Height.%d, Width.%d\n",
                 m_tCfg.m_tVideoEncParam[0].m_wVideoHeight, m_tCfg.m_tVideoEncParam[0].m_wVideoWidth);
        m_tCfg.m_tVideoEncParam[0].m_wVideoHeight = 288;
        m_tCfg.m_tVideoEncParam[0].m_wVideoWidth  = 352;
    }
    if ( m_bDbVid &&
         m_tCfg.m_tVideoEncParam[1].m_byEncType == MEDIA_TYPE_H264 &&
         (m_tCfg.m_tVideoEncParam[1].m_wVideoHeight > 288 ||
          m_tCfg.m_tVideoEncParam[1].m_wVideoWidth  > 352 ))
    {
        vmplog("[StartVidMixProc] Video.1 format has been adjusted due to Height.%d, Width.%d\n",
                m_tCfg.m_tVideoEncParam[0].m_wVideoHeight, m_tCfg.m_tVideoEncParam[0].m_wVideoWidth);        
        m_tCfg.m_tVideoEncParam[1].m_wVideoHeight = 288;
        m_tCfg.m_tVideoEncParam[1].m_wVideoWidth  = 352;
    }

    s32 nRet = m_cHardMulPic.StartMerge( m_tCfg.m_tVideoEncParam, m_tSndAddr ); // ��ʼ�ϳ�
    if ( HARD_MULPIC_ERROR == nRet )
    {
        OspPrintf(TRUE, FALSE, "[Info]startmerge correct, Error code: %u\n", nRet);
        cServMsg.SetMsgBody(NULL, 0);
        cServMsg.SetErrorCode(1);
        SendMsgToMcu(VMP_MCU_STARTVIDMIX_NACK, &cServMsg);
        return;
    }
    
    // guzh [3/12/2007] ����ƽ������
	// ƽ������������Ȼʹ�ó�ʼ���ʽ�������, zgc, 2008-03-19
    SetSmoothSendRule( ntohl(m_tSndAddr[0].dwIp), ntohs(m_tSndAddr[0].wPort),
                       tParam[0].m_wBitRate );
    // ����ҵ���ж��Ƿ���Ҫ�ڶ�·ƽ��, zgc, 2008-03-19
	//if ( m_bDbVid )
	if ( m_bDbVid && ( MEDIA_TYPE_NULL != m_tParam[1].m_byEncType ) )
    {
        SetSmoothSendRule( ntohl(m_tSndAddr[1].dwIp), ntohs(m_tSndAddr[1].wPort),
                           tParam[1].m_wBitRate );
    }
	
	u8 byDisplayType = DISPLAY_TYPE_PAL;
	m_tCfg.GetDisplayType( byDisplayType );
	m_cHardMulPic.SetDisplayType( byDisplayType );
	
    SendMsgToMcu(VMP_MCU_STARTVIDMIX_ACK, &cServMsg);

    cServMsg.SetMsgBody(NULL, 0);

    if ( StartHardwareMix(tParam[0])) // ��Ӻϳɳ�Ա
    {
        cServMsg.SetErrorCode(0); //ok
    }
    else
    {
        cServMsg.SetErrorCode(1); //Error
    }

    SendMsgToMcu(VMP_MCU_STARTVIDMIX_NOTIF, &cServMsg);

    vmplog("[Info]Vmp now is work properly,video display on local or send to remote.This module state is %u\n", CurState());

    m_cConfId = cServMsg.GetConfId();

    SendStatusChangeMsg(1, 1, 0/*always*/); 

    //�趪���ش�����
    TNetRSParam tNetRSParam;
    u8 byMaxMemNum = GetVmpNumOfStyle(tParam[0].m_byVMPStyle);
    if( TRUE == byNeedPrs )
    {
        tNetRSParam.m_wFirstTimeSpan = m_tPrsTimeSpan.m_wFirstTimeSpan;
        tNetRSParam.m_wSecondTimeSpan = m_tPrsTimeSpan.m_wSecondTimeSpan;
        tNetRSParam.m_wThirdTimeSpan = m_tPrsTimeSpan.m_wThirdTimeSpan;
        tNetRSParam.m_wRejectTimeSpan = m_tPrsTimeSpan.m_wRejectTimeSpan;
        m_cHardMulPic.SetNetSendFeedbackVideoParam(2000, TRUE);
        m_cHardMulPic.SetNetRecvFeedbackVideoParam(tNetRSParam, TRUE, byMaxMemNum);
    }
    else
    {
        tNetRSParam.m_wFirstTimeSpan = 0;
        tNetRSParam.m_wSecondTimeSpan = 0;
        tNetRSParam.m_wThirdTimeSpan = 0;
        tNetRSParam.m_wRejectTimeSpan = 0;
        m_cHardMulPic.SetNetSendFeedbackVideoParam(2000, FALSE);
        m_cHardMulPic.SetNetRecvFeedbackVideoParam(tNetRSParam, FALSE, byMaxMemNum);
    }

    return;
}

/*=============================================================================
  �� �� ���� SetEncryptParam
  ��    �ܣ� ���ü��ܲ�������ʼ�ϳ�ʱ���ã�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChnNo        ��ͨ����
             TMediaEncrypt     �����ܲ���
             u8 byDoublePayload�������غ�
  �� �� ֵ�� void 
=============================================================================*/
void CVMPInst::SetEncryptParam(u8 byChnNo, TMediaEncrypt * ptVideoEncrypt, u8 byDoublePayload)
{
    if( NULL == ptVideoEncrypt )
	{
		return;
	}

    u8  abyKeyBuf[MAXLEN_KEY];
	memset( abyKeyBuf, 0, MAXLEN_KEY );

    s32 nKeyLen = 0;

    u8 byEncryptMode = ptVideoEncrypt->GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
        vmplog("[Info]we set key to NULL! (SetEncryptParam)\n");
        
        // zbq[10/18/2007] FEC ֧��
        u8 byPayLoadValue = 0;
        if ( MEDIA_TYPE_FEC == byDoublePayload )
        {
            byPayLoadValue = MEDIA_TYPE_FEC;
        }
        m_cHardMulPic.SetVidEncryptPT(byChnNo, byPayLoadValue);
        m_cHardMulPic.SetVidEncryptKey(byChnNo, NULL, 0, 0);
    }
    else  if (CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode) // ���������
    {
        if (CONF_ENCRYPTMODE_DES == byEncryptMode) // ���²�궨��ת��
        {
            byEncryptMode = DES_ENCRYPT_MODE;
        }
        else
        {
            byEncryptMode = AES_ENCRYPT_MODE;
        }

        ptVideoEncrypt->GetEncryptKey( abyKeyBuf, &nKeyLen );
        
        m_cHardMulPic.SetVidEncryptPT(byChnNo, byDoublePayload);
        m_cHardMulPic.SetVidEncryptKey(byChnNo, (s8*)abyKeyBuf, (u16)nKeyLen, byEncryptMode);

        vmplog("[Info]Mode: %u KeyLen: %u PT: %u \n",
                ptVideoEncrypt->GetEncryptMode(), 
                nKeyLen, 
                byDoublePayload);

        vmplog("[Info]Key is : ");
        for (u8 byKeyLoop = 0; byKeyLoop < nKeyLen; byKeyLoop++) // ������
        {
            vmplog("%x ", abyKeyBuf[byKeyLoop]);
        }
        vmplog("\n");
    }
    return;
}

/*=============================================================================
  �� �� ���� SetDecryptParam
  ��    �ܣ� ���ý������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChnNo
             TMediaEncrypt *  ptVideoEncrypt
             TDoublePayload * ptDoublePayload
  �� �� ֵ�� void 
=============================================================================*/
void CVMPInst::SetDecryptParam(u8 byChnNo, TMediaEncrypt *  ptVideoEncrypt, TDoublePayload * ptDoublePayload)
{
    u8 byEncryptMode;
    u8 abyKeyBuf[MAXLEN_KEY];
    s32 byKenLen = 0;
    memset(abyKeyBuf, 0, MAXLEN_KEY);

    byEncryptMode = ptVideoEncrypt->GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
        // zbq [10/13/2007] FEC֧��
        u8 byPayLoadValue;
        if ( MEDIA_TYPE_FEC  == ptDoublePayload->GetActivePayload() )
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
        else
        {
            byPayLoadValue = 0;
        }
        
        vmplog("[SetDecryptParam]Set key to NULL!\n");

        m_cHardMulPic.SetVideoActivePT(byChnNo, byPayLoadValue, byPayLoadValue);
        m_cHardMulPic.SetVidDecryptKey(byChnNo, NULL, 0, 0);

    }
    else  if (CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode)
    {
        if (CONF_ENCRYPTMODE_DES == byEncryptMode)
        {
            byEncryptMode = DES_ENCRYPT_MODE;
        }
        else
        {
            byEncryptMode = AES_ENCRYPT_MODE;
        }

        // zbq [10/13/2007] FEC֧��
        u8 byRealPayload = ptDoublePayload->GetRealPayLoad();
        u8 byActivePayload = ptDoublePayload->GetActivePayload();
        if ( MEDIA_TYPE_FEC == byActivePayload ) 
        {
            byRealPayload = MEDIA_TYPE_FEC;
        }
        ptVideoEncrypt->GetEncryptKey(abyKeyBuf, &byKenLen);

        m_cHardMulPic.SetVideoActivePT(byChnNo, byActivePayload, byRealPayload);
        m_cHardMulPic.SetVidDecryptKey(byChnNo, (s8*)abyKeyBuf, (u16)byKenLen, byEncryptMode);

        vmplog("[SetDecryptParam]Mode: %u KeyLen: %u PT: %u\n",
                ptVideoEncrypt->GetEncryptMode(), 
                byKenLen, 
                ptDoublePayload->GetRealPayLoad());

        vmplog("Key is : ");
        for (u8 byKeyLoop=0; byKeyLoop<byKenLen; byKeyLoop++)
        {
            vmplog("%x ", abyKeyBuf[byKeyLoop]);
        }
       vmplog("\n");
    }
    return;
}

/*====================================================================
	������  :StartHardwareMix
	����    :��ʼ���渴����������
	����    :��Ϣ
	���    :��
	����ֵ  :��
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	03/12/4		v1.0		zhangsh			create
====================================================================*/
BOOL CVMPInst::StartHardwareMix(CKDVVMPParam& tParam)
{
    s32 nRet = 0;
    u8  byErrNum = 0; 
    TNetAddress tNetAddr;
    TNetAddress tNetRtcpAddr;
    u8 byStyle;
    
    byStyle = ConvertVcStyle2HardStyle(tParam.m_byVMPStyle);

    m_tCfg.m_tStatus.m_byVMPStyle =  byStyle;
    m_tCfg.m_tStatus.m_byMemberNum = tParam.m_byMemberNum;
    
    tNetAddr.dwIp = 0;
    tNetRtcpAddr.dwIp = m_tSndAddr[0].dwIp;
    tParam.Print();

    vmplog("Now we will open %u channels!\n", tParam.m_byMemberNum);
    vmplog("[StartHardwareMix]m_tCfg.m_tStatus.m_byVMPStyle= %d\n", m_tCfg.m_tStatus.m_byVMPStyle);
    vmplog("tParam.m_byEncType = %d\n", tParam.m_byEncType ); // �ϲ��������
    vmplog("tParam.m_byEncType = %d\n", VmpGetActivePayload(tParam.m_byEncType));// ת����ı�������

    // zbq [10/15/2007] FECǰ����� ֧�֣�Ŀǰ������˫����
    u8 byFECType = m_tCapSupportEx.GetVideoFECType();
    BOOL32 bFECEnable = FECTYPE_NONE != byFECType ? TRUE : FALSE;
    
	// ���ñ������
    SetEncryptParam(0, &tParam.m_tVideoEncrypt[0], VmpGetActivePayload(tParam.m_byEncType));
    m_cHardMulPic.SetVidFecEnable(0, bFECEnable);
    
    if( m_bDbVid )
    {
        SetEncryptParam(1, &m_tParam[1].m_tVideoEncrypt[0], VmpGetActivePayload(m_tParam[1].m_byEncType)); // ���õڶ�·�������
        m_cHardMulPic.SetVidFecEnable(1, bFECEnable);
    }
    vmplog("[StartHardwareMix] bDbVid.%d: Fec Enable.%d, FecMode.%d\n", m_bDbVid, bFECEnable, byFECType);
    if ( m_bDbVid )
    {
        vmplog("[StartHardwareMix] bDbVid.%d: Fec Enable.%d, FecMode.%d\n", m_bDbVid, bFECEnable, byFECType);
    }

    for(u8 byChnNo = 0; byChnNo < tParam.m_byMemberNum; byChnNo++)
    {
        SetDecryptParam(byChnNo, &tParam.m_tVideoEncrypt[byChnNo], &tParam.m_tDoublePayload[byChnNo]);

        vmplog("RealPayLoad: %d, ActivePayload: %d\n",
                tParam.m_tDoublePayload[byChnNo].GetRealPayLoad(),
                tParam.m_tDoublePayload[byChnNo].GetActivePayload());

        if (tParam.m_tDoublePayload[byChnNo].GetRealPayLoad() != 0
			|| tParam.m_tDoublePayload[byChnNo].GetActivePayload() != 0) // �ж��غ�ֵ�Ƿ���Ч
        {
            tNetAddr.wPort = htons(m_tCfg.wRecvStartPort + PORTSPAN*byChnNo);            
            tNetRtcpAddr.wPort = htons(GetSendRtcpPort(byChnNo)); 
            nRet = m_cHardMulPic.AddChannel(byChnNo, tNetAddr, tNetRtcpAddr); // tNetAddr�����ؽ��ܵ�ַ��tNetRtcpAddr��Զ�˽���Rtcp���ĵ�ַ
            vmplog("[Param]:Add channel: %u!\n", byChnNo);
            if (HARD_MULPIC_OK != nRet )
            {
                byErrNum++;
            }
            else
            {
                m_bAddVmpChannel[byChnNo] = TRUE;
/*				
				s8 achDataBuf[MAX_BMPFILE_SIZE];
				memset(achDataBuf, 0, sizeof(achDataBuf));
				u32 dwDataLen = 0;
				TTopicParam tTopicParam;
				tTopicParam.SetFontType(m_tStyleInfo.GetFontType() );
				tTopicParam.SetFontSize(m_tStyleInfo.GetFontSize() );
				tTopicParam.SetHeight( wDEFAULT_BMP_HEIGHT );
				tTopicParam.SetWidth( ntohs(tParam.m_wVideoWidth) );
				tTopicParam.SetTextColor( m_tStyleInfo.GetTextColor() );
				tTopicParam.SetBkColor( m_tStyleInfo.GetTopicBkColor() );
				vmplog("Text color= %0x, Bk color= %0x.\n", m_tStyleInfo.GetTextColor(), m_tStyleInfo.GetTopicBkColor());
				tTopicParam.SetAlignment( MIDDLE_ALIGN );
				tTopicParam.SetText( tParam.m_atMtMember[byChnNo].GetMbAlias() );		
				vmplog(" channel=%d, alias= %s\n", byChnNo, tParam.m_atMtMember[byChnNo].GetMbAlias());

				m_cDrawInterface.DrawBmp(achDataBuf, dwDataLen, tTopicParam);
				TBackBGDColor tBackBGDColor;
				tBackBGDColor.RColor = GetRColor(m_tStyleInfo.GetTopicBkColor());
				tBackBGDColor.GColor = GetGColor(m_tStyleInfo.GetTopicBkColor());
				tBackBGDColor.BColor = GetBColor(m_tStyleInfo.GetTopicBkColor());

				nRet = m_cHardMulPic.StartAddIcon( byChnNo, (u8*)achDataBuf, dwDataLen, 30, 40, tTopicParam.GetWidth(),
					                         tTopicParam.GetHeight(), tBackBGDColor, m_tStyleInfo.GetDiaphaneity() );
*/
				vmplog(" channel = %d, Add Icon . return code = %d.\n", byChnNo, nRet);
            }

            vmplog("Add Channel correct is %u,now add channel-%u return value is %u,the ip is 0x%x ,port is %u\n",
                                       HARD_MULPIC_OK, byChnNo, nRet, tNetAddr.dwIp, tNetAddr.wPort);

        }
    }

    NEXTSTATE(RUNNING);
    return ( 0 == byErrNum );
}

/*====================================================================
	������  :MsgStopVidMixProc
	����    :ֹͣ������������
	����    :CMessage
	���    :
	����ֵ  :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	03/12/4	    v1.0		zhangsh			create
====================================================================*/
void CVMPInst::MsgStopVidMixProc(CMessage * const pMsg)
{
    // volatile int nCount = 0;
    CServMsg cServMsg(pMsg->content, pMsg->length);

    switch(CurState())
    {
    case RUNNING:
        break;
    default:
        OspPrintf( TRUE, FALSE, "[Vmp Error]Error State:%d\n", CurState());
        return;
    }

    cServMsg.SetMsgBody(NULL, 0);
    SendMsgToMcu(VMP_MCU_STOPVIDMIX_ACK, &cServMsg);
    memset(m_tParam, 0, 2 * sizeof(CKDVVMPParam));

    // ֹͣ����
    s32 nRetCode = m_cHardMulPic.StopMerge();

    vmplog("Call stopmerge() error code= %d (0 is ok)\n", nRetCode );
    if ( HARD_MULPIC_OK == nRetCode )
    {
        cServMsg.SetErrorCode(0);
    }
    else
    {
        cServMsg.SetErrorCode(1);
    }

    // guzh [3/12/2007] ֹͣƽ������
    ClearSmoothSendRule( ntohl(m_tSndAddr[0].dwIp), ntohs(m_tSndAddr[0].wPort) );
    if ( m_bDbVid )
    {
        ClearSmoothSendRule( ntohl(m_tSndAddr[1].dwIp), ntohs(m_tSndAddr[1].wPort) );
    }

	vmplog("we clear EncryptKey DecryptKey EncryptPT ActivePT Info success!\n");
    for(u8 byChnNo = 0; byChnNo < m_tCfg.m_tStatus.m_byMemberNum; byChnNo++)
    {
        m_bAddVmpChannel[byChnNo] = FALSE;
        m_cHardMulPic.SetVideoActivePT(byChnNo, 0, 0);
        m_cHardMulPic.SetVidDecryptKey(byChnNo, NULL, 0, 0);
        m_cHardMulPic.SetVidEncryptPT(byChnNo, 0);
        m_cHardMulPic.SetVidEncryptKey(byChnNo, NULL, 0, 0);
    }

    cServMsg.SetMsgBody(NULL, 0);
    SendMsgToMcu(VMP_MCU_STOPVIDMIX_NOTIF, &cServMsg);

    SendStatusChangeMsg(1, 0, 0/*always*/);

    // �趪���ش�����
    TNetRSParam tNetRSParam;
    tNetRSParam.m_wFirstTimeSpan = m_tPrsTimeSpan.m_wFirstTimeSpan;
    tNetRSParam.m_wSecondTimeSpan = m_tPrsTimeSpan.m_wSecondTimeSpan;
    tNetRSParam.m_wThirdTimeSpan = m_tPrsTimeSpan.m_wThirdTimeSpan;
    tNetRSParam.m_wRejectTimeSpan = m_tPrsTimeSpan.m_wRejectTimeSpan;
    m_cHardMulPic.SetNetSendFeedbackVideoParam(2000, FALSE);
    m_cHardMulPic.SetNetRecvFeedbackVideoParam(tNetRSParam, FALSE, 4);

    NEXTSTATE(NORMAL);
    return ;
} 

/*====================================================================
	����  : MsgChangeVidMixParamProc
	����  : �ı仭�渴�ϲ�����Ϣ��Ӧ
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    03/12/4	    v1.0	    zhangsh	      create
====================================================================*/
void CVMPInst::MsgChangeVidMixParamProc(CMessage* const pMsg)
{
    BOOL bCheckRet;
    CServMsg cServMsg(pMsg->content, pMsg->length);
    CKDVVMPParam tParam[2];

    switch(CurState())
    {
    case RUNNING:
        break;
    default:
        OspPrintf( TRUE, FALSE, "[VMP Eroor]Error State:%d\n", CurState());
        return;
    }

	memcpy((u8*)tParam, cServMsg.GetMsgBody(), 2 * sizeof(CKDVVMPParam));

	// ��������������������, zgc, 2008-03-19
	u8 byNewStyleChnNum = GetVmpChlNumByStyle(tParam[0].m_byVMPStyle);
    u8 byOldStyleChnNum = GetVmpChlNumByStyle(m_tParam[0].m_byVMPStyle);
    memcpy(m_tParam, tParam, 2 * sizeof(CKDVVMPParam));

    // guzh [10/18/2007] ֧����������
	// zgc, 2008-03-19, ֱ���޸�m_tParam
    if ( m_tCfg.IsEnableBitrateCheat() )
    {        
        m_tParam[0].m_wBitRate = m_tCfg.GetDecRateDebug( m_tParam[0].m_wBitRate );
        vmplog("[MsgChangeVidMixParamProc] New Enc Bitrate: %d\n", m_tParam[0].m_wBitRate);

        if ( m_bDbVid )
        {
            m_tParam[1].m_wBitRate = m_tCfg.GetDecRateDebug( m_tParam[1].m_wBitRate );
            vmplog("[MsgChangeVidMixParamProc] New 2nd Enc Bitrate: %d\n", m_tParam[1].m_wBitRate);
        }
    }   

	/* �Ƶ����������Ϸ�, zgc, 2008-03-19
    u8 byNewStyleChnNum = GetVmpChlNumByStyle(tParam[0].m_byVMPStyle);
    u8 byOldStyleChnNum = GetVmpChlNumByStyle(m_tParam[0].m_byVMPStyle);

    memcpy(&m_tParam, &tParam, 2 * sizeof(CKDVVMPParam));
	*/

    u8 byNeedPrs = *(cServMsg.GetMsgBody() + 2 * sizeof(CKDVVMPParam));
    
    m_tStyleInfo = *(TVmpStyleCfgInfo*)(cServMsg.GetMsgBody() + 2 * sizeof(CKDVVMPParam) + sizeof(u8));
    
    // ����ϳɷ��ת�����ͨ����
    if ( byNewStyleChnNum >= byOldStyleChnNum )
    {
        // 1.���ñ߿����ɫ
        BOOL32 bRet = SetVmpAttachStyle(m_tStyleInfo);
        if(FALSE == bRet)
        {
            OspPrintf(TRUE, FALSE, "[Info]Fail to set vmp attach style (MsgChangeVidMixParamProc)\n");
            cServMsg.SetMsgBody(NULL, 0);
            cServMsg.SetErrorCode(1);
            SendMsgToMcu(VMP_MCU_CHANGEVIDMIXPARAM_NACK, &cServMsg);
            return;
        }
        else
        {
            vmplog("now we get new params from mcu, change according to it!\n");
            SendMsgToMcu(VMP_MCU_CHANGEVIDMIXPARAM_ACK, &cServMsg);
        }
        // 2.���û���ϳɷ��
        bCheckRet = ChangeVMPStyle(tParam[0]);
        if (bCheckRet)
        {
            cServMsg.SetErrorCode(0); // ok
        }
        else
        {
            
			OspPrintf(TRUE, FALSE, "[MsgChangeVidMixParamProc] Fail to ChangeVMPStyle\n");
			cServMsg.SetMsgBody(NULL, 0);
            cServMsg.SetErrorCode(1);
            SendMsgToMcu(VMP_MCU_CHANGEVIDMIXPARAM_NACK, &cServMsg);
			return;
        }
    }
    else
    {
        // 2.���û���ϳɷ��
        bCheckRet = ChangeVMPStyle(tParam[0]);
        if (bCheckRet)
        {
            cServMsg.SetErrorCode(0); // ok
        }
        else
        {
			OspPrintf(TRUE, FALSE, "[MsgChangeVidMixParamProc] Fail to ChangeVMPStyle\n");
			cServMsg.SetMsgBody(NULL, 0);
            cServMsg.SetErrorCode(1); // error
            SendMsgToMcu(VMP_MCU_CHANGEVIDMIXPARAM_NACK, &cServMsg);
			return;

        }
        // 1.���ñ߿����ɫ
        BOOL32 bRet = SetVmpAttachStyle(m_tStyleInfo);
        if(FALSE == bRet)
        {
            OspPrintf(TRUE, FALSE, "[Info]Fail to set vmp attach style (MsgChangeVidMixParamProc)\n");
            cServMsg.SetMsgBody(NULL, 0);
            cServMsg.SetErrorCode(1);
            SendMsgToMcu(VMP_MCU_CHANGEVIDMIXPARAM_NACK, &cServMsg);
            return;
        }
        else
        {
            vmplog("now we get new params from mcu, change according to it!\n");
            SendMsgToMcu(VMP_MCU_CHANGEVIDMIXPARAM_ACK, &cServMsg);
        }
    }
	// ��ֹͣԭ���Ĺ���, zgc, 2007-09-21
	ClearSmoothSendRule( ntohl(m_tSndAddr[0].dwIp), ntohs(m_tSndAddr[0].wPort) );
	// ����ƽ����������, zgc, 2007-09-19
	// ƽ������������Ȼʹ�ó�ʼ���ʽ�������, zgc, 2008-03-19
	SetSmoothSendRule( ntohl(m_tSndAddr[0].dwIp), ntohs(m_tSndAddr[0].wPort),
                       tParam[0].m_wBitRate );
	vmplog( "[ChangeParam] Main bitrate : %d\n", tParam[0].m_wBitRate );
    // ����ҵ���ж��Ƿ���Ҫ��ڶ�·ƽ����zgc, 2008-03-19
	//if ( m_bDbVid )
	if ( m_bDbVid && ( MEDIA_TYPE_NULL != m_tParam[1].m_byEncType ) )
    {
		ClearSmoothSendRule( ntohl(m_tSndAddr[1].dwIp), ntohs(m_tSndAddr[1].wPort) );
        SetSmoothSendRule( ntohl(m_tSndAddr[1].dwIp), ntohs(m_tSndAddr[1].wPort),
                           tParam[1].m_wBitRate );
		vmplog( "[ChangeParam] Sec bitrate : %d\n", tParam[1].m_wBitRate );
    }
    
    cServMsg.SetMsgBody(NULL, 0);
    SendMsgToMcu(VMP_MCU_CHANGESTATUS_NOTIF, &cServMsg);

    vmplog("[ChangeParam]change param success,vmp work according to new params!\n");
    vmplog("[ChangeParam] PrsTimeSpan: Fst.%d, Snd.%d, Third.%d, Reject.%d\n",
            ntohs(m_tPrsTimeSpan.m_wFirstTimeSpan), ntohs(m_tPrsTimeSpan.m_wSecondTimeSpan),
            ntohs(m_tPrsTimeSpan.m_wThirdTimeSpan), ntohs(m_tPrsTimeSpan.m_wRejectTimeSpan) );

    SendStatusChangeMsg(1, 1, 0/*always*/);

    TNetRSParam tNetRSParam;
    u8 byMaxMemNum = GetVmpNumOfStyle(tParam[0].m_byVMPStyle);
    
    if (byNeedPrs)// ���ö����ش�����
    {
        tNetRSParam.m_wFirstTimeSpan = m_tPrsTimeSpan.m_wFirstTimeSpan;
        tNetRSParam.m_wSecondTimeSpan = m_tPrsTimeSpan.m_wSecondTimeSpan;
        tNetRSParam.m_wThirdTimeSpan = m_tPrsTimeSpan.m_wThirdTimeSpan;
        tNetRSParam.m_wRejectTimeSpan = m_tPrsTimeSpan.m_wRejectTimeSpan;
        /*u16 wRet = */m_cHardMulPic.SetNetSendFeedbackVideoParam(2000, TRUE);
        /*wRet = */m_cHardMulPic.SetNetRecvFeedbackVideoParam(tNetRSParam, TRUE, byMaxMemNum);
    }
    else
    {
        //zbq [09/21/2007] VMPδ����PRS����͸��������
        //tNetRSParam.m_wFirstTimeSpan = m_tPrsTimeSpan.m_wFirstTimeSpan;
        //tNetRSParam.m_wSecondTimeSpan = m_tPrsTimeSpan.m_wSecondTimeSpan;
        //tNetRSParam.m_wThirdTimeSpan = m_tPrsTimeSpan.m_wThirdTimeSpan;
        //tNetRSParam.m_wRejectTimeSpan = m_tPrsTimeSpan.m_wRejectTimeSpan;

        tNetRSParam.m_wFirstTimeSpan = 0;
        tNetRSParam.m_wSecondTimeSpan = 0;
        tNetRSParam.m_wThirdTimeSpan = 0;
        tNetRSParam.m_wRejectTimeSpan = 0;
        
        /*u16 wRet = */m_cHardMulPic.SetNetSendFeedbackVideoParam(2000, FALSE);
        /*wRet = */m_cHardMulPic.SetNetRecvFeedbackVideoParam(tNetRSParam, FALSE, byMaxMemNum);
    }
    
    return ;
}

/*====================================================================
	����  : ChangeVMPStyle
	����  : �ı仭�渴�ϲ���(�ı��˷��)��Ϣ��Ӧ
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   03/12/4	   v1.0	      zhangsh	    create
====================================================================*/
BOOL CVMPInst::ChangeVMPStyle(CKDVVMPParam& tParam)
{
    int nRet = 0;
    int nErrNum = 0;
    TNetAddress tNetAddr;
    TNetAddress tNetRtcpAddr;
    BOOL bChangeStyle = FALSE;

    u8   byStyle = ConvertVcStyle2HardStyle(tParam.m_byVMPStyle);

    vmplog( "byStyle: %d, m_byVMPStyle: %d\n", byStyle, m_tCfg.m_tStatus.m_byVMPStyle);

    // tNetAddr.dwIp = m_tCfg.dwMcuIP;
    tNetAddr.dwIp = 0;
    tNetRtcpAddr.dwIp = m_tSndAddr[0].dwIp;

    if (byStyle != m_tCfg.m_tStatus.m_byVMPStyle)// ��ԭ�з��ͬ
    {
        bChangeStyle = TRUE;
        
        vmplog("[Param]:Change VMP Style to %u, the total channel: %d !\n", byStyle, m_tCfg.m_tStatus.m_byMemberNum);

        // �����Ƴ����е�ͨ��
        for (u8 byChnNo = 0; byChnNo < m_tCfg.m_tStatus.m_byMemberNum; byChnNo++)
        {
            if (TRUE == m_bAddVmpChannel[byChnNo])
            {
                m_bAddVmpChannel[byChnNo] = FALSE;
                // nRet = m_cHardMulPic.RemoveChannel(byChnNo);
				// nRet = m_cHardMulPic.StopAddIcon( byChnNo );  // ��ʱ����
				vmplog("Stop add icon channel = %d, Return code: %d.", byChnNo, nRet);
            }
        }
        // �ı�����
        nRet = m_cHardMulPic.SetMulPicType(byStyle);
        if (HARD_MULPIC_OK != nRet)
        {
            OspPrintf(TRUE, FALSE, "[Info] Fail to call: SetMulPicType, Error code: %d\n", nRet);
            return FALSE;
        }
    }

    m_tCfg.m_tStatus.m_byVMPStyle = byStyle;
    m_tCfg.m_tStatus.m_byMemberNum = tParam.m_byMemberNum;

    // ���ñ������
    SetEncryptParam(0, &tParam.m_tVideoEncrypt[0], VmpGetActivePayload(tParam.m_byEncType));
    if( m_bDbVid )
    {
        SetEncryptParam(1, &tParam.m_tVideoEncrypt[1], VmpGetActivePayload(tParam.m_byEncType)); // ���õڶ�·�������
    }

	//zhouyiliang 20110214 �������л���ʱ���񲻱䵫�ǳ�Ա�����ɶ���٣���ô�ϵĶ������ͨ����״̬ҲҪ���
    for(u8 byChnNo = 0; byChnNo < MAXNUM_MPUSVMP_MEMBER/*m_tCfg.m_tStatus.m_byMemberNum*/; byChnNo++)
    {
		if ((TRUE == m_bAddVmpChannel[byChnNo]) && (tParam.m_atMtMember[byChnNo].IsNull()))
		{
			m_bAddVmpChannel[byChnNo] = FALSE;
			nRet = m_cHardMulPic.RemoveChannel(byChnNo); // ������ԭ�з����ͬ����

			vmplog("[Info]Remove channel %d, the return code: %d\n", byChnNo, nRet);
		 }
		if (byChnNo >= m_tCfg.m_tStatus.m_byMemberNum) //������ֽ����������
		{
			continue;	
		}
        
		SetDecryptParam(byChnNo, &tParam.m_tVideoEncrypt[byChnNo], &tParam.m_tDoublePayload[byChnNo]);

		vmplog("[ChangeVMPStyle] Channel: %d, RealPayLoad: %d, ActivePayLoad: %d\n", byChnNo,
                    tParam.m_tDoublePayload[byChnNo].GetRealPayLoad(),
                    tParam.m_tDoublePayload[byChnNo].GetActivePayload());
      
        if (bChangeStyle || ((FALSE == m_bAddVmpChannel[byChnNo]) &&
            (tParam.m_tDoublePayload[byChnNo].GetRealPayLoad() != 0 ||
            tParam.m_tDoublePayload[byChnNo].GetActivePayload() != 0)))
        {
            // ���ͨ��
            tNetAddr.wPort = htons(m_tCfg.wRecvStartPort + byChnNo*PORTSPAN);
            tNetRtcpAddr.wPort = htons(GetSendRtcpPort(byChnNo));
            nRet = m_cHardMulPic.AddChannel(byChnNo, tNetAddr, tNetRtcpAddr);

            vmplog("[ChangeVMPStyle]:Add channel: %u, return code: %d!\n", byChnNo, nRet);
            if (HARD_MULPIC_OK != nRet)
            {
                nErrNum++;
            }
            else
            {        
				m_bAddVmpChannel[byChnNo] = TRUE;
/*		
				vmplog(" channel=%d, alias= %s\n", byChnNo, tParam.m_atMtMember[byChnNo].GetMbAlias());				
				vmplog("Text color= %0x, Bk color= %0x.\n", m_tStyleInfo.GetTextColor(), m_tStyleInfo.GetTopicBkColor());

				s8 achDataBuf[MAX_BMPFILE_SIZE];
				memset(achDataBuf, 0, sizeof(achDataBuf));
				u32 dwDataLen = 0;
				TTopicParam tTopicParam;
				tTopicParam.SetFontType(m_tStyleInfo.GetFontType() );
				tTopicParam.SetFontSize(m_tStyleInfo.GetFontSize() );
				tTopicParam.SetHeight( wDEFAULT_BMP_HEIGHT );
				tTopicParam.SetWidth( ntohs(tParam.m_wVideoWidth) );
				tTopicParam.SetTextColor( m_tStyleInfo.GetTextColor() );
				tTopicParam.SetBkColor( m_tStyleInfo.GetTopicBkColor() );
				tTopicParam.SetAlignment( MIDDLE_ALIGN );
				tTopicParam.SetText(tParam.m_atMtMember[byChnNo].GetMbAlias());		
				m_cDrawInterface.DrawBmp(achDataBuf, dwDataLen, tTopicParam);
				TBackBGDColor tBackBGDColor;
				tBackBGDColor.RColor = GetRColor(m_tStyleInfo.GetTopicBkColor());
				tBackBGDColor.GColor = GetGColor(m_tStyleInfo.GetTopicBkColor());
				tBackBGDColor.BColor = GetBColor(m_tStyleInfo.GetTopicBkColor());

				nRet = m_cHardMulPic.StartAddIcon( byChnNo, (u8*)achDataBuf, dwDataLen, 0, 0, tTopicParam.GetWidth(),
					                         tTopicParam.GetHeight(), tBackBGDColor, m_tStyleInfo.GetDiaphaneity() );
				vmplog("[Info] Add Icon . error code = %d.", nRet);

*/
            }
            vmplog("[Info]Channel-%u add return value is %u(correct is %u),the Ip is %u Port is %u!\n",
                                       byChnNo, nRet, HARD_MULPIC_OK, tNetAddr.dwIp,tNetAddr.wPort);

        }
    }
    return ( 0 == nErrNum );
}

/*=============================================================================
  �� �� ���� MsgUpdataVmpEncryptParamProc
  ��    �ܣ� ���ļ��ܲ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CVMPInst::MsgUpdataVmpEncryptParamProc(CMessage * const pMsg)
{
    if( NULL == pMsg )
	{
		OspPrintf(TRUE, FALSE, "[MsgUpdataVmpEncryptParamProc] Error input parameter.\n");
		return;
	}

	u8  byChannelNo = 0;
    TMediaEncrypt  tVideoEncrypt;
    TDoublePayload tDoublePayload;

    CServMsg cServMsg(pMsg->content, pMsg->length);

    byChannelNo    = *(u8 *)(cServMsg.GetMsgBody());
    tVideoEncrypt  = *(TMediaEncrypt *)(cServMsg.GetMsgBody() + sizeof(u8));
    tDoublePayload = *(TDoublePayload *)(cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TMediaEncrypt));


    vmplog("byChannelNo: %d, ActivePayLoad: %d, RealPayLoad: %d\n", 
                                                        byChannelNo,
                                                        tDoublePayload.GetActivePayload(),
                                                        tDoublePayload.GetRealPayLoad());

    SetDecryptParam(byChannelNo, &tVideoEncrypt, &tDoublePayload);

    if ( !m_bAddVmpChannel[byChannelNo]
		&&( tDoublePayload.GetRealPayLoad() != 0
		    || tDoublePayload.GetActivePayload() != 0 ) )
    {
        TNetAddress tNetAddr;
        TNetAddress tNetRtcpAddr;
        //���ͨ��
        //tNetAddr.dwIp = m_tCfg.dwMcuIP;
        tNetAddr.dwIp = 0;
        tNetRtcpAddr.dwIp = m_tSndAddr[0].dwIp;
        tNetAddr.wPort = htons(m_tCfg.wRecvStartPort + byChannelNo*PORTSPAN);        
        tNetRtcpAddr.wPort = htons(GetSendRtcpPort(byChannelNo));         
        s32 nRet = m_cHardMulPic.AddChannel(byChannelNo, tNetAddr, tNetRtcpAddr);
        if ( HARD_MULPIC_OK == nRet )
        {
            m_bAddVmpChannel[byChannelNo] = TRUE;
        }

        vmplog("[Info]Channel-%u add return value is %u(correct is %u),the Ip is %u Port is %u!\n",
                                   byChannelNo, nRet, HARD_MULPIC_OK, tNetAddr.dwIp,tNetAddr.wPort);

    }

    cServMsg.SetMsgBody(NULL, 0);
    SendMsgToMcu(VMP_MCU_CHANGESTATUS_NOTIF, &cServMsg);

    SendStatusChangeMsg(1, 1, 0/*always*/);

	return;
}

/*====================================================================
	����  : ChangeVMPChannel
	����  : �ı仭�渴�ϵĲ���(���ı��߼�ͨ��)��Ϣ��Ӧ
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   03/12/4	   v1.0	      zhangsh	    create
====================================================================*/
BOOL CVMPInst::ChangeVMPChannel(CKDVVMPParam& tParam)
{
    int nErrNum = 0;
    TNetAddress rcvAddr;
    TNetAddress tNetRtcpAddr;
    int nRet = HARD_MULPIC_OK;
    rcvAddr.dwIp = 0;
    tNetRtcpAddr.dwIp = m_tSndAddr[0].dwIp;

    vmplog("[Info]now we begin to change channel,but if you see this there maybe error!\n");

    u32 nLoop = 0;
    for(nLoop =0; nLoop < tParam.m_byMemberNum; nLoop++)
    {
        if (tParam.m_atMtMember[nLoop].IsNull())
        {
            vmplog("[Info]The member-%u is NULL\n", nLoop);
        }
        else
        {
            vmplog("[Info]The member-%u is to be add\n", nLoop);
        }

        if (!m_tCfg.m_tStatus.m_atMtMember[nLoop].IsNull())
        {
            if (tParam.m_atMtMember[nLoop].IsNull())
            {
                // ��ǰ�����˵�����û���ˣ��Ƴ�
                nRet = m_cHardMulPic.RemoveChannel(nLoop);
                OspDelay(2000);
                if (nRet != HARD_MULPIC_OK)
                {
                    nErrNum ++;
                }
                vmplog("[Info] Remove channel %d, return code: %d!\n", nLoop, nRet);

            }
        }			
        else
        {
            if (!tParam.m_atMtMember[nLoop].IsNull())
            {
                // ��ǰû��, ������, ����
                rcvAddr.wPort = htons(m_tCfg.wRecvStartPort + nLoop*PORTSPAN);               
                tNetRtcpAddr.wPort = htons(GetSendRtcpPort(nLoop)); 
                nRet = m_cHardMulPic.AddChannel(nLoop, rcvAddr, tNetRtcpAddr);	
                if (nRet != HARD_MULPIC_OK)
                {
                    nErrNum++;
                }
                else
                {
                    m_bAddVmpChannel[nLoop] = TRUE;
                }

                vmplog("[Info]Channel-%u add return value is %u(correct is %u),the Ip is %u Port is %u!\n",
                                          nLoop,nRet, HARD_MULPIC_OK,rcvAddr.dwIp,rcvAddr.wPort);
            }
        }
        m_tCfg.m_tStatus.m_atMtMember[nLoop] = tParam.m_atMtMember[nLoop];
    }
    return (0 == nErrNum);
}

/*====================================================================
	����  : MsgSetBitRate
	����  : �ı䷢������
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void CVMPInst::MsgSetBitRate(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    u8 byChalNum = cServMsg.GetChnIndex();
    u16 wBitrate = 0;
	u16 wCheatBit = 0;

    cServMsg.GetMsgBody((u8*)&wBitrate, sizeof(u16));
    wBitrate = ntohs(wBitrate);

    vmplog("[Info]now we set the encode-%u bitrate to %u!\n", byChalNum, wBitrate);

    // guzh [10/18/2007] ֧����������
    if ( m_tCfg.IsEnableBitrateCheat() )
    {
        wCheatBit = m_tCfg.GetDecRateDebug( wBitrate );
    }   

    if (1 == byChalNum ) // �Ƿ�˫����
    {
        m_cHardMulPic.ChangeBitRate(FALSE, wCheatBit);
		m_tParam[0].m_wBitRate = wCheatBit; //����������, zgc, 2007-10-09
    }
    if (2 == byChalNum)
    {
        m_cHardMulPic.ChangeBitRate(TRUE, wCheatBit);
		m_tParam[1].m_wBitRate = wCheatBit; //����������, zgc, 2007-10-09
    }

	// �޸�ƽ�����͹���, zgc, 2007-10-09
	// ƽ������������Ȼʹ�ó�ʼ���ʽ�������, zgc, 2008-03-19
	if ( !m_tCfg.IsNoSmooth() )
	{
		if ( 1 == byChalNum )
		{			
			ClearSmoothSendRule( ntohl(m_tSndAddr[0].dwIp), ntohs(m_tSndAddr[0].wPort) );
			SetSmoothSendRule( ntohl(m_tSndAddr[0].dwIp), ntohs(m_tSndAddr[0].wPort),
							wBitrate );
			vmplog( "[MsgSetBitRate]Set main smooth rule, bitrate : %d\n", wBitrate );
		}
		// ����ҵ���ж��Ƿ���Ҫ��ڶ�·ƽ����zgc, 2008-03-19
		//if ( 2 == byChalNum && m_bDbVid )
		if ( 2 == byChalNum && m_bDbVid && ( MEDIA_TYPE_NULL != m_tParam[1].m_byEncType ) )
		{
			ClearSmoothSendRule( ntohl(m_tSndAddr[1].dwIp), ntohs(m_tSndAddr[1].wPort) );
			SetSmoothSendRule( ntohl(m_tSndAddr[1].dwIp), ntohs(m_tSndAddr[1].wPort),
							   wBitrate );
			vmplog( "[MsgSetBitRate]Set sec smooth rule, bitrate : %d\n", wBitrate );
		}
	}

    SendMsgToMcu(VMP_MCU_SETCHANNELBITRATE_ACK, &cServMsg);
}

/*====================================================================
	����  : MsgTimerNeedIFrameProc
	����  : 
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
    ��  ��         �汾        �޸���       �޸�����
    05/06/15       v3.6       libo        create
====================================================================*/
void CVMPInst::MsgTimerNeedIFrameProc(void)
{
    CServMsg cServMsg;
    cServMsg.SetConfId(m_cConfId);
    TKdvDecStatis tDecStatis;
    for (u8 byChnNo = 0; byChnNo < m_byMaxChannelNum; byChnNo++)
    {        
        if(m_bAddVmpChannel[byChnNo])
        {
            memset(&tDecStatis, 0, sizeof(TKdvDecStatis));
            m_cHardMulPic.GetVidRecvStatis(byChnNo, tDecStatis);
            if (tDecStatis.m_bVidCompellingIFrm)
            {
                cServMsg.SetChnIndex(byChnNo);
                SendMsgToMcu(VMP_MCU_NEEDIFRAME_CMD, &cServMsg);
                vmplog("[MsgTimerNeedIFrameProc]vmp channel:%d request iframe!!\n", byChnNo);

            }
            else
            {   
                // vmplog("[MsgTimerNeedIFrameProc]vmp channel:%d NOT NEED request iframe!!\n", byChnNo);
            }
        }
    }

    SetTimer(EV_VMP_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);
	return;
}

/*====================================================================
	����  : MsgFastUpdatePicProc
	����  : 
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
    ��  ��         �汾        �޸���       �޸�����
    05/06/23       v3.6       libo        create
====================================================================*/
void CVMPInst::MsgFastUpdatePicProc(void)
{
    u32 dwTimeInterval = 1 * OspClkRateGet();

    u32 dwCurTick = OspTickGet();	
    if( dwCurTick - m_dwLastFUPTick > dwTimeInterval)
    {
        m_dwLastFUPTick = dwCurTick;
        m_cHardMulPic.SetFastUpdata();

        vmplog("[MsgFastUpdatePicProc]m_cHardMulPic.SetFastUpdata()!\n");

    }
	return;
}

/*====================================================================
	����  : MsgGetVidMixParamProc
	����  : MCU��ѯ���渴�ϲ�����Ϣ��Ӧ
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void CVMPInst::MsgGetVidMixParamProc(CMessage* const pMsg)
{
	if( NULL == pMsg )
	{
		OspPrintf(TRUE, FALSE, "[MsgGetVidMixParamProc] pMsg is null\n");
	}
    CServMsg cServMsg;
	cServMsg.SetMsgBody((u8*)&(m_tCfg.m_tStatus), sizeof(m_tCfg.m_tStatus));

    SendMsgToMcu(VMP_MCU_GETVIDMIXPARAM_ACK, &cServMsg);

    vmplog("[Info]Get vmp params success!\n");

    return;
}

/*====================================================================
	����  : StatusShow
	����  : ��ʾ��ǰ���渴����״̬��Ϣ
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void CVMPInst::StatusShow()
{
    TMulPicStatus tMulPicStatus;
    TMulPicStatis tMulPicStatis;
    memset(&tMulPicStatus, 0, sizeof(tMulPicStatus));
    memset(&tMulPicStatis, 0, sizeof(tMulPicStatis));

    m_cHardMulPic.GetStatus(tMulPicStatus);
    m_cHardMulPic.GetStatis(tMulPicStatis);

    ::OspPrintf(TRUE, FALSE, "\t===========The state of VMP is below===============\n");
	
	::OspPrintf(TRUE, FALSE, "The MTU Size is : %d\n", m_wMTUSize );

    ::OspPrintf(TRUE, FALSE, "  IDLE =%d, NORMAL=%d, RUNNING=%d, VMP Current State =%d\n",
	                          IDLE, NORMAL, RUNNING, CurState());

    ::OspPrintf(TRUE, FALSE, "\t===========The status of VMP is below===============\n");
    ::OspPrintf(TRUE, FALSE, "Merge=%d\t Style=%d\t MapNum=%d\t CurChannel=%d\n",
                              tMulPicStatus.bMergeStart,
                              tMulPicStatus.byType,
                              tMulPicStatus.dwTotalMapNum,
                              tMulPicStatus.byCurChnNum);

    ::OspPrintf(TRUE, FALSE, "Send Ip Address:0x%x \t,Port:%d\n", 
                ntohl(tMulPicStatus.atNetSndAddr.dwIp), tMulPicStatus.atNetSndAddr.wPort);

    s32 nLoop = 0;
    for (nLoop =0; nLoop < tMulPicStatus.byCurChnNum; nLoop++)
    {    
        ::OspPrintf(TRUE, FALSE, "Receive Channel(%d) Ip Address:0x%x \t, Port:%d\n",
                    nLoop, ntohl(tMulPicStatus.atNetRcvAddr[nLoop].dwIp), tMulPicStatus.atNetRcvAddr[nLoop].wPort);
    }
    ::OspPrintf(TRUE, FALSE, "\t-------------The Video Encoding Params--------------\n");
    ::OspPrintf(TRUE, FALSE, "EncType=%d\t ComMode=%d\t KeyFrameInter=%d\t MaxQuant=%d\t MinQuant=%d\t\n",
                              tMulPicStatus.tVideoEncParam.m_byEncType,
                              tMulPicStatus.tVideoEncParam.m_byRcMode,
                              tMulPicStatus.tVideoEncParam.m_byMaxKeyFrameInterval,
                              tMulPicStatus.tVideoEncParam.m_byMaxQuant,
                              tMulPicStatus.tVideoEncParam.m_byMinQuant);

    ::OspPrintf(TRUE, FALSE, "BitRate=%d\t SndBandWith=%d\t FrameRate=%d\t ImageQuality=%d\t VideoWidth=%d\t VideoHeight=%d\t\n",
                              tMulPicStatus.tVideoEncParam.m_wBitRate,
                              tMulPicStatus.tVideoEncParam.m_dwSndNetBand,
                              tMulPicStatus.tVideoEncParam.m_byFrameRate,
                              tMulPicStatus.tVideoEncParam.m_byImageQulity,
                              tMulPicStatus.tVideoEncParam.m_wVideoWidth,
                              tMulPicStatus.tVideoEncParam.m_wVideoHeight);

    ::OspPrintf(TRUE, FALSE, "\t-------------Network Statics --------------\n");
    ::OspPrintf(TRUE, FALSE, "SendBitRate=%d\t SendPackNum=%d\t LostPackNum=%d\t\n",
                              tMulPicStatis.m_dwSendBitRate,
                              tMulPicStatis.m_dwSendPackNum,
                              tMulPicStatis.m_dwSendDiscardPackNum);

    for(nLoop = 0; nLoop < tMulPicStatus.byCurChnNum; nLoop++)
    {
        ::OspPrintf(TRUE, FALSE, "Receive Channel(%d) BitRate:%d\t RecvPackNum:%d\t LostPackNum:%d\n",nLoop,
                                  tMulPicStatis.atMulPicChnStatis[nLoop].m_dwRecvBitRate,
                                  tMulPicStatis.atMulPicChnStatis[nLoop].m_dwRecvPackNum,
                                  tMulPicStatis.atMulPicChnStatis[nLoop].m_dwRecvLosePackNum);
    }
    return;
}

/*=============================================================================
�� �� ���� ParamShow
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/3/19   4.0		�ܹ��                  ����
=============================================================================*/
void CVMPInst::ParamShow(void)
{
	::OspPrintf(TRUE, FALSE, "\t===========================The Param of VMP is below=============================\t\n");
	::OspPrintf(TRUE, FALSE, "\t----------------------------- The first video chnnl -----------------------------\t\n");
	m_tParam[0].Print();
	::OspPrintf(TRUE, FALSE, "\n");
	::OspPrintf(TRUE, FALSE, "\t----------------------------- The second video chnnl ----------------------------\t\n");
	m_tParam[1].Print();
	::OspPrintf(TRUE, FALSE, "\t---------------------------------------------------------------------------------\t\n");
}

/*====================================================================
	����  : SendStatusChangeMsg
	����  : ״̬�ı��֪ͨMCU
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void CVMPInst::SendStatusChangeMsg( u8 byOnline, u8 byState, u8 byStyle )
{
    CServMsg cServMsg;
    TPeriEqpStatus tInitStatus;
    memset(&tInitStatus,0,sizeof(TPeriEqpStatus));

    tInitStatus.m_byOnline = byOnline;
    tInitStatus.SetMcuEqp(m_tCfg.byMcuId, m_tCfg.byEqpId, m_tCfg.byEqpType);
    tInitStatus.SetAlias(m_tCfg.szAlias);
    tInitStatus.m_tStatus.tVmp.m_tVMPParam.SetVMPStyle(m_tCfg.m_tStatus.m_byVMPStyle);
    tInitStatus.m_tStatus.tVmp.m_byUseState = byState;
//	tInitStatus.m_tStatus.tVmp.m_byEncodeNum = m_bDbVid ? 2 : 1; //����·��, zgc, 20070620

    vmplog("Before: alias: %s EqpId: %u EqpType: %u McuId: %u style: %d\n",
                                m_tCfg.szAlias, 
                                m_tCfg.byEqpId, 
                                m_tCfg.byEqpType, 
                                m_tCfg.byMcuId,
								byStyle);
    vmplog("After: alias: %s EqpId: %u EqpType: %u McuId: %u\n",
                                m_tCfg.szAlias, 
                                tInitStatus.GetEqpId(), 
                                tInitStatus.GetEqpType(), 
                                tInitStatus.GetMcuId());

    tInitStatus.m_tStatus.tVmp.m_byChlNum = m_byMaxChannelNum;
    cServMsg.SetMsgBody((u8*)&(tInitStatus), sizeof(TPeriEqpStatus));
    SendMsgToMcu(VMP_MCU_VMPSTATUS_NOTIF, &cServMsg);
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
void CVMPInst::ProcSetQosInfo(CMessage* const pMsg)
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
  ��    �ܣ� �󸴺�Qosֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8& byValue
             u8 byPrior
  �� �� ֵ�� void 
=============================================================================*/
void CVMPInst::ComplexQos(u8& byValue, u8 byPrior)
{
    u8 byLBit = ((byPrior & 0x08) >> 3);
    u8 byRBit = ((byPrior & 0x04) >> 1);
    u8 byTBit = ((byPrior & 0x02)<<1);
    u8 byDBit = ((byPrior & 0x01)<<3);
    
    byValue = byValue + ((byDBit + byTBit + byRBit + byLBit)*2);
    return;
}

/*=============================================================================
  �� �� ���� ProcSetStyleMsg
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CVMPInst::ProcSetStyleMsg(CMessage* const pMsg)
{
    TVmpStyleCfgInfo  tStyleInfo;
    CServMsg cStyleInfo(pMsg->content, pMsg->length);

    cStyleInfo.GetMsgBody((u8*)&tStyleInfo, sizeof(TVmpStyleCfgInfo));
    BOOL32 bRet = SetVmpAttachStyle(tStyleInfo);
    if(FALSE == bRet)
    {
        vmplog( "[Error] set vmp attach style failed !\n");
        return;
    }
    return;

}

/*=============================================================================
�� �� ���� ProcStopMapCmd
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage* const pMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/4/24   4.0			�ܹ��                  ����
=============================================================================*/
void CVMPInst::ProcStopMapCmd(CMessage* const pMsg)
{
	if( NULL == pMsg )
	{
		OspPrintf(TRUE, FALSE, "[ProcStopMapCmd] pMsg is null\n");
	}
	
	if ( INVALID_NODE != g_cVMPApp.m_dwMcuNode )
	{
		OspDisconnectTcpNode( g_cVMPApp.m_dwMcuNode );
	}
	if ( INVALID_NODE != g_cVMPApp.m_dwMcuNodeB)
	{
		OspDisconnectTcpNode( g_cVMPApp.m_dwMcuNodeB );
	}
	if( INVALID_NODE == g_cVMPApp.m_dwMcuNode && INVALID_NODE == g_cVMPApp.m_dwMcuNodeB)
	{
		ClearCurrentInst();
	}
	m_cHardMulPic.Quit();
}

/*=============================================================================
  �� �� ���� ProcSetVmpAttachStyle
  ��    �ܣ� ���ø��Ϸ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
BOOL32 CVMPInst::SetVmpAttachStyle(TVmpStyleCfgInfo&  tStyleInfo)
{
    
    memcpy((u8*)&m_tStyleInfo, (void*)&tStyleInfo, sizeof(TVmpStyleCfgInfo));
    
    TMulPicColor tBGDColor;
    GetRGBColor(tStyleInfo.GetBackgroundColor(), tBGDColor.RColor,
                                                 tBGDColor.GColor,
                                                 tBGDColor.BColor );
    
    u32 dwChairColor    = tStyleInfo.GetChairFrameColor();
    u32 dwSpeakerColor  = tStyleInfo.GetSpeakerFrameColor();
    u32 dwAudienceColor = tStyleInfo.GetFrameColor();

    u8 byRColorChair = 0;
    u8 byGColorChair = 0;
    u8 byBColorChair = 0;

    u8 byRColorAudience = 0;
    u8 byGColorAudience = 0;
    u8 byBColorAudience = 0;

    u8 byRColorSpeaker = 0;
    u8 byGColorSpeaker = 0;
    u8 byBColorSpeaker = 0;

    GetRGBColor(dwChairColor,    byRColorChair,    byGColorChair,    byBColorChair);
    GetRGBColor(dwSpeakerColor,  byRColorSpeaker,  byGColorSpeaker,  byBColorSpeaker);
    GetRGBColor(dwAudienceColor, byRColorAudience, byGColorAudience, byBColorAudience);

    TMulPicColor atSlidelineColor[MULPIC_MAX_CHNS];
    memset(atSlidelineColor, 0, sizeof(atSlidelineColor));

    for(u8 byLoop = 0; byLoop < m_tParam[0].m_byMemberNum; byLoop++)
    {
        if(MT_STATUS_CHAIRMAN == m_tParam[0].m_atMtMember[byLoop].GetMemStatus())
        {
            atSlidelineColor[byLoop].RColor = byRColorChair;
            atSlidelineColor[byLoop].GColor = byGColorChair;
            atSlidelineColor[byLoop].BColor = byBColorChair;
            vmplog("The channel: %d, The Chairman Color Value: %0x\n", byLoop, dwChairColor);
        }
        if(MT_STATUS_SPEAKER == m_tParam[0].m_atMtMember[byLoop].GetMemStatus())
        {
            atSlidelineColor[byLoop].RColor = byRColorSpeaker;
            atSlidelineColor[byLoop].GColor = byGColorSpeaker;
            atSlidelineColor[byLoop].BColor = byBColorSpeaker;
            vmplog("The channel: %d, The Speaker  Color Value: %0x\n", byLoop, dwSpeakerColor);
        }
        if(MT_STATUS_AUDIENCE == m_tParam[0].m_atMtMember[byLoop].GetMemStatus())
        {
            atSlidelineColor[byLoop].RColor = byRColorAudience;
            atSlidelineColor[byLoop].GColor = byGColorAudience;
            atSlidelineColor[byLoop].BColor = byBColorAudience;
            vmplog("The channel: %d, The Audience Color Value: %0x\n", byLoop, dwAudienceColor);
        }
    }
    vmplog("The Background Color Value: 0x%x\n", tStyleInfo.GetBackgroundColor() );
    vmplog("The RGB Color of Chairman : R<%d> G<%d> B<%d> \n", byRColorChair,    byGColorChair,    byBColorChair );
    vmplog("The RGB Color of Speaker  : R<%d> G<%d> B<%d> \n", byRColorSpeaker,  byGColorSpeaker,  byBColorSpeaker );
    vmplog("The RGB Color of Audience : R<%d> G<%d> B<%d> \n", byRColorAudience, byGColorAudience, byBColorAudience );
    vmplog("The RGB Color of Backgroud: R<%d> G<%d> B<%d> \n", tBGDColor.RColor, tBGDColor.GColor, tBGDColor.BColor );
	
    s32 nRet = 0;

	//����ϳɷ��ӱ߿�
	if ( m_tStyleInfo.GetIsRimEnabled() )
	{
		nRet = m_cHardMulPic.SetBGDAndSidelineColor(tBGDColor, atSlidelineColor, TRUE);
	}
	else
	{
		nRet = m_cHardMulPic.SetBGDAndSidelineColor(tBGDColor, atSlidelineColor, FALSE);
	}

    if(HARD_MULPIC_OK != nRet)
    {
        OspPrintf(TRUE, FALSE, "[Vmp] Fail to call the SetBGDAndSidelineColor. (ProcSetVmpAttachStyle)\n");
        return FALSE;
    }
    
	vmplog("Set VMP Scheme:%d with RimEnabled:%d successfully !\n", m_tStyleInfo.GetSchemeId(), m_tStyleInfo.GetIsRimEnabled());
    
	return TRUE;
}

/*=============================================================================
  �� �� ���� GetRColor
  ��    �ܣ� ȡRedֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwColorValue
  �� �� ֵ�� u8 
=============================================================================*/
u8 CVMPInst::GetRColor(u32 dwColorValue)
{
    u32 dwRetValue = dwColorValue & 0x000000ff;
    return (u8)dwRetValue;
}

/*=============================================================================
  �� �� ���� GetGColor
  ��    �ܣ� ȡGreenֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwColorValue
  �� �� ֵ�� u8 
=============================================================================*/
u8 CVMPInst::GetGColor(u32 dwColorValue)
{
    u32 dwRetValue = ((dwColorValue & 0x0000ff00) >> 8);
    return (u8)dwRetValue;
}

/*=============================================================================
  �� �� ���� GetBColor
  ��    �ܣ� ȡBlueֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwColorValue
  �� �� ֵ�� u8 
=============================================================================*/
u8 CVMPInst::GetBColor(u32 dwColorValue)
{
    u32 dwRetValue = ((dwColorValue & 0x00ff0000) >> 16);
    return (u8)dwRetValue;
}

/*=============================================================================
  �� �� ���� GetRGBColor
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwColor
             u8& byRColor
             u8& byGColor
             u8& byBColor
  �� �� ֵ�� void 
=============================================================================*/
void CVMPInst::GetRGBColor(u32 dwColor, u8& byRColor, u8& byGColor, u8& byBColor)
{
    byRColor = (u8)dwColor & 0x000000ff;
    byGColor = (u8)((dwColor & 0x0000ff00) >> 8);
    byBColor = (u8)((dwColor & 0x00ff0000) >> 16);
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
u16 CVMPInst::GetSendRtcpPort(u8 byChanNo)
{
    if (byChanNo >= 8)
    {
        return (m_tSndAddr[0].wPort + 4 + byChanNo); 
    }
    else
    {
        return (m_tSndAddr[0].wPort + 2 + byChanNo); 
    }      
}

/*=============================================================================
�� �� ���� SetSmoothSendRule
��    �ܣ� ����ƽ�����͹���
�㷨ʵ�֣� 
ȫ�ֱ����� SSErrno
��    ���� ��Ϊ������
�� �� ֵ�� void
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/03/12  4.0			����                  ����
=============================================================================*/
void CVMPInst::SetSmoothSendRule(u32 dwDestIp, u16 wPort, u32 dwRate)
{
    dwRate = dwRate >> 3;   // bit 2 byte
    u32 dwPeak = dwRate + dwRate / 5; // add 20%
    vmplog( "[SetSmoothSendRule] Setting ssrule for 0x%x:%d, rate/peak=%u/%u KByte.\n",
            dwDestIp, wPort, dwRate, dwPeak );

#ifdef _LINUX_
    s32 nRet = SetSSRule(dwDestIp, wPort, dwRate, dwPeak, 2); // default 2 seconds
    if ( 0 == nRet )
    {
        vmplog( "[SetSmoothSendRule] Set rule failed. SSErrno=%d\n", SSErrno );
    }
#endif

}

/*=============================================================================
�� �� ���� ClearSmoothSendRule
��    �ܣ� ȡ������ƽ�����͹���
�㷨ʵ�֣� 
ȫ�ֱ����� SSErrno
��    ���� ��Ϊ������
�� �� ֵ�� void
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/03/12  4.0			����                  ����
=============================================================================*/
void CVMPInst::ClearSmoothSendRule(u32 dwDestIp, u16 wPort)
{
    vmplog( "[ClearSmoothSendRule] Unsetting ssrule for 0x%x:%d.\n",
            dwDestIp, wPort );

#ifdef _LINUX_
    s32 nRet = UnsetSSRule(dwDestIp, wPort);
    if ( 0 == nRet )
    {
        vmplog( "[ClearSmoothSendRule] Unset rule failed. SSErrno=%d\n", SSErrno );
    }
#endif

}

/*=============================================================================
�� �� ���� StopSmoothSend
��    �ܣ� ֹͣƽ������
�㷨ʵ�֣� 
ȫ�ֱ����� SSErrno
��    ���� 
�� �� ֵ�� void
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/03/12  4.0			����                  ����
=============================================================================*/
void CVMPInst::StopSmoothSend()
{
    vmplog( "[StopSmoothSend] Stopping SmoothSend\n" );

#ifdef _LINUX_
    s32 nRet = CloseSmoothSending();
    if ( 0 == nRet )
    {
        vmplog( "[ClearSmoothSendRule] CloseSmoothSending failed. SSErrno=%d\n", SSErrno );
    }
#endif 

}

/*=============================================================================
    �� �� ���� GetVmpChlNumByStyle
    ��    �ܣ� ���ݻ���ϳɷ���úϳ�ͨ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byVMPStyle
    �� �� ֵ�� u8
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    04/03/31    3.0         ������                ����
=============================================================================*/
u8 CVMPInst::GetVmpChlNumByStyle( u8 byVMPStyle )
{
    u8   byMaxMemNum = 0;
    
    switch( byVMPStyle ) 
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
    case VMP_STYLE_SPECFOUR:
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
    case VMP_STYLE_SEVEN:
        byMaxMemNum = 7;
        break;
    default:
        break;
    }
    
    return byMaxMemNum;
}

/*=============================================================================
    �� �� ���� ProcSetBackBoardOutCmd
    ��    �ܣ� ��������Ƿ�֧��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� u8
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    04/03/31    3.0         ������                ����
=============================================================================*/
void CVMPInst::ProcSetBackBoardOutCmd( CMessage * const pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if ( 1 != cServMsg.GetMsgBodyLen() )
    {
        OspPrintf( TRUE, FALSE, "[ProcSetBackBoardOutCmd] ServMsgLen err\n" );
        return;
    }
    u8 byOut = *(u8*)cServMsg.GetMsgBody();
    m_cHardMulPic.SetBackBoardOut((BOOL32)byOut);
    vmplog( "[ProcSetBackBoardOutCmd] BackBoardOut.%d \n", byOut );

    return;
}

/*=============================================================================
    �� �� ���� VmpGetActivePayload
    ��    �ܣ� 
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� u8
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    07/10/18    4.0         �ű���                ����
=============================================================================*/
u8 CVMPInst::VmpGetActivePayload(u8 byRealPayloadType)
{
    u8 byAPayload = MEDIA_TYPE_NULL;
    u8 byFECType  = FECTYPE_NONE;
    
    switch(byRealPayloadType)
    {
    case MEDIA_TYPE_PCMA:
    case MEDIA_TYPE_PCMU:
    case MEDIA_TYPE_G721:
    case MEDIA_TYPE_G722:
    case MEDIA_TYPE_G7231:
    case MEDIA_TYPE_G728:
    case MEDIA_TYPE_G729:
    case MEDIA_TYPE_MP3:
    case MEDIA_TYPE_G7221C:
    case MEDIA_TYPE_G7221:
    case MEDIA_TYPE_ADPCM:
        break;
        
    case MEDIA_TYPE_H261:
    case MEDIA_TYPE_H262:
    case MEDIA_TYPE_H263:
    case MEDIA_TYPE_MP4:
    case MEDIA_TYPE_H263PLUS:
    case MEDIA_TYPE_H264:

        byFECType = m_tCapSupportEx.GetVideoFECType();
        break;
        
    default:
        OspPrintf( TRUE, FALSE, "[GetActivePayload] unexpected media type.%d, ignore it\n", byRealPayloadType );
        break;
    }
    
    if ( FECTYPE_NONE != byFECType )
    {
        byAPayload = MEDIA_TYPE_FEC;
    }
    else
    {
        switch(byRealPayloadType)
        {
        case MEDIA_TYPE_PCMA:   byAPayload = ACTIVE_TYPE_PCMA;  break;
        case MEDIA_TYPE_PCMU:   byAPayload = ACTIVE_TYPE_PCMU;  break;
        case MEDIA_TYPE_G721:   byAPayload = ACTIVE_TYPE_G721;  break;
        case MEDIA_TYPE_G722:   byAPayload = ACTIVE_TYPE_G722;  break;
        case MEDIA_TYPE_G7231:  byAPayload = ACTIVE_TYPE_G7231; break;
        case MEDIA_TYPE_G728:   byAPayload = ACTIVE_TYPE_G728;  break;
        case MEDIA_TYPE_G729:   byAPayload = ACTIVE_TYPE_G729;  break;
        case MEDIA_TYPE_H261:   byAPayload = ACTIVE_TYPE_H261;  break;
        case MEDIA_TYPE_H262:   byAPayload = ACTIVE_TYPE_H262;  break;
        case MEDIA_TYPE_H263:   byAPayload = ACTIVE_TYPE_H263;  break;
        //case MEDIA_TYPE_ADPCM:  byAPayload = bNewActivePT ? ACTIVE_TYPE_ADPCM : byRealPayloadType;  break;
        //case MEDIA_TYPE_G7221C: byAPayload = bNewActivePT ? ACTIVE_TYPE_G7221C : byRealPayloadType; break;
        default:                byAPayload = byRealPayloadType; break;
        }        
    }
    return byAPayload;
}


CVMPCfg::CVMPCfg()
{
//    FreeStatusDataA();
    m_dwMcuNode = INVALID_NODE;   
    m_dwMcuIId  = INVALID_INS;

//    FreeStatusDataB();
    m_dwMcuNodeB = INVALID_NODE;   
    m_dwMcuIIdB  = INVALID_INS;

    m_bEmbed  = FALSE;
    m_bEmbedB = FALSE;
    m_byRegAckNum = 0;
	m_dwMpcSSrc = 0;
}

CVMPCfg::~CVMPCfg()
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
void CVMPCfg::FreeStatusDataA(void)
{
    m_dwMcuNode = INVALID_NODE;   
    m_dwMcuIId  = INVALID_INS;
    return;
}

/*=============================================================================
  �� �� ���� FreeStatusDataB
  ��    �ܣ� ���B��״̬��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CVMPCfg::FreeStatusDataB(void)
{
    m_dwMcuNodeB = INVALID_NODE;   
    m_dwMcuIIdB  = INVALID_INS;
    return;
}
//END OF FILE
