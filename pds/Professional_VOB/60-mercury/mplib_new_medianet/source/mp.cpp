 /*****************************************************************************
   ģ����      : MP
   �ļ���      : mp.cpp
   ����ļ�    : mp.h
   �ļ�ʵ�ֹ���: MP�ĳ�ʼ�������������߼�ʵ��
   ����        : ������
   �汾        : V4.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���          �޸�����
   2003/07/10   0.1         ������          ����
   2007/03/21   4.0         �ű���          �㲥�����߼������ӡ������Ż�
******************************************************************************/
#include "mp.h"
#include "kdvsys.h"
#include "bindmp.h"
#include "mcuver.h"

#ifdef _LINUX_
#include "dsccomm.h"
#include "bindwatchdog.h"
#endif

extern CNetTrfcStat g_cNetTrfcStat;
extern BOOL32       g_bPrintMpNetStat;

u8  g_byMpPrtLvl = LOG_CRIT;



//=======================================================//
//                                                       //
//                        CallBack                       //
//                                                       //
//=======================================================//

/*=============================================================================
  �� �� ���� MpSendFilterCallBackProc
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwRecvIP, u16 wRecvPort,
             u32 dwSrcIP,  u16 wSrcPort,
             TNetSndMember * ptSends,      // ת��Ŀ���б�
             u16 * pwSendNum,              // ת��Ŀ�����
             u8  * pUdpData, u32 dwUdpLen
  �� �� ֵ�� void
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/04    4.0			�ű���                ֧���ڲ��������� �ؼ�֡��ת�����״̬ �л�
=============================================================================*/
void MpSendFilterCallBackProc( u32 dwRecvIP, u16 wRecvPort,
                               u32 dwSrcIP,  u16 wSrcPort,
                               TNetSndMember * ptSends,
                               u16 * pwSendNum,
                               u8  * pUdpData, u32 dwUdpLen )
{
	return;

    u8   byPayloadType;
    u16  wNewSeqNum;
    u32  dwNewTimeStamp;
    u32  dwNewSSRC;
    u16  wSendNum;
    u8   byPayload;
    u16  wSwitchSrcSeqInterval = SWITCH_SRC_SEQ_INTERVAL;  

    TNetSndMember tSendTemp;

    wSendNum = *pwSendNum;

    if (dwUdpLen <= 12)    // С�ڵ���rtpͷ(12Byte)��Ϊת�����԰�
    {
        for(u16 wSenderIdx = 0; wSenderIdx < wSendNum; wSenderIdx++)
        {
            ptSends[wSenderIdx].wOffset = 0;
            ptSends[wSenderIdx].wLen    = 0;
        }
        MpLog(LOG_DETAIL, "[CallBack] Pack for port.%d is ignored due to UdpLen.%d\n", wRecvPort, dwUdpLen);
        return;
    }

    if( NULL == g_cMpApp.m_pptFilterParam || 
        NULL == g_cMpApp.m_pptWaitIFrmData )
    {
        return;
    }

    // ����Ƿ���RTP��

    // ��UdpData��ȡSequence��TimeStamp��SSRC
    byPayloadType  = ntohs(*(u16 *)pUdpData) & 0x007F;
    wNewSeqNum     = ntohs(*(u16 *)(pUdpData + 2));
    dwNewTimeStamp = ntohl(*(u32 *)(pUdpData + 4));
    dwNewSSRC      = ntohl(*(u32 *)(pUdpData + 8));

    MpLog( LOG_DETAIL, "[CallBack] Payload.%d for port.%d, SeqNum.%d, TimeStamp.%d��dwNewSSRC.%d !\n", 
                                  byPayloadType, wRecvPort, wNewSeqNum, dwNewTimeStamp, dwNewSSRC );


    // zbq [04/20/2007] �ж϶˿ڣ������������Ƶ�͵ڶ�·��Ƶ
    BOOL32 bChkIFrm = g_cMpApp.IsPortNeedChkIFrm( wRecvPort );
    
    //����Ƿ�263�ؼ�֡
    if ( MEDIA_TYPE_H263 == byPayloadType && g_cMpApp.m_bIsWaitingIFrame && FALSE)
    {
        // xsl [9/20/2006] 263�ȹؼ�֡ʱ�������䣨���sony�ն��л��������⣩
        wSwitchSrcSeqInterval = 1;

        BOOL32 bH263IFrame = CMpBitStreamFilter::IsKeyFrame(byPayloadType, pUdpData, dwUdpLen);          

        TWaitIFrameData tWaitData;
        if (g_cMpApp.GetWaitIFrameDataBySrc(dwRecvIP, wRecvPort, &tWaitData))
        {
            BOOL bWaitingIFrame = tWaitData.bWaitingIFrame;
            u32  dwFstTimeStamp = tWaitData.dwFstTimeStamp;              
            
            if (bH263IFrame)
            {
                MpLog( LOG_DETAIL, "[CallBack] received h263 IFrame, payload :%d, confno :%d, mtid :%d, dwFstTimeStamp:%d, dwNewTimeStamp:%d\n",
                       byPayloadType, tWaitData.byConfNo, tWaitData.byMtId, dwFstTimeStamp, dwNewTimeStamp );
            }

            if (bWaitingIFrame && tWaitData.byMtId <= MAXNUM_CONF_MT+POINT_NUM)
            {        
                if (bH263IFrame && 0 != dwFstTimeStamp && dwNewTimeStamp != dwFstTimeStamp) //�ж��Ƿ�ؼ�֡
                {
                    MpLog( LOG_ERR, "[CallBack] reset bWaitingIFrame, confno :%d, mtid :%d\n", 
                                     tWaitData.byConfNo, tWaitData.byMtId );
                    g_cMpApp.m_pptWaitIFrmData[tWaitData.byConfNo][tWaitData.byMtId-1].bWaitingIFrame = FALSE;            
                }   
                else //�����ǹؼ�֡�򲻽���ת��
                {
                    MpLog( LOG_WARN, "[CallBack] Not IFrame, wait for next pack !\n" );
                    wSendNum = 0;
                }
                
                if (0 == dwFstTimeStamp)
                {
                    g_cMpApp.m_pptWaitIFrmData[tWaitData.byConfNo][tWaitData.byMtId-1].dwFstTimeStamp = dwNewTimeStamp;
                }
            }
        }
        else
        {
            MpLog(LOG_VERBOSE, "[MpSendFilterCallBackProc] get waitdata failed by src\n");
        }        
    }

    // ���ת����Ա���޸�����
    for(u16 wSenderIdx = 0; wSenderIdx < wSendNum; wSenderIdx++)
    {
        u8   byConfNo;
        u16  wChanNo;
        BOOL32 bUniform;

        // �п�϶,�����
        if (NULL == ptSends[wSenderIdx].pAppData)
        {
            u16 wSenderIdx2 = wSendNum - 1;
            while(NULL == ptSends[wSenderIdx2].pAppData && wSenderIdx2 > wSenderIdx)
            {
                wSenderIdx2--;
                wSendNum--;
            }

            if (wSenderIdx2 > wSenderIdx)
            {
                tSendTemp = ptSends[wSenderIdx];
                ptSends[wSenderIdx] = ptSends[wSenderIdx2];
                ptSends[wSenderIdx2] = tSendTemp;

                wSendNum--;
            }
            else
            {
                wSendNum--;
                continue;
            }
        }

        // zbq [04/20/2007] �ǹ�һ���������޸����к�
        if ( bChkIFrm )
        {
            u8 byConfIdx = ((TSendFilterAppData *)ptSends[wSenderIdx].pAppData)->byConfNo;
            if ( CONF_UNIFORMMODE_VALID != g_cMpApp.GetConfUniMode(byConfIdx) )
            {
                MpLog( LOG_DETAIL, "[CallBack] port.%d for conf.%d, ignore uniform\n", wRecvPort, byConfIdx );
                continue;
            }
        }
        else
        {
            MpLog( LOG_VERBOSE, "[CallBack] unexpected port.%d to chk IFrm \n" );
        }

        bUniform   = ((TSendFilterAppData *)ptSends[wSenderIdx].pAppData)->bUniform;
        if (FALSE == bUniform)
        {
            ptSends[wSenderIdx].wOffset = 0;
            ptSends[wSenderIdx].wLen    = 0;
            continue;
        }

        byConfNo  = ((TSendFilterAppData *)ptSends[wSenderIdx].pAppData)->byConfNo;
        wChanNo   = ((TSendFilterAppData *)ptSends[wSenderIdx].pAppData)->wChannelNo;
        byPayload = ((TSendFilterAppData *)ptSends[wSenderIdx].pAppData)->byPayload;

        // ��ֹ�����������
        //  xsl [1/21/2006] ����id���ֵΪ��������������ģ����
        if (byConfNo >= MAX_CONFIDX || wChanNo >= MAX_SWITCH_CHANNEL)
        {
            continue;
        }

        //����ͬһ��Դ���޸������кš�ʱ���
        if ( dwNewSSRC == g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].dwLastSSRC )
        {
            // zbq [04/10/2007] MP�ڲ�ת�����������ݱ�����ԭ�����кź�ʱ���
            s32 nSeqNumInterval    = 0;
            s32 nTimeStampInterval = 0;
            if ( !g_cMpApp.IsPointMt(wRecvPort) && !g_cMpApp.IsPointSrc(wRecvPort) )
            {
                // ���к�
                nSeqNumInterval = wNewSeqNum - g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].wLastSeqNum;
                g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].nSeqNumInterval = nSeqNumInterval;

                if ( nSeqNumInterval < MAX_SEQNUM_INTERVAL &&
                     nSeqNumInterval > MIN_SEQNUM_INTERVAL )
                {
                    g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].wModifySeqNum += nSeqNumInterval;
                }
                else
                {
                    g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].wModifySeqNum += wSwitchSrcSeqInterval;
                }
                
                // ʱ���
                nTimeStampInterval = dwNewTimeStamp - g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].dwLastTimeStamp;
                g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].nTimeStampInterval = nTimeStampInterval;

                if ( nTimeStampInterval < MAX_TIMESTAMP_INTERVAL &&
                     nTimeStampInterval > MIN_TIMESTAMP_INTERVAL )
                {
                    g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].dwModifyTimeStamp += nTimeStampInterval;
                }
                else
                {
                    g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].dwModifyTimeStamp += wSwitchSrcSeqInterval * DEFAULT_TIMESTAMP_INTERVAL;
                }
            }
        }
        else
        {
            //��һ�α������
            if ( g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].dwModifySSRC == 0 )
            {
                u16 wTempSeqNum     = ((TSendFilterAppData *)ptSends[wSenderIdx].pAppData)->wSeqNum;
                u32 dwTempTimeStamp = ((TSendFilterAppData *)ptSends[wSenderIdx].pAppData)->dwTimeStamp;
                u32 dwTempSSRC      = ((TSendFilterAppData *)ptSends[wSenderIdx].pAppData)->dwSSRC;

                MpLog( LOG_INFO, "[CallBack] dwNewSSRC.%d come first \n", dwTempSSRC );
            
                g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].wModifySeqNum     = wTempSeqNum;
                g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].dwModifyTimeStamp = dwTempTimeStamp;
                g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].dwModifySSRC      = dwTempSSRC;
            }
            else
            {
                // zbq [04/10/2007] MP�ڲ�ת�����������ݱ�����ԭ�����кź�ʱ���
                if ( !g_cMpApp.IsPointMt(wRecvPort) && !g_cMpApp.IsPointSrc(wRecvPort) )
                {
                    g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].wModifySeqNum     += wSwitchSrcSeqInterval;
                    g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].dwModifyTimeStamp += wSwitchSrcSeqInterval * DEFAULT_TIMESTAMP_INTERVAL;
                }
            }
        }

        g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].wLastSeqNum     = wNewSeqNum;  // ���汾������
        g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].dwLastTimeStamp = dwNewTimeStamp;
        g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].dwLastSSRC      = dwNewSSRC;

        
        ptSends[wSenderIdx].wOffset = 0;
        ptSends[wSenderIdx].wLen    = 12;

        // zbq [04/10/2007] MP�ڲ�ת�����������ݱ�����ԭ�����кź�ʱ���
        if ( !g_cMpApp.IsPointMt(wRecvPort) && !g_cMpApp.IsPointSrc(wRecvPort) )
        {
            //����ṹ��Աת��������
            wNewSeqNum     = htons(g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].wModifySeqNum);
            dwNewTimeStamp = htonl(g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].dwModifyTimeStamp);
            dwNewSSRC      = htonl(g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].dwModifySSRC);
        }

		//���Ӷ�RTPͷ�غ�ֵ�Ĺ�һ����
        if (byPayloadType >= AP_MIN && byPayloadType <= AP_MAX && INVALID_PAYLOAD != byPayload)
        {
            u16 wRtpHeadData = ntohs(*(u16 *)pUdpData);
            wRtpHeadData = htons((wRtpHeadData & 0xFF80) + byPayload);			
            *(u16 *)(ptSends[wSenderIdx].pNewData) = wRtpHeadData;
        }
        else
        {
            *(u16 *)(ptSends[wSenderIdx].pNewData) = *(u16 *)pUdpData;
        }
        *(u16 *)(ptSends[wSenderIdx].pNewData + 2) = wNewSeqNum;
        *(u32 *)(ptSends[wSenderIdx].pNewData + 4) = dwNewTimeStamp;
        *(u32 *)(ptSends[wSenderIdx].pNewData + 8) = dwNewSSRC;

        MpLog( LOG_DETAIL, "[CallBack] Payload.%d->Unied for port.%d, SeqNum.%d, TimeStamp.%d��dwNewSSRC.%d !\n", 
                             byPayloadType, wRecvPort, g_cMpApp.m_pptFilterParam[byConfNo][wChanNo].wModifySeqNum, dwNewTimeStamp, dwNewSSRC );
    }

    // zbq [04/03/2007] �㲥������Դ���л��ж�
    u8 byConfIdx = 0;
    u8 byPointId = 0;
    if ( g_cMpApp.IsPointSrc(wRecvPort, &byConfIdx, &byPointId) ) 
    {
        if ( PS_GETSRC   == g_cMpApp.GetPtState(byConfIdx, byPointId) &&
             PS_SWITCHED == g_cMpApp.GetPtState(byConfIdx, byPointId, TRUE) )
        {
            // zbq [04/11/2007] �ж��Ƿ�ؼ�֡�������Ƿ��л���ǰԴ
            if ( g_cMpApp.m_bCancelWaitIFrameNoUni ||
                 CMpBitStreamFilter::IsKeyFrame( byPayloadType, pUdpData, dwUdpLen ) ) 
            {
                g_cMpApp.SetPtState(byConfIdx, byPointId, PS_SWITCHED);
                g_cMpApp.SetPtState(byConfIdx, byPointId, PS_IDLE, TRUE);

                CServMsg cServMsg;
                cServMsg.SetConfIdx( byConfIdx + 1 );
                cServMsg.SetMsgBody( (u8*)&byPointId, sizeof(u8) );

                // zbq [04/30/2007] ֪ͨDeamon�����һ�ν����������Ϣ
                OspPost( MAKEIID(AID_MP, CInstance::DAEMON), 
                         EV_MP_DEAL_CALLBACK_NTF,
                         cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

                MpLog( LOG_INFO, "[CallBack] ConfIdx.%d Pt.%d 's chg to T until IFrame!\n",
                                  byConfIdx, byPointId );
            }
            else
            {
                MpLog( LOG_VERBOSE, "[CallBack] ConfIdx.%d Pt.%d 's state.%d, cut at Port.%d!\n",
                           byConfIdx, byPointId, g_cMpApp.GetPtState(byConfIdx, byPointId), wRecvPort );
            }
        }
        else
        {
            MpLog( LOG_VERBOSE, "[CallBack] ConfIdx.%d, Pt.%d<s:%d>, another<s:%d>, port.%d, no chg!\n",
                                 byConfIdx, 
                                 byPointId, g_cMpApp.GetPtState(byConfIdx, byPointId),
                                 g_cMpApp.GetPtState(byConfIdx, byPointId, TRUE),
                                 wRecvPort );
        }

        if ( PS_SWITCHED != g_cMpApp.GetPtState(byConfIdx, byPointId))
        {
            wSendNum = 0;
        }
    }
    
    *pwSendNum = wSendNum;

    return;
}



/*------------------------------------------------------------- 
 *�������� 
 *-------------------------------------------------------------*/
#define SECTION_switch_table			(const s8*)"swtable"
#define FIELD_SrcIpAddr					(const s8*)"SrcIpAddr"
#define FIELD_DisIpAddr					(const s8*)"DisIpAddr"
#define FIELD_RcvIpAddr					(const s8*)"RcvIpAddr"
#define FIELD_RcvPort					(const s8*)"RcvPort"
#define FIELD_RcvBindIpAddr				(const s8*)"RcvBindIpAddr"
#define FIELD_DstIpAddr					(const s8*)"DstIpAddr"
#define FIELD_DstPort					(const s8*)"DstPort"
#define FIELD_SndBindIpAddr				(const s8*)"SndBindIpAddr"

#define MAX_PORT_LENGTH					(u8)5




#define INET_ADDR( szAddr )   ( strncmp(szAddr, "0", strlen(szAddr)) == 0 ? 0 : inet_addr(szAddr) )


/*=============================================================================
�� �� ���� TableMemoryFree
��    �ܣ� �ͷ�ָ������ڴ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  void **ppMem
u32 dwEntryNum
�� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 TableMemoryFree( void **ppMem, u32 dwEntryNum )
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


BOOL32 AgtGetRegKeyStringTable( const s8* lpszProfileName,    
 							    const s8* lpszSectionName,      
							    const s8* lpszDefault,     
							    s8* *lpszEntryArray, 
							    u32 *pdwEntryNum, 
							    u32 dwBufSize 
										  )
{
	u32 dwEntryNum = *pdwEntryNum;
	BOOL32 bResult = GetRegKeyStringTable( lpszProfileName,    
										   lpszSectionName,      
										   lpszDefault,     
										   lpszEntryArray, 
										   pdwEntryNum, 
										   dwBufSize 
											);
	bResult &= ( dwEntryNum == *pdwEntryNum ) ? TRUE : FALSE;
	return bResult;
}

/*=============================================================================
  �� �� ���� ReadSWTableFromCfg
  ��    �ܣ� 
  �㷨ʵ�֣� д��ȫ�ֽ�����
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 ReadSWTableFromCfg( void )
{
    //����������
    s8    achFileName[32] = {0};

#define PATH_MPCFG			"conf\\swcfg.ini"

#ifdef WIN32
	sprintf( achFileName, "%s/%s", "./conf", "swcfg.ini");
	//strcpy(achFileName, PATH_MPCFG);
#else
	sprintf( achFileName, "%s/%s", "conf", "swcfg.ini");
#endif

	//��Ч��
	FILE *tmpFp = fopen(achFileName, "r");
	if(tmpFp == NULL)
	{
		printf( "[ReadSWTableFromCfg] %s not found.\n", achFileName);
		return FALSE;
	}
	else
	{
		fclose(tmpFp);
	}

	//���û�ȡ
	s8*   *ppszTable = NULL;        // ��ʱ�ռ�
    s8    achSeps[] = " \t";        // �ָ���
    s8    *pchToken = NULL;
    u32   dwLoop;
    s32   nMemEntryNum;
    BOOL32  bResult = TRUE;
    BOOL32  bSucceedRead = TRUE;

	s8     achPortStr[32];        // ��ʱ����ַ���
	memset(achPortStr, '\0', sizeof(achPortStr));

	//������
	bResult = ::GetRegKeyInt( achFileName, "swtable", 
							  "EntryNum", 0, &nMemEntryNum );
    if( !bResult ) 
    {
		printf("[ReadSWTableFromCfg] Wrong profile while reading %s %s!\n", 
				SECTION_switch_table, STR_ENTRY_NUM );
    }
    if( 0 == nMemEntryNum )
    {
        printf("[ReadSWTableFromCfg] The switch num cannot be 0 \n");
        return FALSE;
    }


	//alloc memory
    ppszTable = new s8*[nMemEntryNum];
    if( NULL == ppszTable )
    {
        printf("[ReadSWTableFromCfg] Fail to malloc memory \n");
        return FALSE;
    }
	
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN+1];
        if(NULL == ppszTable[dwLoop])
        {
            printf("[ReadSWTableFromCfg] Fail to malloc memory for board table\n");
            TableMemoryFree( ( void ** )ppszTable, dwLoop );
			return  FALSE;
        }
    }

	u32 dwEntryNum = (u32)nMemEntryNum;

	// ȡ������
	bResult = AgtGetRegKeyStringTable( achFileName, SECTION_switch_table,
									   "fail", ppszTable, &dwEntryNum, MAX_VALUE_LEN + 1 );
	if( !bResult)
	{
		printf( "[ReadSWTableFromCfg] Wrong profile while reading %s! table\n", SECTION_switch_table );
		TableMemoryFree( ( void ** )ppszTable, nMemEntryNum );
		return FALSE;
    }

	// �����ִ�
	for( dwLoop = 0; dwLoop < nMemEntryNum; dwLoop++ )
	{
		TSwitchChannel tSwitchChn;

		// [1].SrcIp
		pchToken = strtok( ppszTable[dwLoop], achSeps );
		if( NULL == pchToken)
		{
			printf("[ReadSWTableFromCfg] Wrong profile while reading %s!\n", FIELD_SrcIpAddr );
			bSucceedRead = FALSE;
			continue;
		}
		else
		{
			tSwitchChn.SetSrcIp(ntohl(INET_ADDR( pchToken )));
		}
		
		// [2].DisIp
		pchToken = strtok( NULL, achSeps );
		if( NULL == pchToken )
		{
			printf("[ReadSWTableFromCfg] Wrong profile while reading %s!\n", FIELD_DisIpAddr );
			bSucceedRead = FALSE;
			continue;
		}
		else
		{
			tSwitchChn.SetDisIp(ntohl(INET_ADDR( pchToken )));
		}

		// [3].RcvIp
		pchToken = strtok( NULL, achSeps );
		if( NULL == pchToken )
		{
			printf("[ReadSWTableFromCfg] Wrong profile while reading %s!\n", FIELD_RcvIpAddr );
			bSucceedRead = FALSE;
			continue;
		}
		else
		{
			tSwitchChn.SetRcvIP(ntohl(INET_ADDR( pchToken )));
		}

		// [4].RcvPort
		pchToken = strtok( NULL, achSeps );
		if( NULL == pchToken )
		{
			printf("[ReadSWTableFromCfg] Wrong profile while reading %s!\n", FIELD_RcvPort );
			bSucceedRead = FALSE;
			continue;
		}
		else
		{
			strncpy( achPortStr, pchToken, MAX_PORT_LENGTH);
			tSwitchChn.SetRcvPort(atoi(achPortStr));
		}

		// [5].RcvBindIp
		pchToken = strtok( NULL, achSeps );
		if( NULL == pchToken )
		{
			printf("[ReadSWTableFromCfg] Wrong profile while reading %s!\n", FIELD_RcvBindIpAddr );
			bSucceedRead = FALSE;
			continue;
		}
		else
		{
			tSwitchChn.SetRcvBindIP(ntohl(INET_ADDR( pchToken )));
		}


		// [6].DstIp
		pchToken = strtok( NULL, achSeps );
		if( NULL == pchToken )
		{
			printf("[ReadSWTableFromCfg] Wrong profile while reading %s!\n", FIELD_DstIpAddr );
			bSucceedRead = FALSE;
			continue;
		}
		else
		{
			tSwitchChn.SetDstIP(ntohl(INET_ADDR( pchToken )));
		}

		// [7].DstPort
		pchToken = strtok( NULL, achSeps );
		if( NULL == pchToken )
		{
			printf("[ReadSWTableFromCfg] Wrong profile while reading %s!\n", FIELD_DstPort );
			bSucceedRead = FALSE;
			continue;
		}
		else
		{
			strncpy( achPortStr, pchToken, MAX_PORT_LENGTH);
			tSwitchChn.SetDstPort(atoi(achPortStr));
		}


		// [8].SndBindIp
		pchToken = strtok( NULL, achSeps );
		if( NULL == pchToken )
		{
			printf("[ReadSWTableFromCfg] Wrong profile while reading %s!\n", FIELD_SndBindIpAddr );
			bSucceedRead = FALSE;
			continue;
		}
		else
		{
			tSwitchChn.SetSndBindIP(ntohl(INET_ADDR( pchToken )));
		}
	}
	
	// �ͷ���ʱ�ռ�
	TableMemoryFree( ( void ** )ppszTable, nMemEntryNum );
    
	return bSucceedRead;
}

/*=============================================================================
  �� �� ���� mpstart
  ��    �ܣ� ����Mp
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TStartMpParam tMpParamMaster
             TMp* ptMp
             TStartMpParam* ptMpParamSlave = NULL
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 mpstart(TStartMpParam tMpParamMaster, TMp* ptMp, TStartMpParam* ptMpParamSlave)
{
    if (!IsOspInitd())
    {
        OspInit(FALSE); 
    }
    
    //LINUX���轫APIע��
    MpAPIEnableInLinux();

    u32 dwMcuIpA = tMpParamMaster.dwMcuIp;
    u32 dwMcuIpB = 0;
    if(NULL != ptMpParamSlave)
    {
        dwMcuIpB = ptMpParamSlave->dwMcuIp;
    }
    // �ж�Ip����Ч��
    if( (0 == dwMcuIpA || INVALID_IPADDR == dwMcuIpA) && 
        (0 == dwMcuIpB || INVALID_IPADDR == dwMcuIpB) )
    {
        MpLog( LOG_CRIT, "[mpstart] param err: MCU.A Ip.0x%x and MCU.B Ip.0x%x\n", dwMcuIpA, dwMcuIpB);
        return FALSE;
    }

    g_cMpApp.CreateApp("Mp", AID_MP, APPPRI_MP, 512, 100 << 10);

    if(!g_cMpApp.Init())
    {
        MpLog( LOG_CRIT, "[mpstart] CMpData::Init() failed!\n");
        return FALSE;
    }
    
    //�Ƿ�ȹؼ�֡
    s8    achProfileName[32];
    s32   nValue = 0;
    sprintf( achProfileName, "%s/%s", DIR_CONFIG, "modcfg.ini");
    
    // guzh [3/9/2007] ȱʡΪ���ȹؼ�֡
    if(GetRegKeyInt( achProfileName, "mpcfg", "waitingIFrame", 0, &nValue))
    {
        g_cMpApp.m_bIsWaitingIFrame = (0 != nValue) ? TRUE : FALSE;
    }    

    //Send filter parameter inital
    g_cMpApp.MpSendFilterParamInit();

	//�̶�������Ļ�ȡ
	if (!ReadSWTableFromCfg())
	{
		printf("[mpstart] read switch table from cfg file failed !\n");
		//return FALSE;
	}

    OspPost(MAKEIID(AID_MP, CInstance::DAEMON), OSP_POWERON);
    
    if(FALSE == g_cMpApp.CreateDS())
    {
        MpLog( LOG_CRIT, "[mpstart] Create Data Switch Failure !\n");
        return FALSE;
    }

    sprintf(g_cMpApp.m_abyMpAlias, "Mp%d", ptMp->GetMpId());
    g_cMpApp.m_byAttachMode = ptMp->GetAttachMode();
    g_cMpApp.m_byMpId       = ptMp->GetMpId();
    g_cMpApp.m_byMcuId      = ptMp->GetMcuId();
    g_cMpApp.m_dwMcuIpAddr  = tMpParamMaster.dwMcuIp;
    g_cMpApp.m_wMcuPort     = tMpParamMaster.wMcuPort;
	g_cMpApp.m_dwIpAddr     = ptMp->GetIpAddr();
    
    if(1 == g_cMpApp.m_byAttachMode)
    {
        g_cMpApp.m_bEmbedA = TRUE;
        g_cMpApp.m_dwMcuNode = 0;
        ::OspPost(MAKEIID(AID_MP, CInstance::DAEMON), EV_MP_REGISTER_MCU, NULL, 0);
    }
    else
    {
        g_cMpApp.m_bEmbedA = FALSE;
    }
    
    if(NULL != ptMpParamSlave) // g_cMpApp.m_byAttachModeΪ1ʱ,�Ѿ���ptMpParamSlave��ΪNull
    {
        g_cMpApp.m_dwMcuIpAddrB = ptMpParamSlave->dwMcuIp;
        g_cMpApp.m_wMcuPortB  = ptMpParamSlave->wMcuPort;
        if(g_cMpApp.m_dwIpAddr == ntohl(g_cMpApp.m_dwMcuIpAddrB))
        {
            g_cMpApp.m_bEmbedB = TRUE;
            g_cMpApp.m_dwMcuNodeB = 0;
            ::OspPost(MAKEIID(AID_MP, CInstance::DAEMON), EV_MP_REGISTER_MCUB, NULL, 0);
        }
        else
        {
            g_cMpApp.m_bEmbedB = FALSE;
        }
    }
    else
    {
        g_cMpApp.m_dwMcuIpAddrB = 0;
        g_cMpApp.m_wMcuPortB  = 0;
    }
    
    if( 0 != g_cMpApp.m_dwMcuIpAddr && !g_cMpApp.m_bEmbedA )
	{
        ::OspPost(MAKEIID(AID_MP, CInstance::DAEMON), EV_MP_CONNECT_MCU, NULL, 0);
	}

    if( 0 != g_cMpApp.m_dwMcuIpAddrB && !g_cMpApp.m_bEmbedB )
	{
        ::OspPost(MAKEIID(AID_MP, CInstance::DAEMON), EV_MP_CONNECT_MCUB, NULL, 0);
	}

    // guzh [7/11/2007] �ƶ������ȷ������PXY����MP8000B������WD
#ifdef _LINUX_    
    WDStartWatchDog( emMP );    
#endif
        
	return TRUE;
}

/*=============================================================================
  �� �� ���� mpstart
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwMcuTcpNode
             u32 dwMcuIp
             u16 wMcuPort
             TMp *ptMp
             TStartMpParam* ptMpParamSlave
  �� �� ֵ�� BOOL32 
/*=============================================================================*/
BOOL32 mpstart( u32 dwMcuTcpNode, u32 dwMcuIp, u16 wMcuPort, TMp *ptMp, TStartMpParam* ptMpParamSlave)
{
    if(NULL == ptMp)
    {
        MpLog( LOG_CRIT, "[Mp] The Tmp struct cannot be Null\n");
        return FALSE;
    }

    BOOL32 bIsHaveSlave = TRUE;
    if(NULL != ptMpParamSlave)
    {
        if(ptMpParamSlave->dwMcuIp == dwMcuIp )
        {
            bIsHaveSlave = FALSE;
        }
        if( 0 == dwMcuIp && 0 == ptMpParamSlave->dwMcuIp )
        {
            MpLog( LOG_CRIT, "[Mp] The McuA and McuB's Ip are all 0 !\n");
            return FALSE;
        }
    }

    TStartMpParam tMpParamMaster;   
    tMpParamMaster.dwMcuIp  = dwMcuIp;
    tMpParamMaster.wMcuPort = wMcuPort;
    
    if(1 == g_cMpApp.m_byAttachMode)  // Ƕ����Mcu��,��MpǶ��������ģ����ʱ,ֻ֧��һ��Mpc��
    {
        tMpParamMaster.dwMcuIp = htonl(ptMp->GetIpAddr());  // Mcu Ip Ӧ����������
        ptMpParamSlave = NULL;
    }

    BOOL32 bRet = TRUE;
    if(FALSE == bIsHaveSlave)
    {
        bRet = mpstart(tMpParamMaster, ptMp, NULL);
    }
    else
    {
        bRet = mpstart(tMpParamMaster, ptMp, ptMpParamSlave);
    }

    return bRet;
}

/*=============================================================================
  �� �� ���� mpstop
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
=============================================================================*/
void mpstop()
{
    OspDelay(1000);
    g_cMpApp.DestroyDS();
    OspQuit();
    
    return;
}

/*=============================================================================
  �� �� ���� strofip
  ��    �ܣ� �õ�Ip�ַ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwIp
  �� �� ֵ�� s8 * 
=============================================================================*/
s8 * strofip(u32 dwIp)
{
	static char g_strIp[18];  
	memset(g_strIp, 0, 18);   // clear

    u32 dwTmpIp = dwIp;
	sprintf(g_strIp, "%d.%d.%d.%d%c", (dwTmpIp>>24)&0xFF, (dwTmpIp>>16)&0xFF, (dwTmpIp>>8)&0xFF, dwTmpIp&0xFF, 0);

	return g_strIp;
}

/*=============================================================================
  �� �� ���� strofip
  ��    �ܣ� �õ�Ip�ַ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwIp
             char* StrIp
  �� �� ֵ�� void 
=============================================================================*/
void strofip(u32 dwIp, s8* StrIp)
{
    u32 dwTmpIp = dwIp;
    sprintf(StrIp, "%d.%d.%d.%d%c", (dwTmpIp>>24)&0xFF, (dwTmpIp>>16)&0xFF, (dwTmpIp>>8)&0xFF, dwTmpIp&0xFF, 0);
    return;
}

/*=============================================================================
�� �� ���� mpinit
��    �ܣ� mp��ʼ��api�ӿ�, ��������ǽ����ʹ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� API BOOL 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/4/11  4.0			������                  ����
=============================================================================*/
#ifdef _LINUX_

#include "boardwrapper.h"

#ifndef OUT
#define OUT
#endif

#ifdef _MDSC_BRD_

/*=============================================================================
  �� �� ���� ReadBoardConfig
  ��    �ܣ� ȡMpc1��ַ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� OUT u8& byBoardId
             OUT u32& dwMcuIp
             OUT u16& wMcuPort
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 MpReadBoardConfig(OUT u8& byBoardId, OUT u32& dwMcuIp, OUT u16& wMcuPort, OUT u8& byEthId )
{
	char    achProfileName[32];
    BOOL    bResult;
    char    achDefStr[] = "Cannot find the section or key";
    char    achReturn[MAX_VALUE_LEN + 1];
    s32     nValue;
    
    printf("The Dsc defined  _CONFIG_FROM_FILE \n");

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);
    
    byBoardId = 1;

    bResult = GetRegKeyString( achProfileName, "BoardSystem", "MpcIpAddr", 
                               achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult   )  
	{
		MpLog( LOG_CRIT, "Wrong profile while reading %s!\n", "MpcIpAddr" );
		return( FALSE );
	}
    dwMcuIp =  INET_ADDR( achReturn );


	bResult = GetRegKeyInt( achProfileName, "BoardSystem", "MpcPort", 0, &nValue );
	if( FALSE == bResult )  
	{
		MpLog( LOG_CRIT, "Wrong profile while reading %s!\n", "MpcPort" );
		return( FALSE );
	}
	wMcuPort = ( u16 )nValue;

    // guzh [10/31/2007] MDSC ֻ��ǰ���ڣ����ﲻȥ��ȡ����ֹ������
    byEthId = 0;
    /*
	bResult = GetRegKeyInt( achProfileName, "IsFront", "Flag", 0, &nValue );
	if( FALSE == bResult )  
	{
		MpLog( LOG_CRIT, "Wrong profile while reading %s!\n", "Flag" );
		nValue = 0;
	}
	byEthId = (u8)nValue;
    */

    return TRUE;
}

u32 g_dwMpcIpAddr = 0;
u16 g_wMpcPort    = 0;

/*=============================================================================
  �� �� ���� main
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
#ifdef WIN32
#ifndef _LIB
void main(void)
{
    TStartMpParam tMpParamMaster;
    TMp tMp;
    mpstart(tMpParamMaster, &tMp, NULL);
    Sleep(INFINITE);
}
#endif
#endif


//=======================================================//
//                                                       //
//                        API                            //
//                                                       //
//=======================================================//

/*=============================================================================
  �� �� ���� BrdGetDstMcuNode
  ��    �ܣ� mp ��ʼ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� API u32 
=============================================================================*/
API u32 BrdGetDstMcuNode( void )
{
    u32 dwMpcNode = ::OspConnectTcpNode( htonl(g_dwMpcIpAddr), g_wMpcPort, 10, 3, 100 );
    printf("the mpc ip= %0x, port= %d. node= %d.\n", g_dwMpcIpAddr, g_wMpcPort, dwMpcNode);
    return dwMpcNode;
}

/*=============================================================================
  �� �� ���� BrdGetDstMcuNodeB
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� API u32 
=============================================================================*/
API u32 BrdGetDstMcuNodeB(void)
{
    return 0;
}

/*=============================================================================
  �� �� ���� mpinit
  ��    �ܣ� mp ��ʼ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� API BOOL 
=============================================================================*/
API BOOL mpinit(void)
{
    u32 dwMpcIpAddr = 0;
    u16 wMpcPort = 0;
    u32 dwRegisterIp = 0;

    u32 dwMpcIpAddrB = 0;
    u16 wMpcPortB = 0;
    u8  byBoardId = 1;
	u8  byEthId = 0;

	BrdInit();
	
	BOOL32 bRet = MpReadBoardConfig( byBoardId, dwMpcIpAddr, wMpcPort, byEthId );
	printf("\n[mpinit] Mcu Ip= %0x, port= %d. byEthId= %d.\n", dwMpcIpAddr, wMpcPort, byEthId);
	if( !bRet )
	{
		printf("[mpinit] Read config file fail.\n");
		return FALSE;
	}
	
	g_dwMpcIpAddr = dwMpcIpAddr;
	g_wMpcPort    = wMpcPort;
	
	TBrdEthParam tBrdEthParam;
	u8 byRet = BrdGetEthParam( byEthId, &tBrdEthParam );
	dwRegisterIp = tBrdEthParam.dwIpAdrs;
	printf("[mpinit] local ip = %0x. return value = %d.\n", dwRegisterIp, byRet );
    
	if( OK != byRet )
	{
		printf("[mpinit] fail to call BrdGetEthParam , Error= %d.\n", byRet );
		return FALSE;
	}

    TMp tMp;
    tMp.SetIpAddr( ntohl(dwRegisterIp) );
    tMp.SetMpId(byBoardId);
    tMp.SetAttachMode(2);
    tMp.SetMcuId(LOCAL_MCUID);
    
    TStartMpParam tMpParamSlave;
    tMpParamSlave.dwMcuIp = dwMpcIpAddrB;
    tMpParamSlave.wMcuPort = wMpcPortB;

    if( !mpstart(0, dwMpcIpAddr, wMpcPort, &tMp, &tMpParamSlave) )
    {
        MpLog( LOG_CRIT, "[mpinit] starting mp failed.\n");
        return FALSE;
    }

    return TRUE;
}
#endif // _MDSC_BRD_

#endif // _LINUX


/*=============================================================================
  �� �� ���� psw - short for print switch
  ��    �ܣ� ��ʾ���н���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� API void 
=============================================================================*/
API void psw()
{
//    if (g_pMpInst)
//    {
//        g_pMpInst->ShowSwitch();
//    }
    OspPost( MAKEIID(AID_MP, CInstance::DAEMON), EV_SWITCHTABLE_PRTNEXTPAGE_TEMER);
}

/*=============================================================================
  �� �� ���� psw - short for print switch
  ��    �ܣ� ��ʾ���н���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� API void 
=============================================================================*/
API void wsw()
{
    g_cMpApp.ShowWholeSwitch();
}

/*=============================================================================
  �� �� ���� cw - cancel wait IFram
  ��    �ܣ� ȡ���ȴ��ؼ�֡
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� API void 
=============================================================================*/
API void cw()
{
    g_cMpApp.m_bCancelWaitIFrameNoUni = TRUE;
}

/*=============================================================================
  �� �� ���� rw - restore wait IFram
  ��    �ܣ� �ָ��ȴ��ؼ�֡
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� API void 
=============================================================================*/
API void rw()
{
    g_cMpApp.m_bCancelWaitIFrameNoUni = FALSE;
}

/*=============================================================================
  �� �� ���� showSCUniform
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� API void 
=============================================================================*/
API void showSCUniform()
{
    g_cMpApp.ShowSwitchUniformInfo();
}

/*=============================================================================
  �� �� ���� mpver
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� API void 
=============================================================================*/
API void mpver()
{
    static s8 gs_VersionBuf[128] = {0};
	
    strcpy(gs_VersionBuf, KDV_MCU_PREFIX);
    
    s8 achMon[16] = {0};
    u32 byDay = 0;
    u32 byMonth = 0;
    u32 wYear = 0;
    static s8 achFullDate[24] = {0};
    
    s8 achDate[32] = {0};
    sprintf(achDate, "%s", __DATE__);
    StrUpper(achDate);
    
    sscanf(achDate, "%s %d %d", achMon, &byDay, &wYear );
    
    if ( 0 == strcmp( achMon, "JAN") )		 
        byMonth = 1;
    else if ( 0 == strcmp( achMon, "FEB") )
        byMonth = 2;
    else if ( 0 == strcmp( achMon, "MAR") )
        byMonth = 3;
    else if ( 0 == strcmp( achMon, "APR") )		 
        byMonth = 4;
    else if ( 0 == strcmp( achMon, "MAY") )
        byMonth = 5;
    else if ( 0 == strcmp( achMon, "JUN") )
        byMonth = 6;
    else if ( 0 == strcmp( achMon, "JUL") )
        byMonth = 7;
    else if ( 0 == strcmp( achMon, "AUG") )
        byMonth = 8;
    else if ( 0 == strcmp( achMon, "SEP") )		 
        byMonth = 9;
    else if ( 0 == strcmp( achMon, "OCT") )
        byMonth = 10;
    else if ( 0 == strcmp( achMon, "NOV") )
        byMonth = 11;
    else if ( 0 == strcmp( achMon, "DEC") )
        byMonth = 12;
    else
        byMonth = 0;
    
    if ( byMonth != 0 )
    {
        sprintf(achFullDate, "%04d%02d%02d", wYear, byMonth, byDay);
        sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
    else
    {
        // for debug information
        sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
	OspPrintf( TRUE, FALSE, "Mp Version: %s\n", gs_VersionBuf  );
	
	OspPrintf(TRUE, FALSE, "Mp  Module version: %s    compile time: %s, %s \n", 
                            VER_MP, __TIME__, __DATE__);
}

/*=============================================================================
  �� �� ���� setmplog
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� API void 
=============================================================================*/
API void setmplog(u8 byLvl)
{
    if ( byLvl > LOG_VERBOSE )
    {
        byLvl = LOG_VERBOSE;
    }
    g_byMpPrtLvl = byLvl;
    return;
}

/*=============================================================================
  �� �� ���� mptau
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� API void 
=============================================================================*/

API void mptau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
}

/*=============================================================================
  �� �� ���� mpshownetstat
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� API void 
=============================================================================*/
API void mpshownetstat()
{
    g_cNetTrfcStat.DumpAllStat();
}

/*=============================================================================
  �� �� ���� pmpnetstatmsg
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� API void 
=============================================================================*/
API void pmpnetstatmsg()
{
    g_bPrintMpNetStat = !g_bPrintMpNetStat;
}

/*=============================================================================
  �� �� ���� ppt : print point state
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� API void 
=============================================================================*/
API void ppt( void )
{
    BOOL32 bExist = FALSE;
    for( u8 byIdx = 0; byIdx < MAX_CONFIDX; byIdx ++ )
    {
        bExist = FALSE;

        if ( PS_IDLE != g_cMpApp.GetPtState(byIdx, 1) ||
             PS_IDLE != g_cMpApp.GetPtState(byIdx, 2) ) 
        {
            bExist = TRUE;
        }
        if ( bExist )
        {
            OspPrintf( TRUE, FALSE, "\nConfIdx.%d: point<%d : %d> \n\n",
                                     byIdx, g_cMpApp.GetPtState(byIdx, 1), g_cMpApp.GetPtState(byIdx, 2));
        }
    }
}

/*=============================================================================
  �� �� ���� mphelp
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� API void 
=============================================================================*/
API void mphelp()
{
    mpver();
    OspPrintf(TRUE, FALSE, "setmplog(u8)    -->set print level: 0.critical(default), 1,err, 2.warning, 3.info, 4.detail, 5.verbose!\n");
	OspPrintf(TRUE, FALSE, "psw()           -->show mp switch info!\n");
    OspPrintf(TRUE, FALSE, "showSCUniform() -->show uniform info!\n");
	OspPrintf(TRUE, FALSE, "dshelp()        -->show dataswitch help info!\n");
    OspPrintf(TRUE, FALSE, "mptau(UsrName, Pwd)-->mp telnet author!\n");
    OspPrintf(TRUE, FALSE, "mpshownetstat   -->Show MP All Net Traffic Statics!\n");
    OspPrintf(TRUE, FALSE, "pmpnetstatmsg   -->Enable/Disable MP Net Traffic Timely Print!\n");
    OspPrintf(TRUE, FALSE, "ppt             -->print point state!\n");
    OspPrintf(TRUE, FALSE, "showhandle      -->show sem handle!\n");
}

/*=============================================================================
  �� �� �� showhandle
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� API void 
=============================================================================*/
API void showhandle()
{
    g_cMpApp.ShowHandle();
}

API void mpAdd(LPCSTR pRcvIp, u16 wRcvPort, LPCSTR pRcvBindIp,
			   LPCSTR pDstIp, u16 wDstPort, LPCSTR pDstBindIp)
{
	if (NULL == pRcvIp ||
		NULL == pDstIp ||
		NULL == pRcvBindIp ||
		NULL == pDstBindIp )
	{
		return;
	}
	s32 nRet = ::dsAdd(g_cMpApp.m_nInitUdpHandle,
						INET_ADDR(pRcvIp),
						wRcvPort,
						INET_ADDR(pRcvBindIp),
						INET_ADDR(pDstIp),
						wDstPort,
						INET_ADDR(pDstBindIp));
	if (DSOK != nRet)
	{
		OspPrintf(TRUE, FALSE, "dsAdd failed, ret.%d!\n", nRet);
	}
}


API void mpRemove(LPCSTR pDstIp, u16 wDstPort)
{
	if (NULL == pDstIp )
	{
		return;
	}
	s32 nRet = ::dsRemove(g_cMpApp.m_nInitUdpHandle,
							INET_ADDR(pDstIp),
							wDstPort);
	if (DSOK != nRet)
	{
		OspPrintf(TRUE, FALSE, "dsRemove failed, ret.%d!\n", nRet);
	}

}

/*=============================================================================
  �� �� ���� MpAPIEnableInLinux
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� API void 
=============================================================================*/
void MpAPIEnableInLinux( void )
{
#ifdef _LINUX_        
    OspRegCommand("mpver",         (void*)mpver,         "MP version command");
    OspRegCommand("mphelp",        (void*)mphelp,        "MP help command");
    OspRegCommand("setmplog",      (void*)setmplog,      "Mp print level set command");
    OspRegCommand("psw",           (void*)psw,           "MP print switch command");
    OspRegCommand("showSCUniform", (void*)showSCUniform, "Show SCUniform command");
    OspRegCommand("mptau",         (void*)mptau,         "MP telnet author");
    OspRegCommand("mpshownetstat", (void*)mpshownetstat, "MP Show Net Traffic");
    OspRegCommand("pmpnetstatmsg", (void*)pmpnetstatmsg, "MP show Net Traffic Timely Message");
    OspRegCommand("ppt",           (void*)ppt,           "Mp print point state");
    OspRegCommand("showhandle",    (void*)showhandle,    "Mp show sem handle");
    OspRegCommand("cw",            (void*)cw,            "Mp cancel wait IFrame");
    OspRegCommand("rw",            (void*)rw,            "Mp restore wait IFrame");
	OspRegCommand("mpAdd",         (void*)mpAdd,            "Mp restore wait IFrame");
	OspRegCommand("mpRemove",      (void*)mpRemove,            "Mp remove ds switch");
#endif
}

/*=============================================================================
  �� �� ���� MpLog
  ��    �ܣ� 
  �㷨ʵ�֣� Z
  ȫ�ֱ����� 
  ��    ���� u8 byPrtLvl, s8* pszFmt, ...
             ...
  �� �� ֵ�� void 
=============================================================================*/
void MpLog( u8 byPrtLvl, s8* pszFmt, ...)
{
    s8 achPrintBuf[255];
    s32 nBufLen = 0;
    va_list argptr;
	if ( byPrtLvl <= g_byMpPrtLvl )
	{
        OspPrintf( TRUE, FALSE, "TICK.%d ", OspTickGet() );
		nBufLen = sprintf(achPrintBuf, "[MP]: ");
		va_start(argptr, pszFmt);
		nBufLen += vsprintf(achPrintBuf+nBufLen, pszFmt, argptr);
		va_end(argptr);
		OspPrintf(TRUE, FALSE, achPrintBuf);
	}
    return;
}


void CBNetSWRcvRtpPk(TRtpPack *pRtpPack, u32 dwContext)
{
	CNetSwitch *pcNetSW = (CNetSwitch*)dwContext;
	if (NULL == pcNetSW)
	{
		return;
	}
	pcNetSW->m_cMediaSnd.Send(pRtpPack);
}


void CNetSwitch::Init(TSwitchChannel tSwitchChn, u8 bySndPT)
{
	//���ն���
	u16 wRet = m_cMediaRcv.Create(MAX_FRAME_SIZE,
									CBNetSWRcvRtpPk,
									(u32)this);
	if (wRet != MEDIANET_NO_ERROR)
	{
		OspPrintf(TRUE, FALSE, "[CNetSwitch::Init] m_cMediaRcv.Create failed!\n");
	}
	
	//���Ͷ���
	wRet = m_cMediaSnd.Create(MAX_FRAME_SIZE,
								NET_SW_NETBAND,
								30,
								bySndPT);
	if (wRet != MEDIANET_NO_ERROR)
	{
		OspPrintf(TRUE, FALSE, "[CNetSwitch::Init] m_cMediaSnd.Create failed!\n");
	}
	
	//���ղ���
	TLocalNetParam tParam;
	tParam.m_tLocalNet.m_dwRTPAddr = tSwitchChn.GetRcvIP();
	tParam.m_tLocalNet.m_wRTPPort = RCV_PORT_OFFSET + tSwitchChn.GetRcvPort() % 10000;

	m_cMediaRcv.SetNetRcvLocalParam(tParam);

	//���Ͳ���
	TNetSndParam tSndParam;
	tSndParam.m_byNum = 1;
	tSndParam.m_tLocalNet.m_dwRTPAddr = tSwitchChn.GetRcvIP();
	tSndParam.m_tLocalNet.m_wRTPPort = SND_PORT_OFFSET + tSwitchChn.GetRcvPort() % 10000;
	tSndParam.m_tRemoteNet[0].m_dwRTPAddr = tSwitchChn.GetDstIP();
	tSndParam.m_tRemoteNet[0].m_wRTPPort = tSwitchChn.GetDstPort();
	m_cMediaSnd.SetNetSndParam(tSndParam);

	//�����շ�
	m_cMediaRcv.StartRcv();
	

	m_tSWChn = tSwitchChn;

	return;
}

void CNetSwitch::Quit()
{
	m_cMediaRcv.StopRcv();

	m_cMediaRcv.RemoveNetRcvLocalParam();
	m_cMediaSnd.RemoveNetSndLocalParam();

	SetNull();
}


BOOL32 CNetSwitch::IsNull()
{
	return m_tSWChn.IsNull();
}

void CNetSwitch::SetNull()
{
	m_tSWChn.SetNull();
}


// CNetSWGrp::CNetSWGrp()
// {
// 	memset(this, 0, sizeof(CNetSWGrp));
// }

u8 CNetSWGrp::GetIdleNetSW()
{
	u8 byIdx = 0;
	for (; byIdx < MAXNUM_NET_SW; byIdx ++)
	{
		if (m_atNetSW[byIdx].IsNull())
		{
			return byIdx;
		}
	}
	if (byIdx >= MAXNUM_NET_SW)
	{
		OspPrintf(TRUE, FALSE, "[GetIdleNetSW] get idle sw failed, check it!\n");
	}
	
	return 0xff;
}

void CNetSWGrp::InitNetSW(u8 byIdx, TSwitchChannel tSwitchChn, u8 bySndPT)
{
	m_atNetSW[byIdx].Init(tSwitchChn, bySndPT);
	return;
}

void CNetSWGrp::QuitNetSW(TSwitchChannel tSwitchChn, u8 bySndPT)
{
	u8 byIdx = 0;
	for (; byIdx < MAXNUM_NET_SW; byIdx ++)
	{
		if (tSwitchChn.GetDstIP() == m_atNetSW[byIdx].m_tSWChn.GetDstIP() &&
			tSwitchChn.GetDstPort() == m_atNetSW[byIdx].m_tSWChn.GetDstPort())
		{
			m_atNetSW[byIdx].Quit();
			break;
		}
	}
	if (byIdx >= MAXNUM_NET_SW)
	{
		OspPrintf(TRUE, FALSE, "[QuitNetSW] switch<0x%x@%d> quit failed, check it!\n",
			tSwitchChn.GetDstIP(), tSwitchChn.GetDstPort());
	}
}

void CNetSWGrp::QuitNetSW(u8 byIdx)
{
	m_atNetSW[byIdx].Quit();
	return;
}


// END OF FILE
