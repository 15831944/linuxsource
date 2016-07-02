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
#include "evmcu.h"
#include "evmcutest.h"
#include "mcuvc.h"
#include "mcsssn.h"
#include "mcuerrcode.h"
#include "mpmanager.h"
#include "mcuutility.h"

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
BOOL32 CMcuVcInst::StartSwitchToSubMt(TMt   tSrc, 
                                      u8    bySrcChnnl,
                                      u8    byDstMtId,
                                      u8    byMode,
                                      u8    bySwitchMode,
                                      BOOL32 bMsgStatus,
                                      BOOL32 bSndFlowCtrl, BOOL32 bIsSrcBrd)
{
    TMtStatus tMtStatus;
    CServMsg  cServMsg;
    u8 byMediaMode = byMode;
    BOOL32 bSwitchedAudio = FALSE;

    tSrc = GetLocalMtFromOtherMcuMt(tSrc);

	//����KDC�ն��Ƿ�������״̬�����򲻽�����Ƶ����	
	if( TYPE_MT == tSrc.GetType() && 
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
	if( MT_MANU_KDC != m_ptMtTable->GetManuId( byDstMtId ) && 
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

    BOOL32 bMMcuNeedAdp = FALSE;
	
	//ֻ���ϼ�Mcu��spyMt
	TMt tDstMt = m_ptMtTable->GetMt( byDstMtId );
	if(m_tCascadeMMCU.GetMtId() != 0 && byDstMtId == m_tCascadeMMCU.GetMtId())
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tDstMt.GetMtId());
		if( ptConfMcInfo != NULL && 
			( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
		{
			if( TYPE_MT == tSrc.GetType() &&
                !(ptConfMcInfo->m_tSpyMt.GetMtId() == tSrc.GetMtId()&&
				  ptConfMcInfo->m_tSpyMt.GetMcuId() == tSrc.GetMcuId()))
			{
                return FALSE;
			}

            //�ж�spymt���ϼ��Ƿ���Ҫ����
            TSimCapSet tSrcSimCap = m_ptMtTable->GetSrcSCS(ptConfMcInfo->m_tSpyMt.GetMtId());
            TSimCapSet tDstSimCap = m_ptMtTable->GetDstSCS(byDstMtId);
            
            if (tSrcSimCap.IsNull())
            {
                ConfLog(FALSE, "[StartSwitchToSubMt] tSrc.%d primary logic chan unexist!\n", ptConfMcInfo->m_tSpyMt.GetMtId());
                return FALSE;
            }
            tSrcSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtSndBitrate(ptConfMcInfo->m_tSpyMt.GetMtId()));
            
            if (tDstSimCap.IsNull())
            {
                ConfLog(FALSE, "[StartSwitchToSubMt] tDst.%d back logic chan unexist!\n", byDstMtId);
                return FALSE;
            }
            tDstSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtReqBitrate(byDstMtId));
            
            if (tDstSimCap < tSrcSimCap)
            {
                bMMcuNeedAdp = TRUE;
            }
		}
		g_cMpManager.SetSwitchBridge(tSrc, 0, byMediaMode);
	}
    
	//ֻ�ղ��� 
	TMt TNullMt;
	TNullMt.SetNull();
	if( byDstMtId == 0 )
	{
        g_cMpManager.StartSwitchToSubMt( tSrc, bySrcChnnl, TNullMt, byMediaMode, bySwitchMode );
		return TRUE;
	}

	//ֻ�����ն�, ���ý��н���
	m_ptMtTable->GetMtStatus(byDstMtId, &tMtStatus);
	if( ( (!tMtStatus.IsReceiveVideo()) && byMediaMode == MODE_VIDEO ) ||
		( (!tMtStatus.IsReceiveAudio()) && byMediaMode == MODE_AUDIO ) )
	{
		return FALSE;
	}

    //Ŀ���ն��Ƿ��ڻ���
    if (tMtStatus.IsMediaLoop(MODE_VIDEO))
    {
        if (byMediaMode == MODE_VIDEO)
        {
            return TRUE;
        }
        else if (byMediaMode == MODE_BOTH)
        {
            byMediaMode = MODE_AUDIO;
        }
    }
    
    if (tMtStatus.IsMediaLoop(MODE_AUDIO))
    {
        if (byMediaMode == MODE_AUDIO)
        {
            return TRUE;
        }
        else if (byMediaMode == MODE_BOTH)
        {
            byMediaMode = MODE_VIDEO;
        }
    }

	//�����ѽ��õ�һ·��Ƶͨ������˫�����նˣ��л���һ·��ƵԴ�����ܾ�
	if( TRUE == JudgeIfSwitchFirstLChannel(tSrc, bySrcChnnl, tDstMt) )
	{	
		if( byMediaMode == MODE_VIDEO )
		{
			return TRUE;
		}
		else if ( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_AUDIO;
		}
	}
	
	//��Ƶ���������Լ�, �����Դ��Ŀ���Ϊmcu����Ӧ���н���
	if( (tSrc.GetMtId() == byDstMtId) && 
		(tSrc.GetType() == TYPE_MT) && 
		(tSrc.GetMtType() == MT_TYPE_MT) )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			StopSwitchToSubMt( byDstMtId, byMediaMode, SWITCH_MODE_BROADCAST, bMsgStatus );
			return TRUE;
		}
		else if ( byMediaMode == MODE_BOTH )
		{
			StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgStatus );
			byMediaMode = MODE_VIDEO;
		}
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
		}
	}
	
	//��Ҫ���Լ�������������ȥ, �����Դ��Ŀ���Ϊmcu����Ӧ���н���
	if( (tSrc.GetType() == TYPE_MT) &&
		(tSrc.GetMtType() == TYPE_MT) && 
		(tSrc.GetMtId() == byDstMtId) && 
		(m_ptMtTable->GetManuId( byDstMtId ) == MT_MANU_KDC) )
	{
		if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
		{
			NotifyMtReceive( tSrc, tDstMt.GetMtId() );
		}
		
		StopSwitchToSubMt( byDstMtId, byMediaMode, SWITCH_MODE_BROADCAST, bMsgStatus );

		// guzh [8/31/2006] �����������ն�״̬��Stop��ʱ�������
		// m_ptMtTable->SetMtSrc( byDstMtId, ( const TMt * )&tSrc, byMediaMode );			
        
		return TRUE;
	}
    
    //for h320 mcu cascade select
    if ( m_ptMtTable->GetMtType(byDstMtId) == MT_TYPE_SMCU && tSrc.GetMtId() == byDstMtId &&
         !m_tLastSpyMt.IsNull() && 
         m_tConfAllMtInfo.MtJoinedConf(m_tLastSpyMt.GetMcuId(), m_tLastSpyMt.GetMtId()) )
    {
        tSrc = m_tLastSpyMt;
    }

    // zbq [08/31/2007] ������ֻ������Ƶ����Ƶ����˫�����޹�ϵ
    /*
	//�Ƿ�Ҫת��H.239���ݣ�����˫�����������з��������ƣ��Ժ󷢺���ЧΪ׼
	TLogicalChannel tLogicalChannel;
	if( tSrc.GetType() == TYPE_MT && 
		m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) )
	{
		//g_cMpManager.StartSwitchToSubMt( tSrc, 1, tDstMt, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE );
        g_cMpManager.StartSwitchToSubMt( tSrc, 0, tDstMt, MODE_SECVIDEO, SWITCH_MODE_BROADCAST, TRUE );
		//�µ�˫�����ն˼��룬����ǿ�Ʋ����ؼ�֡���Ƿ���Ҫ�ؼ�֡���ն���������
	}*/

    //modify bas 2
	//�ն��Ƿ�������
    TMt tSwitchSrc;
    u8  bySwitchSrcChnnl;
    tSwitchSrc.SetNull();

	if (!m_tConf.GetConfAttrb().IsUseAdapter() || SWITCH_MODE_SELECT == bySwitchMode)
	{
        tSwitchSrc = tSrc;
        bySwitchSrcChnnl = bySrcChnnl;
	}
	else
	{
        if (MODE_AUDIO != byMediaMode && m_tConf.m_tStatus.IsBrdstVMP())
        {
            TSimCapSet tSimCapSet; 
            u8 byVmpSrcChl = 0;
            u8 byMediaType = m_tConf.GetMainVideoMediaType();
            u16 wDstMtReqBitrate = m_ptMtTable->GetMtReqBitrate(byDstMtId);

            TCapSupport tCap;
			/*
            //�ȴ���vmp˫�ٵ���ʽ�����
            if (0 != m_tConf.GetSecBitRate() && 
                MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
            {
                //����2·
                if(wDstMtReqBitrate < m_tConf.GetBitRate())
                {
                    byVmpSrcChl = 1;
                    if (wDstMtReqBitrate < m_wBasBitrate)
                    {
                        ChangeVmpBitRate(wDstMtReqBitrate, 2);
                    }
                }
                //����1·
                else
                {
                    byVmpSrcChl = 0;
                    if (wDstMtReqBitrate < m_wVidBasBitrate)
                    {
                        ChangeVmpBitRate(wDstMtReqBitrate, 1);
                    }
                }

                //FIXME: δ�������VMP˫��
            }
            //����˫��ʽ��˫��˫��ʽ
            else
            {
                tSimCapSet = m_ptMtTable->GetDstSCS(byDstMtId);
                if (tSimCapSet.GetVideoMediaType() == byMediaType)
                {
                    byVmpSrcChl = 0;
                    if (wDstMtReqBitrate < m_wVidBasBitrate)
                    {
                        ChangeVmpBitRate(wDstMtReqBitrate, 1);
                    }
                }
                else
                {
                    byVmpSrcChl = 1;
                    if (wDstMtReqBitrate < m_wBasBitrate)
                    {
                        ChangeVmpBitRate(wDstMtReqBitrate, 2);
                    }
                }
            }            
			bySwitchSrcChnnl = byVmpSrcChl;*/
			// xliang [8/4/2009] ��VMP�㲥����,�˴�����ֻ����VMP�㲥���ȥ
			bySwitchSrcChnnl = AdjustVmpBrdBitRate(&tDstMt);
            tSwitchSrc = m_tVmpEqp;
            
        }
        else
        {
            TSimCapSet tTmpSCS;
            TSimCapSet tSrcSCS;
            TSimCapSet tDstSCS;
            u16 wAdaptBitRate;

            if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
            {
                if (tSrc.GetType() == TYPE_MT &&
                    IsMtNeedAdapt(ADAPT_TYPE_AUD, byDstMtId, &tSrc))
                {
                    if (!m_tConf.m_tStatus.IsAudAdapting())
                    {
                        tTmpSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
                        tSrcSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
                        tTmpSCS = m_ptMtTable->GetDstSCS(byDstMtId);
                        tDstSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
                        if (!StartAdapt(ADAPT_TYPE_AUD, 0, &tDstSCS, &tSrcSCS))
                        {
                            MpManagerLog("StartSwitchToSubMt %d failed because audio Adapter cann't available!\n", byDstMtId);
                            return FALSE;
                        }
                    }                    

                    MpManagerLog("IsMtNeedAdapt(ADAPT_TYPE_AUD, %d)\n", byDstMtId);
                    tSwitchSrc = m_tAudBasEqp;
                    bySwitchSrcChnnl = m_byAudBasChnnl;
                }
                else
                {
                    tSwitchSrc = tSrc;
                    bySwitchSrcChnnl = bySrcChnnl;
                }

                if (MODE_BOTH == byMediaMode)
                {
                    MpManagerLog("IsMtNeedAdapt(ADAPT_TYPE_AUD, %d, MODE_BOTH)\n", byDstMtId);
                    g_cMpManager.StartSwitchToSubMt(tSwitchSrc, bySwitchSrcChnnl, tDstMt, MODE_AUDIO, bySwitchMode);
                    bSwitchedAudio = TRUE;
                }
            }

            if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
            {
                // �жϸ�������, zgc, 2008-08-08
                TMt tSrcTemp;
                tSrcTemp.SetNull();
                TSimCapSet tSrcSCSTemp;
                if ( IsHDConf( m_tConf ) )
                {
                    if (m_cMtRcvGrp.IsMtNeedAdp(byDstMtId) ||
                        bMMcuNeedAdp)
                    {
						tSwitchSrc = tSrc;
						bySwitchSrcChnnl = bySrcChnnl;
                    }
                    else
                    {
                        tSwitchSrc = m_tVidBrdSrc;
                        bySwitchSrcChnnl = m_tVidBrdSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
                    }
                }
                else
                if (IsMtNeedAdapt(ADAPT_TYPE_VID, byDstMtId, &tSrc))
                {
                    if (!m_tConf.m_tStatus.IsVidAdapting())
                    {
                        if (IsNeedVidAdapt(tDstSCS, tSrcSCS, wAdaptBitRate, &tSrc))
                        {
                            //��Ҫ�����Ƿ����StartAdapt()���
                            if (!StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSCS, &tSrcSCS))
                            {
                                MpManagerLog("StartSwitchToSubMt %d failed because video Adapter cann't available!\n", byDstMtId);
                                return FALSE;
                            }
                        }
                    }
                    MpManagerLog("IsMtNeedAdapt(ADAPT_TYPE_VID, %d)\n", byDstMtId);
                    tSwitchSrc = m_tVidBasEqp;
                    bySwitchSrcChnnl = m_byVidBasChnnl;
                }
                else if (IsMtNeedAdapt(ADAPT_TYPE_BR, byDstMtId, &tSrc))
                {
                    if (!m_tConf.m_tStatus.IsBrAdapting())
                    {
                        if (IsNeedBrAdapt(tDstSCS, tSrcSCS, wAdaptBitRate, &tSrc))
                        {
                            if (!StartAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSCS, &tSrcSCS))
                            {
                                MpManagerLog("StartSwitchToSubMt %d failed because bitrate Adapter cann't available!\n", byDstMtId);
                                return FALSE;
                            }
                        }
                    }
                    MpManagerLog("IsMtNeedAdapt(ADAPT_TYPE_BR, %d)\n", byDstMtId);
                    tSwitchSrc = m_tBrBasEqp;
                    bySwitchSrcChnnl = m_byBrBasChnnl;
                }
                else
                {
                    tSwitchSrc = tSrc;
                    bySwitchSrcChnnl = bySrcChnnl;
                }
            }
        }        
    }

    if (!tSwitchSrc.IsNull())
    {
        u8 byTmpMediaMode = byMediaMode;
        if (MODE_BOTH == byMediaMode && bSwitchedAudio) //������Ƶ�ظ�����
        {
            byTmpMediaMode = MODE_VIDEO;
        }
        
        //zbq [08/07/2007] BAS�����ն˵Ľ���,����ƽ������,������StartStop����.
        BOOL32 bStopBeforeStart = TRUE;
        if ( (tSwitchSrc.GetType()    == TYPE_MCUPERI &&
              tSwitchSrc.GetEqpType() == EQP_TYPE_BAS ) ||
             (tSwitchSrc.GetType()    == TYPE_MT &&
              tSwitchSrc.GetEqpType() == MT_TYPE_SMCU) )
        {
            bStopBeforeStart = FALSE;
        }

		if (!g_cMpManager.StartSwitchToSubMt(tSwitchSrc, bySwitchSrcChnnl, tDstMt, byTmpMediaMode, bySwitchMode, FALSE, bStopBeforeStart))
		{
			MpManagerLog( "StartSwitchToSubMt() failed! Cannot switch to specified sub mt!\n" );
			return FALSE;
		}

        if (m_tConf.GetConfAttrb().IsResendLosePack())
        {
            //����ʱ�Ķ����ش�(�ڽ���bas��>dstmt��rtpʱ������dstmt��>prs��rtcp)
            if (EQP_TYPE_BAS == tSwitchSrc.GetEqpType())
            {                     
                if (tSwitchSrc == m_tVidBasEqp && (MODE_BOTH == byMediaMode || MODE_VIDEO == byMediaMode))
                {
                    AddRemovePrsMember(tDstMt, TRUE, PRSCHANMODE_VIDBAS);
                }            
                else if (tSwitchSrc == m_tBrBasEqp && (MODE_BOTH == byMediaMode || MODE_VIDEO == byMediaMode))
                {               
                    AddRemovePrsMember(tDstMt, TRUE, PRSCHANMODE_BRBAS);
                } 
                
                if (tSwitchSrc == m_tAudBasEqp && (MODE_BOTH == byMediaMode || MODE_AUDIO == byMediaMode))
                {     
                    AddRemovePrsMember(tDstMt, TRUE, PRSCHANMODE_AUDBAS);
                }

                // FIXME���������䶪���ش�δ���,zgc
            }
            
            //vmp.2
//             if (0 != m_tConf.GetSecBitRate() && 
//                 MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType() &&
//                 m_tVidBrdSrc == m_tVmpEqp)
//             {
//                 if (tSwitchSrc == m_tVmpEqp && 1 == bySwitchSrcChnnl &&
//                     (MODE_BOTH == byMediaMode || MODE_VIDEO == byMediaMode))
//                 {
//                     AddRemovePrsMember(tDstMt, TRUE, PRSCHANMODE_VMP2);
//                 }
//             }
			
			if( m_tVidBrdSrc == m_tVmpEqp )
			{
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				if( byVmpSubType != VMP )
				{
					// xliang [4/28/2009] FIXME ����ͨ��bySwitchSrcChnnl��prs����
					 u8 abyPrsModVmp[4] = {PRSCHANMODE_VMPOUT1, PRSCHANMODE_VMPOUT2, PRSCHANMODE_VMPOUT3, PRSCHANMODE_VMPOUT4};
					 AddRemovePrsMember(tDstMt, TRUE, abyPrsModVmp[bySwitchSrcChnnl]);

					// xliang [5/7/2009] temporarily test version. The final version is above
					/*
					 if(bySwitchSrcChnnl == 0) //0��
					{
						if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut2 )
						{
							AddRemovePrsMember(tDstMt, TRUE, PRSCHANMODE_VMPOUT2);
						}
						else
						{
							AddRemovePrsMember(tDstMt, TRUE, PRSCHANMODE_VMPOUT1);
						}
					}
					else if(bySwitchSrcChnnl == 2)
					{
						AddRemovePrsMember(tDstMt, TRUE, PRSCHANMODE_VMPOUT4);
					}
					// end temporary version
					*/

				}
				
			}
        }        
    }

	//֪ͨ�ն˿�ʼ���� 
	if( tSrc.GetType() == TYPE_MT )
	{
        if (bSndFlowCtrl)
        {
            NotifyMtSend( tSrc.GetMtId(), byMediaMode, TRUE );
        }		

        // zbq [06/25/2007] ����Ƶ����������ؼ�֡
        if ( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode )
        {
            NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
        }
	}
    else
    {
        //zbq[05/07/2009] �����ش� �������� �������ؼ�֡
        if ((MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode) &&
            tSrc.GetEqpId() == m_tCasdVidBasEqp.GetEqpId() &&
            byDstMtId == m_tCascadeMMCU.GetMtId())
        {
            NotifyFastUpdate(tSrc, 0);
            NotifyFastUpdate(tSrc, 1);
        }
    }
	
	// xliang [6/16/2009] FIXME: ��MPU����ؼ�֡��
	
	//�����ն˵�������ʾ��ʶ
	TPeriEqpStatus tEqpStatus;
	if( tSrc == m_tPlayEqp )
	{
		g_cMcuVcApp.GetPeriEqpStatus( m_tPlayEqp.GetEqpId(), &tEqpStatus );
		cServMsg.SetMsgBody( (u8*)tEqpStatus.GetAlias(), MAXLEN_EQP_ALIAS );
		SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
	}
	//�����VMP,�����������ʾ��ʶ
	if( tSrc == m_tVmpEqp )
	{
		u8 abyVmpAlias[8];
		abyVmpAlias[0] = ' ';
		abyVmpAlias[1] = '\0';
		cServMsg.SetMsgBody( abyVmpAlias, sizeof(abyVmpAlias) );
		SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
	}

    
    //zbq[01/01/2009] ����bas�������Ե�������Դ����������������֪ͨ����
    if (tSrc == m_tAudBasEqp || tSrc == m_tVidBasEqp || tSrc == m_tBrBasEqp /*|| 
             ( tSrc.GetType() == TYPE_MCUPERI && 
             tSrc.GetEqpType() == EQP_TYPE_BAS &&
             g_cMcuAgent.IsEqpBasHD( tSrc.GetEqpId() ) ) */)
	{
		return TRUE;
	}

	//֪ͨ�ն˿�ʼ����
	if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
	{
		NotifyMtReceive( tSrc, tDstMt.GetMtId() );
	}

    if (tSrc.GetType() == TYPE_MCUPERI && 
        tSrc.GetEqpType() == EQP_TYPE_BAS &&
        g_cMcuAgent.IsEqpBasHD(tSrc.GetEqpId()))
    {
		m_ptMtTable->SetMtSrc(byDstMtId, &m_tVidBrdSrc, byMediaMode);
		MtStatusChange(byDstMtId, TRUE);
        return TRUE;
    }
		
	// xsl [5/22/2006]�����ն�״̬, ���ﱣ��㲥Դ��Ϣ��������bas��Ϣ����Ϊ�����ط������Դ��Ϣ����Ӧ����
	m_ptMtTable->SetMtSrc( byDstMtId, ( const TMt * )&tSrc, byMediaMode );

	//����Ŀ���ն�Ϊ�ϼ�mcu�����
	m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );
	if( tSrc.GetType() == TYPE_MT && tSrc.GetMtId() != byDstMtId && 
		(m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId()) )
	{
		TMtStatus tSrcMtStatus;
		u8 byAddSelByMcsMode = MODE_NONE;
		m_ptMtTable->GetMtStatus( tSrc.GetMtId(), &tSrcMtStatus );

        //zbq[12/06/2007] VMP����ʱ������
        if( ((!(tSrc == m_tVidBrdSrc)) || (tSrc == m_tVidBrdSrc && 
                                           ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() &&
                                           m_tRollCaller == m_tVidBrdSrc)) &&
                                           tSrcMtStatus.IsSendVideo() && 
            ( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode ) )
		{
			byAddSelByMcsMode = MODE_VIDEO;
		}
		if( !(tSrc == m_tAudBrdSrc) && tSrcMtStatus.IsSendAudio() && 
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
		}
	}
	else
	{
		tMtStatus.RemoveSelByMcsMode( byMediaMode );
	}
	m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );
	
	if( bMsgStatus )
	{
        MtStatusChange( byDstMtId, TRUE );
	}

	//����ѡ������Ŀ���ն˵���Ҫ���ڷ����ն˵�����
	if( tSrc.GetType() == TYPE_MT && bySwitchMode == SWITCH_MODE_SELECT && 
		( byMediaMode == MODE_VIDEO || byMediaMode == MODE_BOTH ) && 
	    tSrc.GetMtId() != m_tVidBrdSrc.GetMtId() )
	{
		u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( byDstMtId );
		AdjustMtVideoSrcBR( byDstMtId, wMtBitRate );
	}

	return TRUE;
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
void CMcuVcInst::StopSwitchToSubMt( u8 byDstMtId, u8 byMode,u8 bySwitchMode,BOOL32 bMsgStatus, BOOL32 bStopSelByMcs /* = TRUE*/, BOOL32 bSelSelf  )
{
	CServMsg	cServMsg;
	TMt    TMtNull;
	TMtStatus   tMtStatus;

	TMtNull.SetNull();
	TMt tDstMt = m_ptMtTable->GetMt( byDstMtId );
    m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );

    //Ŀ���ն��Ƿ��ڻ���
    if (tMtStatus.IsMediaLoop(MODE_VIDEO))
    {
        if (byMode == MODE_VIDEO)
        {
            return;
        }
        else if (byMode == MODE_BOTH)
        {
            byMode = MODE_AUDIO;
        }
    }
    
    if (tMtStatus.IsMediaLoop(MODE_AUDIO))
    {
        if (byMode == MODE_AUDIO)
        {
            return;
        }
        else if (byMode == MODE_BOTH)
        {
            byMode = MODE_VIDEO;
        }
    }

    if ( bSelSelf &&
          // Ϊͼ��ƽ����vcs�ڵ���ģʽ����ϯ�ն˲����п��Լ���ͼ���л���ֱ����һ�������ն�����
		 !(VCS_CONF == m_tConf.GetConfSource() && 
 		   byDstMtId == m_tConf.GetChairman().GetMtId() && 
 		   (VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
		    !m_cVCSConfStatus.GetCurVCMT().IsNull()) ||
		   (ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
		    m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP)))
    {
		// xliang [4/15/2009] FIXME����ϯ��ѡ��ĳ��MT�л���ѡ��VMP���ͻ��ȿ��Լ��ٿ�VMP ��
	    //֪ͨ�ն�ֹͣ���գ����Լ� 
	    if( byMode == MODE_BOTH || byMode == MODE_VIDEO )
	    {
			if( m_tConfAllMtInfo.MtJoinedConf( byDstMtId ) )
		    {                    
                NotifyMtReceive( tDstMt, byDstMtId );
		    }		
	    }
    }
	
    // zbq [06/20/2007] Ϊ��֤������ƽ���п��������ˣ��˴���ͣ��һ��������
    // �˵�����Ƶ�������������µĵ����˽�������ʱ���StartStopSwitch()֮ͣ��
    // zbq [09/11/2007] FIXME: �˴�δ���⴦��������߼������µĵ����˿�����л��Ļ������� �������ǽ��
    // if ( !(!m_tRollCaller.IsNull() && tDstMt == m_tRollCaller) )
    {
        // stop switch
        g_cMpManager.StopSwitchToSubMt( tDstMt, byMode );
    }
		
	if( bStopSelByMcs )
	{
		tMtStatus.RemoveSelByMcsMode( byMode );
	}

	m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );	
	
    // guzh [8/31/2006] Ҫ�������ƶ������棬����SetMtStatus��MtSrc�ᱻ���
	// �����ն���Ƶ״̬Ϊ���Լ�������Ƶ�� TMTNull
    if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
    {
        m_ptMtTable->SetMtSrc( byDstMtId, &TMtNull, MODE_AUDIO );    
    }
    else if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
    {
        m_ptMtTable->SetMtSrc( byDstMtId, &tDstMt, MODE_VIDEO );    
    }	

	if( bMsgStatus )
	{
        MtStatusChange( byDstMtId, TRUE );
	}
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
    BOOL32 bSwitchedAudio = FALSE;

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
	
	//ֻ���ϼ�Mcu��spyMt
	TMt tDstMt = m_ptMtTable->GetMt(byDstMtId);
	if(m_tCascadeMMCU.GetMtId() != 0 && byDstMtId == m_tCascadeMMCU.GetMtId())
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tDstMt.GetMtId());
		if( ptConfMcInfo != NULL && 
			( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
		{
			if(!(ptConfMcInfo->m_tSpyMt.GetMtId() == tSrc.GetMtId()&&
				ptConfMcInfo->m_tSpyMt.GetMcuId() == tSrc.GetMcuId()))
			{
				return FALSE;				
			}
		}
		g_cMpManager.SetSwitchBridge(tSrc, 0, byMediaMode);
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
		}
	}

    // zbq [06/29/2007] �ش�ͨ������Ƶ������ǰ��ͣ��������DS�滻����
    BOOL32 bMediaDstMMcu = FALSE;
        
    // libo [11/24/2005]
    TSimCapSet tTmpSCS;
    TSimCapSet tSrcSCS;
    TSimCapSet tDstSCS;
    u16 wAdaptBitRate = 0;
    TMt tSwitchSrc;
    u8  bySwitchSrcChnnl;
    BOOL32 bAudBasCap = TRUE;
    BOOL32 bVidBasCap = TRUE;
    if (0 == m_tCascadeMMCU.GetMtId() || byDstMtId != m_tCascadeMMCU.GetMtId())
    {

        if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
        {
            if (g_cMcuVcApp.IsPeriEqpConnected(m_tAudBasEqp.GetEqpId()) &&
                IsMtNeedAdapt(ADAPT_TYPE_AUD, byDstMtId, &tSrc))
            {
                if (!m_tConf.m_tStatus.IsAudAdapting())
                {
                    tTmpSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
                    tSrcSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
                    tTmpSCS = m_ptMtTable->GetDstSCS(byDstMtId);
                    tDstSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
                    if (!StartAdapt(ADAPT_TYPE_AUD, 0, &tDstSCS, &tSrcSCS))
                    {
                        ConfLog(FALSE, "StartSwitchToSubMt %d failed because audio Adapter cann't available!\n", byDstMtId);
                        return FALSE;
                    }
                }
                Mt2Log("IsMtNeedAdapt(ADAPT_TYPE_AUD, %d)\n", byDstMtId);
                tSwitchSrc = m_tAudBasEqp;
                bySwitchSrcChnnl = m_byAudBasChnnl;
            }
            else
            {
                tSwitchSrc = tSrc;
                bySwitchSrcChnnl = bySrcChnnl;
            }

            if (MODE_BOTH == byMediaMode)
            {
                Mt2Log("IsMtNeedAdapt(ADAPT_TYPE_AUD, %d, MODE_BOTH)\n", byDstMtId);
                g_cMpManager.StartSwitchToSubMt(tSwitchSrc, bySwitchSrcChnnl, tDstMt, MODE_AUDIO, bySwitchMode);
                bSwitchedAudio = TRUE;
            }
        }

        if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
        {
            if (g_cMcuVcApp.IsPeriEqpConnected(m_tVidBasEqp.GetEqpId()) &&
                IsMtNeedAdapt(ADAPT_TYPE_VID, byDstMtId, &tSrc))
            {
                if (!m_tConf.m_tStatus.IsVidAdapting())
                {
                    if (IsNeedVidAdapt(tDstSCS, tSrcSCS, wAdaptBitRate, &tSrc))
                    {
                        //��Ҫ�����Ƿ����StartAdapt()���
                        if (!StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSCS, &tSrcSCS))
                        {
                            ConfLog(FALSE, "StartSwitchToSubMt %d failed because video Adapter cann't available!\n", byDstMtId);
                            return FALSE;
                        }
                    }
                }
                Mt2Log("IsMtNeedAdapt(ADAPT_TYPE_VID, %d)\n", byDstMtId);
                tSwitchSrc = m_tVidBasEqp;
                bySwitchSrcChnnl = m_byVidBasChnnl;
            }
            else if (g_cMcuVcApp.IsPeriEqpConnected(m_tBrBasEqp.GetEqpId()) &&
                     IsMtNeedAdapt(ADAPT_TYPE_BR, byDstMtId, &tSrc))
            {
                if (!m_tConf.m_tStatus.IsBrAdapting())
                {
                    if (IsNeedBrAdapt(tDstSCS, tSrcSCS, wAdaptBitRate, &tSrc))
                    {
                        if (!StartAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSCS, &tSrcSCS))
                        {
                            ConfLog(FALSE, "StartSwitchToSubMt %d failed because bitrate Adapter cann't available!\n", byDstMtId);
                            return FALSE;
                        }
                    }
                }
                Mt2Log("IsMtNeedAdapt(ADAPT_TYPE_BR, %d)\n", byDstMtId);
                tSwitchSrc = m_tBrBasEqp;
                bySwitchSrcChnnl = m_byBrBasChnnl;
            }
            else
            {
                tSwitchSrc = tSrc;
                bySwitchSrcChnnl = bySrcChnnl;
            }
        }
    }
    else //!if (0 == m_tCascadeMMCU.GetMtId() || byDstMtId != m_tCascadeMMCU.GetMtId())
    {
        u8     byEqpId;
        u8     byChnIdx;

        if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
        {
            if (IsMtNeedAdapt(ADAPT_TYPE_CASDAUD, byDstMtId, &tSrc))
            {
                if (EQP_CHANNO_INVALID == m_byCasdAudBasChnnl || m_tCasdAudBasEqp.IsNull())
                {
                    if (g_cMcuVcApp.GetIdleBasChl(ADAPT_TYPE_AUD, byEqpId, byChnIdx))
                    {
                        m_tCasdAudBasEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_BAS);
                        m_tCasdAudBasEqp.SetConfIdx(m_byConfIdx);
                        m_byCasdAudBasChnnl = byChnIdx;
                        EqpLog("m_byCasdAudBasChnnl = %d\n", m_byCasdAudBasChnnl);
                    }
                    else
                    {
                        bAudBasCap = FALSE;
                        ConfLog(FALSE, "no idle cascade audio adapte channel!\n");
                    }
                }

                if (FALSE == bAudBasCap)
                {
                    tSwitchSrc = tSrc;
                    bySwitchSrcChnnl = bySrcChnnl;
                }
                else
                {
                    if (!m_tConf.m_tStatus.IsCasdAudAdapting())
                    {
                        tSrcSCS.Clear();
                        tDstSCS.Clear();
                        tTmpSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
                        tSrcSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
                        tTmpSCS = m_ptMtTable->GetDstSCS(byDstMtId);
                        tDstSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
                        if (!StartAdapt(ADAPT_TYPE_CASDAUD, 0, &tDstSCS, &tSrcSCS))
                        {
                            ConfLog(FALSE, "StartSwitchToSubMt %d failed because video Adapter cann't available!\n", byDstMtId);
                            bAudBasCap = FALSE;
                        }
                    }
                    if (FALSE == bAudBasCap)
                    {
                        tSwitchSrc = tSrc;
                        bySwitchSrcChnnl = bySrcChnnl;
                    }
                    else
                    {
                        tSwitchSrc = m_tCasdAudBasEqp;
                        bySwitchSrcChnnl = m_byCasdAudBasChnnl;
                    }
                }

            }
            else
            {
                tSwitchSrc = tSrc;
                bySwitchSrcChnnl = bySrcChnnl;
            }

            if (MODE_BOTH == byMediaMode)
            {
                Mt2Log("IsMtNeedAdapt(ADAPT_TYPE_AUD, %d, MODE_BOTH)\n", byDstMtId);
                g_cMpManager.StartSwitchToSubMt(tSwitchSrc, bySwitchSrcChnnl, tDstMt, MODE_AUDIO, bySwitchMode);
                bSwitchedAudio = TRUE;
            }
        }

        if ((tSwitchSrc == m_tCasdAudBasEqp) && (bySwitchSrcChnnl == m_byCasdAudBasChnnl))
        {
            if (!g_cMpManager.StartSwitchToPeriEqp(tSrc, bySrcChnnl, m_tCasdAudBasEqp.GetEqpId(), m_byCasdAudBasChnnl, MODE_AUDIO))
            {
                ConfLog( FALSE, "[StartSwitchToMcu]StartSwitchToPeriEqp() failed! - m_tCasdAudBasEqp\n" );
                return FALSE;
            }
        }

        BOOL32 bHDCasNeedAdp = FALSE;
        if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
        {
            //������鶯̬ռ��
            if (!IsHDConf(m_tConf))
            {
                BOOL32 bCasdVid = IsMtNeedAdapt(ADAPT_TYPE_CASDVID, byDstMtId, &tSrc);
                // xsl [9/29/2006] �����������������䣬ͨ��flowcontrol���ڻش��ն˷�������
                //            BOOL32 bCasdBr = IsMtNeedAdapt(ADAPT_TYPE_BR, byDstMtId, tSrc);
                if (bCasdVid/* || bCasdBr*/)
                {
                    if (EQP_CHANNO_INVALID == m_byCasdVidBasChnnl || m_tCasdVidBasEqp.IsNull())
                    {
                        if (g_cMcuVcApp.GetIdleBasChl(ADAPT_TYPE_VID, byEqpId, byChnIdx))
                        {
                            m_tCasdVidBasEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_BAS);
                            m_tCasdVidBasEqp.SetConfIdx(m_byConfIdx);
                            m_byCasdVidBasChnnl = byChnIdx;
                            EqpLog("m_byCasdVidBasChnnl = %d\n", m_byCasdVidBasChnnl);
                        }
                        else
                        {
                            bVidBasCap = FALSE;
                            ConfLog(FALSE, "no idle cascade video adapte channel!\n");
                        }
                    }
                    
                    if (FALSE == bVidBasCap)
                    {
                        tSwitchSrc = tSrc;
                        bySwitchSrcChnnl = bySrcChnnl;
                    }
                    else
                    {
                        if (!m_tConf.m_tStatus.IsCasdVidAdapting() && (bCasdVid/* || bCasdBr*/))
                        {
                            tSrcSCS.Clear();
                            tDstSCS.Clear();
                            tTmpSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
                            tSrcSCS.SetVideoMediaType(tTmpSCS.GetVideoMediaType());
                            tTmpSCS = m_ptMtTable->GetDstSCS(byDstMtId);
                            tDstSCS.SetVideoMediaType(tTmpSCS.GetVideoMediaType());
                            wAdaptBitRate = m_ptMtTable->GetMtReqBitrate(byDstMtId);
                            if (wAdaptBitRate > m_ptMtTable->GetMtSndBitrate(tSrc.GetMtId()))
                            {
                                wAdaptBitRate = m_ptMtTable->GetMtSndBitrate(tSrc.GetMtId());
                            }
                            if (!StartAdapt(ADAPT_TYPE_CASDVID, wAdaptBitRate, &tDstSCS, &tSrcSCS))
                            {
                                ConfLog(FALSE, "StartSwitchToMcu %d failed because cascade video Adapter not available!\n", byDstMtId);
                                bVidBasCap = FALSE;
                            }
                        }
                        if (FALSE == bVidBasCap)
                        {
                            tSwitchSrc = tSrc;
                            bySwitchSrcChnnl = bySrcChnnl;
                        }
                        else
                        {
                            tSwitchSrc = m_tCasdVidBasEqp;
                            bySwitchSrcChnnl = m_byCasdVidBasChnnl;
                        }
                    }
                }        
                else
                {
                    tSwitchSrc = tSrc;
                    bySwitchSrcChnnl = bySrcChnnl;
                }
            }
            //zbq[03/26/2009]����������ñ������ķ�ʽ�ͱ�ʶ ���� 
            else
            {
                if (MT_TYPE_MT != tSrc.GetMtType() &&
                    MT_TYPE_SMCU != tSrc.GetMtType())
                {
                    ConfLog(FALSE, "[StartSwitchToMcu] Mt can be spy only, ignore<%d, %d>!\n", tSrc.GetMcuId(), tSrc.GetMtId());
                    return FALSE;
                }

                TSimCapSet tSrcSimCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
                TSimCapSet tDstSimCap = m_ptMtTable->GetDstSCS(byDstMtId);

                if (tSrcSimCap.IsNull())
                {
                    ConfLog(FALSE, "[StartSwitchToMcu] tSrc.%d primary logic chan unexist!\n", tSrc.GetMtId());
                    return FALSE;
                }
                tSrcSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtSndBitrate(tSrc.GetMtId()));

                if (tDstSimCap.IsNull())
                {
                    ConfLog(FALSE, "[StartSwitchToMcu] tDst.%d back logic chan unexist!\n", byDstMtId);
                    return FALSE;
                }
                tDstSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtReqBitrate(byDstMtId));
                
                //FIXME: ��������δ����֡�ʣ����ܻ�©����֡�����䣨������һ�㲻֡�����䣩

                bHDCasNeedAdp = tDstSimCap < tSrcSimCap;

                if (bHDCasNeedAdp)
                {
                    //δ�������䣬�Ȳ��ң��Ѿ�������֡�����䣬���������ͱ��������
                    if (m_tCasdVidBasEqp.IsNull())
                    {
                        //��̬��ռ���е�MAU, û�п����򲻻ش���һ��ռ�ã�ֱ�������������������ͷţ�
                        u8 byEqpId = 0;
                        u8 byChnId = 0;
                        if (!g_cMcuVcApp.GetIdleHDBasVidChl(byEqpId, byChnId))
                        {
                            ConfLog(FALSE, "[StartSwitchToMcu] switch failed due to no Idle Chn!\n");
                            return FALSE;
                        }
                        m_tCasdVidBasEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_BAS);
                        m_tCasdVidBasEqp.SetConfIdx(m_byConfIdx);
                        m_byCasdVidBasChnnl = byChnId;
                    }
                    tSwitchSrc = tSrc;
                    bySwitchSrcChnnl = bySrcChnnl;
                }
                else
                {
                    tSwitchSrc = tSrc;
                    bySwitchSrcChnnl = bySrcChnnl;
                }
            }
        }

        if (IsHDConf(m_tConf) && bHDCasNeedAdp)
        {
            if (!StartHDCascadeAdp(&tSwitchSrc))
            {
                ConfLog(FALSE, "[StartSwitchToMcu] StartHDCascadeAdp() failed!\n" );
                return FALSE;
            }
        }
        else
        {
            if ((tSwitchSrc == m_tCasdVidBasEqp) && (bySwitchSrcChnnl == m_byCasdVidBasChnnl))
            {
                if (!g_cMpManager.StartSwitchToPeriEqp(tSrc, bySrcChnnl, m_tCasdVidBasEqp.GetEqpId(), m_byCasdVidBasChnnl, MODE_VIDEO))
                {
                    ConfLog( FALSE, "[StartSwitchToMcu]StartSwitchToPeriEqp() failed! - m_tCasdVidBasEqp\n" );
                    return FALSE;
                }
            }
        }
    }

    if (!tSwitchSrc.IsNull())
    {
        u8 byTmpMediaMode = byMediaMode;
        if (MODE_BOTH == byMediaMode && bSwitchedAudio) //������Ƶ�ظ�����
        {
            byTmpMediaMode = MODE_VIDEO;
        }
        
        BOOL32 bStopBeforeStart = bMediaDstMMcu ? FALSE : TRUE;

        if (!g_cMpManager.StartSwitchToSubMt(tSwitchSrc, bySwitchSrcChnnl, tDstMt, byTmpMediaMode, bySwitchMode, FALSE, bStopBeforeStart))
        {
            ConfLog( FALSE, "StartSwitchToSubMt() failed! Cannot switch to specified sub mt!\n" );
            return FALSE;
        }
    }
    // libo [11/24/2005]end

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

    //modify bas 2
	//if( tSrc == m_tBrBasEqp || tSrc == m_tRtpBasEqp )
    if (tSrc == m_tAudBasEqp || tSrc == m_tVidBasEqp || tSrc == m_tBrBasEqp)
	{
		return TRUE;
	}

	//֪ͨ�ն˿�ʼ����
	if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
	{
		NotifyMtReceive( tSrc, tDstMt.GetMtId() );
	}

	//�����ն�״̬
	m_ptMtTable->SetMtSrc( byDstMtId, ( const TMt * )&tSrc, byMediaMode );

	//����Ŀ���ն�Ϊ�ϼ�mcu�����
	m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );
	if( tSrc.GetType() == TYPE_MT && tSrc.GetMtId() != byDstMtId && 
		(m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId()) )
	{
		TMtStatus tSrcMtStatus;
		u8 byAddSelByMcsMode = MODE_NONE;
		m_ptMtTable->GetMtStatus( tSrc.GetMtId(), &tSrcMtStatus );

		if( !(tSrc == m_tVidBrdSrc) && tSrcMtStatus.IsSendVideo() && 
			( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode ) )
		{
			byAddSelByMcsMode = MODE_VIDEO;
		}
		if( !(tSrc == m_tAudBrdSrc) && tSrcMtStatus.IsSendAudio() && 
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
		}
	}
	else
	{
		tMtStatus.RemoveSelByMcsMode( byMediaMode );
	}
	m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );
	
	if( bMsgStatus )
	{
        MtStatusChange(byDstMtId, TRUE);
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
        TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tDst.GetMtId());
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
    TSimCapSet tTmpSrcSCS;
    if ( IsHDConf( m_tConf ) )
    {
        if (m_cMtRcvGrp.IsMtNeedAdp(byMtId))
        {
            return FALSE;
        }
    }
    else
    //�Ƿ���Ҫ������ܽ���
    if ( IsMtNeedAdapt( ADAPT_TYPE_BR, byMtId, &tTmpSrc  )  ||
         IsMtNeedAdapt( ADAPT_TYPE_VID, byMtId, &tTmpSrc ) )
    {
        return FALSE;
    }

	// ���Ӷ�˫��ʽVMP���ж�, zgc, 20070604
	if( tStatus.IsReceiveVideo() && m_tConf.m_tStatus.IsBrdstVMP() )
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
BOOL32 CMcuVcInst::StartSwitchFromBrd(const TMt &tSrc,
                                      u8        bySrcChnnl,
                                      u8        byDstMtNum,
                                      const TMt* const ptDstBase)
{
    u8 byLoop = 0;

    // zgc, 2008-05-28, ��ѡ��
    TMtStatus tMtStatus;
    u8 bySelMode = MODE_NONE;
    for ( byLoop = 0; byLoop < byDstMtNum; byLoop ++ )
    {
        // zgc, 2008-06-02, �����˺ͱ������˲��ӹ㲥����������ѡ��
        if ( m_tVidBrdSrc == m_tVmpEqp && 
            ( ptDstBase[byLoop] == m_tRollCaller ||
            ptDstBase[byLoop] == GetLocalMtFromOtherMcuMt(m_tRollCallee)) )
        {
            continue;
        }

        m_ptMtTable->GetMtStatus(ptDstBase[byLoop].GetMtId(), &tMtStatus);
        bySelMode = tMtStatus.GetSelByMcsDragMode();
        if ( MODE_VIDEO == bySelMode || MODE_BOTH == bySelMode )
        {	
			u8 byIsRestore = FALSE;
			if(!m_tVidBrdSrc.IsNull() && tSrc == m_tVidBrdSrc)
			{
				byIsRestore = TRUE;
			}
            StopSelectSrc(ptDstBase[byLoop], MODE_VIDEO, byIsRestore);
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
            if ( !( m_tVidBrdSrc == m_tVmpEqp && 
                    ( ptDstBase[0] == m_tRollCaller ||
                      ptDstBase[0] == GetLocalMtFromOtherMcuMt(m_tRollCallee)) ) )
            {
				
				if (!m_tCascadeMMCU.IsNull()  &&
					m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId())->m_tSpyMt == tSrc &&
					m_tCascadeMMCU == ptDstBase[0] )
				{
					// xliang [3/2/2009] ����src�������ڻش�ͨ����򲻲𽻻�
					MpManagerLog("[StartSwitchFromBrd] Since the source is also under back-to-MMCU switch, we shouldn't stop switch in this case.\n" );
				}
				else
				{
					// ���BUG10501, �������Ƶ�����,��֤ͣ--��������Ӧ, zgc, 2008-04-22
					StopSwitchToSubMt( ptDstBase[0].GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE, FALSE );
				}
				
                bRet = g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, ptDstBase);
            }
            else
            {
                MpManagerLog("[StartSwitchFromBrd] Mt.%d is in Rollcall, no accept from brd(DstNum=1)\n", ptDstBase[0].GetMtId() );
                bRet = TRUE;                
            }
		}
		else
		{
			// ������
            if (!(m_cMtRcvGrp.IsMtNeedAdp(ptDstBase[0].GetMtId()) &&
				 StartSwitchToSubMtFromAdp(ptDstBase[0].GetMtId())))
			{
				bRet = StartSwitchToSubMt( tSrc, bySrcChnnl, ptDstBase[0].GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE );
			}
			return bRet;
		}
	}
    else if( byDstMtNum > 1 )
    {
        //zbq[03/29/2008] ������յĹ㲥Ŀ�꣬���˵����˺ͱ���������VMP�����
        if ( m_tVidBrdSrc == m_tVmpEqp &&
             ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
        {
            for( byLoop = 0; byLoop < byDstMtNum; byLoop ++ )
            {
                if ( ptDstBase[byLoop] == m_tRollCaller ||
                     ptDstBase[byLoop] == GetLocalMtFromOtherMcuMt(m_tRollCallee))
                {
                    continue;
                }
                else
                {
                    bRet &= g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, 1, &ptDstBase[byLoop]);
                }
            }
        }
        else
        {
            bRet = g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, ptDstBase);
        }
    }
	else
	{
		return FALSE;
	}

    //zbq[04/15/2009] Tand�Ĺؼ�֡��ıȽϿ죬�Ҽ��Ϊ3s. �ʽ����������ٷ�youareseeing�������õ���һ���ؼ�֡
    if (IsDelayVidBrdVCU() && tSrc == m_tVidBrdSrc)
    {
    }
    else
    {
        //guzh [2008/03/31] ����ؼ�֡
        if (TYPE_MT == tSrc.GetType())
        {
            NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
        }
    }

    CServMsg cServMsg;
    TPeriEqpStatus tEqpStatus;
    //TMtStatus tMtStatus;
    for (byLoop = 0; byLoop < byDstMtNum; byLoop ++)
    {
        // zbq [06/19/2007] �����˺ͱ������˲��ӹ㲥����
        if ( !( m_tVidBrdSrc == m_tVmpEqp && 
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

            m_ptMtTable->SetMtSrc( ptDstBase[byLoop].GetMtId(), &tSrc, MODE_VIDEO );
        }
        else
        {
            // do nothing
            MpManagerLog("[StartSwitchFromBrd] Mt.%d is in Rollcall, no accept from brd\n", ptDstBase[byLoop].GetMtId() );
        }
    }
    
    MtStatusChange();
    
    return bRet;

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
BOOL32 CMcuVcInst::StartSwitchToAllSubMtJoinedConf(const TMt &tSrc, u8 bySrcChnnl, const TMt &tOldSrc, BOOL32 bForce)
{
    // ��ӹ㲥Դ
    if ( TYPE_MT == tSrc.GetType() )
    {
        if ( !m_ptMtTable->IsLogicChnnlOpen(tSrc.GetMtId(), LOGCHL_VIDEO, FALSE) )
        {
            MpManagerLog("[StartSwitchToAllSubMtJoinedConf]Source MT.%d is not sending video!\n",tSrc.GetMtId());
            return FALSE;
        }
    }
    g_cMpManager.StartSwitchToBrd(tSrc, bySrcChnnl, bForce);
    
    TMtStatus tStatus;

    TMt atMtList[MAXNUM_CONF_MT];
    u8 byDstMtNum = 0;

    u8 byLoop = 1;
    TMt tLoopMt;
    for ( ; byLoop <= MAXNUM_CONF_MT; byLoop++ )
    {
        if (!m_tConfAllMtInfo.MtJoinedConf(byLoop))
        {
            continue;
        }

		// ����VCS����,��ϯ��ѯʱ,��ϯ�����㲥Դ
		if (VCS_CONF == m_tConf.GetConfSource() &&
			m_tConf.GetChairman().GetMtId() == byLoop &&
			VCS_POLL_START == m_cVCSConfStatus.GetChairPollState())
		{
			continue;
		}

        tLoopMt = m_ptMtTable->GetMt(byLoop);
        if ( !CanMtRecvVideo( tLoopMt, tSrc ) )
        {
            // guzh [5/11/2007] ����ͨ���㲥�����ģ�����ͨ�����������Զ������䣩
			// �����˲�����ͨ�����߼�, zgc, 2008-04-12
			if ( !(tLoopMt == GetLocalMtFromOtherMcuMt(m_tConf.GetSpeaker())) )
			{
                //zbq[01/01/2009] ����������Ե���������������������������������Զ�������
				if (!(IsHDConf(m_tConf) && m_cMtRcvGrp.IsMtNeedAdp(tLoopMt.GetMtId())))
                {
                    StartSwitchToSubMt( tSrc, bySrcChnnl, byLoop, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE );
                }
			}
            continue;
        }
        
        atMtList[byDstMtNum] = tLoopMt;
        byDstMtNum ++;
    }

    //��������
    
    //���岿��
    if ( byDstMtNum > 0)
    {
        StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, atMtList);
    }
    
    //��������������������������ݻ������Խ�����Ӧ�ش�����
	ChangeSpeakerSrc( MODE_VIDEO, emReasonChangeBrdSrc );
    
    MtStatusChange();
    
	return TRUE;
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
BOOL32 CMcuVcInst::StartSwitchToAllSubMtJoinedConf(const TMt &tSrc, u8 bySrcChnnl)
{
	u8	byLoop;
   
    BOOL32 bResult[MAXNUM_CONF_MT];
    BOOL32 bResultAnd = TRUE;
    for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
    {
        if( m_tConfAllMtInfo.MtJoinedConf( byLoop ))
        {
			// ����������Դ
			if ( byLoop == GetLocalSpeaker().GetMtId() && GetLocalSpeaker().GetType() == TYPE_MT)
			{
				//�·���������Դ�����߼�, zgc, 2008-04-12
				ChangeSpeakerSrc( MODE_AUDIO, emReasonChangeBrdSrc );
			}
            else
            {
            	//������Ҫ��Ƶ������նˣ���Ƶͬ����Ҫ���л��崦���Ա�֤����ͬ��
                TLogicalChannel tVidChn;
                BOOL32 bNeedAdp = IsHDConf(m_tConf) &&
                                  m_cMtRcvGrp.IsMtNeedAdp(byLoop) &&
                                  m_ptMtTable->GetMtLogicChnnl(byLoop, LOGCHL_VIDEO, &tVidChn, TRUE) &&
                                  !IsMtNeedAdapt(ADAPT_TYPE_AUD, byLoop);
                if (bNeedAdp)
                {
                    continue;
                }
                bResult[byLoop-1] = StartSwitchToSubMt(tSrc,
                                                       bySrcChnnl,
                                                       byLoop,
                                                       MODE_AUDIO,
                                                       SWITCH_MODE_BROADCAST,
                                                       FALSE, FALSE, FALSE);
            }
            
            bResultAnd = bResultAnd && bResult[byLoop-1];
        }
    }
    
    MtStatusChange();
    
	return bResultAnd;
}


/*====================================================================
    ������      ��StartSwitchToAllSubMtJoinedConfNeedAdapt
    ����        ����ָ���ն����ݽ������������������ֱ���������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt & tSrc, Դ
				  u8 bySrcChnnl, Դ���ŵ���
				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/27    1.0         LI Yi         ����
	03/07/23    2.0         ������        �޸�
	04/03/22    3.0         ������        �޸�
====================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::StartSwitchToAllSubMtJoinedConfNeedAdapt(TMt tSrc, u8 bySrcChnnl, u8 byMode, u8 byAdaptType)
{
    u8 byLoop;
    TConfMtInfo tConfMtInfo;

    tSrc = GetLocalMtFromOtherMcuMt(tSrc);

    if (!m_tConf.m_tStatus.IsVidAdapting() &&
        !m_tConf.m_tStatus.IsBrAdapting() &&
        !m_tConf.m_tStatus.IsAudAdapting())
    {
        return FALSE;
    }

    BOOL32 bResult[MAXNUM_CONF_MT];
    BOOL32 bResultAnd = TRUE;
    //TMt tDstMt;
    for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && tSrc.GetMtId() != byLoop)
        {
            //tDstMt = m_ptMtTable->GetMt(byLoop);
            Mt2Log("\n\nJoined mtid.%d byAdaptType = %d\n", byLoop, byAdaptType);
            if (IsMtNeedAdapt(byAdaptType, byLoop, &tSrc))
            {
                Mt2Log("need adapt mtid.%d byAdaptType = %d\n", byLoop, byAdaptType);
                bResult[byLoop] = StartSwitchToSubMt(tSrc, bySrcChnnl, byLoop, byMode, SWITCH_MODE_BROADCAST, FALSE);
                bResultAnd = bResultAnd && bResult[byLoop-1];
            }
            Mt2Log("\n\n");
        }
    }

    MtStatusChange();

    return bResultAnd;
}


/*=============================================================================
    �� �� ���� StartSwitchToRecNeedHDVidAdapt
    ��    �ܣ� 
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/8/27   4.0		    �ܹ��                  ����
    2008/12/11  4.5         �ű���                  ��Ӧ�²��Ե���
=============================================================================*/
BOOL32 CMcuVcInst::StartSwitchToRecNeedHDVidAdapt( void )
{
    if ( !IsRecordSrcHDBas() || m_tConf.m_tStatus.IsNoRecording() )
    {
        return TRUE;
    }

    if (0 == m_tConf.GetBitRate())
    {
        return FALSE;
    }

    TEqp tLBEqp;
    u8 byOutIdx = 0;
    BOOL32 bRet = m_cBasMgr.GetLowBREqp(m_tConf, tLBEqp, byOutIdx);

    if (!bRet)
    {
        ConfLog( FALSE, "[StartSwitchToRecNeedHDVidAdapt] get LB Bas failed!\n" );
        return FALSE;
    }

    StartSwitchToPeriEqp(tLBEqp, byOutIdx, m_tRecEqp.GetEqpId(), m_byRecChnnl, MODE_VIDEO);

    return TRUE;
}

/*====================================================================
    ������      ��StartSwitchToSubMtNeedAdp
    ����        �������ݽ����������� �ӱ�����ͨ���������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/20    4.5         �ű���          ����
    09/04/17    4.6         �ű���          ��д������mpu
    09/07/26    4.6 				�ű��� 				  ���Ӷ���Ƶ��������ش���
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId)
{
    u8 byMediaMode = MODE_NONE;
    u8 bySwitchMode = SWITCH_MODE_NONE;

    u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnId);
    
    if (!CheckHdBasChnMode(byChnMode, byMediaMode, bySwitchMode))
    {
        ConfLog(FALSE, "[StartSwitchToSubMtNeedAdp] BasChn<%d, %d> unexist!\n", 
                        tEqp.GetEqpId(), byChnId);
        return FALSE;
    }

    THDBasVidChnStatus tStatus;
    memset(&tStatus, 0, sizeof(tStatus));

    if (!m_cBasMgr.GetChnStatus(tEqp, byChnId, tStatus))
    {
        ConfLog(FALSE, "[StartSwitchToSubMtNeedAdp] eqp<%d,%d> status failed\n", 
                        tEqp.GetEqpId(), byChnId);
        return FALSE;
    }

    u8 byOutIdx = 0;
    switch (byChnMode)
    {
    //1.�����㲥
    case CHN_ADPMODE_MVBRD:
        {
        for(byOutIdx = 0; byOutIdx < MAXNUM_VOUTPUT; byOutIdx++)
        {
            THDAdaptParam tAdpParam = *tStatus.GetOutputVidParam(byOutIdx);
            
            if (tAdpParam.IsNull())
            {
                continue;
            }            
            
            u8 byMVType = tAdpParam.GetVidType();
            u16 wWidth  = tAdpParam.GetWidth();
            u16 wHeight = tAdpParam.GetHeight();
            u8 byRes = GetResByWH(wWidth, wHeight);
            
            u8 byNum = 0;
            u8 abyMt[MAXNUM_CONF_MT];
            m_cMtRcvGrp.GetMVMtList(byMVType, byRes, byNum, abyMt);
            
            u8 byMtIdx = 0;
            for(; byMtIdx < byNum; byMtIdx ++)
            {
                //���������Լ�
                if (m_tVidBrdSrc.GetMtType() == MT_TYPE_MT &&
                    m_tVidBrdSrc.GetMtId() == abyMt[byMtIdx])
                {
                    continue;
                }
                //���������ϼ�MCU
                if (m_tCascadeMMCU.GetMtId() == abyMt[byMtIdx])
                {
                    continue;
                }

#ifndef _SATELITE_
                StartSwitchToSubMt(tEqp, 
                                   byOutIdx+byChnId*MAXNUM_VOUTPUT,
                                   abyMt[byMtIdx], MODE_VIDEO);
                                   

				//��Ƶ���洦��
                StartSwitchAud2MtNeedAdp(abyMt[byMtIdx], bySwitchMode);

#else
				BOOL32 bVidSrcHd = ::topoGetMtInfo(LOCAL_MCUID, m_tVidBrdSrc.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
				if (bVidSrcHd)
				{
					g_cMpManager.StartSatConfCast(tEqp, CAST_SEC, MODE_VIDEO, byChnId*MAXNUM_VOUTPUT+byOutIdx);
				}
				else
				{
					g_cMpManager.StartSatConfCast(tEqp, CAST_FST, MODE_VIDEO, byChnId*MAXNUM_VOUTPUT+byOutIdx);
				}
				m_ptMtTable->SetMtSrc(abyMt[byMtIdx], &tEqp, MODE_VIDEO);
#endif

            }

            //zbq[01/05/2009] ��ʱ��������������Դ����ʱ�Ľ��ն���
            TSimCapSet tSrcSim;
            memset(&tSrcSim, 0, sizeof(tSrcSim));
            GetMVBrdSrcSim(tSrcSim);


            if (MEDIA_TYPE_H264 != tSrcSim.GetVideoMediaType() &&
                MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType()
                //ȡ����������
                //FIXME: �Ը߷ֱ��ʵ����ʵ��ն˵Ĵ������δ������
                /*0 == m_tConf.GetSecBitRate() &&*/)
            {
                u8 byAdjRes = m_tConf.GetMainVideoFormat();
                u8 byAdjType = m_tConf.GetMainVideoMediaType();
                
                u8 byProximalType = 0;
                u8 byProximalRes = 0;
                BOOL32 bRet = GetProximalGrp(byAdjType, 
                                             byAdjRes,
                                             byProximalType, byProximalRes);
                if (!bRet)
                {
                    ConfLog(FALSE, "[StartSwitchToSubMtNeedAdp] no proximal grp for<%d,%d\n", byAdjType, byAdjRes);
                    continue;
                }
                if (byMVType == byProximalType && byRes == byProximalRes)
                {
                    byNum = 0;
                    memset(&abyMt, 0, sizeof(abyMt));
                    m_cMtRcvGrp.GetMVMtList(byAdjType, byAdjRes, byNum, abyMt);
                    
                    for(byMtIdx = 0; byMtIdx < byNum; byMtIdx ++)
                    {
                        if (m_tVidBrdSrc.GetMtType() == MT_TYPE_MT &&
                            m_tVidBrdSrc.GetMtId() == abyMt[byMtIdx])
                        {
                            continue;
                        }

#ifndef _SATELITE_
                        StartSwitchToSubMt(tEqp, 
                                           byChnId * MAXNUM_VOUTPUT + byOutIdx, 
                                           abyMt[byMtIdx], 
                                           byMediaMode, 
                                           bySwitchMode );

						//��Ƶ���洦��
                        StartSwitchAud2MtNeedAdp(abyMt[byMtIdx], bySwitchMode);

#else
						BOOL32 bVidSrcHd = ::topoGetMtInfo(LOCAL_MCUID, m_tVidBrdSrc.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
						if (bVidSrcHd)
						{
							g_cMpManager.StartSatConfCast(tEqp, CAST_SEC, MODE_VIDEO, byChnId*MAXNUM_VOUTPUT+byOutIdx);
						}
						else
						{
							g_cMpManager.StartSatConfCast(tEqp, CAST_FST, MODE_VIDEO, byChnId*MAXNUM_VOUTPUT+byOutIdx);
						}
						m_ptMtTable->SetMtSrc(abyMt[byMtIdx], &tEqp, MODE_VIDEO);
#endif

                    }
                }
            }
            //�Ŵ���
            g_cMpManager.SetSwitchBridge(tEqp, byChnId * MAXNUM_VOUTPUT + byOutIdx, MODE_VIDEO);
            g_cMpManager.SetSwitchBridge(tEqp, byChnId * MAXNUM_VOUTPUT + byOutIdx, MODE_AUDIO);
        }
        }
    	break;
    	
    //FIXME: ��ʱ������ѡ������Ĵ���ͬ������
    case CHN_ADPMODE_DSBRD:
        
        for(byOutIdx = 0; byOutIdx < MAXNUM_VOUTPUT; byOutIdx++)
        {
            THDAdaptParam tAdpParam = *tStatus.GetOutputVidParam(byOutIdx);
            if (tAdpParam.IsNull())
            {
                continue;
            }
            
            u8 byDSType = tAdpParam.GetVidType();
            u16 wWidth  = tAdpParam.GetWidth();
            u16 wHeight = tAdpParam.GetHeight();
            u8 byRes = GetResByWH(wWidth, wHeight);
            
            u8 byNum = 0;
            u8 abyMt[MAXNUM_CONF_MT];
            m_cMtRcvGrp.GetDSMtList(byDSType, byRes, byNum, abyMt);
            
            for(u8 byMtIdx = 0; byMtIdx < byNum; byMtIdx ++)
            {
                if (m_tDoubleStreamSrc.GetMtType() == MT_TYPE_MT &&
                    m_tDoubleStreamSrc.GetMtId() == abyMt[byMtIdx])
                {
                    continue;
                }
                TMt tDstMt = m_ptMtTable->GetMt(abyMt[byMtIdx]);
#ifdef _SATELITE_
				BOOL32 bVidSrcHd = ::topoGetMtInfo(LOCAL_MCUID, m_tDoubleStreamSrc.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
				if (bVidSrcHd)
				{
					g_cMpManager.StartSatConfCast(tEqp, CAST_SEC, MODE_SECVIDEO, byChnId*MAXNUM_VOUTPUT+byOutIdx);
				}
				else
				{
					g_cMpManager.StartSatConfCast(tEqp, CAST_FST, MODE_SECVIDEO, byChnId*MAXNUM_VOUTPUT+byOutIdx);
				}
				m_ptMtTable->SetMtSrc(abyMt[byMtIdx], &tEqp, MODE_SECVIDEO);
#else
				g_cMpManager.StartSwitchToSubMt(tEqp, 
												byChnId * MAXNUM_VOUTPUT + byOutIdx,
												tDstMt, 
												byMediaMode, 
												bySwitchMode, TRUE, TRUE, TRUE);

#endif
            }
            //�Ŵ���
            g_cMpManager.SetSwitchBridge(tEqp, byChnId * MAXNUM_VOUTPUT + byOutIdx, MODE_SECVIDEO, TRUE);
        }
        break;

    case CHN_ADPMODE_DSSEL:
        break;
    case CHN_ADPMODE_MVSEL:
        break;
    }
    
    //zbq[07/11/2009] ����������ɺ󷢹ؼ�֡����
    
    NotifyFastUpdate(tEqp, byChnId, TRUE);
    
    return TRUE;
}

/*====================================================================
    ������      ��StopSwitchToSubMtNeedAdp
    ����        ��������ֹͣ������������ �ӱ�����ͨ����ĳ�ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/27    4.5         �ű���          ����
    09/04/17    4.6         �ű���          ��д������mpu
    09/07/26    4.6         �ű���          ���Ӵ�����Ƶ�������
====================================================================*/
BOOL32 CMcuVcInst::StopSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId)
{
    u8 byMediaMode = MODE_NONE;
    u8 bySwitchMode = SWITCH_MODE_NONE;
    
    u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnId);
    
    if (!CheckHdBasChnMode(byChnMode, byMediaMode, bySwitchMode))
    {
        ConfLog(FALSE, "[StopSwitchToSubMtNeedAdp] BasChn<%d, %d> unexist!\n", 
            tEqp.GetEqpId(), byChnId);
        return FALSE;
    }

    THDBasVidChnStatus tStatus;
    memset(&tStatus, 0, sizeof(tStatus));
    
    if (!m_cBasMgr.GetChnStatus(tEqp, byChnId, tStatus))
    {
        ConfLog(FALSE, "[StopSwitchToSubMtNeedAdp] eqp<%d,%d> status failed\n", 
            tEqp.GetEqpId(), byChnId);
        return FALSE;
    }
    
    //��Ƶ���佻���İ���
    BOOL32 bRemoveAudSW = MODE_VIDEO == byMediaMode;

    u8 byOutIdx = 0;
    switch (byChnMode)
    {
    case CHN_ADPMODE_MVBRD:

        for(byOutIdx = 0; byOutIdx < MAXNUM_VOUTPUT; byOutIdx++)
        {
            THDAdaptParam tAdpParam = *tStatus.GetOutputVidParam(byOutIdx);
            
            u8 byMVType = tAdpParam.GetVidType();
            
            u16 wWidth = tAdpParam.GetWidth();
            u16 wHeight = tAdpParam.GetHeight();
            u8 byRes = GetResByWH(wWidth, wHeight);
            
            u8 byNum = 0;
            u8 abyMt[MAXNUM_CONF_MT];
            m_cMtRcvGrp.GetMVMtList(byMVType, byRes, byNum, abyMt);
            
            u8 byMtIdx = 0;
            for(; byMtIdx < byNum; byMtIdx ++)
            {
                //���������ϼ�mcu
                if (!m_tCascadeMMCU.IsNull() && m_tCascadeMMCU.GetMtId() == abyMt[byMtIdx])
                {
                    continue;
                }
                StopSwitchToSubMt(abyMt[byMtIdx], MODE_VIDEO);
                
                if(bRemoveAudSW &&
                   !IsMtNeedAdapt(ADAPT_TYPE_AUD, abyMt[byMtIdx]))
                {
                	StopSwitchToSubMt(abyMt[byMtIdx], MODE_AUDIO);
                }
            }
            
            //zbq[01/05/2009] ��ʱ��������������Դ����ʱ�Ľ��ն��� �Գ�ȡ��
            TSimCapSet tSrcSim;
            memset(&tSrcSim, 0, sizeof(tSrcSim));
            GetMVBrdSrcSim(tSrcSim);
            if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
                MEDIA_TYPE_H264 != tSrcSim.GetVideoMediaType() 
                //ȡ����������
                /*&&
                0 == m_tConf.GetSecBitRate()*/)
            {
                u8 byAdjType = m_tConf.GetMainVideoMediaType();
                u8 byAdjRes = m_tConf.GetMainVideoFormat();
                
                u8 byProximalType = 0;
                u8 byProximalRes = 0;
                BOOL32 bRet = GetProximalGrp(byAdjType, byAdjRes, byProximalType, byProximalRes);
                if (!bRet)
                {
                    ConfLog(FALSE, "[StopSwitchToSubMtNeedAdp] no proximal grp for <%d, %d\n", byAdjType, byAdjRes);
                    continue;
                }
                if (byMVType == byProximalType && byRes == byProximalRes)
                {
                    byNum = 0;
                    memset(&abyMt, 0, sizeof(abyMt));
                    m_cMtRcvGrp.GetMVMtList(byAdjType, byAdjRes, byNum, abyMt);
                    
                    for(byMtIdx = 0; byMtIdx < byNum; byMtIdx ++)
                    {
                        //���������ϼ�mcu
                        if (!m_tCascadeMMCU.IsNull() && m_tCascadeMMCU.GetMtId() == abyMt[byMtIdx])
                        {
                            continue;
                        }
                        StopSwitchToSubMt(abyMt[byMtIdx], byMediaMode);

                        if(bRemoveAudSW &&
                           !IsMtNeedAdapt(ADAPT_TYPE_AUD, abyMt[byMtIdx]))
                        {
                        	StopSwitchToSubMt(abyMt[byMtIdx], byMediaMode);
                        }
                    }
                }
            }
        }
        break;

    case CHN_ADPMODE_DSBRD:

        for(byOutIdx = 0; byOutIdx < MAXNUM_VOUTPUT; byOutIdx++)
        {
            THDAdaptParam tAdpParam = *tStatus.GetOutputVidParam(byOutIdx);
            if (tAdpParam.IsNull())
            {
                continue;
            }
            
            u8 byDSType = tAdpParam.GetVidType();
            u16 wWidth = tAdpParam.GetWidth();
            u16 wHeight = tAdpParam.GetHeight();
            u8 byRes = GetResByWH(wWidth, wHeight);
            
            u8 byNum = 0;
            u8 abyMt[MAXNUM_CONF_MT];
            m_cMtRcvGrp.GetDSMtList(byDSType, byRes, byNum, abyMt);
            
            for(u8 byMtIdx = 0; byMtIdx < byNum; byMtIdx ++)
            {
                TMt tDstMt = m_ptMtTable->GetMt(abyMt[byMtIdx]);
                g_cMpManager.StopSwitchToSubMt(tDstMt, byMediaMode);
            }
        }
        break;
    case CHN_ADPMODE_DSSEL:
        break;
    case CHN_ADPMODE_MVSEL:
        break;
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
    09/08/07    4.6         �ű���          ����
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchAud2MtNeedAdp(u8 byDstMtId, u8 bySwitchMode)
{
    if (!m_cMtRcvGrp.IsMtNeedAdp(byDstMtId))
    {
        ConfLog(FALSE, "[StartSwitchAud2MtNeedAdp] Mt.%d needn't adp, aud switch lossed!\n", byDstMtId);
        return FALSE;
    }

    if (CONF_POLLMODE_VIDEO == m_tConf.m_tStatus.GetPollMode())
    {
        EqpLog("[StartSwitchAud2MtNeedAdp] Mt.%d needn't aud switch be ignored due to VID poll!\n", byDstMtId);
        return TRUE;
    }

    //���������Ƶ���������ȷ��

    //1��h264�ֱ��ʽ����Ͽ�
    u8 byAudBindRes = 0xff;
    u8 byAudBindPT = MEDIA_TYPE_NULL;

    if (0 != m_tConf.GetSecBitRate())
    {
        byAudBindRes = m_tConf.GetMainVideoFormat();
    }
    else
    {
        if (m_tConf.GetConfAttrbEx().IsResEx720())
        {
            byAudBindRes = VIDEO_FORMAT_HD720;
        }
        else if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
        {
            byAudBindRes = VIDEO_FORMAT_4CIF;
        }
        else if (m_tConf.GetConfAttrbEx().IsResExCif())
        {
            byAudBindRes = VIDEO_FORMAT_CIF;
        }        
    }
    byAudBindPT = m_tConf.GetMainVideoMediaType();

    //2��˫��ʽ
    if (0xff == byAudBindRes ||
        MEDIA_TYPE_NULL == byAudBindPT)
    {
        byAudBindPT = m_tConf.GetSecVideoMediaType();
        byAudBindRes = m_tConf.GetSecVideoFormat();
    }

    if (0xff == byAudBindRes &&
        MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
    {
        ConfLog(FALSE, "[StartSwitchAud2MtNeedAdp] no adp chn bind for aud, impossible!\n");
        return FALSE;
    }

    //3���ҵ���Ӧ��Eqp��chnId��outIdx
    TEqp tBindBas;
    u8 byBindChnId = 0;
    u8 byBindOutIdx = 0;

    BOOL32 bRet = m_cBasMgr.GetBasResource(byAudBindPT,
                                           byAudBindRes,
                                           tBindBas,
                                           byBindChnId, byBindOutIdx);
    if (!bRet)
    {
        ConfLog(FALSE, "[StartSwitchAud2MtNeedAdp] adp chn<PT.%d, Res.%d> has no Bas resource!\n",
                        byAudBindPT, byAudBindRes);
        return FALSE;
    }

    //4��������Ӧ����Ƶ�źͽ���
    tBindBas.SetConfIdx(m_byConfIdx);

    bRet = TRUE;
    bRet &= StartSwitchToSubMt(tBindBas,
                               byBindChnId * MAXNUM_VOUTPUT,
                               byDstMtId,
                               MODE_AUDIO,
                               bySwitchMode);

    bRet &= g_cMpManager.SetSwitchBridge(tBindBas,
                                         byBindChnId * MAXNUM_VOUTPUT,
                                         MODE_AUDIO);

//�ݲ����Ǵ���ͬ��
/*
#ifdef _SATELITE_
	g_cMpManager.StartSatConfCast(tBindBas, MODE_AUDIO, byBindChnId * MAXNUM_VOUTPUT);
#endif
*/

    return bRet;
}

/*====================================================================
    ������      ��StartAllHdBasSwitch
    ����        ������hdbas�����н���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/11    4.5         �ű���          ����
    09/04/17    4.6         �ű���          ��д
====================================================================*/
BOOL32 CMcuVcInst::StartAllHdBasSwitch(BOOL32 bDual)
{
    u8 byNum = 0;

    if (!bDual)
    {
        TBasChn atBasChn[MAXNUM_CONF_MVCHN];
        m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_MVBRD);
        
        for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
        {
            if (atBasChn[byIdx].IsNull())
            {
                continue;
            }
            atBasChn[byIdx].SetConfIdx(m_byConfIdx);
            StartHdBasSwitch(atBasChn[byIdx].GetEqp(), atBasChn->GetChnId());
        }
    }
    else
    {
        TBasChn atBasChn[MAXNUM_CONF_DSCHN];
        m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_DSBRD);

        for (u8 byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
        {
            if (atBasChn[byIdx].IsNull())
            {
                continue;
            }
            atBasChn[byIdx].SetConfIdx(m_byConfIdx);
            StartHdBasSwitch(atBasChn[byIdx].GetEqp(), atBasChn->GetChnId());
        }
    }

    return TRUE;
}

/*====================================================================
������      ��StopAllHdBasSwitch
����        ��ֹͣhdbas�����н���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ����
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
08/12/11    4.5         �ű���          ����
09/04/17    4.6         �ű���          ��д
====================================================================*/
BOOL32 CMcuVcInst::StopAllHdBasSwitch(BOOL32 bDual)
{
    u8 byNum = 0;

    if (!bDual)
    {
        TBasChn atBasChn[MAXNUM_CONF_MVCHN];
        m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_MVBRD);
		        
        for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
        {
            if (atBasChn[byIdx].IsNull())
            {
                continue;
            }
            StopHdBasSwitch(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId());
        }
    }
    else
    {
        TBasChn atBasChn[MAXNUM_CONF_DSCHN];
        m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_DSBRD);

        for (u8 byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
        {
            if (atBasChn[byIdx].IsNull())
            {
                continue;
            }
            StopHdBasSwitch(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId());
        }
    }

    return FALSE;
}

/*====================================================================
    ������      ��StartHdBasSwitch
    ����        ������ĳhdbas�����н���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/11    4.5         �ű���         ����
====================================================================*/
BOOL32 CMcuVcInst::StartHdBasSwitch(const TEqp &tEqp, u8 byChnId)
{
    u8 byMediaMode = MODE_NONE;
    u8 bySwitchMode = SWITCH_MODE_NONE;
    u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnId);
    
    if (!CheckHdBasChnMode(byChnMode, byMediaMode, bySwitchMode))
    {
        ConfLog(FALSE, "[StartHdBasSwitch] BasChn<%d, %d> unexist!\n", 
                        tEqp.GetEqpId(), byChnId);
        return FALSE;
    }

    TMt tSrc;
    tSrc.SetNull();
    
    //������Ƶ���佻������ʱ��ͬʱ�������е���Ƶ�����䡱����
    BOOL32 bAudSWAdd = byMediaMode == MODE_VIDEO;
    bAudSWAdd &= CONF_POLLMODE_VIDEO != m_tConf.m_tStatus.GetPollMode();

    //bas
    u8 bySrcChn = 0;
    if (CHN_ADPMODE_MVBRD == byChnMode)
    {
        bySrcChn = m_tPlayEqp == m_tVidBrdSrc ? m_byPlayChnnl : 0;
        tSrc = m_tVidBrdSrc;
    }
    else if (CHN_ADPMODE_DSBRD == byChnMode)
    {
        bySrcChn = m_tPlayEqp == m_tDoubleStreamSrc ? m_byPlayChnnl : 0;
        tSrc = m_tDoubleStreamSrc;
    }

    if (tSrc.IsNull())
    {
        ConfLog(FALSE, "[StartHdBasSwitch] tSrc.IsNull(), failed!\n");
        return FALSE;
    }

    StartSwitchToPeriEqp(tSrc, 
                         bySrcChn,
                         tEqp.GetEqpId(),
                         byChnId,
                         byMediaMode, bySwitchMode, TRUE);
                         
    if(bAudSWAdd)
    {
        StartSwitchToPeriEqp(tSrc, 
		                     bySrcChn,
		                     tEqp.GetEqpId(),
		                     byChnId,
		                     MODE_AUDIO, bySwitchMode, TRUE);	
    }
    
    //��
    g_cMpManager.SetSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT, byMediaMode, TRUE );
    g_cMpManager.SetSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT+1, byMediaMode, TRUE );
    
    if(bAudSWAdd)
   	{
   		g_cMpManager.SetSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT, MODE_AUDIO, TRUE);
   		g_cMpManager.SetSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT+1, MODE_AUDIO, TRUE);
   	}
    
    //����¼��
    if (IsRecordSrcHDBas() &&
        !m_tConf.m_tStatus.IsNoRecording())
    {
        TEqp tLowBRBas;
        tLowBRBas.SetNull();
        u8 byLowBRBasChnId = 0;
        m_cBasMgr.GetLowBREqp(m_tConf, tLowBRBas, byLowBRBasChnId);
        if (tLowBRBas == tEqp && byLowBRBasChnId == byChnId)
        {
            StartSwitchToPeriEqp(tEqp, 
                                 byChnId*MAXNUM_VOUTPUT+1, 
                                 m_tRecEqp.GetEqpId(), 
                                 m_byRecChnnl, 
                                 byMediaMode, bySwitchMode, TRUE);
        }
    }
    
    //mt
    StartSwitchToSubMtNeedAdp(tEqp, byChnId);

    //prs
    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
		// BAS����RTCP��������
		TLogicalChannel tLogicalChannel;
		m_ptMtTable->GetMtLogicChnnl(tSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);
		u32 dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
		u16 wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();    
		SwitchVideoRtcpToDst(dwDstIp, wDstPort, tEqp, byChnId, MODE_VIDEO, SWITCH_MODE_SELECT, TRUE);
		
		// BAS�������
		for (u8 byOutChnl = 0; byOutChnl < MAXNUM_VOUTPUT; byOutChnl++)
		{
			u8 byPrsId, byPrsChnlId = 0;
			if (m_tConfPrsInfo.FindPrsForSrc(tEqp, byChnId * MAXNUM_VOUTPUT + byOutChnl,
				                             byPrsId, byPrsChnlId))
			{
				BuildRtcpAndRtpSwithForPrs(byPrsId, byPrsChnlId);
			}
			else
			{
				TPrsChannel tIdlePrsChnl;
				if (g_cMcuVcApp.GetIdlePrsChls(1, tIdlePrsChnl))
				{
					if (ChangePrsSrc(tIdlePrsChnl.GetPrsId(), tIdlePrsChnl.m_abyPrsChannels[0],
									 tEqp, byChnId * MAXNUM_VOUTPUT + byOutChnl) &&
						m_tConfPrsInfo.AddPrsChnl(tIdlePrsChnl.GetPrsId(), tIdlePrsChnl.m_abyPrsChannels[0],
												  byMediaMode, tEqp, byChnId * MAXNUM_VOUTPUT + byOutChnl))
					{
						OccupyPrsChnl(tIdlePrsChnl.GetPrsId(), tIdlePrsChnl.m_abyPrsChannels[0]);
						// ��PRS����Ӧ���ͷŶ�Ӧ��PRSͨ��
					}
				}
				else
				{
					EqpLog("[StartHdBasSwitch]warning Not enough idle prs channel for the channel%d of eqp%d\n",
							byChnId * MAXNUM_VOUTPUT + byOutChnl, tEqp.GetEqpId());
				}
			}
		}

    }

    //�����鲥
    
    return TRUE;
}

/*====================================================================
    ������      ��StopHdBasSwitch
    ����        ��ֹͣĳhdbas�����н���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/27    4.5         �ű���         ����
====================================================================*/
BOOL32 CMcuVcInst::StopHdBasSwitch(const TEqp &tEqp, u8 byChnId)
{
    u8 byMediaMode = MODE_NONE;
    u8 bySwitchMode = SWITCH_MODE_NONE;
    u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnId);

    if (!CheckHdBasChnMode(byChnMode, byMediaMode, bySwitchMode))
    {
        ConfLog(FALSE, "[StopHdBasSwitch] BasChn<%d, %d> unexist!\n", tEqp.GetEqpId(), byChnId);
        return FALSE;
    }

    //FIXME: ������©������ʱ��ΪRemoveVIDһ������RemoveAud�������ѡ������ʱ����������Ҫ��������
    BOOL32 bStopAud = MODE_VIDEO == bySwitchMode;
    
    //zbq[08/29/2009]�����ڴ������˵Ĵ���Ƶ��ѯ�¹�Ȼ�����⣺��ѯ��������Դʱ����˷����˵Ľ���
    //������˲���Ľ�������ѯ����ʱǿ�Ʋ���һ��
    bStopAud &= CONF_POLLMODE_VIDEO != m_tConf.m_tStatus.GetPollMode();

    //Eqp
    StopSwitchToPeriEqp(tEqp.GetEqpId(), byChnId, TRUE, byMediaMode);
    if(bStopAud)
    {
    	StopSwitchToPeriEqp(tEqp.GetEqpId(), byChnId, TRUE, MODE_AUDIO);
    }
    
    //��
    g_cMpManager.RemoveSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT, byMediaMode, TRUE );
    g_cMpManager.RemoveSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT+1, byMediaMode, TRUE );
    if(bStopAud)
    {
    	g_cMpManager.RemoveSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT, MODE_AUDIO, TRUE );
    	g_cMpManager.RemoveSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT+1, MODE_AUDIO, TRUE );    	
    }

    //mt
    StopSwitchToSubMtNeedAdp(tEqp, byChnId);

    //�㲥����ģʽ��ͣ����¼��
    if (CHN_ADPMODE_MVBRD == byChnMode)
    {
        //����¼��
        if (IsRecordSrcHDBas() &&
            !m_tConf.m_tStatus.IsNoRecording())
        {
            StopSwitchToPeriEqp(m_tRecEqp.GetEqpId(), m_byRecChnnl, FALSE, byMediaMode);
        }
    }

    //prs
    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
		u8 byPrsId, byPrsChnlId = 0;
		for(u8 byOutChnl = 0; byOutChnl < MAXNUM_VOUTPUT; byOutChnl++)
		{
			if (m_tConfPrsInfo.FindPrsForSrc(tEqp, byChnId*MAXNUM_VOUTPUT + byOutChnl,
				                             byPrsId, byPrsChnlId))
			{
				StopPrs(byPrsId, byPrsChnlId);
				RlsPrsChnl(byPrsId, byPrsChnlId);				
			}
		}
	}


    return TRUE;
}

/*====================================================================
    ������      ��StartSwitchToSubMtFromAdp
    ����        �������ݽ����������� �ӱ�����ͨ����ĳ�ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/27    4.5         �ű���         ����
    09/07/26	  4.6		�ű���         ����֧����Ƶ������֧��
	09/11/23	4.6			�ű���		   ����֧�����ǻ��������㲥
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToSubMtFromAdp(u8 byMtId, BOOL32 bDual)
{
    if ((!m_cMtRcvGrp.IsMtNeedAdp(byMtId) && !bDual) ||
        (!m_cMtRcvGrp.IsMtNeedAdp(byMtId, FALSE) && bDual))
    {
        ConfLog(FALSE, "[StartSwitchToSubMtFromAdp] Mt.%d needn't adp, bDual.%d\n", byMtId, bDual);
        return FALSE;
    }

    u8 byMediaType = MEDIA_TYPE_NULL;
    u8 byRes = 0;

    TEqp tHDBas;
    u8 byChnId = 0;
    u8 byOutIdx = 0;
    
    BOOL32 bRet = FALSE;
    BOOL32 bRet1 = FALSE;

    if (!bDual)
    {
        //���������Լ�����ʹ��Ҫ����
        if (m_tVidBrdSrc.GetMtType() == MT_TYPE_MT &&
            m_tVidBrdSrc.GetMtId() == byMtId)
        {
            return TRUE;
        }
        bRet = m_cMtRcvGrp.GetMtMediaRes(byMtId, byMediaType, byRes);
        bRet1 = m_cBasMgr.GetBasResource(byMediaType, byRes, tHDBas, byChnId, byOutIdx);
        tHDBas.SetConfIdx(m_byConfIdx);

        if ( bRet && bRet1 )
        {
        	//��Ƶ
			g_cMpManager.SetSwitchBridge(tHDBas, byChnId * MAXNUM_VOUTPUT + byOutIdx, MODE_VIDEO);
            NotifyEqpFastUpdate(tHDBas, byChnId, TRUE);


#ifndef _SATELITE_
            StartSwitchToSubMt(tHDBas, byChnId * MAXNUM_VOUTPUT + byOutIdx, byMtId, MODE_VIDEO);

			//��Ƶ���洦��
            StartSwitchAud2MtNeedAdp(byMtId);
#else
			//������Դ���㲥����򵽵�һ�鲥�㣻ѡ�����⴦��

			BOOL32 bVidSrcHd = ::topoGetMtInfo(LOCAL_MCUID, m_tVidBrdSrc.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();

			if (bVidSrcHd)
			{
				g_cMpManager.StartSatConfCast(tHDBas, CAST_SEC, MODE_VIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
			}
			else
			{
				g_cMpManager.StartSatConfCast(tHDBas, CAST_FST, MODE_VIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
			}
			m_ptMtTable->SetMtSrc(byMtId, &tHDBas, MODE_VIDEO);
#endif
        }
        else
        {
            //zbq[01/05/2009] ��ʱ��������������Դ����ʱ�Ľ��ն���
            TSimCapSet tSrcSim;
            memset(&tSrcSim, 0, sizeof(tSrcSim));
            GetMVBrdSrcSim(tSrcSim);
            if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
                MEDIA_TYPE_H264 != tSrcSim.GetVideoMediaType() 
                //ȡ����������
                //FIXME: �Ը߷ֱ��ʵ����ʵ��ն˵Ĵ������δ������
                /*&&
                0 == m_tConf.GetSecBitRate()*/)
            {
                u8 byAdjType = m_tConf.GetMainVideoMediaType();
                u8 byAdjRes = m_tConf.GetMainVideoFormat();
                
                //���ն��Ƿ���Ҫ�߽�������
                u8 byProximalType = 0;
                u8 byProximalRes = 0;
                BOOL32 bRet = GetProximalGrp(byAdjType,
                                             byAdjRes, 
                                             byProximalType, byProximalRes);
                if (!bRet)
                {
                    ConfLog(FALSE, "[StartSwitchToSubMtFromAdp] no proximal grp for <%d, %d\n", byAdjType, byAdjRes);
                    return FALSE;
                }
                //ȡ���������bas��Դ
                bRet1 = m_cBasMgr.GetBasResource(byProximalType, 
                                                 byProximalRes, 
                                                 tHDBas, byChnId, byOutIdx);
                tHDBas.SetConfIdx(m_byConfIdx);
                if (!bRet1)
                {
                    ConfLog(FALSE, "[StartSwitchToSubMtFromAdp] get proximal bas src failed!\n");
                    return FALSE;
                }
                
                //��Ƶ
				g_cMpManager.SetSwitchBridge(tHDBas, byChnId * MAXNUM_VOUTPUT + byOutIdx, MODE_VIDEO); 
                NotifyEqpFastUpdate(tHDBas, byChnId, TRUE);

#ifndef _SATELITE_
                StartSwitchToSubMt(tHDBas, byChnId * MAXNUM_VOUTPUT + byOutIdx, byMtId, MODE_VIDEO);

				//��Ƶ���洦��
                StartSwitchAud2MtNeedAdp(byMtId);

#else
				//������Դ���㲥����򵽵�һ�鲥�㣻ѡ�����⴦��
				
				BOOL32 bVidSrcHd = ::topoGetMtInfo(LOCAL_MCUID, m_tVidBrdSrc.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
				if (bVidSrcHd)
				{
					g_cMpManager.StartSatConfCast(tHDBas, CAST_SEC, MODE_VIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
				}
				else
				{
					g_cMpManager.StartSatConfCast(tHDBas, CAST_FST, MODE_VIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
				}
				m_ptMtTable->SetMtSrc(byMtId, &tHDBas, MODE_VIDEO);
#endif

            }
            else
            {
                ConfLog(FALSE, "[StartSwitchToSubMtFromAdp] failed, ret<%d, %d>\n", bRet, bRet1);
            }
        }
    }
    else
    {
        //���������Լ�����ʹ��Ҫ����
        if (m_tDoubleStreamSrc.GetMtType() == MT_TYPE_MT &&
            m_tDoubleStreamSrc.GetMtId() == byMtId)
        {
            return TRUE;
        }
        bRet = m_cMtRcvGrp.GetMtMediaRes(byMtId, byMediaType, byRes, TRUE);
        bRet1 = m_cBasMgr.GetBasResource(byMediaType,
                                         byRes,
                                         tHDBas,
                                         byChnId,
                                         byOutIdx, 
                                         TRUE, 
                                         IsDSSrcH263p() /*|| IsConfDualEqMV(m_tConf)*/);

        if ( bRet && bRet1 )
        {
            tHDBas.SetConfIdx(m_byConfIdx);


			g_cMpManager.SetSwitchBridge(tHDBas, byChnId * MAXNUM_VOUTPUT + byOutIdx, MODE_SECVIDEO, TRUE); 

#ifndef _SATELITE_
            g_cMpManager.StartSwitchToSubMt(tHDBas,
                                            byChnId * MAXNUM_VOUTPUT + byOutIdx,
                                            m_ptMtTable->GetMt(byMtId),
                                            MODE_SECVIDEO,
                                            SWITCH_MODE_BROADCAST, TRUE, TRUE, TRUE);
            

#else
			BOOL32 bVidSrcHd = ::topoGetMtInfo(LOCAL_MCUID, m_tDoubleStreamSrc.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
			
			if (bVidSrcHd)
			{
				g_cMpManager.StartSatConfCast(tHDBas, CAST_SEC, MODE_SECVIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
			}
			else
			{
				g_cMpManager.StartSatConfCast(tHDBas, CAST_FST, MODE_SECVIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
			}
			m_ptMtTable->SetMtSrc(byMtId, &tHDBas, MODE_SECVIDEO);
#endif

        }
        else
        {
            //zbq[01/17/2009] h263p����������Դ����ʱ ��������
            TDStreamCap tDSCap;
            memset(&tDSCap, 0, sizeof(tDSCap));
            GetDSBrdSrcSim(tDSCap);
            if (MEDIA_TYPE_H263PLUS == tDSCap.GetMediaType())
            {
                //��������̶����
                u8 byProximalType = MEDIA_TYPE_H264;
                u8 byProximalRes = VIDEO_FORMAT_XGA;

                //ȡ���������bas��Դ
                bRet1 = m_cBasMgr.GetBasResource(byProximalType,
                                                 byProximalRes,
                                                 tHDBas,
                                                 byChnId,
                                                 byOutIdx, TRUE, TRUE);
                tHDBas.SetConfIdx(m_byConfIdx);
                if (!bRet1)
                {
                    ConfLog(FALSE, "[StartSwitchToSubMtFromAdp] get proximal bas src failed(dual)!\n");
                    return FALSE;
                }
				
				g_cMpManager.SetSwitchBridge(tHDBas, byChnId * MAXNUM_VOUTPUT + byOutIdx, MODE_SECVIDEO, TRUE); 

#ifndef _SATELITE_
                g_cMpManager.StartSwitchToSubMt(tHDBas,
                                                byChnId * MAXNUM_VOUTPUT + byOutIdx,
                                                m_ptMtTable->GetMt(byMtId),
                                                MODE_SECVIDEO,
                                                SWITCH_MODE_BROADCAST, TRUE, TRUE, TRUE);
                

#else
				BOOL32 bVidSrcHd = ::topoGetMtInfo(LOCAL_MCUID, m_tDoubleStreamSrc.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
				if (bVidSrcHd)
				{
					g_cMpManager.StartSatConfCast(tHDBas, CAST_SEC, MODE_SECVIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
				}
				else
				{
					g_cMpManager.StartSatConfCast(tHDBas, CAST_FST, MODE_SECVIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
				}
				m_ptMtTable->SetMtSrc(byMtId, &tHDBas, MODE_SECVIDEO);
#endif

            }
            else
            {
                ConfLog(FALSE, "[StartSwitchToSubMtFromAdp] failed, ret<%d, %d(dual)>\n", bRet, bRet1);
            }
        }
    }

    return bRet & bRet1;
}

/*====================================================================
    ������      ��CheckHdBasChnMode
    ����        ��HD-BAS����ģʽУ�� ��ȡ����ģʽ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/04/17    4.6         �ű���         ����
====================================================================*/
BOOL32 CMcuVcInst::CheckHdBasChnMode(u8 byMode, u8 &byMediaMode, u8 &bySwitchMode)
{
    byMediaMode = MODE_NONE;
    bySwitchMode = SWITCH_MODE_NONE;
    
    switch (byMode)
    {
    case CHN_ADPMODE_MVBRD:
        byMediaMode = MODE_VIDEO;
        bySwitchMode = SWITCH_MODE_BROADCAST;
        break;
        
    case CHN_ADPMODE_DSBRD:
        byMediaMode = MODE_SECVIDEO;
        bySwitchMode = SWITCH_MODE_BROADCAST;
        break;
        
    case CHN_ADPMODE_MVSEL:
        byMediaMode = MODE_VIDEO;
        bySwitchMode = SWITCH_MODE_SELECT;
        break;
        
    case CHN_ADPMODE_DSSEL:
        byMediaMode = MODE_SECVIDEO;
        bySwitchMode = SWITCH_MODE_SELECT;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

/*====================================================================
    ������      ��StopSwitchToSubMtFromAdp
    ����        ��������ֹͣ������������ �ӱ�����ͨ����ĳ�ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/27    4.5         �ű���         ����
    09/07/26    4.6         �ű���         ������Ƶ�����İ󶨴���
====================================================================*/
BOOL32 CMcuVcInst::StopSwitchToSubMtFromAdp(u8 byMtId, BOOL32 bDual)
{
    if ((!m_cMtRcvGrp.IsMtNeedAdp(byMtId) && !bDual) ||
        (!m_cMtRcvGrp.IsMtNeedAdp(byMtId, FALSE) && bDual))
    {
        ConfLog(FALSE, "[StopSwitchToSubMtFromAdp] Mt.%d needn't adp, bDual.%d\n", byMtId, bDual);
        return FALSE;
    }

    if (!bDual)
    {
        StopSwitchToSubMt(byMtId, MODE_VIDEO);
        StopSwitchToSubMt(byMtId, MODE_AUDIO);
    }
    else
    {
        StopSwitchToSubMt(byMtId, MODE_SECVIDEO);
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

    if ( MODE_AUDIO == byMode || MODE_BOTH == byMode )
    {
        for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
        {
            if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )    
            {
                StopSwitchToSubMt( byLoop, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE );
            }
        }
    }
    
    if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
    {
        // ɾ���㲥Դ
        u8 bySrcChnl = (m_tVidBrdSrc==m_tPlayEqp) ? m_byPlayChnnl : 0;
        g_cMpManager.StopSwitchToBrd( m_tVidBrdSrc, bySrcChnl );
        
        // ɾ���㲥����
        for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
        {
            if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )           
            {
                StopSwitchToSubMt( byLoop, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE );
            }
        }
    }

	MtStatusChange();
    
    return;
}

/*====================================================================
    ������      ��StopSwitchToAllSubMtJoinedConfNeedAdapt
    ����        ��ֹͣ�����ݽ����������������������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bMsg, �Ƿ�֪ͨ������Ϣ��ȱʡΪTRUE
				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/08    1.0         LI Yi         ����
====================================================================*/
//modify bas 2
void CMcuVcInst::StopSwitchToAllSubMtJoinedConfNeedAdapt(BOOL32 bMsg, u8 byMode, u8 byAdaptType)
{
    u8	byLoop;

    TMt tSrcMt;
    tSrcMt.SetNull();
    TMt tDstMt;
    for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        //MT joined conference and need adapt, stop switch to it
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop))
        {
            tDstMt = m_ptMtTable->GetMt(byLoop);
            if (IsMtNeedAdapt(byAdaptType, byLoop, &tSrcMt))
            {
                TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
                tConfAttrb.SetUseAdapter(FALSE);
                m_tConf.SetConfAttrb(tConfAttrb);
                //RestoreRcvMediaBrdSrc(byLoop, byMode, FALSE);
                StopSwitchToSubMt( byLoop, byMode, SWITCH_MODE_BROADCAST, FALSE );
                tConfAttrb.SetUseAdapter(TRUE);
                m_tConf.SetConfAttrb(tConfAttrb);
            }			
        }
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
   ����ֵ˵��  ��TRUE�����������ֱ�ӷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/20    1.0         LI Yi         ����
	03/07/24    2.0         ������        �޸�
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToPeriEqp(TMt tSrc,
                                        u8	bySrcChnnl,
                                        u8	byEqpId,
                                        u16 wDstChnnl,
                                        u8	byMode,
                                        u8	bySwitchMode,
                                        BOOL32 bDstHDBas,
                                        BOOL32 bStopBeforeStart,
                                        BOOL32 bBatchPollChg) 
{
	CServMsg	cServMsg;
	TPeriEqpStatus	tStatus;

	tSrc = GetLocalMtFromOtherMcuMt( tSrc );

	//δ����
	if( !g_cMcuVcApp.IsPeriEqpConnected( byEqpId ) )
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
			return TRUE;
		}
		else if ( byMode == MODE_BOTH )
		{
			byMode = MODE_VIDEO;
		}
	}

    if(!g_cMpManager.StartSwitchToPeriEqp(tSrc, bySrcChnnl, byEqpId, wDstChnnl, 
										  byMode, bySwitchMode, bDstHDBas, bStopBeforeStart))
	{
		ConfLog( FALSE, "StartSwitchToPeriEqp() failed! Cannot switch to specified eqp!\n" );
		return FALSE;
	}

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

	//֪ͨ�ն˿�ʼ����
    if (tSrc.GetType() == TYPE_MT)
    {
        //zbq[04/15/2009] Tand�Ĺؼ�֡��ıȽϿ죬�Ҽ��Ϊ3s. �ʽ����������ٷ�youareseeing�������õ���һ���ؼ�֡
        if (IsDelayVidBrdVCU() && tSrc == m_tVidBrdSrc)
        {
        }
        else
        {
            NotifyMtSend( tSrc.GetMtId(), byMode, TRUE );
			
			TEqp tEqp = g_cMcuVcApp.GetEqp(byEqpId);
			if ( byEqpId >= HDUID_MIN && byEqpId <= HDUID_MAX)
			{
				// zbq [06/25/2007] ����Ƶ����������ؼ�֡
				if( MODE_BOTH == byMode || MODE_VIDEO == byMode )
				{            
					NotifyFastUpdate(tSrc, MODE_VIDEO, FALSE);
				}
			}
			else
			{
				// zbq [06/25/2007] ����Ƶ����������ؼ�֡
				if( MODE_BOTH == byMode || MODE_VIDEO == byMode )
				{            
					NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
				}
			}
        }
    }

	g_cMcuVcApp.SetPeriEqpSrc( byEqpId, (const TMt*)&tSrc, (u8)wDstChnnl, byMode );

	//send message according to its type
	if( !g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tStatus ) )
	{
		return FALSE;
	}

    TTvWallStartPlay tTwPlayPara;
    THduStartPlay  tHduPlayPara;
	if (MODE_AUDIO  == byMode)
    {
        byMode = HDU_OUTPUTMODE_AUDIO;
    }
    else if (MODE_VIDEO == byMode)
    {
        byMode = HDU_OUTPUTMODE_VIDEO;
    }
    else if (MODE_BOTH  == byMode)
    {
        byMode = HDU_OUTPUTMODE_BOTH;
    }
    else
    {
        byMode = HDU_OUTPUTMODE_BOTH;
    }

	tHduPlayPara.SetMode( byMode );

    TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
    if (EQP_TYPE_HDU == tStatus.GetEqpType())
    {
        tHduPlayPara.SetMt(tSrc);
		tHduPlayPara.SetVideoEncrypt(tEncrypt);
    }
	else
	{
		tTwPlayPara.SetMt(tSrc);
	    tTwPlayPara.SetVideoEncrypt(tEncrypt);
	}

    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
        u32 dwMtSwitchIp;
        u16 wMtSwitchPort;
        u32 dwMtSrcIp;

        g_cMpManager.GetSwitchInfo(tSrc, dwMtSwitchIp, wMtSwitchPort, dwMtSrcIp);
		if (EQP_TYPE_HDU == tStatus.GetEqpType())
		{
            tHduPlayPara.SetRtcpBackAddr(dwMtSwitchIp, 0);
            tHduPlayPara.SetIsNeedByPrs(TRUE);
		}
		else
		{
            tTwPlayPara.SetRtcpBackAddr(dwMtSwitchIp, 0);
            tTwPlayPara.SetIsNeedByPrs(TRUE);
		}

    }
    else
    {
		if (EQP_TYPE_HDU == tStatus.GetEqpType())
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
		EqpLog("[StartSwitchToPeriEqp] tDVPayload.GetRealPayLoad() = %u\n", tDVPayload.GetRealPayLoad());
		EqpLog("[StartSwitchToPeriEqp] tDVPayload.GetActivePayload() = %u\n", tDVPayload.GetActivePayload());
    }
    else
    {
        tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType());
        tDVPayload.SetActivePayload(tSrcSCS.GetVideoMediaType());
		EqpLog("[StartSwitchToPeriEqp] tDVPayload.GetRealPayLoad() = %u\n", tDVPayload.GetRealPayLoad());
		EqpLog("[StartSwitchToPeriEqp] tDVPayload.GetActivePayload() = %u\n", tDVPayload.GetActivePayload());
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



	switch(tStatus.GetEqpType())
	{
	case EQP_TYPE_TVWALL:
		{
		    cServMsg.SetMsgBody((u8 *)&tTwPlayPara, sizeof(tTwPlayPara));
		    cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload));
		    cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload));
  	        cServMsg.SetChnIndex((u8)wDstChnnl);
			// MCUǰ�����zgc��2007-09-27
			TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
			cServMsg.CatMsgBody((u8*)&tCapSupportEx, sizeof(tCapSupportEx));

			SendMsgToEqp(byEqpId, MCU_TVWALL_START_PLAY_REQ, cServMsg);
			break;
		}
	case EQP_TYPE_HDU:
		cServMsg.SetMsgBody((u8 *)&tHduPlayPara, sizeof(tHduPlayPara));
		cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload));
		cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload));
		cServMsg.SetChnIndex((u8)wDstChnnl);
        
        if (!bBatchPollChg)
        {
            SendMsgToEqp(byEqpId, MCU_HDU_START_PLAY_REQ, cServMsg);
        }

		// xliang [7/15/2009] ��VMP����ؼ�֡
		if(EQP_TYPE_VMP == tSrc.GetEqpType())
		{
			NotifyFastUpdate(m_tVmpEqp, bySrcChnnl);
		}
        break;
	}

	return TRUE;
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
void CMcuVcInst::StopSwitchToPeriEqp( u8 byEqpId,  u16 wDstChnnl, BOOL32 bMsg, u8 byMode, u8 bySwitchMode )
{
	CServMsg	cServMsg;
	TEqp	tEqp;
	TPeriEqpStatus	tStatus;
	TMt    TMtNull;
	
    // guzh [5/12/2007] ����Ƿ���ͣ��������
    if (g_cMcuVcApp.GetEqpType( byEqpId ) == EQP_TYPE_MIXER &&
        m_tMixEqp.GetEqpId() == byEqpId)
    {
        // zbq [03/19/2007] ������һ�λ���ͨ��λ��
        for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
        {
            if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) )
            {
                if ( 0 != GetMixChnPos(byMtId) &&
                     GetMixChnPos(byMtId) == wDstChnnl )
                {
                    GetMixChnPos( byMtId, TRUE );
                    MpManagerLog( "[StopSwitchToPeriEqp] Mixer Chn pos.%d for MT.%d has been cleared !\n", wDstChnnl, byMtId );
                    break;
                }
            }
        }
    }

	//stop switch
	g_cMpManager.StopSwitchToPeriEqp( m_byConfIdx, byEqpId, wDstChnnl, byMode );
	
	TMtNull.SetNull();
	if( g_cMcuVcApp.GetEqpType( byEqpId ) != EQP_TYPE_MIXER )
	{
		g_cMcuVcApp.SetPeriEqpSrc( byEqpId, &TMtNull, (u8)wDstChnnl, byMode );
	}

	//send stop message
	if( bMsg )
	{
		g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tStatus );
		cServMsg.SetChnIndex( (u8)wDstChnnl );
		cServMsg.SetConfId( m_tConf.GetConfId() );
		tEqp.SetMcuEqp( (u8)LOCAL_MCUID, byEqpId, tStatus.GetEqpType() );
		cServMsg.SetMsgBody( ( u8 * )&tEqp, sizeof( tEqp ) );
		switch( tStatus.GetEqpType() )
		{
		case EQP_TYPE_TVWALL:
			SendMsgToEqp( byEqpId, MCU_TVWALL_STOP_PLAY_REQ, cServMsg );
			break;

		case EQP_TYPE_HDU:    //4.6 �¼�  jlb
			SendMsgToEqp( byEqpId, MCU_HDU_STOP_PLAY_REQ, cServMsg );
			break;

		case EQP_TYPE_RECORDER:
			SendMsgToEqp( byEqpId, MCU_REC_STOPREC_REQ, cServMsg );
			break;
        case EQP_TYPE_BAS:
            SendMsgToEqp( byEqpId, MCU_BAS_STOPADAPT_REQ, cServMsg );
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

	tSrc = GetLocalMtFromOtherMcuMt( tSrc );

	for( byEqpId = 1; byEqpId < MAXNUM_MCU_PERIEQP; byEqpId++)
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
void CMcuVcInst::StopSwitchToAllPeriEqpWatchingSrcMcu( const u16 wMcuId, BOOL32 bMsg , u8 byMode )
{
	TMt tSrcMt;
	u8      byEqpId;
	u8      byChnnlNo;

	for( byEqpId = 1; byEqpId < MAXNUM_MCU_PERIEQP; byEqpId++)
	{
		if( !g_cMcuVcApp.IsPeriEqpConnected( byEqpId ) )
			continue;

		//Ŀǰֻ�Ե���ǽ����
		if( g_cMcuAgent.GetPeriEqpType( byEqpId ) != EQP_TYPE_TVWALL )
			continue;

		for( byChnnlNo = 0; byChnnlNo < MAXNUM_PERIEQP_CHNNL; byChnnlNo++ )
		{
			if( g_cMcuVcApp.GetPeriEqpSrc( byEqpId, &tSrcMt, byChnnlNo, MODE_VIDEO )
				&& tSrcMt.GetMcuId() == wMcuId )
				StopSwitchToPeriEqp( byEqpId, byChnnlNo, TRUE, MODE_VIDEO );

			if( g_cMcuVcApp.GetPeriEqpSrc( byEqpId, &tSrcMt, byChnnlNo, MODE_AUDIO )
				&& tSrcMt.GetMcuId() == wMcuId )
				StopSwitchToPeriEqp( byEqpId, byChnnlNo, TRUE, MODE_AUDIO );
		}
	}
}

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
								u8 byDstChnnl, u8 byMode ) 
{
	CServMsg	cServMsg;

	tSrc = GetLocalMtFromOtherMcuMt( tSrc );
	

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
			return TRUE;
		}
		else if ( byMode == MODE_BOTH )
		{
			byMode = MODE_VIDEO;
		}
	}

    if(!g_cMpManager.StartSwitchToMc(tSrc,bySrcChnnl,wMcInstId,byDstChnnl,byMode))
	{
		ConfLog( FALSE, "StartSwitchToMc() failed! Cannot switch to specified eqp!\n" );
		return FALSE;
	}

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

	//֪ͨ�ն˿�ʼ����
	if( tSrc.GetType() == TYPE_MT )
	{
		NotifyMtSend( tSrc.GetMtId(), byMode, TRUE );

        // zbq [06/25/2007] ����Ƶ����������ؼ�֡
        if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
        {
            NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
        }
	}

	g_cMcuVcApp.SetMcSrc( wMcInstId, ( const TMt* )&tSrc, byDstChnnl, byMode );


	//send message
	TSwitchInfo tSwitchInfo;
	tSwitchInfo.SetSrcMt( tSrc );
	tSwitchInfo.SetSrcChlIdx( bySrcChnnl );
	tSwitchInfo.SetDstChlIdx( byDstChnnl );
	tSwitchInfo.SetMode( byMode );
	
	//�����Լ���̬�غ���Ϣ
	TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
	TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS( tSrc.GetMtId() ); 
	TDoublePayload tDVPayload;
	TDoublePayload tDAPayload;

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
    
    // zw [06/26/2008] ��ӦAAC LC��ʽ
    if ( MEDIA_TYPE_AACLC == tSrcSCS.GetAudioMediaType() )
    {
        TAudAACCap tAudAACCap;
        tAudAACCap.SetMediaType(MEDIA_TYPE_AACLC);
        tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
        tAudAACCap.SetChnlType(AAC_CHNL_TYPE_SINGLE);
        tAudAACCap.SetBitrate(96);
        tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
    
	    cServMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
    }
	
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

	if( bFilterConn && !g_cMcuVcApp.IsMcConnected( wMcInstId ) ) return;
	
	//stop switch
    g_cMpManager.StopSwitchToMc( m_byConfIdx, wMcInstId, byDstChnnl, byMode );

	tMtNull.SetNull();
	g_cMcuVcApp.SetMcSrc( wMcInstId, &tMtNull, byDstChnnl, byMode );

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
	u8  byLoop;
	TMt tMt;

	for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
		{
			tMt = m_ptMtTable->GetMt( byLoop );
			StopSwitchToAllMcWatchingSrcMt( tMt, bMsg, byMode );	
		}
	}
}

/*====================================================================
    ������      ��StopSwitchToAllMcWatchingSrcMt
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
    03/06/28    1.0         JQL           ����
====================================================================*/
void CMcuVcInst::StopSwitchToAllMcWatchingSrcMt(TMt tSrc, BOOL32 bMsg, u8 byMode )
{
	TMt tCurSrc;
	u8      byIndex;
	u8      byChannel;
	u8      byChannelNum;
	TLogicalChannel tLogicalChannel;

	tSrc = GetLocalMtFromOtherMcuMt( tSrc );
	
	for( byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( !g_cMcuVcApp.IsMcConnected( byIndex ) )
			continue;
        
		g_cMcuVcApp.GetMcLogicChnnl( byIndex, MODE_VIDEO, &byChannelNum, &tLogicalChannel );
		
		for( byChannel = 0; byChannel < byChannelNum; byChannel++ )
		{
			if( ( byMode == MODE_VIDEO || byMode == MODE_BOTH ) 
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_VIDEO )
				&& tCurSrc == tSrc )
				StopSwitchToMc( byIndex, byChannel, bMsg, MODE_VIDEO );

			if( ( byMode == MODE_AUDIO || byMode == MODE_BOTH ) 
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_AUDIO )
				&& tCurSrc == tSrc )
				StopSwitchToMc( byIndex, byChannel, bMsg, MODE_AUDIO );
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
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/01/04    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::SwitchSrcToDst( const TSwitchInfo &tSwitchInfo,
                                 const CServMsg & cServMsgHdr,
                                 BOOL32 bBatchPollChg )
{
	CServMsg	cServMsg;
	TMt	tDstMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetDstMt());
	TMt tSrcMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetSrcMt());
	u8	byMode = tSwitchInfo.GetMode();
	u8	bySrcChnnl = tSwitchInfo.GetSrcChlIdx();// xliang [1/7/2009] Ŀǰ����ѡ��VMPʱ����

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
#ifndef _SATELITE_
        g_cMpManager.SetSwitchBridge(tSrcMt, 0, byMode);
		StartSwitchToSubMt(tSrcMt, 0, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE, FALSE);
#else
		g_cMpManager.StartSwitchSrc2Dst(tSrcMt, tDstMt, byMode);


		//FIXME: �Ժ��������� ˢ�´���

		// xsl [5/22/2006]�����ն�״̬, ���ﱣ��㲥Դ��Ϣ��������bas��Ϣ����Ϊ�����ط������Դ��Ϣ����Ӧ����
		m_ptMtTable->SetMtSrc( tDstMt.GetMtId(), ( const TMt * )&tSrcMt, byMode );
		
		//����Ŀ���ն�Ϊ�ϼ�mcu�����
		TMtStatus tSrcMtStatus;
		TMtStatus tMtStatus;
		u8 byAddSelByMcsMode = MODE_NONE;

		m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
		if( tSrcMt.GetType() == TYPE_MT && tSrcMt.GetMtId() != tDstMt.GetMtId() && 
			(m_tCascadeMMCU.GetMtId() == 0 || tDstMt.GetMtId() != m_tCascadeMMCU.GetMtId()) )
		{
			TMtStatus tSrcMtStatus;
			u8 byAddSelByMcsMode = MODE_NONE;
			m_ptMtTable->GetMtStatus( tSrcMt.GetMtId(), &tSrcMtStatus );
			
			//zbq[12/06/2007] VMP����ʱ������
			if( ((!(tSrcMt == m_tVidBrdSrc)) || (tSrcMt == m_tVidBrdSrc && 
												 ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() &&
												 m_tRollCaller == m_tVidBrdSrc)) &&
												 tSrcMtStatus.IsSendVideo() && 
				( MODE_VIDEO == byMode || MODE_BOTH == byMode ) )
			{
				byAddSelByMcsMode = MODE_VIDEO;
			}
			if( !(tSrcMt == m_tAudBrdSrc) && tSrcMtStatus.IsSendAudio() && 
				( MODE_AUDIO == byMode || MODE_BOTH == byMode ) )
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
				tMtStatus.AddSelByMcsMode( byMode );
			}
			else
			{
				tMtStatus.RemoveSelByMcsMode( byMode );
			}
		}
		else
		{
			tMtStatus.RemoveSelByMcsMode( byMode );
		}
		m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tMtStatus );
		
		MtStatusChange( tDstMt.GetMtId(), TRUE );
		
		//����ѡ������Ŀ���ն˵���Ҫ���ڷ����ն˵�����
		if( tSrcMt.GetType() == TYPE_MT && 
			( byMode == MODE_VIDEO || byMode == MODE_BOTH ) && 
			  tSrcMt.GetMtId() != m_tVidBrdSrc.GetMtId() )
		{
			u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( tDstMt.GetMtId() );
			AdjustMtVideoSrcBR( tDstMt.GetMtId(), wMtBitRate );
		}

		RefreshConfState();
#endif
		}
		break;
	case MCS_MCU_STARTSWITCHVMPMT_REQ:// xliang [1/7/2009] ��ϯѡ��VMP 
	case MT_MCU_STARTSWITCHVMPMT_REQ:	// xliang [4/9/2009] MT ������ѡ��
		g_cMpManager.SetSwitchBridge(tSrcMt, bySrcChnnl, byMode); // xliang [4/2/2009] ���߹㲥�߼���û��Ҫ���ţ�// xliang [4/28/2009] �䲻�߹㲥�߼�������Ϊ�㲥ԴҪ����
		StartSwitchToSubMt(tSrcMt, bySrcChnnl, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE, FALSE);
		break;
	case MCS_MCU_STARTSWITCHMC_REQ:
		StartSwitchToMc(tSrcMt, 0, cServMsgHdr.GetSrcSsnId(),tSwitchInfo.GetDstChlIdx(), byMode);
		break;
	case MCS_MCU_START_SWITCH_TW_REQ:
		StartSwitchToPeriEqp(tSrcMt, 0, tDstMt.GetEqpId(), tSwitchInfo.GetDstChlIdx(), byMode, SWITCH_MODE_SELECT);
		break;
	case MCS_MCU_START_SWITCH_HDU_REQ: // xliang [6/23/2009] ��2����������0����Ϊ���ܿ�VMP����ʱSrcChnnl�ɾ����������
		StartSwitchToPeriEqp(tSrcMt,
                             bySrcChnnl,
                             tDstMt.GetEqpId(),
                             tSwitchInfo.GetDstChlIdx(),
                             byMode,
                             SWITCH_MODE_SELECT, FALSE, TRUE, bBatchPollChg);
		break;
	default:
		ConfLog( FALSE, "Exception - wrong switch eventid %u!\n", cServMsgHdr.GetEventId() );
		return;
	}


	if ( TYPE_MT == tSrcMt.GetType())
	{
		NotifyMtFastUpdate( tSrcMt.GetMtId(), MODE_VIDEO );
	}
    else if ( TYPE_MCUPERI == tSrcMt.GetType() )
    {
		NotifyFastUpdate( tSrcMt, 0 );
		NotifyFastUpdate( tSrcMt, 1 );
		NotifyFastUpdate( tSrcMt, 2 );
    }
	else
	{
		ConfLog( FALSE, "Wrong type :tSrcMt.GetType = %d\n" ,tSrcMt.GetType());
	}
	
	if (m_tConf.GetConfAttrb().IsResendLosePack()/* && MODE_AUDIO != byMode*/)
	{
		//��Ŀ�ĵ�Rtcp������Դ
		TLogicalChannel tLogicalChannel;
        u8 bySrcChnnl = 0;
		u32 dwDstIp;
		u16 wDstPort;

        if (TYPE_MT == tDstMt.GetType())
        {
            bySrcChnnl = 0;
        }
        else if (TYPE_MCUPERI == tDstMt.GetType())
        {
            bySrcChnnl = tSwitchInfo.GetDstChlIdx();
        }
        
        if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
        {
            m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);

		    dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
		    wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();

            // guzh [7/25/2007] ѡ������ҲҪMap
            //�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�                        
            SwitchVideoRtcpToDst(dwDstIp, wDstPort, tDstMt, bySrcChnnl, MODE_VIDEO, SWITCH_MODE_SELECT, TRUE);
        }
    
        if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
        {
            m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE);

		    dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
		    wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();

            SwitchVideoRtcpToDst(dwDstIp, wDstPort, tDstMt, bySrcChnnl, MODE_AUDIO, SWITCH_MODE_SELECT, TRUE);
        }
	}

	return;
}

/*====================================================================
    ������      ��SwitchSrcToDstAccord2Adp
    ����        ����������Դ������Ŀ�ģ�ͨ�����䣩
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TSwitchInfo &tSwitchInfo, ������Ϣ
	              const CServMsg & cServMsgHdr,	ҵ����Ϣͷ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/05/22    4.5         �ű���         ����
====================================================================*/
BOOL32 CMcuVcInst::SwitchSrcToDstAccord2Adp(const TSwitchInfo & tSwitchInfo,
                                            const CServMsg & cServMsgHdr)
{
    if (!g_cMcuVcApp.IsSelAccord2Adp())
    {
        ConfLog(FALSE, "[SwitchSrcToDstAccord2Adp] failed due to debug not support!\n");
        return FALSE;
    }

    u8 bySrcMtId = tSwitchInfo.GetSrcMt().GetMtId();
    u8 byDstMtId = tSwitchInfo.GetDstMt().GetMtId();

    //ȡԴ�ն���Ŀ���ն˵�������
    TLogicalChannel tSrcChnl;
    TLogicalChannel tDstChnl;
    m_ptMtTable->GetMtLogicChnnl(bySrcMtId, LOGCHL_VIDEO, &tSrcChnl, FALSE);
    m_ptMtTable->GetMtLogicChnnl(byDstMtId, LOGCHL_VIDEO, &tDstChnl, TRUE);

    //�ݴ���
    if (tSrcChnl.GetChannelType() == tDstChnl.GetChannelType() &&
        tSrcChnl.GetVideoFormat() == tDstChnl.GetVideoFormat() &&
        tSrcChnl.GetChanVidFPS() == tDstChnl.GetChanVidFPS() )
    {
        ConfLog(FALSE, "[SwitchSrcToDstAccord2Adp] Src Eq Dst<%d,%s,%d>, switch directly!\n",
                        tSrcChnl.GetChannelType(),
                        GetResStr(tSrcChnl.GetVideoFormat()),
                        tSrcChnl.GetChanVidFPS());
        SwitchSrcToDst(tSwitchInfo, cServMsgHdr);
        return TRUE;
    }

    u8 bySwitchMode = tSwitchInfo.GetMode();
    TMt tSrcMt = m_ptMtTable->GetMt(bySrcMtId);
    TMt tDstMt = m_ptMtTable->GetMt(byDstMtId);

    return StartHdVidSelAdp(tSrcMt, tDstMt);
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
                         g_cMcuVcApp.FindMp( ptSwitchChnnl->GetDstIP() ) != 0 &&
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
    
    //video�㲥Դ
    if (!m_tVidBrdSrc.IsNull())
    {
        bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
        g_cMpManager.SetSwitchBridge(m_tVidBrdSrc, bySrcChnnl, MODE_VIDEO);
    
        if ((0 != m_tConf.GetSecBitRate() || MEDIA_TYPE_NULL != m_tConf.GetCapSupport().GetSecondSimCapSet().GetVideoMediaType())
            && m_tVidBrdSrc == m_tVmpEqp)
        {
            g_cMpManager.SetSwitchBridge(m_tVidBrdSrc, bySrcChnnl+1, MODE_VIDEO);           
        }
    }        

    //audio�㲥Դ
    if (!m_tAudBrdSrc.IsNull())
    {
        bySrcChnnl = (m_tAudBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;            
        g_cMpManager.SetSwitchBridge(m_tAudBrdSrc, bySrcChnnl, MODE_AUDIO);
    }        

    //bas
    if (m_tConf.m_tStatus.IsAudAdapting() && EQP_CHANNO_INVALID != m_byAudBasChnnl)
    {
        g_cMpManager.SetSwitchBridge(m_tAudBasEqp, m_byAudBasChnnl, MODE_AUDIO);
    }

    if (m_tConf.m_tStatus.IsVidAdapting() && EQP_CHANNO_INVALID != m_byVidBasChnnl)
    {
        g_cMpManager.SetSwitchBridge(m_tVidBasEqp, m_byVidBasChnnl, MODE_VIDEO);
    }

    if (m_tConf.m_tStatus.IsBrAdapting() && EQP_CHANNO_INVALID != m_byBrBasChnnl)
    {
        g_cMpManager.SetSwitchBridge(m_tBrBasEqp, m_byBrBasChnnl, MODE_VIDEO);
    }

    if (m_tConf.m_tStatus.IsCasdAudAdapting() && EQP_CHANNO_INVALID != m_byCasdAudBasChnnl)
    {
        g_cMpManager.SetSwitchBridge(m_tCasdAudBasEqp, m_byCasdAudBasChnnl, MODE_AUDIO);
    }

    if (m_tConf.m_tStatus.IsCasdVidAdapting() && EQP_CHANNO_INVALID != m_byCasdVidBasChnnl)
    {
        g_cMpManager.SetSwitchBridge(m_tCasdVidBasEqp, m_byCasdVidBasChnnl, MODE_VIDEO);
    }

    //˫��Դ
    if (!m_tDoubleStreamSrc.IsNull())
    {
        // zbq [09/04/2007] ��¼���˫�����󣬰���Ӧ�˿ڽ���
        u8 bySrcChn = m_tPlayEqp == m_tDoubleStreamSrc ? m_byPlayChnnl : 0;        
        g_cMpManager.SetSwitchBridge(m_tDoubleStreamSrc, bySrcChn, MODE_SECVIDEO);
    }

    //ѡ��Դ
    TMtStatus tStatus;
    TMt tTmpMt;
	for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (!m_tConfAllMtInfo.MtJoinedConf(byLoop))
        {
            continue;
        }      
		
		if (m_ptMtTable->GetMtStatus(byLoop, &tStatus))
        {
            tTmpMt = tStatus.GetSelectMt(MODE_VIDEO);
            if (!tTmpMt.IsNull() && m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMtId()))
            {               
                g_cMpManager.SetSwitchBridge(tTmpMt, 0, MODE_VIDEO);
            }

            tTmpMt = tStatus.GetSelectMt(MODE_AUDIO);
            if (!tTmpMt.IsNull() && m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMtId()))
            {               
                if (!m_tConf.m_tStatus.IsMixing())
                {
                    g_cMpManager.SetSwitchBridge(tTmpMt, 0, MODE_AUDIO);
                }                    
            }
        }
    }

    //�ش��ն�
	if(m_tCascadeMMCU.GetMtId() != 0)
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
        tTmpMt = ptConfMcInfo->m_tSpyMt;
		if( ptConfMcInfo != NULL && !tTmpMt.IsNull() &&
            m_ptMtTable->GetMtSndBitrate(tTmpMt.GetMtId()) > 0 )
		{
            u8 byMode = MODE_BOTH;
            if( TYPE_MT == m_ptMtTable->GetMainType( tTmpMt.GetMtId() ) && 
		        MT_MANU_KDC != m_ptMtTable->GetManuId( tTmpMt.GetMtId() ) ) 
            {
                if (m_ptMtTable->IsMtAudioDumb(tTmpMt.GetMtId()) || 
                    m_ptMtTable->IsMtAudioMute(tTmpMt.GetMtId()))
                {
                    byMode = MODE_VIDEO;
                }
            }
            
            if (m_tConf.m_tStatus.IsMixing())
            {
                byMode = MODE_VIDEO;
            }
		        
			g_cMpManager.SetSwitchBridge(tTmpMt, 0, byMode);
		}		    
	}

    return TRUE;    
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
			OspPrintf( TRUE, FALSE, "\nMp%d(%s) switch info: ", byLoop1 , StrOfIP(g_cMcuVcApp.GetMpIpAddr(byLoop1)) );
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
						l = sprintf( achTemp, "\n%d  %s:%d(src)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort, byMpId );
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

					OspPrintf( TRUE, FALSE, achTemp );
					
				}
			}
		}
	}
}

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
====================================================================*/
void CMcuVcInst::SwitchVideoRtcpToDst(u32 dwDstIp, u16 wDstPort, TMt tSrc, u8 bySrcChnnl, 
									  u8 byMode, u8 bySwitchMode, BOOL32 bMapSrcAddr)
{
    u32 dwSrcIpAddr, dwRecvIpAddr, dwOldDstIp, dwMapIpAddr;
    u16 wRecvPort, wOldDstPort, wMapPort;
    u8  byChlNum;
    u8  byChanType;

    u32 dwSwitchIpAddr;
    u16 wSwitchPort;

    TLogicalChannel tLogicalChannel;

    if (MODE_VIDEO != byMode && MODE_AUDIO != byMode)
    {
        return;
    }

    EqpLog("[SwitchVideoRtcpToDst] dwDstIp:0x%x, wDstPort:%u, tSrc<%d,%d>, bySrcChnnl:%d, byMode:%d\n",
            dwDstIp, wDstPort, tSrc.GetMcuId(), tSrc.GetMtId(), bySrcChnnl, byMode);

	wMapPort    = 0;
	dwMapIpAddr = 0;

	//�õ�ԭ����Ŀ�Ķ˿�
	switch(tSrc.GetType()) 
	{
	case TYPE_MT://�ն�	
		m_ptMtTable->GetMtRtcpDstAddr(tSrc.GetMtId(), dwOldDstIp, wOldDstPort, byMode);
        if (MODE_VIDEO == byMode)
        {
            byChanType = LOGCHL_VIDEO;
        }
        else if (MODE_AUDIO == byMode)
        {
            byChanType = LOGCHL_AUDIO;
        }
		m_ptMtTable->GetMtLogicChnnl(tSrc.GetMtId(), byChanType, &tLogicalChannel, TRUE);
		wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
		//�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�
		if (bMapSrcAddr)
		{
			wMapPort    = tLogicalChannel.GetRcvMediaChannel().GetPort();
			dwMapIpAddr = tLogicalChannel.GetRcvMediaChannel().GetIpAddr();
		}
		break;

	case TYPE_MCUPERI://����
        g_cMcuVcApp.GetEqpRtcpDstAddr(tSrc.GetEqpId(), bySrcChnnl, dwOldDstIp, wOldDstPort, byMode);
        g_cMcuVcApp.GetPeriEqpLogicChnnl(tSrc.GetEqpId(), byMode, &byChlNum, &tLogicalChannel, TRUE);
        switch(tSrc.GetEqpType())
        {
        case EQP_TYPE_VMP:
            {       
                if (g_cMpManager.GetVmpSwitchAddr(tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort))
                {
                    if (SWITCH_MODE_BROADCAST == bySwitchMode)
                    {
                        wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*bySrcChnnl;
                    }
                    else
                    {
                        //  xsl [6/3/2006] ����ϳɽ�������ͨ����mp��RTCP�˿ڷ���
						// xliang [5/6/2009] ����ԭ�� 0~7	===> 2~9 
						//							  8~15	===> 12~19
						//							  16~20	===> 22~25
                        if (bySrcChnnl >= 16)
						{
							wRecvPort = wSwitchPort + 6 + bySrcChnnl;
						}
						if (bySrcChnnl >= 8)
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
                    ConfLog(FALSE, "SwitchVideoRtcpToDst - GetVmpSwitchAddr error!\n");
                }
            }
        	break;  
            
        case EQP_TYPE_BAS:
            {
                wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*bySrcChnnl;
            }
            break;
        default:
            wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*bySrcChnnl;
        	break;
        }

		break;
	}

    if (TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_TVWALL == tSrc.GetEqpType())
    {
        TPeriEqpStatus tTWStatus;
        if (g_cMcuVcApp.GetPeriEqpStatus(tSrc.GetEqpId(), &tTWStatus))
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

	if (TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_HDU == tSrc.GetEqpType())
    {
        TPeriEqpStatus tHduStatus;
        if (g_cMcuVcApp.GetPeriEqpStatus(tSrc.GetEqpId(), &tHduStatus))
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
    if (0 != dwOldDstIp)
    {
        g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwOldDstIp, wOldDstPort);
    }

    //��ӽ���
    g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwSrcIpAddr, 0, dwRecvIpAddr,wRecvPort, 
		                             dwDstIp, wDstPort, 0, 0, dwMapIpAddr, wMapPort);

    //��¼����Ŀ��
    switch(tSrc.GetType()) 
    {
    case TYPE_MT://�ն�	
        m_ptMtTable->SetMtRtcpDstAddr(tSrc.GetMtId(), dwDstIp, wDstPort, byMode);
        break;

    case TYPE_MCUPERI://����
        g_cMcuVcApp.SetEqpRtcpDstAddr(tSrc.GetEqpId(), bySrcChnnl, dwDstIp, wDstPort, byMode);
        break;
    }
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

	u8 byMode = MODE_VIDEO;
	cServMsg.SetMsgBody(&byMode, sizeof(u8));
	SendMsgToMt( byMtId, MCU_MT_FASTUPDATEPIC_CMD, cServMsg );
	m_ptMtTable->SetLastTick( byMtId, dwCurTick );
	
    if (MT_MANU_TAIDE == m_ptMtTable->GetManuId(byMtId))
    {
        if( byCount < 1 )
        {
            SetTimer( pcMsg->event, 3600, 100*byMtId + byCount + 1  );
            CallLog("[ProcMcuMtFastUpdateTimer] mt.%d fastupdate has been adjust to once more due to Tandberg!\n", byMtId);
        }
    }
    else
    {
	    if( byCount < 3 )
	    {
		    SetTimer( pcMsg->event, 1200, 100*byMtId + byCount + 1  );
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

    CallLog("[ProcMcuEqpFastUpdateTimer] the Eqp.%u, chn.%u, byCount.%d!\n", byEqpId, byChnl, byCount);

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
            
            break;
        }
        
    case EQP_TYPE_VMPTW:
        wEvent = MCU_VMPTW_FASTUPDATEPIC_CMD;
        dwLaskTick = m_dwVmpTwLastVCUTick;
        break;
        
    case EQP_TYPE_BAS:
        
        if ( !g_cMcuAgent.IsEqpBasHD( byEqpId ) )
        {
            cServMsg.SetChnIndex(byChnl);
            wEvent = MCU_BAS_FASTUPDATEPIC_CMD;
            SetBasLastVCUTick( byChnl, dwCurTick );
        }
        else
        {
            cServMsg.SetChnIndex(byChnl);
            wEvent = MCU_BAS_FASTUPDATEPIC_CMD;
            m_cBasMgr.SetChnVcuTick( tDstEqp, byChnl, dwCurTick );
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


// END OF FILE
