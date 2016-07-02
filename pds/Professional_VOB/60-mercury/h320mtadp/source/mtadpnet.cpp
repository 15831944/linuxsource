/*****************************************************************************
ģ����      : h320�ն�����ģ��
�ļ���      : MtAdpNet.cpp
����ļ�    : MtAdpNet.h
�ļ�ʵ�ֹ���: �����շ�����ģ��
����        : ������
�汾        : V3.6  Copyright(C) 1997-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2005/06/07  3.6         ������      ����
******************************************************************************/
#ifdef WIN32
#define TYPE_SCODE
#endif

#include "mtadpnet.h"
#include "mtadpinst.h"
#include "h261addrtpheader.h"
#include "h263decrtpreform.h"

/*=============================================================================
  �� �� ���� ProcVidNetRcv
  ��    �ܣ� ���������Ƶ���ݻص�
  �㷨ʵ�֣� �����յ�����Ƶ����ͨ��Э��ջ�ӿڷ��͵��ն�
  ȫ�ֱ����� 
  ��    ���� PFEFRAMEHDR pFrmHdr
             u32 dwContext
  �� �� ֵ�� void 
=============================================================================*/
void ProcVidNetRcv(PFRAMEHDR pFrmHdr, u32 dwContext)
{
    CMtAdpInst *pcIns = (CMtAdpInst *)dwContext;
    if(NULL != pcIns)
    {       
        u16 wRet = pcIns->m_pcH320Stack->SendData(pFrmHdr->m_pData, 
                                                  pFrmHdr->m_dwDataSize, 
                                                  dtype_video);
        if(KDVH320_OK != wRet)
        {
            pcIns->MtAdpLog(LOG_WARNING, "[ProcVidNetRcv] send video data to mt failed. err no :%d\n", wRet);
        }
    }        
  
    return;
}

/*=============================================================================
  �� �� ���� ProcAudNetRcv
  ��    �ܣ� ���������Ƶ���ݻص�
  �㷨ʵ�֣� �����յ�����Ƶ����ͨ��Э��ջ�ӿڷ��͵��ն�
  ȫ�ֱ����� 
  ��    ���� PFRAMEHDR pFrmHdr
             u32 dwContext
  �� �� ֵ�� void 
=============================================================================*/
void ProcAudNetRcv(PFRAMEHDR pFrmHdr, u32 dwContext)
{
    CMtAdpInst *pcIns = (CMtAdpInst *)dwContext;
    if(NULL != pcIns)
    {       
        u16 wRet = pcIns->m_pcH320Stack->SendData(pFrmHdr->m_pData, 
                                                  pFrmHdr->m_dwDataSize, 
                                                  dtype_audio);
        if(KDVH320_OK != wRet)
        {
            pcIns->MtAdpLog(LOG_WARNING, "[ProcAudNetRcv] send audio data to mt failed. err no :%d\n", wRet);
        }
    }    
    
    return;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMtAdpNet::CMtAdpNet(CMtAdpInst * const pcIns)
{
    m_pcMtAdpIns = pcIns;

    m_pcAudNetRcv = NULL;
    m_pcVidNetRcv = NULL;
    m_pcAudNetSnd = NULL;
    m_pcVidNetSnd = NULL;
   
    m_byAudSndMediaType = MEDIA_TYPE_NULL;
    m_byVidSndMediaType = MEDIA_TYPE_NULL;

    m_byRtpPackBuf = new u8[BUFLEN_RTPPACK];

#ifdef WIN32
    KdvSocketStartup();
#endif

    OspSemBCreate(&m_semAudSnd);
    OspSemBCreate(&m_semVidSnd);   
}

CMtAdpNet::~CMtAdpNet()
{
    SAFE_DELETE(m_pcAudNetRcv);
    SAFE_DELETE(m_pcVidNetRcv);
    SAFE_DELETE(m_pcAudNetSnd);
    SAFE_DELETE(m_pcVidNetSnd);

    SAFE_DELETE(m_byRtpPackBuf);
 
#ifdef WIN32
    KdvSocketCleanup();        
#endif

    OspSemDelete(m_semAudSnd);
    OspSemDelete(m_semVidSnd);
}

/*=============================================================================
  �� �� ���� VidNetRcvInit
  ��    �ܣ� ��Ƶ���ն����ʼ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TLocalNetParam &tNetParam
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::VidNetRcvInit()
{    
    if(NULL == m_pcMtAdpIns)
    {
        return FALSE;
    }

    TChannel *ptChan = &m_pcMtAdpIns->m_atChannel[VID_RCV_INDEX];
    if(CHAN_CONNECTED != ptChan->m_byState)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetRcvInit] video rcv chan state is invalid\n");
        return FALSE;
    }

    u16 wRet = 0;

    // xsl [10/26/2006] ����һ�δ���
    if (NULL == m_pcVidNetRcv)
    {
        m_pcVidNetRcv = new CKdvMediaRcv;
        if(NULL == m_pcVidNetRcv)
        {
            MtAdpLog(LOG_CRITICAL, "[VidNetRcvInit] mem allocation failed!\n");
            return FALSE;
        }
        
        wRet = m_pcVidNetRcv->Create(DEF_VID_FRAMESIZE, ProcVidNetRcv, (u32)m_pcMtAdpIns);
        if(MEDIANET_NO_ERROR != wRet)
        {
            MtAdpLog(LOG_CRITICAL, "[VidNetRcvInit] create failed. err no :%u\n", wRet);
            SAFE_DELETE(m_pcVidNetRcv);
            return FALSE;
        }
    }    

    TLocalNetParam tNetParam;
    memset(&tNetParam, 0, sizeof(tNetParam));
    tNetParam.m_tLocalNet.m_dwRTPAddr = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetNetSeqIpAddr();
    tNetParam.m_tLocalNet.m_wRTPPort = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetPort();
    tNetParam.m_tLocalNet.m_dwRTCPAddr = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetNetSeqIpAddr();
    tNetParam.m_tLocalNet.m_wRTCPPort = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetPort();
    tNetParam.m_dwRtcpBackAddr = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetNetSeqIpAddr();
    tNetParam.m_wRtcpBackPort = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetPort();
    wRet = m_pcVidNetRcv->SetNetRcvLocalParam(tNetParam);
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetRcvInit] SetNetRcvLocalParam failed. err no :%u\n", wRet);
        VidNetRcvQuit();
        return FALSE;
    }

    wRet = m_pcVidNetRcv->StartRcv();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetRcvInit] StartRcv failed. err no :%u\n", wRet);
        VidNetRcvQuit();
        return FALSE;
    }   

    return TRUE;
}

/*=============================================================================
  �� �� ���� VidNetRcvQuit
  ��    �ܣ� ��Ƶ���ն����˳�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::VidNetRcvQuit()
{
    if(NULL == m_pcVidNetRcv)
    {
        return FALSE;
    }
    
    u16 wRet = 0;
    wRet = m_pcVidNetRcv->StopRcv();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetRcvQuit] StopRcv failed. err no :%u\n", wRet);
    }

    wRet = m_pcVidNetRcv->RemoveNetRcvLocalParam();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetRcvQuit] RemoveNetRcvLocalParam failed. err no :%u\n", wRet);
    }

    return TRUE;
}

/*=============================================================================
  �� �� ���� AudNetRcvInit
  ��    �ܣ� ��Ƶ���ն����ʼ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TLocalNetParam &tNetParam
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::AudNetRcvInit()
{
    if(NULL == m_pcMtAdpIns)
    {
        return FALSE;
    }

    TChannel *ptChan = &m_pcMtAdpIns->m_atChannel[AUD_RCV_INDEX];
    if(CHAN_CONNECTED != ptChan->m_byState)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetRcvInit] audio rcv chan state is invalid\n");
        return FALSE;
    }

    u16 wRet = 0;

    // xsl [10/26/2006] ����һ�δ�������
    if (NULL == m_pcAudNetRcv)
    {
        m_pcAudNetRcv = new CKdvMediaRcv;
        if(NULL == m_pcAudNetRcv)
        {
            MtAdpLog(LOG_CRITICAL, "[AudNetRcvInit] mem allocation failed!\n");
            return FALSE;
        }
    
        wRet = m_pcAudNetRcv->Create(DEF_AUD_FRAMESIZE, ProcAudNetRcv, (u32)m_pcMtAdpIns);
        if(MEDIANET_NO_ERROR != wRet)
        {
            MtAdpLog(LOG_CRITICAL, "[AudNetRcvInit] create failed. err no :%u\n", wRet);
            SAFE_DELETE(m_pcAudNetRcv);
            return FALSE;
        }
    }    
    
    TLocalNetParam tNetParam;
    memset(&tNetParam, 0, sizeof(tNetParam));
    tNetParam.m_tLocalNet.m_dwRTPAddr = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetNetSeqIpAddr();
    tNetParam.m_tLocalNet.m_wRTPPort = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetPort();
    tNetParam.m_tLocalNet.m_dwRTCPAddr = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetNetSeqIpAddr();
    tNetParam.m_tLocalNet.m_wRTCPPort = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetPort();
    tNetParam.m_dwRtcpBackAddr = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetNetSeqIpAddr();
    tNetParam.m_wRtcpBackPort = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetPort();
    wRet = m_pcAudNetRcv->SetNetRcvLocalParam(tNetParam);
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetRcvInit] SetNetRcvLocalParam failed. err no :%u\n", wRet);
        AudNetRcvQuit();
        return FALSE;
    }

    wRet = m_pcAudNetRcv->StartRcv();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetRcvInit] StartRcv failed. err no :%u\n", wRet);
        AudNetRcvQuit();
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================
  �� �� ���� AudNetRcvQuit
  ��    �ܣ� ��Ƶ���ն����˳�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
BOOL32 CMtAdpNet::AudNetRcvQuit()
{
    if(NULL == m_pcAudNetRcv)
    {
        return FALSE;
    }
    
    u16 wRet = 0;
    wRet = m_pcAudNetRcv->StopRcv();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetRcvQuit] StopRcv failed. err no :%u\n", wRet);
    }

    wRet = m_pcAudNetRcv->RemoveNetRcvLocalParam();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetRcvQuit] RemoveNetRcvLocalParam failed. err no :%u\n", wRet);
    }    
    
    return TRUE;
}

/*=============================================================================
  �� �� ���� VidNetSndInit
  ��    �ܣ� ��Ƶ���Ͷ����ʼ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TNetSndParam &tNetParam
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::VidNetSndInit()
{
    ENTER(m_semVidSnd);

    if(NULL == m_pcMtAdpIns)
    {
        return FALSE;
    }

    TChannel *ptChan = &m_pcMtAdpIns->m_atChannel[VID_SND_INDEX];
    if(CHAN_CONNECTED != ptChan->m_byState)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetSndInit] vid snd chan state is invalid\n");
        return FALSE;
    }

    u16 wRet = 0;   
    
     //������Ƶ����ý������
    m_byVidSndMediaType = ptChan->m_tLogicChan.GetChannelType();

    // xsl [10/26/2006] ����һ�δ���
    if (NULL == m_pcVidNetSnd)
    {
        m_pcVidNetSnd = new CKdvMediaSnd;
        if(NULL == m_pcVidNetSnd)
        {
            MtAdpLog(LOG_CRITICAL, "[VidNetSndInit] mem allocation failed!\n");
            return FALSE;
        }

        wRet = m_pcVidNetSnd->Create(DEF_VID_FRAMESIZE, 
                                 DEF_VID_NETBAND, 
                                 DEF_FRAME_RATE, 
                                 m_byVidSndMediaType);
        if(MEDIANET_NO_ERROR != wRet)
        {
            MtAdpLog(LOG_CRITICAL, "[VidNetSndInit] create failed. err no :%u\n", wRet);
            SAFE_DELETE(m_pcVidNetSnd);
            return FALSE;
        }
    }    
   
    TNetSndParam tSndParam;
    memset(&tSndParam, 0, sizeof(tSndParam));
    tSndParam.m_byNum = 1;
    tSndParam.m_tLocalNet.m_dwRTPAddr = ptChan->m_tSndMediaChannel.GetNetSeqIpAddr();
    tSndParam.m_tLocalNet.m_wRTPPort = ptChan->m_tSndMediaChannel.GetPort();
    tSndParam.m_tLocalNet.m_dwRTCPAddr = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetNetSeqIpAddr();
    tSndParam.m_tLocalNet.m_wRTCPPort = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetPort();
    tSndParam.m_tRemoteNet[0].m_dwRTPAddr = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetNetSeqIpAddr();
    tSndParam.m_tRemoteNet[0].m_wRTPPort = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetPort();
    tSndParam.m_tRemoteNet[0].m_dwRTCPAddr = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetNetSeqIpAddr();
    tSndParam.m_tRemoteNet[0].m_wRTCPPort = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetPort();
    wRet = m_pcVidNetSnd->SetNetSndParam(tSndParam);
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetSndInit] SetNetSndParam failed. err no :%u\n", wRet);
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================
  �� �� ���� VidNetSndQuit
  ��    �ܣ� ��Ƶ�����˳�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::VidNetSndQuit()
{
    ENTER(m_semVidSnd);

    if(NULL == m_pcVidNetSnd)
    {
        return FALSE;
    }

    u16 wRet = 0;
    wRet = m_pcVidNetSnd->RemoveNetSndLocalParam();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetSndQuit] RemoveNetSndLocalParam failed. err no :%u\n", wRet);
        return FALSE;
    }
    
    return TRUE;
}

/*=============================================================================
  �� �� ���� AudNetSndInit
  ��    �ܣ� ��Ƶ���Ͷ����ʼ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TNetSndParam &tNetParam
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::AudNetSndInit()
{
    ENTER(m_semAudSnd);

    if(NULL == m_pcMtAdpIns)
    {       
        return FALSE;
    }

    TChannel *ptChan = &m_pcMtAdpIns->m_atChannel[AUD_SND_INDEX];
    if(CHAN_CONNECTED != ptChan->m_byState)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetSndInit] aud snd chan state is invalid\n");
        return FALSE;
    }
    
    u16 wRet = 0;

    //������Ƶ����ý������
    m_byAudSndMediaType = ptChan->m_tLogicChan.GetChannelType();  

    // xsl [10/26/2006] ����һ�δ���
    if (NULL == m_pcAudNetSnd)
    {
        m_pcAudNetSnd = new CKdvMediaSnd;
        if(NULL == m_pcAudNetSnd)
        {
            MtAdpLog(LOG_CRITICAL, "[AudNetSndInit] mem allocation failed!\n");
            return FALSE;
        }

        wRet = m_pcAudNetSnd->Create(DEF_AUD_FRAMESIZE, 
                                DEF_AUD_NETBAND, 
                                DEF_FRAME_RATE, 
                                m_byAudSndMediaType);
        if(MEDIANET_NO_ERROR != wRet)
        {
            MtAdpLog(LOG_CRITICAL, "[AudNetSndInit] create failed. err no :%u\n", wRet);
            SAFE_DELETE(m_pcAudNetSnd);
            return FALSE;
        }
    }    
        
    TNetSndParam tSndParam;
    memset(&tSndParam, 0, sizeof(tSndParam));
    tSndParam.m_byNum = 1;
    tSndParam.m_tLocalNet.m_dwRTPAddr = ptChan->m_tSndMediaChannel.GetNetSeqIpAddr();
    tSndParam.m_tLocalNet.m_wRTPPort = ptChan->m_tSndMediaChannel.GetPort();
    tSndParam.m_tLocalNet.m_dwRTCPAddr = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetNetSeqIpAddr();
    tSndParam.m_tLocalNet.m_wRTCPPort = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetPort();    
    tSndParam.m_tRemoteNet[0].m_dwRTPAddr = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetNetSeqIpAddr();
    tSndParam.m_tRemoteNet[0].m_wRTPPort = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetPort();
    tSndParam.m_tRemoteNet[0].m_dwRTCPAddr = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetNetSeqIpAddr();
    tSndParam.m_tRemoteNet[0].m_wRTCPPort = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetPort();
    wRet = m_pcAudNetSnd->SetNetSndParam(tSndParam);
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetSndInit] SetNetSndParam failed. err no :%u\n", wRet);
        return FALSE;
    }
    
    return TRUE;
}

/*=============================================================================
  �� �� ���� AudNetSndQuit
  ��    �ܣ� ��Ƶ�����˳�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::AudNetSndQuit()
{
    ENTER(m_semAudSnd);

    if(NULL == m_pcAudNetSnd)
    {
        return FALSE;
    }
    
    u16 wRet = 0;
    wRet = m_pcAudNetSnd->RemoveNetSndLocalParam();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetSndQuit] RemoveNetSndLocalParam failed. err no :%u\n", wRet);
        return FALSE;
    }    
    
    return TRUE;
}

//for test
static u8  g_byNetVideo = 1;
static u8  g_byRTPVideo = 1;
API void netvideo(u8 by)
{
    g_byNetVideo = by;
}

API void rtpvideo(u8 by)
{
    g_byRTPVideo = by;
}
/*=============================================================================
  �� �� ���� VidNetSnd
  ��    �ܣ� ��Ƶ��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� PFRAMEHDR pFrmHdr
             BOOL32 bAvalid 
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::VidNetSnd(u8 *pbyData, u32 dwLen)
{
    ENTER(m_semVidSnd);

    if(NULL == m_pcVidNetSnd)
    {
        return FALSE;
    }
 
    if(0 == g_byRTPVideo)
    {
        return TRUE;
    }

    //rtp�а�����
    s32 nDataLen = 0;
	if(MEDIA_TYPE_H261 == m_byVidSndMediaType)
	{
		if(ERR_OK != H261AddRTPHeader(pbyData, dwLen, m_byRtpPackBuf, 
#ifdef WIN32
            (long *)&nDataLen
#elif defined _LINUX_
            (long *)&nDataLen
#elif defined _VXWORKS_
            &nDataLen
#endif
            ))
		{
			MtAdpLog(LOG_CRITICAL, "[VidNetSnd] H261AddRTPHeader failed!\n");
			return FALSE;
		}
	}
	else if(MEDIA_TYPE_H263 == m_byVidSndMediaType)
	{
		if(DECRTPERR_OK != H263AddRtpHdr(pbyData, dwLen, m_byRtpPackBuf, &nDataLen))
		{
			MtAdpLog(LOG_CRITICAL, "[VidNetSnd] H263AddRtpHdr failed!\n");
			return FALSE;
		}
	}

    if(0 == g_byNetVideo)
    {
        return TRUE;
    }

    //����net��������
    FRAMEHDR hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.m_byFrameRate = DEF_FRAME_RATE;
    hdr.m_byMediaType = m_byVidSndMediaType;
    hdr.m_pData = m_byRtpPackBuf;
    hdr.m_dwDataSize = nDataLen;

    u16 wRet = 0;
    wRet = m_pcVidNetSnd->Send(&hdr);
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetSnd] send vid data failed. err no :%u\n", wRet);
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================
  �� �� ���� AudNetSnd
  ��    �ܣ� ��Ƶ��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� PFRAMEHDR pFrmHdr
             BOOL32 bAvalid 
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::AudNetSnd(u8 *pbyData, u32 dwLen)
{
    ENTER(m_semAudSnd);

    if(NULL == m_pcAudNetSnd)
    {
        return FALSE;
    }

    //����net��������
    FRAMEHDR hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.m_byMediaType = m_byAudSndMediaType;
    hdr.m_pData = pbyData;
    hdr.m_dwDataSize = dwLen;

    u16 wRet = 0;
    wRet = m_pcAudNetSnd->Send(&hdr);
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetSnd] send aud data failed. err no :%u\n", wRet);
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================
  �� �� ���� MtAdpLog
  ��    �ܣ� ������Ϣ��ӡ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byLevel
             s8 * pInfo
             ...
  �� �� ֵ�� void 
=============================================================================*/
void CMtAdpNet::MtAdpLog(u8 byLevel, s8 * pInfo, ...)
{
    if(byLevel > g_cMtAdpApp.m_byLogLevel)
    {
        return;
    }

    s8 szBuf[512] = {0};
    s32 nIndex = sprintf(szBuf, "[MtAdpInst %u]", m_pcMtAdpIns->GetInsID());	

    va_list arg;
    va_start(arg, pInfo);
    vsprintf(szBuf+nIndex, pInfo, arg);
    OspPrintf(TRUE, FALSE, szBuf);
    va_end(arg);
}
