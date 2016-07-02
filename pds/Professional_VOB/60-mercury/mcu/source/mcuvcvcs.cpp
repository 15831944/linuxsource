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
//#include "eqpssn.h"
#include "mtadpssn.h"
//#include "mcuutility.h"

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
    ������      ��DaemonProcVcsMcuSendPackinfoRqp
    ����        ��VCS���󱣴������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪          ����
====================================================================*/
void CMcuVcInst::DaemonProcVcsMcuSavePackinfoReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	if (cServMsg.GetTotalPktNum() <= cServMsg.GetCurPktIdx())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[SavePack]uncorrect messagebody(total num:%d  curidx:%d)\n",
		          cServMsg.GetTotalPktNum(), cServMsg.GetCurPktIdx());
		cServMsg.SetMsgBody();
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		return;
	}

	CConfId cConfId = cServMsg.GetConfId();	
	// �Բ����ڵĵ���ϯ/�����ȵĵ���ϯ���ɸ��·�����Ϣ
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	u8      byConfPos  = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;
	
	GetAllConfHeadFromFile(acConfId, sizeof(acConfId));
	for (s32 nPos = 0; nPos < (MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE); nPos++)
	{
		if (acConfId[nPos] == cServMsg.GetConfId())
		{
			byConfPos = (u8)nPos;
			break;
		}
	}
	
	if (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE == byConfPos)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "confinfo_head.dat has no temp with specified confid\n");
		cServMsg.SetErrorCode(ERR_MCU_TEMPLATE_NOTEXIST);
		cServMsg.SetMsgBody();
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
	}
	else
	{
		u8 byEditer = g_cMcuVcApp.GetTempEditerInfo(byConfPos);
		if (byEditer && byEditer != cServMsg.GetSrcSsnId())
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS,"reject request from inst%d, because Inst%d is in modification of tempid",
				      cServMsg.GetSrcSsnId(), byEditer);
			cServMsg.GetConfId().Print();

		    cServMsg.SetErrorCode(ERR_MCU_TEMPLATE_INEDITING);
			cServMsg.SetMsgBody();
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
		}

		if (g_cMcuVcApp.IsConfOnGoing(cConfId))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS,"����ϯ���ڱ����ȣ����ɽ����޸�\n");
		    cServMsg.SetErrorCode(ERR_MCU_TEMPLATE_INVC);
			cServMsg.SetMsgBody();
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
		}

		if (!byEditer)
		{
			g_cMcuVcApp.SetTempEditerInfo(byConfPos, cServMsg.GetSrcSsnId());
		}

		KillTimer(MCUVC_PACKINFO_TIMER + byConfPos);
	
		BOOL32 bOverWrite = !cServMsg.GetCurPktIdx();
		SetUnProConfDataToFile(byConfPos, (s8*)cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen(),bOverWrite );
		cServMsg.SetMsgBody();
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

		if (cServMsg.GetCurPktIdx() == cServMsg.GetTotalPktNum() - 1)
		{
			NotifyVCSPackInfo(cConfId);
			g_cMcuVcApp.SetTempEditerInfo(byConfPos, 0);
		}
		else
		{
			SetTimer(MCUVC_PACKINFO_TIMER + byConfPos, 3000, byConfPos);			
		}
	}

	return;
}

/*====================================================================
    ������      ��DaemonProcPackInfoTimer
    ����        ��������Ϣ��ȡ��ʱ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/11/28                ���㻪          ����
====================================================================*/
void CMcuVcInst::DaemonProcPackInfoTimer( const CMessage * pcMsg )
{
	KillTimer(pcMsg->event);

	u8 byConfIdx = *(pcMsg->content);
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	GetAllConfHeadFromFile(acConfId, sizeof(acConfId));
	CConfId cConfId = acConfId[byConfIdx];
	ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Warning: pack info is not complete for confid:");
	cConfId.Print();

	NotifyVCSPackInfo(cConfId);
	g_cMcuVcApp.SetTempEditerInfo(byConfIdx, 0);

}
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

    CServMsg  cServMsg( pcMsg->content, pcMsg->length );

    TConfInfo tTmpConf = *(TConfInfo *)cServMsg.GetMsgBody();
    BOOL32 bConfFromFile = tTmpConf.m_tStatus.IsTakeFromFile();
    tTmpConf.m_tStatus.SetTakeFromFile(FALSE);

	
	//�Ƚ���tConfInfoEx��Ϣ��BAS����Դ�Ƿ��㹻���õ�[12/29/2011 chendaiwei]
	TConfInfoEx tConfInfoEx;
	if(GetConfExInfoFromMsg(cServMsg,tConfInfoEx))
	{
		tTmpConf.SetHasConfExFlag(TRUE);
	}

	m_tConfEx = tConfInfoEx;

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
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Conference %s failure because exceed max conf num!\n", tTmpConf.GetConfName() );
				return;
			}

			if (0 == g_cMcuVcApp.GetMpNum() || 0 == g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323))
			{
				cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Conference %s failure because No_Mp or No_MtAdp!\n", tTmpConf.GetConfName() );
				return;
			}
			
			//�����ն�������
			if( NULL == m_ptMtTable )
			{
				m_ptMtTable = new TConfMtTable;
				START_CONF_CHECK_MEMORY(m_ptMtTable, cServMsg, tTmpConf);
			}

			memset( m_ptMtTable, 0, sizeof( TConfMtTable ) );

			NewMcuBasMgr();
			ClearBasMgr();
			
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
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Conference %s failure for conf full!\n", tTmpConf.GetConfName() );
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
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "MVidType.%d with FPS.%d is unexpected, adjust it\n",
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
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "SVidType.%d with FPS.%d is unexpected, adjust it\n",
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
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "DSVidType.%d with FPS.%d is unexpected, adjust it\n",
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
			m_tConfAllMcuInfo.Clear();
        
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
//         
			//���������ģʽΪ���ն��Զ�������Ӧ�����ն�
			if(0 == byMtNum && m_tConf.GetConfAttrb().IsReleaseNoMt())
			{
				ClearHduSchemInfo();
				CreateConfSendMcsNack( byOldInsId, ERR_MCU_NOMTINCONF, cServMsg );
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Conference %s create failed because on mt in conf!\n", m_tConf.GetConfName() );
				return;
			}

			//MCU�����������Ƿ���
			if ( !IsVCSEqpInfoCheckPass(wErrCode))
			{
				ClearHduSchemInfo();
				CreateConfEqpInsufficientNack( byCreateMtId, byOldInsId, wErrCode, cServMsg );
				return;
			}

			// ���鴦��: ��ʱ����, ��ԤԼ����
			if( NULL == m_ptSwitchTable )
			{
				m_ptSwitchTable = new TConfSwitchTable;
				START_CONF_CHECK_MEMORY(m_ptSwitchTable, cServMsg, tTmpConf);
			}
			if( NULL == m_ptConfOtherMcTable )
			{
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

// 			//MCU�����������Ƿ���
// 			if ( !IsVCSEqpInfoCheckPass(wErrCode))
// 			{
// 				CreateConfEqpInsufficientNack( byCreateMtId, byOldInsId, wErrCode, cServMsg );
// 				return;
// 			}

			TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();

			if( tConfAttrib.IsDiscussConf() )
			{
				// ���� [4/29/2006] ����������ȫ�����
				//tianzhiyong 2010/03/21 ���ӿ���ģʽ
				StartMixing(mcuWholeMix);
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
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "conference %s created and started!\n",m_tConf.GetConfName());
			}
			else if ( CONF_DATAMODE_VAANDDATA == m_tConf.GetConfAttrb().GetDataMode() )
			{
				//��Ѷ������
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "conference %s created and started with data conf function !\n", m_tConf.GetConfName());
			}
			else
			{
				//���ݻ���
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "conference %s created and started with data conf function Only !\n", m_tConf.GetConfName());
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
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "charge postponed due to GetRegGKDriId.%d, ChargeRegOK.%d !\n",
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

			InviteUnjoinedMt(cServMsg);

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
			m_tConfAllMtInfo.m_tLocalMtInfo.SetMcuIdx( LOCAL_MCUIDX );
			//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
			//cServMsg.CatMsgBody( (u8 *)&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
			SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
        
			//��������ն˱� 
			SendMtListToMcs(LOCAL_MCUIDX);	

			// ���͸���ص�ǰ�ĵ��Ȼ����ģʽ
			// Ĭ�ϴ���ʱ��Ϊ��������ģʽ
			u8 byAutoMode = m_cVCSConfStatus.GetCurVCMode();
			m_cVCSConfStatus.VCCDefaultStatus();
			if( 0 != byAutoMode)
			{
				RestoreVCConf(byAutoMode);
			}
			else
			{
				VCSConfStatusNotif();
			}
			
			
			u8 byScrId = cServMsg.GetSrcSsnId();
			if (byScrId != 0)
			{
				m_cVCSConfStatus.SetCurSrcSsnId(byScrId);
			}

			if (CONF_CREATE_MT == m_byCreateBy)
			{
				ProcMMCUGetConfCtrlMsg();
				//zjj20100329 vcs�¼����������ն˳�ʼ��Ϊ�Զ�����
				for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
				{
					if (m_tConfAllMtInfo.MtInConf(byMtId) &&
						!m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
						m_ptMtTable->GetMtType(byMtId) != MT_TYPE_MMCU)
					{						
						m_ptMtTable->SetCallMode( byMtId,CONF_CALLMODE_TIMER );
					}	
				}
			}


			//zjj20091102 ��ȡ�ļ�������Ϣ�ļ��е�Ԥ����Ϣ
			if(!m_cCfgInfoRileExtraDataMgr.ReadExtraDataFromConfInfoFile(m_byConfIdx))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "ReadExtraDataFromConfInfoFile faield!\n");
			}
		}
           
		break;
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	
	return;
}
/*====================================================================
    ������      ��GetVCMTByAlias
    ����        �������ն˱�����ȡ�ն˵�TMt��Ϣ,�����ն���δ����ID��Ϣ��
	              ������Ӧ��Դ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tMtAlias��   �ն˱���
	              wMtDialRate���ն˺�������
				  byForce:     �Ƿ�ȷ����Դ����ɹ���
				               ��id��Դ�ѱ��ľ����ͷ������������ն�(����ϯ ������)
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/12/08                ���㻪          ����
====================================================================*/
TMt CMcuVcInst::GetVCMTByAlias(TMtAlias& tMtAlias, u16 wMtDialRate, u8 byForce)
{
	u8 byMtId = AddMt(tMtAlias, wMtDialRate, CONF_CALLMODE_NONE);
	if (byForce && !byMtId)
	{
		// �ͷ�һ������Դ����Ӧ�նˣ������ͷ���ϯ �����˻����ڵ���ǽ�е��ն�
		for (u8 byMtIdLoop = 1; byMtIdLoop <= MAXNUM_CONF_MT; byMtIdLoop++)
		{
			if (!m_tConfAllMtInfo.MtJoinedConf(byMtIdLoop) &&
				m_tConf.GetChairman().GetMtId() != byMtIdLoop &&
				m_tConf.GetSpeaker().GetMtId() != byMtIdLoop &&
				!m_ptMtTable->IsMtInHdu(byMtIdLoop) &&
				!m_ptMtTable->IsMtInTvWall(byMtIdLoop))
			{
                TMt tMt = m_ptMtTable->GetMt(byMtIdLoop); 
				RemoveMt(tMt, TRUE);
				break;
			}
		}

		byMtId = AddMt(tMtAlias, wMtDialRate, CONF_CALLMODE_NONE);
	}

	//������ػ��������ն���Ϣ
	CServMsg cServMsg;
	//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
	//cServMsg.CatMsgBody( (u8 *)&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//��������ն˱� 
	SendMtListToMcs(LOCAL_MCUIDX);	

	return m_ptMtTable->GetMt(byMtId);

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
			//action abort when no chairman online
			TMt tChairMan = m_tConf.GetChairman();
			if( ( tChairMan.IsNull()  || 
				!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
				m_byCreateBy != CONF_CREATE_MT 
				)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			u8* pbyMsgBody = cServMsg.GetMsgBody();

			TMt tVCMT;
			u8 byMTDtype = *pbyMsgBody++;
			if (VCS_DTYPE_MTID == byMTDtype)
			{
				tVCMT = *(TMt*)pbyMsgBody;
				pbyMsgBody += sizeof(TMt);
			}
			else if (VCS_DTYPE_MTALIAS == byMTDtype)
			{
				u8 byAliasType = *pbyMsgBody++;
				u8 byAliasLen  = *pbyMsgBody++;
				TMtAlias tMtAlias;
				if (!tMtAlias.SetAlias(byAliasType, byAliasLen, (s8*)pbyMsgBody))
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "bad alias info(type:%d byaliaslen:%d)\n",
							  byAliasType, byAliasLen);
					return;
				}
				pbyMsgBody += byAliasLen;
				
				u8 byForce = !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode());
				tVCMT = GetVCMTByAlias(tMtAlias, ntohs( *(u16*)pbyMsgBody ),byForce);
				pbyMsgBody += sizeof(u16);
			}
			else
			{
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "bad mt description type(%d)\n",
					      byMTDtype);
				return;
			}


			u8 byVCType  = *(u8*)pbyMsgBody;
			pbyMsgBody += sizeof(u8); 

			//zhouyiliang 20100824 vcs������ȵ��Ǳ��ر����ն˵�ʱ��return����������
			if ( m_cVCSConfStatus.GetVcsBackupChairMan() ==  tVCMT ) 
			{
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "mt:%d is vcs backupchairman ,can't call \n",
					      tVCMT.GetMtId());
				return;
			}

			//ǿ�� 
			u8 byForceCallType = *(u8*)pbyMsgBody; 
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "operate(vctype:%d, forcecalltype:%d) mt(mcuid:%d mtid:%d) byMTDtype.%d req\n",
				    byVCType, byForceCallType, tVCMT.GetMcuId(), tVCMT.GetMtId(),byMTDtype );


			//zjj, �����ģʽ��,ǿ�����Ͳ������ȴ���,ֻ������
			if( VCS_FORCECALL_CMD == byForceCallType )
			{
                //[11/23/2011 zhushengze]�ն��Ѿ���ᣬ����ǿ��
                if (m_tConfAllMtInfo.MtJoinedConf(tVCMT.GetMcuId(), tVCMT.GetMtId()))
                {
                    cServMsg.SetEventId(cServMsg.GetEventId() + 2);
                    SendReplyBack(cServMsg, cServMsg.GetEventId());
                    LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ProcVcsMcuVCMTReq]mt.%d has joined conf",tVCMT.GetMtId());
                    return;
                }

				if( ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() ) )
				{
					VCSCallMT( cServMsg,tVCMT,byForceCallType );
					return;
				}
				else if( m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE &&
					( !IsMtNotInOtherHduChnnl(tVCMT,0,0) || !IsMtNotInOtherTvWallChnnl(tVCMT,0,0) ) )
				{
					VCSCallMT( cServMsg,tVCMT,byForceCallType );
					return;
				}				
			}

			//zjj20100325  ȡ��ǿ����ն�Ҫ�ĳ��ֶ�����(����ϲ�)
			if( VCS_FORCECALL_CANCELCMD == byForceCallType )
			{
				if( ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() ) )
				{
					m_ptMtTable->SetCallMode( GetLocalMtFromOtherMcuMt(tVCMT).GetMtId(),CONF_CALLMODE_NONE );
					cServMsg.SetEventId(cServMsg.GetEventId() + 1);
					SendReplyBack(cServMsg, cServMsg.GetEventId());
				}
				else
				{
					VCSClearTvWallChannelByMt( tVCMT,TRUE );
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Error, not GroupMode, can't recv VCS_FORCECALL_CANCELCMD.\n");
					cServMsg.SetEventId(cServMsg.GetEventId() + 2);
					SendReplyBack(cServMsg, cServMsg.GetEventId());
				}
				return;
			}
			// �Ե�����Դ����Ч�Խ����ж�,�����������б��У��Ҳ�Ϊ��ʱ���еĹ̶��ǿɵ�����Դ(�������ǽ�����նˡ��¼�mcu)
			if (!m_tConfAllMtInfo.MtInConf(tVCMT.GetMcuId(), tVCMT.GetMtId())
				//|| (CONF_CALLMODE_TIMER == VCSGetCallMode(tVCMT))
				)
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

			// ��Զ෽�໭��,�������һ���ն�,��������ȵ��ն�δ��ɵ���ǰ������Ӧ,����ϳ���δ������ȫҲ������Ӧ
			if ( VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() )
			{
				// ��������ȵ��ն�δ��ɵ���ǰ������Ӧ
				if (!tOldReqVCMT.IsNull())
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_VCMTING);
					cServMsg.SetEventId(cServMsg.GetEventId() + 2);				
					cServMsg.SetMsgBody((u8*)&tOldReqVCMT, sizeof(TMt));
					SendReplyBack(cServMsg, cServMsg.GetEventId());
					return;
				}
				// ����ϳ���δ������ȫҲ������Ӧ
				if (!IsChangeVmpParam())
				{
					cServMsg.SetErrorCode(ERR_VMPBUSY);
					cServMsg.SetEventId(cServMsg.GetEventId() + 2);
					SendReplyBack(cServMsg, cServMsg.GetEventId());
					return;
				}
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
			
			if (byVCMTOnline && !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) )
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
						ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Error, non-curVCMT on line\n");
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
			case VCS_GROUPCHAIRMAN_MODE:
			case VCS_GROUPROLLCALL_MODE:
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
						if( tVCMT.IsNull() 
							&& CONF_SPEAKMODE_ANSWERINSTANTLY == m_tConf.GetConfSpeakMode() 
							&& !m_tApplySpeakQue.IsQueueNull()
							)
						{
							if(  m_tApplySpeakQue.GetQueueHead( tVCMT ) )
							{
								ChgCurVCMT( tVCMT );	
							}
							else
							{
								ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Fail to get Queue Head\n" );
							}
						}
						else
						{
							ChgCurVCMT(tVCMT);
						}						
					}
				}
				break;
			default:
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Wrong work mode\n");
				break;
			}
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Wrong message %u(%s) received in state %u!\n", 
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
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Wrong message %u(%s) received in state %u!\n", 
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
			

			if( byNewMode != VCS_SINGLE_MODE &&
				byNewMode != VCS_MULVMP_MODE &&
				byNewMode != VCS_GROUPSPEAK_MODE &&
				byNewMode != VCS_GROUPVMP_MODE &&
				byNewMode != VCS_GROUPCHAIRMAN_MODE &&
				byNewMode != VCS_GROUPROLLCALL_MODE )
			{
				ConfPrint( LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuVCModeChgReq] error mode.%d\n",byNewMode );
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			//action abort when no chairman online
			TMt tChairMan = m_tConf.GetChairman();
			if( ( tChairMan.IsNull()  || 
				!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
				m_byCreateBy != CONF_CREATE_MT
				)
			{
				// change to mode other than single mode is not allowed 
				if ( VCS_SINGLE_MODE !=  byNewMode ) 
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
					cServMsg.SetEventId(cServMsg.GetEventId() + 2);
					SendReplyBack(cServMsg, cServMsg.GetEventId());
					return;						
				}
			}

			u8 byOldMode = m_cVCSConfStatus.GetCurVCMode();
			if ( byOldMode != byNewMode )
			{
				// �л�����ģʽ,���������Ƿ��㹻
				if (VCS_MULTW_MODE == byNewMode || VCS_GROUPTW_MODE == byNewMode)
				{
					// [5/30/2011 liuxu] ��д��FindUsableTWChan, ��FindNextTvwChnnl����
					if( FindNextTvwChnnl(m_cVCSConfStatus.GetCurUseTWChanInd()) )
					/*if (!FindUsableTWChan(m_cVCSConfStatus.GetCurUseTWChanInd()))*/
					{
						// δ�ҵ����õ�ͨ����������ܾ�
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcVcsMcuVCModeChgReq] Find no usable TW!\n");
						cServMsg.SetErrorCode(ERR_MCU_VCS_NOUSABLETW);
						SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
						return;
					}
				}
				//zjj20091102 �����������ģʽ��ҲҪ����ϳ�
				//zjj20091102 ��Ϊ���������ģʽ���������ϳ�ģʽ��Ҫ�û���ϳ���������������ģʽ����ֱ���л���
				//������������ģʽ֮���л�Ҳ����ȥ�һ���ϳ��豸��
				else if (VCS_MULVMP_MODE == byNewMode || 
						(
							( VCS_GROUPVMP_MODE == byNewMode ||VCS_GROUPROLLCALL_MODE == byNewMode ) &&
							!( VCS_GROUPVMP_MODE == byNewMode && VCS_GROUPROLLCALL_MODE == byOldMode ) && 
							!( VCS_GROUPROLLCALL_MODE == byNewMode && VCS_GROUPVMP_MODE == byOldMode )
						)
				)
				{
					// ��ѯ�Ƿ���ڿ��õ�VMP,���ھ���ռ��
					if (!FindUsableVMP())
					{
						// δ�ҵ����õĻ���ϳ�������ܾ�
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcVcsMcuVCModeChgReq] Find no usable VMP!\n");
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
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "No change about vcmode\n");
				cServMsg.SetErrorCode(ERR_MCU_VCS_NOMODECHG);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			}
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuVCModeChgReq] Wrong message %u(%s) received in state %u!\n", 
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
void CMcuVcInst::ProcVcsMcuMuteReq( CServMsg& cServMsg )
{
/*	CServMsg cServMsg(pcMsg->content, pcMsg->length);*/

	switch(CurState())
	{
	case STATE_ONGOING:
		{
		    if (cServMsg.GetMsgBodyLen() < sizeof(u8) * 4)
		    {
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuMuteReq] invalid param\n");
				return;
		    }	
			
			//action abort when no chairman online
			TMt tChairMan = m_tConf.GetChairman();
			if( ( tChairMan.IsNull()  || 
				!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
				m_byCreateBy != CONF_CREATE_MT
				)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			BOOL32 bCmdFrmOtherMcu = FALSE;

			if ( cServMsg.GetMsgBodyLen() >= 5 )
			{
				bCmdFrmOtherMcu = *(u8*)(cServMsg.GetMsgBody() + 4 * sizeof(u8)) ? TRUE : FALSE;
			}

			u8 byMode = *(u8*)cServMsg.GetMsgBody();
			if( !bCmdFrmOtherMcu && byMode != m_cVCSConfStatus.GetCurVCMode() )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "Mode changed, operation cancel.( byMode:%u, CurVCMode: %u )\n",
					byMode, m_cVCSConfStatus.GetCurVCMode());
				cServMsg.SetErrorCode(ERR_MCU_VCS_CANCEL);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
			
			u8  byMuteOpenFlag = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8));         //�Ƿ���
			u8  byOprObj       = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8) * 2);     //��������/Զ��
			u8  byAudProcType  = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8) * 3);     //������/����
			
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "MuteReq: %s, %s, %s\n", 
				(byOprObj != VCS_OPR_LOCAL) ? "remote" : "local", 
				byAudProcType == VCS_AUDPROC_MUTE ? "dumb" : "mute",
				byMuteOpenFlag ? "open" : "close");

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
					if (m_tConfAllMtInfo.MtJoinedConf(byMTId)  
						//(CONF_CALLMODE_NONE == m_ptMtTable->GetCallMode(byMTId) || MT_TYPE_SMCU == m_ptMtTable->GetMtType(byMTId)))
						//m_tConf.GetChairman().GetMtId() != byMTId 
						)
					{
						TMt tMt = m_ptMtTable->GetMt(byMTId);
						if (!tMt.IsNull())
						{
							// [9/19/2010 xliang] filter chairman optionally
							if ( !bCmdFrmOtherMcu && m_tConf.GetChairman().GetMtId() == byMTId)
							{
								continue;
							}

							// [9/17/2010 xliang] filter mmcu
							if(  MT_TYPE_MMCU == tMt.GetMtType() )
							{
								continue;
							}

							VCSMTMute(tMt, byMuteOpenFlag, byAudProcType);
							
							//////////////////////////////////////////////////////////////////
							/*
							Զ�˾������ı��������Զ�˾���Ϊ�������������£�֮ǰ�������Զ��(�¼��ն�)�ղ����κ�����,
							�����ϼ����������Լ��¼������͸����ġ������Ҫ�ϼ�͸����Ϣ���¼�������Ĵ������͸����
							Ϣ���߼�����ǰ���ǵ�ǰ�¼��ն˶�������Ϻ�������Զ�˾�����
							����ϼ���δ�����κ��ն�ʱ��������Զ�˾���������ٵ����¼��նˣ���ʱ��Զ�˾������Ĵ�����
							MtOnlineChange�У���ʱ��Ϣ�����ڼ���ͨ����δ��ͨ��ϣ����޷����ﵽ�¼���

							����������ԾͲ��Գƣ�Ҫ�ԳƵĻ����Ķ���Ƚϴ��Ҵ�������������Ϣ���ؽ���ܴ�
							�����ǹ���Զ�˾����µ���⣺Զ�˲������˵���Ƶ�����������У��Ǳ����ļ�ΪԶ�ˣ��ϼ������ˡ�
							�������¼��ն˲����ϼ�����Ƶ�����ǿ����պ���ͬ���Լ����¼�����Ƶ��

							���������£��ϼ����¼�MCU˵Ҫ���ľ����ڱ�Ҫ�ط��𽨽�����������͸����Ϣ���¼���
							*/
							////////////////////////////////////////////////////////////////////
							// [2010/09/06 xliang] pass the msg to smcu
// 							if( MT_TYPE_SMCU == tMt.GetMtType() )
// 							{
// 								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVcsMcuMuteReq] pass the msg to mt.%d(smcu)!\n", byMTId );
// 
// 								SendMsgToMt(byMTId, VCS_MCU_MUTE_REQ, cServMsg);
// 							}
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
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuMuteReq] Wrong message %u(%s) received in state %u!\n", 
			     cServMsg.GetEventId(), ::OspEventDesc( cServMsg.GetEventId() ), CurState() );
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
void CMcuVcInst::ProcVCMTOverTime( void )
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
			if( !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) )
			{
				VCSClearTvWallChannelByMt( tReqVCMT,TRUE );
			}
			RemoveJoinedMt( tReqVCMT, TRUE );
		}

		TMt tNull;
		tNull.SetNull();
		m_cVCSConfStatus.SetReqVCMT(tNull);
	}

}

/*====================================================================
    ������      : VCSFindMtInTvWallAndChangeSwitch
    ����        ��ֹͣ��ָ��ն˵�����ǽ�صĽ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
	11/05/26                ����            ��������
====================================================================*/
void CMcuVcInst::VCSFindMtInTvWallAndChangeSwitch( TMt tMt,BOOL32 bIsStopSwitch,u8 byMode )
{
	//�˺�������ֻ֧�ֲ���Ƶ����Ƶ������������֧��
	if( MODE_BOTH == byMode )
	{
		return;
	}

	// һ������֧�ֵĵ���ǽͨ��������
	const u8 byMaxTvwNum = MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL + MAXNUM_PERIEQP_CHNNL * MAXNUM_MAP;
	CConfTvwChnnl acTvwChnnl[ byMaxTvwNum ];
	memset(acTvwChnnl, 0, sizeof(acTvwChnnl));

	u8 byTvwId, byTvwChnnlIdx;

	// ����Դ��������
	u8 byFindNum = FindAllTvwChnnl(tMt, acTvwChnnl, byMaxTvwNum);
	for (u8 byLoop = 0; byLoop < byFindNum; ++byLoop)
	{
		// ��ͨ�����ڴ˻�����
		if (acTvwChnnl[byLoop].GetConfIdx() != m_byConfIdx)
		{
			continue;
		}
		
		// ������, ����������
		if (!g_cMcuVcApp.IsPeriEqpConnected(acTvwChnnl[byLoop].GetEqpId()))
		{
			continue;
		}
		
		byTvwId = acTvwChnnl[byLoop].GetEqpId();
		byTvwChnnlIdx = acTvwChnnl[byLoop].GetChnnlIdx();
		
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU, "Tvwall channel(%d, %d) is mt(%d,%d) bIsStopSwitch(%d)\n",
			byTvwId,
			byTvwChnnlIdx,
			acTvwChnnl[byLoop].GetMember().GetMcuId(),
			acTvwChnnl[byLoop].GetMember().GetMtId(),
			bIsStopSwitch);
		
		if( bIsStopSwitch )
		{
			StopSwitchToPeriEqp( byTvwId, byTvwChnnlIdx, FALSE, byMode );
		}
		else
		{
			StartSwitchToPeriEqp( tMt, 0, byTvwId, byTvwChnnlIdx, byMode, SWITCH_MODE_SELECT,FALSE,TRUE,FALSE,TRUE,FALSE);
		}
	}
}

/*====================================================================
    ������      ProcVcsMcuTvWallMtStartMixing
    ����        ����ʼ����ǽͨ���е��ն˻���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuTvWallMtStartMixing( const CMessage *pcMsg )
{
	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	u8 byIsStartMixing = *(u8*)cServMsg.GetMsgBody();

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "byIsStartMixing is %d\n",byIsStartMixing );

	if( m_cVCSConfStatus.GetMtInTvWallCanMixing() == byIsStartMixing )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "Cur TvWall canMixing(%d) .needn't change\n",m_cVCSConfStatus.GetMtInTvWallCanMixing() );		
		return;
	}
	
	BOOL32 bStartMix = (1 == byIsStartMixing) ? TRUE : FALSE;

	if( m_cVCSConfStatus.GetCurVCMode() != VCS_SINGLE_MODE && m_tConf.m_tStatus.IsNoMixing() )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "Fail to change status because Current mode is not single mode(%d) and no mixing.\n",m_cVCSConfStatus.GetCurVCMode() );
		return;
	}

	m_cVCSConfStatus.SetMtInTvWallCanMixing( bStartMix );
	
	VCSChangeTvWallMtMixStatus( bStartMix );

	VCSConfStatusNotif();

}

/*====================================================================
    ������      ProcVcsMcuClearAllTvWallChannel
    ����        �����ĳ������ǽͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuClearAllTvWallChannel( void )
{
	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	if( m_cVCSConfStatus.GetTVWallManageMode() != VCS_TVWALLMANAGE_MANUAL_MODE )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "Cur TvWall manager mode is not manual mode.cant't clear channel\n",m_cVCSConfStatus.GetTVWallManageMode() );		
		return;
	}

	if( m_cVCSConfStatus.GetIsTvWallOperating() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "Now is operation tvWall.Nack\n" );	
		return;
	}

	//��������ǽ��αԭ�Ӳ���
	//Ҫ��������н�����������ܽ�����һ�˵Ĳ���
	m_cVCSConfStatus.SetIsTvWallOperating( TRUE );

	VCSClearAllTvWallChannel();

	m_cVCSConfStatus.SetIsTvWallOperating( FALSE );

}

/*====================================================================
    ������      ProcVcsMcuClearOneTvWallChannel
    ����        �����ĳ������ǽͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
	11/05/26                ����            ��������
====================================================================*/
void CMcuVcInst::ProcVcsMcuClearOneTvWallChannel( const CMessage *pcMsg )
{
	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	if( m_cVCSConfStatus.GetTVWallManageMode() != VCS_TVWALLMANAGE_MANUAL_MODE )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "Cur TvWall manager mode is not manual mode.cant't clear channel\n",m_cVCSConfStatus.GetTVWallManageMode() );		
		return;
	}

	u8 *pbyBuf = (u8*)cServMsg.GetMsgBody();
	u8 byEqpId = *pbyBuf++;
	u8 byChannelIdx = *pbyBuf;

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "eqpid(%d) eqpType(%d) channelIdx(%d)\n", byEqpId, g_cMcuVcApp.GetEqpType(byEqpId), byChannelIdx );

	if ( !IsValidTvw(byEqpId, byChannelIdx))
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "eqp type is wrong or channelIdx is too large \n " );
		return;
	}

	if(	!g_cMcuVcApp.IsPeriEqpConnected( byEqpId ) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "eqp is not connected\n " );
		return;
	}

	if( m_cVCSConfStatus.GetIsTvWallOperating() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "Now is operation tvWall.Nack\n" );		
		return;
	}

	// �����ҵ�
	CTvwChnnl cTvwChnnl;
	cTvwChnnl.Clear();
	if (!GetTvwChnnl(byEqpId, byChannelIdx, cTvwChnnl))
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "Get tvw chnnl(%d, %d) failed\n", byEqpId, byChannelIdx );		
		return;
	}

	// ͨ����ԱΪ��, ����Ҫ���
	if(cTvwChnnl.GetMember().IsNull())
	{
		return;
	}

	// ��ʽ��ʼ���
	m_cVCSConfStatus.SetIsTvWallOperating( TRUE );

	// ��ȡͨ����Ա
	TMt tOldMember = (TMt)cTvwChnnl.GetMember();

	// Hduͨ�����ʹ���
	if (IsValidHduChn(byEqpId, byChannelIdx))
	{
		// �Ѿ���HDU����ǽ�е��ն�ͣ����
		// [2013/03/11 chenbing] VCS���鲻֧��HDU�໭��,��ͨ����0
		ChangeHduSwitch( NULL, byEqpId, byChannelIdx, 0, TW_MEMBERTYPE_VCSSPEC, TW_STATE_STOP );				
	}
	// ��ͨ����ǽͨ�����ʹ���
	else
	{
		ChangeTvWallSwitch( &tOldMember, byEqpId, byChannelIdx, TW_MEMBERTYPE_VCSSPEC, TW_STATE_STOP );			
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "TvWall channel(%d) is has mt(%d.%d),has stopped switch to tvwall\n ",
		byChannelIdx, tOldMember.GetMcuId(), tOldMember.GetMtId());

	// �ն˱��������ǽʱ, Ҫ���ǹҶϹҶ���
	if (!(m_cVCSConfStatus.GetCurVCMT() == tOldMember ) 
		&& !m_ptMtTable->IsMtInHdu( GetLocalMtFromOtherMcuMt(tOldMember).GetMtId() ) 
		&& !m_ptMtTable->IsMtInTvWall( GetLocalMtFromOtherMcuMt(tOldMember).GetMtId()))
	{
		if (m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE)
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ClearOneTvWallChannel] cur mode is VCS_SINGLE_MODE VCSDropMT mt is in hdu(%d)\n",
				m_ptMtTable->IsMtInHdu( GetLocalMtFromOtherMcuMt(tOldMember).GetMtId() ) 
				);
			
			// �Ҷ��ն�
			VCSDropMT( tOldMember );
		}
		// �෽�໭�������Ҳ���ڻ���ϳ�����, Ҳ��Ҫ���Ҷ�
		else if ( m_cVCSConfStatus.GetCurVCMode() == VCS_MULVMP_MODE ) 
			
		{
			TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
			if (!tConfVmpParam.IsMtInMember(tOldMember))
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ClearTvw(%d,%d)] DropMT(%d,%d) in VcsMode.%d\n",
					byEqpId, byChannelIdx, tOldMember.GetMcuId(), tOldMember.GetMtId(),  m_cVCSConfStatus.GetCurVCMode());
				
				VCSDropMT( tOldMember );
			}
		}
	}

	// �������, ���µ�Tvwͨ��״̬�仯֪ͨ�����̨
	TPeriEqpStatus tStatus;
	if( g_cMcuVcApp.GetPeriEqpStatus( byEqpId,&tStatus )) 
	{
		cServMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
		SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
	}
	
	m_cVCSConfStatus.SetIsTvWallOperating( FALSE );
}

/*====================================================================
    ������      ProcVcsMcuChgTvWallMgrMode
    ����        ���ı����ǽ����ģʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuChgTvWallMgrMode( const CMessage *pcMsg )
{
	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	//action abort when no chairman online
	TMt tChairMan = m_tConf.GetChairman();
	if( ( tChairMan.IsNull()  || 
		!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
		m_byCreateBy != CONF_CREATE_MT
		)
	{
		cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
		cServMsg.SetEventId(cServMsg.GetEventId() + 2);
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}

	u8 byMode = *(u8*)cServMsg.GetMsgBody();

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "[ChgTvWallMgrMode from %d to %d\n", m_cVCSConfStatus.GetTVWallManageMode(), byMode );

	if( !( byMode >= VCS_TVWALLMANAGE_MANUAL_MODE &&
		byMode <= VCS_TVWALLMANAGE_REVIEW_MODE )
		)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuChgTvWallMgrMode] wrong tvwall manage mode(%d).Nack\n",byMode );
		//cServMsg.SetErrorCode( 16 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//ֻ������������²����Զ���Ԥ��ģʽ
	if( byMode != VCS_TVWALLMANAGE_MANUAL_MODE && 
		!ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() )
		)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuChgTvWallMgrMode]  byMode(%d) can't in VCS GroupConfMode(%d).Nack\n",
				byMode,m_cVCSConfStatus.GetCurVCMode() );
		//cServMsg.SetErrorCode( 16 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}


	if( byMode == m_cVCSConfStatus.GetTVWallManageMode() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuChgTvWallMgrMode] curmode is %d. needn't change.Nack\n",byMode );
		//cServMsg.SetErrorCode( 16 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	if( m_cVCSConfStatus.GetIsTvWallOperating() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuChgTvWallMgrMode] Now is operation tvWall.Nack\n" );
		cServMsg.SetErrorCode(ERR_MCU_VCS_PLANISOPERATING);
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		return;
	}

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	//��������ǽ��αԭ�Ӳ���
	//Ҫ��������н�����������ܽ�����һ�˵Ĳ���
	m_cVCSConfStatus.SetIsTvWallOperating( TRUE );

	m_cVCSConfStatus.SetTVWallManageMode( byMode );

	//��Ϊ�Զ�ģʽ�������ն˴ӵ�һ��ͨ����ʼ�������ǽ
	m_cVCSConfStatus.SetCurUseTWChanInd( 0 );

	//�ı�ģʽ������е���ǽͨ��
	VCSClearAllTvWallChannel();	

	VCSConfStatusNotif();

	m_cVCSConfStatus.SetIsTvWallOperating( FALSE );
}

/*====================================================================
    ������      ProcVcsMcuGetAllPlanData
    ����        ��֪ͨ�������е�����Ԥ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
====================================================================*/
void CMcuVcInst::ProcMcsVcsMcuGetAllPlanData( const CMessage *pcMsg )
{
	u16 wPrintModule = 0;
	BOOL32 bIsConsoleVCS = TRUE;

	if( MCS_CONF == m_tConf.GetConfSource())
	{
		wPrintModule = MID_MCU_MCS;
		bIsConsoleVCS = FALSE;
	}
	else
	{
		wPrintModule = MID_MCU_VCS;
	}

	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, wPrintModule, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	if(g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_SWITCH ||
		g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_IDLE )
	{
		ConfPrint(LOG_LVL_WARNING,MID_MCU_NPLUS,"N+1 1 Mode, not support plan!\n");
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		
		return;
	}

	if( !m_cCfgInfoRileExtraDataMgr.CheckPlanData() )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "[ProcVcsMcuGetAllPlanData] checkplandatafailed!\n " );
		//cServMsg.SetErrorCode( 16 );
		
		//MCS��ȡԤ��ʧ�ܣ�������ģ�崴��[11/22/2012 chendaiwei]
		if(!bIsConsoleVCS)
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}
		
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	if( !m_cCfgInfoRileExtraDataMgr.IsReadPlan() )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "[ProcVcsMcuGetAllPlanData] Plan Manager is not initialize.Nack\n " );
		//cServMsg.SetErrorCode( 16 );
		
		//MCS��ȡԤ��ʧ�ܣ�������ģ�崴��[11/22/2012 chendaiwei]
		if(!bIsConsoleVCS)
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}

		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	TMtVcsPlanNames tVcsPlanNames = {0};
	
	u8 idx = 0;
	s8 *pachPlanName[ VCS_MAXNUM_PLAN ];
	for( idx = 0;idx < VCS_MAXNUM_PLAN;idx++ )
	{
		pachPlanName[idx] = &tVcsPlanNames.m_achAlias[idx][0];
	}

	u8 byPlanNum = 0;
	m_cCfgInfoRileExtraDataMgr.GetAllPlanName( (s8**)pachPlanName,byPlanNum );

	if( 0 == byPlanNum )
	{	
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "byPlanNum is 0 ,no planData. can't notify data\n " );		
		return;
	}

	if( byPlanNum > VCS_MAXNUM_PLAN )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "byPlanNum(%d) is too big. can't notify data\n ",byPlanNum );		
		return;
	}

	CConfId tConfId = m_tConf.GetConfId();
	
	for( idx = 0;idx < byPlanNum;idx++ )
	{
		NotifyOnePlanDataToConsole( pachPlanName[idx],MCU_VCS_ALLPLANDATA_SINGLE_NOTIFY,bIsConsoleVCS );
	}
}

/*====================================================================
    ������      ProcVcsMcuModifyPlanName
    ����        ���޸�Ԥ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
====================================================================*/
void CMcuVcInst::ProcMcsVcsMcuModifyPlanName( const CMessage *pcMsg )
{
	u16 wPrintModule = 0;
	if(VCS_CONF == m_tConf.GetConfSource())
	{
		wPrintModule = MID_MCU_VCS;
	}
	else
	{
		wPrintModule = MID_MCU_MCS;
	}

	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, wPrintModule, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);	

	if( m_cVCSConfStatus.GetIsPlanDataOperation() )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "Plan is operating.Nack\n " );		
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANISMODIFYING ); //TBD ����ErrorCode?  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	m_cVCSConfStatus.SetIsPlanDataOperation( TRUE );

	u8* pbyBuf = cServMsg.GetMsgBody();
	if(pbyBuf == NULL)
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,"Modify Plan Buff is NULL\n ");
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	u8 byOldPlanNameLen = *pbyBuf;
	pbyBuf++;
	s8* pbyOldPlanName = new s8[byOldPlanNameLen+1];
	if( NULL == pbyOldPlanName )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "alloc memery pbyOldPlanName error!!!\n" ); 
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	memcpy( pbyOldPlanName,pbyBuf,byOldPlanNameLen );
	pbyOldPlanName[byOldPlanNameLen] = '\0';

	if( 0 == strcmp( "",pbyOldPlanName ) ) 
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "OldPlanName is empty!!!.\n " );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		return;
	}

	pbyBuf += byOldPlanNameLen;
	u8 byNewPlanNameLen = *pbyBuf;
	pbyBuf++;
	s8* pbyNewlanName = new s8[byNewPlanNameLen + 1];
	if( NULL == pbyNewlanName )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "alloc memery pbyNewlanName error!!!\n" ); 
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	memcpy( pbyNewlanName,pbyBuf,byNewPlanNameLen );
	pbyNewlanName[ byNewPlanNameLen ] = '\0';
	

	if( strlen( pbyOldPlanName ) != byOldPlanNameLen ||
		strlen( pbyNewlanName ) != byNewPlanNameLen )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "OldPlanName(%s) Len(%d) is not recLen(%d) or NewPlanName(%s) Len(%d) is not recLen(%d).Nack\n ",
			pbyOldPlanName,strlen( pbyOldPlanName ),byOldPlanNameLen,
			pbyNewlanName,strlen( pbyNewlanName ),byNewPlanNameLen
			);

		//cServMsg.SetErrorCode( 15 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}


	if( byOldPlanNameLen > (VCS_MAXLEN_ALIAS - 1) || 0 == byOldPlanNameLen)
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "OldPlanName(%s) Len is than 31 or is 0.Nack\n ",pbyOldPlanName );

		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNAMELENNOTVALID );//TBD ����ErrorCode?  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}

	if( byNewPlanNameLen > (VCS_MAXLEN_ALIAS - 1) || 0 == byNewPlanNameLen)
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "NewPlanName(%s) Len is than 31 or is 0.Nack\n ",pbyNewlanName );

		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNAMELENNOTVALID );//TBD ����ErrorCode?  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}

	if( !m_cCfgInfoRileExtraDataMgr.IsHasPlanName(pbyOldPlanName) )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "OldPlanName(%s) is not Exist.Nack\n ",pbyOldPlanName );
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNAMENOTEXIST );//TBD ����ErrorCode?  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}

	if( m_cCfgInfoRileExtraDataMgr.IsHasPlanName(pbyNewlanName) )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "NewPlanName(%s) is Exist.Nack\n ",pbyNewlanName );
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNAMEALREADYEXIST );//TBD ����ErrorCode?  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}

	if( !m_cCfgInfoRileExtraDataMgr.CheckPlanData() )
	{
		//MCS��ȡԤ��ʧ�ܣ�������ģ�崴��[11/22/2012 chendaiwei]
		if(MCS_CONF == m_tConf.GetConfSource())
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "[Modify]CheckPlanData failed!\n " );
		//cServMsg.SetErrorCode( 12 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}


	if( !m_cCfgInfoRileExtraDataMgr.IsReadPlan() )
	{
		//MCS��ȡԤ��ʧ�ܣ�������ģ�崴��[11/22/2012 chendaiwei]
		if(MCS_CONF == m_tConf.GetConfSource())
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "Plan Manager is not initialize.Nack\n " );
		//cServMsg.SetErrorCode( 12 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}

	TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
	if( m_cCfgInfoRileExtraDataMgr.ModifyPlanName( pbyOldPlanName,pbyNewlanName, m_byConfIdx,tConfAttrbEx.IsDefaultConf() ) )
	{
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	}
	else
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "Modify PlanName(%s) to PlanName occor error!!!.Nack\n ",
							pbyOldPlanName,pbyNewlanName );
		//cServMsg.SetErrorCode( 12 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}
	delete []pbyOldPlanName;
	delete []pbyNewlanName;

	//VCSNotifyAllPlanName();

	cServMsg.SetEventId( MCU_VCS_MODIFYPLANNAME_NOTIFY );
	SendMsgToAllMcs( MCU_VCS_MODIFYPLANNAME_NOTIFY,cServMsg );

	m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );

}

/*====================================================================
    ������      ProcMcsVcsMcuDelPlanName
    ����        ��ɾ��Ԥ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
	12/02/22				�´�ΰ			�޸�
====================================================================*/
void CMcuVcInst::ProcMcsVcsMcuDelPlanName( const CMessage *pcMsg )
{
	u16 wPrintModule = 0;
	BOOL32 bNotifyVCS = TRUE;

	if( MCS_CONF == m_tConf.GetConfSource())
	{
		wPrintModule = MID_MCU_MCS;
		bNotifyVCS = FALSE;
	}
	else
	{
		wPrintModule = MID_MCU_VCS;
	}

	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, wPrintModule, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	if( m_cVCSConfStatus.GetIsPlanDataOperation() )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "Plan is operating.Nack\n " );		
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANISMODIFYING );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	m_cVCSConfStatus.SetIsPlanDataOperation( TRUE );

	s8 *pbyPlanName = (s8*)cServMsg.GetMsgBody();

	u8 byStrLen = strlen( pbyPlanName );

	if( byStrLen > (VCS_MAXLEN_ALIAS - 1) || 0 == byStrLen)
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "planName(%s) Len is than 31 or is 0.Nack\n ",pbyPlanName );
		
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}

	if( !m_cCfgInfoRileExtraDataMgr.IsHasPlanName(pbyPlanName) )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "planName(%s) is not Exist.Nack\n ",pbyPlanName );
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNAMENOTEXIST ); //TBD �������Ƿ���Ҫ����  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}

	if( !m_cCfgInfoRileExtraDataMgr.CheckPlanData() )
	{
		//MCS��ȡԤ��ʧ�ܣ�������ģ�崴��[11/22/2012 chendaiwei]
		if(MCS_CONF == m_tConf.GetConfSource())
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "[Del]CheckPlanData failed\n " );
		//cServMsg.SetErrorCode( 12 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}

	if( !m_cCfgInfoRileExtraDataMgr.IsReadPlan() )
	{
		//MCS��ȡԤ��ʧ�ܣ�������ģ�崴��[11/22/2012 chendaiwei]
		if(MCS_CONF == m_tConf.GetConfSource())
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "Plan Manager is not initialize.Nack\n " );
		//cServMsg.SetErrorCode( 12 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}

	TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
	if( m_cCfgInfoRileExtraDataMgr.DelPlanName( pbyPlanName, m_byConfIdx,tConfAttrbEx.IsDefaultConf() ) )
	{
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	}
	else
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "DelPlanName(%s) occor error!!!.Nack\n ",pbyPlanName );
		//cServMsg.SetErrorCode( 12 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}

	NotifyAllPlanNameToConsole(bNotifyVCS);

	m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );

}

/*====================================================================
    ������      ProcMcsVcsMcuAddPlanName
    ����        �����Ԥ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
	12/02/22				chendaiwei      modify
====================================================================*/
void CMcuVcInst::ProcMcsVcsMcuAddPlanName( const CMessage *pcMsg )
{
	u16 wModule = 0;
	BOOL32 bIsConsoleVCS = TRUE; //��ʶ����̨��MCS����VCS  [2/22/2012 chendaiwei]

	if( VCS_CONF == m_tConf.GetConfSource() )
	{
		 wModule = MID_MCU_VCS;
	}
	else
	{
		 wModule = MID_MCU_MCS;
		 bIsConsoleVCS = FALSE;
	}

	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, wModule, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	if( m_cVCSConfStatus.GetIsPlanDataOperation() )
	{
		ConfPrint(LOG_LVL_WARNING, wModule,  "Plan is operating.Nack\n " );		
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANISMODIFYING ); //TBD �Ƿ���Ҫ����ErrorCode  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	m_cVCSConfStatus.SetIsPlanDataOperation( TRUE );


	s8 *pbyBuf = (s8*)cServMsg.GetMsgBody();

	if( NULL == pbyBuf )
	{
		ConfPrint(LOG_LVL_WARNING, wModule,  "Add Plan Buff is NULL\n ");
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}
	
	u8 byStrLen = *pbyBuf;
	

	if( byStrLen > (VCS_MAXLEN_ALIAS - 1) || 0 == byStrLen)
	{
		ConfPrint(LOG_LVL_WARNING, wModule,  "planName Len(%d) is than 31 or is 0.Nack\n ",byStrLen );		
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNAMELENNOTVALID ); //TBD �Ƿ���Ҫ����ErrorCode  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}

	pbyBuf++;
	s8 *pbyPlanName = new s8[ byStrLen + 1 ];
	if( NULL == pbyPlanName )
	{
		ConfPrint(LOG_LVL_WARNING, wModule,  "Alloc memory pbyPlanName is error.\n " );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}
	memcpy( pbyPlanName,pbyBuf,byStrLen );
	pbyPlanName[byStrLen] = '\0';	


	if( 0 == strcmp( "",pbyPlanName ) ) 
	{
		ConfPrint(LOG_LVL_WARNING, wModule,  "pbyPlanName is empty!!!.\n " );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyPlanName;
		return;
	}
	
	if( byStrLen != strlen(pbyPlanName) ) 
	{
		ConfPrint(LOG_LVL_WARNING, wModule,  "pbyPlanName len is not expected,error!!!.<expectd.%d,acutal.%d>\n ",byStrLen,strlen(pbyPlanName) );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyPlanName;
		return;
	}

	pbyBuf += byStrLen;
	u8 byMtAliasNum = *pbyBuf;

	BOOL32 bIsAddPlanName = TRUE;//( 0 == byMtAliasNum  );

	ConfPrint(LOG_LVL_DETAIL, wModule,  "[ProcVcsMcuAddPlanName] planName(%s) Len(%d) MtALiasNum(%d)\n ",
		pbyPlanName,byStrLen,byMtAliasNum );

	if(!m_cCfgInfoRileExtraDataMgr.CheckPlanData())
	{
		//MCS��ȡԤ��ʧ�ܣ�������ģ�崴��[11/22/2012 chendaiwei]
		if(!bIsConsoleVCS)
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}
		ConfPrint(LOG_LVL_WARNING, wModule,  "[Add]checkPlandata failed!\n " );
		delete []pbyPlanName;
		//cServMsg.SetErrorCode( 11 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}
	
	if( !m_cCfgInfoRileExtraDataMgr.IsReadPlan() )
	{
		//MCS��ȡԤ��ʧ�ܣ�������ģ�崴��[11/22/2012 chendaiwei]
		if(!bIsConsoleVCS)
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}
		ConfPrint(LOG_LVL_WARNING, wModule,  "Plan Manager is not initialize.Nack\n " );
		delete []pbyPlanName;
		//cServMsg.SetErrorCode( 11 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}

	if( m_cCfgInfoRileExtraDataMgr.IsHasPlanName(pbyPlanName) )
	{
		/*ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "planName(%s) is Exist.Nack\n ",pbyPlanName );
		delete []pbyPlanName;
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNAMEALREADYEXIST );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;*/
		ConfPrint(LOG_LVL_KEYSTATUS, wModule,  "planName(%s) is Exist. so Save PlanData\n ",pbyPlanName );
		bIsAddPlanName = FALSE;
	}
	
	if( bIsAddPlanName )
	{	
		if( (bIsConsoleVCS && m_cCfgInfoRileExtraDataMgr.GetMtPlanNum() >= VCS_MAXNUM_PLAN )
			|| (!bIsConsoleVCS && m_cCfgInfoRileExtraDataMgr.GetMtPlanNum() >= MCS_MAXNUM_PLAN) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, wModule,  "Plan Number(%d) is equal or rather than MAXNUM_PLAN(%d).Nack\n ",
				m_cCfgInfoRileExtraDataMgr.GetMtPlanNum(),bIsConsoleVCS?VCS_MAXNUM_PLAN:MCS_MAXNUM_PLAN );
			delete []pbyPlanName;
			cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNUMOVERFLOW ); //TBD �Ƿ���Ҫ����ErrorCode  [2/22/2012 chendaiwei]
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
			return;
		}
		
		TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
		if( m_cCfgInfoRileExtraDataMgr.AddPlanName( pbyPlanName, m_byConfIdx,tConfAttrbEx.IsDefaultConf()) )
		{
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}
		else
		{
			ConfPrint(LOG_LVL_KEYSTATUS, wModule,  "AddPlanName(%s) occor error!!!.Nack\n ",pbyPlanName );
			delete []pbyPlanName;
			//cServMsg.SetErrorCode( 11 );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
			return;
		}

		NotifyAllPlanNameToConsole(bIsConsoleVCS);
	}
	else//����Ԥ��
	{
		if( !m_cCfgInfoRileExtraDataMgr.IsHasPlanName(pbyPlanName) )
		{
			ConfPrint(LOG_LVL_WARNING, wModule,  "planName(%s) is not Exist.Nack\n ",pbyPlanName );
			delete []pbyPlanName;
			//cServMsg.SetErrorCode( 11 );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
			return;
		}

		pbyBuf++;
		TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
		if( m_cCfgInfoRileExtraDataMgr.SaveMtPlanAliasByPlanName( pbyPlanName,
													(TMtVCSPlanAlias*)pbyBuf,
													byMtAliasNum, 
													m_byConfIdx,
													tConfAttrbEx.IsDefaultConf())
													)
		{
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, wModule,  "SaveMtPlanAliasByPlanName is occor error.Nack\n " );
			delete []pbyPlanName;
			//cServMsg.SetErrorCode( 16 );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	
			m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
			return;
		}
		
		NotifyOnePlanDataToConsole( pbyPlanName,MCU_VCS_ONEPLANDATA_NOTIFY,bIsConsoleVCS );
	}

	if(pbyPlanName)
	{
		delete []pbyPlanName;
		pbyPlanName = NULL;
	}

	m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
	

}

/*====================================================================
    ������      NotifyAllPlanNameToConsole
    ����        ��֪ͨ�����������е�Ԥ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[IN]BOOL32 bNotifyVCSΪTRUE֪ͨVCS��ΪFALSE֪ͨMCS
				
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
	12/02/22				�´�ΰ          �޸�
====================================================================*/
void CMcuVcInst::NotifyAllPlanNameToConsole( BOOL32 bNotifyVCS )
{
	u16 wPrintModule = 0;

	if(bNotifyVCS)
	{
		wPrintModule = MID_MCU_VCS;
	}
	else
	{
		wPrintModule = MID_MCU_MCS;
	}

	CServMsg cMsg;
	TMtVcsPlanNames tVcsPlanNames = {0};
	
	s8 *pachPlanName[ VCS_MAXNUM_PLAN ];
	u8 idx = 0;
	for( idx = 0;idx < VCS_MAXNUM_PLAN;idx++ )
	{
		pachPlanName[idx] = &tVcsPlanNames.m_achAlias[idx][0];
	}
	
	u8 byPlanNum = 0;
	m_cCfgInfoRileExtraDataMgr.GetAllPlanName( (s8**)pachPlanName,byPlanNum );
	tVcsPlanNames.m_byPlanNum = byPlanNum;
	
	for( idx = 0;idx < byPlanNum;idx++)
	{
		ConfPrint(LOG_LVL_DETAIL, wPrintModule,  "PlanName%d:(%s)\n",idx+1,&tVcsPlanNames.m_achAlias[idx][0] );
	}

	cMsg.SetMsgBody( (u8*)&tVcsPlanNames,sizeof( tVcsPlanNames ) );

	SendMsgToAllMcs( MCU_VCS_ALLPLANNAME_NOTIFY,cMsg );

}

/*====================================================================
    ������		  NotifyOnePlanDataToConsole
    ����        ��֪ͨ����ĳ��Ԥ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
				  [IN]BOOL32 bNotifyVCS ΪTRUE֪ͨVCS��ΪFALSE֪ͨMCS
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
	12/02/22				�´�ΰ			�޸�
====================================================================*/
void CMcuVcInst::NotifyOnePlanDataToConsole( s8* pbyPlanName,u16 wEvent,BOOL32 bNotifVCS )
{
	/*lint -save -esym(429, pbyPlanName)*/

	u16 wPrintModule = 0;
	if(bNotifVCS)
	{
		wPrintModule = MID_MCU_VCS;
	}
	else
	{
		wPrintModule = MID_MCU_MCS;
	}

	if( !m_cCfgInfoRileExtraDataMgr.IsHasPlanName( pbyPlanName ) )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "pbyPlanName(%s) is not exists,can't notify!!!\n",pbyPlanName );
		return;
	}

	u8 byMtPlanAliasNum = m_cCfgInfoRileExtraDataMgr.GetMtPlanAliasNumByPlanName( pbyPlanName );

	
	ConfPrint(LOG_LVL_DETAIL, wPrintModule,  "byMtPlanAliasNum is %d.!!!\n",byMtPlanAliasNum );
	
 
	TMtVCSPlanAlias *pMtPlanAlias = NULL;
	if( byMtPlanAliasNum > 0 )
	{
		pMtPlanAlias= new TMtVCSPlanAlias[ byMtPlanAliasNum ];

		if( NULL == pMtPlanAlias )
		{
			ConfPrint(LOG_LVL_WARNING, wPrintModule,  "apply memory error,can't notify!!!\n" );
			return;
		}

		if( !m_cCfgInfoRileExtraDataMgr.GetMtPlanAliasByPlanName( pbyPlanName,pMtPlanAlias,byMtPlanAliasNum ) )
		{
			ConfPrint(LOG_LVL_WARNING, wPrintModule,  "GetMtPlanAliasByPlanName occor error,can't notify!!!\n" );
			delete []pMtPlanAlias;
			return;
		}
	}
	

	CServMsg cServMsg;	
	//CConfId tConfId = m_tConf.GetConfId();
	//cServMsg.SetMsgBody( (u8*)&tConfId,sizeof( tConfId ) );
	u8 byStrLen = strlen( pbyPlanName );
	cServMsg.SetMsgBody( &byStrLen,sizeof(byStrLen) );
	cServMsg.CatMsgBody( (u8*)pbyPlanName,byStrLen );
	cServMsg.CatMsgBody( &byMtPlanAliasNum,sizeof( byMtPlanAliasNum) );
	if( byMtPlanAliasNum > 0 )
	{
		cServMsg.CatMsgBody( (u8*)pMtPlanAlias,sizeof( TMtVCSPlanAlias ) * byMtPlanAliasNum );
	}	

	SendMsgToAllMcs(wEvent, cServMsg);

	if( NULL != pMtPlanAlias )
	{
		delete []pMtPlanAlias;
	}	

	/*lint -restore*/
}

/*====================================================================
    ������      VCSClearMtDisconnectReason
    ����        ������ն˵�����ԭ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/05/06                �ܾ���          ����
====================================================================*/
void CMcuVcInst::VCSClearMtDisconnectReason( TMt tMt )
{
	//ֻ�����ص��ն˺��¼�mcu
	if( !tMt.IsLocal() )
	{
		return;
	}


	TMtStatus tMtStatus;
	if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{	
		if( MT_TYPE_MT == m_ptMtTable->GetMtType( tMt.GetMtId() ) )
		{
			if( m_ptMtTable->GetMtStatus(tMt.GetMtId(),&tMtStatus) )
			{
				tMtStatus.SetMtDisconnectReason( MTLEFT_REASON_NORMAL );
				m_ptMtTable->SetMtStatus( tMt.GetMtId(),&tMtStatus );
			}
			MtStatusChange( &tMt,TRUE );
		}
		else if( MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) )
		{
			TConfMcInfo *ptMcInfo = NULL;
			TConfMtInfo *ptConfMtInfo = NULL;
			u16 wMcuIdx = GetMcuIdxFromMcuId( tMt.GetMtId() );
			ptMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );
			if( NULL == ptMcInfo )
			{
				return;
			}
			ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr( wMcuIdx );//tMt.GetMtId() ); 
			if( NULL == ptConfMtInfo )
			{
				return;
			}
			u8 byMtId = 0;
			while( byMtId < MAXNUM_CONF_MT )
			{				
				if( ptConfMtInfo->MtInConf( byMtId ) )
				{
					ptMcInfo->m_atMtStatus[byMtId].SetMtDisconnectReason( MTLEFT_REASON_NORMAL );
					MtStatusChange( (TMt*)&(ptMcInfo->m_atMtStatus[byMtId]));
				}											
				++byMtId;
			}	
				
		}
	}
	else
	{
		if( m_ptMtTable->GetMtStatus(tMt.GetMtId(),&tMtStatus) )
		{
			tMtStatus.SetMtDisconnectReason( MTLEFT_REASON_NORMAL );
			m_ptMtTable->SetMtStatus( tMt.GetMtId(),&tMtStatus );
		}
		MtStatusChange( &tMt,TRUE );
	}
}

/*====================================================================
    ������      VCSClearTvWallChannelByMt
    ����        �����ĳ������ǽ�е�ĳ���ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
    11/05/27                ����            �ع�
====================================================================*/
void CMcuVcInst::VCSClearTvWallChannelByMt( TMt tMt, BOOL32 bOnlyClearTvWallStatus /*= FALSE*/ )
{
	// һ������֧�ֵĵ���ǽͨ��������
#define MAX_TVW_CHNNL_NUM  (u8)(MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL + MAXNUM_PERIEQP_CHNNL * MAXNUM_MAP)

	CConfTvwChnnl acTvwChnnlFind[MAX_TVW_CHNNL_NUM];					// ��Ѱ�ҵĵ���ǽͨ��
	u8		abyTvwIdFind[MAX_TVW_CHNNL_NUM];							// �洢�ҵ��ĵ���ǽ�豸��ID
	memset(abyTvwIdFind, 0, sizeof(abyTvwIdFind));
	
	const u8 byFindNum = FindAllTvwChnnl(tMt, acTvwChnnlFind, MAX_TVW_CHNNL_NUM);

	u8 byTvwEqpId, byTvwChnnlIdx;								// �ҵ��ĵ���ǽ�豸�ź�ͨ����
	TPeriEqpStatus tStatus;

	for (u8 byLoop = 0; byLoop < byFindNum; ++byLoop)
	{
		if (acTvwChnnlFind[byLoop].IsNull())
		{
			continue;
		}

		byTvwEqpId = acTvwChnnlFind[byLoop].GetEqpId();
		byTvwChnnlIdx = acTvwChnnlFind[byLoop].GetChnnlIdx();

		if (!g_cMcuVcApp.IsPeriEqpConnected(byTvwEqpId))
		{
			continue;
		}

		// ��Hdu�豸
		if (IsValidHduChn(byTvwEqpId, byTvwChnnlIdx))
		{
			// [2013/03/11 chenbing] VCS���鲻֧��HDU�໭��,��ͨ����0
			// Bug00154723��vcs��������ʱ����һ���ڱ�Ļ����е��ն˽�ǽ��ȡ��ǿ���ն�δ��ǽ���ǽ�����ʾ�ն˱���
			ChangeHduSwitch( NULL,
				byTvwEqpId,
				byTvwChnnlIdx,
				0/*chenbing*/, 
				acTvwChnnlFind[byLoop].GetMember().byMemberType,
				TW_STATE_STOP,
				MODE_BOTH, FALSE, FALSE);
		}
		// ����ͨ����ǽ�豸
		else
		{
			// ͣ����
			ChangeTvWallSwitch(&tMt,
				byTvwEqpId,
				byTvwChnnlIdx,
				acTvwChnnlFind[byLoop].GetMember().byMemberType,
				TW_STATE_STOP);
		}
		
		// [11/11/2011 liuxu] ����ǽ��ѯʱ, ��Ӧ����ȫ���ͨ��
		// ����Ӧ�÷ŵ�ChangeHduSwitch�ﴦ���, ���Ǳ������, �����ﴦ��
		if ( TW_MEMBERTYPE_TWPOLL == acTvwChnnlFind[byLoop].GetMember().byMemberType )
		{
			if( g_cMcuVcApp.GetPeriEqpStatus( byTvwEqpId, &tStatus ) )
			{
				if (IsValidHduChn(byTvwEqpId, byTvwChnnlIdx))
				{
					tStatus.m_tStatus.tHdu.atVideoMt[byTvwChnnlIdx].byMemberType = acTvwChnnlFind[byLoop].GetMember().byMemberType;
					tStatus.m_tStatus.tHdu.atVideoMt[byTvwChnnlIdx].SetConfIdx(m_byConfIdx);
				}
				else
				{
					tStatus.m_tStatus.tTvWall.atVideoMt[byTvwChnnlIdx].byMemberType = acTvwChnnlFind[byLoop].GetMember().byMemberType;
					tStatus.m_tStatus.tTvWall.atVideoMt[byTvwChnnlIdx].SetConfIdx(m_byConfIdx);
				}
				
				g_cMcuVcApp.SetPeriEqpStatus( byTvwEqpId, &tStatus);
				
				// ���Ͽ�����һ����ѯ
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byTvwEqpId, byTvwChnnlIdx, dwTimerIdx) )
				{
					KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
					CMessage cMsg;
					memset(&cMsg, 0, sizeof(cMsg));
					cMsg.event = u16(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
					cMsg.content = (u8*)&dwTimerIdx;
					cMsg.length  = sizeof(u32);
					ProcTWPollingChangeTimerMsg(&cMsg);
				}
			}
		}

		// ���ν�Hdu Id����abyTvwIdFind��
		for ( u8 byCounter = 0; byCounter < MAX_TVW_CHNNL_NUM; ++byCounter)
		{
			if ( 0 == abyTvwIdFind[byCounter] )
			{
				abyTvwIdFind[byCounter] = byTvwEqpId;
				break;
			}

			if ( byTvwEqpId == abyTvwIdFind[byCounter] )
			{
				break;
			}
		}
	}
	
	if( !(m_cVCSConfStatus.GetCurVCMT() == tMt) 
		&& m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE 
		&& !m_ptMtTable->IsMtInTvWall( GetLocalMtFromOtherMcuMt(tMt).GetMtId() ) 
		&& !m_ptMtTable->IsMtInHdu( GetLocalMtFromOtherMcuMt(tMt).GetMtId() ))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[VCSClearTvWallChannelByMt] cur mode is VCS_SINGLE_MODE VCSDropMT is mt in hdu(%d) mtconfindex(%d) conidx(%d) curVCMT(%d.%d)\n",
			m_ptMtTable->IsMtInTvWall( GetLocalMtFromOtherMcuMt(tMt).GetMtId() ) ,
			tMt.GetConfIdx(),m_byConfIdx,
			m_cVCSConfStatus.GetCurVCMT().GetMcuId(),
			m_cVCSConfStatus.GetCurVCMT().GetMtId()
			);
		
		VCSDropMT( tMt );
	}

	if( !bOnlyClearTvWallStatus &&
		m_cVCSConfStatus.GetMtInTvWallCanMixing() && 
		m_tConf.m_tStatus.IsMixing() && m_tConf.m_tStatus.IsSpecMixing() &&
		VCSMtNotInTvWallCanStopMixing(tMt) )
	{
		RemoveSpecMixMember( &tMt, 1, FALSE, TRUE );
	}
	
	// �ϱ�Tvwall�豸ͨ��״̬�仯
	u8 byNext = 0;
	CServMsg cServMsg;
	while ( byNext < MAX_TVW_CHNNL_NUM && 0 != abyTvwIdFind[byNext] )
	{
		u8 byTvwId = abyTvwIdFind[byNext];
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  
			"[VCSClearTvWallChannelByMt] notify HDU eqp(id:%u) status\n", 
			byTvwId );

		if( g_cMcuVcApp.GetPeriEqpStatus( byTvwId, &tStatus ) )
		{
			cServMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
			SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
		}

		++byNext;
	}
}

/*====================================================================
    ������      VCSClearAllTvWallChannel
    ����        ��������е���ǽͨ�������������ǽ��ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
    11/05/27                ����            �ع�
====================================================================*/
void CMcuVcInst::VCSClearAllTvWallChannel( const TSwitchInfo *ptSwitchInfo,const u8 byNum )
{
	// һ������֧�ֵĵ���ǽͨ��������
	const u8 byMaxTvwChnnlNum = MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL + MAXNUM_PERIEQP_CHNNL * MAXNUM_MAP;
	CConfTvwChnnl acTvwChnnlFind[byMaxTvwChnnlNum];		// ��Ѱ�ҵĵ���ǽͨ��
	u8 byEqpId, byChnnlIdx;							// ����ǽͨ����Ӧ���豸�ź����豸�е�ͨ����
	
	// �����ж��Ƿ�Ҫ���ͨ��. ���Ҫ��յ�ͨ���е��ն�ǡ��������Ҫ������ն�, ���ͨ���������
	BOOL32 bCanStopSwitch = TRUE;	
	TMt tDropMt;									// ���˳�����ǽͨ�����ն�
	const TSwitchInfo* ptTvWallSwitchInfo = NULL;	// ���ڶ�α���ptSwitchInfo

	// ��ȡ���е���ǽͨ��
	const u8 byTvwChnnlNum = GetAllCfgedTvwChnnl( acTvwChnnlFind, byMaxTvwChnnlNum );

	// ���α�����ȡ�õ���ÿ������ǽͨ�������������
	for ( u8 byLoop = 0; byLoop < byTvwChnnlNum; ++byLoop )
	{
		// ���׵��ж��Ƿ�Ϊ��
		if (acTvwChnnlFind[byLoop].IsNull())
		{
			continue;
		}

		byEqpId = acTvwChnnlFind[byLoop].GetEqpId();
		byChnnlIdx = acTvwChnnlFind[byLoop].GetChnnlIdx();

		// [12/28/2011 liuxu] ������, ҲҪ���в���, �Ա������ǽ����ʱ�����ն���ǽ��,
		// ����ǽ�������ߺ����ǽģʽ�����ı�ʱ, �ְ��ն˺���ǽ��

		// �жϳ�Ա�����Ƿ���vcs�����е�����, �Լ��Ƿ��Ǳ��˻���ռ��
		if( !IsVcsTvwMemberType(acTvwChnnlFind[byLoop].GetMember().byMemberType) 
			&& acTvwChnnlFind[byLoop].GetMember().GetConfIdx() != m_byConfIdx )
		{
			continue;
		}

		// [6/8/2011 liuxu] ����õ���ǽͨ������ѯ, ����ֹͣ��ѯ
		u8 byPollState = POLL_STATE_NONE;
		if( m_tTWMutiPollParam.GetPollState(byEqpId, byChnnlIdx, byPollState) && POLL_STATE_NONE != byPollState)
		{
			// [10/27/2011 liuxu] �������ѯ, ��Ҫͣ��ѯ
			m_tTWMutiPollParam.SetPollState(byEqpId, byChnnlIdx, POLL_STATE_NONE);	

			//��ȡ��ʱ������
			u32 dwTimerIdx = 0;
			if(!m_tTWMutiPollParam.GetTimerIdx(byEqpId, byChnnlIdx, dwTimerIdx))
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[ProcMcsMcuHduPollMsg] Cann't get tvw<EqpId:%d, ChnId:%d> TimerIdx!\n", byEqpId, byChnnlIdx);
				continue;
			}

			// ɱ����ʱ��
			KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);	
		}
		
		// ����bCanStopSwitch
		bCanStopSwitch = TRUE;

		// ���ݴ�������ն���Ϣȷ���Ƿ�Ҫ������մ�ͨ��
		ptTvWallSwitchInfo = ptSwitchInfo;
		if( NULL != ptTvWallSwitchInfo 
			&& byNum > 0 
			&& acTvwChnnlFind[byLoop].GetMember().IsLocal())
		{
			for( u8 byIdx = 0; byIdx < byNum; byIdx++, ptTvWallSwitchInfo++ )
			{							
				if( ptTvWallSwitchInfo->GetSrcMt() == (TMt)acTvwChnnlFind[byLoop].GetMember()
					&& byEqpId == ptTvWallSwitchInfo->GetDstMt().GetEqpId() 
					&& ptTvWallSwitchInfo->GetDstChlIdx() == byChnnlIdx )
				{
					// ���Ҫ��յ�ͨ���е��ն�ǡ��������Ҫ������ն�, ���ͨ���������
					bCanStopSwitch = FALSE;
					break;
				}
			}
		}

		// ��Ҫ����ļ������
		if( bCanStopSwitch )
		{	
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[VCSClearAllTvWallChannel] dorp mt(%d,%d)\n",
				acTvwChnnlFind[byLoop].GetMember().GetMcuId(),
				acTvwChnnlFind[byLoop].GetMember().GetMtId() );
			
			tDropMt = (TMt)acTvwChnnlFind[byLoop].GetMember();

			// hdu����ǽ����
			if (IsValidHduChn(byEqpId, byChnnlIdx))
			{
				// [2013/03/11 chenbing] VCS���鲻֧��HDU�໭��,��ͨ����0
				ChangeHduSwitch(NULL,
					byEqpId,
					byChnnlIdx,
					0, 
					acTvwChnnlFind[byLoop].GetMember().byMemberType, 
					TW_STATE_STOP,
					MODE_BOTH,
					FALSE,
					m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE ? TRUE:FALSE
				);
			}
			// ��ͨ����ǽ����
			else 
			{
				ChangeTvWallSwitch(&tDropMt,
					byEqpId,
					byChnnlIdx,
					acTvwChnnlFind[byLoop].GetMember().byMemberType,
					TW_STATE_STOP);
			}
			
			// �ն˱��������ǽʱ, Ҫ���ǹҶϹҶ���
			if (!(m_cVCSConfStatus.GetCurVCMT() == tDropMt ) 
				&& !m_ptMtTable->IsMtInHdu( GetLocalMtFromOtherMcuMt(tDropMt).GetMtId() ) 
				&& !m_ptMtTable->IsMtInTvWall( GetLocalMtFromOtherMcuMt(tDropMt).GetMtId()))
			{
				// ������������Ҫ�Ҷ�
				if (m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ClearOneTvWallChannel] cur mode is VCS_SINGLE_MODE VCSDropMT mt is in hdu(%d)\n",
						m_ptMtTable->IsMtInHdu( GetLocalMtFromOtherMcuMt(tDropMt).GetMtId() ) 
						);
					
					// �Ҷ��ն�
					VCSDropMT( tDropMt );
					
				}
				// �෽�໭�������Ҳ���ڻ���ϳ�����, Ҳ��Ҫ���Ҷ�
				else if ( m_cVCSConfStatus.GetCurVCMode() == VCS_MULVMP_MODE )
					
				{
					TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
					if (!tConfVmpParam.IsMtInMember(tDropMt))
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ClearTvw(%d,%d)] DropMT(%d,%d) in VcsMode.%d\n",
							byEqpId, byChnnlIdx, tDropMt.GetMcuId(), tDropMt.GetMtId(),  m_cVCSConfStatus.GetCurVCMode());
						
						VCSDropMT( tDropMt );
					}
				}
			}
		}
	}
}

/*====================================================================
    ������      VCSChangeTvWallMtMixStatus
    ����        ��������رյ���ǽ����״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
    11/05/27                ����            �ع�
====================================================================*/
void CMcuVcInst::VCSChangeTvWallMtMixStatus( BOOL32 bIsStartMixing )
{
	TEqp tHduEqp;
	tHduEqp.SetNull();	

	if( bIsStartMixing &&
		m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE )
	{	
		if( !m_tConf.m_tStatus.IsMixing() )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[VCSChangeTvWallMtMixStatus] StartMixing(mcuPartMix)!\n" );
			
			if (!StartMixing(mcuPartMix))
			{
				CServMsg cServMsg;
				cServMsg.SetConfIdx(m_byConfIdx);
				cServMsg.SetConfId(m_tConf.GetConfId());

				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[VCSChangeTvWallMtMixStatus] Find no mixer\n");
				
				if (ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_NOMIXSPEAKER);
				}
				else
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_NOMIXER);
				}
				m_cVCSConfStatus.SetMtInTvWallCanMixing( FALSE );
				SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cServMsg);
				return;
			}
			else
			{
				TMt tMt = m_tConf.GetChairman();
				VCSConfStopMTSel( tMt,MODE_AUDIO );
				m_tConf.m_tStatus.SetSpecMixing();
				ChangeSpecMixMember(&tMt, 1);
				
				if( !m_cVCSConfStatus.GetCurVCMT().IsNull() )
				{
					tMt = m_cVCSConfStatus.GetCurVCMT();
					VCSConfStopMTSel( tMt,MODE_AUDIO );						
					AddSpecMixMember( &tMt,1,TRUE );
				}
			}
		}
		else
		{
			TMt tMt = GetLocalMtFromOtherMcuMt( m_tConf.GetChairman() );
			VCSConfStopMTSel( tMt,MODE_AUDIO );
			if( !m_ptMtTable->IsMtInMixing( tMt.GetMtId() ) )
			{
				AddSpecMixMember( &tMt,1,TRUE );
			}
			
			if( !m_cVCSConfStatus.GetCurVCMT().IsNull() )
			{
				tMt = m_cVCSConfStatus.GetCurVCMT();
				VCSConfStopMTSel( tMt,MODE_AUDIO );
				AddSpecMixMember( &tMt,1,TRUE );
			}
		}
	}
	
	// 5/27/2011 liuxu] ��������, �����˵���ǽ��tvwall�����ദ��
	// һ������֧�ֵĵ���ǽͨ��������
	const u8 byMaxTvwChnnlNum = MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL + MAXNUM_PERIEQP_CHNNL * MAXNUM_MAP;
	CConfTvwChnnl acTvwChnnlFind[byMaxTvwChnnlNum];		// ��Ѱ�ҵĵ���ǽͨ��
	
	TTvwMember tTvwMember;							// ����ǽͨ����Ա
	u8 byMixMemNum = 0;								// ��������ĳ�Ա����
	CServMsg cMixMemServMsg;						
	TMt tLocalMt;
	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	// ��ȡ���е���ǽͨ��
	const u8 byTvwChnnlNum = GetAllCfgedTvwChnnl( acTvwChnnlFind, byMaxTvwChnnlNum );
	for ( u8 byLoop = 0; byLoop < byTvwChnnlNum; ++byLoop )
	{
		tTvwMember = acTvwChnnlFind[byLoop].GetMember();
		if( !IsVcsTvwMemberType( tTvwMember.byMemberType ) 
			|| ( tTvwMember.GetConfIdx() && tTvwMember.GetConfIdx() != m_byConfIdx) )
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "The chnnl Member of Conf Hdu Chnnl.%d is invalid\n", byLoop);
			continue;
		}

		TMt tMt = (TMt)tTvwMember;
		if( FALSE == bIsStartMixing )
		{	
			if( VCSMtInTvWallCanStopMixing(tMt) )
			{
				RemoveSpecMixMember( &tMt, 1, FALSE, TRUE );
			}
			tLocalMt = GetLocalMtFromOtherMcuMt(tMt);
			//�������ش��½�ǽ�ն�����ǵ����ն�ͬʱҲ���ڻ���ϳ��У����¼�MCU�߳��������������¼�δ�������ʱ���������¼��ϴ�ͨ��������
			if (!tMt.IsLocal() && !IsLocalAndSMcuSupMultSpy(tMt.GetMcuIdx()) && 
				m_ptMtTable->IsMtInMixing(tLocalMt.GetMtId()) &&
				GetLocalMtFromOtherMcuMt(m_cVCSConfStatus.GetCurVCMT()).GetMtId() != tLocalMt.GetMtId() && 
				!tConfVmpParam.IsMtInMember(tMt))
			{
				RemoveSpecMixMember( &tLocalMt, 1, FALSE, TRUE );
			}
			//��������ģʽ�£�ȡ��һ���������ڵ���ǽ���ն˾�����ϯ
			/*if( !m_tConf.m_tStatus.IsMixing() &&
				m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE )
			{
				GoOnSelStep( tMt, MODE_AUDIO, TRUE );
			}*/
		}
		else if( bIsStartMixing )
		{
			// [12/1/2011 liuxu] ����ն�ֻ����ʾ��ǽ��, ��û������������, �򲻲������
			if ( 0 == g_cMcuVcApp.GetChnnlMMode(acTvwChnnlFind[byLoop].GetEqpId(), acTvwChnnlFind[byLoop].GetChnnlIdx()) 
				|| !m_tConfAllMtInfo.MtJoinedConf(tMt))
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "The chnnl(%d,%d)'s MODE Is 0\n",
				 acTvwChnnlFind[byLoop].GetEqpId(),acTvwChnnlFind[byLoop].GetChnnlIdx());
				continue;
			}

			if (byMixMemNum == 0)
			{
				cMixMemServMsg.SetMsgBody((u8*)&tMt,sizeof(TMt));
			}
			else
			{
				cMixMemServMsg.CatMsgBody((u8*)&tMt,sizeof(TMt));
			}
			
			byMixMemNum ++;
		}
	}

	if (bIsStartMixing)
	{
		TMt *ptMt = (TMt*)(cMixMemServMsg.GetMsgBody());
		u8 bymixnum = cMixMemServMsg.GetMsgBodyLen() / sizeof(TMt);
		AddSpecMixMember(ptMt, byMixMemNum, FALSE);
	}

	if( !bIsStartMixing && m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE )
	{
		if( m_tConf.m_tStatus.IsMixing() )
		{
			//����������ʱ�����⣬����Ҫ�ȵ���������ֹͣ��Ӧ����ִ���������ֹͣ���еĵ���ǽ�ն˵Ļ���
			StopMixing();
			return;
		}
		//��ϯѡ����ǰ�ն�
		if( !m_cVCSConfStatus.GetCurVCMT().IsNull() )
		{
			GoOnSelStep( m_cVCSConfStatus.GetCurVCMT(),MODE_AUDIO,FALSE );
			GoOnSelStep( m_cVCSConfStatus.GetCurVCMT(),MODE_AUDIO,TRUE );
		}
	}
}

/*====================================================================
    ������      VCSCanMtNotInTvWallStopMixing
    ����        ��������ǽͨ�ն��˳�����ǽʱ�Ƿ�����˳�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
====================================================================*/
BOOL32 CMcuVcInst::VCSMtNotInTvWallCanStopMixing( TMt tMt )
{
	if( tMt.IsNull() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt IsNull.Can't stop mixing \n" );
		return FALSE;
	}	

	if( m_tConf.GetChairman() == tMt )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt is chairman .Can't stop mixing \n" );
		return FALSE;
	}

	if( m_cVCSConfStatus.GetCurVCMT() == tMt )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt Is CurVcmt.Can't stop mixing \n" );
		return FALSE;
	}	
	
	if( tMt.IsLocal() &&
		!m_ptMtTable->IsMtInMixing( tMt.GetMtId() ) ) 
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt Is not in mixing.Can't stop mixing \n" );
		return FALSE;
	}	

	TMt tLocalMt = GetLocalMtFromOtherMcuMt(tMt);

	if( tMt.IsLocal() &&
		(m_ptMtTable->IsMtInTvWall( tMt.GetMtId() ) || m_ptMtTable->IsMtInHdu( tMt.GetMtId() ))
		)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt Is in tvwall(%d) or in hdu(%d).Can't stop mixing \n",
			m_ptMtTable->IsMtInTvWall( tMt.GetMtId() ),
			m_ptMtTable->IsMtInHdu( tMt.GetMtId() ));
		return FALSE;
	}

	if( !tMt.IsLocal() && VCS_TVWALLMANAGE_REVIEW_MODE != m_cVCSConfStatus.GetTVWallManageMode() )
	{
		TPeriEqpStatus tStatus;
		u8 byEqpId = HDUID_MIN;
		while ( byEqpId <= HDUID_MAX  )
		{					
			memset( &tStatus,0,sizeof(tStatus) );
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
			if (1 == tStatus.m_byOnline)
			{			
				u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byEqpId));
				if (0 == byHduChnNum)
				{
					byEqpId++;
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "GetHduChnNumAcd2Eqp failed!\n");
					continue;
				}

				for(u8 byLoop = 0; byLoop < byHduChnNum; byLoop++)
				{
					if( (TMt)tStatus.m_tStatus.tHdu.atVideoMt[byLoop] == tMt )
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt Is in hdu(%d) chnl(%d).Can't stop mixing \n",
							byEqpId,byLoop );
						return FALSE;
					}					
				}
			}		
			byEqpId++;
		}

		byEqpId=TVWALLID_MIN;
		while (  byEqpId < TVWALLID_MAX )
		{					
			memset( &tStatus,0,sizeof(tStatus) );
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
			if (1 == tStatus.m_byOnline)
			{					
				u8 byMemberNum = tStatus.m_tStatus.tTvWall.byChnnlNum;
				for(u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
				{
					if ( (TMt)tStatus.m_tStatus.tTvWall.atVideoMt[byLoop] == tMt )
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt Is in tvwall(%d) chnl(%d).Can't stop mixing \n",
							byEqpId,byLoop );
						return FALSE;
					}
				}
			}
			byEqpId++;
		}			
	}

	if( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE ||
		m_cVCSConfStatus.GetCurVCMode() == VCS_MULVMP_MODE )
	{
		TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
		u8 byVmpChnlIdx = tVmpParam.GetChlOfMtInMember( tMt );		
		if( byVmpChnlIdx > 0 && byVmpChnlIdx < MAXNUM_VMP_MEMBER )
		{
			if(  tVmpParam.IsMtInMember( tMt ) )
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt Is vmp channel(%d).Can't stop mixing \n", byVmpChnlIdx);
				return FALSE;
			}
			else
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "param tMt Is vmp channel(%d).but not in mtmember \n", byVmpChnlIdx);
			}			
		}
	}

	return TRUE;
}
/*====================================================================
    ������      IsHasMtInHduOrTwByMcuIdx
    ����        ������ǽͨ�����Ƿ����ĳ��MCU�µ��ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    12/12/14                ��־��          ����
====================================================================*/
BOOL32 CMcuVcInst::IsHasMtInHduOrTwByMcuIdx(u16 wSmcuIdx)
{
	if( wSmcuIdx == INVALID_MCUIDX )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[IsHasMtInHduOrTwByMcuIdx]wSmcuIdx == INVALID_MCUIDX,So Return!\n" );
		return FALSE;
	}	
	u8 byMtId = GetFstMcuIdFromMcuIdx(wSmcuIdx);
	TPeriEqpStatus tStatus;
	u8 byEqpId = HDUID_MIN;
	while ( byEqpId <= HDUID_MAX  )
	{					
		memset( &tStatus,0,sizeof(tStatus) );
		g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
		if (1 == tStatus.m_byOnline)
		{			
			u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byEqpId));
			if (0 == byHduChnNum)
			{
				byEqpId++;
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "GetHduChnNumAcd2Eqp failed!\n");
				continue;
			}

			for(u8 byLoop = 0; byLoop < byHduChnNum; byLoop++)
			{
				if( GetLocalMtFromOtherMcuMt((TMt)tStatus.m_tStatus.tHdu.atVideoMt[byLoop]).GetMtId() == byMtId )
				{
					if ( 0 == g_cMcuVcApp.GetChnnlMMode(byEqpId, byLoop) )
					{
						continue;
					}
					else
					{
						return TRUE;
					}
				}					
			}
		}		
		byEqpId++;
	}

	byEqpId=TVWALLID_MIN;
	while (  byEqpId < TVWALLID_MAX )
	{					
		memset( &tStatus,0,sizeof(tStatus) );
		g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
		if (1 == tStatus.m_byOnline)
		{					
			u8 byMemberNum = tStatus.m_tStatus.tTvWall.byChnnlNum;
			for(u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
			{
				if ( GetLocalMtFromOtherMcuMt((TMt)tStatus.m_tStatus.tTvWall.atVideoMt[byLoop]).GetMtId() == byMtId )
				{
					if ( 0 == g_cMcuVcApp.GetChnnlMMode(byEqpId, byLoop) )
					{
						continue;
					}
					else
					{
						return TRUE;
					}
				}
			}
		}
		byEqpId++;
	}	
	return FALSE;
}
/*====================================================================
    ������      VCSMtInTvWallCanStopMixing
    ����        ���ڵ���ǽ�е��ն��Ƿ����ֹͣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
====================================================================*/
BOOL32 CMcuVcInst::VCSMtInTvWallCanStopMixing( TMt tMt )
{
	if( tMt.IsNull() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "param tMt IsNull.Can't stop mixing \n" );
		return FALSE;
	}	
	//
	if( tMt.IsLocal() &&
		!m_ptMtTable->IsMtInMixing( tMt.GetMtId() ) ) 
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "param tMt Is not in mixing.Can't stop mixing \n" );
		return FALSE;
	}

	if( m_tConf.GetChairman() == tMt )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "param tMt Is ChairMan.Can't stop mixing \n" );
		return FALSE;
	}

	if( m_cVCSConfStatus.GetCurVCMT() == tMt )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "param tMt Is CurVcmt.Can't stop mixing \n" );
		return FALSE;
	}

	TMtStatus tStatus;
	if ( !GetMtStatus(tMt, tStatus) 
		|| !tStatus.IsInMixing() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "tmt(%d,%d) is not in mixing\n" ,tMt.GetMcuIdx(),tMt.GetMtId());
		return FALSE;
	}			

	TMt tLocalMt = GetLocalMtFromOtherMcuMt(tMt);

	if( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE ||
		m_cVCSConfStatus.GetCurVCMode() == VCS_MULVMP_MODE)
	{
		TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
		u8 byVmpChnlIdx = tVmpParam.GetChlOfMtInMember( tMt );		
		if( byVmpChnlIdx > 0 && byVmpChnlIdx < MAXNUM_VMP_MEMBER )
		{
			if( tVmpParam.IsMtInMember( tMt ) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "param tMt Is in vmp channel(%d).Can't stop mixing \n", byVmpChnlIdx);
				return FALSE;
			}
			else
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[VCSMtInTvWallCanStopMixing] param tMt Is in vmp channel(%d). but not in mtmember\n", byVmpChnlIdx);
			}
		}
	}

	return TRUE;
}

/*====================================================================
    ������      SetSingleMtInTW
    ����        ���ֶ�ģʽ�е��ն˽�����ǽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����	TSwitchInfo ptSwitchInfo	[i]
					const CMessage &cServMsg	[i]
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    11/4/27                 xueliang        ����
    11/05/27                ����            �ع�
====================================================================*/
void CMcuVcInst::SetSingleMtInTW(TSwitchInfo *ptSwitchInfo, CServMsg &cServMsg)
{
	if ( NULL == ptSwitchInfo )
	{
		return;
	}

	TMt tOrgSrcMt = ptSwitchInfo->GetSrcMt();
	TMt tDstMt = ptSwitchInfo->GetDstMt();
	u8 byDstChannelIdx = ptSwitchInfo->GetDstChlIdx();
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] tSrcMt(%d.%d) EqpType(%d) EqpId(%d) byDstChlIdx(%d)\n",
		tOrgSrcMt.GetMcuId(),tOrgSrcMt.GetMtId(),tDstMt.GetEqpType(),
		tDstMt.GetEqpId(),byDstChannelIdx );
	

	//���¼��е�ǰ�����նˣ���ͬһ���¼��������ն��������ǽ����NACK
	if( !tOrgSrcMt.IsLocal() 
		&& !IsLocalAndSMcuSupMultSpy(tOrgSrcMt.GetMcuId()) 
		&& IsMtInMcu(GetLocalMtFromOtherMcuMt(tOrgSrcMt), m_cVCSConfStatus.GetCurVCMT()) 
		&& !( m_cVCSConfStatus.GetCurVCMT() == tOrgSrcMt ))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] CurVCMT(%d.%d) and Mt(%d.%d) is in same smcu,Mt can't in tvwall \n ",
			m_cVCSConfStatus.GetCurVCMT().GetMcuId(),
			m_cVCSConfStatus.GetCurVCMT().GetMtId(),
			tOrgSrcMt.GetMcuId(),
			tOrgSrcMt.GetMtId() );
		
		cServMsg.SetErrorCode( ERR_MCU_VCS_NOUSABLEBACKCHNNL );
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		
		return;
	}		
		
	/*u8 byVmpChnlIdx = m_tConf.m_tStatus.GetVmpParam().GetChlOfMtInMember( tOrgSrcMt );
	//�������ϳ�ģʽ�£����¼��Ѿ����ն˽��뻭��ϳɣ���ô���¼������ն˲��ܽ�����ǽ
	TVMPParam tvmpParam = m_tConf.m_tStatus.GetVmpParam();
	if( !tOrgSrcMt.IsLocal() 
		&& !IsLocalAndSMcuSupMultSpy(tOrgSrcMt.GetMcuId()) 
		&& m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE 
		&& (byVmpChnlIdx > 0 && byVmpChnlIdx < MAXNUM_MPUSVMP_MEMBER ) 
		&& !( tOrgSrcMt == *tvmpParam.GetVmpMember( byVmpChnlIdx ) )
		)
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] other mt is in vmp chl(%d) and it is same smcu to tOrgSrcMt(%d.%d) ,Mt can't in tvwall \n ",
			m_tConf.m_tStatus.GetVmpParam().GetChlOfMtInMember( tOrgSrcMt ),
			tOrgSrcMt.GetMcuId(),
			tOrgSrcMt.GetMtId()						
			);
		
		cServMsg.SetErrorCode( ERR_MCU_VCS_NOUSABLEBACKCHNNL );
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		return;
	}*/
	if (!tOrgSrcMt.IsLocal() && m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE &&
		!IsLocalAndSMcuSupMultSpy(tOrgSrcMt.GetMcuIdx()))
	{
		TVMPParam_25Mem tvmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
		TVMPMember *ptVMPMember;
		for (u8 byVmpChnIdx = 0 ; byVmpChnIdx < tvmpParam.GetMaxMemberNum() ; byVmpChnIdx++)
		{
			ptVMPMember = tvmpParam.GetVmpMember(byVmpChnIdx);
			if (ptVMPMember == NULL || ptVMPMember->IsNull() 
				|| ptVMPMember->IsLocal() || *ptVMPMember == tOrgSrcMt)
			{
				continue;
			}
			if (GetLocalMtFromOtherMcuMt(*ptVMPMember) == GetLocalMtFromOtherMcuMt(tOrgSrcMt))
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] other mt is in vmp chl(%d) and it is same smcu to ptVMPMember(%d.%d) ,Mt can't in tvwall \n ",
					byVmpChnIdx,ptVMPMember->GetMcuId(),ptVMPMember->GetMtId());
				cServMsg.SetErrorCode( ERR_MCU_VCS_NOUSABLEBACKCHNNL );
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
		}
	}
		
	// ����ǽ�Ƿ�, Nack
	if ( !IsValidTvw(tDstMt.GetEqpId(), byDstChannelIdx)
		|| !g_cMcuVcApp.IsPeriEqpConnected( tDstMt.GetEqpId() ))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] Wrong Peri Type (%d) or is has not registered \n ",
			tDstMt.GetEqpType() );
		
		cServMsg.SetErrorCode(ERR_MCU_VCS_NOUSABLETW);
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
	
		return;
	}
		
	TMt tSrcMt = GetLocalMtFromOtherMcuMt( tOrgSrcMt );
	TMt tDropMt;
	tDropMt.SetNull();
	TLogicalChannel tChannel;
	
	//ͬһ���¼�mcu���ն�ֻ����һ���������ǽͨ��������Ҫ��ǰһ���޳�������ǽ
	if( !tOrgSrcMt.IsLocal() 
		&& !IsLocalAndSMcuSupMultSpy(tOrgSrcMt.GetMcuId()) 
		&& m_tConfAllMtInfo.MtJoinedConf( tSrcMt.GetMtId() ) )
	{		
		FindSmcuMtInTvWallAndStop( tOrgSrcMt, &tDropMt );				
		OnMMcuSetIn(tOrgSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
	}
	
	// [5/27/2011 liuxu] ��Tvwall��Hdu����ͳһ����
	CTvwChnnl cDstTvwChnnl;
	if (!GetTvwChnnl( tDstMt.GetEqpId(), byDstChannelIdx, cDstTvwChnnl ))
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_VCS, "[SetSingleMtInTW]Get Tvwall Chnnl(%d, %d) falied\n", tDstMt.GetEqpId(), byDstChannelIdx);
		cServMsg.SetErrorCode(ERR_MCU_VCS_NOUSABLETW);
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		return;
	}

	// �ȰѾɵ�ͨ����Ա�߳���ͨ��
	if( !cDstTvwChnnl.GetMember().IsNull() )
	{
		if( cDstTvwChnnl.GetMember().GetMcuId() == tOrgSrcMt.GetMcuId() &&
			cDstTvwChnnl.GetMember().GetMtId() == tOrgSrcMt.GetMtId())
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] Mt is already in TvWall\n" );
			return;		
		}
		
		TMt tOldMt = cDstTvwChnnl.GetMember();
		
		//ֹͣ�ɳ�Ա������ǽ�Ľ���
		if (IsValidHduChn(tDstMt.GetEqpId(), byDstChannelIdx))
		{
			//�Ѿ��ڵ���ǽ�е��ն�ͣ����
			// [2013/03/11 chenbing] VCS���鲻֧��HDU�໭��,��ͨ����0
			ChangeHduSwitch( NULL, tDstMt.GetEqpId(), byDstChannelIdx, 0, TW_MEMBERTYPE_VCSSPEC,
							TW_STATE_STOP, MODE_BOTH, FALSE, FALSE );
		}
		else
		{
			ChangeTvWallSwitch( &tOldMt, tDstMt.GetEqpId(), byDstChannelIdx, TW_MEMBERTYPE_VCSSPEC, TW_STATE_STOP );
		}
			
		//�ǵ�ǰ�����ն��˳�����ǽ�����Ѿ���һ���������ܣ����˳����ƻ���
		if( m_cVCSConfStatus.GetMtInTvWallCanMixing() && 
			m_tConf.m_tStatus.IsMixing() && m_tConf.m_tStatus.IsSpecMixing() &&
			VCSMtNotInTvWallCanStopMixing(	tOldMt )			
			)
		{
			RemoveSpecMixMember( &tOldMt, 1, FALSE );
		}
		
		// [2013/04/08 chenbing] ��ӻ�����жϣ�����������ն˲�������йҶϲ���
		// �޸�Bug00134330��tOldMtΪ���������ն˲���MtIdΪ1ʱ��VCS�е�ǰ��ϯMtIdΪ1��
		// ������߼����Ҷϵ�ǰ��ϯ
		if( !(m_cVCSConfStatus.GetCurVCMT() == tOldMt) &&
			m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE &&
			!m_ptMtTable->IsMtInTvWall( GetLocalMtFromOtherMcuMt(tOldMt).GetMtId() ) &&
			!m_ptMtTable->IsMtInHdu( GetLocalMtFromOtherMcuMt(tOldMt).GetMtId() ) &&
			(tOldMt.GetConfIdx() == m_byConfIdx))//�������ͬ������ִ�йҶϲ���
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS,  "[SetSingleMtInTW] VCSDropMt Chairman.confId:%d ChairmanMcuId: %d ChairmanMtId: %d \n",
				m_tConf.GetChairman().GetConfIdx(), m_tConf.GetChairman().GetMcuId(), m_tConf.GetChairman().GetMtId() );
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS,  "[SetSingleMtInTW] VCSDropMt tOldMt.confId:%d McuId: %d MtId: %d \n",
				tOldMt.GetConfIdx(), tOldMt.GetMcuId(), tOldMt.GetMtId( ));
			VCSDropMT( tOldMt );
		}
	}

	// ���ն��Ƿ�����,�ն������߲������߼�ͨ��δ�����绰�ն�֧��
	BOOL byOnline = m_tConfAllMtInfo.MtJoinedConf( tOrgSrcMt.GetMcuId(), tOrgSrcMt.GetMtId() );
	if( byOnline )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] mt is online,start switch to TVWall \n " );

		//BOOL32 bStartTvwOk = TRUE;
		if (IsValidHduChn( tDstMt.GetEqpId(), byDstChannelIdx))
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] mt is online,start switch to HDU \n " );
			// [2013/03/11 chenbing] VCS���鲻֧��HDU�໭��,��ͨ����0
			ChangeHduSwitch( &tOrgSrcMt, tDstMt.GetEqpId(), byDstChannelIdx, 0, TW_MEMBERTYPE_VCSSPEC, TW_STATE_START );
		}
		else
		{
			//������
			ChangeTvWallSwitch( &tOrgSrcMt,
				tDstMt.GetEqpId(),
				byDstChannelIdx,
				TW_MEMBERTYPE_VCSSPEC,
				TW_STATE_START );

		}
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] mt is offline,save it in TVWall \n " );
		
		//δ�����ն��������߻�򿪺���ͨ���Ժ󽨵�����ǽ�Ľ���
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( tDstMt.GetEqpId(),&tStatus );

		// [2013/04/08 chenbing] ��ǰ����ǽͨ������������ʹ�����ܽ�ǽ
		// �޸�Bug00134330��Vcs�����ն˽�������������ռ�õĵ���ǽͨ��
		if ( m_byConfIdx == tStatus.m_tStatus.tTvWall.atVideoMt[byDstChannelIdx].GetConfIdx()
			 || 0 == tStatus.m_tStatus.tTvWall.atVideoMt[byDstChannelIdx].GetConfIdx())
		{
			tStatus.m_tStatus.tTvWall.atVideoMt[byDstChannelIdx].SetMt( tOrgSrcMt );
			tStatus.m_tStatus.tTvWall.atVideoMt[byDstChannelIdx].byMemberType = TW_MEMBERTYPE_VCSSPEC;
			g_cMcuVcApp.SetPeriEqpStatus( tDstMt.GetEqpId(),&tStatus );
		}

		cServMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
		SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
		
		//Bug00156555��VCS��������һ�������������ն˽�hdu��hdu2���ն˻�һֱͣ���ڵ���ǽͨ����
		//yrl20131108 �ô����VCS_SINGLE_MODE֧�֣���ΪMCUVC_VCMTOVERTIMER_TIMER����ʱm_cVCSConfStatus.SetReqVCMT()Ϊ�գ�
		//�����ն˲����������ǽͨ��
		if( m_cVCSConfStatus.GetCurVCMode() == VCS_MULVMP_MODE
			|| m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE)
		{
			m_cVCSConfStatus.SetReqVCMT( tOrgSrcMt );
		}
		
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] m_byConfIdx:%d HduChnMtConfId:%d tOrgSrcMt.McuId:%d tOrgSrcMt.MtId:%d\n ",
			m_byConfIdx, tStatus.m_tStatus.tTvWall.atVideoMt[byDstChannelIdx].GetConfIdx(), tOrgSrcMt.GetMcuId(), tOrgSrcMt.GetMtId());

		VCSCallMT( cServMsg,tOrgSrcMt,VCS_FORCECALL_REQ );
	}	
}

/*====================================================================
    ������      ProcVcsMcuMtInTvWallInManuMode
    ����        ���ֶ�ģʽ�е��ն˽�����ǽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    11/4/27                 xueliang           ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuMtInTvWallInManuMode(CServMsg& cServMsg)
{
	m_cVCSConfStatus.SetIsTvWallOperating( TRUE );

	u8 *pbyBuf = cServMsg.GetMsgBody();
	u8 bySwitchInfoNum = *pbyBuf;
	if( 0 == bySwitchInfoNum )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ProcVcsMcuMtInTvWallInManuMode] bySwitchInfoNum is 0.Nack\n " );
		
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		m_cVCSConfStatus.SetIsTvWallOperating( FALSE );
		return;
	}

	//ack
	SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

	pbyBuf ++;
	for( u8 byIdx = 0; byIdx<bySwitchInfoNum; byIdx++ )
	{
		CServMsg cOutMsg(cServMsg.GetServMsg() ,SERV_MSGHEAD_LEN);
		TSwitchInfo *ptSwitchInfo = (TSwitchInfo*)(pbyBuf + byIdx * sizeof( TSwitchInfo ));
		SetSingleMtInTW(ptSwitchInfo, cOutMsg);
	}

	m_cVCSConfStatus.SetIsTvWallOperating( FALSE );
	return;

}

/*====================================================================
    ������      ProcVcsMcuMtInTvWallInReviewMode
    ����        ��Ԥ��ģʽ�е��ն˽�����ǽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuMtInTvWallInReviewMode(CServMsg& cServMsg)
{
	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsgSend;

	//Ԥ��ģʽ�²�������ǽ��αԭ�Ӳ���
	//��ΪԤ��ģʽ�������Ľ�����Ϣ�϶࣬Ҫ��������н�����������ܽ�����һ�˵Ĳ���
	m_cVCSConfStatus.SetIsTvWallOperating( TRUE );

	u8 *pbyBuf = cServMsg.GetMsgBody();
	// [6/2/2011 liuxu] Ҫ�ж�ָ��
	if ( !pbyBuf )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Damaged msg\n", CurState());
		return;
	}

	u8 bySwitchInfoNum = *pbyBuf;

	if( 0 == bySwitchInfoNum )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "bySwitchInfoNum is 0.Nack\n " );
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		m_cVCSConfStatus.SetIsTvWallOperating( FALSE );
		return;
	}

	if( m_cVCSConfStatus.GetTVWallManageMode() != VCS_TVWALLMANAGE_REVIEW_MODE )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "cur TvWallManagerMode is not VCS_TVWALLMANAGE_REVIEW_MODE\n" );
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		m_cVCSConfStatus.SetIsTvWallOperating( FALSE );
		return;
	}

    SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

	pbyBuf++;
	TSwitchInfo *ptSwitchInfo = NULL; 

	TMt tOrgSrcMt;// = tSwitchInfo.GetSrcMt();
	TMt tDstMt;// = tSwitchInfo.GetDstMt();

	u8 byDstChannelIdx = 0;//tSwitchInfo.GetDstChlIdx();

	TTWSwitchInfo tSwitchInfo;

	BOOL32 bCanSwitchToTW = TRUE;

	TMt tDropMt;

	TPeriEqpStatus tStatus;
	
	u8 byIdx = 0;

	//Ԥ��ģʽ���ն˽������ǽ����������е���ǽͨ��(������ϼ����ն˾Ϳ���ԭ��ͨ���Ƿ�һ������ͬһ���ն˾Ͳ�������¼�һ�����)
	VCSClearAllTvWallChannel( (TSwitchInfo*)pbyBuf,bySwitchInfoNum );

	CTvwChnnl cTvwChnnl;						// ����һ��Tvwͨ��
	
	TVMPParam tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);// m_tConf.m_tStatus.GetVmpParam();
	//�ڶ����Ѵ���������Ϣ��Դ�ն˽�������ǽ�Ľ���
	//����Ǳ������ն˾���������������������¼��ľ��Ȳ���ͬһ���¼��������ն��Ƿ��н�����ǽ��
	//���û�оͽ�����������оͱ����Ա��Ϣ�ϱ����档
	for( byIdx = 0; byIdx < bySwitchInfoNum; byIdx++ , pbyBuf += sizeof( TSwitchInfo ))
	{
		ptSwitchInfo = (TSwitchInfo*)pbyBuf;
		if (!ptSwitchInfo)
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Invalid pointer\n", CurState());
			return;
		}

		tOrgSrcMt = ptSwitchInfo->GetSrcMt();		
		tDstMt = ptSwitchInfo->GetDstMt();
		byDstChannelIdx = ptSwitchInfo->GetDstChlIdx();

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  
			"DstEqpType(%d) DstEqpId(%d) SrcMt(%d.%d) eqpConnected(%d) dstChlIdx(%d)\n",
			tDstMt.GetEqpType(), tDstMt.GetEqpId(), tOrgSrcMt.GetMcuId(), tOrgSrcMt.GetMtId(),
			g_cMcuVcApp.IsPeriEqpConnected( tDstMt.GetEqpId() ),byDstChannelIdx );

		// [5/30/2011 liuxu] ��tvwall�����жϽ��з�װ
		if (!IsValidTvwEqp(tDstMt) || !IsValidTvw(tDstMt.GetEqpId(), byDstChannelIdx))
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "Invalid Tvw Eqp(%d, %d)\n" , tDstMt.GetEqpId(), byDstChannelIdx);
			continue;
		}

		if( !g_cMcuVcApp.IsPeriEqpConnected( tDstMt.GetEqpId() ) )
		{
			continue;
		}		

		tSwitchInfo.SetSrcMt(tOrgSrcMt);
		tSwitchInfo.SetDstMt(tDstMt);
		tSwitchInfo.SetDstChlIdx(byDstChannelIdx);
		tSwitchInfo.SetMemberType(TW_MEMBERTYPE_VCSSPEC);
		tSwitchInfo.SetMode(MODE_VIDEO);
		
		g_cMcuVcApp.GetPeriEqpStatus( tDstMt.GetEqpId(), &tStatus );
		bCanSwitchToTW = TRUE;
		tDropMt.SetNull();

		if( !m_tConfAllMtInfo.MtJoinedConf( tOrgSrcMt.GetMcuId(),tOrgSrcMt.GetMtId() ) )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "tOrgSrcMt(%d,%d) is not JoinedConf.So don't switch to TW\n",
								tOrgSrcMt.GetMcuId(),tOrgSrcMt.GetMtId() );

			bCanSwitchToTW = FALSE;
		}

		if( bCanSwitchToTW 
			&& !tOrgSrcMt.IsLocal() 
			&& !IsLocalAndSMcuSupMultSpy(tOrgSrcMt.GetMcuId()) 
			&& !(tOrgSrcMt == m_cVCSConfStatus.GetCurVCMT()) 
		   )
		{
			if( ( m_cVCSConfStatus.GetCurVCMT().GetMcuId() == tOrgSrcMt.GetMcuId() &&
							m_cVCSConfStatus.GetCurVCMT().GetMtId() != tOrgSrcMt.GetMtId() ) 
					|| 
				GetFstMcuIdFromMcuIdx(tOrgSrcMt.GetMcuId()) == GetFstMcuIdFromMcuIdx(m_cVCSConfStatus.GetCurVCMT().GetMcuId())
				)
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "Smcu(%d) has CurVcmt(%d.%d) is not SrcMt.So don't switch to TW\n",
					m_cVCSConfStatus.GetCurVCMT().GetMcuId(),m_cVCSConfStatus.GetCurVCMT().GetMcuId(),
					m_cVCSConfStatus.GetCurVCMT().GetMtId(),tOrgSrcMt.GetMcuId(),tOrgSrcMt.GetMtId()
					);

				bCanSwitchToTW = FALSE;
			}


			// 12/30/2010 liuxu][�߶�]�߼�������VCSLOG�����������byVmpChnlIdxȡ������tOrgSrcMt��
			u8 byVmpChnlIdx = tConfVmpParam.GetChlOfMtInMember( tOrgSrcMt );

			if( bCanSwitchToTW 
				&& m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE 
				&& ( byVmpChnlIdx > 0 && byVmpChnlIdx < MAXNUM_VMP_MEMBER ) 
				&& !( tOrgSrcMt == *tConfVmpParam.GetVmpMember( byVmpChnlIdx ) )
			  )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "now mode is GROUPVMP_MODE and Smcu(%d) has a mt in vmp channel(%d) and the mt is not srcMt(%d.%d).So don't switch to TW\n",
					tOrgSrcMt.GetMcuId(),byVmpChnlIdx,
					tOrgSrcMt.GetMcuId(),tOrgSrcMt.GetMtId()
					);

				bCanSwitchToTW = FALSE;
			}

			//����ǵ�һ��������Ϣ�����ǽ����������Ǻͽ���Լ���õ�
			//����ж��ͬһ���¼����նˣ�Ҫ�������ķ�����ǰ�棬���ں���ı����Ա��Ϣ����������
			if( byIdx != 0 
				&& bCanSwitchToTW 
				&& ( m_cVCSConfStatus.GetCurVCMode() != VCS_GROUPVMP_MODE 
					// [12/30/2010 liuxu][�߶�] ��������||����
				     || ( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE && !( byVmpChnlIdx > 0 && byVmpChnlIdx < MAXNUM_VMP_MEMBER ) ) 
					 || ( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE && ( byVmpChnlIdx > 0 && byVmpChnlIdx < MAXNUM_VMP_MEMBER ) 
						  && !( tOrgSrcMt == *tConfVmpParam.GetVmpMember( byVmpChnlIdx ) ) )
					)
				)
			{				
				tDropMt.SetNull();
				FindSmcuMtInTvWallAndStop( tOrgSrcMt, &tDropMt, FALSE);
				if(  !tDropMt.IsNull() )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "Smcu(%d) has a mt(%d.%d) in tvwall and the mt is not srcMt(%d.%d).So don't switch to TW\n",
							tOrgSrcMt.GetMcuId(),tDropMt.GetMcuId(),tDropMt.GetMtId(),
							tOrgSrcMt.GetMcuId(),tOrgSrcMt.GetMtId()
							);
					bCanSwitchToTW = FALSE;
				}								
			}
		}
		
		// [5/31/2011 liuxu] ��Tvwall��Hdu�Ĵ�������˺ϲ�
		// ��ȡ����ǽͨ��, ��ȡʧ��continue
		if (!GetTvwChnnl(tDstMt.GetEqpId(), byDstChannelIdx, cTvwChnnl))
		{
			continue;
		}		
		
		// ��ȡ��ǽ�ڵĵ�ǰ�ն�
		tDropMt = (TMt)cTvwChnnl.GetMember();
		
		// ��������ն�����ն˲�ͬ,��Ҫ�Ѿ��ն��߳�ǽ
		if ( tDropMt.IsNull()						// ��Ϊ��
			|| !(tDropMt == tOrgSrcMt)				// �¾ɲ����
			|| !bCanSwitchToTW)						// ���ܽ�������ǽ
		{
			// �Ѿ��ն��߳�ǽ
			if( IsValidHduChn(tDstMt.GetEqpId(), byDstChannelIdx) )
			{
				// [2013/03/11 chenbing] VCS���鲻֧��HDU�໭��,��ͨ����0
				ChangeHduSwitch( NULL, tDstMt.GetEqpId(), byDstChannelIdx, 0, 
					cTvwChnnl.GetMember().byMemberType, TW_STATE_STOP,
					MODE_BOTH, FALSE, FALSE);	
			}
			else 
			{
				ChangeTvWallSwitch(&tDropMt, tDstMt.GetEqpId(), byDstChannelIdx,
					cTvwChnnl.GetMember().byMemberType, TW_STATE_STOP);
			}
			
			// ���ն˽�ǽ
			if( bCanSwitchToTW )
			{						
				VCSConfMTToTW( tSwitchInfo );
			}
			else
			{
				// tzy ������飬Ԥ������ǽģʽʱ��VCS���鲻�����ն˻�����MCS�����ն�ռ�õĵ���ǽͨ��
				if ( tDropMt.GetConfIdx() == 0 ||
					tDropMt.GetConfIdx() == m_byConfIdx)
				{
					// ����ͨ��
					if( g_cMcuVcApp.GetPeriEqpStatus( tDstMt.GetEqpId(),&tStatus ) )
					{
						tStatus.m_tStatus.tTvWall.atVideoMt[byDstChannelIdx].SetMt( tOrgSrcMt );
						tStatus.m_tStatus.tTvWall.atVideoMt[byDstChannelIdx].byMemberType = TW_MEMBERTYPE_VCSSPEC;
						g_cMcuVcApp.SetPeriEqpStatus( tDstMt.GetEqpId(), &tStatus );
					}
				}
			}
		}

		if( !tDropMt.IsNull() )
		{				
			//�ǵ�ǰ�����ն��˳�����ǽ�����Ѿ���һ���������ܣ����˳����ƻ���
			if( m_cVCSConfStatus.GetMtInTvWallCanMixing() 
				&& m_tConf.m_tStatus.IsMixing() 
				&& m_tConf.m_tStatus.IsSpecMixing() 
				&& VCSMtNotInTvWallCanStopMixing( tDropMt ))
			{
				RemoveSpecMixMember( &tDropMt, 1, FALSE );
			}				
		}		
		
		// �����µ�ͨ��״̬�ϱ��������
		if (g_cMcuVcApp.GetPeriEqpStatus( tDstMt.GetEqpId(),&tStatus ))
		{
			cServMsgSend.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
			SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsgSend);
		}		
		
		// �Ƿ�Ҫ�ָ��ֱ���
		if( NeedChangeVFormat(tDropMt) 
			&& !m_ptMtTable->IsMtInTvWall( GetLocalMtFromOtherMcuMt(tDropMt).GetMtId() ) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "Restore Format mt(%d.%d)\n", tDropMt.GetMcuId(), tDropMt.GetMtId() );
			ChangeVFormat( tDropMt,FALSE );					//�ָ��ֱ���			
		}
	}

	m_cVCSConfStatus.SetIsTvWallOperating( FALSE );
}

/*====================================================================
    ������      : VCSTransOutMsgToInnerMsg
    ����        �����ⲿͨ����Ϣ��ת��Ϊ�ڲ�������Ϣ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[IN/OUT]cOutMsg:�ⲿ��Ϣ��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                ���㻪          ����
====================================================================*/
void CMcuVcInst::VCSTransOutMsgToInnerMsg(CServMsg& cOutMsg)
{
	CServMsg cInnerMsg = cOutMsg;
	u8* pbyBuf = cOutMsg.GetMsgBody();

	if (VCS_MCU_SETMTINTVWALL_REQ == cOutMsg.GetEventId())
	{
		// ����MCSͨ�ŵ���Ϣ��ת��Ϊ�򵥲�����ڲ�������Ϣ��(u8(������) + TSwitchInfo + ...
		u8 bySwitchInfoNum = *pbyBuf++;
		cInnerMsg.SetMsgBody(&bySwitchInfoNum, sizeof(bySwitchInfoNum));
		TSwitchInfo tTemSwitchInfo;
		for (u8 byLoop = 0; byLoop < bySwitchInfoNum; byLoop++ )
		{
			if (VCS_DTYPE_MTALIAS == *pbyBuf++)
			{
				TMt tVCMT; 
				u8 byAliasType = *pbyBuf++;
				u8 byAliasLen  = *pbyBuf++;
				TMtAlias tMtAlias;
				if (tMtAlias.SetAlias(byAliasType, byAliasLen, (s8*)pbyBuf))
				{
					pbyBuf += byAliasLen;
					u8 byForce = !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode());
					tVCMT = GetVCMTByAlias(tMtAlias, ntohs( *(u16*)pbyBuf ),byForce);
					pbyBuf += sizeof(u16);

				}
				else
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuMtInTwMsg]bad alias info(type:%d byaliaslen:%d)\n",
							  byAliasType, byAliasLen);
					pbyBuf = pbyBuf + byAliasLen + sizeof(u16);
					tVCMT.SetNull();
				}
				tTemSwitchInfo = *(TSwitchInfo*)pbyBuf;
				tTemSwitchInfo.SetSrcMt(tVCMT);
			}
			else
			{
				tTemSwitchInfo = *(TSwitchInfo*)pbyBuf;
			}
			cInnerMsg.CatMsgBody((u8*)&tTemSwitchInfo, sizeof(TSwitchInfo));
			pbyBuf += sizeof(TSwitchInfo);
		}
	}
	else
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSTransOutMsgToInnerMsg]unproc msg(%d)\n",
			      cOutMsg.GetEventId());
	}

	cOutMsg = cInnerMsg;
}
/*====================================================================
    ������      ProcVcsMcuMtInTwMsg
    ����        ���ն˽�����ǽͨ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26                �ܾ���          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuMtInTwMsg( const CMessage *pcMsg )
{
	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	if( m_cVCSConfStatus.GetIsTvWallOperating() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuMtInTwMsg] Now is operation tvWall.Nack\n" );
		cServMsg.SetErrorCode(ERR_MCU_VCS_PLANISOPERATING);
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		return;
	}

	//action abort when no chairman online
	TMt tChairMan = m_tConf.GetChairman();
	if( ( tChairMan.IsNull()  || !m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) 
		&& m_byCreateBy != CONF_CREATE_MT )
	{
		cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
		cServMsg.SetEventId(cServMsg.GetEventId() + 2);
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}
	
	u8 bySwitchInfoNum = *cServMsg.GetMsgBody();
	if( 0 == bySwitchInfoNum )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuMtInTwMsg] bySwitchInfoNum is 0.Nack\n " );
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		return;
	}	

	// �ж���һ��������ȵ��ն��Ƿ���ɵ���
	TMt tOldReqVCMT = m_cVCSConfStatus.GetReqVCMT();
	if (!tOldReqVCMT.IsNull())
	{
		cServMsg.SetErrorCode(ERR_MCU_VCS_VCMTING);
		cServMsg.SetEventId(cServMsg.GetEventId() + 2);				
		cServMsg.SetMsgBody((u8*)&tOldReqVCMT, sizeof(TMt));
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}
	// ����ϳ����ģʽ,�ϳ���δ����Ҳ������Ӧ
	if (g_cMcuVcApp.GetVMPMode(m_tVmpEqp) == CONF_VMPMODE_NONE 
		&& (VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() ||
			VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode() ||
			VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode())
		)
	{
		cServMsg.SetErrorCode(ERR_MCU_VMPNOTSTART);
		cServMsg.SetEventId(cServMsg.GetEventId() + 2);
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}

	CServMsg cInnerMsg = cServMsg;
	VCSTransOutMsgToInnerMsg(cInnerMsg);

	switch(m_cVCSConfStatus.GetTVWallManageMode())
	{
	case VCS_TVWALLMANAGE_REVIEW_MODE:
		{
			ProcVcsMcuMtInTvWallInReviewMode( cInnerMsg );
			break;
		}

	case VCS_TVWALLMANAGE_MANUAL_MODE:
		{
			ProcVcsMcuMtInTvWallInManuMode( cInnerMsg);
			break;
		}

	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuMtInTwMsg]unproc twmode(%d)\n", m_cVCSConfStatus.GetTVWallManageMode());
	    break;
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
			//action abort when no chairman online
			TMt tChairMan = m_tConf.GetChairman();
			if( ( tChairMan.IsNull()  || !m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) 
				&& m_byCreateBy != CONF_CREATE_MT )
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

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
						m_ptMtTable->SetCallMode( ptMt->GetMtId(),CONF_CALLMODE_TIMER );
						InviteUnjoinedMt(cServBackMsg, ptMt);						
					}
					else
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ProcVcsMcuGroupCallMtReq]mtid(%d) not in mtlist of the conf or has joined the conf\n",
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
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuGroupCallMtReq] Wrong message %u(%s) received in state %u!\n", 
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

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVcsMcuGroupDropMtReq]byGroupNum.%d\n", byGroupNum);
			for (u8 byGroupIdx = 0; byGroupIdx < byGroupNum; byGroupIdx++)
			{
				byGroupNameLen = *pbyMsg++;

				if (!m_cVCSConfStatus.OprGroupCallList((s8*)pbyMsg, byGroupNameLen, FALSE, wErrCode))
				{
					cServMsg.SetMsgBody(pbyMsg, byGroupNameLen);
					cServMsg.SetErrorCode(wErrCode);
					SendReplyBack(cServBackMsg, cServMsg.GetEventId() + 3);
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "OprGroupCallList(%s,%d) failed, err.%d\n", (s8*)pbyMsg, byGroupNameLen, wErrCode);
					continue;
				}

				pbyMsg += byGroupNameLen;
				byCallMtNum = *pbyMsg++;
				TMt* ptMt = (TMt*)pbyMsg;
				for (u8 byIdx = 0; byIdx < byCallMtNum; byIdx++)
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcVcsMcuGroupDropMtReq] Group Drop Mt.%d at loop[%d,%d]\n",ptMt->GetMtId(), byIdx, byCallMtNum );
					if (m_tConfAllMtInfo.MtJoinedConf(ptMt->GetMtId()))
					{
						VCSDropMT(*ptMt);
					}
					else
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVcsMcuGroupDropMtReq]mtid(%d) not joined the conf IsInTvWall(%d) IsInHdu(%d)\n",
							   ptMt->GetMtId(),m_ptMtTable->IsMtInTvWall( ptMt->GetMtId() ),
							   m_ptMtTable->IsMtInTvWall( ptMt->GetMtId() ) );
						
						// [10/20/2011 liuxu] ����Ҷ�ʱ, ���������ն˶���Ҫ��ճ�����ǽ
						//VCSClearTvWallChannelByMt( *ptMt );	
						
						RemoveJoinedMt( *ptMt,TRUE );						
					}

					// [10/20/2011 liuxu] ����Ҷ�ʱ, ���������ն˶���Ҫ��ճ�����ǽ
					VCSClearTvWallChannelByMt( *ptMt );	

					m_ptMtTable->SetCallMode( ptMt->GetMtId(),CONF_CALLMODE_NONE );
					VCSClearMtDisconnectReason( *ptMt );					

					ptMt++;
					pbyMsg += sizeof(TMt);
				}			
			}

			VCSConfStatusNotif();

		}
		break;

	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuGroupDropMtReq] Wrong message %u(%s) received in state %u!\n", 
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
			//action abort when no chairman online
			TMt tChairMan = m_tConf.GetChairman();
			if( ( tChairMan.IsNull()  || 
				!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
				m_byCreateBy != CONF_CREATE_MT
				)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			if (m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP)
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcVcsMcuStartChairPollReq]Chairpoll has already started\n");
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
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuStartChairPollReq] Wrong message %u(%s) received in state %u!\n", 
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
void CMcuVcInst::ProcChairPollTimer( void )
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

	// miaoqingsong [06/27/2011] ����ϯ��ѯѡ���߼�����ʱ������ͣ�ϴ�ѡ�������´�ѡ�����ʷſ�����ע������
 	TMt tChairman = m_tConf.GetChairman();
	//zhouyiliang 20121225 ����ϴ���ѯ�ĺ����Ҫ��ѯ����ͬһ���նˣ������²𽨽���
	TMt tPollMt = VCSGetNextPollMt();
	TMtStatus tMtState;
	m_ptMtTable->GetMtStatus(tChairman.GetMtId(), &tMtState);
	TMt tSelectVideoMt = tMtState.GetSelectMt(MODE_VIDEO);
	if ( !tPollMt.IsNull() && !tSelectVideoMt.IsNull() && ( tPollMt == tSelectVideoMt ) )
	{
		SetTimer(MCUVC_VCS_CHAIRPOLL_TIMER, 1000 * m_cVCSConfStatus.GetPollIntval());
		return;
	}
	// miaoqingsong [06/27/2011] ����ϯ��ѯѡ���߼�����ʱ������ͣ�ϴ�ѡ�������´�ѡ�����ʷſ�����ע������
	if (!tSelectVideoMt.IsNull() )
	{
		VCSConfStopMTSel(tChairman, MODE_VIDEO);
	}

	// zjj
// 	TMtStatus tMtState;
// 	m_ptMtTable->GetMtStatus(tChairman.GetMtId(), &tMtState);	
// 	if (!tMtState.GetSelectMt(MODE_VIDEO).IsNull())
// 	{
// 		VCSConfStopMTSel(tChairman, MODE_VIDEO);
// 	}


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
		StopSwitchToSubMt( 1,&tChairman,MODE_VIDEO );		
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
			VcsStopChairManPoll();
// 			KillTimer(MCUVC_VCS_CHAIRPOLL_TIMER);
// 			if (m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP)
// 			{
// 				TMt tNullMt;
// 				tNullMt.SetNull();
// 				m_cVCSConfStatus.SetChairPollState(VCS_POLL_STOP);
// 				m_cVCSConfStatus.SetCurChairPollMt(tNullMt);
// 				m_cVCSConfStatus.SetPollIntval(0);
// 
// 				VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);
// 
// 				TMt tChairMan = m_tConf.GetChairman();
// 
//                 // miaoqingsong [06/22/2011] VCS�������ǽģʽ/�����ϯģʽ/�������ģʽ�£���ϯ��᲻Ϊ��
// 				// ���Ҵ��ڵ����ն˵������£�ͣ��ϯ��ѯ����Ҫ�ָ���ϯѡ����ǰ�����ն˵���Ƶ
// 				if ( (m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPTW_MODE ||
// 					  m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPCHAIRMAN_MODE ||
// 					  m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE) 
// 					  && !m_cVCSConfStatus.GetCurVCMT().IsNull() 
// 					  && !tChairMan.IsNull()	// filter chairman
// 					  && m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId())
// 					 ) 
// 				{
// 					TSwitchInfo tSwitchInfo;
// 					tSwitchInfo.SetSrcMt( m_cVCSConfStatus.GetCurVCMT() );
// 					tSwitchInfo.SetDstMt( m_tConf.GetChairman() );
// 					tSwitchInfo.SetMode( MODE_VIDEO );
// 					VCSConfSelMT( tSwitchInfo );
// 				}
// 
// 				VCSConfStatusNotif();
// 			}

			
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuStopChairPollReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}


/*====================================================================
    ������      VcsStopChairManPoll
    ����        ��ֹͣ��ϯ��ѯ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    12/09/06                zhouyiliang          ����
====================================================================*/
void CMcuVcInst::VcsStopChairManPoll()
{
	switch(CurState())
	{
	case STATE_ONGOING:
		{

			KillTimer(MCUVC_VCS_CHAIRPOLL_TIMER);
			if (m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP)
			{
				TMt tNullMt;
				tNullMt.SetNull();
				m_cVCSConfStatus.SetChairPollState(VCS_POLL_STOP);
				m_cVCSConfStatus.SetCurChairPollMt(tNullMt);
				m_cVCSConfStatus.SetPollIntval(0);
				

				TMt tChairMan = m_tConf.GetChairman();

                // miaoqingsong [06/22/2011] VCS�������ǽģʽ/�����ϯģʽ/�������ģʽ�£���ϯ��᲻Ϊ��
				// ���Ҵ��ڵ����ն˵������£�ͣ��ϯ��ѯ����Ҫ�ָ���ϯѡ����ǰ�����ն˵���Ƶ
				if ( !tChairMan.IsNull()	// filter chairman
					  && m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId())
					 ) 
				{
					if( (m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPTW_MODE ||
						  m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPCHAIRMAN_MODE ||
						  m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE) 
							&& !m_cVCSConfStatus.GetCurVCMT().IsNull() &&
							m_tConfAllMtInfo.MtJoinedConf(m_cVCSConfStatus.GetCurVCMT().GetMcuId(), m_cVCSConfStatus.GetCurVCMT().GetMtId())
						)
					{
						VCSConfStopMTSel( m_tConf.GetChairman(), MODE_VIDEO,FALSE );
						TSwitchInfo tSwitchInfo;
						tSwitchInfo.SetSrcMt( m_cVCSConfStatus.GetCurVCMT() );
						tSwitchInfo.SetDstMt( m_tConf.GetChairman() );
						tSwitchInfo.SetMode( MODE_VIDEO );
						VCSConfSelMT( tSwitchInfo );
					}
					else
					{
						TMtStatus tMtStatus; 
						m_ptMtTable->GetMtStatus( m_tConf.GetChairman().GetMtId(), &tMtStatus );
						TMt tVidSrc = tMtStatus.GetSelectMt( MODE_VIDEO );

						VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);
						if( tVidSrc.IsNull() )
						{
							RestoreRcvMediaBrdSrc( m_tConf.GetChairman().GetMtId(),MODE_VIDEO );
						}					
					}
				}
				else
				{
					VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);
				}
				
				

				VCSConfStatusNotif();
			}

			
		}
		break;
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VcsStopChairManPoll] handle in wrong state %u!\n", CurState() );
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
			//action abort when no chairman online
			TMt tChairMan = m_tConf.GetChairman();
			if( ( tChairMan.IsNull()  || 
				!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
				m_byCreateBy != CONF_CREATE_MT
				)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			//TAddMtInfo* ptAddMtInfo = (TAddMtInfo*)cServMsg.GetMsgBody();
			CServMsg cSendServMsg;
			cSendServMsg.SetConfIdx( m_byConfIdx );
			cSendServMsg.SetConfId( m_tConf.GetConfId() );
			
			TAddMtInfo* ptAddMtInfo = NULL;
			u8 *pbyBuf = cServMsg.GetMsgBody();
			u8 byCount = *pbyBuf;
			if( 0 == byCount )
			{
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcVcsMcuAddMtReq] byCount is 0.Nack\n");
				return;
			}
			
			pbyBuf++;

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ProcVcsMcuAddMtReq] byCount.%d\n",byCount );

			
			for( u8 byIdx = 0;byIdx<byCount;byIdx++ )
			{
				ptAddMtInfo = (TAddMtInfo*)pbyBuf;
				//���ܺ��л��鱾��
				if (mtAliasTypeE164 == ptAddMtInfo->m_AliasType)
				{
					if (0 == strcmp(ptAddMtInfo->m_achAlias, m_tConf.GetConfE164()))
					{
						cServMsg.SetErrorCode(ERR_MCU_NOTCALL_CONFITSELF);
						SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
						ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcVcsMcuAddMtReq]Cannot Call conf itself.\n");
						return;
					}
				}

				//�Ƿ񳬹���������
				if (!m_cVCSConfStatus.IsAddEnable())
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_OVERADDMTNUM);
					SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
					ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcVcsMcuAddMtReq]over addmt num\n");
					return;
				}
				
				//���������ն��б�
				u8 byMtId = AddMt(*ptAddMtInfo, ptAddMtInfo->GetCallBitRate(), ptAddMtInfo->GetCallMode());
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ProcVcsMcuAddMtReq]add mt-%d callmode-%d DialBitRate-%d type-%d alias-", 
						byMtId, ptAddMtInfo->GetCallMode(), ptAddMtInfo->GetCallBitRate(), 
						ptAddMtInfo->m_AliasType );
				if ( ptAddMtInfo->m_AliasType == mtAliasTypeTransportAddress )
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "%s:%d!\n", StrOfIP(ptAddMtInfo->m_tTransportAddr.GetIpAddr()), 
									   ptAddMtInfo->m_tTransportAddr.GetPort() );
				}
				else
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "%s!\n",	ptAddMtInfo->m_achAlias);
				}

				if (0 == byMtId)
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcVcsMcuAddMtReq]Fail to alloc mtid\n");
					SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
					return;
				}

				//SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

				cSendServMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
    

				TMt tMt = m_ptMtTable->GetMt( byMtId );
				if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
				{
					//InviteUnjoinedMt( cServMsg, &tMt, TRUE, TRUE );
					InviteUnjoinedMt( cSendServMsg, &tMt, TRUE, TRUE );
				}

				//������ػ��������ն���Ϣ
				//cSendServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
				//cSendServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
				SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cSendServMsg );

				//��������ն˱�
				SendMtListToMcs(LOCAL_MCUIDX);

				m_cVCSConfStatus.OprNewMt(tMt, TRUE);

				pbyBuf += sizeof( TAddMtInfo );

			}
			
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

			VCSConfStatusNotif();
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuAddMtReq] Wrong message %u(%s) received in state %u!\n", 
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
			//action abort when no chairman online
			TMt tChairMan = m_tConf.GetChairman();
			if( ( tChairMan.IsNull()  || 
				!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
				m_byCreateBy != CONF_CREATE_MT
				)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			TMt tMt = *(TMt*)cServMsg.GetMsgBody();
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

			if (!tMt.IsNull() && 
				m_cVCSConfStatus.OprNewMt(tMt, FALSE))
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ProcVcsMcuDelMtReq]drop mt(mcuid:%d, mtid:%d)\n", 
					   tMt.GetMcuId(), tMt.GetMtId());
				VCSDropMT(tMt);

				VCSConfStatusNotif();
			}
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuDelMtReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcVcsMcuChangeDualStatusReq
    ����        ���ı���鵱ǰ˫��ʹ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/13/2010              �ű���          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuChangeDualStatusReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
    switch(CurState())
    {
    case STATE_ONGOING:
        {
            u8 byStatus = *cServMsg.GetMsgBody();
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "byStatus.%d\n",byStatus );

			if (CONF_DUALSTATUS_ENABLE == byStatus || CONF_DUALSTATUS_DISABLE == byStatus)
			{
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
			}
			else
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuChangeDualStatusReq] unexpected DsStatus:%d!\n", byStatus);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			}

            
            u8 byOldStatus = m_cVCSConfStatus.IsConfDualEnable() ? CONF_DUALSTATUS_ENABLE : CONF_DUALSTATUS_DISABLE;

			if (byStatus != byOldStatus)
			{
				if (CONF_DUALSTATUS_DISABLE == byStatus)
				{
					StopDoubleStream(TRUE, FALSE);
				}
				m_cVCSConfStatus.SetConfDualEnable(byStatus);
				VCSConfStatusNotif();
			}
            
        }
        break;
    default:
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
        ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuChangeDualStatusReq] Wrong message %u(%s) received in state %u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        break;
    }
}

/*====================================================================
    ������      ��ProcVcsMcuMtJoinConfRsp
    ����        ���ն��������Ӧ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    11/12/2010              �ܾ���          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuMtJoinConfRsp( const CMessage * pcMsg )
{
	if( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuMtJoinConfRsp] Wrong message %u(%s) received in state %u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	TMtAlias    tFstAlias = *( TMtAlias * )( cServMsg.GetMsgBody() + sizeof(u8) * 2 );
	TMtAlias    tMtH323Alias = *( TMtAlias * )( cServMsg.GetMsgBody() + sizeof(u8) * 2 + sizeof(TMtAlias) );
	TMtAlias    tMtE164Alias = *( TMtAlias * )( cServMsg.GetMsgBody() + sizeof(TMtAlias) * 2 + sizeof(u8) * 2 );
	TMtAlias    tMtAddr      = *( TMtAlias * )( cServMsg.GetMsgBody() + sizeof(TMtAlias) * 3 + sizeof(u8) * 2 );
	u8          byEncrypt    = *(cServMsg.GetMsgBody());
	u8			bySrcDriId = *(cServMsg.GetMsgBody() + sizeof(u8) );
	u16         wCallRate    = *(u16*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)));
	u8			bySrcSsnId = *(u8*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)+sizeof(u16)));
	u8			byMtInConf = *(u8*)(cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)+sizeof(u16)+sizeof(u8));
    wCallRate = htons( wCallRate ); 
	u8 byMtId = 0;
	

    switch(cServMsg.GetEventId())
    {
    case VCS_MCU_MTJOINCONF_ACK:
        {
            if (byMtInConf == 0)
            {
                if ( !tMtE164Alias.IsNull() )
                {
                    tFstAlias = tMtE164Alias;
                }
                else if ( !tMtH323Alias.IsNull() )
                {
                    tFstAlias = tMtH323Alias;
                }
                else
                {
                    tFstAlias = tMtAddr;
                }
            }
			

            wCallRate -= GetAudioBitrate( m_tConf.GetMainAudioMediaType() );               
        
			//�����ն�ID��
			byMtId = AddMt( tFstAlias, wCallRate, CONF_CALLMODE_NONE/*, TRUE*/ );

			//�ն˺�����ᣬ��ز��������б����Ϊ��������������Ϊ��������նˣ���ͨ���򿪺����ϯ����ѡ��
			if ( VCS_CONF == m_tConf.GetConfSource() && !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
				CONF_CREATE_MT != m_byCreateBy 
				)
			{
                //[11/22/2011 zhushengze]�ն��Ѿ���ᣬMCUVC_VCMTOVERTIMER_TIMER�Ͳ�����֮��ɱ��
                //�����Ѿ�����ն�MCUVC_VCMTOVERTIMER_TIMER����ʱ��remove
                if (m_tConfAllMtInfo.MtJoinedConf( byMtId ))
                {
                    LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ProcVcsMcuMtJoinConfRsp]mt.%d has joined conf!\n", byMtId);
                    return;
                }

				//zjj20110105 �ڵ�������ģʽ��,����ڵ����ն�ʱǰһ�������ն˻��ڵ��ȹ����о;ܾ������ն˵��������
				if( !m_cVCSConfStatus.GetReqVCMT().IsNull()	)
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuMtJoinConfRsp] Fail to Call Mt(%s),because now has req Mt(%d.%d)\n",
							tFstAlias.m_achAlias,m_cVCSConfStatus.GetReqVCMT().GetMcuId(),
							m_cVCSConfStatus.GetReqVCMT().GetMtId()
							);
					cServMsg.SetSrcMtId(CONF_CREATE_MT);
					cServMsg.SetSrcDriId( bySrcDriId );
					cServMsg.SetSrcSsnId( bySrcSsnId );
					cServMsg.SetConfIdx( m_byConfIdx );
					cServMsg.SetConfId( m_tConf.GetConfId() );
					cServMsg.SetEventId( MCU_MT_CREATECONF_NACK );
					g_cMtAdpSsnApp.SendMsgToMtAdpSsn(bySrcDriId, MCU_MT_CREATECONF_NACK, cServMsg);
					return;
				}
				TMt tMt = m_ptMtTable->GetMt(byMtId);
				m_cVCSConfStatus.SetReqVCMT( tMt );
				SetTimer( MCUVC_VCMTOVERTIMER_TIMER, (g_cMcuVcApp.GetVcsMtOverTimeInterval() + 5)*1000  );			
			}

			// zbq [08/09/2007] ��������Ip�κ���,�����б����ֲ���Ip���ն�,�˴��豣����Ip
			if ( !(tFstAlias == tMtAddr) )
			{
				m_ptMtTable->SetMtAlias( byMtId, &tMtAddr );
				// guzh [10/29/2007] ����TMtExt �����IP��ַ
				m_ptMtTable->SetIPAddr( byMtId, tMtAddr.m_tTransportAddr.GetIpAddr() );
			}
        
			// xsl [11/8/2006] ������ն˼�����1
			// xliang [2/14/2009] ������MT����MCU,������
			//u8 byDriId = cServMsg.GetSrcDriId();		
			g_cMcuVcApp.IncMtAdpMtNum( bySrcDriId, m_byConfIdx, byMtId );
			m_ptMtTable->SetDriId(byMtId, bySrcDriId);
	// 		if(byType == TYPE_MCU)
	// 		{
	// 			//�������ն˼�����Ҫ+1
	// 			//��mtadplib�Ƕ���ͨ���жϣ����Կ϶���ʣ����������ռ��
	// 			g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum++;
	// 		}
			
			if( byMtId > 0  )
			{
				//���Ϊ�����ն�
				//m_ptMtTable->SetMtCallingIn( byMtId, TRUE );
            
				// zbq [08/31/2007] �����ն����º��뱣�����������
            
                if ( 0 == byMtInConf )
                {
                    m_ptMtTable->SetNotInvited( byMtId, TRUE );
                }
				
            
				//m_ptMtTable->SetAddMtMode(byMtId, ADDMTMODE_MTSELF);
				
				
				m_ptMtTable->SetMtType( byMtId, MT_TYPE_MT );
				
				
				m_ptMtTable->SetMtAlias(byMtId, &tMtH323Alias);
				m_ptMtTable->SetMtAlias(byMtId, &tMtE164Alias);
				if( /*!bMtInConf*/
					!m_tConfAllMtInfo.MtJoinedConf( byMtId )&& 
					VCS_CONF == m_tConf.GetConfSource() && 				
					CONF_CREATE_MT != m_byCreateBy
					)
				{
					TMt tMt = m_ptMtTable->GetMt( byMtId );
					m_cVCSConfStatus.OprNewMt(tMt, TRUE);
					VCSConfStatusNotif();
				}
				
				cServMsg.SetConfIdx( m_byConfIdx );
				cServMsg.SetConfId( m_tConf.GetConfId() );
				cServMsg.SetDstMtId( byMtId );
				cServMsg.SetMsgBody(&byEncrypt, sizeof(u8));
				TCapSupport tCap = m_tConf.GetCapSupport();
				cServMsg.CatMsgBody( (u8*)&tCap, sizeof(tCap));
				TMtAlias tAlias;
				tAlias.SetH323Alias(m_tConf.GetConfName());
				cServMsg.CatMsgBody((u8 *)&tAlias, sizeof(tAlias));

				// Ex��������Ҫ��������Mtadp�࣬������ֱ���Mtadp��û�н������������µ�ʱ�򣨼���û��Ex����ʱ�����Զ�������������
				// ���±ȳ��Ĺ�ͬ˫����������
				TCapSupportEx tCapEx = m_tConf.GetCapSupportEx();
				cServMsg.CatMsgBody( (u8*)&tCapEx, sizeof(tCapEx));

				// ��֯��չ��������ѡ��Mtadp [12/8/2011 chendaiwei]
				TVideoStreamCap atMSVideoCap[MAX_CONF_CAP_EX_NUM];
				u8 byCapNum = MAX_CONF_CAP_EX_NUM;
				m_tConfEx.GetMainStreamCapEx(atMSVideoCap,byCapNum);
				
				TVideoStreamCap atDSVideoCap[MAX_CONF_CAP_EX_NUM];
				u8 byDSCapNum = MAX_CONF_CAP_EX_NUM;
				m_tConfEx.GetDoubleStreamCapEx(atDSVideoCap,byDSCapNum);
				
				cServMsg.CatMsgBody((u8*)&atMSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
				cServMsg.CatMsgBody((u8*)&atDSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);

				TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//��Ƶ����
				//�ӻ���������ȡ������֧�ֵ���Ƶ����
				m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
				cServMsg.CatMsgBody((u8*)&atAudioTypeDesc[0], sizeof(TAudioTypeDesc)* MAXNUM_CONF_AUDIOTYPE);

				cServMsg.SetEventId( MCU_MT_CREATECONF_ACK );
				cServMsg.SetSrcMtId(CONF_CREATE_MT);
				cServMsg.SetSrcDriId( bySrcDriId );
				cServMsg.SetSrcSsnId( bySrcSsnId );
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn(bySrcDriId, MCU_MT_CREATECONF_ACK, cServMsg);			
				
				g_cMcuVcApp.UpdateAgentAuthMtNum();
			}
			else
			{
				cServMsg.SetSrcMtId(CONF_CREATE_MT);
				cServMsg.SetSrcDriId( bySrcDriId );
				cServMsg.SetSrcSsnId( bySrcSsnId );
				cServMsg.SetConfIdx( m_byConfIdx );
				cServMsg.SetConfId( m_tConf.GetConfId() );
				cServMsg.SetEventId( MCU_MT_CREATECONF_NACK );
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn(bySrcDriId, MCU_MT_CREATECONF_NACK, cServMsg);

				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt 0x%x join conf %s request was refused because conf full!\n", 
						  tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
			}

			//cServMsg.SetEventId( MCU_MT_CREATECONF_ACK );
			//cServMsg.SetMsgBody( cServMsg.GetMsgBody(),sizeof(u8) + sizeof(TCapSupport) + sizeof(TMtAlias) );
            //g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), MCU_MT_CREATECONF_ACK , cServMsg);
        }
        break;
	case VCS_MCU_MTJOINCONF_NACK:
		{
			cServMsg.SetSrcMtId(CONF_CREATE_MT);
			cServMsg.SetSrcDriId( bySrcDriId );
			cServMsg.SetSrcSsnId( bySrcSsnId );
			cServMsg.SetConfIdx( m_byConfIdx );
			cServMsg.SetConfId( m_tConf.GetConfId() );
			cServMsg.SetEventId( MCU_MT_CREATECONF_NACK );			
			cServMsg.SetErrorCode( ERR_MCU_CONFNUM_EXCEED );			
            g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), MCU_MT_CREATECONF_NACK , cServMsg);
		}
		break;
    default:  
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuMtJoinConfRsp] Wrong message %u(%s) received\n", 
				::OspEventDesc( pcMsg->event )
				);
        break;
    }
}

/*====================================================================
������      ��ProcVcsMcuGroupModeConfLockCmd
����        �������������ͽ�������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const CMessage * pcMsg, �������Ϣ 
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
05/19/2011              ��ʤ��          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuGroupModeConfLockCmd( const CMessage * pcMsg )
{
    if( STATE_ONGOING != CurState() )
    {
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVcsMcuGroupModeConfLockCmd] Wrong message %u(%s) received in state %u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        return;
    }
    
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
    u8 byIsLock = *(u8*)cServMsg.GetMsgBody();

    //�����¼����ܸı�����״̬
    if (m_tCascadeMMCU.IsNull())
    {
		BOOL32 bIsLock = ( 1 == byIsLock ) ? TRUE : FALSE;
        m_cVCSConfStatus.SetGroupModeLock( bIsLock);
    }
       
    VCSConfStatusNotif();
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
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVcsMcuMsg] %d(%s) passed\n", pcMsg->event, OspEventDesc(pcMsg->event));


	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	

	// �����鴦�ڱ��ϼ�����״̬,��ܾ��κε��Ȳ���
	// ����MCS���飬�յ�Ԥ�����������Ϣ��Ӧ�÷Ź�У��[11/16/2012 chendaiwei]
	if ( CONF_CREATE_MT == m_byCreateBy  &&
	    !(m_tConf.GetConfSource() == MCS_CONF
		 && (VCS_MCU_ADDPLANNAME_REQ == pcMsg->event  ||
		    VCS_MCU_DELPLANNAME_REQ == pcMsg->event   ||
		    VCS_MCU_MODIFYPLANNAME_REQ == pcMsg->event||
		    VCS_MCU_GETALLPLANDATA_REQ == pcMsg->event ))
		)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS,"[ProcVcsMcuMsg]Conf is Cascade by other mcu.So Can't do anything.\n" );
		cServMsg.SetErrorCode(ERR_MCU_VCS_MMCUINVC);
		cServMsg.SetEventId(cServMsg.GetEventId() + 2);
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}

	//����ʱ���ʵ��
	if( MCU_SCHEDULE_VCSCONF_START == pcMsg->event )
	{
		//���ʵ��
		ClearVcInst();
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
		ProcVcsMcuMuteReq(cServMsg);
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
	//zjj20091026
	case VCS_MCU_SETMTINTVWALL_REQ:
		ProcVcsMcuMtInTwMsg( pcMsg );
		break;

	//zjj20091102
	case VCS_MCU_ADDPLANNAME_REQ://VCS�������Ԥ��
		ProcMcsVcsMcuAddPlanName( pcMsg );
		break;
	case VCS_MCU_DELPLANNAME_REQ:			//VCS����ɾ��Ԥ��
		ProcMcsVcsMcuDelPlanName( pcMsg );
		break;
	case VCS_MCU_MODIFYPLANNAME_REQ:		//VCS�����޸�Ԥ������
		ProcMcsVcsMcuModifyPlanName( pcMsg );
		break;
	//case VCS_MCU_SAVEPLANDATA_REQ:			//VCS���󱣴�ĳ��Ԥ��
		//ProcVcsMcuSavePlanData( pcMsg );
		//break;
	case VCS_MCU_GETALLPLANDATA_REQ:		//VCS����������Ԥ��������Ϣ
		ProcMcsVcsMcuGetAllPlanData( pcMsg );
		break;
	case VCS_MCU_CHGTVWALLMODE_REQ:			//vcs����mcu�ı����ǽģʽ
		ProcVcsMcuChgTvWallMgrMode( pcMsg );
		break;
	//case VCS_MCU_REVIEWMODEMTINTVWALL_REQ:	//VCS֪ͨmcu�ն˽������ǽͨ��(ֻ������Ԥ��ģʽ��)
		//break;
	case VCS_MCU_CLEARONETVWALLCHANNEL_CMD:	//VCS֪ͨmcu���ĳ������ǽͨ����Ϣ(ֻ���ڷ��Զ�ģʽ)
		ProcVcsMcuClearOneTvWallChannel( pcMsg );
		break;
	case VCS_MCU_CLEARALLTVWALLCHANNEL_CMD:	//VCS֪ͨmcu���ȫ������ǽͨ����Ϣ(ֻ���ڷ��Զ�ģʽ)
		ProcVcsMcuClearAllTvWallChannel();
		break;
	case VCS_MCU_TVWALLSTARMIXING_CMD:		//vcs֪ͨmcu���õ���ǽͨ���ն˻���״̬	
		ProcVcsMcuTvWallMtStartMixing( pcMsg );
		break;

	case VCS_MCU_START_MONITOR_UNION_REQ:
		ProcVcsMcuStartMonReq(pcMsg);
		break;

	case VCS_MCU_STOP_MONITOR_UNION_REQ:
		ProcVcsMcuStopMonReq(pcMsg);
		break;
	case VCS_MCU_STOP_ALL_MONITORUNION_CMD:
		ProcVcsMcuStopAllMonCmd(pcMsg);
		break;
	/*case VCS_MCU_LOCKPLAN_REQ:				//vcs�������������ĳ��Ԥ��
		ProcVcsMcuLockPlan( pcMsg );
		break;
	case VCS_MCU_VCSUNLOCKPLAN_CMD:
		ProcVcsMcuUnLockPlanByVcsssnInst( pcMsg );
		break;
	*/
	case VCS_MCU_CHGDUALSTATUS_REQ:
		ProcVcsMcuChangeDualStatusReq( pcMsg );
        break;

	case VCS_MCU_MTJOINCONF_ACK:			//�ն��������ͬ��
	case VCS_MCU_MTJOINCONF_NACK:			//�ն��������ܾ�	
		ProcVcsMcuMtJoinConfRsp( pcMsg );
		break;
    case VCS_MCU_GROUPMODELOCK_CMD://[5/19/2011 zhushengze]�����������״̬����
        ProcVcsMcuGroupModeConfLockCmd( pcMsg );
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
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuRlsMtMsg] wrong msg lenth(real:%d, less than %d)\n",
			    cServMsg.GetMsgBodyLen(), sizeof(TMt));
		return;
	}
	u8* pbyMsgBody = cServMsg.GetMsgBody();
	TMt tMt = *(TMt*)pbyMsgBody;
	pbyMsgBody += sizeof(TMt);

	s8 abyConfName[MAXLEN_CONFNAME + 1] = {0};
	u8 byPermitOrNot = FALSE;

	const u16 wCpyLen = cServMsg.GetMsgBodyLen() - sizeof(TMt);
	memcpy(abyConfName, pbyMsgBody, min(MAXLEN_CONFNAME, wCpyLen));
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
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuRlsMtMsg] receive un exist msg(%d)\n", pcMsg->event);
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
	TMtAlias tBackAlias ;

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

	//zjj20110411 ����ն��б������ϼ�mcu
	if( CONF_CREATE_MT == m_byCreateBy) 
	{
		m_tOrginMtList.SetMtList(byMtNum-1, &tMtAliasArray[1], &awMtDialRate[1]);
	}
	else
	{		
		m_tOrginMtList.SetMtList(byMtNum, tMtAliasArray, awMtDialRate);
	}

	// �õ������ߵı���
	if( CONF_CREATE_MT == m_byCreateBy )
	{
        wAliasBufLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
		u32 nLen = sizeof(TConfInfo)+sizeof(u16)+wAliasBufLen;
		TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
		if(tConfAttrb.IsHasTvWallModule())
			nLen += sizeof(TMultiTvWallModule);
		if(tConfAttrb.IsHasVmpModule())
			nLen += sizeof(TVmpModule);
		u8 byIsHDTWCfg = *(u8*)(cServMsg.GetMsgBody() + nLen);
		nLen += sizeof(u8);
		if (byIsHDTWCfg)
		{
			nLen += sizeof(THDTvWall);
		}
		
		//�¼�hdunumĬ������Ϊ0
		u8 byHduChnlNum = *(u8*)(cServMsg.GetMsgBody() + nLen);
		nLen += sizeof(u8);
		
		u8 byIsVCSSMCUCfg = *(u8*)(cServMsg.GetMsgBody() + nLen);
		nLen += sizeof(u8);
		if (byIsVCSSMCUCfg)
		{
			u16 wAliasBufLen2 = ntohs(*(u16*)(cServMsg.GetMsgBody() + nLen));
			nLen += sizeof(u16) + wAliasBufLen2;
		}
		u8 byMTPackExist = *(u8*)(cServMsg.GetMsgBody() + nLen);
		nLen += sizeof(u8);
		tBackAlias = *(TMtAlias *)(cServMsg.GetMsgBody() + nLen);
		nLen += sizeof(TMtAlias);
		nLen += sizeof(u8);

		// wConfExInfoLength�����п���Ϊ0���ն˴���ʱ���������u16[11/14/2012 chendaiwei]
		u16 wConfExInfoLength = ntohs( *(u16*)(cServMsg.GetMsgBody() + nLen));
		nLen = nLen + wConfExInfoLength + sizeof(u16);

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

	// ��������ϯ�ն˼��ϼ�MCU��Ա
	if( CONF_CREATE_MT == m_byCreateBy) 
	{
		byMtId = AddMt(tMtAliasArray[0], awMtDialRate[0], CONF_CALLMODE_NONE);
        
        // zbq [12/19/2007] ֱ�Ӵ����ն����ڴ˲���IP������E164��������µ�©��
        if ( tMtAliasArray[0].m_AliasType != mtAliasTypeTransportAddress && ptMtAddr )
        {
            m_ptMtTable->SetMtAlias( byMtId, ptMtAddr );
            m_ptMtTable->SetIPAddr( byMtId, ptMtAddr->m_tTransportAddr.GetIpAddr() );                    
        }

        // xsl [11/8/2006] ������ն˼�����1
		// [9/9/2010 xliang] ������MT����MCU, ��MCU�������ն˼����������+1
// 		u16 wExtraNum = (byMtType == TYPE_MCU)? 1: 0;
//         g_cMcuVcApp.IncMtAdpMtNum( cServMsg.GetSrcDriId(), m_byConfIdx, byMtId, wExtraNum );
        g_cMcuVcApp.IncMtAdpMtNum( cServMsg.GetSrcDriId(), m_byConfIdx, byMtId, byMtType );
        m_ptMtTable->SetDriId(byMtId, cServMsg.GetSrcDriId());
	
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
	//������ϯ
    TMtAlias tMtAlias = m_tConf.GetChairAlias();
	u8 byChairMtId = AddMt(tMtAlias);
	m_tConf.SetNoChairman();
	if( byChairMtId > 0 )
	{
        m_tConf.SetChairman( m_ptMtTable->GetMt(byChairMtId) );
	}

	pszMsgBuf = (char*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16) + wAliasBufLen);

	//�������ǽģ��
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if( tConfAttrb.IsHasTvWallModule() )
	{
        TPeriEqpStatus tTWStatus;
		TMultiTvWallModule *ptMultiTvWallModule = (TMultiTvWallModule *)pszMsgBuf;
        m_tConfEqpModule.SetMultiTvWallModule( *ptMultiTvWallModule );
        m_tConfEqpModule.SetTvWallInfo( *ptMultiTvWallModule );

		CServMsg CTvwEqpStatusMsg;
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
				
				// [9/19/2011 liuxu] ����õ���ǽ����������ռ����,����ֱ���޸�
				TTvwMember tTempTvwMbr = tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop];
				if ( tTempTvwMbr.byMemberType != TW_MEMBERTYPE_NULL
					&& tTempTvwMbr.GetConfIdx() != 0
					&& tTempTvwMbr.GetConfIdx() != m_byConfIdx )
				{
					continue;
				}

				// VCS����ǽģ��Ŀǰ��ȡ��̬���ó�Ա�����ģ����ͨ����ָ����Ա������Ҫָ����Щͨ��ʹ��
				// ʹ��ʱ��ָ��ͨ����˳��ʹ�ã�������ͨ����Ϣ���浽����״̬��
				// [10/19/2011 liuxu] ����ʱ��Ԥռ��
//                 tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byMemberType;
//                 tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);

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

						case TW_MEMBERTYPE_SWITCHVMP:
							tMt.SetNull();
							bValid = TRUE;
							break;

                        case TW_MEMBERTYPE_DOUBLESTREAM:
                            tMt = m_tDoubleStreamSrc;
                            bValid = TRUE;
                            break;

                        default:
                            ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Invalid member type %d for idx %d in CreateConf TvWall Module\n", 
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
                        ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Invalid MtIndex :%d in CreateConf TvWall Module\n", byMtIndex);
                    }
				}
			}
			
            g_cMcuVcApp.SetPeriEqpStatus(tTvWallModule.m_tTvWall.GetEqpId(), &tTWStatus);

			if (tTWStatus.m_byOnline)
			{
				// ֪ͨ���
				CTvwEqpStatusMsg.SetMsgBody((u8*)&tTWStatus, sizeof(tTWStatus));
				SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, CTvwEqpStatusMsg);
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

	// VCS: + 1byte(u8: 0 1  �Ƿ������˸������ǽ)
	//      +(��ѡ, THDTvWall])
	//      + 1byte(u8: ��������HDU��ͨ������)
	//      + (��ѡ, THduModChnlInfo+...)
	//      + 1byte(�Ƿ�Ϊ��������)��(��ѡ��2byte[u16 ���������ô�����ܳ���]+�¼�mcu����[1byte(��������)+1byte(��������)+xbyte(�����ַ���)+2byte(��������)...))
	//      + 1byte(�Ƿ�֧�ַ���)��(��ѡ��2byte(u16 ��������Ϣ�ܳ�)+����(1byte(����)+n��[1TVCSGroupInfo��m��TVCSEntryInfo])
	//       + 1 TMtAlias (���ص�ַ�ı���)
	//      + 1byte (vcs �Զ������ָ����ģʽ)
	// �������ǽ
    if (*pszMsgBuf++)
    {
		m_tConfEqpModule.SetHDTvWall((THDTvWall*)pszMsgBuf);
		for( byLoop = 1; byLoop <= m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_tHDTvWall.m_byHDTWDevNum; byLoop++)
		{
			TMtAlias tTmpAlias;
			memcpy(&tTmpAlias,&m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_tHDTvWall.m_atHDTWModuleAlias[byLoop-1],sizeof(THDTWModuleAlias));

			// ��Ӹ��嵽�����б��У���Ϊ���޴ζ�ʱ����
			byMtId = AddMt( tTmpAlias, 
							m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_tHDTvWall.m_awHDTWDialBitRate[byLoop-1],
							CONF_CALLMODE_TIMER );
		}
		pszMsgBuf += sizeof(THDTvWall);
		
		byMtNum += m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_tHDTvWall.m_byHDTWDevNum;
    }
	else
	{
		// δ���ø������ǽ����ղ���
		THDTvWall tNull;
		tNull.SetNull();
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
			byMtId = AddMt( tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], CONF_CALLMODE_NONE );
		}
		pszAliaseBuf += sizeof(u16) + wAliasBufLen;
		
		byMtNum += byDevNum;
    }
	pszMsgBuf++;
	//zhouyiliang 20100820 �������ն˵ı���
	
	memcpy( &tBackAlias, pszMsgBuf, sizeof(TMtAlias));
	pszMsgBuf += sizeof(TMtAlias);
	if ( !tBackAlias.IsNull() ) 
	{
		u8 bybackMt = AddMt( tBackAlias );
		//zhouyiliang 20100818 ������ַ���ݵ�ַ��¼
		if ( bybackMt > 0 ) 
		{
			m_cVCSConfStatus.SetVcsBackupChairMan( m_ptMtTable->GetMt( bybackMt ) );

		}
	}
	else
	{
		TMt tNullMt;
		tNullMt.SetNull();
		m_cVCSConfStatus.SetVcsBackupChairMan( tNullMt );
	}

	// [4/20/2011 xliang] vcs AutoMode
	u8 byVcsAutoMode = *(u8*)pszMsgBuf;
	if( ISGROUPMODE(byVcsAutoMode) )
	{	
		m_cVCSConfStatus.SetCurVCMode(byVcsAutoMode); 
	}
	pszMsgBuf++;

	//������չ��������ѡ��Ϣ[12/27/2011 chendaiwei]
	m_tConfEx.Clear();
	TConfInfoEx tTempConfEx = GetConfInfoExFromConfAttrb(m_tConf);
	
	if(m_tConf.HasConfExInfo())
	{
		u16 wConfExLength = 0;
		BOOL32 bExistUnknowType = FALSE;
		UnPackConfInfoEx(m_tConfEx,(u8*)pszMsgBuf,wConfExLength,bExistUnknowType);
		TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];
		u8   byAudioCapNum = m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
		if (byAudioCapNum > 0)//������չ��Ϣ������Ƶ������Ϣ����Ҫ����չ��Ϣ��ȥ����
		{
			tTempConfEx.SetAudioTypeDesc(atAudioTypeDesc,byAudioCapNum);
		}
		m_tConfEx.AddCapExInfo(tTempConfEx);
	}
	else
	{
		m_tConfEx = tTempConfEx;
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

	if(tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264
		&& tCapSupport.GetDStreamMediaType() == MEDIA_TYPE_H264)
	{
		TDStreamCap tTempDsCap = tCapSupport.GetDStreamCapSet();
		
#ifdef _8KH_	
		tTempDsCap.SetH264ProfileAttrb(tCapSupport.GetMainStreamProfileAttrb());
#elif defined _8KE_
		//3. 8KG˫��ʼ��ΪBP(�˴�����)
		tTempDsCap.SetH264ProfileAttrb(emBpAttrb);
#else      
		//4. 8000A���������� 
		tTempDsCap.SetH264ProfileAttrb(tCapSupport.GetMainStreamProfileAttrb());
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

	//RestoreVCConf(byVcsAutoMode);

	g_cMcuVcApp.UpdateAgentAuthMtNum();

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
        ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsVCSEqpInfoCheckPass] tConfInfo.IsNull, ignore it\n" );
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
            ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsVCSEqpInfoCheckPass]Conference %s create failed because of multicast address occupied!\n", m_tConf.GetConfName() );
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
            ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsVCSEqpInfoCheckPass]Conference %s create failed because of distributed conf multicast address occupied!\n", m_tConf.GetConfName() );
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
            ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsVCSEqpInfoCheckPass]Conference %s create failed because of no DCS registered!\n", m_tConf.GetConfName() );
            return FALSE;
        }
    }

	//������VCS���飬Ĭ��ģ������Ϊ��������������ʱ������Ԥ����������ʱ������Ҫ����
	//���ڻ���ϳ���������ǽ����������ԴЧ��Ԥ��

	//��Ҫ�����ش��Ļ���,Ԥ�����ش�ͨ�� 
	if (tConfAttrib.IsResendLosePack() && !AssignPrsChnnl4Conf())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsVCSEqpInfoCheckPass] not enough prs chanl\n");
	}

    // guzh [8/29/2007] ˫ý�������������,��֧�ֵ����鲥
#ifdef _MINIMCU_
    if ( ISTRUE(m_byIsDoubleMediaConf) )                
    {
        if ( m_tConf.GetConfAttrb().IsMulticastMode() && 
             m_tConf.GetConfAttrb().IsMulcastLowStream() )
        {
            wErrCode = ERR_MCU_MCLOWNOSUPPORT;
            ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Conf.%s not support multicast low stream\n", m_tConf.GetConfName() );
            return FALSE;                
        }
    }
#endif

    //���鿪ʼ�Զ�¼��
	//  [12/27/2009 pengjie] Modify �����޸Ĳ��ܸ��춨ʱ¼�����⣬���Ѹò��ִ�����ȡ�����ӿ�
	if( !SetAutoRec(wErrCode) )
	{
		return FALSE;
	}
	// End Modify

	//�ѻ���ָ��浽������
	if( !g_cMcuVcApp.AddConf( this ) )
	{
		wErrCode = ERR_MCU_CONFOVERFLOW;
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Conference %s create failed because of full instance!\n", m_tConf.GetConfName() );
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
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSCallMT] Call MT(MCUID:%d MTID:%d) whose mcu is not in conf\n",
			           tMt.GetMcuId(), tMt.GetMtId());
		//�ն˲��ڿɵ��ȵ���Դ��Χ��
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINVC );
		cServMsg.SetEventId( cServMsg.GetEventId() + 2 );
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}

	if( tMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{	
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSCallMT] Call MT(MCUID:%d MTID:%d) has in conf\n",
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
        ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSCallMT] GetDialAlias failed, byMtId.%d\n", tMt.GetMtId());
        return;
	}

	cServMsg.SetEventId( cServMsg.GetEventId() + 1 );	
	SendReplyBack(cServMsg, cServMsg.GetEventId());
	

	//zhouyiliang20100702 ȡ��debuginfo�ļ������õĳ�ʱʱ��,��λ��s��������ʱx1000
	u8 byVcMtOverTime = g_cMcuVcApp.GetVcsMtOverTimeInterval();

	if (VCS_FORCECALL_CMD == byCallType)
	{
		// ��ռ���ն˵������ʱ����Ϊ�����ն˵����ʱ����+5s
		SetTimer( MCUVC_VCMTOVERTIMER_TIMER, (byVcMtOverTime + 5)*1000  );
	}
	else
	{
		// ���õ����ն˵����ʱ���������ʱ����ն�δ��������ʾ�����ն˳�ʱ
		//zjj 20090908 ��Ϊ10��===��zhouyiliang20100702��Ϊ��debuginfo�ļ���ȡ��
		SetTimer( MCUVC_VCMTOVERTIMER_TIMER, byVcMtOverTime*1000 );
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
		
		TMsgHeadMsg tHeadMsg;
		//u8 byFstMcuId = 0,bySecMcuId = 0;
		u8 abyMcuId[MAX_CASCADEDEPTH - 1];
		memset( &abyMcuId[0],0,sizeof( abyMcuId ) );
		memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
		tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		cServMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
		SendMsgToMt( (u8)tMt.GetMcuId(), MCU_MCU_REINVITEMT_REQ, cServMsg );
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
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSDropMT] Del MT(MCUID:%d MTID:%d) whose mcu is not in conf\n",
			            tMt.GetMcuId(), tMt.GetMtId());
		return;
	}
	if(!tMtInfo.MtJoinedConf(tMt.GetMtId()))
	{	
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSDropMT] Del MT(MCUID:%d MTID:%d) has not in conf\n",
			            tMt.GetMcuId(), tMt.GetMtId());
		return;
	}

	// [8/12/2010 xliang] ��ϯ���ܱ��Ҷϱ���
	if(m_tConf.GetChairman() == tMt)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSDropMT] chairman(%d, %d) shouldn't be dropped!\n");
		return;
	}
	
	ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "VCSDropMT(%d,%d)\n", tMt.GetMcuId(), tMt.GetMtId());

    TMt tVCMT = m_cVCSConfStatus.GetCurVCMT();
	TMt tNullMT;
	tNullMT.SetNull();
	TMt tChairMan = m_tConf.GetChairman();
	if (tMt == GetLocalMtFromOtherMcuMt(m_cVCSConfStatus.GetCurVCMT()))
	{
		m_cVCSConfStatus.SetCurVCMT(tNullMT);
//		ChangeConfRestoreMixParam();
		if (VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
			!m_tConf.GetChairman().IsNull())
		{			
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
	else
	{
		//yrl20131120ɾ������ǽ�е������ն˻����ϯѡ���Ľ������
		if( VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
			tMt == m_cVCSConfStatus.GetCurVCMT() &&
			!tChairMan.IsNull() )
		{
			StopSelectSrc( tChairMan,MODE_BOTH,FALSE,TRUE );
		}					
	}

	if (tMt == GetLocalMtFromOtherMcuMt(m_cVCSConfStatus.GetReqVCMT()))
	{
		m_cVCSConfStatus.SetReqVCMT(tNullMT);
	}
	if ((tMt.IsLocal() && m_ptMtTable->IsMtInMixing(tMt.GetMtId())) || 
		(!tMt.IsLocal() && m_ptConfOtherMcTable->IsMtInMixing(tMt)))
    {
		RemoveSpecMixMember(&tMt, 1, FALSE,TRUE);
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

		u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
		memset( &abyMcuId[0],0,sizeof(abyMcuId) );
		TMsgHeadMsg tHeadMsg;
		TMt tDstMt;
		memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
		tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tDstMt );
			
		//u8 byMcuId = GetFstMcuIdFromMcuIdx(tMt.GetMcuId());
		cServMsg.SetEventId(MCU_MCU_DROPMT_REQ);
		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody((u8 *)&tDstMt, sizeof(TMt));
		cServMsg.CatMsgBody((u8 *)&tHeadMsg, sizeof(TMsgHeadMsg));
		SendMsgToMt( (u8)tDstMt.GetMcuId(), MCU_MCU_DROPMT_REQ, cServMsg );

		TMt tSMCU = m_ptMtTable->GetMt( (u8)tDstMt.GetMcuId() );//tMt.GetMcuId());
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

		//zbq [05/13/2010] ����ȴ�������У�FIXME: �缶��ʶ��
		/*OL32 bIsSendToChairman = FALSE;
        if (m_tApplySpeakQue.IsMtInQueue(tMt) )
        {
            m_tApplySpeakQue.ProcQueueInfo(tMt, bIsSendToChairman, FALSE);
            NotifyMcsApplyList( bIsSendToChairman );
        }*/
		return;
	}	

	TMt tLocalMt = tMt;
	//�Ҷϱ����ն�ǰ���Ƚ��ն˻ָ��ɷǾ�����״̬
	//��Ϊ�����նˣ���Ҫ�ָ��ֱ���
	if (tMt.IsLocal())
	{
		VCSMTMute(tMt, FALSE, VCS_AUDPROC_MUTE);
		VCSMTMute(tMt, FALSE, VCS_AUDPROC_SILENCE);
	}
	else
	{
		tLocalMt = GetLocalMtFromOtherMcuMt( tMt );
	}

	TLogicalChannel tSrcRvsChannl;

	if ( m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
	{
		if ( tSrcRvsChannl.GetChannelType() == MEDIA_TYPE_H264 &&
			( IsVidFormatHD( tSrcRvsChannl.GetVideoFormat() ) ||
			  tSrcRvsChannl.GetVideoFormat() == VIDEO_FORMAT_4CIF ) )
		{
			ChangeVFormat(tMt/*, tSrcRvsChannl.GetVideoFormat()*/);
		}
	}

	RemoveJoinedMt( tMt, TRUE );

	//zjj20100330 
	TMtStatus tMtStatus;
	if( m_ptMtTable->GetMtStatus(tMt.GetMtId(),&tMtStatus) )
	{
		tMtStatus.SetMtDisconnectReason( MTLEFT_REASON_NORMAL );
		m_ptMtTable->SetMtStatus( tMt.GetMtId(),&tMtStatus );
	}
	
	//zjj20100325 ������ֶ��Ҷ�һ���ն˶ˣ�����ģʽ��Ϊ�ֶ�
	if( ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode() ) && !(tMt == m_tConf.GetChairman()) &&
		!(tMt == m_cVCSConfStatus.GetVcsBackupChairMan()) )
	{		
		m_ptMtTable->SetCallMode( GetLocalMtFromOtherMcuMt(tMt).GetMtId(),CONF_CALLMODE_NONE );
	}

    if( tVCMT == tMt )
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
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[VCSDropMT] Fail to get Queue Head!\n" );
            }
        }
        else
        {	
            tSpeakMt.SetNull();
            ChgCurVCMT( tSpeakMt );				
        }
	}
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
	10/03/23				�ܾ���		  ��ش��޸�
====================================================================*/
BOOL CMcuVcInst::VCSConfSelMT( TSwitchInfo tSwitchInfo,BOOL32 bNeedPreSetin /*= TRUE*/ )
{    
 
    if (CurState() != STATE_ONGOING)
    {
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[VCSConfSelMT] InstID(%d) in wrong state(%d)\n", GetInsID(), CurState());
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
        ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfSelMT] Mt(%d,%d) Select see Mt(%d,%d), returned with nack!\n", 
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
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfSelMT] Mt(%d,%d) select see Mt(%d,%d), selmode(%d)\n", 
			     tDstMt.GetMcuId(), tDstMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSwitchInfo.GetMode() );

		tDstMt = GetLocalMtFromOtherMcuMt( tDstMt );				
		tDstMt = m_ptMtTable->GetMt(tDstMt.GetMtId());
		tSwitchInfo.SetDstMt( tDstMt );	
		
        //Ŀ���ն�δ�����
        if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
        {
            ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfSelMT] Specified Mt(%u,%u) not joined conference!\n", 
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
		//tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);
		//tSwitchInfo.SetSrcMt( tSrcMt );
		//Դ�ն�δ����飬NACK
		if( !m_tConfAllMtInfo.MtJoinedConf( GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId() ) )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfSelMT] Select source Mt(%u,%u) has not joined current conference!\n",
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
    u8 bySrcMtID = GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId();
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
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfSelMT] fail to sel(%d) because of the openning of dual logical channel\n",
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
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfSelMT] Conference %s is mixing now. Cannot switch only audio!\n", 
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
                ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfSelMT] Conference %s is mixing now. Cannot switch audio!\n", 
                         m_tConf.GetConfName() );
//              cServMsg.SetErrorCode( ERR_MCU_SELBOTH_INMIXING );
// 				SendMsgToAllMcs( MCU_VCS_VCMT_NOTIF, cServMsg );
				bySwitchMode = MODE_VIDEO;
            }
			tSwitchInfo.SetMode( bySwitchMode );
		}
	}

	if( bNeedPreSetin && !tSrcMt.IsLocal() )
	{
		if( IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()) )
		{
			TMt tDst;
			tDst.SetNull();
			TPreSetInReq tPreSetInReq;
			tDst.SetNull();
			tPreSetInReq.m_tSpyMtInfo.SetSpyMt(tSrcMt);
			tPreSetInReq.m_tSpyInfo.m_tSpySwitchInfo.m_tDstMt = tSwitchInfo.GetDstMt();
			tPreSetInReq.m_bySpyMode = tSwitchInfo.GetMode();			
			tPreSetInReq.SetEvId(VCS_MCU_CONFSELMT_CMD);
			tPreSetInReq.m_tReleaseMtInfo.SetCount(0);
			
			//zjj20100915 һ����δ�����Ϊ��bugɾ�����Ҳ�����˭�ӵģ�˭��ӡ������
			//vcs��ϯ�����£���غ���ϯѡ�������
			/*TMt tMcSrc;
			u8  byReleaseMode = MODE_NONE;
			for (u8 byLoop = 0; byLoop < MAXNUM_MCU_MC + MAXNUM_MCU_VC; byLoop++)
			{
				for (u8 byChnIdx = 0; byChnIdx < MAXNUM_MC_CHANNL; byChnIdx++)
				{
					tMcSrc.SetNull();
					if(g_cMcuVcApp.GetMcSrc(byLoop, &tMcSrc, byChnIdx, MODE_VIDEO))
					{
						if (!tMcSrc.IsNull() && tMcSrc.GetConfIdx() == m_byConfIdx)
						{
							tPreSetInReq.m_tReleaseMtInfo.m_tMt = tMcSrc;
							tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum++;
							byReleaseMode += MODE_VIDEO;
						}
					}

					if(g_cMcuVcApp.GetMcSrc(byLoop, &tMcSrc, byChnIdx, MODE_AUDIO))
					{
						if (!tMcSrc.IsNull() && tMcSrc.GetConfIdx() == m_byConfIdx)
						{
							tPreSetInReq.m_tReleaseMtInfo.m_tMt = tMcSrc;
							tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum++;
							byReleaseMode += MODE_AUDIO;
						}
					}
					if (byReleaseMode != MODE_NONE)
					{
						tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = byReleaseMode;
						break;
					}	
				}
				if (byReleaseMode != MODE_NONE)
				{
					break;
				}	
			}*/

			// [pengjie 2010/9/13] ��Ŀ�Ķ�����
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()) );
			//zjl20101116 �����ǰ�ն��ѻش���������Ҫ���ѻش�Ŀ��������ȡС
			if(!GetMinSpyDstCapSet(tSrcMt, tSimCapSet))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[VCSConfSelMT] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
					tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
				return FALSE;
			}
			
			tPreSetInReq.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		    // End
		
			if( m_ptMtTable->GetMtStatus( tSwitchInfo.GetDstMt().GetMtId(), &tDstMtStatus ) )
			{
				TMt tSrc;
				TMt tMcSrc;
				tSrc = tDstMtStatus.GetSelectMt( tSwitchInfo.GetMode() );
				if( !tSrc.IsNull() && VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState() )
				{
					tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = tSwitchInfo.GetMode();
					tPreSetInReq.m_tReleaseMtInfo.m_tMt = tSrc;
					if( MODE_BOTH == tSwitchInfo.GetMode() || MODE_VIDEO == tSwitchInfo.GetMode() )
					{
						++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
					}
					if( MODE_BOTH == tSwitchInfo.GetMode() || MODE_AUDIO == tSwitchInfo.GetMode() )
					{
						++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
					}

					u8 byMtInMcNum = GetCanRlsNumInVcsMc(tSrc, MODE_VIDEO);
					tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum += byMtInMcNum;
					
					if(MODE_AUDIO == tSwitchInfo.GetMode())
					{
						tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_BOTH;
						
						byMtInMcNum = GetCanRlsNumInVcsMc(tSrc, MODE_AUDIO);
						tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum += byMtInMcNum;
						
					}
// 					tMcSrc.SetNull();
// 					g_cMcuVcApp.GetMcSrc( m_cVCSConfStatus.GetCurSrcSsnId(), &tMcSrc, VCS_VCMT_MCCHLIDX, MODE_VIDEO );
// 					if( tMcSrc == tSrc && (tSwitchInfo.GetMode() == MODE_BOTH || tSwitchInfo.GetMode() == MODE_VIDEO) )
// 					{
// 						++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
// 						if( MODE_AUDIO == tSwitchInfo.GetMode() )
// 						{
// 							tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_BOTH;
// 						}
// 					}
					//zhouyiliang 20101015 ����˫������������ѡ���ն˻����˻���ϳ�
					if ( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE ) 
					{
						++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
					}					
				}
				else
				{
					if( VCS_POLL_STOP != m_cVCSConfStatus.GetChairPollState() && 
						!m_cVCSConfStatus.GetCurVCMT().IsNull() && 
						m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPCHAIRMAN_MODE )
					{						
						tMcSrc.SetNull();
						g_cMcuVcApp.GetMcSrc( m_cVCSConfStatus.GetCurSrcSsnId(), &tMcSrc, VCS_VCMT_MCCHLIDX, MODE_VIDEO );
						if( tMcSrc == tSrc &&
							( MODE_BOTH == tSwitchInfo.GetMode() || MODE_VIDEO == tSwitchInfo.GetMode() )
							)
						{
							tPreSetInReq.m_tReleaseMtInfo.m_tMt = tSrc;
							tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = tSwitchInfo.GetMode();
							++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;							
						}
						tMcSrc.SetNull();
						g_cMcuVcApp.GetMcSrc( m_cVCSConfStatus.GetCurSrcSsnId(), &tMcSrc, VCS_VCMT_MCCHLIDX, MODE_AUDIO );						
						if( tMcSrc == tSrc &&
							( MODE_BOTH == tSwitchInfo.GetMode() || MODE_AUDIO == tSwitchInfo.GetMode() )
							)
						{
							tPreSetInReq.m_tReleaseMtInfo.m_tMt = tSrc;
							tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = tSwitchInfo.GetMode();
							++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
						}
					}
				}
			}
			
			if (m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_AUTO_MODE
				&& !IsMtNotInOtherHduChnnl(tPreSetInReq.m_tReleaseMtInfo.m_tMt,0,0)
				&& tPreSetInReq.m_tReleaseMtInfo.m_tMt == m_cVCSConfStatus.GetCurVCMT())
			{
				tPreSetInReq.m_tReleaseMtInfo.m_tMt.SetNull();
				tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 0;
				tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 0;
				//byReleaseMode = MODE_NONE;
			}

			OnMMcuPreSetIn( tPreSetInReq );
			return TRUE;
		}
	}else
	{
		// [11/23/2010 liuxu] ֪ͨ�¼���ʼ������
		if (!tSrcMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()))
		{
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));
			SendMMcuSpyNotify( tSrcMt, VCS_MCU_CONFSELMT_CMD, tSimCapSet);
		}
	}

	if ( VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState() &&
		tSwitchInfo.GetDstMt().GetMtId() == m_tConf.GetChairman().GetMtId() &&
		( tSwitchInfo.GetMode() == MODE_BOTH || tSwitchInfo.GetMode() == MODE_VIDEO )
		)
	{
		VCSConfStopMTSel( m_tConf.GetChairman(), tSwitchInfo.GetMode(),FALSE );
	}
	
	BOOL32 bBasEnough   = TRUE;
	BOOL32 bNeedVidAdpt = FALSE;
	BOOL32 bNeedAudAdpt = FALSE;
	
	// [9/26/2011 liuxu] ʵ�ʽ�����ʱ��tRealSwitchInfo����tSwitchInfo
	// �Ա��޸�bySwitchMode, �����Ͳ�����ΪtSwitchInfo�е�ģʽʱBoth
	// �����ֽ�����������
	TSwitchInfo tRealSwitchInfo = tSwitchInfo;

	if (bySwitchMode == MODE_VIDEO || bySwitchMode == MODE_BOTH)
	{
		tRealSwitchInfo.SetMode(MODE_VIDEO);
		bNeedVidAdpt = IsNeedSelAdpt(tOrigSrcMt, tDstMt, MODE_VIDEO);
		if(bNeedVidAdpt)
		{
		   if(!StartSelAdapt(tOrigSrcMt, tDstMt, MODE_VIDEO))
		   {
			  bBasEnough = FALSE;
			  ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[VCSConfSelMT] mt(mcuid:%d, mtid:%d) sel(mode:%d) mt(mcuid:%d, mtid:%d) using adp Failed!\n",
			   tDstMt.GetMcuId(), tDstMt.GetMtId(), bySwitchMode, tOrigSrcMt.GetMcuId(), tOrigSrcMt.GetMtId());
			  StopSwitchToSubMt(1,&tDstMt,MODE_VIDEO);
		   }
		}
		else
		{
			VCSSwitchSrcToDst( tRealSwitchInfo );
		}
	}

	if (bySwitchMode == MODE_AUDIO || bySwitchMode == MODE_BOTH)
	{
		tRealSwitchInfo.SetMode(MODE_AUDIO);
		bNeedAudAdpt = IsNeedSelAdpt(tOrigSrcMt, tDstMt, MODE_AUDIO);
		if(bNeedAudAdpt)
		{
			if(!StartSelAdapt(tOrigSrcMt, tDstMt, MODE_AUDIO))
			{
				bBasEnough = FALSE;
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[VCSConfSelMT] mt(mcuid:%d, mtid:%d) sel(mode:%d) mt(mcuid:%d, mtid:%d) using Audadp Failed!\n",
					tDstMt.GetMcuId(), tDstMt.GetMtId(), bySwitchMode, tOrigSrcMt.GetMcuId(), tOrigSrcMt.GetMtId());
				StopSwitchToSubMt(1,&tDstMt,MODE_AUDIO);
			}
		}
		else
		{
			VCSSwitchSrcToDst( tRealSwitchInfo );
		}
	}

	if(!bBasEnough)
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
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[VCSConfSelMT]mt(mcuid:%d, mtid:%d) sel mt(mcuid:%d, mtid:%d) need adp, but fail to start adp\n",
			    tDstMt.GetMcuId(), tDstMt.GetMtId(), tOrigSrcMt.GetMcuId(), tOrigSrcMt.GetMtId());
		return FALSE;
	}

    //����ѡ��������ѡ����setin
    if ( !tOrigSrcMt.IsLocal() && !IsLocalAndSMcuSupMultSpy(tOrigSrcMt.GetMcuId()) )
    {   
        OnMMcuSetIn( tOrigSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
    }

    // ����ѡ������ѡ����setout
    if( tOrigDstMt.GetMtType() == MT_TYPE_SMCU || tOrigDstMt.GetMtType() == MT_TYPE_MMCU )
    {
		u16 wMcuIdx = GetMcuIdxFromMcuId( tDstMt.GetMtId() );
        TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);//tDstMt.GetMtId());
        if(ptConfMcInfo != NULL)
        {
            ptConfMcInfo->m_tSpyMt = tSrcMt;
            //send output ��Ϣ
            TSetOutParam tOutParam;
			TMsgHeadMsg tHeadMsg;
			memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
            tOutParam.m_nMtCount = 1;
            tOutParam.m_atConfViewOutInfo[0].m_tMt = tDstMt;
            tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
            tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;
            CServMsg cServMsg2;
			//tHeadMsg.m_tMsgSrc.m_tMt = tDstMt;
			cServMsg2.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
			cServMsg2.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
            cServMsg2.SetEventId(MCU_MCU_SETOUT_NOTIF);
            SendMsgToMt(tDstMt.GetMtId(), MCU_MCU_SETOUT_NOTIF, cServMsg2);
        }
    }

	TMt tOldSelSrc;// ��þ���Ƶѡ��Դ�����ڵ��ֱ���
    // ����ѡ���ն�(��VCS���ն�)��״̬
    if( MODE_AUDIO == tSwitchInfo.GetMode() || 
        MODE_VIDEO == tSwitchInfo.GetMode() ||
        MODE_BOTH  == tSwitchInfo.GetMode() )
    {
        m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tDstMtStatus ); // ѡ��
		if (MODE_VIDEO == tSwitchInfo.GetMode() || MODE_BOTH  == tSwitchInfo.GetMode())
		{
			tOldSelSrc = tDstMtStatus.GetSelectMt(MODE_VIDEO);
		}
        tDstMtStatus.SetSelectMt( tOrigSrcMt, tSwitchInfo.GetMode() );
        m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );
    }

	// �����¾�ѡ��Դ�ն˷ֱ���
	TVMPParam_25Mem  tVMPParam   = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	if (tVMPParam.GetVMPMode() != CONF_VMPMODE_NONE &&
		(MODE_VIDEO == tSwitchInfo.GetMode() || MODE_BOTH  == tSwitchInfo.GetMode()))
	{
		// �о�ѡ��Դ
		if (ChgMtVidFormatRequired(tOldSelSrc))
		{
			ChangeMtVideoFormat(tOldSelSrc, FALSE);
		}
		if (ChgMtVidFormatRequired(tOrigSrcMt))
		{
			ChangeMtVideoFormat(tOrigSrcMt, FALSE);
		}
	}

    //�ж�ѡ�����Ƿ��������
	u16 wBitrate = 0;
	TLogicalChannel tLogicalChannel;
    if (!bNeedVidAdpt && IsNeedAdjustMtSndBitrate(GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId(), wBitrate) &&
		m_ptMtTable->GetMtLogicChnnl(GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE) &&
		wBitrate < tLogicalChannel.GetFlowControl())
    {
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[VCSConfSelMT] mt.%d, mode %d, old flowcontrol :%d, min flowcontrol :%d\n",
                GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId(), 
                tLogicalChannel.GetMediaType(), tLogicalChannel.GetFlowControl(), wBitrate);                   
        
        tLogicalChannel.SetFlowControl(wBitrate);
        cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
		SendMsgToMt(GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
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
void CMcuVcInst::VCSConfStopMTSel( TMt tDstMt, u8 byMode,BOOL32 bIsRestoreSeeBrd /*= TRUE*/ )
{	
    //change mode if mixing
    if ( MODE_BOTH == byMode )
    {
        if( m_tConf.m_tStatus.IsMixing() )
        {
            byMode = MODE_VIDEO;
        }
    }
	
    ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfStopMTSel] Mcu%dMt%d Cancel Select See.bIsRestoreSeeBrd.%d\n", tDstMt.GetMcuId(), tDstMt.GetMtId(),bIsRestoreSeeBrd );
    
    // ѡ����Ŀ��������¼��ն�
    tDstMt = GetLocalMtFromOtherMcuMt(tDstMt);

	if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfStopMTSel] Dst Mt%u-%u not joined conference!\n", 
			             tDstMt.GetMcuId(), tDstMt.GetMtId() );
		return;
	}

	// ���ѡ��״̬
	TMtStatus tMtStatus; 
	m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );

	TMt tVidSrc = tMtStatus.GetSelectMt( MODE_VIDEO );
	TMt tAudSrc = tMtStatus.GetSelectMt( MODE_AUDIO );

	if( tVidSrc.IsNull() && MODE_VIDEO == byMode )
	{
		return;
	}
	if( tAudSrc.IsNull() && MODE_AUDIO == byMode )
	{
		return;
	}
	if( tVidSrc.IsNull() && tAudSrc.IsNull() && MODE_BOTH == byMode )
	{
		return;
	}

	if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
	{
		tMtStatus.RemoveSelByMcsMode( MODE_VIDEO );

		// ���µ���ѡ���ߵķ�������, Ŀǰֻ֧�ֵ�������ѡ���ߵ�����		
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
		if( !tVidSrc.IsLocal() && IsLocalAndSMcuSupMultSpy(tVidSrc.GetMcuId()))
		{
			FreeRecvSpy( tVidSrc,MODE_VIDEO );
		}
		
	}

	if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
	{
		tMtStatus.RemoveSelByMcsMode( MODE_AUDIO );

		// ���µ���ѡ���ߵķ�������, Ŀǰֻ֧�ֵ�������ѡ���ߵ�����
		
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
		if( !tAudSrc.IsLocal() && IsLocalAndSMcuSupMultSpy(tAudSrc.GetMcuId()))
		{
			FreeRecvSpy( tAudSrc,MODE_AUDIO );
		}
	}
	BOOL32 byIsCanRestoreBrdSrc = FALSE;
	if( bIsRestoreSeeBrd )
	{
		byIsCanRestoreBrdSrc = TRUE;
		//zjj20091026 vcs������ϯ��ѯʱ,���ָܻ��㲥ԭ����ϯ�Ľ���
		if( m_tConf.GetConfSource() == VCS_CONF &&		 
			m_tConf.GetChairman() == tDstMt && 
			ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() )
			)
		{		
			if( m_cVCSConfStatus.GetChairPollState() == VCS_POLL_START ||
				m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE
				)
			{		
				if( MODE_BOTH == byMode )
				{
					byMode = MODE_AUDIO;
				}
				if( MODE_VIDEO == byMode)
				{
					byIsCanRestoreBrdSrc = FALSE;
				}
			}
			
		}
	}
	// ���ʽѡ���ͷŶ�Ӧ��������Դ
	ReleaseResbySel(tDstMt, byMode,byIsCanRestoreBrdSrc);
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[VCSConfStopMTSel] Mcu%dMt%d Cancel Select See.byIsCanRestoreBrdSrc.%d\n", tDstMt.GetMcuId(), tDstMt.GetMtId(),byIsCanRestoreBrdSrc );
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
	// �������ն˷ֱ���
	if ((MODE_VIDEO == byMode || MODE_BOTH == byMode)
		&& ChgMtVidFormatRequired(tVidSrc))
	{
		TVMPParam_25Mem	tVMPParam  = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp); 
		ChangeMtVideoFormat(tVidSrc, FALSE); 
	}
 
	if (bIsRestoreSeeBrd)
	{
		if( byIsCanRestoreBrdSrc  )
		{
			// ����ѡ��������ƵԴ
			RestoreRcvMediaBrdSrc( tDstMt.GetMtId(), byMode );
		}	
		else 
		{
			if( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE &&
				m_cVCSConfStatus.GetChairPollState() != VCS_POLL_START )
			{
				//zjl 20110510 StopSwitchToSubMt �ӿ������滻
				//StopSwitchToSubMt( tDstMt.GetMtId(),byMode );
				StopSwitchToSubMt(1, &tDstMt, byMode);
			}
		}
	}
	

	// ����֪ͨ��ѡ���ն˵�״̬�仯
    MtStatusChange( &tDstMt, TRUE );

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
	ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfStopMTSeled]Stop all mt sel spec mt(mcuid:%d, mtid:%d)\n", 
		    tSrcMt.GetMcuId(), tSrcMt.GetMtId());

	// ��������ѡ�����ն˵�ʵ�壬�𽻻��������ѡ��״̬��֪ͨ���Լ�
	TMt       tMt;
	TMtStatus tMtStatus;
    TMt       tNullMt;
	tNullMt.SetNull();

	tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);

	//��������Ƶѡ��
	TMt atStopVideoDstMt[MAXNUM_CONF_MT];
	u8 byStopVideoMtNum = 0;
	//��������Ƶѡ��
	TMt atStopAudioDstMt[MAXNUM_CONF_MT];
	u8 byStopAudioMtNum = 0;

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
						//zjl 20110510 StopSwitchToSubMt �ӿ������滻
						//StopSwitchToSubMt( tMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE);
						atStopVideoDstMt[byStopVideoMtNum] = tMt;
						byStopVideoMtNum ++;

						tMtStatus.SetSelectMt(tNullMt, MODE_VIDEO);
						m_ptMtTable->SetMtStatus(byMtIdxLp, &tMtStatus);

						if (IsNeedAdapt(tSelMt, m_ptMtTable->GetMt(byMtIdxLp), MODE_VIDEO)/*IsNeedSelApt(tSelMt.GetMtId(), byMtIdxLp, MODE_VIDEO)*/)
						{
							StopSelAdapt(tSelMt, m_ptMtTable->GetMt(byMtIdxLp), MODE_VIDEO);
							//StopHdVidSelAdp(tSrcMt, m_ptMtTable->GetMt(byMtIdxLp), MODE_VIDEO);
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
						
						//zjl 20110510 StopSwitchToSubMt �ӿ������滻
						//StopSwitchToSubMt( tMt.GetMtId(), MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);
						atStopAudioDstMt[byStopAudioMtNum] = tMt;
						byStopAudioMtNum ++;

						tMtStatus.SetSelectMt(tNullMt, MODE_VIDEO);
						m_ptMtTable->SetMtStatus(byMtIdxLp, &tMtStatus);
						if (IsNeedAdapt(tSrcMt, m_ptMtTable->GetMt(byMtIdxLp), MODE_AUDIO)/*IsNeedSelApt(tSelMt.GetMtId(), byMtIdxLp, MODE_AUDIO)*/)
						{
							StopSelAdapt(tSrcMt, m_ptMtTable->GetMt(byMtIdxLp), MODE_AUDIO);
							//StopAudSelAdp(tSrcMt, m_ptMtTable->GetMt(byMtIdxLp));
						}
					}
				}

				if (bySel)
				{
					TMt tTempMt = m_ptMtTable->GetMt(byMtIdxLp);
				    MtStatusChange( &tTempMt, TRUE );
				}


			}
		}
	}	

	if (byStopVideoMtNum > 0)
	{
		StopSwitchToSubMt(byStopVideoMtNum, atStopVideoDstMt, MODE_VIDEO, FALSE);
	}

	if (byStopAudioMtNum > 0)
	{
		StopSwitchToSubMt(byStopAudioMtNum, atStopAudioDstMt, MODE_AUDIO, FALSE);
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
	TMt	tDstMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetDstMt());
	TMt tSrcMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetSrcMt());
	u8	byMode = tSwitchInfo.GetMode();

    // ֪ͨ�������Ͷ˷�������      
    NotifyMtSend( tSrcMt.GetMtId(), byMode );

	TMt tUnlocalSrcMt = tSwitchInfo.GetSrcMt();
	TMt tUnlocalDstMt = tSwitchInfo.GetDstMt();
	CRecvSpy tSpyResource;
	u16 wSpyPort = SPY_CHANNL_NULL;	
	if( m_cSMcuSpyMana.GetRecvSpy( tUnlocalSrcMt, tSpyResource ) )
	{
		wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
	}
	//zjj 20091025ѡ��������
    //g_cMpManager.SetSwitchBridge(tSrcMt, 0, byMode);

	if (TYPE_MT == tDstMt.GetType())
	{
		//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
		//StartSwitchToSubMt(tUnlocalSrcMt, 0, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE, FALSE,FALSE,wSpyPort);
		TSwitchGrp tSwitchGrp;
		tSwitchGrp.SetSrcChnl(0);
		tSwitchGrp.SetDstMtNum(1);
		tSwitchGrp.SetDstMt(&tDstMt);
		StartSwitchToAll(tUnlocalSrcMt, 1, &tSwitchGrp, byMode, SWITCH_MODE_SELECT, TRUE, FALSE, wSpyPort);

		TMtStatus tMtStatus;
		if( m_ptMtTable->GetMtStatus( tDstMt.GetMtId(),&tMtStatus ) )
		{
			tMtStatus.SetSelectMt( tUnlocalSrcMt,byMode );
			m_ptMtTable->SetMtStatus( tDstMt.GetMtId(),&tMtStatus );
		}
		
	}
	else if (TYPE_MCUPERI == tDstMt.GetType())
	{
		StartSwitchToPeriEqp(tUnlocalSrcMt, 0, tDstMt.GetEqpId(), tSwitchInfo.GetDstChlIdx(), byMode, SWITCH_MODE_SELECT);
	}
	
	
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (tConfAttrb.IsResendLosePack() && !(tUnlocalSrcMt == tUnlocalDstMt))
	{
		//��Ŀ�ĵ�Rtcp������Դ
        u8 bySrcChnnl = 0;
        if (TYPE_MT == tDstMt.GetType())
        {
            bySrcChnnl = 0;
        }
        else if (TYPE_MCUPERI == tDstMt.GetType())
        {
            bySrcChnnl = tSwitchInfo.GetDstChlIdx();
        }
        BuildRtcpSwitchForSrcToDst(tUnlocalDstMt, tUnlocalSrcMt, byMode, bySrcChnnl, TRUE);
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
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfMTToTW] InstID(%d) in wrong state(%d)\n", GetInsID(), CurState());
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
	TMt tOrigMt = tTWSwitchInfo.GetSrcMt();

	u8 byDstId = tTWSwitchInfo.GetDstMt().GetEqpId();

	// ����Դ�ն�Ϊ�¼��նˣ�֪ͨ�¼�MCUѡ����ն�
	if (!tOrigMt.IsLocal() && !IsLocalAndSMcuSupMultSpy(tOrigMt.GetMcuIdx()))
    {		
        OnMMcuSetIn(tOrigMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
    }

		
    switch(tTWSwitchInfo.GetMemberType()) 
    {
	case TW_MEMBERTYPE_VCSAUTOSPEC:
	case TW_MEMBERTYPE_VCSSPEC:
		{
			//Դ�ն˱����Ѿ��������
			if (FALSE == m_tConfAllMtInfo.MtJoinedConf(tSrcMT.GetMtId()))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfMTToTW] select source MT%u-%u has not joined current conference! Error!\n",
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
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfMTToTW] select source MT%u-%u has not ability to send video!\n",
						tSrcMT.GetMcuId(), tSrcMT.GetMtId());
// 				cServMsg.SetErrorCode(ERR_MCU_SRCISRECVONLY);
// 				SendMsgToAllMcs(MCU_VCS_VCMT_NOTIF, cServMsg);
				return FALSE;
			}	
		}
        break;
    default:
        ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfMTToTW] Wrong TvWall Member Type!\n");
		return FALSE;
	}

	// VCS���ն˽������ǽ
	ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfMTToTW] tOrigMt(%d,%d)!\n",tOrigMt.GetMcuId(),tOrigMt.GetMtId() );
	tOrigMt.SetMtType(MT_TYPE_MT);

	// [8/19/2011 liuxu] ��ֹ�ն˽������ǽʧ��,ȴ�����˻���
	BOOL32 bStartTvwOk = TRUE;
	if (byDstId >= TVWALLID_MIN && byDstId <= TVWALLID_MAX )
	{
	    ChangeTvWallSwitch(&tOrigMt,
                            tTWSwitchInfo.GetDstMt().GetEqpId(),
                            tTWSwitchInfo.GetDstChlIdx(),
                            tTWSwitchInfo.GetMemberType(), 
							TW_STATE_START);
	}
	else if (byDstId >= HDUID_MIN && byDstId <= HDUID_MAX)
	{
		// [2013/03/11 chenbing] VCS���鲻֧��HDU�໭��,��ͨ����0
		bStartTvwOk = ChangeHduSwitch(&tOrigMt, byDstId, tTWSwitchInfo.GetDstChlIdx(), 0,
                         tTWSwitchInfo.GetMemberType(), TW_STATE_START);
	}
	else
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfMTToTW] wrong dest id(%d)\n", byDstId);
		return FALSE;
	}

	return bStartTvwOk;
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
			tMt = m_ptMtTable->GetMt(GetFstMcuIdFromMcuIdx(tMt.GetMcuId()));
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
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] mt(mcuid:%d, mtid:%d) go on sel chairman\n",
			   tMt.GetMcuId(), tMt.GetMtId());
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] chairman go on sel mt(mcuid:%d, mtid:%d)\n",
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
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] No sel step for subconf\n");
		return;
	}

	// ���ģʽ�²�������ϯ��ѡ����ѡ������
	if (ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) || 
		m_cVCSConfStatus.GetCurVCMode() == VCS_MULVMP_MODE )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] No sel step in this mode(%d)\n", m_cVCSConfStatus.GetCurVCMode());
		return;
	}

	TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();
	if( bySelDirect )
	{
		if( !m_cVCSConfStatus.GetReqVCMT().IsNull()  )
		{
			if( !(tMt==m_cVCSConfStatus.GetReqVCMT()) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] tmt(%d) is not GetReqVCMT(%d)\n",tMt.GetMtId(),m_cVCSConfStatus.GetReqVCMT().GetMtId() );
				return;
			}			
		}
		else 
		{
			if( !tCurVCMT.IsNull() )
			{		
				if( !(tMt==tCurVCMT) )
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] tmt(%d) is not GetCurVCMT(%d)\n",tMt.GetMtId(),tCurVCMT.GetMtId() );
					return;	
				}							
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] tmt(%d) is not GetReqVCMT(%d) and GetCurVCMT(%d)\n",tMt.GetMtId(),m_cVCSConfStatus.GetReqVCMT().GetMtId(),tCurVCMT.GetMtId() );
				return;	
			}
		}
	}
	
	
	// ��ǰ�����ն˻�ǰ�����ն����ڵ�mcu�ſɽ�����ϯѡ������ϯѡ��,�ų�����
	// ����ʱ�����⣬����ѡ����ϯ, �ſ�Ҫ��, ֻ��ǹ̶���Դ����
	if ((!tCurVCMT.IsNull() && 
		(tCurVCMT == tMt || tMt.GetMtId() == tCurVCMT.GetMcuId())) ||
		(TRUE == bySelDirect /*&&	 CONF_CALLMODE_NONE == VCSGetCallMode(tMt)*/))
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
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] uncorrect sel mediatype(%d) in single mode\n", byMediaType);
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
					u8 bySrcId = (tSwitchInfo.GetSrcMt().IsLocal() ? tSwitchInfo.GetSrcMt().GetMtId() : GetFstMcuIdFromMcuIdx(tSwitchInfo.GetSrcMt().GetMcuId()));
					u8 byDstId = (tSwitchInfo.GetDstMt().IsLocal() ? tSwitchInfo.GetDstMt().GetMtId() : GetFstMcuIdFromMcuIdx(tSwitchInfo.GetDstMt().GetMcuId()));
					TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(bySrcId);
					TSimCapSet tDstCap = m_ptMtTable->GetDstSCS(byDstId);
					CBasChn *pcBasChn=NULL;
					TBasChnCapData tBasChnCapData;
					if (IsNeedSelApt(bySrcId, byDstId, MODE_VIDEO) && 					
						!g_cMcuVcApp.GetIdleBasChn(tSrcCap, tDstCap, MODE_VIDEO,&pcBasChn,&tBasChnCapData))
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[GoOnSelStep]not enough bas for all mt in twmode, so stop mt sel chairman\n");
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

		default:
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[GoOnSelStep] wrong vc mode(%d)\n", m_cVCSConfStatus.GetCurVCMode());
			return;

		}
		//[nizhijun 2011/01/13] ѡ��ʧ�ܣ��������ϯѡ���Ļ�������ϯ���Լ�
		if (!VCSConfSelMT(tSwitchInfo) && !bySelDirect)
		{
			NotifyMtReceive(m_tConf.GetChairman(), m_tConf.GetChairman().GetMtId());
		}
	}
}

/*====================================================================
    ������      ��FindSmcuMtInTvWallAndStop
    ����        �������¼��ն�ʱҪ��ͬ���������ն��޳�����ǽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt tNewCurVCMT �µ��ȵ��ն� 
				  TMt *	ptDropMt �޳�������ǽ���ն�
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/09/14                �ܾ���        ����
====================================================================*/
void CMcuVcInst::FindSmcuMtInTvWallAndStop(const TMt& tNewCurVCMT, TMt *const ptDropMt, BOOL32 bIsStopSwitch /* = TRUE*/ )
{
	// �����ն�Ϊ��,��������
	if( tNewCurVCMT.IsNull() )
	{
		return;
	}

	// [5/30/2011 liuxu] ֻ�е��ش�ʱ�Ż������´���
	if(IsLocalAndSMcuSupMultSpy(tNewCurVCMT.GetMcuId()))
	{
		return;
	}

	if (ptDropMt != NULL)
	{
		ptDropMt->SetNull();
	}

	// [5/30/2011 liuxu] ��HDU�͵���ǽ����ͳһ����
	// һ������֧�ֵĵ���ǽͨ��������
	const u8 byMaxTvwChnnlNum = MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL + MAXNUM_PERIEQP_CHNNL * MAXNUM_MAP;
	CConfTvwChnnl acTvwChnnlFind[byMaxTvwChnnlNum];					// ��Ѱ�ҵĵ���ǽͨ��
	u8		abyTvwIdFind[byMaxTvwChnnlNum];							// �洢�ҵ��ĵ���ǽ�豸��ID
	memset(abyTvwIdFind, 0, sizeof(abyTvwIdFind));
	
	const u8 byFindNum = FindAllTvwChnnlByMcu(tNewCurVCMT, acTvwChnnlFind, byMaxTvwChnnlNum);

	u8 byTvwEqpId, byTvwChnnlIdx;								// �ҵ��ĵ���ǽ�豸�ź�ͨ����
	TTvwMember tTvwMember;										// ����ǽͨ����Ա

	// ���������ҵ��ĵ���ǽͨ�����д���
	for (u8 byLoop = 0; byLoop < byFindNum; ++byLoop)
	{
		if (acTvwChnnlFind[byLoop].IsNull())
		{
			continue;
		}
		
		byTvwEqpId = acTvwChnnlFind[byLoop].GetEqpId();
		byTvwChnnlIdx = acTvwChnnlFind[byLoop].GetChnnlIdx();
		tTvwMember = acTvwChnnlFind[byLoop].GetMember();
		
		// �ն�����ʱ�Ĵ���
		if (g_cMcuVcApp.IsPeriEqpConnected(byTvwEqpId))
		{
			// ���ش�ʱ����һ���¼��ն˽���������ǽͨ��
			if ((TMt)tTvwMember == tNewCurVCMT)
			{
				continue;	
			}

			// ͨ����Ա������TW_MEMBERTYPE_VCSAUTOSPEC��TW_MEMBERTYPE_VCSSPEC
			if (!IsVcsTvwMemberType(acTvwChnnlFind[byLoop].GetMember().byMemberType))
			{
				continue;
			}

			// ͨ����Ա�Ǳ������, ������
			if ( tTvwMember.GetConfIdx() && tTvwMember.GetConfIdx() != m_byConfIdx)
			{
				continue;
			}

			if( NULL != ptDropMt)
			{
				*ptDropMt = (TMt)tTvwMember;
			}
			
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  
				"[FindSmcuMtInTvWallAndStop] dorp mt(%d,%d) bIsStopSwitch(%d)\n",
				tTvwMember.GetMcuId(), tTvwMember.GetMtId(), bIsStopSwitch );
			
			if( bIsStopSwitch )
			{		
				TMt tOldMt = (TMt)tTvwMember;
		
				// Hdu�豸����
				if (IsValidHduChn(byTvwEqpId, byTvwChnnlIdx))
				{
					// [2013/03/11 chenbing] VCS���鲻֧��HDU�໭��,��ͨ����0
					ChangeHduSwitch( NULL, byTvwEqpId, byTvwChnnlIdx, 0, tTvwMember.byMemberType, 
						TW_STATE_STOP, MODE_BOTH, FALSE, FALSE );

					//zjj20120414 vcs��ͨ����ѯ�����ش��¼������¼��ĵ����ն˻�ǰ��ǽ�նˣ�������յ���ǽ������ȥ����һ���նˣ��������
					u8 byTwPollState = POLL_STATE_NONE;
					if( !tOldMt.IsLocal() && !IsLocalAndSMcuSupMultSpy(tOldMt.GetMcuId()) && 
						m_tTWMutiPollParam.GetPollState(byTvwEqpId, byTvwChnnlIdx, byTwPollState )  )
					{
						u32 dwTimerIdx = 0;
						if( byTwPollState == POLL_STATE_NORMAL &&
							m_tTWMutiPollParam.GetTimerIdx(byTvwEqpId, byTvwChnnlIdx, dwTimerIdx) )
						{
							SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx,100,dwTimerIdx );
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  
									"[FindSmcuMtInTvWallAndStop] byTvwEqpId.%d byTvwChnnlIdx.%d restart timer\n",
									byTvwEqpId, byTvwChnnlIdx );
						}
					}
				}
				// Tvwall�豸����
				else
				{
					ChangeTvWallSwitch(&tOldMt, byTvwEqpId, byTvwChnnlIdx, tTvwMember.byMemberType, TW_STATE_STOP);
				}

				
			
				//�ǵ�ǰ�����ն��˳�����ǽ�����Ѿ���һ���������ܣ����˳����ƻ���
				if( m_cVCSConfStatus.GetMtInTvWallCanMixing() 
					&& m_tConf.m_tStatus.IsMixing() 
					&& m_tConf.m_tStatus.IsSpecMixing() 
					&& VCSMtNotInTvWallCanStopMixing( tOldMt )		
					)
				{
					RemoveSpecMixMember( &tOldMt, 1, FALSE );
				}
			}

		}else				// �ն˲�����
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "Get Conf Tvwall(%d, %d) failed\n", byTvwEqpId, byTvwChnnlIdx);
			
			//20101101_tzy VCS����ʱ�����ش�ʱ,ͬһ���¼���ԭ������ǽ�ն��뵱ǰ�ϴ��ն˲�һ�����ûָ�
			if( GetFstMcuIdFromMcuIdx( tTvwMember.GetMcuId() ) == GetFstMcuIdFromMcuIdx( tNewCurVCMT.GetMcuId() )
				&& !((TMt)tTvwMember == tNewCurVCMT)
				&& (IsVcsTvwMemberType(tTvwMember.byMemberType) )
				&& tTvwMember.GetConfIdx() == m_byConfIdx ) 
			{
				// ��ո�ͨ��
				ClearTvwChnnlStatus(byTvwEqpId, byTvwChnnlIdx);
				
				// ���ν�Hdu Id����abyTvwIdFind��, �Ա�ͳһ����
				// ����ͬһ���豸�¶��ͨ�������ʱ��η���
				for ( u8 byCounter = 0; byCounter < byMaxTvwChnnlNum; ++byCounter)
				{
					// Ϊ0˵��Ϊ��
					if ( 0 == abyTvwIdFind[byCounter] )
					{
						abyTvwIdFind[byCounter] = byTvwEqpId;
						break;
					}
					
					// �Ѿ�����Ͳ���Ҫ�ٲ�����
					if ( byTvwEqpId == abyTvwIdFind[byCounter] )
					{
						break;
					}
				}
			}
		}
	}

	// �����ߵ���ǽ�豸ͳһ��ղ�֪ͨ����
	u8 byNext = 0;
	TPeriEqpStatus tStatus;
	CServMsg cServMsg;
	while ( byNext < byMaxTvwChnnlNum && 0 != abyTvwIdFind[byNext] )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[FindSmcuMtInTvWallAndStop] notify tvw eqp(%d) status\n", abyTvwIdFind[byNext] );
		if( g_cMcuVcApp.GetPeriEqpStatus( abyTvwIdFind[byNext], &tStatus ) )
		{
			cServMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
			SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
		}
		
		++byNext;
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
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ChgCurVCMT] NewCurVCMT hasn't change\n");	
		return;
	}
	
	u8  byVCMode = m_cVCSConfStatus.GetCurVCMode();
	
	//yanghuaizhi 20120822 ����˫�����ش��ϴλ���ϳ�δ������ɣ�ֱ��return(VMP notify��û��/δ���µ�m_tLastVmpParam��Ϣ��)
	//ֻ��ִ����Setvmpchnl����,�ŻὨ��������m_tLastVmpParam��Ϣ,����ϳɴ���Ž���.
	// ��Գ���:vmp notify�ջ�,���˼������ֱ���,����δ�յ����ֱ���Ack(m_tLastVmpParamδ����)ʱ,
	// ������һ�ն�,��vmpʱ,PresetinReq��Ϣ���޿��ͷ��ն�(����m_tLastVmpParam���),��ֻ��1·����ᱨ������
	// Ϊ��֤vmp�����ԭ����,ǰһ��������ɺ�,��ִ����һ������.
	if (VCS_GROUPROLLCALL_MODE == byVCMode || VCS_GROUPVMP_MODE == byVCMode)
	{
		// ����ϳ����账����
		if (!m_tConfInStatus.IsVmpNotify())
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ChgCurVCMT]VCS_GROUPROLLCALL_MODE/VCS_GROUPVMP_MODE last vmp req not notify yet! return\n");
			return;
		} /* notify�ջ�,�����м������ֱ��ʴ���,Ack��Ż�Setvmpchnl.Setvmpchnl��ű�ʾ����ok
		else if (!tCurVCMT.IsNull() && !tCurVCMT.IsLocal() && m_tConfAllMtInfo.MtJoinedConf(tCurVCMT)
			&& IsLocalAndSMcuSupMultSpy(tCurVCMT.GetMcuIdx())
			&& !m_tLastVmpParam.IsMtInMember(tCurVCMT))
		{
			// ���¼��Ƿǿƴ�MT,��ǰ���䲻��ʱ,�������Lastvmpparam
			if(!IsKedaMt(tCurVCMT, FALSE) && m_tVmpChnnlInfo.GetHDChnnlNum() == m_tVmpChnnlInfo.GetMaxNumHdChnnl())
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ChgCurVCMT]tCurVCMT(%d,%d) is noneKedaMt, VmpHdChl[%d] is full.\n");
			}
			else
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ChgCurVCMT]VCS_GROUPROLLCALL_MODE/VCS_GROUPVMP_MODE last vmp req not setvmpchl yet! return\n");
				return;
			}
		}*/
	}

	//zhouyiliang 20101229�������� �µ����ն��Ǳ����ն˻����ǵ��ش��¼��ն��Ҳ����Զ��໭��ģʽ�ϴε�����Ӧû����������л���ǰ�����ն�
	if ( ( tNewCurVCMT.IsLocal() || (!tNewCurVCMT.IsLocal() && !IsLocalAndSMcuSupMultSpy(tNewCurVCMT.GetMcuIdx()))) 
		&& !( VCS_GROUPVMP_MODE == byVCMode  && !m_tConfInStatus.IsVmpNotify() )
		)
	{
/*#ifdef _8KE_
		if( ( VCS_GROUPVMP_MODE == byVCMode || VCS_GROUPROLLCALL_MODE == byVCMode ) &&
			!tNewCurVCMT.IsNull() && !IsKedaMt(tNewCurVCMT) )
		{
			if( !IsNoneKedaMtCanInMpu(tNewCurVCMT) )
			{
				TMt tNullMt;
				tNullMt.SetNull();
				m_cVCSConfStatus.SetReqVCMT(tNullMt);
				//VCSConfStatusNotif();
				NotifyMcsAlarmInfo(0,ERR_VMP_8000E_NOTSPPORT_NONEKEDAMT);					
				return;
			}
		}
#endif*/
		m_cVCSConfStatus.SetCurVCMT(tNewCurVCMT);
//		ChangeConfRestoreMixParam();
	}

	// [6/29/2011 liuxu] ���ģʽ�µ����⴦��
	if (ISGROUPMODE(byVCMode))
	{
		if (!tNewCurVCMT.IsNull())
		{
			// ���ش��µĴ���
			if (!tNewCurVCMT.IsLocal() && !IsLocalAndSMcuSupMultSpy( tNewCurVCMT.GetMcuId()))
			{
				OnMMcuSetIn(tNewCurVCMT, m_cVCSConfStatus.GetCurSrcSsnId(), SWITCH_MODE_SELECT);
				
				// ��ȡ����ǽ����ģʽ
				const u8 byTvwMgrMode = m_cVCSConfStatus.GetTVWallManageMode();
				
				//zjj20091025 ��������������,��������ģʽ��
				//�����¼��ն�ʱ��Ҫ��ͬһ���¼��Ĳ�ͬ�ն��޳�������ǽ,��Ϊ����ֻ����һ���ش��ն�		
				TMt tDropMt;
				tDropMt.SetNull();
				
				if( byTvwMgrMode == VCS_TVWALLMANAGE_MANUAL_MODE )
				{
					FindSmcuMtInTvWallAndStop( tNewCurVCMT,&tDropMt );	
				}
			}			
		}
	}

	//20101222_tzy ֻ�����µ����ն˷Ǳ��������µ����ն�����MCU֧�ֶ�ش�ʱ�����߶�ش��߼���������ԭ�����ش��߼�
	//�ߵ��ش��龰������ȡ�����ȣ��µ����ն�Ϊ���������µ���Ϊ�¼��ն˵���֧�ֶ�ش�
	if( ISGROUPMODE(byVCMode) 
		&& m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_AUTO_MODE 
		&& (tNewCurVCMT.IsNull() 
		    || tNewCurVCMT.IsLocal() 
			|| !IsLocalAndSMcuSupMultSpy(tNewCurVCMT.GetMcuId())))
	{
		TMt  tDropOutMT;
		tDropOutMT.SetNull();

		//zjj20090911�����¼��ն�ʱҪ��ͬ���������ն��޳�����ǽ
		if(!tNewCurVCMT.IsNull() && !tNewCurVCMT.IsLocal() )
		{
			//zhouyiliang 20101229�������� �ϲ����ش��ظ�����
			OnMMcuSetIn(tNewCurVCMT, m_cVCSConfStatus.GetCurSrcSsnId(), SWITCH_MODE_SELECT);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ChgCurVCMT]FindSmcuMtInTvWallAndStop\n" );
			FindSmcuMtInTvWallAndStop( tNewCurVCMT,&tDropOutMT );

			tDropOutMT.SetNull();			
		}

		// ��һ���ն˵��ȳɹ��ٽ��У���ǰ�����ն��滻��ԭ�ն˽�����ǽ�Ĳ���
		if (!tCurVCMT.IsNull() 
			&& m_tConfAllMtInfo.MtJoinedConf(tCurVCMT.GetMcuId(), tCurVCMT.GetMtId()))
		{
			TEqp tEqp;
			
			tEqp.SetNull();
			u8 byNeedInTW = TRUE;

			// ����ǰ���ȵ��ն����µ��ȵ��ն�����ͬһ���¼�MCU,�����轫��ǰ���ȵ��ն˷������ǽ
			if (!tCurVCMT.IsLocal() 
				&& !tNewCurVCMT.IsLocal()  
				&& IsMtInMcu(GetLocalMtFromOtherMcuMt(tCurVCMT), tNewCurVCMT))
			{
				byNeedInTW = FALSE;
			}

			// [5/30/2011 liuxu] ��д��FindUsableTWChan, ��FindNextTvwChnnl����.
			CConfTvwChnnl cNextTvwChnnl;

			if (byNeedInTW)
			{
				// �ҵ����õĵ���ǽͨ������ԭ�����ն�ѡ��������ǽ��û�����֪ͨ�����ָ�����������ģʽ
				// [5/30/2011 liuxu] ��д��FindUsableTWChan, ��FindNextTvwChnnl����
				u16 wNextChanIdx = FindNextTvwChnnl(m_cVCSConfStatus.GetCurUseTWChanInd(), &tCurVCMT, &cNextTvwChnnl, FALSE );
				tDropOutMT = (TMt)cNextTvwChnnl.GetMember();				

				if( wNextChanIdx 
					&& ( (IsValidHduChn(cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx()) && CheckHduAbility(tCurVCMT,cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx()))
						|| (IsValidTvw(cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx()) && !IsValidHduChn(cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx())))
				  )					
				{	
					if ( !tDropOutMT.IsNull() && !(tDropOutMT == tCurVCMT))
					{
						if (IsValidHduChn(cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx()))
						{
							// [2013/03/11 chenbing] VCS���鲻֧��HDU�໭��,��ͨ����0
							ChangeHduSwitch( NULL,cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx(),0, TW_MEMBERTYPE_VCSAUTOSPEC, TW_STATE_STOP,MODE_BOTH,FALSE,FALSE );
						}
						else
						{
							ChangeTvWallSwitch( &tDropOutMT,cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx(),TW_MEMBERTYPE_VCSAUTOSPEC, TW_STATE_STOP);
						}
					}

					// [10/9/2011 liuxu] ���滻�ҵ��ĵ��ش���ͬ�¼��ն���ռ��ͨ��,�������ҵ��ĵ�һ������ͨ��
					CConfTvwChnnl cActTvwChnnl;
					cActTvwChnnl.Clear();
					if (GetCfgTvwChnnl( (u8)wNextChanIdx, cActTvwChnnl))
					{
						tEqp = g_cMcuVcApp.GetEqp( cActTvwChnnl.GetEqpId() );
						
						TTWSwitchInfo tSwitchInfo;
						tSwitchInfo.SetSrcMt(tCurVCMT);
						tSwitchInfo.SetDstMt(tEqp);

						tSwitchInfo.SetDstChlIdx(cActTvwChnnl.GetChnnlIdx());
						tSwitchInfo.SetMemberType(TW_MEMBERTYPE_VCSAUTOSPEC);
						tSwitchInfo.SetMode(MODE_VIDEO);
						
						VCSConfMTToTW(tSwitchInfo);
					}
				}

				// [8/16/2011 liuxu] ����2�� ���ش����߳�����ǰ�����ն������, �����ܷ�������ǽ, �ɵ����ն˶�Ҫռ�ô�ͨ��
				if(wNextChanIdx)
					m_cVCSConfStatus.SetCurUseTWChanInd(wNextChanIdx);
			}			
		}
	}

	if( !IsLocalAndSMcuSupMultSpy( tNewCurVCMT.GetMcuId()) )
	{
		BOOL32 bIsSendToChairman = FALSE;
		if( m_tApplySpeakQue.ProcQueueInfo( tNewCurVCMT,bIsSendToChairman,FALSE ) )
		{
			NotifyMcsApplyList( bIsSendToChairman );
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ChgCurVCMT] Drop tMt(%d.%d) from Queue. \n",
						tNewCurVCMT.GetMcuId(),
						tNewCurVCMT.GetMtId()
						);
		}
		if( byVCMode != VCS_MULVMP_MODE &&
			byVCMode != VCS_GROUPVMP_MODE )
		{
			NotifyMtSpeakStatus( tCurVCMT, emCanceled );
		}			
		NotifyMtSpeakStatus( tNewCurVCMT, emAgreed );
	}

	switch(byVCMode)
	{
	case VCS_SINGLE_MODE:
		if (!tCurVCMT.IsNull() && 
			m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_MANUAL_MODE &&
			!m_ptMtTable->IsMtInTvWall( tCurVCMT.GetMtId() ) &&
            !m_ptMtTable->IsMtInHdu( tCurVCMT.GetMtId() ) && 
            IsMtNotInOtherTvWallChnnl(tCurVCMT,0,0) &&
			IsMtNotInOtherHduChnnl(tCurVCMT,0,0)
            )
		{
			VCSDropMT(tCurVCMT);
		}
		else
		{
			// �ڵ���ǽ���Ϊ�ǵ����նˣ�ͣѡ��
			VCSConfStopMTSel(tCurVCMT, MODE_BOTH);
		}
		if ( !tNewCurVCMT.IsNull() && m_tConf.m_tStatus.IsSpecMixing() )
		{				
			AddSpecMixMember(&tNewCurVCMT, 1, TRUE);
		}
		break;

	case VCS_MULVMP_MODE:
		// �����������		
	    break;

	case VCS_GROUPSPEAK_MODE:
	//20091026��������������������ϯģʽ
	case VCS_GROUPCHAIRMAN_MODE:
	//zjj20091102 �����������ģʽ
	case VCS_GROUPROLLCALL_MODE:
			// �·�����ģʽ�������(���ն˿���ϯ����ϯ����ǰ�ĵ����նˣ���ϯ����ǰ�����ն˻���)
			if (m_tConf.m_tStatus.IsSpecMixing() && 
				(tNewCurVCMT.IsNull() || tNewCurVCMT.IsLocal() || !IsLocalAndSMcuSupMultSpy(tNewCurVCMT.GetMcuId())))
			{
				if( (!m_cVCSConfStatus.GetMtInTvWallCanMixing() 
					|| (IsMtNotInOtherHduChnnl(tCurVCMT,0,0) 
						&& IsMtNotInOtherTvWallChnnl(tCurVCMT,0,0) 
						&& m_cVCSConfStatus.GetMtInTvWallCanMixing())) )
	
				{		
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, "[ChgCurVCMT] call RemoveSpecMixMember The tCurVCMT is (%d %d)!\n", 
						tCurVCMT.GetMcuId(), tCurVCMT.GetMtId());

					RemoveSpecMixMember( &tCurVCMT, 1, FALSE, FALSE );
					if (!tCurVCMT.IsLocal() && 
						GetLocalMtFromOtherMcuMt(tCurVCMT).GetMtId() !=  GetLocalMtFromOtherMcuMt(tNewCurVCMT).GetMtId() &&
						(!m_cVCSConfStatus.GetMtInTvWallCanMixing() || !IsHasMtInHduOrTwByMcuIdx(tCurVCMT.GetMcuIdx())))
					{
						TMt tLocalMt = GetLocalMtFromOtherMcuMt(tCurVCMT);
						RemoveSpecMixMember(&tLocalMt,1,FALSE,FALSE);
					}
				}
			}
			if (tNewCurVCMT.IsNull() )
			{
				if (VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState())
				{
					VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);
				}
				if( byVCMode == VCS_GROUPSPEAK_MODE )
				{
					TMt tChairMan = m_tConf.GetChairman();
					ChangeSpeaker(&tChairMan, FALSE, FALSE);
				}
			}
			else
			{
				
				//zjj20091026 �����������������������ģʽ��ǰ�����ն���������
				if( VCS_GROUPSPEAK_MODE == byVCMode )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ChgCurVCMT] byVCMode is VCS_GROUPSPEAK_MODE ChangeSpeaker to curVcmt. \n" );
					ChangeSpeaker(&tNewCurVCMT, FALSE, FALSE);

					//20091102������ģʽ �����˿��Լ�
					if( tNewCurVCMT.IsLocal() )
					{
						NotifyMtReceive(tNewCurVCMT, tNewCurVCMT.GetMtId());		
					}							
				}
				else
				{
					/*zhouyiliang 20110301����˫�����ش��ϴλ���ϳɵ�notify��û����ֱ��return
					if (VCS_GROUPROLLCALL_MODE == byVCMode && !m_tConfInStatus.IsVmpNotify() 
						&& IsLocalAndSMcuSupMultSpy(tNewCurVCMT.GetMcuId()) ) 
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ChgCurVCMT]VCS_GROUPROLLCALL_MODE last vmp req not notify yet! return\n");
						return;
					}*/
					TSwitchInfo tSwitchInfo;
					tSwitchInfo.SetSrcMt(tNewCurVCMT);
					tSwitchInfo.SetDstMt(m_tConf.GetChairman());
					tSwitchInfo.SetMode(MODE_VIDEO);
					//zjj20110223 ������ϯ��ѯʱ����ĳ��֧�ֶ�ش����¼�mcu���ն�ʱ����Ҫ����presetin,�Խ����ն�
						//���ó�Ϊ��ǰ�����ն�,��֤��ػ��������ʾ���ն�ͼ��
					if ( VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState() || 
							(VCS_GROUPCHAIRMAN_MODE == byVCMode && !tNewCurVCMT.IsLocal() &&
								IsLocalAndSMcuSupMultSpy(tNewCurVCMT.GetMcuId())
							)
						)
					{
						//��ϯѡ����ǰ�����ն�					
						BOOL32 bSelSuc = VCSConfSelMT(tSwitchInfo);
						if (!bSelSuc)
						{
							VCSConfStopMTSel(tSwitchInfo.GetDstMt(),tSwitchInfo.GetMode());
						}
					}
				
				}				

				if (m_tConf.m_tStatus.IsSpecMixing()
					&& (tNewCurVCMT.IsLocal() || !IsLocalAndSMcuSupMultSpy(tNewCurVCMT.GetMcuId())))
				{				
					AddSpecMixMember(&tNewCurVCMT, 1, TRUE);
				}
			}

			//zjj20091102
			if( VCS_GROUPROLLCALL_MODE == byVCMode )
			{
				m_cVCSConfStatus.SetReqVCMT(tNewCurVCMT);
				ChangeVmpStyle(tNewCurVCMT, TRUE);
			}
			break;

		case VCS_GROUPVMP_MODE:	
			if (!tNewCurVCMT.IsNull()
				&& m_tConfInStatus.IsVmpNotify()  //zhouyiliang 20101216 vcs�Զ�����ϳ�����ϴ�notify��û������β�����
				)
			{
				//�������ϳ�ģʽҪ�ѵ����ն˽������,
				//����ֻ�ǽ��Ǳ��ؽ����������Ϊ���صĻ��ڻ���ϳɻ�VMP_MCU_CHANGESTATUS_NOTIF��Ϣʱ��
				//ChangeSpecMixMember�����������
				TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
				if ( !tConfVmpParam.IsMtInMember(tNewCurVCMT))
				{
					m_cVCSConfStatus.SetReqVCMT(tNewCurVCMT);
					ChangeVmpStyle(tNewCurVCMT, TRUE);					
				}
				else
				{
					// 2011-10-27 add by pgf:����µ����ն��Ѿ���VMP�У��Ͳ���Ҫ�ٵ�VMP�ˣ�ֱ���裬Ȼ���ɽ��淢MCS_MCU_STARTSWITCHMT_REQ���ٽ�����صĽ���
					m_cVCSConfStatus.SetCurVCMT(tNewCurVCMT);
				}
			}
			break;

		default:
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ChgCurVCMT]Wrong vcmode\n");
			break;
	}
	
	TMt tNullMt;
	tNullMt.SetNull();
	m_cVCSConfStatus.SetReqVCMT(tNullMt);

	// ͨ������е�VCS����״̬�ĸı�
	VCSConfStatusNotif();
}

/*====================================================================
������      ��FindNextTWChan
����        ����ѯ��һ�����õĵ���ǽͨ��
�㷨ʵ��    ��(1) �����㷨�Ǵ�swCurChnnlIdx+1λ�ÿ�ʼ,���α�������ͨ��; ���������л᷵��0����ѭ����
			  (2) ���ptNextMtΪ��, �򷵻�swCurChnnlIdx�����һ�����õ�ͨ��������; �Ҳ�������0
			  (3) �������ptNextMtΪ�¼��ն�, �ҵ��ش�, ������ҵ�����ͬ���������ն���ռ��ͨ��,
				  �򷵻ش�ͨ��, ���򷵻ص�һ�����õ�ͨ��. �Ҳ�������0
			  (4) ���򷵻ص�һ�����õ�ͨ��. �Ҳ�������0
����ȫ�ֱ�����
�������˵����[IN]		wCurChnnlIdx : ��ǰ��ʹ�õ�ͨ������
			  [IN]         ptNextMt: ���������ǽ���ն�
			  [OUT]		pcNextChnnl: ��û��δ�õ�ͨ��������ռ���ն���Ϣ
			  [in] bUseSingleSpyFirst: �Ƿ�����ʹ�õ��ش���ptNextMtͬ���ն���ռ��ͨ��
			  Ĭ����true, ���ش�ʱ����ʹ��ptNextMt(�Ǳ���)ͬ����ռ�ĵ���ǽͨ��
����ֵ˵��  ��s16 : ���ص�ǰ���õ�ͨ��������, û�ҵ�����ֵС��0�� �ҵ�����ڻ����0
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
11/05/28                ����          ����
====================================================================*/
u16 CMcuVcInst::FindNextTvwChnnl( const u16 wCurChnnlIdx, 
								 const TMt* ptNextMt/* = NULL*/, 
								 CConfTvwChnnl* pcNextChnnl /*= NULL*/,
								 const BOOL32 bUseSingleSpyFirst/* = TRUE*/)
{
	// �����ܹ����õĵ���ǽͨ������
	const u16 wConfCfgedTvwChnNum = GetVcsHduAndTwModuleChnNum();
	if ( wCurChnnlIdx > wConfCfgedTvwChnNum )
	{
		return 0;
	}

	// ������pcNextChnnl���
	if (pcNextChnnl)
	{
		pcNextChnnl->Clear();
	}
	
	CConfTvwChnnl cTvwChnnl;								// ��Ѱ�ҵ�ͨ��
	BOOL32 bFindChnnlFinded = FALSE;						// ���ش�ʱ��һ�����õ�ͨ���Ƿ��ҵ��ı�־
	u16 wNextChnnlIdx = 0;									// �����ص�ͨ������ֵ

	// ��swCurChnnlIdx++λ�ÿ�ʼ��������tvwͨ��, ֱ���ҵ����ʵ�ͨ��. ������byConfCfgedTvwChnNum��
	for ( u16 wLoop = 0, wNextIdx = wCurChnnlIdx + 1; wLoop < wConfCfgedTvwChnNum; ++wLoop, ++wNextIdx)
	{
		// �����һ��ͨ�����������������, ���1 ��ʼ����
		if ( wNextIdx > wConfCfgedTvwChnNum )
		{
			wNextIdx = 1;
		}
	
		if (!GetCfgTvwChnnl((u8)wNextIdx, cTvwChnnl))
		{
			continue;
		}

		// ͨ�������豸������, continue;���Ӷ�HDUͨ��״̬���ж�,״̬������continue;
		if ( !g_cMcuVcApp.IsPeriEqpConnected(cTvwChnnl.GetEqpId()) || 
			 (HDUID_MIN <= cTvwChnnl.GetEqpId() && cTvwChnnl.GetEqpId() <= HDUID_MAX && 
			 cTvwChnnl.GetStatus() != THduChnStatus::eREADY && cTvwChnnl.GetStatus() != THduChnStatus::eRUNNING)
		   )
		{
			continue;
		}

		// ͨ����Ա���Ͳ���VCS�����Զ�����, continue
		if ( TW_MEMBERTYPE_VCSAUTOSPEC != cTvwChnnl.GetConfMemberType()
			&& cTvwChnnl.GetConfIdx() != 0 )
		{
			continue;
		}

		// ͨ������������ռ�þ�continue,���������Ƿ��г�Ա
		if (/* !cTvwChnnl.GetMember().IsNull() &&*/
			cTvwChnnl.GetMember().GetConfIdx() != 0 
			&& cTvwChnnl.GetMember().GetConfIdx() != m_byConfIdx)
		{
			continue;
		}

		// ���ش�ʱ, �¼��ն˽������ǽ, �����滻��ͬ�����ڵĵ���ǽͨ��
		if (ptNextMt && !ptNextMt->IsNull() &&!ptNextMt->IsLocal() && !IsLocalAndSMcuSupMultSpy(ptNextMt->GetMcuIdx()))
		{
			// �ѵ�һ�����õ�λ�ü�¼����
			if (!bFindChnnlFinded)
			{
				bFindChnnlFinded = TRUE;
				wNextChnnlIdx = wNextIdx;
				if ( pcNextChnnl )
				{
					*pcNextChnnl = cTvwChnnl;
				}
				
				// ����������滻��ش���ռͨ��, ��ô����Ϳ��Է�����
				if (!bUseSingleSpyFirst)
				{
					return wNextChnnlIdx;
				}
			}

			// �ҵ��¼��ն˵�ͬ���ն��ն���ռ��ͨ��, ���ش�ͨ��
			if (!cTvwChnnl.GetMember().IsNull() 
				&& GetFstMcuIdFromMcuIdx( cTvwChnnl.GetMember().GetMcuId() ) == GetFstMcuIdFromMcuIdx( ptNextMt->GetMcuId() ))
			{
				if ( pcNextChnnl )
				{
					*pcNextChnnl = cTvwChnnl;
				}
				
				// [10/9/2011 liuxu] �޸ĳɲ��滻��ͨ��, ����ȡ��ͬ�����ڵ�����ͨ��, ������
				// ��һ���ҵ��Ŀ���ͨ��
				
				if (wNextChnnlIdx
					&& wNextIdx != wNextChnnlIdx
					&& bUseSingleSpyFirst)
				{
					//����:  �����Ѿ��ֵ�2��,���ڸý�������ͨ��. ���ڲ�ȡ�����滻����, 
					//		 ���ҵ���ͨ���ֲ��ǵ�����ͨ��, ��ô����Ӧ�û�ͣ����2
					wNextChnnlIdx = (wNextChnnlIdx > 1) ? ( wNextChnnlIdx -1 ): wNextChnnlIdx;
					return wNextChnnlIdx;
				}else
				{
					wNextChnnlIdx = wNextIdx;
				}
				
				break;	//return wNextIdx;
			}
		}
		// ���������, ����ֱ�ӷ��ش˵���ǽͨ��
		else
		{
			if ( pcNextChnnl )
			{
				*pcNextChnnl = cTvwChnnl;
			}

			return wNextIdx;
		}
		
		continue;
	}

	// �����ϱ�����, �������ptNextMtΪ��, �����ﷵ��-1
	// �������ptNextMtΪ�¼��ն�, �ҵ��ش�, �����ﷵ�������ҵ���
	// ��������(ptNextMtΪ����, ��֧�ֶ�ش�)Ҳ����-1
	return wNextChnnlIdx;
}

/*==============================================================================
������    :  GetVcsRestoreConfVmpParam
����      :  ���vcs�л�����ʱ��Ĭ��VmpParam��Ϣ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-03-20                yanghuaizhi
==============================================================================*/
TVMPParam_25Mem CMcuVcInst::GetVcsRestoreConfVmpParam()
{
	TVMPParam_25Mem  tParam;
	tParam.SetVMPAuto(TRUE);
	tParam.SetVMPMode(CONF_VMPMODE_AUTO);
	tParam.SetVMPStyle(VMP_STYLE_DYNAMIC);
	tParam.SetVMPBrdst(TRUE);
	tParam.SetVMPSchemeId(0);

	return tParam;
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
	/* ��ʱ��һ��Ĭ��ֵ
	TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
	tConfAttrib.SetHasVmpModule(TRUE);
	m_tConf.SetConfAttrb(tConfAttrib);

	TVMPParam  tParam;
	memset(&tParam, 0, sizeof(tParam));	// [12/3/2010 xliang] set 0 is a must as it has no construct.

	tParam.SetVMPAuto(TRUE);
	tParam.SetVMPMode(CONF_VMPMODE_AUTO);
	tParam.SetVMPStyle(VMP_STYLE_DYNAMIC);
	tParam.SetVMPBrdst(TRUE);
	tParam.SetVMPSchemeId(0);

	//TODO:��yanghuaizhi����
	m_tConfEqpModule.SetVmpModuleParam(tParam);

	tConfAttrib = m_tConf.GetConfAttrb();
	if(tConfAttrib.IsHasVmpModule())			
	{*/
		u8 byIdleVMPNum = 0;
		u8 abyVMPId[MAXNUM_PERIEQP];
		memset(abyVMPId, 0, sizeof(abyVMPId));
		g_cMcuVcApp.GetIdleVMP(abyVMPId, byIdleVMPNum, sizeof(abyVMPId));

		//�Ƿ��л���ϳ���
		if( byIdleVMPNum != 0)
		{
			//TVmpModule tVmpModule = GetVmpModule();
			TVMPParam_25Mem tVmpParam = GetVcsRestoreConfVmpParam();

			u8  byVmpId = 0;
            if ( CONF_VMPMODE_AUTO == tVmpParam.GetVMPMode() )
            {
                //��ǰ���еĿ���VMP�Ƿ�֧������ĺϳɷ��
                u16 wError  = 0;

                if ( IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError) ) 
                {
					// vmp������vicp��Դռ���жϣ����������б�
					if (!PrepareVmpOutCap(byVmpId))
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[FindUsableVMP] There is not enough vicp.\n");
						g_cMcuVcApp.ClearVmpInfo(g_cMcuVcApp.GetEqp(byVmpId));
						return FALSE;
					}

					m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );//VCS���ǹ㲥vmp���˼�¼��ȷ
					m_tVmpEqp.SetConfIdx( m_byConfIdx );
                }
                else
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[FindUsableVMP] no vmp support thise style\n");
					return FALSE;
				}
			}
			else
			{
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[FindUsableVMP] VCSCONF hasn't non-auto vmp\n");
				return FALSE;
			}

			// ����ռ�û���ϳ���
			AddVmpIdIntoVmpList(byVmpId);
            TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::RESERVE;//��ռ��,��ʱ��δ�ɹ��ٷ���
			tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);
			tPeriEqpStatus.SetConfIdx( m_byConfIdx );
			g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[FindUsableVMP]vmp(%d) has been used by conf(%d)\n", byVmpId, m_byConfIdx);
			// ����ʱ�������ǰ������Ŀ
			TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
			TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
			tVmpChnnlInfo.clear();// ����ʱ���ǰ������Ϣ
			u8 byMaxStyleNum = 0; //���֧�ּ�������ڲ����ֱ���
			u8 byMaxHdChnnlNum = GetMaxHdChnlNumByVmpId(m_tConf, byVmpId, byMaxStyleNum);
			tVmpChnnlInfo.SetMaxNumHdChnnl(byMaxHdChnnlNum);
			tVmpChnnlInfo.SetMaxStyleNum(byMaxStyleNum);
			g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);

			/*���ͨ��
			for( u8 byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++ )
			{
				m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );
			}*/
		}
		else
		{
			return FALSE;
		}
	/*}
	else
	{
		return FALSE;
	}*/

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
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[RestoreVCConf] wrong curstate(%d)\n", CurState());
		return;
	}

	CServMsg cServMsg;
	cServMsg.SetConfIdx(m_byConfIdx);
	cServMsg.SetConfId(m_tConf.GetConfId());

	u8 byOldMode = m_cVCSConfStatus.GetCurVCMode();

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "restore vcsconf from %d to %d\n", byOldMode, byNewMode);

	//zhouyiliang 20100812 ��������֮���һ�����ش�Ҫ������Ӧ��free
	if( ISGROUPMODE(byOldMode) && ISGROUPMODE(byNewMode) )
	{
		//zhouyiliang 20100812 1.������freeһ��
		TMt tCurVcMt =	m_cVCSConfStatus.GetCurVCMT();
		if ( !tCurVcMt.IsNull() && !tCurVcMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tCurVcMt.GetMcuId())  ) 
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[RestoreVCConf] stop mc switch to curVcMt, StopSwitchToAllMcWatchingSrcMt:McuId.%d,MtId.%d\n"
				,tCurVcMt.GetMcuId(),tCurVcMt.GetMtId());
			AdjustSwitchToAllMcWatchingSrcMt( tCurVcMt,TRUE,MODE_BOTH,TRUE,VCS_VCMT_MCCHLIDX );
		}
	}

    // ֹͣ������ϯ�ն˵���������
    //NotifyMtSend(m_tConf.GetChairman().GetMtId(), MODE_VIDEO, FALSE);
	// ���ڱ������ߵ���ϯ�ն�״̬�Ļָ�
	// ����ϯ�ն���ѡ���¼�MCU����ȡ��
	// �����ģʽ����ϯ��ѯ��ֹͣ
	if (!ISGROUPMODE(byNewMode))
	{
		KillTimer(MCUVC_VCS_CHAIRPOLL_TIMER);
		m_cVCSConfStatus.SetChairPollState(VCS_POLL_STOP);
	}

	if (!(ISGROUPMODE(byNewMode) && ISGROUPMODE(byOldMode)) ||
		//�����������ģʽ��Ҫֹͣ��ϯѡ��
		( ( byOldMode == VCS_GROUPSPEAK_MODE || byOldMode == VCS_GROUPTW_MODE ||
			byOldMode == VCS_GROUPCHAIRMAN_MODE || byOldMode == VCS_GROUPROLLCALL_MODE ) &&
		   byNewMode != byOldMode &&
     	   m_cVCSConfStatus.GetChairPollState() == VCS_POLL_STOP )
		)
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
	if (g_cMcuVcApp.GetVMPMode(m_tVmpEqp) != CONF_VMPMODE_NONE &&
		byNewMode != VCS_MULVMP_MODE && byNewMode != VCS_GROUPVMP_MODE &&
		byNewMode != VCS_GROUPROLLCALL_MODE )
	{
		TPeriEqpStatus tPeriEqpStatus;
		if (!m_tVmpEqp.IsNull() &&
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
			tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState != TVmpStatus::WAIT_STOP)
		{
			u8 byVmpIdx = m_tVmpEqp.GetEqpId() - VMPID_MIN;
			SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER+byVmpIdx, TIMESPACE_WAIT_VMPRSP);
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
			SendMsgToEqp( m_tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cServMsg );
		}
	}

	// ��������Դ���ͷ�
	// ֹͣ������	
	//zjj20091102 �����ģʽ�ǵ���������û��һ������������ֹͣ����
	if( m_tConf.m_tStatus.IsMixing() && !ISGROUPMODE(byNewMode) && 
		byNewMode != VCS_MULTW_MODE && byNewMode != VCS_MULVMP_MODE &&		
		( byNewMode == VCS_SINGLE_MODE && !m_cVCSConfStatus.GetMtInTvWallCanMixing() ) 
		)
	{
		StopMixing();
	}

	u8 byCanStopToTvWall = 0;

	// ����ǽ��Դ���ͷ�
	// ����ǽͨ������
	if (GetVcsHduAndTwModuleChnNum())						// ���������˵���ǽ
	{
		// һ������֧�ֵĵ���ǽͨ��������
		const u8 byMaxTvwChnnlNum = MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL + MAXNUM_PERIEQP_CHNNL * MAXNUM_MAP;
		CConfTvwChnnl acTvwChnnlFind[byMaxTvwChnnlNum];		// ��Ѱ�ҵĵ���ǽͨ��
		u8 byEqpId, byChnnlIdx;								// ����ǽͨ����Ӧ���豸�ź����豸�е�ͨ����
		
		u8 abyTvwEqpId[128] = { 0 };

		u8 byMtConfIdx;										// ����ǽͨ����Ա�Ļ���idx
		u8 byInTWMtId;										// ����ǽͨ����Աid
		TMt tDropMt;
		u8 byLoop;											
		//TSwitchInfo tSwitchInfo;

		// ��ȡ���е���ǽͨ��
		const u8 byTvwChnnlNum = GetAllCfgedTvwChnnl( acTvwChnnlFind, byMaxTvwChnnlNum );
		for ( byLoop = 0; byLoop < byTvwChnnlNum; ++byLoop )
		{
			if (acTvwChnnlFind[byLoop].IsNull())
			{
				continue;
			}
			
			byEqpId = acTvwChnnlFind[byLoop].GetEqpId();
			byChnnlIdx = acTvwChnnlFind[byLoop].GetChnnlIdx();
			
			if ( 0 != acTvwChnnlFind[byLoop].GetMember().GetConfIdx() 
				&& acTvwChnnlFind[byLoop].GetMember().GetConfIdx() != m_byConfIdx )
			{
				continue;
			}

			// ������, �򲻽��в���
			if (!g_cMcuVcApp.IsPeriEqpConnected(byEqpId))
			{
				continue;
			}
			
			byCanStopToTvWall = 1;
			byMtConfIdx =  acTvwChnnlFind[byLoop].GetMember().GetConfIdx();
			
			u8 byMemberType = acTvwChnnlFind[byLoop].GetMember().byMemberType;
			
			//zjj20091102 �������������󣬷������������������������ҳ�Ա������TW_MEMBERTYPE_VCSSPEC���Ҷ�	
			if ( ((!ISGROUPMODE( byOldMode) && !ISGROUPMODE( byNewMode)) || (ISGROUPMODE( byOldMode) && ISGROUPMODE( byNewMode)) ) 
				&& ( TW_MEMBERTYPE_VCSSPEC == byMemberType || TW_MEMBERTYPE_VCSAUTOSPEC == byMemberType ) )
			{
				byCanStopToTvWall = 0;
			}
			
			// [6/8/2011 liuxu] vcs���ģʽ֮���л�ʱ, ͨ������ѯ�Ĳ����ն�
			if ((ISGROUPMODE( byOldMode) && ISGROUPMODE( byNewMode))
				&& TW_MEMBERTYPE_TWPOLL == byMemberType )
			{
				byCanStopToTvWall = 0;
			}

			if (!ISGROUPMODE( byNewMode))
			{
				// [6/8/2011 liuxu] ����ǽ����ѯ,ҲҪֹͣ
				u8 byPollState = POLL_STATE_NONE;
				if( m_tTWMutiPollParam.GetPollState(byEqpId, byChnnlIdx, byPollState) && POLL_STATE_NONE != byPollState)
				{
					//��ȡ��ʱ������
					u32 dwTimerIdx = 0;
					if(!m_tTWMutiPollParam.GetTimerIdx(byEqpId, byChnnlIdx, dwTimerIdx))
					{
						ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[ProcMcsMcuHduPollMsg] Cann't get tvw<EqpId:%d, ChnId:%d> TimerIdx!\n", byEqpId, byChnnlIdx);
						continue;
					}
					
					// �ٴ����ö�ʱ��,�������������,�ն˻�Ҷ�,�Ҳ�����һ����ѯ�ն�,���ջ��ͷ���Դ
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx,100,dwTimerIdx);	
				}
			}
			
			if (m_byConfIdx == byMtConfIdx && byCanStopToTvWall )
			{							
				byInTWMtId  = GetLocalMtFromOtherMcuMt((TMt)acTvwChnnlFind[byLoop].GetMember()).GetMtId();
				
				StopSwitchToPeriEqp(byEqpId, byChnnlIdx);
				tDropMt = GetLocalMtFromOtherMcuMt( (TMt)acTvwChnnlFind[byLoop].GetMember() );

				ClearTvwChnnlStatus(byEqpId, byChnnlIdx);

				//���ն�״̬
				m_ptMtTable->SetMtInHdu(byInTWMtId, FALSE);
				TMt tMt = m_ptMtTable->GetMt(byInTWMtId);
				MtStatusChange(&tMt, TRUE);
				
				//zjj20091102 ���������
				if( !tDropMt.IsNull() 
					&& m_ptMtTable->IsMtInMixing( tDropMt.GetMtId() )) 
				{
					RemoveSpecMixMember( &tDropMt, 1, FALSE );
				}	
				
				// �������ǽ�ն��˳�ʱ��Ҫ�ָ��ֱ���
				if (!IsValidHduChn(byEqpId, byChnnlIdx))
				{
					if( NeedChangeVFormat(tDropMt) 
						&& !m_ptMtTable->IsMtInTvWall( GetLocalMtFromOtherMcuMt(tDropMt).GetMtId() ) )
					{
						ChangeVFormat( tDropMt, FALSE );					//�ָ��ֱ���
					}
				}

				// ����abyTvwEqpId����, ������һ���ϱ�
				for (u8 byTvwNotify = 0; byTvwNotify < 128; ++byTvwNotify)
				{
					if ( byEqpId && byEqpId == abyTvwEqpId[byTvwNotify] )
					{
						break;
					}

					if ( 0 == abyTvwEqpId[byTvwNotify] )
					{
						abyTvwEqpId[byTvwNotify] = byEqpId;
						break;
					}
				}
			}
			
			//zjj20091102�ڵ���ģʽ�£�����û�����ǽ����������ǽ�е��ն˾�Ҫ�ָ�����ϯ
			/*if( 0 == byCanStopToTvWall && VCS_SINGLE_MODE == byNewMode )
			{						
				if( !acTvwChnnlFind[byLoop].GetMember().IsNull() )
				{						
					tDropMt = GetLocalMtFromOtherMcuMt( (TMt)acTvwChnnlFind[byLoop].GetMember() );
					
					tSwitchInfo.SetSrcMt( m_tConf.GetChairman() );
					tSwitchInfo.SetDstMt( tDropMt );						
					tSwitchInfo.SetMode( MODE_BOTH );						
					VCSConfSelMT( tSwitchInfo );
				}
			}*/			
		}

		// �����ϱ�tvwall״̬
		TPeriEqpStatus tTvwallStatus;
		for ( byLoop = 0; byLoop < 128; ++byLoop )
		{
			if ( abyTvwEqpId[byLoop] 
				&& g_cMcuVcApp.GetPeriEqpStatus(abyTvwEqpId[byLoop], &tTvwallStatus))
			{
				cServMsg.SetMsgBody((u8 *)&tTvwallStatus, sizeof(tTvwallStatus));
				SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
			}else
			{
				break;
			}
		}
	}

	BOOL32 bPollStart = m_cVCSConfStatus.GetChairPollState() == VCS_POLL_START;
	// �����֮���л�, ��ѯҪֹͣ
	bPollStart = bPollStart && ISGROUPMODE(byOldMode) && ISGROUPMODE(byNewMode);

	// �ͷ�ѡ���ն�
	/*for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		// [8/29/2011 liuxu] ��������֮���л�ʱ,��ϯ��ѯ���ܱ���
		TMt tLoopMt = m_ptMtTable->GetMt(byMtId);
		if (bPollStart && tLoopMt == m_tConf.GetChairman())
		{
			continue;
		}

		if (m_tConfAllMtInfo.MtJoinedConf(byMtId) && 
			//CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtId) &&
			MT_TYPE_MT ==m_ptMtTable->GetMtType(byMtId) &&
			byMtId != m_tConf.GetChairman().GetMtId())
		{
			VCSConfStopMTSel(tLoopMt, MODE_BOTH);
		}
	}*/

	//zjj20101222 �����һЩ�����Ƚ���,����������������������,�����ȱ�ģʽ������,֮��Ĳ���mcu�ں�̨��
	if( ISGROUPMODE(byOldMode) && !ISGROUPMODE(byNewMode) )
	{
		m_cVCSConfStatus.SetCurVCMode(byNewMode);
		// ͨ������е�VCS����״̬�ĸı�
		VCSConfStatusNotif();
	}

	u16 wMcuIdx = INVALID_MCUIDX;
	// ������Դ���ͷ�
	// �����ģʽ���л����Ҷ����зǶ�ʱ���еĵ�����Դ
	if (!(ISGROUPMODE(byNewMode)  && ISGROUPMODE(byOldMode)))
	{
		TMt tMt;
		BOOL32 bIsNotInTWorHdu = FALSE;
		for( u8 byIndex = 1; byIndex <= MAXNUM_CONF_MT; byIndex++ )
		{	
			tMt = m_ptMtTable->GetMt(byIndex);
			//zhouyiliang 20110119 ��ϯ�ͱ�����ϯ��ʹ������ҲҪ���ֶ�ʱ����
			if(!m_tConfAllMtInfo.MtJoinedConf(byIndex) && 
				!(tMt == m_cVCSConfStatus.GetVcsBackupChairMan()) &&
				!(tMt == m_tConf.GetChairman())
			  )
			{
				m_ptMtTable->SetCallMode( byIndex,CONF_CALLMODE_NONE );
				continue;
			}
			bIsNotInTWorHdu = IsMtNotInOtherTvWallChnnl( byIndex,0,0 ) && IsMtNotInOtherHduChnnl( byIndex,0,0 );
			if ( m_tConf.GetChairman().GetMtId() != byIndex &&//CONF_CALLMODE_NONE == m_ptMtTable->GetCallMode(byIndex) &&
				m_ptMtTable->GetMtType(byIndex) != MT_TYPE_MMCU &&
				//zjj20091025 ���������������ڵ���ǽ�е��ն˲��Ҷ�
				/*IsMtNotInOtherTvWallChnnl( byIndex,0,0 ) &&
				IsMtNotInOtherHduChnnl( byIndex,0,0 )*/
				bIsNotInTWorHdu
				//zhouyiliang 20100824 vcs���ص�ַ1+1���ݵ�mt����drop
				&& m_cVCSConfStatus.GetVcsBackupChairMan().GetMtId() != byIndex
				) 
			{				
				VCSDropMT(tMt);
			}
			else if (!bIsNotInTWorHdu)
			{
				// ��hdu�У����Ҷϣ����ѡ�������������ж෽�໭�棬������ն��ڵ���ǽ���Ҷϣ������ն�һֱѡ����ϯ��δ��vmp
				VCSConfStopMTSel(tMt, MODE_BOTH);
			}
			/*else if (MT_TYPE_SMCU == m_ptMtTable->GetMtType(byIndex))
			{
				TMt tSMt;
				wMcuIdx = GetMcuIdxFromMcuId( byIndex );
				//���ߵ�SMCU,�Ҷ��������еķǶ�ʱ���еĵ�����Դ
				TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);//byIndex);
				if (NULL == ptMcInfo)
				{
					continue;
				}
				for (u8 bySIndex = 1; bySIndex <= MAXNUM_CONF_MT; bySIndex++)
				{
					tSMt.SetMcuId(byIndex);
					tSMt.SetMtId(bySIndex);
					if (m_tConfAllMtInfo.MtJoinedConf(wMcuIdx, bySIndex) //&& 
						//CONF_CALLMODE_NONE == VCSGetCallMode(tSMt)
						)
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
			}*/
		}
	}

	if ((ISGROUPMODE(byOldMode) && !ISGROUPMODE(byNewMode)) ||
		CONF_CREATE_MT == m_byCreateBy)
	{
		RlsAllBasForConf();
	}

	// ���������
	ChangeSpeaker(NULL, FALSE, FALSE);

	// [1/24/2011 liuxu][���]ֹͣ����¼��
	if ((ISGROUPMODE(byOldMode) && !ISGROUPMODE(byNewMode))
		&& !m_tConf.m_tStatus.IsNoRecording())
	{
		//send it to recorder
		TMt tConfMt;
		tConfMt.SetNull();

		cServMsg.SetChnIndex( m_byRecChnnl );
		cServMsg.SetMsgBody( (u8*)&tConfMt, sizeof( tConfMt ) );
		cServMsg.CatMsgBody( (u8*)&m_tRecEqp, sizeof( m_tRecEqp ) );
		SendMsgToEqp( m_tRecEqp.GetEqpId(), MCU_REC_STOPREC_REQ, cServMsg );
	}

	if( !m_cVCSConfStatus.GetCurVCMT().IsNull() )
	{
		NotifyMtSpeakStatus( m_cVCSConfStatus.GetCurVCMT(), emCanceled );
	}

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
		// miaoqingsong [05/12/2011] ����Ƿ����ڵ���ǽ�л����ն˵��жϣ����������л�һ��Ҫ��ǰһ��
        // �ĵ����ն��˳�����,������ֻ����Ǳ��صģ���Ϊ���صĻ��������ChangeSpecMixMember���������
		TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();
		m_cVCSConfStatus.VCCRestoreGroupStatus();		
		
		if( m_tConf.m_tStatus.IsMixing() &&
			!tCurVCMT.IsNull() &&
			!tCurVCMT.IsLocal() &&
			( ( IsMtNotInOtherHduChnnl( tCurVCMT,0,0 ) &&
			IsMtNotInOtherTvWallChnnl( tCurVCMT,0,0 ) ) ||
			!m_cVCSConfStatus.GetMtInTvWallCanMixing() )
			//!m_ptMtTable->IsMtInHdu( m_cVCSConfStatus.GetCurVCMT().GetMcuId() ) &&
			//!m_ptMtTable->IsMtInTvWall( m_cVCSConfStatus.GetCurVCMT().GetMcuId() ) 	
		  )

		{
			RemoveSpecMixMember( &tCurVCMT, 1, FALSE );
			if (!tCurVCMT.IsLocal() && 
				(!m_cVCSConfStatus.GetMtInTvWallCanMixing() || !IsHasMtInHduOrTwByMcuIdx(tCurVCMT.GetMcuIdx())))
			{
				TMt tLocalMt = GetLocalMtFromOtherMcuMt(tCurVCMT);
				RemoveSpecMixMember(&tLocalMt,1,FALSE,FALSE);
			}
		}
	}
	else
	{
		m_cVCSConfStatus.VCCRestoreStatus();
		//zhouyiliang 20100917 ���������л����෽�໭��,���ݵ�ǰ�����ն˵�����ǰ����ͨ����,��Ϊ�������ն��ڵ���ǽ�У�û�йҶ�
		if (VCS_SINGLE_MODE == byOldMode && VCS_MULVMP_MODE == byNewMode ) 
		{
			u8 byOnLineMtNum = 1;
			for( u8 byIndex = 1; byIndex <= MAXNUM_CONF_MT; byIndex++ )
			{
				if ( !m_tConfAllMtInfo.MtJoinedConf(byIndex) 
				|| m_cVCSConfStatus.GetVcsBackupChairMan().GetMtId() == byIndex 
				|| m_tConf.GetChairman().GetMtId() == byIndex) 
				{
					continue;
				}
				byOnLineMtNum++;
			}
			m_cVCSConfStatus.SetCurUseVMPChanInd( byOnLineMtNum );
		}
	}

	//zjj20091102 ����ǵ������ȶ������˵���ǽһ������Ҳ��Ҫ�����
	if (ISGROUPMODE(byNewMode) 
		|| VCS_MULTW_MODE == byNewMode 
		|| VCS_MULVMP_MODE == byNewMode 
		|| ( VCS_SINGLE_MODE == byNewMode && m_cVCSConfStatus.GetMtInTvWallCanMixing() )
		)
	{
		TMt tChairMan = m_tConf.GetChairman();
		if (!m_tConf.m_tStatus.IsMixing())
		{
			// [miaoqingsong 20111031] ��ӻ�����״̬��ӡ
// 			for ( u8 byLoopEqpId = 1; byLoopEqpId <= MAXNUM_MCU_PERIEQP ; byLoopEqpId++ )
// 			{
// 				u8 byMixerSubtype = UNKONW_MIXER;
// 				g_cMcuAgent.GetMixerSubTypeByEqpId(byLoopEqpId,byMixerSubtype);
// 				if( g_cMcuVcApp.IsPeriEqpValid( byLoopEqpId ) && 
// 					(byMixerSubtype == EAPU_MIXER || byMixerSubtype == APU2_MIXER || byMixerSubtype == MIXER_8KI)&&
// 					g_cMcuVcApp.m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus.m_byOnline )
// 				{
// 					OspPrintf( TRUE, FALSE, "[RestoreVCConf] EMixer Group's status is %d!\n", 
// 						g_cMcuVcApp.m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[0].m_byGrpState );    
// 				}     
// 			}

			// ���ģʽ���������ƻ���, �෽�໭������ǽʹ��ȫ�����
			BOOL32 dwStartResult = FALSE;

			//tianzhiyong 2010/03/21 ���ӿ�����ʽ�Ϳ���ģʽ
			dwStartResult = StartMixing(mcuPartMix);
			if (!dwStartResult)
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[RestoreVCConfInGroupMode] Find no mixer\n");
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
			else
			{
				// ��ʼ״ֻ̬����ϯ���������
				for (u8 byMixMtId = 1; byMixMtId <= MAXNUM_CONF_MT; byMixMtId++)
				{
					if (tChairMan.GetMtId() == byMixMtId)
					{
						m_ptMtTable->SetMtInMixing(byMixMtId, TRUE, TRUE);
					}
					else if( (m_ptMtTable->IsMtInTvWall( byMixMtId ) ||
						m_ptMtTable->IsMtInHdu( byMixMtId ) ) &&
						m_ptMtTable->GetMtType( byMixMtId ) != MT_TYPE_SMCU
						)
					{
						m_ptMtTable->SetMtInMixing(byMixMtId, TRUE, TRUE);
					}
					else
					{
						if( m_ptMtTable->GetMtType( byMixMtId ) != MT_TYPE_SMCU )
						{
							m_ptMtTable->SetMtInMixing(byMixMtId, FALSE, TRUE);
						}
					}
				}
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
			//20091102 ���л���ģʽ�����ƻ���
			//if (ISGROUPMODE(byNewMode))
			//{
				m_tConf.m_tStatus.SetSpecMixing();
				ChangeSpecMixMember(&tChairMan, 1);
				if (!m_ptMtTable->IsMtInMixing(tChairMan.GetMtId()))
				{
					AddSpecMixMember(&tChairMan,1);
				}
			//}
			
			//else
			//{
			//	SwitchToAutoMixing();
			//}
		}
	}

	//�����ģʽ�е����ģʽ����������б�
	if (!ISGROUPMODE(byOldMode) && ISGROUPMODE(byNewMode))
	{
		ReLoadOrigMtList();
	}
	if( ISGROUPMODE(byOldMode) && !ISGROUPMODE(byNewMode) )
	{
		ReLoadOrigMtList( FALSE );
	}

	//zbq [05/13/2010] ���ģʽ�л��������ģʽ����շ���/���������б�
    if ( (!ISGROUPMODE(byOldMode) && ISGROUPMODE(byNewMode)) ||
		(ISGROUPMODE(byOldMode) && !ISGROUPMODE(byNewMode))		
		)
    {
        m_tApplySpeakQue.ClearQueue();
        NotifyMcsApplyList( TRUE );
		m_cVCSConfStatus.SetConfSpeakMode( CONF_SPEAKMODE_NORMAL );
		m_tConf.SetConfSpeakMode( CONF_SPEAKMODE_NORMAL );
		m_cVCSConfStatus.ClearAllNewMt();
    }

	//�뿪������ģʽ��ҲҪ�������״̬
	if( byOldMode == VCS_GROUPSPEAK_MODE &&
		m_cVCSConfStatus.GetConfSpeakMode() == CONF_SPEAKMODE_ANSWERINSTANTLY &&
		byOldMode != byNewMode )
	{
		m_tApplySpeakQue.ClearQueue();
        NotifyMcsApplyList( TRUE );
		m_cVCSConfStatus.SetConfSpeakMode( CONF_SPEAKMODE_NORMAL );
		m_tConf.SetConfSpeakMode( CONF_SPEAKMODE_NORMAL );		
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
	//zjj20091102 �����������ģʽ��ҲҪ����ϳ�
	case VCS_GROUPROLLCALL_MODE:
		{
			//���黭��ϳɲ����ָ�
			//TConfAttrb tConfattrb = m_tConf.GetConfAttrb();
			TVMPParam_25Mem tVmpParam = GetVcsRestoreConfVmpParam();
			/*if (tConfattrb.IsHasVmpModule())
			{
				tVmpParam = GetVmpParam25MemFromModule();
				/*TVmpModule tModule = GetVmpModule();
				TVMPParam tTmpParam = tModule.GetVmpParam();
				memcpy(&tVmpParam, &tTmpParam, sizeof(TVMPParam));*
				//(TVMPParam)tVmpParam = tModule.GetVmpParam();//��ʱ��ʽ����ģ��֧�ֻ�VCS���Ե���������ģ�淽ʽ����
			}
			else
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuVCModeChgReq] must has vmp module currently\n");
				break;						
			}*/

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
					tVmpState.m_tStatus.tVmp.SetVmpParam(tVmpParam);
					g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpState);
					//m_tConf.m_tStatus.SetVmpParam(tVmpParam);
					AdjustVmpParam(m_tVmpEqp.GetEqpId(), &tVmpParam);
				}
				else
				{
					/*SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, TIMESPACE_WAIT_VMPRSP);
					tVmpState.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_START;
					g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpState);*/
					
					tVmpState.m_tStatus.tVmp.SetVmpParam(tVmpParam);
					g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpState);
					// ��������ϳ�
					//m_tConf.m_tStatus.SetVmpParam(tVmpParam);
					AdjustVmpParam(m_tVmpEqp.GetEqpId(), &tVmpParam, TRUE);
				}

				// ����Զ��໭��ʱ,��ϯ��vmp,�е�����˫����ʱҪ���vmp����ϯ�Ľ���
				//���������ģʽ���е�����˫����ҲҪ�����˵���ϯ�Ľ�����changespeaker null��vcs���鲻RestoreAllSubMtJoinedConfWatchingSrcMt��
				if ( (VCS_GROUPVMP_MODE == byOldMode || VCS_GROUPSPEAK_MODE == byOldMode )
					&& VCS_GROUPROLLCALL_MODE == byNewMode 
					&& m_cVCSConfStatus.GetChairPollState() != VCS_POLL_START )
				{
					TMt tChairMan = m_tConf.GetChairman();
					StopPrsMemberRtcp(tChairMan, MODE_VIDEO);
					StopSwitchToSubMt(1, &tChairMan, MODE_VIDEO);
				}

				//zhouyiliang 20101224 ����˫����ʱ��ϯ��������ϳɣ��е��Զ��໭���ʱ��Ҫ��һ������ϳɵ���ϯ�Ľ���
				if ( VCS_GROUPROLLCALL_MODE == byOldMode 
					&& VCS_GROUPVMP_MODE == byNewMode 
					&& tVmpParam.IsVMPBrdst() 
					&& m_cVCSConfStatus.GetChairPollState() != VCS_POLL_START )
				{
					/*u8 byVmpSubType = GetVmpSubType(m_tVmpEqp.GetEqpId());
					u8 byMaxOutChnl = MAXNUM_MPU_OUTCHNNL;
					if (MPU2_VMP_ENHACED == byVmpSubType ||
						MPU2_VMP_BASIC == byVmpSubType)//MPU2������������
					{
						byMaxOutChnl = m_tVMPOutParam.GetVmpOutCount();
					}
					u8  bySrcChnnl = GetVmpOutChnnlByDstMtId( m_tConf.GetChairman().GetMtId() );
					if ( 0xFF == bySrcChnnl ) 
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[RestoreVCConf]get chairman:%d vmp out channl failed!\n",  m_tConf.GetChairman().GetMtId());
					}
					else if(bySrcChnnl < byMaxOutChnl)
					{
						//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
						//StartSwitchToSubMt(GetLocalVidBrdSrc(), bySrcChnnl, m_tConf.GetChairman().GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode ��Ĭ��ֵ����SWITCH_MODE_SELECT
						TSwitchGrp tSwitchGrp;
						tSwitchGrp.SetSrcChnl(bySrcChnnl);
						tSwitchGrp.SetDstMtNum(1);
						TMt tChairman = m_tConf.GetChairman();
						tSwitchGrp.SetDstMt(&tChairman);
						StartSwitchToAll(GetLocalVidBrdSrc(), 1, &tSwitchGrp, MODE_VIDEO, SWITCH_MODE_SELECT);
					}	*/
					SwitchNewVmpToSingleMt( m_tConf.GetChairman() );
				}
				if( VCS_GROUPROLLCALL_MODE == byNewMode && m_cVCSConfStatus.GetChairPollState() != VCS_POLL_START )
				{
					NotifyMtReceive( m_tConf.GetChairman(),m_tConf.GetChairman().GetMtId() );
				}
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[RestoreVCConf]Hasn't reserve vmp for conf(confidx:%d)\n",
					      m_byConfIdx);
			}
		}			
		break;

	case VCS_GROUPSPEAK_MODE:
	case VCS_GROUPTW_MODE:
	case VCS_GROUPCHAIRMAN_MODE:
		// ��ϯ��Ϊ������
		{
			TMt tSpeaker = m_tConf.GetChairman();
			ChangeSpeaker(&tSpeaker, FALSE, FALSE);
		}
		break;
	default:
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[RestoreVCConf] uncorrect vcmode(%d)\n", byNewMode);
	    break;
	}
	
	m_cVCSConfStatus.SetCurVCMode(byNewMode);

	// ͨ������е�VCS����״̬�ĸı�
	VCSConfStatusNotif();

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[RestoreVCConf] restore vcsconf for mode(%d) end\n", byNewMode);
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
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[SwitchToAutoMixing] fail to automixing because conf mix status is %d, not in mixing\n",
			   m_tConf.m_tStatus.GetMixerMode());
		return FALSE;
	}

	m_tConf.m_tStatus.SetAutoMixing();
	TMt tMt;
	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
		{
			if (m_ptMtTable->GetMtType(byMtId) == MT_TYPE_VRSREC)
			{
				//����vrs��¼��ʵ��
				continue;
			}
			tMt = m_ptMtTable->GetMt(byMtId);
			AddMixMember(&tMt);
			StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
								 (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt)), 
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
		tMt = m_ptMtTable->GetMt(GetFstMcuIdFromMcuIdx(tMt.GetMcuId()));
	}

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[VCSMTMute] TMt(mcuid:%d mtid:%d) OpenFlag:%d MuteType:%d\n", tMt.GetMcuId(), tMt.GetMtId(), byMuteOpenFlag, byMuteType);

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
	}else if (byVendor == MT_MANU_KDCMCU)
	{
		TMsgHeadMsg tHeadMsg;
		memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
		tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
		
		CServMsg cMsg;
		cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );

		cMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		cMsg.CatMsgBody( (u8 *)&byMuteOpenFlag, sizeof(byMuteOpenFlag));
		cMsg.CatMsgBody( (u8 *)&byMuteType, sizeof(byMuteType));
		
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "Req to mute mcu[%d,%d]\n", tMt.GetMcuId(), tMt.GetMtId());
		
		SendMsgToMt( (u8)tMt.GetMtId(), MCU_MCU_MTAUDMUTE_REQ, cMsg );

		//������KEDA�նˣ�KEDAMCU��Tandberg������������
		//tMt = GetLocalMtFromOtherMcuMt(tMt);
        //AdjustKedaMcuAndTaideMuteInfo(&tMt, byMuteType, byMuteOpenFlag);
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
u8 CMcuVcInst::GetVCSAutoVmpMember(u8 byVmpId, TMt* ptVmpMember)
{
   // ��ȡ���������
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	u8 byVmpCapChlNum = MAXNUM_SDVMP_MEMBER; // [12/21/2009 xliang] 8KEд��16
#else
    u8 byVmpCapChlNum = CMcuPfmLmt::GetMaxCapVMPByConfInfo(m_tConf);
#endif
	// ռ�õ�vmp�豸����֧�ֵ������
    TPeriEqpStatus tPeriEqpStatus;
    g_cMcuVcApp.GetPeriEqpStatus(byVmpId , &tPeriEqpStatus);
    u8 byEqpMaxChl = tPeriEqpStatus.m_tStatus.tVmp.m_byChlNum;
	// ��ǰ���֧�ֽ��뻭��ϳɵ��ն���
	u8 byMaxMemberNum  = min(byVmpCapChlNum, byEqpMaxChl); 

	//zjj20091102 ���������������������ϳ�ģʽ���֧��4����
	if( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE &&
		byMaxMemberNum > 4 )
	{
		byMaxMemberNum = 4;
	}
	
	//zjj20091102 ������������������������ģʽ���֧��2����
	if( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE &&
		byMaxMemberNum >= 1 )
	{
		if( m_cVCSConfStatus.GetReqVCMT().IsNull()
			&& m_cVCSConfStatus.GetCurVCMT().IsNull() ) // �����ҵ�ǰû�����ڵ��ȵ��ն�ʱ�����Ŷ���1����
		{
			byMaxMemberNum = 1;
		}
		else
		{
			byMaxMemberNum = 2;
		}
		
	}


	u8 byVmpStyle = GetVmpDynStyle(byVmpId, byMaxMemberNum);

// 	if (m_tVmpEqp.GetEqpId() > (VMPID_MIN + 8))
// 	{
// 		byVmpStyle = VMP_STYLE_FOUR;
// 	}

	if (VMP_STYLE_NONE == byVmpStyle)
	{
		return byVmpStyle;
	}


	TVMPParam_25Mem tVmpParam;
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

	// ConfVmpParam�����������⴦��ͨ��EqpstatusVmpParam��Ϣ��ó�Ա��Ϣ,RestoreVCConf�������⴦��
	TVMPParam_25Mem  tLastVMPParam   = tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam();
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[GetVCSAutoVmpMember]last vmp param: \n");
	if(g_bPrintEqpMsg)
	{
		tLastVMPParam.Print();
	}

	// ����֮ǰ���õĻ���ϳɳ�Ա
	TMt tMt;
	TVMPMember *ptMember = NULL;  
	u8 byLastMemNum = tLastVMPParam.GetMaxMemberNum();
	for (u8 byLoop = 0; byLoop < byLastMemNum; byLoop++)
	{
		tMt.SetNull();
		ptMember = tLastVMPParam.GetVmpMember(byLoop);
		memcpy(&tMt, ptMember, sizeof(TMt));
		if (tMt.IsNull() || tMt.GetMtId() > MAXNUM_CONF_MT)
		{
			continue;
		}
		//zhouyiliang 20100824 �Զ�����ϳɵ�ʱ�򱸷��ն˲����Զ�����ϳ�
		if ( m_tConf.GetChairman() == tMt || m_cVCSConfStatus.GetVcsBackupChairMan() == tMt )
		{
			continue;
		}
		// ��ֹ�������ǽ���뻭��ϳ���
		// ����VCS���鶨ʱ���е���ԴΪ�̶���Դ
		/*if (CONF_CALLMODE_TIMER == VCSGetCallMode(tMt))
		{
			continue;
		}
		*/
		

		// �ǻش�ͨ���е��¼��ն˲�������ϳɳ�Աͳ��
		// ��Ϊ�����ն���Ҫ�Ž�����ϳɵ���������ն�ͬ���������ն�����һ�εĻ���ϳɳ�Ա��,�Ǿͻ�ռ���Ǹ�ͨ��
		if (!tMt.IsLocal()  )
		{
			if(!m_tConfAllMtInfo.MtJoinedConf(tMt))
			{
				continue;
			}

			if( !IsLocalAndSMcuSupMultSpy( tMt.GetMcuId() ) )
			{
				//zhouyiliang20101115���ش���tMt��curvcmt����ͬһ���նˣ�������ͬ��һ��mcu�£��滻
				u8 byMtFstMcuId = GetFstMcuIdFromMcuIdx(tMt.GetMcuId());
				u8 byCurFstMcuId = GetFstMcuIdFromMcuIdx( m_cVCSConfStatus.GetCurVCMT().GetMcuId());
//				TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo( byMtFstMcuId );
//				if ( NULL != ptMcInfo &&
//					!(ptMcInfo->m_tMMcuViewMt == tMt) 
//					)
//				{
				//zhouyiliang 20101112��GetFstMcuIdFromMcuIdx���ж��Ƿ���ͬһmcu�µ��նˣ����ܵ�3���͵�2�����滻��
				if( !(tMt == m_cVCSConfStatus.GetCurVCMT()) &&
					byMtFstMcuId == byCurFstMcuId 
					)
				{
					atVmpMember[byChnlNum].SetNull();
					atVmpMember[byChnlNum] = m_cVCSConfStatus.GetCurVCMT();
					byChnlNum++;
					continue;
				}					
				//}
			}			
		}
		

		if (m_tConfAllMtInfo.MtJoinedConf(tMt.GetMcuId(), tMt.GetMtId()))
		{
			atVmpMember[byChnlNum++] = tMt;
		}
	}

	//u8 byMtId    = 1;
	//u8 byMaxMtId = MAXNUM_CONF_MT;
//	u8 byChannel = 0;
	if( !ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() ) )
	{
		for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
		{
			if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
			{
				if (byMtId == m_tConf.GetChairman().GetMtId()
				 || tLastVMPParam.IsMtInMember(m_ptMtTable->GetMt(byMtId))
				 // ���ն˺���ͨ��δ�򿪣����ð�ռ����ϳ���ͨ��
				 || !m_ptMtTable->IsLogicChnnlOpen(byMtId, LOGCHL_VIDEO, FALSE)
				 || byMtId == m_cVCSConfStatus.GetVcsBackupChairMan().GetMtId()
				 || byMtId == m_cVCSConfStatus.GetCurVCMT().GetMtId() //zhouyiliang 20100915��ǰ�����ն��ں������CurUseVMPChanInd��
				 || m_ptMtTable->GetMtType(byMtId) == MT_TYPE_VRSREC //vrs��¼����ռ�ϳ�ͨ��
				 )
				{
					continue;
				}				
				atVmpMember[byChnlNum++] = m_ptMtTable->GetMt(byMtId);
			}
		}

	}


	if( VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode() ||
		VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() ||
		VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode()
		)
	{
		u8	byChannel = 0;
		u8 byIsFind = 0;
		TMt tReqMt = m_cVCSConfStatus.GetReqVCMT();
		if ( VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() )
		{	
			tReqMt = m_cVCSConfStatus.GetCurVCMT();

		}
		//zhouyiliang 20100911curvcmt�ȵ�presetinack��ʱ�����裬��ʱ��reqvcmt
		if( !/*m_cVCSConfStatus.GetCurVCMT()*/tReqMt.IsNull() &&
			m_tConfAllMtInfo.MtJoinedConf( /*m_cVCSConfStatus.GetCurVCMT()*/tReqMt.GetMcuId(),
										/*m_cVCSConfStatus.GetCurVCMT()*/tReqMt.GetMtId() )
			)
		{
		
			for(u8 byIdx = 0; byIdx < byMaxMemberNum; byIdx++)
			{
				//ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[GetVCSAutoVmpMember] Now vmp Member mcuid:%d mtid:%d\n", 
				//		   atVmpMember[byIdx].GetMcuId(), atVmpMember[byIdx].GetMtId());
				if( atVmpMember[byIdx] == tReqMt/*m_cVCSConfStatus.GetCurVCMT()*/ )
				{
					byIsFind = 1;
					break;
				}
			}		
			
			if( 0 == byIsFind )
			{
				byChannel = (u8)m_cVCSConfStatus.GetCurUseVMPChanInd();
				if( byChannel >= byMaxMemberNum )
				{
					byChannel = 1;
				}
				atVmpMember[ byChannel ] = tReqMt/*m_cVCSConfStatus.GetCurVCMT()*/;
				++byChannel;
				++byChnlNum;
				if( byChannel >= byMaxMemberNum )
				{
					byChannel = 1;
				} //�ӱ���,����ģʽ,ֻ���µ�1·ͨ��,vpumap����ʱ,��֧��һ��һС����,byMaxMemberNum������2
				else if (VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode() && byChannel > 1)
				{
					byChannel = 1;
				}
				m_cVCSConfStatus.SetCurUseVMPChanInd( byChannel );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[GetVCSAutoVmpMember] Update vcs CurUseVMPChanInd.%d:\n",byChannel);
			}

		}			
	
	}

	u8 byFinalMemNum = min(byChnlNum, byMaxMemberNum);
	if (ptVmpMember != NULL)
	{
		memset(ptVmpMember, 0, sizeof(TMt) * MAXNUM_VMP_MEMBER);
		memcpy(ptVmpMember, atVmpMember, sizeof(TMt) * byFinalMemNum);
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[GetVCSAutoVmpMember] final vcs autovmp member:\n");
		if (g_bPrintEqpMsg)
		{
			for(u8 byIdx = 0; byIdx < byFinalMemNum; byIdx++)
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "%d.mcuid:%d mtid:%d\n", 
						  byIdx, ptVmpMember[byIdx].GetMcuId(), ptVmpMember[byIdx].GetMtId());
			}
		}

	}

	byVmpStyle = GetVmpDynStyle(byVmpId, byFinalMemNum);
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[GetVCSAutoVmpMember] final autovmp style: %d\n", byVmpStyle);

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
	// �˽ӿ�δ��ʹ�ã�����
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
	TVMPParam_25Mem  tNewVMPParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);

	// ������֯�ĳ�Ա������������Ӧ��ͨ����
	for (byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++)
	{
		TMt tMt = pVmpMember[byLoop];
		if (!tMt.IsNull())
		{
			SetVmpChnnl(m_tVmpEqp.GetEqpId(), tMt, byLoop, VMP_MEMBERTYPE_VAC);
			
			tNewVMPParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
			// �����ն˵ķֱ���
			ChangeMtVideoFormat(tMt);
		}
		else
		{
			// ��ԭ�����ն�
			TMt tOldMt = (TMt)(*tLastVMPParam.GetVmpMember(byLoop));
			if (!tOldMt.IsNull())
			{
				StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
			}
			
		}
	}

	// �Ҷϱ��߳�����ϳ����ĳ�Ա
	if ( CONF_VMPMODE_NONE != tLastVMPParam.GetVMPMode() )
	{
		for( s32 nIndex = 0; nIndex < tLastVMPParam.GetMaxMemberNum(); nIndex ++ )
		{
			TMt tVMPMemberOld = (TMt)(*tLastVMPParam.GetVmpMember(nIndex));
        
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
	if ( VCS_CONF == m_tConf.GetConfSource() && 
		m_byCreateBy != CONF_CREATE_MT )
	{
		//zhouyiliang 20100818 vcs �л�������ַ��������ַ1+1���ݣ�
		BOOL32 bChange = VCSChangeChairMan();
		if ( !bChange &&  !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
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
void CMcuVcInst::ChangeVFormat(TMt tMt, u8 byStart, u8 byNewFormat, u8 byEqpId, u8 byVmpStyle, u8 byPos, u32 dwResW, u32 dwResH, u8 byHduChnId)
{

    if (tMt.IsNull() || tMt.GetType() == TYPE_MCUPERI)
    {
        ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[ChangeHDVFormat] dstmt.%d is not mt or kdc mt. type.%d\n",
                tMt.GetMtId(), tMt.GetType());
        return;
    }

	// [10/18/2011 liuxu] mcu�ն˵ķֱ��ʲ���Ҫ���е���
	// ת����������ʵ��ʵ���ϴ��ն�
	if (IsMcu(tMt) || !tMt.IsLocal())
	{
		CascadeAdjMtRes(tMt, byNewFormat, byStart, byVmpStyle, byPos, dwResW, dwResH, byEqpId, byHduChnId);
		return;
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ChangeVFormat] tMt.IsLocal() tMt(%d, %d) videoformat change to %d\n",
			    tMt.GetMcuId(), tMt.GetMtId(), byNewFormat);

    // ���ڸ����ն˷���������ǽ,��ȡ���ֱ���
    TLogicalChannel tSrcRvsChannl;
    if ( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
    {
		u8 byChnnlType   = tSrcRvsChannl.GetMediaType();
		u8 byChnnlFormat;
		if (byStart) 
		{
			byChnnlFormat = byNewFormat;
		}else
		{
			byChnnlFormat = tSrcRvsChannl.GetVideoFormat();
		}

		// [pengjie 2010/9/13] ��ش����ֱ����߼�����
        SendChgMtVidFormat( tMt.GetMtId(), byChnnlType, byChnnlFormat, FALSE, byStart, dwResW, dwResH);

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF,  "[ChangeVFormat] mt(%d) videoformat change to %d\n",
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
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ChangeSpecMixMember]mix has not started\n");
		return;
	}

	TMt tMt;
	u8  byInMixMem = FALSE;
	u8 byMtIdx = 0;
	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
		{
			continue;
		}

		byInMixMem = FALSE;
		for (byMtIdx = 0; byMtIdx < byMemberNum; byMtIdx++)
		{
			if ( GetLocalMtFromOtherMcuMt( ptMixMember[byMtIdx] ).GetMtId() == byMtId )
			{
				byInMixMem = TRUE;
				break;
			}
		}

		//���vcs�����Ѿ������ǽһ���������ڵ���ǽ�ڵ��ն�Ҳ���ڻ�����Ա��,�Ͳ����˳�������
		if( !byInMixMem && m_tConf.GetConfSource() == VCS_CONF &&
			m_cVCSConfStatus.GetMtInTvWallCanMixing() &&
			( m_ptMtTable->IsMtInHdu( byMtId ) || m_ptMtTable->IsMtInTvWall( byMtId ) )
			)
		{
			byInMixMem = TRUE;
		}


		tMt = m_ptMtTable->GetMt(byMtId);
		if (m_ptMtTable->IsMtInMixGrp(byMtId) && !byInMixMem && m_ptMtTable->GetMtType(byMtId) == MT_TYPE_MT)
		{
			RemoveSpecMixMember(&tMt, 1, FALSE,FALSE);
		}
		
		if (!m_ptMtTable->IsMtInMixGrp(byMtId) && byInMixMem)
		{
			AddSpecMixMember(&tMt, 1, TRUE);
		}

		for (byMtIdx = 0; byMtIdx < byMemberNum; byMtIdx++)
		{
			if( byInMixMem && !ptMixMember[byMtIdx].IsLocal() )
			{			
				AddSpecMixMember(&ptMixMember[byMtIdx], 1, TRUE);
			}
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
/*lint -save -e850*/
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

	u8 byLoopNum = 0;
	u8  byStartMtId   = byMtId;
	u8  byLoopRestart = FALSE; 
	TMt tNextPollMt;
	tNextPollMt.SetNull();
	TMtStatus tMtStatus;
	BOOL32 bLoop = TRUE;
	while ( bLoop ) 
	{
		for (; byMtId <= MAXNUM_CONF_MT; byMtId++)
		{
			if (byStartMtId == byMtId && byLoopRestart)
			{
				break;
			}

			//zjj 20090910		
			if( m_ptMtTable->GetMainType( byMtId ) != TYPE_MT )
			{
				continue;
			}

			//zjj 20090910		
			if( MT_TYPE_MMCU == m_ptMtTable->GetMtType( byMtId ) || 
				MT_TYPE_SMCU == m_ptMtTable->GetMtType( byMtId ) ||
				MT_TYPE_VRSREC == m_ptMtTable->GetMtType( byMtId ) //����vrs��¼��
				)
			{
				continue;
			}		
			
            // miaoqingsong [20110616] ���������жϣ�VCS������ϯ��ѯʱ���˵������ն�
			if (m_tConfAllMtInfo.MtJoinedConf((byMtId)) && 
				m_tConf.GetChairman().GetMtId() != byMtId && 
				m_cVCSConfStatus.GetVcsBackupChairMan().GetMtId() != byMtId//&& 
				//m_ptMtTable->GetCallMode(byMtId) != CONF_CALLMODE_TIMER
				)
			{				
				if( m_ptMtTable->GetMtStatus(byMtId, &tMtStatus) &&
					(tMtStatus.IsVideoLose() || MT_MANU_CHAORAN == m_ptMtTable->GetManuId(byMtId)) )
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_VCS, "[VCSGetNextPollMt] Mt.%d Is Lose Video,jump it\n",byMtId );
					continue;	
				}
				tNextPollMt = m_ptMtTable->GetMt(byMtId);
				break;
			}

			if (MAXNUM_CONF_MT == byMtId)
			{
				byMtId = 0;
				byLoopRestart = TRUE;
			}
	

		}

		if( !tNextPollMt.IsNull() )
		{
			break;
		}
		else if ( byMtId > MAXNUM_CONF_MT && byLoopNum < 1) 
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[VCSGetNextPollMt] byMtId > MAXNUM_CONF_MT\n" );
			byMtId = 1;
			byLoopNum++;
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[VCSGetNextPollMt] Get Next Poll Mt error !\n");
			break;
		}
		
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[VCSGetNextPollMt] Next Poll Mt(%d,%d) mainType.%d mtType.%d!\n",
		tNextPollMt.GetMcuId(),tNextPollMt.GetMtId(),m_ptMtTable->GetMainType(tNextPollMt.GetMtId()),
		m_ptMtTable->GetMtType( tNextPollMt.GetMtId() ));
	
	return tNextPollMt;
}
/*lint -restore*/

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
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsNeedHdSelApt] wrong input param about bySwitchMode(%d)\n", bySwitchMode);
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
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsNeedHdSelApt] polocom no need to use bas\n");
			return FALSE;
		}
		// ����ʽ��һ��
		else if (tSrcLogChnl.GetChannelType() != tDstLogChnl.GetChannelType())
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsNeedHdSelApt] src type.%d matchless with dst type.%d, ocuupy new adp!\n",
				    tSrcLogChnl.GetChannelType(), tDstLogChnl.GetChannelType());
            return TRUE;
		}
		else if (IsResG(tSrcLogChnl.GetVideoFormat(), tDstLogChnl.GetVideoFormat()))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsNeedHdSelApt] src res.%d matchless with dst res.%d, ocuupy new adp!\n",
				    tSrcLogChnl.GetVideoFormat(), tDstLogChnl.GetVideoFormat());
			return TRUE;
		}
		// zjj20090929��ʱԴ��Ŀ���ն�δ��������������Ϊ0��ȡ�������ʴ����ж�
		else if (m_ptMtTable->GetDialBitrate(bySrcMtId) > m_ptMtTable->GetDialBitrate(byDstMtId) &&
			     VCS_CONF == m_tConf.GetConfSource())
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsNeedHdSelApt] src bitrate.%d large than dst bitrate.%d!\n", 
				    m_ptMtTable->GetDialBitrate(bySrcMtId), m_ptMtTable->GetDialBitrate(byDstMtId));
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
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsNeedHdSelApt] src audio type.%d matchless with dst audio type.%d, ocuupy new adp!\n",
				    tSrcLogChnl.GetChannelType(), tDstLogChnl.GetChannelType());
			return TRUE;
		}
	}
	return FALSE;
}

/*====================================================================
    ������      ��ProcMMCUGetConfCtrl
    ����        �����ϼ�MCU�ӹܻ������,�ָ���������ģʽ,���������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/04/14                ���㻪        ����
====================================================================*/
void CMcuVcInst::ProcMMCUGetConfCtrlMsg()
{
	// �ָ��¼����鵽��������ģʽ,���޸Ļ��鴴����ʽ,��ֹ�¼�����
	RestoreVCConf(VCS_SINGLE_MODE);
	m_byCreateBy = CONF_CREATE_MT;

	u16 wErrCode;
	if (!PrepareAllNeedBasForConf(&wErrCode))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcMMCUGetConfCtrlMsg]not enough bas\n");
	}

	ReLoadOrigMtList();

	CServMsg cServMsg;
	TMt tCallMT;
	
	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (m_tConfAllMtInfo.MtInConf(byMtId) &&
			!m_tConfAllMtInfo.MtJoinedConf(byMtId)&&
			MT_TYPE_MMCU != m_ptMtTable->GetMtType( byMtId )
			)
		{   
			tCallMT = m_ptMtTable->GetMt(byMtId);
			InviteUnjoinedMt(cServMsg, &tCallMT);
		}	
	}
}

/*====================================================================
    ������      ��ReLoadOrigMtList
    ����        �����¼��ػ�������б�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/04/14                ���㻪        ����
====================================================================*/
void CMcuVcInst::ReLoadOrigMtList( BOOL32 bIsAdd /*= TRUE*/ )
{
	for (u8 byMtLoop = 1; byMtLoop <= MAXNUM_CONF_MT; byMtLoop++)
	{
		if (m_tConf.GetChairman().GetMtId() != byMtLoop &&
			m_ptMtTable->GetMt(byMtLoop).GetMtId() != 0 &&
			MT_TYPE_MMCU != m_ptMtTable->GetMtType( byMtLoop )
			&& m_cVCSConfStatus.GetVcsBackupChairMan().GetMtId() != byMtLoop )
		{
            TMt tMt = m_ptMtTable->GetMt(byMtLoop); 
			RemoveMt(tMt, FALSE,FALSE );
		}			
	}

	CServMsg cServMsg;	
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	if( bIsAdd )
	{
		TMtAlias* patMtAlias = NULL;
		u16* pawMtDialRate = NULL;
		u8 byMtNum = m_tOrginMtList.GetMtNum();
		for (u8 byMtIdx = 0; byMtIdx < byMtNum; byMtIdx++)
		{
			AddMt(m_tOrginMtList.GetMtAliasByIdx(byMtIdx), 
				  m_tOrginMtList.GetMtDialRateByIdx(byMtIdx),
				  CONF_CALLMODE_NONE);
		}
	}

	VCSConfStatusNotif();

	//������ػ��������ն���Ϣ	
	cServMsg.SetServMsg( NULL,0 );
	cServMsg.SetMsgBody( NULL,0 );
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//��������ն˱� 
	SendMtListToMcs(LOCAL_MCUIDX);
}

/*====================================================================
    ������      ��NotifyVCSPackInfo
    ����        ��ͨ����VCSָ������ϯ�ķ�����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/04/14                ���㻪        ����
====================================================================*/
void CMcuVcInst::NotifyVCSPackInfo(CConfId& cConfId, u8 byVcsId)
{
	if (cConfId.GetConfSource() != VCS_CONF)
	{
		return;
	}

	u8 byConfPos = MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE;
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	GetAllConfHeadFromFile(acConfId, sizeof(acConfId));
	for (s32 nPos = 0; nPos < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); nPos++)
	{
		if (acConfId[nPos] == cConfId)
		{
			byConfPos = (u8)nPos;
			break;
		}
	}

	if (byConfPos < (MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE))
	{
		CServMsg cServMsg;
		cServMsg.SetConfId(cConfId);

		u32 dwTotalLen = GetUnProConfDataToFileLen(byConfPos);
		u8  byPackNum = (u8)(dwTotalLen / MAX_VCSPACKIFNOLEN + ((dwTotalLen % MAX_VCSPACKIFNOLEN) ? 1 : 0));
		cServMsg.SetTotalPktNum(byPackNum);

		u32 dwOutLen   = 0;
		u32 dwBeginPos = 0;
		for (u8 byPackIdx = 0; byPackIdx < byPackNum; byPackIdx++)
		{
			dwOutLen = MAX_VCSPACKIFNOLEN;
			cServMsg.SetCurPktIdx(byPackIdx);
			cServMsg.SetMsgBody();
			GetUnProConfDataToFile(byConfPos, (s8*)cServMsg.GetMsgBody(), dwOutLen, dwBeginPos);
			dwBeginPos += dwOutLen;
			cServMsg.SetMsgBodyLen((u16)dwOutLen);
			if (byVcsId)
			{
				SendMsgToMcs(byVcsId, MCU_VCS_PACKINFO_NOTIFY, cServMsg);
			}
			else
			{
				SendMsgToAllMcs(MCU_VCS_PACKINFO_NOTIFY, cServMsg);
			}
		}
	}
}

/*====================================================================
    ������      HandleVCSAutoVmpSpyRepalce
    ����        ������vcs�Զ�����ϳɶ�ش���������ʱ���滻presetin����ȥ�Ŀ��ͷ��ն��߼�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const tPreSetInRsp, TPreSetInRsp����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
   2010-7-19                zhouyiliang     modify
====================================================================*/
void CMcuVcInst::HandleVCSAutoVmpSpyRepalce(TPreSetInRsp &tPreSetInRsp )
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

	if ( tPreSetInRsp.m_tSetInReqInfo.GetEvId() != MCS_MCU_STARTVMP_REQ 
		|| tSrc.IsNull() || tReleaseMt.IsNull() ) //�����Զ�����ϳɷ���presetin
	{
		return;
	}
	
	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	BOOL32 bIsVcsAuto = m_tConf.GetConfSource() == VCS_CONF && tConfVmpParam.IsVMPAuto();
	//zhouyiliang 20100813 �滻���ͷ��նˣ�m_byIsNeedRelease�ٱ���һ��
	if ( bIsVcsAuto && tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsNeedRelease )
	{
		u8 byReplaceVmpPos = 1;
		//���ҿ��滻�ն���vmpparam�е�ͨ����,�������return
		if ( !tConfVmpParam.FindVmpMember( tReleaseMt, byReplaceVmpPos ) ) 
		{
			return;
		}
	
		//������滻�Ĳ���presetinreqҪ����Ǹ�ͨ��,Ҫ�ָ�������ͻ���һ����Ա����style����		 
		if (tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos != byReplaceVmpPos )
		{
				TVMPParam_25Mem tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(m_tVmpEqp);
				TVMPMember tOldMember = *tLastVmpParam.GetVmpMember(/*1*/tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos);
				tConfVmpParam.SetVmpMember( tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos,tOldMember );
				u16 byCurUseChnId = m_cVCSConfStatus.GetCurUseVMPChanInd();
				u8 byMaxVmpMember = tConfVmpParam.GetMaxMemberNum();
				//�ϴα���Ҫ���õ�λ��Ϊ�գ����Ҫ�������λ�ã��ش����������ˣ����Բ��ܷ����λ�ã�styleӦ�ûָ��ϴε�
				if ( tOldMember.IsNull() )
				{
					u8 byStyl = tLastVmpParam.GetVMPStyle();
					tConfVmpParam.SetVMPStyle( byStyl );
					
				}	
				g_cMcuVcApp.SetConfVmpParam(m_tVmpEqp, tConfVmpParam);
				//�ָ�presetin֮ǰ��curUseVmpChanInd���籾��Ҫ��4���棬ͨ������ֻ��3���棬curUseVmpChan�Ͳ�����1������ԭ����3��
				byCurUseChnId = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos;
				m_cVCSConfStatus.SetCurUseVMPChanInd( byCurUseChnId );
		}

		tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos = byReplaceVmpPos;
	}
}

/*====================================================================
    ������      ��FindVCSCanReplaceSpyMt
    ����        ����һ���ܹ��滻��mt�����ҵ������ڶ�ش�ͨ�����������ͨ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
	              ��out��swCanReplaceSpyIndex ���滻�Ķ�ش�ͨ����
				  ��out��byCanReplaceChnnPos ���滻��ͨ���ţ�mt���ڵ�vmpͨ������hduͨ���ŵȣ�
    ����ֵ˵��  ��TMt  ���ؿ��滻��mt��IsNull��ʾû�ҵ����滻mt
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
   2010-7-19                zhouyiliang     modify
   2010-08-31               ������          modify
====================================================================*/
TMt CMcuVcInst::FindVCSCanReplaceSpyMt( u8 byMtInEqpType , const TMt& tSrc ,s16& swCanReplaceSpyIndex, u8& byCanReplaceChnnPos )
{
	// �˽ӿڲ��ٱ�ʹ�ã�����
	swCanReplaceSpyIndex = -1;
	TMt tCanReplaceMt;
	tCanReplaceMt.SetNull();
	if (m_tConf.GetConfSource() != VCS_CONF ) //�������vcs����
	{
		return tCanReplaceMt;
	}

	switch( byMtInEqpType )
	{
	case EQP_TYPE_VMP:
		{
			if ( !m_tConf.m_tStatus.m_tVMPParam.IsVMPAuto() )//vmp �Զ�����ϳ��滻����,�Զ�����ϳɲ����滻����
			{
				return tCanReplaceMt;
			}

			//zhoyiliang 20100716 �ҵ����Ƚ��뻭��ϳɵı�����ͬһ�¼����նˣ��滻��
			//zhouyiliang 20100831 �滻���Ըı�ԭ���滻ͬһ�¼��ն˸�Ϊ���滻���Ƚ��뻭��ϳɵ��Ǹ�mt
			u8 byMaxVmpMember = m_tConf.m_tStatus.m_tVMPParam.GetMaxMemberNum();
		
			//�ҵ�ͬ��mcu���һ������ϳɳ�Ա���滻���ȼ�Ϊ0 �ģ�����ϯ�⣩λ��
			//zhouyiliang 20100831 �滻���Ըı�ԭ���滻ͬһ�¼��ն˸�Ϊ���滻���Ƚ��뻭��ϳɵ��Ǹ�mt
			u8 byLowestPrioIndex = 1;
			for ( u8 byLoop = 1 ; byLoop < byMaxVmpMember ; byLoop++ )
			{
				TVMPMember tTempMember = *m_tConf.m_tStatus.m_tVMPParam.GetVmpMember( byLoop );
				//u8 byReplacePrio = m_cVCSConfStatus.GetVmpMemberReplacePrio(byLoop);
				if (tTempMember.IsNull()) //Ϊ�յ�����
				{
					continue;
				}
				TMt tTempMt = (TMt)(tTempMember);
				if ( /*tTempMt->GetMcuId() == tSrc.GetMcuId()
					&& tTempMt->GetMtId() != tSrc.GetMtId()*/ 
					!( tTempMt == tSrc ))
					//&& byReplacePrio == 0  ) 
				{
					byLowestPrioIndex = byLoop;
					break;
				}
			}

			//����byLowestPrioIndex�Ժ�Ŀ��滻�նˣ����Ƚ�vmpparam��ͬ���նˣ������û������֮ǰ��
			//zhouyiliang 20100831 �滻���Ըı�ԭ���滻ͬһ�¼��ն˸�Ϊ���滻���Ƚ��뻭��ϳɵ��Ǹ�mt
			BOOL32 bFind = FALSE;
			for ( u8 byLoopHigh = byLowestPrioIndex + 1 ; byLoopHigh < byMaxVmpMember ; byLoopHigh++ )
			{
				TVMPMember tTempMember = *m_tConf.m_tStatus.m_tVMPParam.GetVmpMember( byLoopHigh );
				if (tTempMember.IsNull()) //Ϊ�յ�����
				{
					continue;
				}
				TMt tTempMt = (TMt)(tTempMember);
				//zhouyiliang 20100831 �滻���Ըı�ԭ���滻ͬһ�¼��ն˸�Ϊ���滻���Ƚ��뻭��ϳɵ��Ǹ�mt
				if ( /* tTempMt->GetMcuId() == tSrc.GetMcuId()
					&& tTempMt->GetMtId() != tSrc.GetMtId()*/
					!( tTempMt == tSrc ) ) 
				{
					swCanReplaceSpyIndex = m_cSMcuSpyMana.FindSpyMt(tTempMt);
						
					//Ҫ�滻��vmpmember���ڻش�ͨ��������һ��
					if (-1 == swCanReplaceSpyIndex ) 
					{
						 continue;
					}
					//�ҵ��ˣ���������ڵ���ǽһ��������Ϊ���滻�ն�
					if(	IsMtNotInOtherHduChnnl( tTempMt,0,0 ) &&
							IsMtNotInOtherTvWallChnnl( tTempMt,0,0 ) )
					{
						 //�¼����vmpmember��λ�á�
						byCanReplaceChnnPos = byLoopHigh;
						bFind = TRUE;
						break;
					}				  
				}
			}

			//���û������֮ǰ��,��0������ϯ�����ñȽ�
			for ( u8 byLoopLow = 1 ; byLoopLow < byLowestPrioIndex && !bFind; byLoopLow++ )
			{
				TVMPMember tTempMember = *m_tConf.m_tStatus.m_tVMPParam.GetVmpMember( byLoopLow );
				if (tTempMember.IsNull()) //Ϊ�յ�����
				{
					continue;
				}
				TMt tTempMt = (TMt)(tTempMember);
				//zhouyiliang 20100831 �滻���Ըı�ԭ���滻ͬһ�¼��ն˸�Ϊ���滻���Ƚ��뻭��ϳɵ��Ǹ�mt
				if ( /*tTempMt->GetMcuId() == tSrc.GetMcuId()
					&& tTempMt->GetMtId() != tSrc.GetMtId()*/
					!( tTempMt == tSrc ) ) 
				{
					swCanReplaceSpyIndex = m_cSMcuSpyMana.FindSpyMt(tTempMt);
					//Ҫ�滻��vmpmember���ڻش�ͨ��������һ��
					if (-1 == swCanReplaceSpyIndex) 
					{
						 continue;
					}
					//�ҵ��ˣ���������ڵ���ǽһ��������Ϊ���滻�ն�
					if(	IsMtNotInOtherHduChnnl( tTempMt,0,0 ) &&
							IsMtNotInOtherTvWallChnnl( tTempMt,0,0 ) )
					{
					   //�¼����vmpmember��λ�á�
					   byCanReplaceChnnPos = byLoopLow;
						bFind = TRUE;
						break;
					}
				}
			}

			//���0��ǰ��û�����ȼ�Ϊ1�ģ����滻0����
			if (FALSE == bFind) 
			{
				TVMPMember tTempLowestMember = *m_tConf.m_tStatus.m_tVMPParam.GetVmpMember( byLowestPrioIndex );
				if ( !tTempLowestMember.IsNull()) //Ϊ�յ�����
				{
					 TMt tTempMt = (TMt)(tTempLowestMember);
					 //zhouyiliang 20100831 �滻���Ըı�ԭ���滻ͬһ�¼��ն˸�Ϊ���滻���Ƚ��뻭��ϳɵ��Ǹ�mt
					if ( /*tTempMt->GetMcuId() == tSrc.GetMcuId()
							&& tTempMt->GetMtId() != tSrc.GetMtId() */
							!( tTempMt == tSrc ) ) 
					{
						swCanReplaceSpyIndex = m_cSMcuSpyMana.FindSpyMt(tTempMt);
						//Ҫ�滻��vmpmember�ڻش�ͨ�����ҵ��ˣ���������ڵ���ǽһ��������Ϊ���滻�ն�
						if (-1 != swCanReplaceSpyIndex  && IsMtNotInOtherHduChnnl( tTempMt,0,0 ) &&
							IsMtNotInOtherTvWallChnnl( tTempMt,0,0 ) ) 
						{
							//�¼����vmpmember��λ�á�
							byCanReplaceChnnPos = byLowestPrioIndex;
							bFind = TRUE;
						}
					}
				}
			}

			//���滻mt
			if (-1 != swCanReplaceSpyIndex && bFind ) 
			{
				TVMPParam tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
				tCanReplaceMt = (TMt)(*tVmpParam.GetVmpMember( byCanReplaceChnnPos ));
			}
			
		}
		break;

	case EQP_TYPE_HDU:
		break;

	default:
		break;
	}

	return tCanReplaceMt;
}

/*====================================================================
    ������      ��VCSChangeChairMan
    ����        ��vcs������ַ1+1���ݣ��л����ߵ���ϯ��ַΪ������ϯ��ַ����ģʽ����ҵ��ָ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��TMt  ���ؿ��滻��mt��IsNull��ʾû�ҵ����滻mt
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
   2010-8-18                zhouyiliang     create
====================================================================*/
BOOL32 CMcuVcInst::VCSChangeChairMan( ) 
{
	BOOL32 bRet = FALSE;
	if ( m_tConf.GetConfSource() != VCS_CONF ) //����
	{
		return FALSE;
	}
	//����changechairman
	TMt tBackupChairman = m_cVCSConfStatus.GetVcsBackupChairMan();
	//�����ն�����
	if ( !tBackupChairman.IsNull() && m_tConfAllMtInfo.MtJoinedConf( tBackupChairman ) ) 
	{
		TMt toldChairMan = m_tConf.GetChairman();
		TMt tCurMt = m_cVCSConfStatus.GetCurVCMT();
		//	��ģʽ����ҵ��ָ�
		u8 byMode = m_cVCSConfStatus.GetCurVCMode();
		VCSMTMute(tBackupChairman, m_cVCSConfStatus.IsLocMute(), VCS_AUDPROC_MUTE);
		VCSMTMute(tBackupChairman, m_cVCSConfStatus.IsLocSilence(), VCS_AUDPROC_SILENCE);

		ChangeChairman( &tBackupChairman );
		m_cVCSConfStatus.SetVcsBackupChairMan( toldChairMan );


		switch (byMode)
		{
		case VCS_SINGLE_MODE:
			{
				//��������ģʽ������ϯ�ɹ��뵱ǰ�����ն˽�������ѡ��
				TSwitchInfo tSwitchInfo;
				
				//����ϯѡ����ǰ�����ն�
				if( !tCurMt.IsNull() )
				{
					VCSConfStopMTSel( toldChairMan, MODE_BOTH );
									
					tSwitchInfo.SetSrcMt( tCurMt );
					tSwitchInfo.SetDstMt( tBackupChairman );
					tSwitchInfo.SetMode(MODE_BOTH);
					VCSConfSelMT(tSwitchInfo);
				}
				
				//���������ն�ѡ������ϯ
				for ( u8 byLoopMt = 1 ; byLoopMt < MAXNUM_CONF_MT ; byLoopMt++ )
				{
					TMt tMt = m_ptMtTable->GetMt(byLoopMt);
					if ( tMt.IsNull() || tMt == tBackupChairman || tMt == toldChairMan ) 
					{
						continue;
					}
					if (!m_tConfAllMtInfo.MtJoinedConf(tMt) ) 
					{
						continue;
					}
					VCSConfStopMTSel( tMt, MODE_BOTH );	
					tSwitchInfo.SetSrcMt( tBackupChairman );
					tSwitchInfo.SetDstMt(  tMt );
					tSwitchInfo.SetMode(MODE_BOTH);
					VCSConfSelMT(tSwitchInfo);
				}
				break;
			}

		case VCS_GROUPROLLCALL_MODE:
		case VCS_GROUPVMP_MODE:
		case VCS_MULVMP_MODE:
			{
				//�෽�໭�桢����˫���桢�Զ�����ϳ�:����ϯ������ϳɼ�������
				//����˫���棬����ϯѡ����ǰ�����ն�
				if ( VCS_GROUPROLLCALL_MODE == byMode ) 
				{
					VCSConfStopMTSel( toldChairMan, MODE_VIDEO );	
					TSwitchInfo tSwitchInfo;
					tSwitchInfo.SetSrcMt( tCurMt );
					tSwitchInfo.SetDstMt( tBackupChairman );
					tSwitchInfo.SetMode( MODE_VIDEO );
					VCSConfSelMT(tSwitchInfo);
				}
				TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
				if ( tConfVmpParam.IsMtInMember( toldChairMan ) )//����
				{
					//����ϯ������ϳ�
					TVMPMember* ptChairmanMemeber = tConfVmpParam.GetVmpMember(0);//ȡ��ϯ���ڵ��Ǹ�vmpmemeber
					ptChairmanMemeber->SetMemberTMt( tBackupChairman );
					g_cMcuVcApp.SetConfVmpParam(m_tVmpEqp, tConfVmpParam);
					AdjustVmpParam(m_tVmpEqp.GetEqpId(), &tConfVmpParam ,FALSE,FALSE);//��ϯ�϶��Ǳ����նˣ������ٷ�presetin
				}
				break;
			}

		case VCS_GROUPSPEAK_MODE:
			{
				//������ģʽ:�޷����˾�������ϯ,����ϯ������
				if ( tCurMt.IsNull() ) 
				{	
					ChangeSpeaker(&tBackupChairman);
				}
				break;
			}
		
		case VCS_GROUPCHAIRMAN_MODE:
			{
				ChangeSpeaker(&tBackupChairman);
				
				//����ϯѡ����ǰ�����ն�
				if ( !tCurMt.IsNull() ) 
				{
					VCSConfStopMTSel( toldChairMan, MODE_VIDEO );
					TSwitchInfo tSwitchInfo;
					tSwitchInfo.SetSrcMt( tCurMt );
					tSwitchInfo.SetDstMt( tBackupChairman );
					tSwitchInfo.SetMode(MODE_VIDEO);
					VCSConfSelMT(tSwitchInfo);
				}
				break;
			}
		default:
			break;
		}
		
		//zhouyiliang 20101209 ����ϯ������,����ϯ������
		if ( m_tConf.m_tStatus.IsMixing() && !m_ptMtTable->IsMtInMixGrp( tBackupChairman.GetMtId() ) ) 
		{
			AddSpecMixMember(&tBackupChairman,1,TRUE);
			if (m_ptMtTable->IsMtInMixGrp( toldChairMan.GetMtId() )) 
			{
				RemoveSpecMixMember( &toldChairMan, 1, FALSE );
			}			
		}

	

		bRet = TRUE;
	}else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "backup chairman is off-line\n");
	}

	if (bRet)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "changechairman ok\n");
	}else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "changechairman failed\n");
	}

	return bRet;
}

/*====================================================================
    ������      ��ProcVcsMcuStopAllMonCmd
    ����        ���رռ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    11/03/18    4.6         Ѧ��          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuStopAllMonCmd(const CMessage *pcMsg)
{
	if( NULL== pcMsg )
	{
		return ;
	}
	
	STATECHECK;

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	u8 byMcsId = cServMsg.GetSrcSsnId();
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVcsMcuStopAllMonCmd] byMcsId is %d\n", byMcsId);

	StopSwitchToMonitor(byMcsId, TRUE);
}

/*====================================================================
    ������      ��ProcVcsMcuStopMonReq
    ����        ���رռ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26    4.6         Ѧ��          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuStopMonReq(const CMessage *pcMsg )
{
	if( NULL== pcMsg )
	{
		return ;
	}
	
	STATECHECK;
    
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	TSwitchDstInfo tSwitchDstInfo = *(TSwitchDstInfo*) ( cServMsg.GetMsgBody() );
	
	TSwitchDstInfo tTempSwitchInfo;
	memcpy(&tTempSwitchInfo, &tSwitchDstInfo, sizeof(TSwitchDstInfo));
	
	//[2011/11/10/zhangli]��������mcs�ౣ�������tSwitchDstInfo.m_tSrcMt��ͣ������¼��ն�ʱ���ȼ��mt1�ɹ���
	//�ټ��mt2ֻ��audio�ɹ�����Ȼ���mt1��video��ͣ���ʱ��tSwitchDstInfo.m_tSrcMt=mt2������mt1����Ƶ�����޷��ͷ�
	//StopSwitchToMonitor(tSwitchDstInfo, cServMsg.GetSrcSsnId());
	
	TMt tOldSrc;
	if (MODE_BOTH == tSwitchDstInfo.m_byMode || MODE_VIDEO == tSwitchDstInfo.m_byMode)
	{
		if(g_cMcuVcApp.GetMonitorSrc(cServMsg.GetSrcSsnId(), MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, &tOldSrc))
		{
			if(!tOldSrc.IsNull())
			{
				tTempSwitchInfo.m_tSrcMt = tOldSrc;
				tTempSwitchInfo.m_byMode = MODE_AUDIO;
				StopSwitchToMonitor(tTempSwitchInfo, cServMsg.GetSrcSsnId());
			}
		}
	}
	
	if (MODE_BOTH == tSwitchDstInfo.m_byMode || MODE_AUDIO == tSwitchDstInfo.m_byMode)
	{
		tOldSrc.SetNull();
		if(g_cMcuVcApp.GetMonitorSrc(cServMsg.GetSrcSsnId(), MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, &tOldSrc))
		{
			if(!tOldSrc.IsNull())
			{
				tTempSwitchInfo.m_tSrcMt = tOldSrc;
				tTempSwitchInfo.m_byMode = MODE_VIDEO;
				StopSwitchToMonitor(tTempSwitchInfo, cServMsg.GetSrcSsnId());
			}
		}
	}

	// ack
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
}

/*====================================================================
    ������      ��ProcVcsMcuStartMonReq
    ����        ����ʼ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/10/26    4.6         Ѧ��          ����
====================================================================*/
void CMcuVcInst::ProcVcsMcuStartMonReq( const CMessage *pcMsg )
{
	if( NULL== pcMsg )
	{
		return ;
	}

	STATECHECK;
    
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	//action abort when no chairman online
	TMt tChairMan = m_tConf.GetChairman();
	if( ( tChairMan.IsNull()  || 
		!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
		m_byCreateBy != CONF_CREATE_MT
		)
	{
		cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
		cServMsg.SetEventId(cServMsg.GetEventId() + 2);
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}

	CServMsg	cMsg( pcMsg->content, pcMsg->length );
	TSwitchDstInfo tSwitchDstInfo = *(TSwitchDstInfo*) ( cServMsg.GetMsgBody() );

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVcsMcuStartMonReq] param is: mode: %u, srcChannel: %u, VidIp: 0x%x, VidPort: %d\n\tAudIp:0x%x, AudPort:%d, srcmt(%d,%d)\n", 
				tSwitchDstInfo.m_byMode, 
				tSwitchDstInfo.m_bySrcChnnl, 
				tSwitchDstInfo.m_tDstVidAddr.GetIpAddr(),
				tSwitchDstInfo.m_tDstVidAddr.GetPort(),
				tSwitchDstInfo.m_tDstAudAddr.GetIpAddr(),
				tSwitchDstInfo.m_tDstAudAddr.GetPort(),
				tSwitchDstInfo.m_tSrcMt.GetMcuId(),
				tSwitchDstInfo.m_tSrcMt.GetMtId()
			);

	//tSwitchDstInfo ���������У��
	TMt tSrcMt = tSwitchDstInfo.m_tSrcMt;
	u8 byMode = tSwitchDstInfo.m_byMode;
	u32 dwDstIp = tSwitchDstInfo.m_tDstVidAddr.GetIpAddr();
	
	if( MODE_NONE == byMode || 0 == dwDstIp )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "tSwitchDstInfo param is wrong( bymode:%u, DstIp:0x%X ) \n", byMode, dwDstIp);
		return;	
	}

	/*if (m_tConf.GetConfAttrb().IsSatDCastMode()
		&& IsMultiCastMt(tSrcMt.GetMtId()))
	{
		if (IsSatMtOverConfDCastNum(tSrcMt))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcMtMcuStartSwitchMtReq] over max upload mt num. nack!\n");
			cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
    }*/
	
	
	// ���Զ˻�ack
    SendReplyBack( cMsg, cMsg.GetEventId() + 1 );

	u8 byMcsId = cServMsg.GetSrcSsnId();

	// [12/23/2010 xliang] if UI ask the same Src in the same Monitor channel, MCU should do nothing.
	TMt tMcAudCurSrc;
	TMt tMcVidCurSrc;
	g_cMcuVcApp.GetMonitorSrc(byMcsId, MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, &tMcVidCurSrc);
	g_cMcuVcApp.GetMonitorSrc(byMcsId, MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, &tMcAudCurSrc);
	if( ( byMode == MODE_BOTH && tMcVidCurSrc == tSrcMt && tMcAudCurSrc == tSrcMt )
		|| ( byMode == MODE_VIDEO && tMcVidCurSrc == tSrcMt )
		|| ( byMode == MODE_AUDIO && tMcAudCurSrc == tSrcMt )
		)
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "repeated mt(%d,%d), mode: %u is in the monitor channel, mcu do nothing\n",
			tSrcMt.GetMcuId(), tSrcMt.GetMtId(), byMode);
		return;
	}

	// [1/27/2011 xliang] filter mode
	if( byMode == MODE_BOTH && tMcVidCurSrc == tSrcMt && (!(tMcAudCurSrc == tSrcMt)) )
	{
		byMode = MODE_AUDIO;
	}
	else if( byMode == MODE_BOTH && (!(tMcVidCurSrc == tSrcMt))  && tMcAudCurSrc == tSrcMt)
	{
		byMode = MODE_VIDEO;
	}
	tSwitchDstInfo.m_byMode = byMode;

	//��ش�
	if( !tSrcMt.IsLocal() )
	{
		if( IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()) )
		{
			TPreSetInReq tSpySrcInitInfo;
			tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt(tSrcMt);
			tSpySrcInitInfo.m_bySpyMode = byMode;
			tSpySrcInitInfo.SetEvId(cServMsg.GetEventId());
			
			TMcsRegInfo tMcRegInfo;
			if (g_cMcuVcApp.GetMcsRegInfo(byMcsId, &tMcRegInfo))
			{
				tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchDstInfo.SetMcIp(tMcRegInfo.GetMcsIpAddr());
				tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchDstInfo.SetMcSSRC(tMcRegInfo.GetMcsSSRC());
			}
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcVcsMcuStartMonReq] VcsIp.%s McSSrc.%x!\n", 
										 StrOfIP(tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchDstInfo.GetMcIp()),
										 tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchDstInfo.GetMcSSRC());
		
			tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchDstInfo.m_bySrcChnnl  = tSwitchDstInfo.m_bySrcChnnl;
			tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchDstInfo.m_tDstVidAddr = tSwitchDstInfo.m_tDstVidAddr;
			tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchDstInfo.m_tDstAudAddr = tSwitchDstInfo.m_tDstAudAddr;

// 			tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_stChlIdx = tSwitchInfo.GetDstChlIdx();
// 			tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_bySrcChlIdx = tSwitchInfo.GetSrcChlIdx();
// 			tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_byMcInstId  = cServMsg.GetSrcSsnId();
			
			// [pengjie 2010/9/13] ��Ŀ�Ķ�����
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()) );
			//zjl20101116 �����ǰ�ն��ѻش���������Ҫ���ѻش�Ŀ��������ȡС
			if(!GetMinSpyDstCapSet(tSrcMt, tSimCapSet))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcVcsMcuStartMonReq] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
					tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
				return;
			}
			
			tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		    // End

			// ���ϴ������ͨ�����ն���ɿ��滻�ն�
			TMt tMcSrc;
			u8 byReleaseMode = MODE_NONE;
			if( g_cMcuVcApp.GetMonitorSrc(byMcsId, MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, &tMcSrc) )
			{
				if( !tMcSrc.IsNull() )
				{
					if( tMcSrc == tSrcMt )
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcVcsMcuStartMonReq] GetMcSrc == NewMt, MODE_VIDEO\n" );
					}
					else
					{
						byReleaseMode += MODE_VIDEO;
					}
				}
			}
			
			if( g_cMcuVcApp.GetMonitorSrc(byMcsId, MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, &tMcSrc) )
			{
				if( !tMcSrc.IsNull() )
				{
					if( tMcSrc == tSrcMt )
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcVcsMcuStartMonReq] GetMcSrc == NewMt, MODE_AUDIO !\n" );
					}
					else
					{
						byReleaseMode += MODE_AUDIO;
					}
				}
			}
			
			if (MODE_NONE != byReleaseMode  ) 
			{
				tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode = byReleaseMode;
				if( MODE_BOTH == tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode )
				{
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
				}
				else if( MODE_VIDEO == tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode )
				{
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
				}
				else if( MODE_AUDIO == tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode )
				{
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
				}
				//zhouyiliang 20120424 ������������ȼ��ȵ��ȵͣ�����Ϊ�ǿ��滻����ϳ�
// 				if ( m_tConf.GetConfSource() == VCS_CONF && m_tConf.m_tStatus.m_tVMPParam.IsVMPAuto() )
// 				{
// 					++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
// 				}
				
				tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMcSrc;								
			}
			OnMMcuPreSetIn( tSpySrcInitInfo );
			return;
		}
		else	//���ش��߼�
		{
			//���¼��е�ǰ�����նˣ���ͬһ���¼��������ն˱�ѡ������NACK
			if( m_tConf.GetConfSource() == VCS_CONF && !m_cVCSConfStatus.GetCurVCMT().IsNull()
				&&  IsMtInMcu(GetLocalMtFromOtherMcuMt(tSrcMt), m_cVCSConfStatus.GetCurVCMT()) 
				&& !( m_cVCSConfStatus.GetCurVCMT() == tSrcMt ))
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuMtInTwMsg] CurVCMT(%d.%d) and Mt(%d.%d) is in same smcu,Mt can't be selected \n ",
					m_cVCSConfStatus.GetCurVCMT().GetMcuId(),
					m_cVCSConfStatus.GetCurVCMT().GetMtId(),
					tSrcMt.GetMcuId(),
					tSrcMt.GetMtId() );
				
				cServMsg.SetErrorCode( ERR_MCU_VCS_NOUSABLEBACKCHNNL );
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
			//�鿴MC
			TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tSrcMt.GetMcuId());
			if(ptMcInfo == NULL)
			{
				cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
				SendReplyBack( cMsg, cMsg.GetEventId() + 2 );
				return;
			}
			//����ѡ��Դ
			TMt tMt;
			tMt.SetMcuIdx(tSrcMt.GetMcuId());
			tMt.SetMtId(0);
			TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
			if(ptStatus == NULL)
			{                    
				cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
				SendReplyBack( cMsg, cMsg.GetEventId() + 2 );
				return;
			}
			
			OnMMcuSetIn( tSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
		}
	}
	
	//������������¼
	u16 wMcInstId = cServMsg.GetSrcSsnId();
	StartSwitchToMonitor(tSwitchDstInfo, wMcInstId);

// 	if( !ProcStartMonitorOpr(tSwitchDstInfo, wErrorCode) )
// 	{
// 		cServMsg.SetErrorCode( wErrorCode );
// 		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
// 	}

	return;
}

/*=============================================================================
�� �� ���� StartSwitchToMonitor
��    �ܣ� ��ؽ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/08/15   4.6		Ѧ��                  ����
=============================================================================*/
BOOL32 CMcuVcInst::StartSwitchToMonitor(TSwitchDstInfo &tSwitchDstInfo, u16 wMcInstId)
{
	if( !g_cMcuVcApp.IsMcConnected( wMcInstId ) )
	{
		return FALSE;
	}

	TMt tSrcMt = tSwitchDstInfo.m_tSrcMt;
	u8	byMode = tSwitchDstInfo.m_byMode;
	u8  bySrcChnnl = tSwitchDstInfo.m_bySrcChnnl;


	TMt tSrc = GetLocalMtFromOtherMcuMt( tSrcMt );
	u8 bySwitchMode = byMode;
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

	//������ش�֧��
	TMt tMcSrc;
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH ) // ��Ƶ
	{
		if( g_cMcuVcApp.GetMonitorSrc(wMcInstId, MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, &tMcSrc) )
		{
			if( !tMcSrc.IsNull() )
			{
				if( tMcSrc == tSrcMt )
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "StartSwitchToMonitor GetMcSrc == NewMt(%d,%d), MODE_AUDIO\n",
						tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
				}
				else
				{
					FreeRecvSpy( tMcSrc, MODE_AUDIO);
					g_cMcuVcApp.SetMonitorSrc( wMcInstId, MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, tSrcMt );
				}
			}
		}
		else
		{
			g_cMcuVcApp.SetMonitorSrc( wMcInstId, MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, tSrcMt );
		}
	}
	
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH ) // ��Ƶ
	{
		if( g_cMcuVcApp.GetMonitorSrc(wMcInstId, MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, &tMcSrc) )
		{
			if( !tMcSrc.IsNull() )
			{
				if( tMcSrc == tSrcMt )
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "StartSwitchToMonitor GetMcSrc == NewMt(%d,%d), MODE_VIDEO\n",
						tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
				}
				else
				{
					FreeRecvSpy( tMcSrc, MODE_VIDEO);
					g_cMcuVcApp.SetMonitorSrc( wMcInstId, MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, tSrcMt );
				}
			}
		}
		else
		{
			g_cMcuVcApp.SetMonitorSrc( wMcInstId, MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, tSrcMt );
		}
	}



	CRecvSpy tSpyResource;
	u16 wSpyPort = SPY_CHANNL_NULL;	
	if( m_cSMcuSpyMana.GetRecvSpy( tSrcMt, tSpyResource ) )
	{
		wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
	}


	//������
	tSwitchDstInfo.m_byMode = bySwitchMode;
	if( !g_cMpManager.StartSwitchToDst(tSwitchDstInfo, wSpyPort) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "StartSwitchToDst() failed! Cannot switch to specified eqp!\n" );
		return FALSE;
	}

	

	if( !tSrcMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuIdx()) )
	{
		TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));
		SendMMcuSpyNotify( tSrcMt, VCS_MCU_START_MONITOR_UNION_REQ, tSimCapSet);
	}

	//֪ͨsrc��ʼ����������ؼ�֡
	NotifySrcSend(tSrcMt, byMode, bySrcChnnl, TRUE);
	
	//����notif������
	tSwitchDstInfo.m_byMode = byMode;
	NotifyUIStartMonitor(tSwitchDstInfo, wMcInstId);

	return TRUE;

}

/*=============================================================================
�� �� ���� NotifyUIStartMonitor
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/08/15   4.6		Ѧ��                  ����
=============================================================================*/
void CMcuVcInst::NotifyUIStartMonitor(TSwitchDstInfo &tSwitchDstInfo, u16 wMcInstId)
{

	TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
	TDoublePayload tDVPayload;
	TDoublePayload tDAPayload;
	TMt tSrcMt = tSwitchDstInfo.m_tSrcMt;
	TMt tLocalMt = GetLocalMtFromOtherMcuMt(tSrcMt);

	if( tSrcMt.GetType() == TYPE_MT )
	{
		TSimCapSet    tSrcSCS  = m_ptMtTable->GetSrcSCS( tLocalMt.GetMtId() ); 
		
		if( MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType() || 
			MEDIA_TYPE_H263PLUS == tSrcSCS.GetVideoMediaType() || 
			CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() )
		{
			tDVPayload.SetRealPayLoad( tSrcSCS.GetVideoMediaType() );
			tDVPayload.SetActivePayload( GetActivePayload(m_tConf, tSrcSCS.GetVideoMediaType() ) );
		}
		else
		{
			tDVPayload.SetRealPayLoad( tSrcSCS.GetVideoMediaType() );
			tDVPayload.SetActivePayload( tSrcSCS.GetVideoMediaType() );
		}
		if( CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() )
		{
			tDAPayload.SetRealPayLoad( tSrcSCS.GetAudioMediaType() );
			tDAPayload.SetActivePayload( GetActivePayload(m_tConf, tSrcSCS.GetAudioMediaType() ) );
		}
		else
		{
			tDAPayload.SetRealPayLoad( tSrcSCS.GetAudioMediaType() );
			tDAPayload.SetActivePayload( tSrcSCS.GetAudioMediaType() );
		}
	}
	else if( tSrcMt.GetType() == TYPE_MCUPERI )
	{
		//FIXME: ��չӦ�ã��ɸ��ݲ�ͬ�����ĳһ���ȡ��ͬ��payload 
		tDVPayload.SetRealPayLoad(m_tConf.GetMainVideoMediaType());
		tDVPayload.SetActivePayload(m_tConf.GetMainVideoMediaType());
	}
	else
	{
		// do nothing
	}

	TLogicalChannel tAudLog;
	m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(),LOGCHL_AUDIO,&tAudLog,FALSE );
	
	CServMsg cMsg;
	cMsg.SetMsgBody( (u8 *)&tSwitchDstInfo, sizeof(tSwitchDstInfo));
	cMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
	cMsg.CatMsgBody( (u8 *)&tDVPayload, sizeof(tDVPayload));
	cMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
	cMsg.CatMsgBody( (u8 *)&tDAPayload, sizeof(tDAPayload));
	
	// [8/17/2010 xliang] ���������ܷ�ȥ��
	// zw [06/26/2008] ��ӦAAC LC��ʽ
	TAudAACCap tAudAACCap;
	if ( MEDIA_TYPE_AACLC == m_tConf.GetMainAudioMediaType() )
	{
		tAudAACCap.SetMediaType(MEDIA_TYPE_AACLC);
		tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
		//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
		tAudAACCap.SetChnlType(GetAACChnlTypeByAudioTrackNum(tAudLog.GetAudioTrackNum()));
		tAudAACCap.SetBitrate(96);
		tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
		
		cMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
	}
	//tianzhiyong 2010/04/02 ��ӦAAC LD��ʽ
	else if ( MEDIA_TYPE_AACLD == m_tConf.GetMainAudioMediaType() )
	{
		tAudAACCap.SetMediaType(MEDIA_TYPE_AACLD);
		tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
		//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
		tAudAACCap.SetChnlType(GetAACChnlTypeByAudioTrackNum(tAudLog.GetAudioTrackNum()));
		tAudAACCap.SetBitrate(96);
		tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
		
		cMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[NotifyUIStartMonitor] m_tConf.GetMainAudioMediaType() does'nt equals 103 or 104 !\n");
		cMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
	}

	//֧��UI���ͼ�񶪰��ش� // [8/28/2010 xliang] FIXME: Ŀǰֻ֧���ն˽���صĶ����ش�
	// ֱ�Ӹ�֪����
	//[liu lijiu][2010/09/25]����˫���Լ���ش�����µĶ����ش�
	TTransportAddr  tVidAddr;
	TTransportAddr  tAudAddr;
	tVidAddr.SetNull();
	tAudAddr.SetNull();
	u8 byMode = tSwitchDstInfo.m_byMode;


	//[2011/11/03/zhangli]��ͳһ�ӿڻ�ȡ��ԭд����֧�ֵ��ش�
	if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
	{
		if (!GetRemoteRtcpAddr(tSrcMt, tSwitchDstInfo.m_bySrcChnnl, MODE_AUDIO, tAudAddr))
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[NotifyUIStartMonitor]GetRemoteRtcpAddr failed\n");
		}
	}
	
	if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
	{
		if (!GetRemoteRtcpAddr(tSrcMt, tSwitchDstInfo.m_bySrcChnnl, MODE_VIDEO, tVidAddr))
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[NotifyUIStartMonitor]GetRemoteRtcpAddr failed\n");
		}
	}

	//��ص�˫��ʱ
	if (byMode == MODE_SECVIDEO)
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

	//��ƵRTCPͨ��
	cMsg.CatMsgBody( (u8 *)&tVidAddr, sizeof(tVidAddr) );	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[NotifyUIStartMonitor]tell ui monitor video rtcp port(%s, %d)\n", 
		    StrOfIP(tVidAddr.GetIpAddr()), tVidAddr.GetPort());

	//��ƵRTCPͨ��
	cMsg.CatMsgBody( (u8 *)&tAudAddr, sizeof(tAudAddr) );	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[NotifyUIStartMonitor]tell ui monitor audio rtcp port(%s, %d)\n", 
		        StrOfIP(tAudAddr.GetIpAddr()), tAudAddr.GetPort());
	
	SendMsgToMcs(  (u8)wMcInstId, MCU_VCS_START_MONITOR_NOTIF, cMsg );
}


/*=============================================================================
�� �� ���� AdjustSwitchToMonitorWatchingSrc
��    �ܣ� ������Ӧ��������ü�صĽ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TMt		&tSrc		[i] 
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/09/10   4.6		Ѧ��                  ����
=============================================================================*/
void CMcuVcInst::AdjustSwitchToMonitorWatchingSrc(const TMt &tSrc,u8 byMode /*= MODE_NONE*/,BOOL32 bIsStop /*= TRUE*/,BOOL32 bIsOnlySwitch /*= FALSE*/)
{
	TSwitchDstInfo tSwitchDstInfo;
	TMonitorData   tMonitorData;
	u8 byIndex;

	
	
	for( byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( !g_cMcuVcApp.IsMcConnected( byIndex ) )
		{
			continue;
		}

		for( u16 wLoop = 0; wLoop < MAXNUM_MONITOR_NUM; wLoop ++ )
		{
			if( !g_cMcuVcApp.GetMonitorData(byIndex, wLoop, tMonitorData) )
			{
				continue;
			}

			memset( &tSwitchDstInfo,0,sizeof(tSwitchDstInfo) );
			tSwitchDstInfo.m_tSrcMt = tMonitorData.GetMonitorSrc();
			if( tSwitchDstInfo.m_tSrcMt == tSrc || ( IsMcu(tSrc) && IsMtInMcu(tSrc, tSwitchDstInfo.m_tSrcMt) ) )
			{
				tSwitchDstInfo.m_byMode = tMonitorData.GetMode();
				if( MODE_NONE != byMode )
				{
					if(tSwitchDstInfo.m_byMode == byMode )
					{
						if( MODE_BOTH == byMode || MODE_VIDEO == byMode )
						{
							tSwitchDstInfo.m_tDstVidAddr = tMonitorData.GetDstAddr();		
						}
						if( MODE_BOTH == byMode || MODE_AUDIO == byMode )
						{
							tSwitchDstInfo.m_tDstAudAddr = tMonitorData.GetDstAddr();		
						}
									
					
						if( !bIsStop )
						{
							StartSwitchToMonitor( tSwitchDstInfo,byIndex );
						}
						else
						{
							StopSwitchToMonitor(tSwitchDstInfo, byIndex, bIsOnlySwitch);
						}
					}
				}
				else
				{
					if(tSwitchDstInfo.m_byMode == MODE_VIDEO )
					{
						tSwitchDstInfo.m_tDstVidAddr = tMonitorData.GetDstAddr();
					}
					else
					{
						tSwitchDstInfo.m_tDstAudAddr = tMonitorData.GetDstAddr();
					}
					if( !bIsStop )
					{
						StartSwitchToMonitor( tSwitchDstInfo,byIndex );
					}
					else
					{
						StopSwitchToMonitor(tSwitchDstInfo, byIndex, bIsOnlySwitch);
					}
				}
				
				
				
			}
		}
	}
	return ;
}

/*=============================================================================
�� �� ���� StopSwitchToMonitor
��    �ܣ� ֹͣ��Ӧ��������ü�صĽ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u16		wMcInstId	[i]
		   BOOL32	bConf		[i] �Ƿ��ͣ�û����Ӧ�ļ������
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/08/15   4.6		Ѧ��                  ����
=============================================================================*/
void CMcuVcInst::StopSwitchToAllMonitor(BOOL32 bConf /*= FALSE*/ )
{
	u8 byIndex;
	for( byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( !g_cMcuVcApp.IsMcConnected( byIndex ) )
		{
			continue;
		}

		StopSwitchToMonitor(byIndex, bConf);
	}

	return ;
}

/*=============================================================================
�� �� ���� StopSwitchToMonitor
��    �ܣ� ֹͣ��Ӧ��������ü�صĽ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u16		wMcInstId	[i]
		   BOOL32	bConf		[i] �Ƿ��ͣ�û����Ӧ�ļ������
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/08/15   4.6		Ѧ��                  ����
=============================================================================*/
BOOL32 CMcuVcInst::StopSwitchToMonitor(u16 wMcInstId, BOOL32 bConf /*= FALSE*/ )
{
	if( wMcInstId == 0 || wMcInstId > MAXNUM_MCU_MC + MAXNUM_MCU_VC )
	{
		return FALSE;
	}
	
	TSwitchDstInfo tSwitchDstInfo;
	TMonitorData   tMonitorData;
	for( u16 wLoop = 0; wLoop < MAXNUM_MONITOR_NUM; wLoop ++ )
	{
		if( !g_cMcuVcApp.GetMonitorData(wMcInstId, wLoop, tMonitorData) )
		{
			continue;
		}
		
		tSwitchDstInfo.m_tSrcMt = tMonitorData.GetMonitorSrc();
		if( bConf && tSwitchDstInfo.m_tSrcMt.GetConfIdx() != m_byConfIdx )
		{
			continue;
		}

		tSwitchDstInfo.m_byMode = tMonitorData.GetMode();
		if(tSwitchDstInfo.m_byMode == MODE_VIDEO )
		{
			tSwitchDstInfo.m_tDstVidAddr = tMonitorData.GetDstAddr();
		}
		else
		{
			tSwitchDstInfo.m_tDstAudAddr = tMonitorData.GetDstAddr();
		}
		
		StopSwitchToMonitor(tSwitchDstInfo, wMcInstId);			
	}

	return TRUE;
}

/*=============================================================================
�� �� ���� StopSwitchToMonitor
��    �ܣ� ֹͣ��Ӧ�����Ӧ���յ�ַ�ļ�ؽ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/08/15   4.6		Ѧ��                  ����
=============================================================================*/
BOOL32 CMcuVcInst::StopSwitchToMonitor(TSwitchDstInfo &tSwitchDstInfo, u16 wMcInstId, BOOL32 bIsOnlySwitch/* = FALSE*/)
{
	//1,stop switch
	g_cMpManager.StopSwitchToDst(tSwitchDstInfo);

	if( bIsOnlySwitch )
	{
		return TRUE;
	}

	//2,��ش���Դ����monitor table����
	TMt tSrcMt = tSwitchDstInfo.m_tSrcMt;
	u8  byMode = tSwitchDstInfo.m_byMode;
	TMt tMtNull;
	tMtNull.SetNull();

	if( byMode == MODE_AUDIO || byMode == MODE_BOTH ) // ͣ��Ƶ
	{
		if( !tSrcMt.IsNull() && !tSrcMt.IsLocal())
		{
			FreeRecvSpy( tSrcMt, MODE_AUDIO);
		}

		g_cMcuVcApp.SetMonitorSrc( wMcInstId, MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, tMtNull );
	}
	
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH ) // ͣ��Ƶ
	{
		if( !tSrcMt.IsNull() && !tSrcMt.IsLocal())
		{
			FreeRecvSpy( tSrcMt, MODE_VIDEO);
		}

		g_cMcuVcApp.SetMonitorSrc( wMcInstId, MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, tMtNull );
	}

	return TRUE;
}

/*=============================================================================
�� �� ���� GetVcsHduAndTwModuleChnNum
��    �ܣ� ��õ�ǰVCS�������õĵ���ǽģ��ͨ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/10/29   4.6		��־��                 ����
2011/05/25   4.6		��  ��                 �޸�
                                             ��TMultiTvWallModuleѰ��ͨ����Ŀ���߼��������޸�,
											 ���Ƶ���TMultiTvWallModule�ڲ�
=============================================================================*/
u8 CMcuVcInst::GetVcsHduAndTwModuleChnNum()
{
	u8 byTwAndHduChnNum = 0;
	BOOL32 bVcsConf =  ( VCS_CONF == m_tConf.GetConfSource()  ) ? TRUE : FALSE;
	byTwAndHduChnNum += m_tConfEqpModule.m_tHduModule.GetHduChnlNum( bVcsConf ); 
	
	TMultiTvWallModule tMultiTvWallModule;
	m_tConfEqpModule.GetMultiTvWallModule( tMultiTvWallModule );
	byTwAndHduChnNum += tMultiTvWallModule.GetChnnlNum( bVcsConf );	

	return byTwAndHduChnNum;
}

/*=============================================================================
�� �� ���� SetMtInTvWallAndHduInFailPresetin
��    �ܣ� ��Presetinʧ��ʱ��Ԥ������ǽģʽ�������ն������ǽ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/11/24   4.6		��־��                 ����
2011/05/25   4.6	    ����				   �޸�
=============================================================================*/
void CMcuVcInst::SetMtInTvWallAndHduInFailPresetinAndInReviewMode( TPreSetInRsp &tPreSetInRsp )
{
	if( VCS_CONF != m_tConf.GetConfSource() || m_cVCSConfStatus.GetTVWallManageMode() != VCS_TVWALLMANAGE_REVIEW_MODE )
	{
		return;
	}

	const TMt tSrc = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	TPeriEqpStatus tStatus;
	
	// hdu��Ϣ
	if ( MCS_MCU_START_SWITCH_HDU_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() &&
		tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo.m_bySrcMtType == TW_MEMBERTYPE_VCSSPEC)
	{
		// ��ȡhdu�豸��ͨ��
		const u8 byDstChnnlIdx = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo.m_byDstChlIdx;
		const TEqp tHduEqp = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo.m_tHdu;

		if ( !g_cMcuVcApp.GetPeriEqpStatus( tHduEqp.GetEqpId(), &tStatus ))
		{
			return;
		}

		tStatus.m_tStatus.tHdu.atVideoMt[byDstChnnlIdx].SetMt(tSrc);
		tStatus.m_tStatus.tHdu.atVideoMt[byDstChnnlIdx].byMemberType = TW_MEMBERTYPE_VCSSPEC;
		g_cMcuVcApp.SetPeriEqpStatus( tHduEqp.GetEqpId(), &tStatus );
	}

	// tvwall�豸��Ϣ
	if (MCS_MCU_START_SWITCH_TW_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() &&
		tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_bySrcMtType == TW_MEMBERTYPE_VCSSPEC )
	{
		// ��ȡhdu�豸��ͨ��
		const u8 byDstChnnlIdx = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_byDstChlIdx;
		const TEqp tTvwEqp = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_tTvWall;

		if ( !g_cMcuVcApp.GetPeriEqpStatus( tTvwEqp.GetEqpId(), &tStatus ))
		{
			return;
		}

		tStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx].SetMt( tSrc );
		tStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx].byMemberType = TW_MEMBERTYPE_VCSSPEC;
		g_cMcuVcApp.SetPeriEqpStatus( tTvwEqp.GetEqpId(), &tStatus );
	}

	// �ϱ�״̬
	CServMsg cServMsgSend;
	cServMsgSend.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
	SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsgSend);	
}


/*=============================================================================
�� �� ���� ShowCfgExtraData
��    �ܣ� ��ӡvcsԤ��������Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/11/14   4.6		�ܼ���                 ����
=============================================================================*/
void CMcuVcInst::ShowCfgExtraData()
{
	StaticLog("VCS Scheme Num. %d\n", m_cCfgInfoRileExtraDataMgr.GetMtPlanNum());
	StaticLog("VCS Scheme IsRead. %d\n", m_cCfgInfoRileExtraDataMgr.IsReadPlan());
	StaticLog("VCS DataLen. %d\n", m_cCfgInfoRileExtraDataMgr.GetMSDataLen());
	
	u8 idx = 0;
	TMtVcsPlanNames tVcsPlanNames = {0};	
	s8 *pachPlanName[ VCS_MAXNUM_PLAN ];
	for( idx = 0;idx < VCS_MAXNUM_PLAN;idx++ )
	{
		pachPlanName[idx] = &tVcsPlanNames.m_achAlias[idx][0];
	}
	u8 byPlanNum = 0;
	m_cCfgInfoRileExtraDataMgr.GetAllPlanName( (s8**)pachPlanName,byPlanNum);

	for ( idx = 0;idx < VCS_MAXNUM_PLAN;idx++ )
	{
		StaticLog("VCS PlanName. %s  \n", pachPlanName[idx]);
	}
}
