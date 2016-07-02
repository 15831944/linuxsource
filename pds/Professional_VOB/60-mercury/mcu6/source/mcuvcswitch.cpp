/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcuvcswitch.cpp
   ����ļ�    : mcuvc.h
   �ļ�ʵ�ֹ���: MCUҵ�񽻻�����
   ����        : ������
   �汾        : V2.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/06  2.0         ������      ����
   2005/02/19  3.6         ����      �����޸ġ���3.5�汾�ϲ�
******************************************************************************/

#include "evmcumcs.h"
#include "evmcumt.h"
#include "evmcueqp.h"
#include "evmp.h"
#include "evmcu.h"
#include "evmcutest.h"
#include "mcuvc.h"
#include "mcsssn.h"
#ifdef _8KH_
	#include "mpssn.h"
#endif
//#include "mcuerrcode.h"
//#include "mpmanager.h"
//#include "mcuutility.h"

/*====================================================================
    ������      ��StartSwitchToSubMt
    ����        ����ָ���ն����ݽ������¼�ֱ���Ǵμ��նˣ�����Ƿ�ֱ���ն���
				  �����н���ֱ�ӷ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt & tSrc, Դ
				  u8 bySrcChnnl,    Դ���ŵ���
				  u8 byDstMtId,     Ŀ���ն�
				  u8 byMode,        ������ʽ����Ƶ������Ƶ����ȱʡΪMODE_BOTH 
                  bySwitchMode��    ������ʽ��ȱʡ��SWITCH_MODE_BROADCAST��
                  bIsSrcBrd��       ��Դ�ն��Ƿ��ǹ㲥Դ
    ����ֵ˵��  ��TRUE�����������ֱ�ӷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/12    1.0         LI Yi         ����
	03/07/23    2.0         ������        �޸�
	04/03/06    3.0         ������        �޸�
    07/02/13    4.0         ����        �޸�
====================================================================*/
// BOOL32 CMcuVcInst::StartSwitchToSubMt(TMt   tSrc, 
//                                       u8    bySrcChnnl,
//                                       u8    byDstMtId,
//                                       u8    byMode,
//                                       u8    bySwitchMode,
//                                       BOOL32 bMsgStatus,
//                                       BOOL32 bSndFlowCtrl, BOOL32 bIsSrcBrd,
// 										u16 wSpyPort )
// {
//     TMtStatus tMtStatus;
//     CServMsg  cServMsg;
//     u8 byMediaMode = byMode;
//     BOOL32 bSwitchedAudio = FALSE;
// 
// 	//zjj20100201
// 	//  [11/26/2009 pengjie] Modify ������ش�֧��
// 	TMt tUnlocalSrc = tSrc;
// 	// End
// 
//     tSrc = GetLocalMtFromOtherMcuMt(tSrc);
// 
// 	//����KDC�ն��Ƿ�������״̬�����򲻽�����Ƶ����	
// 	if( TYPE_MT == tSrc.GetType() && 
// 		MT_MANU_KDC != m_ptMtTable->GetManuId( tSrc.GetMtId() ) && 
// 		m_ptMtTable->IsMtAudioDumb( tSrc.GetMtId() ) )
// 	{
// 		if( byMediaMode == MODE_AUDIO )
// 		{
// 			return TRUE;
// 		}
// 		else if ( byMediaMode == MODE_BOTH )
// 		{
// 			byMediaMode = MODE_VIDEO;
// 		}
// 	}
// 
// 	//����KDC�ն��Ƿ��ھ���״̬����������������Ƶ����	
// 	if( MT_MANU_KDC != m_ptMtTable->GetManuId( byDstMtId ) && 
// 		m_ptMtTable->IsMtAudioMute( byDstMtId ) )
// 	{
// 		if( byMediaMode == MODE_AUDIO )
// 		{
// 			return TRUE;
// 		}
// 		else if ( byMediaMode == MODE_BOTH )
// 		{
// 			byMediaMode = MODE_VIDEO;
// 		}
// 	}
// 
// 	u32 dwTimeIntervalTicks = 3*OspClkRateGet();
// 
//     BOOL32 bMMcuNeedAdp = FALSE;
// 
// 	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();
// 	
// 	//ֻ���ϼ�Mcu��spyMt
// 	TMt tDstMt = m_ptMtTable->GetMt( byDstMtId );
// 	if(m_tCascadeMMCU.GetMtId() != 0 && byDstMtId == m_tCascadeMMCU.GetMtId())
// 	{
// 		//////////////////////////////////////////////////////////////////////////
// 		//zjj20100201 ��ش�����
// 		u16 wSpyPort1 = SPY_CHANNL_NULL;	
// 		CSendSpy cSendSpy;
// 		wSpyPort1 = SPY_CHANNL_NULL;
// 		if( TYPE_MT == tSrc.GetType() &&
// 			!m_cLocalSpyMana.GetSpyChannlInfo(tSrc, cSendSpy) )
// 		{
// 			u16 wMMcuIdx = GetMcuIdxFromMcuId( tDstMt.GetMtId() );
// 			//m_tConfAllMcuInfo.GetIdxByMcuId( tDstMt.GetMtId(),0,&wMMcuIdx );
// 			//zjj20100329 ���ϼ�mcu���������Ƕ�ش��նˣ����ǻش�ͨ���е��նˣ���������Ǿ�ֱ�ӷ���
// 			TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx);//tDstMt.GetMtId());
// 			if ( NULL == ptConfMcInfo ) 
// 			{
// 				return FALSE;
// 			} 
// 			if( !(ptConfMcInfo->m_tSpyMt.GetMtId() == tSrc.GetMtId()&&
// 				  ptConfMcInfo->m_tSpyMt.GetMcuId() == tSrc.GetMcuId())
// 				  )
// 			{
//                 return FALSE;
// 			}			
// 		}	
// 		 //�ж�spymt���ϼ��Ƿ���Ҫ����
//         TSimCapSet tSrcSimCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
//         TSimCapSet tDstSimCap = m_ptMtTable->GetDstSCS(byDstMtId);
//         
//         if (tSrcSimCap.IsNull())
//         {
//             ConfLog(FALSE, "[StartSwitchToSubMt] tSrc.%d primary logic chan unexist!\n", tSrc.GetMtId());
//             return FALSE;
//         }
//         tSrcSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtSndBitrate(tSrc.GetMtId()));
//         
//         if (tDstSimCap.IsNull())
//         {
//             ConfLog(FALSE, "[StartSwitchToSubMt] tDst.%d back logic chan unexist!\n", byDstMtId);
//             return FALSE;
//         }
//         tDstSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtReqBitrate(byDstMtId));
//         
//         if (tDstSimCap < tSrcSimCap)
//         {
//             bMMcuNeedAdp = TRUE;
//         }
// 		//////////////////////////////////////////////////////////////////////////
// 		
// 		/*TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tDstMt.GetMtId());
// 		if( ptConfMcInfo != NULL && 
// 			( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
// 		{
// 			if( TYPE_MT == tSrc.GetType() &&
//                 !(ptConfMcInfo->m_tSpyMt.GetMtId() == tSrc.GetMtId()&&
// 				  ptConfMcInfo->m_tSpyMt.GetMcuId() == tSrc.GetMcuId()))
// 			{
//                 return FALSE;
// 			}
// 
//             //�ж�spymt���ϼ��Ƿ���Ҫ����
//             TSimCapSet tSrcSimCap = m_ptMtTable->GetSrcSCS(ptConfMcInfo->m_tSpyMt.GetMtId());
//             TSimCapSet tDstSimCap = m_ptMtTable->GetDstSCS(byDstMtId);
//             
//             if (tSrcSimCap.IsNull())
//             {
//                 ConfLog(FALSE, "[StartSwitchToSubMt] tSrc.%d primary logic chan unexist!\n", ptConfMcInfo->m_tSpyMt.GetMtId());
//                 return FALSE;
//             }
//             tSrcSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtSndBitrate(ptConfMcInfo->m_tSpyMt.GetMtId()));
//             
//             if (tDstSimCap.IsNull())
//             {
//                 ConfLog(FALSE, "[StartSwitchToSubMt] tDst.%d back logic chan unexist!\n", byDstMtId);
//                 return FALSE;
//             }
//             tDstSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtReqBitrate(byDstMtId));
//             
//             if (tDstSimCap < tSrcSimCap)
//             {
//                 bMMcuNeedAdp = TRUE;
//             }
// 		}
// 		//zjj20100201
// 		g_cMpManager.SetSwitchBridge(tSrc, 0, byMediaMode,FALSE,wSpyPort );
// 		*/
// 	}
//     
// 	//ֻ�ղ��� 
// 	TMt TNullMt;
// 	TNullMt.SetNull();
// 	if( byDstMtId == 0 )
// 	{
//         g_cMpManager.StartSwitchToSubMt( tSrc, bySrcChnnl, TNullMt, byMediaMode, bySwitchMode,
// 									FALSE,TRUE
// 									,FALSE,wSpyPort 
// 									);
// 		return TRUE;
// 	}
// 
// 	//ֻ�����ն�, ���ý��н���
// 	m_ptMtTable->GetMtStatus(byDstMtId, &tMtStatus);
// 	if( ( (!tMtStatus.IsReceiveVideo()) && byMediaMode == MODE_VIDEO ) ||
// 		( (!tMtStatus.IsReceiveAudio()) && byMediaMode == MODE_AUDIO ) )
// 	{
// 		return FALSE;
// 	}
// 
//     //Ŀ���ն��Ƿ��ڻ���
//     if (tMtStatus.IsMediaLoop(MODE_VIDEO))
//     {
//         if (byMediaMode == MODE_VIDEO)
//         {
//             return TRUE;
//         }
//         else if (byMediaMode == MODE_BOTH)
//         {
//             byMediaMode = MODE_AUDIO;
//         }
//     }
//     
//     if (tMtStatus.IsMediaLoop(MODE_AUDIO))
//     {
//         if (byMediaMode == MODE_AUDIO)
//         {
//             return TRUE;
//         }
//         else if (byMediaMode == MODE_BOTH)
//         {
//             byMediaMode = MODE_VIDEO;
//         }
//     }
// 
// 	//�����ѽ��õ�һ·��Ƶͨ������˫�����նˣ��л���һ·��ƵԴ�����ܾ�
// 	if( TRUE == JudgeIfSwitchFirstLChannel(tSrc, bySrcChnnl, tDstMt) )
// 	{	
// 		if( byMediaMode == MODE_VIDEO )
// 		{
// 			return TRUE;
// 		}
// 		else if ( byMediaMode == MODE_BOTH )
// 		{
// 			byMediaMode = MODE_AUDIO;
// 		}
// 	}
// 	
// 	//��Ƶ���������Լ�, �����Դ��Ŀ���Ϊmcu����Ӧ���н���
// 	if( (tSrc.GetMtId() == byDstMtId) && 
// 		(tSrc.GetType() == TYPE_MT) && 
// 		(tSrc.GetMtType() == MT_TYPE_MT) )
// 	{
// 		if( byMediaMode == MODE_AUDIO )
// 		{
// 			StopSwitchToSubMt( byDstMtId, byMediaMode, SWITCH_MODE_BROADCAST, bMsgStatus );
// 			return TRUE;
// 		}
// 		else if ( byMediaMode == MODE_BOTH )
// 		{
// 			StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgStatus );
// 			byMediaMode = MODE_VIDEO;
// 		}
// 	}
// 
// 	if( m_tConf.m_tStatus.IsMixing() ) 
// 	{
// 		if( byMediaMode == MODE_AUDIO )
// 		{
// 			return FALSE;
// 		}
// 		else if( byMediaMode == MODE_BOTH )
// 		{
// 			byMediaMode = MODE_VIDEO;
// 		}
// 	}
// 
// 	// [pengjie 2010/8/20] ����ش�ͨ����֪ͨ��ش��¼�������
// //	CRecvSpy cRecvSpy;
// //  if(!tUnlocalSrc.IsLocal() && m_cSMcuSpyMana.GetRecvSpy( tUnlocalSrc, cRecvSpy ) )
// //	{
// //		SendMMcuSpyNotify( tUnlocalSrc, MCS_MCU_STARTSWITCHMT_REQ );
// //	}
// 	// End
// 	
// 	//��Ҫ���Լ�������������ȥ, �����Դ��Ŀ���Ϊmcu����Ӧ���н���
// 	if( (tSrc.GetType() == TYPE_MT) &&
// 		(tSrc.GetMtType() == MT_TYPE_MT) && 
// 		(tSrc.GetMtId() == byDstMtId) && 
// 		(m_ptMtTable->GetManuId( byDstMtId ) == MT_MANU_KDC) )
// 	{
// 		if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
// 		{
// 			NotifyMtReceive( tSrc, tDstMt.GetMtId() );
// 		}
// 		
// 		StopSwitchToSubMt( byDstMtId, byMediaMode, SWITCH_MODE_BROADCAST, bMsgStatus );
// 
// 		// guzh [8/31/2006] �����������ն�״̬��Stop��ʱ�������
// 		// m_ptMtTable->SetMtSrc( byDstMtId, ( const TMt * )&tSrc, byMediaMode );			
//         
// 		return TRUE;
// 	}
// 
// 	//lukunpeng 2010/06/02 ������¼���mcu������û����ƵԴ����Ҫ���Լ��������������Լ�
// 	if(tSrc == tDstMt
// 		&& tDstMt.GetMtType() == MT_TYPE_SMCU
// 		&& MODE_AUDIO != byMediaMode
// 		&& tLocalVidBrdSrc.IsNull())
// 	{
// 		//lukunpeng 2010/06/17 ���ںܶ�ʱ����ڷǿƴ��նˣ�û�в𽻻������½�������
// 		//��ɴӴ˴����غ󣬷���ԭ���Ľ���û�в𣬹��ڴ˲���һ�½���
// 		StopSwitchToSubMt( byDstMtId, MODE_VIDEO, SWITCH_MODE_BROADCAST, bMsgStatus );
// 		return TRUE;
// 	}
// 
// 	//lukunpeng 2010/06/29 ������ϼ���mcu���¼��ķ����ˣ�����Ҫ���Լ�����Ƶ�ٽ�����ȥ
// 	if (tSrc == tDstMt
// 		&& tDstMt.GetMtType() == MT_TYPE_MMCU
// 		&& MODE_VIDEO != byMediaMode)
// 	{
// 		StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgStatus );
// 		return TRUE;
// 	}
// 
// 	//lukunpeng 2010/07/13 ��Ӧ�����ն����ش�Դ�������ϼ��Ĺ㲥Դ�Ǳ������նˣ���Ҫ����Ƶ��������Լ�
// 	//�����ڻ���������£������ˣ��¼������ն����ϼ���������Nģʽ������������ȫ�ַ����˵��¼��ն�
// 	TConfMcInfo *ptConfMcInfo = NULL;
// 	if (!m_tCascadeMMCU.IsNull() && MODE_VIDEO != byMediaMode)
// 	{
// 		ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
// 		
// 		if (ptConfMcInfo != NULL
// 			&& tSrc == m_tCascadeMMCU
// 			&& ptConfMcInfo->m_tSpyMt.GetMtId() == byDstMtId
// 			&& ptConfMcInfo->m_tConfAudioInfo.m_byMixerCount == 1
// 			&& ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId() == byDstMtId
// 			)
// 		{
// 			StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgStatus );
// 			return TRUE;
// 		}
// 	}
// 	
//     //for h320 mcu cascade select
//     if ( m_ptMtTable->GetMtType(byDstMtId) == MT_TYPE_SMCU && tSrc.GetMtId() == byDstMtId &&
//          !m_tLastSpyMt.IsNull() && 
//          m_tConfAllMtInfo.MtJoinedConf(m_tLastSpyMt.GetMcuId(), m_tLastSpyMt.GetMtId()) )
//     {
//         tSrc = m_tLastSpyMt;
//     }
// 
//     // zbq [08/31/2007] ������ֻ������Ƶ����Ƶ����˫�����޹�ϵ
//     /*
// 	//�Ƿ�Ҫת��H.239���ݣ�����˫�����������з��������ƣ��Ժ󷢺���ЧΪ׼
// 	TLogicalChannel tLogicalChannel;
// 	if( tSrc.GetType() == TYPE_MT && 
// 		m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) )
// 	{
// 		//g_cMpManager.StartSwitchToSubMt( tSrc, 1, tDstMt, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE );
//         g_cMpManager.StartSwitchToSubMt( tSrc, 0, tDstMt, MODE_SECVIDEO, SWITCH_MODE_BROADCAST, TRUE );
// 		//�µ�˫�����ն˼��룬����ǿ�Ʋ����ؼ�֡���Ƿ���Ҫ�ؼ�֡���ն���������
// 	}*/
// 
//     //modify bas 2
// 	//�ն��Ƿ�������
//     TMt tSwitchSrc;
//     u8  bySwitchSrcChnnl;
//     tSwitchSrc.SetNull();
// 
// 	if (!m_tConf.GetConfAttrb().IsUseAdapter() || SWITCH_MODE_SELECT == bySwitchMode)
// 	{
//         tSwitchSrc = tSrc;
//         bySwitchSrcChnnl = bySrcChnnl;
// 	}
// 	else
// 	{
//         if (MODE_AUDIO != byMediaMode && m_tConf.m_tStatus.IsBrdstVMP())
//         {
//             TSimCapSet tSimCapSet; 
//             u8 byVmpSrcChl = 0;
//             u8 byMediaType = m_tConf.GetMainVideoMediaType();
//             u16 wDstMtReqBitrate = m_ptMtTable->GetMtReqBitrate(byDstMtId);
// 
//             TCapSupport tCap;
// 			/*
//             //�ȴ���vmp˫�ٵ���ʽ�����
//             if (0 != m_tConf.GetSecBitRate() && 
//                 MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
//             {
//                 //����2·
//                 if(wDstMtReqBitrate < m_tConf.GetBitRate())
//                 {
//                     byVmpSrcChl = 1;
//                     if (wDstMtReqBitrate < m_wBasBitrate)
//                     {
//                         ChangeVmpBitRate(wDstMtReqBitrate, 2);
//                     }
//                 }
//                 //����1·
//                 else
//                 {
//                     byVmpSrcChl = 0;
//                     if (wDstMtReqBitrate < m_wVidBasBitrate)
//                     {
//                         ChangeVmpBitRate(wDstMtReqBitrate, 1);
//                     }
//                 }
// 
//                 //FIXME: δ�������VMP˫��
//             }
//             //����˫��ʽ��˫��˫��ʽ
//             else
//             {
//                 tSimCapSet = m_ptMtTable->GetDstSCS(byDstMtId);
//                 if (tSimCapSet.GetVideoMediaType() == byMediaType)
//                 {
//                     byVmpSrcChl = 0;
//                     if (wDstMtReqBitrate < m_wVidBasBitrate)
//                     {
//                         ChangeVmpBitRate(wDstMtReqBitrate, 1);
//                     }
//                 }
//                 else
//                 {
//                     byVmpSrcChl = 1;
//                     if (wDstMtReqBitrate < m_wBasBitrate)
//                     {
//                         ChangeVmpBitRate(wDstMtReqBitrate, 2);
//                     }
//                 }
//             }            
// 			bySwitchSrcChnnl = byVmpSrcChl;*/
// 			// xliang [8/4/2009] ��VMP�㲥����,�˴�����ֻ����VMP�㲥���ȥ
// 			bySwitchSrcChnnl = AdjustVmpBrdBitRate(&tDstMt);
//             tSwitchSrc = m_tVmpEqp;
//             
//         }
//         else
//         {
//             TSimCapSet tTmpSCS;
//             TSimCapSet tSrcSCS;
//             TSimCapSet tDstSCS;
// 
//             if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
//             {
// 			    if (tSrc.GetType() == TYPE_MT &&
// 					IsNeedAdapt(tSrc, m_ptMtTable->GetMt(byDstMtId), MODE_AUDIO))
// 			    {
// 					TEqp tBas;
// 					tBas.SetNull();
// 					u8 byChnId  = 0;
// 					u8 byOutNum = 0;
// 					u8 byOutIdx = 0;
// 					//[nizhijun 2010119] ��Ҫ�Ƚ�tmt�������ܱȽ�mtid�����������Լ�
// 					if (tUnlocalSrc == m_ptMtTable->GetMt(byDstMtId))
// 					{
// 						return FALSE;
// 					}
// 					if(!FindBasChn2BrdForMt(byDstMtId, MODE_AUDIO, tBas, byChnId, byOutNum, byOutIdx))
// 					{
// 						ConfLog(FALSE, "[FindBasChn2BrdForMt] FindBasChn2BrdForMt:%d with Mode:%d failed!\n", byDstMtId, MODE_AUDIO);
// 						return FALSE;
// 					}
// 					tBas.SetConfIdx(m_byConfIdx);
// 					tSwitchSrc = tBas;
// 					bySwitchSrcChnnl = byChnId * byOutNum + byOutIdx;
// 			    }
// 				else
// 				{
// 					tSwitchSrc = tSrc;
//                     bySwitchSrcChnnl = bySrcChnnl;
// 				}
// //                 if (tSrc.GetType() == TYPE_MT &&
// //                     IsMtNeedAdapt(ADAPT_TYPE_AUD, byDstMtId, &tSrc))
// //                 {
// //                     if (!m_tConf.m_tStatus.IsAudAdapting())
// //                     {
// //                         tTmpSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
// //                         tSrcSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
// //                         tTmpSCS = m_ptMtTable->GetDstSCS(byDstMtId);
// //                         tDstSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
// //                         if (!StartAdapt(ADAPT_TYPE_AUD, 0, &tDstSCS, &tSrcSCS))
// //                         {
// //                             ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartSwitchToSubMt %d failed because audio Adapter cann't available!\n", byDstMtId);
// //                             return FALSE;
// //                         }
// //                     }                    
// // 
// //                     ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "IsMtNeedAdapt(ADAPT_TYPE_AUD, %d)\n", byDstMtId);
// //                     tSwitchSrc = m_tAudBasEqp;
// //                     bySwitchSrcChnnl = m_byAudBasChnnl;
// //                 }
// //                 else
// //                 {
// //                     tSwitchSrc = tSrc;
// //                     bySwitchSrcChnnl = bySrcChnnl;
// //                 }
// 
//                 if (MODE_BOTH == byMediaMode)
//                 {
//                     ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "IsMtNeedAdapt(ADAPT_TYPE_AUD, %d, MODE_BOTH)\n", byDstMtId);
// 					//zjj20100201
//                     g_cMpManager.StartSwitchToSubMt(tSwitchSrc, bySwitchSrcChnnl, tDstMt, MODE_AUDIO, bySwitchMode,
// 										FALSE,TRUE,FALSE,wSpyPort
// 										);
//                     bSwitchedAudio = TRUE;
//                 }
//             }
// 
//             if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
//             {
// 				// [9/20/2010 liuxu] ¼����������ʱҲ��Ҫ��������ͨ��
// 				if ((tSrc.GetType() == TYPE_MT || (TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_RECORDER == tSrc.GetEqpType()))
// 					&&
// 					IsNeedAdapt(tSrc, m_ptMtTable->GetMt(byDstMtId), MODE_VIDEO))
// 				{
// 					TEqp tBas;
// 					tBas.SetNull();
// 					u8 byChnId  = 0;
// 					u8 byOutNum = 0;
// 					u8 byOutIdx = 0;
// 					//[nizhijun 2010119] ��Ҫ�Ƚ�tmt�������ܱȽ�mtid�����������Լ�
// 					if (tUnlocalSrc == m_ptMtTable->GetMt(byDstMtId))
// 					{
// 						return FALSE;
// 					}
// 					if(!FindBasChn2BrdForMt(byDstMtId, MODE_VIDEO, tBas, byChnId, byOutNum, byOutIdx))
// 					{
// 						ConfLog(FALSE, "[FindBasChn2BrdForMt] FindBasChn2BrdForMt:%d with Mode:%d failed!\n",byDstMtId, MODE_VIDEO);
// 						return FALSE;
// 					}
// 					tBas.SetConfIdx(m_byConfIdx);
// 					tSwitchSrc = tBas;
// 					bySwitchSrcChnnl = byChnId * byOutNum + byOutIdx;
// 				}
// 				else
// 				{
// 					tSwitchSrc = tSrc;
//                     bySwitchSrcChnnl = bySrcChnnl;
// 				}
// //                 // �жϸ�������, zgc, 2008-08-08
// //                 TMt tSrcTemp;
// //                 tSrcTemp.SetNull();
// //                 TSimCapSet tSrcSCSTemp;
// //                 if ( IsHDConf( m_tConf ) )
// //                 {
// //                     if (m_cMtRcvGrp.IsMtNeedAdp(byDstMtId) ||
// //                         bMMcuNeedAdp)
// //                     {
// // 						tSwitchSrc = tSrc;
// // 						bySwitchSrcChnnl = bySrcChnnl;
// //                     }
// //                     else
// //                     {
// //                         if (!tLocalVidBrdSrc.IsNull())
// //                         {
// //                             tSwitchSrc = tLocalVidBrdSrc;
// //                             bySwitchSrcChnnl = tLocalVidBrdSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
// //                         }
// //                         else
// //                         {
// //                             //m_tVidBrdSrcΪ�գ��ֲ����䣬��Ĭ��Դ�����磺�⳧�̿��Լ���
// //                             tSwitchSrc = tSrc;
// //                             bySwitchSrcChnnl = bySrcChnnl;
// //                         }
// //                     }
// //                 }
// //                 else
// //                 if (IsMtNeedAdapt(ADAPT_TYPE_VID, byDstMtId, &tSrc))
// //                 {
// //                     if (!m_tConf.m_tStatus.IsVidAdapting())
// //                     {
// //                         if (IsNeedVidAdapt(tDstSCS, tSrcSCS, wAdaptBitRate, &tSrc))
// //                         {
// //                             //��Ҫ�����Ƿ����StartAdapt()���
// //                             if (!StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSCS, &tSrcSCS))
// //                             {
// //                                 ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartSwitchToSubMt %d failed because video Adapter cann't available!\n", byDstMtId);
// //                                 return FALSE;
// //                             }
// //                         }
// //                     }
// //                     ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "IsMtNeedAdapt(ADAPT_TYPE_VID, %d)\n", byDstMtId);
// //                     tSwitchSrc = m_tVidBasEqp;
// //                     bySwitchSrcChnnl = m_byVidBasChnnl;
// //                 }
// //                 else if (IsMtNeedAdapt(ADAPT_TYPE_BR, byDstMtId, &tSrc))
// //                 {
// //                     if (!m_tConf.m_tStatus.IsBrAdapting())
// //                     {
// //                         if (IsNeedBrAdapt(tDstSCS, tSrcSCS, wAdaptBitRate, &tSrc))
// //                         {
// //                             if (!StartAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSCS, &tSrcSCS))
// //                             {
// //                                 ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartSwitchToSubMt %d failed because bitrate Adapter cann't available!\n", byDstMtId);
// //                                 return FALSE;
// //                             }
// //                         }
// //                     }
// //                     ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "IsMtNeedAdapt(ADAPT_TYPE_BR, %d)\n", byDstMtId);
// //                     tSwitchSrc = m_tBrBasEqp;
// //                     bySwitchSrcChnnl = m_byBrBasChnnl;
// //                 }
// //                 else
// //                 {
// //                     tSwitchSrc = tSrc;
// //                     bySwitchSrcChnnl = bySrcChnnl;
// //                 }
//             }
//         }        
//     }
// 
//     if (!tSwitchSrc.IsNull())
//     {
//         u8 byTmpMediaMode = byMediaMode;
//         if (MODE_BOTH == byMediaMode && bSwitchedAudio) //������Ƶ�ظ�����
//         {
//             byTmpMediaMode = MODE_VIDEO;
//         }
//         
//         //zbq [08/07/2007] BAS�����ն˵Ľ���,����ƽ������,������StartStop����.
//         BOOL32 bStopBeforeStart = TRUE;
// 		TMt tMtOrgSrc;
// 
// 		//zjl [09/11/30] ��ȡ��ǰѡ���ն���ǰ��Դ������뵱ǰԴ��ͬ���򲻲𽻻�
// 		m_ptMtTable->GetMtSrc(tDstMt.GetMtId(), &tMtOrgSrc, byTmpMediaMode);
// 		//zjl [03/11/2010] zjl �����ǰĿ���ն˵�Դ��bas����������ǰ��ԴҲ��bas, ͬʱ����ͨ��Ҳ��ͬ���򲻲𽻻�
// 		TEqp tBas;
// 		u8 byOutChnl = 0;
// 		GetMtRealSrc(tDstMt.GetMtId(), byTmpMediaMode, tBas, byOutChnl);
//         if ( (tSwitchSrc.GetType()    == TYPE_MCUPERI &&
//               tSwitchSrc.GetEqpType() == EQP_TYPE_BAS  &&
// 			  tBas == tSwitchSrc && byOutChnl == bySwitchSrcChnnl) ||
//              (tSwitchSrc.GetType()    == TYPE_MT &&
//               tSwitchSrc.GetEqpType() == MT_TYPE_SMCU && 
// 			  tSwitchSrc == tMtOrgSrc))
//         {
//             bStopBeforeStart = FALSE;
// 			ConfLog( FALSE, "[StartSwitchToSubMt] DstMt<McuId:%d, MtId:%d>'s OrgSrc<McuId:%d,MtId:%d, BasId:%d, OutChn:%d> and Current Src<McuId:%d, MtId:%d>!\n",
// 						tDstMt.GetMcuId(), tDstMt.GetMtId(), tMtOrgSrc.GetMcuId(),
// 						tMtOrgSrc.GetMtId(), tBas.GetEqpId(), byOutChnl, 
// 						tSwitchSrc.GetMcuId(), tSwitchSrc.GetMtId()
// 						);
//         }
//         /*if ( (tSwitchSrc.GetType()    == TYPE_MCUPERI &&
//               tSwitchSrc.GetEqpType() == EQP_TYPE_BAS ) ||
//              (tSwitchSrc.GetType()    == TYPE_MT &&
//               tSwitchSrc.GetEqpType() == MT_TYPE_SMCU && 
// 			  tSwitchSrc == tMtOrgSrc))
//         {
//             bStopBeforeStart = FALSE;
//         }*/
// 
// 		if (!g_cMpManager.StartSwitchToSubMt(tSwitchSrc, bySwitchSrcChnnl, tDstMt, byTmpMediaMode, bySwitchMode,
// 					FALSE, bStopBeforeStart, FALSE,wSpyPort)
// 					)
// 		{
// 			ConfPrint(LOG_LVL_ERROR, MID_MCU_MPMGR,  "StartSwitchToSubMt() failed! Cannot switch to specified sub mt!\n" );
// 			return FALSE;
// 		}
// 
//         if (m_tConf.GetConfAttrb().IsResendLosePack())
//         {  		
// 			if( (MODE_VIDEO == byMode || MODE_BOTH == byMode) && tLocalVidBrdSrc == m_tVmpEqp )
// 			{
// 				TPeriEqpStatus tPeriEqpStatus; 
// 				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
// 				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
// 				if( byVmpSubType != VMP )
// 				{				
// 					u8 byPrsId = 0;
// 					u8 byPrsChnId = 0;
// 					if (FindPrsChnForSrc( m_tVmpEqp, bySwitchSrcChnnl, byPrsId, byPrsChnId))
// 					{
// 						AddRemovePrsMember(tDstMt.GetMtId(), byPrsId, byPrsChnId, MODE_VIDEO, TRUE);
// 					}	
// 				}				
// 			}
//         }        
//     }
// 
// 	if( !tUnlocalSrc.IsLocal() && m_cSMcuSpyMana.IsMtInSpyMember( tUnlocalSrc, MODE_VIDEO ) )
// 	{
// 		if ( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode )
//         {
//     		NotifyFastUpdate(tUnlocalSrc, MODE_VIDEO, TRUE);
// 		}
// 	}
// 	//֪ͨ�ն˿�ʼ���� 
// 	else if( tSrc.GetType() == TYPE_MT )
// 	{
//         if (bSndFlowCtrl)
//         {
//             NotifyMtSend( tSrc.GetMtId(), byMediaMode, TRUE );
//         }		
// 
//         // zbq [06/25/2007] ����Ƶ����������ؼ�֡
//         if ( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode )
//         {
// 			NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
//         }
// 	}
//     else
//     {
//         //zbq[05/07/2009] �����ش� �������� �������ؼ�֡
// //         if ((MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode) &&
// //             tSrc.GetEqpId() == m_tCasdVidBasEqp.GetEqpId() &&
// //             byDstMtId == m_tCascadeMMCU.GetMtId())
// //         {
// //             NotifyFastUpdate(tSrc, 0);
// //             NotifyFastUpdate(tSrc, 1);
// //         }
//     }
// 	
// 	// xliang [6/16/2009] FIXME: ��MPU����ؼ�֡��
// 	
// 	//�����ն˵�������ʾ��ʶ
// 	TPeriEqpStatus tEqpStatus;
// 	if( tSrc == m_tPlayEqp )
// 	{
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPlayEqp.GetEqpId(), &tEqpStatus );
// 		cServMsg.SetMsgBody( (u8*)tEqpStatus.GetAlias(), MAXLEN_EQP_ALIAS );
// 		SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
// 	}
// 	//�����VMP,�����������ʾ��ʶ
// 	if( tSrc == m_tVmpEqp )
// 	{
// 		u8 abyVmpAlias[8];
// 		abyVmpAlias[0] = ' ';
// 		abyVmpAlias[1] = '\0';
// 		cServMsg.SetMsgBody( abyVmpAlias, sizeof(abyVmpAlias) );
// 		SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
// 	}
// 
// 	//֪ͨ�ն˿�ʼ����
// 	if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
// 	{
// 		NotifyMtReceive( tSrc, tDstMt.GetMtId() );
// 	}
// 
//     if (tSrc.GetType() == TYPE_MCUPERI && 
//         tSrc.GetEqpType() == EQP_TYPE_BAS)
// 	{	
// #ifndef _8KE_
// 		if (g_cMcuAgent.IsEqpBasHD(tSrc.GetEqpId()))
// #endif
// 		{		
// 			//zjj20100422��¼��ʵ��ƵԴ,���������Ҫ��¼��ʵ������Ϊ��ƵԴ
// 			if( tLocalVidBrdSrc.GetType() != TYPE_MCUPERI )
// 			{
// 				tSrc = m_tConf.GetSpeaker();
// 				m_ptMtTable->SetMtSrc(byDstMtId, &tSrc, byMediaMode);
// 			}
// 			else
// 			{
// 				m_ptMtTable->SetMtSrc(byDstMtId, &tLocalVidBrdSrc, byMediaMode);
// 			}
// 			MtStatusChange(&tDstMt, TRUE);
// 			return TRUE;	
// 		}
//     }
// 	
// 		
// 	// xsl [5/22/2006]�����ն�״̬, ���ﱣ��㲥Դ��Ϣ��������bas��Ϣ����Ϊ�����ط������Դ��Ϣ����Ӧ����
// 	BOOL32 bCanSetMtSrc = TRUE;
// 
// 	//zjj20100329 �ϼ�mcu��Դֻ���ǻش�ͨ���е��ն�
// 	if( !m_tCascadeMMCU.IsNull() && byDstMtId == m_tCascadeMMCU.GetMtId() )
// 	{
// 		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tDstMt.GetMtId()));
// 		if( NULL != ptConfMcInfo &&
// 			tSrc.GetMtId() != ptConfMcInfo->m_tSpyMt.GetMtId() &&
// 			tSrc.GetMcuId() != ptConfMcInfo->m_tSpyMt.GetMcuId()
// 			)
// 		{
// 			bCanSetMtSrc = FALSE;
// 		}
// 	}
// 	
// 	if( bCanSetMtSrc )
// 	{
// 		TMt tRealMt;
// 		tRealMt.SetNull();
// 		//lukunpeng 2010/07/01 ��ش�Ҫ����������Դ
// 		m_ptMtTable->SetMtSrc( byDstMtId, &tUnlocalSrc, byMediaMode );	
// 	}
// 
// 	//����Ŀ���ն�Ϊ�ϼ�mcu�����
// 	m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );
// 	if( tSrc.GetType() == TYPE_MT && tSrc.GetMtId() != byDstMtId && 
// 		(m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId()) )
// 	{
// 		TMtStatus tSrcMtStatus;
// 		u8 byAddSelByMcsMode = MODE_NONE;
// 		m_ptMtTable->GetMtStatus( tSrc.GetMtId(), &tSrcMtStatus );
// 
// 		//lukunpeng 2010/07/01 ���ڵ��ش�������ƥ��local���նˣ����ڶ�ش�������ƥ����ʵԴ
// 		TMt tVidBrdSrc = tLocalVidBrdSrc;
// 		TMt tAudBrdSrc = GetLocalAudBrdSrc();
// 
// 		// [pengjie 2010/7/29] ����Դ���ܱ�����tSrc = tUnlocalSrc
// 		TMt tMtSrc = tSrc;
// 		if( wSpyPort != SPY_CHANNL_NULL)
// 		{
// 			//tSrc = tUnlocalSrc;
// 			tMtSrc = tUnlocalSrc;
// 
// 			tVidBrdSrc = GetVidBrdSrc();
// 			tAudBrdSrc = GetAudBrdSrc();
// 		}
// 
// 	
// 		//[nizhijun 2010/11/30]IsLocalAndSMcuSupMultSpy(tUnlocalSrc.GetMcuId()),��ش���֧�ֵ��ش���֧��
// 		//[nizhijun 2010/11/17] ����ѡ��Դ���¼�MCU�����
//         //zbq[12/06/2007] VMP����ʱ������
//         if( (  (!(tMtSrc == tVidBrdSrc)) || (tMtSrc == tVidBrdSrc && 
//                                            ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() &&
//                                            m_tRollCaller == tVidBrdSrc)
// 										   || (tMtSrc == tVidBrdSrc &&                                            
// 											!tMtStatus.GetSelectMt(byMediaMode).IsNull()
// 										   )  ) &&
//                                            tSrcMtStatus.IsSendVideo() && 
//             ( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode ) )
// 		{
// 
// 			byAddSelByMcsMode = MODE_VIDEO;
// 		}
// 		
// 		//[nizhijun 2010/11/30]IsLocalAndSMcuSupMultSpy(tUnlocalSrc.GetMcuId()),��ش���֧��,���ش���֧��
// 		if( ( !(tMtSrc == tAudBrdSrc) 
// 			  || ( tMtSrc == tVidBrdSrc &&
// 				   !tMtStatus.GetSelectMt(byMediaMode).IsNull()
// 				  )  )	
// 			&& tSrcMtStatus.IsSendAudio() && 
// 			( MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode ) )
// 		{
// 			if( MODE_VIDEO == byAddSelByMcsMode )
// 			{
// 				byAddSelByMcsMode = MODE_BOTH;
// 			}
// 			else
// 			{
// 				byAddSelByMcsMode = MODE_AUDIO;
// 			}
// 		}
// 
// 		if( MODE_NONE != byAddSelByMcsMode )
// 		{
// 			tMtStatus.AddSelByMcsMode( byMediaMode );
// 		}
// 		else
// 		{
// 			tMtStatus.RemoveSelByMcsMode( byMediaMode );
// 			// ����ϴ�ѡ���ն�	
// 			TMt tNullMt;
// 			tNullMt.SetNull();
// 			tMtStatus.SetSelectMt(tNullMt, byMediaMode);
// 		}
// 	}
// 	else
// 	{
// 		tMtStatus.RemoveSelByMcsMode( byMediaMode );
// 		// ����ϴ�ѡ���ն�	
// 		TMt tNullMt;
// 		tNullMt.SetNull();
// 		tMtStatus.SetSelectMt(tNullMt, byMediaMode);
// 	}
// 	m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );
// 	
// 	if( bMsgStatus )
// 	{
//         MtStatusChange( &tDstMt, TRUE );
// 	}
// 
// 	//����ѡ������Ŀ���ն˵���Ҫ���ڷ����ն˵�����
// 	if( tSrc.GetType() == TYPE_MT && bySwitchMode == SWITCH_MODE_SELECT && 
// 		( byMediaMode == MODE_VIDEO || byMediaMode == MODE_BOTH ) && 
// 	    tSrc.GetMtId() != tLocalVidBrdSrc.GetMtId() )
// 	{
// 		u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( byDstMtId );
// 		AdjustMtVideoSrcBR( byDstMtId, wMtBitRate );
// 	}
// 
// 	return TRUE;
// }

/*====================================================================
    ������      StartSwitchToAll
    ����        ����ָ���ն����ݽ������¼�ֱ���Ǵμ��նˣ�����Ƿ�ֱ���ն���
				  �����н���ֱ�ӷ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt &tSrc Դ�ն�/����
	              const u8 bySwitchGrpNum ��Դ�м��������飨bas�����ж����
				  TSwitchGrp *pSwitchGrp ����������
				  u8    byMode // ��/��Ƶģʽ
				  u8 bySwitchMode // ����ģʽ��ѡ��/�㲥��
				  BOOL32 bMsgStatus // �Ƿ����ն�״̬�ı���Ϣ
				  BOOL32 bSndFlowCtrl �Ƿ�֪ͨԴ��ʼ������
				  u16 wSpyStartPort // ��ش��˿ڣ��ش�Դ
				  BOOL32 bInnerSelect �Ƿ�Ϊ�ڲ�ѡ����true��������mcsdragmode
    ����ֵ˵��  ��TRUE�����������ֱ�ӷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/12    1.0         LI Yi         ����
	03/07/23    2.0         ������        �޸�
	04/03/06    3.0         ������        �޸�
    07/02/13    4.0         ����        �޸�
	20110505    4.6         pengjie       ����������֧��
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToAll( TMt tSrc, 
										   const u8 bySwitchGrpNum,
										   TSwitchGrp *pSwitchGrp,
										   u8 byMode,
										   u8 bySwitchMode,
										   BOOL32 bMsgStatus,
										   BOOL32 bSndFlowCtrl,
										   u16 wSpyStartPort,
										   BOOL32 bInnerSelect)
{
	
	if( bySwitchGrpNum == 0 || pSwitchGrp == NULL)
	{
		return FALSE;
	}

	// [8/5/2011 liuxu] ��CSwitchGrpAdpt��һ��ת��, �滻pSwitchGrp��mtlist, ��ֹ���²�������mtlistֵ,
	// �Ӷ�Ӱ�����StartSwitchToAll�Ľӿ�	
	CSwitchGrpAdpt cSwitchGrpAdpt;
	cSwitchGrpAdpt.Convert(bySwitchGrpNum, pSwitchGrp);

    TMtStatus tMtStatus;
    CServMsg  cServMsg;
    u8 byMediaMode = byMode;
    BOOL32 bSwitchedAudio = FALSE;
	
	//zjj20100201
	//  [11/26/2009 pengjie] Modify ������ش�֧��
	TMt tUnlocalSrc = tSrc;
	// End
	
    tSrc = GetLocalMtFromOtherMcuMt(tSrc);
	u8 byLoop = 0;
	u8 byMtNum = 0;
	//����KDC�ն��Ƿ�������״̬�����򲻽�����Ƶ����	
	if( TYPE_MT == tSrc.GetType() && 
		MT_MANU_KDC != m_ptMtTable->GetManuId( tSrc.GetMtId() ) && 
		m_ptMtTable->IsMtAudioDumb( tSrc.GetMtId() ) )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			//zhouyiliang 20121116 ��kdc�ն˴���������������Ƶ����������mcsdragmodҪˢ��ȥ
			if(bySwitchMode == SWITCH_MODE_SELECT)
			{
				for( byLoop = 0; byLoop < bySwitchGrpNum; byLoop++ )
				{
					TMt *pDstMt = pSwitchGrp[byLoop].GetDstMt();
					for( byMtNum = 0; byMtNum < pSwitchGrp[byLoop].GetDstMtNum(); byMtNum++ )
					{
						u8 byDstMtId = pDstMt[byMtNum].GetMtId();
						AddRemoveMcsDragMod(tUnlocalSrc,byDstMtId,byMediaMode,bySwitchMode,wSpyStartPort,bMsgStatus,bInnerSelect);
					
						
					}
				}
			}
		
		
			return TRUE;
		}
		else if ( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_VIDEO;
		}
	}
	
	
	u32 dwTimeIntervalTicks = 3*OspClkRateGet();
	
	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();
	

	for( byLoop = 0; byLoop < bySwitchGrpNum; byLoop++ )
	{
		for( byMtNum = 0; byMtNum < pSwitchGrp[byLoop].GetDstMtNum(); byMtNum++ )
		{
			TMt *pDstMt = pSwitchGrp[byLoop].GetDstMt();
			u8 byDstMtId = pDstMt[byMtNum].GetMtId();
			
			//ֻ���ϼ�Mcu��spyMt
			TMt tDstMt = m_ptMtTable->GetMt( byDstMtId );

			if(m_tCascadeMMCU.GetMtId() != 0 && byDstMtId == m_tCascadeMMCU.GetMtId())
			{
				// ָ������������ն�Ϊ�����˻������ѯ�������ն�ʱ���Խ����ϼ�MCU�Ľ���
				if (SWITCH_MODE_BROADCAST == bySwitchMode && byMode != MODE_SECVIDEO)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[StartSwitchToAll] ignore MMcu's %d brd switch!\n", byMode);
					pDstMt[byMtNum].SetNull();
					continue;
				}

				if(TYPE_MT == tSrc.GetType() /*&& !m_cLocalSpyMana.GetSpyChannlInfo(tSrc, cSendSpy)*/)
				{
					TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tDstMt.GetMtId()));
					if ( NULL == ptConfMcInfo ) 
					{
						pDstMt[byMtNum].SetNull();
						continue;
					} 
					if( !(ptConfMcInfo->m_tSpyMt == tSrc) )
					{
						pDstMt[byMtNum].SetNull();
						continue;
					}             
				}
                //songkun,20110630,����ʱ���¼�������ϳɲ��ٽ������ϼ�MCU�Ľ���
                if(TYPE_MCUPERI == tSrc.GetType())
                {
                    if(tSrc.GetEqpType() == EQP_TYPE_VMP)
                    {
                        pDstMt[byMtNum].SetNull();
                        continue;
                    }
                }
			}

			//˫ѡ�����������¼�
			if (MODE_VIDEO2SECOND == byMode && tDstMt.GetMtType() == MT_TYPE_SMCU)
			{
				pDstMt[byMtNum].SetNull();
				continue;
			}
			
			//����KDC�ն��Ƿ��ھ���״̬����������������Ƶ����	
			if( MT_MANU_KDC != m_ptMtTable->GetManuId( byDstMtId ) && 
				m_ptMtTable->IsMtAudioMute( byDstMtId ) )
			{
				if( byMediaMode == MODE_AUDIO )
				{
					//zhouyiliang 20121116 ��KDC�ն��Ƿ��ھ���״̬��������������Ƶ����������mcsdragmodҪˢ��ȥ
					if(bySwitchMode == SWITCH_MODE_SELECT)
					{
						AddRemoveMcsDragMod(tUnlocalSrc,byDstMtId,byMediaMode,bySwitchMode,wSpyStartPort,bMsgStatus,bInnerSelect);
					
					}
					pDstMt[byMtNum].SetNull();
					continue;
				}
			}
			
			//ֻ�����ն�, ���ý��н���
			m_ptMtTable->GetMtStatus(byDstMtId, &tMtStatus);
			if( ( (!tMtStatus.IsReceiveVideo()) && byMediaMode == MODE_VIDEO ) ||
				( (!tMtStatus.IsReceiveAudio()) && byMediaMode == MODE_AUDIO ) )
			{
				pDstMt[byMtNum].SetNull();
				continue;
			}
			
			//Ŀ���ն��Ƿ��ڻ���
			if (tMtStatus.IsMediaLoop(MODE_VIDEO))
			{
				if (byMediaMode == MODE_VIDEO)
				{
					pDstMt[byMtNum].SetNull();
					continue;
				}
			}
			
			if (tMtStatus.IsMediaLoop(MODE_AUDIO))
			{
				if (byMediaMode == MODE_AUDIO)
				{
					pDstMt[byMtNum].SetNull();
					continue;
				}
			}
			
			//�����ѽ��õ�һ·��Ƶͨ������˫�����նˣ��л���һ·��ƵԴ�����ܾ�
			if(MODE_VIDEO2SECOND != byMode && JudgeIfSwitchFirstLChannel(tSrc, pSwitchGrp[byLoop].GetSrcChnl(), tDstMt) )
			{	
				if( byMediaMode == MODE_VIDEO )
				{
					pDstMt[byMtNum].SetNull();
					continue;
				}
			}
			
			//��Ƶ���������Լ�, �����Դ��Ŀ���Ϊmcu����Ӧ���н���
			if( (tSrc.GetMtId() == byDstMtId) && 
				(tSrc.GetType() == TYPE_MT) && 
				(tSrc.GetMtType() == MT_TYPE_MT) )
			{
				if( byMediaMode == MODE_AUDIO )
				{
					//zjl 20110510 StopSwitchToSubMt �ӿ������滻
					//StopSwitchToSubMt( byDstMtId, byMediaMode, SWITCH_MODE_BROADCAST, bMsgStatus );
					StopSwitchToSubMt(1, &pDstMt[byMtNum], byMediaMode, bMsgStatus);

					pDstMt[byMtNum].SetNull();
					continue;
				}
			}
			
			if( m_tConf.m_tStatus.IsMixing() && 
				!(tDstMt == m_tConf.GetChairman())
				&& !(tSrc  == m_tMixEqp )
               )
			{
				if( byMediaMode == MODE_AUDIO )
				{
					pDstMt[byMtNum].SetNull();
					continue;
				}
			}
			
			//��Ҫ���Լ�������������ȥ, �����Դ��Ŀ���Ϊmcu����Ӧ���н���
			if( (tSrc.GetType() == TYPE_MT) &&
				(tSrc.GetMtType() == MT_TYPE_MT) && 
				(tSrc.GetMtId() == byDstMtId) && 
				( m_ptMtTable->GetManuId( byDstMtId ) == MT_MANU_KDC || 
					m_ptMtTable->GetManuId( byDstMtId ) == MT_MANU_KDCMCU ) 
				)
			{
				//zjj20110330 ��StopSwitchToSubMt���������У�byselselfĬ��Ϊtrue�����Ըú��������ն˿��Լ���������һϵ�б�������������עȥ
				/*if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
				{
				NotifyMtReceive( tSrc, tDstMt.GetMtId() );
			}*/
				//zjl 20110510 StopSwitchToSubMt �ӿ������滻
				//StopSwitchToSubMt( byDstMtId, byMediaMode, SWITCH_MODE_BROADCAST, bMsgStatus );
				StopSwitchToSubMt(1, &pDstMt[byMtNum], byMediaMode, bMsgStatus);
				
				// guzh [8/31/2006] �����������ն�״̬��Stop��ʱ�������
				// m_ptMtTable->SetMtSrc( byDstMtId, ( const TMt * )&tSrc, byMediaMode );			
				
				pDstMt[byMtNum].SetNull();
				continue;
			}
			
			//lukunpeng 2010/06/02 ������¼���mcu������û����ƵԴ����Ҫ���Լ��������������Լ�
			if(tSrc == tDstMt
				&& (tDstMt.GetMtType() == MT_TYPE_SMCU || tDstMt.GetMtType() == MT_TYPE_MMCU)
				&& MODE_AUDIO != byMediaMode
				&& tLocalVidBrdSrc.IsNull())
			{
				//zjl 20110510 StopSwitchToSubMt �ӿ������滻
				//lukunpeng 2010/06/17 ���ںܶ�ʱ����ڷǿƴ��նˣ�û�в𽻻������½�������
				//��ɴӴ˴����غ󣬷���ԭ���Ľ���û�в𣬹��ڴ˲���һ�½���
				//StopSwitchToSubMt( byDstMtId, MODE_VIDEO, SWITCH_MODE_BROADCAST, bMsgStatus );
				StopSwitchToSubMt(1, &pDstMt[byMtNum], MODE_VIDEO, bMsgStatus);
				pDstMt[byMtNum].SetNull();
				continue;
			}
			
			//lukunpeng 2010/06/29 ������ϼ���mcu���¼��ķ����ˣ�����Ҫ���Լ�����Ƶ�ٽ�����ȥ
			if (tSrc == tDstMt
				&& tDstMt.GetMtType() == MT_TYPE_MMCU
				&& MODE_VIDEO != byMediaMode)
			{
				//zjl 20110510 StopSwitchToSubMt �ӿ������滻
				//StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgStatus );
				StopSwitchToSubMt(1,  &pDstMt[byMtNum], MODE_AUDIO, bMsgStatus);

				pDstMt[byMtNum].SetNull();
				continue;
			}
			
			//lukunpeng 2010/07/13 ��Ӧ�����ն����ش�Դ�������ϼ��Ĺ㲥Դ�Ǳ������նˣ���Ҫ����Ƶ��������Լ�
			//�����ڻ���������£������ˣ��¼������ն����ϼ���������Nģʽ������������ȫ�ַ����˵��¼��ն�
			TConfMcInfo *ptConfMcInfo = NULL;
			if (!m_tCascadeMMCU.IsNull() && MODE_VIDEO != byMediaMode)
			{
				ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
				
				if (ptConfMcInfo != NULL
					&& tSrc == m_tCascadeMMCU
					&& ptConfMcInfo->m_tSpyMt.GetMtId() == byDstMtId
					&& ptConfMcInfo->m_tConfAudioInfo.m_byMixerCount == 1
					&& ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.IsMcuIdLocal()
					&& ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId() == byDstMtId
					)
				{
					//zjl 20110510 StopSwitchToSubMt �ӿ������滻
					//StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgStatus );
					StopSwitchToSubMt(1, &pDstMt[byMtNum], MODE_AUDIO, bMsgStatus);
					pDstMt[byMtNum].SetNull();
					continue;
				}
			}
			
			//for h320 mcu cascade select
			if ( m_ptMtTable->GetMtType(byDstMtId) == MT_TYPE_SMCU && tSrc.GetMtId() == byDstMtId &&
				!m_tLastSpyMt.IsNull() && 
				m_tConfAllMtInfo.MtJoinedConf(m_tLastSpyMt.GetMcuId(), m_tLastSpyMt.GetMtId()) )
			{
				tSrc = m_tLastSpyMt;
			}
			
		}
	}
	
	if( !g_cMpManager.StartSwitchToAll( tSrc, bySwitchGrpNum, pSwitchGrp, byMode, bySwitchMode, TRUE, wSpyStartPort ) )
	{
		ConfPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitch failed! Cannot switch to specified sub mt!\n" );
		return FALSE;
	}
	
	//////////////////////////////
	if( !tUnlocalSrc.IsLocal() && m_cSMcuSpyMana.IsMtInSpyMember( tUnlocalSrc, MODE_VIDEO ) )
	{
		if ( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode || MODE_VIDEO2SECOND == byMediaMode)
        {
			NotifyFastUpdate(tUnlocalSrc, MODE_VIDEO);
		}
	}
	//֪ͨ�ն˿�ʼ���� 
	else if( tSrc.GetType() == TYPE_MT )
	{
        if (bSndFlowCtrl)
        {
            NotifyMtSend( tSrc.GetMtId(), byMediaMode, TRUE );
        }		
		
        // zbq [06/25/2007] ����Ƶ����������ؼ�֡
        if ( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode || MODE_VIDEO2SECOND == byMediaMode)
        {
			// 2011-8-5 add by pgf : ȡ�������˺�̩���ն˵���
			if ( MT_MANU_TAIDE == m_ptMtTable->GetManuId(tSrc.GetMtId()))
			{
				// ������ؼ�֡
			}
			else
			// 2011-8-5 add end
			{

				NotifyFastUpdate(tSrc, MODE_VIDEO);
			}			
        }
	}
    else
    {
		//
    }
	
	for( byLoop = 0; byLoop < bySwitchGrpNum; byLoop++ )
	{
		for( byMtNum = 0; byMtNum < pSwitchGrp[byLoop].GetDstMtNum(); byMtNum++ )
		{
			TMt *pDstMtId = pSwitchGrp[byLoop].GetDstMt();
			u8 byDstMtId = pDstMtId[byMtNum].GetMtId();
			TMt tDstMt = m_ptMtTable->GetMt( byDstMtId );
			////////////////////////
			
			//�����ն˵�������ʾ��ʶ
			TPeriEqpStatus tEqpStatus;
			if( tSrc == m_tPlayEqp )
			{
				g_cMcuVcApp.GetPeriEqpStatus( m_tPlayEqp.GetEqpId(), &tEqpStatus );
				cServMsg.SetMsgBody( (u8*)tEqpStatus.GetAlias(), MAXLEN_EQP_ALIAS );
				SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
			}
			
			//�����VMP,�����������ʾ��ʶ
			if( tSrc.GetEqpType() == EQP_TYPE_VMP )
			{
				u8 abyVmpAlias[8];
				abyVmpAlias[0] = ' ';
				abyVmpAlias[1] = '\0';
				cServMsg.SetMsgBody( abyVmpAlias, sizeof(abyVmpAlias) );
				SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
			}   
			
			//֪ͨ�ն˿�ʼ����
			if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
			{
				NotifyMtReceive( tSrc, tDstMt.GetMtId() );
			}
			
			//�������������ѯ�������նˣ���ʱ���ܽ�������������ѯ�ն�(�ڶ������鷢����)��ָ��Ϊ�ϼ�MCU����/��ƵԴ
			if ( SWITCH_MODE_BROADCAST == bySwitchMode &&
				 !m_tCascadeMMCU.IsNull() &&
				 m_tCascadeMMCU == tDstMt)
			{
				continue;
			}

			if (tSrc.GetType() == TYPE_MCUPERI && 
				tSrc.GetEqpType() == EQP_TYPE_BAS)
			{	
				
				TBasOutInfo tTempOutInfo;
				BOOL32  bFindBasChn = FALSE;
				if (  SWITCH_MODE_BROADCAST == bySwitchMode )
				{
					 bFindBasChn = FindBasChn2BrdForMt(byDstMtId,byMediaMode,tTempOutInfo);
					 if ( bFindBasChn )
					 {
						 TMt tTempRealSrc = GetBasChnSrc(tTempOutInfo.m_tBasEqp,tTempOutInfo.m_byChnId);
						 m_ptMtTable->SetMtSrc(byDstMtId, &tTempRealSrc, byMediaMode);	
						 ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_MPMGR,"[StartSwitchToAll]set mt:%d src:%d-%d\n",
									byDstMtId,tTempRealSrc.GetMcuId(), tTempRealSrc.GetMtId()
								  );
					 }
					 else
					 {
						 ConfPrint(LOG_LVL_ERROR,MID_MCU_MPMGR,"[StartSwitchToAll]FindBasChn2BrdForMt for mt:%d failed!\n",byDstMtId);
					 }			 
				}
				else
				{
					ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_MPMGR,"[StartSwitchToAll]bySwitchMode:%d,if it's sel-status,no need to SetMtSrc\n",bySwitchMode);
				}
// 				//zjj20100422��¼��ʵ��ƵԴ,���������Ҫ��¼��ʵ������Ϊ��ƵԴ
// 				if( tLocalVidBrdSrc.GetType() != TYPE_MCUPERI )
// 				{
// 					TMt tTempSrc = m_tConf.GetSpeaker();
// 					
// 					if ( tTempSrc.IsNull() )
// 					{
// 						tTempSrc = tLocalVidBrdSrc;
// 					}
// 					m_ptMtTable->SetMtSrc(byDstMtId, &tTempSrc, byMediaMode);
// 				}
// 				else
// 				{
// 					m_ptMtTable->SetMtSrc(byDstMtId, &tLocalVidBrdSrc, byMediaMode);
// 				}
				MtStatusChange(&pDstMtId[byMtNum], TRUE);
				continue;	
			}
			
			// xsl [5/22/2006]�����ն�״̬, ���ﱣ��㲥Դ��Ϣ��������bas��Ϣ����Ϊ�����ط������Դ��Ϣ����Ӧ����
			BOOL32 bCanSetMtSrc = TRUE;
			
			//zjj20100329 �ϼ�mcu��Դֻ���ǻش�ͨ���е��ն�
			if( !m_tCascadeMMCU.IsNull() && byDstMtId == m_tCascadeMMCU.GetMtId() )
			{
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tDstMt.GetMtId()));
				if( NULL != ptConfMcInfo &&
					tSrc.GetMtId() != ptConfMcInfo->m_tSpyMt.GetMtId() &&
					tSrc.GetMcuId() != ptConfMcInfo->m_tSpyMt.GetMcuId()
					)
				{
					bCanSetMtSrc = FALSE;
				}
			}

			if (MODE_VIDEO2SECOND == byMediaMode)
			{
				bCanSetMtSrc = FALSE;
			}
			
			if( bCanSetMtSrc )
			{
				TMt tRealMt;
				tRealMt.SetNull();
				//lukunpeng 2010/07/01 ��ش�Ҫ����������Դ
				m_ptMtTable->SetMtSrc( byDstMtId, &tUnlocalSrc, byMediaMode );	
			}
			
			AddRemoveMcsDragMod(tUnlocalSrc,byDstMtId,byMediaMode,bySwitchMode,wSpyStartPort,bMsgStatus,bInnerSelect);
	
			
			//����ѡ������Ŀ���ն˵���Ҫ���ڷ����ն˵�����
			if( tSrc.GetType() == TYPE_MT && bySwitchMode == SWITCH_MODE_SELECT && 
				( byMediaMode == MODE_VIDEO || byMediaMode == MODE_BOTH ) && 
				tSrc.GetMtId() != tLocalVidBrdSrc.GetMtId() )
			{
				u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( byDstMtId );
				AdjustMtVideoSrcBR( byDstMtId, wMtBitRate );
			}
			
		}
	}
	
	return TRUE;
}


/*====================================================================
    ������      AddRemoveMcsDragMod
    ����        ��ֹͣ�����ݽ�����ֱ���¼��Ǵμ��ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tUnlocalSrc:��local����srcmt
				  u8 byDstMtId, Ŀ���ն˺�
				  u8 byMediaMode��ѡ����ģʽ
				  u8 bySwitchMode����ѡ�����ǹ㲥ģʽ
				  u16 wSpyStartPort����ش��˿�
				  BOOL32 bMsgStatus,�Ƿ�״̬֪ͨmcs
				  BOOL32 bInnerSelect, �Ƿ����ڲ�ѡ��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    16/11/2012    4.7       zhouyiliang         ����
====================================================================*/
void CMcuVcInst::AddRemoveMcsDragMod(const TMt& tUnlocalSrc, const u8 &byDstMtId, const u8& byMediaMode, u8 bySwitchMode /* = SWITCH_MODE_BROADCAST */, u16 wSpyStartPort /* = SPY_CHANNL_NULL */, BOOL32 bMsgStatus,BOOL32 bInnerSelect /* = TRUE */)
{
	TMtStatus tMtStatus;
	m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );
	TMt tSrc = GetLocalMtFromOtherMcuMt(tUnlocalSrc);
	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();
	if (bInnerSelect)
	{
		// ��ϯ��ѯ�ڲ�ѡ������dragmode,��Ҳ����else��ѡ��Դ
	}
	else if( /*!bInnerSelect &&*/ tSrc.GetType() == TYPE_MT && tSrc.GetMtId() != byDstMtId && 
		(m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId()) )
	{
		TMtStatus tSrcMtStatus;
		u8 byAddSelByMcsMode = MODE_NONE;
		m_ptMtTable->GetMtStatus( tSrc.GetMtId(), &tSrcMtStatus );
		
		//lukunpeng 2010/07/01 ���ڵ��ش�������ƥ��local���նˣ����ڶ�ش�������ƥ����ʵԴ
		TMt tVidBrdSrc = tLocalVidBrdSrc;
		TMt tAudBrdSrc = GetLocalAudBrdSrc();
		
		// [pengjie 2010/7/29] ����Դ���ܱ�����tSrc = tUnlocalSrc
		TMt tMtSrc = tSrc;
		if( wSpyStartPort != SPY_CHANNL_NULL)
		{
			tMtSrc = tUnlocalSrc;		
			tVidBrdSrc = GetVidBrdSrc();
			tAudBrdSrc = GetAudBrdSrc();
		}
		
		//[nizhijun 2010/11/30]IsLocalAndSMcuSupMultSpy(tUnlocalSrc.GetMcuId()),��ش���֧�ֵ��ش���֧��
		//[nizhijun 2010/11/17] ����ѡ��Դ���¼�MCU�����
		//zbq[12/06/2007] VMP����ʱ������
		if( (  (!(tMtSrc == tVidBrdSrc)) || (tMtSrc == tVidBrdSrc && 
			ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() &&
			m_tRollCaller == tVidBrdSrc)
			|| (tMtSrc == tVidBrdSrc &&                                            
			!tMtStatus.GetSelectMt(byMediaMode).IsNull()
			)  ) &&
			tSrcMtStatus.IsSendVideo() && 
			( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode ))
		{
			
			byAddSelByMcsMode = MODE_VIDEO;
		}
		
		//[nizhijun 2010/11/30]IsLocalAndSMcuSupMultSpy(tUnlocalSrc.GetMcuId()),��ش���֧��,���ش���֧��
		if( ( !(tMtSrc == tAudBrdSrc)
			|| ( tMtSrc == tVidBrdSrc &&
			!tMtStatus.GetSelectMt(byMediaMode).IsNull()
			)  )	
			&& tSrcMtStatus.IsSendAudio() && 
			( MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode ) &&
			bySwitchMode == SWITCH_MODE_SELECT)
		{
			if( MODE_VIDEO == byAddSelByMcsMode )
			{
				byAddSelByMcsMode = MODE_BOTH;
			}
			else
			{
				byAddSelByMcsMode = MODE_AUDIO;
			}
		}
		
		if (MODE_VIDEO2SECOND == byMediaMode)
		{
			if (!(tMtSrc == GetSecVidBrdSrc()) || !(m_ptMtTable->GetMtSelMtByMode(byDstMtId, MODE_VIDEO2SECOND).IsNull()))
			{
				byAddSelByMcsMode = MODE_VIDEO2SECOND;
			}
		}
		
		if( MODE_NONE != byAddSelByMcsMode )
		{
			tMtStatus.AddSelByMcsMode( byMediaMode );
			m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );
		}
		else
		{
			m_ptMtTable->RemoveMtSelMtByMode(byDstMtId, byMediaMode);
		}
	}
	else if(EQP_TYPE_VMP == tSrc.GetEqpType())
	{
		//20110623 zjl vmp�ǹ㲥ʱ����ϯ�ն�ѡ��vmp��������ϯ�ն�ѡ��ԴΪvmp����elseѡ��Դ�ֱ���գ�
		//����ͣѡ��vmpʱ�޷���ȷ��ʶ������ϯ�Ƿ�vmp���޷���֪�ն˵�ǰ״̬��rtcp������Ҫ֪��Դ��Ϣ�����Ҳ�޷����
		//����������ʱ��������
	}
	else
	{
		m_ptMtTable->RemoveMtSelMtByMode(byDstMtId, byMediaMode);
	}


	if( bMsgStatus )
	{
		TMt tDstMt = m_ptMtTable->GetMt(byDstMtId);
		if (MODE_VIDEO2SECOND == byMediaMode)
		{
			MtSecSelSrcChange(tDstMt);
		}	
		MtStatusChange(&tDstMt, TRUE);
	}
}

/*====================================================================
    ������      ��StopSwitchToSubMt
    ����        ��ֹͣ�����ݽ�����ֱ���¼��Ǵμ��ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byDstMtId, Ŀ���ն˺�
				  BOOL32 bMsg, �Ƿ�֪ͨ������Ϣ��ȱʡΪTRUE
				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/11    1.0         LI Yi         ����
====================================================================*/
// void CMcuVcInst::StopSwitchToSubMt( u8 byDstMtId, u8 byMode,u8 bySwitchMode,BOOL32 bMsgStatus, BOOL32 bStopSelByMcs /* = TRUE*/, BOOL32 bSelSelf,u16 wSpyPort )
// {
// 	CServMsg	cServMsg;
// 	TMt    TMtNull;
// 	TMtStatus   tMtStatus;
// 
// 	TMtNull.SetNull();
// 	TMt tDstMt = m_ptMtTable->GetMt( byDstMtId );
//     m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );
// 
//     //Ŀ���ն��Ƿ��ڻ���
//     if (tMtStatus.IsMediaLoop(MODE_VIDEO))
//     {
//         if (byMode == MODE_VIDEO)
//         {
//             return;
//         }
//         else if (byMode == MODE_BOTH)
//         {
//             byMode = MODE_AUDIO;
//         }
//     }
//     
//     if (tMtStatus.IsMediaLoop(MODE_AUDIO))
//     {
//         if (byMode == MODE_AUDIO)
//         {
//             return;
//         }
//         else if (byMode == MODE_BOTH)
//         {
//             byMode = MODE_VIDEO;
//         }
//     }
// 
//     if ( bSelSelf &&
//           // Ϊͼ��ƽ����vcs�ڵ���ģʽ����ϯ�ն˲����п��Լ���ͼ���л���ֱ����һ�������ն�����
// 		 !(VCS_CONF == m_tConf.GetConfSource() && 
//  		   byDstMtId == m_tConf.GetChairman().GetMtId() && 
//  		   (VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
// 		    !m_cVCSConfStatus.GetCurVCMT().IsNull()) ||
// 		   (ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
// 		    m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP)))
//     {
// 		// xliang [4/15/2009] FIXME����ϯ��ѡ��ĳ��MT�л���ѡ��VMP���ͻ��ȿ��Լ��ٿ�VMP ��
// 	    //֪ͨ�ն�ֹͣ���գ����Լ� 
// 	    if( byMode == MODE_BOTH || byMode == MODE_VIDEO )
// 	    {
// 		    if( m_tConfAllMtInfo.MtJoinedConf( byDstMtId ) )
// 		    {                    
//                 NotifyMtReceive( tDstMt, byDstMtId );
// 		    }		
// 	    }
//     }
// 	
//     // zbq [06/20/2007] Ϊ��֤������ƽ���п��������ˣ��˴���ͣ��һ��������
//     // �˵�����Ƶ�������������µĵ����˽�������ʱ���StartStopSwitch()֮ͣ��
//     // zbq [09/11/2007] FIXME: �˴�δ���⴦��������߼������µĵ����˿�����л��Ļ������� �������ǽ��
//     // if ( !(!m_tRollCaller.IsNull() && tDstMt == m_tRollCaller) )
//     {
//         // stop switch
//         g_cMpManager.StopSwitchToSubMt( tDstMt, byMode,FALSE,wSpyPort );
//     }
// 		
// 	if( bStopSelByMcs )
// 	{
// 		tMtStatus.RemoveSelByMcsMode( byMode );
// 		// ����ϴ�ѡ���ն�	
// 		TMt tNullMt;
// 		tNullMt.SetNull();
// 		tMtStatus.SetSelectMt(tNullMt, byMode);
// 	}
// 
// 	m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );	
// 	
//     // guzh [8/31/2006] Ҫ�������ƶ������棬����SetMtStatus��MtSrc�ᱻ���
// 	// �����ն���Ƶ״̬Ϊ���Լ�������Ƶ�� TMTNull
//     if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
//     {
//         m_ptMtTable->SetMtSrc( byDstMtId, &TMtNull, MODE_AUDIO );    
//     }
// 
//     if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
//     {
// 		if( MT_TYPE_MMCU != m_ptMtTable->GetMtType( byDstMtId )  )
// 		{
// 			m_ptMtTable->SetMtSrc( byDstMtId, &tDstMt, MODE_VIDEO );
// 		}
// 		else
// 		{
// 			m_ptMtTable->SetMtSrc( byDstMtId, &TMtNull, MODE_VIDEO );
// 		}
//     }	
// 
// 	if( bMsgStatus )
// 	{
//         MtStatusChange( &tDstMt, TRUE );
// 	}
// }

/*====================================================================
    ������      ��StopSwitchToSubMt
    ����        ��ֹͣ�����ݽ�����ֱ���¼��Ǵμ��ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtNum �ն���Ŀ
	              TMt *ptDstMt �ն���Ϣ
				  u8 byMode ����Ƶģʽ
				  BOOL32 bMsg, �Ƿ�֪ͨ������Ϣ��ȱʡΪTRUE
				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/11    1.0         LI Yi         ����
	20110505    4.6         pengjie       �����𽻻�֧��
====================================================================*/
void CMcuVcInst::StopSwitchToSubMt(u8 byMtNum,
								   TMt *ptDstMt,
								   u8 byMode, // MODE_VIDEO Or MODE_AUDIO
								   BOOL32 bMsgStatus, 
								   BOOL32 bStopSelByMcs, 
								   BOOL32 bSelSelf,
								   u16 wSpyPort)
{
	if( byMtNum == 0 || ptDstMt == NULL )
	{
		return;
	}

	TMt atDstMt[MAXNUM_CONF_MT];
	memcpy( atDstMt, ptDstMt, sizeof(TMt)*byMtNum );

	if( MODE_BOTH == byMode )
	{
		StopSwitchToSubMt( byMtNum, atDstMt, MODE_VIDEO, bMsgStatus, bStopSelByMcs, bSelSelf, wSpyPort );
		StopSwitchToSubMt( byMtNum, atDstMt, MODE_AUDIO, bMsgStatus, bStopSelByMcs, bSelSelf, wSpyPort );
		return;
	}

	TMt    TMtNull;
	TMtStatus   tMtStatus;
	TMtNull.SetNull();

	for( u8 byLoop = 0; byLoop < byMtNum; byLoop++ )
	{
		m_ptMtTable->GetMtStatus( atDstMt[byLoop].GetMtId(), &tMtStatus );

		//Ŀ���ն��Ƿ��ڻ���
		if (tMtStatus.IsMediaLoop(MODE_VIDEO) && byMode == MODE_VIDEO)
		{
			atDstMt[byLoop].SetNull();
			continue;
		}

		if (tMtStatus.IsMediaLoop(MODE_AUDIO) && byMode == MODE_AUDIO )
		{
			atDstMt[byLoop].SetNull();
			continue;
		}

		if ( bSelSelf &&
			// Ϊͼ��ƽ����vcs�ڵ���ģʽ����ϯ�ն˲����п��Լ���ͼ���л���ֱ����һ�������ն�����
			/*!(VCS_CONF == m_tConf.GetConfSource() && 
			atDstMt[byLoop].GetMtId() == m_tConf.GetChairman().GetMtId() && 
			(VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
			!m_cVCSConfStatus.GetCurVCMT().IsNull()) ||
			(ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
			m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP &&
			m_cVCSConfStatus.GetChairPollState() != VCS_POLL_PAUSE))*/			
			( VCS_CONF != m_tConf.GetConfSource() ||
				( atDstMt[byLoop].GetMtId() != m_tConf.GetChairman().GetMtId() && 
					( !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) || GetVidBrdSrc().IsNull() && !m_tConfAllMtInfo.MtJoinedConf(m_tConf.GetChairman()) ) 
				) ||
				( atDstMt[byLoop].GetMtId() == m_tConf.GetChairman().GetMtId() &&
					( ( VCS_SINGLE_MODE != m_cVCSConfStatus.GetCurVCMode() || m_cVCSConfStatus.GetCurVCMT().IsNull() ) ||
						( !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) ||
							m_cVCSConfStatus.GetChairPollState() == VCS_POLL_STOP ||
							m_cVCSConfStatus.GetChairPollState() == VCS_POLL_PAUSE 
						)
					)
				) 			
			)			
		)
		{
			// xliang [4/15/2009] FIXME����ϯ��ѡ��ĳ��MT�л���ѡ��VMP���ͻ��ȿ��Լ��ٿ�VMP ��
			//֪ͨ�ն�ֹͣ���գ����Լ� 
			if( byMode == MODE_BOTH || byMode == MODE_VIDEO )
			{
				if( m_tConfAllMtInfo.MtJoinedConf( atDstMt[byLoop].GetMtId() ) )
				{                    
					NotifyMtReceive( atDstMt[byLoop], atDstMt[byLoop].GetMtId() );
				}		
			}
		}
	}

	g_cMpManager.StopSwitchToSubMt( m_byConfIdx, byMtNum, atDstMt, byMode, wSpyPort);

	for( u8 byMtIdx = 0; byMtIdx < byMtNum; byMtIdx++ )
	{
		m_ptMtTable->GetMtStatus( atDstMt[byMtIdx].GetMtId(), &tMtStatus );
		if( bStopSelByMcs )
		{
			tMtStatus.RemoveSelByMcsMode( byMode );
		}
		
		m_ptMtTable->SetMtStatus( atDstMt[byMtIdx].GetMtId(), &tMtStatus );	
		
		// guzh [8/31/2006] Ҫ�������ƶ������棬����SetMtStatus��MtSrc�ᱻ���
		// �����ն���Ƶ״̬Ϊ���Լ�������Ƶ�� TMTNull
		//zhouyiliang 20121015 �����Ŀ���ն����ϼ�mcu����ͣ���Ƕ�ش��Ľ�����������srcmt
		if ( m_tCascadeMMCU.IsNull() || atDstMt[byMtIdx].GetMtId() != m_tCascadeMMCU.GetMtId() 
			|| wSpyPort == SPY_CHANNL_NULL )
		{
			if (byMode == MODE_AUDIO )
			{
				m_ptMtTable->SetMtSrc( atDstMt[byMtIdx].GetMtId(), &TMtNull, MODE_AUDIO );    
			}
			
			if (byMode == MODE_VIDEO )
			{
				if( MT_TYPE_MMCU != m_ptMtTable->GetMtType( atDstMt[byMtIdx].GetMtId() )  )
				{
					m_ptMtTable->SetMtSrc( atDstMt[byMtIdx].GetMtId(), &atDstMt[byMtIdx], MODE_VIDEO );
				}
				else
				{
					m_ptMtTable->SetMtSrc( atDstMt[byMtIdx].GetMtId(), &TMtNull, MODE_VIDEO );
				}
			}	
		}
	
		
		if( bMsgStatus )
		{
			MtStatusChange( &atDstMt[byMtIdx], TRUE );
		}
	}
	

	return;
}

/*=============================================================================
    �� �� ���� StartSwitchToMMcu
    ��    �ܣ� ��ָ���ն����ݽ�����mcu
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const TMt & tSrc, Դ
			   u8 bySrcChnnl, Դ���ŵ���
			   u8 byDstMtId, Ŀ���ն�
			   u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH 
    ����ֵ˵��  ��TRUE�����������ֱ�ӷ���FALSE
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/5/23   3.6			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::StartSwitchToMcu(TMt tSrc,
                                    u8  bySrcChnnl,
                                    u8  byDstMtId,
                                    u8  byMode,
                                    u8  bySwitchMode,
                                    BOOL32 bMsgStatus)
{
	TMtStatus   tMtStatus;
	CServMsg    cServMsg;
	u8 byMediaMode = byMode;
    BOOL32 bSetRealMediaMode = FALSE;

	TMt tUnLocalSrc = tSrc;

	tSrc = GetLocalMtFromOtherMcuMt(tSrc);

	//����KDC�ն��Ƿ�������״̬�����򲻽�����Ƶ����	
	if( TYPE_MT == m_ptMtTable->GetMainType( tSrc.GetMtId() ) && 
		MT_MANU_KDC != m_ptMtTable->GetManuId( tSrc.GetMtId() ) && 
		m_ptMtTable->IsMtAudioDumb( tSrc.GetMtId() ) )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			return TRUE;
		}
		else if ( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_VIDEO;
			bSetRealMediaMode = TRUE;
		}
	}

	//����KDC�ն��Ƿ��ھ���״̬����������������Ƶ����	
	if( TYPE_MT == m_ptMtTable->GetMainType( byDstMtId ) && 
		MT_MANU_KDC != m_ptMtTable->GetManuId( byDstMtId ) && 
		m_ptMtTable->IsMtAudioMute( byDstMtId ) )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			return TRUE;
		}
		else if ( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_VIDEO;
			bSetRealMediaMode = TRUE;
		}
	}

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

// 	TLogicalChannel     tAudChnnl;
// 	m_ptMtTable->GetMtLogicChnnl( tSrc, LOGCHL_AUDIO, &tAudChnnl, FALSE );	
	
	//ֻ���ϼ�Mcu��spyMt
	TMt tDstMt = m_ptMtTable->GetMt(byDstMtId);
	if(m_tCascadeMMCU.GetMtId() != 0 && byDstMtId == m_tCascadeMMCU.GetMtId())
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tDstMt.GetMtId()));
		if( ptConfMcInfo != NULL && 
			( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
		{
			if(ptConfMcInfo->m_tSpyMt.GetMtId() == tSrc.GetMtId()&&
				ptConfMcInfo->m_tSpyMt.GetMcuId() == tSrc.GetMcuId())
			{
				// �����ն˻򵥻ش��¼��ն˽��ϴ�ͨ��
			}
			else if (!tUnLocalSrc.IsLocal() && IsLocalAndSMcuSupMultSpy(tUnLocalSrc.GetMcuId())
				&&  ptConfMcInfo->m_tSpyMt.GetMtId() == tUnLocalSrc.GetMtId()&&
					ptConfMcInfo->m_tSpyMt.GetMcuId() == tUnLocalSrc.GetMcuId())
			{
				// ��ش��¼��ն˽��ϴ�ͨ��
			}
			else
			{
				return FALSE;
			}
		}		
		//zjj20100211 �ն˻ش�ѡ��������
		if( !(TYPE_MT == tSrc.GetType() && SWITCH_MODE_SELECT == bySwitchMode) )
		{
			g_cMpManager.SetSwitchBridge(tSrc, 0, byMediaMode);
		}
		if( !IsCanSetOutView( tUnLocalSrc, byMediaMode ) )
		{
			return FALSE;
		}
	}
 
	//ֻ�����ն�, ���ý��н���
	m_ptMtTable->GetMtStatus(byDstMtId, &tMtStatus);
	if( ( (!tMtStatus.IsReceiveVideo()) && byMediaMode == MODE_VIDEO ) ||
		( (!tMtStatus.IsReceiveAudio()) && byMediaMode == MODE_AUDIO ) )
	{
		return FALSE;
	}

	if( m_tConf.m_tStatus.IsMixing() )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			return FALSE;
		}
		else if( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_VIDEO;
			bSetRealMediaMode = TRUE;
		}
	}

    // zbq [06/29/2007] �ش�ͨ������Ƶ������ǰ��ͣ��������DS�滻����
    BOOL32 bMediaDstMMcu = FALSE;
        
    // libo [11/24/2005]
    u16 wAdaptBitRate = 0;
    BOOL32 bAudBasCap = TRUE;
    BOOL32 bVidBasCap = TRUE;
	BOOL32 bCaseAdp   = FALSE;
	if (0 == m_tCascadeMMCU.GetMtId() || byDstMtId != m_tCascadeMMCU.GetMtId())
	{
		if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
        {
			if (IsNeedAdapt(tSrc, m_ptMtTable->GetMt(byDstMtId), MODE_AUDIO))
			{
				if (!m_tConf.m_tStatus.IsAudAdapting())
				{
					StartBrdAdapt(MODE_AUDIO);
				}
				else
				{
					RefreshBrdBasParamForSingleMt(byDstMtId, MODE_AUDIO);
					StartSwitchToSubMtFromAdp(byDstMtId, MODE_AUDIO);
				}
			}
			else
			{
				//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
				//Ŀ�ķ��ϼ�mcu bStopBeforeStart = true
				//g_cMpManager.StartSwitchToSubMt(tSrc, bySrcChnnl, tDstMt, MODE_AUDIO, bySwitchMode, FALSE, TRUE);
				TSwitchGrp tSwitchGrp;
				tSwitchGrp.SetSrcChnl(bySrcChnnl);
				tSwitchGrp.SetDstMtNum(1);
				tSwitchGrp.SetDstMt(&tDstMt);
				g_cMpManager.StartSwitchToAll(tSrc, 1, &tSwitchGrp, MODE_AUDIO, bySwitchMode);
			}
		}

		if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
        {
			if (IsNeedAdapt(tSrc, m_ptMtTable->GetMt(byDstMtId), MODE_VIDEO))
			{
				if (!m_tConf.m_tStatus.IsVidAdapting())
				{
					StartBrdAdapt(MODE_VIDEO);
				}
				else
				{
					RefreshBrdBasParamForSingleMt(byDstMtId, MODE_VIDEO);
					StartSwitchToSubMtFromAdp(byDstMtId, MODE_VIDEO);
				}
			}
			else
			{
				//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
				//Ŀ�ķ��ϼ�mcu bStopBeforeStart = true
				//g_cMpManager.StartSwitchToSubMt(tSrc, bySrcChnnl, tDstMt, MODE_VIDEO, bySwitchMode, FALSE, TRUE);
				TSwitchGrp tSwitchGrp;
				tSwitchGrp.SetSrcChnl(bySrcChnnl);
				tSwitchGrp.SetDstMtNum(1);
				tSwitchGrp.SetDstMt(&tDstMt);
				g_cMpManager.StartSwitchToAll(tSrc, 1, &tSwitchGrp, MODE_VIDEO, bySwitchMode);
			}
		}
	}
	else
	{
		CRecvSpy tSpyResource;
		u16 wSpyPort = SPY_CHANNL_NULL;	
		if( m_cSMcuSpyMana.GetRecvSpy( tUnLocalSrc, tSpyResource ) )
		{
			wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
		}

		if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
        {
			//ͣ��һ��ѡ������
			TMtStatus tMMcuStatus;
			m_ptMtTable->GetMtStatus(m_tCascadeMMCU.GetMtId(), &tMMcuStatus);
			TMt tOldAudSel = tMMcuStatus.GetSelectMt(MODE_AUDIO);
			if (!tOldAudSel.IsNull() &&
				IsNeedSelAdpt(tOldAudSel, m_tCascadeMMCU, MODE_AUDIO))
			{
				StopSelAdapt(tOldAudSel, m_tCascadeMMCU, MODE_AUDIO,TRUE,FALSE);
			}

			//��Ƶ����
			if (IsNeedSelAdpt(tSrc, m_tCascadeMMCU, MODE_AUDIO))
			{
				StartSelAdapt(tUnLocalSrc, m_tCascadeMMCU, MODE_AUDIO);
			}
			else
			{	
				//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
				//Ŀ���ϼ�mcu bStopBeforeStart = TRUE
				//zhouyiliang 20100826�޸Ļش�ͨ���ն��滻��������stopbeforestart Ϊtrue
				//g_cMpManager.StartSwitchToSubMt(tSrc, bySrcChnnl, m_tCascadeMMCU, MODE_AUDIO, bySwitchMode, FALSE, TRUE);
				TSwitchGrp tSwitchGrp;
				tSwitchGrp.SetSrcChnl(bySrcChnnl);
				tSwitchGrp.SetDstMtNum(1);
				tSwitchGrp.SetDstMt(&m_tCascadeMMCU);
				g_cMpManager.StartSwitchToAll(tSrc, 1, &tSwitchGrp, MODE_AUDIO, bySwitchMode, TRUE, wSpyPort);
			}	
		}

		if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
		{
			//ͣ��һ��ѡ������
			TMtStatus tMMcuStatus;
			m_ptMtTable->GetMtStatus(m_tCascadeMMCU.GetMtId(), &tMMcuStatus);
			TMt tOldVidSel = tMMcuStatus.GetSelectMt(MODE_VIDEO);
			if (!tOldVidSel.IsNull() &&
				IsNeedSelAdpt(tOldVidSel, m_tCascadeMMCU, MODE_VIDEO))
			{
				StopSelAdapt(tOldVidSel, m_tCascadeMMCU, MODE_VIDEO,TRUE,FALSE);
			}

			//��Ƶ����
			if (IsNeedSelAdpt(tSrc, m_tCascadeMMCU, MODE_VIDEO))
			{
				StartSelAdapt(tUnLocalSrc, m_tCascadeMMCU, MODE_VIDEO);
			}
			else
			{	
				//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
				//Ŀ���ϼ�mcu bStopBeforeStart = TRUE
				//zhouyiliang 20100826�޸Ļش�ͨ���ն��滻��������stopbeforestart Ϊtrue
				//g_cMpManager.StartSwitchToSubMt(tSrc, bySrcChnnl, m_tCascadeMMCU, MODE_VIDEO, bySwitchMode, FALSE, TRUE);
				TSwitchGrp tSwitchGrp;
				tSwitchGrp.SetSrcChnl(bySrcChnnl);
				tSwitchGrp.SetDstMtNum(1);
				tSwitchGrp.SetDstMt(&m_tCascadeMMCU);
				g_cMpManager.StartSwitchToAll(tSrc, 1, &tSwitchGrp, MODE_VIDEO, bySwitchMode, TRUE, wSpyPort);
			}
		}
	}

	//֪ͨ�ն˿�ʼ���� 
	if( tSrc.GetType() == TYPE_MT )
	{
		NotifyMtSend( tSrc.GetMtId(), byMediaMode, TRUE );

        // zbq [06/25/2007] ����Ƶ����������ؼ�֡
        if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
        {
            NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
        }
	}

	//�����ն˵�������ʾ��ʶ
	TPeriEqpStatus tEqpStatus;
	if( tSrc == m_tPlayEqp )
	{
		g_cMcuVcApp.GetPeriEqpStatus( m_tPlayEqp.GetEqpId(), &tEqpStatus );
		cServMsg.SetMsgBody( (u8*)tEqpStatus.GetAlias(), MAXLEN_EQP_ALIAS );
		SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
	}

	//֪ͨ�ն˿�ʼ����
	if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
	{
		NotifyMtReceive( tSrc, tDstMt.GetMtId() );
	}

	//�����ն�״̬
	m_ptMtTable->SetMtSrc( byDstMtId, &tUnLocalSrc, bSetRealMediaMode ? byMode:byMediaMode );

	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();

	//20110519 zjl ָ���¼��ն˻ش����൱���ϼ�mcuѡ���¼��նˣ��ش�������ʱ�����ñ���selectmt,
	//��Ϊ�ͷ�bas��������Ҫ֪���ն�ѡ��״̬�������ﲻ����mcsѡ��״̬
	///*(m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId())*/
	//�����жϼ�����������ˣ�����startseladpt�����õ�selectmt�������ﱻ���
	
	if(0 == m_tCascadeMMCU.GetMtId() || m_tCascadeMMCU.GetMtId() != tDstMt.GetMtId())
	{
		m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );
		if( tSrc.GetType() == TYPE_MT && tSrc.GetMtId() != byDstMtId //&& 
			/*(m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId())*/ )
		{
			TMtStatus tSrcMtStatus;
			u8 byAddSelByMcsMode = MODE_NONE;
			m_ptMtTable->GetMtStatus( tSrc.GetMtId(), &tSrcMtStatus );
			
			if( !(tSrc == tLocalVidBrdSrc) && tSrcMtStatus.IsSendVideo() && 
				( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode ) )
			{
				byAddSelByMcsMode = MODE_VIDEO;
			}
			if( !(tSrc == GetLocalAudBrdSrc()) && tSrcMtStatus.IsSendAudio() && 
				( MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode ) )
			{
				if( MODE_VIDEO == byAddSelByMcsMode )
				{
					byAddSelByMcsMode = MODE_BOTH;
				}
				else
				{
					byAddSelByMcsMode = MODE_AUDIO;
				}
			}
			
			if( MODE_NONE != byAddSelByMcsMode )
			{
				tMtStatus.AddSelByMcsMode( byMediaMode );
			}
			else
			{
				tMtStatus.RemoveSelByMcsMode( byMediaMode );
				// ����ϴ�ѡ���ն�	
				TMt tNullMt;
				tNullMt.SetNull();
				tMtStatus.SetSelectMt(tNullMt, byMediaMode);
			}
		}
		else
		{
			tMtStatus.RemoveSelByMcsMode( byMediaMode );
			// ����ϴ�ѡ���ն�	
			TMt tNullMt;
			tNullMt.SetNull();
			tMtStatus.SetSelectMt(tNullMt, byMediaMode);
		}
		m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );
	}


	
	if( bMsgStatus )
	{
        MtStatusChange(&tDstMt, TRUE);
	}
    
	return TRUE;
}

/*=============================================================================
    �� �� ���� StartSwitchToMMcu
    ��    �ܣ� ��ָ���ն����ݽ�����mcu
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const TMt & tSrc, Դ
			   u8 bySrcChnnl, Դ���ŵ���
			   u8 byDstMtId, Ŀ���ն�
			   const TSimCapSet &tDstCap, Ŀ�Ķ�����
			   u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH 
    ����ֵ˵��  ��TRUE�����������ֱ�ӷ���FALSE
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/5/23   3.6			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::StartSwitchToMcuByMultiSpy(TMt tSrc,
                                    u16  wSrcChnnl,
                                    u8  byDstMtId,
									const TSimCapSet &tDstCap,
									u16 &wErrorCode,
                                    u8  byMode,
                                    u8  bySwitchMode,
                                    BOOL32 bMsgStatus)
{
	TMtStatus   tMtStatus;
	CServMsg    cServMsg;
	u8 byMediaMode = byMode;
    BOOL32 bSwitchedAudio = FALSE;

	wErrorCode = 0;

	TMt tUnLocalSrc = tSrc;

	tSrc = GetLocalMtFromOtherMcuMt(tSrc);

	//����KDC�ն��Ƿ�������״̬�����򲻽�����Ƶ����	
	if( TYPE_MT == m_ptMtTable->GetMainType( tSrc.GetMtId() ) && 
		MT_MANU_KDC != m_ptMtTable->GetManuId( tSrc.GetMtId() ) && 
		m_ptMtTable->IsMtAudioDumb( tSrc.GetMtId() ) )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			return TRUE;
		}
		else if ( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_VIDEO;
		}
	}

	//����KDC�ն��Ƿ��ھ���״̬����������������Ƶ����	
	if( TYPE_MT == m_ptMtTable->GetMainType( byDstMtId ) && 
		MT_MANU_KDC != m_ptMtTable->GetManuId( byDstMtId ) && 
		m_ptMtTable->IsMtAudioMute( byDstMtId ) )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			return TRUE;
		}
		else if ( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_VIDEO;
		}
	}

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

	//zjj20100201
	//  [11/9/2009 pengjie] ������ش��ж�
	// [11/14/2009 xliang] mode ��ȷ�����������������byMediaMode  
	CSendSpy cSendSpy;
	u16 wSpyPort = SPY_CHANNL_NULL;
	if( m_cLocalSpyMana.GetSpyChannlInfo(tUnLocalSrc, cSendSpy) )
	{
		wSpyPort = cSendSpy.m_tSpyAddr.GetPort() ;
		
		//��һ��ȷ��mode
		//byMediaMode = m_cLocalSpyMana.GetSpyMode(tUnLocalSrc);
	}
	
	//ֻ���ϼ�Mcu��spyMt
	TMt tDstMt = m_ptMtTable->GetMt(byDstMtId);
	if(m_tCascadeMMCU.GetMtId() != 0 && byDstMtId == m_tCascadeMMCU.GetMtId())
	{
		if( !m_cLocalSpyMana.GetSpyChannlInfo(tUnLocalSrc, cSendSpy) )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy] tSrc(%d.%d) is not in SpyChannelInfo.\n",
				tSrc.GetMcuId(),tSrc.GetMtId()
				);
			
			return FALSE;
		}
		if( SPY_CHANNL_NULL == cSendSpy.m_tSpyAddr.GetPort() || 0 == cSendSpy.m_tSpyAddr.GetPort() )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy] tSrc(%d.%d) m_wSpyStartPort(%d) Get Failed.\n",
				tSrc.GetMcuId(),tSrc.GetMtId(),cSendSpy.m_tSpyAddr.GetPort() );
			return FALSE;
		}		
	}
 
	//ֻ�����ն�, ���ý��н���
	m_ptMtTable->GetMtStatus(byDstMtId, &tMtStatus);
	if( ( (!tMtStatus.IsReceiveVideo()) && byMediaMode == MODE_VIDEO ) ||
		( (!tMtStatus.IsReceiveAudio()) && byMediaMode == MODE_AUDIO ) )
	{
		return FALSE;
	}

	//2010/06/24 lukunpeng ��ȫ�����ϼ�����˼����ʹ�ڻ����������Ѵ��ն���Ƶ���ϼ�
	/*
	if( m_tConf.m_tStatus.IsMixing() )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			return FALSE;
		}
		else if( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_VIDEO;
		}
	}
	*/

    // zbq [06/29/2007] �ش�ͨ������Ƶ������ǰ��ͣ��������DS�滻����
    BOOL32 bMediaDstMMcu = FALSE;

    // libo [11/24/2005]
    u16 wAdaptBitRate = 0;
    BOOL32 bAudBasCap = TRUE;
    BOOL32 bVidBasCap = TRUE;
	BOOL32 bCaseAdp   = FALSE;
	TEqp tCasdVidBasEqp,tCasdAudBasEqp;
	tCasdVidBasEqp.SetNull();
	tCasdAudBasEqp.SetNull();
	u8 byCasdAudBasChnnl=EQP_CHANNO_INVALID,byCasdVidBasChnnl=EQP_CHANNO_INVALID;

	TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
	// [2013/04/19 chenbing] MPEG-4 AUTO���ݻ������ʵõ��ֱ��� 
	if ( VIDEO_FORMAT_AUTO == tSrcCap.GetVideoResolution() )
	{
		//���ݻ������ʵõ��ֱ���
		tSrcCap.SetVideoResolution(GetAutoResByBitrate(VIDEO_FORMAT_AUTO, m_tConf.GetBitRate()));
	}
	if ( MEDIA_TYPE_MP4 == tSrcCap.GetVideoMediaType() && VIDEO_FORMAT_16CIF == tSrcCap.GetVideoResolution() )
	{
		tSrcCap.SetVideoResolution( VIDEO_FORMAT_4CIF );
	}

	if (0 == m_tCascadeMMCU.GetMtId() || byDstMtId != m_tCascadeMMCU.GetMtId())
	{
		ConfPrint( LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy] The m_tCascadeMMCU is Null or m_tCascadeMMCU.%d != DstMtId.%\n",
			m_tCascadeMMCU.GetMtId(), byDstMtId );
		return FALSE;
	}
	else
	{
		if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
        {
			// [pengjie 2010/9/13] �����ǲ���keda�ģ���Ƶ��ƥ�䶼Ҫ������
			// 1.1 ��ƵҪ������			
			TAudioTypeDesc tSrcAudCap;		
			TMt tLocalSrcMt = GetLocalMtFromOtherMcuMt( tSrc );
			TLogicalChannel tSrcAudLgc;
			if (!m_ptMtTable->GetMtLogicChnnl(tLocalSrcMt.GetMtId(), MODE_AUDIO, &tSrcAudLgc, FALSE))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS,  "[IsNeedAdapt] GetMtLogicChnnl Src AUD Mt%d failed!\n", tLocalSrcMt.GetMtId());
				return FALSE;
			}
			
			tSrcAudCap.SetAudioMediaType( tSrcAudLgc.GetChannelType() );
			tSrcAudCap.SetAudioTrackNum( tSrcAudLgc.GetAudioTrackNum() );
			
			TAudioTypeDesc tDstAudCap;
			TLogicalChannel tDstAudLgc;
			if (!m_ptMtTable->GetMtLogicChnnl(m_tCascadeMMCU.GetMtId(), MODE_AUDIO, &tDstAudLgc, TRUE))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS,  "[IsNeedSpyAdpt] GetMtLogicChnnl Dst Aud Mt%d failed!\n", m_tCascadeMMCU.GetMtId());
				return FALSE;
			}
			
			tDstAudCap.SetAudioMediaType( tDstAudLgc.GetChannelType() );
			tDstAudCap.SetAudioTrackNum( tDstAudLgc.GetAudioTrackNum() );	

			if (  tSrcAudCap.GetAudioMediaType() != MEDIA_TYPE_NULL &&
				tDstAudCap.GetAudioMediaType() != MEDIA_TYPE_NULL &&
				 ( tSrcAudCap.GetAudioMediaType() != tDstAudCap.GetAudioMediaType() 
			    	||tSrcAudCap.GetAudioTrackNum() != tDstAudCap.GetAudioTrackNum()
				  )
				)
			{
				if ( !StartSpyAudAdapt(tUnLocalSrc) )
				{
					wErrorCode = ERR_MCU_CASDBASISNOTENOUGH;
					return FALSE;
				}
			}
			// 1.2 ��Ƶ��������
			else
			{
				BOOL32 bStopBeforeStart = bMediaDstMMcu ? FALSE : TRUE;
				if (!g_cMpManager.StartSwitchToMmcu(tSrc, wSrcChnnl, tDstMt, MODE_AUDIO, bySwitchMode,
					FALSE, bStopBeforeStart, FALSE, wSpyPort))        
				{
					ConfPrint( LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy] StartSwitchToMmcu() failed! Cannot switch to specified sub mt!\n" );
					return FALSE;
				}
			}
		}
		
		//��Ƶ����
		if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
		{
			// ������
			//20101213 zjl ��ʽ��ͬ����Դ�ķֱ����п����һ�ߴ���Ŀ�ģ�����Ϊ���Ե��ֱ���
			s32 nResCmpRst = ResWHCmp(tSrcCap.GetVideoResolution(), tDstCap.GetVideoResolution());
			if(IsNeedSpyAdpt(tSrc, tDstCap, MODE_VIDEO))
			{
				if (!StartSpyAdapt(tUnLocalSrc, tDstCap, MODE_VIDEO))
				{
					wErrorCode = ERR_MCU_CASDBASISNOTENOUGH;
					return FALSE;
				}
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy] StartSpyAdapt OK!\n");
			}
			//���ֱ���
			else if (tSrcCap.GetVideoMediaType() == tDstCap.GetVideoMediaType() &&
				( 1 == nResCmpRst || 
				-1 == nResCmpRst || 
				2 == nResCmpRst))
			{
				// [pengjie 2010/9/13] ��ش����ֱ����߼�����
				u8 byChnnlType = LOGCHL_VIDEO;
				u8 byRes = tDstCap.GetVideoResolution();

				ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY,  "[StartSwitchToMcuByMultiSpy] Adjust SendChangeMtRes %d.fps!\n",
						tDstCap.GetVideoResolution());

				// [2013/05/16 chenbing] ���·ֱ��ʵ���ֻ�ܲ��������ն�
				// Mcu���¼��ն˲����в���
				//Bug00161920:��������֧���ϴ������У�������MCU�����ǽ����ϳ�ͨ�����������ն˽��ϴ�ͨ���󲻽��ֱ���
				//yrl20131114:��������ش���3��mcu���ϴ��ն˽�1��mcu����ǽ4����(��TVWall��VMP)Ҫ�󽵷ֱ���
				//if (!IsMcu(tSrc) && tSrc.IsLocal())
				if (tSrc.IsLocal())
				{
					SendChgMtVidFormat( tSrc.GetMtId(), byChnnlType, byRes, TRUE );
				}
				
				if ( IsNeedAdjustSpyFps(tSrc, tDstCap) )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY,  "[StartSwitchToMcuByMultiSpy] 2Adjust tSrc(%d, %d) SendChangeMtFps %d.fps!\n",tSrc.GetMcuId(), tSrc.GetMtId(),
						tDstCap.GetUserDefFrameRate());
					SendChangeMtFps(tSrc.GetMtId(), LOGCHL_VIDEO, tDstCap.GetUserDefFrameRate());
				}
			
				//ֱ�Ӵ򽻻�
				BOOL32 bStopBeforeStart = bMediaDstMMcu ? FALSE : TRUE;
				if (!g_cMpManager.StartSwitchToMmcu(tSrc, wSrcChnnl, tDstMt, MODE_VIDEO, bySwitchMode, FALSE, bStopBeforeStart, FALSE, wSpyPort))        
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy] StartSwitchToMmcu() failed! Cannot switch to specified sub mt!\n");
					return FALSE;
				}
			}
			//��֡��
			else if (IsNeedAdjustSpyFps(tSrc, tDstCap))
			{
				if (tDstCap.GetVideoMediaType() == MEDIA_TYPE_H264)
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY,  "[StartSwitchToMcuByMultiSpy] Adjust SendChangeMtFps %d.fps!\n",
						tDstCap.GetUserDefFrameRate());
					SendChangeMtFps(tSrc.GetMtId(), LOGCHL_VIDEO, tDstCap.GetUserDefFrameRate());
					
					BOOL32 bStopBeforeStart = bMediaDstMMcu ? FALSE : TRUE;
					if (!g_cMpManager.StartSwitchToMmcu(tSrc, wSrcChnnl, tDstMt, MODE_VIDEO, bySwitchMode, FALSE, bStopBeforeStart, FALSE, wSpyPort))        
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy-after adjust fps] StartSwitchToMmcu() failed! Cannot switch to specified sub mt!\n" );
						return FALSE;
					}
				}				
			}
			else
			{
				BOOL32 bStopBeforeStart = bMediaDstMMcu ? FALSE : TRUE;
				if (!g_cMpManager.StartSwitchToMmcu(tSrc, wSrcChnnl, tDstMt, MODE_VIDEO, bySwitchMode, FALSE, bStopBeforeStart, FALSE, wSpyPort))        
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy] StartSwitchToMmcu() failed! Cannot switch to specified sub mt!\n");
					return FALSE;
				}
			}
		}
	}
	
	//֪ͨ�ն˿�ʼ���� 
	if( tSrc.GetType() == TYPE_MT )
	{
		NotifyMtSend( tSrc.GetMtId(), byMediaMode, TRUE );
		
        // zbq [06/25/2007] ����Ƶ����������ؼ�֡
        if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
        {
            NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
        }
	}
	
	//�����ն˵�������ʾ��ʶ
	TPeriEqpStatus tEqpStatus;
	if( tSrc == m_tPlayEqp )
	{
		g_cMcuVcApp.GetPeriEqpStatus( m_tPlayEqp.GetEqpId(), &tEqpStatus );
		cServMsg.SetMsgBody( (u8*)tEqpStatus.GetAlias(), MAXLEN_EQP_ALIAS );
		SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
	}
	
	
	//֪ͨ�ն˿�ʼ����
	if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
	{
		NotifyMtReceive( tSrc, tDstMt.GetMtId() );
	}
	
	//�����ն�״̬
	if( m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId() )
	{
		m_ptMtTable->SetMtSrc( byDstMtId, ( const TMt * )&tUnLocalSrc, byMediaMode );
	}	

	//[2011/07/22/zhangli]��δ���ɾ��
	//�������жϣ�if (0 == m_tCascadeMMCU.GetMtId() || byDstMtId != m_tCascadeMMCU.GetMtId())��return
	//���if{}��Ϊ�٣�ͬʱɾ��else������ϴ�ѡ���ն�û�б�Ҫ(Ϊ�˽��Bug00059187)

// 	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();
// 	//����Ŀ���ն�Ϊ�ϼ�mcu�����
// 	m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );
// 	if( tSrc.GetType() == TYPE_MT && tSrc.GetMtId() != byDstMtId && 
// 		(m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId()) )
// 	{
// 		TMtStatus tSrcMtStatus;
// 		u8 byAddSelByMcsMode = MODE_NONE;
// 		m_ptMtTable->GetMtStatus( tSrc.GetMtId(), &tSrcMtStatus );
// 
// 		if( !(tSrc == tLocalVidBrdSrc) && tSrcMtStatus.IsSendVideo() && 
// 			( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode ) )
// 		{
// 			byAddSelByMcsMode = MODE_VIDEO;
// 		}
// 		if( !(tSrc == GetLocalAudBrdSrc()) && tSrcMtStatus.IsSendAudio() && 
// 			( MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode ) )
// 		{
// 			if( MODE_VIDEO == byAddSelByMcsMode )
// 			{
// 				byAddSelByMcsMode = MODE_BOTH;
// 			}
// 			else
// 			{
// 				byAddSelByMcsMode = MODE_AUDIO;
// 			}
// 		}
// 
// 		if( MODE_NONE != byAddSelByMcsMode )
// 		{
// 			tMtStatus.AddSelByMcsMode( byMediaMode );
// 		}
// 		else
// 		{
// 			tMtStatus.RemoveSelByMcsMode( byMediaMode );
// 			// ����ϴ�ѡ���ն�	
// 			TMt tNullMt;
// 			tNullMt.SetNull();
// 			tMtStatus.SetSelectMt(tNullMt, byMediaMode);
// 		}
// 	}
// 	else
// 	{
// 		tMtStatus.RemoveSelByMcsMode( byMediaMode );
// 		// ����ϴ�ѡ���ն�	
// 		TMt tNullMt;
// 		tNullMt.SetNull();
// 		tMtStatus.SetSelectMt(tNullMt, byMediaMode);
// 	}
// 	m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );
	
	if( bMsgStatus )
	{
        MtStatusChange(&tDstMt, TRUE);
	}
    
	return TRUE;
}

/*====================================================================
    ������      ��CanMtRecvVideo
    ����        ���ж�Ŀ���ն��Ƿ��ܽ�����Ƶ����(ͬʱ�ж�������)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tDst, ����Ŀ��
    ����ֵ˵��  ��TRUE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/03/31    4.0         ����        ����
====================================================================*/
BOOL32 CMcuVcInst::CanMtRecvVideo(const TMt &tDst, const TMt &tSrc)
{
    TMtStatus tStatus;

    u8 byMtId = tDst.GetMtId();
    m_ptMtTable->GetMtStatus(byMtId, &tStatus);
    
    if ( !m_tConfAllMtInfo.MtJoinedConf( byMtId ) || 
         !m_ptMtTable->IsLogicChnnlOpen(byMtId, LOGCHL_VIDEO, TRUE) //||
		 //zbq[08/27/2008] tSrc��tDst����mcu��������Ȼ����(Ŀǰ���ཻ��δ�߹㲥)
         /*tSrc == tDst*/ )
    {
        return FALSE;
    }            
    if ( !tStatus.IsReceiveVideo() )
    {
        return FALSE;
    }

    // ���Ŀ����MMCU��ֻ���ϼ�Mcu��spyMt������������
    if ( !m_tCascadeMMCU.IsNull() && tDst == m_tCascadeMMCU )
    {
		u16 wMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
        TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
        if  ( ptConfMcInfo != NULL && 
            ( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
        {
            if (!( ptConfMcInfo->m_tSpyMt.GetMtId() == tSrc.GetMtId() &&
                   ptConfMcInfo->m_tSpyMt.GetMcuId() == tSrc.GetMcuId() ) )
            {
                return FALSE;				
            }
        }
	}

    //�����ѽ��õ�һ·��Ƶͨ������˫�����նˣ��л���һ·��ƵԴ�����ܾ�
    u8 bySrcChn = tSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
    TMt tTmpSrc = tSrc;
    TMt tTmpDst = tDst;
    if( JudgeIfSwitchFirstLChannel(tTmpSrc, bySrcChn, tTmpDst) )
    {
        return FALSE;
	}

    //��Ҫ���Լ�������������ȥ, �����Դ��Ŀ���Ϊmcu����Ӧ���н���
    if( tSrc == tDst && 
        m_ptMtTable->GetManuId( byMtId ) == MT_MANU_KDC )
    {               
        return FALSE;
	}

    //�Ƿ���Ҫ����������ܽ���
//     if ( IsHDConf( m_tConf ) )
//     {
//         if (m_cMtRcvGrp.IsMtNeedAdp(byMtId))
//         {
//             return FALSE;
//         }
//     }
//     else
    //�Ƿ���Ҫ������ܽ���
    if (IsNeedAdapt(tSrc, tDst, MODE_VIDEO))
    {
        return FALSE;
    }
	
	// ���Ӷ�˫��ʽVMP���ж�, zgc, 20070604
	if( tStatus.IsReceiveVideo() && g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) )
	{
		TSimCapSet tSimCapSet; 
        u8 byMediaType = m_tConf.GetCapSupport().GetMainSimCapSet().GetVideoMediaType();
        u16 wDstMtReqBitrate = m_ptMtTable->GetMtReqBitrate( byMtId );

        //�ȴ�����ʽ�����
        if ( 0 != m_tConf.GetSecBitRate() &&
			MEDIA_TYPE_NULL == m_tConf.GetCapSupport().GetSecondSimCapSet().GetVideoMediaType() )
        {
			if ( wDstMtReqBitrate < m_wVidBasBitrate )
			{
				return FALSE;
			}
		}
		//˫��ʽ
		else
        {                
            tSimCapSet = m_ptMtTable->GetDstSCS( byMtId );
            if (tSimCapSet.GetVideoMediaType() == byMediaType)
            {
				if ( wDstMtReqBitrate < m_wVidBasBitrate )
				{
					return FALSE;
				}
            }
            else
            {
                // ����ʽ�ն˲��߹㲥������VMP�ڶ�·, zgc, 2008-09-20
                /*
				if ( wDstMtReqBitrate < m_wBasBitrate )
                {
                     return FALSE;
                }
                */
                return FALSE;
            }
        }          
	}

    return TRUE;
}


/*====================================================================
    ������      ��StartSwitchFromBrd
    ����        �����㲥Դ������������ָ���ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tDst, ����Ŀ��
    ����ֵ˵��  ��TRUE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/03/31    4.0         ����        ����
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchFromBrd(const TMt &tSrcMt,
                                      u8        bySrcChnnl,
                                      u8        byDstMtNum,
                                      const TMt* const ptDstBase,
									  u16 wSpyStartPort)
{
    u8 byLoop = 0;

	// [pengjie 2010/4/15] ��ش�֧��
	TMt tUnLocalMt = tSrcMt;
	TMt tSrc = GetLocalMtFromOtherMcuMt( tSrcMt );

    // zgc, 2008-05-28, ��ѡ��
    TMtStatus tMtStatus;
    u8 bySelMode = MODE_NONE;
	TMt tSelSrc;
	tSelSrc.SetNull();

	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();

	TMt tVidBrdSrc = GetVidBrdSrc();

    for ( byLoop = 0; byLoop < byDstMtNum; byLoop ++ )
    {
        // zgc, 2008-06-02, �����˺ͱ������˲��ӹ㲥����������ѡ��
        if ( tLocalVidBrdSrc == m_tVmpEqp && 
            ( ptDstBase[byLoop] == m_tRollCaller ||
            ptDstBase[byLoop] == m_tRollCallee) )
        {
            continue;
        }

        m_ptMtTable->GetMtStatus(ptDstBase[byLoop].GetMtId(), &tMtStatus);
        bySelMode = tMtStatus.GetSelByMcsDragMode();

		//lukunpeng 2010/07/22 ��Ҫͣ�����˵�ѡ��
		if (ptDstBase[byLoop] == m_tConf.GetSpeaker() || bySelMode == MODE_NONE)
		{
			continue;
		}

		//  [11/26/2009 pengjie] Modify ������ش� ����Ӧ�ø���ʵ��ѡ����ģʽ��������ֹͣ��������ֻͣ����
		u8 byIsRestore = FALSE;
		if(!tVidBrdSrc.IsNull() && tSrc == tVidBrdSrc)
		{
			byIsRestore = TRUE;
		}
		
		//ֻ����Ƶѡ��[6/14/2012 chendaiwei]
		if( MODE_BOTH == bySelMode || MODE_VIDEO == bySelMode )
		{
			StopSelectSrc(ptDstBase[byLoop], MODE_VIDEO, byIsRestore);
		}
    }

	//zjj20100402 ���ش�ͨ���նˣ��������������ϼ�mcu�����˿ڴ򽻻�
	BOOL32 bIsCanSwitchBrdToMMCU = TRUE;
	TConfMcInfo *ptConfMcInfo = NULL;
	u16 wMMcuIdx = INVALID_MCUIDX;
	if ( !m_tCascadeMMCU.IsNull() )
    {
		wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
        ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx);//m_tCascadeMMCU.GetMtId());
		if( NULL != ptConfMcInfo &&
			!( tSrc == ptConfMcInfo->m_tSpyMt ) )
		{
			bIsCanSwitchBrdToMMCU = FALSE;
		}
	}
    BOOL32 bRet = TRUE;
    // ���ڵ����Ľ����������ǵ��������¼�����ն˵ȣ�Ҫ�����ж��Ƿ��ܽ���
    // �����������������ڹ�������Ŀ���б�ʱͳһ�ж�
    if  ( byDstMtNum == 1 )
	{		
		if( CanMtRecvVideo(ptDstBase[0], tSrc) )
		{
            //StopSwitchToSubMt( ptDstBase[0].GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE, FALSE );

            // zbq [06/19/2007] �����˺ͱ������˲��ӹ㲥����
            if ( !( tLocalVidBrdSrc == m_tVmpEqp && 
                    ( ptDstBase[0] == m_tRollCaller ||
                      ptDstBase[0] == GetLocalMtFromOtherMcuMt(m_tRollCallee)) ) )
            {
				
				if (!m_tCascadeMMCU.IsNull()  &&
					m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx/*m_tCascadeMMCU.GetMtId()*/) != NULL &&
					m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx/*m_tCascadeMMCU.GetMtId()*/)->m_tSpyMt == tSrc &&
					m_tCascadeMMCU == ptDstBase[0] )
				{
					// xliang [3/2/2009] ����src�������ڻش�ͨ����򲻲𽻻�
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[StartSwitchFromBrd] Since the source is also under back-to-MMCU switch, we shouldn't stop switch in this case.\n" );
				}
				else
				{
					//zjl 20110510 StopSwitchToSubMt �ӿ������滻
					// ���BUG10501, �������Ƶ�����,��֤ͣ--��������Ӧ, zgc, 2008-04-22
					//StopSwitchToSubMt( ptDstBase[0].GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE, FALSE );
					TMt tMt = ptDstBase[0];
					StopSwitchToSubMt(1, &tMt, MODE_VIDEO, FALSE, FALSE, FALSE);
				}
				
				if(  MT_TYPE_MMCU != m_ptMtTable->GetMtType( ptDstBase[0].GetMtId() )  ||
						 ( ptDstBase[0].GetMtId() != tSrc.GetMtId() && bIsCanSwitchBrdToMMCU )						 	
					)
				{
					bRet = g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, ptDstBase,wSpyStartPort);

					//[2011/08/22/zhangli]�ָ����㲥PRS��RTCP����
					TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
					if (tConfAttrb.IsResendLosePack())
					{
						u8 byPrsId = 0;
						u8 byPrsChlId = 0;
						if (FindPrsChnForSrc(tUnLocalMt, bySrcChnnl, MODE_VIDEO, byPrsId, byPrsChlId))
						{
							AddRemovePrsMember(ptDstBase[0].GetMtId(), byPrsId, byPrsChlId, MODE_VIDEO);
						}
					}
				}
            }
            else
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[StartSwitchFromBrd] Mt.%d is in Rollcall, no accept from brd(DstNum=1)\n", ptDstBase[0].GetMtId() );
                bRet = TRUE;              
            }
		}
		else
		{
            if (!( ( m_ptMtTable->GetMtType(ptDstBase[0].GetMtId()) == MT_TYPE_SMCU || !(tSrc == ptDstBase[0] ) ) && IsNeedAdapt(tSrc, ptDstBase[0], MODE_VIDEO) && 
				StartSwitchToSubMtFromAdp(ptDstBase[0].GetMtId(), MODE_VIDEO)))
			{
				//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
				//bRet = StartSwitchToSubMt( tSrc, bySrcChnnl, ptDstBase[0].GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE,FALSE,wSpyStartPort );
				TSwitchGrp tSwitchGrp;
				tSwitchGrp.SetSrcChnl(bySrcChnnl);
				tSwitchGrp.SetDstMtNum(1);
				TMt tMt = ptDstBase[0];
				tSwitchGrp.SetDstMt(&tMt);
				StartSwitchToAll(tSrc, 1, &tSwitchGrp, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE, wSpyStartPort);

				//[2013/02/20]Bug00127469 ������ϼ�MCU������Ҫ����RTCP����
				if ( !m_tCascadeMMCU.IsNull() && ptDstBase[0] == m_tCascadeMMCU && tSrc == ptDstBase[0] )
				{
					u16 wMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
					ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
					if  ( ptConfMcInfo != NULL && 
						( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
					{
						if (!( ptConfMcInfo->m_tSpyMt.GetMtId() == tSrc.GetMtId() &&
							ptConfMcInfo->m_tSpyMt.GetMcuId() == tSrc.GetMcuId() ) )
						{
							return bRet;				
						}
					}
				}

				//[2011/08/22/zhangli]�ָ����㲥PRS��RTCP����
				TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
				if (tConfAttrb.IsResendLosePack())
				{
					u8 byPrsId = 0;
					u8 byPrsChlId = 0;
					if (FindPrsChnForSrc(tUnLocalMt, bySrcChnnl, MODE_VIDEO, byPrsId, byPrsChlId))
					{
						AddRemovePrsMember(ptDstBase[0].GetMtId(), byPrsId, byPrsChlId, MODE_VIDEO);
					}
				}
			}
			return bRet;
		}
	}
    else if( byDstMtNum > 1 )
    {
		TMt atDstMt[MAXNUM_CONF_MT];
		memcpy( atDstMt, ptDstBase, (sizeof(TMt) * byDstMtNum) );

        //zbq[03/29/2008] ������յĹ㲥Ŀ�꣬���˵����˺ͱ���������VMP�����
        if ( tLocalVidBrdSrc == m_tVmpEqp &&
             ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
        {
            for( byLoop = 0; byLoop < byDstMtNum; byLoop ++ )
            {
                if ( ptDstBase[byLoop] == m_tRollCaller ||
                     ptDstBase[byLoop] == GetLocalMtFromOtherMcuMt(m_tRollCallee))
                {
					atDstMt[byLoop].SetNull();
                    continue;
                }
                else
                {
					if( !( MT_TYPE_MMCU != m_ptMtTable->GetMtType( ptDstBase[byLoop].GetMtId() )  ||
						( ptDstBase[byLoop].GetMtId() != tSrc.GetMtId() && bIsCanSwitchBrdToMMCU ))
						)
					{
						atDstMt[byLoop].SetNull();
                        continue;
						//bRet &= g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, 1, &ptDstBase[byLoop],wSpyStartPort);
					}
                }
            }
			bRet &= g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, atDstMt,wSpyStartPort);
        }
        else
        {
			for( byLoop = 0; byLoop < byDstMtNum; byLoop ++ )
            {
				if(  !(MT_TYPE_MMCU != m_ptMtTable->GetMtType( ptDstBase[byLoop].GetMtId() )  ||
						( ptDstBase[byLoop].GetMtId() != tSrc.GetMtId() && bIsCanSwitchBrdToMMCU ) )
						)
				{
					atDstMt[byLoop].SetNull();
                    continue;
					//bRet = g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, 1, &ptDstBase[byLoop],wSpyStartPort);
				}				
			}
			bRet = g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, atDstMt,wSpyStartPort);
        }
    }
	else
	{
		return FALSE;
	}

    //zbq[04/15/2009] Tand�Ĺؼ�֡��ıȽϿ죬�Ҽ��Ϊ3s. �ʽ����������ٷ�youareseeing�������õ���һ���ؼ�֡
    if (IsDelayVidBrdVCU() && tSrc == tLocalVidBrdSrc)
    {
    }
    else
    {
        //guzh [2008/03/31] ����ؼ�֡
        if (TYPE_MT == tUnLocalMt.GetType())
        {		
			//����Ƿ���������ؼ�֡������Timer,�����������[7/17/2012 chendaiwei]
			if(m_tConf.GetSpeaker().IsLocal() && m_tConf.GetSpeaker().GetMtId() == tUnLocalMt.GetMtId())
			{
				NotifyFastUpdate(tUnLocalMt, MODE_VIDEO, TRUE);
			}
			else
			{
				NotifyFastUpdate(tUnLocalMt, MODE_VIDEO);
			}
		}
    }

    CServMsg cServMsg;
    TPeriEqpStatus tEqpStatus;
    //TMtStatus tMtStatus;
    for (byLoop = 0; byLoop < byDstMtNum; byLoop ++)
    {
        // zbq [06/19/2007] �����˺ͱ������˲��ӹ㲥����
        if ( !( tLocalVidBrdSrc == m_tVmpEqp && 
                ( ptDstBase[byLoop] == m_tRollCaller ||
                  ptDstBase[byLoop] == GetLocalMtFromOtherMcuMt(m_tRollCallee)) ) )
        {
            NotifyMtReceive( tSrc, ptDstBase[byLoop].GetMtId() );

            //�����ն˵�������ʾ��ʶ        
            if( tSrc == m_tPlayEqp )
            {
                g_cMcuVcApp.GetPeriEqpStatus( m_tPlayEqp.GetEqpId(), &tEqpStatus );
                cServMsg.SetMsgBody( (u8*)tEqpStatus.GetAlias(), MAXLEN_EQP_ALIAS );
                SendMsgToMt( ptDstBase[byLoop].GetMtId(), MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
            }
            //�����VMP,�����������ʾ��ʶ
            else if( tSrc == m_tVmpEqp )
            {
                u8 abyVmpAlias[8];
                abyVmpAlias[0] = ' ';
                abyVmpAlias[1] = '\0';
                cServMsg.SetMsgBody( abyVmpAlias, sizeof(abyVmpAlias) );
                SendMsgToMt( ptDstBase[byLoop].GetMtId(), MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
    	    }

            /* zgc, 2008-05-28, ���´����ں�����ʼ������
            // ���ѡ��״̬
            m_ptMtTable->GetMtStatus(ptDstBase[byLoop].GetMtId(), &tMtStatus);
            tMtStatus.RemoveSelByMcsMode( MODE_VIDEO );
            m_ptMtTable->SetMtStatus(ptDstBase[byLoop].GetMtId(), &tMtStatus);
            */

			if( MT_TYPE_MMCU != m_ptMtTable->GetMtType( ptDstBase[byLoop].GetMtId() )  ||
					( ptDstBase[byLoop].GetMtId() != tSrc.GetMtId() && bIsCanSwitchBrdToMMCU )
						)
			{
				TMt tRealMt;
				tRealMt.SetNull();
				//lukunpeng 2010/07/01 ��ش�Ҫ����������Դ
				m_ptMtTable->SetMtSrc( ptDstBase[byLoop].GetMtId(), &tUnLocalMt, MODE_VIDEO );
			
			}
        }
        else
        {
            // do nothing
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[StartSwitchFromBrd] Mt.%d is in Rollcall, no accept from brd\n", ptDstBase[byLoop].GetMtId() );
        }
    }
    
    MtStatusChange(NULL, TRUE);
    
    return bRet;
}
/*====================================================================
    ������      StartSwitchToAllNeedAdpWhenChangVid
    ����        ���л���Ƶ�㲥Դʱ�������䵽��Ҫ������ն���Ƶ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����         
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/03/20    4.0         �ܼ���        ����
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToAllNeedAdpWhenSrcChanged(TMt tSrc, u8 byDstNum, TMt *ptDstMt, u8 byMode)
{
	if (tSrc.IsNull() || 0 == byDstNum || NULL == ptDstMt)
	{
		return FALSE;
	}

	if (MODE_VIDEO != byMode && MODE_AUDIO != byMode)
	{
		return FALSE;
	}
	TMt tLocalSrc = GetLocalMtFromOtherMcuMt(tSrc);
	TMt tLoopMt;
	u8 byEqpNum = 0;
	TEqp atBas[MAXNUM_PERIEQP];
	memset(atBas, 0, sizeof(atBas));

	//��¼���齻��
	u8 byGrpNum = 0;
	//��Ӧĳ��bas����ĳһ������ն�tmt��Ϣ
	TMt atDstMt[MAXNUM_BASOUTCHN][MAXNUM_CONF_MT];
	//��¼Դbas
	TEqp tTempBas;

	if (GetDiffBrdEqp(byMode, byEqpNum, atBas))
	{
		for (u8 byIdx = 0; byIdx < byEqpNum; byIdx++)
		{
			u8 byChnNum = g_cMcuVcApp.GetBasInPutChnNum(atBas[byIdx]);

			for (u8 byChnId = 0; byChnId < byChnNum; byChnId++)
			{
				//[2011/07/05/zhangli]bas����֮���ٲ���
				if (g_cMcuVcApp.GetBasChnStatus(atBas[byIdx], byChnId) != BASCHN_STATE_RUNNING)
				{
					continue;
				}

				u8 byOutNum = 0;
				u8 byFrontOutNum = 0;
				if ( !GetBasChnOutputNum(atBas[byIdx],byChnId,byOutNum,byFrontOutNum) )
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[StartSwitchToAllNeedAdpWhenSrcChanged] GetBasChnOutputNum:%d-%d failed!\n",
							 atBas[byIdx].GetEqpId(),byChnId
							 );
					return FALSE;
				}

				byGrpNum = 0;	
				tTempBas.SetNull();	
				TSwitchGrp atSwitchGrp[MAXNUM_PERIEQP];
			    memset(atDstMt, 0, sizeof(atDstMt));

				for (u8 byOutId = 0; byOutId < byOutNum; byOutId++)
				{
					//ĳһbas�����Ч���ն���
				    u8 byVailedNum = 0;

					for (u8 byLoop = 0; byLoop < byDstNum; byLoop++)
					{ 
						tLoopMt = GetLocalMtFromOtherMcuMt(ptDstMt[byLoop]);

						if (!m_tConfAllMtInfo.MtJoinedConf(tLoopMt.GetMtId()))
						{
							continue;
						}

						if (ptDstMt[byLoop] == GetVidBrdSrc())
						{
							continue;
						}

						TBasOutInfo tOutInfo;
 						if (IsNeedAdapt(tLocalSrc, tLoopMt, byMode) &&
							FindBasChn2BrdForMt(tLoopMt.GetMtId(), byMode, tOutInfo) &&
							tOutInfo.m_tBasEqp.GetEqpId() == atBas[byIdx].GetEqpId() &&
							tOutInfo.m_byChnId == byChnId && 
							 tOutInfo.m_byOutIdx== byOutId)
 						{
							if (tTempBas.IsNull())
							{
								tTempBas = tOutInfo.m_tBasEqp;
								tTempBas.SetConfIdx(m_byConfIdx);
							}

							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[StartSwitchToAllNeedAdpWhenSrcChanged] Src<BasId.%d, ChnId.%d, OutId.%d>->DstMt.%d!\n",
													    atBas[byIdx].GetEqpId(),
													    byChnId, byOutId, 
													    tLoopMt.GetMtId());

							atDstMt[byOutId][byVailedNum] = tLoopMt;
							byVailedNum++;
						}									
					}
					//ĳһ����ͨ����ĳһ�����Ŀ���ն�����Ϊ0
					if (byVailedNum != 0)
					{
						for (u8 byGrpIdx = 0; byGrpIdx < MAXNUM_PERIEQP; byGrpIdx++)
						{
							if (atSwitchGrp[byGrpIdx].IsNull())
							{
								byGrpNum ++;
								atSwitchGrp[byGrpIdx].SetSrcChnl(byFrontOutNum + byOutId);
								atSwitchGrp[byGrpIdx].SetDstMtNum(byVailedNum);
								atSwitchGrp[byGrpIdx].SetDstMt(atDstMt[byOutId]);
								break;
							}
						}
					}
				}
				if (!tTempBas.IsNull() && byGrpNum > 0)
				{
					for (u8 byGrpIdx = 0; byGrpIdx < byGrpNum; byGrpIdx++)
					{
						for (u8 byDstIdx = 0; byDstIdx < atSwitchGrp[byGrpIdx].GetDstMtNum(); byDstIdx++)
						{
							TMt *ptMt = atSwitchGrp[byGrpIdx].GetDstMt();
							if (ptMt != NULL)
							{
								ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[StartSwitchToAllNeedAdpWhenChangVid] DstMtId:%d\n", 
											ptMt[byDstIdx].GetMtId());
							}			
						}
					}					
					StartSwitchToAll(tTempBas, byGrpNum, atSwitchGrp, byMode);
				}
			}
		}
	}
	return TRUE;
}
/*====================================================================
    ������      ��StartSwitchToAllSubMtJoinedConf
    ����        ����ָ���ն�����Ƶ�㲥���������������ֱ���ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt & tSrc, Դ
				  u8 bySrcChnnl, Դ���ŵ���
                  const TMt &tOldSrc, ԭ���ķ�����(��δʹ��)
                  BOOL32 bForce, �Ƿ�ǿ�������л�                  
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/03/20    4.0         ����        ����
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToAllSubMtJoinedConf(const TMt &tSrc, u8 bySrcChnnl, const TMt &tOldSrc, BOOL32 bForce, u16 wSpyStartPort )
{

	TMt tLocalSrc = GetLocalMtFromOtherMcuMt(tSrc);

    // ��ӹ㲥Դ
    if ( TYPE_MT == tLocalSrc.GetType() )
    {
        if ( !m_ptMtTable->IsLogicChnnlOpen(tLocalSrc.GetMtId(), LOGCHL_VIDEO, FALSE) )
        {
            ConfPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "[StartSwitchToAllSubMtJoinedConf]Source MT.%d is not sending video! tOldSrc<McuId.%u MtId.%u>\n",tLocalSrc.GetMtId(),tOldSrc.GetMcuId(),tOldSrc.GetMtId());
            return FALSE;
        }
    }

    
    TMtStatus tStatus;


	//�����佻�����
	TMt atMtListNeedAdp[MAXNUM_CONF_MT];
	u8 byNeedAdpDstMtNum = 0;
	
	//�������佻�����
    TMt atMtList[MAXNUM_CONF_MT];
    u8 byDstMtNum = 0;

    u8 byLoop = 1;
    TMt tLoopMt;

    for ( ; byLoop <= MAXNUM_CONF_MT; byLoop++ )
    {

		// ��ϯ���ȼ���ߣ�ֻҪ����ϯѡ��Դ���ն˶���continue
		if (m_tConf.GetChairman().GetMtId() == byLoop)
		{
			if( m_ptMtTable->GetMtStatus(byLoop,&tStatus) &&
				!tStatus.GetSelectMt(MODE_VIDEO).IsNull() )
			{
				continue;
			}
			
		}
        //����Դ�ն��Լ�
		tLoopMt = m_ptMtTable->GetMt(byLoop);
        if ( !m_tConfAllMtInfo.MtJoinedConf(byLoop) || 
             ( tSrc == tLoopMt && 
               tSrc.GetType() == TYPE_MT ) )
        {
			//�����Լ���ʱ��Ҫ֪ͨ�ն˿��Լ����������鲥��ַ���Լ�������
			if ( tSrc == tLoopMt
				&& tSrc.GetType() == TYPE_MT )
			{
				if ( IsMultiCastMt(byLoop) )
				{
					ChangeSatDConfMtRcvAddr(byLoop, MODE_VIDEO, FALSE);
				}
				NotifyMtReceive(tSrc, byLoop);
			}
            continue;
        }

		// ����VCS����,��ϯ��ѯʱ,��ϯ�����㲥Դ
		if (VCS_CONF == m_tConf.GetConfSource() &&
			m_tConf.GetChairman().GetMtId() == byLoop &&
			VCS_POLL_START == m_cVCSConfStatus.GetChairPollState())
		{
			continue;
		}

	

        //[2013/01/22 chenbing] �����նˣ���������
		//pgf�·��������ܽ��յ������նˣ�Ҫ�������ܽ��յģ�����ͨ�ն�һ��������
		if ( IsMultiCastMt(byLoop) && !IsCanSatMtReceiveBrdSrc(tLoopMt))
		{
			ChangeSatDConfMtRcvAddr(byLoop, MODE_VIDEO, FALSE);
			NotifyMtReceive(tLoopMt, byLoop);
			continue;
		}

		// [1/20/2010 xliang] �ϴ���ѯ����
		//zhouyiliang 20120328 �ϼ�mcu����
		if( MT_TYPE_MMCU == tLoopMt.GetMtType() && m_tCascadeMMCU == tLoopMt )
		{
		
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[StartSwitchToAllSubMtJoinedConf] brd to MMCU should be skipped !\n");
			continue; 
			
		}

		// vrs��¼������
		if (m_ptMtTable->GetMtType(byLoop) == MT_TYPE_VRSREC)
		{
			continue;
		}

        if ( !CanMtRecvVideo( tLoopMt, tLocalSrc ) )
        {			
			//[nizhijun 2010/11/19] ������Ҫ�ͷ�local�Ĺ㲥Դ�Ƚ�
			if(!(tLoopMt == GetVidBrdSrc()))
			{
				//zbq[01/01/2009] ����������Ե���������������������������������Զ�������
				if (IsNeedAdapt(tLocalSrc, tLoopMt, MODE_VIDEO)/*!(IsHDConf(m_tConf) && m_cMtRcvGrp.IsMtNeedAdp(tLoopMt.GetMtId()))*/)
				{
					atMtListNeedAdp[byNeedAdpDstMtNum] = tLoopMt;
					byNeedAdpDstMtNum ++;
				//	StartSwitchToSubMt( tSrc, bySrcChnnl, byLoop, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE, FALSE);
				}
			}
			// [10/21/2010 xliang] ��㲥Դ�����ն˵Ľ�������֪ͨ���ն˿��Լ�
			/* eg:
			Mt1,Mt2 in conf
			poll Mt1:
			Mt1--->Brd
			Brd--->Mt2
			poll Mt2:
			Mt1-X->Brd
			Mt2--->Brd
			Brd--->Mt1
			Brd-X->Mt2    this switch is removed by the following codes
			*/
			else if(CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode()
				&& MODE_VIDEO == m_tConf.m_tStatus.GetPollInfo()->GetMediaMode()
				)
			{		
				NotifyMtReceive(tLoopMt, tLoopMt.GetMtId());
				//zjl 20110510 StopSwitchToSubMt �ӿ������滻
				//StopSwitchToSubMt( tLoopMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE );
				StopSwitchToSubMt(1, &tLoopMt, MODE_VIDEO, FALSE);	
			}            
			
			continue;
		}
        
        atMtList[byDstMtNum] = tLoopMt;
        byDstMtNum ++;
    }
	
	if (byNeedAdpDstMtNum > 0)
	{
		StartSwitchToAllNeedAdpWhenSrcChanged(tSrc, byNeedAdpDstMtNum, atMtListNeedAdp, MODE_VIDEO);
	}
	
	//[nizhijun 2010/10/22] ��rtcp����
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (tConfAttrb.IsResendLosePack())
	{
		StartBasPrsSupportEx(MODE_VIDEO);
	}
    //��������
    
    //���岿��
    if ( byDstMtNum > 0)
    {		
        StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, atMtList, wSpyStartPort);
    }
    
    //��������������������������ݻ������Խ�����Ӧ�ش�����
	ChangeSpeakerSrc( MODE_VIDEO, emReasonChangeBrdSrc,wSpyStartPort );	 
    MtStatusChange();
    
	return TRUE;
//	TMt tLocalSrc = GetLocalMtFromOtherMcuMt(tSrc);
//
//    // ��ӹ㲥Դ
//    if ( TYPE_MT == tLocalSrc.GetType() )
//    {
//        if ( !m_ptMtTable->IsLogicChnnlOpen(tLocalSrc.GetMtId(), LOGCHL_VIDEO, FALSE) )
//        {
//            ConfPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "[StartSwitchToAllSubMtJoinedConf]Source MT.%d is not sending video! tOldSrc<McuId.%u MtId.%u>\n",tLocalSrc.GetMtId(),tOldSrc.GetMcuId(),tOldSrc.GetMtId());
//            return FALSE;
//        }
//    }
//
//	// [2013/01/22 chenbing] ���Ƿ�ɢ����
//	if ( m_tConf.GetConfAttrb().IsSatDCastMode())
//	{
//		//�鲥�����¹���:ͬʱҲ�й㲥Դ�����ǵ�ַ�Ľ���
//		g_cMpManager.StartDistrConfCast(tSrc, MODE_VIDEO, bySrcChnnl);
//    }
//
//    g_cMpManager.StartSwitchToBrd(tLocalSrc, bySrcChnnl, bForce, wSpyStartPort);
//    
//    TMtStatus tStatus;
//
//	//��vmp�㲥�������
//	TMt atOldVmpDstMt[MAXNUM_CONF_MT];
//	u8  byOldVmpDstMtNum = 0;
//
//	//�����佻�����
//	TMt atMtListNeedAdp[MAXNUM_CONF_MT];
//	u8 byNeedAdpDstMtNum = 0;
//	
//	//�������佻�����
//    TMt atMtList[MAXNUM_CONF_MT];
//    u8 byDstMtNum = 0;
//
//    u8 byLoop = 1;
//    TMt tLoopMt;
//
//    for ( ; byLoop <= MAXNUM_CONF_MT; byLoop++ )
//    {
//
//		// ��ϯ���ȼ���ߣ�ֻҪ����ϯѡ��Դ���ն˶���continue
//		if (m_tConf.GetChairman().GetMtId() == byLoop)
//		{
//			if( m_ptMtTable->GetMtStatus(byLoop,&tStatus) &&
//				!tStatus.GetSelectMt(MODE_VIDEO).IsNull() )
//			{
//				continue;
//			}
//			
//			// ��ϯѡ������ϳ�ʱ,�����㲥Դ
//			if (IsVmpSeeByChairman())
//			{
//				continue;
//			}
//		}
//        //����Դ�ն��Լ�
//		tLoopMt = m_ptMtTable->GetMt(byLoop);
//        if ( !m_tConfAllMtInfo.MtJoinedConf(byLoop) || 
//             ( tSrc == tLoopMt && 
//               tSrc.GetType() == TYPE_MT ) )
//        {
//			//�����Լ���ʱ��Ҫ֪ͨ�ն˿��Լ����������鲥��ַ���Լ�������
//			if ( tSrc == tLoopMt
//				&& tSrc.GetType() == TYPE_MT )
//			{
//				if ( IsMultiCastMt(byLoop) )
//				{
//					ChangeSatDConfMtRcvAddr(byLoop, MODE_VIDEO, FALSE);
//				}
//				NotifyMtReceive(tSrc, byLoop);
//			}
//            continue;
//        }
//
//		// ����VCS����,��ϯ��ѯʱ,��ϯ�����㲥Դ
//		if (VCS_CONF == m_tConf.GetConfSource() &&
//			m_tConf.GetChairman().GetMtId() == byLoop &&
//			VCS_POLL_START == m_cVCSConfStatus.GetChairPollState())
//		{
//			continue;
//		}
//
//	
//
//        //[2013/01/22 chenbing] �����նˣ���������
//		//pgf�·��������ܽ��յ������նˣ�Ҫ�������ܽ��յģ�����ͨ�ն�һ��������
//		if ( IsMultiCastMt(byLoop) && !IsCanSatMtReceiveBrdSrc(tLoopMt))
//		{
//			ChangeSatDConfMtRcvAddr(byLoop, MODE_VIDEO, FALSE);
//			NotifyMtReceive(tLoopMt, byLoop);
//			continue;
//		}
//
//		// [1/20/2010 xliang] �ϴ���ѯ����
//		//zhouyiliang 20120328 �ϼ�mcu����
//		if( MT_TYPE_MMCU == tLoopMt.GetMtType() && m_tCascadeMMCU == tLoopMt )
//		{
//		
//			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[StartSwitchToAllSubMtJoinedConf] brd to MMCU should be skipped !\n");
//			continue; 
//			
//		}
//
//		
//
//        if ( !CanMtRecvVideo( tLoopMt, tLocalSrc ) )
//        {
//			// [10/21/2010 xliang] old vmp��2·�Ľ���
//			TSimCapSet tSimCapSet = m_ptMtTable->GetDstSCS( byLoop );
//			if( MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() 
//				&& ( tLocalSrc == m_tVmpEqp && VMP == GetVmpSubType(m_tVmpEqp.GetEqpId()) )
//				&& (tSimCapSet.GetVideoMediaType() == m_tConf.GetSecVideoMediaType() )
//				)
//			{
//				//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
//				//StartSwitchToSubMt(tSrc, 1, byLoop, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE);
//				//g_cMpManager.StartSwitchToSubMt(tSrc, 1, tLoopMt, MODE_VIDEO);
//				atOldVmpDstMt[byOldVmpDstMtNum] = tLoopMt;
//				byOldVmpDstMtNum++;
//			}
//			else 
//			{
//				//[nizhijun 2010/11/19] ������Ҫ�ͷ�local�Ĺ㲥Դ�Ƚ�
//				if(!(tLoopMt == GetVidBrdSrc()))
//				{
//					//zbq[01/01/2009] ����������Ե���������������������������������Զ�������
//					if (IsNeedAdapt(tLocalSrc, tLoopMt, MODE_VIDEO)/*!(IsHDConf(m_tConf) && m_cMtRcvGrp.IsMtNeedAdp(tLoopMt.GetMtId()))*/)
//					{
//						atMtListNeedAdp[byNeedAdpDstMtNum] = tLoopMt;
//						byNeedAdpDstMtNum ++;
//					//	StartSwitchToSubMt( tSrc, bySrcChnnl, byLoop, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE, FALSE);
//					}
//				}
//				// [10/21/2010 xliang] ��㲥Դ�����ն˵Ľ�������֪ͨ���ն˿��Լ�
//				/* eg:
//				Mt1,Mt2 in conf
//				poll Mt1:
//				Mt1--->Brd
//				Brd--->Mt2
//				poll Mt2:
//				Mt1-X->Brd
//				Mt2--->Brd
//				Brd--->Mt1
//				Brd-X->Mt2    this switch is removed by the following codes
//				*/
//				else if(CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode()
//					&& MODE_VIDEO == m_tConf.m_tStatus.GetPollInfo()->GetMediaMode()
//					)
//				{		
//					NotifyMtReceive(tLoopMt, tLoopMt.GetMtId());
//					//zjl 20110510 StopSwitchToSubMt �ӿ������滻
//					//StopSwitchToSubMt( tLoopMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE );
//					StopSwitchToSubMt(1, &tLoopMt, MODE_VIDEO, FALSE);
//
//					
//					
//				}
//			}
//            
//            continue;
//        }
//        
//        atMtList[byDstMtNum] = tLoopMt;
//        byDstMtNum ++;
//    }
//	
//	if (byOldVmpDstMtNum > 0)
//	{
//		TSwitchGrp tSwitchGrp;
//		tSwitchGrp.SetSrcChnl(1);
//		tSwitchGrp.SetDstMtNum(byOldVmpDstMtNum);
//		tSwitchGrp.SetDstMt(atOldVmpDstMt);
//		StartSwitchToAll(tSrc, 1, &tSwitchGrp, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE);
//	}
//	
//	if (byNeedAdpDstMtNum > 0)
//	{
//		StartSwitchToAllNeedAdpWhenSrcChanged(tSrc, byNeedAdpDstMtNum, atMtListNeedAdp, MODE_VIDEO);
//	}
//	
//	//[nizhijun 2010/10/22] ��rtcp����
//	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
//	if (tConfAttrb.IsResendLosePack())
//	{
//		StartBasPrsSupportEx(MODE_VIDEO);
//	}
//    //��������
//    
//    //���岿��
//    if ( byDstMtNum > 0)
//    {		
//        StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, atMtList, wSpyStartPort);
//    }
//    
//    //��������������������������ݻ������Խ�����Ӧ�ش�����
//	ChangeSpeakerSrc( MODE_VIDEO, emReasonChangeBrdSrc,wSpyStartPort );	 
//    MtStatusChange();
//    
//	return TRUE;
}

/*====================================================================
    ������      ��StartSwitchToAllSubMtJoinedConf
    ����        ����ָ���ն���Ƶ�������������������ֱ���ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt & tSrc, Դ
				  u8 bySrcChnnl, Դ���ŵ���
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/12    1.0         LI Yi         ����
	03/07/23    2.0         ������        �޸�
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToAllSubMtJoinedConf(const TMt &tSrc, u8 bySrcChnnl, u16 wSpyStartPort)
{
	u8	byLoop;
    BOOL32 bResultAnd = TRUE;
	TMt tTempSrc;
	//u8  tTempSrcChn;
	//[nizhijun 2010/12/04] ��������ʹ����ʱ��SypStartPort
	u16 tTempSypStartPort;

	u8 byGlobalSpeakerId = 0;
	if (!m_tCascadeMMCU.IsNull() && tSrc == m_tConf.GetSpeaker() &&
					m_tCascadeMMCU == tSrc )
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
		
		if( ptConfMcInfo != NULL						
			&& ptConfMcInfo->m_tConfAudioInfo.m_byMixerCount == 1
			&& ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.IsMcuIdLocal()
			)
		{			
			byGlobalSpeakerId = ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId();
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[StartSwitchToAllSubMtJoinedConf] Mt(%d) is GlobalSpeaker,so not switch audio to it.\n",byGlobalSpeakerId );
		}
	}

	//����ͬ����������
	TMt atVADstMt[MAXNUM_CONF_MT];
	TSwitchGrp tSwitchGrpVA;
	u8 byVADstNum = 0;
	TEqp tVABas;
	u8 byVAChnId  = 0;
	u8 byVAOutNum = 0;
	u8 byVAOutIdx = 0;
	BOOL32 bVABasOut = TRUE;
	if(g_cMcuVcApp.IsVASimultaneous() &&
		GetAudBufOutPutInfo(tVABas, byVAChnId, byVAOutNum, byVAOutIdx))
	{
		tVABas.SetConfIdx(m_byConfIdx);
		tSwitchGrpVA.SetSrcChnl(byVAChnId * byVAOutNum);
	}
	else
	{
		bVABasOut = FALSE;
	}

	//��Ƶ���佻������
	TMt atDstMtNeedAudAdp[MAXNUM_CONF_MT];
	u8 byNeedAudAdpDstNum = 0;
	
	//��ͨ��������
	TSwitchGrp tSwitchGrp;
	tSwitchGrp.SetSrcChnl( bySrcChnnl );
	
	u8 byDstMtNum = 0;
	TMt atDstMt[MAXNUM_CONF_MT];
	memset( atDstMt, 0, sizeof(atDstMt) );

	TMtStatus tMtStatus;

    for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
    {
		tTempSypStartPort = wSpyStartPort;
		tTempSrc    = tSrc;
	
		if( byGlobalSpeakerId == byLoop )
		{
			continue;
		}
        if( m_tConfAllMtInfo.MtJoinedConf( byLoop ))
        {
			if (m_ptMtTable->GetMtType(byLoop) == MT_TYPE_VRSREC)
			{
				//����vrs��¼��ʵ��
				continue;
			}

			if( m_ptMtTable->GetMtStatus(byLoop,&tMtStatus) &&
				!tMtStatus.GetSelectMt(MODE_AUDIO).IsNull() )
			{
				continue;
			}

//             //���Ƿ�ɢ�����鲥�ն����鲥��ַ������������������
//pgf:�����յ����������յĽ�����
			if ( IsMultiCastMt(byLoop) && !IsCanSatMtReceiveBrdSrc(m_ptMtTable->GetMt(byLoop), MODE_AUDIO))
			{
				continue;
			}


			// ����������Դ
			if ( byLoop == GetLocalSpeaker().GetMtId() && 
				m_ptMtTable->GetMtType( GetLocalSpeaker().GetMtId() ) == MT_TYPE_MT 
				)
			{
				//�·���������Դ�����߼�, zgc, 2008-04-12
				ChangeSpeakerSrc( MODE_AUDIO, emReasonChangeBrdSrc,tTempSypStartPort );
			}
            else
            {            	
				if(IsNeedAdapt(GetLocalAudBrdSrc(), m_ptMtTable->GetMt(byLoop), MODE_AUDIO))
				{
					atDstMtNeedAudAdp[byNeedAudAdpDstNum] = m_ptMtTable->GetMt(byLoop);
					byNeedAudAdpDstNum++;
				}	
				else
				{
				//[03/01/2010] zjl modify 8000e ��֧�ִ���ͬ������Ƶ��������bas��ֱ�ӽ�����Ŀ���ն�               
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
					//zjl20101028����ͬ���������ļ�,���Һͷ����˰�
					if(g_cMcuVcApp.IsVASimultaneous() && !m_tConf.GetSpeaker().IsNull()
						&& tSrc == GetVidBrdSrc() )	// 2011-8-25 add by pgf:��ӹ���������������ƵͬԴ����������ͬ������������ͨ����					
					{
						//Ŀǰֻ��mpu��mpu_h֧�ִ���ͬ���������ƵҪ���䲢�ҷ�mpu��mpu_h��ͬʱ��ƵҲ����Ҫ���䣬
						//��ֱ�ӽ�����Ƶ������Ŀ���ն�
						TLogicalChannel tVidChn;
						TPeriEqpStatus tBasStatus;
						TBasOutInfo	tOutInfo;
						BOOL32 bNeedVidAdp =  (m_ptMtTable->GetMtLogicChnnl(byLoop, LOGCHL_VIDEO, &tVidChn, TRUE) &&
							IsNeedAdapt(m_tConf.GetSpeaker(), m_ptMtTable->GetMt(byLoop), MODE_VIDEO) &&
							FindBasChn2BrdForMt(byLoop, MODE_VIDEO, tOutInfo) &&
							g_cMcuVcApp.GetPeriEqpStatus(tOutInfo.m_tBasEqp.GetEqpId(), &tBasStatus) &&
							( g_cMcuAgent.IsEqpBasHD( tOutInfo.m_tBasEqp.GetEqpId() ) ) &&
							(tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU ||
							tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU_H ||
							tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU2_BASIC ||
							tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU2_ENHANCED)
							);
						
						//ͳ����Ҫ����ͬ����Ŀ���ն�
						//1.��Ƶ������
						//2.�ҵ���Ƶ�������(û�ҵ�����Ƶ����ֱ�ӽ���)
						if (bNeedVidAdp && bVABasOut)
						{
							atVADstMt[byVADstNum] = m_ptMtTable->GetMt(byLoop);
							byVADstNum ++;
							continue;
						}
					}
#endif		 
					//ͳ�Ʋ������䣬��Ƶ����ֱ�ӽ�����Ŀ���ն�
					atDstMt[byDstMtNum] = m_ptMtTable->GetMt(byLoop);
					byDstMtNum++;
				}
			}
		}
	}

	//����ͬ���������
	if (byVADstNum > 0)
	{
		tSwitchGrpVA.SetDstMtNum(byVADstNum);
		tSwitchGrpVA.SetDstMt(atVADstMt);
		StartSwitchToAll( tVABas, 1, &tSwitchGrpVA, MODE_AUDIO);
	}	

// 	//���佻�����
// 	if (byNeedAudAdpDstNum > 0)
// 	{
// 		StartSwitchToAllNeedAdpWhenSrcChanged(tSrc, byNeedAudAdpDstNum, atDstMtNeedAudAdp, MODE_AUDIO);
// 		TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
// 		if (tConfAttrb.IsResendLosePack())
// 		{
// 			StartBasPrsSupportEx(MODE_AUDIO);
// 		}		
// 	}

	//��ͨ�������
	if( byDstMtNum > 0 )
	{
		tSwitchGrp.SetDstMtNum( byDstMtNum );
		tSwitchGrp.SetDstMt( atDstMt );
		StartSwitchToAll( tTempSrc, 1, &tSwitchGrp, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE, FALSE, tTempSypStartPort);
	}
    
    MtStatusChange();
    
	return bResultAnd;
}

/*====================================================================
    ������      StartSwitchToSubMtNeedAdp
    ����        �������ݽ����������� �ӱ�����ͨ���������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bIsConsiderBiteRate �Ƿ�ֻ������������
						Ĭ��ΪFALSE��˵��ֻҪ��Ҫ����ͽ�������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
  2010-06-30    4.6         �ܼ���		   ����
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId, BOOL32 bIsConsiderBiteRate /*= FALSE*/)
{

	u8 byMode = GetBasChnMode(tEqp, byChnId);
	if (MODE_NONE == byMode)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchToSubMtNeedAdpt] byMode is MODE_NONE!\n");
		return FALSE;
	}
	
	TMt tSrc = GetBasChnSrc(tEqp, byChnId);
	if (tSrc.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchToSubMtNeedAdpt] EqpId:%d + ChnId:%d 's Src is null!\n", tEqp.GetEqpId(), byChnId);
		return FALSE;
	}
	
	TBasOutInfo tOutInfo;
	TMt tDst;
	tDst.SetNull();

	//������Ч����
	u8 byGrpNum = 0;
	//������
	TSwitchGrp atSwitchGrp[MAXNUM_PERIEQP];
	memset(atSwitchGrp, 0, sizeof(atSwitchGrp));
	//ÿһ·�����������Ч�ն���
	u8 byVailedMtNum = 0;

	TMt atDstMt[MAXNUM_BASOUTCHN][MAXNUM_CONF_MT];
	memset(atDstMt, 0, sizeof(atDstMt));

	//�������·��
	u8 byChnOutNum = 0;
	u8 byFrontOutNum = 0;
	if ( !GetBasChnOutputNum(tEqp,byChnId,byChnOutNum,byFrontOutNum) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[StartSwitchToSubMtNeedAdpt] GetBasChnOutputNum:%d-%d failed!\n",
				  tEqp.GetEqpId(),byChnId
				  );
		return FALSE;
	}

	TMtStatus tMtStatus;
	TMt tSelSrc;
	for (u8 bySrcOutIdx = 0; bySrcOutIdx < byChnOutNum; bySrcOutIdx++)
	{
		//��Ӧ����ĳһ������ն�id
		byVailedMtNum = 0;

		for (u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
		{
			if (!m_tConfAllMtInfo.MtJoinedConf(byMtIdx))
			{
				continue;
			}
			//���������Լ�,��Ҫ�������¼�mcu
			if ( tSrc == m_ptMtTable->GetMt(byMtIdx) &&
				MT_TYPE_SMCU != m_ptMtTable->GetMtType(byMtIdx) )
			{
				continue;
			}
			
			//���������ϼ�mcu
			if (m_tCascadeMMCU.GetMtId() == byMtIdx)
			{
				continue;
			}

			// ���������ն�¼���vrs��¼��ʵ��
			if (m_ptMtTable->GetMtType(byMtIdx) == MT_TYPE_VRSREC)
			{
				if (m_ptMtTable->GetRecChlType(byMtIdx) == TRecChnnlStatus::TYPE_RECORD
					&& !m_ptMtTable->GetRecSrc(byMtIdx).IsNull())
				{
					continue;
				}
			}

			if (m_tConf.GetChairman().GetMtId() == byMtIdx)
			{
				m_ptMtTable->GetMtStatus(byMtIdx, &tMtStatus);
				tSelSrc = tMtStatus.GetSelectMt( byMode );
				// �²���,��ϯ��ѡ��Դʱ,�����㲥Դ
				if (!tSelSrc.IsNull())
				{
					continue;
				}				
			}

			// [2013/01/22 chenbing] �²��ԣ����Ƿ�ɢ�����£������ն�ֻ�����鲥��ַ������
			//�������䵽�����ն˵Ľ���
			if(	IsMultiCastMt(byMtIdx) )
			{
				continue;
			}

			tOutInfo.clear();
			tDst = m_ptMtTable->GetMt(byMtIdx);
			if (!IsNeedAdapt(tSrc, tDst, byMode))
			{
				continue;
			}

			//���ֻ�����������䣬��ô���������Ͳ�������
			//�ó���ʹ���ڣ�ͣ˫��ʱ�������������������佻��
			if ( bIsConsiderBiteRate )
			{
				TSimCapSet  tMVSrcCap;
				TSimCapSet  tMVDstCap;
				TMt tTempSrc;
				TMt tTempDst;
				tTempSrc = GetLocalMtFromOtherMcuMt(tSrc);
				tMVSrcCap = m_ptMtTable->GetSrcSCS(tTempSrc.GetMtId());
				tTempDst = GetLocalMtFromOtherMcuMt(tDst);
				tMVDstCap = m_ptMtTable->GetDstSCS(tTempDst.GetMtId());
				
				//������������������䣺��ʽ���ֱ��ʡ�֡�ʡ�profiletype ��ô˵����û��Ҫ����������
				//����˵����ֻ���������䣬��ô��Ҫ��������
				if ( tMVSrcCap.GetVideoMaxBitRate() <= tMVDstCap.GetVideoMaxBitRate() ||
					tMVSrcCap.GetVideoMediaType() != tMVDstCap.GetVideoMediaType() ||
					tMVSrcCap.GetVideoProfileType() != tMVDstCap.GetVideoProfileType() ||
					tMVDstCap.GetVideoCap().IsResLower( tMVDstCap.GetVideoResolution(), tMVSrcCap.GetVideoResolution() ) ||
					( tMVSrcCap.GetVideoMediaType() == MEDIA_TYPE_H264 && tMVDstCap.GetVideoMediaType() == MEDIA_TYPE_H264 &&
					tMVDstCap.GetVideoCap().IsH264CapLower( tMVSrcCap.GetVideoResolution(),tMVSrcCap.GetUserDefFrameRate() )
					)||
					( tMVSrcCap.GetVideoMediaType() != MEDIA_TYPE_H264 && tMVDstCap.GetVideoMediaType() != MEDIA_TYPE_H264 &&
					tMVSrcCap.GetVideoFrameRate() > 	tMVDstCap.GetVideoFrameRate()	
					)
					)		
				{
					continue;
				}	
				else
				{
					//�����¼�MCU�����Դ���¼���ͬʱĿ��Ҳ���¼��ĵĻ�����Ϊ�¼������������ʵĵ���������stopdoublestream�
					//����ͨ����flowcontrol�����ƣ��������������¼������¼������������������ôҲ��Ҫ����
					if ( tTempSrc == tTempDst &&
						tTempDst.GetMtType() == MT_TYPE_SMCU 
						)
					{
						continue;
					}
					ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_BAS,"[StartSwitchToSubMtNeedAdp]tMVSrcCap:%d-%d-%d-%d-%d,tMVSrcCap:%d-%d-%d-%d-%d\n",
						tMVSrcCap.GetVideoMediaType(),tMVSrcCap.GetVideoResolution(),tMVSrcCap.GetUserDefFrameRate(),tMVSrcCap.GetVideoProfileType(),tMVSrcCap.GetVideoMaxBitRate(),
						tMVDstCap.GetVideoMediaType(),tMVDstCap.GetVideoResolution(),tMVDstCap.GetUserDefFrameRate(),tMVDstCap.GetVideoProfileType(),tMVDstCap.GetVideoMaxBitRate()
						);
					ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_BAS,"[StartSwitchToSubMtNeedAdp] tSrc:%d-%d bIsConsiderBiteRate,tEqp:%d,chnid:%d build switch to tmt:%d-%d\n",
						tTempSrc.GetMcuId(),tTempSrc.GetMtId(),
						tEqp.GetEqpId(),byChnId,tTempDst.GetMcuId(),tTempDst.GetMtId()
						);
				}
			}

			if(!FindBasChn2BrdForMt(byMtIdx, byMode, tOutInfo))
			{
				continue;
			}
	
			//���C�����}�������Q
			if (tOutInfo.m_tBasEqp.GetEqpId() != tEqp.GetEqpId() 
				|| tOutInfo.m_byChnId != byChnId 
				|| tOutInfo.m_byOutIdx != bySrcOutIdx
				)
			{
				continue;
			}
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[StartSwitchToSubMtNeedAdp] Src<BasId.%d, ChnId.%d, OutId.%d>->DstMt.%d!\n",
										tEqp.GetEqpId(), byChnId, bySrcOutIdx, byMtIdx);

			atDstMt[bySrcOutIdx][byVailedMtNum] = tDst;
			byVailedMtNum ++;
		}
		if (byVailedMtNum > 0)
		{
			atSwitchGrp[byGrpNum].SetSrcChnl(byFrontOutNum + bySrcOutIdx);
			atSwitchGrp[byGrpNum].SetDstMtNum(byVailedMtNum);
			atSwitchGrp[byGrpNum].SetDstMt(atDstMt[bySrcOutIdx]);
			byGrpNum ++;	
		}	
	}
// 	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[StartSwitchToSubMtNeedAdpt] --------------GrpNum:%d--------------!\n", byGrpNum);
// 	for (u8 byGrpIdx = 0; byGrpIdx < byGrpNum; byGrpIdx++)
// 	{
// 		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[StartSwitchToSubMtNeedAdpt] Grp:%d, SrcChn:%d, DstNum:%d!\n",
// 					byGrpIdx, atSwitchGrp[byGrpIdx].GetSrcChnl(), atSwitchGrp[byGrpIdx].GetDstMtNum());
// 		for (u8 byDstIdx = 0; byDstIdx < atSwitchGrp[byGrpIdx].GetDstMtNum(); byDstIdx++)
// 		{
// 			TMt *ptMt = atSwitchGrp[byGrpIdx].GetDstMt();
// 			if (ptMt != NULL)
// 			{
// 				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[StartSwitchToSubMtNeedAdpt] DstMtId:%d\n", ptMt[byDstIdx].GetMtId());
// 			}			
// 		}
// 	}
	StartSwitchToAll(tEqp, byGrpNum, atSwitchGrp, byMode);
	//����ؼ�֡

	if ( byMode != MODE_AUDIO )
	{
		NotifyFastUpdate(tEqp, byChnId, TRUE);
	}
	return TRUE;
}

/*====================================================================
    ������          StopSwitchToSubMtNeedAdp
    ����        ��������ֹͣ������������ �ӱ�����ͨ����ĳ�ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010-6-30   4.6         �ܼ���		   ����
====================================================================*/
BOOL32 CMcuVcInst::StopSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId,BOOL32 bSelSelf /*= TRUE*/)
{

	u8 byMode = GetBasChnMode(tEqp, byChnId);
	//[nizhijun 2010/9/25] �ж��Ƿ�����Ƶ����
	TPeriEqpStatus tBasStatus;
	BOOL32 bStopAud =  ( g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tBasStatus) ) &&
					   ( g_cMcuAgent.IsEqpBasHD( tEqp.GetEqpId() ) ) &&
					   ( tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU ||
					     tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU_H ||
						 tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU2_BASIC ||
					     tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU2_ENHANCED 
						 )     &&
					   (byMode == MODE_VIDEO);
	if (MODE_NONE == byMode)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StopSwitchToSubMtNeedAdpt] byMode is MODE_NONE!\n");
		return FALSE;
	}
	
	TMt tSrc = GetBasChnSrc(tEqp, byChnId);

	if (tSrc.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StopSwitchToSubMtNeedAdpt] EqpId:%d + ChnId:%d 's Src is null!\n", tEqp.GetEqpId(), byChnId);
		return FALSE;
	}
	
	TBasOutInfo tOutInfo;
	TMt tDst;
	tDst.SetNull();
	TMt tDstSrc;
	tDstSrc.SetNull();

	TMt atDst[MAXNUM_CONF_MT];
	u8 byMtNum = 0;
	TMt atDstAud[MAXNUM_CONF_MT];
	u8 byAudMtNum = 0;

	for (u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
	{
		if (!m_tConfAllMtInfo.MtJoinedConf(byMtIdx))
		{
			continue;
		}

		//[2011/09/21/zhangli]������ϼ�MCU�������;Bug00065099,�ϴ��ͷ�������ͬʱ���ϴ������䣬ȡ�������ˣ��ϴ�ͨ�������
		if (byMtIdx == m_tCascadeMMCU.GetMtId())
		{
			continue;
 		}

		tOutInfo.clear();
		tDst = m_ptMtTable->GetMt(byMtIdx);

		//[nizhijun 2010/9/10]
// 		if(!(m_ptMtTable->GetMtStatus(byMtIdx, &tDstStatus) &&
// 		     tDstStatus.GetSelectMt(byMode) == tSrc))
// 		{
// 			continue;
// 		}

		// [pengjie 2011/3/3] Ӧ�ÿ����ն��ڽ���ѡ����û�н������������������
		if (byMode != MODE_SECVIDEO )
		{
			//Bug00104047 ѡ��Դ�ǿգ���ͣ����,�˴�����ʹ��GetMtSrc���ж�
			//��Ϊ����BAS���ߣ���Ҫ��һЩ��Ƶ�������
// 			TMt tMtSrc;
// 			m_ptMtTable->GetMtSrc( tDst.GetMtId(), &tMtSrc, byMode );
// 			if( !(tMtSrc == tSrc) )
// 			{
// 				continue;
// 			}
	
			TMtStatus tTempMtStatus;
			if ( m_ptMtTable->GetMtStatus(tDst.GetMtId(),&tTempMtStatus) && 
				 !tTempMtStatus.GetSelectMt(byMode).IsNull() 
				)
			{
				ConfPrint(LOG_LVL_DETAIL,MID_MCU_BAS,"[StopSwitchToSubMtNeedAdp]mt:%d-%d has select src:%d-%d\n",
						 tDst.GetMcuId(),tDst.GetMtId(), 
						 tTempMtStatus.GetSelectMt(byMode).GetMcuId(),tTempMtStatus.GetSelectMt(byMode).GetMtId()
						 );
				continue;
			}
			//Bug00104047 [end]

			TMt tMtSrc;
			m_ptMtTable->GetMtSrc( tDst.GetMtId(), &tMtSrc, byMode );
			// ��ϯ��ѯѡ��ʱ,����ѯ�ն�Ϊ��ǰ������,ȡ��������,��ͣbas����ϯ�Ľ���
			if (byMode == MODE_VIDEO && 
				!tMtSrc.IsNull() && 
				m_tConf.GetChairman() == tDst &&
				(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode() ||
				 CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()) &&
				 m_ptMtTable->GetMtStatus(tDst.GetMtId(),&tTempMtStatus) && tTempMtStatus.GetSelectMt(byMode) == tMtSrc 
				)
			{
				continue;
			}
		}		
		//End

		if(!IsNeedAdapt(tSrc, tDst, byMode))
		{
			continue;
		}
		if (!FindBasChn2BrdForMt(byMtIdx, byMode, tOutInfo))
		{
			continue;
		}

		if (tOutInfo.m_tBasEqp.GetEqpId() != tEqp.GetEqpId() || tOutInfo.m_byChnId != byChnId)
		{
			continue;
		}

		//����ϲ�
		if (MT_MANU_KDC != m_ptMtTable->GetManuId(byMtIdx) &&
			MT_MANU_KDCMCU != m_ptMtTable->GetManuId(byMtIdx) )
		{
			//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
			//��KEDA��ȡ���������ȡ�������Լ�
			//StartSwitchToSubMt(m_ptMtTable->GetMt(byMtIdx), 0, byMtIdx, MODE_VIDEO);
			TSwitchGrp tSwitchGrp;
			tSwitchGrp.SetSrcChnl(0);
			tSwitchGrp.SetDstMtNum(1); 
			tSwitchGrp.SetDstMt(&tDst);
			StartSwitchToAll(tDst, 1, &tSwitchGrp, MODE_VIDEO);
			continue;
		}
	
		//StopSwitchToSubMt(byMtIdx, byMode);	
		atDst[byMtNum] = tDst;
		byMtNum++;
		
	
		if (bStopAud)
		{
			TMtStatus   tMtStatus;	
			memset( &tMtStatus,0,sizeof( tMtStatus ) );
			m_ptMtTable->GetMtStatus( byMtIdx, &tMtStatus ) ;
			//zjj20091230����ն���ƵԴ�ǻ������������ﲻ�ܲ𽻻�
			if( tMtStatus.GetAudioMt().GetType() == TYPE_MCUPERI &&
				EQP_TYPE_MIXER == tMtStatus.GetAudioMt().GetEqpType() )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[StopSwitchToSubMtNeedAdpt] mt(%d)'s AudioSrc is mixer. can't stop audio switch\n", byMtIdx);                    
			}
			else
			{
				if(g_cMcuVcApp.IsVASimultaneous()&&
					!IsNeedAdapt(tSrc, tDst, MODE_AUDIO)
				  )
				{
					//StopSwitchToSubMt(byMtIdx, MODE_AUDIO);
					atDstAud[byAudMtNum] = tDst;
					byAudMtNum++;
				}				
			}
		}
	}

	StopSwitchToSubMt( byMtNum, atDst, byMode, TRUE ,TRUE,bSelSelf);
	
	if( byAudMtNum > 0 )
	{
		StopSwitchToSubMt( byAudMtNum, atDstAud, MODE_AUDIO, TRUE );
	}

	return TRUE;
}

/*====================================================================
    ������      ��StartSwitchAud2MtNeedAdp
    ����        ��zjj20090925 ��������ͬ��״̬��bas�������ն˵���Ƶ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/09/25    4.6         �ܾ���          ����
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchAud3MtNeedAdp( u8 bySwitchMode )
{
	u8 byLoop = 0;
	BOOL32 bNeedAdp = FALSE;
	TLogicalChannel tVidChn;

	BOOL32 bRet = TRUE;

	TMt tLocalAudBrdSrc = GetLocalAudBrdSrc();

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS,  "[StartSwitchAud3MtNeedAdp] Start Vmp To Mt Audio Switch. bySwitchMode.%u\n",bySwitchMode );

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)	
	//8ke ��֧�ִ���ͬ����ֱ�Ӵӹ㲥Դ������Ƶ
 	return TRUE;
#endif
	
	if (!g_cMcuVcApp.IsVASimultaneous())
	{
		return FALSE;
	}
//////////////zjj20090925
	for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ))
		{       

			//���������Լ�
			if (tLocalAudBrdSrc.GetMtType() == MT_TYPE_MT &&
				tLocalAudBrdSrc.GetMtId() == byLoop)
			{
				continue;
			}
			//���������ϼ�MCU
			if (m_tCascadeMMCU.GetMtId() == byLoop )
			{
				continue;
			}			
			
            //������Ҫ��Ƶ������նˣ���Ƶͬ����Ҫ���л��崦���Ա�֤����ͬ��					
			bNeedAdp = IsHDConf(m_tConf) &&
							  IsNeedAdapt(GetLocalVidBrdSrc(), m_ptMtTable->GetMt(byLoop), MODE_VIDEO) &&
							  m_ptMtTable->GetMtLogicChnnl(byLoop, LOGCHL_VIDEO, &tVidChn, TRUE) &&
							  !IsNeedAdapt(GetLocalAudBrdSrc(), m_ptMtTable->GetMt(byLoop), MODE_AUDIO);
			if (bNeedAdp)
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,  "[StartSwitchAud3MtNeedAdp] Start Vmp To Mt(%d,%d) Audio Switch\n",
					m_ptMtTable->GetMt(byLoop).GetMcuId(),
					m_ptMtTable->GetMt(byLoop).GetMtId()
					);
				//continue;
//				bRet = ( bRet && StartSwitchAud2MtNeedAdp( byLoop, bySwitchMode) );
				bRet = ( bRet && StartSwitchAud2MtNeedAdp( GetVidBrdSrc(), m_ptMtTable->GetMt(byLoop)));
			}				
		
		}
	}

	return bRet;

		
}

/*====================================================================
    ������      StartSwitchAud2Perieqp
    ����        ������ͬ������ ������Դ�����й㲥��Ƶ�����ͨ��(setparam cmd ʱ��)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/08/07    4.6         �ܼ���          ����
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchAud2Perieqp(TMt tSrc)
{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	return TRUE;
#endif
	if(!g_cMcuVcApp.IsVASimultaneous())
	{
		return FALSE;
	}

	if (tSrc.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchAud2Perieqp] tSrc is null!\n");
		return FALSE;
	}
	
	// [10/18/2010 xliang] vmp won't broadcast through bas
	if (TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_VMP == tSrc.GetEqpType())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchAud2Perieqp] tSrc is VMP, so no through bas!\n");
		return FALSE;
	}

	u8 byNum = 0;
	CBasChn *apcBasChn[MAXNUM_PERIEQP]={NULL};
	if(!GetBasBrdChnGrp(byNum, apcBasChn, MODE_VIDEO))
	{
		return FALSE;
	}
	u8 bySrcChn = m_tPlayEqp == GetLocalVidBrdSrc() ? m_byPlayChnnl : 0;

	TPeriEqpStatus tStatus;
	TEqp tBas;
	tBas.SetNull();
	for (u8 byIdx = 0; byIdx < byNum; byIdx++)
	{
		if ( NULL == apcBasChn[byIdx] )
		{
			continue;
		}
		if(g_cMcuVcApp.GetPeriEqpStatus(apcBasChn[byIdx]->GetBas().GetEqpId(), &tStatus) &&
			( g_cMcuAgent.IsEqpBasHD( apcBasChn[byIdx]->GetBas().GetEqpId() ) ) &&
		    (tStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU ||
		    tStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU_H ||
			tStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU2_BASIC ||
		    tStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU2_ENHANCED
			)
		   
		   )
		{
			//[2011/08/18/zhangli]����ͬ����֧��PRS��StartSwitchToPeriEqp���һ������ΪFALSE
			StartSwitchToPeriEqp(tSrc, bySrcChn, apcBasChn[byIdx]->GetBas().GetEqpId(), apcBasChn[byIdx]->GetChnId(), 
				MODE_AUDIO, SWITCH_MODE_BROADCAST, TRUE, TRUE, FALSE, FALSE);
			tBas = apcBasChn[byIdx]->GetBas();
			tBas.SetConfIdx(m_byConfIdx);
		
			u8 byOutNum = 0;
			u8 byFrontOutNum = 0;
			if ( !GetBasChnOutputNum(apcBasChn[byIdx]->GetBas(),apcBasChn[byIdx]->GetChnId(),byOutNum,byFrontOutNum) )
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[StartSwitchToSubMtNeedAdpt] GetBasChnOutputNum:%d-%d failed!\n",
						 apcBasChn[byIdx]->GetBas().GetEqpId(), apcBasChn[byIdx]->GetChnId()
						);
				return FALSE;
			}

			for (u8 byOutIdx = 0; byOutIdx < byOutNum; byOutIdx++)
			{
				g_cMpManager.SetSwitchBridge(tBas, byFrontOutNum + byOutIdx, MODE_AUDIO, TRUE );
			}
		}
	}
	return TRUE;
}
/*====================================================================
    ������      ��StartSwitchAud2MtNeedAdp
    ����        ������ͬ������ ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/08/07    4.6         �ܼ���          ����
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchAud2MtNeedAdp(TMt tSrc, TMt tDst, u8 bySwitchMode /*= SWITCH_MODE_BROADCAST*/)
{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
	//8ke ��֧�ִ���ͬ����ֱ�Ӵӹ㲥Դ������Ƶ
	//StartSwitchToSubMt(tSrc, 0, tDst.GetMtId(), MODE_AUDIO, bySwitchMode);
	u16 wSpyPort = SPY_CHANNL_NULL;
	if( !tSrc.IsLocal() )
	{
		CRecvSpy cRecvSpy;
		if( m_cSMcuSpyMana.GetRecvSpy( tSrc,cRecvSpy ) )
		{
			wSpyPort = cRecvSpy.m_tSpyAddr.GetPort();
		}
	}
	TMt tLocalMt = GetLocalMtFromOtherMcuMt(tDst);
	u8 bySrcChn = 0;
	if ( tSrc == m_tMixEqp )
	{
		//֧��APU2��Ƶ�����,��Ҫ֪���û���������Ϊ�������û��ǻ������ã�������Ҫ����������������
		if (m_tConf.m_tStatus.IsMixing())//�������Ϊ��������
		{
			/*if (m_ptMtTable->IsMtInMixGrp(tLocalMt.GetMtId()))
			{
				bySrcChn = GetMixChnPos(tLocalMt);
			} 
			else
			{
				bySrcChn = GetMixerNModeChn();
			}*/
			RestoreRcvMediaBrdSrc( tLocalMt.GetMtId(),MODE_AUDIO );
		}
		else//��Ϊ�������ã�������Ҫ��ȡ��Ӧ����ͨ������ӻ�ȡ��Ӧ����ͨ������
		{
		}
	}
	/*TSwitchGrp t8kgSwitchGrp;
	t8kgSwitchGrp.SetSrcChnl(bySrcChn);
	t8kgSwitchGrp.SetDstMtNum(1);
	t8kgSwitchGrp.SetDstMt(&tDst);
	StartSwitchToAll(tSrc, 1, &t8kgSwitchGrp, MODE_AUDIO, bySwitchMode,FALSE,FALSE,wSpyPort);*/
 	return TRUE;
#endif
	
	if (!g_cMcuVcApp.IsVASimultaneous())
	{
		return FALSE;
	}

	if (tSrc.IsNull() || tDst.IsNull())
	{
		return FALSE;
	}
	
	if (CONF_POLLMODE_VIDEO == m_tConf.m_tStatus.GetPollMode())
    {
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[StartSwitchAud2MtNeedAdp] Mt.%d needn't aud switch be ignored due to VID poll!\n", tDst.GetMtId());
        return TRUE;
    }
	
	//��Ƶ���䲻������ͬ��
	if (IsNeedAdapt(tSrc, tDst, MODE_AUDIO))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[StartSwitchAud2MtNeedAdp] Mt:%d need aud adp, aud switch lossed!\n", tDst.GetMtId());
		return FALSE;
	}
	
	//��Ƶ�����䲻������ͬ��
	if (!IsNeedAdapt(tSrc, tDst, MODE_VIDEO))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[StartSwitchAud2MtNeedAdp] Mt:%d needn't vid adp, aud switch lossed!\n", tDst.GetMtId());
		return FALSE;
	}
	
	TEqp tBas;
	tBas.SetNull();
	u8 byChnId  = 0;
	u8 byOutNum = 0;
	u8 byOutIdx = 0;
	//��ȡ��Ƶ���������
	if (!GetAudBufOutPutInfo(tBas, byChnId, byOutNum, byOutIdx))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchAud2MtNeedAdp] GetAudBufOutPutInfo failed!\n");
		return FALSE;
	}
	
	//[nizhijun 2010/9/25] ��MPU ��������Ƶ����
	TPeriEqpStatus tBasStatus;
	if ( (!g_cMcuVcApp.GetPeriEqpStatus(tBas.GetEqpId(),&tBasStatus)) ||
		 ( g_cMcuAgent.IsEqpBasAud( tBas.GetEqpId() ) ) ||
		 ( tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU && 
		   tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU_H&&
		   tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU2_BASIC && 
		   tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU2_ENHANCED
		  )  	 	
	   )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[StartSwitchAud2MtNeedAdp] Mt:%d not need Aud!\n",tDst.GetMtId());
		return FALSE;
	}

	tBas.SetConfIdx(m_byConfIdx);
	
	//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
	//����Ƶ����
	//StartSwitchToSubMt(tBas, byChnId * byOutNum, tDst.GetMtId(), MODE_AUDIO, bySwitchMode);	
	TSwitchGrp tSwitchGrp;
	tSwitchGrp.SetSrcChnl(byChnId * byOutNum);
	tSwitchGrp.SetDstMtNum(1);
	tSwitchGrp.SetDstMt(&tDst);
	StartSwitchToAll(tBas, 1, &tSwitchGrp, MODE_AUDIO, bySwitchMode);
	return TRUE;
}

/*====================================================================
������       GetAudBufOutPutInfo
����        ����ȡ��Ƶ��������㣬�ҹ㲥ͨ����һ��ͨ���������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ����
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
10/07/16    4.6         �ܼ���          ����
2011/12/31  4.7         ��־��			��д
====================================================================*/
BOOL32 CMcuVcInst::GetAudBufOutPutInfo(TEqp &tBas, u8 &byChnId, u8 &byOutNum, u8 &byOutIdx)
{
	CBasChn *pBasChn[MAXNUM_PERIEQP] = {NULL};
	u8		byBasChnNum = 0;

	if ( !GetBasBrdChnGrp(byBasChnNum, pBasChn,MODE_VIDEO) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[GetAudBufOutPutInfo]GetBasBrdChnGrp failed!\n");
		return FALSE;
	}

	for( u8 byIdx = 0 ;byIdx <byBasChnNum; byIdx++ )
	{
		if ( pBasChn[byIdx] != NULL  &&
			 !pBasChn[byIdx]->GetBas().IsNull()
			)
		{
			TPeriEqpStatus tBasStatus;
			if (!g_cMcuVcApp.GetPeriEqpStatus( pBasChn[byIdx]->GetBas().GetEqpId(), &tBasStatus))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[GetAudBufOutPutInfo] GetPeriEqpStatus failed!\n");
				continue;
			}
			
			if (  g_cMcuAgent.IsEqpBasAud( tBas.GetEqpId() )  )
			{
				continue;
			}

			if (tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU &&
				tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU_H &&
				tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU2_BASIC &&
				tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU2_ENHANCED
				)
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_BAS, "[GetAudBufOutPutInfo] bas:%d-%d is not mpu or mpu2 which is not support With Aud!\n",
						  pBasChn[byIdx]->GetBas().GetEqpId(), pBasChn[byIdx]->GetChnId()
						  );
				continue;
			}

			tBas = pBasChn[byIdx]->GetBas();
			byChnId = pBasChn[byIdx]->GetChnId();
			THDAdaptParam atParam[MAXNUM_BASOUTCHN];
			if ( g_cMcuVcApp.GetBasOutPutParam(tBas,byChnId,byOutNum,atParam) )
			{
				for ( u8 byLoop = 0; byLoop<byOutNum; byLoop++ )
				{
					if ( !atParam[byLoop].IsNull() )
					{
						byOutIdx = byLoop;
						return TRUE;
					}
						
				}
			}
		}

	}

	return TRUE;
}
/*====================================================================
    ������      StartSwitchToSubMtFromAdp
    ����        �������ݽ����������� �ӱ�����ͨ����ĳ�ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
  2010-6-30		4.6			�ܼ���			����
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToSubMtFromAdp(u8 byMtId, u8 byMediaMode)
{
	if (byMediaMode != MODE_AUDIO && byMediaMode != MODE_VIDEO && byMediaMode != MODE_SECVIDEO)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchToSubMtFromAdp2] unexpected MediaMode:%d!\n", byMediaMode);
		return FALSE;
	}

	TBasOutInfo tOutInfo;
	if (!FindBasChn2BrdForMt(byMtId, byMediaMode, tOutInfo))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchToSubMtFromAdp2] FindAcceptBasChnForMt%d failed!\n", byMtId);
		return FALSE;
	}

	if ( !g_cMcuVcApp.IsPeriEqpConnected(tOutInfo.m_tBasEqp.GetEqpId()) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchToSubMtFromAdp] tBas:%d-%d is not online,don't switch to byMtId:%d!\n",
				tOutInfo.m_tBasEqp.GetEqpId(),tOutInfo.m_byChnId,byMtId);
		return FALSE;
	}

	tOutInfo.m_tBasEqp.SetConfIdx(m_byConfIdx);
	//����
	g_cMpManager.SetSwitchBridge(tOutInfo.m_tBasEqp, tOutInfo.m_byFrontOutNum + tOutInfo.m_byOutIdx, byMediaMode, TRUE);
	//������Ŀ���ն�
	if (MODE_SECVIDEO == byMediaMode)
	{
		//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
// 		g_cMpManager.StartSwitchToSubMt(tBas, 
// 										byChnId * byOutNum + byOutIdx,
// 										m_ptMtTable->GetMt(byMtId), 
// 										byMediaMode, 
// 										SWITCH_MODE_BROADCAST, TRUE, TRUE, TRUE);
		TSwitchGrp tSwitchGrp;
		tSwitchGrp.SetSrcChnl(tOutInfo.m_byFrontOutNum + tOutInfo.m_byOutIdx);
		tSwitchGrp.SetDstMtNum(1);
		TMt tMt = m_ptMtTable->GetMt(byMtId);
		tSwitchGrp.SetDstMt(&tMt);
		g_cMpManager.StartSwitchToAll(tOutInfo.m_tBasEqp, 1, &tSwitchGrp, byMediaMode);
	}
	else
	{
		//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
		//StartSwitchToSubMt(tBas, byChnId * byOutNum + byOutIdx, byMtId, byMediaMode);
		TSwitchGrp tSwitchGrp;
		tSwitchGrp.SetSrcChnl(tOutInfo.m_byFrontOutNum + tOutInfo.m_byOutIdx);
		tSwitchGrp.SetDstMtNum(1);
		TMt tMt = m_ptMtTable->GetMt(byMtId);
		tSwitchGrp.SetDstMt(&tMt);
		StartSwitchToAll(tOutInfo.m_tBasEqp, 1, &tSwitchGrp, byMediaMode);

		StartSwitchAud2MtNeedAdp(GetAudBrdSrc(), m_ptMtTable->GetMt(byMtId));	
	}
	
	//[nizhijun 2011/01/05]����������Ҫ��������ն˵�prs��RTCP��������
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if ( tConfAttrb.IsResendLosePack() )
	{
		u8 byPrsId, byPrsChnnlId = 0;
		if (FindPrsChnForSrc(tOutInfo.m_tBasEqp,tOutInfo.m_byFrontOutNum+tOutInfo.m_byOutIdx, byMediaMode, byPrsId,byPrsChnnlId))
		{
			AddRemovePrsMember( byMtId, byPrsId, byPrsChnnlId, byMediaMode, TRUE);
		} 
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[StartSwitchToSubMtFromAdp2] FindPrsChnnl for recvmt:%d failed!\n", byMtId);
		}
	}

	return TRUE;
}

/*====================================================================
������      StartAllBasSwitch
����        ��������Ӧģʽ�Ĺ㲥���佻��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ����
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2010/07/14  3.0         �ܼ���         ����
====================================================================*/
BOOL32 CMcuVcInst::StartAllBasSwitch(u8 byMediaMode)
{
	if (byMediaMode != MODE_AUDIO && byMediaMode != MODE_VIDEO && byMediaMode != MODE_SECVIDEO)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_BAS, "[StartAllBasSwitch] unexpected ModiaMode:%d!\n", byMediaMode);
		return FALSE;
	}

	CBasChn *apcBasChn[MAXNUM_PERIEQP]={NULL};
	u8 byNum = 0;
	if(!GetBasBrdChnGrp(byNum, apcBasChn, byMediaMode))
	{
		return FALSE;
	}
	for (u8 byIdx = 0; byIdx < byNum; byIdx++)
	{
		if ( NULL == apcBasChn[byIdx] )
		{
			continue;
		}
		StartBasSwitch(apcBasChn[byIdx]->GetBas(), apcBasChn[byIdx]->GetChnId());
	}
	return TRUE;
}
/*====================================================================
������      StopAllBasSwitch
����        �������Ӧģʽ�Ĺ㲥���佻��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ����
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2010/07/14  3.0         �ܼ���         ����
====================================================================*/
BOOL32 CMcuVcInst::StopAllBasSwitch(u8 byMediaMode)
{
	if (byMediaMode != MODE_AUDIO && byMediaMode != MODE_VIDEO && byMediaMode != MODE_SECVIDEO)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_BAS, "[StopAllBasSwitch] unexpected ModiaMode:%d!\n", byMediaMode);
		return FALSE;
	}
	
	CBasChn *apcBasChn[MAXNUM_PERIEQP]={NULL};
	u8 byNum = 0;
	if(!GetBasBrdChnGrp(byNum, apcBasChn, byMediaMode))
	{
		return FALSE;
	}
	for (u8 byIdx = 0; byIdx < byNum; byIdx++)
	{
		if ( NULL == apcBasChn[byIdx] )
		{
			continue;
		}
		TEqp tBas = apcBasChn[byIdx]->GetBas();
		tBas.SetConfIdx(m_byConfIdx);
		StopBasSwitch(tBas, apcBasChn[byIdx]->GetChnId());
	}
	return TRUE;
}

/*====================================================================
    ������      ��StopSwitchToAllSubMtJoinedConf
    ����        ��ֹͣ�����ݽ����������������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bMsg, �Ƿ�֪ͨ������Ϣ��ȱʡΪTRUE
				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/08    1.0         LI Yi         ����
    07/03/23    4.0         ����        �㲥��Ƶ�����޸�
====================================================================*/
void CMcuVcInst::StopSwitchToAllSubMtJoinedConf( u8 byMode )
{
	u8	byLoop;

	TMt atDstMt[MAXNUM_CONF_MT];
	u8 byMtNum = 0;

    if ( MODE_AUDIO == byMode || MODE_BOTH == byMode )
    {
        for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
        {
            if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )    
            {
				atDstMt[byMtNum] = m_ptMtTable->GetMt( byLoop);
				byMtNum++;
                //StopSwitchToSubMt( byLoop, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE );
            }
        }

		StopSwitchToSubMt( byMtNum, atDstMt, MODE_AUDIO, FALSE );
    }

	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();

	memset( atDstMt, 0, sizeof(atDstMt) );
    byMtNum = 0;
    if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
    {
        // ɾ���㲥Դ
        u8 bySrcChnl = (tLocalVidBrdSrc==m_tPlayEqp) ? m_byPlayChnnl : 0;
        g_cMpManager.StopSwitchToBrd( tLocalVidBrdSrc, bySrcChnl );
        
        // ɾ���㲥����
        for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
        {
            if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )           
            {
				atDstMt[byMtNum] = m_ptMtTable->GetMt( byLoop);
				byMtNum++;
                //StopSwitchToSubMt( byLoop, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE );
            }
        }

		StopSwitchToSubMt( byMtNum, atDstMt, MODE_VIDEO, FALSE );
    }

	MtStatusChange();
    
    return;
}

/*====================================================================
    ������      ��StartSwitchToPeriEqp
    ����        ����ָ���ն����ݽ���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt & tSrc, Դ
				  u8 bySrcChnnl, Դ���ŵ��� (��������˿�)
				  u8 byEqpId, �����
				  u8 byDstChnnl, Ŀ���ŵ������ţ�ȱʡΪ0 (��������˿�)
   				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
				  BOOL32 bAddRtcp���Ƿ����赽Դ��RTCP������ע�����㣺
				  ��1��Դ��ռ��PRS�ģ�ΪTRUE
				  ��2��ռ��PRS�������ΪFALE���������������У�����¼��
				  ��3������Ҫ��RTCP������ΪFALE��Ŀǰ��PRS��HDU��TVWALL

   ����ֵ˵��  ��TRUE�����������ֱ�ӷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/20    1.0         LI Yi         ����
	03/07/24    2.0         ������        �޸�
	2011/08/18  4.6         zhangli       �޸ģ��Ӳ���bAddRtcp��
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToPeriEqp(TMt tSrc,
                                        u16	wSrcChnnl,
                                        u8	byEqpId,
                                        u8  byDstChnnl,
                                        u8	byMode,
                                        u8	bySwitchMode,
                                        BOOL32 bDstHDBas,
                                        BOOL32 bStopBeforeStart,
                                        BOOL32 bBatchPollChg,
										BOOL32 bAddRtcp,
										BOOL32 bSendMsgToEqp,
										const u8 byHduSubChnId,
										const u8 byMulitModeHdu) 
{
	CServMsg	cServMsg;
	TPeriEqpStatus	tStatus;

	//��Ҫ��MODE_VIDEO2SECONDģʽ�Ľ������������bas��prs,prs���轨rtcp������bas���ܹ���ģʽMODE_VIDEO2SECONDתΪMODE_VIDEO
	if (MODE_VIDEO2SECOND == byMode)
	{
		byMode = MODE_VIDEO;
	}

	u8 bySrcChnnl = (u8)wSrcChnnl;
	//  [11/26/2009 pengjie] Modify ������ش�֧�֣���������Ľ���
	CRecvSpy tSpyResource;
	TMt tUnlocalMt = tSrc;
    if(!tSrc.IsLocal() &&
	   m_cSMcuSpyMana.GetRecvSpy( tSrc, tSpyResource ) )
	{

		wSrcChnnl  = tSpyResource.m_tSpyAddr.GetPort();
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp]recv mt(mcuid:%d, mtid:%d) from port %d.bBatchPollChg.%d\n",
			   tSrc.GetMcuId(), tSrc.GetMtId(), wSrcChnnl,bBatchPollChg);
	}
	// End

	tSrc = GetLocalMtFromOtherMcuMt( tSrc );

	//δ����
	if( !g_cMcuVcApp.IsPeriEqpConnected( byEqpId ) )
	{
		return FALSE;
	}
	if( !m_tConf.m_tStatus.IsSpecMixing() &&
		m_ptMtTable->GetManuId(tSrc.GetMtId()) == MT_MANU_CHAORAN && 
		byEqpId >= MIXERID_MIN && byEqpId <= MIXERID_MAX)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_EQP, "[StartSwitchToPeriEqp]ChaoRan Mt(%d,%d) Can't Joined Mixer(%d),So Return!\n",
			tSrc.GetMcuId(), tSrc.GetMtId(), byEqpId);
		return FALSE;
	}
	//����KDC�ն��Ƿ�������״̬�����򲻽�����Ƶ����	
	if( TYPE_MT == m_ptMtTable->GetMainType( tSrc.GetMtId() ) && 
		MT_MANU_KDC != m_ptMtTable->GetManuId( tSrc.GetMtId() ) && 
		m_ptMtTable->IsMtAudioDumb( tSrc.GetMtId() ) )
	{
		if( byMode == MODE_AUDIO )
		{
			return TRUE;
		}
		else if ( byMode == MODE_BOTH )
		{
			byMode = MODE_VIDEO;
		}
	}

	if( !g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tStatus ) )
	{
		return FALSE;
	}


	//[nizhijun 2011/12/29] BAS���ն˿����⴦��ֱ���ڴ˴����м��㣬���÷ŵ�mp����м���
	if ( EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) )
	{
		u8 byBasOutNum = 0;
		u8 byBasFrontOutNum = 0;
		if ( !GetBasChnOutputNum(g_cMcuVcApp.GetEqp(byEqpId),(u8)byDstChnnl,byBasOutNum,byBasFrontOutNum) )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[StopSwitchToPeriEqp] GetBasChnOutputNum:%d-%d failed!\n",
					byEqpId,byDstChnnl
					);
			return  FALSE;
		}
		if(!g_cMpManager.StartSwitchToPeriEqp(tSrc, wSrcChnnl, byEqpId, byBasFrontOutNum, 
										  byMode, bySwitchMode, bDstHDBas, bStopBeforeStart))
		{
			ConfPrint( LOG_LVL_ERROR, MID_MCU_EQP, "[StartSwitchToPeriEqp] ERROR: Cannot switch to specified eqp!\n" );
			return FALSE;
		}
	}
	else
	{
		// [2013/03/11 chenbing] ��ǰHDU�Ƿ�֧�ֶ໭��
		if ( IsValidHduEqp(g_cMcuVcApp.GetEqp(byEqpId))
			&& g_cMcuVcApp.IsMultiModeHdu(byEqpId)
			)
		{
			// [2013/03/11 chenbing] �໭��ͨ��(0,1)���Ϊ20 
			if(!g_cMpManager.StartSwitchToPeriEqp(tSrc, wSrcChnnl, byEqpId, COMPOSECHAN(byDstChnnl, byHduSubChnId), 
				byMode, bySwitchMode, bDstHDBas, bStopBeforeStart, byHduSubChnId))
			{
				ConfPrint( LOG_LVL_ERROR, MID_MCU_EQP, "[StartSwitchToPeriEqp] ERROR: Cannot switch to specified eqp!\n" );
				return FALSE;
			}
		}
		else
		{
			if(!g_cMpManager.StartSwitchToPeriEqp(tSrc, wSrcChnnl, byEqpId, byDstChnnl, 
				byMode, bySwitchMode, bDstHDBas, bStopBeforeStart))
			{
				ConfPrint( LOG_LVL_ERROR, MID_MCU_EQP, "[StartSwitchToPeriEqp] ERROR: Cannot switch to specified eqp!\n" );
				return FALSE;
			}
		}

	}

	g_cMcuVcApp.SetPeriEqpSrc( byEqpId, (const TMt*)&tUnlocalMt, (u8)byDstChnnl, byMode);

	//[2011/08/18/zhangli]����Ƕ������飬����eqp--->Դ��RTCP����
	TEqp tEqp = g_cMcuVcApp.GetEqp(byEqpId);

	TConfAttrb tConfattrb = m_tConf.GetConfAttrb();
	if (bAddRtcp && tConfattrb.IsResendLosePack())
	{
		// [2013/03/11 chenbing] �޸�Ŀ��ͨ�� 
		BuildRtcpSwitchForSrcToDst(tEqp, tUnlocalMt, byMode, u8(byDstChnnl), u8(wSrcChnnl), TRUE);
	}

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();

	//֪ͨ�ն˿�ʼ����
    if (tSrc.GetType() == TYPE_MT)
    {
        //zbq[04/15/2009] Tand�Ĺؼ�֡��ıȽϿ죬�Ҽ��Ϊ3s. �ʽ����������ٷ�youareseeing�������õ���һ���ؼ�֡
        if (IsDelayVidBrdVCU() && tSrc == tLocalVidBrdSrc)
        {
        }
        else
        {
            NotifyMtSend( tSrc.GetMtId(), byMode, TRUE );
			
			if ( byEqpId >= PRSID_MIN && byEqpId <= PRSID_MAX )
			{
				ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] prs:%d don't need NotifyMtSend!\n",byEqpId);
			}
			else
			{
				
				if ( byEqpId >= HDUID_MIN && byEqpId <= HDUID_MAX)
				{
					// zbq [06/25/2007] ����Ƶ����������ؼ�֡
					// [pengjie 2010/4/6] Modify ������ش�����ؼ�֧֡��
					if( tUnlocalMt.IsLocal() )
					{						
						if( MODE_BOTH == byMode || MODE_VIDEO == byMode )
						{            
							NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
						}	
						
						// [pengjie 2010/9/6] ˫��֧��
						if( MODE_SECVIDEO == byMode )
						{
							ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] Hdu NotifyFastUpdate MODE_SECVIDEO!\n" );
							NotifyFastUpdate(tSrc, MODE_SECVIDEO, TRUE);
						}
					}
					else
					{
						if(m_cSMcuSpyMana.IsMtInSpyMember( tUnlocalMt, MODE_VIDEO ))
						{
							// [pengjie 2010/9/6] ����˫��֧��
							//NotifyFastUpdate(tUnlocalMt, MODE_VIDEO, FALSE);
							if( MODE_BOTH == byMode || MODE_VIDEO == byMode )
							{
								NotifyFastUpdate(tUnlocalMt, MODE_VIDEO, FALSE);
							}
							
							if( MODE_SECVIDEO == byMode )
							{
								ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] Hdu Cascade NotifyFastUpdate MODE_SECVIDEO!\n" );
								NotifyFastUpdate( tUnlocalMt, MODE_SECVIDEO, FALSE );
							}
						}
					}
					// End
				}
				else
				{
					// zbq [06/25/2007] ����Ƶ����������ؼ�֡
					// 				if( MODE_BOTH == byMode || MODE_VIDEO == byMode )
					// 				{            
					// 					NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
					// 				}
					// [pengjie 2010/4/6] Modify ������ش�����ؼ�֧֡��
					if( tUnlocalMt.IsLocal() )
					{
						BOOL32 bNeedIFrame = TRUE;
#if defined(_8KH_) || defined(_8KE_) || defined(_8KI_)
						// 8khvmp���������ԣ����з��ʱ�������仯ʱ��ͣ�������򴴽��½��������Ա����ն�mcu�ཨ����ʱ����������ؼ�֡
						if (IsValidVmpId(byEqpId))
						{
							// ����������ؼ�֡
							bNeedIFrame = FALSE;
						}
#endif
						if (bNeedIFrame)
						{
							if(MODE_BOTH == byMode || MODE_VIDEO == byMode || MODE_VIDEO2SECOND == byMode)
							{          
								NotifyFastUpdate(tSrc, MODE_VIDEO);
							}					
							// ˫��֧��
							if( MODE_SECVIDEO == byMode )
							{
								ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] eqp(id:%d) NotifyFastUpdate MODE_SECVIDEO!\n", byEqpId);
								NotifyFastUpdate(tSrc, MODE_SECVIDEO, FALSE);
							}
						}
					}
					else
					{
						if(m_cSMcuSpyMana.IsMtInSpyMember( tUnlocalMt, MODE_VIDEO ))
						{
							NotifyFastUpdate(tUnlocalMt, MODE_VIDEO);
						}
					}
					// End
				}
			}
        }
    }

	//send message according to its type
	if( !g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tStatus ) )
	{
		return FALSE;
	}

    TTvWallStartPlay tTwPlayPara;
    THduStartPlay  tHduPlayPara;
	u8 byRtcpMode = byMode;
	if (MODE_AUDIO  == byMode)
    {
        byMode = HDU_OUTPUTMODE_AUDIO;
    }
    else if (MODE_VIDEO == byMode || MODE_SECVIDEO == byMode)
    {
        byMode = HDU_OUTPUTMODE_VIDEO;
    }
    else if (MODE_BOTH  == byMode || MODE_SECVIDEO == byMode)
    {
        byMode = HDU_OUTPUTMODE_BOTH;
    }
    else
    {
        byMode = HDU_OUTPUTMODE_BOTH;
    }

	tHduPlayPara.SetMode( byMode );

    TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();

	if(IsValidHduEqp(g_cMcuVcApp.GetEqp(tStatus.GetEqpId())))
    {
        tHduPlayPara.SetMt(tUnlocalMt);
		tHduPlayPara.SetVideoEncrypt(tEncrypt);
    }
	else
	{
		tTwPlayPara.SetMt(tUnlocalMt);
	    tTwPlayPara.SetVideoEncrypt(tEncrypt);
	}

	u32 dwVidSwitchIpAddr = 0;
	u16 wVidSwitchPort = 0;
	u32 dwAudSwitchIpAddr = 0;
	u16 wAudSwitchPort = 0;
	TTransportAddr tAudRemoteAddr;

	TConfAttrb tTmpConfAttrb = m_tConf.GetConfAttrb();
    if (tTmpConfAttrb.IsResendLosePack())
    {
		if(IsValidHduEqp(g_cMcuVcApp.GetEqp(tStatus.GetEqpId())))
		{
			if(MODE_VIDEO == byRtcpMode  || MODE_BOTH == byRtcpMode || MODE_SECVIDEO == byRtcpMode)
			{
				GetHduMemberRtcpAddrInfo(tUnlocalMt,bySrcChnnl, byRtcpMode, dwVidSwitchIpAddr, wVidSwitchPort, dwAudSwitchIpAddr, wAudSwitchPort);
				tHduPlayPara.SetVidRtcpBackAddr(dwVidSwitchIpAddr, wVidSwitchPort);
			}

			if(MODE_AUDIO == byRtcpMode || MODE_BOTH == byRtcpMode)
			{
				GetHduMemberRtcpAddrInfo(tUnlocalMt,bySrcChnnl, byRtcpMode, dwVidSwitchIpAddr, wVidSwitchPort, dwAudSwitchIpAddr, wAudSwitchPort);
				//tHduPlayPara.SetAudRtcpBackAddr(dwAudSwitchIpAddr, wAudSwitchPort);
				tAudRemoteAddr.SetIpAddr(dwAudSwitchIpAddr);
				tAudRemoteAddr.SetPort(wAudSwitchPort);
			}

            tHduPlayPara.SetIsNeedByPrs(TRUE);
		}
		else
		{
			if(MODE_VIDEO == byRtcpMode || MODE_SECVIDEO == byRtcpMode || MODE_BOTH == byRtcpMode)
			{
				GetHduMemberRtcpAddrInfo(tUnlocalMt,bySrcChnnl, byRtcpMode, dwVidSwitchIpAddr, wVidSwitchPort, dwAudSwitchIpAddr, wAudSwitchPort);
				tTwPlayPara.SetVidRtcpBackAddr(dwVidSwitchIpAddr, wVidSwitchPort);
			}
			
			if(MODE_AUDIO == byRtcpMode || MODE_BOTH == byRtcpMode)
			{
				GetHduMemberRtcpAddrInfo(tUnlocalMt,bySrcChnnl, byRtcpMode, dwVidSwitchIpAddr, wVidSwitchPort, dwAudSwitchIpAddr, wAudSwitchPort);
				//tTwPlayPara.SetAudRtcpBackAddr(dwAudSwitchIpAddr, wAudSwitchPort);
				tAudRemoteAddr.SetIpAddr(dwAudSwitchIpAddr);
				tAudRemoteAddr.SetPort(wAudSwitchPort);
			}

            tTwPlayPara.SetIsNeedByPrs(TRUE);
		}

    }
    else
    {
		if(IsValidHduEqp(g_cMcuVcApp.GetEqp(tStatus.GetEqpId())))
		{
			tHduPlayPara.SetIsNeedByPrs(FALSE);
		}
		else
		{
            tTwPlayPara.SetIsNeedByPrs(FALSE);
		}
	}
	
    TSimCapSet tSrcSCS;
	if ( EQP_TYPE_VMP == tSrc.GetEqpType() )    //hduѡ��vmpʱ��vmp���Ϊ�ն�Դ
	{
		// xliang [6/23/2009] �˴���mediatypeά�ֻ�������ʽ����vmp��HDU������������ѡ����ͨ��
        tSrcSCS.SetVideoMediaType( m_tConf.GetMainVideoMediaType() );
		// xliang [5/12/2009] ����VMP ��0��ʵ��������غɸ�ֵ
// 		u8 byMediaType = 0;
// 		GetResByVmpOutChnnl(byMediaType, 0, m_tConf, m_tVmpEqp.GetEqpId());
// 		tSrcSCS.SetVideoMediaType(byMediaType);
	}
	else
	{
		tSrcSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId()); 
	}
    TDoublePayload tDVPayload;
    TDoublePayload tDAPayload;

    if (MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType() || 
        MEDIA_TYPE_H263PLUS == tSrcSCS.GetVideoMediaType() || 
        CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
        // zbq [10/29/2007] ��ӦFEC֧��������ʽ
        m_tConf.GetCapSupportEx().IsVideoSupportFEC())
    {
        tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType());
        tDVPayload.SetActivePayload(GetActivePayload(m_tConf, tSrcSCS.GetVideoMediaType()));
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] tDVPayload.GetRealPayLoad() = %u\n", tDVPayload.GetRealPayLoad());
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] tDVPayload.GetActivePayload() = %u\n", tDVPayload.GetActivePayload());
    }
    else
    {
        tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType());
        tDVPayload.SetActivePayload(tSrcSCS.GetVideoMediaType());
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] tDVPayload.GetRealPayLoad() = %u\n", tDVPayload.GetRealPayLoad());
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] tDVPayload.GetActivePayload() = %u\n", tDVPayload.GetActivePayload());
    }
    
    if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
        // zbq [10/29/2007] ��ӦFEC֧��������ʽ
        m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
    {
        tDAPayload.SetRealPayLoad(tSrcSCS.GetAudioMediaType());
        tDAPayload.SetActivePayload(GetActivePayload(m_tConf, tSrcSCS.GetAudioMediaType()));
    }
    else
    {
        tDAPayload.SetRealPayLoad(tSrcSCS.GetAudioMediaType());
        tDAPayload.SetActivePayload(tSrcSCS.GetAudioMediaType());
    }

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] tSrcSCS.GetAudioMediaType(): %d\n", 
	tSrcSCS.GetAudioMediaType());


	if (bSendMsgToEqp)
	{
		switch(tStatus.GetEqpType())
		{
		case EQP_TYPE_TVWALL:
			{
				cServMsg.SetMsgBody((u8 *)&tTwPlayPara, sizeof(tTwPlayPara));
				cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload));
				cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload));
  				cServMsg.SetChnIndex(byDstChnnl);
				// MCUǰ�����zgc��2007-09-27
				TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
				cServMsg.CatMsgBody((u8*)&tCapSupportEx, sizeof(tCapSupportEx));
				cServMsg.CatMsgBody((u8*)&tAudRemoteAddr, sizeof(tAudRemoteAddr));

				SendMsgToEqp(byEqpId, MCU_TVWALL_START_PLAY_REQ, cServMsg);
				break;
			}
		case EQP_TYPE_HDU:
// 		case EQP_TYPE_HDU_H:
// 		case EQP_TYPE_HDU_L:
// 		case EQP_TYPE_HDU2:
// 		case EQP_TYPE_HDU2_L:
			{
				cServMsg.SetMsgBody((u8 *)&tHduPlayPara, sizeof(tHduPlayPara));
				cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload));
				cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload));
				cServMsg.SetChnIndex(byDstChnnl);
				// [2013/03/11 chenbing] ���HDUId 
				cServMsg.SetEqpId(byEqpId);
        
				//  [1/13/2010 pengjie] Modify ����HDU����ͨ���ı���
				TEqpExCfgInfo tEqpExCfgInfo;
				if( SUCCESS_AGENT != g_cMcuAgent.GetEqpExCfgInfo( tEqpExCfgInfo ) )
				{
					tEqpExCfgInfo.Init();
				}
				// [2013/03/11 chenbing]  
				TPeriEqpStatus tHduStatus;
				g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tHduStatus);

				THDUExCfgInfo tHDUExCfgInfo;
				tHDUExCfgInfo = tEqpExCfgInfo.m_tHDUExCfgInfo;
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,  "[StartSwitchToPeriEqp] Set HduChn IdleChlShowMode: %d \n", tHDUExCfgInfo.m_byIdleChlShowMode );	
				cServMsg.CatMsgBody( (u8 *)&tHDUExCfgInfo, sizeof(THDUExCfgInfo) );
				cServMsg.CatMsgBody( (u8 *)&tAudRemoteAddr, sizeof(tAudRemoteAddr) );
				//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
				TLogicalChannel tLogicalChannel;
				m_ptMtTable->GetMtLogicChnnl(tSrc.GetMtId(),LOGCHL_AUDIO,&tLogicalChannel,FALSE);
				u8 byAudioTrackNum = tLogicalChannel.GetAudioTrackNum();
				cServMsg.CatMsgBody( (u8 *)&byAudioTrackNum, sizeof(u8) );
				// [2013/03/11 chenbing]  ׷��HDU��ͨ��
				cServMsg.CatMsgBody( (u8 *)&byHduSubChnId, sizeof(u8) );
				// [2013/03/11 chenbing]  ׷��HDU���
				cServMsg.CatMsgBody( (u8 *)&byMulitModeHdu, sizeof(u8) );
				// End Modify

				// [2013/03/11 chenbing]  
				u8 byCurChnState = tHduStatus.m_tStatus.tHdu.GetChnStatus(byDstChnnl, byHduSubChnId);

				if ( byDstChnnl >=MAXNUM_HDU_CHANNEL )
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU,  "[StartSwitchToPeriEqp]HDUID:%d,CHNID:%d Status:%d,So Return!!\n",byEqpId,byDstChnnl,byCurChnState);
					return FALSE;
				}
				
				// [2013/03/11 chenbing]  
				tHduStatus.m_tStatus.tHdu.SetChnStatus(byDstChnnl, byHduSubChnId, THduChnStatus::eWAITSTART);
				ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] ");
				ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP, "Mcs HduId(%d) ChnId(%d) HduSubChnId(%d) MeMber:(%d) Status(%d)!!!\n",
					byEqpId, byDstChnnl, byHduSubChnId, tHduStatus.m_tStatus.tHdu.GetMemberType(byDstChnnl),
				tHduStatus.m_tStatus.tHdu.GetChnStatus(byDstChnnl, byHduSubChnId) );

				g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tHduStatus);

				SendMsgToEqp(byEqpId, MCU_HDU_START_PLAY_REQ, cServMsg);

				// xliang [7/15/2009] ��VMP����ؼ�֡
				if(EQP_TYPE_VMP == tSrc.GetEqpType())
				{
					NotifyFastUpdate(tSrc, bySrcChnnl);
				}
				break;
			}
		default:
			break;
		}
	}

	return TRUE;
}

/*====================================================================
    ������      : GetHduMemberRtcpAddrInfo
    ����        : ��ָ���ն����ݽ���������
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: TMt      tSrc[in]:      ����ǽ�ĳ�Ա
   				  u8     byMode[in]:      ý��ģʽ��audio�� video, secdvideo;
				  u32 dwVidIpAddr[out]:      ��Ƶ�ն�IP��ַ
				  u16    wVidPort[out]:      ��Ƶ�ն˶˿�
				  u32 dwAudIpAddr[out]:      ��Ƶ�ն�IP��ַ
				  u16    wAudPort[out]:      ��Ƶ�ն˶˿�
   ����ֵ˵��  �� void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/09/13  4.6         ������        ����
====================================================================*/
void CMcuVcInst::GetHduMemberRtcpAddrInfo(const TMt tSrc, 
										  const u8 bySrcChl,
										  const u8 byMode, 
										  u32 &dwVidIpAddr, 
										  u16 &wVidPort, 
										  u32 &dwAudIpAddr, 
										  u16 &wAudPort)
{
	if(tSrc.IsNull())
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "Tmt is NULL!\n");
		return;
	}

	//�ն�
	if( TYPE_MT == tSrc.GetType())
	{
		TMt tTempPrsSrc = GetLocalMtFromOtherMcuMt(tSrc);
		if ( IsMultiCastMt(tTempPrsSrc.GetMtId()) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_PRS,"[GetHduMemberRtcpAddrInfo]tTempPrsSrc:%d-%d is multicastmt!\n",tTempPrsSrc.GetMcuId(), tTempPrsSrc.GetMtId());
			return;
		}	

		TLogicalChannel tLogicalChannel;
		//�����ն�
		if(tSrc.IsLocal())
		{
			if(MODE_VIDEO == byMode || MODE_BOTH == byMode)
			{
				if(!m_ptMtTable->GetMtLogicChnnl(tSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo:local+vid] Fail to Get Mt logical\n");
					
				}

				dwVidIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
		        wVidPort =    tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
			}
			
			if(MODE_AUDIO == byMode || MODE_BOTH == byMode)
			{
				if(!m_ptMtTable->GetMtLogicChnnl(tSrc.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE))
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo:local+aud] Fail to Get Mt logical\n");

				}

				dwAudIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
		        wAudPort =    tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
			}
			
			if(MODE_SECVIDEO == byMode)
			{
				if(!m_ptMtTable->GetMtLogicChnnl(tSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE))
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo:local+sec] Fail to Get Mt logical\n");
				}

				dwVidIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
		        wVidPort =    tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
			}
				
		}
		//�Ǳ����ն�
		else
		{
			if (!IsLocalAndSMcuSupMultSpy(tSrc.GetMcuId()))
			{
				TMt tLocalHduSrcMt = GetLocalMtFromOtherMcuMt(tSrc);				
				if (!tLocalHduSrcMt.IsNull())
				{
					if(MODE_VIDEO == byMode || MODE_BOTH == byMode)
					{
						if(m_ptMtTable->GetMtLogicChnnl(tLocalHduSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
						{
							dwVidIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
							wVidPort =    tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
						}
						else
						{
							ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo:Unlocal+UnSpy+Vid] Fail to Get Mt logical\n");
						}
					}
					
					if(MODE_AUDIO == byMode || MODE_BOTH == byMode)
					{
						if(m_ptMtTable->GetMtLogicChnnl(tLocalHduSrcMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE))
						{
							dwAudIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
							wAudPort =    tLogicalChannel.GetSndMediaCtrlChannel().GetPort();						
						}
						else
						{
							ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo:Unlocal+UnSpy+Aud] Fail to Get Mt logical\n");
						}
						
					}
					
					if(MODE_SECVIDEO == byMode)
					{
						if(m_ptMtTable->GetMtLogicChnnl(tLocalHduSrcMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE))
						{
							dwVidIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
							wVidPort =    tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
						}
						else
						{
							ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo:Unlocal+UnSpy+Sec] Fail to Get Mt logical\n");
						}
					}
				}
				else
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo] GetLocalMtFromOtherMcuMt(tmt:%d-%d) failed!\n",tSrc.GetMcuId(),tSrc.GetMtId());
					return;
				}
			}
			else
			{
				TMt tSecLocalHduSrcMt = GetLocalMtFromOtherMcuMt(tSrc);
				if(MODE_SECVIDEO == byMode)
				{
					if(m_ptMtTable->GetMtLogicChnnl(tSecLocalHduSrcMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE))
					{
						dwVidIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
						wVidPort =    tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
					}
					else
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo:Unlocal+Spy+Sec] Fail to Get Mt logical\n");
					}
				}
				else
				{
					CRecvSpy tRecvSpyInfo;	
					u32 dwIpAddr;
					dwIpAddr = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( GetLocalMtFromOtherMcuMt(tSrc).GetMtId()));
					if(!m_cSMcuSpyMana.GetRecvSpy( tSrc, tRecvSpyInfo ))
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo] Fail to get multiple spy info\n");
						return;
					}
					if(MODE_VIDEO == byMode || MODE_BOTH == byMode)
					{
						dwVidIpAddr = dwIpAddr;
						wVidPort = tRecvSpyInfo.m_tSpyAddr.GetPort() + 1;
					}
					
					if(MODE_AUDIO == byMode || MODE_BOTH == byMode)
					{
						dwAudIpAddr = dwIpAddr;
						wAudPort = tRecvSpyInfo.m_tSpyAddr.GetPort() + 3;
					}
				}
			}
			return;
		}
	}
	else if(EQP_TYPE_VMP == tSrc.GetEqpType())
	{
		/*u8 byMediaType = m_tConf.GetMainVideoMediaType();
		u8 byRes = m_tConf.GetMainVideoFormat();
		
		
		
		TVideoStreamCap tStrCap = m_tConf.GetMainSimCapSet().GetVideoCap();
		bySrcChnnl = GetVmpOutChnnlByRes(tSrc.GetEqpId(),
			tStrCap.GetResolution(), 
			tStrCap.GetMediaType(),
			tStrCap.GetUserDefFrameRate(),
			tStrCap.GetH264ProfileAttrb());

		u32 dwSwitchIp = 0;
		u16 wSwitchPort = 0;*/
		TTransportAddr tRemoteAddr;
		if( GetRemoteRtcpAddr(tSrc, bySrcChl, MODE_VIDEO, tRemoteAddr) )
		{
			dwVidIpAddr = tRemoteAddr.GetIpAddr();
			wVidPort   = tRemoteAddr.GetPort();
		}
		//g_cMpManager.GetSwitchInfo(tSrc, dwSwitchIp, wSwitchPort, dwVidIpAddr);
		//wVidPort = wSwitchPort + bySrcChnnl * PORTSPAN + 1;
		return;
	}

}
/*====================================================================
    ������      ��StopSwitchToPeriEqp
    ����        ��ֹͣ�����ݽ���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpId,  �����
				  u8 byDstChnnl, �ŵ������ţ�ȱʡΪ0
				  BOOL32 bMsg, �Ƿ�ֹͣ������Ϣ��ȱʡΪTRUE
				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/11    1.0         LI Yi         ����
	03/07/24    2.0         ������        �޸�
====================================================================*/
void CMcuVcInst::StopSwitchToPeriEqp( u8 byEqpId,  u8 byDstChnnl, BOOL32 bMsg, u8 byMode, u8 bySwitchMode, u8 byHduSubChnId)
{
	CServMsg	cServMsg;
	TEqp	tEqp;
	TPeriEqpStatus	tStatus;
	TMt    TSrcMt;

	u8 byEqpChnnl = 0;
	//ͣ������Ľ���ʱ��Ŀǰֻ��bas��mode������MODE_VIDEO2SECOND
	if (MODE_VIDEO2SECOND == byMode)
	{
		byMode = MODE_VIDEO;
	}
    // guzh [5/12/2007] ����Ƿ���ͣ��������
	// lukunpeng [03/26/2010] �����ǰ������״̬�����ͷ�ͨ����
	//[0901]fxh �˽ӿڹ��ܲ�Ӧ��ҵ����������ڻ�������ֹͣ���������ý�������������1.�ӻ��������Ƴ����ն�2.ֻ��Ϊ�ﵽ����ĳ�ն�����
	// ����1ҵ����Ҫ���ն˴ӻ����б����������2ȴ����
// 	if (bReleaseChnnl)
// 	{
// 		//tianzhiyong 2010/03/21 ����EAPU���ͻ�����
// 		if (( g_cMcuVcApp.GetEqpType( byEqpId ) == EQP_TYPE_MIXER || g_cMcuVcApp.GetEqpType( byEqpId ) == EQP_TYPE_EMIXER) &&
// 			m_tMixEqp.GetEqpId() == byEqpId)
// 		{
// 			TMt tmt;
// 			// zbq [03/19/2007] ������һ�λ���ͨ��λ��
// 			for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
// 			{
// 				if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) )
// 				{
// 					tmt = m_ptMtTable->GetMt(byMtId);
// 					if ( 0 != GetMixChnPos(tmt) &&
// 						 GetMixChnPos(tmt) == byDstChnnl )
// 					{
// 						GetMixChnPos( tmt, TRUE );
// 						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR,  "[StopSwitchToPeriEqp] Mixer Chn pos.%d for MT.%d has been cleared !\n", byDstChnnl, byMtId );
// 						break;
// 					}
// 				}
// 			}
// 		}
// 	}

	//[nizhijun 2011/12/29] BAS���ն˿����⴦��ֱ���ڴ˴����м��㣬���÷ŵ�mp����м���
	if ( EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) )
	{
		u8 byBasOutNum = 0;
		u8 byBasFrontOutNum = 0;
		if ( !GetBasChnOutputNum(g_cMcuVcApp.GetEqp(byEqpId),byDstChnnl,byBasOutNum,byBasFrontOutNum) )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[StopSwitchToPeriEqp] GetBasChnOutputNum:%d-%d failed!bySwitchMode.%d\n",
					  byEqpId,byDstChnnl,bySwitchMode
					);
			return ;
		}
		//stop switch
		g_cMpManager.StopSwitchToPeriEqp( m_byConfIdx, byEqpId, byBasFrontOutNum, byMode );
		byEqpChnnl = byDstChnnl;
	}
	else
	{
		if ( IsValidHduEqp(g_cMcuVcApp.GetEqp(byEqpId))
			&& g_cMcuVcApp.IsMultiModeHdu(byEqpId)
			)
		{
			// [2013/03/11 chenbing] �໭��ͨ��(0,1)���Ϊ20
			byEqpChnnl = COMPOSECHAN(byDstChnnl, byHduSubChnId);
			g_cMpManager.StopSwitchToPeriEqp( m_byConfIdx, byEqpId, byEqpChnnl, byMode, byHduSubChnId );
		}
		else
		{
			g_cMpManager.StopSwitchToPeriEqp( m_byConfIdx, byEqpId, byDstChnnl, byMode );
			byEqpChnnl = byDstChnnl;
		}
	}

	//[2011/08/11/zhangli]ͣrtcp����
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (tConfAttrb.IsResendLosePack())
	{
		TEqp tDstEqp = g_cMcuVcApp.GetEqp(byEqpId);

		u32 dwDstIp = 0;
		u16 wDstPort = 0;
		if (byMode == MODE_SECVIDEO)
		{
			g_cMcuVcApp.GetEqpRtcpDstAddr(byEqpId, byEqpChnnl, dwDstIp, wDstPort, MODE_VIDEO);
		}
		else
		{
			g_cMcuVcApp.GetEqpRtcpDstAddr(byEqpId, byEqpChnnl, dwDstIp, wDstPort, byMode);
		}
		
		if (dwDstIp != 0)
		{
			AddRemoveRtcpToDst(dwDstIp,wDstPort,tDstEqp, byEqpChnnl, byMode, TRUE, SPY_CHANNL_NULL, FALSE);
		}
		else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[StopSwitchToPeriEqp] GetEqpRtcpDstAddr for eqp:%d chnl:%d failed!\n",byEqpId,byDstChnnl);
		}
	}


	TSrcMt.SetNull();
	g_cMcuVcApp.SetPeriEqpSrc( byEqpId, &TSrcMt, byDstChnnl, byMode );

	//send stop message
	if( bMsg )
	{
		g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tStatus );
		// [2013/03/11 chenbing] �������Id
		cServMsg.SetEqpId(byEqpId);
		cServMsg.SetChnIndex( byDstChnnl );
		cServMsg.SetConfId( m_tConf.GetConfId() );
		tEqp.SetMcuEqp( (u8)LOCAL_MCUID, byEqpId, tStatus.GetEqpType() );
		cServMsg.SetMsgBody( ( u8 * )&tEqp, sizeof( tEqp ) );
		switch( tStatus.GetEqpType() )
		{
		case EQP_TYPE_TVWALL:
			SendMsgToEqp( byEqpId, MCU_TVWALL_STOP_PLAY_REQ, cServMsg );
			break;

		case EQP_TYPE_HDU:    //4.6 �¼�  jlb
// 		case EQP_TYPE_HDU_H:
// 		case EQP_TYPE_HDU_L:
// 		case EQP_TYPE_HDU2:
// 		case EQP_TYPE_HDU2_L:
			{
				if (byDstChnnl >=MAXNUM_HDU_CHANNEL
					|| THduChnStatus::eRUNNING != tStatus.m_tStatus.tHdu.GetChnStatus(byDstChnnl, byHduSubChnId))
				{
					LogPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[StopSwitchToPeriEqp] EQP(id:%d,chn:%d) Status != THduChnStatus::eRUNNING,So Break!\n",
						byEqpId,byDstChnnl);
					break;
				}
		
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[StopSwitchToPeriEqp] EQP(id:%d, chn:%d, SubChnId:%d) byMode(%d)\n",
						byEqpId, byDstChnnl, byHduSubChnId, byMode);

				// [2013/03/11 chenbing] �л�ͨ��״̬
				tStatus.m_tStatus.tHdu.SetChnStatus(byDstChnnl, byHduSubChnId, THduChnStatus::eWAITSTOP);
				g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);

				// [2013/03/11 chenbing] �����ͨ��
				cServMsg.CatMsgBody((u8 *)&byHduSubChnId, sizeof(u8));
				cServMsg.CatMsgBody((u8 *)&byMode, sizeof(u8));
				SendMsgToEqp( byEqpId, MCU_HDU_STOP_PLAY_REQ, cServMsg );
				CServMsg cToMcsPeriStatusMessage;
				cToMcsPeriStatusMessage.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
				SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
				break;
			}

		case EQP_TYPE_RECORDER:
			SendMsgToEqp( byEqpId, MCU_REC_STOPREC_REQ, cServMsg );
			break;
        case EQP_TYPE_BAS:
#if defined(_8KE_) || defined(_8KH_)																//[03/01/2010] zjl add
			SendMsgToEqp( byEqpId, MCU_BAS_STOPADAPT_REQ, cServMsg );
#else
			if (g_cMcuAgent.IsEqpBasHD(byEqpId))
			{
				SendMsgToEqp( byEqpId, MCU_HDBAS_STOPADAPT_REQ, cServMsg );
			}
			else
			{
				SendMsgToEqp( byEqpId, MCU_BAS_STOPADAPT_REQ, cServMsg );
			}          
#endif
		 
            break;
        default:
            break;
		}
	}
}

/*====================================================================
    ������      ��StopSwitchToAllPeriEqpWatchingSrcMt
    ����        ��ֹͣ�����ݽ����������ѡ��ý��ԴΪָ���ն˵�ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt & tSrc, ָ���ն� 
				  BOOL32 bMsg, �Ƿ�ֹͣ������Ϣ��ȱʡΪTRUE
				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/30    1.0         JQL           ����
====================================================================*/
void CMcuVcInst::StopSwitchToAllPeriEqpWatchingSrcMt(TMt  tSrc, BOOL32 bMsg , u8 byMode )
{
	TMt tSrcMt;
	u8      byEqpId;
	u8      byChnnlNo;

	ConfPrint(LOG_LVL_DETAIL,MID_MCU_MPMGR,"[StopSwitchToAllPeriEqpWatchingSrcMt] bMsg.%d byMode.%d\n",bMsg,byMode);

	tSrc = GetLocalMtFromOtherMcuMt( tSrc );

	for( byEqpId = 1; byEqpId <= MAXNUM_MCU_PERIEQP; byEqpId++)
	{
		if( !g_cMcuVcApp.IsPeriEqpConnected( byEqpId ) )
			continue;

		//Ŀǰֻ�Ե���ǽ�뻭��ϳ�������
		if( g_cMcuAgent.GetPeriEqpType( byEqpId ) != EQP_TYPE_TVWALL 
			&& g_cMcuAgent.GetPeriEqpType( byEqpId ) != EQP_TYPE_VMP )
		{
			continue;
		}

		for( byChnnlNo = 0; byChnnlNo < MAXNUM_PERIEQP_CHNNL; byChnnlNo++ )
		{
			if( g_cMcuVcApp.GetPeriEqpSrc( byEqpId, &tSrcMt, byChnnlNo, MODE_VIDEO )
				&& tSrcMt == tSrc )
				StopSwitchToPeriEqp( byEqpId, byChnnlNo, TRUE, MODE_VIDEO );

			if( g_cMcuVcApp.GetPeriEqpSrc( byEqpId, &tSrcMt, byChnnlNo, MODE_AUDIO )
				&& tSrcMt == tSrc )
				StopSwitchToPeriEqp( byEqpId, byChnnlNo, TRUE, MODE_AUDIO );
		}
	}
}

/*====================================================================
    ������      ��StopSwitchToAllPeriEqpWatchingSrcMcu
    ����        ��ֹͣ�����ݽ����������ѡ��ý��Դ����ָ��MCU��ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const u16 wMcuId, ָ��MCU
				  BOOL32 bMsg, �Ƿ�ֹͣ������Ϣ��ȱʡΪTRUE
				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/30    1.0         JQL           ����
====================================================================*/
//void CMcuVcInst::StopSwitchToAllPeriEqpWatchingSrcMcu( const u16 wMcuId, BOOL32 bMsg , u8 byMode )
// {
// 	TMt tSrcMt;
// 	u8      byEqpId;
// 	u8      byChnnlNo;
// 
// 	for( byEqpId = 1; byEqpId <= MAXNUM_MCU_PERIEQP; byEqpId++)
// 	{
// 		if( !g_cMcuVcApp.IsPeriEqpConnected( byEqpId ) )
// 			continue;
// 
// 		//Ŀǰֻ�Ե���ǽ����
// 		if( g_cMcuAgent.GetPeriEqpType( byEqpId ) != EQP_TYPE_TVWALL )
// 			continue;
// 
// 		for( byChnnlNo = 0; byChnnlNo < MAXNUM_PERIEQP_CHNNL; byChnnlNo++ )
// 		{
// 			if( g_cMcuVcApp.GetPeriEqpSrc( byEqpId, &tSrcMt, byChnnlNo, MODE_VIDEO )
// 				&& tSrcMt.GetMcuId() == wMcuId )
// 				StopSwitchToPeriEqp( byEqpId, byChnnlNo, TRUE, MODE_VIDEO );
// 
// 			if( g_cMcuVcApp.GetPeriEqpSrc( byEqpId, &tSrcMt, byChnnlNo, MODE_AUDIO )
// 				&& tSrcMt.GetMcuId() == wMcuId )
// 				StopSwitchToPeriEqp( byEqpId, byChnnlNo, TRUE, MODE_AUDIO );
// 		}
// 	}
// }

/*====================================================================
    ������      ��StopSwitchToAllPeriEqpInConf
    ����        ��ֹͣ�����ݽ����������ѡ��ý��ԴΪָ���ն˵�ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bMsg, �Ƿ�ֹͣ������Ϣ��ȱʡΪTRUE
				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/30    1.0         JQL           ����
====================================================================*/
void CMcuVcInst::StopSwitchToAllPeriEqpInConf(BOOL32 bMsg , u8 byMode )
{
	TMt tMt;
	u8  byLoop;

	for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
		{
			tMt = m_ptMtTable->GetMt( byLoop );
			StopSwitchToAllPeriEqpWatchingSrcMt( tMt, bMsg, byMode );
		}
	}
}

/*====================================================================
    ������      ��StartSwitchToMc
    ����        ����ָ���ն����ݽ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt & tSrc, Դ
				  u8 bySrcChnnl, Դ���ŵ���
				  u16 wMcInstId, �������̨ʵ����
				  u8 byDstChnnl, Ŀ���ŵ������ţ�ȱʡΪ0
   				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
   ����ֵ˵��  ��TRUE�����������ֱ�ӷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/18    1.0         LI Yi         ����
	03/07/24    2.0         ������        �޸�
	03/03/25    3.0         ������        �޸�
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToMc(TMt tSrc, u8 bySrcChnnl, u16 wMcInstId, 
								u8 byDstChnnl, u8 byMode, u16 wSpyPort ) 
{
	CServMsg	cServMsg;	

	u8 bySwitchMode = byMode;
	//  [11/27/2009 pengjie] Modify ������ش� �����¼��δ��Local����Դ�ն�
	TMt tUnlocalSrc = tSrc;
	tSrc = GetLocalMtFromOtherMcuMt( tSrc );
	// End
	

	//δ����
	if( !g_cMcuVcApp.IsMcConnected( wMcInstId ) )
	{
		return FALSE;
	}


	//����KDC�ն��Ƿ�������״̬�����򲻽�����Ƶ����	
	if( TYPE_MT == m_ptMtTable->GetMainType( tSrc.GetMtId() ) && 
		MT_MANU_KDC != m_ptMtTable->GetManuId( tSrc.GetMtId() ) && 
		m_ptMtTable->IsMtAudioDumb( tSrc.GetMtId() ) )
	{
		if( byMode == MODE_AUDIO )
		{
			//return TRUE;
			bySwitchMode = MODE_NONE;
		}
		else if ( byMode == MODE_BOTH )
		{
			//byMode = MODE_VIDEO;
			bySwitchMode = MODE_VIDEO;
		}
	}


	//  [11/27/2009 pengjie] Modify ������ش�֧��  �����¼�¼�ص��ն˺���Ӧ�ļ��ͨ��
// 	CMcChnnlInfo cMcChnnlInfo;
// 	if( m_cMcChnnlMgr.GetMcChnnlInfo( byDstChnnl, cMcChnnlInfo ) )
// 	{
// 		TMt tMcSrc = cMcChnnlInfo.GetMcSrc();
// 		u8  byMode = cMcChnnlInfo.GetMcChnnlMode();
// 		FreeRecvSpy( tMcSrc, byMode);
// 		m_cMcChnnlMgr.FreeMcChnnl( byDstChnnl );
// 	}
//     if(!g_cMpManager.StartSwitchToMc(tSrc,bySrcChnnl,wMcInstId,byDstChnnl,byMode,wSpyPort))
// 	{
// 		ConfLog( FALSE, "StartSwitchToMc() failed! Cannot switch to specified eqp!\n" );
// 		return FALSE;
// 	}
// 	cMcChnnlInfo.Clear();
// 	cMcChnnlInfo.SetMcChnnlMode( byMode );
// 	cMcChnnlInfo.SetMcSrc( tUnlocalSrc );
// 	m_cMcChnnlMgr.SetMcChnnlInfo( byDstChnnl, cMcChnnlInfo );
	// End

	// [pengjie 2010/8/4] ������ش�֧��  ��ؼ�ز��Ե���
	TMt tMcSrc;
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH ) // ��Ƶ
	{
		g_cMcuVcApp.GetMcSrc( wMcInstId, &tMcSrc, byDstChnnl, MODE_AUDIO );
		if( !tMcSrc.IsNull() )
		{
			if( tMcSrc == tUnlocalSrc )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "StartSwitchToMc GetMcSrc == NewMt, MODE_AUDIO\n" );
				FreeRecvSpy( tMcSrc, MODE_AUDIO);
			}
			else
			{
				//zhouyiliang 20121017 ����ϸ�Դ������һ���������������һ������ֹͣ
				if ( tMcSrc.GetConfIdx() != m_byConfIdx )
				{
					CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle(tMcSrc.GetConfIdx());
					if (NULL != pcVcInst  )
					{
						pcVcInst->FreeRecvSpy( tMcSrc, MODE_AUDIO);
					}
					g_cMpManager.StopSwitchToMc(tMcSrc.GetConfIdx(), wMcInstId, byDstChnnl, MODE_AUDIO);
				}			
				else
				{
					FreeRecvSpy( tMcSrc, MODE_AUDIO);
				}

				g_cMcuVcApp.SetMcSrc( wMcInstId, ( const TMt* )&tUnlocalSrc, byDstChnnl, MODE_AUDIO );
			}
		}
		else
		{
			g_cMcuVcApp.SetMcSrc( wMcInstId, ( const TMt* )&tUnlocalSrc, byDstChnnl, MODE_AUDIO );
		}
	}
	
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH ) // ��Ƶ
	{
		g_cMcuVcApp.GetMcSrc( wMcInstId, &tMcSrc, byDstChnnl, MODE_VIDEO );
		if( !tMcSrc.IsNull() )
		{
			if( tMcSrc == tUnlocalSrc )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "StartSwitchToMc GetMcSrc == NewMt, MODE_VIDEO !\n" );
				FreeRecvSpy( tMcSrc, MODE_VIDEO);
			}
			else
			{
				//zhouyiliang 20121017 ����ϸ�Դ������һ���������������һ������ֹͣ
				if ( tMcSrc.GetConfIdx() != m_byConfIdx )
				{			
					CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle(tMcSrc.GetConfIdx());
					if (NULL != pcVcInst  )
					{
						pcVcInst->FreeRecvSpy( tMcSrc, MODE_VIDEO);
					}
					g_cMpManager.StopSwitchToMc(tMcSrc.GetConfIdx(), wMcInstId, byDstChnnl, MODE_VIDEO);
				}			
				else
				{
					FreeRecvSpy( tMcSrc, MODE_VIDEO);
				}

				g_cMcuVcApp.SetMcSrc( wMcInstId, ( const TMt* )&tUnlocalSrc, byDstChnnl, MODE_VIDEO );
			}
		}
		else
		{
			g_cMcuVcApp.SetMcSrc( wMcInstId, ( const TMt* )&tUnlocalSrc, byDstChnnl, MODE_VIDEO );
		}
	}
	
	if (byMode == MODE_SECVIDEO)
	{
		g_cMcuVcApp.GetMcSrc( wMcInstId, &tMcSrc, byDstChnnl, MODE_SECVIDEO );
		if (!tMcSrc.IsNull())
		{
			if (tMcSrc == tUnlocalSrc)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "StartSwitchToMc GetMcSrc == NewMt, MODE_SECVIDEO !\n" );
			}
			else
			{
				g_cMcuVcApp.SetMcSrc( wMcInstId, ( const TMt* )&tUnlocalSrc, byDstChnnl, MODE_SECVIDEO );
			}
		}
		else
		{
			g_cMcuVcApp.SetMcSrc( wMcInstId, ( const TMt* )&tUnlocalSrc, byDstChnnl, MODE_SECVIDEO );
		}
	}

	if( bySwitchMode != MODE_NONE && !g_cMpManager.StartSwitchToMc(tSrc, bySrcChnnl, wMcInstId, byDstChnnl, bySwitchMode, wSpyPort) )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "StartSwitchToMc() failed! Cannot switch to specified eqp! bySwitchMode.%d\n",bySwitchMode );
		return FALSE;
	}
	//zhouyiliang 20100813 spychnnllnotify
	if (!tUnlocalSrc.IsLocal() && IsLocalAndSMcuSupMultSpy(tUnlocalSrc.GetMcuId()))
	{
		TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tUnlocalSrc.GetMcuId()));
 		SendMMcuSpyNotify( tUnlocalSrc,MCS_MCU_STARTSWITCHMT_REQ, tSimCapSet);
	}
	
	// End

	//֪ͨ�ն˿�ʼ����
	if( tSrc.GetType() == TYPE_MT )
	{
		NotifyMtSend( tSrc.GetMtId(), byMode, TRUE );


        // zbq [06/25/2007] ����Ƶ����������ؼ�֡
        if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
        {
			// [pengjie 2010/8/4] ����Ӧ�������¼��ն˵Ĺؼ�֡
            NotifyFastUpdate(tUnlocalSrc, MODE_VIDEO, TRUE);
        }
		if ( MODE_SECVIDEO == byMode )
		{
			NotifyFastUpdate(tUnlocalSrc, MODE_SECVIDEO);
		}
	}
	else if (tSrc.GetType() == TYPE_MCUPERI)
	{
		// ���Դ��vmp,��vmp��Ӧͨ������ؼ�֡
		if (tSrc.GetEqpType() == EQP_TYPE_VMP)
		{
			//��SwitchSrcToDst�е��ô˷�������vmpָ��ͨ������ؼ�֡,��vmp�������ؼ�֡��Ӱ��vmp��������?
			//NotifyFastUpdate(m_tVmpEqp, bySrcChnnl);
		}
	}

	//send message
	TSwitchInfo tSwitchInfo;
	// [pengjie 2010/8/30] ������Ҫ����local���ն���Ϣ������
	tSwitchInfo.SetSrcMt( tUnlocalSrc );
	tSwitchInfo.SetSrcChlIdx( bySrcChnnl );
	tSwitchInfo.SetDstChlIdx( byDstChnnl );
	tSwitchInfo.SetMode( byMode );
	
	//�����Լ���̬�غ���Ϣ
	TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
	TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS( tSrc.GetMtId() ); 
	TDoublePayload tDVPayload;
	TDoublePayload tDAPayload;

	// ���Դ��vmp
	if (tSrc.GetType() == TYPE_MCUPERI && tSrc.GetEqpType() == EQP_TYPE_VMP)
	{
		tSrcSCS = m_tConf.GetMainSimCapSet();
		tSrcSCS.SetAudioMediaType(MEDIA_TYPE_NULL);
	}

	if( MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType() || 
		MEDIA_TYPE_H263PLUS == tSrcSCS.GetVideoMediaType() || 
		CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
        // zbq [10/29/2007] ��ӦFEC֧��������ʽ
        m_tConf.GetCapSupportEx().IsVideoSupportFEC())
	{
		tDVPayload.SetRealPayLoad( tSrcSCS.GetVideoMediaType() );
		tDVPayload.SetActivePayload( GetActivePayload(m_tConf, tSrcSCS.GetVideoMediaType() ) );
	}
	else
	{
		tDVPayload.SetRealPayLoad( tSrcSCS.GetVideoMediaType() );
		tDVPayload.SetActivePayload( tSrcSCS.GetVideoMediaType() );
	}
	if( CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
        // zbq [10/29/2007] ��ӦFEC֧��������ʽ
        m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
	{
		tDAPayload.SetRealPayLoad( tSrcSCS.GetAudioMediaType() );
		tDAPayload.SetActivePayload( GetActivePayload(m_tConf, tSrcSCS.GetAudioMediaType() ) );
	}
	else
	{
		tDAPayload.SetRealPayLoad( tSrcSCS.GetAudioMediaType() );
		tDAPayload.SetActivePayload( tSrcSCS.GetAudioMediaType() );
	}
		
    cServMsg.SetChnIndex( byDstChnnl );
	cServMsg.SetMsgBody( ( u8 * )&tSwitchInfo, sizeof( tSwitchInfo ) );
	cServMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
	cServMsg.CatMsgBody( (u8 *)&tDVPayload, sizeof(tDVPayload));
	cServMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
	cServMsg.CatMsgBody( (u8 *)&tDAPayload, sizeof(tDAPayload));
    
	TLogicalChannel tAudLog;
	m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(),LOGCHL_AUDIO,&tAudLog,FALSE );
    // zw [06/26/2008] ��ӦAAC LC��ʽ
	TAudAACCap tAudAACCap;
    if ( MEDIA_TYPE_AACLC == tSrcSCS.GetAudioMediaType() )
    {
        tAudAACCap.SetMediaType(MEDIA_TYPE_AACLC);
        tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
		//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
        tAudAACCap.SetChnlType(GetAACChnlTypeByAudioTrackNum(tAudLog.GetAudioTrackNum())); //[5/22/2012 chendaiwei]Ŀǰ֧��AAC_CHNL_TYPE_SINGLE��AAC_CHNL_TYPE_DOUBLE
        tAudAACCap.SetBitrate(96);
        tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
    
	    cServMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
    }
	//tianzhiyong 2010/04/02 ��ӦAAC LD��ʽ
    else if ( MEDIA_TYPE_AACLD == tSrcSCS.GetAudioMediaType() )
    {
        tAudAACCap.SetMediaType(MEDIA_TYPE_AACLD);
        tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
		//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
		tAudAACCap.SetChnlType(GetAACChnlTypeByAudioTrackNum(tAudLog.GetAudioTrackNum())); //[5/22/2012 chendaiwei]Ŀǰ֧��AAC_CHNL_TYPE_SINGLE��AAC_CHNL_TYPE_DOUBLE
        tAudAACCap.SetBitrate(96);
        tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
    
	    cServMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
    }
	else /*( MEDIA_TYPE_AACLD == tSrcSCS.GetAudioMediaType() && MEDIA_TYPE_AACLC == tSrcSCS.GetAudioMediaType() )*/
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[StartSwitchToMc] m_tConf.GetMainAudioMediaType() does'nt equals 103 or 104 !\n");
		cServMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
	}

	TTransportAddr  tVidAddr;
	TTransportAddr  tAudAddr;
	tVidAddr.SetNull();
	tAudAddr.SetNull();

	//[2011/11/09/zhangli]��ͳһ�ӿڻ�ȡ��ԭд����֧�ֵ��ش�
	//˫��RTCPͨ��, ˫����PRS֧����ͨ��PRS��������ʵ�ֵ�
	if (MODE_SECVIDEO == byMode)
	{
		TLogicalChannel tEqpLogicalChannel;
		u8 byChlNum = 0;
		u8 byPrsId = 0;
		u8 byPrsChnId = 0;
		if (FindPrsChnForBrd(MODE_SECVIDEO, byPrsId, byPrsChnId) &&
			g_cMcuVcApp.GetPeriEqpLogicChnnl(byPrsId, MODE_VIDEO, &byChlNum, &tEqpLogicalChannel, TRUE))
		{
			tVidAddr.SetIpAddr(tEqpLogicalChannel.GetRcvMediaCtrlChannel().GetIpAddr());
			tVidAddr.SetPort(tEqpLogicalChannel.GetRcvMediaCtrlChannel().GetPort() + PORTSPAN * byPrsChnId + 2);
		}
	}
	else
	{
		if (!GetRemoteRtcpAddr(tUnlocalSrc, bySrcChnnl, MODE_AUDIO, tAudAddr))
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[StartSwitchToMc]GetRemoteRtcpAddr failed\n");
		}
		
		if (!GetRemoteRtcpAddr(tUnlocalSrc, bySrcChnnl, MODE_VIDEO, tVidAddr))
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[StartSwitchToMc]GetRemoteRtcpAddr failed\n");
		}
	}


	//��ƵRTCPͨ��
	cServMsg.CatMsgBody( (u8 *)&tVidAddr, sizeof(TTransportAddr) );	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[StartSwitchToMc]tell ui monitor video rtcp port(%s, %d)\n", 
			StrOfIP(tVidAddr.GetIpAddr()), tVidAddr.GetPort());

	//��ƵRTCPͨ��
	cServMsg.CatMsgBody( (u8 *)&tAudAddr, sizeof(TTransportAddr) );	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[StartSwitchToMc]tell ui monitor video rtcp port(%s, %d)\n", 
			StrOfIP(tAudAddr.GetIpAddr()), tAudAddr.GetPort());

	SendMsgToMcs(  (u8)wMcInstId, MCU_MCS_STARTPLAY_NOTIF, cServMsg );
	
	return( TRUE );
}

/*====================================================================
    ������      ��StopSwitchToMc
    ����        ��ֹͣ�����ݽ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wMcInstId,  ʵ����
				  u8 byDstChnnl, �ŵ������ţ�ȱʡΪ0
				  BOOL32 bMsg, �Ƿ�ֹͣ������Ϣ��ȱʡΪTRUE
				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/18    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::StopSwitchToMc( u16 wMcInstId, u8 byDstChnnl, BOOL32 bMsg, u8 byMode, BOOL32 bFilterConn )
{
	CServMsg	cServMsg;
	TMt     tMtNull;
	BOOL32  bSrcAudioValid = TRUE;
	BOOL32  bSrcVideoValid = TRUE;
	u8 byStopMode = byMode;

	if( bFilterConn && !g_cMcuVcApp.IsMcConnected( wMcInstId ) )
    {
        LogPrint( LOG_LVL_WARNING, MID_MCU_VCS, "[StopSwitchToMc]Mc.%d not connected!\n", wMcInstId);
        return;
    }
	
	//[2011/11/29/zhangli]����������ͣ��أ���Դ��ʱ���ͣ��bug:��һ��vcs���飬Ȼ���ٿ�һ��vcs����¼ʧ�ܣ�����ǰһ��vcs��ر�ͣ
	//stop switch
    //g_cMpManager.StopSwitchToMc( m_byConfIdx, wMcInstId, byDstChnnl, byMode );

	// [pengjie 2010/8/4] ������ش���ȡ����أ����ջش���Դ ���Ե���
	TMt tMcSrc;
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH ) // ͣ��Ƶ
	{
		g_cMcuVcApp.GetMcSrc( wMcInstId, &tMcSrc, byDstChnnl, MODE_AUDIO );
		// [pengjie 2010/9/1] �жϸ��ն��Ƿ��Ǳ�������ն˻���ն���NULL
		if( tMcSrc.IsNull() || tMcSrc.GetConfIdx() != m_byConfIdx )
		{
			bSrcAudioValid = FALSE;
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MCS, "[StopSwitchToMc] GetMcSrc MODE_AUDIO Is Invalid, McId.%d, m_byConfIdx = %d, MtConfIdx.%d\n",
				wMcInstId, m_byConfIdx, tMcSrc.GetConfIdx() );

			byStopMode = MODE_VIDEO & byStopMode;
		}
		else
		{
			FreeRecvSpy( tMcSrc, MODE_AUDIO);
			tMtNull.SetNull();
			g_cMcuVcApp.SetMcSrc( wMcInstId, &tMtNull, byDstChnnl, MODE_AUDIO );
		}

	}

	if( byMode == MODE_VIDEO || byMode == MODE_BOTH ) // ͣ��Ƶ
	{
		g_cMcuVcApp.GetMcSrc( wMcInstId, &tMcSrc, byDstChnnl, MODE_VIDEO );

		// [pengjie 2010/9/1] �жϸ��ն��Ƿ��Ǳ�������ն˻���ն���NULL
		if( tMcSrc.IsNull() || tMcSrc.GetConfIdx() != m_byConfIdx )
		{
			bSrcVideoValid = FALSE;
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MCS, "[StopSwitchToMc] GetMcSrc MODE_VIDEO Is Invalid, McId.%d, m_byConfIdx = %d, MtConfIdx.%d\n",
				wMcInstId, m_byConfIdx, tMcSrc.GetConfIdx() );

			byStopMode = MODE_AUDIO & byStopMode;
		}
		else
		{
			FreeRecvSpy( tMcSrc, MODE_VIDEO);
			tMtNull.SetNull();
			g_cMcuVcApp.SetMcSrc( wMcInstId, &tMtNull, byDstChnnl, MODE_VIDEO );
		}
	}
	// End

	g_cMpManager.StopSwitchToMc(m_byConfIdx, wMcInstId, byDstChnnl, byStopMode);
	
	if( byMode == MODE_BOTH && !bSrcAudioValid && !bSrcVideoValid )
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_MCS, "[StopSwitchToMc] Src is invalid, so return here! Mode:BOTH\n");
		return;
	}
	if( byMode == MODE_AUDIO && !bSrcAudioValid )
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_MCS, "[StopSwitchToMc] Src is invalid, so return here! Mode:AUDIO\n");
		return;
	}
	if( byMode == MODE_VIDEO && !bSrcVideoValid )
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_MCS, "[StopSwitchToMc] Src is invalid, so return here! Mode:VIDEO\n");
		return;
	}	


	if( (byMode == MODE_AUDIO || byMode == MODE_BOTH) &&  bSrcAudioValid )
	{
		tMtNull.SetNull();
		g_cMcuVcApp.SetMcSrc( wMcInstId, &tMtNull, byDstChnnl, MODE_AUDIO );
	}

	if( (byMode == MODE_VIDEO || byMode == MODE_BOTH) &&  bSrcVideoValid )
	{
		tMtNull.SetNull();
		g_cMcuVcApp.SetMcSrc( wMcInstId, &tMtNull, byDstChnnl, MODE_VIDEO );
	}
	

	//send stop message
	if( bMsg )
	{
		TSwitchInfo tSwitchInfo;
		tSwitchInfo.SetDstChlIdx( byDstChnnl );
		tSwitchInfo.SetMode( byMode );
		cServMsg.SetChnIndex( byDstChnnl );
		cServMsg.SetMsgBody( ( u8 * )&tSwitchInfo, sizeof( tSwitchInfo ) );
		SendMsgToMcs(  (u8)wMcInstId, MCU_MCS_STOPPLAY_NOTIF, cServMsg );
	}
}

/*====================================================================
    ������      ��StopSwitchToAllMcInConf
    ����        ��ֹͣ�����ݽ����������տ��������ն˵Ļ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bMsg, �Ƿ�ֹͣ������Ϣ��ȱʡΪTRUE
				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/28    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::StopSwitchToAllMcInConf( BOOL32 bMsg, u8 byMode )
{	
// 	u8  byLoop;
// 	TMt tMt;
// 
// 	for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
// 	{
// 		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
// 		{
// 			tMt = m_ptMtTable->GetMt( byLoop );
// 			StopSwitchToAllMcWatchingSrcMt( tMt, bMsg, byMode );	
// 		}
// 	}

	TMt		tCurSrc;
	u8      byIndex;
	u8      byChannel;
	u8      byChannelNum;
	TLogicalChannel tLogicalChannel;
	
	for( byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( !g_cMcuVcApp.IsMcConnected( byIndex ) )
			continue;
        
		g_cMcuVcApp.GetMcLogicChnnl( byIndex, MODE_VIDEO, &byChannelNum, &tLogicalChannel );
		
		for( byChannel = 0; byChannel < byChannelNum; byChannel++ )
		{
			if( ( byMode == MODE_VIDEO || byMode == MODE_BOTH ) 
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_VIDEO )
				&& tCurSrc.GetConfIdx() == m_byConfIdx
				)
			{
				StopSwitchToMc( byIndex, byChannel, bMsg, MODE_VIDEO );
			}
			
			if( ( byMode == MODE_AUDIO || byMode == MODE_BOTH ) 
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_AUDIO )
				&& tCurSrc.GetConfIdx() == m_byConfIdx
				)
			{
				StopSwitchToMc( byIndex, byChannel, bMsg, MODE_AUDIO );
			}
		}
	}
}

/*====================================================================
    ������      ��AdjustSwitchToAllMcWatchingSrcMt
    ����        ���������ݽ����������ѡ��ý��ԴΪָ���ն˵�ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt & tSrc, ָ���ն� 
				  BOOL32 bMsg, �Ƿ�ֹͣ������Ϣ��ȱʡΪTRUE
				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
				  BOOL32 bIsStop���ǽ��������ǲ������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/28    1.0         JQL           ����
====================================================================*/
void CMcuVcInst::AdjustSwitchToAllMcWatchingSrcMt(TMt tSrc, BOOL32 bMsg, u8 byMode, BOOL32 bIsStop/*=TRUE*/,u8 byChl /*= 0xFF*/ )
{
	TMt tCurSrc;
	u8      byIndex;
	u8      byChannel;
	u8      byChannelNum;
	TLogicalChannel tLogicalChannel;
	

	CRecvSpy tSpyResource;
	u16 wSpyPort = SPY_CHANNL_NULL;	
	
	for( byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( !g_cMcuVcApp.IsMcConnected( byIndex ) )
			continue;
        
		g_cMcuVcApp.GetMcLogicChnnl( byIndex, MODE_VIDEO, &byChannelNum, &tLogicalChannel );
		
		for( byChannel = 0; byChannel < byChannelNum; byChannel++ )
		{
			if( ( byMode == MODE_VIDEO || byMode == MODE_BOTH ) && ( 0xff == byChl || byChannel == byChl )
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_VIDEO )
				&& ( tCurSrc == tSrc || ( IsMcu(tSrc) && IsMtInMcu(tSrc, tCurSrc) ) ) // [9/10/2010 xliang] ?��??��?��?????mcu??��??????��D?
				)
			{
				if( bIsStop )
				{
					if( bMsg )
					{
						StopSwitchToMc( byIndex, byChannel, bMsg, MODE_VIDEO );
					}
					else
					{
						g_cMpManager.StopSwitchToMc(m_byConfIdx, byIndex, byChannel, MODE_VIDEO);
					}
				}
				else
				{
					if( m_cSMcuSpyMana.GetRecvSpy( tCurSrc, tSpyResource ) )
					{
						wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
					}
					if( bMsg )
					{						
						
						StartSwitchToMc( tCurSrc,0,byIndex,byChannel,MODE_VIDEO,wSpyPort );
					}
					else
					{
						g_cMpManager.StartSwitchToMc(tCurSrc, 0, byIndex, byChannel, MODE_VIDEO, wSpyPort);
					}
				}				
			}
			
			if( ( byMode == MODE_AUDIO || byMode == MODE_BOTH ) && ( 0xff == byChl || byChannel == byChl )
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_AUDIO )
				&& ( tCurSrc == tSrc || ( IsMcu(tSrc) && IsMtInMcu(tSrc, tCurSrc) ) )
				)
			{
				

				if( bIsStop )
				{
					if( bMsg )
					{						
						StopSwitchToMc( byIndex, byChannel, bMsg, MODE_AUDIO );
					}
					else
					{
						g_cMpManager.StopSwitchToMc(m_byConfIdx, byIndex, byChannel, MODE_AUDIO);
					}
				}
				else
				{
					if( m_cSMcuSpyMana.GetRecvSpy( tCurSrc, tSpyResource ) )
					{
						wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
					}
					if( bMsg )
					{	
						StartSwitchToMc( tCurSrc,0,byIndex,byChannel,MODE_AUDIO,wSpyPort );
					}
					else
					{
						g_cMpManager.StartSwitchToMc(tCurSrc, 0, byIndex, byChannel, MODE_AUDIO, wSpyPort);
					}
				}		
			}
		}
	}
}

/*====================================================================
    ������      ��StopSwitchToAllMcWatchingSrcMcu
    ����        ��ֹͣ�����ݽ����������ѡ��ý��Դ����ָ��MCU��ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const u16 wMcuId, ָ��MCU
				  BOOL32 bMsg, �Ƿ�ֹͣ������Ϣ��ȱʡΪTRUE
				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/28    1.0         JQL           ����
====================================================================*/
void CMcuVcInst::StopSwitchToAllMcWatchingSrcMcu( const u16 wMcuId, BOOL32 bMsg, u8 byMode )
{
	TMt tCurSrc;
	u8      byIndex;
	u8      byChannel;
	u8      byChannelNum;
	TLogicalChannel tLogicalChannel;

	for( byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( !g_cMcuVcApp.IsMcConnected( byIndex ) )
			continue;
        
		g_cMcuVcApp.GetMcLogicChnnl( byIndex, MODE_VIDEO, &byChannelNum, &tLogicalChannel );
		
		for( byChannel = 0; byChannel < byChannelNum; byChannel++ )
		{
			if( ( byMode == MODE_VIDEO || byMode == MODE_BOTH ) 
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_VIDEO )
				&& tCurSrc.GetMcuId() == wMcuId )
				StopSwitchToMc( byIndex, byChannel, bMsg, MODE_VIDEO );

			if( ( byMode == MODE_AUDIO || byMode == MODE_BOTH ) 
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_AUDIO )
				&& tCurSrc.GetMcuId() == wMcuId )
				StopSwitchToMc( byIndex, byChannel, bMsg, MODE_AUDIO );
		}
	}
}

/*====================================================================
    ������      ��SwitchSrcToDst
    ����        ����������Դ������Ŀ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TSwitchInfo &tSwitchInfo, ������Ϣ
	              const CServMsg & cServMsgHdr,	ҵ����Ϣͷ
				  bBatchPollChg
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/01/04    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::SwitchSrcToDst( const TSwitchInfo &tSwitchInfo,
                                 const CServMsg & cServMsgHdr,
                                 BOOL32 bBatchPollChg,
								 const THduSwitchInfo *ptHduSwitchInfo,
								 const u8 byMulitModeHdu)
{
	CServMsg	cServMsg;
	TMt	tDstMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetDstMt());
	TMt tSrcMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetSrcMt());
	u8	byMode = tSwitchInfo.GetMode();
	u8	bySrcChnnl = tSwitchInfo.GetSrcChlIdx();// xliang [1/7/2009] Ŀǰ����ѡ��VMPʱ����

	//zjj20100201
	//  [11/17/2009 pengjie] Modify ������ش�����
	TMt tUnlocalSrcMt = tSwitchInfo.GetSrcMt();
	TMt tUnlocalDstMt = tSwitchInfo.GetDstMt();
	CRecvSpy tSpyResource;
	u16 wSpyPort = SPY_CHANNL_NULL;	
	if( m_cSMcuSpyMana.GetRecvSpy( tUnlocalSrcMt, tSpyResource ) )
	{
		wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
	}
	// End Modify

	//set initial message header
	cServMsg.SetServMsg( ( u8 * )cServMsgHdr.GetServMsg(), cServMsgHdr.GetServMsgLen() );
            
    NotifyMtSend( tSrcMt.GetMtId(), byMode );

    //��kdc�����й㲥Դ�ҽ�ʡ����ʱ��֪ͨmtֹͣ��������
    if(g_cMcuVcApp.IsSavingBandwidth() && 
        MT_MANU_KDC != m_ptMtTable->GetManuId( tDstMt.GetMtId()) &&
        MT_MANU_KDCMCU != m_ptMtTable->GetManuId( tDstMt.GetMtId()))
    {
        NotifyOtherMtSend(tDstMt.GetMtId(), FALSE);
    }

	switch( cServMsgHdr.GetEventId() )
	{
	case MT_MCU_STARTSELMT_REQ:
	case MT_MCU_STARTSELMT_CMD:
	case MCS_MCU_STARTSWITCHMT_REQ:
		{
			//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
			//zjj20091023 ѡ��������
			//g_cMpManager.SetSwitchBridge(tSrcMt, 0, byMode);
			//StartSwitchToSubMt(tUnlocalSrcMt, 0, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE, FALSE, FALSE, wSpyPort);
			TSwitchGrp tSwitchGrp;
			tSwitchGrp.SetSrcChnl(0);
			tSwitchGrp.SetDstMtNum(1);
			tSwitchGrp.SetDstMt(&tDstMt);
			//zhouyiliang 20120924 ��ϯ��ѯģʽ�£����ڲ�ѡ��������Ҫ����mcsdragmod
			BOOL32 bInnerSelect = FALSE;
			if ( cServMsgHdr.GetSrcSsnId() == 0   )		
			{
				bInnerSelect = TRUE;
			}
			StartSwitchToAll(tUnlocalSrcMt, 1, &tSwitchGrp, byMode, SWITCH_MODE_SELECT, TRUE, FALSE, wSpyPort,bInnerSelect);
		}		
		break;
	case MCS_MCU_STARTSWITCHVMPMT_REQ:// xliang [1/7/2009] ��ϯѡ��VMP 
	case MT_MCU_STARTSWITCHVMPMT_REQ:	// xliang [4/9/2009] MT ������ѡ��
		{
			g_cMpManager.SetSwitchBridge(tSrcMt, bySrcChnnl, byMode); // xliang [4/2/2009] ���߹㲥�߼���û��Ҫ���ţ�// xliang [4/28/2009] �䲻�߹㲥�߼�������Ϊ�㲥ԴҪ����
			//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
			//StartSwitchToSubMt(tSrcMt, bySrcChnnl, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE, FALSE);
			TSwitchGrp tSwitchGrp;
			tSwitchGrp.SetSrcChnl(bySrcChnnl);
			tSwitchGrp.SetDstMtNum(1);
			tSwitchGrp.SetDstMt(&tDstMt);
			StartSwitchToAll(tSrcMt, 1, &tSwitchGrp, byMode, SWITCH_MODE_SELECT, TRUE, FALSE);
		}		
		break;
	case MCS_MCU_STARTSWITCHMC_REQ:
		StartSwitchToMc(tUnlocalSrcMt, bySrcChnnl, cServMsgHdr.GetSrcSsnId(),tSwitchInfo.GetDstChlIdx(), byMode,wSpyPort);
		break;
	case MCS_MCU_START_SWITCH_TW_REQ:
		//[2011/08/18/zhangli]���һ��������FALSE
		StartSwitchToPeriEqp(tUnlocalSrcMt, 0, tDstMt.GetEqpId(), tSwitchInfo.GetDstChlIdx(), byMode, SWITCH_MODE_SELECT, wSpyPort, TRUE, FALSE, FALSE);
		break;
	case MCS_MCU_START_SWITCH_HDU_REQ: // xliang [6/23/2009] ��2����������0����Ϊ���ܿ�VMP����ʱSrcChnnl�ɾ����������
		// [2013/03/11 chenbing] ���HDU��ͨ����ͨ����� 
		if (HDUCHN_MODE_FOUR == byMulitModeHdu && NULL != ptHduSwitchInfo)
		{
			StartSwitchToPeriEqp(tUnlocalSrcMt, bySrcChnnl, tDstMt.GetEqpId(), tSwitchInfo.GetDstChlIdx(),
				byMode, SWITCH_MODE_SELECT, FALSE, TRUE, bBatchPollChg, FALSE, TRUE, ptHduSwitchInfo->GetDstSubChn(), byMulitModeHdu );
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[SwitchSrcToDst] ptHduSwitchInfo is NULL!!!\n" );
			StartSwitchToPeriEqp(tUnlocalSrcMt, bySrcChnnl, tDstMt.GetEqpId(), tSwitchInfo.GetDstChlIdx(),
				byMode, SWITCH_MODE_SELECT, FALSE, TRUE, bBatchPollChg, FALSE);
		}
		break;
	default:

		ConfPrint(LOG_LVL_WARNING, MID_MCU_MCS, "Exception - wrong switch eventid %u!\n", cServMsgHdr.GetEventId() );
		return;
	}


	if ( TYPE_MT == tSrcMt.GetType())
	{
		NotifyMtFastUpdate( tSrcMt.GetMtId(), MODE_VIDEO );
	}
	// ����vmp,ֻ�����Ӧͨ���ؼ�֡
	else if (tSrcMt.GetEqpType() == EQP_TYPE_VMP)
	{
		NotifyFastUpdate(tSrcMt, bySrcChnnl);
	}
    else if ( TYPE_MCUPERI == tSrcMt.GetType() )
    {
		NotifyFastUpdate( tSrcMt, 0 );
		NotifyFastUpdate( tSrcMt, 1 );
		NotifyFastUpdate( tSrcMt, 2 );
    }
	else
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_MCS, "Wrong type :tSrcMt.GetType = %d\n" ,tSrcMt.GetType() );
	}
	
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (tConfAttrb.IsResendLosePack()/* && MODE_AUDIO != byMode*/)
	{
		if( MCS_MCU_START_SWITCH_HDU_REQ == cServMsgHdr.GetEventId() 
			|| MCS_MCU_START_SWITCH_TW_REQ == cServMsgHdr.GetEventId()
			|| MCS_MCU_STARTSWITCHMC_REQ == cServMsgHdr.GetEventId())
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_MCS, "[SwitchSrcToDst] HDU or TW'S or mc RTCP don't build here!\n" );
			return;
		}
		
		u8 byRtpDstChnnl = 0;
		if (TYPE_MT == tDstMt.GetType())
        {
            byRtpDstChnnl = 0;
        }
        else if (TYPE_MCUPERI == tDstMt.GetType())
        {
            byRtpDstChnnl = tSwitchInfo.GetDstChlIdx();
        }
		BuildRtcpSwitchForSrcToDst(tUnlocalDstMt, tUnlocalSrcMt, byMode, byRtpDstChnnl, bySrcChnnl, TRUE);
	}

	return;
}

/*=============================================================================
�� �� ���� AdjustSwitchBridge
��    �ܣ� mp�������ע��ʱ������ǰ�Ž���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMpId ��������ע���mp��id
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/9/15  4.0			������                  ����
=============================================================================*/
BOOL32 CMcuVcInst::AdjustSwitchBridge(u8 byMpId, BOOL32 bInsert)
{
    //����ʱ��ɾ��ǰһ��mp���Ž���
    if (!bInsert)
    {
        u8 byTmpMpId = byMpId-1;
        while(byTmpMpId > 0)
        {
            if (g_cMcuVcApp.IsMpConnected(byTmpMpId))
            {
                break;
            }
            byTmpMpId--;
        }

        //ǰ��û��mp��
        if (byTmpMpId == 0)    
        {
            byTmpMpId = MAXNUM_DRI;
            while(byTmpMpId > byMpId)
            {
                if (g_cMcuVcApp.IsMpConnected(byTmpMpId))
                {
                    break;
                }
                byTmpMpId--;
            }

            //ֻ����һ��mp��
            if (byTmpMpId == byMpId)
            {
                return TRUE;
            }        
        }

        //�ҵ��˱仯mpidǰ���mp��id
        if (byTmpMpId > 0 && byTmpMpId <= MAXNUM_DRI)
        {        
            u32 dwInsertMpIp = g_cMcuVcApp.GetMpIpAddr(byMpId);
            u16 wTableId = 0;
	        while( wTableId < MAX_SWITCH_CHANNEL )
	        {
		        TSwitchChannel *ptSwitchChnnl = m_ptSwitchTable->GetSwitchChannel(byTmpMpId, wTableId);
                if (!ptSwitchChnnl->IsNull())
                {
                    TMt tSrc = ptSwitchChnnl->GetSrcMt();                
                    // guzh [7/3/2007] �жϽ����Ƿ���MP����ȷ�Ž���
                    if ( ptSwitchChnnl->GetRcvPort() == ptSwitchChnnl->GetDstPort() && 
                         //g_cMcuVcApp.FindMp( ptSwitchChnnl->GetDstIP() ) != 0 &&
                         !tSrc.IsNull() )
                    {                        
                        //�ҵ�һ����·�������byMPId����
                        g_cMpManager.StopSwitch(tSrc.GetConfIdx(), ptSwitchChnnl->GetDstIP(), ptSwitchChnnl->GetRcvPort());                        
                    }
                }
                wTableId++;
            }
        }
    }

    // xsl ���½���
    u8 bySrcChnnl;
    
	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();
    //video�㲥Դ
    if (!tLocalVidBrdSrc.IsNull())
    {
        bySrcChnnl = (tLocalVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;

		//[2011/09/02/zhangli]�����Ƶ���������¼��նˣ���Ҫ��30000�ش��˿ڵĽ���
		u16 dSpyPort = SPY_CHANNL_NULL;
		CRecvSpy tSpyResource;

		if (!tLocalVidBrdSrc.IsLocal() && m_cSMcuSpyMana.GetRecvSpy(tLocalVidBrdSrc, tSpyResource))
		{
			dSpyPort  = tSpyResource.m_tSpyAddr.GetPort();
		}

        g_cMpManager.SetSwitchBridge(tLocalVidBrdSrc, bySrcChnnl, MODE_VIDEO, FALSE, dSpyPort);
    
        if (tLocalVidBrdSrc == m_tVmpEqp)
        {
			TPeriEqpStatus tPeriEqpStatus;	
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
			u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

			if (((0 != m_tConf.GetSecBitRate() || MEDIA_TYPE_NULL != m_tConf.GetCapSupport().GetSecondSimCapSet().GetVideoMediaType()))
				&& VMP == byVmpSubType)
			{
				g_cMpManager.SetSwitchBridge(tLocalVidBrdSrc, bySrcChnnl+1, MODE_VIDEO);
			}
			/*[2011/09/02/zhangli]vmp��������svmp���ĳ�
			if (MPU_SVMP == byVmpSubType || VMP_8KE == byVmpSubType)
			{
				g_cMpManager.SetSwitchBridge(tLocalVidBrdSrc, bySrcChnnl+1, MODE_VIDEO);
				g_cMpManager.SetSwitchBridge(tLocalVidBrdSrc, bySrcChnnl+2, MODE_VIDEO);
				g_cMpManager.SetSwitchBridge(tLocalVidBrdSrc, bySrcChnnl+3, MODE_VIDEO);
			}*/
			if (VMP != byVmpSubType)
			{
				u8 byMaxOutChnl = MAXNUM_MPU_OUTCHNNL;// MPU����vmp��4·������,8KG��4·������,8KH��5·������
				// MPU2����vmp��9·������
				if (MPU2_VMP_ENHACED == byVmpSubType ||
					MPU2_VMP_BASIC == byVmpSubType)
				{
					TKDVVMPOutParam tVMPOutParam = g_cMcuVcApp.GetVMPOutParam(m_tVmpEqp);
					byMaxOutChnl = tVMPOutParam.GetVmpOutCount();//����ݺ��������ȷ������
				}
				//����������
				for (bySrcChnnl = 1; bySrcChnnl < byMaxOutChnl; bySrcChnnl++)
				{
					g_cMpManager.SetSwitchBridge(tLocalVidBrdSrc, bySrcChnnl, MODE_VIDEO); 
				}

				//pgf:��ʵ�����������Ժϲ�
                TVideoStreamCap tStrCap = m_tConf.GetMainSimCapSet().GetVideoCap();
                // [1/19/2011 xliang] �鲥�����鲥����
                if (m_tConf.GetConfAttrb().IsMulticastMode())
                {
                    bySrcChnnl = GetVmpOutChnnlByRes(m_tVmpEqp.GetEqpId(),
						tStrCap.GetResolution(), 
                        tStrCap.GetMediaType(),
                        tStrCap.GetUserDefFrameRate(),
                        tStrCap.GetH264ProfileAttrb());
                    g_cMpManager.StartMulticast(tLocalVidBrdSrc, bySrcChnnl, MODE_VIDEO);
                    m_ptMtTable->SetMtMulticasting(tLocalVidBrdSrc.GetMtId());
                }
                
                //  [3/2/2012 pengguofeng]������ǻ��齻��
                if ( m_tConf.GetConfAttrb().IsSatDCastMode() )
                {
                    bySrcChnnl = GetVmpOutChnnlByRes(m_tVmpEqp.GetEqpId(),
						tStrCap.GetResolution(), 
                        tStrCap.GetMediaType(),
                        tStrCap.GetUserDefFrameRate(),
                        tStrCap.GetH264ProfileAttrb());
                    g_cMpManager.StartDistrConfCast(tLocalVidBrdSrc, MODE_VIDEO, bySrcChnnl);
                }
			}
        }
        else
        {
            //����Ƶ�鲥����
            if ( m_tConf.GetConfAttrb().IsSatDCastMode() )
            {
                g_cMpManager.StartDistrConfCast(tLocalVidBrdSrc, MODE_VIDEO, bySrcChnnl);
            }
        }
    }        

	TMt tLocalAudBrdSrc = GetLocalAudBrdSrc();
    //audio�㲥Դ
    if (!tLocalAudBrdSrc.IsNull())
    {
        bySrcChnnl = (tLocalAudBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;            
		u16 dSpyPort = SPY_CHANNL_NULL;
		CRecvSpy tSpyResource;
		
		if (!tLocalAudBrdSrc.IsLocal() && m_cSMcuSpyMana.GetRecvSpy(tLocalAudBrdSrc, tSpyResource))
		{
			dSpyPort  = tSpyResource.m_tSpyAddr.GetPort();
		}

		TAudioTypeDesc atAudTypeInfo[MAXNUM_CONF_AUDIOTYPE];
		u8 byAudCapNum = 1;
		if ( tLocalAudBrdSrc.GetEqpType() == EQP_TYPE_MIXER )
		{
			bySrcChnnl = GetMixerNModeChn();
			byAudCapNum = m_tConfEx.GetAudioTypeDesc(atAudTypeInfo);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[AdjustSwitchBridge eMix AudBrdSrc]:EQPID=[%d]NmodeChnIndx=[%d]\n",m_tMixEqp.GetEqpId(),GetMixerNModeChn());
		}
		for (u8 byLoop = 0; byLoop < byAudCapNum; byLoop++)
		{
			g_cMpManager.SetSwitchBridge(tLocalAudBrdSrc, bySrcChnnl + byLoop, MODE_AUDIO, FALSE, dSpyPort);
		}
		//���Ƿ�ɢ���飬�����鲥������ת������ߣ����ﲹ����ת��������ʱҲ���ؽ�һ��
        //���鲥����������֧���鲥��ת���壬���û�����ɵ�һ��ת���彨
        if ( m_tConf.GetConfAttrb().IsSatDCastMode() )
        {
            g_cMpManager.StartDistrConfCast(tLocalAudBrdSrc, MODE_AUDIO, bySrcChnnl);
        }
    }        

	if (m_tConf.m_tStatus.IsAudAdapting())
	{
		u8 byNum = 0;
		CBasChn *apcBasChn[MAXNUM_PERIEQP]={NULL};
		u8 byOutNum = 0;

		if(GetBasBrdChnGrp(byNum, apcBasChn, MODE_AUDIO))
		{
			for (u8 byIdx = 0; byIdx < byNum; byIdx++)
			{
				if ( NULL == apcBasChn[byIdx] )
				{
					continue;
				}
				TEqp tBas = apcBasChn[byIdx]->GetBas();
				u8 byBasChnl = apcBasChn[byIdx]->GetChnId();
				
				//������û��Ҫ������
				if ( !g_cMcuVcApp.IsPeriEqpConnected(tBas.GetEqpId()) )
				{
					continue;
				}

				byOutNum = 0;
				u8 byFrontOutNum = 0;
				if ( !GetBasChnOutputNum(tBas,byBasChnl,byOutNum,byFrontOutNum) )
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[AdjustSwitchBridge] GetBasChnOutputNum:%d-%d failed!\n",
							tBas.GetEqpId(), byBasChnl
							);
					return FALSE;
				}

				for(u8 byOutIdx = 0; byOutIdx < byOutNum; ++byOutIdx)
				{
					g_cMpManager.SetSwitchBridge(tBas, byFrontOutNum + byOutIdx, MODE_AUDIO);
				}
			}
		}
	}
	
	if (m_tConf.m_tStatus.IsVidAdapting())
	{
		u8 byNum = 0;
		CBasChn *apcBasChn[MAXNUM_PERIEQP]={NULL};
		
		u8 byOutNum = 0;
		
		if(GetBasBrdChnGrp(byNum, apcBasChn, MODE_VIDEO))
		{
			for (u8 byIdx = 0; byIdx < byNum; byIdx++)
			{
				if ( NULL == apcBasChn[byIdx] )
				{
					continue;
				}
				TEqp tBas = apcBasChn[byIdx]->GetBas();
				tBas.SetConfIdx(m_byConfIdx);

				u8 byBasChnl = apcBasChn[byIdx]->GetChnId();
				//������û��Ҫ������
				if ( !g_cMcuVcApp.IsPeriEqpConnected(tBas.GetEqpId()) )
				{
					continue;
				}
				byOutNum = 0;
				u8 byFrontOutNum = 0;
				if ( !GetBasChnOutputNum(tBas,byBasChnl,byOutNum,byFrontOutNum) )
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[AdjustSwitchBridge] GetBasChnOutputNum:%d-%d failed!\n",
						tBas.GetEqpId(), byBasChnl
						);
					return FALSE;
				}

				for(u8 byOutIdx = 0; byOutIdx < byOutNum; ++byOutIdx)
				{
					//g_cMpManager.StopSwitch(m_byConfIdx, ptSwitchChnnl->GetDstIP(), ptSwitchChnnl->GetRcvPort());

					g_cMpManager.SetSwitchBridge(tBas, byFrontOutNum + byOutIdx, MODE_VIDEO);
					if (g_cMcuVcApp.IsVASimultaneous())
					{
						g_cMpManager.SetSwitchBridge(tBas, byFrontOutNum + byOutIdx, MODE_AUDIO);
					}
				}
			}
		}
	} 

    //˫��Դ
    if (!m_tDoubleStreamSrc.IsNull())
    {
        // zbq [09/04/2007] ��¼���˫�����󣬰���Ӧ�˿ڽ���
        u8 bySrcChn = m_tPlayEqp == m_tDoubleStreamSrc ? m_byPlayChnnl : 0;        
        g_cMpManager.SetSwitchBridge(m_tDoubleStreamSrc, bySrcChn, MODE_SECVIDEO);

        //���Ƿ�ɢ���飬�����鲥������ת������ߣ����ﲹ����ת��������ʱҲ���ؽ�һ��
        //���鲥����������֧���鲥��ת���壬���û�����ɵ�һ��ת���彨
        if ( m_tConf.GetConfAttrb().IsSatDCastMode() )
        {
            g_cMpManager.StartDistrConfCast(m_tDoubleStreamSrc, MODE_SECVIDEO, bySrcChn);
        }
    }


	//[2011/09/02/zhangli]ѡ���ͻش��������Ž���������������账��
//     //ѡ��Դ
//     TMtStatus tStatus;
//     TMt tTmpMt;
// 	for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
//     {
//         if (!m_tConfAllMtInfo.MtJoinedConf(byLoop))
//         {
//             continue;
//         }      
// 		
// 		if (m_ptMtTable->GetMtStatus(byLoop, &tStatus))
//         {
//             tTmpMt = tStatus.GetSelectMt(MODE_VIDEO);
//             if (!tTmpMt.IsNull() && m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMtId()))
//             {               
//                 g_cMpManager.SetSwitchBridge(tTmpMt, 0, MODE_VIDEO);
//             }
// 
//             tTmpMt = tStatus.GetSelectMt(MODE_AUDIO);
//             if (!tTmpMt.IsNull() && m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMtId()))
//             {               
//                 if (!m_tConf.m_tStatus.IsMixing())
//                 {
// 					//tianzhiyong 2010/03/21 �жϹ㲥Դ�Ƿ�ΪEAPU������
// 					if ( tTmpMt.GetEqpType() == EQP_TYPE_EMIXER )
// 					{
// 						g_cMpManager.SetSwitchBridge(tTmpMt, GetMixerNModeChn(), MODE_AUDIO);
// 						ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[AdjustSwitchBridge eMix SeeSrc]:EQPID=[%d]NmodeChnIndx=[%d]\n",tTmpMt.GetEqpId(),GetMixerNModeChn());
// 					}
// 					else
// 					{
// 						g_cMpManager.SetSwitchBridge(tTmpMt, 0, MODE_AUDIO);
// 					}
//                 }                    
//             }
//         }
//     }
// 
//     //�ش��ն�
// 	if(m_tCascadeMMCU.GetMtId() != 0)
// 	{
// 		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
// 		if( NULL != ptConfMcInfo )
// 		{
// 			tTmpMt = ptConfMcInfo->m_tSpyMt;
// 		}
// 		if( ptConfMcInfo != NULL && !tTmpMt.IsNull() &&
//             m_ptMtTable->GetMtSndBitrate(tTmpMt.GetMtId()) > 0 )
// 		{
//             u8 byMode = MODE_BOTH;
//             if( TYPE_MT == m_ptMtTable->GetMainType( tTmpMt.GetMtId() ) && 
// 		        MT_MANU_KDC != m_ptMtTable->GetManuId( tTmpMt.GetMtId() ) ) 
//             {
//                 if (m_ptMtTable->IsMtAudioDumb(tTmpMt.GetMtId()) || 
//                     m_ptMtTable->IsMtAudioMute(tTmpMt.GetMtId()))
//                 {
//                     byMode = MODE_VIDEO;
// 					g_cMpManager.SetSwitchBridge(tTmpMt, 0, byMode);
//                 }
//             }
//             
//             if (m_tConf.m_tStatus.IsMixing())
//             {
//                 byMode = MODE_AUDIO;
// 				//tianzhiyong 2010/03/21 �жϹ㲥Դ�Ƿ�ΪEAPU������
// 				if ( tTmpMt.GetEqpType() == EQP_TYPE_EMIXER )//Ϊ������
// 				{
// 					g_cMpManager.SetSwitchBridge(tTmpMt, GetMixerNModeChn(), MODE_AUDIO);
// 					OspPrintf(TRUE,FALSE,"[AdjustSwitchBridge eMix MMcu]:EQPID=[%d]NmodeChnIndx=[%d]\n",tTmpMt.GetEqpId(),GetMixerNModeChn());
// 				}
// 				else
// 				{
// 					g_cMpManager.SetSwitchBridge(tTmpMt, 0, MODE_AUDIO);
// 				}
//             }
// 		        
// 		}		    
// 	}

    return TRUE;    
}

/*====================================================================
������      ��Proc8khmMultiSwitch
����        ������8000H-M���鲥��ַ�л�
�㷨ʵ��    �����ݽ�����������
����ȫ�ֱ�����
�������˵������
����ֵ˵��  ����
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013-6-6    4.7         �����          ����
====================================================================*/
void CMcuVcInst::Proc8khmMultiSwitch()
{
#ifdef _8KH_
	if ( !g_cMcuAgent.Is8000HmMcu() )
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[Proc8khmMultiSwitch]non 8khm not need to handle\n");
		return;
	}
	
	if ( !m_tConf.GetConfAttrb().IsMulticastMode() )
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[Proc8khmMultiSwitch]non multicast not need to handle\n");
		return;
	}
	
	TConfSwitchTable *ptSwitchTable = g_cMcuVcApp.GetConfSwitchTable( m_byConfIdx );
	if ( !ptSwitchTable)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[Proc8khmMultiSwitch]non switch table in conf:%d not need to handle\n", m_byConfIdx);
		return;
	}
	
	u8 byMpId = 16;
	u32 dwMultiCastAddr = g_cMcuVcApp.AssignMulticastIp(m_byConfIdx);
	
	for( u16 wLoop = 0; wLoop < MAX_SWITCH_CHANNEL; wLoop++ )
	{
		TSwitchChannel *ptSwitchChannel = ptSwitchTable->GetSwitchChannel( byMpId, wLoop );
		if( NULL == ptSwitchChannel || ptSwitchChannel->IsNull() )
		{
			continue;
		}
		
		if ( ptSwitchChannel->GetDstIP() != dwMultiCastAddr)
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[switch idx:%d]dstip:%x not the multicast IP\n", wLoop, ptSwitchChannel->GetDstIP() );
			continue;
		}
		
		u32 dwSndBindIp = ptSwitchChannel->GetSndBindIP();
		u32 dwModSndBindIp = GetSwitchSndBindIp();
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[Proc8khmMultiSwitch]SndBindIp is %x will change to :%x\n", dwSndBindIp, dwModSndBindIp);
		
		if ( dwModSndBindIp == 0 )
		{
			continue; //ȡ����0����û��Ҫ�޸�ԭ���Ľ���
		}
		
		//1.stop multicast switch:ԭ������ֱ�ӽ���MP���Ѿ����ڴ˽�������ֱ�ӻ�ACK����������
		g_cMpManager.StartStopSwitch(m_byConfIdx, ptSwitchChannel->GetDstIP(), ptSwitchChannel->GetDstPort(), byMpId);
		
		//2.start multicast switch
		CServMsg cServMsg;
		cServMsg.SetConfIdx(m_byConfIdx);
		cServMsg.SetDstDriId(byMpId);
		TSwitchChannelExt tSwitchChannelExt;
		tSwitchChannelExt.SetNull();
		memcpy(&tSwitchChannelExt, ptSwitchChannel, sizeof(TSwitchChannel));
		tSwitchChannelExt.SetSndBindIP(dwModSndBindIp);
		cServMsg.SetMsgBody((u8 *)&tSwitchChannelExt, sizeof(TSwitchChannelExt));
		
		
		u8 bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
		u8 byUniformPayload = INVALID_PAYLOAD;
		u8 byIsSwitchRtcp = 1;
		cServMsg.CatMsgBody((u8 *)&bySwitchChannelMode, sizeof(u8));
		
		cServMsg.CatMsgBody((u8 *)&byUniformPayload, sizeof(u8));
		
		cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));
		
		g_cMpSsnApp.SendMsgToMpSsn(byMpId, MCU_MP_ADDSWITCH_REQ, 
			cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[Proc8khmMultiSwitch]send MCU_MP_ADDSWITCH_REQ to MP:16\n");
		break;
	}
	
#endif
	return;
}

/*====================================================================
    ������      ��ShowConfSwitch
    ����        ����ӡ���齻����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/10    3.0         ������          ����
====================================================================*/
void CMcuVcInst::ShowConfSwitch( void )
{
	u8 byMpId, byLoop1;
    u16 wLoop2;
	u32 dwSrcIp, dwDisIp, dwRcvIp, dwDstIp;
	u16 wRcvPort, wDstPort;
	TSwitchChannel *ptSwitchChannel;
	char achTemp[255];
	int l;
	
	for( byLoop1 = 1; byLoop1 <= MAXNUM_DRI; byLoop1++ )
	{
		if( g_cMcuVcApp.IsMpConnected( byLoop1 ) )
		{            
			StaticLog("\nMp%d(%s) switch info: ", byLoop1 , StrOfIP(g_cMcuVcApp.GetMpIpAddr(byLoop1)) );
			for( wLoop2 = 0; wLoop2 < MAX_SWITCH_CHANNEL; wLoop2++ )
			{
				ptSwitchChannel = m_ptSwitchTable->GetSwitchChannel( byLoop1, wLoop2 );
				if( !ptSwitchChannel->IsNull() )
				{
					dwSrcIp = ptSwitchChannel->GetSrcIp();
					dwDisIp = ptSwitchChannel->GetDisIp();
					dwRcvIp = ptSwitchChannel->GetRcvIP();
					dwDstIp = ptSwitchChannel->GetDstIP();
                    wRcvPort = ptSwitchChannel->GetRcvPort();
					wDstPort = ptSwitchChannel->GetDstPort();

					memset( achTemp, ' ', 255 );
					l = 0;

					//SrcIp
					byMpId = g_cMcuVcApp.FindMp( dwSrcIp );
					if( byMpId > 0 )
					{
						l = sprintf( achTemp, "\n%d  %s:%d(mp%d)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort, byMpId );
					}
					else
					{
						l = sprintf( achTemp, "\n%d  %s:%d(src)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort );
					}
					
					//DisIp
					memset( achTemp+l, ' ', 255-l );
					if( dwDisIp > 0 )
					{
						byMpId = g_cMcuVcApp.FindMp( dwDisIp );
						if( byMpId > 0 )
						{
							l += sprintf( achTemp+l, "-->%s:%d(mp%d)", StrOfIP( dwDisIp ), wRcvPort, byMpId );
						}
						else
						{
							l += sprintf( achTemp+l, "-->%s:%d(src)", StrOfIP( dwDisIp ), wRcvPort );
						}	
					}

					//RcvIp
					memset( achTemp+l, ' ', 255-l );
					byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
					if( byMpId > 0 )
					{
						l += sprintf( achTemp+l, "-->%s:%d(mp%d)", StrOfIP( dwRcvIp ), wRcvPort, byMpId );
					}
					else
					{
						l += sprintf( achTemp+l, "-->%s:%d(dst)", StrOfIP( dwRcvIp ), wRcvPort );
					}
					
					//DstIp
					memset( achTemp+l, ' ', 255-l );
					byMpId = g_cMcuVcApp.FindMp( dwDstIp );
					if( byMpId > 0 )
					{
						l += sprintf( achTemp+l, "-->%s:%d(mp%d)", StrOfIP( dwDstIp ), wDstPort, byMpId );
					}
					else
					{
						l += sprintf( achTemp+l, "-->%s:%d(dst)", StrOfIP( dwDstIp ), wDstPort );
					}

					StaticLog(achTemp );
					
				}
			}
		}
	}
}

//[liu lijiu][20100902]��������������ش�ͨ����RTCP����
/*====================================================================
    ������      ��SwitchLocalMultipleSpyRtcp
    ����        ����������������ش�ͨ����RTCP����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt    tSrc:              Դ�ն�
	              u8     bySrcChnnl         Դ�ն˵�ͨ������
				  TMt    tDst               �ش�Դ
	              u8     byMode:            ����ģʽ
	              BOOL32 bIsBuildSwitch     �𽻻�(false)�򽨽���(true)
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��        �汾        �޸���        �޸�����
    2010/08/28    4.6         ������         ����
====================================================================*/
// void CMcuVcInst::SwitchLocalMultipleSpyRtcp( TMt tSrc, u8 bySrcChnnl, TMt tDst, u8 byMode, BOOL32 bIsBuildSwitch)
// {
// 	if (MODE_NONE == byMode || tSrc.IsNull() || tDst.IsNull())
// 	{
// 		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[SwitchLocalMultipleSpyRtcp] Wrong params, conn't build rtcp!\n");
// 		return;
// 	}
// 	
// 	u32 dwMpIpAddr = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( GetLocalMtFromOtherMcuMt(tDst).GetMtId()));
// 	CRecvSpy tRcvSpyInfo;
// 	if( !m_cSMcuSpyMana.GetRecvSpy(tDst, tRcvSpyInfo))
// 	{
// 		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[SwitchLocalMultipleSpyRtcp] invalid spy channel!\n");
// 		return;
// 	}
// 
// 	if ( bIsBuildSwitch )//������
// 	{
// 		if (TYPE_MT == tSrc.GetMtType())//�ն�����
// 		{
// 			TLogicalChannel tLogicalChannel;
// 			//����Ƶ��RTCP����
// 			if( MODE_VIDEO == byMode || MODE_BOTH == byMode )
// 			{
// 				if(m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, TRUE ))
// 				{
// 					g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, 
// 													tLogicalChannel.GetRcvMediaCtrlChannel().GetIpAddr(), 0, 
// 													tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr(), 
// 													tLogicalChannel.GetSndMediaCtrlChannel().GetPort(),
// 													dwMpIpAddr, 
// 					                                tRcvSpyInfo.m_tSpyAddr.GetPort() + 1, 
// 													0, 0, 
// 													tLogicalChannel.GetRcvMediaChannel().GetIpAddr(), 
// 													tLogicalChannel.GetRcvMediaChannel().GetPort());
// 				}
// 				
// 			}
// 
// 			//����Ƶ��RTCP����
// 			if( MODE_AUDIO == byMode || MODE_BOTH == byMode )
// 			{
// 				if(m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE ))
// 				{
// 					g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, 
// 													 tLogicalChannel.GetRcvMediaCtrlChannel().GetIpAddr(), 0, 
// 													 tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr(),
// 													 tLogicalChannel.GetSndMediaCtrlChannel().GetPort(), 
// 													 dwMpIpAddr, 
// 					                                 tRcvSpyInfo.m_tSpyAddr.GetPort() + 3,
// 													 0, 0, 
// 													 tLogicalChannel.GetRcvMediaChannel().GetIpAddr(), 
// 													 tLogicalChannel.GetRcvMediaChannel().GetPort());
// 				}
// 			}
// 		}
// 
// 		else if(TYPE_MCUPERI == tSrc.GetMtType())//��������
// 		{
// 			u32 dwSwitchIpAddr = 0;
// 			u16 wSwitchPort    = 0;
// 			u16 wRtcpBackPort  = 0;
// 			
// 			switch(tSrc.GetEqpType())
// 			{
// 			case EQP_TYPE_VMP://����ϳ�
// 				{
// #ifdef _8KE_
// 					u16 wEqpStartPort = 0;					
// 					g_cMpManager.GetSwitchInfo( tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort, wEqpStartPort);
// #else
// 					g_cMpManager.GetVmpSwitchAddr(tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort);
// #endif
// 					
// 					if (bySrcChnnl >= 16)
// 					{
// 						wRtcpBackPort = wSwitchPort + 6 + bySrcChnnl;
// 					}
// 					if (bySrcChnnl >= 8)
// 					{
// 						wRtcpBackPort = wSwitchPort + 4 + bySrcChnnl;
// 					}
// 					else
// 					{
// 						wRtcpBackPort = wSwitchPort + 2 + bySrcChnnl;
// 					}
// 					
// 					g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwSwitchIpAddr, 0, dwSwitchIpAddr, wRtcpBackPort, dwMpIpAddr, 
// 						tRcvSpyInfo.m_tSpyAddr.GetPort() + 1);
// 					break;
// 				}
// 			default:
// 				{
// 					;
// 				}
// 			}
// 		}
// 	}
// 	else//�𽻻�
// 	{
// 		if (TYPE_MT == tSrc.GetMtType())
// 		{
// 			TLogicalChannel tLogicalChannel;
// 			//����Ƶ��RTCP����
// 			if( MODE_VIDEO == byMode || MODE_BOTH == byMode )
// 			{
// 				if(m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, TRUE ))
// 				{
// 					u32 dwVideoRtcpIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
// 					u16 wVideoRtcpPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
// 					g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwVideoRtcpIpAddr, wVideoRtcpPort, dwMpIpAddr, tRcvSpyInfo.m_tSpyAddr.GetPort() + 1);
// 					
// 				}
// 			}	
// 			
// 			//����Ƶ��rtcp����
// 			if( MODE_AUDIO == byMode || MODE_BOTH == byMode )
// 			{
// 				if(m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE ))
// 				{
// 					u32 dwAudioRtcpIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
// 					u16 wAudioRtcpPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
// 					g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwAudioRtcpIpAddr, wAudioRtcpPort, dwMpIpAddr, tRcvSpyInfo.m_tSpyAddr.GetPort() + 3);
// 				}
// 			}
// 			
// 		}
// 		//Ϊ����ʱ
// 		else if(TYPE_MCUPERI == tSrc.GetType())
// 		{
// 			u32 dwSwitchIpAddr = 0;
// 			u16 wSwitchPort    = 0;
// 			u16 wRtcpBackPort  = 0;
// 			
// 			switch(tSrc.GetEqpType())
// 			{
// 			case EQP_TYPE_VMP://����ϳ�
// 				{
// #ifdef _8KE_
// 					u16 wEqpStartPort = 0;					
// 					g_cMpManager.GetSwitchInfo( tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort, wEqpStartPort);
// #else
// 					g_cMpManager.GetVmpSwitchAddr(tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort);
// #endif
// 					
// 					if (bySrcChnnl >= 16)
// 					{
// 						wRtcpBackPort = wSwitchPort + 6 + bySrcChnnl;
// 					}
// 					if (bySrcChnnl >= 8)
// 					{
// 						wRtcpBackPort = wSwitchPort + 4 + bySrcChnnl;
// 					}
// 					else
// 					{
// 						wRtcpBackPort = wSwitchPort + 2 + bySrcChnnl;
// 					}
// 					g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwSwitchIpAddr, wRtcpBackPort, dwMpIpAddr, tRcvSpyInfo.m_tSpyAddr.GetPort() + 1);
// 					break;
// 				}
// 			default:
// 				{
// 					;
// 				}
// 			}
// 		}
// 	}
// }
/*====================================================================
    ������      ��SwitchVideoRtcpToDst
    ����        ������Ƶ����Rtcp������Ŀ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 dwDstIp, Ŀ��Ip 
	              u16 wDstPort, Ŀ�Ķ˿�
	              TEqp tSrcEqp, Դ�ն˻��豸
	              u8 bySrcChnnl Դͨ����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/10/20    3.0         ������         ����
	2011/08/11  4.6         zhangli        �޸�
====================================================================*/
//[2011/08/11/zhanlgi]ȥ������bySwitchMode���˲���û����
void CMcuVcInst::AddRemoveRtcpToDst(u32 dwDstIp, u16 wDstPort, TMt tSrc, u8 bySrcChnnl, 
									u8 byMode, /*u8 bySwitchMode,*/ BOOL32 bMapSrcAddr,
									u16 wSpyStartPort, BOOL32 bAddRtcp /* = TRUE*/)
{
	if (MODE_VIDEO2SECOND == byMode)
	{
		byMode = MODE_VIDEO;
	}

    u32 dwSrcIpAddr = 0, dwRecvIpAddr = 0, dwOldDstIp = 0, dwMapIpAddr = 0;
    u16 wRecvPort = 0, wOldDstPort = 0, wMapPort = 0;
    u8  byChlNum = 0;
    u8  byChanType = 0;

    u32 dwSwitchIpAddr;
    u16 wSwitchPort;

    TLogicalChannel tLogicalChannel;

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[AddRemoveRtcpToDst] dwDstIp:%s, wDstPort:%u, tSrc<%d,%d>, bySrcChnnl:%d, byMode:%d\n",
            StrOfIP(dwDstIp), wDstPort, tSrc.GetMcuId(), tSrc.GetMtId(), bySrcChnnl, byMode);

	wMapPort    = 0;
	dwMapIpAddr = 0;

	switch(tSrc.GetType()) 
	{
	case TYPE_MT:
		{
			TMt tTempPrsSrc = GetLocalMtFromOtherMcuMt(tSrc);
			if ( IsMultiCastMt(tTempPrsSrc.GetMtId()) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_PRS,"[AddRemoveRtcpToDst]tTempPrsSrc:%d-%d is multicastmt!\n",tTempPrsSrc.GetMcuId(), tTempPrsSrc.GetMtId());
				return ;
			}

			if (MODE_SECVIDEO == byMode)
			{
				m_ptMtTable->GetMtRtcpDstAddr(tSrc.GetMtId(), dwOldDstIp, wOldDstPort, MODE_VIDEO);
				wOldDstPort = wOldDstPort + 4;
			}
			else
			{
				m_ptMtTable->GetMtRtcpDstAddr(tSrc.GetMtId(), dwOldDstIp, wOldDstPort, byMode);
			}
			if (MODE_VIDEO == byMode)
			{
				byChanType = LOGCHL_VIDEO;
			}
			else if (MODE_AUDIO == byMode)
			{
				byChanType = LOGCHL_AUDIO;
			}
			else if (MODE_SECVIDEO == byMode)
			{
				byChanType = LOGCHL_SECVIDEO;
			}
			m_ptMtTable->GetMtLogicChnnl(tSrc.GetMtId(), byChanType, &tLogicalChannel, TRUE);

			if( SPY_CHANNL_NULL == wSpyStartPort )
			{
				wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
			}
			else
			{
				if( MODE_VIDEO == byMode )
				{
					wRecvPort = wSpyStartPort + 1;
				}
				else if( MODE_AUDIO == byMode )
				{
					wRecvPort = wSpyStartPort + 3;
				}
				else
				{
					wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
				}
			}
			//�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�
			if (bMapSrcAddr)
			{
				wMapPort    = tLogicalChannel.GetRcvMediaChannel().GetPort();
				dwMapIpAddr = tLogicalChannel.GetRcvMediaChannel().GetIpAddr();
			}
		}
		break;

	case TYPE_MCUPERI://����
		if ( MODE_SECVIDEO != byMode)
		{
			g_cMcuVcApp.GetEqpRtcpDstAddr(tSrc.GetEqpId(), bySrcChnnl, dwOldDstIp, wOldDstPort, byMode);
			g_cMcuVcApp.GetPeriEqpLogicChnnl(tSrc.GetEqpId(), byMode, &byChlNum, &tLogicalChannel, TRUE);
			switch(tSrc.GetEqpType())
			{
			case EQP_TYPE_VMP:
				{       
					BOOL32 bGetSwitchInfo = FALSE;
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
					u16 wEqpStartPort = 0;					
					bGetSwitchInfo = g_cMpManager.GetSwitchInfo( tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort, wEqpStartPort);
#else
					bGetSwitchInfo = g_cMpManager.GetVmpSwitchAddr(tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort);
#endif
					if (bGetSwitchInfo)
					{
						//[2011/08/11/zhangli]ȥ������bySwitchMode������ȡ���Ķ˿���40701������˿�vmp���������˿�
// 						if (SWITCH_MODE_BROADCAST == bySwitchMode)
// 						{
// 							wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*bySrcChnnl;
// 						}
// 						else
						{
							//  xsl [6/3/2006] ����ϳɽ�������ͨ����mp��RTCP�˿ڷ���
							// xliang [5/6/2009] ����ԭ�� 0~7	===> 2~9 
							//							  8~15	===> 12~19
							//16~23	===> 22~29	��vmp��rtp���ʹ�39000��ʼ��˿�Ϊ��39022-39029
							//24~25 ====> 32~33	��vmp��rtp���ʹ�40700��ʼ��˿�Ϊ��39032-39033
							if (bySrcChnnl >= 24)
							{
								wRecvPort = wSwitchPort+8+bySrcChnnl;
							}
							else if (bySrcChnnl >= 16)
							{
								wRecvPort = wSwitchPort + 6 + bySrcChnnl;
							}
							else if (bySrcChnnl >= 8)
							{
								wRecvPort = wSwitchPort + 4 + bySrcChnnl;
							}
							else
							{
								wRecvPort = wSwitchPort + 2 + bySrcChnnl;
							}                        
						}
					}
					else
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "AddRemoveRtcpToDst - GetVmpSwitchAddr error!\n");
					}
				}
				break;  
				
			case EQP_TYPE_BAS:
				{
					//[nizhijun 2010/9/8] BAS��������rtcp�˿ڰ���byOutChnNum����
					u8 byOutChnNum = 0;
					u8 byFrontOutNum = 0;
					if ( !GetBasChnOutputNum(tSrc,bySrcChnnl,byOutChnNum,byFrontOutNum) )
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[AddRemoveRtcpToDst] GetBasChnOutputNum:%d-%d failed!\n",
								tSrc.GetEqpId(), bySrcChnnl
								);
						return ;
					}
					wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*byFrontOutNum;
				}
				break;
			case EQP_TYPE_RECORDER:
				{
					wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN * bySrcChnnl;
				}
				break;
			default:
				wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*bySrcChnnl;
				break;
			}
			//�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�
			if (bMapSrcAddr)
			{
				//[nizhijun 2012/06/08]¼����Ķ˿���Ҫ����ƫ�ƣ���Ϊ���ڶ��ͨ��
				if ( EQP_TYPE_RECORDER == tSrc.GetEqpType() )
				{
					wMapPort    = tLogicalChannel.GetRcvMediaChannel().GetPort()+PORTSPAN*bySrcChnnl;
				}
				else
				{
					wMapPort    = tLogicalChannel.GetRcvMediaChannel().GetPort();
				}
				dwMapIpAddr = tLogicalChannel.GetRcvMediaChannel().GetIpAddr();
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[AddRemoveRtcpToDst]dwMapIpAddr: %s, wMapPort: %d\n", StrOfIP(dwMapIpAddr), wMapPort);
			}
		}
		else//[liu lijiu][20100812][add]˫��ģʽ�Ĵ���start
		{
			//[2011/08/18/zhangli]�������裬ĳ��ͨ�������wOldDstPortû��-4��������ﲻ��+4
			g_cMcuVcApp.GetEqpRtcpDstAddr(tSrc.GetEqpId(), bySrcChnnl, dwOldDstIp, wOldDstPort, MODE_VIDEO);
			//wOldDstPort = wOldDstPort + 4;//˫���Ķ˿ڱ���Ƶ�Ķ˿ڶ�4
			g_cMcuVcApp.GetPeriEqpLogicChnnl(tSrc.GetEqpId(), MODE_VIDEO, &byChlNum, &tLogicalChannel, TRUE);
			switch(tSrc.GetEqpType())
			{
			case EQP_TYPE_VMP:
				{       
					if (g_cMpManager.GetVmpSwitchAddr(tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort))
					{
// 						if (SWITCH_MODE_BROADCAST == bySwitchMode)
// 						{
// 							wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*bySrcChnnl + 4;
// 						}
//						else
						{
							//  xsl [6/3/2006] ����ϳɽ�������ͨ����mp��RTCP�˿ڷ���
							// xliang [5/6/2009] ����ԭ�� 0~7	===> 2~9 
							//							  8~15	===> 12~19
							//16~23	===> 22~29	��vmp��rtp���ʹ�39000��ʼ��˿�Ϊ��39022-39029
							//24~25 ====> 32~33	��vmp��rtp���ʹ�40700��ʼ��˿�Ϊ��39032-39033
							if (bySrcChnnl >= 24)
							{
								wRecvPort = wSwitchPort+8+bySrcChnnl;
							}
							else if (bySrcChnnl >= 16)
							{
								wRecvPort = wSwitchPort + 6 + bySrcChnnl;
							}
							else if (bySrcChnnl >= 8)
							{
								wRecvPort = wSwitchPort + 4  + bySrcChnnl;
							}
							else
							{
								wRecvPort = wSwitchPort + 2 + bySrcChnnl;
							}                        
						}
					}
					else
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "AddRemoveRtcpToDst - GetVmpSwitchAddr error!\n");
					}
				}
				break;  
				
			case EQP_TYPE_BAS:
				{
					//[nizhijun 2010/9/8] BAS��������rtcp�˿ڰ���byOutChnNum����
					u8 byOutChnNum = 0;
					//[nizhijun 2010/9/9] ĿǰBAS��˫������˿ڻ���ԭ���˿ڣ�����Ҫ+4,����Ͱ�ǰ��ӵļ���
					//wOldDstPort = wOldDstPort - 4;
					u8 byFrontOutNum = 0;
					if ( !GetBasChnOutputNum(tSrc,bySrcChnnl,byOutChnNum,byFrontOutNum) )
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[AddRemoveRtcpToDst] GetBasChnOutputNum:%d-%d failed!\n",
							tSrc.GetEqpId(), bySrcChnnl
							);
						return ;
					}

					wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*byFrontOutNum;
				}
				break;
			case EQP_TYPE_RECORDER:
				{
					wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + 4 + PORTSPAN * bySrcChnnl;
				}
				break;
			default:
				wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + 4 + PORTSPAN*bySrcChnnl;
				break;
			}
			//�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�
			if (bMapSrcAddr)
			{
				//[nizhijun 2012/06/08]¼����Ķ˿���Ҫ����ƫ�ƣ���Ϊ���ڶ��ͨ��
				if ( EQP_TYPE_RECORDER == tSrc.GetEqpType() )
				{
					wMapPort    = tLogicalChannel.GetRcvMediaChannel().GetPort()+PORTSPAN*bySrcChnnl+4;
				}
				else
				{
					wMapPort    = tLogicalChannel.GetRcvMediaChannel().GetPort() + 4;
				}
				dwMapIpAddr = tLogicalChannel.GetRcvMediaChannel().GetIpAddr();
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[AddRemoveRtcpToDst]dwMapIpAddr: %s, wMapPort: %d\n", StrOfIP(dwMapIpAddr), wMapPort);
			}			
		}
		break;

	default:
		break;
	}

    if (TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_TVWALL == tSrc.GetEqpType())
    {
        TPeriEqpStatus tTWStatus;
        if ( bySrcChnnl < MAXNUM_PERIEQP_CHNNL && g_cMcuVcApp.GetPeriEqpStatus(tSrc.GetEqpId(), &tTWStatus))
        {
            TMt tMt = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[bySrcChnnl];
            u32 dwMtSwitchIp;
            u16 wMtSwitchPort;
            u32 dwMtSrcIp;

            g_cMpManager.GetSwitchInfo(tMt, dwMtSwitchIp, wMtSwitchPort, dwMtSrcIp);

            dwSrcIpAddr = dwMtSwitchIp;
            dwRecvIpAddr = dwMtSwitchIp;
        }
    }
    else
    {
        dwSrcIpAddr = tLogicalChannel.GetRcvMediaChannel().GetIpAddr();
        dwRecvIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
    }

	if(IsValidHduEqp(tSrc))
    {
        TPeriEqpStatus tHduStatus;
        if ( bySrcChnnl < MAXNUM_HDU_CHANNEL && g_cMcuVcApp.GetPeriEqpStatus(tSrc.GetEqpId(), &tHduStatus))
        {
            TMt tMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[bySrcChnnl];
            u32 dwMtSwitchIp;
            u16 wMtSwitchPort;
            u32 dwMtSrcIp;
			
            g_cMpManager.GetSwitchInfo(tMt, dwMtSwitchIp, wMtSwitchPort, dwMtSrcIp);
			
            dwSrcIpAddr = dwMtSwitchIp;
            dwRecvIpAddr = dwMtSwitchIp;
        }
    }
    else
    {
        dwSrcIpAddr = tLogicalChannel.GetRcvMediaChannel().GetIpAddr();
        dwRecvIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
    }

    //�������µ�Ŀ��ǰ�Ƴ��ϵĽ���
	u32 dwSaveDstIp = 0;
	u16 wSaveDstPort = 0;
    if (0 != dwOldDstIp)
    {
        g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwOldDstIp, wOldDstPort);
    }

	g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIp, wDstPort);

	if (bAddRtcp)
	{
		dwSaveDstIp = dwDstIp;
		wSaveDstPort= wDstPort;

		//��ӽ���
		g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwSrcIpAddr, 0, dwRecvIpAddr,wRecvPort, 
			dwDstIp, wDstPort, 0, 0, dwMapIpAddr, wMapPort);

		LogPrint(LOG_LVL_DETAIL, MID_MCU_PRS, "[AddRemoveRtcpToDst]dwRecvIpAddr::%s,wRecvPort%d\n", StrOfIP(dwRecvIpAddr), wRecvPort);
	} 
	
	//��¼����Ŀ��
	if(MODE_SECVIDEO == byMode)//[liu lijiu][20100814][add]˫��ģʽ
	{
		switch(tSrc.GetType()) 
		{
		case TYPE_MT://�ն�	
			m_ptMtTable->SetMtRtcpDstAddr(tSrc.GetMtId(), dwSaveDstIp, ((wSaveDstPort == 0) ? 0: (wSaveDstPort - 4)), MODE_VIDEO);
			break;
			
		case TYPE_MCUPERI://����
			g_cMcuVcApp.SetEqpRtcpDstAddr(tSrc.GetEqpId(), bySrcChnnl, dwSaveDstIp, wSaveDstPort, MODE_VIDEO);
			break;

		default:
			break;
		}
	}//[liu lijiu][20100814][add]
	else//��˫��ģʽ
	{
		switch(tSrc.GetType()) 
		{
		case TYPE_MT://�ն�	
			m_ptMtTable->SetMtRtcpDstAddr(tSrc.GetMtId(), dwSaveDstIp, wSaveDstPort, byMode);
			break;
			
		case TYPE_MCUPERI://����
			g_cMcuVcApp.SetEqpRtcpDstAddr(tSrc.GetEqpId(), bySrcChnnl, dwSaveDstIp, wSaveDstPort, byMode);
			break;

		default:
			break;
		}
	}

	return;
}

/*====================================================================
������      ��BuidRtcpSwitchSrcToDst
����        ������Src��Dst��RTCP����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����TMt tSrc, RTCP�������ߣ���RTP�����շ� �ն�/����
			  TMt tDst��RTCP�������ߣ���RTP�������� �ն�/�ش�ͨ��/����
			  u8 byMediaMode��ģʽ����/��Ƶ
			  u8 bySrcChnnl   tSrcͨ����
			  u8 bySwitchMode ����ģʽ��ѡ��/�㲥
			  u8 byDstChnnl  tDst��ͨ����Ŀǰ����ѡ��VMPʱʹ��
����ֵ˵��  ����
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/01/06  4.6         ��־��        ����
2011/08/11  4.6         zhangli        �޸�
====================================================================*/
//[2011/08/11/zhanlgi]ȥ������bySwitchMode���˲���û����
void CMcuVcInst::BuildRtcpSwitchForSrcToDst(TMt tSrc, TMt tUnlocalDst, u8 byMediaMode, 
										   u8 bySrcChnnl /* = 0 */, 
										   /*u8 bySwitchMode = SWITCH_MODE_BROADCAST,*/
										   u8 byDstChnnl /*= 0*/,
										   BOOL32 bAddRtcp/* = TRUE*/)
{
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (!tConfAttrb.IsResendLosePack())
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchForSrcToDst]Conf:%d is not support PRS\n",m_byConfIdx);
		return;
	}
	
	if (tSrc.IsNull() || tUnlocalDst.IsNull())
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst] tSrc's or tUnlocalDst's Tmt Info is Null\n");
		return;
	}

	//����Դ�Ƿ�keda�Ĳ�֧��
	if(TYPE_MT == tSrc.GetType() && !IsKedaMt(tSrc, TRUE))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[BuidRtcpSwitchSrcToDst] tSrc:%d-%d isn't mt of kedacom!\n",tSrc.GetMcuId(), tSrc.GetMtId());
		return;
	} 

	TMt tTempPrsSrc = GetLocalMtFromOtherMcuMt(tSrc);
	if ( TYPE_MT == tTempPrsSrc.GetType() && IsMultiCastMt(tTempPrsSrc.GetMtId()) )
	{
		ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_PRS,"[BuildRtcpSwitchForSrcToDst]tTempPrsSrc:%d-%d is multicastmt!\n",tTempPrsSrc.GetMcuId(), tTempPrsSrc.GetMtId());
		return;
	}

	//������Ŀ���Ƿ�keda�Ĳ�֧��
	if(TYPE_MT == tUnlocalDst.GetType() && !IsKedaMt(tUnlocalDst, TRUE))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[BuidRtcpSwitchSrcToDst] tUnlocalDst:%d-%d isn't mt of kedacom!\n",tUnlocalDst.GetMcuId(), tUnlocalDst.GetMtId());
		return;
	} 
	
	if (MODE_NONE == byMediaMode)
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst] MeidaMode is Null\n");
		return;
	}
	
	if (MODE_BOTH == byMediaMode)
	{
		BuildRtcpSwitchForSrcToDst(tSrc, tUnlocalDst, MODE_VIDEO, bySrcChnnl, byDstChnnl, bAddRtcp);
		BuildRtcpSwitchForSrcToDst(tSrc, tUnlocalDst, MODE_AUDIO, bySrcChnnl, byDstChnnl, bAddRtcp);
		return;
	}

// 	if (TYPE_MCUPERI == tSrc.GetType())
// 	{
// 		switch (tSrc.GetEqpType())
// 		{
// 		case EQP_TYPE_RECORDER:
// 			//����¼�� && ������¼������ռ��prs�ģ�����
// 			if (tSrc == m_tRecEqp && bySrcChnnl == m_byRecChnnl && !IsRecNeedAdapt())
// 			{
// 				return;
// 			}
// 		case EQP_TYPE_PRS:
// 			return;
// 		default:
// 			break;
// 		}
// 	}

	u8 bySrcMode = byMediaMode;
	u8 byDstMode = byMediaMode;
	if (MODE_VIDEO2SECOND == byMediaMode)
	{
		bySrcMode = MODE_SECVIDEO;
		byDstMode = MODE_VIDEO;
	}
	TTransportAddr tRemoteAddr;
	if (!GetRemoteRtcpAddr(tUnlocalDst, byDstChnnl, byDstMode, tRemoteAddr))
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[BuidRtcpSwitchForSrcToDst] GetRemoteRtcpAddr failed,dstMt:%d-%d!\n",
			tUnlocalDst.GetMcuId(), tUnlocalDst.GetMtId());
		return;
	}
	
	AddRemoveRtcpToDst(tRemoteAddr.GetIpAddr(),tRemoteAddr.GetPort(), 
		tSrc, bySrcChnnl, bySrcMode, TRUE, SPY_CHANNL_NULL, bAddRtcp);
// 	u32 dwRtcpDstIp   = 0;
// 	u16 wRctpDstPort  = 0;
// 	
// 	if (tUnlocalDst.GetType() == TYPE_MCUPERI && EQP_TYPE_VMP == tUnlocalDst.GetEqpType())
// 	{
// 		u32 dwEqpIP,dwRcvIp;
//         u16 wRcvPort;
// 		if(g_cMpManager.GetSwitchInfo(tUnlocalDst, dwRcvIp, wRcvPort, dwEqpIP))
// 		{
// 			dwRtcpDstIp =  dwEqpIP;
// 			wRctpDstPort = wRcvPort + PORTSPAN * byDstChnnl + 1;
// 			if (dwRtcpDstIp>0 && wRctpDstPort>0)
// 			{
// 				LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]Dst is VMP:tUnlocalDst(EQPID:%d) SwitchVideoRtcpToDst begin,Mode:%d",
// 					tUnlocalDst.GetEqpId(),MODE_VIDEO);
// 				AddRemoveRtcpToDst(dwRtcpDstIp,wRctpDstPort,tSrc,bySrcChnnl,byMediaMode,bySwitchMode,TRUE);
// 			}
// 			else
// 			{
// 				LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy:tUnlocalDst(%d-%d) GetSndMediaCtrlChannel failed!Mode:%d\n",
// 					tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId(),byMediaMode);
// 				return;
// 			}
// 		}
// 		else
// 		{
// 			LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchForSrcToDst]tUnlocalDst(%d) is VMP,Fail to get his switch info!\n",tUnlocalDst.GetEqpId());
// 			return;
// 		}
// 	}
// 	else
// 	{
// 		//�˽ӿڲ�����RTCP ����Դ���ն˻������裬��������������SwitchVideoRtcpToDst��SwitchLocalMultipleSpyRtcp�����֡�
// 		TLogicalChannel tLogicalChannel;
// 		BOOL32 bIsSingleSpy = (!tUnlocalDst.IsLocal()) && ( !IsLocalAndSMcuSupMultSpy(tUnlocalDst.GetMcuId()) );  //�Ƿ�֧�ֵ��ش�
// 		
// 		if (tUnlocalDst.IsLocal() || bIsSingleSpy)
// 		{	
// 			//���Ŀ���ն��Ǳ������ߵ��ش�����µ��¼�����Ŀ���ն˱��ػ�
// 			TMt tLocalDst = GetLocalMtFromOtherMcuMt(tUnlocalDst);
// 			if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
// 			{
// 				if ( m_ptMtTable->GetMtLogicChnnl(tLocalDst.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE) )
// 				{
// 					dwRtcpDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
// 					wRctpDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
// 					if (dwRtcpDstIp >0 && wRctpDstPort>0)
// 					{
// 						LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy/Local:tUnlocalDst(%d-%d) SwitchVideoRtcpToDst begin,Mode:%d",
// 							tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId(),MODE_VIDEO);
// 						AddRemoveRtcpToDst(dwRtcpDstIp,wRctpDstPort,tSrc,bySrcChnnl,MODE_VIDEO,bySwitchMode,TRUE);
// 					}
// 					else
// 					{
// 						LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy:tUnlocalDst(%d-%d) GetSndMediaCtrlChannel failed!Mode:%d\n",
// 							tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId(),byMediaMode);
// 						return;
// 					}
// 				}
// 				else
// 				{
// 					LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy:tUnlocalDst(%d-%d) GetMtLogicChnnl failed!\n",
// 						tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId());
// 					return;
// 				}
// 			}
// 			
// 			if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
// 			{
// 				if ( m_ptMtTable->GetMtLogicChnnl(tLocalDst.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE) )
// 				{
// 					dwRtcpDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
// 					wRctpDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
// 					if (dwRtcpDstIp >0 && wRctpDstPort>0)
// 					{
// 						LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy/Local:tUnlocalDst(%d-%d) SwitchVideoRtcpToDst begin,Mode:%d",
// 							tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId(),MODE_AUDIO);
// 						AddRemoveRtcpToDst(dwRtcpDstIp,wRctpDstPort,tSrc,bySrcChnnl,MODE_AUDIO,bySwitchMode,TRUE);
// 					}
// 					else
// 					{	LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy:tUnlocalDst(%d-%d) GetSndMediaCtrlChannel failed!,Mode:%d\n",
// 							tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId(),byMediaMode);
// 						return;
// 					}
// 				}
// 				else
// 				{
// 					LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy:tDst(%d-%d) GetMtLogicChnnl failed!\n",
// 						tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId());
// 					return;
// 				}				
// 			}
// 			
// 			if (MODE_SECVIDEO == byMediaMode)
// 			{
// 				if ( m_ptMtTable->GetMtLogicChnnl(tLocalDst.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE) )
// 				{
// 					dwRtcpDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
// 					wRctpDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
// 					if (dwRtcpDstIp >0 && wRctpDstPort>0)
// 					{
// 						LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy/Local:tUnlocalDst(%d-%d) SwitchVideoRtcpToDst begin,Mode:%d",
// 							tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId(),MODE_SECVIDEO);
// 						AddRemoveRtcpToDst(dwRtcpDstIp,wRctpDstPort,tSrc,bySrcChnnl,MODE_SECVIDEO,bySwitchMode,TRUE);
// 					}
// 					else
// 					{
// 						LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy:tUnlocalDst(%d-%d) GetSndMediaCtrlChannel failed!Mode:%d\n",
// 							tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId(),MODE_SECVIDEO);
// 						return;
// 					}
// 				}
// 				else
// 				{
// 					LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy:tUnlocalDst(%d-%d) GetMtLogicChnnl failed!\n",
// 						tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId());
// 					return;
// 				}
// 			}
// 		}
// 		//��ش������tUnlocalDst�����¼�,���������ն˵��ش�ͨ����RTCP����
// 		else
// 		{	
// 			LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]MultiSpy:tUnlocalDst is come from smcu",tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId());
// 			SwitchLocalMultipleSpyRtcp(tSrc,bySrcChnnl,tUnlocalDst,byMediaMode,TRUE);		
// 		}		
// 		return;
// 	}
}



/*====================================================================
    ������      ��ProcMcuMtFastUpdateTimer
    ����        ���������ն�����ؼ�֡��ʱ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/10/20    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ProcMcuMtFastUpdateTimer( const CMessage * pcMsg )
{
	u8 byMtId  = (u8)(*((u32*)pcMsg->content)/100); 
	u8 byCount = (u8)(*((u32*)pcMsg->content)%100); 
	u32 dwCurTick = OspTickGet();
	CServMsg cServMsg;
	
	STATECHECK;	

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMcuMtFastUpdateTimer] Mcu Request Mt(%d %d) send FastUpdate Count(%d)\n", 
			m_ptMtTable->GetMcuId(byMtId), byMtId, byCount);

	u8 byMode = MODE_VIDEO;
	cServMsg.SetMsgBody(&byMode, sizeof(u8));
	SendMsgToMt( byMtId, MCU_MT_FASTUPDATEPIC_CMD, cServMsg );
	m_ptMtTable->SetLastTick( byMtId, dwCurTick );
	
	byCount--;
	
    if (MT_MANU_TAIDE == m_ptMtTable->GetManuId(byMtId))
    {
//  [4/10/2012 chendaiwei] ��ԭ���߼���̩���ն˴˴�ʼ�ղ����������ע�͵�
//         if( byCount < 1)
//         {
//             SetTimer( pcMsg->event, 3600, 100*byMtId + byCount);
//             ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMcuMtFastUpdateTimer] mt.%d fastupdate has been adjust to once more due to Tandberg!\n", byMtId);
//         }
    }
    else
    {
	    if( byCount > 0 )
	    {
		    SetTimer( pcMsg->event, 1200, 100*byMtId + byCount);
	    }
    }
}

/*====================================================================
    ������      ��ProcMcuEqpFastUpdateTimer
    ����        ����������������ؼ�֡��ʱ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/07/11    4.6         ������         ����
====================================================================*/
void CMcuVcInst::ProcMcuEqpFastUpdateTimer(const CMessage * pcMsg)
{
    u32 dwParam = *((u32*)pcMsg->content);
    u8 byEqpId = (u8)(dwParam >> 16);
    u8 byChnl  = (u16)dwParam >> 8;
    u8 byCount = (u8)dwParam;

    byEqpId = byEqpId;

    STATECHECK;	

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMcuEqpFastUpdateTimer] the Eqp.%u, chn.%u, byCount.%d!\n", byEqpId, byChnl, byCount);

    CServMsg cServMsg;
    u16 wEvent;
    u32 dwCurTick = OspTickGet();
    u32 dwLaskTick = 0;

    TEqp tDstEqp = g_cMcuVcApp.GetEqp(byEqpId);
    
    switch (tDstEqp.GetEqpType())
    {
    case EQP_TYPE_VMP:
        {
            cServMsg.SetChnIndex(byChnl);
            wEvent = MCU_VMP_FASTUPDATEPIC_CMD;
            m_dwVmpLastVCUTick = dwCurTick;
            SendMsgToEqp(byEqpId, wEvent, cServMsg); 
            return;
        }
        
    case EQP_TYPE_VMPTW:
		wEvent = MCU_VMPTW_FASTUPDATEPIC_CMD;
        //dwLaskTick = m_dwVmpTwLastVCUTick;
        break;
        
    case EQP_TYPE_BAS:
		{
			cServMsg.SetChnIndex(byChnl);
			wEvent = MCU_BAS_FASTUPDATEPIC_CMD;
			SetBasVcuTick(tDstEqp, byChnl, dwCurTick);
		}

		break;
    default:
        return;
    }

    //����1s��϶
    if ( dwCurTick - dwLaskTick > 1 * OspClkRateGet() )
    {
        SendMsgToEqp(tDstEqp.GetEqpId(), wEvent, cServMsg);  
    }
    
    if( byCount < 3 )
    {
        byCount++;
        dwParam = (dwParam & 0xffffff00) | byCount;
        SetTimer( pcMsg->event, 1200, dwParam );
    }

    return;
}

/*====================================================================
    ������      ��ProcMcuMtSecVideoFastUpdateTimer
    ����        ���������ն�����ؼ�֡��ʱ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/10/20    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ProcMcuMtSecVideoFastUpdateTimer( const CMessage * pcMsg )
{
	u8 byMtId  = (u8)(*((u32*)pcMsg->content)/100); 
	u8 byCount = (u8)(*((u32*)pcMsg->content)%100);
	u32 dwCurTick = OspTickGet();
	CServMsg cServMsg;

	STATECHECK;	

	u8 byMode = MODE_SECVIDEO;
	cServMsg.SetMsgBody(&byMode, sizeof(u8));
	SendMsgToMt( byMtId, MCU_MT_FASTUPDATEPIC_CMD, cServMsg );
	m_ptMtTable->SetLastTick( byMtId, dwCurTick, TRUE );
	
	if( byCount < 3 )
	{
		SetTimer( pcMsg->event, 1200, 100*byMtId + byCount + 1  );
	}
}

/*=============================================================================
    �� �� ���� StopSpyMtCascaseSwitch
    ��    �ܣ� ֹͣ�ϴ��������ɱ��������ϼ�mcu�Ľ�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2010/03/02  4.5		    �ܾ���                ����
	20110702    4.6         ���                  �߼�������ע���޸�
=============================================================================*/
BOOL32 CMcuVcInst::StopSpyMtCascaseSwitch( u8 byMode /*= MODE_BOTH*/ )
{
	if( m_tCascadeMMCU.IsNull() )
	{
		return FALSE;
	}

	u16 wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
	TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMMcuIdx/*m_tCascadeMMCU.GetMtId()*/ );
	if(ptConfMcInfo == NULL)
	{		
		return FALSE;
	}

	TMt tSpyMt = ptConfMcInfo->m_tSpyMt;
	if( MODE_BOTH == byMode )
	{
  		ptConfMcInfo->m_tSpyMt.SetNull();
	}

	CServMsg cServMsg;
    cServMsg.SetConfId(m_tConf.GetConfId());

	TMt tSrc;
	tSrc.SetNull();

	TMt tLocalMt;

	TMtStatus tMtStatus;

	if( MODE_BOTH == byMode || MODE_VIDEO ==  byMode )
	{
		m_ptMtTable->GetMtSrc( m_tCascadeMMCU.GetMtId(),&tSrc,MODE_VIDEO );
		tLocalMt = GetLocalMtFromOtherMcuMt( tSrc );
		//if( tSrc == tSpyMt )
		{
			if (IsNeedSelAdpt(tSrc, m_tCascadeMMCU, MODE_VIDEO))
			{
				StopSelAdapt(tSrc, m_tCascadeMMCU, MODE_VIDEO);
			}
			else
			{
				//zjl 20110510 StopSwitchToSubMt �ӿ������滻
				//StopSwitchToSubMt( m_tCascadeMMCU.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT );
				StopSwitchToSubMt(1, &m_tCascadeMMCU, MODE_VIDEO );

				//[2011/08/18/zhangli]��RTCP����
				//������BuildRtcpSwitchForSrcToDst��������¼��նˣ�����ش�Դ�Ѿ���free�ˣ�GetRemoteRtcpAddr��ʧ��
				TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
				if (tConfAttrb.IsResendLosePack())
				{
					//BuildRtcpSwitchForSrcToDst(m_tCascadeMMCU, tSrc, MODE_VIDEO, 0, 1, FALSE);

					u32 dwOldDstIp = 0;
					u16 wOldDstPort = 0;
					m_ptMtTable->GetMtRtcpDstAddr(m_tCascadeMMCU.GetMtId(), dwOldDstIp, wOldDstPort, MODE_VIDEO);
					if (dwOldDstIp != 0)
					{
						AddRemoveRtcpToDst(dwOldDstIp, wOldDstPort, m_tCascadeMMCU, 0, MODE_VIDEO, TRUE, SPY_CHANNL_NULL, FALSE);
					}
				}
			}		
			tSrc.SetNull();
			m_ptMtTable->SetMtSrc( m_tCascadeMMCU.GetMtId(),&tSrc,MODE_VIDEO );	
			m_ptMtTable->GetMtStatus( m_tCascadeMMCU.GetMtId(),&tMtStatus );
			tMtStatus.SetSelectMt( tSrc,MODE_VIDEO );
			m_ptMtTable->SetMtStatus( m_tCascadeMMCU.GetMtId(),&tMtStatus );

		}
		/*else
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[StopSpyMtCascaseAdp] m_tCascadeMMCU's Video SrcMt(%d.%d) is not equal SpyMt(%d.%d).so can't stop spymt video spy switch\n",
				tSrc.GetMcuId(),
				tSrc.GetMtId(),
				tSpyMt.GetMcuId(),
				tSpyMt.GetMtId()
				);
		}*/

	}

	if( MODE_BOTH == byMode || MODE_AUDIO ==  byMode )
	{
		m_ptMtTable->GetMtSrc( m_tCascadeMMCU.GetMtId(),&tSrc,MODE_AUDIO );
		tLocalMt = GetLocalMtFromOtherMcuMt( tSrc );
		CSendSpy cSendSpy;
		//if( tSrc == tSpyMt )	
		{			
			if (IsNeedSelAdpt(tSrc, m_tCascadeMMCU, MODE_AUDIO) &&
				( !m_cLocalSpyMana.GetSpyChannlInfo(tSrc,cSendSpy) || cSendSpy.GetSpyMode() == MODE_VIDEO )
				)
			{
				StopSelAdapt(tSrc, m_tCascadeMMCU, MODE_AUDIO);
			}
			else
			{
				//zjl 20110510 StopSwitchToSubMt �ӿ������滻
				//StopSwitchToSubMt( m_tCascadeMMCU.GetMtId(), MODE_AUDIO, SWITCH_MODE_SELECT );
				StopSwitchToSubMt( 1, &m_tCascadeMMCU, MODE_AUDIO );
				TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
				if (tConfAttrb.IsResendLosePack())
				{	
					u32 dwOldDstIp = 0;
					u16 wOldDstPort = 0;
					m_ptMtTable->GetMtRtcpDstAddr(m_tCascadeMMCU.GetMtId(), dwOldDstIp, wOldDstPort, MODE_AUDIO);
					if (dwOldDstIp != 0)
					{
						AddRemoveRtcpToDst(dwOldDstIp, wOldDstPort, m_tCascadeMMCU, 0, MODE_AUDIO, TRUE, SPY_CHANNL_NULL, FALSE);
					}
				}
			}
			tSrc.SetNull();

			m_ptMtTable->SetMtSrc( m_tCascadeMMCU.GetMtId(), &tSrc,MODE_AUDIO );
			m_ptMtTable->GetMtStatus( m_tCascadeMMCU.GetMtId(),&tMtStatus );
			tMtStatus.SetSelectMt( tSrc,MODE_AUDIO );
			m_ptMtTable->SetMtStatus( m_tCascadeMMCU.GetMtId(),&tMtStatus );


		}
		/*else
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[StopSpyMtCascaseAdp] m_tCascadeMMCU's Audio SrcMt(%d.%d) is not equal SpyMt(%d.%d).so can't stop spymt audio spy switch\n",
				tSrc.GetMcuId(),
				tSrc.GetMtId(),
				tSpyMt.GetMcuId(),
				tSpyMt.GetMtId()
				);
		}*/

	}

	if( !m_tCascadeMMCU.IsNull() && MODE_BOTH == byMode )
	{
		CServMsg cMsg;
		TMsgHeadMsg tHeadMsg;
		TSetOutParam tOutParam;
		memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
		tOutParam.m_nMtCount = 1;
		//tOutParam.m_atConfViewOutInfo[0].m_tMt = m_tCascadeMMCU;
		//tOutParam.m_atConfViewOutInfo[0].m_tMt.SetMcuId( LOCAL_MCUID );
		tOutParam.m_atConfViewOutInfo[0].m_tMt.SetMcuId( m_tCascadeMMCU.GetMtId() );
		tOutParam.m_atConfViewOutInfo[0].m_tMt.SetMtId( 0 );
		tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
		tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;		
		cMsg.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
		cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		cMsg.SetEventId(MCU_MCU_SETOUT_NOTIF);
		SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_SETOUT_NOTIF, cMsg);

		//[2011/10/08/zhangli] Bug00064460����״̬�£�ͣ�������ϴ��ն�ʱ��δ��mcs���͸���״̬��Ӧ
		MtStatusChange(&m_tCascadeMMCU, TRUE);
	}

	return TRUE;
}

// END OF FILE
