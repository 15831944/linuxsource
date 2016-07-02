/*****************************************************************************
   ģ����      : mtadp
   �ļ���      : mtadpd.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: MtAdp Daemonʵ��������ʵ����mcuҵ����Ϣ����ģ�������ڣ�
                 �Լ��󲿷�Э��ջ��Ϣ����ڡ�ҵ���Э��ջ��Ϣ��Daemonʵ����
				 ����һЩԤ����󣬱��ַ�����Ŀ��ʵ����

   ����        : Tan Guang
   �汾        : V1.0  Copyright(C) 2003-2004 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/11/11  1.0         Tan Guang   ����
******************************************************************************/
/*lint -save -e765*/ 
#include "osp.h"
#include "cm.h"
#include "cmsize.h"
#include "mcustruct.h"
#include "evmcumt.h"
#include "evmcu.h"
#include "evmcuvcs.h"
//#include "mcuerrcode.h"
#include "mtadp.h"
#include "mtadputils.h"
//#include "h323adapter.h"
#include "seli.h"
#include "boardagent.h"
#include "mcuver.h"

#include "readlicense.h"
#include "usbkeyconst.h"
#include "licensekeyconst.h"
#include "evmcueqp.h"


#ifdef _VXWORKS_
#include "brddrvLib.h"
#endif

// [pengjie 2010/3/9] CRI2/MPC2 ֧��
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
//        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
//        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
    #include "cri_agent.h"
#endif


#ifdef WIN32
#include "winbrdwrapper.h"
#endif


#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
#include "agentinterface.h"
extern  CAgentInterface     g_cMcuAgent;
#endif



extern u32 g_dwVidInfoTimeout;
extern u32 g_dwAudInfoTimeout;
extern u32 g_dwPartlistInfoTimeout;

//static BOOL32	   s_bDorvstat = FALSE;

static s8  g_achStackCfgFile[KDV_MAX_PATH] = {0};


/*=============================================================================
  �� �� ���� DaemonInstanceEntry
  ��    �ܣ� MCU��Э��ջ��������Ϣ����ڣ������ҵ���Ӧ��Instance��ֱ�ӵ���InstanceEntry���� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  CMessage * const pcMsg����Ϣ��ָ̬��
              CApp* pcApp����APPָ��
  �� �� ֵ��  void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang              ����
  2005/02/01	3.6			xsl					  ���ӿ��Ź�����, �������쳣
  2005/10/27	4.0			�ű���				  ����QOS����
  2005/11/29	4.0			�ű���				  �������ý��滯����
=============================================================================*/
void CMtAdpInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp )
{
	if( !pcMsg )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "NULL message received, ignore it\n!" );
		return;
	}	

	

	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	switch( pcMsg->event )
	{
	case OSP_POWERON:
		DaemonProcPowerOn();
		break;
	
	case OSP_OVERFLOW:
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "no idle instance available!\n");
		break;

	case TIMER_H323_POLL_EVENT:
		{           
			//��ֹ����MtAdpStop()ʱ�쳣
			if( g_cMtAdpApp.m_bMtAdpInited )
			{
				kdvCheckMessage();	
				SetTimer(TIMER_H323_POLL_EVENT, H323_POLL_INTERVAL);
                
                //�յ�Э��ջ��Ϣ��ȡ��ʱ����Ϣ
                g_cMtAdpApp.m_bH323PollWatchdog = FALSE;
			}
			else
			{
				KillTimer(TIMER_H323_POLL_EVENT);
				StaticLog( "[mtadpt][ERROR]g_cMtAdpApp.m_bMtAdpInited is FALSE!\n");
                
                KillTimer(TIMER_POLL_WATCHDOG);
                StaticLog( "[mtadpt][ERROR] h323 poll watchdog tiemr is killed!\n");                
			}
		}
		break;

    case TIMER_POLL_WATCHDOG:
        {
            //����������յ�Э��ջ��Ϣ��ȡ��ʱ����Ϣ
            if(!g_cMtAdpApp.m_bH323PollWatchdog)
            {
                g_cMtAdpApp.m_bH323PollWatchdog = TRUE;                
            }
            //δ�յ���ʱ����Ϣ��������ʱ��
            else
            {
                SetTimer( TIMER_H323_POLL_EVENT, H323_POLL_INTERVAL );
                StaticLog( "[mtadpt][ERROR] not receive h323 poll tiemr msg, restore it!\n" );
            }
            
            SetTimer( TIMER_POLL_WATCHDOG, H323_POLL_INTERVAL*100 );
        }
        break;

	case TIMER_REGGK_REQ:
		{
			CServMsg cRegGkMsg;
			if( !g_cMtAdpApp.m_bGotRRJOrURQFromGK )
			{
				cRegGkMsg.SetConfIdx( MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 ); // lightweight reg
				SetTimer( TIMER_REGGK_REQ, INTERVAL_REG_GK * 1000 );
			}
			else
			{
                cRegGkMsg.SetConfIdx( MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 2 ); // re-registration
			}
			
			cRegGkMsg.SetEventId( MCU_MT_REGISTERGK_REQ );
			post( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), 
				  MCU_MT_REGISTERGK_REQ,
				  cRegGkMsg.GetServMsg(), cRegGkMsg.GetServMsgLen() );
			MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "TIMER_REGGK_REQ(confidx:%d)\n", cRegGkMsg.GetConfIdx() );
		}
		break;

	case MCU_APPTASKTEST_REQ:	//GUARD Probe Message
		DaemonProcAppTaskRequest( pcMsg );
		break;

	case MTADP_MCU_CONNECT:
		DaemonProcConnectMcu(TRUE);
		break;
	case MTADP_MCU_CONNECT2:
		DaemonProcConnectMcu(FALSE);
		break;

	case MTADP_MCU_REGISTER_REQ:
		DaemonProcRegisterMcu(TRUE);
		break;
	case MTADP_MCU_REGISTER_REQ2:
		DaemonProcRegisterMcu(FALSE);
		break;
		
	case MCU_MTADP_REGISTER_ACK:
	case MCU_MTADP_REGISTER_NACK:
		DaemonProcMcuRegisterRsp( pcMsg );
		break;

	case OSP_DISCONNECT:	//MCU disconnected
		DaemonProcMcuDisconnect( pcMsg, pcApp );
		break;

    case TIMER_GETMSSTATUS_REQ:
    case MCU_MTADP_GETMSSTATUS_ACK:
        DaemonProcGetMsStatusRsp( pcMsg, pcApp );
        break;   
	
	case MCU_MT_REGISTERGK_REQ: //register alias on gk	
		DaemonProcSendRRQ( pcMsg );
		break;

	case MCU_MT_UNREGISTERGK_REQ: //unregister alias on gk
		DaemonProcSendURQ( pcMsg );
		break;

//	case MCU_MT_REREGISTERGK_REQ: //register alias on gk		
//		DaemonProcResendRRQReq( pcMsg );
//		break;

	case MCU_MT_UPDATE_REGGKSTATUS_NTF: //update rrq info on gk
		DaemonProcUpdateRRQNtf( pcMsg );
		break;
		
	case MCU_MT_UPDATE_GKANDEPID_NTF:   //update GatekeeperID/EndpointID info
		DaemonProcUpdateGKANDEPIDNtf( pcMsg );
		break;
		
	case MCU_MT_INVITEMT_REQ:	
		DaemonProcInviteMtReq( pcMsg, pcApp );
		break;
			
	//find the dest instance of MT using SrcSsnId. ! an expedient usage
	case MCU_MT_CREATECONF_ACK:
	case MCU_MT_CREATECONF_NACK:
		{
			u16 dstInst = cServMsg.GetSrcSsnId();
			if( pcApp->GetInstance(dstInst) != NULL )
			{
				//zjj20120720 Bug00097518
				if( MCU_MT_CREATECONF_ACK == pcMsg->event )
				{					
					dstInst = g_cMtAdpApp.m_Msg2InstMap[cServMsg.GetConfIdx()][cServMsg.GetDstMtId()];											
					if( dstInst != cServMsg.GetSrcSsnId() && pcApp->GetInstance(dstInst) != NULL )
					{
						MAPrint( LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[DaemonInstanceEntry] recv MCU_MT_CREATECONF_ACK. so clear conf.%d mtid.%d outgoing instance.%d\n", 
                               cServMsg.GetConfIdx(),cServMsg.GetDstMtId(),dstInst );
						pcMsg->event = MCU_MT_DELMT_CMD;
						pcApp->GetInstance(dstInst)->InstanceEntry( pcMsg );
						pcMsg->event = MCU_MT_CREATECONF_ACK;						
					}
					dstInst = cServMsg.GetSrcSsnId();
				}
				
				pcApp->GetInstance(dstInst)->InstanceEntry(pcMsg);
			}
			else
			{		
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Could not find dest inst for message %u(%s): SrcSsnId missing.\n", 
                               pcMsg->event, OspEventDesc(pcMsg->event) );
			}
			return;
		}
//		break;

	case EV_RAD_RAS_NOTIF:
		DaemonProcGkMcuGeneral( pcMsg, pcApp );
		break;

	case EV_RAD_NEWCHAN_NOTIF:	
	case EV_RAD_CALLCTRL_NOTIF:		
	case EV_RAD_CHANCTRL_NOTIF:	
	case EV_RAD_CONFCTRL_NOTIF:		
		DaemonProcMtMcuGeneral( pcMsg, pcApp );	
		break;

	//QOS����
	case MCU_MT_SETQOS_CMD:     
		DaemonProcMcuMtQosSetCmd( pcMsg );
		break;
	
	//���ý��滯����
	case MCU_MT_MTADPCFG_CMD:
		DaemonProcMtAdpCfgCmd( pcMsg );
		break;
	
	//GK���ø�������
	case MCU_MTADP_GKADDR_UPDATE_CMD:
		DaemonProcUpdateGkAddrCmd( /*pcMsg, */pcApp );
		break;

    case MCU_MT_CONF_STARTCHARGE_REQ:
        DaemonProcMcuMtChargeStartReq( pcMsg );
        break;

    case MCU_MT_CONF_STOPCHARGE_REQ:
        DaemonProcMcuMtChargeStopReq( pcMsg );
        break;

    case EV_GM_CONF_ACCTING_START_RSP:
        DaemonProcGKMcuChargeStartRsp( pcMsg );
        break;

    case EV_GM_CONF_ACCTING_STOP_RSP:
        DaemonProcGKMcuChargeStopRsp( pcMsg );
        break;
        
    case MTADP_GK_CONNECT:
        DaemonProcConnectToGK();
        break;
    
    case MTADP_GK_REGISTER:
        DaemonProcRegToGK( /*pcMsg*/ );
        break;

    case EV_GM_CONF_ACCTING_REG_RSP:
        DaemonProcRegGKRsp( pcMsg );
        break;

	case TIMER_CONFCHARGE_STATUS_NTF:
		DaemonProcMcuGKConfChargeStatusNtf(/* pcMsg */);
		break;

	case EV_GM_CONF_ACCTING_STATUS_NTF:
		DaemonProcGKMcuConfChargeStatusNtf( pcMsg );
		break;
        
    case MCU_MT_UNREGGK_NPLUS_CMD:
        DaemonProcNPlusUnregGKCmd( pcMsg );
        break;

    case MCU_MT_CONF_STOPCHARGE_CMD:
        DaemonProcNPlusStopChargeCmd( pcMsg );
        break;
        
    case MCU_MT_RESTORE_MCUE164_NTF:
        DaemonProcMcuE164RestoreNtf( pcMsg );
        break;

    case CALLOUT_NEXT_NOTIFY:
        DaemonProcCallNextMtNtf();
        break;

    case TIMER_URQ_RSP:
        DaemonProcURQTimeOut();
        break;
 
	case TIMER_RAS_RSP:
        DaemonPronRRQTimeOut();
        break;

	case MCU_MT_MMCUCONFNAMESHOWTYPE_CMD:
		DaemonProcMMcuConfNameShowTypeCmd( pcMsg );
		break;

	case MCU_MT_CHANGEADMINLEVEL_CMD:
		DaemonProcMMcuChangeAdminLevelCmd( pcMsg );
		break;
        
	default: 
		DaemonProcMcuMtGeneral( pcMsg, pcApp );
		break;			
	}

	return;
}

/*=============================================================================
  �� �� ���� DaemonProcInviteMtReq
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const  pcMsg
             CApp* pcApp
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                ����
=============================================================================*/
void CMtAdpInst::DaemonProcInviteMtReq( CMessage * const  pcMsg, CApp* pcApp )
{			
	ASSERT( pcMsg && pcApp );

    if ( pcMsg == NULL)
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[DaemonProcInviteMtReq]NULL message received, ignore it\n!" );
		return;
    }
    CServMsg cServMsg(pcMsg->content, pcMsg->length );


	if( !BODY_LEN_GE( cServMsg, sizeof(TMt) + sizeof(TMtAlias) ) )
		return;
	
	TMt tMt = *(TMt*)cServMsg.GetMsgBody();
	TMtAlias tMtAlias = *(TMtAlias*)( cServMsg.GetMsgBody() + sizeof(TMt) );
	
	u8 byDstConfIdx = cServMsg.GetConfIdx();
	u8 byDstMtId = tMt.GetMtId();
	BOOL32 bMtAlreadyCalledOut = FALSE;
	
	u32  dwDstIp     = tMtAlias.m_tTransportAddr.GetIpAddr();
	u16  wDstPort    = tMtAlias.m_tTransportAddr.GetPort();
	s8*  pszDstAlias = tMtAlias.m_achAlias;
	
	//check if we have already called out this mt
	//todo: optimize the lookup algorithm
	for( u16 wDstInst = 1; wDstInst <= MAXNUM_DRI_MT; wDstInst++ )
	{
//		CMtAdpInst *pInst = (CMtAdpInst*)pcApp->GetInstance(wDstInst);//pn
        CMtAdpInst *pInst = NULL;
        if ( pcApp != NULL)
        {
            pInst = (CMtAdpInst*)pcApp->GetInstance(wDstInst);
        }

		if(!pInst || pInst->CurState() == STATE_IDLE)
			continue;
		
		u8		byMtId	  = pInst->m_byMtId;
		u8		byConfIdx = pInst->m_byConfIdx;
		u32		dwIp      = pInst->m_tMtAlias.m_tTransportAddr.GetIpAddr();;
		u16		wPort	  = pInst->m_tMtAlias.m_tTransportAddr.GetPort();;
		s8*	    pszAlias  = pInst->m_tMtAlias.m_achAlias;
		BOOL32  bInConf	  = pInst->m_bHasJoinedConf;
		
		BOOL32	bSameMt	= FALSE;

		if( 0 == wDstPort )
		{
			wDstPort = MCU_Q931_PORT;
		}
		if( 0 == wPort )
		{
			wPort = MCU_Q931_PORT;
		}
		
		//zbq[04/21/2008] ��������ƥ���ٽ���ϸ��, ����ձ�������»ᵼ������.
		if ( tMtAlias.m_AliasType == pInst->m_tMtAlias.m_AliasType )
		{
			if (pInst->m_byRecType != TRecChnnlStatus::TYPE_UNUSE)
			{
				// vrs��¼���ն�����ͬip���룬��������
			}
			else if( tMtAlias.m_AliasType == (u8)mtAliasTypeTransportAddress )
			{
				if( dwIp != 0 && dwDstIp == dwIp && wDstPort == wPort )
					bSameMt = TRUE;
			}
			else
			{
				if( memcmp( pszAlias, pszDstAlias, LEN_TERID ) == 0 )
					bSameMt = TRUE;
			}
		}
		
		//duplicate invitation
		if( bSameMt && byDstConfIdx == byConfIdx/* && dstMtId == mtId*/ )
		{
			if( bInConf )
			{
                // zbq [05/28/2007] inconsistent state with that of vc, just clear it!
                pInst->ClearInst();

                if ( tMtAlias.m_AliasType == (u8)mtAliasTypeTransportAddress )
                {
                    u32 dwShowIp = htonl(dwIp);
				    MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "mt %u.%u.%u.%u already connected by inst.%d, drop it\n",
                                                             QUADADDR( dwShowIp ), wDstInst );
                }
                else
                {
                    MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "mt %s already connected by inst.%d, drop it\n", pszAlias, wDstInst );
                }
			}
			else
			{
				bMtAlreadyCalledOut = TRUE;
				if( tMtAlias.m_AliasType == (u8)mtAliasTypeTransportAddress )	
                {
                    u32 dwShowIp = htonl(dwIp);
					MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "mt %u.%u.%u.%u already called out by inst.%d, ignore!\n",
                                                            QUADADDR( dwShowIp ), wDstInst );
                }
				else
                {
					MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "mt %s already called out by inst.%d\n", pszAlias, wDstInst );
                }
			}
			break;
		}
        else
        {
            // zbq [07/16/2007] ����MCUVC��ǿ�Ƽ�Ȩ:���Ѿ�����һ�µ�ID����,ɾ���Ѵ���MT,��֤Ψһ��
            if ( byDstConfIdx == byConfIdx && byDstMtId == byMtId )
            {
                if( tMtAlias.m_AliasType == (u8)mtAliasTypeTransportAddress )	
                {
                    dwIp = ntohl(dwIp);
                    dwDstIp = ntohl(dwDstIp);
                    MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Mt %u.%u.%u.%u by inst.%d<%d, %d> conflict with new coming Mt %u.%u.%u.%u��drop the former one !\n",
                        QUADADDR(dwIp), wDstInst, byDstConfIdx, byDstMtId, QUADADDR(dwDstIp) );
                }
                else
                {
                    MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Mt.%s by inst.%d<%d, %d> conflict with new coming Mt.%s��drop the former one!\n",
                        pszAlias, wDstInst, byDstConfIdx, byDstMtId, pszDstAlias );
                }
                pInst->ClearInst();
            }
        }
	}
	
    // zbq [04/05/2007] �������в���
	if(!bMtAlreadyCalledOut)
	{
        // �º��е�������ѯ�Ƿ���еȴ� �� ���ں���
        if ( g_cMtAdpApp.m_tWaitQueue.IsMtInQueue(byDstConfIdx, tMt.GetMtId()) )
        {
            MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "ConfIdx.%d MT.%d already in wait queue!\n", byDstConfIdx, tMt.GetMtId());
            return;
        }
        else if ( g_cMtAdpApp.m_tCallQueue.IsMtInQueue(byDstConfIdx, tMt.GetMtId()) )
        {
            MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "ConfIdx.%d MT.%d already in call queue!\n", byDstConfIdx, tMt.GetMtId());
            return;
        }

        //���������Ϣ
        TMtCallInfo tCallInfo;
        tCallInfo.m_cConfId   = cServMsg.GetConfId();
        tCallInfo.m_byConfIdx = byDstConfIdx;
        tCallInfo.m_tMt       = tMt;
        tCallInfo.m_tMtAlias  = tMtAlias;
        tCallInfo.m_tConfAlias = *(TMtAlias*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias));
        tCallInfo.m_byEncrypt  = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias) * 2);
        tCallInfo.m_wCallRate  = *(u16*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias) * 2 + sizeof(u8));
        tCallInfo.m_tCap       = *((TCapSupport*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias) * 2 + sizeof(u8) + sizeof(u16)));
 	    tCallInfo.m_wForceCallInfo = *((u16*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias) * 2 + sizeof(u8) + sizeof(u16) + sizeof(TCapSupport)));
		tCallInfo.m_byAdminLevel = *((u8*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias) * 2 + sizeof(u8) + sizeof(u16) + sizeof(TCapSupport) + sizeof(u16)));
		tCallInfo.m_tCapEx = *((TCapSupportEx*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias) * 2 + sizeof(u8) + sizeof(u16) + sizeof(TCapSupport) + sizeof(u16) + sizeof(u8)));
		
		// ����������˫������չ��������ѡ [12/8/2011 chendaiwei]
		memcpy(tCallInfo.m_atMainStreamCapEx,(u8*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias) * 2 + sizeof(u8) + sizeof(u16) + sizeof(TCapSupport) + sizeof(u16) + sizeof(u8) + sizeof(TCapSupportEx)),sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
		memcpy(tCallInfo.m_atDoubleStreamCapEx,(u8*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias) * 2 + sizeof(u8) + sizeof(u16) + sizeof(TCapSupport) + sizeof(u16) + sizeof(u8) + sizeof(TCapSupportEx) +sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM ),sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
		
		//tCallInfo.m_byAudioTrackNum = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias) * 2 + sizeof(u8) + sizeof(u16) + sizeof(TCapSupport) + sizeof(u16) + sizeof(u8) + sizeof(TCapSupportEx) +2*sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
		tCallInfo.m_tConfE164Alias = *(TMtAlias*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias) * 2 + sizeof(u8) + sizeof(u16) + sizeof(TCapSupport) + sizeof(u16) + sizeof(u8) + sizeof(TCapSupportEx) +2*sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
		tCallInfo.m_byNPlusSmcuAliasFlag = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias) * 2 + sizeof(u8) + sizeof(u16) + sizeof(TCapSupport) + sizeof(u16) + sizeof(u8) + sizeof(TCapSupportEx) +2*sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM+sizeof(TMtAlias));
        
		//׷�ӵĶ���Ƶ��ʽ��Ϣ
		memcpy(tCallInfo.m_atAudioTypeDesc,(u8*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias) * 2 + 
				sizeof(u8) + sizeof(u16) + sizeof(TCapSupport) + sizeof(u16) + sizeof(u8) + sizeof(TCapSupportEx) +
				2*sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM+sizeof(TMtAlias)+sizeof(u8)),
				sizeof(tCallInfo.m_atAudioTypeDesc) );
		// calling ����δ����ֱ�ӽ����ж��в��������. ������ȴ�����
        if ( !g_cMtAdpApp.m_tCallQueue.IsQueueFull() )
        {
            if ( g_cMtAdpApp.m_tCallQueue.ProcQueueInfo(tCallInfo) ) 
            {
                DaemonCallNextFromQueue(tCallInfo);
            }
        }
        else
        {
            g_cMtAdpApp.m_tWaitQueue.ProcQueueInfo(tCallInfo);
        }
	}
    return;
}

/*=============================================================================
  �� �� ���� DaemonProcCallNextMtNtf
  ��    �ܣ� �������в��Ե���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcCallNextMtNtf( void )
{
    if ( g_cMtAdpApp.m_tCallQueue.IsQueueFull() )
    {
        MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[CallNextMtNtf] Call queue full, ignore the opr !\n");
        return;
    }
    if ( g_cMtAdpApp.m_tWaitQueue.IsQueueNull() ) 
    {
        MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[CallNextMtNtf] Wait queue Null, ignore the opr !\n");
        return;
    }

    TMtCallInfo tNextCall;
    if ( !g_cMtAdpApp.m_tWaitQueue.ProcQueueInfo(tNextCall, FALSE, FALSE) )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[CallNextMtNtf] ProcWaitQueueInfo <%d, %d> !\n", 
                        tNextCall.m_byConfIdx, tNextCall.m_tMt.GetMtId() );
        return;
    }
    if ( !g_cMtAdpApp.m_tCallQueue.ProcQueueInfo(tNextCall) )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[CallNextMtNtf] ProcCallQueueInfo <%d, %d> !\n", 
                        tNextCall.m_byConfIdx, tNextCall.m_tMt.GetMtId() );        
        return;
    }
    DaemonCallNextFromQueue( tNextCall );

    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[CallNextMtNtf] calling to Next<%d, %d> !\n", 
                tNextCall.m_byConfIdx, tNextCall.m_tMt.GetMtId() );
    return;    
}

/*=============================================================================
  �� �� ���� DaemonProcMMcuConfNameShowTypeCmd
  ��    �ܣ� mcu����������ʾ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/01/16    4.0			�ܾ���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcMMcuConfNameShowTypeCmd( CMessage * const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	u8 byShowType = *((u8*)cServMsg.GetMsgBody());

	g_cMtAdpApp.m_byCasAliasType = byShowType;

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "CasAliasType is change.now CasAliasType is %d\n", g_cMtAdpApp.m_byCasAliasType );
}

/*=============================================================================
  �� �� ���� DaemonProcMMcuConfNameShowTypeCmd
  ��    �ܣ� mcu����������ʾ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/01/16    4.0			�ܾ���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcMMcuChangeAdminLevelCmd( CMessage * const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	u8 byAdminLevel = *((u8*)cServMsg.GetMsgBody());

	g_cMtAdpApp.m_byCascadeLevel = byAdminLevel;

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "AdminLevel is change.now AdminLevel is %d\n", g_cMtAdpApp.m_byCascadeLevel );
}

/*=============================================================================
  �� �� ���� DaemonCallNextFromQueue
  ��    �ܣ� ͨ��TMtCallInfo�����º���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonCallNextFromQueue( TMtCallInfo &tCallMtInfo )
{
    CServMsg cServMsg;
    cServMsg.SetConfIdx( tCallMtInfo.m_byConfIdx );
    cServMsg.SetConfId( tCallMtInfo.m_cConfId );
    cServMsg.SetMsgBody( (u8*)&tCallMtInfo.m_tMt, sizeof(TMt) );
    cServMsg.CatMsgBody( (u8*)&tCallMtInfo.m_tMtAlias, sizeof(TMtAlias) );
	LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[DaemonCallNextFromQueue]: mt.%d aliasType.%d\n", 
		tCallMtInfo.m_tMt.GetMtId(), tCallMtInfo.m_tMtAlias.m_AliasType);
    cServMsg.CatMsgBody( (u8*)&tCallMtInfo.m_tConfAlias, sizeof(TMtAlias) );
    cServMsg.CatMsgBody( (u8*)&tCallMtInfo.m_byEncrypt, sizeof(u8) );
    cServMsg.CatMsgBody( (u8*)&tCallMtInfo.m_wCallRate, sizeof(u16) );
    cServMsg.CatMsgBody( (u8*)&tCallMtInfo.m_tCap, sizeof(TCapSupport) );
 	cServMsg.CatMsgBody( (u8*)&tCallMtInfo.m_wForceCallInfo, sizeof(u16) );
	cServMsg.CatMsgBody( (u8*)&tCallMtInfo.m_byAdminLevel, sizeof(u8) );
	cServMsg.CatMsgBody( (u8*)&tCallMtInfo.m_tCapEx, sizeof(TCapSupportEx) );

	//��˫����չ��������ѡ [12/8/2011 chendaiwei]
	cServMsg.CatMsgBody( (u8*)&tCallMtInfo.m_atMainStreamCapEx[0], sizeof(tCallMtInfo.m_atMainStreamCapEx));
	cServMsg.CatMsgBody( (u8*)&tCallMtInfo.m_atDoubleStreamCapEx[0],sizeof(tCallMtInfo.m_atDoubleStreamCapEx));
	//cServMsg.CatMsgBody((u8*)&tCallMtInfo.m_byAudioTrackNum,sizeof(tCallMtInfo.m_byAudioTrackNum));
	cServMsg.CatMsgBody( (u8*)&tCallMtInfo.m_tConfE164Alias, sizeof(TMtAlias) );
	cServMsg.CatMsgBody( (u8*)&tCallMtInfo.m_byNPlusSmcuAliasFlag, sizeof(u8) );
	cServMsg.CatMsgBody( (u8*)tCallMtInfo.m_atAudioTypeDesc,sizeof(tCallMtInfo.m_atAudioTypeDesc));

	g_cMtAdpApp.SetMtInviteMsgInOsp( tCallMtInfo.m_byConfIdx,tCallMtInfo.m_tMt.GetMtId() );

    post( MAKEIID(AID_MCU_MTADP, CInstance::PENDING), MCU_MT_INVITEMT_REQ, 
                                                      cServMsg.GetServMsg(),
                                                      cServMsg.GetServMsgLen() );
    return;
}

/*=============================================================================
  �� �� ���� DaemonDelMtFromQueue
  ��    �ܣ� ����Ӧ����ɾ��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/05/12    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonDelMtFromQueue( u8 byConfIdx, u8 byMtId )
{
    if ( byConfIdx > MAX_CONFIDX || byMtId > MAXNUM_CONF_MT )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[DeamonDelMtFromQueue] param err<%d,%d> !\n", byConfIdx, byMtId);
        return;
    }

    TMtCallInfo tTmpCallInfo;
    tTmpCallInfo.m_byConfIdx = byConfIdx;
    tTmpCallInfo.m_tMt.SetMtId(byMtId);
    
    if ( g_cMtAdpApp.m_tWaitQueue.IsMtInQueue(byConfIdx, byMtId) )
    {
        g_cMtAdpApp.m_tWaitQueue.ProcQueueInfo(tTmpCallInfo, FALSE);
        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Mt<ConfIdx, MtId> = <%u, %u> in WaitQueue "
                  "delete it directly !\n", byConfIdx, byMtId );
    }
    else if ( g_cMtAdpApp.m_tCallQueue.IsMtInQueue(byConfIdx, byMtId) )
    {
        g_cMtAdpApp.m_tCallQueue.ProcQueueInfo(tTmpCallInfo, FALSE);
        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Mt<ConfIdx, MtId> = <%u, %u> in CallQueue "
                  "delete it directly !\n", byConfIdx, byMtId );                        
    }
    else
    {
        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Mt<ConfIdx, MtId> = <%u, %u> unexist in this mtadp !\n\n", byConfIdx, byMtId);
    }

    return;
}

/*=============================================================================
  �� �� ���� DaemonProcMcuMtGeneral
  ��    �ܣ� �����mcu��mt�Ļ�����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
             CApp* pcApp
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcMcuMtGeneral( CMessage * const pcMsg, CApp* pcApp )
{	
	ASSERT( pcMsg && pcApp );

    if ( pcMsg == NULL )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[DaemonProcMcuMtGeneral]NULL message received, ignore it\n!" );
        return;
    }
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	if( pcMsg != NULL && 
		( (EV_MCUMT_BGN <= pcMsg->event && pcMsg->event <= EV_MCUMT_END) 
		  ||
		  (EV_VCSVC_BGN <= pcMsg->event && pcMsg->event <= EV_VCSVS_END)
		  || 
		  (EV_MCSVC_BGN <= pcMsg->event && pcMsg->event <= EV_MCSVC_END)
		  || 
		  (EV_MCUREC_BGN <= pcMsg->event && pcMsg->event <= EV_MCUREC_END)//vrs��¼����Ϣ֧��
		)
		)
	{
	
		u8 byConfIdx = cServMsg.GetConfIdx();
		u8 byMtId    = cServMsg.GetDstMtId();
		u16 wDstInst = 0;
		
		//invalid message
		if( byConfIdx == 0 || 
			byMtId    > MAXNUM_CONF_MT || 
			byConfIdx > MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE )
		{
            if ( pcMsg != NULL)
            {
                MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "invalid msgmsg %u(%s) recved. <confIdx, mtId>=<%u, %u>\n", 
				           pcMsg->event, OspEventDesc( pcMsg->event ), byConfIdx, byMtId );
            }
			return;
		}
		
		if( byMtId == 0 ) //broadcast to all terminals in given conf
		{
			for( s32 nConfNum = 1; nConfNum <= MAXNUM_CONF_MT; nConfNum ++ )
			{
				wDstInst = g_cMtAdpApp.m_Msg2InstMap[byConfIdx][nConfNum];
				if( pcApp != NULL && pcApp->GetInstance(wDstInst) != NULL )
                {
                    pcApp->GetInstance(wDstInst)->InstanceEntry(pcMsg);
                }
                else
                {
                    // zbq [05/11/2007] ɾ�����ն��ڶ���������
                    if ( pcMsg != NULL && MCU_MT_DELMT_CMD == pcMsg->event )//pn
                    {
                        DaemonDelMtFromQueue( byConfIdx, nConfNum );
                    }
                }
			}
		}
		else // message for single instance
		{
			wDstInst = g_cMtAdpApp.m_Msg2InstMap[byConfIdx][byMtId];

			if( pcApp != NULL && pcApp->GetInstance(wDstInst) != NULL )
			{
				pcApp->GetInstance(wDstInst)->InstanceEntry(pcMsg);	//pn			
			}
			else
			{
                if ( pcMsg != NULL)
                {
                    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "could not find dest inst for msg %u(%s)"
                        "<ConfIdx, MtId> = <%u, %u>.\n", 
                          pcMsg->event, OspEventDesc( pcMsg->event ), byConfIdx, byMtId );
                }

                // zbq [05/11/2007] ɾ�����ն��ڶ���������
                if ( pcMsg != NULL && MCU_MT_DELMT_CMD == pcMsg->event )//pn
                {
                    DaemonDelMtFromQueue( byConfIdx, byMtId );
					if( g_cMtAdpApp.IsMtInviteMsgInOsp(byConfIdx,byMtId) )
					{
						g_cMtAdpApp.SetNeglectInviteMsg( byConfIdx,byMtId );
					}
                }
			}
		}
		return;
	}
	else
	{
        if ( pcMsg != NULL)
        {
            UNEXPECTED_MESSAGE( pcMsg->event );
        }
	}
}


/*=============================================================================
  �� �� ���� DaemonProcMtMcuGeneral
  ��    �ܣ� ������ն˾���Э��ջ��mcu�Ļ�����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
             CApp* pcApp
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcMtMcuGeneral( CMessage * const pcMsg, CApp* pcApp )
{	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );	

	if( !BODY_LEN_GE( cServMsg, sizeof(HCALL) ) ) 
		return;

	HCALL hsCall = *(HCALL*)cServMsg.GetMsgBody();
	HAPPCALL haCall = NULL;
	kdvCallGetHandle( hsCall, &haCall );
    /*lint -save -e507*/
	u8 byDstInst = (u8)haCall;
    /*lint -restore*/

	if( pcApp->GetInstance(byDstInst) )
	{
		pcApp->GetInstance(byDstInst)->InstanceEntry(pcMsg);
	}	
}


/*=============================================================================
  �� �� ���� DaemonInstanceDump
  ��    �ܣ� ��ӡ����ͳ����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwParam
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
void CMtAdpInst::DaemonInstanceDump( u32 dwParam )
{
	StaticLog( "\n----------------Call Statistics(dwParam.%d):  \n\n",dwParam);	
	
	StaticLog( "Current number of connected calls: \t%5u\n",    g_cMtAdpApp.m_tMaStaticInfo.m_byCurNumOnline );
	StaticLog( "Current number of calls in dialing: \t%5u\n\n", g_cMtAdpApp.m_tMaStaticInfo.m_byCurNumCalling );

	StaticLog( "Total number of incoming calls: \t%5u\n",   g_cMtAdpApp.m_tMaStaticInfo.m_dwIncomingCalls );		
	StaticLog( "Total number of outgoing calls: \t%5u\n",   g_cMtAdpApp.m_tMaStaticInfo.m_dwOutgoingCalls );
	StaticLog( "Total number of successful calls: \t%5u\n", g_cMtAdpApp.m_tMaStaticInfo.m_dwSuccessfulCalls );
	StaticLog( "Total number of failed calls: \t\t%5u\n\n", g_cMtAdpApp.m_tMaStaticInfo.m_dwFailedCalls );

	StaticLog( "Maximum simultaneous connected calls: \t%5u\n", g_cMtAdpApp.m_tMaStaticInfo.m_byMaxNumOnline );
	StaticLog( "Maximum simultaneous calls in dialing:\t%5u\n", g_cMtAdpApp.m_tMaStaticInfo.m_byMaxNumCalling );
}


/*=============================================================================
  �� �� ���� DaemonProcAppTaskRequest
  ��    �ܣ� GUARDģ��̽����Ϣ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const  pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcAppTaskRequest( CMessage * const  pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );
}

/*=============================================================================
    �� �� ���� DaemonProcPowerOn
    ��    �ܣ� Mtadp�ϵ紦��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const  pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2003/11     1.0			TanGuang              ����
    2005/9/10   4.0			����                ����˫�㽨��֧��
=============================================================================*/
void CMtAdpInst::DaemonProcPowerOn( void )
{
    //Start connect or register MCU
	//Notify������Ƕ��ʽAttachedToVc������matadp���ÿ��Ƕ���ע�ἰ��Ӧ����������

	if (g_cMtAdpApp.m_bAttachedToVc)
	{
		OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), MTADP_MCU_REGISTER_REQ, NULL, 0 );
	}
	else
	{
		OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), MTADP_MCU_CONNECT, NULL, 0 );
		if (g_cMtAdpApp.m_bDoubleLink)
		{
			OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), MTADP_MCU_CONNECT2, NULL, 0 );
		}
	}

	return;
}

/*=============================================================================
    �� �� ���� DaemonProcConnectMcu
    ��    �ܣ� ����MCU����ȡ��Ӧ�Ľڵ��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� BOOL32 bMcuA -> TRUE-��һ�����ӽڵ� FALSE-�ڶ������ӽڵ�
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2003/11     1.0			TanGuang              ����
    2005/9/10   4.0			����                ����˫�㽨��֧��
=============================================================================*/
void CMtAdpInst::DaemonProcConnectMcu( BOOL32 bMcuA )
{
	if (g_cMtAdpApp.m_bAttachedToVc)
	{
		StaticLog( "[MtAdp]: attach vc mode is not connecting!\n") ;
		return;
	}

    // zbq [07/04/2007] �ܾ�H323��ʼ��ʧ�ܺ����·��������
    if ( g_cMtAdpApp.m_tMaStaticInfo.m_bH323InitFailed )
    {
        StaticLog( "[MtAdp] H323Init failed, connect rejected\n" );
        return;
    }
	
	u32	dwMcuNode   = INVALID_NODE;
	u32	dwMcuIpAddr = 0;
	u16	wMcuPort    = 0;
	u16 wConTimerId = 0;
	u16 wRegTimerId = 0;
	if (bMcuA)
	{
#ifdef _IS22_		
		g_cMtAdpApp.m_dwMcuNode = OspConnectTcpNode(g_cMtAdpApp.m_dwMcuIpAddr, g_cMtAdpApp.m_wMcuPort , 10, 0, 100 );  
		
        if (OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNode))
        {
            ::OspNodeDiscCBRegQ(g_cMtAdpApp.m_dwMcuNode, GetAppID(),GetInsID());
			dwMcuNode   = g_cMtAdpApp.m_dwMcuNode;
			OspSetHBParam( dwMcuNode, g_cMtAdpApp.GetDiscHeartBeatTime(), g_cMtAdpApp.GetDiscHeartBeatNum() );
            LogPrint(LOG_LVL_KEYSTATUS,MID_MCULIB_MTADP,"Connect to Mcu Success,node is %u!\n", dwMcuNode);
        }
        else
        {			
            //����ʧ��
             LogPrint(LOG_LVL_KEYSTATUS,MID_MCULIB_MTADP,"[prs8ke] Failed to Connect Mcu :%d\n",dwMcuNode);
			//bRet = FALSE;
        }	
#else
		dwMcuNode   = BrdGetDstMcuNode();
#endif
		
		dwMcuIpAddr = g_cMtAdpApp.m_dwMcuIpAddr;
		wMcuPort    = g_cMtAdpApp.m_wMcuPort;
		wConTimerId = MTADP_MCU_CONNECT;
		wRegTimerId = MTADP_MCU_REGISTER_REQ;

		g_cMtAdpApp.m_dwMcuNode = dwMcuNode;
	}
	else
	{
#ifdef _IS22_		
		g_cMtAdpApp.m_dwMcuNodeB = OspConnectTcpNode(g_cMtAdpApp.m_dwMcuIpAddrB, g_cMtAdpApp.m_wMcuPortB, 10, 0, 100 );  
	
		if (OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNodeB))
		{
			::OspNodeDiscCBRegQ(g_cMtAdpApp.m_dwMcuNodeB, GetAppID(),GetInsID());
			dwMcuNode = g_cMtAdpApp.m_dwMcuNodeB;
			OspSetHBParam( dwMcuNode, g_cMtAdpApp.GetDiscHeartBeatTime(), g_cMtAdpApp.GetDiscHeartBeatNum() );
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCULIB_MTADP,"Connect to Mcu Success,node is %u!\n", dwMcuNode);
		}
		else
		{			
			//����ʧ��
			 LogPrint(LOG_LVL_KEYSTATUS,MID_MCULIB_MTADP,"[prs8ke] Failed to Connect Mcu :%d\n",dwMcuNode);
			//bRet = FALSE;
		}
		
#else
		dwMcuNode   = BrdGetDstMcuNodeB();
#endif
		
		dwMcuIpAddr = g_cMtAdpApp.m_dwMcuIpAddrB;
		wMcuPort    = g_cMtAdpApp.m_wMcuPortB;
		wConTimerId = MTADP_MCU_CONNECT2;
		wRegTimerId = MTADP_MCU_REGISTER_REQ2;

		g_cMtAdpApp.m_dwMcuNodeB = dwMcuNode;
	}
	
	if (!OspIsValidTcpNode(dwMcuNode))
	{
		SetTimer(wConTimerId, CONNECTING_MCU_INTERVAL);

		StaticLog( "[MtAdp] Connecting to Mcu.%s node.%u %u.%u.%u.%u:%u\n", 
			      (bMcuA?"A":"B"), dwMcuNode, QUADADDR(dwMcuIpAddr), wMcuPort);		
	}
	else
	{
		OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());
		KillTimer(wRegTimerId);//����֮�������ԭ����ע��timer��Ӧ��kill����
        if ( !g_cMtAdpApp.m_bWaitingURQRsp )
        {
            KillTimer(wConTimerId);
            OspPost(MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), wRegTimerId, NULL, 0);	
			
            StaticLog( "[MtAdp] Mcu.%s node.%u IP=%u.%u.%u.%u connected, start reg;\n", 
                                    (bMcuA?"A":"B"), dwMcuNode, QUADADDR(dwMcuIpAddr));
        }
        else
        {
            StaticLog( "[MtAdp] Connecting to Mcu.%s node.%u %u.%u.%u.%u:%u be Interupted for MSLinkStatus Err, waiting for URQ Rsp ...\n", 
                                     (bMcuA?"A":"B"), dwMcuNode, QUADADDR(dwMcuIpAddr), wMcuPort);                        
        }
	}

	return;
}

/*=============================================================================
  �� �� ���� DaemonProcRegisterMcu
  ��    �ܣ� ��MCUע��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� BOOL32 bMcuA -> TRUE-��һ�����ӽڵ� FALSE-�ڶ������ӽڵ�
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11     1.0			TanGuang              ����
  2005/9/10   4.0			����                ����˫�㽨��֧��
=============================================================================*/
void CMtAdpInst::DaemonProcRegisterMcu( BOOL32 bMcuA )
{
   	TMtAdpReg tMtAdp;	
	tMtAdp.SetDriId( g_cMtAdpApp.m_byDriId );	
	tMtAdp.SetAttachMode( g_cMtAdpApp.m_bAttachedToVc );
	tMtAdp.SetProtocolType( PROTOCOL_TYPE_H323 );
//	tMtAdp.SetMaxMtNum( MAXNUM_H225H245_MT ); //����license��������
	tMtAdp.SetMaxMtNum((u8)g_cMtAdpApp.m_wMaxNumConntMt); // xliang [10/31/2008] license�д��Ľ�������
	tMtAdp.SetMaxHDMtNum(g_cMtAdpApp.m_byMaxHDMtNum); //  [7/28/2011 chendaiwei]���ø���������
	tMtAdp.SetMaxAudMtNum(g_cMtAdpApp.m_wMaxNumConnAudMt);
	tMtAdp.SetAlias( g_cMtAdpApp.m_achMtAdpAlias );
	tMtAdp.SetIpAddr( ntohl(g_cMtAdpApp.m_dwMtAdpIpAddr) );
    tMtAdp.SetQ931Port( g_cMtAdpApp.m_wQ931Port );
    tMtAdp.SetRasPort( g_cMtAdpApp.m_wRasPort );
	//TODO �汾����Ҫ���� [12/9/2011 chendaiwei]
	tMtAdp.SetVersion(DEVVER_MTADP);
	tMtAdp.SetMacAddr(g_cMtAdpApp.m_abyCRIMacAddr);

	CServMsg cServMsg;
	cServMsg.SetMsgBody( (u8 *)&tMtAdp, sizeof(tMtAdp) );

	if (bMcuA)
	{
		if (g_cMtAdpApp.m_bAttachedToVc || OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNode))
		{
			post( MAKEIID( AID_MCU_MTADPSSN, g_cMtAdpApp.m_byDriId, TCP_GATEWAY ), 
				  MTADP_MCU_REGISTER_REQ, cServMsg.GetServMsg(), 
				  cServMsg.GetServMsgLen(), g_cMtAdpApp.m_dwMcuNode );
			SetTimer(MTADP_MCU_REGISTER_REQ, REGISTERING_MCU_INTERVAL);
		}
		else
		{
			StaticLog( "[MtAdp] Mcu node.0 IP=%u.%u.%u.%u lost when reg, reconnect\n", 
				      QUADADDR(g_cMtAdpApp.m_dwMcuIpAddr));
			OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), MTADP_MCU_CONNECT, NULL, 0 );
		}
	}
	else
	{
		if (OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNodeB))
		{
			post( MAKEIID( AID_MCU_MTADPSSN, g_cMtAdpApp.m_byDriId, TCP_GATEWAY ), 
				  MTADP_MCU_REGISTER_REQ, cServMsg.GetServMsg(), 
				  cServMsg.GetServMsgLen(), g_cMtAdpApp.m_dwMcuNodeB );
			SetTimer(MTADP_MCU_REGISTER_REQ2, REGISTERING_MCU_INTERVAL);
		}
		else
		{
			StaticLog( "[MtAdp] Mcu node.1 IP=%u.%u.%u.%u lost when reg, reconnect\n", 
				      QUADADDR(g_cMtAdpApp.m_dwMcuIpAddrB));
			OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), MTADP_MCU_CONNECT2, NULL, 0 );
		}
	}

	return;
}

/*=============================================================================
    �� �� ���� DaemonProcMcuRegisterRsp
    ��    �ܣ� ����MCUע����Ӧ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const  pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2003/11     1.0			TanGuang              ����
    2005/9/10   4.0			����                ����˫�㽨��֧��
=============================================================================*/
void CMtAdpInst::DaemonProcMcuRegisterRsp( CMessage * const  pcMsg )
{
	// �յ�MCU NACK֮ʱ����ȥ��ʼ��323Э��ջ��ֱ��return [pengguofeng 8/29/2013]
	LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[DaemonProcMcuRegisterRsp]recv %d(%s) from mcu\n", pcMsg->event, OspEventDesc(pcMsg->event));
	if ( MCU_MTADP_REGISTER_NACK == pcMsg->event )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[DaemonProcMcuRegisterRsp]mcu ver diffs from mtadp, return!\n");
		return;
	}
	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
    // zgc, 2008-09-27, �Ƿ��һ���յ�ע��ɹ���Ӧ
    BOOL32 bIsFirstRegRsp = FALSE;

	if (MCU_MTADP_REGISTER_ACK == pcMsg->event)
	{ 
	   if( !BODY_LEN_GE( cServMsg, sizeof(TMtAdpRegRsp) ) ) 
       {
            StaticLog( "[MtAdp][DaemonProcMcuRegisterRsp]: Wrong TMtAdpRegRsp Length, expect.%d, actural.%d��Disconnect from MCU ...!\n",
                       sizeof(TMtAdpRegRsp), cServMsg.GetMsgBodyLen() );
            
            // zbq [09/18/2007] ע��ʧ�ܣ��Ͽ���·
            if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNodeB);
            }      
            return;
       }		   

	   struct TMtAdpRegRsp tRsp = *(struct TMtAdpRegRsp*)( cServMsg.GetMsgBody() );
	   BOOL32 bMcuA = FALSE;
	   if (g_cMtAdpApp.m_dwMcuNode == pcMsg->srcnode)
	   {
		   bMcuA = TRUE;
	   }
	   else if (g_cMtAdpApp.m_dwMcuNodeB == pcMsg->srcnode)
	   {
		   bMcuA = FALSE;
	   }
	   else
	   {
		   StaticLog( "[MtAdp]: Receive McuRegisterRsp but srcnode is invalid!\n" );
		   return;
	   }

       //�����ʶע��MCU�ɹ�����·״̬������µ�Urq����Ӧȡ��
       g_cMtAdpApp.m_bWaitingURQRsp = FALSE;


	   if (bMcuA)
	   {
		   KillTimer( MTADP_MCU_REGISTER_REQ );
		   g_cMtAdpApp.m_dwMcuNode  = pcMsg->srcnode;
		   g_cMtAdpApp.m_dwMcuIId   = pcMsg->srcid;
		   g_cMtAdpApp.m_dwVcIpAddr = htonl(tRsp.GetVcIp());
		   tRsp.GetAlias( g_cMtAdpApp.m_achMcuAlias, sizeof(g_cMtAdpApp.m_achMcuAlias) );
	   }
	   else
	   {
		   KillTimer( MTADP_MCU_REGISTER_REQ2 );
		   g_cMtAdpApp.m_dwMcuNodeB  = pcMsg->srcnode;
		   g_cMtAdpApp.m_dwMcuIIdB   = pcMsg->srcid;
		   g_cMtAdpApp.m_dwVcIpAddrB = htonl(tRsp.GetVcIp());
		   tRsp.GetAlias( g_cMtAdpApp.m_achMcuAlias, sizeof(g_cMtAdpApp.m_achMcuAlias) );
	   }
	   
	   // guzh [6/12/2007] У��Ự����
	   if ( g_cMtAdpApp.m_dwMpcSSrc == 0 )
	   {
            // zgc, 2008-09-27, ��¼��һ���յ�ע��ɹ���Ӧ
            bIsFirstRegRsp = TRUE;

			g_cMtAdpApp.m_dwMpcSSrc = tRsp.GetMSSsrc();
		}
		else
		{
			// �쳣������Ͽ������ڵ�
			if ( g_cMtAdpApp.m_dwMpcSSrc != tRsp.GetMSSsrc() )
			{
				StaticLog( "[MsgRegAckProc] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
					      g_cMtAdpApp.m_dwMpcSSrc, tRsp.GetMSSsrc());
				if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNode) )
				{
					OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNode);
				}
				if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNodeB) )
				{
					OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNodeB);
				}      
				return;
			}
		}

       if( '\0' == g_cMtAdpApp.m_achMcuAlias[0] )
       {
           sprintf( g_cMtAdpApp.m_achMcuAlias, "%s %u", "KDC MCU Ver4.0R4", g_cMtAdpApp.m_byDriId );
       }

	   g_cMtAdpApp.m_wMcuNetId      = tRsp.GetMcuNetId();
	   g_cMtAdpApp.m_wH225H245Port  = tRsp.GetH225H245Port();
	   g_cMtAdpApp.m_wH225H245MtNum = tRsp.GetH225H245MtNum();
	   g_cMtAdpApp.m_bMasterMtAdp   = tRsp.IsMtAdpMaster();
       g_cMtAdpApp.m_byCasAliasType = tRsp.GetCasAliasType();   // guzh [4/30/2007] ������ʾ��ʽ
       g_cMtAdpApp.m_byCascadeLevel = tRsp.GetAdminLevel();	   
       g_cMtAdpApp.m_bUseCallingMatch = tRsp.GetUseCallingMatch() == 1 ? TRUE : FALSE;

	   if( 0 == g_cMtAdpApp.m_wH225H245Port )
	   {
		   g_cMtAdpApp.m_wH225H245Port = MCU_H225245_STARTPORT;
	   }
	   //����ն���Ϊ0�������˿�����
	   if( g_cMtAdpApp.m_wH225H245MtNum > MAXNUM_H225H245_MT )
	   {
		   g_cMtAdpApp.m_wH225H245MtNum = MAXNUM_H225H245_MT;
	   }

	   if ( g_cMtAdpApp.m_wMcuNetId == 0 || g_cMtAdpApp.m_wMcuNetId > LOCAL_MCUID )
	   {
		   g_cMtAdpApp.m_wMcuNetId = LOCAL_MCUID;
	   }

	   if ( tRsp.GetHeartBeatInterval() > MIN_DISCCHECKTIME ) //��С5��
	   {
		   g_cMtAdpApp.m_wMaxRTDInterval = tRsp.GetHeartBeatInterval();
	   }
	   else
	   {
			g_cMtAdpApp.m_wMaxRTDInterval = MIN_DISCCHECKTIME;
	   }

	   if( tRsp.GetHeartBeatFailTimes() > MIN_DISCCHECKTIMES ) //��Сһ��
	   {
		   g_cMtAdpApp.m_byMaxRTDFailTimes = tRsp.GetHeartBeatFailTimes();
	   }
	   else
	   {
		   g_cMtAdpApp.m_byMaxRTDFailTimes = 3;
	   }

       g_cMtAdpApp.m_bIsGKCharge  = tRsp.GetIsGKCharge();
	   g_cMtAdpApp.m_tGKAddr.ip   = htonl(tRsp.GetGkIp());
	   g_cMtAdpApp.m_tGKAddr.port = MCU_RAS_PORT;
	   g_cMtAdpApp.m_tGKAddr.type = cmTransportTypeIP;
	   g_cMtAdpApp.m_tGKAddr.distribution = cmDistributionUnicast;

	   g_cMtAdpApp.m_byIsGkUseRRQPwd = tRsp.GetGkRRQUsePwdFlag();
	   memcpy(g_cMtAdpApp.m_achRRQPassword,tRsp.GetGkRRQPassword(),sizeof(g_cMtAdpApp.m_achRRQPassword));
	   
	   if(g_cMtAdpApp.GetFakedProductId() != NULL)
	   {
			g_cMtAdpApp.m_tH323Config.SetProductId( (u8*)g_cMtAdpApp.GetFakedProductId(), strlen(g_cMtAdpApp.GetFakedProductId())+1 );
	   }
	   //���ò�ƷID, zgc, 2007/04/03
	   else if( MCU_TYPE_KDV8000 == tRsp.GetMcuType() 
		   || MCU_TYPE_KDV8000B == tRsp.GetMcuType()
		   || MCU_TYPE_WIN32 == tRsp.GetMcuType() 
		   || MCU_TYPE_KDV8000E == tRsp.GetMcuType()
		   || MCU_TYPE_KDV8000H == tRsp.GetMcuType()
		   || MCU_TYPE_KDV800L == tRsp.GetMcuType()
		   || MCU_TYPE_KDV8000H_M == tRsp.GetMcuType()
		   || MCU_TYPE_KDV8000I == tRsp.GetMcuType()
		   )
	   {
			g_cMtAdpApp.m_tH323Config.SetProductId( (u8*)PRODUCT_MCU_STANDARD, strlen(PRODUCT_MCU_STANDARD)+1 );
	   }
	   else if( MCU_TYPE_KDV8000C == tRsp.GetMcuType() )
	   {
			g_cMtAdpApp.m_tH323Config.SetProductId( (u8*)PRODUCT_MCU_8000C, strlen(PRODUCT_MCU_8000C)+1 );
	   }

       // guzh [9/3/2007] H323Config Version
       //g_cMtAdpApp.m_tH323Config.SetVersionId( (u8*)tRsp.GetMcuVersion(), strlen(tRsp.GetMcuVersion()) );
	   g_cMtAdpApp.m_tH323Config.SetVersionId(KDVVC_VERID, LEN_KDVVC_VERID+1);

       //g_cMtAdpApp.m_tH323Config.SetProductId( (u8*)"VSX 7000", sizeof("VSX 7000") );
       //g_cMtAdpApp.m_tH323Config.SetVersionId( (u8*)"Release 8.5 - 05Nov2006 01:22", sizeof("Release 8.5.2 - 05Nov2006 01:22"));

       u32 dwGkIp = ntohl(tRsp.GetGkIp());
	   if (bMcuA)
	   {
		   StaticLog("[MtAdp]: Registering to MCU.A succeeded and GK ip is: %u.%u.%u.%u!\n", QUADADDR(dwGkIp) );
	   }
	   else
	   {
		   StaticLog("[MtAdp]: Registering to MCU.B succeeded and GK ip is:  %u.%u.%u.%u!\n", QUADADDR(dwGkIp) );
	   }
	   
	   // [11/4/2011 liuxu] ������Ϣ
	   if ( cServMsg.GetMsgBodyLen() >= (sizeof(TMtAdpRegRsp) + 5))
	   {
		   u8* pBuf = cServMsg.GetMsgBody();
		   g_cMtAdpApp.m_byEnableBufSendSMcuMtList = *(pBuf + sizeof(TMtAdpRegRsp));
		   u32 dwBufSendInterval = *(u32*)(pBuf + sizeof(TMtAdpRegRsp) + sizeof(u8));
		   g_cMtAdpApp.m_dwBufSendSMcuMtListInterval = ntohl(dwBufSendInterval);
		   MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "EnableBufSendSMcuMtList is %u, and BufSendInterval is %u\n", 
			   g_cMtAdpApp.m_byEnableBufSendSMcuMtList,
			   g_cMtAdpApp.m_dwBufSendSMcuMtListInterval);

           //[5/14/2013 liaokang] 
           if( bIsFirstRegRsp && ( cServMsg.GetMsgBodyLen() > (sizeof(TMtAdpRegRsp) + 5) ) )
           {
               u8 byMcuEncoding = *(u8*)(pBuf + sizeof(TMtAdpRegRsp) + sizeof(u8) + sizeof(u32));
               g_cMtAdpApp.SetMcuEncoding((emenCodingForm)byMcuEncoding);
               MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "mcu coding form is %d<1:utf8,2:gbk>!", byMcuEncoding);
           }
	   }
	}

    // ��GK���������ڼƷ�[11/09/2006-zbq]
    // ��Ҫ�����Ƿ��Ѿ�ע�����������ܵ������������¼Ʒѻ��鱻����ر�, zgc, 2008-09-26
    if ( g_cMtAdpApp.m_bIsGKCharge && 
         0 != g_cMtAdpApp.m_tGKAddr.ip && 
         g_cMtAdpApp.m_bMasterMtAdp &&
         !g_cMtAdpApp.m_bGKReged &&
         // zgc, 2008-09-27, �ǵ�һ���յ�ע��ɹ���Ӧ������ȥ����GK, �����������Ʒѻ����ظ��������⣬
         // ��󱻹رյ�����
         bIsFirstRegRsp)  
    {
        OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), MTADP_GK_CONNECT, NULL, 0 );
    }
    
	if(H323Init())
	{
		//g_cMtAdpApp.m_bMtAdpInited = TRUE;
		SetTimer( TIMER_H323_POLL_EVENT, H323_POLL_INTERVAL );
		SetTimer( TIMER_POLL_WATCHDOG, H323_POLL_INTERVAL*100 );
	}
	else
	{
        // zbq [07/04/2007] ���ڸ���·�ĳ�ʼ��ʧ�ܣ��Ͽ�����·
        if ( !g_cMtAdpApp.m_bAttachedToVc )
        {
            if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNodeB);
            }            
        }
        g_cMtAdpApp.m_tMaStaticInfo.m_bH323InitFailed = TRUE;
		StaticLog( "[MtAdp]: H323 initialize failed!\n") ;
	}

	return;
}

/*=============================================================================
    �� �� ���� DaemonProcMcuDisconnect
    ��    �ܣ� ��������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const  pcMsg
               CApp* pcApp
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2003/11     1.0			TanGuang              ����
    2005/9/10   4.0			����                ����˫�㽨��֧��	
=============================================================================*/
void CMtAdpInst::DaemonProcMcuDisconnect( CMessage * const  pcMsg, CApp* pcApp )
{
    u32 dwNodeDisconnet = *((u32*)pcMsg->content);
    if ( INVALID_NODE != dwNodeDisconnet ) 
    {
        OspDisconnectTcpNode(dwNodeDisconnet);
    }

    if (dwNodeDisconnet == g_cMtAdpApp.m_dwGKNode)
    {
        StaticLog( "[DaemonProcMcuDisconnect] GK Node disconnected.\n");
        
        DaemonDisconnectFromGK();
        OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), MTADP_GK_CONNECT, NULL, 0 );
        return;
    }

	if (g_cMtAdpApp.m_bAttachedToVc)
	{
		StaticLog( "[MtAdp]: attach vc mode is not disconnected!\n") ;
		return;
	}

	if (dwNodeDisconnet == g_cMtAdpApp.m_dwMcuNode)
	{
        StaticLog( "[DaemonProcMcuDisconnect] McuNode.A disconnected.\n");

		g_cMtAdpApp.m_dwMcuNode = INVALID_NODE;
		g_cMtAdpApp.m_dwMcuIId  = 0;
		OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), MTADP_MCU_CONNECT, NULL, 0 );                
	}
	else if (dwNodeDisconnet == g_cMtAdpApp.m_dwMcuNodeB)// && g_cMtAdpApp.m_bDoubleLink)
	{
        StaticLog( "[DaemonProcMcuDisconnect] McuNode.B disconnected.\n");

		g_cMtAdpApp.m_dwMcuNodeB = INVALID_NODE;
		g_cMtAdpApp.m_dwMcuIIdB  = 0;
		OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), MTADP_MCU_CONNECT2, NULL, 0 );
	}
		
	//������mcu�Ľڵ����Ч�����������ģ��������ն˽������Ӽ���ӦRAS��¼�����½���
	//���򱣳ָ������¼ֻ����Ӧ�Ͽ���mcu�ڵ�����½�������
	if (INVALID_NODE != g_cMtAdpApp.m_dwMcuNode || 
		INVALID_NODE != g_cMtAdpApp.m_dwMcuNodeB)
	{
        if (OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNode))
        {
            post( MAKEIID( AID_MCU_MTADPSSN, g_cMtAdpApp.m_byDriId ), 
                  MTADP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMtAdpApp.m_dwMcuNode );     
            
            StaticLog( "[DaemonProcMcuDisconnect] GetMsStatusReq. McuNode.A\n");
        }
        else if (OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNodeB))
        {
            post( MAKEIID( AID_MCU_MTADPSSN, g_cMtAdpApp.m_byDriId ), 
                  MTADP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMtAdpApp.m_dwMcuNodeB );        

            StaticLog( "[DaemonProcMcuDisconnect] GetMsStatusReq. McuNode.B\n");
        }

        SetTimer(TIMER_GETMSSTATUS_REQ, WAITING_MSSTATUS_TIMEOUT);
        return;
	}
	
	//clear all instances, preparing for a new registration
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[DaemonProcMcuDisconnect]. Clear All Inst.\n");
	for( s32 nDriMtNum = 1; nDriMtNum <= MAXNUM_DRI_MT; nDriMtNum ++ )
	{
		CInstance *pcInst = pcApp->GetInstance(nDriMtNum);
		
		if( pcInst )
			((CMtAdpInst*)pcInst)->ClearInst();
	}	
	//should unregister all the alias on GK if under RAS mode
	if( g_cMtAdpApp.m_bGkAddrSet && g_cMtAdpApp.m_tH323Config.IsManualRAS() && 
		TRUE == g_cMtAdpApp.m_bMasterMtAdp )
	{
        // BUG7096: MCU�Ļ���״̬��GK�Ʒ�ϵͳ�Ļ���״̬��һ��, zgc, 2008-09-26
        // ֪ͨGKֹͣ������صĻ���Ʒ�        
//        u8  abySsnId[LEN_RADIUS_SESSION_ID] = { 0 };
//        u16 wLenOut = 0;        
//        TConfAcctStopReq tStopReq;
        u8 byConfIdx = 0;
        for ( byConfIdx = 1; byConfIdx <= MAX_CONFIDX; byConfIdx++ )
        {
            TConfChargeInfo *ptInfo = &g_cMtAdpApp.m_atChargeInfo[byConfIdx-1];
            TAcctSessionId *ptChargeSsnId = &g_cMtAdpApp.m_atChargeSsnId[byConfIdx-1];
            
            u8  abySsnId[LEN_RADIUS_SESSION_ID] = { 0 };
            u8  bySsnIdLen = sizeof(abySsnId);
            u16 wLenOut = 0;
            if ( ptChargeSsnId->IsSessionIdNull() )
            {
                MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[DaemonProcMcuDisconnect] unexpected situation: SsnId is NULL, confIdx.%d \n", byConfIdx);
            }
            //[20110729 zhushz]pc-lint err:545
            ptChargeSsnId->GetAcctSessionId( (u8*)abySsnId, bySsnIdLen, wLenOut );
            
            if ( !ptInfo->IsNull() )
            {
                TConfAcctStopReq tStopReq;
                tStopReq.SetSeqNum( byConfIdx );
                tStopReq.SetSessionId( abySsnId, wLenOut );
                tStopReq.SetAcctNum( (u8*)ptInfo->GetGKUsrName(), strlen(ptInfo->GetGKUsrName()) );
                tStopReq.SetAcctPwd( (u8*)ptInfo->GetGKPwd(), strlen(ptInfo->GetGKPwd()) );
                
                SendMsgToGK( EV_MG_CONF_ACCTING_STOP, (u8*)&tStopReq, sizeof(tStopReq) );
                
                // ����������Ϣ
                ClearChargeData( byConfIdx );
            }
        }        


		KillTimer( TIMER_REGGK_REQ );

		CServMsg cURQMsg;
		cURQMsg.SetEventId( MCU_MT_UNREGISTERGK_REQ );
		cURQMsg.SetConfIdx( MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 ) ;

		OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), 
				 MCU_MT_UNREGISTERGK_REQ, 
				 cURQMsg.GetServMsg(), cURQMsg.GetServMsgLen() );

        //zbq[03/13/2008] ��ʶ����URQ�Ķ���
        g_cMtAdpApp.m_bWaitingURQRsp = TRUE;

        SetTimer( TIMER_URQ_RSP, URQ_RSP_TIMEOUT * 1000 );
        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcDisconnected] normal set TIMER_URQ_RSP \n" );

		//should issue MTADP_MCU_CONNECT in the URQ callback
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[DaemonProcMcuDisconnect]. Unregister all the alias on GK.\n");

        //zbq[01/07/2007] �Ͽ���MCU����·
        if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNode))
        {
            OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNode);
        }
        if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNodeB))
        {
            OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNodeB);
        }
	}
/*	else
	{
		g_cMtAdpApp.ClearMtAdpData();
		OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT, NULL, 0 );
		if (g_cMtAdpApp.m_bDoubleLink )
		{
			OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT2, NULL, 0 );
		}
	}*/

    //zbq[01/07/2007] ���������ʱ��
    KillTimer(TIMER_GETMSSTATUS_REQ);

    // zbq [05/22/2007] Ӧ���Ѿ�����ɾ��ˣ��������
    g_cMtAdpApp.m_tCallQueue.ClearQueue();
    g_cMtAdpApp.m_tWaitQueue.ClearQueue();

	g_cMtAdpApp.m_dwMpcSSrc = 0;

	return;
}	

/*=============================================================================
�� �� ���� DaemonProcGetMsStatusRsp
��    �ܣ� �ȴ���ȡ����״̬��ʱ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/21  4.0			������                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcGetMsStatusRsp( CMessage * const pcMsg, CApp* pcApp )
{
    if(MCU_MTADP_GETMSSTATUS_ACK == pcMsg->event)
    {                
        CServMsg cServMsg(pcMsg->content, pcMsg->length);
        TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer(TIMER_GETMSSTATUS_REQ);
        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[DaemonProcGetMsStatusRsp]. receive msg MCU_MTADP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                  ptMsStatus->IsMsSwitchOK());

        if(ptMsStatus->IsMsSwitchOK())
        {
            return;
        }
    }

    //��ʱ��ack�л�������ͬ��ʧ�ܣ����������ʵ����Ϣ
    //clear all instances, preparing for a new registration
    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[DaemonProcGetMsStatusRsp] from.%d<%s>. Clear All Inst.\n", pcMsg->event, OspEventDesc(pcMsg->event) );
    for( s32 nDriMtNum = 1; nDriMtNum <= MAXNUM_DRI_MT; nDriMtNum ++ )
    {
        CInstance *pcInst = pcApp->GetInstance(nDriMtNum);
        
        if( pcInst )
            ((CMtAdpInst*)pcInst)->ClearInst();
    }	
    //should unregister all the alias on GK if under RAS mode
    if( g_cMtAdpApp.m_bGkAddrSet && g_cMtAdpApp.m_tH323Config.IsManualRAS() && 
        TRUE == g_cMtAdpApp.m_bMasterMtAdp )
    {
        KillTimer( TIMER_REGGK_REQ );
        
        CServMsg cURQMsg;
        cURQMsg.SetEventId( MCU_MT_UNREGISTERGK_REQ );
        cURQMsg.SetConfIdx( MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 ) ;
        
        OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), 
                 MCU_MT_UNREGISTERGK_REQ, 
                 cURQMsg.GetServMsg(), cURQMsg.GetServMsgLen() );
        
        //zbq[03/13/2008] ��ʶ����URQ�Ķ���
        g_cMtAdpApp.m_bWaitingURQRsp = TRUE;

        SetTimer( TIMER_URQ_RSP, URQ_RSP_TIMEOUT * 1000 );
        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[DaemonProcGetMsStatusRsp] normal set TIMER_URQ_RSP \n" );

        //should issue MTADP_MCU_CONNECT in the URQ callback
        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[DaemonProcGetMsStatusRsp]. Unregister all the alias on GK.\n");
        
        // �Ͽ���GK����·
        DaemonDisconnectFromGK();

        //zbq[01/07/2007] Ҳ�Ͽ���MCU����·
        if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNode))
        {
            OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNode);
        }
        if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNodeB))
        {
            OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNodeB);
        }
    }  
    else
    {
        //zbq[01/07/2007] �Ͽ���MCU����·
        if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNode))
        {
            OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNode);
        }
        if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNodeB))
        {
            OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNodeB);
        }

        g_cMtAdpApp.ClearMtAdpData();
        OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT, NULL, 0 );
        if (g_cMtAdpApp.m_bDoubleLink )
        {
            OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT2, NULL, 0 );
        }
    }

    // zbq [05/22/2007] Ӧ���Ѿ�����ɾ��ˣ��������
    g_cMtAdpApp.m_tCallQueue.ClearQueue();
    g_cMtAdpApp.m_tWaitQueue.ClearQueue();

    return;
}

/*=============================================================================
  �� �� ���� DaemonProcMcuMtQosSetCmd
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/10/27    4.0			�ű���                ����
=============================================================================*/
void CMtAdpInst::DaemonProcMcuMtQosSetCmd( CMessage * const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	if( !BODY_LEN_GE(cServMsg, sizeof(TMcuQosCfgInfo)) ) 
		return;
	
	TMcuQosCfgInfo tMcuQosCfg = *(TMcuQosCfgInfo*)(cServMsg.GetMsgBody());
		
	u8 byTos = 0;    //Ҫ����ȥ���ֶ�
	u8 byIpSerType;  //�ϲ㴫������IP��������
	
	//���ַ���
	if( QOSTYPE_DIFFERENCE_SERVICE == tMcuQosCfg.GetQosType() )
	{
		//���������ֵ�DATA���ȼ�
		if( tMcuQosCfg.GetDataLevel() > QOS_DIFF_MAXLEVEL )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[Qosset] wrong diffrence service level recieved, ignore it !\n" );
			return;
		}
		//ȡ��Ĭ��ֵ��һ��ķ���		
		else if( 0 == tMcuQosCfg.GetDataLevel() )   
		{
			kdvSetDataTOS( (s32)byTos );
		}
		//ȡ������ֵ		
		else
		{
			kdvSetDataTOS( (s32)tMcuQosCfg.GetDataLevel() );
		}
		
		//���������ֵ��������ȼ�
		if( tMcuQosCfg.GetSignalLevel() > QOS_DIFF_MAXLEVEL )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[Qosset] wrong diffrence service level received, ignore it !\n");
			return;
		}
		//ȡ��Ĭ��ֵ��һ��ķ���		
		else if( 0 == tMcuQosCfg.GetSignalLevel() )  
		{
			kdvSetH323TOS( (s32)byTos );
		}
		//ȡ������ֵ
		else                                         
		{
			kdvSetH323TOS( (s32)tMcuQosCfg.GetSignalLevel() );
		}

		//��ӡ��ǰ��QOS����
		tMcuQosCfg.Print();
	}
	
	//IP ����
	else if( QOSTYPE_IP_PRIORITY == tMcuQosCfg.GetQosType() )
	{
		//����IP���ȵ�DATA�����ȼ�
		if( tMcuQosCfg.GetDataLevel() > QOS_IP_MAXLEVEL )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[Qosset] wrong ip data service level recieved, ignore it !\n");
			return;
		}
		else
		{
			//���������ȼ���, ����byTos��ǰ��λ
			byTos = (tMcuQosCfg.GetDataLevel() << 5);	
			
			//���� byTos�ĺ�ĳλȷ������ķ�������, ���һλ��Ԥ��
			byIpSerType = tMcuQosCfg.GetIpServiceType();
			if( 0 == byIpSerType )
			{
				//һ����񣬺���λȫΪ0 (ȫ��ѡ)
				kdvSetDataTOS( (s32)byTos );  
			}
			else
			{
				if ( 0 != (byIpSerType & IPSERVICETYPE_LOW_DELAY) )
				{
					byTos |= 0x10;  //��С��ʱ������λΪ1(Ĭ��ֵ)
				}
				if ( 0 != (byIpSerType & IPSERVICETYPE_HIGH_THROUGHPUT) )
				{
					byTos |= 0x08;  //���������������λΪ1
				}
				if ( 0 != (byIpSerType & IPSERVICETYPE_HIGH_RELIABILITY) )
				{
					byTos |= 0x04;  //��߿ɿ��ԣ�����λΪ1
				}
				if( 0 != (byIpSerType & IPSERVICETYPE_LOW_EXPENSE) )
				{
					byTos = 0x02;   //��ͷ��ã�����λΪ1
				}
			}

		}
		kdvSetDataTOS( (s32)byTos );
		
		//����IP���ȵ��������ȼ�
		if( tMcuQosCfg.GetSignalLevel() > QOS_IP_MAXLEVEL )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[Qosset] wrong ip signal service level received, ignore it !\n");
			return;
		}
		else
		{
			//�������ȼ���, ����nTos�ĸ���λ
			byTos = tMcuQosCfg.GetSignalLevel() << 5; 

			//���� byTos�ĺ�ĳλȷ������ķ�������, ���һλ��Ԥ��
			byIpSerType = tMcuQosCfg.GetIpServiceType();
			if( 0 == byIpSerType )
			{
				//һ����񣬺���λȫΪ0 (ȫ��ѡ)
				kdvSetH323TOS( (s32)byTos ); 
			}
			else  
			{
				if ( 0 != (byIpSerType & IPSERVICETYPE_LOW_DELAY) )
				{
					byTos |= 0x10;  //��С��ʱ������λΪ1(Ĭ��ֵ)
				}

				if ( 0 != (byIpSerType & IPSERVICETYPE_HIGH_THROUGHPUT) )
				{
					byTos |= 0x08;  //���������������λΪ1
				}

				if ( 0 != (byIpSerType & IPSERVICETYPE_HIGH_RELIABILITY) )
				{
					byTos |= 0x04;  //��߿ɿ��ԣ�����λΪ1
				}
				
				if( 0 != (byIpSerType & IPSERVICETYPE_LOW_EXPENSE) )
				{
					byTos = 0x02;   //��ͷ��ã�����λΪ1
				}
			}
			
		}
		kdvSetH323TOS( (s32)byTos );

		//��ӡ��ǰ��QOS����
        if (  g_cMtAdpApp.m_byDebugLevel >= DEBUG_INFO )
        {
            tMcuQosCfg.Print();
        }		
	}
	else
		StaticLog( "[Qosset]--> Unexpected qos type<%d> recived !\n", 
                                                         tMcuQosCfg.GetQosType());
	return;
}

/*=============================================================================
  �� �� ���� DaemonProcMtAdpCfgCmd
  ��    �ܣ� ����MCU����������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/11/29    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcMtAdpCfgCmd( CMessage * const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
			
	if(!BODY_LEN_GE(cServMsg, sizeof(TMtAdpCfg))) 
		return;
			
	TMtAdpCfg tMtAdpCfg = *(TMtAdpCfg*)cServMsg.GetMsgBody();

	//��Ƶ��Ϣ����������
	if( tMtAdpCfg.GetAudInfoRefreshTime() <= CASCADEINFO_MAX_REFRESHTIME &&
		tMtAdpCfg.GetAudInfoRefreshTime() >= CASCADEINFO_MIN_REFRESHTIME)
	{
		g_dwAudInfoTimeout = tMtAdpCfg.GetAudInfoRefreshTime() * 1000;
	}	

	//��Ƶ��Ϣ����������
	if ( tMtAdpCfg.GetVidInfoRefreshTime() <= CASCADEINFO_MAX_REFRESHTIME &&
		 tMtAdpCfg.GetVidInfoRefreshTime() >= CASCADEINFO_MIN_REFRESHTIME )
	{	
		g_dwVidInfoTimeout = tMtAdpCfg.GetVidInfoRefreshTime() * 1000;
	}

	//�ն��б���Ϣ����ʱ��������
	if ( tMtAdpCfg.GetPartListRefreshTime() <= CASCADEINFO_MAX_REFRESHTIME &&
		 tMtAdpCfg.GetPartListRefreshTime() >= CASCADEINFO_MIN_REFRESHTIME )
	{
		g_dwPartlistInfoTimeout = tMtAdpCfg.GetPartListRefreshTime() * 1000;
	}	
	
    if (  g_cMtAdpApp.m_byDebugLevel >= DEBUG_INFO )
    {
        StaticLog( "[MtApdCfg] CurTimeoutInfo: Partlist(0).%d Aud(1).%d Vid(2).%d\n",     	
	               g_dwPartlistInfoTimeout, g_dwAudInfoTimeout, g_dwVidInfoTimeout );
    }

    return;
}

#if !defined(WIN32) && !defined(CALLBACK)
#define CALLBACK
#endif

//a new call comes
static s32 CALLBACK EvNewCall( HCALL hsCall, LPHAPPCALL lphaCall );

//a new channel comes
static s32 CALLBACK EvNewChan( HAPPCALL haCall, HCALL hsCall, HCHAN hsChan, LPHAPPCHAN lphaChan );

//call control messages comes
static s32 CALLBACK EvCallCtrlMsg( HAPPCALL haCall, HCALL hsCall, u16 wMsgType,
                            const void* pBuf, u16 wBufLen );
//channel control message comes
static s32 CALLBACK EvChanCtrlMsg( HAPPCALL haCall, HCALL hsCall, HAPPCHAN haChan, HCHAN hsChan,
                            u16 wMsgType, const void* pBuf, u16 wBufLen );
//conference control message comes
static s32 CALLBACK EvConfCtrlMsg( HAPPCALL haCall, HCALL hsCall, u16 wMsgType, const void* pBuf, u16 wBufLen );

//RAS control message comes
static s32 CALLBACK EvRasCtrlMsg( HAPPCALL haCall, HCALL hsCall, HAPPRAS haRas, HRAS hsRas,
                           u16 wMsgType, const void* pBuf, u16 wBufLen );
//H.281 packets comes
static s32	CALLBACK EvFeccMsg( HCALL hsCall, HCHAN hsChan, TFeccStruct tFeccStruct, 
                       TTERLABEL tTerSrc, TTERLABEL tTerDst );
//MMcu ���鼶��������Ϣ�ص�����
static s32 CALLBACK EvMMcuMsg( HAPPCALL haCall, HCALL hsCall, HAPPCHAN haChan, HCHAN hsChan,
                        u16 wMsgType, const void* pBuf, u16 wBufLen );


static H323EVENT tHevent = {	EvNewCall,
								EvNewChan,
								EvCallCtrlMsg,
								EvChanCtrlMsg,
								EvConfCtrlMsg, 
								EvRasCtrlMsg,
								EvFeccMsg,
								EvMMcuMsg 
							};

/*=============================================================================
  �� �� ���� H323Init
  ��    �ܣ� ��������ʼ��h.323Э��ջ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
BOOL32 CMtAdpInst::H323Init()
{
	s32	nInitRet; 
    sprintf(g_achStackCfgFile, "%s/mcucfg.val", DIR_CONFIG);

	//Э��ջ��֧�ַ�����ʼ����Ǳ�����⣬�����½���ʱ��mcu�����ѱ仯��
	//������Э��ջδ�ṩ��Ӧ���ýӿڣ������ڲ�ͬ������ 2005-11-04
	if( NULL == g_cMtAdpApp.m_hApp )
	{
		if( g_cMtAdpApp.m_tGKAddr.ip != 0 )
		{
			g_cMtAdpApp.m_tH323Config.SetDefGKIP( g_cMtAdpApp.m_tGKAddr.ip );
		}
		
		g_cMtAdpApp.m_tH323Config.SetH225CallingPort( g_cMtAdpApp.m_wQ931Port );
		g_cMtAdpApp.m_tH323Config.SetLocalRASPort( g_cMtAdpApp.m_wRasPort );
		
		//����ն���Ϊ0�������˿����ƣ�����������������
		if( 0 != g_cMtAdpApp.m_wH225H245MtNum )
		{
			g_cMtAdpApp.m_tH323Config.SetPortRange( g_cMtAdpApp.m_wH225H245Port, 
			                                    	( g_cMtAdpApp.m_wH225H245Port + g_cMtAdpApp.m_wH225H245MtNum * 2 ) );

			g_cMtAdpApp.m_tH323Config.SetMaxNum( g_cMtAdpApp.m_wH225H245MtNum, MIN_CHANNEL_NUM );
		}
		
		void* phApp = NULL;

        
        
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		u32 dwIp_N_4H323 = g_cMcuAgent.GetMpcIp();
		g_cMtAdpApp.m_tH323Config.SetStackIP(dwIp_N_4H323); //need net sequence
		
// 		TMcu8KECfg tMcu8KECfg;
// 		g_cMcuAgent.GetMcuEqpCfg(&tMcu8KECfg);
// 		// [3/27/2010 xliang] FIXME: ѡȡactive�����ڵ�0��ip
// 		TNetAdaptInfo * ptNetAdapt = tMcu8KECfg.m_tLinkNetAdap.GetValue(0);
// 		
// 		if( ptNetAdapt != NULL )
// 		{
// 			TNetParam *ptNetParam = ptNetAdapt->m_tLinkIpAddr.GetValue(0);			//host ip, mask
// 			if( ptNetParam != NULL )
// 			{
// 				dwIp_N_4H323 = htonl(ptNetParam->GetIpAddr());
// 			}
// 		}
// 		g_cMtAdpApp.m_tH323Config.SetStackIP( dwIp_N_4H323 ); //need net sequence

#endif		
        phApp = kdvInitH323Adapter(g_achStackCfgFile, &g_cMtAdpApp.m_tH323Config);
        
        // zbq [07/04/2007]
        g_cMtAdpApp.m_bMtAdpInited = TRUE;
		
        if( phApp == NULL )
		{
			StaticLog( "[MtAdp] H323 stack initialize failed! \n");
			return FALSE;
		}
		else
		{
			g_cMtAdpApp.m_hApp = *(HAPP*)phApp;
			StaticLog( "[MtAdp] H323 stack initialize succeeded!\n");
		}
		
		nInitRet = kdvSetAppCallBack( &tHevent );	
		if (nInitRet < 0)
		{
			StaticLog( "[MtAdp] Setting callbacks failed\n");
			return FALSE;
		}    
		
		seliSetMultiThreads(FALSE);

        if( emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() )
        {
            //[4/8/2013 liaokang] ��UTF-8��ʽ����
            kdvSetLocalIsUTF8(TRUE);
			g_cMtAdpApp.SetStackInitUtf8(TRUE);
            StaticLog( "[MtAdp] kdv set local is UTF8!\n");
        }        
        else
		{
			StaticLog( "[MtAdp] kdv set local is GBK!\n");
			g_cMtAdpApp.SetStackInitUtf8(FALSE);
		}
	}
	else
	{
		StaticLog( "[MtAdp]: H323 initialized before!\n") ;
	}

	//��GK��ַд��Э��ջ	
	if (!SetGKRasAddress( g_cMtAdpApp.m_tGKAddr))   
	{		
		return FALSE;
	}			
	
	return TRUE;
}

/*=============================================================================
  �� �� ���� SetGKRasAddress
  ��    �ܣ� ����gk��ַ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CServMsg * pcServMsg
             u16 dstInst
  �� �� ֵ�� s32  
=============================================================================*/
BOOL32 CMtAdpInst::SetGKRasAddress(cmTransportAddress	&tGKAddr)
{
    if(0 != memcmp(&g_cMtAdpApp.m_tGKAddrOld, &tGKAddr, sizeof(cmTransportAddress)))
    {
        if ((s32)act_ok == kdvSetGKRASAddress(g_cMtAdpApp.m_hApp, tGKAddr))
        {
            g_cMtAdpApp.m_bGkAddrSet = (0 != g_cMtAdpApp.m_tGKAddr.ip) ? TRUE : FALSE;            
            memcpy(&g_cMtAdpApp.m_tGKAddrOld, &tGKAddr, sizeof(cmTransportAddress));
            
            MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[SetGKRasAddress] kdvSetGKRASAddress g_hApp.0x%x, GkAddr(%u.%u.%u.%u:%d), GKNetAddrType.%d\n", 
                g_cMtAdpApp.m_hApp, QUADADDR(g_cMtAdpApp.m_tGKAddr.ip), g_cMtAdpApp.m_tGKAddr.port, g_cMtAdpApp.m_tGKAddr.type );
        }
        else
        {
            MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[SetGKRasAddress]: return failure in SetGKRasAddress(), g_hApp.0x%x, GkAddr(%u.%u.%u.%u:%u)!\n",
                g_cMtAdpApp.m_hApp, QUADADDR(g_cMtAdpApp.m_tGKAddr.ip), g_cMtAdpApp.m_tGKAddr.port );
            return FALSE;
        }		
    }    
    else
    {
        g_cMtAdpApp.m_bGkAddrSet = (0 != g_cMtAdpApp.m_tGKAddr.ip) ? TRUE : FALSE;            
    }
    
    return TRUE;
}
/*=============================================================================
  �� �� ���� IsR2WJMCU
  ��    �ܣ� �Ƿ�ΪR2WJ MCU
  �㷨ʵ�֣� R2Ŀǰ��Ҫ������WJ GDZF�������ļ����ն�״̬�ϱ��ṹ���ڲ��죬��Ҫ���ִ���
             �������߰汾��Ϣһ�£�Ŀǰ����WJ���������������������Ϣ��������
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32 CMtAdpInst::IsR2WJMCU()
{
	if( strcmp(GetPeerVersionID(m_hsCall), "4.0") == 0
		&& (strstr(m_tIncomingCallParams.GetDisplayInfo(), ".һ��") != NULL ||
			strstr(m_tIncomingCallParams.GetDisplayInfo(), ".����") != NULL ||
			strstr(m_tIncomingCallParams.GetDisplayInfo(), ".����") != NULL ||
			strstr(m_tIncomingCallParams.GetDisplayInfo(), ".�ļ�") != NULL ) )
	{
		return TRUE;
	}
	return FALSE;
}
	
/*=============================================================================
  �� �� ���� IsMtHasVidCap
  ��    �ܣ� Զ���Ƿ�����Ƶ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32 CMtAdpInst::IsMtHasVidCap()
{
	//Զ�����������Ƿ�����Ƶother����
	u8 abyVidOtherCap[5] = { v_h261, v_h262, v_h263, v_h263plus, v_mpeg4};
	for (u8 byIdx = 0; byIdx < sizeof(abyVidOtherCap); byIdx++)
	{
		TVideoCap *ptVidCap = m_ptRemoteCapSet->GetVideoCap(abyVidOtherCap[byIdx]);
		if (NULL != ptVidCap)
		{
			if(0 != ptVidCap->GetBitRate())
			{
				return TRUE;
			}
		}
	}

	//Զ���������Ƿ���h264����
	if (0 != m_ptRemoteCapSet->GetH264Num())
	{
		return TRUE;
	}

	return FALSE;
}
	
/************************************************************************/
/*                                                                      */
/*                            MtAdpData                                 */
/*                                                                      */
/************************************************************************/

CMtAdpData::CMtAdpData()			
{	
    memset( m_Msg2InstMap, 0, sizeof(m_Msg2InstMap) );
    m_dwMtAdpIpAddr     = 0;
    m_wMaxRTDInterval  = 10;
    m_byMaxRTDFailTimes = 5;
    m_byDebugLevel  = 0;
    m_bGkAddrSet    = FALSE;
    m_bGkRegistered = FALSE;
    m_bGotRRJOrURQFromGK = FALSE;
    m_bAlreadyNtfGKEPID  = FALSE;
    m_bWaitingURQRsp     = FALSE;
    m_bMasterMtAdp = FALSE;
    m_bDoubleLink  = FALSE;
    m_dwMcuNode    = INVALID_NODE;
    m_dwMcuNodeB   = INVALID_NODE;
    m_dwMcuIId     = 0;
    m_dwMcuIIdB    = 0;
    m_dwMcuIpAddr  = 0;
    m_dwMcuIpAddrB = 0;
    m_wMcuPort     = 0;
    m_wMcuPortB    = 0;
    
    m_bURQForNPlus = 0;
    m_bIsGKCharge  = FALSE;
    m_dwGKNode     = INVALID_NODE;
    m_dwGKIId      = 0;
    m_bGKReged     = FALSE;
	m_dwMpcSSrc	   = 0;
    m_byGetStackResFailedTimes = 0;
    //[20110729 zhushz]pc-lint err:545
    memset( m_abyRestart, 0, sizeof(m_abyRestart) );
    memset( m_atChargeInfo,  0, sizeof(m_atChargeInfo) );
    memset( m_atChargeSsnId, 0, sizeof(m_atChargeSsnId) );
    
    memset( (void*)&m_tGKID,  0, sizeof(m_tGKID ) );
    memset( (void*)&m_tEPID, 0, sizeof(m_tEPID) );
    memset( &m_tGKAddr, 0, sizeof(m_tGKAddr) );
    memset( &m_tGKAddrOld, 0, sizeof(m_tGKAddrOld) );
    
    m_bH323PollWatchdog = TRUE;
    m_bMtAdpInited = FALSE;
    m_hApp = NULL;
    
    for( s32 nMcuConfNum = 0; nMcuConfNum < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1; nMcuConfNum ++ )
    {
        m_hsRas[nMcuConfNum] = NULL;
        m_gkConfRegState[nMcuConfNum] = STATE_NULL;
    }
    m_gkRegState = STATE_NULL;
    memset( m_hsRas,      0, sizeof(m_hsRas) );
	m_hsCurrentRRQRas = NULL;
	m_byCurrentRRQConIdx = 0xFF;
    memset( m_atMcuAlias, 0, sizeof(m_atMcuAlias) );
    memset( m_atMtadpAddr, 0, sizeof(m_atMtadpAddr) );
    
    memset( m_abyRRQFifoQueue, 0xff, sizeof(m_abyRRQFifoQueue) );
    
    m_nReqHead = 0;
    m_nReqTail = 0;
    
    m_tH323Config.m_dwSystem_maxCalls    = MAXNUM_DRI_MT;
    m_tH323Config.m_dwSystem_maxChannels = MAXNUM_CALL_CHANNEL;
    
    m_tH323Config.SetProductId( (u8*)PRODUCT_MCU_STANDARD, strlen(PRODUCT_MCU_STANDARD) );
    //m_tH323Config.SetVersionId( (u8*)MCU_VERSION_ID, strlen(MCU_VERSION_ID) );
    m_tH323Config.SetVersionId( (u8*)KDVVC_VERID, strlen((s8 *)KDVVC_VERID) + 1 );

    m_tH323Config.m_emTerminalType = emEndpointTypeMCU;
    m_tH323Config.m_bRas_manualRAS = 1;
    m_tH323Config.m_bRas_manualRegistration = 1;
    m_tH323Config.m_bH245_capabilitiesManualOperation = 1;
    m_tH323Config.m_bH245_masterSlaveManualOperation = 1;	
    m_tH323Config.m_emMsTerminalType = /*emMsMCU*/emMsActiveMC/*240*/;
    
    m_byMaxCallNum = 0;
    m_bUseCallingMatch = FALSE;
    m_emMcuEncoding = emenCoding_GBK;
    m_emGkEncoding = emenCoding_GBK;
	m_byStackInitUtf8 = 0;
	m_wMaxNumConntMt = 0;
	m_wCurNumConntMt = 0;
	m_byMaxHDMtNum = 0;
	m_wMaxNumConnAudMt = 0;
	m_byLicenseErrCode = 0; //0��û��license error
	memset( m_abyConnMcuMtList, 0, sizeof(m_abyConnMcuMtList));

	m_bSupportSlice  = 0;	//not support slice by default
    m_byCascadeLevel = 4;
	
	m_byEnableBufSendSMcuMtList = 0;
	m_dwBufSendSMcuMtListInterval = 0;

    //[20110729 zhushz]pc-lint err:545
	memset( m_abyInvitedMsgInOspMt,0,sizeof(m_abyInvitedMsgInOspMt) );
	memset( m_abyNeglectInviteMsgMt,0,sizeof(m_abyNeglectInviteMsgMt) );
	
	m_dwRestrictManuTable = 0;

	memset(m_achFakedProductId,0,sizeof(m_achFakedProductId));
	memset(m_abyCRIMacAddr,0,sizeof(m_abyCRIMacAddr));
#ifdef _IS22_
	m_wDiscHeartBeatTime = 0;
	m_byDiscHeartBeatNum = 0;
#endif

	m_byIsGkUseRRQPwd = 0;
	memset(m_achRRQPassword,0,sizeof(m_achRRQPassword));
	m_tAllSMcuList.Init();
}
/*lint -save -e1551*/
CMtAdpData::~CMtAdpData()
{
    m_tCallQueue.Quit();
    m_tWaitQueue.Quit();
	m_tAllSMcuList.Init();
}
/*lint -restore*/

void CMtAdpData::ClearMtAdpData()
{
    memset( m_Msg2InstMap, 0, sizeof(m_Msg2InstMap) );
    m_wMaxRTDInterval  = 10;
    m_byMaxRTDFailTimes = 5;
    m_bGkAddrSet    = FALSE;
    m_bGkRegistered = FALSE;
    m_bGotRRJOrURQFromGK = FALSE;
    m_bAlreadyNtfGKEPID  = FALSE;
    m_bMasterMtAdp = FALSE;
    m_dwMcuNode    = INVALID_NODE;
    m_dwMcuNodeB   = INVALID_NODE;
    m_dwMcuIId     = 0;
    m_dwMcuIIdB    = 0;
    m_bURQForNPlus = 0;
    m_bIsGKCharge  = FALSE;
    m_dwGKNode     = INVALID_NODE;
    m_dwGKIId      = 0;
    m_bGKReged     = FALSE;
	m_dwMpcSSrc	   = 0;
  
    //[20110729 zhushz]pc-lint err:545
    memset( m_abyRestart, 0, sizeof(m_abyRestart) );
    memset( m_atChargeInfo,  0, sizeof(m_atChargeInfo) );
    memset( m_atChargeSsnId, 0, sizeof(m_atChargeSsnId) );
    
    memset( (void*)&m_tGKID, 0, sizeof(m_tGKID) );
    memset( (void*)&m_tEPID, 0, sizeof(m_tEPID) );
    memset( &m_tGKAddr, 0, sizeof(m_tGKAddr) );

    m_bH323PollWatchdog = TRUE;
    
    for( s32 nMcuConfNum = 0; nMcuConfNum < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1; nMcuConfNum ++ )
    {
        m_hsRas[nMcuConfNum] = NULL;
        m_gkConfRegState[nMcuConfNum] = STATE_NULL;
    }
    m_gkRegState = STATE_NULL;
    memset( m_hsRas, 0, sizeof(m_hsRas) );
	m_hsCurrentRRQRas = NULL;
	m_byCurrentRRQConIdx = 0xFF;
    memset( m_atMcuAlias, 0, sizeof(m_atMcuAlias) );
    memset( m_atMtadpAddr, 0, sizeof(m_atMtadpAddr) );
    
    memset( m_abyRRQFifoQueue, 0xff, sizeof(m_abyRRQFifoQueue) );
    m_nReqHead = 0;
    m_nReqTail = 0;

    //m_byMaxCallNum = 0;
    m_bUseCallingMatch = FALSE;
    m_emMcuEncoding = emenCoding_GBK;
    m_emGkEncoding = emenCoding_GBK;
	m_byStackInitUtf8 = 0;
// 
// 	m_wMaxNumConntMt = 0;
// 	m_wCurNumConntMt = 0;
// 	m_byLicenseErrCode = 0; //0��û��license error
//	memset( m_abyConnMcuMtList, 0, sizeof(m_abyConnMcuMtList));// xliang [1/5/2009] FIXME�����ﲻ����

	m_byIsGkUseRRQPwd = 0;
	memset(m_achRRQPassword,0,sizeof(m_achRRQPassword));
}

/*=============================================================================
  �� �� ���� ProcH323CallBack
  ��    �ܣ� h.323Э��ջ�����лص�����MtAdpApp�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CServMsg * pcServMsg
             u16 dstInst
  �� �� ֵ�� s32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
s32 CMtAdpData::ProcH323CallBack( const CServMsg * pcServMsg, u16 wDstInst )
{	
	if( !pcServMsg ) 
		return 0;

	CMessage cMsg;
	memset( &cMsg, 0, sizeof(CMessage) );	

	cMsg.event   = pcServMsg->GetEventId();
	cMsg.content = pcServMsg->GetServMsg();
	cMsg.length  = pcServMsg->GetServMsgLen();	
	
	CApp * pcApp = (CApp*)&g_cMtAdpApp;

	if( !pcApp ) 
		return 0;

	// xliang [11/18/2008] HDI����MT��Ŀ
	u16 wMaxHDIConntMt = g_cMtAdpApp.m_wMaxNumConntMt;
	if(wMaxHDIConntMt == 0)
	{
		StaticLog( "[ProcH323CallBack]HDI has no access permission!\n");
		return 0;
	}

	switch( wDstInst ) 
	{
	case ( CInstance::PENDING ):
		{
			CInstance *pcInst = NULL;
			
//			for( s32 nDriMtNum = 1; nDriMtNum <= wMaxHDIConntMt; nDriMtNum ++)
			for( s32 nDriMtNum = 1; nDriMtNum <= MAXNUM_DRI_MT; nDriMtNum ++ )
			{
				pcInst = pcApp->GetInstance(nDriMtNum);
				if( pcInst && pcInst->CurState() == CMtAdpInst::STATE_IDLE )
				{
					( (CMtAdpInst*)pcInst )->ProcRadEvNewCall(&cMsg);
					return nDriMtNum;
				}				
			}
		}
		break;

	case (CInstance::DAEMON):
		{
			CInstance *pcInst = pcApp->GetInstance( wDstInst );
			if(pcInst)	
				pcInst->DaemonInstanceEntry( &cMsg, pcApp );
		}
		break;

	default: //regular instance
		{
			CMtAdpInst *pcInst = (CMtAdpInst*)( pcApp->GetInstance(wDstInst) );
			if(!pcInst)	break;
			CMessage *pcMsg = &cMsg;

			switch(pcServMsg->GetEventId()) 
			{				
			case EV_RAD_NEWCHAN_NOTIF: // new channel event
				pcInst->ProcRadEvNewChannel( pcMsg );
				break;
				
			case EV_RAD_CALLCTRL_NOTIF: // call control event
				pcInst->ProcRadEvCallCtrl( pcMsg );
				break;
				
			case EV_RAD_CHANCTRL_NOTIF: // channel control event 
				pcInst->ProcRadEvChanCtrl( pcMsg );
				break;
				
			case EV_RAD_CONFCTRL_NOTIF: // conference control event
				pcInst->ProcRadEvConfCtrl( pcMsg );
				break;
				
				
			default:
				break;				
			}
		}
		break;
	}
	return 0;
}


/*=============================================================================
  �� �� ���� ProcEnqueueRRQ
  ��    �ܣ� ��RRQ��Ϣ�����ݴ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const u8 byConfIdx
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
 2003/11        1.0			TanGuang                  ����
=============================================================================*/
BOOL32 CMtAdpData::ProcEnqueueRRQ( const u8 byConfIdx )
{			
	if( m_abyRRQFifoQueue[m_nReqTail] != 0xff )
	{
		StaticLog( "CMtAdpData::ProcEnqueueRRQ: Buffer Full!\n" );
		return FALSE;
	}
	
	m_abyRRQFifoQueue[ m_nReqTail ] = byConfIdx;		
	m_nReqTail = ( m_nReqTail + 1 ) % ( MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 );

	return TRUE;
}


/*=============================================================================
  �� �� ���� ProcDequeueRRQ
  ��    �ܣ� �Ӷ�����ȡ����һ��RRQ��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 *pbyConfIdx
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
BOOL32 CMtAdpData::ProcDequeueRRQ( u8 *pbyConfIdx )
{
	if( pbyConfIdx == NULL )
		return FALSE;

	if( m_abyRRQFifoQueue[g_cMtAdpApp.m_nReqHead] == 0xff )
		return FALSE;
	
	*pbyConfIdx = m_abyRRQFifoQueue[m_nReqHead];
	m_abyRRQFifoQueue[m_nReqHead] = 0xff;
	m_nReqHead = ( m_nReqHead + 1 ) % ( MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 );

	return TRUE;
}

/*=============================================================================
  �� �� ���� IsConfInQueue
  ��    �ܣ� �Ӷ������Ƿ���ڸ�RRQ��Ϣ
  �㷨ʵ�֣� FIXME: �����㷨�Ż�һ��
  ȫ�ֱ����� 
  ��    ���� const u8 byConfIdx
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/11       4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMtAdpData::IsConfInQueue( const u8 byConfIdx )
{
    for( u8 nIndex = 0; nIndex < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1; nIndex++)
    {
        if ( m_abyRRQFifoQueue[nIndex] == byConfIdx )
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
  �� �� ���� GetCallDataFromDebugFile
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16& wRasPort
             u16& wQ931Port
  �� �� ֵ�� s32 
=============================================================================*/
BOOL32 CMtAdpData::GetCallDataFromDebugFile(u16& wRasPort, u16& wQ931Port, u8& byMaxCall)
{
    s8 achFName[KDV_MAX_PATH] = {0};
    sprintf(achFName, "%s/%s", DIR_CONFIG, "mtadpdebug.ini");
    
    s32 nRasPort = 0;
    ::GetRegKeyInt( achFName, "calldata", "rasport", MCU_RAS_PORT, &nRasPort);	
    wRasPort = (0 == nRasPort ? MCU_RAS_PORT : (u16)nRasPort);
    
    s32 nQ931Port = 0;
    ::GetRegKeyInt( achFName, "calldata", "q931port", MCU_Q931_PORT, &nQ931Port );
    wQ931Port = (0 == nQ931Port ? MCU_Q931_PORT : (u16)nQ931Port);

    s32 nMaxCall = 0;
    ::GetRegKeyInt( achFName, "calldata", "maxcall", DEFAULT_NUM_CALLOUT, &nMaxCall );
    byMaxCall = (0 == nMaxCall ? DEFAULT_NUM_CALLOUT : (u8)nMaxCall);
  
    return TRUE;
}

/*=============================================================================
  �� �� ���� GetMtSliceInfoFromDbgFile
  ��    �ܣ� �������ն��Ƿ��ȡ�а�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2008/10       4.5			Ѧ��                  ����
=============================================================================*/
BOOL32 CMtAdpData::GetMtSliceInfoFromDbgFile()
{
	s8 achFName[KDV_MAX_PATH] = {0};

#ifdef _LINUX_
    sprintf(achFName, "%s/%s", "/usr/bin", "mtcap.ini");
#elif WIN32
    sprintf(achFName, "%s/%s", "./conf", "mtcap.ini");
#else
	sprintf(achFName, "%s/%s", "/conf", "mtcap.ini");
#endif

    s32 nKeyValue = 0;
    ::GetRegKeyInt(achFName, "slice", "UseSlice", 0, &nKeyValue); //Ĭ�ϲ�֧���а�

	g_cMtAdpApp.m_bSupportSlice = (nKeyValue == 0) ? FALSE : TRUE ;

	return TRUE;

}

/*=============================================================================
  �� �� ���� GetMtSliceInfoFromDbgFile
  ��    �ܣ� ��̩���ն��޷�ȡ��˫������ʱ���Ƿ����׼���˫������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/08       4.5			�ܾ���                  ����
=============================================================================*/
BOOL32 CMtAdpData::GetTaideDsSupport( void )
{
	s8 achFName[KDV_MAX_PATH] = {0};

#ifdef _LINUX_
    sprintf(achFName, "%s/%s", "/usr/bin", "mtcap.ini");
#elif WIN32
    sprintf(achFName, "%s/%s", "./conf", "mtcap.ini");
#else
	sprintf(achFName, "%s/%s", "/conf", "mtcap.ini");
#endif

    s32 nKeyValue = 0;
    ::GetRegKeyInt(achFName, "dscheat", "taidesupport", 1, &nKeyValue); //Ĭ��֧��˫����������

	g_cMtAdpApp.m_bSupportTaideDsCap = (nKeyValue == 0) ? FALSE : TRUE ;

	return TRUE;
}

/*=============================================================================
  �� �� ���� GetIsRespDRQFormGK
  ��    �ܣ� �Ƿ���ӦGK��DRQ��Ϣ��0������Ӧ��1����Ӧ��Ĭ��Ϊ1
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  20091211       4.5	    pengjie                Create
=============================================================================*/
BOOL32 CMtAdpData::GetIsRespDRQFormGK( void )
{
    s8 achFName[KDV_MAX_PATH] = {0};
    sprintf(achFName, "%s/%s", DIR_CONFIG, "mtadpdebug.ini");
	
	s32 nIsRespDRQFromGK = 0;
	::GetRegKeyInt( achFName, "calldata", "IsRespDRQFromGK", 0, &nIsRespDRQFromGK );
	g_cMtAdpApp.m_bIsRespDRQFromGK = (0 == nIsRespDRQFromGK ? FALSE : TRUE);

	printf( "[MtAdplib] GetIsRespDRQFormGK(), Read And Set m_bIsRespDRQFromGK = %d\n", g_cMtAdpApp.m_bIsRespDRQFromGK );

	return TRUE;
}

/*=============================================================================
  �� �� ���� GetMtDSDbgInfoFromDbgFile
  ��    �ܣ� �������ն˵�˫��Debug����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2008/10       4.5			�ű���                  ����
=============================================================================*/
/*lint -save -esym(438, ppszTable)*/
BOOL32 CMtAdpData::GetMtDSDbgInfoFromDbgFile()
{
    s8 achFName[KDV_MAX_PATH] = {0};

#ifdef _LINUX_
    sprintf(achFName, "%s/%s", "/usr/bin", "mtcap.ini");
#elif WIN32
    sprintf(achFName, "%s/%s", "./conf", "mtcap.ini");
#else
	sprintf(achFName, "%s/%s", "/conf", "mtcap.ini");
#endif

    s32 nEntryNum = 0;
    ::GetRegKeyInt(achFName, "mtcaptable", "EntryNum", 0, &nEntryNum);

    if (nEntryNum == 0)
    {
        return TRUE;
    }

    //�����ã������ڴ�
    s8 **ppszTable = NULL;
    ppszTable = new s8*[(u32)nEntryNum];
    
    if (NULL == ppszTable)
    {
        printf("[GetMtDSDbgInfo] failed to new mem!\n");
        return FALSE;
    }

    u8 byIdx = 0;
    for (; byIdx < (u8)nEntryNum; byIdx ++)
    {
        ppszTable[byIdx] = new s8[MAX_VALUE_LEN+1];
        if(NULL == ppszTable[byIdx])
        {
            printf("[GetMtDSDbgInfo] failed to new mem for mt.%d!\n", byIdx);

            //�ͷ��Ѿ�������ڴ�
            u8 byMemIdx = byIdx;

            for(u8 byLoop = 0; byLoop < byMemIdx; byLoop++)
            {
                if (NULL != ppszTable[byLoop])
                {
                    delete [] (s8*)ppszTable[byLoop];
                    ppszTable[byLoop] = NULL;
                }
            }

            delete [] (s8*)ppszTable;
            ppszTable = NULL;
            
            return FALSE;
        }
    }

    // ȡ������
    BOOL32 bResult = FALSE;
    u32 dwTmpEntryNum = (u32)nEntryNum;
    bResult = GetRegKeyStringTable( achFName, "mtcaptable", "fail",
                                    ppszTable, &dwTmpEntryNum, (MAX_VALUE_LEN+1) );
    bResult &= (dwTmpEntryNum == (u32)nEntryNum ? TRUE : FALSE);

    if( !bResult)
    {
        printf("[GetMtDSDbgInfo] Wrong profile while reading mtcaptable!\n");
        
        //�ͷ��Ѿ�������ڴ�
        u8 byMemIdx = byIdx;
        
        for(byIdx = 0; byIdx < byMemIdx; byIdx++)
        {
            if (NULL != ppszTable[byIdx])
            {
                delete [] (s8*)ppszTable[byIdx];
                ppszTable[byIdx] = NULL;
            }
        }
        
        delete [] (s8*)ppszTable;
        ppszTable = NULL;
        return FALSE;
    }

    s8 achSeps[] = ", ";
    s8 *pchToken = NULL;

    //�����ִ���д��ȫ�����������ο�
    for(u8 byLoop = 0; byLoop < (u8)nEntryNum; byLoop++)
    {
        TMtDSCap tMtDSCap;

        //HWVer
        pchToken = strtok(ppszTable[byLoop], achSeps);
        if( NULL == pchToken )
        {
            printf("[GetMtDSDbgInfo] Wrong profile while reading HWVer[%d]!\n", byLoop);
            continue;
        }
        else
        {
            tMtDSCap.SetHWVer(pchToken);
        }

        //Res
        pchToken = strtok(NULL, achSeps);
        if (NULL == pchToken)
        {
            printf("[GetMtDSDbgInfo] Wrong profile while reading Res[%d]!\n", byLoop);
            continue;
        }
        else
        {
            u8 byRes = CMtAdpUtils::GetResFromStr(pchToken);
            tMtDSCap.SetRes(byRes);
        }

        //Fps
        pchToken = strtok(NULL, achSeps);
        if (NULL == pchToken)
        {
            printf("[GetMtDSDbgInfo] Wrong profile while reading Fps[%d]!\n", byLoop);
        }
        else
        {
            tMtDSCap.SetFps(atoi(pchToken));
        }
        
        m_tDSCaps.AddMtCap(tMtDSCap.GetHWVer(),
                           tMtDSCap.GetRes(), tMtDSCap.GetFps());
    }

    //�ͷ��Ѿ�������ڴ�
    u8 byMemIdx = byIdx;
    
    for(byIdx = 0; byIdx < byMemIdx; byIdx++)
    {
        if (NULL != ppszTable[byIdx])
        {
            delete [] (s8*)ppszTable[byIdx];
            ppszTable[byIdx] = NULL;
        }
    }
    
    delete [] (s8*)ppszTable;
    ppszTable = NULL;

    return TRUE;
}
/*lint -restore*/
/*=============================================================================
�� �� ���� SetExpireDate
��    �ܣ� ����license ��ʱʹ������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TKdvTime &tExpireDate [in]
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/10/23   			Ѧ��                  ����
=============================================================================*/
void CMtAdpData::SetExpireDate( TKdvTime &tExpireDate )
{
    m_tExpireDate = tExpireDate;
}
/*==============================================================================
������    :  IsExpiredDate
����      :  
�㷨ʵ��  :  
����˵��  :  TKdvTime &tDate [in]
����ֵ˵��:  BOOL������ʧЧ����TRUE
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2008-10-23					Ѧ��							����
==============================================================================*/

BOOL32 CMtAdpData::IsExpiredDate(const TKdvTime &tDate)
{
    if (tDate.GetYear() == 0)
    {
        return FALSE;
    }
	
    time_t tExpireDate;
    tDate.GetTime(tExpireDate);
	
    return (time(NULL) > tExpireDate);
}

/*==============================================================================
������    :  GetLicenseErrorCode
����      :  
�㷨ʵ��  :  
����˵��  :  void
����ֵ˵��:  u8
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2008-11-4					Ѧ��
==============================================================================*/
u8 CMtAdpData::GetLicenseErrorCode(void)
{
	return m_byLicenseErrCode;
}

/*==============================================================================
������    :  SetLicenseErrorCode
����      :  
�㷨ʵ��  :  
����˵��  :  u8 byErrorCode
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2008-11-4					Ѧ��
==============================================================================*/
void CMtAdpData::SetLicenseErrorCode(u8 byErrorCode)
{
	m_byLicenseErrCode = byErrorCode;
}
/*==============================================================================
������    :  GetLicenseDataFromFile
����      :  ��license
�㷨ʵ��  :  
����˵��  :  s8* pchPath	[in]�ļ�·��
����ֵ˵��:  BOOL
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2008-10-23	     			Ѧ��							����
==============================================================================*/
BOOL32 CMtAdpData::GetLicenseDataFromFile( s8* pchPath )
{
#if ( defined (_8KE_) || defined(_8KH_) || defined(_8KI_) )|| defined(_MPC_EMBEDDED_MTADP_)
	return TRUE;	// [3/6/2010 xliang] 8000E ������ú�������������ֻ��Ϊ�������� 
#endif

#ifndef WIN32    
	u16 wRet = 0; 
	wRet = ReadLicenseFile( pchPath );
	if(SUCCESS_LICENSE != wRet) 
	{
		printf("Read License file failed! wRet = %d!\n", wRet);
		StaticLog( "Read License file failed! wRet = %d!\n", wRet);
		g_cMtAdpApp.m_wMaxNumConntMt = 0; 
		g_cMtAdpApp.SetLicenseErrorCode(1);
		return FALSE;
	}
	
	s8 achGetValue[50];
	s32 nValuelen = 50;

	//�����֤�汾��
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
    wRet = GetValue(KEY_LICENSE_VERSION, achGetValue, nValuelen);
	if( SUCCESS_LICENSE != wRet )
	{
		printf("Read license version failed!\n");
		StaticLog( "Read license version failed!\n");
//		return FALSE;
	}
    if( 0 != strcmp( LICENSE_VERSION_HDI, achGetValue ) ) //�º�
	{
		//g_cMtAdpApp.m_wMaxNumConntMt = 0;
		//g_cMtAdpApp.SetLicenseErrorCode(2);
		printf("License version(%s) dismatched!\n",achGetValue);
		StaticLog( "License version(%s) dismatched!\n",achGetValue);
//		return FALSE;
	}

// 	//�����֤���
// 	nValuelen = 50;
// 	memset( achGetValue, 0, sizeof(achGetValue) );
//     wRet = GetValue(KEY_LICENSE_TYPE, achGetValue, nValuelen);
// 	if( SUCCESS_LICENSE != wRet)
// 	{
// 		g_cMtAdpApp.m_wMaxNumConntMt = 0;
// 		g_cMtAdpApp.SetLicenseErrorCode(2);
// 
// 		printf("Read license type failed!\n");
// 		OspPrintf(TRUE,FALSE,"Read license type failed!\n");
// 		return FALSE;
// 	}
// 	if( 0 != memcmp( LICENSE_TYPE_HDI, achGetValue, strlen(LICENSE_TYPE_HDI) ) ) 
// 	{
// 		g_cMtAdpApp.m_wMaxNumConntMt = 0;
// 		g_cMtAdpApp.SetLicenseErrorCode(2);
// 
// 		printf("License type is not correct!\n");
// 		OspPrintf(TRUE,FALSE,"License type is not correct!\n");
// 		return FALSE;
// 	}
	//printf("License type: %s\n",achGetValue);
	
	//��License ����
// 	nValuelen = 50;
// 	memset( achGetValue, 0, sizeof(achGetValue) );
// 	if( SUCCESS_LICENSE != ( wRet = GetValue( (s8*)KEY_LICENSE_KEY, achGetValue, nValuelen ) ) )
// 	{
// 		g_cMtAdpApp.m_wMaxNumConntMt = 0;
// 		printf("Read LicenseKey failed!\n");
// 		OspPrintf(TRUE,FALSE,"Read LicenseKey failed!\n");
// 		return FALSE;
// 	}
    //g_cMcuGuardApp.SetLicenseSN(achGetValue);

	//���豸���к�
    nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
    wRet = GetValue(KEY_HDI_DEVICE_ID, achGetValue, nValuelen);
	if( SUCCESS_LICENSE != wRet)
	{
		g_cMtAdpApp.m_wMaxNumConntMt = 0;
		g_cMtAdpApp.SetLicenseErrorCode(3);

		printf("Read device id failed!\n");
		return FALSE;
	}
    StrUpper( achGetValue );

    //�ֱ���2�ַָ���
    u8 achSequenceStr[64] = {0};    
	g_cBrdAgentApp.GetBoardSequenceNum( achSequenceStr, ":" );
	StrUpper( (s8*)achSequenceStr );	
	if( 0 != strcmp( (s8*)achSequenceStr, achGetValue ) )
	{
        memset(achSequenceStr, 0, sizeof(achSequenceStr));
        g_cBrdAgentApp.GetBoardSequenceNum( achSequenceStr, "-" ); 
        StrUpper( (s8*)achSequenceStr );	
        if( 0 != strcmp( (s8*)achSequenceStr, achGetValue ) )
	    {
		    printf("Device id is incorrect for this board!\n");
		    g_cMtAdpApp.m_wMaxNumConntMt = 0;
		    g_cMtAdpApp.SetLicenseErrorCode(3);
		    return FALSE;
        }
	}
	printf("Device id: %s\n",achGetValue);

	//��mcu����
// 	nValuelen = 50;
// 	memset( achGetValue, 0, sizeof(achGetValue) );
// 	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_TYPE, achGetValue, nValuelen) ) ) 
// 	{
// 		printf("Read Mcu type failed!\n");
// 		OspPrintf(TRUE,FALSE,"Read Mcu type failed!\n");
// 		g_cMtAdpApp.m_wMaxNumConntMt = 0;
// 		g_cMtAdpApp.SetLicenseErrorCode(4);
// 		return FALSE;
// 	}
// 	else
// 	{
// 		// ��MCU���ͽ���У��
// 		if( 0 != strcmp( "HDI", achGetValue ) )
// 		{
// 			g_cMtAdpApp.m_wMaxNumConntMt = 0;
// 			g_cMtAdpApp.SetLicenseErrorCode(4);
// 			printf("Mcu type(%s) is error!\n", achGetValue);
// 			OspPrintf(TRUE,FALSE, "Mcu type(%s) is error!\n", achGetValue);
// 			return FALSE;
// 		}
// 	} 
// 	
	//����Ȩ��������
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
    wRet = GetValue(KEY_HDI_ACCESS_NUM, achGetValue, nValuelen);
	if( SUCCESS_LICENSE != wRet )
	{
		g_cMtAdpApp.m_byMaxHDMtNum = 0;
		g_cMtAdpApp.SetLicenseErrorCode(5);
		
		printf("Read Mtadp access num failed!\n");
		StaticLog( "Read Mtadp access num failed!\n");
		return FALSE;
	}
	//g_cMtAdpApp.m_wMaxNumConntMt = atoi(achGetValue) ;
	g_cMtAdpApp.m_byMaxHDMtNum = (u8)atoi(achGetValue) ; //  [7/28/2011 chendaiwei]��¼Mtadp����������
    // ���ܴ�������ֵ
    if (g_cMtAdpApp.m_byMaxHDMtNum > MAXNUM_HDI_MT)
    {
        g_cMtAdpApp.m_byMaxHDMtNum = MAXNUM_HDI_MT;
    }
	
	//��ʧЧ����
// 	nValuelen = 50;
// 	memset( achGetValue, 0, sizeof(achGetValue) );
// 	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_EXP_DATE, achGetValue, nValuelen) ) )
// 	{
// 		g_cMtAdpApp.m_wMaxNumConntMt = 0;
// 		g_cMtAdpApp.SetLicenseErrorCode(6);
// 		printf("Read Mcu expire time failed!\n");
// 		OspPrintf(TRUE,FALSE,"Read Mcu expire time failed!\n");
// 		return FALSE;
// 	}
// 	TKdvTime tDate;
// 	s8 * pchToken = strtok( achGetValue, "-" );
// 	if( NULL != pchToken)
// 	{
// 		tDate.SetYear( atoi( pchToken ) );
// 		pchToken = strtok( NULL, "-" );
// 		if( NULL != pchToken )
// 		{
// 			tDate.SetMonth( atoi( pchToken ) );
// 			pchToken = strtok( NULL, "-" );
// 			if( NULL != pchToken )
// 			{
// 				tDate.SetDay( atoi( pchToken ) );
// 			}
// 		}
// 	}
// 	if( 0 == tDate.GetYear() || 0 == tDate.GetMonth() || 0 == tDate.GetDay() )
// 	{
// 		g_cMtAdpApp.m_wMaxNumConntMt = 0;
// 		g_cMtAdpApp.SetLicenseErrorCode(6);
// 		printf("Read Mtadp expire time failed!\n");
// 		OspPrintf(TRUE,FALSE,"Read Mcu expire time failed!\n");
// 		return FALSE;
// 	}
//     // guzh [5/8/2007] ������ó���2030�꣬��Ϊ������
//     if ( tDate.GetYear() >= 2030 )
//     {
//         memset( &tDate, 0, sizeof(tDate) );
//     }
// 	if( g_cMtAdpApp.IsExpiredDate(tDate) )
// 	{
// 		g_cMtAdpApp.m_wMaxNumConntMt = 0;
// 		g_cMtAdpApp.SetLicenseErrorCode(6);
// 		printf("License date expired!\n");
// 		OspPrintf(TRUE,FALSE,"License date expired!\n");
// 		return FALSE;
// 	}
// 	g_cMtAdpApp.SetExpireDate(tDate);
	
#endif
	return TRUE;
}

/*====================================================================
    ������      : SetMcuCodingForm
    ����        : �洢mcu���뷽ʽ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: emCodingForm emMcuCodingForm
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2013/04/08              liaokang      ����
====================================================================*/
void CMtAdpData::SetMcuEncoding(emenCodingForm emMcuEncoding)
{
    m_emMcuEncoding = emMcuEncoding;
}

/*====================================================================
    ������      : GetMcuCodingForm
    ����        : ��ȡMCU���뷽ʽ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  : emCodingForm
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2013/04/08              liaokang      ����
====================================================================*/
emenCodingForm CMtAdpData::GetMcuEncoding()
{
    return m_emMcuEncoding;
}

/*====================================================================
    ������      : SetGkEncoding
    ����        : �洢Gk���뷽ʽ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2013/04/08              liaokang      ����
====================================================================*/
void CMtAdpData::SetGkEncoding(emenCodingForm emGkEncoding)
{
    m_emGkEncoding = emGkEncoding;
}

/*====================================================================
    ������      : GetGkEncoding
    ����        : ��ȡGk���뷽ʽ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  : 
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2013/04/08              liaokang      ����
====================================================================*/
emenCodingForm CMtAdpData::GetGkEncoding()
{
    return m_emGkEncoding;
}

/*====================================================================
������      : SetStackInitUtf8
����        : ����Э��ջ�Ƿ�ΪUTF8����
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��: 
����ֵ˵��  :
----------------------------------------------------------------------
�޸ļ�¼    :
��  ��      �汾        �޸���      �޸�����
2013/08/29              �����      ����
====================================================================*/
void CMtAdpData::SetStackInitUtf8(BOOL32 bUtf8)
{
	m_byStackInitUtf8 = bUtf8 ? 1:0;
}

/*====================================================================
������      : GetStackInitUtf8
����        : ȡ��Э��ջ�Ƿ�UTF8����
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��: 
����ֵ˵��  :
----------------------------------------------------------------------
�޸ļ�¼    :
��  ��      �汾        �޸���      �޸�����
2013/08/29              �����      ����
====================================================================*/
u8 CMtAdpData::GetStackInitUtf8(void)
{
	return m_byStackInitUtf8;
}

/*==============================================================================
������    :  AddToConnMcuList
����      :  
�㷨ʵ��  :  
����˵��  :  u8 byInstId	[in]
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
1/4/2009 					Ѧ��							����
==============================================================================*/
void CMtAdpData::AddToConnMcuList( u16 wInstId)
{
	if( wInstId == 0 || wInstId > MAXNUM_CONF_MT)
	{
		return;
	}
	m_abyConnMcuMtList[wInstId-1] = 2;
}

void CMtAdpData::AddToConnMtList( u16 wInstId)
{
	if( wInstId == 0 || wInstId > MAXNUM_CONF_MT)
	{
		return;
	}
	m_abyConnMcuMtList[wInstId-1] = 1;
}

BOOL32 CMtAdpData::IsInConnMcuList(u16 wInstId)
{
	if(m_abyConnMcuMtList[wInstId-1] == 2)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL32 CMtAdpData::IsInConnMtList(u16 wInstId)
{
	if(m_abyConnMcuMtList[wInstId-1] == 1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CMtAdpData::ClearConnMcuMtList(u16 wInstId)
{
	if( wInstId == 0 || wInstId > MAXNUM_CONF_MT)
	{
		return;
	}
	m_abyConnMcuMtList[wInstId-1] = 0;
}

/*==============================================================================
������    :  SetMtInviteMsgInOsp
����      :  ���������Ϣ�Ѿ���/��osp��Ϣ���� 
�㷨ʵ��  :  
����˵��  :  u8 byConfIdx [in]
			 u8 byMtId [in]
			 bIsMsgIsOsp [in]��TRUE�������У�FALSE��������	
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2011/03/15 					�ܾ���							����
==============================================================================*/
void CMtAdpData::SetMtInviteMsgInOsp( u8 byConfIdx,u8 byMtId,BOOL32 bIsMsgIsOsp /*= TRUE*/ )
{
	if( 0 == byMtId || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	u8 byPos = (byMtId-1) % 8;
	u8 byIdx = (byMtId-1) / 8;
	if( bIsMsgIsOsp )
	{
		m_abyInvitedMsgInOspMt[byConfIdx][ byIdx ] |= ( (unsigned int)1 << byPos );
	}
	else
	{
		m_abyInvitedMsgInOspMt[byConfIdx][ byIdx ] &= ~( (u8)((unsigned int)1 << byPos) );
	}
}

/*==============================================================================
������    :  IsMtInviteMsgInOsp
����      :  ������Ϣ�Ƿ����OSP��Ϣ���� 
�㷨ʵ��  :  
����˵��  :  u8 byConfIdx[in] 
			 u8 byMtId	[in]
����ֵ˵��:  BOOL32:������Ϣ�Ƿ����OSP��Ϣ����
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2011/03/15 					�ܾ���							����
==============================================================================*/
BOOL32 CMtAdpData::IsMtInviteMsgInOsp( u8 byConfIdx,u8 byMtId )
{
	if( 0 == byMtId || byMtId > MAXNUM_CONF_MT || byConfIdx >= MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE)
	{
		return FALSE;
	}

	u8 byPos = (byMtId-1) % 8;
	u8 byIdx = (byMtId-1) / 8;

	if( m_abyInvitedMsgInOspMt[byConfIdx][ byIdx ] & ( (unsigned int)1 << byPos ) )
	{
		return TRUE;
	}
	return FALSE;
}

/*==============================================================================
������    :  SetNeglectInviteMsg
����      :  �����Ƿ����������Ϣ
�㷨ʵ��  :  
����˵��  :  u8 byConfIdx [in]
			 u8 byMtId [in]
			 BOOL32 bIsNeglect [in]:TRUE:���ԣ�FALSE:������
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2011/03/15 					�ܾ���							����
==============================================================================*/
void CMtAdpData::SetNeglectInviteMsg( u8 byConfIdx,u8 byMtId,BOOL32 bIsNeglect /*= TRUE*/ )
{
	if( 0 == byMtId || byMtId > MAXNUM_CONF_MT || byConfIdx >= MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE)
	{
		return;
	}
	u8 byPos = (byMtId-1) % 8;
	u8 byIdx = (byMtId-1) / 8;
	if( bIsNeglect )
	{
		m_abyNeglectInviteMsgMt[byConfIdx][byIdx] |= ( (unsigned int)1 << byPos );
	}
	else
	{
		m_abyNeglectInviteMsgMt[byConfIdx][ byIdx ] &= ~( (u8)((unsigned int)1 << byPos) );
	}
	
}

/*==============================================================================
������    :  IsNeglectInviteMsg
����      :  �Ƿ����������Ϣ
�㷨ʵ��  :  
����˵��  :  u8 byConfIdx [in]
			 u8 byMtId	[in]
����ֵ˵��:  BOOL32:�Ƿ����������Ϣ
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2011/03/15 					�ܾ���							����
==============================================================================*/
BOOL32 CMtAdpData::IsNeglectInviteMsg( u8 byConfIdx,u8 byMtId )
{
	if( 0 == byMtId || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	u8 byPos = (byMtId-1) % 8;
	u8 byIdx = (byMtId-1) / 8;

	if( m_abyNeglectInviteMsgMt[byConfIdx][ byIdx ] & ( (unsigned int)1 << byPos ) )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL32 CMtAdpData::GetRestrictManu( void )
{
    s8 achFName[MAX_PATH] = {0};
    sprintf(achFName, "%s/%s", DIR_CONFIG, "mtadpdebug.ini");

    BOOL32 bResult = FALSE;
    s32   nMemEntryNum;    
    //  get the number of entry 
    bResult = GetRegKeyInt( achFName, (const s8*)"RestrictManu", 
                                  STR_ENTRY_NUM, 0, &nMemEntryNum );
    if( !bResult ) 
    {
        printf("[GetRestrictManu] Wrong profile while reading %s %s!\n", 
            (const s8*)"RestrictManu", STR_ENTRY_NUM );
        return FALSE;
    }
    
    // û������
    if(0 == nMemEntryNum)
    {
        return TRUE;
    }

    s8* *ppszTable = NULL;
    ppszTable = new s8*[(u32)nMemEntryNum];
    if(NULL == ppszTable )
    {
        printf("[GetRestrictManu] Fail to malloc memory for Restrict Manu table\n");
        return FALSE;
    }
    
    for( u32 dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];

        if(NULL == ppszTable[dwLoop] )
        {
            printf("[GetRestrictManu] Fail to malloc memory for Restrict Manu table1\n");
            for(u32 dwIdex = 0; dwIdex < dwLoop; dwIdex++ )
            {
                if( ppszTable[dwIdex] != NULL )
                {
                    delete [] (s8*)ppszTable[dwIdex] ;
                }
            }            
            delete [] (s8*)ppszTable;

            return FALSE;
        }
    }

    bResult = GetRegKeyStringTable( achFName,       
                                    (const s8*)"RestrictManu",             
                                    "failed",                        
                                    ppszTable,                       
                                    (u32*)&nMemEntryNum,                                                                    
                                    MAX_VALUE_LEN + 1                
                                  );

    if( !bResult )
    {
        printf("[GetRestrictManu] Wrong profile while reading %s table!\n", 
            (const s8*)"RestrictManu");

        if( ppszTable != NULL )
        {
            for(u32 dwInnerLoop = 0; dwInnerLoop < (u32)nMemEntryNum; dwInnerLoop++ )
            {
                if( ppszTable[dwInnerLoop] != NULL )
                {
                    delete [] ppszTable[dwInnerLoop];
                }
            }
            
            delete [] ppszTable;   
        }
        
        return FALSE;
    }
     
    s8    *pchToken = NULL;
    s8    achSeps[] = " ,\t";       /* space or tab as seperators */
    for( u32 dwManuIdx = 0; dwManuIdx < (u32)nMemEntryNum; dwManuIdx++)
    {
        pchToken = strtok( ppszTable[dwManuIdx], achSeps );
        
        if( NULL == pchToken )
        {
            printf("[GetRestrictManu] Wrong profile while reading Manu index %d!\n", dwManuIdx);
            //bResult = FALSE;
            continue;
        }
        else
        {
            u8 byManuId = atoi(pchToken);

            //��������Ϣ�������ƺ����б�
            if ( byManuId > 0 && byManuId <= 32)
            {
                AddManuToRestrictTable( byManuId );
            }       
        }        
    }

	//�ͷ�Buffer
	if( ppszTable != NULL )
	{
		for(u32 dwIdx = 0; dwIdx < (u32)nMemEntryNum; dwIdx++ )
		{
			if( ppszTable[dwIdx] != NULL )
			{
				delete [] ppszTable[dwIdx];
			}
		}
		
		delete [] ppszTable;   
    }

	return TRUE;
}

/*==============================================================================
������    :  ReadFakedProductId
����      :  �������ļ��л�ȡαװ�Ĳ�ƷID��Mtadp�� ��Ŀǰ���TSʹ�ã�
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2012/03/23				   �´�ΰ							����
==============================================================================*/
void CMtAdpData::ReadFakedProductId( void )
{
    s8 achFName[KDV_MAX_PATH] = {0};
    sprintf(achFName, "%s/%s", DIR_CONFIG, "mtadpdebug.ini");
	
	s8   achDefStr[] = "";	
    BOOL32 bResult = GetRegKeyString( achFName, "calldata", "FakedProductId", 
		achDefStr, m_achFakedProductId, MAX_ProductIDSize );

	if(bResult)
	{
		printf( "[MtAdplib]ReadFakedProductId success: %s\n", m_achFakedProductId );
	}

	return;
}

/*==============================================================================
������    :  GetPinHoleTime
����      :  ��ȡ��ʱ����
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013/05/28				   �±�								����
==============================================================================*/
void CMtAdpData::GetPinHoleTime(void)
{
    s8 achFName[KDV_MAX_PATH] = {0};
    sprintf(achFName, "%s/%s", DIR_CONFIG, "mtadpdebug.ini");
	
	s32 nPinHoleInterval = 0;
	::GetRegKeyInt( achFName, "PinHoldConfig", "PinHoleInterval", 5, &nPinHoleInterval );
	g_cMtAdpApp.m_dwPinHoleInterval = nPinHoleInterval;
}


/*==============================================================================
������    :  ReadDiscHeartBeatParam
����      :  ��ö���������
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2012/12/04				   �ܾ���							����
==============================================================================*/
#ifdef _IS22_
void CMtAdpData::ReadDiscHeartBeatParam( void )
{
	s8    achProfileName[50] = {0};
    s32   sdwValue;    

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

	//��ȡosp����������
    GetRegKeyInt( achProfileName, SECTION_BoardSystem, KEY_HeartBeatTime, DEF_OSPDISC_HBTIME, &sdwValue );
    if (sdwValue >= 3 )
    {
        m_wDiscHeartBeatTime = (u16)sdwValue;
    }
    else
    {
        m_wDiscHeartBeatTime = DEF_OSPDISC_HBTIME;
    }

    
    GetRegKeyInt( achProfileName, SECTION_BoardSystem, KEY_HeartBeatNum, DEF_OSPDISC_HBNUM, &sdwValue );
    if (sdwValue >= 1)
    {
        m_byDiscHeartBeatNum = (u8)sdwValue;
    }   
    else
    {
        m_byDiscHeartBeatNum = DEF_OSPDISC_HBNUM;
    }
}

/*==============================================================================
������    :  GetDiscHeartBeatNum
����      :  ��ö���������
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  ���ض���������
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2012/12/04				   �ܾ���							����
==============================================================================*/
u8 CMtAdpData::GetDiscHeartBeatNum( void )
{
	return m_byDiscHeartBeatNum;
}

/*==============================================================================
������    :  GetDiscHeartBeatTime
����      :  ��ö������ʱ��
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  ���ض������ʱ��
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2012/12/04				   �ܾ���							����
==============================================================================*/
u16 CMtAdpData::GetDiscHeartBeatTime( void )
{
	return m_wDiscHeartBeatTime;
}
#endif


/*==============================================================================
������    :  GetFakedProductId
����      :  ��ȡαװ�Ĳ�ƷID ��Ŀǰ���TSʹ�ã�
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  ����Faked ProductID�ַ���.���δ��ȡ���ַ���������NULL
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2012/03/23				   �´�ΰ							����
==============================================================================*/
LPCSTR CMtAdpData::GetFakedProductId( void )
{
	if(strcmp(m_achFakedProductId,""))
	{
		return m_achFakedProductId;
	}
	else
	{
		return NULL;
	}
}

BOOL32 CMtAdpData::AddManuToRestrictTable( u8 byManuId )
{
    if (byManuId > 0 && byManuId <= 32)
    {
        m_dwRestrictManuTable |= ( (unsigned int)0x00000001 << ( byManuId - 1 ));
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL32 CMtAdpData::IsManuInRestrictTable( u8 byManuId )
{
    if ( ((unsigned int)0x00000001 << ( byManuId - 1 ) ) & m_dwRestrictManuTable )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/************************************************************************/
/*                                                                      */
/*                            TMtQueue                                  */
/*                                                                      */
/************************************************************************/

//����
TMtQueue::TMtQueue() : m_ptCallInfo(NULL),
                       m_byLen(0),
                       m_nHead(0),
                       m_nTail(0),
                       m_bPrtChgInfo(FALSE)
{
}

//����
/*lint -save -e1551 -e1579*/
TMtQueue::~TMtQueue()
{
    Quit();
}
/*lint -restore*/
/*=============================================================================
  �� �� ���� Init
  ��    �ܣ� ���г�ʼ�ڴ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byQueueLen
  �� �� ֵ�� void
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
void TMtQueue::Init( u8 byQueueLen )
{
    if ( 0 == byQueueLen ) 
    {
        StaticLog( "[TMtQueue] err: Len.%d, Init failed !\n", byQueueLen );
        return;
    }
    m_ptCallInfo = new TMtCallInfo[byQueueLen];
    if ( NULL != m_ptCallInfo )
    {
        m_byLen = byQueueLen;
    }
    else
    {
        StaticLog( "[TMtQueue] Init failed !\n" );
    }
    return;    
}

/*=============================================================================
  �� �� ���� Quit
  ��    �ܣ� �����ڴ��ͷ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� void
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
void TMtQueue::Quit()
{
    if ( NULL != m_ptCallInfo ) 
    {
        delete [] m_ptCallInfo;
        m_ptCallInfo = NULL;
    }
    m_byLen = 0;
    m_nHead = 0;
    m_nTail = 0;
    m_bPrtChgInfo = FALSE;
}

/*=============================================================================
  �� �� ���� ProcQueueInfo
  ��    �ܣ� �������
  �㷨ʵ�֣� ������ �� ��ѭ�ϸ�� FIFO ����
  ȫ�ֱ����� 
  ��    ���� TMtCallInfo &tCallInfo:
             BOOL32      bInc      : TRUE ���У�    FALSE ������� �� ��������
             BOOL32      bDel      : TRUE ������У�FALSE ��������
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
BOOL32 TMtQueue::ProcQueueInfo( TMtCallInfo &tCallInfo, BOOL32 bInc, BOOL32 bDel )
{
    if ( bInc )
    {
        if ( IsQueueFull() )
        {
			QueueLog("[ProcQueueInfo] Buffer Full!\n");
            return FALSE;
        }
        if ( tCallInfo.IsNull() ) 
        {
			QueueLog("[ProcQueueInfo] tCallInfo Null!\n");
            return FALSE;
        }
        if ( IsMtInQueue(tCallInfo.m_byConfIdx, tCallInfo.m_tMt.GetMtId()) )
        {
			QueueLog("[ProcQueueInfo] Mt<%d, %d> exist already!\n",
                      tCallInfo.m_byConfIdx, tCallInfo.m_tMt.GetMtId() );
            return FALSE;
        }

        memcpy(&m_ptCallInfo[m_nTail], &tCallInfo, sizeof(tCallInfo));
        m_nTail = (m_nTail + 1) % m_byLen;

        QueueLog("[ProcQueueInfo] Mt <%d, %d> INC queue !\n",
                  tCallInfo.m_byConfIdx, tCallInfo.m_tMt.GetMtId());
        if ( m_bPrtChgInfo ) 
        {
            ShowQueue();
        }

        return TRUE;
    }
    else
    {
        if ( IsQueueNull() )
        {
			QueueLog("[ProcQueueInfo] Buffer NULL!\n");
            return FALSE;
        }

        if ( bDel ) 
        {
            if ( tCallInfo.IsNull() ) 
            {
                QueueLog("[ProcQueueInfo] tCallInfo Null<Del>!\n");
                return FALSE;
            }
            
            u8 byQue = 0;
            if ( !IsMtInQueue(tCallInfo.m_byConfIdx, tCallInfo.m_tMt.GetMtId(), &byQue) )
            {
                QueueLog( "[ProcQueueInfo] Mt <%d, %d> to be DE unexist !\n",
                           tCallInfo.m_byConfIdx, tCallInfo.m_tMt.GetMtId());
                return FALSE;
            }
            m_ptCallInfo[byQue].SetNull();
            
            // ɾ����MT���ڶ���ͷ���������
            if ( m_nHead != byQue )
            {
                s32 nPos = 0;
                s32 nAdjustPos = byQue > m_nHead ? byQue : byQue + m_byLen;
                
                for( nPos = nAdjustPos; nPos > m_nHead; nPos-- )
                {
                    s32 nQueNew   = nPos;
                    s32 nQueNewFr = nQueNew - 1;
                    nQueNew   = nQueNew % m_byLen;
                    nQueNewFr = nQueNewFr % m_byLen;
                    m_ptCallInfo[nQueNew] = m_ptCallInfo[nQueNewFr];
                }
                m_ptCallInfo[m_nHead].SetNull();
            }
            m_nHead = (m_nHead + 1) % m_byLen;
        }
        else
        {
            if ( !m_ptCallInfo[m_nHead].IsNull() )
            {
                tCallInfo = m_ptCallInfo[m_nHead];
                m_ptCallInfo[m_nHead].SetNull();
                m_nHead = (m_nHead + 1) % m_byLen;
            }
            else
            {
                QueueLog("[ProcQueueInfo] None member in queue !\n");
            }
        }

        QueueLog("[ProcQueueInfo] Mt <%d, %d> DE queue !\n",
                             tCallInfo.m_byConfIdx, tCallInfo.m_tMt.GetMtId());
        if ( m_bPrtChgInfo ) 
        {
            ShowQueue();
        }

        return TRUE;
    }
}

/*=============================================================================
  �� �� ���� IsQueueNull
  ��    �ܣ� �����Ƿ�Ϊ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
BOOL32 TMtQueue::IsQueueNull()
{
    if ( m_ptCallInfo != NULL && m_ptCallInfo[m_nHead].IsNull() )
    {
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
  �� �� ���� IsQueueFull
  ��    �ܣ� �����Ƿ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
BOOL32 TMtQueue::IsQueueFull()
{
    if ( m_ptCallInfo != NULL && !m_ptCallInfo[m_nTail].IsNull() )//pn
    {
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
  �� �� ���� IsMtInQueue
  ��    �ܣ� ��ѯ�Ƿ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
             u8 byMtId
             u8 byPos   :����λ��
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
BOOL32 TMtQueue::IsMtInQueue( u8 byConfIdx, u8 byMtId, u8 *pbyPos )
{
    if ( IsQueueNull() ) 
    {
        return FALSE;
    }
    if ( 0 == byConfIdx || 0 == byMtId ) 
    {
        return FALSE;
    }

    s32 nQue  = m_nHead;
    s32 nTail = m_nTail > m_nHead ? m_nTail : m_nTail + m_byLen;

    for( ; nQue < nTail; nQue++ )
    {
        s32 nPos = nQue;
        nPos = nPos % m_byLen;
        if ( byConfIdx == m_ptCallInfo[nPos].m_byConfIdx &&
             byMtId    == m_ptCallInfo[nPos].m_tMt.GetMtId() ) 
        {
            if ( NULL != pbyPos )
            {
                *pbyPos = (u8)nPos;
            }
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
  �� �� ���� ClearQueue
  ��    �ܣ� ��ն��г�Ա
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/05/22    4.0			�ű���                  ����
=============================================================================*/
void TMtQueue::ClearQueue()
{
    if (m_ptCallInfo != NULL)
    {
        memset( m_ptCallInfo, 0, sizeof(TMtCallInfo)*m_byLen );
    }
//    memset( m_ptCallInfo, 0, sizeof(TMtCallInfo)*m_byLen );

    m_nHead = 0;
    m_nTail = 0;
    m_bPrtChgInfo = FALSE;

    return;
}

/*=============================================================================
  �� �� ���� ShowQueue
  ��    �ܣ� ��ʾ��ǰ�ĺ��ж��к͵ȴ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
void TMtQueue::ShowQueue()
{
    // zbq [06/12/2007] �쳣����
    if ( m_byLen == 0 )
    {
        return;
    }
    
    BOOL32 bNull  = IsQueueNull();
    BOOL32 bCallQ = m_byLen == MAXNUM_DRI_MT ? FALSE : TRUE;

    s32 nPos        = 0;
    s32 nAdjustTail = 0;

    if ( !bNull ) 
    {
        if ( bCallQ )
            StaticLog( "\n--------------------- Call Queue ----------------------\n" );
        else
            StaticLog( "--------------------- Wait Queue ----------------------\n" );

        StaticLog( "Head.%d, Tail.%d, Len.%d \n", m_nHead, m_nTail,
                                 m_nTail>m_nHead ? m_nTail-m_nHead : m_nTail+m_byLen-m_nHead );

        nAdjustTail = m_nHead < m_nTail ? m_nTail : m_nTail + m_byLen;

        for( nPos = m_nHead; nPos < nAdjustTail; nPos++ )
        {
            s32 nRealPos = nPos;
            nRealPos = nRealPos % m_byLen;

            StaticLog( "   queue[%d]: < %d, %d > ",
                                        nRealPos,
                                        m_ptCallInfo[nRealPos].m_byConfIdx, 
                                        m_ptCallInfo[nRealPos].m_tMt.GetMtId() );
            
            TMtAlias tMtAlias = m_ptCallInfo[nRealPos].m_tMtAlias;
            u8 byAliasType = tMtAlias.m_AliasType;
            
            switch( byAliasType )
            {
            case mtAliasTypeE164:
                StaticLog( "Alias\tE164   : %s \n", tMtAlias.m_achAlias );
                break;
            case mtAliasTypeH323ID:
                StaticLog( "Alias\tH323ID : %s \n", tMtAlias.m_achAlias );
                break;
            case mtAliasTypeTransportAddress:
                {
                    s8 achIpAddr[16];
                    CMtAdpUtils::Ipdw2Str( htonl(tMtAlias.m_tTransportAddr.GetIpAddr()), achIpAddr );
                    StaticLog( "Alias\tIPAddr : %s \n", achIpAddr );
                }
                break;
            case mtAliasTypeH320Alias:
            case mtAliasTypeH320ID:
            case mtAliasTypeOthers:
            default:
                StaticLog( "\n" );
                break;
            }

            
        }
        if ( bCallQ )
            StaticLog( "--------------------- Call Queue End ------------------\n\n" );
        else
            StaticLog( "--------------------- Wait Queue End ------------------\n\n" );
    }
    else
    {
        if ( bCallQ )
            StaticLog( "\n********************* Call Queue NULL *****************\n\n" );
        else
            StaticLog( "********************* Wait Queue NULL *****************\n\n" );
    }
    return;
}

/*=============================================================================
  �� �� ���� PrtQueueInfo
  ��    �ܣ� ʵʱ��ӡ���ж��к͵ȴ����еĳ�������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/06    4.0			�ű���                  ����
=============================================================================*/
void TMtQueue::PrtQueueInfo( void )
{
    m_bPrtChgInfo = !m_bPrtChgInfo;
}


/*=============================================================================
  �� �� ���� QueueLog
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  s8* pszStr...
  �� �� ֵ�� void 
----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/4/17  4.0			�ܹ��                  ����
=============================================================================*/
/*lint -save -esym(530, argptr)*/
/*lint -save -esym(526, __builtin_va_start)*/
/*lint -save -esym(628, __builtin_va_start)*/
void TMtQueue::QueueLog( s8* pszStr, ... )
{
	s8 achBuf[255];
    va_list argptr;
    if ( m_bPrtChgInfo )
    {
        s32 nPrefix = sprintf( achBuf, "[MtQueue]:" );
        va_start( argptr, pszStr );
        vsprintf( achBuf + nPrefix, pszStr, argptr );
        StaticLog( achBuf );
        va_end( argptr );
        
    }
}
/*lint -restore*/

/************************************************************************/
/*                                                                      */
/*                              GLOBAL                                  */
/*                                                                      */
/************************************************************************/


/*=============================================================================
  �� �� ���� EvNewCall
  ��    �ܣ� �º����������ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� HCALL hsCall
             LPHAPPCALL lphaCall
  �� �� ֵ�� s32 CALLBACK 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
static s32 CALLBACK EvNewCall( HCALL hsCall, LPHAPPCALL lphaCall )
{
	HCALL pStackCall = hsCall;
	s32 nDstInst = 0;

	CServMsg cServMsg;
	cServMsg.SetMsgBody( (u8*)&pStackCall, sizeof(HCALL) );
	cServMsg.SetEventId( EV_RAD_NEWCALL_NOTIF );

	nDstInst = g_cMtAdpApp.ProcH323CallBack( &cServMsg, CInstance::PENDING );

	if( nDstInst > 0 && nDstInst <= MAXNUM_DRI_MT && lphaCall )
	{
		*lphaCall = (HAPPCALL)nDstInst;
	}
	else
	{
		// xliang [10/24/2008] ֪ͨmcu 
		// xliang [1/4/2009] Ų����ͨʵ���ﴦ��
// 		CServMsg cServMsg;
// 		cServMsg.SetSrcDriId(g_cMtAdpApp.m_byDriId);
// 		//cServMsg.SetMsgBody((u8*)&(g_cMtAdpApp.m_byDriId), sizeof(u8));
// 		OspPost( MAKEIID( AID_MCU_MTADPSSN, g_cMtAdpApp.m_byDriId, TCP_GATEWAY ), 
// 			MT_MCU_CALLFAIL_HDIFULL_NOTIF, 
// 			cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cMtAdpApp.m_dwMcuNode );

		StaticLog( "no idle instance found.\n" );
	}

	return TRUE;
}


/*=============================================================================
  �� �� ���� EvNewChan
  ��    �ܣ� ���ŵ��������ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  HAPPCALL haCall
              HCALL hsCall
              HCHAN hsChan
              LPHAPPCHAN lphaChan
  �� �� ֵ��  s32 CALLBACK 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
static s32 CALLBACK EvNewChan( HAPPCALL   haCall, 
					    HCALL      hsCall, 
					    HCHAN      hsChan, 
					    LPHAPPCHAN lphaChan )
{
	LogPrint( LOG_LVL_DETAIL,MID_MCULIB_MTADP,"[EvNewChan]lphaChan.%p\n",lphaChan );

	HCALL pStackCall = hsCall;
	HCHAN pStackChan = hsChan;

    /*lint -save -e507*/
	u8 byDstInst = (u8)haCall;
    /*lint -restore*/
	
	CServMsg cServMsg;
	cServMsg.SetMsgBody( (u8*)&pStackCall, sizeof(HCALL) );
	cServMsg.CatMsgBody( (u8*)&pStackChan, sizeof(HCHAN) );
	cServMsg.SetEventId( EV_RAD_NEWCHAN_NOTIF );

	g_cMtAdpApp.ProcH323CallBack( &cServMsg, byDstInst );
	
	return TRUE;
}


/*=============================================================================
  �� �� ���� EvCallCtrlMsg
  ��    �ܣ� ���п��ƻص� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� HAPPCALL haCall
             HCALL hsCall
             u16 wMsgType
             const void *pBuf
             u16 wBufLen
  �� �� ֵ�� s32 CALLBACK 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
static s32 CALLBACK EvCallCtrlMsg( HAPPCALL    haCall,
							HCALL       hsCall, 
							u16         wMsgType, 
							const void *pBuf, 
							u16         wBufLen )
{
	HCALL pStackCall = hsCall;
	u16   wType      = wMsgType;
    /*lint -save -e507*/
	u8    byDstInst  = (u8)haCall;
    /*lint -restore*/
	
	if( g_cMtAdpApp.m_byDebugLevel >= DEBUG_INFO && PszCallCtrlTypeName(wMsgType) )
	{
        // zbq [04/25/2007]
        TIME_SHOW
		OspPrintf( TRUE, FALSE, "[RADSTACK  %d]  call %p call back, msgtype = %s\n", 
						         byDstInst, hsCall, PszCallCtrlTypeName(wMsgType));
	}
	
	CServMsg cServMsg;
	cServMsg.SetMsgBody( (u8*)&pStackCall, sizeof(HCALL) );
	cServMsg.CatMsgBody( (u8*)&wType, sizeof(u16) );
	cServMsg.CatMsgBody( (u8*)pBuf, wBufLen );
	cServMsg.SetEventId( EV_RAD_CALLCTRL_NOTIF );	

	g_cMtAdpApp.ProcH323CallBack( &cServMsg, byDstInst );
	
	return TRUE;
}


/*=============================================================================
  �� �� ���� EvChanCtrlMsg
  ��    �ܣ� �ŵ����ƻص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� HAPPCALL haCall
             HCALL hsCall
             HAPPCHAN haChan
             HCHAN hsChan
             u16 wMsgType
             const void *pBuf
             u16 wBufLen
  �� �� ֵ�� s32 CALLBACK 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
static s32 CALLBACK EvChanCtrlMsg( HAPPCALL    haCall, 
						    HCALL       hsCall, 
							HAPPCHAN    haChan, 
							HCHAN       hsChan, 
							u16         wMsgType, 
							const void *pBuf, 
							u16         wBufLen )
{
	u16   wType      = wMsgType;	
	HCALL pStackCall = hsCall;
	HCHAN pStackChan = hsChan;
    /*lint -save -e507*/
    u8    byDstInst  = (u8)haCall;
    /*lint -restore*/

	if( g_cMtAdpApp.m_byDebugLevel >= DEBUG_INFO && PszChanCtrlTypeName(wMsgType) )
	{
        // zbq [04/25/2007]
        TIME_SHOW
		OspPrintf( TRUE, FALSE, "[RADSTACK  %d]  chan %p callback, msgtype = %s,haChan = %p\n", 
			                   byDstInst, hsChan, PszChanCtrlTypeName(wMsgType),haChan );
	}

	CServMsg cServMsg;
	cServMsg.SetMsgBody( (u8*)&pStackCall, sizeof(HCALL) );
	cServMsg.CatMsgBody( (u8*)&pStackChan, sizeof(HCHAN) );
	cServMsg.CatMsgBody( (u8*)&wType, sizeof(u16) );
	cServMsg.CatMsgBody( (u8*)pBuf, wBufLen );
	cServMsg.SetEventId( EV_RAD_CHANCTRL_NOTIF );

	g_cMtAdpApp.ProcH323CallBack( &cServMsg, byDstInst );
	return TRUE;
}


/*=============================================================================
  �� �� ���� EvConfCtrlMsg
  ��    �ܣ� ������ƻص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� HAPPCALL  haCall
             HCALL     hsCall
             u16       wMsgType
             const void *pBuf
             u16       wBufLen
  �� �� ֵ�� s32       CALLBACK 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
static s32 CALLBACK EvConfCtrlMsg( HAPPCALL    haCall,
						    HCALL       hsCall,
							u16         wMsgType,
							const void *pBuf,
							u16         wBufLen )
{
	HCALL pStackCall = hsCall;
	u16 wType = wMsgType;

    /*lint -save -e507*/
    u16 wDstInst = (u16)haCall;
    /*lint -restore*/

	if( g_cMtAdpApp.m_byDebugLevel >= DEBUG_INFO && PszConfCtrlTypeName(wMsgType) )
    {
        // zbq [04/25/2007]
        TIME_SHOW
        OspPrintf( TRUE, FALSE, "[RADSTACK  %d]  conf call back, msgtype = %s\n", 
                                      wDstInst, PszConfCtrlTypeName(wMsgType) );
    }		

	if( wDstInst == 0 || wDstInst > MAXNUM_DRI_MT )
		return TRUE;	

	CServMsg cServMsg;
	cServMsg.SetMsgBody( (u8*)&pStackCall, sizeof(HCALL) );
	cServMsg.CatMsgBody( (u8*)&wType, sizeof(u16) );
	cServMsg.CatMsgBody( (u8*)pBuf, wBufLen ); //possibly truncated to fit in 1 ServMsg
	cServMsg.SetEventId( EV_RAD_CONFCTRL_NOTIF );

	g_cMtAdpApp.ProcH323CallBack( &cServMsg, wDstInst );
	
	return TRUE;
}


/*=============================================================================
  �� �� ���� EvRasCtrlMsg
  ��    �ܣ� RAS���ܻص�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����HAPPCALL	haCall,
			HCALL		hsCall, 
			HAPPRAS		haRas, 
			HRAS		hsRas,
			u16			wMsgType,
			const void*	pBuf, 
			u16			wBufLen
  �� �� ֵ�� s32       CALLBACK 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
static s32 CALLBACK EvRasCtrlMsg( HAPPCALL		haCall,
						   HCALL		hsCall, 
						   HAPPRAS		haRas, 
						   HRAS			hsRas,
						   u16			wMsgType,
						   const void*	pBuf, 
						   u16			wBufLen)
{
	HAPPRAS hAppRas		= haRas;
	HRAS	hStackRas   = hsRas;
	HCALL	hStackCall	= hsCall;
    
    /*lint -save -e507*/
    u16     wDstInst    = (u16)haCall;
    /*lint -restore*/
	u16		wType	= wMsgType;


	if( g_cMtAdpApp.m_byDebugLevel >= DEBUG_INFO && PszRasCtrlTypeName(wMsgType) )
    {
        // zbq [04/25/2007]
        TIME_SHOW
        OspPrintf( TRUE, FALSE, "[RADSTACK  %d]  ras call back, msgtype = %s\n", 
                                 wDstInst, PszRasCtrlTypeName(wMsgType) );
    }
    
    CServMsg cServMsg;
    cServMsg.SetMsgBody((u8*)&hAppRas, sizeof(hAppRas));
	cServMsg.CatMsgBody((u8*)&hStackRas, sizeof(hStackRas));
	cServMsg.CatMsgBody((u8*)&wType, sizeof(wType));

	if( wMsgType == (u16)h_ras_ACF || wMsgType == (u16)h_ras_ARJ || 
		wMsgType == (u16)h_ras_BCF || wMsgType == (u16)h_ras_BRJ || 
		wMsgType == (u16)h_ras_DRQ )
	{
		cServMsg.CatMsgBody( (u8*)&hStackCall, sizeof(hStackCall) );
	}

	cServMsg.CatMsgBody( (u8*)pBuf, wBufLen );	
	cServMsg.SetEventId( EV_RAD_RAS_NOTIF );

	g_cMtAdpApp.ProcH323CallBack( &cServMsg, CInstance::DAEMON );

	return TRUE;
}

/*=============================================================================
  �� �� ���� EvFeccMsg
  ��    �ܣ� ����ͷԶң���ƻص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����HCALL		hsCall, 
			HCHAN		hsChan, 
			TFeccStruct	tFeccStruct, 
			TTERLABEL	tTerSrc, 
			TTERLABEL	tTerDst
			u16			wBufLen
  �� �� ֵ��s32       CALLBACK 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
static s32	CALLBACK EvFeccMsg( HCALL		hsCall, 
					    HCHAN		hsChan, 
					    TFeccStruct	tFeccStruct, 
					    TTERLABEL	tTerSrc, 
					    TTERLABEL	tTerDst)
{	
	HCALL pStackCall = hsCall;
	TMt   tMt;
	u16   wType;
	u8    byAction;

	HAPPCALL haCall = NULL;
	kdvCallGetHandle( hsCall, &haCall );
    /*lint -save -e507*/
    u16 dstInst = (u16)haCall;
    /*lint -restore*/
	if( dstInst ==0 || dstInst > MAXNUM_DRI_MT )
		return TRUE;	
		
	//tMt.SetMt( tTerDst.GetMcuNo(), tTerDst.GetTerNo() );
	CMtAdpUtils::ConverTTERLABEL2TMt( tTerDst, tMt );

	CMtAdpUtils::CameraCommandOut2In( tFeccStruct, wType, byAction );

	CServMsg cServMsg;
	cServMsg.SetMsgBody( (u8*)&pStackCall, sizeof(HCALL) );
	cServMsg.CatMsgBody( (u8*)&wType, sizeof(wType) );
	cServMsg.CatMsgBody( (u8*)&tMt, sizeof(tMt) );
	cServMsg.CatMsgBody( &byAction, 1 );
	cServMsg.SetEventId( EV_RAD_FECCCTRL_NOTIF );
	
	OspPost( MAKEIID( AID_MCU_MTADP, dstInst ), 
			 cServMsg.GetEventId(),
			 cServMsg.GetServMsg(), 
			 cServMsg.GetServMsgLen());
	
	LogPrint(LOG_LVL_DETAIL,MID_MCULIB_MTADP,"[EvFeccMsg]hsChan.%p tTerSrc.GetTerNo().%d\n",hsChan,tTerSrc.GetTerNo());
	
	return TRUE;
}
 
/*=============================================================================
  �� �� ���� EvMMcuMsg
  ��    �ܣ� MCU������Ϣ���ƻص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� HAPPCALL	 haCall
             HCALL		 hsCall
             HAPPCHAN	 haChan
             HCHAN		 hsChan
             u16		 wMsgType
             const void* pBuf
             u16		 wBufLen
  �� �� ֵ�� s32		 CALLBACK 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
static s32 CALLBACK EvMMcuMsg( HAPPCALL	 haCall, 
						HCALL		 hsCall, 
						HAPPCHAN	 haChan, 
						HCHAN		 hsChan,
						u16			 wMsgType, 
						const void*  pBuf, 
						u16          wBufLen     )
{
    /*lint -save -e507*/
	u16 wDstInst = (u16)haCall;
    /*lint -restore*/
	if( wDstInst ==0 || wDstInst > MAXNUM_DRI_MT )
		return TRUE;	
	
    if( g_cMtAdpApp.m_byDebugLevel >= DEBUG_INFO && PszCascadeCtrlTypeName(wMsgType) )
    {
        // zbq [04/25/2007]
        TIME_SHOW
        OspPrintf( TRUE, FALSE, "[RADSTACK  %d]  cascade.%d callback, msgtype = %s\n", 
                                 wDstInst, hsChan, PszCascadeCtrlTypeName(wMsgType) );
    }

	CServMsg cServMsg;
	cServMsg.SetMsgBody( (u8*)&wMsgType, sizeof(wMsgType) );
	cServMsg.CatMsgBody( (u8*)&hsCall, sizeof(HCALL) );
	cServMsg.CatMsgBody( (u8*)&hsChan, sizeof(hsChan) );
	cServMsg.CatMsgBody( (u8*)pBuf, wBufLen );
	cServMsg.SetEventId( EV_RAD_MMCUCTRL_NOTIF );
	
	OspPost( MAKEIID( AID_MCU_MTADP, wDstInst ), 
			 cServMsg.GetEventId(),
			 cServMsg.GetServMsg(), 
			 cServMsg.GetServMsgLen() );
	
	LogPrint(LOG_LVL_DETAIL,MID_MCULIB_MTADP,"[EvMMcuMsg] event.%d\n", cServMsg.GetEventId());
		
	return TRUE;
}


/*=============================================================================
  �� �� ���� *PszCallCtrlTypeName
  ��    �ܣ� ���п����¼�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wType
  �� �� ֵ�� s8   
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
s8* PszCallCtrlTypeName( u16 wType )
{
	static s8 achName[50];
	switch( wType ) 
	{
#define CALL_CASE(x) case x: strncpy( achName, #x, sizeof(achName) ); break;
	
	CALL_CASE( h_call_make )
	CALL_CASE( h_call_incoming )
	CALL_CASE( h_call_answer )	
	CALL_CASE( h_call_drop )	
	CALL_CASE( h_call_connected )
	CALL_CASE( h_call_disconnected )
	CALL_CASE( h_call_facility )	
	default:	
		return NULL;
	}
	return achName;

#undef CALL_CASE
}

/*=============================================================================
  �� �� ���� *chanCtrlTypeName
  ��    �ܣ� �ŵ������¼�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wType
  �� �� ֵ�� s8   
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
s8* PszChanCtrlTypeName( u16 wType )
{
	static s8   achName[50];
	switch(wType) 
	{
#define CHAN_CASE(x) case x: strncpy(achName, #x, sizeof(achName)); break;
		
	CHAN_CASE( h_chan_open ) 
	CHAN_CASE( h_chan_openAck )
	CHAN_CASE( h_chan_openReject )
	CHAN_CASE( h_chan_connected )
	CHAN_CASE( h_chan_close )	
	CHAN_CASE( h_chan_parameter )
	CHAN_CASE( h_chan_remoteRtpAddress )
	CHAN_CASE( h_chan_remoteRtcpAddress )			
	CHAN_CASE( h_chan_flowControl )	
	default:	
		return NULL;
	}
return achName;

	#undef CHAN_CASE
}

/*=============================================================================
  �� �� ���� *confCtrlTypeName
  ��    �ܣ� ��������¼�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wType
  �� �� ֵ�� s8   
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
s8* PszConfCtrlTypeName( u16 wType )
{
	static s8   achName[50];
	switch( wType ) 
	{		
#define CONF_CASE(x) case x: strncpy( achName, #x, sizeof(achName) ); break;

	CONF_CASE( h_ctrl_h245TransportConnected )	
	CONF_CASE( h_ctrl_masterSlave )
	CONF_CASE( h_ctrl_masterSlaveDetermine )
	CONF_CASE( h_ctrl_capNotifyExt )
	CONF_CASE( h_ctrl_capResponse )
	CONF_CASE( h_ctrl_h245Established )
	CONF_CASE( h_ctrl_MultipointConference )		
	CONF_CASE( h_ctrl_communicationModeCommand )			
	CONF_CASE( h_ctrl_enterH243Password )		
	CONF_CASE( h_ctrl_passwordResponse ) 		
	CONF_CASE( h_ctrl_terminalNumberAssign )		
	CONF_CASE( h_ctrl_terminalListRequest )         		
	CONF_CASE( h_ctrl_terminalListResponse )       		
	CONF_CASE( h_ctrl_requestTerminalID )          		
	CONF_CASE( h_ctrl_mCTerminalIDResponse )       		
	CONF_CASE( h_ctrl_enterH243TerminalID )				
	CONF_CASE( h_ctrl_terminalIDResponse )          		
	CONF_CASE( h_ctrl_requestAllTerminalIDs )      		
	CONF_CASE( h_ctrl_requestAllTerminalIDsResponse ) 			
	CONF_CASE( h_ctrl_terminalJoinedConference )   		
	CONF_CASE( h_ctrl_terminalLeftConference )				
	CONF_CASE( h_ctrl_dropTerminal )              		
	CONF_CASE( h_ctrl_terminalDropReject )        			
	CONF_CASE( h_ctrl_dropConference )            			
	CONF_CASE( h_ctrl_requestForFloor )           			
	CONF_CASE( h_ctrl_floorRequested )            			
	CONF_CASE( h_ctrl_makeTerminalBroadcaster )   			
	CONF_CASE( h_ctrl_makeTerminalBroadcasterResponse )			
	CONF_CASE( h_ctrl_cancelMakeTerminalBroadcaster ) 				
	CONF_CASE( h_ctrl_seenByAll )		
	CONF_CASE( h_ctrl_cancelSeenByAll )      
	CONF_CASE( h_ctrl_seenByAtLeastOneOther )      		
	CONF_CASE( h_ctrl_cancelSeenByAtLeastOneOther ) 
	CONF_CASE( h_ctrl_sendThisSource )            		
	CONF_CASE( h_ctrl_sendThisSourceResponse )    		
	CONF_CASE( h_ctrl_cancelSendThisSource )		
	CONF_CASE( h_ctrl_terminalYouAreSeeing )
	CONF_CASE( h_ctrl_makeMeChair )         
	CONF_CASE( h_ctrl_cancelMakeMeChair )   
	CONF_CASE( h_ctrl_makeMeChairResponse )
	CONF_CASE( h_ctrl_withdrawChairToken ) 
	CONF_CASE( h_ctrl_requestChairTokenOwner )        
	CONF_CASE( h_ctrl_chairTokenOwnerResponse )	
	CONF_CASE( h_ctrl_mcLocationIndication )		
	CONF_CASE( h_ctrl_chairTransferReq )		
	CONF_CASE( h_ctrl_chairInviteTerminalReq )
	CONF_CASE( h_ctrl_terStatusReq )						
	CONF_CASE( h_ctrl_whoViewThisTerReq )		
	CONF_CASE( h_ctrl_confInfoReq )			
	CONF_CASE( h_ctrl_delayConfTimeReq )		
	CONF_CASE( h_ctrl_letTerViewMeReq )		
	CONF_CASE( h_ctrl_startVACReq )		
	CONF_CASE( h_ctrl_stopVACReq )		
	CONF_CASE( h_ctrl_startDiscussReq )	
	CONF_CASE( h_ctrl_stopDiscussReq )	
	CONF_CASE( h_ctrl_startVMPReq )		
	CONF_CASE( h_ctrl_stopVMPReq )		
	CONF_CASE( h_ctrl_changeVMPParamReq )	
	CONF_CASE( h_ctrl_getVMPParamReq )	
	CONF_CASE( h_ctrl_whoViewThisTerRsp )		
	CONF_CASE( h_ctrl_pollParamRsp )													
	CONF_CASE( h_ctrl_confInfoRsp )			
	CONF_CASE( h_ctrl_delayConfTimeRsp )		
	CONF_CASE( h_ctrl_letTerViewMeRsp )			
	CONF_CASE( h_ctrl_startVACRsp )			
	CONF_CASE( h_ctrl_stopVACRsp )			
	CONF_CASE( h_ctrl_startDiscussRsp )		
	CONF_CASE( h_ctrl_stopDiscussRsp )		
	CONF_CASE( h_ctrl_startVMPRsp )			
	CONF_CASE( h_ctrl_stopVMPRsp )			
	CONF_CASE( h_ctrl_changeVMPParamRsp )		
	CONF_CASE( h_ctrl_getVMPParamRsp )			
	CONF_CASE( h_ctrl_makeTerQuietCmd )		
	CONF_CASE( h_ctrl_makeTerMuteCmd )		
	CONF_CASE( h_ctrl_feccCmd )				
	CONF_CASE( h_ctrl_stopViewMeCmd )				
	CONF_CASE( h_ctrl_chairInviteFailedInd )	
	CONF_CASE( h_ctrl_confWillEndInd )		
	CONF_CASE( h_ctrl_delayConfTimeInd )      
	CONF_CASE( h_ctrl_terApplyChairInd )		
	CONF_CASE( h_ctrl_smsInd )				
	default:    
        return NULL;
	}
	return achName;

#undef CONF_CASE
}

/*=============================================================================
  �� �� ���� *rasCtrlTypeName
  ��    �ܣ� RAS�����¼�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wType
  �� �� ֵ�� s8   
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11       1.0			TanGuang                  ����
=============================================================================*/
s8* PszRasCtrlTypeName( u16 wType )
{
	static s8   achName[50];
	switch( wType ) 
	{
#define RAS_CASE(x) case x: strncpy(achName, #x, sizeof(achName)); break;
	
	RAS_CASE( h_ras_GRQ )
	RAS_CASE( h_ras_GCF )
	RAS_CASE( h_ras_GRJ )
	RAS_CASE( h_ras_RRQ )
	RAS_CASE( h_ras_RCF )
	RAS_CASE( h_ras_RRJ )
	RAS_CASE( h_ras_LWRRQ )
	RAS_CASE( h_ras_ARQ )
	RAS_CASE( h_ras_ACF )
	RAS_CASE( h_ras_ARJ )
	RAS_CASE( h_ras_BRQ )
	RAS_CASE( h_ras_BCF )
	RAS_CASE( h_ras_BRJ )
	RAS_CASE( h_ras_DRQ )
	RAS_CASE( h_ras_DCF )
	RAS_CASE( h_ras_DRJ )
	RAS_CASE( h_ras_URQ )
	RAS_CASE( h_ras_UCF )
	RAS_CASE( h_ras_URJ )
	default:	
		return NULL;
	}
	return achName;

#undef RAS_CASE
}


/*=============================================================================
  �� �� ���� PszCascadeCtrlTypeName
  ��    �ܣ� ���������¼�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wType
  �� �� ֵ�� s8   
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/26    4.0			�ű���                  ����
=============================================================================*/
s8* PszCascadeCtrlTypeName( u16 wType )
{
    static s8   achName[50];
    switch( wType ) 
    {
#define CASECAD_CASE(x) case x: strncpy(achName, #x, sizeof(achName)); break;
        
    CASECAD_CASE( H_CASCADE_REGUNREG_REQ )
    CASECAD_CASE( H_CASCADE_REGUNREG_RSP )
    CASECAD_CASE( H_CASCADE_NEWROSTER_NTF )
    CASECAD_CASE( H_CASCADE_PARTLIST_REQ )
    CASECAD_CASE( H_CASCADE_PARTLIST_RSP )
    CASECAD_CASE( H_CASCADE_INVITEPART_REQ )
    CASECAD_CASE( H_CASCADE_INVITEPART_RSP )
    CASECAD_CASE( H_CASCADE_NEWPART_NTF )
    CASECAD_CASE( H_CASCADE_CALL_ALERTING_NTF )
    CASECAD_CASE( H_CASCADE_REINVITEPART_REQ )
    CASECAD_CASE( H_CASCADE_REINVITEPART_RSP )
    CASECAD_CASE( H_CASCADE_DISCONNPART_REQ )
    CASECAD_CASE( H_CASCADE_DISCONNPART_RSP )
    CASECAD_CASE( H_CASCADE_DISCONNPART_NTF )
    CASECAD_CASE( H_CASCADE_DELETEPART_REQ )
    CASECAD_CASE( H_CASCADE_DELETEPART_RSP )
    CASECAD_CASE( H_CASCADE_DELETEPART_NTF )
    CASECAD_CASE( H_CASCADE_SETIN_REQ )
    CASECAD_CASE( H_CASCADE_SETIN_RSP )
    CASECAD_CASE( H_CASCADE_SETOUT_REQ )
    CASECAD_CASE( H_CASCADE_SETOUT_RSP )
    CASECAD_CASE( H_CASCADE_OUTPUT_NTF )
    CASECAD_CASE( H_CASCADE_NEWSPEAKER_NTF )
    CASECAD_CASE( H_CASCADE_AUDIOINFO_REQ )
    CASECAD_CASE( H_CASCADE_AUDIOINFO_RSP )
    CASECAD_CASE( H_CASCADE_VIDEOINFO_REQ )
    CASECAD_CASE( H_CASCADE_VIDEOINFO_RSP )
    CASECAD_CASE( H_CASCADE_CONFVIEW_CHG_NTF )
    CASECAD_CASE( H_CASCADE_PARTMEDIACHAN_REQ )
    CASECAD_CASE( H_CASCADE_PARTMEDIACHAN_RSP )
    CASECAD_CASE( H_CASCADE_PARTMEDIACHAN_NTF )
    CASECAD_CASE( H_CASCADE_GETMCUSERVICELIST_REQ )
    CASECAD_CASE( H_CASCADE_GETMCUSERVICELIST_RSP )
    CASECAD_CASE( H_CASCADE_GETMUCCONFLIST_REQ )
    CASECAD_CASE( H_CASCADE_GETMUCCONFLIST_RSP )
    CASECAD_CASE( H_CASCADE_GETCONFGIDBYNAME_REQ )
    CASECAD_CASE( H_CASCADE_GETCONFGIDBYNAME_RSP )
    CASECAD_CASE( H_CASCADE_GET_CONFPROFILE_REQ )
    CASECAD_CASE( H_CASCADE_GET_CONFPROFILE_RSP )
    CASECAD_CASE( H_CASCADE_SET_LAYOUT_AUTOSWITCH_REQUEST )
    CASECAD_CASE( H_CASCADE_SET_LAYOUT_AUTOSWITCH_RESPONSE )
    CASECAD_CASE( H_CASCADE_NONSTANDARD_REQ )
    CASECAD_CASE( H_CASCADE_NONSTANDARD_RSP )
    CASECAD_CASE( H_CASCADE_NONSTANDARD_NTF )
    CASECAD_CASE( H_CASCADE_NOT_SUPPORTED )
    CASECAD_CASE( H_CASCADE_VA_REQ )
    CASECAD_CASE( H_CASCADE_VA_RSP )
    CASECAD_CASE( H_CASCADE_RELEASECONTROL_OF_CONF_REQ )
    CASECAD_CASE( H_CASCADE_RELEASECONTROL_OF_CONF_RSP )
    CASECAD_CASE( H_CASCADE_TAKECONTROL_OF_CONF_REQ )
    CASECAD_CASE( H_CASCADE_TAKECONTROL_OF_CONF_RSP )
    CASECAD_CASE( H_CASCADE_ALLPART_SETOUT_REQ )
    CASECAD_CASE( H_CASCADE_ALLPART_SETOUT_RSP )
    CASECAD_CASE( H_CASCADE_CREATE_CONF_REQ )
    CASECAD_CASE( H_CASCADE_CREATE_CONF_RSP )
    CASECAD_CASE( H_CASCADE_TERMINATE_CONF_REQ )
    CASECAD_CASE( H_CASCADE_TERMINATE_CONF_RSP )
    CASECAD_CASE( H_CASCADE_UNDEFINED_REQ )
    CASECAD_CASE( H_CASCADE_UNDEFINED_RSP )
    CASECAD_CASE( H_CASCADE_UNDEFINED_NTF )

    default:	
        return NULL;
    }
    return achName;
    
#undef CASECAD_CASE
}


/************************************************************************/
/*                                                                      */
/*                               API                                    */
/*                                                                      */
/************************************************************************/

// API void rvstat()
// {	
// 	StaticLog( "cmSizeTpktChans Cur = %d, Max = %d  ", 
// 		cmSizeCurTpktChans(g_cMtAdpApp.m_hApp), cmSizeMaxTpktChans(g_cMtAdpApp.m_hApp) );
// 	
// 	StaticLog( "cmSizeMessages Cur = %d, Max = %d\n", 
// 		cmSizeCurMessages(g_cMtAdpApp.m_hApp), cmSizeMaxMessages(g_cMtAdpApp.m_hApp) );
// }

/*lint -restore*/
// API void rvstatStart()
// {
// 	s_bDorvstat = TRUE;
// }
// 
// API void rvstatStop()
// {
// 	s_bDorvstat = FALSE;
// }


// END OF FILE