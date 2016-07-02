/*****************************************************************************
  ģ����      : libeapu.a
  �ļ���      : eapuinst.cpp
  ����ļ�    : eapuinst.h
  �ļ�ʵ�ֹ���: 
  ����        : ��־��
  �汾        : V1.0  Copyright(C) 2009-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2010/01/20  1.0         ��־��        ����
******************************************************************************/
#include "eapuinst.h"
#include "boardagentbasic.h"
extern s32  g_nPrintElog;
s32 g_nSendTimes = 0;
s32 g_nSendSpan  = 0;
/*====================================================================
    ������      ��mixlog
    ����        ����־����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
/*lint -save -e438 -e530 -e843*/
static void mixlog( u8 byLevel, s8* pszFmt, ... )
{
    static const s8 * pchLevelName[] = {(s8 *)"[CRIT]", (s8 *)"[WARN]", (s8 *)"[INFO]",
        (s8 *)"[RCVDATA]",(s8 *)"[SNDDATA]", (s8 *)"[DETAIL]", ""};

    s8 achPrintBuf[512];
    s32  nBufLen = 0;
    va_list argptr;
    
    if ( g_nPrintElog >= byLevel )
    {          
        nBufLen = sprintf( achPrintBuf, "%s: ", pchLevelName[byLevel] );         
        va_start( argptr, pszFmt );
        vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
        va_end( argptr ); 
        OspPrintf( TRUE, FALSE, achPrintBuf ); 
    }

    return;
}
/*lint -restore*/
/*====================================================================
    ������      ��StrOfIP
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
static char * StrOfIP( u32 dwIP )
{
    dwIP = htonl(dwIP);
    static char strIP[17];  
    u8 *p = (u8 *)&dwIP;
    sprintf(strIP,"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);
    return strIP;
}

/*=============================================================================
  �� �� ���� SaveDataToFile
  ��    �ܣ� ���浽�ļ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const s8* szFileName���ļ���
                const void* pData����������
                u32 nLen��pData����
                BOOL32 bNext����������(FALSE����������ļ�����)
                FILE **pFile, FILE **pLenFile, �����ļ��������ֹͬʱ�ж�·��������
  �� �� ֵ�� BOOL: TRUE,�ɹ�����,FALSE,���α���ʧ�� 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��        �汾        �޸���        �߶���    �޸�����
    2010/01/20  1.0         ��־��          ����
=============================================================================*/
static BOOL32 SaveDataToFile(const s8* szFileName, const void* pData, u32 nLen, BOOL32 bNext, FILE **pFile, FILE **pLenFile)
{
    //�״δ��ļ�
    if((NULL == *pFile) && (NULL == *pLenFile) && (NULL != szFileName))
    {
        s8 dataname[64] = {0};
        strcpy(dataname,szFileName);
        *pFile = fopen(dataname,"wb");
        if(NULL == *pFile)
        {
            printf("[SaveDataToFile]: open data file failed\n");
            return FALSE;
        }
        strcat(dataname, ".txt");
        *pLenFile = fopen(dataname,"wb");
        if(NULL == *pLenFile)
        {
            printf("[SaveDataToFile]: open datalen file failed\n");
            fclose(*pFile);
            *pFile = NULL;
            return FALSE;
        }
    }

    if(NULL == *pFile || NULL == *pLenFile)
    {
        printf("[SaveDataToFile]: save file(%s) error, file(%p, %p)\n", szFileName, *pFile, *pLenFile);
        if(*pFile != NULL)
        {
            fclose(*pFile);
            *pFile = NULL;
        }
        if(*pLenFile != NULL)
        {
            fclose(*pLenFile);
            *pLenFile = NULL;
        }
        return FALSE;
    }

    //д�ļ�
    if (NULL != pData && nLen > 0)
    {
        //д����
        u8* pDst = (u8*)pData;
        fwrite(pDst, nLen, 1, *pFile);
        //д����
        fprintf(*pLenFile, "%d\n", nLen);
    }
    //�Ƿ�ر��ļ�
    if(!bNext)
    {
        //�ر������ļ�
        fclose(*pFile);
        *pFile = NULL;
        printf("[SaveDataToFile]: %s over!.\n",szFileName);
        //�رճ����ļ�
        fclose(*pLenFile);
        *pLenFile = NULL;
        printf("[SaveDataToFile]: %s.txt over!.\n",szFileName);
    }
    return TRUE;
}

/*====================================================================
    ������      ��MixNetRcvCallBack
    ����        �����ձ���ص��������յ������ݴ���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
extern u32 g_dwSaveChId;
extern u32 g_dwSaveNum;
static void MixNetRcvCallBack(PFRAMEHDR pFrmHdr, u32 dwContext)
{
    static FILE* s_fFile = NULL;
    static FILE* s_fLen = NULL;
    CRcvCallBack *pcManaPara = (CRcvCallBack *)dwContext;
    TFrameHeader tFrame = {0};
    tFrame.m_dwMediaType = pFrmHdr->m_byMediaType;
    if ((u8)tFrame.m_dwMediaType == MEDIA_TYPE_AACLC)
    {
        tFrame.m_dwAudioMode = AUDIO_MODE_AACLC_32_M;
    }
    else if ((u8)tFrame.m_dwMediaType == MEDIA_TYPE_AACLD)
    {
        tFrame.m_dwAudioMode = AUDIO_MODE_AACLD_32_M;
    } 
    else
    {
        tFrame.m_dwAudioMode = pFrmHdr->m_byAudioMode;
    }
    tFrame.m_dwDataSize  = pFrmHdr->m_dwDataSize;
    tFrame.m_pData       = pFrmHdr->m_pData;
    tFrame.m_dwFrameID   = pFrmHdr->m_dwFrameID;
    tFrame.m_dwSSRC      = pFrmHdr->m_dwSSRC;

    //save dec data
    if(g_dwSaveNum > 0 && g_dwSaveChId == pcManaPara->m_byChnnlId)
    {
        BOOL32 bNext = (g_dwSaveNum> 1) ? TRUE : FALSE;
        BOOL32 bRet = SaveDataToFile("/ramdisk/mcudecdata", tFrame.m_pData, tFrame.m_dwDataSize, bNext, &s_fFile, &s_fLen);
        if(bRet)
        {
            g_dwSaveNum--;
        }
    }


    //�����յ��Ļ�����Ա֡���ݴ�����Ӧ�Ļ������顣
    u16 wRet = pcManaPara->m_pcMixerInst->m_cMixer.SetAudDecData(pcManaPara->m_byChnnlId, tFrame);
    if (wRet != (u16)Codec_Success)
    {
        mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MixNetRcvCallBack() Call SetAudDecData() Failed! ret=[%d]  chnid(%d), datasize(%d)\n",
                pcManaPara->m_pcMixerInst->GetInsID(), wRet, pcManaPara->m_byChnnlId, tFrame.m_dwDataSize);
    }
    if (MIXLOG_LEVER_RCVDATA == g_nPrintElog)
    {
        mixlog(MIXLOG_LEVER_RCVDATA,"Mixer[%d]MixNetRcvCallBack() Call SetAudDecData() chnid(%d),MediaType(%d)  AudioMode(%d) datasize(%d)\n",
            pcManaPara->m_pcMixerInst->GetInsID(), pcManaPara->m_byChnnlId,tFrame.m_dwMediaType,tFrame.m_dwAudioMode, tFrame.m_dwDataSize);
    }
}
/*====================================================================
    ������      ��MixEncFrameCallBack
    ����        ������������ص�������������õĻ�������ͨ�����Ͷ����ͳ�ȥ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
static void MixEncFrameCallBack(TKdvMixerType* ptType, PTFrameHeader pFrameInfo, void* pContext)
{
    CEapuMixInst *pInst = (CEapuMixInst*)pContext;
    FRAMEHDR tFrmHdr = {0};
    tFrmHdr.m_dwDataSize = pFrameInfo->m_dwDataSize;
    tFrmHdr.m_dwFrameID = pFrameInfo->m_dwFrameID;
    tFrmHdr.m_byMediaType = (u8)pFrameInfo->m_dwMediaType;
    if (tFrmHdr.m_byMediaType == MEDIA_TYPE_AACLC)
    {
        tFrmHdr.m_byAudioMode = AUDIO_MODE_AACLC_32_M;
    }
    else if (tFrmHdr.m_byMediaType == MEDIA_TYPE_AACLD)
    {
        tFrmHdr.m_byAudioMode = AUDIO_MODE_AACLD_32_M;
    } 
    else
    {
        tFrmHdr.m_byAudioMode = (u8)pFrameInfo->m_dwAudioMode;
    }
    tFrmHdr.m_dwSSRC = pFrameInfo->m_dwSSRC;
    tFrmHdr.m_pData = pFrameInfo->m_pData;
    u16 wRet = 0 ;
    CKdvMediaSnd * pcMediaSnd =  NULL;
    pcMediaSnd = pInst->GetMediaSndPtr(ptType->dwChID,ptType->bDMode);
    if (pcMediaSnd == NULL)
    {
        mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MixEncFrameCallBack() pcMediaSnd == NULL  CurrentMixMode : mcuPartMix\n",pInst->GetInsID());
        return;
    }
    wRet = pcMediaSnd->Send(&tFrmHdr);
    if (wRet != MEDIANET_NO_ERROR)
    {
        mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MixEncFrameCallBack() call Send() FAILED!! wRet = [%d] chid[%d] bDMode=[%d] MediaType=[%d]\n",
        pInst->GetInsID(),wRet, ptType->dwChID,ptType->bDMode,tFrmHdr.m_byMediaType);
    }
    if (MIXLOG_LEVER_SNDDATA == g_nPrintElog)
    {
        mixlog(MIXLOG_LEVER_SNDDATA,"Mixer[%d]MixEncFrameCallBack() call Send() chid(%d) bDMode(%d) MediaType(%d) AudioMode(%d)\n",
                pInst->GetInsID(), ptType->dwChID,ptType->bDMode,tFrmHdr.m_byMediaType,tFrmHdr.m_byAudioMode);
    }
    return;
}
/*====================================================================
    ������      ��EAPUVOLACTCALLBACK
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
static void EAPUVOLACTCALLBACK(TKdvMixerType* ptType, void* pContext)
{
    CEapuMixInst *pInst = (CEapuMixInst*)pContext;
    u32 adwChnId[MAXNUM_EMIXER_DEPTH];
    u32 dwChNum = 0 ;
    u8  abyActiveChnId[MAXNUM_EMIXER_DEPTH];
    pInst->m_cMixer.GetActiveChID(adwChnId,dwChNum);
	
	/*lint -save -e650*/
    if ( dwChNum != 0)
    {
        for ( u8 byIndex = 0 ;byIndex < MAXNUM_EMIXER_DEPTH;byIndex++)
        {
            if ( 0xFFFF != adwChnId[byIndex] )
            {
                abyActiveChnId[byIndex] = (u8)adwChnId[byIndex];
            }
        }
    }
	/*lint -restore*/

    if (ptType->dwChID == 0xFFFF)
    {
        OspPrintf(TRUE,FALSE,"\t\t\tInvalid ActiveChnId\n");
        return;
    }
    u8 byExitChid = (u8)ptType->dwChID;
    CServMsg cServMsg;
    cServMsg.SetMsgBody(abyActiveChnId, MAXNUM_EMIXER_DEPTH * sizeof(u8));
    cServMsg.CatMsgBody((u8*)&byExitChid, sizeof(u8));
    ::OspPost(MAKEIID(AID_MIXER, (u16)pInst->GetInsID()),
              EV_MIXER_ACTIVECHANGE_NOTIF,
              cServMsg.GetServMsg(),
              cServMsg.GetServMsgLen());
    return;
}
//���캯��
CEapuMixInst::CEapuMixInst()
{
    m_byMixMemNum = 0;
    m_byRcvOff = 0;                             //����ƫ��
    m_bySndOff = 0;                             //����ƫ��
    m_bIsDMode = FALSE;                         //�û������Ƿ�Ϊ���ʽ������
    m_tGrpStatus.Reset();                       //������״̬
    m_byAudType[0] = MEDIA_TYPE_MP3;            //Ĭ����Ƶ��ʽΪMP3
    m_byAudType[1] = MEDIA_TYPE_MP3;
    for ( u8 byChnIdx = 0;byChnIdx < MAXNUM_MIXER_CHNNL;byChnIdx++)
    {
        m_dwMcuRcvIp =0;
        m_dwMcuRcvIpB = 0;
        m_wMcuRcvStartPort = 0;
        m_wMcuRcvStartPortB = 0;
        m_abyMtId2ChnIdx[byChnIdx] = 0;
    }
    m_bNeedPrs = FALSE;
    m_bIsSendRedundancy = FALSE;    
    m_bIsStartVac =FALSE;
    m_bIsStartMix = FALSE;
    m_dwMcuRcvIp = 0;           //MCU.A ���յ�ַ
    m_dwMcuRcvIpB = 0;          //MCU.B ���յ�ַ
    m_wMcuRcvStartPort = 0;     //MCU.A ������ʼ�˿ں�
    m_wMcuRcvStartPortB = 0;    //MCU.B ������ʼ�˿ں�
}
//��������
/*lint -save -e1551*/
CEapuMixInst::~CEapuMixInst()
{
    m_cMixer.Destroy();
}
/*lint -restore*/
/*====================================================================
    ������      ��
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp)
{
    if( NULL == pMsg || NULL == pcApp )
    {
        OspPrintf( TRUE, FALSE, "[DaemonInstanceEntry] The received msg's pointer in the msg entry is NULL!\n");
        return;
    }
    mixlog(MIXLOG_LEVER_INFO,"[DaemonInstanceEntry]:Recv msg %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event));
    switch(pMsg->event)
    {
    //�������ϵ���ɰ�����ͣ���ʼ��
    case EV_MIXER_INIT:
        DaemonProcInitalMsg(pMsg);
        break;
    // ����Qosֵ
    case MCU_EQP_SETQOS_CMD:
        ProcSetQosInfo(pMsg);
        break;

    //OSP ������Ϣ
    case OSP_DISCONNECT:
        DaemonMsgDisconnectProc(pMsg);
        break;

    case MCU_MIXER_STARTMIX_REQ:            // ��ʼ��������Ϣ
    case MCU_MIXER_STOPMIX_REQ:             // ֹͣ��������Ϣ
    case MCU_MIXER_ADDMEMBER_REQ:           // ����,ɾ����Ա������Ϣ
    case MCU_MIXER_REMOVEMEMBER_REQ:    
    case MCU_MIXER_SETCHNNLVOL_CMD:         // ��������
    case MCU_MIXER_FORCEACTIVE_REQ:         // ǿ�ƻ�������
    case MCU_MIXER_CANCELFORCEACTIVE_REQ:   // ȡ��ǿ�ƻ�������
    case MCU_MIXER_SETMIXDEPTH_REQ:         // ���û������������Ϣ
    case MCU_MIXER_SEND_NOTIFY:             // �Ƿ�Ҫ��������֪ͨ
    case MCU_MIXER_VACKEEPTIME_CMD:         // ����������ʱ����ʱ����Ϣ
    case MCU_MIXER_STARTVAC_CMD:
    case MCU_MIXER_STOPVAC_CMD:
/*  case MCU_MIXER_SETMEMBER_CMD:           // ָ��������Ա
    case MCU_MIXER_CHANGEMIXDELAY_CMD:*/
        {
            u8 byIndex;
            for (byIndex = 0; byIndex < g_cEapuMixerApp.m_tEapuCfg.m_byMixerNum ; byIndex++)
            {
                if (u16(pMsg->srcid) == g_cEapuMixerApp.m_tEapuCfg.m_tEapuMixerCfg[byIndex].byEqpId)
                {
                    break;
                }
            }
            post(MAKEIID(GetAppID(), byIndex + 1), pMsg->event, pMsg->content, pMsg->length);
        }
        break;
    // ��ʾ������״̬
    case EV_MIXER_SHOWGROUP:
        ShowAllMixGroupStatus(pcApp);
        break;
/*    // ȡ��������״̬Ӧ���ʱ
    case EV_MIXER_MCU_GETMSSTATUS:
    case MCU_EQP_GETMSSTATUS_ACK:
        DaemonProcGetMsStatusRsp(pMsg,pcApp);*/
    default:
        break;
    }
}
/*====================================================================
    ������      ��DaemonProcInitalMsg
    ����        �������ɹ���Աʵ�����յ��ĳ�ʼ����Ϣ��������Ϣ�ַ���������ͨ������ʵ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::DaemonProcInitalMsg(CMessage* const pMsg)
{
    if( NULL == pMsg )
    {
        OspPrintf( TRUE, FALSE, "[DaemonProcInitalMsg]NULL == pMsg\n");
        return;
    }
    u32 dwChnCount = 0 ;
    u16 wIndex = 0 ;
    if (pMsg->content == NULL)
    {
        OspPrintf( TRUE, FALSE, "[DaemonProcInitalMsg]pMsg->content == NULL\n");
		printf("[DaemonProcInitalMsg]pMsg->content == NULL ,So Return!!\n");
		return;
    }
	s32 sdwDefault = 0;
	s32 sdwReturnValue = 0;
	s8  achProfileName[64] = {0};
	memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, "mcueqp.ini");
	BOOL32 bRet = GetRegKeyInt( achProfileName, SECTION_EqpMixer, KEY_QualityLvl, sdwDefault, &sdwReturnValue);
	//--�������ȵȼ����� 0:�ر� 1:�� 2:�� Ĭ����:�ȼ�0
	if( sdwReturnValue == 1 )  
	{
		g_nSendTimes = 1;
		g_nSendSpan = 1;
	}
	else if( sdwReturnValue == 2 )
	{	
		g_nSendTimes = 2;
		g_nSendSpan = 1;
	}
	else
	{	
		g_nSendTimes = 0;
		g_nSendSpan = 0;
	}
    TEapuCfg* ptEapuCfg = (TEapuCfg*)pMsg->content;
    g_cEapuMixerApp.m_tEapuCfg = *ptEapuCfg;
    if (g_nPrintElog == MIXLOG_LEVER_DETAIL)
    {
        g_cEapuMixerApp.m_tEapuCfg.printf();
    }
    for ( wIndex = 0 ;wIndex < ptEapuCfg->m_byMixerNum ; wIndex++ )
    {
        if ( (dwChnCount += ptEapuCfg->m_tEapuMixerCfg[wIndex].m_byMixerMemberNum) > MAXNUM_MIXER_CHNNL)//��Ҫ�󴴽���ͨ�����ʹ���64·
        {
            mixlog(MIXLOG_LEVER_WARN,"Mixer[%d][DaemonProcInitalMsg] ChnNum[%d] AllChnNum[%d] > MAXNUM_MIXER_CHNNL[%d]\n",
                wIndex,g_cEapuMixerApp.m_tEapuCfg.m_tEapuMixerCfg[wIndex].m_byMixerMemberNum,dwChnCount, MAXNUM_MIXER_CHNNL);
            return;
        }
        else
        {
            if ( ptEapuCfg->m_tEapuMixerCfg[wIndex].m_byMixerMemberNum >= MIXER_EAPU_CHN_MIXNUM)
            {
                post(MAKEIID(GetAppID(),wIndex+1),EV_MIXER_INIT,(u8*)(&ptEapuCfg->m_tEapuMixerCfg[wIndex]),sizeof(TEapuMixerCfg));
                mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]DaemonProcInitalMsg()  ChnNum[%d]\n",wIndex, ptEapuCfg->m_tEapuMixerCfg[wIndex].m_byMixerMemberNum);
            }
        }    
    }
	NEXTSTATE( (s32)NORMAL );  //DAEMON ʵ������ NORMAL ״̬
    mixlog(MIXLOG_LEVER_INFO,"DAEMON IDLE -->  NORMAL\n");
}
/*====================================================================
    ������      ��DaemonProcGetMsStatusRsp
    ����        ������������״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::DaemonProcGetMsStatusRsp(CMessage* const pMsg)
{
    BOOL bSwitchOk = FALSE;
    CServMsg cServMsg(pMsg->content, pMsg->length);
    if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event )
    {
        TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer(EV_MIXER_MCU_GETMSSTATUS);
        mixlog(MIXLOG_LEVER_INFO,"[DeamonProcGetMsStatus] receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                  ptMsStatus->IsMsSwitchOK());

        if(ptMsStatus->IsMsSwitchOK())         //�����ɹ�
        {
            bSwitchOk = TRUE;
        }
    }
    // ����ʧ�ܻ��߳�ʱ
    if( !bSwitchOk )
    {
        NEXTSTATE((int)IDLE);                      //  DAEMON ʵ���������״̬
        if( INVALID_NODE == g_cEapuMixerApp.m_dwMcuNode )
        {
            SetTimer(EV_MIXER_CONNECT, MIX_CONNETC_TIMEOUT);
        }
        if( INVALID_NODE == g_cEapuMixerApp.m_dwMcuNodeB )
        {
            SetTimer(EV_MIXER_CONNECTB, MIX_CONNETC_TIMEOUT);
        }
    }
    return;
}
/*====================================================================
    ������      ��ProcGetMsStatusRsp
    ����        ������������״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::ProcGetMsStatusRsp(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
	BOOL32 bSwitchOK = FALSE;
    if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event )
    {
        TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer(EV_MIXER_MCU_GETMSSTATUS);
        mixlog(MIXLOG_LEVER_INFO,"[ProcGetMsStatusRsp] receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                  ptMsStatus->IsMsSwitchOK());

	    bSwitchOK = ptMsStatus->IsMsSwitchOK();
    }

	if (!bSwitchOK)
	{
		//20110914 zjl ��������ʧ�� ����״̬  �ٶ��� ������
		mixlog(MIXLOG_LEVER_INFO, "[ProcGetMsStatusRsp] ClearInstStatus!\n");	
		m_tRegAckInfo.m_dwMcuIId = INVALID_INS;
		m_tRegAckInfo.m_dwMcuIIdB = INVALID_INS;
		m_tRegAckInfo.m_dwMpcSSrc = 0;
		StopAllTask();

		if (OspIsValidTcpNode(g_cEapuMixerApp.m_dwMcuNode))
		{
			mixlog(MIXLOG_LEVER_INFO, "[ProcGetMsStatusRsp] OspDisconnectTcpNode A!\n");	
			OspDisconnectTcpNode(g_cEapuMixerApp.m_dwMcuNode);
		}
		if (OspIsValidTcpNode(g_cEapuMixerApp.m_dwMcuNodeB))
		{
			mixlog(MIXLOG_LEVER_INFO, "[ProcGetMsStatusRsp] OspDisconnectTcpNode B!\n");	
			OspDisconnectTcpNode(g_cEapuMixerApp.m_dwMcuNodeB);
		}

		if( INVALID_NODE == g_cEapuMixerApp.m_dwMcuNode)
		{
			mixlog(MIXLOG_LEVER_INFO, "[ProcGetMsStatusRsp] EV_MIXER_CONNECT!\n");	
			SetTimer(EV_MIXER_CONNECT, MIX_CONNETC_TIMEOUT);
		}
		if( INVALID_NODE == g_cEapuMixerApp.m_dwMcuNodeB)
		{
			mixlog(MIXLOG_LEVER_INFO, "[ProcGetMsStatusRsp] EV_MIXER_CONNECTB!\n");	
			SetTimer(EV_MIXER_CONNECTB, MIX_CONNETC_TIMEOUT);
		}
	}
    return;
}
/*====================================================================
    ������      ��MsgProcInitalMsg
    ����        �������������ʼ����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::MsgProcInitalMsg(CMessage* const pMsg)
{
    if (pMsg == NULL || pMsg->content == NULL)
    {
        mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgProcInitalMsg() pMsg == NULL || pMsg->content == NULL\n",GetInsID());
        return;
    }
    if (pMsg->content ==NULL)
    {
        mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgProcInitalMsg() pMsg->content ==NULL\n",GetInsID());
    }
    m_tEapuMixerCfg = *(TEapuMixerCfg*)(pMsg->content);
    m_tEapuMixerCfg.printf();
    m_bIsDMode = m_tEapuMixerCfg.m_bIsMultiFormat;
    m_byMixMemNum = m_tEapuMixerCfg.m_byMixerMemberNum;
    m_tGrpStatus.Reset();
    m_tGrpStatus.m_byGrpId = 0 ;
    NEXTSTATE((int)TMixerGrpStatus::IDLE);//״̬�л�
    m_tGrpStatus.m_byGrpState = (u8)TMixerGrpStatus::IDLE ;
    m_tGrpStatus.m_byMixGrpChnNum = m_byMixMemNum;
    m_byRcvOff = 0 ;
    m_bySndOff = 0 ;
    
    for ( u8 byIndex = 0 ;byIndex < GetInsID() - 1 ;byIndex++)
    {
        m_byRcvOff += g_cEapuMixerApp.m_tEapuCfg.m_tEapuMixerCfg[byIndex].m_byMixerMemberNum;
    }
    m_bySndOff = m_byRcvOff + GetInsID() - 1 ;
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgProcInitalMsg() m_bIsDMode=[%d]m_byMixMemNum=[%d]dwSupAudTypeMaxNum=[%d]m_wRcvStartPort=[%d]m_byRcvOff=[%d]m_bySndOff=[%d]\n",
        GetInsID(),m_bIsDMode,m_byMixMemNum,MAXNUM_MIXER_AUDTYPE,m_tEapuMixerCfg.wRcvStartPort,m_byRcvOff,m_bySndOff);
    TKdvMixerInitParam initParam = { 0 } ;
    if(!m_bIsDMode) ///����ʽ
    {
        initParam.dwAudDecDspID = GetInsID() - 1 ;
        initParam.dwAudEncDspID = GetInsID() - 1 ;
        initParam.dwDModeChMum  = 1 ;//1·Dģʽȫ��
        initParam.dwMaxChNum = m_byMixMemNum;
    }
    else ///���ʽ
    {
        if (GetInsID() == 1)
        {
            initParam.dwAudDecDspID = 0 ;
            initParam.dwAudEncDspID = 1 ;
            initParam.dwDModeChMum  = MAXNUM_MIXER_AUDTYPE ;
        }
        if (GetInsID() == 2)
        {
            initParam.dwAudDecDspID = 3 ;
            initParam.dwAudEncDspID = 2 ;
            initParam.dwDModeChMum  = MAXNUM_MIXER_AUDTYPE ;
        }
        if (GetInsID() == 3)
        {
            initParam.dwAudDecDspID = 3 ;
            initParam.dwAudEncDspID = 2 ;
            initParam.dwDModeChMum  = MAXNUM_MIXER_AUDTYPE ;
        }
    }
    u16 wRet = m_cMixer.Create(initParam);
    if ( wRet != (u16)Codec_Success)//����������
    {
        //����ʧ�ܣ�������
        mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgProcInitalMsg() Call Create() Failed! wRet=[%d]\n", GetInsID(), wRet);
        return;
    }
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgProcInitalMsg() Call Create() Successed!\n",GetInsID());
    wRet =m_cMixer.SetAudEncDataCallback(MixEncFrameCallBack, this);//���ñ���ص�
    if ( wRet != (u16)Codec_Success)//����������
    {
        m_cMixer.Destroy();
        mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgProcInitalMsg() Call SetAudEncDataCallback() Failed! wRet=[%d]\n", GetInsID(), wRet);
        return;
    }
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgProcInitalMsg() Call SetAudEncDataCallback() Successed!\n",GetInsID());
    wRet = m_cMixer.SetVolActiveCallback(EAPUVOLACTCALLBACK, this);
    if(wRet != (u16)Codec_Success)
    {
        m_cMixer.Destroy();
        mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgProcInitalMsg() Call SetVolActiveCallback failed(%d)\n",GetInsID(), wRet);
        return;
    }
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgProcInitalMsg() Call SetVolActiveCallback() Successed!\n",GetInsID());

    //TLocalNetParam tlocalNetParm;
    u8 byRcvIdx = 0;
    u8 bySndIdx = 0;
    for (u8 byChnIdx = 0;byChnIdx < m_byMixMemNum;byChnIdx++)
    {
        byRcvIdx = m_byRcvOff + byChnIdx;
        bySndIdx = m_bySndOff + byChnIdx;
        g_cEapuMixerApp.m_acRcvCB[byRcvIdx].m_byChnnlId = byChnIdx;
        g_cEapuMixerApp.m_acRcvCB[byRcvIdx].m_pcMixerInst = this;
        wRet = g_cEapuMixerApp.m_acAudRcv[byRcvIdx].Create(MAX_AUDIO_FRAME_SIZE, MixNetRcvCallBack, (u32)&g_cEapuMixerApp.m_acRcvCB[byRcvIdx]);
        if(wRet != MEDIANET_NO_ERROR)
        {
            mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgProcInitalMsg() Call Create() Failed! wRet=[%d]\n", GetInsID(), wRet);
            return;
        }
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgProcInitalMsg() Call Create() RCV Successed! wAudType=[%d]dwChnIdx=[%d]\n",
                    GetInsID(),m_byAudType[0],bySndIdx);

		// [miaoqingsong 20111111] Ϊ��ֹ�˿��ظ��󶨣��ڳ�ʼʱ��ͳһ�󶨶˿ڣ��Ⱦ�������ն�ʱ�ٰ󶨣�ע�����´���
        /*memset(&tlocalNetParm, 0, sizeof(TLocalNetParam));
        tlocalNetParm.m_tLocalNet.m_dwRTPAddr = 0;
        tlocalNetParm.m_tLocalNet.m_wRTPPort  = MIXER_EQP_STARTPORT + (u16)(byRcvIdx) * PORTSPAN + 2;//���ؽ��ն˿�
        mixlog(MIXLOG_LEVER_INFO,"info:chn[%d]���ؽ��ն˿�[%d]\n",byChnIdx,tlocalNetParm.m_tLocalNet.m_wRTPPort);
        wRet = g_cEapuMixerApp.m_acAudRcv[byRcvIdx].SetNetRcvLocalParam(tlocalNetParm);
        if(wRet != MEDIANET_NO_ERROR)
        {
            mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgProcInitalMsg() Call SetNetRcvLocalParam() Failed! wRet=[%d]\n", GetInsID(), wRet);
            return;
        }
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgProcInitalMsg() Call SetNetRcvLocalParam() Successed!\n", GetInsID());*/

        wRet = g_cEapuMixerApp.m_acAudSnd[bySndIdx].Create(MAX_AUDIO_FRAME_SIZE, 64<<10, 25, m_byAudType[0]);
        if(wRet != MEDIANET_NO_ERROR)
        {
            mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgProcInitalMsg() Call Create() SND Failed! wRet=[%d]wAudType=[%d]dwChnIdx=[%d]\n",
                    GetInsID(),wRet,m_byAudType[0],bySndIdx);
            return;
        }
		wRet = g_cEapuMixerApp.m_acAudSnd[bySndIdx].SetAudioResend(g_nSendTimes, g_nSendSpan);
        if(wRet != MEDIANET_NO_ERROR)
        {
            mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgProcInitalMsg() Call SetAudioResend() SND Failed! wRet=[%d]dwChnIdx=[%d]\n",
				GetInsID(),wRet,bySndIdx);
            return;
        }
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgProcInitalMsg() Call SetAudioResend() SND Successed! wAudType=[%d]dwChnIdx=[%d]\n",
                    GetInsID(),m_byAudType[0],bySndIdx);
    }
    wRet = g_cEapuMixerApp.m_acAudSnd[m_bySndOff + m_byMixMemNum].Create(MAX_AUDIO_FRAME_SIZE, 64<<10, 25,m_byAudType[0]);
    if(wRet != MEDIANET_NO_ERROR)
    {
        mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgProcInitalMsg() Call Create() Failed! wRet=[%d]\n", GetInsID(), wRet);
        return;
    }
	wRet = g_cEapuMixerApp.m_acAudSnd[m_bySndOff + m_byMixMemNum].SetAudioResend(g_nSendTimes, g_nSendSpan);
	if(wRet != MEDIANET_NO_ERROR)
	{
		mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgProcInitalMsg() Call SetAudioResend() SND Failed! wRet=[%d]dwChnIdx=[%d]\n",
			GetInsID(),wRet,m_bySndOff + m_byMixMemNum);
		return;
    }
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgProcInitalMsg() Call Create() Successed! DModeSendIndex=[%d]\n", 
            GetInsID(),m_bySndOff + m_byMixMemNum);
    // ����Mcu
    if(0 != g_cEapuMixerApp.m_tEapuCfg.m_tEapuMixerCfg[GetInsID() - 1].dwConnectIP && FALSE == g_cEapuMixerApp.m_bEmbed)  // connect to mcu a
    { 
        SetTimer(EV_MIXER_CONNECT, MIX_CONNETC_TIMEOUT); 
    }
    if( 0 != g_cEapuMixerApp.m_tEapuCfg.m_tEapuMixerCfg[GetInsID() - 1].dwConnectIpB && FALSE == g_cEapuMixerApp.m_bEmbedB) // connect to mcu b
    { 
        SetTimer(EV_MIXER_CONNECTB, MIX_CONNETC_TIMEOUT); 
    }
    return;
}
/*====================================================================
    ������      ��GetMediaSndPtr
    ����        ��ȡ�û�������ĳͨ����Ӧ�ķ��Ͷ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
CKdvMediaSnd* CEapuMixInst::GetMediaSndPtr(u32 dwChID, BOOL32 bDMode)
{
    if (bDMode)//����ʽȫ�췢��ͨ��
    {
        return &(g_cEapuMixerApp.m_acAudSnd[m_bySndOff  + m_byMixMemNum]);
    }
    return &(g_cEapuMixerApp.m_acAudSnd[m_bySndOff + dwChID]);
}
/*====================================================================
    ������      ��MsgAddMixMemberProc
    ����        ����������ӻ�����Ա
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::MsgAddMixMemberProc(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    //�Ƿ�����ͬһ������
    if (!(m_cConfId == cServMsg.GetConfId()))
    {
        char achCurConfId[64]  = {0};
        char achStopConfId[64] = {0};
        m_cConfId.GetConfIdString(achCurConfId, sizeof(achCurConfId));
        cServMsg.GetConfId().GetConfIdString(achStopConfId, sizeof(achStopConfId));
        cServMsg.SetErrorCode(ERR_MIXER_ERRCONFID);
        SendMsgToMcu(MIXER_MCU_ADDMEMBER_NACK, cServMsg);
        mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgAddMixMemberProc() Serving %s refuse confId(%s) stop req.\n", GetInsID() , achCurConfId, achStopConfId);
        return;
    }
    u8 *pbyMsgBody = cServMsg.GetMsgBody();
    TMixMember *ptMixMmb = (TMixMember*)(pbyMsgBody + sizeof(u8));
    u8 byChnIdx = cServMsg.GetChnIndex();
	if (g_cEapuMixerApp.m_tEapuCfg.m_byIsSimuAPU)
	{
		byChnIdx = GetMtId2ChnIdx( ptMixMmb->m_tMember.GetMtId(), TRUE );
	} 
    if (byChnIdx + 1 > m_byMixMemNum  )//����������ͨ����
    {
        cServMsg.SetErrorCode(ERR_MIXER_CHNINVALID);
        SendMsgToMcu(MIXER_MCU_ADDMEMBER_NACK, cServMsg);
        mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgAddMixMemberProc() addchn(%d) > maxchnnum(%d)!\n", GetInsID(), byChnIdx + 1, m_byMixMemNum);
        return;
    }
    if (g_cEapuMixerApp.m_bIsChnUsed[m_byRcvOff + byChnIdx])//��ͨ���Ƿ��Ѿ��ڻ�������
    {
        cServMsg.SetErrorCode(ERR_MIXER_CHNINVALID);
        SendMsgToMcu(MIXER_MCU_ADDMEMBER_NACK, cServMsg);
        mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgAddMixMemberProc() chn(%d) is being used!\n",GetInsID(),byChnIdx);
        return;
    }
    TKdvMixerType tType;
    m_abyMtId2ChnIdx[byChnIdx] = ptMixMmb->m_tMember.GetMtId();
    tType.dwChID = byChnIdx;
    tType.bDMode = FALSE;
    tType.byAudType = m_byAudType[0];
    tType.byAudMode = GetAudioMode(m_byAudType[0]) ;
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgAddMixMemberProc() ChnIdx = [%d] MtId = [%d] AudType[0] = [%d] AudType[1] = [%d]\n",
        GetInsID(), byChnIdx,m_abyMtId2ChnIdx[byChnIdx],m_byAudType[0],m_byAudType[1]);
    tType.byAudMode = GetAudioMode(tType.byAudType);
    SetSndObjectPara(ptMixMmb->m_tAddr.GetIpAddr(),ptMixMmb->m_tAddr.GetPort(),m_bySndOff + byChnIdx);    
	
	//���ӽ��ն���rtcpback��ַ
	TLocalNetParam tLocalNetParam;
	memset(&tLocalNetParam, 0, sizeof(TLocalNetParam));
	tLocalNetParam.m_dwRtcpBackAddr = m_dwMcuRcvIp;
	//[2011/08/31/zhangli]�ȴ�Զ��������RTCP�˿�Ӧ��Ϊm_tRtcpBackAddr����m_tAddr
	tLocalNetParam.m_wRtcpBackPort  = ptMixMmb->m_tRtcpBackAddr.GetPort();	
	tLocalNetParam.m_tLocalNet.m_dwRTPAddr = 0;
    tLocalNetParam.m_tLocalNet.m_wRTPPort  =  m_tEapuMixerCfg.wRcvStartPort + byChnIdx * PORTSPAN + 2;//���ؽ��ն˿�
	tLocalNetParam.m_tLocalNet.m_wRTCPPort =  m_tEapuMixerCfg.wRcvStartPort + byChnIdx * PORTSPAN + 2 + 1;
	OspPrintf(TRUE, FALSE, "[MsgAddMixMemberProc] Set RcvChn<%d + %d> tLocalNetParam<m_wRTPPort:%d, m_wRTCPPort:%d, RtcpbackIp:%x, RtcpbackPort:%d>\n", 
						    m_byRcvOff,
							byChnIdx,
							tLocalNetParam.m_tLocalNet.m_wRTPPort,
							tLocalNetParam.m_tLocalNet.m_wRTCPPort,
							tLocalNetParam.m_wRtcpBackPort);

	u16 wRet = g_cEapuMixerApp.m_acAudRcv[m_byRcvOff + byChnIdx].SetNetRcvLocalParam(tLocalNetParam);
	if(wRet != MEDIANET_NO_ERROR)
	{
		mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgAddMixMemberProc() Call SetNetRcvLocalParam() Failed! wRet=[%d]\n", GetInsID(), wRet);
		return;
    }
	else
	{
		OspPrintf(TRUE, FALSE, "[MsgAddMixMemberProc] SetNetRcvLocalParam success!\n");
	}
	wRet = m_cMixer.AddChannel(byChnIdx);//������ͨ���Ŵ�0��ʼ
    if(wRet != (u16)Codec_Success)
    {
        cServMsg.SetErrorCode(ERR_MIXER_CHNINVALID);
        SendMsgToMcu(MIXER_MCU_ADDMEMBER_NACK, cServMsg);
        mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgAddMixMemberProc() call AddChannel(%d) failed(%d)\n",GetInsID(), byChnIdx, wRet);
        return;
    }
    wRet = m_cMixer.SetAudEncParam(tType);
    if(wRet != (u16)Codec_Success)
    {
        cServMsg.SetErrorCode(ERR_MIXER_CHNINVALID);
        SendMsgToMcu(MIXER_MCU_ADDMEMBER_NACK, cServMsg);
        mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgAddMixMemberProc() Call SetAudEncParam(%d) failed(%d)\n", byChnIdx, wRet);
        return;
    }
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgAddMixMemberProc() Call SetAudEncParam() Successed!\n", GetInsID());
    wRet = m_cMixer.SetMixerChVol(FALSE,byChnIdx,min(MAXVALUE_EMIXER_VOLUME/2,ptMixMmb->m_byVolume));
    if(wRet != (u16)Codec_Success)
    {
        mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgAddMixMemberProc() Call SetMixerChVol(%d) failed(%d)\n",GetInsID(), byChnIdx, wRet);
    }
    else
    {
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgAddMixMemberProc() Call SetMixerChVol() Successed!\n",GetInsID());
    }
    SetRcvObjectParaAndStartRcv(m_byRcvOff + byChnIdx);
    m_tGrpStatus.AddMmb((u8)byChnIdx + 1);
    cServMsg.SetErrorCode(0);
    g_cEapuMixerApp.m_bIsChnUsed[m_byRcvOff + byChnIdx] = TRUE;
    SendMsgToMcu(MIXER_MCU_ADDMEMBER_ACK, cServMsg);
    SendGrpNotif();
}

/*====================================================================
    ������      ��GetAudioMode
    ����        ������������Ա���շ��Ͷ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����������˫������MEDIA_TYPE_AACLC��MEDIA_TYPE_AACLD����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
u8 CEapuMixInst::GetAudioMode(u8 byAudioType , BOOL32 bIsSingleChn)
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
        byAudMode = AUDIO_MODE_AACLC_32_M;
        break;
    case MEDIA_TYPE_AACLD://MPEG4 AAL-LD
        byAudMode = AUDIO_MODE_AACLD_32_M;
        break;
    default:
        mixlog(MIXLOG_LEVER_WARN,"GetAudioMode() MediaType[%d] not surportted!!",byAudioType);
        return 0;
    }
    mixlog(MIXLOG_LEVER_INFO,"MediaType = [%d] bIsSingleChn = [%d] AudMode = [%d]\n",
                            byAudioType,bIsSingleChn,byAudMode);
    return byAudMode;
}
/*====================================================================
    ������      ��CreateRcvAndSnd
    ����        ������������Ա���շ��Ͷ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
u16 CEapuMixInst::SetRcvObjectParaAndStartRcv(u8 byChnIdx)
{
    u16 wRet = 0;
    u8 byEncryptMode;
    u8 abyKeyBuf[MAXLEN_KEY];
    s32 byKenLen = 0;
    memset(abyKeyBuf, 0, MAXLEN_KEY);
    byEncryptMode = m_tMediaEncrypt.GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
        mixlog(MIXLOG_LEVER_INFO, "[SetDecryptParam]Set key to NULL!\n");
        g_cEapuMixerApp.m_acAudRcv[byChnIdx].SetDecryptKey( NULL, 0, 0 );
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
        g_cEapuMixerApp.m_acAudRcv[byChnIdx].SetDecryptKey( (s8*)abyKeyBuf, (u16)byKenLen, byEncryptMode );
    }
    u8 byRealPayload = m_tDoublePayload.GetRealPayLoad();
    u8 byActivePayload = m_tDoublePayload.GetActivePayload();
    g_cEapuMixerApp.m_acAudRcv[byChnIdx].SetActivePT( byActivePayload, byRealPayload );
    mixlog(MIXLOG_LEVER_INFO, "[SetDecryptParam]Mode: %u KeyLen: %u PT: %d AT: %d KeyBuf [%s]\n",
                m_tMediaEncrypt.GetEncryptMode(), 
                byKenLen, 
                byRealPayload ,byActivePayload,abyKeyBuf);
    if (m_bNeedPrs)
    {
        TRSParam tNetRSParam ;
        tNetRSParam.m_wFirstTimeSpan  = m_tNetRSParam.m_wFirstTimeSpan;
        tNetRSParam.m_wSecondTimeSpan = m_tNetRSParam.m_wSecondTimeSpan;
        tNetRSParam.m_wThirdTimeSpan  = m_tNetRSParam.m_wThirdTimeSpan;
        tNetRSParam.m_wRejectTimeSpan = m_tNetRSParam.m_wRejectTimeSpan;
		OspPrintf(TRUE, FALSE, "[SetRcvObjectParaAndStartRcv1]m_wFirstTimeSpan:%d, m_wSecondTimeSpan:%d, m_wThirdTimeSpan:%d, m_wRejectTimeSpan:%d!\n",
					m_tNetRSParam.m_wFirstTimeSpan,m_tNetRSParam.m_wSecondTimeSpan, m_tNetRSParam.m_wThirdTimeSpan,  m_tNetRSParam.m_wRejectTimeSpan
					);
        wRet = g_cEapuMixerApp.m_acAudRcv[byChnIdx].ResetRSFlag( tNetRSParam, TRUE );//���ý����ش�ʱ����
        if(wRet != MEDIANET_NO_ERROR)
        {
            mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgProcInitalMsg() Call ResetRSFlag() Failed! wRet=[%d]\n", GetInsID(), wRet);
        }
        else
        {
			mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgProcInitalMsg() Call ResetRSFlag() Successed!\n", GetInsID());
        }
    }
    else
    {
        TRSParam tNetRSParam ;
        tNetRSParam.m_wFirstTimeSpan  = 0;
        tNetRSParam.m_wSecondTimeSpan = 0;
        tNetRSParam.m_wThirdTimeSpan  = 0;
        tNetRSParam.m_wRejectTimeSpan = 0;
		OspPrintf(TRUE, FALSE, "[SetRcvObjectParaAndStartRcv2]m_wFirstTimeSpan:%d, m_wSecondTimeSpan:%d, m_wThirdTimeSpan:%d, m_wRejectTimeSpan:%d!\n",
			m_tNetRSParam.m_wFirstTimeSpan,m_tNetRSParam.m_wSecondTimeSpan, m_tNetRSParam.m_wThirdTimeSpan,  m_tNetRSParam.m_wRejectTimeSpan
					);
        wRet = g_cEapuMixerApp.m_acAudRcv[byChnIdx].ResetRSFlag( tNetRSParam, TRUE );//���ý����ش�ʱ����
        if(wRet != MEDIANET_NO_ERROR)
        {
            mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgProcInitalMsg() Call ResetRSFlag() Failed! wRet=[%d]\n", GetInsID(), wRet);
        }
        else
        {
			mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgProcInitalMsg() Call ResetRSFlag() Successed!\n", GetInsID());
        }
    }
    wRet = g_cEapuMixerApp.m_acAudRcv[byChnIdx].StartRcv();
    if (MEDIANET_NO_ERROR != wRet)
    {
        mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgProcInitalMsg() Call StartRcv() failed(%d)!\n", GetInsID(), wRet);
    }
    else
    {
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgProcInitalMsg() Call StartRcv() Successed!!\n", GetInsID());
    }
    return 0;
}
/*====================================================================
    ������      ��CreateRcvAndSnd
    ����        ������������Ա���շ��Ͷ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
u16 CEapuMixInst::SetSndObjectPara(u32 dwDstIp,u16 wDstPort,u8 byChnIdx)
{
    u16 wRet = 0;
    wRet = g_cEapuMixerApp.m_acAudSnd[byChnIdx].Create(MAX_AUDIO_FRAME_SIZE, 64<<10, 25,m_byAudType[0]);
    if(wRet != MEDIANET_NO_ERROR)
    {
        mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]SetSndObjectPara() Call Create() Failed! wRet=[%d]\n", GetInsID(), wRet);
        //return wRet;
    }
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]SetSndObjectPara() Call Create() Successed! SendIndex=[%d]\n", 
                GetInsID(),byChnIdx);

    //���ö�Ӧ�ķ��Ͷ���
    TNetSndParam tNetSndPar;
    memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );
    tNetSndPar.m_byNum  = 1;
    tNetSndPar.m_tLocalNet.m_wRTPPort       = BRD_EAPU_SNDPORT + byChnIdx * PORTSPAN + 2;//���Ͷ˿�
    tNetSndPar.m_tLocalNet.m_wRTCPPort      = BRD_EAPU_SNDPORT + byChnIdx * PORTSPAN + 2 + 1;//Զ��δ�յ�ʱ���������صĶ˿�
    tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = dwDstIp;
    tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = wDstPort;//Զ�˽��ն˿�
    wRet = g_cEapuMixerApp.m_acAudSnd[byChnIdx].SetNetSndParam(tNetSndPar);
    if(wRet != MEDIANET_NO_ERROR)
    {
        mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]CreateSndObject() Call SetNetSndParam() SND Failed!LocRTPSndPort��[%d]LocRTCPSndPort��[%d]RemoteRcvIP��[%s]RemoteRcvPort��[%d] wret=[%d]\n",
        GetInsID(),tNetSndPar.m_tLocalNet.m_wRTPPort,tNetSndPar.m_tLocalNet.m_wRTCPPort,
        StrOfIP(tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr),tNetSndPar.m_tRemoteNet[0].m_wRTPPort,wRet);
        //return wRet;
    }
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]CreateSndObject() Call SetNetSndParam() SND Successed!LocRTPSndPort��[%d]LocRTCPSndPort��[%d]RemoteRcvIP��[%s]RemoteRcvPort��[%d]\n",
        GetInsID(),tNetSndPar.m_tLocalNet.m_wRTPPort,tNetSndPar.m_tLocalNet.m_wRTCPPort,
        StrOfIP(tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr),tNetSndPar.m_tRemoteNet[0].m_wRTPPort);
    wRet = g_cEapuMixerApp.m_acAudSnd[byChnIdx].ResetRSFlag( 2000, TRUE );//���÷����ش�ʱ����
    if(wRet != MEDIANET_NO_ERROR)
    {
        mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]CreateSndObject() Call ResetRSFlag() Failed! wRet=[%d]\n", GetInsID(), wRet);
    }
    else
    {
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]CreateSndObject() Call ResetRSFlag() Successed!\n", GetInsID());
    }
    u8  abyKeyBuf[MAXLEN_KEY];
    memset( abyKeyBuf, 0, MAXLEN_KEY );
    s32 nKeyLen = 0;
    u8 byEncryptMode = m_tMediaEncrypt.GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
        mixlog(MIXLOG_LEVER_INFO, "[Info]we set key to NULL! (SetEncryptParam)\n" );
     
        g_cEapuMixerApp.m_acAudSnd[byChnIdx].SetEncryptKey( NULL, 0, 0 );
    }
    else if ( CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode ) // ���������
    {
        if ( CONF_ENCRYPTMODE_DES == byEncryptMode ) // ���²�궨��ת��
        {
            byEncryptMode = DES_ENCRYPT_MODE;
        }
        else
        {
            byEncryptMode = AES_ENCRYPT_MODE;
        }
        m_tMediaEncrypt.GetEncryptKey( abyKeyBuf, &nKeyLen );

        g_cEapuMixerApp.m_acAudSnd[byChnIdx].SetEncryptKey( (s8*)abyKeyBuf, (u16)nKeyLen, byEncryptMode );
    }
    g_cEapuMixerApp.m_acAudSnd[byChnIdx].SetActivePT( m_tDoublePayload.GetActivePayload() );

    mixlog(MIXLOG_LEVER_INFO, "[Info]Mode: %u KeyLen: %u RealPT: %u ActivePT:%d\n",
                m_tMediaEncrypt.GetEncryptMode(), 
                nKeyLen, 
                m_tDoublePayload.GetRealPayLoad() ,m_tDoublePayload.GetActivePayload());
	wRet = g_cEapuMixerApp.m_acAudSnd[byChnIdx].SetAudioResend(g_nSendTimes, g_nSendSpan);
	if(wRet != MEDIANET_NO_ERROR)
    {
        mixlog(MIXLOG_LEVER_WARN,"[SetSndObjectPara]Mixer[%d]SetAudioResend() Failed! wRet=[%d]\n", GetInsID(), wRet);
    }
	else
	{
		mixlog(MIXLOG_LEVER_INFO,"[SetSndObjectPara]Mixer[%d] Chn:(%d)SetAudioResend(%d,%d) Successed!\n", GetInsID(),byChnIdx,g_nSendTimes, g_nSendSpan);
	}
    return 0;
}
/*====================================================================
    ������      ��MsgRemoveMixMemberPorc
    ����        ������ӻ��������Ƴ�������Ա
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::MsgRemoveMixMemberPorc(CMessage *const pMsg)
{
    //ֻҪͨ���ż���
    CServMsg cServMsg(pMsg->content,pMsg->length);
    TMixMember *ptMixMmb;
    ptMixMmb =(TMixMember*)(cServMsg.GetMsgBody() + sizeof(u8));
	u8 byChnIdx = cServMsg.GetChnIndex() ;
	if (g_cEapuMixerApp.m_tEapuCfg.m_byIsSimuAPU)
	{	
		for( u8 byLp = 0; byLp < MAXNUM_APU_MIXING_MEMBER; byLp++ )
		{
			if( m_abyMtId2ChnIdx[byLp] == ptMixMmb->m_tMember.GetMtId() )
			{
				byChnIdx = byLp;
				m_abyMtId2ChnIdx[byLp] = 0;
				break;
			}
		}
	}

    
    if ( !(m_cConfId == cServMsg.GetConfId()) || (VALUE_INVALID == byChnIdx) ) //����
    {
        s8 achCurConfId[64] ={0};
        s8 achStopConfId[64] ={0};
        m_cConfId.GetConfIdString(achCurConfId, sizeof(achCurConfId));
        cServMsg.GetConfId().GetConfIdString(achStopConfId, sizeof(achStopConfId));
        cServMsg.SetErrorCode(ERR_MIXER_ERRCONFID);
        SendMsgToMcu(MIXER_MCU_REMOVEMEMBER_NACK, cServMsg);
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d] Serving %s refuse confId(%s) remove req.\n",GetInsID(), achCurConfId, achStopConfId);
        return;
    }
    u16 wRet = g_cEapuMixerApp.m_acAudRcv[m_byRcvOff + byChnIdx].StopRcv();
    if(wRet != MEDIANET_NO_ERROR)
    {
        mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]FreeAllRcv() Call StopRcv() Failed! wRet=[%d]\n", GetInsID(), wRet);
        //return wRet;
    }
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]FreeAllRcv() Call StopRcv() Successed!\n", GetInsID());
    wRet = m_cMixer.DelChannel(byChnIdx);
    if ((u16)Codec_Success != wRet)
    {
        cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
        SendMsgToMcu(MIXER_MCU_REMOVEMEMBER_NACK, cServMsg);
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgRemoveMixMemberPorc() Call DelChannel() Failed(%d)!\n", GetInsID(), wRet);
        return;
    }
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgRemoveMixMemberPorc() Call DelChannel(%d) Successed()!\n", GetInsID(),byChnIdx);
    wRet = g_cEapuMixerApp.m_acAudSnd[m_bySndOff + byChnIdx].RemoveNetSndLocalParam();//�ͷŵײ��׽���
    if (MEDIANET_NO_ERROR != wRet)
    {
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgRemoveMixMemberPorc() Call RemoveNetSndLocalParam failed(%d)!byChnIdx[%d]\n", GetInsID() , wRet,byChnIdx);
        return;
    }
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgRemoveMixMemberPorc() Call RemoveNetSndLocalParam Successed!!byChnIdx[%d]\n", GetInsID() ,byChnIdx);
    m_abyMtId2ChnIdx[byChnIdx] = 0 ;
    m_tGrpStatus.RemoveMmb((u8)byChnIdx + 1);
    g_cEapuMixerApp.m_bIsChnUsed[m_byRcvOff + byChnIdx] = FALSE;
    cServMsg.SetErrorCode(0);
    SendMsgToMcu(MIXER_MCU_REMOVEMEMBER_ACK, cServMsg);
    SendGrpNotif();
    return;
}

/*====================================================================
    ������      ��FreeRcvAndSnd
    ����        ����������Ա��Ӧ�ķ��ͽ��ն����ͷ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
u16 CEapuMixInst::FreeAllRcv()
{
    u16 wRet = 0 ;
    for (u16 wIndex = 0;wIndex < m_byMixMemNum ; wIndex++)
    {
        if (g_cEapuMixerApp.m_bIsChnUsed[m_byRcvOff + wIndex])
        {
            wRet = g_cEapuMixerApp.m_acAudRcv[m_byRcvOff + wIndex].StopRcv();
            if(wRet != MEDIANET_NO_ERROR)
            {
                mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]FreeAllRcv() Call StopRcv() Failed! wRet=[%d]\n", GetInsID(), wRet);
                //return wRet;
            }
            mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]FreeAllRcv() Call StopRcv() Successed!\n", GetInsID());
            wRet = m_cMixer.DelChannel(wIndex);
            if ((u16)Codec_Success != wRet)
            {
                mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]FreeAllRcv() call DelChannel(%d) failed(%d)!\n", GetInsID() ,wIndex, wRet);
                //return wRet;
            }
            mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]FreeAllRcv() call DelChannel(%d) successed()!\n", GetInsID() ,wIndex);
            m_tGrpStatus.RemoveMmb((u8)wIndex + 1);
            g_cEapuMixerApp.m_bIsChnUsed[m_byRcvOff + wIndex] = FALSE;
            wRet = g_cEapuMixerApp.m_acAudSnd[m_bySndOff + wIndex].RemoveNetSndLocalParam();//�ͷŵײ��׽���
            if (MEDIANET_NO_ERROR != wRet)
            {
                mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]FreeAllRcv() Call RemoveNetSndLocalParam failed(%d)!byChnIdx[%d]\n", GetInsID() , wRet,m_bySndOff + wIndex);
                //return;
            }
			m_abyMtId2ChnIdx[wIndex] = 0;
            mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]FreeAllRcv() Call RemoveNetSndLocalParam Successed!!byChnIdx[%d]\n", GetInsID() ,m_bySndOff + wIndex);
        }
    }
    return 0;
}
/*====================================================================
    ������      ��MsgSetMixDepthProc
    ����        �����û��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::MsgSetMixDepthProc(CMessage *const pMsg)
{
    CServMsg cServMsg(pMsg->content,pMsg->length);
    u8 * pbyMsgBody = cServMsg.GetMsgBody();
    u8 byGrpId     = *pbyMsgBody;
    u8 byDepth  = *(pbyMsgBody + sizeof(u8));
    if(!(m_cConfId == cServMsg.GetConfId()))
    {
        s8 achCurConfId[64] ={0};
        s8 achStopConfId[64] ={0};
        m_cConfId.GetConfIdString(achCurConfId, sizeof(achCurConfId));
        cServMsg.GetConfId().GetConfIdString(achStopConfId, sizeof(achStopConfId));
        cServMsg.SetErrorCode(ERR_MIXER_ERRCONFID);
        SendMsgToMcu(MIXER_MCU_SETMIXDEPTH_NACK, cServMsg);
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d] Serving %s refuse confId(%s) stop req.\n", GetInsID() - 1, achCurConfId, achStopConfId);
        return;
    }
    m_tGrpStatus.m_byGrpMixDepth = min(byDepth,MAXNUM_EMIXER_DEPTH);
    u16 wRet = m_cMixer.SetMixDepth(byDepth);
    if ((u16)Codec_Success != wRet)
    {
        cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
        SendMsgToMcu(MIXER_MCU_SETMIXDEPTH_NACK, cServMsg);
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgSetMixDepthProc() Call SetMixDepth() Failed! wRet = [%d]\n", byGrpId, wRet);
        return;
    }
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgSetMixDepthProc() Call SetMixDepth() Successed!\n", byGrpId);
    cServMsg.SetErrorCode(0);
    SendMsgToMcu(MIXER_MCU_SETMIXDEPTH_ACK, cServMsg);
    SendGrpNotif();
}
void CEapuMixInst::MsgStartVacProc(CMessage *const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    u16 wRet = 0;
    switch(CurState())
    {
    case TMixerGrpStatus::IDLE:   
        mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgStartVacProc() at IDLE state recv [%d] cmd.\n", GetInsID());
        return;
    case TMixerGrpStatus::READY: 
        {
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgStartVacProc() DEAL MCU_MIXER_STARTVAC_CMD Msg\n", GetInsID());
        TMixerStart tMixerStart = *(TMixerStart*)cServMsg.GetMsgBody();
        m_tMediaEncrypt = tMixerStart.GetAudioEncrypt();    
        m_tDoublePayload = *(TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(TMixerStart));
        m_tCapSupportEx = *(TCapSupportEx*)(cServMsg.GetMsgBody() + sizeof(TMixerStart) + sizeof(TDoublePayload));
        u8 byDepth = tMixerStart.GetMixDepth();
        m_byAudType[0] =  tMixerStart.GetAudioMode();
        m_byAudType[1] =  tMixerStart.GetSecAudioMode();
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgStartVacProc() MainMediaType = [%d] SlaveMediaType = [%d] MixDepth: [%d] \n", 
                          GetInsID(),m_byAudType[0] ,   m_byAudType[1] ,  byDepth );
        m_bNeedPrs = tMixerStart.IsNeedByPrs();
        m_tGrpStatus.m_byGrpMixDepth = min(byDepth,MAXNUM_EMIXER_DEPTH);
        wRet = m_cMixer.StartVolActive();
        if(wRet != (u16)Codec_Success)
        {
            mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgStartVacProc() Call StartVolActive() Failed! wRet = [%d]\n", GetInsID(),wRet);
            return;
        }
        m_cConfId = cServMsg.GetConfId();//��¼�û���������Ļ�����Ϣ
        NEXTSTATE((u16)TMixerGrpStatus::MIXING);//������״̬�л�
        m_tGrpStatus.m_byGrpState = (u8)TMixerGrpStatus::MIXING;
        m_bIsStartVac = TRUE;
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgStartVacProc() Call StartVolActive() Successed!\n", GetInsID());
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d] STATE : [READY ----> MIXING]\n", GetInsID());
        return;
        }
    case TMixerGrpStatus::MIXING:  
        if (!(m_cConfId == cServMsg.GetConfId()))
        {
            char achCurConfId[64]  = {0};
            char achStopConfId[64] = {0};
            m_cConfId.GetConfIdString(achCurConfId, sizeof(achCurConfId));
            cServMsg.GetConfId().GetConfIdString(achStopConfId, sizeof(achStopConfId));
            mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgStartVacProc() Serving %s refuse confId(%s) stop req.\n", GetInsID() , achCurConfId, achStopConfId);
            return;
        }
        if (m_bIsStartVac)
        {
            mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgStartVacProc() Call StartVolActive() Failed! vacing\n", GetInsID());
            return;
        }
        else
        {
            wRet = m_cMixer.StartVolActive();
            if(wRet != (u16)Codec_Success)
            {
                mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgStartVacProc() Call StartVolActive() Failed! wRet = [%d]\n", GetInsID(),wRet);
                return;
            }
            m_bIsStartVac = TRUE;
            mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgStartVacProc() Call StartVolActive() Successed!\n", GetInsID());
            return;
        }
    default:
        break;
    }
}
void CEapuMixInst::MsgStopVacProc(CMessage *const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    u16 wRet = 0;
    switch(CurState())
    {
    case TMixerGrpStatus::IDLE:   
    case TMixerGrpStatus::READY: 
        mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgStopVacProc() at IDLE or READY state recv MCU_MIXER_STOPVAC_CMD.\n", GetInsID());
        return;

    case TMixerGrpStatus::MIXING:  
        if (!(m_cConfId == cServMsg.GetConfId()))
        {
            char achCurConfId[64]  = {0};
            char achStopConfId[64] = {0};
            m_cConfId.GetConfIdString(achCurConfId, sizeof(achCurConfId));
            cServMsg.GetConfId().GetConfIdString(achStopConfId, sizeof(achStopConfId));
            mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgStopVacProc() Serving %s refuse confId(%s) stop req.\n", GetInsID() , achCurConfId, achStopConfId);
            return;
        }
        if (!m_bIsStartVac)
        {
            mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgStopVacProc() is not vacing \n", GetInsID());
            return;
        }
        wRet = m_cMixer.StopVolActive();
        if(wRet != (u16)Codec_Success)
        {
            mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgStopVacProc() Call MsgStopVacProc() Failed! wRet = [%d]\n", GetInsID(),wRet);
            return;
        }
        m_bIsStartVac = FALSE;
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgStartVacProc() Call MsgStopVacProc() Successed!\n", GetInsID());
        if (!m_bIsStartMix)
        {
            FreeAllRcv();
            m_tGrpStatus.m_byGrpMixDepth = 0;
            m_tGrpStatus.m_byConfId = 0;
            m_cConfId.SetNull();//��¼�û���������Ļ�����Ϣ
            NEXTSTATE((u16)TMixerGrpStatus::READY);//������״̬�л�
            m_tGrpStatus.m_byGrpState = (u8)TMixerGrpStatus::READY;
            mixlog(MIXLOG_LEVER_INFO,"Mixer[%d] STATE : [MIXING ----> READY]\n", GetInsID());
        }
        return;
    default:
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d] STATE ERROR : [%d]\n", GetInsID(),CurState());
        break;
    }
}
/*====================================================================
    ������      ��MsgSetVolumeProc
    ����        �����û�����Ա����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::MsgSetVolumeProc(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    u8 *pbyMsgBody = (u8*)cServMsg.GetMsgBody();
    u8 byGrpId   = *pbyMsgBody;
    TMixMember* ptMixMmb  = (TMixMember*)(pbyMsgBody + sizeof(u8));
    u8 byMtId = ptMixMmb->m_tMember.GetMtId();
    u8 byChnlIdx = cServMsg.GetChnIndex();
    if( 0 == byChnlIdx )
    {
        OspPrintf(TRUE, FALSE, "[AMix] Error Mt id= %d.\n", ptMixMmb->m_tMember.GetMtId() );
        return;
    }
    OspPrintf(TRUE, FALSE, "[MsgSetVolumeProc] mt id= %d. idle channel= %d.\n", byMtId, byChnlIdx );

    u8 byVol  = ptMixMmb->m_byVolume;
    if (m_abyMtId2ChnIdx[byChnlIdx] != 0)//�ڻ����б���
    {
        u16 wRet = m_cMixer.SetMixerChVol(FALSE,byChnlIdx,byVol);
        if ((u16)Codec_Success != wRet)
        {
            OspPrintf(TRUE, FALSE, "[AMix] Grp.%d set chn.%d volume(%d) failed! (%d).\n", byGrpId, byChnlIdx, byVol, wRet);
            return;
        }
    } 
    else//û�ڻ����б�����ΪNȫ��
    {
        if (byVol >= 30)
        {
            byVol = MAXVALUE_EMIXER_VOLUME/2;
        }
        u16 wRet = m_cMixer.SetMixerChVol(TRUE,0,byVol);
        if ((u16)Codec_Success != wRet)
        {
            OspPrintf(TRUE, FALSE, "[AMix] Grp.%d set chn.%d volume(%d) failed! (%d).\n", byGrpId, byChnlIdx, byVol, wRet);
            return;
        }
    }
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgSetVolumeProc() Call SetMixerChVol() Successed!\n", byGrpId);
    cServMsg.SetMsgBody((u8*)ptMixMmb, sizeof(TMixMember));
    SendMsgToMcu(MIXER_MCU_CHNNLVOL_NOTIF, cServMsg);
    return;
}
/*====================================================================
    ������      ��MsgSetMixSendProc
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::MsgSetMixSendProc(CMessage *const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    u8 *pbyMsgBody = (u8*)cServMsg.GetMsgBody();
    u8 byGrpId    = *pbyMsgBody;
    u8 bySend     = *(pbyMsgBody + sizeof(u8));
    BOOL32 bSend = (1 == bySend)? FALSE: TRUE;
    mixlog(MIXLOG_LEVER_INFO,"Grp.%d call stop mixer send. para=%d \n", byGrpId, bSend);
    u16 wRet = 0;
    if (m_bIsStartVac && !m_bIsStartMix && !bSend)
    {
        m_bIsStartMix = TRUE;
        TKdvMixerType tType;
        tType.dwChID = 0;//���ʽҪ�޸�
        tType.bDMode = TRUE;
        tType.byAudType = m_byAudType[0];
        tType.byAudMode = GetAudioMode(m_byAudType[0]) ;
        wRet = m_cMixer.SetDModeChannel(tType);
        if ((u16)Codec_Success != wRet)
        {
            mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgSetMixSendProc() Call SetDModeChannel() FAILED wRet=[%d].\n", GetInsID(),wRet);
        }
        else
        {
            mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgSetMixSendProc() Call SetDModeChannel() Successed!\n", GetInsID());
        }
        wRet = m_cMixer.SetMixerChVol(TRUE,0,MAXVALUE_EMIXER_VOLUME/2);
        if(wRet != (u16)Codec_Success)
        {
            mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgSetMixSendProc() Call SetMixerChVol(dmode) failed(%d)\n",GetInsID(), wRet);
        }
        else
        {
            mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgSetMixSendProc() Call SetMixerChVol() Successed!\n",GetInsID());
        }
        wRet = m_cMixer.StartAudMix();
        if ( wRet != (u16)Codec_Success )
        {
            mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgSetMixSendProc() at MIXING state Call StartAudMix() Failed! wRet = [%d]\n", GetInsID(),wRet);
            return;
        }
        mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgSetMixSendProc() at MIXING state Call StartAudMix() Successed!!\n", GetInsID());
    }
    if (m_bIsStartVac && m_bIsStartMix && bSend)
    {
        m_bIsStartMix = FALSE;
        wRet = m_cMixer.DelDModeChannel(0);
        if (wRet != (u16)Codec_Success)
        {
            mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgSetMixSendProc() call DeleteDmodeChnAndDModeSndObject() Failed!\n", GetInsID());
            return;
        }
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgSetMixSendProc() call DeleteDmodeChnAndDModeSndObject() Successed!\n", GetInsID());
        wRet = m_cMixer.StopAudMix();
        if (wRet != (u16)Codec_Success)
        {
            mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgSetMixSendProc() call StopAudMix() Failed!\n", GetInsID());
            return;
        }
		mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgSetMixSendProc() call StopAudMix() Successed!\n", GetInsID());
    }
    return;
}
/*====================================================================
    ������      ��MsgForceActiveProc
    ����        ��ǿ�Ƽ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::MsgForceActiveProc(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    
    u8 byChnlIdx = cServMsg.GetChnIndex();
    u16 wRet = m_cMixer.SetForceChannel(byChnlIdx);
    if ((u16)CODEC_NO_ERROR != wRet)
    {
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgForceActiveProc() Call SetForceChannel() Failed! wRet = [%d]\n", GetInsID() - 1, wRet);
        cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
        SendMsgToMcu(MIXER_MCU_FORCEACTIVE_NACK, cServMsg);
    }
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgForceActiveProc() Call SetForceChannel() Successed!\n", GetInsID() - 1);
    cServMsg.SetErrorCode(0);
    SendMsgToMcu(MIXER_MCU_FORCEACTIVE_ACK, cServMsg);
    return;    
}
/*====================================================================
    ������      ��MsgCancelForceActiveProc
    ����        ��ȡ��ǿ�Ƽ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::MsgCancelForceActiveProc(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    u16 wRet = m_cMixer.SetForceChannel( MAXNUM_MIXER_CHNNL );///ȡ��ǿ�ƻ���
    if ((u16)CODEC_NO_ERROR != wRet)
    {
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgCancelForceActiveProc() Call SetForceChannel() Failed!Cancel wRet = [%d]\n", GetInsID() - 1, wRet);
        cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
        SendMsgToMcu(MIXER_MCU_CANCELFORCEACTIVE_NACK, cServMsg);
        return;        
    }
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgCancelForceActiveProc() Call SetForceChannel() Successed!Cancel\n", GetInsID() - 1);
    cServMsg.SetErrorCode(0);
    SendMsgToMcu(MIXER_MCU_CANCELFORCEACTIVE_ACK, cServMsg);
    return;    
}
/*====================================================================
    ������      ��ProcConnectMcuTimeOut
    ����        ���������ӳ�ʱ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::ProcConnectMcuTimeOut(BOOL32 bIsConnectA)
{
    BOOL32 bRet = FALSE;
    if(TRUE == bIsConnectA)
    {
        bRet = ConnectMcu(bIsConnectA, g_cEapuMixerApp.m_dwMcuNode);
        if(TRUE == bRet)
        { 
			::OspNodeDiscCBRegQ(g_cEapuMixerApp.m_dwMcuNode, GetAppID(), GetInsID());
        	post(MAKEIID(GetAppID(),GetInsID()),EV_MIXER_REGISTER);
        }
        else
        {
            SetTimer(EV_MIXER_CONNECT, MIX_CONNETC_TIMEOUT);    //��ʱ����
        }
    }
    else
    {
        bRet = ConnectMcu(bIsConnectA, g_cEapuMixerApp.m_dwMcuNodeB);
        if(TRUE == bRet)
        {  
			::OspNodeDiscCBRegQ(g_cEapuMixerApp.m_dwMcuNodeB, GetAppID(), GetInsID());
            post(MAKEIID(GetAppID(),GetInsID()),EV_MIXER_REGISTERB);
        }
        else
        {
            SetTimer(EV_MIXER_CONNECTB, MIX_CONNETC_TIMEOUT);    //��ʱ����
        }
    }
    return;
}
/*====================================================================
    ������      ��ConnectMcu
    ����        ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
BOOL32 CEapuMixInst::ConnectMcu(BOOL32 bIsConnectA, u32& dwMcuNode)
{
    BOOL32 bRet = TRUE;
    if (!OspIsValidTcpNode(dwMcuNode))   
    {   
        if(TRUE == bIsConnectA)
        {   
            dwMcuNode = BrdGetDstMcuNode();  //�Ӵ�������������MCU���ӵĽڵ��,
        }
        else
        {    
            dwMcuNode = BrdGetDstMcuNodeB(); //�Ӵ����������뱸MCU���ӵĽڵ��
        }

        if (::OspIsValidTcpNode(dwMcuNode))
        {
            mixlog(MIXLOG_LEVER_INFO,"Connect Mcu Success!\n");
            ::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), CInstance::DAEMON);// ��DAEMONʵ�����������Ϣ
        }
        else 
        {
            bRet = FALSE;
        }
    }
    return bRet;

}
/*====================================================================
    ������      ��ProcRegisterTimeOut
    ����        ������ע�ᳬʱ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::ProcRegisterTimeOut(BOOL32 bIsRegisterA)
{
    if(bIsRegisterA)
    {
        Register(g_cEapuMixerApp.m_dwMcuNode);
        SetTimer(EV_MIXER_REGISTER, MIX_REGISTER_TIMEOUT);
    }
    else
    {
        Register(g_cEapuMixerApp.m_dwMcuNodeB);
        SetTimer(EV_MIXER_REGISTERB, MIX_REGISTER_TIMEOUT);
    }
    return;
}
/*====================================================================
    ������      ��Register
    ����        ��ע��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::Register(u32& dwMcuNode)
{
    CServMsg cSvrMsg;
    TPeriEqpRegReq tReg;
    tReg.SetEqpId(m_tEapuMixerCfg.byEqpId);
    tReg.SetEqpType(m_tEapuMixerCfg.byEqpType);
    tReg.SetEqpAlias(m_tEapuMixerCfg.achAlias);
    tReg.SetPeriEqpIpAddr(m_tEapuMixerCfg.dwLocalIP);
    tReg.SetStartPort(m_tEapuMixerCfg.wRcvStartPort);
	
	if(g_cEapuMixerApp.m_tEapuCfg.m_byIsSimuAPU)
	{
		tReg.SetVersion(DEVVER_MIXER_R3FULL);//R3_FULL 4301
	}
	else
	{
		tReg.SetVersion(DEVVER_EMIXER);
	}

    tReg.SetMcuId((u8)LOCAL_MCUID);
    cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
    //��MCU�˷���ע����Ϣ
    post( MAKEIID(AID_MCU_PERIEQPSSN, (u8)(m_tEapuMixerCfg.byEqpId)), MIXER_MCU_REG_REQ,
            cSvrMsg.GetServMsg(), cSvrMsg.GetServMsgLen(), dwMcuNode );
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]Register() to Mcu%d dwMcuNode=[%d]wRcvStartPort=[%d]....\n",GetInsID(),
            m_tRegAckInfo.m_dwMcuIId,dwMcuNode,m_tEapuMixerCfg.wRcvStartPort);
    mixlog(MIXLOG_LEVER_INFO,"Ip: %0x, IpB: %0x\n", m_tEapuMixerCfg.dwConnectIP,
            m_tEapuMixerCfg.dwConnectIpB);
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]Register() A's node: %d, B's node: %d\n",GetInsID(), dwMcuNode, dwMcuNode);
    return;
}
/*====================================================================
    ������      ��MsgRegAckProc
    ����        ������ע��ȷ����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����a
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::MsgRegAckProc(CMessage* const pMsg)
{
    TPeriEqpRegAck* ptRegAck = NULL;
    CServMsg cServMsg(pMsg->content, pMsg->length);
    ptRegAck =(TPeriEqpRegAck*)cServMsg.GetMsgBody();
    if( g_cEapuMixerApp.m_dwMcuNode == pMsg->srcnode)// ע��ɹ�
    {
        m_dwMcuRcvIp = ptRegAck->GetMcuIpAddr();
        m_wMcuRcvStartPort = ptRegAck->GetMcuStartPort();
        m_tRegAckInfo.m_dwMcuIId = pMsg->srcid;
        m_tRegAckInfo.m_tPrsTimeSpan = *(TPrsTimeSpan *)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck));
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgRegAckProc() send to AMCU m_dwMcuIId=[%d]\n",GetInsID(),m_tRegAckInfo.m_dwMcuIId);
        m_tRegAckInfo.m_byRegAckNum ++;
        KillTimer(EV_MIXER_REGISTER);
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgRegAckProc() Register A's Mcu%d  success !\n",GetInsID(), m_tRegAckInfo.m_dwMcuIId);
    }
    else if( g_cEapuMixerApp.m_dwMcuNodeB == pMsg->srcnode )
    {
        m_dwMcuRcvIpB = ptRegAck->GetMcuIpAddr();
        m_wMcuRcvStartPortB = ptRegAck->GetMcuStartPort();
        m_tRegAckInfo.m_dwMcuIIdB = pMsg->srcid;
        m_tRegAckInfo.m_byRegAckNum ++;
        KillTimer(EV_MIXER_REGISTERB);
        mixlog(MIXLOG_LEVER_INFO,"Register B's Mcu%d  success !\n", m_tRegAckInfo.m_dwMcuIIdB);
    }
    if ( m_tRegAckInfo.m_dwMpcSSrc == 0 )
    {
        m_tRegAckInfo.m_dwMpcSSrc = ptRegAck->GetMSSsrc();
    }
    else
    {
        // �쳣������Ͽ������ڵ�
        if ( m_tRegAckInfo.m_dwMpcSSrc != ptRegAck->GetMSSsrc() )
        {
            mixlog(MIXLOG_LEVER_INFO,"[MsgRegAckProc] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
                      m_tRegAckInfo.m_dwMpcSSrc, ptRegAck->GetMSSsrc());
            if ( OspIsValidTcpNode(g_cEapuMixerApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cEapuMixerApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cEapuMixerApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cEapuMixerApp.m_dwMcuNodeB);
            }      
            return;
        }
    }

    if(FIRST_REGACK == m_tRegAckInfo.m_byRegAckNum)
    {
        //��¼����MCU��Ϣ
        m_dwMcuRcvIp  = ptRegAck->GetMcuIpAddr();
        m_wMcuRcvStartPort = ptRegAck->GetMcuStartPort();
        u32 dwOtherMcuIp = ntohl( ptRegAck->GetAnotherMpcIp() );
        if ( g_cEapuMixerApp.m_tEapuCfg.m_tEapuMixerCfg[GetInsID() - 1].dwConnectIpB == 0 && dwOtherMcuIp != 0 )
        {
			g_cEapuMixerApp.m_tEapuCfg.m_tEapuMixerCfg[GetInsID() - 1].dwConnectIpB = dwOtherMcuIp;
            SetTimer(EV_MIXER_CONNECTB,MIX_CONNETC_TIMEOUT);//����һ��MCU��������
            mixlog(MIXLOG_LEVER_INFO,"[RegAck] Found another Mpc IP: %s, try connecting...\n", StrOfIP(dwOtherMcuIp) );
        }
        mixlog(MIXLOG_LEVER_INFO,"[RegAck]main mpc IP[%s] rcv port [%d]\n", 
            StrOfIP(m_dwMcuRcvIp),m_wMcuRcvStartPort );
        // ��¼�����ش�������Ϊ���������ش�����,��ADDMEMBERʱ���õ�
        TPrsTimeSpan tPrsTimeSpan = *(TPrsTimeSpan *)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck));
        m_tNetRSParam.m_wFirstTimeSpan  = tPrsTimeSpan.m_wFirstTimeSpan;
        m_tNetRSParam.m_wSecondTimeSpan = tPrsTimeSpan.m_wSecondTimeSpan;
        m_tNetRSParam.m_wThirdTimeSpan  = tPrsTimeSpan.m_wThirdTimeSpan;
        m_tNetRSParam.m_wRejectTimeSpan = tPrsTimeSpan.m_wRejectTimeSpan;
        //�Ƿ����෢�ͣ���MCU�˷��͹����ڷ��Ͷ����SetAudioResend���趨
        u8 byIsSendRedundancy = *(u8*)( cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan) + sizeof(u16) );
        mixlog(MIXLOG_LEVER_INFO,"[m_tNetRsParam]m_wFirstTimeSpan = %d m_wSecondTimeSpan = %d, m_wThirdTimeSpan = %d, m_wRejectTimeSpan = %d!\n",
                        m_tNetRSParam.m_wFirstTimeSpan,m_tNetRSParam.m_wSecondTimeSpan,
                        m_tNetRSParam.m_wThirdTimeSpan,m_tNetRSParam.m_wRejectTimeSpan);
        m_bIsSendRedundancy = byIsSendRedundancy;
        mixlog(MIXLOG_LEVER_INFO,"[RegAck] IsSendRedundancy : %d\n", m_bIsSendRedundancy );
		NEXTSTATE((u16)TMixerGrpStatus::READY);//״̬�л�
		m_tGrpStatus.m_byGrpState = (u8)TMixerGrpStatus::READY;
		mixlog(MIXLOG_LEVER_INFO,"Mixer[%d] STATE : [IDLE ----> READY]\n", GetInsID());
    }
    TPeriEqpStatus tEqpStatus;
    tEqpStatus.SetMcuEqp(LOCAL_MCUID, m_tEapuMixerCfg.byEqpId, m_tEapuMixerCfg.byEqpType);
    //////////////////////////////////////////////////////////////////////////

    tEqpStatus.m_byOnline = TRUE;//������
    memcpy(&tEqpStatus.m_tStatus.tMixer.m_atGrpStatus[0],&m_tGrpStatus, sizeof(TMixerGrpStatus));
    tEqpStatus.m_tStatus.tMixer.m_byGrpNum = 1 ;
    tEqpStatus.m_tStatus.tMixer.m_byMixOffChnNum = m_byRcvOff;
    tEqpStatus.SetAlias(m_tEapuMixerCfg.achAlias);
    cServMsg.SetMsgBody((u8*)&tEqpStatus, sizeof(tEqpStatus));

    //��MCU֪����EAPU�崦�ں��ֹ���״̬����������������״̬
    if ( g_cEapuMixerApp.m_dwMcuNode == pMsg->srcnode )
    {
        SendMsgToMcuA(MIXER_MCU_MIXERSTATUS_NOTIF, cServMsg);
    }
    else if (  g_cEapuMixerApp.m_dwMcuNodeB == pMsg->srcnode )
    {
        SendMsgToMcuB(MIXER_MCU_MIXERSTATUS_NOTIF, cServMsg);
    }
    SendGrpNotif();
    return;
}
/*====================================================================
    ������      ��SendGrpNotif
    ����        �����ͻ�����״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::SendGrpNotif()
{
    CServMsg cServMsg;
    cServMsg.SetConfId(m_cConfId);
    m_cConfId.Print();
    cServMsg.SetMsgBody((u8*)&m_tGrpStatus, sizeof(m_tGrpStatus));
    SendMsgToMcuA(MIXER_MCU_GRPSTATUS_NOTIF, cServMsg);
    SendMsgToMcuB(MIXER_MCU_GRPSTATUS_NOTIF, cServMsg);
/*    u8   byIdx;
    u8   byMmbNum;
    u8   abyActiveMmb[MAXNUM_MIXER_DEPTH];
    u8   abyMixMmb[256];
    OspPrintf( TRUE ,FALSE ,"//////////////////////////////////////////////////////////////\n");
    OspPrintf( TRUE ,FALSE ,"Active Mmb[%d]:",m_tGrpStatus.GetActiveMmbNum());
    byMmbNum = m_tGrpStatus.GetActiveMmb( abyActiveMmb ,MAXNUM_MIXER_DEPTH);
    for( byIdx=0 ;byIdx< byMmbNum ;byIdx++)
    {
        OspPrintf( TRUE ,FALSE ," %d",abyActiveMmb[byIdx]);        
    }
    OspPrintf( TRUE ,FALSE ,"\n");
    OspPrintf( TRUE ,FALSE ,"Mixing Member Id[%d]:\n",m_tGrpStatus.GetMmbNum());
    byMmbNum = m_tGrpStatus.GetMixMmb( abyMixMmb ,200);
    for( byIdx=0 ;byIdx< byMmbNum ;byIdx++)
    {
        OspPrintf( TRUE ,FALSE ," %3d",abyMixMmb[byIdx]);
        if( ((byIdx+1)%10)==0 )
        OspPrintf( TRUE ,FALSE ,"\n");
    }
    OspPrintf( TRUE ,FALSE ,"\n//////////////////////////////////////////////////////////////\n");*/
    return;
}
/*====================================================================
    ������      ��SendMsgToMcu
    ����        �����ͻ�����״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
BOOL32 CEapuMixInst::SendMsgToMcu(u16 wEvent, CServMsg const &cServMsg)
{
    BOOL32 bRet = FALSE;
    bRet = SendMsgToMcuA(wEvent, cServMsg);
    bRet &= SendMsgToMcuB(wEvent, cServMsg);
    return bRet;
}
/*====================================================================
    ������      ��SendMsgToMcuA
    ����        ����MCU����ָ����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
BOOL32 CEapuMixInst::SendMsgToMcuA(u16 wEvent, CServMsg const &cServMsg)
{
    BOOL bRet = TRUE;
    if (g_cEapuMixerApp.m_bEmbed || OspIsValidTcpNode( g_cEapuMixerApp.m_dwMcuNode ))
    {
        post(m_tRegAckInfo.m_dwMcuIId, wEvent, cServMsg.GetServMsg(), 
           cServMsg.GetServMsgLen(), g_cEapuMixerApp.m_dwMcuNode);
        mixlog(MIXLOG_LEVER_INFO,"Send Message %u(%s) to A's Mcu%d eqpid[%d]\n", wEvent, ::OspEventDesc(wEvent), LOCAL_MCUID,
            (u16)(m_tRegAckInfo.m_dwMcuIId));
    }
    else
    {
        mixlog(MIXLOG_LEVER_INFO,"Send Message failed %u(%s) ,since disconnected with AMCU%d .\n",
            wEvent, ::OspEventDesc(wEvent), 192);
        bRet = FALSE;
    }
    return bRet;
}
/*====================================================================
    ������      ��SendMsgToMcuB
    ����        ����MCU����ָ����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
BOOL32 CEapuMixInst::SendMsgToMcuB(u16 wEvent, CServMsg const &cServMsg)
{
    BOOL bRet = TRUE;
    if( g_cEapuMixerApp.m_bEmbedB || OspIsValidTcpNode(g_cEapuMixerApp.m_dwMcuNodeB))
    {
        post(m_tRegAckInfo.m_dwMcuIIdB, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cEapuMixerApp.m_dwMcuNodeB);
        mixlog(MIXLOG_LEVER_INFO,"Send Message %u(%s) to B's Mcu%d\n", wEvent, ::OspEventDesc(wEvent), LOCAL_MCUID);
    }
    else
    {
        mixlog(MIXLOG_LEVER_INFO,"Send Message failed %u(%s) ,since disconnected with BMCU%d .\n",
            wEvent, ::OspEventDesc(wEvent), 192);
        bRet = FALSE;
    }
    return bRet;
}
/*====================================================================
    ������      ��MsgRegNackProc
    ����        ������ע��ʧ����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::MsgRegNackProc(CMessage* const pMsg)
{
    if(pMsg->srcnode == g_cEapuMixerApp.m_dwMcuNode)
    {
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d] registe be refused by A's Mcu%d .\n",GetInsID(), m_tRegAckInfo.m_dwMcuIId);
    }

    if(pMsg->srcnode == g_cEapuMixerApp.m_dwMcuNodeB)
    {
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d] registe be refused by B's Mcu%d .\n",GetInsID(), m_tRegAckInfo.m_dwMcuIId);
    }
    return;
}
/*====================================================================
    ������      ��MsgDisconnectProc
    ����        �����������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::DaemonMsgDisconnectProc(CMessage* const pMsg)
{
    u32 dwNode = *(u32*)pMsg->content;

    if (INVALID_NODE != dwNode)
    {
        OspDisconnectTcpNode(dwNode);
    }
    if(dwNode == g_cEapuMixerApp.m_dwMcuNode)
    {
        for (u8 byMixIdx = 1;byMixIdx <= min(g_cEapuMixerApp.m_tEapuCfg.m_byMixerNum,MAXNUM_EAPU_MIXER);byMixIdx++)
		{
            post(MAKEIID(GetAppID(),byMixIdx),OSP_DISCONNECT,pMsg->content,pMsg->length);
		}
        g_cEapuMixerApp.m_dwMcuNode = INVALID_NODE;
        SetTimer(EV_MIXER_CONNECT, MIX_CONNETC_TIMEOUT);
    }
    else if(dwNode == g_cEapuMixerApp.m_dwMcuNodeB)
    {
        for (u8 byMixIdx = 1;byMixIdx <= min(g_cEapuMixerApp.m_tEapuCfg.m_byMixerNum,MAXNUM_EAPU_MIXER);byMixIdx++)
		{
            post(MAKEIID(GetAppID(),byMixIdx),OSP_DISCONNECT,pMsg->content,pMsg->length);
		}
        g_cEapuMixerApp.m_dwMcuNodeB = INVALID_NODE;
        SetTimer(EV_MIXER_CONNECTB, MIX_CONNETC_TIMEOUT);
    }

    // ������һ����������Mcuȡ��������״̬���ж��Ƿ�ɹ�
/*
    if (INVALID_NODE != g_cEapuMixerApp.m_dwMcuNode || INVALID_NODE != g_cEapuMixerApp.m_dwMcuNodeB)
    {
        if (OspIsValidTcpNode(g_cEapuMixerApp.m_dwMcuNode))
        {
            // �Ƿ�����ʱһ���Mcu�ȶ����ٷ�
            post( m_tRegAckInfo.m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cEapuMixerApp.m_dwMcuNode );     
            mixlog(MIXLOG_LEVER_INFO,"[MsgDisconnectProc] GetMsStatusReq. McuNode.A\n");
        }
        else if (OspIsValidTcpNode(g_cEapuMixerApp.m_dwMcuNodeB))
        {
            post( m_tRegAckInfo.m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cEapuMixerApp.m_dwMcuNodeB ); 
            mixlog(MIXLOG_LEVER_INFO,"[MsgDisconnectProc] GetMsStatusReq. McuNode.B\n");
        }
        return;
    }
*/
    if(INVALID_NODE == g_cEapuMixerApp.m_dwMcuNode && INVALID_NODE == g_cEapuMixerApp.m_dwMcuNodeB)
    {
		m_tRegAckInfo.m_byRegAckNum = 0;
        NEXTSTATE((u16)IDLE);                      //  DAEMON ʵ���������״̬
        mixlog(MIXLOG_LEVER_INFO,"DAEMON IDLE\n");
    }
    return;
}
/*====================================================================
    ������      ��MsgDisconnectProc
    ����        �����������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::MsgDisconnectProc()
{
    // ������һ����������Mcuȡ��������״̬���ж��Ƿ�ɹ�
    if (INVALID_NODE != g_cEapuMixerApp.m_dwMcuNode || INVALID_NODE != g_cEapuMixerApp.m_dwMcuNodeB)
    {
        if (OspIsValidTcpNode(g_cEapuMixerApp.m_dwMcuNode))
        {
            // �Ƿ�����ʱһ���Mcu�ȶ����ٷ�
            post( m_tRegAckInfo.m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cEapuMixerApp.m_dwMcuNode );
            mixlog(MIXLOG_LEVER_INFO,"[MsgDisconnectProc] GetMsStatusReq. McuNode.A\n");
        }
        else if (OspIsValidTcpNode(g_cEapuMixerApp.m_dwMcuNodeB))
        {
            post( m_tRegAckInfo.m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cEapuMixerApp.m_dwMcuNodeB );  
            mixlog(MIXLOG_LEVER_INFO,"[MsgDisconnectProc] GetMsStatusReq. McuNode.B\n");
        }
        // �ȴ�ָ��ʱ��
        SetTimer(EV_MIXER_MCU_GETMSSTATUS, WAITING_MSSTATUS_TIMEOUT);
    }
	if (!OspIsValidTcpNode(g_cEapuMixerApp.m_dwMcuNode))
	{
		SetTimer(EV_MIXER_CONNECT, MIX_CONNETC_TIMEOUT);
		m_dwMcuRcvIp = 0;
		m_wMcuRcvStartPort = 0;
	}
	if (!OspIsValidTcpNode(g_cEapuMixerApp.m_dwMcuNodeB))
	{
		SetTimer(EV_MIXER_CONNECTB, MIX_CONNETC_TIMEOUT);
		m_dwMcuRcvIpB = 0;
		m_wMcuRcvStartPortB = 0;
	}
    if(INVALID_NODE == g_cEapuMixerApp.m_dwMcuNode && INVALID_NODE == g_cEapuMixerApp.m_dwMcuNodeB)
    {
		m_tRegAckInfo.m_byRegAckNum = 0;
        m_tRegAckInfo.m_dwMcuIId = INVALID_INS;
        m_tRegAckInfo.m_dwMcuIIdB = INVALID_INS;
        m_tRegAckInfo.m_dwMpcSSrc = 0;
	    StopAllTask();
    }
}
/*====================================================================
    ������      ��InstanceEntry
    ����        ����ͨʵ����Ϣ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::InstanceEntry(CMessage * const pMsg)
{
    if( NULL == pMsg )
    {
        OspPrintf( TRUE, FALSE, "CMcuCfgInst: The received msg's pointer in the msg entry is NULL!\n");
        return;
    }
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d] Recv msg [%u](%s).\n", GetInsID(), pMsg->event, ::OspEventDesc(pMsg->event));

    switch(pMsg->event)
    {
    case EV_MIXER_INIT:
        MsgProcInitalMsg(pMsg);
        break;
    // ����
    case EV_MIXER_CONNECT:
        ProcConnectMcuTimeOut(TRUE);
        break;
    // ����
    case EV_MIXER_CONNECTB:
        ProcConnectMcuTimeOut(FALSE);
		break;
    // ע����Ϣ
    case EV_MIXER_REGISTER:      
        ProcRegisterTimeOut(TRUE);
        break;

    case EV_MIXER_REGISTERB:      
        ProcRegisterTimeOut(FALSE);
        break;

    // MCU ע��Ӧ����Ϣ
    case MCU_MIXER_REG_ACK:
        MsgRegAckProc(pMsg);
        break;

    // MCU�ܾ���������ע��
    case MCU_MIXER_REG_NACK:
        MsgRegNackProc(pMsg);
        break;
    // ���ӻ�����Ա
    case MCU_MIXER_ADDMEMBER_REQ:
        MsgAddMixMemberProc(pMsg);
        break;

    // ɾ��������Ա
    case MCU_MIXER_REMOVEMEMBER_REQ:
        MsgRemoveMixMemberPorc(pMsg);
        break;

    // ��ʼ����
    case MCU_MIXER_STARTMIX_REQ:
        MsgStartMixProc(pMsg);
        break;

    // ֹͣ����
    case MCU_MIXER_STOPMIX_REQ:
        MsgStopMixProc(pMsg);
        break;

    // ��������
    case MCU_MIXER_SETCHNNLVOL_CMD:
        MsgSetVolumeProc(pMsg);
        break;

    // ǿ�ƻ�������
    case MCU_MIXER_FORCEACTIVE_REQ:
        MsgForceActiveProc(pMsg);
        break;

    // ȡ��ǿ�ƻ�������
    case MCU_MIXER_CANCELFORCEACTIVE_REQ:
        MsgCancelForceActiveProc(pMsg);
        break;

    // �����Ա�仯
    case EV_MIXER_ACTIVECHANGE_NOTIF:
        MsgActiveMmbChangeProc(pMsg);
        break;
        
/*    // ������ʱ
    case MCU_MIXER_CHANGEMIXDELAY_CMD:
        MsgChangeMixDelay(pMsg);
        break;*/
    // ���û������
    case MCU_MIXER_SETMIXDEPTH_REQ:
        MsgSetMixDepthProc(pMsg);
        break;
    // ���û�����Ա
 /*   case MCU_MIXER_SETMEMBER_CMD:
        MsgSetMixChannelProc(pMsg);
        break;
    // */
    case MCU_MIXER_SEND_NOTIFY:
        MsgSetMixSendProc(pMsg);
        break;
    // ��������
    case MCU_MIXER_VACKEEPTIME_CMD:
        MsgSetVacKeepTimeProc(pMsg);
        break;
  /*      
    // ����Qosֵ
    case MCU_EQP_SETQOS_CMD:
        ProcSetQosInfo(pMsg);
        break;*/
       case MCU_MIXER_STARTVAC_CMD:
        MsgStartVacProc(pMsg);
        break;
    case MCU_MIXER_STOPVAC_CMD:
        MsgStopVacProc(pMsg);
        break;
	    //OSP ������Ϣ
    case OSP_DISCONNECT:
        MsgDisconnectProc();
        break;
    // ȡ��������״̬Ӧ���ʱ
    case EV_MIXER_MCU_GETMSSTATUS:
    case MCU_EQP_GETMSSTATUS_ACK:
        ProcGetMsStatusRsp(pMsg);
		break;
	case MCU_EQP_MODSENDADDR_CMD:
		MsgModifySendAddr(pMsg);
		break;
    
	default:
        break;
    }
}

/*====================================================================
    ������      ��MsgStartMixProc
    ����        ������ʼ����������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::MsgStartMixProc(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    TMixerStart tMixerStart = *(TMixerStart*)cServMsg.GetMsgBody();
    m_tMediaEncrypt = tMixerStart.GetAudioEncrypt();    
    m_tDoublePayload = *(TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(TMixerStart));
    m_tCapSupportEx = *(TCapSupportEx*)(cServMsg.GetMsgBody() + sizeof(TMixerStart) + sizeof(TDoublePayload));
    u8 byDepth = tMixerStart.GetMixDepth();
    m_byAudType[0] =  tMixerStart.GetAudioMode();
    m_byAudType[1] =  tMixerStart.GetSecAudioMode();
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgStartMixProc() MainMediaType = [%d] SlaveMediaType = [%d] MixDepth: [%d] \n", 
                          GetInsID(),m_byAudType[0] ,   m_byAudType[1] ,  byDepth );
    m_bNeedPrs = tMixerStart.IsNeedByPrs();
	OspPrintf(TRUE, FALSE, "[MsgStartMixProc]m_bNeedPrs = %d!\n", m_bNeedPrs);
    m_tGrpStatus.m_byGrpMixDepth = min(byDepth,MAXNUM_EMIXER_DEPTH);
    mixlog(MIXLOG_LEVER_INFO,"////////////*******MsgStartMixProc**********/////////////\n");
    u16 wRet = 0 ;
    SetSndObjectPara(m_dwMcuRcvIp,
		m_wMcuRcvStartPort + (g_cEapuMixerApp.m_tEapuCfg.m_byIsSimuAPU ? 0 :m_byMixMemNum) * PORTSPAN + 2,
		m_bySndOff + m_byMixMemNum);
    if( g_nPrintElog == MIXLOG_LEVER_INFO )
    {
        m_tCapSupportEx.Print();
    }
    switch(CurState())
    {
    case TMixerGrpStatus::IDLE:      //�û�����δ����
        cServMsg.SetErrorCode(ERR_MIXER_STATUIDLE);
        SendMsgToMcu(MIXER_MCU_STARTMIX_NACK, cServMsg);
        mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgStartMixProc() at IDLE state recv [%d] cmd.\n", GetInsID());
        return;
    case TMixerGrpStatus::MIXING:
        if (m_bIsStartMix)
        {
            cServMsg.SetErrorCode(ERR_MIXER_MIXING);         
            SendMsgToMcu(MIXER_MCU_STARTMIX_NACK, cServMsg);
            mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgStartMixProc() at READY state Call StartAudMix() Failed!\n", GetInsID());
            return;
        }
        if (!(m_cConfId == cServMsg.GetConfId())) // ����
        {
            s8 achCurConfId[64] ={0};
            s8 achStopConfId[64] ={0};
            m_cConfId.GetConfIdString(achCurConfId, sizeof(achCurConfId));
            cServMsg.GetConfId().GetConfIdString(achStopConfId, sizeof(achStopConfId));
            cServMsg.SetErrorCode(ERR_MIXER_ERRCONFID);
            SendMsgToMcu(MIXER_MCU_STOPMIX_NACK, cServMsg);
            mixlog(MIXLOG_LEVER_INFO,"Mixer[%d] Serving %s refuse confId(%s) stop req.\n", GetInsID(), achCurConfId, achStopConfId);
            return;
        }
		return;
    case TMixerGrpStatus::READY:
        {
            wRet = m_cMixer.StartAudMix();
            if ( wRet != (u16)Codec_Success )
            {
                cServMsg.SetErrorCode(wRet);         
                SendMsgToMcu(MIXER_MCU_STARTMIX_NACK, cServMsg);
                mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgStartMixProc() at READY state Call StartAudMix() Failed! wRet = [%d]\n", GetInsID(),wRet);
                //return;
            } 
			if (g_cEapuMixerApp.m_tEapuCfg.m_byIsSimuAPU)
			{
				wRet = m_cMixer.StartVolActive();
				m_bIsStartVac = TRUE;
				if ( wRet != (u16)Codec_Success )
				{
					cServMsg.SetErrorCode(wRet);         
					SendMsgToMcu(MIXER_MCU_STARTMIX_NACK, cServMsg);
					mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgStartMixProc() at READY state Call StartAudMix() Failed! wRet = [%d]\n", GetInsID(),wRet);
				} 
			}
            TKdvMixerType tType;
            tType.dwChID = 0;//���ʽҪ�޸�
            tType.bDMode = TRUE;
            tType.byAudType = m_byAudType[0];
            tType.byAudMode = GetAudioMode(m_byAudType[0]) ;
            wRet = m_cMixer.SetDModeChannel(tType);
            if ((u16)Codec_Success != wRet)
            {
                mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgStartMixProc() Call SetDModeChannel() FAILED wRet=[%d].\n", GetInsID(),wRet);
            }
            else
            {
                mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgStartMixProc() Call SetDModeChannel() Successed!\n", GetInsID());
            }
            wRet = m_cMixer.SetMixerChVol(TRUE,0,MAXVALUE_EMIXER_VOLUME/2);
            if(wRet != (u16)Codec_Success)
            {
                mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgAddMixMemberProc() Call SetMixerChVol(dmode) failed(%d)\n",GetInsID(), wRet);
            }
            else
            {
                mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgAddMixMemberProc() Call SetMixerChVol() Successed!\n",GetInsID());
            }
            wRet = m_cMixer.SetMixDepth(byDepth);
            if ((u16)Codec_Success != wRet)
            {
                mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgStartMixProc() Call SetMixDepth() FAILED wRet=[%d].\n", GetInsID(),wRet);
            }
            else
            {
                mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgStartMixProc() Call SetMixDepth() Successed!\n", GetInsID() );
            }
            mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgStartMixProc() Call StartAudMix() Successed!\n", GetInsID());
            m_cConfId = cServMsg.GetConfId();//��¼�û���������Ļ�����Ϣ
            NEXTSTATE((u16)TMixerGrpStatus::MIXING);//������״̬�л�
            m_tGrpStatus.m_byGrpState = (u8)TMixerGrpStatus::MIXING;
            mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgStartMixProc()  STATE : [READY ----> MIXING]  MIXMODE :  [NOMIX  ---->  PARTMIX] \n", GetInsID());
            cServMsg.SetErrorCode(wRet);
            SendMsgToMcu(MIXER_MCU_STARTMIX_ACK, cServMsg);
            SendGrpNotif();
            m_bIsStartMix = TRUE;
            return;
        }

    default:
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]undefine state (%d).\n", GetInsID() , CurState());
        break;
    }
    return;
}
/*====================================================================
    ������      ��
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::MsgStopMixProc(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    //u8* pbyMsgBody = (u8*)cServMsg.GetMsgBody();
    if (!(m_cConfId == cServMsg.GetConfId())) // ����
    {
        s8 achCurConfId[64] ={0};
        s8 achStopConfId[64] ={0};
        m_cConfId.GetConfIdString(achCurConfId, sizeof(achCurConfId));
        cServMsg.GetConfId().GetConfIdString(achStopConfId, sizeof(achStopConfId));
        cServMsg.SetErrorCode(ERR_MIXER_ERRCONFID);
        SendMsgToMcu(MIXER_MCU_STOPMIX_NACK, cServMsg);
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d] Serving %s refuse confId(%s) stop req.\n", GetInsID(), achCurConfId, achStopConfId);
        return;
    }
    u16 wRet = 0 ;
    switch(CurState())
    {
    case TMixerGrpStatus::IDLE:      //�û�����δ����
        cServMsg.SetErrorCode(ERR_MIXER_STATUIDLE);
        SendMsgToMcu(MIXER_MCU_STARTMIX_NACK, cServMsg);
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgStopMixProc() at IDLE state recv stat mixing cmd.\n", GetInsID(), CurState());
        return;
    //�û�����δ��ʹ��
    case TMixerGrpStatus::READY:
        cServMsg.SetErrorCode(ERR_MIXER_NOTMIXING);
        SendMsgToMcu(MIXER_MCU_STOPMIX_NACK, cServMsg);
        mixlog(MIXLOG_LEVER_WARN,"Mixer[%d]MsgStopMixProc() at READY state recv cmd.\n", GetInsID());
        return;

    case TMixerGrpStatus::MIXING: //��ǰ�ѿ�ʼ����
        {
			if (m_bIsStartMix)
			{
				wRet = m_cMixer.DelDModeChannel(0);
				if (wRet != (u16)Codec_Success)
				{
					cServMsg.SetErrorCode(wRet);
					SendMsgToMcu(MIXER_MCU_STOPMIX_NACK, cServMsg);
					mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgStopMixProc() call DeleteDmodeChnAndDModeSndObject() Failed!\n", GetInsID());
					return;
				}
				mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgStopMixProc() call DeleteDmodeChnAndDModeSndObject() Successed!\n", GetInsID());
				wRet = m_cMixer.StopAudMix();
				if (wRet != (u16)Codec_Success)
				{
					cServMsg.SetErrorCode(wRet);
					SendMsgToMcu(MIXER_MCU_STOPMIX_NACK, cServMsg);
					mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgStopMixProc() call StopAudMix() Failed!\n", GetInsID());
					return;
				}
				mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgStopMixProc() call StopAudMix() Successed!\n", GetInsID());
				m_bIsStartMix = FALSE;
			}
			if (m_bIsStartVac)
			{
				wRet = m_cMixer.StopVolActive();
				if (wRet != (u16)Codec_Success)
				{
					cServMsg.SetErrorCode(wRet);
					SendMsgToMcu(MIXER_MCU_STOPMIX_NACK, cServMsg);
					mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgStopMixProc() call StopAudMix() Failed!\n", GetInsID());
					return;
				}
				mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]MsgStopMixProc() call StopVolActive() Successed!\n", GetInsID());
				m_bIsStartVac = FALSE;
			}
			if (!m_bIsStartVac && !m_bIsStartMix)
			{
                wRet = FreeAllRcv();
                if (wRet != (u16)Codec_Success)
                {
                    cServMsg.SetErrorCode(wRet);
                    SendMsgToMcu(MIXER_MCU_STOPMIX_NACK, cServMsg);
                    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgStopMixProc() call FreeAllRcv() Failed!\n", GetInsID());
                    return;
                }
                mixlog(MIXLOG_LEVER_INFO,"Mixer[%d]MsgStopMixProc() STATE : [MIXING ----> READY]  MIXMODE :  [PARTMIX  ---->  NOMIX] \n", GetInsID());
                m_tGrpStatus.m_byGrpMixDepth = 0;
                m_tGrpStatus.m_byConfId = 0;
                //m_byLastAudType[0] = m_byAudType[0];
                m_cConfId.SetNull();//��Ϊ�κλ������ 
                NEXTSTATE((u16)TMixerGrpStatus::READY);//������״̬�л�
                m_tGrpStatus.m_byGrpState = (u8)TMixerGrpStatus::READY;
			}
            SendGrpNotif();
            SendMsgToMcu(MIXER_MCU_STOPMIX_ACK, cServMsg);
        }
        break;
    default:
        mixlog(MIXLOG_LEVER_INFO,"Mixer[%d] at undefine state (%d).\n", GetInsID() - 1 , CurState());
        break;
    }
}
void CEapuMixInst::StopAllTask()
{
    u16 wRet = 0;
    switch(CurState())
    {
        //�û�����δ��ʹ��
        case TMixerGrpStatus::IDLE:
        case TMixerGrpStatus::READY:
        case TMixerGrpStatus::MIXING:
        {
            if (m_bIsStartMix)
            {            
                wRet = m_cMixer.StopAudMix();
                if ( wRet != (u16)Codec_Success )
				{
                    mixlog(MIXLOG_LEVER_WARN,"error:[StopAudMix]\n");
				}
				m_bIsStartMix = FALSE;
            }
            if (m_bIsStartVac)
            {            
                wRet = m_cMixer.StopVolActive();
                if ( wRet != (u16)Codec_Success )
				{
                    mixlog(MIXLOG_LEVER_WARN,"error:[StopVolActive]\n");
				}
				m_bIsStartVac = FALSE;
            }
            break;
        }
        default:
            mixlog(MIXLOG_LEVER_INFO,"\t\t\terror:unkonwn state\n");
            break;
    }
    FreeAllRcv();
    u8 byState = m_tGrpStatus.m_byGrpState;
    m_tGrpStatus.m_byConfId = 0;
    m_cConfId.SetNull();
    m_tGrpStatus.m_byGrpState = (u8)TMixerGrpStatus::IDLE ;
    NEXTSTATE((u16)TMixerGrpStatus::IDLE);//״̬�л�
    static char status[3][12] = {"IDLE", "READY", "MIXING"};
    mixlog(MIXLOG_LEVER_INFO,"Mixer[%d] STATE : [%s ----> IDLE]\n", GetInsID(),status[byState]);
    SendGrpNotif();
}
/*====================================================================
    ������      ��
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::MsgSetVacKeepTimeProc( CMessage * const pcMsg )
{
    u16 wRet = 0;
    CServMsg cServMsg(pcMsg->content,pcMsg->length);
    u8 *pbyMsgBody = (u8*)cServMsg.GetMsgBody();
    //u8 byGrpId    = *pbyMsgBody;
    u32 dwKeepTime = ntohl(*(u32*)(pbyMsgBody + sizeof(u8)));
    wRet = m_cMixer.SetVolActKeepTime(dwKeepTime * 1000);
    if ((u16)Codec_Success != wRet)
    {
        mixlog(MIXLOG_LEVER_WARN,"Mixer[%d] SetVolActKeepTime Failed! para=%d.\n",GetInsID(),dwKeepTime);
    }
    mixlog(MIXLOG_LEVER_INFO,"info:Mixer[%d] call SetVolActKeepTime(%d) Successed!\n",GetInsID(), dwKeepTime);
    return;
}
/*====================================================================
    ������      ��
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::ShowAllMixGroupStatus(CApp* pcApp)
{
	OspPrintf(TRUE,FALSE,"g_nSendTimes = [%d] g_nSendSpan = [%d]\n",g_nSendTimes, g_nSendSpan);
    for(u8 byMixerId = 1; byMixerId <= g_cEapuMixerApp.m_tEapuCfg.m_byMixerNum; byMixerId++)
    {
        CEapuMixInst* pInst;
        pInst = (CEapuMixInst*)pcApp->GetInstance(byMixerId);
        pInst->ShowGrpStatus();
    }
    return;
}
/*====================================================================
    ������      ��
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::ShowGrpStatus()
{
    static char status[3][12] = {"IDLE", "READY", "MIXING"};
    OspPrintf(TRUE, FALSE, "\n--------------------------------------    Mixer[%d]    ----------------------------------------\n", GetInsID());
    OspPrintf(TRUE, FALSE, "\tType:[%d]Status:[%s]Depth:[%d]RcvOff[%d]SndOff[%d]Chn[%d]RcvStartPort[%d]\n", 
        m_bIsDMode,status[m_tGrpStatus.m_byGrpState],m_tGrpStatus.m_byGrpMixDepth,
        m_byRcvOff,m_bySndOff,m_byMixMemNum,m_tEapuMixerCfg.wRcvStartPort);
    OspPrintf(TRUE, FALSE, "\tIsStartMix[%d] IsStartVac[%d] MediaType[%d]\n", m_bIsStartMix,m_bIsStartVac,m_byAudType[0]);

    u8 byVolume = 0 ;
    TKdvMixerChStatus tKdvMixerChStatus;
    TKdvMixerChStatis tKdvMixerChStatis;
    u16 wRet = 0;
    for ( u16 wIndex = 0;wIndex < m_byMixMemNum ; wIndex++)
    {
        if (g_cEapuMixerApp.m_bIsChnUsed[m_byRcvOff + wIndex])
        {
            wRet = m_cMixer.GetMixerChVol(FALSE , wIndex, byVolume);
            if ((u16)Codec_Success != wRet)
            {
                mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]ShowGrpStatus() Call GetMixerChVol() failed!chnIndex =[%d]wRet=[%d].\n", GetInsID() - 1,wIndex,wRet );
            }
            memset(&tKdvMixerChStatus, 0, sizeof(TKdvMixerChStatus));
            wRet = m_cMixer.GetMixerChStatus(FALSE, wIndex, tKdvMixerChStatus);
            if ((u16)Codec_Success != wRet)
            {
                mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]ShowGrpStatus() Call GetMixerChStatus() failed!chnIndex =[%d]wRet=[%d].\n", GetInsID() - 1,wIndex,wRet );
            }
            memset(&tKdvMixerChStatis, 0, sizeof(TKdvMixerChStatis));
            wRet = m_cMixer.GetMixerChStatis(FALSE, wIndex, tKdvMixerChStatis);
            if ((u16)Codec_Success != wRet)
            {
                mixlog(MIXLOG_LEVER_CRIT,"Mixer[%d]ShowGrpStatus() Call GetMixerChStatus() failed!chnIndex =[%d]wRet=[%d].\n", GetInsID() - 1,wIndex,wRet );
            }

            OspPrintf(TRUE, FALSE, "\n\n");
            OspPrintf(TRUE, FALSE, "  ________MixChnlStatus_______           ________EncStatis_________     ___________DecStatis_________\n");
            OspPrintf(TRUE, FALSE, " |       |        |           |         |        |        |        |       |           |        |\n");
            OspPrintf(TRUE, FALSE, "ChID ChVolume AudEncType AudDecType FrameRate BitRate PackLose PackError FrameRate RecvFrame LoseFrame LoseRatio\n");
            OspPrintf(TRUE, FALSE, "%2d     %3d       %4d    %4d    %4d    %3d  %5d %8d  %d %8d  %8d       %d\n",
                wIndex,byVolume,tKdvMixerChStatus.byAudEncType,tKdvMixerChStatus.byAudDecType,
                        tKdvMixerChStatis.tAudEncStatis.m_dwFrameRate,tKdvMixerChStatis.tAudEncStatis.m_dwBitRate,tKdvMixerChStatis.tAudEncStatis.m_dwPackLose,tKdvMixerChStatis.tAudEncStatis.m_dwPackError,
                        tKdvMixerChStatis.tAudDecStatis.m_wFrameRate,tKdvMixerChStatis.tAudDecStatis.m_dwRecvFrame,tKdvMixerChStatis.tAudDecStatis.m_dwLoseFrame,tKdvMixerChStatis.tAudDecStatis.m_wLoseRatio);
            if (m_bIsStartMix)
            {
                TKdvRcvStatus tKdvRcvStatus;
                g_cEapuMixerApp.m_acAudRcv[m_byRcvOff + wIndex].GetStatus ( tKdvRcvStatus );
                TKdvSndStatus tKdvSndStatus;
                g_cEapuMixerApp.m_acAudSnd[m_bySndOff + wIndex].GetStatus ( tKdvSndStatus );
                OspPrintf(TRUE, FALSE, "MT-ID [%d]  RcvPort [%d] SndIP [%s] SndPort [%d]\n",
                    m_abyMtId2ChnIdx[wIndex],tKdvRcvStatus.m_tRcvAddr.m_tLocalNet.m_wRTPPort,
                    StrOfIP(tKdvSndStatus.m_tSendAddr.m_tRemoteNet[0].m_dwRTPAddr),tKdvSndStatus.m_tSendAddr.m_tRemoteNet[0].m_wRTPPort);
            }
            else
            {
                TKdvRcvStatus tKdvRcvStatus;
                g_cEapuMixerApp.m_acAudRcv[m_byRcvOff + wIndex].GetStatus ( tKdvRcvStatus );
                OspPrintf(TRUE, FALSE, "MT-ID [%2d]  RcvPort [%d]\n",
                    m_abyMtId2ChnIdx[wIndex],tKdvRcvStatus.m_tRcvAddr.m_tLocalNet.m_wRTPPort);
            }
        }
    }
    if (m_bIsStartMix)
    {
        TNetSndParam tNetSndParam;
        g_cEapuMixerApp.m_acAudSnd[m_bySndOff + m_byMixMemNum].GetNetSndParam(&tNetSndParam);
        OspPrintf(TRUE, FALSE, "\n\nN SndPort [%d] RemoteRcvIP [%s] RemoteRcvPort [%d]\n",tNetSndParam.m_tLocalNet.m_wRTPPort,
            StrOfIP(tNetSndParam.m_tRemoteNet[0].m_dwRTPAddr),tNetSndParam.m_tRemoteNet[0].m_wRTPPort);
        u32 adwChnId[MAXNUM_EMIXER_DEPTH] = {0};
        u32 dwChNum = 0 ;
        m_cMixer.GetActiveChID(adwChnId,dwChNum);
        if ( dwChNum != 0)
        {
            OspPrintf(TRUE,FALSE,"Mixer[%d]Working Channels Number =[%d]\n",GetInsID(),dwChNum);
            OspPrintf(TRUE,FALSE,"\tWorking Channel's NO:");
			
			/*lint -save -e650*/
			/*Warning -- Constant '-1' out of range for operator '!='*/
            for ( u8 byIndex = 0 ;byIndex < MAXNUM_EMIXER_DEPTH;byIndex++)
            {
                if ( adwChnId[byIndex] != 0xFFFF )
                {
                    OspPrintf(TRUE,FALSE,"[%d]",adwChnId[byIndex]);
                }
            }
			/*lint -restore*/

            OspPrintf(TRUE,FALSE,"\n");
        }
        u8   byIdx;
        u8   byMmbNum;
        u8   abyActiveMmb[MAXNUM_MIXER_DEPTH];
        u8   abyMixMmb[256];
        OspPrintf( TRUE ,FALSE ,"Active Mmb[%d]:",m_tGrpStatus.GetActiveMmbNum());
        byMmbNum = m_tGrpStatus.GetActiveMmb( abyActiveMmb ,MAXNUM_MIXER_DEPTH);
        for( byIdx=0 ;byIdx< byMmbNum ;byIdx++)
        {
            OspPrintf( TRUE ,FALSE ," %d",abyActiveMmb[byIdx]);        
        }
        OspPrintf( TRUE ,FALSE ,"\n");
        OspPrintf( TRUE ,FALSE ,"Mixing Member Id[%d]:\n",m_tGrpStatus.GetMmbNum());
        byMmbNum = m_tGrpStatus.GetMixMmb( abyMixMmb ,200);
        for( byIdx=0 ;byIdx< byMmbNum ;byIdx++)
        {
            OspPrintf( TRUE ,FALSE ," %3d",abyMixMmb[byIdx]);
            if( ((byIdx+1)%10)==0 )
			{
				OspPrintf( TRUE ,FALSE ,"\n");
			}
        }
    }
	OspPrintf(TRUE,FALSE,"MainMpc(%s)  MpcRecvIp(%s)(port:%d)\n",StrOfIP(m_dwMcuRcvIp),
		StrOfIP(m_dwMcuRcvIp),m_wMcuRcvStartPort);
	OspPrintf(TRUE,FALSE,"OtheMpc(%s)  MpcRecvIp(%s)(port:%d)\n",StrOfIP(m_dwMcuRcvIp),
		StrOfIP(m_dwMcuRcvIpB),m_wMcuRcvStartPortB);
	m_tRegAckInfo.print();
    OspPrintf(TRUE,FALSE,"\n-----------------------------------------------------------------------------------------------\n");
}
/*====================================================================
    ������      ��
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::ProcSetQosInfo(CMessage* const pMsg)
{
    if(NULL == pMsg)
    {
        mixlog(MIXLOG_LEVER_INFO,"[Mixer] The Mix's Qos infomation is Null\n");
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

    mixlog(MIXLOG_LEVER_INFO,"Type: %d, Aud= %d, Vid= %d, Data= %d, IpPrior= %d\n", 
            byQosType, byAudioValue, byVideoValue, byDataValue, byIpPriorValue);

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
        
    mixlog(MIXLOG_LEVER_INFO,"\nAud= %d, Vid= %d, Data= %d\n", 
                    byAudioValue, byVideoValue, byDataValue);

    kdvSetMediaTOS((s32)byAudioValue, QOS_AUDIO);
    kdvSetMediaTOS((s32)byVideoValue, QOS_VIDEO);
    kdvSetMediaTOS((s32)byDataValue, QOS_DATA);
    
	return;
}
/*====================================================================
    ������      ��
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::MsgActiveMmbChangeProc(CMessage * const pMsg)
{
    u8* pbyCurActiveMmb;
    u8  abyMmb[MAXNUM_EMIXER_DEPTH];
    u8  abyMtMmb[MAXNUM_EMIXER_DEPTH];
    CServMsg cServMsg(pMsg->content, pMsg->length);
    pbyCurActiveMmb = (u8*)cServMsg.GetMsgBody();
    //OspPrintf(TRUE,FALSE,"MsgActiveMmbChangeProc��ǰ�������ͨ����=[%d]\n",GetInsID(),dwChNum);
    OspPrintf(TRUE,FALSE,"\tActive Mixing Channels' number and NO:");
    for ( u8 byIndex = 0 ;byIndex < MAXNUM_EMIXER_DEPTH;byIndex++)
    {
        if ( pbyCurActiveMmb[byIndex] != 0xFF )
        {
            //u8 byActiveChIdx = pbyCurActiveMmb[byIndex];
            abyMtMmb[byIndex] = m_abyMtId2ChnIdx[pbyCurActiveMmb[byIndex]];
            OspPrintf(TRUE,FALSE,"[%d]",abyMtMmb[byIndex]);
        }
    }
    OspPrintf(TRUE,FALSE,"\n");
    //OspPrintf(TRUE,FALSE,"��ǰ��������ͨ��Ϊ[%d]m_abyMtId2ChnIdx=[%d]\n",
    //    *(pbyCurActiveMmb + MAXNUM_EMIXER_DEPTH * sizeof(u8)),m_abyMtId2ChnIdx[*(pbyCurActiveMmb + MAXNUM_EMIXER_DEPTH * sizeof(u8))]);
    memset(abyMmb, 0, MAXNUM_EMIXER_DEPTH);
    memcpy(abyMmb, pbyCurActiveMmb, MAXNUM_EMIXER_DEPTH);

    u8 byExcitChid = *(pbyCurActiveMmb + MAXNUM_EMIXER_DEPTH);
    u8 byExcitMtId = 0;
    byExcitMtId = m_abyMtId2ChnIdx[byExcitChid];
    mixlog(MIXLOG_LEVER_INFO,"\t\tCHNL[%d] Changed As a Actived Member��MT-ID Is [%d]\n",byExcitChid,byExcitMtId);
    cServMsg.SetConfId(m_cConfId);
    cServMsg.SetErrorCode(0);
    cServMsg.SetMsgBody(abyMtMmb, MAXNUM_MIXER_DEPTH);
    cServMsg.CatMsgBody(&byExcitMtId, sizeof(u8));
    SendMsgToMcu(MIXER_MCU_ACTIVEMMBCHANGE_NOTIF, cServMsg);

    m_tGrpStatus.UpdateActiveMmb(abyMmb);
    SendGrpNotif();
    return;
}
/*====================================================================
    ������      ��
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
void CEapuMixInst::ComplexQos(u8& byValue, u8 byPrior)
{
    u8 byLBit = ((byPrior & 0x08) >> 3);
    u8 byRBit = ((byPrior & 0x04) >> 1);
    u8 byTBit = ((byPrior & 0x02)<<1);
    u8 byDBit = ((byPrior & 0x01)<<3);
    
    byValue = byValue + ((byDBit + byTBit + byRBit + byLBit)*2);
    return;
}
/*====================================================================
    ������      ��
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/01/20  1.0         ��־��          ����
====================================================================*/
CEapuCfg::CEapuCfg()
{
    m_dwMcuNode = INVALID_NODE;
    m_dwMcuNodeB = INVALID_NODE;
    m_bEmbed = FALSE;               //�Ƿ񱾻�MCU.A
    m_bEmbedB = FALSE;              //�Ƿ���Ƕ��B����MCU.B
    for ( u8 byChnIdx = 0;byChnIdx < MAXNUM_MIXER_CHNNL;byChnIdx++ )
    {
        m_bIsChnUsed[byChnIdx] = FALSE;
    }
    KdvSocketStartup();
}

/*lint -save -e1551*/
CEapuCfg::~CEapuCfg()
{
    KdvSocketCleanup();
}
/*lint -restore*/

/*=============================================================================
  �� �� ���� GetMtId2ChnIdx
  ��    �ܣ� �ն�Id��ͨ����ӳ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byMtId: ��1��ʼ
              BOOL32 bAddMt /* = TRUE  �����նˣ��ҿ���ͨ����� */
//  �� �� ֵ�� u8 
//=============================================================================*/
u8 CEapuMixInst::GetMtId2ChnIdx( u8 byMtId, BOOL32 bAddMt /* = TRUE  */)
{
	u8 byChIdx = 0;
	for( u8 byLp = 0; byLp < MAXNUM_APU_MIXING_MEMBER; byLp++ )
	{
		if( bAddMt )
		{
			if( 0 == m_abyMtId2ChnIdx[byLp] )
			{
				byChIdx = byLp;
				m_abyMtId2ChnIdx[byLp] = byMtId;
				break;
			}
		}
		else if( m_abyMtId2ChnIdx[byLp] == byMtId )
		{
			byChIdx = byLp;
			break;
		}
	}
	return byChIdx;
}
/*=============================================================================
  �� �� ���� MsgModifySendAddr
  ��    �ܣ� ������͵�ַ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  
  �� �� ֵ�� void 
=============================================================================*/
void CEapuMixInst::MsgModifySendAddr(CMessage * const pcMsg)
{
	u32 dwSendIP = *(u32*)pcMsg->content;
	m_dwMcuRcvIp = ntohl(dwSendIP);
	if (CurState() == TMixerGrpStatus::MIXING)
	{
		SetSndObjectPara(m_dwMcuRcvIp,m_wMcuRcvStartPort + (g_cEapuMixerApp.m_tEapuCfg.m_byIsSimuAPU ? 0 :m_byMixMemNum) * PORTSPAN + 2,m_bySndOff + m_byMixMemNum);
	}
}
