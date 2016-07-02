/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcuvcvcs.cpp
   ����ļ�    : mcuvc.h
   �ļ�ʵ�ֹ���: MCU�������ҵ����
   ����        : ���㻪
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   08/12/08                ���㻪        ����
******************************************************************************/
 
#include "evmcumcs.h"
#include "evmcuvcs.h"
#include "evmcumt.h"
#include "evmcueqp.h"
#include "evmcu.h"
#include "evmcutest.h"
#include "evmcudcs.h"
#include "evmp.h"
#include "mcuvc.h"
#include "mcsssn.h"
#include "eqpssn.h"
#include "mcuerrcode.h"
#include "mtadpssn.h"
#include "mcuutility.h"
#include "mpmanager.h"
#include "radiusinterface.h"

#if defined(_VXWORKS_)
#include <inetLib.h>
#include "brddrvlib.h"
#elif defined(_LINUX_)
#include "boardwrapper.h"
#endif

/*====================================================================
    ������      ��ProcVcsMcuCreateConfReq
    ����        ��VCS���������鴦���� 
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪          ����

====================================================================*/
void CMcuVcInst::ProcVcsMcuCreateConfReq( const CMessage * pcMsg )
{
	TConfMode  tConfMode;
    u8 byMtNum = 0;
    u8 byCreateMtId;
    u8 byOldInsId = 0;
    u16 wErrCode = 0;
    CServMsg cMtMsg;

    CServMsg  cServMsg( pcMsg->content, pcMsg->length );

    TConfInfo tTmpConf = *(TConfInfo *)cServMsg.GetMsgBody();
    BOOL32 bConfFromFile = tTmpConf.m_tStatus.IsTakeFromFile();
    tTmpConf.m_tStatus.SetTakeFromFile(FALSE);

	// MCS VCS���黥��
// 	if (g_cMcuVcApp.GetConfNum(TRUE, TRUE, FALSE, MCS_CONF) > 0)
// 	{
// 		cServMsg.SetErrorCode(ERR_MCU_VCSMCS_ONETYPECONF);
// 		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
// 		ConfLog(FALSE, "[ProcMcsMcuCreateConfReq]Only one type conf at the same time\n");
// 		return;
// 	}

	switch( CurState() )
	{
	case STATE_IDLE:
		{
			//���ʵ��
			ClearVcInst();
			cServMsg.SetErrorCode(0);       

			//���ÿ�ʼʱ��
			tTmpConf.m_tStatus.SetOngoing();
			tTmpConf.SetStartTime( time( NULL ) );
      
			// �Ѵ���������������ʱ�������Ƿ���
			if (g_cMcuVcApp.IsConfNumOverCap(tTmpConf.m_tStatus.IsOngoing(), tTmpConf.GetConfSource()))
			{
				cServMsg.SetErrorCode( ERR_MCU_CONFNUM_EXCEED );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq]Conference %s failure because exceed max conf num!\n", tTmpConf.GetConfName() );
				return;
			}

			if (0 == g_cMcuVcApp.GetMpNum() || 0 == g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323))
			{
				cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq]Conference %s failure because No_Mp or No_MtAdp!\n", tTmpConf.GetConfName() );
				return;
			}
			
			//�����ն�������
			if( NULL == m_ptMtTable )
			{
				m_ptMtTable = new TConfMtTable;
			}

			memset( m_ptMtTable, 0, sizeof( TConfMtTable ) );
			
			m_dwSpeakerViewId  = OspTickGet();//102062000;//
			m_dwSpeakerVideoId = m_dwSpeakerViewId+100;//102052024;//
			m_dwVmpViewId      = m_dwSpeakerVideoId+100;
			m_dwVmpVideoId     = m_dwVmpViewId+100;
			m_dwSpeakerAudioId = m_dwVmpVideoId+100;
			m_dwMixerAudioId   = m_dwSpeakerAudioId+100;
			
			//�õ�������
			m_byCreateBy = cServMsg.GetSrcMtId();
        
			m_byConfIdx = g_cMcuVcApp.GetTemConfIdxByE164(tTmpConf.GetConfE164());                    
			if(0 == m_byConfIdx)
			{
				m_byConfIdx = g_cMcuVcApp.GetIdleConfidx();
				if(0 == m_byConfIdx)
				{                      
					ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] Conference %s failure for conf full!\n", tTmpConf.GetConfName() );
					return;
				}
			}           
			byOldInsId = g_cMcuVcApp.GetConfMapInsId(m_byConfIdx);
			g_cMcuVcApp.SetConfMapInsId(m_byConfIdx, (u8)GetInsID());

			//�õ�������Ϣ
			m_tConf = tTmpConf;        

			//zbq[12/04/2007] ������ģ�����֡�ʶ���
			if ( MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
				 !m_tConf.IsMainVidUsrDefFPS() )
			{
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] MVidType.%d with FPS.%d is unexpected, adjust it\n",
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
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] SVidType.%d with FPS.%d is unexpected, adjust it\n",
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
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] DSVidType.%d with FPS.%d is unexpected, adjust it\n",
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

			tConfMode = m_tConf.m_tStatus.GetConfMode();
			if(0 == g_cMcuAgent.GetGkIpAddr())
			{
				tConfMode.SetRegToGK(FALSE);
			}

			// �趨����Ķ�ʱ���м��
			if( m_tConf.m_tStatus.GetCallInterval() < MIN_CONF_CALLINTERVAL )
			{
				tConfMode.SetCallInterval( MIN_CONF_CALLINTERVAL );
			}
			else
			{
				tConfMode.SetCallInterval( m_tConf.m_tStatus.GetCallInterval() );
			}
			// �趨���鶨ʱ���еĴ��������ڵ��Ȼ����е����ж�ʱ�����նˣ���ȡ���޴κ���
			tConfMode.SetCallTimes( DEFAULT_CONF_CALLTIMES );
			m_tConf.m_tStatus.SetConfMode( tConfMode );
			
			m_tConf.m_tStatus.SetPrsing( FALSE );
       		m_tConfAllMtInfo.Clear();
			m_tConfAllMtInfo.m_tLocalMtInfo.SetConfIdx( m_byConfIdx );
         
			   // VCSͨ��ģ�崴�����û���ģ��Ļ���id
	//         if ( !bConfFromFile )
	//         {
	//             m_tConf.SetConfId( g_cMcuVcApp.MakeConfId(m_byConfIdx, 0, m_tConf.GetUsrGrpId(),
	// 				                                      m_tConf.GetConfSource()) );
	//         }
        
			// VCSĬ������PRS
		    TConfAttrb tAttrb = m_tConf.GetConfAttrb();
			tAttrb.SetPrsMode(TRUE);
			m_tConf.SetConfAttrb(tAttrb);

			//������Ϣ �߼����
			if ( !IsConfInfoCheckPass(cServMsg, m_tConf, wErrCode))
			{
				CreateConfSendMcsNack( byOldInsId, wErrCode, cServMsg );
				return;
			}

			//������������, �ڴ˴��������ʱ���е�ʵ������ֶ����е�ʵ��
			VCSConfDataCoordinate(cServMsg, byMtNum, byCreateMtId);
        
			//���������ģʽΪ���ն��Զ�������Ӧ�����ն�
			if(0 == byMtNum && m_tConf.GetConfAttrb().IsReleaseNoMt())
			{
				CreateConfSendMcsNack( byOldInsId, ERR_MCU_NOMTINCONF, cServMsg );
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] Conference %s create failed because on mt in conf!\n", m_tConf.GetConfName() );
				return;
			}

			// ���鴦��: ��ʱ����, ��ԤԼ����
			if( NULL == m_ptSwitchTable )
			{
				m_ptSwitchTable = new TConfSwitchTable;
			}
			if( NULL == m_ptConfOtherMcTable )
			{
				m_ptConfOtherMcTable = new TConfOtherMcTable;
			}
			
			if (m_ptSwitchTable != NULL)
			{
				memset( m_ptSwitchTable, 0, sizeof( TConfSwitchTable ) );
			}
			
			if (m_ptConfOtherMcTable != NULL)
			{
				memset( m_ptConfOtherMcTable, 0, sizeof( TConfOtherMcTable ) );
			}
			
			//���GKע����Ϣ����ʱģ�屾��ע��ɹ�
			m_tConf.m_tStatus.SetRegToGK( FALSE );

			//MCU�����������Ƿ���
			if ( !IsVCSEqpInfoCheckPass(wErrCode))
			{
				CreateConfEqpInsufficientNack( byCreateMtId, byOldInsId, wErrCode, cServMsg );
				return;
			}

			TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();

			if( tConfAttrib.IsDiscussConf() )
			{
				// ���� [4/29/2006] ����������ȫ�����
				StartMixing( mcuWholeMix );
			}

			//Ӧ�� 
			cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
			cServMsg.SetConfId( m_tConf.GetConfId() );
			if( m_byCreateBy == CONF_CREATE_MCS && !bConfFromFile )
			{
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
			}
            else if (m_byCreateBy == CONF_CREATE_MT && !bConfFromFile)
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
                
                // guzh [7/19/2007] ���͸�������
                SendMsgToMt(CONF_CREATE_MT, MCU_MT_CREATECONF_ACK, cAckMsg);
            }


			//��ӡ��Ϣ 
			if ( CONF_DATAMODE_VAONLY == m_tConf.GetConfAttrb().GetDataMode() )
			{
				//��Ѷ����
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] conference %s created and started!\n",m_tConf.GetConfName());
			}
			else if ( CONF_DATAMODE_VAANDDATA == m_tConf.GetConfAttrb().GetDataMode() )
			{
				//��Ѷ������
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] conference %s created and started with data conf function !\n", m_tConf.GetConfName());
			}
			else
			{
				//���ݻ���
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] conference %s created and started with data conf function Only !\n", m_tConf.GetConfName());
			}


			   //���浽�ļ�(���ϼ�MCU�ĺ��д����Ļ��鲻����)��N+1����ģʽ�����浽�ļ�
			   //VCS���鲻��Ҫ�����ļ����棬MCU������VCS���鲻�Զ����������ɵ���Ա����
	//         if ( CONF_CREATE_NPLUS == m_byCreateBy && MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
	//         {
	//             NPlusLog("[ProcVcsMcuCreateConfReq] conf info don't save to file in N+1 restore mode.\n");
	//         }
	//         else
	//         {
	//             g_cMcuVcApp.SaveConfToFile( m_byConfIdx, FALSE );
	//         }
															
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
					ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] charge postponed due to GetRegGKDriId.%d, ChargeRegOK.%d !\n",
									 g_cMcuVcApp.GetRegGKDriId(), g_cMcuVcApp.GetChargeRegOK() );
				}
			}

			// ���� [4/29/2006]��ԭ��֪ͨMCS�����ڱ��浽�ļ����档
			// �ƶ���������Ϊ�˱�֤Gk����Ϣ����ȷ�ġ�֪ͨ���л�ء�
			cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
			SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

			// �����նˣ���������Ѿ����
// 			if (bInviteMtNow)                            
			{
				InviteUnjoinedMt(cServMsg);
			}

			//��Trap��Ϣ
			TConfNotify tConfNotify;
			CConfId cConfId;
			cConfId = m_tConf.GetConfId( );
			memcpy( tConfNotify.m_abyConfId, &cConfId, sizeof(cConfId) );
			astrncpy( tConfNotify.m_abyConfName, m_tConf.GetConfName(),
					  sizeof(tConfNotify.m_abyConfName), MAXLEN_CONFNAME);
			SendTrapMsg( SVC_AGT_CONFERENCE_START, (u8*)&tConfNotify, sizeof(tConfNotify) );	
        
			//�����ն�״̬ˢ�µļ��
			m_tRefreshParam.dwMcsRefreshInterval = g_cMcuVcApp.GetMcsRefreshInterval();

			//��ʱ���޴κ���δ����Ķ�ʱ����ʵ��
			SetTimer( MCUVC_INVITE_UNJOINEDMT_TIMER, 1000*m_tConf.m_tStatus.GetCallInterval() );

			//��ʱͨ��MCS�����¼�MCU�ĵ�ǰý��Դ��Ϣ
			SetTimer( MCUVC_MCUSRC_CHECK_TIMER, TIMESPACE_MCUSRC_CHECK);
        
			u8 m_byUniformMode = tConfAttrib.IsAdjustUniformPack();
			cServMsg.SetMsgBody((u8 *)&m_byUniformMode, sizeof(u8));
			cServMsg.SetConfIdx(m_byConfIdx);
			SendMsgToAllMp(MCU_MP_CONFUNIFORMMODE_NOTIFY, cServMsg);

			//�ı����ʵ��״̬
			NEXTSTATE( STATE_ONGOING );                

			//������ػ��������ն���Ϣ
			m_tConfAllMtInfo.m_tLocalMtInfo.SetMcuId( LOCAL_MCUID );
			cServMsg.SetMsgBody( (u8 *)&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
			SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
        
			//��������ն˱� 
			SendMtListToMcs(LOCAL_MCUID);	

			// ���͸���ص�ǰ�ĵ��Ȼ����ģʽ
			// Ĭ�ϴ���ʱ��Ϊ��������ģʽ
			m_cVCSConfStatus.VCCDefaultStatus();
			VCSConfStatusNotif();
		}
           
		break;
	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	
	return;
}

/*====================================================================
    ������      ��ProcVcsMcuVCMTReq
    ����        ��VCS���������鴦���� 
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/08                ���㻪          ����

====================================================================*/
void CMcuVcInst::ProcVcsMcuVCMTReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	switch(CurState())
	{
	case STATE_ONGOING:
		{
			u8* pbyMsgBody = cServMsg.GetMsgBody();

			TMt tVCMT;
			memcpy(&tVCMT, pbyMsgBody, sizeof(TMt));
			pbyMsgBody += sizeof(TMt);

			u8 byVCType  = *(u8*)pbyMsgBody;
			pbyMsgBody += sizeof(u8); 

			//ǿ�� 
			u8 byForceCallType = *(u8*)pbyMsgBody; 
			ConfLog(FALSE, "[ProcVcsMcuVCMTReq] operate(vctype:%d, forcecalltype:%d) mt(mcuid:%d mtid:%d) req\n",
				    byVCType, byForceCallType, tVCMT.GetMcuId(), tVCMT.GetMtId());
			// �Ե�����Դ����Ч�Խ����ж�,�����������б��У��Ҳ�Ϊ��ʱ���еĹ̶��ǿɵ�����Դ(�������ǽ�����նˡ��¼�mcu)
			if (!m_tConfAllMtInfo.MtInConf(tVCMT.GetMcuId(), tVCMT.GetMtId())
				|| (CONF_CALLMODE_TIMER == VCSGetCallMode(tVCMT)))
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_NOVCSOURCE);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			// �ж���һ��������ȵ��ն��Ƿ���ɵ���
			TMt tOldReqVCMT = m_cVCSConfStatus.GetReqVCMT();
			if (!tOldReqVCMT.IsNull() && !(tOldReqVCMT == tVCMT))
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_VCMTING);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				cServMsg.SetMsgBody((u8*)&tOldReqVCMT, sizeof(TMt));
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}
			
			// ���浱ǰ������vcsssn��Ϣ
			u8 byScrId = cServMsg.GetSrcSsnId();
			if (byScrId != 0)
			{
				m_cVCSConfStatus.SetCurSrcSsnId(byScrId);
			}

			TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();

			// �Ե�����Դ�������������жϣ�������ᣬ�ڵ���ģʽ��Ĭ��Ϊ���йҶϲ���			
			BOOL byVCMTOnline = m_tConfAllMtInfo.MtJoinedConf(tVCMT.GetMcuId(), tVCMT.GetMtId());
			if (byVCMTOnline && !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
			{
				VCSDropMT(tVCMT);
			}

	
			switch(m_cVCSConfStatus.GetCurVCMode())
			{
			// ��������ģʽ��
			case VCS_SINGLE_MODE:
				{
					// ��ģʽ�£�����������߷ǵ����ն�
					if (byVCMTOnline && !(tCurVCMT == tVCMT))
					{
						OspPrintf(TRUE, FALSE, "[ProcVcsMcuVCMTReq] Error, non-curVCMT on line\n");
						cServMsg.SetEventId(cServMsg.GetEventId() + 2);
						SendReplyBack(cServMsg, cServMsg.GetEventId());
						return;
					}

					// �����ߵ����ն�, �Ҷ�ֱ�ӻ�ӦACK
					if (tVCMT == tCurVCMT)
					{
						cServMsg.SetEventId(cServMsg.GetEventId() + 1);
						SendReplyBack(cServMsg, cServMsg.GetEventId());
					}
					// �Է������ն˵��ȣ������µ����ն�
					else
					{		
						// ���õ�ǰ������ȵ��նˣ����гɹ���ŹҶϵ�ǰ�����ն�
						m_cVCSConfStatus.SetReqVCMT(tVCMT);
						VCSCallMT(cServMsg, tVCMT, byForceCallType);
					}
				}
				break;
			case VCS_MULTW_MODE:
				{
					// �����ߵ����е�����Դ���ȣ�ֱ��ɾ�����ն˶���������ҵ��
					if (byVCMTOnline)
					{
						cServMsg.SetEventId(cServMsg.GetEventId() + 1);
						SendReplyBack(cServMsg, cServMsg.GetEventId());
						return;
					}
					// �Բ����ߵĵ�����Դ�����к�����ѡ��
					else
					{
						m_cVCSConfStatus.SetReqVCMT(tVCMT);
						VCSCallMT(cServMsg, tVCMT, byForceCallType);
					}					

				}

				break;
			case VCS_MULVMP_MODE:
				{
					// �����ߵ����е�����Դ���ȣ�ֱ��ɾ�����ն˶���������ҵ��
					if (byVCMTOnline)
					{
						cServMsg.SetEventId(cServMsg.GetEventId() + 1);
						SendReplyBack(cServMsg, cServMsg.GetEventId());
						return;
					}

					// ���ڲ����ߵĵ�����Դ�����к��������ϳ�
					m_cVCSConfStatus.SetReqVCMT(tVCMT);
					VCSCallMT(cServMsg, tVCMT, byForceCallType);
				}
				break;
			case VCS_GROUPSPEAK_MODE:
			case VCS_GROUPTW_MODE:
			case VCS_GROUPVMP_MODE:
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
				if (VCS_VCTYPE_DROPMT == byVCType)
				{
					VCSDropMT(tVCMT);
				}
				else
				{
					if (!m_tConfAllMtInfo.MtJoinedConf(tVCMT.GetMcuId(), tVCMT.GetMtId()))
					{
						// ���������Զ����в����뷢����
						m_cVCSConfStatus.SetReqVCMT(tVCMT);
						VCSCallMT(cServMsg, tVCMT, byForceCallType);
					}
					else
					{
						// ���ģʽ���ٴε����ǰ�����ն�,��ոõ�ǰ�����ն�
						if (tVCMT == tCurVCMT)
						{
							tVCMT.SetNull();
						}
						ChgCurVCMT(tVCMT);
					}
				}
				break;
			default:
				ConfLog(FALSE, "[ProcVcsMcuVCMTReq] Wrong work mode\n");
				break;
			}
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	VCSConfStatusNotif();
}
/*====================================================================
    ������      ��ProcVcsMcuVCModeReq
    ����        ��VCS����ģʽ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/08                ���㻪          ����

====================================================================*/
void CMcuVcInst::ProcVcsMcuVCModeReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			u8 byMode = m_cVCSConfStatus.GetCurVCMode();
			cServMsg.SetMsgBody(&byMode, sizeof(u8));
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
	    break;
	}
	
}
/*====================================================================
    ������      ��ProcVcsMcuVCModeChgReq
    ����        ��VCS�޸ĵ���ģʽ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/08                ���㻪          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuVCModeChgReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			u8 byNewMode = *(u8 *)cServMsg.GetMsgBody();
			u8 byOldMode = m_cVCSConfStatus.GetCurVCMode();
			if ( byOldMode != byNewMode)
			{
				// �л�����ģʽ,���������Ƿ��㹻
				if (VCS_MULTW_MODE == byNewMode || VCS_GROUPTW_MODE == byNewMode)
				{
					if (!FindUsableTWChan(m_cVCSConfStatus.GetCurUseTWChanInd()))
					{
						// δ�ҵ����õ�ͨ����������ܾ�
						EqpLog("[ProcVcsMcuVCModeChgReq] Find no usable TW!\n");
						cServMsg.SetErrorCode(ERR_MCU_VCS_NOUSABLETW);
						SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
						return;
					}
				}
				else if (VCS_MULVMP_MODE == byNewMode || VCS_GROUPVMP_MODE == byNewMode)
				{
					// ��ѯ�Ƿ���ڿ��õ�VMP,���ھ���ռ��
					if (!FindUsableVMP())
					{
						// δ�ҵ����õĻ���ϳ�������ܾ�
						EqpLog("[ProcVcsMcuVCModeChgReq] Find no usable VMP!\n");
						cServMsg.SetErrorCode(ERR_MCU_VCS_NOUSABLEVMP);
						SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
						return;
					}		
				}
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

				// ���浱ǰ������vcsssn��Ϣ
				u8 byScrId = cServMsg.GetSrcSsnId();
				if (byScrId != 0)
				{
					m_cVCSConfStatus.SetCurSrcSsnId(byScrId);
				}

				// �ָ�����ʼ��Ϊ��ģʽ
				RestoreVCConf(byNewMode);
				cServMsg.SetMsgBody(&byNewMode, sizeof(u8));
				SendMsgToAllMcs(cServMsg.GetEventId() + 3, cServMsg);

			}
			else
			{
				VcsLog("[ProcVcsMcuVCModeChgReq] No change about vcmode\n");
				cServMsg.SetErrorCode(ERR_MCU_VCS_NOMODECHG);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			}
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfLog( FALSE, "[ProcVcsMcuVCModeChgReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
	    break;
	}
	
}
/*====================================================================
    ������      ��ProcVcsMcuMuteReq
    ����        ��VCS�޸ľ�����ģʽ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/08                ���㻪          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuMuteReq( const CMessage * pcMsg )
{
	VcsLog("[ProcVcsMcuMuteReq] %d(%s) passed\n", pcMsg->event, OspEventDesc(pcMsg->event));

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
		    if (cServMsg.GetMsgBodyLen() < sizeof(u8) * 4)
		    {
				ConfLog(FALSE, "[ProcVcsMcuMuteReq] message %u(%s) received with uncorrect length!\n",
					    pcMsg->event, ::OspEventDesc((pcMsg->event)));
				return;
		    }	
			
			u8 byMode = *(u8*)cServMsg.GetMsgBody();
			if(byMode != m_cVCSConfStatus.GetCurVCMode())
			{
				VcsLog("[ProcVcsMcuMuteReq] Mode changed, operation cancel\n");
				cServMsg.SetErrorCode(ERR_MCU_VCS_CANCEL);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
			
			u8  byMuteOpenFlag = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8));         //�Ƿ���
			u8  byOprObj       = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8) * 2);     //��������/Զ��
			u8  byAudProcType  = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8) * 3);     //������/����
			
			if (VCS_OPR_LOCAL == byOprObj)
			{
				// ���汾�ؾ�����״̬
				m_cVCSConfStatus.SetLocAudProcType(byMuteOpenFlag, byAudProcType);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

				// ���ؾ������������ϯ�ն�
				TMt tMt;
				tMt = m_tConf.GetChairman();
				if (!tMt.IsNull())
				{
					VCSMTMute(tMt, byMuteOpenFlag, byAudProcType);
				}
			}
			else
			{
				// ����Զ�˾�����״̬
				m_cVCSConfStatus.SetRemAudProcType(byMuteOpenFlag, byAudProcType);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

				// Զ�˾�����������������ߵĿɵ����ն� + �¼���mcu
				for (u8 byMTId = 1; byMTId <= MAXNUM_CONF_MT; byMTId++)
				{
					if (m_tConfAllMtInfo.MtJoinedConf(byMTId) && 
						(CONF_CALLMODE_NONE == m_ptMtTable->GetCallMode(byMTId) || MT_TYPE_SMCU == m_ptMtTable->GetMtType(byMTId)))
					{
						TMt tMt = m_ptMtTable->GetMt(byMTId);
						if (!tMt.IsNull())
						{
							VCSMTMute(tMt, byMuteOpenFlag, byAudProcType);
						}					
						
					}
				}
			}
			
			// ͨ�����״̬��Ϣ
			VCSConfStatusNotif();
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfLog( FALSE, "[ProcVcsMcuMuteReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

}
/*====================================================================
    ������      ��ProcVCMTOverTime
    ����        �������ն˴���ʱ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/08                ���㻪          ����

====================================================================*/
void CMcuVcInst::ProcVCMTOverTime(const CMessage * pcMsg)
{
	KillTimer(MCUVC_VCMTOVERTIMER_TIMER);

	TMt tReqVCMT = m_cVCSConfStatus.GetReqVCMT();
	if (!tReqVCMT.IsNull())
	{
		CServMsg cServMsg;
		cServMsg.SetConfId(m_tConf.GetConfId());
		cServMsg.SetConfIdx(m_byConfIdx);
		cServMsg.SetMsgBody((u8*)&tReqVCMT, sizeof(TMt));
		cServMsg.SetErrorCode(ERR_MCU_VCS_VCMTOVERTIME);
		SendMsgToAllMcs(MCU_VCS_VCMT_NOTIF, cServMsg);

		if (tReqVCMT.IsLocal())
		{
			RemoveJoinedMt( tReqVCMT, TRUE );
		}

		TMt tNull;
		tNull.SetNull();
		m_cVCSConfStatus.SetReqVCMT(tNull);
	}

}
/*====================================================================
    ������      ��ProcVcsMcuGroupCallMtReq
    ����        ������ն�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/04/11                ���㻪          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuGroupCallMtReq(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	CServMsg cServBackMsg;
	cServBackMsg.SetServMsg(pcMsg->content, SERV_MSGHEAD_LEN);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			SendReplyBack(cServBackMsg, cServMsg.GetEventId() + 1);

			// u8(����)+u8(��������)+s8�ַ���(����)+u8(�ն���)+TMt����
			u8* pbyMsg = (u8*)cServMsg.GetMsgBody();
			u8 byGroupNum = *pbyMsg++;

			u16 wErrCode       = 0;
			u8  byCallMtNum    = 0;
			u8  byGroupNameLen = 0;
			for (u8 byGroupIdx = 0; byGroupIdx < byGroupNum; byGroupIdx++)
			{
				byGroupNameLen = *pbyMsg++;

				if (!m_cVCSConfStatus.OprGroupCallList((s8*)pbyMsg, byGroupNameLen, TRUE, wErrCode))
				{
					cServMsg.SetMsgBody(pbyMsg, byGroupNameLen);
					cServMsg.SetErrorCode(wErrCode);
					SendReplyBack(cServBackMsg, cServMsg.GetEventId() + 3);
					continue;
				}

				pbyMsg += byGroupNameLen;
				byCallMtNum = *pbyMsg++;
				TMt* ptMt = (TMt*)pbyMsg;
				for (u8 byIdx = 0; byIdx < byCallMtNum; byIdx++)
				{
					if (m_tConfAllMtInfo.MtInConf(ptMt->GetMtId()) &&
						!m_tConfAllMtInfo.MtJoinedConf(ptMt->GetMtId()))
					{
						InviteUnjoinedMt(cServBackMsg, ptMt);
					}
					else
					{
						VcsLog("[ProcVcsMcuGroupCallMtReq]mtid(%d) not in mtlist of the conf or has joined the conf\n",
							   ptMt->GetMtId());
					}

					ptMt++;
					pbyMsg += sizeof(TMt);
				}			
			}

			VCSConfStatusNotif();
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfLog( FALSE, "[ProcVcsMcuGroupCallMtReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}
/*====================================================================
    ������      ��ProcVcsMcuGroupDropMtReq
    ����        ���Ҷ�����ն�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/04/11                ���㻪          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuGroupDropMtReq(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	CServMsg cServBackMsg;
	cServBackMsg.SetServMsg(pcMsg->content, SERV_MSGHEAD_LEN);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			SendReplyBack(cServBackMsg, cServMsg.GetEventId() + 1);

			// u8(����)+u8(��������)+s8�ַ���(����)+u8(�ն���)+TMt����
			u8* pbyMsg = (u8*)cServMsg.GetMsgBody();
			u8 byGroupNum = *pbyMsg++;

			u16 wErrCode       = 0;
			u8  byCallMtNum    = 0;
			u8  byGroupNameLen = 0;
			for (u8 byGroupIdx = 0; byGroupIdx < byGroupNum; byGroupIdx++)
			{
				byGroupNameLen = *pbyMsg++;

				if (!m_cVCSConfStatus.OprGroupCallList((s8*)pbyMsg, byGroupNameLen, FALSE, wErrCode))
				{
					cServMsg.SetMsgBody(pbyMsg, byGroupNameLen);
					cServMsg.SetErrorCode(wErrCode);
					SendReplyBack(cServBackMsg, cServMsg.GetEventId() + 3);
					continue;
				}

				pbyMsg += byGroupNameLen;
				byCallMtNum = *pbyMsg++;
				TMt* ptMt = (TMt*)pbyMsg;
				for (u8 byIdx = 0; byIdx < byCallMtNum; byIdx++)
				{
					if (m_tConfAllMtInfo.MtJoinedConf(ptMt->GetMtId()))
					{
						VCSDropMT(*ptMt);
					}
					else
					{
						VcsLog("[ProcVcsMcuGroupDropMtReq]mtid(%d) not joined the conf\n",
							   ptMt->GetMtId());
					}

					ptMt++;
					pbyMsg += sizeof(TMt);
				}			
			}

			VCSConfStatusNotif();

		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfLog( FALSE, "[ProcVcsMcuGroupDropMtReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}
/*====================================================================
    ������      ��ProcVcsMcuStartChairPollReq
    ����        ��������ϯ��ѯ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/04/11                ���㻪          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuStartChairPollReq(const CMessage *pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			if (m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP)
			{
				VcsLog("[ProcVcsMcuStartChairPollReq]Chairpoll has already started\n");
				cServMsg.SetErrorCode(ERR_MCU_VCS_CHAIRPOLLING);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			}

			SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

			u8 byPollIntval = *(u8*)cServMsg.GetMsgBody();
			m_cVCSConfStatus.SetChairPollState(VCS_POLL_START);
			m_cVCSConfStatus.SetPollIntval(byPollIntval);

			VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);
			TMt tPollMt = VCSGetNextPollMt();
			if (!tPollMt.IsNull())
			{
				TSwitchInfo tSwitchInfo;
				tSwitchInfo.SetSrcMt(tPollMt);
				tSwitchInfo.SetDstMt(m_tConf.GetChairman());
				tSwitchInfo.SetMode(MODE_VIDEO);
				VCSConfSelMT(tSwitchInfo);

			}
			m_cVCSConfStatus.SetCurChairPollMt(tPollMt);
			VCSConfStatusNotif();

			SetTimer(MCUVC_VCS_CHAIRPOLL_TIMER, 1000 * byPollIntval);
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfLog( FALSE, "[ProcVcsMcuStartChairPollReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}
/*====================================================================
    ������      ��ProcChairPollTimer
    ����        ��������ϯ��ѯ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/04/11                ���㻪          ����
====================================================================*/
void CMcuVcInst::ProcChairPollTimer(const CMessage *pcMsg)
{
	KillTimer(MCUVC_VCS_CHAIRPOLL_TIMER);

	if (!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
	{
		TMt tNull;
		tNull.SetNull();
		m_cVCSConfStatus.SetCurChairPollMt(tNull);
		m_cVCSConfStatus.SetChairPollState(VCS_POLL_STOP);
		return;
	}

	if (VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState())
	{
		return;
	}

	TMt tChairman = m_tConf.GetChairman();
	TMtStatus tMtState;
	m_ptMtTable->GetMtStatus(tChairman.GetMtId(), &tMtState);	
	if (!tMtState.GetSelectMt(MODE_VIDEO).IsNull())
	{
		VCSConfStopMTSel(tChairman, MODE_VIDEO);
	}

	TMt tPollMt = VCSGetNextPollMt();
	if (!tPollMt.IsNull())
	{
		TSwitchInfo tSwitchInfo;
		tSwitchInfo.SetSrcMt(tPollMt);
		tSwitchInfo.SetDstMt(tChairman);
		tSwitchInfo.SetMode(MODE_VIDEO);
		VCSConfSelMT(tSwitchInfo);
	}
	else
	{
		// �޿���ѯ���ն�, ��ϯ���Լ�, ֱ�������ն˻��߽�����ѯ
		NotifyMtReceive(tChairman, tChairman.GetMtId());
	}
	m_cVCSConfStatus.SetCurChairPollMt(tPollMt);
	VCSConfStatusNotif();

	SetTimer(MCUVC_VCS_CHAIRPOLL_TIMER, 1000 * m_cVCSConfStatus.GetPollIntval());
}
/*====================================================================
    ������      ��ProcVcsMcuStopChairPollReq
    ����        ��ֹͣ��ϯ��ѯ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/04/11                ���㻪          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuStopChairPollReq(const CMessage *pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
			KillTimer(MCUVC_VCS_CHAIRPOLL_TIMER);
			if (m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP)
			{
				TMt tNullMt;
				tNullMt.SetNull();
				m_cVCSConfStatus.SetChairPollState(VCS_POLL_STOP);
				m_cVCSConfStatus.SetCurChairPollMt(tNullMt);
				m_cVCSConfStatus.SetPollIntval(0);

				VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);

				VCSConfStatusNotif();
			}

			
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfLog( FALSE, "[ProcVcsMcuStopChairPollReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}
/*====================================================================
    ������      ��ProcVcsMcuAddMtReq
    ����        ����ʱ�����ն�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/04/11                ���㻪          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuAddMtReq(const CMessage *pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			TAddMtInfo* ptAddMtInfo = (TAddMtInfo*)cServMsg.GetMsgBody();
			
			//���ܺ��л��鱾��
			if (mtAliasTypeE164 == ptAddMtInfo->m_AliasType)
			{
				if (0 == strcmp(ptAddMtInfo->m_achAlias, m_tConf.GetConfE164()))
				{
					cServMsg.SetErrorCode(ERR_MCU_NOTCALL_CONFITSELF);
					SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
					VcsLog("[ProcVcsMcuAddMtReq]Cannot Call conf itself.\n");
					return;
				}
			}

			//�Ƿ񳬹���������
			if (!m_cVCSConfStatus.IsAddEnable())
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_OVERADDMTNUM);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				VcsLog("[ProcVcsMcuAddMtReq]over addmt num\n");
				return;
			}
			
			//���������ն��б�
			u8 byMtId = AddMt(*ptAddMtInfo, ptAddMtInfo->GetCallBitRate(), ptAddMtInfo->GetCallMode());
			VcsLog("[ProcVcsMcuAddMtReq]add mt-%d callmode-%d DialBitRate-%d type-%d alias-", 
					byMtId, ptAddMtInfo->GetCallMode(), ptAddMtInfo->GetCallBitRate(), 
					ptAddMtInfo->m_AliasType );
			if ( ptAddMtInfo->m_AliasType == mtAliasTypeTransportAddress )
			{
				VcsLog("%s:%d!\n", StrOfIP(ptAddMtInfo->m_tTransportAddr.GetIpAddr()), 
							       ptAddMtInfo->m_tTransportAddr.GetPort() );
			}
			else
			{
				VcsLog("%s!\n",	ptAddMtInfo->m_achAlias);
			}

			if (0 == byMtId)
			{
				VcsLog("[ProcVcsMcuAddMtReq]Fail to alloc mtid\n");
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}

			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

			TMt tMt = m_ptMtTable->GetMt( byMtId );
			if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
			{
				InviteUnjoinedMt( cServMsg, &tMt, TRUE, TRUE );
			}

			//������ػ��������ն���Ϣ
			cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
			SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

			//��������ն˱�
			SendMtListToMcs(LOCAL_MCUID);

			m_cVCSConfStatus.OprNewMt(tMt, TRUE);
			VCSConfStatusNotif();
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfLog( FALSE, "[ProcVcsMcuAddMtReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}
/*====================================================================
    ������      ��ProcVcsMcuDelMtReq
    ����        ����ʱɾ���ն�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/04/11                ���㻪          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuDelMtReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			TMt tMt = *(TMt*)cServMsg.GetMsgBody();
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

			if (!tMt.IsNull() && 
				m_cVCSConfStatus.OprNewMt(tMt, FALSE))
			{
				VcsLog("[ProcVcsMcuDelMtReq]drop mt(mcuid:%d, mtid:%d)\n", 
					   tMt.GetMcuId(), tMt.GetMtId());
				VCSDropMT(tMt);

				VCSConfStatusNotif();
			}
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfLog( FALSE, "[ProcVcsMcuDelMtReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}
/*====================================================================
    ������      ��ProcVcsMcuMsg
    ����        ��VCS�����Ϣ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/08                ���㻪          ����

====================================================================*/
void CMcuVcInst::ProcVcsMcuMsg( const CMessage * pcMsg )
{
	VcsLog("[ProcVcsMcuMsg] %d(%s) passed\n", pcMsg->event, OspEventDesc(pcMsg->event));

	// �жϱ�����ϯ�ն��Ƿ����ߣ�����������������κε��Ȳ���
	if (VCS_MCU_VCMT_REQ == pcMsg->event || VCS_MCU_CHGVCMODE_REQ == pcMsg->event)
	{
		CServMsg cServMsg(pcMsg->content, pcMsg->length);
		TMt tChairMan;
		tChairMan = m_tConf.GetChairman();
		if (tChairMan.IsNull() || 
			!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) &&
			m_byCreateBy != CONF_CREATE_MT &&
			!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
		{
			cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
			cServMsg.SetEventId(cServMsg.GetEventId() + 2);
			SendReplyBack(cServMsg, cServMsg.GetEventId());
			return;
		}
	}

	switch(pcMsg->event)
	{
	case MCU_SCHEDULE_VCSCONF_START:
		ProcVcsMcuCreateConfReq(pcMsg);
		break;
	case VCS_MCU_VCMT_REQ:
		ProcVcsMcuVCMTReq(pcMsg);
		break;
	case VCS_MCU_VCMODE_REQ:
		ProcVcsMcuVCModeReq(pcMsg);
	    break;
	case VCS_MCU_CHGVCMODE_REQ:
		ProcVcsMcuVCModeChgReq(pcMsg);
		break;
	case VCS_MCU_MUTE_REQ:
		ProcVcsMcuMuteReq(pcMsg);
		break;
	case VCS_MCU_GROUPCALLMT_REQ:
		ProcVcsMcuGroupCallMtReq(pcMsg);
		break;
	case VCS_MCU_GROUPDROPMT_REQ:
		ProcVcsMcuGroupDropMtReq(pcMsg);
		break;
	case VCS_MCU_STARTCHAIRMANPOLL_REQ:
		ProcVcsMcuStartChairPollReq(pcMsg);
		break;
	case VCS_MCU_STOPCHAIRMANPOLL_REQ:
		ProcVcsMcuStopChairPollReq(pcMsg);
		break;
	case VCS_MCU_ADDMT_REQ:
		ProcVcsMcuAddMtReq(pcMsg);
		break;
	case VCS_MCU_DELMT_REQ:
	    ProcVcsMcuDelMtReq(pcMsg);
		break;
	default:
	    break;
	}
}
/*====================================================================
    ������      ��ProcVcsMcuRlsMtMsg
    ����        ��VCS��ռ���ͷ��ն���Ϣ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/08                ���㻪          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuRlsMtMsg( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	if (cServMsg.GetMsgBodyLen() < sizeof(TMt))
	{
		ConfLog(FALSE, "[ProcVcsMcuRlsMtMsg] wrong msg lenth(real:%d, less than %d)\n",
			    cServMsg.GetMsgBodyLen(), sizeof(TMt));
		return;
	}
	u8* pbyMsgBody = cServMsg.GetMsgBody();
	TMt tMt = *(TMt*)pbyMsgBody;
	pbyMsgBody += sizeof(TMt);

	s8 abyConfName[MAXLEN_CONFNAME + 1] = {0};
	u8 byPermitOrNot = FALSE;
	memcpy(abyConfName, pbyMsgBody, min(MAXLEN_CONFNAME, cServMsg.GetMsgBodyLen() - sizeof(TMt)));
	if (VCS_MCU_RELEASEMT_ACK == pcMsg->event)
	{
		cServMsg.SetEventId(MCU_MT_RELEASEMT_ACK);
		byPermitOrNot = TRUE;
	}
	else if (VCS_MCU_RELEASEMT_NACK == pcMsg->event)
	{
		cServMsg.SetEventId(MCU_MT_RELEASEMT_NACK);
		byPermitOrNot = FALSE;
	}
	else
	{
		ConfLog(FALSE, "[ProcVcsMcuRlsMtMsg] receive un exist msg(%d)\n", pcMsg->event);
		return;
	}

	SendMsgToAllMcs(MCU_VCS_RELEASEMT_NOTIF, cServMsg);

	cServMsg.SetMsgBody(&byPermitOrNot, sizeof(byPermitOrNot));
	cServMsg.CatMsgBody((u8*)abyConfName,  strlen(abyConfName));
	SendMsgToMt(tMt.GetMtId(), cServMsg.GetEventId(), cServMsg);


}
/*====================================================================
    ������      ��VCSConfDataCoordinate
    ����        ��VCS���������������
	              ������ϯ(������ϯ�󶨵��ն�)���������ǽ�նˡ��¼�mcu��ȡ���޴ζ�ʱ����
				  ���������������ɵ����ն˲�ȡ�ֶ����еķ�ʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
void CMcuVcInst::VCSConfDataCoordinate(CServMsg &cServMsg, u8 &byMtNum, u8 &byCreateMtId)
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

    wAliasBufLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
    pszAliaseBuf = (char*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16));
    // �����ɵ��ȵ��ն��б�
    UnPackTMtAliasArray( pszAliaseBuf, wAliasBufLen, &m_tConf, 
                         tMtAliasArray, awMtDialRate, byMtNum );

	//�õ������ߵı���
	if( CONF_CREATE_MT == m_byCreateBy )
	{
        u16 wAliasBufLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
		s32 nLen = sizeof(TConfInfo)+sizeof(u16)+wAliasBufLen;
		if(m_tConf.GetConfAttrb().IsHasTvWallModule())
			nLen += sizeof(TMultiTvWallModule);
		if(m_tConf.GetConfAttrb().IsHasVmpModule())
			nLen += sizeof(TVmpModule);
		u8 byIsHDTWCfg = *(u8*)(cServMsg.GetMsgBody() + nLen);
		nLen += sizeof(u8);
		if (byIsHDTWCfg)
		{
			nLen += sizeof(THDTvWall);
		}
		u8 byIsVCSSMCUCfg = *(u8*)(cServMsg.GetMsgBody() + nLen);
		nLen += sizeof(u8);
		if (byIsVCSSMCUCfg)
		{
			u16 wAliasBufLen = ntohs(*(u16*)(cServMsg.GetMsgBody() + nLen));
			nLen += sizeof(u16) + wAliasBufLen;
		}

		if( cServMsg.GetMsgBodyLen() > nLen )
		{
			ptMtH323Alias = (TMtAlias *)(cServMsg.GetMsgBody()+nLen);
			ptMtE164Alias = (TMtAlias *)(cServMsg.GetMsgBody()+nLen)+1;
			ptMtAddr = (TMtAlias *)(cServMsg.GetMsgBody()+nLen)+2;
			byMtType = *(cServMsg.GetMsgBody()+nLen+sizeof(TMtAlias)*3);
		}
	}
	
	//�����ն����״̬ 
	m_tConfAllMtInfo.RemoveAllJoinedMt();

	for( byLoop=1; byLoop<=byMtNum; byLoop++)
	{
		//�ն˺������ʱ�����۴��ỹ�Ǽ������л��飬������������DRI������Ϣ
		if( CONF_CREATE_MT == m_byCreateBy && 1 == byLoop ) 
		{
			byMtId = AddMt(tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], CONF_CALLMODE_NONE);
            
            // zbq [12/19/2007] ֱ�Ӵ����ն����ڴ˲���IP������E164��������µ�©��
            if ( tMtAliasArray[byLoop-1].m_AliasType != mtAliasTypeTransportAddress )
            {
                m_ptMtTable->SetMtAlias( byMtId, ptMtAddr );
                m_ptMtTable->SetIPAddr( byMtId, ptMtAddr->m_tTransportAddr.GetIpAddr() );                    
            }

            // xsl [11/8/2006] ������ն˼�����1
            g_cMcuVcApp.IncMtAdpMtNum( cServMsg.GetSrcDriId(), m_byConfIdx, byMtId );
            m_ptMtTable->SetDriId(byMtId, cServMsg.GetSrcDriId());
		}
		else
		{
			// ���ɵ����ն���Ϣ���棬�����������б�
			// Ĭ��Ϊ��������ģʽ����������ģʽΪ�ֶ�
			byMtId = AddMt( tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], CONF_CALLMODE_NONE );
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
				m_ptMtTable->SetMtType(byMtId, MT_TYPE_MMCU);
				//�����ϼ�MCU������Ϊ�������նˣ��Ͽ����Զ�ɾ�����б�
				m_ptMtTable->SetNotInvited( byMtId, TRUE );
			}
			else
			{
				m_ptMtTable->SetMtType(byMtId, MT_TYPE_MT);
			}
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
		//������ϯ��Ҫ��������Ϊ���޴ζ�ʱ����
		m_ptMtTable->SetCallMode(byMtId, CONF_CALLMODE_TIMER);
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
	if( m_tConf.GetConfAttrb().IsHasTvWallModule() )
	{
        TPeriEqpStatus tTWStatus;
		TMultiTvWallModule *ptMultiTvWallModule = (TMultiTvWallModule *)pszMsgBuf;
        m_tConfEqpModule.SetMultiTvWallModule( *ptMultiTvWallModule );
        m_tConfEqpModule.SetTvWallInfo( *ptMultiTvWallModule );

        TTvWallModule tTvWallModule;
        u8 byMtIndex = 0;
        u8 byMemberType = 0;
        for( u8 byTvLp = 0; byTvLp < ptMultiTvWallModule->GetTvModuleNum(); byTvLp++ )
        {
            ptMultiTvWallModule->GetTvModuleByIdx(byTvLp, tTvWallModule);
            g_cMcuVcApp.GetPeriEqpStatus(tTvWallModule.m_tTvWall.GetEqpId(), &tTWStatus);
			for( byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++ )
			{
				byMtIndex = tTvWallModule.m_abyTvWallMember[byLoop]; 
                byMemberType = tTvWallModule.m_abyMemberType[byLoop];
				// VCS����ǽģ��Ŀǰ��ȡ��̬���ó�Ա�����ģ����ͨ����ָ����Ա������Ҫָ����Щͨ��ʹ��
				// ʹ��ʱ��ָ��ͨ����˳��ʹ�ã�������ͨ����Ϣ���浽����״̬��
                tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byMemberType;
                tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
				// ��֧��VCSԤָ��ͨ����Ա
				if( byMtIndex > 0 )//������1Ϊ��׼
				{
                    if( byMtIndex <= MAXNUM_CONF_MT )
                    {
                        byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAliasArray[byMtIndex-1] );
                        if( byMtId > 0 )
                        {
                            TMt tMt = m_ptMtTable->GetMt( byMtId );
                            m_tConfEqpModule.SetTvWallMemberInTvInfo( tTvWallModule.m_tTvWall.GetEqpId(), byLoop, tMt );
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
                            ConfLog(FALSE, "Invalid member type %d for idx %d in CreateConf TvWall Module\n", 
                                    byMemberType, 
                                    byLoop);        
                            bValid = FALSE;
                        }
                        if (bValid)
                        {
                            // ������������ն��������ᴥ����ϯ�����˱��
                            m_tConfEqpModule.SetTvWallMemberInTvInfo( tTvWallModule.m_tTvWall.GetEqpId(), byLoop, tMt );
                         }                            
                    }
                    else
                    {
                        ConfLog(FALSE, "Invalid MtIndex :%d in CreateConf TvWall Module\n", byMtIndex);
                    }
				}
			}
            g_cMcuVcApp.SetPeriEqpStatus(tTvWallModule.m_tTvWall.GetEqpId(), &tTWStatus);
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

	//������ϳ�ģ�� 
	if( m_tConf.GetConfAttrb().IsHasVmpModule() )
	{
        TVMPParam tVmpParam;
		memset( &tVmpParam, 0 ,sizeof(tVmpParam) );
        
		TVmpModule *ptVmpModule = (TVmpModule*)pszMsgBuf;
		tVmpParam = ptVmpModule->m_tVMPParam;
        u8 byMtIndex = 0;
        u8 byMemberType = 0;
		for( byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++ )
		{
			byMtIndex = ptVmpModule->m_abyVmpMember[byLoop];
            byMemberType = ptVmpModule->m_abyMemberType[byLoop];
			if( byMtIndex > 0 )
			{
                if(byMtIndex <= MAXNUM_CONF_MT)
                {
                    byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAliasArray[byMtIndex-1] );
                    if( byMtId > 0 )
                    {
                        TMt tMt = m_ptMtTable->GetMt( byMtId );
                        m_tConfEqpModule.SetVmpMember( byLoop, tMt );
                    }
                }	
                // ����� 193��˵�����������淽ʽ
                else if (byMtIndex == MAXNUM_CONF_MT + 1)
                {
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
                    case VMP_MEMBERTYPE_POLL:
                        // ��ѯ������ʱ�϶�û����ѯ
                        tMt.SetNull();
                        bValid = TRUE;
                        break;
                    default:
                        ConfLog(FALSE, "Invalid member type %d for idx %d in CreateConf Vmp Module\n", 
                                byMemberType, 
                                byLoop);        
                        bValid = FALSE;
                    }
                    if (bValid)
                    {
                        m_tConfEqpModule.SetVmpMember( byLoop, tMt );
                    }                          
                }
                else
                {
                    ConfLog(FALSE, "Invalid MtIndex :%d in CreateConf Vmp Module\n", byMtIndex);
                }
			}
		}
		m_tConfEqpModule.SetVmpModule( *ptVmpModule );
		pszMsgBuf += sizeof(TVmpModule);
	}
    else
    {
        TVmpModule tVmpModule;
        memset( &tVmpModule, 0, sizeof(tVmpModule) );
        m_tConfEqpModule.SetVmpModule( tVmpModule );
		TMt tNullMt;
        tNullMt.SetNull();
        for( byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++ )
		{
			m_tConfEqpModule.SetVmpMember( byLoop, tNullMt );
		}
    }

	// VCS: + 1byte(u8: 0 1  �Ƿ������˸������ǽ)
	//      +(��ѡ, THDTvWall])
	//      + 1byte(u8: ��������HDU��ͨ������)
	//      + (��ѡ, THduModChnlInfo+...)
	//      + 1byte(�Ƿ�Ϊ��������)��(��ѡ��2byte[u16 ���������ô�����ܳ���]+�¼�mcu����[1byte(��������)+1byte(��������)+xbyte(�����ַ���)+2byte(��������)...))
	//      + 1byte(�Ƿ�֧�ַ���)��(��ѡ��2byte(u16 ��������Ϣ�ܳ�)+����(1byte(����)+n��[1TVCSGroupInfo��m��TVCSEntryInfo])

	// �������ǽ
    if (*pszMsgBuf++)
    {
		m_tConfEqpModule.SetHDTvWall((THDTvWall*)pszMsgBuf);
		for( byLoop = 1; byLoop <= m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum; byLoop++)
		{
			// ��Ӹ��嵽�����б��У���Ϊ���޴ζ�ʱ����
			byMtId = AddMt( m_tConfEqpModule.m_tHDTvWall.m_atHDTWModuleAlias[byLoop-1], 
							m_tConfEqpModule.m_tHDTvWall.m_awHDTWDialBitRate[byLoop-1],
							CONF_CALLMODE_TIMER );
		}
		pszMsgBuf += sizeof(THDTvWall);
		
		byMtNum += m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum;
    }
	else
	{
		// δ���ø������ǽ����ղ���
		THDTvWall tNull;
		m_tConfEqpModule.SetHDTvWall(&tNull);
	}

	// HDU
	u8 byHduChnlNum = *pszMsgBuf++;
	if (byHduChnlNum)
	{
		m_tConfEqpModule.m_tHduModule.SetHduModuleInfo(byHduChnlNum, pszMsgBuf);
		pszMsgBuf += byHduChnlNum * sizeof(THduModChnlInfo);
	}
	else
	{
		m_tConfEqpModule.m_tHduModule.SetNull();
	}

	// �¼�MCU
	// ��ȡ���޴ζ�ʱ����
    if (*pszMsgBuf++)
    {
		u8 byDevNum = 0;
		wAliasBufLen = ntohs( *(u16*)pszMsgBuf );
		pszAliaseBuf = (char*)(pszMsgBuf + sizeof(u16));
		// ������Ϊ�������ǽ�ĸ����ն��б�
		UnPackTMtAliasArray( pszAliaseBuf, wAliasBufLen, &m_tConf, 
							 tMtAliasArray, awMtDialRate, byDevNum );

		for( byLoop = 1; byLoop <= byDevNum; byLoop++)
		{
			// ��Ӹ��嵽�����б��У���Ϊ���޴ζ�ʱ����
			byMtId = AddMt( tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], CONF_CALLMODE_TIMER );
		}
		pszAliaseBuf += sizeof(u16) + wAliasBufLen;
		
		byMtNum += byDevNum;
    }

    return;
}
/*====================================================================
    ������      ��IsVCSEqpInfoCheckPass
    ����        �������Ƿ�����Ҫ�󣬲�Ԥ������������Դ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16       &wErrCode : [OUT]����ִ�����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/05                ���㻪        ����
====================================================================*/
BOOL32 CMcuVcInst::IsVCSEqpInfoCheckPass(u16 &wErrCode)
{
    if ( m_tConf.GetConfId().IsNull() )
    {
        ConfLog( FALSE, "[IsVCSEqpInfoCheckPass] tConfInfo.IsNull, ignore it\n" );
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
            ConfLog( FALSE, "[IsVCSEqpInfoCheckPass]Conference %s create failed because of multicast address occupied!\n", m_tConf.GetConfName() );
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
            ConfLog( FALSE, "[IsVCSEqpInfoCheckPass]Conference %s create failed because of distributed conf multicast address occupied!\n", m_tConf.GetConfName() );
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
            ConfLog( FALSE, "[IsVCSEqpInfoCheckPass]Conference %s create failed because of no DCS registered!\n", m_tConf.GetConfName() );
            return FALSE;
        }
    }

	//������VCS���飬Ĭ��ģ������Ϊ��������������ʱ������Ԥ����������ʱ������Ҫ����
	//���ڻ���ϳ���������ǽ����������ԴЧ��Ԥ��

	//��Ҫ�����ش��Ļ���,Ԥ�����ش�ͨ�� 
	if (tConfAttrib.IsResendLosePack())
	{
		u8 byEqpId;
        u8 byChnIdx;
		TPeriEqpStatus tStatus;
        BOOL32 bNoIdlePrs = FALSE;
		u8 byNeedPrsChls = DEFAULT_PRS_CHANNELS;
		if(EQP_CHANNO_INVALID != m_byVidBasChnnl)
		{
			byNeedPrsChls += 1;
		}
		if(EQP_CHANNO_INVALID != m_byBrBasChnnl)
		{
			byNeedPrsChls += 1;
		}
		if(EQP_CHANNO_INVALID != m_byAudBasChnnl)
		{
			byNeedPrsChls += 1;
		}
		
		// if (g_cMcuVcApp.GetIdlePrsChl(byEqpId, byChnIdx, byChnIdx2, byChnIdxAud))
		TPrsChannel tPrsChannel;
		if( g_cMcuVcApp.GetIdlePrsChls( byNeedPrsChls, tPrsChannel) )
		{
			byEqpId = tPrsChannel.GetPrsId();
			m_tPrsEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_PRS);
			m_tPrsEqp.SetConfIdx(m_byConfIdx);
			byChnIdx = 0;
			m_byPrsChnnl = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			m_byPrsChnnl2 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			m_byPrsChnnlAud = tPrsChannel.m_abyPrsChannels[byChnIdx++];

			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
			//Ŀǰһ��PRS����Ϊ��������������޸�Ϊ��ÿ��PRSͨ���м�¼��ͨ������Ļ�������
			//zgc, 2007/04/24
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnl, m_byConfIdx );
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnl2, m_byConfIdx );
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlAud, m_byConfIdx );

			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl].SetReserved(TRUE);
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl2].SetReserved(TRUE);
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAud].SetReserved(TRUE);
			g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
			
            if(EQP_CHANNO_INVALID != m_byVidBasChnnl)
            {
                m_byPrsChnnlVidBas = tPrsChannel.m_abyPrsChannels[byChnIdx++];
                g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
                tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVidBas].SetReserved(TRUE);                        
                tStatus.SetConfIdx(m_byConfIdx);                        
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
            }                  

            if(EQP_CHANNO_INVALID != m_byBrBasChnnl)
            {
                m_byPrsChnnlBrBas = tPrsChannel.m_abyPrsChannels[byChnIdx++];
                g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
                tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlBrBas].SetReserved(TRUE);
                tStatus.SetConfIdx(m_byConfIdx);                        
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
            }   
            
            if(EQP_CHANNO_INVALID != m_byAudBasChnnl)
            {
                m_byPrsChnnlAudBas = tPrsChannel.m_abyPrsChannels[byChnIdx++];
                g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
                tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAudBas].SetReserved(TRUE);
                tStatus.SetConfIdx(m_byConfIdx);                        
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
            }                                        
            
			ConfLog(FALSE, "Prs eqp %d chl %d, chl2 %d, chlaud %d, chlVidBas %d, chlBrBas %d, chlAudBas %d reserved for conf %s\n",
                            byEqpId, m_byPrsChnnl,m_byPrsChnnl2,m_byPrsChnnlAud, m_byPrsChnnlVidBas, 
                            m_byPrsChnnlBrBas, m_byPrsChnnlAudBas, m_tConf.GetConfName());
		}
		else
		{
            bNoIdlePrs = TRUE;
        }
        
        // ������Դʧ��
		// VCS�����ش�δռ�õ�������ʾ,�Ǳ�����Դ
        if(bNoIdlePrs &&
		   m_tConf.GetConfSource() != VCS_CONF)
        {
            wErrCode = ERR_MCU_NOIDLEPRS;
			ConfLog( FALSE, "Conference %s create failed because no idle prs!\n", m_tConf.GetConfName() );
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
            ConfLog( FALSE, "Conf.%s not support multicast low stream\n", m_tConf.GetConfName() );
            return FALSE;                
        }
    }
#endif

    //���鿪ʼ�Զ�¼��
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
                ConfLog(FALSE, "Conf %s Request Recorder %d is not online now.\n", 
                                m_tConf.GetConfName(), 
                                tAutoRecAttrb.GetRecEqp());
                wErrCode = ERR_MCU_CONFSTARTREC_MAYFAIL;
				return FALSE;
            }

        #ifdef _MINIMCU_
            if ( ISTRUE(m_byIsDoubleMediaConf) && tAutoRecAttrb.GetRecParam().IsRecLowStream() )
            {
                wErrCode = ERR_MCU_RECLOWNOSUPPORT;
                ConfLog( FALSE, "Conf.%s can't support auto record low stream\n", m_tConf.GetConfName() );
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
        }
    }

	//�ѻ���ָ��浽������
	if( !g_cMcuVcApp.AddConf( this ) )
	{
		wErrCode = ERR_MCU_CONFOVERFLOW;
		ConfLog( FALSE, "Conference %s create failed because of full instance!\n", m_tConf.GetConfName() );
		return FALSE;
	}
    return TRUE;
}

/*====================================================================
    ������      ��VCSCallMT
    ����        ��VCS�����ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����cServMsgHdr: �������Ϣͷ
				  TMt:         ������ն�
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
void CMcuVcInst::VCSCallMT(CServMsg& cServMsgHdr, TMt& tMt, u8  byCallType)
{
	CServMsg cServMsg;
	cServMsg.SetServMsg(cServMsgHdr.GetServMsg(), SERV_MSGHEAD_LEN);
    cServMsg.SetConfIdx(m_byConfIdx) ;
    cServMsg.SetConfId(m_tConf.GetConfId());

	//���������ն��б���
    TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMcuId());
	if(tMtInfo.IsNull())
	{
		ConfLog(FALSE, "[VCSCallMT] Call MT(MCUID:%d MTID:%d) whose mcu is not in conf\n",
			           tMt.GetMcuId(), tMt.GetMtId());
		//�ն˲��ڿɵ��ȵ���Դ��Χ��
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINVC );
		cServMsg.SetEventId( cServMsg.GetEventId() + 2 );
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}

	if( tMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{	
		ConfLog(FALSE, "[VCSCallMT] Call MT(MCUID:%d MTID:%d) has in conf\n",
			           tMt.GetMcuId(), tMt.GetMtId());
		return;
	}

	TMtAlias tMtAlias;
    if (tMt.IsLocal() && 
		!m_ptMtTable->GetDialAlias( tMt.GetMtId(), &tMtAlias ))
    {
        cServMsg.SetErrorCode( ERR_MCU_CALLEDMT_NOADDRINFO);
		cServMsg.SetEventId( cServMsg.GetEventId() + 2 );
		SendReplyBack(cServMsg, cServMsg.GetEventId());
        ConfLog(FALSE, "[VCSCallMT] GetDialAlias failed, byMtId.%d\n", tMt.GetMtId());
        return;
	}

	cServMsg.SetEventId( cServMsg.GetEventId() + 1 );	
	SendReplyBack(cServMsg, cServMsg.GetEventId());

	if (VCS_FORCECALL_CMD == byCallType)
	{
		// ��ռ���ն˵������ʱ����Ϊ15s
		SetTimer( MCUVC_VCMTOVERTIMER_TIMER, 15000 );
	}
	else
	{
		// ���õ����ն˵����ʱ����5s��5s���ն�δ��������ʾ�����ն˳�ʱ
		SetTimer( MCUVC_VCMTOVERTIMER_TIMER, 5000 );
	}
	
	//���ڱ�MCU��
	if( !tMt.IsLocal())
	{
		cServMsg.SetEventId( cServMsg.GetEventId() + 1 );	
		SendReplyBack(cServMsg, cServMsg.GetEventId());

		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
			sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
		astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
			sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));
		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		SendMsgToMt( tMt.GetMcuId(), MCU_MCU_REINVITEMT_REQ, cServMsg );
		return;
	}
	else
	{
		//���뱾���ն�
		InviteUnjoinedMt(cServMsg, &tMt, FALSE, FALSE, byCallType);
	}




	return;
}

/*====================================================================
    ������      ��VCSDropMT
    ����        ��VCS�Ҷ��ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt:     ������ն�
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
void CMcuVcInst::VCSDropMT(TMt tMt)
{
	CServMsg cServMsg;
	cServMsg.SetSrcSsnId(m_cVCSConfStatus.GetCurSrcSsnId());
    cServMsg.SetConfIdx(m_byConfIdx);
    cServMsg.SetConfId(m_tConf.GetConfId()) ;

	//���������ն��б���
	TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMcuId());
	if(tMtInfo.IsNull())
	{
		ConfLog(FALSE, "[VCSDropMT] Del MT(MCUID:%d MTID:%d) whose mcu is not in conf\n",
			            tMt.GetMcuId(), tMt.GetMtId());
		return;
	}
	if(!tMtInfo.MtJoinedConf(tMt.GetMtId()))
	{	
		ConfLog(FALSE, "[VCSDropMT] Del MT(MCUID:%d MTID:%d) has not in conf\n",
			            tMt.GetMcuId(), tMt.GetMtId());
		return;
	}

	TMt tNullMT;
	tNullMT.SetNull();
	if (tMt == m_cVCSConfStatus.GetCurVCMT())
	{
		m_cVCSConfStatus.SetCurVCMT(tNullMT);
		if (VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
			!m_tConf.GetChairman().IsNull())
		{
			TMt tChairMan = m_tConf.GetChairman();
			if (m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMtId()))
			{
				NotifyMtReceive(tChairMan, tChairMan.GetMtId());
			}
		}

		// ���������������˼��������ǽģʽ, �Ҷϵ�ǰ�������Զ����ı����ն���������
		u8 byMode = m_cVCSConfStatus.GetCurVCMode();
		if (VCS_GROUPSPEAK_MODE == byMode || VCS_GROUPTW_MODE == byMode)
		{
			TMt tChairMan = m_tConf.GetChairman();
			ChangeSpeaker(&tChairMan, FALSE, FALSE);
		}
	}

	if (tMt == m_cVCSConfStatus.GetReqVCMT())
	{
		m_cVCSConfStatus.SetReqVCMT(tNullMT);
	}

	//���ն��Ƿ�ֱ���ն�
	if( !tMt.IsLocal())
	{
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy( tReq.m_szUserName, tMcsReg.m_achUser, 
				  sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser) );
		astrncpy( tReq.m_szUserPwd, tMcsReg.m_achPwd, 
				  sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd) );

		cServMsg.SetEventId(MCU_MCU_DROPMT_REQ);
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody((u8 *)&tMt, sizeof(TMt));
		SendMsgToMt( tMt.GetMcuId(), MCU_MCU_DROPMT_REQ, cServMsg );

		TMt tSMCU = m_ptMtTable->GetMt(tMt.GetMcuId());
		if (m_cVCSConfStatus.GetCurVCMode() != VCS_MULVMP_MODE)
		{
			// ȡ��ѡ��
			VCSConfStopMTSel(tSMCU, MODE_BOTH);
			// ȡ����ѡ��
			VCSConfStopMTSeled(tSMCU, MODE_BOTH);
		}

		// ���ش�ͨ���Ÿ��նˣ������Ϣ
		TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMcuId());
		if (ptMcInfo != NULL && tMt == ptMcInfo->m_tMMcuViewMt)
		{
			VCSConfStopMTToTW(ptMcInfo->m_tMMcuViewMt);
			ptMcInfo->m_tMMcuViewMt.SetNull();		
		}

		return;
	}		


	//�Ҷϱ����ն�ǰ���Ƚ��ն˻ָ��ɷǾ�����״̬
	//��Ϊ�����նˣ���Ҫ�ָ��ֱ���
	if (tMt.IsLocal())
	{
		VCSMTMute(tMt, FALSE, VCS_AUDPROC_MUTE);
		VCSMTMute(tMt, FALSE, VCS_AUDPROC_SILENCE);
		TLogicalChannel tSrcRvsChannl;
		if ( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
		{
			if ( tSrcRvsChannl.GetChannelType() == MEDIA_TYPE_H264 &&
				( IsVidFormatHD( tSrcRvsChannl.GetVideoFormat() ) ||
				  tSrcRvsChannl.GetVideoFormat() == VIDEO_FORMAT_4CIF ) )
			{
				ChangeVFormat(tMt, tSrcRvsChannl.GetVideoFormat());
			}
		}
	}
	
	RemoveJoinedMt( tMt, TRUE );		
}


/*====================================================================
    ������      ��VCSConfSelMT
    ����        ��VCS����ѡ���ն�, �������ȣ���������Ƶѡ�����෽������Ƶѡ�������л���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��BOOL
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
BOOL CMcuVcInst::VCSConfSelMT( TSwitchInfo tSwitchInfo )
{    
 
    if (CurState() != STATE_ONGOING)
    {
		CfgLog("[VCSConfSelMT] InstID(%d) in wrong state(%d)\n", GetInsID(), CurState());
		return FALSE;
    }
 	CServMsg  cServMsg;
	cServMsg.SetSrcSsnId(m_cVCSConfStatus.GetCurSrcSsnId());
    cServMsg.SetConfIdx(m_byConfIdx);
    cServMsg.SetConfId(m_tConf.GetConfId()) ;

    // ���ѡ���ն˻�ѡ���ն�Ϊ�գ���ֱ�ӷ���
	TMt	tDstMt, tSrcMt;
    tSrcMt = tSwitchInfo.GetSrcMt();
    tDstMt = tSwitchInfo.GetDstMt();
    if ( tDstMt.IsNull() || tSrcMt.IsNull())
    {
        ConfLog(FALSE, "[VCSConfSelMT] Mt(%d,%d) Select see Mt(%d,%d), returned with nack!\n", 
                tDstMt.GetMcuId(), 
                tDstMt.GetMtId(),
                tSrcMt.GetMcuId(), 
                tSrcMt.GetMtId());
        return FALSE;
    }    

	// ����ѡ�����ն˾�����
	TMt tOrigDstMt = tDstMt;
	if( TYPE_MT == tDstMt.GetType() )
	{
		ConfLog( FALSE, "[VCSConfSelMT] Mt(%d,%d) select see Mt(%d,%d), selmode(%d)\n", 
			     tDstMt.GetMcuId(), tDstMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSwitchInfo.GetMode() );

		tDstMt = GetLocalMtFromOtherMcuMt( tDstMt );				
		tDstMt = m_ptMtTable->GetMt(tDstMt.GetMtId());
		tSwitchInfo.SetDstMt( tDstMt );	
		
        //Ŀ���ն�δ�����
        if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
        {
            ConfLog( FALSE, "[VCSConfSelMT] Specified Mt(%u,%u) not joined conference!\n", 
                     tDstMt.GetMcuId(), tDstMt.GetMtId() );
			//VCSѡ��ҵ����û���������,������ʾ
//          cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
// 			SendMsgToAllMcs(MCU_VCS_VCMT_NOTIF, cServMsg);
            return FALSE;
        }   
	}
 
    TMt tOrigSrcMt = tSrcMt;
	if ( TYPE_MT == tSrcMt.GetType() )
	{
		// ��Ϊ�¼��նˣ���ѡ��ԭ�޸�Ϊ�����ڵ��¼�MCU
		tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);
		tSwitchInfo.SetSrcMt( tSrcMt );
		//Դ�ն�δ����飬NACK
		if( !m_tConfAllMtInfo.MtJoinedConf( tSrcMt.GetMtId() ) )
		{
			ConfLog( FALSE, "[VCSConfSelMT] Select source Mt(%u,%u) has not joined current conference!\n",
					 tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
			//VCSѡ��ҵ����û���������,������ʾ
// 			cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
// 			SendMsgToAllMcs(MCU_VCS_VCMT_NOTIF, cServMsg);
			return FALSE;
		}  
	}

    //�ն�״̬��ѡ��ģʽ�Ƿ�ƥ��,��ƥ��NACK
	TMtStatus tSrcMtStatus;
    TMtStatus tDstMtStatus;   
    u8 bySrcMtID = tSrcMt.GetMtId();
	u8 byDstMtId = tDstMt.GetMtId();
	u8 bySwitchMode    = tSwitchInfo.GetMode();
	m_ptMtTable->GetMtStatus(bySrcMtID, &tSrcMtStatus);
    m_ptMtTable->GetMtStatus(byDstMtId, &tDstMtStatus);
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
		//VCSѡ��ҵ����û���������,������ʾ
// 		if (MODE_AUDIO == tSwitchInfo.GetMode())
// 		{
//  		cServMsg.SetErrorCode(ERR_MCU_VCS_NOVCMTAUD);
// 		}
// 		else if (MODE_VIDEO == tSwitchInfo.GetMode())
// 		{
// 			cServMsg.SetErrorCode(ERR_MCU_VCS_NOVCMTVID);
// 		}
// 		else
// 		{
// 	        cServMsg.SetErrorCode( ERR_MCU_SRCISRECVONLY );
// 		}
// 		SendMsgToAllMcs( MCU_VCS_VCMT_NOTIF, cServMsg );

		ConfLog(FALSE, "[VCSConfSelMT] fail to sel(%d) because of the openning of dual logical channel\n",
			    tSwitchInfo.GetMode());
        return FALSE;
	}
	else
	{
		tSwitchInfo.SetMode(bySwitchMode);
	}

	//ѡ��ģʽ�ͻ���״̬�Ƿ�ƥ��
	//����ʱ����ѡ��ģʽ
	if( m_tConf.m_tStatus.IsMixing() )
	{
        //ѡ����Ƶ
		if( MODE_AUDIO == bySwitchMode )
		{
			ConfLog( FALSE, "[VCSConfSelMT] Conference %s is mixing now. Cannot switch only audio!\n", 
                     m_tConf.GetConfName() );
// 			cServMsg.SetErrorCode( ERR_MCU_SELAUDIO_INMIXING );
// 			SendMsgToAllMcs( MCU_VCS_VCMT_NOTIF, cServMsg );
            return FALSE;
		}
        //������תΪѡ����Ƶ
		else
		{
            //ѡ������Ƶ����ʾ��Ƶѡ��ʧ��
            if ( MODE_BOTH == bySwitchMode )
            {
                ConfLog( FALSE, "[VCSConfSelMT] Conference %s is mixing now. Cannot switch audio!\n", 
                         m_tConf.GetConfName() );
//              cServMsg.SetErrorCode( ERR_MCU_SELBOTH_INMIXING );
// 				SendMsgToAllMcs( MCU_VCS_VCMT_NOTIF, cServMsg );
				bySwitchMode = MODE_VIDEO;
            }
			tSwitchInfo.SetMode( bySwitchMode );
		}
	}

    // �����ն���Ƶѡ����Ҫ�ж����ߵ���Ƶ�����Ƿ�ƥ��
	BOOL byIsNeedHDAdpt= IsNeedSelApt(bySrcMtID, byDstMtId, bySwitchMode);
	if (byIsNeedHDAdpt && MODE_VIDEO == bySwitchMode &&
		StartHdVidSelAdp(tOrigSrcMt, tDstMt, bySwitchMode))
	{
	    VcsLog("[VCSConfSelMT] mt(mcuid:%d, mtid:%d) sel(mode:%d) mt(mcuid:%d, mtid:%d) using hdadp suc\n",
			   tDstMt.GetMcuId(), tDstMt.GetMtId(), bySwitchMode, tOrigSrcMt.GetMcuId(), tOrigSrcMt.GetMtId());
	}
	else if (byIsNeedHDAdpt && MODE_AUDIO == bySwitchMode &&
		     StartAudSelAdp(tOrigSrcMt, tDstMt))
	{
	    VcsLog("[VCSConfSelMT] mt(mcuid:%d, mtid:%d) sel(mode:%d) mt(mcuid:%d, mtid:%d) using audadp suc\n",
			   tDstMt.GetMcuId(), tDstMt.GetMtId(), bySwitchMode, tOrigSrcMt.GetMcuId(), tOrigSrcMt.GetMtId());

	}
    else if (!byIsNeedHDAdpt)
    {
		VCSSwitchSrcToDst( tSwitchInfo );
    }
	else
	{
		if(tDstMt == m_tConf.GetChairman())
		{
			if (MODE_VIDEO == bySwitchMode)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCNOACCEPTVIDEO);
			}

			if (MODE_AUDIO == bySwitchMode)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCNOACCEPTAUDIO);
			}
		}
		else
		{
			if (MODE_VIDEO == bySwitchMode)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_REMNOACCEPTVIDEO);
			}
			
			if (MODE_AUDIO == bySwitchMode)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_REMNOACCEPTAUDIO);
			}
		}
		if (cServMsg.GetErrorCode() != 0)
		{
			SendMsgToAllMcs(MCU_VCS_VCMT_NOTIF, cServMsg);
		}
		VcsLog("[VCSConfSelMT]mt(mcuid:%d, mtid:%d) sel mt(mcuid:%d, mtid:%d) need adp, but fail to start adp\n",
			    tDstMt.GetMcuId(), tDstMt.GetMtId(), tOrigSrcMt.GetMcuId(), tOrigSrcMt.GetMtId());
		return FALSE;
	}

    //����ѡ��������ѡ����setin
    if ( !tOrigSrcMt.IsLocal() )
    {
        //�鿴MC
        TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tOrigSrcMt.GetMcuId());
        if(ptMcInfo == NULL)
        {
			ConfLog( FALSE, "[VCSConfSelMT] MCU(mcuid:%d) Info(ptMcInfo) is NULL!\n",
				     tOrigSrcMt.GetMcuId());
            return FALSE;
        }
        //����ѡ��Դ
        TMt tMt;
        tMt.SetMcuId(tOrigSrcMt.GetMcuId());
        tMt.SetMtId(0);
        TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
        if(ptStatus == NULL)
        {
			ConfLog( FALSE, "[VCSConfSelMT] MCU(mcuid:%d) status(ptStatus) is NULL!\n",
				     tOrigSrcMt.GetMcuId());
            return FALSE;
        }
    
        OnMMcuSetIn( tOrigSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
    }

    // ����ѡ������ѡ����setout
    if( tOrigDstMt.GetMtType() == MT_TYPE_SMCU || tOrigDstMt.GetMtType() == MT_TYPE_MMCU )
    {
        TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tDstMt.GetMtId());
        if(ptConfMcInfo != NULL)
        {
            ptConfMcInfo->m_tSpyMt = tSrcMt;
            //send output ��Ϣ
            TSetOutParam tOutParam;
            tOutParam.m_nMtCount = 1;
            tOutParam.m_atConfViewOutInfo[0].m_tMt = tDstMt;
            tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
            tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;
            CServMsg cServMsg2;
            cServMsg2.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
            cServMsg2.SetEventId(MCU_MCU_SETOUT_NOTIF);
            SendMsgToMt(tDstMt.GetMtId(), MCU_MCU_SETOUT_NOTIF, cServMsg2);
        }
    }

    // ����ѡ���ն�(��VCS���ն�)��״̬
    if( MODE_AUDIO == tSwitchInfo.GetMode() || 
        MODE_VIDEO == tSwitchInfo.GetMode() ||
        MODE_BOTH  == tSwitchInfo.GetMode() )
    {
        m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tDstMtStatus ); // ѡ��
        tDstMtStatus.SetSelectMt( tOrigSrcMt, tSwitchInfo.GetMode() );
        m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );
    }

    //�ж�ѡ�����Ƿ��������
	u16 wBitrate = 0;
	TLogicalChannel tLogicalChannel;
    if (!byIsNeedHDAdpt && IsNeedAdjustMtSndBitrate(tSrcMt.GetMtId(), wBitrate) &&
		m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE) &&
		wBitrate < tLogicalChannel.GetFlowControl())
    {
        Mt2Log("[VCSConfSelMT] mt.%d, mode %d, old flowcontrol :%d, min flowcontrol :%d\n",
                tSrcMt.GetMtId(), 
                tLogicalChannel.GetMediaType(), tLogicalChannel.GetFlowControl(), wBitrate);                   
        
        tLogicalChannel.SetFlowControl(wBitrate);
        cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
		SendMsgToMt(tSrcMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
    }


    return TRUE;
}
/*====================================================================
    ������      ��VCSConfStopMTSel
    ����        ��VCS�����tDstMt�ն�ֹͣѡ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tDstMt:ѡ���ն�
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
void CMcuVcInst::VCSConfStopMTSel( TMt tDstMt, u8 byMode )
{	
    //change mode if mixing
    if ( MODE_BOTH == byMode )
    {
        if( m_tConf.m_tStatus.IsMixing() )
        {
            byMode = MODE_VIDEO;
        }
    }
	 
    ConfLog(FALSE, "[VCSConfStopMTSel] Mcu%dMt%d Cancel Select See.\n", tDstMt.GetMcuId(), tDstMt.GetMtId() );
    
    // ѡ����Ŀ��������¼��ն�
    tDstMt = GetLocalMtFromOtherMcuMt(tDstMt);

	if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
	{
		ConfLog( FALSE, "[VCSConfStopMTSel] Dst Mt%u-%u not joined conference!\n", 
			             tDstMt.GetMcuId(), tDstMt.GetMtId() );
		return;
	}

	// ���ѡ��״̬
	TMtStatus tMtStatus; 
	m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
	if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
	{
		tMtStatus.RemoveSelByMcsMode( MODE_VIDEO );

		// ���µ���ѡ���ߵķ�������, Ŀǰֻ֧�ֵ�������ѡ���ߵ�����
		TMt tVidSrc = tMtStatus.GetSelectMt( MODE_VIDEO );
		if (!tVidSrc.IsNull() && tVidSrc.IsLocal())
		{
			TMtStatus tSrcStatus;
			m_ptMtTable->GetMtStatus(tVidSrc.GetMtId(), &tSrcStatus);
			if ( m_tConfAllMtInfo.MtJoinedConf( tVidSrc.GetMtId() ) 
				&& tSrcStatus.IsSendVideo() 
			   )
			{
				NotifyMtSend( tVidSrc.GetMtId(), MODE_VIDEO );
			}
		}
	}

	if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
	{
		tMtStatus.RemoveSelByMcsMode( MODE_AUDIO );

		// ���µ���ѡ���ߵķ�������, Ŀǰֻ֧�ֵ�������ѡ���ߵ�����
		TMt tAudSrc = tMtStatus.GetSelectMt( MODE_AUDIO );
		if (!tAudSrc.IsNull() && tAudSrc.IsLocal())
		{
			TMtStatus tSrcStatus;
			m_ptMtTable->GetMtStatus(tAudSrc.GetMtId(), &tSrcStatus);
			if ( m_tConfAllMtInfo.MtJoinedConf( tAudSrc.GetMtId() ) 
				&& tSrcStatus.IsSendAudio() 
			   )
			{
				NotifyMtSend( tAudSrc.GetMtId(), MODE_AUDIO );
			}
		}
	}

	// ���ʽѡ���ͷŶ�Ӧ��������Դ
	ReleaseResbySel(tDstMt, byMode);
	// ���ѡ�����ն������Ϣ
    TMt tNullMt;
	tNullMt.SetNull();
    if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
    {
		tMtStatus.SetSelectMt(tNullMt, MODE_VIDEO);
    }
	if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
	{
		tMtStatus.SetSelectMt(tNullMt, MODE_AUDIO);
	}
	m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tMtStatus );
 
	// ����ѡ��������ƵԴ
	RestoreRcvMediaBrdSrc( tDstMt.GetMtId(), byMode );

	// ����֪ͨ��ѡ���ն˵�״̬�仯
    MtStatusChange( tDstMt.GetMtId(), TRUE );

    return;
}

/*====================================================================
    ������      ��VCSConfStopMTSeled
    ����        ��VCS����tSrcMt�ն˱�ѡ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tSrcMt:��ѡ���ն�
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
void CMcuVcInst::VCSConfStopMTSeled(TMt tSrcMt, u8 byMode)
{
	ConfLog(FALSE, "[VCSConfStopMTSeled]Stop all mt sel spec mt(mcuid:%d, mtid:%d)\n", 
		    tSrcMt.GetMcuId(), tSrcMt.GetMtId());

	// ��������ѡ�����ն˵�ʵ�壬�𽻻��������ѡ��״̬��֪ͨ���Լ�
	TMt       tMt;
	TMtStatus tMtStatus;
    TMt       tNullMt;
	tNullMt.SetNull();

	tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);

	u8         bySel = FALSE;
	for ( u8 byMtIdxLp = 1; byMtIdxLp <= MAXNUM_CONF_MT; byMtIdxLp ++ )
	{
		if( byMtIdxLp != tSrcMt.GetMtId() ) // �ų�����
		{
			tMt = m_ptMtTable->GetMt( byMtIdxLp );
			if( !tMt.IsNull() ) // ���ն��Ƿ���Ч
			{
				bySel = FALSE;
				m_ptMtTable->GetMtStatus(byMtIdxLp, &tMtStatus);
				TMt tSelMt;
				if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
				{
					tSelMt = tMtStatus.GetSelectMt(MODE_VIDEO);
					if( (!tSelMt.IsLocal() && tSelMt.GetMcuId() == tSrcMt.GetMtId()) ||
						tSelMt == tSrcMt ) 
					{
						bySel = TRUE;
						StopSwitchToSubMt( tMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE);
						tMtStatus.SetSelectMt(tNullMt, MODE_VIDEO);
						m_ptMtTable->SetMtStatus(byMtIdxLp, &tMtStatus);

						if (IsNeedSelApt(tSelMt.GetMtId(), byMtIdxLp, MODE_VIDEO))
						{
							StopHdVidSelAdp(tSrcMt, m_ptMtTable->GetMt(byMtIdxLp), MODE_VIDEO);
						}
					}
				}
				if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
				{
					tSelMt = tMtStatus.GetSelectMt(MODE_AUDIO);
					if( (!tSelMt.IsLocal() && tSelMt.GetMcuId() == tSrcMt.GetMtId()) ||
						tSelMt == tSrcMt ) 
					{
						bySel = TRUE;
						StopSwitchToSubMt( tMt.GetMtId(), MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);
						tMtStatus.SetSelectMt(tNullMt, MODE_VIDEO);
						m_ptMtTable->SetMtStatus(byMtIdxLp, &tMtStatus);
						if (IsNeedSelApt(tSelMt.GetMtId(), byMtIdxLp, MODE_AUDIO))
						{
							StopAudSelAdp(tSrcMt, m_ptMtTable->GetMt(byMtIdxLp));
						}
					}
				}

				if (bySel)
				{
				    MtStatusChange( byMtIdxLp, TRUE );
				}


			}
		}
	}	
}

/*====================================================================
    ������      ��VCSSwitchSrcToDst
    ����        �����VCS�������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
void CMcuVcInst::VCSSwitchSrcToDst( const TSwitchInfo &tSwitchInfo )
{
	CServMsg	cServMsg;
	TMt	tDstMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetDstMt());
	TMt tSrcMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetSrcMt());
	u8	byMode = tSwitchInfo.GetMode();

    // ֪ͨ�������Ͷ˷�������      
    NotifyMtSend( tSrcMt.GetMtId(), byMode );

    g_cMpManager.SetSwitchBridge(tSrcMt, 0, byMode);

	if (TYPE_MT == tDstMt.GetType())
	{
		StartSwitchToSubMt(tSrcMt, 0, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE, FALSE);
	}
	else if (TYPE_MCUPERI == tDstMt.GetType())
	{
		StartSwitchToPeriEqp(tSrcMt, 0, tDstMt.GetEqpId(), tSwitchInfo.GetDstChlIdx(), byMode, SWITCH_MODE_SELECT);
	}
	
	
	if (m_tConf.GetConfAttrb().IsResendLosePack())
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

}
/*====================================================================
    ������      ��VCSConfMTToTW
    ����        ��VCS���齫�ն�ѡ�����ǽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��BOOL
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
BOOL CMcuVcInst::VCSConfMTToTW( TTWSwitchInfo tTWSwitchInfo )
{  
	// ���ڸ����ն���Ϊ����ǽ��ʵ��ת��Ϊѡ��ҵ��
 	if (TYPE_MT == tTWSwitchInfo.GetDstMt().GetType())
	{
		TSwitchInfo tSwitchInfo = (TSwitchInfo)tTWSwitchInfo;
		return VCSConfSelMT(tSwitchInfo);
	}

    if (CurState() != STATE_ONGOING)
    {
		OspPrintf(TRUE, FALSE, "[VCSConfMTToTW] InstID(%d) in wrong state(%d)\n", GetInsID(), CurState());
		return FALSE;
    }


	CServMsg cServMsg;
    cServMsg.SetConfIdx(m_byConfIdx);
    cServMsg.SetConfId(m_tConf.GetConfId()) ;
	u8 bySrcId = m_cVCSConfStatus.GetCurSrcSsnId();
	if (bySrcId != 0)
	{
		cServMsg.SetSrcSsnId(bySrcId);
	}

    TMt tSrcMT  = tTWSwitchInfo.GetSrcMt();
    tSrcMT = GetLocalMtFromOtherMcuMt(tSrcMT); 

	u8 byDstId = tTWSwitchInfo.GetDstMt().GetEqpId();
    // ���ڸ����ն˷���������ǽ���ı�ֱ���
    TLogicalChannel tSrcRvsChannl;
    if ( byDstId >= TVWALLID_MIN && byDstId <= TVWALLID_MAX &&
		 m_ptMtTable->GetMtLogicChnnl( tSrcMT.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
    {
        if ( tSrcRvsChannl.GetChannelType() == MEDIA_TYPE_H264 &&
            ( IsVidFormatHD( tSrcRvsChannl.GetVideoFormat() ) ||
              tSrcRvsChannl.GetVideoFormat() == VIDEO_FORMAT_4CIF ) )
        {
            VcsLog("[VCSConfMTToTW] Mt<%d> VidType.%d with format.%d support no tvwall\n",
                   tSrcMT.GetMtId(), tSrcRvsChannl.GetChannelType(), tSrcRvsChannl.GetVideoFormat() );
			ChangeVFormat(tSrcMT, VIDEO_FORMAT_CIF);
        }
    }

	// ����Դ�ն�Ϊ�¼��նˣ�֪ͨ�¼�MCUѡ����ն�
	TMt tOrigMt = tTWSwitchInfo.GetSrcMt();
	if (!tOrigMt.IsLocal())
    {		
        OnMMcuSetIn(tOrigMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
    }

		
    switch(tTWSwitchInfo.GetMemberType()) 
    {
	case TW_MEMBERTYPE_VCSAUTOSPEC:
		{
			//Դ�ն˱����Ѿ��������
			if (FALSE == m_tConfAllMtInfo.MtJoinedConf(tSrcMT.GetMtId()))
			{
				ConfLog(FALSE, "[VCSConfMTToTW] select source MT%u-%u has not joined current conference! Error!\n",
						tSrcMT.GetMcuId(), tSrcMT.GetMtId());
// 				cServMsg.SetErrorCode(ERR_MCU_MT_NOTINCONF);
// 				SendMsgToAllMcs(MCU_VCS_VCMT_NOTIF, cServMsg);
				return FALSE;
			}

			TMtStatus tMtStatus;
			//Դ�ն˱���߱���Ƶ��������
			m_ptMtTable->GetMtStatus(tSrcMT.GetMtId(), &tMtStatus);
			if (FALSE == tMtStatus.IsSendVideo())
			{
				ConfLog(FALSE, "[VCSConfMTToTW] select source MT%u-%u has not ability to send video!\n",
						tSrcMT.GetMcuId(), tSrcMT.GetMtId());
// 				cServMsg.SetErrorCode(ERR_MCU_SRCISRECVONLY);
// 				SendMsgToAllMcs(MCU_VCS_VCMT_NOTIF, cServMsg);
				return FALSE;
			}	
		}
        break;
    default:
        ConfLog(FALSE, "[VCSConfMTToTW] Wrong TvWall Member Type!\n");
		return FALSE;
	}

	if (byDstId >= TVWALLID_MIN && byDstId <= TVWALLID_MAX )
	{
	    ChangeTvWallSwitch(&tSrcMT,
                            tTWSwitchInfo.GetDstMt().GetEqpId(),
                            tTWSwitchInfo.GetDstChlIdx(),
                            tTWSwitchInfo.GetMemberType(), TW_STATE_START);
	}
	else if (byDstId >= HDUID_MIN && byDstId <= HDUID_MAX)
	{
		ChangeHduSwitch(&tSrcMT,
                         byDstId,
                         tTWSwitchInfo.GetDstChlIdx(),
                         tTWSwitchInfo.GetMemberType(), TW_STATE_START/*, MODE_VIDEO*/);
	}
	else
	{
		ConfLog(FALSE, "[VCSConfMTToTW] wrong dest id(%d)\n", byDstId);
		return FALSE;
	}
	return TRUE;
}
/*====================================================================
    ������      ��VCSConfStopMTToTW
    ����        ��VCS���齫�ն��߳�����ǽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
void CMcuVcInst::VCSConfStopMTToTW(TMt tMt)
{
	TMt tOrigMt = tMt;
	
	TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
	if(tConfAttrib.IsHasTvWallModule() )
    {
		if (!tMt.IsLocal())
		{
			tMt = m_ptMtTable->GetMt(tMt.GetMcuId());
		}

        TMultiTvWallModule tMultiTvWallModule;
        m_tConfEqpModule.GetMultiTvWallModule( tMultiTvWallModule );
        TTvWallModule tTvWallModule;
        for( u8 byTvLp = 0; byTvLp < tMultiTvWallModule.GetTvModuleNum(); byTvLp++ )
        {
            tMultiTvWallModule.GetTvModuleByIdx(byTvLp, tTvWallModule);
			u8 byEqpId = tTvWallModule.m_tTvWall.GetEqpId();
            if( g_cMcuVcApp.IsPeriEqpConnected(byEqpId))
			{
				TPeriEqpStatus tTvwallStatus;
				g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
				if (tTvwallStatus.m_byOnline == 1)
				{
					u8 byTmpMtId = 0;
					u8 byMtConfIdx = 0;
					u8 byMemberNum = tTvwallStatus.m_tStatus.tTvWall.byChnnlNum;
					for(u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
					{
						byTmpMtId = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMtId();
						byMtConfIdx = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
						if (tMt.GetMtId() == byTmpMtId && m_byConfIdx == byMtConfIdx)
						{
							StopSwitchToPeriEqp(byEqpId, byLoop);
							tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
							tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
							tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
            
							//���ն�״̬
							m_ptMtTable->SetMtInTvWall(byTmpMtId, FALSE);
						}               
					}
					g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tTvwallStatus);

// 					cServMsg.SetMsgBody((u8 *)&tTvwallStatus, sizeof(tTvwallStatus));
// 					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
				}
			}
		}
	}

	// �������ǽ
	if (m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum != 0 )
	{
		u8 byNum = m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum;
		u8 byMtId = 0;
		THDTWModuleAlias* ptAlias = (THDTWModuleAlias*)m_tConfEqpModule.m_tHDTvWall.m_atHDTWModuleAlias;
		for (u8 byIndex = 0; byIndex < byNum; byIndex++)
		{
			byMtId = m_ptMtTable->GetMtIdByAlias((TMtAlias*)ptAlias);
			if (byMtId > 0 && m_tConfAllMtInfo.MtJoinedConf(byMtId))
			{
				TMtStatus tMTStatus;
				m_ptMtTable->GetMtStatus(byMtId, &tMTStatus);
				if (tOrigMt == tMTStatus.GetSelectMt(MODE_VIDEO))
				{
					VCSConfStopMTSel(m_ptMtTable->GetMt(byMtId), MODE_VIDEO);
				}
			}
		}
	}

}

/*====================================================================
    ������      ��GoOnSelStep
    ����        �����ݻ���״̬����������ѡ����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tMt��ѡ������ն�
	              byMediaType��ѡ��ý������
				  bySelDirect: ѡ����ϯ(TRUE)/����ϯѡ��(FALSE)
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
void CMcuVcInst::GoOnSelStep(TMt tMt, u8 byMediaType, u8 bySelDirect)
{
	if (bySelDirect)
	{
		VcsLog("[GoOnSelStep] mt(mcuid:%d, mtid:%d) go on sel chairman\n",
			   tMt.GetMcuId(), tMt.GetMtId());
	}
	else
	{
		VcsLog("[GoOnSelStep] chairman go on sel mt(mcuid:%d, mtid:%d)\n",
			   tMt.GetMcuId(), tMt.GetMtId());
	}

	// �жϱ�����ϯ�ն��Ƿ����ߣ��������򷵻�
	TMt tChairMan;
	tChairMan = m_tConf.GetChairman();
	if (tChairMan.IsNull() || 
		!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()))
	{
		return;
	}

	// �¼����鲻����ѡ������
	if (CONF_CREATE_MT == m_byCreateBy)
	{
		VcsLog("[GoOnSelStep] No sel step for subconf\n");
		return;
	}

	// ���ģʽ�²�������ϯ��ѡ����ѡ������
	if (ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
	{
		VcsLog("[GoOnSelStep] No sel step in this mode(%d)", m_cVCSConfStatus.GetCurVCMode());
		return;
	}
	
	TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();
	// ��ǰ�����ն˻�ǰ�����ն����ڵ�mcu�ſɽ�����ϯѡ������ϯѡ��,�ų�����
	// ����ʱ�����⣬����ѡ����ϯ, �ſ�Ҫ��, ֻ��ǹ̶���Դ����
	if ((!tCurVCMT.IsNull() && 
		(tCurVCMT == tMt || tMt.GetMtId() == tCurVCMT.GetMcuId())) ||
		(TRUE == bySelDirect &&	 CONF_CALLMODE_NONE == VCSGetCallMode(tMt)))
	{
		// ���콻��ͨ��
		TSwitchInfo tSwitchInfo;
		if (bySelDirect)
		{
			tSwitchInfo.SetDstMt(tMt);
			tSwitchInfo.SetSrcMt(tChairMan);
		}
		else
		{
			tSwitchInfo.SetDstMt(tChairMan);
			tSwitchInfo.SetSrcMt(tMt);
		}

		switch(m_cVCSConfStatus.GetCurVCMode())
		{
		case VCS_SINGLE_MODE:
			if (MODE_VIDEO == byMediaType)
			{
				tSwitchInfo.SetMode(MODE_VIDEO);
			}
			else if (MODE_AUDIO == byMediaType)
			{
				tSwitchInfo.SetMode(MODE_AUDIO);
			}
			else
			{
				VcsLog("[GoOnSelStep] uncorrect sel mediatype(%d) in single mode\n", byMediaType);
				return;
			}
			break;
		case VCS_MULTW_MODE:
			{
				// ����ǽģʽ��Ϊ����ģʽ�����Խ���Ե�����Դ����Ƶ����ѡ��
				if (MODE_VIDEO == byMediaType)
				{
					tSwitchInfo.SetMode(MODE_VIDEO);
					// ���ڵ�ǰ������Դ����ѡ��ʱ,�ж�ѡ��ʱ������Դ�Ƿ����,
					// �������ͷ�֮ǰռ�õ�����ѡ����Դ,������֤��ǰ����ѡ���ɹ�
					u8 bySrcId = (tSwitchInfo.GetSrcMt().IsLocal() ? tSwitchInfo.GetSrcMt().GetMtId() : tSwitchInfo.GetSrcMt().GetMcuId());
					u8 byDstId = (tSwitchInfo.GetDstMt().IsLocal() ? tSwitchInfo.GetDstMt().GetMtId() : tSwitchInfo.GetDstMt().GetMcuId());
					if (IsNeedSelApt(bySrcId, byDstId, MODE_VIDEO) && 
						IsNeedNewSelAdp(bySrcId, byDstId, MODE_VIDEO) && 
						!g_cMcuVcApp.IsIdleHDBasVidChlExist())
					{
						EqpLog("[GoOnSelStep]not enough bas for all mt in twmode, so stop mt sel chairman\n");
						for (u8 byMtId = 0; byMtId < MAXNUM_CONF_MT; byMtId++)
						{
							if (byMtId != bySrcId && byMtId != byDstId &&
								m_tConfAllMtInfo.MtJoinedConf(byMtId))
							{
								VCSConfStopMTSel(m_ptMtTable->GetMt(byMtId), MODE_VIDEO);
							}

						}

					}
				}
				else
				{ 
					return;
				}
			}
		    break;
		case VCS_MULVMP_MODE:
			// ��ģʽ�¾�������ϳ�ͼ�񣬲�����ѡ��
			return;
			break;
		default:
			OspPrintf(TRUE, FALSE, "[GoOnSelStep] wrong vc mode(%d)\n", m_cVCSConfStatus.GetCurVCMode());
			return;
		    break;
		}
		VCSConfSelMT(tSwitchInfo);


	}
}
/*====================================================================
    ������      ��ChgCurVCMT
    ����        �����ݻ���״̬���л���ǰ�����ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
void CMcuVcInst::ChgCurVCMT(TMt tNewCurVCMT)
{
	TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();
	if (tCurVCMT == tNewCurVCMT)
	{
		VcsLog("[ChgCurVCMT] NewCurVCMT hasn't change\n");	
		return;
	}
	m_cVCSConfStatus.SetCurVCMT(tNewCurVCMT);

	u8  byVCMode = m_cVCSConfStatus.GetCurVCMode();
	switch(byVCMode)
	{
	case VCS_SINGLE_MODE:
		if (!tCurVCMT.IsNull())
		{
			VCSDropMT(tCurVCMT);
		}
		break;
	case VCS_MULTW_MODE:
	case VCS_GROUPTW_MODE:
		// ��һ���ն˵��ȳɹ��ٽ��У���ǰ�����ն��滻��ԭ�ն˽�����ǽ�Ĳ���
		if (!tCurVCMT.IsNull() && 
			 m_tConfAllMtInfo.MtJoinedConf(tCurVCMT.GetMcuId(), tCurVCMT.GetMtId()))
		{
			u8 byNeedInTW = TRUE;

			// ����ǰ���ȵ��ն����µ��ȵ��ն�����ͬһ���¼�MCU,�����轫��ǰ���ȵ��ն˷������ǽ
			if (!tCurVCMT.IsLocal() && !tNewCurVCMT.IsLocal() && tCurVCMT.GetMcuId() == tNewCurVCMT.GetMcuId())
			{
				byNeedInTW = FALSE;
			}
			if (byNeedInTW)
			{
				TEqp tEqp;
				TMt  tDropOutMT;
				tEqp.SetNull();
				tDropOutMT.SetNull();
				u8 byChanIdx = 0;
				// �ҵ����õĵ���ǽͨ������ԭ�����ն�ѡ��������ǽ��û�����֪ͨ�����ָ�����������ģʽ
				u16 dwChanIdx = FindUsableTWChan(m_cVCSConfStatus.GetCurUseTWChanInd(), &tDropOutMT, 
												 &tEqp, &byChanIdx, &tCurVCMT);
				if (!dwChanIdx)
				{
					CServMsg cServMsg;
					cServMsg.SetConfIdx(m_byConfIdx);
					cServMsg.SetConfId(m_tConf.GetConfId());
					cServMsg.SetEventId(MCU_VCS_VCMT_NOTIF);
					cServMsg.SetErrorCode(ERR_MCU_VCS_NOUSABLETW);
					SendMsgToMcs(m_cVCSConfStatus.GetCurSrcSsnId(), MCU_VCS_VCMT_NOTIF, cServMsg);

					// �������л�Ϊ�鲥������ģʽ���ߵ�������ģʽ
					if (ISGROUPMODE(byVCMode))
					{
						RestoreVCConf(VCS_GROUPSPEAK_MODE);
					}
					else
					{
						RestoreVCConf(VCS_SINGLE_MODE);
					}
					return;
				}
				else
				{
					TTWSwitchInfo tSwitchInfo;
					tSwitchInfo.SetSrcMt(tCurVCMT);
					tSwitchInfo.SetDstMt(tEqp);
					tSwitchInfo.SetDstChlIdx(byChanIdx);
					tSwitchInfo.SetMemberType(TW_MEMBERTYPE_VCSAUTOSPEC);
					tSwitchInfo.SetMode(MODE_VIDEO);
					if (VCSConfMTToTW(tSwitchInfo))
					{
						m_cVCSConfStatus.SetCurUseTWChanInd(dwChanIdx);
						// ��ͨ��ԭ���ն��Զ��Ҷ�
						if(!tDropOutMT.IsNull() && VCS_MULTW_MODE == byVCMode)
						{
							// ����ԭͨ�������Ϊ�¼�MCU,�Ҷϸ�MCU�»ش�ͨ���е��ն�
							if (tDropOutMT.IsLocal() && 
								MT_TYPE_SMCU == m_ptMtTable->GetMtType(tDropOutMT.GetMtId()))
							{
								TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tDropOutMT.GetMtId());
								tDropOutMT = ptConfMcInfo->m_tMMcuViewMt;
							}
							ConfLog(FALSE, "[ChgCurVCMT]mcuid:%d mtid:%d enter tw(%d) %d channel and drop mcuid:%d mtid:%d\n", 
								   tCurVCMT.GetMcuId(), tCurVCMT.GetMtId(), tEqp.GetEqpId(), byChanIdx, tDropOutMT.GetMcuId(), tDropOutMT.GetMtId());
							VCSDropMT(tDropOutMT);
						}
					}
				}
			}

		}

		if (VCS_MULTW_MODE == byVCMode)
		{
			VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);
		}

		if (VCS_GROUPTW_MODE == byVCMode)
		{
			if (tNewCurVCMT.IsNull())
			{
				TMt tChairMan = m_tConf.GetChairman();
				ChangeSpeaker(&tChairMan, FALSE, FALSE);
			}
			else
			{
				ChangeSpeaker(&tNewCurVCMT, FALSE, FALSE);
			}
		}
		break;
	case VCS_MULVMP_MODE:
		// �����������
	    break;
	case VCS_GROUPSPEAK_MODE:
		if (tNewCurVCMT.IsNull())
		{
			TMt tChairMan = m_tConf.GetChairman();
			ChangeSpeaker(&tChairMan, FALSE, FALSE);
		}
		else
		{
			ChangeSpeaker(&tNewCurVCMT, FALSE, FALSE);
		}
	    break;
	case VCS_GROUPVMP_MODE:
		if (!tNewCurVCMT.IsNull())
		{
			ChangeVmpStyle(tNewCurVCMT, TRUE);
		}
		break;
	default:
		OspPrintf(TRUE, FALSE, "[ChgCurVCMT]Wrong vcmode\n");
	    break;
	}

	TMt tNullMt;
	tNullMt.SetNull();
	m_cVCSConfStatus.SetReqVCMT(tNullMt);

	// ͨ������е�VCS����״̬�ĸı�
	VCSConfStatusNotif();
}
/*====================================================================
    ������      ��FindUsableTWChan
    ����        ����ѯ��һ�����õĵ���ǽͨ����û�п��е�����ռ����ʹ�õ�ͨ����,
	              ������ͨ���е�ԭ���ն˹Ҷ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[IN]wCurUseChanIndex: ��ǰ��ʹ�õ�ͨ������
	              [IN/OUT] ptDropOutMT: ��û��δ�õ�ͨ��������ռ���ն���Ϣ
 	              [IN/OUT]        tEqp: ��õĿ��õĵ���ǽ��Ϣ
	              [IN/OUT]   byChanIdx: ��õĿ��õĵ���ǽ��Ӧͨ����Ϣ
				  [IN]          ptInMT: ���������ǽ���ն�
    ����ֵ˵��  ��u16:���ص�ǰ���õ�ͨ�������ţ���������Ϊ0����û�п��õ�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
u16 CMcuVcInst::FindUsableTWChan(u16 wCurUseChanIndex, TMt* ptDropOutMT, TEqp* ptEqp, u8* pbyChanIdx, TMt* ptInMT)
{
	u16 wUsableChanNum  = 0;       // ���п��õ�ͨ����
	u16 wChanIndex      = 0;       // ͨ������
	u16 wNextChanIndex  = 0;       // ��һ�����õ�ͨ������
	u16 wFirstChanIndex = 0;       // ��һ�����õ�ͨ������

	TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
	// �����¼����ն˽�����ǽ������ռ��ԭ�¼��ն˵�ͨ��
	// ĿǰӦ��ģʽ�����ܴӸöη��أ��¼�����ϯ��ֻ�ܴ���һ���ն����ߣ�
	// �ն˴ӵ�ǰ�����ն�״̬�����л���������ǽ״̬������ǰһ��״̬ʱ���ѽ�����ԭ������ǽ���¼��ն˹Ҷ�
	if (ptInMT != NULL && !ptInMT->IsNull() && !ptInMT->IsLocal())
	{
		u8 byInMTId = ptInMT->GetMcuId();
		if(tConfAttrib.IsHasTvWallModule() )
		{
			TMultiTvWallModule tMultiTvWallModule;
			m_tConfEqpModule.GetMultiTvWallModule( tMultiTvWallModule );
			TTvWallModule tTvWallModule;
			for( u8 byTvLp = 0; byTvLp < tMultiTvWallModule.GetTvModuleNum(); byTvLp++ )
			{
				tMultiTvWallModule.GetTvModuleByIdx(byTvLp, tTvWallModule);
				if( g_cMcuVcApp.IsPeriEqpConnected( tTvWallModule.m_tTvWall.GetEqpId()) )
				{
					TPeriEqpStatus tStatus;
					TMt tTWMember;
					g_cMcuVcApp.GetPeriEqpStatus(tTvWallModule.m_tTvWall.GetEqpId(), &tStatus);
					for (u8 byIndex = 0; byIndex < MAXNUM_PERIEQP_CHNNL; byIndex++)
					{
						wChanIndex++;
						tTWMember = (TMt)tStatus.m_tStatus.tTvWall.atVideoMt[byIndex];
						tTWMember = GetLocalMtFromOtherMcuMt(tTWMember);
						if (tTWMember.GetMtId() == byInMTId)
						{
							if(ptEqp != NULL)
							{
								*ptEqp = tTvWallModule.GetTvEqp();
							}
							if (pbyChanIdx != NULL)
							{
								*pbyChanIdx = byIndex;
							}
							VcsLog("smcu(mtid:%d) has in tw %d channel, new smt(mcuid:%d mtid:%d) still use this channel\n", 
								   tTWMember.GetMtId(), byIndex, ptInMT->GetMcuId(), ptInMT->GetMtId());
							return wChanIndex;
						}
					}
				}
				else
				{
					wChanIndex += MAXNUM_PERIEQP_CHNNL;
				}
			}
		}

		if (m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum != 0)
		{
			u8 byNum = m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum;
			u8 byMtId = 0;
			THDTWModuleAlias* ptAlias = (THDTWModuleAlias*)m_tConfEqpModule.m_tHDTvWall.m_atHDTWModuleAlias;
			// ���������ǽ������״̬
			for (u8 byIndex = 0; byIndex < byNum; byIndex++)
			{
				wChanIndex++;
				byMtId = m_ptMtTable->GetMtIdByAlias((TMtAlias*)(ptAlias + byIndex));
				if (byMtId > 0 && m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
					m_tConfEqpModule.m_tHDTvWall.m_abyMemberType[byIndex][0] != TW_MEMBERTYPE_NULL)
				{
					TMtStatus tMTStatus;
					m_ptMtTable->GetMtStatus(byMtId, &tMTStatus);
					TMt tSelMT = tMTStatus.GetSelectMt(MODE_VIDEO);
					tSelMT = GetLocalMtFromOtherMcuMt(tSelMT); 
					if (tSelMT.GetMtId() == byInMTId)
					{
						if(ptEqp != NULL)
						{
							*ptEqp = m_ptMtTable->GetMt(byMtId);
						}
						if (pbyChanIdx != NULL)
						{
							*pbyChanIdx = 0;
						}
						VcsLog("smcu(mtid:%d) has in hdtw(mtid:%d), new smt(mcuid:%d mtid:%d) still use this channel\n", 
							    tSelMT.GetMtId(), byMtId, ptInMT->GetMcuId(), ptInMT->GetMtId());
						return wChanIndex;
					}
				}
			}
		}

		if ( m_tConfEqpModule.m_tHduModule.GetHduChnlNum() != 0 )
		{
			u8 byHduChnlNum = m_tConfEqpModule.m_tHduModule.GetHduChnlNum();
			THduModChnlInfo tChnlInfo;
			u8 byHduId, byHduChnlIdx = 0;
			for( u8 byChnlIdx = 0; byChnlIdx < byHduChnlNum; byChnlIdx++ )
			{
				tChnlInfo    = m_tConfEqpModule.m_tHduModule.GetOneHduChnlInfo(byChnlIdx);
				byHduId      = tChnlInfo.GetHduID();
				byHduChnlIdx = tChnlInfo.GetHduChnlIdx();
				if( byHduChnlIdx < MAXNUM_HDU_CHANNEL &&
					byHduId >= HDUID_MIN && byHduId <= HDUID_MAX &&
					g_cMcuVcApp.IsPeriEqpConnected(byHduId) &&
					TW_MEMBERTYPE_VCSAUTOSPEC == tChnlInfo.GetMemberType())
				{
					TPeriEqpStatus tStatus;
					g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tStatus);
					wChanIndex++;
					if (tStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetMtId() == byInMTId)
					{
						if(ptEqp != NULL)
						{
							*ptEqp = tStatus.m_tStatus.tHdu.atHduChnStatus[byHduChnlIdx].GetEqp();
						}
						if (pbyChanIdx != NULL)
						{
							*pbyChanIdx = byHduChnlIdx;
						}
						VcsLog("smcu(mtid:%d) has in HDU(hduid:%d) %d channel, new smt(mcuid:%d mtid:%d) still use this channel\n", 
							    byInMTId, byHduId, byHduChnlIdx, ptInMT->GetMcuId(), ptInMT->GetMtId());
						return wChanIndex;
					}
				}
			}
		}

	}

	// �����ն˽������ǽ
	wUsableChanNum  = 0;       // ���п��õ�ͨ����
	wChanIndex      = 0;       // ͨ������
	wNextChanIndex  = 0;       // ��һ�����õ�ͨ������
	wFirstChanIndex = 0;       // ��һ�����õ�ͨ������

	// hduͨ������
	if (m_tConfEqpModule.m_tHduModule.GetHduChnlNum() != 0 )
	{
		u8 byHduChnlNum = m_tConfEqpModule.m_tHduModule.GetHduChnlNum();
		THduModChnlInfo tChnlInfo;
		u8 byHduId, byHduChnlIdx = 0;
		for( u8 byChnlIdx = 0; byChnlIdx < byHduChnlNum; byChnlIdx++ )
		{
			wChanIndex++;
			tChnlInfo    = m_tConfEqpModule.m_tHduModule.GetOneHduChnlInfo(byChnlIdx);
			byHduId      = tChnlInfo.GetHduID();
			byHduChnlIdx = tChnlInfo.GetHduChnlIdx();
			if( byHduChnlIdx < MAXNUM_HDU_CHANNEL &&
				byHduId >= HDUID_MIN && byHduId <= HDUID_MAX &&
				g_cMcuVcApp.IsPeriEqpConnected(byHduId) &&
				TW_MEMBERTYPE_VCSAUTOSPEC == tChnlInfo.GetMemberType() &&				
				(0 == wFirstChanIndex || wChanIndex > wCurUseChanIndex))
			{
				TPeriEqpStatus tStatus;
				g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tStatus);
				// ��ͨ���Ѿ���ռ�ã�Ҫ�߳�
				if (ptDropOutMT != NULL)
				{
					*ptDropOutMT = tStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx];
				}
				if (ptEqp != NULL)
				{
					*ptEqp = tStatus.m_tStatus.tHdu.atHduChnStatus[byHduChnlIdx].GetEqp();
				}
				if (pbyChanIdx != NULL)
				{
					*pbyChanIdx = byHduChnlIdx;
				}

				if (0 == wFirstChanIndex)
				{
					wFirstChanIndex = wChanIndex;
				}

				if (wChanIndex > wCurUseChanIndex)
				{
					wNextChanIndex = wChanIndex;
					return wNextChanIndex;
				}

			}
		}
	}

	if(0 == wNextChanIndex && tConfAttrib.IsHasTvWallModule() )
    {
        TMultiTvWallModule tMultiTvWallModule;
        m_tConfEqpModule.GetMultiTvWallModule( tMultiTvWallModule );
        TTvWallModule tTvWallModule;
        for( u8 byTvLp = 0; byTvLp < tMultiTvWallModule.GetTvModuleNum(); byTvLp++ )
        {
            tMultiTvWallModule.GetTvModuleByIdx(byTvLp, tTvWallModule);
            if( g_cMcuVcApp.IsPeriEqpConnected( tTvWallModule.m_tTvWall.GetEqpId()) )
            {
				for (u8 byIndex = 0; byIndex < MAXNUM_PERIEQP_CHNNL; byIndex++)
				{
					wChanIndex++;
					if (tTvWallModule.m_abyMemberType[byIndex] != TW_MEMBERTYPE_NULL &&
						(0 == wFirstChanIndex || wChanIndex > wCurUseChanIndex))
					{
						// ��ȡ����ǰͨ������һ�����߿���ͨ����״̬
						TPeriEqpStatus tStatus;
						g_cMcuVcApp.GetPeriEqpStatus(tTvWallModule.m_tTvWall.GetEqpId(), &tStatus);
						TMt tOldMt = (TMt)tStatus.m_tStatus.tTvWall.atVideoMt[byIndex];
						// ��ͨ���Ѿ���ռ�ã�Ҫ�߳�
						if (ptDropOutMT != NULL)
						{
							*ptDropOutMT = tOldMt;
						}
						if (ptEqp != NULL)
						{
							*ptEqp = tTvWallModule.GetTvEqp();
						}
						if (pbyChanIdx != NULL)
						{
							*pbyChanIdx = byIndex;
						}

						if (0 == wFirstChanIndex)
						{
							wFirstChanIndex = wChanIndex;
						}

						if (wChanIndex > wCurUseChanIndex)
						{
							wNextChanIndex = wChanIndex;
							return wNextChanIndex;
						}

					}
				}
            }
			else
			{
				wChanIndex += MAXNUM_PERIEQP_CHNNL;
			}
        }
    }

	// ��δ����ͨǽ���ҵ���ǰʹ��ͨ������һ������ͨ���������������ǽ
	if ( 0 == wNextChanIndex && m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum != 0 )
	{
		u8 byNum = m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum;
		u8 byMtId = 0;
		THDTWModuleAlias* ptAlias = (THDTWModuleAlias*)m_tConfEqpModule.m_tHDTvWall.m_atHDTWModuleAlias;
		// ���������ǽ������״̬
		for (u8 byIndex = 0; byIndex < byNum; byIndex++)
		{
			wChanIndex++;
			byMtId = m_ptMtTable->GetMtIdByAlias((TMtAlias*)(ptAlias + byIndex));
			if (byMtId > 0 && m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
				m_tConfEqpModule.m_tHDTvWall.m_abyMemberType[byIndex][0] != TW_MEMBERTYPE_NULL &&
				(0 == wFirstChanIndex || wChanIndex > wCurUseChanIndex))
			{

				TMtStatus tMTStatus;
				m_ptMtTable->GetMtStatus(byMtId, &tMTStatus);
				if (ptDropOutMT != NULL)
				{
					*ptDropOutMT = tMTStatus.GetSelectMt(MODE_VIDEO);
				}	
				if (ptEqp != NULL)
				{
					*ptEqp = m_ptMtTable->GetMt(byMtId);						
				}
				if (pbyChanIdx != NULL)
				{
					*pbyChanIdx = 0;
				}

				if (0 == wFirstChanIndex)
				{
					wFirstChanIndex = wChanIndex;
				}
				if (wChanIndex > wCurUseChanIndex)
				{
					wNextChanIndex = wChanIndex;
					return wNextChanIndex;
				}
			}						
		}
	}

	return wFirstChanIndex;
}
/*====================================================================
    ������      ��FindUsableVMP
    ����        ����ѯ���õĻ���ϳ���������ռ�øû���ϳ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
BOOL CMcuVcInst::FindUsableVMP()
{
	// ��ʱ��һ��Ĭ��ֵ
	TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
	tConfAttrib.SetHasVmpModule(TRUE);
	m_tConf.SetConfAttrb(tConfAttrib);

	TVmpModule tModule;
	TVMPParam  tParam;
	tParam.SetVMPAuto(TRUE);
	tParam.SetVMPMode(CONF_VMPMODE_AUTO);
	tParam.SetVMPStyle(VMP_STYLE_DYNAMIC);
	tParam.SetVMPBrdst(TRUE);
	tParam.SetVMPSchemeId(0);
	tModule.SetVmpParam(tParam);
	m_tConfEqpModule.SetVmpModule(tModule);


	tConfAttrib = m_tConf.GetConfAttrb();
	if(tConfAttrib.IsHasVmpModule())			
	{
		u8 byIdleVMPNum = 0;
		u8 abyVMPId[MAXNUM_PERIEQP];
		memset(&abyVMPId, 0, sizeof(abyVMPId));
		g_cMcuVcApp.GetIdleVMP(abyVMPId, byIdleVMPNum, sizeof(abyVMPId));

		//�Ƿ��л���ϳ���
		if( byIdleVMPNum != 0)
		{
			TVMPParam tVmpParam;
			tVmpParam = m_tConfEqpModule.GetVmpModule().GetVmpParam();

			u8  byVmpId = 0;
            if ( CONF_VMPMODE_AUTO == tVmpParam.GetVMPMode() )
            {
                //��ǰ���еĿ���VMP�Ƿ�֧������ĺϳɷ��
                u16 wError  = 0;

                if ( IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError) ) 
                {
					m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
					m_tVmpEqp.SetConfIdx( m_byConfIdx );
					
                }
                else
				{
					VcsLog("[FindUsableVMP] no vmp support thise style\n");
					return FALSE;
				}
			}
			else
			{
                VcsLog("[FindUsableVMP] VCSCONF hasn't non-auto vmp\n");
				return FALSE;
			}

			// ����ռ�û���ϳ���
            TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::RESERVE;//��ռ��,��ʱ��δ�ɹ��ٷ���
			tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam  = tVmpParam;
			tPeriEqpStatus.SetConfIdx( m_byConfIdx );
			g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			EqpLog("[FindUsableVMP]vmp(%d) has been used by conf(%d)\n", byVmpId, m_byConfIdx);

			//���ͨ��
			for( u8 byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++ )
			{
				m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );
			}
	
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;

}
/*====================================================================
    ������      ��RestoreVCConf
    ����        ���ָ����鵽δ�����κε��Ȳ���״̬����ģʽ
    �㷨ʵ��    ��1.������ģʽ,�ͷŲ���Ҫ����Դ
	              2.�л�����ģʽ�ĳ�ʼ״̬
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
void CMcuVcInst::RestoreVCConf(u8 byNewMode)
{
	if (CurState() != STATE_ONGOING)
	{
		return;
	}

	VcsLog("[RestoreVCConf] restore vcsconf for mode(%d) begin\n", byNewMode);

	CServMsg cServMsg;
	cServMsg.SetConfIdx(m_byConfIdx);
	cServMsg.SetConfId(m_tConf.GetConfId());

	u8 byOldMode = m_cVCSConfStatus.GetCurVCMode();

    // ֹͣ������ϯ�ն˵���������
    NotifyMtSend(m_tConf.GetChairman().GetMtId(), MODE_VIDEO, FALSE);
	// ���ڱ������ߵ���ϯ�ն�״̬�Ļָ�
	// ����ϯ�ն���ѡ���¼�MCU����ȡ��
	// �����ģʽ����ϯ��ѯ��ֹͣ
	if (!ISGROUPMODE(byNewMode))
	{
		KillTimer(MCUVC_VCS_CHAIRPOLL_TIMER);
		m_cVCSConfStatus.SetChairPollState(VCS_POLL_STOP);
	}
	if (!(ISGROUPMODE(byNewMode) && ISGROUPMODE(byOldMode)))
	{
		TMtStatus tChairmanStat;
		if (m_ptMtTable->GetMtStatus(m_tConf.GetChairman().GetMtId(), &tChairmanStat))
		{
			if (!tChairmanStat.GetSelectMt(MODE_VIDEO).IsNull())
			{
				VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);	
			}

			if (!tChairmanStat.GetSelectMt(MODE_AUDIO).IsNull())
			{
				VCSConfStopMTSel(m_tConf.GetChairman(), MODE_AUDIO);
			}
		}
	}


	// ����ϳ�����Դ���ͷ�
	// ��ֹͣ����ϳɣ�����һ�����������ն�ʱ������Ҫ�Ļ���ϳ��л�
	if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE &&
		byNewMode != VCS_MULVMP_MODE && byNewMode != VCS_GROUPVMP_MODE)
	{
		TPeriEqpStatus tPeriEqpStatus;
		if (!m_tVmpEqp.IsNull() &&
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
			tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState != TVmpStatus::WAIT_STOP)
		{
			SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, 6*1000);
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
			SendMsgToEqp( m_tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cServMsg );
		}

		// Bug00013728����ʱ������ �ȵ�����ϳ���ֹͣ�ɹ���ŷ�������ϳ��� 
// 		TPeriEqpStatus tPeriEqpStatus;
//  	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
// 		tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = FALSE;
// 		tPeriEqpStatus.SetConfIdx( 0 );
// 		g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus ); 

		m_tConf.m_tStatus.SetVMPMode(CONF_VMPMODE_NONE);

        if (m_tConf.m_tStatus.GetVmpParam().IsVMPBrdst())
        {
            ChangeVidBrdSrc(NULL);
        }

	}

	// ��������Դ���ͷ�
	// ֹͣ������
	if( m_tConf.m_tStatus.IsMixing() && !ISGROUPMODE(byNewMode) && 
		byNewMode != VCS_MULTW_MODE && byNewMode != VCS_MULVMP_MODE)
	{
		StopMixing();

		//���������� ͬ������ʱ������
// 		TPeriEqpStatus tPeriEqpStatus;
//  		g_cMcuVcApp.GetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
// 		tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = TMixerGrpStatus::READY;
//         tPeriEqpStatus.SetConfIdx( 0 );
// 		g_cMcuVcApp.SetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
// 
//         cServMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
//         SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
	}

	// ����ǽ��Դ���ͷ�
	// HDU�ͷ�
	u8  byLoop = 0,  byEqpId = 1;
	TPeriEqpStatus tTvwallStatus;
	for (byEqpId = HDUID_MIN; byEqpId <= HDUID_MAX; byEqpId++)
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byEqpId))
		{
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
            if (1 == tTvwallStatus.m_byOnline)
            {
                u8 byMtConfIdx;
				u8 byInTWMtId;
                u8 byMemberNum = tTvwallStatus.m_tStatus.tHdu.byChnnlNum;
			    for(byLoop = 0; byLoop < byMemberNum; byLoop++)
			    {
                    byMtConfIdx = tTvwallStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
					byInTWMtId  = tTvwallStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetMtId();
				    if (m_byConfIdx == byMtConfIdx)
				    {
					    StopSwitchToPeriEqp(byEqpId, byLoop);
                        tTvwallStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
                        tTvwallStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
						//���ն�״̬
						m_ptMtTable->SetMtInHdu(byInTWMtId, FALSE);
						MtStatusChange(byInTWMtId, TRUE);						
				    }
			    }
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tTvwallStatus);

                cServMsg.SetMsgBody((u8 *)&tTvwallStatus, sizeof(tTvwallStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
            }			

		}
	}

	// �������ǽ�ͷ�
	for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byEqpId))
		{
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
            if (1 == tTvwallStatus.m_byOnline)
            {
                u8 byMtConfIdx;
				u8 byInTWMtId;
                u8 byMemberNum = tTvwallStatus.m_tStatus.tTvWall.byChnnlNum;
			    for(byLoop = 0; byLoop < byMemberNum; byLoop++)
			    {
                    byMtConfIdx = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
					byInTWMtId  = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMtId();
				    if (m_byConfIdx == byMtConfIdx)
				    {
					    StopSwitchToPeriEqp(byEqpId, byLoop);
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
						//���ն�״̬
						m_ptMtTable->SetMtInTvWall(byInTWMtId, FALSE);
						MtStatusChange(byInTWMtId, TRUE);						
				    }
			    }
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tTvwallStatus);

                cServMsg.SetMsgBody((u8 *)&tTvwallStatus, sizeof(tTvwallStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
            }			

		}
	}

	// �ͷŸ������ǽ
	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (m_tConfAllMtInfo.MtJoinedConf(byMtId) && 
			CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtId) &&
			MT_TYPE_MT ==m_ptMtTable->GetMtType(byMtId) &&
			byMtId != m_tConf.GetChairman().GetMtId())
		{
			VCSConfStopMTSel(m_ptMtTable->GetMt(byMtId), MODE_BOTH);
		}
	}

	// ������Դ���ͷ�
	// �����ģʽ���л����Ҷ����зǶ�ʱ���еĵ�����Դ
	if (!(ISGROUPMODE(byNewMode)  && ISGROUPMODE(byOldMode)))
	{
		TMt tMt;
		for( u8 byIndex = 1; byIndex <= MAXNUM_CONF_MT; byIndex++ )
		{	
			tMt = m_ptMtTable->GetMt(byIndex);
			if(!m_tConfAllMtInfo.MtJoinedConf(byIndex)) 
			{
				continue;
			}
			if (CONF_CALLMODE_NONE == m_ptMtTable->GetCallMode(byIndex) &&
				m_ptMtTable->GetMtType(byIndex) != MT_TYPE_MMCU) 
			{
				VCSDropMT(tMt);
			}
			else if (MT_TYPE_SMCU == m_ptMtTable->GetMtType(byIndex))
			{
				TMt tSMt;
				//���ߵ�SMCU,�Ҷ��������еķǶ�ʱ���еĵ�����Դ
				TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(byIndex);
				if (NULL == ptMcInfo)
				{
					continue;
				}
				for (u8 bySIndex = 1; bySIndex <= MAXNUM_CONF_MT; bySIndex++)
				{
					tSMt.SetMcuId(byIndex);
					tSMt.SetMtId(bySIndex);
					if (m_tConfAllMtInfo.MtJoinedConf(byIndex, bySIndex) && 
						CONF_CALLMODE_NONE == VCSGetCallMode(tSMt))
					{
						VCSDropMT(tSMt);
					}
				}
            
				// ������ѡ������ȡ��ѡ��
				if (!ptMcInfo->m_tSpyMt.IsNull())
				{
					VCSConfStopMTSel(tMt, MODE_BOTH);
				}
				// ��ջش�ͨ���е���Ϣ
				ptMcInfo->m_tMMcuViewMt.SetNull();
			}	

		}
	}

	if (ISGROUPMODE(byOldMode) && !ISGROUPMODE(byNewMode))
	{
		RlsAllBasForConf();
	}

	// ���������
	ChangeSpeaker(NULL, FALSE, FALSE);

	// �ָ���ϯ�ն˵ķǾ�����״̬
	// ģʽ�л�ʱ���ָ�������״̬
// 	VCSMTMute(m_tConf.GetChairman(), FALSE, VCS_AUDPROC_MUTE);
// 	VCSMTMute(m_tConf.GetChairman(), FALSE, VCS_AUDPROC_SILENCE);

	// Ŀǰ�����������Ҫ��BAS����,��������������
	u16 wErrCode;
	if (!ISGROUPMODE(byOldMode) &&
		ISGROUPMODE(byNewMode) &&
		!PrepareAllNeedBasForConf(&wErrCode))
	{
		cServMsg.SetErrorCode(wErrCode);
		SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cServMsg);
		RestoreVCConf(VCS_SINGLE_MODE);		
		return;
	}

	// VCS����״̬�ָ�
	if (ISGROUPMODE(byOldMode) && ISGROUPMODE(byNewMode))
	{
		m_cVCSConfStatus.VCCRestoreGroupStatus();
	}
	else
	{
		m_cVCSConfStatus.VCCRestoreStatus();
	}

	if (ISGROUPMODE(byNewMode) || VCS_MULTW_MODE == byNewMode || VCS_MULVMP_MODE == byNewMode)
	{
		TMt tChairMan = m_tConf.GetChairman();
		// ��ʼ״ֻ̬����ϯ���������
		for (u8 byMixMtId = 1; byMixMtId <= MAXNUM_CONF_MT; byMixMtId++)
		{
			if (tChairMan.GetMtId() == byMixMtId)
			{
				m_ptMtTable->SetMtInMixing(byMixMtId, TRUE);
			}
			else
			{
				m_ptMtTable->SetMtInMixing(byMixMtId, FALSE);
			}
		}

		if (!m_tConf.m_tStatus.IsMixing())
		{
			// ���ģʽ���������ƻ���, �෽�໭������ǽʹ��ȫ�����
			BOOL32 dwStartResult = FALSE;
			if (ISGROUPMODE(byNewMode))
			{
				dwStartResult = StartMixing(mcuPartMix);
			}
			else
			{
				dwStartResult = StartMixing(mcuWholeMix);
			}
			if (!dwStartResult)
			{
				VcsLog("[RestoreVCConfInGroupMode] Find no mixer\n");
				if (ISGROUPMODE(byNewMode))
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_NOMIXSPEAKER);
				}
				else
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_NOMIXER);
				}
				SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cServMsg);
			}	

		}
		else
		{
			// ����ϯ�ն����ڻ�������
/*			TMt tMt;*/
// 			for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
// 			{
// 				tMt = m_ptMtTable->GetMt(byMtId);
// 				if (m_ptMtTable->IsMtInMixGrp(byMtId) &&
// 					!(tMt == tChairMan))
// 				{
// 					RemoveMixMember(&tMt);
// 					StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
// 										(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())),
// 										FALSE, MODE_AUDIO );
// 					g_cMpManager.StopSwitchToSubMt(tMt, MODE_AUDIO);   
// 					SwitchMixMember(&tMt, TRUE);
// 				}
// 			}
			if (ISGROUPMODE(byNewMode))
			{
				m_tConf.m_tStatus.SetSpecMixing();
				ChangeSpecMixMember(&tChairMan, 1);
			}
			else
			{
				SwitchToAutoMixing();
			}
		}
	}


	// ��ģʽ״̬�ĳ�ʼ��
	switch(byNewMode)
	{
	case VCS_SINGLE_MODE:
		break;
	case VCS_MULTW_MODE:
		break;
	case VCS_MULVMP_MODE:
	case VCS_GROUPVMP_MODE:
		{
			//���黭��ϳɲ����ָ�
			TVMPParam tVmpParam;
			if (m_tConf.GetConfAttrb().IsHasVmpModule())
			{
				tVmpParam = m_tConfEqpModule.GetVmpModule().GetVmpParam();
			}
			else
			{
				OspPrintf(TRUE, FALSE, "[ProcVcsMcuVCModeChgReq] must has vmp module currently\n");
				break;						
			}

			// ������������ϳ�,���޸Ļ���ϳɲ���������ϯһ����״̬
			TPeriEqpStatus tVmpState;
			if (/*m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE &&*/
				!m_tVmpEqp.IsNull() &&
				g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpState) &&
				tVmpState.GetConfIdx() == m_byConfIdx)
			{
				if (TVmpStatus::WAIT_START== tVmpState.m_tStatus.tVmp.m_byUseState ||
					TVmpStatus::START == tVmpState.m_tStatus.tVmp.m_byUseState)
				{
					m_tConf.m_tStatus.SetVmpParam(tVmpParam);
					AdjustVmpParam(&tVmpParam);
				}
				else
				{
					SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, 6*1000);
					tVmpState.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_START;
					g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpState);

					// ��������ϳ�
					m_tConf.m_tStatus.SetVmpParam(tVmpParam);
					AdjustVmpParam(&tVmpParam, TRUE);
				}

			}
			else
			{
				OspPrintf(TRUE, FALSE, "[RestoreVCConf]Hasn't reserve vmp for conf(confidx:%d)\n",
					      m_byConfIdx);
			}
		}			
		break;
	case VCS_GROUPSPEAK_MODE:
	case VCS_GROUPTW_MODE:
		// ��ϯ��Ϊ������
		{
			TMt tSpeaker = m_tConf.GetChairman();
			ChangeSpeaker(&tSpeaker, FALSE, FALSE);
		}
		break;
	default:
		VcsLog("[RestoreVCConf] uncorrect vcmode(%d)\n", byNewMode);
	    break;
	}

	m_cVCSConfStatus.SetCurVCMode(byNewMode);

	// ͨ������е�VCS����״̬�ĸı�
	VCSConfStatusNotif();

	VcsLog("[RestoreVCConf] restore vcsconf for mode(%d) end\n", byNewMode);

	return;	
}
/*====================================================================
    ������      ��SwitchToAutoMixing
    ����        ���л����Զ�����ģʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
BOOL32 CMcuVcInst::SwitchToAutoMixing()
{
	if (m_tConf.m_tStatus.IsNoMixing())
	{
		EqpLog("[SwitchToAutoMixing] fail to automixing because conf mix status is %d, not in mixing\n",
			   m_tConf.m_tStatus.GetMixerMode());
		return FALSE;
	}

	m_tConf.m_tStatus.SetAutoMixing();
	TMt tMt;
	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
		{
			tMt = m_ptMtTable->GetMt(byMtId);
			AddMixMember(&tMt);
			StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
								 (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())), 
								 MODE_AUDIO, SWITCH_MODE_SELECT);
		}
	}
	return TRUE;
}

/*====================================================================
    ������      ��VCSMTMute
    ����        ����ָ���ն˽��о���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tMt��           �����ն�
	              byMuteOpenFlag������(1)��ر�(0)
				  byMuteType��    ����(1)������(0)
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪        ����
====================================================================*/
void CMcuVcInst::VCSMTMute(TMt tMt, u8 byMuteOpenFlag, u8 byMuteType)
{
	// ���ڷǱ����նˣ���������������ڵ�mcu���в���
	if (!tMt.IsLocal())
	{
		tMt = m_ptMtTable->GetMt(tMt.GetMcuId());
	}

    VcsLog("[VCSMTMute] TMt(mcuid:%d mtid:%d) OpenFlag:%d MuteType:%d\n", tMt.GetMcuId(), tMt.GetMtId(), byMuteOpenFlag, byMuteType);

	u8 byVendor = m_ptMtTable->GetManuId(tMt.GetMtId());
	if(byVendor == MT_MANU_KDC)
	{
		u16 wEvent;
		CServMsg cMsg;
		if(byMuteType == 1)
		{
			wEvent = MCU_MT_MTMUTE_CMD;
		}
		else
		{
			wEvent = MCU_MT_MTDUMB_CMD;
		}	
		cMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));
		cMsg.CatMsgBody(&byMuteOpenFlag, sizeof(byMuteOpenFlag));
		cMsg.SetDstMtId(tMt.GetMtId());
		SendMsgToMt(tMt.GetMtId(), wEvent, cMsg);
	}
	else
	{                    
        //������KEDA�նˣ�KEDAMCU��Tandberg������������
		tMt = GetLocalMtFromOtherMcuMt(tMt);
        AdjustKedaMcuAndTaideMuteInfo(&tMt, byMuteType, byMuteOpenFlag);
	}
                
}
/*====================================================================
    ������      ��GetVCSAutoVmpMember
    ����        ��VCS�Զ�����ϳɣ���ȡ��ǰ����½��뻭��ϳɵĳ�Ա
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[IN\OUT]ptVmpMember����ͨ����Ա��Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/23                ���㻪        ����
====================================================================*/
u8 CMcuVcInst::GetVCSAutoVmpMember(TMt* ptVmpMember)
{
    // ��ȡ���������
    u8 byVmpCapChlNum = CMcuPfmLmt::GetMaxCapVMPByConfInfo(m_tConf);
	// ռ�õ�vmp�豸����֧�ֵ������
    TPeriEqpStatus tPeriEqpStatus;
    g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
    u8 byEqpMaxChl = tPeriEqpStatus.m_tStatus.tVmp.m_byChlNum;
	// ��ǰ���֧�ֽ��뻭��ϳɵ��ն���
	u8 byMaxMemberNum  = min(byVmpCapChlNum, byEqpMaxChl); 

	u8 byVmpStyle = GetVmpDynStyle(byMaxMemberNum);

// 	if (m_tVmpEqp.GetEqpId() > (VMPID_MIN + 8))
// 	{
// 		byVmpStyle = VMP_STYLE_FOUR;
// 	}

	if (VMP_STYLE_NONE == byVmpStyle)
	{
		return byVmpStyle;
	}

	TVMPParam tVmpParam;
	tVmpParam.SetVMPStyle(byVmpStyle);
	byMaxMemberNum = tVmpParam.GetMaxMemberNum();     

	TMt atVmpMember[MAXNUM_CONF_MT];
	memset(atVmpMember, 0, MAXNUM_CONF_MT * sizeof(TMt));
	// ��֤��һ��ͨ���ڷ���ϯ
	u8 byChnlNum = 0;
	BOOL32 byHasChairman = HasJoinedChairman(); 
	if (byHasChairman)
	{
		atVmpMember[byChnlNum++] = m_tConf.GetChairman();
	}


	TVMPParam  tLastVMPParam   = m_tConf.m_tStatus.GetVmpParam(); 
	EqpLog("[GetVCSAutoVmpMember]last vmp param: \n");
	if(g_bPrintEqpMsg)
	{
		tLastVMPParam.Print();
	}

	// ����֮ǰ���õĻ���ϳɳ�Ա
	TMt tMt;
	u8 byLoop = 0;
	TVMPMember *ptMember = NULL;  
	u8 byLastMemNum = tLastVMPParam.GetMaxMemberNum();
	for (; byLoop < byLastMemNum; byLoop++)
	{
		tMt.SetNull();
		ptMember = tLastVMPParam.GetVmpMember(byLoop);
		memcpy(&tMt, ptMember, sizeof(TMt));
		if (tMt.IsNull() || tMt.GetMtId() > MAXNUM_CONF_MT)
		{
			continue;
		}

		if (m_tConf.GetChairman() == tMt)
		{
			continue;
		}
		// ��ֹ�������ǽ���뻭��ϳ���
		// ����VCS���鶨ʱ���е���ԴΪ�̶���Դ
		if (CONF_CALLMODE_TIMER == VCSGetCallMode(tMt))
		{
			continue;
		}
		
		// �ǻش�ͨ���е��¼��ն˲�������ϳɳ�Աͳ��
		if (!tMt.IsLocal())
		{
			TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMcuId());
			if (!(ptMcInfo->m_tMMcuViewMt == tMt))
			{
				continue;
			}
		}

		if (m_tConfAllMtInfo.MtJoinedConf(tMt.GetMcuId(), tMt.GetMtId()))
		{
			atVmpMember[byChnlNum++] = tMt;
		}
	}

	u8 byMtId    = 1;
	u8 byMaxMtId = MAXNUM_CONF_MT;
	if (ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
	{
		// �������ģʽ�»���ϳɳ�ԱΪ��һ�λ���ϳɳ�Ա + ��ǰ�����ն˳�Ա
		TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();
		if (tCurVCMT.IsNull() || !m_tConfAllMtInfo.MtJoinedConf(tCurVCMT.GetMcuId(), tCurVCMT.GetMtId()))
		{
			byMaxMtId = 0;
		}
		else
		{
			tCurVCMT = GetLocalMtFromOtherMcuMt(tCurVCMT);
			byMtId = tCurVCMT.GetMtId();
			byMaxMtId = byMtId;
		}
	}
	for (; byMtId <= byMaxMtId; byMtId++)
	{
		if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
		{
			if (byMtId == m_tConf.GetChairman().GetMtId()
			 || tLastVMPParam.IsMtInMember(m_ptMtTable->GetMt(byMtId))
			 || (CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtId) && 
				 m_ptMtTable->GetMtType(byMtId) != MT_TYPE_SMCU)
			 // ���ն˺���ͨ��δ�򿪣����ð�ռ����ϳ���ͨ��
			 || !m_ptMtTable->IsLogicChnnlOpen(byMtId, LOGCHL_VIDEO, FALSE))
			{
				continue;
			}

			if (MT_TYPE_SMCU == m_ptMtTable->GetMtType(byMtId))
			{
				TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(byMtId);
				if (ptMcInfo != NULL && !ptMcInfo->m_tMMcuViewMt.IsNull() &&
					m_tConfAllMtInfo.MtJoinedConf(ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId()) &&
					!tLastVMPParam.IsMtInMember(ptMcInfo->m_tMMcuViewMt))
				{
					atVmpMember[byChnlNum++] = ptMcInfo->m_tMMcuViewMt;
				}
			}
			else
			{
				atVmpMember[byChnlNum++] = m_ptMtTable->GetMt(byMtId);
			}

		}
	}

	// ȷ���³�Ա���뻭��ϳ�
	if (byChnlNum > byMaxMemberNum )
	{
		u16 wChanInd = m_cVCSConfStatus.GetCurUseVMPChanInd();		
		for (u8 byIdx = 0; byIdx < (byChnlNum - byMaxMemberNum); byIdx++)
		{
			wChanInd += byIdx;
			if (wChanInd >= byMaxMemberNum)
			{
				wChanInd = 1;
			}

			atVmpMember[wChanInd] = atVmpMember[byMaxMemberNum + byIdx];
			atVmpMember[byMaxMemberNum + byIdx].SetNull();
		}

	}

	u8 byFinalMemNum = min(byChnlNum, byMaxMemberNum);
	if (ptVmpMember != NULL)
	{
		memset(ptVmpMember, 0, sizeof(TMt) * MAXNUM_VMP_MEMBER);
		memcpy(ptVmpMember, atVmpMember, sizeof(TMt) * byFinalMemNum);
		EqpLog("[GetVCSAutoVmpMember] final vcs autovmp member:\n");
		if (g_bPrintEqpMsg)
		{
			for(u8 byIdx = 0; byIdx < byFinalMemNum; byIdx++)
			{
				OspPrintf(TRUE, FALSE, "%d.mcuid:%d mtid:%d\n", 
						  byIdx, ptVmpMember[byIdx].GetMcuId(), ptVmpMember[byIdx].GetMtId());
			}
		}

	}



	byVmpStyle = GetVmpDynStyle(byFinalMemNum);
	EqpLog("[GetVCSAutoVmpMember] final autovmp style: %d\n", byVmpStyle);

	return byVmpStyle;
}

/*====================================================================
    ������      ��SetVCSVmpMemInChanl
    ����        ����vmp��Աѡ��ͨ���������������������޳�����ϳɵ��ն˹Ҷ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[IN]pVmpMember����ͨ����Ӧ��Ա
	              [IN]byVmpSytle: ָ���Ļ���ϳɷ��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/23                ���㻪        ����
====================================================================*/
void CMcuVcInst::SetVCSVmpMemInChanl(TMt* pVmpMember, u8 byVmpSytle)
{
	if (NULL == pVmpMember || VMP_STYLE_NONE == byVmpSytle)
	{
		return;
	}

	// �����һ�γ�Ա
	TVMPParam  tLastVMPParam   = m_tConf.m_tStatus.GetVmpParam();

	u8         byMaxMemberNum  = tLastVMPParam.GetMaxMemberNum();
	u8         byLoop          = 0;
	for(byLoop = 0; byLoop < byMaxMemberNum; byLoop++ )
	{
		m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );			
	} 
	m_tConf.m_tStatus.SetVmpStyle(byVmpSytle);
	TVMPParam  tNewVMPParam = m_tConf.m_tStatus.GetVmpParam();

	// ������֯�ĳ�Ա������������Ӧ��ͨ����
	for (byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++)
	{
		TMt tMt = pVmpMember[byLoop];
		if (!tMt.IsNull())
		{
			SetVmpChnnl(tMt, byLoop, VMP_MEMBERTYPE_VAC);
			
			tNewVMPParam = m_tConf.m_tStatus.GetVmpParam();
			// �����ն˵ķֱ���
			ChangeMtVideoFormat(tMt, &tNewVMPParam);
		}
		else
		{
			// ��ԭ�����ն�
			TMt tOldMt = *(TMt*)tLastVMPParam.GetVmpMember(byLoop);
			if (!tOldMt.IsNull())
			{
				StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
			}
			
		}
	}

	// �Ҷϱ��߳�����ϳ����ĳ�Ա
	CServMsg cServMsg;
	if ( CONF_VMPMODE_NONE != tLastVMPParam.GetVMPMode() )
	{
		for( s32 nIndex = 0; nIndex < tLastVMPParam.GetMaxMemberNum(); nIndex ++ )
		{
			TMt tVMPMemberOld = *(TMt *)tLastVMPParam.GetVmpMember(nIndex);
        
			if ( !m_tConf.m_tStatus.m_tVMPParam.IsMtInMember(tVMPMemberOld) &&
				m_tConfAllMtInfo.MtJoinedConf(tVMPMemberOld.GetMcuId(), tVMPMemberOld.GetMtId())) 
			{
				// ChangeMtVideoFormat( tVMPMemberOld.GetMtId(), &m_tLastVmpParam, FALSE );
				if (!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
				{
					VCSDropMT(tVMPMemberOld);
				}
				// HD-VMP��ʱ�棬ֻ��¼��һ�ο��õĸ����滻ͨ��(1��2ͨ��)
				if (g_cMcuVcApp.IsDistinguishHDSDMt() &&
					m_tVmpEqp.GetEqpId() > (VMPID_MIN + 8) &&
					m_tVmpEqp.GetEqpId() <= VMPID_MAX)
				{
					if (nIndex > 0 && nIndex < 3)
					{
						m_cVCSConfStatus.SetCurUseVMPChanInd(nIndex + 1);
					}
				}
				else
				{
					m_cVCSConfStatus.SetCurUseVMPChanInd(nIndex + 1);
				}
			}
		}             
	}

}
/*====================================================================
    ������      ��ProcVCSChairOffLine
    ����        ��������ϯ�ն˲����߸����н�����ʾ���һָ�����״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/23                ���㻪        ����
====================================================================*/
void CMcuVcInst::VCSChairOffLineProc()
{
	if (VCS_CONF == m_tConf.GetConfSource() && 
		m_byCreateBy != CONF_CREATE_MT &&
		!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
	{
		// �������л�����������״̬
		RestoreVCConf(VCS_SINGLE_MODE);	

		CServMsg cServMsg;
		cServMsg.SetConfIdx(m_byConfIdx);
		cServMsg.SetConfId(m_tConf.GetConfId()) ;
		cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
		
		SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cServMsg);
		return;
	}
}
/*====================================================================
    ������      ��GetCallMode
    ����        �����ָ��ʵ��ĺ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt��ʵ����Ϣ
    ����ֵ˵��  ��CONF_CALLMODE_TIMER(���ɵ�����Դ)/CONF_CALLMODE_NONE(�ɵ�����Դ)
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/23                ���㻪        ����
====================================================================*/
u8 CMcuVcInst::VCSGetCallMode(TMt tMt)
{
	if (tMt.IsNull())
	{
		// ��Ч�նˣ���Ϊ���ɵ�����Դ
		return CONF_CALLMODE_TIMER;
	}

	if (tMt.IsLocal())
	{
		return m_ptMtTable->GetCallMode(tMt.GetMtId());
	}
	else
	{
		TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMcuId());
		if (ptMcInfo != NULL)
		{
			for(s32 nLoop = 0; nLoop < MAXNUM_CONF_MT; nLoop++)
			{
				if(ptMcInfo->m_atMtExt[nLoop].GetMtId() == tMt.GetMtId())
				{
					return ptMcInfo->m_atMtExt[nLoop].GetCallMode();
				}
			}
		}
	}

	return CONF_CALLMODE_TIMER;
}
/*====================================================================
    ������      ��ChangeVFormat
    ����        ���ı�ָ���ն˵���Ƶ�ֱ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tMt��ָ���ն�
	              byNewFormat: ָ���ı��ĸ�ʽ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/23                ���㻪        ����
====================================================================*/
void CMcuVcInst::ChangeVFormat(TMt tMt, u8 byNewFormat)
{
    if (tMt.IsNull() || tMt.GetType() != TYPE_MT || m_ptMtTable->GetManuId(tMt.GetMtId()) != MT_MANU_KDC )
    {
        VcsLog("[ChangeHDVFormat] dstmt.%d is not mt or kdc mt. type.%d manu.%d\n",
                tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return;
    }
    // ���ڸ����ն˷���������ǽ,��ȡ���ֱ���
    TLogicalChannel tSrcRvsChannl;
    if ( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
    {
		u8 byChnnlType   = tSrcRvsChannl.GetMediaType();
		u8 byChnnlFormat = byNewFormat;

		CServMsg cServMsg;
		cServMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
		cServMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
		cServMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
		SendMsgToMt( tMt.GetMtId(), cServMsg.GetEventId(), cServMsg );

		VcsLog( "[ChangeHDVFormat] mt(%d) videoformat change to %d\n",
			     tMt.GetMtId(), byChnnlFormat);

    }
    return;
}

/*====================================================================
    ������      ��VCSMTAbilityNotif
    ����        ��ָ���ն���������ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tMt��ָ���ն�
	              byMediaType: ���������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/23                ���㻪        ����
====================================================================*/
void CMcuVcInst::VCSMTAbilityNotif(TMt tMt, u8 byMediaType)
{
	//����VCS����, ����ʾ���ն��޷�����ͼ��
	if (VCS_CONF == m_tConf.GetConfSource())
	{
		CServMsg  cMsg;
		cMsg.SetConfId(m_tConf.GetConfId());
		cMsg.SetConfIdx(m_byConfIdx);
		cMsg.SetErrorCode(0);
		if (tMt == m_cVCSConfStatus.GetCurVCMT())
		{
			if (MODE_VIDEO == byMediaType)
			{
				cMsg.SetErrorCode(ERR_MCU_VCS_REMNOACCEPTVIDEO);
			}
			else if (MODE_AUDIO == byMediaType)
			{
				cMsg.SetErrorCode(ERR_MCU_VCS_REMNOACCEPTAUDIO);
			}
		}
		else if (tMt == m_tConf.GetChairman())
		{
			if (MODE_VIDEO == byMediaType)
			{
				cMsg.SetErrorCode(ERR_MCU_VCS_LOCNOACCEPTVIDEO);
			}
			else if (MODE_AUDIO == byMediaType)
			{
				cMsg.SetErrorCode(ERR_MCU_VCS_LOCNOACCEPTAUDIO);
			}
		}

		if (cMsg.GetErrorCode() != 0)
		{
			SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cMsg);
		}
	}	

}
/*====================================================================
    ������      ��ChangeSpecMixMember
    ����        ���޸Ķ��ƻ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����ptMixMember:ָ���ƻ�����Ա��ָ��
	              byMemberNum:���ƻ�����Ա��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/04/14                ���㻪        ����
====================================================================*/
void CMcuVcInst::ChangeSpecMixMember(TMt* ptMixMember, u8 byMemberNum)
{
	if(!m_tConf.m_tStatus.IsMixing())
	{
		OspPrintf(TRUE, FALSE, "[ChangeSpecMixMember]mix has not started\n");
		return;
	}

	TMt tMt;
	u8  byInMixMem = FALSE;
	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
		{
			continue;
		}

		byInMixMem = FALSE;
		for (u8 byMtIdx = 0; byMtIdx < byMemberNum; byMtIdx++)
		{
			if (ptMixMember[byMtIdx].GetMtId() == byMtId)
			{
				byInMixMem = TRUE;
				break;
			}
		}

		tMt = m_ptMtTable->GetMt(byMtId);
		if (m_ptMtTable->IsMtInMixGrp(byMtId) && !byInMixMem)
		{
			AddRemoveSpecMixMember(&tMt, 1, FALSE);
		}
		
		if (!m_ptMtTable->IsMtInMixGrp(byMtId) && byInMixMem)
		{
			AddRemoveSpecMixMember(&tMt, 1, TRUE);
		}
	}

}
/*====================================================================
    ������      ��VCSConfStatusNotif
    ����        ��֪ͨ����VCS����״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/04/14                ���㻪        ����
====================================================================*/
void CMcuVcInst::VCSConfStatusNotif()
{
	CServMsg cServMsg;
	cServMsg.SetConfId(m_tConf.GetConfId());
	cServMsg.SetConfIdx(m_byConfIdx);
	cServMsg.SetEventId(MCU_VCS_CONFSTATUS_NOTIF);
	cServMsg.SetMsgBody( (u8*)&m_cVCSConfStatus, sizeof(CBasicVCCStatus) );
	SendMsgToAllMcs(MCU_VCS_CONFSTATUS_NOTIF, cServMsg);
}
/*====================================================================
    ������      ��VCSGetNextPollMt
    ����        ����ȡ��һ����ѯ�ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/04/14                ���㻪        ����
====================================================================*/
TMt CMcuVcInst::VCSGetNextPollMt()
{
	u8  byMtId = 0;
	TMt tCurPollMt = m_cVCSConfStatus.GetCurChairPollMt();
	if (!tCurPollMt.IsNull())
	{
		byMtId = tCurPollMt.GetMtId();
	}
	byMtId++;

	if (byMtId > MAXNUM_CONF_MT)
	{
		byMtId = 1;
	}

	u8  byStartMtId   = byMtId;
	u8  byLoopRestart = FALSE; 
	TMt tNextPollMt;
	tNextPollMt.SetNull();
    for (; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (byStartMtId == byMtId && byLoopRestart)
		{
			break;
		}

		if (m_tConfAllMtInfo.MtJoinedConf((byMtId)) && 
			m_tConf.GetChairman().GetMtId() != byMtId && 
			m_ptMtTable->GetCallMode(byMtId) != CONF_CALLMODE_TIMER)
		{
			tNextPollMt = m_ptMtTable->GetMt(byMtId);
			break;
		}

		if (MAXNUM_CONF_MT == byMtId)
		{
			byMtId = 0;
			byLoopRestart = TRUE;
		}
	}
	
	return tNextPollMt;
}

/*====================================================================
    ������      ��IsNeedHdSelApt
    ����        ���ж����ն�ѡ���Ƿ���Ҫѡ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����bySwitchMode:MODE_VIDEO, MODE_AUDIO
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/04/14                ���㻪        ����
====================================================================*/
BOOL CMcuVcInst::IsNeedSelApt(u8 bySrcMtId, u8 byDstMtId, u8 bySwitchMode)
{
	if (bySwitchMode != MODE_AUDIO && bySwitchMode != MODE_VIDEO)
	{
		ConfLog(FALSE, "[IsNeedHdSelApt] wrong input param about bySwitchMode(%d)\n", bySwitchMode);
		return FALSE;
	}

	// ��Ƶѡ��
    if(MODE_VIDEO == bySwitchMode)
    {
		//ȡԴ�ն���Ŀ���ն˵�������
		TLogicalChannel tSrcLogChnl;
		TLogicalChannel tDstLogChnl;
		m_ptMtTable->GetMtLogicChnnl(bySrcMtId, LOGCHL_VIDEO, &tSrcLogChnl, FALSE);
		m_ptMtTable->GetMtLogicChnnl(byDstMtId, LOGCHL_VIDEO, &tDstLogChnl, TRUE);

		//����POLOCOM�ն˽���ѡ����֧�ֲ����зֱ����жϣ�ֱ�ӽ�����
		if (g_cMcuVcApp.IsVidAdjustless4Polycom() && MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byDstMtId))
		{
			ConfLog(FALSE, "[IsNeedHdSelApt] polocom no need to use bas\n");
			return FALSE;
		}
		// ����ʽ��һ��
		else if (tSrcLogChnl.GetChannelType() != tDstLogChnl.GetChannelType())
		{
			ConfLog(FALSE, "[IsNeedHdSelApt] src type.%d matchless with dst type.%d, ocuupy new adp!\n",
				    tSrcLogChnl.GetChannelType(), tDstLogChnl.GetChannelType());
            return TRUE;
		}
		else if (IsResG(tSrcLogChnl.GetVideoFormat(), tDstLogChnl.GetVideoFormat()))
		{
			ConfLog(FALSE, "[IsNeedHdSelApt] src res.%d matchless with dst res.%d, ocuupy new adp!\n",
				    tSrcLogChnl.GetVideoFormat(), tDstLogChnl.GetVideoFormat());
			return TRUE;
		}

	}

	// ��Ƶѡ��
	if (MODE_AUDIO == bySwitchMode)
	{
		//ȡԴ�ն���Ŀ���ն˵�������
		TLogicalChannel tSrcLogChnl;
		TLogicalChannel tDstLogChnl;
		m_ptMtTable->GetMtLogicChnnl(bySrcMtId, LOGCHL_AUDIO, &tSrcLogChnl, FALSE);
		m_ptMtTable->GetMtLogicChnnl(byDstMtId, LOGCHL_AUDIO, &tDstLogChnl, TRUE);

		if (tSrcLogChnl.GetChannelType() != tDstLogChnl.GetChannelType())
		{
			ConfLog(FALSE, "[IsNeedHdSelApt] src audio type.%d matchless with dst audio type.%d, ocuupy new adp!\n",
				    tSrcLogChnl.GetChannelType(), tDstLogChnl.GetChannelType());
			return TRUE;
		}
	}
	return FALSE;
}

