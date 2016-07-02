/*****************************************************************************
   ģ����      : BAS-HD
   �ļ���      : basInst.cpp
   ����ļ�    : basInst.h
   �ļ�ʵ�ֹ���: BAS��ʵ��
   ����        : ����
   �汾        : V1.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/11/10  2.0         zsh         ����
   2005/01/12  3.6         libo        ���Ӽ���
   2006/03/21  4.0         �ű���      �����Ż�
   2008/08/13  4.5         ����      BAS-HD 

  FIXME:

  1���ش����ִ������⣺1) ���ؽ����ش����á�2) ��Դ�����ش����� ��������
  2������֡����Ҫҵ����������
  3������˫��û��ͨ������
  4��basshow ��ӡ�������ƣ�������Ϣ����Ĵ�ӡҲ�������� start/change req
  5����Ҫ��Ŀǰ����ֻ������һ��instance������֧�� VGA ����
******************************************************************************/
#include "basinst.h"
#include "evmcueqp.h"
#include "evbas.h"
#include "baserr.h"
#include "boardagent.h"
#include "mcuver.h"

s8	BASCFG_FILE[KDV_MAX_PATH] = {0};
s8	SECTION_BASSYS[] = "EQPBAS";
s8  SECTION_BASDEBUG[] = "EQPDEBUG";

s32     g_nbaslog    = 0;

CBasApp g_cBasApp;


u8		g_byAdpMode = STATUS_HD;

/*=============================================================================
  �� �� ���� CBRecvFrame0
  ��    �ܣ� �����յ���֡����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� PFRAMEHDR pFrmHdr, u32 dwContext
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����ǿ         ����
=============================================================================*/
static void CBRecvFrame0(PFRAMEHDR pFrmHdr, u32 dwContext)
{
	CBasInst * pInst = (CBasInst*)dwContext;
    
    u8 byNo = (u8)dwContext;

    if (pFrmHdr == NULL || pInst == NULL)
    {
        OspPrintf(TRUE, FALSE, "[CBRecvFrame0] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrmHdr, dwContext);
        return ;
    }


    CHdAdpGroup *pAdpGroup = pInst->m_pAdpCroup;
	if (pAdpGroup == NULL)
	{
		OspPrintf(TRUE, FALSE, "[CBRecvFrame0] pAdpGroup 0x%x\n", pAdpGroup);
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

    baslogall("[CBRecvFrame0] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, SSRC:%u, DataSize:%u\n",
              tFrameHdr.m_dwMediaType,
              tFrameHdr.m_dwFrameID,
              tFrameHdr.m_tVideoParam.m_bKeyFrame,
              tFrameHdr.m_tVideoParam.m_wVideoWidth,
              tFrameHdr.m_tVideoParam.m_wVideoHeight,              
              tFrameHdr.m_dwSSRC,
              tFrameHdr.m_dwDataSize
              );

    pAdpGroup->m_cAptGrp.SetData(0, tFrameHdr);

	return;
}


static void CBRecvFrame1(PFRAMEHDR pFrmHdr, u32 dwContext)
{
    CBasInst * pInst = (CBasInst*)dwContext;
    
    u8 byNo = (u8)dwContext;
    
    if (pFrmHdr == NULL || pInst == NULL)
    {
        OspPrintf(TRUE, FALSE, "[CBRecvFrame0] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrmHdr, dwContext);
        return ;
    }
    
    
    CHdAdpGroup *pAdpGroup = pInst->m_pAdpCroup;
    if (pAdpGroup == NULL)
    {
        OspPrintf(TRUE, FALSE, "[CBRecvFrame0] pAdpGroup 0x%x\n", pAdpGroup);
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
    
    baslogall("[CBRecvFrame0] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, SSRC:%u, DataSize:%u\n",
        tFrameHdr.m_dwMediaType,
        tFrameHdr.m_dwFrameID,
        tFrameHdr.m_tVideoParam.m_bKeyFrame,
        tFrameHdr.m_tVideoParam.m_wVideoWidth,
        tFrameHdr.m_tVideoParam.m_wVideoHeight,              
        tFrameHdr.m_dwSSRC,
        tFrameHdr.m_dwDataSize
        );
    
    pAdpGroup->m_cAptGrp.SetData(1, tFrameHdr);
    
    return;
}


static void CBRecvFrame2(PFRAMEHDR pFrmHdr, u32 dwContext)
{
    CBasInst * pInst = (CBasInst*)dwContext;
    
    u8 byNo = (u8)dwContext;
    
    if (pFrmHdr == NULL || pInst == NULL)
    {
        OspPrintf(TRUE, FALSE, "[CBRecvFrame0] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrmHdr, dwContext);
        return ;
    }
    
    
    CHdAdpGroup *pAdpGroup = pInst->m_pAdpCroup;
    if (pAdpGroup == NULL)
    {
        OspPrintf(TRUE, FALSE, "[CBRecvFrame0] pAdpGroup 0x%x\n", pAdpGroup);
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
    
    baslogall("[CBRecvFrame0] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, SSRC:%u, DataSize:%u\n",
        tFrameHdr.m_dwMediaType,
        tFrameHdr.m_dwFrameID,
        tFrameHdr.m_tVideoParam.m_bKeyFrame,
        tFrameHdr.m_tVideoParam.m_wVideoWidth,
        tFrameHdr.m_tVideoParam.m_wVideoHeight,              
        tFrameHdr.m_dwSSRC,
        tFrameHdr.m_dwDataSize
        );
    
    pAdpGroup->m_cAptGrp.SetData(2, tFrameHdr);
    
    return;
}


static void CBRecvFrame3(PFRAMEHDR pFrmHdr, u32 dwContext)
{
    CBasInst * pInst = (CBasInst*)dwContext;
    
    u8 byNo = (u8)dwContext;
    
    if (pFrmHdr == NULL || pInst == NULL)
    {
        OspPrintf(TRUE, FALSE, "[CBRecvFrame0] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrmHdr, dwContext);
        return ;
    }
    
    
    CHdAdpGroup *pAdpGroup = pInst->m_pAdpCroup;
    if (pAdpGroup == NULL)
    {
        OspPrintf(TRUE, FALSE, "[CBRecvFrame0] pAdpGroup 0x%x\n", pAdpGroup);
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
    
    baslogall("[CBRecvFrame0] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, SSRC:%u, DataSize:%u\n",
        tFrameHdr.m_dwMediaType,
        tFrameHdr.m_dwFrameID,
        tFrameHdr.m_tVideoParam.m_bKeyFrame,
        tFrameHdr.m_tVideoParam.m_wVideoWidth,
        tFrameHdr.m_tVideoParam.m_wVideoHeight,              
        tFrameHdr.m_dwSSRC,
        tFrameHdr.m_dwDataSize
        );
    
    pAdpGroup->m_cAptGrp.SetData(3, tFrameHdr);
    
    return;
}


/*=============================================================================
  �� �� ���� CBSendFrame
  ��    �ܣ� ����������֡����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� PFRAMEHDR pFrmHdr, u32 dwContext
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����ǿ         ����
=============================================================================*/
static void CBSendFrame(PTFrameHeader pFrmHdr, void* dwContext)
{
    if ( pFrmHdr == NULL || dwContext == NULL )
    {
        OspPrintf(TRUE, FALSE, "[CBSendFrame] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrmHdr, dwContext);
        return ;
    }

	CKdvMediaSnd *pMediaSnd = (CKdvMediaSnd*)dwContext;

	FRAMEHDR tTempFrmHdr;
	memset(&tTempFrmHdr, 0x00, sizeof(tTempFrmHdr));

	tTempFrmHdr.m_byMediaType  = (u8)pFrmHdr->m_dwMediaType;
	tTempFrmHdr.m_dwFrameID    = pFrmHdr->m_dwFrameID;
	tTempFrmHdr.m_byFrameRate  = FRAME_RATE;                //֡�ʹ̶�Ϊ25֡

//	tTempFrmHdr.m_dwSSRC       = 0;                         //���յ�ʱ����Զ�����
	tTempFrmHdr.m_pData        = pFrmHdr->m_pData;
	tTempFrmHdr.m_dwDataSize   = pFrmHdr->m_dwDataSize;
//	tTempFrmHdr.m_dwPreBufSize = 0;
//	tTempFrmHdr.m_dwTimeStamp  = 0;                         //���͵�ʱ����Զ�����
	tTempFrmHdr.m_tVideoParam.m_bKeyFrame    = pFrmHdr->m_tVideoParam.m_bKeyFrame;
	tTempFrmHdr.m_tVideoParam.m_wVideoHeight = pFrmHdr->m_tVideoParam.m_wVideoHeight;
	tTempFrmHdr.m_tVideoParam.m_wVideoWidth  = pFrmHdr->m_tVideoParam.m_wVideoWidth;

	baslogall("[CBSendFrame] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, m_dwDataSize:%u\n",
	    	   tTempFrmHdr.m_byMediaType,
		       tTempFrmHdr.m_dwFrameID,
    		   tTempFrmHdr.m_tVideoParam.m_bKeyFrame,
               tTempFrmHdr.m_tVideoParam.m_wVideoWidth,
	    	   tTempFrmHdr.m_tVideoParam.m_wVideoHeight,	    	   
		       tTempFrmHdr.m_dwDataSize
              );
    
	if (STATUS_HD == g_byAdpMode)
	{
		pMediaSnd->Send(&tTempFrmHdr);
	}
	return;
}

//����
CBasInst::CBasInst()
{	    
    m_pAdpCroup = NULL;
}

CBasInst::~CBasInst()
{
}


/*====================================================================
	����  : DaemonInstanceEntry
	����  : Daemonʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
    ��  ��      �汾        �޸���      �޸�����
    2008/08/04  4.5         ����         ����
====================================================================*/
void CBasInst::DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp)
{
    switch(pMsg->event)
    {
    //�ϵ��ʼ��
    case OSP_POWERON:
		DaemonProcPowerOn(pMsg, pcApp);
		break;

    // ����
    case EV_BAS_CONNECT:
        DaemonProcConnectTimeOut(TRUE);
        break;
	// ����Mcu
    case EV_BAS_CONNECTB:
        DaemonProcConnectTimeOut(FALSE);
        break;

    // ע����Ϣ
    case EV_BAS_REGISTER:  	
        DaemonProcRegisterTimeOut(TRUE);
        break;
	// ע��
    case EV_BAS_REGISTERB:  	
        DaemonProcRegisterTimeOut(FALSE);
        break;

    // MCU ע��Ӧ����Ϣ
    case MCU_VMP_REGISTER_ACK:
        DaemonProcMcuRegAck(pMsg, pcApp);
        break;

    // MCU�ܾ���������ע��
    case MCU_VMP_REGISTER_NACK:
        DaemonProcMcuRegNack(pMsg);
        break;

    // OSP ������Ϣ
    case OSP_DISCONNECT:
        DaemonProcOspDisconnect(pMsg, pcApp);
        break;

    // ����Qosֵ
    case MCU_EQP_SETQOS_CMD:
        DaemonProcSetQosCmd(pMsg);
        break;

    // ȡ��������״̬
    case EV_BAS_GETMSSTATUS:
    case MCU_EQP_GETMSSTATUS_ACK:
        DeamonProcGetMsStatusRsp(pMsg);
		break;

    // ��ʼ����
    case MCU_HDBAS_STARTADAPT_REQ:
    // ֹͣ����
    case MCU_HDBAS_STOPADAPT_REQ:
    // �����������
    case MCU_HDBAS_SETADAPTPARAM_CMD:
    // MCU�����һ���ؼ�֡
    case MCU_BAS_FASTUPDATEPIC_CMD:
        {
            CServMsg cServMsg(pMsg->content, pMsg->length);
            u8 byChnIdx = cServMsg.GetChnIndex();
            u8 byChnId = byChnIdx + 1;
            post( MAKEIID( GetAppID(), byChnId ), pMsg->event, pMsg->content, pMsg->length );
            break;        
        }

    case MCU_VMP_STARTVIDMIX_REQ:
    case MCU_VMP_STOPVIDMIX_REQ:
    case MCU_VMP_CHANGEVIDMIXPARAM_CMD:
    case MCU_VMP_GETVIDMIXPARAM_REQ:
    case MCU_VMP_FASTUPDATEPIC_CMD:
        OspPost(MAKEIID(AID_BAS, 1), pMsg->event, pMsg->content, pMsg->length);
        break;

    default:
        break;
    }
    return;
}

/*====================================================================
	����  : InstanceEntry
	����  : ��ͨʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
    ��  ��      �汾        �޸���      �޸�����
    2008/08/04  4.5         ����         ����
====================================================================*/
void CBasInst::InstanceEntry(CMessage* const pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    cServMsg.SetEventId(pcMsg->event);
    
    if ( pcMsg->event != EV_BAS_NEEDIFRAME_TIMER &&
         pcMsg->event != EV_BAS_STATUSCHK_TIMER )
    {
        baslog("[BasInst.%d] Receive msg.%d<%s>\n", GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event) );
    }

    switch (pcMsg->event)    
    {
    // ��ʼ��ͨ��
    case EV_BAS_INI:
        ProcInitBas();
        break;
    case EV_BAS_QUIT:
        ProcStopBas();        
        break;

    // ��ʼ����
    case MCU_HDBAS_STARTADAPT_REQ:
        ProcStartAdptReq(cServMsg);
        break;
        
        // ֹͣ����
    case MCU_HDBAS_STOPADAPT_REQ:
        ProcStopAdptReq(cServMsg);
        break;
        
        // �����������
    case MCU_HDBAS_SETADAPTPARAM_CMD:
        ProcChangeAdptCmd(cServMsg);
        break;
        
        // �����һ���ؼ�֡
    case MCU_BAS_FASTUPDATEPIC_CMD:
        ProcFastUpdatePicCmd(cServMsg);
        break;
                
        // ��ʾ����״̬    
    case EV_BAS_SHOW:
        StatusShow();
        break;

    case EV_BAS_SENDCHNSTATUS:
        SendChnStatus();
        break;

        /************************************************************************/
        /*                                                                      */
        /*                                HDVMP                                 */
        /*                                                                      */
        /************************************************************************/

    // ��ʼ����
    case MCU_VMP_STARTVIDMIX_REQ:
        MsgStartVidMixProc(pcMsg);
        break;

    // ֹͣ����
    case MCU_VMP_STOPVIDMIX_REQ:
        MsgStopVidMixProc(pcMsg);
        break;
        
    // �����ı�
    case MCU_VMP_CHANGEVIDMIXPARAM_CMD:
        MsgChangeVidMixParamProc(pcMsg);
        break;
        
        // ��ѯ����
    case MCU_VMP_GETVIDMIXPARAM_REQ:
        MsgGetVidMixParamProc(pcMsg);
        break;
        
        // ����Qosֵ
    case MCU_EQP_SETQOS_CMD:
        ProcSetQosInfo(pcMsg);
        break;

        // ǿ�ƹؼ�֡
    case MCU_VMP_FASTUPDATEPIC_CMD:
        MsgFastUpdatePicProc();
        break;

        // �������ؼ�֡��ʱ��
    case EV_BAS_NEEDIFRAME_TIMER:
        ProcTimerNeedIFrame();
        break;

        // ����ͨ������
    case MCU_VMP_SETCHANNELBITRATE_REQ:
        MsgSetBitRate(pcMsg);
        break;

     /*
        // ��ʾ����ͨ����Ϣ
    case EV_BAS_SHOWINFO:
        BasInfo();
		break;
    */

	case EV_BAS_STATUSCHK_TIMER:
		//AdpStatusAdjust();
		break;

    default:
        break;
    }
    return;
}

/*=============================================================================
  �� �� ���� DaemonProcPowerOn
  ��    �ܣ� ��ʼ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
             CApp* pcApp
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
=============================================================================*/
void CBasInst::DaemonProcPowerOn(CMessage* const pcMsg, CApp* pcApp)
{
    if ( pcMsg->length == sizeof(TEqpCfg) )
    {
        memcpy( &g_cBasApp.m_tCfg, pcMsg->content, sizeof(TEqpCfg));
    }

    // guzh [4/9/2007] ��ȡ�����������
    sprintf(BASCFG_FILE, "%s/mcueqp.ini", DIR_CONFIG);
    g_cBasApp.ReadDebugValues();    

    u16 wRet = KdvSocketStartup();
    if ( MEDIANET_NO_ERROR != wRet)
    {
        OspPrintf(TRUE, FALSE, "KdvSocketStartup failed, error: %d\n", wRet);
        //notify user!!!
        return ;
    }

#ifdef _LINUX_
    TAdapterInit tInitParam;
    tInitParam.dwChannelNum = 2;
    wRet = HardInitAdapter(&tInitParam);
    if ( Codec_Success != wRet )
    {
        OspPrintf(TRUE, FALSE, "HardInitAdapter failed, error: %d\n", wRet);
        //notify user!!!
        return ;
    }
#endif	  
    
    // ��ʼ��MCUע��
    if (TRUE == g_cBasApp.m_bEmbedMcu)
    {
        baslog( "the bas embed in Mcu A\n");
        g_cBasApp.m_dwMcuNode = 0;                          //��Ƕʱ�����ڵ����Ϊ0�Ϳ���
        SetTimer(EV_BAS_REGISTER, BAS_REGISTER_TIMEOUT );   //ֱ��ע��
    }
    
    if( TRUE == g_cBasApp.m_bEmbedMcuB)
    {
        baslog("the bas embed in Mcu B\n");
        g_cBasApp.m_dwMcuNodeB = 0;
        SetTimer(EV_BAS_REGISTERB, BAS_REGISTER_TIMEOUT );  //ֱ��ע��
    } 
    
    if(0 != g_cBasApp.m_tCfg.dwConnectIP && FALSE == g_cBasApp.m_bEmbedMcu)
    {
        SetTimer(EV_BAS_CONNECT, BAS_CONNETC_TIMEOUT );     //3s���ٽ���
    }
    if(0 != g_cBasApp.m_tCfg.dwConnectIpB && FALSE == g_cBasApp.m_bEmbedMcuB)
    {
        SetTimer(EV_BAS_CONNECTB, BAS_CONNETC_TIMEOUT );    //3s���ٽ���
    }

	return;
}

/*=============================================================================
  �� �� ���� DaemonProcConnectTimeOut
  ��    �ܣ� ����ע�ᳬʱ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� BOOL32 bIsConnectA
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
=============================================================================*/
void CBasInst::DaemonProcConnectTimeOut(BOOL32 bIsConnectA)
{
    BOOL32 bRet = FALSE;
    if(TRUE == bIsConnectA)
    {
        bRet = ConnectMcu(bIsConnectA, g_cBasApp.m_dwMcuNode);
        if(TRUE == bRet)
        {  
            SetTimer(EV_BAS_REGISTER, BAS_REGISTER_TIMEOUT ); 
        }
        else
        {
            SetTimer(EV_BAS_CONNECT, BAS_CONNETC_TIMEOUT);   //3s���ٽ���
        }
    }
    else
    {
        bRet = ConnectMcu(bIsConnectA, g_cBasApp.m_dwMcuNodeB);
        if(TRUE == bRet)
        { 
            SetTimer(EV_BAS_REGISTERB, BAS_REGISTER_TIMEOUT ); 
        }
        else
        {
            SetTimer(EV_BAS_CONNECTB, BAS_CONNETC_TIMEOUT);   //3s���ٽ���
        }
    }
    return;
}
/*====================================================================
	����  : ConnectMcu
	����  : ��MCU����
	����  : ��
	���  : ��
	����  : ��
	ע    :
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
====================================================================*/
BOOL32 CBasInst::ConnectMcu(BOOL32 bIsConnectA, u32& dwMcuNode)
{
    BOOL32 bRet = TRUE;
    if(!OspIsValidTcpNode(dwMcuNode)) // ���˲��Ϸ��ڵ�
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
		    baslog("Connect Mcu %s Success, node: %d!\n", 
                   bIsConnectA ? "A" : "B", dwMcuNode);
		    ::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());// ������Ϣ����
	    }
	    else 
	    {
		    baslog("Connect to Mcu failed, will retry\n");
            bRet = FALSE;
	    }
    }
    return bRet;
}

/*=============================================================================
  �� �� ���� DaemonProcRegisterTimeOut
  ��    �ܣ� ����ע�ᳬʱ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� BOOL32 bIsRegiterA
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
=============================================================================*/
void CBasInst::DaemonProcRegisterTimeOut(BOOL32 bIsRegiterA)
{
    if(TRUE == bIsRegiterA)
    {
        Register(bIsRegiterA, g_cBasApp.m_dwMcuNode);
        SetTimer(EV_BAS_REGISTER, BAS_REGISTER_TIMEOUT);  //ֱ��ע��
    }
    else
    {
        Register(bIsRegiterA, g_cBasApp.m_dwMcuNodeB);
		SetTimer(EV_BAS_REGISTERB, BAS_REGISTER_TIMEOUT);  //ֱ��ע��
    }
    return;
}

/*==================================================================
	����  : Register
	����  : ��MCUע��
	����  : ��
	���  : ��
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
====================================================================*/
void CBasInst::Register(BOOL32 bIsRegiterA, u32 dwMcuNode)
{
    CServMsg       cSvrMsg;
    TPeriEqpRegReq tReg;

    tReg.SetMcuEqp((u8)g_cBasApp.m_tCfg.wMcuId, g_cBasApp.m_tCfg.byEqpId, g_cBasApp.m_tCfg.byEqpType);
    tReg.SetPeriEqpIpAddr(g_cBasApp.m_tCfg.dwLocalIP);
    tReg.SetEqpType(EQP_TYPE_VMP);
    tReg.SetVersion(DEVVER_HDVMP);
    tReg.SetHDEqp(TRUE);
    tReg.SetStartPort(VMP_EQP_STARTPORT);

    
    cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
        
    post(MAKEIID(AID_MCU_PERIEQPSSN, g_cBasApp.m_tCfg.byEqpId),
            VMP_MCU_REGISTER_REQ,
            cSvrMsg.GetServMsg(),
            cSvrMsg.GetServMsgLen(),
            dwMcuNode);

	return;
}

/*====================================================================
	����  : DaemonProcMcuRegAck 
	����  : MCUӦ��ע����Ϣ������
	����  : ��
	���  : ��
	����  : ��
	ע    :
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
====================================================================*/
void CBasInst::DaemonProcMcuRegAck(CMessage* const pMsg, CApp* pcApp)
{
	if(NULL == pMsg)
    {
        baslog("Recv Reg Ack Msg, but the msg's pointer is Null\n");
        return;
    }

	TPeriEqpRegAck* ptRegAck;

	CServMsg cServMsg(pMsg->content, pMsg->length);
	ptRegAck =(TPeriEqpRegAck*)cServMsg.GetMsgBody();
	
    TPrsTimeSpan tPrsTimeSpan = *(TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck));
	// ���MTU size, zgc, 2007-04-02
	u16 wMTUSize = *(u16*)( cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan) );
	wMTUSize = ntohs( wMTUSize );

    //guzh 2008/08/07
    TEqpBasCfgInfo tCfgInfo;
    memcpy( &tCfgInfo,
            cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan) + sizeof(u16),
            sizeof(tCfgInfo) );

	
    if(pMsg->srcnode == g_cBasApp.m_dwMcuNode) // ע��Ӧ��
    {
	    g_cBasApp.m_dwMcuIId = pMsg->srcid;
        g_cBasApp.m_byRegAckNum++;
        KillTimer(EV_BAS_REGISTER);// �رն�ʱ��

        baslog("[RegAck] Regist success to mcu A\n");
    }
    else if(pMsg->srcnode == g_cBasApp.m_dwMcuNodeB)
    {
        g_cBasApp.m_dwMcuIIdB = pMsg->srcid;
        g_cBasApp.m_byRegAckNum++;

        KillTimer(EV_BAS_REGISTERB);

        baslog("[RegAck] Regist success to mcu B\n");       
    }

	// guzh [6/12/2007] У��Ự����
    if ( g_cBasApp.m_dwMpcSSrc == 0 )
    {
        g_cBasApp.m_dwMpcSSrc = ptRegAck->GetMSSsrc();
    }
    else
    {
        // �쳣������Ͽ������ڵ�
        if ( g_cBasApp.m_dwMpcSSrc != ptRegAck->GetMSSsrc() )
        {
            OspPrintf(TRUE, FALSE, "[RegAck] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
                      g_cBasApp.m_dwMpcSSrc, ptRegAck->GetMSSsrc());
            if ( OspIsValidTcpNode(g_cBasApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cBasApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cBasApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cBasApp.m_dwMcuNodeB);
            }      
            return;
        }
    }
    if(FIRST_REGACK == g_cBasApp.m_byRegAckNum) // ��һ���յ�ע��ɹ���Ϣ
    {
        g_cBasApp.m_dwMcuRcvIp       = ptRegAck->GetMcuIpAddr();
	    g_cBasApp.m_wMcuRcvStartPort = ptRegAck->GetMcuStartPort();

        // ��������ʱû�л�ȡ�������ȡ����һ��MCU�������ȡ��ȥ����
        u32 dwOtherMcuIp = htonl(ptRegAck->GetAnotherMpcIp());
        if ( g_cBasApp.m_tCfg.dwConnectIpB == 0 && dwOtherMcuIp != 0 )
        {
            g_cBasApp.m_tCfg.dwConnectIpB = dwOtherMcuIp;
            SetTimer(EV_BAS_CONNECTB, BAS_CONNETC_TIMEOUT);

            OspPrintf(TRUE, FALSE, "[RegAck] Found another Mpc IP: %s, try connecting...\n", 
                      ipStr(dwOtherMcuIp) );
        }

        //guzh ����û�е��������Ҫ�����ﱣ�汾�ؽ��յ�ַ��������
        g_cBasApp.m_tCfg.wRcvStartPort = tCfgInfo.GetEqpStartPort();
        strncpy(g_cBasApp.m_tCfg.achAlias, tCfgInfo.GetAlias(), MAXLEN_ALIAS);

        baslog("[RegAck] Local Recv Start Port.%u, Mcu Start Port.%u, Alias.%s\n",
               g_cBasApp.m_tCfg.wRcvStartPort, g_cBasApp.m_wMcuRcvStartPort , g_cBasApp.m_tCfg.achAlias);

        g_cBasApp.m_tPrsTimeSpan = tPrsTimeSpan;
        baslog("[RegAck] The Bas Prs span: first: %d, second: %d, three: %d, reject: %d\n", 

                g_cBasApp.m_tPrsTimeSpan.m_wFirstTimeSpan,
                g_cBasApp.m_tPrsTimeSpan.m_wSecondTimeSpan,
                g_cBasApp.m_tPrsTimeSpan.m_wThirdTimeSpan,
                g_cBasApp.m_tPrsTimeSpan.m_wRejectTimeSpan);
        
		// ����MTU size, zgc, 2007-04-02
		g_cBasApp.m_wMTUSize = wMTUSize;
		baslog( "[RegAck]The Network MTU is : %d\n", g_cBasApp.m_wMTUSize );
		//SetVideoSendPacketLen( (s32)g_cBasApp.m_wMTUSize );     

        g_cBasApp.m_tEqp.SetMcuEqp((u8)g_cBasApp.m_tCfg.wMcuId, g_cBasApp.m_tCfg.byEqpId, g_cBasApp.m_tCfg.byEqpType);	


        NEXTSTATE(NORMAL); // DAEMON ʵ������ NORMAL ״̬

        for (u8 byLoop = 1; byLoop <= MAXNUM_BASHD_CHANNEL; byLoop++ )
        {
            post(MAKEIID(GetAppID(), byLoop), EV_BAS_INI);
        }
    }

    TPeriEqpStatus tEqpStatus;
    tEqpStatus.SetMcuEqp((u8)g_cBasApp.m_tCfg.wMcuId, g_cBasApp.m_tCfg.byEqpId, g_cBasApp.m_tCfg.byEqpType);	
    tEqpStatus.m_byOnline = 1;
    tEqpStatus.SetAlias(g_cBasApp.m_tCfg.achAlias);
    cServMsg.SetMsgBody((u8*)&tEqpStatus, sizeof(tEqpStatus));
	SendMsgToMcu(VMP_MCU_VMPSTATUS_NOTIF, cServMsg);

    for (u8 byLoop = 1; byLoop <= MAXNUM_BASHD_CHANNEL; byLoop++ )
    {
        post(MAKEIID(GetAppID(), byLoop), EV_BAS_SENDCHNSTATUS);
    }

    return;
}

/*====================================================================
	����  : DaemonProcMcuRegNack
	����  : MCU �ܾ�ע����Ϣ������(Ŀǰ�Ĳ����Ǳ��ܺ����ע��)
	����  : ��
	���  : ��
	����  : ��
	ע    :
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
====================================================================*/
void CBasInst::DaemonProcMcuRegNack(CMessage* const pMsg)
{
    if( pMsg->srcnode == g_cBasApp.m_dwMcuNode)    
    {
        baslog("Bas registe be refused by A.\n");
    }
    if( pMsg->srcnode == g_cBasApp.m_dwMcuNodeB)    
    {
        baslog("Bas registe be refused by B.\n");
    }
    return;
}

/*====================================================================
	����  : DaemonProcOspDisconnect
	����  : Osp������Ϣ������
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	05/04/11	4.0			liuhuiyun	  ����
====================================================================*/
void CBasInst::DaemonProcOspDisconnect(CMessage* const pMsg, CApp* pcApp)
{
    if ( NULL == pMsg )  
    {
        baslog("message's pointer is Null\n");
        return;
    }

    u32 dwNode = *(u32*)pMsg->content;

    if (INVALID_NODE != dwNode)
    {
        OspDisconnectTcpNode(dwNode);
    } 
    if(dwNode == g_cBasApp.m_dwMcuNode) // ����
    {
        baslog("[DaemonProcOspDisconnect] McuNode.A disconnect\n");
        g_cBasApp.FreeStatusDataA();
        SetTimer(EV_BAS_CONNECT, BAS_CONNETC_TIMEOUT);
    }
    else if(dwNode == g_cBasApp.m_dwMcuNodeB)
    {
        baslog("[DaemonProcOspDisconnect] McuNode.B disconnect\n");
        g_cBasApp.FreeStatusDataB();
        SetTimer(EV_BAS_CONNECTB, BAS_CONNETC_TIMEOUT);
    }
	// ������һ����������Mcuȡ��������״̬���ж��Ƿ�ɹ�
	if (INVALID_NODE != g_cBasApp.m_dwMcuNode || INVALID_NODE != g_cBasApp.m_dwMcuNodeB)
	{
		if (OspIsValidTcpNode(g_cBasApp.m_dwMcuNode))
		{
			post( g_cBasApp.m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cBasApp.m_dwMcuNode );           
			baslog("[DaemonProcOspDisconnect] GetMsStatusReq. McuNode.A\n");
		}
		else if (OspIsValidTcpNode(g_cBasApp.m_dwMcuNodeB))
		{
			post( g_cBasApp.m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cBasApp.m_dwMcuNodeB );        
			baslog("[DaemonProcMcuDisconnect] GetMsStatusReq. McuNode.B\n");
		}

		SetTimer(EV_BAS_GETMSSTATUS, WAITING_MSSTATUS_TIMEOUT);
		return;
	}
	// �����ڵ㶼�ϣ����״̬��Ϣ
    if(INVALID_NODE == g_cBasApp.m_dwMcuNode && INVALID_NODE == g_cBasApp.m_dwMcuNodeB)
    {
        ClearInstStatus();
    }
    return;
}


/*=============================================================================
  �� �� ���� DeamonProcGetMsStatus
  ��    �ܣ� ����ȡ��������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
=============================================================================*/
void CBasInst::DeamonProcGetMsStatusRsp(CMessage* const pMsg)
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg(pMsg->content, pMsg->length);
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event )
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer(EV_BAS_GETMSSTATUS);
        baslog("[DeamonProcGetMsStatus]. receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                ptMsStatus->IsMsSwitchOK());

        if(ptMsStatus->IsMsSwitchOK()) // �����ɹ�
        {
            bSwitchOk = TRUE;
        }
	}

	// ����ʧ�ܻ��߳�ʱ
	if( !bSwitchOk )
	{
		ClearInstStatus();
		if( INVALID_NODE == g_cBasApp.m_dwMcuNode)// �п��ܲ�����������Connect�������
		{
			SetTimer(EV_BAS_CONNECT, BAS_CONNETC_TIMEOUT);
		}
		if( INVALID_NODE == g_cBasApp.m_dwMcuNodeB)
		{
			SetTimer(EV_BAS_CONNECTB, BAS_CONNETC_TIMEOUT);
		}
	}
	return;
}

/*=============================================================================
  �� �� ���� ClearInstStatus
  ��    �ܣ� ��ձ�ʵ�������״̬��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
=============================================================================*/
void CBasInst::ClearInstStatus(void)
{
	CServMsg cServMsg;
	// ��ֹͣ����
    for (u8 byLoop = 1; byLoop <= MAXNUM_BASHD_CHANNEL; byLoop++ )
    {
        post(MAKEIID(GetAppID(), byLoop), EV_BAS_QUIT);
    }


	NEXTSTATE(IDLE);   // DAEMON ʵ���������״̬
    g_cBasApp.m_byRegAckNum = 0;
	g_cBasApp.m_dwMpcSSrc = 0;
	return;
}

/*====================================================================
	����  : SendMsgToMcu
	����  : ��MCU������Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
====================================================================*/
s32 CBasInst::SendMsgToMcu(u16 wEvent, CServMsg& cServMsg)
{
    if (GetInsID() != CInstance::DAEMON)
    {
        cServMsg.SetChnIndex((u8)GetInsID() - 1);
    }
    
	BOOL32 bRet = TRUE;
    if (g_cBasApp.m_bEmbedMcu || OspIsValidTcpNode(g_cBasApp.m_dwMcuNode)) 
	{
		post(g_cBasApp.m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cBasApp.m_dwMcuNode);
		baslog("Send Message %u(%s) to Mcu A\n",
                wEvent, ::OspEventDesc(wEvent));
	}
	else
	{
		bRet = FALSE;
	}

    if(g_cBasApp.m_bEmbedMcuB || OspIsValidTcpNode(g_cBasApp.m_dwMcuNodeB))
    {
	    post(g_cBasApp.m_dwMcuIIdB, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cBasApp.m_dwMcuNodeB);
		baslog("Send Message %u(%s) to Mcu B\n",
               wEvent, ::OspEventDesc(wEvent));
    }
    else
    {
		bRet = FALSE;
    }

    return bRet;
}

/*====================================================================
	����  : SendChnNotif
	����  : ��MCU����ͨ��״̬֪ͨ
	����  : 
	���  : ��
	����  : �ɹ�����TRUE����FALSE
	ע    : 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
====================================================================*/
void CBasInst::SendChnNotif(BOOL32 bUsed)
{
    CServMsg cServMsg;
    TPeriEqpStatus tInitStatus;
    memset(&tInitStatus,0,sizeof(TPeriEqpStatus));
    
    tInitStatus.m_byOnline = TRUE;
    tInitStatus.SetMcuEqp((u8)g_cBasApp.m_tCfg.wMcuId, g_cBasApp.m_tCfg.byEqpId, EQP_TYPE_VMP);
    tInitStatus.SetAlias("baphd");
    tInitStatus.m_tStatus.tVmp.m_tVMPParam.SetVMPStyle(VMP_STYLE_FOUR);
    tInitStatus.m_tStatus.tVmp.m_byUseState = bUsed;
    
    tInitStatus.m_tStatus.tVmp.m_byChlNum = 4;
    cServMsg.SetMsgBody((u8*)&(tInitStatus), sizeof(TPeriEqpStatus));
    SendMsgToMcu(VMP_MCU_VMPSTATUS_NOTIF, cServMsg);

    return;
}

/*====================================================================
	����  : ProcStartAdptReq
	����  : ��ʼ���䴦��
	����  : 
	���  : ��
	����  : ��
	ע    : 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	05/04/11	3.6		    liuhuiyun	  ע������
    2008/08/13  4.5         ����        BAS-HD
====================================================================*/
void CBasInst::ProcStartAdptReq(CServMsg& cServMsg)
{
    return;
}


/*====================================================================
	����  : ProcChangeAdptCmd
	����  : �����������
	����  : 
	���  : ��
	����  : ��
	ע��  : 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
====================================================================*/
void CBasInst::ProcChangeAdptCmd(CServMsg& cServMsg)
{
    return;
}

/*====================================================================
	����  : ProcStopAdptReq
	����  : ֹͣ����
	����  : 
	���  : ��
	����  : ��
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
====================================================================*/
void CBasInst::ProcStopAdptReq(CServMsg& cServMsg)
{
    return;
}



/*====================================================================
	����  : ProcTimerNeedIFrame
	����  : 
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
    ��  ��         �汾        �޸���       �޸�����
    05/08/22       v3.6        libo         ����
====================================================================*/
void CBasInst::ProcTimerNeedIFrame()
{
    CServMsg cServMsg;    
    TAdapterChannelStatis tAdapterChannelStatis;    
    memset(&tAdapterChannelStatis, 0, sizeof(TAdapterChannelStatis));
    static u8 byPrintCount = 1;

    if( byPrintCount > 20)
    {
        byPrintCount = 1;
    }
	if(m_pAdpCroup && m_pAdpCroup->IsStart())
	{
		//fxh ��ͨ���ؼ�֡������Ӧ
		for (u8 byChnNo = 0; byChnNo < MAXNUM_RCV; byChnNo++ )
		{
			m_pAdpCroup->GetVideoChannelStatis(byChnNo, tAdapterChannelStatis);        
			if (tAdapterChannelStatis.m_bVidCompellingIFrm)
			{
				cServMsg.SetConfId( m_tChnInfo.m_cChnConfId );
				cServMsg.SetChnIndex( (u8)tAdapterChannelStatis.dwDecChnl );
				cServMsg.SetSrcSsnId( g_cBasApp.m_tCfg.byEqpId );
				SendMsgToMcu(VMP_MCU_NEEDIFRAME_CMD, cServMsg);

    			baslog("[ProcTimerNeedIFrame]vmp channel:%d request iframe!!\n", byChnNo);
			}
		}
       
    	SetTimer(EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL); //��ʱ���
	}
    return;
}


void CBasInst::AdpStatusAdjust()
{
    return;
}

/*================================================================== : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
    ��  ��         �汾        �޸���       �޸�����
    05/08/22       v3.6        libo         ����
====================================================================*/
void CBasInst::ProcFastUpdatePicCmd(CServMsg& cServMsg)
{
    baslog("[ProcFastUpdatePicCmd for Chn.%d !\n", GetInsID() - 1);

    u32 dwTimeInterval = OspClkRateGet();
    u32 dwLastFUPTick = m_tChnInfo.m_dwLastFUPTick;

    u32 dwCurTick = OspTickGet();	
    if (dwCurTick - dwLastFUPTick > dwTimeInterval)
    {
        m_tChnInfo.m_dwLastFUPTick = dwCurTick;
		if(m_pAdpCroup)
		{
    		m_pAdpCroup->SetFastUpdata();
		}

    }
    else
    {
        baslog("[ProcFastUpdatePicCmd tick intval is %d - %d !\n", dwCurTick , dwLastFUPTick);
    }

    return;
}

void CBasInst::MsgFastUpdatePicProc()
{
    u32 dwTimeInterval = OspClkRateGet();
    u32 dwLastFUPTick = m_tChnInfo.m_dwLastFUPTick;
    
    u32 dwCurTick = OspTickGet();	
    if (dwCurTick - dwLastFUPTick > dwTimeInterval)
    {
        m_tChnInfo.m_dwLastFUPTick = dwCurTick;
        if(m_pAdpCroup)
        {
            m_pAdpCroup->SetFastUpdata();
        }
        
    }
    else
    {
        baslog("[ProcFastUpdatePicCmd tick intval is %d - %d !\n", dwCurTick , dwLastFUPTick);
    }
    return;
}

void CBasInst::ProcSetQosInfo(CMessage * const pcMsg)
{
    if(NULL == pcMsg)
    {
        OspPrintf(TRUE, FALSE, "[Mixer] The Mix's Qos infomation is Null\n");
        return;
    }
    
    TMcuQosCfgInfo  tQosInfo;
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
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
    
    //FIXME: QOS��Ч
    /*
    s32 nRet = SetMediaTOS((s32)byAudioValue, QOS_AUDIO);
    nRet = SetMediaTOS((s32)byVideoValue, QOS_VIDEO);
    nRet = SetMediaTOS((s32)byDataValue, QOS_DATA);
    */
    return;
}


/*====================================================================
    ����  : MsgGetVidMixParamProc
    ����  : MCU��ѯ���渴�ϲ�����Ϣ��Ӧ
    ����  : ��
    ���  : ��
    ����  : ��
    ע    :
    ----------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/20	4.5		    �ű���			create
====================================================================*/
void CBasInst::MsgGetVidMixParamProc(CMessage* const pMsg)
{
    CServMsg cServMsg;
    //cServMsg.SetMsgBody((u8*)&(m_tCfg.m_tStatus), sizeof(m_tCfg.m_tStatus));
    
    SendMsgToMcu(VMP_MCU_GETVIDMIXPARAM_ACK, cServMsg);
    
    baslog("[Info]Get vmp params success!\n");
    
    return;
}

/*====================================================================
    ����  : MsgStartVidMixProc
    ����  : MCU��ѯ���渴�ϲ�����Ϣ��Ӧ
    ����  : ��
    ���  : ��
    ����  : ��
    ע    :
    ----------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/20	4.5		    �ű���			create
====================================================================*/
void CBasInst::MsgStartVidMixProc(CMessage * const pcMsg)
{
    if (NULL == pcMsg->content)
    {
        OspPrintf(TRUE, FALSE, "[MsgStartVidMixProc] The pointer cannot be Null (MsgStartVidMixProc)\n");
        return;
    }
    
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
    CKDVVMPParam tParam[2];    // ������Ƶͨ��
    memset(&tParam, 0, 2 * sizeof(CKDVVMPParam));
    memcpy((u8*)&tParam, cServMsg.GetMsgBody(), 2 * sizeof(CKDVVMPParam));    
    u8 byNeedPrs = *(cServMsg.GetMsgBody() + 2 * sizeof(CKDVVMPParam));

    //���²���������
    TVmpStyleCfgInfo tStyleInfo = *(TVmpStyleCfgInfo*)(cServMsg.GetMsgBody() + 2 * sizeof(CKDVVMPParam) + sizeof(u8));
    TCapSupportEx tCapSupportEx = *(TCapSupportEx*)(cServMsg.GetMsgBody() + 2 * sizeof(CKDVVMPParam) + sizeof(u8) + sizeof(TVmpStyleCfgInfo));
    
    switch(CurState())
    {
    case READY:
    case NORMAL:
        break;
    default:
        OspPrintf(TRUE, FALSE, "[Error]Cannot start merge while in state:%d\n", CurState());
        return ;
    }

    // guzh [4/9/2007] ֧����������
    if ( g_cBasApp.IsEnableCheat() )
    {
        u16 wBitrate = tParam[0].m_wBitRate;
        wBitrate = g_cBasApp.GetDecRateDebug( wBitrate );
        tParam[0].m_wBitRate = wBitrate;
        
        wBitrate = tParam[1].m_wBitRate;
        wBitrate = g_cBasApp.GetDecRateDebug( wBitrate );
        tParam[1].m_wBitRate = wBitrate;
    }

    u16 wRet = 0;
    TVideoEncParam atVidEncPar[2];

    g_cBasApp.GetDefaultParam(tParam[0].m_byEncType, atVidEncPar[0]);

    //���õ�һ·�������
	atVidEncPar[0].m_wVideoHeight          = tParam[0].m_wVideoHeight;
	atVidEncPar[0].m_wVideoWidth           = tParam[0].m_wVideoWidth;
    atVidEncPar[0].m_wBitRate              = tParam[0].m_wBitRate; 
	atVidEncPar[0].m_byFrameRate		   = 30;

	//zbq[09/04/2008] FIXME: ��ʱ�������Ժ������MCU
	if (atVidEncPar[0].m_wVideoWidth == 720 &&
		atVidEncPar[0].m_wVideoHeight == 576)
	{
		atVidEncPar[0].m_wVideoWidth = 704;
	}
	if (atVidEncPar[0].m_byFrameRate == 30)
	{
		atVidEncPar[0].m_byFrameRate = 25;
	}

	//zbq[09/04/2008] VGA˫����С��������������30
	if (m_pAdpCroup->IsVga())
	{
		atVidEncPar[0].m_byMinQuant = 30;
	}

    //zbq[10/10/2008] ���ݷֱ��ʵ������ؼ�֡ ���
    if (atVidEncPar[0].m_wVideoWidth == 1280 &&
		atVidEncPar[0].m_wVideoHeight == 720)
    {
        atVidEncPar[0].m_byMaxKeyFrameInterval = 3000;
    }
    else if(atVidEncPar[0].m_wVideoWidth == 704 &&
			atVidEncPar[0].m_wVideoHeight == 576)
    {
        atVidEncPar[0].m_byMaxKeyFrameInterval = 1000;
    }
    else if (atVidEncPar[0].m_wVideoWidth == 352 &&
			 atVidEncPar[0].m_wVideoHeight == 288)
    {
        atVidEncPar[0].m_byMaxKeyFrameInterval = 500;
    }

    baslog("Start Video Enc Param: %u, W*H: %dx%d, Bitrate.%dKbps, FrmRate.%d, MaxKeyFrmInt.%d\n",
           tParam[0].m_byEncType, 
           atVidEncPar[0].m_wVideoWidth, atVidEncPar[0].m_wVideoHeight,
           atVidEncPar[0].m_wBitRate, atVidEncPar[0].m_byFrameRate,
           atVidEncPar[0].m_byMaxKeyFrameInterval);

    g_cBasApp.GetDefaultParam(tParam[1].m_byEncType, atVidEncPar[1]);
    
    //���õ�һ·�������
    atVidEncPar[1].m_wVideoHeight          = tParam[1].m_wVideoHeight;
    atVidEncPar[1].m_wVideoWidth           = tParam[1].m_wVideoWidth;
    atVidEncPar[1].m_wBitRate              = tParam[1].m_wBitRate;  
	atVidEncPar[1].m_byFrameRate		   = 25;

	//zbq[09/04/2008] FIXME: ��ʱ�������Ժ������MCU
	if (atVidEncPar[1].m_wVideoWidth == 720 &&
		atVidEncPar[1].m_wVideoHeight == 576)
	{
		atVidEncPar[1].m_wVideoWidth = 704;
	}
	if (atVidEncPar[1].m_byFrameRate == 30)
	{
		atVidEncPar[1].m_byFrameRate = 25;
	}

    //zbq[10/10/2008] ���ݷֱ��ʵ������ؼ�֡ ���
    if (atVidEncPar[1].m_wVideoWidth == 1280 &&
        atVidEncPar[1].m_wVideoHeight == 720)
    {
        atVidEncPar[1].m_byMaxKeyFrameInterval = 3000;
    }
    else if(atVidEncPar[1].m_wVideoWidth == 704 &&
            atVidEncPar[1].m_wVideoHeight == 576)
    {
        atVidEncPar[1].m_byMaxKeyFrameInterval = 1000;
    }
    else if (atVidEncPar[1].m_wVideoWidth == 352 &&
             atVidEncPar[1].m_wVideoHeight == 288)
    {
        atVidEncPar[1].m_byMaxKeyFrameInterval = 500;
    }

    baslog("Start Video Enc Param2: %u, W*H: %dx%d, Bitrate.%dKbps, FrmRate.%d\n",
           tParam[1].m_byEncType, 
           atVidEncPar[1].m_wVideoWidth, 
           atVidEncPar[1].m_wVideoHeight,
           atVidEncPar[1].m_wBitRate, 
           atVidEncPar[1].m_byFrameRate,
           atVidEncPar[1].m_byMaxKeyFrameInterval);

	if(TRUE == m_pAdpCroup->SetVideoEncParam(atVidEncPar))
	{
		// ��¼ͨ��״̬
	    THDAdaptParam* ptAdpParam = m_tChnInfo.m_tChnStatus.GetOutputVidParam(0);

	    if(NULL != ptAdpParam)
		{
		    ptAdpParam->SetAudType(MEDIA_TYPE_NULL);
		    ptAdpParam->SetVidType(tParam[0].m_byEncType);
		    ptAdpParam->SetBitRate(tParam[0].m_wBitRate);
			ptAdpParam->SetResolution(tParam[0].m_wVideoWidth, tParam[0].m_wVideoHeight);
			ptAdpParam->SetFrameRate(30);
		}

        THDAdaptParam* ptAdpParam1 = m_tChnInfo.m_tChnStatus.GetOutputVidParam(1);
        
        if(NULL != ptAdpParam1)
        {
            ptAdpParam1->SetAudType(MEDIA_TYPE_NULL);
            ptAdpParam1->SetVidType(tParam[1].m_byEncType);
            ptAdpParam1->SetBitRate(tParam[1].m_wBitRate);
            ptAdpParam1->SetResolution(tParam[1].m_wVideoWidth, tParam[1].m_wVideoHeight);
			ptAdpParam1->SetFrameRate(25);
		}
	}

	//���ü���
	m_pAdpCroup->SetVidEncryptKey(tParam[0].m_tVideoEncrypt[0]);

	//���ü���
	m_pAdpCroup->SetSecVidEncryptKey(tParam[0].m_tVideoEncrypt[0]);

	//���ö�̬�غ�
    for(u8 byIdx = 0; byIdx < MAXNUM_RCV; byIdx++)
    {
	    m_pAdpCroup->SetVideoActivePT(byIdx, 
                                      tParam[0].m_tDoublePayload[byIdx].GetActivePayload(), 
                                      tParam[0].m_tDoublePayload[byIdx].GetRealPayLoad() );
        baslog( "[StartMix] RcvActive/Real PT.<%d, %d>\n", 
                                    tParam[0].m_tDoublePayload[byIdx].GetActivePayload(),
                                    tParam[0].m_tDoublePayload[byIdx].GetRealPayLoad() );
    }

    //���ý���
    m_pAdpCroup->SetVidDecryptKey(tParam[0].m_tVideoEncrypt[0]);


	m_tChnInfo.m_cChnConfId = cServMsg.GetConfId();

	// �趪���ش�����
	TRSParam tNetRSParam;
    if (byNeedPrs)		//�Ƿ���Ҫ���ش��� 1: �ش�, 0: ���ش�
	{		
		baslog("\nBas is needed by Prs!\n");
        tNetRSParam.m_wFirstTimeSpan  = 40/*g_cBasApp.m_tPrsTimeSpan.m_wFirstTimeSpan*/;
		tNetRSParam.m_wSecondTimeSpan = 120/*g_cBasApp.m_tPrsTimeSpan.m_wSecondTimeSpan*/;
		tNetRSParam.m_wThirdTimeSpan  = 240/*g_cBasApp.m_tPrsTimeSpan.m_wThirdTimeSpan*/;
		tNetRSParam.m_wRejectTimeSpan = 640/*g_cBasApp.m_tPrsTimeSpan.m_wRejectTimeSpan*/;
		m_pAdpCroup->SetNetSendFeedbackVideoParam(2000, TRUE);
		m_pAdpCroup->SetNetSecSendFeedbackVideoParam(2000, TRUE);
		m_pAdpCroup->SetNetRecvFeedbackVideoParam(tNetRSParam, TRUE);
	}
	else
	{
		baslog("\nBas is not needed by Prs!\n");
        tNetRSParam.m_wFirstTimeSpan  = 0;
		tNetRSParam.m_wSecondTimeSpan = 0;
		tNetRSParam.m_wThirdTimeSpan  = 0;
		tNetRSParam.m_wRejectTimeSpan = 0;
		m_pAdpCroup->SetNetSendFeedbackVideoParam(2000, FALSE);
		m_pAdpCroup->SetNetSecSendFeedbackVideoParam(2000, FALSE);
		m_pAdpCroup->SetNetRecvFeedbackVideoParam(tNetRSParam, FALSE);
	}

	//zbq[09/03/2008] ��ʱ��������
	if(!m_pAdpCroup->IsStart())
	{
		BOOL bStartOk = m_pAdpCroup->StartAdapter(TRUE);
		if ( !bStartOk )
		{
			cServMsg.SetErrorCode(ERR_BAS_OPMAP);
			cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp, sizeof(g_cBasApp.m_tEqp));
			SendMsgToMcu(cServMsg.GetEventId() + 2, cServMsg);
			return;
		}
        else
        {
            //���ֺ���
            for (u8 byIdx = 0; byIdx < MAXNUM_RCV; byIdx++)
            {
                if (tParam[0].m_atMtMember[byIdx].IsNull())
                {
                    m_pAdpCroup->m_cAptGrp.RemoveVideoChannel(byIdx);
                    baslog("[StartMix] MemIdx.%d has been removed!\n", byIdx);
                }
            }
        }
	}

    // ƽ������
    if ( g_cBasApp.m_bIsUseSmoothSend )
    {
        m_pAdpCroup->SetSmoothSendRule();				
	}

    cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp, sizeof(g_cBasApp.m_tEqp));
    SendMsgToMcu(cServMsg.GetEventId() + 1, cServMsg);
    
    SendMsgToMcu(VMP_MCU_STARTVIDMIX_NOTIF, cServMsg);

    NextState(NORMAL);            
    SendChnNotif(TRUE);
    return;
}


void CBasInst::MsgStopVidMixProc(CMessage * const pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    cServMsg.SetMsgBody(NULL, 0);

    BOOL32 wRet = FALSE;

    switch(CurState())
    {
    case NORMAL:
        wRet = m_pAdpCroup->StopAdapter();
        if( TRUE != wRet )
        {	
            cServMsg.SetErrorCode(ERR_BAS_OPMAP);
            SendMsgToMcu(cServMsg.GetEventId() + 2, cServMsg);
            baslog("Remove channel %d failed when stop adpt, error: %d\n", GetInsID()-1, wRet);
            return;				           
        }        		
        
        //cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp, sizeof(g_cBasApp.m_tEqp));
        SendMsgToMcu(cServMsg.GetEventId() + 1, cServMsg);
        
        SendMsgToMcu(VMP_MCU_STOPVIDMIX_NOTIF, cServMsg);

        NextState(READY);
        
        SendChnNotif(FALSE);
        break;
    case READY:
        {
            cServMsg.SetErrorCode(ERR_BAS_CHREADY);
            //cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp,sizeof(g_cBasApp.m_tEqp));
            SendMsgToMcu(cServMsg.GetEventId() + 2, cServMsg);
            return;
        }
        break;
        
    default:
        cServMsg.SetErrorCode(ERR_BAS_CHNOTREAD);
        //cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp, sizeof(g_cBasApp.m_tEqp));
        SendMsgToMcu(cServMsg.GetEventId() + 2, cServMsg);
        break;
    }
}

void CBasInst::MsgChangeVidMixParamProc(CMessage * const pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    CKDVVMPParam tParam[2];
    CKDVVMPParam *ptParm = &tParam[0];
    CKDVVMPParam *ptParm1 = &tParam[1];
    
    switch(CurState())
    {
    case NORMAL:
    case READY:
        break;
    default:
        OspPrintf( TRUE, FALSE, "[MsgChangeVidMixParamProc] State:%d\n", CurState());
        return;
    }
    
    memcpy((u8*)ptParm, cServMsg.GetMsgBody(), 2 * sizeof(CKDVVMPParam));
    u8 byNeedPrs = *(cServMsg.GetMsgBody() + 2 * sizeof(CKDVVMPParam));
    TVmpStyleCfgInfo tStyleInfo = *(TVmpStyleCfgInfo*)(cServMsg.GetMsgBody() + 2 * sizeof(CKDVVMPParam) + sizeof(u8));

    // guzh [4/9/2007] ֧����������
    if ( g_cBasApp.IsEnableCheat() )
    {
        u16 wBitrate = tParam[0].m_wBitRate;
        wBitrate = g_cBasApp.GetDecRateDebug( wBitrate );
        tParam[0].m_wBitRate =(wBitrate);
        
        wBitrate = tParam[1].m_wBitRate;
        wBitrate = g_cBasApp.GetDecRateDebug( wBitrate );
        tParam[1].m_wBitRate = (wBitrate);
    }


    TVideoEncParam atVidParam[2];
    memset(&atVidParam[0], 0, sizeof(TVideoEncParam));
    
	// guzh [5/17/2007] ���������ļ�ֵ
    atVidParam[0].m_byEncType = ptParm->m_byEncType;            
    g_cBasApp.GetDefaultParam(atVidParam[0].m_byEncType, atVidParam[0]);

    atVidParam[0].m_wVideoHeight  = ptParm->m_wVideoHeight;
    atVidParam[0].m_wVideoWidth   = ptParm->m_wVideoWidth;
    atVidParam[0].m_wBitRate      = ptParm->m_wBitRate;
	atVidParam[0].m_byFrameRate   = 30;

	//zbq[09/04/2008] FIXME: ��ʱ�������Ժ������MCU
	if (atVidParam[0].m_wVideoWidth == 720 &&
		atVidParam[0].m_wVideoHeight == 576)
	{
		atVidParam[0].m_wVideoWidth = 704;
	}
	if (atVidParam[0].m_byFrameRate == 30)
	{
		atVidParam[0].m_byFrameRate = 25;
	}
	
	//zbq[09/04/2008] VGA˫����С��������������30
	if (m_pAdpCroup->IsVga())
	{
		atVidParam[0].m_byMinQuant = 30;
	}

    
    //zbq[10/10/2008] ���ݷֱ��ʵ������ؼ�֡ ���
    if (atVidParam[0].m_wVideoWidth == 1280 &&
        atVidParam[0].m_wVideoHeight == 720)
    {
        atVidParam[0].m_byMaxKeyFrameInterval = 3000;
    }
    else if(atVidParam[0].m_wVideoWidth == 704 &&
            atVidParam[0].m_wVideoHeight == 576)
    {
        atVidParam[0].m_byMaxKeyFrameInterval = 1000;
    }
    else if (atVidParam[0].m_wVideoWidth == 352 &&
             atVidParam[0].m_wVideoHeight == 288)
    {
        atVidParam[0].m_byMaxKeyFrameInterval = 500;
    }
    
    baslog("Change Video Enc Param: %u, W*H: %dx%d, Bitrate.%dKbps, wFrmRate.%d, MaxKeyFrmInt.%d\n",
            atVidParam[0].m_byEncType,                
            atVidParam[0].m_wVideoHeight,
            atVidParam[0].m_wVideoWidth,
            atVidParam[0].m_wBitRate,
			atVidParam[0].m_byFrameRate,
            atVidParam[0].m_byMaxKeyFrameInterval);

    memset(&atVidParam[1], 0, sizeof(TVideoEncParam));

    // guzh [5/17/2007] ���������ļ�ֵ
    atVidParam[1].m_byEncType     = ptParm1->m_byEncType;            
    g_cBasApp.GetDefaultParam(atVidParam[1].m_byEncType, atVidParam[1]);

    atVidParam[1].m_wVideoHeight  = ptParm1->m_wVideoHeight;
    atVidParam[1].m_wVideoWidth   = ptParm1->m_wVideoWidth;
    atVidParam[1].m_wBitRate      = ptParm1->m_wBitRate; 
	atVidParam[1].m_byFrameRate   = 25;

	//zbq[09/04/2008] FIXME: ��ʱ�������Ժ������MCU
	if (atVidParam[1].m_wVideoWidth == 720 &&
		atVidParam[1].m_wVideoHeight == 576)
	{
		atVidParam[1].m_wVideoWidth = 704;
	}
	if (atVidParam[1].m_byFrameRate == 30)
	{
		atVidParam[1].m_byFrameRate = 25;
	}
	
    //zbq[10/10/2008] ���ݷֱ��ʵ������ؼ�֡ ���
    if (atVidParam[1].m_wVideoWidth == 1280 &&
        atVidParam[1].m_wVideoHeight == 720)
    {
        atVidParam[1].m_byMaxKeyFrameInterval = 3000;
    }
    else if(atVidParam[1].m_wVideoWidth == 704 &&
            atVidParam[1].m_wVideoHeight == 576)
    {
        atVidParam[1].m_byMaxKeyFrameInterval = 1000;
    }
    else if (atVidParam[1].m_wVideoWidth == 352 &&
             atVidParam[1].m_wVideoHeight == 288)
    {
        atVidParam[1].m_byMaxKeyFrameInterval = 500;
    }

    baslog("Change Video Enc Param2: %u, W*H: %dx%d, Bitrate.%dKbps, FrmRate.%d, KeyInt.%d\n",
            atVidParam[1].m_byEncType,                
            atVidParam[1].m_wVideoHeight,
            atVidParam[1].m_wVideoWidth,
            atVidParam[1].m_wBitRate,
			atVidParam[1].m_byFrameRate,
            atVidParam[1].m_byMaxKeyFrameInterval);
    
    {
        //zbq[10/09/2008] �ݴ���
        BOOL32 bStartAdp = FALSE;
		if(m_pAdpCroup->IsStart() == FALSE)
		{
			m_pAdpCroup->StartAdapter(TRUE);
            bStartAdp = TRUE;

            //���ֺ���
            for (u8 byIdx = 0; byIdx < MAXNUM_RCV; byIdx++)
            {
                if (ptParm->m_atMtMember[byIdx].IsNull())
                {
                    m_pAdpCroup->m_cAptGrp.RemoveVideoChannel(byIdx);
                    baslog("[ChangeMix] MemIdx.%d has been removed!\n", byIdx);
                }
                else
                {
                    m_pAdpCroup->m_cAptGrp.AddVideoChannel(NULL, byIdx);
                    baslog("[ChangeMix] MemIdx.%d has been added again!\n", byIdx);
                }
            }
		}
        else
        {
            //zbq[02/06/2009] �Ѿ��������������������
            for (u8 byIdx = 0; byIdx < MAXNUM_RCV; byIdx++)
            {
                if (ptParm->m_atMtMember[byIdx].IsNull())
                {
                    m_pAdpCroup->m_cAptGrp.RemoveVideoChannel(byIdx);
                    baslog("[ChangeMix] MemIdx.%d has been removed!\n", byIdx);
                }
                else
                {
                    m_pAdpCroup->m_cAptGrp.AddVideoChannel(NULL, byIdx);
                    baslog("[ChangeMix] MemIdx.%d has been added again!\n", byIdx);
                }
            }
        }

        BOOL bRet = m_pAdpCroup->SetVideoEncParam(atVidParam, bStartAdp);  
        if (TRUE != bRet)
		{
			baslog("[ChangeMix] Change Video Enc Param failed!(%d)\n", bRet);
            return;
		}

        //���ü���
        m_pAdpCroup->SetVidEncryptKey(ptParm->m_tVideoEncrypt[0]);
        
        //���ü���
        m_pAdpCroup->SetSecVidEncryptKey(ptParm->m_tVideoEncrypt[0]);
        
        //���ý���
        m_pAdpCroup->SetVidDecryptKey(tParam[0].m_tVideoEncrypt[0]);
        
        //���ö�̬�غ�
        u8 byIdx = 0;
        for ( ; byIdx < MAXNUM_RCV; byIdx ++)
        {
            m_pAdpCroup->SetVideoActivePT(byIdx,
                                          ptParm->m_tDoublePayload[byIdx].GetActivePayload(),
                                          ptParm->m_tDoublePayload[byIdx].GetRealPayLoad());

            baslog( "[ChangeMix] RcvActive/Real PT.<%d, %d>\n", 
                                        ptParm->m_tDoublePayload[byIdx].GetActivePayload(),
                                        ptParm->m_tDoublePayload[byIdx].GetRealPayLoad());
        }


		// ��¼ͨ��״̬
	    THDAdaptParam* ptAdpParam = m_tChnInfo.m_tChnStatus.GetOutputVidParam(0);

	    if(NULL != ptAdpParam)
		{
		    ptAdpParam->SetAudType(MEDIA_TYPE_NULL);
	        ptAdpParam->SetVidType(ptParm->m_byEncType);
		    ptAdpParam->SetBitRate(ptParm->m_wBitRate);
		    ptAdpParam->SetResolution(ptParm->m_wVideoWidth, ptParm->m_wVideoHeight);
			ptAdpParam->SetFrameRate(30);
		}

        baslog("The Input Param is: video: %d, BitRate: %d, Width: %d, Height: %d, FrmRate.%d\n",
                ptAdpParam->GetVidType(), ptAdpParam->GetBitrate(),
                ptAdpParam->GetWidth(), ptAdpParam->GetHeight(), ptAdpParam->GetFrameRate());

        if(NULL != ptAdpParam)
        {
            ptAdpParam->SetAudType(MEDIA_TYPE_NULL);
	        ptAdpParam->SetVidType(ptParm1->m_byEncType);
            ptAdpParam->SetBitRate(ptParm1->m_wBitRate);
   	        ptAdpParam->SetResolution(ptParm1->m_wVideoWidth, ptParm1->m_wVideoHeight);
			ptAdpParam->SetFrameRate(25);
        }
        
        baslog("The Input Param2 is: video: %d, BitRate: %d, Width: %d, Height: %d, FrmRate.%d\n",
                 ptAdpParam->GetVidType(), ptAdpParam->GetBitrate(),
                 ptAdpParam->GetWidth(), ptAdpParam->GetHeight(), ptAdpParam->GetFrameRate());

        // ���������ش�
        TRSParam tNetRSParam;
        if (byNeedPrs)		//�Ƿ���Ҫ���ش��� 1: �ش�, 0: ���ش�
        {		
            baslog("\nBas is needed by Prs!\n");
            tNetRSParam.m_wFirstTimeSpan  = 40;//g_cBasApp.m_tPrsTimeSpan.m_wFirstTimeSpan;
            tNetRSParam.m_wSecondTimeSpan = 120;//g_cBasApp.m_tPrsTimeSpan.m_wSecondTimeSpan;
            tNetRSParam.m_wThirdTimeSpan  = 240;//g_cBasApp.m_tPrsTimeSpan.m_wThirdTimeSpan;
            tNetRSParam.m_wRejectTimeSpan = 640;//g_cBasApp.m_tPrsTimeSpan.m_wRejectTimeSpan;
            m_pAdpCroup->SetNetSendFeedbackVideoParam(2000, TRUE);
            m_pAdpCroup->SetNetSecSendFeedbackVideoParam(2000, TRUE);
            m_pAdpCroup->SetNetRecvFeedbackVideoParam(tNetRSParam, TRUE);
        }
        else
        {
            baslog("\nBas is not needed by Prs!\n");
            tNetRSParam.m_wFirstTimeSpan  = 0;
            tNetRSParam.m_wSecondTimeSpan = 0;
            tNetRSParam.m_wThirdTimeSpan  = 0;
            tNetRSParam.m_wRejectTimeSpan = 0;
            m_pAdpCroup->SetNetSendFeedbackVideoParam(2000, FALSE);
            m_pAdpCroup->SetNetSecSendFeedbackVideoParam(2000, FALSE);
            m_pAdpCroup->SetNetRecvFeedbackVideoParam(tNetRSParam, FALSE);
		}

		if ( g_cBasApp.m_bIsUseSmoothSend )
		{
			// ��ֹͣ����������, zgc, 2007-09-21
			m_pAdpCroup->StopSmoothSend();

		    // ��������ƽ�����͹���, zgc, 2007-09-19
			m_pAdpCroup->SetSmoothSendRule();				
		}
    }   

    cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp,sizeof(g_cBasApp.m_tEqp));

    SendMsgToMcu(cServMsg.GetEventId() + 1, cServMsg);

    SendMsgToMcu(VMP_MCU_CHANGESTATUS_NOTIF, cServMsg);

    SendChnNotif(TRUE);

    return;
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
09/05/05    4.5         �ű���          �ӱ���vmplib��ֲ����
====================================================================*/
void CBasInst::MsgSetBitRate(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    u8 byChalNum = cServMsg.GetChnIndex();
    u16 wBitrate = 0;
    u16 wCheatBit = 0;
    
    cServMsg.GetMsgBody((u8*)&wBitrate, sizeof(u16));
    wBitrate = ntohs(wBitrate);
    
    baslog("[MsgSetBitRate] now we set the encode-%u bitrate to %u!\n", byChalNum, wBitrate);
    
    // guzh [10/18/2007] ֧����������
    if ( g_cBasApp.IsEnableCheat() )
    {
        wCheatBit = g_cBasApp.GetDecRateDebug( wBitrate );
    }

    if (NULL == m_pAdpCroup)
    {
        OspPrintf(TRUE, FALSE, "[MsgSetBitRate] NULL == m_pAdpCroup, failed!\n");
        return;
    }
    
    TVideoEncParam atVidEncParam[2];
    memset(&atVidEncParam, 0, sizeof(atVidEncParam));

    m_pAdpCroup->GetVideoEncParam(atVidEncParam[0]);
    m_pAdpCroup->GetSecVideoEncParam(atVidEncParam[1]);

    atVidEncParam[0].m_wBitRate = wCheatBit;
    atVidEncParam[1].m_wBitRate = wCheatBit;

    if (!m_pAdpCroup->SetVideoEncParam(atVidEncParam, FALSE))
    {
        OspPrintf(TRUE, FALSE, "[MsgSetBitRate] SetVideoEncParam failed!\n");
        return;
    }
    
    if ( g_cBasApp.m_bIsUseSmoothSend )
    {
        m_pAdpCroup->StopSmoothSend();
        m_pAdpCroup->SetSmoothSendRule();				
	}

    SendMsgToMcu(VMP_MCU_SETCHANNELBITRATE_ACK, cServMsg);

    return;
}


/*====================================================================
	����  : ProcStopAdptReq
	����  : ֹͣ����
	����  : 
	���  : ��
	����  : ��
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
====================================================================*/
void CBasInst::ProcInitBas()
{

    s8    achProfileName[32] = {0};
    BOOL  bResult = FALSE;    
    sprintf( achProfileName, "%s/%s", DIR_CONFIG, "mcueqp.ini");
    
    //RebootTimes
    s32 nValue = 0;
    bResult = GetRegKeyInt( achProfileName, "EQPVMP", "fpgaErrTimes", 0, &nValue );
    if( bResult == FALSE )  
    {
        printf("[ProcInitBas] Wrong profile while reading %s!\n", "fpgaErrTimes" );
    }
    u8 byFpgaErrTimes = (u8)nValue;


    BOOL bVga = (GetInsID() == 2);
	
    BOOL bRet = TRUE;

    u16 wRecvPort = g_cBasApp.m_tCfg.wRcvStartPort + (GetInsID() - 1)*PORTSPAN*2;    
    u16 wDstPort = g_cBasApp.m_wMcuRcvStartPort + (GetInsID() - 1)*PORTSPAN*2;

    if ( m_pAdpCroup == NULL )
    {
        // start new
		{
            m_pAdpCroup = new CHdAdpGroup( bVga );
            bRet = m_pAdpCroup->Create( MAX_VIDEO_FRAME_SIZE, 
                                    wRecvPort + 1,
                                    wRecvPort,
                                    g_cBasApp.m_dwMcuRcvIp,
                                    wDstPort,
                                    byFpgaErrTimes,
									(u32)this);
		}
	}
    else
    {
        //zbq[05/16/2009] ˢ���������, �Ի�ȡ���µ�mcuת����ַ
        m_pAdpCroup->UpdateNetParam(wRecvPort + 1,
                                    wRecvPort,
                                    g_cBasApp.m_dwMcuRcvIp,
                                    wDstPort);
    }
    
    if (bRet)
    {
        NextState(READY);
        SetTimer(EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);   // ��ʱ��ѯ�������Ƿ���Ҫ�ؼ�֡
    }
    
// 	if (!bVga)
// 	{
// 		SetTimer(EV_BAS_STATUSCHK_TIMER, 100);
// 	}

    SendChnNotif(FALSE);    
  
    return ;
}

/*====================================================================
	����  : ProcStopAdptReq
	����  : ֹͣ����
	����  : 
	���  : ��
	����  : ��
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
====================================================================*/
void CBasInst::ProcStopBas()
{
    KillTimer(EV_BAS_NEEDIFRAME_TIMER);

    NextState(IDLE);

    SendChnNotif(FALSE);
}

/*====================================================================
	����  : StatusShow
	����  : ��ʾ������״̬
	����  : 
	���  : ��
	����  : ��
	ע    : 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void CBasInst::StatusShow()
{
    if ( GetInsID() != CInstance::DAEMON)
    {
        if(m_pAdpCroup)
        {
            m_pAdpCroup->ShowChnInfo();
        }
        else
        {
            OspPrintf(TRUE, FALSE, "m_pAdpCroup is NULL\n");
        }
    }

	OspPrintf( TRUE, FALSE, "[StatusShow] g_byAdpMode.%d\n", g_byAdpMode );
	
    return;
}

/*=============================================================================
�� �� ���� SendChnStatus
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/8/14   4.0		�ܹ��                  ����
=============================================================================*/
void CBasInst::SendChnStatus(void)
{
    if ( GetInsID() != CInstance::DAEMON)
    {
        SendChnNotif(FALSE);
    }
    
    return;
}

/*=============================================================================
  �� �� ���� DaemonProcSetQosCmd
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBasInst::DaemonProcSetQosCmd(CMessage* const pMsg)
{

    if(NULL == pMsg)
    {
        baslog("The Bas's Qos infomation is Null\n");
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
	baslog("Type: %d, Aud= %d, Vid= %d, Data= %d, IpPrior= %d\n", 
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

	baslog("\nAud= %d, Vid= %d, Data= %d\n", 
					byAudioValue, byVideoValue, byDataValue);
    
    /*
    SetMediaTOS((s32)byAudioValue, QOS_AUDIO);
    SetMediaTOS((s32)byVideoValue, QOS_VIDEO);
    SetMediaTOS((s32)byDataValue,  QOS_DATA );
    */

    return;
}

/////////////////////////////////////////////////////////////////
/////////// Qosֵ��Ϲ���
/////////////////////////////////////////////////////////////////
/*
(u8)nTOS�ĸ�λ���壺
0Ϊ���λ

IP���ȷ�ʽ��

   0     1     2     3     4     5     6     7
+-----+-----+-----+-----+-----+-----+-----+-----+
|                 |     |     |     |     |     |
|   PRECEDENCE    |  D  |  T  |  R  |  0  |  0  |
|                 |     |     |     |     |     |
+-----+-----+-----+-----+-----+-----+-----+-----+

Bits 0-2: Precedence.
Bit  3:   0 = Normal Delay, 1 = Low Delay.
Bits 4:   0 = Normal Throughput, 1 = High Throughput.
Bits 5:   0 = Normal Relibility, 1 = High Relibility.
Bit 6-7:  Reserved for Future Use.

Precedence:
111 - Network Control
110 - Internetwork Control
101 - CRITIC/ECP
100 - Flash Override
011 - Flash
010 - Immediate
001 - Priority
000 - Routine


0Ϊ���λ

�������ʽ��

   0     1     2     3     4     5     6     7
+-----+-----+-----+-----+-----+-----+-----+-----+
|                                   |     |     |
|   	���ַ���      	            |  0  |  0  |
|                                   |     |     |
+-----+-----+-----+-----+-----+-----+-----+-----+

���ַ���:0-63
//////////////////////////////////////////////////////////////////////////////
*/

/*=============================================================================
  �� �� ���� ComplexQos
  ��    �ܣ� ���Qosֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8& byValue
             u8 byPrior
  �� �� ֵ�� void 
=============================================================================*/
void CBasInst::ComplexQos(u8& byValue, u8 byPrior)
{
    u8 byLBit = ((byPrior & 0x08) >> 3);
    u8 byRBit = ((byPrior & 0x04) >> 1);
    u8 byTBit = ((byPrior & 0x02) << 1);
    u8 byDBit = ((byPrior & 0x01) << 3);
    
    byValue = byValue + ((byDBit + byTBit + byRBit + byLBit)*2);
    return;
}

/*====================================================================
	����  : GetMediaType
	����  : ȡ��ý������
	����  : nType: ý������
	���  : ��
	����  : ý�����Ͷ�Ӧ���ַ���
	ע    : 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
BOOL32 CBasInst::GetMediaType(u8 byType, s8* pDstType)
{    
    if ( NULL == pDstType )
    {
        baslog("[GetMediaType] the pointer should not be NULL !\n");
        return FALSE;
    }

    memset(pDstType, 0, LENGTH_TYPE);

	switch(byType)
	{
    //audio
	case MEDIA_TYPE_MP3:                //mp3 mode 0-4
		sprintf(pDstType, "mp3\0");
		break;
		
	case MEDIA_TYPE_PCMA:               //G.711 Alaw  mode 5
		sprintf(pDstType, "G.711A\0");
		break;
		
	case MEDIA_TYPE_PCMU:               //G.711 ulaw  mode 6
		sprintf(pDstType, "G.711U\0");
		break;

	case MEDIA_TYPE_G721:               //G.721
		sprintf(pDstType, "G.721\0");
		break;
	case MEDIA_TYPE_G722:               //G.722
		sprintf(pDstType, "G.722\0");
		break;
	case MEDIA_TYPE_G7231:              //G.7231
		sprintf(pDstType, "G.7231\0");
		break;
	case MEDIA_TYPE_G728:               //G.728
		sprintf(pDstType, "G.728\0");
		break;
	case MEDIA_TYPE_G729:               //G.729
		sprintf(pDstType, "G.729\0");
		break;
	case MEDIA_TYPE_G7221C:              //G.7221
		sprintf(pDstType, "G.7221c\0");
		break;
    case MEDIA_TYPE_AACLC:
        sprintf( pDstType, "AAC-LC\0" );  //AAC-LC
        break;
    case MEDIA_TYPE_AACLD:
        sprintf( pDstType, "AAC-LD\0" );  //AAC-LD
        break;		
    //video
	case MEDIA_TYPE_MP4:                //MPEG-4
		sprintf(pDstType, "MPG4\0");
		break;
	case MEDIA_TYPE_H261:               //H.261
		sprintf(pDstType, "H.261\0");
		break;
	case MEDIA_TYPE_H262:               //H.262(MPEG-2)
		sprintf(pDstType, "MPG2\0");
		break;
	case MEDIA_TYPE_H263:               //H.263
		sprintf(pDstType, "H.263\0");
		break;
	case MEDIA_TYPE_H264:               //H.264
		sprintf(pDstType, "H.264\0");
		break;
    case MEDIA_TYPE_NULL:
		sprintf(pDstType, "NULL\0");
        break;
	default:
		sprintf(pDstType, "Unknown(%d)\0", byType); // δ����
		break;
	}
	return TRUE;
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
====================================================================*/
s8* ipStr(u32 dwIP)
{
	static s8 achIPStr[4][32]={0};
	static s8 byIdx = 0;
    struct in_addr in;

	byIdx++;
	byIdx %= sizeof(achIPStr)/sizeof(achIPStr[0]);
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


CBasCfg::CBasCfg()
{
    m_dwMcuRcvIp  = 0;

    m_wMcuRcvStartPort  = 0;

    m_bEmbedMcu  = 0;
    m_bEmbedMcuB = 0;

    m_byRegAckNum = 0;
	m_dwMpcSSrc  = 0;
    FreeStatusDataA();
    FreeStatusDataB();
    memset( &m_tEqp, 0, sizeof(m_tEqp) );
    memset( &m_tCfg, 0, sizeof(m_tCfg) );
    memset( &m_tPrsTimeSpan, 0, sizeof(m_tPrsTimeSpan) );

	m_bIsUseSmoothSend = FALSE;   
}

CBasCfg::~CBasCfg()
{
}

/*=============================================================================
  �� �� ���� FreeStatusDataA
  ��    �ܣ� ���״̬����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CBasCfg::FreeStatusDataA(void)
{
    m_dwMcuNode = INVALID_NODE;
    m_dwMcuIId  = INVALID_INS;
    return;
}

/*=============================================================================
  �� �� ���� FreeStatusDataB
  ��    �ܣ� ���״̬����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CBasCfg::FreeStatusDataB(void)
{
    m_dwMcuNodeB = INVALID_NODE;
    m_dwMcuIIdB  = INVALID_INS;
    return;
}

/*=============================================================================
  �� �� ���� ReadDebugValues
  ��    �ܣ� �������ļ���ȡ��������ֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CBasCfg::ReadDebugValues()
{
    s32 nValue;
    ::GetRegKeyInt( BASCFG_FILE, SECTION_BASDEBUG, "IsEnableRateAutoDec", 1, &nValue );
    m_tDebugVal.SetEnableBitrateCheat( (nValue==0) ? FALSE : TRUE );

    if ( !m_tDebugVal.IsEnableBitrateCheat() )
        return;

    ::GetRegKeyInt( BASCFG_FILE, SECTION_BASDEBUG, "DefaultTargetRatePct", 75, &nValue );
    m_tDebugVal.SetDefaultRate((u16)nValue);

    return;
}


/*====================================================================
	����  : GetDefaultParam
	����  : ����ý������ȡ��Ӧ����Ĳ���
	����  : byEnctype: ý������ [In]
			tEncparam: ������� [Out]
	���  : ��
	����  : ��
	ע    : 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    07/05/17    4.0         ����        ����
====================================================================*/
void CBasCfg::GetDefaultParam(u8 byEnctype, TVideoEncParam& tEncparam)
{
#if 1
    s32 nValue;
	// ���Ƿ��ƽ������, zgc, 2008-03-03
	if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASDEBUG, "IsUseSmoothSend", 1, &nValue))
	{
		m_bIsUseSmoothSend = TRUE;
	}
	else
	{
		m_bIsUseSmoothSend = ( 1 == nValue ) ? TRUE : FALSE;
	}

    switch( byEnctype )
    {
    case MEDIA_TYPE_MP4:
        tEncparam.m_byEncType = (u8)EnVidFmt[0][0];

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[0][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[0][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[0][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[0][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[0][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4BandWidth", 0, &nValue))
        {
//            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
//            tEncparam.m_dwSndNetBand = (u32)nValue;
        }
/*
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4VideoEncLostFrameOnOff", 0, &nValue))
        {
            encparam.m_byAutoDelFrm = 2;
        }
        else
        {
            encparam.m_byAutoDelFrm = (u32)nValue;
        }
*/
    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//        tEncparam.m_byPalNtsc = (u8)EnVidFmt[0][6];
//        tEncparam.m_byCapPort = (u8)EnVidFmt[0][7];
    
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[0][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8)EnVidFmt[0][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[0][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[0][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H261: //261
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: H261, %d\n", MEDIA_TYPE_H261);
        tEncparam.m_byEncType = (u8)EnVidFmt[1][0];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[1][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[1][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[1][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[1][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[1][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261BandWidth", 0, &nValue))
        {
//            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
 //           tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//        tEncparam.m_byPalNtsc = (u8)EnVidFmt[1][6];
//        tEncparam.m_byCapPort = (u8)EnVidFmt[1][7];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[1][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8)EnVidFmt[1][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[1][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[1][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H262:   //262(mepg2)
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: H262, %d\n", MEDIA_TYPE_H262);
        tEncparam.m_byEncType = (u8)EnVidFmt[2][0];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[2][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[2][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[2][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[2][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[2][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "262BandWidth", 0, &nValue))
        {
//            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
 //           tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//        tEncparam.m_byPalNtsc = (u8)EnVidFmt[2][6];
//        tEncparam.m_byCapPort = (u8)EnVidFmt[2][7];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[2][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[2][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "262VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[2][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[2][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H263:  //263
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: H263, %d\n", MEDIA_TYPE_H263);
        tEncparam.m_byEncType = (u8)EnVidFmt[3][0];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[3][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[3][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[3][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[3][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[3][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263BandWidth", 0, &nValue))
        {
//            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
//            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//        tEncparam.m_byPalNtsc = (u8)EnVidFmt[3][6];
 //       tEncparam.m_byCapPort = (u8)EnVidFmt[3][7];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[3][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[3][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[3][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[3][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H263PLUS:  //263+
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: H263+, %d\n", MEDIA_TYPE_H263PLUS);
        tEncparam.m_byEncType = (u8)EnVidFmt[4][0];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSBitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[4][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSVideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[4][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSVideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[4][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSVideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[4][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSEncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[4][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263PLUSBandWidth", 0, &nValue))
        {
 //           tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
//            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
 //       tEncparam.m_byPalNtsc = (u8)EnVidFmt[4][6];
 //       tEncparam.m_byCapPort = (u8)EnVidFmt[4][7];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSFrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[4][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSVideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[4][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263PLUSVideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[4][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSVideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[4][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H264:  //264
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: H264, %d\n", MEDIA_TYPE_H264);
        tEncparam.m_byEncType = (u8)EnVidFmt[5][0];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264SBitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[5][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[5][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[5][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[5][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[5][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "264BandWidth", 0, &nValue))
        {
//            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
//            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//        tEncparam.m_byPalNtsc = (u8)EnVidFmt[5][6];
 //       tEncparam.m_byCapPort = (u8)EnVidFmt[5][7];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[5][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264SVideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = 0; // (u8) EnVidFmt[5][9];  // Ŀǰh264ֻ֧���ٶ�����
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "264VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[5][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[5][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    default:
        break;
    }
#endif
    return;
}

/*====================================================================
	����  : GetDefaultDecParam
	����  : ����ý������ȡ��Ӧ����Ĳ���
	����  : mediatype: ý������, 
			tParam   : �������
	���  : ��
	����  : ��
	ע    : 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	05/04/11	3.6		    liuhuiyun	  ע������
    07/05/17    4.0         ����        ����
====================================================================*/
void CBasCfg::GetDefaultDecParam( u8 byMediatype, TVideoDecParam& tParam )
{
	s32 nValue = 0;
#if 0
	switch( byMediatype )
	{
		case MEDIA_TYPE_MP4:
		    if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "MPEG4StartBuf", 0, &nValue ) )
            {	tParam.dwStartRecvBufs = 1; }
		    else
            {	tParam.dwStartRecvBufs   = (u32)nValue; }
			    
		    if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "MPEG4FastBuf", 0, &nValue ) )
            {	tParam.dwFastRecvBufs = 8; }
		    else
            {	tParam.dwFastRecvBufs   = (u32)nValue; }
			    
		    if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "MPEG4PostPrcLvl", 0, &nValue ) )
            {	tParam.dwPostPrcLvl   = 1; }
		    else
            {	tParam.dwPostPrcLvl   = (u32)nValue;  }
		  break;
          
		case MEDIA_TYPE_H261: //261
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "261StartBuf", 0, &nValue ) )
            {	tParam.dwStartRecvBufs = 1; }
			else
            {	tParam.dwStartRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "261FastBuf", 0, &nValue ) )
            {	tParam.dwFastRecvBufs = 8; }
			else
            {	tParam.dwFastRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "261PostPrcLvl", 0, &nValue ) )
            {	tParam.dwPostPrcLvl   = 1; }
			else
            {	tParam.dwPostPrcLvl   = (u32)nValue; }
			break;

		case MEDIA_TYPE_H262:   //262
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "262StartBuf", 0, &nValue ) )
            {	tParam.dwStartRecvBufs = 1; }
			else
            {	tParam.dwStartRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "262FastBuf", 0, &nValue ) )
            {	tParam.dwFastRecvBufs = 8; }
			else
            {	tParam.dwFastRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "262PostPrcLvl", 0, &nValue ) )
            {	tParam.dwPostPrcLvl   = 1; }
			else
            {	tParam.dwPostPrcLvl   = (u32)nValue; }
			break;

		case MEDIA_TYPE_H263://263
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263StartBuf", 0, &nValue ) )
            {	tParam.dwStartRecvBufs = 1; }
			else
            {	tParam.dwStartRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263FastBuf", 0, &nValue ) )
            {	tParam.dwFastRecvBufs = 8; }
			else
            {	tParam.dwFastRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263PostPrcLvl", 0, &nValue ) )
            {	tParam.dwPostPrcLvl   = 1; }
			else
            {	tParam.dwPostPrcLvl   = (u32)nValue; }
			break;

		case MEDIA_TYPE_H263PLUS:   //263+
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263PLUSStartBuf", 0, &nValue ) )
            {	tParam.dwStartRecvBufs = 1; }
			else
            {	tParam.dwStartRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263PLUSFastBuf", 0, &nValue ) )
            {	tParam.dwFastRecvBufs = 8; }
			else
            {	tParam.dwFastRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263PLUSPostPrcLvl", 0, &nValue ) )
            {	tParam.dwPostPrcLvl   = 1; }
			else
            {	tParam.dwPostPrcLvl   = (u32)nValue; }
			break;
		case MEDIA_TYPE_H264:   //264
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "264StartBuf", 0, &nValue ) )
            {	tParam.dwStartRecvBufs = 1; }
			else
            {	tParam.dwStartRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "264FastBuf", 0, &nValue ) )
            {	tParam.dwFastRecvBufs = 8; }
			else
            {	tParam.dwFastRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "264PostPrcLvl", 0, &nValue ) )
            {	tParam.dwPostPrcLvl   = 1; }
			else
            {	tParam.dwPostPrcLvl   = (u32)nValue; }
			break;

		default:
			break;
	}
#endif
	return;
}


BOOL CHdAdpGroup::Create(u32                  dwMaxRcvFrameSize,  
		 		        u16                   wRtcpBackPort,    //rtcp�ط��˿�
			            u16                   wLocalStartPort,  //Ϊ���ؽ��յ�rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
			            u32                   dwDestIp,         //����Ŀ��Ip��ַ��
	                    u16                   dwDestPort,       //��һ·����Ŀ�Ķ˿ڣ��ڶ�·���Ͷ˿�Ϊ��һ·����Ŀ�Ķ˿� + 10
                        u8                    byFpgaErrTimes,   //fpga�������������޴���
						u32					  dwContext)		//��Ӧ����͸��������ʵ��ָ��

{
	m_dwNetBand        = 8000000;
	m_dwRtcpBackAddr   = htonl(dwDestIp);
	m_wRtcpBackPort    = wRtcpBackPort;
	m_wLocalStartPort  = wLocalStartPort;
	m_dwDestAddr       = htonl(dwDestIp);
	m_wDestPort        = dwDestPort;

    baslog("\n[CHdAdpGroup::Create] m_dwNetBand:%d m_dwRtcpBackAddr:%s, m_wRtcpBackPort:%u m_wLocalStartPort:%u, m_dwDestAddr:%s m_wDestPort:%d\n",
           m_dwNetBand,
           ipStr(m_dwRtcpBackAddr),
           m_wRtcpBackPort,
           m_wLocalStartPort,
           ipStr(m_dwDestAddr),
		   m_wDestPort);

	u16 wRet = 0;
	do{
		wRet = m_cAptGrp.CreateGroup(m_bIsVga);
		if( Codec_Success != wRet )
		{
			OspPrintf(TRUE, FALSE, "CHdAdpGroup::Create-->m_cAptGrp.CreateGroup fail, Error code is: %d \n", wRet);
            printf("CHdAdpGroup::Create-->m_cAptGrp.CreateGroup fail, Error code is: %d \n", wRet);
			break;
		}
        else
        {
            printf("CHdAdpGroup::Create-->m_cAptGrp.CreateGroup succeed!\n");
        }

        m_cAptGrp.SetFpgaErrReboot(byFpgaErrTimes);
        printf("CHdAdpGroup::Create-->m_cAptGrp.SetFpgaErrReboot(%d)!\n", byFpgaErrTimes);

        for (u8 byIdx = 0; byIdx < MAXNUM_RCV; byIdx ++)
        {
            m_pcMediaRcv[byIdx] = new CKdvMediaRcv;

            if (0 == byIdx)
            {
                wRet = m_pcMediaRcv[byIdx]->Create(MAX_VIDEO_FRAME_SIZE,
                    CBRecvFrame0,   
                    dwContext);
                
                if( MEDIANET_NO_ERROR !=  wRet )
                {
                    m_cAptGrp.DestroyGroup();
                    OspPrintf(TRUE, FALSE, "CHdAdpGroup::Create-->m_pcMediaRcv.%d.Create fail, Error code is: %d \n", byIdx, wRet);
                    printf("CHdAdpGroup::Create-->m_pcMediaRcv.%d Create fail, Error code is: %d \n", byIdx, wRet);
                    break;
                }
            }
            else if (1 == byIdx)
            {
                wRet = m_pcMediaRcv[byIdx]->Create(MAX_VIDEO_FRAME_SIZE,
                    CBRecvFrame1,   
                    dwContext);
                
                if( MEDIANET_NO_ERROR !=  wRet )
                {
                    m_cAptGrp.DestroyGroup();
                    OspPrintf(TRUE, FALSE, "CHdAdpGroup::Create-->m_pcMediaRcv.%d.Create fail, Error code is: %d \n", byIdx, wRet);
                    printf("CHdAdpGroup::Create-->m_pcMediaRcv.%d Create fail, Error code is: %d \n", byIdx, wRet);
                    break;
                }
            }
            else if (2 == byIdx)
            {
                wRet = m_pcMediaRcv[byIdx]->Create(MAX_VIDEO_FRAME_SIZE,
                    CBRecvFrame2,   
                    dwContext);
                
                if( MEDIANET_NO_ERROR !=  wRet )
                {
                    m_cAptGrp.DestroyGroup();
                    OspPrintf(TRUE, FALSE, "CHdAdpGroup::Create-->m_pcMediaRcv.%d.Create fail, Error code is: %d \n", byIdx, wRet);
                    printf("CHdAdpGroup::Create-->m_pcMediaRcv.%d Create fail, Error code is: %d \n", byIdx, wRet);
                    break;
                }
            }
            else
            {
                wRet = m_pcMediaRcv[byIdx]->Create(MAX_VIDEO_FRAME_SIZE,
                    CBRecvFrame3,   
                    dwContext);
                
                if( MEDIANET_NO_ERROR !=  wRet )
                {
                    m_cAptGrp.DestroyGroup();
                    OspPrintf(TRUE, FALSE, "CHdAdpGroup::Create-->m_pcMediaRcv.%d.Create fail, Error code is: %d \n", byIdx, wRet);
                    printf("CHdAdpGroup::Create-->m_pcMediaRcv.%d Create fail, Error code is: %d \n", byIdx, wRet);
                    break;
                }
            }

            TLocalNetParam tlocalNetParm;
            memset(&tlocalNetParm, 0, sizeof(TLocalNetParam));
            tlocalNetParm.m_tLocalNet.m_wRTPPort  = m_wLocalStartPort + PORTSPAN * byIdx;
            tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_wLocalStartPort + PORTSPAN * byIdx + 1;
            tlocalNetParm.m_dwRtcpBackAddr        = m_dwDestAddr;
            tlocalNetParm.m_wRtcpBackPort         = wRtcpBackPort + PORTSPAN * byIdx;

            wRet = m_pcMediaRcv[byIdx]->SetNetRcvLocalParam(tlocalNetParm);
            if( MEDIANET_NO_ERROR !=  wRet )
            {
                m_cAptGrp.DestroyGroup();
                OspPrintf(TRUE, FALSE, "CHdAdpGroup::Create-->m_pcMediaRcv.%d.SetNetRcvLocalParam fail, Error code is: %d \n", byIdx, wRet);
                printf("CHdAdpGroup::Create-->m_pcMediaRcv.%d.SetNetRcvLocalParam fail, Error code is: %d \n", byIdx, wRet);
                break;
		    }

            // ����HDFlag Ϊ TRUE
            m_pcMediaRcv[byIdx]->SetHDFlag(TRUE);
        }


		return TRUE;
	}
	while ( 0 );


	return FALSE;
}

void CHdAdpGroup::UpdateNetParam(u16 wRtcpBackPort,
                                 u16 wLocalStartPort,
                                 u32 dwDestIp,
                                 u16 dwDestPort)
{
    m_dwNetBand        = 8000000;
    m_dwRtcpBackAddr   = htonl(dwDestIp);
    m_wRtcpBackPort    = wRtcpBackPort;
    m_wLocalStartPort  = wLocalStartPort;
    m_dwDestAddr       = htonl(dwDestIp);
	m_wDestPort        = dwDestPort;

    return;
}

//���õ�һ·���������ķ��Ͷ���
BOOL CHdAdpGroup::SetVideoEncSend(u32 dwNetBand,  u16 wLoacalStartPort,  u32 dwDestIp,  u16 dwDestPort)
{
	baslog("[SetVideoEncSend] wLoacalStartPort:%u, dest:%s:%u\n",wLoacalStartPort,ipStr(dwDestIp),dwDestPort);
	u16 wRet = 0;

	do
	{

        u8 byFrameRate = m_atVidEncParam[0].m_byFrameRate;
    	u8 byMediaType = m_atVidEncParam[0].m_byEncType;

        //zbq[10/10/2008] ��ӦMCU��Ⱥ���Ż�����������������
        if (byFrameRate > 60)
        {
            byFrameRate = 30;
        }
        if (dwNetBand > 8000000)
        {
            if (m_atVidEncParam[1].m_wBitRate <= 8128)
            {
                dwNetBand = m_atVidEncParam[1].m_wBitRate * 1024;
            }
            else
            {
                dwNetBand = 0;
            }
        }

    	wRet = (m_cMediaSnd[0]).Create(MAX_VIDEO_FRAME_SIZE,
		    	                       dwNetBand,
			                           byFrameRate,
			    				       byMediaType);

        if(MEDIANET_NO_ERROR != wRet)
        {
            m_cAptGrp.DestroyGroup();
            OspPrintf(TRUE, FALSE, "CHdAdpGroup::SetVideoEncSend-->m_cMediaSnd[0].Create fail, Error code is:%d, NetBand.%d, FrmRate.%d, MediaType.%d\n", wRet, dwNetBand, byFrameRate, byMediaType);
            printf("CHdAdpGroup::SetVideoEncSend-->m_cMediaSnd[0].Create fail, Error code is: %d \n", wRet);
    	   	break;
        }
		
		//�ж��Ƿ�Ҫ���ø����־
		u32 dwType = 0;
		wRet = m_cAptGrp.GetVideoCodecType(0, 0, dwType);

    	if(Codec_Success != wRet)
		{
    		OspPrintf(TRUE, FALSE, "CHdAdpGroup::SetVideoEncSend-->m_cAptGrp.GetVideoCodecType fail,Error code is: %d \n", wRet);
    		break;
		}

		if(VID_CODECTYPE_FPGA == dwType)
		{
			m_cMediaSnd[0].SetHDFlag(TRUE);
		}

    	//���÷��Ͳ���
    	TNetSndParam tNetSndPar;
    	memset(&tNetSndPar, 0x00, sizeof(tNetSndPar));

    	tNetSndPar.m_byNum  = 1;
    	tNetSndPar.m_tLocalNet.m_wRTPPort       = wLoacalStartPort;
     	tNetSndPar.m_tLocalNet.m_wRTCPPort      = wLoacalStartPort + 1;
    	tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = dwDestIp;
    	tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = dwDestPort;
    	tNetSndPar.m_tRemoteNet[0].m_dwRTCPAddr = dwDestIp;
    	tNetSndPar.m_tRemoteNet[0].m_wRTCPPort  = dwDestPort + 1;

    	wRet = m_cMediaSnd[0].SetNetSndParam(tNetSndPar);

    	if ( MEDIANET_NO_ERROR != wRet )
		{
    		m_cAptGrp.DestroyGroup();
    		OspPrintf(TRUE, FALSE, "CHdAdpGroup::Great-->m_cMediaSnd[0].SetNetSndParam fail, Error code is: %d \n", wRet);
            printf("CHdAdpGroup::Great-->m_cMediaSnd[0].SetNetSndParam fail, Error code is: %d \n", wRet);
    		break;
		}
	
    	//���ñ���ص�
    	wRet = m_cAptGrp.SetVidDataCallback(0, 0, CBSendFrame, &(m_cMediaSnd[0]));

    	if( Codec_Success != wRet )
		{
    		OspPrintf(TRUE, FALSE, "CHdAdpGroup::Great-->m_cAptGrp.SetVidDataCallback(0 ,0 ) fail, Error code is: %d \n", wRet);
    		break;
		}

		return TRUE;

	} while ( 0 );


	return FALSE;
}

//���õڶ�·���������ķ��Ͷ���
BOOL CHdAdpGroup::SetSecVideoEncSend(u32 dwNetBand,u16 wLoacalStartPort,  u32 dwDestIp,  u16 dwDestPort)
{
	baslog("[SetSecVideoEncSend] wLoacalStartPort:%u, dest:%s:%u\n",wLoacalStartPort, ipStr(dwDestIp), dwDestPort);
	if(TRUE == m_bIsVga)
	{
		OspPrintf(TRUE, FALSE, "Vga group do not has second video enc!\n");
		return FALSE;
	}

	u16 wRet = 0;

	do
	{        
        u8 byFrameRate = m_atVidEncParam[1].m_byFrameRate;
    	u8 byMediaType = m_atVidEncParam[1].m_byEncType;

        //zbq[10/10/2008] ��ӦMCU��Ⱥ���Ż�����������������
        if (byFrameRate > 60)
        {
            byFrameRate = 5;
        }
        if (dwNetBand > 8000000)
        {
            if (m_atVidEncParam[0].m_wBitRate <= 8128)
            {
                dwNetBand = m_atVidEncParam[0].m_wBitRate * 1024;
            }
            else
            {
                dwNetBand = 0;
            }
        }

    	wRet = (m_cMediaSnd[1]).Create(MAX_VIDEO_FRAME_SIZE,
		    	                       dwNetBand,
			                           byFrameRate,
			    				       byMediaType);

        if( MEDIANET_NO_ERROR != wRet )
		{
         	m_cAptGrp.DestroyGroup();
    		OspPrintf(TRUE, FALSE, "CHdAdpGroup::Great-->m_cMediaSnd[1].Create fail, Error is: %d \n", wRet);
            printf("CHdAdpGroup::Great-->m_cMediaSnd[1].Create fail, Error is: %d \n", wRet);
    	   	break;
		}

		//�ж��Ƿ�Ҫ���ø����־
		u32 dwType = 0;
		wRet = m_cAptGrp.GetVideoCodecType(0, 1, dwType);

    	if(Codec_Success != wRet)
		{
    		OspPrintf(TRUE, FALSE, "CHdAdpGroup::SetVideoEncSend-->m_cAptGrp.GetVideoCodecType fail,Error is: %d \n", wRet);
    		break;
		}

		if(VID_CODECTYPE_FPGA == dwType)
		{
			m_cMediaSnd[1].SetHDFlag(TRUE);
		}

    	//���÷��Ͳ���
    	TNetSndParam tNetSndPar;
    	memset(&tNetSndPar, 0x00, sizeof(tNetSndPar));

    	tNetSndPar.m_byNum  = 1;
    	tNetSndPar.m_tLocalNet.m_wRTPPort       = wLoacalStartPort;
     	tNetSndPar.m_tLocalNet.m_wRTCPPort      = wLoacalStartPort + 1;
    	tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = dwDestIp;
    	tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = dwDestPort;
    	tNetSndPar.m_tRemoteNet[0].m_dwRTCPAddr = dwDestIp;
    	tNetSndPar.m_tRemoteNet[0].m_wRTCPPort  = dwDestPort + 1;

    	wRet = m_cMediaSnd[1].SetNetSndParam(tNetSndPar);

    	if ( MEDIANET_NO_ERROR != wRet )
		{
    		m_cAptGrp.DestroyGroup();
    		OspPrintf(TRUE, FALSE, "CHdAdpGroup::Great-->m_cMediaSnd[0].SetNetSndParam fail, Error is: %d \n", wRet);
            printf("CHdAdpGroup::Great-->m_cMediaSnd[0].SetNetSndParam fail, Error is: %d \n", wRet);
    		break;
		}
	
    	//���ñ���ص�
    	wRet = m_cAptGrp.SetVidDataCallback(0, 1, CBSendFrame, &(m_cMediaSnd[1]));

    	if( Codec_Success != wRet )
		{
    		OspPrintf(TRUE, FALSE, "CHdAdpGroup::Great-->m_cAptGrp.SetVidDataCallback(0 ,1 ) fail, Error is: %d \n", wRet);
    		break;
		}

		return TRUE;

	} while ( 0 );

	return FALSE;
}

BOOL CHdAdpGroup::SetVideoEncParam( TVideoEncParam * ptVidEncParam,
									BOOL32			 bSetNetParam )
{
	baslog("CHdAdpGroup::SetVideoEncParam\n");
	if(NULL == ptVidEncParam)
	{
		OspPrintf(TRUE, FALSE, "CHdAdpGroup::ChangeVideoEncParam-->bad paramter!\n");
		return FALSE;
	}

    u8 byChnNo = 0; //fixed

	u16 wRet = 0;
	do
	{
        u8 byChnlNum = IsVga() ? 1 : 2;
        memcpy( m_atVidEncParam, ptVidEncParam, sizeof(TVideoEncParam) * byChnlNum );
    	wRet = m_cAptGrp.ChangeVideoEncParam(byChnNo, m_atVidEncParam, byChnlNum);

    	if(Codec_Success != wRet)
		{
            OspPrintf(TRUE, FALSE, "CHdAdpGroup::SetVideoEncParam-->m_cAptGrp.ChangeVideoEncParam fail, Error code is: %d \n", wRet);
            break;
		}

		//��������ڿ�����ʱ�����������þͿ�����
		if (bSetNetParam)
		{
			if(!SetVideoEncSend(m_dwNetBand, PORT_SNDBIND + (byChnlNum - 1) * PORTSPAN, m_dwDestAddr, m_wDestPort))
			{
				OspPrintf(TRUE, FALSE, "CHdAdpGroup::ChangeVideoEncParam-->SetVideoEncSend fail\n");
				break;
			}
			
			if (!IsVga())
			{
				if(!SetSecVideoEncSend(m_dwNetBand, PORT_SNDBIND + (byChnlNum - 1) * PORTSPAN + 2, m_dwDestAddr, m_wDestPort + PORTSPAN))
				{
					OspPrintf(TRUE, FALSE, "CHdAdpGroup::ChangeVideoEncParam-->SetVideoEncSend fail\n");
					break;
				}
			}			
		}

		return TRUE;

	} while ( 0 );
	
	return FALSE;
}


//�ı���Ƶ����������
BOOL CHdAdpGroup::ChangeVideoDecParam( TVideoDecParam * ptVidDecParam )
{
	if(NULL == ptVidDecParam)
	{
		OspPrintf(TRUE, FALSE, "CHdAdpGroup::ChangeVideoDecParam-->bad paramter!\n");
	}
    u8 byChnNo = 0;
	u16 wRet = 0;
	wRet = m_cAptGrp.ChangeVideoDecParam(byChnNo, ptVidDecParam);

	if(Codec_Success != wRet)
	{
		OspPrintf(TRUE, FALSE,"CHdAdpGroup::ChangeVideoDecParam-->m_cAptGrp.ChangeVideoDecParam fail! Error code is:%d\n", wRet);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

//��ʼ����
BOOL CHdAdpGroup::StartAdapter(BOOL32 bStartMediaRcv)
{
	baslog("CHdAdpGroup::StartAdapter\n");
	u16 wRet = 0;

	if(m_bIsStart)
	{
    	baslog("CHdAdpGroup::StartAdapter success\n");
		return TRUE;
	}
	wRet = m_cAptGrp.StartGroup();

	if (bStartMediaRcv)
	{
        for (u8 byIdx = 0; byIdx < MAXNUM_RCV; byIdx ++)
        {
            m_pcMediaRcv[byIdx]->StartRcv();
        }
	}

	if(Codec_Success != wRet)
	{
        for (u8 byIdx = 0; byIdx < MAXNUM_RCV; byIdx ++)
        {
		    m_pcMediaRcv[byIdx]->StopRcv();
        }
        OspPrintf(TRUE, FALSE, "CHdAdpGroup::StartAdapter-->m_cAptGrp.StartGroup fail, Error code is: %d \n", wRet);
		return FALSE;
	}

	m_bIsStart = TRUE;

	return TRUE;
}

//ֹͣ����
BOOL CHdAdpGroup::StopAdapter(BOOL32 bStopMediaRcv)
{
	u16 wRet = 0;
	baslog("CHdAdpGroup::StopAdapter\n");
	if(!m_bIsStart)
	{
    	baslog("CHdAdpGroup::StopAdapter success\n");
		return TRUE;
	}
	if (bStopMediaRcv)
	{
        for (u8 byIdx = 0; byIdx < MAXNUM_RCV; byIdx ++)
        {
		    m_pcMediaRcv[byIdx]->StopRcv();
        }
	}

    // fxh[2009/02/10],ֹͣ����ϳ�ʱ���Ƴ����е�ͨ����������һ֡��ͼ��
    for (u8 byIdx = 0; byIdx < MAXNUM_RCV; byIdx++)
    {
        m_cAptGrp.RemoveVideoChannel(byIdx);
    }
	wRet = m_cAptGrp.StopGroup();

	if(Codec_Success != wRet)
	{
        OspPrintf(TRUE, FALSE, "CHdAdpGroup::StopAdapter-->m_cAptGrp.StopGroup fail, Error code is: %d \n", wRet);
		return FALSE;
	}

	m_bIsStart = FALSE;
   	baslog("CHdAdpGroup::StopAdapter success\n");	
	return TRUE;
}

//��ȡ��Ƶ�������
BOOL CHdAdpGroup::GetVideoEncParam( TVideoEncParam& tVidEncParam )
{
//	return wRet = m_cAptGrp.GetVideoEncParam(byChnNo, ptVidEncParam);
	tVidEncParam = m_atVidEncParam[0];
	return TRUE;
}


//�ı���Ƶ����������
BOOL CHdAdpGroup::GetSecVideoEncParam(TVideoEncParam& tVidEncParam)
{
//	return wRet = m_cAptGrp.GetSecVideoEncParam(byChnNo, ptVidEncParam);
	tVidEncParam = m_atVidEncParam[1];
	return TRUE;
}

//������������ش�����
BOOL  CHdAdpGroup::SetNetRecvFeedbackVideoParam(TRSParam tNetRSParam, BOOL32 bRepeatSnd/* = FALSE */)
{
	u16 wRet = 0;

    for (u8 byIdx = 0; byIdx < MAXNUM_RCV; byIdx ++)
    {
	    //�������
	    memcpy(&m_tRsParam[byIdx], &tNetRSParam, sizeof(m_tRsParam[0]));
	    m_bNeedRS[byIdx] = bRepeatSnd;
	    wRet = m_pcMediaRcv[byIdx]->ResetRSFlag(tNetRSParam, bRepeatSnd);

	    if(MEDIANET_NO_ERROR != wRet)
	    {
		    OspPrintf(TRUE, FALSE, "CHdAdpGroup::SetNetRecvFeedbackVideoParam-->m_cMediaRcv.%d.ResetRSFlag fail,Error code is:%d\n", byIdx, wRet);
		    return FALSE;
	    }
    }
    return TRUE;
}

/*����ͼ������緢���ش�����*/
BOOL  CHdAdpGroup::SetNetSendFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd /*= FALSE*/)
{
	u16 wRet = 0;

	//�������
	m_awBufTime[0] = wBufTimeSpan;
	wRet = m_cMediaSnd[0].ResetRSFlag(wBufTimeSpan, bRepeatSnd);

	if(MEDIANET_NO_ERROR != wRet)
	{
		OspPrintf(TRUE, FALSE, "CHdAdpGroup::SetNetSendFeedbackVideoParam-->m_cMediaSnd[0].ResetRSFlag fail,Error code is:%d\n",wRet);
		return FALSE;
	}

	return TRUE;
}

/*����ͼ������緢���ش�����*/
BOOL  CHdAdpGroup::SetNetSecSendFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd /*= FALSE*/)
{
	u16 wRet = 0;
	
	//�������
   	m_awBufTime[1] = wBufTimeSpan;
	if(FALSE == m_bIsVga)
	{
       	wRet = m_cMediaSnd[1].ResetRSFlag(wBufTimeSpan, bRepeatSnd);
     	if(MEDIANET_NO_ERROR != wRet)
		{
     		OspPrintf(TRUE, FALSE, "CHdAdpGroup::SetNetSendFeedbackVideoParam-->m_cMediaSnd[1].ResetRSFlag fail,Error code is:%d\n",wRet);
    		return FALSE;
		}
		return TRUE;
	}
	OspPrintf(TRUE, FALSE, "CHdAdpGroup::SetNetSecSendFeedbackVideoParam-->is not vga!\n");
	return FALSE;
}

BOOL CHdAdpGroup::GetGroupStatus(TAdapterGroupStatus &tAdapterGroupStatus)
{
	u16 wRet = 0;
	wRet = m_cAptGrp.GetGroupStatus(tAdapterGroupStatus);

	if(Codec_Success != wRet)
	{
		OspPrintf(TRUE, FALSE,"CHdAdpGroup::GetGroupStatus-->m_cAptGrp.GetGroupStatus fail! Error code is:%d\n", wRet);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL CHdAdpGroup::SetFastUpdata(void)
{
	u32 dwType = 0;
	baslog("CHdAdpGroup::SetFastUpdata\n");

    m_cAptGrp.SetFastUpdata(ADAPTER_CODECID_ENCHD);
    m_cAptGrp.SetFastUpdata(ADAPTER_CODECID_ENCSD);
    /*
	m_cAptGrp.GetVideoCodecType(0, 0, dwType);
	m_cAptGrp.SetFastUpdata(dwType);

	if(FALSE == IsVga())
	{
    	m_cAptGrp.GetVideoCodecType(0, 1, dwType);
    	m_cAptGrp.SetFastUpdata(dwType);
	}
    */
	return TRUE;
}

BOOL CHdAdpGroup::GetVideoChannelStatis(u8 byChnNo, TAdapterChannelStatis &tAdapterChannelStatis)
{
	u16 wRet = 0;
	wRet = m_cAptGrp.GetVideoChannelStatis(byChnNo,tAdapterChannelStatis);

	if(Codec_Success != wRet)
	{
		OspPrintf(TRUE, FALSE,"CHdAdpGroup::GetVideoChannelStatis-->m_cAptGrp.GetVideoChannelStatis fail! Error code is:%d\n", wRet);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CHdAdpGroup::SetSmoothSendRule(void)
{
    u16 wBitrate = m_atVidEncParam[0].m_wBitRate;
    u32 dwPeak = wBitrate + wBitrate / 5;   // add 20% 
    baslog( "[SetSmoothSendRule] Setting ssrule for 0x%x:%u, rate/peak=%u/%u KByte.\n",
        m_dwDestAddr, m_wDestPort, wBitrate, dwPeak );

#ifdef _LINUX_
    s32 nRet = SetSSRule(m_dwDestAddr, m_wDestPort, wBitrate, dwPeak, 2);   // default 2 second
    if ( 0 == nRet )
    {
        baslog( "[SetSmoothSendRule] Set rule failed. SSErrno=%d\n", SSErrno );
    }

	if(TRUE == m_bIsVga)
	{
        wBitrate = m_atVidEncParam[1].m_wBitRate;
        dwPeak = wBitrate + wBitrate / 5;   // add 20% 
        baslog( "[SetSmoothSendRule] Setting ssrule for 0x%x:%d, rate/peak=%u/%u KByte.\n",
                m_dwDestAddr, m_wDestPort + PORTSPAN, wBitrate, dwPeak );

        s32 nRet = SetSSRule(m_dwDestAddr, m_wDestPort + PORTSPAN, wBitrate, dwPeak, 2);   // default 2 second
        if ( 0 == nRet )
		{
            baslog( "[SetSmoothSendRule] Set rule failed. SSErrno=%d\n", SSErrno );
		}
	}
#endif
}

void CHdAdpGroup::ClearSmoothSendRule(void)
{
    baslog( "[ClearSmoothSendRule] Unsetting ssrule for 0x%x:%u.\n",
        m_dwDestAddr, m_wDestPort );

#ifdef _LINUX_
    s32 nRet = UnsetSSRule(m_dwDestAddr, m_wDestPort);
    if ( 0 == nRet )
    {
        baslog( "[ClearSmoothSendRule] Unset rule failed. SSErrno=%d\n", SSErrno );
    }
	if(TRUE == m_bIsVga)
	{
        s32 nRet = UnsetSSRule(m_dwDestAddr, m_wDestPort + PORTSPAN);
        if ( 0 == nRet )
		{
            baslog( "[ClearSmoothSendRule] Unset rule failed. SSErrno=%d\n", SSErrno );
		}
	}
#endif
}

void CHdAdpGroup::StopSmoothSend()
{
    baslog( "[StopSmoothSend] Stopping SmoothSend\n" );

#ifdef _LINUX_
    s32 nRet = CloseSmoothSending();
    if ( 0 == nRet )
    {
        baslog( "[ClearSmoothSendRule] CloseSmoothSending failed. SSErrno=%d\n", SSErrno );
    }
#endif 	
}

//��ʾͨ����Ϣ,����·���붼��ӡ����
void  CHdAdpGroup::ShowChnInfo(void)
{
	OspPrintf(TRUE, FALSE, "\n----------------Adapter Group Info-------------------------\n");

	//������Ϣ
	OspPrintf(TRUE, FALSE, "\n*************Basic info*************\n");
	OspPrintf(TRUE, FALSE, "\t Is Vga:%d\n",   m_bIsVga);
	OspPrintf(TRUE, FALSE, "\t Is Start:%d\n", m_bIsStart);	
    if ( !m_bIsStart) 
    {
        return;
    }

    OspPrintf(TRUE, FALSE, "\t Net Band:%u\n", m_dwNetBand);

	//�������
	OspPrintf(TRUE, FALSE, "\n*************DEC Parmater***********\n");
	u8 abyDecKey[64];
	s32 iLen = 0;
	m_atMediaDec[0].GetEncryptKey(abyDecKey, &iLen);
	abyDecKey[iLen] = 0;

	OspPrintf(TRUE, FALSE, "\t DEC key:%s\n", abyDecKey);
	OspPrintf(TRUE, FALSE, "\t DEC key len:%d\n", iLen);
	OspPrintf(TRUE, FALSE, "\t DEC mode:%u\n", m_atMediaDec[0].GetEncryptMode());
	OspPrintf(TRUE, FALSE, "\t Active payload:%u\n", m_atDbPayload[0].GetActivePayload());
	OspPrintf(TRUE, FALSE, "\t Real payload:%u\n", m_atDbPayload[0].GetRealPayLoad());

	//�������
	OspPrintf(TRUE, FALSE, "\n*************ENC Parmater***********\n");

	OspPrintf(TRUE, FALSE, "\nThe first:\n");
	OspPrintf(TRUE, FALSE, "\t VideoWidth:%u\n", m_atVidEncParam[0].m_wVideoWidth);
	OspPrintf(TRUE, FALSE, "\t VideoHeight:%u\n", m_atVidEncParam[0].m_wVideoHeight);
	OspPrintf(TRUE, FALSE, "\t MaxKeyFrameInterval:%u\n", m_atVidEncParam[0].m_byMaxKeyFrameInterval);
	OspPrintf(TRUE, FALSE, "\t BitRate:%u\n",m_atVidEncParam[0].m_wBitRate);
	OspPrintf(TRUE, FALSE, "\t EncType:%u\n", m_atVidEncParam[0].m_byEncType);
	OspPrintf(TRUE, FALSE, "\t RcMode:%u\n", m_atVidEncParam[0].m_byRcMode);
	OspPrintf(TRUE, FALSE, "\t MaxQuant:%u\n", m_atVidEncParam[0].m_byMaxQuant);
	OspPrintf(TRUE, FALSE, "\t MinQuant:%u\n", m_atVidEncParam[0].m_byMinQuant);
	OspPrintf(TRUE, FALSE, "\t FrameRate:%u\n", m_atVidEncParam[0].m_byFrameRate);
	OspPrintf(TRUE, FALSE, "\t FrameFmt:%u\n", m_atVidEncParam[0].m_byFrameFmt);

	m_atMediaEnc[0].GetEncryptKey(abyDecKey, &iLen);
	abyDecKey[iLen] = 0;
	OspPrintf(TRUE, FALSE, "\n");
	OspPrintf(TRUE, FALSE, "\t ENC key:%s\n", abyDecKey);
	OspPrintf(TRUE, FALSE, "\t ENC key len:%d\n", iLen);
	OspPrintf(TRUE, FALSE, "\t ENC moe:%u\n", m_atMediaEnc[0].GetEncryptMode());

	if(FALSE == IsVga())
	{
    	OspPrintf(TRUE, FALSE, "\nThe second:\n");
    	OspPrintf(TRUE, FALSE, "\t VideoWidth:%u\n", m_atVidEncParam[1].m_wVideoWidth);
    	OspPrintf(TRUE, FALSE, "\t VideoHeight:%u\n", m_atVidEncParam[1].m_wVideoHeight);
    	OspPrintf(TRUE, FALSE, "\t MaxKeyFrameInterval:%u\n", m_atVidEncParam[1].m_byMaxKeyFrameInterval);
    	OspPrintf(TRUE, FALSE, "\t BitRate:%u\n",m_atVidEncParam[1].m_wBitRate);
    	OspPrintf(TRUE, FALSE, "\t EncType:%u\n", m_atVidEncParam[1].m_byEncType);
     	OspPrintf(TRUE, FALSE, "\t RcMode:%u\n", m_atVidEncParam[1].m_byRcMode);
    	OspPrintf(TRUE, FALSE, "\t MaxQuant:%u\n", m_atVidEncParam[1].m_byMaxQuant);
    	OspPrintf(TRUE, FALSE, "\t MinQuant:%u\n", m_atVidEncParam[1].m_byMinQuant);
    	OspPrintf(TRUE, FALSE, "\t FrameRate:%u\n", m_atVidEncParam[1].m_byFrameRate);
    	OspPrintf(TRUE, FALSE, "\t FrameFmt:%u\n", m_atVidEncParam[1].m_byFrameFmt);

    	m_atMediaEnc[1].GetEncryptKey(abyDecKey, &iLen);
    	abyDecKey[iLen] = 0;
    	OspPrintf(TRUE, FALSE, "\n");
    	OspPrintf(TRUE, FALSE, "\t ENC key:%s\n", abyDecKey);
    	OspPrintf(TRUE, FALSE, "\t ENC key len:%d\n", iLen);
    	OspPrintf(TRUE, FALSE, "\t ENC moe:%u\n", m_atMediaEnc[1].GetEncryptMode());
	}

	m_cAptGrp.ShowChnInfo(0, TRUE);
}

void baslog( s8* pszFmt, ...)
{
    s8 achPrintBuf[512];
    s32  nBufLen = 0;
    va_list argptr;
    if( g_nbaslog >= 1 )
    {		  
        nBufLen = sprintf( achPrintBuf, "[Bas]: " );         
        va_start( argptr, pszFmt );
        vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
        va_end(argptr); 
        OspPrintf( TRUE, FALSE, achPrintBuf ); 
    }
    return;
}

void baslogall( s8* pszFmt, ...)
{
    s8 achPrintBuf[512];
    s32  nBufLen = 0;
    va_list argptr;
    if( g_nbaslog >= 2 )
    {		  
        nBufLen = sprintf( achPrintBuf, "[BasDBG]: " );         
        va_start( argptr, pszFmt );
        vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
        va_end(argptr); 
        OspPrintf( TRUE, FALSE, achPrintBuf ); 
    }
    return;
}



// END OF FILE
