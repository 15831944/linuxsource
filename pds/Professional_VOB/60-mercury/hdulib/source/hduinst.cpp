/*****************************************************************************
   ģ����      : ������뿨
   �ļ���      : hduinst.cpp
   ����ļ�    : hduinst.h
   �ļ�ʵ�ֹ���: HDU��ʵ��
   ����        : ���ֱ�
   �汾        : V1.0  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2008/11/27  4.6         ���ֱ�       ����
******************************************************************************/
#include "hduinst.h"

static s8	HDUCFG_FILE[KDV_MAX_PATH] = {0};
//static s8	SECTION_HDUSYS[] = "EQPHDU";
static const s8  SECTION_HDUDEBUG[] = "EQPDEBUG";
extern s32     g_nHduPLog;

//extern void RegsterCommands();
CHduApp g_cHduApp;

/*=============================================================================
  �� �� ���� CBVidRecvFrame    
  ��    �ܣ� �����յ�����Ƶ֡����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� PFRAMEHDR pFrmHdr, u32 dwContext
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2008/12/10  4.6         ���ֱ�         ����
=============================================================================*/
static void CBVidRecvFrame(PFRAMEHDR pFrmHdr, u32 dwContext)
{
	CHduInstance * pcInst = (CHduInstance*)dwContext;
    if ( NULL == pFrmHdr || NULL == pcInst )
    {
        hdulog("[CBVidRecvFrame] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrmHdr, dwContext);
        return ;
    }

	CDecoderGrp *pDecoderGrp = pcInst->m_pDecCroup;
	if ( NULL == pDecoderGrp )
	{
		hdulog("[CBVidRecvFrame] pDecoderGrp 0x%x\n", pDecoderGrp);
        return ;
    }

	TFrameHeader tFrameHdr;
	memset(&tFrameHdr, 0x00, sizeof(tFrameHdr));

	tFrameHdr.m_dwMediaType = pFrmHdr->m_byMediaType;
	tFrameHdr.m_dwFrameID   = pFrmHdr->m_dwFrameID;
    tFrameHdr.m_dwSSRC      = pFrmHdr->m_dwSSRC;
	tFrameHdr.m_pData       = pFrmHdr->m_pData + pFrmHdr->m_dwPreBufSize; 
	tFrameHdr.m_dwDataSize  = pFrmHdr->m_dwDataSize;
	tFrameHdr.m_tVideoParam.m_bKeyFrame    = pFrmHdr->m_tVideoParam.m_bKeyFrame;
	tFrameHdr.m_tVideoParam.m_wVideoHeight = pFrmHdr->m_tVideoParam.m_wVideoHeight;
	tFrameHdr.m_tVideoParam.m_wVideoWidth  = pFrmHdr->m_tVideoParam.m_wVideoWidth;

    hdulogall("[CBVidRecvFrame] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, SSRC:%u, DataSize:%u\n",
              tFrameHdr.m_dwMediaType,
              tFrameHdr.m_dwFrameID,
              tFrameHdr.m_tVideoParam.m_bKeyFrame,
              tFrameHdr.m_tVideoParam.m_wVideoWidth,
              tFrameHdr.m_tVideoParam.m_wVideoHeight,              
              tFrameHdr.m_dwSSRC,
              tFrameHdr.m_dwDataSize
              );

	pDecoderGrp->m_cVidDecoder.SetData(tFrameHdr);

	return;
}

/*=============================================================================
  �� �� ���� CBAudRecvFrame    
  ��    �ܣ� �����յ�����Ƶ֡����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� PFRAMEHDR pFrmHdr, u32 dwContext
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2008/12/10  4.6         ���ֱ�         ����
=============================================================================*/
static void CBAudRecvFrame(PFRAMEHDR pFrmHdr, u32 dwContext)
{
	CHduInstance * pcInst = (CHduInstance*)dwContext;
    if (pFrmHdr == NULL || pcInst == NULL)
    {
        hdulog("[CBAudRecvFrame] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrmHdr, dwContext);
        return ;
    }

	CDecoderGrp *pDecoderGrp = pcInst->m_pDecCroup;
	if ( NULL == pDecoderGrp )
	{
		hdulog("[CBAudRecvFrame] pDecoderGrp 0x%x\n", pDecoderGrp);
        return ;
    }

	TFrameHeader tFrameHdr;
	memset(&tFrameHdr, 0x00, sizeof(tFrameHdr));

	tFrameHdr.m_dwMediaType = pFrmHdr->m_byMediaType;
	tFrameHdr.m_dwFrameID   = pFrmHdr->m_dwFrameID;
    tFrameHdr.m_dwSSRC      = pFrmHdr->m_dwSSRC;
	tFrameHdr.m_pData       = pFrmHdr->m_pData + pFrmHdr->m_dwPreBufSize; 
	tFrameHdr.m_dwDataSize  = pFrmHdr->m_dwDataSize;
    tFrameHdr.m_dwAudioMode = pFrmHdr->m_byAudioMode;

    hdulogall("[CBAudRecvFrame] MediaType:%u, FrameID:%u, AudioMode:%u, SSRC:%u, DataSize:%u\n",
              tFrameHdr.m_dwMediaType,
              tFrameHdr.m_dwFrameID,
              tFrameHdr.m_dwAudioMode,              
              tFrameHdr.m_dwSSRC,
              tFrameHdr.m_dwDataSize
              );

	pDecoderGrp->m_cAudDecoder.SetData(tFrameHdr);

	return;
}

/* ------------------CHduInstance���ʵ��  start----------------------*/

/*====================================================================
����  : CHduInstance
����  : ���캯��
����  : 
���  : 
����  : 
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
CHduInstance::CHduInstance()
{

    m_pDecCroup = NULL;
}
/*====================================================================
����  : ~CHduInstance
����  : ��������
����  : 
���  : 
����  : 
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
CHduInstance::~CHduInstance()
{
	if (NULL != m_pDecCroup)
	{
		/*lint -save -e1551*/
		delete(m_pDecCroup);
		/*lint -restore*/
		m_pDecCroup = NULL;
	}
}
/*====================================================================
����  : InstanceEntry
����  : ��ͨʵ����Ϣ���
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void CHduInstance::InstanceEntry( CMessage* const pMsg )
{
	if (NULL == pMsg)
	{
		OspPrintf(TRUE, FALSE, "[InstanceEntry]: The pMsg point can not be Null!\n");
		return;
	}

	CServMsg cServMsg(pMsg->content, pMsg->length);
    cServMsg.SetEventId(pMsg->event);

    if ( pMsg->event != EV_HDU_NEEDIFRAME_TIMER )
    {
        hdulog("[HduInst.%d] Receive msg.%d<%s>\n", 
            GetInsID(), pMsg->event, ::OspEventDesc(pMsg->event) );
    }
	switch (CurState())
	{
	case eIDLE:
		switch (pMsg->event)
		{
		//ͨ����ʼ��
		case EV_HDU_INIT:
			ProcInitHdu();
			break;

		default:
            OspPrintf(TRUE, FALSE,"[InstanceEntry]wrong message %d<%s> in eIDLE\n", pMsg->event, OspEventDesc(pMsg->event));
		}
		break;

	case eREADY:
        switch (pMsg->event)
        {			
		//��ʼ��ͨ����Ϣ
		case EV_HDU_INITCHNLINFO:
		    ProcInitHduChnl( pMsg );
			break;

		case MCU_HDU_CHANGEPLAYPOLICY_NOTIF:
			ProcChangePlayPolicy(pMsg);
			break;

		default:
            OspPrintf(TRUE, FALSE,"wrong message %d in eREADY\n", pMsg->event);
			break;
        }
		break;

	case eNORMAL:
        switch (pMsg->event)
        {
		//ֹͣHDU�豸
		case EV_HDU_QUIT:
			ProcStopHdu();
			break;
			
		//��ʼ����
		case MCU_HDU_START_PLAY_REQ:
			ProcStartPlayReq( cServMsg );
			break;
			
		//ֹͣ����
		case MCU_HDU_STOP_PLAY_REQ:
			ProcStopPlayReq( cServMsg );
			break;
			
		//Hdu����Mcu���ؼ�֡
		case EV_HDU_NEEDIFRAME_TIMER:
			ProcTimerNeedIFrame();
			break;
			
		//HDUͨ����Ϣ״̬��ʾ
		case EV_HDU_STATUSSHOW:
			StatusShow();
			break;
			
		//����ת��ʱ��Ҫ֪ͨ�µ����嵱ǰ����ͨ����״̬
		case EV_HDU_SENDCHNSTATUS:
			SendChnNotif(FALSE);
			break;

		//�������������ʽ�����ű���
		case MCU_HDU_CHANGEMODEPORT_NOTIF:
		case EV_HDU_SETMODE:
			ProcSetMode(pMsg);
			break;

		// ����hdu��������
		case MCU_HDU_CHANGEVOLUME_CMD:
			ProcHduChangeVolumeReq( cServMsg );
			break;
		case MCU_HDU_CHANGEPLAYPOLICY_NOTIF:
			ProcChangePlayPolicy(pMsg);
			break;
		default: 
            OspPrintf(TRUE, FALSE, "wrong message %d<%s> in eNORMAL\n",
                pMsg->event, OspEventDesc(pMsg->event));
        }

		break;

	default:
        OspPrintf(TRUE, FALSE, "[InstanceEntry]wrong state %d for msg.%d<%s> \n",
            CurState(), pMsg->event, OspEventDesc(pMsg->event));
        break;
	}

	return;
}

/*====================================================================
����  : DaemonInstanceEntry
����  : ��ͨʵ����Ϣ���
����  : CMessage* const, CApp*
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void CHduInstance::DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp )
{
	if ( NULL == pMsg || NULL == pcApp )
	{
		hdulog("[DaemonInstanceEntry] pMsg or pcApp is Null!\n");
        return;
	}
 	switch (CurState())
 	{
    case eIDLE:
		switch (pMsg->event)
		{
		//�ϵ��ʼ��
		case OSP_POWERON:
			DaemonProcPowerOn(pMsg);
	        break;

 		default:
 			OspPrintf(TRUE, FALSE, "[DaemonInstanceEntry]wrong message %d<%s> in eIDLE!\n", 
                pMsg->event, OspEventDesc(pMsg->event));
 		}
 		break;
 
 	case eREADY:
 		switch (pMsg->event)
 		{
		// ��ʼ����
		case MCU_HDU_START_PLAY_REQ:         
			DaemonProcHduStartPlay(pMsg);
			break;
			
		// ֹͣ����
		case MCU_HDU_STOP_PLAY_REQ:            
			DaemonProcHduStopPlay( pMsg );
			break;
			
		// ������������
		case MCU_HDU_CHANGEVOLUME_CMD:
			DaemonProcHduChangeVolumeReq( pMsg );
			break;

		// ����
		case EV_HDU_CONNECT:
			DaemonProcConnectTimeOut(TRUE);
			break;
			
		// ����
		case EV_HDU_CONNECTB:
			DaemonProcConnectTimeOut(FALSE);
			break;
			
		// ע��
		case EV_HDU_REGISTER:  	
			DaemonProcRegisterTimeOut(TRUE);
			break;
		// ע��
		case EV_HDU_REGISTERB:  	
			DaemonProcRegisterTimeOut(FALSE);
			break;
			
		// MCU ע��Ӧ����Ϣ
		case MCU_HDU_REG_ACK:
			DaemonProcMcuRegAck(pMsg);
			break;
			
		// MCU �ܾ�ע��
		case MCU_HDU_REG_NACK:
			DaemonProcMcuRegNack(pMsg);
			break;
			
		// ���������ʽģʽ����֪ͨ
		case MCU_HDU_CHANGEMODEPORT_NOTIF:    
			DaemonProcModePortChangeNotif(pMsg);
			break;
			
		// OSP ������Ϣ
		case OSP_DISCONNECT:
			DaemonProcOspDisconnect(pMsg);
			break;
			
		// ����Qosֵ
		case MCU_EQP_SETQOS_CMD:
			DaemonProcSetQosCmd(pMsg);
			break;
			
		// ȡ��������״̬
		case EV_HDU_GETMSSTATUS_TIMER:
		case MCU_EQP_GETMSSTATUS_ACK:
			DeamonProcGetMsStatusRsp(pMsg);
			break;
		case MCU_HDU_CHANGEPLAYPOLICY_NOTIF:
			DaemonProcChangePlayPolicy(pMsg);
			break;
		default: 
		    OspPrintf(TRUE, FALSE, "[DaemonInstanceEntry] wrong message %d<%s> in eNormal\n",
                pMsg->event, OspEventDesc(pMsg->event));

		}
		break;

 	default:
         OspPrintf(TRUE, FALSE, "[DaemonInstanceEntry]wrong status.%d for msg.%d<%s>!\n", 
             CurState(), pMsg->event, OspEventDesc(pMsg->event));
 
 	}

}
/************************************************************************/
/*                  DaemonInstance��Ϣ������                          */
/************************************************************************/

/*====================================================================
����  : DaemonProcPowerOn
����  : ��ʼ��
����  : CMessage* const��CApp*
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::DaemonProcPowerOn( CMessage* const pMsg)      
{
	if ( pMsg->length == sizeof(THduCfg) )
    {
        memcpy( &g_cHduApp.m_tCfg, pMsg->content, sizeof(THduCfg));
    }
	
    // ��ȡ�����������
    sprintf(HDUCFG_FILE, "%s/mcueqp.ini", DIR_CONFIG);
    g_cHduApp.ReadDebugValues();    
	
	g_cHduApp.m_byOutPutMode =  BrdGetHduModel();  

    g_cHduApp.m_tHduStatus.byOutputMode = g_cHduApp.m_byOutPutMode;

	s32 nRet = 0;
#ifdef _HDUDVI_
	nRet = HardMPUInit(INITMODE_HDU_D);
	hdulog("Enter HardMPUInit(INITMODE_HDU_D)!\n");
    printf("Enter HardMPUInit(INITMODE_HDU_D)!\n");
#else
	nRet = HardMPUInit(u32(INITMODE_HDU));
	hdulog("Enter HardMPUInit(INITMODE_HDU)!\n");
    printf("Enter HardMPUInit(INITMODE_HDU)!\n");
#endif

	if ( CODEC_NO_ERROR != nRet )
	{
        hdulog("HardMPUInit error %d\n", nRet);
        printf("HardMPUInit error %d\n", nRet);
	}
	else
	{
        hdulog("HardMPUInit return %d\n", nRet);
        printf("HardMPUInit return %d\n", nRet);
	}

    u16 wRet = KdvSocketStartup();
    if ( MEDIANET_NO_ERROR != wRet)
    {
        OspPrintf(TRUE, FALSE, "KdvSocketStartup failed, error: %d\n", wRet);
        return ;
    }

	NextState(u32(eREADY));

	// ��ʼ��MCUע��
    if (TRUE == g_cHduApp.m_bEmbedMcu)
    {
        hdulog( "the hdu embed in Mcu A\n");
        g_cHduApp.m_dwMcuNode = 0;                          //��Ƕʱ�����ڵ����Ϊ0�Ϳ���
        SetTimer(EV_HDU_REGISTER, HDU_REGISTER_TIMEOUT );   //ֱ��ע��
    }
    
    if( TRUE == g_cHduApp.m_bEmbedMcuB)
    {
        hdulog("the hdu embed in Mcu B\n");
        g_cHduApp.m_dwMcuNodeB = 0;
        SetTimer(EV_HDU_REGISTERB, HDU_CONNETC_TIMEOUT );  //ֱ��ע��
    } 
    
    if(0 != g_cHduApp.m_tCfg.dwConnectIP && FALSE == g_cHduApp.m_bEmbedMcu)
    {
        SetTimer(EV_HDU_CONNECT, HDU_CONNETC_TIMEOUT );     //3s���ٽ���
    }
    if(0 != g_cHduApp.m_tCfg.dwConnectIpB && FALSE == g_cHduApp.m_bEmbedMcuB)
    {
        SetTimer(EV_HDU_CONNECTB, HDU_CONNETC_TIMEOUT );    //3s���ٽ���
    }
    
    return;
}
/*====================================================================
����  : DaemonProcHduStartPlay
����  : ��ʼ��
����  : CMessage* const��CApp*
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::DaemonProcHduStartPlay( CMessage* const pMsg )
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
    post(MAKEIID(GetAppID(), cServMsg.GetChnIndex()+1), MCU_HDU_START_PLAY_REQ, pMsg->content,pMsg->length);
	return;

}

/*====================================================================
����  : DaemonProcHduStopPlay
����  : ��ʼ��
����  : CMessage* const��CApp*
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::DaemonProcHduStopPlay( CMessage* const pMsg )
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
    post(MAKEIID(GetAppID(), cServMsg.GetChnIndex()+1), MCU_HDU_STOP_PLAY_REQ, pMsg->content,pMsg->length);
	return;

}

/*====================================================================
����  : DaemonProcOspDisconnect
����  : ��������
����  : CMessage* const��CApp*
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::DaemonProcOspDisconnect( CMessage* const pMsg) 
{
	if ( NULL == pMsg )  
    {
        hdulog("[DaemonProcOspDisconnect] message's pointer is Null\n");
        return;
    }

    u32 dwNode = *(u32*)pMsg->content;

    if (INVALID_NODE != dwNode)
    {
        OspDisconnectTcpNode(dwNode);
    } 
    if(dwNode == g_cHduApp.m_dwMcuNode) // ����
    {
        hdulog("[DaemonProcOspDisconnect] McuNode.A disconnect\n");
        g_cHduApp.FreeStatusDataA();
        SetTimer(EV_HDU_CONNECT, HDU_CONNETC_TIMEOUT);
    }
    else if(dwNode == g_cHduApp.m_dwMcuNodeB)
    {
        hdulog("[DaemonProcOspDisconnect] McuNode.B disconnect\n");
        g_cHduApp.FreeStatusDataB();
        SetTimer(EV_HDU_CONNECTB, HDU_CONNETC_TIMEOUT);
    }
	// ������һ����������Mcuȡ��������״̬���ж��Ƿ�ɹ�
	if (INVALID_NODE != g_cHduApp.m_dwMcuNode || INVALID_NODE != g_cHduApp.m_dwMcuNodeB)
	{
		if (OspIsValidTcpNode(g_cHduApp.m_dwMcuNode))
		{
			post( g_cHduApp.m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cHduApp.m_dwMcuNode );           
			hdulog("[DaemonProcOspDisconnect] GetMsStatusReq. McuNode.A\n");
		}
		else if (OspIsValidTcpNode(g_cHduApp.m_dwMcuNodeB))
		{
			post( g_cHduApp.m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cHduApp.m_dwMcuNodeB );        
			hdulog("[DaemonProcMcuDisconnect] GetMsStatusReq. McuNode.B\n");
		}
		
		SetTimer(EV_HDU_GETMSSTATUS_TIMER, WAITING_MSSTATUS_TIMEOUT);
		return;
	}

	if (INVALID_NODE == g_cHduApp.m_dwMcuNode && INVALID_NODE == g_cHduApp.m_dwMcuNodeB)
	{
		// ֹͣHDU
		for (u8 byLoop = 1; byLoop <= MAXNUM_HDU_CHANNEL; byLoop++ )
		{
			post(MAKEIID(GetAppID(), byLoop), EV_HDU_QUIT);
		}
	}
    //NextState(eIDLE);1

    return;
}

/*====================================================================
����  : DaemonProcConnectTimeOut
����  : ��MCU����
����  : BOOL32 ��MCUA or MCUB
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::DaemonProcConnectTimeOut( BOOL32 bIsConnectA )   
{
	BOOL32 bRet = FALSE;
    if(TRUE == bIsConnectA)
    {
        bRet = ConnectMcu(bIsConnectA, g_cHduApp.m_dwMcuNode);
        if(TRUE == bRet)
        {  
            SetTimer(EV_HDU_REGISTER, HDU_REGISTER_TIMEOUT ); 
        }
        else
        {
            SetTimer(EV_HDU_CONNECT, HDU_CONNETC_TIMEOUT);   //3s���ٽ���
        }
    }
    else
    {
        bRet = ConnectMcu(bIsConnectA, g_cHduApp.m_dwMcuNodeB);
        if(TRUE == bRet)
        { 
            SetTimer(EV_HDU_REGISTERB, HDU_REGISTER_TIMEOUT ); 
        }
        else
        {
            SetTimer(EV_HDU_CONNECTB, HDU_CONNETC_TIMEOUT);   //3s���ٽ���
        }
    }

    return;
}

/*====================================================================
����  : DaemonProcRegisterTimeOut
����  : ��MCUע��
����  : BOOL32 ע�� MCUA or MCUB
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::DaemonProcRegisterTimeOut( BOOL32 bIsRegiterA )       
{
    if(TRUE == bIsRegiterA)
    {
        Register(g_cHduApp.m_dwMcuNode);
        SetTimer(EV_HDU_REGISTER, HDU_REGISTER_TIMEOUT);  //ֱ��ע��
    }
    else
    {
        Register(g_cHduApp.m_dwMcuNodeB);
		SetTimer(EV_HDU_REGISTERB, HDU_REGISTER_TIMEOUT);  //ֱ��ע��
    }
    return;
}

/*====================================================================
����  : DaemonProcMcuRegAck
����  : ����ע��ACK��Ӧ
����  : CMessage* const, CApp*
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::DaemonProcMcuRegAck(CMessage* const pMsg)      
{
    if(NULL == pMsg)
    {
        hdulog("Recv Reg Ack Msg, but the msg's pointer is Null\n");
        return;
    }
	
	TPeriEqpRegAck *ptRegAck = NULL;
	TEqpHduCfgInfo *ptCfgInfo = NULL;

	CServMsg cServMsg(pMsg->content, pMsg->length);
	ptRegAck = (TPeriEqpRegAck*)cServMsg.GetMsgBody();
	if (NULL == ptRegAck)
	{
		hdulog("[DaemonProcMcuRegAck] ptRegAck is null!\n");
		return;
	}

	ptCfgInfo = (TEqpHduCfgInfo*)(cServMsg.GetMsgBody() + sizeof(TPrsTimeSpan) + sizeof(TPeriEqpRegAck));
    if (NULL == ptCfgInfo)
    {
		hdulog("ptCfgInfo is Null!\n");
		return;
    }
	THduChnlModePort tHduChnModePort;
	THduModePort tModePort;
	memset(&tModePort, 0x0, sizeof(tModePort));
	memset(&tHduChnModePort, 0x0, sizeof(tHduChnModePort));
	ptCfgInfo->GetHduChnlModePort(0, tHduChnModePort);
    tModePort.SetOutModeType(tHduChnModePort.GetOutModeType());
	tModePort.SetOutPortType(tHduChnModePort.GetOutPortType());
	tModePort.SetZoomRate(tHduChnModePort.GetZoomRate());
    g_cHduApp.m_tCfg.SetHduModePort(0, tModePort);
	ptCfgInfo->GetHduChnlModePort(1, tHduChnModePort);
    tModePort.SetOutModeType(tHduChnModePort.GetOutModeType());
	tModePort.SetOutPortType(tHduChnModePort.GetOutPortType());
	tModePort.SetZoomRate(tHduChnModePort.GetZoomRate());
    g_cHduApp.m_tCfg.SetHduModePort(1, tModePort);
    
	g_cHduApp.m_tCfg.wRcvStartPort = ptCfgInfo->GetEqpStartPort();
    memcpy(g_cHduApp.m_tCfg.achAlias, ptCfgInfo->GetAlias(), MAXLEN_EQP_ALIAS);
	
	THduModePort tModePort1, tModePort2;
	memset(&tModePort1, 0x0, sizeof(tModePort1));
	memset(&tModePort2, 0x0, sizeof(tModePort2));
    g_cHduApp.m_tCfg.GetHduModePort(0, tModePort1);
    g_cHduApp.m_tCfg.GetHduModePort(1, tModePort2);

	printf("[DaemonProcMcuRegAck] m_byOutPortType1 = %d, m_byOutModeType1 = %d\nm_byOutPortType2 = %d, m_byOutModeType2 = %d\n",
		tModePort1.GetOutPortType(), tModePort1.GetOutModeType(),
		tModePort2.GetOutPortType(), tModePort2.GetOutModeType()
		);
		

	if(pMsg->srcnode == g_cHduApp.m_dwMcuNode)     
    {
		g_cHduApp.m_dwMcuIId = pMsg->srcid;
        g_cHduApp.m_byRegAckNum++;
        KillTimer(EV_HDU_REGISTER);                
		
        hdulog("[RegAck] Regist success to mcu A\n");
    }
    else if(pMsg->srcnode == g_cHduApp.m_dwMcuNodeB)
    {
        g_cHduApp.m_dwMcuIIdB = pMsg->srcid;
        g_cHduApp.m_byRegAckNum++;
		
        KillTimer(EV_HDU_REGISTERB);
		
        hdulog("[RegAck] Regist success to mcu B\n");       
    }
	
	// У��Ự����
    if ( g_cHduApp.m_dwMpcSSrc == 0 )
    {
        g_cHduApp.m_dwMpcSSrc = ptRegAck->GetMSSsrc();
    }
    else
    {
        // �쳣������Ͽ������ڵ�
        if ( g_cHduApp.m_dwMpcSSrc != ptRegAck->GetMSSsrc() )
        {
            OspPrintf(TRUE, FALSE, "[RegAck] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
				g_cHduApp.m_dwMpcSSrc, ptRegAck->GetMSSsrc());
            if ( OspIsValidTcpNode(g_cHduApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cHduApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cHduApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cHduApp.m_dwMcuNodeB);
            }      
            return;
        }
    }

    /*
    // ����������
    for (u8 byLoop = 1; byLoop <= MAXNUM_HDU_CHANNEL; byLoop++ )
    {
        post(MAKEIID(GetAppID(), byLoop), EV_HDU_INIT, NULL, 0);
	}*/

	// ��ʼ��ͨ����Ϣ
	if ( g_cHduApp.m_byRegAckNum == FIRST_REGISTERACK )
	{
        g_cHduApp.m_tHduStatus.byChnnlNum = g_cHduApp.m_bMaxChnNum;
        TPrsTimeSpan *ptPrsTime = NULL;
        ptPrsTime = (TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck));
        g_cHduApp.m_tPrsTimeSpan.m_wFirstTimeSpan  = ptPrsTime->m_wFirstTimeSpan; 
        g_cHduApp.m_tPrsTimeSpan.m_wSecondTimeSpan = ptPrsTime->m_wSecondTimeSpan; 
        g_cHduApp.m_tPrsTimeSpan.m_wThirdTimeSpan  = ptPrsTime->m_wThirdTimeSpan; 
        g_cHduApp.m_tPrsTimeSpan.m_wRejectTimeSpan = ptPrsTime->m_wRejectTimeSpan; 
        printf("[DaemonProcMcuRegAck]m_wFirstTimeSpan = %d\nm_wSecondTimeSpan = %d\nm_wThirdTimeSpan = %d\nm_wRejectTimeSpan = %d",
            ptPrsTime->m_wFirstTimeSpan,
            ptPrsTime->m_wSecondTimeSpan,
            ptPrsTime->m_wThirdTimeSpan,
            ptPrsTime->m_wRejectTimeSpan);

        // ����������
        for (u8 byLoop = 1; byLoop <= MAXNUM_HDU_CHANNEL; byLoop++ )
        {
            post(MAKEIID(GetAppID(), byLoop), EV_HDU_INIT, NULL, 0);
	    }

		for (u8 byIdx = 1; byIdx <= MAXNUM_HDU_CHANNEL; byIdx++ )
		{
			post(MAKEIID(GetAppID(), byIdx), EV_HDU_INITCHNLINFO, ptPrsTime, sizeof(TPrsTimeSpan));
		}

	}
    
	StatusNotify();    //״̬֪ͨ
	
    for (u8 byLoop2 = 1; byLoop2 <= MAXNUM_HDU_CHANNEL; byLoop2++ )
    {
        post(MAKEIID(GetAppID(), byLoop2), EV_HDU_STATUSSHOW);
		//����ת��ʱ��Ҫ֪ͨ�µ����嵱ǰ����ͨ����״̬
		post(MAKEIID(GetAppID(), byLoop2), EV_HDU_SENDCHNSTATUS);
    }

    return;
}
/*====================================================================
����  : DaemonProcMcuRegNack
����  : ��ʽ�ı�֪ͨ
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::DaemonProcHduChangeVolumeReq( CMessage* const pMsg )
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
    
	THduVolumeInfo *ptHduVolumeInfo = (THduVolumeInfo*)cServMsg.GetMsgBody();

	post(MAKEIID(GetAppID(), ptHduVolumeInfo->GetChnlIdx()+1), pMsg->event, pMsg->content,pMsg->length);

	return;

}

/*====================================================================
����  : DaemonProcMcuRegNack
����  : ��ʽ�ı�֪ͨ
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::DaemonProcModePortChangeNotif( CMessage* const pMsg )  
{
	if ( NULL == pMsg)
	{
		OspPrintf(TRUE, FALSE, "[hdu] DaemonProcModePortChangeNotif param if Null!\n");
		return;
	}
	THduChnlModePort *ptHduChnModePort = (THduChnlModePort*)pMsg->content;
    THduModePort tModePort;
    for (u8 byLoop=0; byLoop<MAXNUM_HDU_CHANNEL; byLoop++)
    {
		g_cHduApp.m_tCfg.GetHduModePort(byLoop, tModePort);
		if ( (tModePort.GetOutModeType() != ptHduChnModePort[byLoop].GetOutModeType())
			 || (tModePort.GetOutPortType() != ptHduChnModePort[byLoop].GetOutPortType())
			 || (tModePort.GetZoomRate() != ptHduChnModePort[byLoop].GetZoomRate()) )
		{
			tModePort.SetOutModeType(ptHduChnModePort[byLoop].GetOutModeType());
			tModePort.SetOutPortType(ptHduChnModePort[byLoop].GetOutPortType());
			tModePort.SetZoomRate(ptHduChnModePort[byLoop].GetZoomRate());
            g_cHduApp.m_tCfg.SetHduModePort(byLoop, tModePort);
			post(MAKEIID(GetAppID(), byLoop+1), MCU_HDU_CHANGEMODEPORT_NOTIF, &tModePort,sizeof(THduModePort));
		}
		else
		{
			hdulog("hdu chn%d's mode and port have not changed!\n", byLoop);
		}

    }

	return;
}      

/*====================================================================
����  : DaemonProcMcuRegNack
����  : ����ע��NACK��Ӧ
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::DaemonProcMcuRegNack(CMessage* const pMsg)               
{
	if( pMsg->srcnode == g_cHduApp.m_dwMcuNode)    
    {
        hdulog("Hdu registe be refused by A.\n");
    }
    if( pMsg->srcnode == g_cHduApp.m_dwMcuNodeB)    
    {
        hdulog("Hdu registe be refused by B.\n");
    }

    return;
}

/*====================================================================
����  : DeamonProcGetMsStatusRsp
����  : ȡ��������״̬
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::DeamonProcGetMsStatusRsp( CMessage* const pMsg )         
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg(pMsg->content, pMsg->length);
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event )
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer(EV_HDU_GETMSSTATUS_TIMER);
        hdulog("[DeamonProcGetMsStatusRsp]. receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
			ptMsStatus->IsMsSwitchOK());
		
        if(ptMsStatus->IsMsSwitchOK()) // �����ɹ�
        {
            bSwitchOk = TRUE;
        }
	}
		
	if( !bSwitchOk )    // ����ʧ�ܻ��߳�ʱ
	{
		//20110914 zjl ��������ʧ�� ����״̬  �ٶ��� ������
		ClearInstStatus();
		hdulog("[DeamonProcGetMsStatusRsp] ClearInstStatus!\n");

		if (OspIsValidTcpNode(g_cHduApp.m_dwMcuNode))
		{
			hdulog("[DeamonProcGetMsStatusRsp] OspDisconnectTcpNode A!\n");	
			OspDisconnectTcpNode(g_cHduApp.m_dwMcuNode);
		}
		if (OspIsValidTcpNode(g_cHduApp.m_dwMcuNodeB))
		{
			hdulog("[DeamonProcGetMsStatusRsp] OspDisconnectTcpNode B!\n");	
			OspDisconnectTcpNode(g_cHduApp.m_dwMcuNodeB);
		}

		if( INVALID_NODE == g_cHduApp.m_dwMcuNode)    // �п��ܲ�����������Connect�������
		{
			hdulog("[DeamonProcGetMsStatusRsp] EV_HDU_CONNECT!\n");	
			SetTimer(EV_HDU_CONNECT, HDU_CONNETC_TIMEOUT);
		}
		if( INVALID_NODE == g_cHduApp.m_dwMcuNodeB)
		{
			hdulog("[DeamonProcGetMsStatusRsp] EV_HDU_CONNECTB!\n");	
			SetTimer(EV_HDU_CONNECTB, HDU_CONNETC_TIMEOUT);
		}
	}

    return;
}

/*====================================================================
����  : DaemonProcSetQosCmd
����  : ����Qosֵ
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::DaemonProcSetQosCmd( CMessage* const pMsg )    
{
    if(NULL == pMsg)
    {
        hdulog("The Hdu's Qos infomation is Null\n");
        return;
    }
    TMcuQosCfgInfo  tQosInfo;
    CServMsg cServMsg(pMsg->content, pMsg->length);
    tQosInfo = *(TMcuQosCfgInfo*)cServMsg.GetMsgBody();

    u8 byQosType    = tQosInfo.GetQosType();
    u8 byAudioValue = tQosInfo.GetAudLevel();
    u8 byVideoValue = tQosInfo.GetVidLevel();
    u8 byDataValue  = tQosInfo.GetDataLevel();
    u8 byIpPriorValue = tQosInfo.GetIpServiceType();
	hdulog("Type: %d, Aud= %d, Vid= %d, Data= %d, IpPrior= %d\n", 
		    byQosType, byAudioValue, byVideoValue, byDataValue, byIpPriorValue);

    if(QOSTYPE_IP_PRIORITY == byQosType)
    {
        byAudioValue = (byAudioValue << 5);
        byVideoValue = (byVideoValue << 5);
        byDataValue  = (byDataValue << 5);
        ComplexQos(byAudioValue, byIpPriorValue);
        ComplexQos(byVideoValue, byIpPriorValue);
        ComplexQos(byDataValue,  byIpPriorValue);
    }
    else
    {
        byAudioValue = (byAudioValue << 2);
        byVideoValue = (byVideoValue << 2);
        byDataValue  = (byDataValue  << 2);
    }

	hdulog("\nAud= %d, Vid= %d, Data= %d\n", 
					byAudioValue, byVideoValue, byDataValue);
    
    /*
    SetMediaTOS((s32)byAudioValue, QOS_AUDIO);
    SetMediaTOS((s32)byVideoValue, QOS_VIDEO);
    SetMediaTOS((s32)byDataValue,  QOS_DATA );
    */
    return;
}

/************************************************************************/
/*                  Instance��Ϣ������                          */
/************************************************************************/

/*====================================================================
����  : ProcInitHdu
����  : ��ʼ��HDU
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::ProcInitHdu( )
{
	BOOL bRet = TRUE;

    u32 dwChnIdx = GetInsID()-1;
    if ( m_pDecCroup == NULL )
    {
        // start new, delete in ~CHduInstance(){}
	    m_pDecCroup = new CDecoderGrp( );
			
        u16 wRecvPort = g_cHduApp.m_tCfg.wRcvStartPort + dwChnIdx*PORTSPAN;

		hdulog("[hdu]start Create!\n");
		printf("[hdu]start Create!\n");
        bRet = m_pDecCroup->Create(dwChnIdx, 
 			                       g_cHduApp.m_dwMcuRcvIp,
				                   wRecvPort + 1,
				                   wRecvPort,
				                   (u32)this);
		//OspDelay(40);
		//OspPrintf(TRUE, FALSE, "[ProcInitHdu] OspDelay 40Ms!\n");

		if ( !bRet )
		{
			printf("[ProcInitHdu] Create failed!\n");
			return;
		}
        
		//zjl[20091224]
		m_pDecCroup->m_cVidDecoder.SetVidDecResizeMode(g_cHduApp.GetVidDecResizeMode());
		printf("VidDecResizeMode = %d\n", g_cHduApp.GetVidDecResizeMode());
   
		//�������������ʽ
		THduModePort tModePort;
		memset(&tModePort, 0x0, sizeof(tModePort));
		g_cHduApp.m_tCfg.GetHduModePort(dwChnIdx, tModePort);
		
		printf("[ProcInitHdu] Create ChnIdx%d succeed as m_byOutPortType = %d, m_byOutModeType = %d\n",
			dwChnIdx, 
			tModePort.GetOutPortType(), 
			tModePort.GetOutModeType()
			);
		
		TVidSrcInfo tVidSrcInfo;
		memset( &tVidSrcInfo, 0x0, sizeof(tVidSrcInfo) );
		bRet = GetVidSrcInfoByTypeAndMode(tModePort.GetOutPortType(), 
			tModePort.GetOutModeType(), 
			&tVidSrcInfo);
		if (!bRet)
		{
			printf("[ProcInitHdu]:GetVidSrcInfoByTypeAndMode fail\n");
			return;
		}
		hdulog("[hdu]start m_pDecCroup->SetVideoPlyInfo\n");
		printf("[hdu]start m_pDecCroup->SetVideoPlyInfo\n");
		bRet = m_pDecCroup->SetVideoPlyInfo(&tVidSrcInfo);
		if (!bRet)
		{
			printf("[ProcInitHdu] SetVideoPlyInfo failed!\n");
			return;
		}
		else
		{
			printf("[hdu]:m_pDecCroup->SetVideoPlyInfo ChnlIdx%d m_wWidth=%d, m_wHeight=%d, m_bProgressive=%d, m_dwFrameRate= %d\n", 
				dwChnIdx, tVidSrcInfo.m_wWidth, tVidSrcInfo.m_wHeight,
				tVidSrcInfo.m_bProgressive, tVidSrcInfo.m_dwFrameRate);
		}

		u16 wWidth, wHeigh;
		switch (tModePort.GetZoomRate())
		{
		case HDU_ZOOMRATE_4_3:
			wWidth = 4;
			wHeigh = 3;
			break;

		case HDU_ZOOMRATE_16_9:
			wWidth = 16;
			wHeigh = 9;
			break;

		default:
			wWidth = 0;
			wHeigh = 0;
		}

		bRet = m_pDecCroup->SetPlayScales(wWidth, wHeigh);
		if (!bRet)
		{
			printf("[ProcInitHdu]:ChnlIdx%d SetPlayScales failed!\n", dwChnIdx);
			return;
		}
		else
		{
			printf("[hdu]:m_pDecCroup->SetPlayScales ChnlIdx%d m_wWidth=%d, m_wHeight=%d\n", 
				   dwChnIdx,wWidth, wHeigh);
		}
		
		u16 wRet;
		wRet = m_pDecCroup->SetAudioVolume( HDU_VOLUME_DEFAULT );
		if (u16(Codec_Success) != wRet)
		{
			printf("[ProcInitHdu]:ChnlIdx%d SetAudioVolume failed!\n", dwChnIdx);
			return;
		}
		else
		{
			printf("[hdu]:m_pDecCroup->SetAudioVolume ChnlIdx%d successful\n", 
				dwChnIdx);
		}

		m_tChnInfo.m_tHduChnStatus.SetVolume( HDU_VOLUME_DEFAULT );

	}
	else
	{
		printf("[ProcInitHdu] ChnlIdx%d: m_pcDecGrp.0x%p is not Null!\n", dwChnIdx, m_pDecCroup);
		return;
	}
    
	NextState(u32(eREADY));

    return;
}

/*====================================================================
����  : ProcInitHduChnl
����  : ��ʼ��HDU
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void CHduInstance::ProcInitHduChnl( CMessage* const pMsg )
{	
	if (NULL == pMsg)
	{
	    hdulog("ProcInitHduChnl's pMsg is Null!\n");
	    return;
	}
	if (m_pDecCroup == NULL)
	{
		return;
	}

	//ÿ��ͨ�����涪���ش�����
	TPrsTimeSpan *ptPrsTimeSpan = (TPrsTimeSpan*)pMsg->content;
	TRSParam tPrsParam;
	memset(&tPrsParam, 0x0, sizeof(tPrsParam));
	tPrsParam.m_wFirstTimeSpan = ptPrsTimeSpan->m_wFirstTimeSpan;
	tPrsParam.m_wSecondTimeSpan = ptPrsTimeSpan->m_wSecondTimeSpan;
	tPrsParam.m_wThirdTimeSpan = ptPrsTimeSpan->m_wThirdTimeSpan;
	tPrsParam.m_wRejectTimeSpan = ptPrsTimeSpan->m_wRejectTimeSpan;
	m_pDecCroup->SetRsParam(tPrsParam);

    hdulog("[ProcInitHduChnl]m_wFirstTimeSpan = %d\nm_wSecondTimeSpan = %d\nm_wThirdTimeSpan = %d\nm_wRejectTimeSpan = %d",
            ptPrsTimeSpan->m_wFirstTimeSpan,
            ptPrsTimeSpan->m_wSecondTimeSpan,
            ptPrsTimeSpan->m_wThirdTimeSpan,
            ptPrsTimeSpan->m_wRejectTimeSpan);

    SendChnNotif(TRUE);
	NextState(u32(eNORMAL));

	printf("[hdu] start SetTimer EV_HDU_NEEDIFRAME_TIMER\n");
    SetTimer(EV_HDU_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);   // ��ʱ��ѯ�������Ƿ���Ҫ�ؼ�֡

	return;
}

/*====================================================================
����  : ProcHduChangeVolumeReq
����  : ������������
����  : CServMsg &cServMsg
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::ProcHduChangeVolumeReq( CServMsg &cServMsg )
{
	if (m_pDecCroup == NULL)
	{
		return;
	}
    THduVolumeInfo tHduVolumeInfo = *( THduVolumeInfo* )cServMsg.GetMsgBody();
	
    printf("[ProcHduChangeVolumeReq] Volume = %u\n", tHduVolumeInfo.GetVolume());
    hdulog("[ProcHduChangeVolumeReq] Volume = %u\n", tHduVolumeInfo.GetVolume());

	printf("[ProcHduChangeVolumeReq] IsMute = %u\n", tHduVolumeInfo.GetIsMute());
    hdulog("[ProcHduChangeVolumeReq] IsMute = %u\n", tHduVolumeInfo.GetIsMute());
	u16 wRet;
	wRet = m_pDecCroup->m_cAudDecoder.SetAudioVolume( tHduVolumeInfo.GetVolume() );
	if (u16(Codec_Success) != wRet)
	{
		hdulog( "[ProcHduChangeVolumeReq]SetAudioVolume fail!\n" );
	}
	else
	{
	    m_tChnInfo.m_tHduChnStatus.SetVolume( tHduVolumeInfo.GetVolume() );
		hdulog( "[ProcHduChangeVolumeReq]SetAudioVolume sucessful!\n" );
	}

	wRet = m_pDecCroup->m_cAudDecoder.SetAudioMute( tHduVolumeInfo.GetIsMute() );
	if (u16(Codec_Success) != wRet)
	{
		hdulog( "[ProcHduChangeVolumeReq]SetAudioMute fail!\n" );
	}
	else
	{
		m_tChnInfo.m_tHduChnStatus.SetIsMute( tHduVolumeInfo.GetIsMute() );
//		g_cHduApp.m_bMute[GetInsID()  - 1] = m_tChnInfo.m_tHduChnStatus.GetIsMute();
		hdulog( "[ProcHduChangeVolumeReq]SetAudioMute sucessful!\n" );
	}

	SendChnNotif();

	return;
}

/*====================================================================
����  : ProcInitHdu
����  : ��ʼ��HDU
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void CHduInstance::ProcSetMode( CMessage* const pMsg )
{
	BOOL bRet;
	THduModePort tModePort, *ptModePort = NULL;
	TVidSrcInfo tVidSrcInfo;
    u16 wWidth;
	u16 wHeigh;
	u32 dwChnlIdx = GetInsID() - 1;
	memset(&tModePort, 0x0, sizeof(tModePort));
	memset( &tVidSrcInfo, 0x0, sizeof(tVidSrcInfo) );
	switch (pMsg->event)
	{
	case EV_HDU_SETMODE:
		g_cHduApp.m_tCfg.GetHduModePort(dwChnlIdx, tModePort);
		printf("[ProcSetMode] Set ChnIdx%d succeed as m_byOutPortType = %d, m_byOutModeType = %d\n",
			dwChnlIdx, 
			tModePort.GetOutPortType(), 
			tModePort.GetOutModeType()
			);

		bRet = GetVidSrcInfoByTypeAndMode(tModePort.GetOutPortType(), 
			                         tModePort.GetOutModeType(), 
			                         &tVidSrcInfo);
		if (!bRet)
		{
			printf("[ProcSetMode]:GetVidSrcInfoByTypeAndMode fail\n");
			return;
		}
        else
		{
			printf("[ProcSetMode]:GetVidSrcInfoByTypeAndMode Suceeded\n");
		}

		switch (tModePort.GetZoomRate())
		{
		case HDU_ZOOMRATE_4_3:
            wWidth = 4;
			wHeigh = 3;
			break;
		case HDU_ZOOMRATE_16_9:
			wWidth = 16;
			wHeigh = 9;
			break;
		default:
			wWidth = 0;
			wHeigh = 0;
		}
		break;

	case MCU_HDU_CHANGEMODEPORT_NOTIF:
        ptModePort = (THduModePort*)pMsg->content;
		printf("[ProcSetMode] Change ChnIdx%d succeed as m_byOutPortType = %d, m_byOutModeType = %d\n",
			dwChnlIdx, 
			tModePort.GetOutPortType(), 
			tModePort.GetOutModeType()
			);
		bRet = GetVidSrcInfoByTypeAndMode(ptModePort->GetOutPortType(), 
			ptModePort->GetOutModeType(), 
			&tVidSrcInfo);
		if ( !bRet)
		{
			printf("[ProcSetMode]:GetVidSrcInfoByTypeAndMode fail\n");
			return;
		}
		else
		{
            printf("[ProcSetMode]:GetVidSrcInfoByTypeAndMode suceeded!\n");
		}

		switch (ptModePort->GetZoomRate())
		{
		case HDU_ZOOMRATE_4_3:
            wWidth = 4;
			wHeigh = 3;
			break;
		case HDU_ZOOMRATE_16_9:
			wWidth = 16;
			wHeigh = 9;
			break;
		default:
			wWidth = 0;
			wHeigh = 0;
		}
		break;	

	default:
	    OspPrintf(TRUE, FALSE, "[hdu]ProcSetMode:Message %d is wrong!\n", pMsg->event);
		return;
	}

	hdulog("[hdu]start m_pDecCroup->SetVideoPlyInfo in ProcSetMode()\n");
	printf("[hdu]start m_pDecCroup->SetVideoPlyInfo in ProcSetMode()\n");
	bRet = m_pDecCroup->SetVideoPlyInfo(&tVidSrcInfo);
	if ( !bRet)
	{
		printf("[ProcSetMode]ChnIdx%d: SetVideoPlyInfo failed!\n", dwChnlIdx);
		return;
	}
	else
	{
		hdulog("[ProcSetMode]ChnIdx%d: m_wWidth=%d, m_wHeight=%d, m_bProgressive=%d, m_dwFrameRate= %d", 
			dwChnlIdx, tVidSrcInfo.m_wWidth, tVidSrcInfo.m_wHeight, tVidSrcInfo.m_bProgressive, tVidSrcInfo.m_dwFrameRate);
	}
	
 	bRet = m_pDecCroup->SetPlayScales( wWidth, wHeigh );
 	if (!bRet)
 	{
 		printf("[ProcSetMode]ChnIdx%d: SetPlayScale fail\n", dwChnlIdx);
 		return;
 	}
 	else
 	{
 		printf("[ProcSetMode]ChnIdx%d: wWidth = %d, wHeigh = %d\n", dwChnlIdx, wWidth, wHeigh);
 	}

//	SetTimer( EV_HDU_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL );

    return;
}

/*====================================================================
����  : ProcStopHdu
����  : ֹͣHDU�豸
����  : void
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::ProcStopHdu(void)                          
{
	m_tChnInfo.m_tHduChnStatus.SetIsMute( 0 );
	m_tChnInfo.m_tHduChnStatus.SetVolume( HDU_VOLUME_DEFAULT );
	KillTimer(EV_HDU_NEEDIFRAME_TIMER);
	NextState(u32(eREADY));
    SendChnNotif();
	ClearInstStatus();
	return;
}

/*====================================================================
����  : ProcStartPlayReq
����  : ��ʼ����
����  : CServMsg&
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
=====================================================================*/
void  CHduInstance::ProcStartPlayReq( CServMsg &cServMsg )
{
	if (m_pDecCroup == NULL)
	{
		return;
	}
    u8 byChnIdx = cServMsg.GetChnIndex();

	m_tChnInfo.m_cChnConfId = cServMsg.GetConfId();
	THduModePort tModePort;
    g_cHduApp.m_tCfg.GetHduModePort( byChnIdx, tModePort );
	hdulog("[ProcStartPlayReq] enter start play hdu as mode.<PortType.%d, ModeType.%d>!\n", 
		tModePort.GetOutPortType(), tModePort.GetOutModeType() );
	
	if ( byChnIdx != GetInsID()-1 )    //ͨ�������Ŵ�0��ʼ
	{
		cServMsg.SetErrorCode(ERR_HDU_INVALID_CHANNL);
		SendMsgToMcu(HDU_MCU_START_PLAY_NACK, cServMsg);
		hdulog("[CHduInstance]:ProcStartPlayReq -->byChnIdx too big\n");
		return;
	}

    TMt tSrcMt;
    THduStartPlay tHduStartPlay;
    TMediaEncrypt    tMediaEncrypt;
    TDoublePayload   tDoublePayload, tAudPayLoad;

    tHduStartPlay = *(THduStartPlay*)cServMsg.GetMsgBody();
    tDoublePayload   = *(TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(THduStartPlay));
    hdulog("m_byRealPayload = %d, m_byActivePayload = %d\n", tDoublePayload.GetRealPayLoad(), tDoublePayload.GetActivePayload());

    if( HDU_OUTPUTMODE_BOTH == tHduStartPlay.GetMode() )
    {
        tAudPayLoad = *(TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(THduStartPlay) + sizeof(TDoublePayload));
    }

	TCapSupportEx tCapSupportEx = *(TCapSupportEx*)(cServMsg.GetMsgBody() + sizeof(THduStartPlay) + sizeof(TDoublePayload)*2 );
	if( g_nHduPLog > 0 )
	{
		tCapSupportEx.Print();
	}
	
	//  [1/13/2010 pengjie] Modify HDU����ͨ����ʾ����
	THDUExCfgInfo tHDUExCfgInfo;
	if( cServMsg.GetMsgBodyLen() > sizeof(THduStartPlay) + sizeof(TDoublePayload) * 2 )
	{
		tHDUExCfgInfo = *(THDUExCfgInfo *)(cServMsg.GetMsgBody() + sizeof(THduStartPlay) +
			sizeof(TDoublePayload)*2 );	
	}
	else
	{
		tHDUExCfgInfo.m_byIdleChlShowMode = VMP_SHOW_BLACK_MODE;
	}
	
	if( tHDUExCfgInfo.m_byIdleChlShowMode > HDU_SHOW_USERDEFPIC_MODE )
	{
		tHDUExCfgInfo.m_byIdleChlShowMode = VMP_SHOW_BLACK_MODE; // Ĭ����ʾ����
	}
	hdulog( "ProcStartPlayReq, Set HDU IdleChlShowMode: %d \n", tHDUExCfgInfo.m_byIdleChlShowMode );
	
	TTransportAddr tAudRemoteAddr;
	tAudRemoteAddr.SetNull();

	if (cServMsg.GetMsgBodyLen() > (sizeof(THduStartPlay) + sizeof(TDoublePayload) * 2 + sizeof(THDUExCfgInfo)) &&
		tHduStartPlay.IsNeedByPrs())
	{
		tAudRemoteAddr = *(TTransportAddr *)(cServMsg.GetMsgBody() + sizeof(THduStartPlay) +
											 sizeof(TDoublePayload)*2 + sizeof(THDUExCfgInfo));
		hdulog("[ProcStartPlayReq] AudRemoteAddr<%x, %d>!\n", tAudRemoteAddr.GetIpAddr(), tAudRemoteAddr.GetPort());
	}

	s32 nReturn = 0;
	
	// ���ײ����ÿ���ͨ����ʾģʽ
	if(	NULL != m_pDecCroup )
	{
		if( HDU_SHOW_BLACK_MODE == tHDUExCfgInfo.m_byIdleChlShowMode )
 		{
 			nReturn = m_pDecCroup->m_cVidDecoder.SetNoStreamBak(u32(NoStrm_PlyBlk));
 		}
 		else if( HDU_SHOW_LASTFRAME_MODE == tHDUExCfgInfo.m_byIdleChlShowMode )
 		{
 			nReturn = m_pDecCroup->m_cVidDecoder.SetNoStreamBak(u32(NoStrm_PlyLst));
 		}
 		else if( HDU_SHOW_DEFPIC_MODE == tHDUExCfgInfo.m_byIdleChlShowMode )
 		{
 			nReturn = m_pDecCroup->m_cVidDecoder.SetNoStreamBak(u32(NoStrm_PlyBmp_dft));
 		}
 		else if( HDU_SHOW_USERDEFPIC_MODE == tHDUExCfgInfo.m_byIdleChlShowMode )// �û��Զ���ͼƬ
 		{
 			nReturn = m_pDecCroup->m_cVidDecoder.SetNoStreamBak(u32(NoStrm_PlyBmp_usr));
 		}
		hdulog( "ProcStartPlayReq, SetNoStreamBak return: %d ,mode: %d\n", nReturn, tHDUExCfgInfo.m_byIdleChlShowMode );
	}
 	
	// End Modify

    tSrcMt = tHduStartPlay.GetMt();

    tMediaEncrypt = tHduStartPlay.GetVideoEncrypt();

    hdulog("********************************************************\n");
    hdulog("tSrcMt.GetMcuId() = %d,tSrcMt.GetEqpId() = %d\n",
                        tSrcMt.GetMcuId(),
						tSrcMt.GetEqpId());
    hdulog("GetRealPayLoad() = %d, GetActivePayload() = %d\n",
                        tDoublePayload.GetRealPayLoad(), 
                        tDoublePayload.GetActivePayload());
    s32 nLen;
    u8  achKey[20];
    memset((s8*)achKey, '\0', sizeof(achKey));
    tMediaEncrypt.GetEncryptKey(achKey, &nLen);
    hdulog("GetEncryptMode() = %d, key = %s, len = %d\n",
                        tMediaEncrypt.GetEncryptMode(), achKey, nLen);
    hdulog("********************************************************\n");

    // ���ü��ܲ���
    SetEncryptParam(&tMediaEncrypt, &tDoublePayload);
    if( HDU_OUTPUTMODE_BOTH == tHduStartPlay.GetMode() || 
		HDU_OUTPUTMODE_AUDIO == tHduStartPlay.GetMode() )
    {
        SetAudEnctypt(&tMediaEncrypt, &tAudPayLoad); // ��Ƶ����
    }
    //�趪���ش�����
    TRSParam tRsParam;
    memset(&tRsParam, 0x0, sizeof(tRsParam));
    m_pDecCroup->GetRsParam(tRsParam);

    if (tHduStartPlay.IsNeedByPrs())
    {	
        m_pDecCroup->SetNetRecvFeedbackVideoParam(tRsParam, TRUE);
		m_pDecCroup->SetNetRecvFeedbackAudioParam(tRsParam, TRUE);
    }
    else
    {		
        m_pDecCroup->SetNetRecvFeedbackVideoParam(tRsParam, FALSE);
		m_pDecCroup->SetNetRecvFeedbackAudioParam(tRsParam, FALSE);
	}

    BOOL bRet;
    u32 dwRtcpBackIp;
    u16 wRtcpBackPort;
    TLocalNetParam tNetParm;
    

    //��Ƶ
    if( HDU_OUTPUTMODE_BOTH  == g_cHduApp.m_byOutPutMode || 
        HDU_OUTPUTMODE_VIDEO == g_cHduApp.m_byOutPutMode )
    {
		tHduStartPlay.GetVidRtcpBackAddr(dwRtcpBackIp, wRtcpBackPort);
        memset(&tNetParm, 0, sizeof(tNetParm));
        tNetParm.m_tLocalNet.m_dwRTPAddr  = g_cHduApp.m_tCfg.dwLocalIP;
        tNetParm.m_tLocalNet.m_wRTPPort   = g_cHduApp.m_tCfg.wRcvStartPort + byChnIdx*PORTSPAN;
        tNetParm.m_tLocalNet.m_dwRTCPAddr = g_cHduApp.m_tCfg.dwLocalIP;
        tNetParm.m_tLocalNet.m_wRTCPPort  = g_cHduApp.m_tCfg.wRcvStartPort + 1 + byChnIdx*PORTSPAN;
        tNetParm.m_dwRtcpBackAddr = dwRtcpBackIp;
        tNetParm.m_wRtcpBackPort  = wRtcpBackPort;

        bRet = m_pDecCroup->SetVidNetRcvLocalParam(&tNetParm);
        if (!bRet)
        {
            hdulog("[CHduInstance::ProcStartPlayReq] m_pDecCroup->SetVidNetRcvLocalParam failed \n");
            return;
        }
    }

    // ��Ƶ
    if( HDU_OUTPUTMODE_BOTH  == g_cHduApp.m_byOutPutMode || 
        HDU_OUTPUTMODE_AUDIO == g_cHduApp.m_byOutPutMode )
    {
		//tHduStartPlay.GetAudRtcpBackAddr(dwRtcpBackIp, wRtcpBackPort);
        memset(&tNetParm, 0, sizeof(tNetParm));
        tNetParm.m_tLocalNet.m_dwRTPAddr  = g_cHduApp.m_tCfg.dwLocalIP;
        tNetParm.m_tLocalNet.m_wRTPPort   = g_cHduApp.m_tCfg.wRcvStartPort + 2 + byChnIdx*PORTSPAN;
        tNetParm.m_tLocalNet.m_dwRTCPAddr = g_cHduApp.m_tCfg.dwLocalIP;
        tNetParm.m_tLocalNet.m_wRTCPPort  = g_cHduApp.m_tCfg.wRcvStartPort + 3 + byChnIdx*PORTSPAN;
        tNetParm.m_dwRtcpBackAddr = tAudRemoteAddr.GetIpAddr();
        tNetParm.m_wRtcpBackPort  = tAudRemoteAddr.GetPort();

        bRet = m_pDecCroup->SetAudNetRcvLocalParam(&tNetParm); 
        if (!bRet)
        {
            hdulog("[CHduInstance::ProcStartPlayReq] m_pDecCroup->SetAudNetRcvLocalParam failed \n");
            return;
        }
    }

    if (Play( tHduStartPlay.GetMode() ))
    {
        g_cHduApp.m_tHduStatus.atVideoMt[byChnIdx].SetMt(tSrcMt);
        SendMsgToMcu(HDU_MCU_START_PLAY_ACK, cServMsg);
    }
    else
    {
        cServMsg.SetErrorCode(ERR_HDU_STARTPLAY_FAIL);
        SendMsgToMcu(HDU_MCU_START_PLAY_NACK, cServMsg);
    }

	return;
}

/*====================================================================
����  : ProcStopPlayReq
����  : ֹͣ����
����  : CServMsg&
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼ ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::ProcStopPlayReq( CServMsg& cServMsg)   		  
{
	// ����ֹͣ���ŵ�ͨ����
	u8 byChnIdx = cServMsg.GetChnIndex(); 
	if ( byChnIdx != GetInsID()-1 )    //ͨ�������Ŵ�0��ʼ
	{
        hdulog("[CHduInstance]:ProcStopPlayReq -->byChnIdx too big\n");
		cServMsg.SetErrorCode(ERR_HDU_INVALID_CHANNL);
		SendMsgToMcu(HDU_MCU_STOP_PLAY_NACK, cServMsg);
		return;
	}

	if (Stop())
	{
        OspPrintf(TRUE, FALSE, "[CHduInstance] Stop channel: %d success\n", byChnIdx); 
		g_cHduApp.m_tHduStatus.atVideoMt[byChnIdx].SetMcu(0);
		SendMsgToMcu(HDU_MCU_STOP_PLAY_ACK, cServMsg);
	}
	else
	{
		cServMsg.SetErrorCode(ERR_HDU_STOPPLAY_FAIL);
		SendMsgToMcu(HDU_MCU_STOP_PLAY_NACK, cServMsg);
		OspPrintf(TRUE, FALSE, "[CHduInstance]Chn.%d Call Stop failed!\n", byChnIdx);
		return;
	}

	return;
}

/*====================================================================
����  : ProcTimerNeedIFrame
����  : Hdu����Mcu���ؼ�֡
����  : void
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void  CHduInstance::ProcTimerNeedIFrame(void)                  
{
	CServMsg cServMsg;    
    TKdvDecStatis tDecChannelStatis;    
    memset( &tDecChannelStatis, 0, sizeof(TKdvDecStatis) );

	if( (NULL != m_pDecCroup) && m_pDecCroup->IsStart() )
	{  
		BOOL bRet;
        bRet = m_pDecCroup->GetVidDecoderStatis(tDecChannelStatis);     // ��Ƶͨ���̶�Ϊ0
        if (!bRet)
        {
			hdulog("[proctimerneediframe] GetVidDecoderStatis fail!\n");
			printf("[proctimerneediframe] GetVidDecoderStatis fail!\n");
        }

		if ( tDecChannelStatis.m_bVidCompellingIFrm )
		{
            cServMsg.SetConfId( m_tChnInfo.m_cChnConfId);
            cServMsg.SetChnIndex( GetInsID()-1 );
            cServMsg.SetSrcSsnId( g_cHduApp.m_tCfg.byEqpId );
            SendMsgToMcu( HDU_MCU_NEEDIFRAME_CMD, cServMsg );
			
			hdulog("[ProcTimerNeedIFrame]Hdu channel:%d request iframe!!\n", GetInsID() - 1);
		}

	}
		
    SetTimer( EV_HDU_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL );     //��ʱ���

	return;
}

/************************************************************************/
/*                              ���ܺ���                                */
/************************************************************************/

/*====================================================================
����  : ConnectMcu
����  : ��MCU
����  : BOOL32 -->��MCUA or MCUB, u32 --> MCU�ڵ��
���  : MCU�ڵ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL32 CHduInstance::ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode )    
{
	BOOL32 bRet = TRUE;
    if(!OspIsValidTcpNode(dwMcuNode)) //�ڵ���Ч��ֱ�ӽ����ɹ�
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
			hdulog("Connect Mcu %s Success, node: %d!\n", 
				bIsConnectA ? "A" : "B", dwMcuNode);
			::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), CHduInstance::DAEMON);// ������Ϣ����
		}
		else 
		{
			hdulog("Connect to Mcu failed, will retry\n");
            bRet = FALSE;
		}
    }
    return bRet;
}

/*====================================================================
����  : Register
����  : ע��
����  : BOOL32 -->ע��MCUA or MCUB, u32 --> MCU�ڵ��
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void   CHduInstance::Register(u32 dwMcuNode )       
{
	CServMsg       cSvrMsg;
    TPeriEqpRegReq tReg;
	
    tReg.SetMcuEqp((u8)g_cHduApp.m_tCfg.wMcuId, g_cHduApp.m_tCfg.byEqpId, g_cHduApp.m_tCfg.byEqpType);
    tReg.SetPeriEqpIpAddr(htonl(g_cHduApp.m_tCfg.dwLocalIP));
	tReg.SetChnnlNum(g_cHduApp.m_bMaxChnNum);     //���ŵ����������ļ����
    tReg.SetVersion(DEVVER_HDU);
	tReg.SetStartPort(HDU_EQP_STARTPORT);
    tReg.SetHDEqp(TRUE);
    
    cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
	
    post(MAKEIID(AID_MCU_PERIEQPSSN, g_cHduApp.m_tCfg.byEqpId),
		HDU_MCU_REG_REQ,
		cSvrMsg.GetServMsg(),
		cSvrMsg.GetServMsgLen(),
        dwMcuNode);
	return;
}

/*====================================================================
����  : SendMsgToMcu
����  : ��MCU������Ϣͨ�ú���
����  : u16 -->��Ϣ��, CServMsg&
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL32 CHduInstance::SendMsgToMcu( u16 wEvent, CServMsg& cServMsg ) 
{
	if (GetInsID() != CInstance::DAEMON)
    {
        cServMsg.SetChnIndex((u8)GetInsID() - 1);
    }
    
	BOOL32 bRet = TRUE;
    if (g_cHduApp.m_bEmbedMcu || OspIsValidTcpNode(g_cHduApp.m_dwMcuNode)) 
	{
		post(g_cHduApp.m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cHduApp.m_dwMcuNode);
		hdulog("Send Message %u(%s) to Mcu A\n",
			wEvent, ::OspEventDesc(wEvent));
	}
	else
	{
		bRet = FALSE;
	}
	
    if(g_cHduApp.m_bEmbedMcuB || OspIsValidTcpNode(g_cHduApp.m_dwMcuNodeB))
    {
		post(g_cHduApp.m_dwMcuIIdB, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cHduApp.m_dwMcuNodeB);
		hdulog("Send Message %u(%s) to Mcu B\n",
			wEvent, ::OspEventDesc(wEvent));
    }
    else
    {
		bRet &= FALSE;
    }

	return bRet;
}

/*====================================================================
����  : SendChnNotif
����  : ״̬֪ͨ
����  : void
���  : ��
����  : void
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void   CHduInstance::SendChnNotif( BOOL32 bFirstNotif/* = FALSE*/ )                                       
{
	 u8 byChnIdx = (u8)GetInsID() - 1;
	 
	 CServMsg cServMsg;
	 cServMsg.SetConfId(m_tChnInfo.m_cChnConfId);
	 
	 u8 bFirstNotifTmp = bFirstNotif ? 1:0;
	 TEqp tEqp;
	 tEqp.SetEqpType( g_cHduApp.m_tCfg.GetEqpType()/*EQP_TYPE_HDU*/ );
	 tEqp.SetEqpId( g_cHduApp.m_tCfg.GetEqpId() );
	 tEqp.SetMcuId( g_cHduApp.m_tCfg.GetMcuId() );
	 
	 m_tChnInfo.m_tHduChnStatus.SetEqp( tEqp );
	 m_tChnInfo.m_tHduChnStatus.SetChnIdx(byChnIdx);
	 m_tChnInfo.m_tHduChnStatus.SetStatus((u8)CurState());  
	 
	 cServMsg.SetMsgBody( (u8*)&m_tChnInfo.m_tHduChnStatus, sizeof(m_tChnInfo.m_tHduChnStatus)  );
	 cServMsg.CatMsgBody( (u8*)&bFirstNotifTmp, sizeof(bFirstNotifTmp));
	 
	 hdulog( "[SendChnNotif] Inst.%d Channel.%u state.%u\n",
		 GetInsID(),
		 byChnIdx,
		 CurState() );
	 
	 SendMsgToMcu(HDU_MCU_CHNNLSTATUS_NOTIF, cServMsg);
 	return;
}

/*====================================================================
����  : ClearInstStatus
����  : ��ձ�ʵ�������״̬��Ϣ
����  : void
���  : ��
����  : void
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void   CHduInstance::ClearInstStatus( void )
{
	// DAEMON ʵ���������״̬
	NEXTSTATE(u32(eREADY));
    g_cHduApp.m_byRegAckNum = 0;
	g_cHduApp.m_dwMpcSSrc = 0;

	//���Hduͨ��״̬�����������HDU�����ģʽ
	u8 byOutputMode = g_cHduApp.m_tHduStatus.byOutputMode;
	memset( &g_cHduApp.m_tHduStatus, 0, sizeof(g_cHduApp.m_tHduStatus) );
	u8 byIndex;
	for (byIndex = 0; byIndex < MAXNUM_HDU_CHANNEL; byIndex ++)
	{
		g_cHduApp.m_tHduStatus.atHduChnStatus[byIndex].SetVolume( HDU_VOLUME_DEFAULT );
	}
	g_cHduApp.m_tHduStatus.byOutputMode = byOutputMode;

	memset( &m_tChnInfo, 0x0, sizeof(m_tChnInfo));
	m_tChnInfo.m_tHduChnStatus.SetIsMute( 0 );
	m_tChnInfo.m_tHduChnStatus.SetVolume( HDU_VOLUME_DEFAULT );
	
	return;
}

/*====================================================================
����  : StatusShow
����  : ״̬��ʾ����ʾ���Ͻ�����״̬
����  : void
���  : ��
����  : void
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void   CHduInstance::StatusShow(void)                                    
{
	if ( GetInsID() != CInstance::DAEMON)
    {
        if(NULL != m_pDecCroup)
        {
            m_pDecCroup->ShowChnInfo();
        }
        else
        {
            OspPrintf(TRUE, FALSE, "m_pDecCroup is NULL\n");
        }
    }
	
	return;
}

/*====================================================================
����  : StatusNotify
����  : HDU״̬֪ͨ
����  : void
���  : ��
����  : void
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void   CHduInstance::StatusNotify(void)
{
	TPeriEqpStatus tEqpStatus;
	memset( &tEqpStatus, 0, sizeof(tEqpStatus) );
	tEqpStatus.SetMcuEqp((u8)g_cHduApp.m_tCfg.wMcuId, g_cHduApp.m_tCfg.byEqpId, g_cHduApp.m_tCfg.byEqpType);	
	tEqpStatus.m_byOnline = 1;
	tEqpStatus.SetAlias(g_cHduApp.m_tCfg.achAlias);
	tEqpStatus.m_tStatus.tHdu.byOutputMode = g_cHduApp.m_byOutPutMode;
    //����Ĭ��������С��16
	for ( u8 byChnlIdx=0; byChnlIdx<MAXNUM_HDU_CHANNEL; byChnlIdx++ )
	{
        tEqpStatus.m_tStatus.tHdu.atHduChnStatus[byChnlIdx].SetVolume( HDU_VOLUME_DEFAULT );
	}

	CServMsg cServMsg;
	cServMsg.SetMsgBody((u8*)&tEqpStatus, sizeof(tEqpStatus));
 	SendMsgToMcu(HDU_MCU_STATUS_NOTIF, cServMsg);

	return;
}
/*====================================================================
����  : ComplexQos
����  : ���Qosֵ
����  : u8& -->����Ƶ�������źŵȼ���u8 -->ip��������
���  : ��
����  : void
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void   CHduInstance::ComplexQos( u8& byValue, u8 byPrior )              
{
	u8 byLBit = ((byPrior & 0x08) >> 3);
    u8 byRBit = ((byPrior & 0x04) >> 1);
    u8 byTBit = ((byPrior & 0x02) << 1);
    u8 byDBit = ((byPrior & 0x01) << 3);
    
    byValue = byValue + ((byDBit + byTBit + byRBit + byLBit)*2);
    return;
}

/*====================================================================
����  : SetEncryptParam
����  : ������Ƶ���ܲ���
����  : u8 byChnIdx, 
        TMediaEncrypt *ptMediaEncryptVideo, 
		TDoublePayload *ptDoublePayloadVideo
���  : ��
����  : void
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL32 CHduInstance::SetEncryptParam( /*u8 byChnIdx, */
									 TMediaEncrypt *ptMediaEncryptVideo, 
					                 TDoublePayload *ptDoublePayloadVideo )
{
	if (m_pDecCroup == NULL)
	{
		return FALSE;
	}

	BOOL bRet;
    u8  abyKeyBuf[MAXLEN_KEY];
	memset( abyKeyBuf, 0, sizeof(abyKeyBuf) );
	s32 nKeySize;
    u8  byDecryptMode;
	
    //����-��Ƶ
    byDecryptMode = ptMediaEncryptVideo->GetEncryptMode();
	hdulog("[SetEncryptParam]:byDecryptMode == %d!\n", byDecryptMode);

    if (byDecryptMode == CONF_ENCRYPTMODE_NONE)
    {
        // FEC֧��
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
		
        hdulog("[hdu]start m_pDecCroup->SetVideoActivePT(%d, %d)\n", byPayLoadValue, byPayLoadValue);
        printf("[hdu]start m_pDecCroup->SetVideoActivePT(%d, %d)\n", byPayLoadValue, byPayLoadValue);
        
		//bRet = m_pDecCroup->SetVideoActivePT(byPayLoadValue, byPayLoadValue);
		//���ö�̬�غɲ���Ϊ����Ӧ������Ƶ��ʽ�����仯���������Ӧ����
		bRet = m_pDecCroup->SetVideoActivePT(MEDIA_TYPE_H264, MEDIA_TYPE_H264);
        if (!bRet)
		{
			hdulog("[CHduInstance] m_pDecCroup->SetVideoActivePT failed.\n");
			printf("[CHduInstance] m_pDecCroup->SetVideoActivePT failed.\n");
			return FALSE;
		}
		else
		{
			printf("[CHduInstance] m_pDecCroup->SetVideoActivePT sucessful.\n");
			hdulog("[CHduInstance] m_pDecCroup->SetVideoActivePT sucessful.\n");
		}
		
        hdulog("[hdu]SetVidDecryptKey(NULL, (u16)0, %d)\n", byDecryptMode);
		printf("[hdu]SetVidDecryptKey(NULL, (u16)0, %d)\n", byDecryptMode);

		bRet = m_pDecCroup->SetVidDecryptKey((s8*)NULL, (u16)0, byDecryptMode);
		if (!bRet)
		{
			hdulog("[CHduInstance] m_pDecCroup->SetVidDecryptKey failed \n");
			return  FALSE;
		}

		hdulog("[hdu]end SetVidDecryptKey \n");
		printf("[hdu]end SetVidDecryptKey \n");
    }
	else if (byDecryptMode == CONF_ENCRYPTMODE_DES || byDecryptMode == CONF_ENCRYPTMODE_AES)
    {
		
        //�ϲ����²�ı����ģʽ��һ��
        if (byDecryptMode == CONF_ENCRYPTMODE_DES)
        {
            byDecryptMode = DES_ENCRYPT_MODE;
        }
        else if (byDecryptMode == CONF_ENCRYPTMODE_AES)
        {
            byDecryptMode = AES_ENCRYPT_MODE;
        }
		else
		{
			hdulog("[CHduInstance::SetEncryptParam]other decrypt model\n");
		}
		
        // FEC֧��
        u8 byRealPayload = ptDoublePayloadVideo->GetRealPayLoad();
        u8 byActivePayload = ptDoublePayloadVideo->GetActivePayload();
        if ( MEDIA_TYPE_FEC == byActivePayload )
        {
            byRealPayload = MEDIA_TYPE_FEC;
        }

        hdulog("[hdu]start m_pDecCroup->SetVideoActivePT(%d, %d) in else\n", byActivePayload, byRealPayload);
        printf("[hdu]start m_pDecCroup->SetVideoActivePT(%d, %d) in else\n", byActivePayload, byRealPayload);
  		
        //bRet = m_pDecCroup->SetVideoActivePT(byActivePayload, byRealPayload);
		//���ö�̬�غɲ���Ϊ����Ӧ������Ƶ��ʽ�����仯���������Ӧ����
		bRet = m_pDecCroup->SetVideoActivePT(MEDIA_TYPE_H264, MEDIA_TYPE_H264);
        if (!bRet)
		{
			hdulog("[CHduInstance] m_pDecCroup->SetVideoActivePT failed.\n");
			return FALSE;
		}
		
        ptMediaEncryptVideo->GetEncryptKey(abyKeyBuf, &nKeySize);
		
		hdulog("[hdu]start SetVidDecryptKey(NULL, (u16)0, %d) in else\n", byDecryptMode);
		printf("[hdu]start SetVidDecryptKey(NULL, (u16)0, %d) in else\n", byDecryptMode);

		bRet = m_pDecCroup->SetVidDecryptKey((s8*)abyKeyBuf, (u16)nKeySize, byDecryptMode);
		if (!bRet)
		{
			hdulog("[CHduInstance] m_pDecCroup->SetVidDecryptKey failed \n");
			return FALSE;
		}

		hdulog("[hdu]end SetVidDecryptKey in else\n");
		printf("[hdu]end SetVidDecryptKey in else\n");

    }

	return TRUE;
}

/* ==================================================================
����  : SetAudEnctypt
����  : ������Ƶ���ܲ���
����  : u8 byChnIdx, 
        TMediaEncrypt *ptMediaEncryptVideo, 
		TDoublePayload *ptDoublePayloadVideo
���  : ��
����  : void
ע    :
---------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
===================================================================== */
BOOL32 CHduInstance::SetAudEnctypt(TMediaEncrypt *ptMediaEncryptVideo, 
		                           TDoublePayload *ptDoublePayloadVideo )  
{
	if (m_pDecCroup == NULL)
	{
		return FALSE;
	}
	if(NULL == ptMediaEncryptVideo || NULL == ptDoublePayloadVideo)
	{
		hdulog("[CHduInstance] The pointer cannot be Null (SetAudEnctypt)\n");
		return FALSE;
	}

	BOOL bRet;
	u8  abyKeyBuf[MAXLEN_KEY];
	s32 wKeySize;
	u8  byDecryptMode;
	   
	byDecryptMode = ptMediaEncryptVideo->GetEncryptMode();
	hdulog("[CHduInstance]����-��Ƶ\n");
	hdulog("[CHduInstance] Audio PayLoad: EncryptMode: %d, ActivePayload: %d, RealPayLoad: %d\n", 
		byDecryptMode, ptDoublePayloadVideo->GetActivePayload(),
		ptDoublePayloadVideo->GetRealPayLoad());
	// ���ò�����Ϊ���ܺͲ�����
	if (byDecryptMode == CONF_ENCRYPTMODE_NONE)
	{
		// FEC֧��
		u8 byPayLoadValue = 0;
		   
		if (ptDoublePayloadVideo->GetActivePayload() == MEDIA_TYPE_FEC)
		{
			byPayLoadValue = MEDIA_TYPE_FEC;
		 }
		   
		bRet =  m_pDecCroup->SetAudActivePT(byPayLoadValue, byPayLoadValue);
		if (!bRet)
		{
			hdulog("[CHduInstance] m_pDecCroup->SetAudActivePT failed.\n");
			return FALSE;
		}
		   
		bRet = m_pDecCroup->SetAudDecryptKey((s8*)NULL, (u16)0, byDecryptMode);
		if (!bRet)
		{
			hdulog("[CHduInstance] m_pDecCroup->SetAudDecryptKey failed !\n");
			return FALSE;
		}
	}
	else if (byDecryptMode == CONF_ENCRYPTMODE_DES || byDecryptMode == CONF_ENCRYPTMODE_AES)
	{
		//�ϲ����²�ı����ģʽ��һ��
		if (byDecryptMode == CONF_ENCRYPTMODE_DES)
		{
			byDecryptMode = DES_ENCRYPT_MODE;
		}
		else if (byDecryptMode == CONF_ENCRYPTMODE_AES)
		{
			byDecryptMode = AES_ENCRYPT_MODE;
		}
		else
		{
			hdulog("[CHduInstance::SetAudEnctypt]other decrypt model\n");
		}
		// FEC֧��
		u8 byRealPayload = ptDoublePayloadVideo->GetRealPayLoad();
		u8 byActivePayload = ptDoublePayloadVideo->GetActivePayload();
		if ( MEDIA_TYPE_FEC == byActivePayload )
		{
			byRealPayload = MEDIA_TYPE_FEC;
		}
		
		bRet = m_pDecCroup->SetAudActivePT(byActivePayload, byRealPayload); 
		if ( !bRet )
		{
			hdulog("[CHduInstance] m_pDecCroup->SetAudActivePT failed.\n");
			return FALSE;
		}
		
		ptMediaEncryptVideo->GetEncryptKey(abyKeyBuf, &wKeySize);
		
		bRet = m_pDecCroup->SetAudDecryptKey((s8*)abyKeyBuf, (u16)wKeySize, byDecryptMode);
		if (!bRet)
		{
			hdulog("[CHduInstance] m_pDecCroup->SetAudDecryptKey failed \n");
			return FALSE;
		}
	}
	return TRUE;
}

/* ==================================================================
����  : Play
����  : ͨ������    //һ��ʵ��һ��ͨ�������Բ��ô�ͨ����
����  : ��
���  : ��
����  : void
ע    :
---------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
===================================================================== */
BOOL CHduInstance::Play( u8 byMode )
{
	if (m_pDecCroup == NULL)
	{
		return FALSE;
	}
	hdulog( "[Play] Process to Start Channel: %d\n", GetInsID()-1 );
    BOOL bRet = TRUE;

	bRet = m_pDecCroup->StartDecode(TRUE, byMode);
    if(!bRet)
    {
        hdulog("[CHduInstance] Chn.%d Play failed as mode.%d \n",
			GetInsID()-1, byMode);
        bRet = FALSE;
    }
	
	SetTimer(EV_HDU_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);
 
    return bRet;

}

/* ==================================================================
����  : Stop
����  : ͨ��ֹͣ
����  : ��
���  : ��
����  : void
ע    :
---------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
===================================================================== */
BOOL CHduInstance::Stop( void )						                            
{
    BOOL bRet = TRUE;

    if (NULL != m_pDecCroup)
    {
        bRet = m_pDecCroup->StopDecode(TRUE, g_cHduApp.m_byOutPutMode);
        if(!bRet)
        {
            hdulog("[CHduInstance] Stop failed \n");
            bRet = FALSE;
        }
		hdulog("[Stop]:end StopDecode\n");
        printf("[Stop]:end StopDecode\n");
    }
    else
	{
		printf("[Stop]m_pDecCroup is null ,can not stop!\n");
	    bRet = FALSE;
	}

    KillTimer( EV_HDU_NEEDIFRAME_TIMER);
	
	return bRet;

}

/* ==================================================================
����  : GetVidSrcInfoByTypeAndMode
����  : ����HDU����ӿں������ʽ�õ���ƵԴ��Ϣ
����  : ��
���  : ��
����  : void
ע    :
---------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
===================================================================== */
BOOL CHduInstance::GetVidSrcInfoByTypeAndMode(u8 byOutPortType, u8 byOutModeType, TVidSrcInfo *ptVidSrcInfo)
{
    if (NULL == ptVidSrcInfo)
    {
		hdulog("[CHduInstance]:GetHduOutType--> ptVidSrcInfo is Null\n");
        return FALSE;
	}
    switch (byOutPortType)
	{
        case HDU_OUTPUT_YPbPr:
			switch ( byOutModeType )
			{
			case HDU_YPbPr_1080P_24fps:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 24;
				break;

			case HDU_YPbPr_1080P_25fps:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 25;
				break;
			
			case HDU_YPbPr_1080P_30fps:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 30;
				break;
			
			case HDU_YPbPr_1080P_50fps:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 50;
				break;
			
			case HDU_YPbPr_1080P_60fps:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;
			
			case HDU_YPbPr_1080i_60HZ:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 0;
				ptVidSrcInfo->m_dwFrameRate = 30;
				break;
			case HDU_YPbPr_720P_50fps:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 720;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 50;
				break;
			case HDU_YPbPr_720P_60fps:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 720;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;
			
			case HDU_YPbPr_576i_50HZ:
				ptVidSrcInfo->m_wWidth = 720;
				ptVidSrcInfo->m_wHeight = 576;
				ptVidSrcInfo->m_bProgressive = 0;
				ptVidSrcInfo->m_dwFrameRate = 25;
				break;
			
			case HDU_YPbPr_480i_60HZ:    
				ptVidSrcInfo->m_wWidth = 720;
				ptVidSrcInfo->m_wHeight = 480;
				ptVidSrcInfo->m_bProgressive = 0;
				ptVidSrcInfo->m_dwFrameRate = 30;
				break;
			
			default:   //HDU_YPbPr_1080i_50HZ
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 0;
				ptVidSrcInfo->m_dwFrameRate = 25;
				break;
			}
		    break;
	    
		case HDU_OUTPUT_VGA:
			switch (byOutModeType)
			{
			case HDU_VGA_SXGA_60HZ:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 1024;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;

			case HDU_VGA_SXGA_75HZ:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 1024;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 75;
				break;

		
			case HDU_VGA_XGA_75HZ:
                ptVidSrcInfo->m_wWidth = 1024;
				ptVidSrcInfo->m_wHeight = 768;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 75;
				break;
            
			case HDU_VGA_SVGA_60HZ:
				ptVidSrcInfo->m_wWidth = 800;
				ptVidSrcInfo->m_wHeight = 600;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;
			
			case HDU_VGA_SVGA_75HZ:
				ptVidSrcInfo->m_wWidth = 800;
				ptVidSrcInfo->m_wHeight = 600;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 75;
				break;
			
			case HDU_VGA_VGA_60HZ:
				ptVidSrcInfo->m_wWidth = 640;
				ptVidSrcInfo->m_wHeight = 480;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;
			
			case HDU_VGA_VGA_75HZ:
				ptVidSrcInfo->m_wWidth = 640;
				ptVidSrcInfo->m_wHeight = 480;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 75;
				break;
      
        
			default:      //HDU_VGA_XGA_60HZ
				ptVidSrcInfo->m_wWidth = 1024;
				ptVidSrcInfo->m_wHeight = 768;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
			}
		break;
	    
	default:
	    printf("[CHduInstance::GetHduOutType]:byOutModeType is not exit!\n");
		return FALSE;
	}
      
    return TRUE;
}
/*====================================================================
����  : DaemonProcChangePlayPolicy
����  : ����ͨ����ʾ���Ըı䴦��
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2012/09/18   4.7         ��־��        ����
====================================================================*/
void  CHduInstance::DaemonProcChangePlayPolicy( CMessage* const pMsg ) 
{
	if ( NULL == pMsg)
	{
		hdulog("[DaemonProcChangePlayPolicy]NULL == pMsg,So Return!\n");
		return;
	}
	u8 byIdleChnPlayPolicy = *(u8*)pMsg->content;
    for (u8 byLoop=0; byLoop<MAXNUM_HDU_CHANNEL; byLoop++)
    {
		post(MAKEIID(GetAppID(), byLoop+1), MCU_HDU_CHANGEPLAYPOLICY_NOTIF, &byIdleChnPlayPolicy,sizeof(u8));
	}
}
/*====================================================================
����  : ProcChangePlayPolicy
����  : ����ͨ����ʾ���Ըı䴦��
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2012/09/18   4.7         ��־��        ����
====================================================================*/
void  CHduInstance::ProcChangePlayPolicy( CMessage* const pMsg ) 
{
	if (m_pDecCroup == NULL)
	{
		return;
	}
	u8 byIdleChnPlayPolicy = *(u8*)pMsg->content;
	s32 nReturn = 0;
	// ���ײ����ÿ���ͨ����ʾģʽ
 	if( HDU_SHOW_BLACK_MODE == byIdleChnPlayPolicy )
 	{
 		nReturn = m_pDecCroup->m_cVidDecoder.SetNoStreamBak(u32(NoStrm_PlyBlk));
 	}
 	else if( HDU_SHOW_LASTFRAME_MODE == byIdleChnPlayPolicy )
 	{
 		nReturn = m_pDecCroup->m_cVidDecoder.SetNoStreamBak(u32(NoStrm_PlyLst));
 	}
 	else if( HDU_SHOW_DEFPIC_MODE == byIdleChnPlayPolicy )
 	{
 		nReturn = m_pDecCroup->m_cVidDecoder.SetNoStreamBak(u32(NoStrm_PlyBmp_dft));
 	}
 	else if( HDU_SHOW_USERDEFPIC_MODE == byIdleChnPlayPolicy )// �û��Զ���ͼƬ
 	{
 		nReturn = m_pDecCroup->m_cVidDecoder.SetNoStreamBak(u32(NoStrm_PlyBmp_usr));
 	}
	
	hdulog( "[ProcChangePlayPolicy]ProcStartPlayReq, SetNoStreamBak return: %d mode: %d\n", nReturn, byIdleChnPlayPolicy );
}
/* -------------------CHduInstance���ʵ��  end---------------------*/


/* ---------------------CHduData���ʵ��  start---------------------*/

CHduData::CHduData()
{
	memset(this, 0, sizeof(CHduData));
}

CHduData::~CHduData()
{
    
}

/*====================================================================
����  : SetNull
����  : ��0
����  : void
���  : ��
����  : void
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void CHduData::SetNull(void)
{
	memset(this, 0, sizeof(CHduData));
	return;
}
/*====================================================================
����  : FreeStatusDataA
����  : ���A״̬����
����  : void
���  : ��
����  : void
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void CHduData::FreeStatusDataA(void)
{
    m_dwMcuNode = INVALID_NODE;
    m_dwMcuIId  = INVALID_INS;
	return;
}

/*====================================================================
����  : FreeStatusDataB
����  : ���B״̬����
����  : void
���  : ��
����  : void
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void CHduData::FreeStatusDataB(void)
{
	m_dwMcuNodeB = INVALID_NODE;
    m_dwMcuIIdB  = INVALID_INS;
	return;
}
/*====================================================================
����  : ReadDebugValues
����  : �������ļ���ȡ��������ֵ
����  : u8& -->����Ƶ�������źŵȼ���u8 -->ip��������
���  : ��
����  : void
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void CHduData::ReadDebugValues(void)
{
	s32 nValue;
	//  zjl[20091224]
    ::GetRegKeyInt( HDUCFG_FILE, SECTION_HDUDEBUG, "DecResizeMode", 0, &nValue );
	printf("DecResizeMode = %d\n", nValue);
    m_tDebugVal.SetVidDecResizeMode((u32)nValue);

   
    ::GetRegKeyInt( HDUCFG_FILE, SECTION_HDUDEBUG, "IsEnableRateAutoDec", 1, &nValue );
    m_tDebugVal.SetEnableBitrateCheat( (nValue==0) ? FALSE : TRUE );
	
    if ( !m_tDebugVal.IsEnableBitrateCheat() )
        return;
	
    ::GetRegKeyInt( HDUCFG_FILE, SECTION_HDUDEBUG, "DefaultTargetRatePct", 75, &nValue );
    m_tDebugVal.SetDefaultRate((u16)nValue);
	
    return;
}

/*====================================================================
����  : IsEnableCheat
����  : ��ȡ�Ƿ�����
����  : 
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL32 CHduData::IsEnableCheat(void) const
{
	return m_tDebugVal.IsEnableBitrateCheat();
}

/*====================================================================
����  : GetDecRateDebug
����  : ��ȡ��������ֵ
����  : 
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
u16 CHduData::GetDecRateDebug(u16 wOriginRate) const
{
	return m_tDebugVal.GetDecRateDebug(wOriginRate);
}

/* ---------------------CHduData���ʵ��  end-----------------------*/


/* --------------------CDecoderGrp���ʵ��  start-----------------------*/

/*====================================================================
����  : Create
����  : ����KdvMediaRcv,KdvVidDec,KdvAudDec
����  : u32    dwMaxRcvFrameSize      //������֡��С
        u32    dwRtcpBackAddr         //rtcp�ط�Ip��ַ��
        u16    wRtcpBackPort          //rtcp�ط��˿�
        u16    wLocalStartPort        //rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
		u32    dwContext              //��Ӧ����͸��������ʵ��ָ��
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL CDecoderGrp::Create(u32    dwChnId,                //ͨ��ID
			             u32    dwRtcpBackAddr,         //rtcp�ط�Ip��ַ��
			             u16    wRtcpBackPort,          //rtcp�ط��˿�
			             u16    wLocalStartPort,        //rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
			             u32    dwContext)              //��Ӧ����͸��������ʵ��ָ��
{
	m_dwNetBand        = 8000000;
	m_dwRtcpBackAddr   = htonl(dwRtcpBackAddr);
	m_wRtcpBackPort    = wRtcpBackPort;
	m_wLocalStartPort  = wLocalStartPort;
	
    hdulog("\n[CDecoderGrp::Create] m_dwNetBand:%d m_dwRtcpBackAddr:%s, m_wRtcpBackPort:%u m_wLocalStartPort:%u\n",
		   m_dwNetBand,
		   ipStr(m_dwRtcpBackAddr),
		   m_wRtcpBackPort,
		   m_wLocalStartPort);
    
	u16 wRet = 0;
	do 
	{
		TDecoder tDecoder;
        memset(&tDecoder, 0, sizeof(TDecoder));
		tDecoder.dwCapPort = 0;
		tDecoder.dwChnID   = dwChnId;  //GetInsID() -1;
		tDecoder.dwDspID   = 0;
		tDecoder.dwVPID    = 0;

		hdulog("[create]start vid CreateDecoder!\n");
		printf("[create]start vid CreateDecoder!\n");
		wRet = m_cVidDecoder.CreateDecoder(&tDecoder);    //������Ƶ������
		if(u16(Codec_Success) != wRet)
		{
			//FIXME: param
			hdulog("CDecoderGrp::Create-->m_cVidDecoder.CreateDecoder fail, Error code is: %d \n", wRet);
 			printf("CDecoderGrp::Create-->m_cVidDecoder.CreateDecoder fail, Error code is: %d \n", wRet);
			break;
		}
        else
        {
			hdulog("CDecoderGrp::Create-->m_cVidDecoder.CreateDecoder succeed!\n");
            printf("CDecoderGrp::Create-->m_cVidDecoder.CreateDecoder succeed!\n");
			if (g_cHduApp.m_tCfg.GetHduSubType() == HDU_SUBTYPE_HDU_H)
			{
				m_cVidDecoder.SetWorkMode(u32(HDU_WORKMODE_1_CHNL));
				OspPrintf(TRUE, FALSE, "[CDecoderGrp::Create] Set WorkMode HDU_WORKMODE_1_CHNL!\n");
				printf("[CDecoderGrp::Create] Set WorkMode HDU_WORKMODE_1_CHNL!\n");
			}
			else if (g_cHduApp.m_tCfg.GetHduSubType() == HDU_SUBTYPE_HDU_L ||
				g_cHduApp.m_tCfg.GetHduSubType() == HDU_SUBTYPE_HDU_M)
			{
				m_cVidDecoder.SetWorkMode(u32(HDU_WORKMODE_2_CHNL));
				OspPrintf(TRUE, FALSE, "[CDecoderGrp::Create] Set WorkMode HDU_WORKMODE_2_CHNL!\n");
				printf("[CDecoderGrp::Create] Set WorkMode HDU_WORKMODE_2_CHNL!\n");
			}
		}

		wRet = m_cAudDecoder.CreateDecoder(&tDecoder);    //������Ƶ������
		if(u16(Codec_Success) != wRet)
		{
			hdulog("CDecoderGrp::Create-->m_cAudDecoder.CreateDecoder fail, Error code is: %d \n", wRet);
			printf("CDecoderGrp::Create-->m_cAudDecoder.CreateDecoder fail, Error code is: %d \n", wRet);
			break;
		}
        else
        {
			hdulog("CDecoderGrp::Create-->m_cAudDecoder.CreateDecoder succeed!\n");
            printf("CDecoderGrp::Create-->m_cAudDecoder.CreateDecoder succeed!\n");
		}

		wRet = m_cVidMediaRcv.Create(MAX_VIDEO_FRAME_SIZE, CBVidRecvFrame, dwContext);	//������Ƶ����	
        if( MEDIANET_NO_ERROR !=  wRet )
		{
			hdulog("CDecoderGrp::Create-->m_cVidMediaRcv.Create fail, Error code is: %d \n", wRet);
 			printf("CDecoderGrp::Create-->m_cVidMediaRcv.Create fail, Error code is: %d \n", wRet);
			break;
		}
		else
		{
			hdulog("CDecoderGrp::Create-->m_cVidMediaRcv.Create succeed!\n");
            printf("CDecoderGrp::Create-->m_cVidMediaRcv.Create succeed!\n");
		}

 		wRet = m_cAudMediaRcv.Create(MAX_AUDIO_FRAME_SIZE, CBAudRecvFrame, dwContext);	//������Ƶ����	
		if(MEDIANET_NO_ERROR !=  wRet)
 		{
			hdulog("CDecoderGrp::Create-->m_cAudMediaRcv.Create fail, Error code is: %d \n", wRet);
			printf("CDecoderGrp::Create-->m_cAudMediaRcv.Create fail, Error code is: %d \n", wRet);
 			break;
 		}
 		else
 		{
             printf("CDecoderGrp::Create-->m_cAudMediaRcv.Create succeed!\n");
 		}
		
        // ����HDFlag Ϊ TRUE
        m_cVidMediaRcv.SetHDFlag(TRUE);

		return TRUE;

	} while (0);

	return FALSE;
}

/*====================================================================
����  : StartDecode
����  : ��������Ƶ������
����  : BOOL32 bStartMediaRcv  //�Ƿ�������
        u8 byOutPutMode        //HDU����ģʽ
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL CDecoderGrp::StartDecode(BOOL32 bStartMediaRcv, u8 byOutPutMode/* = HDU_OUTPUTMODE_BOTH */)
{
    hdulog("CDecoderGrp::StartDecode\n");
	if(IsStart()&& GetLastOutPutMode() == byOutPutMode)
	{
		hdulog("CDecoderGrp::StartVidDec success\n");
		return TRUE;
 	}
	//��ʼ����Ƶ����
	if (HDU_OUTPUTMODE_BOTH == byOutPutMode) 
	{
		if (!StartVidDec(bStartMediaRcv))
		{
            hdulog("CDecoderGrp::StartDecoder --> StartVidDec fail!\n");
			printf("CDecoderGrp::StartDecoder --> StartVidDec fail!\n");
			return FALSE;
		}
		else
		{
            hdulog("CDecoderGrp::StartDecoder --> StartVidDec success!\n");
			printf("CDecoderGrp::StartDecoder --> StartVidDec success!\n");
		}

		if (!StartAudDec(bStartMediaRcv))
		{
			hdulog("CDecoderGrp::StartDecoder --> StartAudDec fail!\n");
			printf("CDecoderGrp::StartDecoder --> StartAudDec fail!\n");
			return FALSE;

		}
		else
		{
            hdulog("CDecoderGrp::StartDecoder --> StartAudDec success!\n");
			printf("CDecoderGrp::StartDecoder --> StartAudDec success!\n");
		}
	}
	else if ( HDU_OUTPUTMODE_VIDEO == byOutPutMode)
	{
		if (!StartVidDec(bStartMediaRcv))
		{
            hdulog("CDecoderGrp::StartDecoder --> StartVidDec fail!\n");
			printf("CDecoderGrp::StartDecoder --> StartVidDec fail!\n");
			return FALSE;
		}
		else
		{
            hdulog("CDecoderGrp::StartDecoder --> StartVidDec success!\n");
			printf("CDecoderGrp::StartDecoder --> StartVidDec success!\n");
		}
        
		StopAudDec( bStartMediaRcv );
	}
	else if ( HDU_OUTPUTMODE_AUDIO == byOutPutMode)
	{
		if (!StartAudDec(bStartMediaRcv))
		{
			hdulog("CDecoderGrp::StartDecoder --> StartAudDec fail!\n");
			printf("CDecoderGrp::StartDecoder --> StartAudDec fail!\n");
			return FALSE;
		}
		else
		{
            hdulog("CDecoderGrp::StartDecoder --> StartAudDec success!\n");
			printf("CDecoderGrp::StartDecoder --> StartAudDec success!\n");
		}

		StopVidDec( bStartMediaRcv );
	}
    else
    {
        StopAudDec( bStartMediaRcv );
        StopVidDec( bStartMediaRcv );
	}
 
    if ( HDU_OUTPUTMODE_BOTH  == byOutPutMode || 
         HDU_OUTPUTMODE_VIDEO == byOutPutMode ||
         HDU_OUTPUTMODE_AUDIO == byOutPutMode)
    {
	    SetIsStart(TRUE);
		SetLastOutPutMode(byOutPutMode);
    }

	return TRUE;
}

/*====================================================================
����  : StopDecode
����  : ֹͣ����Ƶ������
����  : BOOL32 bStartMediaRcv  //�Ƿ�������
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL CDecoderGrp::StopDecode( BOOL32 bStartMediaRcv, u8 byOutPutMode/* = HDU_OUTPUTMODE_BOTH */ )
{
	hdulog("CDecoderGrp::StopDecode\n");
	if(!IsStart())
	{
		hdulog("CDecoderGrp::StopDecode success\n");
		return TRUE;
 	}

	/************************************************************************/
	/*    �˴�ֹͣ����Ƶ������ʱ���Բ���ֹͣ���������ײ���ƽӿں�ſ�      */
	/************************************************************************/

    //2009-7-23 �ײ�ý�����Ŀǰ��֧�� ����ֹͣ������������ֹͣͨ��ʱ ���ֽ��벻����ֻ����������

	//ֹͣ��Ƶ����
	if ( (HDU_OUTPUTMODE_BOTH == byOutPutMode) || (HDU_OUTPUTMODE_VIDEO == byOutPutMode) )
	{
		if (!StopVidDec(bStartMediaRcv))
		{
            hdulog("CDecoderGrp::StopDecode --> StopVidDec fail!\n");
			printf("CDecoderGrp::StopDecode --> StopVidDec fail!\n");
			return FALSE;
		}
		else
		{
            hdulog("CDecoderGrp::StopDecode --> StopVidDec sucessful!\n");
			printf("CDecoderGrp::StopDecode --> StopVidDec sucessful!\n");
		}
	}
    //ֹͣ��Ƶ����
	if ( (HDU_OUTPUTMODE_BOTH == byOutPutMode) || (HDU_OUTPUTMODE_AUDIO == byOutPutMode) )
	{
		if (!StopAudDec(bStartMediaRcv))
		{
			hdulog("CDecoderGrp::StopDecode --> StopAudDec fail!\n");
			return FALSE;
		}
	}

	SetIsStart(FALSE);
	SetLastOutPutMode(0);

	return TRUE;
}

/*====================================================================
����  : StartVidDec
����  : ��ʼ��Ƶ����
����  : BOOL32 bStartMediaRcv  --->�Ƿ�����Media����
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL CDecoderGrp::StartVidDec(BOOL32 bStartMediaRcv/* = FALSE */)
{
	u16 wRet = 0;
		
	wRet = m_cVidDecoder.StartDec();
	
	if (bStartMediaRcv)
	{
		m_cVidMediaRcv.StartRcv();
	}
	
	if(u16(Codec_Success) != wRet)
	{
		m_cVidMediaRcv.StopRcv();
        OspPrintf(TRUE, FALSE, "CDecoderGrp::StartVidDec-->m_cVidDecoder.StartDec fail, Error code is: %d \n", wRet);
		return FALSE;
	}
	
	return TRUE;
}

/*====================================================================
����  : StopVidDec
����  : ֹͣ��Ƶ����
����  : BOOL32 bStopMediaRcv  --->�Ƿ�ֹͣMedia����
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL CDecoderGrp::StopVidDec(BOOL32 bStopMediaRcv, BOOL32 bStopDec)
{
	u16 wRet =0;
    if (bStopMediaRcv)
    {
        wRet = m_cVidMediaRcv.StopRcv();
		if (u16(Codec_Success) != wRet)
		{
			hdulog("CDecoderGrp::StopVidDec -->m_cVidDecoder.StopRcv fail! Error code is: %d\n", wRet);
			//return FALSE;
		}
		else
		{
			hdulog("CDecoderGrp::StopVidDec -->m_cVidDecoder.StopRcv sucessful!\n");
        }

        BOOL32 bRet = SetVidNetRcvLocalParam(NULL);
        if (!bRet)
        {
            hdulog("CDecoderGrp::StopVidDec -->m_pDecCroup->SetVidNetRcvLocalParam failed \n");
            //return FALSE;
        }
			
		TRSParam tRsParam;
		memset(&tRsParam, 0x0, sizeof(tRsParam));
		bRet = SetNetRecvFeedbackVideoParam(tRsParam, FALSE);
        if (!bRet)
        {
            hdulog("CDecoderGrp::StopVidDec -->m_pDecCroup->SetNetRecvFeedbackVideoParam failed \n");
            //return FALSE;
        }
	}

    if (bStopDec)
    {
        wRet = m_cVidDecoder.StopDec();
        if (u16(Codec_Success) != wRet)
        {
            hdulog("CDecoderGrp::StopVidDec -->m_cVidDecoder.StopDec fail! Error code is: %d\n", wRet);
            return FALSE;
        }
        else
        {
            hdulog("CDecoderGrp::StopVidDec -->m_cVidDecoder.StopDec sucessful!\n");
        }
    }

	return TRUE;
}

/*====================================================================
����  : StartAudDec
����  : ��ʼ��Ƶ����
����  : BOOL32 bStartMediaRcv  --->�Ƿ�ʼMedia����
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL CDecoderGrp::StartAudDec(BOOL32 bStartMediaRcv/* = FALSE*/)
{
	u16 wRet = 0;
	
	wRet = m_cAudDecoder.StartDec();
	
	if (bStartMediaRcv)
	{
		m_cAudMediaRcv.StartRcv();
	}
	
	if(u16(Codec_Success) != wRet)
	{
		m_cAudMediaRcv.StopRcv();
        hdulog("CDecoderGrp::CDecoderGrp-->m_cAudDecoder.StartDec fail, Error code is: %d \n", wRet);
		return FALSE;
	}
	else
	{
        hdulog("CDecoderGrp::CDecoderGrp-->m_cAudDecoder.StartDec sucessful\n");
	}

	return TRUE;
}

/*====================================================================
����  : StopAudDec
����  : ֹͣ��Ƶ����
����  : BOOL32 bStopMediaRcv  --->�Ƿ�ֹͣMedia����
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL CDecoderGrp::StopAudDec(BOOL32 bStopMediaRcv, BOOL32 bStopDec)
{
	u16 wRet =0;
    if (bStopMediaRcv)
    {
        wRet = m_cAudMediaRcv.StopRcv();
		if (u16(Codec_Success) != wRet)
		{
			hdulog("CDecoderGrp::StopAudDec -->m_cAudMediaRcv.StopRcv fail! Error code is: %d\n", wRet);
			//return FALSE;
		}
		else
		{
			hdulog("CDecoderGrp::StopAudDec -->m_cAudMediaRcv.StopRcv sucessful! \n");
	    }

        BOOL32 bRet = SetAudNetRcvLocalParam(NULL);
        if (!bRet)
        {
            hdulog("CDecoderGrp::StopAudDec -->m_pDecCroup->SetAudNetRcvLocalParam failed \n");
            //return FALSE;
        }

		TRSParam tRsParam;
		memset(&tRsParam, 0x0, sizeof(tRsParam));
		bRet = SetNetRecvFeedbackAudioParam(tRsParam, FALSE);
        if (!bRet)
        {
            hdulog("CDecoderGrp::StopAudDec -->m_pDecCroup->SetNetRecvFeedbackAudioParam failed \n");
            //return FALSE;
        }
	}
    if (bStopDec)
    {
        wRet = m_cAudDecoder.StopDec();
        if (u16(Codec_Success) != wRet)
        {
            hdulog("CDecoderGrp::StopAudDec -->m_cAudDecoder.StopDec fail! Error code is: %d\n", wRet);
            return FALSE;
        }
        else
        {
            hdulog( "CDecoderGrp::StopAudDec -->m_cAudDecoder.StopDec sucessful! \n" );
        }
    }
	return TRUE;
}

/*====================================================================
����  : SetNetRecvFeedbackVideoParam
����  : ����ͼ�����������ش�����
����  : TRSParam tNetRSParam  -->�����ش�����
        BOOL32 bRepeatSnd --->�Ƿ��ط�
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL  CDecoderGrp::SetNetRecvFeedbackVideoParam(TRSParam tNetRSParam, BOOL32 bRepeatSnd/* = FALSE*/)
{
	//�������
    //SetRsParam(tNetRSParam);
	SetIsNeedRS(bRepeatSnd);

	u16 wRet = m_cVidMediaRcv.ResetRSFlag(tNetRSParam, bRepeatSnd);

    hdulog("[SetNetRecvFeedbackVideoParam]m_wFirstTimeSpan = %d\nm_wSecondTimeSpan = %d\nm_wThirdTimeSpan = %d\nm_wRejectTimeSpan = %d",
            tNetRSParam.m_wFirstTimeSpan,
            tNetRSParam.m_wSecondTimeSpan,
            tNetRSParam.m_wThirdTimeSpan,
            tNetRSParam.m_wRejectTimeSpan);

	if(MEDIANET_NO_ERROR != wRet)
	{
		hdulog("CDecoderGrp::SetNetRecvFeedbackVideoParam-->m_cVidMediaRcv.ResetRSFlag fail,Error code is:%d\n",wRet);
		//return FALSE;
	}

	return TRUE;
}

/*====================================================================
����  : SetVideoActivePT
����  : ���� H.263+/H.264 �ȶ�̬��Ƶ�غɵ� Playloadֵ
����  : u8 byRmtActivePT
        u8 byRealPT 
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL  CDecoderGrp::SetVideoActivePT(u8 byRmtActivePT, u8 byRealPT )
{
	u16 wRet = m_cVidMediaRcv.SetActivePT(byRmtActivePT, byRealPT);
	if (u16(Codec_Success) != wRet)
	{
		hdulog("[CDecoderGrp]:SetVideoActivePT-->m_cVidMediaRcv.SetActivePT fail as %d\n", wRet);
        return FALSE;
	}
	else
	{
		hdulog("[CDecoderGrp]:SetVideoActivePT-->m_cVidMediaRcv.SetActivePT sucessful\n");
	}
	return TRUE;
}

/*====================================================================
����  : SetAudActivePT
����  : ������Ƶ�غɵ� Playloadֵ
����  : u8 byRmtActivePT
        u8 byRealPT 
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL CDecoderGrp::SetAudActivePT(u8 byRmtActivePT, u8 byRealPT )
{
	u16 wRet = m_cAudMediaRcv.SetActivePT(byRmtActivePT, byRealPT);
	if (u16(Codec_Success) != wRet)
	{
		hdulog("[CDecoderGrp]:SetVideoActivePT-->m_cVidMediaRcv.SetActivePT fail as %d\n", wRet);
        return FALSE;
	}
	else
	{
		hdulog("[CDecoderGrp]:SetVideoActivePT-->m_cVidMediaRcv.SetActivePT sucessfule\n");
	}

	return TRUE;
}

/*====================================================================
����  : SetVidDecryptKey
����  : ���� ��Ƶ����key�ִ� �Լ� ����ģʽ Aes or Des
����  : s8 *pszKeyBuf
        u16 wKeySize
		u8 byDecryptMode
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL CDecoderGrp::SetVidDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
{
	m_tMediaDec.SetEncryptKey((u8*)pszKeyBuf, wKeySize);
	m_tMediaDec.SetEncryptMode(byDecryptMode);
    u16 wRet = m_cVidMediaRcv.SetDecryptKey(pszKeyBuf, wKeySize, byDecryptMode);
    if (u16(Codec_Success) != wRet)
    {
		hdulog("[CDeoderGrp]:SetVidDecryptKey-->m_cVidMediaRcv.SetDecryptKey fail as %d\n", wRet);
        return FALSE;
	}
	else
	{
   		hdulog("[CDeoderGrp]:SetVidDecryptKey-->m_cVidMediaRcv.SetDecryptKey sucessful\n");
	}
    
	return TRUE;
}


/*====================================================================
����  : SetAudDecryptKey
����  : ������Ƶ�����ִ�������ģʽ
����  : s8 *pszKeyBuf
        u16 wKeySize
		u8 byDecryptMode
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL CDecoderGrp::SetAudDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
{
	m_tMediaDec.SetEncryptKey((u8*)pszKeyBuf, wKeySize);
	m_tMediaDec.SetEncryptMode(byDecryptMode);
    u16 wRet = m_cAudMediaRcv.SetDecryptKey(pszKeyBuf, wKeySize, byDecryptMode);
    if (u16(Codec_Success) != wRet)
    {
		hdulog("[CDeoderGrp]:SetVidDecryptKey-->m_cVidMediaRcv.SetDecryptKey fail as %d\n", wRet);
        return FALSE;
	}
	else
	{
		hdulog("[CDeoderGrp]:SetVidDecryptKey-->m_cVidMediaRcv.SetDecryptKey sucessful\n");
	}

	return TRUE;
}

/*====================================================================
����  : SetNetRecvFeedbackAudioParam
����  : ������Ƶ����������ش�����
����  : TRSParam tNetRSParam  -->�����ش�����
        BOOL32 bRepeatSnd --->�Ƿ��ط�
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL  CDecoderGrp::SetNetRecvFeedbackAudioParam(TRSParam tNetRSParam, BOOL32 bRepeatSnd/* = FALSE*/)
{
	u16 wRet = m_cAudMediaRcv.ResetRSFlag(tNetRSParam, bRepeatSnd);
	if(MEDIANET_NO_ERROR != wRet)
	{
		hdulog("CDecoderGrp::SetNetRecvFeedbackAudioParam-->m_cAudMediaRcv.ResetRSFlag fail,Error code is:%d\n",wRet);
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
����  : GetVidDecoderStatus
����  : ��ȡ��Ƶ������״̬
����  : TKdvDecStatus &tKdvDecStatus
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL    CDecoderGrp::GetVidDecoderStatus(TKdvDecStatus &tKdvDecStatus)
{
    u16 wRet = m_cVidDecoder.GetDecoderStatus(tKdvDecStatus);
	if (u16(Codec_Success) != wRet)
	{
		hdulog("[CDecorderGrp]GetVidDecoderStatus-->m_cVidDecoder.GetDecoderStatus() fail! as %d\n", wRet);
	    return FALSE;
	}
	else
	{
		hdulog("[CDecorderGrp]GetVidDecoderStatus-->m_cVidDecoder.GetDecoderStatus() sucessful\n");
	}
	return TRUE;
}

/*====================================================================
����  : GetVidDecoderStatis
����  : ��ȡ��Ƶ��������ͳ����Ϣ
����  : TKdvDecStatis &tKdvDecStatis
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL    CDecoderGrp::GetVidDecoderStatis(TKdvDecStatis &tKdvDecStatis)
{
	u16 wRet = m_cVidDecoder.GetDecoderStatis(tKdvDecStatis);
    if (u16(Codec_Success) != wRet)
    {
		hdulog("[CDecoderGrp]GetVidDecoderStatis-->m_cVidDecoder.GetDecoderStatis() fail as %d\n", wRet);
        return FALSE;
    }

	return TRUE;
}	

/*====================================================================
����  : GetAudDecoderStatus
����  : ��ȡ��Ƶ������״̬
����  : TKdvDecStatus &tKdvDecStatus
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL    CDecoderGrp::GetAudDecoderStatus(TKdvDecStatus &tKdvDecStatus)
{
    u16 wRet = u16(Codec_Success);

	wRet = m_cAudDecoder.GetDecoderStatus(tKdvDecStatus);
    if (u16(Codec_Success) != wRet)
	{
        hdulog("[CDecoderGrp]::GetAudDecoderStatus-->m_cAudDecoder.GetDecoderStatus fail as %d\n", wRet);
	    return FALSE;
	}
	else
	{
        hdulog("[CDecoderGrp]::GetAudDecoderStatus-->m_cAudDecoder.GetDecoderStatus sucessful\n");
	}
	return TRUE;
}

/*====================================================================
����  : GetAudVidDecoderStatis
����  : ��ȡ��Ƶ��������ͳ����Ϣ
����  : TKdvDecStatis &tKdvDecStatis
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL    CDecoderGrp::GetAudVidDecoderStatis(TKdvDecStatis &tKdvDecStatis)
{
    u16 wRet = u16(Codec_Success);

    wRet = m_cAudDecoder.GetDecoderStatis(tKdvDecStatis);
	if (u16(Codec_Success) != wRet)
	{
		hdulog("[CDecoderGrp]:GetAudVidDecoderStatis-->m_cAudDecoder.GetDecoderStatis fail\n");
	    return FALSE;
	}
	else
	{
		hdulog( "[CDecoderGrp]:GetAudVidDecoderStatis-->m_cAudDecoder.GetDecoderStatis successful\n");
	}
	return TRUE;
}	

/*====================================================================
����  : SetVidPlyPortType
����  : ������Ƶ���Ŷ˿����� VGA or DVI
����  : u32 dwType
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL    CDecoderGrp::SetVidPlyPortType(u32 dwType)
{
	hdulog("[hdu]enter SetVidPlyPortType()\n");
    u16 wRet = m_cVidDecoder.SetVidPlyPortType(dwType);
	if (u16(Codec_Success) != wRet)
	{
		hdulog("[CDecoderGrp]:SetVidPlyPortType-->m_cVidDecoder.SetVidPlyPortType fail\n");
        return FALSE;
	}
	else
	{
		hdulog("[CDecoderGrp]:SetVidPlyPortType-->m_cVidDecoder.SetVidPlyPortType successful\n");
	}

	return TRUE;
}

/*====================================================================
����  : SetPlayScale
����  : �������ű���
����  : 16 wWidth, u16 wHeigh
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL CDecoderGrp::SetPlayScales( u16 wWidth, u16 wHeigh )
{
    u16 wRet = m_cVidDecoder.SetPlayScale(wWidth, wHeigh);
	if (u16(Codec_Success) != wRet)
	{
		hdulog("[CDecoderGrp]:SetPlayScale-->m_cVidDecoder.SetPlayScale fail\n");
		return FALSE;
	}
	else
	{
		hdulog("[CDecoderGrp]:SetPlayScale-->m_cVidDecoder.SetPlayScale sucessful\n");
	}
	
	return TRUE;
}

/*====================================================================
����  : SetVideoPlyInfo
����  : ������Ƶ����ź���ʽ��NULL��ʾ�Զ�����
����  : TVidSrcInfo* ptInfo
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL CDecoderGrp::SetVideoPlyInfo(TVidSrcInfo* ptInfo)
{
    u16 wRet = m_cVidDecoder.SetVideoPlyInfo(ptInfo);
	if (u16(Codec_Success) != wRet)
	{
		hdulog("[CDecoderGrp]:SetVideoPlyInfo-->m_cVidDecoder.SetVideoPlyInfo fail\n");
		return FALSE;
	}
	else
	{
		hdulog("[CDecoderGrp]:SetVideoPlyInfo-->m_cVidDecoder.SetVideoPlyInfo sucessful\n");
	}
	
	return TRUE;
}

/*====================================================================
����  : SetAudOutPort
����  : ������Ƶ���Ŷ˿����� 
����  : u32 dwType
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL    CDecoderGrp::SetAudOutPort(u32 dwAudPort)
{
    u16 wRet = u16(Codec_Success);

	wRet = m_cAudDecoder.SetAudOutPort(dwAudPort);
    if (u16(Codec_Success) != wRet)
    {
		hdulog("[CDecoderGrp]:SetAudOutPort-->m_cAudDecoder.SetAudOutPort fail\n");
        return FALSE;
    }
	else
	{
		hdulog("[CDecoderGrp]:SetAudOutPort-->m_cAudDecoder.SetAudOutPort sucessful\n");
	}

	return TRUE;
}

/*====================================================================
����  : SetVidNetRcvLocalParam
����  : ������Ƶ���յ�ַ����(���еײ��׽��ӵĴ������󶨶˿ڵȶ���)
����  : TLocalNetParam tLocalNetParam
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL    CDecoderGrp::SetVidNetRcvLocalParam ( TLocalNetParam *ptLocalNetParam )
{
    u16 wRet = u16(Codec_Success);
	if (NULL != ptLocalNetParam)
	{
		m_cVidMediaRcv.SetNetRcvLocalParam(*ptLocalNetParam);
	}
	else
	{
		m_cVidMediaRcv.RemoveNetRcvLocalParam();
	}

	if (u16(Codec_Success) != wRet)
	{
		hdulog("[CDecoderGrp]: SetVidNetRcvLocalParam failed as %d\n", wRet);
	    return FALSE;
	}
	else
	{
        hdulog("[CDecoderGrp]: SetVidNetRcvLocalParam suceeded\n");
	}

	return TRUE;
}

/*====================================================================
����  : SetAudioVolume
����  : ���������������
����  : u8 byVolume
���  : ��
����  : u16
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
u16  CDecoderGrp::SetAudioVolume(u8 byVolume )
{
    return m_cAudDecoder.SetAudioVolume( byVolume );
} 

//�õ������������
/*====================================================================
����  : GetAudioVolume
����  : �õ������������
����  : u8 byVolume
���  : ��
����  : u8
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
u16  CDecoderGrp::GetAudioVolume(u8 &pbyVolume )
{
    return m_cAudDecoder.GetAudioVolume( pbyVolume );
}

/*====================================================================
����  : SetAudioMute
����  : ���þ���
����  : BOOL32 bMute
���  : ��
����  : u16
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
u16  CDecoderGrp::SetAudioMute(BOOL32 bMute)
{
    return m_cAudDecoder.SetAudioMute( bMute );
}

/*====================================================================
����  : SetAudNetRcvLocalParam
����  : ������Ƶ���յ�ַ����(���еײ��׽��ӵĴ������󶨶˿ڵȶ���)
����  : TLocalNetParam tLocalNetParam
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
BOOL    CDecoderGrp::SetAudNetRcvLocalParam ( TLocalNetParam *ptLocalNetParam )
{
    u16 wRet = u16(Codec_Success);
	if (NULL != ptLocalNetParam)
	{
		m_cAudMediaRcv.SetNetRcvLocalParam(*ptLocalNetParam);
	}
	else
	{
		m_cAudMediaRcv.RemoveNetRcvLocalParam();
	}
	
	if (u16(Codec_Success) != wRet)
	{
		hdulog("[CDecoderGrp]: SetAudNetRcvLocalParam failed as %d\n", wRet);
	    return FALSE;
	}
	else
	{
		hdulog("[CDecoderGrp]: SetAudNetRcvLocalParam sucessful\n");
	}

	return TRUE;
}
/*====================================================================
����  : ShowChnInfo
����  : ��ʾͨ����Ϣ
����  : ��
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void    CDecoderGrp::ShowChnInfo(void)
{
	OspPrintf(TRUE, FALSE, "\n----------------CDecoder Group Info-------------------------\n");
	
	//������Ϣ
	OspPrintf(TRUE, FALSE, "\n*************Basic info*************\n");
	OspPrintf(TRUE, FALSE, "\t Is Start:%d\n", IsStart());	
    if ( !IsStart()) 
    {
        return;
    }
	
    OspPrintf(TRUE, FALSE, "\t Net Band:%u\n", m_dwNetBand);
	
	//�������
	OspPrintf(TRUE, FALSE, "\n*************DEC Parmater***********\n");
	u8 abyDecKey[64];
	s32 iLen = 0;
	m_tMediaDec.GetEncryptKey(abyDecKey, &iLen);
	abyDecKey[iLen] = 0;
	
	OspPrintf(TRUE, FALSE, "\t DEC key:%s\n", abyDecKey);
	OspPrintf(TRUE, FALSE, "\t DEC key len:%d\n", iLen);
	OspPrintf(TRUE, FALSE, "\t DEC mode:%u\n", m_tMediaDec.GetEncryptMode());
	
	return;
}

/* --------------------CDecoderGrp���ʵ��  end-----------------------*/



/*====================================================================
����  : hdulog
����  : ��ӡ
����  : 
���  : ��
����  : ��
ע    : 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
/*lint -save -e438 -e526 -e530 -e628 -e1055*/
void hdulog( s8* pszFmt, ...)
{
    s8 achPrintBuf[512];
    s32  nBufLen = 0;
	
    va_list argptr;
    if( g_nHduPLog >= 1 )
    {		  
        nBufLen = sprintf( achPrintBuf, "[Hdu]: " );         
        va_start( argptr, pszFmt );
        vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
        va_end(argptr); 
        OspPrintf( TRUE, FALSE, achPrintBuf ); 
    }
    return;
}

/*====================================================================
����  : hdulogall
����  : ��ӡ����
����  : ��ӡ��ʽ�ַ���
���  : ��
����  : ��
ע    : 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
void hdulogall( s8* pszFmt, ...)
{
    s8 achPrintBuf[512];
    s32  nBufLen = 0;
    va_list argptr;
    if( g_nHduPLog >= 2 )
    {		  
        nBufLen = sprintf( achPrintBuf, "[HduDBG]: " );         
        va_start( argptr, pszFmt );
        vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
        va_end(argptr); 
        OspPrintf( TRUE, FALSE, achPrintBuf ); 
    }
}

/*====================================================================
����  : ipStr
����  : ȡ��IP��ַ�ַ���
����  : dwIp: IP��ַ
���  : ��
����  : IP��ַ��Ӧ���ַ���
ע    : 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2008/11/27   4.6         ���ֱ�        ����
====================================================================*/
s8* ipStr(u32 dwIP)
{
	static s8 achIPStr[4][32]={0};
	static s8 byIdx = 0;
    struct in_addr in;
	
	byIdx++;
	s8 byBite = sizeof(achIPStr)/sizeof(achIPStr[0]);
	byIdx %= byBite;
    in.s_addr = dwIP;
	memset(achIPStr[byIdx], 0, sizeof(achIPStr[0]));
	
#ifdef _MSC_VER
	strncpy(achIPStr[byIdx],inet_ntoa(in),sizeof(achIPStr[0]));
#endif //_MSC_VER
	
#ifdef _LINUX_
	strncpy(achIPStr[byIdx],inet_ntoa(in),sizeof(achIPStr[0]));
#endif //_LINUX_
	
#ifdef _VXWORKS_
	inet_ntoa_b(in, achIPStr[byIdx]);
#endif //_VXWORKS_
	
	achIPStr[byIdx][31] = '\0'; // �ַ���������־
	return achIPStr[byIdx];
}


//END OF FILE

