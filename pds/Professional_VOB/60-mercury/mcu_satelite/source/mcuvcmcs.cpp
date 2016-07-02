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
#include "mcuerrcode.h"
#include "mtadpssn.h"
#include "mcuutility.h"
#include "mpmanager.h"
#include "radiusinterface.h"
#include "modemssn.h"

#if defined(_VXWORKS_)
    #include <inetLib.h>
    #include "brddrvlib.h"
#elif defined(_LINUX_)
    #include "boardwrapper.h"
#endif

extern TMtTopo		g_atMtTopo[MAXNUM_TOPO_MT];	//����ȫ����MT������Ϣ
extern u16		    g_wMtTopoNum;		        //����ȫ����MT��Ŀ


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

	//save info
	g_cMcuVcApp.SetMcConnected( byInstId, TRUE );
	g_cMcuVcApp.SetMcsRegInfo( byInstId, tRegReq );
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
	TLogicalChannel tLogicalChannel;

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
                 if ( ROLLCALL_MODE_NONE != ptConfInfo->m_tStatus.GetRollCallMode() )
                 {
                     ptConfInfo->m_tStatus.SetRollCallMode(ROLLCALL_MODE_NONE);
                 }
			 }
			 
             if( ptConfInfo->m_tStatus.GetProtectMode() == CONF_LOCKMODE_NEEDPWD )
			 {
				 //�´ε�¼��������������
                 ptConfProtectInfo->SetMcsPwdPassed(byInstId, FALSE);
			 }
		}
	}

	//stop switch to MC
	g_cMcuVcApp.BroadcastToAllConf( MCU_MCSDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
	
	//clear info
	g_cMcuVcApp.SetMcConnected( byInstId, FALSE );

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
    CServMsg cMtMsg;

    CServMsg  cServMsg( pcMsg->content, pcMsg->length );

    TConfInfo tTmpConf = *(TConfInfo *)cServMsg.GetMsgBody();
    BOOL32 bConfFromFile = tTmpConf.m_tStatus.IsTakeFromFile();
    tTmpConf.m_tStatus.SetTakeFromFile(FALSE);

	TCapSupportEx tCapEx;

	switch( CurState() )
	{
	case STATE_WAITEQP:
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
			BOOL32 bRet = g_cMcuVcApp.AnalyEqpCapacity( tConfEqpCap, tMcuEqpCap );            

            // [12/28/2006-zbq] N+1���ݻع���ָ��Ļ��飬��Ϊ�����Ѿ�ͨ����ֱ�Ӵ���
			if( !bRet && MCU_NPLUS_IDLE == g_cNPlusApp.GetLocalNPlusState() )
			{
				// ���ö�ʱ��ȥ���¿���		
				if( bConfFromFile )
				{
                    // ��������Դ���
					if( MAX_TIMES_CREATE_CONF < m_wRestoreTimes ) 
					{
						if (!g_cMcuVcApp.RemoveConfFromFile(tTmpConf.GetConfId()))
						{
							ConfLog( FALSE, "[ProcMcsMcuCreateConfReq] remove conf from file failed\n");
						}
                        else
                        {
                            ConfLog( FALSE, "[ProcMcsMcuCreateConfReq] conf.%s has been remove from file due to RestoreTimes.%d\n", tTmpConf.GetConfName(), m_wRestoreTimes);
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
				
				ConfLog( FALSE, "Conference %s create failed because NO enough equipment(%d)!\n", tTmpConf.GetConfName(), m_wRestoreTimes );
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
						OspPrintf(TRUE, FALSE, "[ProcMcsMcuCreateConfReq] remove conf from file failed\n");
					}
                    */
					g_cMcuVcApp.SetConfStore(byConfIdx-MIN_CONFIDX, FALSE);// �Ժ��ٻָ�

					NEXTSTATE(STATE_IDLE);
                    m_wRestoreTimes = 1;

                    KillTimer(MCU_WAITEQP_CREATE_CONF_NOTIFY);
				}
			}
		}  // �˴�û��Break����Ϊ��Mcu֧�ֻ������������ʱ������ִ������ĳ���������

	case STATE_IDLE:

		//���ʵ��
		ClearVcInst();
        cServMsg.SetErrorCode(0);       

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

        // guzh [4/16/2007] �жϼ�ʱ���鿪ʼʱ��
        if ( tTmpConf.m_tStatus.IsOngoing() &&
             tTmpConf.GetStartTime() > time(NULL)+1*60 &&
             // zbq [08/02/2007] N+1���ݻָ��Ļ���ֱ���ٿ�
             CONF_CREATE_NPLUS != cServMsg.GetSrcMtId() )
        {
            cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );            
            ConfLog( FALSE, "Conference %s failure because start time error!\n", tTmpConf.GetConfName() );
			return;
        }
        
        // �Ѵ���������������ʱ�������Ƿ���
		if ( g_cMcuVcApp.IsConfNumOverCap(tTmpConf.m_tStatus.IsOngoing(), tTmpConf.GetConfSource()) )
		{
			cServMsg.SetErrorCode( ERR_MCU_CONFNUM_EXCEED );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            if ( bConfFromFile )
            {
                g_cMcuVcApp.RemoveConfFromFile(tTmpConf.GetConfId());
            }      
			ConfLog( FALSE, "Conference %s failure because exceed max conf num!\n", tTmpConf.GetConfName() );
			return;
		}

		if (0 == g_cMcuVcApp.GetMpNum() || 0 == g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323))
		{
			cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			ConfLog( FALSE, "Conference %s failure because No_Mp or No_MtAdp!\n", tTmpConf.GetConfName() );
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
                ConfLog( FALSE, "Conference %s failure for conf full!\n", tTmpConf.GetConfName() );
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
            ConfLog( FALSE, "[CreateConf] MVidType.%d with FPS.%d is unexpected, adjust it\n",
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
            ConfLog( FALSE, "[CreateConf] SVidType.%d with FPS.%d is unexpected, adjust it\n",
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
            ConfLog( FALSE, "[CreateConf] DSVidType.%d with FPS.%d is unexpected, adjust it\n",
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

        //zbq[11/15/2008] debug���ӵڶ�˫������
        if (MEDIA_TYPE_NULL == m_tConf.GetCapSupportEx().GetSecDSType())
        {
            AddSecDSCapByDebug(m_tConf);
        }

#ifdef _SATELITE_
		if (IsHDConf(m_tConf) &&
			g_cMcuVcApp.IsConfAdpManually())
		{
			tCapEx = m_tConf.GetCapSupportEx();
			
			tCapEx.SetSecDSBitRate(m_tConf.GetBitRate()); //ͬ��һ·˫��������ȫ����
			tCapEx.SetSecDSFrmRate(5);
			tCapEx.SetSecDSType(MEDIA_TYPE_H263);
			tCapEx.SetSecDSRes(VIDEO_FORMAT_SVGA);
			
			m_tConf.SetCapSupportEx(tCapEx);
			ConfLog(FALSE, "[AddSecDSCapByDebug] add dual ds cap<h263+, SVGA, 5fps>, satelite\n");

		}

#endif
		

		//���Ƿ�ɢ���飬debug���ӵڶ�·��������: ģ�����ǻ���Ļ���
		if (g_cMcuVcApp.IsConfAdpManually() &&
			IsHDConf(m_tConf) &&
			m_tConf.GetConfAttrb().IsSatDCastMode() )
		{
			TSimCapSet tSimCap;
			tSimCap.SetAudioMediaType(MEDIA_TYPE_PCMA);
			tSimCap.SetVideoFrameRate(25);
			tSimCap.SetVideoMaxBitRate(512);
			tSimCap.SetVideoMediaType(MEDIA_TYPE_H263);
			tSimCap.SetVideoResolution(VIDEO_FORMAT_CIF);
			
			TCapSupport tConfCap = m_tConf.GetCapSupport();
			tConfCap.SetSecondSimCapSet(tSimCap);
			m_tConf.SetCapSupport(tConfCap);

			m_tConf.SetSecBitRate(512);
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
            CreateConfSendMcsNack( byOldInsId, ERR_MCU_NOMTINCONF, cServMsg );
            ConfLog( FALSE, "Conference %s create failed because on mt in conf!\n", m_tConf.GetConfName() );
            return;
        }

        // N+1 ������Ϊ��֧��ԤԼ���� [12/20/2006-zbq]
		// ���鴦��: ��ʱ����
		if ( CONF_CREATE_NPLUS == m_byCreateBy ||
            ( CONF_CREATE_NPLUS != m_byCreateBy && m_tConf.m_tStatus.IsOngoing() )  )
		{
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

            //MCU�����������Ƿ��֣��������������
            if ( !IsEqpInfoCheckPass(cServMsg, wErrCode))
            {
                CreateConfEqpInsufficientNack( byCreateMtId, byOldInsId, wErrCode, cServMsg );

                if (ERR_MCU_NOENOUGH_HDBAS_CONF == wErrCode)
                {
                    TMcuHdBasStatus tStatus;
                    m_cBasMgr.GetHdBasStatus(tStatus, m_tConf);
                    
                    CServMsg cMsg;
                    cMsg.SetMsgBody((u8*)&tStatus, sizeof(tStatus));
                    cMsg.SetEventId(MCU_MCS_MAUSTATUS_NOTIFY);
                    SendMsgToMcs(cServMsg.GetSrcSsnId(), MCU_MCS_MAUSTATUS_NOTIFY, cMsg);
                }
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
                
                // guzh [7/19/2007] ���͸�������
                SendMsgToMt(CONF_CREATE_MT, MCU_MT_CREATECONF_ACK, cAckMsg);
            }

			//��ӡ��Ϣ 
			if ( CONF_DATAMODE_VAONLY == m_tConf.GetConfAttrb().GetDataMode() )
			{
				//��Ѷ����
				ConfLog( FALSE, "conference %s created and started!\n",m_tConf.GetConfName());
			}
			else if ( CONF_DATAMODE_VAANDDATA == m_tConf.GetConfAttrb().GetDataMode() )
			{
				//��Ѷ������
				ConfLog( FALSE, "conference %s created and started with data conf function !\n", m_tConf.GetConfName());
			}
			else
			{
				//���ݻ���
				ConfLog( FALSE, "conference %s created and started with data conf function Only !\n", m_tConf.GetConfName());
			}

            //zbq[12/18/2008] �ֶ������ɹ����嶨ʱ�ȴ�
            KillTimer(MCU_WAITEQP_CREATE_CONF_NOTIFY);

			//���浽�ļ�(���ϼ�MCU�ĺ��д����Ļ��鲻����)��N+1����ģʽ�����浽�ļ�
            if ( CONF_CREATE_NPLUS == m_byCreateBy && MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
            {
                NPlusLog("[ProcMcsMcuCreateConfReq] conf info don't save to file in N+1 restore mode.\n");
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
                    ConfLog( FALSE, "[CreateConf] charge postponed due to GetRegGKDriId.%d, ChargeRegOK.%d !\n",
                                     g_cMcuVcApp.GetRegGKDriId(), g_cMcuVcApp.GetChargeRegOK() );
                }
            }

            //���ǻ��� ������ע������ Ƶ��
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                ProcCreateConfAccordNms(pcMsg);
                ApplySatFrequence();
            }

            // ���� [4/29/2006]��ԭ��֪ͨMCS�����ڱ��浽�ļ����档
            // �ƶ���������Ϊ�˱�֤Gk����Ϣ����ȷ�ġ�֪ͨ���л�ء�
            cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
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

			SetTimer( MCUVC_INVITE_UNJOINEDMT_TIMER, 1000*m_tConf.m_tStatus.GetCallInterval() );
			
			SetTimer( MCUVC_MCUSRC_CHECK_TIMER, TIMESPACE_MCUSRC_CHECK);
            
			u8 m_byUniformMode = tConfAttrib.IsAdjustUniformPack();
			cServMsg.SetMsgBody((u8 *)&m_byUniformMode, sizeof(u8));
            cServMsg.SetConfIdx(m_byConfIdx);
			SendMsgToAllMp(MCU_MP_CONFUNIFORMMODE_NOTIFY, cServMsg);

            //�ı����ʵ��״̬
            NEXTSTATE( STATE_ONGOING );

            //������ػ��������ն���Ϣ
            m_tConfAllMtInfo.m_tLocalMtInfo.SetMcuId( LOCAL_MCUID );
            cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
            SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
            
            //��������ն˱� 
            SendMtListToMcs(LOCAL_MCUID);	
            
            //֪ͨn+1���ݷ��������»�����Ϣ
            if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
            {
                ProcNPlusConfInfoUpdate(TRUE);
            }            
		}
		else //ԤԼ����
		{					
            m_tConf.m_tStatus.SetScheduled();

			//�ѻ���ָ��浽������
			if( !g_cMcuVcApp.AddConf( this ) )	//add into table
			{
                CreateConfSendMcsNack( byOldInsId, ERR_MCU_CONFOVERFLOW, cServMsg, TRUE);
				ConfLog( FALSE, "Conference %s create failed because of full instance!\n", m_tConf.GetConfName() );
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

			//���浽�ļ�
			BOOL32 bRetTmp = g_cMcuVcApp.SaveConfToFile( m_byConfIdx, FALSE );
            if (!bRetTmp)
            {
                OspPrintf(TRUE, FALSE, "[creat conf]SaveConfToFile error!!!\n");
            }

			//����ʱ��
			SetTimer( MCUVC_SCHEDULED_CHECK_TIMER, TIMESPACE_SCHEDULED_CHECK );

			//��ӡ
			ConfLog( FALSE, "A scheduled conference %s created!\n", m_tConf.GetConfName() );
			
			//�ı����ʵ��״̬
			NEXTSTATE( STATE_SCHEDULED );
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
    ������      :ProcCreateConfAccordNms
    ����        :���ܰ��Ż���
    �㷨ʵ��    :
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/19    1.0         zhangsh         ����
    09/09/11    4.6         �ű���          �����ǰ汾��ֲ����
====================================================================*/
void CMcuVcInst::ProcCreateConfAccordNms( const CMessage* pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	
    u8 byMtLoop;
	u32 dwMtIp;
	
    TSchConfInfoHeader tSchInfo;
	tSchInfo.SetConfId( m_tConf.GetConfId() );
	tSchInfo.SetPassWord( (u8*)m_tConf.GetConfPwd() );
	tSchInfo.SetStartTime( m_tConf.GetStartTime() );
	tSchInfo.SetEndTime( m_tConf.GetStartTime() + m_tConf.GetDuration()*60 );
	tSchInfo.SetMessageType( MCU_NMS_SCHEDULE_CONF_REQ );
	tSchInfo.SetMtNum( m_tConfAllMtInfo.GetAllMtNum() );
	tSchInfo.SetSerialId( GetSerialId() );
	cServMsg.SetMsgBody( (u8*)&tSchInfo, sizeof(TSchConfInfoHeader) );
	
    //add all Mt Ip
	for ( byMtLoop = 1; byMtLoop < MAXNUM_CONF_MT; byMtLoop ++ )
	{
		if ( m_tConfAllMtInfo.MtInConf( byMtLoop ) )
		{
			dwMtIp = ::topoGetMtInfo( LOCAL_MCUID, byMtLoop, g_atMtTopo, g_wMtTopoNum ).GetIpAddr();
			dwMtIp = htonl( dwMtIp );
			cServMsg.CatMsgBody( (u8*)&dwMtIp, sizeof(u32) );
		}
	}
	g_cMcuVcApp.SendMsgToDaemonConf( MCU_NMS_SENDNMSMSG_CMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	SetTimer( TIMER_SCHEDCONF, 5*1000 );
	
    return;
}

/*====================================================================
    ������      :ProcNmsMcuSchedConf
    ����        :ԤԼ������֤,������ܲ��أ���Ĭ��ʧ��
    �㷨ʵ��    
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/02/02    1.0         zhangsh         ����
    09/09/11    4.6         �ű���          �����ǰ汾��ֲ����
====================================================================*/
void CMcuVcInst::ProcNmsMcuSchedConf( const CMessage*pcMsg )
{
	TSchConfNotif tSchConf = *(TSchConfNotif*) pcMsg->content;
	CServMsg cServMsg;
	u32 *pNackIp;
	u32 dwMtIp;
	u8  abyConfId[sizeof( CConfId )];
	//cServMsg.SetSrcSsnId( m_byMcSsnId );
	//cServMsg.SetSerialNo( m_dwMcSerialNo );
	//cServMsg.SetSrcMcu( g_cMcuAgent.GetId() );
	
	switch( CurState() )
	{
	//FIXME: �Ժ��ϸ� ����״̬��
	case STATE_ONGOING:

	case STATE_SCHEDULED:
    case STATE_WAITAUTH:
	case STATE_WAITEQP:

		
        KillTimer( TIMER_SCHEDCONF );
		
        //FIXME: �˴�������״̬��
        //m_tConf.m_tStatus.SetScheduled();	//set state

		//m_tConf.SetMcuId( (u8)g_cMcuAgent.GetId() );
		m_tConf.GetConfId().GetConfId( abyConfId, sizeof( CConfId ) );
	
        //������ܾܾ��������ն�,�������
		if ( tSchConf.GetMtNum() == m_tConfAllMtInfo.GetAllMtNum() || tSchConf.GetMcs() == 0 )
		{
			//cServMsg.SetErrorCode( ERR_MCU_NMSNOPARTICIPANT );
			//SendReplyBack( cServMsg, MCU_VCCTRL_CREATECONF_NACK );
            
            NotifyMcsAlarmInfo(0, ERR_MCU_NMSNOPARTICIPANT);
			ReleaseConf( TRUE );
			NEXTSTATE( STATE_IDLE );
			ConfLog( FALSE, "[ProcNmsMcuSchedConf] Nms del all Mt!\n");
			return;
		}
		
        //����ɾ���˲����ն�
		if ( tSchConf.GetMtNum() != 0 )
		{
			pNackIp = (u32*)(pcMsg->content+sizeof(TSchConfNotif));
			for ( u16 wDelMt = 0 ; wDelMt < tSchConf.GetMtNum() ; wDelMt ++ )
			{
				for ( u16 byMtLoop = 1; byMtLoop < MAXNUM_CONF_MT; byMtLoop ++ )
				{
					if ( m_tConfAllMtInfo.MtInConf( byMtLoop ) )
					{
						dwMtIp = ::topoGetMtInfo( LOCAL_MCUID, byMtLoop, g_atMtTopo, g_wMtTopoNum ).GetIpAddr();
						dwMtIp = htonl( dwMtIp );
						if ( *pNackIp == dwMtIp )
						{
                            TMt tMt = m_ptMtTable->GetMt(byMtLoop);

							//FIXME: �����remove�ĺ����Ǵ�ģ������ʱɾ������ʱ������
							RemoveMt( tMt, TRUE );
						}
					}
				}
				pNackIp ++;
			}			
		}

        if (STATE_WAITAUTH == CurState())
        {
            //zbq[09/11/2009] ������ʱ��ֱ�����Ի���ķ�����
            /*
            cServMsg.SetMsgBody( (u8*)&m_tConf,sizeof(m_tConf) );
            OspPost( MAKEIID( AID_MCU_VC,GetInsID() ), MCU_CREATECONF_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
            */
		    break;
        }
        else
        {
            //FIXME: ��������У��
            /*
       		if( SetAlias( ( const char* )abyConfId, sizeof( CConfId ) )== OSPERR_ALIAS_REPEAT )
		    {
			    //cServMsg.SetErrorCode( ERR_MCU_CONFIDREPEAT );
			    //SendReplyBack( cServMsg, MCU_VCCTRL_CREATECONF_NACK );
                
                NotifyMcsAlarmInfo(0, ERR_MCU_CONFIDREPEAT);
			    ReleaseConf();
			    NEXTSTATE( STATE_IDLE);
			    ConfLog( FALSE, "[ProcNmsMcuSchedConf] Conf.%s create failed due to repeat conference ID at NMS SCH!\n", m_tConf.GetConfName() );
			    return;
		    }*/
		    
            //zbq[09/11/2009] �����Ļ��������Ѿ�����add������ֻ��У��
            /*
		    if( !g_cMcuVcApp.AddConf( &m_tConf ) )	//add into table
		    {
			    cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
			    SendReplyBack( cServMsg, MCU_VCCTRL_CREATECONF_NACK );
			    DeleteAlias();
			    ReleaseConf();
			    NEXTSTATE( STATE_IDLE);
			    if ( bConfMsg )
				    log( LOGLVL_EXCEPTION, "CMcuVcInst: Confernece %s create failed because of full instance!\n", m_tConf.GetConfName() );
			    return;
		    }*/
	    
            //zbq[09/11/2009] �����ϸ����ݲ�������
            /*
		    cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
		    cServMsg.SetConfId( m_tConf.GetConfId() );
		    
            //NAck
		    g_cMcSsnApp.SendMsgToMcs( cServMsg.GetSrcSsnId(), MCU_VCCTRL_CREATECONF_ACK, cServMsg );
		    
            //Notification
		    g_cMcSsnApp.SendMsgToAllMcs( MCU_VCCTRL_CONF_NOTIF, cServMsg );
		    
            //write file
		    ::AddConfInfoToFile( m_tConf );*/


		    //SetTimer( TIMER_SCHEDULED_CHECK, TIMESPACE_SCHEDULED_CHECK );
		    break;
        }

	default:
		ConfLog( FALSE, "[ProcNmsMcuSchedConf] Wrong message %u(%s) received in state %u!\n", 
			            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
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
        ConfLog( FALSE, "Conference %s create failed because bitrate(Vid.%d, Aud.%d) error!\n", tConfInfo.GetConfName(), tConfInfo.GetBitRate(), GetAudioBitrate(tConfInfo.GetMainAudioMediaType()) );
        return FALSE;
    }

    if ( !bTemplate )
    {
        //�����E164�����Ѵ��ڣ��ܾ� 
	    if( g_cMcuVcApp.IsConfE164Repeat( tConfInfo.GetConfE164(), bTemplate ) )
	    {
            wErrCode = ERR_MCU_CONFE164_REPEAT;
            if ( bConfFromFile )
            {
                g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
            }      
            ConfLog( FALSE, "Conference %s E164 repeated and create failure!\n", tConfInfo.GetConfName() );
            return FALSE;
        }

	    //�������Ѵ��ڣ��ܾ�
	    if( g_cMcuVcApp.IsConfNameRepeat( tConfInfo.GetConfName(), bTemplate ) )
	    {
            wErrCode = ERR_MCU_CONFNAME_REPEAT;
            if ( bConfFromFile )
            {
                g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
            }      
		    ConfLog( FALSE, "Conference %s name repeated and create failure!\n", m_tConf.GetConfName() );
		    return FALSE;
	    }

	    //�����ѽ������ܾ� 
	    if( tConfInfo.GetDuration() != 0 && 
		    ( time( NULL ) > tConfInfo.GetStartTime() + tConfInfo.GetDuration() * 60 ))
	    {
            wErrCode = ERR_MCU_STARTTIME_WRONG;
            if ( bConfFromFile )
            {
                g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
            }      
		    ConfLog( FALSE, "Conference %s too late and canceled!\n", tConfInfo.GetConfName() );
		    return FALSE;
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
            ConfLog( FALSE, "Conference %s create failed because of repeat conference ID!\n", tConfInfo.GetConfName() );
            return FALSE;
        }        
    }


	//dynamic vmp and vmp module conflict
	if( tConfInfo.GetConfAttrb().IsHasVmpModule() && 
		CONF_VMPMODE_AUTO == tConfInfo.m_tStatus.GetVMPMode() )
	{
        wErrCode = ERR_MCU_DYNAMCIVMPWITHMODULE;
		ConfLog( FALSE, "Conf or temp %s create failed because has module with dynamic vmp!\n", tConfInfo.GetConfName() );
		return FALSE;		
	}

	//��Ч�Ļ��鱣����ʽ,�ܾ�
	if( tConfInfo.m_tStatus.GetProtectMode() > CONF_LOCKMODE_LOCK )
	{
        wErrCode = ERR_MCU_INVALID_CONFLOCKMODE;
		ConfLog( FALSE, "Conf or temp %s protect mode invalid and nack!\n", tConfInfo.GetConfName() );
		return FALSE;
	}

	//��Ч�Ļ�����в���,�ܾ�
	if( tConfInfo.m_tStatus.GetCallMode() > CONF_CALLMODE_TIMER )
	{
        wErrCode = ERR_MCU_INVALID_CALLMODE;
		ConfLog( FALSE, "Conf or temp %s call mode invalid and nack!\n", tConfInfo.GetConfName() );
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
                ConfLog( FALSE, "Conference %s not config gk in gk charge conf and nack!\n", tConfInfo.GetConfName() );
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
                    ConfLog( FALSE, "Conference %s configed gk support no charge conf and nack!\n", tConfInfo.GetConfName() );
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
        !((tConfInfo.GetCapSupport().GetDStreamMediaType() == VIDEO_DSTREAM_MAIN &&
           tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264) ||
          tConfInfo.GetCapSupport().GetDStreamMediaType() == VIDEO_DSTREAM_H264_H239 ||
          tConfInfo.GetCapSupport().GetDStreamMediaType() == VIDEO_DSTREAM_H264) &&
        tConfInfo.GetCapSupport().GetDStreamMediaType() != VIDEO_DSTREAM_H264_H263PLUS_H239 &&
		// VCS����Ĭ�϶�����������
		tConfInfo.GetConfSource() != VCS_CONF)
    {
        ttmpAttrb.SetUseAdapter(FALSE);
        tConfInfo.SetConfAttrb(ttmpAttrb);
        ConfLog(FALSE, "template/conf %s need not bas adapter, auto modify it in conf attrib.\n", tConfInfo.GetConfName());
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
            ConfLog(FALSE, " conf.%d need vmp auto adapt, open.\n", m_byConfIdx );
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
            ConfLog( FALSE, "template/conf %s not support H264-HD in MCU8000B-NoHD\n", tConfInfo.GetConfName() );
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
            ConfLog( FALSE, "Conf or temp.%s create failed because of multicast address invalid!\n", tConfInfo.GetConfName() );
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
        BOOL32 bAdpNeeded = FALSE;
        if (tConfInfo.GetSecBitRate() != 0 ||
            !tConfInfo.GetCapSupport().GetSecondSimCapSet().IsNull())
        {
            // ˫��˫��ʽ���鲻��ѡ���ɢ���鷽ʽ
            bAdpNeeded = TRUE;

			// FIXME: ���Ƿ�ɢ���飬��ʱ֧��˫��ʽ���飬�Ժ�Ͷ��������ǻ����ϸ����ֿ���
			bAdpNeeded = FALSE;
        }
        
        // ������ֻ�ܿ��Լ�
        if (ttmpAttrb.GetSpeakerSrc() != CONF_SPEAKERSRC_SELF)
        {
            ttmpAttrb.SetSpeakerSrc( CONF_SPEAKERSRC_SELF );
            ConfLog( FALSE, "Conf or temp.%s change speakersrc to self.\n", tConfInfo.GetConfName() );
        }
        // ����������
        if (ttmpAttrb.IsUseAdapter())
        {
            ttmpAttrb.SetUseAdapter(FALSE);
            ConfLog( FALSE, "Conf or temp.%s no use adapter\n", tConfInfo.GetConfName() );
        }
        // �����ۻ���
        if (ttmpAttrb.IsDiscussConf())
        {
            ttmpAttrb.SetDiscussConf(FALSE);
            ConfLog( FALSE, "Conf or temp.%s no start discuss\n", tConfInfo.GetConfName() );
        }
        // �������ش�
        if (ttmpAttrb.IsResendLosePack())
        {
            ttmpAttrb.SetPrsMode(FALSE);
            ConfLog( FALSE, "Conf or temp.%s no use prs\n", tConfInfo.GetConfName() );
        }
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
        
        if (bIpInvalid || bAdpNeeded)
        {                
            if (bIpInvalid)
            {
                wErrCode = ERR_MCU_DCASTADDR_INVALID;
                ConfLog( FALSE, "Conf or temp.%s create failed because of distributed conf multicast address invalid!\n", tConfInfo.GetConfName() );
            }
            else
            {
                wErrCode = ERR_MCU_DCAST_NOADP;
                ConfLog( FALSE, "Conf or temp.%s create failed because of distributed conf not support second rate or format!\n", tConfInfo.GetConfName() );
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
            ConfLog( FALSE, "Conf or temp.%s not support multicast low stream\n", tConfInfo.GetConfName() );
            wErrCode = ERR_MCU_MCLOWNOSUPPORT;
            return FALSE;
        }
        
        // ��֧�ֵ���¼��
        TConfAutoRecAttrb tRecAttrb = tConfInfo.GetAutoRecAttrb();
        if ( tRecAttrb.IsAutoRec() && tRecAttrb.GetRecParam().IsRecLowStream() )
        {
            ConfLog( FALSE, "Conf or temp.%s can't support auto record low stream\n", tConfInfo.GetConfName() );
            wErrCode = ERR_MCU_RECLOWNOSUPPORT;
            return FALSE;    
        }
    }
#endif

    //zbq[11/18/2008] mau��صĴ���������������ʾ
    if (tConfInfo.GetConfSource() != VCS_CONF &&
        ((IsHDConf(tConfInfo) &&
		  (0 != tConfInfo.GetSecBitRate() ||
		   (MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType() && 0 != tConfInfo.GetSecVideoMediaType()) ||
		   tConfInfo.GetConfAttrbEx().IsResEx1080() ||
		   tConfInfo.GetConfAttrbEx().IsResEx720()  ||
		   tConfInfo.GetConfAttrbEx().IsResEx4Cif() ||
		   tConfInfo.GetConfAttrbEx().IsResExCif()
		  )
		 )
		 ||
		 // ˫˫��
		 (tConfInfo.GetCapSupport().IsDStreamSupportH239() &&
		  MEDIA_TYPE_H264 == tConfInfo.GetDStreamMediaType() &&
		  (g_cMcuVcApp.IsSupportSecDSCap() || MEDIA_TYPE_NULL != tConfInfo.GetCapSupportEx().GetSecDSType())
		 )
        )
	   )
    {
        if (!m_cBasMgr.IsHdBasSufficient(tConfInfo))
        {
            ConfLog(FALSE, "temp.%s's mau might be insufficient\n", tConfInfo.GetConfName());
            NotifyMcsAlarmInfo(cMsg.GetSrcSsnId(), ERR_MCU_NOENOUGH_HDBAS_TEMP);
        }

        TMcuHdBasStatus tHdBasStatus;
        m_cBasMgr.GetHdBasStatus(tHdBasStatus, tConfInfo);

        CServMsg cServMsg;
        cServMsg.SetMsgBody((u8*)&tHdBasStatus, sizeof(tHdBasStatus));
        cServMsg.SetEventId(MCU_MCS_MAUSTATUS_NOTIFY);
        SendMsgToMcs(cMsg.GetSrcSsnId(), MCU_MCS_MAUSTATUS_NOTIFY, cServMsg);
    }

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
	07/12/25    4.0         �ű���          ����
    08/11/18    4.5         �ű���          ���Ӷ�MAU��У��
====================================================================*/
void CMcuVcInst::RlsAllBasForConf()
{
	if (m_tConf.m_tStatus.IsAudAdapting() || EQP_CHANNO_INVALID != m_byAudBasChnnl)
	{
		StopBasAdapt(ADAPT_TYPE_AUD);
        g_cMcuVcApp.SetBasChanStatus(m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl, TBasChnStatus::READY);
        g_cMcuVcApp.SetBasChanReserved(m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl, FALSE);
	}

    if (m_tConf.m_tStatus.IsVidAdapting() || EQP_CHANNO_INVALID != m_byVidBasChnnl)
	{
		StopBasAdapt(ADAPT_TYPE_VID);
        g_cMcuVcApp.SetBasChanStatus(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, TBasChnStatus::READY);
        g_cMcuVcApp.SetBasChanReserved(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, FALSE);
	}

    if (m_tConf.m_tStatus.IsBrAdapting() || EQP_CHANNO_INVALID != m_byBrBasChnnl)
	{
		StopBasAdapt(ADAPT_TYPE_BR);
        g_cMcuVcApp.SetBasChanStatus(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, TBasChnStatus::READY);
        g_cMcuVcApp.SetBasChanReserved(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, FALSE);
	}

    if (m_tConf.m_tStatus.IsCasdAudAdapting() || EQP_CHANNO_INVALID != m_byCasdAudBasChnnl)
	{
		StopBasAdapt(ADAPT_TYPE_CASDAUD);
        g_cMcuVcApp.SetBasChanStatus(m_tCasdAudBasEqp.GetEqpId(), m_byCasdAudBasChnnl, TBasChnStatus::READY);
        g_cMcuVcApp.SetBasChanReserved(m_tCasdAudBasEqp.GetEqpId(), m_byCasdAudBasChnnl, FALSE);
	}

    if (m_tConf.m_tStatus.IsCasdVidAdapting() || EQP_CHANNO_INVALID != m_byCasdVidBasChnnl)
	{
		StopBasAdapt(ADAPT_TYPE_CASDVID);
        g_cMcuVcApp.SetBasChanStatus(m_tCasdVidBasEqp.GetEqpId(), m_byCasdVidBasChnnl, TBasChnStatus::READY);
        g_cMcuVcApp.SetBasChanReserved(m_tCasdVidBasEqp.GetEqpId(), m_byCasdVidBasChnnl, FALSE);
	}

    //�ͷŸ�������ͨ��
    StopHDMVBrdAdapt();
    StopHDDSBrdAdapt();
    StopHDCascaseAdp();

    //�ͷ�mau
    m_cBasMgr.ReleaseHdBasChn();

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
    if ( IsHDConf(m_tConf) )
    {
        //zbq[11/18/2008] MAUУ�� �� ռ��
        if (!m_cBasMgr.IsHdBasSufficient(m_tConf))
        {
            ConfLog(FALSE, "Conf.%s's mau is insufficient\n", m_tConf.GetConfName());
            *pwErrCode = ERR_MCU_NOENOUGH_HDBAS_CONF;
            return FALSE;
        }

        if (!m_cBasMgr.OcuppyHdBasChn(m_tConf))
        {
            ConfLog(FALSE, "Conf.%s's ocuppy mau(s) failed!\n", m_tConf.GetConfName());
            *pwErrCode = ERR_MCU_OCUPPYHDBAS;
            return FALSE;
        }
    }
    else
    {
        //��������
        u8     byEqpId;
        u8     byChnIdx;
        
        BOOL32 bVidBasCap = TRUE;
        BOOL32 bBrBasCap  = TRUE;
        
        TPeriEqpStatus tStatus;
        u8     byMainMediaType;
        u8     bySecondMediaType;
        u8     byMainVidFormat;
        u8     bySecVidFormat;

        byMainMediaType = m_tConf.GetMainVideoMediaType();
        bySecondMediaType = m_tConf.GetSecVideoMediaType();
        byMainVidFormat = m_tConf.GetMainVideoFormat();
        bySecVidFormat = m_tConf.GetSecVideoFormat();
        if ( (MEDIA_TYPE_NULL != bySecondMediaType && byMainMediaType != bySecondMediaType)
            ||(byMainMediaType == bySecondMediaType && byMainVidFormat != bySecVidFormat) )
        {
            if (g_cMcuVcApp.GetIdleBasChl(ADAPT_TYPE_VID, byEqpId, byChnIdx))
            {
                m_tVidBasEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_BAS);
                m_tVidBasEqp.SetConfIdx(m_byConfIdx);
                m_byVidBasChnnl = byChnIdx;
                EqpLog("m_byVidBasChnnl = %d\n", m_byVidBasChnnl);
            }
            else
            {
                bVidBasCap = FALSE;
                ConfLog(FALSE, "no idle video adapte channel!\n");
            }
        }
		
        if (0 != m_tConf.GetSecBitRate())
        {
        #ifdef _MINIMCU_
            // 8000B/8000C ˫ý�����
            if (m_byVidBasChnnl != EQP_CHANNO_INVALID)
            {      
                m_byIsDoubleMediaConf = 1;
                ConfLog(FALSE, "conf uses double media@bitrate support!\n");
                //�������䲻��Ҫ
            }
            else
            {
        #endif                    
                if (g_cMcuVcApp.GetIdleBasChl(ADAPT_TYPE_BR, byEqpId, byChnIdx))
                {
                    m_tBrBasEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_BAS);
                    m_tBrBasEqp.SetConfIdx(m_byConfIdx);
                    m_byBrBasChnnl = byChnIdx;
                    EqpLog("m_byBrBasChnnl = %d\n", m_byBrBasChnnl);
                }
                else
                {
                    bBrBasCap = FALSE;
                    ConfLog(FALSE, "no idle bitrate adapte channel!\n");
                }
        #ifdef _MINIMCU_
            }
        #endif
        }

        if (/*!bAudBasCap || */!bVidBasCap || !bBrBasCap)
        {
            *pwErrCode = ERR_MCU_NOIDLEADAPTER;
            ConfLog(FALSE, "Conference %s create failed because no idle VID/BR adapter!\n",
                    m_tConf.GetConfName());
            return FALSE;
        }

        //����˫˫�� ���� �� ����SXGA|fps>5˫������
        if((m_tConf.GetCapSupportEx().GetSecDSType() != MEDIA_TYPE_NULL &&
            m_tConf.GetCapSupportEx().GetSecDSType() != 0))
        {
            BOOL32 bMpuSufficient = FALSE;
            BOOL32 bMauSufficient = FALSE;
            
            if (!m_cBasMgr.IsHdBasSufficient(m_tConf))
            {
                *pwErrCode = ERR_MCU_NOENOUGH_HDBAS_CONF;
                ConfLog(FALSE, "Conf.%s's has no enough mpu or mau!\n", m_tConf.GetConfName());
                return FALSE;
            }
            if (!m_cBasMgr.OcuppyHdBasChn(m_tConf))
            {
                *pwErrCode = ERR_MCU_OCUPPYMPU;
                ConfLog(FALSE, "Conf.%s's ocuppy mpu chnnl failed!\n", m_tConf.GetConfName());
                return FALSE;
            }
        }
    }

    //��Ƶ
    u8     byEqpId;
    u8     byChnIdx;
    BOOL32 bAudBasCap = TRUE;
    u8     byMainAudType;
    u8     bySecondAudType;
    byMainAudType = m_tConf.GetMainAudioMediaType();
    bySecondAudType = m_tConf.GetSecAudioMediaType();
    if (MEDIA_TYPE_NULL != bySecondAudType && byMainAudType != bySecondAudType)
    {
        if (g_cMcuVcApp.GetIdleBasChl(ADAPT_TYPE_AUD, byEqpId, byChnIdx))
        {
            m_tAudBasEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_BAS);
            m_tAudBasEqp.SetConfIdx(m_byConfIdx);
            m_byAudBasChnnl = byChnIdx;
            EqpLog("m_byAudBasChnnl = %d\n", m_byAudBasChnnl);
        }
        else
        {
            bAudBasCap = FALSE;
            ConfLog(FALSE, "no idle audio adapte channel!\n");
        }
    }
    
    if (!bAudBasCap)
    {
        *pwErrCode = ERR_MCU_NOIDLEADAPTER;
        ConfLog(FALSE, "Conference %s create failed because no idle AUD adapter!\n",
                m_tConf.GetConfName());
        
        //�ͷŸ�������
        m_cBasMgr.ReleaseHdBasChn();

        //�ͷű�������ͨ��
        if (m_tBrBasEqp.IsNull())
        {
            g_cMcuVcApp.SetBasChanStatus(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, TBasChnStatus::READY);
            g_cMcuVcApp.SetBasChanReserved(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, FALSE);
        }
        if (m_tVidBasEqp.IsNull())
        {
            g_cMcuVcApp.SetBasChanStatus(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, TBasChnStatus::READY);
            g_cMcuVcApp.SetBasChanReserved(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, FALSE);
        }

        return FALSE;
    }

	return TRUE;
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
    memset(&abyVMPId, 0, sizeof(abyVMPId));
    g_cMcuVcApp.GetIdleVMP(abyVMPId, byIdleVMPNum, sizeof(abyVMPId));

    TVMPParam tVmpParam = m_tConfEqpModule.GetVmpModule().GetVmpParam();

    if ( m_tConf.GetConfId().IsNull() )
    {
        ConfLog( FALSE, "[EqpInfoCheck] tConfInfo.IsNull, ignore it\n" );
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
            ConfLog( FALSE, "Conference %s create failed because of multicast address occupied!\n", m_tConf.GetConfName() );
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
            ConfLog( FALSE, "Conference %s create failed because of distributed conf multicast address occupied!\n", m_tConf.GetConfName() );
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
            ConfLog( FALSE, "Conference %s create failed because of no DCS registered!\n", m_tConf.GetConfName() );
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
                ConfLog( FALSE, "the master mcu couldn't restore VMP due to no VMP !\n" );

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
                    ConfLog( FALSE, "Conf.%s created by NPlus error which is impossible, check it !\n", m_tConf.GetConfName() );
                }
            }
            else
            {
                wErrCode = ERR_MCU_NOIDLEVMP;
                ConfLog( FALSE, "Conference %s create failed because no idle vmp!\n", m_tConf.GetConfName() );
                return FALSE;
            }
		}
        else
        {
            m_tConf.m_tStatus.SetVMPMode(CONF_VMPMODE_CTRL);
            m_tConf.m_tStatus.SetVmpBrdst(m_tConfEqpModule.GetVmpModule().m_tVMPParam.IsVMPBrdst());
        }
	}
    
    if( tConfAttrib.IsHasTvWallModule() )
    {
        BOOL32 bTvWallDisconnected = FALSE;  //  �Ƿ���ڵ���ǽ������
        TMultiTvWallModule tMultiTvWallModule;
        m_tConfEqpModule.GetMultiTvWallModule( tMultiTvWallModule );
        TTvWallModule tTvWallModule;
        for( u8 byTvLp = 0; byTvLp < tMultiTvWallModule.GetTvModuleNum(); byTvLp++ )
        {
            tMultiTvWallModule.GetTvModuleByIdx(byTvLp, tTvWallModule);
            if( !g_cMcuVcApp.IsPeriEqpConnected( tTvWallModule.m_tTvWall.GetEqpId()) )
            {
                bTvWallDisconnected = TRUE;
                break;
            }
        }
        if( bTvWallDisconnected )
        {
            memset((void*)&m_tConfEqpModule, 0, sizeof(m_tConfEqpModule));
            wErrCode = ERR_MCU_NOIDLETVWALL;
			ConfLog( FALSE, "Conference %s create failed because Tv %d is outline!\n", 
                                m_tConf.GetConfName(), tTvWallModule.m_tTvWall.GetEqpId() );
			return FALSE;
        }
    }
	
	//�Ƿ�ʼ����, Ԥ�������� 
	if( tConfAttrib.IsDiscussConf() )
	{
        // ���񻪣��������ȼ���Ƿ��л����� [4/29/2006]
        // �������ٿ�ʼ����
        u8 byEqpId = 0;
	    u8 byGrpId = 0;
        g_cMcuVcApp.GetIdleMixGroup( byEqpId, byGrpId );
        
        if (0 == byEqpId)
		{
            if( CONF_CREATE_MT == m_byCreateBy ) //�ն˴��᲻ֱ�ӷ���
            {
                tConfAttrib.SetDiscussConf(FALSE);
                m_tConf.SetConfAttrb(tConfAttrib);
            }
            else
            {
                wErrCode = ERR_MCU_NOIDLEMIXER;
				ConfLog( FALSE, "Conference %s create failed because no idle mixer!\n", m_tConf.GetConfName() );
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
		if (!AssignPrsChnnl())
        {
			// ������Դʧ��
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

    //�趯̬�����Ļ��鿪ʼ����ϳ� 
	if( CONF_VMPMODE_AUTO == m_tConf.m_tStatus.GetVMPMode() || 
		tConfAttrib.IsHasVmpModule() )			
	{
		//�Ƿ��л���ϳ��� 
		if( 0 != byIdleVMPNum )
		{
            //����֧�ֵ�ǰ�ϳɷ��Ŀ���VMP��ID
            u8 byVmpId = 0;

            if ( CONF_VMPMODE_AUTO != m_tConf.m_tStatus.GetVMPMode() )
            {
                //��ǰ���еĿ���VMP�Ƿ�֧������ĺϳɷ��
                u16 wError = 0;
                if ( IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError) ) 
                {
                    m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
                    m_tVmpEqp.SetConfIdx( m_byConfIdx );
                }
                else
                {
                    if ( CONF_CREATE_NPLUS == m_byCreateBy )
                    {
                        NotifyMcsAlarmInfo( cSrcMsg.GetSrcSsnId(), ERR_MCU_ALLIDLEVMP_NO_SUPPORT_NPLUS );
                        ConfLog( FALSE, "Conference %s create failed because all the idle vmp can't support the VMP style!\n", m_tConf.GetConfName() );

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
                            ConfLog( FALSE, "Conf.%s created by NPlus error which is impossible, check it !\n", m_tConf.GetConfName() );
                        }
                    }
                    else
                    {
                        g_cMcuVcApp.RemoveConf( m_byConfIdx );    
                        wErrCode = wError;
                        ConfLog( FALSE, "Conference %s create failed because all the idle vmp can't support the VMP style!\n", m_tConf.GetConfName() );
                        return FALSE;
                    }
                }
            }
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
                }
                else
                {
                    // N+1�����Զ�����ϳɻָ�ʧ�ܣ�����ǰ���е�״̬ [01/16/2007-zbq]
                    if ( CONF_CREATE_NPLUS == m_byCreateBy ) 
                    {
                        NotifyMcsAlarmInfo( cSrcMsg.GetSrcSsnId(), ERR_MCU_ALLIDLEVMP_NO_SUPPORT_NPLUS );
                        ConfLog( FALSE, "Conference %s create failed because all the idle vmp can't support the VMP style!\n", m_tConf.GetConfName() );
                        
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
                            ConfLog( FALSE, "Conf.%s created by NPlus error which is impossible, check it !\n", m_tConf.GetConfName() );
                        }                                
                    }
                    else
                    {
                        //��������Ĵ����������ڵ�VMP�������߼��ͱ�������������������������[01/16/2007-zbq]
                        ConfLog( FALSE, "Conf.%s created which is impossible, check it !\n", m_tConf.GetConfName() );
                        wErrCode = ERR_MCU_NOIDLEVMP;
                        g_cMcuVcApp.RemoveConf( m_byConfIdx );    
                        ConfLog( FALSE, "Conference %s create failed because no idle vmp!\n", m_tConf.GetConfName() );
                        return FALSE;
                    }
                }
			}
            
            TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TRUE;//��ռ��,��ʱ��δ�ɹ��ٷ���
			tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpParam;
			tPeriEqpStatus.SetConfIdx( m_byConfIdx );
			g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			
			m_tConf.m_tStatus.SetVmpStyle( tVmpParam.GetVMPStyle() );
			
			u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType; //��ȡ������
			u8 byMaxVmpMem = 0;
			if(byVmpSubType != VMP) //��VMP����Ա����Ϊ20
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
            }                                     
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
            g_cMcuVcApp.RemoveConf( m_byConfIdx );
			wErrCode = ERR_MCU_NOIDLEVMP;
			ConfLog( FALSE, "Conference %s create failed because no idle vmp!\n", m_tConf.GetConfName() );
			return FALSE;
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
	BOOL32 bInMtTable ; // xliang [12/26/2008] 

    wAliasBufLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
    pszAliaseBuf = (char*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16));
    
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
			byMtId = AddMt( tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], m_tConf.m_tStatus.GetCallMode() );
            
			if(!bInMtTable)
			{
				m_byMtIdNotInvite = byMtId; // xliang [12/26/2008] �������ն��Ժ�Ӧ��Ϊ����MT
				CallLog("[ConfdataCoordinate] Mt.%u should not be in Invite table\n", m_byMtIdNotInvite);
			}
				// zbq [12/19/2007] ֱ�Ӵ����ն����ڴ˲���IP������E164��������µ�©��
            if ( tMtAliasArray[byLoop-1].m_AliasType != mtAliasTypeTransportAddress )
            {
                m_ptMtTable->SetMtAlias( byMtId, ptMtAddr );
                m_ptMtTable->SetIPAddr( byMtId, ptMtAddr->m_tTransportAddr.GetIpAddr() );                    
            }

            // xsl [11/8/2006] ������ն˼�����1
			// xliang [2/4/2009] ������MT����MCU
			u8 byDriId = cServMsg.GetSrcDriId();
			g_cMcuVcApp.IncMtAdpMtNum( byDriId, m_byConfIdx, byMtId );
            m_ptMtTable->SetDriId(byMtId, byDriId);
			if(byMtType == TYPE_MCU)
			{
				//�������ն˼�����Ҫ+1
				//��mtadplib�Ƕ���ͨ���жϣ����Կ϶��ж���2�����������ռ��
				g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum++;
			}
		}
		else
		{
			//FIXME: �Ժ������Ƿ�ɢ�����ֿ���
			if (tMtAliasArray[byLoop-1].m_AliasType == mtAliasTypeTransportAddress &&
				tMtAliasArray[byLoop-1].m_tTransportAddr.GetIpAddr() == 0xac103c80 &&
				0 != m_tConf.GetSecBitRate())
			{
				awMtDialRate[byLoop-1] = m_tConf.GetSecBitRate();
			}

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
				m_ptMtTable->SetMtType(byMtId, MT_TYPE_MMCU);
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
				if( byMtIndex > 0 )//������1Ϊ��׼
				{
                    if( byMtIndex <= MAXNUM_CONF_MT )
                    {
                        byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAliasArray[byMtIndex-1] );
                        if( byMtId > 0 )
                        {
                            TMt tMt = m_ptMtTable->GetMt( byMtId );
                            m_tConfEqpModule.SetTvWallMemberInTvInfo( tTvWallModule.m_tTvWall.GetEqpId(), byLoop, tMt );
                        
                            // ���� [5/11/2006] ����֧��ָ�����淽ʽ
                            // tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = TW_MEMBERTYPE_MCSSPEC;
                            tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byMemberType;
                            tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
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
                            tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byMemberType;
                            tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
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
		for( byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
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

						// xliang [4/8/2009] С��SetVmpMember�ӿڵ����⡣tVmpParamҪ��һ��
						TVMPMember tVmpMember;
						tVmpMember.SetMemberTMt( tMt );
						tVmpMember.SetMemberType( byMemberType ); //�˴�member type�ǻ��ָ��
						tVmpParam.SetVmpMember( byLoop, tVmpMember );
						
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

						// xliang [4/8/2009] С��SetVmpMember�ӿڵ����⡣tVmpParamҪ��һ��
						TVMPMember tVmpMember;
						tVmpMember.SetMemberTMt( tMt );
						tVmpMember.SetMemberType( byMemberType ); 
						tVmpParam.SetVmpMember( byLoop, tVmpMember );
                    }                          
                }
                else
                {
                    ConfLog(FALSE, "Invalid MtIndex :%d in CreateConf Vmp Module\n", byMtIndex);
                }
			}
		}

		ptVmpModule->SetVmpParam( tVmpParam );
		m_tConfEqpModule.SetVmpModule( *ptVmpModule );
	}
    else
    {
        TVmpModule tVmpModule;
        memset( &tVmpModule, 0, sizeof(tVmpModule) );
        m_tConfEqpModule.SetVmpModule( tVmpModule );
		TMt tNullMt;
        tNullMt.SetNull();
        for( byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
		{
			m_tConfEqpModule.SetVmpMember( byLoop, tNullMt );
		}
    }

    //zbq[11/18/2008] ��������Ŀ��֧��
    TConfAttrb tAttrb = m_tConf.GetConfAttrb();
    
    if (0 != m_tConf.GetSecBitRate() ||
        (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() &&
         0 != m_tConf.GetSecVideoMediaType()))
    {
        tAttrb.SetUseAdapter(TRUE);
    }
    else
    {
        if (
            (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
             (m_tConf.GetConfAttrbEx().IsResEx1080() ||
              m_tConf.GetConfAttrbEx().IsResEx720()  ||
              m_tConf.GetConfAttrbEx().IsResEx4Cif() ||
              m_tConf.GetConfAttrbEx().IsResExCif())
            ) 
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
	if(m_tConf.m_tStatus.IsRegToGK())
	{
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

	//����ע��GK�Ժ�������ն�
	if(m_tConfInStatus.IsInviteOnGkReged())
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
                GetNPlusDataFromConf( tConfData );
                cServMsg.SetConfId( m_tConf.GetConfId() );
                cServMsg.SetMsgBody( (u8*)&tConfData, sizeof(tConfData) );
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
        ConfLog( FALSE, "[ProcGKChargeRsp] unexpected msg.%d<%s> received !\n", 
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
			MtLog("[ProcMcsMcuReleaseConfReq] VCS Conf cann't be released by mt\n");
			return;
		}
		
		
		//Ӧ��
		if( cServMsg.GetEventId() == MCS_MCU_RELEASECONF_REQ )
		{		
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}      

		//��������
		ReleaseConf( TRUE );
        
        NEXTSTATE( STATE_IDLE );
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
		ConfLog(FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
				OspPrintf( TRUE, FALSE, "\n\n Cannot get speaker's Mt Alias.\n\n");
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
				OspPrintf( TRUE, FALSE, "\n\n Cannot get chairman's Mt Alias.\n\n");
			}
		}
		
		//���浽�ļ�
		g_cMcuVcApp.SaveConfToFile( m_byConfIdx, FALSE, bySaveDefault );
			
		break;

	default :
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
        
		//if too old, send Nack; Duration time is 0 denoted that meeting could be ended at any time
		if( ( time( NULL ) > tConf.GetStartTime() + 30 * 60 || 
			  ( tConf.GetDuration() != 0 && 
			    time( NULL ) > tConf.GetStartTime() + tConf.GetDuration() * 60 ) ) )
		{
			cServMsg.SetErrorCode( ERR_MCU_STARTTIME_WRONG );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfLog( FALSE, "Conference %s too late and canceled!\n", m_tConf.GetConfName() );
			return;
		}
		
		//�����E164�����Ѵ��ڣ��ܾ� 
		if( 0 != strcmp( (char*)tConf.GetConfE164(), (char*)m_tConf.GetConfE164() ) &&
			g_cMcuVcApp.IsConfE164Repeat( tConf.GetConfE164(), FALSE ) )
		{
			cServMsg.SetErrorCode( ERR_MCU_CONFE164_REPEAT );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfLog( FALSE, "Conference %s E164 repeated and create failure!\n", m_tConf.GetConfName() );
			return;
		}

		//�������Ѵ��ڣ��ܾ�
		if( 0 != strcmp( (char*)tConf.GetConfName(), (char*)m_tConf.GetConfName() ) &&
			g_cMcuVcApp.IsConfNameRepeat( tConf.GetConfName(), FALSE ) )
		{
			cServMsg.SetErrorCode( ERR_MCU_CONFNAME_REPEAT );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfLog( FALSE, "Conference %s name repeated and create failure!\n", m_tConf.GetConfName() );
			return;
		}

		//dynamic vmp and vmp module conflict
		if( tConfAttrib.IsHasVmpModule() && tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_AUTO)
		{
			cServMsg.SetErrorCode( ERR_MCU_DYNAMCIVMPWITHMODULE );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfLog( FALSE, "Conference %s create failed because has module with dynamic vmp!\n", m_tConf.GetConfName() );
			return;		
		}

	    //��Ч�Ļ��鱣����ʽ,�ܾ�
		if( tConf.m_tStatus.GetProtectMode() > CONF_LOCKMODE_LOCK )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_CONFLOCKMODE );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfLog( FALSE, "Conference %s protect mode invalid and nack!\n", m_tConf.GetConfName() );
			return;
		}

		//��Ч�Ļ�����в���,�ܾ�
		if( tConf.m_tStatus.GetCallMode() > CONF_CALLMODE_TIMER )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_CALLMODE );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			ConfLog( FALSE, "Conference %s call mode invalid and nack!\n", m_tConf.GetConfName() );
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
		
		ConfLog( FALSE, "A scheduled conference %s modified and restart!\n",m_tConf.GetConfName() );

		//release conference
		ReleaseConf( TRUE );

		NEXTSTATE( STATE_IDLE );	
		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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

            ConfLog(FALSE, "Conf %s is manual release. Can not delay!\n", m_tConf.GetConfName());
            return;
        }

        if (MT_MCU_DELAYCONF_REQ == pcMsg->event)
        {
            TMt tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
            if (!(tMt == m_tConf.GetChairman()))
            {
                ConfLog(FALSE, "Conf %s has delayed fail by no chairman!\n", m_tConf.GetConfName());
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
		m_tConf.SetDuration( m_tConf.GetDuration() + ntohs( wDelayTime ) );        

		//Notification
		BroadcastToAllSubMtJoinedConf( MCU_MT_DELAYCONF_NOTIF, cServMsg ); 		
		SendMsgToAllMcs( MCU_MCS_DELAYCONF_NOTIF, cServMsg );
		
		if( pcMsg->event == MT_MCU_DELAYCONF_REQ )
		{
			ConfLog( FALSE, "Conf %s has delayed %d minute by chairman!\n", 
				 m_tConf.GetConfName(),  ntohs( wDelayTime ) );
		}
		else
		{
			ConfLog( FALSE, "Conf %s has delayed %d minute by mcs!\n", 
				 m_tConf.GetConfName(),  ntohs( wDelayTime ) );
		}

		
		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
            ConfLog( FALSE, "[ProcMcsMcuChangeVacHoldTimeReq] distributed conf not supported vac\n");
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
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
			ConfLog( FALSE, "Same chairman specified in conference %s! failure!\n", 
				m_tConf.GetConfName() );
			cServMsg.SetErrorCode( ERR_MCU_SAMECHAIRMAN );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	//nack
			return;
		}

		//����ϯ�����
		if( !m_tConfAllMtInfo.MtJoinedConf( tNewChairman.GetMtId() ) )
		{
			ConfLog( FALSE, "New chairman MT%u not in conference %s! Error!\n", 
				    tNewChairman.GetMtId(), m_tConf.GetConfName() );
			cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	//nack
			return;
		}

		//Ӧ��
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );	//ack

		//�ı���ϯ
		ChangeChairman( &tNewChairman );

		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
				
        //Ӧ��
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );


		//ȡ����ϯ
		ChangeChairman( NULL );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
	CServMsg	cServMsg( pcMsg->content, pcMsg->length ), cTempMsg;
	TMt			tNewSpeaker;
	TMt         tRealMt;
	TMtStatus	tMtStatus;

	STATECHECK

    cServMsg.SetErrorCode(0);
		
	//�����ݽ�ģʽ�������������Ʒ���״̬,����ָ��������
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
	if( m_tConf.m_tStatus.IsPlaying() )
	{
		cServMsg.SetErrorCode( ERR_MCU_CONFPLAYING );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		ConfLog( FALSE, "It playing now in conference %s! Cannot spcify the speaker!\n", 
			   m_tConf.GetConfName() );
		return;
	}

	tNewSpeaker = *( TMt * )cServMsg.GetMsgBody();
	tRealMt = tNewSpeaker;
	if( tNewSpeaker.GetMcuId() == LOCAL_MCUID )
	{
		tNewSpeaker = m_ptMtTable->GetMt( tNewSpeaker.GetMtId() );
	}
	else
	{
		tNewSpeaker = m_ptMtTable->GetMt( tNewSpeaker.GetMcuId());
	}

	//�·�����δ���
	if( tNewSpeaker.GetType() == TYPE_MT && 
		!m_tConfAllMtInfo.MtJoinedConf( tNewSpeaker.GetMcuId(), tNewSpeaker.GetMtId() ) )
	{
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		ConfLog( FALSE, "New speaker MT%u not in conference %s! Error!\n", 
			   tNewSpeaker.GetMtId(), m_tConf.GetConfName() );
		return;
	}

	//�·�������ֻ�����ն�
	m_ptMtTable->GetMtStatus(tNewSpeaker.GetMtId(), &tMtStatus);
	if( tNewSpeaker.GetType() == TYPE_MT && 
		( !tMtStatus.IsSendVideo() ) && 
		( !tMtStatus.IsSendAudio() ) )
	{
		cServMsg.SetErrorCode( ERR_MCU_RCVONLYMT );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	
		ConfLog( FALSE, "New speaker MT%u is receive only MT! Error!\n", 
				         tNewSpeaker.GetMtId() );
		return;
	}
    
    //����������飬��û����Ӧ�������������������ʾ��
    if (!IsHDConf(m_tConf) &&
        m_tConf.GetConfAttrb().IsUseAdapter())
    {
        BOOL32 byFlag = TRUE;
        u16 wAdaptBitRate = 0;
        TSimCapSet tSrcSimCapSet;
        TSimCapSet tDstSimCapSet;

        if (!tNewSpeaker.IsNull())
        {
            if (!g_cMcuVcApp.IsPeriEqpConnected(m_tVidBasEqp.GetEqpId())
                && ( IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, &tNewSpeaker) || IsNeedCifAdp()) )
            {
                byFlag &= FALSE;
            }

            if (!g_cMcuVcApp.IsPeriEqpConnected(m_tBrBasEqp.GetEqpId()) &&
                IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, &tNewSpeaker))
            {
                byFlag &= FALSE;
            }

            if (!g_cMcuVcApp.IsPeriEqpConnected(m_tAudBasEqp.GetEqpId()) &&
                IsNeedAudAdapt(tDstSimCapSet, tSrcSimCapSet, &tNewSpeaker))
            {
                byFlag &= FALSE;
            }

            if (FALSE == byFlag)
            {
                cServMsg.SetErrorCode(ERR_BASCHANNEL_INVALID);
                ConfLog(FALSE, "New speaker MT%u need bas! Error!\n", tNewSpeaker.GetMtId());
            }
        }
    }

	if(!tRealMt.IsLocal() )
	{
		//Ŀǰ�ݲ�֧���¼�mcu���ն�״̬֪ͨ,���������ж��ȹر�
		/*
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tRealMt.GetMcuId());
		if(ptConfMcInfo != NULL)
		{
			TMcMtStatus *ptStatus = ptConfMcInfo->GetMtStatus(tRealMt);
			if(ptStatus != NULL)
			{
				if( ( !ptStatus->IsSendVideo() ) && 
					( !ptStatus->IsSendAudio() ) ) 
				{
					cServMsg.SetErrorCode( ERR_MCU_RCVONLYMT );
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	
					ConfLog( FALSE, "New speaker MCU%uMT%u is receive only MT! Error!\n", 
						tRealMt.GetMcuId(), tRealMt.GetMtId() );
					return;
				}
			}
		}
		*/
	}

	//�·��������Ƿ�����
	if( tNewSpeaker == GetLocalSpeaker() )	
	{			
		ConfLog( FALSE, "Same speaker specified in conference %s! Cannot spcify the speaker!\n", 
				m_tConf.GetConfName() );
	}

    // xsl [7/20/2006] ���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����, ��֧���滻��Ҫ����Ŀ���ն��Ƿ���vmp��tvwall����ؼ�ء���ѡ����mtw���ն�¼��˫�������Ե����
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        if (IsOverSatCastChnnlNum(tNewSpeaker.GetMtId()))
        {
            ConfLog(FALSE, "[ProcMcsMcuSpecSpeakerReq] over max upload mt num. nack!\n");
            
            if( cServMsg.GetEventId() != MT_MCU_SPECSPEAKER_CMD )
            {
                cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            }            
            return;
        }
    }    

	//Ӧ��
	if( cServMsg.GetEventId() != MT_MCU_SPECSPEAKER_CMD )
	{
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	}
	
	//�ı䷢����
	ChangeSpeaker( &tRealMt );

    //�¼�Mcֻ��ָ�������ն˵ķ���
	if( !tRealMt.IsLocal() )
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tRealMt.GetMcuId());
		if(ptConfMcInfo != NULL)
		{
			TMt tMt; //�Լ�
			tMt.SetMcuId(tRealMt.GetMcuId());
			tMt.SetMtId(0);
			TMcMtStatus *ptStatus = ptConfMcInfo->GetMtStatus(tMt);
			if(ptStatus != NULL)
			{
				OnMMcuSetIn(tRealMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_BROADCAST);
                
                // ���¼������˼��붨�ƻ����б���
                if( m_tConf.m_tStatus.IsSpecMixing() )
                {
                    // xsl [7/28/2006] ���Ѿ��ڻ������ظ�����
                    if (!m_ptConfOtherMcTable->IsMtInMixing(tRealMt))
                    {
                        TMcu tMcu;
                        tMcu.SetMcu(tRealMt.GetMcuId());
                        cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
                        cServMsg.CatMsgBody((u8 *)&tRealMt, sizeof(TMt));
                        OnAddRemoveMixToSMcu(&cServMsg, TRUE);   
                    }                     
                }                
			}
		}
	}
	
	// xliang [5/26/2009] ֪ͨMCS��MT����window. FIXME
	/*
	CServMsg cMsg;
	if(MT_MCU_SPECSPEAKER_CMD == cServMsg.GetEventId())
	{

	}
	else if(MCS_MCU_SPECSPEAKER_REQ)
	{
		//֪ͨ��ϯMT������ضԻ���
		if( HasJoinedChairman() )
		{
			TMt tMt = m_tConf.GetChairman();
			//...
		}
	}
	*/

	
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
		tSpeaker = m_tConf.GetSpeaker();
		if( !m_tConf.HasSpeaker() )
		{
			if( cServMsg.GetEventId() == MCS_MCU_CANCELSPEAKER_REQ )
			{
				cServMsg.SetErrorCode( ERR_MCU_NOSPEAKER );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			}
			ConfLog( FALSE, "No speaker in conference %s now! Cannot cancel the speaker!\n", 
				m_tConf.GetConfName() );
			return;
		}
				
		//Ӧ��
		if( cServMsg.GetSrcMtId() == 0 )	//not MT source
		{
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}
		
		//ͣ�鲥
		if( m_ptMtTable->IsMtMulticasting( tSpeaker.GetMtId() ) )
		{
			g_cMpManager.StopMulticast( tSpeaker, 0, MODE_BOTH );
			m_ptMtTable->SetMtMulticasting( tSpeaker.GetMtId(), FALSE );
		}
		
		//ȡ��������
		ChangeSpeaker( NULL );

		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
                ConfLog(FALSE, "[ProcMcsMcuSeeSpeakerCmd] MtId<%d> isn't chairman, ignore it!\n");
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
            ConfLog(FALSE, "No must see speaker while mixing!\n");
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
			if(m_tConf.m_tStatus.IsBrdstVMP() && m_tVidBrdSrc == m_tVmpEqp)
			{
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				if(byVmpSubType != VMP)
				{
					bNewVmpBrd = TRUE;
				}
			}

            //�����Ƿ��й㲥Դ: ��ѡ��, �տ�(����)�㲥Դ
            for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
            {
				TMtStatus tMtStatus;
                if ( m_tConfAllMtInfo.MtJoinedConf( byMtId ) ) 
                {
                    bRestoreAud = TRUE;
                    bRestoreVid = TRUE;

                    TMt tMtVideo;
                    TMt tMtAudio;                    

                    m_ptMtTable->GetMtSrc( byMtId, &tMtVideo, MODE_VIDEO );
                    m_ptMtTable->GetMtSrc( byMtId, &tMtAudio, MODE_AUDIO );

                    if (GetLocalSpeaker() == m_ptMtTable->GetMt(byMtId))
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

                    //����ƵԴ
                    if ( !tMtAudio.IsNull() && bRestoreAud)
                    {
                        //ָ���ָ�������Ƶ�㲥ý��Դ
                        RestoreRcvMediaBrdSrc( byMtId, MODE_AUDIO );        
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
								TMt tDstMt = m_ptMtTable->GetMt(byMtId);
								SwitchNewVmpToSingleMt(tDstMt);	//����ϳɹ㲥����������
							}
						}
						else
						{
							//ָ���ָ�������Ƶ�㲥ý��Դ
							RestoreRcvMediaBrdSrc( byMtId, MODE_VIDEO );
						}
					}
                    //���ԭ���������ڻָ���ѡ��״̬����ǿ�ƹ㲥������ָ�
                    TMt tNullMt;
                    tNullMt.SetNull();

                    m_ptMtTable->GetMtStatus( byMtId, &tMtStatus );                
                    tMtStatus.SetSelectMt(tNullMt, MODE_AUDIO);
                    // guzh [11/7/2007] Fixme VideoӦ�ø濴�Լ�
                    tMtStatus.SetSelectMt(tNullMt, MODE_VIDEO);
                
                    m_ptMtTable->SetMtStatus( byMtId, &tMtStatus );              
                }
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
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
	if (mtAliasTypeE164 == ptAddMtInfo->m_AliasType)
	{
		if (0 == strcmp(ptAddMtInfo->m_achAlias, m_tConf.GetConfE164()))
		{
			cServMsg.SetErrorCode(ERR_MCU_NOTCALL_CONFITSELF);
			SendReplyBack(cServMsg, MCU_MCS_ADDMT_NACK);
			ConfLog(FALSE, "[ProcMcsMcuAddMtExReq]CMcuVcInst: Cannot Call conf itself.\n");
			return;
		}
	}

	// ����
	if (LOCAL_MCUID != tMcu.GetMcuId())
	{
		u8 byMtId = tMcu.GetMcuId();
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
    
		ConfLog(FALSE, "[ProcMcsMcuAddMtExReq]Mcs add mt-%d DialBitRate-%d Alias-", 
				byMtId, ptAddMtInfo[byLoop].GetCallBitRate());
		if ( ptAddMtInfo[byLoop].m_AliasType == mtAliasTypeTransportAddress )
		{
			OspPrintf(TRUE, FALSE, "%s:%d!\n",				
					  StrOfIP(ptAddMtInfo[byLoop].m_tTransportAddr.GetIpAddr()), 
					  ptAddMtInfo[byLoop].m_tTransportAddr.GetPort() );
		}
		else
		{
			OspPrintf(TRUE, FALSE, "%s!\n",	ptAddMtInfo[byLoop].m_achAlias);
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
			OspPrintf(TRUE, FALSE, "[ProcMcsMcuAddMtExReq]MainMaxBitRate-%d MainMediaType-%d MainResolution-%d MainFrameRate-%d\n",
				                    tCapInfo.GetMainMaxBitRate(), tCapInfo.GetMainMediaType(),
							        tCapInfo.GetMainResolution(), 
									tCapInfo.IsMainFrameRateUserDefined()?tCapInfo.GetUserDefMainFrameRate():tCapInfo.GetMainFrameRate());
			
			OspPrintf(TRUE, FALSE, "[ProcMcsMcuAddMtExReq]DStreamMaxBitRate-%d DStreamMediaType-%d DStreamResolution-%d DStreamFrameRate-%d\n",
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
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//��������ն˱�
	SendMtListToMcs(LOCAL_MCUID);

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
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	u8   byMtNum = 0;
	TMcu tMcu;
    tMcu.SetNull();
	TAddMtInfo  *ptAddMtInfo = NULL;

	if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
	{
        if (cServMsg.GetMsgBodyLen() >= sizeof(TMcuMcuReq)+sizeof(TMt))
        {
		    byMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TMcuMcuReq)-sizeof(TMt) )/sizeof( TAddMtInfo);
		    ptAddMtInfo = (TAddMtInfo *)(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq)+sizeof(TMcu));
		    tMcu = *(TMcu*)(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq)); 
        }
	}
	else
	{
        if (cServMsg.GetMsgBodyLen() >= sizeof(TMcu))
        {
		    tMcu = *(TMcu*)(cServMsg.GetMsgBody());
		    byMtNum= ( cServMsg.GetMsgBodyLen() - sizeof(TMt) )/sizeof( TAddMtInfo );
		    ptAddMtInfo = (TAddMtInfo *)(cServMsg.GetMsgBody()+sizeof(TMcu));

			for (u8 byIdx = 0; byIdx < byMtNum; byIdx ++)
			{
				//FIXME: ���ǻ�����ʱ�޸ģ��Ժ������Ƿ�ɢ������
				if (ptAddMtInfo[byIdx].m_AliasType == mtAliasTypeTransportAddress &&
					ptAddMtInfo[byIdx].m_tTransportAddr.GetIpAddr() == 0xac103c80)
				{
					if (0 != m_tConf.GetSecBitRate())
					{
						ptAddMtInfo->m_wDialBitRate = m_tConf.GetSecBitRate();
					}
				}
			}
        }
	}

	u8  byLoop, byMtId;
	TMt tMt;

	STATECHECK


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
	if (mtAliasTypeE164 == ptAddMtInfo->m_AliasType)
	{
		if (0 == strcmp(ptAddMtInfo->m_achAlias, m_tConf.GetConfE164()))
		{
			cServMsg.SetErrorCode(ERR_MCU_NOTCALL_CONFITSELF);
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			ConfLog(FALSE, "CMcuVcInst: Cannot Call conf itself.\n");
			return;
		}
	}

	//�Ƿ��ڱ���������
	if (LOCAL_MCUID != tMcu.GetMcuId())
	{
		byMtId = tMcu.GetMcuId();
		if (!m_tConfAllMtInfo.m_tLocalMtInfo.MtJoinedConf(byMtId))
		{
			cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
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

		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
	}

	//��������, ack
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	//���������ն��б�
	for (byLoop = 0; byLoop < byMtNum; byLoop++)
	{
		byMtId = AddMt( ptAddMtInfo[byLoop], ptAddMtInfo[byLoop].GetCallBitRate(), 
                        (ptAddMtInfo[byLoop].m_byCallMode == CONF_CALLMODE_TIMER || ptAddMtInfo[byLoop].m_byCallMode == CONF_CALLMODE_NONE) ? ptAddMtInfo[byLoop].m_byCallMode : m_tConf.m_tStatus.GetCallMode() );
        
		ConfLog(FALSE, "Mcs add mt-%d DialBitRate-%d type-%d alias-", 
				byMtId, ptAddMtInfo[byLoop].GetCallBitRate(), 
				ptAddMtInfo[byLoop].m_AliasType );
        if ( ptAddMtInfo[byLoop].m_AliasType == mtAliasTypeTransportAddress )
        {
            OspPrintf(TRUE, FALSE, "%s:%d!\n",				
                      StrOfIP(ptAddMtInfo[byLoop].m_tTransportAddr.GetIpAddr()), 
                      ptAddMtInfo[byLoop].m_tTransportAddr.GetPort() );
        }
        else
        {
            OspPrintf(TRUE, FALSE, "%s!\n",	ptAddMtInfo[byLoop].m_achAlias);
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
			InviteUnjoinedMt( cServMsg, &tMt, TRUE, TRUE );

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
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//��������ն˱�
	SendMtListToMcs(LOCAL_MCUID);

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
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt tMt;
	if(cServMsg.GetEventId() == MCU_MCU_DELMT_REQ)
	{
		tMt = *( TMt * )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));
	}
	else
	{
		tMt = *( TMt * )cServMsg.GetMsgBody();
	}

	STATECHECK

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

	TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMcuId());
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
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//���Ǳ���MCU�µ��ն�
    if( !tMt.IsLocal() )
	{
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
			sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
		astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
			sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));
		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		
		SendMsgToMt( tMt.GetMcuId(), MCU_MCU_DELMT_REQ, cServMsg );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
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
	RemoveMt( tMt, TRUE );
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
====================================================================*/
void CMcuVcInst::ProcMcsMcuSendMsgReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u16 wMtNum = *(u16*)cServMsg.GetMsgBody();
	TMt	*ptMt = (TMt *)( cServMsg.GetMsgBody() + sizeof(u16) );
	CRollMsg* ptROLLMSG = (CRollMsg*)( cServMsg.GetMsgBody() + ntohs(wMtNum)*sizeof(TMt) + sizeof(u16) );

    CServMsg	cSendMsg;
	TMt tMt;
    
    // xsl [11/3/2006] �������ļ������Ƿ��ն˵Ķ���Ϣת���������е��ն�
    if (MT_MCU_SENDMSG_CMD == pcMsg->event)
    {
        if (!g_cMcuVcApp.IsTransmitMtShortMsg())
        {
            ConfLog(FALSE, "[ProcMcsMcuSendMsgReq] not permit transmit mt short message in mcu config\n");
            return;
        }
        // guzh [4/7/2007] ����MCU����Ϣ����� MCU_MCU_SENDMSG_NOTIF
        /*
		//��ֹ�¼����ϼ����ݶ���Ϣ, zgc, 2007-04-06
		if ( m_ptMtTable->GetMtType( cServMsg.GetSrcMtId() ) == MT_TYPE_SMCU )
		{
			if( !HasJoinedChairman()
				|| m_tConf.GetChairman().GetMtId() != cServMsg.GetSrcMtId()
			  )
			{
				ConfLog(FALSE, "[ProcMcsMcuSendMsgReq] Can not transmit short message from smcu to mmcu!\n");
				return;
			}
		}		
        */
    }

	switch( CurState() )
	{
	case STATE_ONGOING:

		if( wMtNum == 0 )	//���������ն� MCU_MT_SENDMSG_NOTIF�����ܰ������¼�MCU���ڶԶ��յ�ʱ����
		{
			tMt.SetNull();
			cSendMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
			cSendMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );
			BroadcastToAllSubMtJoinedConf( MCU_MT_SENDMSG_NOTIF, cSendMsg );
		}
		else //����ĳЩ�ն� 
		{
			for( u16 wLoop = 0; wLoop < ntohs(wMtNum); wLoop++ )
			{
				if( ptMt->IsLocal() )
				{				
					tMt = m_ptMtTable->GetMt( ptMt->GetMtId() );
					cSendMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
					cSendMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );
					SendMsgToMt( tMt.GetMtId(), MCU_MT_SENDMSG_NOTIF, cSendMsg );
				}
				else
				{
					TMt tMcuMt = m_ptMtTable->GetMt( ptMt->GetMcuId() );
					TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMt->GetMcuId());
					if(ptMcInfo == NULL)
					{
						continue;
					}
					cSendMsg.SetMsgBody( (u8*)ptMt, sizeof(TMt) );
					cSendMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );
					SendMsgToMt( tMcuMt.GetMtId(), MCU_MCU_SENDMSG_NOTIF, cSendMsg );
				}
				ptMt++;
			}
		}
		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetMtListReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	//�õ�MCU��
	TMcu tMcu = *(TMcu*)cServMsg.GetMsgBody(); 

	//�����ն��б�
	if( tMcu.GetMcuId() == 0 )
	{	
		//�ȷ�����MC
		for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
		{
			u8 byMcuId = m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId();
            // guzh [4/30/2007] �ϼ�MCU�б����
			if( byMcuId != 0 && 
                ( byMcuId != m_tCascadeMMCU.GetMtId() || g_cMcuVcApp.IsShowMMcuMtList() )
               )      
			{					
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( byMcuId );
				tMcu.SetMcu( m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId() );
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
			}
		}
	    //��������� 
		tMcu.SetMcu( LOCAL_MCUID );
	}		

	//���Ǳ�����MCU
	if(!tMcu.IsLocal() )
	{
        // guzh [4/30/2007] �ϼ�MCU�б����
        if ( !g_cMcuVcApp.IsShowMMcuMtList() && 
             !m_tCascadeMMCU.IsNull() && m_tCascadeMMCU.GetMtId() == tMcu.GetMcuId() )
        {
        }
        else
        {
            TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( tMcu.GetMcuId() );
            cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
            for(s32 nLoop=0; nLoop<MAXNUM_CONF_MT; nLoop++)
            {
                if( ptConfMcInfo->m_atMtStatus[nLoop].IsNull() || 
                    ptConfMcInfo->m_atMtStatus[nLoop].GetMtId() == 0 ) //�Լ�
                {
                    continue;
                }
                //TMcMtStatus:public TMtStatus
                cServMsg.CatMsgBody((u8 *)&(ptConfMcInfo->m_atMtExt[nLoop]), sizeof(TMtExt));
            }
            
            SendReplyBack( cServMsg, MCU_MCS_MTLIST_NOTIF );
        }        
	}
	else
	{
		//�õ����������б�
		cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
		cServMsg.CatMsgBody( ( u8 * )m_ptMtTable->m_atMtExt, 
			                 m_ptMtTable->m_byMaxNumInUse * sizeof( TMtExt ) );
		SendReplyBack( cServMsg, MCU_MCS_MTLIST_NOTIF );
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
====================================================================*/
void CMcuVcInst::ProcMcsMcuRefreshMcuCmd( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	//�õ�MCU��
	TMcu tMcu = *(TMcu*)cServMsg.GetMsgBody(); 

	for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
	{
		u8 byMcuId = m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId();
		if( byMcuId == tMcu.GetMcuId() )
		{
			//�����ն��б�
			TMcuMcuReq tReq;
			memset(&tReq, 0, sizeof(tReq));
			cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
			SendMsgToMt( byMcuId, MCU_MCU_MTLIST_REQ,  cServMsg);
			
			if( m_tCascadeMMCU.IsNull() || 
				(!m_tCascadeMMCU.IsNull() && m_tCascadeMMCU.GetMtId() != byMcuId) )
			{
				//Ϊ�¼�mcu����ȡ�¼�ֱ���ն�״̬
				OnGetMtStatusCmdToSMcu( byMcuId );
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
            TMcuHdBasStatus tStatus;
            m_cBasMgr.GetHdBasStatus(tStatus, m_tConf);
            cServMsg.SetMsgBody((u8*)&tStatus, sizeof(tStatus));
            SendReplyBack( cServMsg, cServMsg.GetEventId()+1 );
        }
        break;
    default:
        OspPrintf(TRUE, FALSE, "[ProcMcsMcuGetConfInfoReq] unexpected msg.%d<%s>!\n",
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
====================================================================*/
BOOL32 CMcuVcInst::CheckVmpParam(TVMPParam& tVmpParam, u16& wErrorCode)
{
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

	// ��Ա���ô�����ֱ�����
	TMt tMt;
	for(u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		TVMPMember tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		TMt tTmpMt = GetLocalMtFromOtherMcuMt(tVMPMember);
		tMt.SetNull();
// 			TMt tTmpPollMt = m_tConf.m_tStatus.GetPollInfo()->GetMtPollParam().GetTMt();		

		if( tTmpMt.GetMtId() != 0 )
		{
			if( ( VMP_MEMBERTYPE_CHAIRMAN == tVMPMember.GetMemberType() && !HasJoinedChairman() ) 
				|| ( VMP_MEMBERTYPE_SPEAKER == tVMPMember.GetMemberType() 
					&& // ���� [5/23/2006] ֻ�����ն˽���ͨ��������������豸
					( !HasJoinedSpeaker() || GetLocalSpeaker().GetType() == TYPE_MCUPERI ) )
				||( VMP_MEMBERTYPE_POLL == tVMPMember.GetMemberType()  && 
					(POLL_STATE_NONE == m_tConf.m_tStatus.GetPollState() /*||
 					 // xliang [3/30/2009] ��ѯ�������,����Param. ����֮���ж�ͬһ��MT�����ͨ�����ƻ�����
				     (!tTmpPollMt.IsNull() && !(tTmpPollMt == tTmpMt))*/))
			  )
			{							
				tVMPMember.SetMemberTMt( tMt );
			}
			else if (!m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMtId()))
			{
				// guzh [9/26/2006] �ն˲�����
				tVMPMember.SetMemberTMt( tMt );
			}
			else if ( tVMPMember.GetMcuId() != LOCAL_MCUID )
			{
				//�з���
				OnMMcuSetIn(tVMPMember, 0, SWITCH_MODE_SELECT);
				
				tMt = m_ptMtTable->GetMt( tVMPMember.GetMcuId()); // xliang [4/7/2009] FIXME:�����޸�tmt�Ƿ����

				tVMPMember.SetMemberTMt( tMt );
			}
			else
			{
				tVMPMember.SetMemberTMt( m_ptMtTable->GetMt(tVMPMember.GetMtId()) );
			}
			tVmpParam.SetVmpMember( byLoop, tVMPMember );
		}
	}

	return TRUE;
}

/*==============================================================================
������    :  CheckMpuAudPollConflct
����      :  �������Ƶ��ѯ
�㷨ʵ��  :  �������Ƶ��ѯʱ������ϳ�����֧����ѯ����ͷ����˸���ͬʱ����
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-8-20					Ѧ��
==============================================================================*/
void CMcuVcInst::CheckMpuAudPollConflct(TVMPParam& tVmpParam, BOOL32 bChgParam /*= FALSE*/)
{
	
	TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
	if ( tPollInfo.GetMediaMode() == MODE_BOTH 
		&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER)
		&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
		)
	{
		if( !bChgParam )
		{
// 			wErrorCode = ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
// 			return FALSE;
			throw (u16)ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
		}
		else
		{
			u8 byConflictMemType = 0;
			if (m_tLastVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL))
			{
				//�˴������˷����˸���,������Ϊ��ͻԴ
				byConflictMemType = VMP_MEMBERTYPE_SPEAKER;
			}
			else if(m_tLastVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER))
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
				TVMPMember tVmpMember;
				for (byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop ++)
				{
					tVmpMember = *tVmpParam.GetVmpMember(byLoop);
					if(tVmpMember.GetMemberType() == byConflictMemType)
					{
						// xliang [4/3/2009] ��ش�������TVMPParam������modeֵ�ǿյģ����ﱣ��һ��
						tVmpParam.SetVMPMode(CONF_VMPMODE_CTRL);
						ClearOneVmpMember(byLoop, tVmpParam, TRUE);
						break;
					}
				}

// 				wErrorCode = ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
// 				return FALSE;
				throw (u16)ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
			}
		}
	}
}

/*==============================================================================
������    :  CheckSameMtInMultChnnl
����      :  �����в�����ͬһ��MTռ�˶����Աͨ��
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-8-20					Ѧ��							
==============================================================================*/
void CMcuVcInst::CheckSameMtInMultChnnl(TVMPParam & tVmpParam)
{
	// xliang [3/12/2009] LOCAL_MCUID�����192���������,��MT.192��MCU�Ļ����ͻ����С�
	// һ�������к���192��mt��Ҳ�Ǻ�С���ʵġ� �ú��ݲ��Ķ������ܸĳ�0���ĳ�193�����������кγ��
	u8 abyMcMtInVMPStatis[MAXNUM_CONF_MT+1][MAXNUM_CONF_MT+1] = {0};
	BOOL32	bRepeatedVmpMem = FALSE;
	u8 bySpeakerChnnl	= 0;
	u8 byPollChnnl		= 0;
	for( u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		TVMPMember tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		
		// xliang [3/24/2009] membertype�Ĺ��ˣ�Ҫ����tVMPMember.IsNull()��
		// ���򵱷����˸��浫�������޷����˵�����£�����ʧЧ
		if(tVMPMember.GetMemberType() == VMP_MEMBERTYPE_SPEAKER)
		{
			bySpeakerChnnl ++;
		}
		if(tVMPMember.GetMemberType() == VMP_MEMBERTYPE_POLL)
		{
			byPollChnnl ++;
		}
		
		if(tVMPMember.IsNull()) 
		{
			continue;
		}
		
		if( abyMcMtInVMPStatis[tVMPMember.GetMcuId()][tVMPMember.GetMtId()] == 1)
		{
			bRepeatedVmpMem = TRUE;
			break;
		}
		else
		{
			abyMcMtInVMPStatis[tVMPMember.GetMcuId()][tVMPMember.GetMtId()] ++ ;
		}
	}
	if(bRepeatedVmpMem &&
		!g_cMcuVcApp.IsAllowVmpMemRepeated())
	{
		throw (u16)ERR_MCU_REPEATEDVMPMEMBER;
		return;
	}
	if(bySpeakerChnnl > 1)	//�ж��ͨ������ɷ����˸���
	{
		throw (u16)ERR_MCU_REPEATEDVMPSPEAKER;
		return;
	}
	
	if(byPollChnnl > 1)//�ж��ͨ���������ѯ����
	{
		throw (u16)ERR_MCU_REPEATEDVMPPOLL;
		return;
	}

}

/*==============================================================================
������    :  CheckAdpChnnlAtStart
����      :  ��ʼ�ϳ�ʱ��ǰ����ͨ�����ƹ���
�㷨ʵ��  :  
����˵��  :  u8 byMaxHdChnnlNum		[i]	���ǰ����ͨ����
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-8-20					Ѧ��
==============================================================================*/
void CMcuVcInst::CheckAdpChnnlAtStart(TVMPParam &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg)
{
	u8 byIdxInHdChnnl  = 0;
	u8 byIdxOutHdChnnl = 0;
	BOOL32 bOverHdChnnlLmt = FALSE;
	TVmpHdChnnlMemInfo tVmpHdChnnlMemInfo;
	if(m_tConf.HasSpeaker() && tVmpParam.IsMtInMember(GetLocalSpeaker()) //GetSpeaker()��������
		&& !tVmpParam.IsVMPBrdst()	//VMP�㲥��ѷ�������Ƶ�㲥���
		)
	{
		tVmpHdChnnlMemInfo.tMtInHdChnnl[byIdxInHdChnnl++] = GetLocalSpeaker();
	}
	for( u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		TVMPMember tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		
		if( tVMPMember.IsNull() )
		{
			continue;	//�ճ�Ա����
		}
		
		if( GetLocalSpeaker().GetMtId() == tVMPMember.GetMtId() )
		{
			//take speaker into account before,so continue
			continue;
		}
		
		if( ( !tVmpParam.IsVMPBrdst()			//VMP�㲥��ѷ�������Ƶ�㲥�Լ�ѡ��ȫ���
			&&  IsSelectedbyOtherMt(tVMPMember.GetMtId()) )						//��ѡ��
			|| ( MT_MANU_KDC != m_ptMtTable->GetManuId(tVMPMember.GetMtId())  
			&& MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tVMPMember.GetMtId()) )	//�Ƿ��keda�ն�
			
			)//ע�������з�keda��������Ϊ0��Ҳ�ᱻ��Ϊ��keda������֮ǰҪ���˴���Ϊ0�����
		{
			u8 byChnnlType = LOGCHL_VIDEO;
			TLogicalChannel tLogicChannel;
			BOOL32 bNeedAdjustRes = FALSE;
			TMt		tMt;
			if ( m_ptMtTable->GetMtLogicChnnl( tVMPMember.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) )
			{
				u8 byMtStandardFormat = tLogicChannel.GetVideoFormat();
				u8 byReqRes;
				bNeedAdjustRes = VidResAdjust( tVmpParam.GetVMPStyle(), byLoop, byMtStandardFormat, byReqRes);
				
				if (bNeedAdjustRes) //�Ƚ���������Ҫ������MT�����п��������ǰ����ͨ��
				{
					if( byIdxInHdChnnl < byMaxHdChnnlNum )
					{
						tVmpHdChnnlMemInfo.tMtInHdChnnl[byIdxInHdChnnl++] = m_ptMtTable->GetMt(tVMPMember.GetMtId());
					}
					else
					{
						tVmpHdChnnlMemInfo.tMtOutHdChnnl[byIdxOutHdChnnl++] = m_ptMtTable->GetMt(tVMPMember.GetMtId());
						bOverHdChnnlLmt = TRUE;
						tMt.SetNull();
						tVMPMember.SetMemberTMt( tMt );
						tVmpParam.SetVmpMember( byLoop, tVMPMember );
					}
				}
			}
			
		}
	}
	if(bOverHdChnnlLmt)
	{
		cServMsg.SetMsgBody((u8*)&tVmpHdChnnlMemInfo,sizeof(tVmpHdChnnlMemInfo));
		SendReplyBack(cServMsg, MCU_MCS_VMPOVERHDCHNNLLMT_NTF);
		//SendMsgToAllMcs(MCU_MCS_VMPOVERHDCHNNLLMT_NTF,cServMsg);
	}

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
void CMcuVcInst::CheckAdpChnnlAtChange(TVMPParam &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg)
{
	//�ҵ�ǰ����ӵĺϳɳ�Ա
	for( u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		TVMPMember tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		if(!tVMPMember.IsNull() && !m_tLastVmpParam.IsMtInMember((TMt)tVMPMember))//��������ӵ��ն�
		{
			u8 byChnnlType = LOGCHL_VIDEO;
			TLogicalChannel tLogicChannel;
			BOOL32 bNeedAdjustRes = FALSE;
			if ( m_ptMtTable->GetMtLogicChnnl( tVMPMember.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) )
			{
				u8 byMtStandardFormat = tLogicChannel.GetVideoFormat();
				u8 byReqRes;
				bNeedAdjustRes = VidResAdjust( tVmpParam.GetVMPStyle(), byLoop, byMtStandardFormat, byReqRes);
				if(bNeedAdjustRes)
				{
					//��������ж�
					BOOL32 bSpeaker = (GetLocalSpeaker() == (TMt)tVMPMember);							//�Ƿ��Ƿ�����
					BOOL32 bNoneKeda = ( MT_MANU_KDC != m_ptMtTable->GetManuId(tVMPMember.GetMtId())  
						&& MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tVMPMember.GetMtId()) );			//�Ƿ��keda
					BOOL32 bSelected = IsSelectedbyOtherMt(tVMPMember.GetMtId());						//�Ƿ�ѡ��
					if( (bSpeaker && !tVmpParam.IsVMPBrdst())
						|| bNoneKeda 
						|| bSelected
						)
					{
						if(m_tVmpChnnlInfo.m_byHDChnnlNum == byMaxHdChnnlNum) //ǰ����ͨ��������Ҫ��ռ��ʾ
						{
							u8 byChnnlIdx = 0;
							TChnnlMemberInfo tChnnlMemInfo;
							u8 bySeizeMtId = tVMPMember.GetMtId();
							TMt tSeizeMt = (TMt)tVMPMember;
							cServMsg.SetMsgBody((u8*)&tSeizeMt,sizeof(TMt));	//��Ҫ��ռ��������ͨ�����ն�
							for(; byChnnlIdx < byMaxHdChnnlNum; byChnnlIdx++)	//����ռ��HD��ͨ����MT��Ϣ
							{
								m_tVmpChnnlInfo.GetHdChnnlInfo(byChnnlIdx,&tChnnlMemInfo);
								cServMsg.CatMsgBody((u8 *)&tChnnlMemInfo,sizeof(tChnnlMemInfo));
							}
							SendReplyBack(cServMsg, MCU_MCS_VMPPRISEIZE_REQ);
							//SendMsgToAllMcs(MCU_MCS_VMPPRISEIZE_REQ, cServMsg);
							//��¼
							m_atVMPTmpMember[byLoop] = tVMPMember;
							
							return;
						}
					}
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
void CMcuVcInst::CheckAdpChnnlLmt(u8 byVmpId, TVMPParam& tVmpParam, CServMsg &cServMsg, BOOL32 bChgParam /* = FALSE */)
{
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

	if(VMP == byVmpSubType || tVmpParam.IsVMPAuto())
	{
		return;
	}

	u8 byMpuBoardVer = tPeriEqpStatus.m_tStatus.tVmp.m_byBoardVer;	
	u8 byMaxHdChnnlNum = 0;
	if(byVmpSubType == MPU_SVMP || byVmpSubType == EVPU_SVMP)	//SVMP
	{
		byMaxHdChnnlNum = (byMpuBoardVer == MPU_BOARD_A128) ? MAXNUM_SVMP_HDCHNNL : MAXNUM_SVMPB_HDCHNNL;
	}
	else //DVMP //Ŀǰ��dvmp����
	{
		byMaxHdChnnlNum = MAXNUM_DVMP_HDCHNNL; 
	}
	
	if(!bChgParam)
	{
		CheckAdpChnnlAtStart(tVmpParam, byMaxHdChnnlNum, cServMsg);
	}
	else
	{
		CheckAdpChnnlAtChange(tVmpParam, byMaxHdChnnlNum, cServMsg);
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
BOOL32 CMcuVcInst::CheckMpuMember(TVMPParam& tVmpParam, u16& wErrorCode, BOOL32 bChgParam /*= FALSE */)
{
	//�Զ�����ϳɲ���������У��
	if (tVmpParam.IsVMPAuto())
	{
		return TRUE;
	}

	//VMP��֧����ϯ����
	if(tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_CHAIRMAN))
	{
		ConfLog(FALSE,"[VmpCommonReq]MPU not support chairman-membertype!\n");
		wErrorCode = ERR_VMPUNSUPPORTCHAIRMAN;
		return FALSE;
	}
	
	try
	{
		//�����в�����ͬһ��MTռ�˶����Աͨ��
		CheckSameMtInMultChnnl(tVmpParam);

		//�������Ƶ��ѯʱ������ϳ�����֧����ѯ����ͷ����˸���ͬʱ����
		CheckMpuAudPollConflct(tVmpParam, bChgParam);
	
	}
	catch (u16 wErr)
	{
		wErrorCode = wErr;
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
	
	u8 byLoop = 0;
    TVMPParam tVmpParam;
    memset( &tVmpParam, 0, sizeof(TVMPParam) );
	TPeriEqpStatus tPeriEqpStatus;
    TMt tMt;

    //�õ����еĻ���ϳ��� ���µ���VMP�Ƿ����� �жϲ��� [12/27/2006-zbq]
    u8 byIdleVMPNum = 0;
    u8 abyIdleVMPId[MAXNUM_PERIEQP];
    memset( &abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
    g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );

    //zbq[12/27/2007] �ǿƴ��ն˲���VMP����
	// xliang [12/8/2008] FIXME:���ﲻ�ܣ���֮����VMP�����ͺͷֱ������жϾ����
    //BOOL32 bNonKEDAReject = FALSE;
    //TLogicalChannel tLogicChan;
	u8 byVmpSubType = 0;  //VMP������

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
				ProcChangeVmpParamReq(cServMsg); // xliang [1/21/2009] Change VMP Param ����
				break;
			}
		case MCS_MCU_START_VMPBATCHPOLL_REQ:// xliang [12/18/2008] ��ʼvmp ������ѯ
			{
				//��鵱ǰ����Ļ���ϳ�״̬
				if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
				{
					cServMsg.SetErrorCode( ERR_MCU_VMPRESTART );
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					
					return;
				}
				
				//FIXME: ��δ���ڼ����Ĵ���
				tVmpParam = *( TVMPParam* )cServMsg.GetMsgBody();
				m_tVmpBatchPollInfo = *(TVmpBatchPollInfo*)(cServMsg.GetMsgBody()+sizeof(TVMPParam));
				
				if(m_tVmpBatchPollInfo.GetPollNum() == 0 )
				{
					m_tVmpBatchPollInfo.SetLoopNoLimt(TRUE);
				}

				//��¼��ǰ��Ҫvmp������ѯ,����λ��֮��������
				//����Ҫ��¼��m_tConf�У���Ϊ֮��m_tConf��Ϊʵ�δ���IsMCUSupportVmpStyle()
				m_tConf.m_tStatus.m_tVMPParam.SetVMPBatchPoll( 1 );//��ֵ��֮���յ�VMP->MCU notif��ᱻ������,���Բ������д���
				tVmpParam.SetVMPBatchPoll(1);
				
				if(g_bPrintEqpMsg)
				{
					EqpLog("[VmpCommonReq] start vmp param:\n");
					tVmpParam.Print();
				}
				
				u8 byVmpId;
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
						CallLog("No Mpu for VMP Batch Poll!\n");
						return;
					}
				}
				//����vmp member
				TVMPMember tVMPMember;
				u8 byMtInConfNum    = (u8)m_tConfAllMtInfo.GetAllMtNum();//�����ն���
				u8 byCurPolledMtNum = m_tVmpPollParam.GetVmpPolledMtNum();	//��ǰ��ѯ����MT��Ŀ
				u8 byMtId = m_tVmpPollParam.GetCurrentIdx()+1;	//��ǰ��ѯ�����ն�id
				BOOL32 bPollOver	= FALSE;
				BOOL32 bFirst		= TRUE;
				for(byMtId; byMtId < MAXNUM_CONF_MT; byMtId ++)
				{
					if(!m_tConfAllMtInfo.MtInConf( byMtId ))//������նˣ�����
					{
						continue;
					}
					if(byLoop < tVmpParam.GetMaxMemberNum() )
					{
						tMt = m_ptMtTable->GetMt(byMtId);
						tVMPMember.SetMemberTMt( tMt );
						tVMPMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);
						tVmpParam.SetVmpMember( byLoop, tVMPMember );
						m_tVmpPollParam.SetCurPollBlokMtId(byLoop,byMtId);//��¼������Ϣ
						byLoop++;
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
						byLoop = 0;//member id���´�0��ʼ
						bFirst = FALSE;//�´ξͲ��ǵ�һ����
						break;
					}
				}

				//��������Ϣ���浽m_tVmpPollParam: 
				m_tVmpPollParam.SetVmpPolledMtNum(byCurPolledMtNum);//��ǰ��ѯ����MT��Ŀ
				m_tVmpPollParam.SetCurrentIdx(byMtId);//�´�Ҫ�ֵ���MT Id,���ﲻ��1
				m_tVmpPollParam.SetVmpChnnlIdx(byLoop);	  //member Id��
				m_tVmpPollParam.SetIsBatchFirst(bFirst);	//�´��Ƿ��һ����
				m_tVmpPollParam.SetIsVmpPollOver(bPollOver); //����vmpһ��������ѯ�Ƿ����

				CallLog("VMP Batch Poll: CurPolledMtNum is %u,MtId is %u,MemberId is %u\n",\
					byCurPolledMtNum, byMtId, byLoop);


				m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
				m_tVmpEqp.SetConfIdx( m_byConfIdx );
				
				//���û���ϳɲ���
				//�����Ƚ�tVMPParam�ݴ��TPeriEqpStatus��,���յ�VMP notify���ٴ��m_tConf. 
				//ע��m_tConf��ʱ�Ѿ���m_tVMPParam.m_byVMPBatchPoll����ֵ(�ս�case��)
				g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
				
				tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::RESERVE;//��ռ��,��ʱ��δ�ɹ��ٷ���
				tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpParam;
				tPeriEqpStatus.SetConfIdx( m_byConfIdx );
				g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
				//ACK
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

				//��ʼ����ϳ� 
				AdjustVmpParam(&tVmpParam, TRUE);

				break;
			}
	    case MCS_MCU_STOPVMP_REQ:        //��ؽ�����Ƶ��������	
	    case MT_MCU_STOPVMP_REQ:         //��ϯ������Ƶ��������

			// ���ڵȴ�����ϳɿ�ʼ��Ӧ,�ֶ�������Ӧ��ʱ����,�ͷŻ���ϳ���Դ
			if (!m_tVmpEqp.IsNull() &&
				g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
				tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
				TVmpStatus::WAIT_START == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
			{
				// ֱ�Ӵ�����ʱ����
				CMessage cMsg;
				ProcVmpRspWaitTimer(&cMsg);	
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
				return;
			}

			//��鵱ǰ����Ļ���ϳ�״̬ 
			if( m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_NONE)
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

			//ֹͣʱģ��ʧЧ
			if( m_tConf.GetConfAttrb().IsHasVmpModule() )
			{
				TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
                tConfAttrib.SetHasVmpModule(FALSE);
				m_tConf.SetConfAttrb( tConfAttrib );
			}

			SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, 6*1000);
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tPeriEqpStatus);

			//ֹͣ
            g_cEqpSsnApp.SendMsgToPeriEqpSsn( m_tVmpEqp.GetEqpId(),MCU_VMP_STOPVIDMIX_REQ, 
				                              (u8*)cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			
			break;
			
	    case MCS_MCU_GETVMPPARAM_REQ:    //��ز�ѯ��Ƶ���ϲ�������
	    case MT_MCU_GETVMPPARAM_REQ:     //��ϯ��ѯ��Ƶ���ϲ�������

			tVmpParam = m_tConf.m_tStatus.GetVmpParam();
			cServMsg.SetMsgBody( (u8*)&tVmpParam, sizeof(TVMPParam) );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
            if (g_bPrintEqpMsg)
            {
                OspPrintf(TRUE, FALSE, "%s return param:", OspEventDesc(cServMsg.GetEventId()));
                tVmpParam.Print();
            }
            // guzh [7/25/2007] ��VMP��ȡ���������壿
			/*
            g_cEqpSsnApp.SendMsgToPeriEqpSsn( m_tVmpEqp.GetEqpId(), 
				                              MCU_VMP_GETVIDMIXPARAM_REQ, NULL, 0 );
            */
			break;

		case MCS_MCU_STARTVMPBRDST_REQ:         //�������̨����MCU��ʼ�ѻ���ϳ�ͼ��㲥���ն�
		case MT_MCU_STARTVMPBRDST_REQ:		//��ϯ����MCU��ʼ�ѻ���ϳ�ͼ��㲥���ն�
			{
				if( m_byVmpOperating == 1 )	// xliang [4/17/2009] αԭ�Ӳ���FIXME
				{
					McsLog("VMP is busy!!!\n");
					cServMsg.SetErrorCode(ERR_VMPBUSY);
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					return;
				}
				
				//��鵱ǰ����Ļ���ϳ�״̬
				if (CONF_VMPMODE_NONE == m_tConf.m_tStatus.GetVMPMode())
				{
					cServMsg.SetErrorCode( ERR_MCU_VMPNOTSTART );
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					return;
				}
				
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
				
				m_tConf.m_tStatus.SetVmpBrdst( TRUE );
				
				//��ʼ�㲥����ϳ�����
				ChangeVidBrdSrc(&m_tVmpEqp);

				// xliang [3/27/2009] ���VIP��Ա��VMPǰ�����Ա��ϢҪˢ��
				//�����˺ͱ�ѡ���ľ�����,ѡ��������ChangeVidBrdSrc()�н�һ�������˴����ٴ���
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				u8 byMpuBoardVer = tPeriEqpStatus.m_tStatus.tVmp.m_byBoardVer;
				u8 byHdChnnlNum = 0;

				if(byVmpSubType == MPU_SVMP)
				{
					byHdChnnlNum = (byMpuBoardVer == MPU_BOARD_B256) ? MAXNUM_SVMPB_HDCHNNL: MAXNUM_SVMP_HDCHNNL;
				}
				else if(byVmpSubType == EVPU_SVMP)
				{
					byHdChnnlNum = MAXNUM_SVMPB_HDCHNNL;
				}
				else if( byVmpSubType == MPU_DVMP || byVmpSubType == EVPU_DVMP)
				{	
					byHdChnnlNum = MAXNUM_DVMP_HDCHNNL;
				}
				else
				{
					//maintain byHdChnnlNum be 0
				}

				TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
				u8 byVmpStyle = tVmpParam.GetVMPStyle();
				
				//u8 byHdChnnlNum = m_tVmpChnnlInfo.m_byHDChnnlNum;
				TChnnlMemberInfo tChnnlMemInfo;
				u8 byMtId = 0;
				u8 byLoop;
				for(byLoop = 0; byLoop < byHdChnnlNum; byLoop ++)
				{
					m_tVmpChnnlInfo.GetHdChnnlInfo(byLoop, &tChnnlMemInfo);
					if( tChnnlMemInfo.IsAttrSpeaker() 
						//|| tChnnlMemInfo.IsAttrSelected() 
						)
					{
						byMtId = tChnnlMemInfo.GetMtId();
						u8 byPos = tVmpParam.GetChlOfMtInMember(tChnnlMemInfo.GetMt());
						//m_tVmpChnnlInfo.ClearOneChnnl(byMtId);
						//����Mt�ֱ���
						ChangeMtVideoFormat(m_ptMtTable->GetMt(byMtId), &tVmpParam);
					}
				}
				
				//֪ͨ��ظ���ϯ
				cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam) );
				SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
				if (HasJoinedChairman())
				{
					SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_VMPPARAM_NOTIF, cServMsg );
				}	
				
				//FastUpdate to Vmp
				NotifyFastUpdate(m_tVmpEqp, 0);
				if (MPU_SVMP == byVmpSubType && MPU_BOARD_B256 == byMpuBoardVer)
				{
					NotifyFastUpdate(m_tVmpEqp, 1);
					NotifyFastUpdate(m_tVmpEqp, 2);
					NotifyFastUpdate(m_tVmpEqp, 3);
				}

				break;
			}

		case MCS_MCU_STOPVMPBRDST_REQ:          //�������̨����MCUֹͣ�ѻ���ϳ�ͼ��㲥���ն�
		case MT_MCU_STOPVMPBRDST_REQ:		//��ϯ����MCUֹͣ�ѻ���ϳ�ͼ��㲥���ն�			
			
			if( m_byVmpOperating == 1 )	// xliang [4/17/2009] αԭ�Ӳ���FIXME
			{
				McsLog("VMP is busy!!!\n");
				cServMsg.SetErrorCode(ERR_VMPBUSY);
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
			//��鵱ǰ����Ļ���ϳ�״̬
			if( m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_NONE )
			{
				cServMsg.SetErrorCode( ERR_MCU_VMPNOTSTART );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
			}
			
			//��黭��ϳɵĹ㲥״̬
			if( !m_tConf.m_tStatus.IsBrdstVMP() )
			{
				cServMsg.SetErrorCode( ERR_MCU_VMPNOTBRDST );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
			}
/*
            // xsl [11/1/2006] 8000b mcu h264�����Ļ��漰���ϱ���㲥(����Ӧ����)
#ifdef _MINIMCU_
            if (m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264)            
            {
                u8 byVmpType = m_tConf.m_tStatus.GetVmpStyle();
                if (VMP_STYLE_ONE != byVmpType && VMP_STYLE_VTWO != byVmpType && VMP_STYLE_THREE != byVmpType)
                {
                    cServMsg.SetErrorCode( ERR_MCU_OPERATION_NOPERMIT );
				    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
            }
#endif
*/
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

			//ֹͣ�㲥����ϳ�����
            m_tConf.m_tStatus.SetVmpBrdst( FALSE );

			McsLog("Start Stop VMP Broadcast now!!!\n");

			if( HasJoinedSpeaker() )
            {
                TMt tMt = GetLocalSpeaker();
                ChangeVidBrdSrc( &tMt );
            }
            else
            {
                ChangeVidBrdSrc( NULL );
			}

			// xliang [3/31/2009] FIXME: �����˻�ָ��㲥������ǰ���б�ѡ���ģ���ѡ���������˳����
			// ��Ϊʱ���ϵ���¶δ������ChangeVidBrdSrc()��
			TPeriEqpStatus tPeriEqpStatus; 
			g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
			u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
			
			if(byVmpSubType != VMP)
			{
				TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
				u8 byVmpStyle = tVmpParam.GetVMPStyle();
				if( HasJoinedSpeaker() )
				{
					//��ѡ��Ҫ���� --��֮ǰChangeVidBrdSrc()�л��StopSelectSrc(),�ú����Ὣ��ѡ����MT�˳�ǰ����ͨ��
					/*
					u8 byHdChnnlNum = m_tVmpChnnlInfo.m_byHDChnnlNum;
					TChnnlMemberInfo tChnnlMemInfo;
					
					u8 byLoop;
					for(byLoop = 0; byLoop < byHdChnnlNum; byLoop ++)
					{
						m_tVmpChnnlInfo.GetHdChnnlInfo(byLoop,&tChnnlMemInfo);
						if( tChnnlMemInfo.IsAttrSelected() )
						{
							byMtId = tChnnlMemInfo.GetMtId();
							u8 byPos = tVmpParam.GetChlOfMtInMember(tChnnlMemInfo.GetMt());
							m_tVmpChnnlInfo.ClearOneChnnl(byMtId);
							//ǰ����ͨ���ǿճ����ˣ����Ǵ˴���MT��ѡ����״̬��δ�壬�������ϵ��ֱ����ֻ�ռǰ����ͨ��
							//����Mt�ֱ���
							ChangeMtVideoFormat(byMtId,byVmpSubType,byVmpStyle,byPos);
						}
					}*/
					if( tVmpParam.IsMtInMember(GetLocalSpeaker()/*m_tConf.GetSpeaker()*/) )
					{
						//�����˳��Խ�ǰ����ͨ��
						u8 byMtId = m_tConf.GetSpeaker().GetMtId();
						u8 byPos = tVmpParam.GetChlOfMtInMember(m_tConf.GetSpeaker());
						ChangeMtVideoFormat(m_ptMtTable->GetMt(byMtId), &tVmpParam);
					}
				}
			}
			
			
			
            
            
             
            //֪ͨ��ظ���ϯ
            cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam) );
            SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
            if( HasJoinedChairman() )
            {
                SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_VMPPARAM_NOTIF, cServMsg );
            }			
			break;
		}

		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
        ConfLog(FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
    TVMPParam tVmpTwParam;
    TPeriEqpStatus tPeriEqpStatus;
    TMt tMt;

    //zbq[01/08/2007] �ǿƴ��ն˾ܾ�����VMPTW
    BOOL32 bNonKEDAReject = FALSE;
    TLogicalChannel tLogicChan;

    //�õ����еĻ���ϳ��� ���µ���MPW�Ƿ����� �жϲ��� [12/27/2006-zbq]
    u8 byIdleMPWNum = 0;
    u8 abyIdleMPWId[MAXNUM_PERIEQP];
    memset( &abyIdleMPWId, 0, sizeof(abyIdleMPWId) );
    g_cMcuVcApp.GetIdleVmpTw( abyIdleMPWId, byIdleMPWNum, sizeof(abyIdleMPWId) );

    switch(cServMsg.GetEventId())
    {
    case MCS_MCU_STARTVMPTW_REQ:       //��ؿ�ʼ��Ƶ��������

        tVmpTwParam = *(TVMPParam *)cServMsg.GetMsgBody();
        if(g_bPrintEqpMsg)
        {
            EqpLog("[VmpTwCommonReq] change vmp param:\n");
            tVmpTwParam.Print();
        }

        //���ڻ�ؿ���ģʽ�£������淽ʽ����Ӧ�նˣ����ն���Ϣ���       
        memset(&tMt, 0, sizeof(TMt));
        for (byLoop = 0; byLoop < tVmpTwParam.GetMaxMemberNum(); byLoop++)
        {
            TVMPMember tVMPMember = *tVmpTwParam.GetVmpMember(byLoop);
            TMt tTmpMt = GetLocalMtFromOtherMcuMt(tVMPMember);
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
                          MT_MANU_KDC != m_ptMtTable->GetManuId(tVMPMember.GetMtId()) &&
                          MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tVMPMember.GetMtId()) )
                {
                    tMt.SetNull();
                    tVMPMember.SetMemberTMt( tMt );
                    tVmpTwParam.SetVmpMember( byLoop, tVMPMember );
                    
                    bNonKEDAReject = TRUE;
                }                
                else if ( !m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMtId()) )
                {
                    tMt.SetNull();
                    tVMPMember.SetMemberTMt(tMt);
                    tVmpTwParam.SetVmpMember(byLoop, tVMPMember);
                }
                else
                {
                    if (tVMPMember.GetMcuId() != LOCAL_MCUID)
                    {
                        //�з���
                        OnMMcuSetIn(tVMPMember, 0, SWITCH_MODE_SELECT);
                        tMt = m_ptMtTable->GetMt(tVMPMember.GetMcuId());
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
            if (IsOverSatCastChnnlNum(tVmpTwParam))
            {
                ConfLog(FALSE, "[VmpTwCommonReq-start] over max upload mt num. nack!\n");
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
        tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpTwParam;
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
        tVmpTwParam = *(TVMPParam *)cServMsg.GetMsgBody();
        if(g_bPrintEqpMsg)
        {
            EqpLog("[VmpTwCommonReq] change vmp param:\n");
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
            u8 byVmpTwId = 0;
            if ( IsMCUSupportVmpStyle(tVmpTwParam.GetVMPStyle(), byVmpTwId, EQP_TYPE_VMPTW, wError) )
            {
                NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_OTHERVMP_SUPPORT );
                ConfLog( FALSE, "[VmpTwCommonReq] curr VMPTW.%d can't support the style needed, other VMPTW.%d could\n",
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
            if (0 != tVMPMember.GetMtId() && LOCAL_MCUID != tVMPMember.GetMcuId())
            {
                //�з���
                OnMMcuSetIn(tVMPMember, 0, SWITCH_MODE_SELECT);
                TMt tMt;
                tMt = m_ptMtTable->GetMt(tVMPMember.GetMcuId());
                tVMPMember.SetMemberTMt(tMt);
                tVmpTwParam.SetVmpMember(byLoop, tVMPMember);
            }
        }

        // xsl [7/20/2006]���Ƿ�ɢ������Ҫ���ش�ͨ����
        if (m_tConf.GetConfAttrb().IsSatDCastMode()) 
        {            
            if (IsOverSatCastChnnlNum(tVmpTwParam))
            {
                ConfLog(FALSE, "[VmpTwCommonReq-change] over max upload mt num. nack!\n");
                cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
        }

        //ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId() , &tPeriEqpStatus);
        tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpTwParam;
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
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMcu tMcu =  *(TMcu *)cServMsg.GetMsgBody(); 

	MMcuLog( "[ProcMcsMcuMcuMediaSrcReq] McuMcuId.%d - McuMtId.%d\n", 
		     tMcu.GetMcuId(), tMcu.GetMtId() );

	cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu) );
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->m_atConfOtherMcInfo;
	for(s32 nLoop=0; nLoop<MAXNUM_SUB_MCU; nLoop++)
	{
		u8 byMcuId = ptMcInfo[nLoop].m_byMcuId;
		if(byMcuId == 0)
		{
			continue;
		}

		TMt tMt = GetMcuMediaSrc(byMcuId);

		//�����ѯ�����ϼ�mcu����ý��ԴΪ�գ�������ý��Դ���Ǹ�mcu
		if( (tMt.IsNull() || 0 == tMt.GetMtId()) && 
			!m_tCascadeMMCU.IsNull() && 
			byMcuId == m_tCascadeMMCU.GetMtId() )
		{
			tMt.SetMcuId(LOCAL_MCUID);
			tMt.SetMtId(byMcuId);
		}

		McsLog( "[ProcTimerMcuSrcCheck] McuId.%d - MtMcuId.%d MtId.%d MtType.%d MtConfIdx.%d\n", 
				 byMcuId, tMt.GetMcuId(), tMt.GetMtId(), tMt.GetMtType(), tMt.GetConfIdx() );

		ptMcInfo[nLoop].m_tSrcMt = tMt;
		TMcu tMcu;
		tMcu.SetMcuId(byMcuId);
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

	if( NULL == m_ptConfOtherMcTable )
	{
		cServMsg.SetErrorCode( ERR_LOCKSMCU_NOEXIT );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	if( ( ptMcu->IsLocal() ) || 
		( !m_tCascadeMMCU.IsNull() && ptMcu->GetMcuId() == m_tCascadeMMCU.GetMtId() ) )
	{
		cServMsg.SetErrorCode( ERR_LOCKSMCU_NOEXIT );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	CServMsg cMsg;
	cMsg.SetServMsg(cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_LOCK_REQ);

	u8   byMcuNum = 0;
	TConfMcInfo *ptMcInfo = NULL;

	if( 0 != ptMcu->GetMcuId() )
	{
		ptMcInfo = m_ptConfOtherMcTable->GetMcInfo( ptMcu->GetMcuId() );
		if( NULL == ptMcInfo )
		{
			cServMsg.SetErrorCode( ERR_LOCKSMCU_NOEXIT );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
		SendMsgToMt(ptMcu->GetMcuId(), MCU_MCU_LOCK_REQ, cMsg);
	}
	else
	{
		for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
		{
			ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
			if( ( 0 == ptMcInfo->m_byMcuId ) || 
				( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) )
			{
				continue;
			}
			SendMsgToMt(ptMcInfo->m_byMcuId, MCU_MCU_LOCK_REQ, cMsg);

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
        ConfLog( FALSE, "[ProcMcsMcuVACReq] distributed conf not supported vac\n");
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
    u8 byGrpId = 0;

    //���Է���ϯ�նˣ�nack
    if (cServMsg.GetSrcMtId() != 0 &&
        cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId())
    {
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
			ConfLog(FALSE, "[MixerVACReq]ERROR: already in VAC mode!\n");
            cServMsg.SetErrorCode( ERR_MCU_CONFINVACMODE );
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
        
        //û�п��еĻ�����
        if(m_tConf.m_tStatus.IsNoMixing())
        {
            byEqpId = g_cMcuVcApp.GetIdleMixGroup( byEqpId, byGrpId );
            if( byEqpId == 0 )
            {
				ConfLog(FALSE, "[MixerVACReq] ERROR: no idle MIXER!\n");
                cServMsg.SetErrorCode(ERR_MCU_NOIDLEMIXER_INCONF);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
                return;
            }
        }        
        
        //����ѿ�ʼ��ѯ�����ܿ�ʼ��������
        if (m_tConf.m_tStatus.GetPollMode() != CONF_POLLMODE_NONE) 
        {
            cServMsg.SetErrorCode(ERR_MCU_CONFSTARTPOLLVACMODE);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        // xsl [7/24/2006] ���ƻ������ܿ�����������
        if (m_tConf.m_tStatus.IsSpecMixing())
        {
            ConfLog(FALSE, "[MixerVACReq] MixSpec can't start vac. nack\n");
            cServMsg.SetErrorCode(ERR_MCU_VACMUTEXSPECMIX);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        // guzh [1/30/2007] 8000B ��������        
        if ( ! CMcuPfmLmt::IsVacOprSupported( m_tConf, m_tConfAllMtInfo.GetLocalJoinedMtNum(), wError ) )
        {
			ConfLog(FALSE, "[MixerVACReq]ERROR: 8000b limit!\n");
            cServMsg.SetErrorCode(wError);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        //ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

        if (m_tConf.m_tStatus.IsMixing())
        {
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
            if (HasJoinedChairman())
            {
                SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STARTVAC_NOTIF, cServMsg);
            }
            //zbq [11/26/2007] �����ϱ�
            MixerStatusChange();
        }
        else
        {
            StartMixing( mcuVacMix );
        }
        break;

    case MCS_MCU_STOPVAC_REQ:     //�������̨����MCUֹͣ�����������Ʒ���	
    case MT_MCU_STOPVAC_REQ:      //�ն�����MCUֹͣ�����������Ʒ���
        //��δ���ڴ��������������Ʒ���״̬, NACK
        if (!m_tConf.m_tStatus.IsVACing())
        {
            cServMsg.SetErrorCode(ERR_MCU_CONFNOTINVACMODE);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        //ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
        
        if (m_tConf.m_tStatus.IsNoMixing())
        {
            ConfLog(FALSE,"[MixerVACReq] begin to stopMixing!\n");
			StopMixing(); //ֹͣ������������ACK��������״̬
        }
        else
        {
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
            ConfLog(FALSE, "No poll while Vac!\n");
            break;
        }       
        
        //�õ�������ѯ����
        tPollInfo = *(TPollInfo *)cServMsg.GetMsgBody();
        u8 byPollMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TPollInfo)) / sizeof(TMtPollParam);
        TMtPollParam *ptParam = (TMtPollParam*)(cServMsg.GetMsgBody() + sizeof(TPollInfo));
        m_tConfPollParam.InitPollParam( byPollMtNum, ptParam );

        // xsl [8/23/2006] �ж����ǻ����Ƿ�ֹͣ��ѯ
        if (m_tConf.GetConfAttrb().IsSatDCastMode())
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
                        ConfLog(FALSE, "[ProcMcsMcuPollMsg-start] over max upload mt num. nack!\n");  
                        NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_DCAST_OVERCHNNLNUM);

                        memset(&m_tConfPollParam, 0, sizeof(m_tConfPollParam));
                        return;
                    }
                }
            }            
        }     
        
        // guzh [6/22/2007] ���Ϊ��ʼ��ѯ�����ö�ʱ��
        tPollInfo.SetPollState(POLL_STATE_NORMAL);
        m_tConfPollParam.SpecPollPos(POLLING_POS_START);        
        m_tConf.m_tStatus.SetPollInfo(tPollInfo);

        if (MODE_VIDEO == tPollInfo.GetMediaMode())
        {
            m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_VIDEO);
        }
        else
        {
            m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_SPEAKER);
        }
        ConfModeChange();

        SetTimer(MCUVC_POLLING_CHANGE_TIMER, 10);

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

        cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
        cServMsg.CatMsgBody((u8 *)m_tConfPollParam.GetPollMtByIdx(0),
                            m_tConfPollParam.GetPolledMtNum()*sizeof(TMtPollParam ));
        SendReplyBack(cServMsg, pcMsg->event + 1);			
        break;

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
            u8 byIdx;
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
				ConfLog( FALSE, "[ProcMcsMcuPollMsg] <MCS_MCU_SPECPOLLPOS_REQ> Conf not in polling!\n");
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
                TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
                TMtPollParam tCurMtParam = *(m_tConfPollParam.GetPollMtByIdx(byMtIdx));
                tPollInfo.SetMtPollParam( tCurMtParam );
                m_tConf.m_tStatus.SetPollInfo(tPollInfo);
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
void CMcuVcInst::ProcStopConfPoll( void )
{
    u8 byPrvPollMode = m_tConf.m_tStatus.GetPollMode();
    u8 byPollMeida = m_tConf.m_tStatus.GetPollMedia();
    m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_NONE);
    m_tConf.m_tStatus.SetPollState(POLL_STATE_NONE);

    CServMsg cServMsg;
    cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
    SendMsgToAllMcs(MCU_MCS_POLLSTATE_NOTIF, cServMsg);
    
    KillTimer(MCUVC_POLLING_CHANGE_TIMER);
    
    //ȡ����ѯ����ȡ����ѯ��������Ӧ����״̬
    //CancelOneVmpFollowSwitch( VMP_MEMBERTYPE_POLL, TRUE );
    if (CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode())
    {
        ChangeVmpChannelParam(NULL, VMP_MEMBERTYPE_POLL);
    }
    
    if (CONF_VMPTWMODE_NONE != m_tConf.m_tStatus.GetVmpTwMode())
    {
        ChangeVmpTwChannelParam(NULL, VMPTW_MEMBERTYPE_POLL);
    }
    
    if ( !m_tConf.m_tStatus.IsBrdstVMP() )		
    {
        if(HasJoinedSpeaker())
        {
            TMt tMt = GetLocalSpeaker();
            ChangeVidBrdSrc(&tMt);
        }
        else
        {
            ChangeVidBrdSrc(NULL);
            
            //zbq[08/29/2009] ����Ƶ��ѯ�£�ǿ�Ʋ�������ͬ���Ĺ�bas���� 
            if (CONF_POLLMODE_VIDEO == byPrvPollMode)
            {
                TBasChn atBasChn[MAXNUM_CONF_MVCHN];
                u8 byNum = 0;
                m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_MVBRD);
                
                u8 byIdx = 0;
                for(; byIdx < byNum; byIdx ++)
                {
                    StopSwitchToPeriEqp(atBasChn[byIdx].GetEqpId(), atBasChn[byIdx].GetChnId(), TRUE, MODE_AUDIO);
                }
            }
        }
    }
        
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
        if (EQP_TYPE_HDU == g_cMcuVcApp.GetEqpType(byEqpId))
        {
            for (byChnlIdx = 0; byChnlIdx < MAXNUM_HDU_CHANNEL; byChnlIdx++)
            {
                if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tHduStatus) &&
                    TW_MEMBERTYPE_POLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType &&
                    m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx())
                {
                    ChangeHduSwitch(NULL, byEqpId, byChnlIdx, TW_MEMBERTYPE_POLL, TW_STATE_STOP, FALSE);
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
        if (CONF_POLLMODE_NONE == m_tConf.m_tStatus.GetPollMode()) 
        {
            break;
        }

		if ( 0 == m_tConfPollParam.GetPolledMtNum() )
		{
            ProcStopConfPoll();
			break;
		}		
        
		TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
		
		TMt tLastPolledMt = tPollInfo.GetMtPollParam().GetTMt(); // xliang [3/31/2009] ��¼�ϴ��ֵ���MT
        //��¼��ѯ�Ƿ��д�������, zgc, 2007-06-20
		BOOL32 bIsPollNumNoLimit = ( 0 == tPollInfo.GetPollNum() ) ? TRUE : FALSE; 
		u8 byPollIdx = m_tConfPollParam.GetCurrentIdx();
		ptCurPollMt = GetNextMtPolled(byPollIdx, tPollInfo);		
		m_tConf.m_tStatus.SetPollInfo(tPollInfo);
		m_tConfPollParam.SetCurrentIdx(byPollIdx);
	
		// xsl [8/23/2006] �ж����ǻ����Ƿ�ֹͣ��ѯ
		if (m_tConf.GetConfAttrb().IsSatDCastMode())
		{
			BOOL32 bPollWitchAudio = (CONF_POLLMODE_SPEAKER == m_tConf.m_tStatus.GetPollMode());
			TMt tLastPollMt = (TMt)m_tConf.m_tStatus.GetMtPollParam();
			if (IsMtSendingVidToOthers(tLastPollMt, bPollWitchAudio, FALSE, 0) && //��һ���ն˻���������ʵ�巢������
				m_ptMtTable->GetMtSndBitrate(ptCurPollMt->GetMtId()) == 0 && //��һ���ն�û���ڷ�������
				IsOverSatCastChnnlNum(0))
			{
                ProcStopConfPoll();
			    break;
			}
		}   
        
		if (NULL != ptCurPollMt)
		{               
			//֪ͨ���л��
			TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);           
           
			m_tConf.m_tStatus.SetMtPollParam( *ptCurPollMt );
			cServMsg.SetMsgBody( (u8*)&tPollInfo, sizeof(TPollInfo) );
			SendMsgToAllMcs( MCU_MCS_POLLSTATE_NOTIF, cServMsg );
           
			//������渴�ϵ���Ӧ��ѯ����Ľ���
			// xliang [3/31/2009] �����߼�
			if (CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode())
			{
				TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
				
				// xliang [4/21/2009] ��ϯ��ѡ��VMPʧЧ
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
				}

				// xliang [4/2/2009] ����Ƶ�Ļ�����ѯ,VMP��ѯ���棬VMP�����˸���3�߲��ܹ���
				// �����˸���ͨ��ȡ�����ϱ�MCS������ʾ
				if( tPollInfo.GetMediaMode() == MODE_BOTH
					&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) //����Ƶ�Ļ�����ѯ=�����䷢����
					&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)  )	
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
							ClearOneVmpMember(byLoop, tVmpParam);
							break;
						}
					}
				}
				if( tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL) 
					//|| (tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) //����Ƶ�Ļ�����ѯ�������˸���ͨ��
					//&& tPollInfo.GetMediaMode() == MODE_BOTH ) 
					)
				{
					// xliang [3/19/2009] ����MPU������ѯ����MT�Ѿ�������ͨ�����Ҹ�ͨ���Ƿ����˸�����߸�Mt�Ƿ�����
					// �򲻽�VMP�е���ѯ����ͨ��  ----�ϳ� xliang [4/2/2009] 
					// xliang [4/2/2009] ������VMP������ѯ����MT�Ѿ�������ͨ���о�������ѯ����ͨ��
					u8 byMtVmpChnl = tVmpParam.GetChlOfMtInMember(tLocalMt);
					TPeriEqpStatus tPeriEqpStatus; 
					g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
					u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
					if( ( byVmpSubType == MPU_SVMP || byVmpSubType == MPU_DVMP 
						|| byVmpSubType == EVPU_SVMP || byVmpSubType == EVPU_DVMP)
						&& tVmpParam.IsMtInMember(tLocalMt)
						//&& ( GetLocalSpeaker() == tLocalMt 
						//|| tVmpParam.GetVmpMember(byMtVmpChnl)->GetMemberType() == VMP_MEMBERTYPE_SPEAKER )
						)
					{
						McsLog("[ProcPollingChangeTimerMsg] Mt.%u has already in certain VMP channel!\n",tLocalMt.GetMtId());
					}
					else
					{
						u8 byVmpMemType = VMP_MEMBERTYPE_POLL;
						/*if( tPollInfo.GetMediaMode() == MODE_BOTH )	// xliang [3/31/2009] ����Ƶ�Ļ�����ѯ--->�����˸���ͨ��
						{
							byVmpMemType = VMP_MEMBERTYPE_SPEAKER;
						}*/
						ChangeVmpChannelParam(&tLocalMt, byVmpMemType,&tLastPolledMt);
					}
				}
			}

			if (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE)
			{
				ChangeVmpTwChannelParam(&tLocalMt, VMPTW_MEMBERTYPE_POLL);
			}
           
			if (MODE_VIDEO == m_tConf.m_tStatus.GetPollMedia())
			{
				if (!m_tConf.m_tStatus.IsBrdstVMP())
				{
					ChangeVidBrdSrc((TMt *)&tLocalMt);
				}
			}
			else
			{
				ChangeSpeaker(ptCurPollMt, TRUE);
			}
            // zbq [09/06/2007] ������ѯ������ƵҲSetIn,�Ƿ���ѯ����GetParam�����
            // guzh [6/14/2007] �¼��ն�ҪSetIn
            if ( !ptCurPollMt->IsLocal() )
            {
                OnMMcuSetIn(*ptCurPollMt, 0, SWITCH_MODE_BROADCAST, TRUE);
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
						if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus) &&
							TW_MEMBERTYPE_POLL == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType &&
							m_byConfIdx == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx())
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
				if (EQP_TYPE_HDU == g_cMcuVcApp.GetEqpType(byHduEqpId))
				{
					for (byHduChnlIdx = 0; byHduChnlIdx < MAXNUM_HDU_CHANNEL; byHduChnlIdx++)
					{
						if (g_cMcuVcApp.GetPeriEqpStatus(byHduEqpId, &tHduStatus) &&
							TW_MEMBERTYPE_POLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].byMemberType &&
							m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetConfIdx())
						{
							ChangeHduSwitch(ptCurPollMt, byHduEqpId, byHduChnlIdx, TW_MEMBERTYPE_POLL, TW_STATE_CHANGE, m_tConf.m_tStatus.GetPollMedia() );
						}
						else
						{
							McsLog("[ProcPollingChangeTimerMsg] HduEqp%d is not exist or not polling \n", byHduEqpId);
						}
					}
				}
			}

			SetTimer(MCUVC_POLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());
			// zbq [03/09/2007] ֪ͨ��һ����������ѯ�����ն�
			NoifyMtToBePolledNext();
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
    ������      ��NoifyMtToBePolledNext
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
void CMcuVcInst::NoifyMtToBePolledNext( void )
{
    TMtPollParam *ptNextPollMt;
    u8 byPollIdx = m_tConfPollParam.GetCurrentIdx();
    TPollInfo tPollInfo;
	ptNextPollMt = GetNextMtPolled(byPollIdx, tPollInfo);

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
=============================================================================*/
void CMcuVcInst::ProcMcsMcuHduBatchPollMsg(const CMessage *pcMsg)
{
	if ( NULL == pcMsg )
	{
	 	OspPrintf( TRUE, FALSE, "[ProcMcsMcuHduBatchPollMsg] pcMsg is null!\n" );
	    return;
	}
    
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
    u8  byIndex;

	switch ( pcMsg->event )
	{
	// ��ʼ������ѯ
	case MCS_MCU_STARTHDUBATCHPOLL_REQ:
	{
		TPeriEqpStatus tHduStatus;
        THduPollSchemeInfo tHduPollSchemeInfo;
        THduChnlCfgInfo tHduChnlCfgInfo[MAXNUM_HDUCFG_CHNLNUM];
		THduChnlPollInfo *ptHduChnlPollInfo = (THduChnlPollInfo*)m_tHduBatchPollInfo.m_tChnlBatchPollInfo;

		if( POLL_STATE_NONE != m_tHduBatchPollInfo.GetStatus() )
		{
			ConfLog(FALSE, "[ProcMcsMcuHduBatchPollMsg] hdu is already in batch polling\n");
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
		THduChnlInfoInUse tHduChnlInfoInUseTable[MAXNUM_HDUCFG_CHNLNUM];
		for ( byIndex=0; byIndex < MAXNUM_HDUCFG_CHNLNUM; byIndex++ )
		{
			if ( tHduChnlCfgInfo[byIndex].GetEqpId() >= HDUID_MIN
			    && tHduChnlCfgInfo[byIndex].GetEqpId() <= HDUID_MAX
				)
			{
				g_cMcuVcApp.GetPeriEqpStatus( tHduChnlCfgInfo[byIndex].GetEqpId(), &tHduStatus );
                // ��ͨ���ѱ�ռ�ã����ͷź��ٿ���������ѯ
				if ( tHduStatus.m_byOnline == 1
					&& 0 != tHduStatus.m_tStatus.tHdu.atVideoMt[tHduChnlCfgInfo[byIndex].GetChnlIdx()].GetMcuId() )
                {
					bIsChnlInUse = TRUE;
                    tHduChnlInfoInUseTable[byChnlInUseNum].SetChlIdx( tHduChnlCfgInfo[byIndex].GetChnlIdx() );
					tHduChnlInfoInUseTable[byChnlInUseNum].SetEqpId( tHduChnlCfgInfo[byIndex].GetEqpId() );
					tHduChnlInfoInUseTable[byChnlInUseNum].SetConfIdx( 
						tHduStatus.m_tStatus.tHdu.atVideoMt[tHduChnlCfgInfo[byIndex].GetChnlIdx()].GetConfIdx() 
						);

				}
				// ����Ԥ���п��Բ�����ѯ��ͨ��
                if ( !bIsChnlInUse )  
                {
					ptHduChnlPollInfo[byHduChnlInPollNum].SetPosition( byIndex );
					ptHduChnlPollInfo[byHduChnlInPollNum].SetChnlIdx( tHduChnlCfgInfo[byIndex].GetChnlIdx() );
					ptHduChnlPollInfo[byHduChnlInPollNum].SetEqpId( tHduChnlCfgInfo[byIndex].GetEqpId());
				    tHduStatus.m_tStatus.tHdu.atHduChnStatus[tHduChnlCfgInfo[byIndex].GetChnlIdx()].SetSchemeIdx(bySchemeIdx );
					g_cMcuVcApp.SetPeriEqpStatus( tHduChnlCfgInfo[byIndex].GetEqpId(), &tHduStatus );
					byHduChnlInPollNum ++;
                }
			}
			else
			{
				// do nothing!
			}
		}
        
		// Ԥ���������ڱ�ʹ�õ�ͨ�������ر�ռ�õ�ͨ����Ϣ�б�
        if ( bIsChnlInUse )
        {
			ConfLog( FALSE, "[ProcMcsMcuHduBatchPollMsg] current hdu scheme's chnls is in use! please release them! \n" );
			cServMsg.SetErrorCode( ERR_MCU_HDUBATCHPOLL_CHNLINUSE );
			cServMsg.SetMsgBody( (u8*)tHduChnlInfoInUseTable, sizeof(tHduChnlInfoInUseTable) );
			SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 2, cServMsg );
			return;
        }
        u32 dwCycles = tHduPollSchemeInfo.GetCycles();
		m_tHduBatchPollInfo.SetChnlPollNum( byHduChnlInPollNum );
		m_tConf.m_tStatus.GetHduPollInfo()->SetPollNum( dwCycles );
        m_tConf.m_tStatus.GetHduPollInfo()->SetKeepTime( tHduPollSchemeInfo.GetKeepTime() );

		// ��ʼ��һ�����ѯ 
		if ( !HduBatchPollOfOneCycle( TRUE ) )
		{
            ConfLog( FALSE, "[ProcMcsMcuHduBatchPollMsg] have no Mt in current conf!\n" );
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
	    SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
		
		SetTimer(MCUVC_HDUBATCHPOLLI_CHANGE_TIMER, 1000 * tHduPollSchemeInfo.GetKeepTime() );

	}
		break;
    
	// ֹͣhdu������ѯ
	case MCS_MCU_STOPHDUBATCHPOLL_REQ:
	{
        StopHduBatchPollSwitch();

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
	    SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

	}
	    break;
	
	default:
		OspPrintf( TRUE, FALSE, "[ProcMcsMcuHduBatchPollMsg] message type(%u) is wrong!\n" , pcMsg->event );
    
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

    switch(pcMsg->event)
    {
    case MCS_MCU_STARTHDUPOLL_CMD:
        {
            TPeriEqpStatus tHduInfo;
            THduPollInfo tPollInfo = *(THduPollInfo *)(cServMsg.GetMsgBody());
            
            if(tPollInfo.GetTWChnnl() >= MAXNUM_HDU_CHANNEL)
            {
                ConfLog(FALSE, "[ProcMcsMcuHduPollMsg] hdu chnnl id is invalid :%d\n", tPollInfo.GetTWChnnl());
                break;
            }
            
            //ͨ�����Ƿ��Ѿ����ն�            
            g_cMcuVcApp.GetPeriEqpStatus(tPollInfo.GetTvWall().GetEqpId(), &tHduInfo);
			tHduInfo.m_tStatus.tHdu.atHduChnStatus[tPollInfo.GetTWChnnl()].SetSchemeIdx( tPollInfo.GetSchemeIdx() );
            g_cMcuVcApp.SetPeriEqpStatus( tPollInfo.GetTvWall().GetEqpId(), &tHduInfo );
			THduMember tHduMem = tHduInfo.m_tStatus.tHdu.atVideoMt[tPollInfo.GetTWChnnl()];
            if(0 != tHduMem.GetConfIdx() && tHduMem.GetConfIdx() != m_byConfIdx)          //���������û��Ȩ�޲���, ��������滻
            {
                ConfLog(FALSE, "[ProcMcsMcuHduPollMsg] Confidx %d is already in polling, can't replace it\n", tHduMem.GetConfIdx());
                break;
            }
            
            if(POLL_STATE_NONE != m_tConf.m_tStatus.GethduPollState())
            {
                ConfLog(FALSE, "[ProcMcsMcuHduPollMsg] hdu chnnl id :%d is already in polling\n", tPollInfo.GetTWChnnl());
                break;
            }

            //�õ���ѯmt����
			u8 byPollMtNum = (cServMsg.GetMsgBodyLen() - sizeof(THduPollInfo)) / sizeof(TMtPollParam);
            TMtPollParam *atMtPollParam = (TMtPollParam *)( cServMsg.GetMsgBody() + sizeof(THduPollInfo) );
		    m_tHduPollParam.InitPollParam( byPollMtNum, atMtPollParam );
            m_tHduPollParam.SetTvWallEqp(tPollInfo.GetTvWall());
            m_tHduPollParam.SetTvWallChnnl(tPollInfo.GetTWChnnl());
			m_tHduPollParam.SetMode( tPollInfo.GetMediaMode() );
         
            // xsl [8/23/2006] �ж����ǻ����Ƿ�ֹͣ��ѯ
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {            
                //���ش�������������ѯ�ն����в����������ն���ܾ���ѯ
                if (IsOverSatCastChnnlNum(0))
                {
                    for(u8 byIdx = 0; byIdx < m_tHduPollParam.GetPolledMtNum(); byIdx++)
                    {
                        if (m_ptMtTable->GetMtSndBitrate(m_tHduPollParam.GetPollMtByIdx(byIdx)->GetMtId()) == 0)
                        {
                            ConfLog(FALSE, "[ProcMcsMcuHduPollMsg-start] over max upload mt num. nack!\n");  
                            NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_DCAST_OVERCHNNLNUM);
                            memset(&m_tHduPollParam, 0, sizeof(m_tHduPollParam));
                            return;
                        }
                    }
                }            
            }     

            m_tConf.m_tStatus.SetHduPollInfo(tPollInfo);
            m_tConf.m_tStatus.SetHduPollState(POLL_STATE_NORMAL);                        
            
			u8 byCurPollPos = m_tHduPollParam.GetCurrentIdx();
            TMtPollParam *ptCurPollMt = GetMtHduPollParam( byCurPollPos );
            m_tHduPollParam.SetCurrentIdx( byCurPollPos );
			m_tHduPollParam.SetIsStartAsPause( 0 );
			
            if (NULL != ptCurPollMt)
            {
                TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);   
                        
                //֪ͨ���л��
                m_tConf.m_tStatus.SetHduMtPollParam(*ptCurPollMt);
                cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetHduPollInfo(), sizeof(THduPollInfo));
                SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);

                ChangeHduSwitch(&tLocalMt, tPollInfo.GetTvWall().GetEqpId(), tPollInfo.GetTWChnnl(), 
                                    TW_MEMBERTYPE_TWPOLL, TW_STATE_START, tPollInfo.GetMediaMode() );

                ConfStatusChange();
                
                //������ѯʱ��
                SetTimer(MCUVC_HDUPOLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());
            }
            else
            {
                //������ѯʱ��
                SetTimer(MCUVC_HDUPOLLING_CHANGE_TIMER, 1000 * 5);
            }            
        }
        break;

    case MCS_MCU_PAUSEHDUPOLL_CMD:
        {
            m_tConf.m_tStatus.SetHduPollState(POLL_STATE_PAUSE);
            cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetHduPollInfo(), sizeof(THduPollInfo));
            SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
                        
            KillTimer(MCUVC_HDUPOLLING_CHANGE_TIMER);			
        }
        break;

    case MCS_MCU_RESUMEHDUPOLL_CMD:
        {
            m_tConf.m_tStatus.SetHduPollState(POLL_STATE_NORMAL);
            cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetHduPollInfo(), sizeof(THduPollInfo));
            SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
			m_tHduPollParam.SetIsStartAsPause( 1 );
                        
            SetTimer(MCUVC_HDUPOLLING_CHANGE_TIMER,1000*1);
        }
        break;

    case MCS_MCU_STOPHDUPOLL_CMD:
        {
            TMtPollParam tMtPollParam;
            memset(&tMtPollParam, 0, sizeof(tMtPollParam));
           
			m_tConf.m_tStatus.SetHduPollState(POLL_STATE_NONE);
            m_tConf.m_tStatus.SetHduMtPollParam(tMtPollParam);
            cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetHduPollInfo(), sizeof(THduPollInfo));
            SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
            
            THduPollInfo *ptPollInfo = m_tConf.m_tStatus.GetHduPollInfo();
            TMt tMt = (TMt)ptPollInfo->GetMtPollParam();
            ChangeHduSwitch(&tMt, ptPollInfo->GetTvWall().GetEqpId(), ptPollInfo->GetTWChnnl(), 
                                TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);
			m_tHduPollParam.SetIsStartAsPause( 0 );

            TPeriEqpStatus tHduStatus;
			g_cMcuVcApp.GetPeriEqpStatus( ptPollInfo->GetTvWall().GetEqpId(), &tHduStatus );
            tHduStatus.m_tStatus.tHdu.atHduChnStatus[ptPollInfo->GetTWChnnl()].SetSchemeIdx( 0 );
            g_cMcuVcApp.SetPeriEqpStatus( ptPollInfo->GetTvWall().GetEqpId(), &tHduStatus );
            KillTimer(MCUVC_HDUPOLLING_CHANGE_TIMER);
        }
        break;

    case MCS_MCU_GETHDUPOLLPARAM_REQ:
        {
            cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetHduPollInfo(), sizeof(THduPollInfo));
            cServMsg.CatMsgBody((u8 *)m_tHduPollParam.GetPollMtByIdx(0),
                                m_tHduPollParam.GetPolledMtNum()*sizeof(TMtPollParam));
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

    switch(pcMsg->event)
    {
    case MCS_MCU_STARTTWPOLL_CMD:
        {
            TPeriEqpStatus tTWInfo;
            TTvWallPollInfo tPollInfo = *(TTvWallPollInfo *)(cServMsg.GetMsgBody());
            
            if(tPollInfo.GetTWChnnl() >= MAXNUM_PERIEQP_CHNNL)
            {
                ConfLog(FALSE, "[ProcMcsMcuTWPollMsg] tvwall chnnl id is invalid :%d\n", tPollInfo.GetTWChnnl());
                break;
            }
            
            //ͨ�����Ѿ��Ƿ����ն�            
            g_cMcuVcApp.GetPeriEqpStatus(tPollInfo.GetTvWall().GetEqpId(), &tTWInfo);
            TTWMember tTwMem = tTWInfo.m_tStatus.tTvWall.atVideoMt[tPollInfo.GetTWChnnl()];
            if(0 != tTwMem.GetConfIdx() && tTwMem.GetConfIdx() != m_byConfIdx)//���������û��Ȩ�޲���, ��������滻
            {
                ConfLog(FALSE, "[ProcMcsMcuTWPollMsg] Confidx %d is already in polling, can't replace it\n", tTwMem.GetConfIdx());
                break;
            }
            
            if(POLL_STATE_NONE != m_tConf.m_tStatus.GetTvWallPollState())
            {
                ConfLog(FALSE, "[ProcMcsMcuTWPollMsg] tvwall chnnl id :%d is already in polling\n", tPollInfo.GetTWChnnl());
                break;
            }

            //�õ���ѯmt����
			u8 byPollMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TTvWallPollInfo)) / sizeof(TMtPollParam);
            TMtPollParam *atMtPollParam = (TMtPollParam *)( cServMsg.GetMsgBody() + sizeof(TTvWallPollInfo) );
		    m_tTvWallPollParam.InitPollParam( byPollMtNum, atMtPollParam );
            m_tTvWallPollParam.SetTvWallEqp(tPollInfo.GetTvWall());
            m_tTvWallPollParam.SetTvWallChnnl(tPollInfo.GetTWChnnl());
         
            // xsl [8/23/2006] �ж����ǻ����Ƿ�ֹͣ��ѯ
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {            
                //���ش�������������ѯ�ն����в����������ն���ܾ���ѯ
                if (IsOverSatCastChnnlNum(0))
                {
                    for(u8 byIdx = 0; byIdx < m_tTvWallPollParam.GetPolledMtNum(); byIdx++)
                    {
                        if (m_ptMtTable->GetMtSndBitrate(m_tTvWallPollParam.GetPollMtByIdx(byIdx)->GetMtId()) == 0)
                        {
                            ConfLog(FALSE, "[ProcMcsMcuTWPollMsg-start] over max upload mt num. nack!\n");  
                            NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_DCAST_OVERCHNNLNUM);
                            memset(&m_tTvWallPollParam, 0, sizeof(m_tTvWallPollParam));
                            return;
                        }
                    }
                }            
            }     

            m_tConf.m_tStatus.SetTvWallPollInfo(tPollInfo);
            m_tConf.m_tStatus.SetTvWallPollState(POLL_STATE_NORMAL);                        
            
			u8 byCurPollPos = m_tTvWallPollParam.GetCurrentIdx();
            TMtPollParam *ptCurPollMt = GetMtTWPollParam( byCurPollPos );
            m_tTvWallPollParam.SetCurrentIdx( byCurPollPos );
			
            if (NULL != ptCurPollMt)
            {
                TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);   
                
                TLogicalChannel tSrcRvsChannl;
				if (!m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
				{
						ConfLog(FALSE, "[ProcMcsMcuTWPollMsg]: GetMtLogicChnnl fail!\n");
						return;
				}
				
				// xliang [7/13/2009] DEC5 adjust resolution strategy
				if ( tSrcRvsChannl.GetChannelType() == MEDIA_TYPE_H264 &&
					( IsVidFormatHD( tSrcRvsChannl.GetVideoFormat() ) ||
					tSrcRvsChannl.GetVideoFormat() == VIDEO_FORMAT_4CIF ) )
				{
					// nack for none keda MT whose resolution is about  h264 4Cif 
					if ( MT_MANU_KDC != m_ptMtTable->GetManuId(tLocalMt.GetMtId()) )
					{
						ConfLog(FALSE, "[ProcMcsMcuTWPollMsg] Mt<%d> VidType.%d with format.%d support no tvwall\n",
							tLocalMt.GetMtId(), tSrcRvsChannl.GetChannelType(), tSrcRvsChannl.GetVideoFormat() );
// 						cServMsg.SetErrorCode(ERR_MT_MEDIACAPABILITY);
// 						SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
						return;
					}
					else
					{
						// force the mt to adjust its resolution to Cif
						// FIXME: cascade adjusting
						u8 byChnnlType = LOGCHL_VIDEO;
						u8 byNewFormat = VIDEO_FORMAT_CIF;
						CServMsg cMsg;
						cMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
						cMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
						cMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
						SendMsgToMt( tLocalMt.GetMtId(), cMsg.GetEventId(), cMsg );
						
						McsLog( "[ProcMcsMcuTWPollMsg]send videoformat<%d>(%s) change msg to mt<%d>!\n",
							byNewFormat, GetResStr(byNewFormat), tLocalMt.GetMtId() );
					}
					
				}
                //֪ͨ���л��
                m_tConf.m_tStatus.SetTvWallMtPollParam(*ptCurPollMt);
                cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo));
                SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);

                ChangeTvWallSwitch(&tLocalMt, tPollInfo.GetTvWall().GetEqpId(), tPollInfo.GetTWChnnl(), 
                                    TW_MEMBERTYPE_TWPOLL, TW_STATE_START);

                ConfStatusChange();
                
                //������ѯʱ��
                SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());
            }
            else
            {
                //������ѯʱ��
                SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER, 1000 * 5);
            }            
        }
        break;

    case MCS_MCU_PAUSETWPOLL_CMD:
        {
            m_tConf.m_tStatus.SetTvWallPollState(POLL_STATE_PAUSE);
            cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo));
            SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
                        
            KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER);			
        }
        break;

    case MCS_MCU_RESUMETWPOLL_CMD:
        {
            m_tConf.m_tStatus.SetTvWallPollState(POLL_STATE_NORMAL);
            cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo));
            SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
                        
            SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER,1000*1);
        }
        break;

    case MCS_MCU_STOPTWPOLL_CMD:
        {
            TMtPollParam tMtPollParam;
            memset(&tMtPollParam, 0, sizeof(tMtPollParam));

            m_tConf.m_tStatus.SetTvWallPollState(POLL_STATE_NONE);
            m_tConf.m_tStatus.SetTvWallMtPollParam(tMtPollParam);
            cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo));
            SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
            
            TTvWallPollInfo *ptPollInfo = m_tConf.m_tStatus.GetTvWallPollInfo();
            TMt tMt = (TMt)ptPollInfo->GetMtPollParam();
            ChangeTvWallSwitch(&tMt, ptPollInfo->GetTvWall().GetEqpId(), ptPollInfo->GetTWChnnl(), 
                                TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);

            KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER);
        }
        break;

    case MCS_MCU_GETTWPOLLPARAM_REQ:
        {
            cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo));
            cServMsg.CatMsgBody((u8 *)m_tTvWallPollParam.GetPollMtByIdx(0),
                                m_tTvWallPollParam.GetPolledMtNum()*sizeof(TMtPollParam));
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
            CfgLog( FALSE, " [ProcHduBatchPollChangeTimerMsg] current state is not hdu batch polling! \n" );
			KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
				
		    return;
		}
        
		StopHduBatchPollSwitch();
/*		
        if ( !HduBatchPollOfOneCycle( FALSE ) )
        {
            ConfLog( FALSE, "[ProcHduBatchPollChangeTimerMsg] have no Mt in current conf!\n" );
			cServMsg.SetErrorCode( ERR_MCU_HDUBATCHPOLL_NONEMT );
			SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 2, cServMsg );
            KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
			return;
        }
*/        
		// ��ѯ����
		if ( ( !HduBatchPollOfOneCycle(FALSE) ) 
			|| ( 0 != m_tHduBatchPollInfo.GetCycles() 
				&& m_tHduBatchPollInfo.GetCurrentCycle() == m_tHduBatchPollInfo.GetCycles() )
			)
		{
			StopHduBatchPollSwitch(TRUE);
			m_tHduBatchPollInfo.SetNull();
			m_tConf.m_tStatus.m_tConfMode.SetHduInBatchPoll( POLL_STATE_NONE );
			m_tConf.GetConfAttrbEx().SetSchemeIdxInBatchPoll( 0 );
			m_tHduPollSchemeInfo.SetStatus( POLL_STATE_NONE );
			ConfStatusChange();            // ֪ͨ��ػ���״̬�ı�

			cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof( m_tHduPollSchemeInfo ) );
			SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );
			
			//֪ͨ���л��
			cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
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
		OspPrintf( TRUE, FALSE, "[ProcHduBatchPollChangeTimerMsg] state(%d) is wrong! \n", CurState() );
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
  07/05/09    4.6         ���ֱ�         ����
=============================================================================*/
void CMcuVcInst::ProcHduPollingChangeTimerMsg(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
    
    switch(CurState())
    {
    case STATE_ONGOING:
        {
            if (POLL_STATE_NONE == m_tConf.m_tStatus.GethduPollState()) 
            {
                break;
            }
			TMtPollParam *ptCurPollMt = NULL;
			u8 byCurPollPos = m_tHduPollParam.GetCurrentIdx();
            if ( 0 == m_tHduPollParam.GetIsStartAsPause() )   
            {
				byCurPollPos++;
            }
			else // ����ͣ�ָ�ʱ�����ŵ�ǰ�ն���ѯ   
			{
				 m_tHduPollParam.SetIsStartAsPause( 0 );
			}

            ptCurPollMt = GetMtHduPollParam( byCurPollPos );
			m_tHduPollParam.SetCurrentIdx( byCurPollPos );
            
            BOOL32 bStopPoll = FALSE;
            // �ж����ǻ����Ƿ�ֹͣ��ѯ
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                TMt tLastPollMt = (TMt)m_tConf.m_tStatus.GetHduMtPollParam();
                if (IsMtSendingVidToOthers(tLastPollMt, FALSE, TRUE, 0) && //��һ���ն˻���������ʵ�巢������
                    m_ptMtTable->GetMtSndBitrate(ptCurPollMt->GetMtId()) == 0 && //��һ���ն�û���ڷ�������
                    IsOverSatCastChnnlNum(0))
                {
                    bStopPoll = TRUE;
                }
            }        

            if (NULL != ptCurPollMt && !bStopPoll)
            {
                //֪ͨ���л��
                TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);
                
                m_tConf.m_tStatus.SetHduMtPollParam( *ptCurPollMt );
                cServMsg.SetMsgBody( (u8*)m_tConf.m_tStatus.GetHduPollInfo(), sizeof(THduPollInfo) );
                SendMsgToAllMcs( MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg );
                
                THduPollInfo *ptPollInfo = m_tConf.m_tStatus.GetHduPollInfo();
                ChangeHduSwitch(&tLocalMt, ptPollInfo->GetTvWall().GetEqpId(), ptPollInfo->GetTWChnnl(), 
                                   TW_MEMBERTYPE_TWPOLL, TW_STATE_CHANGE, m_tHduPollParam.GetMode());               
                                
                SetTimer(MCUVC_HDUPOLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());

                McsLog("[ProcHduPollingChangeTimerMsg] new mt %d in polling, remain poll times :%u\n", ptCurPollMt->GetMtId(), ptPollInfo->GetPollNum());
            
			    
			}
            else
            {              
                TMtPollParam tMtPollParam;
                memset(&tMtPollParam, 0, sizeof(tMtPollParam));

                m_tConf.m_tStatus.SetHduPollState(POLL_STATE_NONE);
                m_tConf.m_tStatus.SetHduMtPollParam(tMtPollParam);
                cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetHduPollInfo(), sizeof(THduPollInfo));
                SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
                
                THduPollInfo *ptPollInfo = m_tConf.m_tStatus.GetHduPollInfo();
                TMt tMt = (TMt)ptPollInfo->GetMtPollParam();
                ChangeHduSwitch(&tMt, ptPollInfo->GetTvWall().GetEqpId(), ptPollInfo->GetTWChnnl(), 
                                    TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);
                
                KillTimer(MCUVC_HDUPOLLING_CHANGE_TIMER);

                McsLog("[ProcTWPollingChangeTimerMsg] tvwall polling finished\n");
            }
            
            break;
        }
        
    default:
        break;
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
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
    
    switch(CurState())
    {
    case STATE_ONGOING:
        {
            if (POLL_STATE_NONE == m_tConf.m_tStatus.GetTvWallPollState()) 
            {
                break;
            }
			u8 byCurPollPos = m_tTvWallPollParam.GetCurrentIdx();
            byCurPollPos++;
            TMtPollParam *ptCurPollMt = GetMtTWPollParam( byCurPollPos );
			m_tTvWallPollParam.SetCurrentIdx( byCurPollPos );
            
            BOOL32 bStopPoll = FALSE;
            // xsl [8/23/2006] �ж����ǻ����Ƿ�ֹͣ��ѯ
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                TMt tLastPollMt = (TMt)m_tConf.m_tStatus.GetTvWallMtPollParam();
                if (IsMtSendingVidToOthers(tLastPollMt, FALSE, TRUE, 0) && //��һ���ն˻���������ʵ�巢������
                    m_ptMtTable->GetMtSndBitrate(ptCurPollMt->GetMtId()) == 0 && //��һ���ն�û���ڷ�������
                    IsOverSatCastChnnlNum(0))
                {
                    bStopPoll = TRUE;
                }
            }        

            if (NULL != ptCurPollMt && !bStopPoll)
            {
                //֪ͨ���л��
                TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);
                
				TLogicalChannel tSrcRvsChannl;
				if (!m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
				{
					ConfLog(FALSE, "[ProcMcsMcuTWPollMsg]: GetMtLogicChnnl fail!\n");
					return;
				}
				
				// xliang [7/13/2009] DEC5 adjust resolution strategy
				if ( tSrcRvsChannl.GetChannelType() == MEDIA_TYPE_H264 &&
					( IsVidFormatHD( tSrcRvsChannl.GetVideoFormat() ) ||
					tSrcRvsChannl.GetVideoFormat() == VIDEO_FORMAT_4CIF ) )
				{
					// nack for none keda MT whose resolution is about  h264 4Cif 
					if ( MT_MANU_KDC != m_ptMtTable->GetManuId(tLocalMt.GetMtId()) )
					{
						ConfLog(FALSE, "[ProcMcsMcuTWPollMsg] Mt<%d> VidType.%d with format.%d support no tvwall\n",
							tLocalMt.GetMtId(), tSrcRvsChannl.GetChannelType(), tSrcRvsChannl.GetVideoFormat() );
						// 						cServMsg.SetErrorCode(ERR_MT_MEDIACAPABILITY);
						// 						SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
						return;
					}
					else
					{
						// force the mt to adjust its resolution to Cif
						// FIXME: cascade adjusting
						u8 byChnnlType = LOGCHL_VIDEO;
						u8 byNewFormat = VIDEO_FORMAT_CIF;
						CServMsg cMsg;
						cMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
						cMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
						cMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
						SendMsgToMt( tLocalMt.GetMtId(), cMsg.GetEventId(), cMsg );
						
						McsLog( "[ProcMcsMcuTWPollMsg]send videoformat<%d>(%s) change msg to mt<%d>!\n",
							byNewFormat, GetResStr(byNewFormat), tLocalMt.GetMtId() );
					}
					
				}

                m_tConf.m_tStatus.SetTvWallMtPollParam( *ptCurPollMt );
                cServMsg.SetMsgBody( (u8*)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo) );
                SendMsgToAllMcs( MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg );
                
                TTvWallPollInfo *ptPollInfo = m_tConf.m_tStatus.GetTvWallPollInfo();
                ChangeTvWallSwitch(&tLocalMt, ptPollInfo->GetTvWall().GetEqpId(), ptPollInfo->GetTWChnnl(), 
                                   TW_MEMBERTYPE_TWPOLL, TW_STATE_CHANGE);               
                                
                SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());

                McsLog("[ProcTWPollingChangeTimerMsg] new mt %d in polling, remain poll times :%u\n", ptCurPollMt->GetMtId(), ptPollInfo->GetPollNum());
            }
            else
            {              
                TMtPollParam tMtPollParam;
                memset(&tMtPollParam, 0, sizeof(tMtPollParam));

                m_tConf.m_tStatus.SetTvWallPollState(POLL_STATE_NONE);
                m_tConf.m_tStatus.SetTvWallMtPollParam(tMtPollParam);
                cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo));
                SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
                
                TTvWallPollInfo *ptPollInfo = m_tConf.m_tStatus.GetTvWallPollInfo();
                TMt tMt = (TMt)ptPollInfo->GetMtPollParam();
                ChangeTvWallSwitch(&tMt, ptPollInfo->GetTvWall().GetEqpId(), ptPollInfo->GetTWChnnl(), 
                                    TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);
                
                KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER);

                McsLog("[ProcTWPollingChangeTimerMsg] tvwall polling finished\n");
            }
            
            break;
        }
        
    default:
        break;
    }
    
    return;
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
                McsLog("[ProcMcsMcuRollCallMsg] unexpected Rollcall mode.%d\n", byRollCallMode);
                return;
            }

            if ( m_tConf.m_tStatus.IsMustSeeSpeaker() )
            { 
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_MUSTSEESPEAKER);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] can't start rollcall due to conf mode.mustseespeaker\n");
                return;
            }

            // zbq[11/24/2007] ������ѯ��ʱ������������
            if ( m_tConf.m_tStatus.GetPollMode() != CONF_POLLMODE_NONE )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_POLL);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] can't start rollcall due to conf pollmode.%d\n", m_tConf.m_tStatus.GetPollMode());
                return;
            }

            // zbq[03/31/2008] ������ԴԤ��(FIXME��δռ�ã����ܻᱻ��)
            u8 byGrpId = 0;
            u8 byEqpId = 0;
            if ( 0 == g_cMcuVcApp.GetIdleMixGroup(byEqpId, byGrpId) &&
				// �µ����߼�֧�ֱ������ڳ����ģʽ��������������жϱ�����
				// �Ƿ�ռ���˻�����, zgc, 2008-05-22
				m_tConf.m_tStatus.IsNoMixing() &&
				!m_tConf.m_tStatus.IsVACing() )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_NOMIXER);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] can't start rollcall due to no idle mixer\n");
                return;
            }

            u8 byIdleVMPNum = 0;
            u8 abyIdleVMPId[MAXNUM_PERIEQP];
            memset( &abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
            g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );    
			// BUG10614�������������ܵ�, zgc, 2008-04-28
			// ��VMPģʽ����Ҫ�ж��Ƿ���VMP����
            // �µ����߼�֧�ֱ��������л��ϳ�ģʽ��������������ж��Ƿ��Ǳ�����ռ���˻���
            // �ϳ�����zgc, 2008-05-23
            if ( byIdleVMPNum == 0 && byRollCallMode == ROLLCALL_MODE_VMP &&
                m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_NONE )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_NOVMP);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] can't start rollcall due to no idle vmp\n");
                return;
            }
            // zgc, 2008-05-29, ���VMP��������������������涼�޷�֧�֣���ܾ�����
            if ( byRollCallMode == ROLLCALL_MODE_VMP )
            {
                u16 wError = 0;
                u8 byVmpId = 0;
                BOOL32 bRet = IsMCUSupportVmpStyle(VMP_STYLE_VTWO, byVmpId, EQP_TYPE_VMP, wError);
                if ( !bRet )
                {
                    cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_VMPABILITY);
                    SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                    McsLog("[ProcMcsMcuRollCallMsg] can't start rollcall due to vmp ability is not enough!\n");
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
                CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode() &&
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
            if ( CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode() &&
                 ROLLCALL_MODE_VMP != byRollCallMode )
            {
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_VMPING );
            }

            BOOL32 bSelExist = FALSE;
            for( u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++ )
            {
                if ( m_tConfAllMtInfo.MtJoinedConf( byMtIdx ) ) 
                {
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
                
                cMsg.content = cServMsg.GetServMsg();
                cMsg.length = cServMsg.GetServMsgLen();
                ProcMcsMcuLockSMcuReq(&cMsg);                
            }

            u8 byLockMode = CONF_LOCKMODE_LOCK;
            cSMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);
            cSMsg.SetMsgBody((u8*)&byLockMode, sizeof(u8));
            ChangeConfLockMode(cSMsg);

            // zbq [11/22/2007] �õ�ǰ����Ϊ����ϯ����
            m_tConf.m_tStatus.SetNoChairMode( TRUE );
            if( m_tConf.m_tStatus.IsNoChairMode() && HasJoinedChairman() )
            {
                ChangeChairman( NULL );	
            }

            m_tConf.m_tStatus.SetRollCallMode( byRollCallMode );
            
            McsLog("[ProcMcsMcuRollCallMsg] RollCallMode.%d\n", byRollCallMode);

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
                McsLog("[ProcMcsMcuRollCallMsg] Rollcall mode.%d, can't change caller or callee\n", m_tConf.m_tStatus.GetRollCallMode());
                return;
            }
            if ( cServMsg.GetMsgBodyLen() != sizeof(TMt)*2 )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_MSGLEN);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] unexpected msglen.%d, need.%d\n", cServMsg.GetMsgBodyLen(), sizeof(TMt)*2 );
                return;
            }

            TMt tNewRollCaller = *(TMt*)cServMsg.GetMsgBody();
            TMt tNewRollCallee = *(TMt*)(cServMsg.GetMsgBody() + sizeof(TMt));

            if ( tNewRollCaller.IsNull() )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_CALLERNULL);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] Caller Null, ignore it\n");
                return;
            }

            if ( tNewRollCallee.IsNull() )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_CALLEENULL);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] Callee Null, ignore it\n");
                return;
            }

            BOOL32 bCallerChged = tNewRollCaller == m_tRollCaller ? FALSE : TRUE;
            BOOL32 bCalleeChged = tNewRollCallee == m_tRollCallee ? FALSE : TRUE;

            if ( bCallerChged && !tNewRollCaller.IsLocal() )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_CALLER_SMCUMT);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] Mt<%d,%d> can't be caller, it belong to SMcu\n", 
                        tNewRollCaller.GetMcuId(), tNewRollCaller.GetMtId() );
                return;
            }

            SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

            TMt tOldRollCaller = m_tRollCaller;
            TMt tOldRollCallee = m_tRollCallee;

            m_tRollCaller = tNewRollCaller;
            m_tRollCallee = tNewRollCallee;

            if ( !bCallerChged && !bCalleeChged )
            {
                McsLog("[ProcMcsMcuRollCallMsg] there is no change to caller and callee both\n");
                return;
            }

            // ��VMP����ģʽ����Ҫͣ�ϳ�, zgc, 2008-05-23
            if ( ROLLCALL_MODE_VMP != m_tConf.m_tStatus.GetRollCallMode() &&
                CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode() )
            {
                //ֹͣʱģ��ʧЧ
			    if( m_tConf.GetConfAttrb().IsHasVmpModule() )
			    {
				    TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
                    tConfAttrib.SetHasVmpModule(FALSE);
                    m_tConf.SetConfAttrb( tConfAttrib );
                }

                //ֹͣ
				TPeriEqpStatus tPeriEqpStatus;
				if (!m_tVmpEqp.IsNull() &&
					g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
					tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
					tPeriEqpStatus.m_tStatus.tVmp.m_byUseState != TVmpStatus::WAIT_STOP)
				{
					SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, 6*1000);
					tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
					g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tPeriEqpStatus);
					g_cEqpSsnApp.SendMsgToPeriEqpSsn( m_tVmpEqp.GetEqpId(),MCU_VMP_STOPVIDMIX_REQ, 
													  (u8*)cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

				}
            }

            // zbq [11/23/2007] �������˻򱻵����˷�����λ��
            if ( bCallerChged &&
                 ROLLCALL_MODE_CALLEE != m_tConf.m_tStatus.GetRollCallMode() )
            {
				// KDV-BUG2004: �����˻򱻵����˱�ǿ��Ϊ�����ˣ�����MTC���ն�״̬δ�ı�
				// zgc, 2008-05-21, ��ChangeSpeaker��������
                //m_tConf.SetSpeaker( tNewRollCaller );
				ChangeSpeaker( &tNewRollCaller );
            }
            if ( bCalleeChged &&
                 ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
            {
				// KDV-BUG2004: �����˻򱻵����˱�ǿ��Ϊ�����ˣ�����MTC���ն�״̬δ�ı�
				// zgc, 2008-05-21, ��ChangeSpeaker��������
                //m_tConf.SetSpeaker( tNewRollCallee );
				ChangeSpeaker( &tNewRollCallee );
            }
            //֪ͨ���л��
			// KDV-BUG2004: �����˻򱻵����˱�ǿ��Ϊ�����ˣ�����MTC���ն�״̬δ�ı�
			// zgc, 2008-05-21, ��ChangeSpeaker�����д���
            //cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
            //SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

            //��������
            BOOL32 bLocalMixStarted = FALSE;
            
            if ( bCallerChged )
            {
                if ( !m_tConf.m_tStatus.IsNoMixing() )
                {
                    BOOL32 bLocalAutoMix = FALSE;
                    
                    // ͣ��������
                    if ( m_tConf.m_tStatus.IsVACing() )
                    {
                        m_tConf.m_tStatus.SetVACing(FALSE);
                        //֪ͨ��ϯ�����л��
                        SendMsgToAllMcs(MCU_MCS_STOPVAC_NOTIF, cServMsg);
                        if (HasJoinedChairman())
                        {
                            SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cServMsg);
                        }
                    }
                    
                    //ͣ����
                    if ( m_tConf.m_tStatus.IsAutoMixing() )
                    {
                        bLocalAutoMix = TRUE;

                        m_tConf.m_tStatus.SetAutoMixing(FALSE);
                        m_tConf.m_tStatus.SetSpecMixing(TRUE);
                        
                        //FIXME���Ƿ�Ҫ�������� 56
                        //���ԭ�ȵ�N-1ģʽ����
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
                    
                    //�������Ƕ��ƻ���
                    if ( m_tConf.m_tStatus.IsSpecMixing() )
                    {
                        //��һ�ε������Ƴ���ǰ�����ն�
                        if ( tOldRollCaller.IsNull() )
                        {
                            for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
                            {
                                TMt tMt = GetLocalMtFromOtherMcuMt(tNewRollCallee);
                                
                                if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
                                    m_ptMtTable->IsMtInMixing(byMtId) &&
                                    !(m_ptMtTable->GetMt(byMtId) == tMt))
                                {
                                    TMt tMt = m_ptMtTable->GetMt(byMtId);
                                    AddRemoveSpecMixMember(&tMt, 1, FALSE, TRUE);
                                }
                            }
                            AddRemoveSpecMixMember(&tNewRollCaller, 1, TRUE);
                        }
                        else
                        {
                            AddRemoveSpecMixMember(&tNewRollCaller, 1, TRUE);
                            AddRemoveSpecMixMember(&tOldRollCaller, 1, FALSE);                        
                        }
                    }
                    
                    // �����ǰ����������ģʽ, ������Nģʽ�Ľ���
                    if ( bLocalAutoMix )
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
            }
            if ( bCalleeChged )
            {
                if ( !m_tConf.m_tStatus.IsNoMixing() )
                {
                    BOOL32 bLocalAutoMix = FALSE;
                    
                    // ͣ��������
                    if ( m_tConf.m_tStatus.IsVACing() )
                    {
                        m_tConf.m_tStatus.SetVACing(FALSE);
                        
                        //֪ͨ��ϯ�����л��
                        SendMsgToAllMcs(MCU_MCS_STOPVAC_NOTIF, cServMsg);
                        if (HasJoinedChairman())
                        {
                            SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cServMsg);
                        }
                    }
                    
                    //ͣ����
                    if ( m_tConf.m_tStatus.IsAutoMixing() )
                    {
                        bLocalAutoMix = TRUE;

                        m_tConf.m_tStatus.SetAutoMixing(FALSE);
                        
                        //FIXME���Ƿ�Ҫ�������� 56
                        //���ԭ�ȵ�N-1ģʽ����
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
                    
                    //�������Ƕ��ƻ���
                    if ( m_tConf.m_tStatus.IsSpecMixing() )
                    {
                        //��һ�ε������Ƴ���ǰ�����ն�
                        if ( tOldRollCallee.IsNull() )
                        {
                            for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
                            {
                                TMt tMt = GetLocalMtFromOtherMcuMt(tNewRollCaller);
                                
                                if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
                                    m_ptMtTable->IsMtInMixing(byMtId) &&
                                    !(m_ptMtTable->GetMt(byMtId) == tMt))
                                {
                                    TMt tMt = m_ptMtTable->GetMt(byMtId);
                                    AddRemoveSpecMixMember(&tMt, 1, FALSE, TRUE);
                                }
                            }
                            AddRemoveSpecMixMember(&tNewRollCallee, 1, TRUE);
                        }
                        else
                        {
                            AddRemoveSpecMixMember(&tOldRollCallee, 1, FALSE);
                            AddRemoveSpecMixMember(&tNewRollCallee, 1, TRUE);
                            
                            //zbq[11/30/2007] �µ�Callee�;ɵ�Callee����ͬһ��
                            //MCU����Callee���ڵ�MCU�Ƴ��ϼ������飬��ͣ�����.
                            if (!(!tOldRollCallee.IsLocal() && 
                                  !tNewRollCallee.IsLocal() && 
                                  tOldRollCallee.GetMcuId() == tNewRollCallee.GetMcuId()))
                            {
                                TMt tSMcu = GetLocalMtFromOtherMcuMt(tOldRollCallee);
                                AddRemoveSpecMixMember(&tSMcu, 1, FALSE);                                
                            }
                        }
                    }
                    
                    // �����ǰ����������ģʽ, ������Nģʽ�Ľ���
                    if ( bLocalAutoMix )
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
            }
            
            //�����㲥����
            TVMPParam tVmpParam;
            if ( m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP )
            {
                tVmpParam = m_tConf.m_tStatus.GetVmpParam();
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
                u8 byVmpId = 0;
                BOOL32 bRet = IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError);
                if ( !bRet )
                {
                    //NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_STYLECHANGE);           
                    tVmpParam.SetVMPStyle(VMP_STYLE_VTWO);
                    McsLog("[ProcMcsMcuRollCallMsg] rollcall change to style.%d due to vmp ability is not enough!\n",
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
						ConfLog( FALSE, "New roll caller(Mt.%d) is not local mt, it's impossible\n", tNewRollCaller.GetMtId() );
					}
                }
                else if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
                {
                    u8 byCallerPos = 0;
                    if ( !tOldRollCaller.IsNull() &&
                         m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
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
                         m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
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
                if ( CONF_VMPMODE_CTRL == m_tConf.m_tStatus.GetVMPMode() )
                {
                    m_tConf.m_tStatus.SetVmpParam(tVmpParam);

                    TPeriEqpStatus tPeriEqpStatus;
                    g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
                    tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
                    g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
                    
                    //ChangeVmpParam(&tVmpParam);
					// xliang [1/6/2009] ��������VMP��VMP param
					AdjustVmpParam(&tVmpParam);

                }
                else if ( CONF_VMPMODE_AUTO == m_tConf.m_tStatus.GetVMPMode() )
                {
                    m_tConf.m_tStatus.SetVMPMode(CONF_VMPMODE_CTRL);

                    m_tConf.m_tStatus.SetVmpParam(tVmpParam);

                    TPeriEqpStatus tPeriEqpStatus;
                    g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
                    tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
                    g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
                    
                    //ChangeVmpParam(&tVmpParam);
					// xliang [1/6/2009] ��������VMP��VMP param
					AdjustVmpParam(&tVmpParam);
					
                    ConfModeChange();
                }
                else
                {
                    CServMsg cMsg;
                    cMsg.SetEventId(MCS_MCU_STARTVMP_REQ);
                    cMsg.SetMsgBody((u8*)&tVmpParam, sizeof(TVMPParam));
					
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
                    }
                    
                    //ѡ��ʧ�ܣ��ָ����㲥�����ܿ�vmp���Լ�
					//������ѡ����������
                    if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
                    {
                        RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
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
                    if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
                    {
                        RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
                    }
                }
                else // ROLLCALL_MODE_CALLEE
                {
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
                    if ( tOldRollCallee.IsLocal() &&
                        !tOldRollCallee.IsNull() )
                    {
                        StopSelectSrc(tOldRollCallee, MODE_VIDEO);
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
                McsLog("[ProcMcsMcuRollCallMsg] RollCall mode.%d already\n", m_tConf.m_tStatus.GetRollCallMode());
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
    CServMsg cSMsg;
    u8 byLockMode = CONF_LOCKMODE_NONE;
    cSMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);
    cSMsg.SetMsgBody((u8*)&byLockMode, sizeof(u8));
    ChangeConfLockMode(cSMsg);
    
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
            CServMsg cMsg;
            cMsg.SetConfIdx( m_byConfIdx );
            cMsg.SetConfId( m_tConf.GetConfId() );
            cMsg.SetEventId(MCS_MCU_STOPVMP_REQ);
            VmpCommonReq(cMsg);
        }
        RestoreAllSubMtJoinedConfWatchingSrcMt(m_tVidBrdSrc, MODE_VIDEO);
        
        //ͣѡ��
        StopSelectSrc(m_tRollCaller, MODE_BOTH);
        if ( m_tRollCallee.IsLocal() )
        {
            StopSelectSrc(m_tRollCallee, MODE_BOTH);
        }
        
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
void CMcuVcInst::DaemonProcMcsMcuListAllConfReq( const CMessage * pcMsg, CApp* pcApp )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	NtfMcsMcuCurListAllConf(cServMsg);

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
void CMcuVcInst::DaemonProcMcsMcuGetMcuStatusReq( const CMessage * pcMsg, CApp* pcApp )
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
		StopSwitchToMc( cServMsg.GetSrcSsnId(), cServMsg.GetChnIndex() );
		break;
	case MCU_MCSDISCONNECTED_NOTIF:
		byInstId = *( u8 * )cServMsg.GetMsgBody();
		if( g_cMcuVcApp.GetMcLogicChnnl( byInstId, MODE_VIDEO, &byChannelNum, &tLogicalChannel ) )
		{
			while( byChannelNum-- != 0 )
            {
			    StopSwitchToMc( byInstId, byChannelNum, FALSE, MODE_BOTH, FALSE );
            }
		}
		break;
    default:
        ConfLog( FALSE, "ProcMcStopSwitch: unexpected msg %d<%s> received !\n", cServMsg.GetEventId(), OspEventDesc(cServMsg.GetEventId()) );
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
	TLogicalChannel tLogicalChannel;
	
	tHduSwitchInfo = *( THduSwitchInfo * )cServMsg.GetMsgBody();
	tEqp = tHduSwitchInfo.GetDstMt();
	
	// Nack
	if( tEqp.GetEqpType() != EQP_TYPE_HDU )
	{
		ConfLog( FALSE, "[CMcuVcInst]::ProcMcsMcuStopSwitchHduReq: Wrong PeriEquipment type %u or it has not registered!\n", 
			tEqp.GetEqpType() );
		cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	TMt tRemoveMt;
    TPeriEqpStatus tHduStatus;
    u8             byHduId = 0;
    u8             byChnlNo = 0;
    u8             byMtId;
    byHduId = tEqp.GetEqpId();
    byChnlNo   = tHduSwitchInfo.GetDstChlIdx();
    g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
    tRemoveMt = tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlNo];

	if (tRemoveMt.GetType() != TYPE_MCUPERI)
	{
		if (tRemoveMt.GetConfIdx() != m_byConfIdx)
		{
			return;
		}
	}
    byMtId = tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlNo].GetMtId();
	
	StopSwitchToPeriEqp( tEqp.GetEqpId(), tHduSwitchInfo.GetDstChlIdx() );
	
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
    if( IsMtNotInOtherHduChnnl(byMtId, byHduId, byChnlNo) )  
    {
        m_ptMtTable->SetMtInHdu(byMtId, FALSE);
        MtStatusChange();
    }
    
    tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlNo].byMemberType = 0;
    tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlNo].SetNull();
	tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlNo].SetConfIdx( 0 );
    tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnlNo].SetSchemeIdx( 0 );
    g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);

	cServMsg.SetMsgBody((u8*)&tHduStatus, sizeof(tHduStatus));

	//  pengjie[7/27/2009]  �޸���һ��mcs�ر�һ��ͨ������һ��msc��ʾ��ͨ�����ն˻���
	SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);  

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
		OspPrintf(TRUE, FALSE, "[ProcMcsMcuChangeHduVolume] pcMsg is Null!\n");
		return;
	}
	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
    THduVolumeInfo tHduVolumeInfo = *(THduVolumeInfo*)cServMsg.GetMsgBody();
	TPeriEqpStatus tHduStatus;
    u8 byChnlIdx = tHduVolumeInfo.GetChnlIdx();
	g_cMcuVcApp.GetPeriEqpStatus( tHduVolumeInfo.GetEqpId(), &tHduStatus );

    if ( 0 == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx() )
    {
        tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetConfIdx( m_byConfIdx );
    }

    if ( m_byConfIdx != tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx() )
    {
		ConfLog( FALSE, "[CMcuVcInst]::ProcMcsMcuChangeHduVolume: Can not change hdu volume or set mute with different ConfIdx\n");
        cServMsg.SetErrorCode( ERR_MCU_HDUCHANGEVOLUME );
		SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 2, cServMsg );
		return;
    }

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
		OspPrintf(TRUE, FALSE, "[ProcMcsMcuStartSwitchHduReq] pcMsg is NULL!\n");
		return;
    }
	
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
	THduSwitchInfo  tHduSwitchInfo;

	if (MCUVC_STARTSWITCHHDU_NOTIFY ==  pcMsg->event)
	{
		cServMsg.SetMsgBody(pcMsg->content, sizeof( THduSwitchInfo ));
	}

    tHduSwitchInfo = *(THduSwitchInfo*)cServMsg.GetMsgBody();

    TMtStatus      tMtStatus;
    TMt	           tDstMt;
    TMt            tSrcMt;
    TMt *          ptSrcMt = NULL;
    u8             byHduId = 0;
    BOOL32         bNeedSwitchMt = FALSE;    
    u8             byDstChnnlIdx;
    TMt            tSrcOrg;             //��Լ���������

    TLogicalChannel tSrcRvsChannl;
    switch(CurState())
    {
    case STATE_ONGOING:
	{
		tSrcMt = tHduSwitchInfo.GetSrcMt();
		tDstMt = tHduSwitchInfo.GetDstMt();
        byHduId = tDstMt.GetEqpId();
        tSrcOrg = tSrcMt;
        byDstChnnlIdx = tHduSwitchInfo.GetDstChlIdx();

        //���Ͳ��Ի����ڱ�MCU��δ�Ǽ�
        if (EQP_TYPE_HDU != tDstMt.GetEqpType() ||
            !g_cMcuVcApp.IsPeriEqpConnected(byHduId) ||
            byDstChnnlIdx >= MAXNUM_HDU_CHANNEL)
        {
            ConfLog(FALSE, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: Wrong PeriEquipment type %u or chnnlidx %d or it(eqpid:%d) has not registered!\n", 
                            tDstMt.GetEqpType(), byDstChnnlIdx, byHduId);
            cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

 	    //����ǽ��ѯ���ȼ���ߣ�����ѡ��VMP��ѡ���ն�
	    TPeriEqpStatus tHduStatus;
        g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
        if ( TW_MEMBERTYPE_TWPOLL ==  tHduStatus.m_tStatus.tHdu.atVideoMt[byDstChnnlIdx].byMemberType)
        {
			ConfLog(FALSE, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: current chnl is in polling ,can not switch VMP or mt!\n");
          	cServMsg.SetErrorCode(ERR_MCU_NOTSWITCHVMPORMT);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
		}

        if (tSrcMt.GetType() != TYPE_MCUPERI)
        {
			if (!tSrcMt.IsLocal())
			{
				tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);
			}
        }
		else
		{
			//ѡ����vmpΪ��ʱ                      
			if (m_tVmpEqp.IsNull())
			{
				ConfLog(FALSE, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: PeriEquipment type %u has not registered!\n", 
					tDstMt.GetEqpType());
				cServMsg.SetErrorCode(ERR_MCU_MCUPERI_NOTEXIST);
	            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}

			tSrcMt = m_tVmpEqp;
		}


        // δ�򿪺���ͨ�����������������б���
		if ( tSrcMt.GetType() != TYPE_MCUPERI && TW_MEMBERTYPE_MCSSPEC == tHduSwitchInfo.GetMemberType())
		{
			if (!m_ptMtTable->GetMtLogicChnnl( tSrcMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
			{
				ConfLog(FALSE, "[ProcMcsMcuStartSwitchHduReq]: SrcMt(%d) GetMtLogicChnnl fail!\n", tSrcMt.GetMtId());
				cServMsg.SetErrorCode(ERR_MCU_GETMTLOGCHN_FAIL);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
		}

        if (tSrcMt.GetType() != TYPE_MCUPERI && !tSrcOrg.IsLocal())
        {		
            OnMMcuSetIn(tSrcOrg, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
            tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);
        }

        // xsl [7/20/2006] ���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
        if (m_tConf.GetConfAttrb().IsSatDCastMode())
        {
            if (IsOverSatCastChnnlNum(tSrcMt.GetMtId()))
            {
                ConfLog(FALSE, "[ProcMcsMcuStartSwitchHduReq] over max upload mt num. nack!\n");
                cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
        }
	
        switch(tHduSwitchInfo.GetMemberType()) 
        {
        case TW_MEMBERTYPE_MCSSPEC:

            tSrcMt = m_ptMtTable->GetMt(tSrcMt.GetMtId());

            //Դ�ն˱���߱���Ƶ��������
            m_ptMtTable->GetMtStatus(tSrcMt.GetMtId(), &tMtStatus);
            if (FALSE == tMtStatus.IsSendVideo())
            {
                cServMsg.SetErrorCode(ERR_MCU_SRCISRECVONLY);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
                return;
            }	
			
            //Դ�ն˱����Ѿ��������
            if (FALSE == m_tConfAllMtInfo.MtJoinedConf(tSrcMt.GetMtId()))
            {
                log(LOGLVL_DEBUG1, "[ProcMcsMcuStartSwitchHduReq]: select source MT%u-%u has not joined current conference! Error!\n",
                                    tSrcMt.GetMcuId(), tSrcMt.GetMtId());
                cServMsg.SetErrorCode(ERR_MCU_MT_NOTINCONF);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);	//nack
                return;
            }

            bNeedSwitchMt = TRUE;
            break;

        case TW_MEMBERTYPE_SPEAKER:     //ע��ı䷢����ʱ�ı佻��
			
            if (TRUE == HasJoinedSpeaker())
            {
                bNeedSwitchMt = TRUE;
                tSrcMt = GetLocalSpeaker();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;

        case TW_MEMBERTYPE_CHAIRMAN:    //ע��ı���ϯʱ�ı佻��

            if (TRUE == HasJoinedChairman())
            {
                bNeedSwitchMt = TRUE;
                tSrcMt = m_tConf.GetChairman();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;

        case TW_MEMBERTYPE_POLL:        //ע��ı���ѯ�㲥Դʱ�ı佻��

            if (CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode())
            {
                bNeedSwitchMt = TRUE;
                tSrcMt = (TMt)m_tConf.m_tStatus.GetMtPollParam();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;
			
		case TW_MEMBERTYPE_SWITCHVMP:      //hduѡ��vmp
		    if (!m_tVmpEqp.IsNull())
			{
			    tSrcMt = (TMt)m_tVmpEqp;
			    bNeedSwitchMt = TRUE;
			}
			else
			{
				tSrcMt.SetNull();
			}
			break;

		case TW_MEMBERTYPE_BATCHPOLL:
            bNeedSwitchMt = TRUE;
			break;

		case TW_MEMBERTYPE_TWPOLL:
			break;

        default:
            ConfLog(FALSE, "[ProcMcsMcuStartSwitchHduReq]: Wrong TvWall Member Type!\n");
            break;
		}

        tHduStatus.m_tStatus.tHdu.atHduChnStatus[byDstChnnlIdx].SetSchemeIdx(tHduSwitchInfo.GetSchemeIdx());
        g_cMcuVcApp.SetPeriEqpStatus( byHduId, &tHduStatus );

        cServMsg.SetConfIdx(tSrcMt.GetConfIdx());

        //send Ack reply
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
                
        if (bNeedSwitchMt)
        {
            ptSrcMt = &tSrcMt;
        }    

		if ( TW_MEMBERTYPE_POLL == tHduSwitchInfo.GetMemberType() )   // �����Ƿ����Ƶ��ѯ
		{
			ChangeHduSwitch(ptSrcMt, byHduId, byDstChnnlIdx, TW_MEMBERTYPE_POLL, 
				   TW_STATE_START, m_tConf.m_tStatus.GetPollMedia() );
		}
		else if ( TW_MEMBERTYPE_BATCHPOLL == tHduSwitchInfo.GetMemberType() )   // ������ѯʱ������Ƶ��������
		{
			ChangeHduSwitch( ptSrcMt, byHduId, \
				byDstChnnlIdx, TW_MEMBERTYPE_BATCHPOLL, TW_STATE_START, MODE_BOTH);
		}
        else
		{
			ChangeHduSwitch(ptSrcMt, byHduId, byDstChnnlIdx, 
				tHduSwitchInfo.GetMemberType(),	TW_STATE_START);
		}
        break;
    }
    default:
        ConfLog(FALSE, "[ProcMcsMcuStartSwitchHduReq]: Wrong handle in state %u!\n", CurState());
        break;
	}

	return;
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
    TMt *          ptSrcMt = NULL;
    u8             byTvWallId = 0;
    BOOL32         bNeedSwitchMt = FALSE;    
    u8             byDstChnnlIdx;
    TMt            tSrcOrg;             //��Լ���������

    TLogicalChannel tSrcRvsChannl;
    switch(CurState())
    {
    case STATE_ONGOING:

        tTWSwitchInfo = *(TTWSwitchInfo *)cServMsg.GetMsgBody();
        tSrcMt = tTWSwitchInfo.GetSrcMt();
        tDstMt = tTWSwitchInfo.GetDstMt();
        byTvWallId = tDstMt.GetEqpId();
        tSrcOrg = tSrcMt;
        byDstChnnlIdx = tTWSwitchInfo.GetDstChlIdx();

        //���Ͳ��Ի����ڱ�MCU��δ�Ǽ�
        if (EQP_TYPE_TVWALL != tDstMt.GetEqpType() ||
            !g_cMcuVcApp.IsPeriEqpConnected(byTvWallId) ||
            byDstChnnlIdx >= MAXNUM_PERIEQP_CHNNL)
        {
            ConfLog(FALSE, "CMcuVcInst: Wrong PeriEquipment type %u or it has not registered!\n", 
                            tDstMt.GetEqpType());
            cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
        
        if (tSrcMt.GetType() != TYPE_MCUPERI)
        {
			if (!tSrcMt.IsLocal())
			{
				tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt); 
			}
        }
		else
		{
			//ѡ����vmpΪ��ʱ                      
			if (m_tVmpEqp.IsNull())
			{
				ConfLog(FALSE, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: PeriEquipment type %u has not registered!\n", 
					tDstMt.GetEqpType());
				cServMsg.SetErrorCode(ERR_MCU_MCUPERI_NOTEXIST);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
			
			tSrcMt = m_tVmpEqp;
		}

        // δ�򿪺���ͨ�����������������б���
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
		if ( tSrcRvsChannl.GetChannelType() == MEDIA_TYPE_H264 &&
			( IsVidFormatHD( tSrcRvsChannl.GetVideoFormat() ) ||
			tSrcRvsChannl.GetVideoFormat() == VIDEO_FORMAT_4CIF ) )
		{
			// nack for none keda MT whose resolution is about  h264 4Cif 
			if ( MT_MANU_KDC != m_ptMtTable->GetManuId(tSrcMt.GetMtId()) )
			{
				ConfLog(FALSE, "[StartSwitchTWReq] Mt<%d> VidType.%d with format.%d support no tvwall\n",
					tSrcMt.GetMtId(), tSrcRvsChannl.GetChannelType(), tSrcRvsChannl.GetVideoFormat() );
				cServMsg.SetErrorCode(ERR_MT_MEDIACAPABILITY);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
			else
			{
				// force the mt to adjust its resolution to Cif
				// FIXME: cascade adjusting
				u8 byChnnlType = LOGCHL_VIDEO;
				u8 byNewFormat = VIDEO_FORMAT_CIF;
				CServMsg cMsg;
				cMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
				cMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
				cMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
				SendMsgToMt( tSrcMt.GetMtId(), cMsg.GetEventId(), cMsg );
				
				McsLog( "[StartSwitchTWReq]send videoformat<%d>(%s) change msg to mt<%d>!\n",
				 byNewFormat, GetResStr(byNewFormat), tSrcMt.GetMtId() );

			}
			
        }

        if (!tSrcOrg.IsLocal())
        {		
            OnMMcuSetIn(tSrcOrg, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
            tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);
        }

        // xsl [7/20/2006] ���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
        if (m_tConf.GetConfAttrb().IsSatDCastMode())
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
            
            if (IsOverSatCastChnnlNum(tSrcMt.GetMtId()))
            {
                ConfLog(FALSE, "[StartSwitchTWReq] over max upload mt num. nack!\n");
                cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
        }
	
        switch(tTWSwitchInfo.GetMemberType()) 
        {
        case TW_MEMBERTYPE_MCSSPEC:
		case TW_MEMBERTYPE_VCSAUTOSPEC:

            tSrcMt = m_ptMtTable->GetMt(tSrcMt.GetMtId());

            //Դ�ն˱���߱���Ƶ��������
            m_ptMtTable->GetMtStatus(tSrcMt.GetMtId(), &tMtStatus);
            if (FALSE == tMtStatus.IsSendVideo() &&
				m_tConf.GetConfSource() != VCS_CONF)
            {
                cServMsg.SetErrorCode(ERR_MCU_SRCISRECVONLY);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
                return;
            }	
			
            //Դ�ն˱����Ѿ��������
            if (!m_tConfAllMtInfo.MtJoinedConf(tSrcMt.GetMtId()) &&
				m_tConf.GetConfSource() != VCS_CONF)
            {
                log(LOGLVL_DEBUG1, "CMcuVcInst: select source MT%u-%u has not joined current conference! Error!\n",
                                    tSrcMt.GetMcuId(), tSrcMt.GetMtId());
                cServMsg.SetErrorCode(ERR_MCU_MT_NOTINCONF);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);	//nack
                return;
            }

            bNeedSwitchMt = TRUE;
            break;

        case TW_MEMBERTYPE_SPEAKER://ע��ı䷢����ʱ�ı佻��
			
            if (TRUE == HasJoinedSpeaker())
            {
                bNeedSwitchMt = TRUE;
                tSrcMt = GetLocalSpeaker();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;

        case TW_MEMBERTYPE_CHAIRMAN://ע��ı���ϯʱ�ı佻��

            if (TRUE == HasJoinedChairman())
            {
                bNeedSwitchMt = TRUE;
                tSrcMt = m_tConf.GetChairman();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;

        case TW_MEMBERTYPE_POLL://ע��ı���ѯ�㲥Դʱ�ı佻��

            if (CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode())
            {
                bNeedSwitchMt = TRUE;
                tSrcMt = (TMt)m_tConf.m_tStatus.GetMtPollParam();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;

        default:
            ConfLog(FALSE, "CMcuVcInst: Wrong TvWall Member Type!\n");
            break;
		}

        //send Ack reply
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
                
        if (bNeedSwitchMt)
        {
            ptSrcMt = &tSrcMt;
        }    

        ChangeTvWallSwitch(ptSrcMt, byTvWallId, byDstChnnlIdx, tTWSwitchInfo.GetMemberType(), TW_STATE_START);

        break;

    default:
        ConfLog(FALSE, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
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
	TLogicalChannel tLogicalChannel;

	tTWSwitchInfo = *( TTWSwitchInfo * )cServMsg.GetMsgBody();
	tEqp = tTWSwitchInfo.GetDstMt();

	//error, Nack
	if( tEqp.GetEqpType() != EQP_TYPE_TVWALL )
	{
		ConfLog( FALSE, "CMcSsnInst: Wrong PeriEquipment type %u or it has not registered!\n", 
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
    u8             byMtId;
    byTvWallId = tEqp.GetEqpId();
    byChnlNo   = tTWSwitchInfo.GetDstChlIdx();
    g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTWStatus);
    tRemoveMt = tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlNo];
    if (tRemoveMt.GetConfIdx() != m_byConfIdx)
    {
        return;
    }
    byMtId = tRemoveMt.GetMtId();

	StopSwitchToPeriEqp( tEqp.GetEqpId(), tTWSwitchInfo.GetDstChlIdx() );

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
        
    if( IsMtNotInOtherTvWallChnnl(byMtId, byTvWallId, byChnlNo) )  
    {
        m_ptMtTable->SetMtInTvWall(byMtId, FALSE);
        MtStatusChange(); 
		
		// xliang [7/13/2009] recover mt's resolution (take MPU into consideration)
		RecoverMtResInMpu( tRemoveMt );
    }
    
    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlNo].byMemberType = tTWSwitchInfo.GetMemberType();
    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlNo].SetNull();
    g_cMcuVcApp.SetPeriEqpStatus(byTvWallId, &tTWStatus);

    //���TvWallģ����Ӧ��Ա��Ϣ
    TMt tNullMt;
    tNullMt.SetNull();
    u8 byMtPos;
    u8 byMemberType = 0;
    BOOL32 bExist = m_tConfEqpModule.GetTvWallMemberByMt(byTvWallId, tRemoveMt, byMtPos, byMemberType);
    if( bExist )
    {
        m_tConfEqpModule.SetTvWallMemberInTvInfo(byTvWallId, byChnlNo, tNullMt);    // remove tv wall info
        m_tConfEqpModule.m_tMultiTvWallModule.RemoveMtByTvId(byTvWallId, byMtId);  // remove mutilTvWallmodule
    }

	
	
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
    TPeriEqpStatus tTWStatus;
    g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTWStatus);

    //�Ƿ��ڱ�����ǽ������ͨ����
    for(u8 byLp = 0; byLp < tTWStatus.m_tStatus.tTvWall.byChnnlNum; byLp++)
    {
        if(byLp != byChnlId)
        {
            TMt tMtInTv = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byLp];
			// �����ж��ն����ڻ���ţ�����Ὣ����������ն�����
			// zgc, 2008-01-25
            if(tMtInTv.GetMtId() == byMtId && tMtInTv.GetConfIdx() == m_byConfIdx)
            {
                return FALSE;
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
                TMt tMtInTv = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byLp];
				// �����ж��ն����ڻ���ţ�����Ὣ����������ն�����
				// zgc, 2008-01-25
                if(tMtInTv.GetMtId() == byMtId && tMtInTv.GetConfIdx() == m_byConfIdx)
                {
                    return FALSE;
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
        //OspPrintf(TRUE, FALSE, "[ProcTvwallConnectedNotif]CurState() = %d\n", CurState());
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

    tEqp.SetMcuEqp(tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());

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
        bySrcMtConfIdx = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
        if (m_byConfIdx == bySrcMtConfIdx)
        {
            byMemberType = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType;
            if (0 != byMemberType)
            {
                memset(&tTWSwitchInfo, 0, sizeof(TTWSwitchInfo));
                tTWSwitchInfo.SetMemberType(byMemberType);
                tTWSwitchInfo.SetSrcMt((TMt)tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop]);
                tTWSwitchInfo.SetDstMt(tEqp);
                tTWSwitchInfo.SetDstChlIdx(byLoop);

                cServMsg.SetMsgBody((u8 *)&tTWSwitchInfo, sizeof(TTWSwitchInfo));

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
        //OspPrintf(TRUE, FALSE, "[TvwallDisconnectedNotif]CurState() = %d\n", CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();

    TPeriEqpStatus tTvWallStatus;
    u8 bySrcMtConfIdx;
    u8 bySrcMtId;

    g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tTvWallStatus);
    u8 byMemberNum = tTvWallStatus.m_tStatus.tTvWall.byChnnlNum;
    for (u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
    {
        bySrcMtConfIdx = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
        if (m_byConfIdx == bySrcMtConfIdx)
        {
            bySrcMtId = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMtId();
            m_ptMtTable->SetMtInTvWall(bySrcMtId, FALSE);
            MtStatusChange();

            StopSwitchToPeriEqp(tEqp.GetEqpId(), byLoop);
        }
    }
    
    // guzh [5/16/2007] �������ǽ��ѯ
    if (POLL_STATE_NONE != m_tConf.m_tStatus.GetTvWallPollState())
    {
        TMtPollParam tMtPollParam;
        memset(&tMtPollParam, 0, sizeof(tMtPollParam));
        
        m_tConf.m_tStatus.SetTvWallPollState(POLL_STATE_NONE);
        m_tConf.m_tStatus.SetTvWallMtPollParam(tMtPollParam);
        cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo));
        SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);

        KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER);
    }

//    for(u8 byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++)
//    {
//        StopSwitchToPeriEqp(tEqp.GetEqpId(), byLoop);
//    }

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
void CMcuVcInst::ProcHduMcuStatusNotif( const CMessage * pcMsg )
{
//     CServMsg cServMsg(pcMsg->content, pcMsg->length);
//     TPeriEqpStatus *ptHduStatus = (TPeriEqpStatus*)cServMsg.GetMsgBody();
//     g_cMcuVcApp.SetPeriEqpStatus(ptHduStatus->GetEqpId(), ptHduStatus);
	
	return;
}

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
//     if (STATE_ONGOING != CurState())
//     {
//         return;
//     }
// 	
//     CServMsg cServMsg(pcMsg->content, pcMsg->length);
// 	
//     TPeriEqpRegReq tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();
// 	
//     TPeriEqpStatus tHduStatus;
// 	
//     THduSwitchInfo tHduSwitchInfo;
//     u8 byMemberType;
//     u8 bySrcMtConfIdx;
//     u8 byLoop;
//     TEqp tEqp;
// 	
//     tEqp.SetMcuEqp(tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());
// 	
//     g_cMcuVcApp.GetPeriEqpStatus(tRegReq.GetEqpId(), &tHduStatus);
// 
//     for (byLoop = 0; byLoop < MAXNUM_HDU_CHANNEL; byLoop++)
//     {
//         bySrcMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
//         if (m_byConfIdx == bySrcMtConfIdx)
//         {
// 		    byMemberType = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType;
//             if (0 != byMemberType)
//             {
//                 tHduSwitchInfo.SetMemberType(byMemberType);
//                 tHduSwitchInfo.SetSrcMt((TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop]);
//                 tHduSwitchInfo.SetDstMt(tEqp);
//                 tHduSwitchInfo.SetDstChlIdx(byLoop);
// 
// 				::OspPost(MAKEIID(AID_MCU_VC, GetInsID()), MCUVC_STARTSWITCHHDU_NOTIFY, 
// 					         (u8*)&tHduSwitchInfo, sizeof(tHduSwitchInfo));
//             }
//         }
//     }
// 	
//     return;
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

    TPeriEqpStatus tHduStatus;
    u8 bySrcMtConfIdx;
    u8 bySrcMtId;
	
    g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);
    u8 byMemberNum = tHduStatus.m_tStatus.tHdu.byChnnlNum;
    for (u8 byLoop = 0; byLoop < MAXNUM_HDU_CHANNEL; byLoop++)
    {
        bySrcMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
        if (m_byConfIdx == bySrcMtConfIdx)
        {
            bySrcMtId = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetMtId();
            m_ptMtTable->SetMtInHdu(bySrcMtId, FALSE);
            MtStatusChange();
			
            StopSwitchToPeriEqp(tEqp.GetEqpId(), byLoop);
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
		u8 byMtType = m_ptMtTable->GetMtType(byCreateMtId);
		if( MT_TYPE_MMCU == byMtType || MT_TYPE_SMCU == byMtType )
		{
			g_cMcuVcApp.DecMtAdpMtNum( m_ptMtTable->GetDriId(byCreateMtId), m_byConfIdx, byCreateMtId, FALSE,TRUE );
		}
    }

	// �ͷ�Prsͨ��
	if( EQP_CHANNO_INVALID != m_byPrsChnnl )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnl = EQP_CHANNO_INVALID;
	}

	if( EQP_CHANNO_INVALID != m_byPrsChnnl2 )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl2].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnl2 = EQP_CHANNO_INVALID;
	}
		
	if( EQP_CHANNO_INVALID != m_byPrsChnnlAud )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAud].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlAud = EQP_CHANNO_INVALID;
	}
	
	if( EQP_CHANNO_INVALID != m_byPrsChnnlVidBas )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVidBas].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlVidBas = EQP_CHANNO_INVALID;
	}

	if( EQP_CHANNO_INVALID != m_byPrsChnnlBrBas )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlBrBas].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlBrBas = EQP_CHANNO_INVALID;
	}
	
	if( EQP_CHANNO_INVALID != m_byPrsChnnlAudBas )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAudBas].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlAudBas = EQP_CHANNO_INVALID;
	}

	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut1 )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut1].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlVmpOut1 = EQP_CHANNO_INVALID;
	}

	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut2 )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut2].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlVmpOut2 = EQP_CHANNO_INVALID;
	}

	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut3 )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut3].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlVmpOut3 = EQP_CHANNO_INVALID;
	}

	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut4 )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut4].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlVmpOut4 = EQP_CHANNO_INVALID;
	}

	// �ͷ�Basͨ��
	if( EQP_CHANNO_INVALID != m_byAudBasChnnl )
	{				
        g_cMcuVcApp.SetBasChanReserved(m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl, FALSE);
        m_byAudBasChnnl = EQP_CHANNO_INVALID;
		memset( &m_tAudBasEqp, 0, sizeof(m_tAudBasEqp) );
	}

	if( EQP_CHANNO_INVALID != m_byVidBasChnnl )
	{
        g_cMcuVcApp.SetBasChanReserved(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, FALSE);	
		m_byVidBasChnnl = EQP_CHANNO_INVALID;
		memset( &m_tVidBasEqp, 0, sizeof(m_tVidBasEqp) );
	}

	if( EQP_CHANNO_INVALID != m_byBrBasChnnl )
	{
        g_cMcuVcApp.SetBasChanReserved(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, FALSE);			
        m_byBrBasChnnl = EQP_CHANNO_INVALID;
		memset( &m_tBrBasEqp, 0, sizeof(m_tBrBasEqp) );
	}

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
	
    CreateConfSendMcsNack( byOldInsId, nErrCode, cServMsg, TRUE );

    return;
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
    if ( bDeleteAlias )
    {
        DeleteAlias();
    }    
    cServMsg.SetErrorCode( nErrCode );
    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
    g_cMcuVcApp.SetConfMapInsId(m_byConfIdx, byOldInsId);

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
        ConfLog(FALSE, "[AddSecDSCapByDebug] add dual ds cap<h263+, XGA, 5fps>\n");
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
        McsLog("Time to Start Auto Record...\n");
        
        // ����Ѿ���¼������Ǳ���Ϣ��
        if ( m_tConf.m_tStatus.IsNoRecording() )
        {    
            // ���¼�������
            if ( g_cMcuVcApp.IsRecorderOnline(tAttrib.GetRecEqp()) )
            {
                // ����һ����ʼ¼����Ϣ
                CServMsg cServMsg;
                TMt tMt;
                tMt.SetNull();
                cServMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));
                TEqp tRec;
                tRec.SetMcuEqp( LOCAL_MCUID, tAttrib.GetRecEqp(), EQP_TYPE_RECORDER );
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
                    u16 wBitrate = m_tConf.GetBitRate() + GetAudioBitrate( m_tConf.GetMainAudioMediaType() );
                    sprintf(aszRecName, "%s-%04d-%02d-%02d %02d-%02d-%02d_%d_%dK",
                            m_tConf.GetConfName(),
                            tKdvTime.GetYear(),
                            tKdvTime.GetMonth(),
                            tKdvTime.GetDay(), 
                            tKdvTime.GetHour(),
                            tKdvTime.GetMinute(),
                            tKdvTime.GetSecond(),
                            m_tConf.GetUsrGrpId(),
                            wBitrate );
//                }
//                else
//                {
//                    strncpy(aszRecName, tAttrib.GetRecName(), KDV_MAX_PATH - 1);
//                }
                cServMsg.CatMsgBody((u8*)aszRecName, strlen(aszRecName)+1);

                cServMsg.SetEventId(MCS_MCU_STARTREC_REQ);
                cServMsg.SetConfIdx(m_byConfIdx);
                cServMsg.SetConfId( m_tConf.GetConfId() );
                cServMsg.SetNoSrc();
    
                g_cMcuVcApp.SendMsgToConf(m_byConfIdx, 
                                          MCS_MCU_STARTREC_REQ, 
                                          cServMsg.GetServMsg(),
                                          cServMsg.GetServMsgLen());
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
        McsLog("Time to End Auto Record...\n");

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
	u8 byLoop = 0;

	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byConfIdx = *(u8*)cServMsg.GetMsgBody();
	TTemplateInfo  tTemInfo;

	if (g_cMcuVcApp.GetConfMapData(byConfIdx).IsValidConf())
	{
		//get tConfInfo
		if (NULL == g_cMcuVcApp.GetConfInstHandle(byConfIdx))
		{
			ConfLog( FALSE, "[ProcMcsMcuSaveConfToTemplateReq]: GetConfInstHandle NULL! -- %d\n", m_byConfIdx);
			return;
		}
		CMcuVcInst* pcInstance = g_cMcuVcApp.GetConfInstHandle(byConfIdx);

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
        tTemInfo.m_tConfInfo.m_tStatus.SetTvWallPollState(POLL_STATE_NONE);
		tTemInfo.m_tConfInfo.m_tStatus.SetTakeMode(CONF_TAKEMODE_TEMPLATE);

//		tTemInfo.m_tMultiTvWallModule = pcInstance->m_tConfEqpModule.m_tMultiTvWallModule; 
//		tTemInfo.m_atVmpModule = pcInstance->m_tConfEqpModule.GetVmpModule();
		tTemInfo.m_tMultiTvWallModule.Clear();
		tTemInfo.m_atVmpModule.EmptyMember();
		TConfAttrb tConfAttrib = tTemInfo.m_tConfInfo.GetConfAttrb();
		tConfAttrib.SetHasTvWallModule( FALSE );
		tConfAttrib.SetHasVmpModule( FALSE );
		tTemInfo.m_tConfInfo.SetConfAttrb( tConfAttrib );
		tTemInfo.m_byMtNum = pcInstance->m_ptMtTable->m_byMaxNumInUse;
            
        tTemInfo.EmptyAllTvMember();
        tTemInfo.EmptyAllVmpMember();

		//get alias array
        u8 byMemberType = 0;
		TMt tMt;
		TMtAlias tMtAlias;
		for (u8 byLoop = 0; byLoop < pcInstance->m_ptMtTable->m_byMaxNumInUse; byLoop++)
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
				if (tTemInfo.m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
				{
					for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL; byTvLp++)
                    {                            
                        u8 byTvId = pcInstance->m_tConfEqpModule.m_tTvWallInfo[byTvLp].m_tTvWallEqp.GetEqpId();
                        for(u8 byTvChlLp = 0; byTvChlLp < MAXNUM_PERIEQP_CHNNL; byTvChlLp++)
                        {
                            if(TRUE ==pcInstance-> m_tConfEqpModule.IsMtInTvWallChannel(byTvId, byTvChlLp, tMt, byMemberType) )
                            {
                                tTemInfo.SetMtInTvChannel(byTvId, byTvChlLp, byLoop+1, byMemberType);
                            }
                        }
                    }
				}
				if (tTemInfo.m_tConfInfo.GetConfAttrb().IsHasVmpModule())
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
            ConfLog( FALSE, "[ProcMcsMcuSaveConfToTemplateReq]Modify template %s failed!\n", tTemInfo.m_tConfInfo.GetConfName() );
            return;
		}
		
		SendReplyBack(cServMsg, pcMsg->event+1);
        g_cMcuVcApp.TemInfo2Msg(tTemInfo, cServMsg);
        SendMsgToAllMcs(MCU_MCS_TEMSCHCONFINFO_NOTIF, cServMsg); 
	}
	else
	{
		//�����E164�����Ѵ��ڣ��ܾ� 
		if( g_cMcuVcApp.IsConfE164Repeat( tTemInfo.m_tConfInfo.GetConfE164(), TRUE ) )
        {
            cServMsg.SetErrorCode( ERR_MCU_CONFE164_REPEAT );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            ConfLog( FALSE, "[ProcMcsMcuSaveConfToTemplateReq]template %s E164 repeated and create failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
            return;
        }
		//�������Ѵ��ڣ��ܾ�
        if( g_cMcuVcApp.IsConfNameRepeat( tTemInfo.m_tConfInfo.GetConfName(), TRUE ) )
        {
            cServMsg.SetErrorCode( ERR_MCU_CONFNAME_REPEAT );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            ConfLog( FALSE, "[ProcMcsMcuSaveConfToTemplateReq]template %s name repeated and create failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
            return;
        } 
	
		if(!g_cMcuVcApp.AddTemplate(tTemInfo))
        {
			// ���صĴ�����
			cServMsg.SetErrorCode( ERR_MCU_TEMPLATE_NOFREEROOM );
			ConfLog(FALSE, "[ProcMcsMcuSaveConfToTemplateReq] add template %s failed\n", tTemInfo.m_tConfInfo.GetConfName());
			SendReplyBack(cServMsg, pcMsg->event+2);
			return;
         }                 
         cServMsg.SetConfId( tTemInfo.m_tConfInfo.GetConfId() );
		 SendReplyBack(cServMsg, pcMsg->event+1);
         g_cMcuVcApp.TemInfo2Msg(tTemInfo, cServMsg);
         SendMsgToAllMcs(MCU_MCS_TEMSCHCONFINFO_NOTIF, cServMsg); 
	}
	
	McsLog( "Save conf %s to template successfully!\n", tTemInfo.m_tConfInfo.GetConfName() );
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
	
	u8 bySeizeMtId = tSeizeMt.GetMtId();			//FIXME:������ռ

	TVMPParam  tVMPParam   = m_tConf.m_tStatus.GetVmpParam();
	
	if (MCS_MCU_VMPPRISEIZE_ACK == pcMsg->event)
	{
		u8 bySeizedMtNum = (cServMsg.GetMsgBodyLen()-sizeof(TMt)) / sizeof(TSeizeChoice);//����ռ��MT��Ŀ
		TSeizeChoice tSeizeChoice[MAXNUM_SVMPB_HDCHNNL];
		u8 byMtIdx=0;
		CallLog("Mt.%u Seize Mt choice info:\n==================\n",bySeizeMtId);
		for(byMtIdx=0; byMtIdx<bySeizedMtNum; byMtIdx++)
		{
			tSeizeChoice[byMtIdx] = *(TSeizeChoice *)(cServMsg.GetMsgBody()+sizeof(TMt)+sizeof(TSeizeChoice)*byMtIdx);
			CallLog("Mt.%u: byStopVmp(%u), byStopSpeaker(%u), byStopSelected(%u)\n",
				tSeizeChoice[byMtIdx].tSeizedMt.GetMtId(), tSeizeChoice[byMtIdx].byStopVmp,
				tSeizeChoice[byMtIdx].byStopSpeaker, tSeizeChoice[byMtIdx].byStopSelected);
		}

		CServMsg cMsg;
		
		for(byMtIdx=0; byMtIdx<bySeizedMtNum; byMtIdx++)
		{
			//���ݱ���ռѡ����в���
			//1,���ն��˳�����ϳ�
			if(tSeizeChoice[byMtIdx].byStopVmp == 1)
			{
				// xliang [3/25/2009] ���Ը��ģ��ֲ���Ա������������ȫ������
				TMt tMtBeSeized = tSeizeChoice[byMtIdx].tSeizedMt;
				u8	byChl = tVMPParam.GetChlOfMtInMember(tMtBeSeized);
				TMt tMt;
				TVMPMember tVMPMember;
				tMt.SetNull();
				tVMPMember.SetMemberTMt(tMt);
				tVMPMember.SetMemberType(0); //����ԭ���Ƿ����˸��滹������������0
				tVMPParam.SetVmpMember(byChl,tVMPMember);
				m_tConf.m_tStatus.SetVmpParam(tVMPParam);
				memcpy(&m_tLastVmpParam, &tVMPParam, sizeof(TVMPParam));
				
				cServMsg.SetMsgBody((u8*)&tVMPParam,sizeof(tVMPParam));
				SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );	//���߻��ˢVMP����

				// xliang [4/10/2009] ͬʱ����tPeriEqpStatus��mcsҪ���ݴ�ˢMTͼ��,����������TMtStatus :(
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
				g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				cServMsg.SetMsgBody();
				cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );

				//�彻����
				StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byChl, FALSE, MODE_VIDEO);
				m_tVmpChnnlInfo.ClearOneChnnl(tMtBeSeized); //���Mt��Ӧ��HDͨ��(������ڶ�Ӧͨ��)
				
				/*bNeedSeize = FALSE;
				
				TVMPMember tVMPMember;
				
				u8         byMemberNum = tVMPParam.GetMaxMemberNum();
				TMt tMtBeSeized = m_ptMtTable->GetMt(tSeizeChoice[byMtIdx].byMtId);
				
				m_tVmpChnnlInfo.ClearOneChnnl(tMtBeSeized.GetMtId()); //���Mt��Ӧ��HDͨ��(������ڶ�Ӧͨ��)

				if(IsDynamicVmp()) //��ǰ���Զ�����ϳ�ģʽ,���ﲻ���ж�vmpStyle��Ҫ�ж�vmpMode
				{
					// xliang [12/17/2008] �Զ�����ϳ�ʱ��vmpParam��û�г�Ա��Ϣ�ģ�
					// ����û��Ҫ��tVMPMember��Ҳû����ֵ
					// ����Ѹ��ն�ID���뵽VMP��Ա�����б������Ͳ����ٿ��Ǹ��ն���
					AddToVmpNeglected(tSeizeChoice[byMtIdx].byMtId);
				}
				else
				{
					u8	byChl = tVMPParam.GetChlOfMtInMember(tMtBeSeized);
					TMt tMt;
					tMt.SetNull();
					tVMPMember.SetMemberTMt(tMt);
					tVMPMember.SetMemberType(0); //����ԭ���Ƿ����˸��滹������������0
					tVMPParam.SetVmpMember(byChl,tVMPMember);
					//tVMPParam.Print();
				}

				cMsg.SetNoSrc();
				cMsg.SetEventId(MCS_MCU_CHANGEVMPPARAM_REQ);
				cMsg.SetMsgBody((u8 *)&tVMPParam,sizeof(tVMPParam));
				ProcChangeVmpParamReq(cMsg);
				CallLog("[ProcMcsMcuVmpPriSeizeRsp] Start Change Vmp Param!\n");

				*/
			}
			//2,���ն�ȡ��������
			if(tSeizeChoice[byMtIdx].byStopSpeaker == 1)
			{
				BOOL32 bCancelSpeaker = TRUE; //�ܷ�ȡ��������

				//�����ݽ�ģʽ�������������Ʒ���״̬,����ָ��������
				if (m_tConf.m_tStatus.IsVACing())
				{
					ConfLog( FALSE,"[ProcMcsMcuVmpPriSeizeRsp] Conf is VACing! Speaker cannot be canceled!\n");
					bCancelSpeaker = FALSE;
				}

				TMt tSpeaker = m_tConf.GetSpeaker();
				if( !m_tConf.HasSpeaker() )//û�з�����
				{
					ConfLog( FALSE, "No speaker in conference %s now! Cannot cancel the speaker!\n", 
						m_tConf.GetConfName() );
					bCancelSpeaker = FALSE;
				}
				if(bCancelSpeaker)
				{
					//ͣ�鲥
					if( m_ptMtTable->IsMtMulticasting( tSpeaker.GetMtId() ) )
					{
						g_cMpManager.StopMulticast( tSpeaker, 0, MODE_BOTH );
						m_ptMtTable->SetMtMulticasting( tSpeaker.GetMtId(), FALSE );
					}
					
					//ȡ��������
					CallLog("[ProcMcsMcuVmpPriSeizeRsp]begin change speaker!\n");
					ChangeSpeaker( NULL );
				}

// 				cMsg.SetNoSrc();
// 				cMsg.SetEventId(MCS_MCU_CANCELSPEAKER_REQ);	//ȡ��������
// 				cMsg.SetMsgBody();
// 				g_cMcuVcApp.SendMsgToConf(cServMsg.GetConfId(),cMsg.GetEventId(),
// 							cMsg.GetServMsg(),cMsg.GetServMsgLen());
// 
// 				CallLog("[ProcMcsMcuVmpPriSeizeRsp] Send msg MCS_MCU_CANCELSPEAKER_REQ to conf.\
// 					The seized Mt.%u!\n",tSeizeChoice[byMtIdx].byMtId);

			}

			//3,���ն�ȡ����ѡ��
			if(tSeizeChoice[byMtIdx].byStopSelected == 1)
			{
				TMtStatus	tMtStatus;
				TMt			tSrc;
				TMt			tDstMt;
				BOOL32		bSelectedSrc = FALSE; //�Ƿ����ڱ�ѡ��
				//ȡ����������ѡ����ѡ��ԴMT�Ľ���
				for(u8 byLoop=1; byLoop<=MAXNUM_CONF_MT;byLoop++ )
				{
					if(!m_tConfAllMtInfo.MtJoinedConf( byLoop ))
					{
						continue;
					}
					m_ptMtTable->GetMtStatus(byLoop,&tMtStatus);
					//tSrc = tMtStatus.GetVideoMt();// xliang [1/9/2009] ��Ȼ����ԣ����������ѡ��Դ���õ���SetSelectMt���������¾�
					tSrc = tMtStatus.GetSelectMt( MODE_VIDEO ); //��ȡѡ��Դ
					if(tSrc == tSeizeChoice[byMtIdx].tSeizedMt)//��MT��ѡ��Դ�Ǳ���ռ��MT
					{
						//ȡ��ѡ��
						bSelectedSrc = TRUE;
						tDstMt = m_ptMtTable->GetMt(byLoop);
// 						cMsg.SetNoSrc();
// 						cMsg.SetEventId(MCS_MCU_STOPSWITCHMT_REQ);
// 						cMsg.SetMsgBody((u8 *)&tDstMt,sizeof(tDstMt));
// 						g_cMcuVcApp.SendMsgToConf(cServMsg.GetConfId(),cMsg.GetEventId(),
// 							cMsg.GetServMsg(),cMsg.GetServMsgLen());
// 						
// 						CallLog("[ProcMcsMcuVmpPriSeizeRsp] Send msg MCS_MCU_STOPSWITCHMT_REQ to conf.\
// 							Cancel Mt.%u selectseeing Mt.%u.\n",byLoop,tSeizeChoice[byMtIdx].byMtId);
						CallLog("[ProcMcsMcuVmpPriSeizeRsp]Cancel Mt.%u selectseeing Mt.%u.\n",
							byLoop,tSeizeChoice[byMtIdx].tSeizedMt.GetMtId());
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
					CallLog("[ProcMcsMcuVmpPriSeizeRsp]Mt.%u is not the selected source!\n",tSeizeChoice[byMtIdx].tSeizedMt.GetMtId());
				}
			}
		}
		if(bNeedSeize)
		{
			//��ԭ��Ҫ��ռ��Mt���в����������ʱ����ʱ������
			u16 wTimer = 1000;		//FIXME��1�����̫��
			//������¼�һ����Ա��������ռ�������
			//1,vmp param����: ��֮ǰδ����ϳɳ�Ա��MT�������
			//2,����change param��ACK��MCS
			TMt tTmpTmt = m_ptMtTable->GetMt(bySeizeMtId);

			if ( !tVMPParam.IsMtInMember(tTmpTmt) ) 
			{	
				u8 byIndex = 0;
				for(;byIndex < MAXNUM_MPUSVMP_MEMBER; byIndex++)
				{
					if(m_atVMPTmpMember[byIndex].GetMtId() == bySeizeMtId)
					{
						break;
					}
				}
				TVMPMember tVMPMember = m_atVMPTmpMember[byIndex];
				//tVMPMember.SetMemberTMt(tTmpTmt);
				//tVMPMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);	//FIXME: membertype ���������⣬��ѯ������ô��
				tVMPParam.SetVmpMember(byIndex,tVMPMember);
				m_tConf.m_tStatus.SetVmpParam(tVMPParam);
				memcpy(&m_tLastVmpParam, &tVMPParam, sizeof(TVMPParam));

				cServMsg.SetMsgBody((u8*)&tVMPParam,sizeof(tVMPParam));
				
				m_bNewMemberSeize = 1;	//��Ǵ˴���ռ��������һ����Ա��������ռ��֮��Ҫ��������
				SendReplyBack(cServMsg, MCU_MCS_CHANGEVMPPARAM_ACK);	//����ack

				SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );	//���߻��ˢ����

				// xliang [4/10/2009] ͬʱ����tPeriEqpStatus��mcsҪ���ݴ�ˢMTͼ��,����������TMtStatus :(
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
				g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				cServMsg.SetMsgBody();
				cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );
			}
			
			SetTimer(MCUVC_MTSEIZEVMP_TIMER, wTimer, (u32)bySeizeMtId*100);
		}
		else
		{
			ConfLog(FALSE,"[ProcMcsMcuVmpPriSeizeRsp] no need to seize!\n");
		}
	}
	else // xliang [4/21/2009] Ŀǰ������β�ִ��
	{	
		//TMt tTmpTmt = m_ptMtTable->GetMt(bySeizeMtId);
		if ( tVMPParam.IsMtInMember(tSeizeMt) ) 
		{
			//����vip��ݵ�MT�ӻ���ϳɳ�Ա���޳�
			u8	byChl = tVMPParam.GetChlOfMtInMember(tSeizeMt);
			TMt tMt;
			TVMPMember tVMPMember;
			tMt.SetNull();
			tVMPMember.SetMemberTMt(tMt);
			tVMPMember.SetMemberType(0); //����ԭ���Ƿ����˸��滹������������0
			tVMPParam.SetVmpMember(byChl,tVMPMember);
			//tVMPParam.Print();
			CServMsg cMsg;
			cMsg.SetNoSrc();
			cMsg.SetEventId(MCS_MCU_CHANGEVMPPARAM_REQ);
			cMsg.SetMsgBody((u8 *)&tVMPParam,sizeof(tVMPParam));
			ProcChangeVmpParamReq(cMsg);

		}

		// xliang [12/17/2008] ������Ҫ��ռ���ն�ID���뵽VMP��Ա�����б�
		//�����Զ�����ϳ�ģʽ�Ͳ����ٿ��Ǹ��ն���
		AddToVmpNeglected(bySeizeMtId);
		ConfLog(FALSE,"receive MCS_MCU_VMPPRISEIZE_NACK!\n");
	}
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
	u8 bySeizeMtId  = (u8)((*((u32*)pcMsg->content))/100);		//֮ǰҪ��ռ���ն�id
	u8 bySeizeTimes  = (u8)((*((u32*)pcMsg->content))%100)+1;	//��ռ����

	BOOL32 bNeedSeizePromt = FALSE;	//�Ƿ���Ҫ���·���ռ��ʾ
	if(bySeizeTimes > 3)
	{
		bNeedSeizePromt = TRUE;
	}
	//��ȡ֮ǰҪ��ռ���ն�vmpMemberType
	TVMPMember tVMPMember;
	TVMPParam  tVMPParam   = m_tConf.m_tStatus.GetVmpParam();
	u8         byMemberNum = tVMPParam.GetMaxMemberNum();
	u8	       byMemberType = ~0;
	for (u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
	{
		
		//ȡ����ϳɳ�Ա
		tVMPMember = *(tVMPParam.GetVmpMember(byLoop));
		if( tVMPMember.GetMtId() == bySeizeMtId)
		{
			byMemberType = tVMPMember.GetMemberType();
			break;
		}
	}

	if(byMemberType == 0) //����ڳ�ʱ����֮ǰ����������ϳɱ�ֹͣ�ˣ���membertypeӦΪ0
	{
		ConfLog(FALSE,"vmp has been stopped,no need to change Mt.%u's format!\n",bySeizeMtId);
		return;
	}

	//ȡVMPstyle, channel idx
	u8 byVmpStyle = tVMPParam.GetVMPStyle();
	TMt tSeizeMt = m_ptMtTable->GetMt(bySeizeMtId);
	u8 byChlPos	= tVMPParam.GetChlOfMtInMember(tSeizeMt);

	//�����ֱ���
	BOOL32 bRet = ChangeMtVideoFormat(tSeizeMt, &tVMPParam, TRUE, bNeedSeizePromt, FALSE);
	
	if(!m_bNewMemberSeize)//֮ǰ�Ѿ���VMP�У��ҽ��˽���.(���ĳ��Ա��Ϊ�����˵�������ݵ�����ռ�����)
	{
		//do nothing
	}
	else 
	{
		if(bRet)
		{
			//����ͨ����������
			SetVmpChnnl((TMt)tVMPMember, byChlPos, byMemberType);
			CallLog("[ProcMtSeizeVmpTimer] Mt.%u Set Vmp channel.%u!\n",bySeizeMtId, byChlPos);
		}
		else if(!bNeedSeizePromt) //�����Ҫ�ٴη���ռ��ʾ������Ͳ���Ҫ��ʱ��
		{	
			//������ʱ
			SetTimer(MCUVC_MTSEIZEVMP_TIMER, 1000, (u32)bySeizeMtId*100+bySeizeTimes);	       
		}
	}
	
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
	TVMPParam	tVmpParam = *( TVMPParam* )cServMsg.GetMsgBody();
	TPeriEqpStatus tPeriEqpStatus;
	u8 byVmpSubType = 0;			//VMP������
	u8 byLoop		= 0;			
	u8 byVmpId		= 0;

	if(g_bPrintEqpMsg)
	{
		EqpLog("[VmpCommonReq] start vmp param:\n");
		tVmpParam.Print();
	}
	
	//��鵱ǰ����Ļ���ϳ�״̬
	if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE ||
		//�������ȴ�����ϳ�����Ӧʱ�����ٴη���������ϳ�����,����ռ���»���ϳ���
		//��ԭ�ϳ�����Դй©
		(!m_tVmpEqp.IsNull() &&
		g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tPeriEqpStatus) &&
		tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
		TVmpStatus::WAIT_START == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState))
	{
		cServMsg.SetErrorCode( ERR_MCU_VMPRESTART );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );					
		return;
	}
	
	//�ϳɲ���У��
	u16 wErrCode = 0;
	if ( !CheckVmpParam( tVmpParam, wErrCode ) )
	{
		cServMsg.SetErrorCode( wErrCode );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//ѡVMP
	u8 byIdleVMPNum = 0;
    u8 abyIdleVMPId[MAXNUM_PERIEQP];
    memset( &abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
    g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );
	if( byIdleVMPNum == 0 )
	{
		cServMsg.SetErrorCode( ERR_MCU_NOIDLEVMP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		ConfLog(FALSE,"[VmpCommonReq]No Idle VMP!\n"); 
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
	}
	
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
	u8 byVmpDynStyle;
	if (tVmpParam.IsVMPAuto() )
	{
		//�Զ�VMP����
		// ��ǰ��ֵ,Ŀǰ��Ϊ����GetVmpDynStyle��ʹ��
		m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
		m_tVmpEqp.SetConfIdx( m_byConfIdx );
		byVmpDynStyle = GetVmpDynStyle(m_tConfAllMtInfo.GetLocalJoinedMtNum());
		m_tVmpEqp.SetNull();
		if (VMP_STYLE_NONE == byVmpDynStyle)
		{
			cServMsg.SetErrorCode( ERR_INVALID_VMPSTYLE );						
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
		else
		{
			// ������Զ�����ľ���ϳɷ�ʽ	
			McsLog("auto vmp style is confirmed to style:%u\n", byVmpDynStyle); 
			tVmpParam.SetVMPStyle( byVmpDynStyle );
		}
	}
	else if (byVmpSubType != VMP)
	{
		// ������MPU��ͨ����Ա���ƣ����Զ�ģʽ�£�
		if ( !CheckMpuMember( tVmpParam, wErrCode ) )
		{
			cServMsg.SetErrorCode( wErrCode );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
	}
	
	// xsl [7/20/2006]���Ƿ�ɢ������Ҫ���ش�ͨ����
	if (m_tConf.GetConfAttrb().IsSatDCastMode()) 
	{
		if (IsOverSatCastChnnlNum(tVmpParam))
		{
			ConfLog(FALSE, "[VmpCommonReq-start] over max upload mt num. nack!\n");
			cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return; 
		}
	}
	
	m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	m_tVmpEqp.SetConfIdx( m_byConfIdx );
	tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_START;//��ռ��,��ʱ��δ�ɹ��ٷ���
	tPeriEqpStatus.SetConfIdx( m_byConfIdx );
	
	m_tConf.m_tStatus.SetVmpStyle( tVmpParam.GetVMPStyle() );
	
	//ACK
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );				
	
	/**************************����Ϊ����nack�Ĺ���***********************/
	//�����и���ǰ����ͨ����VMP��
	//�ڿ�ʼ֮ǰ����vmp param�Ƿ��г���VMPǰ����ͨ������,�������߳���Ա�������岻�ܣ��Զ�����ϳɳ��⣩
	//VIP����У����������ȼ���ߡ�
	CheckAdpChnnlLmt(byVmpId, tVmpParam, cServMsg);
	
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpParam;	//tVmpParam���ո�PeriEqpStatus
	g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );

	m_byVmpOperating = 1;	//��־����VMPԭ�Ӳ���
	
	//��ʼ����ϳ�
	AdjustVmpParam(&tVmpParam, TRUE);
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
	u8 byVmpDynStle;
	u8 byLoop = 0;
    TVMPParam tVmpParam;
    memset( &tVmpParam, 0, sizeof(TVMPParam) );
	TPeriEqpStatus tPeriEqpStatus;
	/*TMt tMt;*/
	u8 byVmpSubType;
	u8 byMpuBoardVer = ~0;
	u16 wError = 0;
	tVmpParam = *( TVMPParam* )cServMsg.GetMsgBody();
	
	if(g_bPrintEqpMsg)
	{
		EqpLog("[VmpCommonReq] %s change vmp param:\n", OspEventDesc(cServMsg.GetEventId()));
		tVmpParam.Print();
	}
	
	//��鵱ǰ����Ļ���ϳ�״̬ 
	if( m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_NONE )
	{
		cServMsg.SetErrorCode( ERR_MCU_VMPNOTSTART );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//�Զ�����ϳ���������
	byVmpDynStle = GetVmpDynStyle( m_tConfAllMtInfo.GetLocalJoinedMtNum() );
	if ((tVmpParam.GetVMPStyle() == VMP_STYLE_DYNAMIC) && tVmpParam.IsVMPAuto() &&
		VMP_STYLE_NONE == byVmpDynStle)
	{
		cServMsg.SetErrorCode( ERR_INVALID_VMPSTYLE );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	// ���� [5/22/2006] change��ʱ��ҲҪ����Ƿ�֧��
	if ( !IsVMPSupportVmpStyle(tVmpParam.GetVMPStyle(),m_tVmpEqp.GetEqpId(), wError) )
	{
		cServMsg.SetErrorCode( wError  );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		
		// �л�ʧ�ܣ��������� [12/27/2006-zbq]
		// FIXME: ������̫�����ʣ��д��޸Ļ�ɾȥ����ʱ����
		u8 byVmpId = 0;
		if ( IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError) )
		{
			if ( cServMsg.GetEventId() == MCS_MCU_CHANGEVMPPARAM_REQ ) 
			{
				NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_OTHERVMP_SUPPORT );
			}
			ConfLog( FALSE, "[VmpCommonReq] curr VMP.%d can't support the style needed, other VMP.%d could\n",
				m_tVmpEqp.GetEqpId(), byVmpId );
		}
		return;
	}    
	
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
	byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType; 	
	
	//���û���ϳɲ���
	if ( !CheckVmpParam( tVmpParam, wError ) )
	{
		cServMsg.SetErrorCode( wError );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
	}
	
	// xliang [3/5/2009]  �����в�����ͬһ��MTռ�˶����Աͨ��
	// xliang [3/24/2009] �����в�������ͨ��ͬʱ���ó�Ϊ�����˸���ͨ��������ѯ����ͨ��
	//BOOL bConfHD = CMcuPfmLmt::IsConfFormatHD(m_tConf);
	/*if(bConfHD)*/
	if (byVmpSubType != VMP)
	{
		if (!CheckMpuMember(tVmpParam, wError, TRUE))
		{
			cServMsg.SetErrorCode( wError );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
	}

	// xsl [7/20/2006]���Ƿ�ɢ������Ҫ���ش�ͨ����
	if (m_tConf.GetConfAttrb().IsSatDCastMode()) 
	{                
		if (IsOverSatCastChnnlNum(tVmpParam))
		{
			ConfLog(FALSE, "[VmpCommonReq-change] over max upload mt num. nack!\n");
			cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
	}
	
	if ((tVmpParam.GetVMPStyle() == VMP_STYLE_DYNAMIC) && tVmpParam.IsVMPAuto())
	{                
		tVmpParam.SetVMPStyle( byVmpDynStle );
	}

	//��ռ��ʾ����
	CheckAdpChnnlLmt(m_tVmpEqp.GetEqpId(), tVmpParam, cServMsg, TRUE);

	//ACK
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
	m_byVmpOperating = 1;
	
	// xliang [3/20/2009] �����˸����²���
	if(m_tLastVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) 
		&& !tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) 
		)//ĳͨ��ȡ���˷����˸���
	{
		m_tLastVmpChnnlMemInfo.Init();
	}
	else if(!m_tLastVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) 
		&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) 
		)//ĳͨ������˷����˸���
	{
		if( m_tConf.HasSpeaker()
			&& tVmpParam.IsMtInMember( GetLocalSpeaker()/*m_tConf.GetSpeaker()*/ )  
			)
		{
			TMt tSpeaker = m_tConf.GetSpeaker();
			//ChangeVmpChannelParam( &tSpeaker, VMP_MEMBERTYPE_SPEAKER );
			AdjustVmpParambyMemberType(&tSpeaker, tVmpParam);
		}
		else
		{
			//Do nothing
		}
	}
	else //�޷����˸�����ز���
	{
		//Do nothing
	}
	
	// xliang [4/10/2009] ������ѯ���� ��������
	if(!m_tLastVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL) 
		&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
		)
	{
		//��ǰ������ѯ��MT���ڻ���ϳ��У���member��գ�����member type
		TMt tPollMt = m_tConf.m_tStatus.GetMtPollParam().GetTMt();
		if(tVmpParam.IsMtInMember(tPollMt))
		{
			u8 byChnnl = tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_POLL);
			TVMPMember tVmpMember = *tVmpParam.GetVmpMember( byChnnl );
			TMt tNullMt;
			tNullMt.SetNull();
			tVmpMember.SetMemberTMt(tNullMt);
			tVmpParam.SetVmpMember( byChnnl, tVmpMember );
		}

	}
	
	// �������
	// xliang [4/14/2009] tVmpParam����ȫ,������ز�����ϯѡ����������ѯ״̬�ֶ�
	tVmpParam.SetVMPSeeByChairman( m_tConf.m_tStatus.GetVmpParam().IsVMPSeeByChairman() );
	tVmpParam.SetVMPBatchPoll( m_tConf.m_tStatus.GetVmpParam().IsVMPBatchPoll() );
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpParam;	
	g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );

	// xliang [1/6/2009] ��������VMP
	AdjustVmpParam(&tVmpParam);

	return;
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
void CMcuVcInst::ProcVmpBatchPollTimer(const CMessage *pcMsg)
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
				
				CallLog("[ProcVmpBatchPollTimer]Batch Poll times is full, stop VMP!\n");
				
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
				CallLog("[ProcVmpBatchPollTimer]invalid VMP channel!\n");
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

	CallLog("[ProcVmpBatchPollTimer]VMP Batch Poll: CurPolledMtNum is %u,MtId is %u,MemberId is %u,Left Poll times is%u\n",\
		byCurPolledMtNum, byMtId, byMemberId, dwPollTimes);
	
	cMsg.SetEventId(MCS_MCU_CHANGEVMPPARAM_REQ);
	cMsg.SetMsgBody((u8 *)&tVMPParam,sizeof(tVMPParam));
	g_cMcuVcApp.SendMsgToConf(cMsg.GetConfId(),cMsg.GetEventId(),
		cMsg.GetServMsg(),cMsg.GetServMsgLen());
	
	CallLog("[ProcVmpBatchPollTimer] Send msg MCS_MCU_CHANGEVMPPARAM_REQ to conf.\n");

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
			for ( byMemberId; byMemberId < tVMPParam.GetMaxMemberNum(); byMemberId ++)
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
			
			CallLog("[ProcMcsMcuVmpCmd]VMP Batch Poll stops!\n");
			break;
		}
	default:
		ConfLog(FALSE,"[ProcMcsMcuVmpCmd]Unexpected Message event is received!\n");
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
void CMcuVcInst::ClearOneVmpMember(u8 byChnnl, TVMPParam &tVmpParam, BOOL32 bRecover)
{
	tVmpParam.ClearVmpMember(byChnnl);
	TVMPMember *ptLastVmpMember = m_tLastVmpParam.GetVmpMember(byChnnl);
	if(!ptLastVmpMember->IsNull())
	{
		if(bRecover)
		{
			//�ָ�����һ�ε�VMP param������������
			tVmpParam.SetVmpMember(byChnnl, *ptLastVmpMember);
		}
		else
		{
			//�彻��
			StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byChnnl, FALSE, MODE_VIDEO);
            TMt tVmpMem = *(TMt*)ptLastVmpMember;
			m_tVmpChnnlInfo.ClearOneChnnl(tVmpMem); //���Mt��Ӧ��HDͨ��(������ڶ�Ӧͨ��)
		}
	}

	m_tConf.m_tStatus.SetVmpParam(tVmpParam);
	memcpy(&m_tLastVmpParam, &tVmpParam, sizeof(TVMPParam));

	//���߻��ˢ����
	CServMsg cServMsg;
	cServMsg.SetMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
	SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );	

	// xliang [4/10/2009] ͬʱ����tPeriEqpStatus��mcsҪ���ݴ�ˢMTͼ��,����������TMtStatus :(
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
	g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	cServMsg.SetMsgBody();
	cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
	SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );

	return;
}
// END OF FILE

