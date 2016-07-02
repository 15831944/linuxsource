/*****************************************************************************
   ģ����      : ������
   �ļ���      : audbasinst.cpp
   ����ʱ��    : 2013�� 9�� 12��
   ʵ�ֹ���    : 
   ����        : �ܾ���
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   20130912		1.0			�ܾ���		����
   
******************************************************************************/      
#ifdef _8KI_

#include "audbasinst.h"
#include "audmixinst.h"
#include "evmcueqp.h"
#include "boardagent.h"
#include "mcuver.h"

C8KIAudBasApp g_c8KIAudBasApp;

static void BasEncFrameCallBack(u32 dwChID, u32 dwEncID,  PFRAMEHDR pFrmHdr, void* pContext)
{
	/*ý��ͨ����Ϣ*/
	//u8 byRcvIdx = *pdwChID / AUDIO_ADAPTOR_MAX_ENCNUM;
	//u8 bySndIdx = *pdwChID % AUDIO_ADAPTOR_MAX_ENCNUM;
	C8KIAudBasInst *pIns = (C8KIAudBasInst*)pContext;

	if( pIns->GetInsID() - 1 != (u8)dwChID )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, 
			"[BasEncFrameCallBack] baschlid.%d != paramchlid.%d \n", 
			pIns->GetInsID() - 1, dwChID );
		return;
	}

	//255�������Ͳ�����
	if( pIns->GetEncAudType( (u8)dwEncID ) == MEDIA_TYPE_NULL)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[BasEncFrameCallBack] mediatype null\n" );
		return;
	}

	/*����Ĭ����Ƶ����*/
	if (pFrmHdr->m_byMediaType == MEDIA_TYPE_AACLC)
    {
        pFrmHdr->m_byAudioMode = AUDIO_MODE_AACLC_32_M;
    }
    else if (pFrmHdr->m_byMediaType == MEDIA_TYPE_AACLD)
    {
        pFrmHdr->m_byAudioMode = AUDIO_MODE_AACLD_32_M;
    }
		
	CKdvMediaSnd *pcMediaSnd = pIns->GetMediaSndObj( (u8)dwEncID );
    if( NULL == pcMediaSnd )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[BasEncFrameCallBack] pcMediaSnd null\n" );
		return;
	}
	
	/*��ʼ����*/
    u16 wRet = pcMediaSnd->Send(pFrmHdr);
    if (wRet != MEDIANET_NO_ERROR)
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[BasEncFrameCallBack] BAS.%d  SndChnId.%d ERROR.%d\n",pIns->GetInsID()-1, dwEncID, wRet );
    }

    return;
}

static void BasNetRcvCallBack(PFRAMEHDR pFrmHdr, u32 dwContext)
{
	/*�����ص���Ϣ*/
	//CBasRcvCallBack *pcCallBack = (CBasRcvCallBack*)dwContext;
	C8KIAudBasInst *pIns = (C8KIAudBasInst*)dwContext;


	/*����Ĭ����Ƶ����*/
	if (pFrmHdr->m_byMediaType == MEDIA_TYPE_AACLC)
    {
        pFrmHdr->m_byAudioMode = AUDIO_MODE_AACLC_32_M;
    }
    else if (pFrmHdr->m_byMediaType == MEDIA_TYPE_AACLD)
    {
        pFrmHdr->m_byAudioMode = AUDIO_MODE_AACLD_32_M;
    }
	
	/*������Ƶ��������*/
    CAudioAdaptor* pcKdvBas = pIns->GetAdaptObj();

	if( NULL == pcKdvBas )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[MixNetRcvCallBack] pcKdvBas is null\n" );
		return;
	}

	/*���ý�����Ϣ*/
	u16 wRet = pcKdvBas->SetAudDecData( pIns->GetInsID()-1, pFrmHdr );
    if (wRet != (u16)Codec_Success)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[MixNetRcvCallBack] SetAudDecData failed because.%d ChnId.%d datasize.%d!\n",
												wRet, pIns->GetInsID()-1, pFrmHdr->m_dwDataSize );
	}
}

C8KIAudBasInst::C8KIAudBasInst()
{	
	m_bIsNeedPrs = 0;
	m_tMediaEncrypt.Reset();
	m_tAudioCapInfo.Clear();
	m_bySampleRate = 0;
}

C8KIAudBasInst::~C8KIAudBasInst()
{
	m_bIsNeedPrs = 0;
	m_tMediaEncrypt.Reset();
	m_tAudioCapInfo.Clear();
	m_bySampleRate = 0;
}

/*====================================================================
	����  : SendMsgToMcu
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
CAudioAdaptor* C8KIAudBasInst::GetAdaptObj( void )
{
	u8 byChlIdx = GetInsID() - 1;
	if( byChlIdx >= 0 && byChlIdx < MAXNUM_8KI_AUD_BAS )
	{
		return &m_cBas;
	}
	return NULL;
}

/*====================================================================
	����  : SendMsgToMcu
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
CKdvMediaSnd* C8KIAudBasInst::GetMediaSndObj( u8 byEndIdx )
{
	if( byEndIdx >= MAXNUM_8KIAUDBAS_ENCNUM )
		return NULL;

	return &m_cAudioSnd[byEndIdx];
}

/*====================================================================
	����  : SendMsgToMcu
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
u8 C8KIAudBasInst::GetEncAudType( u8 byEndIdx )
{
	if( byEndIdx >= MAXNUM_8KIAUDBAS_ENCNUM )
		return MEDIA_TYPE_NULL;

	return m_atAudAdpParam[byEndIdx].GetAudCodeType();
}

/*====================================================================
	����  : SendMsgToMcu
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
BOOL C8KIAudBasInst::SendMsgToMcu(u16 wEvent, CServMsg* const pcServMsg)
{
    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[Send] audbas Message %u(%s).\n", wEvent, ::OspEventDesc(wEvent));

    if( OspIsValidTcpNode(g_c8KIAudBasApp.m_dwMcuNode))
    {
        post(g_c8KIAudBasApp.m_dwMcuIId, wEvent, pcServMsg->GetServMsg(), pcServMsg->GetServMsgLen(), g_c8KIAudBasApp.m_dwMcuNode);
    }

    return TRUE;
}

/*====================================================================
	����  : ProcInit
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void C8KIAudBasInst::ProcInit(CMessage * const pcMsg)
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcInit] pMsg is null \n" );
		return;
	}

	if( NORMAL == CurState() )
	{
		if (OspIsValidTcpNode( g_c8KIAudBasApp.m_dwMcuNode ) )
		{
			::OspNodeDiscCBRegQ( g_c8KIAudBasApp.m_dwMcuNode, GetAppID(), GetInsID() );
		}

		SendStatusChangeMsg(TRUE , u8(TBasBaseChnStatus::READY));

		LogPrint( LOG_LVL_WARNING, MID_MCU_BAS, "[ProcInit] Current state is NORMAL.\n" );

		return;
	}

	if ( IDLE != CurState() )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcRegAck] Current state is not IDLE! Error!\n" );
		printf("[ProcRegAck] Current state is not IDLE! Error!\n" );
        return;
    }
	
	u16 wRet = m_cBas.Create();
	if (wRet != (u16)Codec_Success)
    {
		printf("[ProcInit] BAS.%d Created Failed! REASON:%d!\n", GetInsID(), wRet);
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [ProcInit] BAS.%d Created Failed! REASON:%d!\n", GetInsID(), wRet);
        return ;
    }

	wRet = m_cBas.SetAudEncDataCallBack(BasEncFrameCallBack, this);
    if (wRet != (u16)Codec_Success)
    {
        m_cBas.Destroy();
		printf("[ProcInit] BAS.%d SetAudEncDataCallBack Failed! REASON:%d!\n", GetInsID(), wRet);
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[ProcInit] BAS.%d SetAudEncDataCallBack Failed! REASON:%d!\n", GetInsID(), wRet);
        return ;
    }
		
	//����BAS���ն���
	wRet = m_cAudioRcv.Create(MAX_AUDIO_FRAME_SIZE, BasNetRcvCallBack, (u32)this);
	if( wRet != MEDIANET_NO_ERROR )
	{
		m_cBas.Destroy();
		printf("ERROR: [Init] BAS.%d m_pcAudioRcv Created Failed! REASON:%d!\n", GetInsID(),  wRet);
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [ProcInit] BAS.%d m_pcAudioRcv Created Failed! REASON:%d!\n", GetInsID(),  wRet);
		return ;
	}
	else
	{
		//ÿ��BASͨ����Ӧ2·����
		for (u8 byIdx = 0; byIdx < MAXNUM_8KIAUDBAS_ENCNUM; byIdx++)
		{			
			wRet = m_cAudioSnd[byIdx].Create(MAX_AUDIO_FRAME_SIZE, 64<<10, 25, MEDIA_TYPE_G7221C);
			if(wRet != MEDIANET_NO_ERROR)
			{
				m_cBas.Destroy();
				printf("ERROR: [Init] BAS.%d m_pcAudioSnd.[%d] Created Failed! REASON:%d\n", GetInsID(), byIdx, wRet);
				LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
					"BAS: [CApu2BasChnnl::Init][error] BAS.%d m_pcAudioSnd.[%d] Created Failed! REASON:%d\n", 
					GetInsID(), byIdx, wRet);
				return ;
			}			
		}
	}



	if (OspIsValidTcpNode( g_c8KIAudBasApp.m_dwMcuNode ) )
    {
		::OspNodeDiscCBRegQ( g_c8KIAudBasApp.m_dwMcuNode, GetAppID(), GetInsID() );
	}


	SendStatusChangeMsg(TRUE , u8(TBasBaseChnStatus::READY));
	

    NEXTSTATE(NORMAL);

    return;
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
void C8KIAudBasInst::SendStatusChangeMsg( u8 byOnline, u8 byState)
{
    CServMsg cServMsg;
    TPeriEqpStatus tBasStatus;
    memset(&tBasStatus,0,sizeof(TPeriEqpStatus));

    tBasStatus.m_byOnline = byOnline;
    tBasStatus.SetMcuEqp( g_c8KIAudBasApp.GetMcuId(), g_c8KIAudBasApp.GetEqpId(), g_c8KIAudBasApp.GetEqpType());
	//tAudBasChnlStatus.ClrOutputAudParam(

	TAudBasChnStatus *ptAudBasChnlStatus = tBasStatus.m_tStatus.tAudBas.m_tAudBasStaus.m_t8KIAudBasStatus.GetAudChnStatus( GetInsID() - 1 );
	if( NULL != ptAudBasChnlStatus )
	{	
		ptAudBasChnlStatus->ClrOutputAudParam();
		ptAudBasChnlStatus->SetStatus(byState);
		tBasStatus.m_tStatus.tAudBas.m_tAudBasStaus.m_t8KIAudBasStatus.SetAudChnStatus( *ptAudBasChnlStatus,GetInsID() - 1 );
	}
    tBasStatus.SetAlias( g_c8KIAudBasApp.GetAlias() );
    
	u8 byChnType = BAS_8KIAUDCHN;
	u8 byChlIdx = GetInsID() - 1;
    cServMsg.SetMsgBody((u8*)&(tBasStatus), sizeof(TPeriEqpStatus));
	cServMsg.CatMsgBody((u8*)&byChnType,sizeof(byChnType));
	cServMsg.CatMsgBody((u8*)&byChlIdx,sizeof(byChlIdx));
    SendMsgToMcu(BAS_MCU_BASSTATUS_NOTIF, &cServMsg);
    return;
}

/*====================================================================
	����  : SetAudResend
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
BOOL32 C8KIAudBasInst::SetAudResend(u8 byEncIdx)
{
	/*���෢��*/
	s32 nSendTimes = 0;
	s32 nSendSpan  = 0;
	switch (g_c8KIAudBasApp.GetQualityLvl())
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

	u16 wRet = m_cAudioSnd[byEncIdx].SetAudioResend(nSendTimes, nSendSpan);
	if(wRet != MEDIANET_NO_ERROR)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[CApu2BasChnnl::SetSndObjectPara][error] BAS.%d m_pcAudioSnd[%d] set AudioResend failed! REASON: %d!\n", GetInsID(), byEncIdx, wRet);
		return FALSE;
	}
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"[CApu2BasChnnl::SetSndObjectPara] BAS.%d m_pcAudioSnd[%d] SetAudioResend Successed.\n", GetInsID(), byEncIdx);

	return TRUE;

}

/*====================================================================
	����  : SetSndObjectPara
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
BOOL32 C8KIAudBasInst::SetSndObjectPara( u32 dwDstIp,u16 wDstPort,u8 byEncIdx )
{
	
    u16 wRet = m_cAudioSnd[byEncIdx].Create( MAX_AUDIO_FRAME_SIZE, 64<<10, 25, m_atAudAdpParam[byEncIdx].GetAudCodeType() );

    if(wRet != MEDIANET_NO_ERROR)
    {
		/*��������255*/
		if( m_atAudAdpParam[byEncIdx].GetAudCodeType() == MEDIA_TYPE_NULL)
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_BAS, "[SetSndObjectPara][warning] BAS.%d m_cAudioSnd[%d] Not Create! REASON: EncAudType--MEDIA_TYPE_NULL!\n", 
				GetInsID(), byEncIdx);
		}
		/*���󴴽��쳣*/
        else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[SetSndObjectPara][error] BAS.%d Created m_cAudioSnd[%d] of AudType:[%d] Failed! REASON: %d!\n",
				GetInsID(), byEncIdx, m_atAudAdpParam[byEncIdx].GetAudCodeType(), wRet);
		}
		return FALSE;
    }

    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[SetSndObjectPara] BAS.%d Created m_cAudioSnd[%d] of AudType:[%d] Successed.\n",
		GetInsID(), byEncIdx, m_atAudAdpParam[byEncIdx].GetAudCodeType() );
	
    
    TNetSndParam tNetSndPar;
    memset( &tNetSndPar, 0, sizeof(tNetSndPar));
    tNetSndPar.m_byNum  = 1;
	/*Զ��RTP��ַ���˿�*/
    tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = dwDstIp;
    tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = wDstPort;
	/*����RTP��RTCP���Ͷ˿�*/
	//tNetSndPar.m_tLocalNet.m_wRTPPort       = wDstPort;
    //tNetSndPar.m_tLocalNet.m_wRTCPPort      = wDstPort + 1;
	u8 byChlIdx = GetInsID() - 1;
	tNetSndPar.m_tLocalNet.m_wRTPPort		  = BAS_8KE_LOCALSND_PORT + (g_c8KIAudBasApp.GetEqpId() - BASID_MIN) * BAS_8KH_PORTSPAN 
													+ ( byChlIdx * MAXNUM_8KIAUDBAS_ENCNUM + byEncIdx ) * PORTSPAN + 2;
	tNetSndPar.m_tLocalNet.m_wRTCPPort        = BAS_8KE_LOCALSND_PORT + (g_c8KIAudBasApp.GetEqpId() - BASID_MIN) * BAS_8KH_PORTSPAN 
													+ ( byChlIdx * MAXNUM_8KIAUDBAS_ENCNUM + byEncIdx ) * PORTSPAN + 2 + 1;

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"[SetSndObjectPara] BAS.%d m_cAudioSnd[%d] LocalSnd<RtpPort.%d, RtcpPort.%d>, RemoteRcv<IP.%s, Port.%d>.\n", 
		GetInsID(), byEncIdx, tNetSndPar.m_tLocalNet.m_wRTPPort, tNetSndPar.m_tLocalNet.m_wRTCPPort,
		strofip(tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr), tNetSndPar.m_tRemoteNet[0].m_wRTPPort);

	/*���÷��Ͷ������*/
    wRet = m_cAudioSnd[byEncIdx].SetNetSndParam(tNetSndPar);
    if(wRet != MEDIANET_NO_ERROR)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[SetSndObjectPara][error] BAS.%d m_cAudioSnd[%d] SetNetSndParam Failed! REASON: %d!\n",
			GetInsID(), byEncIdx, wRet);
		return FALSE;
    }
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[SetSndObjectPara] BAS.%d m_cAudioSnd[%d] SetNetSndParam Success.\n", 
		GetInsID(), byEncIdx);

	/*�����ش����*/
    wRet = m_cAudioSnd[byEncIdx].ResetRSFlag( 2000, TRUE );
    if(wRet != MEDIANET_NO_ERROR)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[SetSndObjectPara][error] BAS.%d m_cAudioSnd[%d] ResetRSFlag Failed! REASON: %d!\n", 
			GetInsID(), byEncIdx, wRet);
		return FALSE;
    }
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[SetSndObjectPara] BAS.%d m_cAudioSnd[%d] ResetRSFlag Successed.\n", 
		GetInsID(), byEncIdx);

	/*���෢������*/
	SetAudResend(byEncIdx);

	/*���ܲ���*/
    u8  abyKeyBuf[MAXLEN_KEY];
    memset(abyKeyBuf, 0, MAXLEN_KEY );
    s32 nKeyLen = 0;
    u8 byEncryptMode = m_tMediaEncrypt.GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
		m_cAudioSnd[byEncIdx].SetEncryptKey( NULL, 0, 0 );
        LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[CApu2BasChnnl] BAS.%d set key to NULL!\n", GetInsID());      
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
		
        m_cAudioSnd[byEncIdx].SetEncryptKey( (s8*)abyKeyBuf, (u16)nKeyLen, byEncryptMode );
    }
    m_cAudioSnd[byEncIdx].SetActivePT(m_atAudAdpParam[byEncIdx].GetAudCodeType());

	return TRUE;
}

/*====================================================================
	����  : SetMediaEncParam
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
BOOL32 C8KIAudBasInst::SetMediaDecParam( void )
{
	u8 byChlIdx = GetInsID() - 1;

	//TAudBasInfo *ptAudBasInfo = GetChnlInfo(byChIdx);
	if (m_tAudioCapInfo.GetAudioMediaType() == MEDIA_TYPE_AACLC || 
		m_tAudioCapInfo.GetAudioMediaType() == MEDIA_TYPE_AACLD)
	{		
		u8 byAudioMode = GetAudioMode(m_tAudioCapInfo.GetAudioMediaType(), m_tAudioCapInfo.GetAudioTrackNum());
		
		u16 wRet = m_cBas.SetAudioDecParam( byChlIdx, m_tAudioCapInfo.GetAudioMediaType(), byAudioMode );

		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
			" [SetMediaDecParam] BAS.%d SetAudioDecParam: byChIdx:[%d] m_dwMediaType[%d] m_dwChannelNum[%d] \n", 
			GetInsID(), byChlIdx, m_tAudioCapInfo.GetAudioMediaType(), m_tAudioCapInfo.GetAudioTrackNum() );
		
		if (wRet != (u16)Codec_Success)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
				"[SetMediaDecParam][error] BAS.%d SetAudioDecParam.%d Failed! Because.%d!\n", 
				GetInsID(), byChlIdx, wRet);
			return FALSE;
		}
	}
	return TRUE;
}

/*====================================================================
	����  : SetMediaEncParam
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
BOOL32 C8KIAudBasInst::SetMediaEncParam( void )
{
	
	u8 byChlIdx = GetInsID() - 1;
	
	u8 byAudioMode = 0;
	for( u8 byLoop = 0; byLoop < MAXNUM_8KIAUDBAS_ENCNUM; byLoop++ )
	{
		if( MEDIA_TYPE_NULL == m_atAudAdpParam[byLoop].GetAudCodeType() )
		{
			continue;
		}
		SetSndObjectPara( ntohl(g_c8KIAudBasApp.GetMcuIp()), 
			g_c8KIAudBasApp.GetMcuStartPort() + (byChlIdx * MAXNUM_8KIAUDBAS_ENCNUM + byLoop) * PORTSPAN + 2, 
			byLoop);		
		
		byAudioMode = GetAudioMode(m_atAudAdpParam[byLoop].GetAudCodeType(), m_atAudAdpParam[byLoop].GetTrackNum());

		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
			"[SetMediaEncParam] BAS.%d SetAudEncParam: byChIdx:[%d][%d] dwAudMode[%d] dwAudType[%d]\n", 
			GetInsID(), byChlIdx, byLoop, byAudioMode, m_atAudAdpParam[byLoop].GetAudCodeType());
		
		u16 wRet = m_cBas.SetAudEncParam(byChlIdx, byLoop, 
								m_atAudAdpParam[byLoop].GetAudCodeType(), 
								byAudioMode);

		if (wRet != (u16)Codec_Success)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
				"[SetMediaEncParam] BAS.%d Chnl.%d SetAudEncParam.%d Failed! Because.%d!\n", 
				GetInsID(), byChlIdx, byLoop, wRet);
			return FALSE;
		}
	}
	return TRUE;
}


/*====================================================================
	����  : SetRcvNetParam
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
BOOL32 C8KIAudBasInst::SetRcvObjectPara( void )
{
	u8 byChlIdx = GetInsID() - 1;
	u16 wRet = 0;
	s32 byKenLen = 0;

    u8 abyKeyBuf[MAXLEN_KEY];  
    memset(abyKeyBuf, 0, MAXLEN_KEY);

	/*���ü��ܲ���*/
    u8 byEncryptMode = m_tMediaEncrypt.GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
        m_cAudioRcv.SetDecryptKey( NULL, 0, 0 );
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
			"[SetRcvObjectPara] BAS.%d Set key to NULL!\n", GetInsID());
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
        m_cAudioRcv.SetDecryptKey( (s8*)abyKeyBuf, (u16)byKenLen, byEncryptMode );
    }
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[SetRcvObjectPara] BAS.%d Wrong byEncryptMode.%d \n", GetInsID(), byEncryptMode );
		return FALSE;
	}

	/*�����غ���Ϣ*/
    u8 byRealPayload   = m_tAudioCapInfo.GetAudioMediaType();
    u8 byActivePayload = m_tAudioCapInfo.GetActivePayLoad();
    m_cAudioRcv.SetActivePT( byActivePayload, byRealPayload );

    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"SetRcvObjectPara BAS.%d Mode: %u KeyLen: %u PT: %d AT: %d KeyBuf [%s]\n", 
		GetInsID(), m_tMediaEncrypt.GetEncryptMode(), byKenLen, byRealPayload ,byActivePayload,abyKeyBuf);


	TRSParam tNetRSParam;

    if (m_bIsNeedPrs)
    {
		TPrsTimeSpan tPrsTimeSpan = g_c8KIAudBasApp.GetPrsTimeSpan();
        tNetRSParam.m_wFirstTimeSpan  = tPrsTimeSpan.m_wFirstTimeSpan;
        tNetRSParam.m_wSecondTimeSpan = tPrsTimeSpan.m_wSecondTimeSpan;
        tNetRSParam.m_wThirdTimeSpan  = tPrsTimeSpan.m_wThirdTimeSpan;
        tNetRSParam.m_wRejectTimeSpan = tPrsTimeSpan.m_wRejectTimeSpan;
    }
    else
    {
        tNetRSParam.m_wFirstTimeSpan  = 0;
        tNetRSParam.m_wSecondTimeSpan = 0;
        tNetRSParam.m_wThirdTimeSpan  = 0;
        tNetRSParam.m_wRejectTimeSpan = 0;
    }

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"[SetRcvObjectPara] BAS.%d m_wFirstTimeSpan:%d, m_wSecondTimeSpan:%d, m_wThirdTimeSpan:%d, m_wRejectTimeSpan:%d!\n", 
		GetInsID(), tNetRSParam.m_wFirstTimeSpan, tNetRSParam.m_wSecondTimeSpan,  tNetRSParam.m_wThirdTimeSpan, tNetRSParam.m_wRejectTimeSpan);
	
	/*�ش����*/
	wRet = m_cAudioRcv.ResetRSFlag( tNetRSParam, TRUE );
	if(wRet != MEDIANET_NO_ERROR)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[SetRcvObjectPara] BAS.%d ResetRSFlag Failed! REASON: %d!\n", GetInsID(), wRet);
		return FALSE;
	}	

	/*��������*/
    wRet = m_cAudioRcv.StartRcv();
    if (MEDIANET_NO_ERROR != wRet)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[SetRcvObjectPara] BAS.%d StartRcv Failed! REASON: %d!\n", GetInsID(), wRet);
		return FALSE;
    }
    
	return TRUE;
	
}

/*====================================================================
	����  : SetRcvNetParam
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
BOOL32 C8KIAudBasInst::SetRcvNetParam( void )
{
	u8 byChlIdx = GetInsID() - 1;
		
	TLocalNetParam tLocalNetParam;
	memset(&tLocalNetParam, 0, sizeof(TLocalNetParam));
	//RTCPԴ
	tLocalNetParam.m_dwRtcpBackAddr = ntohl(g_c8KIAudBasApp.GetMcuIp());
	tLocalNetParam.m_wRtcpBackPort = g_c8KIAudBasApp.GetEqpStartPort() + byChlIdx * MAXNUM_8KIAUDBAS_ENCNUM * PORTSPAN + 2 + 1;
	//RTP����
	tLocalNetParam.m_tLocalNet.m_dwRTPAddr = 0;
    tLocalNetParam.m_tLocalNet.m_wRTPPort = g_c8KIAudBasApp.GetEqpStartPort() + byChlIdx * MAXNUM_8KIAUDBAS_ENCNUM * PORTSPAN + 2;
	tLocalNetParam.m_tLocalNet.m_wRTCPPort = g_c8KIAudBasApp.GetEqpStartPort() + byChlIdx * MAXNUM_8KIAUDBAS_ENCNUM * PORTSPAN + 2 + 3;
	
	//������ӡ
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"[SetRcvNetParam] BAS:%d SetRcvNetParam: RtcpBackAddr[%s]  RtcpBackPort[%d]  LocalRtpPort[%d]  LocalPrtcpPort[%d]\n", 
		GetInsID(), strofip(tLocalNetParam.m_dwRtcpBackAddr), tLocalNetParam.m_wRtcpBackPort, 
		tLocalNetParam.m_tLocalNet.m_wRTPPort, tLocalNetParam.m_tLocalNet.m_wRTCPPort);
	
	u16 wRet = m_cAudioRcv.SetNetRcvLocalParam(tLocalNetParam);
	if(wRet != MEDIANET_NO_ERROR)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[SetRcvNetParam][error] BAS:%d SetRcvNetParam Failed! Because.%d!\n", GetInsID(), wRet);
		return FALSE;
    }

	return TRUE;
}

/*====================================================================
	����  : InstanceEntry
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void C8KIAudBasInst::ProcStartAdaptReq( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcStartAdaptReq] pMsg is null \n" );
		return;
	}

	
	if ( NORMAL != CurState() )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcStartAdaptReq] Current state is not NORMAL! Error!\n" );
		printf("[ProcStartAdaptReq] Current state is not NORMAL! Error!\n" );
        return;
    }

	CServMsg cServMsg(pcMsg->content,pcMsg->length);

	u8 *pbyMsgBody = cServMsg.GetMsgBody();

	

	for( u8 byIdx=0;byIdx < MAXNUM_8KIAUDBAS_ENCNUM;++byIdx )
	{
		m_atAudAdpParam[byIdx] = *(TAudAdaptParam*)pbyMsgBody;
		pbyMsgBody += sizeof(TAudAdaptParam);
	}

	m_bIsNeedPrs = m_atAudAdpParam[0].IsNeedbyPrs();

	m_tMediaEncrypt = *(TMediaEncrypt*)pbyMsgBody;
	pbyMsgBody += sizeof(TMediaEncrypt);

	m_tAudioCapInfo = *(TAudioCapInfo* )pbyMsgBody;
	pbyMsgBody += sizeof(TAudioCapInfo);

	m_bySampleRate = *(u8*)pbyMsgBody;

	TEqp tBas;
	tBas.SetMcuEqp(g_c8KIAudBasApp.GetMcuId(), g_c8KIAudBasApp.GetEqpId(), g_c8KIAudBasApp.GetEqpType());
	cServMsg.SetMsgBody((u8*)&tBas, sizeof(tBas));
	
#if !defined(WIN32)
	u8 byChlIdx = GetInsID() - 1;
	/*ռ��ͨ��*/
	u16 wRet = m_cBas.AddChannel( byChlIdx );
    if(wRet != (u16)Codec_Success)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[AddBasMember] BAS.%d OccupyChnl.%d Failed! Because.%d!\n", GetInsID(), byChlIdx, wRet);
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
        return;
    }

	/*���ý��ն����ַ*/
	if( !SetRcvNetParam() )
	{
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
        return;
	}

	/*���ý��ն���*/
	if( !SetRcvObjectPara() )
	{
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
        return;
	}

	/*���ñ������*/
	if( !SetMediaEncParam() )
	{
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
        return;
	}

	/*���ý������*/
	if( !SetMediaDecParam() )
	{
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
        return;
	}

	
	wRet = m_cBas.StartAdaptor(TRUE);
	if (wRet != (u16)Codec_Success)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::OnStartAdaptReq][error] BAS.%d StartAdapt Failed! Because.%d!\n", GetInsID(), wRet);
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
        return;
	}
#endif

	NEXTSTATE(RUNNING);

	SendMsgToMcu( pcMsg->event + 1,&cServMsg );


}

/*====================================================================
	����  : ProcChgAdaptReq
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void C8KIAudBasInst::ProcChgAdaptReq(CMessage * const pcMsg)
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcChgAdaptReq] pMsg is null \n" );
		return;
	}

	
	if ( RUNNING != CurState() )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcChgAdaptReq] Current state is not NORMAL! Error!\n" );
		printf("[ProcChgAdaptReq] Current state is not NORMAL! Error!\n" );
        return;
    }

	CServMsg cServMsg(pcMsg->content,pcMsg->length);

	u8 *pbyMsgBody = cServMsg.GetMsgBody();

	for( u8 byIdx=0;byIdx < MAXNUM_8KIAUDBAS_ENCNUM;++byIdx )
	{
		m_atAudAdpParam[byIdx] = *(TAudAdaptParam*)pbyMsgBody;
		pbyMsgBody += sizeof(TAudAdaptParam);
	}

	m_bIsNeedPrs = m_atAudAdpParam[0].IsNeedbyPrs();

	m_tMediaEncrypt = *(TMediaEncrypt*)pbyMsgBody;
	pbyMsgBody += sizeof(TMediaEncrypt);

	m_tAudioCapInfo = *(TAudioCapInfo* )pbyMsgBody;
	pbyMsgBody += sizeof(TAudioCapInfo);

	m_bySampleRate = *(u8*)pbyMsgBody;

	TEqp tBas;
	tBas.SetMcuEqp(g_c8KIAudBasApp.GetMcuId(), g_c8KIAudBasApp.GetEqpId(), g_c8KIAudBasApp.GetEqpType());
	cServMsg.SetMsgBody((u8*)&tBas, sizeof(tBas));

#if !defined(WIN32)
	/*���ñ������*/
	if( !SetMediaEncParam() )
	{
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
		return;
	}
	
	/*���ý������*/
	if( !SetMediaDecParam() )
	{
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
		return;
	}
#endif
	
	SendMsgToMcu( pcMsg->event + 1,&cServMsg );
}

/*====================================================================
	����  : ProcStopAdaptReq
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void C8KIAudBasInst::ProcStopAdaptReq( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcStopAdaptReq] pMsg is null \n" );
		return;
	}

	
	if ( RUNNING != CurState() )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcStopAdaptReq] Current state is not NORMAL! Error!\n" );
		printf("[ProcChgAdaptReq] Current state is not NORMAL! Error!\n" );
        return;
    }

	CServMsg cServMsg( pcMsg->content,pcMsg->length );

	TEqp tBas;
	tBas.SetMcuEqp(g_c8KIAudBasApp.GetMcuId(), g_c8KIAudBasApp.GetEqpId(), g_c8KIAudBasApp.GetEqpType());
	cServMsg.SetMsgBody((u8*)&tBas, sizeof(tBas));
	
#if !defined(WIN32)
	/*�ر�����*/
	u16 wRet = m_cBas.StartAdaptor(FALSE);
	if (wRet != (u16)Codec_Success)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[ProcStopAdaptReq] BAS.%d StopAdapt Failed! Because.%d!\n", GetInsID(), wRet);
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
		return;
	}
#endif
	SendMsgToMcu( pcMsg->event + 1,&cServMsg );

	NEXTSTATE(NORMAL);

}

/*====================================================================
	����  : InstanceEntry
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void C8KIAudBasInst::ProcDisconnect( CMessage * const pcMsg )
{
	ProcStopAdaptReq( pcMsg );

	NEXTSTATE(NORMAL);
}

/*====================================================================
	����  : InstanceEntry
	����  : InstanceEntryʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void C8KIAudBasInst::InstanceEntry( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[InstanceEntry] pMsg is null \n" );
		return;
	}

	LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[InstanceEntry]Daemon:Recv msg %u(%s).\n", pcMsg->event, ::OspEventDesc(pcMsg->event));


	switch(pcMsg->event)
    {
	case EV_BAS_INI:
		ProcInit( pcMsg );
		break;

	case MCU_BAS_STARTADAPT_REQ:
		ProcStartAdaptReq(pcMsg);
		break;

	case MCU_BAS_CHANGEAUDPARAM_REQ:
		ProcChgAdaptReq(pcMsg);
		break;

	case MCU_BAS_STOPADAPT_REQ:
		ProcStopAdaptReq(pcMsg);
		break;

	case OSP_DISCONNECT:
		ProcDisconnect( pcMsg );
		break;

	default:
		LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[DaemonInstanceEntry] unknow msg.%d\n",pcMsg->event );
		break;
	}
	
}

/*====================================================================
	����  : DaemonProcInit
	����  : DaemonProcInit
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void C8KIAudBasInst::DaemonProcInit(CMessage * const pcMsg)
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[DaemonInstanceEntry] pMsg is null \n" );
		return;
	}

	//��ʼ��socket
	u16 wRet = KdvSocketStartup();
    if ( MEDIANET_NO_ERROR != wRet )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "BAS: [DaemonProcInit] KdvSocketStartup Failed!\n");
        return;
    }

	T8keEqpCfg *ptEqpCfg = (T8keEqpCfg*)pcMsg->content;
	
	g_c8KIAudBasApp.SetConnectIp( ptEqpCfg->GetConnectIp() );
	g_c8KIAudBasApp.SetConnectPort( ptEqpCfg->GetConnectPort() );
	g_c8KIAudBasApp.SetMcuId( LOCAL_MCUID );
    g_c8KIAudBasApp.SetEqpType( EQP_TYPE_BAS );    
    g_c8KIAudBasApp.SetAlias( "8KIAudBas" );

	s32 sdwQuaLvlDefault = 0;
	s32 sdwReturnValue = 0;
	s8  achProfileName[64] = {0};
	memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, "mcueqp.ini");
	BOOL32 bRet = TRUE;// ��ȡģ���

	// ��ȡ��Ƶ�����ȼ�
	bRet = GetRegKeyInt( achProfileName, SECTION_EqpMixer , KEY_QualityLvl, sdwQuaLvlDefault, &sdwReturnValue );
	g_c8KIAudBasApp.SetQualityLvl((u8)sdwReturnValue);




	NEXTSTATE(IDLE);


	SetTimer(EV_BAS_CONNECT_TIMER, BAS_CONNETC_TIMEOUT);
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[DaemonProcInit]set EV_BAS_CONNECT_TIMER!\n" );	
}

/*=============================================================================
  �� �� ���� ConnectMcu
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
             u32& dwMcuNode
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 C8KIAudBasInst::ConnectMcu( u32& dwMcuNode )
{
    BOOL32 bRet = TRUE;

    if( !OspIsValidTcpNode(dwMcuNode))
    {
        dwMcuNode = OspConnectTcpNode(htonl(g_c8KIAudBasApp.GetConnectIp()), g_c8KIAudBasApp.GetConnectPort(), 10, 0, 100 );  

        if (OspIsValidTcpNode(dwMcuNode))
        {
            ::OspNodeDiscCBRegQ( dwMcuNode, GetAppID(), GetInsID() );
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[ConnectMcu] Connect to Mcu Success,node is %u!\n", dwMcuNode);
			printf("[ConnectMcu] Connect to Mcu Success,node is %u!\n", dwMcuNode);
            // ��¼����IP
            g_c8KIAudBasApp.SetEqpIp( ntohl( OspNodeLocalIpGet(dwMcuNode) ) );
        }
        else
        {
            //����ʧ��
            LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ConnectMcu] Failed to Connect Mcu %s:%d>\n", 
                                    strofip(g_c8KIAudBasApp.GetConnectIp(), FALSE ), g_c8KIAudBasApp.GetConnectPort());
			printf("[ConnectMcu] Failed to Connect Mcu %s:%d>\n", 
                                    strofip(g_c8KIAudBasApp.GetConnectIp(), FALSE ), g_c8KIAudBasApp.GetConnectPort());
            bRet = FALSE;
        }
    }
    return bRet;
}

/*====================================================================
	����  : DaemonProcConnectTimeOut
	����  : connect��Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void C8KIAudBasInst::DaemonProcConnectTimeOut( void )
{	
    switch( CurState() )
    {
    case IDLE:
        {
            BOOL32 bRet = FALSE;
            bRet = ConnectMcu(g_c8KIAudBasApp.m_dwMcuNode);
            if( bRet )
            { 
                SetTimer(EV_BAS_REGISTER_TIMER, BAS_REGISTER_TIMEOUT); 
            }
            else
            {
                SetTimer(EV_BAS_CONNECT_TIMER, BAS_CONNETC_TIMEOUT);
            }
        }
        break;
    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[DaemonProcConnectTimeOut]: EV_BAS_CONNECT_TIMER received in wrong state %u!\n", CurState());
        break;
    }
 
    return;
}

void C8KIAudBasInst::DaemonProcRegisterTimeOut( void )
{	
    switch( CurState() )
	{
	case IDLE:
		{
			LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[DaemonProcRegisterTimeOut] App:%d, Ins:%d \n", GetAppID(), GetInsID());
			CServMsg cSvrMsg;
			TEqpRegReq tReg;
			memset(&tReg, 0, sizeof(tReg));
			
			
			tReg.SetEqpIpAddr( g_c8KIAudBasApp.GetEqpIp() );
			tReg.SetEqpType(g_c8KIAudBasApp.GetEqpType());
			tReg.SetEqpAlias(g_c8KIAudBasApp.GetAlias());
			tReg.SetVersion( DEVVER_BAS );
			tReg.SetMcuId( g_c8KIAudBasApp.GetMcuId() );
			
			cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
			u8 byIsAudBas = 1;
			cSvrMsg.CatMsgBody((u8*)&byIsAudBas, sizeof(byIsAudBas));
			
			post( MAKEIID( AID_MCU_PERIEQPSSN, CInstance::DAEMON ), BAS_MCU_REG_REQ,
						cSvrMsg.GetServMsg(), cSvrMsg.GetServMsgLen(), g_c8KIAudBasApp.m_dwMcuNode);

			SetTimer(EV_BAS_REGISTER_TIMER, BAS_REGISTER_TIMEOUT); 
			break;
		}
	default:
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[CBASInst::ProRegister]: EV_BAS_REGISTER_TIMER received in wrong state %u!\n", CurState());
        break;
	}

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
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void C8KIAudBasInst::DaemonProcRegisterAck( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[DaemonProcRegisterAck] pMsg is null \n" );
		return;
	}

	if ( IDLE != CurState() )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcRegAck] Current state is not IDLE! Error!\n" );
		printf("[ProcRegAck] Current state is not IDLE! Error!\n" );
        return;
    }

    NEXTSTATE(NORMAL);

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqpRegAck tRegAck = *(TEqpRegAck*)cServMsg.GetMsgBody();

	g_c8KIAudBasApp.m_dwMcuIId = pcMsg->srcid;

	KillTimer(EV_BAS_REGISTER_TIMER);
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "register to mcu.A succeed !\n");
	printf("register to mcu.A succeed !\n");

	g_c8KIAudBasApp.SetEqpId( tRegAck.GetEqpId() );
    g_c8KIAudBasApp.SetMcuStartPort( tRegAck.GetMcuStartPort() );
    g_c8KIAudBasApp.SetEqpStartPort( tRegAck.GetEqpStartPort() );
	g_c8KIAudBasApp.SetMcuIp( tRegAck.GetMcuIpAddr() );

	TPrsTimeSpan tPrsTimeSpan = *(TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TEqpRegAck));
	tPrsTimeSpan.m_wFirstTimeSpan  = ntohs(tPrsTimeSpan.m_wFirstTimeSpan);
	tPrsTimeSpan.m_wSecondTimeSpan = ntohs(tPrsTimeSpan.m_wSecondTimeSpan);
	tPrsTimeSpan.m_wThirdTimeSpan  = ntohs(tPrsTimeSpan.m_wThirdTimeSpan);
	tPrsTimeSpan.m_wRejectTimeSpan = ntohs(tPrsTimeSpan.m_wRejectTimeSpan);
	// xsl [8/15/2006] ����һ�����ֵ����ֹ�ײ��ڴ�����ʧ��
	if(tPrsTimeSpan.m_wRejectTimeSpan > DEF_LOSETIMESPAN_PRS)
	{
		tPrsTimeSpan.m_wRejectTimeSpan = DEF_LOSETIMESPAN_PRS;
	}
	g_c8KIAudBasApp.SetPrsTimeSpan( tPrsTimeSpan );

	
	u16 wMTUSize = *(u16*)( cServMsg.GetMsgBody() + sizeof(TEqpRegAck) + sizeof(TPrsTimeSpan) );
	wMTUSize = ntohs( wMTUSize );
	g_c8KIAudBasApp.SetMTUSize( wMTUSize );

	for( u8 byLoop = 1;byLoop <= MAXNUM_8KI_AUD_BAS; ++byLoop )
	{
		post(MAKEIID(GetAppID(), byLoop), EV_BAS_INI, NULL, 0);	
	}
	
}

void C8KIAudBasInst::DaemonProcRegisterNack( void )
{
	if ( IDLE != CurState() )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcRegAck] Current state is not IDLE! Error!\n" );
		printf("[ProcRegAck] Current state is not IDLE! Error!\n" );
        return;
    }

	LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[Info] bas Register to MCU be refused .retry...\n");

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
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void C8KIAudBasInst::DaemonProcAdpatMsg( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[DaemonInstanceEntry] pMsg is null \n" );
		return;
	}

	if ( NORMAL != CurState() )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[DaemonProcAdpatMsg] Current state is not NORMAL! Error!\n" );
		printf("[DaemonProcAdpatMsg] Current state is not NORMAL! Error!\n" );
        return;
	}

	CServMsg cServMsg( pcMsg->content,pcMsg->length );

	LogPrint(LOG_LVL_ERROR, MID_MCU_BAS,"[DaemonProcAdpatMsg] rcv Msg%d(%s) chl.%d\n",pcMsg->event,
		OspEventDesc(pcMsg->event),cServMsg.GetChnIndex() );	

	if( cServMsg.GetChnIndex() >= 0 && cServMsg.GetChnIndex() < MAXNUM_8KI_AUD_BAS )
	{
		post( MAKEIID(GetAppID(), cServMsg.GetChnIndex()+1 ), pcMsg->event, pcMsg->content, pcMsg->length );
	}
	else
	{
		SendMsgToMcu(pcMsg->event+2, &cServMsg);
	}
		

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
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void C8KIAudBasInst::DaemonProcDisconnect( CMessage* const pMsg )
{

	SetTimer(EV_BAS_CONNECT_TIMER, BAS_CONNETC_TIMEOUT);

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[DaemonProcDisconnect]set EV_BAS_CONNECT_TIMER!\n" );	

	NEXTSTATE(IDLE);

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
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void C8KIAudBasInst::DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp)
{
	if( NULL == pMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[DaemonInstanceEntry] pMsg is null \n" );
		return;
	}

	LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[DaemonInstanceEntry]Daemon:Recv msg %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event));
	
	switch(pMsg->event)
    {
	case EV_BAS_INI:
		DaemonProcInit( pMsg );
		break;

	case EV_BAS_CONNECT_TIMER:
		DaemonProcConnectTimeOut();
		break;

	case EV_BAS_REGISTER_TIMER:
        DaemonProcRegisterTimeOut();
        break; 

	case MCU_BAS_REG_ACK:
		DaemonProcRegisterAck( pMsg );
		break;

	case MCU_BAS_REG_NACK:
		DaemonProcRegisterNack();
		break;

	case MCU_BAS_STARTADAPT_REQ:
	case MCU_BAS_CHANGEAUDPARAM_REQ:
	case MCU_BAS_STOPADAPT_REQ:
		DaemonProcAdpatMsg(pMsg);
		break;

	case OSP_DISCONNECT:
		DaemonProcDisconnect( pMsg );
		break;

	default:
		LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[DaemonInstanceEntry] unknow msg.%d\n",pMsg->event );
		break;
	}
}

/*====================================================================
	����  : GetAudioMode
	����  : Daemonʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
u8 C8KIAudBasInst::GetAudioMode(const u8 byAudioType, u8 byAudioTrackNum)
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
			"BAS: [CApu2BasChnnl::GetAudioMode][error] BAS.%d GetAudioMode MediaType.%d not surportted!!\n", GetInsID()-1, byAudioType);
        return 0;
    }
    return byAudMode;
}

void C8KIBASCfg::Clear( void )
{
	m_dwMcuNode = INVALID_NODE;
	m_dwMcuIId  = 0;
	m_dwConnectIP = 0;
	m_wConnectPort = 0;
	m_wEqpStartPort = 0;
	m_wEqpStartPort = 0;
	m_wMcuStartPort = 0;
	m_wMcuStartPort = 0;
	m_EqpId = 0;
	m_byEqpType = 0;
	m_byEqpType = 0;
	m_MTUSize = 0;
	m_byQualityLvl = 0;
	m_dwLocalIP = 0;
	m_byMcuId = 0;
	memset( &m_achAlias,0,sizeof(m_achAlias) );
}

C8KIBASCfg::C8KIBASCfg()
{
	Clear();
}

C8KIBASCfg::~C8KIBASCfg()
{
	Clear();
}

#endif//#ifdef _8KI_