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
#include "rpctrl.h"
//#include "mcuerrcode.h"

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
        ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] mt can't create conf in n+1 mode!\n");
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
	u8       byVendorId = *(u8*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)+sizeof(u16)));
    u16      wConfFirstRate = 0;
    u16      wConfSecondRate = 0;

    BOOL32   bLowLevelMcuCalledIn = ( pcMsg->event == MT_MCU_LOWERCALLEDIN_NTF );
	
	TConfInfo *ptConfInfo = NULL;
	if (cServMsg.GetMsgBodyLen() > 4*sizeof(TMtAlias)+3*sizeof(u8)+sizeof(u16))
	{
		ptConfInfo = (TConfInfo *)(cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+3*sizeof(u8)+sizeof(u16));
	}
	
	m_tConfEx.Clear();

	//TConfInfo֮�����ݿ��ʽ��
    //u8��TConfInfo��׷�����ݿ������ + u16����һ�鳤�ȣ�+ u8��AduioType��+ u8����������+ ... ...(����˳��׷���������ݿ�)
	if(ptConfInfo != NULL 
		&& ptConfInfo->HasConfExInfo()
		&& cServMsg.GetMsgBodyLen() > 4*sizeof(TMtAlias)+3*sizeof(u8)+sizeof(u16)+sizeof(TConfInfo))
	{
		u8 byAudioType = *(cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+3*sizeof(u8)+sizeof(u16)+sizeof(TConfInfo)+sizeof(u8)+sizeof(u16));
		u8 byAudioChnnNum = *(cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+3*sizeof(u8)+sizeof(u16)+sizeof(TConfInfo)+sizeof(u8)+sizeof(u16)+sizeof(u8));

		TCapSupport tCap = ptConfInfo->GetCapSupport();
		TSimCapSet tSim = tCap.GetMainSimCapSet();
		tSim.SetAudioMediaType(byAudioType);
		tCap.SetMainSimCapSet(tSim);
		ptConfInfo->SetCapSupport(tCap);

		m_tConfEx.Clear();
		TAudioTypeDesc tAudioType(byAudioType,byAudioChnnNum);
		//m_tConfEx.SetMainAudioTypeDesc(tAudioType);
		m_tConfEx.SetAudioTypeDesc(&tAudioType,1);
	}

	cServMsg.SetSrcMtId(CONF_CREATE_MT);
    if (bLowLevelMcuCalledIn)
    {
        // guzh [6/19/2007] ���Ϊ�¼�MCU
        cServMsg.SetSrcMtId(CONF_CREATE_SMCU);
    }

    TLocalInfo tLocalInfo;
    g_cMcuAgent.GetLocalInfo(&tLocalInfo);

	const u8 byE164NumLen = strlen(tLocalInfo.GetE164Num());
	const u8 byConfAliasLen = strlen(tConfAlias.m_achAlias);
    u8 byCmpLen = max(byE164NumLen, byConfAliasLen);
    
    // zbq [06/23/2007] �ն˴���: 1��������E164��� �� ��ģ��E164ͨ��ģ�崴�᣻2����MCU����/E164 ��������
    if ( mtAliasTypeE164 == tConfAlias.m_AliasType &&
         0 != memcmp( tLocalInfo.GetE164Num(), tConfAlias.m_achAlias, byCmpLen ) )
    {
        u8 byTemplateConfIdx = g_cMcuVcApp.GetTemConfIdxByE164(tConfAlias.m_achAlias);
	    u8 byOnGoingConfIdx  = g_cMcuVcApp.GetOngoingConfIdxByE164(tConfAlias.m_achAlias);

        ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] byTemplateConfIdx.%d byOnGoingConfIdx.%d\n", 
            byTemplateConfIdx, byOnGoingConfIdx);
  
	    if (byOnGoingConfIdx > 0)  //�ն˺��л���
	    {
			// vcs����/mcs���鲻�ܻ���
			CConfId cConfId = g_cMcuVcApp.GetConfIdByE164(tConfAlias.m_achAlias, FALSE , TRUE);
			if( byType != TYPE_MT && cConfId.GetConfSource() != cServMsg.GetConfId().GetConfSource() 
				&& byVendorId == MT_MANU_KDCMCU)
			{
				cServMsg.SetErrorCode( ERR_MCU_CALLMCUERROR_CONFISHOLDING );
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "VcsConf And McsConf Don't Connect!\n" );
				return;
			}

			
			CMcuVcInst *pInstance = g_cMcuVcApp.GetConfInstHandle(byOnGoingConfIdx);

			if( TYPE_MCU == byType && pInstance != NULL && pInstance->IsInOtherConf() //&&
				/*MCS_CONF == pInstance->m_tConf.GetConfSource()*/ )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] conf not support Cascade and in other conf!!!\n");
				cServMsg.SetErrorCode( ERR_MCU_CASADEBYOTHERHIGHLEVELMCU );
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
				return;
			}
		
			if (!bLowLevelMcuCalledIn)
			{
				//CConfId cConfId = g_cMcuVcApp.GetConfIdByE164(tConfAlias.m_achAlias, FALSE);
						
				//zjj20100113�Ѿ����������������Ļ��鲻���ٱ�����
				//zjj20110316vcs����ͬ������
				if( TYPE_MCU == byType && pInstance != NULL && !pInstance->m_tCascadeMMCU.IsNull() //&&
					/*MCS_CONF == pInstance->m_tConf.GetConfSource()*/ )
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] conf has Cascade by a high adminLevel mcu, so can't Cascade by other mcu!!!\n");
					cServMsg.SetErrorCode( ERR_MCU_CASADEBYOTHERHIGHLEVELMCU );
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



				// ����VCS�������飬���¼�����ϯ�ѱ����ȣ���������м���
				if (!cConfId.IsNull() && VCS_CONF == cConfId.GetConfSource() && TYPE_MCU == byType)
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] subconf has been created by other vcs, restore conf\n");
	// 				cServMsg.SetErrorCode(ERR_MCU_VCS_SMCUINVC);
	//              g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
	// 			    return;
					g_cMcuVcApp.SendMsgToConf( byOnGoingConfIdx, MCUVC_MMCUGETCTRL_CMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
				}
		  
			}
			else
			{
				TConfInfo    *ptTempConfInfo = NULL;
				TConfMtTable *ptConfMtTable = NULL;
				TConfAllMtInfo *ptConfAllMtInfo = NULL;
				
				//zbq[08/23/2008] ������������Ȩ�޽綨
				if (NULL == pInstance/*g_cMcuVcApp.GetConfInstHandle(byOnGoingConfIdx)*/)
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] get handle of conf.%d failed\n", byOnGoingConfIdx);
					return;
				}
				
				ptTempConfInfo = &pInstance->m_tConf;//&g_cMcuVcApp.GetConfInstHandle(byOnGoingConfIdx)->m_tConf;
				ptConfMtTable = g_cMcuVcApp.GetConfMtTable(byOnGoingConfIdx);
				ptConfAllMtInfo = &pInstance->m_tConfAllMtInfo;//&g_cMcuVcApp.GetConfInstHandle(byOnGoingConfIdx)->m_tConfAllMtInfo;
				
				if( byEncrypt == 0 && 
					ptTempConfInfo->GetConfAttrb().GetEncryptMode() != CONF_ENCRYPTMODE_NONE )
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Mt 0x%x join conf %s request was refused because encrypt!\n", 
						tMtAddr.m_tTransportAddr.GetIpAddr(), ptTempConfInfo->GetConfName() );
					return;
					
				}
				
				u8 byMtId = 0;
				//��ջ���, ���䲻�������ն�, �ܾ�
				if(ptTempConfInfo->GetConfAttrb().GetOpenMode() == CONF_OPENMODE_CLOSED )
				{
					byMtId = ptConfMtTable->GetMtIdByAlias( &tMtAddr );
					if( !( byMtId > 0 && ptConfAllMtInfo->MtInConf( byMtId ) ) )
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Mt 0x%x join conf %s request was refused because conf is closed!\n", 
							tMtAddr.m_tTransportAddr.GetIpAddr(), ptTempConfInfo->GetConfName() );
						return;
					}
				}
				
				// guzh [6/19/2007] �¼�MCU�������У���Ϊ�����¼�MCU�������
				TAddMtInfo tAddMtInfo;
				cServMsg.SetConfIdx(byOnGoingConfIdx);            
				cServMsg.SetConfId(g_cMcuVcApp.GetConfId(byOnGoingConfIdx));
				cServMsg.SetSrcMtId(0);
				cServMsg.SetSrcSsnId(0);
				if ( !tMtE164Alias.IsNull() )
				{
					memcpy(&tAddMtInfo, &tMtE164Alias, sizeof(TMtAlias));
				}
				else
				{
					memcpy(&tAddMtInfo, &tMtAddr, sizeof(TMtAlias));
				}
				tAddMtInfo.SetCallMode(CONF_CALLMODE_NONE);
				tAddMtInfo.SetCallBitRate(wCallRate);//mtadp������Ϣʵ�����������͸�ֵ[1/16/2013 chendaiwei]
				TMcu tLocalMcu;
				tLocalMcu.SetMcu(LOCAL_MCUID);
				cServMsg.SetMsgBody((u8*)&tLocalMcu, sizeof(TMcu));
				cServMsg.CatMsgBody((u8*)&tAddMtInfo, sizeof(TAddMtInfo));
				// xliang [8/29/2008] 
				cServMsg.CatMsgBody((u8*)&bLowLevelMcuCalledIn,sizeof(BOOL32));
				g_cMcuVcApp.SendMsgToConf( byOnGoingConfIdx, MCS_MCU_ADDMT_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			}
  
        }
	    else if (byTemplateConfIdx > 0)  //���л���ģ��,��������
	    {
			// vcs����/mcs���鲻�ܻ���
			CConfId cConfId = g_cMcuVcApp.GetConfIdByE164(tConfAlias.m_achAlias, TRUE,FALSE);
			if( byType != TYPE_MT && cConfId.GetConfSource() != cServMsg.GetConfId().GetConfSource()
				&& byVendorId == MT_MANU_KDCMCU)
			{
				cServMsg.SetErrorCode( ERR_MCU_CALLMCUERROR_CONFISHOLDING );
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "VcsConf And McsConf Don't Connect!\n" );
				return;
			}

            //�û��鿪ʼ
		    cServMsg.SetConfIdx(byTemplateConfIdx);
		    TTemplateInfo tTemConf;
		    if (!g_cMcuVcApp.GetTemplate(byTemplateConfIdx, tTemConf))
		    {
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Get Template %d failed!\n", byTemplateConfIdx);
                g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }
			
			//vcs����ܾ��ն˴���
			if (TYPE_MT == byType && VCS_CONF == tTemConf.m_tConfInfo.GetConfSource())
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] VCSConf do not support Mt Creat Conf!\n");
                g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
				return;
			}

		    //��������
		    if (0 == byEncrypt && 
			    CONF_ENCRYPTMODE_NONE != tTemConf.m_tConfInfo.GetConfAttrb().GetEncryptMode())
		    {
				cServMsg.SetErrorCode( ERR_MCU_SETENCRYPT );
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Template encrypt!\n");
			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }

            //  xsl [2/13/2006] ��CreateConf����������ն˳ɹ����ack
            //SendReplyBack(cServMsg, cServMsg.GetEventId()+1);

		    tTemConf.m_tConfInfo.m_tStatus.SetOngoing();
		    tTemConf.m_tConfInfo.SetStartTime( time(NULL) );    
		    tTemConf.m_tConfInfo.m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );

			//////////////////////////////////////////////////////////////////////////			
			//����ģ��ʧ������Ҫ�ϱ��ϼ�mcu����ͬʱ�ϱ�����mcu����mcs�����������벻ͬ���ڽ�������ʾ��ͬ��Ϣ
			if( TYPE_MCU == byType && MCS_CONF == tTemConf.m_tConfInfo.GetConfSource() &&
				g_cMcuVcApp.IsConfNumOverCap( TRUE, tTemConf.m_tConfInfo.GetConfSource() )
				)
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "[DaemonProcMtMcuCreateConfReq] mcs on-going conf num has passed max(%d)\n",
							g_cMcuVcApp.GetMaxMcsOngoingConfNum() );
				cServMsg.SetErrorCode( ERR_MCU_CALLMCUERROR_CONFISHOLDING );
			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

				//cServMsg.SetErrorCode( ERR_MCU_MMCUCALLREFUSE );
				//SendMsgToAllMcs(  MCU_MCS_CREATECONF_BYTEMPLATE_NACK ,cServMsg );
				NotifyMcsAlarmInfo( 0,ERR_MCU_MMCUCALLREFUSE );
				return;
			}
			//////////////////////////////////////////////////////////////////////////

		    wCallRate -= GetAudioBitrate( tTemConf.m_tConfInfo.GetMainAudioMediaType() );

		    TMtAlias atMtAlias[MAXNUM_CONF_MT+1];
            u16      awMtDialBitRate[MAXNUM_CONF_MT+1] = {0};
		    u8       byMtNumInUse = 0;
		    
            // zbq [09/19/2007] ��ģ�崴�ᣬ���ָ��ն���ԭģ���еĺ���ģʽ����; ��ԭΪIP,ע�Ᵽ���˿�.
		    //atMtAlias[0] = tMtAddr;
			//zjj20100114 �����Ҫע��gk���ͺ��¼���164��			
			if( g_cMcuAgent.GetGkIpAddr() != 0 )
			{				
				if( mtAliasTypeE164 == tMtE164Alias.m_AliasType &&
						0 != strcmp( tMtE164Alias.m_achAlias,"" )
						)
				{
					atMtAlias[0] = tMtE164Alias;
				}
				else if( mtAliasTypeH323ID == tMtH323Alias.m_AliasType &&
					0 != strcmp( tMtH323Alias.m_achAlias,"" )
					)
				{
					atMtAlias[0] = tMtH323Alias;
				}
				else
				{
					atMtAlias[0] = tMtAddr;
				}			
			}
			else
			{
				atMtAlias[0] = tMtAddr;				
			}
		    awMtDialBitRate[0] = wCallRate;
		    byMtNumInUse = 1;

		    // ˢ���ն��б�����ǽ�ͻ���ϳ�ģ��
            TMultiTvWallModule tNewTvwallModule = tTemConf.m_tMultiTvWallModule;
            TVmpModule         tNewVmpModule    = tTemConf.m_atVmpModule;

			TConfInfoEx tConfInfoEx;
			THduVmpModuleOrTHDTvWall tHduVmpModule;
			TVmpModuleInfo tVmpModuleEx25;
			u16 wPackConfExInfoLength = 0;
			BOOL32 bUnkownConfInfo = FALSE;
			UnPackConfInfoEx(tConfInfoEx,tTemConf.m_byConInfoExBuf, wPackConfExInfoLength,
						bUnkownConfInfo, &tHduVmpModule, &tVmpModuleEx25);
            
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
				else if (mtAliasTypeE164 == tTemConf.m_atMtAlias[byLoop].m_AliasType)
				{
					if (0 == memcmp(tTemConf.m_atMtAlias[byLoop].m_achAlias, tMtE164Alias.m_achAlias, MAXLEN_ALIAS))
					{
						bInMtTable = TRUE;
						bRepeatThisLoop = TRUE;

						// zbq [09/19/2007] ȡ�����ĺ���ģʽ
						atMtAlias[0] = tMtE164Alias;
					}
				}
				else if (mtAliasTypeH323ID == tTemConf.m_atMtAlias[byLoop].m_AliasType)
				{
					if (0 == memcmp(tTemConf.m_atMtAlias[byLoop].m_achAlias, tMtH323Alias.m_achAlias, MAXLEN_ALIAS))
					{
						bInMtTable = TRUE;
						bRepeatThisLoop = TRUE;

						// zbq [09/19/2007] ȡ�����ĺ���ģʽ
						atMtAlias[0] = tMtH323Alias;
					}
				}
				else if (puAliasTypeIPPlusAlias == tTemConf.m_atMtAlias[byLoop].m_AliasType)
				{
					u32 dwNetIpAddr = *(u32*)tTemConf.m_atMtAlias[byLoop].m_achAlias; //������
					TMtAlias tTmpAlias;
					memcpy(tTmpAlias.m_achAlias,&tTemConf.m_atMtAlias[byLoop].m_achAlias[sizeof(dwNetIpAddr)],strlen(tTemConf.m_atMtAlias[byLoop].m_achAlias)-sizeof(dwNetIpAddr));
					
					if(tMtAddr.m_tTransportAddr.m_dwIpAddr == dwNetIpAddr 
						&& (strcmp(tMtH323Alias.m_achAlias,tTmpAlias.m_achAlias) == 0
						|| strcmp(tMtE164Alias.m_achAlias,tTmpAlias.m_achAlias) == 0)
						)
					{
						bInMtTable = TRUE;
						bRepeatThisLoop = TRUE;
						atMtAlias[0] = tTemConf.m_atMtAlias[byLoop];
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
                                      tNewTvwallModule, tNewVmpModule,tHduVmpModule,tVmpModuleEx25);
		    }
            
		    //���鿪��ģʽ
		    //����Ŀ���ģʽ
		    if ( CONF_OPENMODE_CLOSED  != tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() &&
			     CONF_OPENMODE_NEEDPWD != tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() &&
			     CONF_OPENMODE_OPEN    != tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() ) 
		    {
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Wrong Open Mode!\n");
			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }
		    //������
		    if ( CONF_OPENMODE_CLOSED == tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() )
		    {
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Open Mode is CLOSE!\n");
			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }
			// xliang [12/26/2008] (modify for MT calling MCU initially) 
			//cancel limit here, but need to input password later
		    //��������������
// 		    else if ( CONF_OPENMODE_NEEDPWD == tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() &&
// 				      !bInMtTable )
// 		    {
//                 ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Conf Need Password!\n");
// 			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
// 			    return;
// 		    }
            // ���������б��Ҳ��������ַ��
//             if (!bInMtTable && 
//                 (byType != TYPE_MCU) &&   // MCU �򲻿���IP�Ƿ������
//                 !g_cMcuVcApp.IsMtIpInAllowSeg( tTemConf.m_tConfInfo.GetUsrGrpId(), tMtAddr.m_tTransportAddr.GetNetSeqIpAddr()))
//             {
//                 ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Type is not MCU(type=%d), neither in Allow IP range(0x%x)!\n", 
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


			//����ģ���е�auto���ٷֱ��ʣ�ת��Ϊ��ʵ���õķֱ���cif
			if (VIDEO_FORMAT_AUTO == tTemConf.m_tConfInfo.GetMainVideoFormat() &&
				MEDIA_TYPE_H264   == tTemConf.m_tConfInfo.GetMainVideoMediaType())
			{
				tTemConf.m_tConfInfo.SetMainVideoFormat(VIDEO_FORMAT_CIF);
			}

			if (VIDEO_FORMAT_AUTO == tTemConf.m_tConfInfo.GetSecVideoFormat() &&
				MEDIA_TYPE_H264   == tTemConf.m_tConfInfo.GetSecVideoMediaType())
			{
				tTemConf.m_tConfInfo.SetSecVideoFormat(VIDEO_FORMAT_CIF);
			}
		    
		    cServMsg.SetMsgBody((u8 *)&tTemConf.m_tConfInfo, sizeof(TConfInfo));
		    cServMsg.CatMsgBody((u8 *)&wAliasBufLen, sizeof(wAliasBufLen));
		    cServMsg.CatMsgBody((u8 *)achAliasBuf, ntohs(wAliasBufLen));
			TConfAttrb tConfAttrb = tTemConf.m_tConfInfo.GetConfAttrb();
		    if (tConfAttrb.IsHasTvWallModule())
		    {
			    cServMsg.CatMsgBody((u8*)&tNewTvwallModule, sizeof(TMultiTvWallModule));
		    }
		    if (tConfAttrb.IsHasVmpModule())
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

				//�¼�hdunumĬ������Ϊ0
				u8 byHduChnlNum = 0;
				cServMsg.CatMsgBody(&byHduChnlNum, sizeof(u8));
		
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
				// �����˷���		
				// ������Ϣ����ģ����Ϣ�и���
				tTemConf.m_byMTPackExist = FALSE;
				cServMsg.CatMsgBody((u8*)&tTemConf.m_byMTPackExist, sizeof(u8));
				cServMsg.CatMsgBody((u8*)&tTemConf.m_tVCSBackupChairMan, sizeof(TMtAlias));
				// [4/20/2011 xliang] VCAutoMode
				cServMsg.CatMsgBody((u8*)&tTemConf.m_byVCAutoMode, sizeof(u8));
			}
			
			//���������HDUvmpģ��ͺ�5���vmpģ����[5/30/2013 chendaiwei]
			u16 wConfInfoExLength = 0;
			PackConfInfoEx(tConfInfoEx, &tTemConf.m_byConInfoExBuf[0],
				wConfInfoExLength,&tHduVmpModule,&tVmpModuleEx25);
			if( wConfInfoExLength > CONFINFO_EX_BUFFER_LENGTH )
			{
				ConfPrint(LOG_LVL_WARNING,MID_MCU_CONF,"[DaemonProcMtMcuCreateConfReq] conf.%s ConfInfEx.%s > CONFINFO_EX_BUFFER_LENGTH, error!\n",tTemConf.m_tConfInfo.GetConfName(),wConfInfoExLength);
				
				return;
			}
			else
			{
				cServMsg.CatMsgBody((u8*)&tTemConf.m_byConInfoExBuf[0],wConfInfoExLength);
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
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] VCSConf start command to inst%d\n",
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
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] assign instance id failed!\n");
            }
        }
        else
        {
            cServMsg.SetErrorCode( ERR_MCU_NULLCID );
            g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
            ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] UNEXIST E164.%s to be joined or created !\n", tConfAlias.m_achAlias);
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
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Conference %s failure because encrypt setting is confused - byEncrypt.%d EncryptMode.%d!\n", 
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
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Conference %s failure because create mt has no Alias(GK exist), ignore it!\n", ptConfInfo->GetConfName());
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
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Conference %s failure because create mt has no Alias, ignore it!\n", ptConfInfo->GetConfName());
                return;                
            }
        }
        
		// vcs����/mcs���鲻�ܻ���
		if( byType != TYPE_MT && ptConfInfo->GetConfId().GetConfSource() != cServMsg.GetConfId().GetConfSource()
			&& byVendorId == MT_MANU_KDCMCU)
		{
			cServMsg.SetErrorCode( ERR_MCU_CALLMCUERROR_CONFISHOLDING );
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "VcsConf And McsConf Don't Connect!\n" );
			return;
		}

		//�ն˱��������� 
		// xliang [1/16/2009] ����Ƿ�Ҫ���ϻ���ģ����Ϣ�е������ն�----����
		PackTMtAliasArray(&tMtCreateAlias, &wCallRate, 1, achAliasBuf, wAliasBufLen);
		wAliasBufLen = htons(wAliasBufLen);

		TConfInfo tTmpConfInfo;
		memcpy(&tTmpConfInfo,ptConfInfo,sizeof(TConfInfo));

		//���ն�bug,��264��ʽ֡��Ϊ0,���ڷ�264����25֡��,����һ��
		if( tTmpConfInfo.GetMainSimCapSet().GetVideoMediaType() != MEDIA_TYPE_H264 &&
			tTmpConfInfo.GetMainSimCapSet().GetVideoFrameRate() == 0 )
		{			
			//��264��25֡Ϊ2�������mtadputils.cpp��FrameRateIn2Out����,��VIDEO_FPS_25�Ķ���
			tTmpConfInfo.SetMainVidFrameRate(2);
		}
		TVmpModule tTmpVmpModule;
		//�������ն˴�����ϳɴ��ᣬ��vmp��Ϣ��д��vmpparam�У�����vmpmodule��[1/24/2013 chendaiwei]
		if(!ptConfInfo->GetConfAttrb().IsHasVmpModule() 
			&& ptConfInfo->m_tStatus.GetVmpParam().GetVMPMode() == CONF_VMPMODE_AUTO)
		{
			TVMPParam tTmpVmpParam;
			memset( &tTmpVmpParam, 0 ,sizeof(tTmpVmpParam) );
			tTmpConfInfo.m_tStatus.SetVmpParam(tTmpVmpParam);//clear
			
			tTmpVmpParam.SetVMPAuto(TRUE);
			tTmpVmpParam.SetVMPBrdst(TRUE);
			tTmpVmpModule.SetVmpParam(tTmpVmpParam);
			tTmpConfInfo.SetHasVmpModule(TRUE);
		}
		
		cServMsg.SetMsgBody((u8 *)&tTmpConfInfo, sizeof(TConfInfo));
		cServMsg.CatMsgBody((u8*)&wAliasBufLen, sizeof(wAliasBufLen));
		cServMsg.CatMsgBody((u8*)achAliasBuf, ntohs(wAliasBufLen));
		//������ϳ�ģ�� 
		if( tTmpConfInfo.GetConfAttrb().IsHasVmpModule())
		{
			cServMsg.CatMsgBody((u8*)&tTmpVmpModule, sizeof(tTmpVmpModule));
		}

		u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
		u16 wPackDataLen = 0;
		PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
		cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);

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
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] mt create conf, insId is %d\n", byInsID);
        }
        else
        {
            /*ConfLog(FALSE, "[DaemonProcMtMcuCreateConfReq] assign instance id failed!\n");*/
            ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] assign instance id failed!\n");
        }
		
	}
	else  //�������
	{
		// vcs����/mcs���鲻�ܻ���
		CConfId cConfId = g_cMcuVcApp.GetConfIdByName(tConfAlias.m_achAlias, FALSE,TRUE);
		if( byType != TYPE_MT && cConfId.GetConfSource() != cServMsg.GetConfId().GetConfSource()
			&& byVendorId == MT_MANU_KDCMCU)
		{
			cServMsg.SetErrorCode( ERR_MCU_CALLMCUERROR_CONFISHOLDING );
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "VcsConf And McsConf Don't Connect!\n" );
			return;
		}

		cServMsg.SetMsgBody((u8*)&tMtH323Alias, sizeof(tMtH323Alias));
		cServMsg.CatMsgBody((u8*)&tMtE164Alias, sizeof(tMtE164Alias));
		cServMsg.CatMsgBody((u8*)&tMtAddr, sizeof(tMtAddr));
		cServMsg.CatMsgBody((u8*)&tConfAlias, sizeof(tConfAlias));
		cServMsg.CatMsgBody(&byType, sizeof(byType));
		cServMsg.CatMsgBody(&byEncrypt, sizeof(byEncrypt));
		cServMsg.CatMsgBody((u8*)&wCallRate, sizeof(wCallRate));
        // guzh [6/19/2007] 
        cServMsg.CatMsgBody((u8*)&bLowLevelMcuCalledIn, sizeof(bLowLevelMcuCalledIn));
		g_cMcuVcApp.SendMsgToDaemonConf( MT_MCU_MTJOINCONF_REQ, cServMsg.GetServMsg(),cServMsg.GetServMsgLen() );
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "send MT_MCU_MTJOINCONF_REQ to daemon conf\n");
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
	s32 nPwdLen = cServMsg.GetMsgBodyLen() - sizeof(BOOL32);
	BOOL32 bSupportMultCascade = *(BOOL32 *)(cServMsg.GetMsgBody()+ nPwdLen);

	if( tMt.IsNull() )
	{
		return;
	}

	switch( CurState() )
	{
	case STATE_ONGOING:
		if ( m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()) || 
			 TRUE == DealMtMcuEnterPwdRsp(tMt, pszPwd, nPwdLen) )
		{
		
			// [7/9/2010 xliang] ��¼�Ƿ�֧�ֶ༶��
			if( bSupportMultCascade )
			{

				u8 bySmcuId = cServMsg.GetSrcMtId();
				u16 wMcuIdx = GetMcuIdxFromMcuId( bySmcuId );
				m_ptConfOtherMcTable->SetMcuSupMultCas( wMcuIdx );
			
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Mcu.(%d,%d) support multiple cascade!\n", tMt.GetMcuId(), tMt.GetMtId());
			}

			TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.m_tLocalMtInfo;
			tConfMtInfo.SetMcuIdx( LOCAL_MCUID );
			cServMsg.SetMsgBody((u8 *)&tConfMtInfo, sizeof(TConfMtInfo));
			SendMsgToMt( tMt.GetMtId(), MCU_MCU_ROSTER_NOTIF,  cServMsg);
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "Recv MCU_MCU_REGISTER_NOTIF, Sending MCU_MCU_ROSTER_NOTIF!\n");

			//lukunpeng 2010/07/08 �����Ƿ�֧�ֶ�ش��ж�
			if( IsSupportMultiSpy() )
			{
				//lukunpeng 2010/06/10 ע�⣬�޸ĵ���Ϣ�壬֪ͨ������֧�ֵ���������
				u32 dwMaxSpyBW = m_tConf.GetSndSpyBandWidth();
				dwMaxSpyBW = htonl( dwMaxSpyBW );
				cServMsg.SetMsgBody((u8 *)&dwMaxSpyBW, sizeof(dwMaxSpyBW));
				SendMsgToMt( tMt.GetMtId(), MCU_MCU_MULTSPYCAP_NOTIF,  cServMsg);
				
				SendConfExtInfoToMcs( 0,tMt.GetMtId() );				
			}	
			// End
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "Recv MCU_MCU_REGISTER_NOTIF but ConfPwd-Validate Err and drop mt.%d\n", tMt.GetMtId());
		}

        //����Ӧ�¼�MCU�ļ���ͨ���򿪴������㣬��Ϊ�򿪳ɹ� [01/11/2006-zbq]
//        m_abyCasChnCheckTimes[tMt.GetMtId()-1] = 0;	
//		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "Recv MCU_MCU_REGISTER_NOTIF, m_abyCasChnCheckTimes[%d]=0 !\n", tMt.GetMtId()-1);
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MMCU, "Wrong message %u(%s) received in state %u!\n", 
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
	u8 byLastRosterNotify = *(u8*)(cServMsg.GetMsgBody() + sizeof(TConfMtInfo));

	u8 byMcuId = (u8)ptConfMtInfo->GetMcuIdx();	
	u16 wMcuIdx = 0;
	switch( CurState() )
	{
	case STATE_ONGOING:
		wMcuIdx = GetMcuIdxFromMcuId( byMcuId );
		if( INVALID_MCUIDX == wMcuIdx )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuRosterNotify] Fail To Get McuIdx Info.McuId.%d\n",
				byMcuId
				);
			return;
		}
		ptConfMtInfo->SetMcuIdx( wMcuIdx );
		
		//ˢ��TConfMtInfo
		m_tConfAllMtInfo.SetMtInfo( *ptConfMtInfo );

		//�����ն��б�
		if (byLastRosterNotify &&
			(MT_TYPE_SMCU == m_ptMtTable->GetMtType(byMcuId) ||
			 (MT_TYPE_MMCU == m_ptMtTable->GetMtType(byMcuId) &&
			  g_cMcuVcApp.IsShowMMcuMtList())))
		{
			TMcuMcuReq tReq;
			memset(&tReq, 0, sizeof(tReq));

			CServMsg	cTempServMsg;
			cTempServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
			SendMsgToMt( tMt.GetMtId(), MCU_MCU_MTLIST_REQ,  cTempServMsg);
			
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "Recv Last MCU_MCU_ROSTER_NOTIF and send MCU_MCU_MTLIST_REQ to mt.%d\n", tMt.GetMtId());
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "Recv MCU_MCU_ROSTER_NOTIF without send MCU_MCU_MTLIST_REQ \n");
		}


		//������ػ��������ն���Ϣ
		//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
		//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );

		// [12/9/2010 liuxu][�߶������ֶ�����ǽ]cServMsg�п��ܱ�MCU_MCU_MTLIST_REQ�ĵ���...
		SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MMCU, "Wrong message %u(%s) received in state %u!\n", 
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
	BOOL32 bIsMMcuReq = FALSE;
	
	STATECHECK;		

	if( !m_tCascadeMMCU.IsNull() &&
		tMt.GetMtId() == m_tCascadeMMCU.GetMtId() )
	{
		bIsMMcuReq = TRUE;
	}

	//�����ն��б�
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tMt.GetMtId()));
	LogPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuMtListReq]Recv MtList_REQ from Mcu.%d McuIdx.%d\n", tMt.GetMtId(), GetMcuIdxFromMcuId(tMt.GetMtId()));
    // ���� [5/31/2006] MtIdѭ��Ӧ����1-192
	for(u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
	{
		tTmpMt = m_ptMtTable->GetMt(byLoop);
		if(!tTmpMt.IsLocal())
		{
			continue;
		}
		tTmpMt.SetMcuIdx( LOCAL_MCUID );
		atInfo[byInfoNum].m_tMt = tTmpMt;
		
		TMtAlias tTmpMtAlias;
		m_ptMtTable->GetMtAlias(tTmpMt.GetMtId(), mtAliasTypeE164, &tTmpMtAlias);

		TMt tLocalMt = m_ptMtTable->GetMt(byLoop);
		// ����ǵ绰�նˣ�����ȡE164��
		if (IsPhoneMt(tLocalMt) &&!tTmpMtAlias.IsAliasNull())
		{
			strncpy((s8 *)(atInfo[byInfoNum].m_szMtName), tTmpMtAlias.m_achAlias, sizeof(atInfo[byInfoNum].m_szMtName) - 1);
		}
		else if ( m_ptMtTable->GetMtAlias(tTmpMt.GetMtId(), mtAliasTypeH323ID, &tTmpMtAlias))
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcuMcuMtListReq]copy 323ID to Mt.%d\n", byLoop);
			strncpy((s8 *)(atInfo[byInfoNum].m_szMtName), tTmpMtAlias.m_achAlias, sizeof(atInfo[byInfoNum].m_szMtName) - 1);
		}
		else
		{
			strncpy((s8 *)(atInfo[byInfoNum].m_szMtName), m_ptMtTable->GetMtAliasFromExt(byLoop), sizeof(atInfo[byInfoNum].m_szMtName) - 1);
		}

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
		atInfo[byInfoNum].m_byIsConnected     = m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMtId()) ? 1:0;
		atInfo[byInfoNum].m_byIsFECCEnable    = m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_H224DATA, &tChannel, TRUE ); 
	
		atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount = 0;
		if(tTmpMt == m_tConf.GetSpeaker())
		{
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_nViewID = m_dwSpeakerViewId;
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_bySubframeIndex = 0;
			atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount++;
		}
		TVMPParam tVmpParam = m_tConf.GetStatus().GetVmpParam();
		if(tVmpParam.IsMtInMember(tTmpMt))
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

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuMtListReq] Notified McuID.%d LocalTotalMtNum.%d\n", tMt.GetMtId(), byInfoNum );

	u8 byLastPack = 1;
	cServMsg.SetMsgBody(&byLastPack, sizeof(u8));
	cServMsg.CatMsgBody((u8 *)atInfo, byInfoNum * sizeof(TMcuMcuMtInfo));
	SendMsgToMt( tMt.GetMtId(), MCU_MCU_MTLIST_ACK,  cServMsg); //������

	//�����ϼ�MCU��ĵ�һ���¼�MCU�����ն�״̬ǿ��֪ͨ��֮��ĸ����ն�״̬����ʵʱ֪ͨ
	if( m_tConfInStatus.IsNtfMtStatus2MMcu() ||
		// vcsˢ�б�ͬʱ��Ҫˢ����״̬��Ϣ���������а�����Ҫ���ն˺���ģʽ��Ϣ
		VCS_CONF == m_tConf.GetConfSource())
	{
		OnNtfMtStatusToMMcu( m_tCascadeMMCU.GetMtId() );
		m_tConfInStatus.SetNtfMtStatus2MMcu(FALSE);
	}

	if( bIsMMcuReq )
	{
		SendMtListToMcs(INVALID_MCUIDX, FALSE, TRUE);// ���¼��� [pengguofeng 7/10/2013]
// 		SendAllSMcuMtInfoToMMcu();
// 		SendAllSMcuMtStatusToMMcu(); // ʱ�����⣬�������յ�mtadp����Ϣ֮�����ϱ��������ϱ��Ӳ���ȥmcuidx [pengguofeng 7/26/2013]
// 		SendSMcuUpLoadMtToMMcu();
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
void CMcuVcInst::GetConfViewInfo(u16 wMcuIdx, TCConfViewInfo &tInfo)
{
    u8 byViewIndex = 0;
    tInfo.m_byDefViewIndex = 0;
    
    TCapSupport tCapSupport = m_tConf.GetCapSupport();

    //spy view 
    TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
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
		tInfo.m_atViewInfo[byViewIndex].m_atMts[0].SetMcuId( LOCAL_MCUID );
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
        //tInfo.m_atViewInfo[byViewIndex].m_atMts[0] = m_tConf.GetSpeaker();

		TMt tSpeaker = m_tConf.GetSpeaker();
		BuildMultiCascadeMtInfo( tSpeaker,
						tInfo.m_atViewInfo[byViewIndex].m_atMts[0]
						);
        
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuVideoInfoReq] Speaker byViewIndex.%d McuId.%d MtId.%d\n", 
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
    if(g_cMcuVcApp.GetVMPMode(m_tVmpEqp) != CONF_VMPMODE_NONE)
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
                tInfo.m_atViewInfo[byViewIndex].m_atMts[tInfo.m_atViewInfo[byViewIndex].m_byMtCount].SetMt((TMt)(*tVMPParam.GetVmpMember(nIndex)));
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
    
    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuVideoInfoReq] byViewCount.%d\n", byViewIndex );
    
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
    GetConfViewInfo( GetMcuIdxFromMcuId(tMt.GetMtId()), tInfo );

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

	TConfMcInfo*	ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tMt.GetMtId())); 
	if(ptConfMcInfo == NULL)
	{

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuVideoInfoAck]   ptConfMcInfo is NULL \n" );
		return;
	}
	memcpy(&(ptConfMcInfo->m_tConfViewInfo), ptConfViewInfo, sizeof(TCConfViewInfo));

	/*ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuVideoInfoAck] Mcu(%d) lastSpeaker(%d,%d) now speaker(%d,%d)  videoinfo m_byViewCount:%d SpyMt(%d,%d)\n", tMt.GetMtId(),
							 ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMcuId(),
							ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId(),
							ptConfViewInfo->m_atViewInfo[1].m_atMts[0].GetMcuId(),
							ptConfViewInfo->m_atViewInfo[1].m_atMts[0].GetMtId(),
							ptConfViewInfo->m_byViewCount,
							ptConfMcInfo->m_tSpyMt.GetMcuId(),ptConfMcInfo->m_tSpyMt.GetMtId()
							);			
		

	
	if( tMt == m_tCascadeMMCU && ptConfViewInfo->m_byViewCount > 1 &&
		ptConfViewInfo->m_atViewInfo[1].m_byMtCount > 0 &&
		ptConfViewInfo->m_atViewInfo[1].m_atMts[0] == m_tCascadeMMCU )
	{
		
		//zjj[08/13/2009]����ϴ��ϼ�mcu�Ϸ����������¼�mcu���ն˺���ε��¼�mcu�ķ����˲�ͬʱ
		//(���ϼ�mcuȡ�������ˣ���������������¼�mcu���ն�)
		if( ptConfMcInfo->m_tConfAudioInfo.m_byMixerCount > 0 &&
			ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.IsLocal() &&
			ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId() != ptConfViewInfo->m_atViewInfo[1].m_atMts[0].GetMtId())
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuVideoInfoAck] Mcu(%d) MMCU Speaker is cancel. \
				Now speaker is m_tCascadeMMCU \n",
				tMt.GetMtId() );

			u8 byMtId = ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId();

			//�����ϼ�mcu�������ش�Դ����Ƶ����
			//��ΪҪ���ִ���ͬ������ش��ն˵���Ƶ��bas����ô��ƵҲ��bas������ֱ�ӵĽ���
			TLogicalChannel tVidChn;
			if( IsHDConf(m_tConf) && m_cMtRcvGrp.IsMtNeedAdp(byMtId) &&							  
					m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_VIDEO, &tVidChn, TRUE) &&
					!IsNeedAdapt(GetLocalAudBrdSrc(), m_ptMtTable->GetMt(byMtId), MODE_AUDIO))
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "Mcu(%d) Start bas To Mt(%d,%d) Audio Switch\n",tMt.GetMtId(),
							m_ptMtTable->GetMt(byMtId).GetMcuId(),
							m_ptMtTable->GetMt(byMtId).GetMtId()
									);
				//continue;
				StartSwitchAud2MtNeedAdp( GetLocalAudBrdSrc(), m_ptMtTable->GetMt(byMtId));
			}
			else
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "Mcu(%d) Start m_tAudBrdSrc To Mt(%d,%d) Audio Switch\n",tMt.GetMtId(),
							m_ptMtTable->GetMt(byMtId).GetMcuId(),
							m_ptMtTable->GetMt(byMtId).GetMtId()
									);
				StartSwitchToSubMt( GetLocalAudBrdSrc(), 0, byMtId, MODE_AUDIO );
			}

			

			ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker = ptConfViewInfo->m_atViewInfo[1].m_atMts[0];
			
		}
		else
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuVideoInfoAck] Mcu(%d) MMCU Speaker not cancel lastSpeaker(%d,%d) now speaker(%d,%d)  audioinfo mixercount:%d SpyMt(%d,%d)\n", tMt.GetMtId(),
							 ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMcuId(),
							ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId(),
							ptConfViewInfo->m_atViewInfo[1].m_atMts[0].GetMcuId(),
							ptConfViewInfo->m_atViewInfo[1].m_atMts[0].GetMtId(),
							ptConfMcInfo->m_tConfAudioInfo.m_byMixerCount,
							ptConfMcInfo->m_tSpyMt.GetMcuId(),ptConfMcInfo->m_tSpyMt.GetMtId()
							);			
		}
		
	}
	*/

	//////////////////////////////////////////////////////////////////////////
	
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

	TConfMcInfo*	ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tMt.GetMtId())); 
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
	
	TCConfAudioInfo* ptConfAudioInfo = (TCConfAudioInfo *)(cServMsg.GetMsgBody());
	if (NULL == ptConfAudioInfo)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck]   Rcv ptConfAudioInfo is NULL \n" );
		return;
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck] Rcv AudioInfo Mcu<McuId.%d, MtId.%d>, MixerCount.%d, Speaker<McuId.%d, MtId.%d>\n",
								tMt.GetMcuId(), tMt.GetMtId(),
								ptConfAudioInfo->m_byMixerCount,
								ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId(),
								ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId());

	TConfMcInfo* ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tMt.GetMtId())); 
	if(ptConfMcInfo == NULL)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck]   ptConfMcInfo is NULL \n" );
		return;
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck] Last AudioInfo MixerCount.%d, Speaker<McuId.%d, MtId.%d>, SpyMt<McuId.%d, MtId.%d>, MMcuViewMt<McuId.%d, MtId.%d>!\n",
								ptConfMcInfo->m_tConfAudioInfo.m_byMixerCount,
								ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMcuId(),
								ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId(),
								ptConfMcInfo->m_tSpyMt.GetMcuId(),
								ptConfMcInfo->m_tSpyMt.GetMtId(),
								ptConfMcInfo->m_tMMcuViewMt.GetMcuId(),
								ptConfMcInfo->m_tMMcuViewMt.GetMtId());

	TMt tLastSpeaker;
	tLastSpeaker.SetNull();
	if( ptConfMcInfo->m_tConfAudioInfo.m_byMixerCount >0 )
	{
		tLastSpeaker = ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker;

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Mcu(%d) Last Speaker: (%d,%d)\n", tMt.GetMtId(),
							      tLastSpeaker.GetMcuId(),
							      tLastSpeaker.GetMtId());
	}
	
	memcpy(&(ptConfMcInfo->m_tConfAudioInfo), ptConfAudioInfo, sizeof(TCConfAudioInfo));

	TMt tSpeaker;
	tSpeaker.SetNull();

	u16 wMMcuIdx = INVALID_MCUIDX;
	if( !m_tCascadeMMCU.IsNull() && 
		(wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() )) != INVALID_MCUIDX &&
		IsLocalAndSMcuSupMultSpy( wMMcuIdx ) 
		)
	{
		//����ϼ����¼�mcu�������ˣ����¼����ϴ��ն��������ˡ�
		if (ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId() ==  m_tCascadeMMCU.GetMtId() 
			&& ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId() == 0)
		{
			tSpeaker = tLastSpeaker;//ptConfMcInfo->m_tSpyMt;
		}
		//����ǷǱ����ն����ϼ������ˣ���ԭΪ���ϼ������ˡ�
		else if (!ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.IsMcuIdLocal())
		{
			tSpeaker = tLastSpeaker;
		}
		else
		{
			//�Ǳ����ն���������
			tSpeaker = ptConfAudioInfo->m_tMixerList[0].m_tSpeaker;
		}
	}
	else
	{
		tSpeaker = ptConfMcInfo->m_tSpyMt;
		tSpeaker.SetMcuId( LOCAL_MCUID );
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck] Mcu(%d) Remote Speaker(AudioBrdSrc): (%d,%d) m_byMixerCount(%d) ptConfMcInfo->m_tMMcuViewMt(%d,%d)\n", tMt.GetMtId(),
				ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId(),
				ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId(),
				ptConfAudioInfo->m_byMixerCount,
				tSpeaker.GetMcuId(),
				tSpeaker.GetMtId()
			);

	TMt tMtLocalSpeaker = tSpeaker;
	tMtLocalSpeaker.SetMcuId( LOCAL_MCUIDX );
	TMt tMtLocalLastSpeaker = tLastSpeaker;
	tMtLocalLastSpeaker.SetMcuId( LOCAL_MCUIDX );

    if ( tMt == m_tCascadeMMCU && GetLocalAudBrdSrc() == m_tCascadeMMCU )
    {
		TLogicalChannel tVidChn;
		TSwitchGrp tSwitchGrp;
		//20111009 zjl �µ�ý��Դ�Ǳ����ն�+��һ��ý��Դ���¼�mcu���߷Ǳ����նˣ�Ҫͨ����һ��ý��Դ��ǰ�µ�ý��Դ
		if( tSpeaker.IsMcuIdLocal())
		{
			if (!tLastSpeaker.IsMcuIdLocal())
			{
				SendMsgToMt( (u8)tLastSpeaker.GetMcuId(), MCU_MCU_AUDIOINFO_ACK, cServMsg );
			} 
			/*20111021 zjl �����һ�������˺͵�ǰ�ķ����˶��Ǳ���������ͬһ��mcu, ����Ҫ������ͨ�棬
					       ����ͨ���Ŀ�����ڣ���ǰ�ķ������Ǳ����ġ��նˡ�����һ�����������¼�mcu,
						   ������һ��������<192, n>�ķ����ˣ��¼��յ���ת��<n, 0>����Ϊ���ϼ�mcu��������
						   �¼��Ჹ������һ�������˵Ľ���*/
			else if(tLastSpeaker.IsMcuIdLocal() && 
				    !(tLastSpeaker == tSpeaker) &&
					MT_TYPE_SMCU == m_ptMtTable->GetMtType(tLastSpeaker.GetMtId()))
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck] Notify Smcu start switch to its cur spymt with aud!\n");
				SendMsgToMt( (u8)tLastSpeaker.GetMtId(), MCU_MCU_AUDIOINFO_ACK, cServMsg );
			}			
		}

		//20110727 zjl ��ǰ�ϼ�ý��ԴΪ�������ն�����Ϣ͸����������������Ǳ���mcu����ȡ�ϴ��ն���Ϣ͸�����¼�
		if( ptConfAudioInfo->m_byMixerCount > 0)
		{
			if (!ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.IsMcuIdLocal() && 
				MT_TYPE_SMCU == m_ptMtTable->GetMtType((u8)ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId()))
			{
				SendMsgToMt( (u8)ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId(), 
								 MCU_MCU_AUDIOINFO_ACK, cServMsg );
			}
			else if (ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.IsMcuIdLocal() && 
				MT_TYPE_SMCU == m_ptMtTable->GetMtType((u8)ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId()))
			{
				TCConfAudioInfo tConfAudioInfo = *(TCConfAudioInfo *)(cServMsg.GetMsgBody());
				CServMsg cMsg;
				TMsgHeadMsg tHeadMsg;
				u16 wSmcuIdx = GetMcuIdxFromMcuId( tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId());
				TConfMcInfo * ptMcuMcInfo = m_ptConfOtherMcTable->GetMcInfo( wSmcuIdx );
				if( NULL != ptMcuMcInfo && !ptMcuMcInfo->m_tMMcuViewMt.IsNull() ) 
				{
					tHeadMsg.m_tMsgSrc = BuildMultiCascadeMtInfo( ptMcuMcInfo->m_tMMcuViewMt, tConfAudioInfo.m_tMixerList[0].m_tSpeaker);
				}
				cMsg.SetEventId(MCU_MCU_AUDIOINFO_ACK);
				cMsg.SetMsgBody((u8 *)&tConfAudioInfo, sizeof(tConfAudioInfo));
				cMsg.CatMsgBody((u8 *)&tHeadMsg, sizeof(tHeadMsg));
				SendMsgToMt( (u8)ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId(), 
								 MCU_MCU_AUDIOINFO_ACK, cMsg );
			}	
		}
		
		// guzh [5/14/2007] �ж��ϼ��Ĺ㲥Դ�Ƿ��Ǳ����Ļش��նˣ��ϼ�mcu���¼�mcu��MTΪ�����˻�����жϣ�
        if ( ptConfAudioInfo->m_byMixerCount > 0 &&
             ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.IsMcuIdLocal())
        {
			if (MT_TYPE_MMCU == m_ptMtTable->GetMtType( ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId()))
			{
				if ( ptConfAudioInfo->m_byMixerCount >= 1)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck] New Speaker is mmcu!\n");
					
					if( g_cMcuVcApp.IsVASimultaneous() && IsNeedAdapt(GetLocalVidBrdSrc(), tMtLocalSpeaker, MODE_VIDEO) &&							  
							m_ptMtTable->GetMtLogicChnnl(tSpeaker.GetMtId(), LOGCHL_VIDEO, &tVidChn, TRUE) &&
							!IsNeedAdapt(GetLocalAudBrdSrc(), tMtLocalSpeaker, MODE_AUDIO) 
							)
					{
						StartSwitchAud2MtNeedAdp( GetAudBrdSrc(), tMtLocalSpeaker);
					}
					else
					{
						RestoreRcvMediaBrdSrc(1,&tMtLocalSpeaker,MODE_AUDIO);
					}
					
				 }
			}
			else if(MT_TYPE_SMCU != m_ptMtTable->GetMtType( ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId()))
			{
				 if ( ptConfAudioInfo->m_byMixerCount > 1 )
				 {
					if( g_cMcuVcApp.IsVASimultaneous() && IsNeedAdapt(GetLocalVidBrdSrc(), tMtLocalSpeaker, MODE_VIDEO) &&							  
						m_ptMtTable->GetMtLogicChnnl(tSpeaker.GetMtId(), LOGCHL_VIDEO, &tVidChn, TRUE) &&
						!IsNeedAdapt(GetLocalAudBrdSrc(), tMtLocalSpeaker, MODE_AUDIO) 
						)
					{
						StartSwitchAud2MtNeedAdp( GetAudBrdSrc(), tMtLocalSpeaker);
					}
					else
					{
						RestoreRcvMediaBrdSrc(1,&tMtLocalSpeaker,MODE_AUDIO);
					}
				 }
				 else
				 {
					 TMtStatus tStatus;
					 m_ptMtTable->GetMtStatus( tSpeaker.GetMtId(),&tStatus );
					 
					 if( !tStatus.GetSelectMt(MODE_AUDIO).IsNull() )
					 {
						 ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuAudioInfoAck] Mt(%d) is global speaker,but now sel other mt audio so cann't stop switch to it.\n",
							 tSpeaker.GetMtId()
							 );
					 }
					 else
					 {
						 if (tSpeaker.IsMcuIdLocal() && MT_TYPE_SMCU != m_ptMtTable->GetMtType(tSpeaker.GetMtId()))
						 {
							StopSwitchToSubMt(1, &tMtLocalSpeaker, MODE_AUDIO);
						 }				 
					 }
				  }
			 }	
	
			//zjj [29/08/2009] ����ϴε��ϼ��Ĺ㲥Դ�Ǳ����ն˵�����£��ȽϺͱ����ش��ն��Ƿ�ͬ��
			//�����ͬ�ͻָ������㲥Դ�����ϼ�MCU�����ϴη����˵Ľ���(��Ƶ)
			if( !tLastSpeaker.IsNull() && 
				tSpeaker.GetMcuId() == tLastSpeaker.GetMcuId() &&
				tSpeaker.GetMtId() != tLastSpeaker.GetMtId() 
				)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Mcu(%d) Last Speaker: Restore last speaker audio switch\n", tMt.GetMtId());
				TMtStatus tStatus;
				m_ptMtTable->GetMtStatus( tLastSpeaker.GetMtId(),&tStatus );

				if( !tStatus.GetSelectMt(MODE_AUDIO).IsNull() )
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuAudioInfoAck] Mt(%d) is canceled global speaker,but now sel other mt audio so cann't start switch to it.\n",
						tLastSpeaker.GetMtId());
				}
				else
				{
					//�����ϼ�mcu���ϴη����˵���Ƶ����
					//��ΪҪ���ִ���ͬ������ش��ն˵���Ƶ��bas����ô��ƵҲ��bas������ֱ�ӵĽ���					
					if( g_cMcuVcApp.IsVASimultaneous() && IsNeedAdapt(GetLocalVidBrdSrc(), tMtLocalLastSpeaker, MODE_VIDEO) &&							  
							m_ptMtTable->GetMtLogicChnnl(tLastSpeaker.GetMtId(), LOGCHL_VIDEO, &tVidChn, TRUE) &&
							!IsNeedAdapt(GetLocalAudBrdSrc(), tMtLocalLastSpeaker, MODE_AUDIO) 
							)
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "Mcu(%d) Start bas To Mt(%d,%d) Audio Switch\n",tMt.GetMtId(),
									m_ptMtTable->GetMt(tLastSpeaker.GetMtId()).GetMcuId(),
									m_ptMtTable->GetMt(tLastSpeaker.GetMtId()).GetMtId()
											);
						//continue;
						//StartSwitchAud2MtNeedAdp( tLastSpeaker.GetMtId() );
						StartSwitchAud2MtNeedAdp( GetAudBrdSrc(), tMtLocalLastSpeaker);
					}
					else
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "Mcu(%d) Start m_tAudBrdSrc To Mt(%d,%d) Audio Switch\n",tMt.GetMtId(),
									m_ptMtTable->GetMt(tLastSpeaker.GetMtId()).GetMcuId(),
									m_ptMtTable->GetMt(tLastSpeaker.GetMtId()).GetMtId()
											);
						RestoreRcvMediaBrdSrc(1,&tMtLocalLastSpeaker,MODE_AUDIO);
					}
				}				
				ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.SetMtId( tSpeaker.GetMtId() );
			}	           
        }
        else
        {
			//����ϼ���ȫ�ַ����˲��Ǳ������ն˾�ȡ���ش�Դ������Ƶ������
			//�������ϴα�����ȫ�ַ����˵���Ƶ�������ٽ��ش�Դ���ÿ�
			if( ptConfAudioInfo->m_byMixerCount > 0 &&
				LOCAL_MCUID != ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId() )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck]Mcu(%d) Now Speaker(%d,%d) is mmcu's Speaker,Cancel SpyMt(%d,%d) \n", tMt.GetMtId(),
							 ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId(),
							 ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId(),
							 tSpeaker.GetMcuId(),
							 tSpeaker.GetMtId()
							 );				

				//�ش�Դ��Ϊ�գ����ش�Դ�Ļش�������Ƶ������������������ش�Դ��Ƶ
				if( !tSpeaker.IsNull() && tSpeaker.IsMcuIdLocal() )
				{				
					if( MT_TYPE_SMCU == m_ptMtTable->GetMtType(tSpeaker.GetMtId()) )
					{
						if( ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId() != tSpeaker.GetMtId() ||
							0 == ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId() 
							)
						{
							ptConfAudioInfo->m_tMixerList[0].m_tSpeaker = tSpeaker;
						}							
						SendMsgToMt( tSpeaker.GetMtId(), MCU_MCU_AUDIOINFO_ACK, cServMsg );		
					}
					else
					{
						TMtStatus tStatus;
						m_ptMtTable->GetMtStatus( tSpeaker.GetMtId(),&tStatus );

						if( !tStatus.GetSelectMt(MODE_AUDIO).IsNull() )
						{
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuAudioInfoAck] Mt(%d) is canceled global speaker,but now sel other mt audio so cann't start switch to it.\n",
								tSpeaker.GetMtId());
						}
						else
						{
							//�����ϼ�mcu���ϴη����˵���Ƶ����
							//��ΪҪ���ִ���ͬ������ش��ն˵���Ƶ��bas����ô��ƵҲ��bas������ֱ�ӵĽ���							
							if( g_cMcuVcApp.IsVASimultaneous() && IsNeedAdapt(GetLocalVidBrdSrc(), tMtLocalSpeaker, MODE_VIDEO) &&							  
									m_ptMtTable->GetMtLogicChnnl(tSpeaker.GetMtId(), LOGCHL_VIDEO, &tVidChn, TRUE) &&
									!IsNeedAdapt(GetLocalAudBrdSrc(), tMtLocalSpeaker, MODE_AUDIO) 
									)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "Mcu(%d) Start bas To Mt(%d,%d) Audio Switch\n",tMt.GetMtId(),
											m_ptMtTable->GetMt(tSpeaker.GetMtId()).GetMcuId(),
											m_ptMtTable->GetMt(tSpeaker.GetMtId()).GetMtId()
													);
								
								//continue;
								//StartSwitchAud2MtNeedAdp( tSpeaker.GetMtId() );
								StartSwitchAud2MtNeedAdp( GetAudBrdSrc(), tMtLocalSpeaker);
							}	
							else
							{
								RestoreRcvMediaBrdSrc(1,&tMtLocalSpeaker,MODE_AUDIO);
							}
						}
					}					
				}
				if( !tSpeaker.IsNull() && !tSpeaker.IsMcuIdLocal() )
				{
					if ( ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId() ==  m_tCascadeMMCU.GetMtId() 
							&& ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId() == 0 )
					{
						ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.SetMcuId( LOCAL_MCUID );
						ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.SetMtId( (u8)tSpeaker.GetMcuId() );
					}
					SendMsgToMt( (u8)tSpeaker.GetMcuId(), MCU_MCU_AUDIOINFO_ACK, cServMsg );			
				}
			}
		}
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck]Mcu(%d) tMt(%d,%d) m_tAudBrdSrc(%d,%d) m_tCascadeMMCU(%d,%d)\n", tMt.GetMtId(),
			tMt.GetMcuId(),tMt.GetMtId(),
			GetAudBrdSrc().GetMcuId(),GetAudBrdSrc().GetMtId(),
			m_tCascadeMMCU.GetMcuId(),m_tCascadeMMCU.GetMtId()
			);

		if( tMt == m_tCascadeMMCU && tSpeaker == tLastSpeaker && !tSpeaker.IsNull() && !tSpeaker.IsMcuIdLocal() )
		{		
			if ( ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId() ==  m_tCascadeMMCU.GetMtId() 
					&& ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId() == 0 )
			{
				ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.SetMcuId( LOCAL_MCUID );
				ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.SetMtId( (u8)tSpeaker.GetMcuId() );
			}
			SendMsgToMt( (u8)tSpeaker.GetMcuId(), MCU_MCU_AUDIOINFO_ACK, cServMsg );				
		}
	}

	
	
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
void CMcuVcInst::GetLocalAudioInfo(TCConfAudioInfo &tAudioInfo,TMsgHeadMsg &tHeadMsg)
{
    tAudioInfo.m_byMixerCount = 0;
    tAudioInfo.m_byDefMixerIndex = 0;
    tAudioInfo.m_tMixerList[0].m_tSpeaker.SetNull();
	TMt tSpeaker = m_tConf.GetSpeaker();
    if(!m_tConf.GetSpeaker().IsNull())
    {
		// [pengjie 2010/12/15] ������¼�mcu���ԣ�����ȡ���ϴ�Դ֪ͨ�¼�
		if( tSpeaker.IsLocal() && (tSpeaker.GetMtType() == MT_TYPE_SMCU) )
		{
			u16 wSmcuIdx = GetMcuIdxFromMcuId( tSpeaker.GetMtId() );
			TConfMcInfo * ptMcuMcInfo = m_ptConfOtherMcTable->GetMcInfo( wSmcuIdx );
			if( NULL != ptMcuMcInfo && !ptMcuMcInfo->m_tMMcuViewMt.IsNull() ) 
			{
				tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_nMixerID = m_dwSpeakerAudioId;
				tHeadMsg.m_tMsgSrc = BuildMultiCascadeMtInfo( ptMcuMcInfo->m_tMMcuViewMt, \
					tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker);
			}
		}
		// End
		else
		{
			tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_nMixerID = m_dwSpeakerAudioId;
			tHeadMsg.m_tMsgSrc = BuildMultiCascadeMtInfo( tSpeaker,
				tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker);
		}

        //tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = m_tConf.GetSpeaker();
		//zjj 20090903�����¼�mcu audioinfo ackʱ���¼��ķ�������Ϣ��local�ľ�����
		//�¼�mcu��mtadp��ת���¼�mcu�ı����ն���Ϣ������������δ���ע��
        //if ( !m_tConf.GetSpeaker().IsLocal() )
        //{
            
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
		
			//u8 byMcuId = m_tConf.GetSpeaker().GetMcuId();
            //TMt tRealSpeaker = GetMcuMediaSrc(byMcuId);
            //if (!tRealSpeaker.IsNull())
            //{
            //    tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = tRealSpeaker;
            //}  
        //}
		
        tAudioInfo.m_byMixerCount++;
    }
    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[GetLocalAudioInfo] Local Speaker: (%d, %d)\n", 
        tAudioInfo.m_tMixerList[0].m_tSpeaker.GetMcuId(), 
        tAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId() );
    
    if(m_tConf.GetStatus().IsMixing())
    {
        tAudioInfo.m_byDefMixerIndex = tAudioInfo.m_byMixerCount;
        
        //zbq[11/01/2007] �����Ż�
        //TMixParam tParam = m_tConf.GetStatus().GetMixParam();
        //tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = tParam.m_atMtMember[0];
        tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_nMixerID = m_dwMixerAudioId;
		tSpeaker = GetLocalSpeaker();
		tSpeaker = GetMcuIdMtFromMcuIdxMt( tSpeaker );
        tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = tSpeaker;//GetLocalSpeaker();
        tAudioInfo.m_byMixerCount++;
        
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[GetLocalAudioInfo] Local Mixer: (%d, %d)\n", 
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

	if( !m_tCascadeMMCU.IsNull() &&
		!m_tConf.GetStatus().IsMixing() &&
		m_tConf.GetSpeaker() == m_tCascadeMMCU )
	{
		return;
	}
	TCConfAudioInfo tAudioInfo; 
	TMsgHeadMsg tHeadMsg;
    GetLocalAudioInfo( tAudioInfo,tHeadMsg );

	cServMsg.SetMsgBody((u8 *)&tAudioInfo, sizeof(tAudioInfo));
	cServMsg.CatMsgBody((u8 *)&tHeadMsg, sizeof(tHeadMsg));
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
	//u8 byMcuId = 0,bySMcuId = 0;
	u8 abyMcuId[MAX_CASCADEDEPTH-1];
	memset( &abyMcuId[0],0,sizeof(abyMcuId) );
    if ( MODE_AUDIO == byMode || MODE_BOTH == byMode )
    {
        TCConfAudioInfo tAudioInfo; 
		TMsgHeadMsg tHeadMsg;
        GetLocalAudioInfo( tAudioInfo,tHeadMsg );  
        cServMsg.SetEventId(MCU_MCU_AUDIOINFO_ACK);
        cServMsg.SetMsgBody((u8 *)&tAudioInfo, sizeof(tAudioInfo));
		cServMsg.CatMsgBody((u8 *)&tHeadMsg, sizeof(tHeadMsg));
        
        u16 wMcuIdx = 0;
        for( u16 wLoop = 0; wLoop < m_tConfAllMtInfo.GetMaxMcuNum(); wLoop++ )
        {
            wMcuIdx = m_tConfAllMtInfo.GetMtInfo(wLoop).GetMcuIdx();
			if( IsValidMcuId(wMcuIdx) 
				&& m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) 
				&& m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0])
				)
			{
				if( abyMcuId[0] != 0 
					&& m_tCascadeMMCU.GetMtId() != abyMcuId[0] 
					&& (byTargetMcuId == 0 || byTargetMcuId == abyMcuId[0] ) )
				{
					SendMsgToMt( abyMcuId[0], MCU_MCU_AUDIOINFO_ACK, cServMsg );
				}
			}
        }
    }
    if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
    {
        cServMsg.SetEventId(MCU_MCU_VIDEOINFO_ACK);
        TCConfViewInfo tInfo;
        u16 wMcuIdx = 0;

		// liuxu, m_tConfAllMtInfo����������޸�
        for( u16 wLoop = 0; wLoop < m_tConfAllMtInfo.GetMaxMcuNum(); wLoop++ )
        {
            wMcuIdx = m_tConfAllMtInfo.GetMtInfo(wLoop).GetMcuIdx();
			
			if( IsValidSubMcuId(wMcuIdx) &&
				m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) &&
				m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] )
				)
			{
				if( abyMcuId[0] != 0 
					&& m_tCascadeMMCU.GetMtId() != abyMcuId[0] 
					&& (byTargetMcuId == 0 || byTargetMcuId == abyMcuId[0] ) )
				{
					GetConfViewInfo(wMcuIdx, tInfo);
					cServMsg.SetMsgBody((u8 *)&tInfo, sizeof(tInfo));
					SendMsgToMt( abyMcuId[0], MCU_MCU_VIDEOINFO_ACK, cServMsg );
				}
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
	TMsgHeadMsg tHeadMsg;
    GetLocalAudioInfo(tAudioInfo,tHeadMsg);
    cMsg.SetEventId(MCU_MCU_AUDIOINFO_ACK);
    cMsg.SetMsgBody((u8 *)&tAudioInfo, sizeof(tAudioInfo));
    SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_AUDIOINFO_ACK, cMsg );
    
    TCConfViewInfo tVideoInfo;                
    GetConfViewInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()), tVideoInfo);
    cMsg.SetEventId(MCU_MCU_VIDEOINFO_ACK);
    cMsg.SetMsgBody((u8 *)&tVideoInfo, sizeof(tVideoInfo));
    SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_VIDEOINFO_ACK, cMsg );
}

/*====================================================================
    ������      ��ProcMcuMcuMtOperateRsp
    ����        �������ն˺��У��Ҷϣ�ɾ�������룬����ش��ն���Ӧ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/21    3.6         Jason         ����
====================================================================*/
void CMcuVcInst::ProcMcuMcuMtOperateRsp( const CMessage *pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMsgHeadMsg tHeadMsg = *( TMsgHeadMsg* )(cServMsg.GetMsgBody());

	STATECHECK;

	if( !m_tCascadeMMCU.IsNull() &&
		tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
		tHeadMsg.m_tMsgDst.m_byCasLevel > 0
		)
	{
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),cServMsg.GetEventId(),cServMsg );
	}

}
/*====================================================================
    ������      SendAllMtInfoToAllMcs
    ����        ������Է�mcu�ն��б��Ӧ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/21    3.6         Jason         ����
	10/12/30    4.6         liuxu         �޸�
====================================================================*/
/*lint -save -e850*/
void CMcuVcInst::SendAllMtInfoToAllMcs( u16 wEvent, CServMsg & cServMsg, const BOOL32 bForce/* = FALSE*/ )
{
	if( MCU_MCS_CONFALLMTINFO_NOTIF != wEvent )
	{
		return;
	}
	
	if (CurState() != STATE_ONGOING)
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "SendAllMtInfoToAllMcs called at wrong Vc state.%d, return\n", CurState());
		return;
	}

	// ��SendAllMtInfoToAllMcs��ȡ���巢��
	if ( g_cMcuVcApp.GetMcuMcsMtInfoBufFlag() != 0					// �����˻��巢��
		&& !bForce)
	{
		// [11/14/2011 liuxu] ���ڴ�����ʱSendAllMtInfoToAllMcs��Ϣ��̫��, �����ȡ���巢��
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MCS, "Buf Send AllMtInfoToAllMcs\n");

		if (m_byAllMtToAllMcsTimerFlag)
		{
			//SetTimer(MCUVC_SENDALLMT_ALLMCS_TIMER, 500);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "SendAllMtInfoToAllMcs wait timer, return\n");
			return;
		}else
		{
			m_byAllMtToAllMcsTimerFlag = 1;
			u16 wTemp = SetTimer(MCUVC_SENDALLMT_ALLMCS_TIMER, g_cMcuVcApp.GetRefreshMtInfoInterval());
			return;
		}
	}else
	{
		KillTimer(MCUVC_SENDALLMT_ALLMCS_TIMER);
		m_byAllMtToAllMcsTimerFlag = 0;
	}
	
	u8 byPos = 0;
	cServMsg.SetMsgBody( ( u8 * )&byPos,sizeof(byPos) );
	cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ));
	cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo.m_tLocalMtInfo, sizeof( TConfMtInfo ) );
	cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo.m_tMMCU, sizeof( TMt ) );
	//cServMsg.CatMsgBody( (u8*)&m_tConfAllMtInfo.m_atOtherMcMtInfo[wLoop],sizeof(TConfMtInfo) * byPackSize );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF,cServMsg );
// 	m_tConfAllMtInfo.Print(); // test [pengguofeng 7/8/2013]
	++byPos;
	
	const u16 wMaxPackSize = 100;						// �ְ�����, ÿ�������100��TConfMtInfo
	u16 wSendPackSize = 0;								// ʵ�ʷ��͵�TConfMtInfo�ĸ���

	u16 wLoop = 0;
	const u16 wMaxMcuNum = m_tConfAllMtInfo.GetMaxMcuNum();
	if( wMaxMcuNum > wMaxPackSize )
	{
		for( wLoop = 0; wLoop < wMaxMcuNum; wLoop += wMaxPackSize )
		{
			// ���ı��
			cServMsg.SetMsgBody( ( u8 * )&byPos, sizeof(byPos) );
			
			// TConfMtInfo����ʼλ��
			wLoop = htons(wLoop);
			cServMsg.CatMsgBody( ( u8 * )&wLoop, sizeof(u16) );
			
			// ����ʵ�ʷ��͵İ���
			wLoop = ntohs(wLoop);
			wSendPackSize = wMaxMcuNum - wLoop;
			if( wSendPackSize >= wMaxPackSize )
			{
				wSendPackSize = wMaxPackSize;
			}		

			// ��ָ�뱣��
			if (NULL == m_tConfAllMtInfo.GetMtInfoPtr(wLoop))
			{
				break;
			}

			cServMsg.CatMsgBody( (u8*)m_tConfAllMtInfo.GetMtInfoPtr(wLoop), sizeof(TConfMtInfo) * wSendPackSize );
			SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF,cServMsg );
			
			++byPos;
		}
	}
	else
	{
		cServMsg.SetMsgBody( ( u8 * )&byPos,sizeof(byPos) );
		wLoop = htons(wLoop);
		cServMsg.CatMsgBody( ( u8 * )&wLoop,sizeof(u16) );
		cServMsg.CatMsgBody( (u8*)m_tConfAllMtInfo.GetMtInfoPtr(0),sizeof(TConfMtInfo) * wMaxMcuNum );
		SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF,cServMsg );
	}

	// ���ͻ��� // ���ö෢ [pengguofeng 7/5/2013]
/*	u8 abyMcuIdx[2] = {0};
	for ( u16 wMcuIdx = 0; wMcuIdx < MAXNUM_CONFSUB_MCU; wMcuIdx++)
	{
		if ( m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuIdx[0] )) 
			SendMtListToMcs(wMcuIdx, TRUE);
	}

 	SendMtListToMcs(LOCAL_MCUIDX, TRUE);
*/
/*
	if (m_cMcuMtListSendBuf.GetUsedNum())
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "Start Buf Send m_cMcuMtListSendBuf(num:%d)\n", m_cMcuMtListSendBuf.GetUsedNum());

		u16 wMcuIdx = INVALID_MCUIDX;		
		const u32 dwMaxBufNum = m_cMcuMtListSendBuf.GetCapacity();
		
		for ( u32 dwLoop = 0; dwLoop < dwMaxBufNum; dwLoop++ )
		{
			if(!m_cMcuMtListSendBuf.Get(dwLoop, wMcuIdx))
				continue;
			
			// ��ʱ��mcu�ᶨʱˢ�����б����ն˵�״̬, ��ͨ���ڻ���������һ����Tmt��ǵ�
			// ͬһ�������ն��п��ܱ�ˢ�¶��, ��Ӱ�����ܺ͹���
			if (IsValidMcuId(wMcuIdx)) 
			{
				SendMtListToMcs(wMcuIdx, TRUE);
				continue;
			}
		}
		
		m_cMcuMtListSendBuf.ClearAll();
	}
	else
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MCS, "[SendAllMtInfoToAllMcs]m_cMcuMtListSendBuf.GetUsedNum() == 0\n");
	}
*/
}
/*lint -restore*/

/*====================================================================
    ������      ��SendAllSMcuMtStatusToMMcu
    ����        �����������¼��ն�״̬���ϼ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/08/17    4.6         �ܾ���         ����
====================================================================*/
void CMcuVcInst::SendAllSMcuMtStatusToMMcu( void )
{
	if( m_tCascadeMMCU.IsNull() || !IsMMcuSupportMultiCascade() )
	{
		return;
	}
	u8 byLoop;
	u16 wMcuIdx = INVALID_MCUIDX;
	u8 byMcuId = 0;
	for( byLoop = 1;byLoop <= MAXNUM_CONF_MT;++byLoop )
	{
		if( MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) )
		{
			byMcuId = byLoop;
			if( m_tConfAllMcuInfo.GetIdxByMcuId(&byMcuId,1,&wMcuIdx) )
			{
				SendSMcuMtStatusToMMcu( wMcuIdx );
			}
		}
	}
}

/*====================================================================
    ������      ��SendSMcuMtStatusToMMcu
    ����        ������ĳ���¼��ն�״̬���ϼ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/08/17    4.6         �ܾ���         ����
====================================================================*/
void CMcuVcInst::SendSMcuMtStatusToMMcu( u16 wMcuIdx )
{
	if( m_tCascadeMMCU.IsNull() || !IsMMcuSupportMultiCascade() )
	{
		return;
	}
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	memset( &abyMcuId[0],0,sizeof(abyMcuId) );

	if( !m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0]) )
	{
		return;
	}

	if( 0 == abyMcuId[0] || 0 != abyMcuId[1] )
	{
		return;
	}


	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
	if( NULL == ptMcInfo )
	{
		return;
	}

	s32 nIndex = 0;
	TSMcuMtStatus tSMcuMtStatus;
	u8 byMcuId = GetFstMcuIdFromMcuIdx( wMcuIdx );
	CServMsg cServMsg;
	CServMsg cMsg;
	u8 byMtNum = 0;
	cMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
	cMsg.SetEventId( MCU_MCU_MTSTATUS_NOTIF );

	for(; nIndex<MAXNUM_CONF_MT; nIndex++)
	{
		if( ptMcInfo->m_atMtStatus[nIndex].IsNull() || 
			ptMcInfo->m_atMtStatus[nIndex].GetMtId() == 0 ) //�Լ�
		{
			continue;
		}	
		memset( &tSMcuMtStatus,0,sizeof(tSMcuMtStatus) );
		memcpy( (void*)&tSMcuMtStatus, (void*)&ptMcInfo->m_atMtStatus[nIndex], sizeof(TMt) );
		tSMcuMtStatus.SetMcuId( byMcuId );
		tSMcuMtStatus.SetIsEnableFECC( ptMcInfo->m_atMtStatus[nIndex].IsEnableFECC() );
        tSMcuMtStatus.SetCurVideo( ptMcInfo->m_atMtStatus[nIndex].GetCurVideo() );
        tSMcuMtStatus.SetCurAudio( ptMcInfo->m_atMtStatus[nIndex].GetCurAudio() );
        tSMcuMtStatus.SetMtBoardType( ptMcInfo->m_atMtStatus[nIndex].GetMtBoardType() );
        tSMcuMtStatus.SetIsMixing( ptMcInfo->m_atMtStatus[nIndex].IsInMixing() );
        tSMcuMtStatus.SetVideoLose( ptMcInfo->m_atMtStatus[nIndex].IsVideoLose() );
		BOOL32 bIsAuto = ( ptMcInfo->m_atMtExt[nIndex].GetCallMode() == CONF_CALLMODE_TIMER) ? TRUE : FALSE;
		tSMcuMtStatus.SetIsAutoCallMode( bIsAuto );
		tSMcuMtStatus.SetRecvVideo( ptMcInfo->m_atMtStatus[nIndex].IsReceiveVideo() );
		tSMcuMtStatus.SetRecvAudio( ptMcInfo->m_atMtStatus[nIndex].IsReceiveAudio() );
		tSMcuMtStatus.SetSendVideo( ptMcInfo->m_atMtStatus[nIndex].IsSendVideo() );
		tSMcuMtStatus.SetSendAudio( ptMcInfo->m_atMtStatus[nIndex].IsSendAudio() );
		tSMcuMtStatus.SetDisconnectReason( ptMcInfo->m_atMtStatus[nIndex].GetMtDisconnectReason() );
		tSMcuMtStatus.SetSendVideo2(ptMcInfo->m_atMtStatus[nIndex].IsSndVideo2());


		if( 0 == byMtNum )
		{
			cServMsg.SetMsgBody( (u8*)&tSMcuMtStatus,sizeof(tSMcuMtStatus) );
		}
		else
		{
			cServMsg.CatMsgBody( (u8*)&tSMcuMtStatus,sizeof(tSMcuMtStatus) );
		}
		++byMtNum;
	}


	if( byMtNum > 0 )
	{		
		cMsg.SetMsgBody( (u8*)&byMtNum,sizeof(u8) );
		cMsg.CatMsgBody( cServMsg.GetMsgBody(),cServMsg.GetMsgBodyLen() );
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "send %u mtstatus of smcu.%d to mmcu\n", byMtNum, wMcuIdx);
		SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_MTSTATUS_NOTIF, cMsg );
	}

	
        
        /*if( m_tConfAllMtInfo.MtInConf(byMtId) && byDstMcuId != byMtId )
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

			//zjj20100327
			tSMcuMtStatus.SetMtDisconnectReason( tLocalMtStatus.GetMtDisconnectReason() );

			tSMcuMtStatus.SetMcuIdx( LOCAL_MCUID );
            if( 0 == byMtNum )
            {
                cServMsg.SetMsgBody( (u8*)&tSMcuMtStatus, sizeof(TSMcuMtStatus) );
            }
            else
            {
                cServMsg.CatMsgBody( (u8*)&tSMcuMtStatus, sizeof(TSMcuMtStatus) );
            }
            byMtNum++;
        }*/




}
/*====================================================================
    ������      ��SendSMcuMtInfoToMMcu
    ����        �����������¼��ն��б���ϼ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/07/28    4.6         �ܾ���         ����
====================================================================*/
void CMcuVcInst::SendAllSMcuMtInfoToMMcu()
{
	if( m_tCascadeMMCU.IsNull() || !IsMMcuSupportMultiCascade() )
	{
		return;
	}
	u8 byLoop;
	u16 wMcuIdx = INVALID_MCUIDX;
	u8 byMcuId = 0;
	for( byLoop = 1;byLoop <= MAXNUM_CONF_MT;++byLoop )
	{
		if( MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) )
		{
			byMcuId = byLoop;
			if( m_tConfAllMcuInfo.GetIdxByMcuId(&byMcuId,1,&wMcuIdx) )
			{
				SendSMcuMtInfoToMMcu( wMcuIdx );
			}
		}
	}
}

/*====================================================================
    ������      ��SendSMcuMtInfoToMMcu
    ����        ������ĳ�¼��ն��б���ϼ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/07/28    4.6         �ܾ���         ����
====================================================================*/
void CMcuVcInst::SendSMcuMtInfoToMMcu( u16 wMcuIdx )
{
	if( m_tCascadeMMCU.IsNull() || !IsMMcuSupportMultiCascade() )
	{
		return;
	}

	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	memset( &abyMcuId[0],0,sizeof(abyMcuId) );

	if( !m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0]) )
	{
		return;
	}

	if( 0 == abyMcuId[0] || 0 != abyMcuId[1] )
	{
		return;
	}

	TMcuMcuMtInfo atInfo[MAXNUM_CONF_MT];

	u8 byInfoNum = 0;
	

	//�����ն��б�
	TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );
    
	for(u8 byLoop = 0; byLoop < MAXNUM_CONF_MT; byLoop++)
	{	
		if (NULL == ptMcMtInfo)
		{
			continue;
		}
		if(	ptMcMtInfo->m_atMtExt[byLoop].IsNull() &&
			!( ptMcMtInfo->m_atMtStatus[byLoop].GetMtType() == MT_TYPE_MT && 
			 0 == ptMcMtInfo->m_atMtStatus[byLoop].GetMtId() )
			)
		{
			continue;
		}
		atInfo[byInfoNum].m_tMt = (TMt)ptMcMtInfo->m_atMtExt[byLoop];
		atInfo[byInfoNum].m_tMt.SetMcuId( abyMcuId[0] );
		strncpy((s8 *)(atInfo[byInfoNum].m_szMtName), ptMcMtInfo->m_atMtExt[byLoop].GetAlias(), sizeof(atInfo[byInfoNum].m_szMtName) - 1);
		atInfo[byInfoNum].m_dwMtIp   = htonl( ptMcMtInfo->m_atMtExt[byLoop].GetIPAddr() );
		atInfo[byInfoNum].m_byMtType = ptMcMtInfo->m_atMtExt[byLoop].GetMtType();
		atInfo[byInfoNum].m_byManuId = ptMcMtInfo->m_atMtExt[byLoop].GetManuId();
        atInfo[byInfoNum].m_byProtocolType = ptMcMtInfo->m_atMtExt[byLoop].GetProtocolType();
		
		atInfo[byInfoNum].m_byVideoIn = ptMcMtInfo->m_atMtStatus[byLoop].GetVideoIn();
		atInfo[byInfoNum].m_byVideoOut = ptMcMtInfo->m_atMtStatus[byLoop].GetVideoOut();

		atInfo[byInfoNum].m_byAudioIn = ptMcMtInfo->m_atMtStatus[byLoop].GetAudioIn();
		atInfo[byInfoNum].m_byAudioOut = ptMcMtInfo->m_atMtStatus[byLoop].GetAudioOut();

		atInfo[byInfoNum].m_byIsAudioMuteIn = ptMcMtInfo->m_atMtStatus[byLoop].IsSendAudio() ? 0 : 1;
		atInfo[byInfoNum].m_byIsAudioMuteOut = ptMcMtInfo->m_atMtStatus[byLoop].IsReceiveAudio() ? 0 : 1;

		atInfo[byInfoNum].m_byIsVideoMuteIn = ptMcMtInfo->m_atMtStatus[byLoop].IsSendVideo() ? 0 : 1;
		atInfo[byInfoNum].m_byIsVideoMuteOut = ptMcMtInfo->m_atMtStatus[byLoop].IsReceiveVideo() ? 0 : 1;
		
		TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo( wMcuIdx );
		atInfo[byInfoNum].m_byIsConnected = tMtInfo.MtJoinedConf( atInfo[byInfoNum].m_tMt.GetMtId() );

		atInfo[byInfoNum].m_byIsFECCEnable = ptMcMtInfo->m_atMtStatus[byLoop].IsEnableFECC() ? 1 : 0;

		atInfo[byInfoNum].m_tPartVideoInfo = ptMcMtInfo->m_atMtStatus[byLoop].GetMtVideoInfo();
    
	
		byInfoNum++;	
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[SendSMcuMtInfoToMMcu] Notified McuID.%d LocalTotalMtNum.%d\n", m_tCascadeMMCU.GetMtId(), byInfoNum );

	if( 0 == byInfoNum )
	{
		return;
	}

	u8 byLastPack = 1;
	CServMsg cServMsg;
	cServMsg.SetEventId( MCU_MCU_MTLIST_ACK );
	cServMsg.SetMsgBody(&byLastPack, sizeof(u8));
	cServMsg.CatMsgBody((u8 *)atInfo, byInfoNum * sizeof(TMcuMcuMtInfo));
	SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_MTLIST_ACK,  cServMsg);

	//�����ϼ�MCU��ĵ�һ���¼�MCU�����ն�״̬ǿ��֪ͨ��֮��ĸ����ն�״̬����ʵʱ֪ͨ
	/*if( m_tConfInStatus.IsNtfMtStatus2MMcu() ||
		// vcsˢ�б�ͬʱ��Ҫˢ����״̬��Ϣ���������а�����Ҫ���ն˺���ģʽ��Ϣ
		VCS_CONF == m_tConf.GetConfSource())
	{
		OnNtfMtStatusToMMcu( m_tCascadeMMCU.GetMtId() );
		m_tConfInStatus.SetNtfMtStatus2MMcu(FALSE);
	}
	*/


}
/*====================================================================
    ������      ��SendSMcuUpLoadMtToMMcu
    ����        ������ָ��MCU���ϴ�ͨ���ն˸�MMCU
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    12/01/04    4.7         ��־��         ����
====================================================================*/
void CMcuVcInst::SendSMcuUpLoadMtToMMcu()
{
	TSetOutParam tOutParam;
	TMsgHeadMsg tHeadMsg;
	CServMsg cServMsg;
	u8 byMcuId = 0;
	u16 wMcuIdx = INVALID_MCUIDX;
	TMt tSetoutMt;
	for( u8 byLoop = 1;byLoop <= MAXNUM_CONF_MT;++byLoop )
	{
		if( MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) )
		{
			byMcuId = byLoop;
			if( m_tConfAllMcuInfo.GetIdxByMcuId(&byMcuId,1,&wMcuIdx) )
			{
				TConfMcInfo * ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
				if(NULL != ptMcInfo && !(ptMcInfo->m_tMMcuViewMt.IsNull()))
				{
					// ��ʼ��
					memset(&tOutParam, 0, sizeof(tOutParam));
					memset(&tHeadMsg, 0, sizeof(tHeadMsg));
					memset(&cServMsg, 0, sizeof(cServMsg));
					
					BuildMultiCascadeMtInfo(ptMcInfo->m_tMMcuViewMt, tSetoutMt);
					tOutParam.m_nMtCount = 1;
					++tHeadMsg.m_tMsgDst.m_byCasLevel; //��������������NOTIF��MsgDstLevel�ȼ�1����ʶΪ�¼��ϴ��ն�
					tOutParam.m_atConfViewOutInfo[0].m_tMt = tSetoutMt;
					TConfMcInfo *ptCascadeMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() ));
					if (ptCascadeMcInfo == NULL)
					{
						return;
					}
					tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptCascadeMcInfo->m_dwSpyViewId;
					tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptCascadeMcInfo->m_dwSpyVideoId;
					
					cServMsg.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
					cServMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
					cServMsg.SetEventId(MCU_MCU_SETOUT_NOTIF);
					SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_SETOUT_NOTIF, cServMsg );
				}
			}
		}
	}
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
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "Recv %d mtlist from smcu(%d,%d)\n", nMtNum, tMt.GetMcuId(), tMt.GetMtId());

	u16 wMcuIdx = INVALID_MCUIDX;

	u8 bySecMcuId = 0;

	//  [2/23/2011 chendaiwei]��־�Ƿ���N+1���ᣨ�ָ������ع����ᣩʱ��ӵ���¼��ն���������
	BOOL32 bIsNPlusSmcuSpeaker = FALSE;
	//  [2/23/2011 chendaiwei]N+1����ʱ���¼��ն˷�����
	TMt tNPlusSmcuSpeaker;
	tNPlusSmcuSpeaker.SetNull();

	if( nMtNum > 0 )
	{
		if( 0 != ptMcuMcuMtInfo->m_byCasLevel &&
			ptMcuMcuMtInfo->m_byCasLevel <= MAX_CASCADELEVEL
			)
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuMtListAck] Index 0 tMt(%d.%d.%d) byCasLevel(%d)\n",
				ptMcuMcuMtInfo->m_tMt.GetMcuId(),
				ptMcuMcuMtInfo->m_tMt.GetMtId(),
				ptMcuMcuMtInfo->m_abyMtIdentify[0],
				ptMcuMcuMtInfo->m_byCasLevel
				);
			bySecMcuId = ptMcuMcuMtInfo->m_tMt.GetMtId();
		}
	}
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	memset( &abyMcuId[0],0,sizeof(abyMcuId) );
	abyMcuId[0] = tMt.GetMtId();
	abyMcuId[1] = bySecMcuId;
	
	// [11/29/2011 liuxu] ��ֹmcu���Ҷ�ʱ, �����ն��б���Ȼ��osp��Ϣ������
	if ( !m_tConfAllMtInfo.MtJoinedConf(tMt) )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "FatherMcu(%d,%d) has been off-line, ignore mcumt(1st.%d, 2nd.%d)' mtlist\n",
			tMt.GetMcuId(), tMt.GetMtId(), abyMcuId[0], abyMcuId[1] = bySecMcuId);
		return;
	}

	if( !m_tConfAllMcuInfo.AddMcu( &abyMcuId[0],2,&wMcuIdx ) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuMtListAck] Fail to Add Mcu Info.McuId(%d)\n",tMt.GetMtId() );
		return;
	}
	
	TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->AddMcInfo(wMcuIdx);
	if(ptMcMtInfo == NULL)
	{
		return;
	}
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "It's from %d sMcu(%d,%d)\n", wMcuIdx, abyMcuId[0], abyMcuId[1]);

	if( 0 == nMtNum && ISTRUE(byIsLastPack) && 0 == ptMcMtInfo->m_byLastPos )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuMtListAck] nMtNum is 0 and IsLastPack and m_byLastPos is 0,it's impossible.return\n" );
		return;
	}
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuMtListAck] RemoteMcuID.%d LastMtNum.%d,This Pack include MtNum.%d. byIsLastPack.%d\n", 
		     tMt.GetMtId(), ptMcMtInfo->m_byLastPos, nMtNum, byIsLastPack );

	
	//ȫ�µ�һ��
	if(ptMcMtInfo->m_byLastPos == 0)
	{
		//���ֳ�ʼ��
		memset((ptMcMtInfo->m_atMtExt), 0, sizeof(ptMcMtInfo->m_atMtExt));
		//memset(&(ptMcMtInfo->m_atMtStatus), 0, sizeof(ptMcMtInfo->m_atMtStatus));
		ptMcMtInfo->SetMcuIdx( INVALID_MCUIDX );
		//zjj [08/29/2009]���½ṹ��Ҫ���,�ڹ���ProcMcuMcuAudioInfoAck��Ҫ��
		//memset(&(ptMcMtInfo->m_tConfAudioInfo), 0, sizeof(ptMcMtInfo->m_tConfAudioInfo));
		memset(&(ptMcMtInfo->m_tConfViewInfo), 0, sizeof(ptMcMtInfo->m_tConfViewInfo));
	}
	ptMcMtInfo->SetMcuIdx( wMcuIdx );

	s32 nIndex = 0;
	s32 nLoop  = 0;
	u8 byRealMtId = 0;
/*	BOOL32 bIsCanNotifyToMMcu = FALSE;
	TMtExtU atMtExtU[MAXNUM_CONF_MT]; //  [pengguofeng 7/9/2013]�ϱ�����Ľṹ��
	memset(atMtExtU, 0, sizeof(atMtExtU));
*/
	for(nIndex = ptMcMtInfo->m_byLastPos, nLoop =0; nIndex<ptMcMtInfo->m_byLastPos+nMtNum&&nIndex<MAXNUM_CONF_MT; nIndex++, nLoop++)
	{
		ptMcMtInfo->m_atMtStatus[nIndex].SetAudioIn(ptMcuMcuMtInfo[nLoop].m_byAudioIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetAudioOut(ptMcuMcuMtInfo[nLoop].m_byAudioOut);
		BOOL32 bSendAudio = !ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsAudioMuteIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio(bSendAudio);
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetAudioIn() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio(FALSE);
		}
		BOOL32 bRecvAudio = !ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsAudioMuteOut);
		ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio(bRecvAudio);
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetAudioOut() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio(FALSE);
		}
		ptMcMtInfo->m_atMtExt[nIndex].SetMt(ptMcuMcuMtInfo[nLoop].m_tMt);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMt(ptMcuMcuMtInfo[nLoop].m_tMt);
#ifdef _UTF8
		// ȥ�����ضϵĺ��� [pengguofeng 5/24/2013]
		CorrectUtf8Str(ptMcuMcuMtInfo[nLoop].m_szMtName, strlen(ptMcuMcuMtInfo[nLoop].m_szMtName));
#endif
		ptMcMtInfo->m_atMtExt[nIndex].SetAlias(ptMcuMcuMtInfo[nLoop].m_szMtName);
		
		//  [2/23/2011 chendaiwei]NPlus���ᣬͨ�������˱�������Ƿ����¼��ն���������
		if( !m_tConf.HasSpeaker() && !m_tConf.GetSpeakerAlias().IsNull() &&
			memcmp(m_tConf.GetSpeakerAlias().m_achAlias,ptMcuMcuMtInfo[nLoop].m_szMtName,VALIDLEN_ALIAS) == 0 )
		{
			bIsNPlusSmcuSpeaker = TRUE;
			tNPlusSmcuSpeaker.SetMtId(ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId());
			tNPlusSmcuSpeaker.SetMcuIdx(wMcuIdx);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"Found SMCU speaker: alias == %s,McuIndex == %u, Mt Id == %u\n",m_tConf.GetSpeakerAlias().m_achAlias,tNPlusSmcuSpeaker.GetMcuId(),tNPlusSmcuSpeaker.GetMtId());
		}

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
		BOOL32 bSendVideo = !ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsVideoMuteIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetSendVideo(bSendVideo);   
		BOOL32 bRecvVideo = !ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsVideoMuteOut);
		ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveVideo( bRecvVideo );
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetVideoIn() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetSendVideo(FALSE);    
		}
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetVideoOut() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveVideo(FALSE);
		}
		if( 0 == ptMcuMcuMtInfo[nLoop].m_byCasLevel )
		{
			byRealMtId = ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId();
		}
		else
		{
			if( ptMcuMcuMtInfo[nLoop].m_byCasLevel <= MAX_CASCADELEVEL )
			{
				byRealMtId = ptMcuMcuMtInfo[nLoop].m_abyMtIdentify[ptMcuMcuMtInfo[nLoop].m_byCasLevel-1];
			}
			else
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuMtListAck] Error CasLevel.%d MtId(%d)\n",
					ptMcuMcuMtInfo[nLoop].m_byCasLevel,tMt.GetMtId()
					);				
			}
		}

		TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);
		tMtInfo.RemoveMt(byRealMtId);//ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId());

		if(ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsConnected))
		{
			tMtInfo.AddJoinedMt(byRealMtId);//ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId());

			RestoreSubMtInVmp(ptMcuMcuMtInfo[nLoop].m_tMt);
		}
		else
		{
			tMtInfo.AddMt(byRealMtId);//ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId());
		}
		m_tConfAllMtInfo.SetMtInfo(tMtInfo);
		BOOL32 bEnableFECC = ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsFECCEnable);
        ptMcMtInfo->m_atMtStatus[nIndex].SetIsEnableFECC(bEnableFECC);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMtVideoInfo(ptMcuMcuMtInfo[nLoop].m_tPartVideoInfo);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMcuIdx( wMcuIdx );
		ptMcMtInfo->m_atMtExt[nIndex].SetMcuIdx( wMcuIdx );
		ptMcMtInfo->m_atMtStatus[nIndex].SetMtId( byRealMtId );
		ptMcMtInfo->m_atMtExt[nIndex].SetMtId( byRealMtId );
 		LogPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcuMcuMtListAck]McuIdx.%d conn:%d MtExt loop.%d MtId is %d, name:(%s )\n",
 			wMcuIdx, ptMcuMcuMtInfo[nLoop].m_byIsConnected, nIndex, byRealMtId, ptMcuMcuMtInfo[nLoop].m_szMtName);

		//����Ϊ�����¼��б��ϼ���׼��
		/*if( !m_tCascadeMMCU.IsNull() && 
			MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) &&
			ptMcuMcuMtInfo[nLoop].m_byCasLevel < MAX_CASCADELEVEL
			)
		{
			ptMcuMcuMtInfo[nLoop].m_abyMtIdentify[ptMcuMcuMtInfo[nLoop].m_byCasLevel] = ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId(); 

			++ptMcuMcuMtInfo[nLoop].m_byCasLevel;			
			ptMcuMcuMtInfo[nLoop].m_tMt.SetMcuId( LOCAL_MCUID );
			ptMcuMcuMtInfo[nLoop].m_tMt.SetMtId( tMt.GetMcuId() );

			bIsCanNotifyToMMcu = TRUE;
		}
		*/
/*
		memcpy(&atMtExtU[nIndex], &ptMcMtInfo->m_atMtExt[nIndex], sizeof(TMt) + sizeof(u8)*2+sizeof(u16) + sizeof(u32)*2);
		atMtExtU[nIndex].SetProtocolType(ptMcuMcuMtInfo[nLoop].m_byProtocolType);
		atMtExtU[nIndex].SetAlias(ptMcuMcuMtInfo[nLoop].m_szMtName);
*/	}

	if(ISTRUE(byIsLastPack))
	{
        // ���� [6/1/2006] ������������
        if (nIndex < MAXNUM_CONF_MT)
        {
            memset( ptMcMtInfo->m_atMtStatus+(u32)nIndex, 0, (sizeof(TMcMtStatus) * (MAXNUM_CONF_MT - (u32)nIndex)) );
        }
        
		ptMcMtInfo->m_byLastPos = 0;

		//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ));
		//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
		CServMsg cMsg2Mcs = cServMsg; //�˽ӿڻ�ı����
		SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cMsg2Mcs/*cServMsg*/ );

		//��������ն˱�
		if ( cServMsg.GetSrcMtId() == m_tCascadeMMCU.GetMtId() )
		{
			SendMtListToMcs( wMcuIdx );// mmcu�ع�����mtlist_ack�Ͳ����ٻظ�MMcu�Լ��� [pengguofeng 7/10/2013]
		}
		else
		{
			SendMtListToMcs( wMcuIdx , FALSE, TRUE );
		}
		// Ӧ�ý���ʱ�¼���TMcuMcuMtInfo�����ϼ��������Ĳ��ã���Ϊ��ͱ���ȥ�� [pengguofeng 7/9/2013]
		//���ڷֿ����ľͳ�����
 /*		u8 byMtNum = (u8)nMtNum;
 		PackAndSendMtList2Mcs(byMtNum, atMtExtU, wMcuIdx);
*/
		//�ϱ��¼��б���ϼ�
/*		if( ptMcuMcuMtInfo->m_byCasLevel < MAX_CASCADELEVEL  &&
			!m_tCascadeMMCU.IsNull() &&
			MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) 
			)
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[ProcMcuMcuMtListAck] Send %d mt to MMcu.%d\n", nMtNum, m_tCascadeMMCU.GetMtId());
			cServMsg.SetMsgBody( (u8*)&byIsLastPack,sizeof(u8) );
			cServMsg.CatMsgBody( (u8*)ptMcuMcuMtInfo,sizeof(TMcuMcuMtInfo)*nMtNum );
			SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_MTLIST_ACK,cServMsg );
//			SendSMcuMtInfoToMMcu( wMcuIdx );
			ConfLog( FALSE,"[ProcMcuMcuMtListAck] Send " );
			cServMsg.SetMsgBody( (u8*)&byIsLastPack,sizeof(u8) );
			cServMsg.CatMsgBody( (u8*)ptMcuMcuMtInfo,sizeof(TMcuMcuMtInfo)*nMtNum );
			SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_MTLIST_ACK,cServMsg );
			
		}
*/
		//[2/21/2011 chendaiwei]NPlus�����ҵ����ı䷢����Ϊ�¼��ն�
// 		TMtAlias tSpeaker = m_tConf.GetSpeakerAlias();
// 		TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
// 
// 		if(ptMcMtInfo != NULL)
// 		{
// 			TMt tTemp;
// 			tTemp.SetNull();
// 			tTemp.SetMcuId(wMcuIdx);
// 			for( u8 byMtId = 1;byMtId <= MAXNUM_CONF_MT;byMtId++)
// 			{
// 				tTemp.SetMtId(byMtId);
// 				TMtExt * ptEx = ptMcMtInfo->GetMtExt(tTemp);
// 				if( ptEx != NULL)
// 				{
// 					if(memcmp(ptEx->GetAlias(),tSpeaker.m_achAlias,VALIDLEN_ALIAS) == 0)
// 					{
// 						ChangeSpeaker(&tTemp);
// 
// 						break;
// 					}
// 				}
// 			}
// 		}

		
// [pengjie 2010/12/14] �¼������ϼ�����mtlistʱֱ���ϱ�mt״̬���������ﲻ���ظ�����
//         // ���� [6/1/2006] �����¼�Mcu�ն�״̬�����
// 	    CServMsg cServBackMsg;
// 	    cServBackMsg.SetNoSrc();
//         cServBackMsg.SetMsgBody();
// 		for(nIndex = 0; nIndex < MAXNUM_CONF_MT; nIndex++)
// 		{
// 			if( ptMcMtInfo->m_atMtStatus[nIndex].IsNull() || 
// 				ptMcMtInfo->m_atMtStatus[nIndex].GetMtId() == 0 ) //�Լ�
// 			{
// 				continue;
// 			}
//             TMtStatus tMtStatus = ptMcMtInfo->m_atMtStatus[nIndex].GetMtStatus();
//             cServBackMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );	
//         }
//         if (cServBackMsg.GetMsgBodyLen() != 0)
//         {
		//             SendMsgToAllMcs( MCU_MCS_MTSTATUS_NOTIF, cServBackMsg );       
//         }         
	}
	else
	{
		ptMcMtInfo->m_byLastPos += nMtNum;
	}

	//  [2/23/2011 chendaiwei]N+1�������¼������ˣ��ı䷢����Ϊ�¼������ˡ�ͬʱ�ѷ����˱�����գ�������û�з����ˣ������˱������ɵ���
	//  �����γ����У���������ؽ������˸ĳɱ�������ķ����ˡ�
	if(bIsNPlusSmcuSpeaker && 
		( MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState() || MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState()) )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"Change speaker to smcu speaker McuId == %u, MtId == %u\n",tNPlusSmcuSpeaker.GetMcuId(),tNPlusSmcuSpeaker.GetMtId());
		
		ChangeSpeaker(&tNPlusSmcuSpeaker);

		TMtAlias tNullAlias;
		tNullAlias.SetNull();
		m_tConf.SetSpeakerAlias(tNullAlias);
	}

	if (m_tConf.m_tStatus.IsAutoMixing())
	{
		TMcu tMcu;
		tMcu.SetMcuId(LOCAL_MCUID);	
		CServMsg cMsg;
		u8 byMixDepth = 10;
		cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) ); 
		cMsg.CatMsgBody( (u8*)&byMixDepth, 1 );
		OnStartDiscussToAllSMcu( &cMsg );
	}


	return;
}

/*====================================================================
    ������      ��RestoreSubMtInVmp
    ����        ���ָ��¼��ն˽�����ϳ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt& tMt 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/07/16    4.6         ½����			����
====================================================================*/
void CMcuVcInst::RestoreSubMtInVmp(TMt& tMt)
{
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[RestoreSubMtInVmp] tMt(%d.%d)\n",tMt.GetMcuId(),tMt.GetMtId() );
	return;

	/*if (tMt.IsNull() || tMt.IsLocal())
	{
	return;
	}
	
	  TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpParam();
	  
		u8 byIndex = 0;
		if (!tVMPParam.FindVmpMember(tMt, byIndex))
		{
		return;
		}
		
		  TVMPMember* ptVMPMember = tVMPParam.GetVmpMember(byIndex);
		  
			if (ptVMPMember == NULL)
			{
			return;
			}
			
			  BOOL32 bStart = FALSE;
			  u8 byNewFormat = 0;
			  if (tVMPParam.GetVMPMode() == CONF_VMPMODE_NONE)
			  {
			  bStart = TRUE;
			  }
			  
				if ( IsLocalAndSMcuSupMultSpy(ptVMPMember->GetMcuId()) )
				{
				//ע�⣬�ڷ�PreSetInʱ��Ҫ�Ѵ��ն���Ϣ��Param��ɾ��������PreSetInAck�д���ʱ��ӽ�ȥ
				m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember(byIndex);
				m_tLastVmpParam = m_tConf.m_tStatus.GetVmpParam();
				
				  CVmpChgFormatResult cVmpChgFormatResult;
				  if (!GetMtFormat(*(TMt *)ptVMPMember, &tVMPParam, byNewFormat, cVmpChgFormatResult, bStart))
				  {
				  return;
				  }
				  
					//��Pre Setin (�����˼������ֱ���)
					TPreSetInReq tPreSetInReq;
					tPreSetInReq.m_tSpyMtInfo.SetSpyMt(*(TMt *)ptVMPMember);
					tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_tVmp = m_tVmpEqp;
					tPreSetInReq.m_bySpyMode = MODE_VIDEO;
					tPreSetInReq.m_dwEvId = MCS_MCU_STARTVMP_REQ;
					
					  //		tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byRes = byNewFormat;
					  tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byPos = byIndex;		
					  //		tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byKeepOrgRes = 0; //no adapt channel in old vmp, so set this value 0
					  tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byMemberType = ptVMPMember->GetMemberType();
					  tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byMemStatus = ptVMPMember->GetMemStatus();
					  
						// [pengjie 2010/9/13] ��Ŀ�Ķ�����
						TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(ptVMPMember->GetMcuId()) );	
						if(tSimCapSet.IsNull())
						{
						MultiSpyMgrLog( SPY_CRIT, "[ChangeSpeaker] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
						ptVMPMember->GetMcuId(), ptVMPMember->GetMtId() );
						return;
						}
						// ����������ϳ�Ҫ��ķֱ���
						tSimCapSet.SetVideoResolution( byNewFormat );
						//zjl20101116 �����ǰ�ն��ѻش���������Ҫ���ѻش�Ŀ��������ȡС
						tSimCapSet = GetMinSpyDstCapSet(*(TMt*)ptVMPMember, tSimCapSet);
						tPreSetInReq.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
						// End
						
						  OnMMcuPreSetIn( tPreSetInReq );
	}*/
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

	u16 wMcuIdx = INVALID_MCUIDX;

	u8 bySecMcuId = 0;

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuNewMtNotify]  tMt(%d.%d.%d) byCasLevel(%d)\n",
			tMcuMcuMtInfo.m_tMt.GetMcuId(),
			tMcuMcuMtInfo.m_tMt.GetMtId(),
			tMcuMcuMtInfo.m_abyMtIdentify[0],
			tMcuMcuMtInfo.m_byCasLevel
			);
	
	if( 0 != tMcuMcuMtInfo.m_byCasLevel &&
		tMcuMcuMtInfo.m_byCasLevel <= MAX_CASCADELEVEL
		)
	{		
		bySecMcuId = tMcuMcuMtInfo.m_tMt.GetMtId();
	}
	
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	abyMcuId[0] = tMt.GetMtId();
	abyMcuId[1] = bySecMcuId;

	if( !m_tConfAllMcuInfo.GetIdxByMcuId(&abyMcuId[0],2,&wMcuIdx) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuNewMtNotify] Fail to get Mcu Index.FstMcuId.%d SecMcuId.%d\n",
			tMt.GetMtId(),bySecMcuId
			);
		return;
	}
	

	u16 wSMcuIdx = INVALID_MCUIDX;
	m_tConfAllMcuInfo.GetIdxByMcuId(&abyMcuId[0],1,&wSMcuIdx);
	if( 0 == bySecMcuId && !IsRosterRecved(wSMcuIdx) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,"[ProcMcuMcuNewMtNotify]smcu.%d mcuidx.%d is not recv roster ntf\n",
			abyMcuId[0],wSMcuIdx );	
		return;
	}

	u8 byRealMtId = 0;
	if( 0 == tMcuMcuMtInfo.m_byCasLevel )
	{
		byRealMtId = tMcuMcuMtInfo.m_tMt.GetMtId();
	}
	else
	{
		if( tMcuMcuMtInfo.m_byCasLevel <= MAX_CASCADELEVEL )
		{
			byRealMtId = tMcuMcuMtInfo.m_abyMtIdentify[tMcuMcuMtInfo.m_byCasLevel-1];
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuNewMtNotify] Error CasLevel.%d MtId(%d)\n",
				tMcuMcuMtInfo.m_byCasLevel,tMt.GetMtId()
				);
			return;
		}
		
	}

	TMt tNewMt = tMcuMcuMtInfo.m_tMt;
	tNewMt.SetMcuIdx( wMcuIdx );
	tNewMt.SetMtId( byRealMtId );
	tNewMt.SetConfIdx( m_byConfIdx );

	/*20111024 zjl 1. �ϼ������ն���� + ����֧����ʾ�ϼ��ն��б� = �����ϼ��ն��б�
				   2. �¼������ն���� = ���������¼��ն��б�
	               3. û���ϼ�mcu  �� ��ͬ��2�����*/
	if (!(tMt == m_tCascadeMMCU) ||
		(tMt == m_tCascadeMMCU &&
		 g_cMcuVcApp.IsShowMMcuMtList()))
	{
		//���ӵ��ն˱���
		TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tMt.GetMtId());
		if(tConfMtInfo.IsNull())
		{
			return;
		}
		tConfMtInfo.AddJoinedMt(byRealMtId);//tMcuMcuMtInfo.m_tMt.GetMtId());
		m_tConfAllMtInfo.SetMtInfo(tConfMtInfo);

		RestoreSubMtInVmp( tNewMt );

		//������ػ��������ն���Ϣ
		//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
		//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
		SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
		
		//����mc table
		TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->AddMcInfo(wMcuIdx);//tMt.GetMtId());
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
			if(ptMcMtInfo->m_atMtStatus[nIndex].GetMtId() == byRealMtId )//tMcuMcuMtInfo.m_tMt.GetMtId())
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
		BOOL32 bSendAudio = !ISTRUE(tMcuMcuMtInfo.m_byIsAudioMuteIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio(bSendAudio);
		BOOL32 bRecvAudio = !ISTRUE(tMcuMcuMtInfo.m_byIsAudioMuteOut);
		ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio(bRecvAudio);
		ptMcMtInfo->m_atMtExt[nIndex].SetMt(tMcuMcuMtInfo.m_tMt);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMt(tMcuMcuMtInfo.m_tMt);
#ifdef _UTF8
		// ����utf8�ַ��� [pengguofeng 5/24/2013]
		CorrectUtf8Str(tMcuMcuMtInfo.m_szMtName, strlen(tMcuMcuMtInfo.m_szMtName));
#endif
		ptMcMtInfo->m_atMtExt[nIndex].SetAlias(tMcuMcuMtInfo.m_szMtName);
		ptMcMtInfo->m_atMtExt[nIndex].SetIPAddr(ntohl(tMcuMcuMtInfo.m_dwMtIp));
		ptMcMtInfo->m_atMtExt[nIndex].SetMtType(tMcuMcuMtInfo.m_byMtType);
		ptMcMtInfo->m_atMtExt[nIndex].SetManuId(tMcuMcuMtInfo.m_byManuId);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMtType(tMcuMcuMtInfo.m_byMtType);
		ptMcMtInfo->m_atMtStatus[nIndex].SetVideoIn(tMcuMcuMtInfo.m_byVideoIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetVideoOut(tMcuMcuMtInfo.m_byVideoOut);
	//	ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2In(tMcuMcuMtInfo.m_byVideo2In);
	//	ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2Out(tMcuMcuMtInfo.m_byVideo2Out);
		BOOL32 bSendVideo = !ISTRUE(tMcuMcuMtInfo.m_byIsVideoMuteIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetSendVideo(bSendVideo); 
		BOOL32 bRecvVideo = !ISTRUE(tMcuMcuMtInfo.m_byIsVideoMuteOut);
		ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveVideo(bRecvVideo);
		TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tMt.GetMtId());
		tMtInfo.RemoveMt(byRealMtId);//tMcuMcuMtInfo.m_tMt.GetMtId());
		if(ISTRUE(tMcuMcuMtInfo.m_byIsConnected))
		{
			tMtInfo.AddJoinedMt(byRealMtId);//tMcuMcuMtInfo.m_tMt.GetMtId());
		}
		else
		{
			tMtInfo.AddMt(byRealMtId);//tMcuMcuMtInfo.m_tMt.GetMtId());
		}
		m_tConfAllMtInfo.SetMtInfo(tMtInfo);
		BOOL32 bEnableFEC = ISTRUE(tMcuMcuMtInfo.m_byIsFECCEnable);
		ptMcMtInfo->m_atMtStatus[nIndex].SetIsEnableFECC(bEnableFEC);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMtVideoInfo(tMcuMcuMtInfo.m_tPartVideoInfo); 
		ptMcMtInfo->m_atMtStatus[nIndex].SetMcuIdx( wMcuIdx );
		ptMcMtInfo->m_atMtStatus[nIndex].SetMtId( byRealMtId );
		ptMcMtInfo->m_atMtExt[nIndex].SetMcuIdx( wMcuIdx );
		ptMcMtInfo->m_atMtExt[nIndex].SetMtId( byRealMtId );

		
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuNewMtNotify] nIndex.%d MtId.%d szMtName.%s bOnLine.%d\n", 
				 nIndex, tMcuMcuMtInfo.m_tMt.GetMtId(), tMcuMcuMtInfo.m_szMtName, tMcuMcuMtInfo.m_byIsConnected );

 		SendMtListToMcs( wMcuIdx, FALSE, FALSE ); // ��Ҫ�ٱ� [pengguofeng 7/10/2013]
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuNewMtNotify] tMt<McuId.%d, MtId.%d> is equal to mmcu <McuId.%d, MtId.%d> , IsShowMMcuMtList.%d!\n",
									tMt.GetMcuId(), tMt.GetMtId(), 
									m_tCascadeMMCU.GetMcuId(), m_tCascadeMMCU.GetMtId(),
									g_cMcuVcApp.IsShowMMcuMtList());
	}

	//�ϱ����ϼ�
	if( !m_tCascadeMMCU.IsNull() && 
		MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) &&
		tMcuMcuMtInfo.m_byCasLevel < MAX_CASCADELEVEL &&
		IsMMcuSupportMultiCascade()
		)
	{
		tMcuMcuMtInfo.m_tMt = tNewMt;
		tMcuMcuMtInfo.m_tMt.SetMcuId( abyMcuId[0] );
		cServMsg.SetMsgBody( (u8*)&tMcuMcuMtInfo,sizeof( TMcuMcuMtInfo ) );
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_NEWMT_NOTIF,cServMsg );		
	}

	// ����VCS���飬����Ҫ����ѡ�����ش�����
	if (VCS_CONF == m_tConf.GetConfSource())
	{
		
		MtOnlineChange(tNewMt, TRUE, 0);

		if (m_cVCSConfStatus.GetCurVCMT() == tNewMt)
		{
			// �����¼�mcuԭ�ش�ͨ���еĳ�Ա�Ҷ�
			TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tNewMt.GetMcuId());
			if (ptMcInfo != NULL && !(ptMcInfo->m_tMMcuViewMt.IsNull()) &&
				m_tConfAllMtInfo.MtJoinedConf(ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId()) &&
				!(ptMcInfo->m_tMMcuViewMt == tNewMt))
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuNewMtNotify]drop mt(mcuid:%d, mtid:%d) because of new submt(mcuid:%d, mtid:%d) online\n",
					    ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId(), tNewMt.GetMcuId(), tNewMt.GetMtId());
				VCSDropMT(ptMcInfo->m_tMMcuViewMt);	
			}

			OnMMcuSetIn(tNewMt, m_cVCSConfStatus.GetCurSrcSsnId(), SWITCH_MODE_SELECT);
		}

		/*if (!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
		{
			GoOnSelStep(tMt, MODE_VIDEO, TRUE);
			GoOnSelStep(tMt, MODE_AUDIO, TRUE);
			GoOnSelStep(tNewMt, MODE_VIDEO, FALSE);
			GoOnSelStep(tNewMt, MODE_AUDIO, FALSE);
		}*/

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
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody()+sizeof(TMt)+sizeof(TMtAlias) );

	STATECHECK;

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuCallAlertMtNotify]  tMt(%d.%d.%d) byCasLevel(%d)\n",
		tAlertMt.GetMcuId(),
		tAlertMt.GetMtId(),
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		tHeadMsg.m_tMsgSrc.m_byCasLevel
		);

	//u8 byFstMcuId = tMt.GetMtId();
	u8 bySecMcuId = 0;
	
	u8 byReaMtId = 0;
	if( tHeadMsg.m_tMsgSrc.m_byCasLevel > 0 )
	{
		bySecMcuId = tAlertMt.GetMtId();
		if( tHeadMsg.m_tMsgSrc.m_byCasLevel <= MAX_CASCADELEVEL )
		{
			byReaMtId = tHeadMsg.m_tMsgSrc.m_abyMtIdentify[ tHeadMsg.m_tMsgSrc.m_byCasLevel - 1 ];
		}
	}
	else
	{
		byReaMtId = tAlertMt.GetMtId();
	}	
	
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	abyMcuId[0] = tMt.GetMtId();
	abyMcuId[1] = bySecMcuId;

	u16 wMcuIdx = INVALID_MCUIDX;
	if( !m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],2,&wMcuIdx ) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuCallAlertMtNotify] Fail to get mcuIdx by mcuid(%d.%d)\n",
			tMt.GetMtId(),bySecMcuId
			);
		return;
	}
	
	if (!(tMt == m_tCascadeMMCU) ||
		(tMt == m_tCascadeMMCU &&
		 g_cMcuVcApp.IsShowMMcuMtList()))
	{
		//���ӵ��ն˱���
		TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tMt.GetMtId());
		if(tConfMtInfo.IsNull())
		{
			return;
		}
		tConfMtInfo.AddMt( byReaMtId );
		m_tConfAllMtInfo.SetMtInfo( tConfMtInfo );

		//����mc table
		TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->AddMcInfo(wMcuIdx);//tMt.GetMtId());
		if( NULL == ptMcMtInfo )
		{
			return;
		}
		
		s32 nIndex = 0;
		s32 nIdle = MAXNUM_CONF_MT;
		for(; nIndex<MAXNUM_CONF_MT; nIndex++)
		{
			if(ptMcMtInfo->m_atMtStatus[nIndex].IsNull() && nIdle == MAXNUM_CONF_MT &&
				!( TYPE_MT == ptMcMtInfo->m_atMtStatus[nIndex].GetType() &&
				0 == ptMcMtInfo->m_atMtStatus[nIndex].GetMtId() )//mtidΪ0���ն˱Ƚ����⣬����mcu�Լ�
				)
			{
				// �ҵ���һ�������
				nIdle = nIndex;
			}
			if(ptMcMtInfo->m_atMtStatus[nIndex].GetMtId() == byReaMtId )//tAlertMt.GetMtId())
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
		//cServMsg.SetMsgBody( (u8*)&m_tConfAllMcuInfo,sizeof(TConfAllMcuInfo) );
		//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
		SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
		
		ptMcMtInfo->m_atMtStatus[nIndex].SetAudioIn(MEDIA_TYPE_NULL);
		ptMcMtInfo->m_atMtStatus[nIndex].SetAudioOut(MEDIA_TYPE_NULL);
		ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio( FALSE );
		ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio( FALSE );
		
		ptMcMtInfo->m_atMtExt[nIndex].SetMt(tAlertMt);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMt(tAlertMt);
		
		if( mtAliasTypeTransportAddress == tAlertMtAlias.m_AliasType )
		{
			ptMcMtInfo->m_atMtExt[nIndex].SetIPAddr(tAlertMtAlias.m_tTransportAddr.GetIpAddr());
			//zjj20100909 һ��Ҫ�ѱ����ַ�������Ϊ��,�������ն˺�������,�������ʾ���������ǰһ���ն˵ı���
			ptMcMtInfo->m_atMtExt[nIndex].SetAlias( "" );
			//zjj 20090907 ���ð�ip����Ϊ����
			//u32  dwDialIP  = tAlertMtAlias.m_tTransportAddr.GetIpAddr();
			//ptMcMtInfo->m_atMtExt[nIndex].SetAlias(StrOfIP(dwDialIP));
		}
		else
		{
#ifdef _UTF8
			s8 achMtName[VALIDLEN_ALIAS];
			memset(achMtName, 0, sizeof(achMtName));
			memcpy(achMtName, tAlertMtAlias.m_achAlias, VALIDLEN_ALIAS-1);
			CorrectUtf8Str(achMtName, strlen(achMtName));
			ptMcMtInfo->m_atMtExt[nIndex].SetAlias(achMtName);
#else
			ptMcMtInfo->m_atMtExt[nIndex].SetAlias(tAlertMtAlias.m_achAlias);
#endif
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
		TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tMt.GetMtId());
		tMtInfo.RemoveMt( byReaMtId );//tAlertMt.GetMtId() );
		tMtInfo.AddMt( byReaMtId );//tAlertMt.GetMtId() );
		m_tConfAllMtInfo.SetMtInfo(tMtInfo);
		ptMcMtInfo->m_atMtStatus[nIndex].SetIsEnableFECC( FALSE );
		ptMcMtInfo->m_atMtExt[nIndex].SetMcuIdx( wMcuIdx );
		ptMcMtInfo->m_atMtStatus[nIndex].SetMcuIdx( wMcuIdx );
		ptMcMtInfo->m_atMtExt[nIndex].SetMtId( byReaMtId );
		ptMcMtInfo->m_atMtStatus[nIndex].SetMtId( byReaMtId );
		//ptMcMtInfo->m_atMtStatus[nIndex].m_tPartVideoInfo =  tMcuMcuMtInfo.m_tPartVideoInfo;  
		
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuCallAlertMtNotify] nIndex.%d MtId.%d\n", nIndex, tAlertMt.GetMtId() );
		// ��Ҫ�ٱ� [pengguofeng 7/10/2013]
  		SendMtListToMcs( wMcuIdx, FALSE, FALSE );//tMt.GetMtId() );
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuCallAlertMtNotify] tMt<McuId.%d, MtId.%d> is equal to mmcu <McuId.%d, MtId.%d> , IsShowMMcuMtList.%d!\n",
									tMt.GetMcuId(), tMt.GetMtId(), 
									m_tCascadeMMCU.GetMcuId(), m_tCascadeMMCU.GetMtId(),
									g_cMcuVcApp.IsShowMMcuMtList());
	}
		
	if( !m_tCascadeMMCU.IsNull() && 
		MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) &&
		tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
		IsMMcuSupportMultiCascade()
		)
	{
		//tHeadMsg.m_tMsgSrc.m_tMt = tAlertMt;		
		
		cServMsg.SetMsgBody( (u8*)&tAlertMtAlias, sizeof( TMtAlias ) );
		cServMsg.CatMsgBody( (u8*)&tAlertMt,sizeof(TMt) );
		cServMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_CALLALERTING_NOTIF,cServMsg );
	}
	
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
	STATECHECK;

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	TMt		    tDropMt = *(TMt* )(cServMsg.GetMsgBody());
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody() + sizeof(TMt));
	TMt			tVcsVCMT = m_cVCSConfStatus.GetCurVCMT();

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuDropMtNotify]  tMt(%d.%d.%d) tDropMt(%d.%d) byCasLevel(%d)\n",
		tMt.GetMcuId(),
		tMt.GetMtId(),
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		tDropMt.GetMcuId(),
		tDropMt.GetMtId(),
		tHeadMsg.m_tMsgSrc.m_byCasLevel
		);

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMcuMcuDropMt(%d.%d)\n", tDropMt.GetMcuId(), tDropMt.GetMtId());

	u8 bySecMcuId = 0;
	u8 byRealMtId = 0;
	if( tHeadMsg.m_tMsgSrc.m_byCasLevel > 0 )
	{
		bySecMcuId = tDropMt.GetMtId();
		if( tHeadMsg.m_tMsgSrc.m_byCasLevel <= MAX_CASCADELEVEL )
		{
			byRealMtId = tHeadMsg.m_tMsgSrc.m_abyMtIdentify[ tHeadMsg.m_tMsgSrc.m_byCasLevel - 1 ];
		}
	}
	else
	{
		byRealMtId = tDropMt.GetMtId();
	}
	
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	abyMcuId[0] = tMt.GetMtId();
	abyMcuId[1] = bySecMcuId;

	u16 wMcuIdx = INVALID_MCUIDX;
	if( !m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],2,&wMcuIdx ) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuDropMtNotify] Fail to get mcuIdx by mcuid(%d.%d)\n",
			tMt.GetMtId(),bySecMcuId
			);
		return;
	}	
	
	if( !m_tCascadeMMCU.IsNull() && 
		MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) &&
		tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
		IsMMcuSupportMultiCascade()
		)
	{		
		cServMsg.SetMsgBody( (u8*)&tDropMt,sizeof(tDropMt) );
		cServMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_DROPMT_NOTIF,cServMsg );
	}

	tDropMt.SetNull();
	tDropMt.SetMcuIdx( wMcuIdx );
	tDropMt.SetMtId( byRealMtId );
	tDropMt.SetConfIdx( m_byConfIdx );

	//�����ն���Ϊ��ֱ���ն��Ǳ��ط����ˣ���ձ��ط��������
	if ( tDropMt == m_tConf.GetSpeaker() || IsMtInMcu(tDropMt ,  m_tConf.GetSpeaker())  )
	{  
		// [pengjie 2010/9/2] �Ҷ��¼��ն��������ѯ����ֹͣ
		BOOL32 bIsMtInPolling = FALSE;
		if ( CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollState() )
		{
			if (tDropMt == (TMt)m_tConf.m_tStatus.GetMtPollParam() 
				|| (!m_tConf.m_tStatus.GetMtPollParam().IsLocal() 
				     && IsMtInMcu(tDropMt,(TMt)m_tConf.m_tStatus.GetMtPollParam()))
			   )
			{
				bIsMtInPolling = TRUE;
			}
		}
		ChangeSpeaker( NULL, bIsMtInPolling );
	}

	//���ӵ��ն˱���
	TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tDropMt.GetMcuId());
    if(tConfMtInfo.IsNull())
	{
		return;
	}

	tConfMtInfo.RemoveJoinedMt(byRealMtId);//tDropMt.GetMtId());
	m_tConfAllMtInfo.SetMtInfo(tConfMtInfo);
    
    // guzh [3/9/2007] ֪ͨMCS
    MtOnlineChange(tDropMt, FALSE, MTLEFT_REASON_NORMAL);
	ProcConfPollingByRemoveMt( tDropMt );
	ProcVmpPollingByRemoveMt( tDropMt );
	ProcMMcuSpyMtByRemoveMt( tDropMt );

	//�йػ���ϳɵĴ���
	if (VCS_CONF == m_tConf.GetConfSource() && 
		VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode())
	{
		ChangeVmpStyle(tDropMt, FALSE);
	}

	if ( ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
    {
        if ( tDropMt == m_tRollCallee || IsMtInMcu(tDropMt , m_tRollCallee))
        {
            NotifyMcsAlarmInfo( 0, ERR_MCU_ROLLCALL_CALLEELEFT );
            RollCallStop(cServMsg);
        }
    }

	// [pengjie 2010/10/14] �Ҷ��ն�ʱ����ն˼����նˣ�SMCU���µ��ն˵�״̬����Դ
	// 1�������������Ϣ
	RemoveMtFormPeriInfo( tDropMt, MTLEFT_REASON_NORMAL );

	// 2�����ش�ѡ����Ϣ
	TMt tMtSrc, tMtDst;
	TMtStatus tMtStatus;
	for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf(byLoop) )
		{
			m_ptMtTable->GetMtStatus( byLoop, &tMtStatus );
			tMtSrc = tMtStatus.GetSelectMt( MODE_VIDEO );
			
			// �Ҷϵ����¼��նˣ����¼��ն����ڵ�mcu����Ҫͣѡ��
			if( !tMtSrc.IsNull() && 
				(tMtSrc == tDropMt || (IsMtInMcu(tDropMt, tMtSrc)) )
				)
			{
				tMtDst = m_ptMtTable->GetMt( byLoop );
				StopSelectSrc( tMtDst, MODE_VIDEO );
				FreeRecvSpy(tMtSrc, MODE_VIDEO, TRUE);
			}

			tMtSrc = tMtStatus.GetSelectMt( MODE_AUDIO );			
			if( !tMtSrc.IsNull() && 
				(tMtSrc == tDropMt || (IsMtInMcu(tDropMt, tMtSrc)) )
				)
			{
				tMtDst = m_ptMtTable->GetMt( byLoop );
				StopSelectSrc( tMtDst, MODE_AUDIO );
				FreeRecvSpy(tMtSrc, MODE_AUDIO, TRUE);
			}
		}
	}

	if( m_cSMcuSpyMana.IsMtInSpyMember(tDropMt) )
	{
		FreeRecvSpy(tDropMt, MODE_BOTH, TRUE);
	}	

	BOOL32 bIsSendToChairman = FALSE;
    if (m_tApplySpeakQue.IsMtInQueue(tDropMt) )
    {
        m_tApplySpeakQue.ProcQueueInfo(tDropMt, bIsSendToChairman, FALSE);
        NotifyMcsApplyList( bIsSendToChairman );
    }
	
	// 3��������ն���mcu��Ҫɾ�����и�mcu�µĶ�ش���Ϣ
	RemoveMcu( tDropMt );

	// End
	//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
	//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );


	//�����ն˱��ط����ˣ���ձ��ط��������
	if ( VCS_CONF == m_tConf.GetConfSource() )
	{
		if( tDropMt == tVcsVCMT )
		{
			TMt tSpeakMt;		
			if( CONF_SPEAKMODE_ANSWERINSTANTLY == m_tConf.GetConfSpeakMode() &&
				!m_tApplySpeakQue.IsQueueNull() )
			{
				if( m_tApplySpeakQue.GetQueueHead(tSpeakMt) ) 
				{	
					ChgCurVCMT( tSpeakMt );
				}
				else
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcMcuMcuDropMtNotify] Fail to get Queue Head!\n" );
				}
			}
			else
			{
				tSpeakMt.SetNull();
				ChgCurVCMT( tSpeakMt );
			}
		}
		if( ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) )
		{
			VCSClearTvWallChannelByMt( tDropMt,TRUE );
		}
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
	STATECHECK;

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8			byMtId = cServMsg.GetSrcMtId();
	TMt			tDelMt = *(TMt* )(cServMsg.GetMsgBody());
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof( TMt ) );
	

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuDelMtNotify]  tMt(%d.%d.%d) byCasLevel(%d)\n",
		tDelMt.GetMcuId(),
		tDelMt.GetMtId(),
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		tHeadMsg.m_tMsgSrc.m_byCasLevel
		);
	ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "[ProcMcuMcuDelMt(%d.%d)\n", tDelMt.GetMcuId(), tDelMt.GetMtId());

	//u8 byFstMcuId = byMtId;
	u8 bySecMcuId = 0;
	
	u8 byRealMtId = 0;
	if( tHeadMsg.m_tMsgSrc.m_byCasLevel > 0 )
	{
		bySecMcuId = tDelMt.GetMtId();
		if( tHeadMsg.m_tMsgSrc.m_byCasLevel <= MAX_CASCADELEVEL )
		{
			byRealMtId = tHeadMsg.m_tMsgSrc.m_abyMtIdentify[ tHeadMsg.m_tMsgSrc.m_byCasLevel - 1 ];
		}
	}
	else
	{
		byRealMtId = tDelMt.GetMtId();
	}
	
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	abyMcuId[0] = byMtId;
	abyMcuId[1] = bySecMcuId;

	u16 wMcuIdx = INVALID_MCUIDX;
	if( !m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],2,&wMcuIdx ) )
	//if( !m_tConfAllMcuInfo.GetIdxByMcuId( byFstMcuId,bySecMcuId,&wMcuIdx ) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuDelMtNotify] Fail to get mcuIdx by mcuid(%d.%d)\n",
			byMtId,bySecMcuId
			);
		return;
	}	

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuDelMtNotify] Recv Del Mt(%d.%d.%d) Notify\n",
		abyMcuId[0],abyMcuId[1],byRealMtId
		);
	

	if( !m_tCascadeMMCU.IsNull() && 
		MT_TYPE_SMCU == m_ptMtTable->GetMtType( byMtId ) &&
		tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
		IsMMcuSupportMultiCascade()
		)
	{		
		//tHeadMsg.m_tMsgSrc.m_tMt = tDelMt;
		cServMsg.SetMsgBody( (u8*)&tDelMt,sizeof(tDelMt) );
		cServMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_DELMT_NOTIF,cServMsg );
	}

	//TMt tDelMt;
	tDelMt.SetNull();
	tDelMt.SetMcuIdx( wMcuIdx );
	tDelMt.SetMtId( byRealMtId );
	tDelMt.SetConfIdx( m_byConfIdx );

	if ( tDelMt == m_tConf.GetSpeaker() || IsMtInMcu(tDelMt ,  m_tConf.GetSpeaker())  )
	{  
		// [pengjie 2010/9/2] �Ҷ��¼��ն��������ѯ����ֹͣ
		BOOL32 bIsMtInPolling = FALSE;
		if ( CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollState() )
		{
			if (tDelMt == (TMt)m_tConf.m_tStatus.GetMtPollParam() ||
				(!m_tConf.m_tStatus.GetMtPollParam().IsLocal() && 
				 IsMtInMcu(tDelMt,(TMt)m_tConf.m_tStatus.GetMtPollParam())
				) )
			{
				bIsMtInPolling = TRUE;
			}
		}
		ChangeSpeaker( NULL, bIsMtInPolling );
		// End
	}
	//���ӵ��ն˱���
	TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tDelMt.GetMcuId());
    if(tConfMtInfo.IsNull())
	{
		return;
	}
    
	if(tConfMtInfo.MtInConf(byRealMtId))//tDelMt.GetMtId()))
	{
		tConfMtInfo.RemoveMt(byRealMtId);//tDelMt.GetMtId());
		//SendMsgToMt(byMtId, MCU_MCU_DELMT_NOTIF,cServMsg); 
	}
	m_tConfAllMtInfo.SetMtInfo(tConfMtInfo);
	ProcConfPollingByRemoveMt( tDelMt );
	ProcVmpPollingByRemoveMt( tDelMt );
	ProcMMcuSpyMtByRemoveMt( tDelMt );

	// [pengjie 2010/10/14] ɾ���ն�ʱ����ն˼����նˣ�SMCU���µ��ն˵�״̬����Դ
	// 1�������������Ϣ
	RemoveMtFormPeriInfo( tDelMt, MTLEFT_REASON_DELETE );
	
	// 2�����ն˶�ش�ѡ�������Ϣ
	TMt tMtSrc, tMtDst;
	TMtStatus tMtStatus;
	for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf(byLoop) )
		{
			m_ptMtTable->GetMtStatus( byLoop, &tMtStatus );
			tMtSrc = tMtStatus.GetSelectMt( MODE_VIDEO );			
			
			// ɾ�������¼��նˣ����¼��ն����ڵ�mcu����Ҫͣѡ��
			if( !tMtSrc.IsNull() && 
				(tMtSrc == tDelMt || (IsMtInMcu(tDelMt, tMtSrc)) )
				)
			{
				tMtDst = m_ptMtTable->GetMt( byLoop );
				StopSelectSrc( tMtDst, MODE_VIDEO );
				FreeRecvSpy(tMtSrc, MODE_VIDEO, TRUE);
			}

			tMtSrc = tMtStatus.GetSelectMt( MODE_AUDIO );
			if( !tMtSrc.IsNull() && 
				(tMtSrc == tDelMt || (IsMtInMcu(tDelMt, tMtSrc)) )
				)
			{
				tMtDst = m_ptMtTable->GetMt( byLoop );
				StopSelectSrc( tMtDst, MODE_AUDIO );
				FreeRecvSpy(tMtSrc, MODE_AUDIO, TRUE);
			}
		}
	}

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
		// ��Ҫ�ϱ� [pengguofeng 7/10/2013]
		if(bFind)
		{
			SendMtListToMcs(tDelMt.GetMcuId(), FALSE, FALSE);
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

	if( m_cSMcuSpyMana.IsMtInSpyMember(tDelMt) )
	{
		FreeRecvSpy(tDelMt, MODE_BOTH, TRUE);
	}

	// 3��������ն���mcu��Ҫɾ�����и�mcu�µĶ�ش���Ϣ
	RemoveMcu( tDelMt );
	// End

	//������ػ��������ն���Ϣ
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
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
	2007-7-5    4.7.2       pengguofeng    �޸ģ��������ΪINVALID_MCUIDX,���ʾ���������¼�MCU���б�
====================================================================*/
void CMcuVcInst::SendMtListToMcs(u16 wMcuIdx, const BOOL32 bForce/* = true*/,
								 const BOOL32 bMMcuReq /*= FALSE*/ )//(u8 byMcuId)
{
	if ( g_cMcuVcApp.GetMcuMcsMtInfoBufFlag() != 0					// �����˻��巢��
		&& !bForce)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[SendMtListToMcs]use buffer to send\n");

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MCS, "Buf SendMtList of mcu.%d To Mcs\n", wMcuIdx);
		m_cMcuMtListSendBuf.Add(wMcuIdx);
		
		// [11/14/2011 liuxu] ���ڴ�����ʱSendAllMtInfoToAllMcs��Ϣ��̫��, �����ȡ���巢��
		if (m_byAllMtToAllMcsTimerFlag)
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "SendAllMtInfoToAllMcs wait timer, return\n");
			return;
		}else
		{
			m_byAllMtToAllMcsTimerFlag = 1;
			u16 wTemp = SetTimer(MCUVC_SENDALLMT_ALLMCS_TIMER, g_cMcuVcApp.GetRefreshMtInfoInterval());
			return;
		}
	}

	if (CurState() != STATE_ONGOING)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[SendMtListToMcs]wrong state.%d\n", CurState());
		return;
	}

	//��������ն˱�
	TMcu tMcu;
	tMcu.SetNull();
	tMcu.SetMcu( LOCAL_MCUID );
	tMcu.SetMcuIdx( wMcuIdx );
	
	CServMsg cServMsg;

	if(IsLocalMcuId(wMcuIdx))
	{
		tMcu.SetEqpId( m_ptMtTable->m_byMaxNumInUse );

		cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(tMcu) );
		cServMsg.CatMsgBody( ( u8 * )m_ptMtTable->m_atMtExt, 
		                  	 m_ptMtTable->m_byMaxNumInUse * sizeof( TMtExt ) );

		for ( u8 byMtId = 1; byMtId <= m_ptMtTable->m_byMaxNumInUse; byMtId ++ )
		{
			TMtAlias tMtAlias;
			if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeH320Alias, &tMtAlias ))
			{
				if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeH320ID, &tMtAlias ))
				{
					if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeH323ID, &tMtAlias ))
					{
						if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeE164, &tMtAlias ))
						{
							if( !m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeTransportAddress, &tMtAlias ) )
							{
								m_ptMtTable->GetDialAlias( byMtId, &tMtAlias );
							}
						}
					}
				}		
			}
			
			cServMsg.CatMsgBody( (u8*)tMtAlias.m_achAlias, VALIDLEN_ALIAS + MAXLEN_CONFNAME );
		}
	}
	else
	{
		u16 wMMcuIdx = GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId());
		if ( !g_cMcuVcApp.IsShowMMcuMtList() && wMMcuIdx != INVALID_MCUIDX
			  && wMMcuIdx == wMcuIdx )
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[SendMtListToMcs ]NO need to send mtlist to MMcu\n");
			return;
		}

		u8 bySMcuNum = 0xFF;  //��Ҫ����SMCU�ģ���255����Ҫָ���ģ����Լ���֯���ɸ�
		// send REQ to mtadp [pengguofeng 6/7/2013]
		u8 bySend2Mmcu = bMMcuReq?1:0;
		u8 abyMcuId[MAX_CASCADEDEPTH-1];
		memset( &abyMcuId[0],0,sizeof(abyMcuId) );
		CServMsg cSerMsg;
		cSerMsg.SetConfId(m_tConf.GetConfId());
		cSerMsg.SetConfIdx(m_byConfIdx);
 		if ( m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] ) )
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[SendMtListToMcs]SMcu MtId:%d, %d\n", abyMcuId[0], abyMcuId[1]);
			
			bySMcuNum = 1;
			cSerMsg.SetMsgBody(&bySend2Mmcu, sizeof(u8));
			cSerMsg.CatMsgBody(&bySMcuNum, sizeof(u8));
			cSerMsg.CatMsgBody(&abyMcuId[1], sizeof(u8));
			SendMsgToMt(abyMcuId[0], MCU_MTADP_GETMTLIST_CMD, cSerMsg);
			return;
		}
		else
		{
			//foreach every smcu by mtid from 1 to 192
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[SendMtListToMcs]to All sub mcu\n");
			for ( u8 byMtID = 1;byMtID <= MAXNUM_CONF_MT; byMtID++)
			{
			//pengguofeng 2013-7-11:�����ʾ�ϼ���ҲҪ�����ϼ�
				if ( m_ptMtTable->GetMtType(byMtID) == MT_TYPE_SMCU 
					|| m_ptMtTable->GetMtType(byMtID) == MT_TYPE_MMCU 
						&& g_cMcuVcApp.IsShowMMcuMtList()
					)
				{
					cSerMsg.SetMsgBody(&bySend2Mmcu, sizeof(u8));
					cSerMsg.CatMsgBody(&bySMcuNum, sizeof(u8)); //�˴���0xFF
					SendMsgToMt(byMtID, MCU_MTADP_GETMTLIST_CMD, cSerMsg);
				}
			}

			return;
		}
/*		TConfMcInfo* ptMcMtInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);	
		if(ptMcMtInfo == NULL)
		{
			return;
		}
		u16 wMMcuIdx = INVALID_MCUIDX;
		if( !m_tCascadeMMCU.IsNull() )
		{
			wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
			//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
		}

        // guzh [4/30/2007] �ϼ�MCU�б����
        if ( !g_cMcuVcApp.IsShowMMcuMtList() 
			&& !m_tCascadeMMCU.IsNull() 
			&& wMMcuIdx == wMcuIdx )
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
*/	}
	
	SendMsgToAllMcs( MCU_MCS_MTLIST_NOTIF, cServMsg ); // ֻ���ı���! [pengguofeng 7/2/2013]
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

	u16 wMcuIdx = GetMcuIdxFromMcuId(  (u8)ptMtChanStatus->m_tMt.GetMcuId() );
	//m_tConfAllMcuInfo.GetIdxByMcuId( ptMtChanStatus->m_tMt.GetMcuId(),0,&wMcuIdx );

	TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);//ptMtChanStatus->m_tMt.GetMcuId());
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
				BOOL32 bSendVideo = !ptMtChanStatus->m_bMute;
                ptStatus->SetSendVideo(bSendVideo);
			}
			else
			{
				BOOL32 bRecvVideo = !ptMtChanStatus->m_bMute;
                ptStatus->SetReceiveVideo(bRecvVideo);
			}
		}
		else if(ptMtChanStatus->m_byMediaMode == MODE_AUDIO)
		{
			if(ISTRUE(ptMtChanStatus->m_byIsDirectionIn))
			{
				BOOL32 bSendAudio = !(ptMtChanStatus->m_bMute);
                ptStatus->SetSendAudio(bSendAudio);
				//ptStatus->SetAudioIn(GETBBYTE(!(ptMtChanStatus->m_bMute)));
			}
			else
			{
				BOOL32 bSendVideo = !(ptMtChanStatus->m_bMute);
                ptStatus->SetReceiveAudio(bSendVideo);
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
void CMcuVcInst::OnSetOutView( const TMt &tMcuSetInMt, u8 byMode )
{
    if ( m_tCascadeMMCU.IsNull() )
    {
        return;
    }

	TMt tUnlocalSetInMt = tMcuSetInMt;
	TMt tSetInMt = tMcuSetInMt;

	if( !IsLocalAndSMcuSupMultSpy(tSetInMt.GetMcuId()) )
	{
		if ( !tSetInMt.IsLocal() )
		{
			// ����֪ͨ�¼�MCU
			OnMMcuSetIn( tSetInMt, 0, SWITCH_MODE_BROADCAST );
		}
	}

    TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
    if (NULL != ptConfMcInfo && ptConfMcInfo->m_tSpyMt == tSetInMt)
    {
		TMt tSrcMtVid;
		TMt tSrcMtAud;
		tSrcMtVid.SetNull();
		tSrcMtAud.SetNull();

        m_ptMtTable->GetMtSrc( m_tCascadeMMCU.GetMtId(),&tSrcMtVid,MODE_VIDEO );
		m_ptMtTable->GetMtSrc( m_tCascadeMMCU.GetMtId(),&tSrcMtAud,MODE_AUDIO );
		if( MODE_BOTH == byMode )
		{
			if( tSrcMtVid == ptConfMcInfo->m_tSpyMt && tSrcMtAud == ptConfMcInfo->m_tSpyMt )
			{
				// ��ͬ��ֱ�ӷ���
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[OnSetOutView] tSpyMt(%d.%d) is equal tCascadeMMCU's VideoSrc(%d.%d) And AudioSrc(%d.%d).MODE_BOTH\n",
					ptConfMcInfo->m_tSpyMt.GetMcuId(),ptConfMcInfo->m_tSpyMt.GetMtId(),
					tSrcMtVid.GetMcuId(),tSrcMtVid.GetMtId(),
					tSrcMtAud.GetMcuId(),tSrcMtAud.GetMtId()
					);
				return ;
			}
		}
		else if( MODE_VIDEO == byMode )
		{
			if( tSrcMtVid == ptConfMcInfo->m_tSpyMt )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[OnSetOutView] tSpyMt(%d.%d) is equal tCascadeMMCU's VideoSrc(%d.%d).MODE_VIDEO\n",
					ptConfMcInfo->m_tSpyMt.GetMcuId(),ptConfMcInfo->m_tSpyMt.GetMtId(),
					tSrcMtVid.GetMcuId(),tSrcMtVid.GetMtId()					
					);
				return;
			}
		}
		else if( MODE_AUDIO == byMode )
		{
			if( tSrcMtAud == ptConfMcInfo->m_tSpyMt )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[OnSetOutView] tSpyMt(%d.%d) is equal tCascadeMMCU's AudioSrc(%d.%d).MODE_AUDIOn",
					ptConfMcInfo->m_tSpyMt.GetMcuId(),ptConfMcInfo->m_tSpyMt.GetMtId(),
					tSrcMtAud.GetMcuId(),tSrcMtAud.GetMtId()
					);
				return;
			}
		}
    }

	if(!m_tConf.m_tStatus.IsMixing())
	{
		//��Ϊ�ϴ���ѯ���ܵڶ�����ѯ�������ն�,��������������,������������,������ɾ���ϴ�����Ƶ����
		if( !tUnlocalSetInMt.IsLocal() &&
			( m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_SPEAKER_SPY ||			
				m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_SPEAKER_BOTH ||
				m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_SPY||
				m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_BOTH ) 
			)
		{
			StopSpyMtCascaseSwitch( MODE_AUDIO );
		}
	}

   	BOOL32 bMultSpyMt = FALSE;
	if (!tSetInMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tSetInMt.GetMcuId()))
	{
		bMultSpyMt = TRUE;
	}
	else
	{
		tSetInMt = GetLocalMtFromOtherMcuMt(tSetInMt);
	}
    
    //TMt tMMcu;
    //tMMcu.SetMcuId(m_tCascadeMMCU.GetMtId());
    //tMMcu.SetMtId(0);
    //OnMMcuSetIn(tMMcu, 0, SWITCH_MODE_BROADCAST, TRUE); //���л������ˣ���ʱ���ϼ���setin��Ϣ�����ã�����    
   
	if(NULL != ptConfMcInfo)
		ptConfMcInfo->m_tSpyMt = tSetInMt;
    
    //send output notify
    CServMsg cMsg;
    TSetOutParam tOutParam;
	TMsgHeadMsg tHeadMsg;
	memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[OnSetOutView] abyMcuId[0].%d, abyMcuId[1].%d>!\n", 
		tSetInMt.GetMcuId(), tSetInMt.GetMtId());

	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tSetInMt,tSetInMt );
    tOutParam.m_nMtCount = 1;
    tOutParam.m_atConfViewOutInfo[0].m_tMt = tSetInMt;
	if (!bMultSpyMt)
	{
		tOutParam.m_atConfViewOutInfo[0].m_tMt.SetMcuId( LOCAL_MCUID );
	}
	if (NULL != ptConfMcInfo)
	{
		tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
	    tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;
	}
	
	//tHeadMsg.m_tMsgSrc.m_tMt = tSetInMt;
	cMsg.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
	cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
    cMsg.SetEventId(MCU_MCU_SETOUT_NOTIF);
    SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_SETOUT_NOTIF, cMsg);
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[OnSetOutView] abyMcuId[0].%d, abyMcuId[1].%d>!\n", 
		tSetInMt.GetMcuId(), tSetInMt.GetMtId());
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[OnSetOutView] tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0].%d, tHeadMsg.m_tMsgSrc.m_byCasLevel.%d>!\n", 
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0], tHeadMsg.m_tMsgSrc.m_byCasLevel);
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[OnSetOutView] tHeadMsg.m_tMsgDst.m_abyMtIdentify[0].%d, tHeadMsg.m_tMsgDst.m_byCasLevel.%d>!\n", 
		tHeadMsg.m_tMsgDst.m_abyMtIdentify[0], tHeadMsg.m_tMsgDst.m_byCasLevel);

	if(ptConfMcInfo != NULL)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[OnSetOutView] tSpyMt(%d.%d)\n", ptConfMcInfo->m_tSpyMt.GetMcuId(),ptConfMcInfo->m_tSpyMt.GetMtId());
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[OnSetOutView] ptConfMcInfo == NULL!\n");
	}

    NotifyMtSend(tSetInMt.GetMtId(), byMode);
    
    //��������
    if(m_tConf.m_tStatus.IsMixing())
    {
        byMode = MODE_VIDEO;
    }
	//��ͣ�����ϼ�mcu��Ľ�������ͣ�Ļ�StartSwitchToMcu��������佨���ɹ������������ϴ��ϴ��ն˵���Ƶ����
	StopSwitchToSubMt(1,&m_tCascadeMMCU,byMode);
	// ��ش��¼��ն�ʱ����Ҫ�÷�Local���ն˽���ش����������������Local��ĳ�Ա������
	if (bMultSpyMt)
	{
		StartSwitchToMcu(tUnlocalSetInMt, 0, m_tCascadeMMCU.GetMtId(), byMode, SWITCH_MODE_SELECT);
	}
	else
	{
		StartSwitchToMcu(tSetInMt, 0, m_tCascadeMMCU.GetMtId(), byMode, SWITCH_MODE_SELECT);
	}

	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
    if (tConfAttrb.IsResendLosePack())
    {
        //��Ŀ����ƵRtcp������Դ
        u32 dwDstIp = 0;
        u16 wDstPort = 0;
        
		//[2011/08/18/zhangli]����ȡ�˿�����tSetInMt��������¼��ն��Ҷ�ش�����ȡ���ˣ�����BuildRtcpSwitchForSrcToDst
		//�����Ҫ������Ͳ��ý��ˣ�StartSwitchToMcu����������Ĵ���������ܻḲ�ǵ�
        if ((byMode == MODE_VIDEO || byMode == MODE_BOTH) && !IsNeedSelAdpt(tUnlocalSetInMt, m_tCascadeMMCU, MODE_VIDEO))
        {
			BuildRtcpSwitchForSrcToDst(m_tCascadeMMCU, tUnlocalSetInMt, MODE_VIDEO);

//             m_ptMtTable->GetMtLogicChnnl(tSetInMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);
// 
//             dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
//             wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
//             // guzh [7/25/2007] �ش�ͨ��ҲҪMap
//             //�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�        
//             AddRemoveRtcpToDst(dwDstIp, wDstPort, m_tCascadeMMCU, 0, MODE_VIDEO, TRUE);
        }
        
        if ((byMode == MODE_AUDIO || byMode == MODE_BOTH) && !IsNeedSelAdpt(tUnlocalSetInMt, m_tCascadeMMCU, MODE_AUDIO))
        {
			BuildRtcpSwitchForSrcToDst(m_tCascadeMMCU, tUnlocalSetInMt, MODE_AUDIO);

            //��Ŀ����ƵRtcp������Դ
//             m_ptMtTable->GetMtLogicChnnl(tSetInMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE);
//             
//             dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
//             wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
//             AddRemoveRtcpToDst(dwDstIp, wDstPort, m_tCascadeMMCU, 0, MODE_AUDIO, TRUE);
        }
    }   
    
    // guzh [5/9/2007] ֪ͨ�ϼ��µ���Ƶ/��ƵԴ
    NofityMMcuMediaInfo();
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

	u16 wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
	TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx);//m_tCascadeMMCU.GetMtId());
    if(ptConfMcInfo == NULL)
    {
        //NACK
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }    
	//ACK
    SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	if( !IsCanSetOutView(tSetInMt,MODE_BOTH) )
	{	
		//cServMsg.SetErrorCode( wErrorCode );
		//SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	// ���ն˽��ش�ͨ��֮ǰͨ�������ն˴��������ͷŻش�ͨ��
	TMtStatus tMtStatus;
	m_ptMtTable->GetMtStatus(m_tCascadeMMCU.GetMtId(), &tMtStatus);
	TMt tLastSpyMt = tMtStatus.GetVideoMt();
	if ( !tLastSpyMt.IsNull() )
	{
		FreeRecvSpy( tLastSpyMt, MODE_BOTH );
	}

    // �ϼ����¼���PreSetIn�����ɴ˴�ͳһ�������¼��ն˽��ϴ�ͨ���ش�
    if( !tSetInMt.IsLocal() &&
		IsLocalAndSMcuSupMultSpy(tSetInMt.GetMcuId()) 
		)
	{
		TPreSetInReq tSpySrcInitInfo;
		tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt( tSetInMt );
		tSpySrcInitInfo.m_bySpyMode = MODE_BOTH;
		tSpySrcInitInfo.SetEvId(MCS_MCU_SPECOUTVIEW_REQ);
		
		// ��Ŀ�Ķ�����
		TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSetInMt.GetMcuId()) );	
		// �����ǰ�ն��ѻش���������Ҫ���ѻش�Ŀ��������ȡС
		if(!GetMinSpyDstCapSet(tSetInMt, tSimCapSet))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcPollingChangeTimerMsg] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
				tSetInMt.GetMcuId(), tSetInMt.GetMtId() );
			return;
		}
		tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );

		OnMMcuPreSetIn( tSpySrcInitInfo );			
		return;
	}

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

	TMsgHeadMsg tHeadMsg,tHeadMsgRsp;
	if( cServMsg.GetMsgBodyLen() > (sizeof(TMcuMcuReq)+sizeof(TSetInParam)) )
	{
		tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq)+sizeof(TSetInParam));		
		tHeadMsgRsp.m_tMsgSrc = tHeadMsg.m_tMsgDst;
		tHeadMsgRsp.m_tMsgDst = tHeadMsg.m_tMsgSrc;
	}
	

	STATECHECK;

	if( ptSetInParam->m_tMt.IsMcuIdLocal() &&
		ptSetInParam->m_tMt.GetMtId() == byMtId &&
		m_ptMtTable->GetMtType(byMtId) == MT_TYPE_SMCU
		)
	{
		//ACK		
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
	}
	
	TMt tRealSetInMt = ptSetInParam->m_tMt;
	if( !ptSetInParam->m_tMt.IsMcuIdLocal() )
	{
		ptSetInParam->m_tMt = m_ptMtTable->GetMt( (u8)ptSetInParam->m_tMt.GetMcuId() );
	}
	else
	{
		ptSetInParam->m_tMt = m_ptMtTable->GetMt(ptSetInParam->m_tMt.GetMtId());
	}

	//tHeadMsgRsp.m_tMsgSrc.m_tMt = ptSetInParam->m_tMt;
	cServMsg.SetMsgBody( (u8*)&tHeadMsgRsp,sizeof(TMsgHeadMsg) );
	cServMsg.CatMsgBody( (u8*)&tRealSetInMt,sizeof(TMt) );
	
	

	u16 wMMcuIdx = GetMcuIdxFromMcuId( byMtId );
	//m_tConfAllMcuInfo.GetIdxByMcuId( byMtId,0,&wMMcuIdx );
	TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx);
	if(ptConfMcInfo == NULL)
	{
		//NACK		
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
    
    //zbq [12/25/2009] ������setout�����������ػش���ѯֹͣ
    u8 byPollMode = m_tConf.m_tStatus.GetPollMode();
    if (CONF_POLLMODE_NONE != byPollMode)
    {
        switch (byPollMode)
        {
        case CONF_POLLMODE_VIDEO_SPY:
        case CONF_POLLMODE_SPEAKER_SPY:
            ProcStopConfPoll();
            NotifyMcsAlarmInfo(0, ERR_MCU_CASPOLL_STOP);
            break;

        case CONF_POLLMODE_VIDEO_BOTH:
            m_tConf.m_tStatus.SetPollMedia(MODE_VIDEO);
            m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_VIDEO);
            NotifyMcsAlarmInfo(0, ERR_MCU_CASPOLL_STOP);
            break;
        
        case CONF_POLLMODE_SPEAKER_BOTH:
            m_tConf.m_tStatus.SetPollMedia(MODE_BOTH);
            m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_SPEAKER);
            NotifyMcsAlarmInfo(0, ERR_MCU_CASPOLL_STOP);
        	break;

		default:
			break;
        }
    }

	TMt tMt;
	//tMt.SetMcuId(byMtId);
	//tMt.SetMtId(0);
	//OnMMcuSetIn(tMt, 0, SWITCH_MODE_BROADCAST, TRUE); //���л������ˣ���ʱ���ϼ���setin��Ϣ�����ã�����
	
	
	
	if( tRealSetInMt.IsMcuIdLocal() )
	{
		//send output notify
		CServMsg cMsg;
		TSetOutParam tOutParam;
		tOutParam.m_nMtCount = 1;
		tOutParam.m_atConfViewOutInfo[0].m_tMt = tRealSetInMt;//ptSetInParam->m_tMt;//m_ptMtTable->GetMt(byMtId);
		tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
		tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;
		memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
		tHeadMsg = tHeadMsgRsp;
		//tHeadMsg.m_tMsgSrc.m_tMt = tRealSetInMt;//ptSetInParam->m_tMt;
		cMsg.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
		cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		
		cMsg.SetEventId(MCU_MCU_SETOUT_NOTIF);
		SendMsgToMt(byMtId, MCU_MCU_SETOUT_NOTIF, cMsg);
	}

	ptConfMcInfo->m_tSpyMt = ptSetInParam->m_tMt;
	NotifyMtSend( ptSetInParam->m_tMt.GetMtId(), MODE_BOTH);
	TMt tDstMt = m_ptMtTable->GetMt(byMtId);
	TMt tSrcMt = ptSetInParam->m_tMt;

	
	


	NofityMMcuMediaInfo();
	
   
	//��������
	u8 byMode = MODE_BOTH;

	
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
	
	
	

	if( m_tConf.m_tStatus.IsMixing() || 
		( !m_tMixEqp.IsNull() && 
		tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState != TMixerGrpStatus::READY
		)
		)
	{
		byMode = MODE_VIDEO;
	}

	tSrcMt.SetMcuIdx( LOCAL_MCUIDX );
    StartSwitchToMcu(tSrcMt, 0, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT);
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
    if (tConfAttrb.IsResendLosePack())
    {
		u32 dwDstIp  = 0;
		u16 wDstPort = 0;
		TLogicalChannel tLogicalChannel;
		if ((MODE_VIDEO == byMode || MODE_BOTH == byMode))
		{
			if (IsNeedSelAdpt(tSrcMt, tDstMt, MODE_VIDEO))
			{
				TBasOutInfo tOutInfo;
				if (FindBasChn2SelForMt(tSrcMt, tDstMt, MODE_VIDEO, tOutInfo))
				{
					TTransportAddr tBasWaitRtcpAddr;
					if (GetRemoteRtcpAddr(tOutInfo.m_tBasEqp, tOutInfo.m_byFrontOutNum + tOutInfo.m_byOutIdx, MODE_VIDEO, tBasWaitRtcpAddr))
					{
						AddRemoveRtcpToDst(tBasWaitRtcpAddr.GetIpAddr(), tBasWaitRtcpAddr.GetPort(), tDstMt, 0, MODE_VIDEO, TRUE);
					}
					else
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcMcuMcuSetInReq] GetPrsRemoteAddr:Video failed!\n");
					}				
				}
				else
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcMcuMcuSetInReq] FindBasChn2SelForMt:Video failed!\n");
				}
			}
			else
			{			
				m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);				
				dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
				wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();       
				AddRemoveRtcpToDst(dwDstIp, wDstPort, tDstMt, 0, MODE_VIDEO, TRUE);
			}
		}
        
		if ((MODE_AUDIO == byMode || MODE_BOTH == byMode))
		{
			if (IsNeedSelAdpt(tSrcMt, tDstMt, MODE_AUDIO))
			{
				TBasOutInfo tOutInfo;
				if (FindBasChn2SelForMt(tSrcMt, tDstMt, MODE_AUDIO, tOutInfo))
				{
					TTransportAddr tBasWaitRtcpAddr;
					if (GetRemoteRtcpAddr(tOutInfo.m_tBasEqp, tOutInfo.m_byFrontOutNum + tOutInfo.m_byOutIdx, MODE_AUDIO, tBasWaitRtcpAddr))
					{
						AddRemoveRtcpToDst(tBasWaitRtcpAddr.GetIpAddr(), tBasWaitRtcpAddr.GetPort(), tDstMt, 0, MODE_AUDIO, TRUE);
					}
					else
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcMcuMcuSetInReq] GetPrsRemoteAddr:Audio failed!\n");
					}				
				}
				else
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcMcuMcuSetInReq] FindBasChn2SelForMt:Audio failed!\n");
				}
			}
			else
			{
				//��Ŀ����ƵRtcp������Դ
				m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE);				
				dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
				wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
				AddRemoveRtcpToDst(dwDstIp, wDstPort, tDstMt, 0, MODE_AUDIO, TRUE);
			}
		}     
    }

	//ACK
	
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	if( !tRealSetInMt.IsMcuIdLocal() )
	{
		tMt = GetMcuIdxMtFromMcuIdMt( tRealSetInMt );
		// [12/8/2010 xliang] further send SetInReq to it's smcu no matter multiSpy support or not
// 		if( IsLocalAndSMcuSupMultSpy( tMt.GetMcuId() ) )
// 		{
// 			TPreSetInReq tPreSetInReq;
// 			tPreSetInReq.m_tSpyMtInfo.SetSpyMt(tMt);
// 			tPreSetInReq.m_bySpyMode = byMode;
// 			tPreSetInReq.m_tSpyInfo.m_tSpySwitchInfo.m_tDstMt = m_tCascadeMMCU;
// 			//�ݶ��¼�IDΪѡ��
// 			tPreSetInReq.m_dwEvId = MCS_MCU_STARTSWITCHMT_REQ;
// 			if( m_cSMcuSpyMana.GetFstSpyMtByMcuIdx(tMt.GetMcuId(),tMt) )
// 			{
// 				tPreSetInReq.m_tReleaseMtInfo.m_tMt = tMt;
// 				tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = byMode;
// 				tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 
// 				tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
// 			}			
// 			tPreSetInReq.m_tReleaseMtInfo.m_swCount = 0;
// 			
// 			// [pengjie 2010/9/13] ��Ŀ�Ķ�����
// 			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tMt.GetMcuId()) );	
// 			//zjl20101116 �����ǰ�ն��ѻش���������Ҫ���ѻش�Ŀ��������ȡС
// 			tSimCapSet = GetMinSpyDstCapSet(tMt, tSimCapSet);
// 			if(tSimCapSet.IsNull())
// 			{
// 				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ChangeSpeaker] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
// 					tMt.GetMcuId(), tMt.GetMtId() );
// 				return;
// 			}
// 			
// 			tPreSetInReq.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
// 		    // End
// 
// 			OnMMcuPreSetIn( tPreSetInReq );
// 		}
// 		else
		{
			OnMMcuSetIn(tMt, 0, SWITCH_MODE_SELECT);
		}
		
	}
	
	if( !m_tCascadeMMCU.IsNull() && m_ptMtTable->GetMtTransE1(m_tCascadeMMCU.GetMtId()) )
	{
		ProcMMcuBandwidthNotify( m_tCascadeMMCU,GetRealSndSpyBandWidth() );
	}

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
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TCConfViewOutInfo));

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcuMcuSetOutNotify] ptSetOutParam->m_tMt<McuId.%d, MtId.%d>!\n", 
											  ptSetOutParam->m_tMt.GetMcuId(), ptSetOutParam->m_tMt.GetMtId());

	STATECHECK;
	
	if (!ptSetOutParam)
	{
		return;
	}

	u8 byFstMcuId = byMtId;
	u8 bySecMcuId = 0;
	
	if( tHeadMsg.m_tMsgSrc.m_byCasLevel > 0 )
	{
		bySecMcuId = ptSetOutParam->m_tMt.GetMtId();
	}
	
	u16 wMcuIdx = INVALID_MCUIDX;
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	abyMcuId[0] = byFstMcuId;
	abyMcuId[1] = bySecMcuId;
	if( !m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],2,&wMcuIdx ) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuCallAlertMtNotify] Fail to get mcuIdx by mcuid(%d.%d)\n",
			byFstMcuId,bySecMcuId
			);
		return;
	}
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMcuMcuSetOutNotify] abyMcuId[0].%d, abyMcuId[1].%d>!\n", 
		byFstMcuId, bySecMcuId);
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMcuMcuSetOutNotify] tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0].%d, tHeadMsg.m_tMsgSrc.m_byCasLevel.%d>!\n", 
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0], tHeadMsg.m_tMsgSrc.m_byCasLevel);
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMcuMcuSetOutNotify] tHeadMsg.m_tMsgDst.m_abyMtIdentify[0].%d, tHeadMsg.m_tMsgDst.m_byCasLevel.%d>!\n", 
		tHeadMsg.m_tMsgDst.m_abyMtIdentify[0], tHeadMsg.m_tMsgDst.m_byCasLevel);

	//if(ptSetOutParam->m_tMt.IsNull())
	//{
	//	return;
	//}
	TMt tSetoutMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,ptSetOutParam->m_tMt );//ptSetOutParam->m_tMt;
	//tSetoutMt.SetMcuIdx( wMcuIdx );
	
	wMcuIdx = tSetoutMt.GetMcuId();
	
	//�ǵ�2�����3���ն˽��ϴ�ͨ������ʱӦ��ȡ��2����mcinfo
	//3�����������淽������չ��Ҫ�޸�
	if ( tHeadMsg.m_tMsgDst.m_byCasLevel == 1 && abyMcuId[1] != 0 ) 
	{
		wMcuIdx = GetMcuIdxFromMcuId(GetFstMcuIdFromMcuIdx(tSetoutMt.GetMcuId())) ;
	}
	else // >1���������ֻ��=2Ҳ���ǵ�3���նˣ��Ժ���չelse����Ҫ�޸�
	{
		
	}
	TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );
	if(ptInfo == NULL)
	{
		return;
	}
	TConfMcInfo * ptSetoutMtMcinfo = m_ptConfOtherMcTable->GetMcInfo(tSetoutMt.GetMcuId());
	if(ptInfo == NULL || ptSetoutMtMcinfo== NULL )
	{
		return;
	}
	TMcMtStatus *ptMcMtStatus = ptSetoutMtMcinfo->GetMtStatus(tSetoutMt);//ptSetOutParam->m_tMt);
	if(ptMcMtStatus ==  NULL)
	{
		return;
	}
    TMtVideoInfo tInfo = ptMcMtStatus->GetMtVideoInfo();
	tInfo.m_nOutputLID = ptSetOutParam->m_nOutViewID;
	tInfo.m_nOutVideoSchemeID = ptSetOutParam->m_nOutVideoSchemeID;
    ptMcMtStatus->SetMtVideoInfo(tInfo);

	//TMt tOldSrc;
    //zjl[20091226]�µĻش�Դ���ϼ���ͳһ������������ͨ���ĳ�Ա״̬
	TMt tNewSrc = ptInfo->m_tMMcuViewMt;
	if( tNewSrc.IsNull() || !( tNewSrc==tSetoutMt ) )
	{
		// liuxu, 20110120, ��ֹptSetOutParam->m_tMtΪ�Ƿ�ֵ
		// ��Ե�����: sp4����r2ʱ, r2���ն˽���sp4�ĵ���ǽ,������ʾ��ȷ������
		//             ԭ������r2�������ϱ���һ�������tmt
		if ( ptSetOutParam->m_tMt.IsNull() 
			|| (! ptSetOutParam->m_tMt.IsNull() && 0 != ptSetOutParam->m_tMt.GetMtId()))
		{
			// [2013/10/11 chenbing] 
			// �µ��ϴ�ԴΪ�գ��������һ�ϴ�Դ��ͼ��
			// ��ֹ�¼������ϴ���ѯ���ϼ���δ��ѯ�ն˽�����������ʱ�����������һ�ϴ�Դͼ��
			if (tSetoutMt.IsNull())
			{
				RefreshMtStatusInTw(ptInfo->m_tLastMMcuViewMt, FALSE, TRUE);
			}
			ptInfo->m_tLastMMcuViewMt = ptInfo->m_tMMcuViewMt;
			tNewSrc = tSetoutMt;//ptSetOutParam->m_tMt;
			ptInfo->m_tMMcuViewMt = tNewSrc;
		}
	}

	// [3/28/2011 xliang] tOldSrc������ķ�ʽȷ��������
	// ��ֻ�������¼�ָ���ϴ��ն˵�����������ϼ�ָ���ش�Դ����ֵʧȥ����
	TMt tOldSrc = ptInfo->m_tLastMMcuViewMt;
	
	BOOL32 bIsStartSwitch = TRUE;
	// [2013/04/26 chenbing] ���ش������ֱ����߼�
	if ( !IsLocalAndSMcuSupMultSpy(tSetoutMt.GetMcuId()))
	{
		if (!tNewSrc.IsNull())
		{
			//���ش��£��µ��ϴ�Դ�ֱ��ʱ����µ�
			if ( IsViewMtInHduVmp(tNewSrc, &tOldSrc) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuSetOutNotify] !IsSupMultSpy AdjustResAndFps\n");
				ChangeMtResFpsInHduVmp(tNewSrc, 0, 0, 0, bIsStartSwitch, TRUE, TRUE);
 				if (!bIsStartSwitch)
 				{
 					TMt tTMtTemp = tOldSrc.IsNull() ? GetLocalMtFromOtherMcuMt(tNewSrc) : tOldSrc;
 					// �ǿƴ��ն˲��ܽ��ֱ��ʲ����HDU2�໭��Ľ���
 					StopViewToAllHduVmpSwitch(tTMtTemp);
 				}
			}
		}

		if ( !IsOldViewMtAndNewViewMtSame(tNewSrc, tOldSrc) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuSetOutNotify] !IsSupMultSpy Recover AdjustResAndFps\n");
			// �ָ����ϴ�Դ��ֱ���(���ش�����Ҫǿ�ƻָ�)
			ChangeMtResFpsInHduVmp(tOldSrc, 0, 0, 0, bIsStartSwitch, FALSE, TRUE);
		}
	}
	else// [2013/04/26 chenbing] ��ش������ֱ����߼�
	{	
		if (!tNewSrc.IsNull())
		{
			//MCU��Mt��ͨ����, ��Ҫ��
			if (IsViewMtInHduVmp(tNewSrc) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuSetOutNotify] IsSupMultSpy AdjustResAndFps\n");
				ChangeMtResFpsInHduVmp(tNewSrc, 0, 0, 0, bIsStartSwitch, TRUE, TRUE);
 				if (!bIsStartSwitch)
 				{
 					TMt tTMtTemp = GetLocalMtFromOtherMcuMt(tNewSrc);
 					// �ǿƴ��ն˲��ܽ��ֱ��ʲ���¼�Mcu��HDU2�໭��Ľ���
 					StopViewToAllHduVmpSwitch(tTMtTemp);
 				}
			}
		}

		// �ָ����ϴ�Դ��ֱ���
		if ( !IsViewMtInHduVmp(tOldSrc, NULL, FALSE) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuSetOutNotify] IsSupMultSpy Recover AdjustResAndFps\n");
			ChangeMtResFpsInHduVmp(tOldSrc, 0, 0, 0, bIsStartSwitch, FALSE);
		}
	}

	// [10/18/2011 liuxu] hdu����ǽͨ������
	if( m_tConf.GetConfSource() != VCS_CONF 
		|| ( m_tConf.GetConfSource() == VCS_CONF ) 
			&& ( VCS_TVWALLMANAGE_REVIEW_MODE != m_cVCSConfStatus.GetTVWallManageMode() ) )
	{
		// ����hdu�豸״̬
		UpdateTvwOnSMcuViewMtChg( tNewSrc, TRUE, &tOldSrc );

		// ����tvwall�豸״̬( ���Զ������ֱ��� ) 
		UpdateTvwOnSMcuViewMtChg( tNewSrc, FALSE, &tOldSrc );
	}

// 	//hdu
// 	//zjj20091128 vcs���鲻Ҫȥ����hdu���նˣ���ΪvcsԤ��ģʽ�µ���ǽ����Ƚ������п��ܻ��Ų��ǻش�Դ���ն���Ϣ
//     if ( /*!IsLocalAndSMcuSupMultSpy( tNewSrc.GetMcuIdx() ) &&*/
// 		m_tConf.GetConfSource() != VCS_CONF &&
// 		tNewSrc.GetType() != TYPE_MCUPERI && !tNewSrc.IsLocal())
// 	{
// 		for (u8 byIdx = HDUID_MIN; byIdx <= HDUID_MAX; byIdx++)
// 		{
// 			if (!g_cMcuVcApp.IsPeriEqpConnected(byIdx))
// 			{
// 				continue;
// 			}
// 
// 			u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byIdx));
// 			if (0 == byHduChnNum)
// 			{
// 				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuSetOutNotify] GetHduChnNumAcd2Eqp failed!\n");
// 				continue;
// 			}
// 
// 			TPeriEqpStatus tStatus;
// 			g_cMcuVcApp.GetPeriEqpStatus(byIdx, &tStatus);
// 			//20101015_tzy �ϴ�ͨ�����ն˸ı�ʱHDU����ǽͼ��ˢ��
// 			//�����¼���������п��ǣ����ǳ����ϴ��ն�ʱ�Ĳ�����ע����������ǽ�����𣬱��廹Ҫ�����ֱ���
// 			//��������HDUͨ��
// 			//������ǽͨ����Ϊ�¼�MCUʱ���ַ����������
// 				//1.���ϴ��ն����ڸ��¼�
// 					//���ش�ʱ��ͨ�����ն˲��䣬��ԭ���ϴ��ն˵���ǽͼ��ˢ�����µ��ϴ��ն˵���ǽͼ��ˢ��
// 					//��ش�ʱ��//UNDO
// 				//1.���ϴ��ն˲����ڸ��¼�MCUʱ
// 					//UNDO
// 			//������ǽͨ����Ϊ�¼��ն�ʱ���ַ����������
// 				//1.���ϴ��ն����ڸ��¼�MCUʱ
// 					//���ش�ʱ��ͨ�����ն˸�Ϊ�µ��ϴ��նˣ���ԭ���ϴ��ն˵���ǽͼ��ˢ�����µ��ϴ��ն˵���ǽͼ��ˢ��
// 					//��ش�ʱ��//UNDO
// 				//1.���ϴ��ն˲����ڸ��¼�MCUʱ
// 					//UNDO
// 
// 			for (u8 byChnId = 0; byChnId < byHduChnNum; byChnId++)//��������HDUͨ��
// 			{
// 				TMtStatus tMtStatus;
// 				TMt tCurSpyMt = (TMt)tStatus.m_tStatus.tHdu.atVideoMt[byChnId];
// 
// 				//ͨ���ն��Ƿ�Ϸ����
// 				if (tCurSpyMt.IsNull() || tCurSpyMt == m_tVmpEqp || tCurSpyMt.GetConfIdx() != m_byConfIdx )
// 				{
// 					continue;
// 				}
// 				
// 				// 2011-10-13 add by pgf: ��������������жϻ����
// 				if ( tCurSpyMt.GetConfIdx() != m_byConfIdx
// 					|| tNewSrc.GetConfIdx() != m_byConfIdx
// 					|| tOldSrc.GetConfIdx() != m_byConfIdx)
// 				{
// 					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "CONF IDX: m_byConfIdx:%d curspymt<%d> or mmcuviewmt<old:%d new:%d>\n",
// 						m_byConfIdx, tCurSpyMt.GetConfIdx(), tNewSrc.GetConfIdx(), tOldSrc.GetConfIdx());
// 					continue;
// 				}
// 
// 				// [pengjie 2011/9/27] ˢ�¼�mcu���ϴ�Դ״̬
// 				if( /*tNewSrc.IsNull() &&*/ IsMtInMcu(tCurSpyMt, tOldSrc) && IsMtNotInOtherHduChnnl(tOldSrc, 0, 255))
// 				{
// 					RefreshMtStatusInTw( tOldSrc, FALSE, TRUE );
// 				}
// 				
// 				if( !tNewSrc.IsNull() && IsSMcuViewMtAndInTw( tNewSrc, TRUE )/*IsMtInMcu(tCurSpyMt, tNewSrc, TRUE)*/)
// 				{
// 					RefreshMtStatusInTw( tNewSrc, TRUE, TRUE );
// 				}
// 				
// // 				// [pengjie 2011/9/27] ˢ�¼�mcu���ϴ�Դ״̬
// // 				if( IsMtInMcu(tCurSpyMt, tNewSrc) || (tNewSrc.IsNull() && IsMtInMcu(tCurSpyMt, tOldSrc)) )
// // 				{
// // 					RefreshMtStatusInTw( tOldSrc, FALSE, TRUE );
// // 					
// // 					if( !tNewSrc.IsNull() )
// // 					{
// // 						RefreshMtStatusInTw( tNewSrc, TRUE, TRUE );
// // 					}
// // 				}
// 
// 				//ͨ���ն�Ϊ�¼��ն˲��ҵ��ش�ʱ
// 				if (!tCurSpyMt.IsLocal() 
// 					&& !IsLocalAndSMcuSupMultSpy(tCurSpyMt.GetMcuIdx()) 
// 					&& !(tCurSpyMt == tNewSrc)
// 					&& (!IsMtInMcu(tCurSpyMt, tNewSrc) && !IsMtInMcu(tNewSrc, tCurSpyMt)))
// 				{
// 					GetMtStatus(tCurSpyMt, tMtStatus);
// 					tMtStatus.SetInHdu(FALSE);
// 					SetMtStatus(tCurSpyMt, tMtStatus);
// 					MtStatusChange(&tCurSpyMt, TRUE);
// 					
// 					GetMtStatus(tNewSrc, tMtStatus);
// 					tMtStatus.SetInHdu(TRUE);
// 					SetMtStatus(tNewSrc, tMtStatus);
// 					MtStatusChange(&tNewSrc, TRUE);
// 
// 					tStatus.m_tStatus.tHdu.atVideoMt[byChnId].SetMt(tNewSrc);
// 					g_cMcuVcApp.SetPeriEqpStatus(byIdx, &tStatus);
// 					cServMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
// 					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
// 				}
// 			}	
// 		}				
// 	}
		
// 	//dec5
// 	//zjj20091128 vcs���鲻Ҫȥ����hdu���նˣ���ΪvcsԤ��ģʽ�µ���ǽ����Ƚ������п��ܻ��Ų��ǻش�Դ���ն���Ϣ
// 	if ( /*!IsLocalAndSMcuSupMultSpy( tNewSrc.GetMcuIdx() ) &&*/ // [pengjie 2010/10/13] ���ﲻ���ֶ�ش�
// 		tNewSrc.GetType() != TYPE_MCUPERI 
// 		&& !tNewSrc.IsLocal()
// 		// [10/17/2011 liuxu] VCSģʽ����Ҫ����״̬
// 		&& ( m_tConf.GetConfSource() != VCS_CONF 
// 			|| ( m_tConf.GetConfSource() == VCS_CONF ) && ( VCS_TVWALLMANAGE_REVIEW_MODE != m_cVCSConfStatus.GetTVWallManageMode() )  )
// 		)
// 	{
// 		for (u8 byIdx = TVWALLID_MIN; byIdx <= TVWALLID_MAX; byIdx++)
// 		{
// 			if (!g_cMcuVcApp.IsPeriEqpConnected(byIdx))
// 			{
// 				continue;
// 			}
// 			TPeriEqpStatus tStatus;
// 			g_cMcuVcApp.GetPeriEqpStatus(byIdx, &tStatus);
// 			for (u8 byChnId = 0; byChnId < MAXNUM_PERIEQP_CHNNL; byChnId++)
// 			{	
// 				//20101021_tzy �ϴ�ͨ�����ն˸ı�ʱ����ǽͼ��ˢ��
// 				TMtStatus tMtStatus;
// 				TMt tCurSpyMt = (TMt)tStatus.m_tStatus.tTvWall.atVideoMt[byChnId];
// 				if (tCurSpyMt.IsNull() || tCurSpyMt == m_tVmpEqp)//ͨ���ն��Ƿ�Ϸ����
// 				{
// 					continue;
// 				}
// 
// 				if (tCurSpyMt.IsLocal() 
// 					&& m_ptMtTable->GetMtType(tCurSpyMt.GetMtId()) == MT_TYPE_SMCU
// 					&& !(tCurSpyMt == tNewSrc)
// 					// [8/31/2011 liuxu] ���ն��Ǹ�mcu�¼�ֱ���ն˲Ž����������
// 					&& (IsMtInMcu(tCurSpyMt, tNewSrc, TRUE)
// 					    || (tNewSrc.IsNull() && IsMtInMcu(tCurSpyMt, tOldSrc, TRUE))))
// 				{
// 					u8 byTempMtId = tCurSpyMt.GetMtId();
// 					TLogicalChannel tLogicChannel;
// 					if ( m_tConfAllMtInfo.MtJoinedConf(byTempMtId )
// 						&& m_ptMtTable->GetMtLogicChnnl( byTempMtId, LOGCHL_VIDEO, &tLogicChannel, FALSE )
// 						&& MEDIA_TYPE_H264 == tLogicChannel.GetChannelType()	//Ŀǰ�ĵ����ֱ��ʾ������h264
// 						&& tLogicChannel.GetVideoFormat() != VIDEO_FORMAT_CIF)
// 					{
// 						//�ָ�ԭ���ն˷ֱ���
// 						if (!tOldSrc.IsNull())
// 						{
// 							CascadeAdjMtRes( tOldSrc, tLogicChannel.GetVideoFormat(), FALSE);
// 						}
// 						//���¼��ն˷ֱ���
// 						if (!tNewSrc.IsNull())
// 						{
// 							CascadeAdjMtRes( tNewSrc, VIDEO_FORMAT_CIF, TRUE);
// 						}
// 					}
// 				}
// 
// 				// [pengjie 2011/9/27] ˢ�¼�mcu���ϴ�Դ״̬
// 				if( /*tNewSrc.IsNull() && */IsMtInMcu(tCurSpyMt, tOldSrc) && IsMtNotInOtherHduChnnl(tOldSrc, 0, 255) )
// 				{
// 					RefreshMtStatusInTw( tOldSrc, FALSE, FALSE );
// 				}
// 
// 				if( !tNewSrc.IsNull() && IsSMcuViewMtAndInTw( tNewSrc, FALSE )/*IsMtInMcu(tCurSpyMt, tNewSrc, TRUE)*/)
// 				{
// 					RefreshMtStatusInTw( tNewSrc, TRUE, FALSE );
// 				}
// 				
// 				if (!tCurSpyMt.IsLocal() 
// 					&& !IsLocalAndSMcuSupMultSpy(tCurSpyMt.GetMcuIdx()) 
// 					&& !(tCurSpyMt == tNewSrc))//ͨ���ն�Ϊ�¼��ն˲��ҵ��ش�ʱ
// 				{
// 					GetMtStatus(tCurSpyMt, tMtStatus);
// 					tMtStatus.SetInTvWall(FALSE);
// 					SetMtStatus(tCurSpyMt, tMtStatus);
// 					MtStatusChange(&tCurSpyMt, TRUE);
// 					GetMtStatus(tNewSrc, tMtStatus);
// 					tMtStatus.SetInTvWall(TRUE);
// 					SetMtStatus(tNewSrc, tMtStatus);
// 					MtStatusChange(&tNewSrc, TRUE);
// 
// 					tStatus.m_tStatus.tTvWall.atVideoMt[byChnId].SetMt(tNewSrc);
// 					//tStatus.m_tStatus.tHdu.atVideoMt[byChnId].SetMt(tNewSrc);
// 					g_cMcuVcApp.SetPeriEqpStatus(byIdx, &tStatus);
// 					cServMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
// 					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
// 					u8 byLocalMtId = GetLocalMtFromOtherMcuMt(tCurSpyMt).GetMtId();
// 					TLogicalChannel tLogicChannel;
// 					if ( m_tConfAllMtInfo.MtJoinedConf(byLocalMtId )
// 						&& m_ptMtTable->GetMtLogicChnnl( byLocalMtId, LOGCHL_VIDEO, &tLogicChannel, FALSE )
// 						&& MEDIA_TYPE_H264 == tLogicChannel.GetChannelType()	//Ŀǰ�ĵ����ֱ��ʾ������h264
// 						&& tLogicChannel.GetVideoFormat() != VIDEO_FORMAT_CIF)
// 					{
// 						//�ָ�ԭ���ն˷ֱ���
// 						if (!tCurSpyMt.IsNull())
// 						{
// 							CascadeAdjMtRes( tCurSpyMt, tLogicChannel.GetVideoFormat(), FALSE);
// 						}
// 						//���¼��ն˷ֱ���
// 						if (!tNewSrc.IsNull())
// 						{
// 							CascadeAdjMtRes( tNewSrc, VIDEO_FORMAT_CIF, TRUE);
// 						}
// 					}
// 				}
// 
// 				// [8/31/2011 liuxu] ��ش��ϴ�ͨ���ϴ��ն˱������
// 				if (!tCurSpyMt.IsLocal() 
// 					&& IsLocalAndSMcuSupMultSpy(tCurSpyMt.GetMcuIdx()) 
// 					&& IsMcu(tCurSpyMt)						// ���¼���һ��mcu
// 					// �������ǽ��mcu���ϴ�ͨ����Ա�ı�
// 					&& (IsMtInMcu(tCurSpyMt, tNewSrc, TRUE)			
// 						|| ( tNewSrc.IsNull() && IsMtInMcu(tCurSpyMt, tOldSrc, TRUE))))
// 				{
// 					TLogicalChannel tLogicChannel;
// 					u8 byLocalMtId = GetLocalMtFromOtherMcuMt(tCurSpyMt).GetMtId();
// 
// 					if ( m_tConfAllMtInfo.MtJoinedConf(byLocalMtId )
// 						&& m_ptMtTable->GetMtLogicChnnl( byLocalMtId, LOGCHL_VIDEO, &tLogicChannel, FALSE )
// 						&& MEDIA_TYPE_H264 == tLogicChannel.GetChannelType()	//Ŀǰ�ĵ����ֱ��ʾ������h264
// 						&& tLogicChannel.GetVideoFormat() != VIDEO_FORMAT_CIF)
// 					{
// 						//�ָ�ԭ���ն˷ֱ���
// 						if (!tOldSrc.IsNull())
// 						{
// 							CascadeAdjMtRes( tOldSrc, tLogicChannel.GetVideoFormat(), FALSE);
// 						}
// 						//���¼��ն˷ֱ���
// 						if (!tNewSrc.IsNull())
// 						{
// 							CascadeAdjMtRes( tNewSrc, VIDEO_FORMAT_CIF, TRUE);
// 						}
// 					}
// 				}
// 
// 			}	
// 		}				
// 	}


// 	TMt tMt;
// 	if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE &&
// 		m_tConfAllMcuInfo.IsSMcuByMcuIdx(tNewSrc.GetMcuId())
// 		)
// 	{
// 		for (byChnId = 0; byChnId < MAXNUM_MPUSVMP_MEMBER; byChnId++)
// 		{
// 			ptVmpMember = m_tConf.m_tStatus.m_tVMPParam.GetVmpMember(byChnId);
// 			if( NULL == ptVmpMember || ptVmpMember->IsNull() )
// 			{
// 				continue;
// 			}
// 			tMt = (TMt)(*ptVmpMember);
// 			if( ptVmpMember->IsLocal() && 
// 				MT_TYPE_SMCU == m_ptMtTable->GetMtType( ptVmpMember->GetMtId() ) &&
// 				IsMtInMcu( tMt,tNewSrc)
// 				)
// 			{
// 				ChangeMtVideoFormat( tNewSrc, &m_tConf.m_tStatus.m_tVMPParam );	
// 				break;
// 			}
// 		}
		
// 		if( !(tNewSrc == tOldSrc) )
// 		{
// 			byChnId = m_tConf.m_tStatus.m_tVMPParam.GetChlOfMtInMember( tOldSrc );
// 			if( MAXNUM_MPUSVMP_MEMBER == byChnId )
// 			{				
// 				ChangeMtVideoFormat( tOldSrc, &m_tConf.m_tStatus.m_tVMPParam, FALSE );
// 			}
// 			else
// 			{
// 				ChangeMtVideoFormat( tOldSrc, &m_tConf.m_tStatus.m_tVMPParam );	
// 			}			
// 		}
// 	}


	TMt tMt;
	TMt tNullMt ;
	tNullMt.SetNull();
	u8 byCount = 0;
	u8 byChnId = 0;
	if( !m_tCascadeMMCU.IsNull() &&	
		IsMMcuSupportMultiCascade()
		)
	{
		// �����������ش������£��յ�3���ı��ϴ�ͨ����Ϣʱ�������������ϴ�ͨ����Ա�Ǹ�3��mcu�����ڸ�3��mcu
		// ����ϼ��ϱ����ϴ�ͨ����Ա
		u16 wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx);
		if(ptConfMcInfo != NULL && !IsLocalAndSMcuSupMultSpy(wMMcuIdx))
		{		
			//zyl 20121102 �¼�mcu��������setounotify������ϼ�mcu��spymt�ͱ������Ĳ���ͬһ��mcu�µ��նˣ��������ϱ�
			TMt tMMcuLocalSpyMt = GetLocalMtFromOtherMcuMt(ptConfMcInfo->m_tSpyMt);
			TMt tNewSrcLocalMt = GetLocalMtFromOtherMcuMt(tSetoutMt);
			//Bug00162355:�����������ش���������ǽͼ��ˢ���쳣
			//yrl20131113����MCUȡ���ϴ�ͨ���ն�ʱҲ�ϱ���һ��MCU(��3��mcu�����2��mcu���ϴ�ͨ���е�mcuһ��)
			if ( !IsLocalAndSMcuSupMultSpy( tSetoutMt.GetMcuId() )
				&& ( tMMcuLocalSpyMt == tNewSrcLocalMt 
				     || ( tNewSrcLocalMt.IsNull() && ptConfMcInfo->m_tSpyMt.GetMcuId() == tSetoutMt.GetMcuId() ) ) 
				)
			{
				TSetOutParam tOutParam;
				tOutParam.m_nMtCount = 1;
				if( 0 == ptSetOutParam->m_tMt.GetMtId() )
				{
					tHeadMsg.m_tMsgSrc.m_abyMtIdentify[tHeadMsg.m_tMsgSrc.m_byCasLevel] = ptSetOutParam->m_tMt.GetMtId();
					++tHeadMsg.m_tMsgSrc.m_byCasLevel;
					tOutParam.m_atConfViewOutInfo[0].m_tMt.SetMtId( (u8)ptSetOutParam->m_tMt.GetMcuId() );
					tOutParam.m_atConfViewOutInfo[0].m_tMt.SetMcuId( LOCAL_MCUID );
				}
				else
				{
					tOutParam.m_atConfViewOutInfo[0].m_tMt = ptSetOutParam->m_tMt;			
				}				

				tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
				tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;			
				//tHeadMsg.m_tMsgSrc.m_tMt = ptSetOutParam->m_tMt;;
				cServMsg.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
				cServMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
				
				cServMsg.SetEventId(MCU_MCU_SETOUT_NOTIF);
				SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_SETOUT_NOTIF, cServMsg );
				
				ptConfMcInfo->m_tSpyMt = tSetoutMt;
			}
		}
	}
	
 	tMt = m_tConf.GetSpeaker();
	if ( m_tConf.m_tStatus.IsSpecMixing() )
	{
		if (IsMcu(tMt))
		{
			TConfMcInfo *ptMcInfo = NULL;
			if (IsMtInMcu(tMt,tOldSrc) && !( tOldSrc == tNewSrc ))
			{
				ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tOldSrc.GetMcuId());
				if( NULL != ptMcInfo )
				{
					//20101224_tzy �����MCU��ԭ���ϴ��ն��Զ����Ļ������򽫸����ն��Ƴ��������������ȡ���ϴ��ն˻�Ҫ�������ն��Զ�ͣ��������־
					if( ptMcInfo->GetMtAutoInMix(tOldSrc) )
					{
						BOOL32 bStopMixNoMem = ( 0 == tNewSrc.GetMtId() ) ? TRUE:FALSE;
						RemoveSpecMixMember( &tOldSrc,1,FALSE,bStopMixNoMem );
					}
				}
				else
				{
					BOOL32 bStopMixNoMem = ( 0 == tNewSrc.GetMtId() ) ? TRUE:FALSE;
					RemoveSpecMixMember( &tOldSrc,1,FALSE,bStopMixNoMem );
				}
			}
			if( 0 != tNewSrc.GetMtId() && IsMtInMcu(tMt,tNewSrc))
			{
				ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tNewSrc.GetMcuId());
				if( NULL != ptMcInfo)
				{
					if (!m_ptConfOtherMcTable->IsMtInMixing(tNewSrc))
					{
						AddSpecMixMember( &tNewSrc,1,TRUE);
					}
				}
				else
				{
					AddSpecMixMember( &tNewSrc,1,TRUE );
				}			
			}
		} 
	}

	TMt tSpeakerMcu = GetLocalMtFromOtherMcuMt(tMt);
	//zjj201009226 �������������mcuΪ���ش����Ҹ�mcu�ϴ���Դ�Ѿ�ȡ����ȡ��������
	//������л��ϴ�Դ��ô������ҲҪ��Ӧ�ı�֪ͨ����
	/*if( !IsLocalAndSMcuSupMultSpy( tMt.GetMcuId() ) &&
		!tMt.IsNull() && 
		tNewSrc.GetMcuId() == tMt.GetMcuId()
		)*/
	// 2011-12-13 mod by pgf��SMCU�������ˣ���ı��ϴ�Դ����Ӧ�øı䵱ǰ�ķ�����
	if( !tMt.IsNull() && !tMt.IsLocal()		//�������¼�mcu�������˵��������Ӧ�ý���������߼�
		&& !IsLocalAndSMcuSupMultSpy( tMt.GetMcuId() ) 
		&&		 
		( tNewSrc.GetMcuId() == tMt.GetMcuId() || IsMtInMcu( tSpeakerMcu,tNewSrc ) )
		)
	{		
		if(0 == tNewSrc.GetMtId())
		{
			ChangeSpeaker( NULL );
		}
		else
		{
			m_tConf.SetSpeaker( tNewSrc );
			cServMsg.SetEventId( MCU_MCS_CONFINFO_NOTIF );
			cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
			if(m_tConf.HasConfExInfo())
			{
				u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
				u16 wPackDataLen = 0;
				PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
				cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
			}
			SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
			NotifyAllSMcuMediaInfo( 0,MODE_BOTH );
		}				
	}

	//zjj20100414 ����ˢ����ʵý��Դ
	if( //MT_TYPE_SMCU == m_ptMtTable->GetMtType( byMtId ) &&
		tNewSrc.IsNull() || IsMtInMcu( tMt, tNewSrc )
		)
	{
		//zhouyiliang 20101018 ȡ��ý��Դ��ʱ����ӦmcinfoҪ���
		if ( tNewSrc.IsNull() ) 
		{
			TConfMcInfo * ptMcuMcInfo = m_ptConfOtherMcTable->GetMcInfo( tNewSrc.GetMcuId() );
			if (NULL != ptMcuMcInfo ) 
			{
				ptMcuMcInfo->m_tSrcMt = tNewSrc;
			}
		}

		NotifyAllSMcuMediaInfo( 0,MODE_BOTH );
	}	

	TMcu tMcu;
	tMcu.SetMcuId(wMcuIdx);
	cServMsg.SetEventId(MCU_MCS_MCUMEDIASRC_NOTIF);
	cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
	cServMsg.CatMsgBody((u8 *)&tNewSrc, sizeof(tNewSrc));
	SendMsgToAllMcs(MCU_MCS_MCUMEDIASRC_NOTIF, cServMsg);
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuSetOutNotify] Notify MMcu SMcu(%d)'s MediaSrc (%d.%d) IsNull(%d)\n",
								tMcu.GetMcuId(),tNewSrc.GetMcuId(),tNewSrc.GetMtId(), tNewSrc.IsNull());
	
	// [pengjie 2010/10/11] ���ڵ��ش�������ϼ��ն�ѡ��ĳ��mcu�µĶ���¼��նˣ�����ֻ��һ·�ش�
	//                      ��Ҫ��֤�ն�״̬��ʵ�ʿ�����ͼ��һ�£����Խ�ѡ��ͬһmcu���ն˵�ѡ��Դˢ���ϴ�Դ
	if(!IsLocalAndSMcuSupMultSpy(tSetoutMt.GetMcuId()))
	{
		tNullMt.SetNull();
		TMt tSelVidMt;
		TMt tSelAudMt;
		TMt tCurrMt;
		TMtStatus tMtStatus;
		
		//[2011/10/14/zhangli]Դ����ͬһmcu�Ҳ��ȣ�����ն�ѡ��Դ����������䣬��ռ��bas��Դ��ʹ���¼��л��ϴ�Դ�����
		TBasOutInfo tOutInfo;
		
		for(u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
		{
			if(m_tConfAllMtInfo.MtJoinedConf(byLoop))
			{
				tCurrMt = m_ptMtTable->GetMt(byLoop);
				m_ptMtTable->GetMtStatus(byLoop, &tMtStatus);
				tSelVidMt = tMtStatus.GetSelectMt(MODE_VIDEO);
				tSelAudMt = tMtStatus.GetSelectMt(MODE_AUDIO);
				
				if(!tSelVidMt.IsNull() && (tSelVidMt.GetMcuId() == tSetoutMt.GetMcuId())
					&& !(tSelVidMt == tSetoutMt))
				{
					if (IsNeedSelAdpt(tSelVidMt, tCurrMt, MODE_VIDEO))
					{
						if (FindBasChn2SelForMt(tSelVidMt, tCurrMt, MODE_VIDEO, tOutInfo))
						{
							SetBasChnSrc(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId, tSetoutMt);
						}
					}
					tMtStatus.SetVideoMt(tSetoutMt);
					tMtStatus.SetSelectMt(tSetoutMt, MODE_VIDEO);
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[ProcMcuMcuSetOutNotify]change select src of Mt(%d,%d)!\n",
						tCurrMt.GetMcuId(), byLoop);
				}
				
				if(!tSelAudMt.IsNull() && (tSelAudMt.GetMcuId() == tSetoutMt.GetMcuId())
					&& !(tSelAudMt == tSetoutMt))
				{
					if (IsNeedSelAdpt(tSetoutMt, tCurrMt, MODE_AUDIO))
					{
						if (FindBasChn2SelForMt(tSelVidMt, tCurrMt, MODE_AUDIO, tOutInfo))
						{
							SetBasChnSrc(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId, tSetoutMt);
						}
					}
					tMtStatus.SetAudioMt(tSetoutMt);
					tMtStatus.SetSelectMt(tSetoutMt, MODE_AUDIO);
				}
				
				m_ptMtTable->SetMtStatus(byLoop, &tMtStatus);
				
				TMt tMtStatusChange = m_ptMtTable->GetMt(byLoop);
				MtStatusChange(&tMtStatusChange, TRUE);
			}
		}
	}
	// End

	//vmp ����ǣ��ǰ���䴦��,��ȷ�������ѡ���������ִ��
	//zjj20091230 ��ͬһ���¼���mcu������������ĳ���¼�mcu���ն�ֻ����һ���ڻ���ϳɳ�Ա��
	//��ռ�������Ѿ�ռ�ݵ���ǰһ��ͨ��
	// [3/28/2011 xliang] �ϼ�ָ���ش�Դ���¼�ָ���ϴ�Դ���ն˷ֱ��ʵ�Ӱ��
	u8 byVmpCount = GetVmpCountInVmpList();
	if (byVmpCount > 0)
	{
		TEqp tVmpEqp;
		TVMPParam_25Mem tVmpParam;
		TVmpChnnlInfo tVmpChnnlInfo;
		TVMPMember *ptVmpMember = NULL;
		BOOL32 bChanged = FALSE;
		BOOL32 bVmpChged = FALSE; //��vmp�Ƿ��иı�
		BOOL32 bNoneKeda = FALSE;
		BOOL32 bHDStyleOK;
		TVMPParam_25Mem tLastVmpParam;
		TVMPMember *ptLastVmpMember = NULL;
		u8 byTmpVmpId = 0;
		for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
		{
			if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
			{
				continue;
			}
			tVmpEqp = g_cMcuVcApp.GetEqp( m_abyVmpEqpId[byIdx] );
					
			tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
			ptVmpMember = NULL;
			//vmp����,�������¼������ϴ�ͨ���ն˴���,�ϼ����µ��ϴ�ͨ���ն˸ı������ٵ���vmp
			if (  byVmpCount > 0
				&& !tVmpParam.IsVMPAuto() 
				&& tNewSrc.GetType() != TYPE_MCUPERI && !tNewSrc.IsLocal() 
				)
			{
				bVmpChged = FALSE;
				bNoneKeda = FALSE;
				// �Ƿ�÷��֧�������ն˲����ֱ���ȫ��
				bHDStyleOK = tVmpChnnlInfo.GetMaxStyleNum() >= tVmpParam.GetMaxMemberNum();
				if (!tNewSrc.IsNull())
				{
					bNoneKeda = !IsKedaMt(tNewSrc, FALSE);
				}
				for (byChnId = 0; byChnId < MAXNUM_VMP_MEMBER; byChnId++)
				{
					tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
					ptVmpMember = tVmpParam.GetVmpMember(byChnId);

					// [miaoqingsong 20111205] �����ж���������ϯ��ѯ/�ϴ���ѯ�����ն˲�����VMP��Ƶ��ѯ����
					// ����ΪʲôҪ��������ж�����?�����ᵼ����ϯ��ѯʱ,�¼����ϴ�ͨ���ն�,vmp��Ա���޷�����
					if( NULL == ptVmpMember || ptVmpMember->IsNull()/* || 
						(m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_SPY || m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_SPEAKER_SPY) ||
						(m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_CHAIRMAN || m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_BOTH_CHAIRMAN)*/
						)
					{
						continue;
					}

					// ����˫������ͨ��
					if (VMP_MEMBERTYPE_DSTREAM == ptVmpMember->GetMemberType())
					{
						continue;
					}

					tMt = (TMt)(*ptVmpMember);

					// [3/28/2011 xliang] A.���ش��滻�ش�Դ���߼����ϼ�ָ������ش�Դ��
					// ���ش�,�¼��л��ش�Դ,ͬ��Ҳ�п��ܽ�A�߼�
					if( !IsLocalAndSMcuSupMultSpy( tNewSrc.GetMcuIdx() )
						/*&& ptVmpMember != NULL && !ptVmpMember->IsNull() */&& !ptVmpMember->IsLocal() &&
						/* GetLocalMtFromOtherMcuMt( *ptVmpMember ) ==  GetLocalMtFromOtherMcuMt( tNewSrc )*/
						!(tNewSrc == tOldSrc)
					  )
					{			
						// ����if����ȥ�����˴�����ж����ϴ�ԴΪ�յ����
						if ( !tOldSrc.IsNull() && tNewSrc.IsNull()
							&& !tOldSrc.IsLocal()
							&& *ptVmpMember == tOldSrc)
						{
							// ���ش���ͨ��Ϊ�¼��նˣ�ȡ���ϴ�ͨ��ʱ������ͨ��ˢ��Ϊ������mcu
							ptVmpMember->SetMt(GetLocalMtFromOtherMcuMt(tOldSrc));
							bChanged = TRUE;
							bVmpChged = TRUE;
							g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tVmpParam);
						}
						else if (GetLocalMtFromOtherMcuMt( *ptVmpMember ) ==  GetLocalMtFromOtherMcuMt( tNewSrc ))
						{
							if( 0 == byCount )
							{
								ptVmpMember->SetMt( tNewSrc );	
								if (!IsAllowVmpMemRepeated(tVmpEqp.GetEqpId()) || (!bHDStyleOK && bNoneKeda)) 
								{
									byCount++;
								}				
								g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tVmpParam);
							}
							else
							{	
								// ���ܼ򵥵��ÿգ�����Ҫ�����������ͨ��ClearOneVmpMember�ӿ�ʵ��
								//ptVmpMember->SetMt( tNullMt );
								ClearOneVmpMember(tVmpEqp.GetEqpId(), byChnId, tVmpParam);
							}
							bChanged = TRUE;
							bVmpChged = TRUE;
						}
					}
					// [3/28/2011 xliang] B.�ϳɳ�ԱΪ�¼�MCU���߼����¼�ָ�������ϴ�Դ��(���ش�/��ش�������)
					else if( /*ptVmpMember->IsLocal() && */
							IsMcu(tMt) &&
							((tNewSrc.IsNull() && IsMtInMcu(tMt,tOldSrc)) || IsMtInMcu( tMt,tNewSrc))
							)
					{
						// ��Ϊ�ǿƴ��ն�,��MCU�ڶ��ͨ����,������һ·MCU
						if (bNoneKeda && byChnId != tVmpParam.GetChlOfMtInMember(tMt, TRUE))
						{
							ClearOneVmpMember(tVmpEqp.GetEqpId(), byChnId, tVmpParam);
						}
						else
						{
							bChanged = TRUE;
							bVmpChged = TRUE;	
							//ChangeMtVideoFormat( tMt, FALSE);//note: parameter shouldn't be tNewSrc here
						}
					}
					// ��Զ�ش�mcu�������ն�ѡ��,�л��ϴ�Դʱ,��Ҫ���ֱ���,�ϴ�ͨ���ն˽�ǰ����
					else if (IsLocalAndSMcuSupMultSpy(tNewSrc.GetMcuIdx())
						&& (*ptVmpMember == tNewSrc || *ptVmpMember == tOldSrc)
						&& !(tNewSrc == tOldSrc)
						)
					{
						TMcu tSMcu = m_ptMtTable->GetMt(byMtId);
						//mcu��ѡ��ʱ,��ӳ��ѡ���ն�,mcu��������ʱ,ͬ����Ҫӳ��
						if (IsSelectedbyOtherMt(tSMcu)
							|| (!tVmpParam.IsVMPBrdst() && tSMcu == m_tConf.GetSpeaker())
							)
						{
							bChanged = TRUE;
							bVmpChged = TRUE;
						}
					}
				}

				if (bVmpChged)
				{
					if (byTmpVmpId == 0)
					{
						byTmpVmpId = m_abyVmpEqpId[byIdx];
					}
					// ����vmp����,�ڸ���m_tLastVmpParamǰ,����������ˢ̨��,��̨֤����ʾ����
					RefreshVmpChlMemalias(tVmpEqp.GetEqpId());
					// 2010-10-21 add by pgf: �����˸�����VMP�޷����˸���+�¼����ش����飬VMP����δ����
					// LastVmpParam���ܼ򵥵�ȫ���ǣ��п���������Ĵ����п������ڵ��ֱ��ʵȵȣ�LastVmpParam��Ա��ʶ�Ƿ��ѽ�����
					tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
					for (byChnId = 0; byChnId < MAXNUM_VMP_MEMBER; byChnId++)
					{
						ptVmpMember = tVmpParam.GetVmpMember(byChnId);
						ptLastVmpMember = tLastVmpParam.GetVmpMember(byChnId);
						if (ptVmpMember == NULL || ptLastVmpMember == NULL || ptLastVmpMember->IsNull())
						{
							continue;
						}
						if (*ptVmpMember == *ptLastVmpMember)
						{
							continue;
						}
						// ���ϴ�Դ��Ϊ��
						if (!tNewSrc.IsNull())
						{
							// ���ش�����Ҫ����
							if (!IsLocalAndSMcuSupMultSpy( tNewSrc.GetMcuIdx() ))
							{
								if (tNewSrc == *ptVmpMember &&
									GetLocalMtFromOtherMcuMt( *ptLastVmpMember ) ==  GetLocalMtFromOtherMcuMt( tNewSrc ))
								{
									// ���³�Ա
									*ptLastVmpMember = *ptVmpMember;
								}
							}
						}
						else
						{
							// ȡ���ϴ�Դ�����ش�����Ҫ����
							if (!IsLocalAndSMcuSupMultSpy( tOldSrc.GetMcuIdx() ))
							{
								if (tOldSrc == *ptLastVmpMember)
								{
									// ���³�Ա
									*ptLastVmpMember = *ptVmpMember;
								}
							}
						}
					}
					g_cMcuVcApp.SetLastVmpParam(tVmpEqp, tLastVmpParam);
					//memcpy(&m_tLastVmpParam, &m_tConf.m_tStatus.m_tVMPParam, sizeof(m_tLastVmpParam));
					LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "get smcu set out notify and vmp param changed, So keep is to m_tLastVmpParam\n");
					
					CServMsg cSendServMsg;
					cSendServMsg.SetEqpId(tVmpEqp.GetEqpId());
					cSendServMsg.SetMsgBody( (u8*)&tVmpParam, sizeof(tVmpParam) );
					SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cSendServMsg ); //���߻��ˢ����
				}
			}
		}

		if ( bChanged ) 
		{
			// change or recover res for OldSrc
			if( !tOldSrc.IsNull() && !(tNewSrc == tOldSrc) )
			{
				byChnId = tVmpParam.GetChlOfMtInMember( tOldSrc );
				if( MAXNUM_VMP_MEMBER == byChnId )
				{
					//A�߼���,���ϴ�Դ���ںϳ�����,�ָ���ֱ���
					//ChangeMtVideoFormat( tOldSrc);
					ChangeMtResFpsInVmp(byTmpVmpId, tOldSrc, &tVmpParam, FALSE);
				}
				else
				{
					//B�߼�(��ش�)�ſ�����
					ChangeMtVideoFormat( tOldSrc, FALSE);	
				}			
			}
			// change res for the NewSrc (A�߼��ߴ˲�) 
			//if ( !bSMcu )
			{
				ChangeMtVideoFormat( tNewSrc, FALSE );
			}
			
			/* 2011-10-18 add by pgf: ������״̬+���鵱ǰVMP��������MCS��ˢ��ͼ��
			TPeriEqpStatus tStatus;
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tStatus);
			tStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
			//[2011/10/27/zhangli]����eqp״̬
// 			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tStatus);
			CServMsg cSetOutMsg;
			cSetOutMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
			SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cSetOutMsg);*/
		}

	}
	//���¼�������������ն���Ϣ
	//���ϴ�ԴΪ�գ���Ϊ���ش���������ͣ���ϴ�Դ����صĽ���
	if (tNewSrc.IsNull())
	{
		// ������⣺���ش����������¼��ն˽���أ�Ȼ�����¼�������ȡ���ش��նˣ��ϼ�δֹͣ��ؽ����������޷��رռ��ͨ��
		if (!IsLocalAndSMcuSupMultSpy(tOldSrc.GetMcuId()))
		{
			AdjustSwitchToAllMcWatchingSrcMt( tOldSrc, TRUE );
			AdjustSwitchToMonitorWatchingSrc( tOldSrc );	//�������
		}
	}
	else
	{
		UpdateMontiorOnSMcuViewMtChg( tNewSrc );
	}

	//Bug00161920:��������֧���ϴ������У�������MCU�����ǽ����ϳ�ͨ�����������ն˽��ϴ�ͨ���󲻽��ֱ���
	//yrl20131114:��������ش���3��mcu�����ϴ��ն˽�1��mcu����ǽ4����(��TVWall��VMP)��Ȼ�����ն˽��ϴ�ͨ��Ҫ�󽵷ֱ���
	if ( IsLocalAndSMcuSupMultSpy(tSetoutMt.GetMcuId()) )
	{
		//��ȡ������ϼ��´��ķֱ���
		CRecvSpy tSpyInfo;
		TMt tMcuMt = GetLocalMtFromOtherMcuMt(tNewSrc);			
		if( m_cLocalSpyMana.GetSpyChannlInfo( tMcuMt, tSpyInfo ) )
		{
			u8 byMinRes = tSpyInfo.GetSimCapset().GetVideoResolution(); 
			
			// ����ն��ڱ��������еķֱ���
			u8 byTmpMtRes = 0;
			GetMtFormat(tNewSrc, byTmpMtRes, FALSE);
			if ( VIDEO_FORMAT_AUTO == byTmpMtRes )
			{
				byTmpMtRes = GetAutoResByBitrate(VIDEO_FORMAT_AUTO, m_tConf.GetBitRate());
			}
			
			if(	byMinRes != 0 && IsSrcResThanDst(byTmpMtRes, byMinRes) )
			{
				// ���͵��ֱ�����Ϣ
				ChangeVFormat(tNewSrc, TRUE, byMinRes);
			}
		}
	}

	MtStatusChange(&tOldSrc,TRUE);
	MtStatusChange(&tNewSrc,TRUE);
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
	u8 byMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TMcu) - sizeof(u8)) / sizeof(TMt);
	if (byMtNum != 0)
	{
		CServMsg cTransMsg = cServMsg;
		cTransMsg.SetMsgBody((u8*)cServMsg.GetMsgBody(),sizeof(TMcu));
		u8 byDissMtNum = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMcu));
		cTransMsg.CatMsgBody((u8*)&byDissMtNum, sizeof(byDissMtNum));
		TMt tMt;
		for( u8 bylp = 0; bylp < byMtNum; bylp ++ )
		{
			TMt *ptMt = (TMt *)(cServMsg.GetMsgBody() + sizeof(TMcu) + sizeof(u8) + sizeof(TMt) * bylp);
			if (ptMt == NULL)
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_EQP, "[ProcMcuMcuStartMixerCmd]ptMt == NULL,So Return!\n");
				return;
			}
			tMt = *ptMt;
			tMt.SetMcuId(m_ptMtTable->GetMt(tMt.GetMtId()).GetMcuId());
			cTransMsg.CatMsgBody((u8*)&tMt, sizeof(TMt));
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMcuMcuStartMixerCmd]tmt after trans is (%d,%d)\n", tMt.GetMcuId(), tMt.GetMtId());
		}
		u8 byReplace = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMcu) + sizeof(u8) + byMtNum * sizeof(TMt));
		cTransMsg.CatMsgBody((u8*)&byReplace,sizeof(u8));
		cServMsg = cTransMsg;
	}
	
	ProcMixStart(cServMsg);
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

	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_MIXER,"[ProcMcuMcuStartMixerNotif]Smcu(%d) startmixing notify!\n",tMcu.GetMcuId());
	//������ʼ�ɹ�������һ�λ�������
	if (!tMcu.IsLocal())
	{
		OnGetMixParamToSMcu(&cServMsg);
	}
	u8 bySmcuId = (u8)(tMcu.GetMcuId());
	if (VCS_CONF == m_tConf.GetConfSource() && !m_ptMtTable->IsMtInMixing(bySmcuId))
	{
		TMt tMt = m_ptMtTable->GetMt(bySmcuId);
		AddSpecMixMember(&tMt,1);
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

	//����ϼ�Ҫ�¼�ֹͣ�����������¼���û���������������Ҫ���ϼ�����ͣ����notify��
	//�Ա��ϼ��޳���mcu����
// 	if (m_tConf.m_tStatus.IsNoMixing() && !m_tCascadeMMCU.IsNull())
// 	{
// 		TMt tMcu;
//         tMcu.SetMcu(LOCAL_MCUID);
//         cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
//         cServMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
//         SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_STOPMIXER_NOTIF, cServMsg);
// 	}
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
	u8 byMtId = u8(ptMcu->GetMcuId());	
	u16 wMcuIdx = GetMcuIdxFromMcuId( (u8)ptMcu->GetMcuId() );
	ptMcu->SetMcuId( wMcuIdx );
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());

	if(ptMcInfo == NULL)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_EQP, "[ProcMcuMcuStopMixerNotif]ptMcInfo == NULL,So Return!\n");
		return;
	}
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_MIXER,"[ProcMcuMcuStopMixerNotif]Smcu(%d) stopmixing notify!\n",byMtId);
	TMt tMt = m_ptMtTable->GetMt(byMtId);
	if ( (m_tConf.GetSpeaker() == tMt)  || (!IsLocalAndSMcuSupMultSpy(GetMcuIdxFromMcuId(byMtId)) && (GetLocalSpeaker() == tMt))
		)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_EQP, "[ProcMcuMcuStopMixerNotif]Speaker(%d,%d),So Don't Remove!!\n",
			m_tConf.GetSpeaker().GetMcuIdx(),m_tConf.GetSpeaker().GetMtId());
	}
	else
	{
		if( m_ptMtTable->IsMtInMixing(byMtId) )
		{
			RemoveSpecMixMember(&tMt,1);
		}
	}
	
    //20101209_tzy ��R3_FULL�����ԣ�R3_RULL���¼�ͣ���������󲢲����ϼ����ͼ����ն�״̬��Ϣ�����½���ˢ�´���
	u8 byNeedClearMtMixing =  0;
	byNeedClearMtMixing = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMcu));
	if ( byNeedClearMtMixing == 0)
	{
		return;
	}

	for (u8 byMtid = 0; byMtid < MAXNUM_CONF_MT; byMtid++)
	{
		if (ptMcInfo->m_atMtStatus[byMtid].IsInMixing())
		{
			ptMcInfo->m_atMtStatus[byMtid].SetInMixing( FALSE );
			MtStatusChange((TMt*)&(ptMcInfo->m_atMtStatus[byMtid]));
		}
	}     

	return;
}

/*==============================================================================
������    : CascadeAdjMtRes
����      : �����ʼ�������ֱ�������
�㷨ʵ��  :  
����˵��  : TMt		tMt					[in]Ҫ����MT (����[4,1])
			u16 wBitRate				[in]����
			bIsRecover					[in]�Ƿ�ָ�
����ֵ˵��: void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-4-23	4.6				Ѧ��							����
==============================================================================*/
void CMcuVcInst::CascadeAdjMtBitrate( TMt tMt, u16 wBitRate,BOOL32 bIsRecover /* = FALSE*/ )
{
	if( tMt.IsLocal() )
	{
		return;
	}
	TMsgHeadMsg tHeadMsg;
	TMt tMcuMt;
	CServMsg cServMsg;

	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt, tMcuMt );
	cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cServMsg.CatMsgBody( (u8 *)&tMcuMt, sizeof(TMt) );		//Ҫ������Mt
	cServMsg.CatMsgBody( (u8 *)&bIsRecover, sizeof(bIsRecover) );		//Ҫ������Mt
	wBitRate =  htons(wBitRate);
	cServMsg.CatMsgBody( (u8 *)&wBitRate, sizeof(wBitRate) );		//Ҫ������Mt

	SendMsgToMt( (u8)tMcuMt.GetMcuId(), MCU_MCU_ADJMTBITRATE_CMD, cServMsg);

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
void CMcuVcInst::CascadeAdjMtRes( TMt tMt, u8 byNewFormat, BOOL32 bStart, u8 byVmpStyle, u8 byPos, u32 dwResW, u32 dwResH,u8 byEqpId, u8 byHduChlId)
{
	// [pengjie 2010/10/29] �����ն�idΪ0ֱ�ӷ��أ�����ȥ��mcu
	if( tMt.GetMtId() == 0 )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[CascadeAdjMtRes] Dest mt.0 so return !\n" );
		return;
	}


	// End
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[CascadeAdjMtRes] Param--tMt:(%d,%d)\tbyFormat: %d\tbyPos: %d\tdwResWH[%d,%d]\tVmpId:%d\n", 
		tMt.GetMcuId(), tMt.GetMtId(), byNewFormat, byPos, dwResW, dwResH, byEqpId);
	// [10/19/2011 liuxu] ����ֱ�ӵ�����mcu�ն˵ķֱ���,��תΪ�����¼��ϴ��ն˵ķֱ���
	// ԭ��: 1. ��mcu�ն˺������ϴ��ն�ͬʱ���ϼ�����ǽ��ʱ, ��mcu�˳�����ǽʱ��
	// ��ͼ�ָ����ϴ�ͨ�����ն˵ķֱ���
	// תΪ����������ʵ�ϴ��ն˵ķֱ���
	// [10/29/2011 liuxu] ���ʱ����mcu�ж�, Ϊ������R6-R2-R2����ʱ�����
	// ԭ��: ������R2�����ϱ��ش���Ա����һ����R6,����R6ȡ�����������ش���Ա
	// R6-R6-R6����ʱ, ��һ�����������ն�תΪ�ڶ���ȥ�����������ϴ���Ա
	if (( IsMcu(tMt) && tMt.IsLocal() ))
	{
		TMt tTempMt = GetSMcuViewMt(tMt, FALSE);
		if (!tTempMt.IsNull())
		{
			tMt = tTempMt;
		}
	}

	// �绰�ն�����Ƶ�߼�ͨ��δ�������ܵ����ֱ��ʣ������͵��ֱ�����Ϣ
	if (IsPhoneMt(tMt))
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[CascadeAdjMtRes] Dest mt.(%d,%d) is phone, so return !\n", tMt.GetMcuId(), tMt.GetMtId());
		return;
	}

	// ���byNewFormat��Ӧ�ķֱ��ʿ�͸�
	u16 wResW = (u16)dwResW;
	u16 wResH = (u16)dwResH;
	// ���д�����,������ͨ��byNewFormat���,�ϼ����ܻᴫ����,��Ҫ������������mcu��
	if (0 == dwResW || 0 == dwResH)
	{
		// mpeg4 auto��Ҫ���ݻ������ʻ�÷ֱ���
		if ( VIDEO_FORMAT_AUTO == byNewFormat )
		{
			byNewFormat = GetAutoResByBitrate(byNewFormat,m_tConf.GetBitRate());
		}
		GetWHByRes(byNewFormat, wResW, wResH);
		dwResW = wResW;
		dwResH = wResH;
	}
	// תΪ������
	dwResW = htonl(dwResW);
	dwResH = htonl(dwResH);

	// ���ݷֱ���,��ö�Ӧͨ�÷ֱ���,�¾�MCU����ʶ��ķֱ���
	if (bStart)
	{
		byNewFormat = GetNormalRes(wResW, wResH);
	}

	CServMsg cServMsg;
	cServMsg.SetEventId( MCU_MCU_ADJMTRES_REQ );
	u8 byStart = bStart ? 1: 0;
	TMsgHeadMsg tHeadMsg;
	TMt tMcuMt;

	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt, tMcuMt );
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[CascadeAdjMtRes] SendMsg--tMt:(%d,%d)\tbyFormat: %d\tbyStart: %d\tbyPos: %d\tdwResWH[%d,%d]\n", 
		tMcuMt.GetMcuId(), tMcuMt.GetMtId(), byNewFormat, byStart, byPos, wResW, wResH);

	cServMsg.SetEqpId(byEqpId);
	cServMsg.SetChnIndex(byHduChlId);//����hdu��ͨ��
	cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cServMsg.CatMsgBody( (u8 *)&tMcuMt, sizeof(TMt) );		//Ҫ������Mt
	cServMsg.CatMsgBody( &byStart, sizeof(u8) );		//�Ƿ��ǻָ�Res
	cServMsg.CatMsgBody( &byNewFormat, sizeof(u8) );	//Ҫ�������·ֱ���

	// ������2��¼�������Լ�����ȡҪ������RES
	//if( byVmpStyle <= VMP_STYLE_TWENTY && byVmpStyle > 0
	//	&& byPos < MAXNUM_MPUSVMP_MEMBER )
	{
		cServMsg.CatMsgBody( &byVmpStyle, sizeof(u8) );		//VMP �ϳɷ��
		cServMsg.CatMsgBody( &byPos, sizeof(u8) );			//����VMPͨ��λ��
	}

	// v4r7���Ժ�汾mcuר��
	cServMsg.CatMsgBody( (u8 *)&dwResW, sizeof(u32) );	//����Ҫ�����ķֱ��ʿ�
	cServMsg.CatMsgBody( (u8 *)&dwResH, sizeof(u32) );	//����Ҫ�����ķֱ��ʸ�
	
	SendMsgToMt( (u8)tMcuMt.GetMcuId(), cServMsg.GetEventId(), cServMsg);
}

/*==============================================================================
������    : RecoverMtVFormat
����      : �ָ��ն˵ķֱ���
�㷨ʵ��  :  
����˵��  : TMt		tDstMt				[in]Ҫ�ָ��ֱ��ʵĵ�MT (����[4,1])
			u8      bRecoverSMcuViewMt  [in]tDstMt��mcuʱ, �Ƿ�ָ����ϴ�ͨ�����ն˵ķֱ���
����ֵ˵��: 
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2011-10-19	4.6				liuxu							����
==============================================================================*/
void CMcuVcInst::RecoverMtVFormat( const TMt& tDstMt, const BOOL32 bRecoverSMcuViewMt /*= FALSE */)
{
	if ( NeedChangeVFormat( tDstMt ) )
	{
		// tDstMt����Ͳ��ᱻ���ֱ���, ���Բ���Ҫ���ָ��ֱ���, ֱ�ӷ���true
		return;
	}

	TMt tRealMt = tDstMt;
	if ( IsMcu(tDstMt) )
	{
		if ( !bRecoverSMcuViewMt )							// ���ָ����ϴ��ն˵ķֱ���
		{
			return;
		}

		// ��ȡ�ϴ��ն�ʧ��
		tRealMt = GetSMcuViewMt(tDstMt, TRUE);
	}

	if (tRealMt.IsNull())
	{
		return;
	}

	// ��ȡtDstMt�ڱ��ص�MtId
	u8 byLocalMtId = 0;
	if (!tDstMt.IsLocal())
	{
		byLocalMtId = GetLocalMtFromOtherMcuMt(tDstMt).GetMtId();
	}else
	{
		byLocalMtId = tDstMt.GetMtId();
	}
	
	// ��ȡ������Ƶͨ��
	TLogicalChannel tLogicChannel;
	if ( !m_ptMtTable 
		|| !m_ptMtTable->GetMtLogicChnnl( byLocalMtId, LOGCHL_VIDEO, &tLogicChannel, FALSE ))
	{
		return;
	}

	// �ָ���ԭʼ�ֱ���
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "TMt[%d,%d] res recovered\n", tRealMt.GetMcuId(), tRealMt.GetMtId() );
	ChangeVFormat( tRealMt, FALSE, tLogicChannel.GetVideoFormat());
}


/*=============================================================================
    �� �� ���� DecreaseMtVFormatInTw
    ��    �ܣ� ���͵���ǽ���ն˵ķֱ���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [in] tDstMt,    Ҫ�������ն�
    �� �� ֵ�� 
-----------------------------------------------------------------------------
    �޸ļ�¼��
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2011-10-19	4.6				liuxu							����
=============================================================================*/
void CMcuVcInst::DecreaseMtVFormatInTw( const TMt& tDstMt )
{
	// ����tvwall�л�����ͨ����Ա���ϴ��ն�ʱ, ���ָܻ��ֱ���
	if ( !IsSMcuViewMtAndInTw(tDstMt, FALSE)					// ����tvwallͨ����Ա��ֱ���ϴ��ն�
		&& IsMtNotInOtherTvWallChnnl(tDstMt, 0, 255))			// ����tvwall�豸��
	{
		return;
	}

	if ( !NeedChangeVFormat( tDstMt ) )
	{
		// tDstMt����Ͳ��ᱻ���ֱ���, ���Բ���Ҫ���ָ��ֱ���, ֱ�ӷ���true
		return;
	}

	// ʵ��Ҫ�������ն�. tDstMt��Mcu����ʱ, ʵ�ʵ����������ϴ�ͨ�����ն˵ķֱ���
	TMt tRealMt = (IsMcu(tDstMt) && tDstMt.IsLocal() ) ? GetSMcuViewMt(tDstMt, FALSE) : tDstMt;
	if (tRealMt.IsNull())
	{
		return;
	}

	// ��ȡtDstMt�ڱ��ص�MtId
	u8 byLocalMtId = 0;
	if (!tDstMt.IsLocal())
	{
		byLocalMtId = GetLocalMtFromOtherMcuMt(tDstMt).GetMtId();
	}else
	{
		byLocalMtId = tDstMt.GetMtId();
	}
	
	// ��ȡ������Ƶͨ��
	TLogicalChannel tLogicChannel;
	if ( !m_tConfAllMtInfo.MtJoinedConf(byLocalMtId )
		|| !m_ptMtTable 
		|| !m_ptMtTable->GetMtLogicChnnl( byLocalMtId, LOGCHL_VIDEO, &tLogicChannel, FALSE ))
	{
		return;
	}
	
	// �ѷֱ��ʽ�ΪCif
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "TMt[%d,%d] res decreased\n", tRealMt.GetMcuId(), tRealMt.GetMtId() );
	ChangeVFormat( tRealMt, TRUE, VIDEO_FORMAT_CIF );
}


/*=============================================================================
    �� �� ���� SendChgMtVidFormat
    ��    �ܣ� ��ͨ�ն˵��ֱ���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byMtId		Ҫ�������ն�
			   u8 byChnnlType	�ŵ����ͣ�����Ƶ/˫��(LOGCHL_VIDEO...)
			   u8 byFormat		�����ķֱ���
			   BOOL32 bMmcuCmd	���ϼ�Ҫ������Ǳ����Լ�Ҫ�����Ĭ��False������Ҫ���
			   BOOL32 bStart	�Ƿ�����ֱ���
			   u32 wResW		�����ķֱ��ʿ�
			   u32 wResH		�����ķֱ��ʸ�(�п�߰����,�޿�߰�byFormat)
    �� �� ֵ�� BOOL32 �����Ƿ�ɹ������ڱ�����˵��
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
	20100913    4.6			pengjie				   create
=============================================================================*/
BOOL32 CMcuVcInst::SendChgMtVidFormat( u8 byMtId, u8 byChlType, u8 byFormat, BOOL32 bMmcuCmd, BOOL32 bStart, u32 dwResW, u32 dwResH)
{
	CServMsg cServMsg;
	CSendSpy cSendSpy;
	TMt tSrc = m_ptMtTable->GetMt( byMtId );

	// [8/31/2011 liuxu] �Ա���mcu���¼��ն˷ֱ��ʵĵ���
	if ( !tSrc.IsLocal() || IsMcu(tSrc) )
	{
		CascadeAdjMtRes( tSrc, byFormat, bStart, 0XFF, 0XFF, (u16)dwResW, (u16)dwResH);
		return TRUE;
	}

	cServMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
	cServMsg.SetMsgBody( &byChlType, sizeof(u8) );
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[SendChgMtVidFormat] Param--byMtId:%d\t byFormat is: %d\t dwResWH[%d,%d]\n", 
		byMtId, byFormat, dwResW, dwResH);
	
	//���ڷֱ���ΪVIDEO_FORMAT_AUTO���ݻ������ʼ���ֱ���
	if ( VIDEO_FORMAT_AUTO == byFormat )
	{
		byFormat = GetAutoResByBitrate(byFormat, m_tConf.GetBitRate());
	}

	// ��¼ԭʼ����ֱ���
	u8 byReqFormat = byFormat;
	// ��ʱ����
	u16 wTmpResW = 0;
	u16 wTmpResH = 0;
	BOOL32 bResChg = FALSE;
	BOOL32 bIsMMcuNotSupMultSpy = FALSE;//MMCU�Ƿ��ǵ��ش�
	// �ϼ�Ҫ���
	if( bMmcuCmd == TRUE )
	{
		u16 wMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		bIsMMcuNotSupMultSpy = !IsLocalAndSMcuSupMultSpy( wMcuIdx );
		// ����ն��ڱ��������еķֱ���
		u8 byTmpMtRes = 0;
		GetMtFormat(tSrc, byTmpMtRes, FALSE);
		if ( VIDEO_FORMAT_AUTO == byTmpMtRes )
		{
			byTmpMtRes = GetAutoResByBitrate(VIDEO_FORMAT_AUTO, m_tConf.GetBitRate());
		}
		// �д�����,�ϼ���������Ϣ���ֱ���ʱ,�ô���Ŀ�����Ƚ�
		if (0 != dwResW && 0 != dwResH)
		{
			GetWHByRes(byTmpMtRes, wTmpResW, wTmpResH);
			byFormat = GetSuitableResByWH((u16)dwResW, (u16)dwResH, wTmpResW, wTmpResH);
		}
		else //δ������,��byFormat���Ƚ�
		{
			// 2011-9-7 add by pgf:�Ƚϲ���ȷ��Ӧ�ò�ȡ�ȿ�͸�
			byFormat = GetSuitableRes(byTmpMtRes, byFormat);
		}
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[SendChgMtVidFormat] Line:%d byMtInVmpRes<%d> CMP result:<%d>\n",
			__LINE__, byTmpMtRes, byFormat);
		if ( byFormat != byTmpMtRes)
		{
			bResChg = TRUE;
		}
		else
		{
			u8 byTempRes = VIDEO_FORMAT_INVALID;
			TLogicalChannel tLogicChannel;
			if ( !m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_VIDEO, &tLogicChannel, FALSE ) )
			{
				return FALSE;
			}

			//���ڷֱ���ΪVIDEO_FORMAT_AUTO���ݻ������ʼ���ֱ���
			if ( VIDEO_FORMAT_AUTO == tLogicChannel.GetVideoFormat() )
			{
				byTempRes = GetAutoResByBitrate(VIDEO_FORMAT_AUTO, m_tConf.GetBitRate());
			}
			else
			{
				byTempRes = tLogicChannel.GetVideoFormat();
			}

			// �ָ����߼�ͨ������
			if (byFormat == byTempRes)
			{
				bResChg = TRUE;
			}
			
			// ���뵱ǰ�ش��ֱ����жϣ���Իش�С�ֱ��ʣ��ϼ��ָ�ԭʼ�ֱ���ʱ�ն����ڱ���vmpͨ���У�δ������ͨ������������
			u8 byCurSpyRes = VIDEO_FORMAT_INVALID;//��¼��ǰ�ش��ֱ���
			if (m_cLocalSpyMana.GetSpyChannlInfo( tSrc, cSendSpy ))
			{
				byCurSpyRes = cSendSpy.GetSimCapset().GetVideoResolution();
				if ( VIDEO_FORMAT_AUTO == byCurSpyRes )
				{
					byCurSpyRes = GetAutoResByBitrate(VIDEO_FORMAT_AUTO, m_tConf.GetBitRate());
				}
				byTempRes = GetSuitableRes(byCurSpyRes, byFormat);
				// ����ش�ͨ���ķֱ���С��Ҫ�����ķֱ��ʱȱ�����Ҫ�Ĵ󣬻�����Ҫ����һ�ηֱ��ʵ�
				if (byTempRes == byCurSpyRes)
				{
					bResChg = TRUE;
				}
			}
			else
			{
				// ���ش�ʱ�����ϼ�Ҫ��������ն˲����ϴ�Դ�նˣ����ڻش��б��У��滻�ϴ�Դʱ�ָ����ϴ�Դ�ֱ��ʻ���ִ����
				// �����һ�ηֱ���
				if (bIsMMcuNotSupMultSpy)
				{
					bResChg = TRUE;
				}
			}
		}

		/*TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
		if( (tVmpParam.IsVMPBrdst() && tVmpParam.IsMtInMember(tSrc)) )
		{
			CVmpChgFormatResult cVmpChgResult;
			u8 byMtInVmpRes = 0;
			u8 byVmpSubType = GetVmpSubType(m_tVmpEqp.GetEqpId());
			
			if(byVmpSubType == VMP)
			{
				GetMtFormatInSdVmp(tSrc, &tVmpParam, byMtInVmpRes, TRUE);
			}
			else
			{
				GetMtFormatInMpu(tSrc, &tVmpParam, byMtInVmpRes, cVmpChgResult, TRUE, TRUE, FALSE);
			}
			
			// �д�����,�ϼ���������Ϣ���ֱ���ʱ,�ô���Ŀ�����Ƚ�
			if (0 != dwResW && 0 != dwResH)
			{
				GetWHByRes(byMtInVmpRes, wTmpResW, wTmpResH);
				byFormat = GetSuitableResByWH((u16)dwResW, (u16)dwResH, wTmpResW, wTmpResH);
			}
			else //δ������,��byFormat���Ƚ�
			{
				// 2011-9-7 add by pgf:�Ƚϲ���ȷ��Ӧ�ò�ȡ�ȿ�͸�
				byFormat = GetSuitableRes(byMtInVmpRes, byFormat);
			}
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[SendChgMtVidFormat] Line:%d byMtInVmpRes<%d> CMP result:<%d>\n",
			__LINE__, byMtInVmpRes, byFormat);
			// if( byFormat < byMtInVmpRes )
			if ( byFormat != byMtInVmpRes )
			{
				bResChg = TRUE;
			}
		}
		else if( m_ptMtTable->IsMtInTvWall(byMtId) )
		{
			// �д�����,�ϼ���������Ϣ���ֱ���ʱ,�ô���Ŀ�����Ƚ�
			if (0 != dwResW && 0 != dwResH)
			{
				GetWHByRes(VIDEO_FORMAT_CIF, wTmpResW, wTmpResH);
				byFormat = GetSuitableResByWH((u16)dwResW, (u16)dwResH, wTmpResW, wTmpResH);
			}
			else //δ������,��byFormat���Ƚ�
			{
				// 2011-9-7 add by pgf:�Ƚϲ���ȷ��Ӧ�ò�ȡ�ȿ�͸�
				byFormat = GetSuitableRes(VIDEO_FORMAT_CIF, byFormat);
			}
			if( byFormat != VIDEO_FORMAT_CIF )
			{
				bResChg = TRUE;
			}
		}// [2013/03/11 chenbing] �ն��Ƿ���HDU�໭���� 
		else if( IsMtInHduVmp(tSrc) )
		{
			// �д�����,�ϼ���������Ϣ���ֱ���ʱ,�ô���Ŀ�����Ƚ�
			if (0 != dwResW && 0 != dwResH)
			{
				GetWHByRes(VIDEO_FORMAT_HD720, wTmpResW, wTmpResH);
				byFormat = GetSuitableResByWH((u16)dwResW, (u16)dwResH, wTmpResW, wTmpResH);
			}
			else //δ������,��byFormat���Ƚ�
			{
				byFormat = GetSuitableRes(VIDEO_FORMAT_HD720, byFormat);
			}
			if( byFormat != VIDEO_FORMAT_HD720 )
			{
				bResChg = TRUE;
			}
		}
		else
		{
			bResChg = TRUE;
		}*/
	}
	// ����Ҫ���
	else
	{
		// ���ǻش���Աֱ�ӵ��ֱ���
		if( !m_cLocalSpyMana.GetSpyChannlInfo( tSrc, cSendSpy ) )
		{
			bResChg = TRUE;
		}
		else
		{
			u8 byCurRes = cSendSpy.GetSimCapset().GetVideoResolution();
			if( byCurRes != VIDEO_FORMAT_INVALID && byCurRes != 0 )
			{
				// �µķֱ��ʱ��ϵ�ҪС�ŵ���
				// 2011-9-7 add by pgf:�Ƚϲ���ȷ��Ӧ�ò�ȡ�ȿ�͸�
				byFormat = GetSuitableRes(byCurRes, byFormat);
				// 2011-9-26 fix by pgf: ����MT��Ϊ�ϼ�VMP��Ա�����¼��ֱ��ʾ�Ϊ1080p������
				// MT��16����VMP���߳�ʱ���ָ��ķֱ��ʺͻش���Ϣ�ķֱ���һ�£�����������뷢��Ϣ(�����һ�����ܵ���ʧ)
// 				if( byFormat != byCurRes )
				{
					bResChg = TRUE;
				}
			}
			// ֮ǰ�ֱ���Ϊ�գ���ֱ�ӵ��ֱ���
			else 
			{
				bResChg = TRUE;
			}
		}
	}
	
	// ���ش����飬�ϴ�ͨ���ն˷ֱ��ʸı䣬���ش���Ա�ֱ��ʸ��µ��ش��б���
	if (bIsMMcuNotSupMultSpy)
	{
		// ��õ�ǰ�ϴ�Դ����һ�£������б�
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
		if (ptConfMcInfo != NULL)
		{
			if (tSrc.GetMcuId() == ptConfMcInfo->m_tSpyMt.GetMcuId() 
				&& tSrc.GetMtId() == ptConfMcInfo->m_tSpyMt.GetMtId())
			{
				if (bMmcuCmd)
				{
					TSimCapSet tSimCapSet = GetMtSimCapSetByMode(byMtId);
					cSendSpy.Clear();
					cSendSpy.SetSpyMt(tSrc);
					cSendSpy.SetSimCapset(tSimCapSet);
					cSendSpy.SetSpyMode(MODE_VIDEO);//ֻ���ڵ��ֱ���
					// ��ӵ��ش��ش���Ϣ�������·ֱ��ʸ���
					if( m_cLocalSpyMana.AddSpyChannlInfo( cSendSpy ) )
					{
						tSimCapSet.SetVideoResolution(byReqFormat);
						m_cLocalSpyMana.SaveSpySimCap(tSrc, tSimCapSet);
					}
				}
			}
			else
			{
				// ��һ�£���ʾ���ն˲��ǵ�ǰ�ϴ��նˣ�����m_cLocalSpyMana�У������
				if (m_cLocalSpyMana.GetSpyChannlInfo(tSrc, cSendSpy))
				{
					m_cLocalSpyMana.FreeSpyChannlInfo(tSrc, MODE_VIDEO);
				}
			}
		}
	}

	// ��Ҫ����
	if (bResChg)
	{
		wTmpResW = (u16)dwResW;
		wTmpResH = (u16)dwResH;
		if (0 == dwResW || 0 == dwResH)
		{
// 			// mpeg4 auto��Ҫ���ݻ������ʻ�÷ֱ���
// 			if ( VIDEO_FORMAT_AUTO == byFormat )
// 			{
// 				byFormat = GetAutoResByBitrate(byFormat,m_tConf.GetBitRate());
// 			}
			GetWHByRes(byFormat, wTmpResW, wTmpResH);
			dwResW = wTmpResW;
			dwResH = wTmpResH;
		}
		// ���ݷֱ���,��ö�Ӧͨ�÷ֱ���,�¾�MCU����ʶ��ķֱ���
		if (bStart)
		{
			byFormat = GetNormalRes(wTmpResW, wTmpResH);
		}
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[SendChgMtVidFormat] SendMsg--byMtId:%d\t byFormat is: %d\t dwResWH[%d,%d]\n", 
			byMtId, byFormat, dwResW, dwResH);
		//�����תΪ������
		dwResW = htonl(dwResW);
		dwResH = htonl(dwResH);
		cServMsg.CatMsgBody( &byFormat, sizeof(u8) );
		cServMsg.CatMsgBody( (u8 *)&dwResW, sizeof(u32) );	//����Ҫ�����ķֱ��ʿ�
		cServMsg.CatMsgBody( (u8 *)&dwResH, sizeof(u32) );	//����Ҫ�����ķֱ��ʸ�
		SendMsgToMt( byMtId, cServMsg.GetEventId(), cServMsg );
		return TRUE;
	}

	return FALSE;
}

/*====================================================================
    ������      ��SendChgMtFps
    ����        ����mt���͵�֡�ʵ���Ϣ�����Ǽ�����֡�ʣ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt &tMt					[i] 
				  u8 byChnnlType					[i]
				  u8 byFps							[i] 
				  BOOL32 bStart						[i] �ǵ������ǻָ�
				  
    ����ֵ˵��  ��VOID 
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/01/17	4.6         Ѧ��		   ����
====================================================================*/
void CMcuVcInst::SendChgMtFps( const TMt &tMt, u8 byChnnlType, u8 byFps, BOOL32 bStart)
{
	if( !tMt.IsLocal() ||
		m_ptMtTable->GetMtType( tMt.GetMtId() ) == MT_TYPE_SMCU
		)
	{
		if( tMt.IsLocal() && m_ptMtTable->GetMtType( tMt.GetMtId() ) == MT_TYPE_SMCU )
		{
			TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo( GetMcuIdxFromMcuId(tMt.GetMtId()) );
			if(ptInfo != NULL)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[SendChgMtFps] tMt.IsLocal() CascadeAdjMtFps tMt(%d, %d) bIsStart <%d> Adjusted NewFps: <%d>!!!\n",
					tMt.GetMcuId(), tMt.GetMtId(), bStart, byFps);
				CascadeAdjMtFps(ptInfo->m_tMMcuViewMt, bStart, byChnnlType, byFps);
			}			
		}
		else
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[SendChgMtFps] CascadeAdjMtFps tMt(%d, %d) bIsStart <%d> Adjusted NewFps: <%d>!!!\n",
					tMt.GetMcuId(), tMt.GetMtId(), bStart, byFps);
			CascadeAdjMtFps(tMt, bStart, byChnnlType, byFps);
		}
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[SendChgMtFps] tMt(%d, %d) bIsStart <%d> Adjusted NewFps: <%d>!!!\n",
					tMt.GetMcuId(), tMt.GetMtId(), bStart, byFps);
		SendChangeMtFps(tMt.GetMtId(), byChnnlType, byFps);
	}
}
/*=============================================================================
    �� �� ���� SendChangeMtFps
    ��    �ܣ� ��ͨ�ն˵�֡��(����)
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byMtId   Ҫ�������ն�
			   u8 byChnnlType  �ŵ����ͣ�����Ƶ/˫��(LOGCHL_VIDEO...)
			   u8 byFps  ������֡��ֵ
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
	20100423    4.6			pengjie				   create
=============================================================================*/
void CMcuVcInst::SendChangeMtFps( u8 byMtId, u8 byChnnlType, u8 byFps )
{
	CServMsg cServMsg;

	cServMsg.SetEventId( MCU_MT_VIDEOFPSCHANGE_CMD );
	cServMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
	cServMsg.CatMsgBody( &byFps, sizeof(u8) );

	SendMsgToMt( byMtId, cServMsg.GetEventId(), cServMsg );

	return;
}

/*=============================================================================
    �� �� ���� CascadeAdjMtFps
    ��    �ܣ� ������֡��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMt tMt   Ҫ�������¼��ն�
	           u8 bStart ΪTRUE��ʾҪ����FLASE�ָ��ն˱�����֡��
			   u8 byChnnlType  �ŵ����ͣ�����Ƶ/˫��(LOGCHL_VIDEO...)
			   u8 byFps  ������֡��ֵ
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
	20100423    4.6			pengjie				   create
=============================================================================*/
void CMcuVcInst::CascadeAdjMtFps( TMt tMt, BOOL32 bStart, u8 byChnnlType, u8 byFps )
{
	CServMsg cServMsg;
	cServMsg.SetEventId( MCU_MCU_ADJMTFPS_REQ );
	u8 byStart = bStart ? 1: 0;

	// [pengjie 2010/8/23] ����������֡��֧��
	TMsgHeadMsg tHeadMsg;
	TMt tMcuMt;
	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt, tMcuMt );

	cServMsg.SetMsgBody( (u8 *)&tMcuMt, sizeof(TMt) );		// Ҫ������Mt
	cServMsg.CatMsgBody( (u8 *)&byStart, sizeof(u8) );		// �Ƿ��ǻָ�Fps
	cServMsg.CatMsgBody( (u8 *)&byChnnlType, sizeof(u8) );	// �ŵ�����
	cServMsg.CatMsgBody( (u8 *)&byFps, sizeof(u8) );	// �ŵ�����
	cServMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[CascadeAdjMtFps] tMt(%d, %d) bIsStart <%d> Adjusted NewFps: <%d>!!!\n",
					tMt.GetMcuId(), tMt.GetMtId(), bStart, byFps);
	SendMsgToMt( (u8)tMcuMt.GetMcuId(), cServMsg.GetEventId(), cServMsg);
}

/*=============================================================================
    �� �� ���� ProcMcuMcuAdjustMtFpsReq
    ��    �ܣ� ���� ������Mt֡�� ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
	20100423    4.6			pengjie				   create
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAdjustMtFpsReq ( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	STATECHECK;	
	
	TMt tMt        = *(TMt *)( cServMsg.GetMsgBody() );
	u8 byStart     = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMt));		//��ʼ��
	u8 byChnnlType = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) );
	u8 byReqFps    = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(u8) ); 
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody() + sizeof(TMt) + (sizeof(u8) * 3) );

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuAdjustMtFpsReq] Recv tMt(%d, %d) chnnltype(%d) byStart(%d) Fps(%d)!!!\n",
					tMt.GetMcuId(), tMt.GetMtId(), byChnnlType, byStart, byReqFps );

	// [pengjie 2010/8/23] ��������֧�֣��Ǳ����������´�
	if( !tMt.IsMcuIdLocal() )
	{	
		SendMsgToMt( tMt.GetMcuId(), MCU_MCU_ADJMTFPS_REQ, cServMsg );
		return;
	}
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuAdjustMtFpsReq] tMt is MCU!!!\n");

	//[20130516 chenbing]�¼��յ���tMtΪ����Mcuʱ����Ҫ�������´�
	TMt tSrc = m_ptMtTable->GetMt( tMt.GetMtId() );
	if ( IsMcu(tSrc) && tSrc.IsLocal() )
	{
 		SendChgMtFps(tSrc, byChnnlType, byReqFps, byStart);
	 	return;
	}

	u16 wNackReason = CASCADE_ADJFPSNACK_REASON_UNKNOWN;
	u8 byMtId = tMt.GetMtId();
	BOOL32 bRspNack = FALSE;
	TLogicalChannel tLogicChannel;
	if ( !m_tConfAllMtInfo.MtJoinedConf( byMtId ) ||
		!m_ptMtTable->GetMtLogicChnnl( byMtId, byChnnlType, &tLogicChannel, FALSE ) )
	{
		wNackReason = CASCADE_ADJFPSNACK_REASON_DISCON;
		bRspNack = TRUE;
	}

	if( IsNeedChangeFpsMt(byMtId) )
	{
		if( byStart ) // ����
		{
			SendChangeMtFps( byMtId, byChnnlType, byReqFps );
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuAdjustMtFpsReq] Change Fps for mt: %d, chnnl type: %d, Fps: %d !\n",
					byMtId, byChnnlType, byReqFps );
		}
		else  // �ָ�
		{
			byReqFps = tLogicChannel.GetChanVidFPS();
			SendChangeMtFps( byMtId, byChnnlType, byReqFps );
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuAdjustMtFpsReq] Recover Fps for mt: %d, chnnl type: %d, Fps: %d !\n",
					byMtId, byChnnlType, byReqFps );
		}

		cServMsg.SetMsgBody();
		cServMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
		SendReplyBack( cServMsg, MCU_MCU_ADJMTFPS_ACK );
	}
	else
	{
		wNackReason = CASCADE_ADJFPSNACK_REASON_CANT;
	}

	if( bRspNack == TRUE )
	{
		if(bRspNack)
		{
			cServMsg.SetMsgBody();
			cServMsg.SetErrorCode(wNackReason);
			cServMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
			SendReplyBack( cServMsg, MCU_MCU_ADJMTFPS_NACK );
			return;
		}
	}

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuAdjustMtFpsAck
    ��    �ܣ� ���� ������Mt֡�� ���� ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
	20100423    4.6			pengjie				   create
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAdjustMtFpsAck ( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	

	TMt tMt = *(TMt *)cServMsg.GetMsgBody();

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "McuMcuAdjustMtFpsAck come McuId: %d, MtId: %d !\n",
		tMt.GetMcuId(), tMt.GetMtId() );

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuAdjustMtFpsNack
    ��    �ܣ� ���� ������Mt֡�� ���� �ܾ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
	20100423    4.6			pengjie				   create
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAdjustMtFpsNack ( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	

	TMt tMt = *(TMt *)cServMsg.GetMsgBody();

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "McuMcuAdjustMtFpsNack come McuId: %d, MtId: %d !\n",
		tMt.GetMcuId(), tMt.GetMtId());

	return;
}

/*=============================================================================
    �� �� ���� CheckMtResAdjust
    ��    �ܣ� �����ն˷ֱ���ǰ��飬�����Ƿ��ܵ�������֪�ܵ����ķֱ���ֵ
				(ע��ú�����������Ϣ���ն�)
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ����	TMt &tMt		[i]
				u8  byReqRes	[i]		
				u16 &wErrorCode	[o]
				u8	&byRealRes	[o]
				u8  byStart		[i]		default: TRUE
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
	4/11/2009   4.6			Ѧ��					create
=============================================================================*/
BOOL32 CMcuVcInst::CheckMtResAdjust( const TMt &tMt, u8 byReqRes, u16 &wErrorCode, u8 &byRealRes, u8 byStart /* = TRUE */, u32 dwResW, u32 dwResH)
{
	u8 byMtId = tMt.GetMtId();
	wErrorCode = CASCADE_ADJRESNACK_REASON_UNKNOWN;	
	BOOL32 bRspNack	= FALSE;				//�Ƿ��nack
	//	BOOL32 bNeedAdjustRes = TRUE;			//Ĭ����Ҫ���ֱ���
	u8 byChnnlType = LOGCHL_VIDEO;			//Ŀǰֻ�ı��һ·��Ƶͨ���ķֱ���
	
	BOOL32 bNoneKeda = !IsKedaMt(tMt, TRUE);
	BOOL32 bG400IPCMt = IsG400IPCMt(tMt);

	TLogicalChannel tLogicChannel;
	if ( !m_tConfAllMtInfo.MtJoinedConf( byMtId ) ||
		!m_ptMtTable->GetMtLogicChnnl( byMtId, byChnnlType, &tLogicChannel, FALSE ) )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[CheckMtResAdjust]Backward LC has problems. Nack!!\n");
		bRspNack = TRUE;
	}
	
	u8 byMtStandardFormat = tLogicChannel.GetVideoFormat(); // ԭʼ�ֱ���

	// [11/26/2009 xliang] FIXME: ��Ҫ����Res >= ԭʼ�ֱ��ʣ����ڷǿƴ��Ҳ��ack
	if( bNoneKeda || bG400IPCMt)
	{
		//yrl20131120�жϣ���Ҫ����Res >= ԭʼ�ֱ��ʣ����ڷǿƴ��Ҳ��ack
		if (IsSrcResThanDst(byMtStandardFormat, byReqRes))
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[CheckMtResAdjust]None Keda Mt. Nack!!\n");
			bRspNack = TRUE;
			wErrorCode = CASCADE_ADJRESNACK_REASON_NONEKEDA;
		}
	}
	
	if(bRspNack)
	{
		return FALSE;
	}
	
	u8 byNewFormat;
	u8 bySuitableRes = 0;
	// δ������ʱ,��byReqRes����,������ʱ,����ߴ���
	if (0 == dwResW || 0 == dwResH)
	{
		bySuitableRes = GetSuitableRes(byReqRes, byMtStandardFormat);
	}
	else
	{
		u16 wTmpResW = 0;
		u16 wTmpResH = 0;
		GetWHByRes(byMtStandardFormat, wTmpResW, wTmpResH);
		bySuitableRes = GetSuitableResByWH((u16)dwResW, (u16)dwResH, wTmpResW, wTmpResH);
	}
	if( byStart == 0 )
	{
		byNewFormat = byMtStandardFormat;
	}
	else
	{
		byNewFormat = bySuitableRes;
	}
	
	byRealRes = byNewFormat;
	// 	CServMsg cMsg;
	// 	cMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
	// 	cMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
	// 	cMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
	// 	SendMsgToMt( byMtId, cMsg.GetEventId(), cMsg );

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[CheckMtResAdjust] RealRes for Mt.%u is %s. ReqRes is: %s\n", 
		byMtId, GetResStr(byRealRes), GetResStr(byReqRes));
	
	return TRUE;
	
}

/*=============================================================================
    �� �� ���� ProcMcuMcuMuteDumbReq
    ��    �ܣ� ���� ������Mt��������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
	10/08/13   4.6			Ѧ��					create
=============================================================================*/
void CMcuVcInst::ProcMcuMcuMuteDumbReq(const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
	
	TMt *ptMt = (TMt *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
	u8 byMuteOpenFlag = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TMt) );
	u8 byMuteType = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TMt) + sizeof(u8) );

	ProcSingleMtMuteDumbOpr(cServMsg, ptMt, byMuteOpenFlag, byMuteType );

	return;

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
void CMcuVcInst::ProcMcuMcuAdjustMtBitRateCmd( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TMt tMt = *(TMt *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
	u8 bIsRecover = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof( TMt ) );
	u16 wBitRate = *(u16 *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof( TMt ) + sizeof(u8) );
	wBitRate = ntohs(wBitRate);
	TMt tOrgTmt = tMt; 

	TMt tUnLocalMt = GetMtFromMultiCascadeMtInfo(tHeadMsg.m_tMsgDst, tOrgTmt);

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtBitRateCmd]mt(%d.%d) bIsRecover.%d wBitRate.%d\n",
			tUnLocalMt.GetMcuId(),tUnLocalMt.GetMtId(),bIsRecover,wBitRate
			);
	
	if( !tUnLocalMt.IsLocal() )
	{
		SendMsgToMt((u8)tMt.GetMcuId(),MCU_MCU_ADJMTRES_REQ,cServMsg );
		return;
	}

	CSendSpy cSendSpy;
	if( !m_cLocalSpyMana.GetSpyChannlInfo( tUnLocalMt,cSendSpy ) )
	{
		return;
	}
	
	TSimCapSet tSim = cSendSpy.GetSimCapset();

	if( !bIsRecover && tSim.GetVideoMaxBitRate() == wBitRate )
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtBitRateCmd] tSim bitrate.%d is equal bitrate.%d\n",tSim.GetVideoMaxBitRate(),wBitRate );
		return;
	}
	if( bIsRecover && tSim.GetVideoMaxBitRate() ==  m_ptMtTable->GetRcvBandWidth(tUnLocalMt.GetMtId()) )
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtBitRateCmd] bIsRecover tSim bitrate.%d is equal bitrate.%d\n",tSim.GetVideoMaxBitRate(),m_ptMtTable->GetRcvBandWidth(tUnLocalMt.GetMtId()) );
		return;
	}


	ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtBitRateCmd] now bitrate.%d\n",tSim.GetVideoMaxBitRate() );

	
	

	if( IsNeedSpyAdpt(tUnLocalMt,tSim,MODE_VIDEO) )
	{		
		if( bIsRecover )
		{			
			tSim.SetVideoMaxBitRate( m_ptMtTable->GetRcvBandWidth(tUnLocalMt.GetMtId()) );
		}
		else
		{			
			tSim.SetVideoMaxBitRate( wBitRate );
		}
		BOOL32 bIsUp = FALSE;
		if( tSim.GetVideoMaxBitRate() > cSendSpy.GetSimCapset().GetVideoMaxBitRate()  )
		{
			bIsUp = TRUE;
		}
		
		if ( TRUE == bIsUp )
		{
			RefreshSpyBasParam( tUnLocalMt,tSim,MODE_VIDEO, TRUE,TRUE );
		}
		else
		{
			RefreshSpyBasParam( tUnLocalMt,tSim,MODE_VIDEO);
		}
		m_cLocalSpyMana.SaveSpySimCap(tUnLocalMt, tSim);
	}
	else
	{	
		
		TLogicalChannel tLogChn;
		if (m_ptMtTable->GetMtLogicChnnl(tUnLocalMt.GetMtId(), LOGCHL_VIDEO, &tLogChn, FALSE))
		{							
			if( bIsRecover )
			{
				tLogChn.SetFlowControl( m_ptMtTable->GetRcvBandWidth(tUnLocalMt.GetMtId()) );
				tSim.SetVideoMaxBitRate( m_ptMtTable->GetRcvBandWidth(tUnLocalMt.GetMtId()) );
			}
			else
			{
				tLogChn.SetFlowControl(wBitRate);	
				tSim.SetVideoMaxBitRate( wBitRate );
			}									
			cServMsg.SetMsgBody((u8*)&tLogChn, sizeof(tLogChn)); 

			m_cLocalSpyMana.SaveSpySimCap(tUnLocalMt, tSim);
			SendMsgToMt( tUnLocalMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );  
		}
	}
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
	u8* pbyBuf = cServMsg.GetMsgBody();
	u16 wMsgLen = cServMsg.GetMsgBodyLen();
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)pbyBuf;//cServMsg.GetMsgBody();
	pbyBuf += sizeof(TMsgHeadMsg);
	wMsgLen -= sizeof(TMsgHeadMsg);
	TMt tMt = *(TMt *)pbyBuf;
	pbyBuf += sizeof(TMt);
	wMsgLen -= sizeof(TMt);
	TMt tOrgTmt = tMt; //[nizhijun 2010/12/24] ������±��棬Ϊ���������BAS�Ľӿ�ʹ��

	TMsgHeadMsg tHeadMsgRsp;
	tHeadMsgRsp.m_tMsgDst = tHeadMsg.m_tMsgSrc;
	tHeadMsgRsp.m_tMsgSrc = tHeadMsg.m_tMsgDst;
	//20100708_tzy ��Ŀ���ն�ΪNULLʱ�����ʾ����ǰ���ϴ�ͨ���е��ն�
	if (tMt.IsNull())
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResReq]adjust m_tSpyMt res\n");
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
	
		if ( !ptConfMcInfo || ptConfMcInfo->m_tSpyMt.IsNull())
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResReq]CascadeMMCU.m_tSpyMt is NULL\n");
			return;
		}
		tMt = ptConfMcInfo->m_tSpyMt;
	}
	u8 byMtId = tMt.GetMtId();

	u8 byStart = *(u8 *)pbyBuf;		//��ʼ��Res
	pbyBuf += sizeof(u8);
	wMsgLen -= sizeof(u8);
	u8 byReqRes = *(u8 *)pbyBuf;
	pbyBuf += sizeof(u8);
	wMsgLen -= sizeof(u8);
	
	u8 byPos = ~0;
	if( wMsgLen >= sizeof(u8) + sizeof(u8))
	{
		//����byVmpStyle
		pbyBuf += sizeof(u8);
		wMsgLen -= sizeof(u8);
		byPos = *(u8 *)pbyBuf;
		pbyBuf += sizeof(u8);
		wMsgLen -= sizeof(u8);
	}
	u32 dwResW = 0;
	u32 dwResH = 0;
	// ʣ�೤�Ȱ����ֱ��ʿ�͸�
	if (wMsgLen >= sizeof(u32) + sizeof(u32))
	{
		dwResW = *(u32 *)pbyBuf;
		pbyBuf += sizeof(u32);
		wMsgLen -= sizeof(u32);
		dwResH = *(u32 *)pbyBuf;
		pbyBuf += sizeof(u32);
		wMsgLen -= sizeof(u32);
		//�������������תΪ������
		dwResW = ntohl(dwResW);
		dwResH = ntohl(dwResH);
		byReqRes = GetMcuSupportedRes((u16)dwResW, (u16)dwResH);
	}
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResReq]Eqp(%d) Param--tMt:(%d,%d)\tbyStart: %d\tbyFormat: %d\tbyPos: %d\tdwResWH[%d,%d]\n", 
		cServMsg.GetEqpId(), tMt.GetMcuId(), tMt.GetMtId(), byStart, byReqRes, byPos, dwResW, dwResH);

	u16 wNackReason = CASCADE_ADJRESNACK_REASON_UNKNOWN;
	u8 byRealRes	= VIDEO_FORMAT_INVALID;

	// ��Ҫ�������ն��ǵ绰�նˣ���ֱ�ӻظ�Ack���¼�֧�ֵ绰�ն˽�vmp,�ϼ���MCU��vmpʱ������ϴ�ͨ���ն˷ֱ���
	if (IsPhoneMt(tMt))
	{
		cServMsg.SetMsgBody((u8*)&tHeadMsgRsp,sizeof(TMsgHeadMsg));
		cServMsg.CatMsgBody((u8 *)&tMt, sizeof(TMt));
		cServMsg.CatMsgBody(&byStart, sizeof(u8));
		cServMsg.CatMsgBody(&byPos, sizeof(u8));
		SendReplyBack(cServMsg, MCU_MCU_ADJMTRES_ACK);
		
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResReq] tMt(%d,%d) is phone,so return ack. byStart.%d, byPos.%d.\n", 
			tMt.GetMcuId(), byMtId, byStart, byPos);
		return;
	}

	/*[2011/12/14/zhangli]�������ֱ����߼��޸ģ��������⣺�ϼ�mcuͨ��H264-720P��MTͨ��H264-1080P����MT�����ϴ�ͨ��(�����䣬bas720���)��
	Ȼ��MT����vmp(�ش���������)�����ն˷ֱ��ʵ�ΪCIF������ʹ�ϴ��������������ΪCIF*/

	BOOL32 bIsNeedSpyAdp = FALSE;	//�ش��Ƿ������
	BOOL32 bIsNeedSelAdp = FALSE;	//�ϴ��Ƿ������
	BOOL32 bIsInSetOut	 = FALSE;	//�Ƿ����ϴ�
	BOOL32 bIsInSpy		 = FALSE;	//�Ƿ��ڻش�
	TMt tUnLocalMt = GetMtFromMultiCascadeMtInfo(tHeadMsg.m_tMsgDst, tOrgTmt);
	
	//���ն����ϴ�
	TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
	if ( !IsSupportMultiSpy()
		|| ( NULL != ptConfMcInfo && ptConfMcInfo->m_tSpyMt == tUnLocalMt ) 
	    || ( NULL != ptConfMcInfo && IsMcu(tUnLocalMt) && IsSmcuViewMtInMcu(ptConfMcInfo->m_tSpyMt, tUnLocalMt) )
		|| ( NULL != ptConfMcInfo && IsMcu(ptConfMcInfo->m_tSpyMt) && IsSmcuViewMtInMcu(tUnLocalMt, ptConfMcInfo->m_tSpyMt) )
	   )
	{
		bIsInSetOut = TRUE;
		if (IsNeedSelAdpt(tUnLocalMt, m_tCascadeMMCU, MODE_VIDEO))
		{
			bIsNeedSelAdp = TRUE;
		}
	}

	if (IsSupportMultiSpy())
	{		
		CSendSpy *ptSndSpy = NULL;
		TSimCapSet tDstCap;
		for (u8 bySpyIdx = 0; bySpyIdx < MAXNUM_CONF_SPY; bySpyIdx++)
		{
			ptSndSpy = m_cLocalSpyMana.GetSendSpy(bySpyIdx);
			if (NULL == ptSndSpy)
			{
				continue;
			}
			if (!(ptSndSpy->GetSpyMt() == tUnLocalMt))
			{
				continue;
			}
			
			bIsInSpy = TRUE;
			
			tDstCap = ptSndSpy->GetSimCapset();
			
			if (IsNeedSpyAdpt(tUnLocalMt, tDstCap, MODE_VIDEO))
			{
				bIsNeedSpyAdp = TRUE;
			}
			break;
		}
		
		//byStart==1������û�����ϴ�Ҳû���ڻش���ֱ��ack
		//Ӧ�ó�������ش������¼�һ���ն˽�vmp(�¼�����)����ʱ����ն˲���m_cLocalSpyMana�����ն˵ķֱ��ʵ���
		if (byStart == 1 && !bIsInSpy && !bIsInSetOut)
		{
			tDstCap = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());
			//yrl20131120�ܹ�BAS���ն˲Ż�ACK
			if(IsNeedSpyAdpt(tUnLocalMt, tDstCap, MODE_VIDEO))
			{
				//tMt.SetMcuIdx(LOCAL_MCUID);//�˴������3���ն�,��2�������LOCAL�ٷ���1��,�����
				cServMsg.SetMsgBody((u8*)&tHeadMsgRsp,sizeof(TMsgHeadMsg));
				cServMsg.CatMsgBody((u8 *)&tMt, sizeof(TMt));
				cServMsg.CatMsgBody(&byStart, sizeof(u8));
				cServMsg.CatMsgBody(&byPos, sizeof(u8));
				SendReplyBack(cServMsg, MCU_MCU_ADJMTRES_ACK);
				
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResReq] not find in localmana,so return ack,byMtId: %d byNewFormat: %s, byStart: %u \n", 
					byMtId, GetResStr(byReqRes), byStart);	
				return;
			}
		}
	}
	
	//������ϴ��ҹ����䣬����bas
	BOOL32 bIsNAck = FALSE;
	if (bIsInSetOut && bIsNeedSelAdp)
	{
		bIsNAck = !AdjustSpyStreamFromAdpForMMcu(tOrgTmt, byReqRes, tHeadMsg);
	}
	
	//����ڻش��ҹ����䣬����bas
	if (bIsInSpy && bIsNeedSpyAdp)
	{
		bIsNAck = !AdjustSpyStreamFromAdp(tOrgTmt, byReqRes, tHeadMsg) || bIsNAck;
	}
	
	CServMsg cMsg = cServMsg;
	// ȡ�ûش���Ա�ش�����,�����ǻش���Ա,����nack.
	u8 bySpyMode = m_cLocalSpyMana.GetSpyMode(tUnLocalMt);
	//�����һ��û�е��ɹ�����NACK
	if ( bIsNAck && !( MODE_BOTH == bySpyMode || MODE_VIDEO == bySpyMode) )
	{
		cMsg.SetEventId( MCU_MCU_ADJMTRES_NACK );
		cMsg.SetSrcMtId( cServMsg.GetSrcMtId() );
		cMsg.SetEqpId( cServMsg.GetEqpId() );
		cMsg.SetErrorCode(wNackReason);		
		cMsg.SetMsgBody( (u8*)&tHeadMsgRsp,sizeof(TMsgHeadMsg) );
		cMsg.CatMsgBody( cServMsg.GetMsgBody()+sizeof(TMsgHeadMsg),cServMsg.GetMsgBodyLen() - sizeof(TMsgHeadMsg) );
		SendReplyBack( cMsg, MCU_MCU_ADJMTRES_NACK );
		return;
	}

	//�����һ�����ɹ�����ACK
	if ((bIsInSetOut && bIsNeedSelAdp) || (bIsInSpy && bIsNeedSpyAdp))
	{
		cServMsg.SetMsgBody( (u8*)&tHeadMsgRsp,sizeof(TMsgHeadMsg) );
		cServMsg.CatMsgBody( (u8 *)&tOrgTmt, sizeof(TMt) );
		cServMsg.CatMsgBody( &byStart, sizeof(u8) );
		cServMsg.CatMsgBody( &byPos, sizeof(u8) );
		SendReplyBack( cServMsg, MCU_MCU_ADJMTRES_ACK );
		if (tMt.IsMcuIdLocal())
		{
			tMt.SetMcuIdx( LOCAL_MCUIDX );
		}
		byRealRes = byReqRes;
	}
	//���byStart == 0��ֹͣ���ָ��ֱ��ʣ������������ϴ��Ҳ������䣬���ն˷ֱ���
	//���byStart=1,�ڻش��Ҳ������䣬���ﲻ��Ӧ�����ֳ����»�ProcMcuMcuSpyNotify����Ӧ
	//if ( (byStart == 0 ) || (bIsInSetOut && !bIsNeedSelAdp) || (bIsInSpy && !bIsNeedSpyAdp))
	else
	{
		if (!tOrgTmt.IsMcuIdLocal())
		{
			SendMsgToMt((u8)tMt.GetMcuId(),MCU_MCU_ADJMTRES_REQ,cServMsg );
			return;
		}
		else
		{
			tMt.SetMcuIdx( LOCAL_MCUIDX );
		}
		
		if(!CheckMtResAdjust( tMt, byReqRes, wNackReason, byRealRes, byStart, dwResW, dwResH))
		{
			//��nack
			cMsg.SetEventId( MCU_MCU_ADJMTRES_NACK );
			cMsg.SetSrcMtId( cServMsg.GetSrcMtId() );
			cMsg.SetEqpId( cServMsg.GetEqpId() );
			cMsg.SetErrorCode(wNackReason);		
			cMsg.SetMsgBody( (u8*)&tHeadMsgRsp,sizeof(TMsgHeadMsg) );
			cMsg.CatMsgBody( cServMsg.GetMsgBody()+sizeof(TMsgHeadMsg),cServMsg.GetMsgBodyLen() - sizeof(TMsgHeadMsg) );
			SendReplyBack( cMsg, MCU_MCU_ADJMTRES_NACK );
			return;
		}
		
		// [pengjie 2010/9/13] ��ش����ֱ����߼�����
		u8 byChnnlType = LOGCHL_VIDEO;			//Ŀǰֻ�ı��һ·��Ƶͨ���ķֱ���
		SendChgMtVidFormat( byMtId, byChnnlType, byRealRes, TRUE);
		
		tMt.SetMcuIdx( LOCAL_MCUID );
		cServMsg.SetMsgBody( (u8*)&tHeadMsgRsp,sizeof(TMsgHeadMsg) );
		cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );
		cServMsg.CatMsgBody( &byStart, sizeof(u8) );
		cServMsg.CatMsgBody( &byPos, sizeof(u8) );
		SendReplyBack( cServMsg, MCU_MCU_ADJMTRES_ACK );
		
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResReq] byMtId: %d byNewFormat: %s, byStart: %u \n", byMtId, GetResStr(byRealRes), byStart);	
	}

	// [3/16/2011 xliang] save tmt res to multispy related structure
	CSendSpy cSendSpy;
	if ( m_cLocalSpyMana.GetSpyChannlInfo( tUnLocalMt, cSendSpy ) )
	{
		TSimCapSet tDstCap = cSendSpy.GetSimCapset();
		tDstCap.SetVideoResolution(byRealRes);
		m_cLocalSpyMana.SaveSpySimCap(tUnLocalMt, tDstCap);
	}
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
	2013/03/11   4.7.2      �±�                  �޸�(HDU�໭��֧��)
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAdjustMtResAck( const CMessage * pcMsg)
{	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	

	u8  byPos = ~0;
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 &&
		!m_tCascadeMMCU.IsNull() 
		)
	{
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_ADJMTRES_ACK,cServMsg );
		return;
	}
	TMt tMt = *(TMt *)(cServMsg.GetMsgBody()+sizeof(TMsgHeadMsg));	//�����õ�MT
	u8  byStart = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt)+sizeof(TMsgHeadMsg));
	if( cServMsg.GetMsgBodyLen() > (sizeof(TMt) + sizeof(u8) +sizeof(TMsgHeadMsg)) )
	{		
		//��Ϣ�� = TMsgHeadMsg(��Ϣͷ)+TMt+byStart(��/�ָ�)+byPos(�����ķ��λ��)
		byPos = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TMt) + sizeof(u8));		
	}
	if (byPos == 0xFF) //zhouyiliang 20101124����¼�û�л�pos�����߻ص�pos�Ǹ�0xff������Ҫ��������صĴ�����
	{
		return;
	}

	tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tMt );

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResAck] Adjust Mt(%u,%u) res success!---bStart:%u byPos(%d)\n",
		tMt.GetMcuId(), tMt.GetMtId(), byStart ,byPos
			);

	// [2013/03/11 chenbing] Hdu�໭������ֱ��� 
	if(byStart)
	{
		// [2013/03/11 chenbing] �õ�EqpId 
		u8  byEqpId    = cServMsg.GetEqpId();

		// [2013/03/11 chenbing] �Ƿ�ΪHDU 
		if ( IsValidHduEqp(g_cMcuVcApp.GetEqp(byEqpId)) )
		{
			u8  byChnId    = cServMsg.GetChnIndex();
			u8  bySubChnId = byPos;

			TPeriEqpStatus tHduStatus;
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tHduStatus);
			TMt tMtTemp = tHduStatus.m_tStatus.tHdu.GetChnMt(byChnId, bySubChnId);
			if ( !tMtTemp.IsNull() )
			{
				// ֪ͨ�¼����ش�����SpyNotify,�˴����ܵ���ChangeHduSwitch,������Ҫ�ڴ��·�SpyNotify
				// ����ChangeHduSwitch����ɵ��ش����ϼ����¼�Mcu��Mtͼ��ˢ�´���
				if ( IsLocalAndSMcuSupMultSpy(tMt.GetMcuIdx()) )
				{
					SendMMcuSpyNotify( tMt, MCS_MCU_START_SWITCH_HDU_REQ, GetHduChnSupportCap(tMt, byEqpId));
				}

				StartSwitchToPeriEqp(tMt, 0, byEqpId, byChnId, MODE_VIDEO,
					SWITCH_MODE_BROADCAST, FALSE, TRUE, FALSE, TRUE, TRUE, bySubChnId, HDUCHN_MODE_FOUR);

				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,
					"[ProcMcuMcuAdjustMtResAck] Mt(%u,%u), Adjusted Res Success SetHduChnnl[%d] and SetHduSubChnnl[%d]!\n", 
					tMt.GetMcuId(), tMt.GetMtId(), byChnId, bySubChnId);
				return;
			}
		}
		else if (IsValidVmpId(byEqpId))
		{
			TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byEqpId );

			// �����յ����ֱ���Ack׼��Setvmpchnlʱ,���ֵ�ǰ����vmp������Ϣ,���Դ˴�Setvmpchnl,�ȴ�����Ϣ����
			// ��ֹSetvmpchnlʱ��m_tConf��VmpParam���ǵ�Eqpstatus��.
			if(!m_tConfInStatus.IsVmpNotify())
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResAck] VmpNotify is false, can't set tMt(%d,%d)  in vmp channel:%d.\n",tMt.GetMcuId(), tMt.GetMtId(), byPos);
				return;
			}

			TPeriEqpStatus tPeriEqpStatus; 
			BOOL32 bSetChnnl = FALSE;
			u8 byVmpMemberType = VMP_MEMBERTYPE_VAC;
			TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			tVmpParam.Print(LOG_LVL_KEYSTATUS);

			//1.���ֱ���ACK�������ĵ�tmt��pos���ϼ�vmpparam���һ�£�������
			//2.���ֱ���ACK��������tmt���ϼ�vmpparam����Ӧpos��tmtͬ�����ش�ͬһMCU�µ��նˡ�TMT��VMPParam��ԭ�����Ϊ׼[9/19/2012 chendaiwei]
			if(byPos < MAXNUM_VMP_MEMBER && (tMt == (TMt)*tVmpParam.GetVmpMember(byPos) ||
				(GetLocalMtFromOtherMcuMt(tMt) == GetLocalMtFromOtherMcuMt((TMt)*tVmpParam.GetVmpMember(byPos))
				 && !IsLocalAndSMcuSupMultSpy(tMt.GetMcuIdx())
	 			 && IsAllowVmpMemRepeated(byEqpId)))
				)			//������VMPĳͨ���Ľ���
			{
				//�����ڶ������[9/19/2012 chendaiwei]
				tMt = (TMt)*tVmpParam.GetVmpMember(byPos);

				TVMPParam_25Mem tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
				TVMPMember* ptVmpMember = tVmpParam.GetVmpMember(byPos);
				TVMPMember* ptLastVmpMember = tLastVmpParam.GetVmpMember(byPos);
				byVmpMemberType = ptVmpMember->GetMemberType();
				bSetChnnl = TRUE;

				if( bSetChnnl)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Mt(%u,%u), Pos.%u in McuMcuadjResAck, begin to SetVmpChnnl!\n", tMt.GetMcuId(), tMt.GetMtId(), byPos);
					
					TMt tMcu = (TMt)(*ptVmpMember);
					// yanghuaizhi20110802 �������ϳ���ΪSMCU����SMCU������ϳɴ�����ֹ��ʵ�ն˽���ش�������
					if (NULL != ptLastVmpMember &&  MT_TYPE_SMCU == ptLastVmpMember->GetMtType() && (TMt)(*ptVmpMember) == (TMt)(*ptLastVmpMember)
						&&  IsMtInMcu(tMcu,tMt)
						)
					{
						//���conf��vmpparam����ó�Ա�Ѿ���mcu����lastҲ��mcu����ͨ����Ա���������ã�changemvpswitch�����Ѿ�������mcu
						//��vmp�Ľ���
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResAck] mt.(%d,%d) is chnl:%d vmpmember(mcu)'s viewmt,no need setvmpchnl\n",
							tMt.GetMcuId(),tMt.GetMtId(),byPos);
					}
					else
					{
						SetVmpChnnl(byEqpId, tMt, byPos, byVmpMemberType);
					}

					// zjj20090911 ����VCS�������������ϳɵ�ģʽ��,������Ա�뻭��ϳɳ�Ա��
					if (VCS_CONF == m_tConf.GetConfSource() && 
						VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode() &&
						m_tConf.m_tStatus.IsMixing())
					{
						u8 byMemNum = tVmpParam.GetMaxMemberNum();
						TMt tTempMt;
						TMt atMixMember[MAXNUM_CONF_MT];
						u8  byMixMemNum = 0;
						for (u8 byVMPMemIdx = 0; byVMPMemIdx < byMemNum; byVMPMemIdx++)
						{
							tTempMt = (TMt)(*tVmpParam.GetVmpMember(byVMPMemIdx));
							if (!tTempMt.IsNull())
							{
								atMixMember[byMixMemNum++] = tTempMt;				
							}

						}
						ChangeSpecMixMember(atMixMember, byMixMemNum);

					}
				}
				else
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResAck] not set in vmp chnnl for certain reason!!\n");
				}
			}
			else
			{
				//����Ԥ������
			}
		}
		else
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,	"[ProcMcuMcuAdjustMtResAck] byEqpId(%d) is Not Valid HDUId or VMPid.\n", byEqpId);
		}
	}

}

/*=============================================================================
    �� �� ���� ProcMcuMcuAdjustMtResNack
    ��    �ܣ� ���� ������Mt�ֱ��� Ӧ��nack
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
	20/11/2009   4.6			Ѧ��					create
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAdjustMtResNack( const CMessage * pcMsg)
{	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	

	u8  byPos = ~0;
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_ADJMTRES_NACK,cServMsg );
		return;
	}
	u16 wErrorCode = cServMsg.GetErrorCode();
	TMt tMt = *(TMt *)(cServMsg.GetMsgBody() +sizeof(TMsgHeadMsg));	//�����õ�MT
	u8  byStart = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt)+sizeof(TMsgHeadMsg));
	if( cServMsg.GetMsgBodyLen() > (sizeof(TMt) + sizeof(u8) + sizeof(u8) + sizeof(u8)+sizeof(TMsgHeadMsg)) )
	{		
		byPos = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(u8) + sizeof(u8)+sizeof(TMsgHeadMsg));
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResNack] has byPos(%d)\n",byPos );
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResNack] VideoFormat(%d) vmpStyle(%d) ChannelPos(%d) len(%d)\n",
			*(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8)),
			*(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(u8)),
			*(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(u8) + sizeof(u8)),
			cServMsg.GetMsgBodyLen()
			);
	}

	// �¼����ֱ���Nack��Ϣ���½���cMsg���ϰ�mcu��֧�ַ���EqpId�����ݿ��ǣ�Nackʱ���������´�����������Nack��������
	tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tMt );
	u8 byEqpId = cServMsg.GetEqpId();
	
	if (!byStart)
	{
		// do nothing
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResNAck]EqpId(%d) Adjust Mt(%u,%u) res failed for reason:%d!---bStart:%u --byPos:%d \n",
			byEqpId, tMt.GetMcuId(), tMt.GetMtId(), wErrorCode, byStart,byPos );
		return;
	}

	// [2013/03/11 chenbing] Hdu�໭������ֱ��� 
	// [2013/03/11 chenbing] �Ƿ�ΪHDU 
	if ( IsValidHduEqp(g_cMcuVcApp.GetEqp(byEqpId)) )
	{
		u8  byChnId    = cServMsg.GetChnIndex();
		u8  bySubChnId = byPos;
		
		TPeriEqpStatus tHduStatus;
		g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tHduStatus);

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,
			"[ProcMcuMcuAdjustMtResNAck] Adjust Res Faild CurHduMode(%d) Mt(%d.%d) HduId(%d) ChnId(%d) SubChnId(%d)\n",
				tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byChnId),
				tMt.GetMcuId(),tMt.GetMtId(), byEqpId, byChnId, bySubChnId );	

		//�����ֱ�����ʧ�ܣ��������ǽ
		ChangeHduSwitch(NULL, byEqpId, byChnId, bySubChnId, tHduStatus.m_tStatus.tHdu.GetMemberType(byChnId, bySubChnId),
				TW_STATE_STOP, g_cMcuVcApp.GetChnnlMMode(byEqpId, byChnId), FALSE, FALSE);

		//���ܽ��ֱ��ʵ��ն˲��ܽ�����
		NotifyMcsAlarmInfo(0, ERR_MCU_RESOURCES_NOT_ENOUGH);

		return;
	}
	else if (IsValidVmpId(byEqpId))
	{
		// ����ϳ���ش���
		TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byEqpId );
		
		TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
		
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResNAck]EqpId(%d) Adjust Mt(%u,%u) res failed for reason:%d!---bStart:%u --byPos:%d vmpMode:%d IsAuto:%d\n",
			byEqpId, tMt.GetMcuId(), tMt.GetMtId(), wErrorCode, byStart,byPos,
			tVmpParam.GetVMPMode(),
			tVmpParam.IsVMPAuto()
			);
		//������VMPĳͨ���Ľ�������
		if( byPos < MAXNUM_VMP_MEMBER
			&& tVmpParam.GetVMPMode() != CONF_VMPMODE_NONE
			&& IsChangeVmpParam()// ����ǰ����vmp������Ϣ,���Դ˴�Setvmpchnl,�ȴ�����Ϣ����,��ֹSetvmpchnlʱ��m_tConf��VmpParam���ǵ�Eqpstatus��.
			)			
		{
			BOOL32 bSetChnnl = TRUE;		//�Ƿ���Խ�����
			u8 byVmpMemberType = VMP_MEMBERTYPE_VAC;

			//VMPУ��
			
			if( !tVmpParam.IsVMPAuto() )						//�Զ�����ϳɲ�У���Ա
			{
				TVMPMember tVmpMember = *tVmpParam.GetVmpMember(byPos);
				if ( !(tVmpMember == tMt) )//���ڶ��ƺϳɣ�����һ�£��Է�ack�յ�֮ǰ�ϳɲ����б�
				{
					bSetChnnl = FALSE;
				}
				else
				{
					byVmpMemberType = tVmpMember.GetMemberType();
				}
			}
			
			if( bSetChnnl )
			{
				//��nack reason
				switch ( wErrorCode)
				{
				case CASCADE_ADJRESNACK_REASON_NONEKEDA:
					{
						//��Ȼ�ܽ�����������:
						//a, 4���������
						//b, ��ǰ����ͨ����ռ
						//FIXME: ǰ����ͨ����ռ
						// �����¼����⳧���ն�ֱ�����ϼ��жϣ������ڴ��ֳ�������Ϊ���ɽ����������޷�ռ��ǰ����
						bSetChnnl = FALSE;//ChangeMtVideoFormat(tMt, TRUE, TRUE);
						break;
					}
				default:
					bSetChnnl = FALSE;//����������Ϊ���ܽ����������ɽ���
					break;
					
				}
			}

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResNAck]  Mt(%d.%d) bSetChnnl(%d) byPos(%d)\n",
						tMt.GetMcuId(),tMt.GetMtId(),bSetChnnl,byPos );

			if( bSetChnnl)
			{
				SetVmpChnnl(tVmpEqp.GetEqpId(), tMt, byPos, byVmpMemberType);

				// zjj20090911 ����VCS�������������ϳɵ�ģʽ��,������Ա�뻭��ϳɳ�Ա��
				if (VCS_CONF == m_tConf.GetConfSource() && 
					VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode() &&
					m_tConf.m_tStatus.IsMixing())
				{
					u8 byMemNum = tVmpParam.GetMaxMemberNum();
					TMt tTempMt;
					TMt atMixMember[MAXNUM_CONF_MT];
					u8  byMixMemNum = 0;
					for (u8 byVMPMemIdx = 0; byVMPMemIdx < byMemNum; byVMPMemIdx++)
					{
						tTempMt = (TMt)(*tVmpParam.GetVmpMember(byVMPMemIdx));
						if (!tTempMt.IsNull())
						{
							atMixMember[byMixMemNum++] = tTempMt;				
						}

					}
					ChangeSpecMixMember(atMixMember, byMixMemNum);

				}
			}
			else
			{
				// ��Last�г�Ա���µ�m_tConf��VmpParam��,ʹ�䱣��һ��<�������ֱ���ʧ�ܣ����ܽ�LastVmpParam��Ա���ǵ�ConfVmpParam�У����ɽ���>
				//m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byPos, *m_tLastVmpParam.GetVmpMember(byPos));
				u8 byMode = MODE_VIDEO;
				TVMPParam_25Mem tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
				if (VMP_MEMBERTYPE_DSTREAM == tLastVmpParam.GetVmpMember(byPos)->GetMemberType())
				{
					byMode = MODE_SECVIDEO;
				}
				StopSwitchToPeriEqp(tVmpEqp.GetEqpId(), byPos, FALSE, byMode);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResNack] not set in vmp chnnl for certain reason!!\n");
			}
			
		}
		else
		{
			//����Ԥ������
		}

	}
	else
	{
		//yrl20131120�˴�����������MCU������ǽ4���棬Ȼ���Ϸǿƴ��ն˽��ϴ�ͨ����ȡ����EqpId
		TMt tTempMcu = GetLocalMtFromOtherMcuMt(tMt);
		if (GetSMcuViewMt(tTempMcu, TRUE) == tMt || GetSMcuViewMt(tTempMcu, FALSE) == tMt)
		{
			//�ǿƴ��ն˲��ܽ��ֱ��ʲ���¼�Mcu��HDU2�໭��Ľ���
 			StopViewToAllHduVmpSwitch(tTempMcu);
		}

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,	"[ProcMcuMcuAdjustMtResNAck] byEqpId(%d) is Not Valid HDUId or VMPId.\n",	byEqpId);
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

	tMixParam.ClearAllMembers();
	
	for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
	{
		if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && m_ptMtTable->IsMtInMixing(byLoop))            
		{
			if (m_ptMtTable->GetMt(byLoop).GetMtId() != m_tCascadeMMCU.GetMtId())
			{
				tMixParam.AddMember(m_ptMtTable->GetMt(byLoop).GetMtId());		
			}
		}
	}
	
	m_tConf.m_tStatus.SetMixerParam(tMixParam);

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
	//CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
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
	//CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
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
	if ( m_tCascadeMMCU.IsNull() )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMcuMcuAddMixerMemberCmdAddMixerMemberCmd] m_tCascadeMMCU.IsNull, impossible \n" );
		return;
    }
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	STATECHECK

	CServMsg cMsg(cServMsg);
	TMcu tmcu;
	cMsg.SetMsgBody((u8 *)&tmcu, sizeof(TMcu));
	TMt *ptTempMt = NULL;
	u8 byMixNum = (cServMsg.GetMsgBodyLen() - sizeof(u8))/sizeof(TMt);
	BOOL32 bIsReturn = FALSE;

	//��֯���л�����Ա
	for (u8 byIdx = 0; byIdx < byMixNum; byIdx++)
	{
		//���������ն�
		ptTempMt = (TMt *)(cServMsg.GetMsgBody() + sizeof(TMt) * byIdx);
		if ( ptTempMt == NULL)
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcMcuMcuAddMixerMemberCmd] warning ptTempMt == NULL,so return.\n" );
			return;
		}

		//�ѵ�3���ն˵�MCU���óɱ��������ն�
		if (!ptTempMt->IsLocal())
		{
			u16 wMcuIdx = GetMcuIdxFromMcuId(u8(ptTempMt->GetMcuId()));
			ptTempMt->SetMcuIdx(wMcuIdx);
		}

		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcMcuMcuAddMixerMemberCmd] MT(%d,%d) Local(%d)\n", 
			       ptTempMt->GetMcuId(), ptTempMt->GetMtId(), ptTempMt->IsLocal() );

		cMsg.CatMsgBody((u8 *)ptTempMt, sizeof(TMt));
	}

	u8 byReplace = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMt) * byMixNum);
	cMsg.CatMsgBody( (u8 *)&byReplace, sizeof(u8) );
	cServMsg.SetMsgBody((u8*)cMsg.GetMsgBody(), cMsg.GetMsgBodyLen()); 
		
	ProcMixStart(cServMsg);
 
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
	
	u8 *pMsgBody = (u8 *)cServMsg.GetMsgBody();

	pMsgBody += sizeof(TMcu);
	TMt *ptMt  = (TMt*)pMsgBody;

	u8 byMtNum = 0;
	byMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TMcu))/sizeof(TMt);
	pMsgBody += byMtNum * sizeof(TMt);

	//lukunpeng 2010/05/27 ��û�л�����Աʱͣ��������������ϼ����ϰ汾���¼�Ĭ��Ϊ��ͣ����
//	BOOL32 bNotStopNoMix = TRUE;
	//lukunpeng 2010/05/27 �Ƿ�ǿ��ɾ��������Ա����������ϼ����ϰ汾���¼�Ĭ��Ϊǿ��ɾ��
	BOOL32 bCancelForceMix = FALSE;

	//lukunpeng 2010/05/27 �жϼ���������
// 	if(cServMsg.GetMsgBodyLen() >= sizeof(TMcu) + byMtNum * sizeof(TMt) + 2 * sizeof(u8))
// 	{
// 		bNotStopNoMix	= (*(u8 *)pMsgBody == 1) ? TRUE : FALSE;
// 		pMsgBody += sizeof(u8);
// 		bForceRemove		= (*(u8 *)pMsgBody == 0) ? TRUE : FALSE;	
// 	}

	if(cServMsg.GetMsgBodyLen() >= sizeof(TMcu) + byMtNum * sizeof(TMt) + sizeof(u8))
	{
		bCancelForceMix		= (*(u8 *)pMsgBody == 0) ? FALSE : TRUE;	
	}
	
	STATECHECK
	if (!ptMt->IsLocal())
	{
		TMt *pTmpMt = ptMt;

		TMcu tMcu = *((TMcu*)cServMsg.GetMsgBody());
		tMcu.SetMcuId(GetMcuIdxFromMcuId(u8(pTmpMt->GetMcuId())));
		memcpy((u8*)(cServMsg.GetMsgBody()),(u8*)&tMcu,sizeof(TMcu));

		for( u8 byIdx = 0;byIdx < byMtNum; ++byIdx )
		{			
			if (!pTmpMt->IsLocal())
			{
				pTmpMt->SetMcuIdx(GetMcuIdxFromMcuId(u8(pTmpMt->GetMcuId())));				
			}
			
			// [miaoqingsong 20111209] ��ӱ������������������ϼ�ɾ���¼�������Աʱ��Ҫɾ���ĵ������ն˲��ڻ���ʱ�������Է�ֹ�����������ɾ����Ա��Ϣ
			//            ���µڶ�������¼�ĵ�������Ա��Ŀ����ȷ��(Bug00071831)
			if (!m_ptConfOtherMcTable->IsMtInMixing(*pTmpMt))
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcMcuMcuRemoveMixerMemberCmd] The Mt(%d,%d) is not in Mixer, Can't remove, so return!\n", 
					pTmpMt->GetMcuId(), pTmpMt->GetMtId() );
				
				return;
			}

			++pTmpMt;
		}

		OnAddRemoveMixToSMcu(&cServMsg,FALSE,TRUE);
	}
	else
	{
		if (m_ptMtTable->GetMtType(ptMt->GetMtId()) == MT_TYPE_SMCU)
		{
			TConfMcInfo *ptMcInfo = NULL;
			ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(ptMt->GetMtId()));
			if( NULL != ptMcInfo )
			{
				//20101224_tzy �����MCU��ԭ���ϴ��ն��Զ����Ļ������򽫸����ն��Ƴ��������������ȡ���ϴ��ն˻�Ҫ�������ն��Զ�ͣ��������־
				if(ptMcInfo != NULL && m_ptConfOtherMcTable->IsMtInMixing(ptMcInfo->m_tMMcuViewMt) && ptMcInfo->GetMtAutoInMix(ptMcInfo->m_tMMcuViewMt))
				{
					RemoveSpecMixMember( &ptMcInfo->m_tMMcuViewMt,1,FALSE);
				}

				// miaoqingsong [20110921] �汾���ݣ�R2֧�ֽ�MCU�������ͨ���������ƻ���
				// Bug00065017��R2-R6-R2�����������飬�رյ�һ������ͨ���еĵ�����MCUӦ֧��ɾ���¼�MCU
				RemoveSpecMixMember( ptMt,1,FALSE);
			}
		} 
		else
		{
			RemoveSpecMixMember( ptMt, byMtNum, bCancelForceMix);
		}
	}

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

	if(  !m_tCascadeMMCU.IsNull() && byMcuId == m_tCascadeMMCU.GetMtId() )
	{
		TConfMcInfo *ptMcInfo = NULL;
		u16 wMMcuIdx = INVALID_MCUIDX;
		
		if( !m_tCascadeMMCU.IsNull() )
		{
			wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );			
		}
		for( u16 wLoop = 0; wLoop < TConfOtherMcTable::GetMaxMcuNum(); wLoop++ )
		{
			ptMcInfo = (m_ptConfOtherMcTable->GetMcInfo(wLoop));
			if( ( NULL == ptMcInfo || !IsValidSubMcuId(ptMcInfo->GetMcuIdx())) 
				|| ( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->GetMcuIdx() == wMMcuIdx ) )
			{
				continue;
			}

			if( m_tConfAllMcuInfo.IsSMcuByMcuIdx(ptMcInfo->GetMcuIdx()) )
			{
				SendMsgToMt( GetFstMcuIdFromMcuIdx(ptMcInfo->GetMcuIdx()), MCU_MCU_LOCK_REQ, cServMsg);		
			}
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

	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(cServMsg.GetSrcMtId()));
	if (NULL == ptMcInfo)
	{
		cServMsg.SetSrcMtId(0);
		cServMsg.SetEventId(MCU_MCS_LOCKSMCU_NACK);
		SendReplyBack( cServMsg, cServMsg.GetEventId());
	}
	BOOL32 byLock = (*(cServMsg.GetMsgBody()+sizeof(TMcu)) != 0) ? TRUE : FALSE;

	if(ptMcInfo)
		ptMcInfo->SetIsLocked(byLock);

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
	//CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
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
	if (INVALID_MCUIDX != ptMcu->GetMcuIdx())
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
		u16 wMMcuIdx = INVALID_MCUIDX;
		if( !m_tCascadeMMCU.IsNull() )
		{
			wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
			//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
		}
		for (u16 wLoop = 0; wLoop < TConfOtherMcTable::GetMaxMcuNum(); wLoop++)
		{
			ptMcInfo = (m_ptConfOtherMcTable->GetMcInfo(wLoop));
			if ( NULL == ptMcInfo 
				|| !IsValidSubMcuId(ptMcInfo->GetMcuIdx()) 
				|| (!m_tCascadeMMCU.IsNull() && ptMcInfo->GetMcuIdx() == wMMcuIdx))
			{
				continue;
			}

			tMcu.SetNull();
			tMcu.SetMcu(0);
			tMcu.SetMcuIdx( ptMcInfo->GetMcuIdx() );
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
    TMt		tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());  //sgx
	
	STATECHECK;	

	if( NULL == m_ptConfOtherMcTable )
	{
		return;
	}

	TMcu *ptMcu = (TMcu *)(cServMsg.GetMsgBody());
	TConfMcInfo *ptMcInfo = NULL;
	
	if( NULL == ptMcu || 0 == ptMcu->GetMcuId() )
	{
		return;
	}

	u16 wMcuIdx = INVALID_MCUIDX;
	u8 abyMcuId[MAX_CASCADEDEPTH-1];
	abyMcuId[0] = (u8)ptMcu->GetMcuId();
	if( ptMcu->GetMtId() != 0 )
	{
		abyMcuId[1] = ptMcu->GetMtId();
		m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],2,&wMcuIdx );
	}
	else
	{
		m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],1,&wMcuIdx );
	}
	
	ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
	if( NULL == ptMcInfo )
	{
		return;
	}

	u8 byMtNum = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMcu) );//(cServMsg.GetMsgBodyLen()-sizeof(TMcu))/sizeof(TSMcuMtStatus);
	
	TSMcuMtStatus *ptSMcuMtStatus = (TSMcuMtStatus *)(cServMsg.GetMsgBody()+sizeof(TMcu)+sizeof(u8));
	CServMsg cServBackMsg;
	cServBackMsg.SetNoSrc();
	u8 byMtId = 0;
	BOOL32 bIsSend = FALSE;

	
	for( u8 byPos=0; byPos<byMtNum; byPos++ )
	{	
		if( ptSMcuMtStatus->m_byCasLevel > 0 )
		{
			byMtId  = ptSMcuMtStatus->m_abyMtIdentify[ptSMcuMtStatus->m_byCasLevel-1];
		}
		else
		{
			byMtId  = ptSMcuMtStatus->GetMtId();
		}
		
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
			
		bIsSend = FALSE;
		if( nIndex < MAXNUM_CONF_MT )
		{
			/*//ptMcInfo->m_atMtStatus[nIndex].SetMt( *((TMt *)ptSMcuMtStatus) );
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
			//zjj20100327
			ptMcInfo->m_atMtStatus[nIndex].SetMtDisconnectReason( ptSMcuMtStatus->GetMtDisconnectReason() );
			//sgx20100703[Bug00033516] �����¼��ն˵ĺ��з�ʽ
            if (ptSMcuMtStatus->IsAutoCallMode())
			{
                ptMcInfo->m_atMtExt[nIndex].SetCallMode(CONF_CALLMODE_TIMER);
			}
			else
			{
                ptMcInfo->m_atMtExt[nIndex].SetCallMode(CONF_CALLMODE_NONE);
			}*/			

			if( ptMcInfo->m_atMtStatus[nIndex].IsEnableFECC() != ptSMcuMtStatus->IsEnableFECC() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetIsEnableFECC( ptSMcuMtStatus->IsEnableFECC() );
			}
			if( ptMcInfo->m_atMtStatus[nIndex].GetCurVideo() != ptSMcuMtStatus->GetCurVideo() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetCurVideo( ptSMcuMtStatus->GetCurVideo() );
			}
			if( ptMcInfo->m_atMtStatus[nIndex].GetCurAudio() != ptSMcuMtStatus->GetCurAudio() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetCurAudio( ptSMcuMtStatus->GetCurAudio() );
			}
			if( ptMcInfo->m_atMtStatus[nIndex].GetCurAudio() != ptSMcuMtStatus->GetCurAudio() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetCurAudio( ptSMcuMtStatus->GetCurAudio() );
			}
			if( ptMcInfo->m_atMtStatus[nIndex].GetMtBoardType() != ptSMcuMtStatus->GetMtBoardType() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetMtBoardType( ptSMcuMtStatus->GetMtBoardType() );   
			}
			if( ptMcInfo->m_atMtStatus[nIndex].IsInMixing() != ptSMcuMtStatus->IsMixing() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetInMixing( ptSMcuMtStatus->IsMixing() );
				if (ptSMcuMtStatus->IsMixing())
				{
					MtStatusChange(&tMt,TRUE);
				}
			}
			if( ptMcInfo->m_atMtStatus[nIndex].IsVideoLose() != ptSMcuMtStatus->IsVideoLose() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetVideoLose( ptSMcuMtStatus->IsVideoLose() );
			}
			if( ptMcInfo->m_atMtStatus[nIndex].IsReceiveVideo() != ptSMcuMtStatus->IsRecvVideo() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetReceiveVideo( ptSMcuMtStatus->IsRecvVideo() );
			}
			if( ptMcInfo->m_atMtStatus[nIndex].IsReceiveAudio() != ptSMcuMtStatus->IsRecvAudio() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetReceiveAudio( ptSMcuMtStatus->IsRecvAudio() );
			}

			// [2013/07/26 chenbing] �ն��Ƿ��ڷ�����Ƶ 
			if( ptMcInfo->m_atMtStatus[nIndex].IsSendVideo() != ptSMcuMtStatus->IsSendVideo() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetSendVideo( ptSMcuMtStatus->IsSendVideo() );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuMtStatusNotif] New Video: %d\n", ptSMcuMtStatus->IsSendVideo());
				if ( ptSMcuMtStatus->IsSendVideo()
					&& !m_tConf.GetSpeaker().IsNull()
					&& ptMcInfo->m_atMtStatus[nIndex] == m_tConf.GetSpeaker()
					)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuMtStatusNotif] ChangeSpeaker Video Switch\n");
					ChangeSpeaker(&m_tConf.GetSpeaker());
				}
			} 
			
			// [2013/07/26 chenbing] �ն��Ƿ��ڷ�����Ƶ
			if( ptMcInfo->m_atMtStatus[nIndex].IsSendAudio() != ptSMcuMtStatus->IsSendAudio() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetSendAudio( ptSMcuMtStatus->IsSendAudio() );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuMtStatusNotif] New Audio: %d\n", ptSMcuMtStatus->IsSendAudio());
				if ( ptSMcuMtStatus->IsSendAudio()
					&& !m_tConf.GetSpeaker().IsNull()
					&& ptMcInfo->m_atMtStatus[nIndex] == m_tConf.GetSpeaker()
					)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuMtStatusNotif] ChangeSpeaker Audio Switch\n");
					ChangeSpeaker(&m_tConf.GetSpeaker());
				}
			}

			//zjj20100327
			if( ptMcInfo->m_atMtStatus[nIndex].GetMtDisconnectReason() != ptSMcuMtStatus->GetDisconnectReason() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetMtDisconnectReason( ptSMcuMtStatus->GetDisconnectReason() );
			}
			
			if( ptMcInfo->m_atMtStatus[nIndex].IsSndVideo2() != ptSMcuMtStatus->IsSendVideo2() )
			{
				if (ptSMcuMtStatus->IsSendVideo2())
				{
					if( !m_tDoubleStreamSrc.IsNull() && 
						m_tDoubleStreamSrc.GetMtId() == GetLocalMtFromOtherMcuMt((TMt)(ptMcInfo->m_atMtStatus[nIndex])).GetMtId())
					{		
						bIsSend = TRUE;
						ptMcInfo->m_atMtStatus[nIndex].SetSndVideo2( ptSMcuMtStatus->IsSendVideo2() );
						UpdateVmpDStream((TMt)(ptMcInfo->m_atMtStatus[nIndex]));
					}
				}
				else
				{
					bIsSend = TRUE;
					ptMcInfo->m_atMtStatus[nIndex].SetSndVideo2(ptSMcuMtStatus->IsSendVideo2());
				}
			}			
			//sgx20100703[Bug00033516] �����¼��ն˵ĺ��з�ʽ
            if (ptSMcuMtStatus->IsAutoCallMode())
			{
				if( ptMcInfo->m_atMtExt[nIndex].GetCallMode() != CONF_CALLMODE_TIMER )
				{
					bIsSend = TRUE;
					ptMcInfo->m_atMtExt[nIndex].SetCallMode(CONF_CALLMODE_TIMER);
				}
			}
			else
			{
				if( ptMcInfo->m_atMtExt[nIndex].GetCallMode() != CONF_CALLMODE_NONE )
				{
					bIsSend = TRUE;
					ptMcInfo->m_atMtExt[nIndex].SetCallMode(CONF_CALLMODE_NONE);
				}
			}	

			if( bIsSend )
			{
				MtStatusChange( (TMt*)&(ptMcInfo->m_atMtStatus[nIndex]));
			}

		}
		ptSMcuMtStatus++;
	}
	//sgx20100703[Bug00033516]��ӡ��¼�MCUֱ���ն��б���Ϣ���͵���ء��Ĺ���
	//zjj ����ֱ�ӷ����б����棬�ᵼ�½��濨��
	//SendMtListToMcs( wMcuIdx );

	ptSMcuMtStatus = (TSMcuMtStatus *)(cServMsg.GetMsgBody()+sizeof(TMcu)+sizeof(s32));

	if( !m_tCascadeMMCU.IsNull() && 
		MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) &&
		ptSMcuMtStatus->m_byCasLevel < MAX_CASCADELEVEL
		)
	{
		// ��������˻��巢��, ���û��巢��, ����ֱ�ӷ���
		if( g_cMcuVcApp.GetMcuMcuMtStatusBufFlag() != 0 )
		{			
			// [11/15/2011 liuxu] ��ȡ���巢��
			m_cSMcuMtStatusToMMcuBuf.Add(wMcuIdx);
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Buf SMcu.%d MtStatus To MMcu\n", wMcuIdx);
			if (m_byMtStatusToMMcuTimerFlag)
			{
				return;
			}else
			{
				SetTimer(MCUVC_SEND_SMCUMTSTATUS_TIMER, g_cMcuVcApp.GetRefreshSMcuMtInterval());
				m_byMtStatusToMMcuTimerFlag = 1;
				return;
			}
		}else
		{
			//u8 byMtNum = (u8)nMtNum;
			cServBackMsg.SetMsgBody( (u8*)&byMtNum, sizeof( byMtNum ) );
			cServBackMsg.CatMsgBody( cServMsg.GetMsgBody()+sizeof(u8)+sizeof(TMcu),
				cServMsg.GetMsgBodyLen()-sizeof(u8)-sizeof(TMcu)
				);
			cServBackMsg.SetEventId( MCU_MCU_MTSTATUS_NOTIF );
			cServBackMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
			SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_MTSTATUS_NOTIF, cServBackMsg );
		}
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

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAutoSwitchReq] tSwitchReq with On.%d, level.%d, time.%d, lid.%d\n",
            tSwitchReq.m_bSwitchOn, tSwitchReq.m_nAutoSwitchLevel, 
            tSwitchReq.m_nSwitchSpaceTime, tSwitchReq.m_nSwitchLayerId );
    
    if ( m_tCascadeMMCU.IsNull() ||
         bySrcMtId != m_tCascadeMMCU.GetMtId() ||
         ( MT_MANU_RADVISION != m_ptMtTable->GetManuId(m_tCascadeMMCU.GetMtId()) &&
           MT_MANU_KDCMCU != m_ptMtTable->GetManuId(m_tCascadeMMCU.GetMtId()) ) )
    {
        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[AutoSwitchReq] Roused by Mt.%d, Manu.%d, ignore it\n",
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

        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Start auto poll switch.\n" );
        
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
void CMcuVcInst::ProcMcuMcuAutoSwitchTimer( void )
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
        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "All Mts has be switched over.\n");
        
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
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "Notify restart auto poll swtich to mmcu, bOn.%d, lelev.%d, time.%d, lid.%d\n" ,
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
        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Auto Switched to Setout Mt.%d to poll.\n", byMtId );
        
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
    if ( byMtId > MAXNUM_CONF_MT || NULL == m_ptMtTable)
    {
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MMCU, "[OnNtfMtStatusToMMcu] Mt.%d is illeagal, ignore it!\n", byMtId);
        return;
    }
	// mcu�ն�ʵʱ�ϱ�
	if ( byMtId != 0 
		&&  (/*m_ptMtTable && */!IsMcu(m_ptMtTable->GetMt(byMtId))))
	{
		if(byDstMcuId == m_tCascadeMMCU.GetMtId())
		{
			// �����˻��巢��, ���û��巢�ͻ���, ����ֱ�ӷ���
			if(g_cMcuVcApp.GetMcuMcuMtStatusBufFlag() != 0)
			{
				// [11/14/2011 liuxu] ���巢��
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Buf snd localMt.%d Status To MMcu\n", byMtId);
				m_cLocalMtStatusToMMcuBuf.Add(byMtId);
				if (m_byMtStatusToMMcuTimerFlag)
				{
					return;
				}else
				{
					SetTimer(MCUVC_SEND_SMCUMTSTATUS_TIMER, g_cMcuVcApp.GetRefreshSMcuMtInterval());
					m_byMtStatusToMMcuTimerFlag = 1;
					return;
				}
			}
		}
	}

	CServMsg cServMsg;
	CServMsg cMsg;
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
			//zhouyiliang 20101224 vcs����Ҳ�����ϼ�mcu�����ϱ�
			if( byDstMcuId != byMtLoop &&
					( m_tConfAllMtInfo.MtJoinedConf( byMtLoop ) ||
						(VCS_CONF == m_tConf.GetConfSource() && m_tConfAllMtInfo.MtInConf(byMtLoop)) )//CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtLoop)))
				)
			{		
				m_ptMtTable->GetMtStatus( byMtLoop, &tLocalMtStatus );
				memcpy( (void*)&tSMcuMtStatus, (void*)&tLocalMtStatus, sizeof(TMt) );
				tSMcuMtStatus.SetIsEnableFECC( tLocalMtStatus.IsEnableFECC() );
				tSMcuMtStatus.SetCurVideo( tLocalMtStatus.GetCurVideo() );
				tSMcuMtStatus.SetCurAudio( tLocalMtStatus.GetCurAudio() );
				tSMcuMtStatus.SetMtBoardType( tLocalMtStatus.GetMtBoardType() );
                tSMcuMtStatus.SetIsMixing( tLocalMtStatus.IsInMixing() );
                tSMcuMtStatus.SetVideoLose( tLocalMtStatus.IsVideoLose() );
				BOOL32 bAutoMode = (CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtLoop)) ? TRUE : FALSE;
				tSMcuMtStatus.SetIsAutoCallMode(bAutoMode);
				tSMcuMtStatus.SetRecvVideo( tLocalMtStatus.IsReceiveVideo() );
				tSMcuMtStatus.SetRecvAudio( tLocalMtStatus.IsReceiveAudio() );
				tSMcuMtStatus.SetSendVideo( tLocalMtStatus.IsSendVideo() );
				tSMcuMtStatus.SetSendAudio( tLocalMtStatus.IsSendAudio() );

				//zjj20100327
				tSMcuMtStatus.SetDisconnectReason( tLocalMtStatus.GetMtDisconnectReason() );
				tSMcuMtStatus.SetSendVideo2(tLocalMtStatus.IsSndVideo2());
				tSMcuMtStatus.SetMcuIdx( LOCAL_MCUID );

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
			BOOL32 bAutoMode = (CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtId)) ? TRUE : FALSE;
			tSMcuMtStatus.SetIsAutoCallMode(bAutoMode);
			tSMcuMtStatus.SetRecvVideo( tLocalMtStatus.IsReceiveVideo() );
			tSMcuMtStatus.SetRecvAudio( tLocalMtStatus.IsReceiveAudio() );
			tSMcuMtStatus.SetSendVideo( tLocalMtStatus.IsSendVideo() );
			tSMcuMtStatus.SetSendAudio( tLocalMtStatus.IsSendAudio() );

			//zjj20100327
			tSMcuMtStatus.SetDisconnectReason( tLocalMtStatus.GetMtDisconnectReason() );
			tSMcuMtStatus.SetSendVideo2(tLocalMtStatus.IsSndVideo2());
			tSMcuMtStatus.SetMcuIdx( LOCAL_MCUID );
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
		cMsg.SetDstMtId( byDstMcuId );
		cMsg.SetEventId( MCU_MCU_MTSTATUS_NOTIF );
		cMsg.SetMsgBody( (u8*)&byMtNum,sizeof(u8) );
		cMsg.CatMsgBody( cServMsg.GetMsgBody(),cServMsg.GetMsgBodyLen() );		
		SendMsgToMt( byDstMcuId, MCU_MCU_MTSTATUS_NOTIF, cMsg );
	}

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
    2011/11/18  4.6         lixuu				  ����
=============================================================================*/
void CMcuVcInst::OnBufSendMtStatusToMMcu( )
{
	if (CurState() != STATE_ONGOING)
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "OnBufSendMtStatusToMMcu called at wrong Vc state.%d, return\n", CurState());
		return;
	}
	
	m_byMtStatusToMMcuTimerFlag = 0;				// ����״̬
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "start buf snd MtStatus To MMcu\n");

	// û���ϼ�, ��ղ�����
	if (m_tCascadeMMCU.IsNull())
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "MMcu is null\n");
		m_cLocalMtStatusToMMcuBuf.ClearAll();
		m_cSMcuMtStatusToMMcuBuf.ClearAll();
		return;
	}

	// ��ȡ�����ն���Ŀ
	const u8 bySMcuMtNum = (u8)m_cLocalMtStatusToMMcuBuf.GetUsedNum();
	
	// �ȷ��ͱ����ն�״̬
	if ( bySMcuMtNum )
	{
		CServMsg cMsg;
		cMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
		cMsg.SetEventId( MCU_MCU_MTSTATUS_NOTIF );
		cMsg.SetMsgBody( (u8*)&bySMcuMtNum,sizeof(u8) );
		
		const u32 dwMaxBufNum = m_cLocalMtStatusToMMcuBuf.GetCapacity();
		u8 byMtId;
		TMtStatus tLocalMtStatus;
		TSMcuMtStatus tSMcuMtStatus;

		u32 dwActSendNum = 0;
		
		for ( u32 dwLoop = 0; dwLoop < dwMaxBufNum; dwLoop++ )
		{
			byMtId = 0;
			if(!m_cLocalMtStatusToMMcuBuf.Get(dwLoop, byMtId))
				continue;
			
			if ( !IsValidMtId(byMtId)) 
			{
				continue;
			}
			
			if( m_tConfAllMtInfo.MtInConf(byMtId) && m_tCascadeMMCU.GetMtId() != byMtId )
			{		
				m_ptMtTable->GetMtStatus( byMtId, &tLocalMtStatus );
				memcpy( (void*)&tSMcuMtStatus, (void*)&tLocalMtStatus, sizeof(TMt) );
				tSMcuMtStatus.SetIsEnableFECC( tLocalMtStatus.IsEnableFECC() );
				tSMcuMtStatus.SetCurVideo( tLocalMtStatus.GetCurVideo() );
				tSMcuMtStatus.SetCurAudio( tLocalMtStatus.GetCurAudio() );
				tSMcuMtStatus.SetMtBoardType( tLocalMtStatus.GetMtBoardType() );
				tSMcuMtStatus.SetIsMixing( tLocalMtStatus.IsInMixing() );
				tSMcuMtStatus.SetVideoLose( tLocalMtStatus.IsVideoLose() );
				BOOL32 bAutoMode = ( CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtId) ) ? TRUE : FALSE;
				tSMcuMtStatus.SetIsAutoCallMode(bAutoMode);
				tSMcuMtStatus.SetRecvVideo( tLocalMtStatus.IsReceiveVideo() );
				tSMcuMtStatus.SetRecvAudio( tLocalMtStatus.IsReceiveAudio() );
				tSMcuMtStatus.SetSendVideo( tLocalMtStatus.IsSendVideo() );
				tSMcuMtStatus.SetSendAudio( tLocalMtStatus.IsSendAudio() );
				
				//zjj20100327
				tSMcuMtStatus.SetDisconnectReason( tLocalMtStatus.GetMtDisconnectReason() );
				tSMcuMtStatus.SetSendVideo2(tLocalMtStatus.IsSndVideo2());
				tSMcuMtStatus.SetMcuIdx( LOCAL_MCUID );
				
				dwActSendNum++;
				cMsg.CatMsgBody( (u8*)&tSMcuMtStatus, sizeof(TSMcuMtStatus) );
			}
		}
		
		m_cLocalMtStatusToMMcuBuf.ClearAll();
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "send %u localmt status to mmcu\n", dwActSendNum);
		SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_MTSTATUS_NOTIF, cMsg );
	}

	// �ٷ����¼��ն�״̬
	const u16 wSMcuNum = (u16)m_cSMcuMtStatusToMMcuBuf.GetUsedNum();
	if (wSMcuNum)
	{
		const u32 dwMaxBufNum = m_cLocalMtStatusToMMcuBuf.GetCapacity();
		u16 wMcuIdx = INVALID_MCUIDX;
		u32 dwActSendNum = 0;
		for ( u32 dwSMcuLoop = 0; dwSMcuLoop < dwMaxBufNum; dwSMcuLoop++ )
		{
			wMcuIdx = INVALID_MCUIDX;
			if(!m_cSMcuMtStatusToMMcuBuf.Get(dwSMcuLoop, wMcuIdx))
				continue;
			
			if ( !IsValidMcuId(wMcuIdx)) 
			{
				continue;
			}

			SendSMcuMtStatusToMMcu(wMcuIdx);
			dwActSendNum++;
		}

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "send %u smcu to mmcu\n", dwActSendNum);
	}
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
	10/12/30    4.6         liuxu          �޸�
====================================================================*/
void CMcuVcInst::BroadcastToAllMcu( u16 wEvent, CServMsg & cServMsg )
{
	u16 wMcuIdx = 0;
	u16 wNum = 0;
	u16 wCount = m_tConfAllMcuInfo.GetMcuCount();
	u8 abyMcuId[MAX_CASCADEDEPTH-1];	
	
	for( u16 wLoop = 0; wLoop < m_tConfAllMtInfo.GetMaxMcuNum(); ++wLoop )
	{
		if( wNum >= wCount )
		{
			break;
		}

		wMcuIdx = m_tConfAllMtInfo.GetMtInfo(wLoop).GetMcuIdx();
		if( m_tConfAllMtInfo.GetMtInfo(wLoop).IsNull() 
			|| !IsValidMcuId(wMcuIdx))
		{
			continue;
		}

		memset( &abyMcuId[0],0,sizeof(abyMcuId) );
		if( !m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] ) )
		{			
			continue;
		}	
		
		if( 0 == abyMcuId[0] )
		{
			continue;
		}

		if( abyMcuId[1] != 0 )
		{
			++wNum;
			continue;
		}
		
		SendMsgToMt( abyMcuId[0], wEvent, cServMsg );		
		++wNum;
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
	tMt.SetMcuId( LOCAL_MCUID );
	tInfo.m_tMt = tMt;


	TMtAlias tTmpMtAlias;
	m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeE164, &tTmpMtAlias);
	
	TMt tLocalMt = m_ptMtTable->GetMt(byMtId);

	// ����ǵ绰�նˣ�����ȡE164��
	if (IsPhoneMt(tLocalMt)&&!tTmpMtAlias.IsAliasNull())
	{
		strncpy((s8 *)(tInfo.m_szMtName), tTmpMtAlias.m_achAlias, sizeof(tInfo.m_szMtName));
	}
	else if ( m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH323ID, &tTmpMtAlias))
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcuMcuMtListReq]copy 323ID to Mt.%d\n", tMt.GetMtId());
		strncpy((s8 *)(tInfo.m_szMtName), tTmpMtAlias.m_achAlias, sizeof(tInfo.m_szMtName) - 1);
	}
	else
	{
		strncpy((s8 *)(tInfo.m_szMtName), m_ptMtTable->GetMtAliasFromExt(tMt.GetMtId()), sizeof(tInfo.m_szMtName));
	}

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
	tInfo.m_byIsConnected		= GETBBYTE(m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()));
	tInfo.m_byIsFECCEnable		= GETBBYTE(m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_H224DATA, &tChannel, TRUE )); 
	
	tInfo.m_tPartVideoInfo.m_nViewCount = 0;
	if(tMt == m_tConf.GetSpeaker())
	{
		tInfo.m_tPartVideoInfo.m_atViewPos[tInfo.m_tPartVideoInfo.m_nViewCount].m_nViewID = m_dwSpeakerViewId;
		tInfo.m_tPartVideoInfo.m_atViewPos[tInfo.m_tPartVideoInfo.m_nViewCount].m_bySubframeIndex = 0;
		tInfo.m_tPartVideoInfo.m_nViewCount++;
	}
	TVMPParam tVmpParam = m_tConf.GetStatus().GetVmpParam();
	if(tVmpParam.IsMtInMember(tMt))
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

    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[NotifyMcuNewMt] MtId.%d szMtName.%s \n", tInfo.m_tMt.GetMtId(), tInfo.m_szMtName);

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
	TMsgHeadMsg tMsgHeadMsg;


	tMt.SetMcuId( LOCAL_MCUID );
	//tMsgHeadMsg.m_tMsgSrc.m_tMt = tMt;	
	cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
	cServMsg.CatMsgBody( (u8*)&tMsgHeadMsg,sizeof(TMsgHeadMsg) );	
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
	TMsgHeadMsg tMsgHeadMsg;


	tMt.SetMcuId( LOCAL_MCUID );
	//tMsgHeadMsg.m_tMsgSrc.m_tMt = tMt;	
	
	cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
	cServMsg.CatMsgBody( (u8*)&tMsgHeadMsg,sizeof(TMsgHeadMsg) );	
	BroadcastToAllMcu( MCU_MCU_DELMT_NOTIF, cServMsg );
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
    ������      : GetMcuMcInfo
    ����        : ��ȡmcu��Mc��Ϣ
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: [in] tMcu, Ҫ��ȡ��mcu�ն�(������mcu�ն�, ���򷵻�NULL)
    ����ֵ˵��  : �ɹ�����mc��Ϣ, ʧ�ܷ���NULL
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    11/10/18    4.6         ����          ����
====================================================================*/
TConfMcInfo* CMcuVcInst::GetMcuMcInfo( const TMt& tMcu)
{
	if (!m_ptConfOtherMcTable)
	{
		return NULL;
	}	

	// ������mcu, ����
	if (!IsMcu(tMcu))
	{
		return NULL;
	}
	
	// ��ȡ��mcu��Ϣ
	TConfMcInfo *ptConfOtherMcInfo = NULL;
	
	if ( !tMcu.IsLocal() )
	{
		u8 abyMcuId[MAX_CASCADEDEPTH-1] = { 0 };
		abyMcuId[0] = GetFstMcuIdFromMcuIdx(tMcu.GetMcuIdx());
		abyMcuId[1] = tMcu.GetMtId();
		u16 wMcuIdx = INVALID_MCUIDX;
		
		if( !m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0], 2, &wMcuIdx ))
		{
			return NULL;
		}

		ptConfOtherMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );
	}else
	{
		ptConfOtherMcInfo = m_ptConfOtherMcTable->GetMcInfo( GetMcuIdxFromMcuId(tMcu.GetMtId()) );
	}
	
	return ptConfOtherMcInfo;
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
	TMt tSpeaker = m_tConf.GetSpeaker();

	if( tSpeaker.IsNull() )								// NULL,����NULL
	{
		return tSpeaker;
	}
	else if( tSpeaker.IsLocal() )						// �������Ǳ���MT�򱾼���mcu
	{
		return tSpeaker;
	}
	else												// ���������¼�MT���¼�mcu
	{		
		return m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx( tSpeaker.GetMcuIdx() ) );
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
TMt  CMcuVcInst::GetMcuMediaSrc(u16 wMcuIdx)
{
	TMt tMt;
	tMt.SetMcuIdx(wMcuIdx);
	tMt.SetMtId(0);

	//u16  wMcuIdx = GetMcuIdxFromMcuId( byMcuId );
	

	//m_tConfAllMcuInfo.GetIdxByMcuId( byMcuId,0,&wMcuIdx );	
	TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
	if(ptInfo == NULL)
	{
		return tMt;
	}

	return ptInfo->m_tMMcuViewMt;
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
	10/08/12	4.6			xl				multi-cascade support and codes improve
====================================================================*/
void CMcuVcInst::ProcMcuMcuSendMsgReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );


	// ������¼�MCU���ϼ�����, �����, zgc, 2007-04-07
	TMt tSrcMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	if( tSrcMt.GetMtType() == MT_TYPE_SMCU )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuSendMsgReq]This is unallowed that transmit the short msgs to mmcu from smcu!\n");
		return;
	}

	TMt tMt = *(TMt *)( cServMsg.GetMsgBody() );
	CRollMsg* ptROLLMSG = (CRollMsg*)( cServMsg.GetMsgBody() + sizeof(TMt));

	
// 	TMsgHeadMsg tHeadMsg;
// 	if( cServMsg.GetMsgBodyLen > (sizeof(TMt) + ptROLLMSG->GetTotalMsgLen()) )
// 	{
// 		tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody() + sizeof(TMt) + ptROLLMSG->GetTotalMsgLen() );
// 	}

	switch( CurState() )
	{
	case STATE_ONGOING:

		if( tMt.IsNull() )	//���������ն� MCU_MCU_SENDMSG_NOTIF
		{
			for(u8 byLoop = 1; byLoop<=MAXNUM_CONF_MT; byLoop++)
			{
				if( m_tConfAllMtInfo.MtJoinedConf(byLoop) && byLoop != cServMsg.GetSrcMtId() )
                {
                    if (m_ptMtTable->GetMtType(byLoop) == MT_TYPE_MT)
                    {
						cServMsg.SetMsgBody( cServMsg.GetMsgBody() , (sizeof(TMt) + ptROLLMSG->GetTotalMsgLen()));
                        SendMsgToMt( byLoop, MCU_MT_SENDMSG_NOTIF, cServMsg );	
                    }
                    else if(m_ptMtTable->GetMtType(byLoop) == MT_TYPE_SMCU)// xsl [9/26/2006] ֧�ֶ༶����Ϣ�㲥, zgc [04/03/2007] �����ϼ�MCU����
                    {
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuSendMsgReq]-send to Mt.%u(type:SMCU) \n", byLoop);
						TMsgHeadMsg tHeadMsg;
						CServMsg cSendMsg;
						cSendMsg.SetMsgBody( (u8*)&tHeadMsg, sizeof(TMsgHeadMsg) );
						cSendMsg.CatMsgBody( (u8*)&tMt, sizeof(TMt) );
						cSendMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );
                        SendMsgToMt( byLoop, MCU_MCU_SENDMSG_NOTIF, cSendMsg );
                    }
                }			
			}
		}
		else //����ĳһ�ն� 
		{
			ProcSingleMtSmsOpr(cServMsg, &tMt, ptROLLMSG);
// 			if( !tMt.IsMcuIdLocal() )
// 			{
// 				SendMsgToMt( (u8)tMt.GetMcuId(), MCU_MCU_SENDMSG_NOTIF, cServMsg );
// 			}
// 			else
// 			{
// 				if(tMt.GetMtId() != cServMsg.GetSrcMtId())
// 				{
// 					if ( m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MT )
// 					{
// 						cServMsg.SetMsgBody( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg),cServMsg.GetMsgBodyLen() - sizeof(TMsgHeadMsg));
// 						SendMsgToMt( tMt.GetMtId(), MCU_MT_SENDMSG_NOTIF, cServMsg );
// 					}
// 					else if (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU)// xsl [9/26/2006] ֧�ֶ༶����Ϣ�㲥, zgc [04/03/2007] �����ϼ�MCU����
// 					{
// 						CServMsg cTmpMsg(pcMsg->content, pcMsg->length);
// 						TMt tNullMt;
// 						tNullMt.SetNull();
// 						cTmpMsg.SetMsgBody((u8*)&tNullMt, sizeof(tNullMt));
// 						cTmpMsg.CatMsgBody((cServMsg.GetMsgBody() + sizeof(TMt)), cServMsg.GetMsgBodyLen()-sizeof(TMt));
// 						SendMsgToMt( tMt.GetMtId(), MCU_MCU_SENDMSG_NOTIF, cTmpMsg );
// 					}
// 				}
// 			}
		}
		break;

	default:
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "Wrong message %u(%s) received in state %u!\n", 
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
void CMcuVcInst::OnMMcuSetIn(TMt tMt, u8 byMcsSsnId, u8 bySwitchMode)
{
	TSetInParam tSetInParam;
	tSetInParam.m_nViewId = -1145368303;
	tSetInParam.m_bySubFrameIndex = 0;
	tSetInParam.m_tMt = tMt;
	
	CServMsg  cMsg;
	TMsgHeadMsg tHeadMsg;
	TMcuMcuReq tReq;
	TMcsRegInfo	tMcsReg;
	TMtAlias tMtAlias;
	g_cMcuVcApp.GetMcsRegInfo( byMcsSsnId, &tMcsReg );
	astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
		sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
	astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
		sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));


	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tSetInParam.m_tMt );
	/*tHeadMsg.m_tMsgDst.m_tMt = tMt;
	u8 byFstMcuId = 0,bySecMcuId = 0;
	u8 abyMcuId[MAX_CASCADEDEPTH-1];
	memset( &abyMcuId[0],0,sizeof(abyMcuId) );
	if( !m_tConfAllMcuInfo.GetMcuIdByIdx( tMt.GetMcuIdx(),&abyMcuId[0] ) )
	{
		ConfLog( FALSE,"[OnMMcuSetIn] Fail to Get Mcuid.McuIdx.%d\n",tMt.GetMcuIdx() );
		return;
	}
	if( 0 != bySecMcuId )	
	{
		tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( byFstMcuId );
		tHeadMsg.m_tMsgDst.m_tMt.SetMtId( bySecMcuId );
		tHeadMsg.m_tMsgDst.m_abyMtIdentify[0] = tMt.GetMtId();
		tHeadMsg.m_tMsgDst.m_byCasLevel = 1;
	}
	else
	{
		tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( byFstMcuId );
		tHeadMsg.m_tMsgDst.m_byCasLevel = 0;
	}*/
	
	
	cMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));					
	cMsg.CatMsgBody((u8 *)&tSetInParam, sizeof(tSetInParam));
	cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );

	u8 byFstMcuId = GetFstMcuIdFromMcuIdx( tMt.GetMcuIdx() );
	if( m_ptMtTable->GetDialAlias( byFstMcuId/*tMt.GetMcuId()*/, &tMtAlias ) && 
		mtAliasTypeH320ID == tMtAlias.m_AliasType )
	{
		//���ܳ������У���֮ǰ�ǹ㲥Դ���㲥Դ�������ͬʱ����ѡ��������״̬��������
		//֪ͨH320���뽻����ʽ
		cMsg.CatMsgBody((u8 *)&bySwitchMode, sizeof(bySwitchMode));
	}	

	SendMsgToMt( byFstMcuId, MCU_MCU_SETIN_REQ, cMsg);
    
	TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMcuIdx());//tMt.GetMcuId());
	if (ptMcInfo != NULL)
	{
		// miaoqingsong [20111017] ����VCS���黹��MCS����һ��Ҫ�ϸ�֤m_tLastMMcuViewMt��m_tMMcuViewMt��ͬ
		if( !(ptMcInfo->m_tMMcuViewMt == tMt) )
		{
			ptMcInfo->m_tLastMMcuViewMt = ptMcInfo->m_tMMcuViewMt;
			ptMcInfo->m_tMMcuViewMt = tMt;
		}

		//zjj20091218 vcs�����ϸ�֤m_tLastMMcuViewMt��m_tMMcuViewMt��ͬ
// 		if( VCS_CONF == m_tConf.GetConfSource() )
// 		{
// 			if( !(ptMcInfo->m_tMMcuViewMt == tMt) )
// 			{
// 				ptMcInfo->m_tLastMMcuViewMt = ptMcInfo->m_tMMcuViewMt;
// 				ptMcInfo->m_tMMcuViewMt = tMt;
// 			}
// 		}
// 		else
// 		{
// 			ptMcInfo->m_tLastMMcuViewMt = ptMcInfo->m_tMMcuViewMt;
// 			ptMcInfo->m_tMMcuViewMt = tMt;
// 		}	
	}

	//zhouyiliang 20101026 �༶���ش�������м��mcu��viewmtҲҪ��
	//Ŀǰֻ������3���������3������Ҫ��for����whileѭ��
	if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx( tMt.GetMcuIdx() ) )
	{
		u8 byMcuId[MAX_CASCADEDEPTH-1];
		u16 wMcuIdx = GetMcuIdxFromMcuId( byFstMcuId );
		ptMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );
		if( m_tConfAllMcuInfo.GetMcuIdByIdx( tMt.GetMcuIdx(),&byMcuId[0] ) &&
			0 != byMcuId[1]
			)
		{
			if (ptMcInfo != NULL)
			{
				// [10/19/2011 liuxu] ��һ���ϴ��ն�Ҳ��Ҫ����, ��Ȼû�л�������
				ptMcInfo->m_tMMcuViewMt.SetConfIdx(m_byConfIdx);
				ptMcInfo->m_tLastMMcuViewMt = ptMcInfo->m_tMMcuViewMt;
		
				ptMcInfo->m_tMMcuViewMt.SetMcuId( wMcuIdx );
				ptMcInfo->m_tMMcuViewMt.SetMtId( byMcuId[1] );

				// [10/19/2011 liuxu] ��ʹ�޵�����, �����ﾹȻ��mcu���ϴ��ն˸ı���
				// ������ChangeHduSwitch/ChangeTvwallSwitch/PrecxxxSetoutNotify�д�mcu��
				// ��һ���ϴ�Դ�ĵ���ǽ��־������ȷ��ˢ��. ���������һ������
				//RefreshMtStatusInTw(ptMcInfo->m_tLastMMcuViewMt, FALSE, TRUE);
				//RefreshMtStatusInTw(ptMcInfo->m_tLastMMcuViewMt, FALSE, FALSE);
			}
		}
	}

    // guzh [8/31/2006] �ж��¼��ն��ڱ������Ե����
    // guzh [9/1/2006]  Pollingʱ���л������ˣ��л������˻ᵼ����ѯֹͣ��������Ȼ���л�
	//zhouyiliang 20120720 ��setoutnotify�л������˴�������ǰ��
//     TMt tSpeaker = m_tConf.GetSpeaker();
// 	TMt tMcu = GetLocalMtFromOtherMcuMt(tMt);
//     if ( tSpeaker.GetType() == TYPE_MT &&
//          (tMt.GetMcuIdx() != tSpeaker.GetMcuIdx() ||
//          tMt.GetMtId() != tSpeaker.GetMtId()) &&
// 		 IsMtInMcu(tMcu, tSpeaker) &&
//          !bPolling )
//     {
//         ChangeSpeaker(&tMt);
//     }    
	
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
	if(ptMcu == NULL || ptMcu->IsLocal())
	{
		return;
	}
	//TMt tMcuMt = m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx(ptMcu->GetMcuId()) );
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(ptMcu->GetMcuId()));
	//TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());
	if(ptMcInfo == NULL)
	{
		return;
	}
	u16 wMMcuIdx = INVALID_MCUIDX;
	if( !m_tCascadeMMCU.IsNull() )
	{
		wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
	}
	
	if( ( INVALID_MCUIDX == ptMcInfo->GetMcuIdx() ) || 
		( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->GetMcuIdx() == wMMcuIdx ) )
	{
		return;
	}
	
	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_STARTMIXER_CMD);
	cMsg.SetDstMtId(ptMcu->GetMcuId() );
	
	SendMsgToMt(ptMcu->GetMcuId(), MCU_MCU_STARTMIXER_CMD, cMsg);

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
	if(ptMcu == NULL || ptMcu->IsMcuIdLocal())
	{
		return;
	}

	u16 wMcuIdx = GetMcuIdxFromMcuId( (u8)ptMcu->GetMcuId() );
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
	if(ptMcInfo == NULL)
	{
		return;
	}
	u16 wMMcuIdx = INVALID_MCUIDX;
	if( !m_tCascadeMMCU.IsNull() )
	{
		wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
	}

	if( ( INVALID_MCUIDX == ptMcInfo->GetMcuIdx() ) || 
		( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->GetMcuIdx() == wMMcuIdx ) )
	{
		return;
	}
	TMt tMt = m_ptMtTable->GetMt(u8(ptMcu->GetMcuId()));
	RemoveSpecMixMember(&tMt, 1, FALSE, FALSE);
	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_STOPMIXER_CMD);
	cMsg.SetDstMtId( (u8)ptMcu->GetMcuId() );
	
	SendMsgToMt( (u8)ptMcu->GetMcuId(), MCU_MCU_STOPMIXER_CMD, cMsg);

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
	
	//u16 wMMcuIdx = INVALID_MCUIDX;
	if( !m_tCascadeMMCU.IsNull() )
	{
		//wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
	}

	//TConfMcInfo* ptMcInfo = NULL;
	for( u8 byLoop = 0; byLoop < MAXNUM_CONF_MT; byLoop++ )
	{
		if( MT_TYPE_SMCU != m_ptMtTable->GetMtType(byLoop) )
		{
			continue;
		}
		//ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
		if( !m_tCascadeMMCU.IsNull() &&  byLoop == m_tCascadeMMCU.GetMtId() ) 
		{
			continue;
		}
		cMsg.SetDstMtId( byLoop );
		SendMsgToMt(byLoop, MCU_MCU_STARTMIXER_CMD, cMsg);
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
	//u16 wMMcuIdx = INVALID_MCUIDX;
	if( !m_tCascadeMMCU.IsNull() )
	{
		//wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
	}
	
	/*TConfMcInfo* ptMcInfo = NULL;
	for( u8 byLoop = 1; byLoop < MAXNUM_SUB_MCU; byLoop++ )
	{
		ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
		if( ( INVALID_MCUIDX == ptMcInfo->GetMcuIdx() ) || 
			( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->GetMcuIdx() == wMMcuIdx ) )
		{
			continue;
		}
		cMsg.SetDstMtId( GetFstMcuIdFromMcuIdx(ptMcInfo->GetMcuIdx()) );
		SendMsgToMt( cMsg.GetDstMtId(), MCU_MCU_STOPMIXER_CMD, cMsg);
	}*/
	//TConfMcInfo* ptMcInfo = NULL;
	for( u8 byLoop = 0; byLoop < MAXNUM_CONF_MT; byLoop++ )
	{
		if( MT_TYPE_SMCU != m_ptMtTable->GetMtType(byLoop) )
		{
			continue;
		}
		//ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
		if( !m_tCascadeMMCU.IsNull() &&  byLoop == m_tCascadeMMCU.GetMtId() ) 
		{
			continue;
		}
		cMsg.SetDstMtId( byLoop );
		SendMsgToMt(byLoop, MCU_MCU_STOPMIXER_CMD, cMsg);
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
	if(ptMcu == NULL ||ptMcu->IsLocal())
	{
		return;
	}

	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_GETMIXERPARAM_REQ);
	
	if( INVALID_MCUIDX == ptMcu->GetMcuIdx() )
	{
		//TConfMcInfo* ptMcInfo = NULL;
		for( u8 byLoop = 1; byLoop < MAXNUM_CONF_MT; byLoop++ )
		{
			if( MT_TYPE_SMCU != m_ptMtTable->GetMtType(byLoop) )
			{
				continue;
			}
			//ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
			if( !m_tCascadeMMCU.IsNull() &&  byLoop == m_tCascadeMMCU.GetMtId()  )
			{
				continue;
			}
			cMsg.SetDstMtId( byLoop );
			SendMsgToMt( byLoop, MCU_MCU_GETMIXERPARAM_REQ, cMsg );
		}
	}
	else
	{
		cMsg.SetDstMtId( GetFstMcuIdFromMcuIdx(ptMcu->GetMcuIdx()) );
		SendMsgToMt( cMsg.GetDstMtId(), MCU_MCU_GETMIXERPARAM_REQ, cMsg);
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
void CMcuVcInst::OnAddRemoveMixToSMcu(CServMsg *pcSerMsg, BOOL32 bAdd, BOOL32 bStopMixerNoMember/* = TRUE */)
{
	TMcu *ptMcu = (TMcu *)(pcSerMsg->GetMsgBody());
	if(ptMcu == NULL)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu] ptMcu == NULL! So Return!\n");
		return;
	}
	if(ptMcu->IsLocal())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu] The MCU is local! So Return! bStopMixerNoMember = %d\n",bStopMixerNoMember);
		return;
	}
    
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	memset(&abyMcuId[0], 0, sizeof(abyMcuId));

	if( !m_tConfAllMcuInfo.GetMcuIdByIdx(ptMcu->GetMcuIdx(), &abyMcuId[0]) &&
		!m_tConfAllMcuInfo.IsSMcuByMcuIdx(ptMcu->GetMcuIdx())
		)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER,  "[OnAddRemoveMixToSMcu] Fail to Get McuId By McuIdx.%d!\n",
			ptMcu->GetMcuIdx());
		return;
	}

	TMt tMcuMt = m_ptMtTable->GetMt(abyMcuId[0]);
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuIdx());
	if(ptMcInfo == NULL)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu] ptMcInfo == NULL so return!\n");
		return;
	}

	if( (INVALID_MCUIDX == ptMcInfo->GetMcuIdx()) || 
		(!m_tCascadeMMCU.IsNull() && abyMcuId[0] == m_tCascadeMMCU.GetMtId() ) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu] INVALID_MCUIDX == ptMcInfo->GetMcuIdx() so return!\n");
		return;
	}

	TMt *ptMt = (TMt *)(pcSerMsg->GetMsgBody() + sizeof(TMcu));
	TMt tTempMt;	
	CServMsg cMsg;
	
	if (ptMt == NULL)
	{
		ConfPrint( LOG_LVL_ERROR, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu] The add/remove ptMt is NULL, so return!\n" );
		return;
	}

	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	u8  byMtNum = (pcSerMsg->GetMsgBodyLen() - sizeof(TMcu) - sizeof(u8))/sizeof(TMt);

	//u8  byReplace = 0;
	u8  byNotForceRemove = 0;
	if (bAdd)
	{
		cMsg.SetMsgBody((u8*)&tTempMt, sizeof(TMcu));
		//byReplace = *(u8*)(pcSerMsg->GetMsgBody() + sizeof(TMt) * byMtNum + sizeof(TMcu));

		u8  bySecMcuLocalMtNum = 0;     //�ڶ����ն���
		TMt atSecMcuLocalMt[MAXNUM_MIXING_MEMBER];
		u8  byThdMcuNum = 0;
		TMt atThdMcuMt[MAXNUM_SUB_MCU];
		u8  abyThdMcuMtNum[MAXNUM_SUB_MCU] = { 0 };    //������MCU����
		TMultiCacMtInfo tMultiCacMtInfo[MAXNUM_SUB_MCU][MAXNUM_MIXING_MEMBER];
		TMultiCacMtInfo tTempMultiCacMtInfo;

		for ( u8 byIndex = 0; byIndex < byMtNum; byIndex ++ )
		{
			if (ptMt == NULL)
			{
				return;
			}

			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu]Add tMt[%d][%d]\n", 
					ptMt->GetMcuId(), ptMt->GetMtId() );
			tTempMultiCacMtInfo = BuildMultiCascadeMtInfo(*ptMt, tTempMt);
			if ( tTempMultiCacMtInfo.m_byCasLevel == 0 )
			{
				for ( u8 byIdx1 = 0; byIdx1 < MAXNUM_MIXING_MEMBER; byIdx1++ )
				{
					if (atSecMcuLocalMt[byIdx1].IsNull())
					{
						atSecMcuLocalMt[byIdx1] = tTempMt;
						bySecMcuLocalMtNum++;
						break;
					}
				}
			}
			else
			{
				for ( u8 byIdx = 0; byIdx < MAXNUM_SUB_MCU; byIdx++ )
				{
					if (atThdMcuMt[byIdx] == tTempMt)
					{
						for ( u8 byIdx1 = 0; byIdx1 < MAXNUM_MIXING_MEMBER; byIdx1++ )
						{
							if (tMultiCacMtInfo[byIdx][byIdx1].IsNull())
							{
								abyThdMcuMtNum[byIdx]++;
								tMultiCacMtInfo[byIdx][byIdx1] = tTempMultiCacMtInfo;
								break;
							}
						}
						break;
					}

					if (atThdMcuMt[byIdx].IsNull())
					{
						byThdMcuNum++;
						abyThdMcuMtNum[byIdx]++;
						atThdMcuMt[byIdx] = tTempMt;
						tMultiCacMtInfo[byIdx][0] = tTempMultiCacMtInfo;
						break;
					}
				}
			}

			ptMt++;
		}

		cMsg.SetMsgBody( (u8*)&bySecMcuLocalMtNum, sizeof(u8) );
		for ( u8 byIdx = 0; byIdx < bySecMcuLocalMtNum; byIdx++ )
		{
			cMsg.CatMsgBody( (u8*)&atSecMcuLocalMt[byIdx], sizeof(TMt) );
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu]Add Idx(%d) tMt[%d][%d]\n",byIdx, 
					atSecMcuLocalMt[byIdx].GetMcuId(), atSecMcuLocalMt[byIdx].GetMtId() );
		}

		cMsg.CatMsgBody( &byThdMcuNum, sizeof(u8) );
		for ( u8 byThdIndex = 0; byThdIndex < byThdMcuNum; byThdIndex++ )
		{
			cMsg.CatMsgBody( (u8*)&atThdMcuMt[byThdIndex], sizeof(TMt) );
			cMsg.CatMsgBody( (u8*)&abyThdMcuMtNum[byThdIndex], sizeof(u8) );
			
			for ( u8 byMtIndex = 0; byMtIndex < abyThdMcuMtNum[byThdIndex]; byMtIndex++ )
			{
				cMsg.CatMsgBody( (u8*)&tMultiCacMtInfo[byThdIndex][byMtIndex], sizeof(TMultiCacMtInfo) );
			}
		}

		u8 byReplace2 = *(u8*)(pcSerMsg->GetMsgBody() + sizeof(TMt) * byMtNum + sizeof(TMcu));
		cMsg.CatMsgBody((u8*)&byReplace2, sizeof(u8));
		
		// zjj20100428 ֻ��������¼��ն˽�����ʱ�����¼�mcu���ڻ�����ʱ�Ž����������
		if ( !m_ptMtTable->IsMtInMixing(tMcuMt.GetMtId()) )
		{
			AddSpecMixMember(&tMcuMt, 1, TRUE);
		}
	}
	else
	{
		TMt tCurMt = m_cVCSConfStatus.GetCurVCMT();
		if( VCS_CONF == m_tConf.GetConfSource() && ptMt != NULL && (ptMt->GetMcuId() == tCurMt.GetMcuId() && ptMt->GetMtId() == tCurMt.GetMtId()) )
		{
			ConfPrint( LOG_LVL_ERROR, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu] TMT(%d, %d) IS CURRENT VCS MT, SO CAN'T REMOVE!!\n", 
				ptMt->GetMcuId(),ptMt->GetMtId() );

			return;
		}

		TMsgHeadMsg tMsgHeadMsg;
		tMsgHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo(*ptMt, tTempMt);
		byNotForceRemove = *(u8*)(pcSerMsg->GetMsgBody() + sizeof(TMcu) + sizeof(TMt) * byMtNum );
		cMsg.SetMsgBody((u8*)&tTempMt,sizeof(TMcu));
		if( tMsgHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu]Remove level = [%d] tMt[%d][%d]\n",
				tMsgHeadMsg.m_tMsgDst.m_byCasLevel, ptMt->GetMcuId(), ptMt->GetMtId());

			cMsg.CatMsgBody((u8*)ptMt, sizeof(TMt) * byMtNum);
		}
		else
		{
			for (u8 byIndex = 0 ;byIndex < byMtNum ;byIndex ++)
			{
				if (ptMt == NULL)
				{
					return;
				}

				BuildMultiCascadeMtInfo( *ptMt, tTempMt );
				cMsg.CatMsgBody( (u8*)&tTempMt, sizeof(TMt) );
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu]Remove level = [%d] tMt[%d][%d]\n", 
					tMsgHeadMsg.m_tMsgDst.m_byCasLevel, tTempMt.GetMcuId(), tTempMt.GetMtId() );
				ptMt++;
			}
		}
		cMsg.CatMsgBody((u8*)&byNotForceRemove, sizeof(u8));
		cMsg.CatMsgBody((u8*)&tMsgHeadMsg, sizeof(TMsgHeadMsg));
	}
	
	cMsg.SetDstMtId( tMcuMt.GetMtId() );
	if(bAdd)
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

/*==============================================================================
������    :  GetMcuMultiSpyBW
����      :  ����¼�mcu�Ļش��������ʣ�����
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-08-06                 �ܾ���							create
==============================================================================*/
BOOL32 CMcuVcInst::GetMcuMultiSpyBW( u16 wMcuIdx,u32 &dwMaxSpyBW, s32 &nRemainSpyBW )
{
	if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
	{
		u8 byMcuId = GetFstMcuIdFromMcuIdx( wMcuIdx );
		wMcuIdx = GetMcuIdxFromMcuId( &byMcuId );
	}

	return m_ptConfOtherMcTable->GetMultiSpyBW(wMcuIdx, dwMaxSpyBW, nRemainSpyBW);
}

/*==============================================================================
������    :  SetMcuSupMultSpyRemainBW
����      :  ����ĳ���¼�mcu��ʣ��ش�����
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-08-06                 �ܾ���							create
==============================================================================*/
BOOL32 CMcuVcInst::SetMcuSupMultSpyRemainBW( u16 wMcuIdx, s32 nRemainSpyBW )
{
	if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
	{
		u8 byMcuId = GetFstMcuIdFromMcuIdx( wMcuIdx );
		wMcuIdx = GetMcuIdxFromMcuId( &byMcuId );
	}

	return m_ptConfOtherMcTable->SetMcuSupMultSpyRemainBW(wMcuIdx, nRemainSpyBW );
}


/*==============================================================================
������    :  IsLocalAndSMcuSupMultSpy
����      :  �жϱ��ؼ��¼�mcu�Ƿ�֧�ֶ�ش�
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-06-07                 lukunpeng						create
2010-08-06                 �ܾ���							�޸�(�����ж��Ƿ�Ϊֱ���¼�mcu)
==============================================================================*/
BOOL32 CMcuVcInst::IsLocalAndSMcuSupMultSpy( u16 wMcuIdx )
{
	if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
	{
		u8 byMcuId = GetFstMcuIdFromMcuIdx( wMcuIdx );
		wMcuIdx = GetMcuIdxFromMcuId( &byMcuId );
	}
	// [2010/12/30 miaoqingsong] �жϻ����mcu�Ƿ�֧�ֶ�ش�
	return  IsSupportMultiSpy() && m_ptConfOtherMcTable->GetIsMcuSupMultSpy( wMcuIdx ); 
}

/*==============================================================================
������    :  IsSupportMultCas
����      :  �ж�mcu�Ƿ�֧�ֶ༶��
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-07-09                 xueliang						   create
==============================================================================*/
BOOL32 CMcuVcInst::IsSupportMultCas( u16 wMcuIdx )
{
	if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
	{
		u8 byMcuId = GetFstMcuIdFromMcuIdx( wMcuIdx );
		wMcuIdx = GetMcuIdxFromMcuId( &byMcuId );
	}	
	return m_ptConfOtherMcTable->IsMcuSupMultCas(wMcuIdx);
}

/*==============================================================================
������    :  IsSupportMultCas
����      :  �ж��ϼ�mcu�Ƿ�֧�ֶ༶��
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  �Ƿ�֧�ֶ༶��
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-08-17                 �ܾ���						   ����
==============================================================================*/
BOOL32 CMcuVcInst::IsMMcuSupportMultiCascade( void )
{
	if( m_tCascadeMMCU.IsNull() )
	{
		return FALSE;
	}
	u8 byMcuId = m_tCascadeMMCU.GetMtId();
	u16 wMcuIdx = GetMcuIdxFromMcuId( &byMcuId );
	if( INVALID_MCUIDX == wMcuIdx )
	{
		return FALSE;
	}
	return m_ptConfOtherMcTable->IsMcuSupMultCas(wMcuIdx);
}

/*==============================================================================
������    :  IsPreSetInRequired
����      :  �ж��Ƿ���ҪpreSetIn
�㷨ʵ��  :  
����˵��  :  [IN] tMt �¼�ĳ�ն�
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-02-22                 Ѧ��							  create
==============================================================================*/
BOOL32 CMcuVcInst::IsPreSetInRequired (const TMt &tMt)
{
	BOOL32 bPreSetInRqrd = TRUE;
	
	//1���и�MT�Ƿ��Ѿ���ĳ�ش�ͨ������
	if( m_cSMcuSpyMana.IsMtInSpyMember(tMt, MODE_VIDEO) )
	{
		bPreSetInRqrd = FALSE;
	}
	
	return bPreSetInRqrd;
	
}

/*==============================================================================
������    :  GetMtSimCapSetByMode
����      :  �õ������ն˵�����Ƶ����
�㷨ʵ��  :  
����˵��  :  [IN] u8 byMtId ĳ�ն�id
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
20100911                   pengjie						   create
==============================================================================*/
TSimCapSet CMcuVcInst::GetMtSimCapSetByMode( u8 byMtId )
{
	TLogicalChannel tSrcLogicChl;
	TSimCapSet tSimCapSet;
	tSimCapSet.Clear();
	
	// ��Ƶ
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_VIDEO, &tSrcLogicChl, FALSE ) )
	{
		tSimCapSet.SetVideoMediaType( tSrcLogicChl.GetChannelType() );
		tSimCapSet.SetVideoMaxBitRate( m_ptMtTable->GetSndBandWidth( byMtId) );
		tSimCapSet.SetVideoResolution( tSrcLogicChl.GetVideoFormat() );
		tSimCapSet.SetVideoProfileType(tSrcLogicChl.GetProfileAttrb() );
		tSimCapSet.SetVideoProfileType(tSrcLogicChl.GetProfileAttrb());
		if( MEDIA_TYPE_H264 == tSrcLogicChl.GetChannelType() )
		{
			tSimCapSet.SetUserDefFrameRate( tSrcLogicChl.GetChanVidFPS() );
		}
		else
		{
			tSimCapSet.SetVideoFrameRate( tSrcLogicChl.GetChanVidFPS() );
		}
	}
	
	// ��Ƶ	
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_AUDIO, &tSrcLogicChl, FALSE ) )
	{
		tSimCapSet.SetAudioMediaType( tSrcLogicChl.GetChannelType() );
	}
	
	return tSimCapSet;
}

/*==============================================================================
������    :  GetMinSpyDstCapSet
����      :  ��ǰҵ������ش���������ûش��ն�Ŀ��������ȡС
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
20101117                  �ܼ���						   create
==============================================================================*/
BOOL32 CMcuVcInst::GetMinSpyDstCapSet(const TMt &tMt , TSimCapSet &tReqDstCap )
{
	if (tMt.IsNull() || tReqDstCap.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MMCU, "[GetMinSpyDstCapSet] param is invalid!\n");
		return FALSE;
	}
	CRecvSpy tSrcSpy;
	TSimCapSet tSpyDstCap;
	if (m_cSMcuSpyMana.GetRecvSpy(tMt, tSrcSpy))
	{
		tSpyDstCap = tSrcSpy.GetSimCapset();
		if (!tSpyDstCap.IsNull() && tSpyDstCap.GetVideoMediaType() == tReqDstCap.GetVideoMediaType())
		{
			u8 byMinRes = GetMinResAcdWHProduct(tReqDstCap.GetVideoResolution(), 
												tSpyDstCap.GetVideoResolution());
			if (VIDEO_FORMAT_INVALID != byMinRes)
			{
				tReqDstCap.SetVideoResolution(byMinRes);
			}		
			
			if (tSpyDstCap.GetVideoMaxBitRate() < tReqDstCap.GetVideoMaxBitRate())
			{
				tReqDstCap.SetVideoMaxBitRate(tSpyDstCap.GetVideoMaxBitRate());
			}
		}
	}	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[GetMinSpyDstCapSet] tReqDstCap<Media:%d, Res:%d, BR:%d>!\n",
		tReqDstCap.GetVideoMediaType(), tReqDstCap.GetVideoResolution(), tReqDstCap.GetVideoMaxBitRate());
	
	return !tReqDstCap.IsNull();
}

/*==============================================================================
������    :  UpdateCurSpyDstCapSet
����      :  �����ϼ�ҵ��仯���µ�ǰ�ش��ն�Ŀ��������(�ϼ�����)
			 
�㷨ʵ��  :  ȡ����ͨ�������ͱ���ҵ����Ҫ��ش�����ȡС���棬
			 Ŀǰֻ�ж��˸��ն��Ƿ��ڻ���ϳɺ��Ƿ���dec5��
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
20101117                  �ܼ���						   create
==============================================================================*/
void CMcuVcInst::UpdateCurSpyDstCapSet(const TMt &tMt )
{
	if (tMt.IsNull() || tMt.IsLocal())
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[UpdateCurSpyDstCapSet] param is invalid!\n");
		return;
	}

	//ȡ����ͨ�������� vmp������ Dec5������ �ֱ���ȡС
	TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tMt.GetMcuId()) );
	if (tSimCapSet.IsNull())
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[UpdateCurSpyDstCapSet]get simcaptset faied!\n");
		return;
	}
	//vmp
	TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	if( (tVmpParam.IsVMPBrdst() && tVmpParam.IsMtInMember(tMt)) )
	{
		CVmpChgFormatResult cVmpChgResult;
		u8 byMtInVmpRes = 0;
		u8 byVmpSubType = GetVmpSubType(m_tVmpEqp.GetEqpId());
		
		if(byVmpSubType == VMP)
		{
			GetMtFormatInSdVmp(tMt, &tVmpParam, byMtInVmpRes, TRUE);
		}
		else
		{
			GetMtFormatInMpu(tMt, &tVmpParam, byMtInVmpRes, cVmpChgResult, TRUE, TRUE, FALSE);
		}	

		const u8 byRealRes = tSimCapSet.GetVideoResolution();
		tSimCapSet.SetVideoResolution(min(byRealRes, byMtInVmpRes));
	}
	
	//dec5
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMcuId());		
	if (ptMcInfo != NULL)
	{
		TMcMtStatus *ptMcMtStatus = ptMcInfo->GetMtStatus(tMt);
		if (ptMcMtStatus != NULL && ptMcMtStatus->GetMtStatus().IsInTvWall())
		{
			const u8 byRealRes = tSimCapSet.GetVideoResolution();
			tSimCapSet.SetVideoResolution(min(byRealRes, VIDEO_FORMAT_CIF));
		}			
	}
	m_cSMcuSpyMana.SaveSpySimCap(tMt, tSimCapSet);
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[UpdateCurSpyDstCapSet] CurrentSpyCap<Media:%d, Res:%d, BR:%d>!\n",
					tSimCapSet.GetVideoMediaType(), tSimCapSet.GetVideoResolution(), tSimCapSet.GetVideoMaxBitRate());
	return;
}
/*==============================================================================
������    :  IsCanAddSpyMtDstNum
����      :  �Ƿ�������ӻش��ն˵�Ŀ����
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-9-28                   Ѧ��							create
2009-11-01                   pengjie                         modify
==============================================================================*/
/*
BOOL32 CMcuVcInst::IsCanAddSpyMtDstNum( TMt tSrcSpyMt,TPreSetInRsp &tPreSetInRsp )
{
	
	switch( tPreSetInRsp.m_dwEvId )
	{
	case MT_MCU_STARTSELMT_CMD:			 //��׼ѡ������
	case MT_MCU_STARTSELMT_REQ:          
	case MCS_MCU_STARTSWITCHMT_REQ:	     //����̨Ҫ�󽻻�����
	case VCS_MCU_CONFSELMT_CMD:
		{
			TMtStatus tDstMtStatus;
			TMt tSrcMt;
			memset( &tDstMtStatus,0,sizeof( tDstMtStatus ) );
			if( m_ptMtTable->GetMtStatus( tPreSetInRsp.m_tDest.GetMtId(),&tDstMtStatus ) )
			{
				if( MODE_BOTH == tPreSetInRsp.m_byRspSpyMode || MODE_VIDEO == tPreSetInRsp.m_byRspSpyMode )
				{
					tSrcMt = tDstMtStatus.GetSelectMt( MODE_VIDEO );
					if( tSrcMt == tPreSetInRsp.m_tSrc )
					{
						return FALSE;
					}
				}
				if( MODE_BOTH == tPreSetInRsp.m_byRspSpyMode || MODE_AUDIO == tPreSetInRsp.m_byRspSpyMode )
				{
					tSrcMt = tDstMtStatus.GetSelectMt( MODE_AUDIO );
					if( tSrcMt == tPreSetInRsp.m_tSrc )
					{
						return FALSE;
					}
				}
			}
		}
		break;
	case MCS_MCU_STARTSWITCHMC_REQ:      //���ѡ���ն�
		
		break;
	
	case MCS_MCU_STARTVMP_REQ:
		//zjj���ﷵ��false��ͳһ��setvmpchannel�����Ӷ�ش��ն˵�����ƵĿ����
		return FALSE;
		break;

	case MCS_MCU_SPECSPEAKER_REQ:        //�������ָ̨��һ̨�ն˷���
	case MT_MCU_SPECSPEAKER_REQ:		 //��ͨ�ն�ָ������������
	case MT_MCU_SPECSPEAKER_CMD:
		{
			TMt tSpeaker = m_tConf.GetSpeaker();
			if( tSpeaker == tSrcSpyMt )
			{
				return FALSE;
			}
		}	
		//ProcSpeakerStartPreSetInAck( tPreSetInRsp );
		break;

	case MT_MCU_ADDMIXMEMBER_CMD:           //��ϯ���ӻ����ն�
	case MCS_MCU_ADDMIXMEMBER_CMD:          //���ӻ�����Ա
			
		//ProcMixerStartPreSetInAck( tPreSetInRsp );
		break;

	case MCS_MCU_START_SWITCH_HDU_REQ:     // ���ѡ����������ǽ
	case MCUVC_STARTSWITCHHDU_NOTIFY:
//	case MCS_MCU_CHANGEHDUVOLUME_REQ:
		{
			TSpyHduInfo tSpyHduInfo = tPreSetInRsp.m_tSpyInfo.m_tSpyHduInfo;
			TPeriEqpStatus tHduStatus;   
			memset( &tHduStatus,0,sizeof( tHduStatus ) );
			
			g_cMcuVcApp.GetPeriEqpStatus(tPreSetInRsp.m_tDest.GetEqpId(), &tHduStatus);

			if( tHduStatus.m_tStatus.tHdu.atVideoMt[tSpyHduInfo.m_byDstChlIdx].GetMcuId() == tSrcSpyMt.GetMcuId() &&
				tHduStatus.m_tStatus.tHdu.atVideoMt[tSpyHduInfo.m_byDstChlIdx].GetMtId() == tSrcSpyMt.GetMtId() &&
				tHduStatus.m_tStatus.tHdu.atVideoMt[tSpyHduInfo.m_byDstChlIdx].GetConfIdx() == m_byConfIdx )
			{
				return FALSE;
			}			
		}
		//ProcHduStartPreSetInAck( tPreSetInRsp );
		break;

	case MCUVC_POLLING_CHANGE_TIMER:       // ������ѯ
		//ProcPollStartPreSetInAck( tPreSetInRsp );
		break;

	case MCS_MCU_START_SWITCH_TW_REQ:      // ���ѡ�������ǽ
		{		
			TSpyTVWallInfo tSpyTVWallInfo = tPreSetInRsp.m_tSpyInfo.m_tSpyTVWallInfo;	
			TPeriEqpStatus tTvWallStatus;   
			memset( &tTvWallStatus,0,sizeof( tTvWallStatus ) );
			
			g_cMcuVcApp.GetPeriEqpStatus(tPreSetInRsp.m_tDest.GetEqpId(), &tTvWallStatus);

			if( tTvWallStatus.m_tStatus.tTvWall.atVideoMt[tSpyTVWallInfo.m_byDstChlIdx].GetMcuId() == tSrcSpyMt.GetMcuId() &&
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[tSpyTVWallInfo.m_byDstChlIdx].GetMtId() == tSrcSpyMt.GetMtId() &&
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[tSpyTVWallInfo.m_byDstChlIdx].GetConfIdx() == m_byConfIdx )
			{
				return FALSE;
			}	
		}
		//ProcTWStartPreSetInAck( tPreSetInRsp );  
		break;

    case MCS_MCU_CHANGEROLLCALL_REQ:       // ��������߼�
		{
			TMtStatus tMtStatus;
			memset( &tMtStatus,0,sizeof( tMtStatus ) );
			if(  ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() ||
				ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode()
				)
			{
				m_ptMtTable->GetMtStatus( m_tRollCaller.GetMtId(),&tMtStatus );
				TMt tSrcSelMt = tMtStatus.GetSelectMt( MODE_VIDEO );
				if( tSrcSelMt == tSrcSpyMt )
				{
					return FALSE;
				}
			}
			else if( ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
			{

			}
		}
		
		//m_tRollCaller
		//ProcRollCallPreSetInAck( tPreSetInRsp );
		break;

// 	case :
// 		break;
	default:
		break;
	}	
	return TRUE;
}

*/

/*==============================================================================
������    :  EvaluateSpyFromEvent
����      :  ���ݸ�event��SetInRsp�ж�����Դ��Ŀ�����ʹ���Ϣ�����ͷŶ��ٴ���
�㷨ʵ��  :  
����˵��  :  const TPreSetInRsp &tPreSetInRsp PreSetIn��Ӧ��Ϣ��
			 u8 &byAddDestSpyNum	�ش�Դ�����ӵ�Ŀ����
			 u32 &dwCanReleaseBW	����Ϣ���߼����ͷŴ�����
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-6-10                  lukunpeng						create
==============================================================================*/
void CMcuVcInst::EvaluateSpyFromEvent(const TPreSetInRsp &tPreSetInRsp, u8 &byAddDestSpyNum, u32 &dwCanReleaseBW, s16 &swCanRelIndex)
{
	byAddDestSpyNum = 0;
	dwCanReleaseBW = 0;
	swCanRelIndex = -1;

	TMt tCanReleaseMt;

	if( tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.GetCount() >= 0 )
	{
		TMultiCacMtInfo tMtInfo;
		tMtInfo.m_byCasLevel = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCasLevel;
		memcpy( &tMtInfo.m_abyMtIdentify[0],
			&tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_abyMtIdentify[0],
			sizeof(tMtInfo.m_abyMtIdentify)
			);
		tCanReleaseMt = GetMtFromMultiCascadeMtInfo( tMtInfo,
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt
						);
		m_cSMcuSpyMana.IsCanFree( tCanReleaseMt, 
					tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseMode,
					tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum,
					tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum,
					dwCanReleaseBW, swCanRelIndex 
					);
		if( tCanReleaseMt ==  tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() )
		{
			byAddDestSpyNum = 0;
		}
		else
		{
			byAddDestSpyNum = 1;
		}
		if( GetFstMcuIdFromMcuIdx( tCanReleaseMt.GetMcuId() ) != 
			GetFstMcuIdFromMcuIdx (tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId() )
			)
		{
			dwCanReleaseBW = 0;
		}
		
	}

	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	if( 0 == dwCanReleaseBW && m_tCascadeMMCU.IsNull() )
	{
		switch( tPreSetInRsp.m_tSetInReqInfo.GetEvId() )
		{
			//zjj20101220 ������ѯ,��ѯ�����ն���ʵҲ�ǿ��ͷ��ն�,Ӧ�ü�����ͷŴ���
			case MCUVC_POLLING_CHANGE_TIMER:
				if( tConfVmpParam.GetVMPMode() != CONF_VMPMODE_NONE )
				{
					u8 byChl = tConfVmpParam.GetChlOfMemberType( VMP_MEMBERTYPE_POLL );
					if( tConfVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL) &&
						MAXNUM_VMP_MEMBER != byChl )
					{
						TVMPMember *ptMember = tConfVmpParam.GetVmpMember( byChl );
						memcpy( &tCanReleaseMt,ptMember,sizeof(tCanReleaseMt) );
						if( GetFstMcuIdFromMcuIdx( tCanReleaseMt.GetMcuId() ) == 
								GetFstMcuIdFromMcuIdx (tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId() )
							)
						{
							m_cSMcuSpyMana.IsCanFree( tCanReleaseMt, 
										MODE_VIDEO,
										1,
										0,
										dwCanReleaseBW, swCanRelIndex 
										);

							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[EvaluateSpyFromEvent] McuId:%d MtId:%d is in vmp.Type Is VMP_MEMBERTYPE_POLL.So alse can release\n", 
									tCanReleaseMt.GetMcuId(),
									tCanReleaseMt.GetMtId()									
									);
						}				
					}
				}
				break;
			default:
				break;
		}
	}

	/*switch( tPreSetInRsp.m_tSetInReqInfo.m_dwEvId )
	{
	case MT_MCU_STARTSELMT_CMD:			 //��׼ѡ������
	case MT_MCU_STARTSELMT_REQ:          
	case MCS_MCU_STARTSWITCHMT_REQ:	     //����̨Ҫ�󽻻�����
	case VCS_MCU_CONFSELMT_CMD:
		{
			TMtStatus tDstMtStatus;
			TMt tSrcMt;
			memset(&tDstMtStatus, 0, sizeof( tDstMtStatus ));
			if( m_ptMtTable->GetMtStatus( tSpyInfo.m_tSpySwitchInfo.m_tDstMt.GetMtId(),&tDstMtStatus ) )
			{
				//�����Both����ô����Ƶѡ��Դ�ǲ��Ǳ������!�������ȣ������߼�����
				if( MODE_BOTH == tPreSetInRsp.m_byRspSpyMode || MODE_VIDEO == tPreSetInRsp.m_byRspSpyMode )
				{
					tSrcMt = tDstMtStatus.GetSelectMt( MODE_VIDEO );
					if( tSrcMt == tPreSetInRsp.m_tSetInReqInfo.m_tSrc )
					{
						byAddDestSpyNum = 0;
					}
					else
					{
						byAddDestSpyNum = 1;
						m_cSMcuSpyMana.LeftOnceToFree(tSrcMt, MODE_VIDEO, dwCanReleaseBW, swCanRelIndex);
						//ֻ��ͬһ�¼���mcu���ж�Ԥ�ͷŴ���
						if (tSrcMt.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
						{
							dwCanReleaseBW = 0;
						}
					}
				}

				if( MODE_BOTH == tPreSetInRsp.m_byRspSpyMode || MODE_AUDIO == tPreSetInRsp.m_byRspSpyMode )
				{
					tSrcMt = tDstMtStatus.GetSelectMt( MODE_AUDIO );
					if( tSrcMt == tPreSetInRsp.m_tSetInReqInfo.m_tSrc )
					{
						byAddDestSpyNum = 0;
					}
					else
					{
						byAddDestSpyNum = 1;
						m_cSMcuSpyMana.LeftOnceToFree(tSrcMt, MODE_AUDIO, dwCanReleaseBW, swCanRelIndex);
						//ֻ��ͬһ�¼���mcu���ж�Ԥ�ͷŴ���
						if (tSrcMt.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
						{
							dwCanReleaseBW = 0;
						}
					}
				}
			}
		}
		break;

	case MCS_MCU_STARTSWITCHMC_REQ:      //���ѡ���ն�
		{
			u8 byDstChnnl = tSpyInfo.m_tSpySwitchInfo.m_byDstChlIdx;
			CMcChnnlInfo cMcChnnlInfo;
			if( m_cMcChnnlMgr.GetMcChnnlInfo( byDstChnnl, cMcChnnlInfo ) )
			{
				TMt tMcSrc = cMcChnnlInfo.GetMcSrc();
				u8  byMode = cMcChnnlInfo.GetMcChnnlMode();
				m_cSMcuSpyMana.LeftOnceToFree(tMcSrc, byMode, dwCanReleaseBW, swCanRelIndex);
				//ֻ��ͬһ�¼���mcu���ж�Ԥ�ͷŴ���
				if (tMcSrc.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
				{
					dwCanReleaseBW = 0;
				}
			}
			byAddDestSpyNum = 1;
		}
		break;
	case MCS_MCU_STARTVMP_REQ:
		{
			if (MODE_BOTH == tPreSetInRsp.m_byRspSpyMode || MODE_VIDEO == tPreSetInRsp.m_byRspSpyMode)
			{
				if (m_tLastVmpParam.IsMtInMember(tPreSetInRsp.m_tSetInReqInfo.m_tSrc))
				{
					byAddDestSpyNum = 0;
				}
				else
				{
					u32 dwRelBW = dwCanReleaseBW;
					s16 swRelIndex = -1;

					u8 byIndex = tSpyInfo.m_tSpyVmpInfo.m_byPos;

					TVMPMember *ptVMPMember = m_tLastVmpParam.GetVmpMember(byIndex);

					if (ptVMPMember != NULL)
					{	
						m_cSMcuSpyMana.LeftOnceToFree(*(TMt *)ptVMPMember, MODE_VIDEO, dwCanReleaseBW, swCanRelIndex);
					}

					
					byAddDestSpyNum = 1;
				}
			}
		}
		break;

	case MCS_MCU_SPECSPEAKER_REQ:        //�������ָ̨��һ̨�ն˷���
	case MT_MCU_SPECSPEAKER_REQ:		 //��ͨ�ն�ָ������������
	case MT_MCU_SPECSPEAKER_CMD:
		{
			TMt tSpeaker = m_tConf.GetSpeaker();
			if( tSpeaker == tPreSetInRsp.m_tSetInReqInfo.m_tSrc )
			{
				byAddDestSpyNum = 0;
			}
			else
			{
				byAddDestSpyNum = 1;
				m_cSMcuSpyMana.LeftOnceToFree(tSpeaker, MODE_BOTH, dwCanReleaseBW, swCanRelIndex);
				//ֻ��ͬһ�¼���mcu���ж�Ԥ�ͷŴ���
				if (tSpeaker.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
				{
					dwCanReleaseBW = 0;
				}
			}
		}
		break;

	case MT_MCU_ADDMIXMEMBER_CMD:           //��ϯ���ӻ����ն�
	case MCS_MCU_ADDMIXMEMBER_CMD:          //���ӻ�����Ա
		break;

	case MCS_MCU_START_SWITCH_HDU_REQ:     // ���ѡ����������ǽ
	case MCUVC_STARTSWITCHHDU_NOTIFY:
		{
			TSpyHduInfo tSpyHduInfo = tSpyInfo.m_tSpyHduInfo;
			TPeriEqpStatus tHduStatus;   
			memset( &tHduStatus,0,sizeof( tHduStatus ) );
			
			g_cMcuVcApp.GetPeriEqpStatus(tSpyHduInfo.m_tHdu.GetEqpId(), &tHduStatus);

			TMt tOldMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[tSpyHduInfo.m_byDstChlIdx];

			if( tOldMt.GetMcuId() == tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId()
				&& tOldMt.GetMtId() == tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMtId() )
			{
				byAddDestSpyNum = 0;
			}
			else
			{
				byAddDestSpyNum = 1;
				m_cSMcuSpyMana.LeftOnceToFree(tOldMt, MODE_BOTH, dwCanReleaseBW, swCanRelIndex);
				//ֻ��ͬһ�¼���mcu���ж�Ԥ�ͷŴ���
				if (tOldMt.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
				{
					dwCanReleaseBW = 0;
				}
			}
		}
		break;

	case MCUVC_POLLING_CHANGE_TIMER:       // ������ѯ
		{
			switch(tPreSetInRsp.m_byRspSpyMode)
			{
			case MODE_BOTH:
				{
					//�߸ı䷢�����߼�
					TMt tSpeaker = m_tConf.GetSpeaker();
					if( tSpeaker == tPreSetInRsp.m_tSetInReqInfo.m_tSrc )
					{
						byAddDestSpyNum = 0;
					}
					else
					{
						byAddDestSpyNum = 1;
						m_cSMcuSpyMana.LeftOnceToFree(tSpeaker, MODE_BOTH, dwCanReleaseBW, swCanRelIndex);
						//ֻ��ͬһ�¼���mcu���ж�Ԥ�ͷŴ���
						if (tSpeaker.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
						{
							dwCanReleaseBW = 0;
						}
					}
				}
				break;
			case MODE_VIDEO:
				{
					//��Ƶ������ѯ
					TMt tLastMt = m_tConfPollParam.GetLastPolledMt();		
					
					u8 bySrcChnnl = ( GetLocalMtFromOtherMcuMt(tLastMt) == m_tPlayEqp ? m_byPlayChnnl : 0);
					
					//������Ƶ�㲥
					if (!m_tConf.m_tStatus.IsBrdstVMP())
					{
						if( tLastMt == tPreSetInRsp.m_tSetInReqInfo.m_tSrc )
						{
							byAddDestSpyNum = 0;
						}
						else
						{
							byAddDestSpyNum = 1;
							m_cSMcuSpyMana.LeftOnceToFree(tLastMt, MODE_VIDEO, dwCanReleaseBW, swCanRelIndex);
							//ֻ��ͬһ�¼���mcu���ж�Ԥ�ͷŴ���
							if (tLastMt.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
							{
								dwCanReleaseBW = 0;
							}
						}
					}
				}
				break;
			default:
				break;
			}
		}
		break;

	case MCS_MCU_START_SWITCH_TW_REQ:      // ���ѡ�������ǽ
		{
			TSpyTVWallInfo tSpyTVWallInfo = tSpyInfo.m_tSpyTVWallInfo;	
			TPeriEqpStatus tTvWallStatus;   
			memset( &tTvWallStatus,0,sizeof( tTvWallStatus ) );
			
			g_cMcuVcApp.GetPeriEqpStatus(tSpyTVWallInfo.m_tTvWall.GetEqpId(), &tTvWallStatus);

			TMt tOldMt = (TMt)tTvWallStatus.m_tStatus.tTvWall.atVideoMt[tSpyTVWallInfo.m_byDstChlIdx];

			if( tOldMt.GetMcuId() == tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId()
				&&tOldMt.GetMtId() == tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMtId()
				)
			{
				byAddDestSpyNum = 0;
			}
			else
			{
				byAddDestSpyNum = 1;

				m_cSMcuSpyMana.LeftOnceToFree(tOldMt, MODE_BOTH, dwCanReleaseBW, swCanRelIndex);
				//ֻ��ͬһ�¼���mcu���ж�Ԥ�ͷŴ���
				if (tOldMt.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
				{
					dwCanReleaseBW = 0;
				}
			}
		}
		break;

    case MCS_MCU_CHANGEROLLCALL_REQ:       // ��������߼�
		{
			TMtStatus tMtStatus;
			memset( &tMtStatus,0,sizeof( tMtStatus ) );
			if(  ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() ||
				ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode()
				)
			{
				m_ptMtTable->GetMtStatus( m_tRollCaller.GetMtId(),&tMtStatus );
				TMt tSrcSelMt = tMtStatus.GetSelectMt( MODE_VIDEO );
				if( tSrcSelMt == tPreSetInRsp.m_tSetInReqInfo.m_tSrc )
				{
					byAddDestSpyNum = 0;
				}
				else
				{
					byAddDestSpyNum = 1;
					m_cSMcuSpyMana.LeftOnceToFree(tSrcSelMt, MODE_VIDEO, dwCanReleaseBW, swCanRelIndex);
					//ֻ��ͬһ�¼���mcu���ж�Ԥ�ͷŴ���
					if (tSrcSelMt.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
					{
						dwCanReleaseBW = 0;
					}
				}
			}
			else if( ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
			{
				if (tPreSetInRsp.m_tSetInReqInfo.m_tSrc == tSpyInfo.m_tSpyRollCallInfo.m_tOldCallee)
				{
					byAddDestSpyNum = 0;
				}
				else
				{
					byAddDestSpyNum = 1;

					TMt tOldCallee;
					tOldCallee = tSpyInfo.m_tSpyRollCallInfo.m_tOldCallee;
					m_cSMcuSpyMana.LeftOnceToFree(tOldCallee, MODE_VIDEO, dwCanReleaseBW, swCanRelIndex);
					//ֻ��ͬһ�¼���mcu���ж�Ԥ�ͷŴ���
					if (tOldCallee.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
					{
						dwCanReleaseBW = 0;
					}
				}
			}
		}
		break;

	case MCS_MCU_STARTREC_REQ:		//�ն�¼��
		{
			byAddDestSpyNum = 1;
		}
		break;
	default:
		break;
	}	*/
	return;
}

/*==============================================================================
������    :  OnMMcuPreSetIn
����      :  ������ش����ڷ���Setin֮ǰ���Ƚ���������ȷ���Ƿ�֧�ֶ�ش�
             ���������ͻش�ͨ��
�㷨ʵ��  :  
����˵��  :  [IN] TPreSetInReq �����¼��ش��ı�Ҫ��Ϣ
����ֵ˵��:  BOOL32
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-9-28                   Ѧ��							create
2009-11-01                   pengjie                         modify
20100730                    pengjie                        ��ش���������
2011/01/19					xl								����ֵ�޸�
==============================================================================*/
BOOL32 CMcuVcInst::OnMMcuPreSetIn( TPreSetInReq &tPreSetInReq )
{
	if( tPreSetInReq.m_tSpyMtInfo.GetSpyMt().IsNull() || tPreSetInReq.m_tSpyMtInfo.GetSpyMt().IsLocal() )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[OnMMcuPreSetIn] SpySrcMt error !" );
		return FALSE;
	}

	s16 swIndex = m_cSMcuSpyMana.FindSpyMt(tPreSetInReq.m_tSpyMtInfo.GetSpyMt());
	//����˻ش�Դ�Ѿ��ڻش��б���
	if (-1 != swIndex)
	{
		CRecvSpy tSrcSpyInfo;
		m_cSMcuSpyMana.GetRecvSpy(swIndex, tSrcSpyInfo);
		if( tSrcSpyInfo.m_byUseState == TSpyStatus::WAIT_FREE &&
			(  VCS_CONF != m_tConf.GetConfSource() ||
				m_cVCSConfStatus.GetTVWallManageMode() != VCS_TVWALLMANAGE_AUTO_MODE ||
				(
				 MCS_MCU_START_SWITCH_HDU_REQ != tPreSetInReq.GetEvId() &&
				 MCS_MCU_START_SWITCH_TW_REQ != tPreSetInReq.GetEvId()
				)
				||
				tPreSetInReq.m_tReleaseMtInfo.m_tMt.IsNull() ||
				GetFstMcuIdFromMcuIdx(tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMcuId()) !=
				GetFstMcuIdFromMcuIdx( tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMcuId() )
				)
			)
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[OnMMcuPreSetIn] Fail to Presetin,SpySrcMt(%d.%d) Now Status  is WAIT_FREE!\n",
						tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMcuId(),
						tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMtId()
						);
			CServMsg cServMsg;
			cServMsg.SetEventId( MCU_MCS_ALARMINFO_NOTIF );
			cServMsg.SetErrorCode( ERR_MCU_CONFSNDBANDWIDTHISFULL );	
			SendMsgToAllMcs( MCU_MCS_ALARMINFO_NOTIF, cServMsg );
			return FALSE;
		}
	}

	//zhouyiliang 20101015 ���spymt����Ӧ�߼�ͨ��û�򿪣�return
	TMtStatus tMtStatus;	
	if ( GetMtStatus( tPreSetInReq.m_tSpyMtInfo.GetSpyMt(),tMtStatus ) )
	{
		BOOL32 bLogicalChnlOpen = TRUE;
		if ( !tMtStatus.IsSendVideo() 
			&& ( tPreSetInReq.m_bySpyMode == MODE_VIDEO || tPreSetInReq.m_bySpyMode == MODE_BOTH ||  
			     tPreSetInReq.m_bySpyMode == MODE_VIDEO_CHAIRMAN || tPreSetInReq.m_bySpyMode == MODE_BOTH_CHAIRMAN )
			) 
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[OnMMcuPreSetIn] SpySrcMt video logical chanel not open !" );
			bLogicalChnlOpen = FALSE;
		} 
		if ( !tMtStatus.IsSendAudio() 
			&& ( tPreSetInReq.m_bySpyMode == MODE_AUDIO || tPreSetInReq.m_bySpyMode == MODE_BOTH || 
			     tPreSetInReq.m_bySpyMode == MODE_BOTH_CHAIRMAN )
			) 
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[OnMMcuPreSetIn] SpySrcMt audio logical chanel not open  !" );
			bLogicalChnlOpen = FALSE;
		} 	
		if ( !bLogicalChnlOpen ) 
		{
			//zhouyiliang 20101015 �����vmp������Ϊvcs�Զ�����ϳɣ�presetin���ɹ���Ӧ�ûָ�CurUseVMPChanInd
			TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
			if ( m_tConf.GetConfSource() == VCS_CONF 
				&& tConfVmpParam.IsVMPAuto()
				&& MCS_MCU_STARTVMP_REQ ==  tPreSetInReq.GetEvId())
			{
				u16 byCurUseChnId = tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byPos;
				m_cVCSConfStatus.SetCurUseVMPChanInd( byCurUseChnId );
				TMt tMtNull;
				tMtNull.SetNull();
				m_cVCSConfStatus.SetReqVCMT( tMtNull );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[OnMMcuPreSetIn] Update vcs CurUseVMPChanInd.%d:\n",byCurUseChnId);
			}

			return FALSE;
		}
	}

	CServMsg cServMsg;
	TMsgHeadMsg tHeadMsg;

	TMt tNormalMt;
	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tPreSetInReq.m_tSpyMtInfo.GetSpyMt(), tNormalMt );	
    tPreSetInReq.m_tSpyMtInfo.SetSpyMt( tNormalMt );

	//������ͷ��ն���Ϣ
	TMultiCacMtInfo tMtInfo = BuildMultiCascadeMtInfo( tPreSetInReq.m_tReleaseMtInfo.m_tMt,tPreSetInReq.m_tReleaseMtInfo.m_tMt );
	tPreSetInReq.m_tReleaseMtInfo.m_byCasLevel	= tMtInfo.m_byCasLevel;
	memcpy( &tPreSetInReq.m_tReleaseMtInfo.m_abyMtIdentify[0],
		&tMtInfo.m_abyMtIdentify[0],
		sizeof(tPreSetInReq.m_tReleaseMtInfo.m_abyMtIdentify)
		);
	tPreSetInReq.m_tReleaseMtInfo.m_byIsNeedRelease = 0;
	tPreSetInReq.m_tReleaseMtInfo.SetCount(0);

	u8 bySrcMcuId = (u8)tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMcuId();
	if( bySrcMcuId != (u8)tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMcuId() )
	{
		tPreSetInReq.m_tReleaseMtInfo.SetCount(tPreSetInReq.m_tReleaseMtInfo.GetCount()-1);
	}

	//zhouyiliang 20121023 ע�͵�����Ϊ�п�����Ȼ�п��滻�նˣ����Ƿ��ֿ��滻�ն˵ļ���Ϊ0
	//�������ѯ����Ƶ����ѯ������ȡ����ǰ�����ˣ�����һ���նˡ���ʱ�����Ϳ���Ϊ0����1�Ļ������������ҵ���У��ͱ��滻����
// 	if( 0 == tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum &&
// 		( MODE_BOTH == tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode ||
// 			MODE_VIDEO == tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode )
// 		)
// 	{
// 		++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
// 	}
// 	if( 0 == tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum &&
// 		( MODE_BOTH == tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode ||
// 			MODE_AUDIO == tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode )
// 		)
// 	{
// 		++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
// 	}

	//[2011/09/16/zhangli]�����MP4-16CIF�����·��ֱ���Ϊ4CIF
	TSimCapSet tSimCapSet = tPreSetInReq.m_tSpyMtInfo.GetSimCapset();
	if (MEDIA_TYPE_MP4 == tSimCapSet.GetVideoMediaType()
		&& VIDEO_FORMAT_16CIF == tSimCapSet.GetVideoResolution())
	{	
		tSimCapSet.SetVideoResolution(VIDEO_FORMAT_4CIF);
		tPreSetInReq.m_tSpyMtInfo.SetSimCapset(tSimCapSet);
	}
	
	// �����վ����������еķֱ���������
	u32 dwResW = 0;
	u32 dwResH = 0;
	u16 wTmpResW = 0;
	u16 wTmpResH = 0;
	u8 byRes = tSimCapSet.GetVideoResolution();
	// mpeg4 auto��Ҫ���ݻ������ʻ�÷ֱ���
	if ( VIDEO_FORMAT_AUTO == byRes )
	{
		byRes = GetAutoResByBitrate(byRes,m_tConf.GetBitRate());
	}
	// �Էֱ��ʽ��е���,�����¼��Ǿ�mcu,�跢��ͨ�ֱ���,��ʵ�ֱ��ʿ�������׷��
	GetWHByRes(byRes, wTmpResW, wTmpResH);
	// ���ݷֱ���,��ö�Ӧͨ�÷ֱ���,�¾�MCU����ʶ��ķֱ���
	dwResW = htonl((u32)wTmpResW);
	dwResH = htonl((u32)wTmpResH);
	byRes = GetNormalRes(wTmpResW, wTmpResH);
	tSimCapSet.SetVideoResolution(byRes);
	tPreSetInReq.m_tSpyMtInfo.SetSimCapset(tSimCapSet);

	//���Ͷ�ش���������
	cServMsg.SetEventId(MCU_MCU_PRESETIN_REQ);
	cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cServMsg.CatMsgBody( (u8 *)&tPreSetInReq, sizeof(TPreSetInReq) );
	cServMsg.CatMsgBody( (u8 *)&dwResW, sizeof(u32) );
	cServMsg.CatMsgBody( (u8 *)&dwResH, sizeof(u32) );

	SendMsgToMt( bySrcMcuId, MCU_MCU_PRESETIN_REQ, cServMsg);

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "Send PRESETIN_REQ to mt(%d.%d.%d) lvl.%d.(%s) ReleaseMt(%d,%d,%d)[VNum.%d,ANum.%d,mode.%d].Res.%d,Width.%dHeight.%d\n",
		tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMcuId(),
		tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMtId(),
		tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
		tHeadMsg.m_tMsgDst.m_byCasLevel,
		OspEventDesc( u16(tPreSetInReq.GetEvId()) ),
		tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMcuId(),
		tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMtId(),
		tPreSetInReq.m_tReleaseMtInfo.m_abyMtIdentify[0],
		tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum,
		tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum,
		tPreSetInReq.m_bySpyMode,
		byRes,
		wTmpResW,
		wTmpResH
		);

	return TRUE;
}


/*==============================================================================
������    :  ProcMcuMcuSpyFastUpdateCmd
����      :  ��������ش�����ؼ�֡
�㷨ʵ��  :  
����˵��  :  [IN] const CMessage
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
20100402                 pengjie                         create
==============================================================================*/
void CMcuVcInst::ProcMcuMcuSpyFastUpdateCmd( const CMessage *pcMsg )
{
	STATECHECK;	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);	

	TMt tOrgMt = *(TMt *)( cServMsg.GetMsgBody() );
	u8  byMode = *(u8  *)( cServMsg.GetMsgBody() + sizeof(TMt) );
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) );

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuSpyFastUpdateCmd] tOrgMt<McuId:%d, MtId:%d>!\n",
			  	   tOrgMt.GetMcuId(), tOrgMt.GetMtId());
	TMt tSrcMt;
	u8  byLocalMtId = 0;
	if (!tOrgMt.IsMcuIdLocal())
	{
		//�Ǳ�����ת�ɱ�������ʶ��mcuidx��ʽtmt
		tSrcMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst, tOrgMt); 
		byLocalMtId = u8(tOrgMt.GetMcuId());
	}
	else
	{
		tSrcMt = tOrgMt;

		tSrcMt.SetMcuIdx(LOCAL_MCUIDX);
		byLocalMtId = tSrcMt.GetMtId();
	}

// 	if( !tSrcMt.IsMcuIdLocal() )
// 	{
// 		cServMsg.SetEventId( MCU_MCU_SPYFASTUPDATEPIC_CMD );		
// 		cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
// 		cServMsg.CatMsgBody( (u8 *)&tSrcMt, sizeof(TMt) );
// 		cServMsg.CatMsgBody( (u8 *)&byMode, sizeof(u8) );
// 		
// 		SendMsgToMt( (u8)tSrcMt.GetMcuId(), MCU_MCU_SPYFASTUPDATEPIC_CMD, cServMsg );
// 		return;
// 	}

// 	tSrcMt.SetMcuIdx( LOCAL_MCUIDX );
	
	// [11/10/2010 xliang] check if the mt is spyMt before check multispyMana
	BOOL32 bSrcIsSpyMt = FALSE; //�Ƿ����¼��ϴ��ն�
	TSimCapSet tDstCap;
	if( !m_tCascadeMMCU.IsNull() && tOrgMt.IsMcuIdLocal())
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
		if( ptConfMcInfo != NULL && ptConfMcInfo->m_tSpyMt == tSrcMt )
		{
			bSrcIsSpyMt = TRUE;
			tDstCap = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());
		}
	}

	if( !bSrcIsSpyMt )
	{
		//�Ȼ�ȡ����������ϼ��ɽ���������
		CSendSpy tSendSpy;
		if(!m_cLocalSpyMana.GetSpyChannlInfo(tSrcMt, tSendSpy))
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuSpyFastUpdateCmd] GetSpyChannlInfo Mt:%d failed!\n", tSrcMt.GetMtId());
			return;
		}
		if (tSendSpy.GetSimCapset().IsNull())
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuSpyFastUpdateCmd] Mt%d's m_cLocalSpyMana->Cap is null!\n", tSrcMt.GetMtId());
			return;
		}
		tDstCap = tSendSpy.GetSimCapset();
	}
 	
	TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(byLocalMtId);

	TBasOutInfo tReqBasOutInfo;
	if (MODE_AUDIO == byMode)
	{
		if(tDstCap.GetAudioMediaType() != tSrcCap.GetAudioMediaType())
		{
			if(!FindBasChn2SpyForMt(tSrcMt, tDstCap, byMode,tReqBasOutInfo))
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuSpyFastUpdateCmd:Aud] FindBasChn2SpyForMt%d failed!\n", tSrcMt.GetMtId());
				return;
			}
			NotifyFastUpdate(tReqBasOutInfo.m_tBasEqp, tReqBasOutInfo.m_byChnId);
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyFastUpdateCmd:Aud] tSrcId: %d, NotifyFastUpdate Bas<EqpId:%d, ChnId:%d>!\n",
 							tSrcMt.GetMtId(),  tReqBasOutInfo.m_tBasEqp.GetEqpId(), tReqBasOutInfo.m_byChnId);
		}
		else
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuSpyFastUpdateCmd:Aud] tSrcId: %d, Not Used SelBas!\n",
				tSrcMt.GetMtId() );
			NotifyFastUpdate(tSrcMt, byMode);
		}
	}
	else if (MODE_VIDEO == byMode || MODE_SECVIDEO == byMode)
	{
		if(IsNeedSpyAdpt(tSrcMt, tDstCap, MODE_VIDEO))
		{
			if(!FindBasChn2SpyForMt(tSrcMt, tDstCap, byMode, tReqBasOutInfo))
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuSpyFastUpdateCmd:Vid] FindBasChn2SpyForMt%d failed!\n", tSrcMt.GetMtId());
				return;
			}
			NotifyFastUpdate(tReqBasOutInfo.m_tBasEqp, tReqBasOutInfo.m_byChnId);
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyFastUpdateCmd:Vid] tSrcId: %d, NotifyFastUpdate Bas<EqpId:%d, ChnId:%d>!\n",
 						tSrcMt.GetMtId(),  tReqBasOutInfo.m_tBasEqp.GetEqpId(), tReqBasOutInfo.m_byChnId );
		}
		else
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuSpyFastUpdateCmd:Vid] tSrcId: %d, Not Used SelBas!\n",
				tSrcMt.GetMtId() );
			NotifyFastUpdate(tSrcMt, byMode);
		}
	}
	else
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[ProcMcuMcuSpyFastUpdateCmd] unexpected Mode:%d!\n", byMode);
	}

	return;
}

/*==============================================================================
������    :  SendMcuMcuSpyFastUpdateCmd
����      :  ���ͼ�����ش�����ؼ�֡
�㷨ʵ��  :  
����˵��  :  TMt &tMt �¼�ĳ��Դ�ն�
             u8 byMode ��Ƶ���ͣ�������˫����Ĭ������
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
20100402                 pengjie                         create
==============================================================================*/
void CMcuVcInst::SendMcuMcuSpyFastUpdateCmd( const TMt &tMt, u8 byMode )
{
	CServMsg cServMsg;
	TMsgHeadMsg tHeadMsg;


	cServMsg.SetEventId( MCU_MCU_SPYFASTUPDATEPIC_CMD );
	TMt tNormalMt;
	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
	//tMt = ;
	

	cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cServMsg.CatMsgBody( (u8 *)&tNormalMt, sizeof(TMt) );
	cServMsg.CatMsgBody( (u8 *)&byMode, sizeof(u8) );

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY,  "[SendMcuMcuSpyFastUpdateCmd] tSrcId: %d, Mode: %d ! \n", tMt.GetMtId(), byMode );
	
	SendMsgToMt( GetFstMcuIdFromMcuIdx(tMt.GetMcuId()), MCU_MCU_SPYFASTUPDATEPIC_CMD, cServMsg );

	return;
}


/*==============================================================================
����		:  JudgeMcuMcuPreSetIn
����		:  �ж��¼��Ƿ�֧�ִ��ն˵Ļش�
����˵��	:	const TPreSetInReq& tPreSetInReq req������
				TPreSetInRsp& tPreSetInRsp ׼�����ظ��ϼ���rsp�������ᱻ�޸�
				u16 &wErrorCode ����������¼���ش������Ļ���������Ӧ��ErrorCode
����ֵ˵��	: TRUE֧�֣�FALSE��֧��
��ע		:  
-------------------------------------------------------------------------------
�޸ļ�¼:  
��  ��     �汾          �޸���          �߶���          �޸ļ�¼
2010-6-10                lukunpeng                        modify
==============================================================================*/
BOOL32 CMcuVcInst::JudgeMcuMcuPreSetIn(const TPreSetInReq& tPreSetInReq, const TMsgHeadMsg& tHeadMsg, TPreSetInRsp& tPreSetInRsp, u16 &wErrorCode)
{
	//[2011/10/27/zhangli]Ϊ���������ԭֵ������Ų�����ֵ
	//wErrorCode = 0;
	TMt tOrgSrc = tPreSetInReq.m_tSpyMtInfo.GetSpyMt();
	TMt tUnLocalSrc = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst,tOrgSrc );
	u8  bySpyMode = tPreSetInReq.m_bySpyMode;
	u8 byRealRes = 0;

	TMt tSrc;
	if (!tOrgSrc.IsMcuIdLocal())
	{
		tSrc = m_ptMtTable->GetMt(u8(tOrgSrc.GetMcuId()));
	}
	else
	{
		tSrc = m_ptMtTable->GetMt(tOrgSrc.GetMtId());
	}

	//1��������֧�ֶ�ش�����
	if( !IsSupportMultiSpy() )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[JudgeMcuMcuPreSetIn] Conf Is Not Support MultiSpy.\n" );
		wErrorCode = ERR_MCU_CONFNOTSUPPORTMULTISPY;
		return FALSE;
	}
	
	//2������ش�Դ������
	if( tSrc.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "JudgeMcuMcuPreSetIn False, tSrc is NULL! \n" );
		return FALSE;
	}

	//3������ش�mode��none����
	if( bySpyMode == MODE_NONE )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "JudgeMcuMcuPreSetIn False, SpyMode == MODE_NONE! \n" );
		return FALSE;
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "JudgeMcuMcuPreSetIn SpyMode = %d \n", bySpyMode );

	//4���ش�Դ�ն˱����Ѿ��������
	if (FALSE == m_tConfAllMtInfo.MtJoinedConf(tSrc.GetMtId()))
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "JudgeMcuMcuPreSetIn Failed Mt:%d Don't Online \n",tSrc.GetMtId() );
		return FALSE;
	}

	//5������ش�Դ�ĺ���ͨ��û�д򿪣�ҲҪ��NACK
	TLogicalChannel tVidChnnl;
	TLogicalChannel tAudChnnl;
	if (MODE_VIDEO == bySpyMode || MODE_BOTH == bySpyMode)
	{
		if (FALSE == m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_VIDEO, &tVidChnnl, FALSE ))
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY,  "JudgeMcuMcuPreSetIn Failed Mt:%d 's Revert Vid LogicChnnl not open \n",
							tSrc.GetMtId() );
			return FALSE;
		}
	}

	if (MODE_AUDIO == bySpyMode || MODE_BOTH == bySpyMode)
	{
		if (FALSE == m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_AUDIO, &tAudChnnl, FALSE ))
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY,  "JudgeMcuMcuPreSetIn Failed Mt:%d 's Revert Aud LogicChnnl not open \n",
							tSrc.GetMtId() );
			return FALSE;
		}
	}

	TSimCapSet tSimCap = tPreSetInReq.m_tSpyMtInfo.GetSimCapset();

	// 6.������ԴУ��
	TBasOutInfo	tBasOutInfo;
	BOOL32 bAdpOK = FALSE;
	if(MODE_BOTH == bySpyMode || MODE_VIDEO == bySpyMode)
	{
		TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
		
		if (IsNeedSpyAdpt(tSrc, tSimCap, MODE_VIDEO) &&
			!FindBasChn2SpyForMt(tUnLocalSrc, tSimCap, MODE_VIDEO, tBasOutInfo))
		{
			////[2011/08/25/zhangli]����Ҳ������������ң������ϵ��Ƿ��Ϊ���ش�ռ�ã��ǵĻ�������ˢ��ͨ������
			////����Ҳ����ٳ���ռ�ÿ��е�
			TSimCapSet tTempSrcCap = tSimCap;
			u8 byNextUpStandRes = GetUpStandRes(tSimCap.GetVideoResolution());
			while (VIDEO_FORMAT_INVALID != byNextUpStandRes)
			{
				tTempSrcCap.SetVideoResolution(byNextUpStandRes);
				if (FindBasChn2SpyForMt(tUnLocalSrc, tTempSrcCap, MODE_VIDEO, tBasOutInfo))
				{
					if (IsCanRlsBasChn(tUnLocalSrc, tUnLocalSrc, tTempSrcCap, MODE_VIDEO))
					{	
						bAdpOK = TRUE;
						break;
					}
				}
				byNextUpStandRes = GetUpStandRes(byNextUpStandRes);
			}
			
			if (!bAdpOK)
			{
				//ѭ���Խ��ֱ�������
				CBasChn *pcBasChn=NULL;
				TBasChnCapData tBasChnCapData;
				bAdpOK = GetBasChnForMultiSpy(tUnLocalSrc,tSimCap,MODE_VIDEO,&pcBasChn,tBasChnCapData);	
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[JudgeMcuMcuPreSetIn] tPreSetInReq.m_tReleaseMtInfo.GetCount() = %d!\n", tPreSetInReq.m_tReleaseMtInfo.GetCount());
				//û�п���ͨ�����ҿ��ͷ��ն���Ҫ���������Ը��øÿ��ͷ�����ͨ��
				if( !bAdpOK && tPreSetInReq.m_tReleaseMtInfo.GetCount() >= 0 )
				{
					TMultiCacMtInfo tMtInfo;
					tMtInfo.m_byCasLevel = tPreSetInReq.m_tReleaseMtInfo.m_byCasLevel;
					memcpy( &tMtInfo.m_abyMtIdentify[0],
						&tPreSetInReq.m_tReleaseMtInfo.m_abyMtIdentify[0],
						sizeof(tMtInfo.m_abyMtIdentify));

					TMt tCanReleaseMt = GetMtFromMultiCascadeMtInfo( tMtInfo, tPreSetInReq.m_tReleaseMtInfo.m_tMt);
					CSendSpy tSendSpy;
					if (!tCanReleaseMt.IsNull() && 
						m_cLocalSpyMana.GetSpyChannlInfo(tCanReleaseMt, tSendSpy))
					{
						//��һ�γ��Կ��ͷ�����ͨ���Ƿ�ɸ���
						bAdpOK = IsCanRlsBasChn(tCanReleaseMt, tUnLocalSrc, tSendSpy.GetSimCapset(), MODE_VIDEO);
						if(!bAdpOK)
						{
							//�ֱ���ȡС�ٳ��Կ��ͷ�����ͨ���Ƿ�ɸ���
							TSimCapSet tDstCap = tSendSpy.GetSimCapset();
							
							const u8 byDstRes = tDstCap.GetVideoResolution();
							const u8 bySrcRes = tSrcCap.GetVideoResolution();
							tDstCap.SetVideoResolution(min(byDstRes, bySrcRes));
							bAdpOK = IsCanRlsBasChn(tCanReleaseMt, tUnLocalSrc,tDstCap, MODE_VIDEO);
						}
						if (bAdpOK)
						{
							tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsReuseBasChl = MODE_VIDEO;
						}					
					}	
				}
			}
		}	
		else
		{
			//���лش�����ͨ���ɸ���
			bAdpOK = TRUE;
		}

		if (!bAdpOK)
		{
			bySpyMode  = (MODE_BOTH == bySpyMode) ? MODE_AUDIO:MODE_NONE;
			wErrorCode = ERR_MCU_CASDBASISNOTENOUGH;
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[JudgeMcuMcuPreSetIn] No idle Bas channel(Video) support Mt(%d) to Mt(%d).Now Mode(%d)\n",
				tSrc.GetMtId(),m_tCascadeMMCU.GetMtId(),bySpyMode );
		}		
	}

	bAdpOK = FALSE;
	if (MODE_BOTH == bySpyMode || MODE_AUDIO == bySpyMode)
	{
		CBasChn *pcBasChn=NULL;
		TBasChnCapData tBasChnCapData;

		//���Դ����
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

		//���Ŀ������
		TAudioTypeDesc tDstAudCap;
		TLogicalChannel tDstAudLgc;
		if (!m_ptMtTable->GetMtLogicChnnl(m_tCascadeMMCU.GetMtId(), MODE_AUDIO, &tDstAudLgc, TRUE))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS,  "[IsNeedSpyAdpt] GetMtLogicChnnl Dst Aud Mt%d failed!\n", m_tCascadeMMCU.GetMtId());
			return FALSE;
		}
		tDstAudCap.SetAudioMediaType( tDstAudLgc.GetChannelType() );
		tDstAudCap.SetAudioTrackNum( tDstAudLgc.GetAudioTrackNum() );

		//�ȳ��Ի�ȡ��������ͨ��
		if ( IsNeedSpyAdpt(tSrc,tSimCap,MODE_AUDIO)&&
			!FindBasChn2SpyForMt(tUnLocalSrc, tSimCap, MODE_AUDIO, tBasOutInfo) &&
			!g_cMcuVcApp.GetIdleAudBasChn(tSrcAudCap, tDstAudCap,&pcBasChn))
		{
			//û�п���ͨ�����ҿ��ͷ��ն���Ҫ���������Ը�������ͨ��
			if( tPreSetInReq.m_tReleaseMtInfo.GetCount() >= 0 )
			{
				TMultiCacMtInfo tMtInfo;
				tMtInfo.m_byCasLevel = tPreSetInReq.m_tReleaseMtInfo.m_byCasLevel;
				memcpy( &tMtInfo.m_abyMtIdentify[0],
					    &tPreSetInReq.m_tReleaseMtInfo.m_abyMtIdentify[0],
					    sizeof(tMtInfo.m_abyMtIdentify)
					     );
				TMt tCanReleaseMt = GetMtFromMultiCascadeMtInfo( tMtInfo, tPreSetInReq.m_tReleaseMtInfo.m_tMt);
				CSendSpy tSendSpy;
				if (!tCanReleaseMt.IsNull() && 
					m_cLocalSpyMana.GetSpyChannlInfo(tCanReleaseMt, tSendSpy) &&
					IsCanRlsBasChn(tCanReleaseMt,tUnLocalSrc, tSendSpy.GetSimCapset(), MODE_AUDIO))
				{
					if (MODE_VIDEO == tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsReuseBasChl)
					{
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsReuseBasChl = MODE_BOTH;
					}
					else
					{
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsReuseBasChl = MODE_AUDIO;
					}
					
					bAdpOK = TRUE;
				}			
			}		
		}
		else
		{	
			bAdpOK = TRUE;		
		}
		if (!bAdpOK)
		{
			bySpyMode  = (MODE_BOTH == bySpyMode) ? MODE_VIDEO:MODE_NONE;
			wErrorCode = ERR_MCU_CASDBASISNOTENOUGH;
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,"[JudgeMcuMcuPreSetIn] No idle Bas channel(Audio) support Mt(%d) to Mt(%d).Now Mode(%d)\n",
				tSrc.GetMtId(),m_tCascadeMMCU.GetMtId(),bySpyMode );
		}		
	}

	//zjj20110119 ���ڻ�����ѯ�¼�û��bas�����޷��ϴ���Ƶ���ն�����ƵԴ����ȡ��������
	if( MODE_AUDIO == bySpyMode && MCUVC_POLLING_CHANGE_TIMER == tPreSetInReq.GetEvId() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY,  "JudgeMcuMcuPreSetIn Failed Mt:%d 's,no Video Mode No Polling.\n",
						tSrc.GetMtId() );
		return FALSE;
	}

	if( MODE_BOTH != bySpyMode && MCS_MCU_STARTREC_REQ == tPreSetInReq.GetEvId() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY,  "JudgeMcuMcuPreSetIn Failed Mt:%d 's,no Both Mode No Recording.\n",
						tSrc.GetMtId() );
		return FALSE;
	}

	if( MODE_NONE == bySpyMode )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY,  "JudgeMcuMcuPreSetIn Failed Mt:%d 's Revert Aud LogicChnnl not open \n",
						tSrc.GetMtId() );
		return FALSE;
	}

	// ת�����ģʽ�������ϼ�
	tPreSetInRsp.m_byRspSpyMode = bySpyMode;

	return TRUE;
}

/*==============================================================================
����    :  ProcMcuMcuPreSetInReq
����    :  �����ϼ�������PreSetIn�����������¼�������Ƿ�����ش��������ش���������ȣ�Ȼ��֪ͨ�ϼ�
��Ҫ�ӿ�:  
����    :  [IN] const CMessage
��ע    :  
-------------------------------------------------------------------------------
�޸ļ�¼:  
��  ��     �汾          �޸���          �߶���          �޸ļ�¼
2010-6-10                lukunpeng                        modify
20100730                 pengjie                         ��ش���������
==============================================================================*/
void CMcuVcInst::ProcMcuMcuPreSetInReq(const CMessage *pcMsg )
{
	STATECHECK;	

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	CServMsg cMsg;
	BOOL32 bNack = FALSE;
	//[2011/10/27/zhangli]�����ϼ��������Ĵ���ţ�����ڶ�������������Ҫȡ������ţ�������һ��
	u16 wErrorCode = cServMsg.GetErrorCode();
	
	TPreSetInReq tPreSetInReq = *(TPreSetInReq *)( cServMsg.GetMsgBody() );
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TPreSetInReq));

	//����ϼ�����ʵ�ֱ��ʵĿ�ߣ���Ҫ����[11/12/2012 chendaiwei]
	u32 dwResW = 0;
	u32 dwResH = 0;
	if( cServMsg.GetMsgBodyLen() > sizeof(TPreSetInReq) + sizeof(TMsgHeadMsg))
	{
		dwResW = *(u32*)(cServMsg.GetMsgBody()+sizeof(TPreSetInReq)+sizeof(TMsgHeadMsg));
		dwResH = *(u32*)(cServMsg.GetMsgBody()+sizeof(TPreSetInReq)+sizeof(TMsgHeadMsg)+sizeof(u32));
		//�������������תΪ������
		dwResW = ntohl(dwResW);
		dwResH = ntohl(dwResH);
		u8 byRes = GetMcuSupportedRes((u16)dwResW, (u16)dwResH);
		TSimCapSet tCap = tPreSetInReq.m_tSpyMtInfo.GetSimCapset();
		tCap.SetVideoResolution(byRes);
		tPreSetInReq.m_tSpyMtInfo.SetSimCapset(tCap);
	}

	TPreSetInRsp tPreSetInRsp;
	tPreSetInRsp.m_tSetInReqInfo = tPreSetInReq;
	TMt tSrc = tPreSetInReq.m_tSpyMtInfo.GetSpyMt();
	TMsgHeadMsg tHeadMsgRsp;

	tHeadMsgRsp.m_tMsgSrc = tHeadMsg.m_tMsgDst;
	tHeadMsgRsp.m_tMsgDst = tHeadMsg.m_tMsgSrc;
	cMsg.SetMsgBody( (u8 *)&tHeadMsgRsp, sizeof(TMsgHeadMsg) );

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuPreSetInReq] Rcv MMcu PreSetInReq SpyMode:%d , VidCap<Media.%d Res.%d BR.%d Fps.%d Profile:%d>, AudCap<Media.%d>, ReleaseCount.%d, ReleaseMt<McuId.%d, MtId.%d>[V.%d, A.%d], Ev.%d[%s]!\n",
												tPreSetInReq.m_bySpyMode,
												tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoMediaType(),
												tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoResolution(),
												tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoMaxBitRate(),
												MEDIA_TYPE_H264 == tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoMediaType() ?
												tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetUserDefFrameRate(): tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoFrameRate(),
												tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoProfileType(),
												tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetAudioMediaType(),
												tPreSetInReq.m_tReleaseMtInfo.GetCount(),
												tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMcuId(),
												tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMtId(),
												tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum,
												tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum,
												tPreSetInReq.GetEvId(),
												OspEventDesc((u16)tPreSetInReq.GetEvId())
												);

	//ע�⣺�˴�judge����������ʻ��tPreSetInRsp���޸�
	if (!JudgeMcuMcuPreSetIn(tPreSetInReq, tHeadMsg, tPreSetInRsp, wErrorCode))
	{
		cMsg.SetErrorCode(wErrorCode);
		cMsg.SetEventId(MCU_MCU_PRESETIN_NACK);
		cMsg.CatMsgBody( (u8 *)&tPreSetInRsp, sizeof(tPreSetInRsp) );
		
		SendMsgToMt( cServMsg.GetSrcMtId(), MCU_MCU_PRESETIN_NACK, cMsg );
		return;
	}

	//ת�ɱ���McuIdx��ʶ
	TMt tSpyMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst, tPreSetInReq.m_tSpyMtInfo.GetSpyMt() );

/*  20110531 zjl ��δ���û�ã�����Ϣ��������ʱ����������һ��
	if( !tPreSetInReq.m_tSpyMtInfo.GetSpyMt().IsMcuIdLocal() )
	{
		tPreSetInReq.m_tSpyMtInfo.SetSpyMt( m_ptMtTable->GetMt((u8)tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMcuId()) );
	}
	else
	{
		tPreSetInReq.m_tSpyMtInfo.SetSpyMt( m_ptMtTable->GetMt( tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMtId() ) );
	}*/

	s16 swIndex = m_cSMcuSpyMana.FindSpyMt( tSpyMt );
	CRecvSpy tSrcSpyInfo;	
	//����˻ش�Դ�Ѿ��ڻش��б���,����״̬Ϊ�ȴ��ͷž�ֱ�ӻ�Nack���ϼ�
	if (-1 != swIndex)
	{
		m_cSMcuSpyMana.GetRecvSpy(swIndex, tSrcSpyInfo);
		if( tSrcSpyInfo.m_byUseState == TSpyStatus::WAIT_FREE &&
			(  VCS_CONF != m_tConf.GetConfSource() ||
				 ( CONF_CREATE_MT != m_byCreateBy &&
						m_cVCSConfStatus.GetTVWallManageMode() != VCS_TVWALLMANAGE_AUTO_MODE
					 )
					||
				(
				 MCS_MCU_START_SWITCH_HDU_REQ != tPreSetInReq.GetEvId() &&
				 MCS_MCU_START_SWITCH_TW_REQ != tPreSetInReq.GetEvId() 
				)
				||
				tPreSetInReq.m_tReleaseMtInfo.m_tMt.IsNull() || 
				tPreSetInReq.m_tReleaseMtInfo.GetCount() < 0 ||
				GetFstMcuIdFromMcuIdx(tSpyMt.GetMcuId()) != 
					tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMcuId()
					)
			)
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuPreSetInReq] Fail to Presetin,SpySrcMt(%d.%d) Now Status  is WAIT_FREE!\n",
						tSpyMt.GetMcuId(),
						tSpyMt.GetMtId()
						);
		
			cMsg.SetEventId(MCU_MCU_PRESETIN_NACK);
			cMsg.SetErrorCode( ERR_MCU_SPYMTSTATE_WAITFREE );
			cMsg.CatMsgBody( (u8 *)&tPreSetInRsp, sizeof(tPreSetInRsp) );
			SendMsgToMt( cServMsg.GetSrcMtId(), MCU_MCU_PRESETIN_NACK, cMsg );
			return;
		}
	}

	//����ش��ն˲��Ǳ����ģ��ٷ�����Ӧ��mcu
	if( !tSrc.IsMcuIdLocal() )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuPreSetInReq] tSrc(%d.%d) is not local mt,continue send Req to Mcu(%d)\n",
													tSrc.GetMcuId(),tSrc.GetMtId(),tSrc.GetMcuId()
			);
		if( IsLocalAndSMcuSupMultSpy( tSpyMt.GetMcuId() ) )
		{
			//zjl20101117[add]��ȡ�������¼�����ͨ������������SimCapSet
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSpyMt.GetMcuId()));

			//zjl20110117 �������������ͨ����ʽ�͵�һ��Ҫ��ڶ����ش��ĸ�ʽ��ͬ����ֱ���ȡС
			if(!tSimCapSet.IsNull() &&
					(tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoMediaType() == tSimCapSet.GetVideoMediaType()
						)
				)
			{
				u8 byMinRes = GetMinResAcdWHProduct(tSimCapSet.GetVideoResolution(), 
													tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoResolution());
				
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuPreSetInReq] GetMinRes %d between DstRes:%d and SrcRes:%d!\n",
											byMinRes, 
											tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoResolution(),
											tSimCapSet.GetVideoResolution());
				
				if (VIDEO_FORMAT_INVALID != byMinRes)
				{
					tSimCapSet.SetVideoResolution(byMinRes);
				}				
			}

			//zjl20101116 �����ǰ�ն��ѻش���������Ҫ���ѻش�Ŀ��������ȡС
			if (!GetMinSpyDstCapSet(tSpyMt, tSimCapSet))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuPreSetInReq] tSimCapSet is null!\n" );
				return;
			}
			
			u16 wTmpResW = 0;
			u16 wTmpResH = 0;
			// �Էֱ��ʽ��е���,�����¼��Ǿ�mcu,�跢��ͨ�ֱ���,��ʵ�ֱ��ʿ�������׷��
			u8 byCommonRes = tSimCapSet.GetVideoResolution();
			GetWHByRes(byCommonRes, wTmpResW, wTmpResH);
			// ���ݷֱ���,��ö�Ӧͨ�÷ֱ���,�¾�MCU����ʶ��ķֱ���
			dwResW = wTmpResW;
			dwResH = wTmpResH;
			byCommonRes = GetNormalRes(wTmpResW, wTmpResH);
			tSimCapSet.SetVideoResolution(byCommonRes);

			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuPreSetInReq:SendToNextMcu] VidCap<Media:%d, Res:%d, BR:%d, Fps:%d>!\n",
														tSimCapSet.GetVideoMediaType(), 
														tSimCapSet.GetVideoResolution(), 
														tSimCapSet.GetVideoMaxBitRate(),
														MEDIA_TYPE_H264 == tSimCapSet.GetVideoMediaType() ?
														tSimCapSet.GetUserDefFrameRate(): tSimCapSet.GetVideoFrameRate());

			tPreSetInReq.m_tSpyMtInfo.SetSimCapset(tSimCapSet);	
			tPreSetInReq.m_tSpyMtInfo.SetSpyMt( tSrc );
			tPreSetInReq.m_bySpyMode = tPreSetInRsp.m_byRspSpyMode;

			//tHeadMsg.m_tMsgDst.m_tMt = tSrc;//BuildMultiCascadeMtInfo( tPreSetInReq.m_tSrc );
			if(  tPreSetInReq.m_tReleaseMtInfo.GetCount() < 0 ||
				(u8)tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMcuId() != (u8)tSrc.GetMcuId() 
			  )
			{
				tPreSetInReq.m_tReleaseMtInfo.SetCount(tPreSetInReq.m_tReleaseMtInfo.GetCount()-1);				
			}
			
			//[2011/10/27/zhangli]�Ѵ���Ŵ����¼����Ա��ڵõ�ackʱ����Ӧ��ʾ
			tPreSetInReq.m_tReleaseMtInfo.m_byIsReuseBasChl = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsReuseBasChl;
			cServMsg.SetErrorCode(wErrorCode);
			cServMsg.SetEventId(MCU_MCU_PRESETIN_REQ);
			cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
			cServMsg.CatMsgBody( (u8 *)&tPreSetInReq, sizeof(TPreSetInReq) );
			
			if( dwResW !=0 && dwResH != 0)
			{
				dwResW = htonl(dwResW);
				dwResH = htonl(dwResH);
				cServMsg.CatMsgBody( (u8 *)&dwResW, sizeof(u32) );
				cServMsg.CatMsgBody( (u8 *)&dwResH, sizeof(u32) );
			}

			SendMsgToMt( (u8)tSrc.GetMcuId(), MCU_MCU_PRESETIN_REQ, cServMsg );
			return;
		}		
		else
		{
			tSrc = m_ptMtTable->GetMt( (u8)tSrc.GetMcuId() );
		}
	}

	//������лش��ն˴������
	TLogicalChannel     tAudChnnl;
	m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_AUDIO, &tAudChnnl, FALSE );

	u32 dwAudBitrate  = GetAudioBitrate( tAudChnnl.GetChannelType() );
	u32 dwVidBitrate  = m_ptMtTable->GetDialBitrate(tSrc.GetMtId());
	

	//ֱ�Ӱ�����Ƶ�����ظ��ϼ������ϼ�����SpyMode����ʹ��
	tPreSetInRsp.SetAudSpyBW(dwAudBitrate);
	tPreSetInRsp.SetVidSpyBW(dwVidBitrate);

	//zjl20101116PreSetInAck����Ŀ��������
	TSimCapSet tNullCap;
	tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSimCapset(tNullCap);

	//[2011/09/06/zhangli]���������ĳһ�ش�ģʽ�����ﴫ�ش���ţ�����ʾ����
	cMsg.SetErrorCode(wErrorCode);
	cMsg.SetEventId(MCU_MCU_PRESETIN_ACK);
	cMsg.CatMsgBody( (u8 *)&tPreSetInRsp, sizeof(tPreSetInRsp) );
	SendMsgToMt( cServMsg.GetSrcMtId(), MCU_MCU_PRESETIN_ACK, cMsg );

	return;
}



/*==============================================================================
����        :  ProcMcuMcuSpyBWFull
����        :  �ش��������ˣ�����Ӧ�Ĵ���
����˵��	:  [IN] const CMessage *pcMsg
	           ��out��swCanRelIndex���滻�Ķ�ش�ͨ���ţ�ֻ��vcs�Զ�����ϳɸò�����Ч
			   ��out��byInsertpos ����presetin���ն�Ӧ�÷���vmpParam�е�λ�ã�ֻ��vcs�Զ�����ϳɸò�����ı�
����ֵ˵��	:  void
��ע		:  
-------------------------------------------------------------------------------
�޸ļ�¼:  
��  ��     �汾          �޸���          �߶���          �޸ļ�¼
20100730   4.6              pengjie                      ��ش���������
==============================================================================*/
void CMcuVcInst::ProcMcuMcuSpyBWFull(const CMessage *pcMsg,TPreSetInRsp *ptPreSetInRsp,u16 wErrorCode /*= ERR_MCU_CONFSNDBANDWIDTHISFULL*/ )
{

	STATECHECK;
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	TPreSetInRsp tPreSetInRsp = *ptPreSetInRsp;
	TMt tSrc = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();

	u8 byLoop1 = 0,byLoop = 0;
	TConfMcInfo *ptMcInfo = NULL;
	TConfMtInfo *ptConfMtInfo = NULL;
	
	BOOL32 bInConf = FALSE,bInJoinedConf = FALSE;
	BOOL32 bAlarm = TRUE;
	u8 byMcuId = 0;

	//������Դ���������Ϣ���⴦��,��Ҫ����ѯ��ʱ��Ҫ��ʾ��������
	switch(tPreSetInRsp.m_tSetInReqInfo.GetEvId())
	{
	case MCUVC_POLLING_CHANGE_TIMER:
		{
			//����ǻ�����ѯ�����һش�����������Ϊ���ٶȿ��ǣ�ֱ���������նˡ�
			KillTimer(MCUVC_POLLING_CHANGE_TIMER);
			//zhouyiliang 20121225 ���ֻ��һ���¼��ն�(����ֻ��һ���¼��ն����֣�����ն�û��Ƶ)���Ҵ����bas���㣬��ʱӦ�ø�����ʾ
			BOOL32 bOnlyPollOneMt = FALSE;
			TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
			u8 byPollIdx = m_tConfPollParam.GetCurrentIdx();
			// 20110413_miaoqingsong �õ���һ�����ϱ���ѯ�������ն�
			TMtPollParam *ptNextPollMt = GetNextMtPolled(byPollIdx, tPollInfo); 
			if ( NULL != ptNextPollMt && !ptNextPollMt->IsNull() && !ptNextPollMt->IsLocal() && (TMt)*ptNextPollMt == tSrc )
			{
				bOnlyPollOneMt = TRUE;
			}
			if (!bOnlyPollOneMt)
			{
				ProcPollingChangeTimerMsg(pcMsg);
				return;
			}
			else
			{
				ProcStopConfPoll();
				LogPrint(LOG_LVL_ERROR,MID_MCU_SPY,"[ProcMcuMcuSpyBWFull]Only one submt in poll,and not enough spy bandwith or bas!\n");
				bAlarm = TRUE;
				break;
				
			}

		}
		//break;
	case MCS_MCU_START_SWITCH_HDU_REQ:
		{
			TSpyHduInfo* ptSpyHduInfo = &tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo;

			//�����hdu����ǽ������ѯ��ֱ�ӷ���
			if (ptSpyHduInfo->m_bySrcMtType == TW_MEMBERTYPE_BATCHPOLL)
			{
				return;
			}
			else if (ptSpyHduInfo->m_bySrcMtType == TW_MEMBERTYPE_TWPOLL)
			{
				//����ǵ���ǽ��ͨ����ѯ,ֱ���������ն�
				//20110610_tzy Bug00055651������ش����飬�¼��ش���������������£����¼����ն˽��е���ǽ��ѯ����ѯ˳������

				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(ptSpyHduInfo->m_tHdu.GetEqpId(), ptSpyHduInfo->m_byDstChlIdx, dwTimerIdx) )
				{
					TTvWallPollParam *ptTWPollParam = m_tTWMutiPollParam.GetTWPollParamByTimerIdx((u8)dwTimerIdx);

					u8 byCurPollPos     = ptTWPollParam->GetCurrentIdx();					
					u8 byTWId           = ptTWPollParam->GetTvWall().GetEqpId();
					u8 byChnId          = ptTWPollParam->GetTWChnnl();
					u8 byIsStartAsPause = ptTWPollParam->GetIsStartAsPause();

					if ( 0 == byIsStartAsPause)   
					{
						byCurPollPos++;
					}
					else // ����ͣ�ָ�ʱ�����ŵ�ǰ�ն���ѯ   
					{
						m_tTWMutiPollParam.SetIsStartAsPause(byTWId, byChnId, 0);
					}

					TMtPollParam *ptCurPollMt = GetMtTWPollParam(byCurPollPos, byTWId, byChnId);

					CMessage cMsg;
					if( NULL != ptCurPollMt && (TMt)(*ptCurPollMt) == tSrc )
					{						
						cServMsg.SetEventId( MCS_MCU_STOPHDUPOLL_CMD );
						cServMsg.SetMsgBody( &byTWId,sizeof(byTWId) );
						cServMsg.CatMsgBody( &byChnId,sizeof(byChnId) );
						cMsg.event = MCS_MCU_STOPHDUPOLL_CMD;
						cMsg.content = cServMsg.GetServMsg();
						cMsg.length  = cServMsg.GetServMsgLen();
						ProcMcsMcuHduPollMsg( &cMsg );
					}
					else
					{		

						KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
						CMessage cMsg;
						memset(&cMsg, 0, sizeof(cMsg));
						cMsg.event = u16(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
						cMsg.content = (u8*)&dwTimerIdx;
						cMsg.length  = sizeof(u32);
						ProcTWPollingChangeTimerMsg(&cMsg);
						return;
					}
				}
			}
			//20101111_tzy VCSԤ��ģʽ�¶�Ҫ�����ն����õ���Ӧ����ǽ��				
			SetMtInTvWallAndHduInFailPresetinAndInReviewMode( tPreSetInRsp );
			
		}
		break;
	case MCS_MCU_START_SWITCH_TW_REQ:
		{

			TSpyTVWallInfo* ptSpyTvWallInfo = &tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo;
			
			if (ptSpyTvWallInfo->m_bySrcMtType == TW_MEMBERTYPE_TWPOLL)
			{
				//����ǵ���ǽ��ͨ����ѯ,ֱ���������ն�
				//20110610_tzy Bug00055651������ش����飬�¼��ش���������������£����¼����ն˽��е���ǽ��ѯ����ѯ˳������
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(ptSpyTvWallInfo->m_tTvWall.GetEqpId(), ptSpyTvWallInfo->m_byDstChlIdx, dwTimerIdx) )
				{
					TTvWallPollParam *ptTWPollParam = m_tTWMutiPollParam.GetTWPollParamByTimerIdx((u8)dwTimerIdx);

					u8 byCurPollPos     = ptTWPollParam->GetCurrentIdx();					
					u8 byTWId           = ptTWPollParam->GetTvWall().GetEqpId();
					u8 byChnId          = ptTWPollParam->GetTWChnnl();
					u8 byIsStartAsPause = ptTWPollParam->GetIsStartAsPause();
					
					

					if ( 0 == byIsStartAsPause)   
					{
						byCurPollPos++;
					}
					else // ����ͣ�ָ�ʱ�����ŵ�ǰ�ն���ѯ   
					{
						m_tTWMutiPollParam.SetIsStartAsPause(byTWId, byChnId, 0);
					}

					TMtPollParam *ptCurPollMt = GetMtTWPollParam(byCurPollPos, byTWId, byChnId);

					CMessage cMsg;
					if( NULL != ptCurPollMt && (TMt)(*ptCurPollMt) == tSrc )
					{						
						cServMsg.SetEventId( MCS_MCU_STOPTWPOLL_CMD );
						cServMsg.SetMsgBody( &byTWId,sizeof(byTWId) );
						cServMsg.CatMsgBody( &byChnId,sizeof(byChnId) );
						cMsg.event = MCS_MCU_STOPTWPOLL_CMD;
						cMsg.content = cServMsg.GetServMsg();
						cMsg.length  = cServMsg.GetServMsgLen();
						ProcMcsMcuTWPollMsg( &cMsg );
					}
					else
					{					
						KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
						CMessage cMsg;
						memset(&cMsg, 0, sizeof(cMsg));
						cMsg.event = u16(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
						cMsg.content = (u8*)&dwTimerIdx;
						cMsg.length  = sizeof(u32);
						ProcTWPollingChangeTimerMsg(&cMsg);
						return;
					}
				}

				
			}
			
			//20101111_tzy VCSԤ��ģʽ�¶�Ҫ�����ն����õ���Ӧ����ǽ��				
			SetMtInTvWallAndHduInFailPresetinAndInReviewMode( tPreSetInRsp );	
		}
		break;

	case MCS_MCU_STARTVMP_REQ:
		{
			TEqp tVmpEqp = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_tVmp;
			// ALLVMPPRESETIN_ACK_TIMERʧЧ
			u8 byVmpIdx = tVmpEqp.GetEqpId() - VMPID_MIN;
			KillTimer(MCUVC_WAIT_ALLVMPPRESETIN_ACK_TIMER+byVmpIdx);


			TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
			if( TVmpStatus::RESERVE == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState )
			{
				tPeriEqpStatus.SetConfIdx( 0 );
				tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
				g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
			}		
			

			TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
			// ����ն�ռ�õ�ǰ����,AdjustVmpParam�����Getmtformat,��Ϊ�¼��ǿƴ�MT,���ܻ���ռǰ����,������Ҫ����
			tVmpChnnlInfo.ClearChnlByMt(tSrc);
			
			g_cMcuVcApp.GetPeriEqpStatus(tVmpEqp.GetEqpId() , &tPeriEqpStatus);
			TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			TVMPParam_25Mem tEqpStatusVmpParam = tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam();// = g_cMcuVcApp.GetLastVmpParam(m_tVmpEqp);
			//zhouyiliang20100910���Ӹ�������ȷ��param��û���������ý�ȥ���������presetin��Ա
			if ( tConfVmpParam.IsMtInMember( tSrc ) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMcuMcuSpyBWFull] tSrc is a member of tConfVmpParam. \n");
			}
			if (tEqpStatusVmpParam.IsMtInMember( tSrc ))
			{
				u8 byChl = tEqpStatusVmpParam.GetChlOfMtInMember( tSrc );	
				//zhouyiliang 20100902 ����������ˣ��ָ�curUsechn����style
				TVMPMember *pVmpMember = tConfVmpParam.GetVmpMember(byChl);
				
				if (pVmpMember != NULL)
				{
					tEqpStatusVmpParam.SetVmpMember(byChl, *pVmpMember);
				}
				else
				{
					tEqpStatusVmpParam.ClearVmpMember( byChl );
				}
			}
			
			// vmp��ͨ����ѯʱ��������,������ѯ��һ�ն�
			if (POLL_STATE_NONE != m_tVmpPollParam.GetPollState() && 
				VMP_MEMBERTYPE_VMPCHLPOLL == tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byMemberType)
			{
				TVmpPollInfo tPollInfo = m_tVmpPollParam.GetVmpPollInfo();
				u8 byPollIdx = m_tVmpPollParam.GetCurrentIdx();
				TMtPollParam * ptCurPollMt = GetNextMtPolledForVmp(byPollIdx, tPollInfo);
				// ��Ҫ��ѯ����һ���ն˾��ǵ�ǰ�ն�,��ʾ���д��ն�������ѯ����,�����ն��ֲ��ʺϽ�vmp,ͣ��ѯ
				if (!ptCurPollMt || tSrc == *ptCurPollMt )
				{
					ProcStopVmpPoll();
				} else
				{
					// ����ͨ��Ϊvmp��ͨ����ѯʱ,��Ҫ������ѯ��һ�ն�
					SetTimer(MCUVC_VMPPOLLING_CHANGE_TIMER, 10);
					bAlarm = FALSE;
				}
			}

			if ( tConfVmpParam.IsVMPAuto() )
			{
				u8 byStyl = tConfVmpParam.GetVMPStyle();
				tEqpStatusVmpParam.SetVMPStyle( byStyl );
			}
			//�ָ�presetin֮ǰ��curUseVmpChanInd���籾��Ҫ��4���棬ͨ������ֻ��3���棬curUseVmpChan�Ͳ�����1������ԭ����3��
			if ( VCS_CONF == m_tConf.GetConfSource() ) 
			{
				tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tEqpStatusVmpParam);
				g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u16 byCurUseChnId = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos;
				m_cVCSConfStatus.SetCurUseVMPChanInd( byCurUseChnId );
				TMt tMtNull;
				tMtNull.SetNull();
				m_cVCSConfStatus.SetReqVCMT( tMtNull );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMcuMcuSpyBWFull] Update vcs CurUseVMPChanInd.%d:\n",byCurUseChnId);
			}
		
			cServMsg.SetEqpId(tVmpEqp.GetEqpId());
			cServMsg.SetMsgBody( (u8*)&tConfVmpParam, sizeof(tConfVmpParam) );
			SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg ); //���߻��ˢ����		
			
			if (m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP)
			{
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_SPY,"[ProcMcuMcuSpyBWFull]Recover Rollcall finish Mark\n");
				SetLastMultiSpyRollCallFinish(TRUE);
			}

// 			TPeriEqpStatus tOldStatus;
// 			g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tOldStatus );
// 			tOldStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
// 			//�������ϲ�������û��һ����Ա����vmpͣ��
// 			if (m_tConf.m_tStatus.m_tVMPParam.GetVMPMemberNum() == 0) 
// 			{
// 				CServMsg cTempServ;
// 				SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cTempServ); 
// 				//��������ϳ���
// 				tOldStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
// 				tOldStatus.SetConfIdx( 0 );
// 				if (m_tConf.m_tStatus.GetVmpParam().IsVMPBrdst())
// 				{
// 					ChangeVidBrdSrc(NULL);
// 				}
// 			}
// 			g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tOldStatus );
// 			cServMsg.SetMsgBody((u8 *)&tOldStatus, sizeof(tOldStatus));
// 			SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
		}
		break;

	case MT_MCU_STARTSELMT_CMD:			 //��׼ѡ������
	case MT_MCU_STARTSELMT_REQ:          
	case MCS_MCU_STARTSWITCHMT_REQ:	     //����̨Ҫ�󽻻�����
	case MCS_MCU_STARTSWITCHMC_REQ:      //���ѡ���ն�
		{
			break;
		}
	case VCS_MCU_CONFSELMT_CMD:
		{
			break;
		}
	case MCS_MCU_SPECSPEAKER_REQ:        //�������ָ̨��һ̨�ն˷���
	case MT_MCU_SPECSPEAKER_REQ:		 //��ͨ�ն�ָ������������
	case MT_MCU_SPECSPEAKER_CMD:
		{
			if( NULL != ptPreSetInRsp &&
				VCS_CONF == m_tConf.GetConfSource() &&
				MCS_MCU_SPECSPEAKER_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() &&
				CONF_SPEAKMODE_ANSWERINSTANTLY == m_tConf.GetConfSpeakMode() &&
				!m_tApplySpeakQue.IsQueueNull()
				)
			{				
				TMt tVCMT;
				TMt tCurMt = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
				if(  m_tApplySpeakQue.GetQueueNextMt( tCurMt,tVCMT ) )
				{
					ChgCurVCMT( tVCMT );	
				}
				else
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuSpyBWFull] Fail to get Queue NextMt.CurMt(%d.%d)\n",
						tCurMt.GetMcuId(),tCurMt.GetMtId()
						);
				}
			}
			break;
		}

	case MCS_MCU_STARTREC_REQ:
		{
			// ���vrs��¼������Ҷ�vrsʵ��
			u8 byVrsMtId = GetVrsRecMtId(tSrc);
			if (byVrsMtId > 0)
			{
				if (m_ptMtTable->GetRecChlType(byVrsMtId) == TRecChnnlStatus::TYPE_RECORD
					&& m_ptMtTable->GetRecChlState(byVrsMtId) != TRecChnnlStatus::STATE_IDLE)
				{
					ReleaseVrsMt(byVrsMtId);
				}
			}
			break;
		}

		
	case MCS_MCU_CHANGEROLLCALL_REQ:
	//����ʧ��,�ָ���������
		if( MCS_MCU_CHANGEROLLCALL_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() )
		{
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_SPY,"[ProcMcuMcuSpyBWFull]Recover Rollcall finish Mark\n");
			SetLastMultiSpyRollCallFinish(TRUE);
			m_tRollCallee = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tOldCallee;
			//zhouyiliang 20110118 �����¼��ն�,�¼���ack,�ش���������LastRollCallFinishedҲҪ�û�true���ڼ�ROLLCALL_MODE_VMPģʽ������
			//���л��������ˣ���ֹʱ������
			if (m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP) 
			{
// 				m_tConfInStatus.SetLastVmpRollCallFinished(TRUE);
				//zhouyiliang 20110330�����һ�ε����Ͳ��ɹ�����ʱӦ�ý�������Ҳ��գ�conf��vmpparamĬ��memsetΪ0
				TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
				if ( tConfVmpParam.GetVMPStyle() == VMP_STYLE_NONE || 
					 tConfVmpParam.GetVMPStyle() == 0
					) 
				{
					m_tRollCaller.SetNull();
				}
			}
		}
		break;
		
	default:
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyBWFull] Default Msg.%d %s!\n", tPreSetInRsp.m_tSetInReqInfo.GetEvId() ,OspEventDesc(u16(tPreSetInRsp.m_tSetInReqInfo.GetEvId())));
		}
		break;
		//return;
	}

	if( bAlarm )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyBWFull] SendErrorAlarm to UI,error:%d\n", wErrorCode);

		cServMsg.SetEventId( MCU_MCS_ALARMINFO_NOTIF );
		//[2011/10/26/zhangli]���ݴ�����������ʾ
		cServMsg.SetErrorCode( wErrorCode );	
		SendMsgToAllMcs( MCU_MCS_ALARMINFO_NOTIF, cServMsg );
	}

	OnPresetinFailed( tSrc );	
}

/*==============================================================================
������    :  ProcMcuMcuPreSetInAck
����      :  �ϼ������¼�������PreSetInAck����������Ҫ�Ƿ���ش������ش�ͨ����
�㷨ʵ��  :  
����˵��  :  [IN] const CMessage
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010/07/19	 4.6		   ½����						   ��ش�����
==============================================================================*/
void CMcuVcInst::ProcMcuMcuPreSetInAck( const CMessage *pcMsg )
{
	STATECHECK;
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	CServMsg cMsg;
	
	TPreSetInRsp tPreSetInRsp = *(TPreSetInRsp *)(cServMsg.GetMsgBody());
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TPreSetInRsp));
	TMt tOrgMt = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	TMt tSrc = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	u8  bySpyMode = tPreSetInRsp.m_byRspSpyMode;
	u8  byIsNeedAck = TRUE;
	BOOL32 bIsPreAdd = FALSE;

	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if(tConfAttrb.IsResendLosePack())
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuPreSetInAck][AUDIO	RTCP-ACK][entry of function]%s@%d\n", StrOfIP(tPreSetInRsp.GetAudSpyRtcpAddr().GetIpAddr()),
			tPreSetInRsp.GetAudSpyRtcpAddr().GetPort());
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuPreSetInAck][VIDEO RTCP-ACK]%s@%d\n", StrOfIP(tPreSetInRsp.GetVidSpyRtcpAddr().GetIpAddr()),
						tPreSetInRsp.GetVidSpyRtcpAddr().GetPort());
	}

	if( !m_tCascadeMMCU.IsNull() &&
		tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
		tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		bIsPreAdd = TRUE;
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuPreSetInAck] bySpyMode.%d EventDesc(%s)!\n",bySpyMode,
		OspEventDesc(u16(tPreSetInRsp.m_tSetInReqInfo.GetEvId()))
		);

	//�ݴ��жϻش�Դ��NULL
	if( tSrc.IsNull() )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "ProcMcuMcuPreSetInAck False, tSrc is NULL!\n" );
		return;
	}

	//�ݴ��жϻش�ģʽ��MODE_NONE
	if( bySpyMode == MODE_NONE )
	{
        ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "ProcMcuMcuPreSetInAck False, SpyMode == MODE_NONE!\n" );
		return;
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuPreSetInAck]The MMcu Have SpyVidBW: %d, SpyAudBW: %d, SpyMode: %d\n", tPreSetInRsp.GetVidSpyBW(), tPreSetInRsp.GetAudSpyBW(), bySpyMode );
	//��ȡ��mcu֧�ֵ��ܴ���ʣ�����
	u32 dwMaxSpyBW = 0;
	s32 nRemainSpyBW = 0;

	//zjj20100804 Ҫ��mtadp��������TMtת����McuIdx��ʾ��TMt
	u16 wSrcMcuIdx = GetMcuIdxFromMcuId( (u8)tSrc.GetMcuId() );	
	tSrc.SetMcuIdx( wSrcMcuIdx );	

	m_ptConfOtherMcTable->GetMultiSpyBW(tSrc.GetMcuId(), dwMaxSpyBW, nRemainSpyBW);
	tSrc = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc, tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() );
	tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt( tSrc );

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuPreSetInAck]Before Mcu.%u dwMaxSpyBW:%d, nRemainSpyBW:%d\n", 
		tSrc.GetMcuId(), dwMaxSpyBW,nRemainSpyBW ); 

	//zhouyiliang 20121221 �¼�bas��Դ���㣬���ܽ�bothģʽ��Ϊaudioģʽ����ʱ��������Ϊ���Ȳ��ɹ�
	if ( tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode != bySpyMode &&  
		( MCS_MCU_SPECSPEAKER_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() ||
		MT_MCU_SPECSPEAKER_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() || 
		MT_MCU_SPECSPEAKER_CMD == tPreSetInRsp.m_tSetInReqInfo.GetEvId()
		)
		)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "ProcMcuMcuPreSetInAck SpecSpeakerReq , ackSpyMode == %d,reqspymod=%d!\n", bySpyMode,tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode);
		OnMMcuBanishSpyCmd( tSrc, bySpyMode, bySpyMode );
		//Ԥռ�ñ�ʾ����Ҫ��nack��Ϣ�����ϼ�����Ϣ��û���͵�Դͷ
		if( bIsPreAdd )
		{
			tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt(tOrgMt);
			cMsg.SetErrorCode( ERR_MCU_CASDBASISNOTENOUGH );
			cMsg.SetEventId( MCU_MCU_PRESETIN_NACK );
			cMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
			cMsg.CatMsgBody( (u8*)&tPreSetInRsp,sizeof(TPreSetInRsp) );
			SendMsgToMt( m_tCascadeMMCU.GetMtId(),cMsg.GetEventId(),cMsg );
			
			return;
		}
		//[2011/10/26/zhangli]�������ţ����ݴ�����������ʾ
		ProcMcuMcuSpyBWFull(pcMsg ,&tPreSetInRsp, ERR_MCU_CASDBASISNOTENOUGH);
		return;
	}

	s16 swIndex = m_cSMcuSpyMana.FindSpyMt(tSrc);
	CRecvSpy tSrcSpyInfo;
	//����˻ش�Դ�Ѿ��ڻش��б���,��ʹ��״̬�Ǵ��ͷţ���ֱ�ӷ���
	if (-1 != swIndex)
	{
		m_cSMcuSpyMana.GetRecvSpy(swIndex, tSrcSpyInfo);
		if( tSrcSpyInfo.m_byUseState == TSpyStatus::WAIT_FREE &&
			(  VCS_CONF != m_tConf.GetConfSource() ||
				 ( CONF_CREATE_MT != m_byCreateBy &&
						m_cVCSConfStatus.GetTVWallManageMode() != VCS_TVWALLMANAGE_AUTO_MODE
					 ) 
					||
				(
				 MCS_MCU_START_SWITCH_HDU_REQ != tPreSetInRsp.m_tSetInReqInfo.GetEvId() &&
				 MCS_MCU_START_SWITCH_TW_REQ != tPreSetInRsp.m_tSetInReqInfo.GetEvId() 
				)
				||
				tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt.IsNull() || 
				tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.GetCount() < 0 ||
				GetFstMcuIdFromMcuIdx(tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId()) != 
					tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt.GetMcuId()
				)
			)
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuPreSetInAck] Fail to Presetin,SpySrcMt(%d.%d) Now Status  is WAIT_FREE!\n",
						tSrc.GetMcuId(),
						tSrc.GetMtId()
						);
			if( !m_tCascadeMMCU.IsNull() &&
					tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
					tHeadMsg.m_tMsgDst.m_byCasLevel > 0
					)
			{
				tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt(tOrgMt);
				cMsg.SetEventId( MCU_MCU_PRESETIN_NACK );
				cMsg.SetErrorCode( ERR_MCU_CONFSNDBANDWIDTHISFULL );
				cMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
				cMsg.CatMsgBody( (u8*)&tPreSetInRsp,sizeof(TPreSetInRsp) );
				SendMsgToMt( m_tCascadeMMCU.GetMtId(),cMsg.GetEventId(),cMsg );
			}
			return;
		}
	}

	// [9/20/2011 liuxu] ����ǽҵ�����⴦��, ����ش�ģʽ��������Ƶ,��ܾ�
	if ( !bIsPreAdd 
		&& (MCS_MCU_START_SWITCH_TW_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId()
			|| MCS_MCU_START_SWITCH_HDU_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId()
			|| MCUVC_STARTSWITCHHDU_NOTIFY == tPreSetInRsp.m_tSetInReqInfo.GetEvId()))
	{
		if (bySpyMode != MODE_BOTH && bySpyMode != MODE_VIDEO)
		{
			OnMMcuBanishSpyCmd( tSrc, bySpyMode, bySpyMode );
			//[2011/10/26/zhangli]�������ţ����ݴ�����������ʾ
			ProcMcuMcuSpyBWFull(pcMsg ,&tPreSetInRsp, cServMsg.GetErrorCode());
			return;
		}
	}

	//����˴λش�����Ҫ�Ĵ���
	s32 nTotalBW = 0;
	if (bySpyMode == MODE_BOTH || bySpyMode == MODE_VIDEO)
	{
		nTotalBW += (s32)tPreSetInRsp.GetVidSpyBW();
		//����Ѿ��ڻش����ܹ���Ҫ�Ļش�Ҫ�����Ѿ����ڵĻش�����
		nTotalBW -= (s32)tSrcSpyInfo.m_dwVidBW;
	}

	if (bySpyMode == MODE_BOTH || bySpyMode == MODE_AUDIO)
	{
		nTotalBW += (s32)tPreSetInRsp.GetAudSpyBW();
		//����Ѿ��ڻش����ܹ���Ҫ�Ļش�Ҫ�����Ѿ����ڵĻش�����
		nTotalBW -= (s32)tSrcSpyInfo.m_dwAudBW;
	}

	//����һ�´˻ش��Ƿ���Ҫ����Ŀ���������ͷŶ��ٴ���
	u32 dwCanReleaseBW = 0;
	u8 byAddDestSpyNum = 1;
	s16 swCanRelIndex = -1;
	
	//�˴λش��Ĵ������ʣ�����ӿ��ͷŴ���֪ͨmcs�¼��ش���������
	if( nTotalBW > nRemainSpyBW )
	{
		EvaluateSpyFromEvent(tPreSetInRsp, byAddDestSpyNum, dwCanReleaseBW, swCanRelIndex);

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "ProcMcuMcuPreSetInAck McuId:%d MtId:%d AddDest:%d CanRBW:%d\n", 
		tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId(),
		tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMtId(),
		byAddDestSpyNum, dwCanReleaseBW);

	
		if (nTotalBW > nRemainSpyBW + (s32)dwCanReleaseBW)
		{
			u8 bySpyNoUse = 0;
			bySpyNoUse |= bySpyMode;
			
			// [8/17/2011 liuxu] �Ѿ��ڻش�ҵ���,����Ҫ�ͷ�
			if( -1 != swIndex )
			{
				if (tSrcSpyInfo.m_byVSpyDstNum)
				{
					bySpyNoUse &= ~MODE_VIDEO;
				}
				
				if (tSrcSpyInfo.m_byASpyDstNum)
				{
					bySpyNoUse &= ~MODE_AUDIO;
				}
			}				
						
			OnMMcuBanishSpyCmd( tSrc,tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode, bySpyNoUse);
			
			//Ԥռ�ñ�ʾ����Ҫ��nack��Ϣ�����ϼ�����Ϣ��û���͵�Դͷ
			if( bIsPreAdd )
			{
				tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt(tOrgMt);
				cMsg.SetErrorCode( ERR_MCU_CONFSNDBANDWIDTHISFULL );
				cMsg.SetEventId( MCU_MCU_PRESETIN_NACK );
				cMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
				cMsg.CatMsgBody( (u8*)&tPreSetInRsp,sizeof(TPreSetInRsp) );
				SendMsgToMt( m_tCascadeMMCU.GetMtId(),cMsg.GetEventId(),cMsg );
				
				return;
			}
			ProcMcuMcuSpyBWFull(pcMsg ,&tPreSetInRsp);
			return;
			
		}
		else
		{
			tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsNeedRelease = 1;
		}

	}

	

	//����¼��Ѿ������ͷ��ն���ռ��bas��Ϊ������,��ͣ�����еļ��,ֻ�е���ҵ���vcs���漰
	//ȷ���ͼ����������ҵ��������ͷſ��ͷ��ն˴Ӷ��ͷ��¼�basͨ��
	if( !bIsPreAdd &&
		MODE_NONE != tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsReuseBasChl &&
		!tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt.IsNull() )
	{
		TMultiCacMtInfo tMtInfo;
		tMtInfo.m_byCasLevel = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCasLevel;
		memcpy( &tMtInfo.m_abyMtIdentify[0],
			&tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_abyMtIdentify[0],
			sizeof(tMtInfo.m_abyMtIdentify)
			);

		TMt tCanReleaseMt = GetMtFromMultiCascadeMtInfo( tMtInfo,
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt
						);		
		
		if( MCS_MCU_CHANGEROLLCALL_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() 
			|| VCS_CONF == m_tConf.GetConfSource() )
		{
			s16 swIndexRelMt = m_cSMcuSpyMana.FindSpyMt(tCanReleaseMt/*tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt()*/);
				
			if (-1 != swIndexRelMt)
			{
				CRecvSpy tRelMtSrcSpyInfo;
				m_cSMcuSpyMana.GetRecvSpy(swIndexRelMt, tRelMtSrcSpyInfo);
				if( tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum < tRelMtSrcSpyInfo.m_byVSpyDstNum ||
					tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum < tRelMtSrcSpyInfo.m_byASpyDstNum)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuPreSetInAck]Fail Spy Mt.Smcu(%d) is reuse bas chl,but mt(%d.%d) DstNum(V:%d,A:%d) is bigger than relMt(%d.%d.%d,level.%d) DstNum(V:%d,A:%d)!\n",
						cServMsg.GetSrcMtId(),tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId(),
						tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMtId(), 
						tRelMtSrcSpyInfo.m_byVSpyDstNum,tRelMtSrcSpyInfo.m_byASpyDstNum,
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt.GetMcuId(),
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt.GetMtId(),						
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_abyMtIdentify[0],
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCasLevel,
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum,
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum);
					//[2011/11/21/zhangli]�ͷŴ�����������Ϊ������Դ���㵼��ҵ��ʧ�ܣ��������������Դ����
					OnMMcuBanishSpyCmd(tSrc, tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode, tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode);
					ProcMcuMcuSpyBWFull(pcMsg ,&tPreSetInRsp, ERR_MCU_CASDBASISNOTENOUGH);
					return;
				}
			}
			
// 			u16 wMcIndex = 0;
// 			if (VCS_CONF == m_tConf.GetConfSource())
// 			{
// 				wMcIndex = m_cVCSConfStatus.GetCurSrcSsnId();
// 			}
// 			else
// 			{
// 				wMcIndex = g_cMcuVcApp.GetMcIns(tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.GetMcIp(), 
// 												tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.GetMcSSRC(),
// 												m_tConf.GetConfSource());
// 			}
			//u8 byMcIndex = VCS_CONF == m_tConf.GetConfSource() ? m_cVCSConfStatus.GetCurSrcSsnId() : tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_byMcInstId;
		
			TMt tMcSrc;	
			BOOL32 bIsNeedReGetBW = FALSE;
// 			for( u8 byLoop = 0; byLoop < MAXNUM_MC_CHANNL; byLoop++ ) 
// 			{
// 				if ( g_cMcuVcApp.GetMcSrc( wMcIndex, &tMcSrc, byLoop, MODE_VIDEO ) ) 
// 				{
// 					if( !tMcSrc.IsNull() &&  ( tMcSrc == tCanReleaseMt/*tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt()*/ ) )
// 					{
// 						StopSwitchToMc( wMcIndex,byLoop,TRUE,MODE_VIDEO );		
// 						bIsNeedReGetBW = TRUE;
// 					}
// 				}
// 			}		
			//[2011/11/24/zhangli]���vcs��ͬһmcu����Ҫͣ����vcs�ļ��
			for (u16 byMcIndex = MAXNUM_MCU_VC; byMcIndex <= MAXNUM_MCU_MC + MAXNUM_MCU_VC; ++byMcIndex)
			{
				for( u8 byLoop = 0; byLoop < MAXNUM_MC_CHANNL; byLoop++ ) 
				{
					if ( g_cMcuVcApp.GetMcSrc(byMcIndex, &tMcSrc, byLoop, MODE_VIDEO)) 
					{
						if(!tMcSrc.IsNull() &&  (tMcSrc == tCanReleaseMt))
						{
							StopSwitchToMc(byMcIndex, byLoop, TRUE, MODE_VIDEO);
							bIsNeedReGetBW = TRUE;
						}
					}
					
					if ( g_cMcuVcApp.GetMcSrc(byMcIndex, &tMcSrc, byLoop, MODE_AUDIO)) 
					{
						if(!tMcSrc.IsNull() &&  (tMcSrc == tCanReleaseMt))
						{
							StopSwitchToMc(byMcIndex, byLoop, TRUE, MODE_AUDIO);
							bIsNeedReGetBW = TRUE;
						}
					}
				}
			}

			if( bIsNeedReGetBW )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuPreSetInAck] Stop All Mc And Need ReGetBW.tSrc(%d.%d) tCanReleaseMt(%d.%d)\n",
										tSrc.GetMcuId(),tSrc.GetMtId(),tCanReleaseMt.GetMcuId(),tCanReleaseMt.GetMtId() );
					
				GetMcuMultiSpyBW(tCanReleaseMt.GetMcuId(), dwMaxSpyBW, nRemainSpyBW);
			}
		}
		else
		{
			//zjj20110124 �����㹻,��ͬһ�¼����ն�Ҫ�����Ѿ��ϴ���ͬһ�¼��Ŀ��ͷ��ն˵�ռ�õ�basͨ��,����޷��ͷſ��ͷ��ն˾Ͳ����ϴ��ɹ�
			if( GetFstMcuIdFromMcuIdx( tCanReleaseMt.GetMcuId() ) == GetFstMcuIdFromMcuIdx( tSrc.GetMcuId() ))
			{
				s16 swRelIndex = -1;
				u32 dwReleaseBW = 0;
				CRecvSpy cRecvSpy;
				if (m_cSMcuSpyMana.GetRecvSpy(tCanReleaseMt, cRecvSpy))
				{
					//[2011/11/18/zhangli]byReleaseBasMode������MODE_VIDEO��MODE_AUDIO��MODE_BOTH����ֵ�������ģʽ�µ�bas�ɸ���
					//ֻ���ж�ĳ��ģʽ�µ�bas���ͷż��ɡ��������¼�ֻ��һ·������������
					//��-MT1ѡ����-MT1��Ƶ����-MT2ѡ����-MT1����Ƶ����Ƶ�����䣬Ȼ����-MT2ѡ����-MT2�޷��滻ѡ��
					u8 byReleaseBasMode = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsReuseBasChl;
					u8 byReleaseVideoNum = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
					u8 byReleaseAduioNum = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
					if (MODE_VIDEO == byReleaseBasMode)
					{
						byReleaseAduioNum = cRecvSpy.m_byASpyDstNum;
					}
					else if (MODE_AUDIO == byReleaseBasMode)
					{
						byReleaseVideoNum = cRecvSpy.m_byVSpyDstNum;
					}
					
					m_cSMcuSpyMana.IsCanFree(tCanReleaseMt, tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseMode, 
						byReleaseVideoNum, byReleaseAduioNum, dwReleaseBW, swRelIndex);
				}
				
				if( -1 == swRelIndex )
				{		
					ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[ProcMcuMcuPreSetInAck] Fai to Spy tSrc(%d.%d),because it cann't use tMt's(%d.%d) Bas\n",
										tSrc.GetMcuId(),tSrc.GetMtId(),tCanReleaseMt.GetMcuId(),tCanReleaseMt.GetMtId() );
					
					OnMMcuBanishSpyCmd( tSrc,tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode, MODE_NONE );
					ProcMcuMcuSpyBWFull(pcMsg, &tPreSetInRsp, ERR_MCU_CASDBASISNOTENOUGH);					
					return;
				}
			}
		}
	}

	//����˻ش�Դ�Ѿ��ڻش��б���
	if (-1 != swIndex)
	{
		//���SpyMode��֮ǰ�Ĳ�һ��
		if(tSrcSpyInfo.m_bySpyMode != bySpyMode)
		{
			//�����µ�SpyMode
			m_cSMcuSpyMana.AddSpyMode(swIndex, bySpyMode, bIsPreAdd);
			//�޸��µ�SpyMode��Ӧ�Ļش�����
			m_cSMcuSpyMana.ModifySpyModeBW(swIndex, bySpyMode, tPreSetInRsp.GetVidSpyBW(), tPreSetInRsp.GetAudSpyBW());
		}
	}
	else
	{
		//����п������õ�ͨ�������ø�ͨ��״̬Ϊ���ͷ�(�����ٶԸö˿ڵ��ն˵Ķ�ش������;ܾ�)
		if( swCanRelIndex != -1)
		{
			m_cSMcuSpyMana.GetRecvSpy(swCanRelIndex, tSrcSpyInfo);
			
			//zjj20100810 �������ö˿�
			m_cSMcuSpyMana.ModifyUseState( swCanRelIndex,TSpyStatus::WAIT_FREE );			
		}

		//����˻ش�Դ���ڴ˻ش��б���,����˻ش�Դͨ������������m_cSMcuSpyMana�д�ŵ�index
		if (!AddSpyChnnlInfo(tSrc, swIndex, bIsPreAdd))
		{
			//�˴�����ErrorCode����ʾ�ش�ͨ���Ѿ�մ�������ܷ���
			return;
		}

		//�����µ�SpyMode
		m_cSMcuSpyMana.AddSpyMode(swIndex, bySpyMode, bIsPreAdd);
		//�޸��µ�SpyMode��Ӧ�Ļش�����
		m_cSMcuSpyMana.ModifySpyModeBW(swIndex, bySpyMode, tPreSetInRsp.GetVidSpyBW(), tPreSetInRsp.GetAudSpyBW());
	}

	//ֻ����Ҫ���õĴ������0������Ҫ����ʣ�����
	//����˵�����ն��Ѿ��ڻش��У��Ҵ��ڻ���ڴ˴�����Ҫ�Ļش�����
	if (nTotalBW > 0)
	{
		//����һ��ʣ���������û�м�����ܻ��ͷŵĴ������Դ˴�ʣ�������ܻ��Ǹ���
		m_ptConfOtherMcTable->SetMcuSupMultSpyRemainBW(wSrcMcuIdx/*tSrc.GetMcuId()*/, nRemainSpyBW - (s32)nTotalBW);

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "PreSetInAck:After Mcu.%u MaxSpyBW:%d, RemainSpyBW:%d\n", 
		tSrc.GetMcuId(), dwMaxSpyBW, nRemainSpyBW - (s32)nTotalBW ); 
	}

	//���ش��ն˶�Ӧ��SpyMode�ն�Ŀ������һ
	if (byAddDestSpyNum != 0)
	{
		m_cSMcuSpyMana.IncSpyModeDst(swIndex, bySpyMode, byAddDestSpyNum, bIsPreAdd);
	}

	if( !m_tCascadeMMCU.IsNull() &&
		tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
		tHeadMsg.m_tMsgDst.m_byCasLevel > 0
		)
	{
		tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt(tOrgMt);
		//[2011/10/27/zhangli]�Ѵ���ŷ��ظ��ϼ�
		cMsg.SetErrorCode(cServMsg.GetErrorCode());
		cMsg.SetEventId( MCU_MCU_PRESETIN_ACK );
		cMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		cMsg.CatMsgBody( (u8*)&tPreSetInRsp,sizeof(TPreSetInRsp) );
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),cMsg.GetEventId(),cMsg );
		return;
	}

	//�����ϼ��ش�ʣ���߼�
	ProcUnfinishedCascadEvent( tPreSetInRsp );

	//[2011/09/06]���������Դ���㣬������ʾ
	if(ERR_MCU_CASDBASISNOTENOUGH == cServMsg.GetErrorCode())
	{
		NotifyMcsAlarmInfo(0, ERR_MCU_CASDBASISNOTENOUGH);
	}

	if ( VCS_CONF == m_tConf.GetConfSource() 
		&& ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode())
		&& (tPreSetInRsp.m_tSetInReqInfo.GetEvId() == VCS_MCU_CONFSELMT_CMD
			|| ( tPreSetInRsp.m_tSetInReqInfo.GetEvId() == MCS_MCU_STARTVMP_REQ 
				&& ( m_cVCSConfStatus.GetCurVCMode() != VCS_GROUPROLLCALL_MODE || VCS_POLL_STOP != m_cVCSConfStatus.GetChairPollState() ))
		|| tPreSetInRsp.m_tSetInReqInfo.GetEvId() == MCS_MCU_SPECSPEAKER_REQ)
		&& !( tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() == m_cVCSConfStatus.GetCurVCMT() )	
		)
	{
		//�л���ǰ�����ն�
		TMt tNewCurVCMT = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
		TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();
		u8 byVCMode = m_cVCSConfStatus.GetCurVCMode();
		m_cVCSConfStatus.SetCurVCMT(tNewCurVCMT);

		//20110610_tzy Bug00055645 PDS��VCS������ش�������5������ǽ���ش�����Ϊ6·������ǽԤ��ģʽֻ�����¼��նˣ����ε��ȣ�����ʾ�ش���������
		//Ԥ��ģʽ�£�����ǽͨ�������ϵ����ն˵������ش�·��ʱ���µ����ն˵���������
		//Ӧ���ϵ����ն��ڼ���е���Ƶ����һ�飬�����ϵ����ն�δ�ͷŸɾ��������е��ն˽�ǽʧ��
		if ( m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE )
		{
			for( u8 byMcId = MAXNUM_MCU_MC+1;byMcId<= MAXNUM_MCU_MC + MAXNUM_MCU_VC;++byMcId )
			{
				TMt tMcSrc;
				g_cMcuVcApp.GetMcSrc( byMcId, &tMcSrc, VCS_VCMT_MCCHLIDX , MODE_VIDEO );
				if( tMcSrc == tCurVCMT )
				{				
					StopSwitchToMc( byMcId, VCS_VCMT_MCCHLIDX, TRUE, MODE_VIDEO );
				}
			}
		}

		//zhouyiliang 20101213 ��remove��Add����ֹ����һ�����������ʱadd���ɹ�
		// [9/29/2011 liuxu] �Ƶ�����ǽ����֮ǰ, ���������˵���ǽ��������presetinʱ���½�ǽ�ֳ�ǽ�����
		if( m_cVCSConfStatus.GetCurVCMode() != VCS_GROUPVMP_MODE 
			&& !tCurVCMT.IsNull() 
			&& ( !m_cVCSConfStatus.GetMtInTvWallCanMixing() || 
			( IsMtNotInOtherHduChnnl(tCurVCMT,0,0) && IsMtNotInOtherTvWallChnnl(tCurVCMT,0,0) ) )
			)
		{					
			RemoveSpecMixMember( &tCurVCMT, 1,FALSE,FALSE );
		}

		//zjj20101222 ��鵱ǰ�����ն˵Ļش�״̬�������WAIT_FREE˵������Ҫ���ȵ��ն�tNewCurVCMT�͵�ǰ�����ն��ǹ��ش�·����,
		//���ұ��ε��ȵ��ն����¼��Ѿ�Ԥռ�˵�ǰ�����ն˵Ĵ����͵ȵ�ǰ�����ն��ͷţ���������������µ�ǰ�����ն����Զ�����ǽ
		//ģʽ�½�ǽ�ǲ����Եģ���ΪҪ��֤���ε��ȵ��ն�������
		tSrcSpyInfo.Clear();
		swIndex = m_cSMcuSpyMana.FindSpyMt(tCurVCMT);
		if( -1 != swIndex )
		{
			m_cSMcuSpyMana.GetRecvSpy(swIndex, tSrcSpyInfo);

			if( m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_AUTO_MODE &&
				TSpyStatus::WAIT_FREE == tSrcSpyInfo.m_byUseState )
			{
				TEqp tEqp;
				TMt  tDropOutMt;
				tDropOutMt.SetNull();

				// [5/30/2011 liuxu] ��д��FindUsableTWChan, ��FindNextTvwChnnl����
 				u8 byChanIdx = 0;
				CConfTvwChnnl cNextTvwChnnl;
				u16 wNextChnnlIdx = FindNextTvwChnnl( m_cVCSConfStatus.GetCurUseTWChanInd(), &tCurVCMT, &cNextTvwChnnl, FALSE );
				
				// Hdu�豸�������
				BOOL32 bCheckHdu = IsValidHduChn( cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx() );
				bCheckHdu &= CheckHduAbility( tCurVCMT, cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx() );

				// ��ͨ����ǽ���
				BOOL32 bCheckTvw = IsValidTvw( cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx() );
				
				// ��ȡ��ͨ����Ա��ͨ����Ӧ���豸
				tDropOutMt = (TMt)cNextTvwChnnl.GetMember();
				tEqp = g_cMcuVcApp.GetEqp( cNextTvwChnnl.GetEqpId() );
				byChanIdx = cNextTvwChnnl.GetChnnlIdx();

				if( wNextChnnlIdx && (bCheckHdu || bCheckTvw) )
				{
					TVMPParam_25Mem tVmpparm = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
					if( !tDropOutMt.IsNull() 
						&& !( tDropOutMt == m_cVCSConfStatus.GetCurVCMT() ) 
						&& ( m_cVCSConfStatus.GetCurVCMode() != VCS_GROUPVMP_MODE || !tVmpparm.IsMtInMember( tDropOutMt ) ) &&
							GetFstMcuIdFromMcuIdx( tDropOutMt.GetMcuId() ) == GetFstMcuIdFromMcuIdx( tCurVCMT.GetMcuId() )
					  ) 
					{
						TTWSwitchInfo tSwitchInfo;
						tSwitchInfo.SetMemberType(TW_MEMBERTYPE_VCSAUTOSPEC);
						tSwitchInfo.SetSrcMt(tCurVCMT);
						tSwitchInfo.SetDstMt(tEqp);
						tSwitchInfo.SetDstChlIdx(byChanIdx);
						tSwitchInfo.SetMode(MODE_BOTH);
						if (!tCurVCMT.IsNull())
						{
							VCSConfMTToTW(tSwitchInfo);						
						}
					}
					else
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "[ProcMcuMcuPreSetInAck] tCurVCMT(%d.%d) now spy useState is WAIT_FREE and now in autoTvWall Mode,so not switch to TW.\n",
							tCurVCMT.GetMcuId(),tCurVCMT.GetMtId()
							);
						//20110107_tzy Bug00045312�Զ�����ǽ�ش�������ʱ����ʾ
						cMsg.SetEventId( MCU_MCS_ALARMINFO_NOTIF );
						cMsg.SetErrorCode( ERR_MCU_CONFSNDBANDWIDTHISFULL );	
						SendMsgToAllMcs( MCU_MCS_ALARMINFO_NOTIF, cMsg );
					}
				}
			}			
		}

		if ( !tCurVCMT.IsNull() 
			&& m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_AUTO_MODE 
			&& ( //20101223_tzy VCS�����Զ�����ǽģʽ��ԭ�����ն��Ǳ�������������ǽ
				tCurVCMT.IsLocal() 
				// [11/3/2011 liuxu] ���ش�ҲҪ��ǽ
				|| !IsLocalAndSMcuSupMultSpy( tCurVCMT.GetMcuId()) 
				//20101223_tzy ���ε��ȵ��ն�����Ǳ����ն˲���״̬����WAIT_FREE�Ž�ǽ������˵���¾ɵ����ն��ǹ�·����ԭ�����ն˾Ͳ���ǽ
				|| (-1 != swIndex && tSrcSpyInfo.m_byUseState != TSpyStatus::WAIT_FREE )
				// [11/3/2011 liuxu] ��ش�������ɵ����ն��Ѿ����ڻش�ͨ������,��Ҫ���½�ǽ
				|| (-1 == swIndex)
				)
			)
		{
			TEqp tEqp;
			TMt  tDropOutMt;
			tDropOutMt.SetNull();
			u8 byChanIdx = 0;

			CConfTvwChnnl cNextTvwChnnl;
			u16 wNextChnnlIdx = FindNextTvwChnnl( m_cVCSConfStatus.GetCurUseTWChanInd(), &tCurVCMT, &cNextTvwChnnl, FALSE );
			
			// Hdu�豸�������
			BOOL32 bCheckHdu = IsValidHduChn( cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx() );
			bCheckHdu &= CheckHduAbility( tCurVCMT, cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx() );
			
			// ��ͨ����ǽ���
			BOOL32 bCheckTvw = IsValidTvw( cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx() );
			
			// ��ȡ��ͨ����Ա��ͨ����Ӧ���豸
			tDropOutMt = (TMt)cNextTvwChnnl.GetMember();
			tEqp = g_cMcuVcApp.GetEqp( cNextTvwChnnl.GetEqpId() );
			byChanIdx = cNextTvwChnnl.GetChnnlIdx();

			if( wNextChnnlIdx && (bCheckHdu || bCheckTvw) )
			{
				if (!tDropOutMt.IsNull())
				{
					if (tEqp.GetEqpType() != EQP_TYPE_TVWALL)
					{
						// [2013/03/11 chenbing] VCS���鲻֧��HDU�໭��,��ͨ����0
						ChangeHduSwitch( NULL,tEqp.GetEqpId(),byChanIdx,0, TW_MEMBERTYPE_VCSAUTOSPEC, TW_STATE_STOP,MODE_BOTH,FALSE,FALSE );
					}
					else
					{
						ChangeTvWallSwitch( &tDropOutMt,tEqp.GetEqpId(),byChanIdx,TW_MEMBERTYPE_VCSAUTOSPEC, TW_STATE_STOP);
					}
				}

				if (!tCurVCMT.IsNull())
				{
					TTWSwitchInfo tSwitchInfo;
					tSwitchInfo.SetMemberType(TW_MEMBERTYPE_VCSAUTOSPEC);
					tSwitchInfo.SetSrcMt(tCurVCMT);
					tSwitchInfo.SetDstMt(tEqp);
					tSwitchInfo.SetDstChlIdx(byChanIdx);
					tSwitchInfo.SetMode(MODE_BOTH);

					BOOL32 bInToTvwRet = VCSConfMTToTW(tSwitchInfo);

					if (bInToTvwRet 
						|| tCurVCMT.IsLocal() 
						// [8/17/2011 liuxu] ���������鲻��û�н���ǽʱ, Ҳ��Ҫ
						|| !CheckHduAbility(tCurVCMT, tEqp.GetEqpId(), byChanIdx))
 					{
						m_cVCSConfStatus.SetCurUseTWChanInd(wNextChnnlIdx);
					}
				}
			}
		}
		
		AddSpecMixMember(&tNewCurVCMT, 1, TRUE);
	

		//��������ն��ڷ��Զ����ڣ���Ҫ�ӷ��Զ��������
		if( m_cVCSConfStatus.GetCurVCMT() == tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() &&
			m_tApplySpeakQue.IsMtInQueue( tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() )
			)
		{
			BOOL32 bIsSendToChairman = FALSE;
			if( !m_tApplySpeakQue.ProcQueueInfo( tSrc,bIsSendToChairman,FALSE ) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "[ProcVmpStartPreSetInAck] Fail to pop tMt(%d.%d). \n",
							tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId(),
							tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMtId()
							);
				bIsSendToChairman = FALSE;
			}
			NotifyMcsApplyList( bIsSendToChairman );
		}
		NotifyMtSpeakStatus( tNewCurVCMT, emAgreed );
		if( m_cVCSConfStatus.GetCurVCMode() != VCS_GROUPVMP_MODE )
		{
			NotifyMtSpeakStatus( tCurVCMT, emCanceled );
		}		
		TMt tNullMt;
		tNullMt.SetNull();

		KillTimer(MCUVC_VCMTOVERTIMER_TIMER);	
		m_cVCSConfStatus.SetReqVCMT(tNullMt);
		VCSConfStatusNotif();
	}
	
	return;
}


/*==============================================================================
������    :  AddSpyChnnlInfo
����      :  ��ӻش�ͨ��
�㷨ʵ��  :  
����˵��  :  const TMt &tMt [IN] �ն���Ϣ
			 s16 &swIndex	[OUT]������Ӻ�Ļش���Ϣ����
����ֵ˵��:  �Ƿ���ӳɹ�
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010/07/19	 4.6		   ½����						   ��ش�����
==============================================================================*/
BOOL32 CMcuVcInst::AddSpyChnnlInfo(const TMt &tMt, s16 &swIndex,BOOL32 bIsPreAdd/* = FALSE*/ )
{
	swIndex = m_cSMcuSpyMana.FindSpyMt(tMt);

	//�˴�ֻ�Ǳ����ظ����ͨ��,�������Ѿ��ڻش�ͨ���У�˵���߼��ж��д���
	if(-1 != swIndex)
	{
		return TRUE;
	}
	
	CMultiSpyMgr *pcMultiSpyMgr = g_cMcuVcApp.GetCMultiSpyMgr();
	u16 wSpyChannl;
	
	if( NULL == pcMultiSpyMgr )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "GetCMultiSpyMgr False! \n" );
		return FALSE;
	}
	if( !pcMultiSpyMgr->AssignSpyChnnl( wSpyChannl ) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "AssignSpyChnnl False! \n" );
		return FALSE;
	}

	return m_cSMcuSpyMana.AddSpyChnnlInfo(tMt, wSpyChannl * PORTSPAN + CASCADE_SPY_STARTPORT, swIndex,bIsPreAdd);
}

/*==============================================================================
������    :  SendMMcuSpyNotify
����      :  �ϼ����¼�����ش���Ϣ��֪ͨ
�㷨ʵ��  :  
����˵��  :  [IN] TPreSetInRsp ��ش���Ҫ��Ϣ
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010/07/19   4.6			½����							��ش�����
==============================================================================*/
void CMcuVcInst::SendMMcuSpyNotify( const TMt &tSpySrc, u32 dwEvId, TSimCapSet tDstCap/*u8 byRes = VIDEO_FORMAT_INVALID*/ )
{
	CServMsg cServMsg;
	cServMsg.SetEventId( MCU_MCU_SPYCHNNL_NOTIF );

	if (tDstCap.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "param is invalid, tDstCap is null!\n");
		return;
	}

	CRecvSpy tSrcSpyInfo;
	if( !m_cSMcuSpyMana.GetRecvSpy( tSpySrc, tSrcSpyInfo ) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "GetSpyChannlInfo False! \n" );
		return;
	}
	
	//[2011/09/16/zhangli]�����MP4-16CIF�����·��ֱ���Ϊ4CIF
	if (MEDIA_TYPE_MP4 == tDstCap.GetVideoMediaType()
		&& VIDEO_FORMAT_16CIF == tDstCap.GetVideoResolution())
	{
		tDstCap.SetVideoResolution(VIDEO_FORMAT_4CIF);
	}
	//zjl20101116��ǰҪ��ش�����������¼��ն��ѻش�������ȡС
    if (!GetMinSpyDstCapSet(tSpySrc, tDstCap))
    {
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[SendMMcuSpyNotify] GetMinSpyDstCapSet failed!\n");
		return;
    }
	// �����վ����������еķֱ���������
	u32 dwResW = 0;
	u32 dwResH = 0;
	u16 wTmpResW = 0;
	u16 wTmpResH = 0;
	u8 byRes = tDstCap.GetVideoResolution();
	// mpeg4 auto��Ҫ���ݻ������ʻ�÷ֱ���
	if ( VIDEO_FORMAT_AUTO == byRes )
	{
		byRes = GetAutoResByBitrate(byRes,m_tConf.GetBitRate());
	}
	// �Էֱ��ʽ��е���,�����¼��Ǿ�mcu,�跢��ͨ�ֱ���,��ʵ�ֱ��ʿ�������׷��
	GetWHByRes(byRes, wTmpResW, wTmpResH);

	dwResW = wTmpResW;
	dwResH = wTmpResH;
	// ���ݷֱ���,��ö�Ӧͨ�÷ֱ���,�¾�MCU����ʶ��ķֱ���
	byRes = GetNormalRes(wTmpResW, wTmpResH);
	tDstCap.SetVideoResolution(byRes);

	//����
	m_cSMcuSpyMana.SaveSpySimCap( tSpySrc, tDstCap);

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[SendMMcuSpyNotify] Cap<Media:%d, Res:%d, BR:%d, FPS:%d>-dwResWH[%d,%d]!\n",
						tDstCap.GetVideoMediaType(), tDstCap.GetVideoResolution(), tDstCap.GetVideoMaxBitRate(),
						tDstCap.GetUserDefFrameRate(), dwResW, dwResH);
	
	tSrcSpyInfo.SetSimCapset(tDstCap);

	TMsgHeadMsg tHeadMsg;
	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tSrcSpyInfo.m_tSpy,tSrcSpyInfo.m_tSpy );

	TSpyResource tSpyResource;
	tSrcSpyInfo.GetSpyResource( tSpyResource );

	cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cServMsg.CatMsgBody( (u8 *)&tSpyResource, sizeof(TSpyResource) );
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "SendMMcuSpyNotify tSrcSpyInfo.%d Spymode.%d! \n",
		tSrcSpyInfo.m_tSpyAddr.GetPort(),tSrcSpyInfo.m_bySpyMode );
	
	// תΪ������
	dwEvId = htonl(dwEvId);
	dwResW = htonl(dwResW);
	dwResH = htonl(dwResH);	
	cServMsg.CatMsgBody( (u8 *)&dwEvId, sizeof(u32) );
	cServMsg.CatMsgBody( (u8 *)&dwResW, sizeof(u32) );
	cServMsg.CatMsgBody( (u8 *)&dwResH, sizeof(u32) );

	SendMsgToMt( GetFstMcuIdFromMcuIdx(tSpySrc.GetMcuId()), MCU_MCU_SPYCHNNL_NOTIF, cServMsg );
}

/*==============================================================================
������    :  ProcUnfinishEvDeamon
����      :  �����ڶ�ش�������ֺ�֮ǰʣ����߼���Ϣ
�㷨ʵ��  :  
����˵��  :  [IN] TPreSetInRsp ��ش���Ҫ��Ϣ
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-11-05                 pengjie							create
==============================================================================*/
void CMcuVcInst::ProcUnfinishedCascadEvent( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrc = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	switch( tPreSetInRsp.m_tSetInReqInfo.GetEvId() )
	{
	case MT_MCU_STARTSELMT_CMD:			 //��׼ѡ������
	case MT_MCU_STARTSELMT_REQ:          
	case MCS_MCU_STARTSWITCHMT_REQ:	     //����̨Ҫ�󽻻�����
	case MCS_MCU_STARTSWITCHMC_REQ:      //���ѡ���ն�
		ProcSelMtStartPreSetInAck( tPreSetInRsp );
		break;
	case VCS_MCU_START_MONITOR_UNION_REQ:
		ProcStartMonitorPreSetinAck(tPreSetInRsp);
		break;
	case MCS_MCU_STARTVMP_REQ:
		ProcVmpStartPreSetInAck( tPreSetInRsp );
		break;

	case MCS_MCU_SPECSPEAKER_REQ:        //�������ָ̨��һ̨�ն˷���
	case MT_MCU_SPECSPEAKER_REQ:		 //��ͨ�ն�ָ������������
	case MT_MCU_SPECSPEAKER_CMD:
		ProcSpeakerStartPreSetInAck( tPreSetInRsp );
		break;

	case MCS_MCU_START_SWITCH_HDU_REQ:     // ���ѡ����������ǽ
	case MCUVC_STARTSWITCHHDU_NOTIFY:
		ProcHduStartPreSetInAck( tPreSetInRsp );
		break;

	case MCUVC_POLLING_CHANGE_TIMER:       // ������ѯ
		ProcPollStartPreSetInAck( tPreSetInRsp );
		break;

	case MCS_MCU_START_SWITCH_TW_REQ:      // ���ѡ�������ǽ
		ProcTWStartPreSetInAck( tPreSetInRsp );  
		break;

    case MCS_MCU_CHANGEROLLCALL_REQ:       // ��������߼�
		ProcRollCallPreSetInAck( tPreSetInRsp );
		break;

	case VCS_MCU_CONFSELMT_CMD:
		{
			if( VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState() )
			{
				TSwitchInfo tSwitchInfo;
				tSwitchInfo.SetSrcMt( tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() );
				tSwitchInfo.SetDstMt( tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpySwitchInfo.m_tDstMt );
				tSwitchInfo.SetMode( tPreSetInRsp.m_byRspSpyMode );
				
				if(!VCSConfSelMT( tSwitchInfo,FALSE ))
				{
					// ��ϯģʽѡ���¼��ն�ʧ�� [7/19/2012 chendaiwei]
					FreeRecvSpy( tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt(),tPreSetInRsp.m_byRspSpyMode );
				}
			}
			else
			{
				//zjj20110222 �����Ϊ���ڱ�����ѯ�޷�����������ϯ,�ͰѸղ�presetin�����ӵ�Ŀ�����ʹ����ȥ
				FreeRecvSpy( tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt(),tPreSetInRsp.m_byRspSpyMode );			
			}
			//��������ն��ڷ��Զ����ڣ���Ҫ�ӷ��Զ��������
			if( m_cVCSConfStatus.GetCurVCMT() == tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() &&
				m_tApplySpeakQue.IsMtInQueue( tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() )
				)
			{
				BOOL32 bIsSendToChairman = FALSE;
				if( !m_tApplySpeakQue.ProcQueueInfo( tSrc,bIsSendToChairman,FALSE ) )
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "[ProcVmpStartPreSetInAck] Fail to pop tMt(%d.%d). \n",
								tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId(),
								tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMtId()
								);
					bIsSendToChairman = FALSE;
				}
				NotifyMcsApplyList( bIsSendToChairman );
			}
		}
		break;

	case MCS_MCU_STARTREC_REQ:
		{
			ProcRecStartPreSetInAck(tPreSetInRsp);
		}
		break;

	case MCS_MCU_SPECOUTVIEW_REQ:                   // ����������ش������еڶ�����������������ն˽��ش�ͨ��
		{
			ProcDragStartPreSetInAck( tPreSetInRsp );
			break;
		}

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "Error: ProcUnfinishedCascadEvent event id is 0, may cause unusual error\n");
		break;
	}

	return;
}

/*==============================================================================
������    :  SendMMcuSpyBWFullNotify
����      :  �ϼ����¼��Ļش��������֪ͨ
�㷨ʵ��  :  
����˵��  :  [IN] tSrcMt�ش�Դ��[IN] dwSpyBW�ش�����
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-10-15                   pengjie							create
==============================================================================*/
void CMcuVcInst::SendMMcuRejectSpyNotify( const TMt &tSrcMt, u32 dwSpyBW )
{
	CServMsg cServMsg;
	cServMsg.SetEventId( MCU_MCU_REJECTSPY_NOTIF );
	cServMsg.SetMsgBody( (u8 *)&tSrcMt, sizeof(TMt) );
	cServMsg.CatMsgBody( (u8 *)&dwSpyBW, sizeof(u32) );

	SendMsgToMt( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()), MCU_MCU_REJECTSPY_NOTIF, cServMsg );
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "MMcu SendMMcuRejectSpyNotify! \n" );

	return;
}

/*==============================================================================
������    :  ProcStartMonitorPreSetinAck
����      :  ����δ������ļ�������߼�
�㷨ʵ��  :  
����˵��  :  [IN] TPreSetInRsp ��ش���Ҫ��Ϣ
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-08-28                 Ѧ��								create
==============================================================================*/
void CMcuVcInst::ProcStartMonitorPreSetinAck(const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrcMt    = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	
	TSpySwitchDstInfo tSpySwitchDstInfo = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpySwitchDstInfo;

	u8 byOldSpyMode = tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode;
	u8 bySpyMode = tPreSetInRsp.m_byRspSpyMode;

	TSwitchDstInfo tSwitchDstInfo;
	tSwitchDstInfo.m_byMode = tPreSetInRsp.m_byRspSpyMode;
	tSwitchDstInfo.m_bySrcChnnl = tSpySwitchDstInfo.m_bySrcChnnl;
	tSwitchDstInfo.m_tSrcMt = tSrcMt;
	tSwitchDstInfo.m_tDstVidAddr = tSpySwitchDstInfo.m_tDstVidAddr;
	tSwitchDstInfo.m_tDstAudAddr = tSpySwitchDstInfo.m_tDstAudAddr;
	
	u16 wMcInsId = g_cMcuVcApp.GetMcIns(tSpySwitchDstInfo.GetMcIp(), 
										tSpySwitchDstInfo.GetMcSSRC(),
										m_tConf.GetConfSource());
	if (0 == wMcInsId)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[ProcStartMonitorPreSetinAck] GetMcIns from VcsIp.%s McSSrc.%x failed!\n",
								   StrOfIP(tSpySwitchDstInfo.GetMcIp()),
								   tSpySwitchDstInfo.GetMcSSRC());
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcStartMonitorPreSetinAck] GetMcIns.%d success from VcsIp.%s McSSrc.%x!\n",
													wMcInsId, StrOfIP(tSpySwitchDstInfo.GetMcIp()),
													tSpySwitchDstInfo.GetMcSSRC());
	}

	//[2011/10/26/zhangli]Ҫ��ش���ʵ�ʻش���һ�£���ͣ���ɵļ��
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[ProcStartMonitorPreSetinAck] Presetin-Mode:%d, PresetinAck-Mode:%d\n", byOldSpyMode, bySpyMode);

	TMt tOldSrc;
	TSwitchDstInfo tTempSwitchInfo;
	memcpy(&tTempSwitchInfo, &tSwitchDstInfo, sizeof(TSwitchDstInfo));
	
	if (MODE_BOTH == byOldSpyMode && MODE_VIDEO == bySpyMode)
	{
		if(g_cMcuVcApp.GetMonitorSrc(wMcInsId, MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, &tOldSrc))
		{
			if(!tOldSrc.IsNull())
			{
				tTempSwitchInfo.m_tSrcMt = tOldSrc;
				tTempSwitchInfo.m_byMode = MODE_AUDIO;
				StopSwitchToMonitor(tTempSwitchInfo, wMcInsId);
			}
		}
	}
	//���һ��������������MODE_AUDIO == bySpyMode������byOldSpyMode����Ϊ�¼�����Դ�����MODE_VIDEOȥ���������ͷ��ϵ�MODE_VIDEO
	if (/*MODE_BOTH == byOldSpyMode && */MODE_AUDIO == bySpyMode)
	{
		tOldSrc.SetNull();
		if(g_cMcuVcApp.GetMonitorSrc(wMcInsId, MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, &tOldSrc))
		{
			if(!tOldSrc.IsNull())
			{
				tTempSwitchInfo.m_tSrcMt = tOldSrc;
				tTempSwitchInfo.m_byMode = MODE_VIDEO;
				StopSwitchToMonitor(tTempSwitchInfo, wMcInsId);
			}
		}
	}

	StartSwitchToMonitor(tSwitchDstInfo, wMcInsId);
}


/*==============================================================================
������    :  ProcSelMtStartPreSetInAck
����      :  ����δ�������ѡ���߼�
�㷨ʵ��  :  
����˵��  :  [IN] TPreSetInRsp ��ش���Ҫ��Ϣ
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-11-05                 pengjie							create
20100730                   pengjie	                        ��ش���������
==============================================================================*/
void CMcuVcInst::ProcSelMtStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrcMt    = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	u8  bySpyMode = tPreSetInRsp.m_byRspSpyMode;
	u8 byOldSpyMode = tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode;
	u32 EvId      = tPreSetInRsp.m_tSetInReqInfo.GetEvId();
	TSpySwitchInfo tSpySwitchInfo = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpySwitchInfo;
	TMt tDstMt    = tSpySwitchInfo.m_tDstMt;

	CServMsg cServMsg;
	u16 wMcIns = g_cMcuVcApp.GetMcIns(tSpySwitchInfo.GetMcIp(), 
									  tSpySwitchInfo.GetMcSSRC(),
									  m_tConf.GetConfSource());
	if (0 == wMcIns)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[ProcSelMtStartPreSetInAck] GetMcIns from McIp.%s McSSrc.%x failed!\n",
								  StrOfIP(tSpySwitchInfo.GetMcIp()), tSpySwitchInfo.GetMcSSRC());
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcSelMtStartPreSetInAck] GetMcIns McsSSn.%d success from McIp.%s McSSrc.%x!\n",
									 wMcIns, StrOfIP(tSpySwitchInfo.GetMcIp()), tSpySwitchInfo.GetMcSSRC());

		cServMsg.SetSrcSsnId((u8)wMcIns );
	}
	cServMsg.SetEventId( (u16)EvId );

	TSwitchInfo	tSwitchInfo;
	TMtStatus tDstMtStatus;

 	tSwitchInfo.SetSrcMt( tSrcMt );
 	tSwitchInfo.SetDstMt( tDstMt );
 	tSwitchInfo.SetMode( bySpyMode );
	tSwitchInfo.SetDstChlIdx( tSpySwitchInfo.m_byDstChlIdx );
	tSwitchInfo.SetSrcChlIdx( tSpySwitchInfo.m_bySrcChlIdx );


	//�ն�״̬��ѡ��ģʽ��ƥ��, NACK
    // zgc, 2008-07-10, �޸��жϷ�ʽ, �����չ��˽��ΪMODE_NONEʱ���ܾ�ѡ�������������ʾ
	TMtStatus tSrcMtStatus;    
	u8 bySwitchMode = tSwitchInfo.GetMode();	// ��¼��ʼMODE

	m_ptMtTable->GetMtStatus( GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId(), &tSrcMtStatus);
    m_ptMtTable->GetMtStatus(tDstMt.GetMtId(), &tDstMtStatus);
	if( (!tSrcMtStatus.IsSendVideo() ||
         (tDstMt.GetType() == TYPE_MT && !tDstMtStatus.IsReceiveVideo()) ) &&
         (bySwitchMode == MODE_VIDEO || bySwitchMode == MODE_BOTH) )
	{
        bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_AUDIO : MODE_NONE;
	}	
    
    if( (!tSrcMtStatus.IsSendAudio() ||
         (tDstMt.GetType() == TYPE_MT && !tDstMtStatus.IsReceiveAudio()) ) &&
        ( bySwitchMode == MODE_AUDIO || bySwitchMode == MODE_BOTH ) )
    {
        bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_VIDEO : MODE_NONE;
    }

    if ( MODE_NONE == bySwitchMode )
    {
        // �����ѡ��ģʽ��ȫ���ܾ�����NACK
        cServMsg.SetErrorCode( ERR_MCU_SRCISRECVONLY );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
	}

	//zbq[05/22/2009] �ն�ѡ������֧�� FIXME: ��δ�����������Ķ�̬��ռ
    BOOL32 bSelAccordAdp  = FALSE;
	BOOL32 bVidAdp = FALSE;
	BOOL32 bAudAdp = FALSE;
    //�ն�ѡ��
    if( TYPE_MT == tDstMt.GetType() )
    {
		if (MODE_VIDEO == bySwitchMode || MODE_BOTH == bySwitchMode)
		{
			if (IsNeedSelAdpt(tSrcMt, tDstMt, MODE_VIDEO))
			{
				if (g_cMcuVcApp.IsSelAccord2Adp())
				{
					bVidAdp = TRUE;
				}
				else
				{
					//cServMsg.SetErrorCode( ERR_MCU_NOTSAMEMEDIATYPE );
					//[nizhijun 2010/11/15] �������ϢӦ�ûظ����
					FreeRecvSpy(tSrcMt, bySwitchMode);
					NotifyMcsAlarmInfo( 0, ERR_MCU_NOTSAMEMEDIATYPE);
					return;
				}					
			}
		}
		if(MODE_AUDIO == bySwitchMode || MODE_BOTH == bySwitchMode)
		{
			if (IsNeedSelAdpt(tSrcMt, tDstMt, MODE_AUDIO))
			{
				if (g_cMcuVcApp.IsSelAccord2Adp())
				{
					bAudAdp = TRUE;
				}
				else
				{
					//	cServMsg.SetErrorCode( ERR_MCU_NOTSAMEMEDIATYPE );
					//[nizhijun 2010/11/15] �������ϢӦ�ûظ����
					FreeRecvSpy(tSrcMt, bySwitchMode);
					NotifyMcsAlarmInfo( 0, ERR_MCU_NOTSAMEMEDIATYPE );
					return;
				}
			}
		}
	}

	//��ֹSendReplyBack����Ϣ����Ӧ�𣬽�Src��ΪNULL
	if( MT_MCU_STARTSELMT_CMD == EvId  )
	{
		cServMsg.SetNoSrc();
        cServMsg.SetSrcMtId(0);
	}
	
	// ��MCS��֪ͨ
	u8 byMcsId = 0;
	if ( MCS_MCU_STARTSWITCHMT_REQ == EvId   || 
		 MCS_MCU_STARTSWITCHMC_REQ == EvId )
	{
		byMcsId = cServMsg.GetSrcSsnId();		
	}

    if ( MODE_VIDEO == bySwitchMode && MODE_BOTH == tSwitchInfo.GetMode() )
	{
		NotifyMcsAlarmInfo( byMcsId, ERR_MCU_SELMODECHANGE_AUDIOFAIL );
	}

    if ( MODE_AUDIO == bySwitchMode && MODE_BOTH == tSwitchInfo.GetMode() )
	{
		NotifyMcsAlarmInfo( byMcsId, ERR_MCU_SELMODECHANGE_VIDEOFAIL );
	}

    tSwitchInfo.SetMode( bySwitchMode );
	TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tSrcMt.GetMcuId());
	
	// ����
	if( MCS_MCU_STARTSWITCHMC_REQ != EvId )
	{
		if ( tSrcMt.IsLocal() )
		{	
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "ProcSelMtStartPreSetInAck False, tSrc is local, Mtid: %d\n", tSrcMt.GetMtId() );
			return;
		}

		//Ŀ���ն�δ����飬NACK
        if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
        {
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcSelMtStartPreSetInAck]Specified Mt(%u,%u) not joined conference!\n", 
                tDstMt.GetMcuId(), tDstMt.GetMtId() );
            cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            return;
        }

		//����ʱ����ѡ��ģʽ
		if( m_tConf.m_tStatus.IsMixing() && !( tDstMt == m_tConf.GetChairman() )  )
		{
            //ѡ����Ƶ��NACK
			if( tSwitchInfo.GetMode() == MODE_AUDIO  )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "Conference %s is mixing now. Cannot switch only audio!\n", 
                         m_tConf.GetConfName() );
				cServMsg.SetErrorCode( ERR_MCU_SELAUDIO_INMIXING );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
			}
            //������תΪѡ����Ƶ
			else
			{
                //ѡ������Ƶ����ʾ��Ƶѡ��ʧ��
                if ( MODE_BOTH == tSwitchInfo.GetMode() && cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMT_REQ )
                {
                    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "CMcuVcInst: Conference %s is mixing now. Cannot switch audio!\n", 
                             m_tConf.GetConfName() );
                    cServMsg.SetErrorCode( ERR_MCU_SELBOTH_INMIXING );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );                        
                }

				tSwitchInfo.SetMode( MODE_VIDEO );
			}
		}

		StopSelectSrc( tDstMt,tSwitchInfo.GetMode(),FALSE,FALSE );
		
		// �����ѡ��(��Ƶ������Ƶ)����Ҫ��¼����
		if( MODE_AUDIO == tSwitchInfo.GetMode() || 
			MODE_VIDEO == tSwitchInfo.GetMode() ||
			MODE_BOTH  == tSwitchInfo.GetMode() )
		{
			m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tDstMtStatus ); // ѡ��
			tDstMtStatus.SetSelectMt( tSrcMt, tSwitchInfo.GetMode() );
			m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );
		}
        
		if ( MT_MCU_STARTSELMT_CMD != cServMsg.GetEventId() )
		{
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}                

		//for h320 mcu cascade select
		if( MT_MCU_STARTSELMT_REQ == cServMsg.GetEventId() && 
			MT_TYPE_SMCU == m_ptMtTable->GetMtType(tDstMt.GetMtId()) )
		{            
			m_tLastSpyMt = tSrcMt;
		} 

		// [pengjie 2010/4/14] ѡ���¼��ĺϳɳ�Ա�����ֱ���
		if( ChgMtVidFormatRequired(tSrcMt) )
		{
			ChangeMtVideoFormat(tSrcMt, FALSE);
		}

		// ֪ͨ�ն˽��յ�ַ
		/*if (m_tConf.GetConfAttrb().IsSatDCastMode())
		{
			if (MODE_AUDIO == tSwitchInfo.GetMode() || MODE_BOTH == tSwitchInfo.GetMode())
			{
				ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_AUDIO, FALSE);
			}
			
			if (MODE_VIDEO == tSwitchInfo.GetMode() || MODE_BOTH == tSwitchInfo.GetMode())
			{
				ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_VIDEO, FALSE);
			}                
		}*/                   
    } 

	// ��������
    // ������Ҫ�����Ƿ��ǹ㲥����
    // ��Ҫ��ֹ��ؼ�ؽ���
    if( tSrcMt == GetLocalVidBrdSrc() && EvId != MCS_MCU_STARTSWITCHMC_REQ &&
        ( !( (tSrcMt==m_tRollCaller && GetLocalMtFromOtherMcuMt(tDstMt)==GetLocalMtFromOtherMcuMt(m_tRollCallee)) 
		|| (tSrcMt==GetLocalMtFromOtherMcuMt(m_tRollCallee) && tDstMt==m_tRollCaller) ) ) &&
        ( MODE_VIDEO == tSwitchInfo.GetMode() || MODE_BOTH == tSwitchInfo.GetMode() ) )
    {
        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "Dst(Mt.%d) receive Src(Mt.%d) as video broadcast src!\n", tDstMt.GetMtId(), tSrcMt.GetMtId() );

        StartSwitchFromBrd( tSrcMt, 0, 1, &tDstMt );
        
        if ( MODE_BOTH == tSwitchInfo.GetMode() )
        {
            tSwitchInfo.SetMode( MODE_AUDIO );
        }
        else
        {
            // �������ֱ��return����Ϊ�����������ʵ�ʲ�û�з���ѡ��
            return;
        }
	}

	//[2011/10/26/zhangli]Ҫ��ش���ʵ�ʻش���һ��
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[ProcSelMtStartPreSetInAck] Presetin-Mode:%d, PresetinAck-Mode:%d\n", byOldSpyMode, bySpyMode);
	
	TMt tOldSrc;
	BOOL32 bInnerSelect = FALSE;
	if (MT_MCU_STARTSELMT_CMD == cServMsg.GetEventId()
		|| MT_MCU_STARTSELMT_REQ == cServMsg.GetEventId()
		|| MCS_MCU_STARTSWITCHMT_REQ == cServMsg.GetEventId())
	{
		if (cServMsg.GetSrcSsnId() == 0)		
		{
			bInnerSelect = TRUE;
		}
		//���ĳ��ѡ��ʧ�ܣ�ͣ���ɵ�ѡ��
		if (MODE_BOTH == byOldSpyMode && MODE_VIDEO == bySpyMode)
		{
			TMtStatus tMtStatus;
			m_ptMtTable->GetMtStatus(tDstMt.GetMtId(), &tMtStatus);
			tOldSrc = tMtStatus.GetSelectMt(MODE_AUDIO);
			
			if (!tOldSrc.IsNull())
			{
				StopSelectSrc(tDstMt, MODE_AUDIO);
			}
		}
		
		if (MODE_BOTH == byOldSpyMode && MODE_AUDIO == bySpyMode)
		{
			TMtStatus tMtStatus;
			m_ptMtTable->GetMtStatus(tDstMt.GetMtId(), &tMtStatus);
			tOldSrc = tMtStatus.GetSelectMt(MODE_VIDEO);
			
			if (!tOldSrc.IsNull())
			{
				StopSelectSrc(tDstMt, MODE_VIDEO);
			}
		}
	}
	else if (MCS_MCU_STARTSWITCHMC_REQ == cServMsg.GetEventId())
	{
		//���ĳ�ּ��ʧ�ܣ�ͣ���ɵļ�أ�����������FALSE���������滻����ʱ�������ռ��
		if (MODE_BOTH == byOldSpyMode && MODE_VIDEO == bySpyMode)
		{
			g_cMcuVcApp.GetMcSrc(cServMsg.GetSrcSsnId(), &tOldSrc, tSwitchInfo.GetDstChlIdx(), MODE_AUDIO);
			
			if (!tOldSrc.IsNull())
			{
				StopSwitchToMc(cServMsg.GetSrcSsnId(), tSwitchInfo.GetDstChlIdx(), FALSE, MODE_AUDIO);
			}
		}
		
		//���һ��������������MODE_AUDIO == bySpyMode������byOldSpyMode����Ϊ�¼�����Դ�����MODE_VIDEOȥ���������ͷ��ϵ�MODE_VIDEO
		if (/*MODE_BOTH == byOldSpyMode && */MODE_AUDIO == bySpyMode)
		{
			g_cMcuVcApp.GetMcSrc(cServMsg.GetSrcSsnId(), &tOldSrc, tSwitchInfo.GetDstChlIdx(), MODE_VIDEO);
			
			if (!tOldSrc.IsNull())
			{
				StopSwitchToMc(cServMsg.GetSrcSsnId(), tSwitchInfo.GetDstChlIdx(), FALSE, MODE_VIDEO);
			}
		}
	}

	TSwitchInfo tTempSwitchInfo;
	BOOL32 bSwitchVideoSuc = FALSE;
	BOOL32 bSwitchAudioSuc = FALSE;
	if(MODE_BOTH == tSwitchInfo.GetMode() || MODE_VIDEO == tSwitchInfo.GetMode())
	{
		if (bVidAdp)
		{
			bSwitchVideoSuc = StartSelAdapt(tSwitchInfo.GetSrcMt(), tSwitchInfo.GetDstMt(), MODE_VIDEO, bInnerSelect);
			if ( !bSwitchVideoSuc ) 
			{
				FreeRecvSpy(tSwitchInfo.GetSrcMt(),MODE_VIDEO);
				TMt tNullMt;
				tDstMtStatus.SetSelectMt( tNullMt, MODE_VIDEO);
				m_ptMtTable->SetMtStatus( tSwitchInfo.GetDstMt().GetMtId(), &tDstMtStatus );
			}
		}
		else
		{			
			bSwitchVideoSuc = TRUE;
			memcpy(&tTempSwitchInfo, &tSwitchInfo, sizeof(tTempSwitchInfo));
			tTempSwitchInfo.SetMode(MODE_VIDEO);
			SwitchSrcToDst(tTempSwitchInfo, cServMsg);				
		}
	}
	
	if (MODE_BOTH == tSwitchInfo.GetMode() || MODE_AUDIO == tSwitchInfo.GetMode())
	{
		if (bAudAdp)
		{
			bSwitchAudioSuc = StartSelAdapt(tSwitchInfo.GetSrcMt(), tSwitchInfo.GetDstMt(), MODE_AUDIO, bInnerSelect);
			if ( !bSwitchAudioSuc ) 
			{
				FreeRecvSpy(tSwitchInfo.GetSrcMt(),MODE_AUDIO);
				TMt tNullMt;
				tDstMtStatus.SetSelectMt( tNullMt, MODE_AUDIO);
				m_ptMtTable->SetMtStatus( tSwitchInfo.GetDstMt().GetMtId(), &tDstMtStatus );
			}
		}
		else
		{
			bSwitchAudioSuc = TRUE;
			memcpy(&tTempSwitchInfo, &tSwitchInfo, sizeof(tTempSwitchInfo));
			tTempSwitchInfo.SetMode(MODE_AUDIO);
			SwitchSrcToDst(tTempSwitchInfo, cServMsg);
		}
	}

	// ���÷�����ԴΪ�ǻ����Զ�ָ��
	if ( HasJoinedSpeaker() && tDstMt == GetLocalSpeaker() )
	{
		SetSpeakerSrcSpecType( tSwitchInfo.GetMode(), SPEAKER_SRC_MCSDRAGSEL );
	}	
    
	//zjl20101116ѡ��ҵ��SpyNotify
	if( !tSwitchInfo.GetSrcMt().IsLocal() )
	{
		if( IsLocalAndSMcuSupMultSpy(tSwitchInfo.GetSrcMt().GetMcuId()) )
		{
			//��֤�ϼ������ɹ�
			if (bSwitchVideoSuc || bSwitchAudioSuc)
			{
				//ѡ��ҵ���ȡ����ͨ����������Ϊ�ش�Ŀ��������
				TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSwitchInfo.GetSrcMt().GetMcuId()));				
				if(TYPE_MT == tDstMt.GetMtType())
				{
					TSimCapSet tDstCapSet = m_ptMtTable->GetDstSCS(tDstMt.GetMtId());
					tSimCapSet.SetVideoMaxBitRate(tDstCapSet.GetVideoMaxBitRate());
				}
				
				if (tSimCapSet.IsNull())
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[ProcSelMtStartPreSetInAck:SpyNotify] tSimCapSet is null!\n");
					return;
				}	
				SendMMcuSpyNotify( tSwitchInfo.GetSrcMt(), EvId, tSimCapSet);		
			}
// 			else
// 			{
// 				//����ϼ�ѡ��ʧ�ܣ��ͷ���Դ
// 				FreeRecvSpy(tSwitchInfo.GetSrcMt(), tSwitchInfo.GetMode());
// 			}
		}
	}	
}


/*==============================================================================
������    :  ProcVmpStartPreSetInAck
����      :  ����δ������Ļ���ϳ��߼�
�㷨ʵ��  :  
����˵��  :  [IN] TPreSetInRsp ��ش���Ҫ��Ϣ
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-11-11					Ѧ��							create
==============================================================================*/
void CMcuVcInst::ProcVmpStartPreSetInAck(const TPreSetInRsp &tPreSetInRsp1)
{
	//zhouyiliang 20100909 ����vmp���룬�����û���vmpparam����������AdjustVmpParam���������ֻ����tPeriEqpStatus����
	//TVMPParam tVMPParam	= m_tConf.m_tStatus.GetVmpParam();
	TEqp tVmpEqp = tPreSetInRsp1.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_tVmp;
	if (!IsValidVmpId(tVmpEqp.GetEqpId()))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ProcVmpStartPreSetInAck] vmp id.%d is wrong.\n",
			tVmpEqp.GetEqpId());
		return;
	}
	TPeriEqpStatus tPeriEqpStatus; 
	BOOL32 bGetStatus = g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );


	// �жϵ�ǰ״̬�Ƿ�����,������ʱ��Ҫ�ͷŴ�·�ش�����
	if( bGetStatus 
		&& ( TVmpStatus::WAIT_STOP == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState ||
		TVmpStatus::IDLE == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
		)
	{
		TMt tSrc = tPreSetInRsp1.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
		FreeRecvSpy(tSrc, MODE_VIDEO);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ProcVmpStartPreSetInAck] vmp state.%d is not correct, so return here\n",
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState);
		return;
	}

	TVMPParam_25Mem tVMPParam = tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam() ;

	//zhouyiliang 20100820 ���Ҫ���滻���߼��Ļ���Ҫ��tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos
	TPreSetInRsp tPreSetInRsp = tPreSetInRsp1;
	/*if ( tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsNeedRelease ) 
	{
		//vcs �Զ�����ϳ�,ֻ��vcs�Զ�����ϳɲ����滻����
		//zhouyiliang20100917 vcs �Զ�����ϳɲ����޸ģ�ע�͵��ϵĲ���
		//HandleVCSAutoVmpSpyRepalce( tPreSetInRsp );

		//zhouyiliang20100917 mcs���ƻ���ϳ�,����滻��ͨ���ڵ��ն��ڱ��ͨ���ڻ��У��緢���˸���ͨ�����ҵ��滻�ն����ڱ��ͨ���������
		if (VCS_CONF != m_tConf.GetConfSource() && !tVMPParam.IsVMPAuto() && !IsAllowVmpMemRepeated())
		{
			TMt tSrc = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();

			TMultiCacMtInfo tMtInfo;
			tMtInfo.m_byCasLevel = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCasLevel;
			memcpy( &tMtInfo.m_abyMtIdentify[0],
				&tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_abyMtIdentify[0],
				sizeof(tMtInfo.m_abyMtIdentify)
				);
			TMt tReleaseMt = GetMtFromMultiCascadeMtInfo( tMtInfo,
							tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt
							);
			if ( !tReleaseMt.IsNull() ) 
			{				
				for ( u8 byIdx = 0; byIdx < tVMPParam.GetMaxMemberNum() ; byIdx++  ) 
				{
					TVMPMember tTempVmpMember;
					tTempVmpMember = *tVMPParam.GetVmpMember(byIdx);
					if ( tReleaseMt == (TMt)tTempVmpMember 
						&& byIdx != tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos ) 
					{
						tVMPParam.ClearVmpMember( byIdx );
					}
				}
			}
		}
	}*/

	BOOL32 bStart = FALSE;
	//��������״̬�ж��Ƿ��ǵ�һ�ο���vmp
	if (TVmpStatus::RESERVE == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
	{
		bStart = TRUE;
	}

	//���ݻش���Ӧ��Ϣ����ӵ�VMPParam��
	TVMPMember tVmpMember;
	tVmpMember.SetNull();
	tVmpMember.SetMemberTMt(tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt());
	tVmpMember.SetMemberType(tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byMemberType);
	tVmpMember.SetMemStatus(tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byMemStatus);
	tVMPParam.SetVmpMember(tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos, tVmpMember);
	//zhouyiliang 20100721 ��vmpparam������˸ղ��滻���Ǹ���Ա�⣬���еĳ�Ա���滻���ȼ�����Ϊ1���д������Ƿ�++��
// 	for (u8 byLoop = 1; byLoop < tVMPParam.GetMaxMemberNum(); byLoop++)
// 	{
// 		if (tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos == byLoop)//�������滻�ĳ�Ա
// 		{
//              m_cVCSConfStatus.SetVmpMemberReplacePrio(byLoop, 0 );//zhouyiliang20100721 ����vmp�ĳ�Ա��͵��滻���ȼ�
// 			continue;
// 		}
// 		//zhouyiliang 20100831 ע�͵�ԭ����vcs�Զ�����ϳ��滻ͬ��mcu�Ĳ��ԣ����ð�˳���滻���ϲ���
// 	//	TMt tTemMemeber = *(TMt*)tVMPParam.GetVmpMember( byLoop );	
// 	// 	if ( tTemMemeber.GetMcuId() == tVmpMember.GetMcuId() ) 
// 	// {
// 		m_cVCSConfStatus.SetVmpMemberReplacePrio(byLoop, 1);
// 	//	}
// 		
// 	}
	
	//lukunpeng 2010/07/09 ������SetVmpParam���Է�ֹʱ��������
	//m_tConf.m_tStatus.SetVmpParam(tVMPParam);

	//����VMP
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[ProcVmpStartPreSetInAck]needsetin pos:%d\n ",tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos );
	AdjustVmpParam(tVmpEqp.GetEqpId(), &tVMPParam, bStart, FALSE);
	TMt tSrc = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	//��������ն��ڷ��Զ����ڣ���Ҫ�ӷ��Զ��������
	if( m_cVCSConfStatus.GetCurVCMT() == tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() &&
		m_tApplySpeakQue.IsMtInQueue( tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() ) 
		)
	{
		BOOL32 bIsSendToChairman = FALSE;
		if( !m_tApplySpeakQue.ProcQueueInfo( tSrc,bIsSendToChairman,FALSE ) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "[ProcVmpStartPreSetInAck] Fail to pop tMt(%d.%d). \n",
						tSrc.GetMcuId(),tSrc.GetMtId()
						);
			bIsSendToChairman = FALSE;
		}

		NotifyMcsApplyList( bIsSendToChairman );
	}
}

/*==============================================================================
������    :  ProcSpeakerStartPreSetInAck
����      :  ����δ����������¼��ն����ϼ������˵Ĳ���
�㷨ʵ��  :  
����˵��  :  [IN] TPreSetInRsp ��ش���Ҫ��Ϣ
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-11-05                 pengjie							create
2011-7-8     4.6           �����                          �޸�
==============================================================================*/
void CMcuVcInst::ProcSpeakerStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrcMt      = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();

// 2011-7-8 del by peng guofeng : û���õ�
// 	u8  bySpyMode = tPreSetInRsp.m_byRspSpyMode;
// 	u32 EvId      = tPreSetInRsp.m_tSetInReqInfo.m_dwEvId;
// 	CServMsg cServMsg;
// 2011-7-8 del end
	
	//�ı䷢���� ��ʱpresetin�Ѿ���ɣ��ٴε���ChangeSpeaker���������������presetinΪfalse��
	ChangeSpeaker( &tSrcMt,FALSE,TRUE,FALSE );
	return;
}

/*==============================================================================
������    :  ProcHduStartPreSetInAck
����      :  ��������ش��¼��ն˽��ϼ��������ǽ���δ�������ҵ���߼�
�㷨ʵ��  :  
����˵��  :  [IN] TPreSetInRsp ��ش���Ҫ��Ϣ
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-07-16                 ½����							modify
==============================================================================*/
void CMcuVcInst::ProcHduStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrcMt    = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	u8  byHduId   = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo.m_tHdu.GetEqpId();;
	u8  bySpyMode = tPreSetInRsp.m_byRspSpyMode;
	u32 EvId      = tPreSetInRsp.m_tSetInReqInfo.GetEvId();
	TSpyHduInfo tSpyHduInfo = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo;
	if (tSpyHduInfo.m_byDstChlIdx >= MAXNUM_HDU_CHANNEL) 
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ProcHduStartPreSetInAck]tSpyHduInfo.m_byDstChlIdx(%d) >= MAXNUM_HDU_CHANNEL(%d)\n");
		FreeRecvSpy(tSrcMt, bySpyMode);
		return;
	}

	//����ǽ��ѯ���ȼ���ߣ�����ѡ��VMP��ѡ���ն�
	TPeriEqpStatus tHduStatus;
    g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
	
	// [10/24/2011 liuxu] 
	TTvwMember tTvwMember = tHduStatus.m_tStatus.tHdu.GetHduMember(tSpyHduInfo.m_byDstChlIdx, tSpyHduInfo.GetSubChnIdx());
	if (tTvwMember.GetConfIdx() != 0 && tTvwMember.GetConfIdx() != m_byConfIdx)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ProcHduStartPreSetInAck] hdu chnnl has been occupyed by other conf\n");
		FreeRecvSpy(tSrcMt, bySpyMode);
		return;
	}

	// [10/24/2011 liuxu] ״̬����, ����ֹͣ����ǽ������Ӧ����ǽ��ѯ��timer��presetin ack
	if( tSpyHduInfo.m_bySrcMtType == TW_MEMBERTYPE_TWPOLL )
	{
		if (tTvwMember.byMemberType != TW_MEMBERTYPE_TWPOLL)
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ProcHduStartPreSetInAck] poll has been stoped\n");
			FreeRecvSpy(tSrcMt, bySpyMode);
			return;
		}

		u8 byPollState = POLL_STATE_NONE;
		m_tTWMutiPollParam.GetPollState(byHduId, tSpyHduInfo.m_byDstChlIdx, byPollState);
		if (POLL_STATE_NORMAL != byPollState)
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ProcHduStartPreSetInAck] poll mode has been changed\n");
			FreeRecvSpy(tSrcMt, bySpyMode);
			return;
		}
	}

	// [2013/03/11 chenbing] ��Ҫ�ж��Ƿ�Ϊ�ķ�� 
	u8 byHduMode = HDUCHN_MODE_ONE;
	if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(tSpyHduInfo.m_byDstChlIdx)
		&& TW_MEMBERTYPE_BATCHPOLL != tSpyHduInfo.m_bySrcMtType
		&& TW_MEMBERTYPE_TWPOLL != tSpyHduInfo.m_bySrcMtType
	   )
	{ 
		byHduMode = HDUCHN_MODE_FOUR;
	}
	if (!tTvwMember.IsNull() && 
		tHduStatus.m_tStatus.tHdu.GetChnStatus(tSpyHduInfo.m_byDstChlIdx, tSpyHduInfo.GetSubChnIdx()) == THduChnStatus::eRUNNING)
	{
		// [2013/03/11 chenbing]  ��� tSpyHduInfo.GetSubChnIdx()
		u8  byOldMemberType = tHduStatus.m_tStatus.tHdu.GetMemberType(tSpyHduInfo.m_byDstChlIdx, tSpyHduInfo.GetSubChnIdx());
		ChangeHduSwitch( NULL,byHduId, tSpyHduInfo.m_byDstChlIdx, tSpyHduInfo.GetSubChnIdx()/*��ͨ��*/,
			byOldMemberType, TW_STATE_STOP, MODE_BOTH, FALSE, TRUE, FALSE );
		g_cMcuVcApp.GetPeriEqpStatus( byHduId, &tHduStatus );
	}
	tHduStatus.m_tStatus.tHdu.SetSchemeIdx( tSpyHduInfo.m_bySchemeIdx, tSpyHduInfo.m_byDstChlIdx, tSpyHduInfo.GetSubChnIdx());
    g_cMcuVcApp.SetPeriEqpStatus( byHduId, &tHduStatus );
	
	// [2013/03/11 chenbing]  ��� tSpyHduInfo.GetSubChnIdx()
	ChangeHduSwitch( &tSrcMt, byHduId, tSpyHduInfo.m_byDstChlIdx, tSpyHduInfo.GetSubChnIdx(),
		tSpyHduInfo.m_bySrcMtType, TW_STATE_START, bySpyMode, FALSE, TRUE, FALSE, byHduMode);

	if (VCS_CONF == m_tConf.GetConfSource() 
		&& ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode())
		&& m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_AUTO_MODE)
	{
		// [5/31/2011 liuxu] ���ṩ�ӿ�GetConfCfgedTvwIdx��Щ���µĴ���
		const u8 wCurrTwPosId = GetConfCfgedTvwIdx( tSpyHduInfo.m_tHdu.GetEqpId(), tSpyHduInfo.m_byDstChlIdx);
		m_cVCSConfStatus.SetCurUseTWChanInd( wCurrTwPosId );
		
		CRecvSpy tSrcSpyInfo;
		if( m_cSMcuSpyMana.GetRecvSpy(tSrcMt, tSrcSpyInfo) &&
			TSpyStatus::WAIT_FREE == tSrcSpyInfo.m_byUseState )
		{
			m_cSMcuSpyMana.ModifyUseState( tSrcMt,TSpyStatus::NORMAL );
		}
	}
	return;
}

/*==============================================================================
������    :  ProcPollingStartPreSetInAck
����      :  ��������ش� ��ѯ�¼�ʱδ��������߼�
�㷨ʵ��  :  
����˵��  :  [IN] TPreSetInRsp ��ش���Ҫ��Ϣ
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-11-05                 pengjie							create
==============================================================================*/	
void CMcuVcInst::ProcPollStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrcMt    = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	u8  bySpyMode = tPreSetInRsp.m_byRspSpyMode;
	u32 dwEvId      = tPreSetInRsp.m_tSetInReqInfo.GetEvId();
	u16 wKeepTime = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyPollInfo.m_wKeepTime;
	u8  byPollingPos = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyPollInfo.m_byPollingPos;
	u8  byPollMode = tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode;

	//lukunpeng 2010/06/30 ��ȡ��ǰ��ѯ���նˣ�������ش��ն˲�һ�£���˵����ѯ�Ѿ��ߵ���һ���նˣ�
	//��ʱ�Ļش��ն��Ѿ�û�����壬�ͷŵ�ǰ�ش��ն���Ϣ��
	TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
	TMt tCurPolledMt = tPollInfo.GetMtPollParam().GetTMt();
	if (!(tCurPolledMt == tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt()))
	{
		FreeRecvSpy(tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt(), bySpyMode);
		return;
	}

	//��TvWall��Hdu�Ƶ�VMPǰ����������ChangeSpeaker��Ӧ
	//TvWall
	TPeriEqpStatus tTWStatus;
	u8 byChnlIdx;
	u8 byEqpId;
				
	for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
	{
		if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
		{
			// [8/30/2011 liuxu] ����õ���ǽ������, �Ͳ�Ҫ����������
			if(g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus) && tTWStatus.m_byOnline )
			{	
				for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
				{
					// [miaoqingsong 20111205] �����ж�����: ��ϯ��ѯ/�ϴ���ѯģʽ�ֵ���MT����������ǽ��Ƶ��ѯ����ͨ��
					if ( (TW_MEMBERTYPE_POLL == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType) && 
						 (m_byConfIdx == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx()) && 
						 !(tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN) && 
						 !(tPollInfo.GetMediaMode() == MODE_VIDEO_SPY || tPollInfo.GetMediaMode() == MODE_BOTH_SPY) 
						 )
					{
						ChangeTvWallSwitch( &tSrcMt, byEqpId, byChnlIdx, TW_MEMBERTYPE_POLL, TW_STATE_CHANGE);
					}
				}
			}
			
		}
	}
				
	//hdu
	TPeriEqpStatus tHduStatus;
	u8 byHduChnlIdx;
	u8 byHduEqpId;
				
	for (byHduEqpId = HDUID_MIN; byHduEqpId <= HDUID_MAX; byHduEqpId++)
	{
		if(IsValidHduEqp(g_cMcuVcApp.GetEqp(byHduEqpId)))
		{
			if( !g_cMcuVcApp.GetPeriEqpStatus(byHduEqpId, &tHduStatus) || !tHduStatus.m_byOnline )
			{
				continue;
			}
			
			u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byHduEqpId));
			if (0 == byHduChnNum)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU,  "[ProcPollStartPreSetInAck] GetHduChnNumAcd2Eqp failed!\n");
				continue;
			}
			
			for (byHduChnlIdx = 0; byHduChnlIdx < byHduChnNum; byHduChnlIdx++)
			{
				// [miaoqingsong 20111205] �����ж�����: ��ϯ��ѯ/�ϴ���ѯģʽ�ֵ���MT����������ǽ��Ƶ��ѯ����ͨ��
				if ( (TW_MEMBERTYPE_POLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].byMemberType) &&  
					 (m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetConfIdx()) && 
					 !(tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN) && 
					 !(tPollInfo.GetMediaMode() == MODE_VIDEO_SPY || tPollInfo.GetMediaMode() == MODE_BOTH_SPY) 
					 )
				{
					// [2013/03/11 chenbing] HDU�໭�治֧����ѯ,��ͨ����0
					ChangeHduSwitch( &tSrcMt, byHduEqpId, byHduChnlIdx, 0, TW_MEMBERTYPE_POLL, TW_STATE_START, bySpyMode );
				}
				else
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcPollStartPreSetInAck] HduEqp%d is not exist or not polling \n", byHduEqpId);
				}
			}
		}
	}
				
	// 2011-10-11: ��Ӵ˱�ǣ���ʾҪ��Ҫ����������һ��VMP�Ĳ�������
	BOOL32 bVmpNeedChgBySpeaker = TRUE;

	//������渴�ϵ���Ӧ��ѯ����Ľ���
	// xliang [3/31/2009] �����߼�
	//TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	u8 byVmpCount = GetVmpCountInVmpList();
	if (byVmpCount > 0)
	{
		u8 byVmpPollCount = GetVmpChnnlNumBySpecMemberType( VMP_MEMBERTYPE_POLL);//��ѯ����ͨ����
		BOOL32 bNoneKeda = FALSE;
		if (!tCurPolledMt.IsNull())
		{
			bNoneKeda = (!IsKedaMt(tCurPolledMt, TRUE)) || (!IsKedaMt(tCurPolledMt, FALSE));
		}
		/* xliang [4/21/2009] ��ϯ��ѡ��VMPʧЧ
		if( tVmpParam.IsVMPSeeByChairman() )
		{
			m_tConf.m_tStatus.SetVmpSeebyChairman(FALSE);
			
            // ״̬ͬ��ˢ�µ�TPeriStatus��
			TPeriEqpStatus tVmpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
			tVmpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
			g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
			
			// xliang [4/14/2009] ��ϯѡ��VMP�Ľ���ͣ��
			StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO);
		}*/
		
		// xliang [4/2/2009] ����Ƶ�Ļ�����ѯ,VMP��ѯ���棬VMP�����˸���3�߲��ܹ���
		// MPU2֧����ѯ����ͷ����˸���ͬʱ����.
		// �����˸���ͨ��ȡ�����ϱ�MCS������ʾ
		// ����Ƶ���ϴ�+������ѯ,ͬ����VMP��ѯ���棬VMP�����˸���3�߲��ܹ���
		/*u8 byVmpSubType = GetVmpSubType(m_tVmpEqp.GetEqpId());
		if( (tPollInfo.GetMediaMode() == MODE_BOTH || tPollInfo.GetMediaMode() == MODE_BOTH_BOTH)
			&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) //����Ƶ�Ļ�����ѯ=�����䷢����
			&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)  
			//&& (byVmpSubType == MPU_SVMP || byVmpSubType == VMP_8KH)//������vmp����	// [2/28/2010 xliang] ������mpu-vmp������
			&& !IsAllowVmpMemRepeated(m_tVmpEqp.GetEqpId()) //  [2/17/2012 pengguofeng]
			)	
		{
			NotifyMcsAlarmInfo( 0, ERR_AUDIOPOLL_CONFLICTVMPFOLLOW );
			
			//����VMP param
			u8 byLoop;
			TVMPMember tVmpMember;
			for(byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++)
			{
				tVmpMember = *tVmpParam.GetVmpMember(byLoop);
				if(tVmpMember.GetMemberType() == VMP_MEMBERTYPE_SPEAKER)
				{
					ClearOneVmpMember(m_tVmpEqp.GetEqpId(), byLoop, tVmpParam);
					break;
				}
			}
		}*/

		// [miaoqingsong 20111205] �����ж���������ϯ��ѯģʽ�ֵ���MT������VMP��Ƶ��ѯ����ͨ��
		if( byVmpPollCount > 0 && 
			!(tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN) && 
			!(tPollInfo.GetMediaMode() == MODE_VIDEO_SPY || tPollInfo.GetMediaMode() == MODE_BOTH_SPY)
			//|| (tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) //����Ƶ�Ļ�����ѯ�������˸���ͨ��
			//&& tPollInfo.GetMediaMode() == MODE_BOTH ) 
			)
		{
			bVmpNeedChgBySpeaker = FALSE;
			// xliang [3/19/2009] ����MPU������ѯ����MT�Ѿ�������ͨ�����Ҹ�ͨ���Ƿ����˸�����߸�Mt�Ƿ�����
			// �򲻽�VMP�е���ѯ����ͨ��  ----�ϳ� xliang [4/2/2009] 
			// xliang [4/2/2009] ������VMP������ѯ����MT�Ѿ�������ͨ���о�������ѯ����ͨ��

			// [2/5/2010 xliang] ����
			/*u8 byMtVmpChnl = tVmpParam.GetChlOfMtInMember(tCurPolledMt /*tLocalMt/);
			if( (!IsAllowVmpMemRepeated(m_tVmpEqp.GetEqpId())) 
				&&  tVmpParam.IsMtInMember(tCurPolledMt)
				)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MCS, "[ProcPollingChangeTimerMsg] Mt.(%u,%u) has already in certain VMP channel!\n",
					tCurPolledMt.GetMcuId(), tCurPolledMt.GetMtId() );
			}
			else
			{*/
				/*��ǰ�ͷ���ѯ����ͨ���ľ���ѯ�ն˶�ش���Դ,��ֹʱ������(vmpδ�ͷ�֮ǰ,��ѯ�ȷ���SendMMcuSpyNotify)
				//��ѯ��������ڶ��vmpͨ����
				for (u8 byChlNum=0; byChlNum<tVmpParam.GetMaxMemberNum(); byChlNum++)
				{
					TVMPMember tOldMember = *tVmpParam.GetVmpMember(byChlNum);
					if (!tOldMember.IsNull() && !tOldMember.IsLocal() && !(tOldMember == tCurPolledMt))
					{
						// �ͷ���ѯ�����ش���Դ
						if (VMP_MEMBERTYPE_POLL == tOldMember.GetMemberType())
						{
							/* �ǿƴ��ն�,������1·��ѯ����ͨ��(������,�������·����)
							if (bNoneKeda && byChlNum != tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_POLL))
							{
								ClearOneVmpMember(byChlNum, tVmpParam);
								continue;
							}*
							FreeRecvSpy( tOldMember, MODE_VIDEO );
						}
						// ����Ǵ���Ƶ����ѯ,ͬ���ڴ˴��ͷŷ����˸����ش���Դ
						else if (VMP_MEMBERTYPE_SPEAKER == tOldMember.GetMemberType())
						{
							/* �ǿƴ��ն�,������1·��ѯ����ͨ��
							if (bNoneKeda && byChlNum != tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_SPEAKER))
							{
								ClearOneVmpMember(byChlNum, tVmpParam);
								continue;
							}*
							// ����Ƶ�ı�����ѯ,����Ƶ�Ļش���ѯ+������ѯ
							if (tPollInfo.GetMediaMode() == MODE_BOTH ||
								tPollInfo.GetMediaMode() == MODE_BOTH_BOTH)
							{
								FreeRecvSpy( tOldMember, MODE_VIDEO );
							}
						}
					}
				}*/

				//u8 byVmpMemType = VMP_MEMBERTYPE_POLL;
				//TMt tLastMt = m_tConfPollParam.GetLastPolledMt();
				//ChangeVmpChannelParam(&tCurPolledMt/*&tLocalMt*/, byVmpMemType, &tLastMt);
				// 2011-10-11 ��ѯ��˳����������˵����ͨ�������Բ��÷������ٵ�����
				/*bVmpNeedChgBySpeaker = FALSE;
			}*/
		}
	}

	if ( (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE) && 
		 !(tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN) && 
		 !(tPollInfo.GetMediaMode() == MODE_VIDEO_SPY || tPollInfo.GetMediaMode() == MODE_BOTH_SPY) 
		 )
	{
		ChangeVmpTwChannelParam(&tCurPolledMt, VMPTW_MEMBERTYPE_POLL);
	}

	TMt tLastMt = m_tConfPollParam.GetLastPolledMt();
	//tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	BOOL32 bIsVmpBrd = (GetVidBrdSrc().GetEqpType() == EQP_TYPE_VMP);

 	switch( tPollInfo.GetMediaMode() )
 	{
 	case MODE_VIDEO:
 		{		
			u8 bySrcChnnl = (tLastMt == m_tPlayEqp ? m_byPlayChnnl : 0);
			
			FreeRecvSpy( tLastMt, MODE_VIDEO );	
			if(!bIsVmpBrd)
			{
				ChangeVidBrdSrc( &tSrcMt );
			}

			m_tConfPollParam.SetLastPolledMt( tSrcMt );
		}
		break;

	case MODE_BOTH:
        {
			ChangeSpeaker(&tSrcMt, TRUE, bVmpNeedChgBySpeaker/*TRUE*/, FALSE ); // 2011-10-8 mod by pgf:VMP������ͨ���Ѿ�����ѯ�д����˴�������Ҫ
			m_tConfPollParam.SetLastPolledMt( tSrcMt );	
		}	
        break;
    
	// miaoqingsong [20110608] ������ش���ϯ��ѯѡ���������
	case MODE_VIDEO_CHAIRMAN:
	case MODE_BOTH_CHAIRMAN:
		{
			if( !tLastMt.IsNull() )
			{
				if( MODE_VIDEO_CHAIRMAN == tPollInfo.GetMediaMode() )
				{
					StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO, FALSE, FALSE);
				}
				else //both
				{
					StopSelectSrc(m_tConf.GetChairman(), MODE_BOTH, FALSE, FALSE);
				}
			}

			if(HasJoinedChairman())
			{
				ChangeSelectSrc(tSrcMt, m_tConf.GetChairman(), bySpyMode);
			}
			else
			{
				FreeRecvSpy( tSrcMt,tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN?MODE_VIDEO:MODE_BOTH );
			}

			// ��ѡ���ն���vmp�еĵ��ֱ��ʴ���
			if (!(tLastMt == tSrcMt) )
			{
				//������ѡ��Դ�ֱ���
				if(!tLastMt.IsNull() && ChgMtVidFormatRequired(tLastMt) )
				{
					ChangeMtVideoFormat(tLastMt);
				}
				//������ѡ��Դ�ֱ���
				if(!tSrcMt.IsNull() && ChgMtVidFormatRequired(tSrcMt) )
				{
					ChangeMtVideoFormat(tSrcMt, FALSE);
				}
			}

			m_tConfPollParam.SetLastPolledMt( tSrcMt );
		}
		break;

	case MODE_VIDEO_SPY:
    case MODE_BOTH_SPY:
		{
			if( !tLastMt.IsNull() )
			{
				if( MODE_VIDEO_SPY == tPollInfo.GetMediaMode() )
				{
					FreeRecvSpy( tLastMt, MODE_VIDEO );
				}
				else
				{
					FreeRecvSpy( tLastMt, MODE_BOTH );
				}
			}

			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));				
			TSimCapSet tDstCapSet = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());
			tSimCapSet.SetVideoMaxBitRate(tDstCapSet.GetVideoMaxBitRate());
			if (tSimCapSet.IsNull())
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, " GetMtSimCapSetByMode is null!\n");
				return;
			}
			
			SendMMcuSpyNotify( tSrcMt, dwEvId, tSimCapSet);		
			OnSetOutView(tSrcMt, bySpyMode);
			m_tConfPollParam.SetLastPolledMt( tSrcMt );
		}
		break;

    case MODE_VIDEO_BOTH:
		{				
			u8 bySrcChnnl = ( GetLocalMtFromOtherMcuMt(tLastMt) == m_tPlayEqp ? m_byPlayChnnl : 0);
			
			//������Ƶ�㲥
			FreeRecvSpy( tLastMt, MODE_VIDEO );
			if (!bIsVmpBrd)
			{
				ChangeVidBrdSrc( &tSrcMt );
			}
			//�ͷ��ϴ�ͨ���ش���Դ
			if( !tLastMt.IsNull() )
			{
				FreeRecvSpy( tLastMt, MODE_VIDEO );
			}
			
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));				
			TSimCapSet tDstCapSet = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());
			tSimCapSet.SetVideoMaxBitRate(tDstCapSet.GetVideoMaxBitRate());
			if (tSimCapSet.IsNull())
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, " GetMtSimCapSetByMode is null!\n");
				return;
			}
			
			SendMMcuSpyNotify( tSrcMt, dwEvId, tSimCapSet);		
			OnSetOutView(tSrcMt, bySpyMode);
			m_tConfPollParam.SetLastPolledMt( tSrcMt );
		}
        break;

    case MODE_BOTH_BOTH:
        {
			//���ط���        
			ChangeSpeaker(&tSrcMt, TRUE, bVmpNeedChgBySpeaker, FALSE ); // 2011-10-8 mod by pgf:��������VMP��Ա����ѯ�������ڸı䷢�����в���Ҫ��������
			
			if( !tLastMt.IsNull() )
			{
				FreeRecvSpy( tLastMt, MODE_BOTH );
			}
			
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));				
			TSimCapSet tDstCapSet = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());
			tSimCapSet.SetVideoMaxBitRate(tDstCapSet.GetVideoMaxBitRate());
			if (tSimCapSet.IsNull())
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, " GetMtSimCapSetByMode is null!\n");
				return;
			}
			
			SendMMcuSpyNotify( tSrcMt, dwEvId, tSimCapSet);		
			OnSetOutView(tSrcMt, bySpyMode);
			m_tConfPollParam.SetLastPolledMt( tSrcMt );
		}
        break;

    default:
        ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY,  "[ProcMcsMcuPollMsg] unexpected poll mode.%d rcved, ignore it!\n",bySpyMode );
        break;
	}

	// �ȶ�ش���Դ���ͷŸɾ���,����vmp����
	if (!bVmpNeedChgBySpeaker)
	{
		u8 byVmpMemType = VMP_MEMBERTYPE_POLL;
		ChangeVmpChannelParam(&tCurPolledMt/*&tLocalMt*/, byVmpMemType, &tLastMt);
	}
				
	SetTimer( MCUVC_POLLING_CHANGE_TIMER, 1000 * wKeepTime );
	// ֪ͨ��һ����������ѯ�����ն�
	NotifyMtToBePolledNext();
	
	CServMsg cServMsg;
	cServMsg.SetMsgBody( (u8*)&tPollInfo, sizeof(TPollInfo) );
	SendMsgToAllMcs( MCU_MCS_POLLSTATE_NOTIF, cServMsg );

	//֪ͨ�¼�������
	if (!tSrcMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()))
	{
		TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));
		SendMMcuSpyNotify( tSrcMt, dwEvId , tSimCapSet);
	}	
	return;
}

/*==============================================================================
������    :  ProcTVWStartPreSetInAck
����      :  ��������ش��¼��ն˽��ϼ�����ǽ���δ�������ҵ���߼�
�㷨ʵ��  :  
����˵��  :  [IN] TPreSetInRsp ��ش���Ҫ��Ϣ
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010/07/19   4.6           ½����							��ش�����ǽ����
==============================================================================*/
void CMcuVcInst::ProcTWStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrcMt    = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	u8 byTvWallId = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_tTvWall.GetEqpId();
	u8  bySpyMode = tPreSetInRsp.m_byRspSpyMode;
	u32 EvId      = tPreSetInRsp.m_tSetInReqInfo.GetEvId();
    TSpyTVWallInfo tSpyTVWallInfo = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo;	

	TPeriEqpStatus tTvwStatus;
    g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTvwStatus);

	// [10/24/2011 liuxu] 
	TTvwMember tTvwMember = tTvwStatus.m_tStatus.tTvWall.atVideoMt[tSpyTVWallInfo.m_byDstChlIdx];
	if (tTvwMember.GetConfIdx() != 0 && tTvwMember.GetConfIdx() != m_byConfIdx)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcTWStartPreSetInAck] hdu chnnl has been occupyed by other conf\n");
		FreeRecvSpy(tSrcMt, bySpyMode);
		return;
	}
	
	// [10/24/2011 liuxu] ״̬����, ����ֹͣ����ǽ������Ӧ����ǽ��ѯ��timer��presetin ack
	if( tSpyTVWallInfo.m_bySrcMtType == TW_MEMBERTYPE_TWPOLL )
	{
		if (tTvwMember.byMemberType != TW_MEMBERTYPE_TWPOLL)
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcTWStartPreSetInAck] poll has been stoped\n");
			FreeRecvSpy(tSrcMt, bySpyMode);
			return;
		}
		
		u8 byPollState = POLL_STATE_NONE;
		m_tTWMutiPollParam.GetPollState(byTvWallId, tSpyTVWallInfo.m_byDstChlIdx, byPollState);
		if (POLL_STATE_NORMAL != byPollState)
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcTWStartPreSetInAck] poll mode has been changed\n");
			FreeRecvSpy(tSrcMt, bySpyMode);
			return;
		}
	}

 	BOOL32 bNeedSwitchMt = FALSE;

	ChangeTvWallSwitch(&tSrcMt, byTvWallId, tSpyTVWallInfo.m_byDstChlIdx, 
		tSpyTVWallInfo.m_bySrcMtType, TW_STATE_START,FALSE, bySpyMode );
	
	if (   VCS_CONF == m_tConf.GetConfSource() 
		&& ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode())
		&& m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_AUTO_MODE)
	{
		// [5/31/2011 liuxu] ���ṩ�ӿ�GetConfCfgedTvwIdx��Щ���µĴ���
		const u8 wCurrTwPosId = GetConfCfgedTvwIdx( tSpyTVWallInfo.m_tTvWall.GetEqpId(), tSpyTVWallInfo.m_byDstChlIdx);
		m_cVCSConfStatus.SetCurUseTWChanInd( wCurrTwPosId );

		CRecvSpy tSrcSpyInfo;
		if( m_cSMcuSpyMana.GetRecvSpy(tSrcMt, tSrcSpyInfo) &&
			TSpyStatus::WAIT_FREE == tSrcSpyInfo.m_byUseState )
		{
			m_cSMcuSpyMana.ModifyUseState( tSrcMt,TSpyStatus::NORMAL );
		}

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcHduStartPreSetInAck] CURRENT TW AND HDU CHN NUM (%d) CURRENTID(%d)\n",GetVcsHduAndTwModuleChnNum(),wCurrTwPosId);
	}
	return;
}

/*==============================================================================
������    :  ProcRollCallPreSetInAck
����      :  ��������ش��¼��ն�����������ʱ���δ��������߼�
�㷨ʵ��  :  
����˵��  :  [IN] TPreSetInRsp ��ش���Ҫ��Ϣ
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-11-05                 pengjie							create
==============================================================================*/
void CMcuVcInst::ProcRollCallPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tNewRollCallee  = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	u8  bySpyMode       = tPreSetInRsp.m_byRspSpyMode;
	u32 EvId            = tPreSetInRsp.m_tSetInReqInfo.GetEvId();
	TMt tNewRollCaller  = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tCaller;
	TMt tOldRollCaller  = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tOldCaller;
	TMt tOldRollCallee  = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tOldCallee;
    BOOL32 bCallerChged = tNewRollCaller == tOldRollCaller ? FALSE : TRUE;
    BOOL32 bCalleeChged = tNewRollCallee == tOldRollCallee ? FALSE : TRUE;

	/*20110809 zjl �������˴�PreSetInAck��¼���ͷ�����ѡ����ҵ�񱣳�һ�£�
				   ��ֹ�¼��ն˵����������յ�ͨ���ȶ�tmtȡ��ҵ��ʧ��*/
	m_tRollCallee = tNewRollCallee;


	if ( bCalleeChged &&
         ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
    {
		// KDV-BUG2004: �����˻򱻵����˱�ǿ��Ϊ�����ˣ�����MTC���ն�״̬δ�ı�
		// zgc, 2008-05-21, ��ChangeSpeaker��������
        //m_tConf.SetSpeaker( tNewRollCallee );
		ChangeSpeaker( &tNewRollCallee,FALSE,TRUE,FALSE );
    }
	
	//zhouyiliang 20120921 ����ʱ��������ģʽ�ͱ�������ģʽ���¼�����ack����Ϊ��ε����ɹ���
	//�����뱻������ģʽ�����vmp����notify����Ϊ���ε����ɹ�
	if (m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_CALLEE || ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcRollCallPreSetInAck]Recover Rollcall finish Mark \n" );
		SetLastMultiSpyRollCallFinish(TRUE);
	}

	//��������
    BOOL32 bLocalMixStarted = FALSE;

	if ( m_tConf.m_tStatus.GetMixerMode() != mcuNoMix)
	{	
		BOOL32 bRemoveMixMember = FALSE;
		BOOL32 bLocalAutoMix = FALSE;
		
		// ͣ��������
		if ( m_tConf.m_tStatus.IsVACing() )
		{
			bRemoveMixMember = TRUE;
			m_tConf.m_tStatus.SetVACing(FALSE);
			//֪ͨ��ϯ�����л��

			CServMsg cServMsg;
			cServMsg.SetEventId(MCU_MCS_STOPVAC_NOTIF);
			SendMsgToAllMcs(MCU_MCS_STOPVAC_NOTIF, cServMsg);
			if ( HasJoinedChairman() )
			{
				cServMsg.SetEventId(MCU_MT_STOPVAC_NOTIF);
				SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cServMsg);
			}
		}

		//ͣ����
		if ( m_tConf.m_tStatus.IsAutoMixing() )
		{
			bRemoveMixMember = TRUE;
			bLocalAutoMix = TRUE;		
			m_tConf.m_tStatus.SetAutoMixing(FALSE);
		}
		
		if (bRemoveMixMember)
		{
			TMt atDstMt[MAXNUM_CONF_MT];
			memset(atDstMt, 0, sizeof(atDstMt));
			u8  byDstMtNum = 0;
			for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
			{
				if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
				{
					TMt tMt = m_ptMtTable->GetMt(byMtId);
					RemoveMixMember(&tMt, FALSE);
					StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
						(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+byMtId), FALSE, MODE_AUDIO);
					
					if (bLocalAutoMix)
					{
						//zjl 20110510 StopSwitchToSubMt �ӿ������滻
						//StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);
						atDstMt[byDstMtNum] = tMt;
						byDstMtNum ++;
					}
					
					// �ָ����鲥��ַ����
					/*if (m_tConf.GetConfAttrb().IsSatDCastMode() && m_ptMtTable->IsMtInMixGrp(byMtId))
					{
						ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_AUDIO);
					}*/
				}
			}

			if (byDstMtNum > 0)
			{
				StopSwitchToSubMt(byDstMtNum, atDstMt, MODE_AUDIO, FALSE);
			}
		}

		if ( bCallerChged  || bCalleeChged)
		{
			//�������Ƕ��ƻ���
			if ( m_tConf.m_tStatus.IsSpecMixing() )
			{
				//��һ�ε������Ƴ���ǰ�����ն�
				if ( tOldRollCaller.IsNull() || tOldRollCallee.IsNull())
				{
					for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
					{							
						if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
							m_ptMtTable->IsMtInMixing(byMtId))
						{
							TMt tMt = m_ptMtTable->GetMt(byMtId);
							RemoveSpecMixMember(&tMt, 1, FALSE, FALSE);
						}
					}
				}
			}
		}
		
		m_tConf.m_tStatus.SetSpecMixing(TRUE);

		if (bCallerChged)
		{
			if ( !tOldRollCaller.IsNull() )
            {
				RemoveSpecMixMember(&tOldRollCaller, 1, FALSE, FALSE);
			}

			AddSpecMixMember(&tNewRollCaller, 1, FALSE);
		}

		if (bCalleeChged)
		{
			BOOL32 bInSameSMcu = FALSE;
			
			if (!tOldRollCallee.IsLocal() && 
				!tNewRollCallee.IsLocal() && 
				tOldRollCallee.GetMcuId() == tNewRollCallee.GetMcuId())
			{
				bInSameSMcu = TRUE;
			}
			
			BOOL32 bStopNoMix = FALSE;
			
			if (!bInSameSMcu)
			{
				bStopNoMix = TRUE;
			}
			
			AddSpecMixMember(&tNewRollCallee, 1, FALSE);
			if ( !tOldRollCallee.IsNull() )
			{
				RemoveSpecMixMember(&tOldRollCallee, 1, FALSE, bStopNoMix);
			}
			
		}

		// �����ǰ����������ģʽ, ������Nģʽ�Ľ���
		if ( bRemoveMixMember )
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
	}
	else
	{
		//����ʱ�����⿼�ǣ�ֻStartһ��
		if ( !bLocalMixStarted )
		{
			bLocalMixStarted = TRUE;
			
			CServMsg cMsg;
			cMsg.SetMsgBody((u8*)&tNewRollCaller, sizeof(TMt));
			cMsg.CatMsgBody((u8*)&tNewRollCallee, sizeof(TMt));
			ProcMixStart(cMsg);
		}                    
    }

	//�����㲥����
	u8 byVmpCount = GetVmpCountInVmpList();
	u8 byVmpId = GetTheMainVmpIdFromVmpList();
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	TVMPParam_25Mem tVmpParam;
	tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	
	if ( m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP )
	{
		tVmpParam.SetVMPBrdst(TRUE);
		tVmpParam.SetVMPMode(CONF_VMPMODE_CTRL);
		tVmpParam.SetVMPAuto(FALSE);
		tVmpParam.SetVMPSchemeId(0);
		tVmpParam.SetIsRimEnabled(FALSE);
		
#ifdef _MINIMCU_
		tVmpParam.SetVMPStyle(VMP_STYLE_VTWO);
#else
		tVmpParam.SetVMPStyle(VMP_STYLE_HTWO);
		u16 wError = 0;
		BOOL32 bRet;
		if (byVmpCount == 0)//��ǰû�ϳ���ʱ����Ҫ�Һϳ���
		{
			bRet = IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError);
		}  
		else
		{
			bRet = IsVMPSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, wError);
		}
		if ( !bRet )
		{
			//NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_STYLECHANGE);           
			tVmpParam.SetVMPStyle(VMP_STYLE_VTWO);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] rollcall change to style.%d due to vmp ability is not enough!\n",
				tVmpParam.GetVMPStyle() );
		} 
#endif                
	}
	
	
	if ( bCallerChged )
	{
		if ( ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() )
		{
			if ( !tNewRollCaller.IsLocal() )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "New roll caller(Mt.%d) is not local mt, it's impossible\n", tNewRollCaller.GetMtId() );
			}
		}
		else if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
		{
			u8 byCallerPos = 0;
			if ( !tOldRollCaller.IsNull() &&
				g_cMcuVcApp.GetVMPMode(tVmpEqp) != CONF_VMPMODE_NONE )
			{
				byCallerPos = tVmpParam.GetChlOfMtInMember(tOldRollCaller);
			}
			//�ɵ����˲����ڣ�ռ0ͨ��
			if ( byCallerPos >= MAXNUM_VMP_MEMBER ) 
			{
				byCallerPos = 0;
			}
			TVMPMember tVmpMember;
			memset(&tVmpMember, 0, sizeof(tVmpMember));
			tVmpMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);
			tVmpMember.SetMemberTMt(tNewRollCaller);
			tVmpMember.SetMemStatus(MT_STATUS_AUDIENCE);
			tVmpParam.SetVmpMember(byCallerPos, tVmpMember);
		}
	}
	if ( bCalleeChged )
	{
		
		if ( ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
		{
			//FreeRecvSpy(tOldRollCallee, MODE_VIDEO);
		}
		else if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
		{			
			u8 byCalleePos = 1;
			if ( !tOldRollCallee.IsNull() &&
				g_cMcuVcApp.GetVMPMode(tVmpEqp) != CONF_VMPMODE_NONE )
			{
//				byCalleePos = tVmpParam.GetChlOfMtInMember(GetLocalMtFromOtherMcuMt(tOldRollCallee));
				// [pengjie 2010/4/14] ���ﲻ����local������Ϣ
				byCalleePos = tVmpParam.GetChlOfMtInMember(tOldRollCallee);
			}
			
			//�ɵ����˲����ڣ�ռ1ͨ��
			if ( byCalleePos >= MAXNUM_VMP_MEMBER )
			{
				byCalleePos = 1;
			}
			TVMPMember tVmpMember;
			memset(&tVmpMember, 0, sizeof(tVmpMember));
			tVmpMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);
//			tVmpMember.SetMemberTMt(GetLocalMtFromOtherMcuMt(tNewRollCallee));
			// [pengjie 2010/4/14] ���ﲻ����local������Ϣ
			tVmpMember.SetMemberTMt(tNewRollCallee);
			tVmpMember.SetMemStatus(MT_STATUS_AUDIENCE);
			tVmpParam.SetVmpMember(byCalleePos, tVmpMember);
			
		}
	}
	
	//����ѡ��
	// zbq [11/24/2007] VMPʱ������������VMPģʽ�����ֶ��嵱ǰ��Ƶѡ������������µ�ѡ�������Զ������.
	// ���������˺ͱ�������֮������������ն˵�ѡ��ȫͣ��
	if ( ( tOldRollCaller.IsNull() || tOldRollCallee.IsNull() ) &&
		ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
	{
		for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
		{
			// miaoqingsong [20110916] �����ϼ�MCUѡ���ϴ�ͨ���ն˵�ѡ��
			if (!m_tCascadeMMCU.IsNull() && 
				m_tCascadeMMCU.GetMtId() == byMtId)
			{
				continue;
			}

			if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
				byMtId != tNewRollCaller.GetMtId() &&
				( (tNewRollCallee.IsLocal() && byMtId != tNewRollCallee.GetMtId()) || (!tNewRollCallee.IsLocal() && byMtId != tNewRollCallee.GetMcuId()) ) 
				) 
			{
				TMtStatus tMtStatus;
				if ( m_ptMtTable->GetMtStatus( byMtId, &tMtStatus ) &&
					!tMtStatus.GetSelectMt(MODE_VIDEO).IsNull() )
				{
					TMt tMt = m_ptMtTable->GetMt(byMtId);
					StopSelectSrc(tMt, MODE_VIDEO);
				}
			}
		}
	}
	
	BOOL32 bSelAdjusted = FALSE;
	
	if ( bCallerChged )
	{
		if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
		{
			if (!tOldRollCaller.IsNull())
			{
				StopSelectSrc(tOldRollCaller, MODE_VIDEO);         
			}
			
			//zyl��20121106��һ�ε�����bCallerChged��bCalleeChged��Ϊtrue,Ҫͣ������ǰ��ѡ��
			if (bCalleeChged)
			{
				StopSelectSrc(tNewRollCaller, MODE_VIDEO,FALSE);
			}
			       
			

			//�����˸ı��ˣ���ͣ���������˵�ѡ�����ͷ���Դ
			if( tNewRollCallee.IsLocal() )
			{
				StopSelectSrc(tNewRollCallee, MODE_VIDEO,FALSE); 
			}
			
			//ѡ��ʧ�ܣ��ָ����㲥�����ܿ�vmp���Լ�
			//������ѡ����������
			if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
			{
				RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
				//[2011/09/15/zhangli]�ͷŻش���ChangeSelectSrc������
				//FreeRecvSpy( tNewRollCallee,MODE_VIDEO );
			}
			
			//��������ѡ��������
			if ( tNewRollCallee.IsLocal() )
			{
				if (!ChangeSelectSrc(tNewRollCaller, tNewRollCallee, MODE_VIDEO))
				{
					RestoreRcvMediaBrdSrc(tNewRollCallee.GetMtId(), MODE_VIDEO);
				}
			}
			bSelAdjusted = TRUE;
		}
		else if ( ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() )
		{
			if ( !tOldRollCaller.IsNull() )
			{
				StopSelectSrc(tOldRollCaller, MODE_VIDEO);                    
			}
			//zyl��20121106��һ�ε�����bCallerChged��bCalleeChged��Ϊtrue,Ҫͣ������ǰ��ѡ��
			if ( bCalleeChged )
			{
				StopSelectSrc(tNewRollCaller,MODE_VIDEO);
			}
			if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
			{
				RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
				//[2011/09/15/zhangli]�ͷŻش���ChangeSelectSrc������
				//FreeRecvSpy( tNewRollCallee,MODE_VIDEO );
			}
		}
		else // ROLLCALL_MODE_CALLEE
		{
			//zjj20091031
			if( tNewRollCallee.IsLocal() )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcsMcuRollCallMsg] bCallerChged(%d) bCalleeChged(%d) StopSelectSrc(tNewRollCallee)\n",
					bCallerChged,bCalleeChged );
				StopSelectSrc(tNewRollCallee, MODE_VIDEO,FALSE );
			}

			if ( tNewRollCallee.IsLocal() )
			{
				if (!ChangeSelectSrc(tNewRollCaller, tNewRollCallee, MODE_VIDEO))
				{
					RestoreRcvMediaBrdSrc(tNewRollCallee.GetMtId(), MODE_VIDEO);
				}
			}
		}
	}
	if ( bCalleeChged )
	{
		if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
		{
//  [12/21/2009 pengjie]
			/*
 			if ( tOldRollCallee.IsLocal() &&
 				!tOldRollCallee.IsNull() )
 			{
 				StopSelectSrc(tOldRollCallee, MODE_VIDEO);
 			}
            */

			
 			if( bCallerChged &&
 				!tOldRollCaller.IsNull() &&
				!tNewRollCaller.IsNull() 
				)
			{						
				StopSelectSrc(tNewRollCaller, MODE_VIDEO,FALSE);
			}
			
 			if( !bCallerChged )
 			{
 				StopSelectSrc(tNewRollCaller, MODE_VIDEO,FALSE);
 			}

			if ( tOldRollCallee.IsLocal() &&
                !tOldRollCallee.IsNull() )
            {
                StopSelectSrc(tOldRollCallee, MODE_VIDEO);
				//zjl�л�һ�α������ˣ����ǰһ���������˵��Ž���
				//g_cMpManager.RemoveSwitchBridge(tOldRollCallee, 0, MODE_VIDEO);
            }		
// End
			if ( !bSelAdjusted )
			{
				if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
				{
					RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
					//[2011/09/15/zhangli]�ͷŻش���ChangeSelectSrc������
					//FreeRecvSpy( tNewRollCallee,MODE_VIDEO );
				}
				if ( tNewRollCallee.IsLocal() )
				{
					if (!ChangeSelectSrc(tNewRollCaller, tNewRollCallee, MODE_VIDEO))
					{
						RestoreRcvMediaBrdSrc(tNewRollCallee.GetMtId(), MODE_VIDEO);
					}
				}
			}
		}
		else if ( ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() )
		{
			//zjj20091031 
			if( !bCallerChged )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcsMcuRollCallMsg] bCallerChged(%d) bCalleeChged(%d) StopSelectSrc(tOldRollCaller)\n",
					bCallerChged,bCalleeChged );
				StopSelectSrc(tOldRollCaller, MODE_VIDEO,FALSE );
			}
			
			if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
			{
				RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
				//[2011/09/15/zhangli]�ͷŻش���ChangeSelectSrc������
				//FreeRecvSpy( tNewRollCallee,MODE_VIDEO );
			}
		}
		else // ROLLCALL_MODE_CALLEE
		{
			if ( tOldRollCallee.IsLocal() &&
				!tOldRollCallee.IsNull() )
			{
				StopSelectSrc(tOldRollCallee, MODE_VIDEO);
			}
			if ( tNewRollCallee.IsLocal() )
			{
				if (!ChangeSelectSrc(tNewRollCaller, tNewRollCallee, MODE_VIDEO))
				{
					RestoreRcvMediaBrdSrc(tNewRollCallee.GetMtId(), MODE_VIDEO);
				}
			}
		}
	}

	if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
	{
		// �л����vmp����ģʽ
		if ( CONF_VMPMODE_CTRL == g_cMcuVcApp.GetVMPMode(m_tVmpEqp) )
		{
			/*TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
			tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus); */ 
			
			//ChangeVmpParam(&tVmpParam);
			// xliang [1/6/2009] ��������VMP��VMP param
			AdjustVmpParam(tVmpEqp.GetEqpId(), &tVmpParam);
			
		}
		else if ( CONF_VMPMODE_AUTO == g_cMcuVcApp.GetVMPMode(m_tVmpEqp) )
		{		
			/*TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
			tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus); */ 
			
			//ChangeVmpParam(&tVmpParam);
			// xliang [1/6/2009] ��������VMP��VMP param
			AdjustVmpParam(tVmpEqp.GetEqpId(), &tVmpParam);
			
			ConfModeChange();
		}
		else
		{
			CServMsg cMsg;
			cMsg.SetEventId(MCS_MCU_STARTVMP_REQ);
			cMsg.SetMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
			
			VmpCommonReq(cMsg);
		}
	}
	
	if( !tNewRollCallee.IsLocal() &&
		IsLocalAndSMcuSupMultSpy(tNewRollCallee.GetMcuId())
		)
	{		
		TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tNewRollCallee.GetMcuId()));	
		SendMMcuSpyNotify( tNewRollCallee, EvId, tSimCapSet);		
	}
	
	CServMsg cServMsg;
	cServMsg.SetMsgBody( (u8*)&m_tRollCaller, sizeof(TMt) );
	cServMsg.CatMsgBody( (u8*)&tNewRollCallee, sizeof(TMt) );
	cServMsg.SetEventId( MCU_MCS_CHANGEROLLCALL_NOTIF );
	SendMsgToAllMcs( cServMsg.GetEventId(), cServMsg );
	
	ConfStatusChange();
	
	return;
}

/*==============================================================================
������    :  ProcRecStartPreSetInAck
����      :  ��������ش��¼��ն˽��ϼ�¼���
�㷨ʵ��  :  
����˵��  :  [IN] TPreSetInRsp ��ش���Ҫ��Ϣ
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010/07/19   4.6           ½����							��ش�����ǽ����
==============================================================================*/
void CMcuVcInst::ProcRecStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tRecordMt = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();

	// [11/23/2010 liuxu] ֪ͨ�¼�mcu��¼�ش���Ϣ
	if (!tRecordMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tRecordMt.GetMcuId()))
	{
		TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tRecordMt.GetMcuId()));
		SendMMcuSpyNotify( tRecordMt,MCS_MCU_STARTREC_REQ, tSimCapSet);
	}

	const TSpyRecInfo *ptSpyRecInfo = &tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRecInfo;

	TStartRecMsgInfo tRecMsg;
	tRecMsg.m_tRecordMt = tRecordMt;
	tRecMsg.m_tRecEqp = ptSpyRecInfo->m_tRec;
	tRecMsg.m_tRecPara = ptSpyRecInfo->m_tRecPara;
	tRecMsg.bIsRecAdaptConf = FALSE;

	//  [5/16/2013 guodawei] ��ش�¼���ļ���GBKת��
	s8  achRecFileName[MAX_FILE_NAME_LEN] = {0};
	memcpy(achRecFileName, ptSpyRecInfo->m_szRecFullName, sizeof(achRecFileName) - 1);
	u16 wRecNameLen = htons(strlen(achRecFileName) + 1);
// #ifdef _UTF8
// 	if (g_cMcuVcApp.GetEqpCodeFormat(tRecMsg.m_tRecEqp.GetEqpId()) == emenCoding_GBK)
// 	{
// 		s8 achGBKFileName[MAX_FILE_NAME_LEN] = {0};
// 		u16 wRet = gb2312_to_utf8(achRecFileName, achGBKFileName, sizeof(achGBKFileName) - 1);
// 		wRecNameLen = htons(strlen(achGBKFileName) + 1);
// 		memcpy(achRecFileName, achGBKFileName, sizeof(achGBKFileName));
// 	}
// #endif
	memcpy(tRecMsg.m_aszRecName, achRecFileName, sizeof(achRecFileName));

	// vrs��¼��֧��
	if (tRecMsg.m_tRecEqp.GetType() == TYPE_MT && tRecMsg.m_tRecEqp.GetMtType() == MT_TYPE_VRSREC)
	{
		StartVrsRec(tRecMsg, TRecChnnlStatus::STATE_RECREADY, FALSE);
	}
	else
	{
		StartMtRec(tRecMsg, FALSE,ptSpyRecInfo);
	}

	return;
}

/*==============================================================================
������    :  ProcRecStartPreSetInAck
����      :  ��������ش��¼��ն˽��ϼ��ش�ͨ��
�㷨ʵ��  :  
����˵��  :  [IN] TPreSetInRsp ��ش���Ҫ��Ϣ
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2011/07/06   4.6           ������			                 ����
==============================================================================*/
void CMcuVcInst::ProcDragStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrcMt    = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	u8  bySpyMode = tPreSetInRsp.m_byRspSpyMode;
	u32 dwEvId    = tPreSetInRsp.m_tSetInReqInfo.GetEvId();

	TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));				
	TSimCapSet tDstCapSet = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());
	tSimCapSet.SetVideoMaxBitRate(tDstCapSet.GetVideoMaxBitRate());
	if (tSimCapSet.IsNull())
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, " GetMtSimCapSetByMode is null!\n");
		return;
	}

	SendMMcuSpyNotify( tSrcMt, dwEvId, tSimCapSet);		
	OnSetOutView(tSrcMt, bySpyMode);
}

/*==============================================================================
������    :  StopSpeakerFollowSwitch
����      :  ֹͣ�����˸����һЩ����
�㷨ʵ��  :  
����˵��  :  byMode Ҫֹͣ�Ľ�����ģʽ(MODE_VIDEO,MODE_AUDIO,MODE_BOTH)

����ֵ˵��:  

-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
10/03/26					�ܾ���							create
==============================================================================*/
void CMcuVcInst::StopSpeakerFollowSwitch( u8 byMode )
{
	TMt tSpeaker = m_tConf.GetSpeaker();
	if( MODE_BOTH == byMode || MODE_VIDEO == byMode  )
	{	
		TPeriEqpStatus tTWStatus;
		u8 byChnlIdx;
		u8 byEqpId;
		for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
		{
			if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
			{
				if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus))
				{
					u8 byMemberType;
					u8 byMtConfIdx;
					for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
					{
						byMemberType = tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType;
						byMtConfIdx = tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx();
						if (TW_MEMBERTYPE_SPEAKER == byMemberType && m_byConfIdx == byMtConfIdx)
						{
							//zjl[03/01/2010] �����˸��棬ָ���¼��ն�Ϊ������ʱ����Ҫ�������ֱ��ʣ���������Դ�˱����Ƿ�local
							ChangeTvWallSwitch(&tSpeaker/*tLocalNewSpeaker*/, byEqpId, byChnlIdx, TW_MEMBERTYPE_SPEAKER, TW_STATE_STOP);
						}
					}
				}
			}
		}

		
		TPeriEqpStatus tHduStatus;
		u8 byHduChnlIdx;
		u8 byHduEqpId;
		for (byHduEqpId = HDUID_MIN; byHduEqpId <= HDUID_MAX; byHduEqpId++)
		{
			if(IsValidHduEqp(g_cMcuVcApp.GetEqp(byHduEqpId)))
			{
				u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byHduEqpId));
				if (0 == byHduChnNum)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[StopSpeakerFollowSwitch] GetHduChnNumAcd2Eqp failed!\n");
					continue;
				}

				if (g_cMcuVcApp.GetPeriEqpStatus(byHduEqpId, &tHduStatus))
				{
					u8 byMemberType;
					u8 byMtConfIdx;
					for (byHduChnlIdx = 0; byHduChnlIdx < byHduChnNum; byHduChnlIdx++)
					{
						byMemberType = tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].byMemberType;
						byMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetConfIdx();
						if (TW_MEMBERTYPE_SPEAKER == byMemberType && m_byConfIdx == byMtConfIdx )
						{
							// [2013/03/11 chenbing] HDU�໭�治֧�ַ����˸���,��ͨ����0
							ChangeHduSwitch(NULL, byHduEqpId, byHduChnlIdx, 0, TW_MEMBERTYPE_SPEAKER, TW_STATE_STOP);
						}
					}
				}
			}
		}
	}
}

/*==============================================================================
������    :  FreeSpyChannlInfoByMcuId
����      :  �ͷŴ�mcu�µ����лش��ն˼�ռ��ͨ��
�㷨ʵ��  :  
����˵��  :  u8 byMcuId McuID
����ֵ˵��: 
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-06-10                 lukunpeng						modify
==============================================================================*/
// void CMcuVcInst::FreeSpyChannlInfoByMcuId(u8 byMcuId)
// {
// 	CMultiSpyMgr *pcMultiSpyMgr = g_cMcuVcApp.GetCMultiSpyMgr();
// 
// 	if (pcMultiSpyMgr == NULL)
// 	{
// 		return;
// 	}
// 
// 	m_cSMcuSpyMana.FreeSpyChannlInfoByMcuId(byMcuId, pcMultiSpyMgr);
// }

/*==============================================================================
������    :  FreeAllRecvSpyByMcuIdx
����      :  �ͷ�ĳ��mcu�µ����лش��ն�
�㷨ʵ��  :  
����˵��  :  u16 wMcuIdx Ҫ�ͷ��ն����ڵ�mcu��mcuidx            

����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-08-27                 �ܾ���							����
==============================================================================*/
void CMcuVcInst::FreeAllRecvSpyByMcuIdx( u16 wMcuIdx )
{
	if( INVALID_MCUIDX == wMcuIdx )
	{
		return;
	}
	
	const CRecvSpy *pcRecvSpy;
	u16 wSpyNum = MAXNUM_CONF_SPY/*m_cSMcuSpyMana.GetSpyNum()*/;
	for( u16 wIdx = 0;wIdx < wSpyNum;++wIdx )
	{
		pcRecvSpy = m_cSMcuSpyMana.GetSpyMemberInfo( wIdx );

		if (NULL == pcRecvSpy)
		{
			continue;
		}

		if (pcRecvSpy->m_tSpy.IsNull())
		{
			continue;
		}

		if( pcRecvSpy->m_tSpy.GetMcuIdx() == wMcuIdx )
		{
			//zhouyiliang 20110113 ���¼�mcu��ͬʱͣ����mcu�����ϼ�mcu�Ľ���
			if ( !m_tCascadeMMCU.IsNull() ) 
			{
				StopSpyMtSpySwitch( pcRecvSpy->m_tSpy );
			}
			FreeRecvSpy( pcRecvSpy->m_tSpy, MODE_BOTH, TRUE );
		}
	}	
}
/*==============================================================================
������    :  FreeRecvSpy
����      :  �ͷű������ɵĻش��ն���ռ�еĻش���Դ
�㷨ʵ��  :  
����˵��  :  tMt        �ش�Դ
             bySpyMode  Ҫ�ͷŵĻش�ģʽ��MODE_VIDEO/MODE_BOTH....��
			 bForce     �Ƿ�Ҫǿ���ͷŸûش�ģʽ���ڻعҶϴ�Դ��ɾ������ߵ����
			             ǿ���ͷţ�Ĭ�ϲ���ΪFALSE ������ǿ���ͷţ�

����ֵ˵��:  TRUE       ��ʾ�ͷųɹ�
             FALSE      ��ʾ�ͷ�ʧ�ܣ�˵���ûش�Դ���ܻ�������Ŀ�Ļش��նˣ�����
			             ���޷��ͷ���ش���Դ��

-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-06-10                 lukunpeng						modify
==============================================================================*/
BOOL32 CMcuVcInst::FreeRecvSpy(const TMt tMt, u8 bySpyMode, BOOL32 bForce)
{
	if( tMt.IsNull() || tMt.IsLocal()  )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[FreeRecvSpy] tMt(%d.%d) is local or null .can't FreeSpy.\n",
			tMt.GetMcuId(),tMt.GetMtId()
			);
		return FALSE;
	}
	
	//�жϴ˼��������Ƿ�֧�ֶ�ش�
	if( !IsLocalAndSMcuSupMultSpy(tMt.GetMcuId()) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[FreeRecvSpy] tMt's mcu (%d.%d) can't Support MultiSpy.\n",
			tMt.GetMcuId(),tMt.GetMtId()
			);
		return FALSE;
	}
	
	//�ش��ն�Ŀ������һ����������ͷ�SpyMode���ͷ����ͷŴ�����������ͷ�Chnnl,����ChnnlID
	u32 dwReleaseBW = 0;
	s16 swSpyChnnlID = -1;

	//[2011/11/09/zhangli]��������־���Ƿ����rtcp��Ϣ����Ϊ��ProcMcuMcuMtExtInfoNotif�ｨrtcp�����ǻ�������ﱣ����ж��Ƿ��½�
	BOOL32 bIsRemoveVidRtcp = FALSE;
	BOOL32 bIsRemoveAudRtcp = FALSE;

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "FreeRecvSpy McuId:%d, MtId:%d, SpyMode:%d, bForce:%d\n", 
		tMt.GetMcuId(), tMt.GetMtId(), bySpyMode, bForce);

	CRecvSpy cRecvSpyInfo;
	if( !m_cSMcuSpyMana.GetRecvSpy(tMt, cRecvSpyInfo) )
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "FreeRecvSpy GetRecvSpy Failed mcu.%dmt.%d\n", \
			tMt.GetMcuId(), tMt.GetMtId() );
		return FALSE;
	}

	//20110818 zjl Ҫ���ͷŵ�ģʽ��ʵ�ʻش���ģʽȡ����
	bySpyMode = (cRecvSpyInfo.GetSpyMode() & bySpyMode);
	if (bySpyMode == 0)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[FreeRecvSpy] No simultaneous between SpyMode.%d and FreeMode.%d!\n",
													cRecvSpyInfo.GetSpyMode(), bySpyMode);
		return FALSE;
	}

	u8 byRelSpyMode = MODE_NONE;

	if (bForce)
	{
		if (!m_cSMcuSpyMana.RemoveSpyMode(tMt, bySpyMode, dwReleaseBW, swSpyChnnlID, byRelSpyMode))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[FreeRecvSpy] tMt's mcu (%d.%d) RemoveSpyMode Error\n",
				tMt.GetMcuId(),tMt.GetMtId()
				);
			return FALSE;
		}
	}
	else
	{	
		if (!m_cSMcuSpyMana.DecAndAdjustSpyMode(tMt, bySpyMode, dwReleaseBW, swSpyChnnlID, byRelSpyMode))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[FreeRecvSpy] tMt's mcu (%d.%d) DecAndAdjustSpyMode Error\n",
				tMt.GetMcuId(),tMt.GetMtId()
				);
			return FALSE;
		}
	}

	//��ô�mcu���ܴ���ʣ�����
	u32 dwMaxSpyBW = 0;
	s32 nRemainSpyBW = 0;
	if (!GetMcuMultiSpyBW(tMt.GetMcuId(), dwMaxSpyBW, nRemainSpyBW))
	{
		return FALSE;
	}
	
	//�ͷŴ��ն˵Ĵ���
	SetMcuSupMultSpyRemainBW(tMt.GetMcuId(), nRemainSpyBW + (s32)dwReleaseBW);
	
	if ( !m_tConfAllMcuInfo.IsSMcuByMcuIdx( tMt.GetMcuId() ) ||
		byRelSpyMode != MODE_NONE
		)
	{
		//[liu lijiu][20100828]���������RTCP����
		TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
		if (tConfAttrb.IsResendLosePack() )
		{
			u32 dwRcvMpIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( GetLocalMtFromOtherMcuMt(tMt).GetMtId()));
			//�����ƵRTCP����
			if(MODE_VIDEO == byRelSpyMode || MODE_BOTH == byRelSpyMode)
			{
				g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRcvMpIp, cRecvSpyInfo.m_tSpyAddr.GetPort() + 1, cRecvSpyInfo.m_tVideoRtcpAddr.GetIpAddr(),
					                                 cRecvSpyInfo.m_tVideoRtcpAddr.GetPort());
				bIsRemoveVidRtcp = TRUE;
			}
			
			//�����ƵRTCP����
			if(MODE_AUDIO == byRelSpyMode || MODE_BOTH == byRelSpyMode)
			{
				g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRcvMpIp, cRecvSpyInfo.m_tSpyAddr.GetPort() + 3, cRecvSpyInfo.m_tAudioRtcpAddr.GetIpAddr(),
					                                  cRecvSpyInfo.m_tAudioRtcpAddr.GetPort());
				bIsRemoveAudRtcp = TRUE;
			}			
		}

		// [11/29/2010 xliang] �Ƿ�Ҫ�𽻻�ʹ��
		u8 bySpyNoUse = 0;
		bySpyNoUse |= byRelSpyMode;

		if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(tMt.GetMcuId()) )
		{
			byRelSpyMode = bySpyMode;
		}				
		
		//֪ͨ�¼����ͷŴ�SpyMode
		OnMMcuBanishSpyCmd( tMt, byRelSpyMode, bySpyNoUse );
	}
	
	if (bIsRemoveAudRtcp || bIsRemoveVidRtcp)
	{
		TTransportAddr tVideoRtcpAddr;
		tVideoRtcpAddr.SetNull();
		s16 swSpyIndex = m_cSMcuSpyMana.FindSpyMt(tMt);
		
		if (bIsRemoveAudRtcp)
		{
			m_cSMcuSpyMana.SetSpyBackAudRtcpAddr(swSpyIndex, MODE_AUDIO, tVideoRtcpAddr);	
		}
		
		if (bIsRemoveVidRtcp)
		{
			m_cSMcuSpyMana.SetSpyBackVidRtcpAddr(swSpyIndex, MODE_VIDEO, tVideoRtcpAddr);	
		}
	}

	//������ص�chnnIDΪ -1�Ļ���˵����ͨ������ʹ�ã������ͷ�
	if (swSpyChnnlID == -1)
	{
		//zjl20101117�����ǰ�ش��ն˻����ϼ�ҵ����(��δ�ͷ�)������Ҫ������ش�Ŀ��������
		UpdateCurSpyDstCapSet(tMt);
		return TRUE;
	}
	
	//��ʼ�ͷŴ�ͨ��
	CMultiSpyMgr *pcMultiSpyMgr = g_cMcuVcApp.GetCMultiSpyMgr();
	if( NULL == pcMultiSpyMgr )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "GetCMultiSpyMgr False! \n" );
		return FALSE;
	}
	pcMultiSpyMgr->ReleaseSpyChnnl(swSpyChnnlID);
	
	return TRUE;
}

/*==============================================================================
������    :  OnMMcuBanishSpyCmd
����      :  �ϼ������¼����ͷŻش���Դ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-10-15                 pengjie							create
==============================================================================*/
void CMcuVcInst::OnMMcuBanishSpyCmd(const TMt &tSrcMt, u8 bySpyMode, u8 bySpyNoUse )
{
	CServMsg cServMsg;
	TMsgHeadMsg tHeadMsg;


	cServMsg.SetEventId( MCU_MCU_BANISHSPY_CMD );
	TMt tMt;
	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tSrcMt,tMt );

	cServMsg.SetMsgBody( (u8*)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );
	cServMsg.CatMsgBody( (u8 *)&(bySpyMode), sizeof(u8) );
	cServMsg.CatMsgBody( (u8 *)&(bySpyNoUse), sizeof(bySpyNoUse) );

	
	SendMsgToMt( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuIdx()), MCU_MCU_BANISHSPY_CMD, cServMsg );
	return;
}

/*==============================================================================
������    :  ProcMcuMcuPreSetInNack
����      :  �¼��ش������㣬��NACK
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-10-15                 pengjie							create
==============================================================================*/
void CMcuVcInst::ProcMcuMcuPreSetInNack( const CMessage *pcMsg )
{
	STATECHECK;	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	TPreSetInRsp tPreSetInRsp = *(TPreSetInRsp *)(cServMsg.GetMsgBody());
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TPreSetInRsp));
	TMt tSrc = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	TEqp tDst = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_tVmp;
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  
		"[PreSetInNack]Req Mt(%d,%d)nacked, Error.%d EVENT %u(%s), RlsMt(%d,%d,%d)\n",
		tSrc.GetMcuIdx(), 
		tSrc.GetMtId(),
		cServMsg.GetErrorCode(),
		tPreSetInRsp.m_tSetInReqInfo.GetEvId(),
		::OspEventDesc((u16)tPreSetInRsp.m_tSetInReqInfo.GetEvId()),
		tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt.GetMcuIdx(),
		tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt.GetMtId(),
		tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_abyMtIdentify[0]
		);

	if( !m_tCascadeMMCU.IsNull() &&
		tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
		tHeadMsg.m_tMsgDst.m_byCasLevel > 0
		)
	{
		CServMsg cMsg;
		cMsg.SetErrorCode(cServMsg.GetErrorCode());
		cMsg.SetEventId( MCU_MCU_PRESETIN_NACK );
		cMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		cMsg.CatMsgBody( (u8*)&tPreSetInRsp,sizeof(TPreSetInRsp) );
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),cMsg.GetEventId(),cMsg );
		return;
	}

	tSrc = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() );
	// [12/15/2009 xliang] FIXME: ��errorcode. ����VMP�������ٸ���preSetIn�Ļ���// [3/16/2011 xliang] no way
	switch (cServMsg.GetErrorCode())
	{
// 	case  CASCADE_ADJRESNACK_REASON_NONEKEDA:
// 		{
// 			if( tDst.GetType() == TYPE_MCUPERI && tDst.GetEqpType() == EQP_TYPE_VMP && !tDst.IsNull() )
// 			{
// 				// tVmpParam���ڶ��Ʒ����˵�ǿ�����ȷ��ȡ��
// 				TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
// 				ChangeMtVideoFormat(tSrc, &tVmpParam, TRUE, TRUE, TRUE);
// 			}
// 			break;
// 		}
		// [1/4/2011 xliang] ����
	case ERR_MCU_CONFSNDBANDWIDTHISFULL:
	case ERR_MCU_CASDBASISNOTENOUGH:
	default:
		{	
			//������־�ûأ������´ε���
			if ( tPreSetInRsp.m_tSetInReqInfo.GetEvId() == MCS_MCU_CHANGEROLLCALL_REQ || 
				 (tPreSetInRsp.m_tSetInReqInfo.GetEvId() == MCS_MCU_STARTVMP_REQ && m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP )
			   )
			{
				StaticLog("[ProcMcuMcuPreSetInNack]Recover Rollcall finish Mark \n") ;
				SetLastMultiSpyRollCallFinish(TRUE);
			}
			BOOL32 bAlarm = TRUE;
			u8 byLoop1 = 0,byLoop = 0;
			TConfMcInfo *ptMcInfo = NULL;
			TConfMtInfo *ptConfMtInfo = NULL;

			BOOL32 bInConf = FALSE,bInJoinedConf = FALSE;
			u8 byMcuId = 0;
			TVMPParam_25Mem tConfVmpParam;
			TEqp tVmpEqp = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_tVmp;
			if( tPreSetInRsp.m_tSetInReqInfo.GetEvId() == MCS_MCU_STARTVMP_REQ )
			{
				tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
				TPeriEqpStatus tPeriEqpStatus; 
				if (  g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus ) && !tVmpEqp.IsNull() )
				{
					LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_EQP,"[ProcMcuMcuPreSetInNack] vmpstatus:%d!\n", tPeriEqpStatus.m_tStatus.tVmp.m_byUseState);
					
					switch(tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
					{
					case TVmpStatus::RESERVE://�����nack��RESERVE״̬����vmp״̬
						{
							tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
							tPeriEqpStatus.SetConfIdx( 0 );
							// Bug00082245,���¼��ն�,����ʱ,�յ�PresetinNack,��vmp״̬��idle,ͬʱ��Ҫkill����Timer
							u8 byVmpIdx = tVmpEqp.GetEqpId() - VMPID_MIN;
							KillTimer(MCUVC_WAIT_ALLVMPPRESETIN_ACK_TIMER+byVmpIdx);
							
							//״̬�б䣬��֪����
							g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
							cServMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
							SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
						}
						break;
					case TVmpStatus::WAIT_START://�ȴ����迪���У���������
						break;
					case TVmpStatus::START:
						{
							//�ָ�presetin֮ǰ��curUseVmpChanInd���籾��Ҫ��4���棬ͨ������ֻ��3���棬curUseVmpChan�Ͳ�����1������ԭ����3��
							if (tConfVmpParam.IsVMPAuto() && VCS_CONF == m_tConf.GetConfSource())
							{
								u16 byCurUseChnId = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos;
								m_cVCSConfStatus.SetCurUseVMPChanInd( byCurUseChnId );
								TMt tMtNull;
								tMtNull.SetNull();
								m_cVCSConfStatus.SetReqVCMT( tMtNull );	
								ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMcuMcuPreSetInNack] Update vcs CurUseVMPChanInd.%d:\n",byCurUseChnId);
							}
							//����ͨ��Ϊvmp��ͨ����ѯʱ,��Ҫ������ѯ��һ�ն�,8ki���ܣ��ݲ�֧��
							
							cServMsg.SetEqpId(tVmpEqp.GetEqpId());
							cServMsg.SetMsgBody( (u8*)&tConfVmpParam, sizeof(tConfVmpParam) );
							SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg ); //���߻��ˢ����
							//����EqpstatusVmpParam��Ϣ����m_tConfһ��
							tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tConfVmpParam);
						}
						break;
					default:
						break;
					}
					//else /*if(tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam.IsMtInMember(tSrc))*/
					/*{
						if (m_tConf.m_tStatus.m_tVMPParam.IsVMPAuto())
						{
							//�ָ�curUsechn����style
							u8 byStyl = m_tLastVmpParam.GetVMPStyle();
							m_tConf.m_tStatus.m_tVMPParam.SetVMPStyle( byStyl );
							
							//�ָ�presetin֮ǰ��curUseVmpChanInd���籾��Ҫ��4���棬ͨ������ֻ��3���棬curUseVmpChan�Ͳ�����1������ԭ����3��
							if ( VCS_CONF == m_tConf.GetConfSource() ) 
							{
								u16 byCurUseChnId = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos;
								m_cVCSConfStatus.SetCurUseVMPChanInd( byCurUseChnId );
								TMt tMtNull;
								tMtNull.SetNull();
								m_cVCSConfStatus.SetReqVCMT( tMtNull );				
								ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMcuMcuPreSetInNack] Update vcs CurUseVMPChanInd.%d:\n",byCurUseChnId);
							}
							else if(MCS_CONF == m_tConf.GetConfSource() )
							{
								//do nothing
							}
							
						}
						else //custom vmp
						{
							//u8 byChl = m_tConf.m_tStatus.m_tVMPParam.GetChlOfMtInMember( tSrc );
							u8 byChl = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos;
							// �ӱ���,��ֹȡ�õ�Chl����ȷ
							if (byChl >= MAXNUM_VMP_MEMBER)
							{
								ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuPreSetInNack]mt(%d,%d) can't join in vmp channel(%d)!\n",
									tSrc.GetMcuId(),tSrc.GetMtId(), byChl );
								break;
							}
							m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byChl );
							
							//ԭͨ����Ա�ָ�
							TVMPMember *pVmpMember = m_tLastVmpParam.GetVmpMember(byChl);
							
							if (pVmpMember != NULL)
							{
								m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byChl, *pVmpMember);
								// ����ͨ��Ϊvmp��ͨ����ѯʱ,��Ҫ������ѯ��һ�ն�
								if (VMP_MEMBERTYPE_VMPCHLPOLL == pVmpMember->GetMemberType() && POLL_STATE_NONE != m_tVmpPollParam.GetPollState())
								{
									TVmpPollInfo tPollInfo = m_tVmpPollParam.GetVmpPollInfo();
									u8 byPollIdx = m_tVmpPollParam.GetCurrentIdx();
									TMtPollParam * ptCurPollMt = GetNextMtPolledForVmp(byPollIdx, tPollInfo);
									// ��Ҫ��ѯ����һ���ն˾��ǵ�ǰ�ն�,��ʾ���д��ն�������ѯ����,�����ն��ֲ��ʺϽ�vmp,ͣ��ѯ
									if (!ptCurPollMt || tSrc == *ptCurPollMt )
									{
										ProcStopVmpPoll();
									} else
									{
										SetTimer(MCUVC_VMPPOLLING_CHANGE_TIMER, 10);
										bAlarm = FALSE;
									}
								}
							}
						}
						
						
					}
					
					// vmp��ͨ����ѯʱ,�յ�nack����Ҫ������ѯ��һ�ն�,����Ȳ�ˢ����
					if (bAlarm)
					{
						cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam) );
						SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg ); //���߻��ˢ����
					}
					
					tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
					//�������ϲ�������û��һ����Ա����vmpͣ��
					if (m_tConf.m_tStatus.m_tVMPParam.GetVMPMemberNum() == 0
						&& !m_tConf.m_tStatus.m_tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_DSTREAM)
						&& !m_tConf.m_tStatus.m_tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_VMPCHLPOLL)
						&& !m_tConf.m_tStatus.m_tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
						&& !m_tConf.m_tStatus.m_tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER)) //������и���ͨ������ͣVMP
					{
						//��������ϳ���,WAIT_START�����洦��,��ΪSTART״̬,�����ΪWAIT_STOP,
						if (tPeriEqpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::START)
						{
							SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, TIMESPACE_WAIT_VMPRSP);
							tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
						}
						CServMsg cTempServ;
						SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cTempServ); 
						if (m_tConf.m_tStatus.GetVmpParam().IsVMPBrdst())
						{
							ChangeVidBrdSrc(NULL);
						}
					}
					else // ��ֹ����1����Աʱ,�滻ʧ�ָܻ���,��1.5���ֽ�vmpͣ��
					{
						KillTimer(MCUVC_WAIT_ALLVMPPRESETIN_ACK_TIMER);
					}
					g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
					cServMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);*/

				}
			}
			else if( tPreSetInRsp.m_tSetInReqInfo.GetEvId() == MCUVC_POLLING_CHANGE_TIMER )
			{			
				KillTimer(MCUVC_POLLING_CHANGE_TIMER);
				//ProcPollingChangeTimerMsg(pcMsg);
				tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt(tSrc);
				ProcMcuMcuSpyBWFull( pcMsg,&tPreSetInRsp );
				bAlarm = FALSE;
			}
			//����ʧ��,�ָ���������
			else if( MCS_MCU_CHANGEROLLCALL_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() )
			{
				
				m_tRollCallee = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tOldCallee;
				if (m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP) 
				{
// 					m_tConfInStatus.SetLastVmpRollCallFinished(TRUE);
					//zhouyiliang 20110330�����һ�ε����Ͳ��ɹ�����ʱӦ�ý�������Ҳ��ա�
					tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
					if ( tConfVmpParam.GetVMPStyle() == VMP_STYLE_NONE ||
						 tConfVmpParam.GetVMPStyle() == 0
						) 
					{
						m_tRollCaller.SetNull();
					}
				}
			}
			else if ( MCS_MCU_START_SWITCH_TW_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() 
				&& TW_MEMBERTYPE_TWPOLL == tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_bySrcMtType )
			{
				//20110519_tzy ����������ͨ����ѵ��ʱ��������Ҫ�ȵ����ն���ѯʱ������ŻῪ����һ����ѯ
				
				/*u8 byTwId = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_tTvWall.GetEqpId();
				u8 byChnlIdx =  tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_byDstChlIdx;

				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuPreSetInNack]mt(%d,%d) can't join in (tvw(%d) channel(%d)!\n",
					tSrc.GetMcuId(),tSrc.GetMtId(),byTwId, byChnlIdx );

				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byTwId, byChnlIdx, dwTimerIdx) )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "dwTimerIdx is %d\n", dwTimerIdx);

					KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
					CMessage cMsg;
					memset(&cMsg, 0, sizeof(cMsg));
					cMsg.event = u16(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
					cMsg.content = (u8 *)&dwTimerIdx;
					cMsg.length  = sizeof(u32);
				    ProcTWPollingChangeTimerMsg(&cMsg);
				}*/
				tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt(tSrc);
				ProcMcuMcuSpyBWFull( pcMsg,&tPreSetInRsp );
			}
			// HDU poll: member should be changed otherwise the SpyNum counting will be wrong lator
			else if ( MCS_MCU_START_SWITCH_HDU_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() 
				&& TW_MEMBERTYPE_TWPOLL == tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo.m_bySrcMtType )
			{	
				tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt(tSrc);
				ProcMcuMcuSpyBWFull( pcMsg,&tPreSetInRsp );
				//20110421_tzy Bug00052315����ǽ��ѯ���������ش�����ռ����ʱ�򣬵ڶ����ش������ͷ�
				//�ʲ��ܽ���ͨ�����ն������������CHANGEHDUSWITCH�о��޷�������նˣ��޷������ն���FREE���������´����ͷų�����

				/*u8 byHduId = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo.m_tHdu.GetEqpId();
				u8 byChnlIdx =  tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo.m_byDstChlIdx;
				
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuPreSetInNack]mt(%d,%d) can't join in (hdu(%d) channel(%d)!\n",
					tSrc.GetMcuId(),tSrc.GetMtId(),byHduId, byChnlIdx );
				
				//20110519_tzy ����������ͨ����ѵ��ʱ��������Ҫ�ȵ����ն���ѯʱ������ŻῪ����һ����ѯ
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byHduId, byChnlIdx, dwTimerIdx) )
				{
					KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
					CMessage cMsg;
					memset(&cMsg, 0, sizeof(cMsg));
					cMsg.event = u16(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
					cMsg.content = (u8*)&dwTimerIdx;
					cMsg.length  = sizeof(u32);
				    ProcTWPollingChangeTimerMsg(&cMsg);
				}*/
			}
			else if( VCS_CONF == m_tConf.GetConfSource() &&
				MCS_MCU_SPECSPEAKER_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() &&
				CONF_SPEAKMODE_ANSWERINSTANTLY == m_tConf.GetConfSpeakMode() &&
				!m_tApplySpeakQue.IsQueueNull()
				)
			{				
				TMt tVCMT;
				//TMt tCurMt = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
				if(  m_tApplySpeakQue.GetQueueNextMt( tSrc,tVCMT ) )
				{
					ChgCurVCMT( tVCMT );	
				}
				else
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,"[ProcMcuMcuPreSetInNack] Fail to get Queue NextMt.tSrc(%d.%d)\n",
						tSrc.GetMcuId(),tSrc.GetMtId()
						);
				}
			}

			// vrs��¼���¼��ն�¼���յ�Nack����Ҫ�Ҷ���¼��ʵ��
			if (MCS_MCU_STARTREC_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId())
			{
				u8 byVrsRecId = GetVrsRecMtId(tSrc);
				if (m_ptMtTable->GetRecChlType(byVrsRecId) == TRecChnnlStatus::TYPE_RECORD)
				{
					ReleaseVrsMt(byVrsRecId);
				}
			}
			
			//20101111_tzy VCSԤ��ģʽ�¶�Ҫ�����ն����õ���Ӧ����ǽ��		
			if(  MCS_MCU_START_SWITCH_HDU_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() ||
				MCS_MCU_START_SWITCH_TW_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() 
				)
			{
				tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt( tSrc );
				SetMtInTvWallAndHduInFailPresetinAndInReviewMode( tPreSetInRsp );
			}

			if( bAlarm )
			{
				if( ERR_MCU_CASDBASISNOTENOUGH == cServMsg.GetErrorCode() )
				{
					NotifyMcsAlarmInfo(0, ERR_MCU_CASDBASISNOTENOUGH);
				}
				else if(ERR_MCU_CONFSNDBANDWIDTHISFULL == cServMsg.GetErrorCode())
				{
					NotifyMcsAlarmInfo(0, ERR_MCU_CONFSNDBANDWIDTHISFULL);
				}
				else if(ERR_MCU_SPYMTSTATE_WAITFREE == cServMsg.GetErrorCode())
				{
					NotifyMcsAlarmInfo(0, ERR_MCU_SPYMTSTATE_WAITFREE);
				}
			}
		}
		break;
	}
	
	OnPresetinFailed( tSrc );	
}

/*=============================================================================
    �� �� ���� ProcMcuMcuMultSpyCapNotif
    ��    �ܣ� ���� ������ش�������̽���ں���ĳ���¼�mcuʱ���У�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
	2010/07/19  4.6			½����				  ����¼�֧�ֶ�ش�����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuMultSpyCapNotif( const CMessage * pcMsg )
{
	STATECHECK;	
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );	

	if( cServMsg.GetEventId() == MCU_MCU_MULTSPYCAP_NOTIF )
	{
        u8 bySmcuid = cServMsg.GetSrcMtId();
		u16 wMcuIdx = GetMcuIdxFromMcuId( bySmcuid );
		m_ptConfOtherMcTable->SetMcuSupMultSpy( wMcuIdx );
		u32 dwMaxSpyBW = *(u32 *)(cServMsg.GetMsgBody() + sizeof(TMcu));
		dwMaxSpyBW = ntohl( dwMaxSpyBW );
		m_ptConfOtherMcTable->SetMcuSupMultSpyMaxBW(wMcuIdx, dwMaxSpyBW);
		m_ptConfOtherMcTable->SetMcuSupMultSpyRemainBW(wMcuIdx, (s32)dwMaxSpyBW);
		
		// [chendaiwei 2010/09/11]
		// VCS һ��MCU�����MCU����ʱ�����ݱ�������ϯģ�����ã��ж��¼�����ϯ����Ҫ��������ֵ�����
		// ��ֵ�����¼�����ϯ�ش�����ֵ�����ܻ���ֵ���ʧ�ܣ���Ҫ֪ͨ�û�
		if( MT_TYPE_SMCU == m_ptMtTable->GetMtType(bySmcuid) 
			&& VCS_CONF  == m_tConf.GetConfSource() 
			&& m_tConf.IsSupportMultiSpy() )
		{
			u32 dwSMCUMaxBw = GetVscSMCUMaxBW();
			
			//�����¼�����ϯ��Ҫ��������ֵʧ��
			if(dwSMCUMaxBw == 0)
			{
				return;
			}
			
			//�ش�����С���¼�����ϯ������
			if(dwMaxSpyBW < dwSMCUMaxBw)
			{
				NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_VCS_SMCUSPYBWISNOTENOUGH );
			}	
		}
		SendConfExtInfoToMcs(0, bySmcuid);
	}

	return;
}

/*==============================================================================
������    :  ProcMcuMcuSpyNotify
����      :  �¼������ϼ�Ϊ�����Ļش���Ϣ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-10-15                 pengjie							create
==============================================================================*/
void CMcuVcInst::ProcMcuMcuSpyNotify( const CMessage *pcMsg )
{
	STATECHECK;	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	if( !IsSupportMultiSpy() )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyNotify] Conf Is Not Support MultiSpy.\n" );		
		return;
	}
	u16 wMsgLen = cServMsg.GetMsgBodyLen();
    TSpyResource tSrcSpyInfo = *(TSpyResource *)( cServMsg.GetMsgBody() );
	TMt tOrgSrc =  tSrcSpyInfo.GetSpyMt(); //[nizhijun 2010/12/15] ���ﱣ���£�Ϊ���淵�ظ��ϼ�RTCP��Ϣʱʹ��
	u32 dwEvId = *(u32 *)( cServMsg.GetMsgBody() + sizeof(TSpyResource) );
	dwEvId = ntohl(dwEvId);
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody() + sizeof(TSpyResource) + sizeof(u32) /*+ sizeof(u8)*/ );
	
	// ��Ϣ�����а������,���¼���
	u8* pbyBuf = cServMsg.GetMsgBody() + sizeof(TSpyResource) + sizeof(u32) + sizeof(TMsgHeadMsg);
	u32 dwResW = 0;
	u32 dwResH = 0;
	if (wMsgLen > sizeof(TSpyResource) + sizeof(u32) + sizeof(TMsgHeadMsg) + sizeof(u32))
	{
		dwResW = *(u32 *)pbyBuf;
		pbyBuf += sizeof(u32);
		dwResH = *(u32 *)pbyBuf;
		//�������������תΪ������
		dwResW = ntohl(dwResW);
		dwResH = ntohl(dwResH);

		u8 byRes = GetMcuSupportedRes((u16)dwResW, (u16)dwResH);
		// ���:v4r7mcu�յ��п����Ϣʱ,�ȱ�֤bas��������,����߶�Ӧ�ֱ��ʸ��µ�tSrcSpyInfo������,bas֮���Ӧ
		TSimCapSet tCap = tSrcSpyInfo.GetSimCapset();
		tCap.SetVideoResolution(byRes);
		tSrcSpyInfo.SetSimCapset(tCap);
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "ProcMcuMcuSpyNotify: SpySrc(Mtid): %d, MMcu Give SpyChnnl: %d, SpyMode: %d, SpyRes:%d, dwResWH[%d,%d]\n",
		tSrcSpyInfo.m_tSpy.GetMtId(), tSrcSpyInfo.m_tSpyAddr.GetPort(), tSrcSpyInfo.m_bySpyMode, tSrcSpyInfo.GetSimCapset().GetVideoResolution(), dwResW, dwResH);


	if( !tSrcSpyInfo.m_tSpy.IsMcuIdLocal() )
	{		

		u8 byMcuId = (u8)tSrcSpyInfo.m_tSpy.GetMcuId();
		u16 wSrcMcuIdx = GetMcuIdxFromMcuId( &byMcuId );
		TMt tSrcSpyMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst,tSrcSpyInfo.m_tSpy );//GetMcuIdxMtFromMcuIdMt( tSrcSpyInfo.m_tSpy );
			
		if( IsLocalAndSMcuSupMultSpy(wSrcMcuIdx) )
		{
			CRecvSpy tSpyInfo;
			if( !m_cSMcuSpyMana.GetRecvSpy( tSrcSpyMt, tSpyInfo ) )
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyNotify] Fail to GetSpyChannlInfo.tSrc(%d.%d) ! \n",
					tSrcSpyInfo.m_tSpy.GetMcuId(),tSrcSpyInfo.m_tSpy.GetMtId()
					);
				return;
			}
			m_cSMcuSpyMana.ModifyUseState( tSrcSpyMt,TSpyStatus::NORMAL );
			
			tSpyInfo.m_tSpy = tSrcSpyInfo.m_tSpy;//BuildMultiCascadeMtInfo( tPreSetInReq.m_tSrc );	

			TSpyResource tSpyResource;
			tSpyInfo.GetSpyResource( tSpyResource );

			//zjl20101116��������ش��ն�Ŀ��������
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode(u8(tSrcSpyInfo.GetSpyMt().GetMcuId()));
			
			//zjl20110117 �������������ͨ����ʽ�͵�һ��Ҫ��ڶ����ش��ĸ�ʽ��ͬ����ֱ���ȡС
			if(!tSimCapSet.IsNull() &&
				(tSrcSpyInfo.GetSimCapset().GetVideoMediaType() == tSimCapSet.GetVideoMediaType()))
			{
				u8 byMinRes = GetMinResAcdWHProduct(tSimCapSet.GetVideoResolution(),
													tSrcSpyInfo.GetSimCapset().GetVideoResolution());
				
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuSpyNotify] GetMinRes %d between DstRes:%d and SrcRes:%d!\n",
											byMinRes, 
											tSrcSpyInfo.GetSimCapset().GetVideoResolution(),
											tSimCapSet.GetVideoResolution());
				
				if (VIDEO_FORMAT_INVALID != byMinRes)
				{
					tSimCapSet.SetVideoResolution(byMinRes);
				}
				
				// [2013/05/14 chenbing] ֡��ȡС
				u8 byMinFps = 0;
				if ( MEDIA_TYPE_H264 == tSimCapSet.GetVideoMediaType() )
				{
					byMinFps = tSrcSpyInfo.GetSimCapset().GetUserDefFrameRate() >= tSimCapSet.GetUserDefFrameRate() 
						? tSimCapSet.GetUserDefFrameRate()
						: tSrcSpyInfo.GetSimCapset().GetUserDefFrameRate();
				
					tSimCapSet.SetUserDefFrameRate(byMinFps);
				}
				else
				{
					byMinFps = tSrcSpyInfo.GetSimCapset().GetVideoFrameRate() >= tSimCapSet.GetVideoFrameRate() 
						? tSimCapSet.GetVideoFrameRate()
						: tSrcSpyInfo.GetSimCapset().GetVideoFrameRate();
					
					tSimCapSet.SetUserDefFrameRate(byMinFps);
				}
			}

			//zjl20101116 �����ǰ�ն��ѻش���������Ҫ���ѻش�Ŀ��������ȡС	
			if (!GetMinSpyDstCapSet(tSrcSpyMt, tSimCapSet))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyNotify] tSimCapSet is null!\n" );
				return;
			}

			//zjl20101116����(����)�¼��ش�Ŀ��������
			m_cSMcuSpyMana.SaveSpySimCap(tSrcSpyMt, tSimCapSet);

			u16 wTmpResW = 0;
			u16 wTmpResH = 0;
			// �Էֱ��ʽ��е���,�����¼��Ǿ�mcu,�跢��ͨ�ֱ���,��ʵ�ֱ��ʿ�������׷��
			u8 byCommonRes = tSimCapSet.GetVideoResolution();
			GetWHByRes(byCommonRes, wTmpResW, wTmpResH);
			// ���ݷֱ���,��ö�Ӧͨ�÷ֱ���,�¾�MCU����ʶ��ķֱ���
			dwResW = wTmpResW;
			dwResH = wTmpResH;
			byCommonRes = GetNormalRes(wTmpResW, wTmpResH);
			tSimCapSet.SetVideoResolution(byCommonRes);
			tSpyResource.SetSimCapset(tSimCapSet);

			u32 dwNetEvId = htonl(dwEvId);

			cServMsg.SetEventId(MCU_MCU_SPYCHNNL_NOTIF);
			cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
			cServMsg.CatMsgBody( (u8 *)&tSpyResource, sizeof(TSpyResource) );		
			cServMsg.CatMsgBody( (u8 *)&dwNetEvId, sizeof(u32) );

			if( dwResW !=0 && dwResH != 0)
			{
				dwResW = htonl(dwResW);
				dwResH = htonl(dwResH);
				cServMsg.CatMsgBody( (u8 *)&dwResW, sizeof(u32) );
				cServMsg.CatMsgBody( (u8 *)&dwResH, sizeof(u32) );
			}

			SendMsgToMt( (u8)tSrcSpyInfo.m_tSpy.GetMcuId(), MCU_MCU_SPYCHNNL_NOTIF, cServMsg );	
			
		}
		else
		{
			//tSrc = GetMcuIdxMtFromMcuIdMt( tSrc );
			OnMMcuSetIn( tSrcSpyMt,0,SWITCH_MODE_BROADCAST );
		}
		tSrcSpyInfo.m_tSpy = tSrcSpyMt;
	}
	else
	{
		tSrcSpyInfo.m_tSpy = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst,tSrcSpyInfo.m_tSpy );
	}

	BOOL32 bIsAlreadySpyVideo = FALSE;
	if( MODE_BOTH == m_cLocalSpyMana.GetSpyMode(tSrcSpyInfo.m_tSpy) ||
		MODE_VIDEO == m_cLocalSpyMana.GetSpyMode(tSrcSpyInfo.m_tSpy) )
	{
		bIsAlreadySpyVideo = TRUE;
	}
	BOOL32 bIsAlreadySpyAudio = FALSE;
	if( MODE_BOTH == m_cLocalSpyMana.GetSpyMode(tSrcSpyInfo.m_tSpy) ||
		MODE_AUDIO == m_cLocalSpyMana.GetSpyMode(tSrcSpyInfo.m_tSpy) )
	{
		bIsAlreadySpyAudio = TRUE;
	}

	// �¼�mcu��¼����Ӧ����Ϣ
	CSendSpy cSendSpy;
	cSendSpy.m_tSpy = tSrcSpyInfo.m_tSpy;
	cSendSpy.m_tSpyAddr = tSrcSpyInfo.m_tSpyAddr;
	cSendSpy.m_bySpyMode = tSrcSpyInfo.m_bySpyMode;
	//cSendSpy.m_dwTotalBW = tSrcSpyInfo.m_dwTotalBW;

//	BOOL32 bIsRepeatedSpy = m_cLocalSpyMana.IsRepeatedSpy(cSendSpy);
	//zhouyiliang 20110222 �����ε�spynotify�ϴ��Ѿ����ˣ���������������ϴ�һ����ֱ��return
// 	if ( bIsRepeatedSpy ) 
// 	{
// 		CSendSpy cRepeatSendSpy;
// 		if ( m_cLocalSpyMana.GetSpyChannlInfo(tSrcSpyInfo.m_tSpy,cRepeatSendSpy) &&  
// 			tSrcSpyInfo.GetSimCapset() == cRepeatSendSpy.GetSimCapset() )
// 		{
// 			// 2011-10-10 add by pgf:����LOG����ʾ����Spy Notify�ظ��������ٽ�������
// 			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "Local Mana cSendSpy[mt<%d %d>] repeat, so dont set switch for it!\n",
// 				cSendSpy.m_tSpy.GetMcuIdx(), cSendSpy.m_tSpy.GetMtId());
// 			return;
// 		}
// 	}

	if( !m_cLocalSpyMana.AddSpyChannlInfo( cSendSpy ) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, " SetSpyChannlInfo False When ProcMcuMcuSpyNotify!\n" );
		return;
	}

	//���汾���ش����ϼ���������
	m_cLocalSpyMana.SaveSpySimCap(tSrcSpyInfo.m_tSpy, tSrcSpyInfo.GetSimCapset());

	u8 byMode = tSrcSpyInfo.m_bySpyMode;
	TMt tDstMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	TMt tSrcMt = tSrcSpyInfo.m_tSpy;

	 u16 wSrcChnnl = 0;
	 if( !tSrcMt.IsLocal() )
	 {
		CRecvSpy cRecvSpy;
		if( m_cSMcuSpyMana.GetRecvSpy( tSrcMt,cRecvSpy ) )
		{
			wSrcChnnl = cRecvSpy.m_tSpyAddr.GetPort();
		}		
	 }
	 if ((byMode == MODE_BOTH && bIsAlreadySpyVideo && bIsAlreadySpyAudio) ||
		 (byMode == MODE_AUDIO && bIsAlreadySpyAudio) || 
		 (byMode == MODE_VIDEO && bIsAlreadySpyVideo))
	 {
		 ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,"[ProcMcuMcuSpyNotify]byMode(%d) bIsAlreadySpyVideo(%d) bIsAlreadySpyAudio(%d),So Return!\n",
					byMode,bIsAlreadySpyVideo,bIsAlreadySpyAudio);
		 return;
	 }
	 if (byMode == MODE_BOTH)
	 {
		 if (bIsAlreadySpyVideo)
		 {
			 ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,"[ProcMcuMcuSpyNotify]bIsAlreadySpyVideo(TRUE) So Remove MODE_VIDEO!\n");
			 byMode = MODE_AUDIO;
		 }
		 if (bIsAlreadySpyAudio)
		 {
			 ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,"[ProcMcuMcuSpyNotify]bIsAlreadySpyAudio(TRUE) So Remove MODE_AUDIO!\n");
			 byMode = MODE_VIDEO;
		 }
	 }

	 u16 wErrorCode = 0;
	 if ( StartSwitchToMcuByMultiSpy( tSrcMt, wSrcChnnl, tDstMt.GetMtId(), tSrcSpyInfo.GetSimCapset(), wErrorCode,
		 byMode, SWITCH_MODE_SELECT, FALSE ) )
	 {
		 //[nizhijun 2010/12/10] ������ش�RTCP��Ϣ�ϸ�
		 TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
		 if (tConfAttrb.IsResendLosePack())
		 {
			 ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuSpyNotify] start multiRtcpInfo orgnize\n!");
			 OnMMcuRtcpInfoNotif(tOrgSrc, tSrcSpyInfo.GetSimCapset(), tHeadMsg);
		 }
	 }
	 else
	 {
		 //[2011/09/09/zhangli]���StartSwitchToMcuByMultiSpyʧ�ܣ�����ͼ��һ�ηֱ��ʣ����ʧ�ܣ������ֱ���ʧ�ܵ���Ϣ���ϼ����Ƴ�vmp��Ա���ͷŻش��Ȳ���
		 if (byMode == MODE_BOTH || byMode == MODE_VIDEO)
		 {
			 if (IsNeedSpyAdpt(tSrcMt, tSrcSpyInfo.GetSimCapset(), MODE_VIDEO)
				 && !AdjustSpyStreamFromAdp(tOrgSrc, tSrcSpyInfo.GetSimCapset().GetVideoResolution(), tHeadMsg))
			 {
				 CServMsg cMsg;
				 
				 TMsgHeadMsg tHeadMsgRsp;
				 tHeadMsgRsp.m_tMsgDst = tHeadMsg.m_tMsgSrc;
				 tHeadMsgRsp.m_tMsgSrc = tHeadMsg.m_tMsgDst;
				 
				 if (tSrcMt.IsLocal())
				 {
					 tSrcMt.SetMcuId(LOCAL_MCUID);
					 //tSrcMt.SetMcuIdx(LOCAL_MCUID);
				 }
				 else
				 {
					tSrcMt = tOrgSrc;
				 }
				 cMsg.SetEventId(MCU_MCU_SWITCHTOMCUFAIL_NOTIF);
				 cMsg.SetSrcMtId(cServMsg.GetSrcMtId());
				 cMsg.SetErrorCode(wErrorCode);
				 cMsg.SetMsgBody((u8*)&tHeadMsgRsp, sizeof(TMsgHeadMsg));
				 cMsg.CatMsgBody((u8 *)&tSrcMt, sizeof(TMt));
				 SendReplyBack(cMsg, MCU_MCU_SWITCHTOMCUFAIL_NOTIF);

				 m_cLocalSpyMana.FreeSpyChannlInfo(cSendSpy.m_tSpy,byMode);
				
				 return;
				 
				 // 2011-10-10 add by pgf:�����������ʧ�ܣ���ô�Ѹ��ն˵Ļش���Ϣ���
// 				 ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyNotify]set switch failed:mt<%d %d> mode:%d, so remove it from LocalMana\n",
// 					 tSrcMt.GetMcuIdx(), tSrcMt.GetMtId(), tSrcSpyInfo.m_bySpyMode);
// 				 if(!m_cLocalSpyMana.FreeSpyChannlInfo(tSrcMt, tSrcSpyInfo.m_bySpyMode))
// 				 {
// 					 ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyNotify] FreeSpyChannlInfo failed tSrc<%d %d>\n",
// 						 tSrcMt.GetMcuIdx(), tSrcMt.GetMtId() );
// 				 }
			 }
		 }
	 }

	 if( !bIsAlreadySpyVideo && !m_tCascadeMMCU.IsNull() && m_ptMtTable->GetMtTransE1(m_tCascadeMMCU.GetMtId()) )
	 {
		ProcMMcuBandwidthNotify( m_tCascadeMMCU,GetRealSndSpyBandWidth() );
	 }
	 
	return;
}

/*==============================================================================
������    :  ProcMcuMcuSwitchToMcuFailNotif
����      :  �¼����������ϼ�mcuʧ�ܵ�ͨ��
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2011-09-08   4.6           zhangli							create
==============================================================================*/
void CMcuVcInst::ProcMcuMcuSwitchToMcuFailNotif( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
	
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	if(tHeadMsg.m_tMsgDst.m_byCasLevel > 0 && !m_tCascadeMMCU.IsNull())
	{
		SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_SWITCHTOMCUFAIL_NOTIF, cServMsg);
		return;
	}
	u16 wErrorCode = cServMsg.GetErrorCode();
	TMt tMt = *(TMt *)(cServMsg.GetMsgBody() +sizeof(TMsgHeadMsg));	//�����õ�MT
	
	tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tMt );
	
	ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuSwitchToMcuFailNotif] Mt(%u,%u) switch to mmcu failed!\n",
		tMt.GetMcuId(), tMt.GetMtId());

	if( VCS_CONF == m_tConf.GetConfSource() && m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_MANUAL_MODE )
	{
		FindConfHduHduChnnlAndStop( &tMt,TW_MEMBERTYPE_NULL,TRUE );	
		FindConfTvWallChnnlAndStop( &tMt,TW_MEMBERTYPE_NULL,TRUE );	
	}
	
	//���������Դ���㣬������ʾ
	if(wErrorCode > 0)
	{
		NotifyMcsAlarmInfo(0, wErrorCode);
	}
	
	//���´����ǰѽ�����ʧ�ܵ�����ն˴ӻ���ϳ������Ƴ����ͷŻش�����
	//Ŀǰֻ��Ҫ������ʾ�����������ʱע��
	// 	if (tMt.IsNull())
	// 	{
	// 		return;
	// 	}
	// 	
	// 	//�Ƴ���ʧ�ܵ�VMP��Ա
	// 	TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
	// 	TVMPMember tVmpMember;
	// 	
	// 	for (u8 byLoop = 0; byLoop < tVmpParam.GetVMPMemberNum(); ++byLoop)
	// 	{
	// 		tVmpMember = *(tVmpParam.GetVmpMember(byLoop));
	// 		if (tVmpMember.IsNull())
	// 		{
	// 			continue;
	// 		}
	// 		if (tVmpMember == tMt)
	// 		{
	// 			ClearOneVmpMember(byLoop, tVmpParam);
	// 			FreeRecvSpy(tMt, MODE_VIDEO);
	// 			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "McuMcuadjResAck, begin to SetVmpChnnl!\n");
	// 		}
	// 	}
}

/*==============================================================================
������    :  ProcMcuMcuRejectSpyNotify
����      :  �ϼ������������¼�֪ͨ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-10-15                 pengjie							create
==============================================================================*/
void CMcuVcInst::ProcMcuMcuRejectSpyNotify( const CMessage *pcMsg )
{
	STATECHECK;	
	//CServMsg cServMsg(pcMsg->content, pcMsg->length);	

	//lukunpeng 2010/06/10 ����Ҫ���������ϴ����ж��ٴ�����ã���ȫ�����ϼ�����
	/*
	TMt tMt      = *(TMt *)( cServMsg.GetMsgBody() );
	u32 dwSpyBW  = *(u32 *)( cServMsg.GetMsgBody() + sizeof(TMt) );
	u32 RemainBW = m_cLocalSpyMana.GetConfRemainSpyBW() + dwSpyBW;
	m_cLocalSpyMana.SetConfRemainSpyBW( RemainBW );
	u8 byOldMode = m_cLocalSpyMana.GetOldMode( tMt );
	m_cLocalSpyMana.SetMode( tMt, byOldMode );
	*/

	ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY,  "The Mmcu come to message RejectSpyNotify! \n" );
	
	//return;
}

BOOL32 CMcuVcInst::StopAllLocalSpySwitch()
{
	u16 wSpyNum = MAXNUM_CONF_SPY/*m_cLocalSpyMana.GetSendSpyNum()*/;

	CSendSpy* pcSendSpy = NULL;

	for (u16 wIndex = 0; wIndex < wSpyNum; ++wIndex)
	{
		pcSendSpy = m_cLocalSpyMana.GetSendSpy(wIndex);

		if (NULL == pcSendSpy)
		{
			continue;
		}

		if (pcSendSpy->m_tSpy.IsNull())
		{
			continue;
		}

		/* [2011/11/14/zhangli]������¼��նˣ�ǿ��FreeRecvSpy��ȱ�ݣ��⽫��������������ն�ͬʱ�ڲ���ڶ���ҵ��Ҳ��free��
		�õ������ն˽���һ���������ϵ�ҵ��������һ��mcu���ڶ����������ﴦ��m_cLocalSpyMana�Ļش���Ϣ������ǵ������նˣ�
		StopSpyMtSpySwitch���FreeRecvSpyһ�Σ�����������ն˲����һ��N��ҵ�񣬽���N-1������*/
		if (!pcSendSpy->m_tSpy.IsLocal())
		{
			FreeRecvSpy(pcSendSpy->m_tSpy, pcSendSpy->m_bySpyMode, TRUE);
		}

		if (!StopSpyMtSpySwitch(pcSendSpy->m_tSpy, pcSendSpy->m_bySpyMode))
		{
			return FALSE;
		}
	}

	m_cLocalSpyMana.Clear();

	return TRUE;
}
/*==============================================================================
������    :  StopSpyMtSpySwitch
����      :  �ͷ�һ·������Ƶ�ش�ͨ��,�����Ҫ���ͷŵ���mcu���ͣ���ô�ͷŵ���mcu�����ж�ش�ͨ��(�¼�mcuʹ��)
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-06-17                 lukunpeng						modify
==============================================================================*/
BOOL32 CMcuVcInst::StopSpyMtSpySwitch( TMt tSrc,u8 bySpyMode, u8 bySpyNoUse/*,BOOL32 bIsNotifyMMcu */)
{
	CSendSpy cSendSpy;
	CRecvSpy cRecvSpy;
	if( !m_cLocalSpyMana.GetSpyChannlInfo( tSrc, cSendSpy ) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StopSpyMtSpySwitch] Stop tSrc(%d.%d)  Multispy Failed.Mt is not in spy member.\n",
			tSrc.GetMcuId(),tSrc.GetMtId()
			);
		//�ͷ�Ԥռ����Դ
		if( !tSrc.IsLocal() )
		{
			u32 dwCanReleaseBW = 0;
			s16 swCanRelIndex = -1;
			u32 dwMaxSpyBW = 0;
			s32 nRemainSpyBW = 0;
			u8 byRelMode = 0;
			
			if( !m_cSMcuSpyMana.GetRecvSpy(tSrc,cRecvSpy) )
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StopSpyMtSpySwitch] Fail to find spy mt tSrc(%d.%d) bySpyMode.%d!\n",
					tSrc.GetMcuId(),tSrc.GetMtId(),bySpyMode 
					);
				return FALSE;
			}
			if( TSpyStatus::WAIT_USE != cRecvSpy.m_byUseState || bySpyMode != cRecvSpy.m_byPreAddMode )
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StopSpyMtSpySwitch] spy mt tSrc(%d.%d) bySpyMode.%d is not pre add or preadd mode(%d) is error!\n",
					tSrc.GetMcuId(),tSrc.GetMtId(),bySpyMode,cRecvSpy.m_byPreAddMode
					);
				return FALSE;
			}
			m_cSMcuSpyMana.ReleasePreAddRes( tSrc,dwCanReleaseBW, swCanRelIndex );
			u16 wSrcMcuIdx = tSrc.GetMcuId();
			if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(tSrc.GetMcuId()) )
			{
				u8 byMcuId = GetFstMcuIdFromMcuIdx( tSrc.GetMcuId() );
				wSrcMcuIdx = GetMcuIdxFromMcuId( &byMcuId );
			}		

			m_ptConfOtherMcTable->GetMultiSpyBW(wSrcMcuIdx, dwMaxSpyBW, nRemainSpyBW);
			nRemainSpyBW += (s32)dwCanReleaseBW;				
			m_ptConfOtherMcTable->SetMcuSupMultSpyRemainBW(wSrcMcuIdx, nRemainSpyBW );		

			CMultiSpyMgr *pcMultiSpyMgr = g_cMcuVcApp.GetCMultiSpyMgr();				
			if( NULL == pcMultiSpyMgr )
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StopSpyMtSpySwitch]GetCMultiSpyMgr False! \n" );
				return FALSE;
			}
			if( -1 != swCanRelIndex )
			{
				pcMultiSpyMgr->ReleaseSpyChnnl( swCanRelIndex );
			}
		}
		
		return FALSE;
	}
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  
		"[StopSpyMtSpySwitch] begin to call FreeSpyChannlInfo (Mt(%d.%d), SpyMode.%d) SpyPort:%d, SpyNoUse:%d\n",
		tSrc.GetMcuId(),tSrc.GetMtId(), bySpyMode,cSendSpy.m_tSpyAddr.GetPort(), bySpyNoUse
		);

	if( !tSrc.IsLocal() )
	{
		FreeRecvSpy( tSrc,bySpyMode );
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, 
			"[StopSpyMtSpySwitch] Mt(%d.%d) adjust free mode(%d)\n",
			tSrc.GetMcuId(),
			tSrc.GetMtId(),
			bySpyMode);
	}

	if( ( MODE_VIDEO == bySpyMode || MODE_BOTH == bySpyMode ) 
		&& ( (bySpyNoUse & MODE_VIDEO) != 0 )
		)
	{
		TSimCapSet tSrcCap;
		if (!tSrc.IsLocal())
		{
			tSrcCap = m_ptMtTable->GetSrcSCS(GetFstMcuIdFromMcuIdx(tSrc.GetMcuId()));
		}
		else
		{
			tSrcCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
		}

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY,  "[StopSpyMtSpySwitch] SrcCap<Media:%d, Res:%d, BR:%d, Fps:%d>, DstCap<Media:%d, Res:%d, BR:%d, Fps:%d>!\n",
												tSrcCap.GetVideoMediaType(),
												tSrcCap.GetVideoResolution(),
												tSrcCap.GetVideoMaxBitRate(),
												MEDIA_TYPE_H264 == tSrcCap.GetVideoMediaType()?
												tSrcCap.GetUserDefFrameRate():tSrcCap.GetVideoFrameRate(),
												cSendSpy.GetSimCapset().GetVideoMediaType(),
												cSendSpy.GetSimCapset().GetVideoResolution(),
												cSendSpy.GetSimCapset().GetVideoMaxBitRate(),
												MEDIA_TYPE_H264 == cSendSpy.GetSimCapset().GetVideoMediaType()?
												cSendSpy.GetSimCapset().GetUserDefFrameRate(): cSendSpy.GetSimCapset().GetVideoFrameRate());
		
		BOOL32 bAdp = FALSE;
		if( ( (tSrc.IsLocal() && 
				m_ptMtTable->IsLogicChnnlOpen( tSrc.GetMtId(), LOGCHL_VIDEO, FALSE )
				) || 
			  (!tSrc.IsLocal() && 
				m_ptMtTable->IsLogicChnnlOpen( GetFstMcuIdFromMcuIdx(tSrc.GetMcuId()), LOGCHL_VIDEO, FALSE )
				  )	
			 )  &&
			 !cSendSpy.GetSimCapset().IsNull() &&
			 IsNeedSpyAdpt(tSrc, cSendSpy.GetSimCapset(), MODE_VIDEO)
				)
		{			
			BOOL32 bIsStopAdpOk = StopSpyAdapt(tSrc, cSendSpy.GetSimCapset(), MODE_VIDEO);
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[StopSpyMtSpySwitch] StopSpyAdapt bIsStopAdpOk:%d!\n",bIsStopAdpOk);
			bAdp = TRUE;
		}
		
		//zjl 20110510 Mp: StopSwitchToSubMt �ӿ������滻 
		//g_cMpManager.StopSwitchToSubMt( m_tCascadeMMCU, MODE_VIDEO, FALSE, cSendSpy.m_tSpyAddr.GetPort() );	
		g_cMpManager.StopSwitchToSubMt(m_byConfIdx, 1, &m_tCascadeMMCU, MODE_VIDEO, cSendSpy.m_tSpyAddr.GetPort());
	
		if( !bAdp )
		{
			if (tSrc.IsLocal())
			{
				SendChgMtVidFormat( tSrc.GetMtId(), MODE_VIDEO, tSrcCap.GetVideoResolution(), TRUE );

				if (IsNeedAdjustSpyFps(tSrc, cSendSpy.GetSimCapset()))
				{
					if (cSendSpy.GetSimCapset().GetVideoMediaType() == MEDIA_TYPE_H264)
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[StopSpyMtSpySwitch] SendChangeMtFps %d.fps OK!\n", tSrcCap.GetUserDefFrameRate());
						SendChangeMtFps(tSrc.GetMtId(), LOGCHL_VIDEO, tSrcCap.GetUserDefFrameRate());
					}		
				}
			}
		}
	}
	
	if( ( MODE_AUDIO == bySpyMode || MODE_BOTH == bySpyMode ) 
		&& ( (bySpyNoUse & MODE_AUDIO) != 0 )
		)
	{
		TSimCapSet tSrcCap;

		/*if (!tSrc.IsLocal())
		{
			tSrcCap = m_ptMtTable->GetSrcSCS(GetFstMcuIdFromMcuIdx(tSrc.GetMcuId()));
		}
		else
		{
			tSrcCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
		}
		TMt tLocalSrc = GetLocalMtFromOtherMcuMt(tSrc);



		TLogicalChannel tDstAudLgc,tSrcAudLgc;	


		if( m_ptMtTable->GetMtLogicChnnl(tLocalSrc.GetMtId(), MODE_AUDIO, &tSrcAudLgc, FALSE) &&
				m_ptMtTable->GetMtLogicChnnl(m_tCascadeMMCU.GetMtId(), MODE_AUDIO, &tDstAudLgc, TRUE) )
		{
			if( ( cSendSpy.GetSimCapset().GetAudioMediaType() != tSrcCap.GetAudioMediaType() ||
				tSrcAudLgc.GetAudioTrackNum() != tDstAudLgc.GetAudioTrackNum() ) &&
				m_ptMtTable->IsLogicChnnlOpen( tLocalSrc.GetMtId(), LOGCHL_AUDIO, FALSE ) &&
				!cSendSpy.GetSimCapset().IsNull() )
			{
				StopSpyAdapt(tSrc, cSendSpy.GetSimCapset(), MODE_AUDIO);
			}
		}*/
		
		
		//TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());

		if( IsNeedSpyAdpt( tSrc,tSrcCap,MODE_AUDIO ) )
		{
			StopSpyAdapt(tSrc, cSendSpy.GetSimCapset(), MODE_AUDIO);
		}
		
		
		//zjl 20110510 Mp: StopSwitchToSubMt �ӿ������滻 
		//g_cMpManager.StopSwitchToSubMt( m_tCascadeMMCU, MODE_AUDIO, FALSE, cSendSpy.m_tSpyAddr.GetPort() );
		g_cMpManager.StopSwitchToSubMt(m_byConfIdx, 1, &m_tCascadeMMCU, MODE_AUDIO, cSendSpy.m_tSpyAddr.GetPort());
	}
	
	if( (bySpyNoUse & bySpyMode) == bySpyMode)
	{
		if(!m_cLocalSpyMana.FreeSpyChannlInfo(tSrc, bySpyMode))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StopSpyMtSpySwitch] FreeSpyChannlInfo False When ProcMcuMcuBanishSpyCmd! tSrc(Mtid): %d\n",
				tSrc.GetMtId() );
		}
	}
	else
	{
		if( MODE_NONE != bySpyNoUse && !m_cLocalSpyMana.FreeSpyChannlInfo(tSrc, bySpyNoUse))
		{
			LogPrint(LOG_LVL_WARNING,MID_MCU_SPY,"[StopSpyMtSpySwitch] FreeSpyChannlInfo False When ProcMcuMcuBanishSpyCmd! tSrc(Mtid): %d bySpyNoUse.%d\n",tSrc.GetMtId(),bySpyNoUse );
		}
	}
	
	// [1/28/2011 xliang] shouldn't change res by force
	/*
	//����������Դ�ֱ��ʣ��Ƿ���ɹ��ӿ����ж�
    TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
	SendChgMtVidFormat( tSrc.GetMtId(), MODE_VIDEO, tSrcCap.GetVideoResolution(), TRUE );

	//�ָ�֡��
	if (IsNeedAdjustSpyFps(tSrc, cSendSpy.GetSimCapset()))
	{
		if (cSendSpy.GetSimCapset().GetVideoMediaType() == MEDIA_TYPE_H264)
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[StopSpyMtSpySwitch] SendChangeMtFps %d.fps OK!\n", tSrcCap.GetUserDefFrameRate());
			SendChangeMtFps(tSrc.GetMtId(), LOGCHL_VIDEO, tSrcCap.GetUserDefFrameRate());
		}		
	}
	*/

	if( !m_tCascadeMMCU.IsNull() && m_ptMtTable->GetMtTransE1(m_tCascadeMMCU.GetMtId()) )
	{
		ProcMMcuBandwidthNotify( m_tCascadeMMCU,GetRealSndSpyBandWidth() );
	}

	return TRUE;
}

/*==============================================================================
������    :  ProcMcuMcuBanishSpyCmd
����      :  �ͷ�һ·������Ƶ�ش�ͨ��(�¼�mcuʹ��)
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-10-15                 pengjie							create
==============================================================================*/
void CMcuVcInst::ProcMcuMcuBanishSpyCmd( const CMessage *pcMsg )
{
	STATECHECK;	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	TMt tSrc =  *(TMt *)( cServMsg.GetMsgBody() );
    u8 bySpyMode = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt));
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) );
	u8 bySpyNoUse = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(TMsgHeadMsg));
	
	tSrc = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst,tSrc );
	
	StopSpyMtSpySwitch( tSrc,bySpyMode, bySpyNoUse );

	return;
}

/*==============================================================================
������    :  ProMcuMcuMtExtInfoNotif
����      :  ��������ش�RTCP��Ϣ��֪
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-12-15                 ��־��							create
==============================================================================*/
void CMcuVcInst::ProcMcuMcuMtExtInfoNotif(const CMessage *pcMsg)
{
	STATECHECK;
	
	TConfAttrb tConfattrb = m_tConf.GetConfAttrb();
	if ( !tConfattrb.IsResendLosePack() )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuMtExtInfoNotif] Conf doesn't support PRS:%d!\n");
		return;
	}

	TMt tSrc;
	CServMsg cServMsg(pcMsg->content,pcMsg->length);
	
	TMultiRtcpInfo tMultiRtcpInfo = *(TMultiRtcpInfo *)(cServMsg.GetMsgBody());
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TMultiRtcpInfo));
	tSrc = tMultiRtcpInfo.m_tSrcMt ;
	u16 wSrcMcuIdx = GetMcuIdxFromMcuId( (u8)tSrc.GetMcuId() );	
	tSrc.SetMcuIdx( wSrcMcuIdx );	
	tSrc = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc, tMultiRtcpInfo.m_tSrcMt );

	//����ش��ն˲��ڻش��б��У���˵���Ƿ��նˣ�ֱ��return
	s16 swIndex = m_cSMcuSpyMana.FindSpyMt(tSrc);	
	if (-1 != swIndex)
	{
		CRecvSpy tSrcSpyInfo;
		BOOL32 bHasBuildedVidRtcp = FALSE;
		BOOL32 bHasBuildedAudRtcp = FALSE;
		m_cSMcuSpyMana.GetRecvSpy(swIndex, tSrcSpyInfo);
		u32 dwRcvMpIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( GetLocalMtFromOtherMcuMt(tSrc).GetMtId()));
		
		switch (tMultiRtcpInfo.m_bySpyMode)
		{
		case MODE_BOTH:
			{
				TTransportAddr tSpyRtcpAddr = tSrcSpyInfo.GetVidSpyBackRtcpAddr();
				if (!(tSpyRtcpAddr == tMultiRtcpInfo.m_tVidRtcpAddr))
				{	
					if (tMultiRtcpInfo.m_tVidRtcpAddr.GetIpAddr()!=0 && tMultiRtcpInfo.m_tVidRtcpAddr.GetPort()!=0)
					{
						bHasBuildedVidRtcp = TRUE;
						g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwRcvMpIp, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort() + 1, 
							tMultiRtcpInfo.m_tVidRtcpAddr.GetIpAddr(),tMultiRtcpInfo.m_tVidRtcpAddr.GetPort(), 
								0, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort());
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "Build MultiSpy Vid RTCP:%s@%d",StrOfIP(dwRcvMpIp),tSrcSpyInfo.m_tSpyAddr.GetPort() + 1);
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "------>%s@%d\n",StrOfIP(tMultiRtcpInfo.m_tVidRtcpAddr.GetIpAddr()),tMultiRtcpInfo.m_tVidRtcpAddr.GetPort());
					}
				}
				
				TTransportAddr tSpyRtcpAudAddr = tSrcSpyInfo.GetAudSpyBackRtcpAddr();
				if (!(tSpyRtcpAudAddr == tMultiRtcpInfo.m_tAudRtcpAddr))
				{	
					if (tMultiRtcpInfo.m_tAudRtcpAddr.GetIpAddr()!=0 && tMultiRtcpInfo.m_tAudRtcpAddr.GetPort()!=0)
					{
						bHasBuildedAudRtcp = TRUE;
						g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwRcvMpIp, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort() + 3, 
							tMultiRtcpInfo.m_tAudRtcpAddr.GetIpAddr(),tMultiRtcpInfo.m_tAudRtcpAddr.GetPort(), 
								0, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort());
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "Build MultiSpy Vid RTCP:%s@%d",StrOfIP(dwRcvMpIp),tSrcSpyInfo.m_tSpyAddr.GetPort() + 3);
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "------>%s@%d\n",StrOfIP(tMultiRtcpInfo.m_tAudRtcpAddr.GetIpAddr()),tMultiRtcpInfo.m_tAudRtcpAddr.GetPort());
					}
				}
			}
			break;
		case MODE_VIDEO:
			{
				TTransportAddr tSpyRtcpVidAddr = tSrcSpyInfo.GetVidSpyBackRtcpAddr();
				if ( tSpyRtcpVidAddr == tMultiRtcpInfo.m_tVidRtcpAddr )
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[ProcMcuMcuMtExtInfoNotif] MODE_VIDEO's RTCP hasbeen build to %s@%d!\n",
						StrOfIP(tSrcSpyInfo.GetVidSpyBackRtcpAddr().GetIpAddr()),tSrcSpyInfo.GetVidSpyBackRtcpAddr().GetPort());
					return;
				}
				else
				{
					if (tMultiRtcpInfo.m_tVidRtcpAddr.GetIpAddr()!=0 && tMultiRtcpInfo.m_tVidRtcpAddr.GetPort()!=0)
					{
						bHasBuildedVidRtcp = TRUE;
						g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwRcvMpIp, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort() + 1, 
							tMultiRtcpInfo.m_tVidRtcpAddr.GetIpAddr(),tMultiRtcpInfo.m_tVidRtcpAddr.GetPort(), 
								0, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort());
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "Build MultiSpy Vid RTCP:%s@%d",StrOfIP(dwRcvMpIp),tSrcSpyInfo.m_tSpyAddr.GetPort() + 1);
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "------>%s@%d\n",StrOfIP(tMultiRtcpInfo.m_tVidRtcpAddr.GetIpAddr()),tMultiRtcpInfo.m_tVidRtcpAddr.GetPort());
					}
				}
			}
			break;
		case MODE_AUDIO:
			{
				TTransportAddr tSpyRtcpAudAddr = tSrcSpyInfo.GetAudSpyBackRtcpAddr();
				if (tSpyRtcpAudAddr == tMultiRtcpInfo.m_tAudRtcpAddr )
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[ProcMcuMcuMtExtInfoNotif] MODE_AUDIO's RTCP hasbeen build to %s@%d!\n",
						StrOfIP(tSrcSpyInfo.GetAudSpyBackRtcpAddr().GetIpAddr()),tSrcSpyInfo.GetAudSpyBackRtcpAddr().GetPort());
					return;
				}
				else
				{
					if (tMultiRtcpInfo.m_tAudRtcpAddr.GetIpAddr()!=0 && tMultiRtcpInfo.m_tAudRtcpAddr.GetPort()!=0)
					{
						bHasBuildedAudRtcp = TRUE;
						g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwRcvMpIp, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort() + 3, 
							tMultiRtcpInfo.m_tAudRtcpAddr.GetIpAddr(),tMultiRtcpInfo.m_tAudRtcpAddr.GetPort(), 
							0, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort());
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "Build MultiSpy Vid RTCP:%s@%d",StrOfIP(dwRcvMpIp),tSrcSpyInfo.m_tSpyAddr.GetPort() + 3);
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "------>%s@%d\n",StrOfIP(tMultiRtcpInfo.m_tAudRtcpAddr.GetIpAddr()),tMultiRtcpInfo.m_tAudRtcpAddr.GetPort());
					}
				}
			}
			break;
		default:
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuMtExtInfoNotif] illegal spymode:%d!\n",tMultiRtcpInfo.m_bySpyMode);
			break;
		}

		if ( TRUE == bHasBuildedVidRtcp )
		{
			m_cSMcuSpyMana.SetSpyBackVidRtcpAddr(swIndex, tMultiRtcpInfo.m_bySpyMode, tMultiRtcpInfo.m_tVidRtcpAddr);	
		}

		if ( TRUE == bHasBuildedAudRtcp )
		{
			m_cSMcuSpyMana.SetSpyBackAudRtcpAddr(swIndex, tMultiRtcpInfo.m_bySpyMode, tMultiRtcpInfo.m_tAudRtcpAddr);
		}
	}
	else
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuMtExtInfoNotif] TMtSrc(%d-%d) is not in RecvSpySource!\n",
						tSrc.GetMcuId(), tSrc.GetMtId());
		return;
	}

	return ;
}

/*==============================================================================
������    :  OnMMcuRtcpInfoNotif
����      :  �¼��������ϼ�����notify���߼�
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-12-15                 ��־��							create
==============================================================================*/
void CMcuVcInst::OnMMcuRtcpInfoNotif(TMt tSrc, const TSimCapSet &tDstCap,TMsgHeadMsg tHeadMsg)
{	 
	u8			byMediaMode	 = MODE_NONE;
	TBasOutInfo	tBasOutInfo;
	TTransportAddr tVidRtcpAddr;
	TTransportAddr tAudRtcpAddr;
	TMt tOrgSrc ;
	TMt tUnLocalSrc ;
	TSimCapSet tSrcCap;

	tOrgSrc	    = tSrc;
	tUnLocalSrc = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst,tSrc );
	tSrc        = GetLocalMtFromOtherMcuMt(tUnLocalSrc);
    tSrcCap		= m_ptMtTable->GetSrcSCS(tSrc.GetMtId());

	CSendSpy cSendSpy;
	if( m_cLocalSpyMana.GetSpyChannlInfo(tUnLocalSrc, cSendSpy) )
	{
		byMediaMode = m_cLocalSpyMana.GetSpyMode(tUnLocalSrc);
	}

	if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
	{
		TAudioTypeDesc tSrcAudCap;		
		TMt tLocalSrcMt = GetLocalMtFromOtherMcuMt( tSrc );
		TLogicalChannel tSrcAudLgc;
		if (!m_ptMtTable->GetMtLogicChnnl(tLocalSrcMt.GetMtId(), MODE_AUDIO, &tSrcAudLgc, FALSE))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS,  "[IsNeedAdapt] GetMtLogicChnnl Src AUD Mt%d failed!\n", tLocalSrcMt.GetMtId());
		}
		
		tSrcAudCap.SetAudioMediaType( tSrcAudLgc.GetChannelType() );
		tSrcAudCap.SetAudioTrackNum( tSrcAudLgc.GetAudioTrackNum() );
		
		TAudioTypeDesc tDstAudCap;
		TLogicalChannel tDstAudLgc;
		if (!m_ptMtTable->GetMtLogicChnnl(m_tCascadeMMCU.GetMtId(), MODE_AUDIO, &tDstAudLgc, TRUE))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS,  "[IsNeedSpyAdpt] GetMtLogicChnnl Dst Aud Mt%d failed!\n", m_tCascadeMMCU.GetMtId());
		}
		
		tDstAudCap.SetAudioMediaType( tDstAudLgc.GetChannelType() );
		tDstAudCap.SetAudioTrackNum( tDstAudLgc.GetAudioTrackNum() );
		
		// 1.1 �����ǲ���keda�ģ���Ƶ��ƥ�䶼Ҫ������
		if (  tSrcAudCap.GetAudioMediaType() != MEDIA_TYPE_NULL &&
			tDstAudCap.GetAudioMediaType() != MEDIA_TYPE_NULL &&
			( tSrcAudCap.GetAudioMediaType() != tDstAudCap.GetAudioMediaType() 
			||tSrcAudCap.GetAudioTrackNum() != tDstAudCap.GetAudioTrackNum()
			)
			)
		{
			if (FindBasChn2SpyForMt(tUnLocalSrc, tDstCap, MODE_AUDIO, tBasOutInfo))
			{
				//�ҵ���Ӧ��BASͨ��RTCP��Ϣ
				tAudRtcpAddr.SetNull();
				//GetSpyBasRtcpInfo(tBasOutInfo.m_tBasEqp,tBasOutInfo.m_byChnId,tBasOutInfo.m_byFrontOutNum,tBasOutInfo.m_byOutIdx,tAudRtcpAddr);
				GetRemoteRtcpAddr( tBasOutInfo.m_tBasEqp, tBasOutInfo.m_byFrontOutNum+tBasOutInfo.m_byOutIdx, MODE_AUDIO, tAudRtcpAddr);
			}
			else
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[OnMMcuRtcpInfoNotif] Can't find BAS's chnnl for MODE_AUDIO!\n" );
			}
		}
		// 1.2 ��Ƶ��������
		else
		{
			//�������䣬��Ҫ����������ͨ������ƵRTCP��Ϣ��֪�ϼ�
			if (tUnLocalSrc.IsLocal())
			{
				tAudRtcpAddr = tSrcAudLgc.GetSndMediaCtrlChannel();
				//8kh���Դ�ն��Ǵ����նˣ���Ҫ�����ip��MCU��IP
#if defined(_8KH_) || defined(_8KE_) || defined(_8KI_)
				TMultiManuNetAccess tMultiManuNetAccess;
				g_cMcuAgent.GetMultiManuNetAccess(tMultiManuNetAccess);
				TGKProxyCfgInfo tGKProxyCfgInfo;
				g_cMcuAgent.GetGkProxyCfgInfo(tGKProxyCfgInfo);
				if( tGKProxyCfgInfo.IsProxyUsed())
				{
					for( u8 byIdx = 0; byIdx < tMultiManuNetAccess.GetIpSecNum(); byIdx++ )
					{
						u32 dwProyIp = tMultiManuNetAccess.GetIpAddr(byIdx);
						if( dwProyIp == tAudRtcpAddr.GetIpAddr() )
						{
							LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_SPY,"8KE/H/I in dwIp:%x is Multi Proxy Ip\n",dwProyIp);
							tAudRtcpAddr.SetIpAddr( g_cMcuAgent.GetMpcIp() );
							break;
						}
					}
				}
#endif
			}
			else
			{
				GetSpyCascadeRtcpInfo(tUnLocalSrc,tVidRtcpAddr,tAudRtcpAddr);
			}
		}
	}
	
	//��Ƶ����
	if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
	{
		// ������
		if(IsNeedSpyAdpt(tSrc, tDstCap, MODE_VIDEO))
		{
			if (FindBasChn2SpyForMt(tUnLocalSrc, tDstCap, MODE_VIDEO, tBasOutInfo))
			{
				//�ҵ���Ӧ��BASͨ��RTCP��Ϣ
				tVidRtcpAddr.SetNull();
				//GetSpyBasRtcpInfo(tBasOutInfo.m_tBasEqp,tBasOutInfo.m_byChnId,tBasOutInfo.m_byFrontOutNum,tBasOutInfo.m_byOutIdx,tVidRtcpAddr);
				GetRemoteRtcpAddr(tBasOutInfo.m_tBasEqp, tBasOutInfo.m_byFrontOutNum+tBasOutInfo.m_byOutIdx, MODE_VIDEO, tVidRtcpAddr);
			}
			else
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[OnMMcuRtcpInfoNotif] Can't find BAS's chnnl for MODE_VIDEO!\n" );
			}
		}
		else
		{
			//�������䣬��Ҫ����������ͨ������ƵRTCP��Ϣ��֪�ϼ�
			if (tUnLocalSrc.IsLocal())
			{
				TLogicalChannel tLogiclChannel;
				m_ptMtTable->GetMtLogicChnnl(tUnLocalSrc.GetMtId(), LOGCHL_VIDEO, &tLogiclChannel, FALSE);
				tVidRtcpAddr = tLogiclChannel.GetSndMediaCtrlChannel();
				//8kh���Դ�ն��Ǵ����նˣ���Ҫ�����ip��MCU��IP
#if defined(_8KH_) || defined(_8KE_) || defined(_8KI_)
				TMultiManuNetAccess tMultiManuNetAccess;
				g_cMcuAgent.GetMultiManuNetAccess(tMultiManuNetAccess);
				TGKProxyCfgInfo tGKProxyCfgInfo;
				g_cMcuAgent.GetGkProxyCfgInfo(tGKProxyCfgInfo);
				if( tGKProxyCfgInfo.IsProxyUsed())
				{
					for( u8 byIdx = 0; byIdx < tMultiManuNetAccess.GetIpSecNum(); byIdx++ )
					{
						u32 dwProyIp = tMultiManuNetAccess.GetIpAddr(byIdx);
						if( dwProyIp == tVidRtcpAddr.GetIpAddr() )
						{
							LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_SPY,"8KE/H/I in dwIp:%x is Multi Proxy Ip\n",dwProyIp);
							tVidRtcpAddr.SetIpAddr( g_cMcuAgent.GetMpcIp() );
							break;
						}
					}
				}
#endif
			}
			else
			{
				GetSpyCascadeRtcpInfo(tUnLocalSrc,tVidRtcpAddr,tAudRtcpAddr);
			}
		}
	}
	
	//���ϼ�����RTCP��ϢNOTIFY
	SendMMcuMtExtInfoNotif(tOrgSrc,tHeadMsg,byMediaMode,tVidRtcpAddr,tAudRtcpAddr);

	return;
}

/*==============================================================================
������    :  GetSpyBasRtcpInfo
����      :  ��ö�ش�����BAS��RTCP��Ϣ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-12-15                 ��־��							create
==============================================================================*/
void CMcuVcInst::GetSpyBasRtcpInfo(TEqp &tBas, u8 &byChnId, u8 &byFrontOutNum, u8 &byOutIdx, TTransportAddr &tRtcpAddr)
{
	u32		dwRtcpSwitchIp  = 0;
	u16		wRtcpSwitchPort = 0;
	u32		dwSrcIp         = 0;

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[GetSpyBasRtcpInfo] byChnId = %d\n",byChnId);

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	#ifdef _8KE_
	wRtcpSwitchPort = BAS_8KE_LOCALSND_PORT + (tBas.GetEqpId()-BASID_MIN)*BAS_8KE_PORTSPAN 
		+ PORTSPAN * (BAS_8KE_PORTSPAN * byChnId + byOutIdx) + 1; 
	#endif
	
	#ifdef _8KH_
	wRtcpSwitchPort = BAS_8KE_LOCALSND_PORT + (tBas.GetEqpId()-BASID_MIN)*BAS_8KH_PORTSPAN 
								+ PORTSPAN * (BAS_8KH_PORTSPAN * byChnId + byOutIdx) + 1; 
	#endif

	#ifdef _8KI_
	wRtcpSwitchPort = BAS_8KE_LOCALSND_PORT +PORTSPAN * (byFrontOutNum+byOutIdx) + 1; 
	#endif

	dwRtcpSwitchIp = g_cMcuVcApp.GetEqpIpAddr(tBas.GetEqpId());
	tRtcpAddr.SetIpAddr(dwRtcpSwitchIp);
	tRtcpAddr.SetPort(wRtcpSwitchPort);
#else
	g_cMpManager.GetSwitchInfo(tBas, dwRtcpSwitchIp, wRtcpSwitchPort, dwSrcIp);
	tRtcpAddr.SetIpAddr(dwSrcIp);
	tRtcpAddr.SetPort(wRtcpSwitchPort + PORTSPAN * (byFrontOutNum+byOutIdx) + 1);
#endif				
}

/*==============================================================================
������    :  GetSpyCascadeRtcpInfo
����      :  ��ö�ش����ڱ�������ͨ����RTCP��Ϣ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-12-10                 ��־��							create
==============================================================================*/
void CMcuVcInst::GetSpyCascadeRtcpInfo(TMt tSrc, TTransportAddr &tVidRtcpAddr, TTransportAddr &tAudRtcpAddr)
{
	if ( IsLocalAndSMcuSupMultSpy(tSrc.GetMcuId()) )
	{
		CRecvSpy tRrvSpyInfo;
		m_cSMcuSpyMana.GetRecvSpy( tSrc, tRrvSpyInfo );
		u32 dwMpIpAddr = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( GetLocalMtFromOtherMcuMt(tSrc).GetMtId()) );
		
		//��Ƶ�ش���ַ
		tVidRtcpAddr.SetIpAddr( dwMpIpAddr );
		tVidRtcpAddr.SetPort( tRrvSpyInfo.m_tSpyAddr.GetPort() + 1 );
		
		//��Ƶ�ش���ַ
		tAudRtcpAddr.SetIpAddr( dwMpIpAddr );
		tAudRtcpAddr.SetPort( tRrvSpyInfo.m_tSpyAddr.GetPort() + 3 );

		ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_SPY,"[GetSpyCascadeRtcpInfo]tSrc:%d-%d is support multispy!\n",tSrc.GetMcuId(), tSrc.GetMtId());
	}
	else
	{
		TMt tLocalSrc = GetLocalMtFromOtherMcuMt(tSrc); 
		TLogicalChannel tLogiclChannel;
		m_ptMtTable->GetMtLogicChnnl(tLocalSrc.GetMtId(), LOGCHL_AUDIO, &tLogiclChannel, FALSE);
		tAudRtcpAddr = tLogiclChannel.GetSndMediaCtrlChannel();

		m_ptMtTable->GetMtLogicChnnl(tLocalSrc.GetMtId(), LOGCHL_VIDEO, &tLogiclChannel, FALSE);
		tVidRtcpAddr = tLogiclChannel.GetSndMediaCtrlChannel();

		ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_SPY,"[GetSpyCascadeRtcpInfo]tSrc:%d-%d is not support multispy!\n",tSrc.GetMcuId(), tSrc.GetMtId());
	}
}

/*==============================================================================
������    :  SendMMcuMtExtInfoNotif
����      :  ���ϼ�����RTCP��ϢNOTIFY
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-12-15                 ��־��							create
==============================================================================*/
void CMcuVcInst::SendMMcuMtExtInfoNotif(TMt tSpySrc, TMsgHeadMsg tHeadMsg, u8 bySpyMode, TTransportAddr &tVidRtcpAddr, TTransportAddr &tAudRtcpAddr)
{
	TMsgHeadMsg tHeadMsgMtextNotif;	 
	tHeadMsgMtextNotif.m_tMsgSrc = tHeadMsg.m_tMsgDst;
	tHeadMsgMtextNotif.m_tMsgDst = tHeadMsg.m_tMsgSrc;
	
	CServMsg cMsg;
	TMultiRtcpInfo tMultiRtcpInfo;
	tMultiRtcpInfo.m_tSrcMt = tSpySrc;
	tMultiRtcpInfo.m_bySpyMode = bySpyMode;
	tMultiRtcpInfo.m_tVidRtcpAddr = tVidRtcpAddr;
	tMultiRtcpInfo.m_tAudRtcpAddr = tAudRtcpAddr;
	cMsg.SetEventId( MCU_MCU_MTEXTINFO_NOTIF );
	cMsg.SetMsgBody( (u8*)&tHeadMsgMtextNotif,sizeof(TMsgHeadMsg) );
	cMsg.CatMsgBody( (u8*)&tMultiRtcpInfo,sizeof(TMultiRtcpInfo) );
	
	SendMsgToMt( m_tCascadeMMCU.GetMtId(),cMsg.GetEventId(),cMsg );
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[SendMMcuMtExtInfoNotif]SpyMode:%d-SpySrc:McuID%d-MtID%d\n 's RTCP info send to MMCU\n",
				  bySpyMode,tSpySrc.GetMcuId(),tSpySrc.GetMtId());
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[SendMMcuMtExtInfoNotif]vid :0x%x-%d,aud :0x%x-%d\n",
				  tVidRtcpAddr.GetIpAddr(), tVidRtcpAddr.GetPort(), tAudRtcpAddr.GetIpAddr(), tAudRtcpAddr.GetPort() 
			 );
}


// �����ն���Ϣ�ͷ�һ���ش�ͨ��
// BOOL32 CMcuVcInst::FreeSpyChannlInfo( const TMt &tMt, u8 bySpyMode/*= MODE_BOTH*/, BOOL32 bForce/*= FALSE*/ )
// {
// 	CRecvSpy tSrcSpyInfo;
// 	u32 dwMaxSpyBW = 0;
// 	s32 nRemainSpyBW = 0;
// 	
// 	//��ô�mcu���ܴ���ʣ�����
// 	if (!m_ptConfOtherMcTable->GetMultiSpyBW(tMt.GetMcuId(), dwMaxSpyBW, nRemainSpyBW))
// 	{
// 		return FALSE;
// 	}
// 
// 	//��ô��ն˵Ļش���Ϣ
// 	if (!m_cSMcuSpyMana.GetRecvSpy(tMt, tSrcSpyInfo))
// 	{
// 		return FALSE;
// 	}
// 
// 	//�ͷŴ��ն˵Ĵ���
// 	//dwRemainSpyBW += tSrcSpyInfo.m_dwTotalBW;
// 	m_ptConfOtherMcTable->SetMcuSupMultSpyRemainBW(tMt.GetMcuId(), nRemainSpyBW);
// 
// 	return TRUE;
// }


/*==============================================================================
������    :  ShowMultiSpyMMcuInfo
����      :  ��ʾ�����洢�������¼��Ķ�ش�����Ϣ�����б�����ش�����Ϣ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-07-12                 ½����							create
==============================================================================*/
void CMcuVcInst::ShowConfMultiSpy( void )
{
	u8 byMtLoop = 1,byMcuLoop = 1,byMtLoop1 = 0;
	u8 byMcuId = 0;
	TConfMcInfo *ptMcInfo = NULL;
	TConfMtInfo *ptConfMtInfo = NULL;
	
	BOOL32 bInConf = FALSE,bInJoinedConf = FALSE;
	char achTemp[255];

	TMtAlias tMtAliasH323id, tMtAliasE164, tMtAliasH320id, tMtAliasH320Alias;
	TMtAlias tDailAlias;

	CSendSpy *pcSendSpy = NULL;
	u16 wSpyNum = MAXNUM_CONF_SPY/*m_cLocalSpyMana.GetSendSpyNum()*/;
	//u8 byMtId = 0;

    
    m_tConf.GetConfId().GetConfIdString(achTemp, sizeof(achTemp));
    StaticLog("cConfId %s\n", achTemp );
	StaticLog( "\n������ش���Ϣ:\n");
	
	TMtExt tMtExt;

	//StaticLog( "����\tMCUID\tMTID\t\tIP\t\t  Alias\tSpyPort SpyMode VidBas chl AudBas chl BasMode PrsId PrsChl\n");
	for (byMtLoop = 0; byMtLoop < wSpyNum; byMtLoop++)
	{
		pcSendSpy = m_cLocalSpyMana.GetSendSpy(byMtLoop);

		if (pcSendSpy == NULL)
		{
			break;
		}

		if (pcSendSpy->m_tSpy.IsNull())
		{
			continue;
		}

		//byMtId = pcSendSpy->m_tSpy.GetMtId();

		if( m_tConfAllMtInfo.MtJoinedConf(pcSendSpy->m_tSpy) )
		{
			sprintf( achTemp, "�� %c", 0 );
		}
		else
		{
			sprintf( achTemp, "�� %c", 0 ); 
		}

		if( pcSendSpy->m_tSpy.IsLocal() )
		{
			tMtAliasH323id.SetNull();
			tMtAliasE164.SetNull();
			tMtAliasH320id.SetNull();
			tMtAliasH320Alias.SetNull();
			m_ptMtTable->GetMtAlias( pcSendSpy->m_tSpy.GetMtId(), mtAliasTypeH323ID, &tMtAliasH323id );
			m_ptMtTable->GetMtAlias( pcSendSpy->m_tSpy.GetMtId(), mtAliasTypeE164, &tMtAliasE164 );
			m_ptMtTable->GetMtAlias( pcSendSpy->m_tSpy.GetMtId(), mtAliasTypeH320ID, &tMtAliasH320id );
			m_ptMtTable->GetMtAlias( pcSendSpy->m_tSpy.GetMtId(), mtAliasTypeH320Alias, &tMtAliasH320Alias);			
        
			BOOL32 bRet = m_ptMtTable->GetDialAlias(pcSendSpy->m_tSpy.GetMtId(), &tDailAlias);
        
			if ( !bRet )
			{
				StaticLog("McuIdx(%d) McuId(%d)   IP:0x%08x 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s SpyPort:%d SpyMode:%d\n,",
								pcSendSpy->m_tSpy.GetMcuId(),pcSendSpy->m_tSpy.GetMtId(),m_ptMtTable->GetIPAddr( pcSendSpy->m_tSpy.GetMtId() ),
								tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
								tMtAliasH320Alias.m_achAlias,pcSendSpy->m_tSpyAddr.GetPort(),pcSendSpy->GetSpyMode()
								);
			}
			else
			{
				if ( mtAliasTypeTransportAddress == tDailAlias.m_AliasType )
				{
					StaticLog("McuIdx(%d) McuId(%d)   IP:0x%08x 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s DialAlias.%s@%d SpyPort:%d SpyMode:%d\n",
								pcSendSpy->m_tSpy.GetMcuId(),pcSendSpy->m_tSpy.GetMtId(),m_ptMtTable->GetIPAddr( pcSendSpy->m_tSpy.GetMtId() ),
								tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
								tMtAliasH320Alias.m_achAlias,
								StrOfIP(tDailAlias.m_tTransportAddr.GetIpAddr()), tDailAlias.m_tTransportAddr.GetPort(),
								pcSendSpy->m_tSpyAddr.GetPort(), pcSendSpy->GetSpyMode()
								);
				}
				else
				{
					StaticLog("McuIdx(%d) McuId(%d)   IP:0x%08x 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s DialAlias.%s SpyPort:%d SpyMode:%d\n,",
								pcSendSpy->m_tSpy.GetMcuId(),pcSendSpy->m_tSpy.GetMtId(),m_ptMtTable->GetIPAddr( pcSendSpy->m_tSpy.GetMtId() ),
								tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
								tMtAliasH320Alias.m_achAlias,
								tDailAlias.m_achAlias,pcSendSpy->m_tSpyAddr.GetPort(), pcSendSpy->GetSpyMode()
								);
				}
			}
		}
		else
		{
			ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(pcSendSpy->m_tSpy.GetMcuId());
			if(ptMcInfo == NULL)
			{
				continue;
			}
			ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr(pcSendSpy->m_tSpy.GetMcuId()); 
			if(ptConfMtInfo == NULL)
			{
				continue;
			}
			for( byMtLoop1 = 1; byMtLoop1 <= MAXNUM_CONF_MT; byMtLoop1++ )
			{
				tMtExt = ptMcInfo->m_atMtExt[byMtLoop1-1];
				if( tMtExt.GetMtId() == pcSendSpy->m_tSpy.GetMtId() )
				{
					StaticLog("McuIdx(%d) McuId(%d) IP:0x%08x(%s) Alias.%s SpyPort:%d SpyMode:%d\n",
								pcSendSpy->m_tSpy.GetMcuId(),pcSendSpy->m_tSpy.GetMtId(),tMtExt.GetIPAddr(),
								StrOfIP(tMtExt.GetIPAddr()),tMtExt.GetAlias(),pcSendSpy->m_tSpyAddr.GetPort(), pcSendSpy->GetSpyMode()
								);
					break;
				}
			}
		}	
		StaticLog( "\t\t\t SpyCap-->VideoCap<Media.%d, Res.%d, BR.%d, Fps.%d>-->AudCap<Media.%d>",
								pcSendSpy->GetSimCapset().GetVideoMediaType(),
								pcSendSpy->GetSimCapset().GetVideoResolution(),
								pcSendSpy->GetSimCapset().GetVideoMaxBitRate(),
								MEDIA_TYPE_H264 == pcSendSpy->GetSimCapset().GetVideoMediaType()?
								pcSendSpy->GetSimCapset().GetUserDefFrameRate():pcSendSpy->GetSimCapset().GetVideoFrameRate(),
								pcSendSpy->GetSimCapset().GetAudioMediaType());
	}

	StaticLog( "\n�����¼���ش���Ϣ:\n");

	//��ȡ��mcu֧�ֵ��ܴ���ʣ�����
	u32 dwMaxSpyBW = 0;
	s32 nRemainSpyBW = 0;
	
	u16 wMcuIdx = INVALID_MCUIDX;

	// liuxu, ��byMcuLoopΪwLoop. �����ȥ����Ƚϴ�
	for( u16 wLoop = 0; wLoop < m_tConfAllMtInfo.GetMaxMcuNum(); wLoop++ )
	{
		wMcuIdx = m_tConfAllMtInfo.GetMtInfo(wLoop).GetMcuIdx();
		if (!IsValidSubMcuId(wMcuIdx))
		{
			continue;
		}
		
		if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx( wMcuIdx) )
		{
			continue;
		}
		
		ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
		if(ptMcInfo == NULL)
		{
			continue;
		}
		ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr(wMcuIdx); 
		if(ptConfMtInfo == NULL)
		{
			continue;
		}

		m_ptConfOtherMcTable->GetMultiSpyBW(wMcuIdx, dwMaxSpyBW, nRemainSpyBW);
		
		StaticLog("McuIdx(%d) McuId(%d): MaxBW:%ld ReMainBW:%ld\n", wMcuIdx,GetFstMcuIdFromMcuIdx( wMcuIdx ),  dwMaxSpyBW, nRemainSpyBW);
	}


	CRecvSpy cRecvSpy;
	u8 abyMcuId[MAX_CASCADEDEPTH-1];
	
	for( byMcuLoop = 0;byMcuLoop < MAXNUM_CONF_SPY;++byMcuLoop)
	{
		if( m_cSMcuSpyMana.GetRecvSpy( byMcuLoop,cRecvSpy ) )
		{
			if( !cRecvSpy.m_tSpy.IsNull() )
			{
				m_tConfAllMcuInfo.GetMcuIdByIdx( cRecvSpy.m_tSpy.GetMcuId(),&abyMcuId[0] );
				if( m_tConfAllMcuInfo.IsSMcuByMcuIdx(cRecvSpy.m_tSpy.GetMcuId()) )
				{
					StaticLog( "McuIdx(%d) McuId(%d) MtId(%d)   VSpyDstNum:%d ASpyDstNum:%d SpyPort:%d VBW:%d ABW:%d SpyMode:%d State:%d\n",
							cRecvSpy.m_tSpy.GetMcuId(),abyMcuId[0],cRecvSpy.m_tSpy.GetMtId(),
							cRecvSpy.m_byVSpyDstNum,cRecvSpy.m_byASpyDstNum,cRecvSpy.m_tSpyAddr.GetPort(),
							cRecvSpy.m_dwVidBW, cRecvSpy.m_dwAudBW,cRecvSpy.m_bySpyMode,cRecvSpy.m_byUseState
							);
				}
				else
				{
					
					StaticLog( "McuIdx(%d) McuId(%d-%d) MtId(%d) VSpyDstNum:%d ASpyDstNum:%d SpyPort:%d VBW:%d ABW:%d SpyMode:%d State:%d\n",
							cRecvSpy.m_tSpy.GetMcuId(),abyMcuId[0],abyMcuId[1],cRecvSpy.m_tSpy.GetMtId(),
							cRecvSpy.m_byVSpyDstNum,cRecvSpy.m_byASpyDstNum,cRecvSpy.m_tSpyAddr.GetPort(),
							cRecvSpy.m_dwVidBW, cRecvSpy.m_dwAudBW,cRecvSpy.m_bySpyMode,cRecvSpy.m_byUseState
							);
				}
				StaticLog( "\t\t\t Require to tMt<McuId.%d>--> VideoCap<Media.%d, Res.%d, BR.%d, Fps.%d>--> AudCap<Media.%d>!\n",
										abyMcuId[0],
										cRecvSpy.GetSimCapset().GetVideoMediaType(),
										cRecvSpy.GetSimCapset().GetVideoResolution(),
										cRecvSpy.GetSimCapset().GetVideoMaxBitRate(),
										MEDIA_TYPE_H264 == cRecvSpy.GetSimCapset().GetVideoMediaType()?
										cRecvSpy.GetSimCapset().GetUserDefFrameRate():cRecvSpy.GetSimCapset().GetVideoFrameRate(),
										cRecvSpy.GetSimCapset().GetAudioMediaType());
			}
		}
	}
	
}


/*==============================================================================
������    :  IsSupportMultiSpy
����      :  �û����Ƿ�֧�ֶ�ش�
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  BOOL32
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-04-12                 �ܾ���							create
==============================================================================*/
BOOL32 CMcuVcInst::IsSupportMultiSpy( )
{
	return m_tConf.IsSupportMultiSpy();
}
/*==============================================================================
������    :  IsCanSetOutView
����      :  �Ƿ���԰��ն˷���ش�ͨ��
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  BOOL32
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-04-12                 �ܾ���							create
==============================================================================*/
BOOL32 CMcuVcInst::IsCanSetOutView( TMt &tSetInMt, u8 byMode/* = MODE_BOTH */)
{
	if( m_tCascadeMMCU.IsNull() )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[IsCanSetOutView]: m_tCascadeMMCU is Null!\n");
		return FALSE;
	}

	if( !IsSupportMultiSpy() )
	{
		return TRUE;
	}
	
	//u32 dwSpyBW = 0;
	TLogicalChannel     tAudChnnl;
	

	TMt tSrcMtVid,tSrcMtAud;
	tSrcMtVid.SetNull();
	tSrcMtAud.SetNull();

	if( m_ptMtTable->GetMtLogicChnnl( m_tCascadeMMCU.GetMtId(), LOGCHL_VIDEO, &tAudChnnl, TRUE ) )
	{
		m_ptMtTable->GetMtSrc( m_tCascadeMMCU.GetMtId(),&tSrcMtVid,MODE_VIDEO );
	}

	if( m_ptMtTable->GetMtLogicChnnl( m_tCascadeMMCU.GetMtId(), LOGCHL_AUDIO, &tAudChnnl, TRUE ) )
	{
		m_ptMtTable->GetMtSrc( m_tCascadeMMCU.GetMtId(),&tSrcMtAud,MODE_AUDIO );
	}
	
	m_ptMtTable->GetMtLogicChnnl( tSetInMt.GetMtId(), LOGCHL_AUDIO, &tAudChnnl, FALSE );	
	u8 byMediaMode = byMode;

	if( tSrcMtVid == tSetInMt 
		&& ( MODE_BOTH == byMediaMode || MODE_VIDEO == byMediaMode ))
	{
		if( MODE_BOTH == byMediaMode )
		{
			byMediaMode = MODE_AUDIO;
		}
		else if( MODE_VIDEO == byMediaMode )
		{
			byMediaMode = MODE_NONE;
		}
	}
	if( tSrcMtAud == tSetInMt )
	{
		if( MODE_BOTH == byMediaMode )
		{
			byMediaMode = MODE_VIDEO;
		}
		else if( MODE_AUDIO == byMediaMode )
		{
			byMediaMode = MODE_NONE;
		}
	}

	switch( byMediaMode )
	{
	case MODE_VIDEO:
		//dwSpyBW = m_ptMtTable->GetDialBitrate( tSetInMt.GetMtId() );
		break;
	case MODE_AUDIO:
		//dwSpyBW = GetAudioBitrate( tAudChnnl.GetChannelType() );
		break;
	case MODE_BOTH:
		//dwSpyBW = m_ptMtTable->GetDialBitrate( tSetInMt.GetMtId() ) + GetAudioBitrate( tAudChnnl.GetChannelType() );
		break;
	case MODE_NONE:
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[IsCanSetOutView] MediaMode is MODE_NONE, so can't setout mt(%d.%d)\n",
				tSetInMt.GetMcuId(),tSetInMt.GetMtId()
				);		
			//*pwErrorCode = ERR_MCU_BACKCHNNL_HAS_SAMEMT;
		}
		//lint -fallthrough
	default:
		return FALSE;
		
	}

	return TRUE;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuApplySpeakerRsp
    ��    �ܣ� �ϼ�MCU�����뷢�Ե���Ӧ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage *pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    05/13/2010  4.6			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuApplySpeakerRsp(const CMessage *pcMsg)
{
	STATECHECK

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TMt tMt = *(TMt*)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );

    if (tMt.IsMcuIdLocal())
    {
		tMt = m_ptMtTable->GetMt( tMt.GetMtId() );
        if (MCU_MCU_APPLYSPEAKER_ACK == pcMsg->event)
        {
            //������ ״̬ �ն��Լ�����
        }
        else
        {
            //nack����ʵʱ֪ͨ
            NotifyMtSpeakStatus(tMt, emDenid);
        }
    }
    else
    {
        SendMsgToMt( (u8)tMt.GetMcuId(),cServMsg.GetEventId(),cServMsg );
    }
}

/*=============================================================================
�� �� ���� ProcMcuMcuCancelMeSpeakerRsp
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage *pcMsg
�� �� ֵ�� void 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
05/13/2010  4.6			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuCancelMeSpeakerRsp(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TMt tMt = *(TMt*)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
    
    if ( tMt.IsMcuIdLocal() ) 
    {
		tMt = m_ptMtTable->GetMt( tMt.GetMtId() );
		if( MCU_MCU_CANCELMESPEAKER_ACK == cServMsg.GetEventId() )
		{
			NotifyMtSpeakStatus(tMt, emCanceled);
		}
		else
		{

		}
    }
	else
	{
		SendMsgToMt( (u8)tMt.GetMcuId(),cServMsg.GetEventId(),cServMsg );
	}
    
    return;
}

/*=============================================================================
�� �� ���� ProcMcuMcuMcuMcuSpeakStatusNtf
��    �ܣ� MCU������������Ե�״̬֪ͨ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage *pcMsg
�� �� ֵ�� void 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
05/28/2010  4.6			�ܾ���                  ����
=============================================================================*/
void CMcuVcInst::ProcMcuMcuSpeakStatusNtf( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TMt tMt = *(TMt*)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
	u32 dwStatus = *(u32*)( cServMsg.GetMsgBody() + sizeof( TMt ) + sizeof(TMsgHeadMsg) );
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuSpeakStatusNtf] Event(%s) tMt(%d.%d.%d) level.%d dwStatus.%d\n",
		OspEventDesc( pcMsg->event ),tMt.GetMcuId(),
		tMt.GetMcuId(),
		tMt.GetMtId(),
		tHeadMsg.m_tMsgDst.m_abyMtIdentify[tHeadMsg.m_tMsgDst.m_byCasLevel],
		tHeadMsg.m_tMsgDst.m_byCasLevel,
		dwStatus
		);

	tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst,tMt );

	NotifyMtSpeakStatus( tMt,(emMtSpeakerStatus)dwStatus );
}

/*=============================================================================
�� �� ���� RemoveMcu
��    �ܣ� ɾ��ĳ��mcu(ֱ�����ֱ��)
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const TMt& tMt mcu��������ն�
�� �� ֵ�� 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
08/26/2010  4.6			�ܾ���                  ����
=============================================================================*/
void CMcuVcInst::RemoveMcu( const TMt &tMt )
{
	u16 wMcuIdx = INVALID_MCUIDX;
	if( !IsMcu( tMt, &wMcuIdx ) )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "Removemcu(%d,%d) failed, it's not a mcu\n", tMt.GetMcuId(), tMt.GetMtId());
		return;
	}
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "Begin to Remove mcu(%d,%d), mcuidx.%d\n", tMt.GetMcuId(), tMt.GetMtId(), wMcuIdx);

	BOOL32 bIsNotify = FALSE;
	if( INVALID_MCUIDX != wMcuIdx )
	{
		// �ϼ�MCU�ϴ�ͨ����Ա������Ҫ�Ҷ�mcu���¼��ն�
		if (!m_tCascadeMMCU.IsNull() && m_tCascadeMMCU.GetMtId() != tMt.GetMtId())
		{
			TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
			if( ptConfMcInfo != NULL && 
				( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
			{
				TMt tMMcuSpyMt = ptConfMcInfo->m_tSpyMt;
				tMMcuSpyMt = GetLocalMtFromOtherMcuMt(tMMcuSpyMt);
				if( tMMcuSpyMt.GetMtId() == tMt.GetMtId()&&
					tMMcuSpyMt.GetMcuId() == tMt.GetMcuId())
				{
					StopSpyMtCascaseSwitch();				
				}
			}
		}

		BOOL32 bSendChairman = FALSE;
		u16 wbyMcuIdx[ MAXNUM_SUB_MCU ];
		memset( &wbyMcuIdx[0],INVALID_MCUIDX,sizeof(wbyMcuIdx) );
		u8 abyMcuId[MAX_CASCADEDEPTH-1];
		memset( &abyMcuId[0],0,sizeof(abyMcuId) );		
		if( m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
		{
			abyMcuId[0] = tMt.GetMtId();
			m_tConfAllMcuInfo.GetMcuIdxByMcuId( &abyMcuId[0],1,&wbyMcuIdx[0] );
			TConfMtInfo *pcInfo = NULL;
			for( u8 byIdx = 0;byIdx < MAXNUM_SUB_MCU;++byIdx)
			{
				if( INVALID_MCUIDX == wbyMcuIdx[byIdx] || 
					wbyMcuIdx[byIdx] == wMcuIdx )
				{
					continue;
				}
				pcInfo = m_tConfAllMtInfo.GetMtInfoPtr(wbyMcuIdx[byIdx]/*tMt.GetMtId()*/);
				if(pcInfo != NULL)
				{
					pcInfo->SetNull();
				}
				FreeAllRecvSpyByMcuIdx( wbyMcuIdx[byIdx] );
				m_tConfAllMcuInfo.RemoveMcu( wbyMcuIdx[byIdx] );
				m_ptConfOtherMcTable->RemoveMcInfo( wbyMcuIdx[byIdx] );
				m_tConfAllMtInfo.RemoveMcuInfo( wbyMcuIdx[byIdx], tMt.GetConfIdx() );				
				if( m_tApplySpeakQue.RemoveMtByMcuIdx( wbyMcuIdx[byIdx],bSendChairman ) )
				{
					bIsNotify = TRUE;
				}
				
			}
		}
		else
		{
			
		}
		FreeAllRecvSpyByMcuIdx( wMcuIdx );
		m_tConfAllMcuInfo.RemoveMcu( wMcuIdx );
		m_ptConfOtherMcTable->RemoveMcInfo( wMcuIdx );//tMt.GetMtId() );
		m_tConfAllMtInfo.RemoveMcuInfo( wMcuIdx/*tMt.GetMtId()*/, tMt.GetConfIdx() );
		if( m_tApplySpeakQue.RemoveMtByMcuIdx( wMcuIdx,bSendChairman ) )
		{
			bIsNotify = TRUE;
		}
		if( bIsNotify )
		{
			NotifyMcsApplyList( bSendChairman );
		}
	}
}
/*=============================================================================
�� �� ���� IsMcu
��    �ܣ� �ж�һ��mt�Ƿ���mcu
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const TMt& tMt
�� �� ֵ�� BOOL32 TRUE ��ʾtMt��Mcu
				  FALSE ��ʾtMt����Mcu
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
08/26/2010  4.6			�ܾ���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::IsMcu( const TMt& tMt,u16 *pwMcuIdx/* = NULL */)
{
	if( 0 == tMt.GetMtId() )
	{
		return FALSE;
	}

	if( tMt.IsLocal() )
	{
		if( m_ptMtTable->GetMtType( tMt.GetMtId() ) == MT_TYPE_SMCU ||
					m_ptMtTable->GetMtType( tMt.GetMtId() ) == MT_TYPE_MMCU )	
		{
			if( NULL != pwMcuIdx )
			{
				*pwMcuIdx = GetMcuIdxFromMcuId( tMt.GetMtId() );
			}
			return TRUE;
		}
		
		return FALSE;
	}
	
	TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->GetMcInfo( tMt.GetMcuId() );
	if( NULL == ptMcMtInfo )
	{
		return FALSE;
	}

	for( u8 byLoop = 0;byLoop < MAXNUM_CONF_MT ; ++byLoop )
	{
		if( ptMcMtInfo->m_atMtExt[byLoop].GetMtId() == tMt.GetMtId() )
		{
			if( MT_TYPE_SMCU == ptMcMtInfo->m_atMtExt[byLoop].GetMtType() )
			{
				u8 abyMcuMcuId[MAX_CASCADEDEPTH - 1];				
				memset( abyMcuMcuId, 0 , sizeof(abyMcuMcuId) );
				m_tConfAllMcuInfo.GetMcuIdByIdx( tMt.GetMcuId(), abyMcuMcuId );					
				if( 0 == abyMcuMcuId[MAX_CASCADEDEPTH - 2] )
				{
					abyMcuMcuId[MAX_CASCADEDEPTH - 2] = tMt.GetMtId();
					if( NULL != pwMcuIdx )
					{
						m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuMcuId[0],2,pwMcuIdx );
					}					
				}
				else
				{
					if( NULL != pwMcuIdx )
					{
						*pwMcuIdx = INVALID_MCUIDX;
					}					
					return FALSE;
				}	

				return TRUE;
			}
			return FALSE;
		}
	}


	return FALSE;
}
/*=============================================================================
�� �� ���� IsMtInMcu
��    �ܣ� �ж�һ��mt�Ƿ���mcu���ն�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const TMt& tMcu, 
		   const TMt& tMt
		   bDirectlyUnder : �Ƿ�Ҫ����ֱ����ϵ(ֱ���������ն�)
�� �� ֵ�� BOOL32 TRUE ��ʾtMt�Ǵ����� tMcu��
				  FALSE ��ʾtMt�������� tMcu
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
08/26/2010  4.6			������                  ����
08/26/2010  4.6			�ܾ���                  �޸�
08/31/2011  4.6			����                    ���ֱ����ϵ�ж�
=============================================================================*/
BOOL32 CMcuVcInst::IsMtInMcu(const TMt& tMcu, const TMt& tMt, const BOOL32 bDirectlyUnder/* = FALSE*/)
{
	BOOL32 bRet = FALSE;

	if( !IsMcu(tMcu) || tMt.IsNull() )
	{
		return FALSE;
	}

	// [11/1/2011 liuxu] ��Ҫ�ж�confidx
	if ( tMcu.GetConfIdx() != m_byConfIdx
		|| tMt.GetConfIdx() != m_byConfIdx )
	{
		return FALSE;
	}

	u8 abyMcuMcuId[MAX_CASCADEDEPTH - 1];
	memset( abyMcuMcuId, 0 , sizeof(abyMcuMcuId) );
	u16 wMcuMcuIndx = INVALID_MCUIDX;
	if( tMcu.IsLocal() )
	{
		if( tMcu.GetMtId() == 0 )
		{
			return FALSE;
		}
		abyMcuMcuId[0] = tMcu.GetMtId();
		//wMcuMcuIndx = GetMcuIdxFromMcuId( tMcu.GetMtId() );
	}
	else
	{
		wMcuMcuIndx = tMcu.GetMcuIdx();
		if( !m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuMcuIndx, abyMcuMcuId ) )
		{
			return FALSE;
		}
		if( 0 == abyMcuMcuId[MAX_CASCADEDEPTH - 2] )
		{
			abyMcuMcuId[MAX_CASCADEDEPTH - 2] = tMcu.GetMtId();
		}
	}
	
	u16 wMtMcuIndx = tMt.GetMcuIdx();
	if( wMcuMcuIndx == wMtMcuIndx )
	{
		//��ͬmcu�µ�mt��û�д�����ϵ
		return FALSE;
	}
	
	u8 abyMtMcuId[MAX_CASCADEDEPTH - 1];	
	memset( abyMtMcuId, 0 , sizeof(abyMtMcuId) );	
	if( !m_tConfAllMcuInfo.GetMcuIdByIdx( wMtMcuIndx, abyMtMcuId ) )
	{
		return FALSE;
	}

	for( u8 byIdx = 0;byIdx < MAX_CASCADEDEPTH - 1;++byIdx )
	{
		// [8/31/2011 liuxu] Ҫ���ж�ֱ����ϵʱ, ����һһ���бȽ�,ֱ�����м����ʾ
		// ����ͬ, ����Ϊ��ֱ����ϵ; ��ֱ����ϵʱ, ֻҪmcu�ı�ʾ��Ӧ�ն���ͬλ�õı�ʾ
		// ��ͬ�Ϳ�����
		if( !bDirectlyUnder && 0 == abyMcuMcuId[byIdx] )
		{
			break;
		}
		if( abyMcuMcuId[byIdx] != abyMtMcuId[byIdx] )
		{
			return FALSE;
		}
	}

	return TRUE;

}

/*=============================================================================
�� �� ���� IsSmcuViewMtInMcu
��    �ܣ� �ж��ն�tmt�Ƿ���tmcu���ϴ�ͨ����Ա
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� [in] tMt, �ն�
		   [in] tMcu, mcu
�� �� ֵ�� true, ���ն��Ǹ�mcu���ϴ�ͨ����Ա; false, ����
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
10/10/2011  4.6			liuxu                  ����
=============================================================================*/	
BOOL32 CMcuVcInst::IsSmcuViewMtInMcu( const TMt& tMt, const TMcu& tMcu)
{
	// У��tMt�Ƿ�Ϸ�
	if ( tMt.IsNull() || tMt.IsLocal() )
	{
		return FALSE;
	}

	// У��tMcu�Ƿ�Ϸ�
	if ( tMcu.IsNull() || !IsMcu(tMcu) )
	{
		return FALSE;
	}

	// У���Ƿ����mcuֱ����ϵ
	if (!IsMtInMcu(tMcu, tMt))
	{
		return FALSE;
	}

	TMt tSMcuViewMt = GetSMcuViewMt(tMcu);
	if(tSMcuViewMt.IsNull())
	{
		return FALSE;
	}

	if ( tMt == tSMcuViewMt )
	{
		return TRUE;
	}

	// �����Ȼ��mcu, ��ݹ������
	if ( IsMcu( tSMcuViewMt ) )
	{
		return IsSmcuViewMtInMcu( tMt, tSMcuViewMt);
	}

	return FALSE;
}

/*=============================================================================
	�� �� ���� GetDirectMcuFromMt
	��    �ܣ� ���tmtֱ��mcu
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� [in] TMt &tMt
	�� �� ֵ�� ֱ��mcu
	-----------------------------------------------------------------------------
	�޸ļ�¼��
	��  ��		�汾		�޸���		�߶���    �޸�����
	12/07/2011  4.6		yanghuaizhi                  ����
=============================================================================*/	
TMt CMcuVcInst::GetDirectMcuFromMt( const TMt &tMt )
{
	TMt tMcu;
	if (tMt.IsLocal())
	{
		return tMt;
	}
	u8 byFstMcuId = GetFstMcuIdFromMcuIdx(tMt.GetMcuIdx());
	tMcu = m_ptMtTable->GetMt(byFstMcuId);
	// ��ֱ����ϵ
	if (!IsMtInMcu(tMcu, tMt, TRUE))
	{
		u16 wMcuIdx = GetMcuIdxFromMcuId(byFstMcuId);
		u8 abyMcuId[MAX_CASCADEDEPTH-1];
		memset( &abyMcuId[0],0,sizeof(abyMcuId) );
		if( m_tConfAllMcuInfo.GetMcuIdByIdx( tMt.GetMcuIdx(),&abyMcuId[0] ) )
		{
			tMcu.SetMcuIdx(wMcuIdx);
			tMcu.SetMtId(abyMcuId[1]);
		}
	}

	return tMcu;
}

/*=============================================================================
	�� �� ���� GetSMcuViewMt
	��    �ܣ� ��ȡ�¼�mcu���ϴ�ͨ����Ա
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� [in] tMcu, Ҫ�ҵ�mcu
			   [in] bDeepest, true, ����ײ���ϴ�ͨ����Ա; false, ��ֱ���ϴ�ͨ����Ա
	�� �� ֵ�� �ҵ����ϴ��ն�
	-----------------------------------------------------------------------------
	�޸ļ�¼��
	��  ��		�汾		�޸���		�߶���    �޸�����
	10/10/2011  4.6			liuxu                  ����
=============================================================================*/	
TMt CMcuVcInst::GetSMcuViewMt( const TMcu& tMcu, const BOOL32 bDeepest /*= FALSE*/ )
{
	TMt tSMcuViewMt;				// �����ص��ϴ��ն�
	tSMcuViewMt.SetNull();			// ������

	// ������mcu, ����
	if (!IsMcu(tMcu))
	{
		return tSMcuViewMt;
	}

	// ��ȡmcu��Ϣ
	TConfMcInfo *ptConfOtherMcInfo = GetMcuMcInfo( tMcu );
	if (!ptConfOtherMcInfo)
	{
		return tSMcuViewMt;
	}

	// ��ȡ��mcu�ϴ�ͨ����Ա
	tSMcuViewMt = ptConfOtherMcInfo->m_tMMcuViewMt;
	if (tSMcuViewMt.IsNull())
	{
		return tSMcuViewMt;
	}
	
	// �����mcu, ��Ҫ����ײ���ϴ�ͨ����Ա, ��ݹ������
	if ( IsMcu( tSMcuViewMt ) && bDeepest )
	{
		return GetSMcuViewMt( tSMcuViewMt, bDeepest);
	}

	// �Ѿ��ҵ���, ����true
	return tSMcuViewMt;
}


/*=============================================================================
�� �� ���� GetVscSMCUMaxBW
��    �ܣ� ����VCS�����¼�����ϯ����������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� ��
�� �� ֵ�� u32 0��ʾ����������ֵʧ�ܣ�����ֵ��ʾ������ֵ
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
09/11/2010  4.6			�´�ΰ                  ����
=============================================================================*/		
u32 CMcuVcInst::GetVscSMCUMaxBW(void)
{
	// �¼�����ϯ������ֵΪ����N+3+1+4������������
	u16 wUsingTvwallChlNum = 0;	// �¼�����ϯ�ĵ���ǽͨ��ʹ������N·
	u8 bySMCUVmpChlNum = 3;		// �¼�����ϯ�Ļ���ϳ�·����3·
	u8 bySMCUSelMTChlNum = 1;	// ѡ���¼��ն�Ԥ��·����1·
	u8 bySMCUMonitorChlNum = 4;	// ���õ���ϯԤ�����Ԥ��·����4·
	u32 dwSMCUMaxBw = 0;        // ��0����ʾ�����¼�����ϯ������ֵʧ��
	
	u32 dwConfBW = m_tConf.GetBitRate();

	u8 byTemplateConfIdx = g_cMcuVcApp.GetTemConfIdxByE164(m_tConf.GetConfE164());
		
	TTemplateInfo tTemConf;
	if(byTemplateConfIdx != 0 && g_cMcuVcApp.GetTemplate(byTemplateConfIdx, tTemConf))
	{
		for( u8 byChlIdx = 0; byChlIdx < MAXNUM_CONF_HDUBRD * MAXNUM_HDU_CHANNEL; byChlIdx++)
		{
			THduModChnlInfo tInfo = tTemConf.m_tHduModule.GetOneHduChnlInfo(byChlIdx);
			if(tInfo.GetMemberType() == TW_MEMBERTYPE_VCSAUTOSPEC)
			{
				wUsingTvwallChlNum ++;
			}
		}

		for( u8 byTvModulIdx = 0 ; byTvModulIdx < MAXNUM_PERIEQP_CHNNL; byTvModulIdx++)
		{
			TTvWallModule tTvwall;
			tTemConf.m_tMultiTvWallModule.GetTvModuleByIdx(byTvModulIdx,tTvwall);
			for(u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL_INSMOUDLE; byIdx++)
			{
				if(tTvwall.m_abyMemberType[byIdx] == TW_MEMBERTYPE_VCSAUTOSPEC)
				{
					wUsingTvwallChlNum ++;
				}
			}
		}
		

		dwSMCUMaxBw = (wUsingTvwallChlNum+bySMCUMonitorChlNum+bySMCUSelMTChlNum+bySMCUVmpChlNum)*m_tConf.GetBitRate();
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[GetVscSMCUMaxBW] TVWall Channel Num == %u, confBitRate == %u, SMCU MaxBw == %u\n",wUsingTvwallChlNum,m_tConf.GetBitRate(),dwSMCUMaxBw);
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[GetVscSMCUMaxBW] Failed to caculate Max Bandwidth of SMCU!\n");
	}

	return dwSMCUMaxBw;
}

/*=============================================================================
�� �� ���� ProcMcuMcuChangeMtSecVidSendCmd
��    �ܣ� ������Ʒ����˷�˫��������Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
05/06/2011  4.6			��ʤ��                  ����
=============================================================================*/	
//[5/4/2011 zhushengze]VCS���Ʒ����˷�˫��
void CMcuVcInst::ProcMcuMcuChangeMtSecVidSendCmd(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
	STATECHECK;	    

    TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
    TMt tMt = *(TMt*)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg));
    u8 byIsSendDStream = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TMt));

    //ת��Ϊ���ر�ʶ
//    tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc, tMt );
    tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst, tMt );

    //VCS����˫��״̬��ת
    if (VCS_CONF == m_tConf.GetConfSource())
    {
        if (1 == byIsSendDStream)
        {
            m_cVCSConfStatus.SetConfDualEnable(CONF_DUALSTATUS_ENABLE);
            ConfPrint(MID_MCU_CALL, LOG_LVL_KEYSTATUS, "mmcu control send DStream, ConfDual Enable!\n");
        }
        else
        {
            m_cVCSConfStatus.SetConfDualEnable(CONF_DUALSTATUS_DISABLE);
            ConfPrint(MID_MCU_CALL, LOG_LVL_KEYSTATUS, "mmcu control close DStream, ConfDual Disable!\n");
        }
    }
    
    if (!tMt.IsLocal())
    {//����Ǳ����ն˼���Ͷ�ݸ��¼�MCU

        tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
        cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
        cServMsg.CatMsgBody( (u8*)&tMt,sizeof(TMt) );
        cServMsg.CatMsgBody( (u8*)&byIsSendDStream,sizeof(u32) );

        cServMsg.SetEventId(u16(MCU_MCU_CHANGEMTSECVIDSEND_CMD));
        SendMsgToMt( u8(tMt.GetMcuId()), MCU_MCU_CHANGEMTSECVIDSEND_CMD, cServMsg );
        return;
    }
    else
    {

        if (!m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
        {
            ConfPrint(MID_MCU_CALL, LOG_LVL_KEYSTATUS, "[ProcMcuMcuChangeMtSecVidSendCmd]MT:%d is not in conf!\n", tMt.GetMtId());
            return;
        }
        cServMsg.SetEventId(MCU_MT_CHANGEMTSECVIDSEND_CMD);
        cServMsg.SetMsgBody((u8*)&byIsSendDStream, sizeof(u8));
        SendMsgToMt(tMt.GetMtId(), cServMsg.GetEventId(), cServMsg);
    }

    return;
}

/*==============================================================================
������    :  OnPresetinFailed
����      :  ������������յ�presetinnack������µ�presetinʧ�ܴ���

  �㷨ʵ��  :  
  ����˵��  :  
  ����ֵ˵��:  
  -------------------------------------------------------------------------------
  �޸ļ�¼  :  
  ��  ��       �汾          �޸���          �߶���          �޸ļ�¼
  20110421     4.6.2         �ܾ���						   create
==============================================================================*/
void CMcuVcInst::OnPresetinFailed( const TMt &tMt )
{
	if( tMt.IsNull() )
	{
		return;
	}
	
	if( VCS_CONF == m_tConf.GetConfSource() && 
		ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
		tMt ==  m_cVCSConfStatus.GetReqVCMT() )
	{
		ProcVCMTOverTime();
	}
}

/*=============================================================================
�� �� ���� ProcMcuMcuTransparentMsgNotify
��    �ܣ� ������桢�ն���Ϣ͸��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2/23/2012   4.6			��ʤ��                  ����
=============================================================================*/	
void CMcuVcInst::ProcMcuMcuTransparentMsgNotify(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    STATECHECK;	    
    
    TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
    TMt tMt = *(TMt*)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg));
    u8* abyMsgContent = (u8*)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TMt) );
    u16 dwMsgLen = cServMsg.GetServMsgLen() - sizeof(TMsgHeadMsg) - sizeof(TMt);
    
    tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst, tMt );
    
    if (!tMt.IsLocal())
    {//����Ǳ����ն˼���Ͷ�ݸ��¼�MCU
        
        tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
        cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
        cServMsg.CatMsgBody( (u8*)&tMt,sizeof(TMt) );
        cServMsg.CatMsgBody( (u8*)abyMsgContent,dwMsgLen );
        
        cServMsg.SetEventId(u16(MCU_MCU_TRANSPARENTMSG_NOTIFY));
        SendMsgToMt( u8(tMt.GetMcuId()), MCU_MCU_TRANSPARENTMSG_NOTIFY, cServMsg );
        return;
    }
    else
    {
        
        if (!m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
        {
            ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuTransparentMsgNotify]MT:%d is not in conf!\n", tMt.GetMtId());
            return;
        }
        cServMsg.SetEventId(MCU_MT_TRANSPARENTMSG_NOTIFY);
        cServMsg.SetMsgBody((u8*)abyMsgContent, dwMsgLen);
        SendMsgToMt(tMt.GetMtId(), cServMsg.GetEventId(), cServMsg);
        return;
    }
}

/*=============================================================================
�� �� ���� SetRealSndSpyBandWidth
��    �ܣ� E1��·����,�¼�mcu��¼��ʵ�ϴ�����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
20120518   4.7.1		�ܾ���                  ����
=============================================================================*/	
void CMcuVcInst::SetRealSndSpyBandWidth( u32 dwBandWidth )
{
	m_dwRealSndSpyBandWidth = dwBandWidth;
}

/*=============================================================================
�� �� ���� SetRealSndSpyBandWidth
��    �ܣ� E1��·����,�¼�mcu��ȡ��ʵ�ϴ�����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
20120518   4.7.1		�ܾ���                  ����
=============================================================================*/	
u32 CMcuVcInst::GetRealSndSpyBandWidth( void )
{
	return m_dwRealSndSpyBandWidth;
}
/*====================================================================
������      ��UpdateTvwOnSMcuViewMtChg
����        �����ش�ģʽʱ, �ϴ��ն˸ı�ʱ���¼��ͨ��״̬
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����[in]tNewSMcuViewMt -- ���������ϴ��ն�
����ֵ˵��  ����
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/06/28  4.7         ��־��          ����
====================================================================*/
void CMcuVcInst::UpdateMontiorOnSMcuViewMtChg( const TMt& tNewSMcuViewMt)
{
	TMonitorData   tMonitorData;
	TSwitchDstInfo tSwitchDstInfo;
	TMt tMcVideoSrc;
	TMt tMcAudioSrc;
	TLogicalChannel tLogicalChannel;
	u8 byChannelNum;
	for(u8 byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( !g_cMcuVcApp.IsMcConnected( byIndex ) )
		{
			continue;
		}
		//all mc update
		//update video
		g_cMcuVcApp.GetMcLogicChnnl( byIndex, MODE_VIDEO, &byChannelNum, &tLogicalChannel ); 
		for (u8 byVidDstChnIdx = 0 ; byVidDstChnIdx < byChannelNum;byVidDstChnIdx++)
		{
			tMcVideoSrc.SetNull();
			g_cMcuVcApp.GetMcSrc( byIndex, &tMcVideoSrc, byVidDstChnIdx, MODE_VIDEO );
			
			if ( !tMcVideoSrc.IsNull() &&  !IsLocalAndSMcuSupMultSpy( tNewSMcuViewMt.GetMcuId() ) 
				&&	( tNewSMcuViewMt.GetMcuId() == tMcVideoSrc.GetMcuId() || IsMtInMcu( tMcVideoSrc,tNewSMcuViewMt ) )
				)
			{
				g_cMcuVcApp.SetMcSrc( byIndex, &tNewSMcuViewMt, byVidDstChnIdx, MODE_VIDEO );
			}
			
		
		}
		//update audio
		g_cMcuVcApp.GetMcLogicChnnl( byIndex, MODE_AUDIO, &byChannelNum, &tLogicalChannel );
		for (u8 byAudDstChnIdx = 0 ; byAudDstChnIdx < byChannelNum; byAudDstChnIdx++)
		{
			tMcAudioSrc.SetNull();
			g_cMcuVcApp.GetMcSrc( byIndex, &tMcAudioSrc, byAudDstChnIdx, MODE_AUDIO );
			//update audio
			if ( !tMcAudioSrc.IsNull() &&  !IsLocalAndSMcuSupMultSpy( tNewSMcuViewMt.GetMcuId() ) 
				&&	( tNewSMcuViewMt.GetMcuId() == tMcAudioSrc.GetMcuId() || IsMtInMcu( tMcAudioSrc,tNewSMcuViewMt ) )
				)
			{
				g_cMcuVcApp.SetMcSrc( byIndex, &tNewSMcuViewMt, byAudDstChnIdx, MODE_AUDIO );
			}
		}
		
		//all monitor update
		for( u16 wLoop = 0; wLoop < MAXNUM_MONITOR_NUM; wLoop ++ )
		{
			if( !g_cMcuVcApp.GetMonitorData(byIndex, wLoop, tMonitorData) )
			{
				continue;
			}
			if (tNewSMcuViewMt.GetConfIdx() == tMonitorData.GetMonitorSrc().GetConfIdx() &&
				!(tMonitorData.GetMonitorSrc() == tNewSMcuViewMt) &&
				!tMonitorData.GetMonitorSrc().IsMcuIdLocal() && 
				!IsLocalAndSMcuSupMultSpy(GetFstMcuIdFromMcuIdx(tMonitorData.GetMonitorSrc().GetMcuId())) &&
				GetFstMcuIdFromMcuIdx(tMonitorData.GetMonitorSrc().GetMcuId()) == GetFstMcuIdFromMcuIdx(tNewSMcuViewMt.GetMcuId()))
			{
				g_cMcuVcApp.SetMonitorSrc( byIndex, tMonitorData.GetMode(), tMonitorData.GetDstAddr(), tNewSMcuViewMt );
				tSwitchDstInfo.m_tSrcMt = tNewSMcuViewMt;
				tSwitchDstInfo.m_byMode = tMonitorData.GetMode();
				tSwitchDstInfo.m_bySrcChnnl = 0;
				if (tMonitorData.GetMode() == MODE_AUDIO)
				{
					tSwitchDstInfo.m_tDstAudAddr = tMonitorData.m_DstAddr;
				}
				else if (tMonitorData.GetMode() == MODE_VIDEO)
				{
					tSwitchDstInfo.m_tDstVidAddr = tMonitorData.m_DstAddr;
				}
				else
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[UpdateMontiorOnSMcuViewMtChg]Unknown Mode!\n");
					continue;
				}
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,"[NotifyUIStartMonitor]McIdx(%d) MonChnIdx(%d) Mt(%d,%d) mode(%d) DstIpAndPort(%s, %d)\n",
					byIndex,wLoop,tSwitchDstInfo.m_tSrcMt.GetMcuId(),tSwitchDstInfo.m_tSrcMt.GetMtId(),
					tSwitchDstInfo.m_byMode,StrOfIP(tMonitorData.m_DstAddr.GetIpAddr()),tMonitorData.m_DstAddr.GetPort());
				NotifyUIStartMonitor(tSwitchDstInfo, byIndex);
			}
		}
	}
}
/*====================================================================
������      OnNtfDsMtStatusToMMcu
����        ����������ʵ˫���ն�״̬�����ϼ�
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ����
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/07/27  4.7         ��־��          ����
====================================================================*/
void CMcuVcInst::OnNtfDsMtStatusToMMcu()
{
	if( m_tCascadeMMCU.IsNull() || m_tDoubleStreamSrc.IsNull() )
	{
		return;
	}

	if( m_tDoubleStreamSrc == m_tCascadeMMCU)
	{
		return;
	}
	OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(), m_tDoubleStreamSrc.GetMtId());
	TMt tRealDsMt = GetConfRealDsMt();
	if(tRealDsMt.IsNull())
	{
		return;
	}
	if (!tRealDsMt.IsLocal())
	{
		TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tDoubleStreamSrc.GetMtId()) );
		if(ptMcInfo == NULL)
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT2, "[OnNtfDsMtStatusToMMcu]ptMcInfo == NULL,So Return!\n");
			return;
		}
		CServMsg cServMsg;
		TSMcuMtStatus tSMcuMtStatus;
		u8 byMtNum = 0;
		for( u8 byIndex = 0; byIndex < MAXNUM_CONF_MT; byIndex++ )
		{	
			if ( tRealDsMt == (TMt)(ptMcInfo->m_atMtStatus[byIndex]))
			{
				memset( &tSMcuMtStatus,0,sizeof(tSMcuMtStatus) );
				memcpy( (void*)&tSMcuMtStatus, (void*)&tRealDsMt, sizeof(TMt) );
				tSMcuMtStatus.SetMcuId( m_tDoubleStreamSrc.GetMtId() );
				tSMcuMtStatus.SetIsEnableFECC( ptMcInfo->m_atMtStatus[byIndex].IsEnableFECC() );
				tSMcuMtStatus.SetCurVideo( ptMcInfo->m_atMtStatus[byIndex].GetCurVideo() );
				tSMcuMtStatus.SetCurAudio( ptMcInfo->m_atMtStatus[byIndex].GetCurAudio() );
				tSMcuMtStatus.SetMtBoardType( ptMcInfo->m_atMtStatus[byIndex].GetMtBoardType() );
				tSMcuMtStatus.SetIsMixing( ptMcInfo->m_atMtStatus[byIndex].IsInMixing() );
				tSMcuMtStatus.SetVideoLose( ptMcInfo->m_atMtStatus[byIndex].IsVideoLose() );
				BOOL32 bAutoCallMode = (ptMcInfo->m_atMtExt[byIndex].GetCallMode() == CONF_CALLMODE_TIMER ) ? TRUE : FALSE;
				tSMcuMtStatus.SetIsAutoCallMode( bAutoCallMode );
				tSMcuMtStatus.SetRecvVideo( ptMcInfo->m_atMtStatus[byIndex].IsReceiveVideo() );
				tSMcuMtStatus.SetRecvAudio( ptMcInfo->m_atMtStatus[byIndex].IsReceiveAudio() );
				tSMcuMtStatus.SetSendVideo( ptMcInfo->m_atMtStatus[byIndex].IsSendVideo() );
				tSMcuMtStatus.SetSendAudio( ptMcInfo->m_atMtStatus[byIndex].IsSendAudio() );
				tSMcuMtStatus.SetDisconnectReason( ptMcInfo->m_atMtStatus[byIndex].GetMtDisconnectReason() );
				tSMcuMtStatus.SetSendVideo2(ptMcInfo->m_atMtStatus[byIndex].IsSndVideo2());
				byMtNum = 1;
				cServMsg.SetMsgBody( (u8*)&byMtNum,sizeof(u8) );
				cServMsg.CatMsgBody( (u8*)&tSMcuMtStatus,sizeof(tSMcuMtStatus) );
				cServMsg.SetEventId( MCU_MCU_MTSTATUS_NOTIF );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[OnNtfDsMtStatusToMMcu]DSmt(%d,%d) mtstatus isSndVideo2(%d) to mmcu\n",
					tSMcuMtStatus.GetMcuIdx(),tSMcuMtStatus.GetMtId(),tSMcuMtStatus.IsSendVideo2());
				SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_MTSTATUS_NOTIF, cServMsg );
				break;
			}		
		}
	}
}

/*====================================================================
������      IsRosterRecved
����        ���Ƿ��յ�ĳ�ϼ����¼�mcu�Ļ������ϱ�
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ����
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/11/21  4.7         �ܾ���          ����
====================================================================*/
BOOL32 CMcuVcInst::IsRosterRecved( const u16 wMcuIdx )
{
	if( INVALID_MCUIDX == wMcuIdx )
	{
		return FALSE;
	}

	TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo( wMcuIdx );	
	return tConfMtInfo.IsMtExists();
}

// END OF FILE

