/*****************************************************************************
   ģ����      : mpu2lib
   �ļ���      : mpu2struct.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: CMpu2BasData CMpu2BasAdpGroup �ṹ�ͻص�ʵ��
   ����        : ��־��
   �汾        : V4.7  
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���      �޸�����
   2012/05/10    4.7         ��־��      ����
******************************************************************************/
#include "mpu2struct.h"
#include "mpu2inst.h"



const s8  SECTION_MPUDEBUG[]    = "EQPDEBUG";
const s8  SECTION_VMPSYS[]      = "EQPVMP";
const s8  SECTION_VMPDEFINE_PIC[] = "VMPDEFINEPIC";
s8	MPULIB_CFG_FILE[KDV_MAX_PATH] = {0};

extern BOOL32 g_bPauseSend;

/*=============================================================================
  �� �� ���� CBRecvFrame
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
void CBMpu2BasRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext )
{
	CMpu2BasInst * pInst = (CMpu2BasInst*)dwContext;
    if ( pFrmHdr == NULL || pInst == NULL )
    {
        mpulog( MPU_CRIT, "[CBMpu2BasRecvFrame] PTFrameHeader 0x%x, dwContext 0x%x.\n", pFrmHdr, dwContext);
        return ;
    }

	CMpu2BasAdpGroup *pAdpGroup = pInst->GetBasAdpGroup();
	if ( pAdpGroup == NULL )
	{
		mpulog( MPU_CRIT, "[CBMpu2BasRecvFrame] pAdpGroup 0x%x is null.\n", pAdpGroup);
        return ;
    }

	TFrameHeader tFrameHdr;
	memset( &tFrameHdr, 0, sizeof(tFrameHdr) );

	tFrameHdr.m_dwMediaType = pFrmHdr->m_byMediaType;
	tFrameHdr.m_dwFrameID   = pFrmHdr->m_dwFrameID;
    tFrameHdr.m_dwSSRC      = pFrmHdr->m_dwSSRC;
	tFrameHdr.m_pData       = pFrmHdr->m_pData + pFrmHdr->m_dwPreBufSize; 
	tFrameHdr.m_dwDataSize  = pFrmHdr->m_dwDataSize;
	tFrameHdr.m_tVideoParam.m_bKeyFrame    = pFrmHdr->m_tVideoParam.m_bKeyFrame;
	tFrameHdr.m_tVideoParam.m_wVideoHeight = pFrmHdr->m_tVideoParam.m_wVideoHeight;
	tFrameHdr.m_tVideoParam.m_wVideoWidth  = pFrmHdr->m_tVideoParam.m_wVideoWidth;

    mpulogall( "[CBMpu2BasRecvFrame] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, SSRC:%u, DataSize:%u\n",
              tFrameHdr.m_dwMediaType,
              tFrameHdr.m_dwFrameID,
              tFrameHdr.m_tVideoParam.m_bKeyFrame,
              tFrameHdr.m_tVideoParam.m_wVideoWidth,
              tFrameHdr.m_tVideoParam.m_wVideoHeight,              
              tFrameHdr.m_dwSSRC,
              tFrameHdr.m_dwDataSize );

	u16 wRet = pAdpGroup->SetAdpGrpData(tFrameHdr);

	if (  wRet != (u16)Codec_Success )
	{
		ErrorLog("[CBMpu2BasRecvFrame]SetAdpGrpData tFrameHdr error:%d\n",wRet);
	}


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
void CBMpu2BasSendFrame( PTFrameHeader pFrmHdr, void* dwContext )
{
    if ( pFrmHdr == NULL || dwContext == NULL )
    {
        mpulog( MPU_CRIT, "[CBMpu2BasSendFrame] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrmHdr, dwContext );
        return ;
    }

    if (g_bPauseSend)
    {
        mpulogall("[CBMpu2BasSendFrame] pause send due to debug!\n");
        return;
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

	mpulogall( "[CBMpu2BasSendFrame] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, m_dwDataSize:%u\n",
	    	   tTempFrmHdr.m_byMediaType,
		       tTempFrmHdr.m_dwFrameID,
    		   tTempFrmHdr.m_tVideoParam.m_bKeyFrame,
               tTempFrmHdr.m_tVideoParam.m_wVideoWidth,
	    	   tTempFrmHdr.m_tVideoParam.m_wVideoHeight,	    	   
		       tTempFrmHdr.m_dwDataSize );
    

	u16 wRet = pMediaSnd->Send( &tTempFrmHdr );
    if (wRet != MEDIANET_NO_ERROR)
    {
        mpulog(MPU_CRIT, "[CBMpu2BasSendFrame] pMediaSnd->Send failed, ret.%d!\n", wRet);
    }

	return;
}

/*=============================================================================
  �� �� ���� CBMpuBasAudRecvRtpPk
  ��    �ܣ� ��Ƶ����ص�(rtp pack directly)
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TRtpPack *pRtpPack, u32 dwContext
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2009/07/29  4.6         �ű���         ����
=============================================================================*/
void CBMpuBasAudRecvRtpPk(TRtpPack *pRtpPack, u32 dwContext)
{
    CMpu2BasInst * pInst = (CMpu2BasInst*)dwContext;
    if ( pRtpPack == NULL || pInst == NULL )
    {
        mpulog( MPU_CRIT, "[CBAudRecvFrame] pRtpPack 0x%x, dwContext 0x%x.\n", pRtpPack, dwContext);
        return ;
    }
    
    CMpu2BasAdpGroup *pAdpGroup = pInst->GetBasAdpGroup();
    if ( pAdpGroup == NULL )
    {
        mpulog( MPU_CRIT, "[CBAudRecvFrame] pAdpGroup 0x%x is null.\n", pAdpGroup);
        return ;
    }

    TRtpPkQueue *ptQueue = pInst->GetRtpPkQueue();
    if ( NULL == ptQueue )
    {
        mpulog( MPU_CRIT, "[CBAudRecvFrame] ptQueue 0x%x is null.\n", ptQueue);
        return ;
    }
    
    mpulogall( "[CBAudRecvFrame] MediaType:%u, RealSize:%u, ExSize:%u, SSRC.%d, Sequence:%d, Timestamp.%d\n",
                pRtpPack->m_byPayload,
                pRtpPack->m_nRealSize,
                pRtpPack->m_nExSize,
                pRtpPack->m_dwSSRC,
                pRtpPack->m_wSequence,
                pRtpPack->m_dwTimeStamp);

    if ((u32)pRtpPack->m_nRealSize + (u32) pRtpPack->m_nExSize * sizeof(u32) > MAXNUM_BUF_LEN)
    {
        OspPrintf(TRUE, FALSE, "[CBAudRecvFrame] RtpPk<Type.%d, RealSize.%d, ExSize.%d, SSRC.%d, Seq.%d, TimeStamp.%d> unexpected, ignore it!\n",
                                pRtpPack->m_byPayload,
                                pRtpPack->m_nRealSize,
                                pRtpPack->m_nExSize,
                                pRtpPack->m_dwSSRC,
                                pRtpPack->m_wSequence,
                                pRtpPack->m_dwTimeStamp);
        return;
    }


    if (!ptQueue->IsQueueFull())
    {
        u8 abyBuf[MAXNUM_BUF_LEN];
        memset(abyBuf, 0, sizeof(abyBuf));
        
        //RealSize
        memcpy(abyBuf,
               pRtpPack->m_pRealData,
               min(pRtpPack->m_nRealSize, MAXNUM_BUF_LEN-1));
        
        //ExSize
        if (0 != pRtpPack->m_nExSize)
        {
            memcpy(&abyBuf[pRtpPack->m_nRealSize], 
                    pRtpPack->m_pExData,
                    min( (u32) pRtpPack->m_nExSize*sizeof(u32), MAXNUM_BUF_LEN-1-(u32)pRtpPack->m_nRealSize));
        }

		s32 nLen = sizeof(u32);
        ptQueue->IncBuf(abyBuf, pRtpPack->m_nRealSize + pRtpPack->m_nExSize*nLen, *pRtpPack);
        mpulogall("[CBAudRecvFrame] RtpPk<Type.%d, RealSize.%d, ExSize.%d, SSRC.%d, Seq.%d, TimeStamp.%d> IncBuf for chn.%d!\n",
                    pRtpPack->m_byPayload,
                    pRtpPack->m_nRealSize,
                    pRtpPack->m_nExSize,
                    pRtpPack->m_dwSSRC,
                    pRtpPack->m_wSequence,
                    pRtpPack->m_dwTimeStamp,
                    pInst->GetInsID()-1);
    }
    else
    {
        //��¼��ǰ��
        u8 abyBufIn[MAXNUM_BUF_LEN];
        memset(abyBufIn, 0, sizeof(abyBufIn));

        //RealSize
        if (NULL != pRtpPack->m_pRealData)
        {
            memcpy(abyBufIn,
                   pRtpPack->m_pRealData,
                   min(pRtpPack->m_nRealSize, MAXNUM_BUF_LEN-1));
        }
        else
        {
            OspPrintf(TRUE, FALSE, "[CBAudRecvFrame] RtpPk<Type.%d, RealSize.%d, ExSize.%d, SSRC.%d, Seq.%d, TimeStamp.%d> pRtpPack->m_pRealData == NULL!\n",
                                    pRtpPack->m_byPayload,
                                    pRtpPack->m_nRealSize,
                                    pRtpPack->m_nExSize,
                                    pRtpPack->m_dwSSRC,
                                    pRtpPack->m_wSequence,
                                    pRtpPack->m_dwTimeStamp);
        }


        //ExSize
        if (0 != pRtpPack->m_nExSize)
        {
            if (NULL != pRtpPack->m_pExData)
            {
                memcpy(&abyBufIn[pRtpPack->m_nRealSize], 
                        pRtpPack->m_pExData,
                        min((u32) pRtpPack->m_nExSize*sizeof(u32), MAXNUM_BUF_LEN-1-(u32)pRtpPack->m_nRealSize));
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBAudRecvFrame] RtpPk<Type.%d, RealSize.%d, ExSize.%d, SSRC.%d, Seq.%d, TimeStamp.%d> pRtpPack->m_pExData == NULL!\n",
                                        pRtpPack->m_byPayload,
                                        pRtpPack->m_nRealSize,
                                        pRtpPack->m_nExSize,
                                        pRtpPack->m_dwSSRC,
                                        pRtpPack->m_wSequence,
                                        pRtpPack->m_dwTimeStamp);
            }
        }

        TRtpPack tRtpPkIn = *pRtpPack;

        //ȡ��ͷ����
        u8 abyBufOut[MAXNUM_BUF_LEN];
        memset(abyBufOut, 0, sizeof(abyBufOut));
        u32 dwBufHdLen = MAXNUM_BUF_LEN;
        TRtpPack tRtpPkOut;
        
        ptQueue->DecBuf(abyBufOut, dwBufHdLen, tRtpPkOut);

        mpulogall("[CBAudRecvFrame] RtpPk<Type.%d, RealSize.%d, ExSize.%d, SSRC.%d, Seq.%d, TimeStamp.%d> DecBuf for chn.%d!\n",
                    pRtpPack->m_byPayload,
                    pRtpPack->m_nRealSize,
                    pRtpPack->m_nExSize,
                    pRtpPack->m_dwSSRC,
                    pRtpPack->m_wSequence,
                    pRtpPack->m_dwTimeStamp,
                    pInst->GetInsID()-1
                   );
        
        //RealData
        tRtpPkOut.m_pRealData = abyBufOut;

        //ExData
        tRtpPkOut.m_pExData = abyBufOut + tRtpPkOut.m_nRealSize;

        //������ͷ֡�����ǿ�֡
        if (0 != tRtpPkOut.m_nRealSize)
        {
            pAdpGroup->m_pcMediaAudSnd[0]->Send(&tRtpPkOut);
            pAdpGroup->m_pcMediaAudSnd[1]->Send(&tRtpPkOut);
        }

		s32 nLen = sizeof(u32);
        //���浱ǰ֡
        ptQueue->IncBuf(abyBufIn, tRtpPkIn.m_nRealSize + tRtpPkIn.m_nExSize*nLen, tRtpPkIn);
        
        mpulogall( "[CBAudRecvFrame] RtpPk<Type.%d, RealSize.%d, ExSize.%d, SSRC.%d, Seq.%d, TimeStamp.%d> IncBuf insteadly for chn.%d!\n",
                    pRtpPack->m_byPayload,
                    pRtpPack->m_nRealSize,
                    pRtpPack->m_nExSize,
                    pRtpPack->m_dwSSRC,
                    pRtpPack->m_wSequence,
                    pRtpPack->m_dwTimeStamp,
                    pInst->GetInsID()-1
                    );
    }
    return;
}

/*=============================================================================
  �� �� ���� Init
  ��    �ܣ� ��ʼ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::Init(  u16 wLocalRcvStartPort,    //rtp�������ն˿ڣ�����rtcp�˿�Ϊrtp�˿�+1
							  u16 wRtcpBackPort,         //rtcp�ط��˿�(ת����˿�)��Ŀǰ������ʹ��RTP���ն˿�+1		   
							  u32 dwDestIp,				//����Ŀ��Ip��ַ��
							  u16 dwDestPort,			//����Ŀ����ʼ�˿�
							  u32 dwContext,			//ʵ��ָ��
							  u8  byChnId,				//ͨ��ID //����ͨ��Id ( 0,1,2,3,4,5,6... ),����������
							  u8  byOutChnlNum,		//���뷢��·��
							  u8  byWorkMode,
							  u8  byBasIdx
							)
{
	m_byWorkMode	   = byWorkMode;
	m_dwNetBand        = 8000000;
	m_byOutChnlNum	   = byOutChnlNum;	
	m_wLoclRcvStartPort= wLocalRcvStartPort;
	m_dwDestIp		   = dwDestIp;		
	m_wDestStartPort   = dwDestPort;
	m_byChnId		   = byChnId;
	m_byBasIdx		   = byBasIdx;

    DetailLog("[CMpu2BasAdpGroup::Init] m_dwNetBand:%d m_wLoclRcvStartPort:%d, m_dwDestIp:%s, m_wDestPort:%d,m_byOutChnlNum:%d\n",
           m_dwNetBand,
           wLocalRcvStartPort,
           StrOfIP(dwDestIp),
           dwDestPort,
		   m_byOutChnlNum
           );

	//������Ƶ�������ն���
    m_pcMediaVidRcv    = new CKdvMediaRcv;
	if ( NULL == m_pcMediaVidRcv )
	{
		ErrorLog("CMpu2BasAdpGroup::init--> new CKdvMediaRcv failed!\n" );
		printf( "CMpu2BasAdpGroup::init--> new CKdvMediaRcv failed!\n" );
		return FALSE;
	}

	//������Ƶ�������ն���
    m_pcMediaAudRcv = new CKdvMediaRcv;
	if ( NULL == m_pcMediaAudRcv )
	{
		ErrorLog("CMpu2BasAdpGroup::init--> new CKdvMediaRcv failed!\n" );
		printf( "CMpu2BasAdpGroup::init--> new CKdvMediaRcv failed!\n" );
		return FALSE;
	}
	
	//������Ƶ����Ƶ�������Ͷ���
	u8 byIdx = 0;	
	for (byIdx = 0; byIdx < m_byOutChnlNum; byIdx++)
	{
		m_pcMediaVidSnd[byIdx] = new CKdvMediaSnd;
		m_pcMediaAudSnd[byIdx] = new CKdvMediaSnd;
		
		if ((NULL == m_pcMediaVidSnd[byIdx]) || (NULL == m_pcMediaAudSnd[byIdx]))
		{
			for(u8 byIndex = 0; byIndex < byIdx+1; byIndex++)
			{
				delete m_pcMediaVidSnd[byIndex];
				m_pcMediaVidSnd[byIndex] = NULL;
				delete m_pcMediaAudSnd[byIndex];
				m_pcMediaAudSnd[byIndex] = NULL;
			}
			ErrorLog("CMpu2BasAdpGroup::Create--> new CKdvMediaSnd[%d] failed!\n", byIdx);
			printf( "CMpu2BasAdpGroup::Create--> new CKdvMediaSnd[%d] failed!\n", byIdx );
			return FALSE;
		}				
	}
  
	//ý�������鴴��������ý��������ͨ���ص���������
	u16 wRet = 0;
	do {
// 		//��������ͽ������ʹ�ʱȷ������
// 		TAdapterChannel tBasChnl;
// 		tBasChnl.m_dwAdapterChnlId = m_byChnId;
// 		tBasChnl.m_dwEncNum = m_byOutChnlNum;
// 		tBasChnl.m_dwDecMediaType = MEDIA_TYPE_H264;
// 		wRet = g_cMpu2BasApp.AddBasChnnl(m_byBasIdx,&tBasChnl);
// 		if ( (u16)Codec_Success != wRet )
// 		{
// 			ErrorLog( "[CMpu2AdpGroup]AddBasChnnl:%d byBasIdx:%d  errno: %d \n",m_byChnId,byBasIdx,wRet );
//             printf( "[CMpu2AdpGroup]AddBasChnnl:%d byBasIdx:%d  errno: %d \n", m_byChnId,byBasIdx,wRet );
// 			return FALSE;
// 		}
// 		else
// 		{
// 			KeyLog("[CMpu2AdpGroup]AddBasChnnl:%d success byBasIdx:%d\n",m_byChnId,byBasIdx);
// 			printf( "CMpu2AdpGroup::AddBasChnnl:%d success  byBasIdx:%d\n",m_byChnId,byBasIdx);
// 		}


		printf("before create!\n");
		wRet = m_pcMediaVidRcv->Create(MAX_VIDEO_FRAME_SIZE,
			                        CBMpu2BasRecvFrame,   
				    			    (u32)dwContext );
		KeyLog("[CMpu2BasAdpGroup::Init]Create CBMpu2BasRecvFrame for basidx:%d baschnid:%d success\n",m_byBasIdx,m_byChnId);
		printf("after create!---wRet :%d\n",wRet);
        if( MEDIANET_NO_ERROR != wRet )
		{
//			printf("before destroy!--wRet:%d\n",wRet);
			//m_cAptGrp.Destroy(); 
//			printf("after destroy!\n");
			ErrorLog("CMpu2AdpGroup::Create-->m_pcMediaVidRcv->Create fail, Error code is: %d \n", wRet );
//            printf( "CMpu2AdpGroup::Create-->m_pcMediaVidRcv->Create fail, Error code is: %d \n", wRet );
			break;
		}

		wRet = m_pcMediaAudRcv->Create(MAX_AUDIO_FRAME_SIZE,
			                           CBMpuBasAudRecvRtpPk,   
				    			       (u32)dwContext );

        if( MEDIANET_NO_ERROR != wRet )
		{
			//m_cAptGrp.Destroy(); 
			ErrorLog("CMpuAdpGroup::Create-->m_pcMediaAudRcv->Create fail, Error code is: %d \n", wRet );
//            printf( "CMpuAdpGroup::Create-->m_pcMediaAudRcv->Create fail, Error code is: %d \n", wRet );
			break;
		}
		
		TLocalNetParam tlocalNetParm;
		//��Ƶ���ղ�������
		memset( &tlocalNetParm, 0, sizeof(TLocalNetParam) );
        tlocalNetParm.m_tLocalNet.m_wRTPPort  = m_wLoclRcvStartPort;
		//8KI������ת���˿ڳ�ͻ���ʱ��ذ󶨵�RTCP��Ƶ�˿�+3
#ifdef _8KI_
		tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_wLoclRcvStartPort + 3 +1;	    //46004
#else
        tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_wLoclRcvStartPort + 1;		//46001
#endif
        tlocalNetParm.m_dwRtcpBackAddr        = htonl(m_dwDestIp);
        tlocalNetParm.m_wRtcpBackPort         = wRtcpBackPort;
		
		KeyLog("before SetRcv!\n");
        wRet = m_pcMediaVidRcv->SetNetRcvLocalParam( tlocalNetParm );
		KeyLog("after SetRcv! --ret: %d\n",wRet);
        if( MEDIANET_NO_ERROR !=  wRet )
        {
            //m_cAptGrp.Destroy();
            ErrorLog("CMpuAdpGroup::Create-->m_pcMediaVidRcv->SetNetRcvLocalParam fail, Error code is: %d \n", wRet );
//            printf( "CMpuAdpGroup::Create-->m_pcMediaVidRcv->SetNetRcvLocalParam fail, Error code is: %d \n", wRet );
            break;
		}

		//��Ƶ���ղ�������
	
        memset( &tlocalNetParm, 0, sizeof(TLocalNetParam) );
        tlocalNetParm.m_tLocalNet.m_wRTPPort  = m_wLoclRcvStartPort + 2;
		//8KI������ת���˿ڳ�ͻ���ʱ��ذ󶨵�RTCP��Ƶ�˿�+3
#ifdef _8KI_
		tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_wLoclRcvStartPort + 3 +2;	    //46005
#else
		tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_wLoclRcvStartPort + 1 + 2;	//46003
#endif
        tlocalNetParm.m_dwRtcpBackAddr        = htonl(m_dwDestIp);
        tlocalNetParm.m_wRtcpBackPort         = wRtcpBackPort + 2;

		KeyLog("before SetRcv!\n");
        wRet = m_pcMediaAudRcv->SetNetRcvLocalParam( tlocalNetParm );
		KeyLog("after SetRcv! --ret: %d\n",wRet);
        if( MEDIANET_NO_ERROR !=  wRet )
        {
            //m_cAptGrp.Destroy();
            ErrorLog("CMpuAdpGroup::Create-->m_pcMediaAudRcv->SetNetRcvLocalParam fail, Error code is: %d \n", wRet );
//            printf( "CMpuAdpGroup::Create-->m_pcMediaAudRcv->SetNetRcvLocalParam fail, Error code is: %d \n", wRet );
            break;
		}


		//mpu2:medianet���ն���Ҫ����SetHdFlag�ӿڻ������ò���Ϊfalse��
		//�������ΪTRUE��net��֡ʱ���һ��2K��ͷ�����ͷ�ǲ���Ҫ�ģ�ֻ�����Ӹ�����
        m_pcMediaVidRcv->SetHDFlag( FALSE );
		return TRUE;
	}
	while ( 0 );

	return FALSE;
}

/*=============================================================================
  �� �� ���� StartAdapter
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byBasIdx ��ʾ�ڼ�������
  �� �� ֵ�� void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::StartAdapter( u8 byBasIdx, BOOL32 bStartMediaRcv )
{
	DetailLog("CMpu2BasAdpGroup::StartAdapter\n" );
	u16 wRet = 0;

	if ( IsStart() )
	{
    	DetailLog("CMpu2BasAdpGroup::StartAdapter success\n" );
		return TRUE;
	}	
	wRet = g_cMpu2BasApp.StartVideoAdapter(byBasIdx,m_byChnId);

	if ( bStartMediaRcv )
	{
		m_pcMediaVidRcv->StartRcv();
        m_pcMediaAudRcv->StartRcv();
	}

	if( (u16)Codec_Success != wRet )
	{
		m_pcMediaVidRcv->StopRcv();
		m_pcMediaAudRcv->StopRcv();
        ErrorLog("CMpu2BasAdpGroup::StartAdapter-->m_cAptGrp.StartGroup fail, Error code is: %d \n", wRet );
		return FALSE;
	}

	SetStart(TRUE);

	return TRUE;
}


/*=============================================================================
  �� �� ���� SetVideoEncSend
  ��    �ܣ� ���õ�һ·���������ķ��Ͷ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::SetVideoEncSend( u8 byIdx, u32 dwNetBand,  u16 wLoacalStartPort,  u32 dwDestIp,  u16 dwDestPort )
{
	DetailLog("[SetVideoEncSend] wLoacalStartPort:%u, dest:%s:%u\n", wLoacalStartPort, StrOfIP(dwDestIp), dwDestPort );
	
    u16 wRet = 0;

	do
	{
        u8 byFrameRate = (u8)m_atVidEncParam[byIdx].m_tVideoEncParam.m_byFrameRate;
    	u8 byMediaType = (u8)m_atVidEncParam[byIdx].m_tVideoEncParam.m_byEncType;
    	wRet = m_pcMediaVidSnd[byIdx]->Create( MAX_VIDEO_FRAME_SIZE,
		    	                        dwNetBand,
			                            byFrameRate,
			    				        byMediaType );

        if ( MEDIANET_NO_ERROR != wRet )
        {
            g_cMpu2BasApp.StopVideoAdapter(m_byBasIdx,m_byChnId);
            ErrorLog("CMpuBasAdpGroup::SetVideoEncSend-->m_pcMediaVidSnd[%d].Create fail, Error code is:%d, NetBand.%d, FrmRate.%d, MediaType.%d\n", byIdx, wRet, dwNetBand, byFrameRate, byMediaType );
//            printf( "CMpuBasAdpGroup::SetVideoEncSend-->m_pcMediaVidSnd[%d].Create fail, Error code is: %d \n", byIdx,  wRet );
    	   	break;
        }
        else
        {
            DetailLog("CMpuBasAdpGroup::SetVideoEncSend-->m_pcMediaVidSnd[%d].0x%x.Create succeed, NetBand.%d, FrmRate.%d, MediaType.%d\n", byIdx, m_pcMediaVidSnd[byIdx], dwNetBand, byFrameRate, byMediaType );
//            printf( "CMpuBasAdpGroup::SetVideoEncSend-->m_pcMediaVidSnd[%d].Create succeed!\n", byIdx);
        }
		
		//ѡ��ͨ�����1·�Ǹ���H264,������˫�����2��·�Ǹ���H264
		if ( m_byOutChnlNum == MPU2_BAS_ENHACNED_SELOUTPUT ||  m_byOutChnlNum == MPU2_BAS_BASIC_SELOUTPUT)
		{
			if ( (byIdx ==(MPU2_BAS_ENHACNED_SELOUTPUT-1)) ||
				 (byIdx ==(MPU2_BAS_BASIC_SELOUTPUT-1))
				)
			{
				m_pcMediaVidSnd[byIdx]->SetHDFlag( FALSE );
			}
			else
			{
				m_pcMediaVidSnd[byIdx]->SetHDFlag( TRUE );
			}
		}
		else
		{
			u8 byEnd = m_byOutChnlNum - 2 -1;	//������˫�����2���Ǳ���
			if ( byIdx > byEnd )
			{
				m_pcMediaVidSnd[byIdx]->SetHDFlag( FALSE );
			}
			else
			{
				m_pcMediaVidSnd[byIdx]->SetHDFlag( TRUE );
			}
		}

		//���÷��Ͳ���
		TNetSndParam tNetSndPar;
		memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );
		
		tNetSndPar.m_byNum  = 1;
		tNetSndPar.m_tLocalNet.m_wRTPPort       = wLoacalStartPort;
		tNetSndPar.m_tLocalNet.m_wRTCPPort      = wLoacalStartPort + 1;
		tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = htonl(dwDestIp);
		tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = dwDestPort;
		tNetSndPar.m_tRemoteNet[0].m_dwRTCPAddr = htonl(dwDestIp);
		tNetSndPar.m_tRemoteNet[0].m_wRTCPPort  = dwDestPort + 1;
		
		wRet = m_pcMediaVidSnd[byIdx]->SetNetSndParam( tNetSndPar );
		
		if ( MEDIANET_NO_ERROR != wRet )
		{
			g_cMpu2BasApp.StopVideoAdapter(m_byBasIdx,m_byChnId);
			ErrorLog( "CMpubasAdpGroup::Great-->m_cMediaSnd[%d].SetNetSndParam fail, Error code is: %d \n", byIdx, wRet );
//			printf( "CMpuBasAdpGroup::Great-->m_cMediaSnd[%d].SetNetSndParam fail, Error code is: %d \n", byIdx, wRet );
			break;
		}
		return TRUE;

	} while ( 0 );

	return FALSE;
}

/*=============================================================================
  �� �� ���� SetAudioSend
  ��    �ܣ� ��Ƶ�ջص����� ֧��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::SetAudioSend(u8 byIdx, u32 dwNetBand, u16 wLocalStartPort, u32 dwDestIp, u16 wDestPort)
{
	mpulog( MPU_INFO, "[SetAudioSend] wLoacalStartPort:%u, dest:%s:%u\n", 
                       wLocalStartPort, StrOfIP(dwDestIp), wDestPort );

	u16 wRet = 0;

	do
	{        
        u8 byFrameRate = 50;
    	wRet = m_pcMediaAudSnd[byIdx]->Create( MAX_AUDIO_FRAME_SIZE,
		    	                        dwNetBand,
			                            byFrameRate,
			    				        m_byAudDecPT );

        if ( MEDIANET_NO_ERROR != wRet )
		{
         	g_cMpu2BasApp.StopVideoAdapter(m_byBasIdx,m_byChnId);
    		ErrorLog("SetAudioSend::Great-->m_pcMediaAudSnd[%d].Create fail, Error is:%d, NetBand.%d, FrmRate.%d, MediaType.%d \n", 
						byIdx, wRet, dwNetBand, byFrameRate, m_byAudDecPT );
//            printf( "SetAudioSend::Great-->m_pcMediaAudSnd[0].Create fail, Error is: %d \n", wRet );
    	   	break;
		}
        else
        {
            DetailLog("SetAudioSend::Great-->m_pcMediaAudSnd[%d].0x%x.Create succeed, NetBand.%d, FrmRate.%d, MediaType.%d\n",
						byIdx, m_pcMediaVidSnd[1], dwNetBand, byFrameRate, m_byAudDecPT );
//            printf( "SetAudioSend::Great-->m_pcMediaAudSnd[%d].Create succeed!\n" , byIdx);
        }

    	//���÷��Ͳ���
    	TNetSndParam tNetSndPar;
    	memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );

    	tNetSndPar.m_byNum  = 1;
    	tNetSndPar.m_tLocalNet.m_wRTPPort       = wLocalStartPort;
     	tNetSndPar.m_tLocalNet.m_wRTCPPort      = wLocalStartPort + 1;
    	tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = htonl(dwDestIp);
    	tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = wDestPort;
    	tNetSndPar.m_tRemoteNet[0].m_dwRTCPAddr = htonl(dwDestIp);
    	tNetSndPar.m_tRemoteNet[0].m_wRTCPPort  = wDestPort + 1;

    	wRet = m_pcMediaAudSnd[byIdx]->SetNetSndParam( tNetSndPar );

    	if ( MEDIANET_NO_ERROR != wRet )
		{
    		g_cMpu2BasApp.StopVideoAdapter(m_byBasIdx,m_byChnId);
    		ErrorLog("SetAudioSend::SetNetSndParam-->m_cMediaSnd[%d].SetNetSndParam fail, Error is: %d \n", 
					  byIdx, wRet );
//            printf( "SetAudioSend::SetNetSndParam-->m_cMediaSnd[%d].SetNetSndParam fail, Error is: %d \n", 
//					byIdx, wRet );
    		break;
		}

		return TRUE;

	} while ( 0 );

	return FALSE;
}

/*=============================================================================
  �� �� ���� AddBasChn
  ��    �ܣ� ��ӱ���ͨ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::AddBasChn(u32 dwDecMediaType,TAdapterEncParam * ptVidEncParam, u8 byVailedChnNum)
{
	//���ͨ��
	TAdapterChannel tBasChnl;
	memset( &tBasChnl,0,sizeof(TAdapterChannel) );
	tBasChnl.m_dwAdapterChnlId = m_byChnId;
	tBasChnl.m_dwEncNum = byVailedChnNum;
	tBasChnl.m_dwDecMediaType = dwDecMediaType;
	tBasChnl.m_ptEncParam = ptVidEncParam;
	u16 wRet = 0;
	wRet = g_cMpu2BasApp.AddBasChnnl(m_byBasIdx,&tBasChnl);
	if ( (u16)Codec_Success != wRet )
	{
		ErrorLog( "[CMpu2BasAdpGroup::AddBasChn]AddBasChnnl:%d-%d fail, Error code is: %d \n", m_byBasIdx,m_byChnId,wRet );
		return FALSE;
	}
	else
	{
		KeyLog("[CMpu2BasAdpGroup::AddBasChn]AddBasChnnl:%d-%d success\n",m_byBasIdx,m_byChnId);
	}
	
	
	// ���ñ���ص�
	for (u8 byOutIdx = 0; byOutIdx < byVailedChnNum; byOutIdx++)
	{
		wRet = g_cMpu2BasApp.SetBasVidDataCallback(m_byBasIdx,m_byChnId,byOutIdx,CBMpu2BasSendFrame,m_pcMediaVidSnd[byOutIdx]);
		if ( (u16)Codec_Success != wRet )
		{
			ErrorLog("[CMpu2BasAdpGroup::AddBasChn]m_cAptGrp.SetBasVidDataCallback(%d ,%d,%d ) fail, Error code is: %d \n", m_byBasIdx, m_byChnId, byOutIdx, wRet );
//			printf("[CMpu2BasAdpGroup::AddBasChn]m_cAptGrp.SetBasVidDataCallback(%d ,%d,%d ) fail, Error code is: %d \n", m_byBasIdx, m_byChnId, byOutIdx,wRet);
			return FALSE;
		}
	}	

	//�����²���
	memcpy( m_atVidEncParam, ptVidEncParam, sizeof(TAdapterEncParam) * byVailedChnNum );
	return TRUE;
}

/*=============================================================================
  �� �� ���� AddNewEncParam
  ��    �ܣ� ��ӱ���ͨ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::AddNewEncParam(const TVideoEncParam &tVidEncParam, u8 byEncIdx)
{
	//����²���
	u16 wRet =0;
	wRet = g_cMpu2BasApp.AddNewEncParam(m_byBasIdx,m_byChnId,byEncIdx,tVidEncParam);
	if ( (u16)Codec_Success != wRet )
	{
		ErrorLog( "[CMpu2BasAdpGroup::AddNewEncParam]AddNewEncParam:%d-%d-%d fail, Error code is: %d \n", m_byBasIdx,m_byChnId,byEncIdx,wRet );
		return FALSE;
	}

	// ���ñ���ص�
	wRet = g_cMpu2BasApp.SetBasVidDataCallback(m_byBasIdx,m_byChnId,byEncIdx,CBMpu2BasSendFrame,m_pcMediaVidSnd[byEncIdx]);
	if ( (u16)Codec_Success != wRet )
	{
		ErrorLog("[CMpu2BasAdpGroup::AddNewEncParam]m_cAptGrp.SetBasVidDataCallback(%d ,%d,%d ) fail, Error code is: %d \n", m_byBasIdx, m_byChnId, byEncIdx, wRet );
//		printf("[CMpu2BasAdpGroup::AddNewEncParam]m_cAptGrp.SetBasVidDataCallback(%d ,%d,%d ) fail, Error code is: %d \n", m_byBasIdx, m_byChnId, byEncIdx,wRet);
		return FALSE;
	}

	//���÷���
	u16	wLocalSendPort =0;
#ifdef _8KI_
	TMpu2BasCfg tMpu2BasCfg;
	if ( g_cMpu2BasApp.GetMpu2BasCfg(0,tMpu2BasCfg) )
	{
		u8 byPos = 0;
		byPos = (m_wDestStartPort-tMpu2BasCfg.m_wMcuRcvStartPort)/PORTSPAN;
		wLocalSendPort = BAS_8KE_LOCALSND_PORT +  PORTSPAN *(byPos+byEncIdx);
	}
	else
	{
		ErrorLog("[CMpu2BasAdpGroup::AddNewEncParam]GetMpu2BasCfg error\n!");
	}
#else
	wLocalSendPort = m_wDestStartPort+ PORTSPAN * byEncIdx;
#endif
	
	//�����²���
	m_atVidEncParam->m_dwEncoderId = byEncIdx;
	memcpy( &(m_atVidEncParam[byEncIdx].m_tVideoEncParam), &tVidEncParam, sizeof(TVideoEncParam) );

	if( !SetVideoEncSend(byEncIdx, m_dwNetBand, wLocalSendPort, m_dwDestIp, m_wDestStartPort+ PORTSPAN * byEncIdx) )
	{
		ErrorLog("[CMpu2BasAdpGroup::AddNewEncParam]SetVideoEncSend(%d ,%d,%d ) fail, Error code is: %d \n", m_byBasIdx, m_byChnId, byEncIdx,wRet);
//		printf("[CMpu2BasAdpGroup::AddNewEncParam]SetVideoEncSend(%d ,%d,%d ) fail, Error code is: %d \n", m_byBasIdx, m_byChnId, byEncIdx,wRet);
		return FALSE;
	}
	KeyLog("[CMpu2BasAdpGroup::AddNewEncParam]SetVideoEncSend(%d,%d,%d), LocalStartPort:%u, DestPort:%d\n", 
			m_byBasIdx,
			m_byChnId,
			byEncIdx,	
			wLocalSendPort,
			m_wDestStartPort+ PORTSPAN * byEncIdx
			);

	KeyLog("[CMpu2BasAdpGroup::AddNewEncParam]AddNewEncParam:%d-%d-%d success\n",m_byBasIdx,m_byChnId,byEncIdx);
	return TRUE;
}


/*=============================================================================
  �� �� ���� IsAddNewParam
  ��    �ܣ� �����Ƿ�ı�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::IsAddNewParam(const TVideoEncParam &tOldParam, const TVideoEncParam &tNewParam)
{
	if ( TYPE_MPUBAS== g_cMpu2BasApp.GetBasWorkMode() ||
		TYPE_MPUBAS_H== g_cMpu2BasApp.GetBasWorkMode()
		)
	{
		if (  (tOldParam.m_byEncType != MEDIA_TYPE_NULL &&  tOldParam.m_byEncType != MEDIA_TYPE_NULL) &&
			(tNewParam.m_byEncType != MEDIA_TYPE_NULL &&  tNewParam.m_byEncType!=0 && tNewParam.m_byEncType != tOldParam.m_byEncType )
			)
		{
			return TRUE;	
		}
	}

	if (  (tOldParam.m_byEncType == MEDIA_TYPE_NULL || tOldParam.m_byEncType == 0) &&
		  (tNewParam.m_byEncType != MEDIA_TYPE_NULL&&tNewParam.m_byEncType!=0)
	   )
	{
		return TRUE;
	}

	return FALSE;
}

/*=============================================================================
  �� �� ���� ConverToRealParam2
  ��    �ܣ� �������ת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMpu2BasAdpGroup::ConverToRealParam2(u8 byOutIdx, TAdapterEncParam &tVidEncParam, THDAdaptParam &tBasParam)
{
	if ( byOutIdx >= MAXNUM_BASOUTCHN )
	{
		ErrorLog("[ConverToRealParam2]byOutNum:%d is error!\n",byOutIdx);
		return;
	}

	ConverToRealParam(m_byBasIdx,m_byChnId,byOutIdx,tBasParam);
	tVidEncParam.m_tVideoEncParam.m_byFrameRate = tBasParam.GetFrameRate();
	tVidEncParam.m_tVideoEncParam.m_wVideoWidth = tBasParam.GetWidth();
	tVidEncParam.m_tVideoEncParam.m_wVideoHeight = tBasParam.GetHeight();
	return;
}

/*=============================================================================
  �� �� ���� ConverToRealParam3
  ��    �ܣ� �������ת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMpu2BasAdpGroup::ConverToRealParam3(u8 byOutIdx,THDAdaptParam &tRealParam,THDAdaptParam &tSpecialParam,TAdapterEncParam &tVidEncParam)
{
	if ( byOutIdx >= MAXNUM_BASOUTCHN )
	{
		ErrorLog("[ConverToRealParam3]byOutNum:%d is error!\n",byOutIdx);
		return;
	}

	//[nizhijun 2012/09/25] �Ƚ�֡�ʺͷֱ����Ƿ���Ҫ����
	if ( IsNeedConvertToSpecialParam(tRealParam,tSpecialParam) )
	{
		KeyLog("[ConverToRealParam3]BasIdx:%d,ChnId:%d,outIdx:%d former param %d-%d-%d\n",
			m_byBasIdx,m_byChnId,byOutIdx,
			tVidEncParam.m_tVideoEncParam.m_byFrameRate,
			tVidEncParam.m_tVideoEncParam.m_wVideoWidth,
			tVidEncParam.m_tVideoEncParam.m_wVideoHeight	
			);	
		ConverToRealParam2(byOutIdx,tVidEncParam,tSpecialParam);
		KeyLog("[ConverToRealParam3]BasIdx:%d,ChnId:%d,outIdx:%d change to special param %d-%d-%d\n",
			m_byBasIdx,m_byChnId,byOutIdx,
			tVidEncParam.m_tVideoEncParam.m_byFrameRate,
			tVidEncParam.m_tVideoEncParam.m_wVideoWidth,
			tVidEncParam.m_tVideoEncParam.m_wVideoHeight	
			);
	}
	else
	{
		ConverToRealParam2(byOutIdx,tVidEncParam,tRealParam);
	}
	
	tVidEncParam.m_tVideoEncParam.m_wBitRate = tSpecialParam.GetBitrate();
	if ( g_cMpu2BasApp.IsEnableCheat())
	{
		u16 wBitrate = 	tVidEncParam.m_tVideoEncParam.m_wBitRate;
		wBitrate = g_cMpu2BasApp.GetDecRateDebug( wBitrate );
		tVidEncParam.m_tVideoEncParam.m_wBitRate = wBitrate;
	}
	return;
}

/*=============================================================================
  �� �� ���� IsNeedConvertToSpecialParam
  ��    �ܣ� �������ת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
BOOL32 CMpu2BasAdpGroup::IsNeedConvertToSpecialParam(THDAdaptParam &tRealParam,THDAdaptParam &tSpecialParam)
{
	//�Ƚ�֡��
	if ( tSpecialParam.IsNull() )
	{
		return FALSE;
	}

	if ( tRealParam.GetFrameRate()> tSpecialParam.GetFrameRate()  )
	{
		return TRUE;
	}
				
	//�ȽϷֱ��ʳ˻��Ƿ���Ҫ��С
	if ( (tRealParam.GetWidth()) *(tRealParam.GetHeight()) > (tSpecialParam.GetWidth())*(tSpecialParam.GetHeight())  )
	{
		return TRUE;
	}
	return FALSE;
}


/*=============================================================================
  �� �� ���� IsParamChange
  ��    �ܣ� �����Ƿ�ı�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::IsParamChange(u8 byOutIdx,const TVideoEncParam &tOldParam, const TVideoEncParam &tNewParam)
{
	if ( tOldParam.m_byEncType != tNewParam.m_byEncType )
	{
		KeyLog("[IsParamChange]outIdx:%d m_byEncType old:%d,new:%d\n",byOutIdx,tOldParam.m_byEncType,tNewParam.m_byEncType);
		return TRUE;
	}

	if ( tOldParam.m_wVideoHeight != tNewParam.m_wVideoHeight )
	{
		KeyLog("[IsParamChange]outIdx:%d m_wVideoHeight old:%d,new:%d\n",byOutIdx,tOldParam.m_wVideoHeight,tNewParam.m_wVideoHeight);
		return TRUE;
	}

	if ( tOldParam.m_wVideoWidth != tNewParam.m_wVideoWidth )
	{
		KeyLog("[IsParamChange]outIdx:%d m_wVideoWidth old:%d,new:%d\n",byOutIdx,tOldParam.m_wVideoWidth,tNewParam.m_wVideoWidth);
		return TRUE;
	}

	if ( tOldParam.m_dwProfile != tNewParam.m_dwProfile )
	{
		KeyLog("[IsParamChange]outIdx:%d m_dwProfile old:%d,new:%d\n",byOutIdx,tOldParam.m_dwProfile,tNewParam.m_dwProfile );
		return TRUE;
	}

	if ( tOldParam.m_wBitRate != tNewParam.m_wBitRate )
	{
		KeyLog("[IsParamChange]outIdx:%d m_wBitRate old:%d,new:%d\n",byOutIdx,tOldParam.m_wBitRate,tNewParam.m_wBitRate);
		return TRUE;
	}

	if ( tOldParam.m_byFrameRate != tNewParam.m_byFrameRate )
	{
		KeyLog("[IsParamChange]outIdx:%d m_byFrameRate old:%d,new:%d\n",byOutIdx,tOldParam.m_byFrameRate,tNewParam.m_byFrameRate);
		return TRUE;
	}

	return FALSE;
}

/*=============================================================================
  �� �� ���� SetNetRecvFeedbackVideoParam
  ��    �ܣ� ������������ش�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
BOOL  CMpu2BasAdpGroup::SetNetRecvFeedbackVideoParam( TRSParam tNetRSParam, BOOL32 bRepeatSnd/* = FALSE */ )
{
	u16 wRet = 0;

	//�������
    m_bNeedRS = bRepeatSnd;
	memcpy( &m_tRsParam, &tNetRSParam, sizeof(m_tRsParam) );

    wRet = m_pcMediaVidRcv->ResetRSFlag( tNetRSParam, bRepeatSnd );
	if ( MEDIANET_NO_ERROR != wRet )
	{
		ErrorLog("CMpuBasAdpGroup::SetNetRecvFeedbackVideoParam-->m_pcMediaRcv->ResetRSFlag fail,Error code is:%d\n", wRet );
		return FALSE;
	}

	//nzj:�����Ƶ���� 2010/9/6
	wRet = m_pcMediaAudRcv->ResetRSFlag( tNetRSParam, bRepeatSnd );
	if ( MEDIANET_NO_ERROR != wRet )
	{
		ErrorLog("CMpuBasAdpGroup::SetNetRecvFeedbackVideoParam-->m_pcMediaAudRcv->ResetRSFlag fail,Error code is:%d\n", wRet );
		return FALSE;
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� SetNetSendFeedbackVideoParam
  ��    �ܣ� ����ͼ������緢���ش�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
BOOL  CMpu2BasAdpGroup::SetNetSendFeedbackVideoParam( u16 wBufTimeSpan, u8 byVailedOutNum, BOOL32 bRepeatSnd/*= FALSE*/ )
{
	u16 wRet = 0;
	if (0 == byVailedOutNum || byVailedOutNum > m_byOutChnlNum)
	{
		WarningLog("[SetNetSendFeedbackVideoParam] byVailedOutNum is %d!\n", byVailedOutNum);
		return FALSE;
	}
	//�������
	for (u8 byIdx = 0; byIdx < byVailedOutNum; byIdx++)
	{
		//δ�������Ͷ���Ĳ����ش�����
		if (0 == m_atVidEncParam[byIdx].m_tVideoEncParam.m_wBitRate 
			||  MEDIA_TYPE_NULL == m_atVidEncParam[byIdx].m_tVideoEncParam.m_byEncType
			||  0 == m_atVidEncParam[byIdx].m_tVideoEncParam.m_byFrameRate)
		{
			continue;
		}

		m_awBufTime[byIdx] = wBufTimeSpan;
		
		wRet = m_pcMediaVidSnd[byIdx]->ResetRSFlag( wBufTimeSpan, bRepeatSnd );
		if ( MEDIANET_NO_ERROR != wRet )
		{
			ErrorLog("CMpu2BasAdpGroup::SetNetSendFeedbackVideoParam-->m_pcMediaVidSnd[%d].ResetRSFlag fail,Error code is:%d\n", byIdx, wRet );
			return FALSE;
		}		
		//nzj:�����Ƶ���� 2010/9/6
		wRet = m_pcMediaAudSnd[byIdx]->ResetRSFlag( wBufTimeSpan, bRepeatSnd );
		if ( MEDIANET_NO_ERROR != wRet )
		{
			ErrorLog("CMpu2BasAdpGroup::SetNetSendFeedbackVideoParam-->m_pcMediaAudSnd[%d].ResetRSFlag fail,Error code is:%d\n", byIdx, wRet );
			return FALSE;
		}

	}
	return TRUE;
}

/*=============================================================================
�� �� ���� SetSmoothSendRule
��    �ܣ� ���ã����������������ƽ�����͹���
�㷨ʵ�֣� 
ȫ�ֱ����� BOOL32 bIsStart 
TRUE�����ù��� FALSE���������
��    ���� void
�� �� ֵ�� void 
-------------------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
20101230    4.6         pengjie       create
=============================================================================*/
void CMpu2BasAdpGroup::SetSmoothSendRule( BOOL32 bNeedSet )
{
	DetailLog("[SetSmoothSendRule] IsSet.%d!\n", bNeedSet );
	
#ifdef _LINUX_
	s32 nRet = 0;
	if( bNeedSet )
	{
		for( u8 byOutIdx = 0; byOutIdx < m_byOutChnlNum; byOutIdx++ )
		{
			// 			u16 wBitrate = m_atVidEncParam[byOutIdx].m_tVideoEncParam.m_wBitRate;
			// 			// ��ԭ����ǰ���ʣ�ƽ���������ܰ��ձ�����������
			// 			wBitrate = g_cMpu2BasApp.GetOrigRate(wBitrate);
			//�����ǰ���õ����ʴ���0���ҵ��ڵ�ǰ��Ҫ���õ����ʣ���ô����Ҫ�ٴ�����
			if ( m_awPreRawBiteRate[byOutIdx] > 0  )
			{
				if (  m_awPreRawBiteRate[byOutIdx] == m_awRawBiteRate[byOutIdx] )
				{
					continue;	
				}
			}
			u16 wBitrate = m_awRawBiteRate[byOutIdx];
			if ( wBitrate>0 )
			{
				// ת����Byte
				wBitrate = wBitrate >> 3;
				// ��ֵ������20%
				u32 dwPeak = wBitrate + wBitrate / 5;
				
				DetailLog("[SetSmoothSendRule] Setting OutIdx:%d ssrule for 0x%x:%u, rate/peak=%u/%u KByte.\n",
					byOutIdx, m_dwDestIp, m_wDestStartPort + PORTSPAN * byOutIdx, wBitrate, dwPeak );
				
				nRet = BatchAddSSRule( m_dwDestIp, m_wDestStartPort + PORTSPAN * byOutIdx, wBitrate, dwPeak, 2 );   // default 2 second
				
				if ( 0 == nRet )
				{
					ErrorLog("[SetSmoothSendRule] Set OutIdx:%d rule failed. SSErrno=%d\n", byOutIdx, nRet );
				}
				else
				{
					m_awPreRawBiteRate[byOutIdx]= m_awRawBiteRate[byOutIdx];
				}
			}
		}
		nRet = BatchExecSSRule();
		if (0 == nRet)
		{
			ErrorLog( "[SetSmoothSendRule]BatchExecSSRule failed. SSErrno=%d. \n", SSErrno);
		}
	}
	else
	{
		for( u8 byOutIdx = 0; byOutIdx < m_byOutChnlNum; byOutIdx++ )
		{
			nRet = BatchAddUnsetSSRule( m_dwDestIp, m_wDestStartPort + PORTSPAN * byOutIdx );
			
			if ( 0 == nRet )
			{
				ErrorLog("[ClearSmoothSendRule] Clear OutIdx:%d rule failed. SSErrno=%d\n", byOutIdx, nRet );
			}
		}
		nRet = BatchExecUnsetSSRule();
		if (0 == nRet)
		{
			ErrorLog( "[SetSmoothSendRule]BatchExecUnsetSSRule failed. SSErrno=%d. \n", SSErrno);
		}
	}
#endif
	
	return;
}

/*=============================================================================
  �� �� ���� StopAdapter
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::StopAdapter( BOOL32 bStopMediaRcv )
{
	u16 wRet = 0;
	DetailLog("CMpuBasAdpGroup::StopAdapter\n" );
	if(!IsStart())
	{
    	DetailLog("CMpuBasAdpGroup::StopAdapter success\n" );
		return TRUE;
	}
	if ( bStopMediaRcv )
	{
		m_pcMediaVidRcv->StopRcv();
		m_pcMediaAudRcv->StopRcv();
	}
	
	wRet = 	g_cMpu2BasApp.StopVideoAdapter(m_byBasIdx,m_byChnId);
	
	if ( (u16)Codec_Success != wRet )
	{
        ErrorLog("CMpuBasAdpGroup::StopAdapter-->m_cAptGrp.StopGroup fail, Error code is: %d \n", wRet );
		return FALSE;
	}

	wRet = 	g_cMpu2BasApp.RemoveBasChnnl(m_byBasIdx,m_byChnId);
	
	if ( (u16)Codec_Success != wRet )
	{
        ErrorLog("CMpuBasAdpGroup::StopAdapter-->m_cAptGrp.RemoveBasChnnl fail, Error code is: %d \n", wRet );
		return FALSE;
	}

	SetStart(FALSE);

	memset(	m_awRawBiteRate,0,sizeof(m_awRawBiteRate) );
	memset(	m_awPreRawBiteRate,0,sizeof(m_awPreRawBiteRate) );
	memset(m_atVidEncParam,0,sizeof(m_atVidEncParam));
   	DetailLog("CMpuBasAdpGroup::StopAdapter success\n" );	
	return TRUE;
}

/*=============================================================================
  �� �� ���� SetFastUpdata
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChnIdx: ĳͨ��
  �� �� ֵ�� void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::SetFastUpdata()
{
	DetailLog("CMpu2BasAdpGroup::SetFastUpdata(basidx:%d, chnid:%d)!\n",m_byBasIdx,m_byChnId );

    u16 dwErrorcode = CODEC_NO_ERROR;
    dwErrorcode = g_cMpu2BasApp.SetEncFastUpdata(m_byBasIdx,m_byChnId);
    if (dwErrorcode != CODEC_NO_ERROR)
    {
        ErrorLog( "[SetFastUpdata] For m_byChnId.%d fail(errcode:%d)\n", m_byChnId, dwErrorcode);
        return FALSE;
	}
	return TRUE;
}

/*=============================================================================
  �� �� ���� GetVideoChannelStatis
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::GetVideoChannelStatis(TKdvDecStatis &tAdapterDecStatis )
{
	u16 wRet = 0;
	wRet = g_cMpu2BasApp.GetBasChnDecStatis(m_byBasIdx,m_byChnId,tAdapterDecStatis);

	if ( (u16)Codec_Success != wRet )
	{
		ErrorLog( "CMpu2BasAdpGroup::GetVideoChannelStatis-->m_cAptGrp.GetVideoChannelStatis fail! Error code is:%d\n", wRet );
		return FALSE;
	}
	else
	{
		DetailLog("CMpu2BasAdpGroup::GetVideoChannelStatis-->m_cAptGrp.GetVideoChannelStatis %d-%d-%d\n", m_byBasIdx,m_byChnId,tAdapterDecStatis.m_bVidCompellingIFrm);
		return TRUE;
	}
}

/*=============================================================================
  �� �� ���� ShowChnInfo
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMpu2BasAdpGroup::ShowChnInfo( )
{
    StaticLog("\n----------------Adapter Group Info-------------------------\n");
    
    //������Ϣ
    StaticLog("\n*************Basic info*************\n");
    StaticLog("\t Is Start:%d\n", m_bIsStart);	
    if ( !m_bIsStart) 
    {
        return;
    }

	StaticLog("\t Is m_byWorkMode:%d\n", m_byWorkMode);
	StaticLog("\t Is m_byBasIdx:%d\n", m_byBasIdx);
	StaticLog("\t Is m_byChnId:%d\n", m_byChnId);
	StaticLog("\t Is m_byOutChnlNum:%d\n", m_byOutChnlNum);
    StaticLog("\t Is m_wLoclRcvStartPort:%d\n", m_wLoclRcvStartPort);
	StaticLog("\t Is m_dwDestIp:%d\n", m_dwDestIp);
	StaticLog("\t Is m_wDestStartPort:%d\n", m_wDestStartPort);

    //�������
    StaticLog("\n*************ENC Parmater***********\n");
    
    for(u32 dwIdx = 0; dwIdx < m_byOutChnlNum; dwIdx++)
    {
		StaticLog("-----\t EncOutIdx:%u--------\n", dwIdx);
		StaticLog("\t EncType:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_byEncType);
        StaticLog("\t VideoWidth:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_wVideoWidth);
        StaticLog("\t VideoHeight:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_wVideoHeight);
        StaticLog("\t ProfileType:%d\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_dwProfile);
        StaticLog("\t BitRate:%u\n",m_atVidEncParam[dwIdx].m_tVideoEncParam.m_wBitRate);
		StaticLog("\t FrameRate:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_byFrameRate);
        StaticLog("\t MaxKeyFrameInterval:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_dwMaxKeyFrameInterval);
        StaticLog("\t RcMode:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_byRcMode);
        StaticLog("\t MaxQuant:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_byMaxQuant);
        StaticLog("\t MinQuant:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_byMinQuant);
        StaticLog("\t FrameFmt:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_byFrameFmt);
    }    
	g_cMpu2BasApp.ShowBasChnStatus(m_byBasIdx,m_byChnId,TRUE);
    return;
}

/*=============================================================================
  �� �� ���� SetAudioParam
  ��    �ܣ� ������Ҫ�������ת��Ƶ���غ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL 
=============================================================================*/
BOOL CMpu2BasAdpGroup::SetAudioParam(u8 byAudDecType)
{	
	if (MEDIA_TYPE_NULL == byAudDecType)
	{
		ErrorLog( "CMpuBasAdpGroup::SetAudioParam-->m_byAudDecPT is NULL!\n" );
		return FALSE;
	}

    if (m_byAudDecPT == byAudDecType)
    {
        return TRUE;
    }

    m_byAudDecPT = byAudDecType;

	for (u8 byOutIdx = 0; byOutIdx < m_byOutChnlNum; byOutIdx++)
	{
		u16	wLocalSendPort =0;
#ifdef _8KI_
		TMpu2BasCfg tMpu2BasCfg;
		if ( g_cMpu2BasApp.GetMpu2BasCfg(0,tMpu2BasCfg) )
		{
			u8 byPos = 0;
			byPos = (m_wDestStartPort-tMpu2BasCfg.m_wMcuRcvStartPort)/PORTSPAN;
			wLocalSendPort = BAS_8KE_LOCALSND_PORT +  PORTSPAN *(byPos+byOutIdx) + 4;//55604
		}
		else
		{
			ErrorLog("[CMpu2BasAdpGroup::SetAudioParam]GetMpu2BasCfg error\n!");
		}
#else
		wLocalSendPort = m_wDestStartPort + PORTSPAN * byOutIdx+4;//53004
#endif
		if (!SetAudioSend(  byOutIdx,
							m_dwNetBand,
							wLocalSendPort,
							m_dwDestIp,
							m_wDestStartPort + PORTSPAN * byOutIdx + 2) )
		{
			ErrorLog("CMpuBasAdpGroup::SetAudioParam-->SetAudioSend fail\n" );
			return FALSE;
		}
	}
    return TRUE;
}

/*=============================================================================
  �� �� ���� SetVideoEncParam
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::SetVideoEncParam(  u32 dwDecMediaType,
										 TAdapterEncParam * aptVidEncParam,
										 THDAdaptParam	*aptBasParam,
										 THDAdaptParam *aptSpecialBasParam,
										 u8				 byVailedChnNum,
									     BOOL32		     bSetNetParam,
										 BOOL32 bChangeParam/* =FALSE*/)

{
	KeyLog( "[SetVideoEncParam]dwDecMediaType:%d,byVailedChnNum:%d,bSetNetParam:%d,bChangeParam:%d\n",
				dwDecMediaType, byVailedChnNum, bSetNetParam, bChangeParam
			 );
	if ( NULL == aptVidEncParam )
	{
		ErrorLog("[SetVideoEncParam]aptVidEncParam is NULL!\n" );
		return FALSE;
	}

	if ( NULL == aptBasParam )
	{
		ErrorLog("[SetVideoEncParam]aptBasParam is NULL!\n" );
		return FALSE;
	}
	
	if ( NULL == aptSpecialBasParam )
	{
		ErrorLog("[SetVideoEncParam]aptSpecialBasParam is NULL!\n" );
		return FALSE;
	}
	
	for ( u8 byOutIdx =0;  byOutIdx<byVailedChnNum; byOutIdx++)
	{
		m_awRawBiteRate[byOutIdx] = aptBasParam[byOutIdx].GetBitrate();
	}

	BOOL32 bIsRetOk = FALSE;
	u16 wRet = 0;
	do
	{
		if ( !bChangeParam )
		{
			//�����ò���AddBasChn������ٸ���ԭ�������������ֱ��ʣ��ʴ˴�������ʱ�������ͨ��
			TAdapterEncParam tTempVidEncParam[MAXNUM_BASOUTCHN];
			memset(tTempVidEncParam, 0, sizeof(TAdapterEncParam)*MAXNUM_BASOUTCHN);
			memcpy(tTempVidEncParam, aptVidEncParam, sizeof(TAdapterEncParam)*byVailedChnNum);
			u8 byIdx = 0;
			if ( g_cMpu2BasApp.IsEnableCheat())
			{
				for ( byIdx = 0; byIdx < byVailedChnNum; byIdx++)
				{
					u16 wBitrate = tTempVidEncParam[byIdx].m_tVideoEncParam.m_wBitRate;
					wBitrate = g_cMpu2BasApp.GetDecRateDebug( wBitrate );
					tTempVidEncParam[byIdx].m_tVideoEncParam.m_wBitRate = wBitrate;
				}
			}
		
			if ( !AddBasChn(dwDecMediaType,tTempVidEncParam,byVailedChnNum) )
			{
				ErrorLog("[CMpu2BasAdpGroup::AddBasChn] failed!\n" );
				break;
			}

			//�����ͨ�����ٽ���ת��
			if ( NULL == aptBasParam )
			{
				ErrorLog("[SetVideoEncParam]aptBasParam is NULL!\n" );
				break;
			}

			for ( byIdx = 0; byIdx < byVailedChnNum; byIdx++)
			{		
				ConverToRealParam3(byIdx,aptBasParam[byIdx],aptSpecialBasParam[byIdx],aptVidEncParam[byIdx]);

				if ( IsParamChange( byIdx, m_atVidEncParam[byIdx].m_tVideoEncParam, aptVidEncParam[byIdx].m_tVideoEncParam) )
				{
					wRet = g_cMpu2BasApp.ChangeBasVideoEncParam(m_byBasIdx,m_byChnId,aptVidEncParam[byIdx]);
					if ( (u16)Codec_Success != wRet )
					{
						ErrorLog( "[CMpu2BasAdpGroup::SetVideoEncParam]after AddBasChn m_cAptGrp.ChangeVideoEncParam:%d-%d-%d fail, Error code is: %d \n", 
							m_byBasIdx,m_byChnId,byIdx,wRet );
						break;
					}
					else
					{
						memcpy( &(m_atVidEncParam[byIdx].m_tVideoEncParam), &(aptVidEncParam[byIdx].m_tVideoEncParam), sizeof(TVideoEncParam));
						KeyLog("[CMpu2BasAdpGroup::SetVideoEncParam]after AddBasChn Change:%d-%d-%d param success\n",m_byBasIdx,m_byChnId,byIdx);
					}
				}
			}
			
		}
		else
		{
			for ( u8 byLoop = 0; byLoop<byVailedChnNum; byLoop++ )
			{
				if ( IsAddNewParam(m_atVidEncParam[byLoop].m_tVideoEncParam,aptVidEncParam[byLoop].m_tVideoEncParam) )
				{
					KeyLog("[CMpu2BasAdpGroup::SetVideoEncParam]new param:out:%d,type:%d,width:%d,height:%d,profietype:%d,framerate:%d,biterate:%d\n",
							byLoop,
							aptVidEncParam[byLoop].m_tVideoEncParam.m_byEncType,
							aptVidEncParam[byLoop].m_tVideoEncParam.m_wVideoWidth,
							aptVidEncParam[byLoop].m_tVideoEncParam.m_wVideoHeight,
							aptVidEncParam[byLoop].m_tVideoEncParam.m_dwProfile,
							aptVidEncParam[byLoop].m_tVideoEncParam.m_byFrameRate,
							aptVidEncParam[byLoop].m_tVideoEncParam.m_wBitRate
						  );
					
					TAdapterEncParam tTempNewVidEncParam =aptVidEncParam[byLoop] ;
					u16 wBitrate = tTempNewVidEncParam.m_tVideoEncParam.m_wBitRate;
					wBitrate = g_cMpu2BasApp.GetDecRateDebug( wBitrate );
					tTempNewVidEncParam.m_tVideoEncParam.m_wBitRate = wBitrate;
					if ( !AddNewEncParam( tTempNewVidEncParam.m_tVideoEncParam,byLoop ) )
					{
						ErrorLog("[CMpu2BasAdpGroup::SetVideoEncParam]basidx:%d--chnid:%d--outidx:%d AddNewEncParam fail\n",
								m_byBasIdx,m_byChnId,byLoop
								);
						return FALSE;
					}

					ConverToRealParam3(byLoop,aptBasParam[byLoop],aptSpecialBasParam[byLoop],aptVidEncParam[byLoop]);
					
					if ( IsParamChange( byLoop, m_atVidEncParam[byLoop].m_tVideoEncParam, aptVidEncParam[byLoop].m_tVideoEncParam) )
					{
						wRet = g_cMpu2BasApp.ChangeBasVideoEncParam(m_byBasIdx,m_byChnId,aptVidEncParam[byLoop]);
						if ( (u16)Codec_Success != wRet )
						{
							ErrorLog( "[CMpu2BasAdpGroup::SetVideoEncParam]after AddBasChn m_cAptGrp.ChangeVideoEncParam:%d-%d-%d fail, Error code is: %d \n", 
								m_byBasIdx,m_byChnId,byLoop,wRet );
							break;
						}
						else
						{
							memcpy( &(m_atVidEncParam[byLoop].m_tVideoEncParam), &(aptVidEncParam[byLoop].m_tVideoEncParam), sizeof(TVideoEncParam));
							KeyLog("[CMpu2BasAdpGroup::SetVideoEncParam]after AddBasChn Change:%d-%d-%d param success\n",m_byBasIdx,m_byChnId,byLoop);
						}
					}

					//����±�������ɹ��󣬾�ֱ�ӷ���
					return	TRUE;
				}
				else
				{
					ConverToRealParam3(byLoop,aptBasParam[byLoop],aptSpecialBasParam[byLoop],aptVidEncParam[byLoop]);
					
					if ( IsParamChange( byLoop, m_atVidEncParam[byLoop].m_tVideoEncParam, aptVidEncParam[byLoop].m_tVideoEncParam) )
					{
						wRet = g_cMpu2BasApp.ChangeBasVideoEncParam(m_byBasIdx,m_byChnId,aptVidEncParam[byLoop]);
						if ( (u16)Codec_Success != wRet )
						{
							ErrorLog( "[CMpu2BasAdpGroup::SetVideoEncParam]m_cAptGrp.ChangeVideoEncParam:%d-%d-%d fail, Error code is: %d \n", 
								m_byBasIdx,m_byChnId,byLoop,wRet );
							break;
						}
						else
						{
							memcpy( &(m_atVidEncParam[byLoop].m_tVideoEncParam), &(aptVidEncParam[byLoop].m_tVideoEncParam), sizeof(TVideoEncParam));
							KeyLog("[CMpu2BasAdpGroup::SetVideoEncParam]Change:%d-%d-%d param success\n",m_byBasIdx,m_byChnId,byLoop);
						}
					}
				}
			}		
		}
	
		//��������ڿ�����ʱ�����������þͿ�����
		if ( bSetNetParam)
		{
			for (u8 byIdx = 0; byIdx < m_byOutChnlNum; byIdx++)
			{
				if ( MEDIA_TYPE_NULL == m_atVidEncParam[byIdx].m_tVideoEncParam.m_byEncType
					||  0 == m_atVidEncParam[byIdx].m_tVideoEncParam.m_byFrameRate)
				{
					KeyLog("[CMpu2BasAdpGroup::SetVideoEncParam]basidx:%d,chnid:%d,outidx:%d,m_wBitRate:%d,m_byEncType:%d,m_byFrameRate:%d continue\n",
								m_byBasIdx,m_byChnId,byIdx,
								m_atVidEncParam[byIdx].m_tVideoEncParam.m_wBitRate,
								m_atVidEncParam[byIdx].m_tVideoEncParam.m_byEncType,
								m_atVidEncParam[byIdx].m_tVideoEncParam.m_byFrameRate
								);
					continue;
				}
				//BAS���ط��Ͷ˿�8000A�Ļ���ת����Ŀ�Ķ˿���ͬ,��8KI������ͬ���ʴ˴����¼���
				u16	wLocalSendPort =0;
#ifdef _8KI_
				TMpu2BasCfg tMpu2BasCfg;
				if ( g_cMpu2BasApp.GetMpu2BasCfg(0,tMpu2BasCfg) )
				{
					u8 byPos = 0;
					byPos = (m_wDestStartPort-tMpu2BasCfg.m_wMcuRcvStartPort)/PORTSPAN;
					wLocalSendPort = BAS_8KE_LOCALSND_PORT +  PORTSPAN *(byPos+byIdx);
				}
				else
				{
					ErrorLog("[CMpu2BasAdpGroup::SetAudioParam]GetMpu2BasCfg error\n!");
				}
#else
				wLocalSendPort = m_wDestStartPort+ PORTSPAN * byIdx;
#endif
				if( !SetVideoEncSend(byIdx, m_dwNetBand, wLocalSendPort, m_dwDestIp, m_wDestStartPort+ PORTSPAN * byIdx) )
				{
					ErrorLog("CMpuBasAdpGroup::ChangeVideoEncParam-->SetVideoEncSend <OutIdx:%d>fail\n", byIdx);
					break;
				}
				KeyLog("[SetVideoEncParam]SetVideoEncSend(%d-%d-%d), LocalStartPort:%u, DestPort:%d\n", 
							m_byBasIdx,
							m_byChnId,
							byIdx,	
							wLocalSendPort,
							m_wDestStartPort+ PORTSPAN * byIdx
						);
			}
		}

		bIsRetOk = TRUE;
		break;

	} while ( 0 );
	
	return bIsRetOk;
}

/*=============================================================================
  �� �� ���� SetKeyAndPT
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CMpu2BasAdpGroup::SetKeyAndPT( u8* abyKeyBuf,			 //�����ַ���
								    u8 byKenLen,		 	 //�����ַ�������
									u8 byEncryptMode,		 //����ģʽ
									u8 *pbySndActive,		 //�����������غ�
									u8 byVailedOutNum,		 //��������·��
									u8 byActivePT /* = 0 */,  //�����������غ�
									u8 byRealPT /* = 0 */     //������������ʵ�غ�
								  )
{
	if (0 == byVailedOutNum)
	{
		WarningLog("[SetKeyandPT] byVailedOutNum is 0\n");
		return;
	}
	// ����
	m_pcMediaVidRcv->SetActivePT( byActivePT, byRealPT );
	m_pcMediaVidRcv->SetDecryptKey( (s8*)abyKeyBuf, byKenLen, byEncryptMode );
	KeyLog("[CMpu2BasAdpGroup::SetKeyAndPT]basidx:%d bychnid:%d byActivePT:%d,byRealPT:%d\n",
			m_byBasIdx,m_byChnId,byActivePT,byRealPT
			);
	// ����
	for (u8 byIdx = 0; byIdx < byVailedOutNum; byIdx++)
	{		
		m_pcMediaVidSnd[byIdx]->SetActivePT(pbySndActive[byIdx]);
		m_pcMediaVidSnd[byIdx]->SetEncryptKey( (s8*)abyKeyBuf, byKenLen, byEncryptMode );
	}
}

u16 CMpu2BasAdpGroup::SetResizeMode(u32 dwResizeMode)
{
	return g_cMpu2BasApp.SetEncResizeMode(m_byBasIdx,m_byChnId,dwResizeMode);
}

u16 CMpu2BasAdpGroup::SetAdpGrpData(const TFrameHeader& tFrameInfo)
{
	DetailLog("[SetAdpGrpData]m_byBasIdx:%d,m_byChnId:%d\n",m_byBasIdx,m_byChnId);
	return g_cMpu2BasApp.SetFrameData(m_byBasIdx,m_byChnId,tFrameInfo);
}

/*=============================================================================
  �� �� ���� ModNetSndIpAddr
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMpu2BasAdpGroup::ModNetSndIpAddr(u32 dwSndIp)
{	
	m_dwDestIp = dwSndIp;
	TNetSndParam tSndParam;
	for (u8 byIdx = 0; byIdx < m_byOutChnlNum; byIdx++)
	{
		if (m_pcMediaVidSnd[byIdx] != NULL)
		{
			m_pcMediaVidSnd[byIdx]->GetNetSndParam(&tSndParam);
			tSndParam.m_tRemoteNet[0].m_dwRTPAddr  = htonl(dwSndIp);
			tSndParam.m_tRemoteNet[0].m_dwRTCPAddr = htonl(dwSndIp);
			m_pcMediaVidSnd[byIdx]->SetNetSndParam(tSndParam);
		}

		if (m_pcMediaAudSnd[byIdx] != NULL)
		{
			m_pcMediaAudSnd[byIdx]->GetNetSndParam(&tSndParam);
			tSndParam.m_tRemoteNet[0].m_dwRTPAddr  = htonl(dwSndIp);
			tSndParam.m_tRemoteNet[0].m_dwRTCPAddr = htonl(dwSndIp);
			m_pcMediaAudSnd[byIdx]->SetNetSndParam(tSndParam);
		}
	}
}

/*=============================================================================
�� �� ���� SetBasData
��    �ܣ� ����BAS������������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TMpu2Cfg *pMpu2Cfg ��������ָ��
�� �� ֵ�� void 
=============================================================================*/
void CMpu2BasData::SetBasData(TMpu2Cfg *pMpu2Cfg)
{
	if ( NULL == pMpu2Cfg )
	{
		return;
	}
	SetBasWorkMode(pMpu2Cfg->GetMpu2WorkMode());
	SetBasNum(pMpu2Cfg->GetMpu2EqpNum());

	for ( u8 byIdx = 0; byIdx < pMpu2Cfg->GetMpu2EqpNum(); byIdx++ )
	{
		m_atMpu2BasCfg[byIdx].m_byRegAckNum = 0;
		m_atMpu2BasCfg[byIdx].m_tCfg = pMpu2Cfg->GetMpu2EqpCfg(byIdx);
		//memcpy(&(m_atMpu2BasCfg[byIdx].m_tCfg), &(pMpu2Cfg->GetMpu2EqpCfg(byIdx)), sizeof(TEqpCfg));
	}
	
    return;
}
/*=============================================================================
�� �� ���� GetMpu2BasCfg
��    �ܣ� ����idx���BAS�������ã�ͨ�����÷���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32  ���ʧ��
=============================================================================*/
void CMpu2BasData::SetMpu2BasCfg(u8 byIdx, const TMpu2BasCfg &tMpu2BasCfg)
{
	memcpy( &m_atMpu2BasCfg[byIdx], &(tMpu2BasCfg), sizeof(TMpu2BasCfg) );
	return;
}


/*=============================================================================
�� �� ���� InitVideoAdapter
��    �ܣ� ��ʼ��BAS������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����u8 byIdx ��ʾ�ڼ���BAS���� 
�� �� ֵ��u16 
=============================================================================*/
u16	CMpu2BasData::InitVideoAdapter(u8 byBasIdx)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	u32 dwAdapterMode;
	if ( TYPE_MPU2BAS_BASIC == m_byWorkMode )
	{
		dwAdapterMode = BAS_BASIC;
	}
	else if ( TYPE_MPU2BAS_ENHANCED == m_byWorkMode )
	{
		dwAdapterMode = BAS_ENHANCED;
	}
	else if ( TYPE_MPUBAS == m_byWorkMode )				//����mpu2ģ��mpu bap4ʱʹ��BAS_BASICģʽ
	{
		dwAdapterMode = BAS_BASIC;
	}
	else if( TYPE_MPUBAS_H == m_byWorkMode )
	{
		//todo ����mpu2ģ��mpu bap2ģʽʱ��4.7���ʹ��MPU2_SIMULATE_MPU_BASICģʽ
		//4.7��ʱ������BAS_BASIC���������Ż�
		dwAdapterMode = BAS_BASIC;		
	}
	else
	{
		printf( "[InitVideoAdapter]m_byWorkMode:%d is wrong\n",m_byWorkMode);
		return (u16)Codec_Error_Base;
	}

	TVidAdapterCreate tAdapteCreate;
	tAdapteCreate.m_dwDevVersion = (u32)en_MPU2_Board;
	tAdapteCreate.m_dwMode = dwAdapterMode;
	printf( "[InitVideoAdapter]tAdapteCreate.m_dwMode:%d\n",dwAdapterMode);
	return m_cAptGrp[byBasIdx].Create(tAdapteCreate);
}

/*=============================================================================
�� �� ���� DestroyVideoAdapter
��    �ܣ� �˳�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����u8 byIdx ��ʾ�ڼ���BAS���� 
�� �� ֵ��
=============================================================================*/
u16 CMpu2BasData::DestroyVideoAdapter(u8 byBasIdx)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	return m_cAptGrp[byBasIdx].Destroy();	
}

/*=============================================================================
�� �� ���� StartVideoAdapter
��    �ܣ� ��ʼ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����u8 byIdx ��ʾ�ڼ���BAS���� 
		  u8 byChnId ��ʾͨ��ID
�� �� ֵ��
=============================================================================*/
u16 CMpu2BasData::StartVideoAdapter(u8 byBasIdx,u8 byChnId)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}

	return m_cAptGrp[byBasIdx].Start(byChnId);	
}

/*=============================================================================
�� �� ���� StopVideoAdapter
��    �ܣ� ��ʼ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����u8 byIdx ��ʾ�ڼ���BAS���� 
		  u8 byChnId ��ʾͨ��ID
�� �� ֵ��
=============================================================================*/
u16 CMpu2BasData::StopVideoAdapter(u8 byBasIdx,u8 byChnId)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	DetailLog("[CMpu2BasData::StopVideoAdapter]stop byBasIdx:%d,byChnId:%d\n",byBasIdx,byChnId);
	return m_cAptGrp[byBasIdx].Stop(byChnId);	
}

/*=============================================================================
�� �� ���� AddBasChnnl
��    �ܣ� ���һ��BASͨ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����u8 byIdx ��ʾ�ڼ���BAS���� 
		  u8 byChnId ��ʾͨ��ID
�� �� ֵ��
=============================================================================*/
u16 CMpu2BasData::AddBasChnnl(u8 byBasIdx,const TAdapterChannel *ptAdpChannel)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	DetailLog("[AddBasChnnl]add byBasIdx:%d,byChnId:%d,byOutNum:%d\n",byBasIdx,ptAdpChannel->m_dwAdapterChnlId,ptAdpChannel->m_dwEncNum);
	return m_cAptGrp[byBasIdx].AddVideoChannel(ptAdpChannel);
}

/*=============================================================================
�� �� ���� AddNewEncParam
��    �ܣ� ���һ��BASͨ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����u8 byIdx ��ʾ�ڼ���BAS���� 
		  u8 byChnId ��ʾͨ��ID
�� �� ֵ��
=============================================================================*/
u16 CMpu2BasData::AddNewEncParam(u8 byBasIdx,u8 byChnId,u8 byOutIdx,const TVideoEncParam &tVidEncParam)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	DetailLog("[AddNewEncParam]add byBasIdx:%d,byChnId:%d,byOutIdx:%d\n",byBasIdx,byChnId,byOutIdx);
	TVideoEncParam tTempVidEncParam;
	memcpy(&tTempVidEncParam,&tVidEncParam,sizeof(TVideoEncParam));
	return m_cAptGrp[byBasIdx].AddVideoEncParam(byChnId,&tTempVidEncParam,byOutIdx);
}

/*=============================================================================
�� �� ���� RemoveBasChnnl
��    �ܣ� ɾ��һ��BASͨ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����u8 byIdx ��ʾ�ڼ���BAS���� 
		  u8 byChnId ��ʾͨ��ID
�� �� ֵ��
=============================================================================*/
u16 CMpu2BasData::RemoveBasChnnl(u8 byBasIdx,u8 byChnId)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	DetailLog("[CMpu2BasData::RemoveBasChnnl]remove byBasIdx:%d,byChnId:%d\n",byBasIdx,byChnId);
	return m_cAptGrp[byBasIdx].RemoveVideoChannel(byChnId);
}

/*=============================================================================
�� �� ���� GetBasChnDecStatis
��    �ܣ� ���ĳ��ͨ���Ľ���״̬����������ؼ�֡
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����u8 byIdx ��ʾ�ڼ���BAS���� 
		  u8 byChnId ��ʾͨ��ID
		  TKdvDecStatis &tVidDecStatis ���صĽ���״̬
�� �� ֵ��
=============================================================================*/
u16 CMpu2BasData::GetBasChnDecStatis(u8 byBasIdx,u8 byChnId,TKdvDecStatis &tVidDecStatis)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	
	return m_cAptGrp[byBasIdx].GetVidDecStatis(byChnId,tVidDecStatis);
}

/*=============================================================================
�� �� ���� ChangeBasVideoEncParam
��    �ܣ� //BAS�����²�ʹ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����u8 byIdx ��ʾ�ڼ���BAS���� 
		  u8 byChnId ��ʾͨ��ID
		  TVideoEncParam* ptVidEncParam �������
		  u32 dwEncNum ����·��
�� �� ֵ��
=============================================================================*/
u16 CMpu2BasData::ChangeBasVideoEncParam(u8 byBasIdx,u8 byChnId, TAdapterEncParam &tVidEncParam)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	KeyLog("[ChangeBasVideoEncParam]byBasIdx:%d,byChnId:%d,outidx:%d,tVidEncParam:type:%d-framerate:%d-height:%d-width:%d-biterate:%d-profiletype:%d\n",
			byBasIdx,byChnId,tVidEncParam.m_dwEncoderId,
			tVidEncParam.m_tVideoEncParam.m_byEncType,
			tVidEncParam.m_tVideoEncParam.m_byFrameRate,
			tVidEncParam.m_tVideoEncParam.m_wVideoHeight,
			tVidEncParam.m_tVideoEncParam.m_wVideoWidth,
			tVidEncParam.m_tVideoEncParam.m_wBitRate,
			tVidEncParam.m_tVideoEncParam.m_dwProfile
			);
	return m_cAptGrp[byBasIdx].ChangeVideoEncParam(byChnId,&(tVidEncParam.m_tVideoEncParam), tVidEncParam.m_dwEncoderId);
}

/*=============================================================================
�� �� ���� SetBasVidDataCallback
��    �ܣ� ������Ƶ���ݻص���������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����u8 byIdx ��ʾ�ڼ���BAS���� 
		  u8 byChnId ��ʾͨ��ID
		  u32 dwEncoderId �������ID
		  FRAMECALLBACK fVidData �ص�����
		  void* pContext ���Ͷ���ָ��
�� �� ֵ��
=============================================================================*/
u16 CMpu2BasData::SetBasVidDataCallback(u8 byBasIdx,u8 byChnId, u32 dwEncoderId, FRAMECALLBACK fVidData, void* pContext)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	
	return m_cAptGrp[byBasIdx].SetVidDataCallback(byChnId,dwEncoderId,fVidData,pContext);
}

/*=============================================================================
�� �� ���� SetFrameData
��    �ܣ� ����֡����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����u8 byIdx ��ʾ�ڼ���BAS���� 
		  u8 byChnId ��ʾͨ��ID
		  const TFrameHeader& tFrameInfo ֡����
�� �� ֵ��
=============================================================================*/
u16 CMpu2BasData::SetFrameData(u8 byBasIdx,u8 byChnId, const TFrameHeader& tFrameInfo)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	
	return m_cAptGrp[byBasIdx].SetData(byChnId,tFrameInfo);
}

/*=============================================================================
�� �� ���� SetEncFastUpdata
��    �ܣ� ���ñ���ؼ�֡
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����u8 byIdx ��ʾ�ڼ���BAS���� 
		  u8 byChnId ��ʾͨ��ID
�� �� ֵ��
=============================================================================*/
u16 CMpu2BasData::SetEncFastUpdata(u8 byBasIdx,u8 byChnId)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	
	return m_cAptGrp[byBasIdx].SetFastUpdata(byChnId,0,TRUE);
}

/*=============================================================================
�� �� ���� SetEncResizeMode
��    �ܣ� ���úڱ߼���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����u8 byIdx ��ʾ�ڼ���BAS���� 
		  u8 byChnId ��ʾͨ��ID
		  u32 dwMode //ģʽ0:�Ӻڱߣ�1:�ñߣ�Ĭ��0
�� �� ֵ��
=============================================================================*/
u16 CMpu2BasData::SetEncResizeMode(u8 byBasIdx,u8 byChnId,u32 dwMode)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	
	//todo ͨ��ID
	return m_cAptGrp[byBasIdx].SetVidEncResizeMode(byChnId,dwMode);
}

/*=============================================================================
�� �� ���� ShowBasChnStatus
��    �ܣ� ��ʾ״̬
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����u8 byIdx ��ʾ�ڼ���BAS���� 
		  u8 byChnId ��ʾͨ��ID
�� �� ֵ��
=============================================================================*/
void CMpu2BasData::ShowBasChnStatus(u8 byBasIdx,u8 byChnId,BOOL32 bIsEnc/* = TRUE*/)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return ;
	}
	
	m_cAptGrp[byBasIdx].ShowChnInfo(byChnId,bIsEnc);
}

/*=============================================================================
�� �� ���� GetMpu2BasCfg
��    �ܣ� ����idx���BAS�������ã�ͨ�����÷���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32  ���ʧ��
=============================================================================*/
BOOL32 CMpu2BasData::GetMpu2BasCfg(u8 byIdx, TMpu2BasCfg &tMpu2BasCfg)
{
	if ( (m_byBasNum-1) < byIdx)
	{
		return FALSE;
	}
	
	memcpy( &tMpu2BasCfg, &(m_atMpu2BasCfg[byIdx]), sizeof(TMpu2BasCfg) );
	
	return TRUE;
}

/*=============================================================================
  �� �� ���� ReadDebugValues
  ��    �ܣ� �������ļ���ȡ��������ֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMpu2BasData::ReadDebugValues()
{
    s32 nValue;
	sprintf( MPULIB_CFG_FILE, "%s/mcueqp.ini", DIR_CONFIG );
    ::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "AudioDeferNum", 12, &nValue );
    m_tDebugVal.SetAudioDeferNum((u32)nValue);

    ::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "IsEnableRateAutoDec", 1, &nValue );
    m_tDebugVal.SetEnableBitrateCheat( (nValue==0) ? FALSE : TRUE );

    ::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "DefaultTargetRatePct", 75, &nValue );
    m_tDebugVal.SetDefaultRate( (u16)nValue );

	//  [11/9/2009 pengjie] �Ƿ�Ӻڱ߻�ñ�
    ::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "VidEncHWMode", 0, &nValue );
	if( nValue != 0 && nValue != 1 && nValue != 2)
	{
		nValue = 0;
	}
    m_tDebugVal.SetVidEncHWMode(nValue);

	
	//  [nizhijun 2011/03/21] ����BAS��ʱ���ؼ�֡ʱ����
	::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "BASIframeInterval", 1000, &nValue );
    m_tDebugVal.SetBasIframeInterval(nValue);

	// ���Ƿ��ƽ������
	if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "IsUseSmoothSend", 1, &nValue))
	{
		m_tDebugVal.SetEnableSmoothSend(TRUE);
	}
	else
	{
		m_tDebugVal.SetEnableSmoothSend( (nValue==0) ? FALSE : TRUE );
	}

    return;
}

/*=============================================================================
  �� �� ���� GetDebugVal
  ��    �ܣ� ������ײ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
TDebugVal CMpu2BasData::GetDebugVal()
{
	return m_tDebugVal;
}


/*=============================================================================
  �� �� ���� FreeStatusDataA
  ��    �ܣ� ���״̬����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMpu2BasData::FreeStatusDataA( void )
{
    m_dwMcuNode = INVALID_NODE;
	for ( u8 byIdx = 0; byIdx < m_byBasNum; byIdx++)
	{
		TMpu2BasCfg tMpu2BasCfg;
		if ( GetMpu2BasCfg(byIdx, tMpu2BasCfg) )
		{
			tMpu2BasCfg.m_dwMcuIId = INVALID_INS;
		}
	}

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
void CMpu2BasData::FreeStatusDataB( void )
{
    m_dwMcuNodeB = INVALID_NODE;
	for ( u8 byIdx = 0; byIdx < m_byBasNum; byIdx++)
	{
		TMpu2BasCfg tMpu2BasCfg;
		if ( GetMpu2BasCfg(byIdx, tMpu2BasCfg) )
		{
			tMpu2BasCfg.m_dwMcuIIdB = INVALID_INS;
		}
	}
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////����Ϊvmp����////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////



/*=============================================================================
  �� �� ���� CBMpuSVmpRecvFrame
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
void CBMpuSVmpRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext )
{
	TMpu2VmpRcvCB* pRcvCB = (TMpu2VmpRcvCB*)dwContext;
    if ( pFrmHdr == NULL || pRcvCB == NULL )
    {
        ErrorLog( "[mpu][error] PTFrameHeader 0x%x, dwContext 0x%x.\n", pFrmHdr, dwContext );
        return;
    }

	CHardMulPic* pHardMulPic = pRcvCB->m_pHardMulPic;
	if ( pHardMulPic == NULL )
	{
		ErrorLog( "[mpu][error] pAdpGroup 0x%x is null.\n", pHardMulPic );
        return;
    }

	TFrameHeader tFrameHdr;
	memset( &tFrameHdr, 0, sizeof(tFrameHdr) );

	tFrameHdr.m_dwMediaType = pFrmHdr->m_byMediaType;
	tFrameHdr.m_dwFrameID   = pFrmHdr->m_dwFrameID;
    tFrameHdr.m_dwSSRC      = pFrmHdr->m_dwSSRC;
	tFrameHdr.m_pData       = pFrmHdr->m_pData + pFrmHdr->m_dwPreBufSize; 
	tFrameHdr.m_dwDataSize  = pFrmHdr->m_dwDataSize;
	tFrameHdr.m_tVideoParam.m_bKeyFrame    = pFrmHdr->m_tVideoParam.m_bKeyFrame;
	tFrameHdr.m_tVideoParam.m_wVideoHeight = pFrmHdr->m_tVideoParam.m_wVideoHeight;
	tFrameHdr.m_tVideoParam.m_wVideoWidth  = pFrmHdr->m_tVideoParam.m_wVideoWidth;

    DetailLog( "MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, SSRC:%u, DataSize:%u\n",
              tFrameHdr.m_dwMediaType,
              tFrameHdr.m_dwFrameID,
              tFrameHdr.m_tVideoParam.m_bKeyFrame,
              tFrameHdr.m_tVideoParam.m_wVideoWidth,
              tFrameHdr.m_tVideoParam.m_wVideoHeight,              
              tFrameHdr.m_dwSSRC,
              tFrameHdr.m_dwDataSize );

	pHardMulPic->SetData( pRcvCB->m_byChnnlId, tFrameHdr );

	return;
}



/*=============================================================================
  �� �� ���� FRAMECALLBACK
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
void VMPCALLBACK( PTFrameHeader pFrameInfo, void* pContext )
{
    if ( pFrameInfo == NULL || pContext == NULL )
    {
        ErrorLog( "[CBSendFrame] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrameInfo, pContext );
        return ;
    }

    if (g_bPauseSend)
    {
        ErrorLog("[VMPCALLBACK] pause send due to debug!\n");
        return;
    }

	CKdvMediaSnd *pMediaSnd = (CKdvMediaSnd*)pContext;

	FRAMEHDR tTempFrmHdr;
	memset(&tTempFrmHdr, 0x00, sizeof(tTempFrmHdr));

	tTempFrmHdr.m_byMediaType  = (u8)pFrameInfo->m_dwMediaType;
	tTempFrmHdr.m_dwFrameID    = pFrameInfo->m_dwFrameID;
	tTempFrmHdr.m_byFrameRate  = FRAME_RATE;                //֡�ʹ̶�Ϊ25֡

//	tTempFrmHdr.m_dwSSRC       = 0;                         //���յ�ʱ����Զ�����
	tTempFrmHdr.m_pData        = pFrameInfo->m_pData;
	tTempFrmHdr.m_dwDataSize   = pFrameInfo->m_dwDataSize;
//	tTempFrmHdr.m_dwPreBufSize = 0;
//	tTempFrmHdr.m_dwTimeStamp  = 0;                         //���͵�ʱ����Զ�����
	tTempFrmHdr.m_tVideoParam.m_bKeyFrame    = pFrameInfo->m_tVideoParam.m_bKeyFrame;
	tTempFrmHdr.m_tVideoParam.m_wVideoHeight = pFrameInfo->m_tVideoParam.m_wVideoHeight;
	tTempFrmHdr.m_tVideoParam.m_wVideoWidth  = pFrameInfo->m_tVideoParam.m_wVideoWidth;

	DetailLog( "[CBSendFrame] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, m_dwDataSize:%u\n",
	    	   tTempFrmHdr.m_byMediaType,
		       tTempFrmHdr.m_dwFrameID,
    		   tTempFrmHdr.m_tVideoParam.m_bKeyFrame,
               tTempFrmHdr.m_tVideoParam.m_wVideoWidth,
	    	   tTempFrmHdr.m_tVideoParam.m_wVideoHeight,	    	   
		       tTempFrmHdr.m_dwDataSize );
    

	pMediaSnd->Send( &tTempFrmHdr );

	return;
}

/*lint -save -e429 -e438 -e737*/
/*=============================================================================
  �� �� ���� ReadDebugValues
  ��    �ܣ� �������ļ���ȡ��������ֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void TMpuVmpCfg::ReadDebugValues()
{
    s32 nValue;
    ::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "IsEnableRateAutoDec", 1, &nValue );
    m_tDebugVal.SetEnableBitrateCheat( (nValue==0) ? FALSE : TRUE );

    if ( m_tDebugVal.IsEnableBitrateCheat() )
	{  
		::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "DefaultTargetRatePct", 75, &nValue );
		m_tDebugVal.SetDefaultRate( (u16)nValue );
	}
	
	//  [nizhijun 2010/11/9] �Ƿ�Ӻڱ߻�ñ�,Ĭ����0
	::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "VidSDMode", 0, &nValue );
	if( nValue != 0 && nValue != 1 && nValue != 2)
	{
		nValue = 0;
	}
    m_tDebugVal.SetVidSDMode(nValue);


	//  [nizhijun 2011/03/21] ����VMP��ʱ���ؼ�֡ʱ����
	::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "VMPIframeInterval", 1000, &nValue );
    m_tDebugVal.SetVmpIframeInterval(nValue);


	
	
	s32 nMemEntryNum = 0;
	::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPDEFINE_PIC, "EntryNum", 0, &nMemEntryNum );
	if (0 == nMemEntryNum)
	{
		return;
	}

	// alloc memory
	s8** ppszTable = NULL;        
    ppszTable = new s8*[nMemEntryNum];
    if( NULL == ppszTable )
    {
        OspPrintf(1, 0,  "[AgentGetBrdCfgTable] Fail to malloc memory \n");
        return;
    }
	
	u32 dwEntryNum = nMemEntryNum;
	u32 dwLoop = 0;
    for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
    {
		ppszTable[dwLoop] = new s8[MAX_VALUE_LEN+1];
		if(NULL == ppszTable[dwLoop])
		{
			//�ͷ��ڴ�
			TableMemoryFree( (void**)ppszTable, nMemEntryNum );
			return;
        }
    }
	u32 dwReadEntryNum = dwEntryNum;
	GetRegKeyStringTable( MPULIB_CFG_FILE,    
		SECTION_VMPDEFINE_PIC,      
		"fail",     
		ppszTable, 
		&dwReadEntryNum, 
		MAX_VALUE_LEN + 1);
	
	if (dwReadEntryNum != dwEntryNum)
	{
		//�ͷ��ڴ�
		TableMemoryFree( (void**)ppszTable, nMemEntryNum );
		return;
	}
	
	s8    achSeps[] = "\t";        // �ָ���
    s8    *pchToken = NULL;
	
	for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++)
    {
		if (dwLoop + 1 == MAXNUM_VMPDEFINE_PIC)
		{
			break;
		}
		//����
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if (NULL == pchToken)
        {
            continue;
        }
		
		//Vmp��Ա����
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
        {
            continue;
        }
		m_tDebugVal.SetVmpMemAlias(dwLoop, pchToken);
		
		//Vmp��Ա��Ҫ��ʾ��ͼƬ
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
        {
            continue;
        }
		m_tDebugVal.SetRoute(dwLoop, pchToken);
	}
	
	TableMemoryFree( (void**)ppszTable, nMemEntryNum );

    return;
}

/*=============================================================================
  �� �� ���� PrintEncParam
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void TMpuVmpCfg::PrintEncParam( u8 byVideoIdx ) const
{

		StaticLog( "\n\tEncType: \t%u \n\tRcMode: \t%u \n\tMaxKeyFrameInterval: \t%d \n\tMaxQuant: \t%u \n\tMinQuant: \t%u\n",
                               m_tVideoEncParam[byVideoIdx].m_byEncType,
							   m_tVideoEncParam[byVideoIdx].m_byRcMode,
                               m_tVideoEncParam[byVideoIdx].m_dwMaxKeyFrameInterval,
							   m_tVideoEncParam[byVideoIdx].m_byMaxQuant,
                               m_tVideoEncParam[byVideoIdx].m_byMinQuant);

		StaticLog( "\n\tBitRate: \t%u \n\tSndNetBand: \t%u \n\tFrameRate: \t%u \n\tImageQulity: \t%u \n\tVideoWidth: \t%d \n\tVideoHeight: \t%d\n\tProfileType:%d\n",
                               m_tVideoEncParam[byVideoIdx].m_wBitRate,
							   m_adwMaxSendBand[byVideoIdx],
                               m_tVideoEncParam[byVideoIdx].m_byFrameRate,
							   m_tVideoEncParam[byVideoIdx].m_byImageQulity,
                               m_tVideoEncParam[byVideoIdx].m_wVideoWidth,
							   m_tVideoEncParam[byVideoIdx].m_wVideoHeight,
							   m_tVideoEncParam[byVideoIdx].m_dwProfile);

}
/*=============================================================================
�� �� ���� TableMemoryFree
��    �ܣ� �ͷ�ָ������ڴ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  void **ppMem
u32 dwEntryNum
�� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 TMpuVmpCfg::TableMemoryFree( void **ppMem, u32 dwEntryNum )
{
	if( NULL == ppMem)
	{
		return FALSE;
	}
	for( u32 dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
	{
		if( NULL != ppMem[dwLoop] )
		{
			delete [] (s8*)ppMem[dwLoop];
			ppMem[dwLoop] = NULL;
		}
	}
	delete [] (s8*)ppMem;
	ppMem = NULL;
	return TRUE;
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
void CMpu2BasData::GetDefaultParam(u8 byEnctype, TVideoEncParam& tEncparam)
{
#if 1
    s32 nValue;
    switch( byEnctype )
    {
    case MEDIA_TYPE_MP4:
        tEncparam.m_byEncType = (u8)EnVidFmt[0][0];

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[0][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[0][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[0][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[0][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[0][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4BandWidth", 0, &nValue))
        {
//            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
//            tEncparam.m_dwSndNetBand = (u32)nValue;
        }
/*
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4VideoEncLostFrameOnOff", 0, &nValue))
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
    
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[0][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8)EnVidFmt[0][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[0][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4VideoWidth", 0, &nValue))
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
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H261, %d\n", MEDIA_TYPE_H261);
        tEncparam.m_byEncType = (u8)EnVidFmt[1][0];
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[1][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[1][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[1][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[1][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[1][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261BandWidth", 0, &nValue))
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
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[1][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8)EnVidFmt[1][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[1][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261VideoWidth", 0, &nValue))
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
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H262, %d\n", MEDIA_TYPE_H262);
        tEncparam.m_byEncType = (u8)EnVidFmt[2][0];
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[2][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[2][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[2][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[2][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[2][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262BandWidth", 0, &nValue))
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
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[2][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[2][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[2][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262VideoWidth", 0, &nValue))
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
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H263, %d\n", MEDIA_TYPE_H263);
        tEncparam.m_byEncType = (u8)EnVidFmt[3][0];
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[3][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[3][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[3][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[3][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[3][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263BandWidth", 0, &nValue))
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
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[3][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[3][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[3][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263VideoWidth", 0, &nValue))
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
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H263+, %d\n", MEDIA_TYPE_H263PLUS);
        tEncparam.m_byEncType = (u8)EnVidFmt[4][0];
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSBitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[4][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSVideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[4][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSVideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[4][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSVideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[4][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSEncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[4][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSBandWidth", 0, &nValue))
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
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSFrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[4][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSVideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[4][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSVideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[4][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSVideoWidth", 0, &nValue))
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
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H264, %d\n", MEDIA_TYPE_H264);
        tEncparam.m_byEncType = (u8)EnVidFmt[5][0];
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264SBitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[5][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[5][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[5][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[5][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[5][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264BandWidth", 0, &nValue))
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
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[5][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264SVideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = 0; // (u8) EnVidFmt[5][9];  // Ŀǰh264ֻ֧���ٶ�����
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[5][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264VideoWidth", 0, &nValue))
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

/*=============================================================================
  �� �� ���� GetDefaultParam
  ��    �ܣ� �������ļ���ȡ��������ֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void TMpuVmpCfg::GetDefaultParam( u8 byEnctype, TVideoEncParam& tEncparam, u32& byNetBand )
{
    s32 nValue;

    // �����ж��Ƿ���Ҫ
// 	// ���Ƿ��ƽ������, zgc, 2007-09-28
// 	if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "IsUseSmoothSend", 1, &nValue) )
// 	{
// 		m_byIsUseSmoothSend = VMP_SMOOTH_OPEN;
// 	}
// 	else
// 	{
// 		m_byIsUseSmoothSend = (u8)nValue;
//	}
	
    switch( byEnctype )
    {
    //case 97: (mepg4)
    case MEDIA_TYPE_MP4:
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: MP4, %d\n", MEDIA_TYPE_MP4);
        tEncparam.m_byEncType = (u8)EnVidFmt[0][0];

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4BitrateControlMode", 0, &nValue) )
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[0][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4VideoEcdIKeyRate", 0, &nValue) )
        {
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[0][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4VideoEcdQualMaxValu", 0, &nValue) )
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[0][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4VideoEcdQualMinValu", 0, &nValue) )
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[0][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4EncodeBitRate", 0, &nValue) )
        {
            tEncparam.m_wBitRate = EnVidFmt[0][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4BandWidth", 0, &nValue) )
        {
            byNetBand = 0;
        }
        else
        {
            byNetBand = (u32)nValue;
        }
/*
        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "MPEG4VideoEncLostFrameOnOff", 0, &nValue))
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
//      tEncparam.m_byPalNtsc = (u8)EnVidFmt[0][6];
//      tEncparam.m_byCapPort = (u8)EnVidFmt[0][7];
    
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4FrameRate", 0, &nValue) )
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[0][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4VideoImageQuality", 0, &nValue) )
        {
            tEncparam.m_byImageQulity = (u8)EnVidFmt[0][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4VideoHeight", 0, &nValue) )
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[0][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4VideoWidth", 0, &nValue) )
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
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H261, %d\n", MEDIA_TYPE_H261);
        tEncparam.m_byEncType = (u8)EnVidFmt[1][0];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261BitrateControlMode", 0, &nValue) )
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[1][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261VideoEcdIKeyRate", 0, &nValue) )
        {
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[1][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261VideoEcdQualMaxValu", 0, &nValue) )
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[1][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261VideoEcdQualMinValu", 0, &nValue) )
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[1][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261EncodeBitRate", 0, &nValue) )
        {
            tEncparam.m_wBitRate = EnVidFmt[1][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261BandWidth", 0, &nValue) )
        {
            byNetBand = 0;
        }
        else
        {
            byNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//      tEncparam.m_byPalNtsc = (u8)EnVidFmt[1][6];
//      tEncparam.m_byCapPort = (u8)EnVidFmt[1][7];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261FrameRate", 0, &nValue) )
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[1][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261VideoImageQuality", 0, &nValue) )
        {
            tEncparam.m_byImageQulity = (u8)EnVidFmt[1][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261VideoHeight", 0, &nValue) )
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[1][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261VideoWidth", 0, &nValue) )
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
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H262, %d\n", MEDIA_TYPE_H262);
        tEncparam.m_byEncType = (u8)EnVidFmt[2][0];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262BitrateControlMode", 0, &nValue) )
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[2][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262VideoEcdIKeyRate", 0, &nValue) )
        {
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[2][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262VideoEcdQualMaxValu", 0, &nValue) )
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[2][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262VideoEcdQualMinValu", 0, &nValue) )
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[2][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262EncodeBitRate", 0, &nValue) )
        {
            tEncparam.m_wBitRate = EnVidFmt[2][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "262BandWidth", 0, &nValue) )
        {
            byNetBand = 0;
        }
        else
        {
            byNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//      tEncparam.m_byPalNtsc = (u8)EnVidFmt[2][6];
//      tEncparam.m_byCapPort = (u8)EnVidFmt[2][7];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262FrameRate", 0, &nValue) )
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[2][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262VideoImageQuality", 0, &nValue) )
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[2][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "262VideoHeight", 0, &nValue) )
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[2][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262VideoWidth", 0, &nValue) )
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
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H263, %d\n", MEDIA_TYPE_H263);
        tEncparam.m_byEncType = (u8)EnVidFmt[3][0];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263BitrateControlMode", 0, &nValue) )
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[3][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263VideoEcdIKeyRate", 0, &nValue) )
        {
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[3][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263VideoEcdQualMaxValu", 0, &nValue) )
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[3][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263VideoEcdQualMinValu", 0, &nValue) )
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[3][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263EncodeBitRate", 0, &nValue) )
        {
            tEncparam.m_wBitRate = EnVidFmt[3][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "263BandWidth", 0, &nValue) )
        {
            byNetBand = 0;
        }
        else
        {
            byNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//      tEncparam.m_byPalNtsc = (u8)EnVidFmt[3][6];
//      tEncparam.m_byCapPort = (u8)EnVidFmt[3][7];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263FrameRate", 0, &nValue) ) 
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[3][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263VideoImageQuality", 0, &nValue) )
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[3][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "263VideoHeight", 0, &nValue) )
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[3][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263VideoWidth", 0, &nValue) )
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
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H263+, %d\n", MEDIA_TYPE_H263PLUS);
        tEncparam.m_byEncType = (u8)EnVidFmt[4][0];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSBitrateControlMode", 0, &nValue) )
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[4][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSVideoEcdIKeyRate", 0, &nValue) )
        {
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[4][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSVideoEcdQualMaxValu", 0, &nValue) )
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[4][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSVideoEcdQualMinValu", 0, &nValue) )
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[4][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSEncodeBitRate", 0, &nValue) )
        {
            tEncparam.m_wBitRate = EnVidFmt[4][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSBandWidth", 0, &nValue) )
        {
            byNetBand = 0;
        }
        else
        {
            byNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//      tEncparam.m_byPalNtsc = (u8)EnVidFmt[4][6];
//      tEncparam.m_byCapPort = (u8)EnVidFmt[4][7];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSFrameRate", 0, &nValue) )
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[4][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSVideoImageQuality", 0, &nValue) )
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[4][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSVideoHeight", 0, &nValue) )
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[4][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSVideoWidth", 0, &nValue) )
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
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H264, %d\n", MEDIA_TYPE_H264);
        tEncparam.m_byEncType = (u8)EnVidFmt[5][0];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264SBitrateControlMode", 0, &nValue) )
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[5][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264VideoEcdIKeyRate", 0, &nValue) )
        {
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[5][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264VideoEcdQualMaxValu", 0, &nValue) )
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[5][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264VideoEcdQualMinValu", 0, &nValue) )
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[5][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264EncodeBitRate", 0, &nValue) )
        {
            tEncparam.m_wBitRate = EnVidFmt[5][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "264BandWidth", 0, &nValue) )
        {
            byNetBand = 0;
        }
        else
        {
            byNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//      tEncparam.m_byPalNtsc = (u8)EnVidFmt[5][6];
//      tEncparam.m_byCapPort = (u8)EnVidFmt[5][7];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264FrameRate", 0, &nValue) )
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[5][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264SVideoImageQuality", 0, &nValue) )
        {
            tEncparam.m_byImageQulity = 0; // (u8) EnVidFmt[5][9];  // Ŀǰh264ֻ֧���ٶ�����
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "264VideoHeight", 0, &nValue) )
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[5][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264VideoWidth", 0, &nValue) )
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

    return;
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
09/03/14    4.6         �ű���                �ӱ�������ֲ����
=============================================================================*/
u8 TMpuVmpCfg::GetVmpChlNumByStyle( u8 byVMPStyle )
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
	case VMP_STYLE_TEN_M:
        byMaxMemNum = 10;
        break;
    case VMP_STYLE_THIRTEEN:
	case VMP_STYLE_THIRTEEN_M:
        byMaxMemNum = 13;
        break;
    case VMP_STYLE_SIXTEEN:
        byMaxMemNum = 16;
        break;       
    case VMP_STYLE_SEVEN:
        byMaxMemNum = 7;
        break;
    case VMP_STYLE_TWENTY:
        byMaxMemNum = 20;
        break;
	case VMP_STYLE_THREE_2BOTTOMRIGHT:
	case VMP_STYLE_THREE_2BOTTOM2SIDE:
	case VMP_STYLE_THREE_2BOTTOMLEFT:
	case VMP_STYLE_THREE_VERTICAL:
		byMaxMemNum = 3;
        break;
	case VMP_STYLE_FOUR_3BOTTOMMIDDLE:
		byMaxMemNum = 4;
        break;
	case VMP_STYLE_TWENTYFIVE:
		byMaxMemNum = 25;
        break;
    default:
        break;
    }
    
    return byMaxMemNum;
}


void TMpuVmpCfg::InitVmpStyleChnlRes()
{
	u8 byStyle = 0;
	u8 byLoop = 0;
     memset( m_aVMPStyleChnlRes, 0, sizeof( m_aVMPStyleChnlRes ) );

    byStyle = VMP_STYLE_ONE;        //1����
    m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_HD1080;


    byStyle = VMP_STYLE_VTWO;       //����2����
    for(byLoop = 0; byLoop < 2;byLoop ++)
    {
         m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_720_960x544;
    }


    byStyle = VMP_STYLE_HTWO;       //һ��һС2����
    m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_HD1080;
    m_aVMPStyleChnlRes[byStyle][1] = VIDEO_FORMAT_640x368;


    byStyle = VMP_STYLE_THREE;      //3����
    for(byLoop = 0; byLoop < 3; byLoop ++)
    {
         m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_960x544;
    }


    byStyle = VMP_STYLE_FOUR;       //4����
    for(byLoop = 0; byLoop < 4; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_960x544;
    }


    byStyle = VMP_STYLE_SIX;        //6����
    m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_HD720;


    for(byLoop = 1; byLoop < 6; byLoop ++)
    {
         m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_640x368;
    }


    byStyle = VMP_STYLE_EIGHT;      //8����
    //m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_1440x816;
    m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_HD720;


    for(byLoop = 1; byLoop < 8; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }


    byStyle = VMP_STYLE_NINE;       //9����
    for(byLoop = 0; byLoop < 9; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_640x368;
    }


    byStyle = VMP_STYLE_TEN;        //10����
    m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_960x544;
    m_aVMPStyleChnlRes[byStyle][5] = VIDEO_FORMAT_960x544;


    for(byLoop = 1; byLoop < 5; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }
    for(byLoop = 6; byLoop < 10; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }

    byStyle = VMP_STYLE_THIRTEEN;//13����
    m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_960x544;


    for(byLoop = 1; byLoop < 13; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }


    byStyle = VMP_STYLE_SIXTEEN;//16����
    for(byLoop = 0; byLoop < 16; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }


    byStyle = VMP_STYLE_SPECFOUR;//����4����
    m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_HD720;


    for(byLoop = 1; byLoop < 4; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_640x368;
    }


    byStyle = VMP_STYLE_SEVEN;      //7����
    for(byLoop = 0; byLoop < 3; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_960x544;
    }
    for(byLoop = 3; byLoop < 7; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }


    byStyle = VMP_STYLE_TWENTY; //20����
    for(byLoop = 0; byLoop < 20; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }

	byStyle = VMP_STYLE_TEN_M; //10���棬���¸�4���м�����
    for(byLoop = 0; byLoop < 2; byLoop ++)//�м��������
    {
		m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_960x544;
		
    }
	for(byLoop = 2; byLoop < 10; byLoop ++)//����4��
    {
		m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
		
    }

	byStyle = VMP_STYLE_THIRTEEN_M; //13���棬�м�һ����
    for(byLoop = 0; byLoop < 13; byLoop ++)
    {
		if ( byLoop == 0)//�м��Ǹ�
		{
			m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_960x544;
			continue;
		}
		m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }
	byStyle = VMP_STYLE_TWENTYFIVE;
	//TODO:25����壬���ĳ�Ա����mpu2���Լ��ĺ�
   for(byLoop = 0; byLoop < 25; byLoop ++)
   {
	   m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_CIF;
   }
	
}
//END OF FILE
