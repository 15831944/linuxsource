/*****************************************************************************
   ģ����      : mcu_new
   �ļ���      : mcuvcmcs.cpp
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
#include "evmcudcs.h"
#include "evmp.h"
#include "mcuvc.h"
#include "mcsssn.h"
#include "eqpssn.h"
//#include "mcuerrcode.h"
#include "mtadpssn.h"
#include "commonlib.h"
//#include "mcuutility.h"
//#include "mpmanager.h"
//#include "radiusinterface.h"

#ifdef _VXWORKS_
    #include "brddrvLib.h"
    #include <inetLib.h>
#endif

// MPC2 ֧��
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif


/*====================================================================
    ������      ��DaemonProcMcuMcsConnectedNotif
    ����        �����ע��ɹ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/18    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::DaemonProcMcuMcsConnectedNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8		byInstId = *( u8 * )cServMsg.GetMsgBody();
	TMcsRegInfo	tRegReq = *( TMcsRegInfo * )( cServMsg.GetMsgBody() + sizeof( u8 ) );
	TLogicalChannel tLogicalChannel;
    
	//����ǰ����Ƶͨ��
	tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( tRegReq.GetMcsIpAddr() );
	tLogicalChannel.m_tRcvMediaChannel.SetPort( tRegReq.GetStartPort() );
	tLogicalChannel.m_tRcvMediaCtrlChannel.SetIpAddr( tRegReq.GetMcsIpAddr());
	tLogicalChannel.m_tRcvMediaCtrlChannel.SetPort( tRegReq.GetStartPort() + 1 );
    tLogicalChannel.m_tSndMediaCtrlChannel.SetIpAddr( 0 );	//��0����ʱ����
	tLogicalChannel.m_tSndMediaCtrlChannel.SetPort( 0 );
	g_cMcuVcApp.SetMcLogicChnnl( byInstId, MODE_VIDEO, tRegReq.GetVideoChnNum(), &tLogicalChannel );
    
	//����ǰ����Ƶͨ��
	tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( tRegReq.GetMcsIpAddr() );
	tLogicalChannel.m_tRcvMediaChannel.SetPort( tRegReq.GetStartPort() + 2 );
	tLogicalChannel.m_tRcvMediaCtrlChannel.SetIpAddr( tRegReq.GetMcsIpAddr());
	tLogicalChannel.m_tRcvMediaCtrlChannel.SetPort( tRegReq.GetStartPort() + 3 );
    tLogicalChannel.m_tSndMediaCtrlChannel.SetIpAddr( 0 );	//��0����ʱ����
	tLogicalChannel.m_tSndMediaCtrlChannel.SetPort( 0 );
	g_cMcuVcApp.SetMcLogicChnnl( byInstId, MODE_AUDIO, tRegReq.GetVideoChnNum(), &tLogicalChannel );

	//����ǰ��˫��ͨ��
	tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( tRegReq.GetMcsIpAddr() );
	tLogicalChannel.m_tRcvMediaChannel.SetPort( tRegReq.GetStartPort() + 4 );
	tLogicalChannel.m_tRcvMediaCtrlChannel.SetIpAddr( tRegReq.GetMcsIpAddr());
	tLogicalChannel.m_tRcvMediaCtrlChannel.SetPort( tRegReq.GetStartPort() + 5 );
    tLogicalChannel.m_tSndMediaCtrlChannel.SetIpAddr( 0 );	//��0����ʱ����
	tLogicalChannel.m_tSndMediaCtrlChannel.SetPort( 0 );
	g_cMcuVcApp.SetMcLogicChnnl( byInstId, MODE_SECVIDEO, tRegReq.GetVideoChnNum(), &tLogicalChannel );

	//save info
	g_cMcuVcApp.SetMcConnected( byInstId, TRUE );
	g_cMcuVcApp.SetMcsRegInfo( byInstId, tRegReq );
    // ���� ��������MCS��IP��Ϣ  [04/24/2012 liaokang]
	if( byInstId <= MAXNUM_MCU_MC )
	{
		g_cMcuAgent.AddRegedMcsIp( tRegReq.GetMcsIpAddr() );
	}
}


/*====================================================================
    ������      ��DaemonProcMcuMcsDisconnectedNotif
    ����        ����ض���֪ͨ���� 
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/18    1.0         LI Yi         ����
	03/11/22    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::DaemonProcMcuMcsDisconnectedNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8	byInstId = *( u8 * )cServMsg.GetMsgBody();

	//�������л���ı���״̬
	TConfInfo *ptConfInfo;
	TConfProtectInfo *ptConfProtectInfo;

    CServMsg cLockMsg;
    TMcu tMcu;
    tMcu.SetMcu(0);
    u8 byLock = 0;
    cLockMsg.SetEventId(MCU_MCU_LOCK_REQ);
    cLockMsg.SetMsgBody((u8*)&tMcu, sizeof(tMcu));
    cLockMsg.CatMsgBody(&byLock, sizeof(byLock));
    CMcuVcInst *pConfInst = NULL;

	for( u8 byLoop = MIN_CONFIDX; byLoop <= MAX_CONFIDX; byLoop++ )
	{
        pConfInst = g_cMcuVcApp.GetConfInstHandle( byLoop );
        if(NULL == pConfInst)
        {
            continue;
        }

		ptConfInfo = &pConfInst->m_tConf;
        if( ptConfInfo->m_tStatus.IsOngoing())
		{
			 ptConfProtectInfo = g_cMcuVcApp.GetConfProtectInfo( byLoop );

		     if( ptConfInfo->m_tStatus.GetProtectMode() == CONF_LOCKMODE_LOCK && 
				 ptConfProtectInfo->GetLockedMcSsnId()== byInstId )
			 {
				 //�������
                 ptConfInfo->m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );
				 ptConfProtectInfo->SetLockByMcs(0);
                 
                 // guzh [7/25/2006] ͬʱ�������MCS�ѻ�������Ϊ����ϯģʽû���ͷţ���ȡ������ϯģʽ
                 if (ptConfInfo->m_tStatus.m_tConfMode.IsNoChairMode())
                 {
                    ptConfInfo->m_tStatus.SetNoChairMode(FALSE);
                 }
                 
                 // xsl [7/26/2006] ����¼�mcu����     
                 cLockMsg.SetConfId(ptConfInfo->GetConfId());
                 g_cMcuVcApp.SendMsgToConf(pConfInst->m_byConfIdx, MCS_MCU_LOCKSMCU_REQ, 
                                           cLockMsg.GetServMsg(), cLockMsg.GetServMsgLen());

                 // zbq [11/22/2007] �������ģʽ
				 // miaoqingsong [20120111] �����MCS�ѻ�������Ϊ����ҵ��ģʽ������ʱ��ȡ���������ҵ��
				 //                 ���������ģʽ����ȡ���������ҵ��ʱ������������ע����
                 if ( ROLLCALL_MODE_NONE != ptConfInfo->m_tStatus.GetRollCallMode() )
                 {
					 CServMsg cStopRollMsg;
					 cStopRollMsg.SetEventId(MCS_MCU_STOPROLLCALL_REQ);
					 cStopRollMsg.SetConfId(ptConfInfo->GetConfId());
					 
					 g_cMcuVcApp.SendMsgToConf( pConfInst->m_byConfIdx, MCS_MCU_STOPROLLCALL_REQ, 
					     cStopRollMsg.GetServMsg(), cStopRollMsg.GetServMsgLen() );
                     //ptConfInfo->m_tStatus.SetRollCallMode(ROLLCALL_MODE_NONE);
                 }
			 }
			 
             if( ptConfInfo->m_tStatus.GetProtectMode() == CONF_LOCKMODE_NEEDPWD )
			 {
				 //�´ε�¼��������������
                 ptConfProtectInfo->SetMcsPwdPassed(byInstId, FALSE);
			 }
		}
	}

#if defined(_8KH_) ||  defined(_8KE_) || defined(_8KI_)
	//��ض���ʱ�����Ƿ�ǰ����ץ����mcs�������ǵĻ�ֹͣץ������ץ��״̬
	emNetCapState emCurNetCapState = g_cMcuVcApp.GetNetCapStatus();
	//У��״̬
	if (emCurNetCapState == emNetCapStart && g_cMcuVcApp.GetOprNetCapMcsInstID() == byInstId )
	{
		//ֹͣץ��
		ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_MCS,"[DaemonProcMcuMcsDisconnectedNotif]stop netcap!\n");
		g_cMcuVcApp.StopNetCap();
		//״̬֪ͨ
		g_cMcuVcApp.SetNetCapStatus(emNetCapIdle);
		u8 byCurState = (u8)emNetCapIdle;
		cServMsg.SetMsgBody(&byCurState,sizeof(u8));
		SendMsgToAllMcs(MCU_MCS_NETCAPSTATUS_NOTIFY,cServMsg);

	}

	if( g_cMcsSsnApp.GetCurrentDebugMode() != emDebugModeNone && 
		g_cMcsSsnApp.GetCurrentDebugModeInsId() == byInstId )
	{
		g_cMcsSsnApp.SetCurrentDebugMode( emDebugModeNone );
		g_cMcsSsnApp.SetCurrentDebugModeInsId( 0 );
	#ifdef _LINUX_
		SetTelnetDown();
		SetSshDown();
	#endif
		u8 byCurMode = (u8)emDebugModeNone;
		cServMsg.SetMsgBody(&byCurMode,sizeof(u8));		
		SendMsgToAllMcs(MCU_MCS_OPENDEBUGMOED_NOTIFY,cServMsg);
	}
#endif

	//stop switch to MC
	g_cMcuVcApp.BroadcastToAllConf( MCU_MCSDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
	
	//clear info
	g_cMcuVcApp.SetMcConnected( byInstId, FALSE );
    // ���� ��������MCS��IP��Ϣ  [04/24/2012 liaokang]
    TMcsRegInfo tMcsRegInfo;
    g_cMcuVcApp.GetMcsRegInfo( byInstId, &tMcsRegInfo );
    g_cMcuAgent.DeleteRegedMcsIp( tMcsRegInfo.GetMcsIpAddr() );
}

/*====================================================================
    ������      ��ProcMcsMcuCreateConfReq
    ����        ���������鴦���� 
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/01    1.0         LI Yi         ����
	04/06/03    3.0         ������        �޸�
	05/12/20	4.0			�ű���		  ����T120
====================================================================*/
void CMcuVcInst::ProcMcsMcuCreateConfReq( const CMessage * pcMsg )
{
	TConfMode  tConfMode;
    u8 byMtNum = 0;
    u8 byCreateMtId;
    u8 byOldInsId = 0;
    u16 wErrCode = 0;

    CServMsg  cServMsg( pcMsg->content, pcMsg->length );

    TConfInfo tTmpConf = *(TConfInfo *)cServMsg.GetMsgBody();
	
    BOOL32 bConfFromFile = tTmpConf.m_tStatus.IsTakeFromFile();
    tTmpConf.m_tStatus.SetTakeFromFile(FALSE);

	//  [12/29/2009 pengjie] Modify AES���ܻ��飬����������ʴ���4M����������Ϊ4M������ԭ��
	if( tTmpConf.GetBitRate() > (4096 - GetAudioBitrate(tTmpConf.GetMainAudioMediaType())) && 
		tTmpConf.GetMediaKey().GetEncryptMode() == CONF_ENCRYPTMODE_AES )
	{
		tTmpConf.SetBitRate(4096 - GetAudioBitrate(tTmpConf.GetMainAudioMediaType()));
	}
	if( tTmpConf.GetSecBitRate() >= (4096 - GetAudioBitrate(tTmpConf.GetSecAudioMediaType())) && 
		tTmpConf.GetMediaKey().GetEncryptMode() == CONF_ENCRYPTMODE_AES )
	{
		tTmpConf.SetSecBitRate(0);
	}
	// End Modify  


	//�Ƚ���tConfInfoEx��Ϣ��BAS����Դ�Ƿ��㹻���õ�[12/29/2011 chendaiwei]
	TConfInfoEx tConfInfoEx;
	if(GetConfExInfoFromMsg(cServMsg,tConfInfoEx))
	{
		tTmpConf.SetHasConfExFlag(TRUE);
	}
			
	

	switch( CurState() )
	{
	case STATE_WAITFOR:
		// ����Mcu֧�ֵ����������ͻ���������������
		{                        
			TEqpCapacity tConfEqpCap;
			TEqpCapacity tMcuEqpCap;			
			u8  byConfIdx = cServMsg.GetConfIdx();
			
			g_cMcuVcApp.GetConfEqpDemand( tTmpConf, tConfEqpCap);
			if( tConfEqpCap.m_tTvCap.IsNeedEqp() ) // ����ǽ����Ŀ
			{
				u16 wAliasLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
				s8 *pMsgBuf = (s8*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16) + wAliasLen);
				TMultiTvWallModule *ptMultiTvWallModule = (TMultiTvWallModule *)pMsgBuf;
				tConfEqpCap.m_tTvCap.SetNeedChannles( ptMultiTvWallModule->GetTvModuleNum() );// ����ǽ��Ŀ
			}

			g_cMcuVcApp.GetMcuEqpCapacity( tMcuEqpCap );
			BOOL32 bRet = g_cMcuVcApp.AnalyEqpCapacity( tTmpConf/*m_tConf*/,tConfInfoEx, tConfEqpCap, tMcuEqpCap );            

			//����ԤԼ���飬����Ҫ�Ƚ�������Դ�Ƿ��㹻������ֱ�ӿ���������������ʱ���ٱȽ�
			if ( !tTmpConf.m_tStatus.IsOngoing())
			{
				bRet = TRUE;
			}

            // [12/28/2006-zbq] N+1���ݻع���ָ��Ļ��飬��Ϊ�����Ѿ�ͨ����ֱ�Ӵ���
			//zjj20121023 Bug00110525 N+1���ݻع���ָ��Ļ��������޸ģ�ԭ����ֱ�����Ƿ�MCU_NPLUS_IDLE̫�ֲ�
			if( !bRet && CONF_CREATE_NPLUS != cServMsg.GetSrcMtId()/*MCU_NPLUS_IDLE == g_cNPlusApp.GetLocalNPlusState()*/ )
			{
				// ���ö�ʱ��ȥ���¿���		
				if( bConfFromFile )
				{
                    // ��������Դ���
					if( MAX_TIMES_CREATE_CONF < m_wRestoreTimes ) 
					{
						if (!g_cMcuVcApp.RemoveConfFromFile(tTmpConf.GetConfId()))
						{
							ConfPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMcsMcuCreateConfReq] remove conf from file failed\n");
						}
                        else
                        {
                            ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "[ProcMcsMcuCreateConfReq] conf.%s has been remove from file due to RestoreTimes.%d\n", tTmpConf.GetConfName(), m_wRestoreTimes);
                        }
						g_cMcuVcApp.SetConfStore(byConfIdx-MIN_CONFIDX, FALSE); // �Ժ��ٻָ�
						NEXTSTATE(STATE_IDLE);
                        m_wRestoreTimes = 1;
					}
					else
					{
                        SetTimer(MCU_WAITEQP_CREATE_CONF_NOTIFY, m_wRestoreTimes <= 3 ? 10000 : m_wRestoreTimes*LEN_WAIT_TIME, (u32)byConfIdx );// ��ʱ
                        m_wRestoreTimes++;
					}					
				}				
				
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Conference %s create failed because NO enough equipment(%d)!\n", tTmpConf.GetConfName(), m_wRestoreTimes );
				return;
			}
			else
			{
				if( bConfFromFile )
				{
                    // guzh [4/10/2007] ������ʱ���ܴ��ļ���ɾ��������ᵼ���ļ���ʧ
                    /*
					// ���ļ����Ƴ�
					if (!g_cMcuVcApp.RemoveConfFromFile(tTmpConf.GetConfId()))
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMcsMcuCreateConfReq] remove conf from file failed\n");
					}
                    */
					g_cMcuVcApp.SetConfStore(byConfIdx-MIN_CONFIDX, FALSE);// �Ժ��ٻָ�

					NEXTSTATE(STATE_IDLE);

					if ( g_cMSSsnApp.IsDoubleLink() )
					{
						//�ָ������ʱ��֪ͨ�������ݻָ��Ļ���
						ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"conf:%s Set RecoverTimer:MCUVC_RECOVERCONF_SYN_TIMER\n",tTmpConf.GetConfName());
						CServMsg cMsgToDeamon;
						u8 byStart = 1;//����deamoninstance����timer,ֱ�Ӹ�Msmanager���Ļ����ܻᵼ�ºܶ�ʱ���ڶ��ͬ��
						cMsgToDeamon.SetMsgBody((u8*)&byStart,sizeof(byStart));
						post(MAKEIID(GetAppID(),CInstance::DAEMON),MCUVC_RECOVERCONF_SYN_TIMER,cMsgToDeamon.GetServMsg(),cMsgToDeamon.GetServMsgLen());
					}
									
                    m_wRestoreTimes = 1;

                    KillTimer(MCU_WAITEQP_CREATE_CONF_NOTIFY);
				
				}
			}
		}  
		//lint -fallthrough
		// �˴�û��Break����Ϊ��Mcu֧�ֻ������������ʱ������ִ������ĳ���������
		
	case STATE_IDLE:
		{
			//���ʵ��
			ClearVcInst();
			cServMsg.SetErrorCode(0);       
			
			//��������ԤԼ���飬����StartTime�ж���һ��ԤԼ���黹�Ǽ�ʱ����
			//������ԤԼ���鴦����ں�����
			if ( !tTmpConf.IsCircleScheduleConf() )
			{
				//���ÿ�ʼʱ��
				if( 0 == tTmpConf.GetStartTime() ||
					MT_MCU_CREATECONF_REQ == pcMsg->event )
				{
					tTmpConf.m_tStatus.SetOngoing();
					tTmpConf.SetStartTime( time( NULL ) );
				}
				
				// zgc [5/18/2007] �ж�ԤԼ���鿪ʼʱ��
				if( tTmpConf.GetStartTime() <= time(NULL) + 1*60 )
				{
					tTmpConf.m_tStatus.SetOngoing();
				}
			}
			
			// guzh [4/16/2007] �жϼ�ʱ���鿪ʼʱ��
			if ( tTmpConf.m_tStatus.IsOngoing() &&
				tTmpConf.GetStartTime() > time(NULL)+1*60 &&
				// zbq [08/02/2007] N+1���ݻָ��Ļ���ֱ���ٿ�
				CONF_CREATE_NPLUS != cServMsg.GetSrcMtId() )
			{
				cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );            
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s failure because start time error!\n", tTmpConf.GetConfName() );
				return;
			}
			
			// �Ѵ���������������ʱ�������Ƿ���
			if ( g_cMcuVcApp.IsConfNumOverCap(tTmpConf.m_tStatus.IsOngoing(), tTmpConf.GetConfSource()) )
			{
				// guzh [6/18/2007] �ϼ�MCU���У�����Ϊ��������ʱ����������֪ͨ����MCS
				/*if (1 == g_cMcuVcApp.GetMaxOngoingConfNum() &&
				cServMsg.GetSrcMtId() == CONF_CREATE_MT)
				{
				//�����ն��б�
				u16 wAliasBufLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
				char* pszAliaseBuf = (char*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16));        
				TMtAlias tMtAliasArray[MAXNUM_CONF_MT];
				u16 awMtDialRate[MAXNUM_CONF_MT];
				UnPackTMtAliasArray( pszAliaseBuf, wAliasBufLen, &tTmpConf, 
				tMtAliasArray, awMtDialRate, byMtNum );
				
				  CServMsg cNotifMsg;
				  u32 dwMMcuIp = tMtAliasArray[0].m_tTransportAddr.GetNetSeqIpAddr();
				  
					cNotifMsg.SetMsgBody((u8*)&dwMMcuIp, sizeof(u32));
					cNotifMsg.SetErrorCode( ERR_MCU_CONFNUM_EXCEED );
					SendMsgToAllMcs(MCU_MCS_MMCUCALLIN_NOTIF, cNotifMsg);              
					}
					else
				{*/
                cServMsg.SetErrorCode( ERR_MCU_CREATECONFERROR_CONFISHOLDING );
				//}
				//[2011/11/29/zhangli]����CreateConfSendMcsNack�ӿ���ʾ������ԤԼ�����޷���ʾ����ͬ
				CreateConfSendMcsNack(byOldInsId, ERR_MCU_CREATECONFERROR_CONFISHOLDING, cServMsg);
//				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				if ( bConfFromFile )
				{
					g_cMcuVcApp.RemoveConfFromFile(tTmpConf.GetConfId());
				}      
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s failure because exceed max conf num!\n", tTmpConf.GetConfName() );
				return;
			}
			
			if (0 == g_cMcuVcApp.GetMpNum() || 0 == g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323))
			{
				cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
//				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				CreateConfSendMcsNack(byOldInsId, ERR_MCU_CONFOVERFLOW, cServMsg);
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s failure because No_Mp or No_MtAdp!\n", tTmpConf.GetConfName() );
				return;
			}
			
			//�����ն�������
			if( NULL == m_ptMtTable )
			{
				m_ptMtTable = new TConfMtTable;
				START_CONF_CHECK_MEMORY(m_ptMtTable, cServMsg, tTmpConf);
				m_tApplySpeakQue.Init();
			}
			memset( m_ptMtTable, 0, sizeof( TConfMtTable ) );
// 			 
 			//����bas������������
// 			if ( NULL == m_pcMcuBasMgr )
// 			{
// 				m_pcMcuBasMgr = new CMcuBasMgr;
// 			}
			NewMcuBasMgr();
			ClearBasMgr();
			//        memset( m_pcMcuBasMgr, 0, sizeof( CMcuBasMgr ) );
			
			m_dwSpeakerViewId  = OspTickGet();//102062000;//
			m_dwSpeakerVideoId = m_dwSpeakerViewId+100;//102052024;//
			m_dwVmpViewId      = m_dwSpeakerVideoId+100;
			m_dwVmpVideoId     = m_dwVmpViewId+100;
			m_dwSpeakerAudioId = m_dwVmpVideoId+100;
			m_dwMixerAudioId   = m_dwSpeakerAudioId+100;
			
			//�õ�������
			m_byCreateBy = cServMsg.GetSrcMtId();
			
			m_byConfIdx = g_cMcuVcApp.GetTemConfIdxByE164(tTmpConf.GetConfE164());      
			//ԤԼ��������Ϊģ�壬confidx���Ǵӿ���idx��ȡһ��
			if( 0 == m_byConfIdx || !tTmpConf.m_tStatus.IsOngoing() /*|| tTmpConf.IsCircleScheduleConf() */)
			{
				m_byConfIdx = g_cMcuVcApp.GetIdleConfidx(); 
				if(0 == m_byConfIdx)
				{                      
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s failure for conf full!\n", tTmpConf.GetConfName() );
					return;
				}
			}           
			byOldInsId = g_cMcuVcApp.GetConfMapInsId(m_byConfIdx);
			g_cMcuVcApp.SetConfMapInsId(m_byConfIdx, (u8)GetInsID());
			
			//[chendaiwei 2010/10/17]�����NPlus������˵�������״̬,
			//ͨ�����û���ģ�����ܻ���������֧�����ܻ��������Ƽ�VAC����ʱ��֧��
			if ( CONF_CREATE_NPLUS == m_byCreateBy )
			{
				TConfAttrb tAttr = tTmpConf.GetConfAttrb();
				if(tTmpConf.m_tStatus.GetMixerMode() == mcuWholeMix
				    || tTmpConf.m_tStatus.GetMixerMode() == mcuVacWholeMix)
				{
					tAttr.SetDiscussConf(TRUE);			
				}
				else
				{
					tAttr.SetDiscussConf(FALSE);
				}
				
				tTmpConf.m_tStatus.SetMixerMode(mcuNoMix);
				tTmpConf.SetConfAttrb(tAttr);
			}
			//�õ�������Ϣ
			
			m_tConf = tTmpConf;    
			m_tConfEx = tConfInfoEx;
			
			//zbq[12/04/2007] ������ģ�����֡�ʶ���
			if ( MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
				!m_tConf.IsMainVidUsrDefFPS() )
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[CreateConf] MVidType.%d with FPS.%d is unexpected, adjust it\n",
					m_tConf.GetMainVideoMediaType(), m_tConf.GetMainVidUsrDefFPS() );
				
				//������һ����ͨ����֡
				if ( 0 == m_tConf.GetMainVidUsrDefFPS() )
				{
					m_tConf.SetMainVidUsrDefFPS(30);
				}
				else
				{
					m_tConf.SetMainVidUsrDefFPS(m_tConf.GetMainVidUsrDefFPS());
				}
			}
			if ( MEDIA_TYPE_H264 == m_tConf.GetSecVideoMediaType() &&
				!m_tConf.IsSecVidUsrDefFPS() )
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[CreateConf] SVidType.%d with FPS.%d is unexpected, adjust it\n",
					m_tConf.GetSecVideoMediaType(), m_tConf.GetSecVidUsrDefFPS() );
				
				//������һ����ͨ����֡
				if ( 0 == m_tConf.GetSecVidUsrDefFPS() )
				{
					m_tConf.SetSecVidUsrDefFPS(30);
				}
				else
				{
					m_tConf.SetSecVidUsrDefFPS(m_tConf.GetSecVidUsrDefFPS());
				}
			}
			if ( MEDIA_TYPE_H264 == m_tConf.GetCapSupport().GetDStreamMediaType() &&
				!m_tConf.IsDStreamUsrDefFPS() )
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[CreateConf] DSVidType.%d with FPS.%d is unexpected, adjust it\n",
					m_tConf.GetCapSupport().GetDStreamMediaType(), m_tConf.GetDStreamUsrDefFPS() );
				
				//������һ����ͨ��1/3��֡
				if ( 0 == m_tConf.GetDStreamUsrDefFPS() )
				{
					m_tConf.SetDStreamUsrDefFPS(10);
				}
				else
				{
					m_tConf.SetDStreamUsrDefFPS(m_tConf.GetDStreamUsrDefFPS());
				}
			}
			
			//�������ļ����Ƿ��������֡������
			//��Aethra��ͨʱ����ؽ�FPS���ó�10�����޸�mcuGeneralParam/ConfFPS��
			u8 byDbgFps = g_cMcuVcApp.GetDbgConfFPS();
			if ( byDbgFps != 0 )
			{
				m_tConf.SetMainVidUsrDefFPS(byDbgFps);
			}
			
			//zbq[11/15/2008] debug���ӵڶ�˫������
			if (MEDIA_TYPE_NULL == m_tConf.GetCapSupportEx().GetSecDSType())
			{
				AddSecDSCapByDebug(m_tConf);
			}
			
			tConfMode = m_tConf.m_tStatus.GetConfMode();
			if(0 == g_cMcuAgent.GetGkIpAddr())
			{
				tConfMode.SetRegToGK(FALSE);
			}
			
			if( m_tConf.m_tStatus.GetCallInterval() < MIN_CONF_CALLINTERVAL )
			{
				tConfMode.SetCallInterval( MIN_CONF_CALLINTERVAL );
			}
			else
			{
				tConfMode.SetCallInterval( m_tConf.m_tStatus.GetCallInterval() );
			}
			if( DEFAULT_CONF_CALLTIMES == m_tConf.m_tStatus.GetCallTimes() )
			{
				tConfMode.SetCallTimes( DEFAULT_CONF_CALLTIMES );
			}
			else if( m_tConf.m_tStatus.GetCallTimes() < MIN_CONF_CALLTIMES )
			{
				tConfMode.SetCallTimes( MIN_CONF_CALLTIMES );
			}
			else
			{
				tConfMode.SetCallTimes( m_tConf.m_tStatus.GetCallTimes() );
			}		
			m_tConf.m_tStatus.SetConfMode( tConfMode );
			
			m_tConf.m_tStatus.SetPrsing( FALSE );
			m_tConfAllMtInfo.Clear();
			m_tConfAllMtInfo.m_tLocalMtInfo.SetConfIdx( m_byConfIdx );
			m_tConfAllMcuInfo.Clear();
			
			// guzh [4/10/2007] ������Ǵӻ���ָ����ļ�������������CConfId��
			// �������þɵģ��������ܸ���ԭ���Ļ�����Ϣ�ļ�����Ψһ����CConfId
			if ( !bConfFromFile )
			{
				m_tConf.SetConfId( g_cMcuVcApp.MakeConfId(m_byConfIdx, 0, m_tConf.GetUsrGrpId(),
					m_tConf.GetConfSource()) );
			}
			
			//������Ϣ �߼����
			if ( !IsConfInfoCheckPass(cServMsg, m_tConf, wErrCode))
			{
				CreateConfSendMcsNack( byOldInsId, wErrCode, cServMsg );
				return;
			}
			//������������
			ConfDataCoordinate(cServMsg, byMtNum, byCreateMtId);


				
			
			//���������ģʽΪ���ն��Զ�������Ӧ�����ն�
			if(0 == byMtNum && m_tConf.GetConfAttrb().IsReleaseNoMt())
			{
				ClearHduSchemInfo();
				CreateConfSendMcsNack( byOldInsId, ERR_MCU_NOMTINCONF, cServMsg );
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because on mt in conf!\n", m_tConf.GetConfName() );
				return;
			}
			
			// N+1 ������Ϊ��֧��ԤԼ���� [12/20/2006-zbq]
			// ���鴦��: ��ʱ����
			if ( CONF_CREATE_NPLUS == m_byCreateBy ||
				( CONF_CREATE_NPLUS != m_byCreateBy && m_tConf.m_tStatus.IsOngoing() )  )
			{				
//				//���GKע����Ϣ����ʱģ�屾��ע��ɹ�
//				m_tConf.m_tStatus.SetRegToGK( FALSE );
				
				//MCU�����������Ƿ��֣��������������
				if ( !IsEqpInfoCheckPass(cServMsg, wErrCode))
				{
					ClearHduSchemInfo();
					CreateConfEqpInsufficientNack( byCreateMtId, byOldInsId, wErrCode, cServMsg );
					
					if (ERR_MCU_NOENOUGH_HDBAS_CONF == wErrCode)
					{
// 						TMcuHdBasStatus tStatus;
// 						m_pcBasMgr->GetHdBasStatus(tStatus, m_tConf);
// 						
// 						CServMsg cMsg;
// 						cMsg.SetMsgBody((u8*)&tStatus, sizeof(tStatus));
// 						cMsg.SetEventId(MCU_MCS_MAUSTATUS_NOTIFY);
// 						SendMsgToMcs(cServMsg.GetSrcSsnId(), MCU_MCS_MAUSTATUS_NOTIFY, cMsg);
					}
					
					return;
				}	

				NPlusVmpParamCoordinate(cServMsg);

				if( NULL == m_ptSwitchTable )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[instance%d]create confswitchtable\n", GetInsID());
					m_ptSwitchTable = new TConfSwitchTable;
					START_CONF_CHECK_MEMORY(m_ptSwitchTable, cServMsg, tTmpConf);
				}
				
				if( NULL == m_ptConfOtherMcTable )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[instance%d]create TConfOtherMcTable\n", GetInsID());
					m_ptConfOtherMcTable = new TConfOtherMcTable;
					START_CONF_CHECK_MEMORY(m_ptConfOtherMcTable, cServMsg, tTmpConf);
				}
				if (m_ptSwitchTable != NULL)
				{
					memset( m_ptSwitchTable, 0, sizeof( TConfSwitchTable ) );
				}
				
				if (m_ptConfOtherMcTable != NULL)
				{
					memset( m_ptConfOtherMcTable, 0, sizeof( TConfOtherMcTable ) );
					m_ptConfOtherMcTable->Init();
				}
				
				//���GKע����Ϣ����ʱģ�屾��ע��ɹ�
				m_tConf.m_tStatus.SetRegToGK( FALSE );
				
				TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();

                //pgf:���Ƿ�ɢ���鲻�ܿ����������������ܻ���
                if (!m_tConf.GetConfAttrb().IsSatDCastMode())
                {
                    if (tConfAttrib.IsDiscussConf())
                    {
                        // ���� [4/29/2006] ����������ȫ�����
                        //tianzhiyong 2010/04/15 ���ӿ���ģʽ
                        if(m_tConf.GetConfAttrbEx().IsSupportAutoVac())
                        {
                            StartMixing(mcuVacWholeMix);
                        }
                        else
                        {
                            StartMixing(mcuWholeMix);
                        }
                    }
                    else
                    {
                        if (m_tConf.GetConfAttrbEx().IsSupportAutoVac()) 
                        {
                            CServMsg cTempServMsg;				
                            cTempServMsg.SetEventId(MCS_MCU_STARTVAC_REQ);				
                            MixerVACReq(cTempServMsg);
                        }
				    }
                }
                else
                {
                    ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, 
                        "[ProcMcsMcuCreateConfReq]SatDCast mode not support Vac or WholeMix!\n");
                }

				
				//Ӧ�� 
				cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
				cServMsg.SetConfId( m_tConf.GetConfId() );
				if( m_byCreateBy == CONF_CREATE_MCS && !bConfFromFile)
				{
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
				}
				else if ( m_byCreateBy == CONF_CREATE_MT && !bConfFromFile )
				{
					// zbq [07/16/2007] �ն�/�ϼ�MCU����ɹ����˴��ſ��Ի�ACK
					CServMsg cAckMsg;
					cAckMsg.SetSrcDriId( cServMsg.GetSrcDriId() );
					
					TCapSupport tCapSupport = m_tConf.GetCapSupport();
					
					// FIXME: �˴������ڴ��������ն��б�ͷ�Ĳ���
					cAckMsg.SetDstMtId( 1 );
					cAckMsg.SetConfIdx( m_byConfIdx );
					cAckMsg.SetConfId( m_tConf.GetConfId() );
					cAckMsg.SetSrcSsnId( cServMsg.GetSrcSsnId() );
					u8 byIsEncrypt = GETBBYTE(tConfAttrib.GetEncryptMode() != CONF_ENCRYPTMODE_NONE);
					cAckMsg.SetMsgBody(&byIsEncrypt, sizeof(u8));
					cAckMsg.CatMsgBody((u8*)&tCapSupport, sizeof(tCapSupport));
					TMtAlias tAlias;
					tAlias.SetH323Alias(m_tConf.GetConfName());
					cAckMsg.CatMsgBody((u8 *)&tAlias, sizeof(tAlias));
					
					// Ex��������Ҫ��������Mtadp�࣬������ֱ���Mtadp��û�н������������µ�ʱ�򣨼���û��Ex����ʱ�����Զ�������������
					// ���±ȳ��Ĺ�ͬ˫����������
					TCapSupportEx tCapEx = m_tConf.GetCapSupportEx();
					cAckMsg.CatMsgBody( (u8*)&tCapEx, sizeof(tCapEx));
					
					// ��֯��չ��������ѡ��Mtadp [12/8/2011 chendaiwei]
					TVideoStreamCap atMSVideoCap[MAX_CONF_CAP_EX_NUM];
					u8 byCapNum = MAX_CONF_CAP_EX_NUM;
					m_tConfEx.GetMainStreamCapEx(atMSVideoCap,byCapNum);
					
					TVideoStreamCap atDSVideoCap[MAX_CONF_CAP_EX_NUM];
					u8 byDSCapNum = MAX_CONF_CAP_EX_NUM;
					m_tConfEx.GetDoubleStreamCapEx(atDSVideoCap,byDSCapNum);
					
					cAckMsg.CatMsgBody((u8*)&atMSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
					cAckMsg.CatMsgBody((u8*)&atDSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
					
					TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//��Ƶ����
					//�ӻ���������ȡ������֧�ֵ���Ƶ����
					m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
					cAckMsg.CatMsgBody((u8*)&atAudioTypeDesc[0], sizeof(TAudioTypeDesc)* MAXNUM_CONF_AUDIOTYPE);

					// guzh [7/19/2007] ���͸�������
					SendMsgToMt(CONF_CREATE_MT, MCU_MT_CREATECONF_ACK, cAckMsg);
				}
				
				//��ӡ��Ϣ 
				if ( CONF_DATAMODE_VAONLY == m_tConf.GetConfAttrb().GetDataMode() )
				{
					//��Ѷ����
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "conference %s created and started!\n",m_tConf.GetConfName());
				}
				else if ( CONF_DATAMODE_VAANDDATA == m_tConf.GetConfAttrb().GetDataMode() )
				{
					//��Ѷ������
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "conference %s created and started with data conf function !\n", m_tConf.GetConfName());
				}
				else
				{
					//���ݻ���
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "conference %s created and started with data conf function Only !\n", m_tConf.GetConfName());
				}
				
				//zbq[12/18/2008] �ֶ������ɹ����嶨ʱ�ȴ�
				KillTimer(MCU_WAITEQP_CREATE_CONF_NOTIFY);
				
				//���浽�ļ�(���ϼ�MCU�ĺ��д����Ļ��鲻����)��N+1����ģʽ�����浽�ļ�
				if ( CONF_CREATE_NPLUS == m_byCreateBy && MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcMcsMcuCreateConfReq] conf info don't save to file in N+1 restore mode.\n");
				}
				else
				{
					g_cMcuVcApp.SaveConfToFile( m_byConfIdx, FALSE );
				}
				
				//�����Զ�����ʱ��
				if( m_tConf.GetDuration() != 0 )
				{
					SetTimer( MCUVC_ONGOING_CHECK_TIMER, TIMESPACE_ONGOING_CHECK );
				}
				
				//���û��鱣��״̬
				if ( CONF_CREATE_MCS == m_byCreateBy && cServMsg.GetSrcSsnId() != 0)
				{
					if ( CONF_LOCKMODE_LOCK == m_tConf.m_tStatus.GetProtectMode() )
					{
						m_tConfProtectInfo.SetLockByMcs( cServMsg.GetSrcSsnId() );
					}
					else if( CONF_LOCKMODE_NEEDPWD == m_tConf.m_tStatus.GetProtectMode() )
					{
						m_tConfProtectInfo.SetMcsPwdPassed(cServMsg.GetSrcSsnId(), TRUE);
					}
				}
				
				//��DCS���𴴻�����
				if ( CONF_DATAMODE_VAANDDATA == m_tConf.GetConfAttrb().GetDataMode() ||
					CONF_DATAMODE_DATAONLY  == m_tConf.GetConfAttrb().GetDataMode() )
				{
					SendMcuDcsCreateConfReq();
				}
				
				//��GKע�����
				//m_byRegGKDriId = g_cMcuVcApp.GetRegGKDriId();
				BOOL32 bInviteMtNow = FALSE;
				
				if( !g_cMcuVcApp.IsTemRegGK(m_byConfIdx) && g_cMcuAgent.GetGkIpAddr() != 0 && 
					!m_tConf.m_tStatus.IsRegToGK()  && (0 != g_cMcuVcApp.GetRegGKDriId()) )
				{
					g_cMcuVcApp.RegisterConfToGK( m_byConfIdx, g_cMcuVcApp.GetRegGKDriId() );
					
					//ע��ɹ��������ն�
					memcpy(m_abySerHdr, cServMsg.GetServMsg(), sizeof(m_abySerHdr));
					m_tConfInStatus.SetInviteOnGkReged(TRUE);
					m_tConfInStatus.SetRegGkNackNtf(TRUE);
				}
				else 
				{	
					// ��������ն�
					if(0 != g_cMcuAgent.GetGkIpAddr())
					{
						m_tConf.m_tStatus.SetRegToGK(TRUE);
					}
					bInviteMtNow = TRUE;
				}
				
				// ��GK����ʼ����Ʒ� [11/09/2006-zbq]
				if ( m_tConf.IsSupportGkCharge() && g_cMcuVcApp.GetRegGKDriId() != 0 &&
					//zbq [03/26/2007] �Ʒ���·����ʧ�ܣ������ᣬ��������Ʒ�
					g_cMcuVcApp.GetChargeRegOK() )
				{               
					g_cMcuVcApp.ConfChargeByGK( m_byConfIdx, g_cMcuVcApp.GetRegGKDriId(), FALSE, m_byCreateBy, byMtNum );
				}
				else
				{
					if ( m_tConf.IsSupportGkCharge() ) 
					{
						ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[CreateConf] charge postponed due to GetRegGKDriId.%d, ChargeRegOK.%d !\n",
							g_cMcuVcApp.GetRegGKDriId(), g_cMcuVcApp.GetChargeRegOK() );
					}
				}
				
				// ���� [4/29/2006]��ԭ��֪ͨMCS�����ڱ��浽�ļ����档
				// �ƶ���������Ϊ�˱�֤Gk����Ϣ����ȷ�ġ�֪ͨ���л�ء�
				cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
				if(m_tConf.HasConfExInfo())
				{
					u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
					u16 wPackDataLen = 0;
					PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
					cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
				}

				SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
				
				// �����նˣ���������Ѿ����
				if (bInviteMtNow)                            
				{
					cServMsg.SetNoSrc();
					cServMsg.SetTimer( TIMESPACE_WAIT_AUTOINVITE );
					cServMsg.SetMsgBody( NULL, 0 );
					InviteUnjoinedMt( cServMsg );
				}
				
				//��Trap��Ϣ
				TConfNotify tConfNotify;
				CConfId cConfId;
				cConfId = m_tConf.GetConfId( );
				memcpy( tConfNotify.m_abyConfId, &cConfId, sizeof(cConfId) );
				astrncpy( tConfNotify.m_abyConfName, m_tConf.GetConfName(),
					sizeof(tConfNotify.m_abyConfName), MAXLEN_CONFNAME);
				SendTrapMsg( SVC_AGT_CONFERENCE_START, (u8*)&tConfNotify, sizeof(tConfNotify) );	
				
				//���ö�ʱ����ʱ��
				m_tRefreshParam.dwMcsRefreshInterval = g_cMcuVcApp.GetMcsRefreshInterval();
				
				//zjj20100419 �ϴ���������
				//lukunpeng 2010/06/10 ����Ҫ���������ϴ����ж��ٴ�����ã���ȫ�����ϼ�����
// 				if( IsSupportMultiSpy() )
// 				{
// 					m_cLocalSpyMana.SetConfRemainSpyBW( m_tConf.GetSndSpyBandWidth() );
// 				}			
				SetTimer( MCUVC_INVITE_UNJOINEDMT_TIMER, 1000*m_tConf.m_tStatus.GetCallInterval() );
				
				SetTimer( MCUVC_MCUSRC_CHECK_TIMER, TIMESPACE_MCUSRC_CHECK);
				
				u8 m_byUniformMode = tConfAttrib.IsAdjustUniformPack();
				cServMsg.SetMsgBody((u8 *)&m_byUniformMode, sizeof(u8));
				cServMsg.SetConfIdx(m_byConfIdx);
				SendMsgToAllMp(MCU_MP_CONFUNIFORMMODE_NOTIFY, cServMsg);
				
				//�ı����ʵ��״̬
				NEXTSTATE( STATE_ONGOING );
				
				//������ػ��������ն���Ϣ
				m_tConfAllMtInfo.m_tLocalMtInfo.SetMcuIdx( LOCAL_MCUIDX );
				//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
				//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
				SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
				
				//��������ն˱� 
				SendMtListToMcs(LOCAL_MCUIDX);	
				
				// [11/1/2011 liuxu]���ն�״̬�����
				MtStatusChange(NULL, TRUE);
				
				//֪ͨn+1���ݷ��������»�����Ϣ
				if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
				{
					ProcNPlusConfInfoUpdate(TRUE);
				}  
				m_swCurSatMtNum = m_tConf.GetSatDCastChnlNum();
			}
			else //ԤԼ����
			{	
			
				//������ԤԼ����
				if ( m_tConf.IsCircleScheduleConf() )
				{
					//������ԤԼ���飬��Чʱ�����ֻѡ���ڣ�����ʱ���룬���Ա�֤durastart��ʱ����Ϊ00:00:00,duraend��ʱ����Ϊ23:59:59
					TDurationDate tDuraDate = m_tConfEx.GetDurationDate();
					ModifyCircleScheduleConfDuraDate(tDuraDate);
					m_tConfEx.SetDurationDate(&tDuraDate);	
					
					BOOL32 bScheduleConfNeedStart = IsCircleScheduleConfNeedStart();
					if ( bScheduleConfNeedStart )
					{
						CServMsg cOngoingConfMsg;
						memcpy(&cOngoingConfMsg,&cServMsg,sizeof(cServMsg));
						TConfInfo* ptConfInfo =	(TConfInfo *)cOngoingConfMsg.GetMsgBody();
						if ( NULL != ptConfInfo )
						{
							ptConfInfo->m_tStatus.SetOngoing();
							ptConfInfo->SetStartTime( time( NULL ) );
							//���������ԤԼ����ģʽ
							ptConfInfo->ClearSchedulConfMode();
							u8 byInsID = AssignIdleConfInsID(GetInsID()+1);
							if(0 != byInsID)
							{
								cOngoingConfMsg.SetSrcMtId(CONF_CREATE_SCH);
								::OspPost(MAKEIID( AID_MCU_VC, byInsID ), MCU_SCHEDULE_CONF_START, 
									cOngoingConfMsg.GetServMsg(), cOngoingConfMsg.GetServMsgLen());
								ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuCreateConfReq]MCU_SCHEDULE_CONF_START send to Ins.%d\n", byInsID);	
							}
							else
							{
								ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMcsMcuCreateConfReq] no idle confinst!\n");
								
							}
						}
					}
					
					//���±�ԤԼ������´ο���ʱ��
					TKdvTime tNextOngoingtime = GetNextOngoingTime(m_tConf,m_tConfEx.GetDurationDate());
					TKdvTime tNullTime;//����ΪmemsetΪ0
					//�´ο���ʱ��Ϊ�գ���nack
					if (tNextOngoingtime == tNullTime   )
					{
			
						if ( !bScheduleConfNeedStart )
						{
							wErrCode = (u16)ERR_MCU_CIRCLESCHEDULECONF_TIME_WRONG;
						
						}
						else //�����ɹ���ʱ������´ο���ʱ��Ϊ�գ�������ԤԼ����Ҳû�п����ı�Ҫ
						{
							wErrCode = (u16)ERR_MCU_CIRCLESCHEDULECONF_WILLNOT_START;
						}
						//��ᣬ��mcs����ʾ
						cServMsg.SetErrorCode( wErrCode );
						SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );            
						ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuCreateConfReq]CircleScheduleConference %s cannot changeto ongoing from now on!\n", tTmpConf.GetConfName() );
						ReleaseConf();
						
						return;
					
					}
				
					
					m_tConfEx.SetNextStartTime(&tNextOngoingtime);
					
					
				}

				m_tConf.m_tStatus.SetScheduled();
				
				//�ѻ���ָ��浽������
				if( !g_cMcuVcApp.AddConf( this ) )	//add into table
				{
					ClearHduSchemInfo();
					CreateConfSendMcsNack( byOldInsId, ERR_MCU_CONFOVERFLOW, cServMsg, TRUE);
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because of full instance!\n", m_tConf.GetConfName() );
					return;
				}		
				
				//Ӧ���� 
				SendMsgToMcs( cServMsg.GetSrcSsnId(), cServMsg.GetEventId() + 1, cServMsg );
				
				//ԤԼ���鲻�ܶ������Ȩ 
				if( CONF_LOCKMODE_LOCK == m_tConf.m_tStatus.GetProtectMode() )
				{
					m_tConf.m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );
				}
				if( CONF_LOCKMODE_NEEDPWD == m_tConf.m_tStatus.GetProtectMode() )
				{
					m_tConfProtectInfo.SetMcsPwdPassed(cServMsg.GetSrcSsnId(), TRUE);
				}			
				
				//֪ͨ���л��
				g_cMcuVcApp.ConfInfoMsgPack(this, cServMsg);
				SendMsgToAllMcs(MCU_MCS_TEMSCHCONFINFO_NOTIF, cServMsg);
				
				// 2011-11-28 add by pgf: zoujunlong�������VMPģ�������Ϣ�ϱ�
				TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
				if( tConfAttrb.IsHasVmpModule() )
				{		
					CServMsg cVmpMsg;
					TVmpModule tVmpModule = GetVmpModule(); 
					cServMsg.SetMsgBody( (u8*)&tVmpModule, sizeof(TVmpModule) );
					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPINFO, cVmpMsg);
				}
				
				//���浽�ļ�
				BOOL32 bRetTmp = g_cMcuVcApp.SaveConfToFile( m_byConfIdx, FALSE );
				if (!bRetTmp)
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[creat conf]SaveConfToFile error!!!\n");
				}
				
				//zjj20100419 �ϴ���������
				//lukunpeng 2010/06/10 ����Ҫ���������ϴ����ж��ٴ�����ã���ȫ�����ϼ�����
// 				if( IsSupportMultiSpy() )
// 				{
// 					m_cLocalSpyMana.SetConfRemainSpyBW( m_tConf.GetSndSpyBandWidth() );
// 				}
				
				//����ʱ��
				SetTimer( MCUVC_SCHEDULED_CHECK_TIMER, TIMESPACE_SCHEDULED_CHECK );
				
				//��ӡ
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "A scheduled conference %s created!\n", m_tConf.GetConfName() );
				
				//�ı����ʵ��״̬
				NEXTSTATE( STATE_SCHEDULED );
			}
			
			//zjj20091102 ��ȡ�ļ�������Ϣ�ļ��е�Ԥ����Ϣ
			TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
			BOOL32 bIsConfFileGBK = IsConfFileGBK(); // mcs��Ԥ�� [pengguofeng 7/24/2013]
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuCreateConfReq]conf.%d default.%d is GBK.%d\n",
				m_byConfIdx, tConfAttrbEx.IsDefaultConf(), bIsConfFileGBK);
			if(!m_cCfgInfoRileExtraDataMgr.ReadExtraDataFromConfInfoFile(m_byConfIdx,tConfAttrbEx.IsDefaultConf()))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_MCS, "ReadExtraDataFromConfInfoFile faield!\n");
			}
			else
			{
				if ( bIsConfFileGBK )
				{
					m_cCfgInfoRileExtraDataMgr.TransEncoding2Utf8();
				}
			}
			
			break;
		}
	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	
	return;
}

/*====================================================================
    ������      ��IsConfFileGBK
    ����        �������Ƿ�ΪGBK��ʽ
    �㷨ʵ��    ���ж���û��UTF8��־
	ע��		��MCSԤ��ֻ��4.7�ż��룬�ϰ汾��Ĭ��FALSE
    ����ȫ�ֱ�����
    �������˵����none
    ����ֵ˵��  ��BOOL32
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	2013/07/17  4.7         �����          ����
====================================================================*/
BOOL32 CMcuVcInst::IsConfFileGBK()
{
	if ( m_tConf.GetConfSource() != MCS_CONF )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[IsConfFileGBK] only hanle MCS conf\n");
		return FALSE;
	}

	TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
	u8 byFileIndex = 0xFF;
	//1. ȱʡ����
	if ( tConfAttrbEx.IsDefaultConf())
	{
		byFileIndex = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;
	}
	else
	{
		byFileIndex = m_byConfIdx - MIN_CONFIDX;
	}

	//2.read file
	TConfStore tConfStoreBuf;
	TPackConfStore *ptPackConfStore = (TPackConfStore *)&tConfStoreBuf;
	if ( !GetConfFromFile(byFileIndex, ptPackConfStore))
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[IsConfFileGBK]get conf_%d.dat failed\n", byFileIndex);
		return FALSE;
	}

	u16 wPackConfDataLen = 0;
	TConfStore tConfStore;
	if (!UnPackConfStore(ptPackConfStore, tConfStore,wPackConfDataLen))
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[IsConfFileGBK]UnPackConfStore conf_%d.dat failed\n", byFileIndex);
		return FALSE;
	}

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[IsConfFileGBK]conf.%d fileidx.%d name.%s \n",
		m_byConfIdx, byFileIndex, tConfStore.m_tConfInfo.GetConfName());
	u16 wConfExInfoLen = 0;
	u8 byEncoding = emenCoding_GBK;
	BOOL32 bUnknown = FALSE;

	// ���½ӿڻ�ı�m_tConfEx����������һ����ʱ���� [pengguofeng 7/19/2013]
	TConfInfoEx tConfInfoEx;
	memcpy(&tConfInfoEx, &m_tConfEx, sizeof(TConfInfoEx));
	UnPackConfInfoEx(tConfInfoEx, tConfStore.m_byConInfoExBuf, wConfExInfoLen, bUnknown, NULL, NULL, &byEncoding);

	if ( byEncoding == emenCoding_GBK )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*====================================================================
    ������      ��IsConfInfoCheckPass
    ����        ��������Ϣ�Ƿ�ͨ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CServMsg &cMsg
                  TConfInfo &tConfInfo
                  u16       &wErrCode
                  BOOL32    bTemplate
    ����ֵ˵��  ��BOOL32
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	07/12/25    4.0         �ű���          ����
====================================================================*/
BOOL32 CMcuVcInst::IsConfInfoCheckPass(const CServMsg &cMsg, TConfInfo &tConfInfo, u16 &wErrCode, BOOL32 bTemplate)
{
    BOOL32 bConfFromFile = tConfInfo.m_tStatus.IsTakeFromFile();

    if ( tConfInfo.GetConfId().IsNull() )
    {
        //ConfLog( FALSE, "[ConfInfoCheck] ConfID.IsNull, ignore it\n" );
        //return FALSE;
    }
	
    // guzh[4/25/2007] ������������
    // zgc [6/18/2007] �޸�������������Ϊ��������Ƶ���ʼ�������Ƶ���ʲ��ܴ�������������
    if ( tConfInfo.GetBitRate() + GetAudioBitrate(tConfInfo.GetMainAudioMediaType()) > MAX_CONFBITRATE )
    {
        wErrCode = ERR_MCU_CONFBITRATE;
        if ( bConfFromFile )
        {
            g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
        }      
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conference %s create failed because bitrate(Vid.%d, Aud.%d) error!\n", tConfInfo.GetConfName(), tConfInfo.GetBitRate(), GetAudioBitrate(tConfInfo.GetMainAudioMediaType()) );
        return FALSE;
    }

    if ( !bTemplate )
    {
        //�����E164�����Ѵ��ڣ��ܾ� 
	    if( g_cMcuVcApp.IsConfE164Repeat( tConfInfo.GetConfE164(), bTemplate,tConfInfo.m_tStatus.IsOngoing() ) )
	    {
            wErrCode = ERR_MCU_CONFE164_REPEAT;
            if ( bConfFromFile )
            {
                g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
            }      
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s E164 repeated and create failure!\n", tConfInfo.GetConfName() );
            return FALSE;
        }

	    //�������Ѵ��ڣ��ܾ�
	    if( g_cMcuVcApp.IsConfNameRepeat( tConfInfo.GetConfName(), bTemplate ,tConfInfo.m_tStatus.IsOngoing()) )
	    {
            wErrCode = ERR_MCU_CONFNAME_REPEAT;
            if ( bConfFromFile )
            {
                g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
            }      
		    ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s name repeated and create failure!\n", m_tConf.GetConfName() );
		    return FALSE;
	    }

	    //�����ѽ������ܾ� 
		if ( !tConfInfo.IsCircleScheduleConf() )//��������ԤԼ����
		{
			//�ֶ���������ǰʱ��>����ʱ��+30min�����ֶ���������ǰʱ��>����ʱ��+����ʱ�䣬return FALSE
			if ( tConfInfo.GetDuration() != 0 && ( time( NULL ) > tConfInfo.GetStartTime() + tConfInfo.GetDuration() * 60 )  )
			{
				wErrCode = ERR_MCU_STARTTIME_WRONG;
				if ( bConfFromFile )
				{
					g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
				}      
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s too late and canceled!\n", tConfInfo.GetConfName() );
				return FALSE;
			}
		}
		else//������ԤԼ���飬���ټ�⣬checktimer�����Ѿ�������
		{

		}
	

        //duplicate confid
        u8 abyConfId[sizeof(CConfId)];
        tConfInfo.GetConfId().GetConfId(abyConfId, sizeof(abyConfId));
        if( OSPERR_ALIAS_REPEAT == SetAlias( ( const char* )abyConfId, sizeof( CConfId ) ) )
        {
            wErrCode = ERR_MCU_CONFIDREPEAT;
            if ( bConfFromFile )
            {
                g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
            }      
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because of repeat conference ID!\n", tConfInfo.GetConfName() );
            return FALSE;
        }        
    }


	//dynamic vmp and vmp module conflict
	TConfAttrb tConfAttrb = tConfInfo.GetConfAttrb();
	if( tConfAttrb.IsHasVmpModule() && 
		CONF_VMPMODE_AUTO == tConfInfo.m_tStatus.GetVMPMode() )
	{
        wErrCode = ERR_MCU_DYNAMCIVMPWITHMODULE;
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf or temp %s create failed because has module with dynamic vmp!\n", tConfInfo.GetConfName() );
		return FALSE;		
	}

	//��Ч�Ļ��鱣����ʽ,�ܾ�
	if( tConfInfo.m_tStatus.GetProtectMode() > CONF_LOCKMODE_LOCK )
	{
        wErrCode = ERR_MCU_INVALID_CONFLOCKMODE;
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf or temp %s protect mode invalid and nack!\n", tConfInfo.GetConfName() );
		return FALSE;
	}

	//��Ч�Ļ�����в���,�ܾ�
	if( tConfInfo.m_tStatus.GetCallMode() > CONF_CALLMODE_TIMER )
	{
        wErrCode = ERR_MCU_INVALID_CALLMODE;
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf or temp %s call mode invalid and nack!\n", tConfInfo.GetConfName() );
		return FALSE;
	}

    // xsl [11/16/2006] ��û������GK�����õ�GK��֧�ּƷѣ�����ҪGK�Ʒ���������
    if (tConfInfo.IsSupportGkCharge())
    {
        if (0 == g_cMcuAgent.GetGkIpAddr())
        {
            if ( !bTemplate )
            {
                wErrCode = ERR_MCU_GK_UNEXIST_CREATECONF;
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s not config gk in gk charge conf and nack!\n", tConfInfo.GetConfName() );
                return FALSE;                
            }
            else
            {
                NotifyMcsAlarmInfo( cMsg.GetSrcSsnId(), ERR_MCU_GK_UNEXIST_CREATECONF );
            }
        }
        else
        {
            if ( !g_cMcuAgent.GetIsGKCharge())
            {
                if ( !bTemplate )
                {
                    wErrCode = ERR_MCU_GK_NOCHARGE_CREATECONF;
                    ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s configed gk support no charge conf and nack!\n", tConfInfo.GetConfName() );
                    return FALSE;
                }
                else
                {
                    NotifyMcsAlarmInfo( cMsg.GetSrcSsnId(), ERR_MCU_GK_NOCHARGE_CREATECONF );
                }
            }
        }
    }

    //�����������������ô���˫��ʽ��˫�ٻ���ģ��
    TConfAttrb ttmpAttrb = tConfInfo.GetConfAttrb();
    if ( !ttmpAttrb.IsUseAdapter() && 
        ( MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType() ||
          MEDIA_TYPE_NULL != tConfInfo.GetSecAudioMediaType() ||
          0 != tConfInfo.GetSecBitRate() ) 
       )
    {        
        ttmpAttrb.SetUseAdapter(TRUE);
        tConfInfo.SetConfAttrb(ttmpAttrb);
    }
    
    //�����鲻��Ҫ���䣬���ڻ��������в�����������
    if (MEDIA_TYPE_NULL == tConfInfo.GetSecVideoMediaType() &&
        MEDIA_TYPE_NULL == tConfInfo.GetSecAudioMediaType() &&
        0 == tConfInfo.GetSecBitRate() &&
        ttmpAttrb.IsUseAdapter() &&
        // ������������ֱ����ն����, zgc, 2008-08-09
        !IsHDConf( tConfInfo ) &&
        // H264˫����ʽ��������, zgc, 2008-08-21
        !(( (tConfInfo.GetCapSupport().GetDStreamMediaType() == VIDEO_DSTREAM_MAIN ||
		     tConfInfo.GetCapSupport().GetDStreamMediaType() == VIDEO_DSTREAM_MAIN_H239) &&
           tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264) ||
          tConfInfo.GetCapSupport().GetDStreamMediaType() == VIDEO_DSTREAM_H264_H239 ||
          tConfInfo.GetCapSupport().GetDStreamMediaType() == VIDEO_DSTREAM_H264) &&
        tConfInfo.GetCapSupport().GetDStreamMediaType() != VIDEO_DSTREAM_H264_H263PLUS_H239 &&
		// VCS����Ĭ�϶�����������
		tConfInfo.GetConfSource() != VCS_CONF)
    {
        ttmpAttrb.SetUseAdapter(FALSE);
        tConfInfo.SetConfAttrb(ttmpAttrb);
        ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "template/conf %s need not bas adapter, auto modify it in conf attrib.\n", tConfInfo.GetConfName());
    }

    // �Ǹ�����鲻�Զ���ѡ����ϳ�����Ӧ, zgc, 2008-08-20
    /*
	// MP4/MP2 CIF���ϵĸ�ʽ ��������ϳ�����Ӧ��zgc��2008-01-17
	if ( ( MEDIA_TYPE_MP4 == tConfInfo.GetMainVideoMediaType() 
		|| MEDIA_TYPE_H262 == tConfInfo.GetMainVideoMediaType() 
		)
		&& 
		( VIDEO_FORMAT_2CIF == tConfInfo.GetMainVideoFormat()
		|| VIDEO_FORMAT_4CIF == tConfInfo.GetMainVideoFormat()
		|| VIDEO_FORMAT_AUTO == tConfInfo.GetMainVideoFormat()
		)
		)
	{
		if ( !tConfInfo.IsVmpAutoAdapt() )
		{
			tConfInfo.SetVmpAutoAdapt( TRUE );
			ConfLog(FALSE, " conf.%d need vmp auto adapt, open.\n", m_byConfIdx );
		}
	}

	// H264/D1��ʽ����������ϳ�����Ӧ, zgc, 2008-04-02
	if ( MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType() &&
		VIDEO_FORMAT_CIF != tConfInfo.GetMainVideoFormat() )
	{
		if ( !tConfInfo.IsVmpAutoAdapt() )
		{
			tConfInfo.SetVmpAutoAdapt( TRUE );
			ConfLog(FALSE, " conf.%d need vmp auto adapt, open.\n", m_byConfIdx );
		}
	}*/
    if ( IsHDConf(tConfInfo) )
    {
        if ( !tConfInfo.IsVmpAutoAdapt() )
        {
            tConfInfo.SetVmpAutoAdapt( TRUE );
            ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, " conf.%d need vmp auto adapt, open.\n", m_byConfIdx );
		}
    }

    // guzh [12/27/2007] H264��D1�����Ϸֱ�����������
    if ( MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType() )
    {
        // guzh 2008/08/07 �ſ�����
        /*
        if (VIDEO_FORMAT_CIF != tConfInfo.GetMainVideoFormat() ||
            tConfInfo.GetBitRate() >= 2048)
        {
            if ( MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType() ||
                 0 != tConfInfo.GetSecBitRate() ||
                 ttmpAttrb.IsHasTvWallModule() )
            {
                wErrCode = ERR_MCU_CONFFORMAT;
                ConfLog( FALSE, "template/conf %s not support H264-D1 and extra feacher at the same time\n", tConfInfo.GetConfName() );
                return FALSE;
            }
        }
        */

#ifdef _MINIMCU_
        if ( CMcuPfmLmt::IsConfFormatHD(tConfInfo) && 
             !g_cMcuAgent.IsMcu8000BHD() )
        {
            wErrCode = ERR_MCU_CONFFORMAT;
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "template/conf %s not support H264-HD in MCU8000B-NoHD\n", tConfInfo.GetConfName() );
            return FALSE;
        }
#endif

    }        

    //δ���û������ģʽ��������Կ����ģʽ����Ч
    if( CONF_ENCRYPTMODE_NONE == ttmpAttrb.GetEncryptMode() &&
        CONF_ENCRYPTMODE_NONE != tConfInfo.GetMediaKey().GetEncryptMode() )
    {
        TMediaEncrypt tEncrypt = tConfInfo.GetMediaKey();            
        tEncrypt.Reset();
        tConfInfo.SetMediaKey(tEncrypt);
    }   

    //�������
    //�����˻������ģʽ��δ������Կ����ģʽ����Ч
    if( CONF_ENCRYPTMODE_NONE != ttmpAttrb.GetEncryptMode() &&
        CONF_ENCRYPTMODE_NONE == tConfInfo.GetMediaKey().GetEncryptMode() )
    {
        TMediaEncrypt tEncrypt = tConfInfo.GetMediaKey();
        tEncrypt.SetEncryptMode(ttmpAttrb.GetEncryptMode());
        
        //DES����
        if( CONF_ENCRYPTMODE_DES == ttmpAttrb.GetEncryptMode() )
        {
            //�Զ���Կ. ��: �ֶ���Կ��ȡ�Ľṹ�Ѵ���,��ͬ.
            if ( !ttmpAttrb.IsEncKeyManual() )
            {
                tEncrypt.SetEncryptKey(GetRandomKey(), LEN_DES);
            }
        }
        //AES����
        else if( CONF_ENCRYPTMODE_AES == ttmpAttrb.GetEncryptMode() )
        {
            //�Զ���Կ����. 
            if ( !ttmpAttrb.IsEncKeyManual() )
            {
                tEncrypt.SetEncryptKey(GetRandomKey(), LEN_AES);
            }
        }
        
        tConfInfo.SetMediaKey(tEncrypt);
    }    

    //��������£���֧�ֹ�һ����������ʽ
    if( CONF_ENCRYPTMODE_NONE != m_tConf.GetMediaKey().GetEncryptMode() )
    {
        ttmpAttrb.SetUniformMode( CONF_UNIFORMMODE_NONE );
        tConfInfo.SetConfAttrb( ttmpAttrb );
    }
    
    // ����@2006.4.6 �鲥���飬ģ����Ҫ����鲥��ַ�Ƿ�Ϸ�
    if ( ttmpAttrb.IsMulticastMode() )
    {
        u32 dwCastIp = ttmpAttrb.GetMulticastIp();
        u16 wCastPort = ttmpAttrb.GetMulticastPort();
        
        BOOL32 bIpInvalid = FALSE;
        if ( dwCastIp < MULTICAST_IP_START || dwCastIp > MULTICAST_IP_END ||
             (dwCastIp > MULTICAST_IP_RESV_START && dwCastIp < MULTICAST_IP_RESV_END ) ||
             wCastPort == 0)
        {
            bIpInvalid = TRUE;
        }
        
        if (bIpInvalid)
        {
            wErrCode = ERR_MCU_MCASTADDR_INVALID;
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf or temp.%s create failed because of multicast address invalid!\n", tConfInfo.GetConfName() );
            return FALSE;
        }
    }

    // ����@2006.4.11 ��ɢ����        
    if ( ttmpAttrb.IsSatDCastMode() )
    {
        u32 dwCastIp = ttmpAttrb.GetSatDCastIp();
        u16 wCastPort = ttmpAttrb.GetSatDCastPort();
        // ��Ҫ����ַ�Ƿ�Ϸ�
        BOOL32 bIpInvalid = FALSE;
        if ( dwCastIp < MULTICAST_IP_START || dwCastIp > MULTICAST_IP_END ||
             (dwCastIp > MULTICAST_IP_RESV_START && dwCastIp < MULTICAST_IP_RESV_END ) ||
             wCastPort == 0)
        {
            bIpInvalid = TRUE;
        }
        //BOOL32 bAdpNeeded = FALSE;
		//���ǻ������������������õ����ǻ��鿪��
/*
        if (tConfInfo.GetSecBitRate() != 0 ||
            /*!tConfInfo.GetCapSupport().GetSecondSimCapSet().IsNull()
			(
			//zjl 20101029 ����ʽ�����ڸ���ʽ������Ƶ��
				(tConfInfo.GetCapSupport().GetMainVideoType() != MEDIA_TYPE_NULL &&
					tConfInfo.GetCapSupport().GetSecVideoType() != MEDIA_TYPE_NULL &&
					tConfInfo.GetCapSupport().GetMainVideoType() != tConfInfo.GetCapSupport().GetSecVideoType()
					  ) ||
				 (tConfInfo.GetCapSupport().GetMainAudioType() != MEDIA_TYPE_NULL &&
					tConfInfo.GetCapSupport().GetSecAudioType() != MEDIA_TYPE_NULL &&
					tConfInfo.GetCapSupport().GetMainAudioType() != tConfInfo.GetCapSupport().GetSecAudioType()
						)
					)
				)

        {
            // ˫��˫��ʽ���鲻��ѡ���ɢ���鷽ʽ
            bAdpNeeded = TRUE;
        }
   */     
        // ������ֻ�ܿ��Լ�
/*        if (ttmpAttrb.GetSpeakerSrc() != CONF_SPEAKERSRC_SELF)
        {
            ttmpAttrb.SetSpeakerSrc( CONF_SPEAKERSRC_SELF );
            ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Conf or temp.%s change speakersrc to self.\n", tConfInfo.GetConfName() );
        }
        // ����������
        if (ttmpAttrb.IsUseAdapter())
        {
            ttmpAttrb.SetUseAdapter(FALSE);
            ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Conf or temp.%s no use adapter\n", tConfInfo.GetConfName() );
        }
*/
        // �����ۻ���
        if (ttmpAttrb.IsDiscussConf())
        {
            ttmpAttrb.SetDiscussConf(FALSE);
            ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Conf or temp.%s no start discuss\n", tConfInfo.GetConfName() );
        }
		//���ǻ��鱾���ն˻���֧��prs�ģ������ﲻ�����⴦��
//         // �������ش�
//         if (ttmpAttrb.IsResendLosePack())
//         {
//             ttmpAttrb.SetPrsMode(FALSE);
//             ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "Conf or temp.%s no use prs\n", tConfInfo.GetConfName() );
//         }
        // xsl [7/20/2006] ��֧�ּ���
        if (ttmpAttrb.IsSupportCascade())
        {
            ttmpAttrb.SetSupportCascade(FALSE);
        }            
        tConfInfo.SetConfAttrb( ttmpAttrb );
        
        // xsl [7/21/2006] �ش�ͨ��������Ϊ0
        if (tConfInfo.GetSatDCastChnlNum() == 0)
        {
            tConfInfo.SetSatDCastChnlNum(1);
        }
        
        if (bIpInvalid/*|| bAdpNeeded*/)
        {                
            if (bIpInvalid)
            {
                wErrCode = ERR_MCU_DCASTADDR_INVALID;
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conf or temp.%s create failed because of distributed conf multicast address invalid!\n", tConfInfo.GetConfName() );
            }
            else
            {
                wErrCode = ERR_MCU_DCAST_NOADP;
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conf or temp.%s create failed because of distributed conf not support second rate or format!\n", tConfInfo.GetConfName() );
            }  
            return FALSE;
        }
    }

    // guzh [9/1/2007] 8000B/8000C˫ý�����֧��
#ifdef _MINIMCU_
    if ( tConfInfo.GetSecBitRate() > 0 &&                 
         MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType() && 
         ( tConfInfo.GetMainVideoMediaType() != tConfInfo.GetSecVideoMediaType() ||
           tConfInfo.GetMainVideoFormat() != tConfInfo.GetSecVideoFormat() )
        )
    {
        // ��֧�ֵ����鲥
        if ( tConfInfo.GetConfAttrb().IsMulticastMode() && tConfInfo.GetConfAttrb().IsMulcastLowStream() )
        {
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conf or temp.%s not support multicast low stream\n", tConfInfo.GetConfName() );
            wErrCode = ERR_MCU_MCLOWNOSUPPORT;
            return FALSE;
        }
        
        // ��֧�ֵ���¼��
        TConfAutoRecAttrb tRecAttrb = tConfInfo.GetAutoRecAttrb();
        if ( tRecAttrb.IsAutoRec() && tRecAttrb.GetRecParam().IsRecLowStream() )
        {
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conf or temp.%s can't support auto record low stream\n", tConfInfo.GetConfName() );
            wErrCode = ERR_MCU_RECLOWNOSUPPORT;
            return FALSE;    
        }
    }
#endif

    //zbq[11/18/2008] mau��صĴ���������������ʾ
//     if (tConfInfo.GetConfSource() != VCS_CONF &&
//         ((IsHDConf(tConfInfo) &&
// 		  (0 != tConfInfo.GetSecBitRate() ||
// 		   (MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType() && 0 != tConfInfo.GetSecVideoMediaType()) ||
// 		   tConfInfo.GetConfAttrbEx().IsResEx1080() ||
// 		   tConfInfo.GetConfAttrbEx().IsResEx720()  ||
// 		   tConfInfo.GetConfAttrbEx().IsResEx4Cif() ||
// 		   tConfInfo.GetConfAttrbEx().IsResExCif()
// 		  )
// 		 )
// 		 ||
// 		 // ˫˫��
// 		 (tConfInfo.GetCapSupport().IsDStreamSupportH239() &&
// 		  MEDIA_TYPE_H264 == tConfInfo.GetDStreamMediaType() &&
// 		  (g_cMcuVcApp.IsSupportSecDSCap() || MEDIA_TYPE_NULL != tConfInfo.GetCapSupportEx().GetSecDSType())
// 		 )
//         )
// 	   )
//     {
//         CBasMgr cBasMgr;
//         if (!cBasMgr.IsHdBasSufficient(tConfInfo))
//         {
//             ConfLog(FALSE, "temp.%s's mau might be insufficient\n", tConfInfo.GetConfName());
//             NotifyMcsAlarmInfo(cMsg.GetSrcSsnId(), ERR_MCU_NOENOUGH_HDBAS_TEMP);
//         }
// 
//         TMcuHdBasStatus tHdBasStatus;
//         cBasMgr.GetHdBasStatus(tHdBasStatus, tConfInfo);
// 
//         CServMsg cServMsg;
//         cServMsg.SetMsgBody((u8*)&tHdBasStatus, sizeof(tHdBasStatus));
//         cServMsg.SetEventId(MCU_MCS_MAUSTATUS_NOTIFY);
//         SendMsgToMcs(cMsg.GetSrcSsnId(), MCU_MCS_MAUSTATUS_NOTIFY, cServMsg);
//     }

    return TRUE;
}
/*====================================================================
    ������      ��RlsAllBasForConf
    ����        ���ͷŻ�������ռ�õ�BAS
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	2010-8-4    4.6			�ܼ���			��д
====================================================================*/
void CMcuVcInst::RlsAllBasForConf()
{
	StopAllBrdAdapt();
	StopAllSelAdapt();
	ReleaseBasChn();
	ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[RlsAllBasForConf]RlsAllBasForConf!\n");
    return;
}

/*====================================================================
    ������      ��PrepareAllNeedBasForConf
    ����        �����ݻ�������Ԥ�������п�����Ҫ��BAS
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16       &wErrCode : [OUT]����ִ�����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	07/12/25    4.0         �ű���          ����
    08/11/18    4.5         �ű���          ���Ӷ�MAU��У��
====================================================================*/
BOOL32 CMcuVcInst::PrepareAllNeedBasForConf(u16* pwErrCode)
{
	CBasChn *aptNeedChn[MAXNUM_PERIEQP]={NULL};	//ͨ��ָ������
	TBasChnCapData atBasChnParam[MAXNUM_PERIEQP];
	u16 wErrorCode = ERR_MCU_NOENOUGH_HDBAS_CONF;
	if(g_cMcuVcApp.IsBasChnlEnoughForConf(m_tConf, m_tConfEx, aptNeedChn,atBasChnParam,wErrorCode))
	{
		ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[PrepareAllNeedBasForConf]PrepareAllNeedBasForConf:%d OK\n",m_byConfIdx);
		g_cMcuVcApp.OcuppyBasChn(aptNeedChn, MAXNUM_PERIEQP);
		AssignBasBrdChn(aptNeedChn, atBasChnParam, MAXNUM_PERIEQP,m_tConf.GetConfId());
		return TRUE;
	}
	else
	{
		if ( NULL == pwErrCode  )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[PrepareAllNeedBasForConf] pwErrCode is NULL!\n");
			return FALSE;
		}
		*pwErrCode = wErrorCode;
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[PrepareAllNeedBasForConf] Bas is not enough for conf!\n");
	}
	return FALSE;
}
/*====================================================================
    ������      ��IsEqpInfoCheckPass
    ����        ��������Ϣ�Ƿ�ͨ����飬�����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CServMsg &cSrcMsg: [IN] ����Դ
                  u16       &wErrCode : [OUT]����ִ�����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	07/12/25    4.0         �ű���          ����
    08/11/18    4.5         �ű���          ���Ӷ�MAU��У��
====================================================================*/
BOOL32 CMcuVcInst::IsEqpInfoCheckPass(const CServMsg &cSrcMsg, u16 &wErrCode)
{
    u8 byIdleVMPNum = 0;
    u8 abyVMPId[MAXNUM_PERIEQP];
    memset(abyVMPId, 0, sizeof(abyVMPId));
    g_cMcuVcApp.GetIdleVMP(abyVMPId, byIdleVMPNum, sizeof(abyVMPId));

    TVMPParam_25Mem tVmpParam = GetVmpParam25MemFromModule();

    if ( m_tConf.GetConfId().IsNull() )
    {
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "[EqpInfoCheck] tConfInfo.IsNull, ignore it\n" );
        return FALSE;
    }

    //������ֻ������ԣ�Ԥ����Դ
    TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();

    // ����@2006.4.6 �鲥���飬��Ҫ����鲥��ַ�Ƿ����
    if ( tConfAttrib.IsMulticastMode() )
    {
        u32 dwCastIp  = tConfAttrib.GetMulticastIp();
        u16 wCastPort = tConfAttrib.GetMulticastPort();
        
        BOOL32 bAddrUsed = FALSE;
        
        // ������еļ�ʱ���飬�Ƿ�ʹ���˱���ַ��
        bAddrUsed = g_cMcuVcApp.IsMulticastAddrOccupied(dwCastIp, wCastPort);
        if (bAddrUsed)
        {
            wErrCode = ERR_MCU_MCASTADDR_USED;
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because of multicast address occupied!\n", m_tConf.GetConfName() );
            return FALSE;
        }
    }
    // ����@2006.4.12 ��ɢ���飬����鲥��ַ�Ƿ����
    if (tConfAttrib.IsSatDCastMode())
    {
        u32 dwCastIp  = tConfAttrib.GetSatDCastIp();
        u16 wCastPort = tConfAttrib.GetSatDCastPort();
        
        BOOL32 bAddrUsed = FALSE;
        
        // ������еļ�ʱ���飬�Ƿ�ʹ���˱���ַ��
        bAddrUsed = g_cMcuVcApp.IsDistrConfCastAddrOccupied(dwCastIp, wCastPort);
        
        if (bAddrUsed)
        {
            wErrCode = ERR_MCU_DCASTADDR_USED;
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conference %s create failed because of distributed conf multicast address occupied!\n", m_tConf.GetConfName() );
            return FALSE;
        }                
    }

    //�Ƿ���Ҫ�������ݻ���            
    if ( CONF_DATAMODE_VAANDDATA == tConfAttrib.GetDataMode() || 
         CONF_DATAMODE_DATAONLY  == tConfAttrib.GetDataMode() )
    {
        BOOL32 bHasDcs = FALSE;
        for ( s32 nIndex = 1; nIndex <= MAXNUM_MCU_DCS; nIndex ++ )
        {
            //���û��DCS����
            if ( g_cMcuVcApp.IsPeriDcsValid( nIndex ) && 
                 g_cMcuVcApp.IsPeriDcsConnected( nIndex ) )
            {
                bHasDcs = TRUE;
                break;
            }
        }
        if(!bHasDcs)
        {
            wErrCode = ERR_MCU_NOIDLEDCS;
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conference %s create failed because of no DCS registered!\n", m_tConf.GetConfName() );
            return FALSE;
        }
    }

    //zbq[12/27/2006]��ѯ����MCU��ǰ���п���VMP��ID �� ����VMP�Ƿ�֧���жϷ���
	if( tConfAttrib.IsHasVmpModule() )
	{
		//�Ƿ��л���ϳ���
		if( 0 == byIdleVMPNum )
		{
            if ( CONF_CREATE_NPLUS == m_byCreateBy )
            {
                NotifyMcsAlarmInfo( cSrcMsg.GetSrcSsnId(), ERR_MCU_NPLUS_MASTER_VMP_NONE );
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "the master mcu couldn't restore VMP due to no VMP !\n" );

                // N+1���ݻع������Ļ��飬���������û��VMP���������ָ�����ϳɣ�
                // ��Ҫ�����ص����ݡ����򣬸û����ڱ����ٴλָ��� �����ᵼ�»�
                // ��״̬���ң����һ���������û��VMP�������޷��ٽ�����ع����� [12/28/2006-zbq]
                if ( MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() )
                {
                    m_tConf.m_tStatus.SetVmpBrdst( FALSE );
                    m_tConf.SetHasVmpModule( FALSE );
                    m_tConf.m_tStatus.SetVMPMode( CONF_VMPMODE_NONE );
                    
                    //���ͨ��
                    for( u8 byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
                    {
                        m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );
                    }
                }
                else
                {
                    //N+1���ݻָ����飬�����ڵ�VMP�������߼��ͱ�����������������
                    ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conf.%s created by NPlus error which is impossible, check it !\n", m_tConf.GetConfName() );
                }
            }
            else
            {
                wErrCode = ERR_MCU_NOIDLEVMP;
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conference %s create failed because no idle vmp!\n", m_tConf.GetConfName() );
                return FALSE;
            }
		}
        /*else
        {
            m_tConf.m_tStatus.SetVMPMode(CONF_VMPMODE_CTRL);
            m_tConf.m_tStatus.SetVmpBrdst(m_tConfEqpModule.GetVmpModule().m_tVMPParam.IsVMPBrdst());
        }*/
	}
    
    if( tConfAttrib.IsHasTvWallModule() )
    {
        BOOL32 bTvWallDisconnected = FALSE;  //  �Ƿ���ڵ���ǽ������
        TMultiTvWallModule tMultiTvWallModule;
        m_tConfEqpModule.GetMultiTvWallModule( tMultiTvWallModule );
        TTvWallModule tTvWallModule;
		THduChnlInfo  atHduChnlInfo[MAXNUM_TVWALL_CHNNL_INSMOUDLE];
        for( u8 byTvLp = 0; byTvLp < tMultiTvWallModule.GetTvModuleNum(); byTvLp++ )
        {
            tMultiTvWallModule.GetTvModuleByIdx(byTvLp, tTvWallModule);
			//zjl[20091208]����ģ������������hduԤ���������ն˳�Ա
			u8 byTvWallId = tTvWallModule.GetTvEqp().GetEqpId();
			u8 byEqpType  = tTvWallModule.GetTvEqp().GetEqpType();
			if (EQP_TYPE_HDU_SCHEME == byEqpType)
			{
				u8 byHduSchemeNum = 0;
				THduStyleInfo atHduStyleInfoTable[MAX_HDUSTYLE_NUM];
				u16 wRet = g_cMcuAgent.ReadHduSchemeTable(&byHduSchemeNum, atHduStyleInfoTable);
				if (SUCCESS_AGENT != wRet)
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[IsEqpInfoCheckPass] ReadHduSchemeTable failed!\n");
				}
				else
				{
					for (u8 byStyleIdx = 0 ; byStyleIdx < byHduSchemeNum; byStyleIdx ++)
					{
						if (atHduStyleInfoTable[byStyleIdx].GetStyleIdx() == byTvWallId)
						{
							atHduStyleInfoTable[byStyleIdx].GetHduChnlTable(atHduChnlInfo);
							for (u8 byChnl = 0; byChnl < MAXNUM_TVWALL_CHNNL_INSMOUDLE; byChnl++)
							{
								if( TW_MEMBERTYPE_NULL != tTvWallModule.m_abyMemberType[byChnl] &&
									!g_cMcuVcApp.IsPeriEqpConnected( atHduChnlInfo[byChnl].GetEqpId()))
								{
									ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[IsEqpInfoCheckPass]HDU(%d,%d) (%d) is not connected,So Return!\n\n",
									atHduChnlInfo[byChnl].GetEqpId(),atHduChnlInfo[byChnl].GetChnlIdx(),tTvWallModule.m_abyMemberType[byChnl]);
									wErrCode = ERR_MCU_NOIDLETVWALL;
									return FALSE;
								}
							}
						}
					}
				}
			}
			else
			{
				if( !g_cMcuVcApp.IsPeriEqpConnected( tTvWallModule.m_tTvWall.GetEqpId()) )
				{
					bTvWallDisconnected = TRUE;
					break;
				}
			}            
        }
		
        if( bTvWallDisconnected )
        {
            memset((void*)&m_tConfEqpModule, 0, sizeof(m_tConfEqpModule));
            wErrCode = ERR_MCU_NOIDLETVWALL;
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because Tv %d is outline!\n", 
                                m_tConf.GetConfName(), tTvWallModule.m_tTvWall.GetEqpId() );
			return FALSE;
        }
    }
	
    //�Ƿ�ʼ����, Ԥ�������� 
    //���Ƿ�ɢ���鲻֧���Զ��������ۺ��������������Բ��ؼ�������Դ
    if( !m_tConf.GetConfAttrb().IsSatDCastMode() && 
        ( tConfAttrib.IsDiscussConf() || 
          m_tConf.GetConfAttrbEx().IsSupportAutoVac() ) )
    {
        // ���񻪣��������ȼ���Ƿ��л����� [4/29/2006]
        // �������ٿ�ʼ����
        u8 byEqpId = 0;
        TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];
		memset(atAudioTypeDesc, 0, sizeof(atAudioTypeDesc));
		u8 byAudioCapNum = m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);

        byEqpId = g_cMcuVcApp.GetIdleMixer(0, byAudioCapNum, atAudioTypeDesc);
        if (0 == byEqpId)
        {
            if( CONF_CREATE_MT == m_byCreateBy ) //�ն˴��᲻ֱ�ӷ���
            {
                tConfAttrib.SetDiscussConf(FALSE);
                m_tConf.SetConfAttrb(tConfAttrib);
            }
			//[chendaiwei2010/10/17] N+1���˻ָ���������˻ع����飬ȡ�����ܻ������û�п��еĻ�����
			else if (CONF_CREATE_NPLUS == m_byCreateBy )
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "Failed to recover N+1 conf with AutoMixing, cancel auto mixing!\n" );

                tConfAttrib.SetDiscussConf(FALSE);
                m_tConf.SetConfAttrb(tConfAttrib);
			}
            else
            {
                wErrCode = ERR_MCU_NOIDLEMIXER;
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conference %s create failed because no idle mixer!\n", m_tConf.GetConfName() );
                return FALSE;
            }                    
        }
        else
        {
            //tianzhiyong 2010/04/15  ���û������Ƿ����
            if (!CheckMixerIsValide(byEqpId))
            {
				wErrCode = ERR_MCU_NOIDLEMIXER_INCONF ;
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because CheckMixerIsValide() mixer!\n", m_tConf.GetConfName() );
                return FALSE;
            }
        }
    }

    //bas modify 2
    //��Ҫý����������Ļ���,Ԥ��������                       
    if ( tConfAttrib.IsUseAdapter() &&
		 !PrepareAllNeedBasForConf(&wErrCode))
	{
		return FALSE;
	}

	//��Ҫ�����ش��Ļ���,Ԥ�����ش�ͨ�� 
	if (tConfAttrib.IsResendLosePack())
	{
		if (!AssignPrsChnnl4Conf())
        {
			// ������Դʧ��
			//zjl[20091209]���prs��Դ���㵼�»����޷��ٿ�����Ҫ�ͷŻ���ռ�õ�bas��Դ
			RlsAllBasForConf();
            wErrCode = ERR_MCU_NOIDLEPRS;
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because no idle prs!\n", m_tConf.GetConfName() );
			return FALSE;
		}
	}

    // guzh [8/29/2007] ˫ý�������������,��֧�ֵ����鲥
#ifdef _MINIMCU_
    if ( ISTRUE(m_byIsDoubleMediaConf) )                
    {
        if ( m_tConf.GetConfAttrb().IsMulticastMode() && 
             m_tConf.GetConfAttrb().IsMulcastLowStream() )
        {
            wErrCode = ERR_MCU_MCLOWNOSUPPORT;
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf.%s not support multicast low stream\n", m_tConf.GetConfName() );
            return FALSE;                
        }
    }
#endif

	// ���鿪ʼ�Զ�¼��
	//  [12/27/2009 pengjie] Modify �����޸Ĳ��ܸ��춨ʱ¼�����⣬���Ѹò��ִ�����ȡ�����ӿ�
	if( !SetAutoRec(wErrCode) )
	{
		return FALSE;
	}
	// End Modify

    //songkun,20110609,�����Ϣ��ǰ���У�ģ�廭��ϳɻ��жϻ����Ƿ�������ӣ�
    //�Ծ���������Ϣ���VMP
    //�ѻ���ָ��浽������
    if(!g_cMcuVcApp.AddConf( this ) )
    {
          RlsAllBasForConf();
          wErrCode = ERR_MCU_CONFOVERFLOW;
          ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because of full instance!\n", m_tConf.GetConfName() );
          return FALSE;
    }

    //�趯̬�����Ļ��鿪ʼ����ϳ�,ģ�濪��(�Զ�/����)vmp,��ģ��ͳһ����,����֧�ְ�auto��������m_tConf.m_tStatus����.
	if( /*CONF_VMPMODE_AUTO == m_tConf.m_tStatus.GetVMPMode() ||*/ 
		tConfAttrib.IsHasVmpModule() )			
	{
		//�Ƿ��л���ϳ��� 
		if( 0 != byIdleVMPNum )
		{
            //����֧�ֵ�ǰ�ϳɷ��Ŀ���VMP��ID
            u8 byVmpId = 0;

            /*if ( CONF_VMPMODE_AUTO != m_tConf.m_tStatus.GetVMPMode() )
            {*/
                //��ǰ���еĿ���VMP�Ƿ�֧������ĺϳɷ��
                u16 wError = 0;
                if ( IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError) ) 
                {
                    /*m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
                    m_tVmpEqp.SetConfIdx( m_byConfIdx );*/

					/* �Զ�vmpʱ,����tVmpParam��Ϣ<yhz���ж��߼�Ҫɾ��,���治Ӧ�ð�vmp��Ϣ�浽TConfInfo��,bug�����ɾ��>
					if ( CONF_VMPMODE_AUTO == m_tConf.m_tStatus.GetVMPMode() )
					{
						tVmpParam.SetVMPAuto(TRUE);
						tVmpParam.SetVMPBrdst(m_tConf.m_tStatus.m_tVMPParam.IsVMPBrdst());
						tVmpParam.SetVMPSchemeId(m_tConf.m_tStatus.m_tVMPParam.GetVMPSchemeId());
						tVmpParam.SetIsRimEnabled((u8)m_tConf.m_tStatus.m_tVMPParam.GetIsRimEnabled());
					}*/
					//ִ�п�������ϳɴ���
					// ���ǻ�����Ҫcheck vmpparam [pengguofeng 1/17/2013]
					// CheckVmpParam���޸�tVmpParam���˴�����ʱ����ȥ�ж� [pengguofeng 1/22/2013]
					TVMPParam_25Mem tTmpVmpParam = tVmpParam;
					if ( m_tConf.GetConfAttrb().IsSatDCastMode()
						&& !CheckVmpParam(byVmpId,/*tVmpParam*/tTmpVmpParam, wErrCode) )
					{
						m_tConfInStatus.SetVmpModuleUsed(TRUE); //  [pengguofeng 1/19/2013]��ֹ����changeVMpSwitch�жϴ�
						LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[IsEqpInfoCheckPass]Sat Conf not Support VmpParam Error:%d\n", wErrCode);
					}
					else //����VMP
					{
						//u8 byIsVmpModule = TRUE;
						CServMsg cServMsg = cSrcMsg;
						cServMsg.SetEqpId(byVmpId);
						cServMsg.SetEventId(MCS_MCU_STARTVMP_REQ);
						cServMsg.SetMsgBody( (u8 *)&tVmpParam, 	sizeof(tVmpParam) );
						//cServMsg.CatMsgBody( (u8 *)&byIsVmpModule, sizeof(u8) );
						ProcStartVmpReq(cServMsg);
					}
                }
                else
                {
                    if ( CONF_CREATE_NPLUS == m_byCreateBy )
                    {
                        NotifyMcsAlarmInfo( cSrcMsg.GetSrcSsnId(), ERR_MCU_ALLIDLEVMP_NO_SUPPORT_NPLUS );
                        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because all the idle vmp can't support the VMP style!\n", m_tConf.GetConfName() );

                        // N+1���ݻع������Ļ��飬���������VMP����С�ڱ��ˣ����ָ�����ϳɣ�
                        // ��Ҫ�����ص����ݡ����򣬸û����ڱ����ٴλָ���ᵼ�»���״̬���ҡ�[12/28/2006-zbq]
                        if ( MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() )
                        {
                            m_tConf.m_tStatus.SetVmpBrdst( FALSE );
                            m_tConf.SetHasVmpModule( FALSE );
                            m_tConf.m_tStatus.SetVMPMode( CONF_VMPMODE_NONE );

                            //���ͨ��
                            for( u8 byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
                            {
                                m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );
                            }
                        }
                        else
                        {
                            //N+1���ݻָ����飬�����ڵ�VMP�������߼��ͱ�����������������
                            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf.%s created by NPlus error which is impossible, check it !\n", m_tConf.GetConfName() );
                        }
                    }
                    else
                    {
						RlsAllBasForConf();
                        g_cMcuVcApp.RemoveConf( m_byConfIdx );    
                        wErrCode = wError;
                        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because all the idle vmp can't support the VMP style!\n", m_tConf.GetConfName() );
                        return FALSE;
                    }
                }
            /*}
			//�Զ�VMPֱ�����û���ϳɲ���  
			else
			{
                //��ǰ���еĿ���VMP�Ƿ�֧������ĺϳɷ��
                u16 wError = 0;
                if ( IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError) ) 
                {
					//VMPȷ����������� m_tVmpEqp [01/13/2006-zbq]
					m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
					m_tVmpEqp.SetConfIdx( m_byConfIdx );
					
                    u8 byMtNum = (u8)m_tConfAllMtInfo.GetAllMtNum();

					memset( &tVmpParam, 0 ,sizeof(tVmpParam) );
					tVmpParam.SetVMPAuto( TRUE );
					tVmpParam.SetVMPBrdst( m_tConf.m_tStatus.m_tVMPParam.IsVMPBrdst() );
					tVmpParam.SetVMPStyle( GetVmpDynStyle(byMtNum) );
                    tVmpParam.SetVMPSchemeId(m_tConf.m_tStatus.m_tVMPParam.GetVMPSchemeId());
					tVmpParam.SetIsRimEnabled((u8)m_tConf.m_tStatus.m_tVMPParam.GetIsRimEnabled());
                }
                else
                {
                    // N+1�����Զ�����ϳɻָ�ʧ�ܣ�����ǰ���е�״̬ [01/16/2007-zbq]
                    if ( CONF_CREATE_NPLUS == m_byCreateBy ) 
                    {
                        NotifyMcsAlarmInfo( cSrcMsg.GetSrcSsnId(), ERR_MCU_ALLIDLEVMP_NO_SUPPORT_NPLUS );
                        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because all the idle vmp can't support the VMP style!\n", m_tConf.GetConfName() );
                        
                        // N+1���ݻع������Ļ��飬���������VMP����С�ڱ��ˣ����ָ�����ϳɣ�
                        // ��Ҫ�����ص����ݡ����򣬸û����ڱ����ٴλָ���ᵼ�»���״̬���ҡ�[12/28/2006-zbq]
                        if ( MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() )
                        {
                            m_tConf.m_tStatus.SetVmpBrdst( FALSE );
                            m_tConf.SetHasVmpModule( FALSE );
                            m_tConf.m_tStatus.SetVMPMode( CONF_VMPMODE_NONE );
                            
                            //���ͨ��
                            for( u8 byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
                            {
                                m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );
                            }
                        }
                        else
                        {
                            //N+1���ݻָ����飬�����ڵ�VMP�������߼��ͱ�����������������
                            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf.%s created by NPlus error which is impossible, check it !\n", m_tConf.GetConfName() );
                        }                                
                    }
                    else
                    {
						RlsAllBasForConf();
                        //��������Ĵ����������ڵ�VMP�������߼��ͱ�������������������������[01/16/2007-zbq]
                        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf.%s created which is impossible, check it !\n", m_tConf.GetConfName() );
                        wErrCode = ERR_MCU_NOIDLEVMP;
                        g_cMcuVcApp.RemoveConf( m_byConfIdx );    
                        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because no idle vmp!\n", m_tConf.GetConfName() );
                        return FALSE;
                    }
                }
			}
            
            TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_START;//��ռ��,��ʱ��δ�ɹ��ٷ���
			tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpParam;
			tPeriEqpStatus.SetConfIdx( m_byConfIdx );
			g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			
			m_tConf.m_tStatus.SetVmpStyle( tVmpParam.GetVMPStyle() );
			
			u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType; //��ȡ������
			u8 byMaxVmpMem = 0;
			if (VMP != byVmpSubType &&
				VMP_8KE != byVmpSubType) //��VMP����Ա����Ϊ20 // [2/28/2010 xliang]  
			{
				byMaxVmpMem = MAXNUM_MPUSVMP_MEMBER;
			}
			else
			{
				byMaxVmpMem = MAXNUM_SDVMP_MEMBER;
			}
			//���ͨ��
			for( u8 byLoop = 0; byLoop < byMaxVmpMem; byLoop++ )
			{
				m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );
			}

			//��ʼ����ϳ�        
            if (!tVmpParam.IsVMPAuto() ||
                (tVmpParam.IsVMPAuto() && tVmpParam.GetVMPStyle() != VMP_STYLE_NONE))
            {
				AdjustVmpParam(&tVmpParam, TRUE);
            }*/
		}
        else if( CONF_CREATE_MT == m_byCreateBy ) //�ն˴��᲻ֱ�ӷ���
        {
            m_tConf.m_tStatus.SetVMPMode(CONF_VMPMODE_NONE);
        }
		else if ( CONF_CREATE_NPLUS == m_byCreateBy )
        {
            // N+1���ݴ�����û��VMP������£��ϱ��Ѿ����������˴�ֱ��������
        }
        else
		{
			RlsAllBasForConf();
            g_cMcuVcApp.RemoveConf( m_byConfIdx );
			wErrCode = ERR_MCU_NOIDLEVMP;
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because no idle vmp!\n", m_tConf.GetConfName() );
			return FALSE;
		}				
	}

	// [2013/05/07 chenbing]  
	if ( m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo()
		&& m_tConf.m_tStatus.IsOngoing()
	   )
	{
		u8 byHduMode = HDUCHN_MODE_FOUR;
		for(u8 byLoopChnIdx = 0; byLoopChnIdx < (HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL; byLoopChnIdx++)
		{
			if(m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].IsValid())
			{
				u8 byHduEqpId = m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_byHduEqpId;
				u8 byParentChnnlIdx = m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_byChIdx;
			
				CServMsg cSMg;
				cSMg.SetEqpId(byHduEqpId);
				cSMg.SetChnIndex(byParentChnnlIdx);
				cSMg.SetMsgBody((u8 *)&byHduMode, sizeof(u8));
				//�л�HDU2��Ӧ��ͨ��Ϊ�ķ��
				ChangeHduVmpMode(cSMg);
			}
		}
	}

//     //�ѻ���ָ��浽������
//     if( !g_cMcuVcApp.AddConf( this ) )
//     {
// 		RlsAllBasForConf();
//         wErrCode = ERR_MCU_CONFOVERFLOW;
//         ConfLog( FALSE, "Conference %s create failed because of full instance!\n", m_tConf.GetConfName() );
//         return FALSE;
//     }

    return TRUE;
}

/*====================================================================
    ������      ��GetConfExInfoFromMsg
    ����        ���Ӵ�����Ϣ���TConfInfoEx����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/12/28    4.0         chendaiwei    ����
====================================================================*/
BOOL32 CMcuVcInst::GetConfExInfoFromMsg(CServMsg &cServMsg, TConfInfoEx &tConfInfoEx)
{
	TConfInfo tConfInfo = *(TConfInfo*)(cServMsg.GetMsgBody());
    u16 wAliasBufLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
    s8 *pszAliaseBuf = (char*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16));
    
	u32 nLen = sizeof(TConfInfo)+sizeof(u16)+wAliasBufLen;

	TConfAttrb tConfAttrb = tConfInfo.GetConfAttrb();
	if(tConfAttrb.IsHasTvWallModule())
		nLen += sizeof(TMultiTvWallModule);
	if(tConfAttrb.IsHasVmpModule())
			nLen += sizeof(TVmpModule);

	//VCS��Ϣ[12/27/2011 chendaiwei]
	u8 * pbyVCSInfoBuf = cServMsg.GetMsgBody()+nLen;
	if (VCS_CONF == tConfInfo.GetConfSource())
	{	
		u8 byIsSupportHDTW = *pbyVCSInfoBuf++;
		if (byIsSupportHDTW)
		{
			pbyVCSInfoBuf += sizeof(THDTvWall);
		}
		
		u8 byHduNum = *pbyVCSInfoBuf++;
		if (byHduNum)
		{
			pbyVCSInfoBuf += byHduNum * sizeof(THduModChnlInfo);
		}
		
		u8 bySMCUExist = *pbyVCSInfoBuf++;
		if (bySMCUExist)
		{
			wAliasBufLen = htons(*(u16*)pbyVCSInfoBuf);
			pbyVCSInfoBuf += sizeof(u16);
			wAliasBufLen = ntohs(wAliasBufLen);
			pbyVCSInfoBuf += wAliasBufLen;					
		}
		
		// ����ģ���в�����������Ϣ
		pbyVCSInfoBuf++;
		//zhouyiliang 20100820 ���ӱ����ն�
		pbyVCSInfoBuf += sizeof(TMtAlias);
		
		// [4/20/2011 xliang] VCAutoMode
		pbyVCSInfoBuf ++;
	}

	//��������ģ�年ѡ��Ϣ[12/26/2011 chendaiwei]
	TConfInfoEx tOriginalEx = GetConfInfoExFromConfAttrb(tConfInfo);

	if( ntohs(*(u16*)pbyVCSInfoBuf) != 0 )
	{
		u16 wConfInfoExSize = 0;
		
		BOOL32 bExistUnknowType = FALSE;
		UnPackConfInfoEx(tConfInfoEx,pbyVCSInfoBuf,wConfInfoExSize,bExistUnknowType);
		TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];
		u8   byAudioCapNum = tConfInfoEx.GetAudioTypeDesc(atAudioTypeDesc);
		if (byAudioCapNum > 0)//������չ��Ϣ������Ƶ������Ϣ����Ҫ����չ��Ϣ��ȥ����
		{
			tOriginalEx.SetAudioTypeDesc(atAudioTypeDesc,byAudioCapNum);
		}
		tConfInfoEx.AddCapExInfo(tOriginalEx);

		return TRUE;
	}
	else
	{
		tConfInfoEx = tOriginalEx;

		return FALSE;
	}
}

void CMcuVcInst::NPlusVmpParamCoordinate (CServMsg &cServMsg)
{
	u8 *pBuf = cServMsg.GetMsgBody();
	TConfInfo tConfInfo = *(TConfInfo*)pBuf;
	pBuf+= sizeof(TConfInfo);

    u16 wAliasBufLen = ntohs( *(u16*)pBuf );
	pBuf+= sizeof(wAliasBufLen);
	pBuf+=wAliasBufLen;

	
	TConfAttrb tConfAttrb = tConfInfo.GetConfAttrb();
	if(tConfAttrb.IsHasTvWallModule())
		pBuf += sizeof(TMultiTvWallModule);
	if(tConfAttrb.IsHasVmpModule())
		pBuf += sizeof(TVmpModule);
	
	if (VCS_CONF == tConfInfo.GetConfSource())
	{	
		u8 byIsSupportHDTW = *pBuf++;
		if (byIsSupportHDTW)
		{
			pBuf += sizeof(THDTvWall);
		}
		
		u8 byHduNum = *pBuf++;
		if (byHduNum)
		{
			pBuf += byHduNum * sizeof(THduModChnlInfo);
		}
		
		u8 bySMCUExist = *pBuf++;
		if (bySMCUExist)
		{
			wAliasBufLen = htons(*(u16*)pBuf);
			pBuf += sizeof(u16);
			wAliasBufLen = ntohs(wAliasBufLen);
			pBuf += wAliasBufLen;					
		}
		
		// ����ģ���в�����������Ϣ
		pBuf++;
		//zhouyiliang 20100820 ���ӱ����ն�
		pBuf += sizeof(TMtAlias);
		
		// [4/20/2011 xliang] VCAutoMode
		pBuf ++;
	}
	
	if( tConfInfo.HasConfExInfo())
	{
		u16 wConfInfoExSize = 0;
		TConfInfoEx tConfInfoEx;
		BOOL32 bExistUnknowType = FALSE;
		UnPackConfInfoEx(tConfInfoEx,pBuf,wConfInfoExSize,bExistUnknowType);
		
		pBuf+=wConfInfoExSize;
	}
	
	//���N+1���ᣬ��Ϣ�帽��SMCU INFO[11/20/2012 chendaiwei]
	if(m_byCreateBy == CONF_CREATE_NPLUS )
	{
		TSmcuCallnfo atSmcuCallInfo[MAXNUM_SUB_MCU];
		pBuf += sizeof(atSmcuCallInfo);

		TNPlusVmpParam tNlpusVmpparam[MAXNUM_PERIEQP];
		u8 byVmpNum = *pBuf;
		pBuf++;

		TNplusVmpModule atVmpModule[MAXNUM_PERIEQP];
		memcpy(&tNlpusVmpparam[0],pBuf,sizeof(tNlpusVmpparam[0])*byVmpNum);
		
		for ( u8 byVmpNumIdx = 0; byVmpNumIdx < byVmpNum; byVmpNumIdx ++ )
		{
			u8 byIdleEqpId = 0;

			u8 byEqualAbilityVmpId = 0;
			u8 bySlightlyLargerAbilityVmpId = 0;
			u8 byMinAbilityVmpId = 0;

			if(!IsVmpSupportNplus(tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam.m_byVmpSubType,tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam.m_byVMPStyle,byEqualAbilityVmpId,bySlightlyLargerAbilityVmpId,byMinAbilityVmpId))
			{
				continue;
			}
			
			//1ģʽ
			if( g_cNPlusApp.GetLocalNPlusState() ==MCU_NPLUS_SLAVE_IDLE
				|| MCU_NPLUS_SLAVE_SWITCH ==  g_cNPlusApp.GetLocalNPlusState())
			{
				if(byEqualAbilityVmpId != 0)
				{
					byIdleEqpId = byEqualAbilityVmpId;
				}
				else
				{
					if( bySlightlyLargerAbilityVmpId != 0)
					{
						byIdleEqpId = bySlightlyLargerAbilityVmpId;
					}
				}
			}
			//Nģʽ
			else
			{
				if(byMinAbilityVmpId != 0)
				{
					byIdleEqpId = byMinAbilityVmpId;
				}
				else if (byEqualAbilityVmpId != 0)
				{
					byIdleEqpId = byEqualAbilityVmpId;
				}
				else if ( bySlightlyLargerAbilityVmpId!= 0)
				{
					byIdleEqpId = bySlightlyLargerAbilityVmpId;
				}
			}
			
			if(byIdleEqpId == 0)
			{
				ConfPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"vmpStyle.%d VmpSubType.%d Find Matached Vmp failed!\n",
					tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam.m_byVMPStyle,
					tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam.m_byVmpSubType);
				continue;
			}
			else
			{
				ConfPrint(LOG_LVL_DETAIL,MID_MCU_NPLUS,"VmpStyle.%d SubType.%d Find Matached Vmp<%d> success!\n",
					tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam.m_byVMPStyle,
					tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam.m_byVmpSubType,
					byIdleEqpId);
			}

			atVmpModule[byVmpNumIdx].m_byEqpId = byIdleEqpId;

			TVMPParam_25Mem tVmpParam;
			TVMPMember tVmpMember;
			memcpy(&tVmpParam,&tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam,sizeof(TVmpBasicParam)-sizeof(u8));

			atVmpModule[byVmpNumIdx].m_byConfIdx = m_byConfIdx;
			
			//Auto����Ҫ����VMP��Ա
			if( tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam.m_byVMPAuto != 1)
			{
				for( u8 byChnIdx = 0; byChnIdx < MAXNUM_VMP_MEMBER; byChnIdx++ )
				{
					TMtAlias tTmpAlias;
					u8 byVmpMemberId = 0;
					u8 byMemberType = tNlpusVmpparam[byVmpNumIdx].m_tVmpMemer.m_tVmpChnnlInfo[byChnIdx].m_byMemberType;
					tTmpAlias.m_AliasType = mtAliasTypeTransportAddress;
					tTmpAlias.m_tTransportAddr = tNlpusVmpparam[byVmpNumIdx].m_tVmpMemer.m_tVmpChnnlInfo[byChnIdx].m_tMtInVmp.GetMtAddr();
					
					byVmpMemberId = m_ptMtTable->GetMtIdByAlias(&tTmpAlias);
					
					//byVmpMemberId == 0��ʾ�޳�Ա������������Ȼ����  [5/2/2013 chendaiwei]
					atVmpModule[byVmpNumIdx].m_byMemberType[byChnIdx] = byMemberType;

					TMt tMt;
					BOOL32 bValid = FALSE;
                    switch (byMemberType)
                    {
                    case VMP_MEMBERTYPE_SPEAKER:
						//��������
						tMt = m_tConf.GetSpeaker();                                
                        bValid = TRUE;
                        break;
                    case VMP_MEMBERTYPE_CHAIRMAN:
                        // ������ϯ
						tMt = m_tConf.GetChairman();                                
                        bValid = TRUE;
                        break;
                    case VMP_MEMBERTYPE_POLL:		// ��ѯ������ʱ�϶�û����ѯ
                    case VMP_MEMBERTYPE_VMPCHLPOLL:	// vmp��ͨ����ѯ������ʱ�϶�û��vmp��ͨ����ѯ
                    case VMP_MEMBERTYPE_DSTREAM:	// ˫�����棬����ʱ�϶�û��˫��Դ
                        tMt.SetNull();
                        bValid = TRUE;
                        break;
                    default:
                        ConfPrint( LOG_LVL_WARNING, MID_MCU_NPLUS,  "[NplusVmpParamCoordinate]Invalid member type %d\n", 
							byMemberType);        
                        bValid = FALSE;
                    }
                    if (bValid && !tMt.IsNull())
                    {
						atVmpModule[byVmpNumIdx].m_byMtId[byChnIdx] = tMt.GetMtId();
						tVmpMember.SetMemberTMt(tMt);
                    }
					else
					{
						atVmpModule[byVmpNumIdx].m_byMtId[byChnIdx] = byVmpMemberId;
						tVmpMember.SetMemberTMt(m_ptMtTable->GetMt(byVmpMemberId));
					}
                
					tVmpMember.SetMemberType(byMemberType);
					tVmpParam.SetVmpMember(byChnIdx,tVmpMember);
				}
			}

			//��¼��ǰռ��VMP MOdule��Ϣ
			g_cMcuVcApp.NplusSaveVmpModuleByConfIdx(m_byConfIdx,&atVmpModule[byVmpNumIdx],1);

			CServMsg cMsg;
			cMsg.SetEqpId(byIdleEqpId);
			cMsg.SetEventId(MCS_MCU_STARTVMP_REQ);
			cMsg.SetMsgBody( (u8 *)&tVmpParam, 	sizeof(TVMPParam_25Mem) );
			ProcStartVmpReq(cMsg);
		}

		g_cMcuVcApp.NplusSaveVmpModuleByConfIdx(m_byConfIdx,atVmpModule,byVmpNum);
	}

	return;
}


/*====================================================================
    ������      ��ConfDataCoordinate
    ����        �����������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	07/12/25    4.0         �ű���          ����
    08/11/18    4.5         �ű���          ����ˢ����������Ŀ��֧��
====================================================================*/
void CMcuVcInst::ConfDataCoordinate(CServMsg &cServMsg, u8 &byMtNum, u8 &byCreateMtId)
{
    //����������
    TCapSupport tCapSupport = m_tConf.GetCapSupport();
    tCapSupport.SetSupportH224( TRUE );	
    if(m_tConf.GetConfAttrb().GetDataMode() != CONF_DATAMODE_VAONLY)
    {
        tCapSupport.SetSupportT120(TRUE);
    }
    else
    {
        tCapSupport.SetSupportT120(FALSE);
    }
    
    //zbq[01/04/2008] ǿ�����FEC
    TCapSupportEx tCapEx = m_tConf.GetCapSupportEx();
    tCapEx.SetVideoFECType(FECTYPE_NONE);
    tCapEx.SetAudioFECType(FECTYPE_NONE);
    tCapEx.SetDVideoFECType(FECTYPE_NONE);
    m_tConf.SetCapSupportEx(tCapEx);

    //��ֹ���˫����ʽ������ʽ��һ��
    if(VIDEO_DSTREAM_MAIN == tCapSupport.GetDStreamType())
    {
        tCapSupport.SetDStreamType(VIDEO_DSTREAM_MAIN);
    }

	if(VIDEO_DSTREAM_MAIN_H239 == tCapSupport.GetDStreamType())
	{
		tCapSupport.SetDStreamType(VIDEO_DSTREAM_MAIN_H239);
	}

    tCapSupport.SetEncryptMode(m_tConf.GetConfAttrb().GetEncryptMode());		
    tCapSupport.SetSupportMMcu(m_tConf.GetConfAttrb().IsSupportCascade()); 
    
    // guzh [6/6/2007] ���������ʱ���
    TSimCapSet  tSimCapSet;
    tSimCapSet = tCapSupport.GetMainSimCapSet();
    if ( tSimCapSet.GetVideoMaxBitRate() == 0 )
    {
        tSimCapSet.SetVideoMaxBitRate(m_tConf.GetBitRate());
        tCapSupport.SetMainSimCapSet(tSimCapSet);
    }
    tSimCapSet = tCapSupport.GetSecondSimCapSet();
    if ( tSimCapSet.GetVideoMediaType() != MEDIA_TYPE_NULL &&
        tSimCapSet.GetVideoMaxBitRate() == 0 )
    {
        tSimCapSet.SetVideoMaxBitRate(m_tConf.GetBitRate());
        tCapSupport.SetSecondSimCapSet(tSimCapSet);
    }
    
    m_tConf.SetCapSupport( tCapSupport );

    //�����ն��б�
    
    //����������ն˱���
    TMtAlias *ptMtH323Alias = NULL;
    TMtAlias *ptMtE164Alias = NULL;
    TMtAlias *ptMtAddr = NULL;

    u8 byLoop = 0;
    u8 byMtId = 0;
    u8 byMtType = TYPE_MT;
    s8 *pszAliaseBuf = NULL;
    u16 wAliasBufLen  = 0;
    u16 awMtDialRate[MAXNUM_CONF_MT];
    s8 *pszMsgBuf = NULL;
    TMtAlias tMtAliasArray[MAXNUM_CONF_MT];
	BOOL32 bInMtTable = TRUE ; // xliang [12/26/2008] 
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();

    wAliasBufLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
    pszAliaseBuf = (char*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16));
    
    UnPackTMtAliasArray( pszAliaseBuf, wAliasBufLen, &m_tConf, 
                         tMtAliasArray, awMtDialRate, byMtNum );

	//�õ������ߵı���
	if( CONF_CREATE_MT == m_byCreateBy )
	{

        u32 nLen = sizeof(TConfInfo)+sizeof(u16)+wAliasBufLen;
		if(tConfAttrb.IsHasTvWallModule())
			nLen += sizeof(TMultiTvWallModule);
		if(tConfAttrb.IsHasVmpModule())
			nLen += sizeof(TVmpModule);
		
		// wConfExInfoLength�����п���Ϊ0���ն˴���ʱ���������u16[11/14/2012 chendaiwei]
		u16 wConfExInfoLength = ntohs( *(u16*)(cServMsg.GetMsgBody() + nLen));
		nLen = nLen + wConfExInfoLength + sizeof(u16);
	
		if( cServMsg.GetMsgBodyLen() > nLen )
		{
			ptMtH323Alias = (TMtAlias *)(cServMsg.GetMsgBody()+nLen);
			ptMtE164Alias = (TMtAlias *)(cServMsg.GetMsgBody()+nLen)+1;
			ptMtAddr = (TMtAlias *)(cServMsg.GetMsgBody()+nLen)+2;
			byMtType = *(cServMsg.GetMsgBody()+nLen+sizeof(TMtAlias)*3);
			bInMtTable = *(BOOL32 *)(cServMsg.GetMsgBody()+nLen+sizeof(TMtAlias)*3+sizeof(u8));// xliang [12/26/2008] 
		}
	}

	//�����ն����״̬ 
	m_tConfAllMtInfo.RemoveAllJoinedMt();

	for( byLoop=1; byLoop<=byMtNum; byLoop++)
	{
		//�ն˺������ʱ�����۴��ỹ�Ǽ������л��飬������������DRI������Ϣ
		if( CONF_CREATE_MT == m_byCreateBy && 1 == byLoop )
		{
			if( byMtType == TYPE_MCU )
			{
				byMtId = AddMt( tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], CONF_CALLMODE_NONE );
			}
			else
			{
				byMtId = AddMt( tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], m_tConf.m_tStatus.GetCallMode() );
			}
			           
			if(!bInMtTable)
			{
				m_byMtIdNotInvite = byMtId; // xliang [12/26/2008] �������ն��Ժ�Ӧ��Ϊ����MT
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ConfdataCoordinate] Mt.%u should not be in Invite table\n", m_byMtIdNotInvite);
			}
				// zbq [12/19/2007] ֱ�Ӵ����ն����ڴ˲���IP������E164��������µ�©��
            if ( ptMtAddr && tMtAliasArray[byLoop-1].m_AliasType != mtAliasTypeTransportAddress )
            {
                m_ptMtTable->SetMtAlias( byMtId, ptMtAddr );
                m_ptMtTable->SetIPAddr( byMtId, ptMtAddr->m_tTransportAddr.GetIpAddr() );                    
            }

            // xsl [11/8/2006] ������ն˼�����1
			// xliang [2/4/2009] ������MT����MCU, ��MCU�������ն˼����������+1
			u8 byDriId = cServMsg.GetSrcDriId();
//			u16 wExtraNum = (byMtType == TYPE_MCU)? 1: 0;
//			g_cMcuVcApp.IncMtAdpMtNum( byDriId, m_byConfIdx, byMtId, wExtraNum);
            g_cMcuVcApp.IncMtAdpMtNum( byDriId, m_byConfIdx, byMtId, byMtType);
            m_ptMtTable->SetDriId(byMtId, byDriId);
// 			if(byMtType == TYPE_MCU)
// 			{
// 				//�������ն˼�����Ҫ+1
// 				//��mtadplib�Ƕ���ͨ���жϣ����Կ϶��ж���2�����������ռ��
// 				g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum++;
// 			}
		}
		else
		{
			byMtId = AddMt( tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], m_tConf.m_tStatus.GetCallMode() );
		}
					
		if( CONF_CALLMODE_TIMER == m_tConf.m_tStatus.GetCallMode() && 
			DEFAULT_CONF_CALLTIMES != m_tConf.m_tStatus.GetCallTimes() )
		{
			m_ptMtTable->SetCallLeftTimes( byMtId, (m_tConf.m_tStatus.GetCallTimes() - 1) );
		}

		if( CONF_CREATE_MT == m_byCreateBy && 1 == byLoop )
		{
			byCreateMtId = byMtId;

			if(ptMtH323Alias != NULL)
			{
				m_ptMtTable->SetMtAlias(byMtId, ptMtH323Alias);
			}
			if(ptMtE164Alias != NULL)
			{
				m_ptMtTable->SetMtAlias(byMtId, ptMtE164Alias);
			}
            if(ptMtAddr != NULL)
            {
                m_ptMtTable->SetMtAlias(byMtId, ptMtAddr);
            }
			if(byMtType == TYPE_MCU) 
			{
				if( m_tConf.GetConfAttrb().IsSupportCascade() )
				{
					m_ptMtTable->SetMtType(byMtId, MT_TYPE_MMCU);
				}
				else
				{
					SetInOtherConf( TRUE,byMtId );
					m_ptMtTable->SetMtType(byMtId, MT_TYPE_MT);
				}
				
				//�����ϼ�MCU������Ϊ�������նˣ��Ͽ����Զ�ɾ�����б�
				m_ptMtTable->SetNotInvited( byMtId, TRUE );
                //m_ptMtTable->SetAddMtMode(byMtId, ADDMTMODE_MTSELF);
			}
			else
			{
				m_ptMtTable->SetMtType(byMtId, MT_TYPE_MT);
			}

            // guzh[5/23/2007]����ֻ�ܽ�����Ϊ�������ն�
            // ��ӣ�����ᵼ�����滭��ϳɳ�Ա�жϳ���.
            //m_tConfAllMtInfo.AddJoinedMt(byMtId);
            m_tConfAllMtInfo.AddMt(byMtId);
		}
	}

	//������ϯ
    TMtAlias tMtAlias;
	tMtAlias = m_tConf.GetChairAlias();
	byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAlias );
	m_tConf.SetNoChairman();
	if( byMtId > 0 )
	{
        m_tConf.SetChairman( m_ptMtTable->GetMt(byMtId) );
	}

	//��������
	tMtAlias = m_tConf.GetSpeakerAlias();
	byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAlias );
	m_tConf.SetNoSpeaker();
	if( byMtId > 0)
	{
        m_tConf.SetSpeaker( m_ptMtTable->GetMt(byMtId) );
	}

	pszMsgBuf = (char*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16) + wAliasBufLen);

	//�������ǽģ��
	if( tConfAttrb.IsHasTvWallModule() )
	{
        TPeriEqpStatus tTWStatus;
		TMultiTvWallModule *ptMultiTvWallModule = (TMultiTvWallModule *)pszMsgBuf;
        m_tConfEqpModule.SetMultiTvWallModule( *ptMultiTvWallModule );
        m_tConfEqpModule.SetTvWallInfo( *ptMultiTvWallModule );

        TTvWallModule tTvWallModule;
        u8 byMtIndex = 0;
        u8 byMemberType = 0;

		CServMsg CTvwEqpStatusMsg;
        for( u8 byTvLp = 0; byTvLp < ptMultiTvWallModule->GetTvModuleNum(); byTvLp++ )
        {
            ptMultiTvWallModule->GetTvModuleByIdx(byTvLp, tTvWallModule);

			//zjl[20091208]����ģ������������hduԤ���������ն˳�Ա
			u8 byEqpType  = tTvWallModule.GetTvEqp().GetEqpType();			
			u8 byTvWallId = tTvWallModule.GetTvEqp().GetEqpId();
			
			TTvwMember tTvwMbr;

			if (EQP_TYPE_HDU_SCHEME == byEqpType)
			{
				u8 byHduSchemeNum = 0;
				THduStyleInfo atHduStyleInfoTable[MAX_HDUSTYLE_NUM];
				u16 wRet = g_cMcuAgent.ReadHduSchemeTable(&byHduSchemeNum, atHduStyleInfoTable);
				if (SUCCESS_AGENT == wRet)
				{
					THduChnlInfo  atHduChnlInfo[MAXNUM_TVWALL_CHNNL_INSMOUDLE];

					u16 wTempLoop = 0;
					for ( wTempLoop = 0; wTempLoop < byHduSchemeNum && wTempLoop < MAX_HDUSTYLE_NUM; wTempLoop++)
					{
						if (atHduStyleInfoTable[wTempLoop].GetStyleIdx() == byTvWallId)
						{
							atHduStyleInfoTable[wTempLoop].GetHduChnlTable(atHduChnlInfo);
							break;
						}
					}
					
					if (wTempLoop >= byHduSchemeNum || byHduSchemeNum > MAX_HDUSTYLE_NUM)
					{
						continue;
					}

					//atHduStyleInfoTable[byTvWallId-1].GetHduChnlTable(atHduChnlInfo);
					
					for (u8 byChnl = 0; byChnl < MAXNUM_TVWALL_CHNNL_INSMOUDLE; byChnl++)
					{
						byMtIndex    = tTvWallModule.m_abyTvWallMember[byChnl]; 
						byMemberType = tTvWallModule.m_abyMemberType[byChnl];

						if (!IsValidHduChn(atHduChnlInfo[byChnl].GetEqpId(), atHduChnlInfo[byChnl].GetChnlIdx()))
						{
							continue;
						}
						
						if(!g_cMcuVcApp.GetPeriEqpStatus(atHduChnlInfo[byChnl].GetEqpId(), &tTWStatus))
						{
							//ConfLog(FALSE, "[ConfDataCoordinate] unexpected PeriEqpStatus! HDU_SCHEME: %d, EQPID:%d,CHNLID:%d\n",
							//	byTvWallId, atHduChnlInfo[byChnl].GetEqpId(), atHduChnlInfo[byChnl].GetChnlIdx());
							continue;
						}
					
						tTvwMbr = tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()];
						
						//20100702_tzy �жϵ�ǰ����ǽͨ���Ƿ���������ռ�ã������ռ������ʹ�ø�ͨ��
						if (0 != tTvwMbr.GetConfIdx()
							&& tTvwMbr.GetConfIdx() != m_byConfIdx)
						{
							ConfPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ConfDataCoordinate] TW<EqpId:%d, ChnId:%d> has been used by other conf:%d!\n", 
								atHduChnlInfo[byChnl].GetEqpId(), 
								atHduChnlInfo[byChnl].GetChnlIdx(),
								tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].GetConfIdx());
							continue;
						}
						
						if (byMtIndex > 0 )
						{
							if( byMtIndex <= MAXNUM_CONF_MT )
							{
								byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAliasArray[byMtIndex-1] );
								if (byMtId > 0)
								{
									TMt tMt = m_ptMtTable->GetMt( byMtId );
									m_tConfEqpModule.SetTvWallMemberInTvInfo( byTvWallId, byChnl, tMt);		
									
									// [2013/04/27 chenbing] �˶δ��������壬�ն˴��߼�ͨ��������ChangeHduSwitch����,�ɿ���ɾ��
									// �����ն���ǽ��, �ȰѾɵ���ʾ��ǽ
									if (tTvwMbr.GetConfIdx() == 0 || tTvwMbr.GetConfIdx() == m_byConfIdx)
									{
										RefreshMtStatusInTw( (TMt)tTvwMbr, FALSE, TRUE);
										
										tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].byMemberType = byMemberType;
										tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].SetConfIdx(m_byConfIdx);
										
										// [11/1/2011 liuxu] Ԥ���ǽ,��ʾ��ǽ����
										tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].SetMt(tMt);
										
										// ֻ�е���ǽ����, �Ż������ն���ʾ��ǽ����
										if (tTWStatus.m_byOnline)
										{
											RefreshMtStatusInTw(tMt, TRUE, TRUE);
										}										
									}
								}
							}
							// ����� 193��˵�����������淽ʽ
							else if (byMtIndex == MAXNUM_CONF_MT + 1)
							{   
								TMt tMt;
								BOOL32 bValid = FALSE;
								switch (byMemberType)
								{
								case TW_MEMBERTYPE_SPEAKER:
									//��������
									tMt = m_tConf.GetSpeaker();                                
									bValid = TRUE;
									break;

								case TW_MEMBERTYPE_CHAIRMAN:
									// ������ϯ
									tMt = m_tConf.GetChairman();                                
									bValid = TRUE;
									break;

								case TW_MEMBERTYPE_POLL:
									// ��ѯ������ʱ�϶�û����ѯ
									tMt.SetNull();
									bValid = TRUE;
									break;

									//��������ģ��hduѡ��vmp��������vmpʱ����������hdu
								case TW_MEMBERTYPE_SWITCHVMP:
								    tMt.SetNull();
									bValid = TRUE;
									break;

								case TW_MEMBERTYPE_DOUBLESTREAM:
									tMt = m_tDoubleStreamSrc;
									bValid = TRUE;
									break;

								default:
									ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Invalid member type %d for idx %d in CreateConf TvWall Module\n", 
										byMemberType, 
										byLoop);        
									bValid = FALSE;
								}
								if (bValid)
								{
									// ������������ն��������ᴥ����ϯ�����˱��
									m_tConfEqpModule.SetTvWallMemberInTvInfo( tTvWallModule.m_tTvWall.GetEqpId(), byChnl, tMt );
									tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].byMemberType = byMemberType;
									tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].SetConfIdx(m_byConfIdx);
								}                            
							}
							else
							{
								ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Invalid MtIndex :%d in CreateConf TvWall Module\n", byMtIndex);
							}
						}

						g_cMcuVcApp.SetPeriEqpStatus(atHduChnlInfo[byChnl].GetEqpId(), &tTWStatus);

						if(tTWStatus.m_byOnline)
						{
							// ֪ͨ���
							CTvwEqpStatusMsg.SetMsgBody((u8*)&tTWStatus, sizeof(tTWStatus));
							SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, CTvwEqpStatusMsg);
						}
					}
				}
				else
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "[ConfDataCoordinate] ReadHduSchemeTable failed!\n");
				}
			}
			else
			{
				g_cMcuVcApp.GetPeriEqpStatus(tTvWallModule.m_tTvWall.GetEqpId(), &tTWStatus);

				for( byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++ )
				{
					byMtIndex = tTvWallModule.m_abyTvWallMember[byLoop]; 
					byMemberType = tTvWallModule.m_abyMemberType[byLoop];
					tTvwMbr = tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop];
					
					if( byMtIndex > 0 )//������1Ϊ��׼
					{
						if( byMtIndex <= MAXNUM_CONF_MT )
						{
							byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAliasArray[byMtIndex-1] );
							if( byMtId > 0 )
							{
								TMt tMt = m_ptMtTable->GetMt( byMtId );
								m_tConfEqpModule.SetTvWallMemberInTvInfo( tTvWallModule.m_tTvWall.GetEqpId(), byLoop, tMt );
								
								if (tTvwMbr.GetConfIdx() == 0 || tTvwMbr.GetConfIdx() == m_byConfIdx )
								{
									// [11/1/2011 liuxu] ��ֹ��ͻ
									RefreshMtStatusInTw((TMt)tTvwMbr, FALSE, FALSE);
									
									// ���� [5/11/2006] ����֧��ָ�����淽ʽ
									// tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = TW_MEMBERTYPE_MCSSPEC;
									tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byMemberType;
									tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
									
									// [11/1/2011 liuxu] Ԥ���ǽ,��ʾ��ǽ����
									tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetMt(tMt);
									
									// ֻ�е���ǽ����, �Ż������ն���ʾ��ǽ����
									if (tTWStatus.m_byOnline)
									{
										RefreshMtStatusInTw(tMt, TRUE, FALSE);
									}
								}
							}
						}
						// ����� 193��˵�����������淽ʽ
						else if (byMtIndex == MAXNUM_CONF_MT + 1)
						{   
							TMt tMt;
							BOOL32 bValid = FALSE;
							switch (byMemberType)
							{
							case TW_MEMBERTYPE_SPEAKER:
								//��������
								tMt = m_tConf.GetSpeaker();                                
								bValid = TRUE;
								break;
							case TW_MEMBERTYPE_CHAIRMAN:
								// ������ϯ
								tMt = m_tConf.GetChairman();                                
								bValid = TRUE;
								break;
							case TW_MEMBERTYPE_POLL:
								// ��ѯ������ʱ�϶�û����ѯ
								tMt.SetNull();
								bValid = TRUE;
								break;

							default:
								ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Invalid member type %d for idx %d in CreateConf TvWall Module\n", 
									byMemberType, 
									byLoop);        
								bValid = FALSE;
							}
							if (bValid)
							{
								// ������������ն��������ᴥ����ϯ�����˱��
								m_tConfEqpModule.SetTvWallMemberInTvInfo( tTvWallModule.m_tTvWall.GetEqpId(), byLoop, tMt );

								if (tTvwMbr.GetConfIdx() == 0 || tTvwMbr.GetConfIdx() == m_byConfIdx )
								{
									tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byMemberType;
									tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
								}
							}                            
						}
						else
						{
							ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Invalid MtIndex :%d in CreateConf TvWall Module\n", byMtIndex);
						}
					}

					g_cMcuVcApp.SetPeriEqpStatus(tTvWallModule.m_tTvWall.GetEqpId(), &tTWStatus);
					
					if(tTWStatus.m_byOnline)
					{
						// ֪ͨ���
						CTvwEqpStatusMsg.SetMsgBody((u8*)&tTWStatus, sizeof(tTWStatus));
						SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, CTvwEqpStatusMsg);
					}
				}
			}
            
        }
		pszMsgBuf += sizeof(TMultiTvWallModule);
	}
    else
    {
        TMultiTvWallModule tMultiTvWallModule;
        memset( &tMultiTvWallModule, 0, sizeof(tMultiTvWallModule) );
        m_tConfEqpModule.SetMultiTvWallModule( tMultiTvWallModule );
	
        for( byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++ )
		{
			m_tConfEqpModule.m_tTvWallInfo[byLoop].ClearTvMember();
		}
    }

	//�ݴ�����Ļ���ϳ�ģ�壨20������£�[3/15/2013 chendaiwei]
	TVmpModule tVmpModule;
	if( tConfAttrb.IsHasVmpModule() )
	{
		memcpy(&tVmpModule,pszMsgBuf,sizeof(tVmpModule));

		pszMsgBuf += sizeof(TVmpModule);
	}

	//��������ģ�年ѡ��Ϣ[12/26/2011 chendaiwei]
	//20130513 �ظ�����ȥ��
	/*m_tConfEx.Clear();
	TConfInfoEx tTempEx = GetConfInfoExFromConfAttrb(m_tConf);
	if(m_tConf.HasConfExInfo())
	{
		u16 wConfInfoExSize = 0;
		BOOL32 bExistUnknowType = FALSE;
		UnPackConfInfoEx(m_tConfEx,(u8*)pszMsgBuf,wConfInfoExSize,bExistUnknowType,&m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall,&m_tConfEqpModule.m_tVmpModuleInfo);
		TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];
		u8   byAudioCapNum = m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
		if (byAudioCapNum > 0)//������չ��Ϣ������Ƶ������Ϣ����Ҫ����չ��Ϣ��ȥ����
		{
			tTempEx.SetAudioTypeDesc(atAudioTypeDesc,byAudioCapNum);
		}
		m_tConfEx.AddCapExInfo(tTempEx);
		pszMsgBuf += wConfInfoExSize;
	}
	else
	{
		m_tConfEx = tTempEx;
	}*/

	//����25���VMPģ��Ͷ໭�����ǽģ����Ϣ[5/23/2013 chendaiwei]
	if(m_tConf.HasConfExInfo())
	{
		u16 wConfInfoExSize = 0;
		BOOL32 bExistUnknowType = FALSE;
		TConfInfoEx tTmpConfInfoEx;
		UnPackConfInfoEx(tTmpConfInfoEx,(u8*)pszMsgBuf,wConfInfoExSize,bExistUnknowType,&m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall,&m_tConfEqpModule.m_tVmpModuleInfo);
		pszMsgBuf += wConfInfoExSize;
	}

	//HDU�໭��ͨ��Ԥ�������߼�[3/8/2013 chendaiwei]
	//1.����Ԥ��ͨ����Ա��Ϣ��TPeriEqpStatus��[�����ã����Դ���]
	//2.m_tHduVmpModule����ͨ����Ա��mtalias�±�ӳ��ΪMtId
	if(m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo())
	{
		for(u8 byLoopChnIdx = 0; byLoopChnIdx < (HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL; byLoopChnIdx++)
		{
			if(m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].IsValid())
			{
				u8 byHduEqpId = m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_byHduEqpId;
				u8 byParentChnnlIdx = m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_byChIdx;
				for (u8 bySubChnlIdx = 0; bySubChnlIdx < HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnlIdx++)
				{
					TPeriEqpStatus tTWStatus;
					u8 byMtIndex =  m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_abyTvWallMember[bySubChnlIdx];
					u8 byMemberType = m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_abyMemberType[bySubChnlIdx];

					if (!IsValidHduChn(byHduEqpId,byParentChnnlIdx))
					{
						continue;
					}
					
					if(!g_cMcuVcApp.GetPeriEqpStatus(byHduEqpId, &tTWStatus))
					{
						continue;
					}

					//TODO:��TPeriEqpStatus�޸Ķ��޸�[2/28/2013 chendaiwei]
// 						tTvwMbr = tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()];
// 						
// 						//20100702_tzy �жϵ�ǰ����ǽͨ���Ƿ���������ռ�ã������ռ������ʹ�ø�ͨ��
// 						if (0 != tTvwMbr.GetConfIdx()
// 							&& tTvwMbr.GetConfIdx() != m_byConfIdx)
// 						{
// 							ConfPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ConfDataCoordinate] TW<EqpId:%d, ChnId:%d> has been used by other conf:%d!\n", 
// 								atHduChnlInfo[byChnl].GetEqpId(), 
// 								atHduChnlInfo[byChnl].GetChnlIdx(),
// 								tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].GetConfIdx());
// 							continue;
// 						}
					
					if (byMtIndex > 0 )
					{
						if( byMtIndex <= MAXNUM_CONF_MT )
						{
							byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAliasArray[byMtIndex-1] );
							if (byMtId > 0)
							{
								TMt tMt = m_ptMtTable->GetMt( byMtId );
								//mt����Idxת����mt ID ���ýӿ�
								m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.SetHduVmpSubChnlMember(byLoopChnIdx,bySubChnlIdx,byMtId);
				
								LogPrint(LOG_LVL_DETAIL,MID_MCU_CONF,"[Confdatacoordinate]HDU Vmp Module:byLoopChIdx.%d byEqpId.%d byChIdx.%d bySubChnlIdx.%d byMemberType.%d\n",byLoopChnIdx,byHduEqpId,
									m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_byChIdx,bySubChnlIdx,byMtId);

								//TODO:��TPeriEqpStaus�����ȡ��ע��
								// �����ն���ǽ��, �ȰѾɵ���ʾ��ǽ
// 									if (tTvwMbr.GetConfIdx() == 0 || tTvwMbr.GetConfIdx() == m_byConfIdx)
// 									{
// 										RefreshMtStatusInTw( (TMt)tTvwMbr, FALSE, TRUE);
// 										
// 										tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].byMemberType = byMemberType;
// 										tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].SetConfIdx(m_byConfIdx);
// 										
// 										// [11/1/2011 liuxu] Ԥ���ǽ,��ʾ��ǽ����
// 										tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].SetMt(tMt);
// 										
// 										// ֻ�е���ǽ����, �Ż������ն���ʾ��ǽ����
// 										if (tTWStatus.m_byOnline)
// 										{
// 											RefreshMtStatusInTw(tMt, TRUE, TRUE);
// 										}										
// 									}
							}
						}
						//  [3/8/2013 chendaiwei] ����� 193��˵�����������淽ʽ �ݲ�֧�ָ���
						/*else if (byMtIndex == MAXNUM_CONF_MT + 1)
						{   
							TMt tMt;
							BOOL32 bValid = FALSE;
							switch (byMemberType)
							{
							case TW_MEMBERTYPE_SPEAKER:
								//��������
								tMt = m_tConf.GetSpeaker();                                
								bValid = TRUE;
								break;

							case TW_MEMBERTYPE_CHAIRMAN:
								// ������ϯ
								tMt = m_tConf.GetChairman();                                
								bValid = TRUE;
								break;

							case TW_MEMBERTYPE_POLL:
								// ��ѯ������ʱ�϶�û����ѯ
								tMt.SetNull();
								bValid = TRUE;
								break;

								//��������ģ��hduѡ��vmp��������vmpʱ����������hdu
							case TW_MEMBERTYPE_SWITCHVMP:
								tMt.SetNull();
								bValid = TRUE;
								break;

							case TW_MEMBERTYPE_DOUBLESTREAM:
								tMt = m_tDoubleStreamSrc;
								bValid = TRUE;
								break;

							default:
								ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Invalid member type %d for idx %d in CreateConf HDU vmp channl Module\n", 
									byMemberType, 
									byLoop);        
								bValid = FALSE;
							}
							if (bValid)
							{
								// ������������ն��������ᴥ����ϯ�����˱��
								m_tConfEqpModule.SetTvWallMemberInTvInfo( byHduEqpId, byParentChnnlIdx, tMt,bySubChnlIdx );
								
								tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].byMemberType = byMemberType;
								tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].SetConfIdx(m_byConfIdx);
							}                            
						}*/
						else
						{
							ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Invalid MtIndex :%d in CreateConf Hdu Vmp Module\n", byMtIndex);
						}
					}
					
// 					g_cMcuVcApp.SetPeriEqpStatus(byHduEqpId, &tTWStatus);
// 
// 					if(tTWStatus.m_byOnline)
// 					{
// 						// ֪ͨ���
// 						CTvwEqpStatusMsg.SetMsgBody((u8*)&tTWStatus, sizeof(tTWStatus));
// 						SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, CTvwEqpStatusMsg);
// 					}
				}
			}
		}
	}
	else
	{
		m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.Clear();
	}

	//������ϳ�ģ�� 
	if( tConfAttrb.IsHasVmpModule() )
	{		
        u8 byMtIndex = 0;
        u8 byMemberType = 0;
		TMt tMt;
        BOOL32 bValid = FALSE;
		for( byLoop = 0; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++ )
		{
			if( byLoop < MAXNUM_MPUSVMP_MEMBER)
			{
				byMtIndex = tVmpModule.m_abyVmpMember[byLoop];
				byMemberType = tVmpModule.m_abyMemberType[byLoop];
			}
			else
			{
				byMtIndex = m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byLoop];
				byMemberType = m_tConfEqpModule.m_tVmpModuleInfo.m_abyMemberType[byLoop];
			}

			if( byMtIndex > 0 )
			{
                if(byMtIndex <= MAXNUM_CONF_MT)
                {
                    byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAliasArray[byMtIndex-1] );
                    if( byMtId > 0 )
                    {
                        tMt = m_ptMtTable->GetMt( byMtId );
						m_tConfEqpModule.SetVmpMember( byLoop, tMt ); 	
						m_tConfEqpModule.SetVmpMemberType(byLoop,byMemberType);
                    }
                }	
                // ����� 193��˵�����������淽ʽ
                else if (byMtIndex == MAXNUM_CONF_MT + 1)
                {
					bValid = FALSE;
                    switch (byMemberType)
                    {
                    case VMP_MEMBERTYPE_SPEAKER:
						//��������
						tMt = m_tConf.GetSpeaker();                                
                        bValid = TRUE;
                        break;
                    case VMP_MEMBERTYPE_CHAIRMAN:
                        // ������ϯ
						tMt = m_tConf.GetChairman();                                
                        bValid = TRUE;
                        break;
                    case VMP_MEMBERTYPE_POLL:		// ��ѯ������ʱ�϶�û����ѯ
                    case VMP_MEMBERTYPE_VMPCHLPOLL:	// vmp��ͨ����ѯ������ʱ�϶�û��vmp��ͨ����ѯ
                    case VMP_MEMBERTYPE_DSTREAM:	// ˫�����棬����ʱ�϶�û��˫��Դ
                        tMt.SetNull();
                        bValid = TRUE;
                        break;
                    default:
                        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Invalid member type %d for idx %d in CreateConf Vmp Module\n", 
							byMemberType, 
							byLoop);        
                        bValid = FALSE;
                    }
                    if (bValid)
                    {
						//�������޳�Ա Member��Ȼ��193
						if(!tMt.IsNull())
						{
							m_tConfEqpModule.SetVmpMember( byLoop, tMt );
						}
						else
						{
							m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byLoop] = MAXNUM_CONF_MT + 1;
						}

						m_tConfEqpModule.SetVmpMemberType(byLoop,byMemberType);
                    }                          
                }
                else
                {
                    ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Invalid MtIndex :%d in CreateConf Vmp Module\n", byMtIndex);
                }
			}
		}
		
		m_tConfEqpModule.SetVmpModuleParam( tVmpModule.m_tVMPParam );
	}
    else
    {
        TVmpModuleInfo tVmpModuleInfo;
        memset( &tVmpModuleInfo, 0, sizeof(tVmpModuleInfo) );
        m_tConfEqpModule.SetVmpModule( tVmpModuleInfo );
    }

	//���N+1���ᣬ��Ϣ�帽��SMCU INFO[11/20/2012 chendaiwei]
	if(m_byCreateBy == CONF_CREATE_NPLUS )
	{
		TSmcuCallnfo atSmcuCallInfo[MAXNUM_SUB_MCU];
		memcpy(&atSmcuCallInfo[0],pszMsgBuf,sizeof(atSmcuCallInfo));
		for( u8 byTmpMtId = 1; byTmpMtId <= byMtNum; byTmpMtId++)
		{
			for(u8 bySmcuIdx = 0; bySmcuIdx < MAXNUM_SUB_MCU; bySmcuIdx++)
			{
				if(tMtAliasArray[byTmpMtId-1].m_AliasType == mtAliasTypeE164
				  && 0 == memcmp(atSmcuCallInfo[bySmcuIdx].m_achAlias,tMtAliasArray[byTmpMtId-1].m_achAlias,sizeof(tMtAliasArray[byTmpMtId-1].m_achAlias)))
				{
					u8 byId = m_ptMtTable->GetMtIdByAlias(&tMtAliasArray[byTmpMtId-1]);
					TMtAlias tTmpDialAlias;
					if( m_ptMtTable->GetDialAlias(byId,&tTmpDialAlias))
					{
						tTmpDialAlias.m_tTransportAddr = atSmcuCallInfo[bySmcuIdx].m_dwMtAddr;
						m_ptMtTable->SetDialAlias(byId,&tTmpDialAlias);

						LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[confdatacoordinate]Mt.%d DialAlias<Type:E164,Alias:%s,Ip:0x%x,Port:%d>\n",byId,tTmpDialAlias.m_achAlias,tTmpDialAlias.m_tTransportAddr.GetIpAddr(),tTmpDialAlias.m_tTransportAddr.GetPort());
					}

					break;
				}
			}
		}

		pszMsgBuf += sizeof(atSmcuCallInfo);
	}

	if(tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264
		&& tCapSupport.GetDStreamMediaType() == MEDIA_TYPE_H264)
	{
		TDStreamCap tTempDsCap = tCapSupport.GetDStreamCapSet();
		
#ifdef _8KH_	
		//tTempDsCap.SetH264ProfileAttrb(tCapSupport.GetMainStreamProfileAttrb());
#elif defined _8KE_
		//3. 8KG˫��ʼ��ΪBP(�˴�����)
		tTempDsCap.SetH264ProfileAttrb(emBpAttrb);
#else      
		//4. 8000A���������� 
		//tTempDsCap.SetH264ProfileAttrb(tCapSupport.GetMainStreamProfileAttrb());
#endif
		tCapSupport.SetDStreamCapSet(tTempDsCap);

		m_tConf.SetCapSupport(tCapSupport);
	}

#if defined (_8KH_) || defined (_8KE_) || defined (_8KI_)
	TVideoStreamCap atDSVideoCap[MAX_CONF_CAP_EX_NUM];
	u8 byDSCapNum = MAX_CONF_CAP_EX_NUM;

	ConstructDSCapExFor8KH8KE(tCapSupport,atDSVideoCap);
	m_tConfEx.SetDoubleStreamCapEx(atDSVideoCap,MAX_CONF_CAP_EX_NUM);
#endif

    //zbq[11/18/2008] ��������Ŀ��֧��
    TConfAttrb tAttrb = m_tConf.GetConfAttrb();
    
    if (0 != m_tConf.GetSecBitRate() ||
        (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() &&
         0 != m_tConf.GetSecVideoMediaType()))
    {
        tAttrb.SetUseAdapter(TRUE);
    }
    else
    {	//�Ƿ�����չ��������ѡ[12/14/2011 chendaiwei]
        if ( m_tConfEx.HasCapEx()
            ||
            (m_tConf.GetCapSupportEx().GetSecDSType() != MEDIA_TYPE_NULL &&
             m_tConf.GetCapSupportEx().GetSecDSType() != 0)
            ||
            (m_tConf.GetSecAudioMediaType() != MEDIA_TYPE_NULL &&
             m_tConf.GetSecAudioMediaType() != 0 &&            
             m_tConf.GetSecAudioMediaType() != m_tConf.GetMainAudioMediaType())
           )
        {
            tAttrb.SetUseAdapter(TRUE);
        }
        else
        {
            tAttrb.SetUseAdapter(FALSE);
        }
    }
    m_tConf.SetConfAttrb(tAttrb);
	
	g_cMcuVcApp.UpdateAgentAuthMtNum();
    
    //FIXME: ��ʱ����
//     if (!m_tConf.GetCapSupport().GetSecondSimCapSet().IsNull() &&
//         m_tConf.GetSecVidFrameRate() < 25 )
//     {
//         TSimCapSet tSim2 = m_tConf.GetCapSupport().GetSecondSimCapSet();
//         tSim2.SetVideoFrameRate(25);
//         TCapSupport tConfCap = m_tConf.GetCapSupport();
//         tConfCap.SetSecondSimCapSet(tSim2);
//         m_tConf.SetCapSupport(tConfCap);
//     }

    return;
}

/*====================================================================
    ������      ��ProcConfRegGkAck
    ����        ���ɹ�ע��GK������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/07/13    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ProcConfRegGkAck( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

    if(m_tConf.m_tStatus.IsRegToGK() &&
       !g_cMcuVcApp.GetRRQDriTransed())
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_GUARD, "[ProcConfRegGkAck] ignore it due to status is ok!\n");
		return;

	}

	m_tConf.m_tStatus.SetRegToGK( TRUE );
	
    u8 byReg = 1;
    cServMsg.SetMsgBody(&byReg, sizeof(byReg));
	SendMsgToAllMcs( MCU_MCS_CONFREGGKSTATUS_NOTIF, cServMsg );
	//֪ͨ���л�أ�֪ͨConfMode�Ϳ�����
    // ���� [4/30/2006] ���Բ���֪ͨ
	//cServMsg.SetMsgBody( ( u8 * )&( m_tConf.m_tStatus.m_tConfMode ), sizeof( TConfMode ) );
	//SendMsgToAllMcs( MCU_MCS_CONFMODE_NOTIF, cServMsg );

	// ����ע��GK�Ժ�������ն�
	// GK ע���ACK���������������Ժ��е�ǰ�����ߵ��ն�
	// if(m_tConfInStatus.IsInviteOnGkReged())
	{		
        cServMsg.SetServMsg(m_abySerHdr, sizeof(m_abySerHdr));
        cServMsg.SetNoSrc();
        cServMsg.SetTimer( TIMESPACE_WAIT_AUTOINVITE );
        cServMsg.SetMsgBody( NULL, 0 );
        InviteUnjoinedMt( cServMsg );

		m_tConfInStatus.SetInviteOnGkReged(FALSE);
	}
}

/*====================================================================
    ������      ��ProcConfRegGkNack
    ����        ��δ�ɹ�ע��GK������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/07/13    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ProcConfRegGkNack( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );    

	if( m_tConf.m_tStatus.IsRegToGK() || m_tConfInStatus.IsRegGkNackNtf() )
	{        
        m_tConf.m_tStatus.SetRegToGK( FALSE );
        
        u8 byReg = 0;
        cServMsg.SetMsgBody(&byReg, sizeof(byReg));
		SendMsgToAllMcs( MCU_MCS_CONFREGGKSTATUS_NOTIF, cServMsg );		

		m_tConfInStatus.SetRegGkNackNtf(FALSE);
	}
}

/*=============================================================================
�� �� ���� ProcGKChargeRsp
��    �ܣ� ����Ʒ���Ӧ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/17  4.0			������                  ����
2006/12/26  4.0         �ű���                  ֧��N+1ģʽ����Ϣ�ı���ͷ���
=============================================================================*/
void CMcuVcInst::ProcGKChargeRsp( const CMessage * pcMsg )
{
    if ( STATE_ONGOING != CurState() )
    {
        return;
    }
    
    CServMsg cServMsg( pcMsg->content, pcMsg->length );

    switch( pcMsg->event )
    {
    case MT_MCU_CONF_STARTCHARGE_ACK:
        {
            TAcctSessionId tSsnId = *(TAcctSessionId*)cServMsg.GetMsgBody();
            memcpy(&m_tChargeSsnId, &tSsnId, sizeof(m_tChargeSsnId));
            
            m_tConf.m_tStatus.SetGkCharge(TRUE);
            
            // N+1ģʽ�¼Ʒ���Ϣ���ͬ��������MCU
            if ( MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() )
            {
                TNPlusConfData tConfData;
				TNPlusVmpParam tNplusVmpParam[MAXNUM_PERIEQP];
				u8 byVmpNum = 0;
				
				GetNPlusDataFromConf( tConfData,&tNplusVmpParam[0],byVmpNum);
				
				TNPlusConfExData tConfExData;
				u16 wPackDataLen = GetNPlusDataExFromConf(tConfExData);
				
                cServMsg.SetConfId( m_tConf.GetConfId() );
                cServMsg.SetMsgBody( (u8*)&tConfData, sizeof(tConfData) );
				cServMsg.CatMsgBody(tConfExData.m_byConInfoExBuf, wPackDataLen);
				cServMsg.CatMsgBody((u8*)&tConfExData.m_atSmcuCallInfo[0], sizeof(tConfExData.m_atSmcuCallInfo));

				cServMsg.CatMsgBody((u8*)&byVmpNum,sizeof(byVmpNum));
				for( u8 byIdx = 0; byIdx < byVmpNum; byIdx ++ )
				{
					cServMsg.CatMsgBody((u8*)&tNplusVmpParam[byIdx].m_tVmpBaiscParam,sizeof(tNplusVmpParam[byIdx].m_tVmpBaiscParam));
					u8 byChnNum = tNplusVmpParam[byIdx].m_tVmpBaiscParam.GetMaxMemberNum();
					cServMsg.CatMsgBody((u8*)&byChnNum,sizeof(u8));
					cServMsg.CatMsgBody((u8*)&tNplusVmpParam[byIdx].m_tVmpMemer.m_tVmpChnnlInfo[0],byChnNum*sizeof(TNPlusVmpChnlMember));
				}
					
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS, "[ProcGkChargeRsp] conf:%s VmpNum:%d\n", m_tConf.GetConfName(),byVmpNum);

                cServMsg.SetEventId( MCU_NPLUS_CONFDATAUPDATE_REQ );
                
                g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
            }
        }
    	break;

    case MT_MCU_CONF_STOPCHARGE_ACK:
        
        // N+1ģʽ��, ��ʱ�����Ѿ�����, N+1���ݻ����������Ϣ. �˴�ֻ���������Ϣ
        memset(&m_tChargeSsnId, 0, sizeof(m_tChargeSsnId));
    	break;

    case MT_MCU_CONF_CHARGEEXP_NOTIF:
    case MT_MCU_CONF_STARTCHARGE_NACK:

        m_tConf.m_tStatus.SetGkCharge(FALSE);
        
        // gk�Ʒ���֤ʧ�ܻ��߼Ʒѷ����쳣ֱ�ӽ�������[11/21/2006-zbq]
        ReleaseConf();
        NEXTSTATE( STATE_IDLE );
        break;
        
    case MT_MCU_CHARGE_REGGK_NOTIF:
        
        if ( m_tConf.IsSupportGkCharge() && !m_tConf.m_tStatus.IsGkCharge() ) 
        {
            u8 byMtNum = 0;
            for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
            {
                // zbq [03/26/2007] N+1���ᣬ�����ǻ���ָ�����������ն˿϶���������Ļ����б��ڵġ���ʱ��ʱ����ʧ�ܡ�
                if ( m_tConfAllMtInfo.MtInConf(byMtId) )
                {
                    byMtNum ++;
                }
            }
            g_cMcuVcApp.ConfChargeByGK( m_byConfIdx, g_cMcuVcApp.GetRegGKDriId(), FALSE, m_byCreateBy, byMtNum );
        }
        break;

    default:
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "[ProcGKChargeRsp] unexpected msg.%d<%s> received !\n", 
                         pcMsg->event, OspEventDesc(pcMsg->event) );
        break;
    }

    return;
}

/*====================================================================
    ������      ��ProcMcsMcuReleaseConfReq
    ����        ���������鴦����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/03    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuReleaseConfReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

    TMt tApplyListMt[MAXNUM_CONF_MT];
    u8 byListLen = 0;
	switch( CurState() )
	{
	case STATE_ONGOING:
	case STATE_SCHEDULED:
		//���Է���ϯ�նˣ�nack 
		if( cServMsg.GetEventId() == MT_MCU_DROPCONF_CMD && 
			cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId() )
		{
			//ȡ������ϯ 
			SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
			return;
		}

		// �ն˲��ɽ���VCS����
		if (cServMsg.GetEventId() == MT_MCU_DROPCONF_CMD && 
			VCS_CONF == m_tConf.GetConfSource())
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMcsMcuReleaseConfReq] VCS Conf cann't be released by mt\n");
			return;
		}
		// ����VCS�������ն˿����Ļ���(�����ڼ���������)������ui�ͷŻ���
		if (CONF_CREATE_MT == m_byCreateBy &&
			VCS_CONF == m_tConf.GetConfSource() &&
			cServMsg.GetEventId() == MCS_MCU_RELEASECONF_REQ)
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "[ProcMcsMcuReleaseConfReq]vcs conf in cascade can't be released by ui\n");
			return;
		}
		
		
		//Ӧ��
		if( cServMsg.GetEventId() == MCS_MCU_RELEASECONF_REQ )
		{		
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}    
        
        //ͨ�淢�������б����ն˷�������״̬
        m_tApplySpeakQue.GetQueueList(tApplyListMt, byListLen);
        if (byListLen <= MAXNUM_CONF_MT)
        {
            for (u8 byPos = 0; byPos < byListLen; byPos++)
            {
                if (!tApplyListMt[byPos].IsNull())
                {
                    NotifyMtSpeakStatus(tApplyListMt[byPos], emDenid);
                }
            }
        }

		//��������
		ReleaseConf( TRUE );
        
        NEXTSTATE( STATE_IDLE );
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuChangeLockModeConfReq
    ����        ���������̨Ҫ��ı���鱣����ʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/13    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuChangeLockModeConfReq( const CMessage * pcMsg )
{
    STATECHECK;
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byLockMode = *(u8 *)cServMsg.GetMsgBody();

	//��Ч�Ļ��鱣����ʽ,NACK
	if (CONF_LOCKMODE_NONE    != byLockMode && 
		CONF_LOCKMODE_NEEDPWD != byLockMode &&
        CONF_LOCKMODE_LOCK    != byLockMode)
	{
		cServMsg.SetErrorCode(ERR_MCU_INVALID_CONFLOCKMODE);
		SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
		return;
	}
	//ACK
	SendReplyBack(cServMsg, cServMsg.GetEventId()+1);

    ChangeConfLockMode(cServMsg);

	return;
}

/*====================================================================
    ������      ��ChangeConfLockMode
    ����        ����������ģʽ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CServMsg &cServMsg
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	07/11/22    4.0         �ű���        ����
====================================================================*/
void CMcuVcInst::ChangeConfLockMode( CServMsg &cServMsg )
{
    u8 byLockMode = *(u8*)cServMsg.GetMsgBody();
    
    //���û��鱣��״̬
    m_tConf.m_tStatus.SetProtectMode(byLockMode);
    
    if (CONF_LOCKMODE_LOCK == m_tConf.m_tStatus.GetProtectMode())
    {
        m_tConfProtectInfo.SetLockByMcs(cServMsg.GetSrcSsnId());
        TMcsRegInfo tMcsRegInfo;
        g_cMcuVcApp.GetMcsRegInfo(cServMsg.GetSrcSsnId(), &tMcsRegInfo);
        cServMsg.SetMsgBody((u8*)&tMcsRegInfo.m_dwMcsIpAddr, sizeof(u32));
        cServMsg.CatMsgBody((u8*)tMcsRegInfo.m_achUser, MAXLEN_PWD);
        
        //FIXME: ������ڵ�����������ʾ����ʾ���Լ���MCS����
        SendMsgToAllMcs(MCU_MCS_LOCKUSERINFO_NOTIFY, cServMsg);
    }
    else if (CONF_LOCKMODE_NEEDPWD == m_tConf.m_tStatus.GetProtectMode())
    {
        m_tConfProtectInfo.SetMcsPwdPassed(cServMsg.GetSrcSsnId(), TRUE);
    }
	//zbq[06/03/2008] ����ģʽתΪ���ţ��������ʷ����
	else if (CONF_LOCKMODE_NONE == m_tConf.m_tStatus.GetProtectMode())
	{
		m_tConfProtectInfo.ResetMcsPwdPassed();
		m_tConf.SetConfPwd(NULL);

		//ʵʱˢ�»��
		cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
		if(m_tConf.HasConfExInfo())
		{
			u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
			u16 wPackDataLen = 0;
			PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
			cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
		}
		SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
	}
    
    //֪ͨ���л�أ����˷����
    cServMsg.SetMsgBody(&byLockMode, sizeof(byLockMode));
    SendMsgToAllMcs(MCU_MCS_CONFLOCKMODE_NOTIF, cServMsg);

    return;
}

/*====================================================================
    ������      ��ProcMcsMcuGetLockInfoReq
    ����        �����Ҫ��õ����������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/13    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetLockInfoReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch (CurState())
	{
	case STATE_ONGOING:

		//�����鱻���� 
        cServMsg.SetMsgBody();
		if (CONF_LOCKMODE_LOCK == m_tConf.m_tStatus.GetProtectMode())
		{
			if (m_tConfProtectInfo.IsLockByMcs())
			{
				TMcsRegInfo tMcsRegInfo;
				g_cMcuVcApp.GetMcsRegInfo( m_tConfProtectInfo.GetLockedMcSsnId(), &tMcsRegInfo );
				cServMsg.SetMsgBody( (u8*)&tMcsRegInfo.m_dwMcsIpAddr, sizeof(u32) );
				cServMsg.CatMsgBody( (u8*)tMcsRegInfo.m_achUser, MAXLEN_PWD );
			}
			else if(m_tConfProtectInfo.IsLockByMcu())
			{
                u8 byMcuId = m_tConfProtectInfo.GetLockedMcuId();
				TMtAlias tMtAlias;
				m_ptMtTable->GetMtAlias(byMcuId, 
					mtAliasTypeTransportAddress,
					&tMtAlias);
				
				cServMsg.SetMsgBody( (u8*)&tMtAlias.m_tTransportAddr.m_dwIpAddr, sizeof(u32) );
				if (! m_ptMtTable->GetMtAlias(byMcuId, 
					                          mtAliasTypeH323ID,
					                          &tMtAlias))
				{
					if (!m_ptMtTable->GetMtAlias(byMcuId, 
				                          		 mtAliasTypeE164,
						                         &tMtAlias))
					{
						strncpy( tMtAlias.m_achAlias, "mcu", sizeof(tMtAlias.m_achAlias) );
					}
				}
				tMtAlias.m_achAlias[MAXLEN_PWD-1] = 0;
				cServMsg.CatMsgBody( (u8*)tMtAlias.m_achAlias, MAXLEN_PWD );	
            }
            SendReplyBack(cServMsg, cServMsg.GetEventId()+1);	
		}		
		else
		{
			SendReplyBack(cServMsg, cServMsg.GetEventId()+2);	
		}
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
				pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
		break;
	}

	return;
}

/*====================================================================
    ������      ��ProcMcsMcuEnterPwdRsp
    ����        ���������̨����������Ӧ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/13    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuEnterPwdRsp( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	char *pchPWD = (char *)cServMsg.GetMsgBody();

	if( pcMsg->event == MCS_MCU_ENTERCONFPWD_ACK )
	{
       if( (memcmp( pchPWD, m_tConf.GetConfPwd() , MAXLEN_PWD ) == 0 ) )
	   {
		   m_tConfProtectInfo.SetMcsPwdPassed(cServMsg.GetSrcSsnId(), TRUE);   
	   }
	   else
	   {
		   //֪ͨ����������
		   SendMsgToMcs(  cServMsg.GetSrcSsnId(), MCU_MCS_WRONGCONFPWD_NOTIF, cServMsg );
	   }
	}

}

/*====================================================================
    ������      ��ProcMcsMcuChangeConfPwdReq
    ����        ���������̨����MCU�޸Ļ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/13    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuChangeConfPwdReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	CServMsg cServMsgNtf;	
	s8 *pchPWD = (s8 *)cServMsg.GetMsgBody();
	s8 szPwd[MAXLEN_PWD+1];

	switch( CurState() )
	{
	case STATE_ONGOING:	
	case STATE_SCHEDULED:
		astrncpy(szPwd, m_tConf.GetConfPwd(),sizeof(szPwd), MAXLEN_PWD+1); 
		//�޸�����
		m_tConf.SetConfPwd( pchPWD );
		
		//ACK
		SendReplyBack( cServMsg, cServMsg.GetEventId()+1 );

		//֪ͨ���л��
		SendMsgToAllMcs( MCU_MCS_CHANGECONFPWD_NOTIF, cServMsg );
		
		//���Ļ�ص����뱣��
		//zbq[06/03/2008] memcmpȡ�ĳ��Ȳ���
		//if(memcmp(pchPWD, szPwd, strlen(szPwd)) != 0)
		if(strcmp(pchPWD, szPwd) != 0)
		{
			if(m_tConf.GetStatus().GetConfMode().GetLockMode() == CONF_LOCKMODE_NEEDPWD)
			{
                m_tConfProtectInfo.ResetMcsPwdPassed();
                m_tConfProtectInfo.SetMcsPwdPassed(cServMsg.GetSrcSsnId());
			}
		}
		
		g_cMcuVcApp.RefreshHtml();

		//MCUͬ���ն������û���Ļ������룬���ںϲ������������֤У��
		cServMsgNtf.SetMsgBody((u8*)m_tConf.GetConfPwd(), MAXLEN_PWD);
		cServMsgNtf.SetEventId(MCU_MT_CONFPWD_NTF);
		BroadcastToAllSubMtJoinedConf( MCU_MT_CONFPWD_NTF, cServMsgNtf );

		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuSaveConfReq
    ����        �����漴ʱ������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/03/10    1.0         Qzj		      ����
	03/11/13    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::ProcMcsMcuSaveConfReq(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt tMt;
	TMtAlias tMtAlias;
	u8   bySaveDefault;

	switch( CurState() )
	{

	case STATE_SCHEDULED:

		//ԤԼ������޸Ļ���ʱ�ᱣ�������Ϣ��������ﲻ��Ҫ�ٱ��棬ֻ��ACK
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		break;

	case STATE_ONGOING:

		bySaveDefault = *(u8*)cServMsg.GetMsgBody();
		
		// Ӧ�� 
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
        
        // guzh [8/29/2006] ֻ����������Ա����Ϊȱʡ����
        if (bySaveDefault)
        {
               if (USRGRPID_SADMIN != CMcsSsn::GetUserGroup(cServMsg.GetSrcSsnId()) )
               {
                   // ֪ͨû���ܹ�����Ϊȱʡ
                   NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_SAVEDEFCONF_NOPERMIT );
                   bySaveDefault = FALSE;
               }
        }
		
		// ���淢����
		tMt = GetLocalSpeaker();
		if( m_tConf.HasSpeaker() && tMt.GetType() == TYPE_MT )
		{
            // zbq [08/08/2007] ���������ȱ�������б���
			if ( m_ptMtTable->GetDialAlias( tMt.GetMtId(), &tMtAlias )
                 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias ) 
				 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias)
				 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeE164, &tMtAlias) 
				 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias) )
			{
				m_tConf.SetSpeakerAlias( tMtAlias );
			}
			else
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "\n\n Cannot get speaker's Mt Alias.\n\n");
			}
		}

		// ������ϯ 
		tMt = m_tConf.GetChairman();
		if( m_tConf.HasChairman() && tMt.GetType() == TYPE_MT )
		{
            // zbq [08/08/2007] ��ϯ���ȱ�������б���
			if ( m_ptMtTable->GetDialAlias( tMt.GetMtId(), &tMtAlias )
                 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias )
				 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias)
				 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeE164, &tMtAlias) 
				 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias) )
			{
				m_tConf.SetChairAlias( tMtAlias );
			}
			else
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "\n\n Cannot get chairman's Mt Alias.\n\n");
			}
		}
		
		//���浽�ļ�
		g_cMcuVcApp.SaveConfToFile( m_byConfIdx, FALSE, bySaveDefault );

		break;

	default :
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuModifyConfReq
    ����        ���޸�ԤԼ���鴦����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/03    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuModifyConfReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TConfInfo	tConf;
	TConfAttrb  tConfAttrib;

	switch( CurState() )
	{
	case STATE_SCHEDULED:

		//�õ���Ϣ�����û����
		tConf = *( TConfInfo * )cServMsg.GetMsgBody();		
		tConfAttrib = tConf.GetConfAttrb();	

		//������ԤԼ����
		if ( tConf.IsCircleScheduleConf() )
		{
			//�����޸ĺ����Чʱ��
			TConfInfoEx tConfInfoEx;
			TDurationDate tDuraDate;
			if ( GetConfExInfoFromMsg(cServMsg,tConfInfoEx) )
			{
				tDuraDate = tConfInfoEx.GetDurationDate();
				//��֤durastart��ʱ����Ϊ00:00:00,duraend��ʱ����Ϊ23:59:59
				ModifyCircleScheduleConfDuraDate(tDuraDate);
				
			}
			TKdvTime tNextStarttime = GetNextOngoingTime(tConf,tDuraDate,TRUE);
			TKdvTime tNullTime;//�������memsetΪ0
			//�Ҳ����´ο�����ʱ��,�����˻ᣬreturn 
			if ( tNullTime == tNextStarttime )
			{

				cServMsg.SetErrorCode( (u16)ERR_MCU_CIRCLESCHEDULECONF_TIME_WRONG );
				SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuModifyConfReq]CirclescheduleConference %s timesetting error��errorcode��%d!\n", 
					m_tConf.GetConfName() ,ERR_MCU_CIRCLESCHEDULECONF_TIME_WRONG);
				return;
			}
		
		

		}
		else //��ͨԤԼ����
		{
			//if too old, send Nack; Duration time is 0 denoted that meeting could be ended at any time
			if( ( tConf.GetDuration() == 0 && time( NULL ) > tConf.GetStartTime() + 30 * 60 || 
				( tConf.GetDuration() != 0 && 
				time( NULL ) > tConf.GetStartTime() + tConf.GetDuration() * 60 ) ) )
			{
				cServMsg.SetErrorCode( ERR_MCU_STARTTIME_WRONG );
				SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s too late and canceled!\n", m_tConf.GetConfName() );
				return;
			}

		}
        
	
		
		//�����E164�����Ѵ��ڣ��ܾ� 
		if( 0 != strcmp( (char*)tConf.GetConfE164(), (char*)m_tConf.GetConfE164() ) &&
			g_cMcuVcApp.IsConfE164Repeat( tConf.GetConfE164(), FALSE,FALSE ) )
		{
			cServMsg.SetErrorCode( ERR_MCU_CONFE164_REPEAT );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s E164 repeated and create failure!\n", m_tConf.GetConfName() );
			return;
		}

		//�������Ѵ��ڣ��ܾ�
		if( 0 != strcmp( (char*)tConf.GetConfName(), (char*)m_tConf.GetConfName() ) &&
			g_cMcuVcApp.IsConfNameRepeat( tConf.GetConfName(), FALSE ,FALSE) )
		{
			cServMsg.SetErrorCode( ERR_MCU_CONFNAME_REPEAT );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s name repeated and create failure!\n", m_tConf.GetConfName() );
			return;
		}

		//dynamic vmp and vmp module conflict
		if( tConfAttrib.IsHasVmpModule() && tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_AUTO)
		{
			cServMsg.SetErrorCode( ERR_MCU_DYNAMCIVMPWITHMODULE );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because has module with dynamic vmp!\n", m_tConf.GetConfName() );
			return;		
		}

	    //��Ч�Ļ��鱣����ʽ,�ܾ�
		if( tConf.m_tStatus.GetProtectMode() > CONF_LOCKMODE_LOCK )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_CONFLOCKMODE );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s protect mode invalid and nack!\n", m_tConf.GetConfName() );
			return;
		}

		//��Ч�Ļ�����в���,�ܾ�
		if( tConf.m_tStatus.GetCallMode() > CONF_CALLMODE_TIMER )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_CALLMODE );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s call mode invalid and nack!\n", m_tConf.GetConfName() );
			return;
		}

        //����ʼʱ��Ϊ0,������ʼ 
        if( tConf.GetStartTime() == 0 )
        {
            tConf.SetStartTime( time( NULL ) );
            
            u8 byMode = CONF_TAKEMODE_SCHEDULED;
            cServMsg.SetMsgBody(&byMode, sizeof(byMode));
            SendMsgToAllMcs( MCU_MCS_RELEASECONF_NOTIF, cServMsg );
        }        

		post( MAKEIID( GetAppID(), GetInsID() ), MCU_SCHEDULE_CONF_START, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "A scheduled conference %s modified and restart!\n",m_tConf.GetConfName() );

		//release conference
		ReleaseConf( TRUE );

		NEXTSTATE( STATE_IDLE );	
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuDelayConfReq
    ����        ���������̨Ҫ���ӳ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/13    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuDelayConfReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u16 wDelayTime;

	switch( CurState() )
	{
	case STATE_ONGOING:
	case STATE_SCHEDULED:
        {
			if (m_tConf.GetDuration() == 0)
			{
				if (MT_MCU_DELAYCONF_REQ == pcMsg->event)
				{
					SendMsgToMt(cServMsg.GetSrcMtId(), MCU_MT_DELAYCONF_NACK, cServMsg);
				}
				else
				{
					SendMsgToMcs(cServMsg.GetSrcSsnId(), MCU_MCS_DELAYCONF_NACK, cServMsg);
				}
				
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Conf %s is manual release. Can not delay!\n", m_tConf.GetConfName());
				return;
			}
			
			if (MT_MCU_DELAYCONF_REQ == pcMsg->event)
			{
				TMt tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
				if (!(tMt == m_tConf.GetChairman()))
				{
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "Conf %s has delayed fail by no chairman!\n", m_tConf.GetConfName());
					SendMsgToMt(cServMsg.GetSrcMtId(), MCU_MT_DELAYCONF_NACK, cServMsg);
					return;
				}
				
				SendMsgToMt( cServMsg.GetSrcMtId() , MCU_MT_DELAYCONF_ACK, cServMsg );
			}
			else
			{
				SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_DELAYCONF_ACK, cServMsg );
			}
			
			wDelayTime = * ( u16* )cServMsg.GetMsgBody();
			
			//[2011/12/15/zhangli]�����޶���������1-65535��������ܳ����������ǰ��+���õ�>65535��������Ϊ65535
			u16 wMaxTime = 65535;		//~0
			u32 dwCurrTime = ntohs(wDelayTime) + m_tConf.GetDuration();
			if (dwCurrTime >= wMaxTime)
			{
				dwCurrTime = wMaxTime;
			}
			
			m_tConf.SetDuration(u16(dwCurrTime));        
			
			//Notification
			BroadcastToAllSubMtJoinedConf( MCU_MT_DELAYCONF_NOTIF, cServMsg ); 		
			SendMsgToAllMcs( MCU_MCS_DELAYCONF_NOTIF, cServMsg );
			
			if( pcMsg->event == MT_MCU_DELAYCONF_REQ )
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Conf %s has delayed %d minute by chairman!\n", 
					m_tConf.GetConfName(),  ntohs( wDelayTime ) );
			}
			else
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "Conf %s has delayed %d minute by mcs!\n", 
					m_tConf.GetConfName(),  ntohs( wDelayTime ) );
			}
		}

		
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuChangeVacHoldTimeReq
    ����        ���������̨Ҫ�ı�������������ʱ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/04/13    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuChangeVacHoldTimeReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byHoldTime;

	switch( CurState() )
	{
	case STATE_ONGOING:
	case STATE_SCHEDULED:

        // xsl [8/19/2006] ���Ƿ�ɢ���鲻֧�ִ˲���
        if (m_tConf.GetConfAttrb().IsSatDCastMode())
        {
	        cServMsg.SetErrorCode( ERR_MCU_DCAST_NOOP );            
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuChangeVacHoldTimeReq] distributed conf not supported vac\n");
            return;
        }

        byHoldTime = * ( u8* )cServMsg.GetMsgBody();
		m_tConf.SetTalkHoldTime( byHoldTime );
		
		SetMixerSensitivity( m_tConf.GetTalkHoldTime() );

        //Ack
		SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_CHANGEVACHOLDTIME_ACK, cServMsg );

		//Notification
		SendMsgToAllMcs( MCU_MCS_CHANGEVACHOLDTIME_NOTIF, cServMsg );
		
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuSpecChairmanReq
    ����        ��ָ����ϯ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/06    1.0         LI Yi         ����
    02/12/18    1.0         LI Yi         �ı���̸�Ϊƽ������
	03/11/24    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::ProcMcsMcuSpecChairmanReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt		tNewChairman;

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		//���Է���ϯ�ն�,�ܾ�
		if( cServMsg.GetEventId() == MT_MCU_SPECCHAIRMAN_REQ && 
			cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId() )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_OPER );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );

			//ȡ������ϯ
			SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
				
			return;
		}

		//����ϯģʽ,�ܾ� 
		if( m_tConf.m_tStatus.IsNoChairMode() )
		{
			cServMsg.SetErrorCode( ERR_MCU_NOCHAIRMANMODE );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				
			return;
		}
		
		tNewChairman = *( TMt * )cServMsg.GetMsgBody();
		tNewChairman = m_ptMtTable->GetMt( tNewChairman.GetMtId() );
		
		//�뵱ǰ��ϯ��ͬ
		if( tNewChairman == m_tConf.GetChairman() )
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Same chairman specified in conference %s! failure!\n", 
				m_tConf.GetConfName() );
			cServMsg.SetErrorCode( ERR_MCU_SAMECHAIRMAN );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	//nack
			return;
		}

		//����ϯ�����
		if( !m_tConfAllMtInfo.MtJoinedConf( tNewChairman.GetMtId() ) )
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "New chairman MT%u not in conference %s! Error!\n", 
				    tNewChairman.GetMtId(), m_tConf.GetConfName() );
			cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	//nack
			return;
		}

		//���������ڡ���ϯ��ѯѡ����ģʽ�£��л���ϯ��ͣ����ѯ
		if ( (CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode())
			 ||(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()) )
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "New chairman MT%u in conference %s  switch ,but now Chairman Polling,so stop polling!\n", 
				    tNewChairman.GetMtId(), m_tConf.GetConfName() );
           ProcStopConfPoll();
		}

		//Ӧ��
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );	//ack

		//�ı���ϯ
		ChangeChairman( &tNewChairman );

		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuCancelChairmanReq
    ����        ��ȡ����ϯ������
    �㷨ʵ��    ����ָ����ϯ�����ϲ�
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/19    1.0         LI Yi         ����
	03/11/25    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::ProcMcsMcuCancelChairmanReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:
		//���Է���ϯ�ն�,nack
		if( cServMsg.GetEventId() == MT_MCU_CANCELCHAIRMAN_REQ && 
			cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId() )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_OPER );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );

			//ȡ������ϯ
			SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
			return;
		}
		
		//����ϯ 
		if( !m_tConf.HasChairman() )
		{
			if( cServMsg.GetEventId() == MCS_MCU_CANCELCHAIRMAN_REQ )
			{
				cServMsg.SetErrorCode( ERR_MCU_NOCHAIRMAN );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			}
			return;
		}

		// ȡ����ϯʱ�������ǰ�����ڡ���ϯ��ѯ��״̬��������ֹͣ��ϯ��ѯ(��������ϯѡ����)
		// 20110414_miaoqs ��ϯ��ѯѡ���������
		if ( (CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()) 
			||(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()))
		{
			// ͣ��ϯ��ѯ
			ProcStopConfPoll();
		}
				
        //Ӧ��
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

		//ȡ����ϯ
		ChangeChairman( NULL );

		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuSetConfChairMode
    ����        ���������̨���û������ϯ��ʽ
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/12/02    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuSetConfChairMode( const CMessage * pcMsg )
{
    STATECHECK;

	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byChairMode = *( u8* )cServMsg.GetMsgBody();

	m_tConf.m_tStatus.SetNoChairMode( byChairMode == 1 ? FALSE : TRUE );

	if( m_tConf.m_tStatus.IsNoChairMode() && HasJoinedChairman() )
	{
		ChangeChairman( NULL );	
	}
	
	//֪ͨ���л��
	SendMsgToAllMcs( MCU_MCS_CONFCHAIRMODE_NOTIF, cServMsg );
    
    return;
}

/*====================================================================
    ������      ��ProcMcsMcuSpecSpeakerReq
    ����        ��ָ�������ն˴�����
    �㷨ʵ��    ��ָ���·�����ֱ�ӷ���Ӧ�𣬲���¼����ȴ�Ӧ�𷵻�
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/06    1.0         LI Yi         ����
    02/12/18    1.0         LI Yi         �л����̸�Ϊ����֮��ƽ������
	03/11/25    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::ProcMcsMcuSpecSpeakerReq( const CMessage * pcMsg )
{
	STATECHECK

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
    cServMsg.SetErrorCode(0);
		
	// �����ݽ�ģʽ�������������Ʒ���״̬,����ָ��������
    // mqs [2011/03/22] ����������������ǿ��ָ�������ˣ���ʵ�ʵ�ָ�������˲�������
	if (m_tConf.m_tStatus.IsVACing())
	{
        if (MT_MCU_SPECSPEAKER_CMD != pcMsg->event)
        {
		    cServMsg.SetErrorCode(ERR_MCU_VACNOSPEAKERSPEC);
		    SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
        }
		return;
	}
		
	//���Է���ϯ�նˣ�nack 
	if( (cServMsg.GetEventId() == MT_MCU_SPECSPEAKER_CMD ||
         cServMsg.GetEventId() == MT_MCU_SPECSPEAKER_REQ) && 
		 cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId() )
	{
		//ȡ������ϯ
		SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );			
		return;
	}
	
	//���ڷ��񣬲���ָ�������� 
	if( m_tConf.m_tStatus.IsPlaying() 
		|| m_tConf.m_tStatus.IsPlayReady())
	{
		cServMsg.SetErrorCode( ERR_MCU_CONFPLAYING );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "It playing now in conference %s! Cannot spcify the speaker!\n", 
			     m_tConf.GetConfName() );
		return;
	}

	
	TMt			tNewSpeaker;
	TMt         tRealMt;
 	tNewSpeaker = *( TMt * )cServMsg.GetMsgBody();
	u8 byIsSecSpeaker = 0;
	if (cServMsg.GetMsgBodyLen() > sizeof(TMt))
	{
		byIsSecSpeaker = *(u8*)(cServMsg.GetMsgBody()+sizeof(TMt));
	}

	tRealMt     = tNewSpeaker;
	if( IsLocalMcuId(tNewSpeaker.GetMcuIdx()))
	{
		tNewSpeaker = m_ptMtTable->GetMt( tNewSpeaker.GetMtId() );
		// ������ϯ�տ�ָ̨��smcu��������ʱ,����type��MT,�轫�����ΪMCU,��mcsָ��mcu�������˱���һ��
		tRealMt = tNewSpeaker;
	}
	else
	{
		tNewSpeaker = m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx( tNewSpeaker.GetMcuIdx() ) );
	}

	//�·�����δ���,nack
	if( tNewSpeaker.GetType() == TYPE_MT && 
		!m_tConfAllMtInfo.MtJoinedConf( tNewSpeaker.GetMcuId(), tNewSpeaker.GetMtId() ) )
	{
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "New speaker MT%u not in conference %s! Error!\n", 
			   tNewSpeaker.GetMtId(), m_tConf.GetConfName() );
		return;
	}

	//�·�������ֻ�����ն�,nack
	//fix ���ж���ֱ��mcu���ն˵�״̬
	TMtStatus	tMtStatus;
	m_ptMtTable->GetMtStatus(tNewSpeaker.GetMtId(), &tMtStatus);
	if( tNewSpeaker.GetType() == TYPE_MT && 
		( !tMtStatus.IsSendVideo() ) && 
		( !tMtStatus.IsSendAudio() ) )
	{
		cServMsg.SetErrorCode( ERR_MCU_RCVONLYMT );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "New speaker MT%u is receive only MT! Error!\n", 
				         tNewSpeaker.GetMtId() );
		return;
	}

    // xsl [7/20/2006] ���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����, ��֧���滻��Ҫ����Ŀ���ն��Ƿ���vmp��tvwall����ؼ�ء���ѡ����mtw���ն�¼��˫�������Ե����
    if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsMultiCastMt(tNewSpeaker.GetMtId()) )
    {
		if ( (IsSpeakerCanBrdVid(&tNewSpeaker)
			// ��Ӧ�þܵ����ں����ChgSpeakerInHdu/VMP�ﴦ���� [pengguofeng 2/22/2013]
// 				|| m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_SPEAKER)
				)
			&& //IsSatMtOverConfDCastNum(tNewSpeaker, emSpeaker, 0xff, 0xff, 0xff,0xff, 0xff, m_tConf.GetSpeaker().GetMtId()))
			!IsSatMtCanContinue(GetLocalMtFromOtherMcuMt(tNewSpeaker).GetMtId(),emSpeaker))
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSpecSpeakerReq]Add Speaker:%d result in Over Sat Chnnl Num!\n",
				tNewSpeaker.GetMtId());

			if(cServMsg.GetEventId() != MT_MCU_SPECSPEAKER_CMD)
			{
				cServMsg.SetErrorCode(ERR_MCU_DCAST_OVERCHNNLNUM);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			}
			return;
		}
    }

	//���ն��ڷ�˫����������˫�㲥
	if (byIsSecSpeaker > 0 && !m_tDoubleStreamSrc.IsNull())
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSpecSpeakerReq] m_tDoubleStreamSrc is not NULL,nack!\n");
		
		if(cServMsg.GetEventId() != MT_MCU_SPECSPEAKER_CMD)
		{
			cServMsg.SetErrorCode(ERR_MCU_SECSPEAKER_DSNOTNULL);
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		}
		return;
	}

	// mqs [2011/03/22] mcu��MCS��ָ��������ACK��Ϣ
	if( cServMsg.GetEventId() != MT_MCU_SPECSPEAKER_CMD )
	{
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	}
	
	
	//�·��������Ƿ�����
	//fix �ж����壬Ϊʲô������?�����ж��Ƿ��������?
	if( tNewSpeaker == m_tConf.GetSpeaker() )	
	{			
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"Same speaker specified in conference %s! Cannot spcify the speaker!\n", 
			m_tConf.GetConfName() );
	}

	//�ı䷢���� 
	if (byIsSecSpeaker)
	{
		ChangeSecSpeaker(tRealMt, TRUE);
	}
	else
	{
		ChangeSpeaker( &tRealMt,FALSE,TRUE,TRUE );
	}
}

/*====================================================================
    ������      ��ProcMcsMcuCancelSpeakerReq
    ����        ��ȡ�������˴�����
    �㷨ʵ��    ����ָ�����Ժϲ�����
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/19    1.0         LI Yi         ����
	03/11/27    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::ProcMcsMcuCancelSpeakerReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byIsSecSpeaker = 0;
	if (cServMsg.GetMsgBodyLen() > 0)
	{
		byIsSecSpeaker = *(u8*)(cServMsg.GetMsgBody());
	}

    TMt tSpeaker;

	switch( CurState() )
	{
	case STATE_ONGOING:

		//�����ݽ�ģʽ�������������Ʒ���״̬,����ָ��������
		if (m_tConf.m_tStatus.IsVACing())
		{
            if (MT_MCU_CANCELSPEAKER_CMD != pcMsg->event)
            {
			    cServMsg.SetErrorCode(ERR_MCU_VACNOSPEAKERCANCEL);
			    SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            }
			return;
		}
		
		//���Է���ϯ�նˣ�nack
		if( cServMsg.GetEventId() == MT_MCU_SPECSPEAKER_CMD && 
			cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId() )
		{
			//ȡ������ϯ
			SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
			return;
		}
		
		//û�з�����
		if(byIsSecSpeaker ==0 && !m_tConf.HasSpeaker()
			|| byIsSecSpeaker > 0 && GetSecVidBrdSrc().IsNull())
		{
			if( cServMsg.GetEventId() == MCS_MCU_CANCELSPEAKER_REQ )
			{
				cServMsg.SetErrorCode( ERR_MCU_NOSPEAKER );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			}
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"No speaker in conference %s now! Cannot cancel the speaker!\n", 
				m_tConf.GetConfName() );
			return;
		}
				
		//Ӧ��
		if( cServMsg.GetSrcMtId() == 0 )	//not MT source
		{
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}
		
		//ͣ�鲥
		tSpeaker = m_tConf.GetSpeaker();
		if( m_ptMtTable->IsMtMulticasting( tSpeaker.GetMtId() ) )
		{
			// 2011-8-11 add by pgf: Bug00061176:�����鲥ʱ��ȡ�������˵��»���ϳ����鲥��ַ�Ľ��������
			// �������⣺��������������ǹ㲥Դ�������趼û������������£�ͣ�鲥������ChangeVidBrdSrc�����ٽ���һ�Ρ�
			if ( m_tConf.GetConfAttrb().IsMulticastMode() && tSpeaker == GetVidBrdSrc())
			{
				g_cMpManager.StopMulticast( tSpeaker, 0, MODE_VIDEO );
			}
			if ( m_tConf.GetConfAttrb().IsMulticastMode() && tSpeaker == GetAudBrdSrc())
			{
				g_cMpManager.StopMulticast( tSpeaker, 0, MODE_AUDIO );
			}
			// 2011-8-11 add end
			m_ptMtTable->SetMtMulticasting( tSpeaker.GetMtId(), FALSE );
		}
		
		//ȡ��������
		if (byIsSecSpeaker > 0)
		{
			TMt tNullMt;
			ChangeSecSpeaker(tNullMt, FALSE);
		}
		else
		{
			ChangeSpeaker( NULL );
		}
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );

		break;
	}
}


/*====================================================================
    ������      ��ProcMcsMcuSeeSpeakerCmd
    ����        ��ǿ�ƹ㲥Դ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/27    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::ProcMcsMcuSeeSpeakerCmd( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byForceBrd;   

	switch( CurState() )
	{
	case STATE_ONGOING:
		{
        if (MT_MCU_VIEWBRAODCASTINGSRC_CMD == pcMsg->event)
        {
            if (m_tConf.GetChairman().GetMtId() != cServMsg.GetSrcMtId())
            {
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSeeSpeakerCmd] MtId<%d> isn't chairman, ignore it!\n");
                return;
            }
        }

		byForceBrd = *(u8*)cServMsg.GetMsgBody();
		
        //���ۡ����ƻ���ʱ������ǿ�ƹ㲥
        if(m_tConf.m_tStatus.IsNoMixing())
        {
            m_tConf.m_tStatus.SetMustSeeSpeaker( byForceBrd );
        }
        else
        {
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "No must see speaker while mixing!\n");
            return;
        }
        
        // ����,�ű���@2006.4.17, 4.20 ǿ�ƹ㲥���� 
        
        //����ǿ�ƹ㲥
        BOOL32 bRestoreAud;
        BOOL32 bRestoreVid;
		BOOL32 bNewVmpBrd = FALSE;	//�Ƿ���MPU�ڹ㲥
		if( m_tConf.m_tStatus.IsMustSeeSpeaker() )
		{
			// xliang [6/8/2009] Ŀǰ�������Ƿ���MPU�ڹ㲥
			if(g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) && GetLocalVidBrdSrc() == m_tVmpEqp)
			{
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				if(byVmpSubType != VMP)
				{
					bNewVmpBrd = TRUE;
				}
			}

			u8 byVidMtNum = 0;
			u8 byAudMtNum = 0;
			TMt atVidDstMt[MAXNUM_CONF_MT];
			TMt atAudDstMt[MAXNUM_CONF_MT];

            //�����Ƿ��й㲥Դ: ��ѡ��, �տ�(����)�㲥Դ
            for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
            {
				TMtStatus tMtStatus;
                if ( m_tConfAllMtInfo.MtJoinedConf( byMtId ) ) 
                {
					if (m_ptMtTable->GetMtType(byMtId) == MT_TYPE_VRSREC)
					{
						//����vrs��¼��
						continue;
					}
					TMt tMt = m_ptMtTable->GetMt(byMtId);
                    bRestoreAud = TRUE;
                    bRestoreVid = TRUE;

                    TMt tMtVideo;
                    TMt tMtAudio;                    

                    m_ptMtTable->GetMtSrc( byMtId, &tMtVideo, MODE_VIDEO );
                    m_ptMtTable->GetMtSrc( byMtId, &tMtAudio, MODE_AUDIO );

                    if (GetLocalSpeaker() == tMt)
                    {
                        // �Ƿ����ˣ�������ڿ���ϯ������һ�����˵�״̬������ģ�����������
            			if( tMtVideo == GetLocalMtFromOtherMcuMt(m_tConf.GetChairman()) && 
				            m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_CHAIR  )
                        {
                            // ���ָ�����Ƶ�㲥������ѡ��
                            bRestoreVid = FALSE;
                        }
                        if (tMtVideo == GetLocalMtFromOtherMcuMt(m_tLastSpeaker) &&
                            m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_LAST)
                        {
                            // ���ָ�����Ƶ�㲥������ѡ��
                            bRestoreVid = FALSE;
                        }

            			if( tMtAudio == GetLocalMtFromOtherMcuMt(m_tConf.GetChairman()) && 
				            m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_CHAIR &&
                            m_tConf.GetConfAttrb().GetSpeakerSrcMode() == MODE_BOTH)
                        {
                            // ���ָ�����Ƶ�㲥������ѡ��
                            bRestoreAud = FALSE;
                        }
            			if( tMtAudio == GetLocalMtFromOtherMcuMt(m_tLastSpeaker) && 
				            m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_LAST &&
                            m_tConf.GetConfAttrb().GetSpeakerSrcMode() == MODE_BOTH)
                        {
                            // ���ָ�����Ƶ�㲥������ѡ��
                            bRestoreAud = FALSE;
                        }                        
                    }

					//[2012/1/13 zhangli]ǿ�ƹ㲥��Ӱ����ϯ��״̬
					if (!m_tConf.GetChairman().IsNull() && m_tConf.GetChairman() == tMt)
					{
						bRestoreAud = FALSE;
						bRestoreVid = FALSE;
					}

                    //����ƵԴ
                    if ( !tMtAudio.IsNull() && bRestoreAud)
                    {
                        //ָ���ָ�������Ƶ�㲥ý��Դ
                        //RestoreRcvMediaBrdSrc( byMtId, MODE_AUDIO );   
						atAudDstMt[byAudMtNum] = m_ptMtTable->GetMt( byMtId );
						byAudMtNum++;      
                    }
                    
                    //����ƵԴ
                    if ( !tMtVideo.IsNull() && bRestoreVid) 
                    {
                        u16 wSrcSndBitrate  = m_ptMtTable->GetMtSndBitrate(tMtVideo.GetMtId());
                        u16 wSrcDialBitrate = m_ptMtTable->GetSndBandWidth(tMtVideo.GetMtId());
                        if (wSrcSndBitrate < wSrcDialBitrate && !(tMtVideo == m_tDoubleStreamSrc))
                        {
/*
                            CServMsg cTmpServMsg;
                            TLogicalChannel tLogicalChannel;
                            if (TRUE == m_ptMtTable->GetMtLogicChnnl(tMtVideo.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
                            {
                                // �޸ķ����ն˵ķ�������
                                tLogicalChannel.SetFlowControl(wSrcDialBitrate);
                                m_ptMtTable->SetMtLogicChnnl(tMtVideo.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);
                                cTmpServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
                                SendMsgToMt(tMtVideo.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cTmpServMsg);
                            }*/

                            
                            // �޸ı��ն˵Ľ�������
//                            u16 wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(byMtId);
//                            m_ptMtTable->SetMtReqBitrate(byMtId, wMtDialBitrate);
                        }
                        if( bNewVmpBrd )
						{
							// xliang [6/8/2009] Make Mts watching other Mts before watch VMP now ; 
							// Mts already watching VMP maintain their status.
							m_ptMtTable->GetMtStatus( byMtId, &tMtStatus );
							if (! tMtStatus.GetSelectMt( MODE_VIDEO ).IsNull())
							{
								//SwitchNewVmpToSingleMt(tMt);	//����ϳɹ㲥����������								
								StopSelectSrc( tMt,MODE_VIDEO );
								bRestoreVid = FALSE;
							}
						}
						else
						{
							//ָ���ָ�������Ƶ�㲥ý��Դ
							//RestoreRcvMediaBrdSrc( byMtId, MODE_VIDEO );
							atVidDstMt[byVidMtNum] = tMt;
							byVidMtNum++;
						}
					}

                    m_ptMtTable->GetMtStatus( byMtId, &tMtStatus );  
					
					//[201112/02/zhangli]�����ѡ���ͷ����������ͷŴ���
					TMt tSelMt = tMtStatus.GetSelectMt(MODE_VIDEO);
					if (bRestoreVid && !tSelMt.IsNull())
					{
						/*if (IsNeedSelAdpt(tSelMt, tMt, MODE_VIDEO))
						{
							StopSelAdapt(tSelMt, tMt, MODE_VIDEO);
						}

						FreeRecvSpy(tSelMt, MODE_VIDEO);
						tSelMt.SetNull();
						tMtStatus.SetSelectMt(tSelMt, MODE_VIDEO);*/
						StopSelectSrc( tMt,MODE_VIDEO,FALSE,FALSE );
						
					}
					
					tSelMt = tMtStatus.GetSelectMt(MODE_AUDIO);
					if (bRestoreAud && !tSelMt.IsNull())
					{
						/*if (IsNeedSelAdpt(tSelMt, tMt, MODE_AUDIO))
						{
							StopSelAdapt(tSelMt, tMt, MODE_AUDIO);
						}
						FreeRecvSpy(tSelMt, MODE_AUDIO);
						tSelMt.SetNull();
						tMtStatus.SetSelectMt(tSelMt, MODE_AUDIO);*/
						StopSelectSrc( tMt,MODE_AUDIO,FALSE,FALSE );
					}

                    //m_ptMtTable->SetMtStatus( byMtId, &tMtStatus );              
                }
            }

			if( byVidMtNum != 0 )
			{
				RestoreRcvMediaBrdSrc( byVidMtNum, atVidDstMt, MODE_VIDEO );
			}
			if( byAudMtNum != 0 )
			{
				RestoreRcvMediaBrdSrc( byAudMtNum, atAudDstMt, MODE_AUDIO );
			}
		}        
		else
		{
            //ֹͣǿ�ƹ㲥�������κδ���
		}
        
		ConfModeChange();

		break;
		}
	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ProcMcsMcuAddMtExReq
    ����        �����԰�������̨�����ն˴�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
	              ��Ϣ��ΪTMcu+TAddMtInfoEx(�����ն˵�����)
    ����ֵ˵��  ��
 ====================================================================*/
void CMcuVcInst::ProcMcsMcuAddMtExReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	TMcu tMcu;
    tMcu.SetNull();
	TAddMtInfoEx  *ptAddMtInfo = NULL;
	u8            byMtNum = 0;

    if (cServMsg.GetMsgBodyLen() >= sizeof(TMcu))
    {
		tMcu = *(TMcu*)(cServMsg.GetMsgBody());
		byMtNum= ( cServMsg.GetMsgBodyLen() - sizeof(TMcu) ) / sizeof( TAddMtInfoEx );
		ptAddMtInfo = (TAddMtInfoEx *)(cServMsg.GetMsgBody() + sizeof(TMcu));
    }
	else
	{
		return;
	}

	STATECHECK

	//���ܺ��л��鱾��
	if ( ptAddMtInfo && mtAliasTypeE164 == ptAddMtInfo->m_AliasType)
	{
		if (ptAddMtInfo && 0 == strcmp(ptAddMtInfo->m_achAlias, m_tConf.GetConfE164()))
		{
			cServMsg.SetErrorCode(ERR_MCU_NOTCALL_CONFITSELF);
			SendReplyBack(cServMsg, MCU_MCS_ADDMT_NACK);
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuAddMtExReq]CMcuVcInst: Cannot Call conf itself.\n");
			return;
		}
	}

	// ����
	if (!tMcu.IsLocal())
	{
		//u16 wMcuIdx = tMcu.GetMcuId();
		u8 byMtId = GetFstMcuIdFromMcuIdx(tMcu.GetMcuId());
		if (!m_tConfAllMtInfo.m_tLocalMtInfo.MtJoinedConf(byMtId))
		{
			cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
			SendReplyBack( cServMsg, MCU_MCS_ADDMT_NACK );
			return;
		}

		CServMsg cMsg;
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
				 sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
		astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
				 sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));
		cMsg.SetServMsg(cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		cMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cMsg.CatMsgBody(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
		SendMsgToMt( byMtId, MCU_MCU_INVITEMT_REQ, cMsg );

		SendReplyBack( cServMsg, MCU_MCS_ADDMT_ACK );
		return;
	}

	//��������, ack
	SendReplyBack( cServMsg, MCU_MCS_ADDMT_ACK );

	//���������ն��б�
	for (u8 byLoop = 0; byLoop < byMtNum; byLoop++)
	{
		u8  byMtId;
		TMt tMt;
		byMtId = AddMt( ptAddMtInfo[byLoop], ptAddMtInfo[byLoop].GetCallBitRate(), 
						(ptAddMtInfo[byLoop].m_byCallMode == CONF_CALLMODE_TIMER || ptAddMtInfo[byLoop].m_byCallMode == CONF_CALLMODE_NONE) ? ptAddMtInfo[byLoop].m_byCallMode : m_tConf.m_tStatus.GetCallMode() );
    
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuAddMtExReq]Mcs add mt-%d DialBitRate-%d Alias-", 
				byMtId, ptAddMtInfo[byLoop].GetCallBitRate());
		if ( ptAddMtInfo[byLoop].m_AliasType == mtAliasTypeTransportAddress )
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT,  "%s:%d!\n",				
					  StrOfIP(ptAddMtInfo[byLoop].m_tTransportAddr.GetIpAddr()), 
					  ptAddMtInfo[byLoop].m_tTransportAddr.GetPort() );
		}
		else
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT, "%s!\n",	ptAddMtInfo[byLoop].m_achAlias);
		}

		// �����ն���������������Ӹ��ն�
		if ( 0 == byMtId )
		{
// 			cServMsg.SetErrorCode(ERR_MCU_NOTCALL_CONFITSELF);
// 			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
// 			ConfLog(FALSE, "CMcuVcInst: alloc MTID Fail.\n");
			return;
		}
		else
		{
			// �ն��������ɻ�ظ�Ԥָ��������ָ�������������ڴ�ͨ��ʱʹ��
			m_ptMtTable->SetMtCapSpecByMCS(byMtId, TRUE);
			m_ptMtTable->m_tMTInfoEx[byMtId].clear();
			m_ptMtTable->m_tMTInfoEx[byMtId].SetMainVideoCap(const_cast<TVideoStreamCap&>(ptAddMtInfo[byLoop].GetMtCapInfo().GetMainVideoCap()));
			m_ptMtTable->m_tMTInfoEx[byMtId].SetDStreamVideoCap(const_cast<TVideoStreamCap&>(ptAddMtInfo[byLoop].GetMtCapInfo().GetDStreamVideoCap()));
			TAddMtCapInfo& tCapInfo = const_cast<TAddMtCapInfo&>(ptAddMtInfo[byLoop].GetMtCapInfo());
			ConfPrint( LOG_LVL_DETAIL, MID_MCU_MT, "[ProcMcsMcuAddMtExReq]MainMaxBitRate-%d MainMediaType-%d MainResolution-%d MainFrameRate-%d\n",
				                    tCapInfo.GetMainMaxBitRate(), tCapInfo.GetMainMediaType(),
							        tCapInfo.GetMainResolution(), 
									tCapInfo.IsMainFrameRateUserDefined()?tCapInfo.GetUserDefMainFrameRate():tCapInfo.GetMainFrameRate());
			
			ConfPrint( LOG_LVL_DETAIL, MID_MCU_MT,"[ProcMcsMcuAddMtExReq]DStreamMaxBitRate-%d DStreamMediaType-%d DStreamResolution-%d DStreamFrameRate-%d\n",
				                    tCapInfo.GetDstreamMaxBitRate(), tCapInfo.GetDstreamMediaType(),
							        tCapInfo.GetDstreamResolution(), 
									tCapInfo.IsDstreamFrameRateUserDefined()?tCapInfo.GetUserDefDstreamFrameRate():tCapInfo.GetDstreamFrameRate());

		}


		if (byMtId > 0 && !m_tConfAllMtInfo.MtJoinedConf(byMtId))
		{
			if (CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode( byMtId ) && 
				DEFAULT_CONF_CALLTIMES != m_tConf.m_tStatus.GetCallTimes())
			{
				m_ptMtTable->SetCallLeftTimes( byMtId, (m_tConf.m_tStatus.GetCallTimes()-1) );
			}
			tMt = m_ptMtTable->GetMt( byMtId );

			//�����ն˺���ģʽ��������֤����һ�Σ��˴�ʹ�û�������
			InviteUnjoinedMt( cServMsg, &tMt, TRUE, TRUE );

			m_ptMtTable->SetAddMtMode(byMtId, ADDMTMODE_MCS);				

		}
	}



	//������ػ��������ն���Ϣ
	//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
	//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//��������ն˱�
	SendMtListToMcs(LOCAL_MCUIDX);

	return;
}

/*====================================================================
    ������      ��ProcMcsMcuAddMtReq
    ����        ���������̨�����ն˴�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/05    1.0         LI Yi         ����
	03/11/27    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::ProcMcsMcuAddMtReq( const CMessage * pcMsg )
{
	STATECHECK
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	u8   byMtNum = 0;
	TMcu tMcu;
    tMcu.SetNull();
	TAddMtInfo  *ptAddMtInfo = NULL;
	BOOL32 bLowLevelMcuCalledIn = FALSE; 
	TMsgHeadMsg tHeadMsg,tHeadMsgAck;
	BOOL32 bIsLocal = TRUE;
	if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
	{
        if (cServMsg.GetMsgBodyLen() >= sizeof(TMcuMcuReq)+sizeof(TMt))
        {
		    byMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TMcuMcuReq)-sizeof(TMt) )/sizeof( TAddMtInfo);
		    ptAddMtInfo = (TAddMtInfo *)(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq)+sizeof(TMcu));
		    tMcu = *(TMcu*)(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq)); 

			if( cServMsg.GetMsgBodyLen() > (byMtNum * sizeof(TAddMtInfo) + sizeof(TMcuMcuReq) + sizeof( TMcu ) ) )
			{
				tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq)+sizeof(TMcu)+byMtNum*sizeof(TAddMtInfo));			
				tHeadMsgAck.m_tMsgSrc = tHeadMsg.m_tMsgDst;
				tHeadMsgAck.m_tMsgDst = tHeadMsg.m_tMsgSrc;	
			}					
			bIsLocal = tMcu.IsMcuIdLocal();
			
        }
	}
	else
	{
        if (cServMsg.GetMsgBodyLen() >= sizeof(TMcu))
        {
		    tMcu = *(TMcu*)(cServMsg.GetMsgBody());
			tMcu.SetType( TYPE_MCU );
		    byMtNum= ( cServMsg.GetMsgBodyLen() - sizeof(TMt) )/sizeof( TAddMtInfo );
		    ptAddMtInfo = (TAddMtInfo *)(cServMsg.GetMsgBody()+sizeof(TMcu));
			// xliang [8/29/2008] ���Ӹ߼���mcu�����ı�־��
			//��������Ӧ����SetNotInvited�б��������߼�,��Ӧ�Լ��������еļ��ܻ���
			if(cServMsg.GetMsgBodyLen() > sizeof(TMcu) + byMtNum * sizeof(TAddMtInfo))
			{		
				bLowLevelMcuCalledIn = *(BOOL32 *)(cServMsg.GetMsgBody() + sizeof(tMcu) + byMtNum * sizeof(TAddMtInfo));
			}			
			if( MT_MCU_ADDMT_REQ == cServMsg.GetEventId() || bLowLevelMcuCalledIn)// [9/27/2010 xliang] �����Ŀ϶�����Ϊ������ӵ�
			{
				bIsLocal = TRUE;
			}
			else
			{
				bIsLocal = tMcu.IsLocal();
			}			
        }
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuAddMtReq] Add Mt From Mcu(%d.%d.%d) level.%d \n",tMcu.GetMcuId(),tMcu.GetMtId(),
				tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],tHeadMsg.m_tMsgDst.m_byCasLevel
				);

	u8  byLoop, byMtId;
	TMt tMt;

	

	//���Է���ϯ�ն�, nack
	if (MT_MCU_ADDMT_REQ == cServMsg.GetEventId() && 
		cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId())
	{
		cServMsg.SetErrorCode( ERR_MCU_INVALID_OPER );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );

		//ȡ����ϯ
		SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );

		return;
	}

	//���ܺ��л��鱾��
	if ( ptAddMtInfo && mtAliasTypeE164 == ptAddMtInfo->m_AliasType)
	{
		if ( ptAddMtInfo && 0 == strcmp(ptAddMtInfo->m_achAlias, m_tConf.GetConfE164()))
		{
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
			cServMsg.SetErrorCode(ERR_MCU_NOTCALL_CONFITSELF);
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Cannot Call conf itself.\n");
			return;
		}
	}

	//�Ƿ��ڱ���������
	if (!bIsLocal)
	{
		u16 wMcuIdx = INVALID_MCUIDX;
		if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
		{
			byMtId = (u8)tMcu.GetMcuId();
			wMcuIdx = GetMcuIdxFromMcuId( byMtId );
			if( INVALID_MCUIDX == wMcuIdx )
			{
				cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Cannot Find McuId Info.McuId.%d\n",byMtId);
				return;
			}			
		}
		else
		{
			wMcuIdx = tMcu.GetMcuIdx();
		}

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuAddMtReq] Mcu is not local.wMcuIdx.%d\n",wMcuIdx );

		//u8 byFstMcuId = 0,bySecMcuId = 0;
		u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
		memset( &abyMcuId[0],0,sizeof(abyMcuId) );
		if( m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] ) )
		{
			if( 0 != abyMcuId[1] &&
				m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],1,&wMcuIdx ) 
				)
			{
				if( !m_tConfAllMtInfo.GetMtInfo( wMcuIdx ).MtJoinedConf(abyMcuId[1]) )
				{
					if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
					{
						cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
					}					
					cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					return;
				}				
			}
			
			if( 0 == abyMcuId[1] &&
				!m_tConfAllMtInfo.m_tLocalMtInfo.MtJoinedConf(abyMcuId[0])
				)
			{
				if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
				{
					cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
				}
				cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
		}

		CServMsg cMsg;
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
				 sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
		astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
				 sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));
		cMsg.SetServMsg(cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		if(cServMsg.GetEventId() != MCU_MCU_INVITEMT_REQ)
		{		
			memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
			
			tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMcu,tMcu );
			/*tMcu = tHeadMsg.m_tMsgDst.m_tMt;
			memset( &abyMcuId[0],0,sizeof(abyMcuId) );			
			if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
			{			
				m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] );
				if( 0 != abyMcuId[1] )
				{
					tHeadMsg.m_tMsgDst.m_byCasLevel = 1;				
					tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( abyMcuId[0] );
					tHeadMsg.m_tMsgDst.m_tMt.SetMtId( abyMcuId[1] );
				}
				else
				{
					tHeadMsg.m_tMsgDst.m_byCasLevel = 0;
					tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( abyMcuId[0] );
				}				
			}*/								
		}
		/*else
		{
			tHeadMsg.m_tMsgDst.m_tMt = tMcu;
		}*/
		
		
		cMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cMsg.CatMsgBody((u8 *)&tMcu, sizeof(TMcu));		
		if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
		{			
			cMsg.CatMsgBody( (u8*)ptAddMtInfo, byMtNum * sizeof(TAddMtInfo) );
		}
		else
		{
			cMsg.CatMsgBody(cServMsg.GetMsgBody()+sizeof(TMcu), cServMsg.GetMsgBodyLen()-sizeof(TMcu));
		}
		cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		
		SendMsgToMt( (u8)tMcu.GetMcuId(), MCU_MCU_INVITEMT_REQ, cMsg );

		if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
		{
			//tMt = m_ptMtTable->GetMt( tMt.GetMcuId() );
			//tHeadMsgAck.m_tMsgSrc.m_tMt = tMcu;
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
			cServMsg.CatMsgBody( (u8*)&tMcu,sizeof(TMcu) );
		}
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
	}

	//��������, ack
	if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
	{
		//tHeadMsgAck.m_tMsgSrc.m_tMt = tMcu;
		cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
		cServMsg.CatMsgBody( (u8*)&tMcu,sizeof(TMcu) );
	}
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	//���������ն��б�
	for (byLoop = 0; byLoop < byMtNum; byLoop++)
	{
		if (!ptAddMtInfo)
		{
			break;
		}
		
		const u8 byCallMode = ( (ptAddMtInfo[byLoop].m_byCallMode == CONF_CALLMODE_TIMER || ptAddMtInfo[byLoop].m_byCallMode == CONF_CALLMODE_NONE)) 
			? (ptAddMtInfo[byLoop].m_byCallMode) : (m_tConf.m_tStatus.GetCallMode()) ;

		byMtId = AddMt( ptAddMtInfo[byLoop], ptAddMtInfo[byLoop].GetCallBitRate(), byCallMode);
        
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Mcs add mt-%d DialBitRate-%d CallMode-%d aliasType-%d\n", 
				byMtId, ptAddMtInfo[byLoop].GetCallBitRate(), 
				ptAddMtInfo[byLoop].GetCallMode(),
				ptAddMtInfo[byLoop].m_AliasType );
        if ( ptAddMtInfo[byLoop].m_AliasType == mtAliasTypeTransportAddress )
        {
            ConfPrint( LOG_LVL_DETAIL, MID_MCU_MT, "%s:%d!\n",				
                      StrOfIP(ptAddMtInfo[byLoop].m_tTransportAddr.GetIpAddr()), 
                      ptAddMtInfo[byLoop].m_tTransportAddr.GetPort() );
        }
        else
        {
            ConfPrint( LOG_LVL_DETAIL, MID_MCU_MT, "%s!\n",	ptAddMtInfo[byLoop].m_achAlias);
        }

		// �ն����������ɻ�ظ�Ԥָ��
		m_ptMtTable->SetMtCapSpecByMCS(byMtId, FALSE);

		if (byMtId > 0 && !m_tConfAllMtInfo.MtJoinedConf(byMtId))
		{
			if (CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode( byMtId ) && 
				DEFAULT_CONF_CALLTIMES != m_tConf.m_tStatus.GetCallTimes())
			{
				m_ptMtTable->SetCallLeftTimes( byMtId, (m_tConf.m_tStatus.GetCallTimes()-1) );
			}
			tMt = m_ptMtTable->GetMt( byMtId );

			//�����ն˺���ģʽ��������֤����һ��
			InviteUnjoinedMt( cServMsg, &tMt, TRUE, TRUE, VCS_FORCECALL_REQ, bLowLevelMcuCalledIn );

			// libo [3/29/2005]
			if (MT_MCU_ADDMT_REQ == cServMsg.GetEventId())
			{
				m_ptMtTable->SetAddMtMode(byMtId, ADDMTMODE_CHAIRMAN);
			}
			else
			{
				m_ptMtTable->SetAddMtMode(byMtId, ADDMTMODE_MCS);
			}
			// libo [3/29/2005]end
		}

		// libo [3/30/2005]
		if (0 == byMtId || 
			(byMtId > 0 && MT_MCU_ADDMT_REQ == cServMsg.GetEventId() && 
			 m_tConfAllMtInfo.MtJoinedConf(byMtId)))
		{
			cServMsg.SetEventId(MCU_MT_ADDMT_NACK);
			cServMsg.SetMsgBody((u8 *)&ptAddMtInfo[byLoop], sizeof(TMtAlias));
			SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_ADDMT_NACK, cServMsg);
		}
		// libo [3/30/2005]end
	}

	//������ػ��������ն���Ϣ
	//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
	//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//��������ն˱�
	SendMtListToMcs(LOCAL_MCUIDX);

	return;
}

/*====================================================================
    ������      ��ProcMcsMcuDelMtReq
    ����        ���������̨ɾ���ն˴�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/05    1.0         LI Yi         ����
	03/11/25    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::ProcMcsMcuDelMtReq( const CMessage * pcMsg )
{
	STATECHECK
		
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt tMt;
	TMsgHeadMsg tHeadMsg,tHeadMsgAck;
	BOOL32 bIsLocal = TRUE;
	u8 byMtId = 0;
	u16 wMcuIdx = INVALID_MCUIDX;
	if(cServMsg.GetEventId() == MCU_MCU_DELMT_REQ)
	{
		tMt = *( TMt * )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));
		if( cServMsg.GetMsgBodyLen() > ( sizeof(TMcuMcuReq) + sizeof( TMt ) ) )
		{
			tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody()+sizeof(TMcuMcuReq) + sizeof(TMt) );
			tHeadMsgAck.m_tMsgSrc = tHeadMsg.m_tMsgDst;
			tHeadMsgAck.m_tMsgDst = tHeadMsg.m_tMsgSrc;
		}	
		
		bIsLocal = tMt.IsMcuIdLocal();
		wMcuIdx = GetMcuIdxFromMcuId( (u8)tMt.GetMcuId() );		
		byMtId = tMt.GetMtId();
	}
	else
	{
		tMt = *( TMt * )cServMsg.GetMsgBody();
// 		//�ն�������Ϣת��һ��
// 		if( MT_MCU_DELMT_REQ == cServMsg.GetEventId() )
// 		{
// 			tMt = m_ptMtTable->GetMt( tMt.GetMtId() );
// 		}
		bIsLocal = tMt.IsLocal();
		wMcuIdx = tMt.GetMcuIdx();
		byMtId = tMt.GetMtId();
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuDelMtReq] Del Mt(%d.%d.%d) level.%d\n",wMcuIdx,byMtId,
				tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],tHeadMsg.m_tMsgDst.m_byCasLevel 
				);

	//���Է���ϯ�նˣ�nack
	if( cServMsg.GetEventId() == MT_MCU_DELMT_REQ && 
		cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId() )
	{
		cServMsg.SetErrorCode( ERR_MCU_INVALID_OPER );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		
		//ȡ����ϯ
		SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
		
		return;
	}

	TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tMt.GetMcuId());
	//�������û��
	if(tMtInfo.IsNull())
	{
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//�Ѿ����ڻ�����,NACK
	if( !tMtInfo.MtInConf( tMt.GetMtId() ) )
	{	
		if(cServMsg.GetEventId() == MCU_MCU_DELMT_REQ)
		{
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
		}
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//���Ǳ���MCU�µ��ն�
    if( !bIsLocal )//!tMt.IsLocal() )
	{
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
			sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
		astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
			sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));
		
		if(cServMsg.GetEventId() != MCU_MCU_DELMT_REQ)
		{
			memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
			tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo(tMt,tMt);
			/*tMt = tHeadMsg.m_tMsgDst.m_tMt;
			if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
			{
				u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
				memset( &abyMcuId[0],0,sizeof(abyMcuId) );
				//u8 byFstMcuId = 0,bySecMcuId = 0;
				m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] );
				if( 0 != abyMcuId[1] )
				{
					tHeadMsg.m_tMsgDst.m_byCasLevel = 1;
					tHeadMsg.m_tMsgDst.m_abyMtIdentify[0] = tMt.GetMtId();
					tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( abyMcuId[0] );
					tHeadMsg.m_tMsgDst.m_tMt.SetMtId( abyMcuId[1] );
				}
				else
				{
					tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( abyMcuId[0] );
					tHeadMsg.m_tMsgDst.m_tMt.SetMtId( tMt.GetMtId() );
					tHeadMsg.m_tMsgDst.m_byCasLevel = 0;
				}
			}*/									
		}
		/*else
		{
			tHeadMsg.m_tMsgDst.m_tMt = tMt;
		}*/

		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		cServMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		
		SendMsgToMt( (u8)tMt.GetMcuId(), MCU_MCU_DELMT_REQ, cServMsg );

		if(cServMsg.GetEventId() == MCU_MCU_DELMT_REQ)
		{
			tMt = m_ptMtTable->GetMt( (u8)tMt.GetMcuId() );
			//tHeadMsgAck.m_tMsgSrc.m_tMt = tMt;
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
			cServMsg.CatMsgBody( (u8*)&tMt,sizeof(tMt) );
		}
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
	}
	if(cServMsg.GetEventId() == MCU_MCU_DELMT_REQ)
	{		
		//tHeadMsgAck.m_tMsgSrc.m_tMt = tMt;
		cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
		cServMsg.CatMsgBody( (u8*)&tMt,sizeof(tMt) );
	}
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	tMt = m_ptMtTable->GetMt( tMt.GetMtId() );

	
	MtOnlineChange( tMt, FALSE, MTLEFT_REASON_NORMAL );

	// xliang [12/24/2008] ��VMP������ѯʱ
	if (m_tConf.m_tStatus.m_tVMPParam.IsVMPBatchPoll())
	{
		//Ҫɾ��MT�����ڵ�ǰ�����У������MT��Ӧ��VMPͨ��
		u8 byMemberId;
		if( m_tVmpPollParam.IsMtInCurPollBlok(tMt.GetMtId(),&byMemberId) )
		{
			StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byMemberId, FALSE, MODE_VIDEO);
		}
	}
	// vrs��¼��֧��
	if (tMt.GetMtType() == MT_TYPE_VRSREC)
	{
		ReleaseVrsMt(tMt.GetMtId());
	}
	else
	{
		RemoveMt( tMt, TRUE );
	}
}

//��Ϣ�壺2 byte(�ն���,������,ֵΪN,0Ϊ�㲥�������ն�) + N��TMt +TROLLMSG
//OSPEVENT( MCS_MCU_SENDRUNMSG_CMD,    EV_MCSVC_BGN + 181 );
/*====================================================================
    ������      ��  
    ����        ������Ϣ���ʹ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/15    1.0         LI Yi         ����
	03/11/25    3.0         ������        �޸�
	10/08/12	4.6			xl			  modify
====================================================================*/
void CMcuVcInst::ProcMcsMcuSendMsgReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
//	CServMsg    cModifyedMsg( pcMsg->content, pcMsg->length );

	u16 wMtNum = *(u16*)cServMsg.GetMsgBody();
//	TMt	*ptMt = (TMt *)( cServMsg.GetMsgBody() + sizeof(u16) );
	CRollMsg* ptROLLMSG = (CRollMsg*)( cServMsg.GetMsgBody() + ntohs(wMtNum)*sizeof(TMt) + sizeof(u16) );

    // xsl [11/3/2006] �������ļ������Ƿ��ն˵Ķ���Ϣת���������е��ն�
    if (MT_MCU_SENDMSG_CMD == pcMsg->event)
    {
        if (!g_cMcuVcApp.IsTransmitMtShortMsg())
        {
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSendMsgReq] not permit transmit mt short message in mcu config\n");
            return;
        }
		//�ն˷�����Ϣ�����Ѿ�����Դ�ı���������ҪԴmt�ˣ���������ʾһ��
		if ( NULL != ptROLLMSG )
		{
			TMTLABEL tSrcMt;
			ptROLLMSG->SetMsgSrcMtId(tSrcMt);
		}
	

// 		// [8/26/2010 xliang] reconstruct msgbody:
// 		TMt	tTmpMt;
// 		cModifyedMsg.SetMsgBody();
// 		cModifyedMsg.SetMsgBody(( u8 *)&wMtNum, sizeof(wMtNum));
// 		for(u16 wNumIdx = 0; wNumIdx < wMtNum; wNumIdx ++)
// 		{
// 			tTmpMt = *(TMt *)( cServMsg.GetMsgBody() + sizeof(u16) + sizeof(TMt) * wNumIdx );
// 			u16 wMcuIdx = GetMcuIdxFromMcuId( (u8)tTmpMt.GetMcuId() );	
// 			u8 byMtId = tTmpMt.GetMtId();
// 			tTmpMt.SetMcuIdx( wMcuIdx );
// 			tTmpMt.SetMtId(byMtId);
// 			cModifyedMsg.CatMsgBody(( u8 *)&tTmpMt, sizeof(tTmpMt));
// 		}
// 		cModifyedMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );
    }

	switch( CurState() )
	{
	case STATE_ONGOING:

		if( wMtNum == 0 )	//���������ն� MCU_MT_SENDMSG_NOTIF�������¼�MCU���ڶԶ��յ�ʱ����
		{
			CServMsg	cSendMsg;
			TMt tMt;
			tMt.SetNull();
			cSendMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
			cSendMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );

			for(u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
			{
				if( m_tConfAllMtInfo.MtJoinedConf(byLoop) && byLoop != cServMsg.GetSrcMtId() )
                {
                    if (m_ptMtTable->GetMtType(byLoop) == MT_TYPE_MT)
                    {
						cServMsg.SetMsgBody( cServMsg.GetMsgBody() , (sizeof(TMt) + ptROLLMSG->GetTotalMsgLen()));
                        SendMsgToMt( byLoop, MCU_MT_SENDMSG_NOTIF, cSendMsg );	
                    }
                    else if(m_ptMtTable->GetMtType(byLoop) == MT_TYPE_SMCU)
                    {
                        SendMsgToMt( byLoop, MCU_MCU_SENDMSG_NOTIF, cSendMsg );
                    }
                }			
			}

			//BroadcastToAllSubMtJoinedConf( MCU_MT_SENDMSG_NOTIF, cSendMsg );
		}
		else //����ĳЩ�ն� 
		{
			if( !m_cSmsControl.IsStateIdle() )
			{
				//notify Error to UI
				NotifyMcsAlarmInfo(0, ERR_MCU_SMSBUSY);
				return;
			}
			
			// [1/7/2011 xliang] send sms through several times
			m_cSmsControl.SetState(CSmsControl::BUSY);
			m_cSmsControl.SetServMsg(cServMsg);

			ProcBatchMtSmsOpr(cServMsg);
// 			TMt	*ptMt = (TMt *)( cModifyedMsg.GetMsgBody() + sizeof(u16) );
// 			for( u16 wLoop = 0; wLoop < ntohs(wMtNum); wLoop++ )
// 			{
// 				ProcSingleMtSmsOpr(cServMsg, ptMt, ptROLLMSG);
// 				ptMt++;
// 			}
		}
		break;
		
	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuGetMtListReq
    ����        ���õ��ն��б�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/27    3.0         ������        ����
	10/12/31    4.6          liuxu        �޸�
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetMtListReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	//�õ�MCU��
	TMcu tMcu = *(TMcu*)cServMsg.GetMsgBody(); 

	u16 wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
	if( !m_tCascadeMMCU.IsNull() &&
		INVALID_MCUIDX == wMMcuIdx )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcsMcuGetMtListReq] Fail to Get m_tCascadeMMCU McuIdx\n" );
	}
	//�����ն��б�
	if( INVALID_MCUIDX == tMcu.GetMcuIdx() )
	{	
		tMcu.SetNull();

		//�ȷ�����MC
		u16 wMcuIdx = INVALID_MCUIDX;						// ������m_tConfAllMtInfo�õ���wMcuIdx
		TConfMtInfo *ptConfMtInfo = NULL;					// ������ʹ�õ�TConfMtInfo
		TConfMcInfo *ptConfMcInfo = NULL;					// ������ʹ�õ�TConfMcInfo
		for( u16 wLoop = 0; wLoop < m_tConfAllMtInfo.GetMaxMcuNum(); wLoop++ )
		{
			// Ϊ��, ��Continue
			if( m_tConfAllMtInfo.GetMtInfo(wLoop).IsNull() )
			{
				continue;
			}

			// liuxu, ��ֱ�ӷ���Ϊָ�����, ��װϸ��
			ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr(wLoop);
			if (NULL == ptConfMtInfo)
			{
				continue;
			}

			wMcuIdx = ptConfMtInfo->GetMcuIdx();
            // guzh [4/30/2007] �ϼ�MCU�б����
			if( !IsLocalMcuId(wMcuIdx)
				&& ( wMcuIdx != wMMcuIdx || g_cMcuVcApp.IsShowMMcuMtList() )
               )      
			{
				SendMtListToMcs(wMcuIdx); // �Ǳ���������mtadp�����󣬲����ϱ�MMCU [pengguofeng 7/2/2013]
/*				ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );				
				if (NULL == ptConfMcInfo)
				{
					continue;
				}

				tMcu.SetMcuIdx( ptConfMtInfo->GetMcuIdx() );
				cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
				
				for(s32 nLoop=0; nLoop<MAXNUM_CONF_MT; nLoop++)
				{
					if( ptConfMcInfo->m_atMtStatus[nLoop].IsNull() || 
						ptConfMcInfo->m_atMtStatus[nLoop].GetMtId() == 0 ) //�Լ�
					{
						continue;
					}

					cServMsg.CatMsgBody((u8 *)&(ptConfMcInfo->m_atMtExt[nLoop]), sizeof(TMtExt));
				}
				
				SendReplyBack( cServMsg, MCU_MCS_MTLIST_NOTIF );
*/
			}
		}

		// [pengjie 2010/5/17] ֪ͨ�����¼���������չ��Ϣ���Ƿ�֧�ֶ�ش���
		SendConfExtInfoToMcs( cServMsg.GetSrcSsnId() );
		// End

	    //��������� 
		tMcu.SetNull();
		tMcu.SetMcu( LOCAL_MCUID );
		tMcu.SetMcuIdx( LOCAL_MCUIDX );		
	}		

	//���Ǳ�����MCU
	if(!tMcu.IsLocal() )
	{
        // guzh [4/30/2007] �ϼ�MCU�б����
        if ( !g_cMcuVcApp.IsShowMMcuMtList() && 
             !m_tCascadeMMCU.IsNull() && wMMcuIdx == tMcu.GetMcuIdx() )
        {
        }
        else
        {
			SendMtListToMcs(tMcu.GetMcuIdx()); // ���¼�������,�����ϱ�MMcu [pengguofeng 7/2/2013]
/*            TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( tMcu.GetMcuId() );
            cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
            for(s32 nLoop=0; nLoop<MAXNUM_CONF_MT; nLoop++)
            {
                if( NULL == ptConfMcInfo ||
					ptConfMcInfo->m_atMtStatus[nLoop].IsNull() || 
                    ptConfMcInfo->m_atMtStatus[nLoop].GetMtId() == 0 ) //�Լ�
                {
                    continue;
                }
                //TMcMtStatus:public TMtStatus
                cServMsg.CatMsgBody((u8 *)&(ptConfMcInfo->m_atMtExt[nLoop]), sizeof(TMtExt));
            }
            
            SendReplyBack( cServMsg, MCU_MCS_MTLIST_NOTIF );
*/
			// [pengjie 2010/5/17] ֪ͨ�����¼���������չ��Ϣ���Ƿ�֧�ֶ�ش���
			SendConfExtInfoToMcs( cServMsg.GetSrcSsnId(),GetFstMcuIdFromMcuIdx(tMcu.GetMcuIdx()) );
			// End
        }        
	}
	else
	{
		tMcu.SetEqpId( m_ptMtTable->m_byMaxNumInUse );

		//�õ����������б�
		cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
		cServMsg.CatMsgBody( ( u8 * )m_ptMtTable->m_atMtExt, 
			                 m_ptMtTable->m_byMaxNumInUse * sizeof( TMtExt ) );

		for (u8 byMtId = 1; byMtId <= m_ptMtTable->m_byMaxNumInUse; byMtId ++)
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

		SendReplyBack( cServMsg, MCU_MCS_MTLIST_NOTIF ); // �������ù� [pengguofeng 7/2/2013]	}
	}
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
}


/*====================================================================
    ������      ��ProcMcsMcuRefreshMcuCmd
    ����        ��ˢ������MCU MT�б�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/2/24     3.6         Jason        ����
	10/12/30    4.6         liuxu        �޸�
====================================================================*/
void CMcuVcInst::ProcMcsMcuRefreshMcuCmd( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	//�õ�MCU��
	TMcu tMcu = *(TMcu*)cServMsg.GetMsgBody(); 

	u16 wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
	if( !m_tCascadeMMCU.IsNull() && !IsValidMcuId(wMMcuIdx))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcsMcuRefreshMcuCmd] Fail to Get m_tCascadeMMCU McuIdx\n" );
	}

	for( u16 wLoop = 0; wLoop < m_tConfAllMtInfo.GetMaxMcuNum(); wLoop++ )
	{
		u16 wMcuIdx = m_tConfAllMtInfo.GetMtInfo(wLoop).GetMcuIdx();
		if( wMcuIdx == tMcu.GetMcuIdx() )
		{
			//�����ն��б�
			TMcuMcuReq tReq;
			memset(&tReq, 0, sizeof(tReq));
			cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
			u8 abyMcuId[MAX_CASCADEDEPTH-1];
			memset( &abyMcuId[0],0,sizeof(abyMcuId) );
			if( m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] ))
			{
				SendMsgToMt( abyMcuId[0], MCU_MCU_MTLIST_REQ,  cServMsg);
			
				if( m_tCascadeMMCU.IsNull() || 
					(!m_tCascadeMMCU.IsNull() && wMMcuIdx != wMcuIdx) )
				{
					//Ϊ�¼�mcu����ȡ�¼�ֱ���ն�״̬
					OnGetMtStatusCmdToSMcu( abyMcuId[0] );
				}
			}
			break;
		}
	}
}


/*====================================================================
    ������      ��ProcMcsMcuGetConfInfoReq
    ����        ���õ�������Ϣ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/27    3.0         ������          ����
    08/11/18    4.5         �ű���          ����MAU��Ϣ��ȡ
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetConfInfoReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	switch (pcMsg->event)
	{
    case MCS_MCU_GETCONFINFO_REQ:
    case MT_MCU_GETCONFINFO_REQ:
        
        //����������Ϣ��
        cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
        break;

    case MCS_MCU_GETMAUSTATUS_REQ:
        {
//             TMcuHdBasStatus tStatus;
//             m_pcBasMgr->GetHdBasStatus(tStatus, m_tConf);
//             cServMsg.SetMsgBody((u8*)&tStatus, sizeof(tStatus));
//             SendReplyBack( cServMsg, cServMsg.GetEventId()+1 );
        }
        break;
    default:
        ConfPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMcsMcuGetConfInfoReq] unexpected msg.%d<%s>!\n",
                                pcMsg->event, OspEventDesc(pcMsg->event));
        break;
	}
    return;
}

/*====================================================================
    ������      ��ProcMcsMcuVMPReq
    ����        ������Vmp��Mcu��֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/10/31    4.0         libo          ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuVMPReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    cServMsg.SetEventId(pcMsg->event);

    VmpCommonReq(cServMsg);
}


/*====================================================================
    ������      ��CheckVmpParam
    ����        ����鲢��������ϳɲ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tVmpParam[IN/OUT] :����ϳɲ���
	              wErrorCode[IN/OUT]:���������������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/06/09    4.5         ���㻪        ������
	10/01/18	4.6.3		Ѧ��		  �޸�
====================================================================*/
BOOL32 CMcuVcInst::CheckVmpParam(const u8 byVmpId, TVMPParam_25Mem& tVmpParam, u16& wErrorCode)
{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	if(tVmpParam.GetMaxMemberNum() > MAXNUM_SDVMP_MEMBER)//8ke 8kh��֧��16�������
	{
		//��Ҫ����ն˿���̨�ϵĲ���������
		return FALSE;
	}
#endif
	
	if (!IsValidVmpId(byVmpId))
	{
		return FALSE;
	}

	//����Ƿ�̬����
	if( tVmpParam.GetVMPStyle() == VMP_STYLE_DYNAMIC )
	{
		//���ǻ��鲻֧���Զ�����ϳ�
		if (m_tConf.GetConfAttrb().IsSatDCastMode()) 
		{
			wErrorCode = ERR_MCU_DCAST_NOOP;
			return FALSE;
		}
		if( !tVmpParam.IsVMPAuto() )
		{
			wErrorCode = ERR_MCU_DYNAMICMUSTBYAUTO;		
			return FALSE;
		}
		return TRUE;
	}  
	
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	// ��Ա���ô�����ֱ�����
	UpdateVmpMembersWithConfInfo(tVmpEqp, tVmpParam);
	
	//zjj20100428 �����ָ���ĳ�Ա�Ƿ��е��ش�mcu�µ��ն�ռ�ö��ͨ�������
	wErrorCode = UpdateVmpMembersWithMultiSpy(tVmpEqp, tVmpParam);
	if (0 != wErrorCode)
	{
		return FALSE;
	}
	
	return TRUE;
}
/*====================================================================
    ������      ��VmpCommonReq
    ����        ������Vmp��Mcu��֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/04    3.0         ������         ����
====================================================================*/
void CMcuVcInst::VmpCommonReq(CServMsg & cServMsg)
{
	
	switch( CurState() )
	{
	case STATE_ONGOING:

		//���Է���ϯ�նˣ�nack
		if( cServMsg.GetSrcMtId() != 0 && 
			cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId() )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_OPER );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			
			//ȡ����ϯ
			SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
				
			return;
		}

		switch(cServMsg.GetEventId())
		{
		case MCS_MCU_STARTVMP_REQ:       //��ؿ�ʼ��Ƶ��������
		case MT_MCU_STARTVMP_REQ:        //��ϯ��ʼ��Ƶ�������� 
			{
				ProcStartVmpReq(cServMsg);
				break;
			}
	    case MCS_MCU_CHANGEVMPPARAM_REQ: //��ػ������̨����MCU�ı���Ƶ���ϲ���
	    case MT_MCU_CHANGEVMPPARAM_REQ:  //��ϯ�������̨����MCU�ı���Ƶ���ϲ���
			{
				ProcChangeVmpParamReq(cServMsg); 
				break;
			}
		case MCS_MCU_START_VMPBATCHPOLL_REQ:// xliang [12/18/2008] ��ʼvmp ������ѯ
			{
				ProcStartVmpBatchPollReq(cServMsg);
				break;
			}
	    case MCS_MCU_STOPVMP_REQ:        //��ؽ�����Ƶ��������	
	    case MT_MCU_STOPVMP_REQ:         //��ϯ������Ƶ��������
			{
				ProcStopVmpReq(cServMsg);
				break;
			}
	    case MCS_MCU_GETVMPPARAM_REQ:    //��ز�ѯ��Ƶ���ϲ�������
	    case MT_MCU_GETVMPPARAM_REQ:     //��ϯ��ѯ��Ƶ���ϲ�������
			{
				TVMPParam_25Mem tVmpParam;
				u8 byVmpId;
				TEqp tVmpEqp;
				cServMsg.SetMsgBody();
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "%s return param:", OspEventDesc(cServMsg.GetEventId()));
				//MCS_MCU_GETVMPPARAM_ACK���ݣ�n*(vmpparam+vmpid)
				for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
				{
					if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
					{
						continue;
					}
					byVmpId = m_abyVmpEqpId[byIdx];
					tVmpEqp = g_cMcuVcApp.GetEqp(byVmpId);
					tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
					cServMsg.CatMsgBody( (u8*)&tVmpParam, sizeof(tVmpParam) );
					cServMsg.CatMsgBody( &byVmpId, sizeof(byVmpId) );
					
					tVmpParam.Print();
				}
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

			}
			break;

		case MCS_MCU_STARTVMPBRDST_REQ:         //�������̨����MCU��ʼ�ѻ���ϳ�ͼ��㲥���ն�
		case MT_MCU_STARTVMPBRDST_REQ:		//��ϯ����MCU��ʼ�ѻ���ϳ�ͼ��㲥���ն�
			{
				ProcStartVmpBrdReq(cServMsg);
				break;
			}

		case MCS_MCU_STOPVMPBRDST_REQ:          //�������̨����MCUֹͣ�ѻ���ϳ�ͼ��㲥���ն�
		case MT_MCU_STOPVMPBRDST_REQ:		//��ϯ����MCUֹͣ�ѻ���ϳ�ͼ��㲥���ն�			
			{
				ProcStopVmpBrdReq(cServMsg);
				break;
			}
		default:
			break;
		}

		break;

	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "Wrong message %u(%s) received in state %u!\n", 
			   cServMsg.GetEventId(), ::OspEventDesc(cServMsg.GetEventId()), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuGetMtListReq
    ����        ������ͼ��ϳ���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/10/31    4.0         libo        ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuVmpTwReq(const CMessage * pcMsg)
{    
    if (STATE_ONGOING != CurState())
    {
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
        pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    cServMsg.SetEventId(pcMsg->event);

    VmpTwCommonReq(cServMsg);
}
/*====================================================================
    ������      ��VmpTwCommonReq
    ����        ������໭�����ǽ�ϳ���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/12/03    3.0         ������        ����
====================================================================*/
void CMcuVcInst::VmpTwCommonReq(CServMsg & cServMsg)
{
    u8 byVmpTwId;
    u8 byVmpTwStyle;
    u8 byLoop;
    TVMPParam_25Mem tVmpTwParam;
    TPeriEqpStatus tPeriEqpStatus;
    TMt tMt;

    //zbq[01/08/2007] �ǿƴ��ն˾ܾ�����VMPTW
    BOOL32 bNonKEDAReject = FALSE;
    TLogicalChannel tLogicChan;

    //�õ����еĻ���ϳ��� ���µ���MPW�Ƿ����� �жϲ��� [12/27/2006-zbq]
    u8 byIdleMPWNum = 0;
    u8 abyIdleMPWId[MAXNUM_PERIEQP];
    memset( abyIdleMPWId, 0, sizeof(abyIdleMPWId) );
    g_cMcuVcApp.GetIdleVmpTw( abyIdleMPWId, byIdleMPWNum, sizeof(abyIdleMPWId) );

    switch(cServMsg.GetEventId())
    {
    case MCS_MCU_STARTVMPTW_REQ:       //��ؿ�ʼ��Ƶ��������

		GetVmpParamFormBuf(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen(), tVmpTwParam);
        if(g_bPrintEqpMsg)
        {
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[VmpTwCommonReq] change vmp param:\n");
            tVmpTwParam.Print();
        }

        //���ڻ�ؿ���ģʽ�£������淽ʽ����Ӧ�նˣ����ն���Ϣ���       
        memset(&tMt, 0, sizeof(TMt));
        for (byLoop = 0; byLoop < tVmpTwParam.GetMaxMemberNum(); byLoop++)
        {
            TVMPMember tVMPMember = *tVmpTwParam.GetVmpMember(byLoop);
            if (0 != tVMPMember.GetMtId())
            {
                if ((VMP_MEMBERTYPE_CHAIRMAN == tVMPMember.GetMemberType() &&
                     !HasJoinedChairman()) ||
                    (VMP_MEMBERTYPE_SPEAKER == tVMPMember.GetMemberType() &&
                     (!HasJoinedSpeaker() || GetLocalSpeaker().GetType() == TYPE_MCUPERI) ) )
                {
                    tMt.SetNull();
                    tVMPMember.SetMemberTMt(tMt);
                    tVmpTwParam.SetVmpMember(byLoop, tVMPMember);
                }
                else if ( m_ptMtTable->GetMtLogicChnnl(tVMPMember.GetMtId(), LOGCHL_VIDEO, &tLogicChan, FALSE) &&
                          MEDIA_TYPE_H264 == tLogicChan.GetChannelType() &&
                          VIDEO_FORMAT_CIF != tLogicChan.GetVideoFormat() &&
						  !IsKedaMt(tVMPMember, TRUE))
                {
                    tMt.SetNull();
                    tVMPMember.SetMemberTMt( tMt );
                    tVmpTwParam.SetVmpMember( byLoop, tVMPMember );
                    
                    bNonKEDAReject = TRUE;
                }                
                else if ( !m_tConfAllMtInfo.MtJoinedConf(tVMPMember) )
                {
                    tMt.SetNull();
                    tVMPMember.SetMemberTMt(tMt);
                    tVmpTwParam.SetVmpMember(byLoop, tVMPMember);
                }
                else
                {
                    if (!tVMPMember.IsLocal())
                    {
                        //�з���
                        OnMMcuSetIn((TMt)tVMPMember, 0, SWITCH_MODE_SELECT);
                        tMt = m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx( tVMPMember.GetMcuIdx() ));
                        tVMPMember.SetMemberTMt(tMt);
                        tVmpTwParam.SetVmpMember(byLoop, tVMPMember);
                    }
                }
            }
        }
        if ( bNonKEDAReject )
        {
            NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_NONKEDAMT_JOINVMP );
        }

        //��鵱ǰ����Ļ���ϳ�״̬
        if (CONF_VMPTWMODE_NONE != m_tConf.m_tStatus.GetVmpTwMode())
        {
            cServMsg.SetErrorCode(ERR_MCU_VMPRESTART);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);

            return;
        }

        //����Ƿ�̬����
        if (VMP_STYLE_DYNAMIC == tVmpTwParam.GetVMPStyle())
        {
            cServMsg.SetErrorCode(ERR_INVALID_VMPSTYLE);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);

            return;
        }

        //�Ƿ��п��еĻ���ϳ��� 
        if (0 == byIdleMPWNum)
        {
            cServMsg.SetErrorCode(ERR_MCU_NOIDLEVMP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);

            return;
        }
        else
        {
            u16 wError = 0;
            if ( !IsMCUSupportVmpStyle(tVmpTwParam.GetVMPStyle(), byVmpTwId, EQP_TYPE_VMPTW, wError) )
            {
                cServMsg.SetErrorCode( ERR_MCU_ALLIDLEVMP_NO_SUPPORT );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
        }

        // xsl [7/20/2006]���Ƿ�ɢ������Ҫ���ش�ͨ����
        if (m_tConf.GetConfAttrb().IsSatDCastMode()) 
        {            
            if (//IsOverSatCastChnnlNum(tVmpTwParam))
				!IsSatMtCanContinue())
            {
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[VmpTwCommonReq-start] over max upload mt num. nack!\n");
                cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
        }

        m_tVmpTwEqp = g_cMcuVcApp.GetEqp(byVmpTwId);
        m_tVmpTwEqp.SetConfIdx(m_byConfIdx);

        // �� ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

        //���û���ϳɲ��� 
        g_cMcuVcApp.GetPeriEqpStatus(byVmpTwId, &tPeriEqpStatus);
        tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::RESERVE;//��ռ��,��ʱ��δ�ɹ��ٷ���
        tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpTwParam);
        tPeriEqpStatus.SetConfIdx(m_byConfIdx);
        g_cMcuVcApp.SetPeriEqpStatus(byVmpTwId, &tPeriEqpStatus);

        // �û��� �Զ�����->���Զ����� ��Ҫ�Ƚ���ֹͣ���������߲�����ֱ��CHANGE����
        byVmpTwStyle = tVmpTwParam.GetVMPStyle();
        m_tConf.m_tStatus.SetVmpTwStyle(byVmpTwStyle);

        //��ʼ����ϳ�
        ChangeVmpTwParam(&tVmpTwParam, TRUE);

        break;

    case MCS_MCU_CHANGEVMPTWPARAM_REQ: //��ػ������̨����MCU�ı���Ƶ���ϲ���
    {
		GetVmpParamFormBuf(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen(), tVmpTwParam);
        if(g_bPrintEqpMsg)
        {
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[VmpTwCommonReq] change vmp param:\n");
            tVmpTwParam.Print();
        }  

        //��鵱ǰ����Ļ���ϳ�״̬ 
        if (CONF_VMPTWMODE_NONE == m_tConf.m_tStatus.GetVmpTwMode())
        {
            cServMsg.SetErrorCode(ERR_MCU_VMPNOTSTART);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            
            return;
        }

        // ���� [5/22/2006] change��ʱ��ҲҪ���
        u16 wError = 0;
        if (!IsVMPSupportVmpStyle(tVmpTwParam.GetVMPStyle(), m_tVmpTwEqp.GetEqpId() , wError ))
        {
            cServMsg.SetErrorCode(wError);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);

            // �л�ʧ�ܣ��������� [12/27/2006-zbq]
            // FIXME: ������̫�����ʣ��д��޸Ļ�ɾȥ����ʱ����
            byVmpTwId = 0;
            if ( IsMCUSupportVmpStyle(tVmpTwParam.GetVMPStyle(), byVmpTwId, EQP_TYPE_VMPTW, wError) )
            {
                NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_OTHERVMP_SUPPORT );
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[VmpTwCommonReq] curr VMPTW.%d can't support the style needed, other VMPTW.%d could\n",
                        m_tVmpTwEqp.GetEqpId(), byVmpTwId );
            }
            return;
        }

        //����Ƿ�̬����
        if (VMP_STYLE_DYNAMIC == tVmpTwParam.GetVMPStyle())
        {
            cServMsg.SetErrorCode(ERR_INVALID_VMPSTYLE);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);

            return;
        }      
        
        //���û���ϳɲ���
        for (byLoop = 0; byLoop < tVmpTwParam.GetMaxMemberNum(); byLoop++)
        {
            TVMPMember tVMPMember = *tVmpTwParam.GetVmpMember(byLoop);
            if (0 != tVMPMember.GetMtId() && !tVMPMember.IsLocal())
            {
                //�з���
                OnMMcuSetIn((TMt)tVMPMember, 0, SWITCH_MODE_SELECT);
                TMt tTempMt;
                tTempMt = m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx(tVMPMember.GetMcuId()) );
                tVMPMember.SetMemberTMt(tTempMt);
                tVmpTwParam.SetVmpMember(byLoop, tVMPMember);
            }
        }

        // xsl [7/20/2006]���Ƿ�ɢ������Ҫ���ش�ͨ����
        if (m_tConf.GetConfAttrb().IsSatDCastMode()) 
        {            
            if (//IsOverSatCastChnnlNum(tVmpTwParam))
				!IsSatMtCanContinue())
            {
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[VmpTwCommonReq-change] over max upload mt num. nack!\n");
                cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
        }

        //ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId() , &tPeriEqpStatus);
        tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpTwParam);
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);

        // �ı�ϳɲ��� 
        ChangeVmpTwParam(&tVmpTwParam);
        break;
    }
    case MCS_MCU_STOPVMPTW_REQ:        //��ؽ�����Ƶ��������	

        //��鵱ǰ����Ļ���ϳ�״̬ 
        if (CONF_VMPTWMODE_NONE == m_tConf.m_tStatus.GetVmpTwMode())
        {
            cServMsg.SetErrorCode(ERR_MCU_VMPNOTSTART);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        //ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

        //ֹͣ
        g_cEqpSsnApp.SendMsgToPeriEqpSsn(m_tVmpTwEqp.GetEqpId(), MCU_VMPTW_STOPVIDMIX_REQ, 
                                         (u8 *)cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        break;

//    case MCS_MCU_GETVMPPARAM_REQ:    //��ز�ѯ��Ƶ���ϲ�������
//    tVmpParam = m_tConf.m_tStatus.GetVmpParam();
//    cServMsg.SetMsgBody( (u8*)&tVmpParam, sizeof(TVMPParam) );
//    SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
//
//    g_cEqpSsnApp.SendMsgToPeriEqpSsn( m_tVmpEqp.GetEqpId(), 
//    MCU_VMP_GETVIDMIXPARAM_REQ, NULL, 0 );
//        break;
	default:
		break;
    }

}

/*====================================================================
    ������      ��ProcMcsMcuGetConfStatusReq
    ����        ���õ�����״̬������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/27    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetConfStatusReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

    cServMsg.SetMsgBody( ( u8 * const )&m_tConf.m_tStatus, sizeof( TConfStatus ) );	
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
}

/*====================================================================
    ������      ��ProcMcsMcuMcuMediaSrcReq
    ����        ���õ�����״̬������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/2/27    3.6         Jason       ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuMcuMediaSrcReq( const CMessage * pcMsg )
{
	if (NULL == m_ptConfOtherMcTable)
	{
		return;
	}

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMcu tMcu =  *(TMcu *)cServMsg.GetMsgBody(); 

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuMcuMediaSrcReq] McuMcuId.%d - McuMtId.%d\n", 
		     tMcu.GetMcuIdx(), tMcu.GetMtId() );

	cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu) );
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
	u16 wMcuIdx = 0;
	//u8 byFstMcuId = 0,bySecMcuId = 0;
	u8 abyMcuId[MAX_CASCADEDEPTH-1];
	memset( &abyMcuId[0],0,sizeof(abyMcuId) );
	
	TMt tMt;

	TConfMcInfo *pMcInfo = NULL;
	for(s32 nLoop=0; nLoop < TConfOtherMcTable::GetMaxMcuNum(); nLoop++)
	{
		pMcInfo = m_ptConfOtherMcTable->GetMcInfo(nLoop);
		if (NULL == pMcInfo)
		{
			continue;
		}

		wMcuIdx = pMcInfo->GetMcuIdx();
		if( !IsValidSubMcuId(nLoop))
		{
			continue;
		}

		if( !m_tConfAllMcuInfo.GetMcuIdByIdx(wMcuIdx,&abyMcuId[0]) )
		{
			continue;
		}
		
		tMt = GetMcuMediaSrc(wMcuIdx);
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcTimerMcuSrcCheck] McuId.%d - MtMcuId.%d MtId.%d MtType.%d MtConfIdx.%d\n", 
				 wMcuIdx, tMt.GetMcuId(), tMt.GetMtId(), tMt.GetMtType(), tMt.GetConfIdx() );
		
		tMcu.SetMcuId(wMcuIdx);
		cServMsg.SetEventId(MCU_MCS_MCUMEDIASRC_NOTIF);
		cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
		cServMsg.CatMsgBody((u8 *)&tMt, sizeof(tMt));
		
		SendReplyBack(cServMsg, MCU_MCS_MCUMEDIASRC_NOTIF);
	}

	return;
}


/*====================================================================
    ������      ProcMcsMcuLockSMcuReq
    ����        �������¼�MCU����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/2/27    3.6         Jason       ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuLockSMcuReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
 	TMcu *ptMcu = (TMcu *)cServMsg.GetMsgBody();
	u8 byLock = *(cServMsg.GetMsgBody()+sizeof(TMcu));

	if( NULL == m_ptConfOtherMcTable )
	{
		cServMsg.SetErrorCode( ERR_LOCKSMCU_NOEXIT );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	
	u16 wMMcuIdx = INVALID_MCUIDX;
	if( !m_tCascadeMMCU.IsNull() )
	{
		wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
	}
	if( (!ptMcu->IsNull() && ptMcu->IsLocal() ) || 
		( !m_tCascadeMMCU.IsNull() && ptMcu->GetMcuIdx() == wMMcuIdx/*m_tCascadeMMCU.GetMtId()*/ ) )
	{
		cServMsg.SetErrorCode( ERR_LOCKSMCU_NOEXIT );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	/* [2012/1/10 zhangli]byLock=1������=0����������ʱ�ѻ��������ģʽm_byLockMode����ΪCONF_LOCKMODE_LOCK(=2)��
	�����¼���ֱ�Ӱ�byLock(=1)��ֵ�������棬���½�����ʾΪCONF_LOCKMODE_NEEDPWD(=1)ģʽ���������byLock����ȷ��ģʽ�����¼�*/
	u8 byLockMode = CONF_LOCKMODE_NONE;
	if (ISTRUE(byLock))
	{
		byLockMode = CONF_LOCKMODE_LOCK;
	}

	CServMsg cMsg;
	cMsg.SetServMsg(cServMsg.GetServMsg(), cServMsg.GetServMsgLen()-sizeof(u8));
	cMsg.CatMsgBody(&byLockMode, sizeof(u8));
	cMsg.SetEventId(MCU_MCU_LOCK_REQ);

	TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptMcu);
	u8   byMcuNum = 0;
	TConfMcInfo *ptMcInfo = NULL;
	
	if( ptMcu && !ptMcu->IsNull() && INVALID_MCUIDX != ptMcu->GetMcuIdx() )
	{
		ptMcInfo = m_ptConfOtherMcTable->GetMcInfo( ptMcu->GetMcuIdx() );
		if( NULL == ptMcInfo )
		{
			cServMsg.SetErrorCode( ERR_LOCKSMCU_NOEXIT );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
		SendMsgToMt( GetFstMcuIdFromMcuIdx( tLocalMt.GetMcuIdx() ), MCU_MCU_LOCK_REQ, cMsg);
	}
	else
	{
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
				SendMsgToMt( GetFstMcuIdFromMcuIdx(ptMcInfo->GetMcuIdx()), MCU_MCU_LOCK_REQ, cMsg);
			}
			
			byMcuNum++;
		}

		if( 0 == byMcuNum )
		{
			cServMsg.SetErrorCode( ERR_LOCKSMCU_NOEXIT );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
	}

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
}

/*====================================================================
    ������      ��ProcMcsMcuVACReq
    ����        �������������ƴ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/27    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuVACReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    STATECHECK

    // xsl [8/19/2006] ���Ƿ�ɢ���鲻֧����������
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
		cServMsg.SetErrorCode( ERR_MCU_DCAST_NOOP );            
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcMcsMcuVACReq] distributed conf not supported vac!\n");
        return;
    }

    MixerVACReq(cServMsg);

    return;
}

/*====================================================================
    ������      ��ProcMcsMcuVACReq
    ����        �������������ƴ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/27    3.0         ������        ����
====================================================================*/
void CMcuVcInst::MixerVACReq(CServMsg & cServMsg)
{
    TPeriEqpStatus tPeriEqpStatus;
    u8 byEqpId = 0;

    //���Է���ϯ�նˣ�nack
    if (cServMsg.GetSrcMtId() != 0 &&
        cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId())
    {
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR: Message is from ordinary terminal!\n");
        cServMsg.SetErrorCode(ERR_MCU_INVALID_OPER);
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);

        //ȡ����ϯ
        SendReplyBack(cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF);
        return;
    }

    u16 wError = 0;
    switch (cServMsg.GetEventId())
    {		
    case MCS_MCU_STARTVAC_REQ:        //�������̨����MCU��ʼ�����������Ʒ���
    case MT_MCU_STARTVAC_REQ:
        
        //�Ѵ��������������Ʒ���״̬, NACK
        if (m_tConf.m_tStatus.IsVACing())
        {
			ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq]ERROR: already in VAC mode.so nack!\n");
            cServMsg.SetErrorCode( ERR_MCU_CONFINVACMODE );
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
		//���ڷ��񣬲��ܿ����������� 
		if( m_tConf.m_tStatus.IsPlaying() 
			|| m_tConf.m_tStatus.IsPlayReady())
		{
			cServMsg.SetErrorCode( ERR_MCU_CONFPLAYING );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			ConfPrint( LOG_LVL_WARNING, MID_MCU_MIXER, "[MixerVACReq]Conf(%s) playing now!So Can't StartVac!\n", 
				m_tConf.GetConfName() );
			return;
		}
        //û�п��еĻ�����
        if(m_tConf.m_tStatus.IsNoMixing())
        {
			TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];
			memset(atAudioTypeDesc, 0, sizeof(atAudioTypeDesc));
			u8 byAudioCapNum = m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);

            byEqpId = g_cMcuVcApp.GetIdleMixer(0, byAudioCapNum, atAudioTypeDesc); // �õ����е��һ����������Ļ�����
            if( byEqpId == 0 )
            {
                ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR: no idle MIXER!\n");
                cServMsg.SetErrorCode(ERR_MCU_NOIDLEMIXER_INCONF);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
                return;
            }
			if (!CheckMixerIsValide(byEqpId))
			{
                ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR: mixer(%d) is invalid!\n",byEqpId);                cServMsg.SetErrorCode(ERR_MCU_NOIDLEMIXER_INCONF);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
        }        
        else
        {
            byEqpId =  m_tMixEqp.GetEqpId();
        }

        // ����ѿ�ʼ��ѯ�����ܿ�ʼ��������
		// mqs [03/21/2011] ��Ϊ���߶������÷����ˣ����Ҳ��Բ�ͬ�����Բ���ͬʱ����
        if (m_tConf.m_tStatus.GetPollMode() != CONF_POLLMODE_NONE) 
        {
			ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR:ConfPolling can't start vac!\n");
            cServMsg.SetErrorCode(ERR_MCU_CONFSTARTPOLLVACMODE);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        // xsl [7/24/2006] ���ƻ������ܿ�����������
        if (m_tConf.m_tStatus.IsSpecMixing())
        {
            ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR:MixSpec can't start VAC!\n");
            cServMsg.SetErrorCode(ERR_MCU_VACMUTEXSPECMIX);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        // guzh [1/30/2007] 8000B ��������        
        if ( ! CMcuPfmLmt::IsVacOprSupported( m_tConf, m_tConfAllMtInfo.GetLocalJoinedMtNum(), wError ) )
        {
			ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR: 8000b's capacity limit!\n");
            cServMsg.SetErrorCode(wError);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
        
		//20100708_tzy �������ն������ڻ���������ʱ�ܾ�
        if (m_tConfAllMtInfo.GetLocalJoinedMtNum() > GetMaxMixNum(byEqpId))
        {
			ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR: JoinedMtNum(%d)  exceed Mixer(%d)'s capacity(%d). nack!\n",
				     m_tConfAllMtInfo.GetLocalJoinedMtNum(), byEqpId, GetMaxMixNum(byEqpId));

            cServMsg.SetErrorCode(ERR_MCU_OVERMAXMIXERCHNNL);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
        }
       
		//ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

		//lukp[12/07/2009] ����MCU_MT_STARTVAC_NOTIF �� MCU_MT_STARTVAC_ACK�ظ�mt����Ϣһ����
		//�ڴ˹��˵�MT_MCU_STARTVAC_REQ
		if (HasJoinedChairman() && (MT_MCU_STARTVAC_REQ != cServMsg.GetEventId()))
		{
			SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STARTVAC_NOTIF, cServMsg);
        }

        if (m_tConf.m_tStatus.IsMixing())
        {
			u8 byMixerSubtype = UNKONW_MIXER;
			g_cMcuAgent.GetMixerSubTypeByEqpId(m_tMixEqp.GetEqpId(),byMixerSubtype);
            if (byMixerSubtype == EAPU_MIXER || byMixerSubtype == APU2_MIXER)
            {
                //������������£�ֻҪ���Ϳ���������������
                SendMsgToEqp(m_tMixEqp.GetEqpId(),MCU_MIXER_STARTVAC_CMD,cServMsg);
            }
            m_tConf.m_tStatus.SetVACing();
            SetMixerSensitivity(m_tConf.GetTalkHoldTime());

            u8 byMtId = m_tVacLastSpeaker.GetMtId();

            if (!m_tVacLastSpeaker.IsNull() &&
                byMtId != GetLocalSpeaker().GetMtId() &&
                !m_ptMtTable->IsMtAudioDumb(byMtId) &&
                (!m_tConf.m_tStatus.IsSpecMixing() ||
                (m_tConf.m_tStatus.IsSpecMixing() && m_ptMtTable->IsMtInMixing(byMtId))))
            {
                //change video switch
                ChangeSpeaker(&m_tVacLastSpeaker);
            }

            //֪ͨ��ϯ�����л��
            SendMsgToAllMcs(MCU_MCS_STARTVAC_NOTIF, cServMsg);

            //zbq [11/26/2007] �����ϱ�
            MixerStatusChange();
        }
        else
        {
			u8 byMixerSubtype = UNKONW_MIXER;
			g_cMcuAgent.GetMixerSubTypeByEqpId(byEqpId,byMixerSubtype);
            if (byMixerSubtype == EAPU_MIXER || byMixerSubtype == APU2_MIXER )
            {
                StartEmixerVac(byEqpId);
            }
            else//APUʱ��ԭ���߼�
            {
                StartMixing( mcuVacMix ,byEqpId);
            }
        }
        break;

    case MCS_MCU_STOPVAC_REQ:     //�������̨����MCUֹͣ�����������Ʒ���	
    case MT_MCU_STOPVAC_REQ:      //�ն�����MCUֹͣ�����������Ʒ���
        
		//��δ���ڴ��������������Ʒ���״̬, NACK
        if (!m_tConf.m_tStatus.IsVACing())
        {
			ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR: Conf is not VAC. So can't stop!\n");
            cServMsg.SetErrorCode(ERR_MCU_CONFNOTINVACMODE);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
        //ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
        
        if (m_tConf.m_tStatus.IsNoMixing())
        {
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, "[MixerVACReq] begin to stopVacing!\n");
            
			u8 byMixerSubtype = UNKONW_MIXER;
			g_cMcuAgent.GetMixerSubTypeByEqpId(m_tMixEqp.GetEqpId(),byMixerSubtype);
            if (byMixerSubtype == EAPU_MIXER || byMixerSubtype == APU2_MIXER)
            {
                SendMsgToEqp(m_tMixEqp.GetEqpId(),MCU_MIXER_STOPVAC_CMD,cServMsg);
                g_cMcuVcApp.GetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);
                tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[0].m_byGrpState = TMixerGrpStatus::READY;
				tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpId = 0;
				tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byConfId = 0;
                g_cMcuVcApp.SetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);
                m_tConf.m_tStatus.SetVACing(FALSE);
                //ֹͣ���������������
                TMt            tMt;
                for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
                {
                    memset(&tMt,0,sizeof(TMt));
                    if (m_tConfAllMtInfo.MtJoinedConf(byMtId))            
                    {
                        tMt = m_ptMtTable->GetMt( byMtId );
                        StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
                            (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt)), FALSE, MODE_AUDIO );
                        m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);
                    }   
                }
                if (HasJoinedChairman())
                {
                    SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cServMsg );
                }
                
				//֪ͨ��ϯ�����л��
                SendMsgToAllMcs(MCU_MCS_STOPVAC_NOTIF, cServMsg);
                
				//����״̬֪ͨ,֪ͨ��ϯ�����л��
                MixerStatusChange();            
                NotifyChairmanMixMode();
                if ( !m_tCascadeMMCU.IsNull() )
                {
                    OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId());
                }
                cServMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
    
                m_tVacLastSpeaker.SetNull();
				TMixParam tMixParam ;
				m_tConf.m_tStatus.SetMixerParam(tMixParam);
                m_tMixEqp.SetNull();
				m_byLastMixMode = mcuNoMix;
				memset( m_abyMixMtId, 0, sizeof(m_abyMixMtId) );
				memset(m_atMixMt,0,sizeof(m_atMixMt));
            }
            else//APUʱ��ԭ���߼�
            {
                StopMixing();
            }
        }
        else
        {
			u8 byMixerSubtype = UNKONW_MIXER;
			g_cMcuAgent.GetMixerSubTypeByEqpId(m_tMixEqp.GetEqpId(),byMixerSubtype);
            if (byMixerSubtype == EAPU_MIXER || byMixerSubtype == APU2_MIXER)
            {
                SendMsgToEqp( m_tMixEqp.GetEqpId(), MCU_MIXER_STOPVAC_CMD, cServMsg );
            }
            // guzh [11/7/2007] ֻ����״̬
            m_tConf.m_tStatus.SetVACing(FALSE);
            //֪ͨ��ϯ�����л��
            SendMsgToAllMcs(MCU_MCS_STOPVAC_NOTIF, cServMsg);
            if (HasJoinedChairman())
            {
                SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cServMsg);
            }
            //zbq [11/26/2007] �����ϱ�
            MixerStatusChange();  
			m_byLastMixMode = mcuWholeMix;
        }

        break;

    default:
        break;
    }
}
/*====================================================================
    ������      ��ProcMcsMcuMixReq
    ����        ���������ƴ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/27    3.0         ������        ����
====================================================================*/		
void CMcuVcInst::ProcMcsMcuMixReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	

	STATECHECK;

    switch(cServMsg.GetEventId())
    {
    case MCS_MCU_STARTDISCUSS_REQ:      //��ؿ�ʼ���ܻ��� - ���ڱ���
        ProcMixStart(cServMsg);
        break;
        
    case MCS_MCU_STOPDISCUSS_REQ:       //���ֹͣ���ܻ��� - ���ڱ���ͣ
        ProcMixStop(cServMsg);
        break;

    case MCS_MCU_GETMIXPARAM_REQ:       //�������̨��ѯ�������� - ���ڱ���ͣ
        {
            u8 byMixMode = m_tConf.m_tStatus.GetMixerParam().GetMode();
            cServMsg.SetMsgBody((u8*)&byMixMode, sizeof(u8));
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);		
            break;
        }
    default:	break;
    }
    return;
}

/*=============================================================================
  �� �� ���� ProcMcsMcuChgMixDelayReq
  ��    �ܣ� ������ʱ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::ProcMcsMcuChgMixDelayReq(const CMessage * pcMsg)
{
    STATECHECK

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if(0 == m_tMixEqp.GetEqpId())
    {
        cServMsg.SetErrorCode(ERR_MCU_CONFNOTMIXING);
        SendReplyBack(cServMsg, pcMsg->event+2);
        return;
    }
    
    u16 wDelayTime = *(u16 *)cServMsg.GetMsgBody();

    m_tConf.SetMixDelayTime(ntohs(wDelayTime));
    SetMixDelayTime(wDelayTime);

    SendReplyBack(cServMsg, pcMsg->event+1);
    SendMsgToAllMcs(MCU_MCS_CHANGEMIXDELAY_NOTIF, cServMsg);

    return;
}

/*====================================================================
    ������      ��ProcStopVmpPoll
    ����        ��ֹͣVmp��ͨ����ѯ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    12/05/02    4.7         �־         ����
====================================================================*/
void CMcuVcInst::ProcStopVmpPoll( void )
{
	u8 byPollState = m_tVmpPollParam.GetPollState();
	// ֹͣ��ѯʱ,vmp��ѯͨ����������,ֻ��KillTimer����
    KillTimer(MCUVC_VMPPOLLING_CHANGE_TIMER); 

    ConfPrint( LOG_LVL_DETAIL, MID_MCU_VMP, "[ProcStopVmpPoll] stop vmp channel poll.\n");

	// ͣ��ѯ����ѯ״̬
    m_tVmpPollParam.SetPollState(POLL_STATE_NONE);
	m_tVmpPollParam.SetMediaMode(MODE_NONE);
	
	//ֹͣ��ѯ��ʱ�������ǰ��ѯ�ն�
	TMtPollParam tMtPollParam = m_tVmpPollParam.GetMtPollParam();
	TMt tNullMt;
	tNullMt.SetNull();
	tMtPollParam.SetTMt(tNullMt);
	m_tVmpPollParam.SetMtPollParam(tMtPollParam);

	// ԭ������ѯ��,ֹͣʱ����淢֪ͨ
	if (POLL_STATE_NONE != byPollState)
	{
		CServMsg cServMsg;
		cServMsg.SetMsgBody((u8 *)&m_tVmpPollParam, sizeof(m_tVmpPollParam));
		SendMsgToAllMcs(MCU_MCS_VMPPOLLSTATE_NOTIF, cServMsg);
	}

	return;
}

/*====================================================================
    ������      ��GetNextMtPolledForVmp
    ����        ���õ���һ�����ϱ���ѯ�������ն�,����vmp��ͨ����ѯ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[IN/OUT] u8 &byPollPos ��ǰ/��һ����ѯ���ն�
                  [IN/OUT] TPollInfo& tPollInfo  ��ѯ����Ϣ
    ����ֵ˵��  ��TMtPollParam *������Ҳ�����һ�����򷵻� NULL
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    12/05/02    4.7         �־         ����
====================================================================*/
TMtPollParam *CMcuVcInst::GetNextMtPolledForVmp(u8 &byPollPos, TPollInfo& tPollInfo)
{
    // ���ȼ���û��Ƿ�ָ������ѯλ��
    if ( m_tVmpPollParam.IsSpecPos() )
    {
        byPollPos = m_tVmpPollParam.GetSpecPos();
        m_tVmpPollParam.ClearSpecPos();
        if ( byPollPos == POLLING_POS_START )
        {
            // ��ͷ��ʼ��ѯ
            byPollPos = 0;
        }
    }
    else
    {
        byPollPos ++;
    }   

    u8 wFindMtNum = 0;
    BOOL32 bJoined = FALSE;
    BOOL32 bSendVideo = FALSE;
	BOOL32 bVideoLose = FALSE;
	
	// [7/1/2010 xliang] �����ش����ˣ���Ŀǰ���������ˣ��ϳɣ�
	BOOL32 bCasSpyFiltered		 = FALSE;
    BOOL32 bCasMcuSpeaker		 = FALSE;
	BOOL32 bCasVmpMemberFiltered = FALSE;

    TMtPollParam tCurMtParam, *ptMtPollParam = NULL;
    do 
    {
        // ��һ�ֵ���ѯ��ʼ
        if (byPollPos >= m_tVmpPollParam.GetPolledMtNum())
        {
            // ������ѯ������������ķ���
            u32 dwPollNum = tPollInfo.GetPollNum();
            if (1 != dwPollNum)
            {
                // rewind to top
                byPollPos = 0;
                if (0 != dwPollNum)
                {
                    tPollInfo.SetPollNum(dwPollNum - 1);
                }            
            }
            else
            {
                // polling to end, all over
                tPollInfo.SetPollNum(0);
                return NULL;
            }
        }
        
		ptMtPollParam = m_tVmpPollParam.GetPollMtByIdx(byPollPos);
        if ( NULL != ptMtPollParam)
        {
			tCurMtParam = *ptMtPollParam;
        }
		else
		{
            break;
		}

        if (tCurMtParam.IsLocal())
        {
            TMtStatus tMtStatus;
            m_ptMtTable->GetMtStatus(tCurMtParam.GetMtId(), &tMtStatus);
            bJoined = m_tConfAllMtInfo.MtJoinedConf( tCurMtParam.GetMtId() );
            bSendVideo = tMtStatus.IsSendVideo();
            
            // guzh [7/27/2006] �����϶������¼�MCU�����ˣ�һ��Ҫ��ֵ
            bCasMcuSpeaker = FALSE;

			bCasVmpMemberFiltered = FALSE;

            // zbq [03/09/2007] ������ǰû����Ƶ���ն�
            bVideoLose = tMtStatus.IsVideoLose();
			
        }
        else
        {
            TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tCurMtParam.GetMcuId());
            if (NULL != ptMcInfo)
            {
                TMcMtStatus *pMcMtStatus = ptMcInfo->GetMtStatus((TMt &)tCurMtParam);
                if (NULL != pMcMtStatus)
                {
                    bJoined = m_tConfAllMtInfo.MtJoinedConf(tCurMtParam.GetMcuId(), tCurMtParam.GetMtId());

					//�����ն�״̬���ֶη�ʵʱ�ϱ�����������list����ʱ�¼�MCU���ն�״̬. ���������¼��ն˴��߼�ͨ������Ӧ������������.
					bSendVideo = TRUE;
					bCasVmpMemberFiltered = FALSE;
					
					// [7/1/2010 xliang]  ����ϼ�������ָ������ʹ�¼�ĳ�ն��ڻش������ڵ��ش�����������.
					// ���磺
					// 1,�ϼ������¼�ĳ�ն���ȫ�ַ����ˣ���������ѯ�����¼���Ա
					// 2,VMP����ָ�����¼���Ա����������������ѯ�����¼���Ա
					// 2011-11-3 add by pgf
					// 3.�Ѿ��е��ش�MCU�ĳ�Ա��������VMP�У�������ڴ�MCU�µ���һ��ѯ�ն�
								
					if( !IsLocalAndSMcuSupMultSpy(tCurMtParam.GetMcuId()) )
					{
						bCasMcuSpeaker = ( GetFstMcuIdFromMcuIdx(m_tConf.GetSpeaker().GetMcuIdx()) == GetFstMcuIdFromMcuIdx(tCurMtParam.GetMcuIdx()));
						
						TEqp tVmpEqp = g_cMcuVcApp.GetEqp(GetTheOnlyVmpIdFromVmpList());
						TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
						if( tVmpParam.GetVMPMode() != CONF_VMPMODE_NONE)
						{
							if( tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_VMPCHLPOLL))
							{
								for (u8 byChnnl = 0; byChnnl < MAXNUM_VMP_MEMBER; byChnnl++)
								{
									// compare VmpMember with tCurPollMt
									TVMPMember *ptVmpMember = tVmpParam.GetVmpMember(byChnnl);
									
									// pass Null Member or MEMBERTYPE_POLL
									if ( ptVmpMember == NULL || ptVmpMember->IsNull()
										|| ptVmpMember->GetMemberType() == VMP_MEMBERTYPE_VMPCHLPOLL)
									{
										continue;
									}
									
									// �Ƚ������ڱ����ı�ʾ����
									TMt tTempMt = (TMt)(*ptVmpMember);
									if ( GetLocalMtFromOtherMcuMt(tCurMtParam.GetTMt()) == GetLocalMtFromOtherMcuMt(tTempMt))
									{
										ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "VmpMember<%d %d>(chnnl:%d) and PollMt<%d %d> are in same mcu\n",
											ptVmpMember->GetMcuIdx(), ptVmpMember->GetMtId(), byChnnl,
											tCurMtParam.GetMcuIdx(), tCurMtParam.GetMtId());
										bCasVmpMemberFiltered = TRUE;
										break;
									}
									
								}
							}
						}

					}
					else //����¼�mcu֧�ֶ�ش��򲻱ؿ����ϼ��Ƿ��лش�Դ
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "multiSpy support, no need to casfilter!\n");
						bCasMcuSpeaker = FALSE;
						bCasVmpMemberFiltered = FALSE;
					}

                    // ������ǰû����Ƶ���¼�MCU�ն�
                    bVideoLose = pMcMtStatus->IsVideoLose();
                }
            }
        }

		// 20120604 yhz ���
		//������MPU��8khvmpҲ֧��vmp��ͨ����ѯ,�ն˲��ܽ�vmp��ͨ��ʱ,�˴���׷���ж�,������������ͨ�����ն�
		
		bCasSpyFiltered	= ( bCasMcuSpeaker || bCasVmpMemberFiltered );

		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[GetNextMtPolledForVmp] SpeakerFilter:(%d), VmpFilter:(%d)!\n", 
			bCasMcuSpeaker, bCasVmpMemberFiltered );
		
        if ( bJoined &&  
             !bCasSpyFiltered &&
             !bVideoLose &&
             bSendVideo 
            )
        {
            break;
        }
        else
        {
            ConfPrint( LOG_LVL_DETAIL, MID_MCU_VMP, "[GetNextMtPolledForVmp] MT(%d,%d) skipped for Joined.%d, SendVideo.%d, SpeakerFilter.%d, VmpFilter.%d!\n", 
                tCurMtParam.GetMcuId(), 
                tCurMtParam.GetMtId(),
                bJoined, 
                bSendVideo, 
                bCasMcuSpeaker,
				bCasVmpMemberFiltered);
        }

        byPollPos++;
        wFindMtNum++;

    // ֱ������һȦ
    }while (wFindMtNum<m_tVmpPollParam.GetPolledMtNum());

    if (wFindMtNum == m_tVmpPollParam.GetPolledMtNum())
    {
        return NULL;
    }
     
    // ��֤��ǰ��ѯ��Ϣ����ȷ��
    tPollInfo.SetMtPollParam( tCurMtParam );
    TMtPollParam *ptNextMt = m_tVmpPollParam.GetPollMtByIdx(byPollPos);    
    return ptNextMt;
}

/*====================================================================
    ������      ��ProcVmpPollingChangeTimerMsg
    ����        ����ʱ��ѯ��Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    12/05/02    4.7         �־         ����
====================================================================*/
void CMcuVcInst::ProcVmpPollingChangeTimerMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
    TMtPollParam * ptCurPollMt;
	
    switch(CurState())
    {
    case STATE_ONGOING:
    {
        if (POLL_STATE_NONE == m_tVmpPollParam.GetPollState()) 
        {
            break;
        }

		if ( 0 == m_tVmpPollParam.GetPolledMtNum() )
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_VMP, "[ProcVmpPollingChangeTimerMsg] PolledMtNum is 0. stop poll\n");
            ProcStopVmpPoll();
			break;
		}		
        
		// ��û���ϳ���vmp��ͨ�������ͨ����
		TEqp tVmpEqp = g_cMcuVcApp.GetEqp(GetTheOnlyVmpIdFromVmpList());
		TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
		u8 byVmpPollChlId = tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_VMPCHLPOLL);
		if (MAXNUM_VMP_MEMBER == byVmpPollChlId)
		{
			ProcStopVmpPoll();
			ConfPrint( LOG_LVL_WARNING, MID_MCU_VMP, "[ProcVmpPollingChangeTimerMsg] There is no channel for VmpChnlPoll. stop poll\n");
			break;
		}
		
		// ��ͣʱ,������ˢ״̬,������ѯ�б��и���
		if (POLL_STATE_PAUSE == m_tVmpPollParam.GetPollState())
		{
			cServMsg.SetMsgBody( (u8*)&m_tVmpPollParam, sizeof(m_tVmpPollParam) );
			SendMsgToAllMcs( MCU_MCS_VMPPOLLSTATE_NOTIF, cServMsg );
            break;
		}

		TVmpPollInfo tPollInfo = m_tVmpPollParam.GetVmpPollInfo();
		
        // xliang [3/31/2009] ��¼�ϴ��ֵ���MT
		TMt tLastPolledMt = tPollInfo.GetMtPollParam().GetTMt(); 
        
        //��¼��ѯ�Ƿ��д�������
		BOOL32 bIsPollNumNoLimit = ( 0 == tPollInfo.GetPollNum() ) ? TRUE : FALSE; 
		u8 byPollIdx = m_tVmpPollParam.GetCurrentIdx();

		// �õ���һ�����ϱ���ѯ�������ն�
		ptCurPollMt = GetNextMtPolledForVmp(byPollIdx, tPollInfo); 
		
		if (ptCurPollMt == NULL)
		{
			ProcStopVmpPoll();
			ConfPrint( LOG_LVL_WARNING, MID_MCU_VMP, "[ProcVmpPollingChangeTimerMsg] Err, Get CurPollMt is Null, LastMt:(%d, %d). stop poll\n", tLastPolledMt.GetMcuId(), tLastPolledMt.GetMtId());
			return;
		}

		ConfPrint( LOG_LVL_DETAIL, MID_MCU_VMP, "[ProcVmpPollingChangeTimerMsg] Get CurPollMt is (%d, %d), LastMt:(%d, %d).\n",
			ptCurPollMt->GetMcuId(), ptCurPollMt->GetMtId(), tLastPolledMt.GetMcuId(), tLastPolledMt.GetMtId());

		m_tVmpPollParam.SetVmpPollInfo(tPollInfo);
		m_tVmpPollParam.SetCurrentIdx(byPollIdx);
	
		// ����Timer
		SetTimer(MCUVC_VMPPOLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());
		
		//֪ͨ���л��
		m_tVmpPollParam.SetMtPollParam(*ptCurPollMt);
		cServMsg.SetMsgBody( (u8*)&m_tVmpPollParam, sizeof(m_tVmpPollParam) );
		SendMsgToAllMcs( MCU_MCS_VMPPOLLSTATE_NOTIF, cServMsg );

		TVMPMember *ptVmpMember = tVmpParam.GetVmpMember(byVmpPollChlId);
		// �¾���ѯ�ն˲�һ��ʱ,��������ϳ�,vmp���ڴ���ʱ������vmp��Ա
		if (NULL != ptVmpMember && !((TMt)*ptCurPollMt == (TMt)*ptVmpMember) && IsChangeVmpParam())
		{
			ptVmpMember->SetMt((TMt)*ptCurPollMt);
			tVmpParam.SetVmpMember(byVmpPollChlId, *ptVmpMember);
			/*TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
			tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpParam;
			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);*/ 

			// ��������ϳ�
			AdjustVmpParam(tVmpEqp.GetEqpId(), &tVmpParam);
		}

        break;
    }
    default:
        break;
    }

    return;
}

/*====================================================================
    ������      ��ProcMcsMcuVmpPollMsg
    ����        ��Vmp��ͨ����ѯ��Ϣ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    12/05/02    4.7         �־         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuVmpPollMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
	
    STATECHECK;
	
    switch(pcMsg->event)
    {
    case MCS_MCU_STARTVMPPOLL_CMD://��ʼVmp��ͨ����ѯ
		{
			//ֻ�е�ǰδ��ѯ,���ܿ�ʼ�µ���ѯ
			if (POLL_STATE_NONE != m_tVmpPollParam.GetPollState() && !m_tVmpPollParam.GetMtPollParam().IsNull()) 
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuVmpPollMsg] Vmp channel is already in polling, currect poll mt is (%d,%d).\n",
					m_tVmpPollParam.GetMtPollParam().GetMcuId(), m_tVmpPollParam.GetMtPollParam().GetMtId());
				break;
			}

			// ��õ�ǰΨһ�Ļ���ϳ���
			u8 byVmpId = GetTheOnlyVmpIdFromVmpList();
			if (byVmpId == 0)
			{
				// δ�ҵ�Ψһvmp
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuVmpPollMsg] Can not find the only vmp id.\n");
				break;
			}

			//��鵱ǰ����Ļ���ϳ������ͣ�Ŀǰ��֧��8KH/8KI֧�ֵ�ͨ����ѯ 
			u8 byVmpSubType = GetVmpSubType(byVmpId);
			if(byVmpSubType == VMP_8KI || byVmpSubType == VMP_8KH)
			{
			}
			else
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuVmpPollMsg] Vmp.%d is not 8kivmp.\n", byVmpId);
				break;
			}

			TEqp tVmpEqp = g_cMcuVcApp.GetEqp(byVmpId);

			//��鵱ǰ����Ļ���ϳ�״̬ 
			if( g_cMcuVcApp.GetVMPMode(tVmpEqp) == CONF_VMPMODE_NONE )
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuVmpPollMsg] Vmp is not start.\n");
				break;
			}
			
			//�õ�Vmp��ͨ����ѯ����
			m_tVmpPollParam = *(TVmpPollParam *)cServMsg.GetMsgBody();       
			
			// ���Ϊ��ʼ��ѯ�����ö�ʱ��
			m_tVmpPollParam.SetPollState(POLL_STATE_NORMAL);
			m_tVmpPollParam.SpecPollPos(POLLING_POS_START);
			
			SetTimer(MCUVC_VMPPOLLING_CHANGE_TIMER, 10);
		}
        break;

    case MCS_MCU_STOPVMPPOLL_CMD://ֹͣVmp��ͨ����ѯ
		ProcStopVmpPoll();
        break;
		
    case MCS_MCU_PAUSEVMPPOLL_CMD://��ͣVmp��ͨ����ѯ     
		
        m_tVmpPollParam.SetPollState(POLL_STATE_PAUSE);
        cServMsg.SetMsgBody((u8 *)&m_tVmpPollParam, sizeof(m_tVmpPollParam));
        SendMsgToAllMcs(MCU_MCS_VMPPOLLSTATE_NOTIF, cServMsg);
		
        KillTimer(MCUVC_VMPPOLLING_CHANGE_TIMER);			
        break;
		
    case MCS_MCU_RESUMEVMPPOLL_CMD://����Vmp��ͨ����ѯ   
		
        m_tVmpPollParam.SetPollState(POLL_STATE_NORMAL);
        cServMsg.SetMsgBody((u8 *)&m_tVmpPollParam, sizeof(m_tVmpPollParam));
        SendMsgToAllMcs(MCU_MCS_VMPPOLLSTATE_NOTIF, cServMsg);
		
		// ����ѯ��ͣ�����¿�ʼ
		m_tVmpPollParam.SpecPollPos( m_tVmpPollParam.GetCurrentIdx() );
		
        SetTimer(MCUVC_VMPPOLLING_CHANGE_TIMER, 10);
        break;
		
    case MCS_MCU_GETVMPPOLLPARAM_REQ://��ѯVmp��ͨ����ѯ���� 
		{
			TEqp tVmpEqp = g_cMcuVcApp.GetEqp(GetTheOnlyVmpIdFromVmpList());
			if (g_cMcuVcApp.GetVMPMode(tVmpEqp) != CONF_VMPMODE_NONE && 
				POLL_STATE_NONE != m_tVmpPollParam.GetPollState())
			{
				cServMsg.SetMsgBody((u8 *)&m_tVmpPollParam, sizeof(m_tVmpPollParam));
				SendReplyBack(cServMsg, pcMsg->event + 1);
			}
			else //����ϳ�δ����ʱ,�ظ�δ��ѯ״̬,����vmp���ߵȴ�����ʱ���������
			{
				TVmpPollParam tTmpVmpPollParam = m_tVmpPollParam;
				tTmpVmpPollParam.SetPollState(POLL_STATE_NONE);
				tTmpVmpPollParam.SetMediaMode(MODE_NONE);
				cServMsg.SetMsgBody((u8 *)&tTmpVmpPollParam, sizeof(tTmpVmpPollParam));
				SendReplyBack(cServMsg, pcMsg->event + 1);
			}
		}
        break;

    default:
        break;
    }
}

/*====================================================================
    ������      ��ProcPollMsg
    ����        ����ѯ��Ϣ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/13    1.0         ������         ����
    03/11/27    3.0         ������         �޸�
====================================================================*/
void CMcuVcInst::ProcMcsMcuPollMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
    TPollInfo tPollInfo;
	
    STATECHECK;
	
    switch(pcMsg->event)
    {		
    case MCS_MCU_STARTPOLL_CMD://��ʼ��ѯ
		{
			//ֻ�е�ǰδ��ѯ,���ܿ�ʼ�µ���ѯ
			if (CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode()) 
			{
				break;
			}
			
		

			//�����ʼ���������������ܿ�ʼ��ѯ
			if (m_tConf.m_tStatus.IsVACing())
			{
				/// To Notify
				LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "No poll while Vac!\n");
				break;
			}       
			
			//�õ�������ѯ����
			tPollInfo = *(TPollInfo *)cServMsg.GetMsgBody();
			u8 byPollMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TPollInfo)) / sizeof(TMtPollParam);
			TMtPollParam *ptParam = (TMtPollParam*)(cServMsg.GetMsgBody() + sizeof(TPollInfo));
			m_tConfPollParam.InitPollParam( byPollMtNum, ptParam );         
			
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuPollMsg] start poll as mode.%d!\n", tPollInfo.GetMediaMode());
			
			// xsl [8/23/2006] �ж����ǻ����Ƿ�ֹͣ��ѯ
			/*if (m_tConf.GetConfAttrb().IsSatDCastMode())
			{
				//���ش�������������ѯ�ն����в����������ն���ܾ���ѯ
				if (IsOverSatCastChnnlNum(0))
				{
					const TMtPollParam *ptMt;
					for(u8 byIdx = 0; byIdx < m_tConfPollParam.GetPolledMtNum(); byIdx++)
					{
						ptMt = m_tConfPollParam.GetPollMtByIdx(byIdx);
						// ��ɢ�����ǲ�֧�ּ����ģ����ÿ���
						if (m_ptMtTable->GetMtSndBitrate(ptMt->GetMtId()) == 0)
						{
							ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuPollMsg-start] over max upload mt num. nack!\n");  
							NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_DCAST_OVERCHNNLNUM);
							
							memset(&m_tConfPollParam, 0, sizeof(m_tConfPollParam));
							return;
						}
					}
				}            
			}     */
			// miaoqingsong [2011/08/26] ��������ϯΪ�ϣ��¼�MCUʱ������������ϯ��ѯѡ��
			if ( ( tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN ) &&
					(MT_TYPE_SMCU == m_tConf.GetChairman().GetMtType() || MT_TYPE_MMCU == m_tConf.GetChairman().GetMtType())
				)
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuPollMsg-start] SMCU can't select. so nack!\n" );  
				NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_CHAIMANPOLL_NACK);	
				return;
			}
			//zhouyiliang 20120518 ��ǰ¼����ط����ʱ��������������ѯ������Ƶ�򲻴���Ƶ������mcs�з����˱���һ��
			if ( ( tPollInfo.GetMediaMode() == MODE_VIDEO || tPollInfo.GetMediaMode() == MODE_BOTH ) &&
					( m_tConf.m_tStatus.IsPlaying() || m_tConf.m_tStatus.IsPlayReady() )
				)
			{
					NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_CONFPLAYING);
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuPollMsg-start] Cannot poll because it's playing!\n" );
					return;

			}
			// guzh [6/22/2007] ���Ϊ��ʼ��ѯ�����ö�ʱ��
			tPollInfo.SetPollState(POLL_STATE_NORMAL);
			m_tConfPollParam.SpecPollPos(POLLING_POS_START);        
			m_tConf.m_tStatus.SetPollInfo(tPollInfo);
			
			u8 byConfPollMode = CONF_POLLMODE_NONE;
			switch (tPollInfo.GetMediaMode())
			{
			case MODE_VIDEO:
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_VIDEO);
				break;
			case MODE_BOTH:
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_SPEAKER);
				break;
			case MODE_VIDEO_SPY:
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_VIDEO_SPY);
				break;
			case MODE_BOTH_SPY:
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_SPEAKER_SPY);
				break;
			case MODE_VIDEO_BOTH:
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_VIDEO_BOTH);
				break;
			case MODE_BOTH_BOTH:
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_SPEAKER_BOTH);
				break;
			case MODE_VIDEO_CHAIRMAN:                                         //20110411_miaoqs ��ϯ��ѯѡ���������
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_VIDEO_CHAIRMAN);
				break;
			case MODE_BOTH_CHAIRMAN:                                          
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_BOTH_CHAIRMAN);
				break;
			default:
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuPollMsg] unexpected poll mode.%d rcved, ignore it!\n", tPollInfo.GetMediaMode());
				break;
			}

			// miaoqingsong [2011/09/27] �����ϴ���ѯ֮ǰ�������ϴ�ͨ��
			if ( (CONF_POLLMODE_VIDEO_SPY == m_tConf.m_tStatus.GetPollMode()) || 
				 (CONF_POLLMODE_SPEAKER_SPY == m_tConf.m_tStatus.GetPollMode()) 
				 )
			{
				// �ͷŻش�ͨ������
				TMtStatus tMtStatus;
				m_ptMtTable->GetMtStatus(m_tCascadeMMCU.GetMtId(), &tMtStatus);
				FreeRecvSpy( tMtStatus.GetVideoMt(), MODE_BOTH );

				// ��ش�ͨ������
				StopSpyMtCascaseSwitch();

				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuPollMsg] First FreeRecvSpy and topSpyMtCascaseSwitch before start spy's poll!\n");
			}

			// miaoqingsong [2011/08/10] ������ѯ��ʱ��֮ǰ����ѯģʽΪ��ϯ��ѯģʽ����ͣ��ϯԭ�е�ѡ��
			if ( (CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()) ||
				(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode())
				)
			{
				TMtStatus tMtStatus;
				m_ptMtTable->GetMtStatus(m_tConf.GetChairman().GetMtId(), &tMtStatus);
				
				if (!tMtStatus.GetSelectMt(MODE_VIDEO).IsNull())
				{
					StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO, FALSE, FALSE);
				} 
				
				if(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode() && !tMtStatus.GetSelectMt(MODE_AUDIO).IsNull())
				{
					StopSelectSrc(m_tConf.GetChairman(), MODE_AUDIO, FALSE, FALSE);
				}

			
			}

			if (CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode())
			{
				ConfModeChange();
				
				SetTimer(MCUVC_POLLING_CHANGE_TIMER, 10);
			}
			break;
		}
		
    case MCS_MCU_STOPPOLL_CMD://ֹͣ��ѯ
		ProcStopConfPoll();
        break;
		
    case MCS_MCU_PAUSEPOLL_CMD://��ͣ��ѯ     
		
        m_tConf.m_tStatus.SetPollState(POLL_STATE_PAUSE);
        cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
        SendMsgToAllMcs(MCU_MCS_POLLSTATE_NOTIF, cServMsg);
		
        ConfStatusChange();
		
        KillTimer(MCUVC_POLLING_CHANGE_TIMER);			
        break;
		
    case MCS_MCU_RESUMEPOLL_CMD://������ѯ   
		
        m_tConf.m_tStatus.SetPollState(POLL_STATE_NORMAL);
        cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
        SendMsgToAllMcs(MCU_MCS_POLLSTATE_NOTIF, cServMsg);
		
        ConfStatusChange();
		
		// ����ѯ��ͣ�����¿�ʼ, zgc, 20070622
		m_tConfPollParam.SpecPollPos( m_tConfPollParam.GetCurrentIdx() );
		
        SetTimer(MCUVC_POLLING_CHANGE_TIMER, 10);
        break;
		
    case MCS_MCU_GETPOLLPARAM_REQ://��ѯ��ѯ���� 
		{
			/*if(m_tConfPollParam.GetLastPolledMt().IsNull())
			{
				//��ǰ��ѯû���������ߵ�ǰ��ѯ�����ˣ�������ѯ�Ĳ���û���óɹ�����ش���������¼��������㣬��������lastpoll��
				SendReplyBack(cServMsg, pcMsg->event + 2);	
			}
			else*/
			//zjj20130709 û����ѯ�ն�ҲҪ��ack,��һ�¼��ն˶���û��ͨ����������ƵԴ�ģ��ǻ�nack��������޷���ʾ��ͣ��ֹͣ��(�ر����������ʱ)
			{
				cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
				cServMsg.CatMsgBody((u8 *)m_tConfPollParam.GetPollMtByIdx(0),
				m_tConfPollParam.GetPolledMtNum()*sizeof(TMtPollParam ));
				SendReplyBack(cServMsg, pcMsg->event + 1);	
			}
					
			break;
		}

		
		// zbq [03/13/2007] ������ѯ�б�
    case MCS_MCU_CHANGEPOLLPARAM_CMD:
        {
            //�õ��µĻ�����ѯ����
            TPollInfo tTmpPollInfo;
            tTmpPollInfo = *(TPollInfo*)cServMsg.GetMsgBody();
			
            TConfPollParam tTmpPollParam;
            u8 byPollMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TPollInfo)) / sizeof(TMtPollParam);
            TMtPollParam *ptParam = (TMtPollParam*)(cServMsg.GetMsgBody() + sizeof(TPollInfo));
            tTmpPollParam.SetPollList(byPollMtNum, ptParam);
            
            // �жϵ�ǰ��ѯ�ն��Ƿ������б���
            TMtPollParam tPollingMt = m_tConf.m_tStatus.GetMtPollParam();
            u8 byIdx = POLLING_POS_INVALID;
			//�����ж��Ƿ�Ϊ��, zgc, 2007-06-21
            if ( !tPollingMt.IsNull() && !tTmpPollParam.IsExistMt(tPollingMt, byIdx) )
            {
                // notify NACK
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_DELCURPOLLMT_FAIL);
                break;
            }
			
            // �������б�
            m_tConfPollParam.SetPollList(byPollMtNum, ptParam);
			
			// ���õ�ǰ��ѯ�ն�������ѯ�б��е�λ��, zgc, 2007-05-24
			m_tConfPollParam.SetCurrentIdx( byIdx );
			
            //ˢ������MCS��ѯ�ն��б�
            PollParamChangeNotify();
        }
        break;
		//ָ����ѯλ��
	case MCS_MCU_SPECPOLLPOS_REQ:
		{
			if( POLL_STATE_NONE == m_tConf.m_tStatus.GetPollInfo()->GetPollState() )
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcsMcuPollMsg] <MCS_MCU_SPECPOLLPOS_REQ> Conf not in polling!\n");
				cServMsg.SetErrorCode( ERR_MCU_SPECPOLLPOS_CONFNOTPOLL );
				SendReplyBack(cServMsg, pcMsg->event + 2);
				break;
			}
			TMt tPollMt = *(TMt *)cServMsg.GetMsgBody();
            u8 byMtIdx;
            if ( !m_tConfPollParam.IsExistMt(tPollMt, byMtIdx) )
            {
                cServMsg.SetErrorCode( ERR_MCU_SPECPOLLPOS_MTNOTINCONF );
                SendReplyBack(cServMsg, pcMsg->event + 2);
                break;
            }
			
            SendReplyBack(cServMsg, pcMsg->event + 1);
			
            // ָ����ѯλ��            
            if (POLL_STATE_PAUSE == m_tConf.m_tStatus.GetPollState())
            {
                TPollInfo tCurPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
                TMtPollParam tCurMtParam = *(m_tConfPollParam.GetPollMtByIdx(byMtIdx));
                tCurPollInfo.SetMtPollParam( tCurMtParam );
                m_tConf.m_tStatus.SetPollInfo(tCurPollInfo);
                m_tConfPollParam.SetCurrentIdx(byMtIdx);
				
                ConfStatusChange();
            }
            else //POLL_STATE_NORMAL
            {
                m_tConfPollParam.SpecPollPos(byMtIdx);
				SetTimer( MCUVC_POLLING_CHANGE_TIMER, 10 );
			}
		}
		break;
    default:
        break;
    }
}

/*====================================================================
    ������      ��PollParamChangeNotify
    ����        ����ѯ�ն��б�ı� ֪ͨ���еĻ�� ˢ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/03/21    4.0         �ű���         ����
====================================================================*/
void CMcuVcInst::PollParamChangeNotify( void )
{
    CServMsg cServMsg;
    cServMsg.SetEventId( MCU_MCS_POLLPARAMCHANGE_NTF );
    cServMsg.SetMsgBody( (u8*)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo) );
    cServMsg.CatMsgBody( (u8*)m_tConfPollParam.GetPollMtByIdx(0), m_tConfPollParam.GetPolledMtNum() * sizeof(TMtPollParam) );
    SendMsgToAllMcs( MCU_MCS_POLLPARAMCHANGE_NTF, cServMsg );

    return;
}

/*====================================================================
    ������      ��ProcStopConfPoll
    ����        ��ֹͣ������ѯ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/06/22    4.0         ����         ����
====================================================================*/
void CMcuVcInst::ProcStopConfPoll(BOOL32 bNeedAdjustVmp)
{
    u8 byPrvPollMode = m_tConf.m_tStatus.GetPollMode();
    u8 byPollMeida = m_tConf.m_tStatus.GetPollMedia();

    KillTimer(MCUVC_POLLING_CHANGE_TIMER);       
    
    //ȡ����ѯ����ȡ����ѯ��������Ӧ����״̬
    //CancelOneVmpFollowSwitch( VMP_MEMBERTYPE_POLL, TRUE );
    if (bNeedAdjustVmp && GetVmpCountInVmpList() > 0)
    {
        ChangeVmpChannelParam(NULL, VMP_MEMBERTYPE_POLL);
    }
    
    if (CONF_VMPTWMODE_NONE != m_tConf.m_tStatus.GetVmpTwMode())
    {
        ChangeVmpTwChannelParam(NULL, VMPTW_MEMBERTYPE_POLL);
    }
	
    TMt tSpyMt;
    tSpyMt.SetNull();
	tSpyMt = m_tConfPollParam.GetLastPolledMt();

	TMt tLocalPollMt = GetLocalMtFromOtherMcuMt( tSpyMt );

	// miaoqingsong [20110602] ���������жϣ�����ϳɹ����У�ֹͣ��ϯ��ѯ����ϯ�ն�Ӧ�ý��ջ���ϳ���Ƶ����
	// Bug00055059 ����ϳɹ����У�ֹͣ��ϯ��ѯ����ϯ�ն���Ȼѡ�����һ���նˣ�Ӧ�ý��ջ���ϳɣ�
    if ( !g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) ||
		( MODE_VIDEO_CHAIRMAN == byPollMeida || MODE_BOTH_CHAIRMAN == byPollMeida ))		
    {
		CRecvSpy tSpyResource;
		u16 wSpyStartPort = SPY_CHANNL_NULL;
		if( m_cSMcuSpyMana.GetRecvSpy( tSpyMt, tSpyResource ) )
		{
			wSpyStartPort = tSpyResource.m_tSpyAddr.GetPort();
		}
		
		TMt tMt;
        tMt.SetNull();

        switch (byPollMeida)
        {
        case MODE_VIDEO:
            
            //�𱾵���Ƶ�㲥
            //ChangeVidBrdSrc(NULL);					

            //�ָ�������Ƶ�㲥
            if(HasJoinedSpeaker() && 
				( !IsMultiCastMt(GetLocalSpeaker().GetMtId()) || IsMultiCastMt(tLocalPollMt.GetMtId())
					|| GetCurSatMtNum() > 0)
				)
            {
                tMt = m_tConf.GetSpeaker();
                ChangeVidBrdSrc(&tMt);
            }
            else
            {		
                ChangeVidBrdSrc(NULL);
            }

			if( SPY_CHANNL_NULL != wSpyStartPort )
			{
				FreeRecvSpy( tSpyMt, MODE_VIDEO );	
			}
            break;
            
        case MODE_BOTH:
            
            //�𱾵ط���
            tMt = m_tConf.GetSpeaker();
			if (!IsMultiCastMt(GetLocalSpeaker().GetMtId()) || 
				IsMultiCastMt(tLocalPollMt.GetMtId()) || GetCurSatMtNum() > 0)
			{
				ChangeVidBrdSrc(&tMt);
			}

            break;
            
        case MODE_VIDEO_SPY:
        case MODE_BOTH_SPY:
			{
				// ���ϴ�Դ(������ֹͣ�ϴ�ʱ��ռ�ûش�����)
				u8 byFreeMode = MODE_NONE;
				if( MODE_VIDEO_SPY == byPollMeida )
				{
					byFreeMode = MODE_VIDEO;
				}
				else
				{
					byFreeMode = MODE_BOTH;
				}

				if( SPY_CHANNL_NULL != wSpyStartPort )
				{
					FreeRecvSpy( tSpyMt, byFreeMode );
				}
				
				StopSpyMtCascaseSwitch();
			}
            break;
            
        case MODE_VIDEO_BOTH:
            {
				//�𱾵���Ƶ�㲥
				//ChangeVidBrdSrc(NULL);
				
				//�ָ�������Ƶ�㲥
				if(HasJoinedSpeaker() && (!IsMultiCastMt(GetLocalSpeaker().GetMtId()) || 
						IsMultiCastMt(tLocalPollMt.GetMtId()) || GetCurSatMtNum() > 0))
				{
					tMt = m_tConf.GetSpeaker();
					ChangeVidBrdSrc(&tMt);
				}
				else
				{
					ChangeVidBrdSrc(NULL);
				}
				
				// ���ϴ�Դ�������ͷŻش�����
				u8 byFreeMode = MODE_NONE;
				if( MODE_VIDEO_SPY == byPollMeida )
				{
					byFreeMode = MODE_VIDEO;
				}
				else
				{
					byFreeMode = MODE_BOTH;
				}
				
				if( SPY_CHANNL_NULL != wSpyStartPort )
				{
					FreeRecvSpy( tSpyMt, byFreeMode );
				}
				StopSpyMtCascaseSwitch();
			}
            break;
            
        case MODE_BOTH_BOTH:
            {
				//�𱾵ط���
				tMt = m_tConf.GetSpeaker();
				if (!IsMultiCastMt(GetLocalSpeaker().GetMtId()) || 
						IsMultiCastMt(tLocalPollMt.GetMtId()) || GetCurSatMtNum() > 0)
				{
					ChangeVidBrdSrc(&tMt);
				}

				// ���ϴ�Դ,��ʱ����Ҫ�ͷŻش���������Ƶ�Ļ�����ѯֹͣʱ����ȡ�������ˣ�
				StopSpyMtCascaseSwitch();
			}
            break;

		case MODE_VIDEO_CHAIRMAN:

			//ֹͣ��ϯѡ��
			StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO);           // 20110414_miaoqs ��ϯ��ѯѡ���������
			break;

		case MODE_BOTH_CHAIRMAN:

			//ֹͣ��ϯѡ��
            StopSelectSrc(m_tConf.GetChairman(), MODE_BOTH);
			break;
            
        default:
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuPollMsg] unexpected poll mode.%d rcved, ignore it!\n", byPollMeida);
            break;
        }
    }
	else
	{
		//����Ǵ���Ƶ��ѯ�Ͳ����ͷ���Ƶ����Ϊ��ʱ��ѯ�����ն��Ƿ����ˣ���ȡ��
		if( m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_POLL) &&
			MODE_BOTH != byPollMeida
			)
		{
			FreeRecvSpy( tSpyMt, MODE_VIDEO );
		}
		else
		{
			if( MODE_VIDEO == byPollMeida )
			{
				FreeRecvSpy( tSpyMt, MODE_VIDEO );
			}
		}
		
	}

	// miaoqingsong [06/29/2011] ͣ��ѯ����ѯ״̬�ͻ���״̬
	m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_NONE);
    m_tConf.m_tStatus.SetPollState(POLL_STATE_NONE);
	m_tConf.m_tStatus.SetPollMedia(MODE_NONE);

	ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcStopConfPoll]stop poll,clear pollstate in confstatus!\n");
	
	//zhouyiliang 20110518 ֹͣ��ѯ��ʱ�������ǰ��ѯ�նˡ�
	TPollInfo* ptPollInfo = m_tConf.m_tStatus.GetPollInfo();
	if (NULL == ptPollInfo)
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcStopConfPoll]GetPollInfo pointer is null\n");
		return;
	}
	TMtPollParam tMtPollParam = ptPollInfo->GetMtPollParam();
	TMt tNullMt;
	tNullMt.SetNull();
	tMtPollParam.SetTMt(tNullMt);
	ptPollInfo->SetMtPollParam(tMtPollParam);
	m_tConf.m_tStatus.SetPollInfo(*ptPollInfo);

	CServMsg cServMsg;
    cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
    SendMsgToAllMcs(MCU_MCS_POLLSTATE_NOTIF, cServMsg);
        
    //stop TvWall
    TPeriEqpStatus tTWStatus, tHduStatus;
    u8 byChnlIdx;
    u8 byEqpId;
    
    for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
    {
        if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
        {
            for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
            {
                if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus) &&
                    TW_MEMBERTYPE_POLL == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType &&
                    m_byConfIdx == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx())
                {
                    ChangeTvWallSwitch(NULL, byEqpId, byChnlIdx, TW_MEMBERTYPE_POLL, TW_STATE_STOP);
                }
            }
        }
    }
    // stop HDU   
    for (byEqpId = HDUID_MIN; byEqpId <= HDUID_MAX; byEqpId++)
    {
		if(IsValidHduEqp(g_cMcuVcApp.GetEqp(byEqpId)))
        {
			u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byEqpId));
			if (0 == byHduChnNum)
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[ProcStopConfPoll] GetHduChnNumAcd2Eqp failed!\n");
				continue;
			}

            for (byChnlIdx = 0; byChnlIdx < byHduChnNum; byChnlIdx++)
            {
                if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tHduStatus) &&
                    TW_MEMBERTYPE_POLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType &&
                    m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx())
                {
					// [2013/03/11 chenbing] HDU�໭��Ŀǰ��֧����ѯ����,��ͨ����0
                    ChangeHduSwitch(NULL, byEqpId, byChnlIdx, 0, TW_MEMBERTYPE_POLL, TW_STATE_STOP, FALSE);
                }
            }
        }
    }

	
    ConfStatusChange();

    return;
}

/*====================================================================
    ������      ��ProcPollingChangeTimerMsg
    ����        ����ʱ��ѯ��Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/13    1.0         ������         ����
    03/11/28    3.0         ������         �޸�
====================================================================*/
void CMcuVcInst::ProcPollingChangeTimerMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
    TMtPollParam * ptCurPollMt;
	
    switch(CurState())
    {
    case STATE_ONGOING:
    {
        if (CONF_POLLMODE_NONE == m_tConf.m_tStatus.GetPollMode() || 
			m_tConf.m_tStatus.GetPollState() == POLL_STATE_PAUSE ) 
        {
            break;
        }

		if ( 0 == m_tConfPollParam.GetPolledMtNum() )
		{
            ProcStopConfPoll();
			break;
		}		
        
		TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
		
        // xliang [3/31/2009] ��¼�ϴ��ֵ���MT
		TMt tLastPolledMt = tPollInfo.GetMtPollParam().GetTMt(); 
        
        //��¼��ѯ�Ƿ��д�������, zgc, 2007-06-20
		BOOL32 bIsPollNumNoLimit = ( 0 == tPollInfo.GetPollNum() ) ? TRUE : FALSE; 
		u8 byPollIdx = m_tConfPollParam.GetCurrentIdx();

		// 20110413_miaoqingsong �õ���һ�����ϱ���ѯ�������ն�
		ptCurPollMt = GetNextMtPolled(byPollIdx, tPollInfo); 
		
		if (ptCurPollMt == NULL)
		{
			ProcStopConfPoll();
	
			//��һ�ο������޿���ѯ�նˣ��ϱ���ʾ[8/5/2013 chendaiwei]
			if(tLastPolledMt.IsNull())
			{
				NotifyMcsAlarmInfo( 0, ERR_MCU_POLLING_NOMTORVIDSRC );
			}

			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Err ProcPollingChangeTimerMsg Get CurPollMt is Null, LastMt:(%d, %d). stop poll\n", tLastPolledMt.GetMcuId(), tLastPolledMt.GetMtId());
			return;
		}

		m_tConf.m_tStatus.SetPollInfo(tPollInfo);
		m_tConfPollParam.SetCurrentIdx(byPollIdx);
	
		// xsl [8/23/2006] �ж����ǻ����Ƿ�ֹͣ��ѯ
		/*if (m_tConf.GetConfAttrb().IsSatDCastMode())
		{
			BOOL32 bPollWitchAudio = (CONF_POLLMODE_SPEAKER == m_tConf.m_tStatus.GetPollMode());
			if (IsMtSendingVidToOthers(tLastPolledMt, bPollWitchAudio, FALSE, 0) && //��һ���ն˻���������ʵ�巢������
				m_ptMtTable->GetMtSndBitrate(ptCurPollMt->GetMtId()) == 0 && //��һ���ն�û���ڷ�������
				IsOverSatCastChnnlNum(0))
			{
                ProcStopConfPoll();
			    break;
			}
		}*/

		
		//lukunpeng 2010/06/30 ��Timer�ŵ�ǰ����������ֹ��ش�ʧ�ܺ󣬲��ܼ�������
		SetTimer(MCUVC_POLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());
		
		// zbq [03/09/2007] ֪ͨ��һ����������ѯ�����ն�
		//NotifyMtToBePolledNext();

		if (NULL != ptCurPollMt)
		{
			   
			m_tConf.m_tStatus.SetMtPollParam( *ptCurPollMt );
			//zjj20121124 ���ں�����֪ͨ���л��  
			//cServMsg.SetMsgBody( (u8*)&tPollInfo, sizeof(TPollInfo) );
			//SendMsgToAllMcs( MCU_MCS_POLLSTATE_NOTIF, cServMsg );
		}
		
		//�ɴ˴�ͳһ����������ѯ�Ļش�
		TMt tSpySourceMt = ptCurPollMt->GetTMt();
		if( !tSpySourceMt.IsLocal() &&
			IsLocalAndSMcuSupMultSpy(tSpySourceMt.GetMcuId()) 
			)
		{
			TPreSetInReq tSpySrcInitInfo;
			tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt( tSpySourceMt );

			// [20111129_miaoqingsong] Ϊ����֧�ֶ�ش��ĵͰ汾��������ش���ѯģʽת�������ϼ�����
			if ( (MODE_VIDEO == tPollInfo.GetMediaMode()) || 
				 (MODE_VIDEO_SPY == tPollInfo.GetMediaMode()) || 
				 (MODE_VIDEO_BOTH == tPollInfo.GetMediaMode()) || 
				 (MODE_VIDEO_CHAIRMAN == tPollInfo.GetMediaMode()) 
				)
			{
				tSpySrcInitInfo.m_bySpyMode = MODE_VIDEO;
			} 
			else
			{
				tSpySrcInitInfo.m_bySpyMode = MODE_BOTH;
			}
			//tSpySrcInitInfo.m_bySpyMode = tPollInfo.GetMediaMode();
			tSpySrcInitInfo.SetEvId(MCUVC_POLLING_CHANGE_TIMER);
			tSpySrcInitInfo.m_tSpyInfo.m_tSpyPollInfo.m_wKeepTime = ptCurPollMt->GetKeepTime();
			tSpySrcInitInfo.m_tSpyInfo.m_tSpyPollInfo.m_byPollingPos = m_tConfPollParam.GetCurrentIdx();
			
			// [pengjie 2010/9/13] ��Ŀ�Ķ�����
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSpySourceMt.GetMcuId()) );	
			//zjl20101116 �����ǰ�ն��ѻش���������Ҫ���ѻش�Ŀ��������ȡС
			if(!GetMinSpyDstCapSet(tSpySourceMt, tSimCapSet))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcPollingChangeTimerMsg] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
					tSpySourceMt.GetMcuId(), tSpySourceMt.GetMtId() );
				return;
			}
			
			tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		    // End

			TMt tLastMt = m_tConfPollParam.GetLastPolledMt();
			if( !tLastMt.IsNull() )
			{
				if( m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO ||
					m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_SPY ||
					m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_BOTH ||
					m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_CHAIRMAN
					)
				{
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_VIDEO;
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
				}
				else
				{
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_BOTH;
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
					//zhouyiliang 20120824 ����Ƶ�ı�����ѯ�ʹ���Ƶ�ı����ϴ���ѯ����ǰ�ķ������ǲ���lastmt�����ǵĻ�������(�����ֵĹ�����ȡ���˷�����)
					//fixme:CONF_POLLMODE_SPEAKER_SPYģʽû����
					if ( CONF_POLLMODE_SPEAKER == m_tConf.m_tStatus.GetPollMode() || 
						CONF_POLLMODE_SPEAKER_BOTH == m_tConf.m_tStatus.GetPollMode())
					{
						if ( !(m_tConf.GetSpeaker() == tLastMt) )
						{
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum--;
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum--;

						}
					}
										
				}
				
				tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tLastMt;
				tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
				//tSpySrcInitInfo.m_tReleaseMtInfo.m_tCanReleaseMtInfo = BuildMultiCascadeMtInfo( tSpySrcInitInfo.m_tReleaseMtInfo.m_tCanReleaseMtInfo.m_tMt );
				
				// vmp ֻ�Ե���vmp����ͨ����ģʽ���п��ͷż���ͳ��
				u8 byVmpCount = GetVmpCountInVmpList();
				if( /*g_cMcuVcApp.GetVMPMode(m_tVmpEqp) != CONF_VMPMODE_NONE*/byVmpCount > 0 && 
					(MODE_VIDEO == tPollInfo.GetMediaMode() || MODE_BOTH == tPollInfo.GetMediaMode()
					|| MODE_VIDEO_BOTH == tPollInfo.GetMediaMode() || MODE_BOTH_BOTH == tPollInfo.GetMediaMode())
					)
				{
					u8 bySpeakerPollNum = 0;
					bySpeakerPollNum += GetVmpChnnlNumBySpecMemberType( VMP_MEMBERTYPE_POLL, &tLastMt);
					
					// 2011-11-8 add by pgf: ���˴���Ƶ��ģʽ,��MODE_BOTH_SPY���������Ѿ����˵���
					if ( tPollInfo.GetMediaMode() == MODE_BOTH ||
						tPollInfo.GetMediaMode() == MODE_BOTH_BOTH )
					{
						bySpeakerPollNum += GetVmpChnnlNumBySpecMemberType(VMP_MEMBERTYPE_SPEAKER, &tLastMt);
					}
					
					if ( bySpeakerPollNum > 0)
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "VMP has %d chnnl of VMP_MEMBERTYPE_SPEAKER and VMP_MEMBERTYPE_POLL\n", bySpeakerPollNum);
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum += bySpeakerPollNum;
					}
// 					TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
// 					u8 byChnIdx = tVmpParam.GetChlOfMtInMember(m_tConfPollParam.GetLastPolledMt());
// 					if( byChnIdx != MAXNUM_VMP_MEMBER )
// 					{
// 						if( VMP_MEMBERTYPE_MCSSPEC == tVmpParam.GetVmpMember(byChnIdx)->GetMemberType()  
// 							|| VMP_MEMBERTYPE_VAC == tVmpParam.GetVmpMember(byChnIdx)->GetMemberType() )
// 						{
// 							
// 						}
// 						else if( VMP_MEMBERTYPE_POLL == tVmpParam.GetVmpMember(byChnIdx)->GetMemberType()  )
// 						{
// 							++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
// 						}
// 						else
// 						{
// 							if( ( tPollInfo.GetMediaMode() == MODE_BOTH || 
// 								tPollInfo.GetMediaMode() == MODE_BOTH_BOTH	)
// 								&& VMP_MEMBERTYPE_SPEAKER == tVmpParam.GetVmpMember(byChnIdx)->GetMemberType() )
// 							{			
// 								//������������ڷ����˸����Զ������˻���ϳ�,���ͷ���ƵĿ����Ҫ��1
// 								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
// 							}
// 						}
// 					}
// 					else
// 					{
// 						//exception
// 						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcPollingChangeTimerMsg] GetChlOfMtInMember error for (%d, %d)\n",
// 							m_tConfPollParam.GetLastPolledMt().GetMcuId(),m_tConfPollParam.GetLastPolledMt().GetMtId() );
// 						
// 					}
				}
				

				TPeriEqpStatus tStatus;
				u8 byChnlIdx;
				u8 byEqpId;
				u8 byTvWallMode = 0;
				//������������ڷ����˸����Զ������˱������ǽ,���ͷ�����ƵĿ����Ҫ��1
				for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
				{
					if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
					{
						if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus))
						{						
							for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
							{							
								if (tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx() == m_byConfIdx &&
									tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetMcuId() == m_tConfPollParam.GetLastPolledMt().GetMcuId() &&
									tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetMtId() == m_tConfPollParam.GetLastPolledMt().GetMtId()
									)
								{
									if( TW_MEMBERTYPE_SPEAKER == tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType &&
										( tPollInfo.GetMediaMode() == MODE_BOTH || 
												tPollInfo.GetMediaMode() == MODE_BOTH_BOTH )
										)
									{
										++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
										++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
									}

									if( TW_MEMBERTYPE_POLL == tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType) 
									{
										byTvWallMode = g_cMcuVcApp.GetTvWallOutputMode(byEqpId);
										if (TW_OUTPUTMODE_AUDIO == byTvWallMode)
										{
											++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
										}
										else if (TW_OUTPUTMODE_VIDEO == byTvWallMode)
										{
											++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
										}
										else if (TW_OUTPUTMODE_BOTH == byTvWallMode)
										{
											++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
											++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
										}
									}	
								}								
							}
						}
					}
				}

				//�����˸���ʱ��ͬ������HDU�е�ͼ��
				u8 byHduChnNum = 0;
				//������������ڷ����˸����Զ������˱������ǽ,���ͷ�����ƵĿ����Ҫ��1
				for (byEqpId = HDUID_MIN; byEqpId <= HDUID_MAX; byEqpId++)
				{
					if(IsValidHduEqp(g_cMcuVcApp.GetEqp(byEqpId)))
					{
						byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byEqpId));
						if (0 == byHduChnNum)
						{						
							continue;
						}

						if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus))
						{						
							for (byChnlIdx = 0; byChnlIdx < byHduChnNum; byChnlIdx++)
							{							
								if ( m_byConfIdx == tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx() &&
									tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetMcuId() == m_tConfPollParam.GetLastPolledMt().GetMcuId() &&
									tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetMtId() == m_tConfPollParam.GetLastPolledMt().GetMtId()
									)
								{
									if( TW_MEMBERTYPE_SPEAKER == tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType &&
										( tPollInfo.GetMediaMode() == MODE_BOTH || 
												tPollInfo.GetMediaMode() == MODE_BOTH_BOTH )
										)
									{
										++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
										++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
									}
									if( TW_MEMBERTYPE_POLL == tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType )
									{
										if( m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO ||
											m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_SPY ||
											m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_BOTH 
											)
										{
											++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
										}
										else
										{
											++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
											++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
										}
									}
								}
							}
						}
					}
				}				
			}

			

			if ( !OnMMcuPreSetIn( tSpySrcInitInfo ) && 
				NULL != ptCurPollMt )
			{
				cServMsg.SetMsgBody( (u8*)&tPollInfo, sizeof(TPollInfo) );
				SendMsgToAllMcs( MCU_MCS_POLLSTATE_NOTIF, cServMsg );
			}
			

			return;
		}
			// End Modify


		// zbq [03/09/2007] ֪ͨ��һ����������ѯ�����ն�
		NotifyMtToBePolledNext();		
        
		//TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
		u8 byVmpCount = GetVmpCountInVmpList();
		if (NULL != ptCurPollMt)
		{
			cServMsg.SetMsgBody( (u8*)&tPollInfo, sizeof(TPollInfo) );
			SendMsgToAllMcs( MCU_MCS_POLLSTATE_NOTIF, cServMsg );

			// ��Ӵ˱�ǣ���ʾҪ��Ҫ����������һ��VMP�Ĳ�������
			BOOL32 bVmpNeedChgBySpeaker = TRUE;
// 			BOOL32 bNoneKeda = FALSE;
// 			if (!ptCurPollMt->IsNull())
// 			{
// 				bNoneKeda = (!IsKedaMt(*ptCurPollMt, TRUE)) || (!IsKedaMt(*ptCurPollMt, FALSE));
// 			}

			//֪ͨ���л��
			TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);
			//������渴�ϵ���Ӧ��ѯ����Ľ���
			// xliang [3/31/2009] �����߼�
			if (byVmpCount > 0)
			{
				u8 byVmpPollCount = GetVmpChnnlNumBySpecMemberType( VMP_MEMBERTYPE_POLL);//��ѯ����ͨ����
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
				
				// [11/19/2009 xliang] ������ѯ+VMP�����¼���Ա��������������ѯ�����¼���Ա,������ѯ֮��
				/*
				if( tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
					&& tVmpParam.HasUnLocalMemberInVmp()
					&& !ptCurPollMt->IsLocal()
					)
				{
					//FIXME
					ConfLog(FALSE,"should not poll mt(%d,%d)!\n", ptCurPollMt->GetMcuId(), ptCurPollMt->GetMtId());
					SetTimer(MCUVC_POLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());
					// zbq [03/09/2007] ֪ͨ��һ����������ѯ�����ն�
					NoifyMtToBePolledNext();
					break;
				}
				*/

				// xliang [4/2/2009] ����Ƶ�Ļ�����ѯ,VMP��ѯ���棬VMP�����˸���3�߲��ܹ���
				// MPU2֧����ѯ����ͷ����˸���ͬʱ����.
				// �����˸���ͨ��ȡ�����ϱ�MCS������ʾ
				// ����Ƶ���ϴ�+������ѯ,ͬ����VMP��ѯ���棬VMP�����˸���3�߲��ܹ���
				/*u8 byVmpSubType = GetVmpSubType(m_tVmpEqp.GetEqpId());
				if( (tPollInfo.GetMediaMode() == MODE_BOTH || tPollInfo.GetMediaMode() == MODE_BOTH_BOTH)
					&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) //����Ƶ�Ļ�����ѯ=�����䷢����
					&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)  
					&& !IsAllowVmpMemRepeated(m_tVmpEqp.GetEqpId())  //  [2/17/2012 pengguofeng]������8000A��8000H��ͨ�������ļ�����
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

				// [miaoqingsong 20111205] �����ж���������ϯ��ѯ/�ϴ���ѯ����MT������VMP��Ƶ��ѯ����ͨ��
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
					//u8 byMtVmpChnl = tVmpParam.GetChlOfMtInMember(*(TMt*)ptCurPollMt /*tLocalMt*/);
					/*if( (!IsAllowVmpMemRepeated(m_tVmpEqp.GetEqpId())) 
						&& tVmpParam.IsMtInMember(*(TMt*)ptCurPollMt)
						)
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcPollingChangeTimerMsg] Mt.(%u,%u) has already in certain VMP channel!\n",
							ptCurPollMt->GetMcuId(), ptCurPollMt->GetMtId() );
					}
					else
					{*/
						/*Ŀǰͳһ����ѯ����ʱ�ͷ�vmp��ѯ����ͨ����ش���Դ
						//��ѯ��������ڶ��vmpͨ����
						for (u8 byChlNum=0; byChlNum<tVmpParam.GetMaxMemberNum(); byChlNum++)
						{
							TVMPMember tOldMember = *tVmpParam.GetVmpMember(byChlNum);
							if (!tOldMember.IsNull() && !tOldMember.IsLocal() && !(tOldMember == *ptCurPollMt))
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
									// ����Ƶ�ı�����ѯ,����Ƶ�Ļش���ѯ+������ѯ
									if (tPollInfo.GetMediaMode() == MODE_BOTH ||
										tPollInfo.GetMediaMode() == MODE_BOTH_BOTH)
									{
										/* �ǿƴ��ն�,������1·��ѯ����ͨ��
										if (bNoneKeda && byChlNum != tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_SPEAKER))
										{
											ClearOneVmpMember(byChlNum, tVmpParam);
											continue;
										}*
										FreeRecvSpy( tOldMember, MODE_VIDEO );
									}
								}
							}
						}*/
						//u8 byVmpMemType = VMP_MEMBERTYPE_POLL;
						/*if( tPollInfo.GetMediaMode() == MODE_BOTH )	// xliang [3/31/2009] ����Ƶ�Ļ�����ѯ--->�����˸���ͨ��
						{
							byVmpMemType = VMP_MEMBERTYPE_SPEAKER;
						}*/
						//ChangeVmpChannelParam(ptCurPollMt/*&tLocalMt*/, byVmpMemType,&tLastPolledMt);
						/*bVmpNeedChgBySpeaker = FALSE;
					}*/
				}
			}

			if ( (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE) && 
				 !(tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN) && 
				 !(tPollInfo.GetMediaMode() == MODE_VIDEO_SPY || tPollInfo.GetMediaMode() == MODE_BOTH_SPY) 
				 )
			{
				ChangeVmpTwChannelParam(ptCurPollMt, VMPTW_MEMBERTYPE_POLL);
			}

			
			//  [12/10/2009 pengjie] Modify ������ش�֧�� �����¼��ն���֧�ֶ�ش�����presetin��Ϣ

			//lukunpeng 2010/06/07 �ɴ˴�ͳһ�ͷ��Ϸ����˵Ļش�
			/*
			TMt  tOldSpeaker = m_tConf.GetSpeaker();
			if(!tOldSpeaker.IsNull()
				&& !tOldSpeaker.IsLocal()
				&& IsLocalAndSMcuSupMultSpy(tOldSpeaker.GetMcuId())
				&& (tPollInfo.GetMediaMode() == MODE_BOTH || tPollInfo.GetMediaMode() ==MODE_BOTH_BOTH))
			{
				FreeRecvSpy( tOldSpeaker, MODE_BOTH);
				StopSpeakerFollowSwitch( MODE_BOTH );
			}
			*/

			// zbq [09/06/2007] ������ѯ������ƵҲSetIn,�Ƿ���ѯ����GetParam�����
            // guzh [6/14/2007] �¼��ն�ҪSetIn
            if ( !ptCurPollMt->IsLocal() )
            {
                OnMMcuSetIn(*(TMt*)ptCurPollMt, 0, SWITCH_MODE_BROADCAST);
            }
			
            //���������ͬģʽ�µĽ���
			TMt tLastMt = m_tConfPollParam.GetLastPolledMt();
			u8 bySrcChnnl = (tLastMt == m_tPlayEqp ? m_byPlayChnnl : 0);
			TMt tFeedBackMt;
            tFeedBackMt.SetNull();

			BOOL32 bIsVmpBrd = (GetVidBrdSrc().GetEqpType() == EQP_TYPE_VMP);
			//tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
            switch ( tPollInfo.GetMediaMode() )
            {
            case MODE_VIDEO:
				{
					FreeRecvSpy( tLastMt, MODE_VIDEO );	
					if (!bIsVmpBrd)
					{
						ChangeVidBrdSrc( ptCurPollMt );
					}
					m_tConfPollParam.SetLastPolledMt( (TMt)(*ptCurPollMt) );
				}
                break;
            
            case MODE_BOTH:
				{
					//���ط���
					ChangeSpeaker(ptCurPollMt, TRUE, bVmpNeedChgBySpeaker, FALSE );//VMP������ͨ���Ѿ�����ѯ�д����˴�������Ҫ
					m_tConfPollParam.SetLastPolledMt( (TMt)(*ptCurPollMt) );
				}
                break;

			case MODE_VIDEO_SPY:
			case MODE_BOTH_SPY:
				{
					u8 byMode = MODE_NONE;
					if( MODE_VIDEO_SPY == tPollInfo.GetMediaMode())
					{
						byMode = MODE_VIDEO;
					}
					else
					{
						byMode = MODE_BOTH;
					}

                    if ( /*!m_tConfPollParam.GetLastPolledMt()*/!tLastMt.IsNull() && 
						 /*!m_tConfPollParam.GetLastPolledMt()*/!tLastMt.IsLocal() )
					{
						FreeRecvSpy( /*m_tConfPollParam.GetLastPolledMt()*/tLastMt, byMode );
					}

					tFeedBackMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);
					OnSetOutView(tFeedBackMt, byMode);
					m_tConfPollParam.SetLastPolledMt( (TMt)(*ptCurPollMt) );
				}
				break;

            case MODE_VIDEO_BOTH:
				{
// 					TMt tLastMt = m_tConfPollParam.GetLastPolledMt();
// 					u8 bySrcChnnl = (tLastMt == m_tPlayEqp ? m_byPlayChnnl : 0);
					
					//������Ƶ�㲥
					FreeRecvSpy( tLastMt, MODE_VIDEO );
					if (!bIsVmpBrd)
					{
						ChangeVidBrdSrc(ptCurPollMt);
					}

					//��Ƶ�ش�
					tFeedBackMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);
					OnSetOutView(tFeedBackMt, MODE_VIDEO);
					m_tConfPollParam.SetLastPolledMt( (TMt)(*ptCurPollMt) );
				}
                break;

            case MODE_BOTH_BOTH:
                {
					//���ط���
					ChangeSpeaker(ptCurPollMt, TRUE, bVmpNeedChgBySpeaker, FALSE );//VMP������ͨ���Ѿ�����ѯ�д����˴�������Ҫ
					//����Ƶ�ش�
					tFeedBackMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);
					OnSetOutView(tFeedBackMt, MODE_BOTH);
					m_tConfPollParam.SetLastPolledMt( (TMt)(*ptCurPollMt) );
				}
                break;

			// 20110414_miaoqs ��ϯ��ѯѡ���������
			case MODE_VIDEO_CHAIRMAN:            
                {
					/*if ( !tLastPolledMt.IsNull() )
					{
						StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO, FALSE, FALSE );
					}*/
				
					if (HasJoinedChairman())
					{
						//ChangeSelectSrc((TMt)(*ptCurPollMt), m_tConf.GetChairman(), MODE_VIDEO);
						//ֱ����ѡ���߼�
						//�ϴ���ѯ���ն˺����Ҫ��ѯ���ն���ͬһ����������ѡ��
						if ( !m_tConfPollParam.GetLastPolledMt().IsNull() && m_tConfPollParam.GetLastPolledMt() == (TMt)(*ptCurPollMt) )
						{
							break;
						}
						TSwitchInfo tSwitchInfo;
						tSwitchInfo.SetSrcMt((TMt)*ptCurPollMt);
						tSwitchInfo.SetDstMt(m_tConf.GetChairman());
						tSwitchInfo.SetMode(MODE_VIDEO);
						cServMsg.SetNoSrc();
						cServMsg.SetSrcMtId(0);
						cServMsg.SetEventId(MCS_MCU_STARTSWITCHMT_REQ);
						cServMsg.SetMsgBody((u8*)&tSwitchInfo, sizeof(TSwitchInfo));
						ProcStartSelSwitchMt(cServMsg);
					}
					
					m_tConfPollParam.SetLastPolledMt( (TMt)(*ptCurPollMt) );
				}
				break;

			case MODE_BOTH_CHAIRMAN:             
                {
					/*if ( !tLastPolledMt.IsNull() )
					{
						StopSelectSrc(m_tConf.GetChairman(), MODE_BOTH, FALSE, FALSE);
					}*/
					
					if (HasJoinedChairman())
					{
						//ChangeSelectSrc((TMt)(*ptCurPollMt), m_tConf.GetChairman(), MODE_BOTH);
						//ֱ����ѡ���߼�
						//�ϴ���ѯ���ն˺����Ҫ��ѯ���ն���ͬһ����������ѡ��
						if ( !m_tConfPollParam.GetLastPolledMt().IsNull() && m_tConfPollParam.GetLastPolledMt() == (TMt)(*ptCurPollMt) )
						{
							break;
						}
						TSwitchInfo tSwitchInfo;
						tSwitchInfo.SetSrcMt((TMt)*ptCurPollMt);
						tSwitchInfo.SetDstMt(m_tConf.GetChairman());
						tSwitchInfo.SetMode(MODE_BOTH);
						cServMsg.SetNoSrc();
						cServMsg.SetSrcMtId(0);
						cServMsg.SetEventId(MCS_MCU_STARTSWITCHMT_REQ);
						cServMsg.SetMsgBody((u8*)&tSwitchInfo, sizeof(TSwitchInfo));
						ProcStartSelSwitchMt(cServMsg);
					}

					m_tConfPollParam.SetLastPolledMt( (TMt)(*ptCurPollMt) );
				}
				break;

            default:

				ConfPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcsMcuPollMsg] unexpected poll mode.%d rcved, ignore it!\n", tPollInfo.GetMediaMode());
                break;
            }          
            
			// ȫ����ش���Դ���ͷŸɾ���,����vmp����
			if (!bVmpNeedChgBySpeaker)
			{
				u8 byVmpMemType = VMP_MEMBERTYPE_POLL;
				ChangeVmpChannelParam(ptCurPollMt/*&tLocalMt*/, byVmpMemType,&tLastPolledMt);
			}

			//TvWall
			TPeriEqpStatus tTWStatus;
			u8 byChnlIdx;
			u8 byEqpId;
           
			for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
			{
				if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
				{
					for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
					{
						// [miaoqingsong 20111205] �����ж���������ϯ��ѯ/�ϴ���ѯ����MT����������ǽ��Ƶ��ѯ����ͨ��
						if ( g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus) &&
							 (TW_MEMBERTYPE_POLL == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType) &&
							 (m_byConfIdx == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx()) && 
							 !(tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN) && 
							 !(tPollInfo.GetMediaMode() == MODE_VIDEO_SPY || tPollInfo.GetMediaMode() == MODE_BOTH_SPY) 
							 )
						{
							ChangeTvWallSwitch(ptCurPollMt, byEqpId, byChnlIdx, TW_MEMBERTYPE_POLL, TW_STATE_CHANGE);
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
					u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byHduEqpId));
					if (0 == byHduChnNum)
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU, "[ReleaseConf] GetHduChnNumAcd2Eqp failed!\n");
						continue;
					}
					for (byHduChnlIdx = 0; byHduChnlIdx < byHduChnNum; byHduChnlIdx++)
					{
						// [miaoqingsong 20111205] �����ж���������ϯ��ѯ/�ϴ���ѯ����MT����������ǽ��Ƶ��ѯ����ͨ��
						if ( g_cMcuVcApp.GetPeriEqpStatus(byHduEqpId, &tHduStatus) &&
							 (TW_MEMBERTYPE_POLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].byMemberType) &&
							 (m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetConfIdx()) &&
							 !(tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN) && 
							 !(tPollInfo.GetMediaMode() == MODE_VIDEO_SPY || tPollInfo.GetMediaMode() == MODE_BOTH_SPY)
							)
						{
							// [2013/03/11 chenbing] HDU�໭��Ŀǰ��֧��������ѯ,��ͨ����0
							ChangeHduSwitch(ptCurPollMt, byHduEqpId, byHduChnlIdx, 0, TW_MEMBERTYPE_POLL, TW_STATE_START, m_tConf.m_tStatus.GetPollMedia() ); 
						}
						else
						{
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcPollingChangeTimerMsg] HduEqp%d is not exist or not polling \n", byHduEqpId);
						}
					}
				}
			}
		}
		else
		{
			// xliang [5/27/2009] ��ѯ�б���û��MT����ѻ�����ѯͣ��
			ProcStopConfPoll();
			/*
			if ( bIsPollNumNoLimit )
			{
				//NotifyMcsAlarmInfo( 0, ERR_MCU_POLLING_NOMTORVIDSRC );
				TMtPollParam tPollMt;
				tPollMt.SetNull();
				tPollInfo.SetMtPollParam( tPollMt );
				m_tConf.m_tStatus.SetPollInfo( tPollInfo );

				cServMsg.SetMsgBody( (u8*)&tPollInfo, sizeof(TPollInfo) );
				SendMsgToAllMcs( MCU_MCS_POLLSTATE_NOTIF, cServMsg );

				SetTimer(MCUVC_POLLING_CHANGE_TIMER, 1000*5);
			}
            else
            {
                ProcStopConfPoll();
            }
			*/
		}

        break;
    }
    default:
        break;
    }

    return;
}

/*====================================================================
    ������      NotifyMtToBePolledNext
    ����        �����Ͷ���Ϣ֪ͨ��һ������ѯ�����ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/05/09    4.0         �ű���         ����
====================================================================*/
void CMcuVcInst::NotifyMtToBePolledNext( void )
{
	
    TMtPollParam *ptNextPollMt;
    u8 byPollIdx = m_tConfPollParam.GetCurrentIdx();
    TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
	ptNextPollMt = GetNextMtPolled(byPollIdx, tPollInfo);
	/*
    if ( NULL != ptNextPollMt &&
	//zbq[12/25/2007]�����ݲ�֧��
	ptNextPollMt->IsLocal())
    {
	Mcu2MtNtfMsgType emMsgType = emBePolledNextNtf;
	u32 dwMsgType = htonl((u32)emMsgType);
	
	  CServMsg cServMsg;
	  cServMsg.SetConfId( m_tConf.GetConfId() );
	  cServMsg.SetEventId( MCU_MT_SOMEOFFERRING_NOTIF );
	  cServMsg.SetMsgBody( (u8*)&dwMsgType, sizeof(u32) );
	  
        SendMsgToMt( ptNextPollMt->GetMtId(), MCU_MT_SOMEOFFERRING_NOTIF, cServMsg );
		}
	*/

	if( !g_cMcuVcApp.IsSendStaticText() )
	{
		return;
	}
	
	// [pengjie 2010/4/26] ��ѯ֪ͨͳһ�߾�̬�ı�����Ϣ
    if ( NULL != ptNextPollMt )
    {
        s8 achPollingNtf[256] = "����������ѯ!\n";
        
        u16 wMtNum = 1;
        wMtNum = htons(wMtNum);
        
        CRollMsg cRollMsg;
        cRollMsg.SetRollMsgContent( (u8*)achPollingNtf, strlen(achPollingNtf) );
        cRollMsg.SetRollTimes( 1 );
        cRollMsg.SetType( ROLLMSG_TYPE_STATICTEXT );
        
		cRollMsg.SetMsgId((u8)MSGID_POLL); //  [pengguofeng 5/10/2013]

        CServMsg cServMsg;
        cServMsg.SetConfId( m_tConf.GetConfId() );
        cServMsg.SetEventId( MCS_MCU_SENDRUNMSG_CMD );
        cServMsg.SetMsgBody( (u8*)&wMtNum, sizeof(u16) );
        cServMsg.CatMsgBody( (u8*)ptNextPollMt, sizeof(TMt) );
        cServMsg.CatMsgBody( (u8*)&cRollMsg, sizeof(CRollMsg) );
        
        CMessage cMsg;
        cMsg.event = MCS_MCU_SENDRUNMSG_CMD;
        cMsg.content = cServMsg.GetServMsg();
        cMsg.length = cServMsg.GetServMsgLen();
        ProcMcsMcuSendMsgReq(&cMsg);
    }
    return;
}


/*====================================================================
������      ��NotifyMtSpeakStatus
����        ��֪ͨ�ն˵�ǰ�ķ���״̬
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����void
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
05/13/2010  4.6         �ű���         ����
====================================================================*/
void CMcuVcInst::NotifyMtSpeakStatus( TMt tMt, emMtSpeakerStatus emStatus)
{
	
	u32 dwStatus = (u32)emStatus;
	CServMsg cServMsg;
	TMsgHeadMsg tHeadMsg;
	if( tMt.IsLocal() )
	{
		dwStatus = htonl(dwStatus);		
		cServMsg.SetConfId( m_tConf.GetConfId() );
		cServMsg.SetEventId( MCU_MT_MTSPEAKSTATUS_NTF );
		cServMsg.SetMsgBody( (u8*)&dwStatus, sizeof(u32) );
		
		SendMsgToMt( tMt.GetMtId(), MCU_MT_MTSPEAKSTATUS_NTF, cServMsg );
	}
    else 
	{				
		tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
		
		cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		cServMsg.CatMsgBody( (u8*)&tMt,sizeof(TMt) );
		cServMsg.CatMsgBody( (u8*)&dwStatus,sizeof(u32) );

		cServMsg.SetEventId( MCU_MCU_SPEAKSTATUS_NTF );
		SendMsgToMt( (u8)tMt.GetMcuId(), MCU_MCU_SPEAKSTATUS_NTF, cServMsg );
	}
	
	
    ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[NotifyMtSpeakStatus] Mt(%d.%d.%d) level.%d speak status.%d has been notified!\n", tMt.GetMcuId(),tMt.GetMtId(),
		tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],tHeadMsg.m_tMsgDst.m_byCasLevel,emStatus
		);
	
    return;
}

/*====================================================================
������      ��NotiyfMtConfSpeakMode
����        ��֪ͨ�ն� ���� ��ǰ�ķ���ģʽ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����void
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
05/13/2010  4.6         �ű���         ����
====================================================================*/
void CMcuVcInst::NotiyfMtConfSpeakMode(u8 byMtId, u8 byMode )
{
    emConfSpeakMode emMode;
    switch (byMode)
    {
    case CONF_SPEAKMODE_NORMAL:
        emMode = emSpeakNormal;
        break;
    case CONF_SPEAKMODE_ANSWERINSTANTLY:
        emMode = emAnswerInstantly;
        break;
    default:
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[NotiyfMtConfSpeakMode] unexpected speak mode.%d!\n", byMode);
        return;
    }
	
	u32 dwStatus = (u32)emMode;	
	dwStatus = htonl( dwStatus );

	
    CServMsg cServMsg;
    cServMsg.SetConfId( m_tConf.GetConfId() );
    cServMsg.SetEventId( MCU_MT_CONFSPEAKMODE_NTF );
    cServMsg.SetMsgBody( (u8*)&dwStatus, sizeof(u32) );
    
    SendMsgToMt( byMtId, MCU_MT_CONFSPEAKMODE_NTF, cServMsg );
	
    return;
}

/*=============================================================================
  �� �� ���� ProcMcsMcuHduBatchPollMsg
  ��    �ܣ� hdu������ѯ��Ϣ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage *pcMsg
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  09/04/09    4.6         ���ֱ�         ����
  2013/03/11  4.7.2       �±�           �޸�(HDU�໭��֧��)
=============================================================================*/
void CMcuVcInst::ProcMcsMcuHduBatchPollMsg(const CMessage *pcMsg)
{
	if ( NULL == pcMsg )
	{
	 	ConfPrint( LOG_LVL_ERROR, MID_MCU_HDU, "[ProcMcsMcuHduBatchPollMsg] pcMsg is null!\n" );
	    return;
	}
    
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
    u8  byIndex;

	switch ( pcMsg->event )
	{
	// ��ʼ������ѯ
	case MCS_MCU_STARTHDUBATCHPOLL_REQ:
	{
        THduPollSchemeInfo tHduPollSchemeInfo;
        THduChnlCfgInfo tHduChnlCfgInfo[MAXNUM_HDUCFG_CHNLNUM];

		// [12/22/2010 liuxu][�߶�] ��Ҫ��գ��Ա������
		memset(m_tHduBatchPollInfo.m_tChnlBatchPollInfo, 0, sizeof(m_tHduBatchPollInfo.m_tChnlBatchPollInfo));

		THduChnlPollInfo *ptHduChnlPollInfo = (THduChnlPollInfo*)m_tHduBatchPollInfo.m_tChnlBatchPollInfo;


		if( POLL_STATE_NONE != m_tHduBatchPollInfo.GetStatus() )
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuHduBatchPollMsg] hdu is already in batch polling\n");
			return;
		}

		tHduPollSchemeInfo = *(THduPollSchemeInfo*)cServMsg.GetMsgBody();
        tHduPollSchemeInfo.m_tHduStyleCfgInfo.GetHduChnlCfgTable( tHduChnlCfgInfo );
        m_tHduBatchPollInfo.SetHduSchemeIdx( tHduPollSchemeInfo.m_tHduStyleCfgInfo.GetStyleIdx() );
        m_tHduBatchPollInfo.SetKeepTime( tHduPollSchemeInfo.GetKeepTime() );
        m_tHduBatchPollInfo.SetCycles( tHduPollSchemeInfo.GetCycles() );
        u8 bySchemeIdx = tHduPollSchemeInfo.m_tHduStyleCfgInfo.GetStyleIdx();

		u8 byHduChnlInPollNum = 0;        //  �ڸ�Ԥ���в���������ѯ��ͨ����
        u8 byChnlInUseNum = 0;            //  ��Ԥ����������ѯǰ��ռ�õ�ͨ���� 
		BOOL bIsChnlInUse = FALSE;        //  �Ƿ���ͨ����ռ��
		BOOL bIsSchemeChnInOtherConf = FALSE; //��ǰ������ѯԤ��ͨ����Դ�Ƿ��б���������ռ�ã�
											  //����nack, û����ͣ�����е�ǰ����ռ�õĵ���ǽ(dec5��hdu)�ٽ���������ѯ
		
		CServMsg cHduStatMsg;
		TPeriEqpStatus tHduStatus;
		for ( byIndex=0; byIndex < MAXNUM_HDUCFG_CHNLNUM; byIndex++ )
		{
			if ( tHduChnlCfgInfo[byIndex].GetEqpId() >= HDUID_MIN
				&& tHduChnlCfgInfo[byIndex].GetEqpId() <= HDUID_MAX
				)
			{
				g_cMcuVcApp.GetPeriEqpStatus( tHduChnlCfgInfo[byIndex].GetEqpId(), &tHduStatus );
                // ��Ԥ��ͨ������������ռ�ã�ֱ��nack
				if ( tHduStatus.m_byOnline == 1
					&& m_byConfIdx != tHduStatus.m_tStatus.tHdu.atVideoMt[tHduChnlCfgInfo[byIndex].GetChnlIdx()].GetConfIdx()
					&& 0 != tHduStatus.m_tStatus.tHdu.atVideoMt[tHduChnlCfgInfo[byIndex].GetChnlIdx()].GetConfIdx())
                {
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[ProcMcsMcuHduBatchPollMsg] current hdu scheme's chnls is used in other conf! please release them! \n" );
                    cServMsg.SetErrorCode( ERR_MCU_HDUBATCHPOLL_CHNLINUSE );
					SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 2, cServMsg );
					
					return;					
				}
				
				
			}			
		}

		//Bug00160221������ǽ��ѯʱ����������ѯ������ǽ��ѯˢ���쳣
		//yrl20131108 �ö���ǰ������Ϊ����������ѯ��MemberType�ı䣬���µ���ǽ��ѯ����ֹͣ
		u8 byHduIdx = tHduChnlCfgInfo[byIndex].GetEqpId();
		u8 byChnIdx = tHduChnlCfgInfo[byIndex].GetChnlIdx();
		CServMsg cSMsg;
		CMessage cMsg;
		for ( byIndex=0; byIndex < MAXNUM_HDUCFG_CHNLNUM; byIndex++ )
		{
			byHduIdx = tHduChnlCfgInfo[byIndex].GetEqpId();
			byChnIdx = tHduChnlCfgInfo[byIndex].GetChnlIdx();
			g_cMcuVcApp.GetPeriEqpStatus( byHduIdx, &tHduStatus );
			if ( byHduIdx >= HDUID_MIN && byHduIdx <= HDUID_MAX )
			{
				if ( HDUCHN_MODE_ONE == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byChnIdx) )
				{
					if (tHduStatus.m_byOnline == 1 
						&& INVALID_MCUIDX != tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].GetMcuId()
						&& m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].GetConfIdx()
						)
					{
						if (TW_MEMBERTYPE_TWPOLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].byMemberType)
						{
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU, "[ProcMcsMcuHduBatchPollMsg] Stop Hdu<EqpId:%d, ChnId:%d> Poll!\n", byHduIdx, byChnIdx);
							cSMsg.SetMsgBody((u8*)&byHduIdx, sizeof(u8));
							cSMsg.CatMsgBody((u8*)&byChnIdx, sizeof(u8));
							cMsg.content = cSMsg.GetServMsg();
							cMsg.length  = cSMsg.GetServMsgLen();
							cMsg.event   = MCS_MCU_STOPHDUPOLL_CMD;
							ProcMcsMcuHduPollMsg(&cMsg);
						}
					}
				}
			}
		}

		for ( byIndex=0; byIndex < MAXNUM_HDUCFG_CHNLNUM; byIndex++ )
		{
			if ( tHduChnlCfgInfo[byIndex].GetEqpId() >= HDUID_MIN
				&& tHduChnlCfgInfo[byIndex].GetEqpId() <= HDUID_MAX
				)
			{
				g_cMcuVcApp.GetPeriEqpStatus( tHduChnlCfgInfo[byIndex].GetEqpId(), &tHduStatus );
                
				if ( tHduStatus.m_byOnline == 1
					&& m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[tHduChnlCfgInfo[byIndex].GetChnlIdx()].GetConfIdx() )
                {
					ChangeHduSwitch( NULL, tHduChnlCfgInfo[byIndex].GetEqpId(),tHduChnlCfgInfo[byIndex].GetChnlIdx(),
									0, TW_MEMBERTYPE_NULL, TW_STATE_STOP, MODE_BOTH );							
				}
				// ����ȡhdustatus
				g_cMcuVcApp.GetPeriEqpStatus( tHduChnlCfgInfo[byIndex].GetEqpId(), &tHduStatus );
				// [2013/08/02 chenbing] ����������ѯ��Ҫ���û���ţ���ֹ��������ռ��
				tHduStatus.m_tStatus.tHdu.atVideoMt[tHduChnlCfgInfo[byIndex].GetChnlIdx()].byMemberType = TW_MEMBERTYPE_BATCHPOLL;
				tHduStatus.m_tStatus.tHdu.atVideoMt[tHduChnlCfgInfo[byIndex].GetChnlIdx()].SetConfIdx(m_byConfIdx);
				ptHduChnlPollInfo[byHduChnlInPollNum].SetPosition( byIndex );
				ptHduChnlPollInfo[byHduChnlInPollNum].SetChnlIdx( tHduChnlCfgInfo[byIndex].GetChnlIdx() );
				ptHduChnlPollInfo[byHduChnlInPollNum].SetEqpId( tHduChnlCfgInfo[byIndex].GetEqpId());
				tHduStatus.m_tStatus.tHdu.atHduChnStatus[tHduChnlCfgInfo[byIndex].GetChnlIdx()].SetSchemeIdx(bySchemeIdx );
				g_cMcuVcApp.SetPeriEqpStatus( tHduChnlCfgInfo[byIndex].GetEqpId(), &tHduStatus );
				byHduChnlInPollNum ++;

				// ������ѯ���û���ź���Ҫ�ϱ�ˢ��
				
				cHduStatMsg.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
				SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cHduStatMsg);
			}			
		}

		//����Ԥ���µ���ǽͨ��
//		for ( byIndex=0; byIndex < MAXNUM_HDUCFG_CHNLNUM; byIndex++ )
// 		{
// 			u8 byHduIdx = tHduChnlCfgInfo[byIndex].GetEqpId();
// 			u8 byChnIdx = tHduChnlCfgInfo[byIndex].GetChnlIdx();
// 			g_cMcuVcApp.GetPeriEqpStatus( byHduIdx, &tHduStatus );
// 			if ( byHduIdx >= HDUID_MIN && byHduIdx <= HDUID_MAX )
// 			{
// 				// [2013/03/11 chenbing] ������ѯ��Ҫֹͣ�ķ�� 
// 				if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byChnIdx) )
// 				{
// 					if( tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].GetConfIdx()/*����Ų�Ϊ0ʱ��Ҫ�жϻ����*/
// 						&& m_byConfIdx != tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].GetConfIdx()
// 					  )
// 					{
// 						break;
// 					}
// 
// 					for ( u8 bySubIndex=0; bySubIndex<HDU_MODEFOUR_MAX_SUBCHNNUM; bySubIndex++)
// 					{
// 						if (THduChnStatus::eRUNNING == tHduStatus.m_tStatus.tHdu.GetChnStatus(byChnIdx, bySubIndex))
// 						{
// 							//��������������е�ͨ��
// 							ChangeHduSwitch(NULL, byHduIdx, byChnIdx, bySubIndex, 
// 								TW_MEMBERTYPE_MCSSPEC, TW_STATE_STOP, MODE_VIDEO);	
// 						}	
// 					}
// 				}
// 				else
// 				{
// 					if (tHduStatus.m_byOnline == 1 
// 						&& INVALID_MCUIDX != tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].GetMcuId()
// 						&& m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].GetConfIdx()
// 						)
// 					{
// 						if (TW_MEMBERTYPE_TWPOLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].byMemberType)
// 						{
// 							CServMsg cSMsg;
// 							CMessage cMsg;
// 							ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU, "[ProcMcsMcuHduBatchPollMsg] Stop Hdu<EqpId:%d, ChnId:%d> Poll!\n", byHduIdx, byChnIdx);
// 							cSMsg.SetMsgBody((u8*)&byHduIdx, sizeof(u8));
// 							cSMsg.CatMsgBody((u8*)&byChnIdx, sizeof(u8));
// 							cMsg.content = cSMsg.GetServMsg();
// 							cMsg.length  = cSMsg.GetServMsgLen();
// 							cMsg.event   = MCS_MCU_STOPHDUPOLL_CMD;
// 				    		ProcMcsMcuHduPollMsg(&cMsg);
// 						}
// 						else
// 						{
// 							ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU, "[ProcMcsMcuHduBatchPollMsg] Stop Hdu<EqpId:%d, ChnId:%d> running!\n", byHduIdx, byChnIdx);
// 							tMember = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx];
// 							// [2013/03/11 chenbing] HDU�໭��Ŀǰ��֧��������ѯ,��ͨ����0,
// 							// [2013/03/11 chenbing] [Bug00133888]TMt��Ϊ�գ�����û��ֹͣHDUͨ��
// 							// StartSwitchToPeriEqp�н�����״̬�ж�ʧ�ܣ�û�п���HDU
// 							ChangeHduSwitch(NULL, byHduIdx, byChnIdx, 0/*��ͨ��Ϊ0*/, 
// 							tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].byMemberType,	TW_STATE_STOP);	
// 						}
// 					}
// 				}
// 			}
// 		}
	

        u32 dwCycles = tHduPollSchemeInfo.GetCycles();
		m_tHduBatchPollInfo.SetChnlPollNum( byHduChnlInPollNum );
		
		m_tConf.m_tStatus.GetHduPollInfo()->SetPollNum( dwCycles );
        m_tConf.m_tStatus.GetHduPollInfo()->SetKeepTime( tHduPollSchemeInfo.GetKeepTime() );

		// ��ʼ��һ�����ѯ 
		if ( !HduBatchPollOfOneCycle( TRUE ) )
		{
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuHduBatchPollMsg] have no Mt in current conf!\n" );
			cServMsg.SetErrorCode( ERR_MCU_HDUBATCHPOLL_NONEMT );
			SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 2, cServMsg );
            return;
		}


		TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
		tConfAttrbEx.SetSchemeIdxInBatchPoll( bySchemeIdx );
		m_tConf.SetConfAttrbEx( tConfAttrbEx );

		tHduPollSchemeInfo.SetStatus( POLL_STATE_NORMAL );

		// ����Ԥ����Ϣ
		memcpy( (void*)&m_tHduPollSchemeInfo, (void*)&tHduPollSchemeInfo, sizeof( tHduPollSchemeInfo ) );

		cServMsg.SetMsgBody( (u8*)&tHduPollSchemeInfo, sizeof( tHduPollSchemeInfo ) );
		SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 1, cServMsg);

		m_tConf.m_tStatus.m_tConfMode.SetHduInBatchPoll( POLL_STATE_NORMAL );
		m_tHduBatchPollInfo.SetStatus( POLL_STATE_NORMAL );		
		// ֪ͨ��ػ���״̬�ı�
		ConfStatusChange();            
		SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );

		//֪ͨ���л��
		cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
		if(m_tConf.HasConfExInfo())
		{
			u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
			u16 wPackDataLen = 0;
			PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
			cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
		}
	    SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
		
		SetTimer(MCUVC_HDUBATCHPOLLI_CHANGE_TIMER, 1000 * tHduPollSchemeInfo.GetKeepTime() );

	}
	break;
    
	// ֹͣhdu������ѯ
	case MCS_MCU_STOPHDUBATCHPOLL_REQ:
	{
        StopHduBatchPollSwitch(TRUE);

		SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 1, cServMsg );

		m_tHduBatchPollInfo.SetNull();
		m_tConf.m_tStatus.m_tConfMode.SetHduInBatchPoll( POLL_STATE_NONE );

 		TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
		tConfAttrbEx.SetSchemeIdxInBatchPoll( 0 );
		m_tConf.SetConfAttrbEx( tConfAttrbEx );
		THduPollInfo tHduPollInfo;		
		tHduPollInfo = *m_tConf.m_tStatus.GetHduPollInfo();
		tHduPollInfo.SetKeepTime( 10 );     //Ĭ��10��
		tHduPollInfo.SetPollNum( 0 );
        m_tConf.m_tStatus.SetHduPollInfo( tHduPollInfo );
		// ֪ͨ��ػ���״̬�ı�
        ConfStatusChange();            
        m_tHduPollSchemeInfo.SetNull();
		cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof( m_tHduPollSchemeInfo ) );
        SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );

		//֪ͨ���л��
		cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
		if(m_tConf.HasConfExInfo())
		{
			u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
			u16 wPackDataLen = 0;			
			PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
			cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
		}
	    SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

		KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
	}
	    break;
    
	// ��ͣhdu������ѯ
	case MCS_MCU_PAUSEHDUBATCHPOLL_REQ:
	{
		SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 1, cServMsg );

	    m_tHduBatchPollInfo.SetStatus( POLL_STATE_PAUSE );
        m_tHduPollSchemeInfo.SetStatus( POLL_STATE_PAUSE );

		m_tConf.m_tStatus.m_tConfMode.SetHduInBatchPoll( POLL_STATE_PAUSE );
		ConfStatusChange();            // ֪ͨ��ػ���״̬�ı�
		cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof( m_tHduPollSchemeInfo ) );
        SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );

		//֪ͨ���л��
		cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
		if(m_tConf.HasConfExInfo())
		{
			u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
			u16 wPackDataLen = 0;
			PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
			cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
		}
	    SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
		
		KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
	}
	    break;
    
	// �ָ�hdu������ѯ
	case MCS_MCU_RESUMEHDUBATCHPOLL_REQ:
	{
		SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 1, cServMsg );
		m_tHduBatchPollInfo.SetStatus( POLL_STATE_NORMAL );
		m_tHduPollSchemeInfo.SetStatus( POLL_STATE_NORMAL );
		
		SetTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER, 1000 * m_tHduBatchPollInfo.GetKeepTime() );

		m_tConf.m_tStatus.m_tConfMode.SetHduInBatchPoll( POLL_STATE_NORMAL );
		ConfStatusChange();            // ֪ͨ��ػ���״̬�ı�
		cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof( m_tHduPollSchemeInfo ) );
		SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );
		//֪ͨ���л��
		cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
		if(m_tConf.HasConfExInfo())
		{
			u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
			u16 wPackDataLen = 0;
			PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
			cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
		}
	    SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

	}
	    break;
		
	case MCS_MCU_GETBATCHPOLLSTATUS_CMD:
		{
			cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof( m_tHduPollSchemeInfo ) );
			SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );
		}
		break;

	default:
		ConfPrint( LOG_LVL_ERROR, MID_MCU_HDU,  "[ProcMcsMcuHduBatchPollMsg] message type(%u) is wrong!\n" , pcMsg->event );
    
    } 	
	return;
}

/*=============================================================================
  �� �� ���� ProcMcsMcuHduPollMsg
  ��    �ܣ� hdu��ѯ��Ϣ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage *pcMsg
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  09/04/09    4.6         ���ֱ�         ����
=============================================================================*/
void CMcuVcInst::ProcMcsMcuHduPollMsg(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch (pcMsg->event)
	{
		case MCS_MCU_STARTHDUPOLL_CMD:
			{
				TPeriEqpStatus tHduStatus;
				TTvWallPollParam tTWPollParam = *(TTvWallPollParam *)(cServMsg.GetMsgBody());
				TTvWallPollInfo  tTwPollInfo  = tTWPollParam.GetTWPollInfo();

				TEqp tHdu          = tTWPollParam.GetTvWall();
				u8   byHduId       = tTWPollParam.GetTvWall().GetEqpId();
				u8   byHduChnId    = tTWPollParam.GetTWChnnl();
				u8   bySchemeIdx   = tTWPollParam.GetSchemeIdx();
				u8   byPolledMtNum = tTWPollParam.GetPolledMtNum();
				u8   byMode        = tTWPollParam.GetMediaMode();
				TMtPollParam *ptMtPollParam = tTWPollParam.GetPollMtByIdx(0);
				
				u8 byPollState   = POLL_STATE_NONE;
				u32 dwTimerIdx    = 0;

				u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(tHdu);
				if (0 == byHduChnNum)
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[ProcMcsMcuHduPollMsg] GetHduChnNumAcd2Eqp failed!\n");
					return;
				}

				if (byHduChnId >= byHduChnNum || !(byHduId >= HDUID_MIN && byHduId <= HDUID_MAX))
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[ProcMcsMcuHduPollMsg] hdu <EqpId:%d, ChnId:%d> is invalid :%d\n", byHduId, byHduChnId);
                    break;
				}
				
				g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
				// [2013/03/11 chenbing] �ķ���ܽ��е�ͨ����ѯ 
				if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byHduChnId) )
				{
					ConfPrint( LOG_LVL_ERROR, MID_MCU_HDU, "[ProcMcsMcuHduPollMsg] Chnnl %d is can't polling, because it is HDUCHN_MODE_FOUR\n", byHduChnId);
					return;
				}

				tHduStatus.m_tStatus.tHdu.atHduChnStatus[byHduChnId].SetSchemeIdx(bySchemeIdx);
                g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);

				THduMember tHduMem = tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnId];
				if(0 != tHduMem.GetConfIdx() && tHduMem.GetConfIdx() != m_byConfIdx)          //���������û��Ȩ�޲���, ��������滻
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[ProcMcsMcuHduPollMsg] Confidx %d is already in polling, can't replace it\n", tHduMem.GetConfIdx());
					break;
				}

                if( m_tTWMutiPollParam.GetPollState(byHduId, byHduChnId, byPollState) &&
					POLL_STATE_NONE != byPollState)
				{
					// ��ֹ����m_tTWMutiPollParam��ͨ����ѯ״̬û����յ�����ѯʧ��
					if (tHduMem.byMemberType != TW_MEMBERTYPE_NULL && tHduMem.GetConfIdx() == m_byConfIdx )
					{
						ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuHduPollMsg] hdu <EqpId:%d, ChnId:%d> is already in polling\n", byHduId, byHduChnId);
						break;
					}
				}

				//���õ�ǰͨ����ѯ��Ϣ
				m_tTWMutiPollParam.SetTWPollInfo(byHduId, byHduChnId, tTwPollInfo);
                m_tTWMutiPollParam.InitPollParam(byHduId, byHduChnId, byPolledMtNum, ptMtPollParam);			

// 				// xsl [8/23/2006] �ж����ǻ����Ƿ�ֹͣ��ѯ
// 				if (m_tConf.GetConfAttrb().IsSatDCastMode())
// 				{            
// 					//���ش�������������ѯ�ն����в����������ն���ܾ���ѯ
// 					if (IsOverSatCastChnnlNum(0))
// 					{
// 						//�߼����󣬲������[8/8/2012 chendaiwei]
// // 						for(u8 byIdx = 0; byIdx < byPolledMtNum; byIdx++)
// // 						{
// // 							TMtPollParam *ptMtPoll = NULL;
// // 							if (m_tTWMutiPollParam.GetPollMtByIdx(byHduId, byHduChnId, byIdx, ptMtPoll) &&
// // 								ptMtPoll && m_ptMtTable->GetMtSndBitrate(ptMtPoll->GetMtId()) == 0)
// // 							{
// // 								ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuHduPollMsg-start] over max upload mt num. nack!\n");  
// // 								NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_DCAST_OVERCHNNLNUM);
// // 								return;
// // 							}
// // 						}
// 					}            
// 				}    
				                   
				u8 byCurPollPos = 0;
				m_tTWMutiPollParam.GetCurrentIdx(byHduId, byHduChnId, byCurPollPos);
				m_tTWMutiPollParam.SetIsStartAsPause(byHduId, byHduChnId, 0);
				
				//��ȡ��ʱ������
				if(!m_tTWMutiPollParam.GetTimerIdx(byHduId, byHduChnId, dwTimerIdx))
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF," [ProcMcsMcuHduPollMsg] Cannot HDU<EqpId:%d, ChnId:%d> getTimerIdx!\n",byHduId, byHduChnId);
					return;
				}

				// [10/28/2011 liuxu] ��ʱ��ѯ����ɹ�
				m_tTWMutiPollParam.SetPollState(byHduId, byHduChnId, POLL_STATE_NORMAL);

				TMtPollParam *ptCurPollMt = GetMtTWPollParam(byCurPollPos, byHduId, byHduChnId);
				m_tTWMutiPollParam.SetCurrentIdx(byHduId, byHduChnId, byCurPollPos);
				if (NULL != ptCurPollMt)
				{
					u32 dwSetTimer = ptCurPollMt->GetKeepTime();
					TMt tOrgMt     = *ptCurPollMt;   
				
					//֪ͨ���л��
				    m_tTWMutiPollParam.SetCurPolledMt(byHduId, byHduChnId, *ptCurPollMt);
					m_tTWMutiPollParam.GetTWPollParam(byHduId, byHduChnId, tTWPollParam);
					tTWPollParam.SetConfIdx(m_byConfIdx);
					m_tTWMutiPollParam.SetTWPollParam(byHduId, byHduChnId, tTWPollParam);
					cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
					SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
					
					BOOL32 bCanInTvw = TRUE;
					if ( VCS_CONF == m_tConf.GetConfSource() )
					{
						//���¼��е�ǰ�����նˣ���ͬһ���¼��������ն��������ǽ����NACK
						if( !tOrgMt.IsLocal() 
							&& !IsLocalAndSMcuSupMultSpy(tOrgMt.GetMcuId()) 
							&& IsMtInMcu(GetLocalMtFromOtherMcuMt(tOrgMt), m_cVCSConfStatus.GetCurVCMT()) 
							&& !( m_cVCSConfStatus.GetCurVCMT() == tOrgMt ))
						{
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  
								"[ProcMcsMcuHduPollMsg] CurVCMT(%d.%d) and Mt(%d.%d) is in same smcu,Mt can't in tvwall \n ",
								m_cVCSConfStatus.GetCurVCMT().GetMcuId(),
								m_cVCSConfStatus.GetCurVCMT().GetMtId(),
								tOrgMt.GetMcuId(),
								tOrgMt.GetMtId() );

							bCanInTvw = FALSE;							
						}	
					}

					if( bCanInTvw )
					{
						// [2013/03/11 chenbing] HDU�໭��Ŀǰ��֧��������ѯ,��ͨ����0
						ChangeHduSwitch(&tOrgMt, byHduId, byHduChnId, 0, TW_MEMBERTYPE_TWPOLL, TW_STATE_START, byMode);
					}
					ConfStatusChange();
					
					//������ѯʱ��
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, 1000 * dwSetTimer, dwTimerIdx);
				}
				else
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuHduPollMsg-start] Cannot CurPollTMt<CurPos:%d> Param in hdu<EqpId%d, ChnId:%d>!\n", 
									byCurPollPos, byHduId, byHduChnId);
					//������ѯʱ��
					// [2013/06/27 chenbing] (δ�ҵ��ն��Ƿ���Բ������ö�ʱ��)
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, 1000 * 5, dwTimerIdx);
					// [2013/06/27 chenbing] �޸�Bug00145985���˴���Ҫ���û���ţ��粻���ý����¶�ʱ�������ѯ������ѯ�ն�ʱ��
					// ���������ϱ���ǰ��ѯ״̬ʱ�޻���ţ����½��������޷������ѯ״̬��
					m_tTWMutiPollParam.GetTWPollParam(byHduId, byHduChnId, tTWPollParam);
					tTWPollParam.SetConfIdx(m_byConfIdx);
					m_tTWMutiPollParam.SetTWPollParam(byHduId, byHduChnId, tTWPollParam);
				}      

				// [10/20/2011 liuxu] Ԥռ�õ���ǽ
				g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
				tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnId].SetConfIdx(m_byConfIdx);
				tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnId].byMemberType = TW_MEMBERTYPE_TWPOLL;
                g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);
				
				CServMsg cHduStatusMsg;
				cHduStatusMsg.SetMsgBody( (u8*)&tHduStatus, sizeof( tHduStatus ) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cHduStatusMsg );
			}
			break;
	    case MCS_MCU_PAUSEHDUPOLL_CMD: 
			{
				u8 byHduId = *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));
				m_tTWMutiPollParam.SetPollState(byHduId, byChnId, POLL_STATE_PAUSE);
				TTvWallPollParam tTWPollParam;
				m_tTWMutiPollParam.GetTWPollParam(byHduId, byChnId, tTWPollParam);

			    cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
				
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byHduId, byChnId, dwTimerIdx) )
				{
					KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
				}	
			}
			break;
		case MCS_MCU_RESUMEHDUPOLL_CMD:
			{
				u8 byHduId = *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));
				m_tTWMutiPollParam.SetPollState(byHduId, byChnId, POLL_STATE_NORMAL);
				TTvWallPollParam tTWPollParam;
				m_tTWMutiPollParam.GetTWPollParam(byHduId, byChnId, tTWPollParam);

				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
				m_tTWMutiPollParam.SetIsStartAsPause(byHduId, byChnId, 1);
				
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byHduId, byChnId, dwTimerIdx) )
				{
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, 1000, dwTimerIdx);
				}
			}
			break;

	    case MCS_MCU_STOPHDUPOLL_CMD:
			{	
				u8 byHduId = *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));

				TPeriEqpStatus tHduStatus;
 				g_cMcuVcApp.GetPeriEqpStatus( byHduId, &tHduStatus );
				if ( tHduStatus.m_tStatus.tHdu.atVideoMt[byChnId].byMemberType != TW_MEMBERTYPE_TWPOLL
					|| tHduStatus.m_tStatus.tHdu.atVideoMt[byChnId].GetConfIdx() != m_byConfIdx )
				{
					break;
				}

				//������Ҫ����ϱ����棬�����ȼ�¼�������ڲ𵽵���ǽ����
				TMt tHduMem = m_tTWMutiPollParam.GetCurPolledMt(byHduId, byChnId);

				TMtPollParam tMtPollParam;
                memset(&tMtPollParam, 0, sizeof(tMtPollParam));
				m_tTWMutiPollParam.SetCurPolledMt(byHduId, byChnId, tMtPollParam);
				m_tTWMutiPollParam.SetPollState(byHduId, byChnId, POLL_STATE_NONE);				
				m_tTWMutiPollParam.SetPollNum(byHduId, byChnId, 0);

				TMtPollParam atTWPollParam[MAXNUM_CONF_MT];
				m_tTWMutiPollParam.InitPollParam(byHduId, byChnId, MAXNUM_CONF_MT, atTWPollParam);
				m_tTWMutiPollParam.SetPolledMtNum(byHduId, byChnId, 0);

				TTvWallPollParam tTWPollParam;				
				m_tTWMutiPollParam.GetTWPollParam(byHduId, byChnId, tTWPollParam);
				
				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);				
 				
				if (!tHduMem.IsNull())
				{
					// [2013/03/11 chenbing] HDU�໭��Ŀǰ��֧��������ѯ,��ͨ����0
					ChangeHduSwitch(NULL, byHduId, byChnId, 0, TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);
				}			
				m_tTWMutiPollParam.SetIsStartAsPause(byHduId, byChnId, 0);	
				TTvWallPollParam tTWPollParamNull;				
				m_tTWMutiPollParam.SetTWPollParam(byHduId, byChnId, tTWPollParamNull);

				// ���»�ȡ״̬
 				g_cMcuVcApp.GetPeriEqpStatus( byHduId, &tHduStatus );
				// Ԥռ�õĵ���ǽͨ��, ��Ҫ�ͷ�
				if (tHduStatus.m_tStatus.tHdu.atVideoMt[byChnId].byMemberType == TW_MEMBERTYPE_TWPOLL
					&& tHduStatus.m_tStatus.tHdu.atVideoMt[byChnId].GetConfIdx() == m_byConfIdx )
				{
					tHduStatus.m_tStatus.tHdu.atVideoMt[byChnId].byMemberType = TW_MEMBERTYPE_NULL;
					tHduStatus.m_tStatus.tHdu.atVideoMt[byChnId].SetConfIdx(0);
				}

				tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnId].SetSchemeIdx( 0 );
				g_cMcuVcApp.SetPeriEqpStatus( byHduId, &tHduStatus );
				
				CServMsg cHduStatusMsg;
				cHduStatusMsg.SetMsgBody( (u8*)&tHduStatus, sizeof( tHduStatus ) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cHduStatusMsg );

				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byHduId, byChnId, dwTimerIdx) )
				{
					KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
				}
			}
			break;
		case MCS_MCU_CHANGEHDUPOLLPARAM_CMD:
			{
				TTvWallPollParam tTWPollParam = *(TTvWallPollParam *)(cServMsg.GetMsgBody());
				TTvWallPollInfo  tTwPollInfo  = tTWPollParam.GetTWPollInfo();

				u8   byPolledMtNum = tTWPollParam.GetPolledMtNum();
				u8   byHduId       = tTWPollParam.GetTvWall().GetEqpId();
				u8   byHduChnId    = tTWPollParam.GetTWChnnl();

				//���õ�ǰͨ����ѯ��Ϣ
				m_tTWMutiPollParam.SetTWPollInfo(byHduId, byHduChnId, tTwPollInfo);

				//��ȡ������ѯ�ն�
				TMtPollParam *ptMtPollParam = tTWPollParam.GetPollMtByIdx(0);
				
				// �������б�
				tTWPollParam.SetPollList(byPolledMtNum, ptMtPollParam);

				//֪ͨ���л��
				m_tTWMutiPollParam.SetTWPollParam(byHduId, byHduChnId, tTWPollParam);
				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(tTWPollParam));
				SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
			}
			break;
		case MCS_MCU_GETHDUPOLLPARAM_REQ:
			{
				u8 byHduId = *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));
				TTvWallPollParam tTWPollParam;
				if(!m_tTWMutiPollParam.GetTWPollParam(byHduId, byChnId, tTWPollParam))
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[ProcMcsMcuHduPollMsg->GETHDUPOLLPARAM_REQ] Cannot Get <EqpId:%d, ChnId:%d> HduPollParam!\n",
						           byHduId, byChnId);
					SendReplyBack(cServMsg, pcMsg->event + 2);
					return;
				}

				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendReplyBack(cServMsg, pcMsg->event + 1);	
			}
			break;
		default:
			break;
	}
	return;
}

/*=============================================================================
  �� �� ���� ProcMcsMcuTWPollMsg
  ��    �ܣ� ����ǽ��ѯ��Ϣ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage *pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::ProcMcsMcuTWPollMsg(const CMessage *pcMsg)
{
	 CServMsg cServMsg(pcMsg->content, pcMsg->length);
  
	 switch (pcMsg->event)
	 {
		case MCS_MCU_STARTTWPOLL_CMD:
			{
				TPeriEqpStatus tTWStatus;
				TTvWallPollParam tTWPollParam = *(TTvWallPollParam *)(cServMsg.GetMsgBody());
				TTvWallPollInfo  tTWPollInfo  = tTWPollParam.GetTWPollInfo();
				TEqp tTvWall       = tTWPollParam.GetTvWall();
				u8   byTWId        = tTWPollParam.GetTvWall().GetEqpId();
				u8   byTWChnId     = tTWPollParam.GetTWChnnl();
				u8   bySchemeIdx   = tTWPollParam.GetSchemeIdx();
				u8   byPolledMtNum = tTWPollParam.GetPolledMtNum();
				u8   byMode        = tTWPollParam.GetMediaMode();
			
				//��ȡ������ѯ�ն�
				TMtPollParam *ptMtPollParam = tTWPollParam.GetPollMtByIdx(0);
				u8 byPollState   = POLL_STATE_NONE;
				u32 dwTimerIdx    = 0;
				
				if (byTWChnId >= MAXNUM_PERIEQP_CHNNL ||  !(byTWId >= TVWALLID_MIN && byTWId <= TVWALLID_MAX) )
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuTWPollMsg] DEC5<EqpId:%, ChnId:%d>  is invalid!\n", byTWId, byTWChnId);
                    break;
				}
				
				g_cMcuVcApp.GetPeriEqpStatus(byTWId, &tTWStatus);
				TTWMember tTwMem = tTWStatus.m_tStatus.tTvWall.atVideoMt[byTWChnId];

				if(0 != tTwMem.GetConfIdx() && tTwMem.GetConfIdx() != m_byConfIdx)//���������û��Ȩ�޲���, ��������滻
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuTWPollMsg] Confidx %d is already in polling, can't replace it\n", tTwMem.GetConfIdx());
					break;
				}

				if( m_tTWMutiPollParam.GetPollState(byTWId, byTWChnId, byPollState) &&
					POLL_STATE_NONE != byPollState)
				{
					// ��ֹ����m_tTWMutiPollParam��ͨ����ѯ״̬û����յ�����ѯʧ��
					if (tTwMem.byMemberType != TW_MEMBERTYPE_NULL && tTwMem.GetConfIdx() == m_byConfIdx )
					{
						ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuTWPollMsg] tvwall <EqpId:%d, ChnId:%d> is already in polling\n", 
							byTWId, byTWChnId);
						break;
					}					
				}
				
				//���õ�ǰͨ����ѯ��Ϣ
				m_tTWMutiPollParam.SetTWPollInfo(byTWId, byTWChnId, tTWPollInfo);
				m_tTWMutiPollParam.InitPollParam(byTWId, byTWChnId, byPolledMtNum, ptMtPollParam);
				// [10/28/2011 liuxu] ��ʱ������������ѯ״̬, ��Ϊ���滹������ѯʧ��
				// m_tTWMutiPollParam.SetPollState(byTWId, byTWChnId, POLL_STATE_NORMAL);			
		        
// 				// xsl [8/23/2006] �ж����ǻ����Ƿ�ֹͣ��ѯ
// 				if (m_tConf.GetConfAttrb().IsSatDCastMode())
// 				{            
// 					//���ش�������������ѯ�ն����в����������ն���ܾ���ѯ
// 					if (IsOverSatCastChnnlNum(0))
// 					{
// 						//�߼����󣬲������ [8/8/2012 chendaiwei]
// // 						for(u8 byIdx = 0; byIdx < byPolledMtNum; byIdx++)
// // 						{
// // 							TMtPollParam *ptMtPoll = NULL;
// // 							if (m_tTWMutiPollParam.GetPollMtByIdx(byTWId, byTWChnId, byIdx, ptMtPoll) &&
// // 								ptMtPoll && m_ptMtTable->GetMtSndBitrate(ptMtPoll->GetMtId()) == 0)
// // 							{
// // 								ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuTWPollMsg-start] over max upload mt num. nack!\n");  
// // 								NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_DCAST_OVERCHNNLNUM);
// // 								return;
// // 							}
// // 						}
// 					}            
// 				}                        

				// [10/28/2011 liuxu] �Ƶ���ȡ��ѯ�ն�֮ǰ
				//��ȡ��ʱ������
				if(!m_tTWMutiPollParam.GetTimerIdx(byTWId, byTWChnId, dwTimerIdx))
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF," [ProcMcsMcuHduPollMsg] Cannot get DEC5<EqpId:%d, ChnId:%d> TimerIdx!\n", byTWId, byTWChnId);
					return;
				}
				
				u8 byCurPollPos = 0;
				m_tTWMutiPollParam.GetCurrentIdx(byTWId, byTWChnId, byCurPollPos);

				//�����ù��ú���GetMtTWPollParam
				TMtPollParam *ptCurPollMt = GetMtTWPollParam(byCurPollPos, byTWId, byTWChnId);
				m_tTWMutiPollParam.SetCurrentIdx(byTWId, byTWChnId, byCurPollPos);
				m_tTWMutiPollParam.SetIsStartAsPause(byTWId, byTWChnId, 0);
				
				// [10/28/2011 liuxu] ��ʱ��ѯ����ɹ�
				m_tTWMutiPollParam.SetPollState(byTWId, byTWChnId, POLL_STATE_NORMAL);
				
				if (NULL != ptCurPollMt)
				{
					TMt tOrgMt = *ptCurPollMt;   
					u32 dwPollTimer = ptCurPollMt->GetKeepTime();
				
					//֪ͨ���л��
					m_tTWMutiPollParam.SetCurPolledMt(byTWId, byTWChnId, *ptCurPollMt);
					m_tTWMutiPollParam.GetTWPollParam(byTWId, byTWChnId, tTWPollParam);
					tTWPollParam.SetConfIdx(m_byConfIdx);
					m_tTWMutiPollParam.SetTWPollParam(byTWId, byTWChnId, tTWPollParam);

					cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
					SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
					
					ChangeTvWallSwitch(&tOrgMt, byTWId, byTWChnId, TW_MEMBERTYPE_TWPOLL, TW_STATE_START);
					
					ConfStatusChange();
					
					//������ѯʱ��
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, 1000 * dwPollTimer, dwTimerIdx);
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuTWPollMsg-start] SetTimer EventId: %d, Timer:%d, TimerIdx:%d ", 
									MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, dwPollTimer, dwTimerIdx);
				}
				else
				{
					//������ѯʱ��
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, 1000 * 5, dwTimerIdx);
				}            

				// [10/19/2011 liuxu] ��������, ��Ԥռ��
				g_cMcuVcApp.GetPeriEqpStatus(byTWId, &tTWStatus);
				tTWStatus.m_tStatus.tTvWall.atVideoMt[byTWChnId].SetConfIdx(m_byConfIdx);
				tTWStatus.m_tStatus.tTvWall.atVideoMt[byTWChnId].byMemberType = TW_MEMBERTYPE_TWPOLL;
                g_cMcuVcApp.SetPeriEqpStatus(byTWId, &tTWStatus);
				
				CServMsg cTvwStatusMsg;
				cTvwStatusMsg.SetMsgBody( (u8*)&tTWStatus, sizeof( tTWStatus ) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cTvwStatusMsg );
			}
			break;

		case MCS_MCU_PAUSETWPOLL_CMD:
			{				
				u8 byTWId =  *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));

				ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "---------DEC5<EqpID:%d, ChnId:%d>---------\n", byTWId, byChnId);
				
				m_tTWMutiPollParam.SetPollState(byTWId, byChnId, POLL_STATE_PAUSE);
				TTvWallPollParam tTWPollParam;
				m_tTWMutiPollParam.GetTWPollParam(byTWId, byChnId, tTWPollParam);
				
				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
				
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byTWId, byChnId, dwTimerIdx) )
				{
					KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
				}		
				
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuTWPollMsg-PAUSE] kill EventId: %d, TimerIdx:%d \n", 
									MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, dwTimerIdx);
			}
			break;

		case MCS_MCU_RESUMETWPOLL_CMD:
			{
				u8 byTWId  = *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));
				m_tTWMutiPollParam.SetPollState(byTWId, byChnId, POLL_STATE_NORMAL);
				TTvWallPollParam tTWPollParam;
				m_tTWMutiPollParam.GetTWPollParam(byTWId, byChnId, tTWPollParam);
				
				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
				m_tTWMutiPollParam.SetIsStartAsPause(byTWId, byChnId, 1);
				
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byTWId, byChnId, dwTimerIdx) )
				{
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, 1000, dwTimerIdx);
				}

				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuTWPollMsg-RESUME] SetTimer EventId: %d, Timer:1000, TimerIdx:%d \n", 
									MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, dwTimerIdx);
			}
			break;

		case MCS_MCU_STOPTWPOLL_CMD:
			{
				u8 byTWId  = *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));

				TPeriEqpStatus tTvwStatus;
				g_cMcuVcApp.GetPeriEqpStatus( byTWId, &tTvwStatus );
				if ( tTvwStatus.m_tStatus.tTvWall.atVideoMt[byChnId].byMemberType != TW_MEMBERTYPE_TWPOLL
					|| tTvwStatus.m_tStatus.tTvWall.atVideoMt[byChnId].GetConfIdx() != m_byConfIdx )
				{
					break;
				}
				
				//������Ҫ����ϱ����棬�����ȼ�¼�������ڲ𵽵���ǽ����
				TMt tTWMem = m_tTWMutiPollParam.GetCurPolledMt(byTWId, byChnId);
				
				//��յ�ǰ����ǽͨ��������ѯ��Ϣ			
				TMtPollParam tMtPollParam;
                memset(&tMtPollParam, 0, sizeof(tMtPollParam));				
				m_tTWMutiPollParam.SetCurPolledMt(byTWId, byChnId, tMtPollParam);
				m_tTWMutiPollParam.SetPollState(byTWId, byChnId, POLL_STATE_NONE);			
				m_tTWMutiPollParam.SetPollNum(byTWId, byChnId, 0);
				
				TMtPollParam atTWPollParam[MAXNUM_CONF_MT];
				m_tTWMutiPollParam.InitPollParam(byTWId, byChnId, MAXNUM_CONF_MT, atTWPollParam);
				m_tTWMutiPollParam.SetPolledMtNum(byTWId, byChnId, 0);

				TTvWallPollParam tTWPollParam;
				m_tTWMutiPollParam.GetTWPollParam(byTWId, byChnId, tTWPollParam);
				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);				
				
				if (!tTWMem.IsNull())
				{
					ChangeTvWallSwitch(&tTWMem, byTWId, byChnId, TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);
				}else
				{
					g_cMcuVcApp.GetPeriEqpStatus( byTWId, &tTvwStatus );
					if ( tTvwStatus.m_tStatus.tTvWall.atVideoMt[byChnId].byMemberType != TW_MEMBERTYPE_TWPOLL
						|| tTvwStatus.m_tStatus.tTvWall.atVideoMt[byChnId].GetConfIdx() != m_byConfIdx )
					{
						break;
					}

					CServMsg cStatusMsg;
					cStatusMsg.SetMsgBody( (u8*)&tTvwStatus, sizeof( tTvwStatus ) );
					SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cStatusMsg );
				}

				m_tTWMutiPollParam.SetIsStartAsPause(byTWId, byChnId, 0);	
				TTvWallPollParam tTWPollParamNull;				
				m_tTWMutiPollParam.SetTWPollParam(byTWId, byChnId, tTWPollParamNull);
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byTWId, byChnId, dwTimerIdx) )
				{
					KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
				}

				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[ProcMcsMcuTWPollMsg-STOP] kill EventId: %d, TimerIdx:%d \n", 
									MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, dwTimerIdx);
			}
			break;

		case MCS_MCU_GETTWPOLLPARAM_REQ:
			{
				u8 byTWId = *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));
				TTvWallPollParam tTWPollParam;
				if(!m_tTWMutiPollParam.GetTWPollParam(byTWId, byChnId, tTWPollParam))
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[ProcMcsMcuHduPollMsg->GETTWPOLLPARAM_REQ] Cannot Get <EqpId:%d, ChnId:%d> HduPollParam!\n",
						byTWId, byChnId);
					SendReplyBack(cServMsg, pcMsg->event + 2);	
					return;
				}
				
				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendReplyBack(cServMsg, pcMsg->event + 1);	
			}
			break;
		default:
			break;
	 }

    return;
}

/*=============================================================================
  �� �� ���� ProcHduPollingChangeTimerMsg
  ��    �ܣ� hdu��ѯ��ʱ�л�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage *pcMsg
  �� �� ֵ�� void 
  ----------------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  09/04/11    4.6         ���ֱ�         ����
=============================================================================*/
void CMcuVcInst::ProcHduBatchPollChangeTimerMsg(const CMessage *pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);	

    switch(CurState())
    {
    case STATE_ONGOING:
    {
		if ( POLL_STATE_NONE == m_tHduBatchPollInfo.GetStatus() )
		{
            ConfPrint(LOG_LVL_ERROR, MID_MCU_HDU, "current state is not hdu batch polling! \n" );
			KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
				
		    return;
		}
        
		//StopHduBatchPollSwitch();
       
		// ��ѯ����
		if ( ( !HduBatchPollOfOneCycle(FALSE) ) 
			|| ( 0 != m_tHduBatchPollInfo.GetCycles() 
				&& m_tHduBatchPollInfo.GetCurrentCycle() == m_tHduBatchPollInfo.GetCycles() )
			)
		{
			StopHduBatchPollSwitch(TRUE);
			m_tHduBatchPollInfo.SetNull();
			m_tConf.m_tStatus.m_tConfMode.SetHduInBatchPoll( POLL_STATE_NONE );
			TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
			tConfAttrbEx.SetSchemeIdxInBatchPoll( 0 );
			m_tConf.SetConfAttrbEx(tConfAttrbEx);
			m_tHduPollSchemeInfo.SetStatus( POLL_STATE_NONE );
			ConfStatusChange();            // ֪ͨ��ػ���״̬�ı�

			cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof( m_tHduPollSchemeInfo ) );
			SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );
			
			//֪ͨ���л��
			cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
			if(m_tConf.HasConfExInfo())
			{
				u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
				u16 wPackDataLen = 0;
				PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
				cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
			}
			SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

			KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
		}
		//������ѯ
		else
		{
			cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof(m_tHduPollSchemeInfo) );
            SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );
			
			SetTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER, 1000 * m_tHduBatchPollInfo.GetKeepTime() );
		}

	    break;
	}
	default:
		ConfPrint( LOG_LVL_ERROR, MID_MCU_HDU,  "[ProcHduBatchPollChangeTimerMsg] state(%d) is wrong! \n", CurState() );
	}
	return;
}

/*=============================================================================
  �� �� ���� ProcTWPollingChangeTimerMsg
  ��    �ܣ� ����ǽ��ѯ��ʱ�л�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage *pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::ProcTWPollingChangeTimerMsg(const CMessage *pcMsg)
{
	BOOL32 bClearTwPollStatus = FALSE;
	
	u8 byPollState = 0xff;
	u8 byCurPollPos = 0xff;
	u8 byIsStartAsPause = 0xff;
	u8 byTWId  = 0xff;
	u8 byChnId = 0xff;
	u8 byMode = 0xff;

	u32 byTimerIdx = 0xffff;

	CServMsg cServMsg;

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			byTimerIdx   = *(u32*)pcMsg->content;
			ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP, "OnTWPollChgTimer:EvId:%d, Timerid:%d\n",pcMsg->event, byTimerIdx);
			
			TTvWallPollParam *ptTWPollParam = m_tTWMutiPollParam.GetTWPollParamByTimerIdx((u8)byTimerIdx);
			if (NULL == ptTWPollParam)
			{
				bClearTwPollStatus = TRUE;
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcTWPollingChangeTimerMsg] GetTWPollParamByTimerIdx failed!\n");
				break;
			}
			
			byPollState      = ptTWPollParam->GetPollState();
			byCurPollPos     = ptTWPollParam->GetCurrentIdx();
			byIsStartAsPause = ptTWPollParam->GetIsStartAsPause();
			byTWId           = ptTWPollParam->GetTvWall().GetEqpId();
			byChnId          = ptTWPollParam->GetTWChnnl();
			byMode			= ptTWPollParam->GetMediaMode();
			
			if ( POLL_STATE_NORMAL != byPollState ) 
			{
				if( byPollState != POLL_STATE_PAUSE)
				{
					bClearTwPollStatus = TRUE;
				}
				
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Tvw.(%d, %d) PollState is not running!\n", byTWId, byChnId);
				break;
			}
			
			TPeriEqpStatus tTvwStatus;
			g_cMcuVcApp.GetPeriEqpStatus( byTWId, &tTvwStatus );
			
			TTvwMember tTvwMember;
			if (byTWId >= TVWALLID_MIN && byTWId <= TVWALLID_MAX)
			{		
				tTvwMember = tTvwStatus.m_tStatus.tTvWall.atVideoMt[byChnId];
			}
			else if(byTWId >= HDUID_MIN && byTWId <= HDUID_MAX)
			{
				tTvwMember = tTvwStatus.m_tStatus.tHdu.atVideoMt[byChnId];  
			}
			else
			{
				bClearTwPollStatus = TRUE;
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Tvw.(%d, %d) invalid tvw eqp, over!\n", byTWId, byChnId);
				break;
			}
			
			if (( tTvwMember.byMemberType != TW_MEMBERTYPE_TWPOLL && tTvwMember.byMemberType != TW_MEMBERTYPE_NULL)
				|| (tTvwMember.GetConfIdx() != m_byConfIdx &&  tTvwMember.GetConfIdx() != 0 ))
			{
				bClearTwPollStatus = TRUE;
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Tvw.(%d, %d) is not at polling or not belong to this conf\n", byTWId, byChnId);
				break;
			}
			
			TMtPollParam *ptCurPollMt = NULL;
			
			if ( 0 == byIsStartAsPause)   
			{
				byCurPollPos++;
			}
			else // ����ͣ�ָ�ʱ�����ŵ�ǰ�ն���ѯ   
			{
				m_tTWMutiPollParam.SetIsStartAsPause(byTWId, byChnId, 0);
			}
			
			ptCurPollMt = GetMtTWPollParam(byCurPollPos, byTWId, byChnId);
			
			BOOL32 bStopPoll = FALSE;
			// [2013/01/29 chenbing] 
// 			// �ж����ǻ����Ƿ�ֹͣ��ѯ
// 			if (m_tConf.GetConfAttrb().IsSatDCastMode())
// 			{
// 				TMt tLastPollMt = m_tTWMutiPollParam.GetCurPolledMt(byTWId, byChnId);
// 				if (!tLastPollMt.IsNull() &&
// 					IsMtSendingVidToOthers(tLastPollMt, FALSE, TRUE, 0) && //��һ���ն˻���������ʵ�巢������
// 					m_ptMtTable->GetMtSndBitrate(ptCurPollMt->GetMtId()) == 0 && //��һ���ն�û���ڷ�������
// 					IsOverSatCastChnnlNum(0))
// 				{
// 					bStopPoll = TRUE;
// 					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "SatDCastMode Tvw.(%d, %d) poll failed\n", byTWId, byChnId);
// 				}
// 			}
			
			if (NULL != ptCurPollMt && !bStopPoll && !ptCurPollMt->IsNull())
			{
				m_tTWMutiPollParam.SetCurPolledMt(byTWId, byChnId, *ptCurPollMt);
				m_tTWMutiPollParam.SetCurrentIdx(byTWId, byChnId, byCurPollPos);
				
				u32 dwSetTimer = ptCurPollMt->GetKeepTime();
				TMt tSrcOrg  = *ptCurPollMt;
				TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);
				
				TTvWallPollParam tTWPollParam;
				m_tTWMutiPollParam.GetTWPollParam(byTWId, byChnId, tTWPollParam);
				cServMsg.SetMsgBody( (u8*)&tTWPollParam, sizeof(TTvWallPollParam) );
				
				// [9/1/2011 liuxu] VCS����ʱ���ش�������ϴ�ͨ����ռ��,���ܽ�ǽ
				BOOL32 bCanInTvw = TRUE;
				if ( VCS_CONF == m_tConf.GetConfSource() )
				{
					//���¼��е�ǰ�����նˣ���ͬһ���¼��������ն��������ǽ����NACK
					if( !tSrcOrg.IsLocal() 
						&& !IsLocalAndSMcuSupMultSpy(tSrcOrg.GetMcuId()) 
						&& IsMtInMcu(GetLocalMtFromOtherMcuMt(tSrcOrg), m_cVCSConfStatus.GetCurVCMT()) 
						&& !( m_cVCSConfStatus.GetCurVCMT() == tSrcOrg ))
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  
							"[ProcMcsMcuHduPollMsg] CurVCMT(%d.%d) and Mt(%d.%d) is in same smcu,Mt can't in tvwall \n ",
							m_cVCSConfStatus.GetCurVCMT().GetMcuId(),
							m_cVCSConfStatus.GetCurVCMT().GetMtId(),
							tSrcOrg.GetMcuId(),
							tSrcOrg.GetMtId() );
						
						bCanInTvw = FALSE;							
						
						cServMsg.SetEventId( MCU_MCS_ALARMINFO_NOTIF );
						cServMsg.SetErrorCode( ERR_MCU_CONFSNDBANDWIDTHISFULL );	
						SendMsgToAllMcs( MCU_MCS_ALARMINFO_NOTIF, cServMsg );
					}	
				}
				
				if(bCanInTvw)
				{
					if (byTWId >= TVWALLID_MIN && byTWId <= TVWALLID_MAX)
					{		
						SendMsgToAllMcs( MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg );
						ChangeTvWallSwitch(&tSrcOrg, byTWId, byChnId, TW_MEMBERTYPE_TWPOLL, TW_STATE_CHANGE);      
					}
					else if(byTWId >= HDUID_MIN && byTWId <= HDUID_MAX)
					{
						SendMsgToAllMcs( MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg );
						// [2013/03/11 chenbing] HDU�໭��Ŀǰ��֧��������ѯ,��ͨ����0
						ChangeHduSwitch(&tSrcOrg, byTWId, byChnId, 0, TW_MEMBERTYPE_TWPOLL, TW_STATE_START, byMode);    
					}
					else
					{
						ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcHduPollingChangeTimerMsg] UnKnow TWId:%d!\n", byTWId);
					}
				}else  // �Ѿɵ����ǽ
				{
					if (byTWId >= TVWALLID_MIN && byTWId <= TVWALLID_MAX)
					{		
						SendMsgToAllMcs( MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg );
						ChangeTvWallSwitch(NULL, byTWId, byChnId, TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);      
					}
					else if(byTWId >= HDUID_MIN && byTWId <= HDUID_MAX)
					{
						SendMsgToAllMcs( MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg );
						// [2013/03/11 chenbing] HDU�໭��Ŀǰ��֧��������ѯ,��ͨ����0
						ChangeHduSwitch(NULL, byTWId, byChnId, 0, TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP, byMode);      
					}
					else
					{
						ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcHduPollingChangeTimerMsg] UnKnow TWId:%d!\n", byTWId);
					}
				}

				SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + byTimerIdx, 1000 * dwSetTimer, byTimerIdx);
				
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_EQP, "TvwPoll: new mt(%d,%d) at poll, remain %u\n", 
					tSrcOrg.GetMcuId(),tSrcOrg.GetMtId(), ptCurPollMt->GetPollNum());
			}
			else
			{
				bClearTwPollStatus = TRUE;
			}
		}
		break;
			
		default:
			break;
	}
	
	if ( bClearTwPollStatus )
	{
		TMt tTWMem = m_tTWMutiPollParam.GetCurPolledMt(byTWId, byChnId);
		
		TMtPollParam tMtPollParam;
		memset(&tMtPollParam, 0, sizeof(tMtPollParam));
		m_tTWMutiPollParam.SetCurPolledMt(byTWId, byChnId, tMtPollParam);
		m_tTWMutiPollParam.SetPollState(byTWId, byChnId, POLL_STATE_NONE);
		
		TTvWallPollParam tTWPollParam;
		m_tTWMutiPollParam.GetTWPollParam(byTWId, byChnId, tTWPollParam);
		
		cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
		
		
		if (byTWId >= TVWALLID_MIN && byTWId <= TVWALLID_MAX)
		{
			SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
			ChangeTvWallSwitch(&tTWMem, byTWId, byChnId, TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);
		}
		else if(byTWId >= HDUID_MIN && byTWId <= HDUID_MAX)
		{
			SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
			// [2013/03/11 chenbing] HDU�໭��Ŀǰ��֧��������ѯ,��ͨ����0
			ChangeHduSwitch(NULL, byTWId, byChnId, 0, TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);
		}
		
		KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + byTimerIdx);
		
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcTWPollingChangeTimerMsg] tvwall[%d,%d] polling finished\n", byTWId, byChnId);
	}

	return;
}

/*====================================================================
    ������      SendConfExtInfoToMcs
    ����        ��������չ��Ϣ֪ͨ����(�¼������Ƿ�֧�ֶ�ش�)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 bySrcSsnId mcu��mcu�ĻỰ�ţ��������Ǹ�mcs��
	              0��Ĭ�Ϸ������л��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    20100517    4.6         pengjie        create
====================================================================*/
void CMcuVcInst::SendConfExtInfoToMcs( u8 bySrcSsnId/*=0*/,u8 byMtId/*=0*/ )
{
	CServMsg cServMsg;

	cServMsg.SetConfIdx( m_byConfIdx );
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetMcuId( LOCAL_MCUID );
	cServMsg.SetEventId( MCU_MCS_CONFEXTINFO_NOTIF );

	u8 byIsSupMultSpy = FALSE;
	TMt tMt;

	if( 0 != byMtId )
	{
		if( m_ptMtTable->GetMtType(byMtId) == MT_TYPE_SMCU )
		{
			tMt = m_ptMtTable->GetMt( byMtId );
			if( IsLocalAndSMcuSupMultSpy( GetMcuIdxFromMcuId(tMt.GetMtId()) ) )
			{			
				byIsSupMultSpy = 1;	
				cServMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
				cServMsg.CatMsgBody( (u8 *)&byIsSupMultSpy, sizeof(u8) );
				if( bySrcSsnId == 0 )
				{
					SendMsgToAllMcs( MCU_MCS_CONFEXTINFO_NOTIF, cServMsg );
				}
				else
				{
					cServMsg.SetSrcSsnId( bySrcSsnId );
					SendMsgToMcs( bySrcSsnId, MCU_MCS_CONFEXTINFO_NOTIF, cServMsg );
				}
			}
		}
	}
	else
	{
		for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
			{			
				if( m_ptMtTable->GetMtType(byLoop) == MT_TYPE_SMCU )
				{				
					tMt = m_ptMtTable->GetMt( byLoop );
					if( IsLocalAndSMcuSupMultSpy( GetMcuIdxFromMcuId(tMt.GetMtId()) ) )
					{			
						byIsSupMultSpy = 1;	
						cServMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
						cServMsg.CatMsgBody( (u8 *)&byIsSupMultSpy, sizeof(u8) );
						if( bySrcSsnId == 0 )
						{
							SendMsgToAllMcs( MCU_MCS_CONFEXTINFO_NOTIF, cServMsg );
						}
						else
						{
							cServMsg.SetSrcSsnId( bySrcSsnId );
							SendMsgToMcs( bySrcSsnId, MCU_MCS_CONFEXTINFO_NOTIF, cServMsg );
						}
					}					
				}
			}
		}
	}
	
	return;
}

/*====================================================================
    ������      ProcCancelSpyMtMsg
    ����        ������ȡ���ش�ͨ���ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/04/15    4.0         �ܾ���         ����
====================================================================*/
void CMcuVcInst::ProcCancelSpyMtMsg( const CMessage * pcMsg )
{
	STATECHECK;

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	if( m_tCascadeMMCU.IsNull() )
	{
		SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcCancelSpyMtMsg] m_tCascadeMMCU is null.so can't Cancel SpyMt.\n" );
        return;
	}

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	// �ͷŻش�ͨ��
	TMtStatus tMtStatus;
	m_ptMtTable->GetMtStatus(m_tCascadeMMCU.GetMtId(), &tMtStatus);
	FreeRecvSpy( tMtStatus.GetVideoMt(), MODE_BOTH );
    
	StopSpyMtCascaseSwitch();
}

/*====================================================================
    ������      ��ProcMcsMcuRollCallMsg
    ����        �������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/11/07    4.0         �ű���         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuRollCallMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
    STATECHECK;

    switch(cServMsg.GetEventId())
    {
    case MCS_MCU_STARTROLLCALL_REQ:
        {
            u8 byRollCallMode = *(u8*)cServMsg.GetMsgBody();
            if ( byRollCallMode == ROLLCALL_MODE_NONE ||
                 byRollCallMode >  ROLLCALL_MODE_CALLEE )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_MODE);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] unexpected Rollcall mode.%d\n", byRollCallMode);
                return;
            }

            if ( m_tConf.m_tStatus.IsMustSeeSpeaker() )
            { 
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_MUSTSEESPEAKER);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] can't start rollcall due to conf mode.mustseespeaker\n");
                return;
            }

            // zbq[11/24/2007] ������ѯ��ʱ������������
            if ( m_tConf.m_tStatus.GetPollMode() != CONF_POLLMODE_NONE )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_POLL);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] can't start rollcall due to conf pollmode.%d\n", m_tConf.m_tStatus.GetPollMode());
                return;
            }

            // zbq[03/31/2008] ������ԴԤ��(FIXME��δռ�ã����ܻᱻ��)
            u8 byGrpId = 0;
            u8 byEqpId = 0;
            if ( // �µ����߼�֧�ֱ������ڳ����ģʽ��������������жϱ�����
				// �Ƿ�ռ���˻�����, zgc, 2008-05-22
				m_tConf.m_tStatus.IsNoMixing() &&
				!m_tConf.m_tStatus.IsVACing() )
            {
				TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];
				memset(atAudioTypeDesc, 0, sizeof(atAudioTypeDesc));
				u8 byAudioCapNum = m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);

				byEqpId = g_cMcuVcApp.GetIdleMixer(0, byAudioCapNum, atAudioTypeDesc);
                if (0 == byEqpId)
				{
					cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_NOMIXER);
					SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg]GetIdleMixer() can't find idle mixer!\n");
					return;
				}
				else
				{
					//tianzhiyong 2010/04/15  ���û������Ƿ����
					if (!CheckMixerIsValide(byEqpId))
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg]CheckMixerIsValide() mixer(%d) is invalide!\n",byEqpId);
						cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_NOMIXER);
						SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
						return;
					}
				}
            }

            u8 byIdleVMPNum = 0;
            u8 abyIdleVMPId[MAXNUM_PERIEQP];
            memset( abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
            g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );    
			// BUG10614�������������ܵ�, zgc, 2008-04-28
			// ��VMPģʽ����Ҫ�ж��Ƿ���VMP����
            // �µ����߼�֧�ֱ��������л��ϳ�ģʽ��������������ж��Ƿ��Ǳ�����ռ���˻���
            // �ϳ�����zgc, 2008-05-23
			u8 byVmpCount = GetVmpCountInVmpList();
            if ( byIdleVMPNum == 0 && byRollCallMode == ROLLCALL_MODE_VMP &&
                0 == byVmpCount )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_NOVMP);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] can't start rollcall due to no idle vmp\n");
                return;
            }
            // zgc, 2008-05-29, ���VMP��������������������涼�޷�֧�֣���ܾ�����
            if ( byRollCallMode == ROLLCALL_MODE_VMP && 0 == byVmpCount)
            {
                u16 wError = 0;
                u8 byVmpId = 0;
                BOOL32 bRet = IsMCUSupportVmpStyle(VMP_STYLE_VTWO, byVmpId, EQP_TYPE_VMP, wError);
                if ( !bRet )
                {
                    cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_VMPABILITY);
                    SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] can't start rollcall due to vmp ability is not enough!\n");
                    return;
                }
            }

            SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

            //���ƻ��������ܻ�������������������ϳɣ��Զ��ͷ��Զ�����ѡ����������ʾ
            if ( m_tConf.m_tStatus.IsSpecMixing() )
            {
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_SPECMIXING);
            }
            if ( m_tConf.m_tStatus.IsAutoMixing() )
            {
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_AUTOMIXING);
            }
            if ( m_tConf.m_tStatus.IsVACing() )
            {
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_VACING);
            }
			// BUG10614�������������ܵ�, zgc, 2008-04-28
			// ��VMPģʽ����Ҫ��ʾ
            // �޸ı����ʾ, zgc, 2008-05-23
            if ( //m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_CTRL 
                0 < byVmpCount &&
				ROLLCALL_MODE_VMP == byRollCallMode )
            {
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_VMPSTYLE);
            }
            //else if ( m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_AUTO
			//		&& byRollCallMode == ROLLCALL_MODE_VMP )
            //{
            //    NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_VMPSTYLE);
            //}

            // �������ںϳɣ�����ģʽ����VMPģʽʱ����Ҫ��ʾ��ͣ�ϳ�, zgc, 2008-05-23
            // �޸ı����ʾ, zgc, 2008-05-23
            if ( 0 < byVmpCount &&
                 ROLLCALL_MODE_VMP != byRollCallMode )
            {
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_VMPING );
            }
			
            BOOL32 bSelExist = FALSE;
            for( u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++ )
            {
                if ( m_tConfAllMtInfo.MtJoinedConf( byMtIdx ) ) 
                {
					// Ŀǰ���ԣ��ϴ��ն˹����䣬�����ϴ�ͨ��Ϊ�ϼ�MCU��ѡ��Դ
					// miaoqingsong [20110916] ���˵�ѡ��ԴΪ�ϴ�ͨ�����ն˵�Ĭ��ѡ��
					if (!m_tCascadeMMCU.IsNull() && 
						m_tCascadeMMCU.GetMtId() == byMtIdx)
					{
						continue;
					}

                    TMtStatus tMtStatus;
                    if ( m_ptMtTable->GetMtStatus( byMtIdx, &tMtStatus ) &&
                         ( !tMtStatus.GetSelectMt(MODE_AUDIO).IsNull() ||
                           !tMtStatus.GetSelectMt(MODE_VIDEO).IsNull() ) )
                    {
                        bSelExist = TRUE;
                        break;
                    }
                }
            }
            if ( bSelExist )
            {
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_SEL);
            }

            // zbq [11/22/2007] ������ǰ��������е��¼�����
            CServMsg cSMsg;
            CMessage cMsg;
            BOOL32 bSMcuExist = FALSE;
            for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
            {
                //zbq[01/09/2009] �ϲ������������������¼�
                if ( m_tConf.GetConfAttrb().IsSupportCascade() &&
                     m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
                     m_ptMtTable->GetMt(byMtId).GetMtType() == MT_TYPE_SMCU )
                {
                    bSMcuExist = TRUE;
                    break;
                }
            }
            if ( bSMcuExist )
            {
                TMcu tMcu;
                cSMsg.SetNoSrc();
                cSMsg.SetEventId(MCS_MCU_LOCKSMCU_REQ);
                cSMsg.SetMsgBody((u8*)&tMcu, sizeof(tMcu));
                
                cMsg.content = cSMsg.GetServMsg();
                cMsg.length = cSMsg.GetServMsgLen();
                ProcMcsMcuLockSMcuReq(&cMsg);                
            }

			if( CONF_LOCKMODE_LOCK != m_tConf.m_tStatus.GetProtectMode() )
			{
				u8 byLockMode = CONF_LOCKMODE_LOCK;
				cSMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);
				cSMsg.SetMsgBody((u8*)&byLockMode, sizeof(u8));
				ChangeConfLockMode(cSMsg);
				m_tConf.m_tStatus.m_tConfMode.SetOldLockMode( CONF_LOCKMODE_NONE );
			}
			else
			{
				m_tConf.m_tStatus.m_tConfMode.SetOldLockMode( CONF_LOCKMODE_LOCK );
			}

            

            // zbq [11/22/2007] �õ�ǰ����Ϊ����ϯ����
            m_tConf.m_tStatus.SetNoChairMode( TRUE );
            if( m_tConf.m_tStatus.IsNoChairMode() && HasJoinedChairman() )
            {
                ChangeChairman( NULL );	
            }

            m_tConf.m_tStatus.SetRollCallMode( byRollCallMode );
            
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] RollCallMode.%d\n", byRollCallMode);

            cServMsg.SetEventId( MCU_MCS_STARTROLLCALL_NOTIF );
            SendMsgToAllMcs( cServMsg.GetEventId(), cServMsg );
        }
    	break;

    case MCS_MCU_CHANGEROLLCALL_REQ:
        {
            if ( m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_NONE ||
                 m_tConf.m_tStatus.GetRollCallMode() >  ROLLCALL_MODE_CALLEE )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_MODE);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] Rollcall mode.%d, can't change caller or callee\n", m_tConf.m_tStatus.GetRollCallMode());
                return;
            }
            if ( cServMsg.GetMsgBodyLen() != sizeof(TMt)*2 )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_MSGLEN);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] unexpected msglen.%d, need.%d\n", cServMsg.GetMsgBodyLen(), sizeof(TMt)*2 );
                return;
            }

			//zhouyiliang 20110115 �������뱸������ģʽ�������л��������˵�ʱ�򣬻���ϳ�æ��ʱ��nack
			TPeriEqpStatus tPeriEqpStatus;
			if ( !IsLastMutiSpyRollCallFinished() ) 
			{
				// ��Ӵ�ӡ
				m_tConfInStatus.Print();

				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuRollCallMsg]LastRollCall Oprator not finished!\n");
				cServMsg.SetErrorCode(ERR_MCU_LASTROLLCALL_NOTFINISH);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                return;
				
			}

            TMt tNewRollCaller = *(TMt*)cServMsg.GetMsgBody();
            TMt tNewRollCallee = *(TMt*)(cServMsg.GetMsgBody() + sizeof(TMt));

            if ( tNewRollCaller.IsNull() )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_CALLERNULL);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] Caller Null, ignore it\n");
                return;
            }

            if ( tNewRollCallee.IsNull() )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_CALLEENULL);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] Callee Null, ignore it\n");
                return;
            }

            BOOL32 bCallerChged = tNewRollCaller == m_tRollCaller ? FALSE : TRUE;
            BOOL32 bCalleeChged = tNewRollCallee == m_tRollCallee ? FALSE : TRUE;

            if ( bCallerChged && !tNewRollCaller.IsLocal() )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_CALLER_SMCUMT);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] Mt<%d,%d> can't be caller, it belong to SMcu\n", 
                        tNewRollCaller.GetMcuId(), tNewRollCaller.GetMtId() );
                return;
            }

            SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcsMcuRollCallMsg] bCallerChged(%d) bCalleeChged(%d) StopSelectSrc bCallerChged\n",
							bCallerChged,bCalleeChged );

            TMt tOldRollCaller = m_tRollCaller;
            TMt tOldRollCallee = m_tRollCallee;

            m_tRollCaller = tNewRollCaller;
            m_tRollCallee = tNewRollCallee;

			u8 byVmpCount = GetVmpCountInVmpList();
			u8 byVmpId = GetTheMainVmpIdFromVmpList();
			TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
			TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			// miaoqingsong [20110909] ��������ʱ����ͣ��ǰ�����˵�������Ƶѡ������Ƶѡ���ڵ���������ͣ
// 			if ( tOldRollCaller.IsNull() || tOldRollCallee.IsNull() )
// 			{
// 				TMtStatus tMtStatus;
// 				m_ptMtTable->GetMtStatus(tNewRollCaller.GetMtId(), &tMtStatus);
// 				
// 				if ( !tMtStatus.GetSelectMt(MODE_VIDEO).IsNull() )
// 				{
// 					StopSelectSrc(tNewRollCaller, MODE_VIDEO, FALSE, FALSE);
// 				}
// 			}

            if ( !bCallerChged && !bCalleeChged )
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] there is no change to caller and callee both\n");
                return;
            }

            // �������byVMPStyle��¼��ǰ����ϳɵķ��
			u8 byVMPStyle = tConfVmpParam.GetVMPStyle();

            // ��VMP����ģʽ����Ҫͣ�ϳ�, zgc, 2008-05-23
            if ( ROLLCALL_MODE_VMP != m_tConf.m_tStatus.GetRollCallMode() )
            {
				if (byVmpCount > 0)
				{
					//ֹͣʱģ��ʧЧ
					TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
					if( tConfAttrb.IsHasVmpModule() )
					{
						TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
						tConfAttrib.SetHasVmpModule(FALSE);
						m_tConf.SetConfAttrb( tConfAttrib );
					}
					
					// MCU8000B:�ڹػ���ϳ�֮ǰ��Ҫ��ʮ������ϳɷ�����������ϳɷ���Ա�֧�ַ�VMP����ģʽ�ܹ���ȷ�ؿ�������
					// 20110426_miaoqingsong Bug00048137 MCU8000B����ʮ������ϳ�������з�VMP����ģʽ���ܿ�����������
#ifdef _MINIMCU_
					if ( VMP_STYLE_SIXTEEN == byVMPStyle )
					{
						tConfVmpParam.SetVMPStyle(VMP_STYLE_VTWO);
						NotifyMcsAlarmInfo( 0, ERR_MCU_ROLLCALL_VMPSTYLE );
					}
#endif
					TEqp tTmpVmpEqp;
					for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
					{
						if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
						{
							continue;
						}
						tTmpVmpEqp = g_cMcuVcApp.GetEqp( m_abyVmpEqpId[byIdx] );
						
						if (!tTmpVmpEqp.IsNull() &&
							g_cMcuVcApp.GetPeriEqpStatus(tTmpVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
							tPeriEqpStatus.GetConfIdx() == m_byConfIdx)
						{
							// ֱ��ֹͣ
							g_cEqpSsnApp.SendMsgToPeriEqpSsn( tTmpVmpEqp.GetEqpId(),MCU_VMP_STOPVIDMIX_REQ, 
								(u8*)cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
							ProcVMPStopSucRsp(m_abyVmpEqpId[byIdx]);
						}
					}
					/*TPeriEqpStatus tPeriEqpStatus;
					if (!tVmpEqp.IsNull() &&
						g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
						tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
						tPeriEqpStatus.m_tStatus.tVmp.m_byUseState != TVmpStatus::WAIT_STOP)
					{
						SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, TIMESPACE_WAIT_VMPRSP);
						tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
						g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tPeriEqpStatus);
						g_cEqpSsnApp.SendMsgToPeriEqpSsn( m_tVmpEqp.GetEqpId(),MCU_VMP_STOPVIDMIX_REQ, 
							(u8*)cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
						
						
					}*/
				}
				else //�޻���ϳ�ʱ����m_tVmpEqp����ֹ����vmp�����ߺ�ָ�(Bug00127809)
				{
					m_tVmpEqp.SetNull();
				}
            }

            // zbq [11/23/2007] �������˻򱻵����˷�����λ��
            if ( bCallerChged &&
                 ROLLCALL_MODE_CALLEE != m_tConf.m_tStatus.GetRollCallMode() )
            {
				// KDV-BUG2004: �����˻򱻵����˱�ǿ��Ϊ�����ˣ�����MTC���ն�״̬δ�ı�
				// zgc, 2008-05-21, ��ChangeSpeaker��������
                //m_tConf.SetSpeaker( tNewRollCaller );
				ChangeSpeaker( &tNewRollCaller,FALSE,FALSE,TRUE );
            }
            
            //֪ͨ���л��
			// KDV-BUG2004: �����˻򱻵����˱�ǿ��Ϊ�����ˣ�����MTC���ն�״̬δ�ı�
			// zgc, 2008-05-21, ��ChangeSpeaker�����д���
            //cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
            //SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

			//  [12/19/2009 pengjie] Modify ������ش�֧�֣����Ҫ�ı䱻�����ˣ��������¼�֧�ֶ�ش���MT����Presetin�������ֿ��Ƿ��ܹ���ش�
			if( (bCalleeChged || (bCallerChged && ROLLCALL_MODE_CALLEE != m_tConf.m_tStatus.GetRollCallMode())) && 
				!tNewRollCallee.IsNull() && !tNewRollCallee.IsLocal()  )
			{
				if( IsLocalAndSMcuSupMultSpy(tNewRollCallee.GetMcuId()) )
				{
					TMt tDstMt;
					tDstMt.SetNull();
					TPreSetInReq tSpySrcInitInfo;
					tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt( tNewRollCallee );
					tSpySrcInitInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tCaller = tNewRollCaller;
					tSpySrcInitInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tOldCaller = tOldRollCaller;
					tSpySrcInitInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tOldCallee = tOldRollCallee;
					tSpySrcInitInfo.m_bySpyMode = MODE_VIDEO;  // ����������ѡ������ʵֻ����Ƶ��ѡ��
					if( ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
					{
						tSpySrcInitInfo.m_bySpyMode = MODE_BOTH;  // ����������ѡ������ʵֻ����Ƶ��ѡ��
					}
					TMcsRegInfo tMcRegInfo;
					if (g_cMcuVcApp.GetMcsRegInfo(cServMsg.GetSrcSsnId(), &tMcRegInfo))
					{
						tSpySrcInitInfo.m_tSpyInfo.m_tSpyRollCallInfo.SetMcIp(tMcRegInfo.GetMcsIpAddr());
						tSpySrcInitInfo.m_tSpyInfo.m_tSpyRollCallInfo.SetMcSSRC(tMcRegInfo.GetMcsSSRC());
					}	
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuRollCallMsg] McsIp.%s, McSSrc.%x!\n", 
												 StrOfIP(tSpySrcInitInfo.m_tSpyInfo.m_tSpyRollCallInfo.GetMcIp()),
												 tSpySrcInitInfo.m_tSpyInfo.m_tSpyRollCallInfo.GetMcSSRC());
					
					tSpySrcInitInfo.SetEvId(cServMsg.GetEventId());

					// [pengjie 2010/9/13] ��Ŀ�Ķ�����
					TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tNewRollCallee.GetMcuId()) );
					//zjl20101116 �����ǰ�ն��ѻش���������Ҫ���ѻش�Ŀ��������ȡС					
					if(!GetMinSpyDstCapSet(tNewRollCallee, tSimCapSet))
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcsMcuRollCallMsg] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
							tNewRollCallee.GetMcuId(), tNewRollCallee.GetMtId() );
						return;
					}
					
					tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		            // End

					if( !tOldRollCallee.IsNull() )
					{			
						tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tOldRollCallee;
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_VIDEO;
						tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
						//20120821 zhouyiliang �����뱻������ģʽ�»���ROLLCALL_MODE_CALLERģʽ�£�ѡ���ɹ��ż�����Ƶ
						if (ROLLCALL_MODE_VMP ==  m_tConf.m_tStatus.GetRollCallMode() 
							|| ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() )
						{
							TMtStatus tStatus ;
							if ( m_ptMtTable->GetMtStatus(m_tRollCaller.GetMtId(),&tStatus) )
							{
								//���ѡ�����ɹ�Ҫ������������Ҫbas�����ɹ��������
								if ( !(tStatus.GetSelectMt(MODE_VIDEO) == tOldRollCallee) )
								{
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum--;
								}
							}
							else
							{
								StaticLog("[ProcMcsMcuRollCallMsg]get m_tRollCaller mtstatus failed!\n");
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum--;
							}
						}
						
						//��1·���
						TMt tMcSrc;
						g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcSrc, MCS_ROLLCALL_MCCHLIDX , MODE_VIDEO );
						if( tMcSrc == tOldRollCallee )
						{
							++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
						}
						
						/*	
						for( u8 byChanId = 0;byChanId < MAXNUM_MC_CHANNL;++byChanId )
						{
							g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcSrc, byChanId , MODE_VIDEO );
							if( tMcSrc == tOldRollCallee )
							{
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
							}
							g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcSrc, byChanId , MODE_AUDIO );
							if( tMcSrc == tOldRollCallee )
							{
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
							}
						}*/

						u8 byChlPos = 0;
						if( m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP )
						{
							/*TMtStatus tStatus;
							m_ptMtTable->GetMtStatus( tNewRollCaller.GetMtId(),&tStatus );
							if( tStatus.GetSelectMt(MODE_VIDEO) == tOldRollCallee )
							{
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
							}*/
							byChlPos = tConfVmpParam.GetChlOfMtInMember(tOldRollCallee);
							if(	tConfVmpParam.GetVMPMode() == CONF_VMPMODE_CTRL  &&
								MAXNUM_VMP_MEMBER != byChlPos )
							{
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
							}

						}

						

						//���ʱ��������ģʽ,���������Ƿ�����,�ѷ����˸���Ŀ����Զ����ǽ�ȥ
						//������������ڷ����˸����Զ������˻���ϳ�,���ͷ���ƵĿ����Ҫ��1						
						if( ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() &&
							m_tConf.GetSpeaker() == tOldRollCallee
							)
						{							
							byChlPos = tConfVmpParam.GetChlOfMtInMember(tOldRollCallee);
							if(	tConfVmpParam.GetVMPMode() == CONF_VMPMODE_AUTO &&
								MAXNUM_VMP_MEMBER != byChlPos &&				
								tConfVmpParam.GetVmpMember(byChlPos)->GetMemberType() == VMP_MEMBERTYPE_SPEAKER					
								)
							{
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
							}
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_BOTH;
							++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;							


							TPeriEqpStatus tStatus;
							u8 byChnlIdx;
							u8 byEqpId;
							//������������ڷ����˸����Զ������˱������ǽ,���ͷ�����ƵĿ����Ҫ��1
							for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
							{
								if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
								{
									if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus))
									{						
										for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
										{							
											if (TW_MEMBERTYPE_SPEAKER == tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType &&
												tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx() == m_byConfIdx &&
												tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetMcuId() == tOldRollCallee.GetMcuId() &&
												tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetMtId() == tOldRollCallee.GetMtId()
												)
											{
												++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
												++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
											}
										}
									}
								}
							}

							//�����˸���ʱ��ͬ������HDU�е�ͼ��
							u8 byHduChnNum = 0;
							//������������ڷ����˸����Զ������˱������ǽ,���ͷ�����ƵĿ����Ҫ��1
							for (byEqpId = HDUID_MIN; byEqpId <= HDUID_MAX; byEqpId++)
							{
								if(IsValidHduEqp(g_cMcuVcApp.GetEqp(byEqpId)))
								{
									byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byEqpId));
									if (0 == byHduChnNum)
									{						
										continue;
									}

									if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus))
									{						
										for (byChnlIdx = 0; byChnlIdx < byHduChnNum; byChnlIdx++)
										{							
											if (TW_MEMBERTYPE_SPEAKER == tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType &&
												m_byConfIdx == tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx() &&
												tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetMcuId() == tOldRollCallee.GetMcuId() &&
												tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetMtId() == tOldRollCallee.GetMtId()
												)
											{
												++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
												++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
											}
										}
									}
								}
							}
						}

						
					}
		
					OnMMcuPreSetIn( tSpySrcInitInfo );  	
					//zhouyiliang 20110118 �����¼��նˣ����¼���ack��nack����û�true���ڼ䲻����
					//���л��������ˣ���ֹʱ������
					if (bCalleeChged)
					{
						StaticLog("[ProcMcsMcuRollCallMsg]Start wait lastrollcall fininsh \n") ;
						SetLastMultiSpyRollCallFinish(FALSE);
					}
					
					
					return;
				}
			}

			if ( bCalleeChged &&
                 ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
            {
				// KDV-BUG2004: �����˻򱻵����˱�ǿ��Ϊ�����ˣ�����MTC���ն�״̬δ�ı�
				// zgc, 2008-05-21, ��ChangeSpeaker��������
                //m_tConf.SetSpeaker( tNewRollCallee );
				ChangeSpeaker( &tNewRollCallee,FALSE,FALSE,TRUE );
            }

            // ͣ��������
            if ( m_tConf.m_tStatus.IsVACing() )
            {
                CServMsg cStopVacMsg;
                cStopVacMsg.SetEventId(MCS_MCU_STOPVAC_REQ);
                MixerVACReq(cStopVacMsg);
            }
			
			//��������
            BOOL32 bLocalMixStarted = FALSE;

			if ( m_tConf.m_tStatus.GetMixerMode() != mcuNoMix)
			{	
				BOOL32 bRemoveMixMember = FALSE;
				BOOL32 bLocalAutoMix = FALSE;
				
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
				
				if (bCallerChged && !m_tMixEqp.IsNull())
				{
					if ( !tOldRollCaller.IsNull() )
                    {
						RemoveSpecMixMember(&tOldRollCaller, 1, FALSE, FALSE);
					}

					AddSpecMixMember(&tNewRollCaller, 1, FALSE);
				}

				if (bCalleeChged && !m_tMixEqp.IsNull())
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
					// 					for (u8 byMtId = 1; byMtId < MAXNUM_CONF_MT; byMtId++)
					// 					{
					// 						if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
					// 						{
					// 							TMt tMt = m_ptMtTable->GetMt(byMtId);
					// 							if (m_tConf.m_tStatus.IsSpecMixing() && !m_ptMtTable->IsMtInMixing(byMtId))
					// 							{
					// 								SwitchMixMember(&tMt, TRUE);
					// 							}
					// 						}           
					// 					}
					
					if( m_tConf.m_tStatus.IsSpecMixing() )
					{
						SwitchMixMember( TRUE );
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

				// MCU8000B:��������������������֮ǰʮ������ϳɷ��������������ָ�Ϊԭ����ʮ������ϳɷ���Ա���ͣ����ϳɽ��������׵�����
				// 20110426_miaoqingsong Bug00048137 MCU8000B����ʮ������ϳ�������з�VMP����ģʽ���ܿ�����������
                #ifdef _MINIMCU_
					u8 byVMPStyle = m_tConf.m_tStatus.GetVmpStyle();
					if ( VMP_STYLE_SIXTEEN == byVMPStyle )
					{
						m_tConf.m_tStatus.SetVmpStyle(VMP_STYLE_SIXTEEN);
					}
                #endif
				}                    
            }
			
            
            //�����㲥����
            TVMPParam_25Mem tVmpParam;
            if ( m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP )
            {
				tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
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
				if (byVmpCount == 0)
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
					// �����˹㲥��ChangeSpeaker�д�������ֻҪ����ǲ��Ǳ��ؾͿ����ˣ�
					// ����Ŀǰ�߼�������Ĵ�ӡӦ���ǲ����ܳ��ֵģ�zgc, 2008-05-22
                    //if ( tNewRollCaller.IsLocal() )
                    //{
                    //    ChangeVidBrdSrc( &tNewRollCaller );
                    //}
					if ( !tNewRollCaller.IsLocal() )
					{
						ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "New roll caller(Mt.%d) is not local mt, it's impossible\n", tNewRollCaller.GetMtId() );
					}
                }
                else if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
                {
                    u8 byCallerPos = 0;
                    if ( !tOldRollCaller.IsNull() &&
                         tConfVmpParam.GetVMPMode() != CONF_VMPMODE_NONE )
                    {
						/*lint -save -esym(645,tVmpParam)*/
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
					// �������˹㲥��ChangeSpeaker�д�������ֻҪ���лش����ã�
					// zgc, 2008-05-22
                    if ( !tNewRollCallee.IsLocal() )
                    {
                        TMcu tSMcu;
                        tSMcu = GetLocalMtFromOtherMcuMt( tNewRollCallee );
                        OnMMcuSetIn(tNewRollCallee, cServMsg.GetSrcSsnId(), SWITCH_MODE_BROADCAST);
                    }
                    //ChangeVidBrdSrc( &tNewRollCallee );
                }
                else if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
                {
                    u8 byCalleePos = 1;
                    if ( !tOldRollCallee.IsNull() &&
                         tConfVmpParam.GetVMPMode() != CONF_VMPMODE_NONE )
                    {
                        byCalleePos = tVmpParam.GetChlOfMtInMember(GetLocalMtFromOtherMcuMt(tOldRollCallee));
                    }
                    
                    //�ɵ����˲����ڣ�ռ1ͨ��
                    if ( byCalleePos >= MAXNUM_VMP_MEMBER )
                    {
                        byCalleePos = 1;
                    }
                    TVMPMember tVmpMember;
                    memset(&tVmpMember, 0, sizeof(tVmpMember));
                    tVmpMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);
                    tVmpMember.SetMemberTMt(GetLocalMtFromOtherMcuMt(tNewRollCallee));
                    tVmpMember.SetMemStatus(MT_STATUS_AUDIENCE);
                    tVmpParam.SetVmpMember(byCalleePos, tVmpMember);
                }
            }
            if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
            {
                if ( CONF_VMPMODE_CTRL == tConfVmpParam.GetVMPMode() )
                {
                    //TPeriEqpStatus tPeriEqpStatus;
                    /*g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
                    tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
                    g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  */
//                     
                    //ChangeVmpParam(&tVmpParam);
					// xliang [1/6/2009] ��������VMP��VMP param
					AdjustVmpParam(tVmpEqp.GetEqpId(), &tVmpParam);

                }
                else if ( CONF_VMPMODE_AUTO == tConfVmpParam.GetVMPMode() )
                {
                    /*g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
                    tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
                    g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  */
                    
                    //ChangeVmpParam(&tVmpParam);
					// xliang [1/6/2009] ��������VMP��VMP param
					AdjustVmpParam(tVmpEqp.GetEqpId(), &tVmpParam);
					
                    ConfModeChange();
                }
                else
                {
					// �˴�������vmpid�����Լ��ҿ���vmp
                    CServMsg cMsg;
                    cMsg.SetEventId(MCS_MCU_STARTVMP_REQ);
                    cMsg.SetMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
					
                    VmpCommonReq(cMsg);
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
					// miaoqingsong [20110916] ���˵��ϼ�MCUѡ���ϴ�ͨ���ն˵�ѡ��
					if (!m_tCascadeMMCU.IsNull() && 
						m_tCascadeMMCU.GetMtId() == byMtId)
					{
						continue;
					} 
 
                    if ( m_tConfAllMtInfo.MtJoinedConf( byMtId ) &&
                         byMtId != tNewRollCaller.GetMtId() &&
                         byMtId != tNewRollCallee.GetMtId() ) 
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
                    if ( !tOldRollCaller.IsNull() )
                    {
                        StopSelectSrc(tOldRollCaller, MODE_VIDEO);   
						//zjl�л�һ�ε����ˣ����ǰһ�������˵��Ž���
						if (!(tOldRollCaller == GetSecVidBrdSrc()))
						{
							g_cMpManager.RemoveSwitchBridge(tOldRollCaller, 0, MODE_VIDEO);
						}
                    }
					//zyl��20121106��һ�ε�����bCallerChged��bCalleeChged��Ϊtrue,Ҫͣ������ǰ��ѡ��
					if (bCalleeChged)
					{
						StopSelectSrc(tNewRollCaller, MODE_VIDEO,FALSE,FALSE);  
					}

					//�����˸ı��ˣ���ͣ���������˵�ѡ�����ͷ���Դ
					if( tNewRollCallee.IsLocal() )
					{
						StopSelectSrc(tNewRollCallee, MODE_VIDEO,FALSE,FALSE); 
					}

					if( !tNewRollCallee.IsLocal() && !IsLocalAndSMcuSupMultSpy(tNewRollCallee.GetMcuIdx()))
					{
						OnMMcuSetIn( tNewRollCallee, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
					}
                    
                    //ѡ��ʧ�ܣ��ָ����㲥�����ܿ�vmp���Լ�
					//������ѡ����������
                    if (!ChangeSelectSrc( tNewRollCallee, tNewRollCaller, MODE_VIDEO))
                    {
                        RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
                    }
					else
					{
						if( !tNewRollCallee.IsLocal() && IsLocalAndSMcuSupMultSpy(tNewRollCallee.GetMcuId()) )
						{
							s16 swIndex = m_cSMcuSpyMana.FindSpyMt(tNewRollCallee);
							if( -1 != swIndex )
							{
								m_cSMcuSpyMana.IncSpyModeDst(swIndex, MODE_VIDEO, 1 );
							}
						}						
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
						StopSelectSrc(tNewRollCaller, MODE_VIDEO);  
					}
                    if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
                    {
                        RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
                    }
					else
					{
						if( !tNewRollCallee.IsLocal() && IsLocalAndSMcuSupMultSpy(tNewRollCallee.GetMcuId()) )
						{
							s16 swIndex = m_cSMcuSpyMana.FindSpyMt(tNewRollCallee);
							if( -1 != swIndex )
							{
								m_cSMcuSpyMana.IncSpyModeDst(swIndex, MODE_VIDEO, 1 );
							}
						}
					}
                }
                else // ROLLCALL_MODE_CALLEE
                {
					//zjj20091031
					if(  tNewRollCallee.IsLocal() )
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcsMcuRollCallMsg] bCallerChged(%d) bCalleeChged(%d) StopSelectSrc(tNewRollCallee)\n",
							bCallerChged,bCalleeChged );
						//20100708_tzy ����������Ϊ�¼��ն�ʱ������Ҫͣѡ������Ϊ��ʱ�¼����ն�ͨ���ϼ��㲥�տ���������
						if( tNewRollCallee.IsLocal() )
						{
							StopSelectSrc(tNewRollCallee, MODE_VIDEO,FALSE ,FALSE);
						}	
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
					if( bCallerChged &&
						!tOldRollCaller.IsNull() &&
						!tNewRollCaller.IsNull() 
						)
					{						
						StopSelectSrc(tNewRollCaller, MODE_VIDEO,FALSE,FALSE);
					}
					
					if( !bCallerChged )
					{
						StopSelectSrc(tNewRollCaller, MODE_VIDEO,FALSE,FALSE);
					}

                    if ( tOldRollCallee.IsLocal() &&
                        !tOldRollCallee.IsNull() )
                    {
                        StopSelectSrc(tOldRollCallee, MODE_VIDEO);
						//zjl�л�һ�α������ˣ����ǰһ���������˵��Ž���
						if (!(tOldRollCallee == GetSecVidBrdSrc()))
						{
							g_cMpManager.RemoveSwitchBridge(tOldRollCallee, 0, MODE_VIDEO);
						}
                    }
                    if ( !bSelAdjusted )
                    {
                        if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
                        {
                            RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
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
						StopSelectSrc(tOldRollCaller, MODE_VIDEO,FALSE ,FALSE);
					}

					if( !tNewRollCallee.IsLocal() )
					{						
						OnMMcuSetIn( tNewRollCallee, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
					}

					if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
                    {
                        RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
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

            cServMsg.SetMsgBody( (u8*)&m_tRollCaller, sizeof(TMt) );
            cServMsg.CatMsgBody( (u8*)&tNewRollCallee, sizeof(TMt) );
            cServMsg.SetEventId( MCU_MCS_CHANGEROLLCALL_NOTIF );
            SendMsgToAllMcs( cServMsg.GetEventId(), cServMsg );

            ConfStatusChange();
        }
    	break;

    case MCS_MCU_STOPROLLCALL_REQ:
        {
            if ( m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_NONE )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_MODE);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] RollCall mode.%d already\n", m_tConf.m_tStatus.GetRollCallMode());
                return;
            }
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
            RollCallStop( cServMsg );
        }
        break;

    default:
        break;
    }

    return;

	/*lint -restore*/
}

/*====================================================================
    ������      ��RollCallStop
    ����        ���������ֹͣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CServMsg &cServMsg
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    12/05/07    4.0         �ű���         ����
====================================================================*/
void CMcuVcInst::RollCallStop( CServMsg &cServMsg )
{
    //zbq [11/22/2007] �����������
	if( CONF_LOCKMODE_LOCK != m_tConf.m_tStatus.m_tConfMode.GetOldLockMode() )
	{
		CServMsg cSMsg;
		u8 byLockMode = CONF_LOCKMODE_NONE;
		cSMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);
		cSMsg.SetMsgBody((u8*)&byLockMode, sizeof(u8));
		ChangeConfLockMode(cSMsg);
		m_tConf.m_tStatus.m_tConfMode.SetOldLockMode( CONF_LOCKMODE_NONE );
	}
    
    //�ָ���ϯģʽ
    m_tConf.m_tStatus.SetNoChairMode( FALSE );
    
    //zbq[11/24/2007] ������������������������˺ͱ�������
    if ( !m_tRollCaller.IsNull() || !m_tRollCallee.IsNull() )
    {
        //ͣ����
        CServMsg cMsg;
        ProcMixStop(cMsg, TRUE);
        
        //ͣ�㲥
        if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
        {
            CServMsg cSendMsg;
            cSendMsg.SetConfIdx( m_byConfIdx );
            cSendMsg.SetConfId( m_tConf.GetConfId() );
            cSendMsg.SetEventId(MCS_MCU_STOPVMP_REQ);
            VmpCommonReq(cSendMsg);

			//�˳�VMPʱ����Ҫ��������˺ͱ������˵��ŵĽ���
            //g_cMpManager.RemoveSwitchBridge(m_tRollCaller, 0);

			//[2012/9/13 zhangli]���������������˫�㲥�򲻲���Ƶ�� 
			if (!(m_tRollCallee == GetSecVidBrdSrc()))
			{
				g_cMpManager.RemoveSwitchBridge(m_tRollCallee, 0);
			}
			g_cMpManager.RemoveSwitchBridge(m_tRollCallee, 0, MODE_AUDIO);
        }
        RestoreAllSubMtJoinedConfWatchingSrcMt(GetVidBrdSrc(), MODE_VIDEO);
        
        //ͣѡ��
        StopSelectSrc(m_tRollCaller, MODE_BOTH);
        if ( m_tRollCallee.IsLocal() )
        {
            StopSelectSrc(m_tRollCallee, MODE_BOTH);
        }

		/*if ( ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
		{
			FreeRecvSpy(m_tRollCallee, MODE_VIDEO);
		}*/
		TMt tMcSrc;
		if( 0 != cServMsg.GetSrcSsnId() )
		{
			g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcSrc, MCS_ROLLCALL_MCCHLIDX , MODE_VIDEO );
			if( tMcSrc == m_tRollCallee )
			{
				StopSwitchToMc( cServMsg.GetSrcSsnId(), MCS_ROLLCALL_MCCHLIDX, TRUE, MODE_VIDEO );
			}
		}
		else
		{
			for( u8 byMcId = 1;byMcId <= MAXNUM_MCU_MC;++byMcId )
			{
				g_cMcuVcApp.GetMcSrc( byMcId, &tMcSrc, MCS_ROLLCALL_MCCHLIDX , MODE_VIDEO );
				if( tMcSrc == m_tRollCallee )
				{				
					StopSwitchToMc( byMcId, MCS_ROLLCALL_MCCHLIDX, TRUE, MODE_VIDEO );
				}
				g_cMcuVcApp.GetMcSrc( byMcId, &tMcSrc, MCS_ROLLCALL_MCCHLIDX , MODE_AUDIO );
				if( tMcSrc == m_tRollCallee )
				{
					StopSwitchToMc( byMcId, MCS_ROLLCALL_MCCHLIDX, TRUE,  MODE_AUDIO );
				}				
			}
		}
		
		
		
		/*for( u8 byChanId = 0;byChanId < MAXNUM_MC_CHANNL;++byChanId )
		{
			g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcSrc, byChanId , MODE_VIDEO );
			if( tMcSrc == m_tRollCallee )
			{				
				StopSwitchToMc( cServMsg.GetSrcSsnId(), byChanId, TRUE, MODE_VIDEO );
			}
			g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcSrc, byChanId , MODE_AUDIO );
			if( tMcSrc == m_tRollCallee )
			{
				StopSwitchToMc( cServMsg.GetSrcSsnId(), byChanId, TRUE,  MODE_AUDIO );
			}
			
		}*/
        
        m_tRollCaller.SetNull();
        m_tRollCallee.SetNull();                
    }
    m_tConf.m_tStatus.SetRollCallMode(ROLLCALL_MODE_NONE);
    
    cServMsg.SetEventId( MCU_MCS_STOPROLLCALL_NOTIF );
    SendMsgToAllMcs( cServMsg.GetEventId(), cServMsg );

    return;
}

/*====================================================================
    ������      ��DaemonProcMcsMcuListAllConfReq
    ����        ����ѯ������Ϣ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/05/26    1.0         LI Yi         ����
    04/01/12    3.0         ������         �޸�
====================================================================*/
void CMcuVcInst::DaemonProcMcsMcuListAllConfReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	NtfMcsMcuCurListAllConf(cServMsg);

	return;
}

/*====================================================================
������      ��SendExtMcuStatus
����        �� ���Ͷ����MCU״̬��Ϣ��֪���棬���HDU����ʹ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����byCurIndex ����������ʼ
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/09/09              ��־��		  ����
====================================================================*/
void CMcuVcInst::SendExtMcuStatusNotify(u8 byMcsId, u8 byCurIndex, BOOL32 bIsSendAll /*= False*/ )
{
	CServMsg cExtMcuStatusMsg;
	
	u8		byExtPeriEqpNum = 0;	                       
	TEqp	atPeriEqp[MAXNUM_MCU_PERIEQP - MAXNUM_OLDMCU_PERIEQP];	    
	u8      byEqpOnline[MAXNUM_MCU_PERIEQP -MAXNUM_OLDMCU_PERIEQP];     
	u32     dwPeriEqpIpAddr[MAXNUM_MCU_PERIEQP - MAXNUM_OLDMCU_PERIEQP];  
	
	if ( !g_cMcuVcApp.GetMcuExtCurStatus(byCurIndex+1, byExtPeriEqpNum, atPeriEqp, byEqpOnline, dwPeriEqpIpAddr) )
	{
		OspPrintf(TRUE,FALSE, "[SendExtMcuStatusNotify]GetMcuExtCurStatus failed\n");
		return;
	}
	
	if ( byExtPeriEqpNum >0 )
	{
		cExtMcuStatusMsg.SetMsgBody(&byExtPeriEqpNum,sizeof(u8));
		u8 nLoop = 0;
		for ( nLoop = 0; nLoop < byExtPeriEqpNum; nLoop++ )
		{
			cExtMcuStatusMsg.CatMsgBody( (u8*)atPeriEqp + nLoop*(sizeof(TEqp)), sizeof(TEqp) );
		}
		
		for ( nLoop = 0; nLoop < byExtPeriEqpNum; nLoop++ )
		{
			cExtMcuStatusMsg.CatMsgBody( (u8*)byEqpOnline + nLoop*(sizeof(u8)), sizeof(u8) );
		}
		for ( nLoop = 0; nLoop < byExtPeriEqpNum; nLoop++ )
		{
			cExtMcuStatusMsg.CatMsgBody( (u8*)atPeriEqp + nLoop*(sizeof(u32)), sizeof(u32) );	
		}
		
		if (bIsSendAll)
		{
			SendMsgToAllMcs(MCU_MCS_MCUEXTSTATUS_NOTIFY , cExtMcuStatusMsg );
		}
		else
		{
			SendMsgToMcs(byMcsId, MCU_MCS_MCUEXTSTATUS_NOTIFY , cExtMcuStatusMsg );
		}
	}
	return;
}

/*====================================================================
    ������      ��DaemonProcMcsMcuGetMcuStatusReq
    ����        ����ѯMCU״̬������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/20    1.0         LI Yi         ����
	03/06/10	1.2			LI Yi         ��MC�Ựʵ����ת�ƹ���
    03/11/28    3.0         ������         �޸�
====================================================================*/
void CMcuVcInst::DaemonProcMcsMcuGetMcuStatusReq( const CMessage * pcMsg )
{
   	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	TMcuStatus tMcuStatus;
	if (TRUE == g_cMcuVcApp.GetMcuCurStatus(tMcuStatus))
	{
		cServMsg.SetMsgBody((u8 *)&tMcuStatus, sizeof(tMcuStatus));
		//Ack 
		if (MCS_MCU_GETMCUSTATUS_REQ == pcMsg->event)
		{               
			SendMsgToMcs(cServMsg.GetSrcSsnId(), pcMsg->event+1, cServMsg);
		}	
		//notify
		SendMsgToMcs(cServMsg.GetSrcSsnId(), MCU_MCS_MCUSTATUS_NOTIF, cServMsg );
		
		//ext notify for HDU����
		SendExtMcuStatusNotify( cServMsg.GetSrcSsnId(), MAXNUM_OLDMCU_PERIEQP );

	}
	else
	{
		//Nack
		SendMsgToMcs(cServMsg.GetSrcSsnId(), pcMsg->event+2, cServMsg);
	}

	return;
}

/*====================================================================
    ������      ��DaemonProcMcsMcuStopSwitchMcReq
    ����        ��ֹͣ��������̨����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/20    1.0         LI Yi         ����
	04/05/17    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::DaemonProcMcsMcuStopSwitchMcReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	g_cMcuVcApp.BroadcastToAllConf( pcMsg->event, pcMsg->content, pcMsg->length );

	//Ack
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
}

/*=============================================================================
    �� �� ���� DaemonProcMcsMcuStopSwitchMcReq
    ��    �ܣ� ֹͣ�����ǽ����������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/02/25  3.6			����                  ����
=============================================================================*/
void CMcuVcInst::DaemonProcMcsMcuStopSwitchTWReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	g_cMcuVcApp.BroadcastToAllConf( pcMsg->event, pcMsg->content, pcMsg->length );

	//Ack
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
}

/*====================================================================
    ������      ��ProcMcStopSwitch
    ����        ��ֹͣ��������̨��������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/05/17    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcMcStopSwitch( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8  byChannelNum;
	u8	byInstId;
	TLogicalChannel tLogicalChannel;

	switch( cServMsg.GetEventId() )
	{
	case MCS_MCU_STOPSWITCHMC_REQ:
		{
			TSwitchInfo tSwtichInfo = *(TSwitchInfo*)cServMsg.GetMsgBody();
			StopSwitchToMc( cServMsg.GetSrcSsnId(), cServMsg.GetChnIndex(), TRUE, tSwtichInfo.GetMode());
		}		
		break;
	case MCU_MCSDISCONNECTED_NOTIF:
		{
			byInstId = *( u8 * )cServMsg.GetMsgBody();
			if( g_cMcuVcApp.GetMcLogicChnnl( byInstId, MODE_VIDEO, &byChannelNum, &tLogicalChannel ) )
			{
				while( byChannelNum-- != 0 )
				{
					StopSwitchToMc( byInstId, byChannelNum, FALSE, MODE_BOTH, FALSE );
					StopSwitchToMc( byInstId, byChannelNum, FALSE, MODE_SECVIDEO, FALSE );
				}
			}
			StopSwitchToMonitor(byInstId, TRUE);

			// �����ڷ���vrs�ļ��б���Ҷ�vrsʵ��
			if (m_tPlayEqpAttrib.GetListRecordSrcSsnId() != 0
				&& m_tPlayEqpAttrib.GetListRecordSrcSsnId() == cServMsg.GetSrcSsnId())
			{
				if (m_tPlayEqp.GetType() == TYPE_MT && m_tPlayEqp.GetMtType() == MT_TYPE_VRSREC)
				{
					if (m_ptMtTable->GetRecChlState(m_tPlayEqp.GetMtId()) != TRecChnnlStatus::STATE_IDLE
						&& m_ptMtTable->GetRecChlState(m_tPlayEqp.GetMtId()) != TRecChnnlStatus::STATE_CALLING)
					{
						ReleaseVrsMt(m_tPlayEqp.GetMtId(), 0, FALSE);
					}
				}
			}
		}
		break;
    default:
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "ProcMcStopSwitch: unexpected msg %d<%s> received !\n", cServMsg.GetEventId(), OspEventDesc(cServMsg.GetEventId()) );
        break;
	}
}

/*=============================================================================
    �� �� ���� ProcMcsMcuStartSwitchTWReq
    ��    �ܣ� ��ʼ����ǽͨ������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/02/25  3.6			����                  ����
    2005/05/23  3.6			libo                  ���ָ��淽ʽ��ʵ��
=============================================================================*/
void CMcuVcInst::ProcMcsMcuStartSwitchTWReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    StartSwitchTWReq(cServMsg);
}

//4.6.1 �¼�  jlb
/*=============================================================================
    �� �� ���� ProcMcsMcuStopSwitchHduReq
    ��    �ܣ� ֹͣHDUͨ������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
 2009/01/17     4.6         ���ֱ�                 ����       
=============================================================================*/
void CMcuVcInst::ProcMcsMcuStopSwitchHduReq( const CMessage * pcMsg )      
{
	if (CurState() != STATE_ONGOING)
    {
        return;
    }
	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	THduSwitchInfo tHduSwitchInfo;
	TMt	tEqp;
	
	tHduSwitchInfo = *( THduSwitchInfo * )cServMsg.GetMsgBody();
	tEqp = tHduSwitchInfo.GetDstMt();

	// [2013/03/11 chenbing]  
	u8 byHduSubChnId = tHduSwitchInfo.GetDstSubChn();
	 
	// Nack
	if(!IsValidHduEqp(tEqp))
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[CMcuVcInst]::ProcMcsMcuStopSwitchHduReq: Wrong PeriEquipment type %u or it has not registered!\n", 
			tEqp.GetEqpType() );
		cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	TMt tRemoveMt;
    TPeriEqpStatus tHduStatus;
    u8             byHduId = 0;
    u8             byChnlNo = 0;
    byHduId = tEqp.GetEqpId();
    byChnlNo   = tHduSwitchInfo.GetDstChlIdx();
    g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
	tRemoveMt = tHduStatus.m_tStatus.tHdu.GetChnMt(byChnlNo, byHduSubChnId);

	if (tRemoveMt.GetType() != TYPE_MCUPERI)
	{
		if (tRemoveMt.GetConfIdx() != m_byConfIdx)
		{
			return;
		}
	}

	//lukunpeng 2010/07/15 Ϊ��ͳһ����ֱ�ӵ���ChangeHduSwitch
	ChangeHduSwitch(NULL, byHduId, byChnlNo, byHduSubChnId, tHduSwitchInfo.GetMemberType(), TW_STATE_STOP);

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	return;
}

/*=============================================================================
    �� �� ���� ProcMcsMcuChangeHduVolume
    ��    �ܣ� ����hdu����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
 2009/01/17     4.6         ���ֱ�                 ����       
=============================================================================*/
void CMcuVcInst::ProcMcsMcuChangeHduVolume( const CMessage * pcMsg ) 
{
	if ( NULL == pcMsg )
	{
		ConfPrint( LOG_LVL_ERROR, MID_MCU_HDU, "[ProcMcsMcuChangeHduVolume] pcMsg is Null!\n");
		return;
	}
	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
    THduVolumeInfo tHduVolumeInfo = *(THduVolumeInfo*)cServMsg.GetMsgBody();
    
	TPeriEqpStatus tHduStatus;
	g_cMcuVcApp.GetPeriEqpStatus( tHduVolumeInfo.GetEqpId(), &tHduStatus );

	const u8 byChnlIdx = tHduVolumeInfo.GetChnlIdx();

	// [10/24/2011 liuxu] ���������ÿյĵ���ǽͨ��, ҪԤռ��
	//yrl20131024�ö�ע�����農����Ӧ��ռ��
//     if ( 0 == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx() && tHduVolumeInfo.GetIsMute() )
//     {
//         tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetConfIdx( m_byConfIdx );
//     }

    if ( m_byConfIdx != tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx()
		&& tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx() != 0)
    {
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[CMcuVcInst]::ProcMcsMcuChangeHduVolume: Can not change hdu volume or set mute with different ConfIdx\n");
        cServMsg.SetErrorCode( ERR_MCU_HDUCHANGEVOLUME );
		SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 2, cServMsg );
		return;
    }

	// [10/24/2011 liuxu] ȡ������ʱ, �����ʱͨ��Ϊ��, ��Ӧ���ͷ�Ԥռ��
	//yrl20131028�ö�ͬ��ע��
// 	if ( !tHduVolumeInfo.GetIsMute() 
// 		&& tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].IsNull()
// 		&& m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx()
// 		&& tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType == TW_MEMBERTYPE_NULL)
// 	{
// 		tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetConfIdx( 0 );
// 	}

    tHduStatus.m_tStatus.tHdu.atHduChnStatus[tHduVolumeInfo.GetChnlIdx()].SetIsMute( tHduVolumeInfo.GetIsMute() );
    tHduStatus.m_tStatus.tHdu.atHduChnStatus[tHduVolumeInfo.GetChnlIdx()].SetVolume( tHduVolumeInfo.GetVolume() );

	g_cMcuVcApp.SetPeriEqpStatus( tHduVolumeInfo.GetEqpId(), &tHduStatus );

    SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 1, cServMsg );

    SendMsgToEqp( tHduVolumeInfo.GetEqpId(), MCU_HDU_CHANGEVOLUME_CMD, cServMsg );

	cServMsg.SetMsgBody( (u8*)&tHduStatus, sizeof( tHduStatus ) );
	SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );

    return;
}

/*=============================================================================
    �� �� ���� ProcMcsMcuStartSwitchHduReq
    ��    �ܣ� ��ʼHDUͨ������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
 2009/01/17     4.6         ���ֱ�                 ����       
=============================================================================*/
void CMcuVcInst::ProcMcsMcuStartSwitchHduReq( const CMessage * pcMsg )     
{
    if ( NULL == pcMsg )
    {
		ConfPrint( LOG_LVL_ERROR, MID_MCU_HDU,  "[ProcMcsMcuStartSwitchHduReq] pcMsg is NULL!\n");
		return;
    }

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	if (MCUVC_STARTSWITCHHDU_NOTIFY ==  pcMsg->event)
	{
		cServMsg.SetMsgBody(pcMsg->content, sizeof( THduSwitchInfo ));
	}

	StartSwitchHduReq(cServMsg);
}
/*=============================================================================
    �� �� ���� StartSwitchHduReq
    ��    �ܣ� ��ʼhduͨ������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/09/15  4.0			libo                  ����
=============================================================================*/
void CMcuVcInst::StartSwitchHduReq(CServMsg & cServMsg)
{
	// ȡ��Ϣ����
	const THduSwitchInfo &tHduSwitchInfo = *(THduSwitchInfo*)cServMsg.GetMsgBody();

    switch(CurState())
    {
    case STATE_ONGOING:
	{
		TMt	 tSrcMt					= tHduSwitchInfo.GetSrcMt();						// Դ
		
		const TMt	 tDstMt			= tHduSwitchInfo.GetDstMt();
		
		const u8   byHduId			= tDstMt.GetEqpId();								// Ŀ��Hdu��Id
		const u8   byDstChnnlIdx	= tHduSwitchInfo.GetDstChlIdx();					// Ŀ��Hdu��ͨ��
		u8   byHduSubChnId	= 0;												// HDU��ͨ��
		
        //���Ͳ��Ի����ڱ�MCU��δ�Ǽ�
        if (!IsValidHduEqp(tDstMt) || !g_cMcuVcApp.IsPeriEqpConnected(byHduId))
        {
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: Wrong PeriEquipment type %u or chnnlidx %d or it(eqpid:%d) has not registered!\n", 
				tDstMt.GetEqpType(), byDstChnnlIdx, byHduId);
            cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

		// ��ȡ��Hdu�ܹ���ͨ����
		u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(tDstMt);
		if (0 == byHduChnNum || byDstChnnlIdx >= byHduChnNum)
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[StartSwitchHduReq] GetHduChnNumAcd2Eqp failed!\n");
			cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
		}

 	    //����ǽ��ѯ���ȼ���ߣ�����ѡ��VMP��ѡ���ն�
		//[nizhijun 2010/10/28] ���Ӷ���ͨ��״̬�Ƿ�Ϊrunning���ж�
		TPeriEqpStatus tHduStatus;
        g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
        if ( (TW_MEMBERTYPE_TWPOLL ==  tHduStatus.m_tStatus.tHdu.atVideoMt[byDstChnnlIdx].byMemberType)
			&& (THduChnStatus::eRUNNING == tHduStatus.m_tStatus.tHdu.atHduChnStatus[byDstChnnlIdx].GetStatus())
			)
        {
			ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: current chnl is in polling ,can not switch VMP or mt!\n");
          	cServMsg.SetErrorCode(ERR_MCU_NOTSWITCHVMPORMT);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
		}

		// [2013/03/11 chenbing] ��ش�ģʽ 
		u8 bySpyMode = MODE_BOTH;
		u8 byHduMode = HDUCHN_MODE_ONE;
		// [2013/03/11 chenbing]
		if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byDstChnnlIdx) )
		{
			byHduSubChnId = tHduSwitchInfo.GetDstSubChn();
			if (  byHduSubChnId >= HDU_MODEFOUR_MAX_SUBCHNNUM )
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU,\
					"[CMcuVcInst]StartSwitchHduReq: current HduSubChnId(%d) Error or ChnStatus(%d) Error !\n",
					byDstChnnlIdx, tHduStatus.m_tStatus.tHdu.GetChnStatus(byDstChnnlIdx, byHduSubChnId));
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
			bySpyMode = MODE_VIDEO;			// ��ش�ģʽ���ķ����ֻ����Ƶ
			byHduMode = HDUCHN_MODE_FOUR;
			g_cMcuVcApp.SetChnnlMMode( byHduId, byDstChnnlIdx, MODE_VIDEO);
		} 

        if (tSrcMt.GetType() == TYPE_MCUPERI)
		{
			//ѡ����vmpΪ��ʱ                      
			if (GetVmpCountInVmpList() == 0)
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: PeriEquipment type %u has not registered!\n", 
					tDstMt.GetEqpType());
				cServMsg.SetErrorCode(ERR_MCU_MCUPERI_NOTEXIST);
	            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}

			//tSrcMt = m_tVmpEqp;
		}

        // δ�򿪺���ͨ�����������������б���
		if ( tSrcMt.GetType() != TYPE_MCUPERI && TW_MEMBERTYPE_MCSSPEC == tHduSwitchInfo.GetMemberType())
		{
			u8 byMtId = tSrcMt.GetMtId();
			if( !tSrcMt.IsLocal() )
			{
				byMtId = GetFstMcuIdFromMcuIdx( tSrcMt.GetMcuId() );
			}

		    TLogicalChannel tSrcRvsChannl;
			if (!m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuStartSwitchHduReq]: SrcMt(%d) GetMtLogicChnnl fail! Type<%d>\n", tSrcMt.GetMtId(),tSrcRvsChannl.GetMediaType());
				cServMsg.SetErrorCode(ERR_MCU_GETMTLOGCHN_FAIL);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
		}
// 
//         // xsl [7/20/2006] ���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
//         if (m_tConf.GetConfAttrb().IsSatDCastMode())
//         {
//             if (IsOverSatCastChnnlNum(tSrcMt))
//             {
//                 ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuStartSwitchHduReq] over max upload mt num. nack!\n");
//                 cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
//                 SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
//                 return;
//             }
//         }

		// [9/15/2010 xliang] filter the same MT in same Hdu channel
		if( TW_MEMBERTYPE_MCSSPEC == tHduSwitchInfo.GetMemberType() 
			|| TW_MEMBERTYPE_VCSSPEC == tHduSwitchInfo.GetMemberType()
			|| TW_MEMBERTYPE_VCSAUTOSPEC== tHduSwitchInfo.GetMemberType())
		{
			if (tHduStatus.m_tStatus.tHdu.GetChnStatus(byDstChnnlIdx, byHduSubChnId) != THduChnStatus::eIDLE
				&& tHduStatus.m_tStatus.tHdu.GetChnMt(byDstChnnlIdx, byHduSubChnId) == tSrcMt &&
				tHduStatus.m_tStatus.tHdu.GetMemberType(byDstChnnlIdx, byHduSubChnId) != TW_MEMBERTYPE_DOUBLESTREAM)
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "UI drag the same mt.(%d,%d) in the same hdu channel.%d memtype.%d, MCU will do nothing!\n",
					tSrcMt.GetMcuId(), tSrcMt.GetMtId(), byDstChnnlIdx,tHduStatus.m_tStatus.tHdu.atVideoMt[byDstChnnlIdx].byMemberType);

				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
				
				return;
			}
		}

		if ( TW_MEMBERTYPE_POLL == tHduSwitchInfo.GetMemberType() )   // �����Ƿ����Ƶ��ѯ
		{
			bySpyMode = m_tConf.m_tStatus.GetPollMedia();
		}

// 		if (!tSrcMt.IsNull() 
// 			&& tSrcMt.GetType() != TYPE_MCUPERI 
// 			&& !tSrcMt.IsLocal() 
// 			&& !IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()))
//         {	
//             OnMMcuSetIn(tSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
//         }
	
        switch(tHduSwitchInfo.GetMemberType()) 
        {
        case TW_MEMBERTYPE_MCSSPEC:
		case TW_MEMBERTYPE_VCSSPEC:
		case TW_MEMBERTYPE_VCSAUTOSPEC:
			{
				//Դ�ն˱���߱���Ƶ��������
// 				TMtStatus      tMtStatus;
// 				GetMtStatus(tSrcMt, tMtStatus);
// 				
// 				if (FALSE == tMtStatus.IsSendVideo())
// 				{
// 					cServMsg.SetErrorCode(ERR_MCU_SRCISRECVONLY);
// 					SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
// 					return;
// 				}
				
				//Դ�ն˱����Ѿ��������
				if (FALSE == m_tConfAllMtInfo.MtJoinedConf(tSrcMt))
				{
					log(LOGLVL_DEBUG1, "[ProcMcsMcuStartSwitchHduReq]: select source MT%u-%u has not joined current conference! Error!\n",
						tSrcMt.GetMcuId(), tSrcMt.GetMtId());
					cServMsg.SetErrorCode(ERR_MCU_MT_NOTINCONF);
					SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);	//nack
					return;
				}
			}
            break;

        case TW_MEMBERTYPE_SPEAKER:     //ע��ı䷢����ʱ�ı佻��
			tSrcMt.SetNull();							// [9/14/2011 liuxu] �����ÿ�,����mcs��ֵ����
            if (TRUE == HasJoinedSpeaker())
            {
                tSrcMt = m_tConf.GetSpeaker();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;

        case TW_MEMBERTYPE_CHAIRMAN:    //ע��ı���ϯʱ�ı佻��
			tSrcMt.SetNull();							// [9/14/2011 liuxu] �����ÿ�,����mcs��ֵ����
            if (TRUE == HasJoinedChairman())
            {
                tSrcMt = m_tConf.GetChairman();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;

        case TW_MEMBERTYPE_POLL:        //ע��ı���ѯ�㲥Դʱ�ı佻��
			tSrcMt.SetNull();							// [9/14/2011 liuxu] �����ÿ�,����mcs��ֵ����
            if ( CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode()
				&& CONF_POLLMODE_VIDEO_CHAIRMAN != m_tConf.m_tStatus.GetPollMode()		// ��ϯ��ѯʱ����ǽ
				&& CONF_POLLMODE_BOTH_CHAIRMAN != m_tConf.m_tStatus.GetPollMode()		// ����Ƶ��ϯ��ѯҲ����ǽ
				&& CONF_POLLMODE_SPEAKER_SPY != m_tConf.m_tStatus.GetPollMode()			// �ϴ���ѯ����
				&& CONF_POLLMODE_VIDEO_SPY != m_tConf.m_tStatus.GetPollMode()			// �ϴ���ѯ����
				)
            {
                tSrcMt = (TMt)m_tConf.m_tStatus.GetMtPollParam();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;
			
		case TW_MEMBERTYPE_SWITCHVMP:      //hduѡ��vmp
			if (tSrcMt.GetEqpType() != EQP_TYPE_VMP || !IsVmpIdInVmpList(tSrcMt.GetEqpId()))
			{
				tSrcMt.SetNull();
			}
			else
			{
				tSrcMt = g_cMcuVcApp.GetEqp( tSrcMt.GetEqpId() );
			}
			/*tSrcMt.SetNull();							// [9/14/2011 liuxu] �����ÿ�,����mcs��ֵ����
		    if (!m_tVmpEqp.IsNull())
			{
			    tSrcMt = (TMt)m_tVmpEqp;
			}
			else
			{
				tSrcMt.SetNull();
			}*/
			break;

		case TW_MEMBERTYPE_BATCHPOLL:
			break;

		case TW_MEMBERTYPE_TWPOLL:
			break;

		case TW_MEMBERTYPE_DOUBLESTREAM:
			{
				if (m_tDoubleStreamSrc.IsNull())
				{
					tSrcMt.SetNull();
				}
				else
				{
					tSrcMt = m_tDoubleStreamSrc;
				}				
			}
			break;
        default:
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuStartSwitchHduReq]: Wrong TvWall Member Type!\n");
            break;
		}

		//������ش�֧�� ����������¼���֧�ֶ�ش���mcu����presetin��Ϣ
		if(!tSrcMt.IsNull() && !tSrcMt.IsLocal() )
		{
			if( IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()) )
			{
				TLogicalChannel tMtAudSendLogicalChannel;
				u8 bySubEqpType = 0;
				//�����ϵ�HDU�忨���ܽ�AACLC��AACLD˫��������������ô�Ͳ�����Ƶ��������ش�ʱҲֻ�ش���Ƶ����
				if( g_cMcuAgent.GetHduSubTypeByEqpId(byHduId,bySubEqpType) && 
					(HDU_SUBTYPE_HDU_M == bySubEqpType ||
					HDU_SUBTYPE_HDU_H == bySubEqpType  ||
					HDU_SUBTYPE_HDU_L == bySubEqpType) &&
					tSrcMt.GetType() == TYPE_MT && 
					m_ptMtTable->GetMtLogicChnnl(GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId(),LOGCHL_AUDIO,&tMtAudSendLogicalChannel,FALSE) &&
					(tMtAudSendLogicalChannel.GetChannelType() == MEDIA_TYPE_AACLC || tMtAudSendLogicalChannel.GetChannelType() == MEDIA_TYPE_AACLD) && 
					tMtAudSendLogicalChannel.GetAudioTrackNum() == 2)
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuStartSwitchHduReq]HDU NOT SUPPORT AACLC OR AACLC DOUBLETRACK,SO REMOVE AUDIO!\n");
					bySpyMode = MODE_VIDEO;
				}
				TPreSetInReq tSpySrcInitInfo;
				tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt(tSrcMt);
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyHduInfo.m_tHdu = tDstMt;
				tSpySrcInitInfo.m_bySpyMode = bySpyMode;
				tSpySrcInitInfo.SetEvId(cServMsg.GetEventId());
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyHduInfo.m_bySrcMtType = tHduSwitchInfo.GetMemberType();
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyHduInfo.m_bySchemeIdx = tHduSwitchInfo.GetSchemeIdx();
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyHduInfo.m_byDstChlIdx = tHduSwitchInfo.GetDstChlIdx();
				// [2013/03/11 chenbing] ׷����ͨ��
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyHduInfo.SetSubChnIdx(byHduSubChnId);

				// [pengjie 2010/9/13] ��Ŀ�Ķ�����
				TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()) );
				//zjl20101116 �����ǰ�ն��ѻش���������Ҫ���ѻش�Ŀ��������ȡС
				if(!GetMinSpyDstCapSet(tSrcMt, tSimCapSet))
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StartSwitchHduReq] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
						tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
					return;
				}
				
				tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		        // End

				// [pengjie 2010/8/31] �滻�߼�����(֧��HDU�໭��)
				if( !tHduStatus.m_tStatus.tHdu.IsChnNull(byDstChnnlIdx, byHduSubChnId) )
				{
					//�޸Ļ�ȡ�ն˽ӿ�tHduStatus.m_tStatus.tHdu.atVideoMt[byDstChnnlIdx]
					tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = (TMt)tHduStatus.m_tStatus.tHdu.GetChnMt(byDstChnnlIdx, byHduSubChnId);
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= g_cMcuVcApp.GetChnnlMMode(byHduId, byDstChnnlIdx);
					tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
					if ( MODE_VIDEO == bySpyMode || MODE_BOTH == bySpyMode)
					{
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
					}

					if ( MODE_AUDIO == bySpyMode || MODE_BOTH == bySpyMode)
					{
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
					}
				}
			    // End
				OnMMcuPreSetIn( tSpySrcInitInfo );  
				
				// �Ȼ�Ӧ���棬��Ȼ����Ῠ���죬����������������������
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
				
				return;
			}
		}
		// End Modify
		/*TMt tOldSrcMt  =  (TMt)tHduStatus.m_tStatus.tHdu.GetChnMt(byDstChnnlIdx, byHduSubChnId);
		u8  byStatus   = tHduStatus.m_tStatus.tHdu.GetChnStatus(byDstChnnlIdx, byHduSubChnId);
		if (!tOldSrcMt.IsNull() && byStatus == THduChnStatus::eRUNNING)
		{
			u8  byOldMemberType = tHduStatus.m_tStatus.tHdu.GetMemberType(byDstChnnlIdx, byHduSubChnId);
			ChangeHduSwitch( NULL,byHduId, byDstChnnlIdx, byHduSubChnId, byOldMemberType, TW_STATE_STOP,MODE_BOTH,FALSE,TRUE,FALSE );
			g_cMcuVcApp.GetPeriEqpStatus( byHduId, &tHduStatus );
		} */
		// [2013/03/11 chenbing]  
		tHduStatus.m_tStatus.tHdu.SetSchemeIdx( tHduSwitchInfo.GetSchemeIdx(), byDstChnnlIdx, byHduSubChnId);
        g_cMcuVcApp.SetPeriEqpStatus( byHduId, &tHduStatus );

        cServMsg.SetConfIdx(tSrcMt.GetConfIdx());

        //send Ack reply
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

		// [2013/08/13 chenbing] ������ģ����������HDUͨ����Ա,����HDUͨ��ʱ��Ҫ�������ģ��ͨ���еĳ�Ա
		ClearHduChnnlInHduMoudle(byHduId, byDstChnnlIdx, byHduSubChnId, byHduMode);

		TMtStatus      tMtStatus;
		GetMtStatus(tSrcMt, tMtStatus);
		
		u8 byMode = MODE_NONE;
		if (!tMtStatus.IsSendAudio())
		{
			byMode = MODE_VIDEO;
		}
		else if(!tMtStatus.IsSendVideo())
		{
			byMode = MODE_AUDIO;
		}
		else
		{
			byMode = MODE_BOTH;
		}

		if ( TW_MEMBERTYPE_POLL == tHduSwitchInfo.GetMemberType() )   // �����Ƿ����Ƶ��ѯ
		{
			if(byMode == MODE_AUDIO && m_tConf.m_tStatus.GetPollMedia() == MODE_VIDEO)
			{
				byMode = MODE_NONE;
			}
			else if(byMode == MODE_AUDIO && m_tConf.m_tStatus.GetPollMedia() == MODE_BOTH)
			{
				byMode = MODE_AUDIO;
			}
			else if(byMode == MODE_VIDEO && m_tConf.m_tStatus.GetPollMedia() == MODE_BOTH)
			{
				byMode = MODE_VIDEO;
			}
			else
			{
				byMode = m_tConf.m_tStatus.GetPollMedia();
			}

			// [2013/03/11 chenbing] HDU�໭��Ŀǰ��֧����ѯ����,��ͨ����0
			ChangeHduSwitch(&tSrcMt, byHduId, byDstChnnlIdx, 0,	TW_MEMBERTYPE_POLL, TW_STATE_START, byMode);
		}
		else
		{
			ChangeHduSwitch(&tSrcMt, byHduId, byDstChnnlIdx, byHduSubChnId,\
				tHduSwitchInfo.GetMemberType(), TW_STATE_START, byMode, FALSE, TRUE, TRUE, byHduMode);
		}
        break;
    }
    default:
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuStartSwitchHduReq]: Wrong handle in state %u!\n", CurState());
        break;
	}

	return;
}

/*=============================================================================
�� �� ���� ClearHduChnnlInHduMoudle
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� void
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
20130813	4.0			�±�					����
=============================================================================*/
BOOL32 CMcuVcInst::ClearHduChnnlInHduMoudle(u8 byHduId, u8 byHduChnId, u8 byHduSubChnId, u8 byHduMode)
{
	if ( byHduId < HDUID_MIN || byHduId > HDUID_MAX )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_HDU, "[ClearHduChnnlInHduMoudle] HduId(%d) is vaild!!!\n", byHduId);
		return FALSE;
	}

	if ( (FALSE == m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo() && HDUCHN_MODE_FOUR == byHduMode)
		|| (FALSE == m_tConf.GetConfAttrb().IsHasTvWallModule() && HDUCHN_MODE_ONE == byHduMode)
	   )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ClearHduChnnlInHduMoudle] HDU-Module is NULL Don't Clear Hdu-Module Member!!!\n");
		return FALSE;
	}

	u8 byHduSchemeNum = 0;
	THduStyleInfo atHduStyleInfoTable[MAX_HDUSTYLE_NUM];
	u16 wRet = g_cMcuAgent.ReadHduSchemeTable(&byHduSchemeNum, atHduStyleInfoTable);

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU, "[ClearHduChnnlInHduMoudle] byHduMode(%d) Hdu2Vmp-Module(%d) Hdu-Module(%d)!!!\n",
		byHduMode, m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo(), m_tConf.GetConfAttrb().IsHasTvWallModule());

	// HDU2�໭��ģ��
	if( m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo() && HDUCHN_MODE_FOUR == byHduMode)
	{
		if (SUCCESS_AGENT == wRet)
		{
			//��������HDUͨ��
			for (u8 byChnId = 0; byChnId < MAXNUM_HDUBRD*MAXNUM_HDU_CHANNEL; byChnId++)
			{
				if( byHduId == m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byChnId].m_byHduEqpId
					&& byHduChnId == m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byChnId].m_byChIdx)
				{
					m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byChnId].m_abyTvWallMember[byHduSubChnId] = 0;
					m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byChnId].m_abyMemberType[byHduSubChnId] = TW_MEMBERTYPE_NULL;
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ClearHduChnnlInHduMoudle] HduVmpInfo HduId(%d) byHduChnId(%d)!\n", byHduId,
						byHduChnId );
					return TRUE;
				}
			}
		}
	}

	// HDUģ��
	if ( m_tConf.GetConfAttrb().IsHasTvWallModule() && HDUCHN_MODE_ONE == byHduMode)
	{
		if (SUCCESS_AGENT == wRet)
		{
			//��������Ԥ��
			for (u8 bySchemeId = 0; bySchemeId < byHduSchemeNum && bySchemeId < MAXNUM_PERIEQP_CHNNL; bySchemeId++)
			{
				THduChnlInfo  atHduChnlTable[MAXNUM_HDUCFG_CHNLNUM];
				atHduStyleInfoTable[bySchemeId].GetHduChnlTable(atHduChnlTable);
				//����Ԥ���µ�����ͨ��
				for (u8 byStyleChnId = 0; byStyleChnId < MAXNUM_HDUCFG_CHNLNUM; byStyleChnId++)
				{
					if(    atHduChnlTable[byStyleChnId].GetEqpId() == byHduId
						&& atHduChnlTable[byStyleChnId].GetChnlIdx() == byHduChnId
					  )
					{
						m_tConfEqpModule.m_tTvWallInfo[bySchemeId].m_atTvWallMember[byStyleChnId].SetNull();
						m_tConfEqpModule.m_tMultiTvWallModule.m_atTvWallModule[bySchemeId].m_abyTvWallMember[byStyleChnId] = 0;
						// ����TW_MEMBERTYPE_VCSAUTOSPEC���ͱ�ʶ����Ϊ�Զ�ģʽ�¸�ͨ�����ã����Զ��䲻���
						if (m_tConfEqpModule.m_tMultiTvWallModule.m_atTvWallModule[bySchemeId].m_abyMemberType[byStyleChnId] != TW_MEMBERTYPE_VCSAUTOSPEC)
						{
							m_tConfEqpModule.m_tMultiTvWallModule.m_atTvWallModule[bySchemeId].m_abyMemberType[byStyleChnId] = 0;
						}
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ClearHduChnnlInHduMoudle] MemberType(%d)!\n",
							m_tConfEqpModule.m_tMultiTvWallModule.m_atTvWallModule[bySchemeId].m_abyMemberType[byStyleChnId]);
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}


//4.7.2 �¼�
/*=============================================================================
�� �� ���� ChangeMtResFpsInHduVmp
��    �ܣ� �ı�Mt��HDU�໭���еķֱ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 	TMt tMt,				��Ҫ������tMt
			u8 byHduId, 
			u8 byHduChnId,
			u8 byHduSubChnId,	
			BOOL32& bIsStartSwitch, TRUE: ���Խ�������FALSE�����ܽ�����
			BOOL32 bIsStart,		TRUE���µ��ֱ��ʣ�FALSE���ָ��ֱ���
			BOOL32 bForceAdjust,	TRUE��ǿ���µ��ֱ��ʣ�FALSE����ǿ��
ģ�鹦�ܣ� HDU�໭��ӿ����
�� �� ֵ�� void 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		 �汾		�޸���		�߶���    �޸�����
2013/03/11   4.7.2       �±�                  ����(HDU�໭��֧��)
=============================================================================*/
BOOL32 CMcuVcInst::ChangeMtResFpsInHduVmp(TMt tMt,
										  u8 byHduId, 
										  u8 byHduChnId,
										  u8 byHduSubChnId/* = 0*/,
										  BOOL32& bIsStartSwitch,
									      BOOL32 bIsStart/* = TRUE*/,
										  BOOL32 bForceAdjust
										  )
{
	if (tMt.IsNull())
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ChangeMtResFpsInHduVmp] tMt(%d, %d) isnull so return!!!\n",tMt.GetMcuId(), tMt.GetMtId());
		return FALSE;
	}

	TExInfoForResFps tExInfoForResFps;
	TPeriEqpStatus tHduStatus;
	g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);

	tExInfoForResFps.m_byEqpid = byHduId;
	tExInfoForResFps.m_byPos   = byHduChnId;
	tExInfoForResFps.m_byHduSubChnId = byHduSubChnId;
	tExInfoForResFps.m_bIsForceAdjust = bForceAdjust;
	tExInfoForResFps.m_byEvent = EvMask_HDUVMP;
	if (bIsStart)
	{
		tExInfoForResFps.m_byOccupy = Occupy_InChannel;
	}
	else
	{
		tExInfoForResFps.m_byOccupy = Occupy_OutChannel;
	}

	// �ж��Ƿ���Ҫ����֡��
	BOOL32 bIsNeedAjsFps = IsNeedAdjustMtVidFps(tMt, tExInfoForResFps);
	if (bIsNeedAjsFps)
	{
		//���Ҫ��������֡��
		u8  byNewFps = tExInfoForResFps.m_byNewFps;
		// ���͵�֡����Ϣ
		SendChgMtFps(tMt, LOGCHL_VIDEO, byNewFps, bIsStart);
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ChangeMtResFpsInHduVmp] tMt(%d, %d) bIsStart <%d> Adjusted NewFps: <%d>!!!\n",
			tMt.GetMcuId(), tMt.GetMtId(), bIsStart, byNewFps);
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ChangeMtResFpsInHduVmp] Current Fps:<%d> don't Adjusted!!!\n", tExInfoForResFps.m_byNewFps);
	}

	//�µ�֡�ʣ����ܽ�������ֱ�ӷ���
	if (!tExInfoForResFps.m_bResultSetVmpChl && bIsStart)
	{
		bIsStartSwitch = tExInfoForResFps.m_bResultSetVmpChl;
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ChangeMtResFpsInHduVmp] Fps IsStartSwitch:<%d> !!!\n", bIsStartSwitch);
		return bIsNeedAjsFps;
	}

	// �ж��Ƿ���Ҫ�����ֱ���
	BOOL32 bIsNeedAjsRes = IsNeedAdjustMtVidFormat(tMt, tExInfoForResFps);
	if (bIsNeedAjsRes)
	{
		//���Ҫ�������ķֱ���
		u8  byNewRes = tExInfoForResFps.m_byNewFormat;
		// ���͵��ֱ�����Ϣ
		ChangeVFormat(tMt, bIsStart, byNewRes, byHduId, 0xff, byHduSubChnId, 0, 0, byHduChnId);
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ChangeMtResFpsInHduVmp] tMt(%d, %d) bIsStart<%d> Adjusted NewRes: <%d> !!!\n",
			tMt.GetMcuId(), tMt.GetMtId(), bIsStart, byNewRes);
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ChangeMtResFpsInHduVmp] Current Res:<%d> don't Adjusted!!!\n", tExInfoForResFps.m_byNewFormat);
	}

	//�ܲ��ܽ�����
	bIsStartSwitch = tExInfoForResFps.m_bResultSetVmpChl;
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ChangeMtResFpsInHduVmp] Res IsStartSwitch:<%d> !!!\n", bIsStartSwitch);


	return bIsNeedAjsRes;
}

//4.7.2 �¼�
/*=============================================================================
�� �� ���� ProcMcsMcuChangeHduVmpMode
��    �ܣ� �ı�HDUͨ�����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
ģ�鹦�ܣ� HDU�໭��ӿ����
�� �� ֵ�� void 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		 �汾		�޸���		�߶���    �޸�����
2013/03/11   4.7.2       �±�                  ����(HDU�໭��֧��)
=============================================================================*/
void CMcuVcInst::ProcMcsMcuChangeHduVmpMode(const CMessage * pcMsg)
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	ChangeHduVmpMode(cServMsg);
}

/*=============================================================================
�� �� ���� ChangeHduVmpMode
��    �ܣ� ������ı�HDUͨ�������Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
ģ�鹦�ܣ� HDU�໭��ӿ����
�� �� ֵ�� void 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		 �汾		�޸���		�߶���    �޸�����
2013/03/11   4.7.2       �±�                  ����(HDU�໭��֧��)
=============================================================================*/
void CMcuVcInst::ChangeHduVmpMode(CServMsg cServMsg)
{
	u8			byHduId = cServMsg.GetEqpId();
	u8          byHduChnId = cServMsg.GetChnIndex();
	u8			byNewHduMode = *(u8 *)cServMsg.GetMsgBody();

	TPeriEqpStatus  tHduStatus;
	if ( (byHduId < HDUID_MIN || byHduId > HDUID_MAX)
		|| !g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus)
		|| !g_cMcuVcApp.IsPeriEqpConnected(byHduId)
        )
	{
		NotifyMcsAlarmInfo(0, ERR_MCU_CHGMODE_INPUTPARAM_ERROR);
		ConfPrint( LOG_LVL_ERROR, MID_MCU_MCS,
			"[ProcMcsMcuChangeHduVmpMode]: error byHduId (%d) onLine(%d)!!!\n",
			byHduId, g_cMcuVcApp.IsPeriEqpConnected(byHduId) );
		return;
	}

	u8 byConfIdx = tHduStatus.m_tStatus.tHdu.GetConfIdx(byHduChnId);
	// ������ͬ�Ļ���Ų���ͬһ���໭�� 
	if ( byConfIdx && byConfIdx != m_byConfIdx )
	{
		NotifyMcsAlarmInfo(0, ERR_MCU_CHGMODE_INPUTPARAM_ERROR);
		ConfPrint( LOG_LVL_ERROR, MID_MCU_MCS,
			"[ProcMcsMcuChangeHduVmpMode]: Wrong OldConfIdx (%d) CurConfIdx(%d)!!!\n", byConfIdx, m_byConfIdx);
		return;
	}
	
	if( !IsValidHduChn(byHduId, byHduChnId)
		/*|| THduChnStatus::eWAITCHGMODE == tHduStatus.m_tStatus.tHdu.GetChnStatus(byHduChnId)*/
	  )
	{
		ConfPrint( LOG_LVL_ERROR, MID_MCU_MCS,
			"[ProcMcsMcuChangeHduVmpMode]: Wrong IsNoValidHduChn: (%d)!!!\n", byHduChnId );
		
		NotifyMcsAlarmInfo(0, ERR_MCU_CHGMODE_INPUTPARAM_ERROR);
		return;
	}
	
	// ��ǰͨ���Ѿ�����Ҫ�л��ķ��
	if ( tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byHduChnId) == byNewHduMode )
	{
		ConfPrint( LOG_LVL_DETAIL, MID_MCU_MCS,
			"[ProcMcsMcuChangeHduVmpMode]: New HduChnMode and Old HduChnMode is same [%d]!!!\n", byNewHduMode );
		return;
	}

	// ��ǰͨ����֧���ķ�� 
	if (HDUCHN_MODE_FOUR == byNewHduMode && HDU_MODEFOUR_MAX_SUBCHNNUM != tHduStatus.m_tStatus.tHdu.GetChnMaxVmpMode(byHduChnId) )
	{
		ConfPrint( LOG_LVL_DETAIL, MID_MCU_MCS,
			"[ProcMcsMcuChangeHduVmpMode]: Current HduChnnl can't Support HduFourMode HduId:(%d) HduChnId!!!\n", byHduId, byHduChnId );
		NotifyMcsAlarmInfo(0, ERR_MCU_HDUCHN_NOMULTIMODE);
		return;
	}
	
	ConfPrint( LOG_LVL_DETAIL, MID_MCU_MCS,
		"[ProcMcsMcuChangeHduVmpMode]: Mcs Send HduId(%d) HduChnId(%d) NewHduMode(%d) !!!\n", byHduId, byHduChnId, byNewHduMode);

	// ��ǰͨ��Ϊ�ķ����һͨ��ҲΪ�ķ��
	if (  HDUCHN_MODE_FOUR == byNewHduMode
		&& HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byHduChnId, TRUE) )
	{
		ConfPrint( LOG_LVL_DETAIL, MID_MCU_MCS,
			"[ProcMcsMcuChangeHduVmpMode]: Other HduChnnl already is HDUCHN_MODE_FOUR !!!\n" );
		NotifyMcsAlarmInfo(0, ERR_MCU_HDUCHN_MULTIMODE_TOOMORE);
		return;
	}

	// ��һ����л����ķ��
	if ( HDUCHN_MODE_ONE == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byHduChnId)
		&& HDUCHN_MODE_FOUR == byNewHduMode )
	{
		// �ж�ͨ���������ն˴���
		if ( !tHduStatus.m_tStatus.tHdu.GetChnMt(byHduChnId).IsNull())
		{
			u8 byHduMemberType = tHduStatus.m_tStatus.tHdu.GetMemberType(byHduChnId, 0);
			//�������
			ChangeHduSwitch(NULL, byHduId, byHduChnId, 0, byHduMemberType, TW_STATE_STOP, MODE_BOTH);
		}
 		// ���»�ȡ״̬��ChangeHduSwitch�лᷢ���ı�
		g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
		tHduStatus.m_tStatus.tHdu.SetConfIdx(m_byConfIdx, byHduChnId );
	}// ���ķ���л���һ���
	else if (  HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byHduChnId)
		    && HDUCHN_MODE_ONE == byNewHduMode)
	{
		for (u8 byIndex=0; byIndex<HDU_MODEFOUR_MAX_SUBCHNNUM; byIndex++)
		{
			// �ж�ͨ���������ն˴���
			if ( !tHduStatus.m_tStatus.tHdu.GetChnMt(byHduChnId, byIndex).IsNull())
			{
				//�������
				ChangeHduSwitch(NULL, byHduId, byHduChnId, byIndex,	TW_MEMBERTYPE_MCSSPEC, TW_STATE_STOP, MODE_VIDEO,
					FALSE, TRUE, TRUE, HDUCHN_MODE_FOUR);
			}
		}
		// ���»�ȡ״̬��ChangeHduSwitch�лᷢ���ı�
		g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
	}
	
	//����ͨ��״̬
	tHduStatus.m_tStatus.tHdu.SetChnStatus(byHduChnId, 0, THduChnStatus::eWAITCHGMODE);
	g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);

	cServMsg.SetConfId(m_tConf.GetConfId());
	SendChangeHduModeReq(cServMsg);

	return;
}

//4.7.2 �¼�
/*=============================================================================
�� �� ���� SendChangeHduModeReq
��    �ܣ� ������ı�HDUͨ�������Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
ģ�鹦�ܣ� HDU�໭��ӿ����
�� �� ֵ�� void 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		 �汾		�޸���		�߶���    �޸�����
2013/03/11   4.7.2       �±�                  ����(HDU�໭��֧��)
=============================================================================*/
void CMcuVcInst::SendChangeHduModeReq(CServMsg cServMsg)
{

	SendMsgToEqp( cServMsg.GetEqpId(), MCU_HDU_CHGHDUVMPMODE_REQ, cServMsg );
	ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP,
		"[SendChangeHduModeMsg]: Send MCU_HDU_CHGHDUVMPMODE_REQ  OK!!!\n" );

}

/*=============================================================================
    �� �� ���� StartSwitchTWReq
    ��    �ܣ� ��ʼ����ǽͨ������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/09/15  4.0			libo                  ����
=============================================================================*/
void CMcuVcInst::StartSwitchTWReq(CServMsg & cServMsg)
{
    TTWSwitchInfo  tTWSwitchInfo;
    TMtStatus      tMtStatus;
    TMt	           tDstMt;
    TMt            tSrcMt;
    u8             byTvWallId = 0;
    u8             byDstChnnlIdx;
    //TMt            tSrcOrg;             //��Լ���������
    TPeriEqpStatus tTvWallStatus;
    switch(CurState())
    {
    case STATE_ONGOING:

        tTWSwitchInfo = *(TTWSwitchInfo *)cServMsg.GetMsgBody();
        tSrcMt = tTWSwitchInfo.GetSrcMt();
        tDstMt = tTWSwitchInfo.GetDstMt();
        byTvWallId = tDstMt.GetEqpId();
        byDstChnnlIdx = tTWSwitchInfo.GetDstChlIdx();

        //���Ͳ��Ի����ڱ�MCU��δ�Ǽ�
        if (EQP_TYPE_TVWALL != tDstMt.GetEqpType() ||
            !g_cMcuVcApp.IsPeriEqpConnected(byTvWallId) ||
            byDstChnnlIdx >= MAXNUM_PERIEQP_CHNNL)
        {
			ConfPrint( LOG_LVL_DETAIL, MID_MCU_MCS, "CMcuVcInst: Wrong PeriEquipment type %u or it has not registered!\n", 
                            tDstMt.GetEqpType() );

            cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
		
        if (g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTvWallStatus))
        {
			if( tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx].GetConfIdx() != 0 &&
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx].GetConfIdx() != m_byConfIdx &&
			   !tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx].IsNull())
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[StartSwitchTWReq] TvWall<EqpId:%d,ChnId:%d> has been ocuppied by Conf%d\n",
								byTvWallId, byDstChnnlIdx,
								tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx].GetConfIdx()
							);
				
				cServMsg.SetErrorCode(ERR_MCU_HDUBATCHPOLL_CHNLINUSE);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
        }
		
        if (tSrcMt.GetType() == TYPE_MCUPERI)
        {
			//ѡ����vmpΪ��ʱ                      
			if (m_tVmpEqp.IsNull())
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: PeriEquipment type %u has not registered!\n", 
					tDstMt.GetEqpType());
				cServMsg.SetErrorCode(ERR_MCU_MCUPERI_NOTEXIST);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
			
			tSrcMt = m_tVmpEqp;
		}

        // δ�򿪺���ͨ�����������������б���
		/*
		if ( tSrcMt.GetType() != TYPE_MCUPERI )
		{
			if (!m_ptMtTable->GetMtLogicChnnl( tSrcMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
			{
				ConfLog(FALSE, "[StartSwitchTWReq]: GetMtLogicChnnl fail!\n");
				cServMsg.SetErrorCode(ERR_MCU_GETMTLOGCHN_FAIL);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			    return;
			}
		}
		// xliang [7/13/2009] DEC5 adjust resolution strategy
		// zjl    [02/24/2010] DEC5 ���ֱ�����changetvwall��ͳһ����
		if ( tSrcRvsChannl.GetChannelType() == MEDIA_TYPE_H264 &&
			( IsVidFormatHD( tSrcRvsChannl.GetVideoFormat() ) ||
			tSrcRvsChannl.GetVideoFormat() == VIDEO_FORMAT_4CIF ) )
		{
			// nack for none keda MT whose resolution is about  h264 4Cif 
			if ( tSrcOrg.IsLocal() && // ensure that it's MT, not MCU
				MT_MANU_KDC != m_ptMtTable->GetManuId(tSrcMt.GetMtId()) 
				)
			{
				
				ConfLog(FALSE, "[StartSwitchTWReq] Mt<%d> VidType.%d with format.%d support no tvwall\n",
					tSrcMt.GetMtId(), tSrcRvsChannl.GetChannelType(), tSrcRvsChannl.GetVideoFormat() );
				cServMsg.SetErrorCode(ERR_MT_MEDIACAPABILITY);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
				
				return;
			}
			else
			{
				// force the mt to adjust its resolution to Cif 
				
				u8 byNewFormat = VIDEO_FORMAT_CIF;

				if(!tSrcOrg.IsLocal())
				{
					TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tSrcOrg.GetMcuId());
					TMt tLastSrcMt = ptMcInfo->m_tLastMMcuViewMt;
					if (!tLastSrcMt.IsNull())			
					{
						//�ָ��ϴλش�ԴMt�ķֱ���
						// [11/11/2009] Xliang ���ﲢû���ϸ��ջش�Դ�Ƿ�֮ǰ��Dec5�С�
						CascadeAdjMtRes( tLastSrcMt, byNewFormat, FALSE);
					}
					
					CascadeAdjMtRes( tSrcOrg, byNewFormat, TRUE);
				}
				else
				{
					u8 byChnnlType = LOGCHL_VIDEO;
					
					CServMsg cMsg;
					cMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
					cMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
					cMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
					SendMsgToMt( tSrcMt.GetMtId(), cMsg.GetEventId(), cMsg );
					
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[StartSwitchTWReq]send videoformat<%d>(%s) change msg to mt<%d>!\n",
						byNewFormat, GetResStr(byNewFormat), tSrcMt.GetMtId() );
				}
				
				// [12/30/2009 xliang] VMP ǰ����ͨ������
				TChnnlMemberInfo tChnnlMemInfo;
				for(u8 byLoop = 0; byLoop < MAXNUM_SVMPB_HDCHNNL; byLoop ++)
				{
					m_tVmpChnnlInfo.GetHdChnnlInfo(byLoop, &tChnnlMemInfo);
					if( tChnnlMemInfo.GetMt() == tSrcOrg )
					{
						m_tVmpChnnlInfo.ClearOneChnnl(tChnnlMemInfo.GetMtId()); 
					}
				}
			}
        }
		*/
        // [9/15/2010 xliang] filter the same MT in same tvwall channel
		if( TW_MEMBERTYPE_MCSSPEC == tTWSwitchInfo.GetMemberType() 
			|| TW_MEMBERTYPE_VCSSPEC == tTWSwitchInfo.GetMemberType())
		{
			if (tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx] == tSrcMt)
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "UI drag the same mt.(%d,%d) in the same tw channel.%d, MCU will do nothing!\n",
					tSrcMt.GetMcuId(), tSrcMt.GetMtId(), byDstChnnlIdx);
				
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
				
				return;
			}
		}
	
        // xsl [7/20/2006] ���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
        if (m_tConf.GetConfAttrb().IsSatDCastMode()
			&& IsMultiCastMt(tSrcMt.GetMtId())
			&& !IsSatMtSend(tSrcMt))
        {
            //��ʱ��֧���滻
//            TPeriEqpStatus tTWStatus;
//            g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTWStatus);
//            BOOL32 bCheck = FALSE;
//            TMt tDstChnnlMt = tTWStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx];
//            if (!tDstChnnlMt.IsNull())
//            {
//                //��Ŀ��ͨ�����ն˻�������ͨ���ڣ����飻�����滻�����
//                bCheck = !IsMtNotInOtherTvWallChnnl(tDstChnnlMt.GetMtId(), byTvWallId, byDstChnnlIdx);
//            }
//            else
//            {
//                bCheck = TRUE;
//            }
            
            if (/*IsOverSatCastChnnlNum(tSrcMt)*/
				//IsSatMtOverConfDCastNum(tSrcMt, emTvWall, 0xff, 0xff, 0xff, byTvWallId,byDstChnnlIdx))
				!IsSatMtCanContinue())
            {
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[StartSwitchTWReq] over max upload mt num. nack!\n");
                cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
        }
	
        switch(tTWSwitchInfo.GetMemberType()) 
        {
        case TW_MEMBERTYPE_MCSSPEC:
		case TW_MEMBERTYPE_VCSAUTOSPEC:
		//zjj20091025
		case TW_MEMBERTYPE_VCSSPEC:

            //Դ�ն˱���߱���Ƶ��������
			GetMtStatus(tSrcMt, tMtStatus);

            if (FALSE == tMtStatus.IsSendVideo() &&
				m_tConf.GetConfSource() != VCS_CONF)
            {
                cServMsg.SetErrorCode(ERR_MCU_SRCISRECVONLY);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
                return;
            }
			
            //Դ�ն˱����Ѿ��������
            if (!m_tConfAllMtInfo.MtJoinedConf(tSrcMt) &&
				m_tConf.GetConfSource() != VCS_CONF)
            {
                log(LOGLVL_DEBUG1, "CMcuVcInst: select source MT%u-%u has not joined current conference! Error!\n",
                                    tSrcMt.GetMcuId(), tSrcMt.GetMtId());
                cServMsg.SetErrorCode(ERR_MCU_MT_NOTINCONF);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);	//nack
                return;
            }
            break;

        case TW_MEMBERTYPE_SPEAKER://ע��ı䷢����ʱ�ı佻��
			
            if (TRUE == HasJoinedSpeaker())
            {
                tSrcMt = m_tConf.GetSpeaker();
            }
			else
			{
				tSrcMt.SetNull();
			}

            break;

        case TW_MEMBERTYPE_CHAIRMAN://ע��ı���ϯʱ�ı佻��
			tSrcMt.SetNull();							// [9/14/2011 liuxu] �����ÿ�,����mcs��ֵ����
            if (TRUE == HasJoinedChairman())
            {
                tSrcMt = m_tConf.GetChairman();
            }

            break;

        case TW_MEMBERTYPE_POLL://ע��ı���ѯ�㲥Դʱ�ı佻��
			tSrcMt.SetNull();							// [9/14/2011 liuxu] �����ÿ�,����mcs��ֵ����
            if (CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode()
				&& CONF_POLLMODE_VIDEO_CHAIRMAN != m_tConf.m_tStatus.GetPollMode()		// ��ϯ��ѯ����
				&& CONF_POLLMODE_BOTH_CHAIRMAN != m_tConf.m_tStatus.GetPollMode()		// ��ϯ��ѯ����
				&& CONF_POLLMODE_SPEAKER_SPY != m_tConf.m_tStatus.GetPollMode()			// �ϴ���ѯ����
				&& CONF_POLLMODE_VIDEO_SPY != m_tConf.m_tStatus.GetPollMode()			// �ϴ���ѯ����
				)
            {
                tSrcMt = (TMt)m_tConf.m_tStatus.GetMtPollParam();
            }

            break;

        default:
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"CMcuVcInst: Wrong TvWall Member Type!\n");
            break;
		}

		//  [12/11/2009 pengjie] Modify ������ش�֧�� ����������¼���֧�ֶ�ش���mcu����presetin��Ϣ
		if(!tSrcMt.IsLocal() && !tSrcMt.IsNull() )
		{
			if( IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()) )
			{
				TPreSetInReq tSpySrcInitInfo;
				tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt(tSrcMt);
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_tTvWall = tDstMt;
				tSpySrcInitInfo.m_bySpyMode = MODE_BOTH;
				tSpySrcInitInfo.SetEvId(cServMsg.GetEventId());
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_bySrcMtType = tTWSwitchInfo.GetMemberType();
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_bySchemeIdx = tTWSwitchInfo.GetSchemeIdx();
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_byDstChlIdx = tTWSwitchInfo.GetDstChlIdx();
				
				// [pengjie 2010/9/13] ��Ŀ�Ķ�����
				TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()) );	
				if(tSimCapSet.IsNull())
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StartSwitchTWReq] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
						tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
					return;
				}
				// �������ǽֻ��cif������
				tSimCapSet.SetVideoResolution( VIDEO_FORMAT_CIF );
				//zjl20101116 �����ǰ�ն��ѻش���������Ҫ���ѻش�Ŀ��������ȡС
				if (!GetMinSpyDstCapSet(tSrcMt, tSimCapSet))
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StartSwitchTWReq] GetMinSpyDstCapSet failed!\n");
					return;
				}
				tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		        // End

				// [pengjie 2010/8/31] �����滻�߼�����
				if( !tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx].IsNull() )
				{
					tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = \
						(TMt)tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx];
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_BOTH;
					tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
				}
			    // End
		
				OnMMcuPreSetIn( tSpySrcInitInfo );  
				
				// �Ȼ�Ӧ���棬��Ȼ����Ῠ���죬����������������������
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
				
				return;
			}
		}
		// End Modify

        //send Ack reply
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

		if (!tSrcMt.IsNull() && !tSrcMt.IsLocal())
        {
            OnMMcuSetIn(tSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
        }

        ChangeTvWallSwitch(&tSrcMt, byTvWallId, byDstChnnlIdx, tTWSwitchInfo.GetMemberType(), TW_STATE_START);

        break;

    default:
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
        break;
	}	
}

/*=============================================================================
    �� �� ���� ProcMcsMcuStopSwitchTWReq
    ��    �ܣ� ֹͣ����ǽͨ������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/02/25  3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMcsMcuStopSwitchTWReq( const CMessage * pcMsg )
{
    if (CurState() != STATE_ONGOING)
    {
        return;
    }

	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TTWSwitchInfo tTWSwitchInfo;
	TMt	tEqp;

	tTWSwitchInfo = *( TTWSwitchInfo * )cServMsg.GetMsgBody();
	tEqp = tTWSwitchInfo.GetDstMt();

	//error, Nack
	if( tEqp.GetEqpType() != EQP_TYPE_TVWALL )
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcSsnInst: Wrong PeriEquipment type %u or it has not registered!\n", 
			             tEqp.GetEqpType() );
		cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	// �öδӻ�ACK��Ϣ�������ᵽ�����ֹ����Ҳ�ڿ�����ǽ�Ļ����ظ���stop����
	// zgc, 2008-01-25
	TMt tRemoveMt;
    TPeriEqpStatus tTWStatus;
    u8             byTvWallId = 0;
    u8             byChnlNo;
    byTvWallId = tEqp.GetEqpId();
    byChnlNo   = tTWSwitchInfo.GetDstChlIdx();
    g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTWStatus);
    tRemoveMt = tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlNo];
    if (tRemoveMt.GetConfIdx() != m_byConfIdx)
    {
        return;
    }
    
	StopSwitchToPeriEqp( tEqp.GetEqpId(), tTWSwitchInfo.GetDstChlIdx() );

	//lukunpeng 2010/07/22 ����ǽ��������,Ϊʲô���ֳɵķ�װ�ӿڲ����ã���Ҫ������ĵ����߼�
	//����������߼�����Ҳ����ȫ������ָ��ֱ��ʵȡ�
	/*
	//  [12/9/2009 pengjie] Modify ������ش�֧�֣����ﳢ���ͷŻش���Դ
	FreeRecvSpy( tRemoveMt, MODE_BOTH );
	//  End
	
	tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlNo].byMemberType = 0;//tTWSwitchInfo.GetMemberType();
    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlNo].SetNull();
	tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlNo].SetConfIdx(0);
    g_cMcuVcApp.SetPeriEqpStatus(byTvWallId, &tTWStatus);

    if( IsMtNotInOtherTvWallChnnl(tRemoveMt, byTvWallId, byChnlNo) )  
    {
//         m_ptMtTable->SetMtInTvWall(tLocalMt.GetMtId(), FALSE);
//         MtStatusChange(); 

		//2010/07/19 lukp �����ն˵���ǽ״̬
		TMtStatus tMtStatus;
		GetMtStatus(tRemoveMt, tMtStatus);
		tMtStatus.SetInTvWall(FALSE);
		SetMtStatus(tRemoveMt, tMtStatus);
		MtStatusChange(&tRemoveMt);
		
		//�ָ��ն˵�ԭʼ�ֱ��ʣ��������¼�����
		RecoverMtResInMpu( tRemoveMt );
    }
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMcsMcuStopSwitchTWReq] Mt<OrgMcuId:%d, OrgMtId:%d> <LocalMcuId:%d, LocalMtId:%d>is in other TvWallChn!\n",
			tRemoveMt.GetMcuId(), tRemoveMt.GetMtId(),
			tLocalMt.GetMcuId(), tLocalMt.GetMtId());
	}
    	
	CServMsg cMsg;
    cMsg.SetMsgBody((u8 *)&tTWStatus, sizeof(tTWStatus));
    SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cMsg);
	*/

	//lukunpeng 2010/07/22 ����ǽ��������,Ϊʲô���ֳɵķ�װ�ӿڲ����ã���Ҫ������ĵ����߼�
	//����������߼�����Ҳ����ȫ������ָ��ֱ��ʵȡ�
	ChangeTvWallSwitch(NULL, byTvWallId, byChnlNo, tTWSwitchInfo.GetMemberType(), TW_STATE_STOP);
	
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
    //���TvWallģ����Ӧ��Ա��Ϣ
    TMt tNullMt;
    tNullMt.SetNull();
    u8 byMtPos;
    u8 byMemberType = 0;
    BOOL32 bExist = m_tConfEqpModule.GetTvWallMemberByMt(byTvWallId, tRemoveMt, byMtPos, byMemberType);
    if( bExist )
    {
        m_tConfEqpModule.SetTvWallMemberInTvInfo(byTvWallId, byChnlNo, tNullMt);    // remove tv wall info
        m_tConfEqpModule.m_tMultiTvWallModule.RemoveMtByTvId(byTvWallId, tRemoveMt.GetMtId());  // remove mutilTvWallmodule
    }

	
	
}

/*=============================================================================
  �� �� ���� IsMtNotInOtherTvWallChnnl
  ��    �ܣ� �Ƿ���Ҫ����ն��ڵ���ǽͨ����״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMt tMt
             u8 byTvWallId
             u8 byChnlId
			 BOOL32 bIsExceptSelf
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcInst::IsMtNotInOtherTvWallChnnl(TMt tMt, u8 byTvWallId, u8 byChnlId,BOOL32 bIsExceptSelf,BOOL32 bIsFakeInTvWall)
{
	TMt tMtInTv;
	TPeriEqpStatus tTWStatus;
	//zjj20091026������
	if (!bIsExceptSelf)
	{
		if( byTvWallId >= TVWALLID_MIN && byTvWallId <= TVWALLID_MAX )
		{
			g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTWStatus);
			
			//�Ƿ��ڱ�����ǽ������ͨ����
			for(u8 byLp = 0; byLp < tTWStatus.m_tStatus.tTvWall.byChnnlNum; byLp++)
			{
				if(byLp != byChnlId)
				{
					tMtInTv = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byLp];
					//zjl[20100205]�������жϻ���id�������ǰlocal��������������ն����õ���ǰ������ͬid���նˣ����»�����ж�����
					if (tMtInTv.GetConfIdx() == m_byConfIdx)
					{					
						if( tMtInTv == tMt )
						{
							//����VCS������˵����ͨ��ģʽ�жϣ����ΪMODE_NONE���ʾΪ�ٽ�ǽ�նˣ�ʵ��ǽ�в��޸��ն�ͼ��
							if (bIsFakeInTvWall && VCS_CONF == m_tConf.GetConfSource() && MODE_NONE == g_cMcuVcApp.GetChnnlMMode(byTvWallId,byLp))
							{
								ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU, "[IsMtNotInOtherTvWallChnnl]VCS_CONF Mt(%d,%d) Is Not In (%d,%d)!\n",
									tMt.GetMcuId(),tMt.GetMtId(),byTvWallId,byLp);	
							} 
							else
							{
								return FALSE;
							}
						}
					}
				}
			}
		}
	}
    //�Ƿ�����������ǽ��ͨ����
    for(u8 byTvId = TVWALLID_MIN; byTvId <= TVWALLID_MAX; byTvId++)
    {
        if(byTvWallId == byTvId)
            continue;
        
        g_cMcuVcApp.GetPeriEqpStatus(byTvId, &tTWStatus);
        if( tTWStatus.m_byOnline )
        {
            for(u8 byLp = 0; byLp < tTWStatus.m_tStatus.tTvWall.byChnnlNum; byLp++)
            {
                tMtInTv = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byLp];
				//zjl[20100205]�������жϻ���id�������ǰlocal��������������ն����õ���ǰ������ͬid���նˣ������жϻ��������
				if (tMtInTv.GetConfIdx() == m_byConfIdx)
				{					
					if(tMtInTv == tMt)
					{
						//����VCS������˵����ͨ��ģʽ�жϣ����ΪMODE_NONE���ʾΪ�ٽ�ǽ�նˣ�ʵ��ǽ�в��޸��ն�ͼ��
						if (bIsFakeInTvWall && VCS_CONF == m_tConf.GetConfSource() && MODE_NONE == g_cMcuVcApp.GetChnnlMMode(byTvId,byLp))
						{
							ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU, "[IsMtNotInOtherTvWallChnnl]VCS_CONF Mt(%d,%d) Is Not In (%d,%d)!\n",
								tMt.GetMcuId(),tMt.GetMtId(),byTvId,byLp);	
						} 
						else
						{
							return FALSE;
						}
					}
				}				
            }
        }
    }
	
    return TRUE;
}

/*=============================================================================
  �� �� ���� IsMtNotInOtherTvWallChnnl
  ��    �ܣ� �Ƿ���Ҫ����ն��ڵ���ǽͨ����״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMtId
             u8 byTvWallId
             u8 byChnlId
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcInst::IsMtNotInOtherTvWallChnnl(u8 byMtId, u8 byTvWallId, u8 byChnlId)
{
	TMt tMtInTv;
    TPeriEqpStatus tTWStatus;
	//zjj20091026������
	if( byTvWallId >= TVWALLID_MIN && byTvWallId <= TVWALLID_MAX )
	{
		g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTWStatus);
		
		//�Ƿ��ڱ�����ǽ������ͨ����
		for(u8 byLp = 0; byLp < tTWStatus.m_tStatus.tTvWall.byChnnlNum; byLp++)
		{
			if(byLp != byChnlId)
			{
				tMtInTv = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byLp];
				//zjl[20100205]�������жϻ���id�������ǰlocal��������������ն����õ���ǰ������ͬid���նˣ����»�����ж�����
				if (tMtInTv.GetConfIdx() == m_byConfIdx)
				{
					tMtInTv = GetLocalMtFromOtherMcuMt( tMtInTv );
					if(tMtInTv.GetMtId() == byMtId)
					{
						//����VCS������˵����ͨ��ģʽ�жϣ����ΪMODE_NONE���ʾΪ�ٽ�ǽ�նˣ�ʵ��ǽ�в��޸��ն�ͼ��
						if (VCS_CONF == m_tConf.GetConfSource() && MODE_NONE == g_cMcuVcApp.GetChnnlMMode(byTvWallId,byLp))
						{
							ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU, "[IsMtNotInOtherTvWallChnnl]VCS_CONF Mt(%d) Is Not In (%d,%d)!\n",
								byMtId,byTvWallId,byLp);	
						} 
						else
						{
							return FALSE;
						}
					}
				}
            }
        }
    }
	
    //�Ƿ�����������ǽ��ͨ����
    for(u8 byTvId = TVWALLID_MIN; byTvId <= TVWALLID_MAX; byTvId++)
    {
        if(byTvWallId == byTvId)
            continue;
        
        g_cMcuVcApp.GetPeriEqpStatus(byTvId, &tTWStatus);
        if( tTWStatus.m_byOnline )
        {
            for(u8 byLp = 0; byLp < tTWStatus.m_tStatus.tTvWall.byChnnlNum; byLp++)
            {
                tMtInTv = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byLp];
				//zjl[20100205]�������жϻ���id�������ǰlocal��������������ն����õ���ǰ������ͬid���նˣ������жϻ��������
				if (tMtInTv.GetConfIdx() == m_byConfIdx)
				{
					tMtInTv = GetLocalMtFromOtherMcuMt( tMtInTv );
					if(tMtInTv.GetMtId() == byMtId)
					{
						//����VCS������˵����ͨ��ģʽ�жϣ����ΪMODE_NONE���ʾΪ�ٽ�ǽ�նˣ�ʵ��ǽ�в��޸��ն�ͼ��
						if (VCS_CONF == m_tConf.GetConfSource() && MODE_NONE == g_cMcuVcApp.GetChnnlMMode(byTvId,byLp))
						{
							ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU, "[IsMtNotInOtherTvWallChnnl]VCS_CONF Mt(%d) Is Not In (%d,%d)!\n",
								byMtId,byTvId,byLp);	
						} 
						else
						{
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
    ������      ��ProcTvwallConnectedNotif
    ����        ������ǽ�ϵ�֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/09/15    4.0         libo          ����
====================================================================*/
void CMcuVcInst::ProcTvwallConnectedNotif(const CMessage * pcMsg)
{
    if (STATE_ONGOING != CurState())
    {
        //ConfPrint( LOG_LVL_ERROR, MID_MCU_EQP,  "[ProcTvwallConnectedNotif]CurState() = %d\n", CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TPeriEqpRegReq tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();

    TPeriEqpStatus tTvWallStatus;

    TTWSwitchInfo tTWSwitchInfo;
    u8 byMemberType;
    u8 bySrcMtConfIdx;
    u8 bySrcMtId;
    u8 byLoop;
    TEqp tEqp;
	TMt tTvwSrcMt;

    tEqp.SetMcuEqp((u8)tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());

    g_cMcuVcApp.GetPeriEqpStatus(tRegReq.GetEqpId(), &tTvWallStatus);
    u8 byMemberNum = tTvWallStatus.m_tStatus.tTvWall.byChnnlNum;
    for (byLoop = 0; byLoop < byMemberNum; byLoop++)
    {
        bySrcMtConfIdx = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
        if (m_byConfIdx == bySrcMtConfIdx)
        {
            byMemberType = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType;
            bySrcMtId = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMtId();
            if (TW_MEMBERTYPE_MCSSPEC == byMemberType && !m_tConfAllMtInfo.MtJoinedConf(bySrcMtId))
            {
                tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
                tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
            }
        }
    }
    g_cMcuVcApp.SetPeriEqpStatus(tRegReq.GetEqpId(), &tTvWallStatus);

    for (byLoop = 0; byLoop < byMemberNum; byLoop++)
    {
		// [9/20/2011 liuxu] ����ȡ״̬, �Ա�����汣��ʱ�þ�״̬������״̬
		g_cMcuVcApp.GetPeriEqpStatus(tRegReq.GetEqpId(), &tTvWallStatus);

        bySrcMtConfIdx = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
		tTvwSrcMt = (TMt)tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop];
        if (m_byConfIdx == bySrcMtConfIdx && !tTvwSrcMt.IsNull())
        {
            byMemberType = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType;
			if (VCS_CONF == m_tConf.GetConfSource() && ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
				m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE) 
			{
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx( (u8)0 );
				g_cMcuVcApp.SetPeriEqpStatus(tRegReq.GetEqpId(), &tTvWallStatus);
				ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU,"[ProcTvwallConnectedNotif]VCS GROUP REVIEWTW MODE AND Mt(%d,%d) not online,So Continue!\n",
					tTvwSrcMt.GetMcuId(),tTvwSrcMt.GetMtId());
				continue;
			}
            if (0 != byMemberType)
            {
				// [8/30/2011 liuxu] ��Ҫ���ָ��ָ���״̬���趨Դ�ն�, ��Ϊ����ǽ��������ʱ,
				// ��ѯ���������п����Ѿ�����
				switch (byMemberType)
				{
				case TW_MEMBERTYPE_POLL:
					{
						if(m_tConfPollParam.GetCurrentMtPolled())
							// ��Ϊ��ǰ����ѯ�ն�
							tTvwSrcMt = m_tConfPollParam.GetCurrentMtPolled()->GetTMt();
						else
							// ˵����ǰû����ѯ
							tTvwSrcMt.SetNull();
					}					
					break;

				case TW_MEMBERTYPE_CHAIRMAN:
					{
						// ��Ϊ��ǰ��ϯ
						tTvwSrcMt = m_tConf.GetChairman();
					}
					break;

				case TW_MEMBERTYPE_SPEAKER:
					{
						tTvwSrcMt = m_tConf.GetSpeaker();
					}
					break;

				default:
					break;
				}

                memset(&tTWSwitchInfo, 0, sizeof(TTWSwitchInfo));
                tTWSwitchInfo.SetMemberType(byMemberType);
                tTWSwitchInfo.SetSrcMt(tTvwSrcMt);
                tTWSwitchInfo.SetDstMt(tEqp);
                tTWSwitchInfo.SetDstChlIdx(byLoop);

                cServMsg.SetMsgBody((u8 *)&tTWSwitchInfo, sizeof(TTWSwitchInfo));

				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ProcTvwallConnectedNotif] tMt(%d,%d) bMemberType.%d\n",
					tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMcuId(),
					tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMtId(),
					byMemberType
					);

				//20101026_tzy VCS����ʱ�����ش�ʱ,ͬһ���¼���ԭ������ǽ�ն��뵱ǰ�ϴ��ն˲�һ�����ûָ�
				TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();
				if (VCS_CONF == m_tConf.GetConfSource() && !tCurVCMT.IsNull()
					&& !tCurVCMT.IsLocal() && tCurVCMT.GetMcuIdx() == tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMcuIdx()
					&& tCurVCMT.GetMtId() != tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMtId()
					&& !IsLocalAndSMcuSupMultSpy(tCurVCMT.GetMcuIdx()))
				{
					tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
					tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
					tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx( (u8)0 );
					g_cMcuVcApp.SetPeriEqpStatus(tRegReq.GetEqpId(), &tTvWallStatus);
					cServMsg.SetMsgBody((u8 *)&tTvWallStatus, sizeof(tTvWallStatus));
					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
					continue;
				}

				cServMsg.SetEventId(MCS_MCU_START_SWITCH_TW_REQ);

				// [9/20/2011 liuxu] ����, StartSwitchTWReq�л��ж�Դ�ǲ����Ѿ���Ŀ��tvwͨ����,����Ǿͻ���ֹ
				// ������������������
                tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
                tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
				g_cMcuVcApp.SetPeriEqpStatus(tRegReq.GetEqpId(), &tTvWallStatus);

				StartSwitchTWReq(cServMsg);
            }
        }
    }

    return;
}

/*====================================================================
    ������      ��ProcTvwallDisconnectedNotif
    ����        ������ǽ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/05/17    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcTvwallDisconnectedNotif(const CMessage * pcMsg)
{
    if (STATE_ONGOING != CurState())
    {
        //ConfPrint( LOG_LVL_ERROR, MID_MCU_EQP, "[TvwallDisconnectedNotif]CurState() = %d\n", CurState());
        return;
    }
	
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();
	
    TPeriEqpStatus tTvWallStatus;
    u8 bySrcMtConfIdx;
	
    g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tTvWallStatus);
    u8 byMemberNum = tTvWallStatus.m_tStatus.tTvWall.byChnnlNum;
    for (u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
    {
        bySrcMtConfIdx = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
        if (m_byConfIdx == bySrcMtConfIdx)
        {
			TMt tChnMt = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop];
			RefreshMtStatusInTw(tChnMt, FALSE, FALSE);
            StopSwitchToPeriEqp(tEqp.GetEqpId(), byLoop);
			
			//  [12/9/2009 pengjie] Modify ������ش�֧�֣����ﳢ���ͷŻش���Դ
			FreeRecvSpy( tChnMt, MODE_BOTH );


//             bySrcMtId = GetLocalMtFromOtherMcuMt( tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop] ).GetMtId();
//             m_ptMtTable->SetMtInTvWall(bySrcMtId, FALSE);
// 			TMt tMt = m_ptMtTable->GetMt( bySrcMtId );
//             MtStatusChange( &tMt );
// 			
//             StopSwitchToPeriEqp(tEqp.GetEqpId(), byLoop);
// 			//  [12/9/2009 pengjie] Modify ������ش�֧�֣����ﳢ���ͷŻش���Դ
// 			FreeRecvSpy( (TMt)tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop], MODE_BOTH );

			// [8/30/2011 liuxu] �������ߺ�, ����ѯ�ն˺ܿ����Ѿ�����
			if (TW_MEMBERTYPE_POLL == tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType)
			{
				//��ѵ��Ƶ�������Բ��䣬�ն���գ����������鲻���壬�Ա����ߺ������ѵ��Ƶ����
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
			}
			if (VCS_CONF == m_tConf.GetConfSource() && ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
				m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE)
			{
				//VCS���Ԥ������ǽģʽ�£�����ǽ���ߺ������߲����ָ�
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
			}
			if(VCS_CONF == m_tConf.GetConfSource() && m_cVCSConfStatus.GetMtInTvWallCanMixing() && 
				!(tChnMt == m_cVCSConfStatus.GetCurVCMT()) && 
				(
				(tChnMt.IsLocal() && m_ptMtTable->IsMtInMixing(tChnMt.GetMtId())) || 
				(!tChnMt.IsLocal() && m_ptConfOtherMcTable->IsMtInMixing(tChnMt))
				) &&
				IsMtNotInOtherTvWallChnnl(tChnMt,tEqp.GetEqpId(),0,TRUE) && IsMtNotInOtherHduChnnl(tChnMt,0,0)
			  )
			{
				RemoveSpecMixMember(&tChnMt,1);
			}
		}
    }

	g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tTvWallStatus);

    //  zjlhdupoll 
    // �������ǽ��ͨ����ѯ
	for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP_CHNNL; byIdx++)
	{
		TTvWallPollParam tTWPollParam;
		memset(&tTWPollParam, 0, sizeof(TTvWallPollParam));
		if (m_tTWMutiPollParam.GetTWPollParam(tEqp.GetEqpId(), byIdx, tTWPollParam) &&
			POLL_STATE_NONE != tTWPollParam.GetPollState())
		{
			TMtPollParam tMtPollParam;
			memset(&tMtPollParam, 0, sizeof(tMtPollParam));
			tTWPollParam.SetMtPollParam(tMtPollParam);
			tTWPollParam.SetPollState(POLL_STATE_NONE);
			
			cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
			SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
			
			u32 dwTimerIdx = 0;
			if (!m_tTWMutiPollParam.GetTimerIdx(tEqp.GetEqpId(), byIdx, dwTimerIdx))
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcTvwallDisconnectedNotif] Cannot get TW<EqpId:%d, ChnId:%d> TimerIdx",
					tEqp.GetEqpId(), byIdx);
			} 
			KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
		}
	}
}

//4.6 �¼�  jlb
/*====================================================================
    ������      ProcHduMcuStatusNotif
    ����        ��Hdu״̬֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/01/20    4.6         ���ֱ�        ����
====================================================================*/
// void CMcuVcInst::ProcHduMcuStatusNotif( const CMessage * pcMsg )
// {
// 	return;
// }

/*====================================================================
    ������      ProcHduConnectedNotif
    ����        ��hdu����֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/01/20    4.6         ���ֱ�        ����
====================================================================*/
void CMcuVcInst::ProcHduConnectedNotif(const CMessage * pcMsg)
{
     if (STATE_ONGOING != CurState())
     {
		 ConfPrint( LOG_LVL_ERROR, MID_MCU_HDU, "[ProcHduConnectedNotif]CurState() = %d\n", CurState());
         return;
     }
	 
     CServMsg cServMsg(pcMsg->content, pcMsg->length);
     TPeriEqpRegReq tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();
 
	 TEqp tEqp;
     tEqp.SetMcuEqp((u8)tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());
	 ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,  "[ProcHduConnectedNotif] HDU ID CONNECTED(%d) \n",tRegReq.GetEqpId() );
 
	 u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(tEqp);
	 if (0 == byHduChnNum)
	 {
		 ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU,"[ProcHduConnectedNotif] GetHduChnNumAcd2Eqp failed!\n");
		 return;
	 }

	 THduSwitchInfo tHduSwitchInfo;
     u8 byMemberType = 0;
     u8 bySrcMtConfIdx;
	 u8 bySchmeIdx;
	 TMt tInHduMt;
	 TPeriEqpStatus tHduStatus;
	 TTvWallPollParam tTWPollParam;

	 BOOL32 bIsChgHduPicture = TRUE;	// [2013/08/14 chenbing] �Ƿ�ı�HDU��ʾ����ͼƬ 
     for (u8 byLoop = 0; byLoop < byHduChnNum; byLoop++)
     {
		 // [12/15/2010 xliang] ��Ϊ֮���tHduStatus�б䶯������ÿ��ѭ��������Ӧ��ȡһ�����µ�tHduStatus
		 g_cMcuVcApp.GetPeriEqpStatus(tRegReq.GetEqpId(), &tHduStatus);
		 tHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetStatus(THduChnStatus::eREADY);
		 // [2013/03/11 chenbing] �����HDU֧�ֵ����ͨ����
		 tHduStatus.m_tStatus.tHdu.SetChnnlNum(tRegReq.GetChnnlNum());

		 g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);
		 //[nizhijun 2010/10/29]����HDU����(����HDU) begin
		 memset(&tTWPollParam, 0, sizeof(TTvWallPollParam));
		 if (m_tTWMutiPollParam.GetTWPollParam(tEqp.GetEqpId(), byLoop, tTWPollParam))
		 {
			 //ͨ����ѯ�У�HDU���ߺ������ߴ���
			 if (POLL_STATE_NORMAL == tTWPollParam.GetPollState())
			 {
				 cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				 SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
				 
				 u32 dwSetTimer = 0;
				 u32 dwTimerIdx = 0;
				 if(m_tTWMutiPollParam.GetTimerIdx(tEqp.GetEqpId(), byLoop, dwTimerIdx))
				 {
					 dwSetTimer = tTWPollParam.GetKeepTime();
					 SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, 1000 * dwSetTimer, dwTimerIdx);
				 }
				 else
				 {
					 ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF," [ProcHduConnectedNotif] Cannot HDU<EqpId:%d, ChnId:%d> getTimerIdx!\n",tEqp.GetEqpId(), byLoop);
				 }
				 
			 }
			 //ͨ����ѯ��ͣ�У�HDU���ߺ������ߴ���
			 else if(POLL_STATE_PAUSE == tTWPollParam.GetPollState())
			 {
				 cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				 SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
			 }
		 }
		 else
		 {
			// [2013/03/11 chenbing] �ķ���µĴ��� 
			if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byLoop) )
			{
				BOOL32 bIsChgHduMode = TRUE;
				//����HDUͨ��
				for (u8 byIndex = 0; byIndex < HDU_MODEFOUR_MAX_SUBCHNNUM; byIndex++)
				{
					g_cMcuVcApp.GetPeriEqpStatus(tRegReq.GetEqpId(), &tHduStatus);
					tInHduMt = (TMt)tHduStatus.m_tStatus.tHdu.GetChnMt(byLoop, byIndex);
					bySrcMtConfIdx = tHduStatus.m_tStatus.tHdu.GetConfIdx(byLoop, byIndex);
					bySchmeIdx     = tHduStatus.m_tStatus.tHdu.GetSchemeIdx(byLoop, byIndex);
					if (m_byConfIdx == bySrcMtConfIdx )
					{
						bIsChgHduMode = bIsChgHduMode ? TRUE : FALSE;
						if( !tInHduMt.IsNull() )
						{
							byMemberType = tHduStatus.m_tStatus.tHdu.GetMemberType(byLoop, byIndex);
							memset(&tHduSwitchInfo, 0, sizeof(THduSwitchInfo));
							tHduSwitchInfo.SetMemberType(byMemberType);
							tHduSwitchInfo.SetSrcMt(tInHduMt);
							tHduSwitchInfo.SetDstMt(tEqp);
							tHduSwitchInfo.SetDstChlIdx(byLoop);
							tHduSwitchInfo.SetSchemeIdx(bySchmeIdx);
							// [2013/03/11 chenbing] ׷����ͨ��
							tHduSwitchInfo.SetDstSubChn(byIndex);
							cServMsg.SetMsgBody((u8 *)&tHduSwitchInfo, sizeof(THduSwitchInfo));	
							cServMsg.SetEventId(MCS_MCU_START_SWITCH_HDU_REQ);
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU,  "[ProcHduConnectedNotif]srcmt(%d,%d) channel(%d)\n",
								tHduStatus.m_tStatus.tHdu.GetMcuId(byLoop, byIndex),
								tHduStatus.m_tStatus.tHdu.GetMtId(byLoop, byIndex),
								byLoop
								);

							if (byIndex)
							{
								tHduStatus.m_tStatus.tHdu.SetConfIdx(0, byLoop, byIndex);
							}
							tHduStatus.m_tStatus.tHdu.SetChnNull(byLoop, byIndex);
							tHduStatus.m_tStatus.tHdu.SetMemberType(TW_MEMBERTYPE_NULL, byLoop, byIndex);
							tHduStatus.m_tStatus.tHdu.SetSchemeIdx(0, byLoop, byIndex);
							g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);
							StartSwitchHduReq( cServMsg );
							bIsChgHduMode = FALSE;
							bIsChgHduPicture = FALSE;
						}
					}
					else
					{
						bIsChgHduMode = FALSE;
					}
				}//for over
				
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU,  "[ProcHduConnectedNotif]bIsChgHduMode (%d)\n", bIsChgHduMode);
				// �ķ��ͨ�������ն�ʱҲҪ�ָ�Ϊ�ķ��
				if ( bIsChgHduMode )
				{
					u8 byHduMode = HDUCHN_MODE_FOUR;
					CServMsg cSMsg;
					cSMsg.SetEqpId(tRegReq.GetEqpId());
					cSMsg.SetChnIndex(byLoop);
					cSMsg.SetMsgBody((u8 *)&byHduMode, sizeof(u8));
					SendChangeHduModeReq(cSMsg);
				}
			
				continue;
			}

			// [2013/03/11 chenbing]  һ����µĴ��� 
			tInHduMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop];
			bySrcMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
			bySchmeIdx     = tHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].GetSchemeIdx();
			if (m_byConfIdx == bySrcMtConfIdx && !tInHduMt.IsNull())
			{
				byMemberType = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType;
				if (VCS_CONF == m_tConf.GetConfSource() && ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
					m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE) 
				{
					tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
					tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
					tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = TW_MEMBERTYPE_NULL;
					g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);
					ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU,"[ProcHduConnectedNotif]VCS GROUP REVIEWTW MODE AND Mt(%d,%d) not online,So Continue!\n",
						tInHduMt.GetMcuId(),tInHduMt.GetMtId());
					continue;
				}
				memset(&tHduSwitchInfo, 0, sizeof(THduSwitchInfo));
				tHduSwitchInfo.SetMemberType(byMemberType);
				tHduSwitchInfo.SetSrcMt(tInHduMt);
				tHduSwitchInfo.SetDstMt(tEqp);
				tHduSwitchInfo.SetDstChlIdx(byLoop);
				tHduSwitchInfo.SetSchemeIdx(bySchmeIdx);
				cServMsg.SetMsgBody((u8 *)&tHduSwitchInfo, sizeof(THduSwitchInfo));	
				cServMsg.SetEventId(MCS_MCU_START_SWITCH_HDU_REQ);     
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU,  "[ProcHduConnectedNotif]srcmt(%d,%d) channel(%d)\n",
					 tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetMcuId(),
					 tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetMtId(),
					 byLoop
					 );

				//[nizhijun 2010/11/23]����Bug00041995������ȡ��tHduSwitchInfo���轫tmt������£�
				//��Ϊ��changhduswitch������Ǽ�������������tmt���ڵĻ�������ո��ն˻ش�����Ϣ
				tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
				tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
				tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = TW_MEMBERTYPE_NULL;
				tHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetSchemeIdx(0);
				g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);
				StartSwitchHduReq( cServMsg );
				bIsChgHduPicture = FALSE;
			}
		 }
     }

	 // �ı�HDU����ͼƬ
	 //Bug00159500:HDU2-S--����ͼƬ����Ϊ����������ǽ�����󣬱���ͼ���ΪĬ��ͼ��
	 if ( bIsChgHduPicture )
	 {
		 TEqpExCfgInfo tEqpExCfgInfo;
		 u16 wRet = g_cMcuAgent.GetEqpExCfgInfo( tEqpExCfgInfo );
		 if (SUCCESS_AGENT != wRet)
		 {
			 ConfPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcHduConnectedNotif] McuAgent GetEqpExCfgInfo failed. ret:%d!\n", wRet);
			 return;
		 }
		
		 u8 byVidPlayPolicy = tEqpExCfgInfo.m_tHDUExCfgInfo.m_byIdleChlShowMode;
		 OspPost(MAKEIID(AID_MCU_PERIEQPSSN, tRegReq.GetEqpId()), MCU_HDU_CHANGEPLAYPOLICY_NOTIF, (void*)&byVidPlayPolicy, sizeof(u8));
     }
}

/*====================================================================
    ������      ��ProcHduDisconnectedNotif
    ����        ��Hdu����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/01/20    4.6         ���ֱ�        ����
====================================================================*/
void CMcuVcInst::ProcHduDisconnectedNotif( const CMessage * pcMsg )
{
	if (STATE_ONGOING != CurState())
    {
        return;
    }
	
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();
	
	u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(tEqp);
	if (0 == byHduChnNum)
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[ProcHduDisconnectedNotif] GetHduChnNumAcd2Eqp failed!\n");
		return;
	}

    TPeriEqpStatus tHduStatus;
    g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);

    u8 bySrcMtConfIdx = 0;

	for (u8 byLoop = 0; byLoop < byHduChnNum; byLoop++)
    { 
		bySrcMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
		// [2013/03/11 chenbing] �ķ�������ÿ����ͨ��״̬ 
		if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byLoop) )
		{
			BOOL32 bIsStopSwitch = FALSE;
			for (u8 byIndex=0; byIndex<HDU_MODEFOUR_MAX_SUBCHNNUM; byIndex++)
			{
				if ( m_byConfIdx == bySrcMtConfIdx )
				{
					TMt tChnMt = tHduStatus.m_tStatus.tHdu.GetChnMt(byLoop, byIndex);
					RefreshMtStatusInTw(tChnMt, FALSE, TRUE);
					ChangeMtResFpsInHduVmp(tChnMt, tEqp.GetEqpId(), byLoop, byIndex, bIsStopSwitch, FALSE);
					StopSwitchToPeriEqp(tEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO, SWITCH_MODE_BROADCAST, byIndex);
					// ������ش�֧�֣����ﳢ���ͷŻش���Դ
					FreeRecvSpy( tChnMt, MODE_VIDEO );
				}
				tHduStatus.m_tStatus.tHdu.SetChnStatus(byLoop, byIndex, THduChnStatus::eIDLE);
			}
		}
		else
		{
			tHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetStatus(THduChnStatus::eIDLE);
			if (m_byConfIdx == bySrcMtConfIdx)
			{
				TMt tChnMt = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop];
				RefreshMtStatusInTw(tChnMt, FALSE, TRUE);
				StopSwitchToPeriEqp(tEqp.GetEqpId(), byLoop);
				
				//  [12/9/2009 pengjie] Modify ������ش�֧�֣����ﳢ���ͷŻش���Դ
				FreeRecvSpy( tChnMt, MODE_BOTH );
				
				// [8/30/2011 liuxu] �������ߺ�, ����ѯ�ն˺ܿ����Ѿ�����
				if (TW_MEMBERTYPE_POLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType)
				{
					//��ѵ��Ƶ�������Բ��䣬�ն���գ����������鲻���壬�Ա����ߺ���Լ�����ѵ��Ƶ����
					tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
				}
				if (VCS_CONF == m_tConf.GetConfSource() && ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
					m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE)
				{
					//VCS���Ԥ������ǽģʽ�£�����ǽ���ߺ������߲����ָ�
					tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
					tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
				}
				if(VCS_CONF == m_tConf.GetConfSource() && m_cVCSConfStatus.GetMtInTvWallCanMixing() && 
					!(tChnMt == m_cVCSConfStatus.GetCurVCMT()) && 
					(
					(tChnMt.IsLocal() && m_ptMtTable->IsMtInMixing(tChnMt.GetMtId())) || 
					(!tChnMt.IsLocal() && m_ptConfOtherMcTable->IsMtInMixing(tChnMt))
					) &&
					IsMtNotInOtherTvWallChnnl(tChnMt,0,0) && IsMtNotInOtherHduChnnl(tChnMt,tEqp.GetEqpId(),0,TRUE)
					)
				{
					RemoveSpecMixMember(&tChnMt,1);
				}
			}
	    }
	}
	g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);
    //zjlhdupoll
	// �������ǽ��ͨ����ѯ
	for(u8 byIdx = 0; byIdx < byHduChnNum; byIdx++)
	{
		TTvWallPollParam tTWPollParam;
		memset(&tTWPollParam, 0, sizeof(TTvWallPollParam));
		if (m_tTWMutiPollParam.GetTWPollParam(tEqp.GetEqpId(), byIdx, tTWPollParam) &&
			POLL_STATE_NONE != tTWPollParam.GetPollState())
		{
			TMtPollParam tMtPollParam;
			memset(&tMtPollParam, 0, sizeof(tMtPollParam));
			tTWPollParam.SetMtPollParam(tMtPollParam);
			tTWPollParam.SetPollState(POLL_STATE_NONE);
			
			cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
			SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
			
			u32 dwTimerIdx = 0;
			if (!m_tTWMutiPollParam.GetTimerIdx(tEqp.GetEqpId(), byIdx, dwTimerIdx))
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcTvwallDisconnectedNotif] Cannot get HDU<EqpId:%d, ChnId:%d> TimerIdx!\n",
					tEqp.GetEqpId(), byIdx);
			} 
			KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
		}
	}

	return;	
}
/*====================================================================
    ������      ��CreateConfEqpInsufficientNack
    ����        �����費���ͳһ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/05/12    4.0         �ű���        ����
====================================================================*/
void CMcuVcInst::CreateConfEqpInsufficientNack( u8 byCreateMtId, u8 byOldInsId, const s32 nErrCode, CServMsg &cServMsg )
{    
    if( CONF_CREATE_MT == cServMsg.GetSrcMtId() )
    {
        // guzh [7/11/2007] �ȷ���Ϣ��ɾ��
        CServMsg cMtMsg;
        SendMsgToMt( byCreateMtId, MCU_MT_DELMT_CMD, cMtMsg );

        // xsl [11/10/2006] �ն˴���ʧ��ʱ�ͷ���Դ
        g_cMcuVcApp.DecMtAdpMtNum( m_ptMtTable->GetDriId(byCreateMtId), m_byConfIdx, byCreateMtId );
		m_ptMtTable->SetDriId(byCreateMtId, 0);   
		
		// xliang [2/4/2009] �����ն�������MCU�����Σ�������ն˼���ֵ����-1
// 		u8 byMtType = m_ptMtTable->GetMtType(byCreateMtId);
// 		if( MT_TYPE_MMCU == byMtType || MT_TYPE_SMCU == byMtType )
// 		{
// 			g_cMcuVcApp.DecMtAdpMtNum( m_ptMtTable->GetDriId(byCreateMtId), m_byConfIdx, byCreateMtId, FALSE,TRUE );
// 		}

		g_cMcuVcApp.UpdateAgentAuthMtNum();
    }

	//�ͷ�prs
	ReleaseAllPrsChn();
	//�ͷ�bas
	RlsAllBasForConf();
	// �ͷ�Prsͨ��
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnl )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnl = EQP_CHANNO_INVALID;
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnl2 )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl2].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnl2 = EQP_CHANNO_INVALID;
// 	}
// 		
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlAud )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAud].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlAud = EQP_CHANNO_INVALID;
// 	}
// 	
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlVidBas )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVidBas].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlVidBas = EQP_CHANNO_INVALID;
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlBrBas )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlBrBas].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlBrBas = EQP_CHANNO_INVALID;
// 	}
// 	
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlAudBas )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAudBas].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlAudBas = EQP_CHANNO_INVALID;
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut1 )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut1].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlVmpOut1 = EQP_CHANNO_INVALID;
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut2 )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut2].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlVmpOut2 = EQP_CHANNO_INVALID;
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut3 )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut3].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlVmpOut3 = EQP_CHANNO_INVALID;
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut4 )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut4].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlVmpOut4 = EQP_CHANNO_INVALID;
// 	}

	// �ͷ�Basͨ��
// 	if( EQP_CHANNO_INVALID != m_byAudBasChnnl )
// 	{				
//         g_cMcuVcApp.SetBasChanReserved(m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl, FALSE);
//         m_byAudBasChnnl = EQP_CHANNO_INVALID;
// 		memset( &m_tAudBasEqp, 0, sizeof(m_tAudBasEqp) );
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byVidBasChnnl )
// 	{
//         g_cMcuVcApp.SetBasChanReserved(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, FALSE);	
// 		m_byVidBasChnnl = EQP_CHANNO_INVALID;
// 		memset( &m_tVidBasEqp, 0, sizeof(m_tVidBasEqp) );
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byBrBasChnnl )
// 	{
//         g_cMcuVcApp.SetBasChanReserved(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, FALSE);			
//         m_byBrBasChnnl = EQP_CHANNO_INVALID;
// 		memset( &m_tBrBasEqp, 0, sizeof(m_tBrBasEqp) );
// 	}

	{   // vmp
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tStatus );
		if( TVmpStatus::RESERVE == tStatus.m_tStatus.tVmp.m_byUseState )
		{
			tStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;//��ռ��,��ʱ��δ�ɹ��ٷ���
			tStatus.SetConfIdx( m_byConfIdx );
			g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tStatus );
		}
	}

	CreateConfSendMcsNack(byOldInsId, nErrCode, cServMsg, TRUE);
}

/*====================================================================
    ������      ��CreateConfSendMcsNack
    ����        ���������뷵��NACK��MCS
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/05/15    4.0         �ű���        ����
====================================================================*/
void CMcuVcInst::CreateConfSendMcsNack( u8 byOldInsId, const s32 nErrCode, CServMsg &cServMsg, BOOL32 bDeleteAlias )
{
	if (CONF_CREATE_SCH == cServMsg.GetSrcMtId())
	{
		//[2011/11/23/zhangli]mcs��������ƣ����m_cConfId������򲻴���������ʾ��������ľ��档Ϊ�˽��������⣬
		//mcs��ӡ�m_cConfId!=NULL��һ������ʾ�����������ﲻ����cSMsg��m_cConfId��ֱ���·���Ϣ
		//NotifyMcsAlarmInfo(0, nErrCode);
		CServMsg cSMsg;
		cSMsg.SetEventId(MCU_MCS_ALARMINFO_NOTIF);
		cSMsg.SetErrorCode(nErrCode);
		cSMsg.SetMcuId(LOCAL_MCUID);
		CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ALARMINFO_NOTIF, cSMsg.GetServMsg(), cSMsg.GetServMsgLen());
	}
	else
	{
		cServMsg.SetErrorCode(nErrCode);
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
	}

    if (bDeleteAlias)
    {
        DeleteAlias();
    }    
    
    g_cMcuVcApp.SetConfMapInsId(m_byConfIdx, byOldInsId);

	ClearVcInst();
    return;
}

/*====================================================================
    ������      ��AddSecDSCapByDebug
    ����        ������debug�����Ƿ���Ҫ�ڶ�˫����������һ˫��������ΪH239/H264��
    �㷨ʵ��    ����ʱд��Ϊ��h263+ XGA 5fps
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/05/15    4.0         �ű���        ����
====================================================================*/
void CMcuVcInst::AddSecDSCapByDebug(TConfInfo &tConfInfo)
{
    if (!g_cMcuVcApp.IsSupportSecDSCap())
    {
        return;
    }

    if (MEDIA_TYPE_H264 == tConfInfo.GetDStreamMediaType() &&
        tConfInfo.GetCapSupport().IsDStreamSupportH239())
    {
        TCapSupportEx tCapEx = tConfInfo.GetCapSupportEx();

        tCapEx.SetSecDSBitRate(tConfInfo.GetBitRate()); //ͬ��һ·˫��������ȫ����
        tCapEx.SetSecDSFrmRate(5);
        tCapEx.SetSecDSType(MEDIA_TYPE_H263PLUS);
        tCapEx.SetSecDSRes(VIDEO_FORMAT_XGA);

        tConfInfo.SetCapSupportEx(tCapEx);
        ConfPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[AddSecDSCapByDebug] add dual ds cap<h263+, XGA, 5fps>\n");
    }
    return;
}


/*=============================================================================
�� �� ���� NotifyMcsAlarmInfo
��    �ܣ� ֪ͨmcs��ʾ��Ϣ��Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u16 wErrorCode
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/8/9  4.0			������                  ����
=============================================================================*/
void CMcuVcInst::NotifyMcsAlarmInfo(u8 byMcsId, u16 wErrorCode)
{
    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_MCS_ALARMINFO_NOTIF);
    cServMsg.SetErrorCode(wErrorCode);
	// ��д���������ͻ����, zgc, 2007-03-26
	u16 wInsID = GetInsID();
	if( DAEMON == wInsID )
	{
		cServMsg.SetConfIdx( 0 );
	}
	else
	{
		cServMsg.SetConfIdx( m_byConfIdx );
		cServMsg.SetConfId( m_tConf.GetConfId() );
	}

    if (0 == byMcsId)
    {
        SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cServMsg);
    }
    else
    {
        SendMsgToMcs(byMcsId, MCU_MCS_ALARMINFO_NOTIF, cServMsg);
    }    

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MCS, "NotifyMcsAlarmInfo(%d, %d)\n", byMcsId, wErrorCode);

    return;
}

/*====================================================================
    ������      ��ProcTimerAutoRec
    ����        ��������鿪ʼ���Զ�¼��ʱ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/07/18    4.0         ����        ����
====================================================================*/
void CMcuVcInst::ProcTimerAutoRec( const CMessage * pcMsg )
{
    // ������ʱ��
    KillTimer( pcMsg->event );

    BOOL32 bMsgStart = FALSE;
    BOOL32 bMsgEnd = FALSE;

    time_t tiCurTime = time(NULL);
    
    TConfAutoRecAttrb tAttrib = m_tConf.GetAutoRecAttrb();

    time_t tiStartTime = 0;
    time_t tiEndTime = 0;
    tAttrib.GetStartTime().GetTime(tiStartTime);
    TKdvTime kdvEndTime = tAttrib.GetEndTime();
    kdvEndTime.GetTime( tiEndTime );

    if ( tiStartTime <= tiCurTime || tiStartTime - tiCurTime <= 10)
    {
        // Ӧ�ÿ�ʼ�Զ�¼����
        bMsgStart = TRUE;
    }
    if (kdvEndTime.GetYear() != 0)
    {
        if ( tiEndTime <= tiCurTime || tiEndTime - tiCurTime <= 10 )
        {
            // Ӧ�ý���¼����
            bMsgEnd = TRUE;
        }
    }

    if (bMsgStart && !bMsgEnd)
    {
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "Time to Start Auto Record...\n");
        
        // ����Ѿ���¼������Ǳ���Ϣ��
        if ( m_tConf.m_tStatus.IsNoRecording() )
        {    
			TMtAlias tVrsCallAlias;
			BOOL32 bIsVrsRecorder = GetVrsCallAliasByVrsCfgId(tAttrib.GetRecEqp(), tVrsCallAlias);
            // ���¼�������
            if (bIsVrsRecorder || g_cMcuVcApp.IsRecorderOnline(tAttrib.GetRecEqp()) )
            {
                // ����һ����ʼ¼����Ϣ MCS_MCU_STARTREC_REQ��Ϣ���ݣ�
				// TMt + TEqp + TRecStartPara + ¼���� + TSimCapSet + TVideoStreamCap + TAudioTypeDesc + tMtalias + �û���Id
                CServMsg cServMsg;
                TMt tMt;
                tMt.SetNull();
                cServMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));
                TEqp tRec;
				// vrs����eqp��ʶ
				if (!bIsVrsRecorder)
				{
					tRec.SetMcuEqp( LOCAL_MCUID, tAttrib.GetRecEqp(), EQP_TYPE_RECORDER );
				}
                cServMsg.CatMsgBody((u8*)&tRec, sizeof(TEqp));

                TRecStartPara tParam = tAttrib.GetRecParam();
                cServMsg.CatMsgBody((u8*)&tParam, sizeof(TRecStartPara));
                s8 aszRecName[KDV_MAX_PATH] = {0};
    
                // xsl [8/17/2006] ����ϲ���ָ��¼���ļ������������ģ��ֻ����һ�Σ��ڶ��ξͻ���¼���ļ����ظ������»���¼��ʧ��
//                if (strlen( tAttrib.GetRecName() ) == 0 )
//                {
                    // �û�û��Զ�����ʱ�����
                    time_t tTime = time(NULL);
                    TKdvTime tKdvTime;
                    tKdvTime.SetTime( &tTime );

					//  [5/31/2013 guodawei] �Զ�¼����Ӧ������
					u16 wBitrate = 0;
					if (tParam.IsRecLowStream())
					{
						wBitrate = m_tConf.GetSecBitRate() + GetAudioBitrate(m_tConf.GetMainAudioMediaType());
					}
                    else
					{
						wBitrate = m_tConf.GetBitRate() + GetAudioBitrate(m_tConf.GetMainAudioMediaType());
					}
					
					//���������е��»���'_'��Ϊ¼���ļ����ƣ����ڷǷ��ַ�����'-'�滻[6/28/2012 chendaiwei]
					s8 achConfName[MAXLEN_CONFNAME+1] = {0};
					memcpy(achConfName,m_tConf.GetConfName(),MAXLEN_CONFNAME+1);
					for( u8 byIdx = 0; byIdx < MAXLEN_CONFNAME+1; byIdx++)
					{
						if(achConfName[byIdx] == '_')
						{
							achConfName[byIdx] = '-';
						}
					}
			
                    sprintf(aszRecName, "%s-%04d-%02d-%02d %02d-%02d-%02d_%d_%dK",
                            achConfName,
                            tKdvTime.GetYear(),
                            tKdvTime.GetMonth(),
                            tKdvTime.GetDay(), 
                            tKdvTime.GetHour(),
                            tKdvTime.GetMinute(),
                            tKdvTime.GetSecond(),
                            m_tConf.GetUsrGrpId(),
                            wBitrate );
					
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "Rec filname is %s\n",aszRecName);

//                }
//                else
//                {
//                    strncpy(aszRecName, tAttrib.GetRecName(), KDV_MAX_PATH - 1);
//                }
				if (bIsVrsRecorder)
				{
					// vrs��¼���������Ȱ�129����
					aszRecName[KDV_NAME_MAX_LENGTH] = 0;
					cServMsg.CatMsgBody((u8*)aszRecName, KDV_NAME_MAX_LENGTH+1);
				}
				else
				{
					// ¼�������������Ȱ�128����
					aszRecName[KDV_NAME_MAX_LENGTH-1] = 0;
					cServMsg.CatMsgBody((u8*)aszRecName, KDV_NAME_MAX_LENGTH);
				}

				// ������������¿�
				TSimCapSet tSimCap;
                cServMsg.CatMsgBody((u8*)&tSimCap, sizeof(tSimCap));
				// ˫����������¿�
				TVideoStreamCap tSecCapSet;
                cServMsg.CatMsgBody((u8*)&tSecCapSet, sizeof(tSecCapSet));
				// ��Ƶ��������¿�
				TAudioTypeDesc tAudCapSet;
                cServMsg.CatMsgBody((u8*)&tAudCapSet, sizeof(tAudCapSet));
				// tMtalias(��¼��alias)
                cServMsg.CatMsgBody((u8*)&tVrsCallAlias, sizeof(tVrsCallAlias));

                cServMsg.SetEventId(MCS_MCU_STARTREC_REQ);
                cServMsg.SetConfIdx(m_byConfIdx);
                cServMsg.SetConfId( m_tConf.GetConfId() );
                cServMsg.SetNoSrc();

				CMessage cMsg;
				cMsg.length = cServMsg.GetServMsgLen();
				cMsg.content = cServMsg.GetServMsg();
				ProcMcsMcuStartRecReq(&cMsg);
//                 g_cMcuVcApp.SendMsgToConf(m_byConfIdx, 
//                                           MCS_MCU_STARTREC_REQ, 
//                                           cServMsg.GetServMsg(),
//                                           cServMsg.GetServMsgLen());
            }

        }

        // ���ý����Զ�¼��ʱ��
        if ( kdvEndTime.GetYear() != 0 && 
            g_cMcuVcApp.IsRecorderOnline(tAttrib.GetRecEqp()) )
        {
            // guzh [4/17/2007] ���þ��Զ�ʱ��
            SetAbsTimer(MCUVC_CONFSTARTREC_TIMER, 
                        kdvEndTime.GetYear(),
                        kdvEndTime.GetMonth(),
                        kdvEndTime.GetDay(),
                        kdvEndTime.GetHour(),
                        kdvEndTime.GetMinute(),
                        kdvEndTime.GetSecond() );
        }

        return;
    }

    if (bMsgEnd)
    {
        // �Զ�����ʱ�䵽
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "Time to End Auto Record...\n");

        // �������¼������Ǳ���Ϣ��
        if (!m_tConf.m_tStatus.IsNoRecording())
        {
            CServMsg cServMsg;
            cServMsg.SetEventId(MCS_MCU_STOPREC_REQ);
            cServMsg.SetConfIdx(m_byConfIdx);
            cServMsg.SetConfId( m_tConf.GetConfId() );
            cServMsg.SetNoSrc();

            TMt tMt;
            tMt.SetNull();
            cServMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));

            g_cMcuVcApp.SendMsgToConf(m_byConfIdx, 
                                      MCS_MCU_STOPREC_REQ, 
                                      cServMsg.GetServMsg(),
                                      cServMsg.GetServMsgLen());

        }

		//  [12/26/2009 pengjie] Modify ���ﲻ�ܾ���������¼���ˣ���Ȼ���컹Ҫ¼��Ļ�û�ж�ʱ��ȥ������
		u16 wErrCode = 0;
		SetAutoRec(wErrCode);
		// End Modify

        return;
    }

    // �����������Ӧ�÷���
    u32 dwTimerSpace = 1000;
    if ( tiStartTime > tiCurTime )
    {
        dwTimerSpace = (tiStartTime - tiCurTime) * 1000;        
        SetTimer( MCUVC_CONFSTARTREC_TIMER, dwTimerSpace );
    }
    else if ( tiEndTime >  tiCurTime )
    {
        dwTimerSpace = (tiEndTime - tiCurTime) * 1000;        
        SetTimer( MCUVC_CONFSTARTREC_TIMER, dwTimerSpace );
    }    
}

/*=============================================================================
�� �� ���� ProcTimerUpdataAutoRec
��    �ܣ� ˢ���Զ�¼��ʱ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
20091225    4.0			pengjie                create
=============================================================================*/
void CMcuVcInst::ProcTimerUpdataAutoRec( const CMessage * pcMsg )
{
	// ������ʱ��
    KillTimer( pcMsg->event );

	u16 wErrCode = 0;     
	SetAutoRec( wErrCode );

	return;
}

/*=============================================================================
�� �� ���� ProcTimerConfHasBrdSrc
��    �ܣ� ���ö�ʱ��3�����޹㲥Դ������ͣ¼�񣨽��治��֪��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
20100929    4.6			pengjie                create
=============================================================================*/
void CMcuVcInst::ProcTimerConfHasBrdSrc( void )
{
	KillTimer( MCUVC_CHECK_CONFBRDSRC_TIMER );

	// [12/15/2010 liuxu]�����ʱ����¼�������, ���޲���
	if (m_tConf.m_tStatus.IsNoRecording())
	{
		return;
	}

	CServMsg cServMsg;
	TMt tMt;
	tMt.SetNull();
	cServMsg.SetChnIndex( m_byRecChnnl );
	cServMsg.SetMsgBody( (u8*)&tMt, sizeof( tMt ) );
	cServMsg.CatMsgBody( (u8*)&m_tRecEqp, sizeof( m_tRecEqp ) );

	BOOL32 bIsNeedPauseRec = FALSE;
	if( m_tRecPara.IsRecDStream() )
	{
		// ����Ҫ¼˫���������ж�˫��Դ
		if( GetVidBrdSrc().IsNull() && GetAudBrdSrc().IsNull() && m_tDoubleStreamSrc.IsNull() )
		{
			bIsNeedPauseRec = TRUE;
		}
	}
	else
	{
		if( GetVidBrdSrc().IsNull() && GetAudBrdSrc().IsNull() )
		{
			bIsNeedPauseRec = TRUE;
		}
	}

	if( bIsNeedPauseRec )
	{
		// [12/15/2010 liuxu] ��������Ѿ���ͣ,����Ҫ����ͣ
		if (! m_tConf.m_tStatus.IsRecPause())
		{
			// ���޹㲥Դ�����û���Ϊ�ڲ���ͣ¼��
			SendMsgToEqp( m_tRecEqp.GetEqpId(), MCU_REC_PAUSEREC_REQ, cServMsg );
			m_tConf.m_tStatus.SetRecPauseByInternal();
		}
	}
	else
	{
		if( m_tConf.m_tStatus.IsRecPauseByInternal() )
		{
			SendMsgToEqp( m_tRecEqp.GetEqpId(), MCU_REC_RESUMEREC_REQ, cServMsg );
		}
	}

	return;
}

/*=============================================================================
�� �� ���� ProcTimerDPalyTokenNtf
��    �ܣ� ˫�����񣬶�ʱ��239���ƻ�֪ͨ,ֻ��Կƴ�mcu
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
20100226    4.0			pengjie                create
=============================================================================*/
void CMcuVcInst::ProcTimer239TokenNtf( void )
{
	KillTimer( MCUVC_NOTIFYMCUH239TOKEN_TIMER );

	u8  byManuId;
	TMt tMt;
    for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( !m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
		{
			continue;
		}

        byManuId = m_ptMtTable->GetManuId( byLoop );
		// ֻ��mcu�����ƻ�֪ͨ
		if( byManuId == MT_MANU_KDCMCU )
		{
    		tMt = m_ptMtTable->GetMt( byLoop );
			NotifyH239TokenOwnerInfo( &tMt );
		}
	}

	//�������ö�ʱ��
	SetTimer( MCUVC_NOTIFYMCUH239TOKEN_TIMER, 10000);

	return;
}

/*=============================================================================
�� �� ���� SetAutoRec
��    �ܣ� �����ƶ�¼��
�㷨ʵ�֣� ����ǰ�ƶ�¼�������߼���ش�������Ϊһ�����������޸Ĳ��ܸ��춨ʱ¼������
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
20091225    4.0			pengjie                create
=============================================================================*/
BOOL32 CMcuVcInst::SetAutoRec( u16 &wErrCode )
{
    TConfAutoRecAttrb tAutoRecAttrb = m_tConf.GetAutoRecAttrb();
    if ( tAutoRecAttrb.IsAutoRec() )
    {
        BOOL32 bInvalid = FALSE;    // �Ƿ񲻽����Զ�¼��

        time_t tiCurTime = ::time(NULL);
        TKdvTime tCurTime;
        tCurTime.SetTime( &tiCurTime );

        // ���ȼ������Ƿ���Ҫ¼��
        tm *ptmWeekDay = ::localtime( &tiCurTime );
        if ( !tAutoRecAttrb.IsRecWeekDay(ptmWeekDay->tm_wday) )
        {                    
            // ���ܽ��첻��Ҫ¼��
            bInvalid = TRUE;
        }

        TKdvTime tRecStartTime = tAutoRecAttrb.GetStartTime();
        TKdvTime tRecEndTime = tAutoRecAttrb.GetEndTime();
        
        BOOL32 bStartInstant = (tRecStartTime.GetYear() == 0);  // �û��Ƿ����ó�������ʼ
        BOOL32 bEndManual = (tRecEndTime.GetYear() == 0);   // �û��Ƿ����ó��ֶ�����

        if ( bStartInstant )
        {
            // ���û��������ʼʱ�䣬��Ϊ��������ʼ
            tRecStartTime.SetTime( &tiCurTime );
        }

        // ģ��ֻ����ÿ���ʲôʱ�򣬲��������գ��ʵ����ɽ���
        time_t tiStartTime = 0;                    
        time_t tiEndTime = 0;
                   
        tRecStartTime.SetYear( tCurTime.GetYear() );
        tRecStartTime.SetMonth( tCurTime.GetMonth() );
        tRecStartTime.SetDay( tCurTime.GetDay() );     
        tRecStartTime.GetTime(tiStartTime);
        
        if (!bEndManual)
        {
            // ��������˽���ʱ�䣬������ɽ����ʱ��                    
            tRecEndTime.SetYear( tCurTime.GetYear() );
            tRecEndTime.SetMonth( tCurTime.GetMonth() );
            tRecEndTime.SetDay( tCurTime.GetDay() );                    
            tRecEndTime.GetTime(tiEndTime);
        }
        
        if ( (!bEndManual) && (tiStartTime > tiEndTime ) )
        {
            // ����ʱ��ȿ�ʼ�磬�������Զ�¼��
            bInvalid = TRUE;
        }

        if ( (!bEndManual) && (tiEndTime < tiCurTime) )
        {
            // �������ʱ���ѹ���������Ҫ��ʼ
            bInvalid = TRUE;
        }

        if (!bInvalid)
        {
            if (tiStartTime <= tiCurTime)
            {                            
                // ���ֿ�ʼʱ���ѹ�������Ҫ������ʼ
                tRecStartTime.SetTime( &tiCurTime );
            }
        }

        // ��¼�±��λ���¼��������ʼ/�����ľ���ʱ�䣬��������
        tAutoRecAttrb.SetStartTime( tRecStartTime );
        if ( !bEndManual )
        {
            tAutoRecAttrb.SetEndTime( tRecEndTime );
        }                    
        m_tConf.SetAutoRecAttrb( tAutoRecAttrb );
        
        // ���ö�ʱ����ʼ
        if (!bInvalid)
        {
            // ���һ�µ�ǰ¼����Ƿ����ߣ���������ߣ���ʾ���ܾ�����
            if ( !g_cMcuVcApp.IsRecorderOnline(tAutoRecAttrb.GetRecEqp()) )
            {
				RlsAllBasForConf();
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf %s Request Recorder %d is not online now.\n", 
                                m_tConf.GetConfName(), 
                                tAutoRecAttrb.GetRecEqp());
                wErrCode = ERR_MCU_CONFSTARTREC_MAYFAIL;
				return FALSE;
            }

        #ifdef _MINIMCU_
            if ( ISTRUE(m_byIsDoubleMediaConf) && tAutoRecAttrb.GetRecParam().IsRecLowStream() )
            {
                wErrCode = ERR_MCU_RECLOWNOSUPPORT;
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf.%s can't support auto record low stream\n", m_tConf.GetConfName() );
                return FALSE;
            }
        #endif                    
            // guzh [4/17/2007] ���þ��Զ�ʱ��
            if (tiStartTime <= tiCurTime)
            {
                SetTimer( MCUVC_CONFSTARTREC_TIMER, 10);
            }
            else
            {
                TKdvTime tKdvTime;
                tKdvTime.SetTime(&tiStartTime);   
                SetAbsTimer(MCUVC_CONFSTARTREC_TIMER, 
                            tKdvTime.GetYear(),
                            tKdvTime.GetMonth(),
                            tKdvTime.GetDay(),
                            tKdvTime.GetHour(),
                            tKdvTime.GetMinute(),
                            tKdvTime.GetSecond() );
            }    

			ConfPrint( LOG_LVL_DETAIL, MID_MCU_REC, "[SetAutoRec] Now Set Timer: MCUVC_CONFSTARTREC_TIMER!\n" );
        }
		//  [12/26/2009 pengjie] �����ʱ¼���ܸ������⣬������첻¼����ô���ö�ʱ���������賿0�㣬�����һ���Ƿ���Ҫ���л��鶨ʱ¼��
		else
		{
			TKdvTime tKdvNextDayNewTime;
			tm *ptmDay = ::localtime( &tiCurTime );
			ptmDay->tm_hour = 23;
			ptmDay->tm_min  = 59;
			ptmDay->tm_sec  = 59;
			time_t tiDayLastTime = ::mktime( ptmDay );
			tiDayLastTime = tiDayLastTime + 1;
			tKdvNextDayNewTime.SetTime( &tiDayLastTime );
			SetAbsTimer(MCUVC_UPDATA_AUTOREC_TIMER, 
				tKdvNextDayNewTime.GetYear(),
				tKdvNextDayNewTime.GetMonth(),
				tKdvNextDayNewTime.GetDay(),
				tKdvNextDayNewTime.GetHour(),
				tKdvNextDayNewTime.GetMinute(),
				tKdvNextDayNewTime.GetSecond() );
			ConfPrint( LOG_LVL_DETAIL, MID_MCU_REC, "[SetAutoRec] Now Set Timer:MCUVC_UPDATA_AUTOREC_TIMER!\n" );
		}	
    }

	return TRUE;
}

/*=============================================================================
�� �� ���� ProcMcsMcuSaveConfToTemplateReq
��    �ܣ� ���浱ǰ����Ϊģ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/4/20   4.0			�ܹ��                  ����
=============================================================================*/
void CMcuVcInst::DaemonProcMcsMcuSaveConfToTemplateReq( const CMessage * pcMsg )
{
	//u8 byLoop = 0;
	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byConfIdx = *(u8*)cServMsg.GetMsgBody();
	TTemplateInfo  tTemInfo;
	
	TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
	if (tMapData.IsValidConf())
	{
		//get tConfInfo
		if (NULL == g_cMcuVcApp.GetConfInstHandle(byConfIdx))
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[ProcMcsMcuSaveConfToTemplateReq]: GetConfInstHandle NULL! -- %d\n", m_byConfIdx);
			return;
		}
		CMcuVcInst* pcInstance = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
		if (!pcInstance)
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[ProcMcsMcuSaveConfToTemplateReq]: GetConfInstHandle NULL! -- %d\n", m_byConfIdx);
			return;
		}
		
		tTemInfo.m_tConfInfo = pcInstance->m_tConf;
		if (CONF_LOCKMODE_LOCK == tTemInfo.m_tConfInfo.m_tStatus.GetProtectMode())
		{
			tTemInfo.m_tConfInfo.m_tStatus.SetProtectMode(CONF_LOCKMODE_NONE);
		}
		
		//		// ����ǰ���еĻ���ϳɱ���Ϊģ��, zgc, 2007/04/25
		//		if( pcInstance->m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE ) 
		//		{
		//			tTemInfo.m_atVmpModule.EmptyMember();
		//			TVMPParam tTempParam = pcInstance->m_tConf.m_tStatus.GetVmpParam();
		//			tTemInfo.m_atVmpModule.SetVmpParam( tTempParam );
		//			u8 byMaxVmpMemberNum = tTempParam.GetMaxMemberNum();
		//			TVMPMember *pVmpMem = NULL;
		//			for( byLoop = 0; byLoop < byMaxVmpMemberNum; byLoop ++ )
		//			{
		//				pVmpMem = tTempParam.GetVmpMember( byLoop );
		//				tTemInfo.m_atVmpModule.SetVmpMember( byLoop, pVmpMem->GetMtId(), pVmpMem->GetMemberType() );
		//			}
		//		}
		//		else
		//		{
		//			
		//		}
		//
		//		// ����ǰʹ�õĶ໭�����ǽ����Ϊģ��, zgc, 2007/04/25
		//		if( pcInstance->m_tConf.m_tStatus.GetVmpTwMode != CONF_VMPTWMODE_NONE )
		//		{
		//
		//		}
		//		else
		//		{
		//
		//		}
		
		//���漴ʱ����ʱ����յ�ǰ����״̬�е�����״̬
        // guzh [11/7/2007] 
        tTemInfo.m_tConfInfo.m_tStatus.SetNoMixing();
        tTemInfo.m_tConfInfo.m_tStatus.SetVACing(FALSE);
        tTemInfo.m_tConfInfo.m_tStatus.SetNoPlaying();
        tTemInfo.m_tConfInfo.m_tStatus.SetNoRecording();
        tTemInfo.m_tConfInfo.m_tStatus.SetVMPMode(CONF_VMPMODE_NONE);
        tTemInfo.m_tConfInfo.m_tStatus.SetVmpTwMode(CONF_VMPTWMODE_NONE);
        tTemInfo.m_tConfInfo.m_tStatus.SetPollMode(CONF_POLLMODE_NONE);
		//[03/04/2010] zjl modify (����ǽ��ͨ����ѯ����ϲ�)
		//      tTemInfo.m_tConfInfo.m_tStatus.SetTvWallPollState(POLL_STATE_NONE);
		tTemInfo.m_tConfInfo.m_tStatus.SetTakeMode(CONF_TAKEMODE_TEMPLATE);
		
		//		tTemInfo.m_tMultiTvWallModule = pcInstance->m_tConfEqpModule.m_tMultiTvWallModule; 
		//		tTemInfo.m_atVmpModule = pcInstance->m_tConfEqpModule.GetVmpModule();
		tTemInfo.m_tMultiTvWallModule.Clear();
		tTemInfo.m_atVmpModule.EmptyMember();
		TConfAttrb tConfAttrib = tTemInfo.m_tConfInfo.GetConfAttrb();
		tConfAttrib.SetHasTvWallModule( FALSE );
		tConfAttrib.SetHasVmpModule( FALSE );
		tTemInfo.m_tConfInfo.SetConfAttrb( tConfAttrib );
		
		if(pcInstance)
			tTemInfo.m_byMtNum = pcInstance->m_ptMtTable->m_byMaxNumInUse;
		
        tTemInfo.EmptyAllTvMember();
        tTemInfo.EmptyAllVmpMember();
		
		//get alias array
        u8 byMemberType = 0;
		TMt tMt;
		TMtAlias tMtAlias;
		for (u8 byLoop = 0; pcInstance && byLoop < pcInstance->m_ptMtTable->m_byMaxNumInUse; byLoop++)
		{
			tMt = pcInstance->m_ptMtTable->GetMt(byLoop+1);
			if (!tMt.IsNull())
			{
				if (MT_TYPE_MMCU == tMt.GetMtType())
				{
					continue;
				}
				
				//����mtAliasTypeH320Alias���Ͳ�������������Ϣ�����Բ����б���
				if (pcInstance->m_ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeH320ID, &tMtAlias))
				{
					tTemInfo.m_atMtAlias[byLoop] = tMtAlias;			
				}
				else if (pcInstance->m_ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeTransportAddress, &tMtAlias))
				{
					tTemInfo.m_atMtAlias[byLoop] = tMtAlias;			
				}
				else 
				{
					if (pcInstance->m_ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeE164, &tMtAlias))
					{
						tTemInfo.m_atMtAlias[byLoop] = tMtAlias;			
					} 
					else 
					{
						if (pcInstance->m_ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeH323ID, &tMtAlias))
						{
							tTemInfo.m_atMtAlias[byLoop] = tMtAlias;
						}
					}
				}
				tTemInfo.m_awMtDialBitRate[byLoop] = pcInstance->m_ptMtTable->GetDialBitrate(byLoop+1);
				
				//����ģ��ӳ���ϵ
				TConfAttrb tConfAttrb = tTemInfo.m_tConfInfo.GetConfAttrb();
				if (tConfAttrb.IsHasTvWallModule())
				{
					for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL; byTvLp++)
                    {                            
                        u8 byTvId = pcInstance->m_tConfEqpModule.m_tTvWallInfo[byTvLp].m_tTvWallEqp.GetEqpId();
                        for(u8 byTvChlLp = 0; byTvChlLp < MAXNUM_TVWALL_CHNNL_INSMOUDLE; byTvChlLp++)
                        {
                            if(TRUE ==pcInstance-> m_tConfEqpModule.IsMtInTvWallChannel(byTvId, byTvChlLp, tMt, byMemberType) )
                            {
                                tTemInfo.SetMtInTvChannel(byTvId, byTvChlLp, byLoop+1, byMemberType);
                            }
                        }
                    }
				}
				if (tConfAttrb.IsHasVmpModule())
				{
                    for (u8 byVmpIdx = 0; byVmpIdx < MAXNUM_MPUSVMP_MEMBER; byVmpIdx++)
                    {
                        if( pcInstance->m_tConfEqpModule.IsMtAtVmpChannel(byVmpIdx, tMt, byMemberType) )
						{
							tTemInfo.m_atVmpModule.SetVmpMember(byVmpIdx, tMt.GetMtId(), byMemberType);
						}
                    }						
				}            
			}// if (!tMt.IsNull())
		}// for(u8 byLoop = 0; byLoop < m_ptMtTable->m_byMax...
	}// if(g_cMcuVcApp.GetConfMapData...	
	
	TConfMapData tConfMapData = g_cMcuVcApp.GetConfMapData( byConfIdx );
	
	tTemInfo.m_byConfIdx = byConfIdx;
	
	if ( tConfMapData.IsTemUsed() )
	{
		TTemplateInfo tOldTempInfo;
		g_cMcuVcApp.GetTemplate( byConfIdx, tOldTempInfo );
		tTemInfo.m_tConfInfo.SetConfId( tOldTempInfo.m_tConfInfo.GetConfId() );
		
		if ( !g_cMcuVcApp.ModifyTemplate( tTemInfo, TRUE ) )
		{
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSaveConfToTemplateReq]Modify template %s failed!\n", tTemInfo.m_tConfInfo.GetConfName() );
            return;
		}
		
		SendReplyBack(cServMsg, pcMsg->event+1);
        g_cMcuVcApp.TemInfo2Msg(tTemInfo, cServMsg);
        SendMsgToAllMcs(MCU_MCS_TEMSCHCONFINFO_NOTIF, cServMsg); 
	}
	else
	{
		//�����E164�����Ѵ��ڣ��ܾ� 
		if( g_cMcuVcApp.IsConfE164Repeat( tTemInfo.m_tConfInfo.GetConfE164(), TRUE ,FALSE) )
        {
            cServMsg.SetErrorCode( ERR_MCU_CONFE164_REPEAT );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSaveConfToTemplateReq]template %s E164 repeated and create failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
            return;
        }
		//�������Ѵ��ڣ��ܾ�
        if( g_cMcuVcApp.IsConfNameRepeat( tTemInfo.m_tConfInfo.GetConfName(), TRUE ,FALSE) )
        {
            cServMsg.SetErrorCode( ERR_MCU_CONFNAME_REPEAT );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSaveConfToTemplateReq]template %s name repeated and create failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
            return;
        } 
		
		if(!g_cMcuVcApp.AddTemplate(tTemInfo))
        {
			// ���صĴ�����
			cServMsg.SetErrorCode( ERR_MCU_TEMPLATE_NOFREEROOM );
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSaveConfToTemplateReq] add template %s failed\n", tTemInfo.m_tConfInfo.GetConfName());
			SendReplyBack(cServMsg, pcMsg->event+2);
			return;
		}                 
		cServMsg.SetConfId( tTemInfo.m_tConfInfo.GetConfId() );
		SendReplyBack(cServMsg, pcMsg->event+1);
		g_cMcuVcApp.TemInfo2Msg(tTemInfo, cServMsg);
		SendMsgToAllMcs(MCU_MCS_TEMSCHCONFINFO_NOTIF, cServMsg); 
	}
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "Save conf %s to template successfully!\n", tTemInfo.m_tConfInfo.GetConfName() );
	return;
}

/*==============================================================================
������    :  ProcMcsMcuVmpPriSeizeRsp
����      :  ������vmp��ռӦ����Ϣ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2008-12-12					Ѧ��							����
==============================================================================*/
void CMcuVcInst::ProcMcsMcuVmpPriSeizeRsp(const CMessage *pcMsg)
{
	STATECHECK;
	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	BOOL32 bNeedSeize = TRUE;

	TMt tSeizeMt = *(TMt*)cServMsg.GetMsgBody();	//���Ҫ��ռ��MT
	u8 byVmpId = cServMsg.GetEqpId();
	if (!IsValidVmpId(byVmpId) || !IsVmpIdInVmpList(byVmpId))
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuVmpPriSeizeRsp]Vmp(id:%d) is not in conf(%d).\n", byVmpId, m_byConfIdx );
	}
	
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	TVMPParam_25Mem  tVMPParam   = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);

	if (MCS_MCU_VMPPRISEIZE_ACK == pcMsg->event)
	{
		u8 bySeizedMtNum = (cServMsg.GetMsgBodyLen()-sizeof(TMt)) / sizeof(TSeizeChoice);//����ռ��MT��Ŀ
		TSeizeChoice tSeizeChoice[MAXNUM_MPU2VMP_E20_HDCHNNL];
		u8 byMtIdx=0;

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "Mt.(%u,%u) Seize Mt choice info:\n==================\n",tSeizeMt.GetMcuId(), tSeizeMt.GetMtId() );
		for(byMtIdx=0; byMtIdx<bySeizedMtNum; byMtIdx++)
		{
			tSeizeChoice[byMtIdx] = *(TSeizeChoice *)(cServMsg.GetMsgBody()+sizeof(TMt)+sizeof(TSeizeChoice)*byMtIdx);
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "Mt.(%u,%u): byStopVmp(%u), byStopSpeaker(%u), byStopSelected(%u)\n",
				tSeizeChoice[byMtIdx].tSeizedMt.GetMcuId(), tSeizeChoice[byMtIdx].tSeizedMt.GetMtId(), 
				tSeizeChoice[byMtIdx].byStopVmp,
				tSeizeChoice[byMtIdx].byStopSpeaker, 
				tSeizeChoice[byMtIdx].byStopSelected);
		}

		for(byMtIdx=0; byMtIdx<bySeizedMtNum; byMtIdx++)
		{
			// ���ݱ���ռѡ����в���
			ExecuteMtBeSeizedChoice(tVmpEqp, tVMPParam, tSeizeChoice[byMtIdx]);
		}

		if(bNeedSeize)
		{
			// vmpparam�����и��£����»�ȡvmpparam���緢���˸���ʱ��ȡ�������˻���·����˸���ͨ��
			tVMPParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			// ����ռ�ն˴���
			ExecuteMtSeizeChoice(tVmpEqp, tVMPParam, tSeizeMt);
		}
		else
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[ProcMcsMcuVmpPriSeizeRsp] no need to seize!\n");
		}
	}
	else 
	{	
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuVmpPriSeizeRsp] receive MCS_MCU_VMPPRISEIZE_NACK!\n");
		CServMsg cMsg;
		cMsg.SetEqpId(byVmpId);
		cMsg.SetMsgBody( (u8*)&tVMPParam, sizeof(tVMPParam) );
		SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cMsg ); //Nackʱ,���߻�ظ���VMPPARAM
	}
	// �����Ƿ���ռ,���m_atVMPTmpMember��¼��Ϣ
	TVmpPriSeizeInfo tPriSeiInfo;
	g_cMcuVcApp.SetVmpPriSeizeInfo(tVmpEqp, tPriSeiInfo);
}

/*==============================================================================
������    :  ExecuteMtBeSeizedChoice
����      :  ���ݱ���ռѡ����в���
�㷨ʵ��  :  
����˵��  :  const TEqp &tVmpEqp
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130531					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::ExecuteMtBeSeizedChoice(const TEqp &tVmpEqp, TVMPParam_25Mem &tVMPParam, TSeizeChoice &tSeizeChoice)
{
	//���tMt��vmp������λ��,����mcu�����ϴ�ͨ���ն˼���໥ӳ��
	u8 abyChnlNo[MAXNUM_VMP_MEMBER];
	u8 byChnlNum = 0;
	memset(abyChnlNo,0,sizeof(abyChnlNo));
	//���ݱ���ռѡ����в���
	//1,���ն��˳�����ϳ�
	if(tSeizeChoice.byStopVmp)
	{
		if (MODE_VIDEO == tSeizeChoice.byStopVmp)
		{
			// xliang [3/25/2009] ���Ը��ģ��ֲ���Ա������������ȫ������
			TMt tMtBeSeized = tSeizeChoice.tSeizedMt;
			//���tMtBeSeized��vmp������λ��,����mcuӳ���ϴ�ͨ���ն�
			GetChlOfMtInVmpParam(tVMPParam, tMtBeSeized, byChnlNum, abyChnlNo);
		} //�ر�˫������ͨ��
		else if (MODE_SECVIDEO == tSeizeChoice.byStopVmp)
		{
			byChnlNum = 1;
			abyChnlNo[0] = tVMPParam.GetChlOfMemberType(VMP_MEMBERTYPE_DSTREAM);
		}
	
		if (0 != byChnlNum)
		{
			u8	byChl = abyChnlNo[0];	//ǰ�����֧�ֵ���ͨ��
			// ���ͨ����Ա����ռ�õĶ�ش���Դ
			ClearOneVmpMember(tVmpEqp.GetEqpId(), byChl, tVMPParam);
		}
	}
	//2,���ն�ȡ��������
	if(tSeizeChoice.byStopSpeaker == 1)
	{
		BOOL32 bCancelSpeaker = TRUE; //�ܷ�ȡ��������

		//�����ݽ�ģʽ�������������Ʒ���״̬,����ָ��������
		if (m_tConf.m_tStatus.IsVACing())
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[ProcMcsMcuVmpPriSeizeRsp] Conf is VACing! Speaker cannot be canceled!\n");
			bCancelSpeaker = FALSE;
		}

		TMt tSpeaker = m_tConf.GetSpeaker();
		if( !m_tConf.HasSpeaker() )//û�з�����
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "No speaker in conference %s now! Cannot cancel the speaker!\n", 
				m_tConf.GetConfName() );
			bCancelSpeaker = FALSE;
		}
		if(bCancelSpeaker)
		{
			//ͣ�鲥
			if( m_ptMtTable->IsMtMulticasting( tSpeaker.GetMtId() ) )
			{
				if ( m_tConf.GetConfAttrb().IsMulticastMode() && tSpeaker == GetVidBrdSrc())
				{
					g_cMpManager.StopMulticast( tSpeaker, 0, MODE_VIDEO );
				}
				if ( m_tConf.GetConfAttrb().IsMulticastMode() && tSpeaker == GetAudBrdSrc())
				{
					g_cMpManager.StopMulticast( tSpeaker, 0, MODE_AUDIO );
				}
				
				m_ptMtTable->SetMtMulticasting( tSpeaker.GetMtId(), FALSE );
			}
			
			//ȡ��������
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMcsMcuVmpPriSeizeRsp]begin change speaker!\n");
			ChangeSpeaker( NULL );
		}

	}

	//3,���ն�ȡ����ѡ��
	if(tSeizeChoice.byStopSelected == 1)
	{
		TMtStatus	tMtStatus;
		TMt			tSrc;
		TMt			tDstMt;
		BOOL32		bSelectedSrc = FALSE; //�Ƿ����ڱ�ѡ��
		TMt tMtBeSeized = tSeizeChoice.tSeizedMt;
		TMt tSrcViewMt;
		//ȡ����������ѡ����ѡ��ԴMT�Ľ���
		for(u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			if(!m_tConfAllMtInfo.MtJoinedConf( byLoop ))
			{
				continue;
			}
			m_ptMtTable->GetMtStatus(byLoop,&tMtStatus);
			tSrc = tMtStatus.GetSelectMt( MODE_VIDEO ); //��ȡѡ��Դ
			// �����ն�ѡ������mcu,������ռ�ն������Ǹ�mcu���ϴ�ͨ���ն�,ͬ����Ҫȡ����mcu��ѡ��
			// ����:����mcu���ϴ�ͨ���ն���vmp��,�ö���mcu����һ�ն�ѡ��
			if (IsMcu(tSrc))
			{
				tSrcViewMt = GetSMcuViewMt(tSrc, TRUE);
			}
			if(tSrc == tMtBeSeized || tSrcViewMt == tMtBeSeized)//��MT��ѡ��Դ�Ǳ���ռ��MT,����ѡ��Դ��mcu
			{
				//ȡ��ѡ��
				bSelectedSrc = TRUE;
				tDstMt = m_ptMtTable->GetMt(byLoop);

				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMcsMcuVmpPriSeizeRsp]Cancel Mt.%u selectseeing Mt.%u.\n",
					byLoop,tSeizeChoice.tSeizedMt.GetMtId());

				StopSelectSrc(tDstMt, MODE_VIDEO);
				
				// ���÷�����ԴΪ��Դ
				if ( HasJoinedSpeaker() && tDstMt == GetLocalSpeaker() )
				{
					SetSpeakerSrcSpecType( MODE_BOTH, SPEAKER_SRC_NOTSEL );
				}
			}
		}

		if(!bSelectedSrc)
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMcsMcuVmpPriSeizeRsp]Mt.%u is not the selected source!\n",tSeizeChoice.tSeizedMt.GetMtId());
		}
	}

	return;
}

/*==============================================================================
������    :  ExecuteMtBeSeizedChoice
����      :  ���ݱ���ռѡ����в���
�㷨ʵ��  :  
����˵��  :  const TEqp &tVmpEqp
			 TVMPParam_25Mem &tVMPParam
			 const TMt &tSeizeMt
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130531					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::ExecuteMtSeizeChoice(const TEqp &tVmpEqp, TVMPParam_25Mem &tVMPParam, const TMt &tSeizeMt)
{
	//��ԭ��Ҫ��ռ��Mt���в����������ʱ����ʱ������
	u16 wTimer = 10;		
	//���tMt��vmp������λ��,����mcu�����ϴ�ͨ���ն˼���໥ӳ��
	u8 abyChnlNo[MAXNUM_VMP_MEMBER];
	u8 byChnlNum = 0;
	memset(abyChnlNo,0,sizeof(abyChnlNo));
	TVmpPriSeizeInfo tPriSeiInfo = g_cMcuVcApp.GetVmpPriSeizeInfo(tVmpEqp);
	// �ж��Ƿ���˫����ռ
	BOOL32 bDStreamSeize = FALSE;
	
	//���tSeizeMt��vmp������λ��,����mcuӳ���ϴ�ͨ���ն�
	GetChlOfMtInVmpParam(tVMPParam, tSeizeMt, byChnlNum, abyChnlNo);

	if (tSeizeMt == (TMt)tPriSeiInfo.m_tPriSeizeMember)
	{
		if (VMP_MEMBERTYPE_DSTREAM == tPriSeiInfo.m_tPriSeizeMember.GetMemberType())
		{
				bDStreamSeize = TRUE;
		}
	}
	u8 byIndex = tPriSeiInfo.m_byChlIdx;

	//����¼�һ����Ա��������ռ�����: ���µ���һ�κϳɳ�Ա
	if( (!tVMPParam.IsMtInMember(tSeizeMt) && 0 == byChnlNum) || bDStreamSeize)
	{	
		if ( byIndex < MAXNUM_VMP_MEMBER)
		{
			TVMPMember tVMPMember = tPriSeiInfo.m_tPriSeizeMember;
			// ˫������,�������ʵ˫��Դ,������ռǰ��smcu,��ʱ�ѿ��Ի����ʵ˫��Դ
			if (VMP_MEMBERTYPE_DSTREAM == tVMPMember.GetMemberType())
			{
				// ��ֹ��ʱ˫����ֹͣ
				if (m_tDoubleStreamSrc.IsNull())
				{
					tVMPMember.SetNull();
				}
				else
				{
					TMt tRealDSMt = GetConfRealDsMt();
					if (!tRealDSMt.IsNull())
					{
						tVMPMember.SetMemberTMt(tRealDSMt);
					}
				}
			}
			tVMPParam.SetVmpMember(byIndex, tVMPMember);
			
			AdjustVmpParam(tVmpEqp.GetEqpId(), &tVMPParam);
		}
	}
	else
	{
		//��abyChnlNo��ȡλ��,�����¼�mcu�����ϴ�ͨ���ն˼��໥ӳ��
		u8 byMemberIdx = abyChnlNo[0];//tVMPParam.GetChlOfMtInMember(tSeizeMt);
		if ( (TMt)tPriSeiInfo.m_tPriSeizeMember == tSeizeMt )
		{
			if(tPriSeiInfo.m_wEventId == MCS_MCU_STARTSWITCHMT_REQ)
			{
				CServMsg cSendMsg;
				cSendMsg.SetSrcSsnId(tPriSeiInfo.m_bySrcSsnId);//���������Ƿ�mcs��קѡ��
				cSendMsg.SetSrcMtId(0);
				cSendMsg.SetEventId(MCS_MCU_STARTSWITCHMT_REQ);
				cSendMsg.SetMsgBody((u8*)&tPriSeiInfo.m_tSwitchInfo, sizeof(TSwitchInfo));
				ProcStartSelSwitchMt(cSendMsg);
			}
			else
			{
				// [1/19/2010 xliang] SetTimer ��������������Ҫ��3����Ϣ��mcuId�� mtId�� byVmpid
				u32 nTimerParam = tSeizeMt.GetMcuId() * 100000 + tSeizeMt.GetMtId() * 100 + tVmpEqp.GetEqpId() ;
				SetTimer(MCUVC_MTSEIZEVMP_TIMER, wTimer, nTimerParam );
			}
		}
	}

	return;
}

/*==============================================================================
������    : ProcMtSeizeVmp 
����      : ��ռ��ʱ����
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2008-12-12					Ѧ��							����
==============================================================================*/
void CMcuVcInst::ProcMtSeizeVmpTimer(const CMessage *pcMsg)
{
// 	u8 bySeizeMtId  = (u8)((*((u32*)pcMsg->content))/100);		//֮ǰҪ��ռ���ն�id
// 	u8 bySeizeTimes  = (u8)((*((u32*)pcMsg->content))%100)+1;	//��ռ����

	u16 wSeizeMcuId = (u16)((*((u32*)pcMsg->content)) / 100000);	//֮ǰҪ��ռ���ն�McuId
	u32 nTmpRes  = ((*((u32*)pcMsg->content)) % 100000);	
	u8 bySeizeMtId  = (u8)(nTmpRes / 100) ;						//֮ǰҪ��ռ���ն�MtId
	u8 byVmpId  = (u8)(nTmpRes % 100);					//byVmpId
	if (!IsValidVmpId(byVmpId))
	{
		return;
	}
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );

	TMt tSeizeMt;

	if( LOCAL_MCUIDX == wSeizeMcuId )
	{
		tSeizeMt = m_ptMtTable->GetMt(bySeizeMtId);
	}
	else
	{
		tSeizeMt = m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx(wSeizeMcuId)); //���ô˽ӿ���һ��driId, confId, mcuId, mtId ��������
		tSeizeMt.SetMcuIdx( wSeizeMcuId );
		tSeizeMt.SetMtId(bySeizeMtId);
	}
	
	ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMtSeizeVmpTimer] Seize Mt Info: Mt.(%u,%u)-confIdx.%u \n", tSeizeMt.GetMcuId(), tSeizeMt.GetMtId(), tSeizeMt.GetConfIdx());

// 	BOOL32 bNeedSeizePromt = FALSE;	//�Ƿ���Ҫ���·���ռ��ʾ //���ֶ�Ŀǰ������
// 	if(bySeizeTimes > 3)
// 	{
// 		bNeedSeizePromt = TRUE;
// 	}
	//��ȡ֮ǰҪ��ռ���ն�vmpMemberType
	TVMPMember tVMPMember;
	tVMPMember.SetNull();
	TVMPParam_25Mem  tVMPParam   = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	u8         byMemberNum = tVMPParam.GetMaxMemberNum();
	u8	       byMemberType = ~0;
	for (u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
	{
		
		//ȡ����ϳɳ�Ա
		tVMPMember = *(tVMPParam.GetVmpMember(byLoop));
		if( tVMPMember.GetMcuIdx() == wSeizeMcuId 
			&& tVMPMember.GetMtId() == bySeizeMtId
			)
		{
			byMemberType = tVMPMember.GetMemberType();
			break;
		}
	}

	if(byMemberType == 0) //����ڳ�ʱ����֮ǰ����������ϳɱ�ֹͣ�ˣ���membertypeӦΪ0
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[ProcMtSeizeVmpTimer] vmp has been stopped,no need to change Mt.%u's format!\n",bySeizeMtId);
		return;
	}

	//ȡVMPstyle, channel idx
	u8 byVmpStyle = tVMPParam.GetVMPStyle();
	u8 byChlPos	= tVMPParam.GetChlOfMtInMember(tSeizeMt);

	TVMPParam_25Mem tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
	
	if(tLastVmpParam.IsMtInMember(tSeizeMt))//֮ǰ�Ѿ���VMP�У��ҽ��˽���.(���ĳ��Ա��Ϊ�����˵�������ݵ�����ռ�����)
	{
		//do nothing
		ChangeMtVideoFormat(tSeizeMt, FALSE);
	}
	else 
	{
		//���ֱ��ʽ�����
		ChangeMtResFpsInVmp(byVmpId, tSeizeMt, &tVMPParam, TRUE, byMemberType, byChlPos, FALSE);
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtSeizeVmpTimer] Mt.(%u,%u) Set Vmp channel.%u!\n", wSeizeMcuId, bySeizeMtId, byChlPos);
		/*if(bRet)
		{
			//����ͨ����������
			SetVmpChnnl(byVmpId, (TMt)tVMPMember, byChlPos, byMemberType);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtSeizeVmpTimer] Mt.(%u,%u) Set Vmp channel.%u!\n", wSeizeMcuId, bySeizeMtId, byChlPos);
		}*/
// 		else if(!bNeedSeizePromt) //�����Ҫ�ٴη���ռ��ʾ������Ͳ���Ҫ��ʱ��
// 		{	
// 			//������ʱ
// 			u32 nTimerParam = wSeizeMcuId * 100000 + bySeizeMtId * 100 + bySeizeTimes ;
// 			SetTimer(MCUVC_MTSEIZEVMP_TIMER, 1000, nTimerParam);	       
// 		}
	}
	
}

/*==============================================================================
������    :  CheckMpuAudPollConflct
����      :  �������Ƶ��ѯ
�㷨ʵ��  :  �������Ƶ��ѯʱ������ϳ�����֧����ѯ����ͷ����˸���ͬʱ����
����˵��  :  
����ֵ˵��:  u16 ����0��ʾ���������򷵻�errorcode
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-8-20					Ѧ��
==============================================================================*/
u16 CMcuVcInst::CheckMpuAudPollConflct(u8 byVmpId, TVMPParam_25Mem& tVmpParam, BOOL32 bChgParam /*= FALSE*/)
{
	u16 wRet = 0;
	TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
	if (!IsAllowVmpMemRepeated(byVmpId) &&
		CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode() 
		&& tPollInfo.GetMediaMode() == MODE_BOTH 
		&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER)
		&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
		)
	{
		if( !bChgParam )
		{
			// 			wErrorCode = ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
			// 			return FALSE;
			wRet = (u16)ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
		}
		else
		{
			TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
			TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			u8 byConflictMemType = 0;
			if (tConfVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL))
			{
				//�˴������˷����˸���,������Ϊ��ͻԴ
				byConflictMemType = VMP_MEMBERTYPE_SPEAKER;
			}
			else if(tConfVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER))
			{
				byConflictMemType = VMP_MEMBERTYPE_POLL;
			}
			else
			{
				//do nothing, maintain the variable byConflictMemType be 0
			}
			
			if(byConflictMemType != 0)
			{
				
				//����VMP param
				u8 byLoop;
				TVMPMember* ptVmpMember;
				for (byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop ++)
				{
					ptVmpMember = tVmpParam.GetVmpMember(byLoop);
					if (NULL == ptVmpMember)
					{
						continue;
					}
					if(ptVmpMember->GetMemberType() == byConflictMemType)
					{
						// xliang [4/3/2009] ��ش�������TVMPParam������modeֵ�ǿյģ����ﱣ��һ��
						tVmpParam.SetVMPMode(CONF_VMPMODE_CTRL);
						ClearOneVmpMember(tVmpEqp.GetEqpId(), byLoop, tVmpParam, TRUE);
						break;
					}
				}
				
				// 				wErrorCode = ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
				// 				return FALSE;
				wRet = (u16)ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
			}
		}
	}
	return wRet;
}

/*==============================================================================
������    :  CheckSameMtInMultChnnl
����      :  �����в�����ͬһ��MTռ�˶����Աͨ��
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  u16 ����0��ʾ���������򷵻�errorcode
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-8-20					Ѧ��							
==============================================================================*/
u16 CMcuVcInst::CheckSameMtInMultChnnl(u8 byVmpId, TVMPParam_25Mem & tVmpParam, BOOL32 bChgParam/* = FALSE*/)
{
	// xliang [3/12/2009] LOCAL_MCUID�����192���������,��MT.192��MCU�Ļ����ͻ����С�
	// һ�������к���192��mt��Ҳ�Ǻ�С���ʵġ� �ú��ݲ��Ķ������ܸĳ�0���ĳ�193�����������кγ��
	//zhouyiliang20101112��GetChlOfMtInMember������ظ�����
	//u8 abyMcMtInVMPStatis[MAXNUM_CONF_MT+1][MAXNUM_CONF_MT+1] = {0};
	u16 wRet = 0;
	BOOL32	bRepeatedVmpMem = FALSE;
	BOOL32  bRepeatedOtherMem = FALSE;
	BOOL32  bRepeatedMMcuMem = FALSE;
	BOOL32  bRepeatedG400IPCMtMem = FALSE;
	BOOL32  bRepeatedSpeMcuMem = FALSE; //�򵥼���MCU�Ƿ��ڶ�ͨ��
	u8 bySpeakerChnnl	= 0;
	u8 byPollChnnl		= 0;
	u8 byVmpPollChnnl	= 0;	//vmp��ͨ����ѯ����
	u8 byDStreamChnnl	= 0;	//˫������ͨ������
	u8 byMcuId = 0;
	TVMPMember tVMPMember;
	TVMPMember tLastVMPMember;
	TMt tRealMt;
	BOOL32 bIsChnlMemberChange = FALSE;//��¼ͨ����Ա�Ƿ��б�
	u8 byChlCount = 0;
	u8 abyChnlNo[MAXNUM_VMP_MEMBER];
	BOOL32 bStyleOK = FALSE;
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
	if (bChgParam) //��ǰ����Ƿ�ǰ�������
	{
		bStyleOK = tVmpChnnlInfo.GetMaxStyleNum() >= tVmpParam.GetMaxMemberNum();
	}

	for( u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		if (NULL == tVmpParam.GetVmpMember( byLoop ))
		{
			tVMPMember.SetNull();
		} 
		else
		{
			tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		}
		if (NULL == tConfVmpParam.GetVmpMember( byLoop ))
		{
			tLastVMPMember.SetNull();
		} 
		else
		{
			tLastVMPMember = *tConfVmpParam.GetVmpMember( byLoop );
		}
		
		// xliang [3/24/2009] membertype�Ĺ��ˣ�Ҫ����tVMPMember.IsNull()��
		// ���򵱷����˸��浫�������޷����˵�����£�����ʧЧ
		if(tVMPMember.GetMemberType() == VMP_MEMBERTYPE_SPEAKER)
		{
			bySpeakerChnnl ++;
			//�ڷ��֧��(ǰ�������)����£���������ͨ������֤�⳧���ն˿��Դ򿪸���ͨ��
			if (bStyleOK)
			{
				continue;
			}
		}
		if(tVMPMember.GetMemberType() == VMP_MEMBERTYPE_POLL)
		{
			byPollChnnl ++;
			//�ڷ��֧��(ǰ�������)����£���������ͨ������֤�⳧���ն˿��Դ򿪸���ͨ��
			if (bStyleOK)
			{
				continue;
			}
		}
		if(tVMPMember.GetMemberType() == VMP_MEMBERTYPE_VMPCHLPOLL)
		{
			byVmpPollChnnl ++;
		}
		if (tVMPMember.GetMemberType() == VMP_MEMBERTYPE_DSTREAM)
		{
			byDStreamChnnl ++;
			//����˫������ͨ��
			continue;
		}
		
		if(tVMPMember.IsNull()) 
		{
			continue;
		}

		//zhouyiliang20101112��GetChlOfMtInMember������ظ�����,�����һ��λ����
		//byMcuId =  tVMPMember.GetMcuIdx() ;
		//u8 byLastMemPos = tVmpParam.GetChlOfMtInMember( (TMt)tVMPMember,TRUE );
		tRealMt = tVMPMember;
		// ͨ������mcu,������ϴ�ͨ���ն�
		if (IsMcu(tVMPMember))
		{
			tRealMt = GetSMcuViewMt(tVMPMember, TRUE);
			if (tRealMt.IsNull())//���ϴ�ͨ���ն�
			{
				tRealMt = tVMPMember;
			}
		}
		// mcu��ӳ���ϴ�ͨ���ն���ͳ��
		if (IsMcu(tVMPMember) || !tVMPMember.IsLocal())
		{
			GetChlOfMtInVmpParam(tVmpParam, tVMPMember, byChlCount);
		}
		else
		{
			tVmpParam.GetChlOfMtInMember(tVMPMember,FALSE,MAXNUM_VMP_MEMBER,abyChnlNo,&byChlCount);
		}
		bIsChnlMemberChange = !(tVMPMember == tLastVMPMember && tVMPMember.GetMemberType() == tLastVMPMember.GetMemberType());
		//if( abyMcMtInVMPStatis[byMcuId][tVMPMember.GetMtId()] == 1)
		// ������¼��ı��ϴ�ͨ���ն˵��µ��ն��ڶ�ͨ��,��Ӱ������vmp����,����������mcu�����ϴ��ն˽�vmp
		if (bIsChnlMemberChange
			&& byChlCount > 1 )
		{
			bRepeatedVmpMem = TRUE;
			// �����ն��Ƿ����ߣ�ģ�濪������ϳ�ʱ���ն˲����ߣ��޷����ն��Ƿ�ΪKedaMt
			if (!m_tConfAllMtInfo.MtJoinedConf(tVMPMember))
			{
				// �������ն˲�����������check
				continue;
			}
			if (tVMPMember == m_tCascadeMMCU)
			{
				bRepeatedMMcuMem = TRUE;
				continue;
			}
			// �򵥼���mcu���ܽ���ͨ��
			if (IsNotSupportCascadeMcu(tVMPMember))
			{
				bRepeatedSpeMcuMem = TRUE;
				continue;
			}
			//�����Ƿ��зǿƴ��ն��ڶ��ͨ����
			if (!IsKedaMt(tVMPMember, TRUE) || !IsKedaMt(tVMPMember, FALSE))//�¼��ն�Ҫ��FALSE���зǿƴ�
			{
				bRepeatedOtherMem = TRUE;
				continue;
			}
			//�������һ��(�ϴ�ͨ���ն�Ϊ�ǿƴ��ն˵�)mcu,���д˷ǿƴ��ն��Ƿ���������ͨ��
			if (IsMcu(tVMPMember) 
				&& !IsKedaMt(tRealMt, FALSE) 
				)
			{
				bRepeatedOtherMem = TRUE;
				continue;
			}
			//IPCǰ���Ƿ����vmp��ͨ��
			if (IsG400IPCMt(tVMPMember))
			{
				bRepeatedG400IPCMtMem = TRUE;
			}
		}
//		else
//		{
//			abyMcMtInVMPStatis[byMcuId][tVMPMember.GetMtId()] ++ ;
//		}
	}
	// �ϳɿ���ʱ,��Other�ն�/G400IPCMt�ڶ��ͨ��ʱ������,�ճ�����(��ʱ�汾)
	// �ϳɸı�ʱ,��Other/G400IPCMt�ն��ڶ��ͨ��ʱ,ֱ�ӷ���nack,ȡ����θı�
	if (/*bChgParam && */
		(bRepeatedOtherMem || bRepeatedG400IPCMtMem))
	{
		wRet =  (u16)ERR_MCU_REPEATEDVMPNONEKEDAMEMBER; //vmp�зǿƴ��ն��ظ�
	}
	
	// �ϳɿ���ʱ,��MMCU�ڶ��ͨ��ʱ������,�ճ�����(��ʱ�汾)
	// �ϳɸı�ʱ,��MMCU�ڶ��ͨ��ʱ,ֱ�ӷ���nack,ȡ����θı�
	if (/*bChgParam &&*/ bRepeatedMMcuMem)
	{
		wRet =  (u16)ERR_MCU_REPEATEDVMPMMCUMEMBER; //vmp��MMCU�ظ�
	}
	if (/*bChgParam &&*/ bRepeatedSpeMcuMem)
	{
		wRet =  (u16)ERR_MCU_REPEATEDVMPNONEKEDAMEMBER; //vmp�м򵥼���MCU�ظ�
	}

	if(bRepeatedVmpMem &&
		!IsAllowVmpMemRepeated(byVmpId))
	{
		wRet = (u16)ERR_MCU_REPEATEDVMPMEMBER;
	}

	if(bySpeakerChnnl > 1 /*&&		//�ж��ͨ������ɷ����˸���
		!IsAllowVmpMemRepeated()*/)	//������8000A��8000H��ͨ�������ļ�������
	{
		wRet =  (u16)ERR_MCU_REPEATEDVMPSPEAKER;
	}
	
	if(byPollChnnl > 1 /*&&			//�ж��ͨ���������ѯ����
		!IsAllowVmpMemRepeated()*/)	//������8000A��8000H��ͨ�������ļ�������
	{
		wRet =  (u16)ERR_MCU_REPEATEDVMPPOLL;
	}
	
	if(byVmpPollChnnl > 1)			//�ж��ͨ�������Vmpͨ����ѯ����
	{
		wRet = (u16)ERR_MCU_REPEATEDVMPPOLL;
	}
	
	if(byDStreamChnnl > 1)			//�ж��ͨ�������˫������
	{
		wRet =  (u16)ERR_MCU_REPEATEDVMPDSTREAM;
	}

	return wRet;

}

/*==============================================================================
������    :  CheckAdpChnnlAtStart
����      :  ��ʼ�ϳ�ʱ��ǰ����ͨ�����ƹ���
�㷨ʵ��  :  
����˵��  :  u8 byMaxHdChnnlNum		[i]	���ǰ����ͨ����
����ֵ˵��:  BOOL32
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-8-20					Ѧ��							����
==============================================================================*/
BOOL32 CMcuVcInst::CheckAdpChnnlAtStart(u8 byVmpId, TVMPParam_25Mem &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg)
{
	u8 byIdxInHdChnnl  = 0;
	u8 byIdxOutHdChnnl = 0;
	BOOL32 bOverHdChnnlLmt = FALSE;
	TVmpHdChnnlMemInfo tVmpHdChnnlMemInfo;
	TMt tLocalVmpMt;
	//���tmt��vmp������λ��
	u8 abyChnlNo[MAXNUM_VMP_MEMBER];
	u8 byChnlNum = 0;
	TMt tRealSpeaker;
	BOOL32 bNoneKeda = FALSE;
	BOOL32 bDStream = FALSE;
	BOOL32 bG400IPC = FALSE;
	//�����˴����������ڶ��ͨ���ڲ���ǰ����
	if(m_tConf.HasSpeaker() && tVmpParam.IsMtInMember(m_tConf.GetSpeaker() /*GetLocalSpeaker()*/) 
		&& !tVmpParam.IsVMPBrdst()	//VMP�㲥��ѷ�������Ƶ�㲥���
		)
	{
		//��������ķ�����,mcuʱ,������Ӧ�������ϴ�ͨ���ն�
		tRealSpeaker = m_tConf.GetSpeaker();
		if (IsMcu(tRealSpeaker))
		{
			tRealSpeaker = GetSMcuViewMt(tRealSpeaker, TRUE);
		}
		bNoneKeda = ( (!IsKedaMt(m_tConf.GetSpeaker(), TRUE)) || (!tRealSpeaker.IsNull() && !IsKedaMt(tRealSpeaker, FALSE)) );		//�Ƿ��keda
		bG400IPC = IsG400IPCMt(tRealSpeaker);
		//tVmpParam.GetChlOfMtInMember( m_tConf.GetSpeaker(), FALSE, MAXNUM_VMP_MEMBER, abyChnlNo, &byChnlNum );
		GetChlOfMtInVmpParam(tVmpParam, m_tConf.GetSpeaker(), byChnlNum, abyChnlNo);
		LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[CheckAdpChnnlAtStart]SpeakerNo:%u,MaxHdChlNo:%u.\n",
			byChnlNum, byMaxHdChnnlNum);
		// �ڶ��ͨ��,���Ƿǿƴ��ն˻�bG400IPC�ն�,������1·
		if ((bNoneKeda || bG400IPC) &&
			byChnlNum > 1 &&
			byChnlNum <= MAXNUM_VMP_MEMBER)
		{
			for (u8 byNum=1; byNum<byChnlNum; byNum++)
			{
				tVmpParam.ClearVmpMember(abyChnlNo[byNum]);
			}
			byChnlNum = 1;
		}
		// ͨ����Ϊ1ʱ,����ռ��ǰ����
		if (1 == byChnlNum)
		{
			if( byIdxInHdChnnl < byMaxHdChnnlNum )
			{
				tVmpHdChnnlMemInfo.tMtInHdChnnl[byIdxInHdChnnl++] = m_tConf.GetSpeaker();
			}
			else
			{
				tVmpHdChnnlMemInfo.tMtOutHdChnnl[byIdxOutHdChnnl++] = m_tConf.GetSpeaker();
				bOverHdChnnlLmt = TRUE;
				tVmpParam.ClearVmpMember(abyChnlNo[0]);
			}
		}
	}

	TVMPMember tVMPMember;
	TMt tRealMt;
	u8 byChnnlType = LOGCHL_VIDEO;
	TLogicalChannel tLogicChannel;
	BOOL32 bNeedAdjustRes = FALSE;
	u8 byMtStandardFormat = VIDEO_FORMAT_INVALID;
	u8 byReqRes = VIDEO_FORMAT_INVALID;
	BOOL32 bIsMMcu = FALSE;
	//�����������Ա����
	for( u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		// �ӱ���,��ָֹ�뷵��Ϊ��
		if (NULL == tVmpParam.GetVmpMember( byLoop )) {
			tVMPMember.SetNull();
		} else {
			tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		}
		
		if( tVMPMember.IsNull() )
		{
			continue;	//�ճ�Ա����
		}
		// �Ƿ���MMCU
		bIsMMcu = (tVMPMember == m_tCascadeMMCU) || IsNotSupportCascadeMcu(tVMPMember);
		tRealMt = tVMPMember;
		if (IsMcu(tRealMt) && !bIsMMcu)
		{
			tRealMt = GetSMcuViewMt(tRealMt, TRUE);
		}
		
		if( tRealSpeaker == tRealMt)
		//if( GetLocalSpeaker().GetMtId() == tVMPMember.GetMtId() )
		{
			//take speaker into account before,so continue
			continue;
		}

		// ˫������ͨ����ԱΪ˫��Դ
		bDStream = FALSE;
		byChnnlType = LOGCHL_VIDEO;
		if (VMP_MEMBERTYPE_DSTREAM == tVMPMember.GetMemberType())
		{
			bDStream = TRUE;
			byChnnlType = LOGCHL_SECVIDEO;
		}
		
		bNoneKeda = ( (!IsKedaMt(tVMPMember, TRUE)) || (!tRealMt.IsNull() && !IsKedaMt(tRealMt, FALSE)) );		//�Ƿ��keda
		bG400IPC = IsG400IPCMt(tRealMt);
		if( ( !tVmpParam.IsVMPBrdst()					//VMP�㲥��ѷ�������Ƶ�㲥�Լ�ѡ��ȫ���
			&&  IsSelectedbyOtherMtInMultiCas(tVMPMember) )		//��ѡ��
			|| bNoneKeda				//�Ƿ��keda�ն�
			|| bDStream					//˫��Դ
			|| bIsMMcu					//MMCU
			|| bG400IPC
			)//ע�������з�keda��������Ϊ0��Ҳ�ᱻ��Ϊ��keda������֮ǰҪ���˴���Ϊ0�����
		{
			//tVmpParam.GetChlOfMtInMember( tVMPMember, FALSE, MAXNUM_VMP_MEMBER, abyChnlNo, &byChnlNum );
			GetChlOfMtInVmpParam(tVmpParam, tVMPMember, byChnlNum, abyChnlNo);
			if (byChnlNum > 1 && byChnlNum <= MAXNUM_VMP_MEMBER && !bDStream) //����˫������
			{
				// �ǿƴ��ն˲�֧�ֽ����ͨ��,����1��ͨ��,�������
				// MMCU��֧�ֽ����ͨ��,����1��ͨ��,�������
				// G400IPC��֧�ֽ����ͨ��,����1��ͨ��,�������
				if (bNoneKeda || bIsMMcu || bG400IPC)
				{
					for (u8 byNum=1; byNum<byChnlNum; byNum++)
					{
						tVmpParam.ClearVmpMember(abyChnlNo[byNum]);
					}
					byChnlNum = 1;
				}
				//�ڶ��ͨ����,��ռ��ǰ����
				else
				{
					continue;
				}
			}

			bNeedAdjustRes = FALSE;
			tLocalVmpMt = GetLocalMtFromOtherMcuMt( tVMPMember );
			if ( m_ptMtTable->GetMtLogicChnnl( tLocalVmpMt.GetMtId()/*tVMPMember*/, byChnnlType, &tLogicChannel, FALSE )
				&& MEDIA_TYPE_H264 == tLogicChannel.GetChannelType()
				)
			{
				byMtStandardFormat = tLogicChannel.GetVideoFormat();
				bNeedAdjustRes = VidResAdjust(byVmpId, tVmpParam.GetVMPStyle(), byLoop, byMtStandardFormat, byReqRes);
				
				if (bNeedAdjustRes) //�Ƚ���������Ҫ������MT�����п��������ǰ����ͨ��
				{
					if( byIdxInHdChnnl < byMaxHdChnnlNum )
					{
						tVmpHdChnnlMemInfo.tMtInHdChnnl[byIdxInHdChnnl++] = tVMPMember;
					}
					else
					{
						tVmpHdChnnlMemInfo.tMtOutHdChnnl[byIdxOutHdChnnl++] = tVMPMember;

						bOverHdChnnlLmt = TRUE;
						tVmpParam.ClearVmpMember( byLoop);
					}
				}
			}

		}
	}
	LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[CheckAdpChnnlAtStart]IdxInHdChnnlNo:%u,IdxOutHdChnnlNo:%u.\n",
			byIdxInHdChnnl, byIdxOutHdChnnl);
	if(bOverHdChnnlLmt)
	{
		// �����ú�,���޳�Ա,����false ����:0��ǰ����ʱ,��Աȫ���߳�
		if ( tVmpParam.GetVMPMemberNum() == 0)
		{
			return FALSE;
		}
		LogPrint(LOG_LVL_WARNING, MID_MCU_VMP, "[CheckAdpChnnlAtStart]IdxInHdChnnlNo:%u,IdxOutHdChnnlNo:%u.\n",
			byIdxInHdChnnl, byIdxOutHdChnnl);
		cServMsg.SetMsgBody((u8*)&tVmpHdChnnlMemInfo,sizeof(tVmpHdChnnlMemInfo));
		SendReplyBack(cServMsg, MCU_MCS_VMPOVERHDCHNNLLMT_NTF);
		//SendMsgToAllMcs(MCU_MCS_VMPOVERHDCHNNLLMT_NTF,cServMsg);
	}

	return TRUE;
}

/*==============================================================================
������    :  CheckAdpChnnlAtChange
����      :  �ı�ϳɲ��������е�ǰ����ͨ�����ƹ���
�㷨ʵ��  :  
����˵��  :  u8 byMaxHdChnnlNum		[i]	���ǰ����ͨ����
����ֵ˵��:  void
--------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-8-20                  Ѧ��
==============================================================================*/
BOOL32 CMcuVcInst::CheckAdpChnnlAtChange(u8 byVmpId, TVMPParam_25Mem &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg)
{
	// [1/19/2010 xliang] FIXME: ��ԱΪ�¼�VIP

	// ��Ա���岿��
	TMt tLocalVmpMember;			//local���ĳ�Ա
	TVMPMember tVMPMember;			//ͨ����Ӧ��Ա
	TVMPMember tLstVmpMember;		//ͨ����Ӧ�ɳ�Ա
	u8 byChnnlType = LOGCHL_VIDEO;
	TLogicalChannel tLogicChannel;
	BOOL32 bNeedAdjustRes = FALSE;
	u8 byMtStandardFormat = VIDEO_FORMAT_INVALID;	//��Աԭ���ֱ���
	u8 byReqRes = VIDEO_FORMAT_INVALID;				//��ԱҪ�������ķֱ���
	TMt tRealMt;		//ͨ����ʵ��ʾ�ն�(mcuӳ���ϴ�ͨ���ն�)
	BOOL32 bSpeaker = FALSE;
	BOOL32 bNoneKeda = FALSE;
	BOOL32 bSelected = FALSE;
	BOOL32 bDStream = FALSE;
	BOOL32 bIsMMcu	= FALSE;
	BOOL32 bG400IPC = FALSE;

	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
	
	TExInfoForResFps tExInfoForRes;
	BOOL32 bIsNeedAjsRes;
	BOOL32 bSeizePromt; //����Ƿ�����ռ

	//�ҵ�ǰ����ӵĺϳɳ�Ա
	for( u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		// �ӱ���,����ָ�뷵��Ϊ��
		if (NULL == tVmpParam.GetVmpMember( byLoop )) {
			tVMPMember.SetNull();
		} else {
			tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		}

		if (tVMPMember.IsNull())
		{
			continue;
		}

		if (NULL == tConfVmpParam.GetVmpMember(byLoop)) {
			tLstVmpMember.SetNull();
		} else {
			tLstVmpMember = *tConfVmpParam.GetVmpMember(byLoop);
		}

		// ׷��ͨ�������ж�
		if(!tLstVmpMember.IsNull() 
			&& tVMPMember.GetMcuId() == tLstVmpMember.GetMcuId() 
 			&& tVMPMember.GetMtId() == tLstVmpMember.GetMtId() 
			&& tVMPMember.GetMemberType() == tLstVmpMember.GetMemberType()
			)
		{
			continue;
		}

		// ˫������ͨ����ԱΪ˫��Դ
		bDStream = FALSE;
		byChnnlType = LOGCHL_VIDEO;
		if (VMP_MEMBERTYPE_DSTREAM == tVMPMember.GetMemberType())
		{
			// ֻ��Ӧ���潫ĳͨ��ָ��Ϊ˫������ʱ����ռ,��tLstVmpMember����˫������,���ڴ˴�����ռ
			if (VMP_MEMBERTYPE_DSTREAM == tLstVmpMember.GetMemberType())
			{
				continue;
			}
			bDStream = TRUE;
			byChnnlType = LOGCHL_SECVIDEO;
		} 

		// ��H264��ʽ��ռ��ǰ����
		tLocalVmpMember  = GetLocalMtFromOtherMcuMt( tVMPMember );
		if (!(m_ptMtTable->GetMtLogicChnnl( tLocalVmpMember.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) &&
			  MEDIA_TYPE_H264 == tLogicChannel.GetChannelType()))
		{
				continue;
		}

		//��������ӵ��ն�
		tExInfoForRes.m_byEvent = EvMask_VMP;
		tExInfoForRes.m_byOccupy = Occupy_InChannel;
		tExInfoForRes.m_byEqpid = byVmpId;
		tExInfoForRes.m_byPos = byLoop;
		tExInfoForRes.m_tVmpParam = tVmpParam;
		tExInfoForRes.m_byMemberType = tVMPMember.GetMemberType(); //֧��˫������
		tExInfoForRes.m_bSeizePromt = TRUE;	//��Ҫ��ռ��ʾ
		tExInfoForRes.m_bConsiderVmpBrd = FALSE; //���ӻ���ϳɹ㲥���ϱ�ѡ���ն���Ҫ��ռ
		bIsNeedAjsRes = IsNeedAdjustMtVidFormat((TMt)tVMPMember, tExInfoForRes);
		bSeizePromt = tExInfoForRes.m_bResultSeizePromt;
		if (bSeizePromt)
		{
			TMt tSeizeMt = (TMt)tVMPMember;
			cServMsg.SetEqpId(byVmpId);
			cServMsg.SetMsgBody((u8*)&tSeizeMt,sizeof(TMt));	//��Ҫ��ռ��������ͨ�����ն�
			u8 byMtNum = tVmpChnnlInfo.GetHDChnnlNum();
			TChnnlMemberInfo atChnnlMemInfo[MAXNUM_MPU2VMP_E20_HDCHNNL];
			for (u8 byIdx=0; byIdx<byMtNum; byIdx++)
			{
				tVmpChnnlInfo.GetHdChnnlInfo(byIdx, &atChnnlMemInfo[byIdx]);
			}
			//��ӿ�ѡ����ռ�ն���Ϣ
			cServMsg.CatMsgBody((u8 *)atChnnlMemInfo,sizeof(TChnnlMemberInfo) * byMtNum);
			u8 byForceSeize = 0;		//�Ƿ�ǿ����ռ,���ֶβ���ɾ�������濿���ֶ���ʾȡ����ť
			cServMsg.CatMsgBody(&byForceSeize, sizeof(byForceSeize)); 
			SendReplyBack(cServMsg, MCU_MCS_VMPPRISEIZE_REQ);
			//��¼
			TVmpPriSeizeInfo tPriSeiInfo;
			tPriSeiInfo.m_tPriSeizeMember = tVMPMember;
			tPriSeiInfo.m_byChlIdx = byLoop;
			g_cMcuVcApp.SetVmpPriSeizeInfo(tVmpEqp, tPriSeiInfo);
			return FALSE;
		}
		

// 		if(!tVMPMember.IsNull() && !m_tLastVmpParam.IsMtInMember(tVMPMember))
		/*{
			tLocalVmpMember  = GetLocalMtFromOtherMcuMt( tVMPMember );
			if ( m_ptMtTable->GetMtLogicChnnl( tLocalVmpMember.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) &&
				MEDIA_TYPE_H264 == tLogicChannel.GetChannelType()
				)
			{
				byMtStandardFormat = tLogicChannel.GetVideoFormat();
				bNeedAdjustRes = VidResAdjust(byVmpId, tVmpParam.GetVMPStyle(), byLoop, byMtStandardFormat, byReqRes);
				if(bNeedAdjustRes)
				{
					tRealMt = IsMcu(tVMPMember) ? GetSMcuViewMt(tVMPMember,TRUE) : tVMPMember;
					//��������ķ�����,mcuʱ,������Ӧ�������ϴ�ͨ���ն�
					tRealSpeaker = m_tConf.GetSpeaker();
					if (IsMcu(tRealSpeaker))
					{
						tRealSpeaker = GetSMcuViewMt(tRealSpeaker, TRUE);
					}
					//��������ж�
					bSpeaker = (!tRealSpeaker.IsNull() && tRealSpeaker == tRealMt);	//�Ƿ��Ƿ�����
					bNoneKeda = ( (!IsKedaMt(tVMPMember, TRUE)) || (!tRealMt.IsNull() && !IsKedaMt(tRealMt, FALSE)) );		//�Ƿ��keda
					bSelected = IsSelectedbyOtherMtInMultiCas(tVMPMember);	//�Ƿ�ѡ��
					bIsMMcu = (tVMPMember == m_tCascadeMMCU);
					bG400IPC = IsG400IPCMt(tVMPMember);
					if( (bSpeaker && !tVmpParam.IsVMPBrdst())
						|| bNoneKeda 
						|| bSelected
						|| bDStream
						|| bIsMMcu
						|| bG400IPC
						)
					{
						//tVmpParam.GetChlOfMtInMember( tVMPMember, FALSE, MAXNUM_VMP_MEMBER, abyChnlNo, &byChnlNum );
						GetChlOfMtInVmpParam(tVmpParam, tVMPMember, byChnlNum);

						// �ڶ��ͨ����,��ռ��ǰ����,�ǿƴ��ն���CheckMpuMember�л��ж�,����˫������ͨ��
						if (byChnlNum > 1 && !bDStream)
						{
							continue;
						}
						
						// 0��ǰ����ʱ,������ռ��ʾ,ֱ�ӷ���Nack
						if (0 == byMaxHdChnnlNum)
						{
							ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[VmpCommonReq-change] Vip vmp Chnnl is not enough. nack!\n");
							cServMsg.SetErrorCode( ERR_VMP_NO_VIP_VMPCHNNL );
							SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
							return FALSE;
						}

						//����ǰҪ���滻���ն���ǰ���䣬��ǰ����ͨ�����ǹ���
						if (!tLstVmpMember.IsNull())
						{
							tVmpChnnlInfo.GetHdChnnlInfoByPos(byLoop, &tChnlInfo);
							if (!tChnlInfo.GetMt().IsNull())
							{
								continue;
							}
							/*TMt tLstRealMt = (TMt)tLstVmpMember;
							if (IsMcu(tLstRealMt))
							{
								tLstRealMt = GetSMcuViewMt(tLstRealMt, TRUE);
							}
							if (0 < m_tVmpChnnlInfo.GetChnlCountByMt(tLstRealMt))
							{
								continue;
							}/
						}

						if(tVmpChnnlInfo.GetHDChnnlNum() == byMaxHdChnnlNum) //ǰ����ͨ��������Ҫ��ռ��ʾ
						{
							TMt tSeizeMt = (TMt)tVMPMember;
							/*if (IsMcu(tSeizeMt))
							{
								tSeizeMt = GetSMcuViewMt(tSeizeMt, TRUE);
							}/
							cServMsg.SetMsgBody((u8*)&tSeizeMt,sizeof(TMt));	//��Ҫ��ռ��������ͨ�����ն�
							u8 byMtNum = tVmpChnnlInfo.GetHDChnnlNum();
							TChnnlMemberInfo atChnnlMemInfo[MAXNUM_MPU2VMP_E20_HDCHNNL];
							//u8 abyChnlNo[MAXNUM_MPU2VMP_E13_HDCHNNL];
							//m_tVmpChnnlInfo.GetChnlInfoList(byMtNum, abyChnlNo, atChnnlMemInfo);
							for (u8 byIdx=0; byIdx<byMtNum; byIdx++)
							{
								tVmpChnnlInfo.GetHdChnnlInfo(byIdx, &atChnnlMemInfo[byIdx]);
							}
							//��ӿ�ѡ����ռ�ն���Ϣ
							cServMsg.CatMsgBody((u8 *)atChnnlMemInfo,sizeof(TChnnlMemberInfo) * byMtNum);
							
							u8 byForceSeize = 0;		//�Ƿ�ǿ����ռ // [1/19/2010 xliang] �����ֶΣ������ͬ���Ķ�
							cServMsg.CatMsgBody(&byForceSeize, sizeof(byForceSeize)); 
							SendReplyBack(cServMsg, MCU_MCS_VMPPRISEIZE_REQ);
							//��¼
							TVmpPriSeizeInfo tPriSeiInfo;
							tPriSeiInfo.m_tPriSeizeMember = tVMPMember;
							tPriSeiInfo.m_byChlIdx = byLoop;
							g_cMcuVcApp.SetVmpPriSeizeInfo(tVmpEqp, tPriSeiInfo);
							//m_atVMPTmpMember[byLoop] = tVMPMember;
							//m_tVmpAdaptChnSeizeOpr.m_tMt = tVMPMember;
							return FALSE;
						}
					}
				}
			}
		}*/
	}
	return TRUE;
}
/*==============================================================================
������    :  SendVmpPriSeizeReq
����      :  ��ռ��ʾ��Ϣ���
�㷨ʵ��  :  
����˵��  :  TMt tMt [i] ��ռ�նˣ�
			 u8 byForceSeize [i] �Ƿ�ǿ����ռ(Ԥ��)
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾			�޸���          �߶���          �޸ļ�¼
2011-11-30					yanghuaizhi
==============================================================================*/
void CMcuVcInst::SendVmpPriSeizeReq(u8 byVmpId, TMt &tSeizeMt, u8 byForceSeize)
{
	if (!IsValidVmpId(byVmpId))
	{
		return;
	}

	CServMsg cServMsg;
	cServMsg.SetEqpId(byVmpId);
	cServMsg.SetMsgBody((u8*)&tSeizeMt,sizeof(TMt));	//��Ҫ��ռ��������ͨ�����ն�
	// ��ð��ն�������ǰ������Ϣ
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
	u8 byMtNum = tVmpChnnlInfo.GetHDChnnlNum();
	TChnnlMemberInfo atChnnlMemInfo[MAXNUM_MPU2VMP_E20_HDCHNNL];
	//u8 abyChnlNo[MAXNUM_MPU2VMP_E13_HDCHNNL];//Ŀǰǰ�����ն˸�����Ϊ1
	//m_tVmpChnnlInfo.GetChnlInfoList(byMtNum, abyChnlNo, atChnnlMemInfo);
	for (u8 byIdx=0; byIdx<byMtNum; byIdx++)
	{
		tVmpChnnlInfo.GetHdChnnlInfo(byIdx, &atChnnlMemInfo[byIdx]);
	}

	//��ӿ�ѡ����ռ�ն���Ϣ
	cServMsg.CatMsgBody((u8 *)atChnnlMemInfo,sizeof(TChnnlMemberInfo) * byMtNum);
	cServMsg.CatMsgBody(&byForceSeize, sizeof(byForceSeize)); //���ֶβ���ɾ�������濿���ֶ���ʾȡ����ť
	SendMsgToAllMcs(MCU_MCS_VMPPRISEIZE_REQ, cServMsg);
}

/*==============================================================================
������    :  SendVmpParamToChairMan
����      :  ֪ͨ��ϯ�ն�ˢ��vmpparam��Ϣ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2012-07-09                yanghuaizhi
==============================================================================*/
void CMcuVcInst::SendVmpParamToChairMan()
{
	CServMsg cServMsg;
	u8 byVmpCount = GetVmpCountInVmpList();
	TEqp tVmpEqp;
	tVmpEqp.SetNull();
	if (1 == byVmpCount)
	{
		u8 byVmpId = GetTheOnlyVmpIdFromVmpList();
		tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	}
	else
	{
		if (HasJoinedChairman())
		{
			SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVMP_NOTIF, cServMsg );
			return;
		}
	}
	// ����ϯ,��vmp�ϳ���,����ϯ����VmpParam
	if (HasJoinedChairman() /*&& CONF_VMPMODE_NONE != g_cMcuVcApp.GetVMPMode(m_tVmpEqp)*/)
	{
		TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
		TVMPMember *ptVmpMember = NULL;
		u8 byLoop = 0;
		
		for(byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++)
		{
			ptVmpMember = tVmpParam.GetVmpMember(byLoop);
			if( ptVmpMember == NULL )
			{
				continue;
			}
			if( ptVmpMember->IsNull())
			{
				// Ϊ��
			}
			else
			{
				if( !ptVmpMember->IsLocal() )
				{
					TMt tTempMt;
					tTempMt.SetMcuId( ptVmpMember->GetMcuId() );
					tTempMt.SetMtId( ptVmpMember->GetMtId() );
					tTempMt = GetLocalMtFromOtherMcuMt( tTempMt );
					ptVmpMember->SetMt( tTempMt );
				}
			}
		}
		
		for(; byLoop < MAXNUM_VMP_MEMBER; byLoop++)
		{
			ptVmpMember = tVmpParam.GetVmpMember(byLoop);
			if( ptVmpMember == NULL )
			{
				continue;
			}
			ptVmpMember->SetMcuId( 0 );
			ptVmpMember->SetMtId( 0 );
		}
		
		cServMsg.SetMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
		SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_VMPPARAM_NOTIF, cServMsg );
	}
}

/*==============================================================================
������    :  UpdateVmpDStream
����      :  ����vmp˫��������ʵ˫��Դ,�ϱ����
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2012-07-31                yanghuaizhi
==============================================================================*/
void CMcuVcInst::UpdateVmpDStream(const TMt tMt)
{
	if (tMt.IsNull())
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VMP, "[UpdateVmpDStream]tMt is null.\n");
		return;
	}

	// ���ģʽ����˫������ͨ��
	TEqp tVmpEqp;
	TVMPParam_25Mem tConfVmpParam;
	TPeriEqpStatus tPeriEqpStatus;
	TVMPParam_25Mem tVmpParam;
	TVMPParam_25Mem tLastVmpParam;
	u8 byChlIdx;
	TVMPMember tTmpMember;
	TVMPMember* ptMember;
	CServMsg cServMsg;
	for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			continue;
		}
		tVmpEqp = g_cMcuVcApp.GetEqp( m_abyVmpEqpId[byIdx] );

		tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
		if (tConfVmpParam.GetVMPMode() == CONF_VMPMODE_CTRL)
		{
			if (g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus))
			{
				tVmpParam = tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam();
				tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
				byChlIdx = tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_DSTREAM);
				//��˫������ͨ��
				if (byChlIdx < MAXNUM_VMP_MEMBER)
				{
					// ��������״̬��Ϣ
					tTmpMember.SetNull();
					ptMember = tVmpParam.GetVmpMember(byChlIdx);
					ptMember->SetMemberTMt(tMt);
					tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);
					g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
					// ���»�����Ϣ
					ptMember = tConfVmpParam.GetVmpMember(byChlIdx);
					if (VMP_MEMBERTYPE_DSTREAM == ptMember->GetMemberType())
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[UpdateVmpDStream]m_tConf is updated, chlIdx:%d.\n", byChlIdx);
						tTmpMember = *ptMember;
						ptMember->SetMemberTMt(tMt);
						//m_tConf.m_tStatus.SetVmpParam(tVmpParam);
						g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tConfVmpParam);
					}
					// ����ˢ��˫���������
					RefreshVmpChlMemalias(tVmpEqp.GetEqpId());
					// ����m_tLastVmpParam��Ϣ
					ptMember = tLastVmpParam.GetVmpMember(byChlIdx);
					if (VMP_MEMBERTYPE_DSTREAM == ptMember->GetMemberType() &&
						tTmpMember == *ptMember)
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[UpdateVmpDStream]m_tLastVmpParam is updated.\n");
						ptMember->SetMemberTMt(tMt);
					}
					g_cMcuVcApp.SetLastVmpParam(tVmpEqp, tLastVmpParam);
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[UpdateVmpDStream]PeriEqpStatus is updated, chlIdx:%d, tMt(%d,%d).\n",
						byChlIdx, tMt.GetMcuId(), tMt.GetMtId());
					// ֪ͨ���,ˢvmpͨ����Ա+ˢ�¼�˫��Դ��vmpͼ��
					cServMsg.Init();
					cServMsg.SetEqpId(tVmpEqp.GetEqpId());
					cServMsg.SetMsgBody( (u8*)&tConfVmpParam, sizeof(tConfVmpParam) );
					SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
					/*cServMsg.SetMsgBody( (u8*)&tPeriEqpStatus, sizeof(TPeriEqpStatus) );
 					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );*/
				}
			}
		}
	}
}

/*==============================================================================
������    :  CheckAdpChnnlLmt
����      :  ǰ����ͨ�����ƹ���
�㷨ʵ��  :  
����˵��  :  BOOL32 bChgParam	[i]		TRUE����ı�ϳɲ��������еĹ��ˣ�
										FALSE����տ�ʼ�ϳɴ����еĹ���
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-8-20					Ѧ��
==============================================================================*/
BOOL32 CMcuVcInst::CheckAdpChnnlLmt(u8 byVmpId, TVMPParam_25Mem& tVmpParam, CServMsg &cServMsg, BOOL32 bChgParam /* = FALSE */)
{
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
	
	ConfPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[CheckAdpChnnlLmt] byVmpSubType == %d\n",byVmpSubType);

	// ����ʱ�������ǰ������Ŀ
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
	if (!bChgParam)
	{
		tVmpChnnlInfo.clear();// ����ʱ���ǰ������Ϣ
		u8 byMaxStyleNum = 0; //���֧�ֶ��ٷ���ڲ����ֱ���
		u8 byMaxHdChnnlNum = GetMaxHdChnlNumByVmpId(m_tConf, byVmpId, byMaxStyleNum);
		tVmpChnnlInfo.SetMaxNumHdChnnl(byMaxHdChnnlNum);
		tVmpChnnlInfo.SetMaxStyleNum(byMaxStyleNum);
		g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
	}

	// ����ʱ,������ǰ������ռ
	if(VMP == byVmpSubType || tVmpParam.IsVMPAuto() || ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode())
	{
		return TRUE;
	}

	// MPU��4���漰����,MPU2��֧�ֵĻ���������¶�����Ҫ����
	// 8kh1080p/30fps��������鿪����vmp4���漰����Ҳ����Ҫ����
	u8 byMemberNum = tVmpParam.GetMaxMemberNum();
	if (/*(MPU_SVMP == byVmpSubType && byMemberNum <= 4) ||
		(Is8khVmpInConf1080p30fps(m_tConf, byVmpId) && byMemberNum <= 4) ||*/ //8kh�����������ж�
		byMemberNum <= tVmpChnnlInfo.GetMaxStyleNum())
	{
		//�������ʱ,��ʼ��ǰ������Ϣ
		tVmpChnnlInfo.clearHdChnnl();
		g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
		return TRUE;
	}

	if(!bChgParam)
	{
		return CheckAdpChnnlAtStart(byVmpId, tVmpParam, tVmpChnnlInfo.GetMaxNumHdChnnl(), cServMsg);
	}
	else
	{
		return CheckAdpChnnlAtChange(byVmpId, tVmpParam, tVmpChnnlInfo.GetMaxNumHdChnnl(), cServMsg);
	}

}

/*==============================================================================
������    :  CheckMpuMember
����      :  У��MPU��Ա
�㷨ʵ��  :  ���ע��
����˵��  :  TVMPParam &	[in]	�ϳɲ���
			 u16 &			[out]	������
����ֵ˵��:  BOOL32					fail�򷵻�FALSE
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-8-19	 4.6			Ѧ��							������
==============================================================================*/
BOOL32 CMcuVcInst::CheckMpuMember(u8 byVmpId, TVMPParam_25Mem& tVmpParam, u16& wErrorCode, BOOL32 bChgParam /*= FALSE */)
{
	//�Զ�����ϳɲ���������У��
	if (tVmpParam.IsVMPAuto())
	{
		return TRUE;
	}
	
	//VMP��֧����ϯ����
	if(tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_CHAIRMAN))
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[VmpCommonReq]MPU not support chairman-membertype!\n");
		wErrorCode = ERR_VMPUNSUPPORTCHAIRMAN;
		return FALSE;
	}


		//�����в�����ͬһ��MTռ�˶����Աͨ��,׷���ж�Other�ն��ڶ��ͨ����
	u16 wRet = 0;
	wRet = CheckSameMtInMultChnnl(byVmpId, tVmpParam, bChgParam);
	if ( wRet != 0 )//wRet��Ϊerrorcode
	{
		wErrorCode = wRet;
		return FALSE;
	}		
	//�������Ƶ��ѯʱ������ϳ�����֧����ѯ����ͷ����˸���ͬʱ����
	wRet = CheckMpuAudPollConflct(byVmpId, tVmpParam, bChgParam);
	if ( wRet != 0 )//wRet��Ϊerrorcode
	{
		wErrorCode = wRet;
		return FALSE;
	}


	return TRUE;
}

/*==============================================================================
������    :  GetTheMainVmpIdFromVmpList
����      :  ���һ����Ҫvmp�����ڵ���ʱ�ж���ϳ�����ָ��һ����������
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
u8 CMcuVcInst::GetTheMainVmpIdFromVmpList()
{
	u8 byVmpId = 0;
	// ����ǰ�й㲥vmp����Ϊ�㲥vmp
	if (!m_tVmpEqp.IsNull())
	{
		if (IsVmpIdInVmpList(m_tVmpEqp.GetEqpId()))//�㲥vmp���ܵ���
		{
			byVmpId = m_tVmpEqp.GetEqpId();
		}
	}
	else// �ҵ�һ���ǹ㲥��vmp�����޹㲥Ҳ�޷ǹ㲥vmp���򷵻�0
	{
		for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
		{
			if (IsValidVmpId(m_abyVmpEqpId[byIdx]))
			{
				byVmpId = m_abyVmpEqpId[byIdx];
				break;
			}
		}
	}

	return byVmpId;
}

/*==============================================================================
������    :  IsVmpIdInVmpList
����      :  �жϻ���vmpList���Ƿ���ָ��VmpId
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsVmpIdInVmpList(u8 byVmpId)
{
	if(!IsValidVmpId(byVmpId))
	{
		return FALSE;
	}

	for (u8 byIdx = 0; byIdx < MAXNUM_CONF_VMP; byIdx++)
	{
		if (byVmpId == m_abyVmpEqpId[byIdx])
		{
			return TRUE;
		}
	}

	return FALSE;
}

/*==============================================================================
������    :  RemoveVmpIdFromVmpList
����      :  ��ָ��vmp������ɾ��
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::RemoveVmpIdFromVmpList(u8 byVmpId)
{
	for (u8 byIdx = 0; byIdx < MAXNUM_CONF_VMP; byIdx++)
	{
		if (byVmpId == m_abyVmpEqpId[byIdx])
		{
			m_abyVmpEqpId[byIdx] = 0;
		}
	}

	// ģ��vmp��ش���,�������vmpid��ģ�濪����vmp�������ģ��vmp
	if (IsValidVmpId(byVmpId) && m_tModuleVmpEqp.GetEqpId() == byVmpId)
	{
		m_tModuleVmpEqp.SetNull();
		//ֹͣ���渴�Ϻ�VMPģ�岻����Ч
		m_tConfInStatus.SetVmpModuleUsed(TRUE);
	}

	if( IsValidVmpId(byVmpId) && m_byCreateBy == CONF_CREATE_NPLUS)
	{
		g_cMcuVcApp.NplusRemoveVmpModuleEqpId(byVmpId);

	}
}

/*==============================================================================
������    :  AddVmpIdIntoVmpList
����      :  ��ָ��vmp��ӵ�����
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::AddVmpIdIntoVmpList(u8 byVmpId)
{
	if(!IsValidVmpId(byVmpId))
	{
		return;
	}

	u8 byTmpPos = MAXNUM_CONF_VMP;
	for (u8 byIdx = 0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		// �ҵ���һ���ɴ洢λ��
		if (!IsValidVmpId(m_abyVmpEqpId[byIdx]) && MAXNUM_CONF_VMP == byTmpPos)
		{
			byTmpPos = byIdx;
		}
		// ���������У����������
		if (byVmpId == m_abyVmpEqpId[byIdx])
		{
			return;
		}
	}

	if (MAXNUM_CONF_VMP != byTmpPos)
	{
		m_abyVmpEqpId[byTmpPos] = byVmpId;
	}
}

/*==============================================================================
������    :  GetVmpCountInVmpList
����      :  ���������ռ�úϳ�������
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
u8 CMcuVcInst::GetVmpCountInVmpList()
{
	u8 byCount = 0;
	for (u8 byIdx = 0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			byCount++;
		}
	}
	return byCount;
}

/*==============================================================================
������    :  GetTheOnlyVmpIdFormVmpList
����      :  �������л�ý��е�Ĭ�Ϻϳ���
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
u8 CMcuVcInst::GetTheOnlyVmpIdFromVmpList(void)
{
	u8 byVmpId = 0;
	// �������������ҵ�Ψһ��һ������ռ�õĺϳ�������ռ�ö��������0
	for (u8 byIdx = 0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			if (0 == byVmpId)
			{
				byVmpId = m_abyVmpEqpId[byIdx];
			}
			else
			{
				byVmpId = 0;
				break;
			}
		}
	}

	return byVmpId;
}

/*==============================================================================
������    :  IsVmpStatusUseStateInVmpList
����      :  �жϵ�ǰ����ռ�õĺϳ������Ƿ���ָ��UseState�ĺϳ���
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsVmpStatusUseStateInVmpList(u8 byUseState)
{
	u8 byVmpId;
	TPeriEqpStatus tPeriEqpStatus; 
	for (u8 byIdx = 0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		byVmpId = m_abyVmpEqpId[byIdx];
		if (IsValidVmpId(byVmpId))
		{
			if (g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus ))
			{
				if (byUseState == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

/*==============================================================================
������    :  ClearAdpChlInfoInAllVmp
����      :  ��ĳ�ն���vmp������ǰ����
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::ClearAdpChlInfoInAllVmp(TMt tMt)
{
	if (tMt.IsNull())
	{
		return;
	}
	if (IsMcu(tMt) && !(tMt == m_tCascadeMMCU))
	{
		//�ҵ���MCU��ײ��ϴ��ն�
		TMt tTmpMt = GetSMcuViewMt(tMt, TRUE);
		if (!tTmpMt.IsNull())
		{
			tMt = tTmpMt;
		}
	}

	u8 byVmpId;
	TEqp tVmpEqp;
	TVmpChnnlInfo tVmpChnnlInfo;
	//���ն����ǰ����ͨ��
	for (u8 byTmpIdx=0; byTmpIdx<MAXNUM_CONF_VMP; byTmpIdx++)
	{
		if (!IsValidVmpId(m_abyVmpEqpId[byTmpIdx]))
		{
			continue;
		}
		byVmpId = m_abyVmpEqpId[byTmpIdx];
		tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
		tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);

		tVmpChnnlInfo.ClearChnlByMt(tMt);
		g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
	}
}

/*==============================================================================
������    :  ChangeMtResFpsInTW
����      :  ��������ϳ����ն˷ֱ���
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-04					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::ChangeMtResFpsInTW(TMt tMt, BOOL32 bOccupy)
{
	BOOL32 bRet = TRUE;
	TExInfoForResFps tExInfoForResFps;
	tExInfoForResFps.m_byEvent = EvMask_TVWall;
	if (bOccupy)
	{
		tExInfoForResFps.m_byOccupy = Occupy_InChannel;
	}
	else
	{
		tExInfoForResFps.m_byOccupy = Occupy_OutChannel;
	}
	
	BOOL32 bIsNeedAjsRes = IsNeedAdjustMtVidFormat(tMt, tExInfoForResFps);
	u8 byNewRes = tExInfoForResFps.m_byNewFormat;	//���Ҫ�������ķֱ���
	BOOL32 bSetChlRes = tExInfoForResFps.m_bResultSetVmpChl; //����Ƿ�Setchnl
	BOOL32 bStart = tExInfoForResFps.m_bResultStart; //��ø��¼����͵������ǻָ��ֱ���
	BOOL32 bIsNeedAjsFps = IsNeedAdjustMtVidFps(tMt, tExInfoForResFps);
	BOOL32 bSetChlFps = tExInfoForResFps.m_bResultSetVmpChl; //����Ƿ�Setchnl
	u8 byNewFps = tExInfoForResFps.m_byNewFps;


	if (bIsNeedAjsRes)
	{
		// ���͵��ֱ�����Ϣ
		ChangeVFormat(tMt, bStart, byNewRes);
	}

 	if (bIsNeedAjsFps)
	{
		// ���͵�֡����Ϣ
		SendChgMtFps(tMt, LOGCHL_VIDEO, byNewFps, bStart);
	}

	if (bOccupy)
	{
		// ��ͨ��ʱ���ܽ�ͨ����������Ϊ���ֱ���ʧ��
		if (!bSetChlRes || !bSetChlFps)
		{
			bRet = FALSE;
		}
	}
	
	return bRet;
}

/*==============================================================================
������    :  ChangeMtResFpsInVmp
����      :  ��������ϳ����ն˷ֱ���
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::ChangeMtResFpsInVmp(u8 byVmpId, TMt tMt, TVMPParam_25Mem *ptVMPParam, 
									   BOOL32 bOccupy/* = TRUE*/, 
									   u8 byMemberType/* = VMP_MEMBERTYPE_NULL*/, 
									   u8 byPos/* = MAXNUM_VMP_MEMBER*/,
									   BOOL32 bMsgMcs/* = FALSE*/)
{
	if (!IsValidVmpId(byVmpId) || tMt.IsNull() || ptVMPParam == NULL)
	{
		return FALSE;
	}

	BOOL32 bRet = TRUE;
	TExInfoForResFps tExInfoForResFps;
	tExInfoForResFps.m_byEvent = EvMask_VMP;
	if (bOccupy)
	{
		tExInfoForResFps.m_byOccupy = Occupy_InChannel;
	}
	else
	{
		tExInfoForResFps.m_byOccupy = Occupy_OutChannel;
	}
	tExInfoForResFps.m_byEqpid = byVmpId;
	tExInfoForResFps.m_byPos = byPos;
	tExInfoForResFps.m_byMemberType = byMemberType;
	tExInfoForResFps.m_tVmpParam = *ptVMPParam;
	tExInfoForResFps.m_bConsiderVmpBrd = FALSE;//�ϳɳ�Ա����ʱ������vmp�㲥
	BOOL32 bIsNeedAjsRes = IsNeedAdjustMtVidFormat(tMt, tExInfoForResFps);
	u8 byNewRes = tExInfoForResFps.m_byNewFormat;	//���Ҫ�������ķֱ���
	BOOL32 bSetVmpChlRes = tExInfoForResFps.m_bResultSetVmpChl; //����Ƿ�Setvmpchnl
	BOOL32 bSeizePromt = tExInfoForResFps.m_bResultSeizePromt; //����Ƿ�����ռ
	BOOL32 bStart = tExInfoForResFps.m_bResultStart; //��ø��¼����͵������ǻָ��ֱ���
	/* ����ռ��������������,�˺���������ռ�����˷���ֵ����ΪTRUE
	if (bSeizePromt)
	{
		u8 bySeizeVmpid = tExInfoForResFps.m_byEqpid;
		u8 bySeizePos = tExInfoForResFps.m_byPos;
		// ��ռ����
	}*/

	BOOL32 bIsNeedAjsFps = IsNeedAdjustMtVidFps(tMt, tExInfoForResFps);
	BOOL32 bSetVmpChlFps = tExInfoForResFps.m_bResultSetVmpChl; //����Ƿ�Setvmpchnl
	u8 byNewFps = tExInfoForResFps.m_byNewFps;
	
	if (bIsNeedAjsRes)
	{
		// ���͵��ֱ�����Ϣ
		ChangeVFormat(tMt, bStart, byNewRes, byVmpId, ptVMPParam->GetVMPStyle(), byPos);
	}
	
	if (bIsNeedAjsFps)
	{
		// ���͵�֡����Ϣ
		SendChgMtFps(tMt, LOGCHL_VIDEO, byNewFps, bStart);
	}

	// ȷ���ɽ��������ն˳�vmpͨ������������������
	if (bOccupy)
	{
		if (bSetVmpChlRes && bSetVmpChlFps)
		{
			// �����ն�ֱ��Setvmpchnl,����Ҫ���ֱ��ʵ��¼��ն�ֱ�ӽ�����,��Ҫ���ֱ��ʵ��¼��ն˵ȵ��ֱ���Ack��Setvmpchnl
			if (tMt.IsLocal() || !bIsNeedAjsRes)
			{
				// �ն��ѽ������Ļ������������½�����
				TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
				TVMPParam_25Mem tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
				TVMPMember tLastVmpMem = *tLastVmpParam.GetVmpMember(byPos);
				if (tLastVmpMem.GetMemberType() == byMemberType &&
					tLastVmpMem.GetMcuId() == tMt.GetMcuId() &&
					tLastVmpMem.GetMtId() == tMt.GetMtId())
				{
					//��������
				}
				else
				{
					SetVmpChnnl(byVmpId, tMt, byPos, byMemberType, bMsgMcs);
				}
			}
		}
		else
		{
			// ��ͨ��ʱ�����յó�������Setvmpchnl������Ϊ���ֱ���ʧ��
			bRet = FALSE;
		}
	}

	return bRet;
}

/*==============================================================================
������    :  IsNotSupportCascadeMcu
����      :  �ж�ĳMCU�Ƿ��Ǽ򵥼�����MCU
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-04					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsNotSupportCascadeMcu(const TMt &tMt)
{
	// ֻ֧���б����µ�mcu
	if (tMt.IsNull() || !tMt.IsLocal())
	{
		return FALSE;
	}

	// ������Ϊ�򵥼�������ʱ������ManuId�ж�
	if (!m_tConf.GetConfAttrb().IsSupportCascade())
	{
		// ���ü򵥼������飬������IsMcu()�ӿ��ж�mcu
		u8 byManuId = m_ptMtTable->GetManuId( tMt.GetMtId() );
		if(byManuId == MT_MANU_KDCMCU)
		{
			return TRUE;
		}
	}
	else if (IsMcu(tMt)) //δ���ü򵥼������飬�����¼������˼򵥼���
	{
		// ��֧��MMCU���¼�mcu�϶�Ϊ�򵥼���mcu
		TMultiCapSupport tCapSupport;
		m_ptMtTable->GetMtMultiCapSupport( tMt.GetMtId(), &tCapSupport );
		if (!tCapSupport.IsSupportMMcu())
		{
			return TRUE;
		}
	}

	return FALSE;
}

/*==============================================================================
������    :  IsVmpSeeByChairman
����      :  ��ϯ�ն��Ƿ���ѡ������ϳ�
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-04					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsVmpSeeByChairman()
{
	// ����ϯ������ϯѡ��Դ��vmpʱ
	if (HasJoinedChairman())
	{
		TMt tChairMan = m_tConf.GetChairman();
		u8 byMode = MODE_VIDEO;
		TMtStatus tDstMtStatus;
		m_ptMtTable->GetMtStatus( tChairMan.GetMtId(), &tDstMtStatus );
		TMt tSelMt = tDstMtStatus.GetSelectMt(byMode);
		if ( !tSelMt.IsNull() &&
			tSelMt.GetType() == TYPE_MCUPERI &&
			tSelMt.GetEqpType() == EQP_TYPE_VMP)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/*==============================================================================
������    :  IsCanAdjustMtRes
����      :  �����ն��ܲ��ܵ��ֱ���
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  FALSE�����ܵ���TRUE���ɵ�
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					   �±�							
==============================================================================*/
BOOL32 CMcuVcInst::IsCanAdjustMtRes(TMt tMt)
{
	if (   (m_tCascadeMMCU == tMt)
		|| IsNotSupportCascadeMcu(tMt)
		/*|| !IsKedaMt(tMt, TRUE)*/
		|| (!tMt.IsNull() && !IsKedaMt(tMt, FALSE))
		|| IsG400IPCMt(tMt)
		|| (MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId())
			&& !GetSMcuViewMt(tMt).IsNull() && !IsKedaMt(GetSMcuViewMt(tMt), FALSE))
	   )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/*==============================================================================
������    :  IsCanAdjustMtFps
����      :  �����ն��ܲ��ܵ�֡��
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  FALSE�����ܵ���TRUE���ɵ�
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-08-07					�±�							
==============================================================================*/
BOOL32 CMcuVcInst::IsCanAdjustMtFps(TMt tMt)
{
	BOOL32 bIsMtCanAdjust = FALSE;

	if ( (m_tCascadeMMCU == tMt)
		|| IsNotSupportCascadeMcu(tMt)
		|| IsG400IPCMt(tMt)
		)
	{
		return FALSE;
	}

	if (tMt.IsLocal())
	{
		// �����ƴ��ն���Ϊ�ɵ�
		if (!IsMcu(tMt) && IsKedaMt(tMt, FALSE))
		{
			bIsMtCanAdjust = TRUE;
		}
		else if(IsMcu(tMt) && IsNeedChangeFpsMt(tMt.GetMtId()))
		{
			TMt tMtTemp = GetSMcuViewMt(tMt, TRUE);
			// �¼�Mcu���ϴ�Դ��Ϊ�ɵ�
			if (tMtTemp.IsNull())
			{
				bIsMtCanAdjust = TRUE;
			}
			else if (!tMtTemp.IsNull() && IsKedaMt(tMtTemp, FALSE))
			{
				//�¼�Mcu�ϴ�ԴΪ�ƴ��ն���Ϊ�ɵ�
				bIsMtCanAdjust = TRUE;
			}
		}
	}
	else
	{
		// �¼�Ϊ�ƴ��ն���Ϊ�ɵ�
		if ( IsKedaMt(tMt, FALSE) )
		{
			bIsMtCanAdjust = TRUE;
		}
	}

	return bIsMtCanAdjust;
}

/*==============================================================================
������    :  IsVipMemForVmp
����      :  �����ն�vmp��vip����
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
TChnnlMemberInfo CMcuVcInst::IsVipMemForVmp(const TMt &tMt, TExInfoForResFps &tExInfoForRes)
{
	TChnnlMemberInfo tChnnlMemInfo;
	if (tMt.IsNull())
	{
		return tChnnlMemInfo;
	}

	BOOL32 bIsMMcu = (m_tCascadeMMCU == tMt) || IsNotSupportCascadeMcu(tMt);
	BOOL32 bDStream = FALSE;
	if (VMP_MEMBERTYPE_DSTREAM == tExInfoForRes.m_byMemberType)
	{
		bDStream = TRUE;
	}
	// ����mcu,��ȡ���ϴ�ͨ���ն�,����ǰ���䴦��
	TMt tRealMt = tMt;
	if (IsMcu(tRealMt) && !bDStream && !bIsMMcu) //˫��Դ����ӳ��,MMCUҲ����ӳ��
	{
		tRealMt = GetSMcuViewMt(tRealMt, TRUE);
	}
	BOOL32 bNoneKeda = ( (!IsKedaMt(tMt, TRUE)) || (!tRealMt.IsNull() && !IsKedaMt(tRealMt, FALSE)) || tExInfoForRes.m_bNonKeda );	//�Ƿ��keda�ն�(�����������ǰ��)
	BOOL32 bG400IPCMt = IsG400IPCMt(tRealMt); // 8000H-M��������: IPCǰ�˽�ǰ����ͨ�� [pengguofeng 10/31/2012]

	//��������ķ�����,mcuʱ,������Ӧ�������ϴ�ͨ���ն�
	TMt tRealSpeaker = m_tConf.GetSpeaker();
	if (IsMcu(tRealSpeaker))
	{
		tRealSpeaker = GetSMcuViewMt(tRealSpeaker, TRUE);
	}
	BOOL32 bSpeaker = (!tRealSpeaker.IsNull() && tRealSpeaker == tRealMt);	//�Ƿ��Ƿ�����
	BOOL32 bSelected = IsSelectedbyOtherMtInMultiCas(tRealMt);				//�Ƿ�ѡ��

	tChnnlMemInfo.SetMt(tRealMt);
	// 8000H-M����:���IPCǰ�˱����ǰ����ͨ�� [pengguofeng 11/2/2012]
	if ( bG400IPCMt)
	{
		tChnnlMemInfo.SetAttrIPC();
	}
	// ˫��Դ����������vip����
	else if (bDStream)
	{
		tChnnlMemInfo.SetAttrDstream();
	}
	// �ϼ�MMCU��vmpҲ����������vip����
	else if (bIsMMcu)
	{
		tChnnlMemInfo.SetAttrMMcu();
	}
	else if (bNoneKeda)
	{
		tChnnlMemInfo.SetAttrNoneKeda();
	}
	else
	{
		// ��ǰ�Ƿ�vmp�ڹ㲥
		BOOL32 bIsBrdstVmp = FALSE;
		TMt tVidBrdSrc = GetVidBrdSrc();
		if (!tVidBrdSrc.IsNull() && EQP_TYPE_VMP == tVidBrdSrc.GetEqpType())
		{
			bIsBrdstVmp = TRUE;
		}
		if(bSpeaker && !bIsBrdstVmp)
		{
			tChnnlMemInfo.SetAttrSpeaker();
		}

		BOOL32 bSelectedByRoll = FALSE;		// xliang [5/7/2009] ������ɵ�ѡ������Ϊʱ�����⣬�������⴦����
		if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
			&& ( m_tRollCaller == tMt 
			|| m_tRollCallee == tMt ) )  
		{
			bSelectedByRoll = TRUE;
		}

		if( ( bSelected 
			&& ( (!bIsBrdstVmp && !bSpeaker)	//��VMP�㲥�·����˻���߱�ѡ��
			|| !tExInfoForRes.m_bConsiderVmpBrd ) )
			|| bSelectedByRoll									//������ɵ�ѡ����ͬVMP�㲥����޹�
			)
		{
			tChnnlMemInfo.SetAttrSelected();
		}
	}

	return tChnnlMemInfo;
}

/*==============================================================================
������    :  IsVmpSupportNplus
����      :  ���Nplus���ݶ໭��ϳ�
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsVmpSupportNplus(u8 byVmpSubType, u8 byVmpStyle, u8 &byEqualAbilityVmpId, u8 &bySlightlyLargerAbilityVmpId, u8 &byMinAbilityVmpId)
{
	// �����ж�
	if (byVmpSubType != MPU2_VMP_ENHACED &&
		byVmpSubType != MPU2_VMP_BASIC &&
		byVmpSubType != MPU_SVMP &&
		byVmpSubType != VMP )
	{
		return FALSE;
	}
	byEqualAbilityVmpId = 0;
	bySlightlyLargerAbilityVmpId = 0;
	byMinAbilityVmpId = 0;

	// MPU2_VMP_ENHACED > MPU2_VMP_BASIC > MPU_SVMP > VMP 
	u8 byIdleVMPNum = 0;
	u8 abyIdleVMPId[MAXNUM_PERIEQP];
	memset( abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
	g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );
	u8 byTmpVmpId;
	u8 byTmpVmpSubType;
	TPeriEqpStatus tPeriEqpStatus; 
	u16 wError = 0;
	u8 byTmpSLargerVmpId = 0;
	u8 byTmpSLargerVmpSubType = byVmpSubType;
	u8 byTmpMinVmpId = 0;
	u8 byTmpMinVmpSubType = byVmpSubType;
	for (u8 byIdx=0; byIdx<byIdleVMPNum; byIdx++)
	{
		byTmpVmpId = abyIdleVMPId[byIdx];
		if (g_cMcuVcApp.GetPeriEqpStatus(byTmpVmpId, &tPeriEqpStatus) &&
			IsVMPSupportVmpStyle(byVmpStyle, byTmpVmpId, wError))
		{
			byTmpVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
			if (byTmpVmpSubType == byVmpSubType)
			{
				if (0 == byEqualAbilityVmpId)
				{
					byEqualAbilityVmpId = byTmpVmpId;
				}
			}
			else if (byTmpVmpSubType > byVmpSubType)//�����ȴ�������ǿ
			{
				if (0 == byTmpSLargerVmpId)//��һ�μ�¼��ǿ����id
				{
					byTmpSLargerVmpId = byTmpVmpId;
					byTmpSLargerVmpSubType = byTmpVmpSubType;
				}
				else
				{
					if (byTmpSLargerVmpSubType > byTmpVmpSubType)//֮���бȴ������С�Ļ����£���Ϊ��С
					{
						byTmpSLargerVmpId = byTmpVmpId;
						byTmpSLargerVmpSubType = byTmpVmpSubType;
					}
				}
			}
			else//������С����
			{
				if (0 == byTmpMinVmpId)
				{
					byTmpMinVmpId = byTmpVmpId;
					byTmpMinVmpSubType = byTmpVmpSubType;
				}
				else
				{
					if (byTmpMinVmpSubType > byTmpVmpSubType)//֮���б���ʱ�������С�Ļ�������ʱ��С��������Ϊ��С
					{
						byTmpMinVmpId = byTmpVmpId;
						byTmpMinVmpSubType = byTmpVmpSubType;
					}
				}
			}
		}
	}
	
	// �����Դ��vmpid������С��vmpid
	bySlightlyLargerAbilityVmpId = byTmpSLargerVmpId;
	byMinAbilityVmpId = byTmpMinVmpId;

	// 1����δ�ҵ�
	if (0 == byEqualAbilityVmpId &&
		0 == bySlightlyLargerAbilityVmpId &&
		0 == byMinAbilityVmpId)
	{
		return FALSE;
	}

	return TRUE;
}

/*==============================================================================
������    :  GetVMPBrdBitrate
����      :  ���vmp��Ӧͨ������
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
u16 CMcuVcInst::GetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx)
{
	return g_cMcuVcApp.GetVMPOutChlBitrate(tVmpEqp, byIdx);
}

/*==============================================================================
������    :  SetVMPBrdBitrate
����      :  ����vmp��Ӧͨ������
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::SetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx, u16 wBr)
{
	g_cMcuVcApp.SetVMPOutChlBitrate(tVmpEqp, byIdx, wBr);
}

/*==============================================================================
������    :  SetVMPBrdst
����      :  ����vmp��Ӧ�㲥ģʽ,�����Eqpstatus�еĹ㲥״̬
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::SetVMPBrdst(TEqp tVmpEqp, u8 byVMPBrdst)
{
	g_cMcuVcApp.SetVMPBrdst(tVmpEqp, byVMPBrdst);
	// ͬ������״̬
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(tVmpEqp.GetEqpId() , &tPeriEqpStatus);
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam.SetVMPBrdst(byVMPBrdst);
	g_cMcuVcApp.SetPeriEqpStatus(tVmpEqp.GetEqpId() , &tPeriEqpStatus);
}

/*==============================================================================
������    :  IsMtNeedAdjustFpsInConfVmp
����      :  ����ն��Ƿ��ڽ�֡�ʵ�vmp��
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsMtNeedAdjustFpsInConfVmp(TMt &tMt, TExInfoForResFps &tExInfoForFps)
{
	if (tMt.IsNull())
	{
		return FALSE;
	}
	
	if (tMt.GetType() != TYPE_MT)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsMtNeedAdjustResInConfVmp] dstmt.%d is not mt. type.%d manu.%d\n",
			tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return FALSE;
	}
	
    if ( !m_tConf.IsVmpAutoAdapt() )	//��ǰ���鲻�ǻ���ϳ�����Ӧ
    {
        return FALSE;					//�ն˽�vmp����֡��
    }
	
	// �ն��߼�ͨ���Ƿ�򿪣�˫������ʱ����˫��ͨ��
	u8 byChnnlType = LOGCHL_VIDEO;
	u8 byMemberType = tExInfoForFps.m_byMemberType;
	if (VMP_MEMBERTYPE_DSTREAM == byMemberType)
	{
		byChnnlType = LOGCHL_SECVIDEO;
	}

	TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMt );
	TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( tMt ) ||
		!m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) )
    {
        return FALSE;
    }
	
	u8 byMtStandardFps = tLogicChannel.GetChanVidFPS(); // ����ն�ԭʼ֡��
	if (tLogicChannel.GetChannelType() != MEDIA_TYPE_H264 || byMtStandardFps <= 30)
	{
        return FALSE;
	}

	BOOL32 bIsNeedAdjFps = FALSE;		//�Ƿ�Ҫ����֡��
	
	// �ж��Ƿ���ҪԤ��ָ��VmpParam
	u8 bySpecVmpId = 0;
	if (IsValidVmpId(tExInfoForFps.m_byEqpid))
	{
		bySpecVmpId = tExInfoForFps.m_byEqpid;
	}

	u8 byVmpId;
	TEqp tVmpEqp;
	TVMPParam_25Mem tConfVmpParam;
	u8 byTmpChnlCount = 0;
	u8 byTmpDSChl = 0;
	u8 byVmpSubType;
	for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			continue;
		}
		byVmpId = m_abyVmpEqpId[byIdx];
		byVmpSubType = GetVmpSubType(byVmpId);
		tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
		//�ն���mpu/8kg/8khʱ��Ҫ��֡,8ki�Ȱ�ȫ����֡�������Ժ��ٵ���
		if (MPU_SVMP == byVmpSubType || VMP_8KE == byVmpSubType || VMP_8KH == byVmpSubType /*|| VMP_8KI == byVmpSubType*/)
		{
			// ��ָ������ʱvmpParam��������ʱVmpParam
			if (bySpecVmpId == byVmpId)
			{
				tConfVmpParam = tExInfoForFps.m_tVmpParam;
			}
			else
			{
				tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			}

			// ˫�����治����GetChlOfMtInVmpParam����
			if (VMP_MEMBERTYPE_DSTREAM == byMemberType)
			{
				byTmpDSChl = tConfVmpParam.GetChlOfMemberType(byMemberType);
				if (byTmpDSChl < MAXNUM_VMP_MEMBER)
				{
					byTmpChnlCount = 1;
				}
			}
			else
			{
				GetChlOfMtInVmpParam(tConfVmpParam, tMt, byTmpChnlCount);
			}

			// ����
			if (byTmpChnlCount > 0)
			{
				/* ���8kivmp�����⴦����ǰ����ĳ�Ա����Ҫ��֡
				if (VMP_8KI == byVmpSubType && VMP_MEMBERTYPE_DSTREAM != byMemberType)
				{
					// �ü�¼Ϊ����˫������ͨ������
					if (g_cMcuVcApp.GetVmpAdpChnlCountByMt(tVmpEqp, tMt) > 0)
					{
						continue;
					}
				}*/

				// ����ϸ�������裬MPU2��������趼��ΪҪ��֡��
				bIsNeedAdjFps = TRUE;
				break;
			}
		}
	}

	return bIsNeedAdjFps;
}

/*==============================================================================
������    :  IsMtNeedAdjustResInConfVmp
����      :  ����ն���vmp����Ϣ��ռ��ͨ��������Сͨ���ֱ��ʡ��Ƿ�Ҫ�����ֱ��ʣ�
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsMtNeedAdjustResInConfVmp(const TMt &tMt, TExInfoForResFps &tExInfoForRes, u8 &byVmpChlRes, u8 &byChlCount)
{
	if (tMt.IsNull())
	{
		return FALSE;
	}

	if (tMt.GetType() != TYPE_MT)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsMtNeedAdjustResInConfVmp] dstmt.%d is not mt. type.%d manu.%d\n",
			tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return FALSE;
	}
	
    if ( !m_tConf.IsVmpAutoAdapt() )	//��ǰ���鲻�ǻ���ϳ�����Ӧ
    {
        return FALSE;					//�ն˽�vmp�����ֱ���
    }

	// �ն��߼�ͨ���Ƿ�򿪣�˫������ʱ����˫��ͨ��
	u8 byMemberType = tExInfoForRes.m_byMemberType;
	u8 byChnnlType = LOGCHL_VIDEO;
	if (VMP_MEMBERTYPE_DSTREAM == byMemberType)
	{
		// ˫������ͨ�����ܽ��ֱ���
		byChnnlType = LOGCHL_SECVIDEO;
	}

	TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMt );
	TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( tMt ) ||
		!m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) )
    {
        return FALSE;
    }

	BOOL32 bIsH264Mt = MEDIA_TYPE_H264 == tLogicChannel.GetChannelType();

	BOOL32 bIsNeedAdjust = FALSE;
	u8 byMtStandardFormat = tLogicChannel.GetVideoFormat(); // ����ն�ԭʼ�ֱ���
	byVmpChlRes = byMtStandardFormat;

	// �ж��Ƿ���ҪԤ��ָ��VmpParam
	u8 bySpecVmpId = 0;
	if (IsValidVmpId(tExInfoForRes.m_byEqpid))
	{
		bySpecVmpId = tExInfoForRes.m_byEqpid;
	}
	
	u8 byVmpId;
	TEqp tVmpEqp;
	TVMPParam_25Mem tConfVmpParam;
	TVmpChnnlInfo tVmpChnnlInfo;
	u8 byMtInVmpid = 0;	// ���ն�ֻ��1��vmp��1��ͨ���ڣ���Ǵ�vmpid������ǰ����ռ��
	u8 byMtInVmpPos = MAXNUM_VMP_MEMBER; // ���ն�ֻ��1��vmp��1��ͨ���ڣ���Ǵ�ͨ���ţ�����ǰ����ռ��
	u8 byMtInVmpCount = 0;

	u8 abyChnlNo[MAXNUM_VMP_MEMBER]; // �ҵ�tMt������Сͨ����λ��,����ͳ��Mcu�����ϴ�ͨ���ն�
	memset(abyChnlNo, 0, sizeof(abyChnlNo));
	u8 byTmpChnlCount = 0;
	u8 byPos = MAXNUM_VMP_MEMBER;
	u8 byReqRes = VIDEO_FORMAT_INVALID;
	u8 byTmpDSChl = 0;
	for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			continue;
		}
		byReqRes = VIDEO_FORMAT_INVALID;
		byVmpId = m_abyVmpEqpId[byIdx];
		tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
		// ��ָ������ʱvmpParam��������ʱVmpParam
		if (bySpecVmpId == byVmpId)
		{
			tConfVmpParam = tExInfoForRes.m_tVmpParam;
		}
		else
		{
			tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
		}
		tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
		// �����֧�ֲ����ֱ��ʷ�����ڣ�����ͳ�ƣ�264��ʽ��������ǰ�������ĺϳ���
		if (tConfVmpParam.GetMaxMemberNum() <= tVmpChnnlInfo.GetMaxStyleNum() && bIsH264Mt)
		{
			continue;
		}
		
		// ��ʼ��byTmpChnlCount��abyChnlNo
		memset(abyChnlNo, 0, sizeof(abyChnlNo));
		byTmpChnlCount = 0;

		// ˫�����治����GetChlOfMtInVmpParam����
		if (VMP_MEMBERTYPE_DSTREAM == byMemberType)
		{
			byTmpDSChl = tConfVmpParam.GetChlOfMemberType(byMemberType);
			if (byTmpDSChl < MAXNUM_VMP_MEMBER)
			{
				byTmpChnlCount = 1;
				abyChnlNo[0] = byTmpDSChl;
			}
		}
		else
		{
			GetChlOfMtInVmpParam(tConfVmpParam, tMt, byTmpChnlCount, abyChnlNo);
		}

		// ��H264�ն˱��뽵��CIF
		if (!bIsH264Mt)
		{
			if (byTmpChnlCount > 0)
			{
				byVmpChlRes = VIDEO_FORMAT_CIF;
				bIsNeedAdjust = TRUE;
				break;
			}
			else
			{
				continue;
			}
		}

		for (u8 byChlIdx=0; byChlIdx<byTmpChnlCount; byChlIdx++)
		{
			// ��Ҫ����ʱ�����Ƿ�ȴ洢�ķֱ���С��С�������С�ֱ���
			if (VidResAdjust(byVmpId, tConfVmpParam.GetVMPStyle(), abyChnlNo[byChlIdx], byVmpChlRes, byReqRes))
			{
				byVmpChlRes = byReqRes;
				bIsNeedAdjust = TRUE;
			}
		}
		if (byTmpChnlCount > 0)
		{
			byMtInVmpCount += byTmpChnlCount; //ͨ������
			if (1 == byTmpChnlCount)
			{
				byMtInVmpid = byVmpId;		//��¼��vmpid
				byMtInVmpPos = abyChnlNo[0];//��¼��vmppos
			}
		}
	}

	// ����ͨ����Ϊ1�������ն����ڵ�vmpid
	if (1 == byMtInVmpCount)
	{
		tExInfoForRes.m_byEqpid = byMtInVmpid;
		tExInfoForRes.m_byPos = byMtInVmpPos;
	}
	// ����ͨ����
	byChlCount = byMtInVmpCount;

	return bIsNeedAdjust;
}

/*==============================================================================
������    :  IsNeedAdjustMtVidFps
����      :  �ж��Ƿ���Ҫ�������ն�֡��,����Ҫ��������Ҫ������֡��
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsNeedAdjustMtVidFps(TMt &tMt, TExInfoForResFps &tExInfoForFps)
{
	if (tMt.IsNull())
	{
        return FALSE;
	}
	
	if (tMt.GetType() != TYPE_MT)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFps] dstmt.%d is not mt. type.%d manu.%d\n",
			tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return FALSE;
	}
	
	u8 byEvent = tExInfoForFps.m_byEvent;	//������ʽ
	u8 byOccupy = tExInfoForFps.m_byOccupy;	//ռ��ͨ��/ȡ��ռ��ͨ��
	BOOL32 bConsiderVmpBrd = tExInfoForFps.m_bConsiderVmpBrd;
	BOOL32 bSeizePromt = tExInfoForFps.m_bSeizePromt; //�Ƿ�����ռ������Ҫ��ռ��ʾ
	u8 byEqpid = tExInfoForFps.m_byEqpid;
	u8 byPos = tExInfoForFps.m_byPos;
	BOOL32 bNonKeda = tExInfoForFps.m_bNonKeda;
	u8 byMemberType = tExInfoForFps.m_byMemberType;
	
	// �ն��߼�ͨ���Ƿ�򿪣�˫������ʱ����˫��ͨ��
	u8 byChnnlType = LOGCHL_VIDEO;
	if (VMP_MEMBERTYPE_DSTREAM == byMemberType)
	{
		byChnnlType = LOGCHL_SECVIDEO;
	}
	// ����ϳ�֧�ֵ绰�նˣ�δ��ͨ������Ҫ����ͨ����ԱSetvmpchnl,֧���¼��绰�ն�
	if (byEvent == EvMask_VMP && byOccupy == Occupy_InChannel && LOGCHL_VIDEO == byChnnlType)
	{
		if (IsPhoneMt(tMt))
		{
			tExInfoForFps.m_bResultSetVmpChl = TRUE;
			return FALSE;
		}
	}
	TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMt );
	TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( tMt ) ||
		!m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) )
    {
        return FALSE;
    }
	
	u8 byMtStandardFps = tLogicChannel.GetChanVidFPS(); // ����ն�ԭʼ֡��
	BOOL32 bIsNeedAdjFps = FALSE;		//�Ƿ�Ҫ����֡��
	u8 byNewFps = byMtStandardFps;		//Ҫ��������֡��
	
	// ֻ���H264�ն˵���֡�ʣ���264������֡��,ԭʼ֡��С�ڵ���30֡���ն����轵֡
	if (tLogicChannel.GetChannelType() != MEDIA_TYPE_H264 || byNewFps <= 30)
	{
		tExInfoForFps.m_byNewFps = byNewFps;
		tExInfoForFps.m_bResultSetVmpChl = TRUE;
		return FALSE;
	}
	
	BOOL32 bIsMtCanAdjust = FALSE; 
	if (IsCanAdjustMtFps(tMt))
	{
		bIsMtCanAdjust = TRUE;
	}
	
	// �Ƿ��ڱ������ǽ��
	BOOL32 bInTw = IsMtInTvWall(tMt); //�ն��Ƿ��ڱ������ǽ

	// �Ƿ���hdu-vmp��
	BOOL32 bIsInHduVmp = FALSE;


	u8 byMtInHduFps = 0;
	u8 byMediaMode = (byChnnlType == LOGCHL_VIDEO) ? MODE_VIDEO : MODE_SECVIDEO;

	// [2013/04/08 chenbing] ��HDU�໭���е��ն˲���Ҫ�ж��Ƿ���Ҫ����֡��
	TPeriEqpStatus tHduStatus;
	g_cMcuVcApp.GetPeriEqpStatus(byEqpid, &tHduStatus);
	if (TW_MEMBERTYPE_BATCHPOLL == tHduStatus.m_tStatus.tHdu.GetMemberType(byPos))
	{
		bIsInHduVmp = IsBatchPollMtInHduVmp(tMt,TRUE);
	}
	else
	{
		bIsInHduVmp = IsMtInHduVmp(tMt,0,0,0,FALSE,FALSE,TRUE);
	}
	
	// �Ƿ�����Ҫ��֡�ʵ�vmp��
	TExInfoForResFps tTmpExInfoForFps = tExInfoForFps;
	BOOL32 bIsNeedAdjustFpsInVmp = IsMtNeedAdjustFpsInConfVmp(tMt, tTmpExInfoForFps);
	
	switch (byOccupy)
	{
	case Occupy_InChannel:
		{
			// �ն˲��ܵ�֡��
			if (!bIsMtCanAdjust)
			{
				bIsNeedAdjFps = FALSE;
				if (byEvent == EvMask_VMP)
				{
					if (bIsNeedAdjustFpsInVmp)
					{
						// ��Ҫ����ʱ�����ܽ�����
						tExInfoForFps.m_bResultSetVmpChl = FALSE;
					}
					else
					{
						// �ն˲���Ҫ�������Խ�����
						tExInfoForFps.m_bResultSetVmpChl = TRUE;
					}
				}
				else if (byEvent == EvMask_HDUVMP)//hduvmp�߼�
				{
					// ���ܽ�����
					tExInfoForFps.m_bResultSetVmpChl = FALSE;
				}
				else if (byEvent == EvMask_TVWall && bInTw)
				{
					//tvwall�߼�
					tExInfoForFps.m_bResultSetVmpChl = FALSE;
				}
			}
			else
			{
				tExInfoForFps.m_bResultSetVmpChl = TRUE;//�ɵ�֡�ʵ��նˣ��ɽ�����
				// ��Ҫ��֡��
				if (bInTw || bIsInHduVmp || bIsNeedAdjustFpsInVmp
					|| EvMask_HDUVMP == byEvent
					|| tExInfoForFps.m_bIsForceAdjust)
				{
					bIsNeedAdjFps = TRUE;
					byNewFps = byNewFps/2;
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFps] bInTw: %d bIsNeedAdjustFpsInHduVmp: %d bIsNeedAdjustFpsInVmp: %d\n",
							bInTw, bIsInHduVmp, bIsNeedAdjustFpsInVmp);
				}
			}
		}
		break;
	case Occupy_OutChannel:
		{
			if (!bIsMtCanAdjust)
			{
				bIsNeedAdjFps = FALSE;//���ܵ�֡�ʵ��նˣ�����֡��
			}
			else
			{	
				// ��ͨ��ʱ���ն˲����κ�����ͨ���Żָ�֡��
				if ((!bInTw && !bIsInHduVmp && !bIsNeedAdjustFpsInVmp) || tExInfoForFps.m_bIsForceAdjust)
				{
					bIsNeedAdjFps = TRUE;//��ͨ���ָ�֡��
				}
				else
				{
					// ԭ���ն˽�������ʱ֡�ʻ��Զ������ָ��ֱ���ʱ���ܻ�ͬʱ�ָ�֡�ʣ�
					// ������Ҫ�ָ��ֱ��ʵ���Ҫ��֡�ĳ���
					bIsNeedAdjFps = TRUE;//��ͨ�����ڱ�Ľ�֡�����У��ص�һ��֡��
					byNewFps = byNewFps/2;
				}
			}
		}
		break;
	default:
		{
			// �ǽ���ͨ������ĵ�֡��
			if (bIsMtCanAdjust)
			{
				// ��Ҫ��֡��
				if (bInTw || bIsInHduVmp || bIsNeedAdjustFpsInVmp)
				{
					bIsNeedAdjFps = TRUE;
					byNewFps = byNewFps/2;
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFps] bInTw: %d bIsNeedAdjustFpsInHduVmp: %d bIsNeedAdjustFpsInVmp: %d\n",
						bInTw, bIsInHduVmp, bIsNeedAdjustFpsInVmp);
				}
			}
		}
		break;
	}
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFps] dstmt: %d bIsNeedAdjFps: %d byNewFps: %d m_bResultSetVmpChl: %d\n",
		tMt.GetMtId(), bIsNeedAdjFps, byNewFps, tExInfoForFps.m_bResultSetVmpChl);

	tExInfoForFps.m_byNewFps = byNewFps;
	return bIsNeedAdjFps;
}

/*==============================================================================
������    :  IsNeedAdjustMtVidFormat
����      :  �ж��Ƿ���Ҫ�������ն˷ֱ���,����Ҫ��������Ҫ�����ķֱ���
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsNeedAdjustMtVidFormat(const TMt &tMt, TExInfoForResFps &tExInfoForRes)
{
	if (tMt.IsNull())
	{
        return FALSE;
	}

	if (tMt.GetType() != TYPE_MT)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] dstmt.%d is not mt. type.%d manu.%d\n",
			tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return FALSE;
	}
	
	u8 byEvent = tExInfoForRes.m_byEvent;	//������ʽ
	u8 byOccupy = tExInfoForRes.m_byOccupy;	//ռ��ͨ��/ȡ��ռ��ͨ��
	BOOL32 bConsiderVmpBrd = tExInfoForRes.m_bConsiderVmpBrd;
	BOOL32 bSeizePromt = tExInfoForRes.m_bSeizePromt; //�Ƿ�����ռ������Ҫ��ռ��ʾ
	u8 byEqpid = tExInfoForRes.m_byEqpid;
	u8 byPos = tExInfoForRes.m_byPos;
	BOOL32 bNonKeda = tExInfoForRes.m_bNonKeda;
	u8 byMemberType = tExInfoForRes.m_byMemberType;

	// �ն��߼�ͨ���Ƿ�򿪣�˫������ʱ����˫��ͨ��
	u8 byChnnlType = LOGCHL_VIDEO;
	if (VMP_MEMBERTYPE_DSTREAM == byMemberType)
	{
		byChnnlType = LOGCHL_SECVIDEO;
	}
	// ����ϳ�֧�ֵ绰�նˣ�δ��ͨ������Ҫ����ͨ����ԱSetvmpchnl,֧���¼��绰�ն�
	if (byEvent == EvMask_VMP && byOccupy == Occupy_InChannel && LOGCHL_VIDEO == byChnnlType)
	{
		if (IsPhoneMt(tMt))
		{
			tExInfoForRes.m_bResultSetVmpChl = TRUE;
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] dstmt.%d is Phone mt.\n", tMt.GetMtId());
			return FALSE;
		}
	}
	TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMt );
	TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( tMt ) ||
		!m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) )
    {
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] Open LogicChannel Error!\n");
		return FALSE;
    }

	u8 byMtStandardFormat = tLogicChannel.GetVideoFormat(); // ����ն�ԭʼ�ֱ���
	BOOL32 bIsNeedAdjRes = FALSE;		//�Ƿ�Ҫ�����ֱ���
	u8 byNewRes = byMtStandardFormat;	//Ҫ�������ķֱ���

	// 1.����ն��ڸ����������
	BOOL32 bInTw = IsMtInTvWall(tMt);	//�ն��Ƿ��ڱ������ǽ
	BOOL32 bInHduVmp = FALSE;			//�ն��Ƿ���hdu-vmp�У�hduvmp�ṩ�ӿڣ��ն���hdu-vmp�еķֱ��ʡ�
	u8 byMinResInHduVmp = VIDEO_FORMAT_INVALID;	//����ն���hdu-vmp�е���Сͨ���ֱ���
	u8 byMinFpsInHduVmp = 0;
	
	TPeriEqpStatus tHduStatus;
	g_cMcuVcApp.GetPeriEqpStatus(byEqpid, &tHduStatus);
	if (TW_MEMBERTYPE_BATCHPOLL == tHduStatus.m_tStatus.tHdu.GetMemberType(byPos))
	{
		bInHduVmp = IsBatchPollMtInHduVmp(tMt,TRUE);
	}
	else
	{
		bInHduVmp = IsMtInHduVmp(tMt,0,0,0,FALSE,FALSE,TRUE);
	}
	TSimCapSet tSimCapSet = GetHduChnSupportCap(tMt, tExInfoForRes.m_byEqpid);
	byMinResInHduVmp = tSimCapSet.GetVideoResolution();

	u8 byMinResInVmp = VIDEO_FORMAT_INVALID;	//����ն���vmp�е���Сͨ���ֱ���
	u8 byInVmpCount = 0;						//����ն���vmp��ͨ����������ͳ��֧��ȫ�������ֱ��ʽ�vmp������
	TExInfoForResFps tTmpExInfoForRes = tExInfoForRes;
	BOOL32 bNeedAdjResInVmp = IsMtNeedAdjustResInConfVmp(tMt, tTmpExInfoForRes, byMinResInVmp, byInVmpCount);
	// ֻ��һ��ͨ��ʱ���᷵�ظ�ͨ�����ڵĺϳ���VmpId�������Ϸ�����ѡ��ʱռǰ���䴦��
	u8 byMtInVmpid = 0;
	u8 byMtInVmpPos = MAXNUM_VMP_MEMBER;
	TVmpChnnlInfo tVmpChnnlInfo;
	BOOL32 bIsInVmpHdAdpChl = FALSE;//���ն��Ƿ���vmpǰ����ͨ���У���vmpǰ����ͨ�����ն˷ֱ��ʲ���vmpӰ��
	if (bNeedAdjResInVmp && 1 == byInVmpCount)
	{
		if (IsValidVmpId(tTmpExInfoForRes.m_byEqpid))
		{
			byMtInVmpid = tTmpExInfoForRes.m_byEqpid;
			byMtInVmpPos = tTmpExInfoForRes.m_byPos;
			// ��¼���ն��Ƿ���vmp��ǰǰ�����У����ڽ����������vmpͨ�����ж�
			if (byEvent != EvMask_VMP)
			{
				TEqp tTmpEqp = g_cMcuVcApp.GetEqp(byMtInVmpid);
				tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tTmpEqp);
				TChnnlMemberInfo tVmpChlInfo;
				tVmpChnnlInfo.GetHdChnnlInfoByPos(byMtInVmpPos, &tVmpChlInfo);
				if (!tVmpChlInfo.IsAttrNull())
				{
					bIsInVmpHdAdpChl = TRUE;
				}
			}
		}
	}
	// �������������У����ֱ��ʵ�bStartӦ����ΪFALSE����ʾ�ָ��ֱ���
	if (!bInTw && !bInHduVmp && !bNeedAdjResInVmp)
	{
		tExInfoForRes.m_bResultStart = FALSE;
	}
	else
	{
		tExInfoForRes.m_bResultStart = TRUE;
	}
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] byEvent(%d) byOccupy(%d)!\n", byEvent, byOccupy);
	// 2.�ж��Ƿ�Ҫ�����ն˷ֱ���
	BOOL32 bUpdateVmpAdpChnnl = FALSE;	//���������Ƿ����ǰ����
	switch (byEvent)
	{
	case EvMask_TVWall:
		{
			//�������ǽ����
			if (byOccupy == Occupy_InChannel)//��ǽ�߼�
			{
				// cif�ն���������ֱ���,��cif�ն���Ҫ������cif
				if (VIDEO_FORMAT_CIF != byMtStandardFormat)
				{
					bIsNeedAdjRes = TRUE;
					byNewRes = VIDEO_FORMAT_CIF;
				}
			} 
			else if (byOccupy == Occupy_OutChannel)//��ǽ�߼�
			{
				// cif�ն˲���,��������TW�в���
				if (VIDEO_FORMAT_CIF == byMtStandardFormat || bInTw)
				{
					bIsNeedAdjRes = FALSE;
					byNewRes = VIDEO_FORMAT_CIF;
				}
				else if (!bInHduVmp && !bNeedAdjResInVmp)//����hduͨ��,vmpͨ����������ָ��ն�ԭʼ�ֱ���
				{
					bIsNeedAdjRes = TRUE;
					byNewRes = byMtStandardFormat;
				}
				else if (!bNeedAdjResInVmp)//�ն˽���hdu-vmp�д���
				{
					bIsNeedAdjRes = TRUE;		//�����
					byNewRes = byMinResInHduVmp;//�ֱ��ʸ���HDU-vmpͨ����
				} 
				else if (!bInHduVmp)//�ն˽���vmp�д���
				{
					bIsNeedAdjRes = TRUE;		//�����
					byNewRes = byMinResInVmp;	//�ֱ��ʸ���vmpͨ����
					bUpdateVmpAdpChnnl = TRUE;	//Ӱ��ǰ����
					bSeizePromt &= FALSE;		//������ռ
				}
				else// ͬʱ��hdu-vmp��vmp��
				{
					if (byMinResInHduVmp == byMtStandardFormat) //hdu-vmp�������ն˷ֱ��ʣ���������vmp
					{
						bIsNeedAdjRes = TRUE;		//�����
						byNewRes = byMinResInVmp;	//�ֱ��ʸ���vmpͨ����
						bUpdateVmpAdpChnnl = TRUE;	//Ӱ��ǰ����
						bSeizePromt &= FALSE;		//������ռ
					}
					else //hdu-vmp�����vmp��ĿǰΪ1080�ն�
					{
						// ��hdu��ǰ���䣬���ն�ԭʼ�ֱ��ʸ���Ϊhdu��Ҫ�ĵķֱ��ʣ��ٹ�vmp������
						byMtStandardFormat = byMinResInHduVmp;
						bIsNeedAdjRes = TRUE;		//�����
						byNewRes = byMinResInVmp;	//�ֱ��ʸ���vmpͨ����
						bUpdateVmpAdpChnnl = TRUE;	//Ӱ��ǰ����
						bSeizePromt &= FALSE;		//������ռ
					}
				}
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat]Event[%d], Occupy[%d] is wrong!\n",byEvent, byOccupy);
			}
		}
		break;
	case EvMask_HDUVMP:
		{
			TMt tMtTemp = tMt;
			//hdu-vmp����
			if (byOccupy == Occupy_InChannel)//��ͨ���߼�
			{
				// ��ȡMPEG-4 AUTO�µķֱ���
				if ( VIDEO_FORMAT_AUTO == byMtStandardFormat )
				{
					byMtStandardFormat = GetAutoResByBitrate(VIDEO_FORMAT_AUTO, m_tConf.GetBitRate());
				}

				//�������ܵ��ֱ��ʵ��߼�����
				if ( !IsCanAdjustMtRes(tMtTemp) )
				{
					if (IsSrcResThanDst(byMtStandardFormat, byMinResInHduVmp))
					{
						byNewRes = byMtStandardFormat;
						//���ܽ�����
						tExInfoForRes.m_bResultSetVmpChl = FALSE;
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] Don't adjust SrcRes byNewRes(%d)\n", byNewRes);
						return FALSE;			//�������
					}
					else
					{
						byNewRes = byMtStandardFormat;
						//���Խ�����
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] Don't Need adjust SrcRes byNewRes(%d)!\n", byNewRes);
						return FALSE;		   //�������
					}
				}

				// �¼�Mcu�ϴ�ͨ����ԱΪ��,�����ֱ��� 
				if (tMt.IsLocal())
				{
					 if ( MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()))
					 {
						tMtTemp = GetSMcuViewMt(tMt, TRUE);
						if ( tMtTemp.IsNull() )
						{
							tExInfoForRes.m_bResultSetVmpChl = TRUE;
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] tMt.IsLocal SMcuViewMt is null!\n");
							return FALSE;					 //�������
						}
					 }
				}
 				else
 				{
					TMt tTemp = tMt;
					TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tTemp.GetMcuIdx());
					if (NULL != ptConfMcInfo && MT_TYPE_SMCU == ptConfMcInfo->GetMtExt(tTemp)->GetMtType())
 					{
						tMtTemp = GetSMcuViewMt(tTemp, TRUE);
						if ( tMtTemp.IsNull() )
						{
							tExInfoForRes.m_bResultSetVmpChl = TRUE;
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] OtherSMcu SMcuViewMt is null!\n");
							return FALSE;					 //�������
						}
 					}
 				}

				if (bInTw)//TvWall��
				{
					bIsNeedAdjRes = FALSE;
					byNewRes = VIDEO_FORMAT_CIF;
					tExInfoForRes.m_bResultSetVmpChl = TRUE;
				}
				else if (bNeedAdjResInVmp)//��vmp��
				{
					if ( IsSrcResThanDst(byMinResInVmp, byMinResInHduVmp) )
					{
						bIsNeedAdjRes = TRUE;//�����
						byNewRes = byMinResInHduVmp;//�ֱ���ȡС
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
					else if( bIsInVmpHdAdpChl && IsSrcResThanDst(byMtStandardFormat, byMinResInHduVmp) )
					{
						bIsNeedAdjRes = TRUE;		//��VMPǰ���������
						byNewRes = byMinResInHduVmp;
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
					else
					{
						bIsNeedAdjRes = FALSE;		//�������
						byNewRes = byMinResInVmp;
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
				}
				else if (bInHduVmp)		  // �Ѿ���HDU2�໭����
				{
					//ǿ�Ƶ����ֱ���
					if (tExInfoForRes.m_bIsForceAdjust)
					{
						bIsNeedAdjRes = TRUE;//�����
						byNewRes = byMinResInHduVmp;
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
					else
					{
						bIsNeedAdjRes = FALSE;		  //�������
						byNewRes = byMinResInHduVmp;
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
				}
				else// ����TvWall��vmp,HDU2�໭������
				{
					if ( IsSrcResThanDst(byMtStandardFormat, byMinResInHduVmp) )
					{
						bIsNeedAdjRes = TRUE;//�����
						byNewRes = byMinResInHduVmp;//�ֱ���ȡС
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
					else
					{
						bIsNeedAdjRes = FALSE;		  //�������
						byNewRes = byMtStandardFormat;
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
				}

				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[Occupy_InChannel]bInTw[%d], bNeedAdjResInVmp[%d]\n", bInTw, bNeedAdjResInVmp);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[Occupy_InChannel]byMinResInVmp[%d] byNewRes[%d]\n", byMinResInVmp, byNewRes);
			} 
			else if (byOccupy == Occupy_OutChannel)//��ͨ���߼�
			{
				if (bInTw)		//����ָ�
				{
					bIsNeedAdjRes = FALSE;
					byNewRes = VIDEO_FORMAT_CIF;
				}
				else if (bNeedAdjResInVmp)//��vmp��
				{
					//VMP�еķֱ��ʴ���HDU�໭��ֱ���
					if ( IsSrcResThanDst(byMinResInVmp, byMinResInHduVmp) )
					{
						bIsNeedAdjRes = TRUE;//��ָ�
						byNewRes = byMinResInVmp;//�ֱ���ȡ��
					}
					else if( bIsInVmpHdAdpChl && IsSrcResThanDst(byMtStandardFormat, byMinResInHduVmp) )
					{
						bIsNeedAdjRes = TRUE;		//��VMPǰ���������
						byNewRes = byMtStandardFormat;
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
					else
					{
						bIsNeedAdjRes = FALSE;		//����ָ�
						byNewRes = byMinResInVmp;
					}
				}
				else if (bInHduVmp)//����HDU2�����໭����
				{
					//ǿ�ƻָ��߼�ͨ������
					if (tExInfoForRes.m_bIsForceAdjust)
					{
						bIsNeedAdjRes = TRUE;//�����
						byNewRes = byMtStandardFormat;//�ֱ���ȡ��
					}	
					else
					{
						bIsNeedAdjRes = FALSE;		//����ָ�
						byNewRes = byMinResInHduVmp;
					}
				}
				else// ����TvWall��vmp��
				{
					bIsNeedAdjRes = TRUE;//�����
					byNewRes = byMtStandardFormat;//�ֱ���ȡ��
				}
				tExInfoForRes.m_bResultSetVmpChl = TRUE;
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[Occupy_OutChannel]bInTw[%d], bNeedAdjResInVmp[%d]\n", bInTw, bNeedAdjResInVmp);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[Occupy_OutChannel]byMinResInVmp[%d] byNewRes[%d]\n", byMinResInVmp, byNewRes);
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "byOccupy Error!!!\n");
			}
		}
		break;
	case EvMask_VMP:
		{
			//vmp����
			if (byOccupy == Occupy_InChannel)//��ͨ���߼�
			{
				BOOL32 bChangeVmpStyle = FALSE;
				TChnnlMemberInfo tChnnlMemInfo = IsVipMemForVmp(tMt, tExInfoForRes);//���Բ��ܽ��ֱ��ʵ��⳧���ն�
				if (IsValidVmpId(byEqpid) && (tChnnlMemInfo.IsAttrNull() || tChnnlMemInfo.IsCanAdjResMt()))
				{
					TEqp tTmpVmpEqp = g_cMcuVcApp.GetEqp(byEqpid);
					TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tTmpVmpEqp);
					TVMPParam_25Mem tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tTmpVmpEqp);
					if (tLastVmpParam.GetVMPMode() != CONF_VMPMODE_NONE &&
						tConfVmpParam.GetVMPStyle() != tLastVmpParam.GetVMPStyle())
					{
						bChangeVmpStyle = TRUE;
					}
					
				}
				if (bInTw)//��TW�в���
				{
					bIsNeedAdjRes = FALSE;
					byNewRes = VIDEO_FORMAT_CIF;
					tExInfoForRes.m_bResultSetVmpChl = TRUE;//��Ҫ������
				}
				else if (!bInHduVmp || byMinResInHduVmp == byMtStandardFormat)//����hduvmp�л�hduvmp��Ӱ���ն˷ֱ���
				{
					// ֻ��vmp�У���������ֱ��ʣ�ֱ�ӷ���
					if (!bNeedAdjResInVmp)
					{
						if (bChangeVmpStyle)
						{
							bIsNeedAdjRes = TRUE;		//����л�ʱ���ɴ����е������ֱ��ʷ��ʱ��������ն˷ֱ���
						}
						else
						{
							bIsNeedAdjRes = FALSE;		//�������
						}
						byNewRes = byMinResInVmp;	//�ֱ��ʸ���vmpͨ����
						bUpdateVmpAdpChnnl = FALSE;	//��Ӱ��ǰ����
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
						tExInfoForRes.m_bResultStart = TRUE;
					}
					else{
						bIsNeedAdjRes = TRUE;		//�����
						byNewRes = byMinResInVmp;	//�ֱ��ʸ���vmpͨ����
						bUpdateVmpAdpChnnl = TRUE;	//Ӱ��ǰ����
						bSeizePromt &= TRUE;		//��ռ,���������Ϊ����ռ������ռ
					}
				} 
				else// ͬʱ��hdu-vmp��vmp��,hdu-vmpӰ���ն˷ֱ���
				{
					// ��vmp�У���������ֱ��ʣ�vmpͨ���Ƚϴ�
					if (!bNeedAdjResInVmp)
					{
						if (bChangeVmpStyle)
						{
							bIsNeedAdjRes = TRUE;		//����л�ʱ���ɴ����е������ֱ��ʷ��ʱ��������ն˷ֱ���
						}
						else
						{
							bIsNeedAdjRes = FALSE;		//�������
						}
						byNewRes = byMinResInHduVmp;	//�ֱ���Ϊhdu�����ķֱ���
						bUpdateVmpAdpChnnl = FALSE;	//��Ӱ��ǰ����
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
						tExInfoForRes.m_bResultStart = TRUE;
					}
					else{
						// ��hdu��ǰ���䣬���ն�ԭʼ�ֱ��ʸ���Ϊhdu��Ҫ�ĵķֱ��ʣ��ٹ�vmp������
						byMtStandardFormat = byMinResInHduVmp;
						bIsNeedAdjRes = TRUE;		//�����
						byNewRes = byMinResInVmp;	//�ֱ��ʸ���vmpͨ����
						bUpdateVmpAdpChnnl = TRUE;	//Ӱ��ǰ����
						bSeizePromt &= TRUE;		//��ռ,���������Ϊ����ռ������ռ
					}
				}
			} 
			else if (byOccupy == Occupy_OutChannel)//��ͨ���߼�
			{
				if (bInTw)//��TW�в���
				{
					bIsNeedAdjRes = FALSE;
					byNewRes = VIDEO_FORMAT_CIF;
				}
				else if (!bInHduVmp || byMinResInHduVmp == byMtStandardFormat)//����hduvmp�л�hduvmp��Ӱ���ն˷ֱ���
				{
					if (!bNeedAdjResInVmp)//vmpͨ�������,�ָ��ն˷ֱ���
					{
						bIsNeedAdjRes = TRUE;
						byNewRes = byMtStandardFormat;
					}
					else//vmpͨ����Ҫ��,����vmpͨ��
					{
						bIsNeedAdjRes = TRUE;		//�����
						byNewRes = byMinResInVmp;	//�ֱ��ʸ���vmpͨ����
						bUpdateVmpAdpChnnl = TRUE;	//Ӱ��ǰ����
						bSeizePromt &= FALSE;		//������ռ
					}
				}
				else// ͬʱ��hdu-vmp��vmp��,hdu-vmpӰ���ն˷ֱ���
				{
					if (!bNeedAdjResInVmp)//vmpͨ�������,������hdu-vmp��Ҫ�ķֱ���
					{
						bIsNeedAdjRes = TRUE;
						byNewRes = byMinResInHduVmp;
					}
					else//hdu-vmp��vmp����Ӱ��
					{
						// ��hdu��ǰ���䣬���ն�ԭʼ�ֱ��ʸ���Ϊhdu��Ҫ�ĵķֱ��ʣ��ٹ�vmp������
						byMtStandardFormat = byMinResInHduVmp;
						bIsNeedAdjRes = TRUE;		//�����
						byNewRes = byMinResInVmp;	//�ֱ��ʸ���vmpͨ����
						bUpdateVmpAdpChnnl = TRUE;	//Ӱ��ǰ����
						bSeizePromt &= FALSE;		//������ռ
					}
				}
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat]Event[%d], Occupy[%d] is wrong!\n",byEvent, byOccupy);
			}
		}
		break;
	case EvMask_Speaker:
	case EvMask_BeSelected:
	default:
		{
			// ���¼��������µĵ��ֱ��ʣ�Ӱ�컭��ϳ�
			if (bInTw)//��TW�в���
			{
				bIsNeedAdjRes = FALSE;
				byNewRes = VIDEO_FORMAT_CIF;
			}
			else if (!bInHduVmp || byMinResInHduVmp == byMtStandardFormat)//����hduvmp�л�hduvmp��Ӱ���ն˷ֱ���
			{
				if (!bNeedAdjResInVmp)//vmpͨ�������,�ָ��ն˷ֱ���
				{
					bIsNeedAdjRes = FALSE;
					byNewRes = byMtStandardFormat;
				}
				else//vmpͨ����Ҫ��,����vmpͨ��
				{
					bIsNeedAdjRes = TRUE;		//�����
					byNewRes = byMinResInVmp;	//�ֱ��ʸ���vmpͨ����
					bUpdateVmpAdpChnnl = TRUE;	//Ӱ��ǰ����
					/* �ն�ѡ��ʱ����ռ
					if (EvMask_BeSelected == byEvent && Occupy_BeSelected == byOccupy)
					{
						bSeizePromt &= TRUE;		//ѡ����ռ
					}
					else
					{
						bSeizePromt &= FALSE;		//������ռ
					}*/
				}
			}
			else// ͬʱ��hdu-vmp��vmp��,hdu-vmpӰ���ն˷ֱ���
			{
				if (!bNeedAdjResInVmp)//vmpͨ�������,������hdu-vmp��Ҫ�ķֱ���
				{
					bIsNeedAdjRes = FALSE;
					byNewRes = byMinResInHduVmp;
				}
				else//hdu-vmp��vmp����Ӱ��
				{
					// ��hdu��ǰ���䣬���ն�ԭʼ�ֱ��ʸ���Ϊhdu��Ҫ�ĵķֱ��ʣ��ٹ�vmp������
					byMtStandardFormat = byMinResInHduVmp;
					bIsNeedAdjRes = TRUE;		//�����
					byNewRes = byMinResInVmp;	//�ֱ��ʸ���vmpͨ����
					bUpdateVmpAdpChnnl = TRUE;	//Ӱ��ǰ����
					bSeizePromt &= FALSE;		//������ռ
				}
			}
		}
		break;
	}

	// 3.��������
	// vmpǰ�������
	TVMPParam_25Mem tConfVmpParam;
	TEqp tVmpEqp;
	BOOL32 bIsInVmpAdpChl = FALSE;
	u8 byReqResInVmp = byNewRes;
	if (bUpdateVmpAdpChnnl)
	{
		//1.�ж��Ƿ���VIP�ն�
		TChnnlMemberInfo tChnnlMemInfo = IsVipMemForVmp(tMt, tExInfoForRes);
		
		//2.�ж��ն��Ƿ�Ҫռǰ����
		BOOL32 bSeizeAdpChl = FALSE;
		//ǰ���������H264��ʽ�ģ���H264��ʽ��ռ��vmpǰ����
		if (MEDIA_TYPE_H264 != tLogicChannel.GetChannelType())
		{
			// ����⳧��MPEG4 AUTOͬ����Ҫ�ж��Ƿ���4CIF�ֱ���
			u8 byNoH264Res = byMtStandardFormat;
			if (VIDEO_FORMAT_AUTO == byNoH264Res)
			{
				byNoH264Res = GetAutoResByBitrate(byNoH264Res,m_tConf.GetBitRate());
			}
			//��H264��ʽ��vmp��һ�ɷֱ��ʸ���Ϊcif��mpu2Ҳ��֧�ֽ�mpeg4 4cif
			if (byNoH264Res == VIDEO_FORMAT_4CIF && !tChnnlMemInfo.IsAttrNull() && !tChnnlMemInfo.IsCanAdjResMt())
			{
				// ��H264 4cif����Ϊ���ɽ��ֱ���vip�նˣ����ܵ��ֱ��ʣ����ܽ�����
				bIsNeedAdjRes = FALSE;
				tExInfoForRes.m_bResultSetVmpChl = FALSE;
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat] NoneKeda Mt.(%u,%u) which is MPEG4 with 4CIF in format is not support by VMP!\n",
							tMt.GetMcuId(), tMt.GetMtId() );
				return bIsNeedAdjRes;
			}
			bIsNeedAdjRes = TRUE;
			tExInfoForRes.m_byNewFormat = VIDEO_FORMAT_CIF;
		}
		else
		{
			if (tChnnlMemInfo.IsAttrNull())
			{
				//��vip,��ռǰ����
			}
			else if (tChnnlMemInfo.IsCanAdjResMt())
			{
				// �ɽ��ֱ���vip����ռ��ͨ����,����1��ͨ���ڿ�ռǰ����
				if (1 == byInVmpCount)
				{
					bSeizeAdpChl = TRUE;
					// ����Ƿ����ˣ�ͳ�Ʒ����˸���ͨ������������ͨ������1��������ռ
					// ��ֹ�ڸ��µ�һ������ͨ��ʱ����Ϊ��������1��ͨ���ڣ�����ռǰ���䣬֮����Ҫ�ͷ�ǰ����
					if (tChnnlMemInfo.IsAttrSpeaker())
					{
						u8 byVmpSpeakerNum = GetVmpChnnlNumBySpecMemberType(VMP_MEMBERTYPE_SPEAKER);
						u8 byVmpPollNum = GetVmpChnnlNumBySpecMemberType(VMP_MEMBERTYPE_POLL);
						if ( m_tConf.m_tStatus.GetPollMode() != CONF_POLLMODE_NONE)
						{
							// ��ѯ�ļ�����������ѯ��ý��ģʽ������
							u8 byPollMode = m_tConf.m_tStatus.GetPollInfo()->GetMediaMode();
							if ( byPollMode == MODE_BOTH || byPollMode == MODE_BOTH_BOTH)
							{
								byVmpSpeakerNum += byVmpPollNum;
							}
						}
						if (byVmpSpeakerNum > 1)
						{
							bSeizeAdpChl = FALSE;
						}
					}
				}
				else
				{
					bSeizeAdpChl = FALSE;
				}
			}
			else
			{
				//���ɽ��ֱ���vip
				bSeizeAdpChl = TRUE;
			}
		}
		//3.�ն�ռǰ����
		if (bSeizeAdpChl)
		{
			BOOL32 bSelectedByRoll = FALSE;		// xliang [5/7/2009] ������ɵ�ѡ������Ϊʱ�����⣬�������⴦����
			if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
				&& ( m_tRollCaller == tMt 
				|| m_tRollCallee == tMt ) )  
			{
				bSelectedByRoll = TRUE;
			}
			
			tVmpEqp = g_cMcuVcApp.GetEqp( byMtInVmpid );
			tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			// ����Ҫռǰ����ĺϳ���������1��ͨ��ʱ��byMtInVmpidΪ���ն����ڵ�vmpid�����ڶ�ϳ���ʱ����ʹ�����ڵ����ĺϳ���
			if (!IsValidVmpId(byMtInVmpid) && IsValidVmpId(byEqpid))
			{
				byMtInVmpid = byEqpid;
				byMtInVmpPos = byPos;
				tVmpEqp = g_cMcuVcApp.GetEqp( byMtInVmpid );
				tConfVmpParam = tExInfoForRes.m_tVmpParam;
			}

			// ��δ����vmp������ǰ����ռ�ô���
			if (IsValidVmpId(byMtInVmpid) && byMtInVmpPos < MAXNUM_VMP_MEMBER)
			{
				tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
				BOOL32 bUnableAjustResMt = !tChnnlMemInfo.IsCanAdjResMt();
				// ����ͨ������
				u8 byHdChnnlNum = tVmpChnnlInfo.GetHDChnnlNum();
				u8 byRemainChnlNum = tVmpChnnlInfo.GetMaxNumHdChnnl() - byHdChnnlNum; //ʣ��ǰ�������
				u8 byUsedChnlCount = 0; //��ͨ����ռ�õ�ǰ�������,����Ҫ�滻���ն���ǰ������
				// ���滻�ն���ǰ������,��ռ��ǰ����+1
				TChnnlMemberInfo tTmpMemInfo;
				tVmpChnnlInfo.GetHdChnnlInfoByPos(byMtInVmpPos, &tTmpMemInfo);
				if (!tTmpMemInfo.GetMt().IsNull())
				{
					byUsedChnlCount++;
				}
				// ǰ���䰴��ͨ������ռ�ã�����⳧���ն˽���ͨ�����ⳡ����ÿ�ΰ���Ҫռ1��ǰ����ͳ�ƣ�ֱ������ռ��Ϊֹ
				u8 bySeizeCount = 1 - byUsedChnlCount; //Ҫ��ռ��ǰ�������
				LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[GetMtFormatInMpu]MaxHdChnnlNo:%u,HdChnnlNo:%u,RemainChnlNo:%u.\n",
					tVmpChnnlInfo.GetMaxNumHdChnnl(), byHdChnnlNum, byRemainChnlNum);
				LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[GetMtFormatInMpu]Mtid:%u,AllChnlNo:%u,UsedChnlNo:%u.\n",
					tMt.GetMtId(), byInVmpCount, byUsedChnlCount);
				if (byRemainChnlNum >= bySeizeCount)	//��ʣ��HDǰ����ͨ��������ռ֮
				{
					//��vmp����ǰ����ͨ�����ֱ��ʲ��ý�,����Ҫ����ȥ
					tVmpChnnlInfo.SetHdChnnlInfo(byMtInVmpPos, &tChnnlMemInfo);
					bIsNeedAdjRes = bUnableAjustResMt ? FALSE : TRUE;
					tExInfoForRes.m_bResultSetVmpChl = TRUE;
					byReqResInVmp = byMtStandardFormat;
					bIsInVmpAdpChl = TRUE;
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]Mt.(%u,%u) -> HD adapter channel, no need to change it's video format.\n",
						tMt.GetMcuId(), tMt.GetMtId() );
				}
				else //ǰ������ռ���
				{
					// ���⴦��������˳��ռ��ǰ����ͨ�������ȱ�֤������
					u8 byReplacePos = 0XFF;
					TChnnlMemberInfo tReplaceMemInfo;
					tVmpChnnlInfo.GetReplaceVmpVipChl(tChnnlMemInfo, byReplacePos, tReplaceMemInfo);
					TVMPParam_25Mem tTmpLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
					if (byReplacePos < MAXNUM_VMP_MEMBER && !bSeizePromt)
					{
						// �ڳ���ͨ��ǰ��������ȵ�vip��
						tVmpChnnlInfo.ClearChnlByVmpPos(byReplacePos);
						tVmpChnnlInfo.SetHdChnnlInfo(byMtInVmpPos, &tChnnlMemInfo);
						bIsNeedAdjRes = bUnableAjustResMt ? FALSE : TRUE;
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
						byReqResInVmp = byMtStandardFormat;
						bIsInVmpAdpChl = TRUE;
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]Mt.(%u,%u) -> HD adapter channel, no need to change it's video format.\n",
							tMt.GetMcuId(), tMt.GetMtId() );
						// ����ǰ������Ϣ
						g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
						// �����������滻�����⳧���ն�����𽻻����Ǳ�ѡ���ն����轵�ֱ���
						if (tReplaceMemInfo.IsCanAdjResMt())
						{
							ChangeMtVideoFormat(tReplaceMemInfo.GetMt());
						}
						else
						{
							u8 byMode = MODE_VIDEO;
							if (tReplaceMemInfo.IsAttrDstream())
							{
								byMode = MODE_SECVIDEO;//˫��ռǰ���䣬ModeΪ�ڶ�·��Ƶ
							}
							StopSwitchToPeriEqp(tVmpEqp.GetEqpId(), byReplacePos, FALSE, byMode);
							// ����LastVmpParam����֤�´ε�����ǰ����ʱ������������
							tTmpLastVmpParam.ClearVmpMember(byReplacePos);
							g_cMcuVcApp.SetLastVmpParam(tVmpEqp, tTmpLastVmpParam);
							NotifyMcsAlarmInfo(0, ERR_MCU_UNABLEADJUSTRESMTOUTVMPADPCHL);
							//��������رո�ͨ�����ͷ�ǰ����
							{
								u8 byTmp = 0XFF;//���ֶα���,���mpu��������,Ϊ����,��һ���ֽڱ�ʶ�ر�ͨ��
								u8 byAdd = 0;//��ʶ�ر�ͨ��
								LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]Send MCU_VMP_ADDREMOVECHNNL_CMD To Vmp, byChnnl:%d.\n", byReplacePos);
								CServMsg cTmpMsg;
								cTmpMsg.SetChnIndex(byReplacePos);
								cTmpMsg.SetMsgBody( &byTmp, sizeof(byTmp) );
								cTmpMsg.CatMsgBody( &byAdd, sizeof(byAdd) );
								SendMsgToEqp( tVmpEqp.GetEqpId(), MCU_VMP_ADDREMOVECHNNL_CMD, cTmpMsg);
							}
						}
					}
					// ����Ϊ��Ҫ��ռʱ��������ռ
					else if (bSeizeAdpChl)
					{
						if (!bSeizePromt)
						{
							//������ռʱ������ռ
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]bSeizePromt is %d, no seize operation!\n", bSeizePromt);
							if (bUnableAjustResMt)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat] The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is full! \
									Since VMP mode is [AUTO VMP] mode, the Mt will not enter any channel.\n",
									tMt.GetMcuId(), tMt.GetMtId() );
								bIsNeedAdjRes = FALSE;
							}
						}
						else if (tConfVmpParam.IsVMPAuto())
						{
							//�Զ�����ϳɲ���ռ
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]no seize operation in VMP Auto mode!\n");
							if (bUnableAjustResMt)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat] The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is full! \
									Since VMP mode is [AUTO VMP] mode, the Mt will not enter any channel.\n",
									tMt.GetMcuId(), tMt.GetMtId() );
								bIsNeedAdjRes = FALSE;
							}
						}
						else if( CONF_SPEAKERSRC_CHAIR == m_tConf.GetConfAttrb().GetSpeakerSrc() 
							&& HasJoinedChairman() && (m_tConf.GetChairman() == tMt) && !(tMt == m_tConf.GetSpeaker())
							&& HasJoinedSpeaker()
							)
						{
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]Speaker select see chairman mode, in this mode, no seize prompt is required.\n");
							if (bUnableAjustResMt)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is full! \
									Since VMP mode is [SpeakerSrc Chair] mode, the Mt will not enter any channel.\n",
									tMt.GetMcuId(), tMt.GetMtId() );
								bIsNeedAdjRes = FALSE;
							}
						}
						else if (tConfVmpParam.GetVMPStyle() != tTmpLastVmpParam.GetVMPStyle())
						{
							//�л�vmp���ʱ,����ռ
							//��MPU�Ļ����ж໭��ʱ,ǰ�������,����vip�Ļ�,���һ������ռ,ֱ�ӽ��ֱ���.
							//��֮ǰ��1��vip�ն��ǽ��ֱ��ʵ�,ǰ��������,�з��ʱ��ȫ����,��vip�԰����ֱ��ʴ���
							if (bUnableAjustResMt)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is full! \
									Since VMP mode is [Batch Poll] mode, the Mt will not enter any channel.\n",
									tMt.GetMcuId(), tMt.GetMtId() );
								bIsNeedAdjRes = FALSE;
							}
						}
						else if (bSelectedByRoll)
						{
							//���������Ļ���ϳɲ���ռ
							if (bUnableAjustResMt)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is full! \
									Since VMP mode is [Batch Poll] mode, the Mt will not enter any channel.\n",
									tMt.GetMcuId(), tMt.GetMtId() );
								bIsNeedAdjRes = FALSE;
							}
						}
						else if (0 == tVmpChnnlInfo.GetMaxNumHdChnnl())
						{
							//ǰ����ͨ������Ϊ0ʱ,����ռ
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]no seize operation in 0 vip vmp chnnl!\n");
							//0ǰ����ʱ������ͨ�����ܸ��벻�ɽ��ֱ����նˣ����ܽ�����
							if (bUnableAjustResMt)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is 0!\n",
									tMt.GetMcuId(), tMt.GetMtId() );
								bIsNeedAdjRes = FALSE;
							}
						}
						else if (byInVmpCount > 1 && bUnableAjustResMt)
						{
							//�ǿƴ��ն����������ռ�ö�ͨ��ʱ,ǰ���䲻��,������ռ
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is full! \
								Since the mt is in %d channels of vmp, the Mt will not enter any channel.\n",
								tMt.GetMcuId(), tMt.GetMtId(), byInVmpCount);
							bIsNeedAdjRes = FALSE;
						}
						/*else if ((tConfVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_SPEAKER) == byMtInVmpPos &&
								  tTmpLastVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_SPEAKER) == byMtInVmpPos)
								 || (tConfVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_POLL) == byMtInVmpPos &&
								  tTmpLastVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_POLL) == byMtInVmpPos))
						{
							//���治��ռ
							if (bUnableAjustResMt)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is 0!\n",
									tMt.GetMcuId(), tMt.GetMtId() );
								bIsNeedAdjRes = FALSE;
							}
						}*/
						else	
						{	
							// ��Ҫ��ռ��ʾ
							tExInfoForRes.m_bResultSeizePromt = TRUE;
							tExInfoForRes.m_byEqpid = byMtInVmpid; //������ռvmpid
							tExInfoForRes.m_byPos = byMtInVmpPos;  //������ռͨ��
							bIsNeedAdjRes = FALSE;
						}
						// ռ��ͨ������ռǰ����ʱ,��մ�ͨ��ǰ������Ϣ
						tVmpChnnlInfo.ClearChnlByVmpPos(byMtInVmpPos);
						if (!bIsNeedAdjRes)
						{
							tExInfoForRes.m_bResultSetVmpChl = FALSE;
						}
						else
						{
							tExInfoForRes.m_bResultSetVmpChl = TRUE;
						}
					}
				}
				g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
			}
			else
			{
				// �⳧���ն˽��˶�ͨ�����ַǽ�vmpͨ������
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]Mt.(%u,%u) bSeizeAdpChl:%d, byInVmpCount:%d.\n",
					tMt.GetMcuId(), tMt.GetMtId(), bSeizeAdpChl, byInVmpCount);
				bIsNeedAdjRes = FALSE;
				tExInfoForRes.m_bResultSetVmpChl = FALSE;
			}
		}
		else
		{
			//����ռǰ����
			ClearAdpChlInfoInAllVmp(tMt);
			tExInfoForRes.m_bResultSetVmpChl = TRUE;
			/*if (IsValidVmpId(byMtInVmpid))
			{	
				tVmpEqp = g_cMcuVcApp.GetEqp( byMtInVmpid );
				tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
				if (!tChnnlMemInfo.IsAttrDstream())
				{
					tVmpChnnlInfo.ClearChnlByMt(tMt);
				}
				if (byMtInVmpPos < MAXNUM_VMP_MEMBER)
				{
					tVmpChnnlInfo.ClearChnlByVmpPos(byMtInVmpPos);
				}
				g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
			}*/
		}
		
		//res compare print
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "--------��Mt.(%u,%u) (Channel.%u): Resolution compare��------\n",tMt.GetMcuId(), tMt.GetMtId(), byPos);
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "Original Res: \t%s\n",GetResStr(byMtStandardFormat));
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "Require Res: \t%s\n",GetResStr(byNewRes));
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "======================================================\n");
		if(bNeedAdjResInVmp && bIsInVmpAdpChl )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "Need Adjust: \t0 (Enter HD adapter channel)\n");
			byNewRes = byReqResInVmp;
		}
		else if (!bNeedAdjResInVmp)
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "Need Adjust: \t0 \n");
		}
		else
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "Need Adjust: \t1 \n");
		}
	}

	// ��ֵҪ�������ķֱ���
    tExInfoForRes.m_byNewFormat = byNewRes;
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] dstmt: %d bIsNeedAdjRes: %d byNewRes: %d bResultSetVmpChl: %d bSeizePromt:%d bStart:%d\n",
		tMt.GetMtId(), bIsNeedAdjRes, byNewRes, tExInfoForRes.m_bResultSetVmpChl, tExInfoForRes.m_bResultSeizePromt, tExInfoForRes.m_bResultStart);

	return bIsNeedAdjRes;
}

/*==============================================================================
������    :  GetVmpParamFormBuf
����      :  ����Ϣ�����л��tVmpParam��Ϣ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::GetVmpParamFormBuf(u8 *pbyBuf, u16 wBufLen, TVMPParam_25Mem &tVmpParam)
{
	// ����������Ȳ���С�ڻ���20��Ա��Ϣ
	if (NULL == pbyBuf || wBufLen < sizeof(TVMPParam))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[GetVmpParamFormBuf]Buf is null or Buf length(%d) is not correct.\n", wBufLen);
		return FALSE;
	}
	
	// ������ʱָ�룬ȡ���ϳɷ��
	u8 byMaxMemberNum = 0;
	u8 *pbyTmpBuf = pbyBuf;//m_byVMPAuto
	tVmpParam.SetVMPAuto(*pbyTmpBuf);
	pbyTmpBuf++;//m_byVMPBrdst
	tVmpParam.SetVMPBrdst(*pbyTmpBuf);
	pbyTmpBuf++;//m_byVMPStyle
	tVmpParam.SetVMPStyle(*pbyTmpBuf);
	byMaxMemberNum = tVmpParam.GetMaxMemberNum();
	pbyTmpBuf++;//m_byVMPSchemeId
	tVmpParam.SetVMPSchemeId(*pbyTmpBuf);
	pbyTmpBuf++;//m_byVMPMode
	tVmpParam.SetVMPMode(*pbyTmpBuf);
	pbyTmpBuf++;//m_byRimEnabled
	tVmpParam.SetIsRimEnabled(*pbyTmpBuf);
	pbyTmpBuf++;//m_byVMPBatchPoll
	tVmpParam.SetVMPBatchPoll(*pbyTmpBuf);
	pbyTmpBuf++;//m_byVMPSeeByChairman
	tVmpParam.SetVMPSeeByChairman(*pbyTmpBuf);
	pbyTmpBuf++;//m_atVMPMember
	for (u8 byIdx=0; byIdx<byMaxMemberNum && byIdx<MAXNUM_MPU2VMP_MEMBER; byIdx++)
	{
		tVmpParam.SetVmpMember(byIdx, *(TVMPMember*)pbyTmpBuf);
		pbyTmpBuf += sizeof(TVMPMember);
	}
	return TRUE;
}

/*==============================================================================
������    :  ProcStartVmpReq
����      :  ����ı仭��ϳɲ�������
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-08						Ѧ��							
==============================================================================*/
void CMcuVcInst::ProcStartVmpReq(CServMsg &cServMsg)
{
	TVMPParam_25Mem	tVmpParam;// = *( TVMPParam* )cServMsg.GetMsgBody();
	GetVmpParamFormBuf(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen(), tVmpParam);
	/*u8 byIsVmpModule = 0;
	if (cServMsg.GetMsgBodyLen() > sizeof(TVMPParam))
	{
		byIsVmpModule = *(cServMsg.GetMsgBody() + sizeof(TVMPParam));
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP,"[ProcStartVmpReq]Start vmp by module, IsVmpModule[%d]!\n", byIsVmpModule); 
	}*/
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	u8 byVmpCount = GetVmpCountInVmpList();
	BOOL32 bIsVmpModule = FALSE;//�Ƿ��ǻ���ģ�濪���Ļ���ϳ�,��ģ�棬δռ�úϳ���ʱ������Ϊģ�濪��
	if (tConfAttrb.IsHasVmpModule() && !m_tConfInStatus.IsVmpModuleUsed() && 0 == byVmpCount)
	{
		bIsVmpModule = TRUE;
	}

	if( MT_MCU_STARTVMP_REQ == cServMsg.GetEventId() )
	{		
		// �ն˿�������ϳ�ǰ������Ϊ��ǰ�ϳɸ���Ϊ0
		if (byVmpCount > 0)
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "ChairmanMt start vmp, vmp count:%d, nack!\n" , byVmpCount);
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );					
			return;
		}
		// ���տ�̨�����ĺϳ���Ϣ���и�ʽ��
		FormatVmpParamFromMTCMsg(tVmpParam);
	}

	u8 byVmpId		= cServMsg.GetEqpId();

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "start vmp(Id:%d) param:\n" , byVmpId);
	tVmpParam.Print();

	u16 wError = 0;
	if (!CheckStartVmpReq(byVmpId, tVmpParam, bIsVmpModule, cServMsg, wError))
	{
		cServMsg.SetErrorCode( wError );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(byVmpId, &tPeriEqpStatus);
	if (bIsVmpModule)
	{
		m_tModuleVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
		m_tModuleVmpEqp.SetConfIdx( m_byConfIdx );
	}
	AddVmpIdIntoVmpList(byVmpId);
	tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::RESERVE;//��ռ��,��ʱ��δ�ɹ��ٷ���
	tPeriEqpStatus.SetConfIdx( m_byConfIdx );
	//m_tConf.m_tStatus.SetVmpStyle( tVmpParam.GetVMPStyle() );
	//��������ϳ�ʱ,��ʼ��vmp��ͨ����ѯ״̬
	TMtPollParam tNullPollParam;
	tNullPollParam.SetNull();
	m_tVmpPollParam.SetPollState(POLL_STATE_NONE);
	m_tVmpPollParam.SetMtPollParam(tNullPollParam);
	tVmpParam.SetVMPSeeByChairman(FALSE);	//����ʱ,��ʼ����ϯѡ��vmp״̬,��ֹ��ϯ�ն˿���vmpʱ,����������ֵ
	
	//ACK
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );	
	
	
	//��������ϳ���ǰ���Խ������õ�ת��������жϣ��������ߣ��Զ������µ�
	g_cMcuVcApp.ChkAndRefreshMpForEqp(byVmpId);
	
	/**************************����Ϊ����nack�Ĺ���***********************/

	tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);	//tVmpParam���ո�PeriEqpStatus
	g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );

	m_byVmpOperating = 1;	//��־����VMPԭ�Ӳ���
	
	//��ʼ����ϳ�
	AdjustVmpParam(byVmpId, &tVmpParam, TRUE);
}

/*==============================================================================
������    :  ProcStopVmpReq
����      :  ����ı仭��ϳɲ�������
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::ProcStopVmpReq(CServMsg &cServMsg)
{
	TPeriEqpStatus tPeriEqpStatus;
	u8 byVmpId = cServMsg.GetEqpId();
	if (!IsValidVmpId(byVmpId))
	{
		byVmpId = GetTheOnlyVmpIdFromVmpList();
		if (!IsValidVmpId(byVmpId))
		{
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
		}
	}
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	
	KillTimer(MCUVC_WAIT_ALLVMPPRESETIN_ACK_TIMER+byVmpId-VMPID_MIN);
	// ���ڵȴ�����ϳɿ�ʼ��Ӧ,�ֶ�������Ӧ��ʱ����,�ͷŻ���ϳ���Դ
	if (!tVmpEqp.IsNull() &&
		g_cMcuVcApp.GetPeriEqpStatus(tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
		tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
		(TVmpStatus::RESERVE == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState
		|| TVmpStatus::WAIT_START == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState))
	{
		// ֱ�Ӵ�����ʱ����
		CMessage cMsg;
		//lukunpeng 2010/06/24 �˴������ʼ��
		memset(&cMsg, 0, sizeof(cMsg));
		ProcVmpRspWaitTimer(&cMsg);	
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
		return;
	}
	
	//��鵱ǰ����Ļ���ϳ�״̬ 
	if( g_cMcuVcApp.GetVMPMode(tVmpEqp) == CONF_VMPMODE_NONE)
	{
		cServMsg.SetErrorCode( ERR_MCU_VMPNOTSTART );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//ACK
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
	//���ȴ�����ϳ�ֹͣ��Ӧ,�����ٽ����ͷŴ���
	if (TVmpStatus::WAIT_STOP == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
	{
		return;
	}
	
	TConfAttrb tConfattrb = m_tConf.GetConfAttrb();
	//ֹͣʱģ��ʧЧ
	if( tConfattrb.IsHasVmpModule() && tVmpEqp.GetEqpId() == m_tModuleVmpEqp.GetEqpId())
	{
		TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
		tConfAttrib.SetHasVmpModule(FALSE);
		m_tConf.SetConfAttrb( tConfAttrib );
	}
	
	if( IsValidVmpId(tVmpEqp.GetEqpId()) && m_byCreateBy == CONF_CREATE_NPLUS)
	{
		g_cMcuVcApp.NplusRemoveVmpModuleEqpId(byVmpId);
	}
	
	//ֹͣvmp��ͨ����ѯ
	ProcStopVmpPoll();
	
	u8 byVmpIdx = byVmpId - VMPID_MIN;
	SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER+byVmpIdx, TIMESPACE_WAIT_VMPRSP);
	tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
	g_cMcuVcApp.SetPeriEqpStatus(tVmpEqp.GetEqpId(), &tPeriEqpStatus);
	
	//ֹͣ
	g_cEqpSsnApp.SendMsgToPeriEqpSsn( tVmpEqp.GetEqpId(),MCU_VMP_STOPVIDMIX_REQ, 
		(u8*)cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

	return;
}

/*==============================================================================
������    :  ProcChangeVmpParamReq
����      :  ����ı仭��ϳɲ�������
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-1-22					Ѧ��							create
==============================================================================*/
void CMcuVcInst::ProcChangeVmpParamReq(CServMsg &cServMsg)
{
    TVMPParam_25Mem tVmpParam;
	
	// ����Ϣ���úϳ���Ϣ
	GetVmpParamFormBuf(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen(), tVmpParam);
	if( MT_MCU_CHANGEVMPPARAM_REQ == cServMsg.GetEventId() )
	{
		FormatVmpParamFromMTCMsg(tVmpParam);
	}
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "change vmp param:\n");
	tVmpParam.Print();

	// ��ö�Ӧ�ĺϳ���Id
	u8 byVmpId = cServMsg.GetEqpId();
	
	// ����ʱ����Check����
	u16 wError = 0;
	if (!CheckChangeVmpReq(byVmpId, tVmpParam, wError))
	{
		cServMsg.SetErrorCode( wError );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//��ռ��ʾ���ˣ���ռʱ����Ҫ�ظ�Nack
	if (!CheckAdpChnnlLmt(byVmpId, tVmpParam, cServMsg, TRUE) )
	{
		return;
	}

	//ACK
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
	// xliang [1/6/2009] ��������VMP
	AdjustVmpParam(byVmpId, &tVmpParam);

	return;
}

/*==============================================================================
������    :  ProcStartVmpBrdReq
����      :  �����ϳɹ㲥������
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::ProcStartVmpBrdReq(CServMsg &cServMsg)
{
    TVMPParam_25Mem tVmpParam;
	TPeriEqpStatus tPeriEqpStatus;
	u8 byVmpSubType = 0;  //VMP������
	u8 byLoop = 0;

	u8 byVmpId = cServMsg.GetEqpId();
	if (!IsValidVmpId(byVmpId))
	{
		if (MT_MCU_STARTVMPBRDST_REQ == cServMsg.GetEventId())
		{
			byVmpId = GetTheOnlyVmpIdFromVmpList();
		}
		if (!IsValidVmpId(byVmpId))
		{
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
	}
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	
	//��鵱ǰ����Ļ���ϳ�״̬
	if (CONF_VMPMODE_NONE == g_cMcuVcApp.GetVMPMode(tVmpEqp))
	{
		cServMsg.SetErrorCode( ERR_MCU_VMPNOTSTART );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
	// ��vmp�ڹ㲥������¾�vmp�㲥ģʽ
	BOOL32 bHasOldVmpBrdst = FALSE;
	if (!m_tVmpEqp.IsNull() && tVmpEqp.GetEqpId() != m_tVmpEqp.GetEqpId())
	{
		bHasOldVmpBrdst = TRUE;
		SetVMPBrdst(m_tVmpEqp, FALSE);
		//ˢ����
		TVMPParam_25Mem tOldBrdstVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
		cServMsg.SetEqpId(m_tVmpEqp.GetEqpId());
		cServMsg.SetMsgBody( (u8*)&tOldBrdstVmpParam, sizeof(tOldBrdstVmpParam) );
		SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
	}

	// ������vmp�㲥ģʽ
	SetVMPBrdst(tVmpEqp, TRUE);
	tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	m_tVmpEqp = tVmpEqp;//��¼��vmp�㲥
	
	//��ʼ�㲥����ϳ�����
	ChangeVidBrdSrc(&tVmpEqp);
	//���mpu,�ڷǹ㲥�й㲥ʱ,��Ҫ���µ���vmp������ͨ������
	if (GetVmpSubType(tVmpEqp.GetEqpId()) != VMP)
	{
		AdjustVmpBrdBitRate();
	}
	
	// ֻ�з�vmp��㲥�л�ʱ����Ҫ���������vip
	if (!bHasOldVmpBrdst)
	{
		//8kh ����ǹ㲥�ͷǹ㲥֮���л���1080p30�������vmp�㲥ʱ�迪��1080p30����
		AdjustVmpOutChnlInChgVmpBrd(tVmpEqp, TRUE);//�ǹ㲥��Ϊ�㲥
		
		//���������
		ChgSpeakerResInVmpBrd();
	}

	g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

	//֪ͨ��ظ���ϯ
	cServMsg.SetEqpId(tVmpEqp.GetEqpId());
	cServMsg.SetMsgBody( (u8*)&tVmpParam, sizeof(tVmpParam) );
	SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
	SendVmpParamToChairMan();
	
	//FastUpdate to Vmp
	NotifyFastUpdate(tVmpEqp, 0);
	if (MPU_SVMP == byVmpSubType || VMP_8KE == byVmpSubType || VMP_8KH == byVmpSubType)
	{
		for (u8 byTmpIdx=1; byTmpIdx<MAXNUM_MPU_OUTCHNNL; byTmpIdx++)
		{
			NotifyFastUpdate(tVmpEqp, byTmpIdx);
		}
	}// MPU2/8KIVMPʱ,���պ������������ؼ�֡
	else if (MPU2_VMP_BASIC == byVmpSubType || MPU2_VMP_ENHACED == byVmpSubType || VMP_8KI == byVmpSubType)
	{
		TKDVVMPOutParam tVMPOutParam = g_cMcuVcApp.GetVMPOutParam(tVmpEqp);
		for (u8 byTmpIdx=1; byTmpIdx<tVMPOutParam.GetVmpOutCount(); byTmpIdx++)
		{
			NotifyFastUpdate(tVmpEqp, byTmpIdx);
		}
	}
	
	return;
}

/*==============================================================================
������    :  ProcStopVmpBrdReq
����      :  ֹͣ�ϳɹ㲥������
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::ProcStopVmpBrdReq(CServMsg &cServMsg)
{
    TVMPParam_25Mem tVmpParam;
	TPeriEqpStatus tPeriEqpStatus;
	u8 byVmpSubType = 0;  //VMP������
	u8 byLoop = 0;

	//��鵱ǰ����Ļ���ϳ�״̬
	tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	if( tVmpParam.GetVMPMode() == CONF_VMPMODE_NONE )
	{
		cServMsg.SetErrorCode( ERR_MCU_VMPNOTSTART );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//��黭��ϳɵĹ㲥״̬
	u8 byVmpId = cServMsg.GetEqpId();//�տ�̨��֧�ֹ㲥�ǹ㲥�л�
	if( !tVmpParam.IsVMPBrdst() || byVmpId != m_tVmpEqp.GetEqpId())
	{
		cServMsg.SetErrorCode( ERR_MCU_VMPNOTBRDST );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	//ֹͣ�㲥����ϳ�����
	tVmpParam.SetVMPBrdst(FALSE);
	SetVMPBrdst(m_tVmpEqp, FALSE);				

	if( HasJoinedSpeaker() )
	{
		TMt tTempMt = m_tConf.GetSpeaker();
		if( m_tConf.GetConfAttrb().IsSatDCastMode() &&
			 IsMultiCastMt(tTempMt.GetMtId()) &&
			 GetCurSatMtNum() == 0 && !IsSatMtSend(GetLocalSpeaker()))
		{
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[VmpCommonReq]stopvmpbrd satMtNum(0),so can't recover vidbrdsrc(%d)\n",
				GetCurSatMtNum(),tTempMt.GetMtId());
			ChangeVidBrdSrc( NULL );
		}
		else
		{
			ChangeVidBrdSrc( &tTempMt );
		}
	}
	else
	{
		ChangeVidBrdSrc( NULL );
	}

	//8kh ����ǹ㲥�ͷǹ㲥֮���л���1080p30�������vmp�ǹ㲥ʱ��ͣ1080p30����
	AdjustVmpOutChnlInChgVmpBrd(m_tVmpEqp, FALSE);//�㲥��Ϊ�ǹ㲥

	// MPU��MPU2ʱ,��Ҫ���Ƿ����˽�ǰ����,�����ָ������˷ֱ���
	if(GetVmpCountInVmpList() > 0)
	{
		TMt tSpeaker = m_tConf.GetSpeaker();
		if( HasJoinedSpeaker() && ChgMtVidFormatRequired(tSpeaker))
		{
			ChangeMtVideoFormat( tSpeaker, FALSE, FALSE, TRUE);
		}
	}
	
	//֪ͨ�������ϯ
	cServMsg.SetEqpId(m_tVmpEqp.GetEqpId());
	cServMsg.SetMsgBody( (u8*)&tVmpParam, sizeof(tVmpParam) );
	SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
	// ȡ��vmp�㲥ʱ�����m_tVmpEqp
	m_tVmpEqp.SetNull();
	SendVmpParamToChairMan();

	return;
}

/*==============================================================================
������    :  AdjustVmpOutChnlInChgVmpBrd
����      :  8kh8ki�ϳɹ㲥��Ϊ�ǹ㲥ʱ���⴦��
�㷨ʵ��  :  
����˵��  :  TEqp tVmpEqp [in]
			 BOOL32 bIsVmpBrd [in] �ǹ㲥��Ϊ�㲥ʱ��TRUE���㲥��Ϊ�ǹ㲥ʱ��FALSE
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::AdjustVmpOutChnlInChgVmpBrd(TEqp tVmpEqp, BOOL32 bStartVmpBrd)
{
	u8 byVmpSubtype = GetVmpSubType(tVmpEqp.GetEqpId());
	BOOL32 bNeedChgVmpOutChl = FALSE;
	
	//8kh ����ǹ㲥�ͷǹ㲥֮���л�
	if (VMP_8KH == byVmpSubtype)
	{
		//�������
		if (IsConfExcludeDDSUseAdapt())
		{
			//�ж�����ʽ��1080p30�������vmp�ǹ㲥ʱ��ͣ1080p30����
			if (emBpAttrb == m_tConf.GetProfileAttrb() &&
				m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
				m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 &&
				m_tConf.GetMainVidUsrDefFPS() >= 25 &&
				m_tConf.GetMainVidUsrDefFPS() <= 30)
			{
				bNeedChgVmpOutChl = TRUE;
			}
			// 720p30fpsHP�������,�㲥�ǹ㲥���л�,��Ҫ�ı�vmp���ͨ��
			if (emHpAttrb == m_tConf.GetProfileAttrb() &&
				m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
				m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD720 &&
				m_tConf.GetMainVidUsrDefFPS() >= 25 &&
				m_tConf.GetMainVidUsrDefFPS() <= 30)
			{
				bNeedChgVmpOutChl = TRUE;
			}
		}
	}
	else if (VMP_8KI == byVmpSubtype)
	{
		//�������
		if (IsConfExcludeDDSUseAdapt())
		{
			// 1080P/i 25/30fps�������vmp�ǹ㲥ʱ��ͣ1080p30����
			if (m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
				m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 &&
				m_tConf.GetMainVidUsrDefFPS() >= 25 &&
				m_tConf.GetMainVidUsrDefFPS() <= 30)
			{
				bNeedChgVmpOutChl = TRUE;
			}
			// 720P 50/60fps�������,�㲥�ǹ㲥���л�,��Ҫ�ı�vmp���ͨ��
			if (m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
				m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD720 &&
				m_tConf.GetMainVidUsrDefFPS() > 30)
			{
				bNeedChgVmpOutChl = TRUE;
			}
		}
	}
	
	if (bNeedChgVmpOutChl)
	{
		if (VMP_8KI == byVmpSubtype)
		{
			// ���µ�0·�������ʣ��Ƿ�ֹͣ����ı�־
			u8 byVmpOutChnnl = 0;
			u16 wMinBitRate = 0;
			if (bStartVmpBrd)// �ǹ㲥�й㲥ʱ��ȡ��Ҫ�����ʣ��㲥�зǹ㲥ʱ������0
			{
				wMinBitRate = GetMinMtRcvBitByVmpChn(tVmpEqp.GetEqpId(), TRUE, byVmpOutChnnl);
			}
			g_cMcuVcApp.SetVMPOutChlBitrate(tVmpEqp, byVmpOutChnnl, wMinBitRate);
		}

		// �������ǽѡ��vmp����
		u8 byHduChnNum = 0;
		TPeriEqpStatus tHduStatus;
		TMt tMtInTv;
		for(u8 byEqpHduId = HDUID_MIN; byEqpHduId <= HDUID_MAX; byEqpHduId++)
		{
			byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byEqpHduId));
			if (0 == byHduChnNum)
			{
				//ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[VmpCommonReq] GetHduChnNumAcd2Eqp2 failed!\n");		
				continue;
			}
			
			if (g_cMcuVcApp.GetPeriEqpStatus(byEqpHduId, &tHduStatus) &&
				tHduStatus.m_byOnline )
			{
				for(u8 byLp = 0; byLp < byHduChnNum; byLp++)
				{
					tMtInTv = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byLp];
					if(tMtInTv.GetConfIdx() == m_byConfIdx)
					{
						// �����ж��ն����ڻ���ţ�����Ὣ����������ն�����
						if(tMtInTv == tVmpEqp)
						{
							// [2013/03/11 chenbing] HDU�໭��Ŀǰ��֧��ѡ��VMP,��ͨ����0
							ChangeHduSwitch(&tVmpEqp, byEqpHduId, byLp, 0, TW_MEMBERTYPE_SWITCHVMP, TW_STATE_START);
						}
					}
				}
			}
		}
		
		// �ر�/�򿪵�0ͨ������
		u8 byVmpOutChnnl = 0;
		CServMsg cMsg;
		u8 abyOutChnlActive[MAXNUM_MPU_OUTCHNNL];//���ͨ���Ƿ�Active,EmVmpOutChnlStatus
		memset(abyOutChnlActive,emVmpOutChnlNotChange,sizeof(abyOutChnlActive));
		// ����vmp�㲥ʱ������ͨ��
		if (bStartVmpBrd)
		{
			abyOutChnlActive[byVmpOutChnnl] = emVmpOutChnlActive;	//����
		}
		else
		{
			// ֹͣvmp�㲥ʱ���ر�ͨ��
			abyOutChnlActive[byVmpOutChnnl] = emVmpOutChnlInactive;	//�ر�
		}
		for (u8 byIdx=0; byIdx<sizeof(abyOutChnlActive); byIdx++)
		{
			LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"VMP Outchnl[%d] Active: %d\n", byIdx,abyOutChnlActive[byIdx]);
		}
		cMsg.SetConfId(m_tConf.GetConfId());
		cMsg.SetMsgBody(abyOutChnlActive,sizeof(abyOutChnlActive));
		SendMsgToEqp(tVmpEqp.GetEqpId(), MCU_VMP_STARTSTOPCHNNL_CMD, cMsg);
	}

	return;
}

/*==============================================================================
������    :  ChgSpeakerResInVmpBrd
����      :  vmp�㲥���������
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::ChgSpeakerResInVmpBrd()
{
	//���������
	TEqp tTmpVmpEqp;
	TVmpChnnlInfo tVmpChnnlInfo;
	u8 byHdChnnlNum;
	TChnnlMemberInfo tChnnlMemInfo;
	BOOL32 bChgSpeakerRes = FALSE;
	u8 byLoop = 0;
	BOOL32 bIsAdpFull = FALSE;
	TVMPParam_25Mem tVmpParam;
	for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			continue;
		}
		tTmpVmpEqp = g_cMcuVcApp.GetEqp( m_abyVmpEqpId[byIdx] );
		tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tTmpVmpEqp);
		byHdChnnlNum = tVmpChnnlInfo.GetHDChnnlNum();
		// ǰ���������������ͷ�1·ǰ����ʱ�����Ե���һ��vmp��Ա����֮ǰ�����Ĳ��ɽ��ֱ����ն�ռǰ����
		bIsAdpFull = byHdChnnlNum >= tVmpChnnlInfo.GetMaxNumHdChnnl();
		
		for(byLoop = 0; byLoop < byHdChnnlNum; byLoop ++)
		{
			tVmpChnnlInfo.GetHdChnnlInfo(byLoop, &tChnnlMemInfo);
			
			//֮ǰChangeVidBrdSrc(&m_tVmpEqp)�л��StopSelectSrc(),�ú����Ὣ��ѡ����MT�˳�ǰ����ͨ��,��������ֻ��Է���VIP������
			if( tChnnlMemInfo.IsAttrSpeaker() )
			{
				//����Mt�ֱ���
				bChgSpeakerRes = TRUE;
				ChangeMtVideoFormat(tChnnlMemInfo.GetMt());
				break;//���ܽ����ͨ��
			}
		}
		if (bChgSpeakerRes)
		{
			if (bIsAdpFull)
			{
				// ��ͣ������ͨ����Ҫ�����²Σ���֤��������ǰ����
				tVmpParam = g_cMcuVcApp.GetConfVmpParam(tTmpVmpEqp);
				AdjustVmpParam(tTmpVmpEqp.GetEqpId(), &tVmpParam, FALSE, FALSE);
			}
			// �ͷŷ�����ǰ���䣬�Զ�����һ��ǰ���䣬����Ҫ��ǰ������ն˽�ǰ����
			//AdjustVmpHdChnlInfo(tTmpVmpEqp);

			break;
		}
	}

	return;
}

/*==============================================================================
������    :  ForamtVmpParamFromMTCMsg
����      :  ���տ�̨�����ĺϳ���Ϣ���и�ʽ��
�㷨ʵ��  :  
����˵��  :  TVMPParam_25Mem &tVmpParam in/out
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::FormatVmpParamFromMTCMsg(TVMPParam_25Mem &tVmpParam)
{
	TVMPMember *ptVmpMember = NULL;
	for( u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); ++byLoop )
	{
		ptVmpMember =  tVmpParam.GetVmpMember(byLoop);
		if( ptVmpMember == NULL || ptVmpMember->IsNull() )
		{
			continue;
		}
		if( ptVmpMember->GetMcuId() == 0 && ptVmpMember->GetMtId() == 0 )
		{
			tVmpParam.GetVmpMember(byLoop)->SetMcuId(0);
			tVmpParam.GetVmpMember(byLoop)->SetMtId(0);
		}
		else
		{
			tVmpParam.GetVmpMember(byLoop)->SetMcuId( 
				m_ptMtTable->GetMt( ptVmpMember->GetMtId() ).GetMcuId()
				);
		}
	}

	return;
}

/*==============================================================================
������    :  CheckStartVmpReq
����      :  ����ϳɿ���ʱ�ĸ���Check
�㷨ʵ��  :  
����˵��  :  u8 &byVmpId in/out
			 TVMPParam_25Mem &tVmpParam in/out
			 BOOL32 bIsVmpModule,
			 CServMsg &cServMsg
			 error code
����ֵ˵��:  BOOL32
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130527					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::CheckStartVmpReq(u8 &byVmpId, TVMPParam_25Mem &tVmpParam, BOOL32 bIsVmpModule, CServMsg &cServMsg, u16 &wError)
{
	wError = 0;
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	TPeriEqpStatus tPeriEqpStatus;
	u8 byVmpCount = GetVmpCountInVmpList();
	// ָ��vmpid����£���������״̬��Ϊ���У�������������
	if (!tVmpEqp.IsNull())
	{
		if (g_cMcuVcApp.GetPeriEqpStatus(tVmpEqp.GetEqpId(), &tPeriEqpStatus) &&
			TVmpStatus::IDLE != tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
		{
			wError = ERR_MCU_VMPRESTART;
			return FALSE;
		}
	}
	else
	{
		//��鵱ǰ����Ļ���ϳ�״̬,ģ�濪���Զ��ϳ�ʱ,Mode��Ϊ��,������
		if (IsVmpStatusUseStateInVmpList(TVmpStatus::WAIT_START) ||
			IsVmpStatusUseStateInVmpList(TVmpStatus::RESERVE))
		{
			wError = ERR_MCU_VMPRESTART;
			return FALSE;
		}
	}

	if (!FindVmpSupportVmpParam(byVmpId, tVmpParam.GetVMPStyle(), byVmpCount, wError))
	{
		return FALSE;
	}

	// vmp������vicp��Դռ���жϣ����������б�
	if (!PrepareVmpOutCap(byVmpId, tVmpParam.IsVMPBrdst()))
	{
		TKDVVMPOutParam tVMPOutParam;
		g_cMcuVcApp.SetVMPOutParam(tVmpEqp, tVMPOutParam);
		//ģ�濪��vmpʧ�ܺ�,VMPģ�岻����Ч
		m_tConfInStatus.SetVmpModuleUsed(TRUE);
		wError = ERR_VMP_VICP_NOT_ENOUGH;
		return FALSE;
	}

	BOOL32 bIsVmpCreateByNPlus = FALSE;
	if((m_byCreateBy == CONF_CREATE_NPLUS && g_cMcuVcApp.NPlusIsVmpOccupyByConf(byVmpId,m_byConfIdx)))
	{
		bIsVmpCreateByNPlus = TRUE;
		ConfPrint(LOG_LVL_DETAIL,MID_MCU_NPLUS,"===============N+1 correct byVmpId.%d==========\n",byVmpId);
	}

	//�ϳɲ���У��
	// ����ģ�濪���Ļ���ϳ�,�����ն������ں���,�����ϳɲ���У��
	if (!bIsVmpModule && 
		!bIsVmpCreateByNPlus)
	{
		if (!CheckVmpParam(byVmpId, tVmpParam, wError ) )
		{
			return FALSE;
		}
	}

	g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	u8 byVmpDynStyle;
	if (tVmpParam.IsVMPAuto() )
	{
		// [pengjie 2010/3/30] ��ش��������¼���mcu����Ҫ���ϳ�
		u8 byMtNum =  m_tConfAllMtInfo.GetLocalJoinedMtNum();

		// [7/2/2010 xliang] ���ֵ��ش�/��ش�, ȷ���Ƿ�Ҫ���¼������ն�����˴κϳɳ�Ա
		// [pengjie 2010/3/22] ���ﻹҪ�жϷ������Ƿ����
		TMt tSpeaker = m_tConf.GetSpeaker();
		if( !tSpeaker.IsNull() && !tSpeaker.IsLocal() )
		{
			if( IsLocalAndSMcuSupMultSpy(tSpeaker.GetMcuId()) )
			{
				byMtNum ++;
			}
		}
		byVmpDynStyle = GetVmpDynStyle(byVmpId, byMtNum);
		if (VMP_STYLE_NONE == byVmpDynStyle)
		{
			wError = ERR_INVALID_VMPSTYLE;
			return FALSE;
		}
		else
		{
			// ������Զ�����ľ���ϳɷ�ʽ	
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "auto vmp style is confirmed to style:%u\n", byVmpDynStyle); 
			tVmpParam.SetVMPStyle( byVmpDynStyle );
		}
	} // ����vmp����CheckMpuMember����,������vmp
	else
	{
		// MPU��ͨ����Ա���ƣ����Զ�ģʽ�£�
		if ( !CheckMpuMember(byVmpId, tVmpParam, wError ) )
		{
			//ģ�濪��vmpʧ�ܺ�,VMPģ�岻����Ч
			m_tConfInStatus.SetVmpModuleUsed(TRUE);
			return FALSE;
		}
	}
	
	// xsl [7/20/2006]���Ƿ�ɢ������Ҫ���ش�ͨ����
	if (m_tConf.GetConfAttrb().IsSatDCastMode()) 
	{
		// ���Ƿ�ɢ����֧�ֶ�vmp
		if (/*byVmpCount > 0 || */!IsSatMtCanContinue(0,emStartVmp,NULL,0xFF,0xFF,&tVmpParam))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[VmpCommonReq] over max upload mt num. nack!\n");
			//ģ�濪��vmpʧ�ܺ�,VMPģ�岻����Ч
			m_tConfInStatus.SetVmpModuleUsed(TRUE);
			wError = ERR_MCU_DCAST_OVERCHNNLNUM; 
			return FALSE;
		}
	}
	
	//�����и���ǰ����ͨ����VMP��
	//�ڿ�ʼ֮ǰ����vmp param�Ƿ��г���VMPǰ����ͨ������,�������߳���Ա�������岻�ܣ��Զ�����ϳɳ��⣩
	//VIP����У����������ȼ���ߡ�
	//����8000H�п��ܻ����ǰ����ͨ��Ϊ0���������ʱ�������һ����Ҫǰ������ն˽�����ϳɣ�����ϳɲ�Ӧ�ÿ���
	if (!CheckAdpChnnlLmt(byVmpId, tVmpParam, cServMsg))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[VmpCommonReq] Vip vmp Chnnl is not enough. nack!\n");
		wError = ERR_VMP_NO_VIP_VMPCHNNL; 
		return FALSE;
	}
	
	return TRUE;
}


/*==============================================================================
������    :  FindVmpSupportVmpParam
����      :  �Һ���vmpid����ָ��vmpidʱ���Ƿ�֧��
�㷨ʵ��  :  
����˵��  :  u8 &byVmpId in/out
			 u8 byVmpStyle in
			 u8 byVmpCount in
����ֵ˵��:  error code
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130527					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::FindVmpSupportVmpParam(u8 &byVmpId,u8 byVmpStyle, u8 byVmpCount, u16 &wError)
{
	wError = 0;
	// �����Ƿ�ָ����vmpid����ָ���˿���ָ��vmp
	if (IsValidVmpId(byVmpId))
	{
		if (!IsVMPSupportVmpStyle(byVmpStyle, byVmpId, wError))
		{
			return FALSE;
		}
	}
	else
	{
		// Ŀǰҵ��δָ��vmpid�����£�ֻ����1��vmp
		if (byVmpCount > 0)
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_VMP, "start vmp(Id:%d), vmp count:%d, nack!\n" , byVmpId, byVmpCount);					
			wError = ERR_MCU_VMPRESTART;
			return FALSE;
		}
		//ѡVMP
		u8 byIdleVMPNum = 0;
		u8 abyIdleVMPId[MAXNUM_PERIEQP];
		memset( abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
		g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );
		if( byIdleVMPNum == 0 )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP,"[VmpCommonReq]No Idle VMP!\n"); 
			wError = ERR_MCU_NOIDLEVMP;
			return FALSE;
		}
		else
		{
			if ( !IsMCUSupportVmpStyle(byVmpStyle, byVmpId, EQP_TYPE_VMP, wError) )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

/*==============================================================================
������    :  UpdateVmpMembersWithConfInfo
����      :  ���ݻ�����Ϣ����vmp��Ա
�㷨ʵ��  :  
����˵��  :  u8 byVmpId in
			 u8 byVmpStyle in
			 u8 byVmpCount in
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::UpdateVmpMembersWithConfInfo(const TEqp &tVmpEqp, TVMPParam_25Mem& tVmpParam)
{
	TMt tMt;
	tMt.SetNull();
	u8 byLoop = 0;
	TVMPMember tVMPMember;
	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	for( byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		if (NULL == tVmpParam.GetVmpMember( byLoop ))
		{
			tVMPMember.SetNull();
		}
		else
		{
			tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		}
		//20110607 zjl �߶�  ��ԱΪ��ʱ��Ա����Ҳ�ǿգ�û��Ҫ��ѭ����continue����
		if( !tVMPMember.IsNull() || tVMPMember.GetMemberType() != VMP_MEMBERTYPE_MCSSPEC)
		{
			if( ( VMP_MEMBERTYPE_CHAIRMAN == tVMPMember.GetMemberType() && !HasJoinedChairman() ) 
				|| ( VMP_MEMBERTYPE_SPEAKER == tVMPMember.GetMemberType() 
					&& // ���� [5/23/2006] ֻ�����ն˽���ͨ��������������豸
					( !HasJoinedSpeaker() || m_tConf.GetSpeaker().GetType() == TYPE_MCUPERI || IsVrsRecMt(m_tConf.GetSpeaker())) )
				||( VMP_MEMBERTYPE_POLL == tVMPMember.GetMemberType()  && 
					(POLL_STATE_NONE == m_tConf.m_tStatus.GetPollState() /*||
 					 // xliang [3/30/2009] ��ѯ�������,����Param. ����֮���ж�ͬһ��MT�����ͨ�����ƻ�����
				     (!tTmpPollMt.IsNull() && !(tTmpPollMt == tTmpMt))*/))
				||( VMP_MEMBERTYPE_DSTREAM == tVMPMember.GetMemberType() && 
					m_tDoubleStreamSrc.IsNull())//˫������ͨ��,��˫��Դʱ,���
			  )
			{
				tVMPMember.SetMemberTMt( tMt );
			}
			// 2011-8-16 add by pgf �����˸������ѯ��Ƶ���棬�ٿ���������ѯ����Ƶ��������ͨ����ѯ��ͬ��
			// ֧���ն˽�vmp��ͨ��ʱ,����Ҫͬ��,�ն˲�֧�ֽ�vmp��ͨ��ʱ,�����ն�����vmp����ͨ����,���������ѯ����ͨ��
			else if ( tVMPMember.GetMemberType() == VMP_MEMBERTYPE_POLL)
			{
				// ����ǻ�����ѯ�����ĵ�ǰ��ѯ�����ն�
				if ( m_tConf.m_tStatus.GetPollState() != POLL_STATE_NONE)
				{
					TMt tmpTmt;
					tmpTmt.SetNull();
					// 2011-12-2 add by pgf: ��ϯ��ѯ���ϴ���ѯʱ����������VMP�ĳ�Ա�ı�
					if ( m_tConf.m_tStatus.GetPollInfo()->GetMediaMode() != MODE_VIDEO_CHAIRMAN
						&& m_tConf.m_tStatus.GetPollInfo()->GetMediaMode() != MODE_BOTH_CHAIRMAN
						&& m_tConf.m_tStatus.GetPollInfo()->GetMediaMode() != MODE_VIDEO_SPY
						&& m_tConf.m_tStatus.GetPollInfo()->GetMediaMode() != MODE_BOTH_SPY
						)
					{
						tmpTmt = m_tConfPollParam.GetCurrentMtPolled()->GetTMt();
					}

					// �����ն˽�vmp��ͨ��,��ͨ���ӷ���ѯ��Ϊ��ѯ����ʱ,������ѯ����ͨ����Ա
					if (IsAllowVmpMemRepeated(tVmpEqp.GetEqpId()) || 
						!tConfVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL))
					{
						tVMPMember.SetMemberTMt(tmpTmt);
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[CheckVmpParam] chnnl.%d,type.%d,member is<%d,%d>\n",
							byLoop, VMP_MEMBERTYPE_POLL, tmpTmt.GetMcuId(), tmpTmt.GetMtId());
					}
				}
			}
			// 2011-8-16 add end
			else if (tVMPMember.GetMemberType() == VMP_MEMBERTYPE_MCSSPEC && !m_tConfAllMtInfo.MtJoinedConf(tVMPMember))
			{
				tVMPMember.SetMemberTMt( tMt );
			}		
			else if( VMP_MEMBERTYPE_SPEAKER == tVMPMember.GetMemberType() 
					&& HasJoinedSpeaker() 
					&& !(m_tConf.GetSpeaker().GetType() == TYPE_MCUPERI || IsVrsRecMt(m_tConf.GetSpeaker())) 
					)
			{
				//tVMPMember.SetMemberTMt(GetLocalSpeaker());	// [1/18/2010 xliang] ����local�ĳ�Ա
				tVMPMember.SetMemberTMt(m_tConf.GetSpeaker());
			}
			// ˫������ͨ��,��˫��Դʱ����ͨ����Ա
			else if (tVMPMember.GetMemberType() == VMP_MEMBERTYPE_DSTREAM 
					&& !m_tDoubleStreamSrc.IsNull())
			{
				// ¼���������vmp,vrs��¼��Ҳ����
				if (TYPE_MCUPERI == m_tDoubleStreamSrc.GetType() || IsVrsRecMt(m_tDoubleStreamSrc))
				{
					tVMPMember.SetMemberTMt( tMt );
				}
				else{
					TMt tRealDSMt = GetConfRealDsMt();
					if (tRealDSMt.IsNull())
					{
						tRealDSMt = m_tDoubleStreamSrc;
					}
					tVMPMember.SetMemberTMt( tRealDSMt );
				}
			}
			//zjj20100428 �����и���ԱУ�飬У�������setin
			else if ( tVMPMember.GetMcuIdx() != LOCAL_MCUIDX )
			{
				// [1/18/2010 xliang]  ������SETIN ���սϴ�������. ����˴�����ԭ�г�Ա��Ϣ�������б���ת��
				// ���ڼ������⡣��MCU�����ֱ���ԭ�г�Ա��Ϣ����һ��Ҫͳһ��
				// ���Լ�ʹ�����¼�mcu��֧�ֶ�ش�������Ҳ���ٽ��г�Աת����
				
				/*if ( !IsSMcuSupMultSpy(tVMPMember.GetMcuId()) )
				{
					OnMMcuSetIn(tVMPMember, 0, SWITCH_MODE_SELECT);
				}*/
				
			}
			else
			{
				tVMPMember.SetMemberTMt( m_ptMtTable->GetMt(tVMPMember.GetMtId()) );
			}
			tVmpParam.SetVmpMember( byLoop, tVMPMember );
		}
	}

	return;
}

/*==============================================================================
������    :  UpdateVmpMembersWithConfInfo
����      :  �����ָ���ĳ�Ա�Ƿ��е��ش�mcu�µ��ն�ռ�ö��ͨ�������
�㷨ʵ��  :  
����˵��  :  u8 byVmpId in
			 u8 byVmpStyle in
			 u8 byVmpCount in
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130527					yanghuaizhi							
==============================================================================*/
u16 CMcuVcInst::UpdateVmpMembersWithMultiSpy(const TEqp &tVmpEqp, TVMPParam_25Mem& tVmpParam)
{
	u16 wErrorCode = 0;
	u8 byLoop1 = 0;
	u8 byTempMcuId = 0;
	u8 byTempMcuId1 = 0;
	TVMPMember tVMPMember;
	TVMPMember tVMPMember1;
	TVMPMember tTempMem;
	TVMPMember* pvmpMemLast = NULL;
	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	BOOL32 bIsAllowVmpMemRepeated = IsAllowVmpMemRepeated(tVmpEqp.GetEqpId());
	for(u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		if (NULL == tVmpParam.GetVmpMember( byLoop ))
		{
			tVMPMember.SetNull();
		}
		else
		{
			tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		}


		if( tVMPMember.IsNull() || !tVMPMember.IsLocal() && IsLocalAndSMcuSupMultSpy(tVMPMember.GetMcuId()))
		{
			continue;
		}

		//����˫������ͨ��
		if (VMP_MEMBERTYPE_DSTREAM == tVMPMember.GetMemberType())
		{
			continue;
		}

		if (tVMPMember.IsLocal())
		{
			byTempMcuId = tVMPMember.GetMtId();
		}
		else
		{
			byTempMcuId = GetFstMcuIdFromMcuIdx( tVMPMember.GetMcuId() );
		}

		for( byLoop1 = byLoop + 1; byLoop1 < tVmpParam.GetMaxMemberNum(); byLoop1++ )
		{
			if (NULL == tVmpParam.GetVmpMember( byLoop1 ))
			{
				tVMPMember1.SetNull();
			} 
			else
			{
				tVMPMember1 = *tVmpParam.GetVmpMember( byLoop1 );
			}

			if( tVMPMember1.IsNull() )
			{
				continue;
			}			
			
			//����˫������ͨ��
			if (VMP_MEMBERTYPE_DSTREAM == tVMPMember1.GetMemberType())
			{
				continue;
			}

			if( tVMPMember1 == tVMPMember ) 			
			{
				if( bIsAllowVmpMemRepeated )
				{
					continue;
				}
				wErrorCode = ERR_MCU_REPEATEDVMPMEMBER;
				return FALSE;
			}
			
			if( tVMPMember1.IsLocal() )//&& tVMPMember.GetMtType() == MT_TYPE_SMCU)
			{
				byTempMcuId1 = tVMPMember1.GetMtId();
			}
			else
			{
				byTempMcuId1 = GetFstMcuIdFromMcuIdx(tVMPMember1.GetMcuId());
			}
			

			if( byTempMcuId1 == byTempMcuId )
			{
				if( m_ptMtTable->GetMtType(byTempMcuId) == MT_TYPE_SMCU &&
					( IsLocalAndSMcuSupMultSpy(GetMcuIdxFromMcuId(&byTempMcuId) ) ||
					bIsAllowVmpMemRepeated )
					)
				{
					//zhouyiliang 20101019 ���ش������ظ������ͨ��������ͬ��mcu�µ��ն����ó����һ������ϳ���ĸ�mcu�µ��ն�
					if (!IsLocalAndSMcuSupMultSpy(GetMcuIdxFromMcuId(&byTempMcuId)) && (!tVMPMember1.IsLocal() || IsMcu(tVMPMember1))) 
					{
						//zhouyiliang 20101206 ���ϴλ���ϳɲ����Ƚϣ������ʱ�����������ã����������λ��
						u8 byLastMemberPos = MAXNUM_VMP_MEMBER;//tVmpParam.GetSMcuLastMtMemberPos( tVMPMember1.GetMcuId() );
						for (u8 byIndex = 0; byIndex < tVmpParam.GetMaxMemberNum()/*GetVMPMemberNum()*/ ; byIndex++ )
						{
							if (NULL == tVmpParam.GetVmpMember( byIndex ))
							{
								tTempMem.SetNull();
							} 
							else
							{
								tTempMem = *tVmpParam.GetVmpMember( byIndex );
							}
							//����˫������ͨ��
							if (VMP_MEMBERTYPE_DSTREAM == tTempMem.GetMemberType())
							{
								continue;
							}
							if ( !tConfVmpParam.IsMtInMember(tTempMem) /*&& tTempMem.GetMcuId() == tVMPMember.GetMcuId()*/
									&& GetLocalMtFromOtherMcuMt(tTempMem) == GetLocalMtFromOtherMcuMt(tVMPMember)
									&& tConfVmpParam.GetVMPStyle() != VMP_STYLE_NONE && tConfVmpParam.GetVMPMemberNum() != 0
								) 
							{
								//tVmpMember ����������
								byLastMemberPos = byIndex;
								break;

							}
							// ����vmpʱ,LastVmpParam��Ա��Ϊ0,��tVmpParam�������һ��ͨ������
							else if (tConfVmpParam.GetVMPMemberNum() == 0 &&
								GetLocalMtFromOtherMcuMt(tTempMem) == GetLocalMtFromOtherMcuMt(tVMPMember))
							{
								byLastMemberPos = byIndex;//�����һ��,����break
							}
						}
						
						if (  byLastMemberPos != MAXNUM_VMP_MEMBER )
						{
							//ֻ�����ն�,������ͨ���������Ա״̬
							//TVMPMember vmpMemLast = *tVmpParam.GetVmpMember( byLastMemberPos );
							pvmpMemLast = tVmpParam.GetVmpMember( byLoop );
							if (NULL != pvmpMemLast && NULL != tVmpParam.GetVmpMember( byLastMemberPos ))
							{
								pvmpMemLast->SetMemberTMt(*tVmpParam.GetVmpMember( byLastMemberPos ));
								tVmpParam.SetVmpMember(byLoop, *pvmpMemLast );
								if (byLastMemberPos !=  byLoop1 && NULL != tVmpParam.GetVmpMember( byLoop1 )) 
								{
									pvmpMemLast = tVmpParam.GetVmpMember( byLoop1 );
									pvmpMemLast->SetMemberTMt(*tVmpParam.GetVmpMember( byLastMemberPos ));
									tVmpParam.SetVmpMember(byLoop1, *pvmpMemLast );	
								}
							}
							
						}
					}
					continue;
				}
			
				wErrorCode = ERR_MCUNOMULTISPYONLYUSEONEVMPCHANNEL;
				return wErrorCode;
			}
		}
	}

	return wErrorCode;
}

/*==============================================================================
������    :  CheckChangeVmpReq
����      :  ����ϳɵ���ʱ�ĸ���Check
�㷨ʵ��  :  
����˵��  :  u8 &byVmpId in/out
			 TVMPParam_25Mem &tVmpParam in/out
			 BOOL32 bIsVmpModule,
			 CServMsg &cServMsg
����ֵ˵��:  error code
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130527					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::CheckChangeVmpReq(u8 &byVmpId, TVMPParam_25Mem &tVmpParam, u16 &wError)
{
	wError = 0;
	if (!IsValidVmpId(byVmpId))
	{
		// ��ϯ�ն˲���ָ��vmpid����ʹ�õ�ǰ���е�vmpֱ��������
		byVmpId = GetTheOnlyVmpIdFromVmpList();
		if (!IsValidVmpId(byVmpId))
		{
			return FALSE;
		}
	} 

	// ��vmp�Ƿ�֧�ֶ�Ӧ���
	if (!IsVMPSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, wError))
	{
		return FALSE;
	}

	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );

	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	//��鵱ǰ����Ļ���ϳ�״̬ 
	if( tConfVmpParam.GetVMPMode() == CONF_VMPMODE_NONE )
	{
		wError = ERR_MCU_VMPNOTSTART;
		return FALSE;
	}
	
	if ( !CheckVmpParam(byVmpId, tVmpParam, wError ) )
	{
		return FALSE;
	}

	if( tVmpParam.IsVMPAuto() )
	{
		//�Զ�����ϳ���������
		u8 byVmpDynStle = GetVmpDynStyle(byVmpId,  m_tConfAllMtInfo.GetLocalJoinedMtNum() );
		if ((tVmpParam.GetVMPStyle() == VMP_STYLE_DYNAMIC) &&
			VMP_STYLE_NONE == byVmpDynStle)
		{
			wError = ERR_INVALID_VMPSTYLE;
			return FALSE;
		}

		tVmpParam.SetVMPStyle( byVmpDynStle );
	}
	
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	
	// ����ϳ�У��,������vmp����
	if( !tVmpParam.IsVMPAuto())
	{
		if (!CheckMpuMember(byVmpId, tVmpParam, wError, TRUE))
		{
			return FALSE;
		}
	}

	// xsl [7/20/2006]���Ƿ�ɢ������Ҫ���ش�ͨ����
	if (m_tConf.GetConfAttrb().IsSatDCastMode())
	{
		/*u8     byNewChnMtid = 0;
		u8     byChnId      = 0xFF;
		TVMPMember *ptVmpMember = NULL;
		TVMPMember *ptConfVmpMember = NULL;
		TPollInfo *ptPollInfo  = NULL;
		for (u8 byLoop1 = 0; byLoop1 < tVmpParam.GetMaxMemberNum(); ++byLoop1 )
		{
			ptVmpMember = tVmpParam.GetVmpMember(byLoop1);
			if (ptVmpMember != NULL)
			{
				if (!ptVmpMember->IsNull())
				{
					ptConfVmpMember = tConfVmpParam.GetVmpMember(byLoop1);
					if (ptConfVmpMember != NULL && !(*tConfVmpParam.GetVmpMember(byLoop1) == *ptVmpMember))
					{
						byNewChnMtid = GetLocalMtFromOtherMcuMt(*(TMt *)ptVmpMember).GetMtId();
						byChnId      = byLoop1;
						break;
					}
				}
				else
				{
					if (ptVmpMember->GetMemberType() != tConfVmpParam.GetVmpMember(byLoop1)->GetMemberType())
					{
						switch (ptVmpMember->GetMemberType())
						{
						case VMP_MEMBERTYPE_SPEAKER:
							if (!GetLocalSpeaker().IsNull())
							{
								byNewChnMtid = GetLocalSpeaker().GetMtId();
								byChnId      = byLoop1;
							}
							break;
						case VMP_MEMBERTYPE_CHAIRMAN:
							if (!m_tConf.GetChairman().IsNull())
							{
								byNewChnMtid = m_tConf.GetChairman().GetMtId();
								byChnId      = byLoop1;
							}
							break;
						case VMP_MEMBERTYPE_POLL:
							ptPollInfo = m_tConf.m_tStatus.GetPollInfo();
							if (ptPollInfo != NULL && !ptPollInfo->GetMtPollParam().GetTMt().IsNull())
							{
								byNewChnMtid = GetLocalMtFromOtherMcuMt(ptPollInfo->GetMtPollParam().GetTMt()).GetMtId();
								byChnId      = byLoop1;
							}
							break;
						case VMP_MEMBERTYPE_DSTREAM:
							if (!m_tDoubleStreamSrc.IsNull())
							{
								byNewChnMtid = m_tDoubleStreamSrc.GetMtId();
								byChnId      = byLoop1;
							}
							break;
						default:
							break;
						}
					}
				}
			}
		}*/
		if (/*byNewChnMtid != 0 &&*/ !IsSatMtCanContinue(0,emVmpChnChange,&tVmpEqp,0xFF, 0xFF, &tVmpParam))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[VmpCommonReq-change] over max upload mt num. nack!\n");
			wError = ERR_MCU_DCAST_OVERCHNNLNUM;
			return FALSE;
		}
	}
	
	return TRUE;
}

/*==============================================================================
������    :  StartVmpSwitchGrp2AllMt
����      :  ��vmpͨ�������ն˵Ľ���,���ն˷��齨����
�㷨ʵ��  :  
����˵��  :  TEqp &tVmpEqp in
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::StartVmpSwitchGrp2AllMt(const TEqp &tVmpEqp)
{
	if (tVmpEqp.IsNull() || !IsValidVmpId(tVmpEqp.GetEqpId()))
	{
		return;
	}
	
	TMt tLocalVidBrdSrc =  GetLocalVidBrdSrc();
	u8 byVmpSubType = GetVmpSubType(tVmpEqp.GetEqpId());
	TSwitchGrp atSwitchGrp[MAXNUM_MPU2_OUTCHNNL];	//��������Ϣ���鳤��9,��������������
	u8 abyOutChnl[MAXNUM_MPU2_OUTCHNNL];			//������ͨ������:����9,���ڼ�¼��ͨ���ն�Idx
	u8 abyTmpMtIdx[MAXNUM_CONF_MT+1];				//�ն�Idx�������鳤��193
	TMt atDstMt[MAXNUM_CONF_MT+1];					//�����ն˻�������:����193,��������������
	u8 bySwitchGrpNum = 0;	//��������Ϣ���鳤��
	u8 bySrcChnnl = 0;
	TMt tMt;
	TMtStatus tMtStatus;
	TMt tSelectedMt;
	
	if( VMP != byVmpSubType )
	{
		TVideoStreamCap tStrCap = m_tConf.GetMainSimCapSet().GetVideoCap();
		u8 byConfMVSrcChnnl = GetVmpOutChnnlByRes(tVmpEqp.GetEqpId(),
												   tStrCap.GetResolution(), 
												   tStrCap.GetMediaType(),
												   tStrCap.GetUserDefFrameRate(),
												   tStrCap.GetH264ProfileAttrb());
		// [1/19/2011 xliang] �鲥�����鲥����
		if (m_tConf.GetConfAttrb().IsMulticastMode())
		{	
			// [1/30/2013 liaokang] �����ж�
			if( 0xFF != byConfMVSrcChnnl )
			{
				g_cMpManager.StartMulticast(tLocalVidBrdSrc, byConfMVSrcChnnl, MODE_VIDEO);
				m_ptMtTable->SetMtMulticasting(tLocalVidBrdSrc.GetMtId());
			}
		}
		
		//  [3/2/2012 pengguofeng]������ǻ��齻��
		if ( m_tConf.GetConfAttrb().IsSatDCastMode() )
		{		
			// [1/30/2013 liaokang] �����ж�
			if( 0xFF != byConfMVSrcChnnl )
			{
				g_cMpManager.StartDistrConfCast(tLocalVidBrdSrc, MODE_VIDEO, byConfMVSrcChnnl);
			}		
		}

		//---------------Ⱥ�齻��--------------
		//����Ⱥ�����
		memset(abyOutChnl, 0, sizeof(abyOutChnl));
		memset(abyTmpMtIdx, 0, sizeof(abyTmpMtIdx));
		
		for(u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++)
		{
			if( !m_tConfAllMtInfo.MtJoinedConf( byMtIdx ) )
			{
				continue;
			}
			
			bySrcChnnl = GetVmpOutChnnlByDstMtId( byMtIdx, tVmpEqp.GetEqpId());

			if( bySrcChnnl != 0xFF )
			{
				// vrs��¼������
				if (m_ptMtTable->GetMtType(byMtIdx) == MT_TYPE_VRSREC)
				{
					continue;
				}

				if( IsMultiCastMt( byMtIdx ) && 0xFF != byConfMVSrcChnnl && bySrcChnnl != byConfMVSrcChnnl )
				{		
					NotifyMtReceive( m_ptMtTable->GetMt(byMtIdx), byMtIdx );
					continue;							
				}

				m_ptMtTable->GetMtStatus(byMtIdx, &tMtStatus);
				tSelectedMt = tMtStatus.GetSelectMt( MODE_VIDEO ); 
				// xliang [4/24/2009] �����˱������˻���ѡ����VMP����������������
				if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
					&& (m_tRollCaller.GetMtId() == byMtIdx 
					|| ( m_tRollCallee.IsLocal() && m_tRollCallee.GetMtId() == byMtIdx))
					)
				{
					//zhouyiliang 20101231 ���������ѡ���Ĳ��Ǳ������ˣ�Ҳ���俴����ϳɣ���������һ��
					//zjj20100304 ���ﲻ���ж�ѡ��Դ������ǵ�һ��������п���ѡ��Դ������
					if (!tSelectedMt.IsNull() && (m_tRollCaller.GetMtId() == byMtIdx && tSelectedMt == m_tRollCallee || 
						m_tRollCallee.GetMtId() == byMtIdx && tSelectedMt == m_tRollCaller)
						) 
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP,"[StartMpuVmpBrd] byMtIdx.%d is RollCaller or RollCallee.so not recv vmp.\n",byMtIdx );
						continue;
					}
				}			
			
				// [1/20/2010 xliang] �ϴ���ѯ����
				tMt = m_ptMtTable->GetMt(byMtIdx);
				if( MT_TYPE_MMCU == m_ptMtTable->GetMtType(byMtIdx) )
				{
					TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
					if ( CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode() 
						&& ( MODE_VIDEO_SPY == tPollInfo.GetMediaMode() || MODE_BOTH_SPY == tPollInfo.GetMediaMode()) 
						)
					{
						continue; 
					}
				}			
				
				// [8/29/2011 liuxu] vcs��ϯ��ѯ����
				if ( m_tConf.GetChairman() == tMt )
				{
					if ( m_tConf.GetConfSource() == VCS_CONF &&
						(m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP || VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode() )
						)
					{
						continue;
					}
					if( !tSelectedMt.IsNull() )
					{
						continue;
					}					
				}
				
				
				abyTmpMtIdx[byMtIdx] = abyOutChnl[bySrcChnnl];	//����TmpMtIdx
				abyOutChnl[bySrcChnnl] = byMtIdx;	//����abyOutChnl
			}
		}
		//�����ն˻��������뽻������Ϣ����
		bySwitchGrpNum = 0;	//��������Ϣ���鳤��
		u8 byDstMtIdx = 0;		//�����ն˻����������λ��
		for (u8 byOutputChl = 0; byOutputChl < MAXNUM_MPU2_OUTCHNNL; byOutputChl++)
		{
			if (0 != abyOutChnl[byOutputChl])//��ͨ�����ն�
			{
				u8 byMtNum = 0;
				atSwitchGrp[bySwitchGrpNum].SetSrcChnl( byOutputChl );
				atSwitchGrp[bySwitchGrpNum].SetDstMt( &atDstMt[byDstMtIdx] );

				//�����ն˻�������
				u8 byInsMtIdx = abyOutChnl[byOutputChl];	//Ҫ������ն�Idx
				do 
				{
					atDstMt[byDstMtIdx] = m_ptMtTable->GetMt(byInsMtIdx);
					byInsMtIdx = abyTmpMtIdx[byInsMtIdx];	//�����һ��Ҫ������ն�Idx
					byMtNum++;	//��¼��ͨ����Ӧ�ն˸���
					byDstMtIdx++; //�����ն������±����
				} while (0 != byInsMtIdx);	//Idx��Ϊ0ʱ��ʾ�����¸��ն�
				
				atSwitchGrp[bySwitchGrpNum].SetDstMtNum( byMtNum );
				bySwitchGrpNum++;
			}
		}
	}
	else//vpu����
	{
		g_cMpManager.StartSwitchToBrd(tLocalVidBrdSrc, 0, TRUE);

		if (m_tConf.GetConfAttrb().IsMulticastMode() )
		{
			if ( !m_tConf.GetConfAttrb().IsMulcastLowStream())
			{
			
				g_cMpManager.StartMulticast( tLocalVidBrdSrc, 0, MODE_VIDEO);
			}
			else
			{
				g_cMpManager.StartMulticast( tLocalVidBrdSrc, 1, MODE_VIDEO);
			}
			m_ptMtTable->SetMtMulticasting(tLocalVidBrdSrc.GetEqpId());
		}
		if ( m_tConf.GetConfAttrb().IsSatDCastMode() )
		{		
			g_cMpManager.StartDistrConfCast(tLocalVidBrdSrc, MODE_VIDEO, 0);
		}
		TMt atMtList[MAXNUM_CONF_MT+1];
		u8 bySecNum = 0,byNum = 0;
		TSimCapSet tSimCapSet;
		for(u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++)
		{
			// vrs��¼������
			if (m_ptMtTable->GetMtType(byMtIdx) == MT_TYPE_VRSREC)
			{
				continue;
			}

			m_ptMtTable->GetMtStatus(byMtIdx, &tMtStatus);
			tSelectedMt = tMtStatus.GetSelectMt( MODE_VIDEO ); 
			// xliang [4/24/2009] �����˱������˻���ѡ����VMP����������������
			if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
				&& (m_tRollCaller.GetMtId() == byMtIdx 
				|| ( m_tRollCallee.IsLocal() && m_tRollCallee.GetMtId() == byMtIdx))
				)
			{
				//zhouyiliang 20101231 ���������ѡ���Ĳ��Ǳ������ˣ�Ҳ���俴����ϳɣ���������һ��
				//TMtStatus tStat;
				//zjj20100304 ���ﲻ���ж�ѡ��Դ������ǵ�һ��������п���ѡ��Դ������
				if (!tSelectedMt.IsNull() && (m_tRollCaller.GetMtId() == byMtIdx && tSelectedMt == m_tRollCallee || 
					m_tRollCallee.GetMtId() == byMtIdx && tSelectedMt == m_tRollCaller)
					) 
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP,"[StartMpuVmpBrd] byMtIdx.%d is RollCaller or RollCallee.so not recv vmp.\n",byMtIdx );
					continue;
				}
			}
		
			// [1/20/2010 xliang] �ϴ���ѯ����
			tMt = m_ptMtTable->GetMt(byMtIdx);
			if( MT_TYPE_MMCU == m_ptMtTable->GetMtType(byMtIdx) )
			{
				TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
				if ( CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode() 
					&& ( MODE_VIDEO_SPY == tPollInfo.GetMediaMode() || MODE_BOTH_SPY == tPollInfo.GetMediaMode()) 
					)
				{
					continue; 
				}
			}			
			
			// [8/29/2011 liuxu] vcs��ϯ��ѯ����
			if ( m_tConf.GetChairman() == tMt )
			{
				if ( m_tConf.GetConfSource() == VCS_CONF &&
					(m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP || VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode() )
					)
				{
					continue;
				}
				if( !tSelectedMt.IsNull() )
				{
					continue;
				}					
			}
			
			if ( !CanMtRecvVideo( tMt, tLocalVidBrdSrc ) )
			{
				tSimCapSet = m_ptMtTable->GetDstSCS( byMtIdx );
				if( MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() 
					&& (tSimCapSet.GetVideoMediaType() == m_tConf.GetSecVideoMediaType() )
					)
				{
					atDstMt[bySecNum++] = tMt;		
				}
			}
			else
			{
				atMtList[byNum++] = tMt;
			}
		}
		
		if ( byNum > 0)
		{		
		    StartSwitchFromBrd(tLocalVidBrdSrc, 0, byNum, atMtList);
		}
		atSwitchGrp[0].SetSrcChnl(1);
		atSwitchGrp[0].SetDstMtNum(bySecNum);
		atSwitchGrp[0].SetDstMt(atDstMt);
		bySwitchGrpNum = 1;
	}
	
	// ������
	StartSwitchToAll( tLocalVidBrdSrc, bySwitchGrpNum, atSwitchGrp, MODE_VIDEO, SWITCH_MODE_SELECT );
	
	return;
}

/*==============================================================================
������    :  SendPreSetInReqForVmp
����      :  ����ϳɼ�����ش�����
�㷨ʵ��  :  
����˵��  :  TEqp &tVmpEqp in
			 TVMPParam_25Mem &tVmpParam in/out
			 bStart in
			 bSendPreSetin out
����ֵ˵��:  BOOL32
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130529					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::SendPreSetInReqForVmp(const TEqp &tVmpEqp, TVMPParam_25Mem *ptVMPParam, BOOL32 bStart, BOOL32 &bSendPreSetIn)
{
	u8 byVmpId = tVmpEqp.GetEqpId();
	if (!IsValidVmpId(byVmpId) || NULL == ptVMPParam)
	{
		return FALSE;
	}

	//���岿��
	bSendPreSetIn = FALSE;
	TVMPMember* ptVMPMember = NULL;
	TVMPMember* ptConfVmpMember = NULL;
	CVmpChgFormatResult cVmpChgFormatResult;
	u8 byNewFormat = 0;
	BOOL32 bMemChange = FALSE;
	TPreSetInReq tPreSetInReq;
	TSimCapSet tSimCapSet;
	u8 byVmpSpeakerNum = 0;
	u8 byVmpPollNum = 0;
	u8 byTWSpeakerVNum = 0;
	u8 byTWSpeakerANum = 0;
	u8 byTWPollVNum = 0;
	u8 byTWPollANum = 0;
	u8 byHduSpeakerVNum = 0;
	u8 byHduSpeakerANum = 0;
	u8 byHduPollVNum = 0;
	u8 byHduPollANum = 0;
	u8 byManuID = MT_MANU_KDC;
	TMt tNullMt;
	tNullMt.SetNull();
	TConfMcInfo *ptMcInfo = NULL;
	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	for(u8 byLoop = 0; byLoop < ptVMPParam->GetMaxMemberNum() ; byLoop++ )
	{
		ptVMPMember = ptVMPParam->GetVmpMember( byLoop );
		ptConfVmpMember = tConfVmpParam.GetVmpMember(byLoop);
		
		if (ptVMPMember == NULL
			|| ptVMPMember->IsNull())
		{
			continue;
		}
		
		if (ptVMPMember->IsLocal())
		{
			continue;
		}

		if (ptConfVmpMember == NULL)
		{
			continue;
		}

		// �绰�ն˲���PresetIn����,�¼��绰�ն˲��������ش�����
		if (IsPhoneMt(*ptVMPMember))
		{
			continue;
		}

		bMemChange = TRUE;
		//tOldVmpMeme = *m_tLastVmpParam.GetVmpMember( byLoop ); ������ش�������LastVmpParam�޹�
		//��Ծɳ�Ա��˫������,�³�Ա�ǻ��ָ��,������Աһ��ʱ,����change
		//�ɷ�˫��������Ϊ˫������ʱ,bNeedPreSetInΪFALSE,��������߼�
		if( ((TMt)(*ptVMPMember) == (TMt)(*ptConfVmpMember)) &&
			VMP_MEMBERTYPE_DSTREAM != ptConfVmpMember->GetMemberType()
			)
		{
			bMemChange = FALSE;
		}

		//���VCS����,���ܳ��ֶ���¼�ͬʱ��λ����,��λ����²��ٷ�presetin����,��changevmpswitch��Ҳ���ٶ���λ�ն���freespy����
		BOOL32 bInOtherchan = FALSE;
		if ( VCS_CONF == m_tConf.GetConfSource() && tConfVmpParam.IsMtInMember(*ptVMPMember)) 
		{
			bInOtherchan = TRUE;
		}

		// [3/20/2012 yhz] ���ڲ����Ƿ����˸���(�ָ�)Ųλ,ֻҪ�¾ɳ�Ա����ͬ,�Ͷ��³�Ա��PresetIn����
		if (bMemChange && !bInOtherchan)
		{
			// ˫������ͨ������������ش�����
			if ( IsLocalAndSMcuSupMultSpy(ptVMPMember->GetMcuId()) && 
				 VMP_MEMBERTYPE_DSTREAM != ptVMPMember->GetMemberType())
			{
				TMt tMtVmpMember = (TMt)(*ptVMPMember);
				if (!GetMtFormat(byVmpId, tMtVmpMember, ptVMPParam, byNewFormat))
				{
					//���ڶ�ش��¼��ն�,��GetMtFormatʧ��,����PresetinReq,Ҳ��Ӧ�ñ���ͨ����Ա,
					//��Ȼ�ᵼ��δ��Presetin��ȴ��ͨ���ﻹ���˽���,���ͷŶ�ش�ʱ,�ֶ��ͷ�
					//���Ǹ���ͨ��,ҲӦ�������ͨ������,��Ϊ�ı䷢����ʱ,���ڸı䷢���˴������ȸ��ݸ���ͨ�������ͷŶ�ش���Դ��
					//����Ļ�,������еĸ���ͨ�����ն��еĸ���ͨ�����ն�
					//����:vmp�ж�·�����˸���,��1�¼��ǿƴ��ն���������,ǰ���䲻��ʱ,GetMtFormat�᷵��FALSE
					if (ptVMPMember->GetMemberType() == VMP_MEMBERTYPE_MCSSPEC)
					{
						ptVMPParam->ClearVmpMember(byLoop);
					}
					else
					{
						ptVMPMember->SetMt(tNullMt);
					}
					continue;
				}
				
				bSendPreSetIn = TRUE;

				// [8/2/2010 xliang] 
				u8 byKeepRes = ( cVmpChgFormatResult.IsKeepResInAdpChnnl() || cVmpChgFormatResult.IsSendMsg2Mt() )? 1: 0;
				
				//��Pre Setin (�����˼������ֱ���)
				memset(&tPreSetInReq, 0, sizeof(tPreSetInReq));
				TMt tSpyMt = (TMt)(*ptVMPMember);
				tPreSetInReq.m_tSpyMtInfo.SetSpyMt( tSpyMt );
				tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_tVmp = tVmpEqp;
				tPreSetInReq.m_bySpyMode = MODE_VIDEO;
				tPreSetInReq.SetEvId(MCS_MCU_STARTVMP_REQ);
				
//					tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byRes = byNewFormat;
				tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byPos = byLoop;		
//					tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byKeepOrgRes = byKeepRes;
				tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byMemberType = ptVMPMember->GetMemberType();
				tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byMemStatus = ptVMPMember->GetMemStatus();

				// [pengjie 2010/9/13] ��Ŀ�Ķ�����
				tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(ptVMPMember->GetMcuId()) );	
				if(tSimCapSet.IsNull())
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[AdjustVmpParam] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
						ptVMPMember->GetMcuId(), ptVMPMember->GetMtId() );
					return FALSE;
				}
				// ����������ϳ�Ҫ��ķֱ���
				tSimCapSet.SetVideoResolution( byNewFormat );
				//zjl20101116 �����ǰ�ն��ѻش���������Ҫ���ѻش�Ŀ��������ȡС
				if (!GetMinSpyDstCapSet((TMt)(*ptVMPMember), tSimCapSet))
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[AdjustVmpParam] GetMinSpyDstCapSet failed!\n");
					return FALSE;
				}
				
				tPreSetInReq.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		        // End

				//zhouyiliang20100806 �༶��������ϳɵ����������vcs�����Զ�����ϳɵĻ������滻��mt��ǰ����ȥ
				s16 swspyIndex = -1;
				u8  byChnnIndex = 0;
				TMt tCanReplaceSpyMt ;
				if ( tConfVmpParam.IsVMPAuto())
				{
					//zhouyiliang 20100831 FindVCSCanReplaceSpyMt����ע�͵�ԭ�е�vcs�Զ�����ϳ��滻���ԣ������ϵ�˳���滻���ԣ��滻��˳����˭�Ƚ��滻˭
					//tCanReplaceSpyMt = FindVCSCanReplaceSpyMt(EQP_TYPE_VMP,tPreSetInReq.m_tSrc,swspyIndex,byChnnIndex);
//							if ( tCanReplaceSpyMt.IsNull() && 1 < ptVMPParam->GetMaxMemberNum() ) //����Ƿ��û�������
//						{
//							tCanReplaceSpyMt = FindVCSCanReplaceSpyMt(EQP_TYPE_VMP,tPreSetInReq.m_tSrc,swspyIndex,byChnnIndex);
//						}
					if (m_tConf.GetConfSource() == VCS_CONF ) 
					{
						tCanReplaceSpyMt = (TMt)(*tConfVmpParam.GetVmpMember( byLoop ));
					}
					else if ( m_tConf.GetSpeaker() == (TMt)(*ptVMPMember) ) //zhouyiliang 20101015mcs�Զ�����ϳ�ֻ�з����˲���Ҫ��releasemt��ȥ
					{
						tCanReplaceSpyMt = m_tLastSpeaker;
					}	
								
					
				}
				//zhouyiliang 20100825 mcs����Ķ��ƻ���ϳɣ�������滻����ҲҪ��releasemt����ȥ
				else /*if( !m_tConf.m_tStatus.m_tVMPParam.IsVMPAuto() )*/
				{
					tCanReplaceSpyMt = (TMt)(*tConfVmpParam.GetVmpMember( byLoop ));
				}
				
				if ( !tCanReplaceSpyMt.IsNull() ) 
				{	
					tPreSetInReq.m_tReleaseMtInfo.m_tMt = tCanReplaceSpyMt;
					tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_VIDEO;
					tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 0;//��ʼ�����ͷ��ն˸���Ϊ0

					//�����˸�������ѯ�������ʱ,��ͳ�Ƹ�����ͨ�����ͷ��ն˸���
					if ( ptVMPMember->GetMemberType() == tConfVmpParam.GetVmpMember(byLoop)->GetMemberType() &&
						 (ptVMPMember->GetMemberType() == VMP_MEMBERTYPE_SPEAKER || ptVMPMember->GetMemberType() == VMP_MEMBERTYPE_POLL)
						)
					{
						// vmp���и���ͨ��
						byVmpSpeakerNum = GetVmpChnnlNumBySpecMemberType(VMP_MEMBERTYPE_SPEAKER, &tCanReplaceSpyMt);
						byVmpPollNum = GetVmpChnnlNumBySpecMemberType(VMP_MEMBERTYPE_POLL, &tCanReplaceSpyMt);
						
						// tvWall���и���ͨ��
						GetTvWallChnnlNumBySpecMemberType(TW_MEMBERTYPE_SPEAKER, byTWSpeakerVNum, byTWSpeakerANum);
						GetTvWallChnnlNumBySpecMemberType(TW_MEMBERTYPE_POLL, byTWPollVNum, byTWPollANum);
						
						// hdu���и���ͨ��
						GetHduChnnlNumBySpecMemberType(TW_MEMBERTYPE_SPEAKER, byHduSpeakerVNum, byHduSpeakerANum);
						GetHduChnnlNumBySpecMemberType(TW_MEMBERTYPE_POLL, byHduPollVNum, byHduPollANum);
						
						if ( m_tConf.m_tStatus.GetPollMode() != CONF_POLLMODE_NONE)
						{
							// ��ѯ�ļ�����������ѯ��ý��ģʽ������
							u8 byPollMode = m_tConf.m_tStatus.GetPollInfo()->GetMediaMode();
							
							// ����Ƶ��ѯʱ����ͳ�Ʒ����˸�������ѯ����֮��
							if ( byPollMode == MODE_BOTH || byPollMode == MODE_BOTH_BOTH)
							{
								tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum
									+ byVmpSpeakerNum + byVmpPollNum + byTWSpeakerVNum + byTWPollVNum + byHduSpeakerVNum + byHduPollVNum;
							}
							// ������Ƶ��ѯʱ���Ҹ�ͨ������ѯ����ʱ��ֻͳ����ѯ��������
							else if (ptVMPMember->GetMemberType() == VMP_MEMBERTYPE_POLL)
							{
								tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum
									+ byVmpPollNum + byTWPollVNum + byHduPollVNum;
							}
							// �в�����Ƶ��ѯʱ�����ֶ��з�����ʱ����Է����˸���ͨ����ͳ�����谴�����˸�����ͳ��
							else
							{
								tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum
									+ byVmpSpeakerNum + byTWSpeakerVNum + byHduSpeakerVNum;
							}
						}
						else
						{
							if ( ptVMPMember->GetMemberType() == VMP_MEMBERTYPE_SPEAKER)
							{
								tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum
									+ byVmpSpeakerNum + byTWSpeakerVNum + byHduSpeakerVNum;
							}
						}
					}
					else //�Ƿ����˸�������ѯ���浼�µ�ͨ������,���ͷ��ն˽�Ϊ��ͨ���ɳ�Ա
					{
						tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
					}

					//vcs�Զ�����ϳɵĻ���������򵽼�ش��ڵģ������ͷŵ�����ƵĿ����Ϊ2��1
					if ( m_tConf.GetConfSource() == VCS_CONF && tConfVmpParam.IsVMPAuto() )
					{
						u8 byMtInMcNum = GetCanRlsNumInVcsMc(tCanReplaceSpyMt, MODE_VIDEO);
						tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum += byMtInMcNum;
						
						byMtInMcNum = GetCanRlsNumInVcsMc(tCanReplaceSpyMt, MODE_AUDIO);
						if (byMtInMcNum > 0)
						{
							tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum += byMtInMcNum;
							tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_BOTH;
						}

						//zhouyiliang 20101015 ����˫���棬��ϯѡ�������ն���ƵĿ�����ټ�1
						if ( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE ) 
						{
							//�ж���ϯѡ�����ն��Ƿ��ǿ��滻�ն�
							TMt tChairman = m_tConf.GetChairman();
							if ( !tChairman.IsNull() ) 
							{
								TMtStatus tStat;
								m_ptMtTable->GetMtStatus(tChairman.GetMtId(), &tStat );
								if (tStat.GetSelectMt( MODE_VIDEO ) == tCanReplaceSpyMt  ) 
								{
									tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum++;
								}
								if (tStat.GetSelectMt( MODE_AUDIO ) == tCanReplaceSpyMt  ) 
								{
									tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum++;
								}
								
							}

							
						}
					
					}
					if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() &&
								m_tRollCallee == tPreSetInReq.m_tSpyMtInfo.GetSpyMt() )
					{
						//�������˼�һ����ص�Ŀ����
						tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum++;
					}
				}
						
				BOOL32 bSendPreSetinOk = OnMMcuPreSetIn( tPreSetInReq );

				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[AdjustVmpParam] send PreSetIn to vmpmember.(%d,%d in chnl.%d) \n",
						ptVMPMember->GetMcuId(), ptVMPMember->GetMtId(), byLoop);

				if( ( !bSendPreSetinOk )
					&& bStart && tPeriEqpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::RESERVE)
				{
					//����ǵ�һ�ο���������PreSetInʧ�ܵ�����£�vmp״̬��Ҫ��ת�ɳ�ʼ̬
					u8 byVmpIdx = byVmpId - VMPID_MIN;
					KillTimer(MCUVC_VMP_WAITVMPRSP_TIMER+byVmpIdx);
					tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
					tPeriEqpStatus.SetConfIdx( 0 );
					tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tConfVmpParam);
					g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
					RemoveVmpIdFromVmpList(byVmpId);//���б������
					
					//update UI
					CServMsg cMsg;
					cMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cMsg);
					
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[AdjustVmpParam] send PreSetIn to vmpmember.(%d,%d in chnl.%d) failed, so turn vmp state into idle state\n",
						ptVMPMember->GetMcuId(), ptVMPMember->GetMtId(), byLoop);
					
					return FALSE;
				}

				//ע�⣬�ڷ�PreSetInʱ��Ҫ�Ѵ��ն���Ϣ��Param��ɾ��������PreSetInAck�д���ʱ��ӽ�ȥ
				//���ڷǻ��ָ����Ա,���ʱ����type,��·��ѯ����ʱ,�Ѹ���type���,ʱ���ϻ��������,���䵱���ر�ͨ������.
				// ChangeVmpSwitch��������ͷŶ�ش���Դ
				// 20121226 ��PreSetInʱ������ͨ����Ϊ�þ�ͨ����Ϣ����Ack��ˢ��
				// �����˸���+��ѯ����ʱ����ѯ���¼���ش��ն�ʱ����һ��ACK�غ���Ϣ�����裬���1������ͨ����Ϊ�գ��������removechl������������
				ptVMPParam->SetVmpMember(byLoop, *ptConfVmpMember);
			}
		}
	}

	return TRUE;
}

/*==============================================================================
������    :  SendPreSetInReqForVmp
����      :  ����ϳɼ�����ش�����
�㷨ʵ��  :  
����˵��  :  u8 byVmpId in
			 TVMPParam_25Mem *ptVMPParam in
����ֵ˵��:  TVmpCommonAttrb
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130529					yanghuaizhi							
==============================================================================*/
TVmpCommonAttrb CMcuVcInst::GetVmpCommonAttrb(u8 byVmpId, TVMPParam_25Mem *ptVMPParam, BOOL32 bStart)
{
	TVmpCommonAttrb tVmpCommonAttrb;
	if (!IsValidVmpId(byVmpId) || NULL == ptVMPParam)
	{
		return tVmpCommonAttrb;
	}

	//vmp style
	tVmpCommonAttrb.m_byVmpStyle = ptVMPParam->GetVMPStyle();

	//number of members and all members' info
	/*
	note:	auto vmp: the value is the actual number of members 
		customer vmp: the value is related to style 
	*/
	u8 byMemberNum = BatchTransVmpMemberToTVMPMemberEx(
		ptVMPParam->GetVmpMember(0),
		ptVMPParam->GetMaxMemberNum(),
		tVmpCommonAttrb.m_atMtMember,
		MAXNUM_VMP_MEMBER);

	if (ptVMPParam->IsVMPAuto() || VCS_CONF == m_tConf.GetConfSource())
	{
		//maintain the memberNum be actual number
	}
	else
	{
		byMemberNum = ptVMPParam->GetMaxMemberNum();
	}
	tVmpCommonAttrb.m_byMemberNum = byMemberNum;
	
	//�������ͨ���Ľ���
	u8 byLoop = byMemberNum;
	if( ROLLCALL_MODE_VMP != m_tConf.m_tStatus.GetRollCallMode() )
	{
		for (; byLoop < MAXNUM_VMP_MEMBER; byLoop++)
		{
			StopSwitchToPeriEqp(byVmpId, byLoop, TRUE, MODE_VIDEO);
			ptVMPParam->ClearVmpMember(byLoop);
		}
	}

	//zjj 20091102 ���������滻���ԣ������Ȱѵڶ�·����ֹͣ����Ϊ�ڶ�·��Զ�ǵ����ն�(��Ҫ�滻���ն�)
	if( VCS_CONF == m_tConf.GetConfSource() && 
		VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode() &&
		ptVMPParam->GetVMPStyle() == VMP_STYLE_HTWO )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP,  "[ChangeVmpParam] VCS_GROUPROLLCALL_MODE StopSwitchToPeriEqp to vmp channel(1) byMember(%d)\n",byMemberNum );
		StopSwitchToPeriEqp(byVmpId, 1, FALSE, MODE_VIDEO);
	}

	// payload, EncryptKey
	TVMPMember *ptVmpMember = NULL;
	TLogicalChannel tChnnl;
	BOOL32 bRet = FALSE;
    BOOL32 bIsHasMember = FALSE;
	u8 byMtId = 0;
	u8 byManuId = 0;
	for (byLoop = 0; byLoop < ptVMPParam->GetMaxMemberNum(); byLoop++)
	{
		ptVmpMember = ptVMPParam->GetVmpMember(byLoop);
		if( ptVmpMember == NULL || ptVmpMember->IsNull())
		{
			continue;
		}
        else
        {
            bIsHasMember = TRUE;
        }

		
		if( !ptVmpMember->IsLocal() )
		{
			byMtId = GetFstMcuIdFromMcuIdx( ptVmpMember->GetMcuIdx() );			
			bRet = m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_VIDEO, &tChnnl, FALSE);
		}
		else
		{
			byMtId = ptVmpMember->GetMtId();
			bRet = m_ptMtTable->GetMtLogicChnnl(ptVmpMember->GetMtId(), LOGCHL_VIDEO, &tChnnl, FALSE);
		}
		
		if (bRet)
		{
			tVmpCommonAttrb.m_tVideoEncrypt[byLoop] = tChnnl.GetMediaEncrypt();
			tVmpCommonAttrb.m_tDoublePayload[byLoop].SetRealPayLoad(tChnnl.GetChannelType());
			tVmpCommonAttrb.m_tDoublePayload[byLoop].SetActivePayload(tChnnl.GetActivePayload());
			tVmpCommonAttrb.m_abyRcvH264DependInMark[byLoop] = IsRcvH264DependInMark(byMtId);
		}
        else
        {
            //zbq [12/23/2009] ģ�忪���Ļ���ϳɣ��ն˻�û�����������˴�����Կ�����ṩ
            tVmpCommonAttrb.m_tVideoEncrypt[byLoop] = m_tConf.GetMediaKey();
        }
	}

    //�����Զ�����ϳ�ʱ�޻���ϳɳ�Ա,��Կ����
    if ( !bIsHasMember && bStart && ptVMPParam->IsVMPAuto() )
    {
        tVmpCommonAttrb.m_tVideoEncrypt[0] = m_tConf.GetMediaKey();  
    }

	//need prs or not
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	tVmpCommonAttrb.m_byNeedPrs = tConfAttrb.IsResendLosePack();//yhz-

	//����ϳɷ��
	u8 byVmpSubType = GetVmpSubType(byVmpId);
    u8 bySchemeId = ptVMPParam->GetVMPSchemeId();
    TVmpStyleCfgInfo tMcuVmpStyle;    
    if(0 == bySchemeId)
    {
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[ChangeVmpParam] SchemeId :%d, use default\n", bySchemeId);
        tMcuVmpStyle.ResetDefaultColor();
        
        //zbq[05/08/2009] ����VMP ����Ĭ��Ϊ��
        if (VMP != byVmpSubType)
        {
            tMcuVmpStyle.SetBackgroundColor(0);
        }
    }  
    else
    {		
        u8  byVmpStyleNum = 0;
        TVmpAttachCfg atVmpStyle[MAX_VMPSTYLE_NUM];
        if (SUCCESS_AGENT == g_cMcuAgent.ReadVmpAttachTable(&byVmpStyleNum, atVmpStyle) && 
            bySchemeId <= MAX_VMPSTYLE_NUM) // ���� [4/30/2006] ����Ӧ�ü���Ƿ�Խ�缴��
        {
            //zbq[04/02/2008] ȡ��Ӧ�����ŵķ�����������ֱ��ȡ����
            u8 byStyleNo = 0;
            for( ; byStyleNo < byVmpStyleNum && byStyleNo < MAX_VMPSTYLE_NUM; byStyleNo ++ )
            {
                if ( bySchemeId == atVmpStyle[byStyleNo].GetIndex() )
                {
                    break;
                }
            }
            if ( byVmpStyleNum == byStyleNo ||
				MAX_VMPSTYLE_NUM == byStyleNo )
            {
                tMcuVmpStyle.ResetDefaultColor();
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[ChangeVmpParam] Get vmp cfg failed! use default(NoIdx)\n");                
            }
            else
            {
                tMcuVmpStyle.SetBackgroundColor(atVmpStyle[byStyleNo].GetBGDColor());
                tMcuVmpStyle.SetFrameColor(atVmpStyle[byStyleNo].GetFrameColor());
                tMcuVmpStyle.SetChairFrameColor(atVmpStyle[byStyleNo].GetChairFrameColor());
                tMcuVmpStyle.SetSpeakerFrameColor(atVmpStyle[byStyleNo].GetSpeakerFrameColor());
                tMcuVmpStyle.SetSchemeId(atVmpStyle[byStyleNo].GetIndex());
                tMcuVmpStyle.SetFontType( atVmpStyle[byStyleNo].GetFontType() );
                tMcuVmpStyle.SetFontSize( atVmpStyle[byStyleNo].GetFontSize() );
                tMcuVmpStyle.SetTextColor( atVmpStyle[byStyleNo].GetTextColor() );
                tMcuVmpStyle.SetTopicBkColor( atVmpStyle[byStyleNo].GetTopicBGDColor() );
                tMcuVmpStyle.SetDiaphaneity( atVmpStyle[byStyleNo].GetDiaphaneity() );
                
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[ChangeVmpParam] get vmpstyle info success, SchemeId :%d\n", bySchemeId);
				
            }
        }
        else
        {
            tMcuVmpStyle.ResetDefaultColor();
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[ChangeVmpParam] Get vmp cfg failed! use default\n");
        }
    }
    ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[ChangeVmpParam] GRDColor.0x%x, AudFrmColor.0x%x, ChairFrmColor.0x%x, SpeakerFrmColor.0x%x\n",
		tMcuVmpStyle.GetBackgroundColor(),
		tMcuVmpStyle.GetFrameColor(),
		
		tMcuVmpStyle.GetChairFrameColor(),
		tMcuVmpStyle.GetSpeakerFrameColor() );
	
    tMcuVmpStyle.SetIsRimEnabled( ptVMPParam->GetIsRimEnabled() );

	memcpy(&tVmpCommonAttrb.m_tVmpStyleCfgInfo, &tMcuVmpStyle, sizeof(tMcuVmpStyle));
	
	//���û���ϳɿ���ͨ���ı��� yhz-
	TEqpExCfgInfo tEqpExCfgInfo;
	if( SUCCESS_AGENT != g_cMcuAgent.GetEqpExCfgInfo( tEqpExCfgInfo ) )
	{
		tEqpExCfgInfo.Init();
	}
	tVmpCommonAttrb.m_tVmpExCfgInfo = tEqpExCfgInfo.m_tVMPExCfgInfo;
    LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP,  "[ChangeVmpParam]Set Vmp IdleChlShowMode: %d \n", 
		tVmpCommonAttrb.m_tVmpExCfgInfo.m_byIdleChlShowMode );	

	return tVmpCommonAttrb;
}

/*==============================================================================
������    :  ProcStartVmpBatchPollReq
����      :  ����ʼ����ϳ�������ѯ����
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-9-7                   Ѧ��
==============================================================================*/
void CMcuVcInst::ProcStartVmpBatchPollReq(CServMsg &cServMsg)
{
	//��鵱ǰ����Ļ���ϳ�״̬
	if( g_cMcuVcApp.GetVMPMode(m_tVmpEqp) != CONF_VMPMODE_NONE )
	{
		cServMsg.SetErrorCode( ERR_MCU_VMPRESTART );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		
		return;
	}
	
	//FIXME: ��δ���ڼ����Ĵ���
	TVMPParam tVmpParam = *( TVMPParam* )cServMsg.GetMsgBody();
	m_tVmpBatchPollInfo = *(TVmpBatchPollInfo*)(cServMsg.GetMsgBody()+sizeof(TVMPParam));
	
	if(m_tVmpBatchPollInfo.GetPollNum() == 0 )
	{
		m_tVmpBatchPollInfo.SetLoopNoLimt(TRUE);
	}

	u8 byVmpId;
	u8 byIdleVMPNum = 0;
    u8 abyIdleVMPId[MAXNUM_PERIEQP];
    memset( abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
    g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );
	if( byIdleVMPNum == 0 )
	{
		cServMsg.SetErrorCode( ERR_MCU_NOIDLEVMP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	else
	{
		u16 wError = 0;
		if ( !IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError) )
		{
			cServMsg.SetErrorCode( wError );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
		else if(byVmpId == 0)//����û�п�����������ѯ����vmp,ֻ����vmp
		{
			//wError = ERR_MCU_NOIDLEMPU;
			cServMsg.SetErrorCode( wError );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "No Mpu for VMP Batch Poll!\n");
			return;
		}
	}


	
	//��¼��ǰ��Ҫvmp������ѯ,����λ��֮��������
	//����Ҫ��¼��m_tConf�У���Ϊ֮��m_tConf��Ϊʵ�δ���IsMCUSupportVmpStyle()
	m_tConf.m_tStatus.m_tVMPParam.SetVMPBatchPoll( 1 );//��ֵ��֮���յ�VMP->MCU notif��ᱻ������,���Բ������д���
	tVmpParam.SetVMPBatchPoll(1);
	
	if(g_bPrintEqpMsg)
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[VmpCommonReq] start vmp param:\n");
		tVmpParam.Print();
	}


	//����vmp member
	TVMPMember tVMPMember;
	TMt tMt;
	BOOL32 bPollOver	= FALSE;
	BOOL32 bFirst		= TRUE;
	u8 byMtInConfNum    = (u8)m_tConfAllMtInfo.GetAllMtNum();	//�����ն���
	u8 byCurPolledMtNum = m_tVmpPollParam.GetVmpPolledMtNum();	//��ǰ��ѯ����MT��Ŀ
	u8 byMtId			= m_tVmpPollParam.GetCurrentIdx() + 1;	//��ǰ��ѯ�����ն�id
	u8 byVmpChnnlIdx	= 0;
	for(; byMtId <= MAXNUM_CONF_MT; byMtId ++)
	{
		if(!m_tConfAllMtInfo.MtInConf( byMtId ))//������նˣ�����
		{
			continue;
		}
		if( byVmpChnnlIdx < tVmpParam.GetMaxMemberNum() )
		{
			tMt = m_ptMtTable->GetMt(byMtId);
			tVMPMember.SetMemberTMt( tMt );
			tVMPMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);
			tVmpParam.SetVmpMember( byVmpChnnlIdx, tVMPMember );
			m_tVmpPollParam.SetCurPollBlokMtId(byVmpChnnlIdx, byMtId);//��¼������Ϣ
			byVmpChnnlIdx++;
			byCurPolledMtNum++;
			//�Ѿ��ֵ����һ���ն���
			if(byCurPolledMtNum == byMtInConfNum)//��һ��poll����������Ŀ���жϡ�
			{
				bPollOver = TRUE; 
				break;
			}
		}
		else //��һ������
		{
			byVmpChnnlIdx = 0;	//member id���´�0��ʼ
			bFirst = FALSE;		//�´ξͲ��ǵ�һ����
			break;
		}
	}
	
	//��������Ϣ���浽m_tVmpPollParam: 
	m_tVmpPollParam.SetVmpPolledMtNum(byCurPolledMtNum);//�Ѿ���ѯ����MT��Ŀ
	m_tVmpPollParam.SetCurrentIdx(byMtId - 1);			//�´�Ҫ�ֵ���MT Id - 1
	m_tVmpPollParam.SetVmpChnnlIdx(byVmpChnnlIdx);		//�´���ѯ�õ�vmp channel index
	m_tVmpPollParam.SetIsBatchFirst(bFirst);			//�´��Ƿ��һ����
	m_tVmpPollParam.SetIsVmpPollOver(bPollOver);		//����vmpһ��������ѯ�Ƿ����
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "VMP Batch Poll: CurPolledMtNum is %u,MtId is %u,MemberId is %u\n",\
		byCurPolledMtNum, byMtId, byVmpChnnlIdx);
	
	
	m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	
	//���û���ϳɲ���
	//�����Ƚ�tVMPParam�ݴ��TPeriEqpStatus��,���յ�VMP notify���ٴ��m_tConf. 
	//ע��m_tConf��ʱ�Ѿ���m_tVMPParam.m_byVMPBatchPoll����ֵ(�ս�case��)
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	
	tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_START;//��ռ��,��ʱ��δ�ɹ��ٷ���
	tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);
	tPeriEqpStatus.SetConfIdx( m_byConfIdx );
	g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	//ACK
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, TIMESPACE_WAIT_VMPRSP);
	
	//��ʼ����ϳ� 
	//AdjustVmpParam(&tVmpParam, TRUE);vmp������ѯ��֧��
	
}

/*==============================================================================
������    :  ProcVmpBatchPollTimer
����      :  VMP ������ѯ��ʱ����
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2008-12-22					Ѧ��							����
==============================================================================*/
void CMcuVcInst::ProcVmpBatchPollTimer( void )
{
	//����vmp member
	CServMsg	cMsg;
	TVMPParam	tVMPParam;
	TVMPMember	tVMPMember;
	TMt			tMt;
	u32 dwPollTimes		= m_tVmpBatchPollInfo.GetPollNum();		//��ѯ����
	tVMPParam			= m_tConf.m_tStatus.m_tVMPParam;
	u16 wMtInConfNum    = m_tConfAllMtInfo.GetAllMtNum();		//�����������ն���
	u8 byMaxMtIdInConf	= m_tConfAllMtInfo.GetMaxMtIdInConf();	//�����������ն���ID���ֵ
	u8 byCurPolledMtNum = m_tVmpPollParam.GetVmpPolledMtNum();	//��ǰ��ѯ����MT��Ŀ
	u8 byMtId			= m_tVmpPollParam.GetCurrentIdx()+1;	//��ǰ��ѯ�����ն�id
	u8 byMemberId		= m_tVmpPollParam.GetVmpChnnlIdx();		//member id
	BOOL32 bPollOver	= m_tVmpPollParam.IsVmpPollOver();		//һ��poll�Ƿ����
	BOOL32 bFirst		= m_tVmpPollParam.IsBatchFirst();		//�Ƿ��ǵ�һ��
	BOOL32 bLoopNoLimit	= m_tVmpBatchPollInfo.IsLoopNoLimit();	//�Ƿ�������ѯ
	
	//��ע�����֮ǰ��һ��δ����֮���ּ��˼����ն�.�������bFirst=FALSE�� ��ʱ��t��
	
	if(bPollOver) //һ����ѯ��������ѯ����-1
	{
		if(bLoopNoLimit) 
		{
			//��һ��poll
			byMtId = 1; //Mt id��member id ����ͷ��ʼ
			byMemberId = 0;
		}
		else
		{
			dwPollTimes --;
			if(dwPollTimes == 0)
			{
				//������ѯ������ֹͣvmp
				cMsg.SetEventId(MCS_MCU_STOPVMP_REQ);
				cMsg.SetMsgBody();
				g_cMcuVcApp.SendMsgToConf(cMsg.GetConfId(),cMsg.GetEventId());
				
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcVmpBatchPollTimer]Batch Poll times is full, stop VMP!\n");
				
				return;
			}
			else
			{
				//��һ��poll
				byMtId = 1;				//Mt id��member id ����ͷ��ʼ
				byMemberId = 0;
				byCurPolledMtNum = 0;	// ��ǰ��ѯ����MT��Ŀ��0
				bPollOver = FALSE;
			}
		}
	}
	//FIXME:���Ż���Ŀǰͳһ����
	//�Ż�������ֻ�е�һ������������ǰ����ն�������ѯ�����ն���Ŀ���Ǻϣ���������ɾ�ն˵ķ�����
	//��ʱҪ�������õ�һ��Ĳ�������֮��ֻ���޸���ѯ�������ɣ������������,����ϢchangeVmpParam
	// 	if( bFirst && wMtInConfNum == byCurPolledMtNum )
	// 	{
	// 		byMtId ++;
	// 		bPollOver = FALSE;
	// 		if(byMtId > )
	// 		m_tVmpBatchPollInfo.SetPollNum(dwPollTimes);
	// 		//֮��ʱt,����t*PolledMtNum��Ҳ��T
	// 		return;
	// 	}
	//	else
	//	{
	while(byMtId <= MAXNUM_CONF_MT)
	{
		if(!m_tConfAllMtInfo.MtInConf( byMtId ))//�������նˣ�����
		{
			byMtId++;
			continue;
		}
		if(!m_tConfAllMtInfo.MtJoinedConf( byMtId ))//�����������MT
		{
			//��ʱʱ��Ҫ����----1 ms
			m_tVmpBatchPollInfo.SetTmpt(1);
		}
		if( byMemberId < tVMPParam.GetMaxMemberNum() )
		{
			tMt = m_ptMtTable->GetMt(byMtId);
			tVMPMember.SetMemberTMt( tMt );
			tVMPMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);
			tVMPParam.SetVmpMember( byMemberId, tVMPMember );
			m_tVmpPollParam.SetCurPollBlokMtId(byMemberId,byMtId); //��¼������Ϣ
			bFirst = FALSE; //���ܿ��Ӱ��滹ͣ���ڵ�һ����񣬾���FALSE
			byMtId++;
			byCurPolledMtNum++;
			
			if (++byMemberId == tVMPParam.GetMaxMemberNum()) //�����������´�member id��0��ʼ
			{
				byMemberId = 0;
			}
			//����ͨ���ж���Ŀ��ȷ���Ƿ���һ����ѯ��������ĳЩ��ɾMT�ĳ��ϻ����У�
			//����ͨ���Ƚ���һ��Ҫ��ѯ��MT Id�ͻ��������ն������Mt Id��ȷ����
			if( byMtId > byMaxMtIdInConf )//�Ѿ��ֵ����һ���ն���
			{
				bPollOver = TRUE; //һ��������ѯover
			}
		}
		else
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcVmpBatchPollTimer]invalid VMP channel!\n");
		}
		break;
	}
	//	}
	
	//��������Ϣ���浽m_tVmpPollParam: 
	m_tVmpPollParam.SetVmpPolledMtNum(byCurPolledMtNum);//��ǰ��ѯ����MT��Ŀ
	m_tVmpPollParam.SetCurrentIdx(byMtId - 1);			//�´�Ҫ�ֵ���MT Id���˴�Ҫ��1
	m_tVmpPollParam.SetVmpChnnlIdx(byMemberId);			//vmp member Id
	m_tVmpPollParam.SetIsBatchFirst(bFirst);			//��ǰ�Ƿ��ǵ�һ����
	m_tVmpPollParam.SetIsVmpPollOver(bPollOver);		//����vmpһ��������ѯ�Ƿ����
	
	m_tVmpBatchPollInfo.SetPollNum(dwPollTimes);		//��ѯ����
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcVmpBatchPollTimer]VMP Batch Poll: CurPolledMtNum is %u,MtId is %u,MemberId is %u,Left Poll times is%u\n",\
		byCurPolledMtNum, byMtId, byMemberId, dwPollTimes);
	
	cMsg.SetEventId(MCS_MCU_CHANGEVMPPARAM_REQ);
	cMsg.SetMsgBody((u8 *)&tVMPParam,sizeof(tVMPParam));
	g_cMcuVcApp.SendMsgToConf(cMsg.GetConfId(),cMsg.GetEventId(),
		cMsg.GetServMsg(),cMsg.GetServMsgLen());
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcVmpBatchPollTimer] Send msg MCS_MCU_CHANGEVMPPARAM_REQ to conf.\n");
	
	//FIXME�����vmp������ѯ�Ѿ���ѯ�����һ������ѯ�������������ٶ�ʱ��ѯ
	// 	u32 dwPollt = m_tVmpBatchPollInfo.m_dwPollt;
	// 	SetTimer(MCUVC_VMPBATCHPOLL_TIMER,dwPollt);
}

/*==============================================================================
������    :  ProcMcsMcuVmpCmd
����      :  ����mcs��mcu��vmp ����
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2008-12-24					Ѧ��							����
==============================================================================*/
void CMcuVcInst::ProcMcsMcuVmpCmd(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);	
    //TPollInfo tPollInfo;
	
    STATECHECK;
	
    switch(pcMsg->event)
    {
	case MCS_MCU_PAUSE_VMPBATCHPOLL_CMD:
		{
			//һ�������Ƿ�ֻ����һ����ѯ���ڣ����ǵĻ�FIXME
			m_tConf.m_tStatus.SetPollState(POLL_STATE_PAUSE);
			cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
			SendMsgToAllMcs(MCU_MCS_POLLSTATE_NOTIF, cServMsg);
			
			ConfStatusChange();
			
			KillTimer(MCUVC_VMPBATCHPOLL_TIMER);

			//

			break;
		}
	case MCS_MCU_RESUME_VMPBATCHPOLL_CMD:	//Resume
		{
			//�ж��Ƿ�ǰ�����MT����ɾ��
			BOOL32 bAllMtInCurBlokDeled = TRUE;  
			TVMPParam tVMPParam;
			tVMPParam		= m_tConf.m_tStatus.m_tVMPParam;
			u8 byMaxMtId	= 0;	//����ɾ����Mt��Id�����  
			u8 byMemberId	= 0;
			for ( ; byMemberId < tVMPParam.GetMaxMemberNum(); byMemberId ++)
			{
				u8 byMtId = m_tVmpPollParam.GetMtInCurPollBlok(byMemberId);
				if(m_tConfAllMtInfo.MtInConf(byMtId))
				{
					bAllMtInCurBlokDeled = FALSE;
					break;
				}
				if(byMtId > byMaxMtId)
				{
					byMaxMtId = byMtId;
				}
			}
			//��������Mtɾ�������,�޸��´�Ҫ�ֵ�MTId��memberId
			if(bAllMtInCurBlokDeled)
			{
				m_tVmpPollParam.SetCurrentIdx(byMaxMtId-1);
				m_tVmpPollParam.SetVmpChnnlIdx(0);
			}

			m_tConf.m_tStatus.SetPollState(POLL_STATE_NORMAL);
			cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
			SendMsgToAllMcs(MCU_MCS_POLLSTATE_NOTIF, cServMsg);
			
			ConfStatusChange();
			
			u32 dwTimert = m_tVmpBatchPollInfo.GetPollt();
			SetTimer(MCUVC_VMPBATCHPOLL_TIMER, dwTimert);

			break;
		}
	case MCS_MCU_STOP_VMPBATCHPOLL_CMD:
		{
			cServMsg.SetNoSrc();
			cServMsg.SetEventId(MCS_MCU_STOPVMP_REQ);
			cServMsg.SetMsgBody();
			g_cMcuVcApp.SendMsgToConf(cServMsg.GetConfId(),cServMsg.GetEventId());
			
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMcsMcuVmpCmd]VMP Batch Poll stops!\n");
			break;
		}
	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[ProcMcsMcuVmpCmd]Unexpected Message event is received!\n");
		break;
	}
}

/*==============================================================================
������    :  ClearOneVmpMember
����      :  ��VMP��ĳ��ͨ���ĳ�Ա���
�㷨ʵ��  :  ��ͨ����Ա��ʹΪ�գ����г�Ա����ҲҪ��
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-4-2	4.6				Ѧ��							create
==============================================================================*/
void CMcuVcInst::ClearOneVmpMember(u8 byVmpId, u8 byChnnl, TVMPParam_25Mem &tVmpParam, BOOL32 bRecover)
{
	// �ӱ���,byChnnlΪ������ͨ��ʱ,����
	if (byChnnl >= MAXNUM_VMP_MEMBER || !IsValidVmpId(byVmpId))
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_VMP, "[ClearOneVmpMember] byChnnl:%d not available for vmp[%d].\n", byChnnl, byVmpId);
		return;
	}
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[ClearOneVmpMember] byChnnl:%d,bReCover:%d.\n", byChnnl,bRecover);
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );

	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	TVMPParam_25Mem tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
	TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
	TVMPMember tVmpMember = *tConfVmpParam.GetVmpMember(byChnnl);
	tConfVmpParam.ClearVmpMember(byChnnl);
	tVmpParam.ClearVmpMember(byChnnl);
	TVMPMember *ptLastVmpMember = tLastVmpParam.GetVmpMember(byChnnl);
	// ˫������ͨ��ͣ˫������
	u8 byMode = MODE_VIDEO;
	if (VMP_MEMBERTYPE_DSTREAM == tVmpMember.GetMemberType())
	{
		byMode = MODE_SECVIDEO;
	}

	// ��ͨ������vmp����ʱ,��ˢ������ͨ��̨��
	s8 achAlias[VALIDLEN_ALIAS+1] = {0};
	TMt tTmpMt;
	CServMsg cServMsg;

	if(!ptLastVmpMember->IsNull())
	{
		if(bRecover)
		{
			//�ָ�����һ�ε�VMP param������������
			tVmpParam.SetVmpMember(byChnnl, *ptLastVmpMember);
			tTmpMt = *ptLastVmpMember;
		}
		else
		{
			//�彻��
			StopSwitchToPeriEqp(tVmpEqp.GetEqpId(), byChnnl, FALSE, byMode);
			tVmpChnnlInfo.ClearChnlByVmpPos(byChnnl); //���Mt��Ӧ��HDͨ��(������ڶ�Ӧͨ��)
			g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
			// �����ն��߳�vmp��,�ͷ�һ·��ش�����,ֻ��Է�˫������ͨ��
			if (!tVmpMember.IsNull() && !tVmpMember.IsLocal() && MODE_VIDEO == byMode)
			{
				FreeRecvSpy( tVmpMember, MODE_VIDEO );
			}
			tLastVmpParam.ClearVmpMember(byChnnl);
			g_cMcuVcApp.SetLastVmpParam(tVmpEqp, tLastVmpParam);

			//�������������ͨ��,MCU_VMP_ADDREMOVECHNNL_CMD����ϢĿǰ��������ĳͨ��,��add����adjustVmpParam�²�
			cServMsg.SetChnIndex(byChnnl);
			u8 byTmp = 0XFF;//���ֶα���,���mpu��������,Ϊ����,��һ���ֽڱ�ʶ�ر�ͨ��
			u8 byAdd = 0;//��ʶ�ر�ͨ��
			cServMsg.SetMsgBody( &byTmp, sizeof(byTmp) );
			cServMsg.CatMsgBody( &byAdd, sizeof(byAdd) );
			SendMsgToEqp( tVmpEqp.GetMtId(), MCU_VMP_ADDREMOVECHNNL_CMD, cServMsg);
		}
	}

	/* ˢ������ͨ��̨��,vpu��mpu���跢�ͱ�����Ϣ
	u8 byVmpSubType = GetVmpSubType(tVmpEqp.GetEqpId());
	if (byVmpSubType != VMP && byVmpSubType != MPU_SVMP)
	{
		cServMsg.Init();
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ClearOneVmpMember]vmp member.%d info: mt(%d,%d), alias is %s!\n", 
			byChnnl, tTmpMt.GetMcuId(), tTmpMt.GetMtId(), achAlias);
		if (GetMtAliasInVmp(tTmpMt, achAlias))
		{
			cServMsg.SetMsgBody( (u8*)achAlias, sizeof(achAlias) );
		}
		cServMsg.SetChnIndex(byChnnl);
		SendMsgToEqp(tVmpEqp.GetEqpId(), MCU_VMP_CHANGEMEMALIAS_CMD, cServMsg);
	}*/

	//m_tConf.m_tStatus.SetVmpParam(tVmpParam);
	g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tConfVmpParam);
	cServMsg.Init();
	
	//���߻��ˢ����
	cServMsg.SetEqpId(tVmpEqp.GetEqpId());
	cServMsg.SetMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
	SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );	

	// xliang [4/10/2009] ͬʱ����tPeriEqpStatus��mcsҪ���ݴ�ˢMTͼ��,����������TMtStatus :(
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tConfVmpParam);
	g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	/*cServMsg.SetMsgBody();
	cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
	SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );*/

	return;
}


/*==============================================================================
    ������    :  ProcMcuMcuSpeakModeNtf
    ����      :  �ϼ��������ķ����˷���ģʽ��֪ͨ
    �㷨ʵ��  :  
    ����˵��  :  
    ����ֵ˵��:  
 -------------------------------------------------------------------------------
    �޸ļ�¼  :  
    ��  ��       �汾          �޸���          �߶���          �޸ļ�¼
    2010-5-12	 4.6		   �ű���							create
==============================================================================*/
/*void CMcuVcInst::ProcMcuMcuSpeakModeNtf( const CMessage *pcMsg )
{
	STATECHECK;

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    u32 dwStatus = *(u32*)cServMsg.GetMsgBody();

	emConfSpeakMode emMode = (emConfSpeakMode)dwStatus;

	if( emSpeakNormal != emMode &&
		emAnswerInstantly != emMode )
	{
		ConfLog( FALSE,"[ProcMcuMcuSpeakModeNtf] Wrong Speak Mode.%d \n",dwStatus );
		return;
	}
	u8 byMode = CONF_SPEAKMODE_NORMAL;
	switch( dwStatus )
	{
	case emSpeakNormal:
		byMode = CONF_SPEAKMODE_NORMAL;
		break;
	case emAnswerInstantly:
		byMode = CONF_SPEAKMODE_ANSWERINSTANTLY;
		break;
	default:
		break;
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuSpeakModeNtf] Notify Speak Mode.%d\n Event(%s)",
		byMode,OspEventDesc( cServMsg.GetEventId() ) );

	if( VCS_CONF == m_tConf.GetConfSource() )
	{
		m_cVCSConfStatus.SetConfSpeakMode( byMode );
	}


	//֪ͨ�ն�ˢ��MTC-BOX
    for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
    {
        if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
        {
            continue;
        }
		if( MT_TYPE_MT == m_ptMtTable->GetMtType(byMtId) )
		{
			NotiyfMtConfSpeakMode(byMtId, byMode);
		}
    }
}*/

/*==============================================================================
    ������    :  ProcMcsMcuChangeSpeakModeReq
    ����      :  MCS�������û���ģʽ
    �㷨ʵ��  :  
    ����˵��  :  
    ����ֵ˵��:  
 -------------------------------------------------------------------------------
    �޸ļ�¼  :  
    ��  ��       �汾          �޸���          �߶���          �޸ļ�¼
    2010-5-12	 4.6		   �ű���							create
==============================================================================*/
void CMcuVcInst::ProcMcsMcuChangeSpeakModeReq(const CMessage * pcMsg)
{
    STATECHECK;

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    u8 byMode = *(u8*)cServMsg.GetMsgBody();

    u8 byOldMode = m_tConf.GetConfSpeakMode();
    switch (byMode)
    {
    case CONF_SPEAKMODE_NORMAL:
    case CONF_SPEAKMODE_ANSWERINSTANTLY:
        
    	break;
    
    default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[ProcMcsMcuChangeSpeakModeReq] Wrong Mode Value.%d!\n",byMode );		
        return;
    }

	SendReplyBack(cServMsg, pcMsg->event + 1);

	

	if( cServMsg.GetEventId() == MCS_MCU_CHGSPEAKMODE_REQ )
	{
		//ˢ�����뷢��/�����б�
		if ( byOldMode != byMode )
		{
			m_tApplySpeakQue.ClearQueue();
			NotifyMcsApplyList( TRUE );
		}
	}

	if (VCS_CONF == m_tConf.GetConfSource())
    {
        m_cVCSConfStatus.SetConfSpeakMode(byMode);
    }
    m_tConf.SetConfSpeakMode(byMode);

	if( VCS_CONF == m_tConf.GetConfSource() &&
			m_byCreateBy != CONF_CREATE_MT )
	{
		//֪ͨUI
		if( CONF_SPEAKMODE_ANSWERINSTANTLY == byMode &&
			!( m_tConf.GetSpeaker() == m_tConf.GetChairman() ) 
			)
		{
			TMt tMt;
			tMt.SetNull();
			ChgCurVCMT( tMt );
		}
		else
		{
			VCSConfStatusNotif();
		}
	}

    //֪ͨ�ն�ˢ��MTC-BOX
    for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
    {
        if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
        {
            continue;
        }
		if( MT_TYPE_MT == m_ptMtTable->GetMtType(byMtId) )
		{
			NotiyfMtConfSpeakMode(byMtId, byMode);
		}
		else if( MT_TYPE_SMCU == m_ptMtTable->GetMtType(byMtId) )
		{
			//NotiyfMtConfSpeakMode( byMtId,byMode,FALSE,MCU_MCU_SPEAKERMODE_NOTIFY );
			cServMsg.SetEventId( MCU_MCU_SPEAKERMODE_NOTIFY );
			SendMsgToMt( byMtId,MCU_MCU_SPEAKERMODE_NOTIFY,cServMsg );
		}

    }

    return;
}

/*==============================================================================
    ������    :  ProcMcsMcuGetSpeakListReq
    ����      :  
    �㷨ʵ��  :  
    ����˵��  :  
    ����ֵ˵��:  
 -------------------------------------------------------------------------------
    �޸ļ�¼  :  
    ��  ��       �汾          �޸���          �߶���          �޸ļ�¼
    2010-5-12	 4.6		   �ű���							create
==============================================================================*/
void CMcuVcInst::ProcMcsMcuGetSpeakListReq(const CMessage * pcMsg)
{
	STATECHECK;

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	u8 byLen = MAXNUM_CONF_MT;
    TMt atMt[MAXNUM_CONF_MT];
    m_tApplySpeakQue.GetQueueList(atMt, byLen);

    cServMsg.SetMsgBody((u8*)&byLen, sizeof(u8));
	if (0 != byLen)
	{
		cServMsg.CatMsgBody((u8*)atMt, sizeof(TMt) * byLen);
	}    
    SendReplyBack(cServMsg, pcMsg->event + 1);
    
    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuGetSpeakListReq] MCU_MCS_MTAPPLYSPEAKERLIST_ACK with Len.%d!\n", byLen);


	return;
}

/*====================================================================
    ������      ProcMcsMcuGetIFrameCmd
    ����        ��������MCU����ؼ�֡����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	10/11/12     4.6          Ѧ��          ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetIFrameCmd(const CMessage *pcMsg)
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt tMt = *(TMt *)cServMsg.GetMsgBody();
	u8 byMode = *(u8*) (cServMsg.GetMsgBody() + sizeof(TMt));

	if( tMt.IsNull() )
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuGetIFrameCmd]tMt(%d,%d) is invalid!\n", tMt.GetMcuId(), tMt.GetMtId());
		return;
	}

	// ֧����������Vmp�ؼ�֡
	if (tMt.GetType() == TYPE_MCUPERI && EQP_TYPE_VMP == tMt.GetEqpType())
	{
		// MCS���ѡ������ϳ�ʱ,����mcu����vmp�Ĺؼ�֡,����ݵ�ǰ��������Ӧͨ��
		TVideoStreamCap tStrCap = m_tConf.GetMainSimCapSet().GetVideoCap();
		u8 bySrcChnnl = GetVmpOutChnnlByRes(tMt.GetEqpId(),
			tStrCap.GetResolution(), 
			tStrCap.GetMediaType(),
			tStrCap.GetUserDefFrameRate(),
			tStrCap.GetH264ProfileAttrb());
		if ( 0xFF == bySrcChnnl ) //û�ҵ���Ӧ��ͨ��
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ProcMcsMcuGetIFrameCmd] Get MCS vmp outchannl failed \n");
			return;
		}
		
		// 8kh�����ⳡ��
		if (VMP_8KH == GetVmpSubType(tMt.GetEqpId()))
		{
			// ������1080p30fpsBP��������720p30fpsHP�������ʱ,Ϊ����vmp�㲥�ǹ㲥�л�ʱͨ���ı�,ȡ1ͨ��
			if (IsConfExcludeDDSUseAdapt())
			{
				if (VIDEO_FORMAT_HD1080 == tStrCap.GetResolution() && 
					tStrCap.GetUserDefFrameRate() < 50 &&
					tStrCap.GetH264ProfileAttrb() == emBpAttrb)
				{
					if (IsMSSupportCapEx(VIDEO_FORMAT_HD720))
					{
						bySrcChnnl = 1;
					}
					else if (IsMSSupportCapEx(VIDEO_FORMAT_4CIF))
					{
						bySrcChnnl = 2;
					}
					else if (IsMSSupportCapEx(VIDEO_FORMAT_CIF))
					{
						bySrcChnnl = 3;
					}
				}
				if (VIDEO_FORMAT_HD720 == tStrCap.GetResolution() && 
					tStrCap.GetUserDefFrameRate() < 50 &&
					tStrCap.GetH264ProfileAttrb() == emHpAttrb)
				{
					if (IsMSSupportCapEx(VIDEO_FORMAT_HD720, tStrCap.GetUserDefFrameRate(), emBpAttrb))
					{
						bySrcChnnl = 1;
					}
					else if (IsMSSupportCapEx(VIDEO_FORMAT_4CIF))
					{
						bySrcChnnl = 2;
					}
					else if (IsMSSupportCapEx(VIDEO_FORMAT_CIF))
					{
						bySrcChnnl = 3;
					}
				}
			}
		}
		// 8ki�����ⳡ��
		else if (VMP_8KI == GetVmpSubType(tMt.GetEqpId()))
		{
			// ������1080p30fps��720p60fps�������ʱ,Ϊ����vmp�㲥�ǹ㲥�л�ʱͨ���ı�,ȡ1ͨ��
			if (IsConfExcludeDDSUseAdapt()) //�������
			{
				if ((m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 && m_tConf.GetMainVidUsrDefFPS() <= 30) ||
					(m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD720 && m_tConf.GetMainVidUsrDefFPS() > 30))
				{
					TKDVVMPOutParam tVMPOutParam = g_cMcuVcApp.GetVMPOutParam(tMt);
					TVideoStreamCap tTmpStrCap = tVMPOutParam.GetVmpOutCapIdx(1);
					// 720/4cif/cif��δ��ѡʱ,�ǲ�����ǹ㲥��,��ȡ��0·
					if (MEDIA_TYPE_H264 == tTmpStrCap.GetMediaType())
					{
						bySrcChnnl = 1;
					}
					else
					{
						bySrcChnnl = 0;
					}
				}
			}
		}

		byMode = bySrcChnnl;
	}
	else if( byMode != MODE_VIDEO && byMode != MODE_SECVIDEO)
	{
		ConfPrint( LOG_LVL_WARNING ,MID_MCU_CONF, "[ProcMcsMcuGetIFrameCmd]mode.%d is invalid!\n", byMode);
		return;
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuGetIFrameCmd]begin to call NotifyFastUpdate to tMt(%d,%d)\n", tMt.GetMcuId(), tMt.GetMtId());
	
	NotifyFastUpdate(tMt, byMode);

}

/*====================================================================
    ������       ProcBatchMtSmsOpr
    ����        �������������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CServMsg &cServMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/01/11     4.6          Ѧ��          ����
====================================================================*/
void CMcuVcInst::ProcBatchMtSmsOpr(const CServMsg &cServMsg)
{
	u16 wMtNum = *(u16*)cServMsg.GetMsgBody(); 
	wMtNum  = ntohs(wMtNum);
	CRollMsg* ptROLLMSG = (CRollMsg*)( cServMsg.GetMsgBody() + wMtNum*sizeof(TMt) + sizeof(u16) );
	
	u16 wPos = m_cSmsControl.GetMtPos();
	TMt	*ptMt = (TMt *)( cServMsg.GetMsgBody() + sizeof(u16) + (sizeof(TMt) * wPos) );
	u8 byPackNum = 0;
	for( ;  wPos < wMtNum; wPos++ )
	{
		// ÿ����100����һ��
		if( ++byPackNum == 101 )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "SetTimer MCUVC_SMSPACK_TIMER for %d(ms)!!\n", m_cSmsControl.GetTimerSpan());
			m_cSmsControl.SetMtPos(wPos);
			SetTimer(MCUVC_SMSPACK_TIMER, m_cSmsControl.GetTimerSpan());
			return;
		}
		ProcSingleMtSmsOpr(cServMsg, ptMt, ptROLLMSG);
		ptMt++;
	}
	
	m_cSmsControl.Init();

}

/*====================================================================
    ������       ProcSmsPackTimer
    ����        �������������Ϣ��ʱ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage *pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/01/11     4.6          Ѧ��          ����
====================================================================*/
void CMcuVcInst::ProcSmsPackTimer( void )
{

	//u8 bySrcMtId  = (u8)(*((u32*)pcMsg->content));
	
	CServMsg	*pcServMsg = m_cSmsControl.GetServMsg();

	ProcBatchMtSmsOpr(*pcServMsg);

}

/*====================================================================
������      ��ProcMcsMcuTransparentMsgNotify
����        ��������桢�ն���Ϣ͸��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const CMessage * pcMsg, �������Ϣ 
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2/23/2012               ��ʤ��          ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuTransparentMsgNotify( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
    //Ŀ���ն���
    u16 wMtNum = ntohs(*( u16* )cServMsg.GetMsgBody());
    //Ŀ���ն�
    TMt *ptDstMt = ( TMt* )( cServMsg.GetMsgBody() + sizeof(u16) );
    //���桢�ն�Э����Ϣ��
    u8 *pbyMsgContent = ( u8* )( cServMsg.GetMsgBody() + sizeof(u16) + sizeof(TMt)*wMtNum );
    //���桢�ն�Э����Ϣ�峤��
    u16 wMsgLen = cServMsg.GetMsgBodyLen() - sizeof(u16) - sizeof(TMt)*wMtNum;
    OspPrintf( TRUE, FALSE, "[ProcMcsMcuTransparentMsgNotify] mcs send to %d Mts msg size is %d!\n", wMtNum, wMsgLen);
    
    CServMsg cMsg;
    cMsg.SetSrcSsnId(cServMsg.GetSrcSsnId()/*m_cVCSConfStatus.GetCurSrcSsnId()*/);
    cMsg.SetConfIdx(m_byConfIdx);
    cMsg.SetConfId(m_tConf.GetConfId());
    
    if ( wMtNum > 0 )
    {
        for ( u16 wLoop = 0; wLoop < wMtNum; wLoop++)
        {
            if ( ptDstMt == NULL )
            {
				break;
            }
            
            if ( ptDstMt->IsLocal() )
            {//����ֱ���ն�
                
                if ( !m_tConfAllMtInfo.MtJoinedConf( ptDstMt->GetMtId() ) )
                {//�ն�δ���
                    
                    ConfPrint(LOG_LVL_WARNING, MID_MCU_MCS,"[ProcMcsMcuTransparentMsgNotify]MT:%d not joined conf !\n", ptDstMt->GetMtId());
                    ptDstMt++;
                    continue;
                }
                
                cMsg.SetEventId(MCU_MT_TRANSPARENTMSG_NOTIFY);
                cMsg.SetMsgBody((u8*)pbyMsgContent, wMsgLen);
                BOOL32 bRet = SendMsgToMt(ptDstMt->GetMtId(), cMsg.GetEventId(), cMsg);
                OspPrintf( TRUE, FALSE, "[ProcMcsMcuTransparentMsgNotify]send to mt msg size is %d\n", cMsg.GetMsgBodyLen());
                if (bRet != 1)
                {
                    ConfPrint(LOG_LVL_WARNING, MID_MCU_MCS,"MCU_MT_TRANSPARENTMSG_NOTIFY send failed!\n");
                }
                
            }
            else
            {//�Ǳ����ն�
                
                TMsgHeadMsg tHeadMsg;
                TMt tSMcu;
                tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( *ptDstMt, tSMcu);
                
                cMsg.SetEventId(MCU_MCU_TRANSPARENTMSG_NOTIFY);
                cMsg.SetMsgBody((u8*)&tHeadMsg, sizeof(tHeadMsg));
                cMsg.CatMsgBody((u8*)&tSMcu, sizeof(TMt));
                cMsg.CatMsgBody((u8*)pbyMsgContent, wMsgLen);
                SendMsgToMt((u8)tSMcu.GetMcuId(), cMsg.GetEventId(), cMsg);
            }
            ptDstMt++;
        }
    }
    else
    {
        ConfPrint(LOG_LVL_WARNING, MID_MCU_MCS,"[ProcMcsMcuTransparentMsgNotify] Dst mt num.%d is illegal!\n", wMtNum);
    }
    
    return;
}
/*====================================================================
    ������      : MCSFindMtInTvWallAndChangeSwitch
    ����        ��ֹͣ��ָ��ն˵�����ǽ�صĽ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵���� 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                ��־��          ����
====================================================================*/
void CMcuVcInst::MCSFindMtInTvWallAndChangeSwitch( TMt tMt,BOOL32 bIsStopSwitch,u8 byMode )
{
	//�˺�������ֻ֧�ֲ���Ƶ����Ƶ������������֧��
	if( tMt.IsNull() || MODE_BOTH == byMode )
	{
		return;
	}
	TPeriEqpStatus tPeriStatus;
	TMt tInTwOrHduChnMt;
	BOOL32 bIsCanChangeSwitch = FALSE;
	u8 byTwOrHduChnNum = 0;
	//ѭ����������DEC5����ǽͨ��
	for (u8 byTwIdx = TVWALLID_MIN; byTwIdx <= TVWALLID_MAX; byTwIdx++)
	{
		if (!g_cMcuVcApp.IsPeriEqpValid(byTwIdx) || !g_cMcuVcApp.GetPeriEqpStatus(byTwIdx, &tPeriStatus)
			|| !tPeriStatus.m_byOnline)
		{
			BOOL32 bInvalidEqp = !g_cMcuVcApp.IsPeriEqpValid(byTwIdx);
			BOOL32 bInvalidEqpStatus = !g_cMcuVcApp.GetPeriEqpStatus(byTwIdx, &tPeriStatus);
			BOOL32 bOffline = !tPeriStatus.m_byOnline;

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[MCSFindMtInTvWallAndChangeSwitch]Tw(%d) IsValid(%d) GetStatus(%d) OnLine(%d),So Continue!\n",byTwIdx,
				bInvalidEqp,bInvalidEqpStatus,bOffline);
			continue;
		}
		byTwOrHduChnNum = tPeriStatus.m_tStatus.tTvWall.byChnnlNum;
		for (u8 byChnIdx = 0; byChnIdx < min(byTwOrHduChnNum,MAXNUM_PERIEQP_CHNNL); byChnIdx++)
		{
			bIsCanChangeSwitch = FALSE;
			tInTwOrHduChnMt = tPeriStatus.m_tStatus.tTvWall.atVideoMt[byChnIdx];
			if (tInTwOrHduChnMt.IsNull())
			{
				continue;
			}
			if (tMt == tInTwOrHduChnMt || IsMtInMcu(tMt,tInTwOrHduChnMt))
			{
				bIsCanChangeSwitch = TRUE;
			} 
			if (bIsCanChangeSwitch)
			{
				if( bIsStopSwitch )
				{
					StopSwitchToPeriEqp( byTwIdx, byChnIdx, FALSE, byMode );
				}
				else
				{
					StartSwitchToPeriEqp( tInTwOrHduChnMt, 0, byTwIdx, byChnIdx, byMode,SWITCH_MODE_SELECT,FALSE,TRUE,FALSE,TRUE,FALSE);
				}
			}
		}
	}

	//ѭ����������HDU����ǽͨ��
	for (u8 byHduIdx = HDUID_MIN; byHduIdx <= HDUID_MAX; byHduIdx++)
	{
		if (!g_cMcuVcApp.IsPeriEqpValid(byHduIdx) || !g_cMcuVcApp.GetPeriEqpStatus(byHduIdx, &tPeriStatus)
			|| !tPeriStatus.m_byOnline)
		{
			BOOL32 bInvalidEqp = !g_cMcuVcApp.IsPeriEqpValid(byHduIdx);
			BOOL32 bInvalidEqpStatus = !g_cMcuVcApp.GetPeriEqpStatus(byHduIdx, &tPeriStatus);
			BOOL32 bOffline = !tPeriStatus.m_byOnline;
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[MCSFindMtInTvWallAndChangeSwitch]Hdu(%d) IsValid(%d) GetStatus(%d) OnLine(%d),So Continue!\n",byHduIdx,
				bInvalidEqp,bInvalidEqpStatus,bOffline);
			continue;
		}
		byTwOrHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byHduIdx));
		for (u8 byChnIdx = 0; byChnIdx < min(byTwOrHduChnNum,MAXNUM_HDU_CHANNEL); byChnIdx++)
		{
			bIsCanChangeSwitch = FALSE;
			tInTwOrHduChnMt = tPeriStatus.m_tStatus.tHdu.atVideoMt[byChnIdx];
			if (tInTwOrHduChnMt.IsNull())
			{
				continue;
			}
			if (tMt == tInTwOrHduChnMt || IsMtInMcu(tMt,tInTwOrHduChnMt))
			{
				bIsCanChangeSwitch = TRUE;
			} 
			if (bIsCanChangeSwitch)
			{
				if( bIsStopSwitch )
				{
					StopSwitchToPeriEqp( byHduIdx, byChnIdx, FALSE, byMode );
				}
				else
				{
					StartSwitchToPeriEqp( tInTwOrHduChnMt, 0, byHduIdx, byChnIdx, byMode, SWITCH_MODE_SELECT,FALSE,TRUE,FALSE,TRUE,FALSE);
				}
			}
		}
	}
}
/*====================================================================
    ������      : ClearHduSchemInfo
    ����        ������HDUԤ�������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵���� 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    12/08/31                ��־��          ����
====================================================================*/
void CMcuVcInst::ClearHduSchemInfo()
{
    if( m_tConf.GetConfAttrb().IsHasTvWallModule() )
    {
		CServMsg cServMsgSend;
        TMultiTvWallModule tMultiTvWallModule;
        m_tConfEqpModule.GetMultiTvWallModule( tMultiTvWallModule );
        TTvWallModule tTvWallModule;
		THduChnlInfo  atHduChnlInfo[MAXNUM_TVWALL_CHNNL_INSMOUDLE];
		TPeriEqpStatus tHduStatus;
        for( u8 byTvLp = 0; byTvLp < tMultiTvWallModule.GetTvModuleNum(); byTvLp++ )
        {
            tMultiTvWallModule.GetTvModuleByIdx(byTvLp, tTvWallModule);
			u8 byTvWallId = tTvWallModule.GetTvEqp().GetEqpId();
			u8 byChnIdx = 0;
			u8 byHduSchemeNum = 0;
			THduStyleInfo atHduStyleInfoTable[MAX_HDUSTYLE_NUM];
			u16 wRet = g_cMcuAgent.ReadHduSchemeTable(&byHduSchemeNum, atHduStyleInfoTable);
			if (SUCCESS_AGENT != wRet)
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ClearHduSchemInfo] ReadHduSchemeTable failed!\n");
			}
			else
			{
				for (u8 byStyleIdx = 0 ; byStyleIdx < byHduSchemeNum; byStyleIdx ++)
				{
					if (atHduStyleInfoTable[byStyleIdx].GetStyleIdx() == byTvWallId)
					{
						atHduStyleInfoTable[byStyleIdx].GetHduChnlTable(atHduChnlInfo);
						for (u8 byChnl = 0; byChnl < MAXNUM_TVWALL_CHNNL_INSMOUDLE; byChnl++)
						{   
							if (!g_cMcuVcApp.IsPeriEqpValid( atHduChnlInfo[byChnl].GetEqpId() ) ||
								!g_cMcuVcApp.GetPeriEqpStatus(atHduChnlInfo[byChnl].GetEqpId(), &tHduStatus))
							{
								ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU, "[ClearHduSchemInfo]fail to get status(hduid:%d)\n", atHduChnlInfo[byChnl].GetEqpId());
								continue;
							}
							byChnIdx = atHduChnlInfo[byChnl].GetChnlIdx();
							if (tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].GetConfIdx() == m_byConfIdx)
							{
								tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].byMemberType = 0;
								tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].SetNull();
								tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].SetConfIdx(0);
								g_cMcuVcApp.SetPeriEqpStatus(atHduChnlInfo[byChnl].GetEqpId(), &tHduStatus);
								cServMsgSend.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
								SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsgSend);
							}
						}
					}
				}
			}      
        }
    }
}

/*====================================================================
    ������      : IsCircleScheduleConfNeedStart
    ����        ��������ԤԼ�����Ƿ���Ҫ������ʱ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵���� 
    ����ֵ˵��  ��TRUE����Ҫ������ʱ���ԣ�FALSE��������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/02/21                ������          ����
====================================================================*/
BOOL32 CMcuVcInst::IsCircleScheduleConfNeedStart( )
{
	if ( !m_tConf.IsCircleScheduleConf() )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[IsCircleScheduleConfNeedStart]conf:%s is not a circlescheduleconf!\n",m_tConf.GetConfName());	
		return FALSE;
	}
	time_t nTimeNow = time(NULL);
	//1.�ȱ���Чʱ�䣬������Χֱ�ӻ�false
	//1.1.durastarttime Ϊ�գ�û����starttime,����ǰʱ��Ӧ������durastarttime
	if ( !m_tConfEx.GetDurationDate().IsDuraStartTimeNull() )
	{
		TKdvTime tDuraStartTime = m_tConfEx.GetDurationDate().GetDuraStartTime();
		time_t nDuraStartTime = 0;
		tDuraStartTime.GetTime(nDuraStartTime);
		if ( nTimeNow < nDuraStartTime) //��ǰ��û����Ч��������ʱ��
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow < nDuraStartTime !\n",
			m_tConf.GetConfName());	
			return FALSE;
		}

	}
	//1.2.duraEndtime Ϊ�գ�û����endtime,����ǰʱ��Ӧ������duraEndtime
	if ( !m_tConfEx.GetDurationDate().IsDuraEndTimeNull() )
	{
		TKdvTime tDuraEndTime = m_tConfEx.GetDurationDate().GetDuraEndTime();
		time_t nDuraEndTime = 0;
		tDuraEndTime.GetTime(nDuraEndTime);
		if ( nTimeNow > nDuraEndTime ) //��ǰ����Ч�������ʱ�仹�����鲻�ܿ�������ǰԤԼ����Ӧ�ý���
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow > nDuraEndTime,endconf !\n",
				m_tConf.GetConfName());
			return FALSE;
		}
		
	}

	//2.����´ο���ʱ����ֵ��˵���Ѿ�������ʱ����ɹ�����ֱ�ӱȵ�ǰʱ����´ο���ʱ��
	TKdvTime tNextStartTime = m_tConfEx.GetNextStartTime();
	if ( !m_tConfEx.IsNextStartTimeNull() )
	{	
		time_t nNextTime = 0;
		tNextStartTime.GetTime(nNextTime);
		if ( nTimeNow < nNextTime )//��û������ʱ��
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow < nNextTime !\n",
				m_tConf.GetConfName());
			return FALSE;
		}
		else
		{
			if ( ( m_tConf.GetDuration() == 0 && nTimeNow > (nNextTime + 30*60) ) ||
				( m_tConf.GetDuration() !=0 && nTimeNow > (nNextTime + m_tConf.GetDuration()*60) )
				)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF,
					"[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow > nNextTime + duratime(or30min) !\n",m_tConf.GetConfName());
				return FALSE;
			}
			//��ǰʱ���ڽ��컹���Կ��ķ�Χ��
			return TRUE;
		}
	} 

	
	//3.��û�����ɹ�����ʱ���飬����ǰʱ���Ƿ���Ͽ�������
	//3.1.ÿ���ͣ��Ƚ�ʱ����
	TKdvTime tStartTime = m_tConf.GetKdvStartTime();
	TKdvTime tKdvTimeNow ;
	tKdvTimeNow.SetTime(&nTimeNow);
	if (m_tConf.GetScheduleConfMode() == CIRCLE_SCHEDULE_CONFMODE_DAY )
	{
		tStartTime.SetYear(tKdvTimeNow.GetYear());
		tStartTime.SetMonth(tKdvTimeNow.GetMonth());
		tStartTime.SetDay(tKdvTimeNow.GetDay());
		time_t nStartTime = 0;
		tStartTime.GetTime(nStartTime);
		if ( nTimeNow < nStartTime  )//��û��Ҫ����ʱ��
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow < nStartTime !\n",
				m_tConf.GetConfName());
			return FALSE;
		}
		else //���쿪���Ļ�����ǰ�Ƿ񳬹�����ʱ���Сʱ���߿���ʱ��+����ʱ��
		{
			if ( ( m_tConf.GetDuration() == 0 && nTimeNow > (nStartTime + 30*60) ) ||
					( m_tConf.GetDuration() !=0 && nTimeNow > (nStartTime + m_tConf.GetDuration()*60) )
				)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF,
					 "[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow > nStartTime + duratime(or30min) !\n",m_tConf.GetConfName());
				return FALSE;
			}
			//��ǰʱ���ڽ��컹���Կ��ķ�Χ��
			return TRUE;
		}
		
	}//end CIRCLE_SCHEDULE_CONFMODE_DAY
	//3.2.ÿ���ͣ��ȽϹ�ѡ���ں�ʱ����
	else if ( m_tConf.GetScheduleConfMode() == CIRCLE_SCHEDULE_CONFMODE_WEEK )
	{
		struct tm * ptTime;
		ptTime = localtime(&nTimeNow);
		//�ȱ������Ƿ�ѡ
		u8 byWeekDay = ptTime->tm_wday;
		if ( m_tConf.IsWeekDaySelected(byWeekDay) )//�Ƚ�ʱ����
		{
			tStartTime.SetYear(tKdvTimeNow.GetYear());
			tStartTime.SetMonth(tKdvTimeNow.GetMonth());
			tStartTime.SetDay(tKdvTimeNow.GetDay());
			time_t nStartTime = 0;
			tStartTime.GetTime(nStartTime);
			if ( nTimeNow < nStartTime  )//��û��Ҫ����ʱ��
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow < nStartTime !\n",
					m_tConf.GetConfName());
				return FALSE;
			}
			else
			{
				if ( ( m_tConf.GetDuration() == 0 && nTimeNow > (nStartTime + 30*60) ) ||
					( m_tConf.GetDuration() !=0 && nTimeNow > (nStartTime + m_tConf.GetDuration()*60) )
					)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF,
						"[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow > nStartTime + duratime(or30min) !\n",m_tConf.GetConfName());
					return FALSE;
				}
				//��ǰʱ���ڽ��컹���Կ��ķ�Χ��
				return TRUE;
			}
		}
		else
		{
			//ÿ����û��ѡ���������
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[IsCircleScheduleConfNeedStart]circlescheduleconf:%s not select today weekday !\n",
				m_tConf.GetConfName());
			return FALSE;
		}

	}//end CIRCLE_SCHEDULE_CONFMODE_WEEK 

	//4.���з���false����������˵��ˣ�����TRUE
	return FALSE;
	
}


/*====================================================================
    ������      : GetNextOngoingTime
    ����        ����ȡ������ԤԼ�����´ο�����ʱ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵���� tConfInfo:�����confinfo�� tValidDuraDate:������Чʱ��
					bAcceptCurOlder:�Ƿ���ܱȵ�ǰ�����ʱ�䣨���Сʱ�ڻ��ߵ�ǰ���ڿ���+����ʱ�䣩
					��������������޸Ļ����ʱ�����ã����ʱ��Ӧ�ÿ��ǣ���Ϊ�����Ļ������������϶��Ѿ������ˣ�
    ����ֵ˵��  ��TKdvTime���´ο���ʱ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/02/21                ������          ����
====================================================================*/
TKdvTime CMcuVcInst::GetNextOngoingTime( const TConfInfo& tConfInfo, const TDurationDate& tValidDuraDate,BOOL32 bAcceptCurOlder /*= FALSE*/)
{
	TKdvTime tNullTime;
	memset(&tNullTime,0,sizeof(TKdvTime));
	TKdvTime tNextTime;
	memset(&tNextTime,0,sizeof(TKdvTime));
	if ( !tConfInfo.IsCircleScheduleConf() )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[GetNextOngoingTime]conf:%s is not a circleshceduleconf !\n",tConfInfo.GetConfName());
		return tNullTime;
	}
	time_t nDuraStartTime = 0;
	time_t nDuraEndTime = 0;
	time_t nTimeNow = time(NULL);
	if ( !tValidDuraDate.IsDuraStartTimeNull() )
	{
		tValidDuraDate.GetDuraStartTime().GetTime(nDuraStartTime);
	}
	if ( !tValidDuraDate.IsDuraEndTimeNull() )
	{
		tValidDuraDate.GetDuraEndTime().GetTime(nDuraEndTime);
	}

	//���쿪����ʱ�����ʱ��
	TKdvTime tConfTodayStartTime = tConfInfo.GetKdvStartTime();
	TKdvTime tKdvTimeNow ;
	tKdvTimeNow.SetTime(&nTimeNow);
	tConfTodayStartTime.SetYear(tKdvTimeNow.GetYear());
	tConfTodayStartTime.SetMonth(tKdvTimeNow.GetMonth());
	tConfTodayStartTime.SetDay(tKdvTimeNow.GetDay());
	time_t nConfTodayStartTime = 0;
	time_t nStartToFindNexttime = 0;
	tConfTodayStartTime.GetTime(nConfTodayStartTime);
	nStartToFindNexttime = nConfTodayStartTime;

	//��ǰʱ�仹������Ч���翪��ʱ�䣬ֱ�Ӵ���Ч���翪��ʱ�俪ʼ���´ο���ʱ��
	if ( nDuraStartTime != 0 && nDuraStartTime > nTimeNow )
	{
		TKdvTime tValidEarlistStarttime = tValidDuraDate.GetDuraStartTime();
		tValidEarlistStarttime.SetHour(tConfTodayStartTime.GetHour());
		tValidEarlistStarttime.SetMinute(tConfTodayStartTime.GetMinute());
		tValidEarlistStarttime.SetSecond(tConfTodayStartTime.GetSecond());
		tValidEarlistStarttime.GetTime(nStartToFindNexttime);
	
	}

	//ÿ���ͣ���+1��
	if (tConfInfo.GetScheduleConfMode() == CIRCLE_SCHEDULE_CONFMODE_DAY)
	{
		time_t nNexttime = nStartToFindNexttime;
		if ( nStartToFindNexttime > nTimeNow )//��ʼ�ҵ�ʱ��ȵ�ǰʱ�����´ο���ʱ��Ϊ��ʼ�ҵ�ʱ��
		{	
		
		}
		else //��ʼ�ҵ�ʱ�����ڵ�ǰ��ʱ�䣬����bAcceptCurOlder��������Ŀ���ʱ���Ƿ���ϣ��������������
		{
		
			if ( bAcceptCurOlder && 
					(( tConfInfo.GetDuration() == 0 && nTimeNow < (nStartToFindNexttime + 30*60) ) ||
						( tConfInfo.GetDuration() !=0 && nTimeNow < (nStartToFindNexttime + tConfInfo.GetDuration()*60) )
					)
				)
			{
				nNexttime = nTimeNow;
			}
			else
			{
				nNexttime +=  24*3600;//+1��ʱ��
			}
			
		}
		
		//��Чʱ�䷶Χ�⣬���ؿ�
		if (  nDuraEndTime != 0 && nDuraEndTime < nNexttime )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[GetNextOngoingTime]conf:%s next starttime out of durationdate !\n",tConfInfo.GetConfName());
			return tNullTime;
		}
	
		tNextTime.SetTime(&nNexttime);
		
	}
	//ÿ���ͣ��ҵ���ѡ����һ����������
	else if ( tConfInfo.GetScheduleConfMode() == CIRCLE_SCHEDULE_CONFMODE_WEEK )
	{
		time_t nNexttime = nStartToFindNexttime;
		if ( nStartToFindNexttime > nTimeNow )//��ʼ�ҵ�ʱ��ȵ�ǰʱ�������컹�����ˣ��ӿ��Կ���ʱ��
		{	
			//�ӿ��Կ���ʱ���ҹ�ѡ
			struct tm * ptTime;
			ptTime = localtime(&nStartToFindNexttime);
			u8 byInc = 0;
			for (u8 byWeekIdx =0 ;byWeekIdx < 7;byWeekIdx ++)
			{
				u8 byNextWeekDay = (ptTime->tm_wday + byInc)%7;
				if ( tConfInfo.IsWeekDaySelected(byNextWeekDay) )//��ѡ��
				{
					break;
				}
				byInc++;
			}
			if ( byInc >= 8 )//��һ��ʱ�䶼û�ҵ���ѡ���쳣���
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[GetNextOngoingTime]conf:%s no next select weekday!\n",tConfInfo.GetConfName());
				return tNullTime;
			}
			nNexttime +=  byInc*24*3600;
			
		}
		else //��ʼ�ҵ�ʱ�����ڵ�ǰ��ʱ�䣬����bAcceptCurOlder��������Ŀ���ʱ���Ƿ���ϣ�����������һ����ѡ
		{

			struct tm * ptTime;
			ptTime = localtime(&nTimeNow);
			if ( bAcceptCurOlder && tConfInfo.IsWeekDaySelected(ptTime->tm_wday)&&
				(( tConfInfo.GetDuration() == 0 && nTimeNow < (nStartToFindNexttime + 30*60) ) ||
				( tConfInfo.GetDuration() !=0 && nTimeNow < (nStartToFindNexttime + tConfInfo.GetDuration()*60) )
				)
				)
			{
				nNexttime = nTimeNow;
				
			}
			else
			{
				//����һ����ѡ
		
				u8 byInc = 1;
				for (u8 byWeekIdx =0 ;byWeekIdx < 7;byWeekIdx ++)
				{
					u8 byNextWeekDay = (ptTime->tm_wday + byInc)%7;
					if ( tConfInfo.IsWeekDaySelected(byNextWeekDay) )//��ѡ��
					{
						break;
					}
					byInc++;
				}
				if ( byInc >= 8 )//��һ��ʱ�䶼û�ҵ���ѡ���쳣���
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[GetNextOngoingTime]conf:%s no next select weekday!\n",tConfInfo.GetConfName());
					return tNullTime;
				}
				nNexttime +=  byInc*24*3600;
			}
		}
		
	
		if ( nDuraEndTime != 0 && nDuraEndTime < nNexttime )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[GetNextOngoingTime]conf:%s next starttime out of durationdate !\n",tConfInfo.GetConfName());
			return tNullTime;
		}
		tNextTime.SetTime(&nNexttime);
	}
	return tNextTime;
}

/*====================================================================
    ������      : IsCircleScheduleConf
    ����        ������kdvstarttime�ж��Ƿ���������ԤԼ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵���� const TKdvTime& tStartTime:���鿪��ʱ��
    ����ֵ˵��  ��TRUE:������ԤԼ���飬FALSE����������ԤԼ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/02/21                ������          ����
====================================================================*/
BOOL32 CMcuVcInst::IsCircleScheduleConf( const TKdvTime& tStartTime )
{

		u16 wYear = tStartTime.GetYear();
		u16 wScheMode = (wYear & 0xE000)>>13;
		//������ԤԼ����
		if ( wScheMode < CIRCLE_SCHEDULE_CONFMODE_NUM && wScheMode > CIRCLE_SCHEDULE_CONFMODE_NONE )
		{
			return TRUE;
		}
		return FALSE;
}



/*====================================================================
    ������      : ModifyCircleScheduleConfDuraDate
    ����        ����֤durastart��ʱ����Ϊ00:00:00,duraend(��ֵ�Ļ�)��ʱ����Ϊ23:59:59
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵���� tDuarationDate
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/02/21                ������          ����
====================================================================*/
void CMcuVcInst::ModifyCircleScheduleConfDuraDate(TDurationDate& tDuarationDate )
{
	if ( !tDuarationDate.IsDuraStartTimeNull() )
	{
		TKdvTime tDuraStartTime = tDuarationDate.GetDuraStartTime();
		tDuraStartTime.SetHour(0);
		tDuraStartTime.SetMinute(0);
		tDuraStartTime.SetSecond(0);
		tDuarationDate.SetDuraStartTime(tDuraStartTime);
	}

	if (!tDuarationDate.IsDuraEndTimeNull())
	{
		TKdvTime tDuraEndTime = tDuarationDate.GetDuraEndTime();
		tDuraEndTime.SetHour(23);
		tDuraEndTime.SetMinute(59);
		tDuraEndTime.SetSecond(59);
		tDuarationDate.SetDuraEndTime(tDuraEndTime);
	}
}

/*====================================================================
    ������      : GetVmpModule
    ����        �����µĻ���ϳ�ģ���л�ȡԭ20�����ϳ�ģ��
				 ������TVmpModuleInfo === > TVmpModule��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵�� void
    ����ֵ˵��  ��TVmpModule ������֯�õ�ԭ20���vmpģ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/03/16                chendaiwei    ����
====================================================================*/
TVmpModule CMcuVcInst::GetVmpModule( void )
{
	TVmpModule tVmpModule;
	TVMPParam tParam;
	memset(&tParam,0,sizeof(tParam));

	u32 byBasicParamLen = sizeof(TVMPParam)-MAXNUM_MPUSVMP_MEMBER*sizeof(TVMPMember);
	memcpy(&tParam,&m_tConfEqpModule.m_tVmpModuleInfo,byBasicParamLen);

	memcpy(&tVmpModule.m_abyVmpMember[0],&m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[0],MAXNUM_MPUSVMP_MEMBER);
	memcpy(&tVmpModule.m_abyMemberType[0],&m_tConfEqpModule.m_tVmpModuleInfo.m_abyMemberType[0],MAXNUM_MPUSVMP_MEMBER);

	for( u8 byIdx = 0; byIdx < MAXNUM_MPUSVMP_MEMBER; byIdx++)
	{
		TVMPMember tMember;
		memset(&tMember,0,sizeof(tMember));

		tMember.SetMemberType(m_tConfEqpModule.m_tVmpModuleInfo.m_abyMemberType[byIdx]);
		TMt tMt = m_ptMtTable->GetMt(m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byIdx]);
		tMember.SetMemberTMt(tMt);

		tParam.SetVmpMember(byIdx,tMember);
	}

	tVmpModule.SetVmpParam(tParam);

	return tVmpModule; 
}

/*====================================================================
    ������      : GetVmpParam25MemFromModule
    ����        �����µĻ���ϳ�ģ���л�ȡԭ20�����ϳ�ģ��
				 ������TVmpModuleInfo === > TVmpModule��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵�� void
    ����ֵ˵��  ��TVmpModule ������֯�õ�ԭ20���vmpģ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/03/16                chendaiwei    ����
====================================================================*/
TVMPParam_25Mem CMcuVcInst::GetVmpParam25MemFromModule( void )
{
	TVMPParam_25Mem tParam;
	memset(&tParam,0,sizeof(tParam));

	u32 byBasicParamLen = sizeof(TVMPParam)-MAXNUM_MPUSVMP_MEMBER*sizeof(TVMPMember);
	memcpy(&tParam,&m_tConfEqpModule.m_tVmpModuleInfo,byBasicParamLen);

	for( u8 byIdx = 0; byIdx < MAXNUM_VMP_MEMBER; byIdx++)
	{
		TVMPMember tMember;
		memset(&tMember,0,sizeof(tMember));

		tMember.SetMemberType(m_tConfEqpModule.m_tVmpModuleInfo.m_abyMemberType[byIdx]);
		TMt tMt = m_ptMtTable->GetMt(m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byIdx]);
		tMember.SetMemberTMt(tMt);

		tParam.SetVmpMember(byIdx,tMember);
	}

	return tParam; 
}


// END OF FILE

