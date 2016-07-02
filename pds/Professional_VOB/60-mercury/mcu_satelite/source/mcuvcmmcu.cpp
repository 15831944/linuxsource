/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcuvcmmcu.cpp
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
#include "evmcuvcs.h"
#include "evmcumt.h"
#include "evmcueqp.h"
#include "evmcu.h"
#include "evmcutest.h"
#include "mcuvc.h"
#include "mcsssn.h"
#include "mtadpssn.h"
#include "mcuerrcode.h"

//��Ϣ��:TMtAlias(mt����)+TMtAlias(mt e164)+TMtAlias(mt ip)+TMtAlias(��MCU�Ļ�����)+
//byEncrypt(1-����,0-������)+TCreateConfParams
/*====================================================================
    ������      ��DaemonProcMtMcuCreateConfReq
    ����        �������ն˵Ĵ�����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/08/17    3.0         ������         ����
====================================================================*/
void CMcuVcInst::DaemonProcMtMcuCreateConfReq( const CMessage * pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );	

    //xsl [11/28/2006] N+1����ģʽ�²������������л���
    if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_IDLE)
    {
        OspPrintf(TRUE, FALSE, "[DaemonProcMtMcuCreateConfReq] mt can't create conf in n+1 mode!\n");
		g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
        return;
    }

	TMtAlias tMtH323Alias = *(TMtAlias*)cServMsg.GetMsgBody();
	TMtAlias tMtE164Alias = *(((TMtAlias *)cServMsg.GetMsgBody())+1);
	TMtAlias tMtAddr    = *(((TMtAlias *)cServMsg.GetMsgBody())+2);
	TMtAlias tConfAlias = *(((TMtAlias *)cServMsg.GetMsgBody())+3);
	u8       byType     = *(cServMsg.GetMsgBody()+4*sizeof(TMtAlias));
	u8       byEncrypt  = *(cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8));
	u16      wCallRate  = *(u16*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)));
    u16      wConfFirstRate = 0;
    u16      wConfSecondRate = 0;

	TConfInfo *ptConfInfo = NULL;
	if (cServMsg.GetMsgBodyLen() > 4*sizeof(TMtAlias)+2*sizeof(u8)+sizeof(u16))
	{
		ptConfInfo = (TConfInfo *)(cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+2*sizeof(u8)+sizeof(u16));
	}

	cServMsg.SetSrcMtId(CONF_CREATE_MT);

    TLocalInfo tLocalInfo;
    g_cMcuAgent.GetLocalInfo(&tLocalInfo);
    u8 byCmpLen = max(strlen(tLocalInfo.GetE164Num()), strlen(tConfAlias.m_achAlias));
    
    // zbq [06/23/2007] �ն˴���: 1��������E164��� �� ��ģ��E164ͨ��ģ�崴�᣻2����MCU����/E164 ��������
    if ( mtAliasTypeE164 == tConfAlias.m_AliasType &&
         0 != memcmp( tLocalInfo.GetE164Num(), tConfAlias.m_achAlias, byCmpLen ) )
    {
        u8 byTemplateConfIdx = g_cMcuVcApp.GetTemConfIdxByE164(tConfAlias.m_achAlias);
	    u8 byOnGoingConfIdx  = g_cMcuVcApp.GetOngoingConfIdxByE164(tConfAlias.m_achAlias);

        CallLog("[DaemonProcMtMcuCreateConfReq] byTemplateConfIdx.%d byOnGoingConfIdx.%d\n", 
            byTemplateConfIdx, byOnGoingConfIdx);
  
	    if (byOnGoingConfIdx > 0)  //�ն˺��л���
	    {
			// ����VCS�������飬���¼�����ϯ�ѱ����ȣ���������м���
			CConfId cConfId = g_cMcuVcApp.GetConfIdByE164(tConfAlias.m_achAlias, FALSE);
			if (!cConfId.IsNull() && VCS_CONF == cConfId.GetConfSource() && TYPE_MCU == byType)
			{
                CallLog("[DaemonProcMtMcuCreateConfReq] subconf has been created by other vcs\n");
				cServMsg.SetErrorCode(ERR_MCU_VCS_SMCUINVC);
                g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
			}

            //���ն˼�����顣���������׼����֤
		    cServMsg.SetConfIdx(byOnGoingConfIdx);
		    cServMsg.SetMsgBody((u8*)&tMtH323Alias, sizeof(tMtH323Alias));
		    cServMsg.CatMsgBody((u8*)&tMtE164Alias, sizeof(tMtE164Alias));
		    cServMsg.CatMsgBody((u8*)&tMtAddr, sizeof(tMtAddr ));
		    cServMsg.CatMsgBody((u8*)&tConfAlias, sizeof(tConfAlias));
		    cServMsg.CatMsgBody(&byType, sizeof(byType));
		    cServMsg.CatMsgBody(&byEncrypt, sizeof(byEncrypt));
		    cServMsg.CatMsgBody((u8*)&wCallRate, sizeof(wCallRate));
		    g_cMcuVcApp.SendMsgToConf( byOnGoingConfIdx, MT_MCU_MTJOINCONF_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	    
        }
	    else if (byTemplateConfIdx > 0)  //���л���ģ��,��������
	    {
            //�û��鿪ʼ
		    cServMsg.SetConfIdx(byTemplateConfIdx);
		    TTemplateInfo tTemConf;
		    if (!g_cMcuVcApp.GetTemplate(byTemplateConfIdx, tTemConf))
		    {
                CallLog("[DaemonProcMtMcuCreateConfReq] Get Template %d failed!\n", byTemplateConfIdx);
                g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }

		    //��������
		    if (0 == byEncrypt && 
			    CONF_ENCRYPTMODE_NONE != tTemConf.m_tConfInfo.GetConfAttrb().GetEncryptMode())
		    {
                CallLog("[DaemonProcMtMcuCreateConfReq] Template encrypt!\n");
			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }

            //  xsl [2/13/2006] ��CreateConf����������ն˳ɹ����ack
            //SendReplyBack(cServMsg, cServMsg.GetEventId()+1);

		    tTemConf.m_tConfInfo.m_tStatus.SetOngoing();
		    tTemConf.m_tConfInfo.SetStartTime( time(NULL) );    
		    tTemConf.m_tConfInfo.m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );

		    wCallRate -= GetAudioBitrate( tTemConf.m_tConfInfo.GetMainAudioMediaType() );

		    TMtAlias atMtAlias[MAXNUM_CONF_MT+1];
            u16      awMtDialBitRate[MAXNUM_CONF_MT+1] = {0};
		    u8       byMtNumInUse = 0;
		    
            // zbq [09/19/2007] ��ģ�崴�ᣬ���ָ��ն���ԭģ���еĺ���ģʽ����; ��ԭΪIP,ע�Ᵽ���˿�.
		    atMtAlias[0] = tMtAddr;
		    awMtDialBitRate[0] = wCallRate;
		    byMtNumInUse = 1;

		    // ˢ���ն��б�����ǽ�ͻ���ϳ�ģ��
            TMultiTvWallModule tNewTvwallModule = tTemConf.m_tMultiTvWallModule;
            TVmpModule         tNewVmpModule    = tTemConf.m_atVmpModule;
            
		    BOOL32 bInMtTable = FALSE;  
            BOOL32 bRepeatThisLoop;
		    for (u8 byLoop = 0; byLoop < tTemConf.m_byMtNum; byLoop++)
		    {
                bRepeatThisLoop = FALSE;
                //����뽨���ն�һ�£����ظ���� (bRepeatThisLoop = TRUE)
			    if (mtAliasTypeH320ID == tTemConf.m_atMtAlias[byLoop].m_AliasType)
			    {
				    if (0 == memcmp(tTemConf.m_atMtAlias[byLoop].m_achAlias, tMtH323Alias.m_achAlias, MAXLEN_ALIAS))
				    {
					    bInMtTable = TRUE;
                        bRepeatThisLoop = TRUE;
                        
                        // zbq [09/19/2007] ȡ�����ĺ���ģʽ
                        atMtAlias[0] = tMtH323Alias;
				    }			    
			    }
			    else if (mtAliasTypeTransportAddress == tTemConf.m_atMtAlias[byLoop].m_AliasType)
			    {
				    if (tTemConf.m_atMtAlias[byLoop].m_tTransportAddr.m_dwIpAddr == tMtAddr.m_tTransportAddr.m_dwIpAddr //&& 
                        // zbq [09/19/2007] �²��Բ��ٱȽ϶˿�
					    // tTemConf.m_atMtAlias[byLoop].m_tTransportAddr.m_wPort == tMtAddr.m_tTransportAddr.m_wPort
                        )
				    {
					    bInMtTable = TRUE;
                        bRepeatThisLoop = TRUE;

                        // zbq [09/19/2007] ȡ�����ĺ���ģʽ, �������˿�
                        if ( tMtAddr.m_tTransportAddr.GetPort() != tTemConf.m_atMtAlias[byLoop].m_tTransportAddr.GetPort() )
                        {
                            tMtAddr.m_tTransportAddr.SetPort(tTemConf.m_atMtAlias[byLoop].m_tTransportAddr.GetPort());
                        }
                        atMtAlias[0] = tMtAddr;
				    }
			    }
			    else 
			    {
				    if (mtAliasTypeE164 == tTemConf.m_atMtAlias[byLoop].m_AliasType)
				    {
					    if (0 == memcmp(tTemConf.m_atMtAlias[byLoop].m_achAlias, tMtE164Alias.m_achAlias, MAXLEN_ALIAS))
					    {
						    bInMtTable = TRUE;
                            bRepeatThisLoop = TRUE;
                            
                            // zbq [09/19/2007] ȡ�����ĺ���ģʽ
                            atMtAlias[0] = tMtE164Alias;
					    }
				    } 
				    else 
				    {
					    if (mtAliasTypeH323ID == tTemConf.m_atMtAlias[byLoop].m_AliasType)
					    {
						    if (0 == memcmp(tTemConf.m_atMtAlias[byLoop].m_achAlias, tMtH323Alias.m_achAlias, MAXLEN_ALIAS))
						    {
                                bInMtTable = TRUE;
                                bRepeatThisLoop = TRUE;

                                // zbq [09/19/2007] ȡ�����ĺ���ģʽ
                                atMtAlias[0] = tMtH323Alias;
						    }
					    }
				    }
			    }

                if (!bRepeatThisLoop)
                {
                    atMtAlias[byMtNumInUse] = tTemConf.m_atMtAlias[byLoop];
                    awMtDialBitRate[byMtNumInUse] = tTemConf.m_awMtDialBitRate[byLoop];
                    byMtNumInUse++;
                }	

                //ÿ��ѭ�����һ���ն˱�����Ҫ����ԭ���ڻ���ϳɺ͵���ǽ������±���£�
                //��������������������봴������ͬ�������ڵ��±���1����ͬ�����±��ǵ�ǰ���ն�ĩβ
                UpdateConfStoreModule(tTemConf, 
                                      byLoop+1, bRepeatThisLoop ? 1 : byMtNumInUse,
                                      tNewTvwallModule, tNewVmpModule);
		    }
            
		    //���鿪��ģʽ
		    //����Ŀ���ģʽ
		    if ( CONF_OPENMODE_CLOSED  != tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() &&
			     CONF_OPENMODE_NEEDPWD != tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() &&
			     CONF_OPENMODE_OPEN    != tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() ) 
		    {
                CallLog("[DaemonProcMtMcuCreateConfReq] Wrong Open Mode!\n");
			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }
		    //������
		    if ( CONF_OPENMODE_CLOSED == tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() )
		    {
                CallLog("[DaemonProcMtMcuCreateConfReq] Open Mode is CLOSE!\n");
			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }
			// xliang [12/26/2008] (modify for MT calling MCU initially) 
			//cancel limit here, but need to input password later
		    //��������������
// 		    else if ( CONF_OPENMODE_NEEDPWD == tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() &&
// 				      !bInMtTable )
// 		    {
//                 CallLog("[DaemonProcMtMcuCreateConfReq] Conf Need Password!\n");
// 			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
// 			    return;
// 		    }
            // ���������б��Ҳ��������ַ��
//             if (!bInMtTable && 
//                 (byType != TYPE_MCU) &&   // MCU �򲻿���IP�Ƿ������
//                 !g_cMcuVcApp.IsMtIpInAllowSeg( tTemConf.m_tConfInfo.GetUsrGrpId(), tMtAddr.m_tTransportAddr.GetNetSeqIpAddr()))
//             {
//                 CallLog("[DaemonProcMtMcuCreateConfReq] Type is not MCU(type=%d), neither in Allow IP range(0x%x)!\n", 
//                         byType, 
//                         tMtAddr.m_tTransportAddr.GetIpAddr());
// 			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
// 			    return;
//             }
        		    
		    //�ն������� MAXNUM_CONF_MT����������ĩβһ���ն�
		    if (byMtNumInUse > MAXNUM_CONF_MT)
		    {
			    byMtNumInUse = MAXNUM_CONF_MT;
		    }
		    
		    //�ն˱��������� 
		    s8  achAliasBuf[SERV_MSG_LEN];
		    u16 wAliasBufLen = 0;
		    PackTMtAliasArray(atMtAlias, awMtDialBitRate, byMtNumInUse, achAliasBuf, wAliasBufLen);
		    wAliasBufLen = htons(wAliasBufLen);
		    
		    cServMsg.SetMsgBody((u8 *)&tTemConf.m_tConfInfo, sizeof(TConfInfo));
		    cServMsg.CatMsgBody((u8 *)&wAliasBufLen, sizeof(wAliasBufLen));
		    cServMsg.CatMsgBody((u8 *)achAliasBuf, ntohs(wAliasBufLen));
		    if (tTemConf.m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
		    {
			    cServMsg.CatMsgBody((u8*)&tNewTvwallModule, sizeof(TMultiTvWallModule));
		    }
		    if (tTemConf.m_tConfInfo.GetConfAttrb().IsHasVmpModule())
		    {
			    cServMsg.CatMsgBody((u8*)&tNewVmpModule, sizeof(TVmpModule));
		    }

			// ����VCS������Ҫ�������ǽ���¼�mcu
			if (VCS_CONF == tTemConf.m_tConfInfo.GetConfSource())
			{
				u8 byIsHDTWCfg = (u8)tTemConf.IsHDTWCfg();
				cServMsg.CatMsgBody(&byIsHDTWCfg, sizeof(u8));
				// �����˸������ǽ
				if (byIsHDTWCfg)
				{
					cServMsg.CatMsgBody((u8*)&tTemConf.m_tHDTWInfo, sizeof(THDTvWall));
				}
				// �������¼�mcu
				u8 byIsVCSSMCUCfg = (u8)tTemConf.IsVCSMCUCfg();
				cServMsg.CatMsgBody(&byIsVCSSMCUCfg, sizeof(u8));
				if (byIsVCSSMCUCfg)
				{
					PackTMtAliasArray(tTemConf.m_tVCSSMCUCfg.m_atSMCUAlias, tTemConf.m_tVCSSMCUCfg.m_awSMCUDialBitRate,
									  tTemConf.m_tVCSSMCUCfg.m_wSMCUNum, achAliasBuf, wAliasBufLen);
					wAliasBufLen = htons(wAliasBufLen);
					cServMsg.CatMsgBody((u8*)&wAliasBufLen, sizeof(u16));
					cServMsg.CatMsgBody((u8*)achAliasBuf, ntohs(wAliasBufLen));
				}
			}				
		    cServMsg.SetConfIdx(byTemplateConfIdx);
		    cServMsg.CatMsgBody((u8*)&tMtH323Alias, sizeof(tMtH323Alias));
		    cServMsg.CatMsgBody((u8*)&tMtE164Alias, sizeof(tMtE164Alias));
		    cServMsg.CatMsgBody((u8*)&tMtAddr, sizeof(tMtAddr));
		    cServMsg.CatMsgBody(&byType, sizeof(byType));
			// xliang [12/26/2008] (modify for MT call MCU initially) add bInMtTalbe Info
			cServMsg.CatMsgBody((u8*)&bInMtTable,sizeof(bInMtTable));

            u8 byInsID = AssignIdleConfInsID();
            if(0 != byInsID)
            {
				if (VCS_CONF == tTemConf.m_tConfInfo.GetConfSource())
				{
					ConfLog(FALSE, "[DaemonProcMtMcuCreateConfReq] VCSConf start command to inst%d\n",
								   byInsID);
					post(MAKEIID( AID_MCU_VC, byInsID ), MCU_SCHEDULE_VCSCONF_START, 
						 cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				}
				else
				{
	                post(MAKEIID( AID_MCU_VC, byInsID ), MCU_SCHEDULE_CONF_START, 
						 cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
           
				}
            }
            else
            {
                ConfLog(FALSE, "[DaemonProcMtMcuCreateConfReq] assign instance id failed!\n");
            }
        }
        else
        {
            cServMsg.SetErrorCode( ERR_MCU_NULLCID );
            g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
            ConfLog(FALSE, "[DaemonProcMtMcuCreateConfReq] UNEXIST E164.%s to be joined or created !\n", tConfAlias.m_achAlias);
        }
        
        return;
    }

    if (NULL != ptConfInfo)  //��������
	{		
		s8  achAliasBuf[SERV_MSG_LEN];
		u16 wAliasBufLen = 0;			

		if ((0 == byEncrypt && 
			 CONF_ENCRYPTMODE_NONE != ptConfInfo->GetConfAttrb().GetEncryptMode()) || 
			(0 != byEncrypt && 
			 CONF_ENCRYPTMODE_NONE == ptConfInfo->GetConfAttrb().GetEncryptMode()))
		{
			cServMsg.SetErrorCode( ERR_MCU_SETENCRYPT );
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			ConfLog( FALSE, "Conference %s failure because encrypt setting is confused - byEncrypt.%d EncryptMode.%d!\n", 
					        ptConfInfo->GetConfName(), byEncrypt, ptConfInfo->GetConfAttrb().GetEncryptMode() );
			return;
		}

		ptConfInfo->m_tStatus.SetOngoing();
		ptConfInfo->SetStartTime( time( NULL ) );
		ptConfInfo->SetStartTime( 0 );
		ptConfInfo->m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );

		wCallRate -= GetAudioBitrate( ptConfInfo->GetMainAudioMediaType() );
		
        //zbq [09/19/2007] �ն˴��� ������¼����: MCU��ǰע����GK,���ΰ����ȼ�ȡE164/H323ID/IP������,ֻȡIP.��ȡIP,�������˿�.
        TMtAlias tMtCreateAlias;
        if ( g_cMcuVcApp.GetConfRegState(0) )
        {
            if (!tMtE164Alias.IsNull())
            {
                tMtCreateAlias = tMtE164Alias;
            }
            else if (!tMtH323Alias.IsNull())
            {
                tMtCreateAlias = tMtH323Alias;
            }
            else if (!tMtAddr.IsNull())
            {
                tMtCreateAlias = tMtAddr;
                if ( MCU_Q931_PORT != tMtCreateAlias.m_tTransportAddr.GetPort())
                {
                    tMtCreateAlias.m_tTransportAddr.SetPort(MCU_Q931_PORT);
                }
            }
            else
            {
                g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
                ConfLog( FALSE, "Conference %s failure because create mt has no Alias(GK exist), ignore it!\n", ptConfInfo->GetConfName());
                return;                
            }
        }
        else
        {
            if (!tMtAddr.IsNull())
            {
                tMtCreateAlias = tMtAddr;
                if ( MCU_Q931_PORT != tMtCreateAlias.m_tTransportAddr.GetPort())
                {
                    tMtCreateAlias.m_tTransportAddr.SetPort(MCU_Q931_PORT);
                }
            }
            else
            {
                g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
                ConfLog( FALSE, "Conference %s failure because create mt has no Alias, ignore it!\n", ptConfInfo->GetConfName());
                return;                
            }
        }
        
		//�ն˱��������� 
		// xliang [1/16/2009] ����Ƿ�Ҫ���ϻ���ģ����Ϣ�е������ն�----����
		PackTMtAliasArray(&tMtCreateAlias, &wCallRate, 1, achAliasBuf, wAliasBufLen);
		wAliasBufLen = htons(wAliasBufLen);

		cServMsg.SetMsgBody((u8 *)ptConfInfo, sizeof(TConfInfo));
		cServMsg.CatMsgBody((u8*)&wAliasBufLen, sizeof(wAliasBufLen));
		cServMsg.CatMsgBody((u8*)achAliasBuf, ntohs(wAliasBufLen));
		//TW Module ??
		//VMP Module ??
		cServMsg.CatMsgBody((u8*)&tMtH323Alias, sizeof(tMtH323Alias));
		cServMsg.CatMsgBody((u8*)&tMtE164Alias, sizeof(tMtE164Alias));
		cServMsg.CatMsgBody((u8*)&tMtAddr, sizeof(tMtAddr));
		cServMsg.CatMsgBody((u8*)&byType, sizeof(byType));

        u8 byInsID = AssignIdleConfInsID();
        if(0 != byInsID)
        {
            ::OspPost(MAKEIID( AID_MCU_VC, byInsID ), cServMsg.GetEventId(), 
                        cServMsg.GetServMsg(),cServMsg.GetServMsgLen());
            CallLog("[DaemonProcMtMcuCreateConfReq] mt create conf, insId is %d\n", byInsID);
        }
        else
        {
            ConfLog(FALSE, "[DaemonProcMtMcuCreateConfReq] assign instance id failed!\n");
        }
		
	}
	else  //�������
	{
		cServMsg.SetMsgBody((u8*)&tMtH323Alias, sizeof(tMtH323Alias));
		cServMsg.CatMsgBody((u8*)&tMtE164Alias, sizeof(tMtE164Alias));
		cServMsg.CatMsgBody((u8*)&tMtAddr, sizeof(tMtAddr));
		cServMsg.CatMsgBody((u8*)&tConfAlias, sizeof(tConfAlias));
		cServMsg.CatMsgBody(&byType, sizeof(byType));
		cServMsg.CatMsgBody(&byEncrypt, sizeof(byEncrypt));
		cServMsg.CatMsgBody((u8*)&wCallRate, sizeof(wCallRate));
		g_cMcuVcApp.SendMsgToDaemonConf( MT_MCU_MTJOINCONF_REQ, cServMsg.GetServMsg(),cServMsg.GetServMsgLen() );
        CallLog("[DaemonProcMtMcuCreateConfReq] send MT_MCU_MTJOINCONF_REQ to daemon conf\n");
	}

	return;
}

/*====================================================================
    ������      ��ProcMcuMcuRegisterNotify
    ����        ��mcu�����ɹ����ע����Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/21    3.6         Jason         ����
    2005/11/3   4.0			����        ���Ӻϲ�����ʱ�Ļ�������У��
====================================================================*/
void CMcuVcInst::ProcMcuMcuRegisterNotify(  const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	s8 *pszPwd  = (s8 *)cServMsg.GetMsgBody();
	s32 nPwdLen = cServMsg.GetMsgBodyLen();
	
	if( tMt.IsNull() )
	{
		return;
	}

	switch( CurState() )
	{
	case STATE_ONGOING:
		if ( m_tConfAllMtInfo.MtJoinedConf(tMt.GetMcuId(), tMt.GetMtId()) || 
			 TRUE == DealMtMcuEnterPwdRsp(tMt, pszPwd, nPwdLen) )
		{
			cServMsg.SetMsgBody((u8 *)&(m_tConfAllMtInfo.m_tLocalMtInfo), sizeof(TConfMtInfo));
			SendMsgToMt( tMt.GetMtId(), MCU_MCU_ROSTER_NOTIF,  cServMsg);
            MMcuLog("Recv MCU_MCU_REGISTER_NOTIF, Sending MCU_MCU_ROSTER_NOTIF!\n");
		}
		else
		{
			MMcuLog("Recv MCU_MCU_REGISTER_NOTIF but ConfPwd-Validate Err and drop mt.%d\n", tMt.GetMtId());
		}

        //����Ӧ�¼�MCU�ļ���ͨ���򿪴������㣬��Ϊ�򿪳ɹ� [01/11/2006-zbq]
        m_abyCasChnCheckTimes[tMt.GetMtId()-1] = 0;	
		MMcuLog("Recv MCU_MCU_REGISTER_NOTIF, m_abyCasChnCheckTimes[%d]=0 !\n", tMt.GetMtId()-1);
		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*====================================================================
    ������      ��ProcMcuMcuRosterNotify
    ����        ��mcu�����ɹ�������֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/21    3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuRosterNotify(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt  tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	TConfMtInfo *ptConfMtInfo = (TConfMtInfo *)(cServMsg.GetMsgBody());

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		//ˢ��TConfMtInfo
		m_tConfAllMtInfo.SetMtInfo( *ptConfMtInfo );

		//�����ն��б�
		TMcuMcuReq tReq;
		memset(&tReq, 0, sizeof(tReq));
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		SendMsgToMt( tMt.GetMtId(), MCU_MCU_MTLIST_REQ,  cServMsg);
        
        MMcuLog("Recv MCU_MCU_ROSTER_NOTIF and send MCU_MCU_MTLIST_REQ to mt.%d\n", tMt.GetMtId());

		//������ػ��������ն���Ϣ
		cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
		SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}


/*====================================================================
    ������      ��ProcMcuMcuMtListReq
    ����        ���Է�mcu�����ն��б�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/21    3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuMtListReq(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt  tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	TMt tTmpMt;
    TMcuMcuMtInfo atInfo[MAXNUM_CONF_MT];

	u8 byInfoNum = 0;
	TLogicalChannel tChannel;
	TMtStatus tMtStatus;
	
	STATECHECK;		

	//�����ն��б�
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMtId());
    // ���� [5/31/2006] MtIdѭ��Ӧ����1-192
	for(u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
	{
		tTmpMt = m_ptMtTable->GetMt(byLoop);
		if(!tTmpMt.IsLocal())
		{
			continue;
		}
		atInfo[byInfoNum].m_tMt = tTmpMt;
		strncpy((s8 *)(atInfo[byInfoNum].m_szMtName), m_ptMtTable->GetMtAliasFromExt(byLoop), sizeof(atInfo[byInfoNum].m_szMtName) - 1);
		atInfo[byInfoNum].m_dwMtIp   = htonl(m_ptMtTable->GetIPAddr(byLoop));
		atInfo[byInfoNum].m_byMtType = m_ptMtTable->GetMtType(byLoop);
		atInfo[byInfoNum].m_byManuId = m_ptMtTable->GetManuId(byLoop);
        atInfo[byInfoNum].m_byProtocolType = m_ptMtTable->GetProtocolType(byLoop);
		strncpy((s8 *)atInfo[byInfoNum].m_szMtDesc, "notsupport", sizeof(atInfo[byInfoNum].m_szMtDesc));
		atInfo[byInfoNum].m_szMtDesc[sizeof(atInfo[byInfoNum].m_szMtDesc)-1] = 0;
		if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tChannel, FALSE ) )
		{
			atInfo[byInfoNum].m_byVideoIn = tChannel.GetChannelType();
		}
		else
		{
			atInfo[byInfoNum].m_byVideoIn = MEDIA_TYPE_NULL;
		}
		if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tChannel, TRUE ) )
		{
			atInfo[byInfoNum].m_byVideoOut = tChannel.GetChannelType();
		}
		else
		{
			atInfo[byInfoNum].m_byVideoOut = MEDIA_TYPE_NULL;
		}
		if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tChannel, FALSE ) )
		{
			atInfo[byInfoNum].m_byVideo2In = tChannel.GetChannelType();
		}
		else
		{
			atInfo[byInfoNum].m_byVideo2In = MEDIA_TYPE_NULL;
		}
		if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tChannel, TRUE ) )
		{
			atInfo[byInfoNum].m_byVideo2Out = tChannel.GetChannelType();
		}
		else
		{
			atInfo[byInfoNum].m_byVideo2Out = MEDIA_TYPE_NULL;
		}
		if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tChannel, FALSE ) )
		{
			atInfo[byInfoNum].m_byAudioIn = tChannel.GetChannelType();
		}
		else
		{
			atInfo[byInfoNum].m_byAudioIn = MEDIA_TYPE_NULL;
		}
		if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tChannel, TRUE ) )
		{
			atInfo[byInfoNum].m_byAudioOut = tChannel.GetChannelType();
		}
		else
		{
			atInfo[byInfoNum].m_byAudioOut = MEDIA_TYPE_NULL;
		}
		atInfo[byInfoNum].m_byIsDataMeeting =  m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_T120DATA, &tChannel, TRUE ) ? 1:0;
		if(atInfo[byInfoNum].m_byIsDataMeeting == 0)
		{
			atInfo[byInfoNum].m_byIsDataMeeting =  m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_T120DATA, &tChannel, FALSE ) ? 1:0;
		}	
		m_ptMtTable->GetMtStatus(byLoop, &tMtStatus);
		atInfo[byInfoNum].m_byIsVideoMuteIn	= tMtStatus.IsSendVideo() ? 0:1;
		atInfo[byInfoNum].m_byIsVideoMuteOut	= tMtStatus.IsReceiveVideo() ? 0:1;   
		atInfo[byInfoNum].m_byIsAudioMuteIn	= tMtStatus.IsSendAudio() ? 0:1;
		atInfo[byInfoNum].m_byIsAudioMuteOut	= tMtStatus.IsReceiveAudio() ? 0:1;
		atInfo[byInfoNum].m_byIsConnected     = m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMcuId(), tTmpMt.GetMtId()) ? 1:0;
		atInfo[byInfoNum].m_byIsFECCEnable    = m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_H224DATA, &tChannel, TRUE ); 
	
		atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount = 0;
		if(tTmpMt == m_tConf.GetSpeaker())
		{
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_nViewID = m_dwSpeakerViewId;
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_bySubframeIndex = 0;
			atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount++;
		}
		if(m_tConf.GetStatus().GetVmpParam().IsMtInMember(tTmpMt))
		{
			u8 byMemberId  = m_tConf.GetStatus().GetVmpParam().GetChlOfMtInMember( tTmpMt );
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_nViewID = m_dwVmpViewId;
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_bySubframeIndex = byMemberId;
			atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount++;
		}
		
		if(ptMcInfo != NULL&&ptMcInfo->m_tSpyMt == tTmpMt)
		{
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_nViewID = ptMcInfo->m_dwSpyViewId;
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_bySubframeIndex = 0;
			atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount++;					
		}
    
		atInfo[byInfoNum].m_tPartVideoInfo.m_nOutputLID = 0;
		atInfo[byInfoNum].m_tPartVideoInfo.m_nOutVideoSchemeID = 0;
		TMt tMtVSrc;
		m_ptMtTable->GetMtSrc(byLoop, &tMtVSrc, MODE_VIDEO);
		if(tMtVSrc.IsNull())
		{
			atInfo[byInfoNum].m_tPartVideoInfo.m_nOutputLID = 0;
			atInfo[byInfoNum].m_tPartVideoInfo.m_nOutVideoSchemeID = 0;
		}

		if(tMt.GetMtId() == tTmpMt.GetMtId())
		{
			if(ptMcInfo != NULL&& 
				ptMcInfo->m_tSpyMt.GetMtId() == tMtVSrc.GetMtId())
			{
				atInfo[byInfoNum].m_tPartVideoInfo.m_nOutputLID = ptMcInfo->m_dwSpyViewId;				
				atInfo[byInfoNum].m_tPartVideoInfo.m_nOutVideoSchemeID = ptMcInfo->m_dwSpyVideoId;
			}
		}

		if(tMtVSrc == m_tConf.GetSpeaker())
		{
			atInfo[byInfoNum].m_tPartVideoInfo.m_nOutputLID = m_dwSpeakerViewId;
			atInfo[byInfoNum].m_tPartVideoInfo.m_nOutVideoSchemeID = m_dwSpeakerVideoId;
		}
		else if(tMtVSrc == m_tVmpEqp)
		{
			atInfo[byInfoNum].m_tPartVideoInfo.m_nOutputLID = m_dwVmpViewId;
			atInfo[byInfoNum].m_tPartVideoInfo.m_nOutVideoSchemeID = m_dwVmpVideoId;
		}

		if( tMt.GetMtId() == tTmpMt.GetMtId() && tMt == m_tCascadeMMCU )
		{
			if(ptMcInfo != NULL)
			{
				atInfo[byInfoNum].m_tPartVideoInfo.m_nOutputLID = ptMcInfo->m_dwSpyViewId;				
				atInfo[byInfoNum].m_tPartVideoInfo.m_nOutVideoSchemeID = ptMcInfo->m_dwSpyVideoId;
			}
		}
   
		byInfoNum++;	
	}

	MMcuLog( "[ProcMcuMcuMtListReq] Notified McuID.%d LocalTotalMtNum.%d\n", tMt.GetMtId(), byInfoNum );

	u8 byLastPack = 1;
	cServMsg.SetMsgBody(&byLastPack, sizeof(u8));
	cServMsg.CatMsgBody((u8 *)atInfo, byInfoNum * sizeof(TMcuMcuMtInfo));
	SendMsgToMt( tMt.GetMtId(), MCU_MCU_MTLIST_ACK,  cServMsg);

	//�����ϼ�MCU��ĵ�һ���¼�MCU�����ն�״̬ǿ��֪ͨ��֮��ĸ����ն�״̬����ʵʱ֪ͨ
	if( m_tConfInStatus.IsNtfMtStatus2MMcu() ||
		// vcsˢ�б�ͬʱ��Ҫˢ����״̬��Ϣ���������а�����Ҫ���ն˺���ģʽ��Ϣ
		VCS_CONF == m_tConf.GetConfSource())
	{
		OnNtfMtStatusToMMcu( m_tCascadeMMCU.GetMtId() );
		m_tConfInStatus.SetNtfMtStatus2MMcu(FALSE);
	}

	return;
}

/*====================================================================
    ������      ��GetConfViewInfo
    ����        ����ȡ��ͼ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMcuId            [In]
                  TCConfViewInfo &tInfo [Out]
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/05/14    4.0         ����         ����
====================================================================*/
void CMcuVcInst::GetConfViewInfo(u8 byMcuId, TCConfViewInfo &tInfo)
{
    u8 byViewIndex = 0;
    tInfo.m_byDefViewIndex = 0;
    
    TCapSupport tCapSupport = m_tConf.GetCapSupport();

    //spy view 
    TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(byMcuId);
    if(ptConfMcInfo == NULL)
    {
        return;
    }
    tInfo.m_atViewInfo[byViewIndex].m_nViewId		= ptConfMcInfo->m_dwSpyViewId;
    tInfo.m_atViewInfo[byViewIndex].m_byRes			=  VIDEO_FORMAT_CIF;//Ŀǰ�汾ֻ֧��h263-cif
    tInfo.m_atViewInfo[byViewIndex].m_byCurGeoIndex	= 0;
    tInfo.m_atViewInfo[byViewIndex].m_byGeoCount	= 1;
    tInfo.m_atViewInfo[byViewIndex].m_abyGeoInfo[0]	= 1;
    if(ptConfMcInfo->m_tSpyMt.IsNull())
    {
        tInfo.m_atViewInfo[byViewIndex].m_byMtCount =  0;
    }
    else
    {
        tInfo.m_atViewInfo[byViewIndex].m_byMtCount =  1;
        tInfo.m_atViewInfo[byViewIndex].m_atMts[0] = ptConfMcInfo->m_tSpyMt;
    }
    
    tInfo.m_atViewInfo[byViewIndex].m_byVSchemeCount = 1;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_byMediaType = tCapSupport.GetMainVideoType();
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMaxRate = m_tConf.GetBitRate();
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMinRate = tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMaxRate;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_bCanUpdateRate = TRUE;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_nFrameRate = 30;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_nVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;

    // guzh [7/19/2007] �ش���ͼ�Ƿ���ѯ�л�״̬
    tInfo.m_atViewInfo[byViewIndex].m_byAutoSwitchStatus = GETBBYTE( m_tConfInStatus.IsPollSwitch() );
    if (m_tConfInStatus.IsPollSwitch())
    {
        tInfo.m_atViewInfo[byViewIndex].m_nAutoSwitchTime = m_tPollSwitchParam.GetTimeSpan();
    }
    else
    {
        tInfo.m_atViewInfo[byViewIndex].m_nAutoSwitchTime = 0;
    }
    
    byViewIndex++;
    
    //������
    tInfo.m_atViewInfo[byViewIndex].m_nViewId			= m_dwSpeakerViewId;
    tInfo.m_atViewInfo[byViewIndex].m_byRes				=  VIDEO_FORMAT_CIF;//Ŀǰ�汾ֻ֧��h263-cif
    tInfo.m_atViewInfo[byViewIndex].m_byCurGeoIndex		= 0;
    tInfo.m_atViewInfo[byViewIndex].m_byGeoCount		= 1;
    tInfo.m_atViewInfo[byViewIndex].m_abyGeoInfo[0]		= 1;
    if(m_tConf.GetSpeaker().IsNull())
    {
        tInfo.m_atViewInfo[byViewIndex].m_byMtCount =  0;
    }
    else
    {
        tInfo.m_atViewInfo[byViewIndex].m_byMtCount =  1;
        tInfo.m_atViewInfo[byViewIndex].m_atMts[0] = m_tConf.GetSpeaker();
        
        MMcuLog( "[ProcMcuMcuVideoInfoReq] Speaker byViewIndex.%d McuId.%d MtId.%d\n", 
            byViewIndex, tInfo.m_atViewInfo[byViewIndex].m_atMts[0].GetMcuId(), 
            tInfo.m_atViewInfo[byViewIndex].m_atMts[0].GetMtId() );
    }
    tInfo.m_atViewInfo[byViewIndex].m_byVSchemeCount = 1;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_byMediaType = tCapSupport.GetMainVideoType();
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMaxRate = m_tConf.GetBitRate();
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMinRate = tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMaxRate;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_bCanUpdateRate = TRUE;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_nFrameRate = 30;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_nVideoSchemeID = m_dwSpeakerVideoId;
    byViewIndex++;
    //��Ƶ����Դ
    if(m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE)
    {	        
        TVMPParam tVMPParam = m_tConf.GetStatus().GetVmpParam();
        tInfo.m_atViewInfo[byViewIndex].m_nViewId			= m_dwVmpViewId;
        tInfo.m_atViewInfo[byViewIndex].m_byRes			=  VIDEO_FORMAT_CIF;//Ŀǰ�汾ֻ֧��h263-cif
        tInfo.m_atViewInfo[byViewIndex].m_byCurGeoIndex	= 0;
        tInfo.m_atViewInfo[byViewIndex].m_byGeoCount		= 1;
        tInfo.m_atViewInfo[byViewIndex].m_abyGeoInfo[0]	= tVMPParam.GetMaxMemberNum();
        tInfo.m_atViewInfo[byViewIndex].m_byMtCount =  0;
		// xliang [2/26/2009] FIXME: �����ݲ���MAXNUM_SUBFRAMEINGEO�������б�����Ϊ16���ĳ�20��ѵײ����
        for(s32 nIndex=0; nIndex<MAXNUM_MPUSVMP_MEMBER&&nIndex<MAXNUM_SUBFRAMEINGEO; nIndex++)
        {
            if(!tVMPParam.GetVmpMember(nIndex)->IsNull())
            {
                tInfo.m_atViewInfo[byViewIndex].m_atMts[tInfo.m_atViewInfo[byViewIndex].m_byMtCount].SetMt(*((TMt *)(tVMPParam.GetVmpMember(nIndex))));
                tInfo.m_atViewInfo[byViewIndex].m_byMtCount++;
            }
        }
        tInfo.m_atViewInfo[byViewIndex].m_byVSchemeCount = 1;
        tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_byMediaType = tCapSupport.GetMainVideoType();
        tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMaxRate = m_tConf.GetBitRate();
        tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMinRate = tInfo.m_atViewInfo[0].m_atVSchemes[0].m_dwMaxRate;
        tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_bCanUpdateRate = TRUE;
        tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_nFrameRate = 30;
        tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_nVideoSchemeID = m_dwVmpVideoId;
        byViewIndex++;
    }
    
    MMcuLog( "[ProcMcuMcuVideoInfoReq] byViewCount.%d\n", byViewIndex );
    
	tInfo.m_byViewCount = byViewIndex; 
}


/*====================================================================
    ������      ��ProcMcuMcuVideoInfoReq
    ����        ���Է�mcu������Ƶ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/24    3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuVideoInfoReq(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt  tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	
	STATECHECK;	

	TCConfViewInfo tInfo;
    GetConfViewInfo( tMt.GetMtId(), tInfo );

	cServMsg.SetMsgBody((u8 *)&tInfo, sizeof(tInfo));
	SendMsgToMt( tMt.GetMtId(), MCU_MCU_VIDEOINFO_ACK,  cServMsg);
}

/*====================================================================
    ������      ��ProcMcuMcuVideoInfoack
    ����        ������Է�mcu��Ƶ��Ϣ�ɹ�Ӧ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/25   3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuVideoInfoAck(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt  tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	
	STATECHECK;	
	
	TCConfViewInfo*	ptConfViewInfo = (TCConfViewInfo *)(cServMsg.GetMsgBody());

	TConfMcInfo*	ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMtId()); 
	if(ptConfMcInfo == NULL)
	{
		return;
	}
	memcpy(&(ptConfMcInfo->m_tConfViewInfo), ptConfViewInfo, sizeof(TCConfViewInfo));

	return;
}

/*====================================================================
    ������      ��ProcMcuMcuConfViewChgNtf
    ����        ��������ͼ�ı�֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/25   3.6          Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuConfViewChgNtf(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt  tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	
	STATECHECK;	
	
	TCConfViewChangeNtf*	ptNtf = (TCConfViewChangeNtf *)(cServMsg.GetMsgBody());

	TConfMcInfo*	ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMtId()); 
	if(ptConfMcInfo == NULL)
	{
		return;
	}
	for(s32 nLoop=0; nLoop<ptConfMcInfo->m_tConfViewInfo.m_byViewCount; nLoop++)
	{
		if(ptConfMcInfo->m_tConfViewInfo.m_atViewInfo[nLoop].m_nViewId != ptNtf->m_nViewID)
		{
			continue;
		}

		TCViewInfo *pInfo = &(ptConfMcInfo->m_tConfViewInfo.m_atViewInfo[nLoop]);
		for(s32 nIndex=0; nIndex<pInfo->m_byGeoCount; nIndex++)
		{
			if(pInfo->m_abyGeoInfo[nIndex] != ptNtf->m_bySubframeCount)
			{
				continue;
			}

			pInfo->m_byCurGeoIndex = nIndex;
			break;
		}
		break;
	}

}

/*====================================================================
    ������      ��ProcMcuMcuAudioInfoack
    ����        ������Է�mcu��Ƶ��Ϣ�ɹ�Ӧ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/25   3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuAudioInfoAck(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt  tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	
	STATECHECK;	
	
	TCConfAudioInfo*	ptConfAudioInfo = (TCConfAudioInfo *)(cServMsg.GetMsgBody());

	TConfMcInfo*	ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMtId()); 
	if(ptConfMcInfo == NULL)
	{
		return;
	}
	memcpy(&(ptConfMcInfo->m_tConfAudioInfo), ptConfAudioInfo, sizeof(TCConfAudioInfo));
}

/*====================================================================
    ������      ��GetLocalAudioInfo
    ����        ����ȡ��Ƶ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TCConfAudioInfo &tInfo [Out]
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/05/14    4.0         ����         ����
====================================================================*/
void CMcuVcInst::GetLocalAudioInfo(TCConfAudioInfo &tAudioInfo)
{
    tAudioInfo.m_byMixerCount = 0;
    tAudioInfo.m_byDefMixerIndex = 0;
    tAudioInfo.m_tMixerList[0].m_tSpeaker.SetNull();
    if(!m_tConf.GetSpeaker().IsNull())
    {
        tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_nMixerID = m_dwSpeakerAudioId;
        tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = m_tConf.GetSpeaker();
        if ( !m_tConf.GetSpeaker().IsLocal() )
        {
            u8 byMcuId = m_tConf.GetSpeaker().GetMcuId();
            /*            
            TConfMcInfo* ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(byMcuId);
            if (ptConfMcInfo != NULL)
            {
                TMt tRealSpeaker = ptConfMcInfo->m_tSpyMt;
                if (!tRealSpeaker.IsNull())
                {
                tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = tRealSpeaker;
                }  
                }
            */
            TMt tRealSpeaker = GetMcuMediaSrc(byMcuId);
            if (!tRealSpeaker.IsNull())
            {
                tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = tRealSpeaker;
            }  
        }
        tAudioInfo.m_byMixerCount++;
    }
    MMcuLog("[GetLocalAudioInfo] Local Speaker: (%d, %d)\n", 
        tAudioInfo.m_tMixerList[0].m_tSpeaker.GetMcuId(), 
        tAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId() );
    
    if(m_tConf.GetStatus().IsMixing())
    {
        tAudioInfo.m_byDefMixerIndex = tAudioInfo.m_byMixerCount;
        
        //zbq[11/01/2007] �����Ż�
        //TMixParam tParam = m_tConf.GetStatus().GetMixParam();
        //tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = tParam.m_atMtMember[0];
        tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_nMixerID = m_dwMixerAudioId;
        tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = GetLocalSpeaker();
        tAudioInfo.m_byMixerCount++;
        
        MMcuLog("[GetLocalAudioInfo] Local Mixer: (%d, %d)\n", 
            tAudioInfo.m_tMixerList[1].m_tSpeaker.GetMcuId(), 
            tAudioInfo.m_tMixerList[1].m_tSpeaker.GetMtId() );
    }
}

/*====================================================================
    ������      ��ProcMcuMcuAudioInfoReq
    ����        ���Է�mcu������Ƶ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/24    3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuAudioInfoReq(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt  tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	
	STATECHECK;	

	TCConfAudioInfo tAudioInfo; 
    GetLocalAudioInfo(tAudioInfo);

	cServMsg.SetMsgBody((u8 *)&tAudioInfo, sizeof(tAudioInfo));
	SendMsgToMt( tMt.GetMtId(), MCU_MCU_AUDIOINFO_ACK,  cServMsg);
}

/*====================================================================
������      ��NotifyAllSMcuMediaInfo
����        ������֪ͨ���������¼�MCU����ý��Դ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����byTargetMcuId     Ҫ֪ͨ���¼�MCU��Ϊ0����ȫ��
byMode            MODE_AUDIO, MODE_VIDEO, MODE_BOTH
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
07/05/14    4.0         ����         ����
====================================================================*/
void CMcuVcInst::NotifyAllSMcuMediaInfo(u8 byTargetMcuId, u8 byMode)
{
    CServMsg cServMsg;
    if ( MODE_AUDIO == byMode || MODE_BOTH == byMode )
    {
        TCConfAudioInfo tAudioInfo; 
        GetLocalAudioInfo(tAudioInfo);  
        cServMsg.SetEventId(MCU_MCU_AUDIOINFO_ACK);
        cServMsg.SetMsgBody((u8 *)&tAudioInfo, sizeof(tAudioInfo));
        
        u8 byMcuId;
        for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
        {
            byMcuId = m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId();
            if( byMcuId != 0 && m_tCascadeMMCU.GetMtId() != byMcuId &&
                (byTargetMcuId == 0 || byTargetMcuId == byMcuId ) )
            {
                SendMsgToMt( byMcuId, MCU_MCU_AUDIOINFO_ACK, cServMsg );
            }
        }
    }
    if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
    {
        cServMsg.SetEventId(MCU_MCU_VIDEOINFO_ACK);
        TCConfViewInfo tInfo;
        u8 byMcuId;
        for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
        {
            byMcuId = m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId();
            if( byMcuId != 0 && m_tCascadeMMCU.GetMtId() != byMcuId &&
                (byTargetMcuId == 0 || byTargetMcuId == byMcuId ) )
            {
                GetConfViewInfo(byMcuId, tInfo);
                cServMsg.SetMsgBody((u8 *)&tInfo, sizeof(tInfo));
                SendMsgToMt( byMcuId, MCU_MCU_VIDEOINFO_ACK, cServMsg );
            }
        }        
    }
}

/*====================================================================
������      ��NofityMMcuMediaInfo
����        ������֪ͨ�ϼ�MCU����ý��Դ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
07/05/15    4.0         ����         ����
====================================================================*/
void CMcuVcInst::NofityMMcuMediaInfo()
{
    if (m_tCascadeMMCU.IsNull())
    {
        return;
    }
    
    CServMsg cMsg;
    
    TCConfAudioInfo tAudioInfo;
    GetLocalAudioInfo(tAudioInfo);
    cMsg.SetEventId(MCU_MCU_AUDIOINFO_ACK);
    cMsg.SetMsgBody((u8 *)&tAudioInfo, sizeof(tAudioInfo));
    SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_AUDIOINFO_ACK, cMsg );
    
    TCConfViewInfo tVideoInfo;                
    GetConfViewInfo(m_tCascadeMMCU.GetMtId(), tVideoInfo);
    cMsg.SetEventId(MCU_MCU_VIDEOINFO_ACK);
    cMsg.SetMsgBody((u8 *)&tVideoInfo, sizeof(tVideoInfo));
    SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_VIDEOINFO_ACK, cMsg );
}

/*====================================================================
    ������      ��ProcMcuMcuMtListAck
    ����        ������Է�mcu�ն��б��Ӧ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/21    3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuMtListAck(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	u8			byIsLastPack = *(cServMsg.GetMsgBody());
	TMcuMcuMtInfo*	ptMcuMcuMtInfo = (TMcuMcuMtInfo*)(cServMsg.GetMsgBody()+sizeof(u8));
	s32			nMtNum = (cServMsg.GetMsgBodyLen()-sizeof(u8))/sizeof(TMcuMcuMtInfo);

	STATECHECK;

	TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->AddMcInfo(tMt.GetMtId());
	if(ptMcMtInfo == NULL)
	{
		return;
	}
	
	MMcuLog( "[ProcMcuMcuMtListAck] RemoteMcuID.%d LastMtNum.%d,This Pack include MtNum.%d\n", 
		     tMt.GetMtId(), ptMcMtInfo->m_byLastPos, nMtNum );

	
	//ȫ�µ�һ��
	if(ptMcMtInfo->m_byLastPos == 0)
	{
		//���ֳ�ʼ��
		memset(&(ptMcMtInfo->m_atMtExt), 0, sizeof(ptMcMtInfo->m_atMtExt));
		//memset(&(ptMcMtInfo->m_atMtStatus), 0, sizeof(ptMcMtInfo->m_atMtStatus));
		ptMcMtInfo->m_byMcuId = 0;
		memset(&(ptMcMtInfo->m_tConfAudioInfo), 0, sizeof(ptMcMtInfo->m_tConfAudioInfo));
		memset(&(ptMcMtInfo->m_tConfViewInfo), 0, sizeof(ptMcMtInfo->m_tConfViewInfo));
	}
	ptMcMtInfo->m_byMcuId = tMt.GetMtId();

	s32 nIndex = 0;
	s32 nLoop  = 0;
	for(nIndex = ptMcMtInfo->m_byLastPos, nLoop =0; nIndex<ptMcMtInfo->m_byLastPos+nMtNum&&nIndex<MAXNUM_CONF_MT; nIndex++, nLoop++)
	{
		ptMcMtInfo->m_atMtStatus[nIndex].SetAudioIn(ptMcuMcuMtInfo[nLoop].m_byAudioIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetAudioOut(ptMcuMcuMtInfo[nLoop].m_byAudioOut);
		ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio(!ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsAudioMuteIn));
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetAudioIn() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio(FALSE);
		}
		ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio(!ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsAudioMuteOut));
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetAudioOut() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio(FALSE);
		}
		ptMcMtInfo->m_atMtExt[nIndex].SetMt(ptMcuMcuMtInfo[nLoop].m_tMt);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMt(ptMcuMcuMtInfo[nLoop].m_tMt);
		ptMcMtInfo->m_atMtExt[nIndex].SetAlias(ptMcuMcuMtInfo[nLoop].m_szMtName);
		ptMcMtInfo->m_atMtExt[nIndex].SetIPAddr(ntohl(ptMcuMcuMtInfo[nLoop].m_dwMtIp));
		ptMcMtInfo->m_atMtExt[nIndex].SetMtType(ptMcuMcuMtInfo[nLoop].m_byMtType);
		ptMcMtInfo->m_atMtExt[nIndex].SetManuId(ptMcuMcuMtInfo[nLoop].m_byManuId);
        
        if(MT_MANU_KDC == ptMcuMcuMtInfo[nLoop].m_byManuId || MT_MANU_KDCMCU == ptMcuMcuMtInfo[nLoop].m_byManuId )
        {
            ptMcMtInfo->m_atMtExt[nIndex].SetProtocolType(ptMcuMcuMtInfo[nLoop].m_byProtocolType); // ȡЭ������
        }

		ptMcMtInfo->m_atMtStatus[nIndex].SetMtType(ptMcuMcuMtInfo[nLoop].m_byMtType);
		ptMcMtInfo->m_atMtStatus[nIndex].SetVideoIn(ptMcuMcuMtInfo[nLoop].m_byVideoIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetVideoOut(ptMcuMcuMtInfo[nLoop].m_byVideoOut);
//		ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2In(ptMcuMcuMtInfo[nLoop].m_byVideo2In);
//		ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2Out(ptMcuMcuMtInfo[nLoop].m_byVideo2Out);
		ptMcMtInfo->m_atMtStatus[nIndex].SetSendVideo(!ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsVideoMuteIn));    
		ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveVideo(!ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsVideoMuteOut));
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetVideoIn() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetSendVideo(FALSE);    
		}
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetVideoOut() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveVideo(FALSE);
		}

		TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMtId());
		tMtInfo.RemoveMt(ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId());
		if(ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsConnected))
		{
			tMtInfo.AddJoinedMt(ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId());
		}
		else
		{
			tMtInfo.AddMt(ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId());
		}
		m_tConfAllMtInfo.SetMtInfo(tMtInfo);
        ptMcMtInfo->m_atMtStatus[nIndex].SetIsEnableFECC(ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsFECCEnable));
		ptMcMtInfo->m_atMtStatus[nIndex].SetMtVideoInfo(ptMcuMcuMtInfo[nLoop].m_tPartVideoInfo);  
	}
	if(ISTRUE(byIsLastPack))
	{
        // ���� [6/1/2006] ������������
        if (nIndex < MAXNUM_CONF_MT)
        {
            memset( ptMcMtInfo->m_atMtStatus+nIndex, 0, sizeof(TMcMtStatus) * (MAXNUM_CONF_MT - nIndex) );
        }
        
		ptMcMtInfo->m_byLastPos = 0;

		cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
		SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

		//��������ն˱�
		SendMtListToMcs(tMt.GetMtId());

        // ���� [6/1/2006] �����¼�Mcu�ն�״̬�����
	    CServMsg cServBackMsg;
	    cServBackMsg.SetNoSrc();
        cServBackMsg.SetMsgBody();
		for(nIndex = 0; nIndex < MAXNUM_CONF_MT; nIndex++)
		{
			if( ptMcMtInfo->m_atMtStatus[nIndex].IsNull() || 
				ptMcMtInfo->m_atMtStatus[nIndex].GetMtId() == 0 ) //�Լ�
			{
				continue;
			}
            TMtStatus tMtStatus = ptMcMtInfo->m_atMtStatus[nIndex].GetMtStatus();
            cServBackMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );	
        }
        if (cServBackMsg.GetMsgBodyLen() != 0)
        {
            SendMsgToAllMcs( MCU_MCS_MTSTATUS_NOTIF, cServBackMsg );       
        }        
	}
	else
	{
		ptMcMtInfo->m_byLastPos += nMtNum;
	}
	
	return;
}

/*====================================================================
    ������      ��ProcMcuMcuNewMtNotify
    ����        ������Է�mcu���ն�֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/21    3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuNewMtNotify(  const CMessage * pcMsg )
{
	CServMsg	 cServMsg( pcMsg->content, pcMsg->length );
	TMt			 tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	TMcuMcuMtInfo tMcuMcuMtInfo;
	memcpy(&tMcuMcuMtInfo, cServMsg.GetMsgBody(), sizeof(tMcuMcuMtInfo));

	STATECHECK;

	//���ӵ��ն˱���
	TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMtId());
    if(tConfMtInfo.IsNull())
	{
		return;
	}
	tConfMtInfo.AddJoinedMt(tMcuMcuMtInfo.m_tMt.GetMtId());
	m_tConfAllMtInfo.SetMtInfo(tConfMtInfo);

	//������ػ��������ն���Ϣ
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
	
	//����mc table
	TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->AddMcInfo(tMt.GetMtId());
	if(ptMcMtInfo == NULL)
	{
		return;
	}
	
	s32 nIndex = 0; 
	s32 nIdle = MAXNUM_CONF_MT;
	for(; nIndex<MAXNUM_CONF_MT; nIndex++)
	{
		if(ptMcMtInfo->m_atMtStatus[nIndex].IsNull() && nIdle == MAXNUM_CONF_MT)
		{
            // �ҵ���һ�������
			nIdle = nIndex;
		}
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetMtId() == tMcuMcuMtInfo.m_tMt.GetMtId())
		{
            // �ҵ�ԭ����ƥ��
			break;
		}
	}

	if(nIndex >= MAXNUM_CONF_MT)
	{
		if(nIdle >= MAXNUM_CONF_MT)
		{
			//no idle pos;
			return;
		}
		else
		{
			nIndex = nIdle;
		}
	}

	ptMcMtInfo->m_atMtStatus[nIndex].SetAudioIn(tMcuMcuMtInfo.m_byAudioIn);
	ptMcMtInfo->m_atMtStatus[nIndex].SetAudioOut(tMcuMcuMtInfo.m_byAudioOut);
	ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio(!ISTRUE(tMcuMcuMtInfo.m_byIsAudioMuteIn));
	ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio(!ISTRUE(tMcuMcuMtInfo.m_byIsAudioMuteOut));
	ptMcMtInfo->m_atMtExt[nIndex].SetMt(tMcuMcuMtInfo.m_tMt);
	ptMcMtInfo->m_atMtStatus[nIndex].SetMt(tMcuMcuMtInfo.m_tMt);
	ptMcMtInfo->m_atMtExt[nIndex].SetAlias(tMcuMcuMtInfo.m_szMtName);
	ptMcMtInfo->m_atMtExt[nIndex].SetIPAddr(ntohl(tMcuMcuMtInfo.m_dwMtIp));
	ptMcMtInfo->m_atMtExt[nIndex].SetMtType(tMcuMcuMtInfo.m_byMtType);
	ptMcMtInfo->m_atMtExt[nIndex].SetManuId(tMcuMcuMtInfo.m_byManuId);
	ptMcMtInfo->m_atMtStatus[nIndex].SetMtType(tMcuMcuMtInfo.m_byMtType);
	ptMcMtInfo->m_atMtStatus[nIndex].SetVideoIn(tMcuMcuMtInfo.m_byVideoIn);
	ptMcMtInfo->m_atMtStatus[nIndex].SetVideoOut(tMcuMcuMtInfo.m_byVideoOut);
//	ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2In(tMcuMcuMtInfo.m_byVideo2In);
//	ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2Out(tMcuMcuMtInfo.m_byVideo2Out);
	ptMcMtInfo->m_atMtStatus[nIndex].SetSendVideo(!ISTRUE(tMcuMcuMtInfo.m_byIsVideoMuteIn));    
	ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveVideo(!ISTRUE(tMcuMcuMtInfo.m_byIsVideoMuteOut));
    TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMtId());
	tMtInfo.RemoveMt(tMcuMcuMtInfo.m_tMt.GetMtId());
	if(ISTRUE(tMcuMcuMtInfo.m_byIsConnected))
	{
		tMtInfo.AddJoinedMt(tMcuMcuMtInfo.m_tMt.GetMtId());
	}
	else
	{
		tMtInfo.AddMt(tMcuMcuMtInfo.m_tMt.GetMtId());
	}
	m_tConfAllMtInfo.SetMtInfo(tMtInfo);
    ptMcMtInfo->m_atMtStatus[nIndex].SetIsEnableFECC(ISTRUE(tMcuMcuMtInfo.m_byIsFECCEnable));
	ptMcMtInfo->m_atMtStatus[nIndex].SetMtVideoInfo(tMcuMcuMtInfo.m_tPartVideoInfo);  

	
	MMcuLog( "[ProcMcuMcuNewMtNotify] nIndex.%d MtId.%d szMtName.%s bOnLine.%d\n", 
		     nIndex, tMcuMcuMtInfo.m_tMt.GetMtId(), tMcuMcuMtInfo.m_szMtName, tMcuMcuMtInfo.m_byIsConnected );

	SendMtListToMcs( tMt.GetMtId() );

	// ����VCS���飬����Ҫ����ѡ�����ش�����
	if (VCS_CONF == m_tConf.GetConfSource())
	{
		TMt tNewMt = tMcuMcuMtInfo.m_tMt;
		MtOnlineChange(tNewMt, TRUE, 0);

		if (m_cVCSConfStatus.GetCurVCMT() == tNewMt)
		{
			// �����¼�mcuԭ�ش�ͨ���еĳ�Ա�Ҷ�
			TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tNewMt.GetMcuId());
			if (ptMcInfo != NULL && !(ptMcInfo->m_tMMcuViewMt.IsNull()) &&
				m_tConfAllMtInfo.MtJoinedConf(ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId()) &&
				!(ptMcInfo->m_tMMcuViewMt == tNewMt))
			{
				MMcuLog("[ProcMcuMcuNewMtNotify]drop mt(mcuid:%d, mtid:%d) because of new submt(mcuid:%d, mtid:%d) online\n",
					    ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId(), tNewMt.GetMcuId(), tNewMt.GetMtId());
				VCSDropMT(ptMcInfo->m_tMMcuViewMt);	
			}

			OnMMcuSetIn(tNewMt, m_cVCSConfStatus.GetCurSrcSsnId(), SWITCH_MODE_SELECT);
		}

		if (!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
		{
			GoOnSelStep(tMt, MODE_VIDEO, TRUE);
			GoOnSelStep(tMt, MODE_AUDIO, TRUE);
			GoOnSelStep(tNewMt, MODE_VIDEO, FALSE);
			GoOnSelStep(tNewMt, MODE_AUDIO, FALSE);
		}

	    //�йػ���ϳɵĴ���
        if(VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode())
        {
            ChangeVmpStyle(tMt, TRUE);
        }
	}
}

/*=============================================================================
    �� �� ���� ProcMcuMcuCallAlertMtNotify
    ��    �ܣ� mcu<->mcu�����ն˺���֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/5/24   3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuCallAlertMtNotify(  const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt      tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	TMt tAlertMt = *( (TMt*)(cServMsg.GetMsgBody()) );
	TMtAlias tAlertMtAlias = *( (TMtAlias*)(cServMsg.GetMsgBody()+sizeof(TMt)) );

	STATECHECK;

	//���ӵ��ն˱���
	TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMtId());
    if(tConfMtInfo.IsNull())
	{
		return;
	}

	tConfMtInfo.AddMt( tAlertMt.GetMtId() );
	m_tConfAllMtInfo.SetMtInfo( tConfMtInfo );
	
	//����mc table
	TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->AddMcInfo(tMt.GetMtId());
	if( NULL == ptMcMtInfo )
	{
		return;
	}
	
	s32 nIndex = 0;
	s32 nIdle = MAXNUM_CONF_MT;
	for(; nIndex<MAXNUM_CONF_MT; nIndex++)
	{
		if(ptMcMtInfo->m_atMtStatus[nIndex].IsNull() && nIdle == MAXNUM_CONF_MT)
		{
            // �ҵ���һ�������
			nIdle = nIndex;
		}
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetMtId() == tAlertMt.GetMtId())
		{
			//���У������ظ�����
			return;
			//break;
		}
	}

	if(nIndex >= MAXNUM_CONF_MT)
	{
		if(nIdle >= MAXNUM_CONF_MT)
		{
			//no idle pos;
			return;
		}
		else
		{
			nIndex = nIdle;
		}
	}

	//������ػ��������ն���Ϣ
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	ptMcMtInfo->m_atMtStatus[nIndex].SetAudioIn(MEDIA_TYPE_NULL);
	ptMcMtInfo->m_atMtStatus[nIndex].SetAudioOut(MEDIA_TYPE_NULL);
	ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio( FALSE );
	ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio( FALSE );

	ptMcMtInfo->m_atMtExt[nIndex].SetMt(tAlertMt);
	ptMcMtInfo->m_atMtStatus[nIndex].SetMt(tAlertMt);

	if( mtAliasTypeTransportAddress == tAlertMtAlias.m_AliasType )
	{
		ptMcMtInfo->m_atMtExt[nIndex].SetIPAddr(tAlertMtAlias.m_tTransportAddr.GetIpAddr());
		u32  dwDialIP  = tAlertMtAlias.m_tTransportAddr.GetIpAddr();
		ptMcMtInfo->m_atMtExt[nIndex].SetAlias(StrOfIP(dwDialIP));
	}
	else
	{
		ptMcMtInfo->m_atMtExt[nIndex].SetAlias(tAlertMtAlias.m_achAlias);
	}

	ptMcMtInfo->m_atMtExt[nIndex].SetMtType(MT_TYPE_MT);
	//ptMcMtInfo->m_atMtExt[nIndex].SetManuId(m_byManuId);
	ptMcMtInfo->m_atMtStatus[nIndex].SetMtType(MT_TYPE_MT);

	ptMcMtInfo->m_atMtStatus[nIndex].SetVideoIn(MEDIA_TYPE_NULL);
	ptMcMtInfo->m_atMtStatus[nIndex].SetVideoOut(MEDIA_TYPE_NULL);
//	ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2In(MEDIA_TYPE_NULL);
//	ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2Out(MEDIA_TYPE_NULL);
	ptMcMtInfo->m_atMtStatus[nIndex].SetSendVideo( FALSE );    
	ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveVideo( FALSE );
    TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMtId());
	tMtInfo.RemoveMt( tAlertMt.GetMtId() );
	tMtInfo.AddMt( tAlertMt.GetMtId() );
	m_tConfAllMtInfo.SetMtInfo(tMtInfo);
    ptMcMtInfo->m_atMtStatus[nIndex].SetIsEnableFECC( FALSE );

	//ptMcMtInfo->m_atMtStatus[nIndex].m_tPartVideoInfo =  tMcuMcuMtInfo.m_tPartVideoInfo;  
	
	MMcuLog( "[ProcMcuMcuCallAlertMtNotify] nIndex.%d MtId.%d\n", nIndex, tAlertMt.GetMtId() );

	SendMtListToMcs( tMt.GetMtId() );

	return;
}

/*====================================================================
    ������      ��ProcMcuMcuDropMtNotify
    ����        ��mcu<->mcu�Ҷ��ն�֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/21    3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuDropMtNotify(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	TMt*		ptDropMt = (TMt* )(cServMsg.GetMsgBody());

	STATECHECK;

	//�����ն���Ϊ��ֱ���ն��Ǳ��ط����ˣ���ձ��ط��������
	if ( *ptDropMt == m_tConf.GetSpeaker() )
	{
		ChangeSpeaker( NULL );
	}
	
	//���ӵ��ն˱���
	TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(ptDropMt->GetMcuId());
    if(tConfMtInfo.IsNull())
	{
		return;
	}

	tConfMtInfo.RemoveJoinedMt(ptDropMt->GetMtId());
	m_tConfAllMtInfo.SetMtInfo(tConfMtInfo);
    
    // guzh [3/9/2007] ֪ͨMCS
    MtOnlineChange(*ptDropMt, FALSE, MTLEFT_REASON_NORMAL);

	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

    if ( *ptDropMt == (TMt)(m_tConf.m_tStatus.GetMtPollParam()) )
    {
        SetTimer( MCUVC_POLLING_CHANGE_TIMER, 10 );
    }

	//�йػ���ϳɵĴ���
	if (VCS_CONF == m_tConf.GetConfSource() && 
		VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode())
	{
		ChangeVmpStyle(tMt, FALSE);

	}
}

/*====================================================================
    ������      ��ProcMcuMcuDelMtNotify
    ����        ��mcu<->mcuɾ���ն�֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/21    3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuDelMtNotify(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8			byMtId = cServMsg.GetSrcMtId();
	TMt			tDelMt = *(TMt* )(cServMsg.GetMsgBody());

	STATECHECK;

	//���ӵ��ն˱���
	TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(tDelMt.GetMcuId());
    if(tConfMtInfo.IsNull())
	{
		return;
	}
    
	if(tConfMtInfo.MtInConf(tDelMt.GetMtId()))
	{
		tConfMtInfo.RemoveMt(tDelMt.GetMtId());
		//SendMsgToMt(byMtId, MCU_MCU_DELMT_NOTIF,cServMsg); 
	}
	m_tConfAllMtInfo.SetMtInfo(tConfMtInfo);
    
	//������ػ��������ն���Ϣ
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//update mtlist
	if(!tDelMt.IsLocal())
	{
		BOOL32 bFind = FALSE;
		TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(tDelMt.GetMcuId());
		if(ptInfo != NULL)
		{
			for(s32 nLoop=0; nLoop<MAXNUM_CONF_MT; nLoop++)
			{
				if(ptInfo->m_atMtExt[nLoop].GetMtId() != tDelMt.GetMtId())
				{
					continue;
				}
				ptInfo->m_atMtExt[nLoop].SetNull();
				ptInfo->m_atMtStatus[nLoop].SetNull();
				bFind = TRUE;
			}
		}
		if(bFind)
		{
			SendMtListToMcs(tDelMt.GetMcuId());
		}

        // guzh [4/4/2007] ����ѯ��Ӱ�촦��
        u8 byIdx;
        if ( m_tConfPollParam.IsExistMt(tDelMt, byIdx) )
        {
            m_tConfPollParam.RemoveMtFromList(tDelMt);
			if ( tDelMt == (TMt)(m_tConf.m_tStatus.GetMtPollParam()) )
			{
				m_tConfPollParam.SpecPollPos(  byIdx );
				SetTimer( MCUVC_POLLING_CHANGE_TIMER, 10 );
			}
        }
	}
}

/*====================================================================
    ������      ��SendMtListToMcs
    ����        �������ն��б�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/02/25    3.6         Jason         ����
====================================================================*/
void CMcuVcInst::SendMtListToMcs(u8 byMcuId)
{
	//��������ն˱�
	TMcu tMcu;
	tMcu.SetMcu( byMcuId );
	CServMsg cServMsg;
	if(byMcuId == LOCAL_MCUID)
	{
		cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(tMcu) );
		cServMsg.CatMsgBody( ( u8 * )m_ptMtTable->m_atMtExt, 
		                  	 m_ptMtTable->m_byMaxNumInUse * sizeof( TMtExt ) );
	}
	else
	{
		TConfMcInfo* ptMcMtInfo = m_ptConfOtherMcTable->GetMcInfo(byMcuId);	
		if(ptMcMtInfo == NULL)
		{
			return;
		}
        // guzh [4/30/2007] �ϼ�MCU�б����
        if ( !g_cMcuVcApp.IsShowMMcuMtList() && 
            !m_tCascadeMMCU.IsNull() && m_tCascadeMMCU.GetMtId() == tMcu.GetMcuId() )
        {
            return;
        }

		cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(tMcu) );
		for(s32 nLoop =0; nLoop<MAXNUM_CONF_MT; nLoop++)
		{
			if( ptMcMtInfo->m_atMtStatus[nLoop].IsNull() || 
				0 == ptMcMtInfo->m_atMtStatus[nLoop].GetMtId() ) //�Լ�
			{
				continue;
			}
			cServMsg.CatMsgBody((u8 *)&(ptMcMtInfo->m_atMtExt[nLoop]), sizeof(TMtExt));
		}
	}
	SendMsgToAllMcs( MCU_MCS_MTLIST_NOTIF, cServMsg );
}

/*====================================================================
    ������      ��ProcMcuMcuSetMtChanReq
    ����        ��mcu<->mcu����ý��ͨ��״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/26    3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuSetMtChanReq(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 byMtId	= cServMsg.GetSrcMtId() ;
	TMtMediaChanStatus*		ptMtChanStatus = (TMtMediaChanStatus* )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));

	STATECHECK;
    
	//Ŀǰ��֧��
	SendMsgToMt(byMtId, cServMsg.GetEventId()+2, cServMsg);
}

/*====================================================================
    ������      ��ProcMcuMcuSetMtChanNotify
    ����        ��mcu<->mcu����ý��ͨ��״̬֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/27    3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuSetMtChanNotify(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	TMtMediaChanStatus*	ptMtChanStatus = (TMtMediaChanStatus* )(cServMsg.GetMsgBody());

	STATECHECK;

	TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(ptMtChanStatus->m_tMt.GetMcuId());
	if(ptInfo == NULL)
	{
		return;
	}
	for(s32 nLoop=0; nLoop<MAXNUM_CONF_MT; nLoop++)
	{
		if(ptInfo->m_atMtStatus[nLoop].GetMtId() != ptMtChanStatus->m_tMt.GetMtId())
		{
			continue;
		}
		TMcMtStatus *ptStatus = &(ptInfo->m_atMtStatus[nLoop]);
        // guzh [6/27/2007] ����û����ȫ��Ӧ���ֶΣ���ʱͨ���Ƿ����շ�״̬����
        // ͬ��Ҳ��������Ϊý��ͨ������Ϊ MEDIA_TYPE_NULL
        // ��ȻĿǰKedaMcu��֧�ָ�֪ͨ
		if(ptMtChanStatus->m_byMediaMode == MODE_VIDEO)
		{
			if(ISTRUE(ptMtChanStatus->m_byIsDirectionIn))
			{                
                ptStatus->SetSendVideo(!ptMtChanStatus->m_bMute);
				//ptStatus->SetVideoIn(GETBBYTE(!(ptMtChanStatus->m_bMute)));
			}
			else
			{
                ptStatus->SetReceiveVideo(!ptMtChanStatus->m_bMute);
				//ptStatus->SetVideoOut(GETBBYTE(!(ptMtChanStatus->m_bMute)));
			}
		}
		else if(ptMtChanStatus->m_byMediaMode == MODE_AUDIO)
		{
			if(ISTRUE(ptMtChanStatus->m_byIsDirectionIn))
			{
                ptStatus->SetSendAudio(!(ptMtChanStatus->m_bMute));
				//ptStatus->SetAudioIn(GETBBYTE(!(ptMtChanStatus->m_bMute)));
			}
			else
			{
                ptStatus->SetReceiveAudio(!(ptMtChanStatus->m_bMute));
				//ptStatus->SetAudioOut(GETBBYTE(!(ptMtChanStatus->m_bMute)));
			}
		}
		else
		{
			//��֧�����ݻ���
		}
			
	}
}

/*====================================================================
    ������      ��ProcMcsMcuSpecOutViewReq
    ����        ��mcs<->mcu ָ���ն˻ش����ϼ�MCU����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/05/09    4.0         ����         ����
====================================================================*/
void CMcuVcInst::OnSetOutView( TMt &tSetInMt )
{
    if ( m_tCascadeMMCU.IsNull() )
    {
        return;
    }

    if ( !tSetInMt.IsLocal() )
    {
        // ����֪ͨ�¼�MCU
        OnMMcuSetIn( tSetInMt, 0, SWITCH_MODE_BROADCAST );
    }
    
    TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
    if (ptConfMcInfo->m_tSpyMt == tSetInMt)
    {
        // ��ͬ��ֱ�ӷ���
        return ;
    }
    
    tSetInMt = GetLocalMtFromOtherMcuMt(tSetInMt);
    
    TMt tMMcu;
    tMMcu.SetMcuId(m_tCascadeMMCU.GetMtId());
    tMMcu.SetMtId(0);
    OnMMcuSetIn(tMMcu, 0, SWITCH_MODE_BROADCAST, TRUE); //���л������ˣ���ʱ���ϼ���setin��Ϣ�����ã�����    
    
    ptConfMcInfo->m_tSpyMt = tSetInMt;
    
    //send output notify
    CServMsg cMsg;
    TSetOutParam tOutParam;
    tOutParam.m_nMtCount = 1;
    tOutParam.m_atConfViewOutInfo[0].m_tMt = tSetInMt;
    tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
    tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;
    cMsg.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
    cMsg.SetEventId(MCU_MCU_SETOUT_NOTIF);
    SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_SETOUT_NOTIF, cMsg);
    
    NotifyMtSend(tSetInMt.GetMtId(), MODE_BOTH);
    
    //��������
    u8 byMode = MODE_BOTH;
    if(m_tConf.m_tStatus.IsMixing())
    {
        byMode = MODE_VIDEO;
    }
    
    StartSwitchToMcu(tSetInMt, 0, m_tCascadeMMCU.GetMtId(), byMode, SWITCH_MODE_SELECT);
    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
        //��Ŀ����ƵRtcp������Դ
        TLogicalChannel tLogicalChannel;
        m_ptMtTable->GetMtLogicChnnl(tSetInMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);
        
        u32 dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
        u16 wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
        // guzh [7/25/2007] �ش�ͨ��ҲҪMap
        //�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�        
        SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tCascadeMMCU, 0, MODE_VIDEO, SWITCH_MODE_SELECT, TRUE);
        
        //��Ŀ����ƵRtcp������Դ
        m_ptMtTable->GetMtLogicChnnl(tSetInMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE);
        
        dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
        wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
        SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tCascadeMMCU, 0, MODE_AUDIO, SWITCH_MODE_SELECT, TRUE);
    }   
    
    // guzh [5/9/2007] ֪ͨ�ϼ��µ���Ƶ/��ƵԴ
    // NofityMMcuMediaInfo();
}

/*====================================================================
    ������      ��ProcMcsMcuSpecOutViewReq
    ����        ��mcs<->mcu ָ���ն˻ش����ϼ�MCU����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/05/09    4.0         ����         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuSpecOutViewReq(  const CMessage * pcMsg )
{
    STATECHECK;

    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    TMt tSetInMt = *(TMt* )(cServMsg.GetMsgBody());
    
    if ( m_tCascadeMMCU.IsNull() )
    {
        //NACK
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }

    TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
    if(ptConfMcInfo == NULL)
    {
        //NACK
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }    
    
    //ACK
    SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

    // ָ���ش�
    OnSetOutView(tSetInMt);
}

/*====================================================================
    ������      ��ProcMcuMcuSetInReq
    ����        ��mcu<->mcu�����ն����뵽ĳ����ͼ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/27    3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuSetInReq(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 byMtId = cServMsg.GetSrcMtId();
	TSetInParam* ptSetInParam = (TSetInParam* )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));

	STATECHECK;

	if( !ptSetInParam->m_tMt.IsLocal() ||
		ptSetInParam->m_tMt.GetMtId() == byMtId )
	{
		//ACK
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
	}
	
	ptSetInParam->m_tMt = m_ptMtTable->GetMt(ptSetInParam->m_tMt.GetMtId());
	TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(byMtId);
	if(ptConfMcInfo == NULL)
	{
		//NACK
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	TMt tMt;
	tMt.SetMcuId(byMtId);
	tMt.SetMtId(0);
	OnMMcuSetIn(tMt, 0, SWITCH_MODE_BROADCAST, TRUE); //���л������ˣ���ʱ���ϼ���setin��Ϣ�����ã�����
	
	
	//send output notify
	CServMsg cMsg;
	TSetOutParam tOutParam;
	tOutParam.m_nMtCount = 1;
	tOutParam.m_atConfViewOutInfo[0].m_tMt = m_ptMtTable->GetMt(byMtId);
	tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
	tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;
	cMsg.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
	cMsg.SetEventId(MCU_MCU_SETOUT_NOTIF);
	SendMsgToMt(byMtId, MCU_MCU_SETOUT_NOTIF, cMsg);


	ptConfMcInfo->m_tSpyMt = ptSetInParam->m_tMt;
	NotifyMtSend( ptSetInParam->m_tMt.GetMtId(), MODE_BOTH);
	TMt tDstMt = m_ptMtTable->GetMt(byMtId);
	TMt tSrcMt = ptSetInParam->m_tMt;
   
	//��������
	u8 byMode = MODE_BOTH;
	if(m_tConf.m_tStatus.IsMixing())
	{
		byMode = MODE_VIDEO;
	}

    StartSwitchToMcu(tSrcMt, 0, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT);
    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
        //��Ŀ����ƵRtcp������Դ
        TLogicalChannel tLogicalChannel;
        m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);

        u32 dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
        u16 wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
        // guzh [7/25/2007] �ش�ͨ��ҲҪMap
        //�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�        
        SwitchVideoRtcpToDst(dwDstIp, wDstPort, tDstMt, 0, MODE_VIDEO, SWITCH_MODE_SELECT, TRUE);

        //��Ŀ����ƵRtcp������Դ
        m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE);

        dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
        wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
        SwitchVideoRtcpToDst(dwDstIp, wDstPort, tDstMt, 0, MODE_AUDIO, SWITCH_MODE_SELECT, TRUE);
    }

	//ACK
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
    return;
}

/*====================================================================
    ������      ��ProcMcuMcuSetOutReq
    ����        ��mcu<->mcu����ĳ����ͼ���뵽�ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/27    3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuSetOutReq(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 byMtId = cServMsg.GetSrcMtId();
	TSetOutParam* ptSetOutParam = (TSetOutParam* )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));

	STATECHECK;

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

    return;
}

/*====================================================================
    ������      ��ProcMcuMcuSetOutNotify
    ����        ��mcu<->mcuĳ����ͼ���뵽�ն�֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/27    3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuSetOutNotify(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 byMtId = cServMsg.GetSrcMtId();
	TCConfViewOutInfo* ptSetOutParam = (TCConfViewOutInfo* )(cServMsg.GetMsgBody());

	STATECHECK;

	TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(byMtId);
	if(ptInfo == NULL)
	{
		return;
	}
	
	if(ptSetOutParam->m_tMt.IsNull())
	{
		return;
	}
	TMcMtStatus *ptMcMtStatus = ptInfo->GetMtStatus(ptSetOutParam->m_tMt);
	if(ptMcMtStatus ==  NULL)
	{
		return;
	}
    TMtVideoInfo tInfo = ptMcMtStatus->GetMtVideoInfo();
	tInfo.m_nOutputLID = ptSetOutParam->m_nOutViewID;
	tInfo.m_nOutVideoSchemeID = ptSetOutParam->m_nOutVideoSchemeID;
    ptMcMtStatus->SetMtVideoInfo(tInfo);

    return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuStartMixerCmd
    ��    �ܣ� ���� �ϼ�MCU�����¼�MCU������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/5/27   3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuStartMixerCmd( const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8 byMtNum = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMcu));
    if ( byMtNum == 0 || m_tConf.m_tStatus.IsNoMixing() )
    {
        ProcMixStart(cServMsg);
    }
    else
    {
        TMt *ptMt = (TMt*)(cServMsg.GetMsgBody() + sizeof(TMcu) + sizeof(u8));
        AddRemoveSpecMixMember(ptMt, byMtNum);
    }
	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuStartMixerNotif
    ��    �ܣ� ���� �¼�MCU�������������ϱ� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuStartMixerNotif( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	TMcu tMcu = *(TMcu*)cServMsg.GetMsgBody();

	STATECHECK;	
    
    // guzh [11/7/2007] 
	//cServMsg.SetSrcMtId(0);
	//cServMsg.SetEventId(MCU_MCS_STARTDISCUSS_NOTIF);
	//SendMsgToAllMcs(MCU_MCS_STARTDISCUSS_NOTIF,cServMsg);

	//������ʼ�ɹ�������һ�λ�������
	if (!tMcu.IsLocal())
	{
		OnGetMixParamToSMcu(&cServMsg);
	}
    
    return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuStopMixerCmd
    ��    �ܣ� ���� �ϼ�MCUֹͣ�¼�MCU������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/5/27   3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuStopMixerCmd( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	STATECHECK;	
    ProcMixStop(cServMsg);
	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuStopMixerNotif
    ��    �ܣ� ���� �¼�MCUֹͣ���������ϱ� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuStopMixerNotif( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
    
	TMcu *ptMcu = (TMcu *)(cServMsg.GetMsgBody());
	
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());
	if(ptMcInfo == NULL)
	{
		return;
	}
	
    // guzh [11/7/2007] 
	//cServMsg.SetSrcMtId(0);
	//cServMsg.SetEventId(MCU_MCS_STOPDISCUSS_NOTIF);
	//SendMsgToAllMcs(MCU_MCS_STOPDISCUSS_NOTIF,cServMsg); 

    // xsl [8/22/2006] �޳��ᵼ���¼����ƻ����ָ�ʱ���ָܻ���������
	//�޳��¼�mcu�ڱ����Ļ�����Աλ��
//	if( m_tConfAllMtInfo.MtJoinedConf( ptMcu->GetMcuId() ) && 
//		m_ptMtTable->IsMtInMixing( ptMcu->GetMcuId() )  )            
//	{
//		TMt tMcuMt = m_ptMtTable->GetMt(ptMcu->GetMcuId());
//		AddRemoveSpecMixMember( &tMcuMt, 1, FALSE );
//	}

    // xsl [7/28/2006] �����ն˻���״̬
    if (!m_ptConfOtherMcTable->ClearMtInMixing(*ptMcu))
    {
        ConfLog(FALSE, "[ProcMcuMcuStopMixerNotif] ClearMtInMixing failed!\n");
    }       

	return;
}

/*==============================================================================
������    : CascadeAdjMtRes
����      : �����ʼ�������ֱ�������
�㷨ʵ��  :  
����˵��  : TMt		tMt					[in]Ҫ����MT (����[4,1])
			u8		byNewFormat			[in]Ҫ�����ķֱ���
			BOOL	bStart				[in]Ϊ1�������Ϊ0��ָ���ԭʼ�ֱ��ʡ�Ĭ��ΪTRUE
			u8		byVmpStyle			[in]����ϳɷ��Ĭ��Ϊ255����������
			u8		byPos				[in]Mt�ڻ���ϳɵ�ͨ��λ�á�Ĭ��Ϊ255����������

����ֵ˵��: void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-4-23	4.6				Ѧ��							����
==============================================================================*/
void CMcuVcInst::CascadeAdjMtRes( TMt tMt, u8 byNewFormat, BOOL32 bStart, u8 byVmpStyle, u8 byPos)
{
	CServMsg cServMsg;
	cServMsg.SetEventId( MCU_MCU_ADJMTRES_REQ );
	u8 byStart = bStart ? 1: 0;
	cServMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );		//Ҫ������Mt
	cServMsg.CatMsgBody( &byStart, sizeof(u8) );		//�Ƿ��ǻָ�Res
	cServMsg.CatMsgBody( &byNewFormat, sizeof(u8) );	//Ҫ�������·ֱ���

	// ������2��¼�������Լ�����ȡҪ������RES
	if( byVmpStyle <= VMP_STYLE_TWENTY && byVmpStyle > 0
		&& byPos < MAXNUM_MPUSVMP_MEMBER )
	{
		cServMsg.CatMsgBody( &byVmpStyle, sizeof(u8) );		//VMP �ϳɷ��
		cServMsg.CatMsgBody( &byPos, sizeof(u8) );			//����VMPͨ��λ��
	}
	
	SendMsgToMt( tMt.GetMcuId(), cServMsg.GetEventId(), cServMsg);
}
/*=============================================================================
    �� �� ���� ProcMcuMcuAdjustMtResReq
    ��    �ܣ� ���� ������Mt�ֱ��� ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
	4/11/2009   4.6			Ѧ��					create
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAdjustMtResReq( const CMessage * pcMsg)
{	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	

	BOOL32 bRspNack	= FALSE;				//�Ƿ��nack
	BOOL32 bNeedAdjustRes = TRUE;			//Ĭ����Ҫ���ֱ���
	BOOL32 bGetResBySelf = FALSE;			//�Ƿ��Լ�����ȡҪ�����ķֱ���
	u8 byChnnlType = LOGCHL_VIDEO;			//Ŀǰֻ�ı��һ·��Ƶͨ���ķֱ���

	TMt tMt = *(TMt *)cServMsg.GetMsgBody();
	u8 byMtId = tMt.GetMtId();
	u8 byStart = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt));		//��ʼ��Res
	u8 byReqRes = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8)); 
	u8 byPos = ~0;
	BOOL32 bNoneKeda = ( MT_MANU_KDC != m_ptMtTable->GetManuId(byMtId)  
							&& MT_MANU_KDCMCU != m_ptMtTable->GetManuId(byMtId) );
	
	CallLog("[ProcMcuMcuAdjustMtResReq] begin adjust Mt.%u(bNoneKeda: %d) resolution!\n",byMtId, bNoneKeda);

	TLogicalChannel tLogicChannel;
	if ( !m_tConfAllMtInfo.MtJoinedConf( byMtId ) ||
		!m_ptMtTable->GetMtLogicChnnl( byMtId, byChnnlType, &tLogicChannel, FALSE ) )
	{
		CallLog("[ProcMcuMcuAdjustMtResReq]Backward LC has problems. Nack!!\n");
		bRspNack = TRUE;
	}
	
	if( bNoneKeda )	//�ǿƴ�Ļ�nack
	{
		CallLog("[ProcMcuMcuAdjustMtResReq]None Keda Mt. Nack!!\n");
		bRspNack = TRUE;
	}

	if(bRspNack)
	{
		SendReplyBack( cServMsg, MCU_MCU_ADJMTRES_NACK );
		return;
	}

	if( cServMsg.GetMsgBodyLen() > sizeof(TMt) + sizeof(u8) + sizeof(u8) )
	{
		bGetResBySelf = TRUE;
	}
	
	u8 byMtStandardFormat = tLogicChannel.GetVideoFormat(); // ԭʼ�ֱ���
	u8 byNewFormat;
	if( bGetResBySelf)
	{	
		byPos = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(u8) + sizeof(u8) );
		if( MEDIA_TYPE_H264 != tLogicChannel.GetMediaType() )
		{
			byReqRes = VIDEO_FORMAT_CIF;
		}
		else
		{
			u8 byVmpStyle =  *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(u8));
			
			bNeedAdjustRes = VidResAdjust( byVmpStyle, byPos, byMtStandardFormat, byReqRes);
		}
		
	}

	if( byStart == 0 )
	{
		byNewFormat = byMtStandardFormat;
	}
	else
	{
		byNewFormat = byReqRes;
	}

//	if( bNeedAdjustRes) //��������Res������ͬchangeMtVidFormat()��
	
	CServMsg cMsg;
	cMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
	cMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
	cMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
	SendMsgToMt( byMtId, cMsg.GetEventId(), cMsg );
	
	cServMsg.SetMsgBody();
	cServMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
	cServMsg.CatMsgBody( &byStart, sizeof(u8) );
	if( byPos < MAXNUM_MPUSVMP_MEMBER)
	{
		cServMsg.CatMsgBody( &byPos, sizeof(u8) );
	}
	SendReplyBack( cServMsg, MCU_MCU_ADJMTRES_ACK );

	CallLog("[ProcMcuMcuAdjustMtResReq] byNewFormat: %u, byStart: %u, byPos: %u \n", byNewFormat, byStart, byPos);

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuAdjustMtResAck
    ��    �ܣ� ���� ������Mt�ֱ��� Ӧ��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
	4/11/2009   4.6			Ѧ��					create
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAdjustMtResAck( const CMessage * pcMsg)
{	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	

	u8  byPos = ~0;
	
	TMt tMt = *(TMt *)cServMsg.GetMsgBody();	//�����õ�MT
	u8  byStart = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt));
	if( cServMsg.GetMsgBodyLen() > sizeof(TMt) + sizeof(u8) )
	{
		byPos = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8));
	}

	CallLog("[ProcMcuMcuAdjustMtResAck] Adjust Mt(%u,%u) res success!---bStart:%u \n",tMt.GetMcuId(), tMt.GetMtId(), byStart);

	if(byStart)
	{
		BOOL32 bSetChnnl = FALSE;
		u8 byVmpMemberType = VMP_MEMBERTYPE_VAC;
		if(byPos < MAXNUM_MPUSVMP_MEMBER)			//������VMPĳͨ���Ľ���
		{
			
			TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
			if( tVmpParam.IsVMPAuto() )				//�Զ�����ϳɲ�У���Ա
			{
				byVmpMemberType = VMP_MEMBERTYPE_SPEAKER;	//�¼���Mtֻ����Ϊ������
				bSetChnnl = TRUE;
			}
			else
			{
				TVMPMember tVmpMember = *tVmpParam.GetVmpMember(byPos);
				if ( tVmpMember.GetMtId() == tMt.GetMcuId()	)//���ڶ��ƺϳɣ�����һ�£��Է�ack�յ�֮ǰ�ϳɲ����б�
					
				{
					byVmpMemberType = tVmpMember.GetMemberType();
					bSetChnnl = TRUE;
				}
			}
			if( bSetChnnl)
			{
				SetVmpChnnl(tMt, byPos, byVmpMemberType);

				if(tVmpParam.IsVMPAuto())
				{
					//�������Զ�����ϳɣ�����tPeriEqpStatus��mcsҪ���ݴ�ˢMTͼ��
					TPeriEqpStatus tPeriEqpStatus; 
					g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
					tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
					g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
					CServMsg cMsg;
					cMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
					SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cMsg );
				}
			}
			else
			{
				ConfLog(FALSE,"[ProcMcuMcuAdjustMtResAck] not set in vmp chnnl for certain reason!!\n");
			}
			
		}
		else
		{
			//����Ԥ������
		}
	}
	else	//�ָ��ֱ���
	{
		//Do nothing
	}

}

/*=============================================================================
    �� �� ���� ProcMcuMcuGetMixerParamReq
    ��    �ܣ� ���� �ϼ�MCU��ȡ�¼�MCU�������� ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuGetMixerParamReq( const CMessage * pcMsg)
{	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	

	TMixParam tMixParam = m_tConf.m_tStatus.GetMixerParam();
    // guzh [11/7/2007] 
    /*
    if (m_tConf.m_tStatus.IsVACMode())
    {
        if (m_tConf.m_tStatus.IsDiscussMode())
        {
            tMixParam.SetMode(mcuVacWholeMix);
        }
        else
        {
            tMixParam.SetMode(mcuVacMix);
        }
    }
    else
    {
        if (FALSE == m_tConf.m_tStatus.IsDiscussMode())
        {
            tMixParam.SetMode(mcuNoMix);
        }
        else if (m_tConf.m_tStatus.IsMixSpecMt())
        {
            tMixParam.SetMode(mcuPartMix);
        }
        else
        {
            tMixParam.SetMode(mcuWholeMix);
        }
    }
    */

	TMcu tMcu;
	tMcu.SetMcuId(LOCAL_MCUID);
	cServMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );
	cServMsg.CatMsgBody( (u8*)&tMixParam, sizeof(TMixParam) );
	SendReplyBack( cServMsg, MCU_MCU_MIXERPARAM_NOTIF );

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuGetMixerParamAck
    ��    �ܣ� ���� �¼�MCU��ȡMCU�������� ��Ӧ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuGetMixerParamAck( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
    
	cServMsg.SetSrcMtId(0);
	//cServMsg.SetEventId(MCU_MCS_GETDISCUSSPARAM_ACK);

    /*
    //zbq[12/06/2007] ��ز��ٸ�֪�¼�MCU�Ļ���״̬
	//�����¼����������Ļ����� MCU_MCS_MIXPARAM_NOTIF ����֪ͨ
	cServMsg.SetEventId(MCU_MCS_MIXPARAM_NOTIF);
	SendReplyBack( cServMsg, cServMsg.GetEventId());*/

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuGetMixerParamNack
    ��    �ܣ� ���� �¼�MCU��ȡMCU�������� ��Ӧ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuGetMixerParamNack( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
/*    
	cServMsg.SetSrcMtId(0);
	cServMsg.SetEventId(MCU_MCS_GETDISCUSSPARAM_NACK);
	SendReplyBack( cServMsg, cServMsg.GetEventId());
*/
    return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuMixerParamNotif
    ��    �ܣ� ���� �¼�MCU�������� �ϱ�֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuMixerParamNotif( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
    /*
    //zbq[12/06/2007] ��ز��ٸ�֪�¼�MCU�Ļ���״̬
	cServMsg.SetSrcMtId(0);
	cServMsg.SetEventId(MCU_MCS_MIXPARAM_NOTIF);
	SendMsgToAllMcs(MCU_MCS_MIXPARAM_NOTIF, cServMsg);
	*/
	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuAddMixerMemberCmd
    ��    �ܣ� ���� �ϼ�MCU���ӻ�����Ա����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAddMixerMemberCmd( const CMessage * pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
    TMcu *ptMcu = (TMcu *)cServMsg.GetMsgBody();
	TMt *ptMt  = (TMt*)(cServMsg.GetMsgBody()+sizeof(TMcu));
	u8 byMtNum = (cServMsg.GetMsgBodyLen()-sizeof(TMcu))/sizeof(TMt);
	
    //zbq[11/29/2007] �滻�߼�֧��
    BOOL32 bReplace = FALSE;
    if ( cServMsg.GetMsgBodyLen() > sizeof(TMcu)+sizeof(TMt)*byMtNum)
    {
        u8 byReplace = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMcu)+sizeof(TMt)*byMtNum);
        bReplace = byReplace == 1 ? TRUE : FALSE;
    }
	STATECHECK
		
    //  [7/19/2006] �����ϼ�ָ��������ʱ���¼�û�п�����������Ҫ�ȿ�������
    // guzh [11/7/2007] 
    if (m_tConf.m_tStatus.IsNoMixing())
    {     
        //���ϼ�mcu��������б�
        byMtNum++;
        
        CServMsg cMsg;
        cMsg.SetEventId(MCU_MCU_STARTMIXER_CMD);
        cMsg.SetMsgBody((u8*)ptMcu, sizeof(TMcu));        
        cMsg.CatMsgBody(&byMtNum, sizeof(byMtNum));
        cMsg.CatMsgBody((u8*)&m_tCascadeMMCU, sizeof(TMt));        
        cMsg.CatMsgBody((u8*)ptMt, sizeof(TMt)*(byMtNum-1));
        g_cMcuVcApp.SendMsgToConf(m_byConfIdx, MCU_MCU_STARTMIXER_CMD, cMsg.GetServMsg(), cMsg.GetServMsgLen());
    }
    else
    {
        //zbq [11/26/2007] �������ơ����ܻ���������ˢ���¼�֧��
        //zbq [09/30/2007] ��ǰ��VAC���ܾ��ϼ��ļ������ƻ�������
        if (m_tConf.m_tStatus.IsVACing())
        {
            //ConfLog(FALSE, "[AddMixerMemberCmd] IsVACMode is true. nack\n");
            //NotifyMcsAlarmInfo( 0, ERR_MCU_CASSPECMIXMUTEXVAC );
            //return;
            
            NotifyMcsAlarmInfo(0, ERR_MCU_MMCUSPECMIX_VAC);
            ConfLog( FALSE, "[AddMixerMemberCmd] Local VAC mode has been canceled due to cas specmixing\n" );
            
            CServMsg cMsg;
            cMsg.SetEventId(MCS_MCU_STOPVAC_REQ);
            MixerVACReq(cMsg);
        }
        


        // zbq [09/30/2007] ���´�R2_GDZF�ϲ�����������ǿ�˴����۵�����
        // ������αװ���ܣ����Խ����MCS����MCU��Ϊ�������Զ����������
        // ǰ���µļ����������µ�ʱ�����⡣��MCS��ʼ֧��MCU��Ϊ��������
        // �����������������������ҪMtAdpLib��ͬ����������֧�ֵ�.

        // zbq [06/29/2007] �����������Ե���: �����Ѿ������Ļ�����������
        // �ϼ�MCU������Ϊ�����Ļ��������ϼ��ͱ���֮��ļ�����������ͣ��
        // ����(������������)��(����)���������¿����������µļ���������
        
        BOOL32 bMMcuInLocalMix = FALSE;
        BOOL32 bLocalWholeMix = FALSE;

        if ( m_tCascadeMMCU.IsNull() )
        {
            ConfLog( FALSE, "[AddMixerMemberCmd] m_tCascadeMMCU.IsNull, impossible \n" );
            return;
        }

        if ( m_ptMtTable->IsMtInMixing(m_tCascadeMMCU.GetMtId()) )
        {
            bMMcuInLocalMix = TRUE;
        }

        if ( !m_tConf.m_tStatus.IsSpecMixing() )
        {
            bLocalWholeMix = TRUE;
        }
        
        if ( !bMMcuInLocalMix || bLocalWholeMix || bReplace )
        {
            if ( !bMMcuInLocalMix || bReplace )
            {
                // �Ƴ�����ԭ�е����г�Ա
                for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
                {
                    if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
                        m_ptMtTable->IsMtInMixing(byMtId) )            
                    {
                        TMt tMt = m_ptMtTable->GetMt(byMtId);
                        AddRemoveSpecMixMember(&tMt, 1, FALSE, TRUE);
                    }
                }
            }

            // zbq [09/30/2007] ����ģʽ����������ָ���ǰ�Ľ���
            if ( bLocalWholeMix )
            {
                for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
                {
                    if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
                    {
                        StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
                                            (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+byMtId), FALSE, MODE_AUDIO);
                        
                        StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);                       
                        
                        // �ָ����鲥��ַ����
                        if (m_tConf.GetConfAttrb().IsSatDCastMode() && m_ptMtTable->IsMtInMixGrp(byMtId))
                        {
                            ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_AUDIO);
                        }
                        m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);
                    }
                }
            }
            
            // ͣ��������
            CServMsg cServMsg;
            cServMsg.SetEventId(MCS_MCU_STOPDISCUSS_REQ);            
            OnStopDiscussToAllSMcu( &cServMsg );
            
            // zbq [07/12/2007] �ϼ�MCUҲ��Ҫ����
            ptMt[byMtNum] = m_tCascadeMMCU;
            byMtNum ++;
        }

        // zbq [09/30/2007] �����ǰ������ģʽ������Ϊ���ƻ���
        if ( bLocalWholeMix )
        {
            m_tConf.m_tStatus.SetSpecMixing(TRUE);
            ConfModeChange();
        }

        AddRemoveSpecMixMember( ptMt, byMtNum, TRUE );

        // zbq [09/30/2007] �����ǰ������ģʽ, ������Nģʽ�Ľ���
        if ( bLocalWholeMix )
        {
            for (u8 byMtId = 1; byMtId < MAXNUM_CONF_MT; byMtId++)
            {
                if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
                {
                    TMt tMt = m_ptMtTable->GetMt(byMtId);
                    if (m_tConf.m_tStatus.IsSpecMixing() && !m_ptMtTable->IsMtInMixing(byMtId))
                    {
                        SwitchMixMember(&tMt, TRUE);
                    }
                }           
            }
        }

        //���ƻ���״̬֪ͨ
        ConfStatusChange();
    }	
	
	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuRemoveMixerMemberCmd
    ��    �ܣ� ���� �ϼ�MCU�Ƴ�������Ա����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuRemoveMixerMemberCmd( const CMessage * pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt *ptMt  = (TMt*)(cServMsg.GetMsgBody()+sizeof(TMcu));
	u8 byMtNum = (cServMsg.GetMsgBodyLen()-sizeof(TMcu))/sizeof(TMt);
	
	STATECHECK
		
	AddRemoveSpecMixMember( ptMt, byMtNum, FALSE );

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuLockReq
    ��    �ܣ� �����ϼ�mcu�Ա���MCU����������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage *pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/11   3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuLockReq( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	STATECHECK;	

	u8 byLock = *(cServMsg.GetMsgBody()+sizeof(TMcu));
    u8 byMcuId = cServMsg.GetSrcMtId();
    if ( byMcuId != m_tCascadeMMCU.GetMtId() )
    {
	    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }

	if (ISTRUE(byLock))
	{
		m_tConf.m_tStatus.SetProtectMode(CONF_LOCKMODE_LOCK);
        // guzh [7/5/2007] ͬʱ��ȡ��mcs������
        m_tConfProtectInfo.SetLockByMcu(byMcuId);
	}
	else
	{
		if (m_tConfProtectInfo.GetLockedMcuId() == byMcuId)
		{
            // guzh [7/5/2007] �ϼ�������ͬʱ����mcs������
			m_tConf.m_tStatus.SetProtectMode(CONF_LOCKMODE_NONE);
            m_tConfProtectInfo.SetLockByMcu(0);
            m_tConfProtectInfo.SetLockByMcs(0);
		}
	}

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	//֪ͨ���
    cServMsg.SetMsgBody((u8*)&byLock, sizeof(byLock));
	SendMsgToAllMcs( MCU_MCS_CONFLOCKMODE_NOTIF, cServMsg );

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuLockAck
    ��    �ܣ� �����¼�mcu�����������Ӧ��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage *pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/11   3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuLockAck( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	STATECHECK;	

	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(cServMsg.GetSrcMtId());
	if (NULL == ptMcInfo)
	{
		cServMsg.SetSrcMtId(0);
		cServMsg.SetEventId(MCU_MCS_LOCKSMCU_NACK);
		SendReplyBack( cServMsg, cServMsg.GetEventId());
	}
	u8 byLock = *(cServMsg.GetMsgBody()+sizeof(TMcu));

	ptMcInfo->SetIsLocked(ISTRUE(byLock));

	cServMsg.SetSrcMtId(0);
	/*
	cServMsg.SetEventId(MCU_MCS_LOCKSMCU_ACK);
	SendReplyBack( cServMsg, cServMsg.GetEventId());
	*/	
	cServMsg.SetEventId(MCU_MCS_MCULOCKSTATUS_NOTIF);
	SendMsgToAllMcs(MCU_MCS_MCULOCKSTATUS_NOTIF, cServMsg);

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuLockNack
    ��    �ܣ� �����¼�mcu�����������Ӧ��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage *pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/11   3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuLockNack( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
/*  
	cServMsg.SetSrcMtId(0);
	cServMsg.SetEventId(MCU_MCS_LOCKSMCU_NACK);
	SendReplyBack( cServMsg, cServMsg.GetEventId());
*/

	return;
}

/*=============================================================================
    �� �� ���� ProcMcsMcuGetMcuLockStatusReq
    ��    �ܣ� �����ضԱ�MCU�ϵ��¼�MCU����״̬�Ĳ�ѯ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage *pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/11   3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMcsMcuGetMcuLockStatusReq(const CMessage *pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	STATECHECK;

	if (NULL == m_ptConfOtherMcTable)
	{
		cServMsg.SetErrorCode(ERR_LOCKSMCU_NOEXIT);
		SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
		return;
	}

	TMcu tMcu;
	u8   byMcuNum = 0;
	TMcu *ptMcu = (TMcu *)(cServMsg.GetMsgBody());
	TConfMcInfo *ptMcInfo = NULL;
	if (0 != ptMcu->GetMcuId())
	{
		ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());
		if (NULL == ptMcInfo)
		{
			cServMsg.SetErrorCode(ERR_LOCKSMCU_NOEXIT );
			SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
			return;
		}
		cServMsg.CatMsgBody((u8 *)&(ptMcInfo->m_byIsLocked), sizeof(u8));
	}
	else
	{
		for (u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++)
		{
			ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
			if (0 == ptMcInfo->m_byMcuId || 
				(!m_tCascadeMMCU.IsNull() && ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId()))
			{
				continue;
			}
			tMcu.SetMcu(ptMcInfo->m_byMcuId);
			if (0 == byMcuNum)
			{
				cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(TMcu));
			}
			else
			{
				cServMsg.CatMsgBody((u8 *)&tMcu, sizeof(TMcu));
			}
			cServMsg.CatMsgBody((u8 *)&(ptMcInfo->m_byIsLocked), sizeof(u8));

			byMcuNum++;
		}
		if (0 == byMcuNum)
		{
			cServMsg.SetErrorCode(ERR_LOCKSMCU_NOEXIT);
			SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
			return;
		}
	}

	SendReplyBack(cServMsg, cServMsg.GetEventId()+1);

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuMtStatusCmd
    ��    �ܣ� �����ϼ�mcu�Ա���ֱ���ն˵�״̬��ѯ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage *pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/11   3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuMtStatusCmd( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	STATECHECK;	

	if (m_tCascadeMMCU.IsNull() || 
		m_tCascadeMMCU.GetMtId() != cServMsg.GetSrcMtId() )
	{
		return;
	}

	OnNtfMtStatusToMMcu( m_tCascadeMMCU.GetMtId() );

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuMtStatusNotif
    ��    �ܣ� �����¼�mcuֱ���ն˵�״̬֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage *pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/11   3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuMtStatusNotif( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	

	if( NULL == m_ptConfOtherMcTable )
	{
		return;
	}

	TMcu *ptMcu = (TMcu *)(cServMsg.GetMsgBody());
	TConfMcInfo *ptMcInfo = NULL;
	if( 0 == ptMcu->GetMcuId() )
	{
		return;
	}
	ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());
	if( NULL == ptMcInfo )
	{
		return;
	}

	s32 nMtNum = (cServMsg.GetMsgBodyLen()-sizeof(TMcu))/sizeof(TSMcuMtStatus);
	TSMcuMtStatus *ptSMcuMtStatus = (TSMcuMtStatus *)(cServMsg.GetMsgBody()+sizeof(TMcu));
	CServMsg cServBackMsg;
	cServBackMsg.SetNoSrc();
	for( s32 nPos=0; nPos<nMtNum; nPos++ )
	{	
		u8 byMtId  = ptSMcuMtStatus->GetMtId();
		s32 nIndex = 0;
		for(; nIndex<MAXNUM_CONF_MT; nIndex++)
		{
			if( ptMcInfo->m_atMtStatus[nIndex].IsNull() || 
				ptMcInfo->m_atMtStatus[nIndex].GetMtId() == 0 ) //�Լ�
			{
				continue;
			}
			if(ptMcInfo->m_atMtStatus[nIndex].GetMtId() == byMtId)
			{
				break;
			}
		}
			
		if( nIndex < MAXNUM_CONF_MT )
		{
			//ptMcInfo->m_atMtStatus[nIndex].SetMt( *((TMt *)ptSMcuMtStatus) );
			ptMcInfo->m_atMtStatus[nIndex].SetIsEnableFECC( ptSMcuMtStatus->IsEnableFECC() );
			ptMcInfo->m_atMtStatus[nIndex].SetCurVideo( ptSMcuMtStatus->GetCurVideo() );
			ptMcInfo->m_atMtStatus[nIndex].SetCurAudio( ptSMcuMtStatus->GetCurAudio() );
			ptMcInfo->m_atMtStatus[nIndex].SetMtBoardType( ptSMcuMtStatus->GetMtBoardType() );   
            ptMcInfo->m_atMtStatus[nIndex].SetInMixing( ptSMcuMtStatus->IsMixing() );
            ptMcInfo->m_atMtStatus[nIndex].SetVideoLose( ptSMcuMtStatus->IsVideoLose() );
			ptMcInfo->m_atMtStatus[nIndex].SetReceiveVideo( ptSMcuMtStatus->IsRecvVideo() );
			ptMcInfo->m_atMtStatus[nIndex].SetReceiveAudio( ptSMcuMtStatus->IsRecvAudio() );
			ptMcInfo->m_atMtStatus[nIndex].SetSendVideo( ptSMcuMtStatus->IsSendVideo() );
			ptMcInfo->m_atMtStatus[nIndex].SetSendAudio( ptSMcuMtStatus->IsSendAudio() );

			TMtStatus tMtStatus = ptMcInfo->m_atMtStatus[nIndex].GetMtStatus();
            cServBackMsg.SetMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
			SendMsgToAllMcs( MCU_MCS_MTSTATUS_NOTIF, cServBackMsg );
		}
		ptSMcuMtStatus++;
	}

	return;
}


/*=============================================================================
    �� �� ���� ProcMcuMcuAutoSwitchReq
    ��    �ܣ� ����RadMMcu���Զ���ѯ ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage *pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/07/09  4.0  		�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAutoSwitchReq( const CMessage *pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );    
    u8 bySrcMtId = cServMsg.GetSrcMtId();
    TCAutoSwitchReq tSwitchReq = *(TCAutoSwitchReq*)cServMsg.GetMsgBody();

    MMcuLog("[ProcMcuMcuAutoSwitchReq] tSwitchReq with On.%d, level.%d, time.%d, lid.%d\n",
            tSwitchReq.m_bSwitchOn, tSwitchReq.m_nAutoSwitchLevel, 
            tSwitchReq.m_nSwitchSpaceTime, tSwitchReq.m_nSwitchLayerId );
    
    if ( m_tCascadeMMCU.IsNull() ||
         bySrcMtId != m_tCascadeMMCU.GetMtId() ||
         ( MT_MANU_RADVISION != m_ptMtTable->GetManuId(m_tCascadeMMCU.GetMtId()) &&
           MT_MANU_KDCMCU != m_ptMtTable->GetManuId(m_tCascadeMMCU.GetMtId()) ) )
    {
        ConfLog( FALSE, "[AutoSwitchReq] Roused by Mt.%d, Manu.%d, ignore it\n",
                         bySrcMtId, m_ptMtTable->GetManuId(m_tCascadeMMCU.GetMtId()) );

        SendReplyBack( cServMsg, cServMsg.GetEventId()+2 );
        return;
    }    
    
    // ��ʼ��ѯ(�ϼ����ʼ��ѯ���������¼�֪ͨ����ɱ�����ѯ��
    if ( tSwitchReq.m_bSwitchOn ) /*||
         ( m_tConfInStatus.IsPollSwitch() && 
           !tSwitchReq.m_bSwitchOn && tSwitchReq.m_nAutoSwitchLevel > m_tPollSwitchParam.GetLevel() ) )*/
    {
        u8 byMtId = 1;
        for(; byMtId <= MAXNUM_CONF_MT; byMtId++ )
        {
            TMtStatus tMtStatus;
            if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
                 m_ptMtTable->GetMtStatus(byMtId, &tMtStatus) 
                 /*&& !tMtStatus.IsVideoLose()*/ )
            {
                break;
            }
        }
        if ( byMtId > MAXNUM_CONF_MT )
        {
            SendReplyBack( cServMsg, cServMsg.GetEventId()+2 );
            return;
        }

        ConfLog( FALSE, "Start auto poll switch.\n" );
        
        // ��������Ѿ�����ѯ�������
        if ( m_tConfInStatus.IsPollSwitch() )
        {
        }
        else
        {
            m_tConfInStatus.SetPollSwitch(TRUE);
            m_tPollSwitchParam.Reset();
            m_tPollSwitchParam.SetTimeSpan(tSwitchReq.m_nSwitchSpaceTime);
            m_tPollSwitchParam.SetLevel(tSwitchReq.m_nAutoSwitchLevel);
            m_tPollSwitchParam.SetLid(tSwitchReq.m_nSwitchLayerId);
            m_tPollSwitchParam.SpecPollPos(POLLING_POS_START);        
        }

        // ��ʼ��ѯ
        SetTimer( MCUVC_AUTOSWITCH_TIMER, 10);        
    }
    else
    {
        KillTimer( MCUVC_AUTOSWITCH_TIMER );
        m_tConfInStatus.SetPollSwitch(FALSE);
        m_tPollSwitchParam.Reset();        
        
        SendReplyBack( cServMsg, cServMsg.GetEventId()+1 );
    }
    return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuAutoSwitchRsp
    ��    �ܣ� ����MMcu���Զ���ѯ�ش� ����Ӧ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage *pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/07/19  4.0  		����                  ����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAutoSwitchRsp( const CMessage *pcMsg )
{
    if (pcMsg->event == MCU_MCU_AUTOSWITCH_NACK)
    {
        // ����������ѯ
        if ( m_tConfInStatus.IsPollSwitch() )
        {
            SetTimer( MCUVC_AUTOSWITCH_TIMER, 10);
        }
    }
}


/*=============================================================================
    �� �� ���� ProcMcuMcuAutoSwitchTimer
    ��    �ܣ� ��ʱ��ѯ������
    �㷨ʵ�֣� ���ݶ�ʱ�������ն�ID,������ѯ�������ߵķ���Ƶ��ʧ�նˡ���ѯ�м�
               ����ն˵�ID���ڵ�ǰ��ѯ���ն�IDǰ���򲻱���ѯ����
    ȫ�ֱ����� 
    ��    ���� const CMessage *pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/07/13  4.0  		�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAutoSwitchTimer( const CMessage *pcMsg )
{
    if ( !m_tConfInStatus.IsPollSwitch() )
    {
        return;
    }

    u8 byStartMtId;
    if ( m_tPollSwitchParam.IsSpecPos() )
    {
        byStartMtId = m_tPollSwitchParam.GetSpecPos();
        if (byStartMtId == POLLING_POS_START)
        {
            byStartMtId = 0;
        }
        m_tPollSwitchParam.ClearSpecPos();
    }
    else
    {
        byStartMtId = m_tPollSwitchParam.GetCurrPos() + 1;
    }

    u8 byMtId = byStartMtId;
    for( ; byMtId <= MAXNUM_CONF_MT; byMtId++ )
    {
        TMtStatus tMtStatus;
        if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
             m_tCascadeMMCU.GetMtId() != byMtId &&
             m_ptMtTable->GetMtStatus(byMtId, &tMtStatus) &&
             tMtStatus.IsSendVideo() 
             /*&& !tMtStatus.IsVideoLose()*/ )
        {
            break;
        }
    }    
    if ( byMtId > MAXNUM_CONF_MT || m_tCascadeMMCU.IsNull() )
    {
        // ����������֪ͨ�ϼ�������ʼ
        ConfLog( FALSE, "All Mts has be switched over.\n");
        
        if (!m_tCascadeMMCU.IsNull())
        {
            TCAutoSwitchReq tSwitchRestartReq;
            tSwitchRestartReq.m_bSwitchOn = TRUE;
            tSwitchRestartReq.m_nAutoSwitchLevel = m_tPollSwitchParam.GetLevel() - 1;
            tSwitchRestartReq.m_nSwitchSpaceTime = m_tPollSwitchParam.GetTimeSpan();
            tSwitchRestartReq.m_nSwitchLayerId = m_tPollSwitchParam.GetLid();
            
            CServMsg cServMsg;
            cServMsg.SetEventId(MCU_MCU_AUTOSWITCH_REQ);
            cServMsg.SetMsgBody((u8*)&tSwitchRestartReq, sizeof(tSwitchRestartReq));
            
            SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_AUTOSWITCH_REQ, cServMsg );
            MMcuLog("Notify restart auto poll swtich to mmcu, bOn.%d, lelev.%d, time.%d, lid.%d\n" ,
                    tSwitchRestartReq.m_bSwitchOn, tSwitchRestartReq.m_nAutoSwitchLevel,
                    tSwitchRestartReq.m_nSwitchSpaceTime, tSwitchRestartReq.m_nAutoSwitchLevel );
        }
        
        KillTimer( MCUVC_AUTOSWITCH_TIMER );
        m_tConfInStatus.SetPollSwitch(FALSE);
        m_tPollSwitchParam.Reset();
    }
    else
    {
        TMt tSetInMt = m_ptMtTable->GetMt(byMtId);
        m_tPollSwitchParam.SetCurrPos(byMtId);
        ConfLog( FALSE, "Auto Switched to Setout Mt.%d to poll.\n", byMtId );
        
        if ( tSetInMt.GetMtType() == MT_TYPE_MT)
        {            
            OnSetOutView(tSetInMt);

            SetTimer( MCUVC_AUTOSWITCH_TIMER, m_tPollSwitchParam.GetTimeSpan()*1000);
        }
        else
        {
            // ��ʼ�¼��Ļش���ѯ��������ʱ��ֹͣ�����ǲ����״̬�����¼����������������
            TCAutoSwitchReq tSwitchStartSMcuReq;
            tSwitchStartSMcuReq.m_bSwitchOn = TRUE;
            tSwitchStartSMcuReq.m_nAutoSwitchLevel = m_tPollSwitchParam.GetLevel() + 1;
            tSwitchStartSMcuReq.m_nSwitchSpaceTime = m_tPollSwitchParam.GetTimeSpan();
            tSwitchStartSMcuReq.m_nSwitchLayerId = m_tPollSwitchParam.GetLid();

            CServMsg cServMsg;
            cServMsg.SetEventId(MCU_MCU_AUTOSWITCH_REQ);
            cServMsg.SetMsgBody((u8*)&tSwitchStartSMcuReq, sizeof(tSwitchStartSMcuReq));
            
            SendMsgToMt( tSetInMt.GetMtId(), MCU_MCU_AUTOSWITCH_REQ, cServMsg );
            KillTimer( MCUVC_AUTOSWITCH_TIMER );
        }                
    }
    
    return;
}
/*=============================================================================
    �� �� ���� OnGetMtStatusCmdToSMcu
    ��    �ܣ� ���¼�mcu����������ֱ���ն�״̬���������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byDstMcuId
               TMtStatus* ptSrcMtStatus
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/11   3.6			����                  ����
=============================================================================*/
void CMcuVcInst::OnGetMtStatusCmdToSMcu( u8 byDstMcuId )
{
	CServMsg cServMsg;
	cServMsg.SetDstMtId( byDstMcuId );
	cServMsg.SetEventId( MCU_MCU_MTSTATUS_CMD );
	SendMsgToMt( byDstMcuId, MCU_MCU_MTSTATUS_CMD, cServMsg );

	return;
}
		
/*=============================================================================
    �� �� ���� OnNtfMtStatusToMMcu
    ��    �ܣ� ���ϼ�mcu���ͱ���ֱ���ն˵�״̬����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byDstMcuId
               u8 byMtId
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/11   3.6			����                ����
    2007/11/29  4.0         �ű���                �ӿ��Ż�
=============================================================================*/
void CMcuVcInst::OnNtfMtStatusToMMcu(u8 byDstMcuId, u8 byMtId)
{
    if ( byMtId > MAXNUM_CONF_MT )
    {
        ConfLog( FALSE, "[OnNtfMtStatusToMMcu] Mt.%d is illeagal, ignore it\n", byMtId );
        return;
    }

	CServMsg cServMsg;
	u8 byMtNum = 0;
	cServMsg.SetDstMtId( byDstMcuId );
	cServMsg.SetEventId( MCU_MCU_MTSTATUS_NOTIF );

	if( 0 == byMtId )
	{
		TMtStatus tLocalMtStatus;
		TSMcuMtStatus tSMcuMtStatus;
		for( u8 byMtLoop = 1; byMtLoop <= MAXNUM_CONF_MT; byMtLoop++ )
		{
			// �����¼�MCUֻ�ϱ����ߵ��ն�״̬
			// ����VCS���黹��Ҫ�ϱ���ʱ�����ն˵���Ϣ
			if( (m_tConfAllMtInfo.MtJoinedConf( byMtLoop ) && 
				 byDstMcuId != byMtLoop) ||
		        (VCS_CONF == m_tConf.GetConfSource() && CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtLoop)))
			{		
				m_ptMtTable->GetMtStatus( byMtLoop, &tLocalMtStatus );
				memcpy( (void*)&tSMcuMtStatus, (void*)&tLocalMtStatus, sizeof(TMt) );
				tSMcuMtStatus.SetIsEnableFECC( tLocalMtStatus.IsEnableFECC() );
				tSMcuMtStatus.SetCurVideo( tLocalMtStatus.GetCurVideo() );
				tSMcuMtStatus.SetCurAudio( tLocalMtStatus.GetCurAudio() );
				tSMcuMtStatus.SetMtBoardType( tLocalMtStatus.GetMtBoardType() );
                tSMcuMtStatus.SetIsMixing( tLocalMtStatus.IsInMixing() );
                tSMcuMtStatus.SetVideoLose( tLocalMtStatus.IsVideoLose() );
				tSMcuMtStatus.SetIsAutoCallMode(CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtLoop));
				tSMcuMtStatus.SetRecvVideo( tLocalMtStatus.IsReceiveVideo() );
				tSMcuMtStatus.SetRecvAudio( tLocalMtStatus.IsReceiveAudio() );
				tSMcuMtStatus.SetSendVideo( tLocalMtStatus.IsSendVideo() );
				tSMcuMtStatus.SetSendAudio( tLocalMtStatus.IsSendAudio() );

				if( 0 == byMtNum )
				{
					cServMsg.SetMsgBody( (u8*)&tSMcuMtStatus, sizeof(TSMcuMtStatus) );
				}
				else
				{
					cServMsg.CatMsgBody( (u8*)&tSMcuMtStatus, sizeof(TSMcuMtStatus) );
				}
				byMtNum++;
			}
		}
	}
	else
	{
        TMtStatus tLocalMtStatus;
        TSMcuMtStatus tSMcuMtStatus;
        
        if( m_tConfAllMtInfo.MtInConf(byMtId) && byDstMcuId != byMtId )
        {		
            m_ptMtTable->GetMtStatus( byMtId, &tLocalMtStatus );
            memcpy( (void*)&tSMcuMtStatus, (void*)&tLocalMtStatus, sizeof(TMt) );
            tSMcuMtStatus.SetIsEnableFECC( tLocalMtStatus.IsEnableFECC() );
            tSMcuMtStatus.SetCurVideo( tLocalMtStatus.GetCurVideo() );
            tSMcuMtStatus.SetCurAudio( tLocalMtStatus.GetCurAudio() );
            tSMcuMtStatus.SetMtBoardType( tLocalMtStatus.GetMtBoardType() );
            tSMcuMtStatus.SetIsMixing( tLocalMtStatus.IsInMixing() );
            tSMcuMtStatus.SetVideoLose( tLocalMtStatus.IsVideoLose() );
			tSMcuMtStatus.SetIsAutoCallMode(CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtId));
			tSMcuMtStatus.SetRecvVideo( tLocalMtStatus.IsReceiveVideo() );
			tSMcuMtStatus.SetRecvAudio( tLocalMtStatus.IsReceiveAudio() );
			tSMcuMtStatus.SetSendVideo( tLocalMtStatus.IsSendVideo() );
			tSMcuMtStatus.SetSendAudio( tLocalMtStatus.IsSendAudio() );

            if( 0 == byMtNum )
            {
                cServMsg.SetMsgBody( (u8*)&tSMcuMtStatus, sizeof(TSMcuMtStatus) );
            }
            else
            {
                cServMsg.CatMsgBody( (u8*)&tSMcuMtStatus, sizeof(TSMcuMtStatus) );
            }
            byMtNum++;
        }
	}

	if( byMtNum > 0 )
	{
		SendMsgToMt( byDstMcuId, MCU_MCU_MTSTATUS_NOTIF, cServMsg );
	}

	return;
}

/*====================================================================
    ������      ��BroadcastToAllMcu
    ����        ������Ϣ�������¼�MCU
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wEvent ��Ϣ��
	              const CMessage * pcMsg, �������Ϣ
				  u8 byExceptMc ����Mc��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/08/30    3.5         ������         ����
====================================================================*/
void CMcuVcInst::BroadcastToAllMcu( u16 wEvent, CServMsg & cServMsg, u8 byExceptMc )
{
	u8 byMcuId = 0;
	for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
	{
		byMcuId = m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId();
		if( byMcuId != 0 )
		{
			SendMsgToMt( byMcuId, wEvent, cServMsg );
		}
	}
	
}


/*====================================================================
    ������      ��NotifyMcuNewMt
    ����        ��֪ͨ����MC���ն˼���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/01/25    3.6         Jason         �޸�
====================================================================*/
void CMcuVcInst::NotifyMcuNewMt(TMt tMt)
{
	CServMsg	cServMsg;
	TMtStatus   tMtStatus;
    TMcuMcuMtInfo tInfo;

	//�����ն��б�
	u8 byMtId = tMt.GetMtId();
	tInfo.m_tMt = tMt;
	strncpy((s8 *)(tInfo.m_szMtName), m_ptMtTable->GetMtAliasFromExt(tMt.GetMtId()), sizeof(tInfo.m_szMtName));
	tInfo.m_szMtName[sizeof(tInfo.m_szMtName)-1] = 0;
	tInfo.m_dwMtIp   = htonl(m_ptMtTable->GetIPAddr(byMtId));
	tInfo.m_byMtType = m_ptMtTable->GetMtType(byMtId);
	tInfo.m_byManuId = m_ptMtTable->GetManuId(byMtId);
	strncpy((s8 *)tInfo.m_szMtDesc, "notsupport", sizeof(tInfo.m_szMtDesc));
	tInfo.m_szMtDesc[sizeof(tInfo.m_szMtDesc)-1] = 0;
	TLogicalChannel  tChannel;
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_VIDEO, &tChannel, FALSE ) )
	{
		tInfo.m_byVideoIn = tChannel.GetChannelType();
	}
	else
	{
		tInfo.m_byVideoIn = MEDIA_TYPE_NULL;
	}
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_VIDEO, &tChannel, TRUE ) )
	{
		tInfo.m_byVideoOut = tChannel.GetChannelType();
	}
	else
	{
		tInfo.m_byVideoOut = MEDIA_TYPE_NULL;
	}
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_SECVIDEO, &tChannel, FALSE ) )
	{
		tInfo.m_byVideo2In = tChannel.GetChannelType();
	}
	else
	{
		tInfo.m_byVideo2In = MEDIA_TYPE_NULL;
	}
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_SECVIDEO, &tChannel, TRUE ) )
	{
		tInfo.m_byVideo2Out = tChannel.GetChannelType();
	}
	else
	{
		tInfo.m_byVideo2Out = MEDIA_TYPE_NULL;
	}
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_AUDIO, &tChannel, FALSE ) )
	{
		tInfo.m_byAudioIn = tChannel.GetChannelType();
	}
	else
	{
		tInfo.m_byAudioIn = MEDIA_TYPE_NULL;
	}
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_AUDIO, &tChannel, TRUE ) )
	{
		tInfo.m_byAudioOut = tChannel.GetChannelType();
	}
	else
	{
		tInfo.m_byAudioOut = MEDIA_TYPE_NULL;
	}
	tInfo.m_byIsDataMeeting =  m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_T120DATA, &tChannel, TRUE ) ? 1:0;
	if(!ISTRUE(tInfo.m_byIsDataMeeting))
	{
		tInfo.m_byIsDataMeeting =  m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_T120DATA, &tChannel, FALSE ) ? 1:0;
	}	
	m_ptMtTable->GetMtStatus(byMtId, &tMtStatus);
	tInfo.m_byIsVideoMuteIn		= GETBBYTE(!tMtStatus.IsSendVideo());
	tInfo.m_byIsVideoMuteOut	= GETBBYTE(!tMtStatus.IsReceiveVideo());   
	tInfo.m_byIsAudioMuteIn		= GETBBYTE(!tMtStatus.IsSendAudio());
	tInfo.m_byIsAudioMuteOut	= GETBBYTE(!tMtStatus.IsReceiveAudio());
	tInfo.m_byIsConnected		= GETBBYTE(m_tConfAllMtInfo.MtJoinedConf(tMt.GetMcuId(), tMt.GetMtId()));
	tInfo.m_byIsFECCEnable		= GETBBYTE(m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_H224DATA, &tChannel, TRUE )); 
	
	tInfo.m_tPartVideoInfo.m_nViewCount = 0;
	if(tMt == m_tConf.GetSpeaker())
	{
		tInfo.m_tPartVideoInfo.m_atViewPos[tInfo.m_tPartVideoInfo.m_nViewCount].m_nViewID = m_dwSpeakerViewId;
		tInfo.m_tPartVideoInfo.m_atViewPos[tInfo.m_tPartVideoInfo.m_nViewCount].m_bySubframeIndex = 0;
		tInfo.m_tPartVideoInfo.m_nViewCount++;
	}
	if(m_tConf.GetStatus().GetVmpParam().IsMtInMember(tMt))
	{
		u8 byMemberId  = m_tConf.GetStatus().GetVmpParam().GetChlOfMtInMember( tMt );
		tInfo.m_tPartVideoInfo.m_atViewPos[tInfo.m_tPartVideoInfo.m_nViewCount].m_nViewID = m_dwVmpViewId;
		tInfo.m_tPartVideoInfo.m_atViewPos[tInfo.m_tPartVideoInfo.m_nViewCount].m_bySubframeIndex = byMemberId;
		tInfo.m_tPartVideoInfo.m_nViewCount++;
	}
    
	TMt tMtVSrc;
	m_ptMtTable->GetMtSrc(byMtId, &tMtVSrc, MODE_VIDEO);
	if(tMtVSrc.IsNull())
	{
		tInfo.m_tPartVideoInfo.m_nOutputLID = 0;
	}
	else if(tMtVSrc == m_tConf.GetSpeaker())
	{
		tInfo.m_tPartVideoInfo.m_nOutputLID = m_dwSpeakerViewId;
	}
	else if(tMtVSrc == m_tVmpEqp)
	{
			tInfo.m_tPartVideoInfo.m_nOutputLID = m_dwVmpViewId;
	}
    tInfo.m_tPartVideoInfo.m_nOutVideoSchemeID = 0;

    MMcuLog("[NotifyMcuNewMt] MtId.%d szMtName.%s \n", tInfo.m_tMt.GetMtId(), tInfo.m_szMtName);

	cServMsg.SetMsgBody((u8 *)&tInfo, sizeof(TMcuMcuMtInfo));
	BroadcastToAllMcu( MCU_MCU_NEWMT_NOTIF, cServMsg );

	// vcsˢ�б�ͬʱ��Ҫˢ����״̬��Ϣ���������а�����Ҫ���ն˺���ģʽ��Ϣ
	if (VCS_CONF == m_tConf.GetConfSource() &&
		!m_tCascadeMMCU.IsNull())
	{
		OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(),tMt.GetMtId());
	}

}

/*====================================================================
    ������      ��NotifyMcuDropMt
    ����        ��֪ͨ����MC�Ҷ��ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/01/25    3.6         Jason         �޸�
====================================================================*/
void CMcuVcInst::NotifyMcuDropMt(TMt tMt)
{
	CServMsg	cServMsg;
	
	cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
	BroadcastToAllMcu( MCU_MCU_DROPMT_NOTIF, cServMsg );
}

/*====================================================================
    ������      ��NotifyMcuDropMt
    ����        ��֪ͨ����MCɾ���ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/01/25    3.6         Jason         �޸�
====================================================================*/
void CMcuVcInst::NotifyMcuDelMt(TMt tMt)
{
	CServMsg	cServMsg;
	
	cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
	BroadcastToAllMcu( MCU_MCU_DELMT_NOTIF, cServMsg );
}

/*====================================================================
    ������      : GetConfOtherMcMtInfo
    ����        : �õ�����Mc���ն���Ϣ
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: ��
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/31    3.0         ������       ����
====================================================================*/
TConfMcInfo *CMcuVcInst::GetConfMcInfo( u8 byMcId )
{
	//����Mc��
	TConfMcInfo *ptConfOtherMcInfo = NULL;
	
	u8 byLoop = 0;
	while( byLoop < MAXNUM_SUB_MCU )
	{
		if( m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop].m_byMcuId != byMcId )
		{
			byLoop ++;
		}
		else
		{
			//�Ѵ���
			ptConfOtherMcInfo = &m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop];
			break;
		}
	}

	return ptConfOtherMcInfo;
}


/*====================================================================
    ������      : GetOtherMcData
    ����        : �õ�����Mc������
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: ��
    ����ֵ˵��  : �ɹ�-TRUE δ����-FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/11/13    3.5         ������       ����
====================================================================*/
BOOL32 CMcuVcInst::GetMcData( u8 byMcuId, TConfMtInfo &tConfMtInfo, TConfMcInfo &tConfOtherMcInfo )
{
	TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(byMcuId);
	if(ptConfMcInfo == NULL)
	{
		return FALSE;
	}
	tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(byMcuId);

	memcpy(&tConfOtherMcInfo, ptConfMcInfo,sizeof(TConfMcInfo));

	return TRUE;
}

/*====================================================================
    ������      ��GetLocalMtFromSmcuMt
    ����        ���õ��¼�MCU��Ӧ�ı����ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt tMt �¼��ն�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/09/01    3.5         ������        ����
====================================================================*/
TMt  CMcuVcInst::GetLocalMtFromOtherMcuMt( TMt tMt )
{
    u8 byMtId;
    if( tMt.IsLocal() )
    {
        if (TYPE_MCUPERI == tMt.GetType())
        {
            return tMt;
        }
        byMtId = tMt.GetMtId();
    }
    else
    {
        byMtId = tMt.GetMcuId();
    }
    return m_ptMtTable->GetMt(byMtId);
}

/*====================================================================
    ������      ��GetLocalSpeaker
    ����        ���õ����صķ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/09/13    3.5         ������        ����
====================================================================*/
TMt  CMcuVcInst::GetLocalSpeaker( void )
{
	if( m_tConf.GetSpeaker().IsNull() )
	{
		return m_tConf.GetSpeaker();
	}
	else if( m_tConf.GetSpeaker().IsLocal() )
	{
		return m_tConf.GetSpeaker();
	}
	else
	{		
		return m_ptMtTable->GetMt(m_tConf.GetSpeaker().GetMcuId());
	}
}

/*====================================================================
    ������      ��GetMcuMediaSrc
    ����        ���õ�Mcu����������Դ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/09/13    3.5         ������        ����
====================================================================*/
TMt  CMcuVcInst::GetMcuMediaSrc(u8 byMcuId)
{
	TMt tMt;
	tMt.SetMcuId(LOCAL_MCUID);
	tMt.SetMtId(byMcuId);
	TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(byMcuId);
	if(ptInfo == NULL)
	{
		return tMt;
	}
	TMt tTmpMt;
	tTmpMt.SetMcuId(byMcuId);
	tTmpMt.SetMtId(0);
	TMcMtStatus *ptStatus = ptInfo->GetMtStatus(tTmpMt);
	if(ptStatus == NULL)
	{
		return tMt;
	}

	s32 nOutputViewId = ptStatus->GetMtVideoInfo().m_nOutputLID;
	s32 nVideoId = ptStatus->GetMtVideoInfo().m_nOutVideoSchemeID;
	if(nOutputViewId == 0)
	{
		nOutputViewId = ptInfo->m_tConfViewInfo.m_atViewInfo[ptInfo->m_tConfViewInfo.m_byDefViewIndex].m_nViewId;
	}

	TCViewInfo *ptViewInfo = NULL;
	for(s32 nIndex=0; nIndex<ptInfo->m_tConfViewInfo.m_byViewCount; nIndex++)
	{
		ptViewInfo = &(ptInfo->m_tConfViewInfo.m_atViewInfo[nIndex]);
		if(ptViewInfo->m_nViewId == nOutputViewId)
		{
			if(ptViewInfo->m_byMtCount > 0)
			{
				tMt = ptViewInfo->m_atMts[0];
			}
			break;
		}
	}
	return tMt;
}

/*====================================================================
    ������      ��ProcMcuMcuSendMsgReq
    ����        ������Ϣ���ʹ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/09/29    3.0         ������          ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuSendMsgReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = *(TMt *)( cServMsg.GetMsgBody() );

	// ������¼�MCU���ϼ�����, �����, zgc, 2007-04-07
	TMt tSrcMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	if( tSrcMt.GetMtType() == MT_TYPE_SMCU )
	{
		ConfLog( FALSE, "[ProcMcuMcuSendMsgReq]This is unallowed that transmit the short msgs to mmcu from smcu!\n");
		return;
	}

	switch( CurState() )
	{
	case STATE_ONGOING:

		if( tMt.IsNull() )	//���������ն� MCU_MCU_SENDMSG_NOTIF
		{
			for(s32 nLoop = 1; nLoop<=MAXNUM_CONF_MT; nLoop++)
			{
				if( m_tConfAllMtInfo.MtJoinedConf(nLoop) && nLoop != cServMsg.GetSrcMtId() )
                {
                    if (m_ptMtTable->GetMtType(nLoop) == MT_TYPE_MT)
                    {
                        SendMsgToMt( nLoop, MCU_MT_SENDMSG_NOTIF, cServMsg );					
                    }
                    else if(m_ptMtTable->GetMtType(nLoop) == MT_TYPE_SMCU)// xsl [9/26/2006] ֧�ֶ༶����Ϣ�㲥, zgc [04/03/2007] �����ϼ�MCU����
                    {
                        SendMsgToMt( nLoop, MCU_MCU_SENDMSG_NOTIF, cServMsg );
                    }
                }			
			}
		}
		else //����ĳһ�ն� 
		{
			if(tMt.GetMtId() != cServMsg.GetSrcMtId())
            {
                if (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MT)
                {
                    SendMsgToMt( tMt.GetMtId(), MCU_MT_SENDMSG_NOTIF, cServMsg );
                }
                else if (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU)// xsl [9/26/2006] ֧�ֶ༶����Ϣ�㲥, zgc [04/03/2007] �����ϼ�MCU����
                {
                    CServMsg cTmpMsg(pcMsg->content, pcMsg->length);
                    TMt tNullMt;
                    tNullMt.SetNull();
                    cTmpMsg.SetMsgBody((u8*)&tNullMt, sizeof(tNullMt));
                    cTmpMsg.CatMsgBody((cServMsg.GetMsgBody() + sizeof(TMt)), cServMsg.GetMsgBodyLen()-sizeof(TMt));
                    SendMsgToMt( tMt.GetMtId(), MCU_MCU_SENDMSG_NOTIF, cTmpMsg );
                }
            }            
		}
		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*=============================================================================
    �� �� ���� OnMMcuSetIn
    ��    �ܣ� �����¼�Mcu�����ش�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMt &tMt
               u8 byMcsSsnId
               u8 bySwitchMode ������ʽ SWITCH_MODE_BROADCAST SWITCH_MODE_SELECT
               BOOL32 bPolling  �Ƿ�����ѯ��ѡ��SetIn
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/19  3.6			����                  ����
=============================================================================*/
void CMcuVcInst::OnMMcuSetIn(TMt &tMt, u8 byMcsSsnId, u8 bySwitchMode, BOOL32 bPolling)
{
	TSetInParam tSetInParam;
	tSetInParam.m_nViewId = -1145368303;
	tSetInParam.m_bySubFrameIndex = 0;
	tSetInParam.m_tMt = tMt;
	
	CServMsg  cMsg;
	TMcuMcuReq tReq;
	TMcsRegInfo	tMcsReg;
	TMtAlias tMtAlias;
	g_cMcuVcApp.GetMcsRegInfo( byMcsSsnId, &tMcsReg );
	astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
		sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
	astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
		sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));
	cMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));					
	cMsg.CatMsgBody((u8 *)&tSetInParam, sizeof(tSetInParam));

	if( m_ptMtTable->GetDialAlias( tMt.GetMcuId(), &tMtAlias ) && 
		mtAliasTypeH320ID == tMtAlias.m_AliasType )
	{
		//���ܳ������У���֮ǰ�ǹ㲥Դ���㲥Դ�������ͬʱ����ѡ��������״̬��������
		//֪ͨH320���뽻����ʽ
		cMsg.CatMsgBody((u8 *)&bySwitchMode, sizeof(bySwitchMode));
	}

	SendMsgToMt(tMt.GetMcuId(), MCU_MCU_SETIN_REQ, cMsg);
    
	TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMcuId());
	if (ptMcInfo != NULL)
	{
		ptMcInfo->m_tLastMMcuViewMt = ptMcInfo->m_tMMcuViewMt;
		ptMcInfo->m_tMMcuViewMt = tMt;
	}

    // guzh [8/31/2006] �ж��¼��ն��ڱ������Ե����
    // guzh [9/1/2006]  Pollingʱ���л������ˣ��л������˻ᵼ����ѯֹͣ��������Ȼ���л�
    TMt tSpeaker = m_tConf.GetSpeaker();
    if ( tSpeaker.GetType() == TYPE_MT &&
         tMt.GetMcuId() == tSpeaker.GetMcuId() &&
         tMt.GetMtId() != tSpeaker.GetMtId() &&
         !bPolling)
    {
        ChangeSpeaker(&tMt);
    }    
	
	return;
}

/*====================================================================
    ������      ��OnStartMixToSMcu
    ����        ��������MCU��ʼ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CServMsg *pcSerMsg ��Ϣ�ֶ�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/03/7		3.6			Jason          ����
====================================================================*/
void CMcuVcInst::OnStartMixToSMcu(CServMsg *pcSerMsg)
{
	TMcu * ptMcu = (TMcu *)(pcSerMsg->GetMsgBody());
	if(ptMcu->IsLocal())
	{
		return;
	}
	TMt tMcuMt = m_ptMtTable->GetMt( ptMcu->GetMcuId() );
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());
	if(ptMcInfo == NULL)
	{
		return;
	}
	if( ( 0 == ptMcInfo->m_byMcuId ) || 
		( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) )
	{
		return;
	}
	
	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_STARTMIXER_CMD);
	cMsg.SetDstMtId( tMcuMt.GetMtId() );
	
	SendMsgToMt(tMcuMt.GetMtId(), MCU_MCU_STARTMIXER_CMD, cMsg);

	return;
}

/*====================================================================
    ������      ��OnStopMixToSMcu
    ����        ��������MCUֹͣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CServMsg *pcSerMsg ��Ϣ�ֶ�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/03/7		3.6			Jason          ����
====================================================================*/
void CMcuVcInst::OnStopMixToSMcu(CServMsg *pcSerMsg)
{
	TMcu * ptMcu = (TMcu *)(pcSerMsg->GetMsgBody());
	if(ptMcu->IsLocal())
	{
		return;
	}
	TMt tMcuMt = m_ptMtTable->GetMt( ptMcu->GetMcuId() );
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());
	if(ptMcInfo == NULL)
	{
		return;
	}
	if( ( 0 == ptMcInfo->m_byMcuId ) || 
		( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) )
	{
		return;
	}

	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_STOPMIXER_CMD);
	cMsg.SetDstMtId( tMcuMt.GetMtId() );
	
	SendMsgToMt(tMcuMt.GetMtId(), MCU_MCU_STOPMIXER_CMD, cMsg);

	return;
}

/*=============================================================================
    �� �� ���� OnStartDiscussToAllSMcu
    ��    �ܣ� ����������ֱ���¼�MCUȫ�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CServMsg *pcSerMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/28   3.6			����                  ����
=============================================================================*/
void CMcuVcInst::OnStartDiscussToAllSMcu(CServMsg *pcSerMsg)
{
	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_STARTMIXER_CMD);
	
	TConfMcInfo* ptMcInfo = NULL;
	for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
	{
		ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
		if( ( 0 == ptMcInfo->m_byMcuId ) || 
			( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) )
		{
			continue;
		}
		cMsg.SetDstMtId( ptMcInfo->m_byMcuId );
		SendMsgToMt(ptMcInfo->m_byMcuId, MCU_MCU_STARTMIXER_CMD, cMsg);
	}
	
	return;
}

/*=============================================================================
    �� �� ���� OnStopDiscussToAllSMcu
    ��    �ܣ� ����ֹͣ����ֱ���¼�MCUȫ�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CServMsg *pcSerMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/28   3.6			����                  ����
=============================================================================*/
void CMcuVcInst::OnStopDiscussToAllSMcu(CServMsg *pcSerMsg)
{
	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_STOPMIXER_CMD);
	
	TConfMcInfo* ptMcInfo = NULL;
	for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
	{
		ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
		if( ( 0 == ptMcInfo->m_byMcuId ) || 
			( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) )
		{
			continue;
		}
		cMsg.SetDstMtId( ptMcInfo->m_byMcuId );
		SendMsgToMt(ptMcInfo->m_byMcuId, MCU_MCU_STOPMIXER_CMD, cMsg);
	}
	
	return;
}

/*====================================================================
    ������      ��OnGetMixParamToSMcu
    ����        ��������MCU��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CServMsg *pcSerMsg ��Ϣ�ֶ�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/03/7		3.6			Jason          ����
====================================================================*/
void CMcuVcInst::OnGetMixParamToSMcu(CServMsg *pcSerMsg)
{
	TMcu * ptMcu = (TMcu *)(pcSerMsg->GetMsgBody());
	if(ptMcu->IsLocal())
	{
		return;
	}

	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_GETMIXERPARAM_REQ);
	
	if( 0 == ptMcu->GetMcuId() )
	{
		TConfMcInfo* ptMcInfo = NULL;
		for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
		{
			ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
			if( ( 0 == ptMcInfo->m_byMcuId ) || 
				( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) )
			{
				continue;
			}
			cMsg.SetDstMtId( ptMcInfo->m_byMcuId );
			SendMsgToMt(ptMcInfo->m_byMcuId, MCU_MCU_GETMIXERPARAM_REQ, cMsg);
		}
	}
	else
	{
		cMsg.SetDstMtId( ptMcu->GetMcuId() );
		SendMsgToMt(ptMcu->GetMcuId(), MCU_MCU_GETMIXERPARAM_REQ, cMsg);
	}

	return;
}

/*=============================================================================
    �� �� ���� OnAddRemoveMixToSMcu
    ��    �ܣ� ����Ƴ�����mcu������Ա
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CServMsg *pcSerMsg
               BOOL32 bAdd
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/6/2    3.6			����                  ����
=============================================================================*/
void CMcuVcInst::OnAddRemoveMixToSMcu(CServMsg *pcSerMsg, BOOL32 bAdd)
{
	TMcu * ptMcu = (TMcu *)(pcSerMsg->GetMsgBody());
	if(ptMcu->IsLocal())
	{
		return;
	}
	TMt tMcuMt = m_ptMtTable->GetMt( ptMcu->GetMcuId() );
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());
	if(ptMcInfo == NULL)
	{
		return;
	}
	if( ( 0 == ptMcInfo->m_byMcuId ) || 
		( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) )
	{
		return;
	}

    // xsl [7/27/2006] ���¼�mcuû�вμӻ�����������������
    if (!m_ptMtTable->IsMtInMixing(tMcuMt.GetMtId()))
    {
        AddRemoveSpecMixMember(&tMcuMt, 1, TRUE);
    }
	
	CServMsg cMsg;
	cMsg.SetServMsg( pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen() );
	cMsg.SetDstMtId( tMcuMt.GetMtId() );
	if( TRUE == bAdd )
	{
		cMsg.SetEventId( MCU_MCU_ADDMIXMEMBER_CMD );	
		SendMsgToMt(tMcuMt.GetMtId(), MCU_MCU_ADDMIXMEMBER_CMD, cMsg);
	}
	else
	{
		cMsg.SetEventId( MCU_MCU_REMOVEMIXMEMBER_CMD );	
		SendMsgToMt(tMcuMt.GetMtId(), MCU_MCU_REMOVEMIXMEMBER_CMD, cMsg);
	}

	return;
}

// END OF FILE
