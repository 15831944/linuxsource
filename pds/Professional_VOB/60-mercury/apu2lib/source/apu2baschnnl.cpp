#include "apu2baschnnl.h"

/*====================================================================
������      ��SetAudioMode
����        ��Ĭ������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����PFRAMEHDR pFrmHdr
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
static void SetDefAudMode(PFRAMEHDR pFrmHdr)
{
	if (pFrmHdr->m_byMediaType == MEDIA_TYPE_AACLC)
    {
        pFrmHdr->m_byAudioMode = AUDIO_MODE_AACLC_32_M;
    }
    else if (pFrmHdr->m_byMediaType == MEDIA_TYPE_AACLD)
    {
        pFrmHdr->m_byAudioMode = AUDIO_MODE_AACLD_32_M;
    }
	return;
}

/*====================================================================
������      ��MixEncFrameCallBack
����        ������ص�
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����TKdvMixerChnlInfoe*	ptChnlInfo
			  PTFrameHeader			pFrameInfo
			  void*					pContext
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
static void BasEncFrameCallBack(u32* pdwChID, PFRAMEHDR pFrmHdr, void* pContext)
{
	PTR_NULL_VOID(pContext);

	/*ý��ͨ����Ϣ*/
	u8 byRcvIdx = *pdwChID / AUDIO_ADAPTOR_MAX_ENCNUM;
	u8 bySndIdx = *pdwChID % AUDIO_ADAPTOR_MAX_ENCNUM;
	CApu2BasChnnl *pcChnnl = (CApu2BasChnnl*)pContext;

	/*��ȡ���Ͷ�������*/
	TAudBasInfo *ptAudBasInfo = pcChnnl->GetChnlInfo(byRcvIdx);

	//255�������Ͳ�����
	if (ptAudBasInfo->abyEncAudType[bySndIdx] == MEDIA_TYPE_NULL)
	{
		return;
	}

	/*����Ĭ����Ƶ����*/
	SetDefAudMode(pFrmHdr);
		
	CKdvMediaSnd *pcMediaSnd = pcChnnl->GetMediaSnd(*pdwChID);
    PTR_NULL_VOID(pcMediaSnd);
	
	/*��ʼ����*/
    u16 wRet = pcMediaSnd->Send(pFrmHdr);
    if (wRet != MEDIANET_NO_ERROR)
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[BasEncFrameCallBack] BAS.%d Send Failed Because.%d SndChnId.%d (%d, %d) MediaType.%d\n", 
			pcChnnl->GetInsID(), wRet, *pdwChID, byRcvIdx, bySndIdx, pFrmHdr->m_byMediaType);
    }

    return;
}

/*====================================================================
������      : MixNetRcvCallBack
����        �����ն���ص�
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����PFRAMEHDR pFrmHdr
			  u32       dwContext
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
extern u32 g_dwSaveNum;
extern u16 g_wChnIdx;
static void BasNetRcvCallBack(PFRAMEHDR pFrmHdr, u32 dwContext)
{
	static FILE* s_fFile = NULL;
    static FILE* s_fLen = NULL;
	
	/*�����ص���Ϣ*/
	CBasRcvCallBack *pcCallBack = (CBasRcvCallBack*)dwContext;

	SetDefAudMode(pFrmHdr);
	
	/*������Ƶ��������*/
    CKdvAudAdaptor* pcKdvBas = pcCallBack->m_pcBasChn->GetKdvBas();

	/*���ý�����Ϣ*/
	u16 wRet = pcKdvBas->SetAudDecData(pcCallBack->m_wChnId, pFrmHdr);
    if (wRet != (u16)Codec_Success)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[MixNetRcvCallBack] BAS.%d SetAudDecData failed because.%d ChnId.%d datasize.%d!\n",
												pcCallBack->m_pcBasChn->GetInsID(), wRet, 
												pcCallBack->m_wChnId, pFrmHdr->m_dwDataSize);
    }
	return;
}

/*====================================================================
������      : CApu2Chnnl
����        ��CApu2Chnnl����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const CEqpBase* const pEqp        �󶨻ص�����
              CEqpCfg* const        pcCfg       ������
			  const u16             wChnIdx     ��ʼͨ������
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
CApu2BasChnnl::CApu2BasChnnl(const CEqpBase* const pEqp, CEqpCfg* const pcCfg, const u16 wChnIdx)
			:CEqpChnnlBase(pEqp, new CApu2BasChnStatus), 
			 m_pcAudioRcv(NULL),
			 m_pcAudioSnd(NULL),
			 m_pcRcvCallBack(NULL),
			 m_byQualityLvl(0),
			 m_pcBasCfg((CApu2BasCfg*)pcCfg)
{	
	PTR_NULL_VOID(m_pcBasCfg);
	VariablesClear(0);
	SetState((u8)CApu2BasChnStatus::IDLE);

	//����ͨ������
	CEqpChnnlBase::SetChnIdx(wChnIdx);

	//������Ƶ�����ȼ�
	m_byQualityLvl = m_pcBasCfg->m_byQualityLvl;

	//ʵ������Ƶ���ն���
	CreateAudioRcv(MAXNUM_APU2_BASCHN);

	//ʵ������Ƶ���Ͷ���
	CreateAydioSnd(MAXNUM_APU2_BASCHN * MAXNUM_APU2BAS_OUTCHN);
}

/*====================================================================
������      ��CApu2BasChnnl
����        ��CApu2BasChnnl����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
CApu2BasChnnl::~CApu2BasChnnl()
{
	CApu2BasChnnl::Destroy();
}

/*====================================================================
������      ��OnMessage
����        ��ʵ����Ϣ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage* const pcMsg
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasChnnl::OnMessage(CMessage* const pcMsg)
{
	PTR_NULL_RETURN(pcMsg, FALSE);

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"BAS: [CApu2BasChnnl::OnMessage] BAS.%d Received MSG [%u](%s).\n", GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));
	
	if (NULL == m_pcBasCfg)
	{
		CServMsg cServMsg(pcMsg->content, pcMsg->length);
		cServMsg.SetErrorCode(ERR_BAS_GETCFGFAILED);
		cServMsg.SetEventId(pcMsg->event + 2);
		PostMsgToMcu(cServMsg);
		return TRUE;
	}
	
	switch (pcMsg->event)
	{
	case MCU_BAS_STARTADAPT_REQ:
		ProcStartAdaptReq(pcMsg);
		break;

	case MCU_BAS_CHANGEAUDPARAM_REQ:
		ProcChgAdaptReq(pcMsg);
		break;

	case MCU_BAS_STOPADAPT_REQ:
		ProcStopAdaptReq(pcMsg);
		break;

	default:
		break;
	}
	return TRUE;
}

/*====================================================================
������      : Destroy
����        ��Apu2ͨ������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasChnnl::Destroy()
{
	SAFE_DELETE_ARRAY(m_pcAudioRcv);
	SAFE_DELETE_ARRAY(m_pcAudioSnd);
	SAFE_DELETE_ARRAY(m_pcRcvCallBack);
	VariablesClear(0);
	return TRUE;
}

/*====================================================================
������      : SetSndObjectPara
����        �����÷��Ͷ������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u32 dwDstIp     Ŀ��IP
			  u16 wDstPort    Ŀ�Ķ˿�
			  u8  byChnIdx    ͨ������    

����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasChnnl::SetSndObjectPara(u32 dwDstIp, u16 wDstPort, u8 bySndChnIdx)
{
	PTR_NULL_RETURN(m_pcAudioSnd, FALSE);

	/*���·��Ͷ���*/
	u16 wRet = 0;
	u8 byRcvIdx = bySndChnIdx / MAXNUM_APU2BAS_OUTCHN;
	u8 bySndIdx = bySndChnIdx % MAXNUM_APU2BAS_OUTCHN;
    wRet = m_pcAudioSnd[bySndChnIdx].Create(MAX_AUDIO_FRAME_SIZE, 64<<10, 25, m_atAudBasInfo[byRcvIdx].abyEncAudType[bySndIdx]);
    if(wRet != MEDIANET_NO_ERROR)
    {
		/*��������255*/
		if (m_atAudBasInfo[byRcvIdx].abyEncAudType[bySndIdx] == MEDIA_TYPE_NULL)
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_BAS, "BAS: [CApu2BasChnnl::SetSndObjectPara][warning] BAS.%d m_pcAudioSnd[%d] Not Create! REASON: EncAudType--MEDIA_TYPE_NULL!\n", GetInsID(), bySndChnIdx);
		}
		/*���󴴽��쳣*/
        else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "BAS: [CApu2BasChnnl::SetSndObjectPara][error] BAS.%d Created m_pcAudioSnd[%d] of AudType:[%d] Failed! REASON: %d!\n", GetInsID(), bySndChnIdx, m_atAudBasInfo[byRcvIdx].abyEncAudType[bySndIdx], wRet);
		}
		return FALSE;
    }
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "BAS: [CApu2BasChnnl::SetSndObjectPara] BAS.%d Created m_pcAudioSnd[%d] of AudType:[%d] Successed.\n", GetInsID(), bySndChnIdx, m_atAudBasInfo[byRcvIdx].abyEncAudType[bySndIdx]);
	
    
    TNetSndParam tNetSndPar;
    memset( &tNetSndPar, 0, sizeof(tNetSndPar));
    tNetSndPar.m_byNum  = 1;
	/*Զ��RTP��ַ���˿�*/
    tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = dwDstIp;
    tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = wDstPort;
	/*����RTP��RTCP���Ͷ˿�*/
	tNetSndPar.m_tLocalNet.m_wRTPPort       = wDstPort;
    tNetSndPar.m_tLocalNet.m_wRTCPPort      = wDstPort + 1;

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"BAS: [CApu2BasChnnl::SetSndObjectPara] BAS.%d m_pcAudioSnd[%d] LocalSnd<RtpPort.%d, RtcpPort.%d>, RemoteRcv<IP.%s, Port.%d>.\n", 
		GetInsID(), bySndChnIdx, tNetSndPar.m_tLocalNet.m_wRTPPort, tNetSndPar.m_tLocalNet.m_wRTCPPort,
		StrOfIP(tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr), tNetSndPar.m_tRemoteNet[0].m_wRTPPort);

	/*���÷��Ͷ������*/
    wRet = m_pcAudioSnd[bySndChnIdx].SetNetSndParam(tNetSndPar);
    if(wRet != MEDIANET_NO_ERROR)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "BAS: [CApu2BasChnnl::SetSndObjectPara][error] BAS.%d m_pcAudioSnd[%d] SetNetSndParam Failed! REASON: %d!\n",GetInsID(), bySndChnIdx, wRet);
		return FALSE;
    }
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "BAS: [CApu2BasChnnl::SetSndObjectPara] BAS.%d m_pcAudioSnd[%d] SetNetSndParam Success.\n", GetInsID(), bySndChnIdx);

	/*�����ش����*/
    wRet = m_pcAudioSnd[bySndChnIdx].ResetRSFlag( 2000, TRUE );
    if(wRet != MEDIANET_NO_ERROR)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "BAS: [CApu2BasChnnl::SetSndObjectPara][error] BAS.%d m_pcAudioSnd[%d] ResetRSFlag Failed! REASON: %d!\n", GetInsID(), bySndChnIdx, wRet);
		return FALSE;
    }
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "BAS: [CApu2BasChnnl::SetSndObjectPara] BAS.%d m_pcAudioSnd[%d] ResetRSFlag Successed.\n", GetInsID(), bySndChnIdx);

	/*���෢������*/
	SetAudResend(bySndChnIdx);

	/*���ܲ���*/
    u8  abyKeyBuf[MAXLEN_KEY];
    memset(abyKeyBuf, 0, MAXLEN_KEY );
    s32 nKeyLen = 0;
    u8 byEncryptMode = m_tMediaEncrypt.GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
		m_pcAudioSnd[bySndChnIdx].SetEncryptKey( NULL, 0, 0 );
        LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "BAS: [CApu2BasChnnl::SetSndObjectPara] BAS.%d set key to NULL!\n", GetInsID());      
    }
    else if ( CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode )
    {
        if ( CONF_ENCRYPTMODE_DES == byEncryptMode )
        {
            byEncryptMode = DES_ENCRYPT_MODE;
        }
        else
        {
            byEncryptMode = AES_ENCRYPT_MODE;
        }
        m_tMediaEncrypt.GetEncryptKey( abyKeyBuf, &nKeyLen );
		
        m_pcAudioSnd[bySndChnIdx].SetEncryptKey( (s8*)abyKeyBuf, (u16)nKeyLen, byEncryptMode );
    }
    m_pcAudioSnd[bySndChnIdx].SetActivePT(m_atAudBasInfo[byRcvIdx].atEncPayload[bySndIdx].GetRealPayLoad());

	return TRUE;
}

/*====================================================================
������      ��SetRcvObjectPara
����        �����ý��ն������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u8 byChnIdx ͨ������
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasChnnl::SetRcvObjectPara(const u8 byChnIdx)
{
	PTR_NULL_RETURN(m_pcAudioRcv, FALSE);

    u16 wRet = 0;
	s32 byKenLen = 0;

    u8 abyKeyBuf[MAXLEN_KEY];  
    memset(abyKeyBuf, 0, MAXLEN_KEY);

	/*���ü��ܲ���*/
    u8 byEncryptMode = m_tMediaEncrypt.GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
        m_pcAudioRcv[byChnIdx].SetDecryptKey( NULL, 0, 0 );
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::SetRcvObjectPara] BAS.%d Set key to NULL!\n", GetInsID());
    }
    else if ( CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode )
    {
        if ( CONF_ENCRYPTMODE_DES == byEncryptMode )
        {
            byEncryptMode = DES_ENCRYPT_MODE;
        }
        else
        {
            byEncryptMode = AES_ENCRYPT_MODE;
        }
        m_tMediaEncrypt.GetEncryptKey(abyKeyBuf, &byKenLen);
        m_pcAudioRcv[byChnIdx].SetDecryptKey( (s8*)abyKeyBuf, (u16)byKenLen, byEncryptMode );
    }

	/*�����غ���Ϣ*/
    u8 byRealPayload   = m_atAudBasInfo[byChnIdx].tDecPayload.GetRealPayLoad();
    u8 byActivePayload = m_atAudBasInfo[byChnIdx].tDecPayload.GetActivePayload();
    m_pcAudioRcv[byChnIdx].SetActivePT( byActivePayload, byRealPayload );

    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"BAS: [CApu2BasChnnl::SetRcvObjectPara] BAS.%d Mode: %u KeyLen: %u PT: %d AT: %d KeyBuf [%s]\n", 
		GetInsID(), m_tMediaEncrypt.GetEncryptMode(), byKenLen, byRealPayload ,byActivePayload,abyKeyBuf);


	TRSParam tNetRSParam;
    if (m_bIsNeedPrs)
    {
        tNetRSParam.m_wFirstTimeSpan  = m_pcBasCfg->m_tPrsTimeSpan.m_wFirstTimeSpan;
        tNetRSParam.m_wSecondTimeSpan = m_pcBasCfg->m_tPrsTimeSpan.m_wSecondTimeSpan;
        tNetRSParam.m_wThirdTimeSpan  = m_pcBasCfg->m_tPrsTimeSpan.m_wThirdTimeSpan;
        tNetRSParam.m_wRejectTimeSpan = m_pcBasCfg->m_tPrsTimeSpan.m_wRejectTimeSpan;
    }
    else
    {
        tNetRSParam.m_wFirstTimeSpan  = 0;
        tNetRSParam.m_wSecondTimeSpan = 0;
        tNetRSParam.m_wThirdTimeSpan  = 0;
        tNetRSParam.m_wRejectTimeSpan = 0;
    }

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"BAS: [CApu2BasChnnl::SetRcvObjectPara] BAS.%d m_wFirstTimeSpan:%d, m_wSecondTimeSpan:%d, m_wThirdTimeSpan:%d, m_wRejectTimeSpan:%d!\n", 
		GetInsID(), tNetRSParam.m_wFirstTimeSpan, tNetRSParam.m_wSecondTimeSpan,  tNetRSParam.m_wThirdTimeSpan, tNetRSParam.m_wRejectTimeSpan);
	
	/*�ش����*/
	wRet = m_pcAudioRcv[byChnIdx].ResetRSFlag( tNetRSParam, TRUE );
	if(wRet != MEDIANET_NO_ERROR)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::SetRcvObjectPara] BAS.%d ResetRSFlag Failed! REASON: %d!\n", GetInsID(), wRet);
	}
	else
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::SetRcvObjectPara] BAS.%d ResetRSFlag Successed.\n", GetInsID());
	}

	/*��������*/
    wRet = m_pcAudioRcv[byChnIdx].StartRcv();
    if (MEDIANET_NO_ERROR != wRet)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::SetRcvObjectPara] BAS.%d StartRcv Failed! REASON: %d!\n", GetInsID(), wRet);
    }
    else
    {
        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::SetRcvObjectPara] BAS.%d StartRcv Successed.\n", GetInsID());
    }
    return TRUE;
}

/*====================================================================
������      ��SetRcvNetParam
����        �����ý��յ�ַ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u8 byChnIdx
����ֵ˵��  ��BOOL32
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/04/02	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasChnnl::SetRcvNetParam(const u8 byChnIdx)
{
	if (byChnIdx >= MAXNUM_APU2_BASCHN)
	{
		return FALSE;
	}

	TLocalNetParam tLocalNetParam;
	memset(&tLocalNetParam, 0, sizeof(TLocalNetParam));
	//RTCPԴ
	tLocalNetParam.m_dwRtcpBackAddr = m_pcBasCfg->m_dwMcuRcvIp;
	tLocalNetParam.m_wRtcpBackPort = m_pcBasCfg->wRcvStartPort + byChnIdx * MAXNUM_APU2BAS_OUTCHN * PORTSPAN + 2 + 1;
	//RTP����
	tLocalNetParam.m_tLocalNet.m_dwRTPAddr = 0;
    tLocalNetParam.m_tLocalNet.m_wRTPPort = m_pcBasCfg->wRcvStartPort + byChnIdx * MAXNUM_APU2BAS_OUTCHN * PORTSPAN + 2;
	tLocalNetParam.m_tLocalNet.m_wRTCPPort = m_pcBasCfg->wRcvStartPort + byChnIdx * MAXNUM_APU2BAS_OUTCHN * PORTSPAN + 2 + 1;
	
	//������ӡ
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"BAS: [CApu2BasChnnl::SetRcvNetParam] BAS:%d SetRcvNetParam: RtcpBackAddr[%s]  RtcpBackPort[%d]  LocalRtpPort[%d]  LocalPrtcpPort[%d]\n", 
		GetInsID(), StrOfIP(tLocalNetParam.m_dwRtcpBackAddr), tLocalNetParam.m_wRtcpBackPort, 
		tLocalNetParam.m_tLocalNet.m_wRTPPort, tLocalNetParam.m_tLocalNet.m_wRTCPPort);
	
	u16 wRet = m_pcAudioRcv[byChnIdx].SetNetRcvLocalParam(tLocalNetParam);
	if(wRet != MEDIANET_NO_ERROR)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::SetRcvNetParam][error] BAS:%d SetRcvNetParam Failed! Because.%d!\n", GetInsID(), wRet);
		return FALSE;
    }
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"BAS: [CApu2BasChnnl::SetRcvNetParam] BAS.%d SetRcvNetParam Success.\n", GetInsID());

	return TRUE;
}
/*====================================================================
������      ��ProcStartAdaptReq
����        ������������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage* const pcMsg
����ֵ˵��  ��u8
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
void CApu2BasChnnl::ProcStartAdaptReq(CMessage* const pcMsg)
{
	PTR_NULL_VOID(pcMsg);

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	//��Ϣ�����
	u8 wResMsgLen = 0;
	u8 byChnlIdx = 0;
	s8 *lpMsgBody = ReadMsgBody(cServMsg, wResMsgLen, byChnlIdx);
	u16 wMsgLen = 0;

	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		//�������
		TAudAdaptParam atAudAdpParam[MAXNUM_APU2BAS_OUTCHN];
		for (u8 byLoop = 0; byLoop < MAXNUM_APU2BAS_OUTCHN; byLoop++)
		{
			atAudAdpParam[byLoop] = *(TAudAdaptParam* )lpMsgBody;
			SetBasEncInfo(&atAudAdpParam[byLoop], byChnlIdx, byLoop);

			lpMsgBody += sizeof(TAudAdaptParam);
			wMsgLen += sizeof(TAudAdaptParam);
		}
		//�����ش�֧��
		m_bIsNeedPrs = atAudAdpParam[0].IsNeedbyPrs();
	}
	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		//���ܲ���
		m_tMediaEncrypt = *(TMediaEncrypt* )lpMsgBody;

		lpMsgBody += sizeof(TMediaEncrypt);
		wMsgLen += sizeof(TMediaEncrypt);
	}
	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		//�������
		TAudioCapInfo tAudCapInfo = *(TAudioCapInfo* )lpMsgBody;
		SetBasDecInfo(&tAudCapInfo, byChnlIdx);

		lpMsgBody += sizeof(TAudioCapInfo);
		wMsgLen += sizeof(TAudioCapInfo);
	}
	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		//���������
		m_atAudBasInfo[byChnlIdx].bySampleRate = *(u8* )lpMsgBody;

		lpMsgBody += sizeof(u8);
		wMsgLen += sizeof(u8);
	}

	/*��ӡ��Ϣ����*/
	ProcMsgPrint(byChnlIdx);

	u16 wRet = ERR_BAS_NOERROR;
	cServMsg.SetEventId(pcMsg->event + 2);

	switch(GetState())
    {
    case CApu2BasChnStatus::IDLE:     
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
				"BAS: [CApu2BasChnnl::ProcStartAdaptReq][error] BAS.%d STATE-IDLE, StartAdapt Failed!\n", GetInsID());
			wRet = ERR_BAS_CHIDLE;
			break;
		}
	case CApu2BasChnStatus::READY:
		{
			/*��������ͨ��*/
			wRet = OnStartAdaptReq();

			if (ERR_BAS_NOERROR == wRet)
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
					"BAS: [CApu2BasChnnl::ProcStartAdaptReq] BAS.%d StartAdapt Success!\n", GetInsID());
				SetState((u8)CApu2BasChnStatus::RUNNING);
			}
			else 
			{
				break;
			}

			//  [6/8/2013 guodawei] ����ɹ�����ִ��RUNNING�Ĵ���
		}
    case CApu2BasChnStatus::RUNNING:
        {
			/*ͨ��ռ��*/
			wRet = AddBasMember(byChnlIdx);

			if (ERR_BAS_NOERROR == wRet)
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
					"BAS: [CApu2BasChnnl::ProcStartAdaptReq] BAS.%d RUNNING OccupyChnl.%d Success!\n", GetInsID(), byChnlIdx);
				cServMsg.SetEventId(pcMsg->event + 1);
				m_acConfId[byChnlIdx] = cServMsg.GetConfId();			
			}
			break;
        }
    default:
        break;
    }

	SendMsgToMcu(cServMsg, wRet, byChnlIdx);

	return;
}

/*====================================================================
������      ��ProcChgAdaptCmd
����        ���޸��������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage* const pcMsg
����ֵ˵��  ��u8
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
void CApu2BasChnnl::ProcChgAdaptReq(CMessage* const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	//��Ϣ�����
	u8 wResMsgLen = 0;
	u8 byChnlIdx = 0;
	s8 *lpMsgBody = ReadMsgBody(cServMsg, wResMsgLen, byChnlIdx);
	u16 wMsgLen = 0;

	//���Ͷ�����
	u8 abyEncChg[MAXNUM_APU2BAS_OUTCHN];
	u8 byEncChgIdx = 0;

	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		TAudAdaptParam atAudAdpParam[MAXNUM_APU2BAS_OUTCHN];
		for (u8 byLoop = 0; byLoop < MAXNUM_APU2BAS_OUTCHN; byLoop++)
		{
			atAudAdpParam[byLoop] = *(TAudAdaptParam* )lpMsgBody;
			SetBasEncInfo(&atAudAdpParam[byLoop], byChnlIdx, byLoop);

			lpMsgBody += sizeof(TAudAdaptParam);
			wMsgLen += sizeof(TAudAdaptParam);
		}
		//�����ش�֧��
		m_bIsNeedPrs = atAudAdpParam[0].IsNeedbyPrs();
	}
	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		//���ܲ���
		m_tMediaEncrypt = *(TMediaEncrypt* )lpMsgBody;

		lpMsgBody += sizeof(TMediaEncrypt);
		wMsgLen += sizeof(TMediaEncrypt);
	}
	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		//�������
		TAudioCapInfo tAudCapInfo = *(TAudioCapInfo* )lpMsgBody;
		SetBasDecInfo(&tAudCapInfo, byChnlIdx);

		lpMsgBody += sizeof(TAudioCapInfo);
		wMsgLen += sizeof(TAudioCapInfo);
	}
	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		//���������
		m_atAudBasInfo[byChnlIdx].bySampleRate = *(u8* )lpMsgBody;

		lpMsgBody += sizeof(u8);
		wMsgLen += sizeof(u8);
	}
	
	/*��ӡ��Ϣ����*/
	ProcMsgPrint(byChnlIdx);
	
	u16 wRet = ERR_BAS_NOERROR;
	cServMsg.SetEventId(pcMsg->event + 2);

	switch(GetState())
    {
    case CApu2BasChnStatus::IDLE:     
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
				"BAS: [CApu2BasChnnl::ProcChgAdaptReq][error] BAS.%d STATE-IDLE, ChgAdapt Chnl.%d Failed!\n", GetInsID(), byChnlIdx);
			wRet = ERR_BAS_CHIDLE;
			break;
		}
	case CApu2BasChnStatus::READY:
		{	
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
				"BAS: [CApu2BasChnnl::ProcChgAdaptReq][error] BAS.%d STATE-READY, ChgAdapt Chnl.%d Failed!\n", GetInsID(), byChnlIdx);
			wRet = ERR_BAS_CHREADY;
            break;
		}
    case CApu2BasChnStatus::RUNNING:
        {
			/*�޸�ͨ������*/
			wRet = OnChgAdaptReq(byChnlIdx, abyEncChg);

			if (ERR_BAS_NOERROR == wRet)
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
					"BAS: [CApu2BasChnnl::ProcChgAdaptReq] BAS.%d RUNNING ChgAdapt Chnl.%d Success!\n", GetInsID(), byChnlIdx);
				cServMsg.SetEventId(pcMsg->event + 1);
			}
            break;
        }		
    default:
        break;
    }

	SendMsgToMcu(cServMsg, wRet, byChnlIdx);
	return;
}

/*====================================================================
������      ��ProcStopAdaptReq
����        ������ֹͣ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage* const pcMsg
����ֵ˵��  ��u8
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
void CApu2BasChnnl::ProcStopAdaptReq(CMessage* const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	//��Ϣ�����
	u8 wResMsgLen = 0;
	u8 byChnlIdx = 0;
	s8 *lpMsgBody = ReadMsgBody(cServMsg, wResMsgLen, byChnlIdx);
	u16 wMsgLen = 0;

	u16 wRet = ERR_BAS_NOERROR;
	cServMsg.SetEventId(pcMsg->event + 2);

	switch(GetState())
    {
    case CApu2BasChnStatus::IDLE:     
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
				"BAS: [CApu2BasChnnl::ProcStopAdaptReq][error] BAS.%d STATE-IDLE, StopAdapt Chnl.%d Failed!\n", GetInsID(), byChnlIdx);
			wRet = ERR_BAS_CHIDLE;
			break;
		}
	case CApu2BasChnStatus::READY:
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
				"BAS: [CApu2BasChnnl::ProcStopAdaptReq][error] BAS.%d STATE-READY, StopAdapt Chnl.%d Failed!\n", GetInsID(), byChnlIdx);
			wRet = ERR_BAS_CHREADY;
            break;
		}
    case CApu2BasChnStatus::RUNNING:
        {	
			/*�ر�����ͨ��*/
			wRet = RemoveBasMember(byChnlIdx);
			if (ERR_BAS_NOERROR == wRet)
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
					"BAS: [CApu2BasChnnl::ProcStopAdaptReq] BAS.%d RUNNING RemoveChnl.%d Success!\n", GetInsID(), byChnlIdx);
				cServMsg.SetEventId(pcMsg->event + 1);
			}
			else
			{
				break;
			}
			m_acConfId[byChnlIdx].SetNull();

			/*�ر�������*/
			BOOL32 bNoneOccupyChnl = TRUE;
			for (u8 byIdx = 0; byIdx < MAXNUM_APU2_BASCHN; byIdx++)
			{
				if (m_abChnlOccupy[byIdx] == TRUE)
				{
					bNoneOccupyChnl = FALSE;
					break;
				}
			}
			if (bNoneOccupyChnl == TRUE && OnStopAdaptReq())
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
						"BAS: [CApu2BasChnnl::ProcStopAdaptReq] BAS.%d StopAdapt Success!\n", GetInsID());
				SetState((u8)CApu2BasChnStatus::READY);
			}
            break;
        }		
    default:
        break;
    }

	SendMsgToMcu(cServMsg, wRet, byChnlIdx);

	return;
}

/*====================================================================
������      ��OnStartAdaptReq
����        ����ʼ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����void
����ֵ˵��  ��BOOL32
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
u16 CApu2BasChnnl::OnStartAdaptReq(void)
{
	/*��֤״̬��*/
	if((u8)CApu2BasChnStatus::READY != GetState())
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::OnStartAdaptReq][error] BAS.%d not STATE-READY, StartAdapt Failed!\n", GetInsID());
		return ERR_BAS_CHNOTREAD;
	}
	
	/*���Ͽ�������*/
	u16 wRet = m_cBas.StartAdaptor(TRUE);
	if (wRet != (u16)Codec_Success)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::OnStartAdaptReq][error] BAS.%d StartAdapt Failed! Because.%d!\n", GetInsID(), wRet);
		return ERR_BAS_OPMAP;
	} 
	
	return ERR_BAS_NOERROR;
}

/*====================================================================
������      ��OnChgAdaptCmd
����        ���޸��������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u8 byChIdx
����ֵ˵��  ��BOOL32
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
u16 CApu2BasChnnl::OnChgAdaptReq(const u8 byChIdx, u8 *abyEncChg)
{
	/*��֤״̬��*/
	if((u8)CApu2BasChnStatus::RUNNING != GetState())
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::OnStartAdaptReq][error] BAS.%d not STATE-RUNNING, ChgAdapt Failed!\n", GetInsID());
		return ERR_BAS_CHREADY;
	}
	
	/*��֤ͨ��ռ��*/
	if (m_abChnlOccupy[byChIdx] == FALSE)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::OnStartAdaptReq][error] BAS.%d Chnl.%d not Occupied, ChgAdapt Failed!\n", GetInsID(), byChIdx);
		return ERR_BAS_CHRUNING;
	}

	u8 byLoop = 0;
	u16 wRet = Codec_Success;

	/*���ñ������*/
	SetMediaEncParam(byChIdx);
	
	/*���ý������*/
	SetMediaDecParam(byChIdx);

	return ERR_BAS_NOERROR;
}

/*====================================================================
������      ��OnStopAdaptReq
����        ��ֹͣ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u8 byChIdx
����ֵ˵��  ��BOOL32
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasChnnl::OnStopAdaptReq(void)
{
	/*��֤״̬��*/
	if((u8)CApu2BasChnStatus::RUNNING != GetState())
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::OnStopAdaptReq][error] BAS.%d not STATE-RUNNING, StopAdapt Failed!\n", GetInsID());
		return FALSE;
	}
	
	/*�ر�����*/
	u16 wRet = m_cBas.StartAdaptor(FALSE);
	if (wRet != (u16)Codec_Success)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::OnStopAdaptReq][error] BAS.%d StopAdapt Failed! Because.%d!\n", GetInsID(), wRet);
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
������      ��AddBasMember
����        ��ͨ��ռ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u8 byChIdx
����ֵ˵��  ��BOOL32
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
u16 CApu2BasChnnl::AddBasMember(const u8 byChIdx)
{
	if (byChIdx >= MAXNUM_APU2_BASCHN)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::AddBasMember][error] BAS.%d byChIdx.%d Must Below.%d!\n", GetInsID(), byChIdx, MAXNUM_APU2_BASCHN);
		return ERR_BAS_NOENOUGHCHLS;
	}
	
	/*������ռ�õ�ͨ��*/
	if (m_abChnlOccupy[byChIdx] == TRUE)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::AddBasMember][error] BAS.%d byChIdx.%d Is Occupied!\n", GetInsID(), byChIdx);
		return ERR_BAS_CHRUNING;
	}

	u8 byLoop = 0;
	u16 wRet = (u16)Codec_Success;

	/*���ý��ն����ַ*/
	SetRcvNetParam(byChIdx);

	/*���ý��ն���*/
	SetRcvObjectPara(byChIdx);

	/*ռ��ͨ��*/
	wRet = m_cBas.AddChannel(byChIdx);
    if(wRet != (u16)Codec_Success)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::AddBasMember][error] BAS.%d OccupyChnl.%d Failed! Because.%d!\n", GetInsID(), byChIdx, wRet);
        return ERR_BAS_OPMAP;
    }

	/*���ñ������*/
	SetMediaEncParam(byChIdx);

	/*���ý������*/
	SetMediaDecParam(byChIdx);

	/*����ͨ��ռ��*/
	m_abChnlOccupy[byChIdx] = TRUE;

	return ERR_BAS_NOERROR;
}

/*====================================================================
������      ��RemoveBasMember
����        ���Ƴ�����ͨ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u8 byChIdx
����ֵ˵��  ��BOOL32
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
u16 CApu2BasChnnl::RemoveBasMember(const u8 byChIdx)
{
	if (byChIdx >= MAXNUM_APU2_BASCHN)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::RemoveBasMember][error] BAS.%d byChIdx.%d Must Below.%d!\n", 
			GetInsID(), byChIdx, MAXNUM_APU2_BASCHN);
		return ERR_BAS_NOENOUGHCHLS;
	}

	/*����δռ�õ�ͨ��*/
	if (m_abChnlOccupy[byChIdx] == FALSE)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::RemoveBasMember][error] BAS.%d byChIdx.%d not Occupied!\n", 
			GetInsID(), byChIdx);
		return ERR_BAS_CHREADY;
	}

	u16 wRet = (u16)Codec_Success;
	u8 byLoop = 0;

	/*ֹͣ����*/
	wRet = m_pcAudioRcv[byChIdx].StopRcv();
    if(wRet != MEDIANET_NO_ERROR)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::RemoveBasMember][error] BAS.%d StopRcv Chnl.%d Failed! Because.%d!\n", 
			GetInsID(), byChIdx, wRet);
        return ERR_BAS_OPMAP;
    }
    
	/*�Ƴ�ͨ��*/
	wRet = m_cBas.DelChannel(byChIdx);
    if ((u16)Codec_Success != wRet)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::RemoveBasMember][error] BAS.%d DelChannel.%d Failed! Because.%d!\n", 
			GetInsID(), byChIdx, wRet);
        return ERR_BAS_OPMAP;
    }
	
	/*������Ͳ���*/
	for (byLoop = 0; byLoop < MAXNUM_APU2BAS_OUTCHN; byLoop++)
	{
		wRet = m_pcAudioSnd[byChIdx * MAXNUM_APU2BAS_OUTCHN + byLoop].RemoveNetSndLocalParam();
		if (MEDIANET_NO_ERROR != wRet)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
				"BAS: [CApu2BasChnnl::RemoveBasMember][error] BAS.%d RemoveNetSndLocalParam Chnl.%d Failed! Because.%d!\n",
				GetInsID() , byChIdx * MAXNUM_APU2BAS_OUTCHN + byLoop, wRet);
			return ERR_BAS_OPMAP;
		}
	}

	/*���ͨ��ռ��*/
	m_abChnlOccupy[byChIdx] = FALSE;
	return ERR_BAS_NOERROR;

}

/*====================================================================
������      ��GetAudioMode
����        ����ȡ��Ƶģʽ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u8 byAudioType ��Ƶ����
����ֵ˵��  ��u8
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
u8 CApu2BasChnnl::GetAudioMode(const u8 byAudioType, u8 byAudioTrackNum)
{
    u8 byAudMode = 0;
    switch (byAudioType)
    {
    case MEDIA_TYPE_G7221C://G.722.1.C Polycom Siren14
        byAudMode = AUDIO_MODE_G7221;//������
        break;
    case MEDIA_TYPE_PCMA://G.711a
        byAudMode = AUDIO_MODE_PCMA;
        break;
    case MEDIA_TYPE_PCMU://G.711u
        byAudMode = AUDIO_MODE_PCMU;
        break;
    case MEDIA_TYPE_G722://G.722
        byAudMode = AUDIO_MODE_G722;
        break;
    case MEDIA_TYPE_G728://G.728
        byAudMode = AUDIO_MODE_G728;
        break;
    case MEDIA_TYPE_G729://G.729
        byAudMode = AUDIO_MODE_G729;
        break;
    case MEDIA_TYPE_G719://G.719
        byAudMode = AUDIO_MODE_G719;
        break;
    case MEDIA_TYPE_MP3://MP3
        byAudMode = AUDIO_MODE_MP3;
        break;
    case MEDIA_TYPE_AACLC://MPEG4 AAL-LC
		if (1 == byAudioTrackNum)
		{
			byAudMode = AUDIO_MODE_AACLC_32_M;
		}
        else if (2 == byAudioTrackNum)
		{
			byAudMode = AUDIO_MODE_AACLC_32;
		}
        break;
    case MEDIA_TYPE_AACLD://MPEG4 AAL-LD
		if (1 == byAudioTrackNum)
		{
			byAudMode = AUDIO_MODE_AACLD_32_M;
		}
		else if (2 == byAudioTrackNum)
		{
			byAudMode = AUDIO_MODE_AACLD_32;
		}
        break;
	case MEDIA_TYPE_NULL:
		break;
    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::GetAudioMode][error] BAS.%d GetAudioMode MediaType.%d not surportted!!\n", GetInsID(), byAudioType);
        return 0;
    }
    return byAudMode;
}

/*====================================================================
������      ��Stop
����        ��ֹͣͨ������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasChnnl::Stop(const u8 byMode)
{
	if (MODE_BOTH != byMode && MODE_AUDIO != byMode)
	{
		return FALSE;
	}

    switch(GetState())
    {
	case CApu2BasChnStatus::RUNNING:
		{
			OnStopAdaptReq();
			SetState((u8)CApu2BasChnStatus::IDLE);
			break;
		}		
	default:
		SetState((u8)CApu2BasChnStatus::IDLE);
		break;
    }

	VariablesClear(1);

	return TRUE;
}

/*====================================================================
������      ��GetMediaSnd
����        ����ȡ���Ͷ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u16 wIndex ͨ������
����ֵ˵��  ��CKdvMediaSnd*
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
CKdvMediaSnd* CApu2BasChnnl::GetMediaSnd(const u32 wIndex)
{
	PTR_NULL_RETURN(m_pcBasCfg, NULL);
	PTR_NULL_RETURN(m_pcAudioSnd, NULL);
	return &m_pcAudioSnd[wIndex];
}

/*====================================================================
������      ��GetKdvBas
����        ����ȡý����ƶ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��CKdvAudAdaptor*
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/04/01	2.0			������		  ����
====================================================================*/
CKdvAudAdaptor* CApu2BasChnnl::GetKdvBas() 
{
	return &m_cBas;
}

/*====================================================================
������      ��TransAudType
����        ��ת����Ƶ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��CKdvMixer*
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29  2.0         ������        ����
====================================================================*/
u8 CApu2BasChnnl::TransSimRate(const u8 bySimpleRate)
{
	u8 byTransRate = 0;
	switch (bySimpleRate)
	{
		case AAC_SAMPLE_FRQ_8:
			byTransRate = SAMPLE_RATE8000;
			break;
		case AAC_SAMPLE_FRQ_11:
			byTransRate = SAMPLE_RATE11025;
			break;
		case AAC_SAMPLE_FRQ_12:
			byTransRate = SAMPLE_RATE12000;
			break;
		case AAC_SAMPLE_FRQ_16:
			byTransRate = SAMPLE_RATE16000;
			break;
		case AAC_SAMPLE_FRQ_22:
			byTransRate = SAMPLE_RATE22050;
			break;
		case AAC_SAMPLE_FRQ_24:
			byTransRate = SAMPLE_RATE24000;
			break;
		case AAC_SAMPLE_FRQ_32:
			byTransRate = SAMPLE_RATE32000;
			break;
		case AAC_SAMPLE_FRQ_44:
			byTransRate = SAMPLE_RATE44100;
			break;
		case AAC_SAMPLE_FRQ_48:
			byTransRate = SAMPLE_RATE48000;
			break;
		case AAC_SAMPLE_FRQ_64:
			byTransRate = SAMPLE_RATE64000;
			break;
		case AAC_SAMPLE_FRQ_88:
			byTransRate = SAMPLE_RATE88200;
			break;
		case AAC_SAMPLE_FRQ_96:
			byTransRate = SAMPLE_RATE96000;
			break;
		default:
			byTransRate = 0;
			break;
	}
	return byTransRate;
}

/*====================================================================
������      : Init
����        ��Apu2ͨ����ʼ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasChnnl::Init()
{  	
	//ͨ������
	u16 wChnIdx = CEqpChnnlBase::GetChnIdx();
	if (INVALID_CHNLIDX == wChnIdx)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "BAS: [CApu2BasChnnl::Init][error] BAS.%d GetChnIdx Failed!\n", GetInsID());
		return FALSE;
	}

	//��֤�������
	PTR_NULL_RETURN(m_pcBasCfg, FALSE);
	PTR_NULL_RETURN(m_pcAudioRcv, FALSE);
	PTR_NULL_RETURN(m_pcAudioSnd, FALSE);

	//��������������
	u16 wRet = m_cBas.Create(en_APU2_Board);
	if (wRet != (u16)Codec_Success)
    {
		printf("ERROR: [Init] BAS.%d Created Failed! REASON:%d!\n", GetInsID(), wRet);
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::Init][error] BAS.%d Created Failed! REASON:%d!\n", GetInsID(), wRet);
        return FALSE;
    }
	printf("SUCCESS: [Init] BAS.%d Created Success!\n", GetInsID());
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"BAS: [CApu2BasChnnl::Init] BAS.%d Created Success!\n", GetInsID());


	//��Ƶ����ص�
	wRet = m_cBas.SetAudEncDataCallBack(BasEncFrameCallBack, this);
    if (wRet != (u16)Codec_Success)
    {
        m_cBas.Destroy();
		printf("ERROR: [Init] BAS.%d SetAudEncDataCallBack Failed! REASON:%d!\n", GetInsID(), wRet);
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::Init][error] BAS.%d SetAudEncDataCallBack Failed! REASON:%d!\n", GetInsID(), wRet);
        return FALSE;
    }
	printf("SUCCESS: [Init] BAS.%d SetAudEncDataCallBack Success!\n", GetInsID());
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"BAS: [CApu2BasChnnl::Init] BAS.%d SetAudEncDataCallBack Success!\n", GetInsID());


	//�����շ�����
	u8 byIdx = 0;
	for ( ; byIdx < MAXNUM_APU2_BASCHN; byIdx++)
	{
		if (NULL != m_pcAudioRcv && NULL != m_pcRcvCallBack)
		{
			//����BAS���ն���
			m_pcRcvCallBack[byIdx].m_pcBasChn   = this;
			m_pcRcvCallBack[byIdx].m_wChnId     = byIdx;
			wRet = m_pcAudioRcv[byIdx].Create(MAX_AUDIO_FRAME_SIZE, BasNetRcvCallBack, (u32)&m_pcRcvCallBack[byIdx]);
			if(wRet != MEDIANET_NO_ERROR)
			{
				printf("ERROR: [Init] BAS.%d m_pcAudioRcv.[%d] Created Failed! REASON:%d!\n", GetInsID(), byIdx, wRet);
				LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
					"BAS: [CApu2BasChnnl::Init][error] BAS.%d m_pcAudioRcv.[%d] Created Failed! REASON:%d!\n", GetInsID(), byIdx, wRet);
				return FALSE;
			}
			else
			{
				//ÿ��BASͨ����Ӧ3·����
				for (u8 byLoop = 0; byLoop < MAXNUM_APU2BAS_OUTCHN; byLoop++)
				{
					if (NULL != m_pcAudioSnd)
					{
						u8 bySndIdx = byIdx * MAXNUM_APU2BAS_OUTCHN + byLoop;
						wRet = m_pcAudioSnd[bySndIdx].Create(MAX_AUDIO_FRAME_SIZE, 64<<10, 25, MEDIA_TYPE_G7221C);
						if(wRet != MEDIANET_NO_ERROR)
						{
							printf("ERROR: [Init] BAS.%d m_pcAudioSnd.[%d] Created Failed! REASON:%d\n", GetInsID(), bySndIdx, wRet);
							LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
								"BAS: [CApu2BasChnnl::Init][error] BAS.%d m_pcAudioSnd.[%d] Created Failed! REASON:%d\n", 
								GetInsID(), bySndIdx, wRet);
							return FALSE;
						}
					}
				}
			}
		}
	}

    return TRUE;
}

/*====================================================================
������      ��Print
����        ��Apu2ͨ����ӡ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
void CApu2BasChnnl::Print()
{
	StaticLog("\nBas : %d", GetInsID());
	
	/*ͨ��״̬*/
	switch (GetState())
	{
	case CApu2BasChnStatus::IDLE:
		StaticLog("\n\t BAS's STATE	: %s", "IDLE");
		break;
	case CApu2BasChnStatus::READY:
		StaticLog("\n\t BAS's STATE	: %s", "READY");
		break;
	case CApu2BasChnStatus::RUNNING:
		StaticLog("\n\t BAS's STATE	: %s", "RUNNING");
		break;
	default:
		StaticLog("\n\t BAS's STATE	: %s.%d", "unexpected state", GetState());
		break;
	}
	
	u8 byLoop = 0;
	u8  abyKeyBuf[MAXLEN_KEY] = {0};
	s32 nKeyLen = 0;
	/*������Ϣ*/
	m_tMediaEncrypt.GetEncryptKey(abyKeyBuf, &nKeyLen);	
	StaticLog("\n\t MediaEncrypt	: Mode[%u]  Key[%s]", 
		m_tMediaEncrypt.GetEncryptMode(), abyKeyBuf);

	/*�����ش�*/
	StaticLog("\n\t IsNeedPRS	: %d\n", m_byQualityLvl);

	/*ͨ��ռ��*/
	for (byLoop = 0; byLoop < MAXNUM_APU2_BASCHN; byLoop++)
	{
		StaticLog("\t CHNL[%d]	: %s\n", byLoop, m_abChnlOccupy[byLoop] == TRUE ? "RUNNING" : "READY");
		if (m_abChnlOccupy[byLoop] == TRUE)
		{
			/*��������*/
			StaticLog("\t\t	: ", byLoop);
			m_acConfId[byLoop].Print();
		}
	}

	StaticLog("\n");
    return;
}

/*====================================================================
������      : PrintChnlInfo
����        ��ͨ����Ϣ��ӡ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/02  2.0         ������          ����
====================================================================*/
void CApu2BasChnnl::PrintChnlInfo()
{
	u8 byIdx = 0;
	
	TKdvRcvStatus tKdvRcvStatus;
	memset(&tKdvRcvStatus, 0, sizeof(tKdvRcvStatus));
	TNetSndParam tNetSndParam;
	memset(&tNetSndParam, 0, sizeof(tNetSndParam));
	
	//ͨ����Ϣ��ӡ
	for (byIdx = 0; byIdx < MAXNUM_APU2_BASCHN; byIdx++)
	{
		if (FALSE == m_abChnlOccupy[byIdx])
		{
			continue;
		}
		if (m_pcAudioRcv != NULL)
		{
			//����ͨ��
			m_pcAudioRcv[byIdx].GetStatus(tKdvRcvStatus);

			StaticLog("\nCHANNEL's ID[%d] : <Rcv> RTP-RcvPort[%d]  RTCP-RcvPort[%d]  RTCP-BackAddr[%s]  RTCP-BackPort[%d] \n", 
				byIdx, tKdvRcvStatus.m_tRcvAddr.m_tLocalNet.m_wRTPPort, tKdvRcvStatus.m_tRcvAddr.m_tLocalNet.m_wRTCPPort, 
				StrOfIP(tKdvRcvStatus.m_tRcvAddr.m_dwRtcpBackAddr), tKdvRcvStatus.m_tRcvAddr.m_wRtcpBackPort);
			StaticLog("\t  : <Rcv> AudType[%d]  SodChnlNum[%d]  ActPayload[%d]  RelPayload[%d]  SamRate[%d] \n",
				m_atAudBasInfo[byIdx].byDecAudType, m_atAudBasInfo[byIdx].byDecSoundChnlNum, 
				m_atAudBasInfo[byIdx].tDecPayload.GetActivePayload(), m_atAudBasInfo[byIdx].tDecPayload.GetRealPayLoad(),
				m_atAudBasInfo[byIdx].bySampleRate);

			for (u8 byLoop = 0; byLoop < MAXNUM_APU2BAS_OUTCHN; byLoop++)
			{
				if (m_pcAudioSnd != NULL)
				{
					m_pcAudioSnd[byIdx * MAXNUM_APU2BAS_OUTCHN + byLoop].GetNetSndParam(&tNetSndParam);
					StaticLog("\t  : <Snd-%d> RTP-SndPort[%d]  RTCP-SndPort[%d]  RemoteAddr[%s]  RemotePort[%d] \n", 
						byLoop, tNetSndParam.m_tLocalNet.m_wRTPPort, tNetSndParam.m_tLocalNet.m_wRTCPPort, 
						StrOfIP(tNetSndParam.m_tRemoteNet[0].m_dwRTPAddr), tNetSndParam.m_tRemoteNet[0].m_wRTPPort);
					StaticLog("\t  : <Snd>   AudType[%d]  SodChnlNum[%d]  ActPayload[%d]  RelPayload[%d]\n",
						m_atAudBasInfo[byIdx].abyEncAudType[byLoop], m_atAudBasInfo[byIdx].abyEncSoundChnlNum[byLoop], 
						m_atAudBasInfo[byIdx].atEncPayload[byLoop].GetActivePayload(), 
					m_atAudBasInfo[byIdx].atEncPayload[byLoop].GetRealPayLoad());
				}
			}
		}
	}

	return;
}

/*====================================================================
������      ��InitChnlInfo
����        ����ʼ��ͨ���������Ϣ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/02  2.0         ������          ����
====================================================================*/
void CApu2BasChnnl::InitChnlInfo(tAudBasInfo &tAudChnlInfo)
{
	for (u8 byLoop = 0; byLoop < MAXNUM_APU2_BASCHN; byLoop++)
	{
		tAudChnlInfo.byDecAudType = MEDIA_TYPE_NULL;
		tAudChnlInfo.byDecSoundChnlNum = 0;
		memset(&tAudChnlInfo.tDecPayload, 0, sizeof(tAudChnlInfo.tDecPayload));
		u8 byIdx = 0;
		for (byIdx = 0; byIdx < MAXNUM_APU2BAS_OUTCHN; byIdx++)
		{
			tAudChnlInfo.abyEncAudType[byIdx] = MEDIA_TYPE_NULL;
			tAudChnlInfo.abyEncSoundChnlNum[byIdx] = 0;
			memset(&tAudChnlInfo.atEncPayload[byIdx], 0, sizeof(tAudChnlInfo.atEncPayload[byIdx]));
		}
		tAudChnlInfo.bySampleRate = 0;
	}
	return;
}

/*====================================================================
������      ��GetChnlInfo
����        ����ȡͨ���������Ϣ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
tAudBasInfo* CApu2BasChnnl::GetChnlInfo(const u8 byIdx)
{
	return &m_atAudBasInfo[byIdx];
}

/*====================================================================
������      ��SetBasEncInfo
����        ������ͨ��������Ϣ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasChnnl::SetBasEncInfo(TAudAdaptParam *pAptParam, u8 byRcvIdx, u8 bySndIdx)
{
	m_atAudBasInfo[byRcvIdx].abyEncAudType[bySndIdx] = pAptParam->GetAudCodeType();
	m_atAudBasInfo[byRcvIdx].abyEncSoundChnlNum[bySndIdx] = pAptParam->GetTrackNum();
	m_atAudBasInfo[byRcvIdx].atEncPayload[bySndIdx].SetActivePayload(pAptParam->GetAudActiveType());
	m_atAudBasInfo[byRcvIdx].atEncPayload[bySndIdx].SetRealPayLoad(pAptParam->GetAudCodeType());
	return TRUE;
}

/*====================================================================
������      ��SetBasDecInfo
����        ������ͨ��������Ϣ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasChnnl::SetBasDecInfo(TAudioCapInfo *pAudCapInfo, u8 byRcvIdx)
{
	m_atAudBasInfo[byRcvIdx].byDecAudType = pAudCapInfo->GetAudioMediaType();
	m_atAudBasInfo[byRcvIdx].byDecSoundChnlNum = pAudCapInfo->GetAudioTrackNum();
	m_atAudBasInfo[byRcvIdx].tDecPayload.SetActivePayload(pAudCapInfo->GetActivePayLoad());
	m_atAudBasInfo[byRcvIdx].tDecPayload.SetRealPayLoad(pAudCapInfo->GetAudioMediaType());
	return TRUE;
}

/*====================================================================
������      ��GetConfId
����        ����ȡ�����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
CConfId CApu2BasChnnl::GetConfId(const u16 wIndex)
{
	return m_acConfId[wIndex];
}

/*====================================================================
������      ��GetChnlOccupy
����        ����ȡͨ��ռ�����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasChnnl::GetChnlOccupy(u8 byChIdx)
{
	return m_abChnlOccupy[byChIdx];
}

/*====================================================================
������      ��CreateAudioRcv
����        ��������Ƶ���ն���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
void CApu2BasChnnl::CreateAudioRcv(u16 wNum)
{
	if (NULL != m_pcAudioRcv) 
	{
		return;
	}
	m_pcAudioRcv = new CKdvMediaRcv[wNum];
	PTR_NULL_VOID(m_pcAudioRcv);

	if (NULL != m_pcRcvCallBack)
	{
		return;
	}
	m_pcRcvCallBack = new CBasRcvCallBack[wNum];
	if (NULL == m_pcRcvCallBack)
	{
		SAFE_DELETE_ARRAY(m_pcAudioRcv);
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "BAS: [CreateAudioRcv][error] Create CRcvCallBack Failed!\n");
		return;
	}
}
/*====================================================================
������      ��CreateAydioSnd
����        ��������Ƶ���Ͷ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
void CApu2BasChnnl::CreateAydioSnd(u16 wNum)
{
	if (NULL != m_pcAudioSnd)
	{
		return;
	}
	m_pcAudioSnd = new CKdvMediaSnd[wNum];
	if (NULL == m_pcAudioSnd)
	{
		SAFE_DELETE_ARRAY(m_pcAudioRcv);
		SAFE_DELETE_ARRAY(m_pcRcvCallBack);
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "BAS: [CreateAydioSnd][error] Create CKdvMediaSnd Failed!\n");
		return;
	}
}
/*====================================================================
������      ��SetMediaEncParam
����        �����õײ�������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
void CApu2BasChnnl::SetMediaEncParam(u8 byChIdx)
{
	TKdvAudioAdaptorTypeParam atKdvAdpParam[MAXNUM_APU2BAS_OUTCHN];
	TAudBasInfo *ptAudBasInfo = GetChnlInfo(byChIdx);
	for (u8 byLoop = 0; byLoop < MAXNUM_APU2BAS_OUTCHN; byLoop++ )
	{
		if (MEDIA_TYPE_NULL == ptAudBasInfo->abyEncAudType[byLoop])
		{
			continue;
		}
		
		SetSndObjectPara(m_pcBasCfg->m_dwMcuRcvIp, 
			m_pcBasCfg->m_wMcuRcvStartPort + (byChIdx * MAXNUM_APU2BAS_OUTCHN + byLoop) * PORTSPAN + 2, 
			byChIdx * MAXNUM_APU2BAS_OUTCHN + byLoop);
		
		atKdvAdpParam[byLoop].dwAudType = ptAudBasInfo->abyEncAudType[byLoop];
		atKdvAdpParam[byLoop].dwAudMode = GetAudioMode(ptAudBasInfo->abyEncAudType[byLoop], ptAudBasInfo->abyEncSoundChnlNum[byLoop]);
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::SetMediaEncParam] BAS.%d SetAudEncParam: byChIdx:[%d][%d] dwAudMode[%d] dwAudType[%d]\n", 
			GetInsID(), byChIdx, byLoop, atKdvAdpParam[byLoop].dwAudMode, atKdvAdpParam[byLoop].dwAudType);
		
		u16 wRet = m_cBas.SetAudEncParam(byChIdx, byLoop, &atKdvAdpParam[byLoop]);
		if (wRet != (u16)Codec_Success)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
				"BAS: [CApu2BasChnnl::SetMediaEncParam][error] BAS.%d Chnl.%d SetAudEncParam.%d Failed! Because.%d!\n", 
				GetInsID(), byChIdx, byLoop, wRet);
		}
	}
	return;
}

/*====================================================================
������      ��SetMediaDecParam
����        �����õײ�������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
void CApu2BasChnnl::SetMediaDecParam(u8 byChIdx)
{
	TAudBasInfo *ptAudBasInfo = GetChnlInfo(byChIdx);
	if (ptAudBasInfo->byDecAudType == MEDIA_TYPE_AACLC || 
		ptAudBasInfo->byDecAudType == MEDIA_TYPE_AACLD)
	{
		TAudioDecParam tAudDecParam;
		tAudDecParam.m_dwMediaType = ptAudBasInfo->byDecAudType;
		tAudDecParam.m_dwChannelNum = ptAudBasInfo->byDecSoundChnlNum;
		tAudDecParam.m_dwSamRate = TransSimRate(ptAudBasInfo->bySampleRate);

		u16 wRet = m_cBas.SetAudioDecParam(byChIdx, tAudDecParam);
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::SetMediaDecParam] BAS.%d SetAudioDecParam: byChIdx:[%d] m_dwMediaType[%d] m_dwChannelNum[%d] m_dwSamRate[%d]\n", 
			GetInsID(), byChIdx, tAudDecParam.m_dwMediaType, tAudDecParam.m_dwChannelNum, tAudDecParam.m_dwSamRate);
		
		if (wRet != (u16)Codec_Success)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
				"BAS: [CApu2BasChnnl::SetMediaDecParam][error] BAS.%d SetAudioDecParam.%d Failed! Because.%d!\n", 
				GetInsID(), byChIdx, wRet);
		}
	}
	return;
}

/*====================================================================
������      ��SetAudResend
����        �����õײ����෢�Ͳ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
void CApu2BasChnnl::SetAudResend(u8 bySndChlIdx)
{
	/*���෢��*/
	s32 nSendTimes = 0;
	s32 nSendSpan  = 0;
	switch (m_byQualityLvl)
	{
	case 0:
		nSendTimes = 0;
		nSendSpan = 0;
		break;
	case 1:
		nSendTimes = 2;
		nSendSpan= 1;
		break;
	default:
		nSendTimes = 1;
		nSendSpan = 1;
		break;
	}

	u16 wRet = m_pcAudioSnd[bySndChlIdx].SetAudioResend(nSendTimes, nSendSpan);
	if(wRet != MEDIANET_NO_ERROR)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::SetSndObjectPara][error] BAS.%d m_pcAudioSnd[%d] set AudioResend failed! REASON: %d!\n", GetInsID(), bySndChlIdx, wRet);
		return;
	}
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"BAS: [CApu2BasChnnl::SetSndObjectPara] BAS.%d m_pcAudioSnd[%d] SetAudioResend Successed.\n", GetInsID(), bySndChlIdx);

}

/*====================================================================
������      ��SendMsgToMcu
����        ��������Ϣ��֯
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/06/08	2.0			������		  ����
====================================================================*/
void CApu2BasChnnl::SendMsgToMcu(CServMsg &cServMsg, u16 wRet, u8 byChIdx)
{
	TEqp tEqp;
	tEqp.SetMcuEqp(LOCAL_MCUID, m_pcBasCfg->byEqpId, m_pcBasCfg->byEqpType);
	cServMsg.SetMsgBody((u8* ) &tEqp, sizeof(TEqp));
	cServMsg.SetChnIndex(byChIdx);
	cServMsg.SetErrorCode(wRet);
	PostMsgToMcu(cServMsg);
}

/*====================================================================
������      ��ReadMsgBody
����        ����ȡ��Ϣ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/06/08	2.0			������		  ����
====================================================================*/
s8* CApu2BasChnnl::ReadMsgBody(CServMsg &cServMsg,  u8 &wMsgLen, u8 &byChIdx)
{
	s8 *lpMsgBody = (s8 *)cServMsg.GetMsgBody();
	wMsgLen = cServMsg.GetMsgBodyLen() - sizeof(u8);
	byChIdx = *(u8 *)(lpMsgBody + wMsgLen);

	return lpMsgBody;
}

/*====================================================================
������      ��VariablesClear
����        ���������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/06/08	2.0			������		  ����
====================================================================*/
void CApu2BasChnnl::VariablesClear(u8 byClearLevel)
{
	switch (byClearLevel)
	{
	case 0:
		memset(&m_tMediaEncrypt, 0, sizeof(m_tMediaEncrypt));
		break;
	default:
		break;
	}
	memset(m_abChnlOccupy, FALSE, sizeof(m_abChnlOccupy));
	for (u8 byLoop = 0; byLoop < MAXNUM_APU2_BASCHN; byLoop++)
	{
		m_acConfId[byLoop].SetNull();
		InitChnlInfo(m_atAudBasInfo[byLoop]);
	}
}

/*====================================================================
������      ��ProcMsgPrint
����        ����Ϣ�����ӡ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/06/08	2.0			������		  ����
====================================================================*/
void CApu2BasChnnl::ProcMsgPrint(u8 byChIdx)
{
	/*���ܲ���*/
	u8 abyKeyBuf[MAXLEN_KEY] = {0};
	s32 nKeyLen = 0;
	m_tMediaEncrypt.GetEncryptKey(abyKeyBuf, &nKeyLen);

	/*��ȡ��Ϣ*/
	tAudBasInfo *ptAudBasInfo = GetChnlInfo(byChIdx);

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"\n*************** BAS.%d Ready To Srt/Chg Adapt With Param As Follows ***************\n", GetInsID());
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		" m_tMediaEncrypt: Mode[%u]  Key[%s] \n", m_tMediaEncrypt.GetEncryptMode(), abyKeyBuf);
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		" m_bIsNeedPrs[%d]", GetInsID(), m_bIsNeedPrs);
	
	//������Ϣ
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,
		"\n CHLIDX[%d]: DecAudType[%d]  DecSodChnlNum[%d]  DecActivePayload[%d]  DecSamRate[%d]", 
		byChIdx, m_atAudBasInfo[byChIdx].byDecAudType, m_atAudBasInfo[byChIdx].byDecSoundChnlNum, 
		m_atAudBasInfo[byChIdx].tDecPayload.GetActivePayload(), m_atAudBasInfo[byChIdx].bySampleRate);
	
	//������Ϣ
	for (u8 byLoop = 0; byLoop < MAXNUM_APU2BAS_OUTCHN; byLoop++)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,
			"\n - OUTIDX[%d]: EncAudType[%d]  EncSodChnlNum[%d]  EncActivePayload[%d]  EncRealPayLoad[%d]", 
			byLoop, m_atAudBasInfo[byChIdx].abyEncAudType[byLoop], m_atAudBasInfo[byChIdx].abyEncSoundChnlNum[byLoop], 
			m_atAudBasInfo[byChIdx].atEncPayload[byLoop].GetActivePayload(), m_atAudBasInfo[byChIdx].atEncPayload[byLoop].GetRealPayLoad());
	}
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"\n*******************************************************************************\n", GetInsID());

}
	
/*====================================================================
������      : CApu2BasChnStatus
����        ��CApu2BasChnStatus����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
CApu2BasChnStatus::CApu2BasChnStatus()
{

}


/*====================================================================
������      ��CApu2BasChnStatus
����        ��CApu2BasChnStatus����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
CApu2BasChnStatus::~CApu2BasChnStatus()
{

}



