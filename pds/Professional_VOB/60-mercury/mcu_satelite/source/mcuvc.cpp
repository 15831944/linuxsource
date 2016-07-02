/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcuvc.cpp
   ����ļ�    : mcuvc.h
   �ļ�ʵ�ֹ���: MCUҵ��Ӧ������Ϣӳ�亯������
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2002/07/25  0.9         ����        ����
   2005/02/19  3.6         ����      �����޸ġ���3.5�汾�ϲ�
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
#include "mcuerrcode.h"
#include "mtadpssn.h"
#include "mcuutility.h"
#include "mpssn.h"
#include "eqpssn.h"
#include "mpmanager.h"
#include "dcsssn.h"
#include "evmodem.h"
#include "vctopo.h"
#include "satconst.h"


#if defined(_VXWORKS_)
#include <vxWorks.h>
#include <usrLib.h>
#include <inetLib.h>
#endif
#if defined(_LINUX_)
#include "boardwrapper.h"
#else
#include "brddrvlib.h"
#endif

CMcuVcApp	        g_cMcuVcApp;	//MCUҵ��Ӧ��ʵ��
CMpManager          g_cMpManager;   //MP������ȫ�ֱ���
CAgentInterface     g_cMcuAgent;    //mcu agent

extern u8              g_byMcuModemNum;
extern TMcuModemTopo   g_atMcuModemTopo[MAXNUM_MCU_MODEM];

BOOL32	g_bpIntMsg  = FALSE;
BOOL32  g_bpUnitMsg = FALSE;
API void showintmsg()
{
	g_bpIntMsg = !g_bpIntMsg;
}
API void showunitmsg()
{
	g_bpUnitMsg = !g_bpUnitMsg;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMcuVcInst::CMcuVcInst()
{
	// NEXTSTATE( STATE_IDLE );

	ClearVcInst();

	m_ptMtTable = NULL;
	m_ptSwitchTable = NULL;
	m_ptConfOtherMcTable = NULL;

    m_tPlayFileMediaInfo.clear();
}

CMcuVcInst::~CMcuVcInst()
{
    MCU_SAFE_DELETE(m_ptMtTable);
    MCU_SAFE_DELETE(m_ptSwitchTable);
    MCU_SAFE_DELETE(m_ptConfOtherMcTable);
}

/*====================================================================
    ������      ��InstanceDump
    ����        �����ش�ӡ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 param, ��ӡ״̬���ˣ�0��ʾ��ӡ���У�
					STATE_SCHEDULED��ӡԤԼ��STATE_MASTER
					��ӡ��MCU����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/18    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::InstanceDump( u32 param )
{
	ConfLog( FALSE, "\n---------------Conf: %s-------------------\n",m_tConf.GetConfName() );
	switch( HIWORD16( param ) ) 
	{
	case DUMPCONFALL:

		ConfLog( FALSE, "\nInsID: %d  Create by.%d\n", GetInsID(), m_byCreateBy );

		m_tConf.Print();

		m_tConf.GetConfAttrb().Print();
   
		ShowConfMod();

		m_tConf.m_tStatus.Print();
        m_tConf.m_tStatus.GetConfMode().Print();
                
        if ( CurState() == STATE_ONGOING )
        {            
            if ( m_tConf.m_tStatus.GetProtectMode() != CONF_LOCKMODE_NONE)
            {
                OspPrintf(TRUE, FALSE, "\n");
                m_tConfProtectInfo.Print();
            }
            OspPrintf(TRUE, FALSE, "\n");
            m_tConfInStatus.Print();
            if ( m_tConfInStatus.IsPollSwitch())
            {
                m_tPollSwitchParam.Print();
            }

            if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
            {
                m_tConf.m_tStatus.GetVmpParam().Print();
            }		
            
            if ( m_tConf.m_tStatus.GetPollState() != POLL_STATE_NONE )
            {
                m_tConfPollParam.Print();
            }
            
            if ( m_tConf.m_tStatus.GetTvWallPollState() != POLL_STATE_NONE )
            {
                m_tTvWallPollParam.Print();
            }
            
			if ( m_tConf.m_tStatus.GethduPollState() != POLL_STATE_NONE )
			{
				m_tHduPollParam.Print();
			}
            ShowConfEqp();
        }        		
        
        //ShowConfMt();

		break;

	case DUMPCONFINFO:

		ConfLog( FALSE, "\nInsID: %d  Create by.%d\n", GetInsID(), m_byCreateBy );

		m_tConf.Print();

		// zgc, 2008-04-25, ���ӻ��鷢����Դ��ָ����ʽ��ӡ
		OspPrintf( TRUE, FALSE, "\nType of spec speaker src:\nAud: %d\nVid: %d\n", 
					m_bySpeakerAudSrcSpecType, m_bySpeakerVidSrcSpecType );

		m_tConf.GetConfAttrb().Print();

		m_tConf.m_tStatus.Print();		
		
		m_tConf.m_tStatus.GetConfMode().Print();
		
		if (CONF_LOCKMODE_NEEDPWD == m_tConf.m_tStatus.GetProtectMode())
		{
			OspPrintf( TRUE, FALSE, "\nAll the mc pwd status: \n");

			for( u8 byIdx = 1; byIdx <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIdx ++)
			{
				if (g_cMcuVcApp.IsMcConnected(byIdx))
				{
					OspPrintf( TRUE, FALSE, "MC.%d pwd passed.%d\n", byIdx, m_tConfProtectInfo.IsMcsPwdPassed(byIdx) );
				}
			}
		}

		// ����VCS����
		if (VCS_CONF == m_tConf.GetConfSource())
		{
			OspPrintf(TRUE, FALSE, "\nVCSConfStatus:\n");
			m_cVCSConfStatus.VCCPrint();
		}

		break;

	case DUMPCONFMT:

        {
            u16 wParam = LOWORD16(param);
            u8 byMtId = (u8)wParam;
            
            ShowConfMt(byMtId);
        }

		break;
        
    case DUMPCHGMTRES:
        {
            u16 wParam = LOWORD16(param);
            u8 byMtId = LOBYTE(wParam);
            u8 byRes = HIBYTE(param);
            
            if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
            {
                OspPrintf(TRUE, FALSE, "[InstanceDump] Mt.%d unexist in conf.%d, try another...\n", byMtId, m_byConfIdx);
                return;
            }
            u8 byChnnlType = LOGCHL_VIDEO;

            CServMsg cServMsg;
            cServMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
            cServMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
            cServMsg.CatMsgBody( &byRes, sizeof(u8) );
            SendMsgToMt( byMtId, cServMsg.GetEventId(), cServMsg );
            
            OspPrintf(TRUE, FALSE, "[InstanceDump] chnnl type: %d, send videoformat<%d> change msg to mt<%d>!\n",
                byChnnlType, byRes, byMtId );
        }
        break;

	case DUMPBASINFO:
		
		{
			u16 wParam = LOWORD16(param);
            u8 byBasId = (u8)wParam;

			ShowBasInfo(byBasId);
		}
		break;

	case DUMPMTMONITOR:

		ShowMtMonitor();

		break;
		
	case DUMPCONFSWITCH:

		ShowConfSwitch();

		break;

	case DUMPCONFEQP:

		ShowConfEqp();

		break;

	case DUMPMTSTAT:

		ShowMtStat();

		break;
	case DUMPVMPADAPT:
		 ShowVmpAdaptMember();
		 break;
	default:
		break;
	}
}

/*====================================================================
    ������      ��DaemonInstanceDump
    ����        �����ش�ӡ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 param
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/09    1.0         JQL            ����
	04/03/10    3.0         ������         �޸�
====================================================================*/
void CMcuVcInst::DaemonInstanceDump( u32 param )
{
	u8   byIndex;
	TLogicalChannel tLogicalChannel;
	CConfId       cConfId;
	u32           dwVSndIp = 0, dwASndIp = 0, dwVRcvIp = 0,dwARcvIp = 0;
	u16           wVSndPort = 0, wASndPort = 0, wVRcvPort = 0, wARcvPort = 0;
	TMt           tMt[4];
	BOOL32        bDcsExist = FALSE;

	cConfId.SetNull();

	OspPrintf(TRUE, FALSE,"\nData information in VC:\n");
	switch( HIWORD16( param ) )
	{
	case DUMPMCINFO: //MC
		if( LOWORD16( param ) == DUMPSRC )
		{
			//MCINST CHANNEL SNDASRC   SNDVSRC
			OspPrintf( TRUE, FALSE, "MCINST CHANNEL  SNDVSRC      SNDASRC   \n");
			OspPrintf( TRUE, FALSE, "------ ------- ----------   ----------\n");
			for( byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
			{
				if( g_cMcuVcApp.IsMcConnected( byIndex ) )
				{
					u8 byChannelNum;
					if( g_cMcuVcApp.GetMcLogicChnnl( byIndex, MODE_VIDEO, &byChannelNum, &tLogicalChannel ) )
					{
						while( byChannelNum-- != 0 )
						{
							if( g_cMcuVcApp.GetMcSrc( byIndex, &tMt[0], byChannelNum, MODE_VIDEO ) 
								|| g_cMcuVcApp.GetMcSrc( byIndex, &tMt[1], byChannelNum, MODE_AUDIO ) )
							{
								ConfLog( FALSE, "%6u %7u %3u-%3u-%2u %3u-%3u-%2u\n", byIndex, byChannelNum,
								tMt[0].GetMcuId(), tMt[0].GetMtId(), tMt[0].GetEqpId(),
								tMt[1].GetMcuId(), tMt[1].GetMtId(), tMt[1].GetEqpId() );
							}
						}
					}
				}
			}
		}
		else
		{
			OspPrintf( TRUE, FALSE, "MCINST SNDVIP   SNDVSTARTPORT SNDVIP   SNDVSTARTPORT MCSNDVCHNNL MCSIP   MCSSSRC  GRP\n");
			OspPrintf( TRUE, FALSE, "------ -------- ------------- -------- ------------- ---------- -------- -------- ---\n");
			
			for (byIndex = 1; byIndex < (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++)
			{
				if (g_cMcuVcApp.IsMcConnected(byIndex))
				{
					u8 byChannelNum;
					TMcsRegInfo	tMcsReg;
					if (g_cMcuVcApp.GetMcLogicChnnl(byIndex, MODE_VIDEO, &byChannelNum, &tLogicalChannel))
					{
						dwVSndIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
						wVSndPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
					}

					if (g_cMcuVcApp.GetMcLogicChnnl(byIndex, MODE_AUDIO, &byChannelNum, &tLogicalChannel))
					{	
						dwASndIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
						wASndPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
					}
					g_cMcuVcApp.GetMcsRegInfo(byIndex, &tMcsReg);

					OspPrintf(TRUE, FALSE, "%6u %.8x %13u %.8x %13u %10u %.8x %.8x %3d\n", 
						      byIndex, dwVSndIp, wVSndPort, dwASndIp, wASndPort, byChannelNum, tMcsReg.GetMcsIpAddr(), tMcsReg.GetMcsSSRC(), CMcsSsn::GetUserGroup(byIndex)); 
				}
			}
		}
		break;

	case DUMPEQPINFO: //Peri
		OspPrintf( TRUE, FALSE, "EQPID  SNDVIP   SPORT SNDAIP   SPORT CHNNUM RCVVIP   RPORT RCVAIP   RPORT CHNNUM\n");
		OspPrintf( TRUE, FALSE, "------ -------- ----- -------- ----- ------ -------- ----- -------- ----- ------\n");
		for( byIndex = 1; byIndex < MAXNUM_MCU_PERIEQP; byIndex++ )
		{
			u8 byFwdChannelNum;
			u8 byRvsChannelNum;

			if( g_cMcuVcApp.IsPeriEqpConnected( byIndex ) )
			{	
				if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byIndex, MODE_VIDEO, &byFwdChannelNum, &tLogicalChannel, TRUE) )
				{
					dwVSndIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
		            wVSndPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
				}

				if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byIndex, MODE_AUDIO, &byFwdChannelNum, &tLogicalChannel, TRUE) )
				{
					dwASndIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
		            wASndPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
				}

				if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byIndex, MODE_VIDEO, &byRvsChannelNum, &tLogicalChannel, FALSE) )
				{
					dwVRcvIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
		            wVRcvPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
				}

				if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byIndex, MODE_AUDIO, &byRvsChannelNum, &tLogicalChannel, FALSE) )
				{
					dwARcvIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
		            wARcvPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
				}
				OspPrintf( TRUE, FALSE, "%6u %.8x %5u %.8x %5u %6u %.8x %5u %.8x %5u %6u\n", byIndex,
				           dwVSndIp, wVSndPort, dwASndIp, wASndPort, byFwdChannelNum,
					       dwVRcvIp, wVRcvPort, dwARcvIp, wARcvPort, byRvsChannelNum);
			}
		}
        
        for( byIndex = 1; byIndex <= MAXNUM_MCU_DCS; byIndex ++ )
        {
            if ( g_cMcuVcApp.IsPeriDcsConnected(byIndex) )
            {
                bDcsExist = TRUE;
                break;
            }
        }
        if ( bDcsExist )
        {
            OspPrintf( TRUE, FALSE, "\nDCSID  DCSIP    DCSPORT\n");
            OspPrintf( TRUE, FALSE, "------ -------- -----\n");

            for( byIndex = 1; byIndex <= MAXNUM_MCU_DCS; byIndex ++ )
            {
                if ( g_cMcuVcApp.IsPeriDcsConnected(byIndex) )
                {
                    OspPrintf( TRUE, FALSE, "%6u %.8x %5u \n", byIndex,
                               g_cMcuVcApp.m_atPeriDcsTable[byIndex-1].m_dwDcsIp, 
                               g_cMcuVcApp.m_atPeriDcsTable[byIndex-1].m_wDcsPort);
                }
            }
        }
        OspPrintf( TRUE, FALSE, "________________________________________________________________________________\n");
		break;
	case DUMPEQPSTATUS:
		g_cMcuVcApp.EqpStatusShow((u8  )LOWORD16(param));
		break;
    case DUMPTEMPLATE:
        g_cMcuVcApp.ShowTemplate();
        break;
    case DUMPCONFMAP:
        g_cMcuVcApp.ShowConfMap();
        break;
    case DUMPRLSCONF:
        ReleaseConf( TRUE );
        NEXTSTATE( STATE_IDLE );
        break;
    case DUMPRLSBAS:
        m_cBasMgr.ReleaseHdBasChn();
        break;
    case DUMPBASINFO:
        g_cMcuVcApp.ShowBasInfo();
        break;
	default:
		break;
	}
}

/*====================================================================
    ������      ��SendMsgToMt
    ����        ���������е��¼��ն˷�����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId. ������Ϣ���ն�ID�ţ����ն˵����ݱ����ڻ���ʵ�����ն��������У�����Ч��
				  u16 wEvent. �¼���
				  u8 byMtId �ն�Id��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
  03/12/09      3.0			JQL			  ����
====================================================================*/
BOOL32 CMcuVcInst::SendMsgToMt( u8 byMtId, u16 wEvent, CServMsg & cServMsg )
{
	if (byMtId == 0)
	{
		ConfLog( FALSE, "Send Message(%s) To Mt.%d not exist.\n", OspEventDesc(wEvent), byMtId );
		return FALSE;
	}
    else if (byMtId == CONF_CREATE_MT)
    {
        // guzh [7/31/2006] 
        // ����Ǵ�����Ϣ����ʱMtId��û�и�ֵ����Ҫֱ��ȡ�����DriId
        CallLog("Send msg to Mt Dri.%d, event: %s(%d)\n", 
                cServMsg.GetSrcDriId(), 
                OspEventDesc(wEvent),
                wEvent);

		cServMsg.SetMcuId( LOCAL_MCUID );
    
    #ifdef _SATELITE_
        return g_cMtSsnApp.SendMsgToMtSsn( byMtId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
    #else
        return g_cMtAdpSsnApp.SendMsgToMtAdpSsn( cServMsg.GetSrcDriId(), wEvent, cServMsg );
    #endif
    
    }
	else
	{		
        TMt tDstMt = m_ptMtTable->GetMt( byMtId );
        
		if( GetInsID() != CInstance::DAEMON )
		{
			cServMsg.SetConfId( m_tConf.GetConfId() );
			cServMsg.SetConfIdx( m_byConfIdx );
		}

        if (MCU_MT_FLOWCONTROL_CMD == wEvent)
        {
            TLogicalChannel tLogicalChannel = *(TLogicalChannel *)cServMsg.GetMsgBody();     
            u16 wBitrate = 0;
            
            if (MODE_VIDEO == tLogicalChannel.GetMediaType())
            {
                //�Ƿ���Ҫ��������
                if (IsNeedAdjustMtSndBitrate(byMtId, wBitrate) && wBitrate < tLogicalChannel.GetFlowControl())
                {
                    Mt2Log("[SendFlowctrl2Mt] mt.%d, mode %d, old flowcontrol :%d, min flowcontrol :%d\n",
                            byMtId, 
                            tLogicalChannel.GetMediaType(), tLogicalChannel.GetFlowControl(), wBitrate);                   
                    
                    tLogicalChannel.SetFlowControl(wBitrate);
                    cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));                                        
                }

				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

                // xsl [8/1/2006] �ն˷������ʴ���4M֪ͨ�ն˰�4M��������
                if ( byVmpSubType == VMP && // xliang [4/20/2009] MPU����������
					tLogicalChannel.GetFlowControl() > MAXBITRATE_MTSEND_INVMP &&
                    (m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tDstMt) || 
                    m_tConf.m_tStatus.GetVmpTwParam().IsMtInMember(tDstMt)))
                {
                    tLogicalChannel.SetFlowControl(MAXBITRATE_MTSEND_INVMP); 
                    cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel)); 
                    
                    Mt2Log("[SendFlowctrl2Mt] mt in vmp, mode %d, old flowcontrol :%d, min flowcontrol :%d\n",
                            tLogicalChannel.GetMediaType(), tLogicalChannel.GetFlowControl(), wBitrate);  
                }

                // xsl [8/7/2006] ���ڻش�ͨ���ڣ���������ӦС���ϼ�mcu���մ���
                if( !m_tCascadeMMCU.IsNull() )
	            {
                    wBitrate = m_ptMtTable->GetMtReqBitrate(m_tCascadeMMCU.GetMtId(), TRUE);
		            TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
		            if( ptConfMcInfo != NULL && !ptConfMcInfo->m_tSpyMt.IsNull() )
		            {
			            if( tLogicalChannel.GetFlowControl() > wBitrate &&
                            ptConfMcInfo->m_tSpyMt.GetMtId() == tDstMt.GetMtId() &&
				            ptConfMcInfo->m_tSpyMt.GetMcuId() == tDstMt.GetMcuId() )
                        {
                            tLogicalChannel.SetFlowControl(wBitrate);
                            cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel)); 
                            
                            Mt2Log("[SendFlowctrl2Mt] mt in spying, mode %d, old flowcontrol :%d, min flowcontrol :%d\n",
                            tLogicalChannel.GetMediaType(), tLogicalChannel.GetFlowControl(), wBitrate);  
                        }
                    }
                }
                
                //�Ƿ�С�ڷ��ʹ���
                wBitrate = m_ptMtTable->GetSndBandWidth(byMtId);
                if (wBitrate > 0 && wBitrate <= m_tConf.GetBitRate() && wBitrate < tLogicalChannel.GetFlowControl())
                {                   
                    Mt2Log("[SendFlowctrl2Mt] mode %d, old flowcontrol :%d, bandwidth :%d\n",
                            tLogicalChannel.GetMediaType(), tLogicalChannel.GetFlowControl(), wBitrate);
                    
                    tLogicalChannel.SetFlowControl(wBitrate);
                    cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));     
                }                
            }   
            else if (MODE_SECVIDEO == tLogicalChannel.GetMediaType())
            {
                if(!m_tDoubleStreamSrc.IsNull())
			    {
                    u16 wMinBitRate = GetLeastMtReqBitrate(FALSE, MEDIA_TYPE_NULL, m_tDoubleStreamSrc.GetMtId()); 
				    if (tLogicalChannel.GetFlowControl() > wMinBitRate)
                    {
                        tLogicalChannel.SetFlowControl(wMinBitRate);
                        cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));     

                        Mt2Log("[SendFlowctrl2Mt] mode %d, old flowcontrol :%d, min flowcontrol :%d\n",
                            tLogicalChannel.GetMediaType(), tLogicalChannel.GetFlowControl(), wMinBitRate); 
                    }
			    }
            }

            if (tLogicalChannel.GetFlowControl() == tLogicalChannel.GetCurrFlowControl() &&
                tLogicalChannel.GetFlowControl() > 64)  //����MT3.6��FlowControl����
            {
                return TRUE;
            }
            else
            {
               tLogicalChannel.SetCurrFlowControl(tLogicalChannel.GetFlowControl());
            }
            
            u8 byChannel;
            if (MODE_VIDEO == tLogicalChannel.GetMediaType())
            {
                byChannel = LOGCHL_VIDEO;
            }
            else if (MODE_AUDIO == tLogicalChannel.GetMediaType())
            {
                byChannel = LOGCHL_AUDIO;
            }
            else if (MODE_SECVIDEO == tLogicalChannel.GetMediaType())
            {
                byChannel = LOGCHL_SECVIDEO;
            }
            else
            {
                return FALSE;
            }
            m_ptMtTable->SetMtLogicChnnl(byMtId, byChannel, &tLogicalChannel, FALSE);

            Mt2Log("[SendFlowctrl2Mt] Mt.%d(%s), bitrate: %d, mode :%d\n", 
                    byMtId, StrOfIP(tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr()), 
                    tLogicalChannel.GetFlowControl(), tLogicalChannel.GetMediaType());
        }
        else if(MCU_MT_YOUARESEEING_NOTIF == wEvent)
        {
            TMt tSrcMt = *(TMt *)cServMsg.GetMsgBody();            
            Mt2Log("[SendMsg] send msg %s to Mt.%d, SrcMt(mtid:%d, mtmaintype:%d, mtsubtype:%d)\n",
                    OspEventDesc(wEvent), byMtId, tSrcMt.GetMtId(), tSrcMt.GetType(), tSrcMt.GetMtType());         
        }
        else if ( MCU_MT_FASTUPDATEPIC_CMD == wEvent )
        {
            u8 byMode = *(cServMsg.GetMsgBody());

#ifdef _SATELITE_
			wEvent = 25011/*MCU_MT_SENDIFRAME_NOTIF*/;
			cServMsg.SetDstMtId(tDstMt.GetMtId());
			//cServMsg.SetConfIdx((u8)GetInsID());
#endif
            Mt2Log("[SendMsg] send msg %s to Mt.%d for mode.%d\n",
                    OspEventDesc(wEvent), byMtId, byMode );   
        }
		// xliang [4/16/2009] print test
        else if ( MCU_MCU_ADJMTRES_REQ == wEvent )
		{	
			TMt tMt = *(TMt *)cServMsg.GetMsgBody();
			Mt2Log("[SendMsg] send msg %s to Mt.%d to ask Mt(%u,%u) adjust Video resolution.\n",
				OspEventDesc(wEvent), byMtId, tMt.GetMcuId(), tMt.GetMtId());
		}
		else if ( MCU_MT_INVITEMT_REQ == wEvent )
		{
	#ifdef _SATELITE_
			u8 byConfSecVid = 0;
			if ( MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() &&
				 0 != m_tConf.GetSecVideoMediaType() )
			{
				byConfSecVid = 1;
			}
			cServMsg.CatMsgBody(&byConfSecVid, sizeof(u8));
	#endif
		}
		cServMsg.SetMcuId( LOCAL_MCUID );
        
    #ifdef _SATELITE_
        return g_cMtSsnApp.SendMsgToMtSsn( tDstMt.GetMtId(), wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
    #else
        return g_cMtAdpSsnApp.SendMsgToMtAdpSsn( tDstMt, wEvent, cServMsg );
    #endif
	}
}

/*====================================================================
    ������      ��SendMsgToEqp
    ����        ������Ϣ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
				  u16 wEvent. �¼���
				  u8 byEqpId ����Id��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/26    3.0         ������         ����
====================================================================*/
BOOL32 CMcuVcInst::SendMsgToEqp( u8 byEqpId, u16 wEvent, CServMsg & cServMsg )
{
	if( byEqpId == 0 )
	{
		ConfLog( FALSE, "Send Message.%d<%s> To Eqp %d not exist.\n", 
                 wEvent, OspEventDesc(wEvent), byEqpId );
		return FALSE;
	}
	else
	{
		if( GetInsID() != CInstance::DAEMON )
		{
			cServMsg.SetConfId( m_tConf.GetConfId() );
			cServMsg.SetConfIdx( m_byConfIdx );
		}
		cServMsg.SetMcuId( LOCAL_MCUID );       

		return g_cEqpSsnApp.SendMsgToPeriEqpSsn( byEqpId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );		 
	}	
}

/*====================================================================
    ������      ��SendMsgToMcs
    ����        ������Ϣ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
				  u16 wEvent. �¼���
				  u8 byMcsId ���Id��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/26    3.0         ������         ����
====================================================================*/
BOOL32 CMcuVcInst::SendMsgToMcs( u8 byMcsId, u16 wEvent, CServMsg & cServMsg )
{
	if( byMcsId == 0 )
	{
		ConfLog( FALSE, "Send Message.%d(%s) To Mcs.%d not exist.\n", wEvent, OspEventDesc(wEvent), byMcsId );
		return FALSE;
	}
	else
	{
		if( GetInsID() != CInstance::DAEMON )
		{
			cServMsg.SetConfId( m_tConf.GetConfId() );
			cServMsg.SetConfIdx( m_byConfIdx );
		}
		cServMsg.SetMcuId( LOCAL_MCUID );
		return CMcsSsn::SendMsgToMcsSsn( byMcsId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );		 
	}
}

/*====================================================================
    ������      ��SendMsgToAllMcs
    ����        ������Ϣ�����л��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
				  u16 wEvent. �¼���
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/26    3.0         ������         ����
====================================================================*/
void CMcuVcInst::SendMsgToAllMcs( u16 wEvent, CServMsg & cServMsg )
{
	if( GetInsID() != CInstance::DAEMON )
	{
		cServMsg.SetConfIdx( m_byConfIdx );
		cServMsg.SetConfId( m_tConf.GetConfId() );
	}
	cServMsg.SetMcuId( LOCAL_MCUID );
	CMcsSsn::BroadcastToAllMcsSsn( wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
}

/*====================================================================
    ������      ��SendMsgToAllMp
    ����        ������Ϣ������MP
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wEvent. �¼���
                  CServMsg & cServMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/04/27    3.6         libo          ����
====================================================================*/
void CMcuVcInst::SendMsgToAllMp(u16 wEvent, CServMsg & cServMsg)
{
	if (GetInsID() != CInstance::DAEMON)
	{
		cServMsg.SetConfIdx(m_byConfIdx);
		//cServMsg.SetConfId( m_tConf.GetConfId() );
	}
	cServMsg.SetMcuId(LOCAL_MCUID);
	g_cMpSsnApp.BroadcastToAllMpSsn(wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
}

/*=============================================================================
  �� �� ���� SendMsgToDcsSsn
  ��    �ܣ� ����Ϣ��DCS�Ự��ĳ��ʵ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� BOOL32
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/16    4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::SendMsgToDcsSsn( u8 byInst, CServMsg &cServMsg )
{
	u32 dwRet = OspPost( MAKEIID(AID_MCU_DCSSSN, byInst), cServMsg.GetEventId(), 
						 cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), 0, 
						 MAKEIID(GetAppID(), GetInsID()) );
	if ( OSP_OK != dwRet )
	{
        ConfLog( FALSE, "send msg %d<%s> to dcsssn failed, ret:%d !\n", 
            cServMsg.GetEventId(), OspEventDesc( cServMsg.GetEventId() ), dwRet );
        return FALSE;
	}	

	return TRUE;
}

/*====================================================================
    ������      ��BroadcastToAllSubMtJoinedConf
    ����        �����㲥��Ϣ�����вμӱ������ֱ���ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ�룬ȱʡΪNULL
				  u16 wLen, ��Ϣ���ȣ�ȱʡΪ0
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/03    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::BroadcastToAllSubMtJoinedConf( u16 wEvent, CServMsg & cServMsg )
{
	cServMsg.SetConfIdx( m_byConfIdx );
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetDstMtId( 0 );
	g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn( wEvent, cServMsg );
}

/*====================================================================
    ������      ��InstanceEntry
    ����        ��ʵ����Ϣ������ں���������override
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/25    1.0         LI Yi         ����
	03/11/17    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::InstanceEntry( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		ConfLog(FALSE, "[CMcuVcInst] The received msg's pointer in the msg InstanceEntry is NULL!");
		return;
	}

	//�������ݵ���ʱ��[MSMag <->McuVc]ģ�黥�������� 2005-12-15
	g_cMSSsnApp.EnterMSSynLock(AID_MCU_VC);

	if ( pcMsg->event > MCUVC_FASTUPDATE_TIMER_ARRAY && pcMsg->event <= MCUVC_FASTUPDATE_TIMER_ARRAY + MAXNUM_CONF_MT )
	{
		ProcMcuMtFastUpdateTimer(pcMsg);
		
		//�������ݵ���ʱ��[MSMag <->McuVc]ģ�黥�������� 2005-12-15
		g_cMSSsnApp.LeaveMSSynLock(AID_MCU_VC);
		return;
	}
    
    if ( pcMsg->event > MCUVC_FASTUPDATE_TIMER_ARRAY4EQP && pcMsg->event <= MCUVC_FASTUPDATE_TIMER_ARRAY4EQP + MAXNUM_PERIEQP*3)
    {
        ProcMcuEqpFastUpdateTimer(pcMsg);
        g_cMSSsnApp.LeaveMSSynLock(AID_MCU_VC);
        return;
    }
    
	switch( pcMsg->event )
	{
	case OSP_POWERON:				//�ϵ���Ϣ
		break;
    
	case OSP_OVERFLOW:
        {
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
			SendMsgToMcs(cServMsg.GetSrcSsnId(), cServMsg.GetEventId()+2, cServMsg);
        }
		break;
    
    /*----------------------------�������-------------------------------*/

	//�������---��������
	case MCS_MCU_CREATECONF_REQ:          //�������̨��MCU�ϴ���һ������
	case MCU_CREATECONF_FROMFILE:		  //������������Ϣ 
	case MCU_SCHEDULE_CONF_START:         //ԤԼ�����Ϊ��ʱ����
	case MT_MCU_CREATECONF_REQ:		      //����������Ϣ
    case MCU_CREATECONF_NPLUS:
		ProcMcsMcuCreateConfReq(pcMsg);
		break;
	case MCU_WAITEQP_CREATE_CONF_NOTIFY:
		ProcWaitEqpToCreateConf(pcMsg);
		break;
        
    case MCUVC_CONFSTARTREC_TIMER:        //�Զ�¼��
        ProcTimerAutoRec(pcMsg);
        break;
        
	//�������---��������
	case MCS_MCU_RELEASECONF_REQ:         //�������̨����MCU����һ������
	case MT_MCU_DROPCONF_CMD:		      //��ϯ�ն���ֹ��������
		ProcMcsMcuReleaseConfReq(pcMsg);
		break;

	//�������---��������
	case MCS_MCU_CHANGECONFLOCKMODE_REQ:  //�������̨����MCU�ı���鱣����ʽ
		ProcMcsMcuChangeLockModeConfReq(pcMsg);
		break;
	case MCS_MCU_ENTERCONFPWD_ACK:        //�������̨��ӦMCU����������   
	case MCS_MCU_ENTERCONFPWD_NACK:       //�������̨�ܾ�MCU����������
		ProcMcsMcuEnterPwdRsp(pcMsg);
		break;
	case MCS_MCU_CHANGECONFPWD_REQ:       //�������̨����MCU�ı��������
		ProcMcsMcuChangeConfPwdReq(pcMsg);
		break;
	case MCS_MCU_GETLOCKINFO_REQ:
		ProcMcsMcuGetLockInfoReq(pcMsg); //��صõ����������Ϣ����
		break;

	//�������---�������
	case MCS_MCU_SAVECONF_REQ:            //�������̨����MCU�������
		ProcMcsMcuSaveConfReq(pcMsg);
		break;

	//�������---�޸Ļ���
	case MCS_MCU_MODIFYCONF_REQ:	      //�޸Ļ�����Ϣ
		ProcMcsMcuModifyConfReq(pcMsg);
		break;

	//�������---�ӳ�����
	case MT_MCU_DELAYCONF_REQ:			  //�ն�����MCU�ӳ�����
	case MCS_MCU_DELAYCONF_REQ:           //�������̨����MCU�ӳ�����
		ProcMcsMcuDelayConfReq(pcMsg);
		break;

	//���ݻ������---����DCS��Ӧ�� 2005-12-16
	case DCSSSN_MCU_CREATECONF_ACK:
	case DCSSSN_MCU_CREATECONF_NACK:
	case DCSSSN_MCU_CONFCREATED_NOTIF:
		ProcDcsMcuCreateConfRsp(pcMsg);
		break;

	case MCS_MCU_CHANGEVACHOLDTIME_REQ:	  //�������̨����MCU�ı����������л�ʱ��
		ProcMcsMcuChangeVacHoldTimeReq(pcMsg);
		break;

    case NMS_MCU_SCHEDULE_CONF_NOTIF:
		ProcNmsMcuSchedConf( pcMsg );
        break; 

    case TIMER_SCHEDCONF:
        ProcSchedConfTimeOut( pcMsg );
		break;

    case NMS_MCU_APPLYCONFFREQ_NOTIF:
    case MCUVC_APPLYFREQUENCE_CHECK_TIMER:
        ProcGetFreqFromNMS( pcMsg );
        break;

	/*----------------------------�������-------------------------------*/

	//�������---��ϯ
	case MCS_MCU_SPECCHAIRMAN_REQ:       //�������ָ̨��һ̨�ն�Ϊ��ϯ
	case MT_MCU_SPECCHAIRMAN_REQ:		 //��ϯ�ն�ָ����ϯ����
		ProcMcsMcuSpecChairmanReq(pcMsg);
		break;
	case MCS_MCU_CANCELCHAIRMAN_REQ:     //�������̨ȡ����ǰ������ϯ 
	case MT_MCU_CANCELCHAIRMAN_REQ:		 //��ϯ�ն�ȡ���Լ���ϯȨ������
		ProcMcsMcuCancelChairmanReq(pcMsg);
		break;
    case MCS_MCU_SETCHAIRMODE_CMD:       //�������̨���û������ϯ��ʽ
        ProcMcsMcuSetConfChairMode(pcMsg);
		break;

	//�������---����
	case MCS_MCU_SPECSPEAKER_REQ:        //�������ָ̨��һ̨�ն˷���
	case MT_MCU_SPECSPEAKER_REQ:		 //��ͨ�ն�ָ������������
	case MT_MCU_SPECSPEAKER_CMD:		 //��ϯ�ն�ָ������������
		ProcMcsMcuSpecSpeakerReq(pcMsg);
		break;
	case MCS_MCU_CANCELSPEAKER_REQ:      //�������̨ȡ������Speaker
	case MT_MCU_CANCELSPEAKER_CMD:		 //��ϯ�ն�ȡ������������
		ProcMcsMcuCancelSpeakerReq(pcMsg);
		break;
	case MCS_MCU_MTSEESPEAKER_CMD:
    case MT_MCU_VIEWBRAODCASTINGSRC_CMD:    //ǿ�ƹ㲥
		ProcMcsMcuSeeSpeakerCmd(pcMsg);
		break;
        
    case MCS_MCU_SPECOUTVIEW_REQ:       //�������ָ̨���ش�ͨ��
        ProcMcsMcuSpecOutViewReq(pcMsg);
        break;        

    //����MCU
	case MCU_MCU_REGISTER_NOTIF:
		ProcMcuMcuRegisterNotify(pcMsg);
		break;
	case MCU_MCU_ROSTER_NOTIF:
		ProcMcuMcuRosterNotify(pcMsg);
		break;
	case MCU_MCU_MTLIST_REQ:
		ProcMcuMcuMtListReq(pcMsg);
		break;
	case MCU_MCU_MTLIST_ACK:
		ProcMcuMcuMtListAck(pcMsg);
		break;
	case MCU_MCU_VIDEOINFO_REQ:
		ProcMcuMcuVideoInfoReq(pcMsg);
		break;
	case MCU_MCU_VIDEOINFO_ACK:
		ProcMcuMcuVideoInfoAck(pcMsg);
		break;
	case MCU_MCU_CONFVIEWCHG_NOTIF:
		ProcMcuMcuConfViewChgNtf(pcMsg);
		break;
	case MCU_MCU_AUDIOINFO_REQ:
		ProcMcuMcuAudioInfoReq(pcMsg);
		break;
	case MCU_MCU_AUDIOINFO_ACK:
		ProcMcuMcuAudioInfoAck(pcMsg);
        break;
	case MCU_MCU_NEWMT_NOTIF:
		ProcMcuMcuNewMtNotify(pcMsg);
		break;
	case MCU_MCU_CALLALERTING_NOTIF:
		ProcMcuMcuCallAlertMtNotify(pcMsg);
		break;
	case MCU_MCU_DROPMT_NOTIF:
		ProcMcuMcuDropMtNotify(pcMsg);
		break;
	case MCU_MCU_DELMT_NOTIF:
		ProcMcuMcuDelMtNotify(pcMsg);
		break;
	case MCU_MCU_SETMTCHAN_REQ:
		ProcMcuMcuSetMtChanReq(pcMsg);
		break;
	case MCU_MCU_SETMTCHAN_NOTIF:
		ProcMcuMcuSetMtChanNotify(pcMsg);
		break;
	case MCU_MCU_SETIN_REQ:
		ProcMcuMcuSetInReq(pcMsg);
		break;
	case MCU_MCU_SETOUT_REQ:
		ProcMcuMcuSetOutReq(pcMsg);
		break;
	case MCU_MCU_SETOUT_NOTIF:
		ProcMcuMcuSetOutNotify(pcMsg);
		break;

	case MCU_MCU_STARTMIXER_CMD:
		ProcMcuMcuStartMixerCmd(pcMsg);
		break;
/*		
	case MCU_MCU_STARTMIXER_REQ:
		ProcMcuMcuStartMixerReq(pcMsg);
		break;
	case MCU_MCU_STARTMIXER_ACK:
		ProcMcuMcuStartMixerAck(pcMsg);
		break;
	case MCU_MCU_STARTMIXER_NACK:
		ProcMcuMcuStartMixerNack(pcMsg);
		break;
*/
	case MCU_MCU_STARTMIXER_NOTIF:
		ProcMcuMcuStartMixerNotif(pcMsg);
		break;
	case MCU_MCU_STOPMIXER_CMD:
		ProcMcuMcuStopMixerCmd(pcMsg);
		break;
	/*
	case MCU_MCU_STOPMIXER_REQ:
		ProcMcuMcuStopMixerReq(pcMsg);
		break;
	case MCU_MCU_STOPMIXER_ACK:
		ProcMcuMcuStopMixerAck(pcMsg);
		break;
	case MCU_MCU_STOPMIXER_NACK:
		ProcMcuMcuStopMixerNack(pcMsg);
		break;
	*/
	case MCU_MCU_STOPMIXER_NOTIF:
		ProcMcuMcuStopMixerNotif(pcMsg);
		break;
	case MCU_MCU_ADJMTRES_REQ:
		ProcMcuMcuAdjustMtResReq(pcMsg);
		break;
	case MCU_MCU_ADJMTRES_ACK:
		ProcMcuMcuAdjustMtResAck(pcMsg);
		break;
	case MCU_MCU_ADJMTRES_NACK:
		break;
	case MCU_MCU_GETMIXERPARAM_REQ:
		ProcMcuMcuGetMixerParamReq(pcMsg);
		break;
	case MCU_MCU_GETMIXERPARAM_ACK:
		ProcMcuMcuGetMixerParamAck(pcMsg);
		break;
	case MCU_MCU_GETMIXERPARAM_NACK:
		ProcMcuMcuGetMixerParamNack(pcMsg);
		break;
	case MCU_MCU_MIXERPARAM_NOTIF:
		ProcMcuMcuMixerParamNotif(pcMsg);
		break;
	case MCU_MCU_ADDMIXMEMBER_CMD:
		ProcMcuMcuAddMixerMemberCmd(pcMsg);
		break;
	case MCU_MCU_REMOVEMIXMEMBER_CMD:
		ProcMcuMcuRemoveMixerMemberCmd(pcMsg);
		break;
	case MCU_MCU_LOCK_REQ:
		ProcMcuMcuLockReq(pcMsg);
		break;
	case MCU_MCU_LOCK_ACK:
		ProcMcuMcuLockAck(pcMsg);
		break;
	case MCU_MCU_LOCK_NACK:
		ProcMcuMcuLockNack(pcMsg);
		break;
	case MCU_MCU_MTSTATUS_CMD:
		ProcMcuMcuMtStatusCmd(pcMsg);
		break;
	case MCU_MCU_MTSTATUS_NOTIF:
		ProcMcuMcuMtStatusNotif(pcMsg);
		break;
    case MCU_MCU_AUTOSWITCH_REQ:
        ProcMcuMcuAutoSwitchReq(pcMsg);
        break;
    case MCU_MCU_AUTOSWITCH_ACK:
    case MCU_MCU_AUTOSWITCH_NACK:           
        ProcMcuMcuAutoSwitchRsp(pcMsg);
        break;
    case MCUVC_AUTOSWITCH_TIMER:
        ProcMcuMcuAutoSwitchTimer(pcMsg);
        break;
		
	case MCS_MCU_ADDMT_REQ:              //�������̨�����ն�	
	case MT_MCU_ADDMT_REQ:			     //��ϯ�ն������ն�
	case MCU_MCU_INVITEMT_REQ:           //MCU-MCU�����ն�
		ProcMcsMcuAddMtReq(pcMsg);
		break;

	case MCS_MCU_ADDMTEX_REQ:
		ProcMcsMcuAddMtExReq(pcMsg);     //���԰��������ն˴�������ͨ��ʹ�õ�������
		break;

    case MCU_MCU_INVITEMT_ACK:
    case MCU_MCU_INVITEMT_NACK:
        break;
	case MCS_MCU_DELMT_REQ:              //�������̨�����ն����
	case MT_MCU_DELMT_REQ:			     //��ϯ�ն�ǿ���ն��˳�����
	case MCU_MCU_DELMT_REQ:			     //��ϯ�ն�ǿ���ն��˳�����
		ProcMcsMcuDelMtReq(pcMsg);
		break;

	// ������� --- hdu  //4.6.1 �¼�  jlb
    case MCS_MCU_START_SWITCH_HDU_REQ:
    case MCUVC_STARTSWITCHHDU_NOTIFY:
        ProcMcsMcuStartSwitchHduReq( pcMsg );
        break;

    case MCS_MCU_STOP_SWITCH_HDU_REQ:
		ProcMcsMcuStopSwitchHduReq( pcMsg );
		break;

    case MCS_MCU_CHANGEHDUVOLUME_REQ:
		ProcMcsMcuChangeHduVolume( pcMsg );
		break;

    //�������---ѡ��
	case MT_MCU_STARTSELMT_CMD:			 //��׼ѡ������
	case MT_MCU_STARTSELMT_REQ:          //
	case MCS_MCU_STARTSWITCHMT_REQ:	     //����̨Ҫ�󽻻�����
	case MCS_MCU_STARTSWITCHMC_REQ:      //���ѡ���ն�
		ProcMtMcuStartSwitchMtReq(pcMsg);
		break;
	case MCS_MCU_STARTSWITCHVMPMT_REQ:	 //��ϯ�ն�ѡ������ϳ� // xliang [12/31/2008]  
	case MT_MCU_STARTSWITCHVMPMT_REQ:	
		ProcMcsMcuStartSwitchVmpMtReq(pcMsg);
		break;
	case MCS_MCU_START_SWITCH_TW_REQ:    //�������̨�����ǽ��ʼ��������
		ProcMcsMcuStartSwitchTWReq(pcMsg);
		break;
	case MT_MCU_STOPSELMT_CMD:			 //��׼ֹͣѡ������
	case MCS_MCU_STOPSWITCHMT_REQ:	     //����̨Ҫ�󽻻�����
		ProcMtMcuStopSwitchMtReq(pcMsg);
		break;

    //�������---����Ϣ
	case MCS_MCU_SENDRUNMSG_CMD:         //�������̨����MCU���ն˷��Ͷ���Ϣ���ն˺�Ϊ0��ʾ���������ն�
	case MT_MCU_SENDMSG_CMD:             //�ն˶���Ϣ����
		ProcMcsMcuSendMsgReq(pcMsg);
		break;
	case MCU_MCU_SENDMSG_NOTIF:          //��MCU�Ķ���Ϣ
		ProcMcuMcuSendMsgReq(pcMsg);
		break;

	//�������---�õ��ն��б�
	case MCS_MCU_GETMTLIST_REQ:          //�������̨����MCU�ĵõ��ն��б�����
        ProcMcsMcuGetMtListReq(pcMsg);
		break;
	case MCS_MCU_REFRESHMCU_CMD:
		ProcMcsMcuRefreshMcuCmd(pcMsg);
		break;
		//vmp��ռӦ��// xliang [12/12/2008] 
	case MCS_MCU_VMPPRISEIZE_ACK:
	case MCS_MCU_VMPPRISEIZE_NACK:
		ProcMcsMcuVmpPriSeizeRsp(pcMsg);
		break;
	case MCUVC_MTSEIZEVMP_TIMER:
		ProcMtSeizeVmpTimer(pcMsg);
		break;
	case MCUVC_VMPBATCHPOLL_TIMER:
		ProcVmpBatchPollTimer(pcMsg);

	//�������---�õ�������Ϣ
	case MCS_MCU_GETCONFINFO_REQ:        //�������̨��MCU��ѯ������Ϣ 
	case MT_MCU_GETCONFINFO_REQ:         //�ն���MCU��ѯ������Ϣ
    case MCS_MCU_GETMAUSTATUS_REQ:       //�������̨��MCU��ѯMAU��Ϣ 
        ProcMcsMcuGetConfInfoReq(pcMsg);
		break;

	//�������---��Ƶ���Ͽ���
	case MCS_MCU_STARTVMP_REQ:			//��ؿ�ʼ��Ƶ��������
	case MCS_MCU_STOPVMP_REQ:			//��ؽ�����Ƶ��������	
	case MCS_MCU_CHANGEVMPPARAM_REQ:	//��ػ������̨����MCU�ı��������			
	case MCS_MCU_GETVMPPARAM_REQ:		//��ز�ѯ������Ա����
	case MCS_MCU_STARTVMPBRDST_REQ:		//�������̨����MCU��ʼ�ѻ���ϳ�ͼ��㲥���ն�
	case MCS_MCU_STOPVMPBRDST_REQ:		//�������̨����MCUֹͣ�ѻ���ϳ�ͼ��㲥���ն�
	case MT_MCU_STARTVMP_REQ:			//��ϯ��ʼ��Ƶ��������
	case MT_MCU_STOPVMP_REQ:			//��ϯ������Ƶ��������	
	case MT_MCU_CHANGEVMPPARAM_REQ:		//��ϯ�������̨����MCU�ı��������
	case MT_MCU_GETVMPPARAM_REQ:		//��ϯ��ѯ������Ա����
	case MT_MCU_STARTVMPBRDST_REQ:		//��ϯ����MCU��ʼ�ѻ���ϳ�ͼ��㲥���ն�
	case MT_MCU_STOPVMPBRDST_REQ:		//��ϯ����MCUֹͣ�ѻ���ϳ�ͼ��㲥���ն�
	case MCS_MCU_START_VMPBATCHPOLL_REQ://��ʼ����ϳ�������ѯ// xliang [12/31/2008] 
        ProcMcsMcuVMPReq(pcMsg);
		break;
	case MCS_MCU_PAUSE_VMPBATCHPOLL_CMD:	//��ͣ����ϳ�������ѯ// xliang [12/31/2008]  
	case MCS_MCU_RESUME_VMPBATCHPOLL_CMD:	//�ָ�����ϳ�������ѯ
	case MCS_MCU_STOP_VMPBATCHPOLL_CMD:		//ֹͣ����ϳ�������ѯ
		ProcMcsMcuVmpCmd(pcMsg);
		break;
	case VMP_MCU_STARTVIDMIX_ACK:       //VMP��MCU��ʼ����ȷ��
	case VMP_MCU_STARTVIDMIX_NACK:      //VMP��MCU��ʼ�����ܾ�
	case VMP_MCU_STOPVIDMIX_ACK:        //VMP��MCUֹͣ����ȷ��
	case VMP_MCU_STOPVIDMIX_NACK:       //VMP��MCUֹͣ�����ܾ�
	case VMP_MCU_CHANGEVIDMIXPARAM_ACK: //VMP��MCU�ı临�ϲ���ȷ��
	case VMP_MCU_CHANGEVIDMIXPARAM_NACK://VMP��MCU�ı临�ϲ����ܾ�
	case VMP_MCU_GETVIDMIXPARAM_ACK:    //VMP��MCU���͸��ϲ���ȷ��
	case VMP_MCU_GETVIDMIXPARAM_NACK:   //VMP��MCU���͸��ϲ����ܾ�
        ProcVmpMcuRsp(pcMsg);
		break;
    case VMP_MCU_STARTVIDMIX_NOTIF:     //VMP��MCU��ʼ����֪ͨ
    case VMP_MCU_STOPVIDMIX_NOTIF:      //VMP��MCUֹͣ����֪ͨ
    case VMP_MCU_CHANGESTATUS_NOTIF:    //VMP��MCU�ı临�ϲ���֪ͨ
    case MCU_VMPCONNECTED_NOTIF:
    case MCU_VMPDISCONNECTED_NOTIF:     //VMP����֪ͨ
    case VMP_MCU_NEEDIFRAME_CMD:        //����ϳ�����I֡
        ProcVmpMcuNotif(pcMsg); 
        break;
	case MCUVC_VMP_WAITVMPRSP_TIMER:      //MCU�ȴ�VMPӦ��ʱ
		ProcVmpRspWaitTimer(pcMsg);
		break;

    //�������---���ϵ���ǽ����
	case MCS_MCU_STARTVMPTW_REQ:        //��ؿ�ʼ���ϵ���ǽ����
	case MCS_MCU_STOPVMPTW_REQ:         //��ؽ������ϵ���ǽ����
	case MCS_MCU_CHANGEVMPTWPARAM_REQ:  //��ػ������̨����MCU�ı临�ϵ���ǽ����
//	case MCS_MCU_GETVMPPARAM_REQ:		//��ز�ѯ������Ա����
//	case MCS_MCU_STARTVMPBRDST_REQ:		//�������̨����MCU��ʼ�ѻ���ϳ�ͼ��㲥���ն�
//	case MCS_MCU_STOPVMPBRDST_REQ:		//�������̨����MCUֹͣ�ѻ���ϳ�ͼ��㲥���ն�
        ProcMcsMcuVmpTwReq(pcMsg);
        break;
    case VMPTW_MCU_STARTVIDMIX_ACK:       //VMPTW��MCU��ʼ����ȷ��
    case VMPTW_MCU_STARTVIDMIX_NACK:      //VMPTW��MCU��ʼ�����ܾ�
    case VMPTW_MCU_STOPVIDMIX_ACK:        //VMPTW��MCUֹͣ����ȷ��
    case VMPTW_MCU_STOPVIDMIX_NACK:       //VMPTW��MCUֹͣ�����ܾ�
    case VMPTW_MCU_CHANGEVIDMIXPARAM_ACK: //VMPTW��MCU�ı临�ϲ���ȷ��
    case VMPTW_MCU_CHANGEVIDMIXPARAM_NACK://VMPTW��MCU�ı临�ϲ����ܾ�
//    case VMPTW_MCU_GETVIDMIXPARAM_ACK:    //VMPTW��MCU���͸��ϲ���ȷ��
//    case VMPTW_MCU_GETVIDMIXPARAM_NACK:   //VMPTW��MCU���͸��ϲ����ܾ�
        ProcVmpTwMcuRsp(pcMsg);
        break;
    case VMPTW_MCU_STARTVIDMIX_NOTIF:     //VMPTW��MCU��ʼ����֪ͨ
	case VMPTW_MCU_STOPVIDMIX_NOTIF:      //VMPTW��MCUֹͣ����֪ͨ
	case VMPTW_MCU_CHANGESTATUS_NOTIF:    //VMPTW��MCU�ı临�ϲ���֪ͨ
    case MCU_VMPTWCONNECTED_NOTIF:
	case MCU_VMPTWDISCONNECTED_NOTIF:     //VMPTW����֪ͨ
	case VMPTW_MCU_NEEDIFRAME_CMD:        //VMPTW��MCU����I֡
	    ProcVmpTwMcuNotif(pcMsg); 
		break;

	//�������---�õ�����״̬
	case MCS_MCU_GETCONFSTATUS_REQ:     //�������̨��MCU��ѯ����״̬
        ProcMcsMcuGetConfStatusReq(pcMsg);
		break;
	case MCS_MCU_MCUMEDIASRC_REQ:
		ProcMcsMcuMcuMediaSrcReq(pcMsg);
		break;
	case MCS_MCU_LOCKSMCU_REQ:
		ProcMcsMcuLockSMcuReq(pcMsg);
		break;	
	case MCS_MCU_GETMCULOCKSTATUS_REQ:
		ProcMcsMcuGetMcuLockStatusReq(pcMsg);
		break;

    //�������---�����������Ʒ���
	case MCS_MCU_STARTVAC_REQ:        //�������̨����MCU��ʼ�����������Ʒ���		
	case MCS_MCU_STOPVAC_REQ:         //�������̨����MCUֹͣ�����������Ʒ���
	case MT_MCU_STARTVAC_REQ:         //�ն�����MCU��ʼ�����������Ʒ���
	case MT_MCU_STOPVAC_REQ:          //�ն�����MCUֹͣ�����������Ʒ���
        ProcMcsMcuVACReq(pcMsg);
		break;		
				
	//�������---��������
	case MCS_MCU_STARTDISCUSS_REQ:     //��ʼ������������ - ���ڱ�����ʼ���۲���
	case MCS_MCU_STOPDISCUSS_REQ:      //���������������� - ���ڱ�����ʼ���۲���

    //zbq[11/01/2007] �����Ż�
    case MCS_MCU_GETMIXPARAM_REQ:      //�������̨��ѯ���۲�������
    //case MCS_MCU_STARTDISCUSS_CMD:     //��ʼ������������ - ���ڿ缶��ʼ���۲���
	//case MCS_MCU_STOPDISCUSS_CMD:      //���������������� - ���ڿ缶��ʼ���۲���
	//case MCS_MCU_GETDISCUSSPARAM_REQ:  //�������̨��ѯ���۲�������

        ProcMcsMcuMixReq(pcMsg);
		break;
	case MT_MCU_STARTDISCUSS_REQ:      //�ն˿�ʼ������������
	case MT_MCU_STOPDISCUSS_REQ:       //�ն�����MCU������������
		ProcMtMcuMixReq(pcMsg);
		break;
        //������ʱ����
    case MCS_MCU_CHANGEMIXDELAY_REQ:
        ProcMcsMcuChgMixDelayReq(pcMsg);
        break;
	case MIXER_MCU_STARTMIX_ACK:        //ͬ�⿪ʼ����Ӧ��
	case MIXER_MCU_STARTMIX_NACK:       //�ܾ���ʼ����Ӧ��
	case MIXER_MCU_STOPMIX_ACK:         //ͬ��ֹͣ����Ӧ��
	case MIXER_MCU_STOPMIX_NACK:        //�ܾ�ֹͣ����Ӧ��
	case MIXER_MCU_ADDMEMBER_ACK:       //�����ԱӦ����Ϣ
	case MIXER_MCU_ADDMEMBER_NACK:      //�����ԱӦ����Ϣ
	case MIXER_MCU_REMOVEMEMBER_ACK:    //ɾ����ԱӦ����Ϣ
	case MIXER_MCU_REMOVEMEMBER_NACK:   //ɾ����ԱӦ����Ϣ
	case MIXER_MCU_FORCEACTIVE_ACK:     //ǿ�Ƴ�Ա����Ӧ��
	case MIXER_MCU_FORCEACTIVE_NACK:    //ǿ�Ƴ�Ա�����ܾ�
	case MIXER_MCU_CANCELFORCEACTIVE_ACK:    //ȡ����Աǿ�ƻ���Ӧ��
	case MIXER_MCU_CANCELFORCEACTIVE_NACK:   //ȡ����Աǿ�ƻ����ܾ�
		ProcMixerMcuRsp(pcMsg);
		break;
	case MIXER_MCU_GRPSTATUS_NOTIF:     //������״̬֪ͨ
	case MIXER_MCU_ACTIVEMMBCHANGE_NOTIF:  //����������Ա�ı�֪ͨ
	case MIXER_MCU_CHNNLVOL_NOTIF:      //ĳͨ������֪ͨ��Ϣ
	case MCU_MIXERCONNECTED_NOTIF:      //����������֪ͨ
	case MCU_MIXERDISCONNECTED_NOTIF:   //����������֪ͨ
    case MIXER_MCU_MIXERSTATUS_NOTIF:   //������ͨ��׼�����֪ͨ
	    ProcMixerMcuNotif(pcMsg);
		break;
	case MCUVC_MIX_WAITMIXERRSP_TIMER:      //MCU�ȴ�VMPӦ��ʱ
		ProcMixerRspWaitTimer(pcMsg);
		break;
	case MT_MCU_ADDMIXMEMBER_CMD:           //��ϯ���ӻ����ն�
	case MCS_MCU_ADDMIXMEMBER_CMD:          //���ӻ�����Ա
		ProcMcsMcuAddMixMemberCmd(pcMsg);
		break;
	case MCS_MCU_REMOVEMIXMEMBER_CMD:       //�Ƴ�������Ա
		ProcMcsMcuRemoveMixMemberCmd(pcMsg);
		break;
    case MCS_MCU_REPLACEMIXMEMBER_CMD:      //�滻������Ա
        ProcMcsMcuReplaceMixMemberCmd(pcMsg);
        break;
	case MT_MCU_APPLYMIX_NOTIF:             //�ն˷���MCU������μӻ�������
		ProcMtMcuApplyMixNotify(pcMsg);
		break;

	//�������---��ѯ����
	case MCS_MCU_STARTPOLL_CMD:         //�������̨����û��鿪ʼ��ѯ�㲥
	case MCS_MCU_STOPPOLL_CMD:          //�������̨����û���ֹͣ��ѯ�㲥  
	case MCS_MCU_PAUSEPOLL_CMD:         //�������̨����û�����ͣ��ѯ�㲥
	case MCS_MCU_RESUMEPOLL_CMD:        //�������̨����û��������ѯ�㲥
	case MCS_MCU_GETPOLLPARAM_REQ:      //�������̨��MCU��ѯ������ѯ����
    case MCS_MCU_CHANGEPOLLPARAM_CMD:   //�������̨����û��������ѯ�б�
	case MCS_MCU_SPECPOLLPOS_REQ:		//�������ָ̨��������ѯλ��
		ProcMcsMcuPollMsg(pcMsg);
		break;
	case MCUVC_POLLING_CHANGE_TIMER:
		ProcPollingChangeTimerMsg(pcMsg);     //��ѯ��ʱ��Ϣ����
		break;

    //����ǽ��ѯ
    case MCS_MCU_STARTTWPOLL_CMD:           
    case MCS_MCU_STOPTWPOLL_CMD:
    case MCS_MCU_PAUSETWPOLL_CMD:
    case MCS_MCU_RESUMETWPOLL_CMD:
    case MCS_MCU_GETTWPOLLPARAM_REQ:
        ProcMcsMcuTWPollMsg(pcMsg);
        break;
    
    //hdu��ѯ
    case MCS_MCU_STARTHDUPOLL_CMD:           
    case MCS_MCU_STOPHDUPOLL_CMD:
    case MCS_MCU_PAUSEHDUPOLL_CMD:
    case MCS_MCU_RESUMEHDUPOLL_CMD:
    case MCS_MCU_GETHDUPOLLPARAM_REQ:
        ProcMcsMcuHduPollMsg(pcMsg);
        break;

	//hdu������ѯ
	case MCS_MCU_STARTHDUBATCHPOLL_REQ:
	case MCS_MCU_STOPHDUBATCHPOLL_REQ:
	case MCS_MCU_RESUMEHDUBATCHPOLL_REQ:
	case MCS_MCU_PAUSEHDUBATCHPOLL_REQ:
		ProcMcsMcuHduBatchPollMsg(pcMsg);
        break;

    // hdu������ѯ��ʱ����
	case MCUVC_HDUBATCHPOLLI_CHANGE_TIMER:
		ProcHduBatchPollChangeTimerMsg( pcMsg );
		break;
	
	// hdu��ͨ����ѯ��ʱ����
    case MCUVC_HDUPOLLING_CHANGE_TIMER:
        ProcHduPollingChangeTimerMsg(pcMsg);
        break;

    case MCUVC_TWPOLLING_CHANGE_TIMER:
        ProcTWPollingChangeTimerMsg(pcMsg);
        break;

    //�������---����
    case MCS_MCU_STARTROLLCALL_REQ:
    case MCS_MCU_STOPROLLCALL_REQ:
    case MCS_MCU_CHANGEROLLCALL_REQ:
        ProcMcsMcuRollCallMsg(pcMsg);
        break;
        
    /*----------------------------�ն˿���-------------------------------*/
    //˫��FastUpdate
    case MCUVC_SECVIDEO_FASTUPDATE_TIMER:
        ProcMcuMtSecVideoFastUpdateTimer(pcMsg);
        break;

	//�ն˿���---������Ҷ��ն�
	case MCS_MCU_CALLMT_REQ:            //�������̨����MCU�����ն�
	case MCU_MCU_REINVITEMT_REQ:        //�ϼ�MCU����MCU�����ն�
		ProcMcsMcuCallMtReq(pcMsg);
		break;
	case MT_MCU_CALLMTFAILURE_NOTIFY:
        ProcMtMcuCallFailureNotify(pcMsg);
		break;
	case MCS_MCU_DROPMT_REQ:            //�������̨�Ҷ��ն�����
	case MCU_MCU_DROPMT_REQ:            //�ϼ�MCU�Ҷ��ն�����
		ProcMcsMcuDropMtReq(pcMsg);
		break;
	case MCS_MCU_SETCALLMTMODE_REQ:     //�������̨����MCU�����ն˷�ʽ
		ProcMcsMcuSetCallMtModeReq(pcMsg);
		break;

	//�ն˿���---��ѯ�ն�״̬�����
	case MT_MCU_GETMTSTATUS_REQ:        //�ն˷���MCU�Ĳ�ѯ������ĳ���ն�״̬����
	case MCS_MCU_GETMTSTATUS_REQ:       //�����MCU��ѯ�ն�״̬
		ProcMcsMcuGetMtStatusReq(pcMsg);
		break;
 	case MCS_MCU_GETALLMTSTATUS_REQ:    //�����MCU��ѯ�����ն�״̬
		ProcMcsMcuGetAllMtStatusReq(pcMsg);
		break;
	case MCS_MCU_GETMTALIAS_REQ:        //�����MCU��ѯ�ն˱���
	case MT_MCU_GETMTALIAS_REQ:			//��ȡ�ն˱���
		ProcMtMcuGetMtAliasReq(pcMsg);
		break;
	case MCS_MCU_GETALLMTALIAS_REQ:		//�����MCU��ѯ�����ն˱���
		ProcMcsMcuGetAllMtAliasReq(pcMsg);
		break;
    case MCS_MCU_GETMTBITRATE_REQ:      //��ز�ѯ�ն�����
        ProcMcsMcuGetMtBitrateReq(pcMsg);
        break;
    case MCS_MCU_GETMTEXTINFO_REQ:      //��ز�ѯ�ն˵���չ��Ϣ:�汾�ŵ�
        ProcMcsMcuGetMtExtInfoReq(pcMsg);
        break;
		
	//�ն˿���---��������
	case MCS_MCU_MTCAMERA_CTRL_CMD:         //�������̨�����ն��������ͷ�˶�	
	case MCS_MCU_MTCAMERA_CTRL_STOP:        //�������̨�����ն��������ͷֹͣ�˶�
	case MCS_MCU_MTCAMERA_RCENABLE_CMD:     //�������̨�����ն������ң��ʹ��	
	case MCS_MCU_MTCAMERA_SAVETOPOS_CMD:    //�������̨�����ն����������ǰλ����Ϣ����ָ��λ��	
	case MCS_MCU_MTCAMERA_MOVETOPOS_CMD:    //�������̨�����ն������������ָ��λ��

	case MCS_MCU_SETMTVIDSRC_CMD:           //���Ҫ��MCU�����ն���ƵԴ

	case MT_MCU_MTCAMERA_CTRL_CMD:		    //��ϯ�����ն�����ͷ�ƶ�
	case MT_MCU_MTCAMERA_CTRL_STOP:		    //��ϯ�����ն�����ͷֹͣ�ƶ�
	case MT_MCU_MTCAMERA_RCENABLE_CMD:	    //��ϯ�����ն�����ͷң����ʹ��
	case MT_MCU_MTCAMERA_MOVETOPOS_CMD:	    //��ϯ�����ն�����ͷ������ָ��λ��
	case MT_MCU_MTCAMERA_SAVETOPOS_CMD:	    //��ϯ�����ն�����ͷ���浽ָ��λ��

    case MT_MCU_SELECTVIDEOSOURCE_CMD:           //���Ҫ��MCU�����ն���ƵԴ
		ProcMcsMcuCamCtrlCmd(pcMsg);
		break;
    case MT_MCU_VIDEOSOURCESWITCHED_CMD:
        ProcMtMcuVideoSourceSwitched(pcMsg);
        break;
	case MCU_MCU_FECC_CMD:
		ProcMMcuMcuCamCtrlCmd(pcMsg);
		break;
		
	case MCS_MCU_SETMTBITRATE_CMD:          //�������̨����MCU�����ն�����

	case MT_MCU_MTMUTE_CMD:                    //��ϯ�ն�����MCU�����ն˾�������
	case MT_MCU_MTDUMB_CMD:                    //��ϯ�ն�����MCU�����ն���������		
		ProcMcsMcuMtOperCmd(pcMsg);
		break;

	case MCS_MCU_SETMTVOLUME_CMD:			//�������MCU�����ն�����,zgc 12/26/2006
		ProcMcsMcuSetMtVolumeCmd(pcMsg);
		break;

	case MCS_MCU_MTAUDMUTE_REQ:             //���Ҫ��MCU�����ն˾���
       
        //�ն����þ������
    case MCS_MCU_MATRIX_GETALLSCHEMES_CMD:
    case MCS_MCU_MATRIX_GETONESCHEME_CMD:
    case MCS_MCU_MATRIX_SAVESCHEME_CMD:
    case MCS_MCU_MATRIX_SETCURSCHEME_CMD:
    case MCS_MCU_MATRIX_GETCURSCHEME_CMD:
	
        //�ն����þ���
    case MCS_MCU_EXMATRIX_GETINFO_CMD:      //��ȡ�ն����þ�������
    case MCS_MCU_EXMATRIX_SETPORT_CMD:      //�������þ������Ӷ˿ں�
    case MCS_MCU_EXMATRIX_GETPORT_REQ:      //�����ȡ���þ������Ӷ˿�
    case MCS_MCU_EXMATRIX_SETPORTNAME_CMD:  //�������þ������Ӷ˿���
    case MCS_MCU_EXMATRIX_GETALLPORTNAME_CMD://�����ȡ���þ�������ж˿���
        
        //�ն���չ��ƵԴ
    case MCS_MCU_GETVIDEOSOURCEINFO_CMD:
    case MCS_MCU_SETVIDEOSOURCEINFO_CMD:
        
        //�ն��л���չ��ƵԴ
    case MCS_MCU_SELECTEXVIDEOSRC_CMD:

		ProcMcsMcuMtOperReq(pcMsg);

		break;
		
    //�ն˿���---�������ն˵�������Ӧ��	

	case MT_MCU_MTCONNECTED_NOTIF:				//�ն���MCU�ɹ���������
		ProcMtMcuConnectedNotif(pcMsg);	
		break;
	case MT_MCU_MTDISCONNECTED_NOTIF:	        //�ն������Ҷ�MCU
	    ProcMtMcuDisconnectedNotif(pcMsg);	
		break;
	case MT_MCU_MTJOINCONF_NOTIF:               //�ն˳ɹ����֪ͨ
        ProcMtMcuMtJoinNotif(pcMsg);
		break;
	case MT_MCU_MTTYPE_NOTIF:                   //�ն�����֪ͨ��320����ʱ�Ķ���֪ͨ��
		ProcMtMcuMtTypeNotif( pcMsg );
		break;
	case MT_MCU_FLOWCONTROL_CMD:				//�ն�Ҫ��ı�������� - ������������
        ProcMtMcuFlowControlCmd(pcMsg, FALSE);
		break;
	case MT_MCU_FLOWCONTROLINDICATION_NOTIF:	//�ն�Ҫ��ı䷢������ - ��������ָʾ
        ProcMtMcuFlowControlIndication(pcMsg);
		break;
	case MT_MCU_MTSTATUS_NOTIF:					//�ն�״̬֪ͨ
		ProcMtMcuMtStatusNotif(pcMsg);	
		break;
	case MT_MCU_INVITEMT_ACK:					//�ն˽�������
		ProcMtMcuInviteMtAck(pcMsg);	
		break;
	case MT_MCU_INVITEMT_NACK:					//�ն˾ܾ�����
		ProcMtMcuInviteMtNack(pcMsg);	
		break;
	case MT_MCU_GETCHAIRMAN_REQ:
		ProcMtMcuGetChairmanReq(pcMsg);
		break;
	case MT_MCU_MTJOINCONF_REQ:					//�ն��������
		ProcMtMcuApplyJoinReq(pcMsg);
		break;
	case MT_MCU_APPLYCHAIRMAN_REQ:				//�ն�������ϯ
		ProcMtMcuApplyChairmanReq(pcMsg);
		break;	
	case MT_MCU_APPLYSPEAKER_NOTIF:				//�ն����뷢��
		ProcMtMcuApplySpeakerNotif(pcMsg);
		break;
	case MT_MCU_SENDMCMSG_CMD:					//����Ϣ�������MCU����̨
		ProcMtMcuSendMcMsgReq(pcMsg);
		break;
	case MT_MCU_OPENLOGICCHNNL_ACK:				//���߼�ͨ��Ӧ��
	case MT_MCU_OPENLOGICCHNNL_NACK:			//���߼�ͨ��Ӧ��
		ProcMtMcuOpenLogicChnnlRsp(pcMsg);
		break;
	case MT_MCU_OPENLOGICCHNNL_REQ:				//�ն˴��߼�ͨ������
	case MT_MCU_LOGICCHNNLOPENED_NTF:
		ProcMtMcuOpenLogicChnnlReq(pcMsg);
		break;
	case MT_MCU_CLOSELOGICCHNNL_NOTIF:
		ProcMtMcuCloseLogicChnnlNotify(pcMsg);
		break;
	case MT_MCU_MEDIALOOPON_REQ:
	case MT_MCU_MEDIALOOPOFF_CMD:
		ProcMtMcuMediaLoopOpr(pcMsg);
		break;
	case MT_MCU_JOINEDMTLIST_REQ:				//��ѯ����ն��б�����
		ProcMtMcuJoinedMtListReq(pcMsg);
		break;
	case MT_MCU_JOINEDMTLISTID_REQ:
		ProcMtMcuJoinedMtListIdReq(pcMsg);
		break;
	case MT_MCU_CAPBILITYSET_NOTIF:				//�ն˷���MCU��������֪ͨ
		ProcMtMcuCapSetNotif(pcMsg);
		break;
	case MT_MCU_MTALIAS_NOTIF:
		ProcMtMcuMtAliasNotif(pcMsg);
		break;
    case MCU_MT_ENTERPASSWORD_REQ: //���ڼ���
		ProcMtMcuEnterPwdReq(pcMsg);
		break;
	case MT_MCU_ENTERPASSWORD_ACK:				//�ն˻�Ӧ����
	case MT_MCU_ENTERPASSWORD_NACK:
        ProcMtMcuEnterPwdRsp(pcMsg);
		break;
	case MT_MCU_GETH239TOKEN_REQ:               //�ն˸�MCU�� ��ȡ H239���� Ȩ������
        ProcMtMcuGetH239TokenReq(pcMsg);
		break;
	case MT_MCU_OWNH239TOKEN_NOTIF:             //�ն˸�MCU�� ӵ�� H239���� Ȩ��֪ͨ
        ProcMtMcuOwnH239TokenNotify(pcMsg);
		break;
	case MT_MCU_RELEASEH239TOKEN_NOTIF:         //�ն˸�MCU�� �ͷ� H239���� Ȩ��֪ͨ
        ProcMtMcuReleaseH239TokenNotify(pcMsg);
		break;
	case POLY_MCU_GETH239TOKEN_ACK:			//��ȡPolyMCU��H239TOKEN ͬ��Ӧ��
	case POLY_MCU_GETH239TOKEN_NACK:		//��ȡPolyMCU��H239TOKEN �ܾ�Ӧ��
	case POLY_MCU_OWNH239TOKEN_NOTIF:		//PolyMCU֪ͨ��ǰ��TOKEN��ӵ����
	case POLY_MCU_RELEASEH239TOKEN_CMD:		//PolyMCU�ͷ�H329TOKEN ����
		ProcPolyMCUH239Rsp(pcMsg);
		break;
	//�ն����ݻ������ --- �����ն˵�Ӧ��
	case DCSSSN_MCU_ADDMT_ACK:					//DCS�ն�ɾ��Ӧ��
	case DCSSSN_MCU_ADDMT_NACK:
		ProcDcsMcuAddMtRsp(pcMsg);
		break;
	case DCSSSN_MCU_DELMT_ACK:					//DCS�ն�����Ӧ��
	case DCSSSN_MCU_DELMT_NACK:
		ProcDcsMcuDelMtRsp(pcMsg);
		break;
	case DCSSSN_MCU_MTJOINED_NOTIF:				//DCS�ն�����֪ͨ
		ProcDcsMcuMtJoinedNtf(pcMsg);
		break;
	case DCSSSN_MCU_MTLEFT_NOTIF:				//DCS�ն�����֪ͨ
		ProcDcsMcuMtLeftNtf(pcMsg);
		break;
	case MCU_DCSCONNCETED_NOTIF:				//�����ն˵�״̬֪ͨ
	case MCU_DCSDISCONNECTED_NOTIF:
		ProcDcsMcuStatusNotif(pcMsg);
		break;

	case MT_MCU_RELEASEMT_REQ:
		ProcMtMcuReleaseMtReq(pcMsg);
		break;

	//������δ������ն���Ϣ
	case MT_MCU_FREEZEPIC_CMD:                 //�ն˷���MCU����ͼ������
		break;
	case MT_MCU_FASTUPDATEPIC_CMD:             //�ն˷���MCU���ٸ���ͼ��
		ProcMtMcuFastUpdatePic(pcMsg);
		break;

	case MT_MCU_GETMTSTATUS_ACK:
	case MT_MCU_GETMTSTATUS_NACK:
    case MT_MCU_GETBITRATEINFO_ACK:
    case MT_MCU_GETBITRATEINFO_NACK:
    case MT_MCU_GETBITRATEINFO_NOTIF:
    case MT_MCU_GETMTVERID_ACK:
    case MT_MCU_GETMTVERID_NACK:
        
	   ProcMtMcuOtherMsg(pcMsg);	
	   break;

//���þ���
    case MT_MCU_MATRIX_ALLSCHEMES_NOTIF:    
    case MT_MCU_MATRIX_ONESCHEME_NOTIF:
    case MT_MCU_MATRIX_SAVESCHEME_NOTIF:
    case MT_MCU_MATRIX_SETCURSCHEME_NOTIF:
    case MT_MCU_MATRIX_CURSCHEME_NOTIF:

 //���þ���    
    case MT_MCU_EXMATRIXINFO_NOTIFY:         
    case MT_MCU_EXMATRIX_GETPORT_ACK:        
    case MT_MCU_EXMATRIX_GETPORT_NACK:       
    case MT_MCU_EXMATRIX_GETPORT_NOTIF:
    case MT_MCU_EXMATRIX_PORTNAME_NOTIF:        
    case MT_MCU_EXMATRIX_ALLPORTNAME_NOTIF:    
        
//��չ��ƵԴ
    case MT_MCU_ALLVIDEOSOURCEINFO_NOTIF:
    case MT_MCU_VIDEOSOURCEINFO_NOTIF:

		ProcMtMcuMatrixMsg(pcMsg);
		break;

        //�ն˴���ָʾ
    case MT_MCU_BANDWIDTH_NOTIF:
        ProcMtMcuBandwidthNotif(pcMsg);
        break;

	//�������---¼�������
    case MCU_RECCONNECTED_NOTIF:
        ProcMcuRecConnectedNotif(pcMsg);
        break;
    case MCU_RECDISCONNECTED_NOTIF:	//¼�������֪ͨ
		ProcMcuRecDisconnectedNotif(pcMsg);
		break;
	case MCS_MCU_STARTREC_REQ:              //�����MCU����ʼ¼��
		ProcMcsMcuStartRecReq(pcMsg);
		break;
	case MCS_MCU_PAUSEREC_REQ:              //�����MCU��ͣ¼��
		ProcMcsMcuPauseRecReq(pcMsg);
		break;
	case MCS_MCU_RESUMEREC_REQ:             //�����MCU�ָ�¼��
		ProcMcsMcuResumeRecReq(pcMsg);
		break;
	case MCS_MCU_STOPREC_REQ:               //�����MCUֹͣ¼��
		ProcMcsMcuStopRecReq(pcMsg);
		break;
	case MCS_MCU_CHANGERECMODE_REQ:
		ProcMcsMcuChangeRecModeReq(pcMsg);
		break;
	case MCS_MCU_STARTPLAY_REQ:             //��ؿ�ʼ��������
		ProcMcsMcuStartPlayReq(pcMsg);
		break;
	case MCS_MCU_PAUSEPLAY_REQ:             //�����ͣ��������
		ProcMcsMcuPausePlayReq(pcMsg);
		break;
	case MCS_MCU_RESUMEPLAY_REQ:            //��ػָ���������
		ProcMcsMcuResumePlayReq(pcMsg);
		break;
	case MCS_MCU_STOPPLAY_REQ:              //���ֹͣ��������
		ProcMcsMcuStopPlayReq(pcMsg);
		break;
    case MCS_MCU_GETRECPROG_CMD:            //��ز�ѯ¼�����
    case MCS_MCU_GETPLAYPROG_CMD:           //��ز�ѯ�������
        ProcMcsMcuGetRecPlayProgCmd(pcMsg);
        break;
	case MCS_MCU_FFPLAY_REQ:                //��ؿ����������
	case MCS_MCU_FBPLAY_REQ:                //��ؿ��˷�������
	case MCS_MCU_SEEK_REQ:                  //��ص��������������
		ProcMcsMcuSeekReq(pcMsg);
		break;
	case MCUVC_RECPLAY_WAITMPACK_TIMER:		//�ȴ�����������ʱ����ʱ,zgc, 2008-03-26
		ProcRecPlayWaitMpAckTimer(pcMsg);
		break;
	case REC_MCU_PLAYCHNSTATUS_NOTIF:		//¼��������ŵ�״̬֪ͨ
		ProcRecMcuPlayChnnlStatusNotif(pcMsg);
		break;
	case REC_MCU_RECORDCHNSTATUS_NOTIF:		//¼���¼���ŵ�״̬֪ͨ
		ProcRecMcuRecChnnlStatusNotif(pcMsg);
		break;
	case REC_MCU_RECORDPROG_NOTIF:			//��ǰ¼�����֪ͨ
	case REC_MCU_PLAYPROG_NOTIF:			//��ǰ�������֪ͨ
		ProcRecMcuProgNotif(pcMsg);
		break;    
	case REC_MCU_NEEDIFRAME_CMD:            //¼�������ؼ�֡
        ProcRecMcuNeedIFrameCmd(pcMsg);     
		break;

     //�������---��������������
    case BAS_MCU_STARTADAPT_ACK:			//��������Ӧ��
	case BAS_MCU_STARTADAPT_NACK:			//ֹͣ����Ӧ��
		ProcBasMcuRsp(pcMsg);
		break;

    case HDBAS_MCU_STARTADAPT_ACK:      //������������Ӧ��
    case HDBAS_MCU_STARTADAPT_NACK:     //������������ܾ�
        ProcHdBasMcuRsp(pcMsg);
        break;

	case MCU_BASCONNECTED_NOTIF:			//��������Ǽ�֪ͨ
		ProcMcuBasConnectedNotif(pcMsg);
		break;
	case MCU_BASDISCONNECTED_NOTIF:			//�����������֪ͨ
		ProcMcuBasDisconnectedNotif(pcMsg);
		break;
	
	case VMP_MCU_VMPSTATUS_NOTIF:
    case BAS_MCU_BASSTATUS_NOTIF:
    case HDBAS_MCU_BASSTATUS_NOTIF:
        break;
    
    case HDBAS_MCU_CHNNLSTATUS_NOTIF:
        ProcHdBasChnnlStatusNotif( pcMsg );
        break;

    case HDU_MCU_CHNNLSTATUS_NOTIF:
		ProcHduMcuChnnlStatusNotif( pcMsg );
		break;

	case MCUVC_WAITBASRRSP_TIMER:				//�ȴ�����������Ӧ��ʱ
	case MCUVC_WAITBASRRSP_TIMER+1:			//�ȴ�����������Ӧ��ʱ
	case MCUVC_WAITBASRRSP_TIMER+2:			//�ȴ�����������Ӧ��ʱ
    case MCUVC_WAITBASRRSP_TIMER+3:
    case MCUVC_WAITBASRRSP_TIMER+4:
		ProcBasStartupTimeout(pcMsg);
		break;
    case BAS_MCU_NEEDIFRAME_CMD:
        ProcBasMcuCommand(pcMsg);
        break;

	case HDU_MCU_NEEDIFRAME_CMD:
		ProcHduMcuNeedIFrameCmd(pcMsg);
		break;
		
    case MCUVC_SENDFLOWCONTROL_TIMER:
        ProcSendFlowctrlToDSMtTimeout(pcMsg);
		break;
		
	case MCU_SMCUOPENDVIDEOCHNNL_TIMER:
		ProcSmcuOpenDVideoChnnlTimer(pcMsg);
		break;

	case MCU_PRSCONNECTED_NOTIF:		//����MCU�ڲ�����ʵ��֪ͨPRS�����ɹ�
		ProcPrsConnectedNotif(pcMsg);
		break;
	case MCU_PRSDISCONNECTED_NOTIF:		//����MCU�ڲ�����ʵ��֪ͨPRS����
		ProcPrsDisconnectedNotif(pcMsg);
		break;
	case PRS_MCU_SETSRC_ACK:			//PRS��MCU������ϢԴȷ��
	case PRS_MCU_SETSRC_NACK:			//PRS��MCU������ϢԴ�ܾ�
//	case PRS_MCU_ADDRESENDCH_ACK:		//PRS��MCUֹͣ����ȷ��
//	case PRS_MCU_ADDRESENDCH_NACK:		//PRS��MCUֹͣ�����ܾ�
//	case PRS_MCU_REMOVERESENDCH_ACK:	//PRS��MCU�ı临�ϲ���ȷ��
//	case PRS_MCU_REMOVERESENDCH_NACK:	//PRS��MCU�ı临�ϲ����ܾ�
	case PRS_MCU_REMOVEALL_ACK:			//PRS��MCU�ı临�ϲ���ȷ��
	case PRS_MCU_REMOVEALL_NACK:		//PRS��MCU�ı临�ϲ����ܾ�
		ProcPrsMcuRsp(pcMsg);
		break;

	case MCS_MCU_STOPSWITCHMC_REQ:
	case MCU_MCSDISCONNECTED_NOTIF:
		ProcMcStopSwitch(pcMsg);
		break;

	case MCS_MCU_STOP_SWITCH_TW_REQ:
		ProcMcsMcuStopSwitchTWReq(pcMsg);
		break;

    case MCU_TVWALLCONNECTED_NOTIF:
        ProcTvwallConnectedNotif(pcMsg);
        break;
	case MCU_TVWALLDISCONNECTED_NOTIF:
		ProcTvwallDisconnectedNotif(pcMsg);
		break;	
		
    //4.6 jlb
    case HDU_MCU_STATUS_NOTIF:
		ProcHduMcuStatusNotif( pcMsg );  
		break;

	case MCU_HDUCONNECTED_NOTIF:                    
		ProcHduConnectedNotif(pcMsg);
		break;

	case MCU_HDUDISCONNECTED_NOTIF:
		ProcHduDisconnectedNotif(pcMsg);
		break;


    //�������ն��������Ӧ��һ�㴦��	
	case REC_MCU_PAUSEREC_ACK:			//��ͣ¼��Ӧ��
	case REC_MCU_RESUMEREC_ACK:			//�ָ�¼��Ӧ��	
	case REC_MCU_CHANGERECMODE_ACK:		//�ı�¼��ģʽ
	case REC_MCU_PAUSEPLAY_ACK:			//��ͣ����Ӧ��
	case REC_MCU_RESUMEPLAY_ACK:		//�ָ�����Ӧ��
	case REC_MCU_STOPPLAY_ACK:			//ֹͣ����Ӧ��
	case REC_MCU_FFPLAY_ACK:			//������Ӧ��
	case REC_MCU_FBPLAY_ACK:			//�������Ӧ��
	case REC_MCU_SEEK_ACK:				//������ȵ���Ӧ��
		ProcCommonOperAck(pcMsg);
		break;

    case REC_MCU_STARTREC_ACK:			//��ʼ¼��Ӧ��
    case REC_MCU_STOPREC_ACK:			//ֹͣ¼��Ӧ��
	case REC_MCU_STARTPLAY_ACK:			//��ʼ����Ӧ��
        ProcRecMcuResp(pcMsg);
        break;

	//�������ն�������ľܾ�Ӧ��һ�㴦��	
    case REC_MCU_STARTREC_NACK:			//��ʼ¼��Ӧ��
	case REC_MCU_PAUSEREC_NACK:			//��ͣ¼��Ӧ��
	case REC_MCU_RESUMEREC_NACK:		//�ָ�¼��Ӧ��
	case REC_MCU_STOPREC_NACK:			//ֹͣ¼��Ӧ��
	case REC_MCU_CHANGERECMODE_NACK:	//�ı�¼��ģʽ
	case REC_MCU_STARTPLAY_NACK:		//��ʼ����Ӧ��
	case REC_MCU_PAUSEPLAY_NACK:		//��ͣ����Ӧ��
	case REC_MCU_RESUMEPLAY_NACK:		//�ָ�����Ӧ��
	case REC_MCU_STOPPLAY_NACK:			//ֹͣ����Ӧ��
	case REC_MCU_FFPLAY_NACK:			//������Ӧ��
	case REC_MCU_FBPLAY_NACK:			//�������Ӧ��
	case REC_MCU_SEEK_NACK:				//������ȵ���Ӧ��
		ProcCommonOperNack(pcMsg);
		break;
	
	//����ϵͳ��Ϣ
	case MT_MCU_STARTMTSELME_REQ:
	case MT_MCU_STOPMTSELME_CMD:
	case MT_MCU_STARTBROADCASTMT_REQ:
	case MT_MCU_STOPBROADCASTMT_CMD:
	case MT_MCU_GETMTSELSTUTS_REQ:
		ProcBuildCaseSpecialMessage(pcMsg);
        break;

	//Mp ��Ϣ
	case MP_MCU_REG_REQ:
	case MP_MCU_ADDSWITCH_ACK:
	case MP_MCU_ADDSWITCH_NACK:
	case MP_MCU_REMOVESWITCH_ACK:
	case MP_MCU_REMOVESWITCH_NACK:
	case MP_MCU_GETSWITCHSTATUS_ACK:
	case MP_MCU_GETSWITCHSTATUS_NACK:
	case MP_MCU_ADDMULTITOONESWITCH_ACK:
	case MP_MCU_ADDMULTITOONESWITCH_NACK:
	case MP_MCU_REMOVEMULTITOONESWITCH_ACK:
	case MP_MCU_REMOVEMULTITOONESWITCH_NACK:
	case MP_MCU_STOPMULTITOONESWITCH_ACK:
	case MP_MCU_STOPMULTITOONESWITCH_NACK:
	case MP_MCU_ADDRECVONLYSWITCH_ACK:
	case MP_MCU_ADDRECVONLYSWITCH_NACK:
	case MP_MCU_REMOVERECVONLYSWITCH_ACK:
	case MP_MCU_REMOVERECVONLYSWITCH_NACK:
	case MP_MCU_SETRECVSWITCHSSRC_ACK:
	case MP_MCU_SETRECVSWITCHSSRC_NACK:
        // guzh [3/29/2007]
    case MP_MCU_ADDBRDSRCSWITCH_ACK:
    case MP_MCU_ADDBRDDSTSWITCH_ACK:
    case MP_MCU_ADDBRDSRCSWITCH_NACK:
    case MP_MCU_ADDBRDDSTSWITCH_NACK:
    case MP_MCU_REMOVEBRDSRCSWITCH_ACK:
    case MP_MCU_REMOVEBRDSRCSWITCH_NACK:
    case MP_MCU_BRDSRCSWITCHCHANGE_NTF:    // �㲥Դ��ʵ���Ƴ�֪ͨ         
		ProcMpMessage(pcMsg);
		break;

	//����Mp����
	case MCU_MP_DISCONNECTED_NOTIFY:
		ProcMpDissconnected(pcMsg);
		break;

	//����MtAdp����
	case MCU_MTADP_DISCONNECTED_NOTIFY:
		ProcMtAdpDissconnected(pcMsg);
		break;

	case MT_MCU_MSD_NOTIF:
		ProcMtMcuMsdRsp(pcMsg);
		break;
	case MCUVC_SCHEDULED_CHECK_TIMER:			//ԤԼ���鶨ʱ���
		ProcTimerScheduledCheck(pcMsg);
		break;
	case MCUVC_ONGOING_CHECK_TIMER:			//�����л��鶨ʱ���
		ProcTimerOngoingCheck(pcMsg);	
		break;
	case MCUVC_INVITE_UNJOINEDMT_TIMER:		//��ʱ����δ����ն�
		ProcTimerInviteUnjoinedMt(pcMsg);
		break;
	case MCUVC_REFRESH_MCS_TIMER:
		ProcTimerRefreshMcs(pcMsg);
		break;
	case MCUVC_MCUSRC_CHECK_TIMER:
		ProcTimerMcuSrcCheck(pcMsg);
		break;
	case MCUVC_RECREATE_DATACONF_TIMER:
		ProcTimerRecreateDataConf(pcMsg);
		break;
    case MCUVC_WAIT_CASCADE_CHANNEL_TIMER:
        ProcTimerReopenCascadeChannel();
        break;
	case MT_MCU_REGISTERGK_ACK:         //��GK��ע��ɹ�
		ProcConfRegGkAck(pcMsg);
		break;
	case MT_MCU_REGISTERGK_NACK:		//��GK��ע��ʧ��
		ProcConfRegGkNack(pcMsg);
		break;
    
    case MT_MCU_CONF_STARTCHARGE_ACK:
    case MT_MCU_CONF_STARTCHARGE_NACK:
	case MT_MCU_CONF_CHARGEEXP_NOTIF:
    case MT_MCU_CONF_STOPCHARGE_ACK:
    case MT_MCU_CHARGE_REGGK_NOTIF:
        ProcGKChargeRsp(pcMsg);
        break;

    case MCUVC_CHANGE_VMPCHAN_SPEAKER_TIMER:
    case MCUVC_CHANGE_VMPCHAN_CHAIRMAN_TIMER:
    case MCUVC_CHANGE_VMPPARAM_TIMER:
        ProcTimerChangeVmpParam(pcMsg);
        break;

	case MT_MCU_UNREGISTERGK_ACK:       //��GK��ע���ɹ�
	case MT_MCU_UNREGISTERGK_NACK:      //��GK��ע��ʧ��
	case VMP_MCU_SETCHANNELBITRATE_ACK: //�ı�����Ӧ��
	case VMP_MCU_SETCHANNELBITRATE_NACK://�ı����ʾܾ�
	case BAS_MCU_STOPADAPT_ACK:			//��������ܾ�
	case BAS_MCU_STOPADAPT_NACK:		//ֹͣ����ܾ�
    case HDBAS_MCU_STOPADAPT_ACK:       //ֹͣ��������Ӧ��
    case HDBAS_MCU_STOPADAPT_NACK:      //ֹͣ��������ܾ�
	case MCU_MCU_REINVITEMT_ACK:
	case MCU_MCU_SETIN_ACK:
	case MCU_MCU_SETIN_NACK:
	case MCU_MCU_DROPMT_ACK:
	case MCU_MCU_DROPMT_NACK:
	case MCU_MCU_DELMT_ACK:
	case MCU_MCU_DELMT_NACK:
		break;

        //N+1ע��ɹ������ع�����Ҫͬ��������Ϣ
    case NPLUS_VC_DATAUPDATE_NOTIF:
        ProcNPlusConfDataUpdate(pcMsg);
        break;
	case REC_MCU_RECSTATUS_NOTIF:
		break;
	case EV_MCU_OPENSECVID_TIMER:
		ProcPolycomSecVidChnnl(pcMsg);
		break;
		
	case TVWALL_MCU_STATUS_NOTIF:
		break;

    //����MODEM����
    case MODEM_MCU_REG_ACK:
    case MODEM_MCU_REG_NACK:
        ProcModemConfRegRsp( pcMsg );
		break;

	case MODEM_MCU_ERRSTAT_NOTIF:
	case MODEM_MCU_RIGHTSTAT_NOTIF:
		ProcModemMcuStatusNotif( pcMsg );
		break;

/*-------------------------------------------------------------------------------------*/
	//VCSͨ��ģ�崴��
	case MCU_SCHEDULE_VCSCONF_START:
	case VCS_MCU_VCMT_REQ:
	case VCS_MCU_VCMODE_REQ:
	case VCS_MCU_CHGVCMODE_REQ:
	case VCS_MCU_MUTE_REQ:
	case VCS_MCU_GROUPCALLMT_REQ:
	case VCS_MCU_GROUPDROPMT_REQ:
	case VCS_MCU_STARTCHAIRMANPOLL_REQ:
	case VCS_MCU_STOPCHAIRMANPOLL_REQ:
	case VCS_MCU_ADDMT_REQ:
	case VCS_MCU_DELMT_REQ:
		ProcVcsMcuMsg(pcMsg);
		break;
	case MCUVC_VCMTOVERTIMER_TIMER:
		ProcVCMTOverTime(pcMsg);
		break;
	case MCUVC_VCS_CHAIRPOLL_TIMER:
		ProcChairPollTimer(pcMsg);
		break;
    case VCS_MCU_RELEASEMT_ACK:
	case VCS_MCU_RELEASEMT_NACK:
		ProcVcsMcuRlsMtMsg(pcMsg);
		break;
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in InstanceEntry()!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ) );
		break;
	}

	//�������ݵ���ʱ��[MSMag <->McuVc]ģ�黥�������� 2005-12-15
	g_cMSSsnApp.LeaveMSSynLock(AID_MCU_VC);

	return;
}

/*====================================================================
    ������      :DaemonInstanceEntry
    ����        :Daemonʵ�����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg,  �������Ϣ
				  CApp* pApp ,����Ӧ��ָ��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/05/26    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::DaemonInstanceEntry( CMessage* const pcMsg, CApp* pcApp )
{
	if( NULL == pcMsg )
	{
		ConfLog( FALSE, "[CMcuVcInst] The received msg's pointer in the msg DaemonEntry is NULL!");
		return;
	}

	//�������ݵ���ʱ��[MSMag <->McuVc]ģ�黥�������� 2005-12-15
	g_cMSSsnApp.EnterMSSynLock(AID_MCU_VC);

	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	switch ( pcMsg->event )
	{
	case OSP_POWERON:
		DaemonProcPowerOn( pcMsg );
		break;

    case MCS_MCU_CREATECONF_REQ:
        DaemonProcMcsMcuCreateConfReq(pcMsg);
        break;

	case MCUVC_WAIT_MPREG_TIMER:              // ��ʱ�ȴ�Mpע��
		ProcTimerWaitMpRegCheck(pcMsg);
		break;

	case MCU_MSSTATE_EXCHANGE_NTF:
		DaemonProcMcuMSStateNtf(pcMsg);       //���ð�����Ϊ���ð�ʱ ֪ͨ[McuVc]ģ��������״̬���
		break;
	case MT_MCU_CREATECONF_REQ:
		DaemonProcMtMcuCreateConfReq(pcMsg);
		break;
	case MCS_MCU_LISTALLCONF_REQ:             //�г���MCU�����л�����Ϣ
		DaemonProcMcsMcuListAllConfReq( pcMsg, pcApp );
		break;   
	case MCS_MCU_GETMCUSTATUS_CMD:
	case MCS_MCU_GETMCUSTATUS_REQ:	          //��ѯMCU״̬
		DaemonProcMcsMcuGetMcuStatusReq(pcMsg, pcApp);
		break;
	case MCU_APPTASKTEST_REQ:			      //GUARD Probe Message
		DaemonProcAppTaskRequest(pcMsg);
		break;
	case MCU_EQPCONNECTED_NOTIF:	          //����Ǽǳɹ�
		DaemonProcMcuEqpConnectedNotif(pcMsg);
		break;
	case MCU_EQPDISCONNECTED_NOTIF:           //�������
		DaemonProcMcuEqpDisconnectedNotif(pcMsg);
		break;
	case MCU_DCSCONNCETED_NOTIF:			  //DCS�Ǽǳɹ�
		DaemonProcMcuDcsConnectedNtf(pcMsg);  
		break;
	case MCU_DCSDISCONNECTED_NOTIF:			  //DCS����
		DaemonProcMcuDcsDisconnectedNtf(pcMsg);
		break;
	case MCU_MCSCONNECTED_NOTIF:		      //��صǼǳɹ�
		DaemonProcMcuMcsConnectedNotif(pcMsg);
		break;
	case MCU_MCSDISCONNECTED_NOTIF:           //��ض���
		DaemonProcMcuMcsDisconnectedNotif(pcMsg);
		break;
    case MCS_MCU_CREATECONF_BYTEMPLATE_REQ: //�������̨��MCU�ϰ�����ģ�崴��һ������(����ģ���ɼ�ʱ����)
    case MCS_MCU_CREATESCHCONF_BYTEMPLATE_REQ:  //����ģ�崴��ԤԼ����
	case VCS_MCU_CREATECONF_BYTEMPLATE_REQ:
        DaemonProcMcsMcuCreateConfByTemplateReq(pcMsg);        
        break;       
    case MCS_MCU_CREATETEMPLATE_REQ:            //����ģ��
    case MCS_MCU_MODIFYTEMPLATE_REQ:            //�޸�ģ��
    case MCS_MCU_DELTEMPLATE_REQ:               //ɾ��ģ��
        DaemonProcMcsMcuTemplateOpr(pcMsg);
        break;

	case MCS_MCU_SAVECONFTOTEMPLATE_REQ:	//�������̨���󽫵�ǰ���鱣��Ϊ����ģ��(Ԥ��), zgc, 2007/04/20
		DaemonProcMcsMcuSaveConfToTemplateReq(pcMsg);
		break;

    case MT_MCU_REGISTERGK_ACK:				//��GK��ע��ɹ�
    case MT_MCU_REGISTERGK_NACK:			//��GK��ע��ʧ��
    case MT_MCU_UNREGISTERGK_ACK:			//��GK��ע���ɹ�
    case MT_MCU_UNREGISTERGK_NACK:			//��GK��ע��ʧ��
        DaemonProcGkRegRsp(pcMsg);
        break;

    case MCU_CREATECONF_NPLUS:
        DaemonProcCreateConfNPlus(pcMsg);
        break;

    case MT_MCU_CONF_STOPCHARGE_ACK:
    case MT_MCU_CONF_STOPCHARGE_NACK:
    case MT_MCU_CONF_STARTCHARGE_ACK:
    case MT_MCU_CONF_STARTCHARGE_NACK:
	case MT_MCU_CONF_CHARGEEXP_NOTIF:
    case MT_MCU_CHARGE_REGGK_NOTIF:
        DaemonProcGKChargeRsp(pcMsg);
        break;

    case MCU_MCUREREGISTERGK_NOITF:          //��GK������ע��mcu����
        DaemonProcMcuReRegisterGKNtf(pcMsg);
        break;

    case HDBAS_MCU_BASSTATUS_NOTIF:           //����������״̬֪ͨ
	case REC_MCU_RECSTATUS_NOTIF:		      //¼���״̬֪ͨ
	case MIXER_MCU_MIXERSTATUS_NOTIF:	      //������״̬֪ͨ��Ϣ
	case BAS_MCU_BASSTATUS_NOTIF:		      //������״̬֪ͨ
	case VMP_MCU_VMPSTATUS_NOTIF:             //����ϳ���״̬֪ͨ
    case VMPTW_MCU_VMPTWSTATUS_NOTIF:         //���ϵ���ǽ״̬֪ͨ
	case TVWALL_MCU_STATUS_NOTIF:             //����ǽ״̬֪ͨ
	case PRS_MCU_PRSSTATUS_NOTIF:			  //VMP��MCU��״̬�ϱ�
	case PRS_MCU_SETSRC_NOTIF:			      //PRS��MCU�ı�����ϢԴ���
//	case PRS_MCU_ADDRESENDCH_NOTIF:		      //PRS��MCU��ֹͣ���
//	case PRS_MCU_REMOVERESENDCH_NOTIF:	      //PRS��MCU��״̬�ı���
	case PRS_MCU_REMOVEALL_NOTIF:		      //PRS��MCU��״̬�ı���
    case HDU_MCU_STATUS_NOTIF:                //�������ǽ״̬֪ͨ
		DaemonProcPeriEqpMcuStatusNotif(pcMsg);
		break;

	//case MIXER_MCU_GRPSTATUS_NOTIF:			  //������״̬֪ͨ
	//	break;

	case BAS_MCU_CHNNLSTATUS_NOTIF://������ͨ��״̬֪ͨ
		ProcBasChnnlStatusNotif( pcMsg);
		break;

    case HDBAS_MCU_CHNNLSTATUS_NOTIF://����������ͨ��״̬֪ͨ
        DaemonProcHDBasChnnlStatusNotif( pcMsg );
		break;

    case HDU_MCU_CHNNLSTATUS_NOTIF:
        DaemonProcHduMcuChnnlStatusNotif( pcMsg );
		break;

	case MCS_MCU_STOPSWITCHMC_REQ:		      //�������ֹ̨ͣ��������
		DaemonProcMcsMcuStopSwitchMcReq(pcMsg);
		break;
	case MCS_MCU_STOP_SWITCH_TW_REQ:	      //ֹͣ�����ǽ��������
		DaemonProcMcsMcuStopSwitchTWReq(pcMsg);
		break;
		
	case MCS_MCU_LISTALLRECORD_REQ:         //����б�����
		ProcMcsMcuListAllRecordReq(pcMsg);
		break;
	case REC_MCU_LISTALLRECORD_NOTIF:		//�г�¼��������м�¼Ӧ��
		ProcRecMcuListAllRecNotif(pcMsg);
		break;
	case MCS_MCU_DELETERECORD_REQ:          //�������ɾ���ļ�
		ProcMcsMcuDeleteRecordReq(pcMsg);
		break;
    case MCS_MCU_RENAMERECORD_REQ:          //�����������ļ���
        ProcMcsMcuRenameRecordReq(pcMsg);
        break;
	case MCS_MCU_PUBLISHREC_REQ:            //����¼������
		ProcMcsMcuPublishRecReq(pcMsg);
		break;
	case MCS_MCU_CANCELPUBLISHREC_REQ:		//��������¼������
		ProcMcsMcuCancelPublishRecReq(pcMsg);
		break;

	case REC_MCU_RECORDCHNSTATUS_NOTIF:	      //¼���¼���ŵ�״̬֪ͨ
	case REC_MCU_PLAYCHNSTATUS_NOTIF:	      //¼��������ŵ�״̬֪ͨ
		DaemonProcRecMcuChnnlStatusNotif(pcMsg);
		break;

	case REC_MCU_RECORDPROG_NOTIF:		      //��ǰ¼�����֪ͨ
	case REC_MCU_PLAYPROG_NOTIF:		      //��ǰ�������֪ͨ
		DaemonProcRecMcuProgNotif(pcMsg);
		break;
	                                           //!¼���ļ�����Ӧ����Ϣ����
		
	case REC_MCU_LISTALLRECORD_NACK:    //¼����ܾ�¼���б�����
	case REC_MCU_PUBLISHREC_NACK:		//����¼��
    case REC_MCU_CANCELPUBLISHREC_NACK: //ȡ������¼��
	case REC_MCU_DELETERECORD_NACK:     //ɾ��¼���¼�ܾ���Ϣ
	case REC_MCU_RENAMERECORD_NACK:     //����¼���¼�ܾ���Ϣ
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		break;
		
	case REC_MCU_PUBLISHREC_ACK:		//����¼��
    case REC_MCU_CANCELPUBLISHREC_ACK:  //ȡ������¼��
	case REC_MCU_DELETERECORD_ACK:      //ɾ��¼���¼Ӧ����Ϣ
	case REC_MCU_RENAMERECORD_ACK:      //����¼���¼Ӧ����Ϣ
	case REC_MCU_LISTALLRECORD_ACK:     //¼�����Ӧ��(���)��Ϣ
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		break;


	case MCS_MCU_GETMCUPERIEQPSTATUS_REQ:     //��ѯMCU����״̬
	case MCS_MCU_GETRECSTATUS_REQ:	          //��ѯ¼���״̬����
	case MCS_MCU_GETMIXERSTATUS_REQ:	      //��ѯ������״̬����
    case MCS_MCU_GETPERIDCSSTATUS_REQ:        //��ѯDCS״̬����
	//case MCS_MCU_GETBASSTATUS_REQ:	      //��ѯ����������״̬����
		DaemonProcMcsMcuGetPeriEqpStatusReq(pcMsg);
		break;
		
	case REC_MCU_EXCPT_NOTIF:			      //¼����쳣֪ͨ��Ϣ
		DaemonProcCommonNotif(pcMsg);
		break;

	case MT_MCU_MTJOINCONF_REQ:
		DaemonProcMtMcuApplyJoinReq(pcMsg);
		break;
		
	//MP Message
	case MCU_MP_DISCONNECTED_NOTIFY:          //Ҫ����Mp����
	case MP_MCU_REG_REQ:
        g_cMpManager.ProcMpToMcuMessage(pcMsg);
        break;
        
    //case MP_MCU_FLUXOVERRUN_NOTIFY:
    case MP_MCU_FLUXSTATUS_NOTIFY:
        DaemonProcMpFluxNotify(pcMsg);
        break;

	//Mtadp Message
	case MCU_MTADP_DISCONNECTED_NOTIFY:       //Ҫ����MtAdp����
	case MTADP_MCU_REGISTER_REQ:
		g_cMpManager.ProcMtAdpToMcuMessage(pcMsg);
		break;
	
	//���ݻ��鳷������Ϣ����
	case DCSSSN_MCU_RELEASECONF_ACK:
	case DCSSSN_MCU_RELEASECONF_NACK:
	case DCSSSN_MCU_CONFRELEASED_NOTIF:
		DaemonProcDcsMcuReleaseConfRsp(pcMsg);
		break;
        
    //N+1 ��������ע�ᱸ��ʧ��
    case MCU_NPLUS_REG_NACK:
        DaemonProcNPlusRegBackupRsp(pcMsg);
        break;

    case MCU_NMS_SENDNMSMSG_CMD:
        DaemonProcSendMsgToNms(pcMsg);
        break;

	//�����ȱ��ݵ�Ԫ���Խӿ�
	case EV_TEST_TEMPLATEINFO_GET_REQ:
	case EV_TEST_CONFINFO_GET_REQ:
	case EV_TEST_CONFMTLIST_GET_REQ:
	case EV_TEST_ADDRBOOK_GET_REQ:
	case EV_TEST_MCUCONFIG_GET_REQ:
		DaemonProcUnitTestMsg(pcMsg);
		break;
	case MT_MCU_CALLFAIL_HDIFULL_NOTIF:
		DaemonProcHDIFullNtf(pcMsg);
		break;

    case MODEM_MCU_REG_ACK:
        DaemonProcModemReg( pcMsg );
		break;

	case MCUVC_CONFINFO_MULTICAST_TIMER:
		DaemonProcTimerMultiCast( pcMsg, pcApp );
		break;

	default:
		ConfLog( FALSE, "[CMcuVcInst] Wrong message %u(%s) received in DaemonEntry()!\n", 
			             pcMsg->event, ::OspEventDesc( pcMsg->event ) );
		break;
	}

	//�������ݵ���ʱ��[MSMag <->McuVc]ģ�黥�������� 2005-12-15
	g_cMSSsnApp.LeaveMSSynLock(AID_MCU_VC);

	return;
}

/*=============================================================================
    �� �� ���� DaemonProcMcuMSStateNtf
    ��    �ܣ� ���ð�����Ϊ���ð�ʱ ֪ͨ[McuVc]ģ��������״̬���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/31  4.0			����                  ����
=============================================================================*/
void CMcuVcInst::DaemonProcMcuMSStateNtf( const CMessage * pcMsg )
{   
    u8 byIsSwitchOk = *( pcMsg->content );

	CServMsg cServMsg;
	cServMsg.SetSrcMtId(0);
	cServMsg.SetSrcSsnId(0);
    cServMsg.SetMsgBody( &byIsSwitchOk, sizeof(u8) );

    // guzh [9/14/2006] 
    //1.���������л�ʱ,֪ͨ�������衢DCS��MP��MtAdp��Mc 
    //�������ʧ��,����Ͽ�����
    g_cMpSsnApp.BroadcastToAllMpSsn( MCU_MSSTATE_EXCHANGE_NTF, &byIsSwitchOk, sizeof(u8) );
    g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn( MCU_MSSTATE_EXCHANGE_NTF, cServMsg );
    CMcsSsn::BroadcastToAllMcsSsn( MCU_MSSTATE_EXCHANGE_NTF, &byIsSwitchOk, sizeof(u8) );
    g_cEqpSsnApp.BroadcastToAllPeriEqpSsn( MCU_MSSTATE_EXCHANGE_NTF, &byIsSwitchOk, sizeof(u8) );
    g_cDcsSsnApp.BroadcastToAllDcsSsn( MCU_MSSTATE_EXCHANGE_NTF,  &byIsSwitchOk, sizeof(u8) );
    
    // guzh [9/14/2006] ���û��ͬ��Ok, ֱ���˳�    
    if ( !byIsSwitchOk )
    {
        return;
    }
 
	//2.���������л�ʱ,�µ����ð� �� [���滯������Ϣ] ����֪ͨ��ǰ���л��
	::OspPost(MAKEIID(AID_MCU_CONFIG, 1), MCU_MSSTATE_EXCHANGE_NTF, pcMsg->content, pcMsg->length);

	//3.���������л�ʱ,�µ����ð彫 [MCU״̬] ����֪ͨ��ǰ���л��
	TMcuStatus tMcuStatus;
	if (TRUE == g_cMcuVcApp.GetMcuCurStatus(tMcuStatus))
	{
		//notify all mcs
		cServMsg.SetMsgBody((u8 *)&tMcuStatus, sizeof(tMcuStatus));
		SendMsgToAllMcs(MCU_MCS_MCUSTATUS_NOTIF, cServMsg);
	}
	
	//4.���������л�ʱ,�µ����ð彫 [��ַ��״̬] ����֪ͨ��ǰ���л��
    CMcsSsn::BroadcastToAllMcsSsn(MCU_ADDRBOOK_GETENTRYLIST_NOTIF);
    CMcsSsn::BroadcastToAllMcsSsn(MCU_ADDRBOOK_GETGROUPLIST_NOTIF);    

	//5.���������л�ʱ,�µ����ð彫 [�û�״̬] ����֪ͨ��ǰ���л��
    CMcsSsn::BroadcastToAllMcsSsn(MCS_MCU_GETUSERLIST_REQ);


	//6.���������л�ʱ,�µ����ð彫 [����״̬/�������״̬/�ն�״̬/SMCU_MEDAISRC/MMCU����״̬] ����֪ͨ��ǰ���л��
	//notify all mcs
	NtfMcsMcuCurListAllConf(cServMsg);

	return;
}

/*=============================================================================
  �� �� ���� DaemoProcMcsMcuCreateConfByTemplateReq
  ��    �ܣ� ͨ��ģ�崴������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::DaemonProcMcsMcuCreateConfByTemplateReq(const CMessage * pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TTemplateInfo tTemConf;   
	u8 byConfIdx = g_cMcuVcApp.GetConfIdx(cServMsg.GetConfId());

	if (!g_cMcuVcApp.GetTemplate(byConfIdx, tTemConf))
	{
		ConfLog(FALSE, "[DaemoProcMcsMcuCreateConfByTemplateReq] invalid confidx :%d\n", byConfIdx);
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//ongoing conf
	if (MCS_MCU_CREATECONF_BYTEMPLATE_REQ == pcMsg->event 
		|| VCS_MCU_CREATECONF_BYTEMPLATE_REQ == pcMsg->event)
	{
		tTemConf.m_tConfInfo.m_tStatus.SetOngoing();
		tTemConf.m_tConfInfo.SetStartTime( time(NULL) );       
	}
	//schedule conf
	else
	{
		tTemConf.m_tConfInfo.m_tStatus.SetScheduled();
		TKdvTime *ptStartTime = (TKdvTime *)cServMsg.GetMsgBody();
		time_t time;
		ptStartTime->GetTime(time);
		tTemConf.m_tConfInfo.SetStartTime(time);
	}   

	g_cMcuVcApp.TemInfo2Msg(tTemConf, cServMsg);
	cServMsg.SetSrcMtId(CONF_CREATE_MCS);
	cServMsg.SetConfIdx(byConfIdx);

    u8 byInsID = AssignIdleConfInsID();
    if(0 != byInsID)
    {
		// ����mcs���顢vcs����ʵ�ʴ������ֿ�
		if (VCS_MCU_CREATECONF_BYTEMPLATE_REQ == pcMsg->event)
		{
			ConfLog(FALSE, "[DaemonProcMcsMcuCreateConfByTemplateReq] VCSConf start command to inst%d\n",
				           byInsID);
			post(MAKEIID( AID_MCU_VC, byInsID ), MCU_SCHEDULE_VCSCONF_START, 
                 cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
		else
		{
			ConfLog(FALSE, "[DaemonProcMcsMcuCreateConfByTemplateReq] MCSConf start command to inst%d\n",
				           byInsID);
			post(MAKEIID( AID_MCU_VC, byInsID ), MCU_SCHEDULE_CONF_START, 
					cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
    }
	else
    {
        cServMsg.SetErrorCode(ERR_MCU_CONFNUM_EXCEED);
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        ConfLog(FALSE, "[DaemoProcMcsMcuCreateConfByTemplateReq] assign instance id failed!\n");
    }

	return;
}

/*=============================================================================
�� �� ���� DaemonProcCreateConfNPlus
��    �ܣ� n+1ģʽ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/28  4.0			������                  ����
=============================================================================*/
void CMcuVcInst::DaemonProcCreateConfNPlus(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TNPlusConfData *ptConfData = (TNPlusConfData *)cServMsg.GetMsgBody();
    CApp *pcApp = &g_cMcuVcApp;

    //��mcu����ع�
    if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_MASTER_CONNECTED)
    {
        //�Ȳ��Ҵ˻����Ƿ��Ѿ����ڣ����ݻ������ƺ�e164��ƥ��
        for (u8 byInsId = 1; byInsId <= MAXNUM_MCU_CONF; byInsId++)
        {
            CMcuVcInst *pcInst = (CMcuVcInst *)pcApp->GetInstance( byInsId );
            if (NULL != pcInst && pcInst->CurState() == STATE_ONGOING)
            {
                if (strcmp(pcInst->m_tConf.GetConfName(), ptConfData->m_tConf.GetConfName()) == 0 &&
                    strcmp(pcInst->m_tConf.GetConfE164(), ptConfData->m_tConf.GetConfE164()) == 0)
                {
                    //�Ƚ����������
                    NPlusLog("[DaemonProcCreateConfNPlus] release conf %s before rollback.\n", pcInst->m_tConf.GetConfName());
                    pcInst->ReleaseConf(FALSE);
                    pcInst->NextState(STATE_IDLE);                    

                    break;
                }
            }
        }
    }
    //����mcu�ϻ���ָ�
    else if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_SWITCH)
    {
        NPlusLog("[DaemonProcCreateConfNPlus] conf restore in N+1 mode\n");
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[DaemonProcCreateConfNPlus] invalid n+1 mode(%d) in conf restore.\n",
                  g_cNPlusApp.GetLocalNPlusState());
        return;
    }

    u8 byInsID = AssignIdleConfInsID();
    if (0 != byInsID)
    {
        if (OSP_OK == post(MAKEIID( AID_MCU_VC, byInsID ), MCU_CREATECONF_NPLUS, pcMsg->content, pcMsg->length))
        {            
		    CMcuVcInst *pcInst = (CMcuVcInst *)pcApp->GetInstance( byInsID );
		    pcInst->NextState(STATE_WAITEQP);
        }        
    }
    else
    {
        ConfLog(FALSE, "[DaemonProcCreateConfNPlus] assign instance id failed! \n");
    }
    return;
}

/*=============================================================================
  �� �� ���� DaemonProcMcsMcuTemplateOpr
  ��    �ܣ� ģ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::DaemonProcMcsMcuTemplateOpr(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);  
    u8 byConfIdx = g_cMcuVcApp.GetConfIdx(cServMsg.GetConfId());

    switch(pcMsg->event)
    {
    case MCS_MCU_CREATETEMPLATE_REQ:
    case MCS_MCU_MODIFYTEMPLATE_REQ:
        {
            TTemplateInfo  tTemInfo;
			s8* pszUnProcInfoHead = NULL;
			u16 wUnProcLen = 0;
            g_cMcuVcApp.Msg2TemInfo(cServMsg, tTemInfo, &pszUnProcInfoHead, &wUnProcLen);
            tTemInfo.m_byConfIdx = byConfIdx;

            //���������Ϣ�߼�
            u16 wErrCode = 0;
            if ( !IsConfInfoCheckPass(cServMsg, tTemInfo.m_tConfInfo, wErrCode, TRUE))
            {
                cServMsg.SetErrorCode( wErrCode );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
			
			// VMP�ϳ���������, zgc, 2008-03-26
			if ( tTemInfo.m_tConfInfo.GetConfAttrb().IsHasVmpModule() )
			{
				TVMPParam tVMPParam = tTemInfo.m_atVmpModule.GetVmpParam();
				// ��������Ϣ�������֧�ֺϳ�����
				u8 byMaxChnlNumByConf = CMcuPfmLmt::GetMaxCapVMPByConfInfo(tTemInfo.m_tConfInfo);
				u8 byTempChnl = GetVmpChlNumByStyle( tVMPParam.GetVMPStyle() );
				if ( byTempChnl > byMaxChnlNumByConf )
				{
					cServMsg.SetErrorCode( ERR_INVALID_VMPSTYLE );
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					return;
				}
			}

            //���������ģʽΪ���ն��Զ�������Ӧ�����ն�
            if(0 == tTemInfo.m_byMtNum && tTemInfo.m_tConfInfo.GetConfAttrb().IsReleaseNoMt())
            {
                cServMsg.SetErrorCode( ERR_MCU_NOMTINCONF );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                ConfLog( FALSE, "template %s create failed because no mt in auto-end conf!\n", tTemInfo.m_tConfInfo.GetConfName() );
                return;
            }

            if(MCS_MCU_CREATETEMPLATE_REQ == pcMsg->event)
            {
                //�����E164�����Ѵ��ڣ��ܾ� 
                if( g_cMcuVcApp.IsConfE164Repeat( tTemInfo.m_tConfInfo.GetConfE164(), TRUE ) )
                {
                    cServMsg.SetErrorCode( ERR_MCU_CONFE164_REPEAT );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    ConfLog( FALSE, "template %s E164 repeated and create failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
                    return;
                }
                
                //�������Ѵ��ڣ��ܾ�
                if( g_cMcuVcApp.IsConfNameRepeat( tTemInfo.m_tConfInfo.GetConfName(), TRUE ) )
                {
                    cServMsg.SetErrorCode( ERR_MCU_CONFNAME_REPEAT );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    ConfLog( FALSE, "template %s name repeated and create failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
                    return;
                }           

                if(!g_cMcuVcApp.AddTemplate(tTemInfo))
                {
                    // ���صĴ�����
                    cServMsg.SetErrorCode( ERR_MCU_TEMPLATE_NOFREEROOM );
                    ConfLog(FALSE, "[DaemonProcMcsMcuTemplateOpr] add template %s failed\n", tTemInfo.m_tConfInfo.GetConfName());
                    SendReplyBack(cServMsg, pcMsg->event+2);
                    return;
                }                 
                cServMsg.SetConfId( tTemInfo.m_tConfInfo.GetConfId() );
            }
            else
            {                
                TTemplateInfo tOldTemInfo;
                if(!g_cMcuVcApp.GetTemplate(byConfIdx, tOldTemInfo))
                {
                    cServMsg.SetErrorCode( ERR_MCU_TEMPLATE_NOTEXIST );
                    ConfLog(FALSE, "[DaemonProcMcsMcuTemplateOpr] get template confIdx<%d> failed\n", byConfIdx);
                    SendReplyBack(cServMsg, pcMsg->event+2);
                    return;
                }

                //�����E164�����Ѵ��ڣ��ܾ� 
                BOOL32 bSameE164 = (0 == strcmp((s8*)tTemInfo.m_tConfInfo.GetConfE164(), (s8*)tOldTemInfo.m_tConfInfo.GetConfE164()));
                if( !bSameE164 &&
                    g_cMcuVcApp.IsConfE164Repeat( tTemInfo.m_tConfInfo.GetConfE164(), TRUE ) )
                {
                    cServMsg.SetErrorCode( ERR_MCU_CONFE164_REPEAT );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    ConfLog( FALSE, "template %s E164 repeated and modify failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
                    return;
                }
                
                //�������Ѵ��ڣ��ܾ�
                BOOL32 bSameName = (0 == strcmp( (s8*)tTemInfo.m_tConfInfo.GetConfName(), (s8*)tOldTemInfo.m_tConfInfo.GetConfName()));
                if( !bSameName &&
                    g_cMcuVcApp.IsConfNameRepeat( tTemInfo.m_tConfInfo.GetConfName(), TRUE ) )
                {
                    cServMsg.SetErrorCode( ERR_MCU_CONFNAME_REPEAT );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    ConfLog( FALSE, "template %s name repeated and create failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
                    return;
                }    
                
                if(!g_cMcuVcApp.ModifyTemplate(tTemInfo, bSameE164))
                {
                    ConfLog(FALSE, "[DaemonProcMcsMcuTemplateOpr] modify template %s failed\n", tTemInfo.m_tConfInfo.GetConfName());
                    SendReplyBack(cServMsg, pcMsg->event+2);
                    return;
                }

                if(!bSameE164)
                {
                    cServMsg.SetConfId(tOldTemInfo.m_tConfInfo.GetConfId());
                    SendMsgToAllMcs(MCU_MCS_DELTEMPLATE_NOTIF, cServMsg); 
                }
            }
			// ���ڴ������账�����Ϣ�����䱣���ڷֿ����ļ���
			if (pszUnProcInfoHead != NULL && wUnProcLen != 0)
			{
				CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
				u8      byConfPos  = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;

				// ��ͷ��Ϣ��¼�� ��ȡ �����ģ������λ��
				GetAllConfHeadFromFile(acConfId, sizeof(acConfId));

				//�������л��飺�˻����ѱ��棬���ǣ�������ȱʡ����λ��
				for (s32 nPos = 0; nPos < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); nPos++)
				{
					if (acConfId[nPos] == tTemInfo.m_tConfInfo.GetConfId())
					{
						byConfPos = (u8)nPos;
						break;
					}
				}
				//һ���߲�������ģ�岻����δ�����ļ���
				if (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE == byConfPos)
				{
					OspPrintf(TRUE, FALSE, "[DaemonProcMcsMcuTemplateOpr] confinfo_head.dat has no temp with specified confid\n");
				}
				else
				{
					SetUnProConfDataToFile(byConfPos, pszUnProcInfoHead, wUnProcLen);
				}
			}

            SendReplyBack(cServMsg, pcMsg->event+1);
            g_cMcuVcApp.TemInfo2Msg(tTemInfo, cServMsg);
            SendMsgToAllMcs(MCU_MCS_TEMSCHCONFINFO_NOTIF, cServMsg); 
        }
        break;

    case MCS_MCU_DELTEMPLATE_REQ:
        {            
            if(!g_cMcuVcApp.DelTemplate(byConfIdx))
            {
                ConfLog(FALSE, "[DaemonProcMcsMcuTemplateOpr] del template confidx<%d> failed\n", byConfIdx);
                SendReplyBack(cServMsg, pcMsg->event+2);
                return;
            }
            SendReplyBack(cServMsg, pcMsg->event+1);
            SendMsgToAllMcs(MCU_MCS_DELTEMPLATE_NOTIF, cServMsg); 
        }
        break;

    default:
        break;
    }

    return;
}

/*=============================================================================
  �� �� ���� DaemonProcGkRegRsp
  ��    �ܣ� gkע����Ӧ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::DaemonProcGkRegRsp(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8 byConfIdx = cServMsg.GetConfIdx();
    
    if ( 0 == byConfIdx )
    {
        if ( m_tConfInStatus.IsRegGkNackNtf() && MT_MCU_REGISTERGK_NACK == pcMsg->event )
        {
            //MCU ע��GK������ͻ
            u16 wErrCode = cServMsg.GetErrorCode();
            if ( ERR_MCU_RAS_DUPLICATE_ALIAS == wErrCode ) 
            {
                NotifyMcsAlarmInfo( 0, wErrCode );
                m_tConfInStatus.SetRegGkNackNtf(FALSE);
            }
        }
        return;
    }
    else
    {
        if ( MT_MCU_REGISTERGK_NACK == pcMsg->event )
        {
            //ģ�� �� ���� ע��GK������ͻ
            u16 wErrCode = cServMsg.GetErrorCode();
            if ( ERR_MCU_RAS_DUPLICATE_ALIAS == wErrCode ) 
            {
                NotifyMcsAlarmInfo( 0, wErrCode );
            }
        }
    }

    if ( byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX )
    {
        ConfLog(FALSE, "[DaemonProcGkRegRsp] confidx received :%d\n", byConfIdx);
        return;
    }

    //���»���ע��״̬
    CMcuVcInst *pcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
    if (NULL != pcInst && pcInst->CurState() == STATE_ONGOING)
    {
        g_cMcuVcApp.SendMsgToConf(byConfIdx, pcMsg->event, pcMsg->content, pcMsg->length);
    }
    
    //����ģ��ע��״̬
    TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
    if(!tMapData.IsTemUsed())
    {            
        return;
    }

    if(MT_MCU_REGISTERGK_ACK == pcMsg->event)
    {
        g_cMcuVcApp.SetTemRegGK(byConfIdx, TRUE);
    }
    else if(MT_MCU_UNREGISTERGK_ACK == pcMsg->event)
    {
        g_cMcuVcApp.SetTemRegGK(byConfIdx, FALSE);
    }
    else if(MT_MCU_REGISTERGK_NACK == pcMsg->event)
    {
        TTemplateInfo tTemInfo;
        if(g_cMcuVcApp.GetTemplate(byConfIdx, tTemInfo))
        {
            u8 byReg = 0;
            cServMsg.SetMsgBody(&byReg, sizeof(byReg));
            cServMsg.SetConfId(tTemInfo.m_tConfInfo.GetConfId());
            SendMsgToAllMcs( MCU_MCS_CONFREGGKSTATUS_NOTIF, cServMsg ); 
        }
    }

    return;
}

/*=============================================================================
  �� �� ���� DaemonProcGKChargeRsp
  ��    �ܣ� GK�Ʒ���Ӧ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/11/09    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::DaemonProcGKChargeRsp( const CMessage * pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );

    u8 byInstID = 0;
    CApp *pcApp = &g_cMcuVcApp;
    CMcuVcInst* pcVcInst = NULL;

    switch( cServMsg.GetEventId() )
    {
    // zbq [03/26/2007] GK �Ʒ�ע�����֪ͨ
    case MT_MCU_CHARGE_REGGK_NOTIF:

        if ( 0 != cServMsg.GetErrorCode() )
        {
            // ֻ�мƷѵ�MCU���������� GK������128
            NotifyMcsAlarmInfo( 0, cServMsg.GetErrorCode() );
            g_cMcuVcApp.SetChargeRegOK( FALSE );
        }
        else
        {
            g_cMcuVcApp.SetChargeRegOK( TRUE );
        }

        CMcuVcInst * pcVcInst;
        for ( ; byInstID <= MAXNUM_MCU_CONF; byInstID++ )
        {
            pcVcInst = (CMcuVcInst *)pcApp->GetInstance(byInstID);
            if ( NULL != pcVcInst )
            {
                if( STATE_IDLE != pcVcInst->CurState() )
                {
                    g_cMcuVcApp.SendMsgToConf( pcVcInst->m_byConfIdx, cServMsg.GetEventId(),
                                               cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
                }
            }
        }
        break;

    case MT_MCU_CONF_STARTCHARGE_ACK:
		
        g_cMcuVcApp.SendMsgToConf( cServMsg.GetConfIdx(), cServMsg.GetEventId(), 
                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		ConfLog( FALSE, "conf<%d> start charge success !\n", cServMsg.GetConfIdx() );
        break;
        
    case MT_MCU_CONF_STARTCHARGE_NACK:

		NotifyMcsAlarmInfo( 0, cServMsg.GetErrorCode() );        
        g_cMcuVcApp.SendMsgToConf( cServMsg.GetConfIdx(), cServMsg.GetEventId(), NULL, 0 );
        ConfLog( FALSE, "conf<%d> start charge failed !\n", cServMsg.GetConfIdx() );
        break;

    case MT_MCU_CONF_STOPCHARGE_ACK:

        g_cMcuVcApp.SendMsgToConf( cServMsg.GetConfIdx(), cServMsg.GetEventId(), NULL, 0 );
        ConfLog( FALSE, "conf<%d> stop charge success !\n", cServMsg.GetConfIdx() );
    	break;

    case MT_MCU_CONF_STOPCHARGE_NACK:

		NotifyMcsAlarmInfo( 0, cServMsg.GetErrorCode() );
        ConfLog( FALSE, "conf<%d> stop charge failed !\n", cServMsg.GetConfIdx() );
        break;

	case MT_MCU_CONF_CHARGEEXP_NOTIF:

		NotifyMcsAlarmInfo( 0, cServMsg.GetErrorCode() );
		g_cMcuVcApp.SendMsgToConf( cServMsg.GetConfIdx(), cServMsg.GetEventId(), NULL, 0 );
		ConfLog( FALSE, "conf<%d> charge status has got exception !\n", cServMsg.GetConfIdx() );
		break;

    default:
        break;
    }
    return;
}

/*=============================================================================
    ������      ��DaemonProcNPlusRegBackupRsp
    ����        ��N+1 ������MCUע�ᱸ��MCU��Ӧ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
-------------------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    06/12/18    4.0         �ű���         ����
=============================================================================*/
void CMcuVcInst::DaemonProcNPlusRegBackupRsp( const CMessage * pcMsg )
{
    // ����ֻ����NACK��ACK��ؿ����ڶ�״̬��ʱ��ֱ�Ӷ���
    if ( MCU_NPLUS_REG_NACK != pcMsg->event )
    {
        ConfLog( FALSE, "[DaemonProcNPlusRegBackupRsp] unexpected msg.%d<%s> received !\n",
                                                pcMsg->event, OspEventDesc(pcMsg->event) );
        return;
    }
    if ( MCU_NPLUS_MASTER_IDLE != g_cNPlusApp.GetLocalNPlusState() ) 
    {
        ConfLog( FALSE, "[DaemonProcNPlusRegBackupRsp] unexpected NPlus state.%d !\n",
                                                   g_cNPlusApp.GetLocalNPlusState() );
        return;
    }
    // guzh [1/15/2007]
/*
    //����McuStatus
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    TMcuStatus tMcuStatus;
    if ( GetMcuCurStatus(tMcuStatus) ) 
    {
        tMcuStatus.m_byNPlusState = (u8)cServMsg.GetErrorCode();
    }
*/
    return;
}

/*=============================================================================
  �� �� ���� AssignIdleConfInsID
  ��    �ܣ� ����������ʵ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
             u8 byStartInsId 
  �� �� ֵ�� u8 
=============================================================================*/
u8 CMcuVcInst::AssignIdleConfInsID(u8 byStartInsId)
{
    CApp *pcApp = &g_cMcuVcApp;
    CMcuVcInst* pcVcInst = NULL;
    for (u8 byInstID = byStartInsId; byInstID <= MAXNUM_MCU_CONF; byInstID++)
    {
        pcVcInst = (CMcuVcInst *)pcApp->GetInstance(byInstID);
        if (NULL != pcVcInst)
        {
            if(STATE_IDLE == pcVcInst->CurState())
            {
                return byInstID;
            }
        }
    }

    return 0;
}

/*====================================================================
    ������      ��DaemonProcMcsMcuCreateConfReq
    ����        ���������ٿ��Ļ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/01/25    4.0         �ű���         ����
====================================================================*/
void CMcuVcInst::DaemonProcMcsMcuCreateConfReq(const CMessage * pcMsg)
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );

    u8 byInsID = 0;
    if ( g_cMcuVcApp.GetMpNum() > 0 || 
         g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323) > 0 )
    {
        // N+1 ģʽ�£��������ش���
        if ( g_cNPlusApp.GetLocalNPlusState() != MCU_NPLUS_SLAVE_IDLE &&
             g_cNPlusApp.GetLocalNPlusState() != MCU_NPLUS_SLAVE_SWITCH )
        {
            byInsID = AssignIdleConfInsID(byInsID+1);
            if(0 != byInsID)
            {
                cServMsg.SetConfIdx(0);
                cServMsg.SetSrcMtId(CONF_CREATE_MCS);
                ::OspPost(MAKEIID( AID_MCU_VC, byInsID ), MCS_MCU_CREATECONF_REQ, 
                                cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                CApp *pcApp = &g_cMcuVcApp;
                CMcuVcInst *pInst = (CMcuVcInst *)pcApp->GetInstance( byInsID );
            }
            else
            {
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                ConfLog(FALSE, "[DaemonProcMcsMcuCreateConfReq] assign instance id failed!\n");
            }                        
        }
        else
        {
            NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_NPLUS_CREATETEMPLATE );
        }
    }
    else
    {
        //һ�㲻���ܵ�����
        NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_CREATECONF_MCS_NOMPMTADP );
    }
    
    return;
}

/*====================================================================
    ������      ��ProcTimerWaitMpRegCheck
    ����        ��(1) ��ʱ���Mp�Ƿ�ע��, �Ӷ�������Ӧ�ȴ������Ļ���
                  (2) ����Ǽ�ʱ���飬��ʵ��״̬ STATE_WAITEQP
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/17    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ProcTimerWaitMpRegCheck( const CMessage * pcMsg )
{
    CServMsg cServMsg;

	TConfStore tConfStoreBuf;
	TPackConfStore *ptPackConfStore = (TPackConfStore *)&tConfStoreBuf;		
	u16 wAliasBufLen = 0;
	u8* pbyAliasBuf  = NULL; //�ն˱�������������
	u8* pbyModuleBuf = NULL; //����ǽ//����ϳ�ģ�建��
    u8 byInsID = 0;
	u8 achMTPackInfo[SERV_MSG_LEN - SERV_MSGHEAD_LEN];  //��ŷ�����Ϣ�Ļ���

    BOOL32 bHasMpMtAdp = (g_cMcuVcApp.GetMpNum() > 0 && 
                          g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323) > 0);

    BOOL32 bPowerOn = (BOOL32)(*(u32*)pcMsg->content);

    // guzh [4/13/2007] �޸Ĳ���
    // Ŀǰ����Ϊ���ϵ���һ�μ��ָ̻�����ģ��
    // ���ǻ�����ȴ�MP/MtAdp

	for (u8 byLoop = 0; byLoop < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); byLoop++)
	{
        if ( !bPowerOn && !bHasMpMtAdp )
        {
            break;
        }
        
		//��ȡ��� tConfStore Ϊ TPackConfStore �ṹ�����ѽ���Pack����Ļ�������
        if (g_cMcuVcApp.HasConfStore( byLoop ) && ::GetConfFromFile(byLoop, ptPackConfStore))
		{
			wAliasBufLen = htons(ptPackConfStore->m_wAliasBufLen);
			pbyAliasBuf  = (u8 *)(ptPackConfStore+1);
			pbyModuleBuf = pbyAliasBuf+ptPackConfStore->m_wAliasBufLen;

            // guzh [4/11/2007] �������Ǵ��ļ�����
            ptPackConfStore->m_tConfInfo.m_tStatus.SetTakeFromFile(TRUE);

            cServMsg.SetMsgBody((u8*)&ptPackConfStore->m_tConfInfo, sizeof(TConfInfo));
            cServMsg.CatMsgBody((u8*)&wAliasBufLen, 2 );
            cServMsg.CatMsgBody(pbyAliasBuf, ntohs(wAliasBufLen) );

            //����ǽģ�� 
            if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
            {
                cServMsg.CatMsgBody(pbyModuleBuf, sizeof(TMultiTvWallModule));
                pbyModuleBuf += sizeof(TMultiTvWallModule);
            }
            //����ϳ�ģ�� 
            if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasVmpModule())
            {
                cServMsg.CatMsgBody(pbyModuleBuf, sizeof(TVmpModule));
                pbyModuleBuf += sizeof(TVmpModule);
			}

			// ����VCS�����Ļ���ģ�壬��������Ҫ�Ķ�����Ϣ 
			//                  + 1byte(u8: 0 1  �Ƿ������˸������ǽ)
			//                  +(��ѡ, THDTvWall)
			//                  + 1byte(u8: ��������HDU��ͨ������)
			//                  + (��ѡ, THduModChnlInfo+...)	
			//                  + 1byte(�Ƿ�Ϊ��������)��(��ѡ��2byte[u16 ���������ô�����ܳ���]+�¼�mcu����[1byte(�ն�����)+1byte(��������)+xbyte(�����ַ���)+2byte(��������)...)])
			//                  + 1byte(�Ƿ�֧�ַ���)��(��ѡ��2byte(u16 ��������Ϣ�ܳ�)+����(1byte(����)+n��[1TVCSGroupInfo��m��TVCSEntryInfo])
			if (VCS_CONF == ptPackConfStore->m_tConfInfo.GetConfSource())
			{
				u8* pbyVCSInfoBuf = pbyModuleBuf;
				u8 byIsSupportHDTW = *pbyVCSInfoBuf++;
				cServMsg.CatMsgBody(&byIsSupportHDTW, sizeof(u8));
				if (byIsSupportHDTW)
				{
					cServMsg.CatMsgBody((u8*)pbyVCSInfoBuf, sizeof(THDTvWall));
					pbyVCSInfoBuf += sizeof(THDTvWall);
				}

				u8 byHduNum = *pbyVCSInfoBuf++;
				cServMsg.CatMsgBody(&byHduNum, sizeof(u8));
				if (byHduNum)
				{
					cServMsg.CatMsgBody((u8*)pbyVCSInfoBuf, byHduNum * sizeof(THduModChnlInfo));
					pbyVCSInfoBuf += byHduNum * sizeof(THduModChnlInfo);
				}
				
				u8 bySMCUExist = *pbyVCSInfoBuf++;
				cServMsg.CatMsgBody(&bySMCUExist, sizeof(u8));
				if (bySMCUExist)
				{
					wAliasBufLen = htons(*(u16*)pbyVCSInfoBuf);
					cServMsg.CatMsgBody((u8*)&wAliasBufLen, sizeof(u16));
					pbyVCSInfoBuf += sizeof(u16);
					wAliasBufLen = ntohs(wAliasBufLen);
					cServMsg.CatMsgBody((u8*)pbyVCSInfoBuf, wAliasBufLen);
					pbyVCSInfoBuf += wAliasBufLen;					
				}

				u8 byMTPackExist = *pbyVCSInfoBuf++;
				if (byMTPackExist)
				{
					if (GetUnProConfDataToFile(byLoop, (s8*)achMTPackInfo, wAliasBufLen))
					{
						// �ɹ���ȡ��Ӧ����
						cServMsg.CatMsgBody((u8*)&byMTPackExist, sizeof(u8));
						cServMsg.CatMsgBody(achMTPackInfo, wAliasBufLen);
					}
					else
					{
						// �ļ����ܱ�ɾ��
						byMTPackExist =  FALSE;
						cServMsg.CatMsgBody((u8*)&byMTPackExist, sizeof(u8));
					}
				}
				else
				{
					cServMsg.CatMsgBody(&byMTPackExist, sizeof(u8));
				}
			}
            if ( ptPackConfStore->m_tConfInfo.m_tStatus.IsTemplate() && bPowerOn)
            {
                // �����ģ��,�򱣴�  
				TTemplateInfo  tTemInfo;
                g_cMcuVcApp.Msg2TemInfo(cServMsg, tTemInfo);
                g_cMcuVcApp.AddTemplate(tTemInfo);

                g_cMcuVcApp.SetConfRegState( byLoop + 1, FALSE );
                SendMsgToAllMcs(MCU_MCS_TEMSCHCONFINFO_NOTIF, cServMsg); 
                continue;
            }
            else if (!ptPackConfStore->m_tConfInfo.m_tStatus.IsTemplate() &&
                     bHasMpMtAdp && 
                     g_cNPlusApp.GetLocalNPlusState() != MCU_NPLUS_MASTER_CONNECTED &&
                     g_cNPlusApp.GetLocalNPlusState() != MCU_NPLUS_MASTER_IDLE)
            {                
                byInsID = AssignIdleConfInsID(byInsID+1);
                if(0 != byInsID)
                {
                    cServMsg.SetConfIdx(byLoop+1);
                    ::OspPost(MAKEIID( AID_MCU_VC, byInsID ), MCU_CREATECONF_FROMFILE, 
                        cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                    CApp *pcApp = &g_cMcuVcApp;
                    CMcuVcInst *pInst = (CMcuVcInst *)pcApp->GetInstance( byInsID );
                    pInst->NextState(STATE_WAITEQP);

                    OspPrintf(TRUE, FALSE, "MCU_CREATECONF_FROMFILE send to Ins.%d, byLoop.%d\n", byInsID, byLoop);
                }
            }
		}
	}

	if (!bHasMpMtAdp)
	{
		SetTimer( MCUVC_WAIT_MPREG_TIMER, TIMESPACE_WAIT_MPREG, FALSE);
		if ( 0 == g_cMcuVcApp.GetMpNum())
		{
			ConfLog( FALSE, "[MCU]: NO Mp regitered, Waiting......\n" );
		}
		if ( 0 == g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323))
		{
			ConfLog( FALSE, "[MCU]: NO MtAdp regitered, Waiting......\n" );
		}
	}
	
	return;
}

/*====================================================================
    ������      ��ProcTimerScheduledCheck
    ����        ��ԤԼ���鶨ʱ��鴦����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/01    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcTimerScheduledCheck( const CMessage * pcMsg )
{
	CServMsg cServMsg;
	
	switch( CurState() )
	{
	case STATE_SCHEDULED:

		//δ����ʼʱ��
		if( time( NULL ) < m_tConf.GetStartTime() )	
		{
			SetTimer( MCUVC_SCHEDULED_CHECK_TIMER, TIMESPACE_SCHEDULED_CHECK );
			break;
		}
		
        // guzh [5/17/2007] ���Ϊ��ʱ����
        m_tConf.m_tStatus.SetOngoing();
        g_cMcuVcApp.ConfInfoMsgPack(this, cServMsg);
        
		cServMsg.SetSrcMtId( CONF_CREATE_SCH );
        cServMsg.SetConfIdx(m_byConfIdx);
		
		//��Ҫʹ�ñ���ת������Ȼ����Ϣ����ȷ����������ReleaseConf()���У������ֻᱻɾ��
		post( MAKEIID( GetAppID(), GetInsID() ), MCU_SCHEDULE_CONF_START, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		
		ConfLog(FALSE, "CMcuVcInst: A Scheduled conference %s started!\n", m_tConf.GetConfName() );
        
		//release conference
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
    ������      ��ProcTimerOngoingCheck
    ����        �������л��鶨ʱ����Ƿ����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/01    1.0         LI Yi         ����
	03/11/15    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::ProcTimerOngoingCheck( const CMessage * pcMsg )
{
	CServMsg	cServMsg;
	u16 wLeftMin;

	switch( CurState() )
	{
	case STATE_ONGOING:
		//�������ǰһ������ʾ���л������ϯ�ն�
		if( time( NULL ) > m_tConf.GetStartTime() + m_tConf.GetDuration() * 60 - 15*60 )	//not stop
		{
			wLeftMin = m_tConf.GetDuration() - (time( NULL ) - m_tConf.GetStartTime())/60;

			if( wLeftMin == 15 || wLeftMin == 10 || wLeftMin == 5 
				|| wLeftMin == 4 || wLeftMin == 3|| wLeftMin == 2 || wLeftMin == 1 )
			{
				wLeftMin = htons( wLeftMin );
				cServMsg.SetMsgBody( (u8*)&wLeftMin, 2);  

				BroadcastToAllSubMtJoinedConf( MCU_MT_CONFWILLEND_NOTIF, cServMsg );   
				SendMsgToAllMcs( MCU_MCS_CONFTIMELEFT_NOTIF, cServMsg );
			}
		}

        //����Ƿ��˽���ʱ��
		if( time( NULL ) < m_tConf.GetStartTime() + m_tConf.GetDuration() * 60 )	//not stop
		{
			SetTimer( MCUVC_ONGOING_CHECK_TIMER, TIMESPACE_ONGOING_CHECK );
			break;
		}
		
		//��������
		ReleaseConf( TRUE );
		ConfLog( FALSE, "Conference %s released at stop time!\n", m_tConf.GetConfName() );

		NEXTSTATE( STATE_IDLE );
		break;
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcTimerInviteUnjoinedMt
    ����        ����ʱ����δ����ն˴�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/01/03    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcTimerInviteUnjoinedMt( const CMessage * pcMsg )
{
	CServMsg	cServMsg;
    TMt tMt;
	u8  byLoop;

#ifdef _DEBUG
	s8	achStr[64];
	m_tConf.GetConfId().GetConfIdString( achStr, sizeof( achStr ) );
	log( LOGLVL_DEBUG1, "CMcuVcInst: Conference %s (confid=%s) now invite all unjoined MT!\n", 
		m_tConf.GetConfName(), achStr );
#endif

	switch( CurState() )
	{
	case STATE_ONGOING:

		//send invite message to all connected MTs
		cServMsg.SetConfId( m_tConf.GetConfId() );
		cServMsg.SetNoSrc();
		cServMsg.SetTimer( TIMESPACE_WAIT_AUTOINVITE );
		cServMsg.SetMsgBody( NULL, 0 );
		for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			if(!m_tConfAllMtInfo.MtInConf( byLoop ) )
			{
				continue;
			}
			if(m_tConfAllMtInfo.MtJoinedConf(byLoop))
			{
				continue;
			}
            // libo [3/30/2005]
            if (m_ptMtTable->GetAddMtMode(byLoop) == ADDMTMODE_CHAIRMAN)
			{
				continue;
			}
            // libo [3/30/2005]end

			if( CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode( byLoop ) )
			{
				u32 dwLeftTimes = m_ptMtTable->GetCallLeftTimes( byLoop );

				//����κ��У���ֱ�ӽ��к���
				if( DEFAULT_CONF_CALLTIMES == m_tConf.m_tStatus.GetCallTimes() )
				{
					tMt = m_ptMtTable->GetMt( byLoop );
					InviteUnjoinedMt( cServMsg, &tMt );
				}
				//���޴κ��У����ʣ����д���
				else if( dwLeftTimes > 0 )
				{
					dwLeftTimes--;
					m_ptMtTable->SetCallLeftTimes( byLoop, dwLeftTimes );
					tMt = m_ptMtTable->GetMt( byLoop );
					InviteUnjoinedMt( cServMsg, &tMt );
				}
			}			
		}		
		
		SetTimer( MCUVC_INVITE_UNJOINEDMT_TIMER, 1000*m_tConf.m_tStatus.GetCallInterval() );	
		
		break;
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcCommonOperAck
    ����        ���ն˻��������ͬ��Ӧ��ͳһ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/14    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcCommonOperAck( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:
		//send messge
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcCommonOperNack
    ����        ���ն˻���������ܾ�Ӧ��ͳһ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/14    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcCommonOperNack( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:
		//send messge
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��DaemonProcAppTaskRequest
    ����        ��GUARDģ��̽����Ϣ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/04/21    1.0         ���         ����
====================================================================*/
void CMcuVcInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );
}

/*====================================================================
    ������      ��DaemonProcCommonNotif
    ����        ���ն˻�������֪ͨͨ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/20    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::DaemonProcCommonNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	switch( pcMsg->event )
	{
	case REC_MCU_EXCPT_NOTIF:
		SendMsgToAllMcs( MCU_MCS_RECORDEREXCPT_NOTIF, cServMsg );

		break;
	default:
		break;
	}
}



/*====================================================================
    ������      ��DaemonProcTestMcuGetAllMasterConfIdCmd
    ����        ������Tester��������Ա�MCUΪ��MCU�Ļ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/04    1.0         JQL           ����
====================================================================*/
void CMcuVcInst::DaemonProcTestMcuGetAllMasterConfIdCmd( const CMessage* pcMsg )
{	
    /*
	CConfId aCConfId[MAXNUM_MCU_CONF];
	u16 wIndex = 0;

	memset( aCConfId, 0, sizeof( aCConfId ) );
	
    for(u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
    {
        CMcuVcInst *pcIns = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
        if(NULL != pcIns)
        {
            aCConfId[wIndex++] = pcIns->m_tConf.GetConfId();
        }
    }	

	//ȷ���ܷ������ݵ�Tester
	if( wIndex == 0 )
		wIndex = 1;
	SetSyncAck( ( const u8*)aCConfId, wIndex* sizeof( CConfId ) );

    */  
}

/*====================================================================
    ������      : DaemonProcTestMcuGetConfFullInfoCmd
    ����        ������Tester��������Ա�MCUΪ��MCU�Ļ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/04    1.0         JQL           ����
====================================================================*/
void CMcuVcInst::DaemonProcTestMcuGetConfFullInfoCmd( const CMessage* pcMsg )
{
    /*
	CConfId cConfId = *(CConfId*)pcMsg->content;

    u8 byConfIdx = g_cMcuVcApp.GetConfIdx(cConfId);
    CMcuVcInst *pcIns = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
    if(NULL != pcIns)
    {
        SetSyncAck( (const u8*)&pcIns->m_tConf, sizeof( TConfInfo ) );
    }
    */
}

/*====================================================================
    ������      ��ReleaseMtRes
    ����        ���ͷ�MTռ�õ���Դ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bForced	[in]����MT����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/07/22    3.0         ������        ����
	2/4/2009	4.5			Ѧ��		  �޸� 
====================================================================*/
void CMcuVcInst::ReleaseMtRes( u8 byMtId, BOOL32 bNeglectMtType )
{
    u8 byDriId = m_ptMtTable->GetDriId(byMtId);
	
	TCapSupport tCapSupport;
	m_ptMtTable->GetMtCapSupport(byMtId,&tCapSupport);
	// xliang [10/27/2008] �ж��Ƿ���HD MT ��mcu���ý���
	if( tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 &&
		(tCapSupport.GetMainVideoResolution() == VIDEO_FORMAT_HD1080 ||
		tCapSupport.GetMainVideoResolution() == VIDEO_FORMAT_HD720 )&&
		/*tCapSupport.GetMainVideoResolution() == VIDEO_FORMAT_4CIF*/ 
		( byDriId == MCU_BOARD_MPC || 
		byDriId == MCU_BOARD_MPCD ) )
	{
		g_cMcuVcApp.DecMtAdpMtNum( byDriId, m_byConfIdx, byMtId, TRUE);
	}
	else
	{
		g_cMcuVcApp.DecMtAdpMtNum( byDriId, m_byConfIdx, byMtId );
	}
	// xliang [2/4/2009] �����ն�������MCU�����Σ�����ֵ����-1
	u8 byMtType = m_ptMtTable->GetMtType(byMtId);
	if( (MT_TYPE_MMCU == byMtType || MT_TYPE_SMCU == byMtType)
		&& !bNeglectMtType )
	{
		g_cMcuVcApp.DecMtAdpMtNum( byDriId, m_byConfIdx, byMtId, FALSE,TRUE );
	}

	u8 byMpId = m_ptMtTable->GetMpId( byMtId );
	g_cMcuVcApp.DecMpMtNum( byMpId, m_byConfIdx, byMtId, m_tConf.GetBitRate() );
}

/*=============================================================================
    �� �� ���� StopAllMtRecoderToPeriEqp
    ��    �ܣ� ֹͣ�����ն�¼��ֹͣ��¼�������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ����
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/03/30  3.6			����                  ����
=============================================================================*/
void CMcuVcInst::StopAllMtRecoderToPeriEqp( )
{
	for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
	{					
		if( m_tConfAllMtInfo.MtJoinedConf( byMtId ) && 
			!m_ptMtTable->IsMtNoRecording( byMtId ) )
		{
			TEqp tRecEqp;
			u8   byRecChn;

			m_ptMtTable->GetMtRecordInfo( byMtId ,&tRecEqp, &byRecChn );
			StopSwitchToPeriEqp( tRecEqp.GetEqpId(), byRecChn);
			m_ptMtTable->SetMtNoRecording(byMtId);
		}
	}
}

/*====================================================================
    ������      ��ReleaseConf
    ����        ���ӻ������ɾ���û��飻
	              ֪ͨ���裬MC��MT��SMCU��
				  ֹͣ���������н�����
				  ֹͣ���ж�ʱ����MASTER����
				  ɾ���ļ���SCHEDULE����
				  �ص�IDLE״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/01/08    1.0         LI Yi         ����
	03/11/13    3.0         ������        �޸�
	05/12/20	4.0			�ű���		  T120����
====================================================================*/
void CMcuVcInst::ReleaseConf( BOOL32 bRemoveFromFile )
{
	CServMsg	cServMsg;
	TConfMtInfo	tConfMtInfo;
	TPeriEqpStatus tPeriEqpStatus;
//	u8  byVmpConfIdx = 0;     //  hduѡ������ϳ�ʱ����������ʱ����vmp confIdx,
	                          //  �Ա�ֹͣhduʱ��ȷ���ѡ��vmp�Ļ���������
	                          
//	TMt tMt;
    
	//delete file
	//����VCS���鲻���浱ǰ������Ϣ���ļ�
	if( bRemoveFromFile && 
		( CurState() == STATE_SCHEDULED || CurState() == STATE_ONGOING )
		&& m_tConf.GetConfSource() != VCS_CONF)
	{
		if(!g_cMcuVcApp.RemoveConfFromFile( m_tConf.GetConfId() ))
        {
            OspPrintf(TRUE, FALSE, "[ReleaseConf] remove conf from file failed\n");
        }
	}

    // xsl [11/16/2006] �Ƿ�֧��gk�Ʒ�
    if ( 0 != g_cMcuAgent.GetGkIpAddr() &&g_cMcuAgent.GetIsGKCharge() &&  
         0 != g_cMcuVcApp.GetRegGKDriId() &&
         m_tConf.IsSupportGkCharge() && m_tConf.m_tStatus.IsGkCharge() )
    {
        //��GK����ֹͣ�Ʒ�����
        g_cMcuVcApp.ConfChargeByGK( m_byConfIdx, g_cMcuVcApp.GetRegGKDriId(), TRUE );
        
    }

	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetNoSrc();

    //Notify all meeting consoles
    u8 byMode = CONF_TAKEMODE_ONGOING;
    if(STATE_SCHEDULED == CurState())
    {
        byMode = CONF_TAKEMODE_SCHEDULED;
    }
//    cServMsg.SetMsgBody(&byMode, sizeof(byMode));
//    SendMsgToAllMcs( MCU_MCS_RELEASECONF_NOTIF, cServMsg );
    
	
    TConfMapData tMapData;
    if(m_byConfIdx < MIN_CONFIDX || m_byConfIdx > MAX_CONFIDX)
    {
        OspPrintf(TRUE, FALSE, "[ReleaseConf] invalid confidx :%d\n", m_byConfIdx);
    }
    else
    {
        tMapData = g_cMcuVcApp.GetConfMapData(m_byConfIdx);
    }
	//��GK��ע�� 
	if( !tMapData.IsTemUsed() && m_tConf.m_tStatus.IsRegToGK() > 0 && g_cMcuVcApp.GetRegGKDriId() > 0 )
	{
		TMtAlias tMtAlias;
		tMtAlias.SetE164Alias( m_tConf.GetConfE164() );
		cServMsg.SetConfId( m_tConf.GetConfId() );
		cServMsg.SetConfIdx( m_byConfIdx );
		cServMsg.SetDstDriId( g_cMcuVcApp.GetRegGKDriId() );
		cServMsg.SetMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
		g_cMtAdpSsnApp.SendMsgToMtAdpSsn( g_cMcuVcApp.GetRegGKDriId(), MCU_MT_UNREGISTERGK_REQ, cServMsg );

		//ע���Ļ���ע���¼�� ͬ�� ��������ģ���� ע��MCU������ʵ�� ע����Ϣ
		g_cMpManager.UpdateRRQInfoAfterURQ( m_byConfIdx );
	}
    
	if( STATE_SCHEDULED == CurState() )
	{
		//�ͷŶ˿�
//		for( u8 byIndex = 1; byIndex <= MAXNUM_CONF_MT; byIndex++ )
//		{
//			if( m_tConfAllMtInfo.MtInConf( byIndex ) )
//			{
//				ReleaseMtRes( byIndex );
//			}
//		}
		
		g_cMcuVcApp.RemoveConf( m_byConfIdx );

        // [12/08/2006]
        cServMsg.SetMsgBody(&byMode, sizeof(byMode));
        SendMsgToAllMcs( MCU_MCS_RELEASECONF_NOTIF, cServMsg );

        //clear map info
        g_cMcuVcApp.SetConfMapInsId(m_byConfIdx, 0);
		
		DeleteAlias();
		
		ConfLog( FALSE, "schedule Conference %s was released!\n", m_tConf.GetConfName() );

		ClearVcInst();
//		MCU_SAFE_DELETE(m_ptMtTable)
		return;
	}

	///////////////////////////////////////////////
	//����ֻ�м�ʱ������Ҫ����    

	//��Trap��Ϣ
	TConfNotify tConfNotify;
	CConfId cConfId;
	cConfId = m_tConf.GetConfId( );
	memcpy( tConfNotify.m_abyConfId, &cConfId, 16 );
	memcpy( tConfNotify.m_abyConfName, m_tConf.GetConfName(), MAXLEN_CONFNAME );
    
    SendTrapMsg( SVC_AGT_CONFERENCE_STOP, (u8*)&tConfNotify, sizeof(tConfNotify) );
 	
	//ɱ������ʱ��
	KillTimer( MCUVC_REFRESH_MCS_TIMER );
	KillTimer( MCUVC_INVITE_UNJOINEDMT_TIMER );
	KillTimer( MCUVC_POLLING_CHANGE_TIMER );
    KillTimer( MCUVC_TWPOLLING_CHANGE_TIMER );
	KillTimer( MCUVC_MCUSRC_CHECK_TIMER );
	KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
	KillTimer( MCUVC_HDUPOLLING_CHANGE_TIMER );
	KillTimer( MCUVC_VCMTOVERTIMER_TIMER );
	KillTimer( MCUVC_VCS_CHAIRPOLL_TIMER );

	//������н�����
	u8 bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
	if( !m_tVidBrdSrc.IsNull() )
	{
		g_cMpManager.RemoveSwitchBridge( m_tVidBrdSrc, bySrcChnnl, MODE_VIDEO );
	}
	
	//ֹͣ˫��
	ClearH239TokenOwnerInfo( NULL );
	StopDoubleStream( TRUE, TRUE );

	if( !m_tAudBrdSrc.IsNull() )
	{
		g_cMpManager.RemoveSwitchBridge( m_tAudBrdSrc, bySrcChnnl, MODE_AUDIO );
	}

	//�鲥�����鲥����
	if( m_tConf.GetConfAttrb().IsMulticastMode() && !m_tVidBrdSrc.IsNull() )
	{
		g_cMpManager.StopMulticast( m_tVidBrdSrc, bySrcChnnl );
	}

	//ֹͣ���н���
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        if (!m_tVidBrdSrc.IsNull())
        {
#ifdef _SATELITE_
			g_cMpManager.StopSatConfCast(m_tVidBrdSrc, CAST_FST, MODE_BOTH, bySrcChnnl);
			g_cMpManager.StopSatConfCast(m_tVidBrdSrc, CAST_SEC, MODE_BOTH, bySrcChnnl);

			for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
			{
				if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
					byMtId != m_tVidBrdSrc.GetMtId())
				{
					TMt tMtNull;
					tMtNull.SetNull();
					m_ptMtTable->SetMtSrc(byMtId, &tMtNull, MODE_BOTH);
				}
			}
#else
			g_cMpManager.StopDistrConfCast(m_tVidBrdSrc, MODE_BOTH, bySrcChnnl);
#endif
        }        
    }
    else
    {
        StopSwitchToAllSubMtJoinedConf( MODE_BOTH );
    }

	StopSwitchToAllMcInConf();	
	StopSwitchToAllPeriEqpInConf();

	//ֹͣ����
	if( m_tConf.m_tStatus.IsMixing() || m_tConf.m_tStatus.IsVACing() )
	{
		StopMixing();

		//����������
 		g_cMcuVcApp.GetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
		tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = TMixerGrpStatus::READY;
        tPeriEqpStatus.SetConfIdx( 0 );
		g_cMcuVcApp.SetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );

        cServMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
        SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
	}

	//ֹͣ����ϳ�
	if (CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode())
	{
		SendMsgToEqp( m_tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cServMsg );

		//��������ϳ���
 		g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
		tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
//		byVmpConfIdx = tPeriEqpStatus.GetConfIdx();
		tPeriEqpStatus.SetConfIdx( 0 );
		g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus ); 

        if (m_tConf.m_tStatus.GetVmpParam().IsVMPBrdst())
        {
            ChangeVidBrdSrc(NULL);
        }
	}
    
    //ֹͣ�໭�����ǽ
    if ( CONF_VMPTWMODE_NONE != m_tConf.m_tStatus.GetVmpTwMode() )
    {
        SendMsgToEqp( m_tVmpTwEqp.GetEqpId(), MCU_VMPTW_STOPVIDMIX_REQ, cServMsg );

        //�ͷ�vpu��Դ
        g_cMcuVcApp.GetPeriEqpStatus( m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus );
        tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = FALSE;
        tPeriEqpStatus.SetConfIdx( 0 );
        g_cMcuVcApp.SetPeriEqpStatus( m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus );
    }
	
	//ֹͣ����¼��ֹͣ��¼�������
	if( m_tConf.m_tStatus.IsRecording() /*&& HasJoinedSpeaker()*/ )
	{
        StopSwitchToPeriEqp( m_tRecEqp.GetEqpId(), m_byRecChnnl, TRUE );
	}
	
	//ֹͣ�����ն�¼��ֹͣ��¼�������
	StopAllMtRecoderToPeriEqp( );

	//ֹͣ����
	if( !m_tConf.m_tStatus.IsNoPlaying() )
	{
		cServMsg.SetMsgBody( ( u8 * )&m_tPlayEqp, sizeof( m_tPlayEqp ) );	//set TEqp
		cServMsg.SetChnIndex( m_byPlayChnnl );
		SendMsgToEqp( m_tPlayEqp.GetEqpId(), MCU_REC_STOPPLAY_REQ, cServMsg );
	}

	//ֹͣý����������, �ͷ�Ԥ�������� -- modify bas 2
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

	//�����ش�ֹͣ
	if( m_tConf.m_tStatus.IsPrsing() )
	{
		StopPrs(PRSCHANMODE_BOTH, TRUE);

        StopPrs(PRSCHANMODE_AUDIO, TRUE);
        StopPrs(PRSCHANMODE_AUDBAS, TRUE);
        StopPrs(PRSCHANMODE_VIDBAS, TRUE);
        StopPrs(PRSCHANMODE_BRBAS, TRUE);

        // FIXME���������䶪���ش�δ���, zgc

		// xliang [4/30/2009] FIXME �㲥Դ4��
		StopPrs(PRSCHANMODE_VMPOUT1, TRUE);
		StopPrs(PRSCHANMODE_VMPOUT2, TRUE);
		StopPrs(PRSCHANMODE_VMPOUT3, TRUE);
		StopPrs(PRSCHANMODE_VMPOUT4, TRUE);
	}

	//�ͷű���ͨ��
	if( m_tConf.GetConfAttrb().IsResendLosePack() )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl].SetReserved( FALSE );
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl2].SetReserved( FALSE );
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAud].SetReserved( FALSE );
        if(EQP_CHANNO_INVALID != m_byPrsChnnlAudBas && m_byPrsChnnlAudBas < MAXNUM_PRS_CHNNL)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAudBas].SetReserved(FALSE);
        }
        if(EQP_CHANNO_INVALID != m_byPrsChnnlVidBas && m_byPrsChnnlVidBas < MAXNUM_PRS_CHNNL)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVidBas].SetReserved(FALSE);
        }
        if(EQP_CHANNO_INVALID != m_byPrsChnnlBrBas && m_byPrsChnnlBrBas < MAXNUM_PRS_CHNNL)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlBrBas].SetReserved(FALSE);
        }
		// xliang [4/30/2009] �ͷŹ㲥Դ4����Ӧ��PRSͨ��
		if(EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut1 && m_byPrsChnnlVmpOut1 < MAXNUM_PRS_CHNNL)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut1].SetReserved(FALSE);
        }
		if(EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut2 && m_byPrsChnnlVmpOut2 < MAXNUM_PRS_CHNNL)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut2].SetReserved(FALSE);
        }
		if(EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut3 && m_byPrsChnnlVmpOut3 < MAXNUM_PRS_CHNNL)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut3].SetReserved(FALSE);
        }
		if(EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut4 && m_byPrsChnnlVmpOut4 < MAXNUM_PRS_CHNNL)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut4].SetReserved(FALSE);
        }
		g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
	}

	//ֹͣ��ѯ
	TPollInfo *ptPollInfo = m_tConf.m_tStatus.GetPollInfo();
	if( ptPollInfo->GetMediaMode() == MODE_VIDEO)
	{
		ChangeVidBrdSrc( NULL );
	}
    
	//ֹͣhdu������ѯ
	if ( POLL_STATE_NONE != m_tHduBatchPollInfo.GetStatus() )
	{
		u8 byHduChlNum = 0;
		u8 byHduEqpId = 0;
		u8 byChnlIdx = 0;
		u8 byChnlPollNum = m_tHduBatchPollInfo.GetChnlPollNum();
		TPeriEqpStatus tHduStatus;
        for ( ; byHduChlNum < byChnlPollNum; byHduChlNum ++ )
        {
			byHduEqpId = m_tHduBatchPollInfo.m_tChnlBatchPollInfo[byHduChlNum].GetEqpId();
			byChnlIdx = m_tHduBatchPollInfo.m_tChnlBatchPollInfo[byHduChlNum].GetChnlIdx();
			if ( byHduEqpId >= HDUID_MIN && byHduEqpId <= HDUID_MAX )
			{
                g_cMcuVcApp.GetPeriEqpStatus( byHduEqpId, &tHduStatus );
				if ( 0 == tHduStatus.m_byOnline )    // hdu������
				{
					tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetNull();
					g_cMcuVcApp.SetPeriEqpStatus( byHduEqpId, &tHduStatus );
					cServMsg.SetMsgBody( (u8*)&tHduStatus, sizeof(tHduStatus) );
					SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF,  cServMsg);
				}
			}
			else
			{
				CfgLog( FALSE, "[ProcMcsMcuHduBatchPollMsg] stop hdu(%d) batch poll error!\n", byHduEqpId);
				return;
			}
        }
		
		m_tHduBatchPollInfo.SetNull();
		m_tConf.m_tStatus.m_tConfMode.SetHduInBatchPoll( POLL_STATE_NONE );
		
		TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
		tConfAttrbEx.SetSchemeIdxInBatchPoll( 0 );
		m_tConf.SetConfAttrbEx( tConfAttrbEx );
		
        m_tHduPollSchemeInfo.SetNull();
        ConfStatusChange();            // ֪ͨ��ػ���״̬�ı�

		//֪ͨ���л��
		cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
		SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
		
		KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
	}

	//ֹͣ�����ǽ����
	u8  byLoop = 0,  byEqpId = 0;
	TPeriEqpStatus tTvwallStatus, tHduStatus;
	memset(&tTvwallStatus, 0x0, sizeof(tTvwallStatus));
	memset(&tHduStatus, 0x0, sizeof(tHduStatus));

	//ֹͣHDU
	for ( byEqpId=HDUID_MIN; byEqpId < HDUID_MAX; byEqpId++ )
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byEqpId) && 
			EQP_TYPE_HDU == g_cMcuVcApp.GetEqpType(byEqpId))
		{			
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tHduStatus);
            if (1 == tHduStatus.m_byOnline)
            {
                u8 byMtConfIdx;
				u8 byMtMainType;

				for(byLoop = 0; byLoop < MAXNUM_HDU_CHANNEL; byLoop++)
				{
                    byMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
					byMtMainType = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetType();
					if ( m_byConfIdx == byMtConfIdx ) // || TYPE_MCUPERI == byMtMainType
					{
						StopSwitchToPeriEqp(byEqpId, byLoop);
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = 0;
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
					}
				}
				tHduStatus.m_tStatus.tHdu.byChnnlNum = 0;
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tHduStatus);
				
                cServMsg.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
            }
		}

	}
	//ֹͣ����ǽ
	for ( byEqpId=TVWALLID_MIN; byEqpId < TVWALLID_MAX; byEqpId++ )
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byEqpId) && 
			EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
		{			
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
            if (1 == tTvwallStatus.m_byOnline)
            {
                u8 byMtConfIdx;
                u8 byMemberNum = tTvwallStatus.m_tStatus.tTvWall.byChnnlNum;
				for(byLoop = 0; byLoop < byMemberNum; byLoop++)
				{
                    byMtConfIdx = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
					if (m_byConfIdx == byMtConfIdx)
					{
						StopSwitchToPeriEqp(byEqpId, byLoop);
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
					}
				}
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tTvwallStatus);
				
                cServMsg.SetMsgBody((u8 *)&tTvwallStatus, sizeof(tTvwallStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
            }
		}
	}

	//�����������ն˵�dump
	if (NULL != m_ptMtTable)
	{
		for( byLoop = 0; byLoop < m_ptMtTable->m_byMaxNumInUse; byLoop++)
		{
			TMt tMcuMt = m_ptMtTable->GetMt(byLoop + 1);
			if(!tMcuMt.IsNull())
			{
				if( m_ptMtTable->IsMtAudioSending( tMcuMt.GetMtId() ) )
				{
					g_cMpManager.StopRecvMt( tMcuMt, MODE_AUDIO );
				}
				if( m_ptMtTable->IsMtVideoSending( tMcuMt.GetMtId() ) )
				{
					g_cMpManager.StopRecvMt( tMcuMt, MODE_VIDEO );
				}			
			}	
		}

	}

	//�Ҷ������ն�(�����п������ڴ�����е��ն�Ҳ�Ҷ�)
	cServMsg.SetConfIdx( m_byConfIdx );
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetDstMtId( 0 );
	cServMsg.SetMsgBody( NULL, 0 );
	g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn( MCU_MT_DELMT_CMD, cServMsg);
	
	//�ͷŶ˿�
	for( u8 byIndex = 1; byIndex <= MAXNUM_CONF_MT; byIndex++ )
	{	
		if( m_tConfAllMtInfo.MtInConf( byIndex ) )
		{
			ReleaseMtRes( byIndex );

            // xsl [10/11/2006] �ͷŶ˿ڵ�ͬʱ�ͷ��ն˽�����ַ��Ϣ
            g_cMcuVcApp.ReleaseMtPort( m_byConfIdx, byIndex );
            m_ptMtTable->ClearMtSwitchAddr( byIndex );
		}
	}

	cServMsg.SetConfIdx( m_byConfIdx );
	g_cMpSsnApp.BroadcastToAllMpSsn( MCU_MP_RELEASECONF_NOTIFY,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());

	//��������ݻ���
	if ( CONF_DATAMODE_VAONLY != m_tConf.GetConfAttrb().GetDataMode() )
	{
		//���Ѿ��ٿ�
		if ( m_tConfInStatus.IsDataConfOngoing() )
		{
			//��֪DCSɾ���û���
			SendMcuDcsReleaseConfReq();
		}
	}
	
    // [12/08/2006]
    cServMsg.SetMsgBody(&byMode, sizeof(byMode));
    SendMsgToAllMcs( MCU_MCS_RELEASECONF_NOTIF, cServMsg );

	//�����������
	g_cMcuVcApp.RemoveConf( m_byConfIdx );

    //clear map info
    g_cMcuVcApp.SetConfMapInsId(m_byConfIdx, 0);

	DeleteAlias();
	ConfLog( FALSE, "Ongoing Conference %s was released!\n", m_tConf.GetConfName() );

    //֪ͨn+1���ݷ��������»�����Ϣ
    if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
    {
        ProcNPlusConfInfoUpdate(FALSE);
    }
    
    //�ͷŸ�������ͨ��
    StopHDMVBrdAdapt();
    StopHDDSBrdAdapt();
    StopHDCascaseAdp();

    //�ͷ�mau
    m_cBasMgr.ReleaseHdBasChn();
	//�ͷ�ѡ��BASͨ��
	u8 byChnlNum = 0;
	u8 abyBasId[MAXNUM_PERIEQP]   = {0};
	u8 abyChnlIdx[MAXNUM_PERIEQP] = {0};
	TPeriEqpStatus tEqpStatus;
	m_cSelChnGrp.GetUsedAdptChnlInfo(byChnlNum, abyBasId, abyChnlIdx);
	for (u8 byIdx = 0; byIdx < byChnlNum; byIdx++)
	{
		CServMsg cServMsg;
		cServMsg.SetChnIndex(abyChnlIdx[byIdx]);
		cServMsg.SetConfId(m_tConf.GetConfId());
		if (!g_cMcuAgent.IsEqpBasHD(abyBasId[byIdx]))
		{
			//������������ֹͣ��Ϣ
			SendMsgToEqp(abyBasId[byIdx], MCU_BAS_STOPADAPT_REQ, cServMsg);

			//�ͷ�ͨ��
			if (g_cMcuVcApp.GetPeriEqpStatus(abyBasId[byIdx], &tEqpStatus))
			{
				tEqpStatus.m_tStatus.tBas.tChnnl[abyChnlIdx[byIdx]].SetStatus(TBasChnStatus::READY);
				tEqpStatus.m_tStatus.tBas.tChnnl[abyChnlIdx[byIdx]].SetReserved(FALSE);
				g_cMcuVcApp.SetPeriEqpStatus(abyBasId[byIdx], &tEqpStatus);
				g_cMcuVcApp.SendPeriEqpStatusToMcs(abyBasId[byIdx]);
			}

		}
		else
		{
			SendMsgToEqp(abyBasId[byIdx], MCU_HDBAS_STOPADAPT_REQ, cServMsg);
			g_cMcuVcApp.ReleaseHDBasChn(abyBasId[byIdx], abyChnlIdx[byIdx]);
		}

	}
	m_cSelChnGrp.Clear();

#ifdef _SATELITE_

	//ֹͣ���е�MODEM,ͬʱƵ���ͷ�
	//if ( g_byMcuModemNum != 0 )
	{
		//ֹͣMCU��MODEM
		StopConfModem( m_tConf.GetConfAttrbEx().GetSatDCastChnlNum() );
		
		//�ͷ�Ƶ��
		if ( m_tConf.GetConfAttrbEx().GetSatDCastChnlNum() != 0 )
		{
			ReleaseConfFreq();
		}
		g_cMcuVcApp.SetConfGetReceiveFreq( m_byConfIdx, FALSE );
		g_cMcuVcApp.SetConfGetSendFreq( m_byConfIdx, FALSE );
	}

	ReleaseSatFrequence();

	//�ͷ��ն�
	m_tConfAllMtInfo.RemoveAllJoinedMt();
	RefreshConfState();
	
#endif

	//��ջ���ʵ��
	ClearVcInst();

#ifdef _SATELITE_
	//RefreshConfState();
#endif

	//֪ͨ���л���,��ǰ�������ն�
	post( MAKEIID( GetAppID(), CInstance::EACH ), MCUVC_INVITE_UNJOINEDMT_TIMER );

	return;
}

/*====================================================================
    ������      ��AdjustMtSendBR
    ����        �������ն˷�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId    Ҫ���ڵ�MT
				  u16 wBitrate Ŀ������
				  u8 byMode    ��Ƶ���� MODE_VIDEO / MODE_SECVIDEO  
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/03/20    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::AdjustMtVideoSrcBR( u8 byMtId, u16 wBitrate, u8 byMode )
{
	if( CurState() == STATE_ONGOING )
	{
		CServMsg cServMsg;
		TLogicalChannel tLogicalChannel;
		if( MODE_VIDEO == byMode )
		{
			TMt tMtSrc;
			m_ptMtTable->GetMtSrc( byMtId, &tMtSrc, MODE_VIDEO );
			if( tMtSrc.GetType() == TYPE_MT && 
				tMtSrc.GetMtId() > 0 &&
				m_ptMtTable->GetMtLogicChnnl( tMtSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE ) )
			{
				tLogicalChannel.SetFlowControl( wBitrate );
				cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );

                //zbq[07/28/2009] ˫��flowctrl����������Ӧ���������ն˵�flowctrl��AdpParam��Ȼ����
                if (IsHDConf(m_tConf) &&
                    tMtSrc == m_tVidBrdSrc &&
                    m_cMtRcvGrp.IsMtNeedAdp(byMtId))
                {
                }
                else
                {
                    SendMsgToMt( tMtSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );
                }
			}
		}
		else
		{
			if( TYPE_MT == m_tDoubleStreamSrc.GetType() && 
				m_tConfAllMtInfo.MtJoinedConf( m_tDoubleStreamSrc.GetMtId() ) && 
				m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) )
			{
				tLogicalChannel.SetFlowControl( wBitrate );
				cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
				
                //zbq[07/28/2009] ˫��flowctrl����������Ӧ���������ն˵�flowctrl��AdpParam��Ȼ����
                if (IsHDConf(m_tConf) &&
                    m_cMtRcvGrp.IsMtNeedAdp(byMtId, FALSE))
                {
                }
                else
                {
                    SendMsgToMt( m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );
                }
			}
		}
	}

	return;
}

/*====================================================================
    ������      ��StartMtSendbyNeed
    ����        ������Ҫ��ʱ�����ն˿�ʼ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt & tSrc Ҫͣ�����ն�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/02/14    3.0         ������        ����
====================================================================*/
void CMcuVcInst::StartMtSendbyNeed( TSwitchChannel* ptSwitchChannel )
{
	if( CurState() == STATE_ONGOING )
	{	
		TMt tSrc =  ptSwitchChannel->GetSrcMt();

		if( !m_tConfAllMtInfo.MtJoinedConf( tSrc.GetMtId() ) )
		{
			return;
		}
		
		if( tSrc.GetType() == TYPE_MT && !ptSwitchChannel->IsNull() )
		{
			u8  byMode;
			if (ptSwitchChannel->GetRcvPort() % PORTSPAN == 2)
			{
				byMode = MODE_AUDIO;
			}
			else if (ptSwitchChannel->GetRcvPort() % PORTSPAN == 0)
			{
				byMode = MODE_VIDEO;
			}
			else
			{
				return;
			}

            u32 dwVmpIp = 0;
            u8 byEqpType = 0;
            if (!m_tVmpEqp.IsNull())
            {
                g_cMcuAgent.GetPeriInfo(m_tVmpEqp.GetEqpId(), &dwVmpIp, &byEqpType);
            }

            if (tSrc == m_tVidBrdSrc &&
                IsDelayVidBrdVCU() &&
                dwVmpIp == ptSwitchChannel->GetDstIP())
            {
                NotifyMtSend( tSrc.GetMtId(), byMode, TRUE );

                NotifyFastUpdate(tSrc, 0, TRUE);
                CallLog("[StartMtSendbyNeed] NotifyFastUpdate Mt.%d util here due to Tandberg!\n", tSrc.GetMtId());
            }
            else
            {
                NotifyMtSend( tSrc.GetMtId(), byMode, TRUE );
            }
        }
    }
    return;
}

/*====================================================================
������      ��StopMtSendbyNeed
����        ������Ҫ��ʱ�����ն�ͣ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const TMt & tSrc Ҫͣ�����ն�
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
04/02/14    3.0         ������        ����
====================================================================*/
void CMcuVcInst::StopMtSendbyNeed( TSwitchChannel* ptSwitchChannel )
{
	if( CurState() == STATE_ONGOING )
	{
		TMt tSrc =  ptSwitchChannel->GetSrcMt();

		if( !m_tConfAllMtInfo.MtJoinedConf( tSrc.GetMtId() ) )
		{
			return;
		}
		
		if( tSrc.GetType() == TYPE_MT )
		{
			u8  byMode;
			if (ptSwitchChannel->GetRcvPort() % PORTSPAN == 2)
			{
				byMode = MODE_AUDIO;
			}
			else if (ptSwitchChannel->GetRcvPort() % PORTSPAN == 0)
			{
				byMode = MODE_VIDEO;
			}
			else
			{
				return;
			}

			u16 wRcvPort = ptSwitchChannel->GetRcvPort();
//			u8  byMpId  = m_ptMtTable->GetMpId( tSrc.GetMtId() );
			u32 dwSrcIp = m_ptMtTable->GetIPAddr( tSrc.GetMtId() );
            u32 dwDstIp = ptSwitchChannel->GetDstIP();

//			if( byMode == MODE_AUDIO )
//			{
//			    NotifyMtSend( tSrc.GetMtId(), byMode, FALSE );
//			}
//			else
			{
                //ֻ���Ƴ��������֪ͨ�ն�ֹͣ������������ֹ�����flowcontrol 0
				if( !m_ptSwitchTable->IsValidSwitchSrcIp( dwSrcIp, wRcvPort, byMode ) )
				{
					//zbq[09/10/2008] �㲥�߼��¹㲥Դ�л���ʱ�����⣬�˴���ʱ���
					if (MT_TYPE_MT == m_tVidBrdSrc.GetMtType() &&
						m_tVidBrdSrc.GetMtId() == m_ptMtTable->GetMtIdByIp(dwSrcIp))
					{
						MpManagerLog("[StopMtSendbyNeed] Src<%s@%d> remove slapped due to it's vidsrc.(removed dst<%s@%d> switch)\n", 
									  StrOfIP(dwSrcIp), wRcvPort, StrOfIP(dwDstIp), ptSwitchChannel->GetDstPort() );						
					}
					else
					{
						MpManagerLog("[StopMtSendbyNeed] Src<%s@%d> Is a invalid switch src.(removed dst<%s@%d> switch)\n", 
									   StrOfIP(dwSrcIp), wRcvPort, StrOfIP(dwDstIp), ptSwitchChannel->GetDstPort() );
						NotifyMtSend( tSrc.GetMtId(), byMode, FALSE );
					}
				}
				else
				{
					MpManagerLog("[StopMtSendbyNeed] Src<%s@%d> Is a valid switch src.(removed dst<%s@%d> switch)\n", 
                                StrOfIP(dwSrcIp), wRcvPort, StrOfIP(dwDstIp), ptSwitchChannel->GetDstPort() );

                    // xsl [8/1/2006] ��4M���ϻ��飬�Ƴ�����ϳ�ͨ�����ն˻���������*�������������޸��ն˷�������
                    if (m_tConf.GetBitRate() > MAXBITRATE_MTSEND_INVMP)
                    {
                        u32 dwVmpIp; 
                        u8 byPeriType;
                        if (SUCCESS_AGENT == g_cMcuAgent.GetPeriInfo(m_tVmpEqp.GetEqpId(), &dwVmpIp, &byPeriType) &&
                            ntohl(dwVmpIp) == dwDstIp)
                        {
                            NotifyMtSend(tSrc.GetMtId(), MODE_VIDEO);
                        }
                        else if (SUCCESS_AGENT == g_cMcuAgent.GetPeriInfo(m_tVmpTwEqp.GetEqpId(), &dwVmpIp, &byPeriType) &&
                                ntohl(dwVmpIp) == dwDstIp)
                        {
                            NotifyMtSend(tSrc.GetMtId(), MODE_VIDEO);
                        }
                    }
				}
			}
		}
	}	
}

/*=============================================================================
�� �� ���� NotifyRecStartPlaybyNeed
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/2/27  4.0			�ܹ��                  ����
=============================================================================*/
void CMcuVcInst::NotifyRecStartPlaybyNeed( TSwitchChannel* ptSwitchChannel )
{
	CServMsg cServMsg;
	cServMsg.SetChnIndex( m_byPlayChnnl );
	if( ptSwitchChannel->GetSrcMt() == m_tPlayEqp && m_tConf.m_tStatus.IsNoPlaying() )
	{
		u8  byMode;
		if (ptSwitchChannel->GetRcvPort() % PORTSPAN == 2)
		{
			byMode = MODE_AUDIO;
		}
		else if (ptSwitchChannel->GetRcvPort() % PORTSPAN == 0)
		{
			byMode = MODE_VIDEO;
		}
		else if ( ptSwitchChannel->GetRcvPort() % PORTSPAN == 4 )
		{
			byMode = MODE_SECVIDEO;
		}
		else
		{
			return;
		}
			
		if( m_tPlayEqpAttrib.IsDStreamPlay() && m_tPlayEqpAttrib.IsDStreamFile() )
		{
			if( byMode == MODE_AUDIO || byMode == MODE_VIDEO )
			{
				//��Ƶ�͵�һ·��Ƶ��������
			}
			else
			{
				KillTimer( MCUVC_RECPLAY_WAITMPACK_TIMER );
				m_tConf.m_tStatus.SetPlaying();
				SendMsgToEqp( m_tPlayEqp.GetEqpId() , MCU_EQP_SWITCHSTART_NOTIF, cServMsg);
			}
		}
		else
		{
			if( byMode == MODE_AUDIO )
			{
				//������ڽ���VMP�㲥��������REC����֪ͨ��������Ƶ��������
				if ( m_tConf.m_tStatus.IsBrdstVMP() )
				{
					KillTimer( MCUVC_RECPLAY_WAITMPACK_TIMER );
					m_tConf.m_tStatus.SetPlaying();
					SendMsgToEqp( m_tPlayEqp.GetEqpId() , MCU_EQP_SWITCHSTART_NOTIF, cServMsg);
				}
            }
            else
            {
				KillTimer( MCUVC_RECPLAY_WAITMPACK_TIMER );
                m_tConf.m_tStatus.SetPlaying();
                SendMsgToEqp( m_tPlayEqp.GetEqpId() , MCU_EQP_SWITCHSTART_NOTIF, cServMsg);
			}
		}
	}
}

/*====================================================================
    ������      ��NotifyMtSend
    ����        ��ֱ֪ͨ���¼�MT����������ͬʱ֪ͨ�����ն�ͣ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byDstMtId, Ҫ֪ͨ��MT
				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
				  BOOL32 bStart, TRUE-��ʼ FALSE-ֹͣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/19    1.0         LI Yi         ����
	04/02/14    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::NotifyMtSend( u8 byDstMtId, u8 byMode, BOOL32 bStart )
{
	CServMsg	cServMsg;
	TLogicalChannel tLogicalChannel;
	u16  wBitRate;

    u8 byManuId = m_ptMtTable->GetManuId( byDstMtId );
	if(byManuId != MT_MANU_KDC && byManuId != MT_MANU_KDCMCU) // xsl [8/7/2006] ֧��mcu flowcontrol
	{
        //��keda�����ڽ�ʡ������Ƶ����������ʱ
        if((MODE_BOTH == byMode || MODE_VIDEO == byMode) && bStart && g_cMcuVcApp.IsSavingBandwidth())
        {
            NotifyOtherMtSend(byDstMtId, TRUE);
        }
		return;
	}

	//֪ͨ�ն˿�ʼ���͡�������͵�����Ƶ����������Ϊ64k
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		if( m_ptMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_AUDIO, &tLogicalChannel, FALSE ) )
		{
			if( bStart )
			{
				u16 wAudioBand = GetAudioBitrate( m_tConf.GetMainAudioMediaType() );
				tLogicalChannel.SetFlowControl( wAudioBand );
				cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
				SendMsgToMt( byDstMtId, MCU_MT_FLOWCONTROL_CMD, cServMsg );
				
				TMt tDstMt;
				tDstMt.SetNull();
				cServMsg.SetMsgBody( (u8*)&tDstMt, sizeof(tDstMt) );
				SendMsgToMt( byDstMtId, MCU_MT_SEENBYOTHER_NOTIF, cServMsg );
			}
			else
			{
				wBitRate =  0;		
				tLogicalChannel.SetFlowControl( wBitRate );
				cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
				SendMsgToMt( byDstMtId, MCU_MT_FLOWCONTROL_CMD, cServMsg );	
			}
		}
	}

	//���Ϊ��Ƶ�������ն˵�������Ϊ��������
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		if( m_ptMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_VIDEO, &tLogicalChannel, FALSE ) )
		{
			if( bStart )
			{
                // xsl [1/21/2006] �����˫��Դ���۰룬û����ȫ�٣�
                // zbq [6/23/2009] ��һ������˫��Դ�������ն˷�����Դ��Ҳ������˫������ȫ�٣�
                u16 wDialBitrate = 0;
				
#ifdef _SATELITE_
				wDialBitrate = m_ptMtTable->GetDialBitrate(byDstMtId);
#else
				wDialBitrate = m_ptMtTable->GetSndBandWidth(byDstMtId);
#endif
				if (m_tDoubleStreamSrc.IsNull())
                {
                    tLogicalChannel.SetFlowControl( wDialBitrate );
                }
                else
                {
                    TLogicalChannel tDSFwdChn;
                    if (!(m_tVidBrdSrc == m_ptMtTable->GetMt(byDstMtId)) &&
                        !m_ptMtTable->GetMtLogicChnnl(byDstMtId, LOGCHL_SECVIDEO, &tDSFwdChn, TRUE))
                    {
                        tLogicalChannel.SetFlowControl( wDialBitrate );
                    }
                    else
                    {
                        tLogicalChannel.SetFlowControl( GetDoubleStreamVideoBitrate(wDialBitrate ));
                    }
                }

				cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
				SendMsgToMt( byDstMtId, MCU_MT_FLOWCONTROL_CMD, cServMsg );

				TMt tDstMt;
				tDstMt.SetNull();
				cServMsg.SetMsgBody( (u8*)&tDstMt, sizeof(tDstMt) );
				SendMsgToMt( byDstMtId, MCU_MT_SEENBYOTHER_NOTIF, cServMsg );
			}
			else
			{               
				tLogicalChannel.SetFlowControl( 0 );
				cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
				SendMsgToMt( byDstMtId, MCU_MT_FLOWCONTROL_CMD, cServMsg );	
			}
		}
	}

#ifdef _SATELITE_
	if (byMode == MODE_NONE)
	{
		return;
	}

	//����Ҫ��MODEM����
	if ( g_byMcuModemNum == 0 ||
		 ::topoGetMtInfo( LOCAL_MCUID, byDstMtId, g_atMtTopo, g_wMtTopoNum).GetConnected() != 1 ||
		 ::topoGetMtInfo( LOCAL_MCUID, byDstMtId, g_atMtTopo, g_wMtTopoNum).GetModemIp() == 0 )
	{
		return;
	}

	if (bStart)
	{
		TMt tMt = m_ptMtTable->GetMt(byDstMtId);
		u8 byModemId = g_cMcuVcApp.GetConfMtUsedMcuModem( m_byConfIdx, tMt, TRUE );

		if (0 != byModemId &&
			MODE_AUDIO == byMode)
		{
			ConfLog(FALSE, "[NotifyMtSend] MT.%d has got modem.%d, ignore the new apply!\n", byDstMtId, byModemId);
			return;
		}

		u32 dwFreq;
		u32 dwBitRate;
		byModemId = g_cMcuVcApp.GetConfIdleMcuModem( m_byConfIdx, TRUE );
		if ( byModemId == 0 )
		{
			ConfLog(FALSE, "[NotifyMtSend] no idle rcv modem bind for MT.%d, bStart.%d!\n", byModemId, bStart);
			return;
		}
		
		g_cMcuVcApp.GetMcuModemRcvData( byModemId, dwFreq, dwBitRate );
		if ( dwFreq != 0)
		{
			TMt tMt = m_ptMtTable->GetMt(byDstMtId);
			
			//tMt.SetMt( LOCAL_MCUID, byDstMtId );
			cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
			cServMsg.CatMsgBody( (u8*)&dwFreq, sizeof(u32) );
			SatLog( "[NotifyMtSend] Mt-%u is send:Freq - %u, bStart.%d\n", byDstMtId, dwFreq, bStart );
			
			g_cMcuVcApp.SetMtOrEqpUseMcuModem( tMt, byModemId, TRUE, TRUE );
			g_cModemSsnApp.SendMsgToModemSsn( byDstMtId + MAXNUM_MCU_MODEM, MCU_MODEM_SEND, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		}
		else
		{
			OspPrintf( TRUE, FALSE, "Cannot Get Frq-%d for this Mt-%u, bStart.%d\n", dwFreq, byDstMtId, bStart );
		}
	}
	else
	{
		TMt tMt = m_ptMtTable->GetMt(byDstMtId);
		u8 byModemId = g_cMcuVcApp.GetConfMtUsedMcuModem( m_byConfIdx, tMt, TRUE );

		if ( byModemId != 0 ) 
		{
			cServMsg.SetMsgBody( (u8*)&tMt,sizeof(tMt) );

			g_cMcuVcApp.ReleaseConfUsedMcuModem( m_byConfIdx, tMt, TRUE );
			g_cModemSsnApp.SendMsgToModemSsn( byDstMtId + MAXNUM_MCU_MODEM, MCU_MODEM_STOPSEND, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );	
		}
	}

	m_ptMtTable->SetMtCurrUpLoad(byDstMtId, bStart);

	RefreshMtBitRate();
#endif

	return;

}

/*====================================================================
    ������      ��NotifyEqpSend
    ����        ��֪ͨ����(ֹͣ)������������Ҫ������ǻ����и��㲥������������/�ͷ�Modem��Դ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byDstMtId, Ҫ֪ͨ��EqpId
				  u8 byMode
				  BOOL32 bStart, ��ʼ���ջ���ֹͣ����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/11/19    4.6         �ű���         ����
====================================================================*/
void CMcuVcInst::NotifyEqpSend(u8 byDstEqpId, u8 byMode, BOOL32 bStart)
{
	if (!g_cMcuVcApp.IsPeriEqpConnected(byDstEqpId))
	{
		ConfLog(FALSE, "[NotifyEqpSend] Eqp.%d unexist, check it!\n", byDstEqpId);
		return;
	}
	TEqp tEqp = g_cMcuVcApp.GetEqp(byDstEqpId);

	// ��Ƶ����Ƶ�������OK.
	// ������Ҫ����Ƶ�ڶ������Vid��������ͷŷ���Modem
	// FIXME: ���߼��Ժ�������ϣ�Modem�Ͷ��������ͳһ������Ƶ����������Modem
	if (byMode == MODE_AUDIO)
	{
		ConfLog(FALSE, "[NotifyEqpSend] byMode.%d, needn't aud sec send, check it!\n");
		return;
	}


}

/*====================================================================
    ������      ��NotifyMtReceive
    ����        ��֪ͨ�ն�(ֹͣ)��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 bySrcMtId, ����Դ�ն˺�
				  u8 byDstMtId, Ҫ֪ͨ��MT
				  BOOL32 bStart, ��ʼ���ջ���ֹͣ����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/19    1.0         LI Yi         ����
	04/03/05    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::NotifyMtReceive(  const TMt & tSrc, u8 byDstMtId )
{
	TMt TNullMt, tSeeSrc;
	TNullMt.SetNull();//ԴΪNULL��������
	TNullMt.SetMcuId( tSrc.GetMcuId() );
	TMt tDstMt = m_ptMtTable->GetMt( byDstMtId );
	CServMsg cServMsg;

	MtLog("[NotifyMtReceive]mt(%d) see src(mtid:%d, mttype:%d, mtsubtype:%d)\n",
		    byDstMtId, tSrc.GetMtId(), tSrc.GetType(), tSrc.GetMtType());
	if( tSrc.GetType() == TYPE_MT  )
	{
		tSeeSrc = tSrc;
	}
    else if (TYPE_MCUPERI == tSrc.GetType() &&
             EQP_TYPE_BAS == tSrc.GetEqpType() &&
             m_cMtRcvGrp.IsMtNeedAdp(byDstMtId) &&
			 m_tVidBrdSrc.GetType() != TYPE_MCUPERI)
    {
        tSeeSrc = m_tVidBrdSrc;
    }
	else
	{
		tSeeSrc = TNullMt;
	}

	cServMsg.SetMsgBody( (u8*)&tSeeSrc, sizeof(tSeeSrc) );
	SendMsgToMt( tDstMt.GetMtId(), MCU_MT_YOUARESEEING_NOTIF, cServMsg );
}

/*=============================================================================
  �� �� ���� NotifyOtherMtSend
  ��    �ܣ� ֪ͨ���������ն��Ƿ�����Ƶ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byDstMtId
             BOOL32 bStart
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::NotifyOtherMtSend( u8 byDstMtId, BOOL32 bStart )
{
    CServMsg	cServMsg;
    TLogicalChannel tLogicalChannel;
    u16  wBitRate = 0;

    if( m_ptMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_VIDEO, &tLogicalChannel, FALSE ) )
    {
        if( bStart )
        {            
            //  xsl [1/21/2006] �����˫��Դ���۰룬û����ȫ��
            u16 wDialBitrate = m_ptMtTable->GetSndBandWidth(byDstMtId);
            if (!m_tDoubleStreamSrc.IsNull())
            {
                tLogicalChannel.SetFlowControl( GetDoubleStreamVideoBitrate(wDialBitrate) );
            }
            else
            {
                tLogicalChannel.SetFlowControl( wDialBitrate );
            }		
        }
        else
        {     
            tLogicalChannel.SetFlowControl( wBitRate );                
        }        

        cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
        SendMsgToMt( byDstMtId, MCU_MT_FLOWCONTROL_CMD, cServMsg );     
    }

    return;
}

/*=============================================================================
�� �� ���� IsNeedAdjustMtSndBitrate
��    �ܣ� �Ƿ���Ҫ�����ն˵ķ�������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMtId
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/22  4.0			������                  ����
=============================================================================*/
BOOL32 CMcuVcInst::IsNeedAdjustMtSndBitrate(u8 byMtId, u16 &wSndBitrate)
{   
    TMt tSrcMt = m_ptMtTable->GetMt(byMtId);    
    TMtStatus tStatus;   
    
    //�Ƿ����˻�˫��Դ
    if (tSrcMt == GetLocalSpeaker() || tSrcMt == m_tDoubleStreamSrc)
    {        
        //���ڷ�keda�ն���Ҫ�����ʽ�����ȥ�����Լ�����ͽ��մ���ʱ��Ҫ����Դ�ն�
        u8 bySrcMtId = 0;
        if (MT_MANU_KDC == m_ptMtTable->GetManuId(byMtId))
        {
            bySrcMtId = byMtId;
        }

        //˫�ٻ������
        if (0 != m_tConf.GetSecBitRate())
        {
            u16 wMinBitRate = m_ptMtTable->GetDialBitrate(byMtId);
            u16 wMtBitRate;
            
            for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
            {
                if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && bySrcMtId != byLoop)
                {
                    m_ptMtTable->GetMtStatus(byLoop, &tStatus);

                    wMtBitRate = m_ptMtTable->GetMtReqBitrate(byLoop, TRUE);
                    if (wMtBitRate != 0 &&                         
                        (wMtBitRate*(g_cMcuVcApp.GetBitrateScale()+100)/100) >= m_ptMtTable->GetDialBitrate(byMtId) &&
                        wMtBitRate < wMinBitRate)
                    {
                        wMinBitRate = wMtBitRate;
                    }

                    // guzh [3/13/2007] ˫��Դ�����ܱ�ѡ��
                    if ( tSrcMt == m_tDoubleStreamSrc &&
                         !(tSrcMt == m_tVidBrdSrc) &&
                         tStatus.GetSelectMt(MODE_VIDEO) == tSrcMt &&
                         wMtBitRate < wMinBitRate )
                    {
                        wMinBitRate = wMtBitRate;
                    }
                }
            }    
            if (wMinBitRate == m_tConf.GetBitRate())
            {
                return FALSE;
            }
            wSndBitrate = wMinBitRate;            
        }
        else
        {
            //����ֻȡͬ��ʽ�ն���С���� (������ʱ��������ն�ͨ������������)
            // guzh [9/19/2007] FIXME������ʽ���ԣ���ͼ�����ܴ������⣬�ڶ��������Ƿ�Ӧ�ô���src�ĸ�ʽ
            wSndBitrate = GetLeastMtReqBitrate(TRUE, m_tConf.GetMainVideoMediaType(), bySrcMtId);
        }        
                
        return TRUE;
    }   

    //�Ƿ������ն�ѡ��
	u16 wMinBitRate = 0xffff;
	u16 wMtReqBit = 0;
    for(u8 byIdx = 1; byIdx <= MAXNUM_CONF_MT; byIdx++)
    {       
        if (byMtId != byIdx && m_tConfAllMtInfo.MtJoinedConf(byIdx))
        {
            if (m_ptMtTable->GetMtStatus(byIdx, &tStatus))
            {
                if (tStatus.GetSelectMt(MODE_VIDEO) == tSrcMt)
                {                
                    // guzh [9/19/2007] FIXME�����Ǳ�����ն�ѡ�������
					// zgc [2008-01-22] ���޸�FIXME
                    wMtReqBit = m_ptMtTable->GetMtReqBitrate(byIdx, TRUE);
					wMinBitRate = ( wMinBitRate <= wMtReqBit ) ? wMinBitRate : wMtReqBit;
                }
            }
        }
    }	
	if ( wMinBitRate < 0xffff )
	{
		wSndBitrate = wMinBitRate;
		return TRUE;
	}

    return FALSE;
}

/*=============================================================================
  �� �� ���� IsMtSendingVideo
  ��    �ܣ� �жϷ�˫���ն��Ƿ��͵�һ·����
  �㷨ʵ�֣� ��˫��ʱ���ڵ�һ·���ʣ���˫��ʱ�ָ���һ·����
  ȫ�ֱ����� 
  ��    ���� u8 byMtId
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcInst::IsDStreamMtSendingVideo(TMt tMt)
{
	TMtStatus tStatus;	

	//�Ƿ�㲥Դ
	if (tMt == GetLocalSpeaker())
	{
		return TRUE;
	}

	//�Ƿ���¼��
	m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tStatus);
	if (tStatus.m_tRecState.IsRecording())
	{
		return TRUE;
	}

	//�Ƿ��ڻش�ͨ����
    if (!m_tCascadeMMCU.IsNull())
    {
        TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
        if (NULL != ptInfo &&
            m_tConfAllMtInfo.MtJoinedConf(ptInfo->m_tSpyMt.GetMcuId(), ptInfo->m_tSpyMt.GetMtId()) &&
            tMt == ptInfo->m_tSpyMt) 
        {   
            return TRUE;
        }        
    }   

	//�Ƿ������ն�ѡ��
	for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (byLoop == m_tDoubleStreamSrc.GetMtId() || !m_tConfAllMtInfo.MtJoinedConf(byLoop))
        {
            continue;
        }      
		
		if (m_ptMtTable->GetMtStatus(byLoop, &tStatus))
        {
            if (tStatus.GetSelectMt(MODE_VIDEO) == tMt)
            {               
                return TRUE;
            }
        }
    }

	return FALSE;
}

/*====================================================================
    ������      ��ChangeSelectSrc
    ����        ���ı�ѡ��Դ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/06/02    3.6         LI BO         ����
    06/04/24    4.0         �ű���        ���Դ���ն˵�������
====================================================================*/
BOOL CMcuVcInst::ChangeSelectSrc(TMt tSrcMt, TMt tDstMt, u8 byMode)
{
    TSwitchInfo  tSwitchInfo;
    tSwitchInfo.SetDstMt(tDstMt);
    tSwitchInfo.SetSrcMt(tSrcMt);
    
    if (m_tConf.m_tStatus.IsMixing())
    {
        tSwitchInfo.SetMode(MODE_VIDEO);
    }
    else
    {
        tSwitchInfo.SetMode(byMode);
    }
    
    //zbq[01/03/2008]��ֹ�¼�ѡ���ϼ�
    if ( !tDstMt.IsLocal() ||
         (tDstMt.IsLocal() && m_ptMtTable->GetMtType(tDstMt.GetMtId()) == MT_TYPE_SMCU) )
    {
        ConfLog(FALSE, "[ChangeSelectSrc] DstMt<%d,%d> isn't local, ignore it\n", tDstMt.GetMcuId(), tDstMt.GetMtId());
        return FALSE;
    }
    
    //ȡԴ�ն���Ŀ���ն˵�������
    TSimCapSet tSrcSCS;
    if (tSrcMt.IsLocal())
    {
        tSrcSCS = m_ptMtTable->GetSrcSCS( tSrcMt.GetMtId() );
    }
    else
    {
        TMt tLocalSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);
        tSrcSCS = m_ptMtTable->GetSrcSCS( tLocalSrcMt.GetMtId() );
    }    
    TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS( tDstMt.GetMtId() );
    
    //ѡ��ģʽ�Ͷ�Ӧ��������ƥ��
    BOOL32 bAccord2Adp = FALSE;
    u8 bySwitchMode = tSwitchInfo.GetMode();
    if ( !IsSelModeAndCapMatched( bySwitchMode, tSrcSCS, tDstSCS, bAccord2Adp ) ) 
    {
        OspPrintf( TRUE, FALSE, "[ChangeSelectSrc]Select mode.%d failed!\n", tSwitchInfo.GetMode() );
        return FALSE;
    }
    tSwitchInfo.SetMode( bySwitchMode );

    // ������Ҫ�����Ƿ��ǹ㲥����, zgc, 2008-06-03
    if( tSrcMt == m_tVidBrdSrc &&
        ( !( (tSrcMt==m_tRollCaller && GetLocalMtFromOtherMcuMt(tDstMt)==GetLocalMtFromOtherMcuMt(m_tRollCallee)) 
          || (tSrcMt==GetLocalMtFromOtherMcuMt(m_tRollCallee) && tDstMt==m_tRollCaller) ) ) &&
        ( MODE_VIDEO == tSwitchInfo.GetMode() || MODE_BOTH == tSwitchInfo.GetMode() ) )
    {
        ConfLog( FALSE, "[ChangeSelectSrc]Dst(Mt.%d) receive Src(Mt.%d) as video broadcast src!\n", tDstMt.GetMtId(), tSrcMt.GetMtId() );

        StartSwitchFromBrd( tSrcMt, 0, 1, &tDstMt );
        
        if ( MODE_BOTH == tSwitchInfo.GetMode() )
        {
            tSwitchInfo.SetMode( MODE_AUDIO );
        }
        else
        {
            // �������ֱ��return����Ϊ�����������ʵ�ʲ�û�з���ѡ��
            return TRUE;
        }
	}
    
    // guzh [2/28/2007] ��¼ѡ��
    TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
    tMtStatus.SetSelectMt( tSrcMt, tSwitchInfo.GetMode() );
    m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tMtStatus );
    
    CServMsg    cServMsgHdr;
    cServMsgHdr.SetEventId(MCS_MCU_STARTSWITCHMT_REQ);
    SwitchSrcToDst(tSwitchInfo, cServMsgHdr);    

    //ConfLog( FALSE, "Mt.%d select set Mt.%d, mode.%d\n", tDstMt.GetMtId(), tSrcMt.GetMtId(), byMode );
    ConfLog( FALSE, "Mt.%d select set Mt.%d, mode.%d\n", tDstMt.GetMtId(), tSrcMt.GetMtId(), tSwitchInfo.GetMode() );

    return TRUE;
}


/*====================================================================
    ������      ��StopSelectSrc
    ����        ��ֹͣ�ն�ѡ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt tDstMt, u8 byMode, u8 byIsRestore
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	07/11/14	4.0			�ű���         ����
====================================================================*/
void CMcuVcInst::StopSelectSrc( TMt tDstMt, u8 byMode, u8 byIsRestore )
{
	//���ʽѡ���ͷŶ�Ӧ��������Դ
	ReleaseResbySel(tDstMt, byMode);

    TMt tNullMt;
    TMtStatus tMtStatus; 

	TMt tSrc;
	TMtStatus tSrcStatus;
	
    //change mode if mixing
    if ( MODE_BOTH == byMode )
    {
        if( m_tConf.m_tStatus.IsMixing() )
        {
            byMode = MODE_VIDEO;
        }
    }
	 
    ConfLog(FALSE, "Mcu%dMt%d Cancel Select See.\n", tDstMt.GetMcuId(), tDstMt.GetMtId() );
    
    // guzh [7/28/2006] ѡ����Ŀ��������¼��ն�
    tDstMt = GetLocalMtFromOtherMcuMt(tDstMt);

	if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
	{
		ConfLog( FALSE, "[StopSelectSrc] Dst Mt%u-%u not joined conference!\n", 
			             tDstMt.GetMcuId(), tDstMt.GetMtId() );
		return;
	}
    // xsl [7/21/2006] �ָ��ն˽��յ�ַ
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
        {
            ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_AUDIO);
        }

        if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
        {
            ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_VIDEO);
        }                
    }           
	
	//���ѡ��״̬, ����϶���������е�ѡ��״̬�������������
	m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
	tMtStatus.RemoveSelByMcsMode( byMode );

	// zgc, 2008-01-22, ���µ���ѡ��Դ�ķ�������,��¼ѡ��Դ
	tSrc = tMtStatus.GetSelectMt( MODE_VIDEO );

	// ����ϴ�ѡ���ն�		
	tNullMt.SetNull();
    // ����@2006.4.17 ����ѡ������
	tMtStatus.SetSelectMt(tNullMt, byMode);

	m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tMtStatus );

	// xliang [1/9/2009] �������ѡ������MT����EQP(Ŀǰ��VMP)
	if(tSrc.GetType() == TYPE_MT)
	{
		// xliang [12/12/2008] ѡ��״̬���ú��ж��Ƿ�����MPU�е�ȡ��ѡ��
		//�ǣ��򽵸�MT�ķֱ���
		TPeriEqpStatus tPeriEqpStatus; 
		g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
		u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
		
		if ( (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE 	//������vmp��
			&& m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tSrc))	//ѡ��ԴMT��vmp�ϳɳ�Ա��
			&& ( byVmpSubType != VMP)	//�õ�VMP����ǰ����ͨ����VMP
			) 
		{	
			//���ֱ���// xliang [4/21/2009] 
			TVMPParam  tVMPParam   = m_tConf.m_tStatus.GetVmpParam();
			u8  byVmpStyle = tVMPParam.GetVMPStyle();  
			u8	byChlPos = tVMPParam.GetChlOfMtInMember(tSrc);
			
			ChangeMtVideoFormat(tSrc, &tVMPParam, TRUE, FALSE);
		}
		// ���µ���ѡ��Դ�ķ������ʣ�zgc, 2008-01-22
		m_ptMtTable->GetMtStatus(tSrc.GetMtId(), &tSrcStatus);
		if ( m_tConfAllMtInfo.MtJoinedConf( tSrc.GetMtId() ) 
			&& tSrcStatus.IsSendVideo() 
			&& (MODE_VIDEO == byMode || MODE_BOTH == byMode)
			)
		{
			NotifyMtSend( tSrc.GetMtId(), MODE_VIDEO );
		}
	}
	
	//���з����ˣ�����ѡ��������
	if ( byIsRestore )
	{
#ifdef _SATELITE_
		g_cMpManager.StopSwitchSrc2Dst(tSrc, tDstMt, byMode);
		RefreshConfState();
		

		//FIXME: ��ʱ����������
		if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
		{
			if (!m_tAudBrdSrc.IsNull())
			{
				m_ptMtTable->SetMtSrc(tDstMt.GetMtId(), &m_tAudBrdSrc, MODE_AUDIO);
			}
		}
		if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
		{
			if (!m_tVidBrdSrc.IsNull())
			{
				m_ptMtTable->SetMtSrc(tDstMt.GetMtId(), &m_tVidBrdSrc, MODE_VIDEO);
			}
		}
#else
		RestoreRcvMediaBrdSrc( tDstMt.GetMtId(), byMode );
#endif
	}
    // zgc, 2008-05-28, ����Ҫ���½��ܹ㲥Դ��ֱ��ͣ����
    else
    {

#ifdef _SATELITE_
		g_cMpManager.StopSwitchSrc2Dst(tSrc, tDstMt, byMode);
		RefreshConfState();
		
#else
		StopSwitchToSubMt( tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT );
#endif
    }
    
    //��ʱ�������߼����ǣ�ֱ������һ��
//     BOOL32 bAccord2Adp = FALSE;
//     TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
//     TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS(tDstMt.GetMtId());
// 
//     u8 bySelMode = (MODE_VIDEO == byMode || MODE_BOTH == byMode) ? MODE_VIDEO : MODE_NONE;
//     if (MODE_VIDEO == bySelMode &&
//         IsSelModeAndCapMatched(bySelMode, tSrcSCS, tDstSCS, bAccord2Adp))
//     {
//         if (!g_cMcuVcApp.IsSelAccord2Adp() || !bAccord2Adp)
//         {
//             return;
//         }
// 
//         StopHdVidSelAdp(tSrc, tDstMt, MODE_VIDEO);
//     }
    MtStatusChange( tDstMt.GetMtId(), TRUE );

    return;
}

/*====================================================================
    ������      ��ChangeChairman
    ����        ���ı���ϯ������������������
	              ֪ͨMC��MT���¼�MCU��
				  ����¡�����ϯֱ����֪ͨ������ı�״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt * ptNewChairman, ����ϯ��NULL��ʾ��������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/01/04    1.0         LI Yi         ����
	03/11/25    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::ChangeChairman( TMt * ptNewChairman )
{
	CServMsg	cServMsg;
	TMt		tOldChairman = m_tConf.GetChairman();
	cServMsg.SetConfId( m_tConf.GetConfId() );
	//TMt    tSpeakerSrc;
	BOOL32 bHasOldChairman = FALSE;

    TPeriEqpStatus tTWStatus;
    TPeriEqpStatus tHduStatus;
	TPeriEqpStatus tVmpStatus;

    u8 byChnlIdx;
    u8 byEqpId;

	if(ptNewChairman != NULL)
    {
	    *ptNewChairman = GetLocalMtFromOtherMcuMt(* ptNewChairman);
    }
			    
	//ȡ��ԭ��ϯ
	if( HasJoinedChairman() )
	{	
		bHasOldChairman = TRUE;

		cServMsg.SetMsgBody( ( u8 * )&tOldChairman, sizeof( tOldChairman ) );
		cServMsg.SetNoSrc();
		SendMsgToMt( tOldChairman.GetMtId(), MCU_MT_CANCELCHAIRMAN_NOTIF,cServMsg );
		m_tConf.SetNoChairman();
		ConfLog( FALSE, "Old chairman MT%u was cancelled!\n", tOldChairman.GetMtId() );

		// xliang [4/2/2009] ��ϯѡ��VMP��ʱʧЧ
		if(m_tConf.m_tStatus.IsVmpSeeByChairman())
		{
			m_tConf.m_tStatus.SetVmpSeebyChairman(FALSE);
			// ״̬ͬ��ˢ�µ�TPeriStatus��
			g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
			tVmpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
			g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
			//�彻����ѡ��Դ״̬
			StopSelectSrc(tOldChairman,MODE_VIDEO);
		}
		/* zgc, 2008-05-05, ����Ĵ��벻��Ҫ������������ϯ�����ĺ�����ͳһ����
		//��������˵�Դ����ϯ
		if( HasJoinedSpeaker() )
		{
			// ��Ƶ
			m_ptMtTable->GetMtSrc( GetLocalSpeaker().GetMtId(), &tSpeakerSrc, MODE_VIDEO );
			if( tSpeakerSrc == tOldChairman && 
				m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_CHAIR && 
				ptNewChairman == NULL &&
				// ����ǻ����ڲ��߼�ѡ������Ҫֹͣ, zgc, 2008-04-14
				SPEAKER_SRC_CONFSEL == GetSpeakerSrcSpecType( MODE_VIDEO ) )
			{
                TMt tLocalSpeaker = GetLocalSpeaker();
				if( m_ptMtTable->GetManuId( tLocalSpeaker.GetMtId() ) == MT_MANU_KDC )
				{
					//StopSwitchToSubMt( tLocalSpeaker.GetMtId(), MODE_BOTH, SWITCH_MODE_SELECT, TRUE );
					StopSwitchToSubMt( tLocalSpeaker.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT, TRUE );

                    // �����ǰ�л�������N-1ģʽ�Ļ��������ָ������������� [12/20/2006-zbq]
					// ��Ƶ��������zgc, 2008-04-14					
                    //if ( m_tConf.m_tStatus.IsMixing() )
                    //{
                    //   SwitchMixMember( &tLocalSpeaker, FALSE );
                    //}
					
				}
				else
				{
                    //��kdc���̻ش�������Ƶ, ��ʡ����ʱ��֪ͨmt��������
                    if(g_cMcuVcApp.IsSavingBandwidth())
                    {
                        NotifyOtherMtSend(tLocalSpeaker.GetMtId(), TRUE);
                    }                    
					
					// ��Ƶ��������zgc, 2008-04-25
					//u8 byMode = m_tConf.m_tStatus.IsMixing() ? MODE_VIDEO : MODE_BOTH;
					//StartSwitchToSubMt( tLocalSpeaker, 0, tLocalSpeaker.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE );
					StartSwitchToSubMt( tLocalSpeaker, 0, tLocalSpeaker.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT, TRUE );
				}
			}

			//��Ƶ��������
			m_ptMtTable->GetMtSrc( GetLocalSpeaker().GetMtId(), &tSpeakerSrc, MODE_AUDIO );
			if( tSpeakerSrc == tOldChairman && 
				m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_CHAIR && 
				ptNewChairman == NULL &&
				m_tConf.GetConfAttrb().GetSpeakerSrcMode() == MODE_BOTH &&
				// ����ǻ����Զ��趨����Ҫֹͣ, zgc, 2008-04-14
				SPEAKER_SRC_CONFSEL == GetSpeakerSrcSpecType( MODE_AUDIO ) )
			{
                TMt tLocalSpeaker = GetLocalSpeaker();
				if( m_ptMtTable->GetManuId( tLocalSpeaker.GetMtId() ) == MT_MANU_KDC )
				{
					StopSwitchToSubMt( tLocalSpeaker.GetMtId(), MODE_AUDIO, SWITCH_MODE_SELECT, TRUE );
					
                    // �����ǰ�л�������N-1ģʽ�Ļ��������ָ������������� [12/20/2006-zbq]
                    if ( m_tConf.m_tStatus.IsMixing() )
                    {
                        SwitchMixMember( &tLocalSpeaker, FALSE );
                    }
				}
				else
				{
                    //��kdc���̻ش�������Ƶ, ��ʡ����ʱ��֪ͨmt��������
                    //if(g_cMcuVcApp.IsSavingBandwidth())
                    //{
                    //    NotifyOtherMtSend(tLocalSpeaker.GetMtId(), TRUE);
                    //}
					//u8 byMode = m_tConf.m_tStatus.IsMixing() ? MODE_VIDEO : MODE_BOTH;
					//StartSwitchToSubMt( tLocalSpeaker, 0, tLocalSpeaker.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE );
					
					if ( !m_tConf.m_tStatus.IsMixing() )
					{
						StartSwitchToSubMt( tLocalSpeaker, 0, tLocalSpeaker.GetMtId(), MODE_AUDIO, SWITCH_MODE_SELECT, TRUE );
					}
				}
			}
		}*/

        //ǿ�ƹ㲥�£��ָ�����ƵԴ[01/24/2007-zbq]
        if ( m_tConf.m_tStatus.IsMustSeeSpeaker() )
        {
            //ָ���ָ�������Ƶ�㲥ý��Դ
            RestoreRcvMediaBrdSrc( tOldChairman.GetMtId(), MODE_BOTH );
        }        

        for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
        {
            if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
            {
                for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
                {
                    if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus) &&
                        tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType == TW_MEMBERTYPE_CHAIRMAN &&
                        tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx() == m_byConfIdx)
                    {
                        ChangeTvWallSwitch(&tOldChairman, byEqpId, byChnlIdx, TW_MEMBERTYPE_CHAIRMAN, TW_STATE_STOP, FALSE);
                    }
                }
            }
        }

		for (byEqpId = HDUID_MIN; byEqpId <= HDUID_MAX; byEqpId++)
        {
            if (EQP_TYPE_HDU == g_cMcuVcApp.GetEqpType(byEqpId))
            {
                for (byChnlIdx = 0; byChnlIdx < MAXNUM_HDU_CHANNEL; byChnlIdx++)
                {
                    if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tHduStatus) &&
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType == TW_MEMBERTYPE_CHAIRMAN &&
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx() == m_byConfIdx &&
						NULL == ptNewChairman)
                    {

						ChangeHduSwitch(&tOldChairman, byEqpId, byChnlIdx, TW_MEMBERTYPE_CHAIRMAN, TW_STATE_STOP);
                    }
                }
            }
        }


		//��ӦԶҡ����MCU����������������е�����MCU��һ��ȡ����ϯ����
        if (g_cMcuVcApp.IsApplyChairToZxMcu())
		{
			for ( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId  ++ )
			{
				if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
					MT_MANU_ZTEMCU == m_ptMtTable->GetManuId(byMtId) )
				{            
					CServMsg cSerMsg;
					SendMsgToMt( byMtId, MCU_MT_CANCELCHAIRMAN_CMD, cSerMsg );                
				}
			}
		}
	}

	//��������ϯ
	if( ptNewChairman != NULL )
	{	
		cServMsg.SetMsgBody( ( u8 * )ptNewChairman, sizeof( TMt ) );
		cServMsg.SetNoSrc();
		SendMsgToMt( ptNewChairman->GetMtId(), MCU_MT_SPECCHAIRMAN_NOTIF,cServMsg );
		m_tConf.SetChairman( *ptNewChairman );

		ConfLog( FALSE, "New chairman MT%u is specified!\n", ptNewChairman->GetMtId() );

        //��ӦԶҡ����MCU����������������е�����MCU��һ��������ϯ���� [zbq 06-04-20]
        if (g_cMcuVcApp.IsApplyChairToZxMcu())
		{
			for ( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId  ++ )
			{
				if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
					MT_MANU_ZTEMCU == m_ptMtTable->GetManuId(byMtId) )
				{            
					CServMsg cSerMsg;
					SendMsgToMt( byMtId, MCU_MT_APPLYCHAIRMAN_CMD, cSerMsg );                
				}
			}
		}
	}

    //֪ͨ���л��
    TBasicConfInfo tBasicConfInfo;
    tBasicConfInfo.SetSpeaker(m_tConf.GetSpeaker());
    tBasicConfInfo.SetChairman(m_tConf.GetChairman());
	cServMsg.SetMsgBody( ( u8 * )&tBasicConfInfo, sizeof( tBasicConfInfo ) );
	SendMsgToAllMcs( MCU_MCS_SIMCONFINFO_NOTIF, cServMsg );
	
	//֪ͨ�����ն�
	//��֪ͨ�ն�ʹ�õĻ�����Ϣ�ṹ 2005-10-11
	TConfStatus tConfStatus = m_tConf.GetStatus();
	TSimConfInfo tSimConfInfo;
	tSimConfInfo.m_tSpeaker  = GetLocalSpeaker();
	tSimConfInfo.m_tChairMan = m_tConf.GetChairman();
	tSimConfInfo.SetVACMode(tConfStatus.IsVACing());
	tSimConfInfo.SetVMPMode(tConfStatus.GetVMPMode());
    // guzh [11/6/2007] 
	tSimConfInfo.SetDiscussMode(tConfStatus.IsMixing());
	cServMsg.SetMsgBody( ( u8 * )&tSimConfInfo, sizeof( tSimConfInfo ) );
	BroadcastToAllSubMtJoinedConf( MCU_MT_SIMPLECONF_NOTIF, cServMsg );
    if (NULL != ptNewChairman)
    {
        TMt tSpeaker = m_tConf.GetSpeaker();
        m_tConf.SetSpeaker(GetLocalSpeaker());
        cServMsg.SetMsgBody((u8 *)&m_tConf, sizeof(TConfInfo));
        SendMsgToMt(ptNewChairman->GetMtId(), MCU_MT_CONF_NOTIF, cServMsg);
        m_tConf.SetSpeaker(tSpeaker);
    }

    //zbq[09/24/2007] ��ϯ�ն˵ĺ���Ƶ��ͨ���򿪣�������ؽ���
    TLogicalChannel tLogicChan;
    //zbq [10/11/2007] ������ȡ����ϯ ����
	//zgc [04/21/2008] ��Ƶ��Ƶ�ֿ�����
	/*
    if ( NULL == ptNewChairman ||
         ( NULL != ptNewChairman &&
           m_ptMtTable->GetMtLogicChnnl(ptNewChairman->GetMtId(), LOGCHL_VIDEO, &tLogicChan, FALSE)))
    {
		TMt *ptOldChairman = NULL;
		if ( bHasOldChairman )
		{
			ptOldChairman = &tOldChairman;
		}
        AdjustChairmanSwitch( ptOldChairman );
    }*/
	// ��Ƶͨ��
	if ( NULL == ptNewChairman ||
		( NULL != ptNewChairman &&
           m_ptMtTable->GetMtLogicChnnl(ptNewChairman->GetMtId(), LOGCHL_AUDIO, &tLogicChan, FALSE)))
	{
		AdjustChairmanAudSwitch();
	}
	// ��Ƶͨ��
	if ( NULL == ptNewChairman ||
		( NULL != ptNewChairman &&
		m_ptMtTable->GetMtLogicChnnl(ptNewChairman->GetMtId(), LOGCHL_VIDEO, &tLogicChan, FALSE)))
	{
		TMt *ptOldChairman = NULL;
		if ( bHasOldChairman )
		{
			ptOldChairman = &tOldChairman;
		}
		AdjustChairmanVidSwitch( ptOldChairman );
	}
	

    //n+1���ݸ�����ϯ��Ϣ
    if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
    {
        ProcNPlusChairmanUpdate();
    }

#ifdef _SATELITE_
    RefreshConfState();
	
#endif

    return;
}


/*====================================================================
    ������      ��AdjustChairmasSwitch
    ����        ��������ǰ��ϯ����ؽ������ĸ����潨��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/09/24    4.0         �ű���        ����
====================================================================*/
void CMcuVcInst::AdjustChairmanSwitch( TMt *ptOldChair )
{
    //zbq [10/11/2007] ������ȡ����ϯ ����
    /*if ( !HasJoinedChairman() )
    {
        ConfLog( FALSE, "[AdjustChairmanSwitch] No chairman, ignore it\n" );
        return;
    }*/
    
    TMt tChairMan = m_tConf.GetChairman();

	//�����˿���ϯʱ��������Դ�ĵ���, zgc, 2008-02-25
	u8 bySelMode = m_tConf.GetConfAttrb().GetSpeakerSrcMode();
	ChangeSpeakerSrc( bySelMode, emReasonChangeChairman );

	// zgc, 2007-12-24,
	// ����VMP��VMPTW�������ͨ���Ĵ��� �� �������ǽ����ͨ��֮���Ƶ� 
	// �����߼�ͨ���Ƿ���ж�֮�ϣ���Ϊ ChangeVmpChannelParam �� ChangeVmpTwChannelParam
	// ��˫������������Ҫ�жϺ����߼�ͨ���Ƿ��
	//���뻭��ϳɸ���ͨ��
    if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE)
    {
        // xsl [10/27/2006]���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
        if (m_tConf.GetConfAttrb().IsSatDCastMode() &&
            m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_CHAIRMAN))
        {
            if (IsOverSatCastChnnlNum(tChairMan.GetMtId()))
            {
                ConfLog(FALSE, "[ChangeChairman] over max upload mt num. nack!\n");            
                NotifyMcsAlarmInfo(0, ERR_MCU_DCAST_OVERCHNNLNUM);
                return;
            }
        }    
        
        if (IsChangeVmpParam(&tChairMan) &&
            // zbq [05/31/2007] ������VMPͨ����������ϯ���棬����Ҫ����ͨ�����
            ( m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_CHAIRMAN) ||
			 m_tConf.m_tStatus.GetVmpParam().IsMtInMember( tChairMan ) || 
			 ( ptOldChair != NULL && m_tConf.m_tStatus.GetVmpParam().IsMtInMember( *ptOldChair ) ) )
		)
        {
            ChangeVmpChannelParam(&tChairMan, VMP_MEMBERTYPE_CHAIRMAN, ptOldChair);
        }
        else
        {
            SetTimer(MCUVC_CHANGE_VMPCHAN_CHAIRMAN_TIMER, TIMESPACE_CHANGEVMPPARAM, VMP_MEMBERTYPE_CHAIRMAN);
        }
    }
    
    //����໭�����ǽ����ͨ��
    if (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE)
    {
        // xsl [10/27/2006]���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
        if (m_tConf.GetConfAttrb().IsSatDCastMode() &&
            m_tConf.m_tStatus.GetVmpTwParam().IsTypeInMember(VMPTW_MEMBERTYPE_CHAIRMAN))
        {
            if (IsOverSatCastChnnlNum(tChairMan.GetMtId()))
            {
                ConfLog(FALSE, "[ChangeChairman] over max upload mt num. nack!\n");            
                NotifyMcsAlarmInfo(0, ERR_MCU_DCAST_OVERCHNNLNUM);
                return;
            }
        }    
        
        ChangeVmpTwChannelParam(&tChairMan, VMPTW_MEMBERTYPE_CHAIRMAN);
    }

    TLogicalChannel tLogicChan;
    if ( !m_ptMtTable->GetMtLogicChnnl(tChairMan.GetMtId(), LOGCHL_VIDEO, &tLogicChan, FALSE))
    {
        ConfLog( FALSE, "[AdjustChairmanSwitch] ChairMt.%d's RL chan unexist, ignore it\n", tChairMan.GetMtId() );
        return;
    }
        
	/*
    if( HasJoinedSpeaker() && 
        HasJoinedChairman() && 
        m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_CHAIR )
    {
        if( //!m_tConf.m_tStatus.IsBrdstVMP() &&
            //!( m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO ) && 
            ( m_tConf.GetConfAttrb().GetSpeakerSrcMode() == MODE_VIDEO || 
            m_tConf.GetConfAttrb().GetSpeakerSrcMode() == MODE_BOTH ) )
        {
            u8 bySelMode = m_tConf.GetConfAttrb().GetSpeakerSrcMode();
            ChangeSelectSrc(tChairMan, GetLocalSpeaker(), bySelMode);
        }
    }
	*/

    /*
    //����з����ˣ���ѷ����˵�ǰ��ƵԴ�ŷ�����ϯ(���ն�Ҫ֪����ǰ��ƵԴ�ţ���Ҫ���ӷǱ���Ϣ֪ͨ)        
    if (m_tConf.HasSpeaker())
    {
        CServMsg cServMsgTmp;
        TMt tMtSpeaker = m_tConf.GetSpeaker();
        u8 byCurrVidSrcNo = m_ptMtTable->GetCurrVidSrcNo(tMtSpeaker.GetMtId());

        cServMsgTmp.SetSrcMtId(tMtSpeaker.GetMtId());
        cServMsgTmp.SetMsgBody((u8 *)ptNewChairman, sizeof(TMt));
        cServMsgTmp.CatMsgBody((u8 *)&byCurrVidSrcNo, sizeof(u8));
        MtVideoSourceSwitched(cServMsgTmp);
    }
    */
  
    //�������ǽ����ͨ��
	TPeriEqpStatus tTWStatus;
    u8 byChnlIdx;
    u8 byEqpId;
    for (byEqpId = 1; byEqpId <= MAXNUM_MCU_PERIEQP; byEqpId++)
    {
        if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
        {
            for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
            {
                if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus) &&
                    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType == TW_MEMBERTYPE_CHAIRMAN &&
                    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx() == m_byConfIdx)
                {
                    // xsl [10/27/2006]���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
                    if (m_tConf.GetConfAttrb().IsSatDCastMode())
                    {
                        if (IsOverSatCastChnnlNum(tChairMan.GetMtId()))
                        {
                            ConfLog(FALSE, "[ChangeChairman] over max upload mt num. nack!\n");            
                            NotifyMcsAlarmInfo(0, ERR_MCU_DCAST_OVERCHNNLNUM);
                            return;
                        }
                    }    
                    
                    ChangeTvWallSwitch(&tChairMan, byEqpId, byChnlIdx, TW_MEMBERTYPE_CHAIRMAN, TW_STATE_CHANGE);
                }
            }
        }
    }
    
    return;
}

/*=============================================================================
�� �� ���� AdjustChairmanVidSwitch
��    �ܣ� ������ϯ��Ƶ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TMt *ptOldChair = NULL
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/21   4.0			�ܹ��                  ����
=============================================================================*/
void CMcuVcInst::AdjustChairmanVidSwitch( TMt *ptOldChair )
{
	//zbq [10/11/2007] ������ȡ����ϯ ����
    /*if ( !HasJoinedChairman() )
    {
        ConfLog( FALSE, "[AdjustChairmanSwitch] No chairman, ignore it\n" );
        return;
    }*/
    
    TMt tChairMan = m_tConf.GetChairman();

	//�����˿���ϯʱ��������Դ�ĵ���, zgc, 2008-02-25
	u8 bySelMode = m_tConf.GetConfAttrb().GetSpeakerSrcMode();
	// ���ӶԷ�����Դ������ģʽ���жϣ���ֹ�������zgc, 2008-04-21
	if ( bySelMode == MODE_VIDEO || bySelMode == MODE_BOTH )
	{
		ChangeSpeakerSrc( MODE_VIDEO, emReasonChangeChairman );
	}

	// zgc, 2007-12-24,
	// ����VMP��VMPTW�������ͨ���Ĵ��� �� �������ǽ����ͨ��֮���Ƶ� 
	// �����߼�ͨ���Ƿ���ж�֮�ϣ���Ϊ ChangeVmpChannelParam �� ChangeVmpTwChannelParam
	// ��˫������������Ҫ�жϺ����߼�ͨ���Ƿ��
	//���뻭��ϳɸ���ͨ��
    if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE)
    {
        // xsl [10/27/2006]���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
        if (m_tConf.GetConfAttrb().IsSatDCastMode() &&
            m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_CHAIRMAN))
        {
            if (IsOverSatCastChnnlNum(tChairMan.GetMtId()))
            {
                ConfLog(FALSE, "[ChangeChairman] over max upload mt num. nack!\n");            
                NotifyMcsAlarmInfo(0, ERR_MCU_DCAST_OVERCHNNLNUM);
                return;
            }
        }    
        
        if (IsChangeVmpParam(&tChairMan) &&
            // zbq [05/31/2007] ������VMPͨ����������ϯ���棬����Ҫ����ͨ�����
            ( m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_CHAIRMAN) ||
			 m_tConf.m_tStatus.GetVmpParam().IsMtInMember( tChairMan ) || 
			 ( ptOldChair != NULL && m_tConf.m_tStatus.GetVmpParam().IsMtInMember( *ptOldChair ) ) )
		)
        {
			ConfLog(FALSE, "[AdjustChairmanVidSwitch] begin to call FUNCTION[ChangeVmpChannelParam]!\n");
            ChangeVmpChannelParam(&tChairMan, VMP_MEMBERTYPE_CHAIRMAN, ptOldChair);
        }
        else
        {
            SetTimer(MCUVC_CHANGE_VMPCHAN_CHAIRMAN_TIMER, TIMESPACE_CHANGEVMPPARAM, VMP_MEMBERTYPE_CHAIRMAN);
        }
    }
    
    //����໭�����ǽ����ͨ��
    if (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE)
    {
        // xsl [10/27/2006]���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
        if (m_tConf.GetConfAttrb().IsSatDCastMode() &&
            m_tConf.m_tStatus.GetVmpTwParam().IsTypeInMember(VMPTW_MEMBERTYPE_CHAIRMAN))
        {
            if (IsOverSatCastChnnlNum(tChairMan.GetMtId()))
            {
                ConfLog(FALSE, "[ChangeChairman] over max upload mt num. nack!\n");            
                NotifyMcsAlarmInfo(0, ERR_MCU_DCAST_OVERCHNNLNUM);
                return;
            }
        }    
        
        ChangeVmpTwChannelParam(&tChairMan, VMPTW_MEMBERTYPE_CHAIRMAN);
    }


	if(tChairMan.IsNull())	
	{
		ConfLog( FALSE, "[AdjustChairmanVidSwitch] no new chairman, ignore further process!\n");
		return;
	}
    TLogicalChannel tLogicChan;
    if ( !m_ptMtTable->GetMtLogicChnnl(tChairMan.GetMtId(), LOGCHL_VIDEO, &tLogicChan, FALSE))
    {
        ConfLog( FALSE, "[AdjustChairmanVidSwitch] ChairMt.%d's video RL chan unexist, ignore it\n", tChairMan.GetMtId() );
        return;
    }
  
    //�������ǽ����ͨ��
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
                    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType == TW_MEMBERTYPE_CHAIRMAN &&
                    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx() == m_byConfIdx)
                {
                    // xsl [10/27/2006]���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
                    if (m_tConf.GetConfAttrb().IsSatDCastMode())
                    {
                        if (IsOverSatCastChnnlNum(tChairMan.GetMtId()))
                        {
                            ConfLog(FALSE, "[ChangeChairman] over max upload mt num. nack!\n");            
                            NotifyMcsAlarmInfo(0, ERR_MCU_DCAST_OVERCHNNLNUM);
                            return;
                        }
                    }    
                    
                    ChangeTvWallSwitch(&tChairMan, byEqpId, byChnlIdx, TW_MEMBERTYPE_CHAIRMAN, TW_STATE_CHANGE);
                }
            }
        }
    }
    
    //����HDU����ͨ��
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
                    tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].byMemberType == TW_MEMBERTYPE_CHAIRMAN &&
                    tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetConfIdx() == m_byConfIdx)
                {
                    // xsl [10/27/2006]���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
                    if (m_tConf.GetConfAttrb().IsSatDCastMode())
                    {
                        if (IsOverSatCastChnnlNum(tChairMan.GetMtId()))
                        {
                            ConfLog(FALSE, "[ChangeChairman] over max upload mt num. nack! by hdu\n");            
                            NotifyMcsAlarmInfo(0, ERR_MCU_DCAST_OVERCHNNLNUM);
                            return;
                        }
                    }    
                    
                    ChangeHduSwitch(&tChairMan, byHduEqpId, byHduChnlIdx, TW_MEMBERTYPE_CHAIRMAN, TW_STATE_CHANGE);
                }
            }
        }
    }
	
    return;
}

/*=============================================================================
�� �� ���� AdjustChairmanAudSwitch
��    �ܣ� ������ϯ��Ƶ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/21   4.0		�ܹ��                  ����
=============================================================================*/
void CMcuVcInst::AdjustChairmanAudSwitch( void )
{
	//�����˿���ϯʱ��������Դ�ĵ���, zgc, 2008-02-25
	u8 bySelMode = m_tConf.GetConfAttrb().GetSpeakerSrcMode();

    // ���ӶԷ�����Դ������ģʽ���жϣ���ֹ�������zgc, 2008-04-21
	if ( bySelMode == MODE_BOTH )
	{
		ChangeSpeakerSrc( MODE_AUDIO, emReasonChangeChairman );
	}
}

/*====================================================================
    ������      ��ChangeVidBrdSrc
    ����        ���ı���Ƶ�㲥Դ������������������
	              ֪ͨMC��MT
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt * ptNewVidBrdSrc, �µ���Ƶ�㲥Դ��NULL��ʾ��������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/25    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ChangeVidBrdSrc(TMt * ptNewVidBrdSrc)
{
    CServMsg cServMsg;
    u8	bySrcChnnl;
    u16 wAdaptBitRate = 0;  //modify bas 2
    TSimCapSet tSrcSimCapSet;
    TSimCapSet tDstSimCapSet;
    TLogicalChannel tLogicChnnl;
    TLogicalChannel tLogicalChannel;
    TLogicalChannel tH239LogicChnnl;
    TLogicalChannel tNullLogicChnnl;

    TMt tOldSrc;
    tOldSrc.SetNull();
    BOOL32 bStopOldVidSrcNow = FALSE;   // �Ƿ������л��㲥Դ
	BOOL32 bNewVmpBrd = FALSE;			// �Ƿ�����VMP�㲥

    if (ptNewVidBrdSrc != NULL)
    {
        *ptNewVidBrdSrc = GetLocalMtFromOtherMcuMt(*ptNewVidBrdSrc);
    }

	//��ֹͣ�㲥ԭ����ƵԴ
    if (m_tVidBrdSrc.IsNull())
    {
        bStopOldVidSrcNow = TRUE;
    }
    else
    {
		//�������䲢�ҿ�����˫��, �ָ��ϵĵ�һ·����Դ�ķ�������,û�������������FlowControl����
        //flowctrl modify - �Ƿ��б�Ҫ�ָ��ϵĵ�һ·����Դ�ķ������ʣ����е���VidSrcҪֹͣ�������ġ�
        // guzh [8/24/2007] �ش���������⣬�������б�Ҫflowctrl��ԭ����׼���ʵġ���Ϊ�㲥Դ���ܻ��ڻش�������¼��VMP�У�����Ҫ�Ȼָ�
        if (!m_tDoubleStreamSrc.IsNull() &&
            !m_tVidBrdSrc.IsNull() && TYPE_MT == m_tVidBrdSrc.GetType())
        {
            TLogicalChannel tLogicalChannel;
            if (TRUE == m_ptMtTable->GetMtLogicChnnl(m_tVidBrdSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
            {
                tLogicalChannel.SetFlowControl(m_ptMtTable->GetSndBandWidth( m_tVidBrdSrc.GetMtId()));
                cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
                SendMsgToMt(m_tVidBrdSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
            }
        }
        
        if (m_tConf.m_tStatus.IsPrsing())
        {
            //ֹͣ��һ·��Ƶ�㲥Դ���ش�
            if( ptNewVidBrdSrc == NULL)
            {
                StopPrs(PRSCHANMODE_FIRST, FALSE);
            }   
            
            //vmp.2 ˫�ٵ���ʽ���飬ֹͣvmp�ڶ�·������prs
            if (m_tVidBrdSrc == m_tVmpEqp)
            {
                if (0 != m_tConf.GetSecBitRate() && 
                    MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
                {
                    StopPrs(PRSCHANMODE_VMP2, TRUE);
                }
            }
        }        

		if (ptNewVidBrdSrc != NULL)
		{		
			//����ϳɿ�ʼ�㲥ʱֹͣý����������
            // ֹͣ��������Ƶ����, zgc, 2008-08-07
            if ( *ptNewVidBrdSrc == m_tVmpEqp && m_tConf.m_tStatus.IsHdVidAdapting() )
            {
                //StopHDVidAdapt();
            }

			if (*ptNewVidBrdSrc == m_tVmpEqp && m_tConf.m_tStatus.IsVidAdapting())
			{
                //modify bas 2
				StopBasAdapt(ADAPT_TYPE_VID);
			}

			//����ϳɿ�ʼ�㲥ʱֹͣ��������
			if (*ptNewVidBrdSrc == m_tVmpEqp && m_tConf.m_tStatus.IsBrAdapting())
			{
				StopBasAdapt(ADAPT_TYPE_BR);
			}
		}

        // zgc, 2008-05-23, ���¹㲥�����߼���ͻ�����ﲻ�ܲ��ţ�����ȵ�MP���Ƴ��㲥Դ
        //    �ɹ�֪ͨ�յ�����ܲ���
        /*
        // zbq [08/23/2007] ��ǰԴ�ڻش�ͨ������ܲ���
        if ( !m_tCascadeMMCU.IsNull() &&
             m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId())->m_tSpyMt == m_tVidBrdSrc )
        {
        }
        else
        {
            //���������
            bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
            g_cMpManager.RemoveSwitchBridge(m_tVidBrdSrc, bySrcChnnl, MODE_VIDEO);            
        }
        
        //  xsl [3/10/2006] ˫�ٻ�˫��ʽ����㲥ԴΪvmpʱ������ڶ�·������
        if ((0 != m_tConf.GetSecBitRate() || MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType())
            && m_tVidBrdSrc == m_tVmpEqp)
        {
            g_cMpManager.RemoveSwitchBridge(m_tVidBrdSrc, bySrcChnnl+1, MODE_VIDEO);           
        }
        */
        
        TLogicalChannel tLogicalChannel;

        //��ֻͣ��Щ�տ��㲥Դ���ն�
        TMt tMt = m_tVidBrdSrc;
        tOldSrc = tMt;
        m_tVidBrdSrc.SetNull();	
        if (NULL == ptNewVidBrdSrc)
        {
            RestoreAllSubMtJoinedConfWatchingSrcMt(tMt, MODE_VIDEO);

            //zbq[01/05/2009] ֹͣ��ǰ�����佻��
            if (IsHDConf(m_tConf) &&
                m_tConf.m_tStatus.IsHdVidAdapting())
            {
                StopAllHdBasSwitch();
            }

            //ˢ��״̬�ͱ�
            m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_VID, FALSE);
            RefreshRcvGrp();

            // xliang [2/6/2009] �ж��Ƿ�����VMP�㲥ֹͣ
			if( tOldSrc == m_tVmpEqp )
			{
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				if(byVmpSubType != VMP)
				{
					bNewVmpBrd = TRUE;
				}
			}
			if(bNewVmpBrd)
			{
				// xliang [5/6/2009] ͣPRS
				if (m_tConf.m_tStatus.IsPrsing())
				{
					StopPrs(PRSCHANMODE_VMPOUT1, TRUE);
					StopPrs(PRSCHANMODE_VMPOUT2, TRUE);
					StopPrs(PRSCHANMODE_VMPOUT3, TRUE);
					StopPrs(PRSCHANMODE_VMPOUT4, TRUE);
				}
				bySrcChnnl = 0;
				// 				g_cMpManager.StopSwitchToBrd(tOldSrc, bySrcChnnl);  // ����ֹͣ�㲥Դ����
				// xliang [2/10/2009] ��������
				g_cMpManager.RemoveSwitchBridge(tOldSrc, bySrcChnnl, MODE_VIDEO);
				g_cMpManager.RemoveSwitchBridge(tOldSrc, bySrcChnnl+1, MODE_VIDEO);
				g_cMpManager.RemoveSwitchBridge(tOldSrc, bySrcChnnl+2, MODE_VIDEO);
				g_cMpManager.RemoveSwitchBridge(tOldSrc, bySrcChnnl+3, MODE_VIDEO);
				CallLog("New Vmp Broadcast Stop: RemoveSwitchBridge!\n");
				
				// xliang [4/2/2009] �ָ���ϯѡ��VMP
// 				if (m_tConf.m_tStatus.IsVmpSeeByChairman())
// 				{
// 					TMt tDstMt = m_tConf.GetChairman();
// 					SwitchNewVmpToSingleMt(tDstMt);
// 				}

				// xliang [3/14/2009] tell Mt(s) which saw VMP before to watch themselves at this moment 
				// except for chairman's watching VMP 
				TMt tLoopMt;
				TMt tCurSrc;
				for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
                {
                    if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
                    {
						m_ptMtTable->GetMtSrc( byLoop, &tCurSrc, MODE_VIDEO ); 
						if( tCurSrc == tOldSrc )	//���ն˿�����֮ǰ�Ĺ㲥Դ(VMP)
						{
							tLoopMt = m_ptMtTable->GetMt( byLoop );
							if( tLoopMt == m_tConf.GetChairman() 
								&& m_tConf.m_tStatus.IsVmpSeeByChairman() )
							{
								//SwitchNewVmpToSingleMt(tLoopMt);	//֮ǰ�����������ﲻ�ö��ⲹ
							}
							else
							{
								NotifyMtReceive( tLoopMt, byLoop );	//watch self
							}
						}
						
                    }
                }
				
			}
			else
			{
				bySrcChnnl = (tOldSrc == m_tPlayEqp ? m_byPlayChnnl : 0);
				// ֹͣ�㲥Դ����
				g_cMpManager.StopSwitchToBrd(tOldSrc, bySrcChnnl);
			}

            // ��������Ƿ�ɢ�鲥����ֹͣ
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
				//FIXME: ���Ƿ�ɢ�����Ƕ���
#ifdef _SATELITE_
				g_cMpManager.StopSatConfCast(tMt, CAST_FST, MODE_VIDEO, bySrcChnnl);
				g_cMpManager.StopSatConfCast(tMt, CAST_SEC, MODE_VIDEO, bySrcChnnl);
				
				for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
				{
					if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
						byMtId != m_tVidBrdSrc.GetMtId())
					{
						TMt tMtNull;
						tMtNull.SetNull();
						m_ptMtTable->SetMtSrc(byMtId, &tMtNull, MODE_VIDEO);
					}
				}
#else
				g_cMpManager.StopDistrConfCast(tMt, MODE_VIDEO, bySrcChnnl);
#endif
                TMt tLoopMt;
                // // ���� [4/25/2006] ֪ͨ�ն˿��Լ�
                for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
                {
                    if( m_tConfAllMtInfo.MtJoinedConf( byLoop ))
                    {
                        tLoopMt = m_ptMtTable->GetMt( byLoop );
                        NotifyMtReceive( tLoopMt, byLoop );
                    }
                }
            }            
        }
	
		//����ϳ�ֹͣ�㲥ʱ����ý����������
        if (TYPE_MCUPERI == tMt.GetType() && tMt == m_tVmpEqp &&
            m_tConf.GetConfAttrb().IsUseAdapter())
        {           
            // ���ָ����������ͨ���䣬zgc, 2008-08-07
            if ( IsHDConf(m_tConf) )
            {
                TMt tSrcTemp;
                tSrcTemp.SetNull();
                if (!m_tConf.m_tStatus.IsHdVidAdapting() && 
                    m_cMtRcvGrp.IsNeedAdp())
                {
                    StartHDMVBrdAdapt();
                }
            }
            else 
            {
                if (!m_tConf.m_tStatus.IsVidAdapting() &&
                    IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
                {
                    StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
                else if( !m_tConf.m_tStatus.IsVidAdapting() && IsNeedCifAdp() )
                {
                    IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate);
                    StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
                
                if (!m_tConf.m_tStatus.IsBrAdapting() &&
                    IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
                {
                    StartAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
            }           
        }

        //ֹͣ��¼�������
        if (!m_tConf.m_tStatus.IsNoRecording())
        {
            StopSwitchToPeriEqp(m_tRecEqp.GetEqpId(), m_byRecChnnl, FALSE, MODE_VIDEO);
        }

        //ֹͣ���������������� - modify bas 2
        //if (m_tConf.m_tStatus.IsVidAdapting())
        if (EQP_CHANNO_INVALID != m_byVidBasChnnl)
        {
            //zbq[08/07/2007] �л�Դ����ɾ��Դ����ͣBAS�Ľ��������½���������MPҵ���DSҵ����˵�
            if ( NULL == ptNewVidBrdSrc )
            {
                StopSwitchToPeriEqp(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, FALSE, MODE_VIDEO);
            }
        }

        //if (m_tConf.m_tStatus.IsBrAdapting())
        if (EQP_CHANNO_INVALID != m_byBrBasChnnl)
        {
            //zbq[08/07/2007] �л�Դ����ɾ��Դ����ͣBAS�Ľ��������½���������MPҵ���DSҵ����˵�
            if ( NULL == ptNewVidBrdSrc )
            {
                StopSwitchToPeriEqp(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, FALSE, MODE_VIDEO);
            }
        }

#ifdef _SATELITE_
		RefreshConfState();
		
#endif
	}

    //����ʼ�㲥����ƵԴ
    if (NULL != ptNewVidBrdSrc)
    {
        m_tVidBrdSrc = *ptNewVidBrdSrc;

        RefreshRcvGrp();
		
		// xliang [2/5/2009] ��VMP�㲥���������߼�
		if( m_tVidBrdSrc == m_tVmpEqp )
		{
			TPeriEqpStatus tPeriEqpStatus; 
			g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
			u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
			if(byVmpSubType != VMP)
			{
				bNewVmpBrd = TRUE;
			}
		}
		if(bNewVmpBrd)	
		{
			//��ѡ����Ŀǰ������������ն˶����յ�VMP�㲥(��MT��������Ⱥ����յ�VMPĳһ·������)
			TMtStatus tMtStatus;
			u8 bySelMode = MODE_NONE;
			TMt tSelMt;
			u8 byLoop;
			for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop ++)
			{
				if(m_tConfAllMtInfo.MtJoinedConf( byLoop ))
				{
					tSelMt = m_ptMtTable->GetMt(byLoop);
					m_ptMtTable->GetMtStatus(byLoop, &tMtStatus);

					if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
						&& ( m_tRollCaller == tSelMt || m_tRollCallee == tSelMt )
						)
					{
						TMt tSelectedMt = tMtStatus.GetSelectMt( MODE_VIDEO ); //��ȡѡ��Դ
						if(!tSelectedMt.IsNull())	//�ж�ѡ��Դ��ʵ���ڣ�����֮ǰѡ��ʧ��
						{
							continue;// xliang [4/24/2009] �����˱������˻���ѡ����VMP����������������
						}
					}

					bySelMode = tMtStatus.GetSelByMcsDragMode();
					if ( MODE_VIDEO == bySelMode || MODE_BOTH == bySelMode )
					{
						StopSelectSrc( tSelMt, MODE_VIDEO, FALSE );
					}
				}
			}
			
			//PRS
			if (m_tConf.GetConfAttrb().IsResendLosePack())
			{
				//���ݻ�������ȷ�������м�·PRS����ChangePrsSrc�����й��ˡ�����ͳһ��ֵ
				ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_VMPOUT1);
				ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_VMPOUT2);
				ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_VMPOUT3);
				ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_VMPOUT4);
			}

			//����������(4��ͨ��)
			bySrcChnnl = 0;
			g_cMpManager.SetSwitchBridge(*ptNewVidBrdSrc, bySrcChnnl  , MODE_VIDEO); 
			g_cMpManager.SetSwitchBridge(*ptNewVidBrdSrc, bySrcChnnl+1, MODE_VIDEO); 
			g_cMpManager.SetSwitchBridge(*ptNewVidBrdSrc, bySrcChnnl+2, MODE_VIDEO); 
			g_cMpManager.SetSwitchBridge(*ptNewVidBrdSrc, bySrcChnnl+3, MODE_VIDEO);
			//Ⱥ�齻��
			
			//����Ⱥ�����(������h263,mpeg4)
			u8 abyRcvMtFormat[4] = { VIDEO_FORMAT_HD1080,
									VIDEO_FORMAT_HD720,
									VIDEO_FORMAT_4CIF,
									VIDEO_FORMAT_CIF
									};
			u8  byRcvIdx = 0;
			u8  byRes = 0;
			u8	byMVType;
			u8  byNum = 0;
			u8	abyMtId[MAXNUM_CONF_MT] = { 0 };
			for(byRcvIdx; byRcvIdx < 4; byRcvIdx ++ )
			{

				byMVType = MEDIA_TYPE_H264; //Ŀǰֻ��H264
				byRes = abyRcvMtFormat[byRcvIdx];
				byNum = 0;	//ÿ�ζ�����0
				m_cMtRcvGrp.GetMVMtList(byMVType, byRes, byNum, abyMtId, TRUE);//ǿ��ȡ����MT�б�������������
				if(byNum > 0)
				{
					for(u8 byMtIdx = 0; byMtIdx < byNum; byMtIdx ++)
					{
						
						if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
							&& (m_tRollCaller.GetMtId() == abyMtId[byMtIdx] 
							|| m_tRollCallee.GetMtId() == abyMtId[byMtIdx])
							)
						{
							m_ptMtTable->GetMtStatus(abyMtId[byMtIdx], &tMtStatus);
							TMt tSelectedMt = tMtStatus.GetSelectMt( MODE_VIDEO ); 
							if(!tSelectedMt.IsNull())	//�ж�ѡ��Դ��ʵ���ڣ�����֮ǰѡ��ʧ��
							{
								continue;// xliang [4/24/2009] �����˱������˻���ѡ����VMP����������������
							}
						}
						bySrcChnnl = GetVmpOutChnnlByRes(byRes, m_tVmpEqp.GetEqpId(), byMVType);
						ConfLog(FALSE, "VMP(srcChnnl:%u)->MT.%u\n", bySrcChnnl, abyMtId[byMtIdx]);
						if(bySrcChnnl < MAXNUM_MPU_OUTCHNNL)	
						{

#ifndef _SATELITE_
							StartSwitchToSubMt(*ptNewVidBrdSrc, bySrcChnnl, abyMtId[byMtIdx], MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode ��Ĭ��ֵ����SWITCH_MODE_SELECT
#else
							//ͬ����Ƶ�ĸ����鲥�������������
							if (byRes == m_tConf.GetMainVideoFormat())
							{
								g_cMpManager.StartSatConfCast(*ptNewVidBrdSrc, CAST_FST, MODE_VIDEO, bySrcChnnl);						
							}
							m_ptMtTable->SetMtSrc(abyMtId[byMtIdx], ptNewVidBrdSrc, MODE_VIDEO);
#endif
							CallLog("Send New Vmp Chnnl.%u(%s) to Mt.%u\n",bySrcChnnl,GetResStr(byRes),abyMtId[byMtIdx]);
						}
					}
				}
			}
			
			//mp4 
			byMVType = MEDIA_TYPE_MP4;
			byRes	= VIDEO_FORMAT_CIF; //Ŀǰ����mpeg4��h263ȡ����Ⱥ�飬Res�����ο�
			byNum = 0;	//ÿ�ζ�����0
			m_cMtRcvGrp.GetMVMtList(byMVType, byRes, byNum, abyMtId, TRUE);//ǿ��ȡ����MT�б�������������
			
			if(byNum > 0)
			{
				for(u8 byMtIdx = 0; byMtIdx < byNum; byMtIdx ++)
				{
					
					if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
						&& (m_tRollCaller.GetMtId() == abyMtId[byMtIdx] 
						|| m_tRollCallee.GetMtId() == abyMtId[byMtIdx])
						)
					{
						m_ptMtTable->GetMtStatus(abyMtId[byMtIdx], &tMtStatus);
						TMt tSelectedMt = tMtStatus.GetSelectMt( MODE_VIDEO ); 
						if(!tSelectedMt.IsNull())	//�ж�ѡ��Դ��ʵ���ڣ�����֮ǰѡ��ʧ��
						{
							continue;// xliang [4/24/2009] �����˱������˻���ѡ����VMP����������������
						}
					}
					bySrcChnnl = GetVmpOutChnnlByRes(byRes, m_tVmpEqp.GetEqpId(), byMVType);
					ConfLog(FALSE, "VMP->MT: bySrcChnnl is %u\n", bySrcChnnl);
					if(bySrcChnnl < MAXNUM_MPU_OUTCHNNL)	
					{
						StartSwitchToSubMt(*ptNewVidBrdSrc, bySrcChnnl, abyMtId[byMtIdx], MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode ��Ĭ��ֵ����SWITCH_MODE_SELECT
// #ifdef _SATELITE_
// 						g_cMpManager.StartSatConfCast(*ptNewVidBrdSrc, FALSE, MODE_VIDEO, bySrcChnnl);
// #endif
						CallLog("Send New Vmp Chnnl.%u(%s) to Mt.%u\n",bySrcChnnl,GetResStr(byRes),abyMtId[byMtIdx]);
					}
				}
			}

			//h263
			byMVType = MEDIA_TYPE_H263;
			byRes	= VIDEO_FORMAT_CIF;
			byNum = 0;	//ÿ�ζ�����0
			m_cMtRcvGrp.GetMVMtList(byMVType, byRes, byNum, abyMtId, TRUE);//ǿ��ȡ����MT�б�������������
			
			if(byNum > 0)
			{

				for(u8 byMtIdx = 0; byMtIdx < byNum; byMtIdx ++)
				{
					
					if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
						&& (m_tRollCaller.GetMtId() == abyMtId[byMtIdx] 
						|| m_tRollCallee.GetMtId() == abyMtId[byMtIdx])
						)
					{
						m_ptMtTable->GetMtStatus(abyMtId[byMtIdx], &tMtStatus);
						TMt tSelectedMt = tMtStatus.GetSelectMt( MODE_VIDEO ); 
						if(!tSelectedMt.IsNull())	//�ж�ѡ��Դ��ʵ���ڣ�����֮ǰѡ��ʧ��
						{
							continue;// xliang [4/24/2009] �����˱������˻���ѡ����VMP����������������
						}
					}
					bySrcChnnl = GetVmpOutChnnlByRes(byRes, m_tVmpEqp.GetEqpId(), byMVType);
					ConfLog(FALSE, "VMP->MT: bySrcChnnl is %u\n", bySrcChnnl);
					if(bySrcChnnl < MAXNUM_MPU_OUTCHNNL)	
					{
#ifndef _SATELITE_
						StartSwitchToSubMt(*ptNewVidBrdSrc, bySrcChnnl, abyMtId[byMtIdx], MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode ��Ĭ��ֵ����SWITCH_MODE_SELECT
#else
						if (IsHDConf(m_tConf))
						{
							g_cMpManager.StartSatConfCast(*ptNewVidBrdSrc, CAST_SEC, MODE_VIDEO, bySrcChnnl);
						}
						else
						{
							g_cMpManager.StartSatConfCast(*ptNewVidBrdSrc, CAST_FST, MODE_VIDEO, bySrcChnnl);
						}
						m_ptMtTable->SetMtSrc(abyMtId[byMtIdx], ptNewVidBrdSrc, MODE_VIDEO);
#endif
						CallLog("Send New Vmp Chnnl.%u(%s) to Mt.%u\n",bySrcChnnl,GetResStr(byRes),abyMtId[byMtIdx]);
					}
				}
			}

		}
		else //����vmp����������/MT
		{

			//�������䲢�ҿ�����˫��, ǿ�ƽ����µĵ�һ·����Դ�ķ�������,û�������������FlowControl����
			if (!m_tDoubleStreamSrc.IsNull() &&
				!m_tVidBrdSrc.IsNull() && TYPE_MT == m_tVidBrdSrc.GetType())
			{
				u16 wDialBitrate = 0;
				TLogicalChannel tLogicalChannel;
				if (TRUE == m_ptMtTable->GetMtLogicChnnl(m_tVidBrdSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
				{
					wDialBitrate = m_ptMtTable->GetSndBandWidth(m_tVidBrdSrc.GetMtId());
					tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(wDialBitrate));
					cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
					SendMsgToMt(m_tVidBrdSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
				} 
			 }
 

			//�ı䶪���ش�Դ
			if (m_tConf.GetConfAttrb().IsResendLosePack())
			{
				ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_FIRST);
            
				//���Ѿ���������ı�prsԴ����û������������ack�иı�prsԴ
				if(m_tConf.m_tStatus.IsVidAdapting())
				{
					ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_VIDBAS, TRUE);
				}
				if(m_tConf.m_tStatus.IsBrAdapting())
				{
					ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_BRBAS, TRUE);
				}
            
				// �������䶪���ش�, zgc, 2008-08-13            
				if( m_tConf.m_tStatus.IsHdVidAdapting() )
				{
					ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_HDBAS_VID, TRUE);
				}
            
				// FIXME: ��������˫�������ش�δ���, zgc, 2008-08-13
            
				//vmp.2, ����vmp�ڶ�·��prs
				if (0 != m_tConf.GetSecBitRate() && 
					MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType() &&                
					m_tVidBrdSrc == m_tVmpEqp)
				{
					ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_VMP2);
				}
			}

			//����������
			bySrcChnnl = (*ptNewVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
			g_cMpManager.SetSwitchBridge(*ptNewVidBrdSrc, bySrcChnnl, MODE_VIDEO);
        
			// xsl [1/21/2006] ˫�ٻ�˫��ʽ����㲥ԴΪvmpʱ����ڶ�·������
			if (m_tVidBrdSrc == m_tVmpEqp &&
				((0 != m_tConf.GetSecBitRate() || MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType())))
			{
				g_cMpManager.SetSwitchBridge(*ptNewVidBrdSrc, bySrcChnnl+1, MODE_VIDEO);           
			}

			//��ʼ��¼�������
			if (!m_tConf.m_tStatus.IsNoRecording())
			{
				AdjustRecordSrcStream( MODE_VIDEO );
			}

            //zbq[04/15/2009]
            if (IsDelayVidBrdVCU())
            {
                //Vmp��ѯ����״̬��Ҫץ��Tandberg�Ĺؼ�֡���ӳٵ�VMP�������÷�youareseeing��fastupdate
            }
            else
            {
                //֪ͨ�ն˿�ʼ����
                if (ptNewVidBrdSrc->GetType() == TYPE_MT)
                {			
                    NotifyMtSend(ptNewVidBrdSrc->GetMtId(), MODE_VIDEO);
			    }
            }

            //zbq[07/15/2009] ��ֹ�������ʺ��� ����
            RefreshRcvGrp();

			//��ʼý����������
			if ( IsHDConf(m_tConf) )
			{
				if (m_tConf.GetConfAttrb().IsUseAdapter() && 
					m_cMtRcvGrp.IsNeedAdp())
				{
					StartHDMVBrdAdapt();

                    //zbq[07/30/2009] ¼������䣬������������ؼ�֡3�������VidSrc�л����ҽ��
                    //                VidSrc���ã���ʼ¼���IFrm������˳�ͻ��������mau/mpu����
                    TEqp tBas;
                    u8 byBasChn = 0;
                    if (!m_tConf.m_tStatus.IsNoRecording() &&
                        IsRecordSrcBas(MODE_VIDEO, tBas, byBasChn))
                    {
                        NotifyEqpFastUpdate(tBas, byBasChn, TRUE);
                    }
				}
				else
				{
					StopHDMVBrdAdapt();
				}
			}
			else
			{
				if (ptNewVidBrdSrc->GetType() == TYPE_MT &&
					m_tConf.GetConfAttrb().IsUseAdapter())
				{
					if (!m_tConf.m_tStatus.IsVidAdapting() &&
						IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, ptNewVidBrdSrc))
					{
						StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
					}
					else if( !m_tConf.m_tStatus.IsVidAdapting() && IsNeedCifAdp() )
					{
						IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, ptNewVidBrdSrc); // �õ�Ŀ���ʽ�ͷֱ���
						StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
					}

					if (!m_tConf.m_tStatus.IsBrAdapting() &&
						IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, ptNewVidBrdSrc))
					{
						StartAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
					}
				}
			}

			//�鲥�����鲥���ݣ������鲥ʱ���������������鲥��ַ
			if (m_tConf.GetConfAttrb().IsMulticastMode() && !m_tConf.GetConfAttrb().IsMulcastLowStream())
			{
				g_cMpManager.StartMulticast(m_tVidBrdSrc, bySrcChnnl, MODE_VIDEO);
				m_ptMtTable->SetMtMulticasting(m_tVidBrdSrc.GetMtId());
			}
                       
			// ����@2006.04.12 ��������Ƿ�ɢ�鲥
			if (m_tConf.GetConfAttrb().IsSatDCastMode())
			{

				//�Ժ�������ǻ���Ķ�������
			#ifdef _SATELITE_
				//�˴�ֻ�����һ�鲥���ڶ��鲥��bas/vmp�������
				g_cMpManager.StartSatConfCast(m_tVidBrdSrc, CAST_FST, MODE_VIDEO, bySrcChnnl);

				for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
				{
					if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
						byMtId != m_tVidBrdSrc.GetMtId())
					{
						m_ptMtTable->SetMtSrc(byMtId, &m_tVidBrdSrc, MODE_VIDEO);
					}
				}
			#else
				g_cMpManager.StartDistrConfCast(m_tVidBrdSrc, MODE_VIDEO, bySrcChnnl);
			#endif
				TMtStatus tMtStatus;
				for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
				{
					if ( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
					{                    
						// xsl [7/21/2006]����ѡ���������������������鲥��ַ����
						if (m_ptMtTable->GetMtStatus(byLoop, &tMtStatus))
						{
							TMt tTmpMt = tMtStatus.GetSelectMt(MODE_VIDEO);
							if (!tTmpMt.IsNull())
							{
								StopSwitchToSubMt(byLoop, MODE_VIDEO, SWITCH_MODE_SELECT);                            
								ChangeSatDConfMtRcvAddr(byLoop, LOGCHL_VIDEO);
							}
						}

						// ֪ͨ�ն˽���Դ
						NotifyMtReceive( m_tVidBrdSrc, byLoop );
					}
				}
			}
			else
			{
				// ��ʼ����ն˽���
				// guzh [3/21/2007] �����½ӿ�
				StartSwitchToAllSubMtJoinedConf(m_tVidBrdSrc, bySrcChnnl, tOldSrc, bStopOldVidSrcNow);

				//��ʼ���������������� -- modify bas 2
				// ���Ӹ��������ж�, zgc, 2008-08-06
				u8 byAdaptChnNum = 0;
				if ( m_tConf.m_tStatus.IsHdVidAdapting() )
				{
					/*
					if ( IsNeedHDVidAdapt( tSrcSimCapSet, m_tVidBrdSrc ) )
					{
						if ( ChangeHDVidAdapt( tSrcSimCapSet, m_tVidBrdSrc ) )
						{
							byAdaptChnNum = m_cConfBasChnMgr.GetVidChnUseNum();
							THDBasVidChnStatus *ptVidChn = NULL;
							ptVidChn = m_cConfBasChnMgr.GetHDBasVidChnStatus( CConfBasChnMgr::emHD_FstVidChn );
							if ( NULL != ptVidChn )
							{
								StartSwitchToPeriEqp( m_tVidBrdSrc, bySrcChnnl, ptVidChn->GetEqpId(),
													 ptVidChn->GetChnIdx(), MODE_VIDEO);
							}
							if ( byAdaptChnNum == 2 )
							{
								ptVidChn = m_cConfBasChnMgr.GetHDBasVidChnStatus( CConfBasChnMgr::emHD_SecVidChn );
								if ( NULL != ptVidChn )
								{
									StartSwitchToPeriEqp( m_tVidBrdSrc, bySrcChnnl, ptVidChn->GetEqpId(),
														 ptVidChn->GetChnIdx(), MODE_VIDEO);
								}
							}
						}
						else
						{
							ConfLog( FALSE, "[ChangeVidBrdSrc] ChangeHDVidAdapt failed!\n" );
						}
					}
					*/
				}
				else // FIXME: ��������ͨ����͸���������ӵ�����������, zgc, 2008-08-08
				{
					if (m_tConf.m_tStatus.IsVidAdapting())
					{                
						if (IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, ptNewVidBrdSrc) 
							|| IsNeedCifAdp() )
						{
							StartSwitchToPeriEqp(*ptNewVidBrdSrc, bySrcChnnl, m_tVidBasEqp.GetEqpId(),
												 m_byVidBasChnnl, MODE_VIDEO);

							ChangeAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
						}
					}

					if (m_tConf.m_tStatus.IsBrAdapting())
					{                
						if (IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, ptNewVidBrdSrc) )
						{
							StartSwitchToPeriEqp(*ptNewVidBrdSrc, bySrcChnnl, m_tBrBasEqp.GetEqpId(),
												  m_byBrBasChnnl, MODE_VIDEO);

							ChangeAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
						}
					}
				}
			}
		}
		
#ifdef _SATELITE_
		RefreshConfState();
		
#endif
    }

	return;
}

/*====================================================================
    ������      ��ChangeAudBrdSrc
    ����        ���ı���Ƶ�㲥Դ������������������
	              ֪ͨMC��MT��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt * ptNewAudBrdSrc, �µ���Ƶ�㲥Դ��NULL��ʾ��������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/25    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ChangeAudBrdSrc( TMt * ptNewAudBrdSrc )
{
    u8	bySrcChnnl;    
    CServMsg cServMsg;
	TSimCapSet tSrcSimCapSet;   //modify bas 2
	TSimCapSet tDstSimCapSet;   //modify bas 2

    //ֹͣ�㲥ԭ����ƵԴ
    if( !m_tAudBrdSrc.IsNull() )
    {
        //ֹͣ��һ·��Ƶ�㲥Դ���ش�
        if( ptNewAudBrdSrc == NULL && m_tConf.m_tStatus.IsPrsing() )
        {
            StopPrs(PRSCHANMODE_AUDIO, FALSE);
        }        

        // zbq [08/23/2007] ��ǰԴ�ڻش�ͨ������ܲ���
        if ( !m_tCascadeMMCU.IsNull() &&
            m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId())->m_tSpyMt == m_tAudBrdSrc )
        {
        }
        else
        {
            //���������
            bySrcChnnl = (m_tAudBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
            g_cMpManager.RemoveSwitchBridge( m_tAudBrdSrc, bySrcChnnl, MODE_AUDIO );
        }
        
        TMt tMt = m_tAudBrdSrc;
        m_tAudBrdSrc.SetNull();
        //ֻͣ��Щ�տ��㲥Դ���ն�
        if( ptNewAudBrdSrc == NULL )
        {
            RestoreAllSubMtJoinedConfWatchingSrcMt( tMt, MODE_AUDIO );
            // ��������Ƿ�ɢ�鲥����ֹͣ
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
#ifdef _SATELITE_
				g_cMpManager.StopSatConfCast(tMt, CAST_FST, MODE_AUDIO, bySrcChnnl);
				g_cMpManager.StopSatConfCast(tMt, CAST_SEC, MODE_AUDIO, bySrcChnnl);

				for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
				{
					if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
						byMtId != m_tVidBrdSrc.GetMtId())
					{
						TMt tMtNull;
						tMtNull.SetNull();
						m_ptMtTable->SetMtSrc(byMtId, &tMtNull, MODE_AUDIO);
					}
				}
#else
				g_cMpManager.StopDistrConfCast(tMt, MODE_AUDIO, bySrcChnnl);
#endif
            }
        }

        //ֹͣ��¼�������
        if( !m_tConf.m_tStatus.IsNoRecording() )
        {
            StopSwitchToPeriEqp( m_tRecEqp.GetEqpId(), m_byRecChnnl, FALSE, MODE_AUDIO );
        }
		
		//ֹͣ���������������� -- modify bas 2
        if (EQP_CHANNO_INVALID != m_byAudBasChnnl)
        {
            StopSwitchToPeriEqp(m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl, FALSE, MODE_AUDIO);
        }
		
#ifdef _SATELITE_
		RefreshConfState();
		
#endif
    }

    //��ʼ�㲥����ƵԴ
    if (ptNewAudBrdSrc != NULL)
    {				
        m_tAudBrdSrc = *ptNewAudBrdSrc;

		bySrcChnnl = (*ptNewAudBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
        if (m_tConf.m_tStatus.IsMixing() && *ptNewAudBrdSrc == m_tMixEqp)
        {
            bySrcChnnl = m_byMixGrpId;
        }
        g_cMpManager.SetSwitchBridge(*ptNewAudBrdSrc, bySrcChnnl, MODE_AUDIO);
        
        // xsl [8/22/2006] ֪ͨ�ն˿�ʼ����
        if (ptNewAudBrdSrc->GetType() == TYPE_MT)
        {			
            NotifyMtSend(ptNewAudBrdSrc->GetMtId(), MODE_AUDIO);
        }

        //�ı䶪���ش�Դ
        if (m_tConf.GetConfAttrb().IsResendLosePack())
        {
            ChangePrsSrc(m_tAudBrdSrc, PRSCHANMODE_AUDIO);
            
            //���Ѿ���������ı�prsԴ����û������������ack�иı�prsԴ
            if(m_tConf.m_tStatus.IsAudAdapting())
            {
                ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_AUDBAS, TRUE);
            }
 
        }

        //��ʼý���������� - modify bas 2
        if (ptNewAudBrdSrc->GetType() == TYPE_MT)
        {
            if (m_tConf.GetConfAttrb().IsUseAdapter())
            {
                if (!m_tConf.m_tStatus.IsAudAdapting() &&
                    IsNeedAudAdapt(tDstSimCapSet, tSrcSimCapSet, ptNewAudBrdSrc))
                {
                    StartAdapt(ADAPT_TYPE_AUD, 0, &tDstSimCapSet, &tSrcSimCapSet);
                }
            }
        }

        //��ʼ��¼�������
        if (!m_tConf.m_tStatus.IsNoRecording())
        {
            AdjustRecordSrcStream( MODE_AUDIO );
        }

        //�鲥�����鲥����
        if (m_tConf.GetConfAttrb().IsMulticastMode())
        {
            g_cMpManager.StartMulticast(*ptNewAudBrdSrc, bySrcChnnl, MODE_AUDIO);
            m_ptMtTable->SetMtMulticasting(ptNewAudBrdSrc->GetMtId());
        }
        // ����@2006.4.13��ʼ��Ƶ��ɢ�鲥��
        if (m_tConf.GetConfAttrb().IsSatDCastMode())
        {
#ifdef _SATELITE_
			g_cMpManager.StartSatConfCast(*ptNewAudBrdSrc, CAST_FST, MODE_AUDIO, bySrcChnnl);

			//FIXME: ����������һ�������������ԣ��������
			//g_cMpManager.StartSatConfCast(*ptNewAudBrdSrc, CAST_SEC, MODE_AUDIO, bySrcChnnl, TRUE);

			for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
			{
				if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
					byMtId != m_tVidBrdSrc.GetMtId())
				{
					m_ptMtTable->SetMtSrc(byMtId, ptNewAudBrdSrc, MODE_AUDIO);
				}
			}
#else
			g_cMpManager.StartDistrConfCast(*ptNewAudBrdSrc, MODE_AUDIO, bySrcChnnl);
#endif
            // xsl [7/21/2006]����ѡ���������������������鲥��ַ����
            TMtStatus tMtStatus;
            for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
            {
                if( m_tConfAllMtInfo.MtJoinedConf( byLoop ))
                {                                      
                    if (m_ptMtTable->GetMtStatus(byLoop, &tMtStatus))
                    {
                        TMt tTmpMt = tMtStatus.GetSelectMt(MODE_AUDIO);
                        if (!tTmpMt.IsNull())
                        {
                            StopSwitchToSubMt(byLoop, MODE_AUDIO, SWITCH_MODE_SELECT);                            
                            ChangeSatDConfMtRcvAddr(byLoop, LOGCHL_AUDIO);
                        }
                    }
                }
            }

            // xsl [9/21/2006] ֪ͨһ���ն�״̬, ����з����������ֹͣ�����ն�״̬֪ͨ����
            MtStatusChange();
        }
        else
        {
            //��ʼ����ն˽���
            if (!m_tConf.m_tStatus.IsMixing())
            {
                StartSwitchToAllSubMtJoinedConf(*ptNewAudBrdSrc, bySrcChnnl);
            }
        }

		//��ʼ���������������� -- modify bas 2
        if (m_tConf.m_tStatus.IsAudAdapting())
        {            
            if (IsNeedAudAdapt(tDstSimCapSet, tSrcSimCapSet, ptNewAudBrdSrc))
            {
                StartSwitchToPeriEqp(*ptNewAudBrdSrc, bySrcChnnl, m_tAudBasEqp.GetEqpId(),
                    m_byAudBasChnnl, MODE_AUDIO);
                
                ChangeAdapt(ADAPT_TYPE_AUD, 0, &tDstSimCapSet, &tSrcSimCapSet);
            }
        }
		
#ifdef _SATELITE_
		RefreshConfState();
		
#endif
	}

	return;
}

/*=============================================================================
�� �� ���� IsSpeakerCanBrdVid
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/3/5   4.0		�ܹ��                  ����
=============================================================================*/
BOOL32 CMcuVcInst::IsSpeakerCanBrdVid(void)
{
	//�ı���Ƶ����, ǿ�ƹ㲥ʱ���ı仭��ϳɱȷ��������ȼ��ߵĲ���
	if ( m_tConf.m_tStatus.IsBrdstVMP() )
	{
		return FALSE;
	}

	if ( !m_tConf.m_tStatus.IsNoPlaying() )
	{
		return FALSE;
	}

	if ( CONF_POLLMODE_VIDEO == m_tConf.m_tStatus.GetPollMode() )
	{
		return FALSE;
	}

	// KDV-BUG2004: �����˻򱻵����˱�ǿ��Ϊ�����ˣ�����MTC���ն�״̬δ�ı�
	// �������ڵ�����ɵķ����˱仯������VMP�ϳ�ģʽ�����������˹㲥��Ƶ��
	// Ŀ����Ϊ�˼��ٽ��㲥��������������
	// zgc, 2008-05-21, 					
	if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
	{
		return FALSE;
	}

	return TRUE;
}

/*=============================================================================
�� �� ���� IsSpeakerCanBrdAud
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/3/5   4.0		�ܹ��                  ����
=============================================================================*/
BOOL32 CMcuVcInst::IsSpeakerCanBrdAud(void)
{
	if (VCS_CONF == m_tConf.GetConfSource() &&
		!m_tMixEqp.IsNull())
	{
		TPeriEqpStatus tPeriEqpStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
		u8  byState = tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState;
		if (TMixerGrpStatus::WAIT_START_SPECMIX == byState ||
			TMixerGrpStatus::WAIT_START_AUTOMIX == byState)
		{
			return FALSE;
		}
	}

	if ( !m_tConf.m_tStatus.IsNoMixing() 
		&& !m_tConf.m_tStatus.IsMustSeeSpeaker() )
	{
		return FALSE;
	}

	if ( !m_tConf.m_tStatus.IsNoPlaying() )
	{
		return FALSE;
	}

	// KDV-BUG2004: �����˻򱻵����˱�ǿ��Ϊ�����ˣ�����MTC���ն�״̬δ�ı�
	// zgc, 2008-05-21, �������ڵ�����ɵķ����˱仯�����������˹㲥��Ƶ
	// ��Ϊ�����������Ǳ�ȻҪ��������������
	if ( ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
	{
		return FALSE;
	}

	return TRUE;
}


/*=============================================================================
�� �� ���� ChangeSpeakerSrc
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u8 byMode
           emChangeSpeakerSrcReason emReason
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/12   4.0			�ܹ��                  ����
=============================================================================*/
void CMcuVcInst::ChangeSpeakerSrc( u8 byMode, emChangeSpeakerSrcReason emReason )
{
	if ( !HasJoinedSpeaker() || m_tConf.GetSpeaker().GetType() == TYPE_MCUPERI )
	{
		ConfLog( FALSE, "[ChangeSpeakerSrc] No speaker or speaker is perieqp.\n" );
		return;
	}

	if ( emReason == emReasonUnkown )
	{
		ConfLog( FALSE, "[ChangeSpeakerSrc] Change reason is unkown!\n" );
		return;
	}
	
	TMt tLocalSpeaker = GetLocalMtFromOtherMcuMt( m_tConf.GetSpeaker() );
	TMt tLocalChairman = GetLocalMtFromOtherMcuMt( m_tConf.GetChairman() ); 
	TMt tLocalLastSpeaker = GetLocalMtFromOtherMcuMt( m_tLastSpeaker ); 
	
	u8 bySelMode = m_tConf.GetConfAttrb().GetSpeakerSrcMode();
	u8 byTempMode = MODE_NONE;

	if ( byMode == MODE_BOTH )
	{
		ChangeSpeakerSrc( MODE_AUDIO, emReason );
		ChangeSpeakerSrc( MODE_VIDEO, emReason );
		return;
	}
	
	if ( byMode == MODE_VIDEO && (bySelMode == MODE_BOTH || bySelMode == MODE_VIDEO) )
	{
		byTempMode = MODE_VIDEO;
	}
	if ( byMode == MODE_AUDIO && bySelMode == MODE_BOTH )
	{
		byTempMode = MODE_AUDIO;
	}

	if ( byTempMode == MODE_NONE && emReason != emReasonChangeBrdSrc )
	{
		return;
	}
	
	u8 bySpeakerSrcType = m_tConf.GetConfAttrb().GetSpeakerSrc();

	if ( (bySpeakerSrcType == CONF_SPEAKERSRC_LAST && emReason == emReasonChangeChairman) ||
		(bySpeakerSrcType == CONF_SPEAKERSRC_SELF && emReason != emReasonChangeBrdSrc) ||
		( m_tConf.m_tStatus.IsMixing() && byMode == MODE_AUDIO && emReason != emReasonChangeBrdSrc ) )
	{
		//������ѡ����һ�η�����ģʽ�£���ϯ����change����Ҫ���е���;
		//�����˿��Լ��Ҳ��ǹ㲥Դ�仯����ģ�����Ҫ���е���;
		//�����ڻ���ʱ����Ƶ��������Ҫ�������㲥Դ�仯����;
		return;
	}

	BOOL32 bRet = TRUE;

	// ��÷����˵�ǰԴ�����
	TMtStatus tStatus;
	m_ptMtTable->GetMtStatus( tLocalSpeaker.GetMtId(), &tStatus );

	TMt tOldSrc;
	u8 byOldSrcChn = 0;
	u8 bySpeakerSrcSpecType = SPEAKER_SRC_NOTSEL;
	BOOL32 bIsSel = FALSE;

	m_ptMtTable->GetMtSrc( tLocalSpeaker.GetMtId(), &tOldSrc, byMode );
	byOldSrcChn = ( tOldSrc == m_tPlayEqp ) ? m_byPlayChnnl : 0;
	bySpeakerSrcSpecType = GetSpeakerSrcSpecType( byMode );
	if ( !tOldSrc.IsNull() && !(tOldSrc == tLocalLastSpeaker) && 
		(tStatus.GetSelByMcsDragMode() == MODE_BOTH ||
		 tStatus.GetSelByMcsDragMode() == byMode ) )
	{
		bIsSel = TRUE;
	}

	// ѡ��������־λδ�ã�˵��ѡ��ʱ���Ƿ����ˣ�������Ҫ������ȷ�ı�־
	if ( bIsSel && bySpeakerSrcSpecType == SPEAKER_SRC_NOTSEL )
	{
		SetSpeakerSrcSpecType( byMode, SPEAKER_SRC_MCSDRAGSEL );
		bySpeakerSrcSpecType = GetSpeakerSrcSpecType( byMode );
	}

	// �ɹ㲥Դ�仯����ĵ���
	BOOL32 bCancelBrdSrc = TRUE;
	if ( emReason == emReasonChangeBrdSrc && 
		(( byMode == MODE_AUDIO && !m_tAudBrdSrc.IsNull() ) ||
		 ( byMode == MODE_VIDEO && !m_tVidBrdSrc.IsNull() )) )
	{
		bCancelBrdSrc = FALSE;

		u8 byBrdSrcChn = 0;
		TMt tBrdSrc;

		if ( byMode == MODE_AUDIO)
		{
			tBrdSrc = m_tAudBrdSrc;
		}
		else
		{
			tBrdSrc = m_tVidBrdSrc;
		}
		// �㲥Դ���Ƿ����ˣ����ӹ㲥Դ�������˵Ĺ㲥����
		if ( !(tBrdSrc == tLocalSpeaker) )
		{			
			byBrdSrcChn = ( tBrdSrc == m_tPlayEqp ) ? m_byPlayChnnl : 0;

			if ( byMode == MODE_VIDEO )
			{
				bRet = StartSwitchFromBrd( tBrdSrc, byBrdSrcChn, 1, &tLocalSpeaker );
			}
			else
			{
				bRet = StartSwitchToSubMt( tBrdSrc, byBrdSrcChn, tLocalSpeaker.GetMtId(), MODE_AUDIO );
			}
			if ( bRet )
			{
				SetSpeakerSrcSpecType( byMode, SPEAKER_SRC_NOTSEL );
				return;
			}
			ConfLog( FALSE, "[ChangeSpeakerSrc] Speaker(Mt.%d) receive broarcastsrc(id.%d)(mode.%d) failed.\n", 
					tLocalSpeaker.GetMtId(), tBrdSrc.GetMtId(), byMode );
		}
	}
	
	// ������ѡ����ϯ��ģʽ
	if( bySpeakerSrcType == CONF_SPEAKERSRC_CHAIR
		&& HasJoinedChairman() 
		&& byTempMode != MODE_NONE
		&& !(tLocalSpeaker == tLocalChairman) )
	{
		// ���ԭ��������ѡ������Դ���ǵ�ǰ����ϯ�նˣ��򲻱�����ѡ��
		if ( !tOldSrc.IsNull() && bIsSel && tOldSrc == tLocalChairman )
		{
			return;
		}

		if( // ѡ������Ҫ�ָ�����ֹ���� �� �����˹㲥ͨ����������ָ�
			ChangeSelectSrc( m_tConf.GetChairman(), tLocalSpeaker, byTempMode ) )//����ϯ����ϯ
		{
			SetSpeakerSrcSpecType( byTempMode, SPEAKER_SRC_CONFSEL );
			return;				
		}
		ConfLog( FALSE, "[ChangeSpeakerSrc] Speaker(Mt.%d) select chairman(mt.%d)(mode.%d) failed.\n",
						tLocalSpeaker.GetMtId(), tLocalChairman.GetMtId(), byTempMode );
	}
	
	// ������ѡ����һ�η�����ģʽ
	if (bySpeakerSrcType == CONF_SPEAKERSRC_LAST
		&& byTempMode != MODE_NONE
		&& !m_tLastSpeaker.IsNull()
		&& !(tLocalSpeaker == tLocalLastSpeaker))
	{
		// ���ԭ��������ѡ������Դ������һ�η����ˣ��򲻱�����ѡ��
		if ( !tOldSrc.IsNull() && bIsSel && tOldSrc == tLocalLastSpeaker )
		{
			return;
		}

		if ( //ѡ������Ҫ�ָ�����ֹ���� �� �����˹㲥ͨ����������ָ�
			ChangeSelectSrc( m_tLastSpeaker, tLocalSpeaker, byTempMode ) )
		{
			SetSpeakerSrcSpecType( byTempMode, SPEAKER_SRC_CONFSEL );
			return;	
		}
		ConfLog( FALSE, "[ChangeSpeakerSrc] Speaker(Mt.%d) select lastspeaker(mt.%d)(mode.%d) failed.\n",
						tLocalSpeaker.GetMtId(), tLocalLastSpeaker.GetMtId(), byTempMode );
	}
	
	// ���︺�������˿��Լ��Լ����洦��ʧ��ʱ�ı���
	if ( !tOldSrc.IsNull() && bIsSel &&
		SPEAKER_SRC_MCSDRAGSEL == bySpeakerSrcSpecType )
	{
		// ����Ƿ����˿��Լ��Ҳ������ڹ㲥Դ�仯��������ȡ���㲥Դ������ĵ�����
		// ����֮ǰ���������޸ķ�����Դ�����Ĳ��������ﲻ��Ҫ����ѡ��
		if ( bySpeakerSrcType != CONF_SPEAKERSRC_SELF || 
			( emReason == emReasonChangeBrdSrc && !bCancelBrdSrc ))
		{
			bRet = ChangeSelectSrc( tOldSrc, tLocalSpeaker, byMode );
			if ( !bRet )
			{
				RestoreRcvMediaBrdSrc( tLocalSpeaker.GetMtId(), byMode );
				SetSpeakerSrcSpecType( byMode, SPEAKER_SRC_NOTSEL );
			}
		}
	}
	else
	{
		RestoreRcvMediaBrdSrc( tLocalSpeaker.GetMtId(), byMode );
		SetSpeakerSrcSpecType( byMode, SPEAKER_SRC_NOTSEL );
	}	
}

/*=============================================================================
�� �� ���� ChangeOldSpeakerSrc
��    �ܣ� �޸ı�ȡ�����Ϸ����˵�Դ��������������һ�η�����֮ǰ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMode
		   TMt tOldSpeaker
		   BOOL32 bIsHaveNewSpeaker 
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/15   4.0			�ܹ��                  ����
=============================================================================*/
void CMcuVcInst::ChangeOldSpeakerSrc( u8 byMode, TMt tOldSpeaker, BOOL32 bIsHaveNewSpeaker )
{
	if ( tOldSpeaker.IsNull() )
	{
		return;
	}
	
	TMt tLocalOldSpeaker = GetLocalMtFromOtherMcuMt( tOldSpeaker );
	
	BOOL32 bIsJoinedMt = FALSE;
	if( tLocalOldSpeaker.GetType() == TYPE_MT && m_tConfAllMtInfo.MtJoinedConf( tLocalOldSpeaker.GetMtId() ) )
	{
		bIsJoinedMt = TRUE;
	}

	if ( byMode == MODE_BOTH )
	{
		ChangeOldSpeakerSrc( MODE_AUDIO, tOldSpeaker, bIsHaveNewSpeaker );
		ChangeOldSpeakerSrc( MODE_VIDEO, tOldSpeaker, bIsHaveNewSpeaker );
		return;
	}
	
	// ֹͣ��ԭ�����˵Ľ���
    if ( bIsJoinedMt && !bIsHaveNewSpeaker )  // ȡ��������
    {
		TMt tSrc;
        m_ptMtTable->GetMtSrc( tLocalOldSpeaker.GetMtId(), &tSrc, byMode);

		TMt tLocalChairman = GetLocalMtFromOtherMcuMt(m_tConf.GetChairman());
		TMt tLocalLastSpeaker = GetLocalMtFromOtherMcuMt(m_tLastSpeaker);
		u8 bySpeakerSrcSpecType = GetSpeakerSrcSpecType( byMode );
		u8 bySpeakerSrc = m_tConf.GetConfAttrb().GetSpeakerSrc();
		u8 bySpeakerSrcMode = m_tConf.GetConfAttrb().GetSpeakerSrcMode();

		if ( !tSrc.IsNull() && bySpeakerSrcSpecType == SPEAKER_SRC_CONFSEL &&
			((tSrc == tLocalChairman && bySpeakerSrc == CONF_SPEAKERSRC_CHAIR) ||
			 (tSrc == tLocalLastSpeaker && bySpeakerSrc == CONF_SPEAKERSRC_LAST)) &&
			 ( byMode == MODE_VIDEO || (byMode == MODE_AUDIO && bySpeakerSrcMode == MODE_BOTH) ) )
		{
			if (m_ptMtTable->GetManuId( tLocalOldSpeaker.GetMtId() ) == MT_MANU_KDC)
			{
				StopSwitchToSubMt( tLocalOldSpeaker.GetMtId(), byMode, SWITCH_MODE_BROADCAST, TRUE);
			}
			else
			{
				//��kdc���̻ش�������Ƶ, ��ʡ����ʱ��֪ͨmt��������
				// ������Ƶ����, zgc, 2008-04-25
				if(g_cMcuVcApp.IsSavingBandwidth() && byMode == MODE_VIDEO )
				{
					NotifyOtherMtSend( tLocalOldSpeaker.GetMtId(), TRUE );
				}     
				
				StartSwitchToSubMt( tLocalOldSpeaker, 0, tLocalOldSpeaker.GetMtId(), byMode, SWITCH_MODE_BROADCAST, TRUE);
			}	
		}
    }
	
	// �ؽ�VMP����Ƶ��ѯ���Ϸ����˵���Ƶ����
	if ( bIsJoinedMt && byMode == MODE_VIDEO )
	{
		TMtStatus tStatus;
		m_ptMtTable->GetMtStatus( tLocalOldSpeaker.GetMtId(), &tStatus );
		BOOL32 bIsSelMode = ( tStatus.GetSelByMcsDragMode() == MODE_VIDEO || tStatus.GetSelByMcsDragMode() == MODE_BOTH );
		// ���Ӷ�ѡ����ʽ���ж�, zgc, 2008-04-25
		if ( !bIsSelMode || ( bIsSelMode && GetSpeakerSrcSpecType( byMode ) == SPEAKER_SRC_CONFSEL ) )
		{
			RestoreRcvMediaBrdSrc( tLocalOldSpeaker.GetMtId(), MODE_VIDEO );  
		}
	}

	// �ָ���־Ϊ��ѡ��
	SetSpeakerSrcSpecType( byMode, SPEAKER_SRC_NOTSEL );
}
/*=============================================================================
�� �� ���� GetSpeakerSrcSpecType
��    �ܣ� ���鷢���˵�Դ�Ƿ����ɻ����Զ�ָ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMode
�� �� ֵ�� u8
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/14   4.0		�ܹ��                  ����
=============================================================================*/
u8 CMcuVcInst::GetSpeakerSrcSpecType(u8 byMode)
{
	switch( byMode )
	{
	case MODE_VIDEO:
		return m_bySpeakerVidSrcSpecType;
		break;
	case MODE_AUDIO:
		return m_bySpeakerAudSrcSpecType;
		break;
	default:
		ConfLog( FALSE, "[GetSpeakerSrcSpecType] Mode(%d) is error,return NOTSEL!\n", byMode );
		break;
	}
	return SPEAKER_SRC_NOTSEL;
}

/*=============================================================================
�� �� ���� SetSpeakerSrcSpecType
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMode
           u8 bySpecType
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/14   4.0		�ܹ��                  ����
=============================================================================*/
void CMcuVcInst::SetSpeakerSrcSpecType(u8 byMode, u8 bySpecType )
{
	switch( byMode )
	{
	case MODE_VIDEO:
		m_bySpeakerVidSrcSpecType = bySpecType;
		break;
	case MODE_AUDIO:
		m_bySpeakerAudSrcSpecType = bySpecType;
		break;
	case MODE_BOTH:
		m_bySpeakerVidSrcSpecType = bySpecType;
		m_bySpeakerAudSrcSpecType = bySpecType;
		break;
	default:
		ConfLog( FALSE, "[SetSpeakerSrcSpecType] Mode(%d) is error,set failed!\n", byMode );
		break;
	}
	return;
}

/*=============================================================================
�� �� ���� AdjustOldSpeakerSwitch
��    �ܣ� ������ȡ�����Ϸ����˵Ľ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  TMt tOldSpeaker				: �Ϸ����ˣ�δ����GetLocal�ĵ���
			BOOL32 bIsHaveNewSpeaker	: �Ƿ����·�����
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/3/4   4.0		�ܹ��                  ����
=============================================================================*/
void CMcuVcInst::AdjustOldSpeakerSwitch( TMt tOldSpeaker, BOOL32 bIsHaveNewSpeaker )
{
	if ( tOldSpeaker.IsNull() )
	{
		return;
	}

	TMt tLocalOldSpeaker = GetLocalMtFromOtherMcuMt( tOldSpeaker );

	BOOL32 bIsJoinedMt = FALSE;
	if( tLocalOldSpeaker.GetType() == TYPE_MT && m_tConfAllMtInfo.MtJoinedConf( tLocalOldSpeaker.GetMtId() ) )
	{
		bIsJoinedMt = TRUE;
	}

	CServMsg cServMsg;
	cServMsg.SetMsgBody( ( u8 * )&tOldSpeaker, sizeof( tOldSpeaker ) );
	cServMsg.SetNoSrc();

	if( tOldSpeaker.GetType() == TYPE_MT )
	{
		if(tOldSpeaker.IsLocal())
		{
			SendMsgToMt( tOldSpeaker.GetMtId() , MCU_MT_CANCELSPEAKER_NOTIF,cServMsg );
		}
	}
	else if( tOldSpeaker.GetType() == TYPE_MCUPERI )
	{
		cServMsg.SetChnIndex( m_byPlayChnnl );	// xliang [8/19/2008] Chanel indexһ��Ҫ��

		SendMsgToEqp( tOldSpeaker.GetEqpId(), MCU_REC_STOPPLAY_REQ, cServMsg );
		m_tConf.m_tStatus.SetNoPlaying();
		ConfModeChange();
	}

	// �Ϸ����˵�����Դ�����߼�, zgc, 2008-04-15
	ChangeOldSpeakerSrc( MODE_BOTH, tOldSpeaker, bIsHaveNewSpeaker );

	/*
	TMt tSpeakerSrc;
	// ֹͣ��ԭ�����˵Ľ���
    if ( bIsJoinedMt && !bIsHaveNewSpeaker )  // ȡ��������
    {
        m_ptMtTable->GetMtSrc( tLocalOldSpeaker.GetMtId(), &tSpeakerSrc, MODE_VIDEO);
		
        if ((tSpeakerSrc == GetLocalMtFromOtherMcuMt(m_tConf.GetChairman()) && m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_CHAIR) || 
            ((tSpeakerSrc == GetLocalMtFromOtherMcuMt(m_tLastSpeaker)) && m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_LAST))
        {				
			if (m_ptMtTable->GetManuId( tLocalOldSpeaker.GetMtId() ) == MT_MANU_KDC)
			{
				StopSwitchToSubMt( tLocalOldSpeaker.GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
			}
			else
			{
				//��kdc���̻ش�������Ƶ, ��ʡ����ʱ��֪ͨmt��������
				if(g_cMcuVcApp.IsSavingBandwidth())
				{
					NotifyOtherMtSend( tLocalOldSpeaker.GetMtId(), TRUE );
				}     

				StartSwitchToSubMt( tLocalOldSpeaker, 0, tLocalOldSpeaker.GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
			}	
        }
					
		// ���Ӷ���Ƶ�Ĵ���, zgc, 2008-02-02
		m_ptMtTable->GetMtSrc( tLocalOldSpeaker.GetMtId(), &tSpeakerSrc, MODE_AUDIO );
		if ( ((tSpeakerSrc == GetLocalMtFromOtherMcuMt(m_tConf.GetChairman()) && m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_CHAIR) || 
            ((tSpeakerSrc == GetLocalMtFromOtherMcuMt(m_tLastSpeaker)) && m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_LAST))
			&& MODE_BOTH == m_tConf.GetConfAttrb().GetSpeakerSrcMode() )
        {
			if (m_ptMtTable->GetManuId( tLocalOldSpeaker.GetMtId()) == MT_MANU_KDC )
			{
				StopSwitchToSubMt( tLocalOldSpeaker.GetMtId(), MODE_AUDIO, SWITCH_MODE_BROADCAST, TRUE );
			}
			else
			{
				//��kdc���̻ش�������Ƶ, ��ʡ����ʱ��֪ͨmt��������
				if( g_cMcuVcApp.IsSavingBandwidth() )
				{
					NotifyOtherMtSend( tLocalOldSpeaker.GetMtId(), TRUE );
				}     
				
				StartSwitchToSubMt( tLocalOldSpeaker, 0, tLocalOldSpeaker.GetMtId(), MODE_AUDIO, SWITCH_MODE_BROADCAST, TRUE );
			}
		}
    }
	
	// �ؽ�VMP����Ƶ��ѯ���Ϸ����˵���Ƶ����, zgc, 2008-02-02
	if ( bIsJoinedMt )
	{
		// �·���������Դ�����߼��޸�, zgc, 2008-04-12
		TMtStatus tStatus;
		m_ptMtTable->GetMtStatus( tLocalOldSpeaker.GetMtId(), &tStatus );
		BOOL32 bIsSelMode = ( tStatus.GetSelByMcsDragMode() == MODE_VIDEO || tStatus.GetSelByMcsDragMode() == MODE_BOTH );
		if ( !bIsSelMode )
		{
			RestoreRcvMediaBrdSrc( tLocalOldSpeaker.GetMtId(), MODE_VIDEO );  
		}
	}
	*/
		
	//ȡ��ǿ�ƻ���
	if( m_tConf.m_tStatus.IsMixing() )
	{		
		// xsl [7/24/2006] ȡ������ʱ�����Ӷ��ƻ����б���ɾ����//�����Ҫ�����ڻ�����
		// ����VCS������Ҫ��ȡ���ķ����˴ӻ�������ȥ��
		if (!m_tConfInStatus.IsLastSpeakerInMixing() &&
			VCS_CONF == m_tConf.GetConfSource() &&
			!(tOldSpeaker == m_tConf.GetChairman()))
		{
			if (tOldSpeaker.IsLocal())
			{
				// VCS��ʹ��������Ϊ��Ҳ���Զ�ͣ����
				AddRemoveSpecMixMember(&tOldSpeaker, 1, FALSE, TRUE); 
			}
			 else
			{
				 CServMsg cMsg;
				 TMcu tMcu;
				 tMcu.SetMcu(tOldSpeaker.GetMcuId());
				 cMsg.SetMsgBody((u8*)&tMcu, sizeof(tMcu));
				 cMsg.CatMsgBody((u8*)&tOldSpeaker, sizeof(tOldSpeaker));
				 OnAddRemoveMixToSMcu(&cMsg, FALSE);
			}
		}    
		else
		{
			m_tConfInStatus.SetLastSpeakerInMixing(FALSE);
		}  
		
		// xsl [8/4/2006] ȡ��ǿ�ƻ���
		RemoveMixMember(&tLocalOldSpeaker, TRUE);
	}

	//TvWall
	TPeriEqpStatus tTWStatus;
    u8 byChnlIdx;
    u8 byEqpId;
    //TvWall
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
						ChangeTvWallSwitch(&tOldSpeaker, byEqpId, byChnlIdx, TW_MEMBERTYPE_SPEAKER, TW_STATE_STOP, FALSE);
					}
				}
			}
		}
    }
    
	//Hdu
	TPeriEqpStatus tHduStatus;
    u8 byHduChnlIdx;
    u8 byHduEqpId;
	for (byHduEqpId = HDUID_MIN; byHduEqpId <= HDUID_MAX; byHduEqpId++)
	{
		if (EQP_TYPE_HDU == g_cMcuVcApp.GetEqpType(byHduEqpId))
		{
			if (g_cMcuVcApp.GetPeriEqpStatus(byHduEqpId, &tHduStatus))
			{
				u8 byMemberType;
				u8 byMtConfIdx;
				for (byHduChnlIdx = 0; byHduChnlIdx < MAXNUM_HDU_CHANNEL; byHduChnlIdx++)
				{
					byMemberType = tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].byMemberType;
					byMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetConfIdx();
					if (TW_MEMBERTYPE_SPEAKER == byMemberType && m_byConfIdx == byMtConfIdx && bIsHaveNewSpeaker == FALSE)
					{
						ChangeHduSwitch(&tOldSpeaker, byHduEqpId, byHduChnlIdx, TW_MEMBERTYPE_SPEAKER, TW_STATE_STOP);
					}
				}
			}
		}
    }    

    // xliang [1/21/2009] ����������˴�
    m_tConf.SetNoSpeaker();
	if ( tOldSpeaker.GetType() == TYPE_MT )
	{
		m_tLastSpeaker = tOldSpeaker;
	}

	// xliang [12/12/2008] �ж��Ƿ���vmpͨ������ռ����µ�ȡ��������
	//�ǣ��򽵸�MT�ķֱ��ʡ�������ǰ����VMP�����
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
	

	if ( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE &&	//������vmp��
		m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tOldSpeaker))	//�Ϸ�����MT��vmp�ϳɳ�Ա��
	{	
		TVMPParam	tVMPParam   = m_tConf.m_tStatus.GetVmpParam();
		u8	byChlPos	= tVMPParam.GetChlOfMtInMember(tOldSpeaker);
		if(byVmpSubType != VMP)//�õ�VMP��HD������VMP
		{
			if( tVMPParam.GetVmpMember(byChlPos)->GetMemberType() == VMP_MEMBERTYPE_SPEAKER ) //��ͨ�������Ƿ����˸���ͨ��
			{
				//�����������ChangeVMPParam, ֮��ȫ����һ�ηֱ��ʵ�������������Ͳ����е���������
				CallLog("[adjustOldSpeakerSwitch] no need to change format for old Speaker Mt.%, \
					because the channel it occupied is��VMP_MEMBERTYPE_SPEAKER��.\n", tOldSpeaker.GetMtId());
			}
			else
			{
				// xliang [4/21/2009]  �˳�VMP����ǰ����ͨ���������ֱ��� 
				ChangeMtVideoFormat(tOldSpeaker, &tVMPParam );
			}
		}
		else
		{
			ChangeMtVideoFormat(tOldSpeaker, &tVMPParam);
		}
	}
	else if ( m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE && 
		m_tConf.m_tStatus.GetVmpTwParam().IsMtInMember(tOldSpeaker) )
    {
        TVMPParam tVmpTwParam = m_tConf.m_tStatus.GetVmpTwParam();
        ChangeMtVideoFormat(tOldSpeaker, &tVmpTwParam);
    }
	
	// xliang [12/17/2008] ����ĸı�vmp�ֱ��ʲ������ϵ�����
    // xsl [8/28/2006] �ı�vmp�ֱ��ʲ���
	//     if ( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE && 
	//          m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tOldSpeaker) )
	//     {
	//         TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
	// 		ChangeMtVideoFormat(tOldSpeaker.GetMtId(), &tVmpParam);
	//     }
	//     else if ( m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE && 
	//               m_tConf.m_tStatus.GetVmpTwParam().IsMtInMember(tOldSpeaker) )
	//     {
	//         TVMPParam tVmpTwParam = m_tConf.m_tStatus.GetVmpTwParam();
	//         ChangeMtVideoFormat(tOldSpeaker.GetMtId(), &tVmpTwParam);
	//     }

	return;
}

/*=============================================================================
�� �� ���� AdjustNewSpeakerSwitch
��    �ܣ� ������ָ���ķ����˵Ľ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  TMt tNewSpeaker		: ��ָ���ķ����ˣ�δ����GetLocal�ĵ���
			BOOL32 bAddToVmp	: �Ƿ�Ҫ���뻭��ϳ�
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/3/4    4.0			�ܹ��                  ����
=============================================================================*/
void CMcuVcInst::AdjustNewSpeakerSwitch( TMt tNewSpeaker, BOOL32 bAddToVmp )
{
	TMt tLocalNewSpeaker;
	tLocalNewSpeaker.SetNull();
	if ( !tNewSpeaker.IsNull() )
	{
		tLocalNewSpeaker = GetLocalMtFromOtherMcuMt( tNewSpeaker );
	}
	
	if ( !tNewSpeaker.IsNull() )
	{
		if(	tLocalNewSpeaker.GetType() == TYPE_MT && tNewSpeaker.IsLocal() )		//ֱ���ն�
		{
			CServMsg cServMsg;
			cServMsg.SetMsgBody( ( u8 * )&tLocalNewSpeaker, sizeof( TMt ) );
			cServMsg.SetNoSrc();
			SendMsgToMt( tLocalNewSpeaker.GetMtId(), MCU_MT_SPECSPEAKER_NOTIF,cServMsg );
		}		
		m_tConf.SetSpeaker( tNewSpeaker );
	}

	//�ı���Ƶ����, ǿ�ƹ㲥ʱ���ı仭��ϳɱȷ��������ȼ��ߵĲ���
	if ( IsSpeakerCanBrdVid()
		 /* || (m_tConf.m_tStatus.IsMustSeeSpeaker() &&
		 CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode())*/)
	{
		if ( !tNewSpeaker.IsNull() )
		{
			TMtStatus tMtStatus;
			if ( tLocalNewSpeaker.GetType() == TYPE_MT )
			{
				m_ptMtTable->GetMtStatus( tLocalNewSpeaker.GetMtId(), &tMtStatus );
			}
			if ( tMtStatus.IsSendVideo() || TYPE_MCUPERI == tLocalNewSpeaker.GetType() )
			{
				// xliang [7/9/2009] ���Ƿ����ˣ���ϯѡ��VMP����Ҫͣ��
				if(m_tConf.m_tStatus.GetVmpParam().IsVMPSeeByChairman() 
					&& !(m_tConf.GetChairman() == tLocalNewSpeaker)
					)
				{
					ConfLog(FALSE, "Chairman.%u stop watching VMP because other Mt's speaking!\n",
						m_tConf.GetChairman().GetMtId());
					m_tConf.m_tStatus.SetVmpSeebyChairman(FALSE);

					// ״̬ͬ��ˢ�µ�TPeriStatus��
					TPeriEqpStatus tVmpStatus;
					g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
					tVmpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
					g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );

					StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO);
				}
				ChangeVidBrdSrc( &tLocalNewSpeaker );
			}
			else
			{
				ChangeVidBrdSrc(NULL);
			}
		}
		else
		{
			ChangeVidBrdSrc(NULL);
		}
	}

	//�ı���������
	if ( IsSpeakerCanBrdAud() )
	{
		if ( !tNewSpeaker.IsNull() )
		{
			TMtStatus tMtStatus;
			if ( tLocalNewSpeaker.GetType() == TYPE_MT )
			{
				m_ptMtTable->GetMtStatus( tLocalNewSpeaker.GetMtId(), &tMtStatus );
			}
			if ( tMtStatus.IsSendAudio() || TYPE_MCUPERI == tLocalNewSpeaker.GetType() )
			{
				ChangeAudBrdSrc( &tLocalNewSpeaker );
			}
			else
			{
				ChangeAudBrdSrc(NULL);
			}
		}
		else
		{
			ChangeAudBrdSrc(NULL);
		}
	}

	if ( !tNewSpeaker.IsNull() && tNewSpeaker.GetType() == TYPE_MT )
	{
		//ָ������ʱ�μӻ���
		if( m_tConf.m_tStatus.IsSpecMixing() )
		{
            // xsl [7/28/2006] ��¼ԭ���Ƿ��ڻ���������ȡ��������ʱ�Ƿ�Ӷ��ƻ����б���ɾ��
            if (tNewSpeaker.IsLocal())
            {
                if (m_ptMtTable->IsMtInMixing(tNewSpeaker.GetMtId()))
                {
                    m_tConfInStatus.SetLastSpeakerInMixing(TRUE);
                }
            }
            else
            {
                if (m_ptConfOtherMcTable->IsMtInMixing(tNewSpeaker))
                {
                    m_tConfInStatus.SetLastSpeakerInMixing(TRUE);
                }
            }
			
            if (!m_tConfInStatus.IsLastSpeakerInMixing())
            {
                // zbq [05/14/2007] ��Ƶ�߼�ͨ�����������ɰѸ÷����˼��������
                // ���򣬿�������Ƶ�߼�ͨ����ʧ�� �� �غ��Ҷϵķ�����(��Ƶ��
                // ��ͨ����δ��)�����߽�����Ƶ�߼�ͨ���ɹ���ʱ����Ӧ�Ĵ���.
                TLogicalChannel tLogicChan;
                if (m_ptMtTable->GetMtLogicChnnl(tLocalNewSpeaker.GetMtId(), LOGCHL_AUDIO, &tLogicChan, TRUE))
                {
                    AddRemoveSpecMixMember( &tLocalNewSpeaker, 1, TRUE );
                }
            }		    
		}

		// �·���������Դ�����߼�, zgc, 2008-04-12
        if ( !IsSpeakerCanBrdVid() )
        {
            ChangeSpeakerSrc( MODE_VIDEO, emReasonChangeSpeaker );
        }
        if ( !IsSpeakerCanBrdAud() )
        {
            ChangeSpeakerSrc( MODE_AUDIO, emReasonChangeSpeaker );
        }
	}
	//���뻭��ϳ�
	// xliang [3/13/2009] change either format or param
	if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE && bAddToVmp)
	{        
		if ( IsChangeVmpParam(&tLocalNewSpeaker) )
		{
			if (m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_SPEAKER)	//���Ƿ����˸��棬����ȫ�ֵ���ͨ��
				|| IsDynamicVmp()														//�Զ�����ϳ������ȫ�ֵ���ͨ��
				|| ( m_tConf.m_tStatus.GetVmpParam().GetVMPSchemeId() != 0 )			//����������VMP������ҲҪȫ�ֵ�������ˢ��memstatus.(eg��������Ҫ�ı߿�ɫ)
				) 
			{
				//��ĳͨ�������·����˻�ɷ����ˣ�������changeParam; ����ʱ��ȥ����
				if( (!tLocalNewSpeaker.IsNull() && m_tConf.m_tStatus.GetVmpParam().IsMtInMember( tLocalNewSpeaker )) //FIXME����localspeaker�Ƿ����
					|| (!m_tLastSpeaker.IsNull() && m_tConf.m_tStatus.GetVmpParam().IsMtInMember( m_tLastSpeaker ) )
					)
				{
					ChangeVmpChannelParam(&tLocalNewSpeaker, VMP_MEMBERTYPE_SPEAKER, &m_tLastSpeaker );
				}
				else
				{
					SetTimer(MCUVC_CHANGE_VMPCHAN_SPEAKER_TIMER, TIMESPACE_CHANGEVMPPARAM, VMP_MEMBERTYPE_SPEAKER);
				}
			}
			else
			{
				if( m_tConf.m_tStatus.GetVmpParam().IsMtInMember( tLocalNewSpeaker ) )
				{
					TPeriEqpStatus tPeriEqpStatus; 
					g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
					u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

					TVMPParam	tVMPParam   = m_tConf.m_tStatus.GetVmpParam();   

					if(byVmpSubType != VMP)//�õ�VMP��MPU
					{
						
						// xliang [4/21/2009] �����ֱ��ʣ���new speaker ��VMPǰ����ͨ��
						u8	byVmpStyle  = tVMPParam.GetVMPStyle();
						u8	byChlPos	= tVMPParam.GetChlOfMtInMember(tLocalNewSpeaker);

						if( !m_tConf.m_tStatus.IsBrdstVMP() )
						{
							// xliang [3/27/2009] ��֮ǰ��ѡ���ĳ�Ա������������������ڽϺ������֣�
							//����������������ǰ����ͨ������Ϣ��
							//���ͷŵ���ѡ��Mtռ��VMPǰ����ͨ���������speakerȥռǰ����ͨ��
							u8 byHdChnnlNum = MAXNUM_SVMP_HDCHNNL;
							if( byVmpSubType == MPU_DVMP || byVmpSubType == EVPU_DVMP)
							{	
								byHdChnnlNum = MAXNUM_DVMP_HDCHNNL;
							}
							//u8 byHdChnnlNum = m_tVmpChnnlInfo.m_byHDChnnlNum;
							TChnnlMemberInfo tChnnlMemInfo;
							u8 byLoop;
							for(byLoop = 0; byLoop < byHdChnnlNum; byLoop ++)
							{
								m_tVmpChnnlInfo.GetHdChnnlInfo(byLoop,&tChnnlMemInfo);
								if( tChnnlMemInfo.IsAttrSelected() )
								{
									// xliang [4/8/2009] ��������������ѡ����MT�Ǳ��·�����ѡ����
									//                    ��������ñ�ѡ��MT��VIP���
									//                    ����ñ������佻�����ᱻ��
									TMtStatus tMtStatus;
									u8 byMode = MODE_VIDEO;
									m_ptMtTable->GetMtStatus(tLocalNewSpeaker.GetMtId(),&tMtStatus);
									if (tChnnlMemInfo.GetMt() == tMtStatus.GetSelectMt( byMode ))
									{
										continue;
									}
									m_tVmpChnnlInfo.ClearOneChnnl(tChnnlMemInfo.GetMt()); 
									//��Mt�ֱ��ʵĶ����ú�
									//u8 byPos = tVMPParam.GetChlOfMtInMember(tChnnlMemInfo.GetMt());
									//ChangeMtVideoFormat(tChnnlMemInfo.GetMtId(),byVmpSubType,byVmpStyle,byPos);
								}
							}
							//�����˳���ռǰVMP����ͨ��
							ChangeMtVideoFormat(tLocalNewSpeaker, &tVMPParam);
						}

					}
					else	//��VMPֱ�ӻָ��ֱ���
					{
						ChangeMtVideoFormat(tLocalNewSpeaker, &tVMPParam, FALSE);	
					}
				}
			}
		}
			

		
		/*if (IsChangeVmpParam(&tLocalNewSpeaker) && 
			// zbq [05/31/2007] ������VMPͨ�������Ƿ����˸��棬����Ҫ����ͨ����� 
			(m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) ||
			m_tConf.m_tStatus.GetVmpParam().IsMtInMember( tLocalNewSpeaker ) || 
			m_tConf.m_tStatus.GetVmpParam().IsMtInMember( m_tLastSpeaker ) )
			)
		{
			ChangeVmpChannelParam(&tLocalNewSpeaker, VMP_MEMBERTYPE_SPEAKER, &m_tLastSpeaker );
		}
		else
		{
			SetTimer(MCUVC_CHANGE_VMPCHAN_SPEAKER_TIMER, TIMESPACE_CHANGEVMPPARAM, VMP_MEMBERTYPE_SPEAKER);
		}
		*/
		// xsl [8/28/2006] �ָ�����ϳɷֱ���
// 		if ( m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tLocalNewSpeaker) )
// 		{
// 			TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
// 			ChangeMtVideoFormat(tLocalNewSpeaker.GetMtId(), &tVmpParam, FALSE);
// 		}
	}
		
	//����໭�����ǽ
	if (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE && bAddToVmp)
	{
		ChangeVmpTwChannelParam(&tLocalNewSpeaker, VMPTW_MEMBERTYPE_SPEAKER);
		
		// xsl [8/28/2006] �ָ�����ϳɷֱ���
		if ( m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tLocalNewSpeaker) )
		{
			TVMPParam tVmpTwParam = m_tConf.m_tStatus.GetVmpTwParam();
			ChangeMtVideoFormat(tLocalNewSpeaker, &tVmpTwParam, FALSE);
		}
	}
	
	//ǿ�Ƽ������
	if( !tNewSpeaker.IsNull() && m_tConf.m_tStatus.IsMixing() )
	{
		AddMixMember( &tLocalNewSpeaker, DEFAULT_MIXER_VOLUME, TRUE );
	}
	
	//��TvWall�������˸���ʱ��ͬ����������ǽ�е�ͼ��
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
						ChangeTvWallSwitch(&tLocalNewSpeaker, byEqpId, byChnlIdx, TW_MEMBERTYPE_SPEAKER, TW_STATE_CHANGE);
					}
				}
			}
		}
	}

	//�����˸���ʱ��ͬ������HDU�е�ͼ��
	TPeriEqpStatus tHduStatus;
	u8 byHduChnlIdx;
	u8 byHduEqpId;
	for (byHduEqpId = HDUID_MIN; byHduEqpId <= HDUID_MAX; byHduEqpId++)
	{
		if (EQP_TYPE_HDU == g_cMcuVcApp.GetEqpType(byHduEqpId))
		{
			if (g_cMcuVcApp.GetPeriEqpStatus(byHduEqpId, &tHduStatus))
			{
				u8 byMemberType;
				u8 byMtConfIdx;
				for (byHduChnlIdx = 0; byHduChnlIdx < MAXNUM_HDU_CHANNEL; byHduChnlIdx++)
				{
					byMemberType = tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].byMemberType;
					byMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetConfIdx();
					if (TW_MEMBERTYPE_SPEAKER == byMemberType && m_byConfIdx == byMtConfIdx && !tNewSpeaker.IsNull())
					{
						ChangeHduSwitch(&tLocalNewSpeaker, byHduEqpId, byHduChnlIdx, TW_MEMBERTYPE_SPEAKER, TW_STATE_CHANGE);
					}
				}
			}
		}
	}


	// �·���������Դ�����߼�, zgc, 2008-04-12
    if ( !IsSpeakerCanBrdVid() )
    {
        ChangeSpeakerSrc( MODE_VIDEO, emReasonChangeSpeaker );
    }
    if ( !IsSpeakerCanBrdAud() )
    {
        ChangeSpeakerSrc( MODE_AUDIO, emReasonChangeSpeaker );
    }

	return;
}

/*====================================================================
    ������      ��ChangeSpeaker
    ����        ���ı䷢���ˣ�����������������
	              ֪ͨMC��MT
				  ����¡��ɷ�����ֱ����֪ͨ������ı�״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt * ptNewSpeaker, �·����ˣ�NULL��ʾ�������������TMtҪ�����������նˣ���ҪGetLocal��
				  BOOL32 bPolling, �Ƿ���Polling���µķ����˸ı�                  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/01/04    1.0         LI Yi         ����
	03/11/25    3.0         ������        �޸�
	05/1/27     3.6         Jason        �޸�
====================================================================*/
void CMcuVcInst::ChangeSpeaker( TMt * ptNewSpeaker, BOOL32 bPolling, BOOL32 bAddToVmp )
{
	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );

    TMt tSpeakerMt;
    tSpeakerMt.SetNull();

	// ����VCS���������ȡ�Զ��������˼��뻭��ϳ�
	if (VCS_CONF == m_tConf.GetConfSource())
	{
		bAddToVmp = FALSE;
	}

	if(ptNewSpeaker != NULL)
    {
        tSpeakerMt = *ptNewSpeaker;
    }
	
	TMt  tOldSpeaker = m_tConf.GetSpeaker();

    ConfLog( FALSE, "[ChangeSpeaker]Old speaker(mcuid:%d, mtid:%d) was cancelled and new speaker(mcuid:%d, mtid:%d) is specified!\n",
		            tOldSpeaker.GetMcuId(), tOldSpeaker.GetMtId(), tSpeakerMt.GetMcuId(), tSpeakerMt.GetMtId());

	//�ڴ���Ƶ����ѯ�㲥ʱ��������һ�ն˷��ԣ���ѯֹͣ
	if( !bPolling )
	{
		if( m_tConf.m_tStatus.GetPollMedia() == MODE_BOTH)
		{
			m_tConf.m_tStatus.SetPollMode( CONF_POLLMODE_NONE );
			m_tConf.m_tStatus.SetPollState( POLL_STATE_NONE );
			cServMsg.SetMsgBody( (u8*)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo) );
			SendMsgToAllMcs( MCU_MCS_POLLSTATE_NOTIF, cServMsg );
			KillTimer( MCUVC_POLLING_CHANGE_TIMER );

			//ȡ����ѯ����ȡ����ѯ��������Ӧ����״̬
			CancelOneVmpFollowSwitch( VMP_MEMBERTYPE_POLL, TRUE );
		}
	}

	//ȡ��ԭ������ // xsl [8/4/2006] �����·��������Ϸ�������ͬ�����
	if( m_tConf.HasSpeaker() && (NULL == ptNewSpeaker || (NULL != ptNewSpeaker && !(tOldSpeaker == *ptNewSpeaker))) )
	{
		BOOL32 bHasNewSpeaker = ( NULL != ptNewSpeaker ) ? TRUE : FALSE;
		AdjustOldSpeakerSwitch( tOldSpeaker, bHasNewSpeaker );

        //  xsl [6/20/2006] ȡ�������˻��滻������ʱ����˫��Ϊ�����˿�������ֹͣ˫��
        if ( ( NULL == ptNewSpeaker || ( TYPE_MCUPERI != ptNewSpeaker->GetType()
				                         //zbq [09/27/2007] ��������¼�����Ҳ��Ϊͣ˫��
                                        || ( TYPE_MCUPERI == ptNewSpeaker->GetType() && EQP_TYPE_RECORDER == ptNewSpeaker->GetEqpType() ) ) )
             // zbq [08/26/2007] �����˲�һ���� m_tVidBrdSrc��������VMP�㲥ʱ�ķ�����
			 && TYPE_MCUPERI != tOldSpeaker.GetType() /*m_tVidBrdSrc.GetType()*/)
        {
            if (CONF_DUALMODE_SPEAKERONLY == m_tConf.GetConfAttrb().GetDualMode()
				&& !m_tDoubleStreamSrc.IsNull() 
                // zbq [08/26/2007] �����˲�һ���� m_tVidBrdSrc��������VMP�㲥ʱ�ķ�����
                // zbq [08/31/2007] ������Ҫȡ���ط�����
                && m_tDoubleStreamSrc == GetLocalMtFromOtherMcuMt(tOldSpeaker) /*m_tVidBrdSrc*/)
            {
                StopDoubleStream(TRUE, TRUE);
            }
        }               
	}
	else
	{
//		m_tLastSpeaker.SetNull();
	}

	AdjustNewSpeakerSwitch( tSpeakerMt, bAddToVmp );

	//֪ͨ���л��
	cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
	SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
	
	//֪ͨ�����ն�
	//��֪ͨ�ն�ʹ�õĻ�����Ϣ�ṹ 2005-10-11
	TConfStatus tConfStatus = m_tConf.GetStatus();
	TSimConfInfo tSimConfInfo;
	tSimConfInfo.m_tSpeaker  = GetLocalSpeaker();
	tSimConfInfo.m_tChairMan = m_tConf.GetChairman();
    // guzh [11/6/2007] 
	tSimConfInfo.SetVACMode(tConfStatus.IsVACing());
	tSimConfInfo.SetVMPMode(tConfStatus.GetVMPMode());
    // guzh [11/6/2007] 
	tSimConfInfo.SetDiscussMode(tConfStatus.IsMixing());
	cServMsg.SetMsgBody( ( u8 * )&tSimConfInfo, sizeof( tSimConfInfo ) );
	BroadcastToAllSubMtJoinedConf( MCU_MT_SIMPLECONF_NOTIF, cServMsg );

    //n+1���ݸ��·�������Ϣ
    if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
    {
        ProcNPlusSpeakerUpdate();
    }	

#ifdef _SATELITE_
    RefreshConfState();
	

	RefreshMtBitRate();
#endif

    return;
}

/*====================================================================
    ������      ��IsChangeVmpParam
    ����        ���Ƿ���Ҫ�ı仭��ϳɲ���

    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����

    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/06/15    3.6         libo          ����
    06/04/24    4.0         �ű���        ��Ӧ�Զ��ϳɼ�����ϯ�ͷ�����
====================================================================*/
BOOL32 CMcuVcInst::IsChangeVmpParam(TMt * ptMt)
{  
    if ( !m_tConfInStatus.IsVmpNotify() )
    {
        EqpLog( "[IsChangeVmpParam] m_bVmpNotify: %d, No change !\n", m_tConfInStatus.IsVmpNotify() );
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================
�� �� ���� GetSatCastChnnlNum
��    �ܣ�  ��ȡ��ǰ��ɢ����ش�ͨ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u8  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/8/22  4.0			������                  ����
=============================================================================*/
u8  CMcuVcInst::GetSatCastChnnlNum(u8 bySrcMtId)
{
    u8 byChnnlNum = 0;
    for(u8 byIdx = 1; byIdx <= MAXNUM_CONF_MT; byIdx++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byIdx) && bySrcMtId != byIdx 
            && (m_ptMtTable->GetMtSndBitrate(byIdx) > 0 || m_ptMtTable->GetMtSndBitrate(byIdx, LOGCHL_SECVIDEO) > 0) )
        {
            byChnnlNum++;
        }
    }
    return byChnnlNum;
}

/*=============================================================================
  �� �� ���� IsOverSatCastChnnlNum
  ��    �ܣ� ���Ƿ�ɢ�������Ƿ񳬹����ش�ͨ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcInst::IsOverSatCastChnnlNum(u8 bySrcMtId, u8 byIncNum/* = 1*/)
{       
    return ((GetSatCastChnnlNum(bySrcMtId)+byIncNum) > m_tConf.GetSatDCastChnlNum());
}

/*=============================================================================
  �� �� ���� IsOverSatCastChnnlNum
  ��    �ܣ� ���Ƿ�ɢ�������Ƿ񳬹����ش�ͨ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TVMPParam tVmpParam
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcInst::IsOverSatCastChnnlNum(TVMPParam &tVmpParam)
{
    u8 byIncNum = 0;
    for (u8 byMemIdx = 0; byMemIdx < tVmpParam.GetMaxMemberNum(); byMemIdx++)
    {
        TMt *ptMt = (TMt *)tVmpParam.GetVmpMember(byMemIdx);
        if (m_tConfAllMtInfo.MtJoinedConf(ptMt->GetMtId()) &&
            m_ptMtTable->GetMtSndBitrate(ptMt->GetMtId()) == 0)
        {
            byIncNum++;
        }
    }

    return IsOverSatCastChnnlNum(0, byIncNum);
}

/*=============================================================================
  �� �� ���� ChangeSatDConfMtRcvAddr
  ��    �ܣ� �ı��ɢ�������ն˽��յ�ַ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� BOOL32 bMulti ��Ϊ�ಥ��Ϊ����ʱ�趨�Ķಥ��ַ��������ַΪ0
  �� �� ֵ�� TTransportAddr 
=============================================================================*/
void CMcuVcInst::ChangeSatDConfMtRcvAddr(u8 byMtId, u8 byChnnlType, BOOL32 bMulti/*= TRUE*/)
{
    TTransportAddr tMtRcvAddr;   
    //�ಥ
    if (bMulti)
    {
        tMtRcvAddr.SetIpAddr(m_tConf.GetConfAttrb().GetSatDCastIp());
        
        u16 wPort = m_tConf.GetConfAttrb().GetSatDCastPort();
        if (LOGCHL_VIDEO == byChnnlType)
        {
            tMtRcvAddr.SetPort(wPort);
        }        
        else if (LOGCHL_AUDIO == byChnnlType)
        {
            tMtRcvAddr.SetPort(wPort+2);
        }
        else if (LOGCHL_SECVIDEO == byChnnlType)
        {
            tMtRcvAddr.SetPort(wPort+4);
        }
        else
        {
            ConfLog(FALSE, "[ChangeSatDConfMtRcvAddr] invalid channel type %d!\n", byChnnlType);
        }
    }
    //����
    else
    {
        TLogicalChannel tLogicChannel;
        m_ptMtTable->GetMtLogicChnnl(byMtId, byChnnlType, &tLogicChannel, TRUE);
        tMtRcvAddr.SetIpAddr(0);
        tMtRcvAddr.SetPort(tLogicChannel.GetRcvMediaChannel().GetPort());        
    }

    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_MT_SATDCONFCHGADDR_CMD);
    cServMsg.SetMsgBody(&byChnnlType, sizeof(byChnnlType));
    cServMsg.CatMsgBody((u8*)&tMtRcvAddr, sizeof(tMtRcvAddr));
    SendMsgToMt(byMtId, MCU_MT_SATDCONFCHGADDR_CMD, cServMsg);

    Mt2Log("[ChangeSatDConfMtRcvAddr] byMtId.%d byChnnlType.%d MtRcvAddr<0x%x, %d>.\n", 
        byMtId, byChnnlType, tMtRcvAddr.GetIpAddr(), tMtRcvAddr.GetPort());

    return;
}

/*=============================================================================
�� �� ���� IsMtSendingVidToOthers
��    �ܣ� �ն˳�����Ƶ�㲥���Ƿ���������ʵ�巢������, �����ڷ�ɢ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TMt tMt
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/8/23  4.0			������                  ����
=============================================================================*/
BOOL32 CMcuVcInst::IsMtSendingVidToOthers(TMt tMt, BOOL32 bConfPoll/*with audio*/, BOOL32 bTwPoll, u8 bySelDstMtId)
{	
    TMtStatus tStatus;
    TPeriEqpStatus tTWStatus;

    //�Ƿ��ڹ㲥(����Ƶ������ѯʱ���ж�)
    if (!bConfPoll)
    {
        if (tMt == GetLocalSpeaker())
        {
            return TRUE;
        }
    }      

    //�Ƿ���vmp��
    if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE)
    {
        if (m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tMt))
        {
            return TRUE;
        }
    }

    //�Ƿ���tvwall��(����ǽ��ѯʱ���ж�)
    if (!bTwPoll)
    {
        for(u8 byTvId = TVWALLID_MIN; byTvId <= TVWALLID_MAX; byTvId++)
        {      
            g_cMcuVcApp.GetPeriEqpStatus(byTvId, &tTWStatus);
            if( tTWStatus.m_byOnline )
            {
                for(u8 byLp = 0; byLp < tTWStatus.m_tStatus.tTvWall.byChnnlNum; byLp++)
                {
                    TMt tMtInTv = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byLp];
                    if(tMtInTv.GetMtId() == tMt.GetMtId())
                    {
                        return TRUE;
                    }
                }
            }
        }
    }    

    //�Ƿ���mtw��
    if (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE)
    {
        if (m_tConf.m_tStatus.GetVmpTwParam().IsMtInMember(tMt))
        {
            return TRUE;
        }
    }

    //�Ƿ��ڻ�ؼ��
    if (IsMtInMcSrc(tMt.GetMtId(), MODE_VIDEO))
    {
        return TRUE;
    }

    //�Ƿ����ն�¼��
	m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tStatus);
	if (tStatus.m_tRecState.IsRecording())
	{
		return TRUE;
	}

    //�Ƿ��ڱ�ѡ��
    for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (!m_tConfAllMtInfo.MtJoinedConf(byLoop) || byLoop == bySelDstMtId)//ȥ����Ŀ���նˣ����Ƿ��������ն���ѡ��
        {
            continue;
        }      
		
		if (m_ptMtTable->GetMtStatus(byLoop, &tStatus))
        {
            if (tStatus.GetSelectMt(MODE_VIDEO) == tMt)
            {               
                return TRUE;
            }
        }
    }

    //��ɢ���鲻֧�ּ��������жϻش�

    return FALSE;
}

/*=============================================================================
�� �� ���� IsMtInMcSrc
��    �ܣ�  �ն��Ƿ��ڻ�ؼ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMtId
           u8 byMode
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/8/23  4.0			������                  ����
=============================================================================*/
BOOL32 CMcuVcInst::IsMtInMcSrc(u8 byMtId, u8 byMode)
{
    TLogicalChannel tLogicalChannel;
    TMt tMt;
    for( u8 byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( g_cMcuVcApp.IsMcConnected( byIndex ) )
		{
			u8 byChannelNum;
			if( g_cMcuVcApp.GetMcLogicChnnl( byIndex, byMode, &byChannelNum, &tLogicalChannel ) )
			{
				while( byChannelNum-- != 0 )
				{
					if( g_cMcuVcApp.GetMcSrc( byIndex, &tMt, byChannelNum, byMode ) )
					{
						if (tMt.GetMtId() == byMtId)
                        {
                            return TRUE;
                        }
					}
				}
			}
		}
	}

    return FALSE;
}

/*====================================================================
    ������      ��RestoreAllSubMtJoinedConfWatchingSrcMt
    ����        ��1. ����ն���˭����(��)ƵԴΪtSrc��, �������з����ˣ�
                     ���տ�(��)�����ˣ��޷����ˣ�ֹͣ����Ľ�����
                  2. δ����ƵԴ�Ĳ�Ҫͣ��
                  3. ��ֹͣ����ĳ����ն˵Ľ����󣬼�����ѡ���ն��Ƿ�Ϊ�գ�
                     ����Ϊ�գ����ٽ�ѡ���ն˵���ƵԴ������������նˡ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt & tSrc, ָ���ն� 
				  u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/26    1.0         JQL           ����
    06/04/14    4.0         �ű���        ͣtSrc�Ľ������жϻָ�ѡ������
====================================================================*/
BOOL32 CMcuVcInst::RestoreAllSubMtJoinedConfWatchingSrcMt(TMt tSrc, u8 byMode )
{
	TMt tCurSrc;
	u8  byMtLoop;
	TMtStatus tStat;

	tSrc = GetLocalMtFromOtherMcuMt(tSrc);
	for( byMtLoop = 1; byMtLoop <= MAXNUM_CONF_MT; byMtLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byMtLoop ) )
		{
			m_ptMtTable->GetMtStatus(byMtLoop, &tStat);
		    if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
			{				
				if (tStat.GetSelectMt(MODE_VIDEO) == tSrc)
				{
					StopSelectSrc(m_ptMtTable->GetMt(byMtLoop), MODE_VIDEO, FALSE);
				}

			    m_ptMtTable->GetMtSrc( byMtLoop, &tCurSrc, MODE_VIDEO ); 
			    if( tCurSrc == tSrc )
				{
					if(tSrc == m_tVmpEqp && m_tConf.m_tStatus.IsVmpSeeByChairman()
						&& m_tConf.GetChairman().GetMtId() == byMtLoop )
					{
						// xliang [4/2/2009] VMP ��������������MT�����Դ˴���ͣ����
					}
					else
					{
						//��KEDA���̿��Լ���KEDAͣ����
						RestoreRcvMediaBrdSrc( byMtLoop, MODE_VIDEO, FALSE );
					}
                    //�жϻָ�ѡ��
//                    RestoreMtSelectStatus( byMtLoop, MODE_VIDEO );
				}
			}

            if( byMode == MODE_AUDIO || byMode == MODE_BOTH ) 
			{
				if (tStat.GetSelectMt(MODE_AUDIO) == tSrc)
				{
					StopSelectSrc(m_ptMtTable->GetMt(byMtLoop), MODE_AUDIO, FALSE);
				}
				m_ptMtTable->GetMtSrc( byMtLoop, &tCurSrc, MODE_AUDIO ); 
			    if( tCurSrc == tSrc/* || ( tSrc.GetMtId() == byMtLoop && tSrc.GetType() == TYPE_MT )*/ )
				{
                    //��KEDA���̿��Լ���KEDAͣ����
					RestoreRcvMediaBrdSrc( byMtLoop, MODE_AUDIO, FALSE );

                    //�жϻָ�ѡ��
//                    RestoreMtSelectStatus( byMtLoop, MODE_AUDIO );
				}			
			}
		}
	}

	MtStatusChange();

	return TRUE;
}

/*====================================================================
    ������      ��RestoreRcvMediaBrdSrc
    ����        ��ָ���ָ����չ㲥ý��Դ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt & tMt, ָ���ն�
				  u8 byMode, �ָ�ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/01/23    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::RestoreRcvMediaBrdSrc( u8 byMtId, u8 byMode, BOOL32 bMsgMtStutas )
{
	CServMsg cServMsg;
	u8		 bySrcChnnl;
	TMt      tMt = m_ptMtTable->GetMt( byMtId );

	cServMsg.SetConfId( m_tConf.GetConfId() );

	// xliang [3/31/2009] ������VMP�㲥���ָ���������ͨ����
	BOOL32 bHdVmpBrdst = FALSE;
	if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE	//��������
		&& m_tConf.m_tStatus.IsBrdstVMP())
	{
		m_tVidBrdSrc = m_tVmpEqp;	// xliang [7/28/2009] ��Ϊʱ�����⣬������ظ���ֵ
		TPeriEqpStatus tPeriEqpStatus; 
		g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
		u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
		if(byVmpSubType != VMP)
		{
			bHdVmpBrdst = TRUE;
		}
	}

	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
        // xsl [8/19/2006] ���ǻ�������鲥����
		if( !m_tVidBrdSrc.IsNull() && !m_tConf.GetConfAttrb().IsSatDCastMode() )
		{
            //��kdc�����й㲥Դ�ҽ�ʡ����ʱ��֪ͨmtֹͣ��������
            if(g_cMcuVcApp.IsSavingBandwidth() && MT_MANU_KDC != m_ptMtTable->GetManuId( tMt.GetMtId()))
            {
                NotifyOtherMtSend(byMtId, FALSE);
            }

			if(bHdVmpBrdst)
			{
				ConfLog(FALSE, "[RestoreRcvMediaBrdSrc] Call SwitchNewVmpToSingleMt here!\n");
				SwitchNewVmpToSingleMt(tMt);
				return;
			}

		    bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0; 
            // guzh [3/21/2007]
            //StartSwitchToSubMt( m_tVidBrdSrc, bySrcChnnl, byMtId, MODE_VIDEO, SWITCH_MODE_BROADCAST, bMsgMtStutas );
            StartSwitchFromBrd(m_tVidBrdSrc, bySrcChnnl, 1, &tMt);
		}
	    else
		{
			if( m_ptMtTable->GetManuId( tMt.GetMtId() ) == MT_MANU_KDC )
			{
				StopSwitchToSubMt( byMtId, MODE_VIDEO, SWITCH_MODE_BROADCAST, bMsgMtStutas );

                // xsl [8/19/2006] ���ǻ�������鲥����
                if (m_tConf.GetConfAttrb().IsSatDCastMode())
                {
                    ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_VIDEO);
                }
			}
			else
			{
                //��kdc�����޷�����ʱ�ش�������Ƶ, ��ʡ����ʱ��֪ͨmt��������
                if(g_cMcuVcApp.IsSavingBandwidth())
                {
                    NotifyOtherMtSend(byMtId, TRUE);
                }

				StartSwitchToSubMt( tMt, 0, tMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, bMsgMtStutas );
			}
		}
	}

	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
        // xsl [8/19/2006] ���ǻ�������鲥����
        // guzh [4/23/2007] ֱ��ȡ��Ƶ�㲥Դ
		if( !m_tAudBrdSrc.IsNull() && !m_tConf.GetConfAttrb().IsSatDCastMode() )
		{
			if( m_tConf.m_tStatus.IsNoMixing()  )
			{
				bySrcChnnl = (m_tConf.GetSpeaker() == m_tPlayEqp) ? m_byPlayChnnl : 0; 
				StartSwitchToSubMt( GetLocalSpeaker(), bySrcChnnl, byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgMtStutas );
			}
		}
	    else
		{
			if( !m_tCascadeMMCU.IsNull() && byMtId == m_tCascadeMMCU.GetMtId() )
			{
				// ������ϼ��������նˣ����ƻ��˻ش�ͨ������
				// StartSwitchToSubMt( tMt, 0, tMt.GetMtId(), MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgMtStutas );
			}
			else
			{
				StopSwitchToSubMt( byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgMtStutas );

                // xsl [8/19/2006] ���ǻ�������鲥����
                if (m_tConf.GetConfAttrb().IsSatDCastMode())
                {
                    ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_AUDIO);
                }
			}
		}
	}
}

/*====================================================================
    ������      ��ConfStatusChange
    ����        ���ı����״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/01/23    1.0         LI Yi         ����
	04/03/07    3.0         ������        ������
====================================================================*/
void CMcuVcInst::ConfStatusChange( )
{
	CServMsg	cServMsg;

	//notify all meeting consoles
	cServMsg.SetMsgBody( ( u8 * )&( m_tConf.m_tStatus ), sizeof( TConfStatus ) );
	SendMsgToAllMcs( MCU_MCS_CONFSTATUS_NOTIF, cServMsg );	
	
	SendConfInfoToChairMt();
}

/*=============================================================================
  �� �� ���� ConfModeChange
  ��    �ܣ� ����ģʽ֪ͨ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::ConfModeChange()
{
    CServMsg	cServMsg;    
    cServMsg.SetMsgBody( ( u8 * )&( m_tConf.m_tStatus.m_tConfMode ), sizeof( TConfMode ) );

    SendMsgToAllMcs( MCU_MCS_CONFMODE_NOTIF, cServMsg );	

    SendConfInfoToChairMt();
}

/*=============================================================================
  �� �� ���� SendConfInfoToChair
  ��    �ܣ� ���ͻ�����Ϣ����ϯ�ն�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::SendConfInfoToChairMt(void)
{
    //������֪ͨ
    if( HasJoinedChairman() )
    {
        TMt tMt = m_tConf.GetChairman();
        TMt tSpeaker = m_tConf.GetSpeaker();
        m_tConf.SetSpeaker( GetLocalSpeaker() );
        CServMsg	cServMsg;    
        cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
        SendMsgToMt( tMt.GetMtId(), MCU_MT_CONF_NOTIF, cServMsg );
        m_tConf.SetSpeaker( tSpeaker );
    }
}

/*====================================================================
    ������      ��MtStatusChange
    ����        ���ı��ն�״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8     byMtId  : �����״̬�ı��ն�ID, 0��ʾ���е��ն�.
                  BOOL32 bForcely: �Ƿ������ϱ���FALSE: ��ѭ�ϱ��������Լ����TRUE: ʵʱ�ϱ�.
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/05/08    3.0         ������        ������
====================================================================*/
void CMcuVcInst::MtStatusChange( u8 byMtId, BOOL32 bForcely )
{
	CServMsg	cServMsg;
	TMtStatus   tMtStatus;

	u32 dwCurTick = OspTickGet();
	BOOL32 bHasStat = FALSE;

	u8 byLoop;
	
    if ( !bForcely )
    {
        if( dwCurTick - m_tRefreshParam.dwMcsLastTick >= m_tRefreshParam.GetMcsRefreshInterval() )
        {
            KillTimer( MCUVC_REFRESH_MCS_TIMER );
            m_tRefreshParam.dwMcsLastTick = dwCurTick;
            bHasStat = TRUE;
            
            cServMsg.SetMsgBody( );
            if ( 0 == byMtId )
            {
                for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
                {
                    if( m_tConfAllMtInfo.MtInConf( byLoop ) && 
                        m_ptMtTable->GetMtStatus( byLoop, &tMtStatus ) ) 
                    {
                        cServMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
                    }
                }                
            }
            else if ( byMtId <= MAXNUM_CONF_MT )
            {
                if( m_tConfAllMtInfo.MtInConf( byMtId ) && 
                    m_ptMtTable->GetMtStatus( byMtId, &tMtStatus ) ) 
                {
                    cServMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
                }
            }
            else
            {
                ConfLog( FALSE, "[MtStatusChange] unexpected MtId.%d\n", byMtId );
            }
            
            
            if ( 0 != cServMsg.GetMsgBodyLen() )
            {
                SendMsgToAllMcs( MCU_MCS_MTSTATUS_NOTIF, cServMsg );
            }
        }
        else
        {
            SetTimer( MCUVC_REFRESH_MCS_TIMER, m_tRefreshParam.dwMcsRefreshInterval*1000, byMtId );
        }
    }
    else
    {
        cServMsg.SetMsgBody( );
        if ( 0 == byMtId )
        {
            for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
            {
                if( m_tConfAllMtInfo.MtInConf( byLoop ) && 
                    m_ptMtTable->GetMtStatus( byLoop, &tMtStatus ) ) 
                {
                    cServMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
                }
            }                
        }
        else if ( byMtId <= MAXNUM_CONF_MT )
        {
            if( m_tConfAllMtInfo.MtInConf( byMtId ) && 
                m_ptMtTable->GetMtStatus( byMtId, &tMtStatus ) ) 
            {
                cServMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
            }
        }
        else
        {
            ConfLog( FALSE, "[MtStatusChange] unexpected MtId.%d\n", byMtId );
        }
        
        
        if ( 0 != cServMsg.GetMsgBodyLen() )
        {
            SendMsgToAllMcs( MCU_MCS_MTSTATUS_NOTIF, cServMsg );
        }        
    }

    return;
}

/*====================================================================
    ������      : ProcTimerRefreshMmcuMcs
    ����        ����ˢ�»��ʱ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/11/04    3.5         ������           ����
====================================================================*/
void CMcuVcInst::ProcTimerRefreshMcs(  const CMessage * pcMsg  )
{
	CServMsg	cServMsg;
	TMtStatus   tMtStatus;
	u8 byLoop;
    u8 byMtId = *(u8*)pcMsg->content;

	switch( CurState() )
	{
	case STATE_ONGOING:

		m_tRefreshParam.dwMcsLastTick = OspTickGet();

        if ( 0 == byMtId )
        {
            cServMsg.SetMsgBody( );
            for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
            {
                if( m_tConfAllMtInfo.MtInConf( byLoop ) && 
                    m_ptMtTable->GetMtStatus( byLoop, &tMtStatus ) ) 
                {
                    cServMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
                }
            }            
        }
        else if ( byMtId <= MAXNUM_CONF_MT )
        {
            if( m_tConfAllMtInfo.MtInConf( byMtId ) && 
                m_ptMtTable->GetMtStatus( byMtId, &tMtStatus ) ) 
            {
                cServMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
            }
        }
        else
        {
            ConfLog( FALSE, "[ProcTimerRefreshMcs] unexpected MtId.%d\n", byMtId );
        }

		SendMsgToAllMcs( MCU_MCS_MTSTATUS_NOTIF, cServMsg );
	
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message TIMER_REFRESH_MCS received in state %u!\n",CurState() );
		break;
	}
	
}

/*====================================================================
    ������      : SendMcuMediaSrcNotify
    ����        : ����MCU����Դ֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bFource = FALSE TRUE-�����Ƿ�仯,ǿ��֪ͨ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/11/04    3.5         ������           ����
====================================================================*/
void CMcuVcInst::SendMcuMediaSrcNotify(BOOL32 bFource)
{
	CServMsg cServMsg;

	TConfMcInfo * ptMcInfo = m_ptConfOtherMcTable->m_atConfOtherMcInfo;

	for (s32 nLoop = 0; nLoop < MAXNUM_SUB_MCU; nLoop++)
	{
		u8 byMcuId = ptMcInfo[nLoop].m_byMcuId;
		if (0 == byMcuId)
		{
			continue;
		}

		TMt tMt = GetMcuMediaSrc(byMcuId);

		//������ǿ��֪ͨ��û�б仯�������ظ�֪ͨ
		if (ptMcInfo[nLoop].m_tSrcMt.GetMcuId() == tMt.GetMcuId() && 
			ptMcInfo[nLoop].m_tSrcMt.GetMtId() == tMt.GetMtId() && 
			FALSE == bFource)
		{
			continue;
		}

		//�����ѯ�����ϼ�mcu����ý��ԴΪ�գ�������ý��Դ���Ǹ�mcu
		if ((tMt.IsNull() || 0 == tMt.GetMtId()) && 
			!m_tCascadeMMCU.IsNull() && 
			byMcuId == m_tCascadeMMCU.GetMtId())
		{
			tMt.SetMcuId(LOCAL_MCUID);
			tMt.SetMtId(byMcuId);
		}

		MMcuLog("[ProcTimerMcuSrcCheck] McuId.%d - MtMcuId.%d MtId.%d MtType.%d MtConfIdx.%d\n", 
				byMcuId, tMt.GetMcuId(), tMt.GetMtId(), tMt.GetMtType(), tMt.GetConfIdx());

        // guzh [4/8/2007] �����µ�ý��Դ
		ptMcInfo[nLoop].m_tSrcMt = tMt;
		TMcu tMcu;
		tMcu.SetMcuId(byMcuId);
		cServMsg.SetEventId(MCU_MCS_MCUMEDIASRC_NOTIF);
		cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
		cServMsg.CatMsgBody((u8 *)&tMt, sizeof(tMt));
		SendMsgToAllMcs(MCU_MCS_MCUMEDIASRC_NOTIF, cServMsg);
	}

	return;
}

/*====================================================================
    ������      : ProcTimerMcuSrcCheck
    ����        ��mcuԴ���ʱ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/11/04    3.5         ������           ����
====================================================================*/
void CMcuVcInst::ProcTimerMcuSrcCheck(const CMessage * pcMsg)
{
	STATECHECK;

	SendMcuMediaSrcNotify(FALSE);

	SetTimer(MCUVC_MCUSRC_CHECK_TIMER, TIMESPACE_MCUSRC_CHECK);

	return;	
}

/*====================================================================
    ������      ProcTimerChangeVmpParam
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/06/14    3.6         LIBO          ����
    06/04/25    4.0         �ű���        ��Ӧ�Զ��ϳ���ϯ�ͷ����˵ļ���
====================================================================*/
void CMcuVcInst::ProcTimerChangeVmpParam(const CMessage * pcMsg)
{
    u32 dwVmpMemberType = *(u32 *)pcMsg->content;

    if (m_tConfInStatus.IsVmpNotify())
    {
        if (MCUVC_CHANGE_VMPPARAM_TIMER == pcMsg->event)
        {
            TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpParam();
            //ChangeVmpParam(&tVMPParam);

			// xliang [1/6/2009] ��������VMP��VMP param
			AdjustVmpParam(&tVMPParam);
        }
        else
        {
            TMt tMt;
            tMt.SetNull();
            if (VMP_MEMBERTYPE_SPEAKER == dwVmpMemberType)
            {
                tMt = GetLocalSpeaker(); //FIXME:ȡLocal���Ƿ����
                EqpLog("\n[ProcTimerChangeVmpChanParam] - VMP_MEMBERTYPE_SPEAKER - mtid:%d\n\n", tMt.GetMtId());
            }
            else if (VMP_MEMBERTYPE_CHAIRMAN == dwVmpMemberType)
            {
                tMt = m_tConf.GetChairman();
                EqpLog("\n[ProcTimerChangeVmpChanParam] - VMP_MEMBERTYPE_CHAIRMAN - mtid:%d\n\n", tMt.GetMtId());
            }

            if (tMt.IsNull())
            {                
                return;
            }

            ChangeVmpChannelParam(&tMt, (u8)dwVmpMemberType);          
        }
    }
    else
    {
        SetTimer(pcMsg->event, TIMESPACE_CHANGEVMPPARAM, dwVmpMemberType);
    }
}

/*====================================================================
    ������      ��MtOnlineChange
    ����        ���ն�����״̬�仯
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/08/12    3.0         ������        ����
====================================================================*/
void CMcuVcInst::MtOnlineChange( TMt tMt, BOOL32 bOnline, u8 byReason )
{
	u8 byOnline = bOnline;
	CServMsg	cServMsg;
    cServMsg.SetMsgBody( (u8*)&tMt, sizeof( TMt ) );
	cServMsg.CatMsgBody( (u8*)&byOnline, sizeof( u8 ) );
	cServMsg.CatMsgBody( (u8*)&byReason, sizeof( u8 ) );
	SendMsgToAllMcs( MCU_MCS_MTONLINECHANGE_NOTIF, cServMsg );

	// ����VCS���飬����Ҫͨ�浱ǰVCS���������״̬��Ϣ
	if (VCS_CONF == m_tConf.GetConfSource())
	{
		TMt tCurVCMT  = m_cVCSConfStatus.GetCurVCMT();
		TMt tReqVCMT  = m_cVCSConfStatus.GetReqVCMT();
		TMt tChairMan = m_tConf.GetChairman();

		if (byOnline)
		{
			// ���ݵ���ϯ��ǰ�ľ�����״̬���ö�Ӧ�ն�״̬
			if (tChairMan == tMt)
			{
				VCSMTMute(tMt, m_cVCSConfStatus.IsLocMute(), VCS_AUDPROC_MUTE);
				VCSMTMute(tMt, m_cVCSConfStatus.IsLocSilence(), VCS_AUDPROC_SILENCE);
			}
			else
			{
				if (CONF_CALLMODE_NONE == VCSGetCallMode(tMt) || 
					MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()))
				{
					VCSMTMute(tMt, m_cVCSConfStatus.IsRemMute(), VCS_AUDPROC_MUTE);
					VCSMTMute(tMt, m_cVCSConfStatus.IsRemSilence(), VCS_AUDPROC_SILENCE);
				}
			}
		}

		TMt tNull;
		tNull.SetNull();
		if (tReqVCMT == tMt && byOnline && !tMt.IsLocal())
		{
			// �����¼��ն�����ͨ�棬��Ϊ����ɵ��ȣ������ڱ����ն���Ҫ�ȵ��ն������ͨ������Ϊ�������
			KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
			ChgCurVCMT(tMt);
		}

		if (tCurVCMT == tMt && !byOnline)
		{
			// ���ڵ�ǰ���ȵ��նˣ����ߣ���>������
			m_cVCSConfStatus.SetCurVCMT(tNull);
			// ���������������˼��������ǽģʽ,�Զ����ı����ն���������
			u8 byMode = m_cVCSConfStatus.GetCurVCMode();
			if (VCS_GROUPSPEAK_MODE == byMode || VCS_GROUPTW_MODE == byMode)
			{
				TMt tChairMan = m_tConf.GetChairman();
				ChangeSpeaker(&tChairMan);
			}
		}


	    VCSConfStatusNotif();
	}
}

/*====================================================================
    ������      ��SendReplyBack
    ����        ����Ӧ�𷢸���ȷ����
    �㷨ʵ��    ��������Ϣͷ�е�ԴMCU��MT�ͻỰ�ž�������
    ����ȫ�ֱ�����
    �������˵����CServMsg & cCmdReply, Ӧ����Ϣ
				  u16 wEvent, Ӧ����Ϣ��
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/09    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::SendReplyBack( CServMsg &cReplyMsg, u16 wEvent )
{
	if( IsMtCmdFromThisMcu( cReplyMsg ) )	   // �����ӱ�MCU���ն˷�������Ӧ��
	{
		 SendMsgToMt( cReplyMsg.GetSrcMtId(), wEvent, cReplyMsg ) ;
	}
	else if( IsMcCmdFromThisMcu( cReplyMsg ) ) // �����ӱ�MCU�Ļ������̨��������Ӧ��
	{
		 SendMsgToMcs( cReplyMsg.GetSrcSsnId(), wEvent, cReplyMsg );
	}

	return;
}

/*====================================================================
    ������      ��IsMtCmdFromThisMcu
    ����        ���ж��Ƿ��Ǳ�MCU�ն˷�������ķ���Ӧ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CServMsg & cCmdReply, Ӧ����Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/08    1.0         LI Yi         ����
====================================================================*/
BOOL32 CMcuVcInst::IsMtCmdFromThisMcu(const CServMsg &cCmdReply) const
{
	if( cCmdReply.GetSrcMtId() !=0 )
		return( TRUE );
	else
		return( FALSE );
}

/*====================================================================
    ������      ��IsMcCmdFromThisMcu
    ����        ���ж��Ƿ��Ǳ�MCU�������̨��������ķ���Ӧ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CServMsg & cCmdReply, Ӧ����Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/05    1.0         LI Yi         ����
====================================================================*/
BOOL32 CMcuVcInst::IsMcCmdFromThisMcu( const CServMsg & cCmdReply ) const
{
	if( cCmdReply.GetSrcMtId() == 0 && cCmdReply.GetSrcSsnId() != 0 )
		return( TRUE );
	else
		return( FALSE );
}

/*====================================================================
    ������      ��ClearVcInst
    ����        �����ʵ�� ���ݣ�m_ptMtTable��m_ptSwitchTable��m_ptConfOtherMcTable���⣩
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/08    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ClearVcInst(void)
{
    m_tConf.Reset();

    memset(&m_tConfPollParam, 0, sizeof(m_tConfPollParam));
    memset(&m_tTvWallPollParam, 0, sizeof(m_tTvWallPollParam));
	memset( &m_tHduPollParam, 0x0, sizeof(m_tHduPollParam) );  // add by jlb
    memset(&m_tConfEqpModule, 0, sizeof(m_tConfEqpModule));
    memset(&m_tConfProtectInfo, 0, sizeof(m_tConfProtectInfo));

    memset(&m_tVmpEqp, 0, sizeof(m_tVmpEqp));
	//memset(&m_tLastVmpEqp, 0, sizeof(m_tLastVmpEqp));
	
    memset(&m_tMixEqp, 0, sizeof(m_tMixEqp));
    memset(&m_tAudBasEqp, 0, sizeof(m_tAudBasEqp));         //modify bas 2
    memset(&m_tVidBasEqp, 0, sizeof(m_tVidBasEqp));         //modify bas 2
    memset(&m_tBrBasEqp, 0, sizeof(m_tBrBasEqp));           //modify bas 2
    memset(&m_tCasdAudBasEqp, 0, sizeof(m_tCasdAudBasEqp)); //modify bas 2
    memset(&m_tCasdVidBasEqp, 0, sizeof(m_tCasdVidBasEqp)); //modify bas 2
    memset(&m_tRecEqp, 0, sizeof(m_tRecEqp));
    memset(&m_tPlayEqp, 0, sizeof(m_tPlayEqp));
    memset(&m_tPrsEqp, 0, sizeof(m_tPrsEqp));
    memset(m_abyCasChnCheckTimes, 0, sizeof(m_abyCasChnCheckTimes));
    memset(&m_tRefreshParam, 0, sizeof(m_tRefreshParam));
    memset(&m_abyMixMtId, 0, sizeof(m_abyMixMtId));

    m_tVidBrdSrc.SetNull();
    m_tAudBrdSrc.SetNull();
    m_tLastSpeaker.SetNull();
    m_tVacLastSpeaker.SetNull();
    m_tCascadeMMCU.SetNull();
    m_tConfAllMtInfo.m_tMMCU.SetNull();
	m_tConfAllMtInfo.RemoveAllJoinedMt();

    m_tLastSpyMt.SetNull();    
    memset(&m_tChargeSsnId, 0, sizeof(m_tChargeSsnId));
	memset(&m_tLastVmpParam, 0, sizeof(m_tLastVmpParam));
	memset(&m_tLastVmpTwParam, 0, sizeof(m_tLastVmpTwParam));
    memset(&m_tLastMixParam, 0, sizeof(m_tLastMixParam));
    m_tHduBatchPollInfo.SetNull();
    m_tHduPollSchemeInfo.SetNull();
	m_cVCSConfStatus.VCCDefaultStatus();
    
	m_tDoubleStreamSrc.SetNull();
    m_tH239TokenOwnerInfo.Clear();

    m_tConfInStatus.Reset();

    m_byMixGrpId        = 0;
    m_byRecChnnl        = EQP_CHANNO_INVALID;
    m_byPlayChnnl       = EQP_CHANNO_INVALID;
    m_byAudBasChnnl     = EQP_CHANNO_INVALID; //modify bas 2
    m_byVidBasChnnl     = EQP_CHANNO_INVALID; //modify bas 2
    m_byBrBasChnnl      = EQP_CHANNO_INVALID; //modify bas 2
    m_byCasdAudBasChnnl = EQP_CHANNO_INVALID;
    m_byCasdVidBasChnnl = EQP_CHANNO_INVALID;
    m_wVidBasBitrate    = 0;
    m_wBasBitrate       = 0;
	memset(m_awVMPBrdBitrate, 0, sizeof(m_awVMPBrdBitrate));

//    m_byRegGKDriId      = 0;
    m_byPrsChnnl        = EQP_CHANNO_INVALID;
    m_byPrsChnnl2       = EQP_CHANNO_INVALID;
    m_byPrsChnnlAud     = EQP_CHANNO_INVALID;
    m_byPrsChnnlAudBas  = EQP_CHANNO_INVALID;
    m_byPrsChnnlVidBas  = EQP_CHANNO_INVALID;
    m_byPrsChnnlBrBas   = EQP_CHANNO_INVALID;

	m_byPrsChnnlVmpOut1 = EQP_CHANNO_INVALID;
	m_byPrsChnnlVmpOut2	= EQP_CHANNO_INVALID;
	m_byPrsChnnlVmpOut3	= EQP_CHANNO_INVALID;
	m_byPrsChnnlVmpOut4	= EQP_CHANNO_INVALID;
	m_byPrsChnnlDsVidBas		= EQP_CHANNO_INVALID;

#ifdef _MINIMCU_
    m_byIsDoubleMediaConf = 0;
#endif

	// ���Ӷ� �����跢������ؼ�֡����ı���, zgc, 2008-04-21
	m_dwVmpLastVCUTick = 0;				
	m_dwVmpTwLastVCUTick = 0;		
	m_dwVidBasChnnlLastVCUTick = 0;	
	m_dwAudBasChnnlLastVCUTick = 0;
	m_dwBrBasChnnlLastVCUTick = 0;
	m_dwCasdAudBasChnnlLastVCUTick = 0;
	m_dwCasdVidBasChnnlLastVCUTick = 0;

	m_wRestoreTimes      = 1;
    m_byConfIdx          = 0;
	m_byDcsIdx			 = 0;
    
    m_byCreateBy         = 0;
	m_byLastDsSrcMtId    = 0;
	m_byMtIdNotInvite    = 0;// xliang [12/26/2008] 
	// ���鷢��������Դ���趨����, zgc, 2008-04-14
	m_bySpeakerAudSrcSpecType = SPEAKER_SRC_NOTSEL;
	m_bySpeakerVidSrcSpecType = SPEAKER_SRC_NOTSEL;

    m_cMtRcvGrp.Clear();
    m_cBasMgr.Clear();
	memset(&m_tLogicChnnl, 0, sizeof(m_tLogicChnnl));
	
	// xliang [12/18/2008] ���vmp channel��Ϣ
	m_tVmpChnnlInfo.clear();
	//memset(m_abyMtVmpChnnl, 0, sizeof(m_abyMtVmpChnnl));
	memset(m_abyMtNeglectedByVmp, 0, sizeof(m_abyMtNeglectedByVmp));

	m_tLastVmpChnnlMemInfo.Init();
	memset(m_atVMPTmpMember,0,sizeof(m_atVMPTmpMember));
	m_bNewMemberSeize = 0;

	//m_tVmpCasMemInfo.Init();
	m_byVmpOperating = 0;
	
    KillTimer(MCUVC_SCHEDULED_CHECK_TIMER);
    KillTimer(MCUVC_ONGOING_CHECK_TIMER);
    KillTimer(MCUVC_INVITE_UNJOINEDMT_TIMER);
    KillTimer(MCUVC_MCUSRC_CHECK_TIMER);
	KillTimer(MCUVC_RECREATE_DATACONF_TIMER);
    KillTimer(MCUVC_CONFSTARTREC_TIMER);

    return;
}

/*====================================================================
    ������      ��ConfLog
    ����        ��ҵ���ӡ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/08    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ConfLog( BOOL32 bFile, s8 * pszFmt, ... )
{
	s8 achPrintBuf[255];

    s32 nBufLen;
    va_list argptr;
	s32 nLen = sprintf( achPrintBuf, "[CONF.%d]:", m_byConfIdx );
    va_start( argptr, pszFmt );    
    nBufLen = vsprintf( achPrintBuf + nLen, pszFmt, argptr );   
    OspPrintf( nLen, bFile, achPrintBuf );
    va_end(argptr); 	
}

/*====================================================================
    ������      ��GetPollParam
    ����        ���õ���һ�����ϱ���ѯ�������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[IN/OUT] u8 &byPollPos ��ǰ/��һ����ѯ���ն�
                  [IN/OUT] TPollInfo& tPollInfo  ��ѯ����Ϣ
    ����ֵ˵��  ��TMtPollParam *������Ҳ�����һ�����򷵻� NULL
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         ������         ����
	07/03/29	4.0			�ܹ��			�޸ģ������ж��Ƿ�����ѯ��BOOL32����
    07/04/04    4.0         ����         ��д
====================================================================*/
TMtPollParam *CMcuVcInst::GetNextMtPolled(u8 &byPollPos, TPollInfo& tPollInfo)
{
    // guzh [4/5/2007] ���ȼ���û��Ƿ�ָ������ѯλ��
    if ( m_tConfPollParam.IsSpecPos() )
    {
        byPollPos = m_tConfPollParam.GetSpecPos();
        m_tConfPollParam.ClearSpecPos();
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
    BOOL32 bSendAudio = FALSE;
    BOOL32 bCasMcuSpeaker = FALSE;
    BOOL32 bVideoLose = FALSE;
    TMtPollParam tCurMtParam, *ptMtPollParam = NULL;
    do 
    {
        // ��һ�ֵ���ѯ��ʼ
        if (byPollPos >= m_tConfPollParam.GetPolledMtNum())
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
        
		ptMtPollParam = m_tConfPollParam.GetPollMtByIdx(byPollPos);
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
            // ����@2006.4.14 ������Ƶ���߼�
            bSendAudio = tMtStatus.IsSendAudio();
            
            // guzh [7/27/2006] �����϶������¼�MCU�����ˣ�һ��Ҫ��ֵ
            bCasMcuSpeaker = FALSE;

            // zbq [03/09/2007] ������ǰû����Ƶ���ն�
            bVideoLose = tMtStatus.IsVideoLose();
			// xliang [4/1/2009] tempararily modified
			//bVideoLose = FALSE;
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

					//zbq[06/30/2008] �����ն�״̬���ֶη�ʵʱ�ϱ�����������list����ʱ�¼�MCU���ն�״̬. ���������¼��ն˴��߼�ͨ������Ӧ������������.
                    //bSendVideo = pMcMtStatus->IsSendVideo();
                    //bSendAudio = pMcMtStatus->IsSendAudio();
					bSendVideo = TRUE;
                    bSendAudio = TRUE;

                    bCasMcuSpeaker = ((m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO) &&
                                      GetLocalSpeaker().GetMtId() == tCurMtParam.GetMcuId());

                    // zbq [03/09/2007] ������ǰû����Ƶ���¼�MCU�ն�
                    bVideoLose = pMcMtStatus->IsVideoLose();
                }
            }
        }

        if ( bJoined &&  
             !bCasMcuSpeaker &&
             !bVideoLose &&
             ( bSendVideo || 
               ( bSendAudio &&
                 m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_SPEAKER ) )
            )
        {
            break;
        }
        else
        {
            McsLog("[GetNextMtPolled] MT(%d,%d) skipped for Joined.%d, SendVideo.%d, Speaker.%d", 
                tCurMtParam.GetMcuId(), 
                tCurMtParam.GetMtId(),
                bJoined, 
                bSendVideo, 
                bCasMcuSpeaker);
        }

        byPollPos++;
        wFindMtNum++;

    // ֱ������һȦ
    }while (wFindMtNum<m_tConfPollParam.GetPolledMtNum());

    if (wFindMtNum == m_tConfPollParam.GetPolledMtNum())
    {
        return NULL;
    }
     
    // ��֤��ǰ��ѯ��Ϣ����ȷ��
    tPollInfo.SetMtPollParam( tCurMtParam );
    TMtPollParam *ptNextMt = m_tConfPollParam.GetPollMtByIdx(byPollPos);    
    return ptNextMt;
}

/*=============================================================================
  �� �� ���� GetMtTWPollParam
  ��    �ܣ� �õ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 &byPollPos
  �� �� ֵ�� TMtPollParam 
=============================================================================*/
TMtPollParam *CMcuVcInst::GetMtTWPollParam(u8 &byPollPos)
{
    u8 wFindMtNum = 0;
    TTvWallPollInfo * ptPollInfo = m_tConf.m_tStatus.GetTvWallPollInfo();    
    
    BOOL32 bJoined = FALSE;
    BOOL32 bSendVideo = FALSE;
    BOOL32 bMcuSpeaker = FALSE;
	TMtPollParam tCurMtParam, *ptMtPollParam = NULL;
    do 
    {
		// �Ӻ�����ͷ�Ƶ������byPollPos���б�����zgc, 2007-04-05
		if (byPollPos == m_tTvWallPollParam.GetPolledMtNum())
		{        
			u32 dwPollNum = ptPollInfo->GetPollNum();
			if (1 != dwPollNum)
			{
				if (0 != dwPollNum)
				{
					ptPollInfo->SetPollNum(dwPollNum - 1);
					m_tConf.m_tStatus.SetTvWallPollInfo(*ptPollInfo);
				}
				byPollPos = 0;
			}
			else
			{
				ptPollInfo->SetPollNum(0);
				m_tConf.m_tStatus.SetTvWallPollInfo(*ptPollInfo);
				return NULL;
			}
		}

        ptMtPollParam = m_tTvWallPollParam.GetPollMtByIdx(byPollPos);
        if ( NULL != ptMtPollParam )
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

            // guzh [7/27/2006] ��������Ҫ����������⣬һ��Ҫ��ֵ
            bMcuSpeaker = FALSE;
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

                    //zbq[2008/08/08] �����ն�״̬���ֶη�ʵʱ�ϱ�����������list����ʱ�¼�MCU���ն�״̬. ���������¼��ն˴��߼�ͨ������Ӧ������������.
                    //bSendVideo = pMcMtStatus->IsSendVideo();
					bSendVideo = TRUE;

                    bMcuSpeaker = GetLocalSpeaker().GetMtId() == tCurMtParam.GetMcuId();
                    
                    if( bJoined && bSendVideo && !bMcuSpeaker )
                    {
                        OnMMcuSetIn((TMt&)tCurMtParam, 0, SWITCH_MODE_SELECT);
                    }
                }
            }
        }
        
        if (bJoined && bSendVideo && !bMcuSpeaker)
        {
            break;
        }
        else
        {
            EqpLog("[GetMtTWPollParam] MT(%d,%d) skipped for Joined.%d, SendVideo.%d, Speaker.%d", 
                tCurMtParam.GetMcuId(), 
                tCurMtParam.GetMtId(),
                bJoined, 
                bSendVideo, 
                bMcuSpeaker);
        }
        
        byPollPos++;

        wFindMtNum++;
        
    }while (wFindMtNum<m_tTvWallPollParam.GetPolledMtNum());
    
    if (wFindMtNum == m_tTvWallPollParam.GetPolledMtNum())
    {
        return NULL;
    }
    
    return m_tTvWallPollParam.GetPollMtByIdx(byPollPos);
}

/*=============================================================================
  �� �� ���� GetMtHduPollParam
  ��    �ܣ� �õ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 &byPollPos
  �� �� ֵ�� TMtPollParam 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  09/03/10    4.6         ���ֱ�         ����
=============================================================================*/
TMtPollParam *CMcuVcInst::GetMtHduPollParam(u8 &byPollPos)
{
    u8 wFindMtNum = 0;
    THduPollInfo * ptPollInfo = m_tConf.m_tStatus.GetHduPollInfo();    
    
    BOOL32 bJoined = FALSE;
    BOOL32 bSendVideo = FALSE;
    BOOL32 bMcuSpeaker = FALSE;
	TMtPollParam tCurMtParam, *ptMtPollParam;
    do 
    {
		// xliang [7/27/2009] ��ɾMt���˳���ѯ������һ���Զ��ԣ�
		//					  ��ѯ����ĩһ������ʱɾ��1��Mt���ᵼ��byPollPos > ��ѯ��Mt��
		//if (byPollPos == m_tHduPollParam.GetPolledMtNum())
		if (!(byPollPos < m_tHduPollParam.GetPolledMtNum()))
		{        
			u32 dwPollNum = ptPollInfo->GetPollNum();	//��ѯ����
			if (1 != dwPollNum)
			{
				if (0 != dwPollNum)
				{
					ptPollInfo->SetPollNum(dwPollNum - 1);
					m_tConf.m_tStatus.SetHduPollInfo(*ptPollInfo);
				}
				byPollPos = 0;
			}
			else
			{
				ptPollInfo->SetPollNum(0);
				m_tConf.m_tStatus.SetHduPollInfo(*ptPollInfo);
				return NULL;
			}
		}

        ptMtPollParam = m_tHduPollParam.GetPollMtByIdx(byPollPos);
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

            // guzh [7/27/2006] ��������Ҫ����������⣬һ��Ҫ��ֵ
            bMcuSpeaker = FALSE;
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

                    //zbq[2008/08/08] �����ն�״̬���ֶη�ʵʱ�ϱ�����������list����ʱ�¼�MCU���ն�״̬. ���������¼��ն˴��߼�ͨ������Ӧ������������.
                    //bSendVideo = pMcMtStatus->IsSendVideo();
					bSendVideo = TRUE;

                    bMcuSpeaker = GetLocalSpeaker().GetMtId() == tCurMtParam.GetMcuId();
                    
                    if( bJoined && bSendVideo && !bMcuSpeaker )
                    {
                        OnMMcuSetIn((TMt&)tCurMtParam, 0, SWITCH_MODE_SELECT);
                    }
                }
            }
        }
        
        if (bJoined && bSendVideo && !bMcuSpeaker)
        {
            break;
        }
        else
        {
            EqpLog("[GetMtTWPollParam] MT(%d,%d) skipped for Joined.%d, SendVideo.%d, Speaker.%d", 
                tCurMtParam.GetMcuId(), 
                tCurMtParam.GetMtId(),
                bJoined, 
                bSendVideo, 
                bMcuSpeaker);
        }
        
        byPollPos++;

        wFindMtNum++;
        
    }while (wFindMtNum<m_tHduPollParam.GetPolledMtNum());
    
    if (wFindMtNum == m_tHduPollParam.GetPolledMtNum())
    {
        return NULL;
    }
    
    return m_tHduPollParam.GetPollMtByIdx(byPollPos);
}

/*====================================================================
    ������      ��VACChange
    ����        �������������Ƹı�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TDiscussParam tDiscussParam ��ǰ��Ա
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         ������         ����
====================================================================*/
void CMcuVcInst::VACChange(const TMixParam &tMixParam, u8 byExciteChn)
{
    TMt tMt;
    TMt tOldSpeaker;

    EqpLog("m_tConf.m_tStatus.GetMixerMode = %d\n",
           m_tConf.m_tStatus.GetMixerMode());

    if (byExciteChn == 0)
    {
        EqpLog( "voice active member num is: %d\n", byExciteChn);
        return;
    }

    tMt = m_ptMtTable->GetMt(byExciteChn);
    tOldSpeaker = GetLocalSpeaker();

    //���Ʒ������л�
    if (m_tConf.m_tStatus.IsVACing() && !(tMt == tOldSpeaker))
    {
        //����ʱ���л����
        ChangeSpeaker(&tMt);
        m_tVacLastSpeaker = tMt;
        EqpLog("Vac speaker change and new speaker is: Mt%d\n", tMt.GetMtId());
    }
    
    return;
}

/*=============================================================================
  �� �� ���� DaemonProcUnitTestMsg
  ��    �ܣ� ��Ԫ������Ϣ����, ����
			 (1) �����ȱ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/2/15    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::DaemonProcUnitTestMsg( CMessage *pcMsg )
{
    /*
	switch( pcMsg->event )
	{
	case EV_TEST_TEMPLATEINFO_GET_REQ:
		ProcUnitTestGetTmpInfoReq(pcMsg);
		break;
	case EV_TEST_CONFINFO_GET_REQ:
		ProcUnitTestGetConfInfoReq(pcMsg);
		break;
	case EV_TEST_CONFMTLIST_GET_REQ:
		ProcUnitTestGetMtListInfoReq(pcMsg);
		break;
	case EV_TEST_ADDRBOOK_GET_REQ:
		ProcUnitTestGetAddrbookReq(pcMsg);
		break;
	case EV_TEST_MCUCONFIG_GET_REQ:
		ProcUnitTestGetMcuCfgReq(pcMsg);
		break;
	default:
		OspPrintf( TRUE, FALSE, "unexpected message %d<%s> received in DaemonProcUnitTestMsg !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
    */
}

/*=============================================================================
  �� �� ���� ProcUnitTestGetTmpInfoReq
  ��    �ܣ� ģ��������ģ����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/2/15    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcUnitTestGetTmpInfoReq( CMessage *pcMsg )
{
    /*
	if ( g_bpUnitMsg )
	{
		OspPrintf( TRUE, FALSE, "[UnitTest]message %d<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
	}
	CConfId cConfId;
	u8		byConfIdx;

	//�ϱ�������Ϣ
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	cConfId = cServMsg.GetConfId();
	byConfIdx = g_cMcuVcApp.GetConfIdx( cConfId );

	// 1. ������Ϣ TConfInfo
	if( byConfIdx != 0 )
	{
		TTemplateInfo tOldTemInfo;
		TConfInfo tConfInfo;
		if(!g_cMcuVcApp.GetTemplate(byConfIdx, tOldTemInfo))
		{
			ConfLog(FALSE, "[ProcUnitTestGetTmpInfoReq] get template confIdx<%d> failed\n", byConfIdx);
			cServMsg.SetEventId( pcMsg->event + 2 );
			OspPost( MAKEIID(AID_MCU_MCSSN, cServMsg.GetSrcSsnId()), cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			return;
        }
		g_cMcuVcApp.TemInfo2Msg( tOldTemInfo, cServMsg );
	}
	cServMsg.SetEventId( pcMsg->event + 1 );
	OspPost( MAKEIID(AID_MCU_MCSSN, cServMsg.GetSrcSsnId()), cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

  */
	return;
}

/*=============================================================================
  �� �� ���� ProcUnitTestGetConfInfoReq
  ��    �ܣ� ģ�������������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/2/15    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcUnitTestGetConfInfoReq( CMessage *pcMsg )
{
    /*
	if ( g_bpUnitMsg )
	{
		OspPrintf( TRUE, FALSE, "[UnitTest]message %d<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
	}
	CConfId cConfId;
	u8		byConfIdx;
	
	//�ϱ�������Ϣ
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	cConfId = cServMsg.GetConfId();
	byConfIdx = g_cMcuVcApp.GetConfIdx( cConfId );
	CMcuVcInst *pInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
	if ( NULL == pInst ) 
	{
		OspPrintf( TRUE, FALSE, "[UnitTest]get mcuvc inst handle failed !\n" );
		return;
	}
	
	// 1. ������Ϣ TConfInfo
	if( byConfIdx != 0 )
	{
		TConfInfo tConfInfo;
		tConfInfo = pInst->m_tConf;
		
		cServMsg.SetEventId( pcMsg->event + 1 );
		cServMsg.SetMsgBody( (u8*)&tConfInfo, sizeof(TConfInfo) );
		OspPost( MAKEIID(AID_MCU_MCSSN, cServMsg.GetSrcSsnId()), cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	}
	*/
	return;
}

/*=============================================================================
  �� �� ���� ProcUnitTestGetMtListInfoReq
  ��    �ܣ� ģ���������ն��б���Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/2/15    4.0			�ű���                  ����
=============================================================================*/
#define MSUNITTEST_BUFLEN		(u16)(1024 * 20)
void CMcuVcInst::ProcUnitTestGetMtListInfoReq( CMessage *pcMsg )
{
    /*
	if ( g_bpUnitMsg )
	{
		OspPrintf( TRUE, FALSE, "[UnitTest]message %d<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
	}
	CConfId cConfId;
	u8		byConfIdx;
	
	//�ϱ�������Ϣ
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	cConfId = cServMsg.GetConfId();
	byConfIdx = g_cMcuVcApp.GetConfIdx( cConfId );

	// 1. �����ն˱� TConfMtTable
	TConfMtTable tConfMtTable;

	u8	byPktNum = sizeof(TConfMtTable) / MSUNITTEST_BUFLEN + 1;

	cServMsg.SetTotalPktNum( byPktNum );	//��Ҫ���͵��ܰ���

	u8 *pMtTable = (u8*)&tConfMtTable;

	if ( g_cMcuVcApp.GetConfMtTable(byConfIdx, &tConfMtTable) )
	{		
		for( s32 nIndex = 0; nIndex < byPktNum; nIndex ++ )
		{
			cServMsg.SetCurPktIdx( nIndex );
			if ( nIndex < byPktNum - 1 )
			{
				cServMsg.SetMsgBody( pMtTable + nIndex * MSUNITTEST_BUFLEN, sizeof(u8) * MSUNITTEST_BUFLEN );
			}
			else
			{
				cServMsg.SetMsgBody( pMtTable + nIndex * MSUNITTEST_BUFLEN, 
										sizeof(tConfMtTable) - sizeof(u8) * MSUNITTEST_BUFLEN * nIndex );
			}
			cServMsg.SetEventId( pcMsg->event + 1 );
			OspPost( MAKEIID(AID_MCU_MCSSN, cServMsg.GetSrcSsnId()), cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			OspDelay(100);
		}
	}
	else
	{
		ConfLog(FALSE, "ProcUnitTestGetMtListInfoReq] get mt table failed !\n");
		return;
	}
    */
}
/*==============================================================================
������    :  DaemonProcHDIFullNtf
����      :  ������HDI����������������MT����MCUʧ��֪ͨ��Ϣ
�㷨ʵ��  :  �ϱ�MCS
����˵��  :  CMessage *pcMsg
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2008-10-24					Ѧ��
==============================================================================*/
void CMcuVcInst::DaemonProcHDIFullNtf ( const CMessage *pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byHdiId = 0;
	byHdiId = cServMsg.GetSrcDriId();
	
	NotifyMcsAlarmInfo( 0, ERR_MCU_MTCALLFAIL_HDIFULL);
	McsLog("[DaemonProcHDIFullNtf]Send HDI FULL NOTIFY to All Mcs:HDI.%u is full\n",byHdiId);
}
/*=============================================================================
  �� �� ���� ProcUnitTestGetMcuCfgReq
  ��    �ܣ� ģ��������MCU������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/2/15    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcUnitTestGetMcuCfgReq( CMessage *pcMsg )
{
    /*
	if ( g_bpUnitMsg )
	{
		OspPrintf( TRUE, FALSE, "[UnitTest]message %d<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
	}
	
	u8 abyCfgBuf[MSUNITTEST_BUFLEN];
	u32 dwBufOut = 0;
	g_cMcuVcApp.GetCfgFileData( (u8*)&abyCfgBuf, sizeof(u8) * MSUNITTEST_BUFLEN, dwBufOut, TRUE );
	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	cServMsg.SetEventId( pcMsg->event + 1 );
	cServMsg.SetMsgBody( abyCfgBuf, sizeof(u8) * dwBufOut );
	
	// �ϱ�����������Ϣ
	OspPost( MAKEIID(AID_MCU_MCSSN, cServMsg.GetSrcSsnId()), cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
    */
}

/*=============================================================================
  �� �� ���� ProcUnitTestGetAddrbookReq
  ��    �ܣ� ģ���������ַ����Ϣ(���ﻹ����ɵ�ַ���ϱ���ֻ�ǻ��˸���Ϣ)
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/2/15    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcUnitTestGetAddrbookReq( CMessage *pcMsg )
{
    /*
	if ( g_bpUnitMsg )
	{
		OspPrintf( TRUE, FALSE, "[UnitTest]message %d<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
	}
	//�ϱ������ַ����Ϣ
    */
}

/*====================================================================
    ������      : IsMtNeedAdapt
    ����        : �ն��Ƿ�������
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u8 byMtId �ն�Id
				  u8 byAdaptType ��������
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    04/02/14    3.0         ������         ����
====================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::IsMtNeedAdapt(u8 byAdaptType, u8 byMtId, TMt *ptSrcMt)
{
    TSimCapSet tBrdSrcSCS;
    TSimCapSet tMtDstSCS;
    
    //����һ��
    if (!m_tConf.GetConfAttrb().IsUseAdapter())
    {
        return FALSE;
    }

    TMt tSrcMt;

    switch(byAdaptType)
    {
    case ADAPT_TYPE_AUD:
    case ADAPT_TYPE_CASDAUD:
        if (NULL == ptSrcMt ||
            (NULL != ptSrcMt && ptSrcMt->IsNull()))
        {
            //�Ƿ���Ҫý����������
            if (TYPE_MT == m_tAudBrdSrc.GetType())
            {
                tSrcMt = m_tAudBrdSrc;
            }
        }
        else
        {
            tSrcMt = *ptSrcMt;
        }

        tMtDstSCS  = m_ptMtTable->GetDstSCS(byMtId);
        //֧��rec��������
        if ( TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetEqpType() )
        {
            tBrdSrcSCS.SetAudioMediaType( m_tPlayEqpAttrib.GetAudioType() );
        }
        else
        {
            tBrdSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());     
        }  
        
        if (tBrdSrcSCS.GetAudioMediaType() != tMtDstSCS.GetAudioMediaType())
        {
            return TRUE;
        }
        break;

    case ADAPT_TYPE_VID:
    case ADAPT_TYPE_CASDVID:
        {
            if (NULL == ptSrcMt ||
                (NULL != ptSrcMt && ptSrcMt->IsNull()))
            {
                //�Ƿ���Ҫý����������
                if (TYPE_MT == m_tVidBrdSrc.GetType())
                {
                    tSrcMt = m_tVidBrdSrc;
                }
            }
            else
            {
                tSrcMt = *ptSrcMt;
            }

            tMtDstSCS  = m_ptMtTable->GetDstSCS(byMtId);
            
            if ( TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetEqpType() )
            {
                tBrdSrcSCS.SetVideoMediaType(m_tPlayEqpAttrib.GetVideoType());
                tBrdSrcSCS.SetVideoResolution(tMtDstSCS.GetVideoResolution());
            }
            else
            {
                tBrdSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());
            }
                               
            if(MEDIA_TYPE_NULL != tBrdSrcSCS.GetVideoMediaType())
            {
                // zbq [08/20/2007] ��������֧��H264��D1�ֱ���
                u8 bySrcType = tBrdSrcSCS.GetVideoMediaType();
                u8 byDstType = tMtDstSCS.GetVideoMediaType();
                u8 bySrcFormat = tBrdSrcSCS.GetVideoResolution();
                u8 byDstFormat = tMtDstSCS.GetVideoResolution();

                if ( bySrcType   != byDstType ||
                     bySrcFormat != byDstFormat )
                {
                    if ( (MEDIA_TYPE_H264 == bySrcType &&
                          VIDEO_FORMAT_4CIF == bySrcFormat)
                        ||
                         (MEDIA_TYPE_H264 == byDstType &&
                          VIDEO_FORMAT_4CIF == byDstFormat)
						//||
						//  ( MEDIA_TYPE_H264 == bySrcType && bySrcType == byDstType
						//  && bySrcFormat < byDstFormat )	
						  )
                    {
						ConfLog(FALSE, "[IsMtNeedAdapt] no need adapter! bySrcType: %u, bySrcFormat: %u, byDstType: %u, byDstFormat: %u\n",
							bySrcType, bySrcFormat, byDstType, byDstFormat);
                    }
                    else
                    {
                        return TRUE;
                    }
                }
                /*
                if ( (tBrdSrcSCS.GetVideoMediaType() != tMtDstSCS.GetVideoMediaType() )
                    || (tBrdSrcSCS.GetVideoResolution() != tMtDstSCS.GetVideoResolution() &&
                        tMtDstSCS.GetVideoResolution() != VIDEO_FORMAT_AUTO &&
                        MEDIA_TYPE_H264 != tMtDstSCS.GetVideoMediaType()) )//  [4/24/2006] 264��ʽû�зֱ���
                {
                    return TRUE;
                }
                */
            }          
        }
        break;

    case ADAPT_TYPE_BR:
		{
			// ���� [6/2/2006] �������˫�ٻ���,��Զ����Ϊ��Ҫ����������
			if (m_tConf.GetSecBitRate() == 0)
			{
				return FALSE;
			}
        
            if (NULL == ptSrcMt ||
                (NULL != ptSrcMt && ptSrcMt->IsNull()))
			{
				//�Ƿ���Ҫý����������
// 				if (TYPE_MT == m_tVidBrdSrc.GetType())
// 				{
					tSrcMt = m_tVidBrdSrc;
// 				}
			}
            else
            {
                tSrcMt = *ptSrcMt;
            }
            
            // xsl [8/25/2006] Դ��Ŀ����ͬ�����������䣬���ж��Ƿ�������������������߼�ͳһ
            if (tSrcMt.GetMtId() == byMtId)
            {
                return FALSE;
            }

            // zbq [08/26/2007] Դ�˵�����Ӧ��ȡ��ǰ�ķ�������
			// u16 wSrcBitrate = m_ptMtTable->GetDialBitrate(tSrcMt.GetMtId());
            u16 wSrcBitrate = m_ptMtTable->GetMtSndBitrate(tSrcMt.GetMtId());

            // zbq [08/20/2007] ������������ͬ��ҪУ����Ƶ��ʽ�ͷֱ���
            tMtDstSCS  = m_ptMtTable->GetDstSCS(byMtId);

			// ¼�����������ǿ��ȡ�����һ����
			if ( TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetEqpType() )
			{
				wSrcBitrate = m_tConf.GetBitRate();

                // zbq [08/20/2007] ������������ͬ��ҪУ����Ƶ��ʽ�ͷֱ���
                tBrdSrcSCS.SetVideoMediaType(m_tPlayEqpAttrib.GetVideoType());
                tBrdSrcSCS.SetVideoResolution(tMtDstSCS.GetVideoResolution());
			}
            else
            {
                // zbq [08/20/2007] ������������ͬ��ҪУ����Ƶ��ʽ�ͷֱ���
                tBrdSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());
            }

			//�Ƿ���Ҫ��������
			//�о� ������Դ�ķ������� ���� ���ն˵Ľ������� ����Ҫ��������   
			if (m_ptMtTable->GetMtReqBitrate(byMtId) > 0 && 
				(m_ptMtTable->GetMtReqBitrate(byMtId)*(g_cMcuVcApp.GetBitrateScale()+100)/100) < wSrcBitrate)
			{
                // zbq [08/20/2007] ��������֧��H264��D1�ֱ���
                u8 bySrcType = tBrdSrcSCS.GetVideoMediaType();
                u8 byDstType = tMtDstSCS.GetVideoMediaType();
                u8 bySrcFormat = tBrdSrcSCS.GetVideoResolution();
                u8 byDstFormat = tMtDstSCS.GetVideoResolution();
                
                if ( (MEDIA_TYPE_H264 == bySrcType &&
                      VIDEO_FORMAT_4CIF == bySrcFormat)
                    ||
                     (MEDIA_TYPE_H264 == byDstType &&
                      VIDEO_FORMAT_4CIF == byDstFormat))
                {
                }
                else
                {
                    return TRUE;
                }
			}
		}
        break;

    default:
        break;
    }

    return FALSE;
}


/*=============================================================================
�� �� ���� IsMtSrcBas
��    �ܣ� �ն�Դ�Ƿ�����bas
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMtId
u8 byMode
�� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcInst::IsMtSrcBas(u8 byMtId, u8 byMode, u8 &byAdpType)
{
    byAdpType = ADAPT_TYPE_NONE;

    TMt tMt = m_ptMtTable->GetMt(byMtId);
    if(tMt.GetType() != TYPE_MT)
    {
        return FALSE;
    }
    
    TMt tMtSrc;
    if(!m_ptMtTable->GetMtSrc(byMtId, &tMtSrc, byMode))
    {
        return FALSE;
    }
    
    if(MODE_VIDEO == byMode)
    {
        // guzh [7/10/2007] ��ʽ��������
        if(m_tConf.m_tStatus.IsVidAdapting() && IsMtNeedAdapt(ADAPT_TYPE_VID, byMtId, &tMtSrc))
        {
            byAdpType = ADAPT_TYPE_VID;
            return TRUE;
        }
        else if(m_tConf.m_tStatus.IsCasdVidAdapting() && IsMtNeedAdapt(ADAPT_TYPE_CASDVID, byMtId, &tMtSrc))
        {
            byAdpType = ADAPT_TYPE_CASDVID;
            return TRUE;
        }
        else if(m_tConf.m_tStatus.IsBrAdapting() && IsMtNeedAdapt(ADAPT_TYPE_BR, byMtId, &tMtSrc))
        {
            byAdpType = ADAPT_TYPE_BR;
            return TRUE;
        }
        else if (m_tConf.m_tStatus.IsHdVidAdapting() &&
			     m_cMtRcvGrp.IsMtNeedAdp(byMtId))
        {
			return TRUE;
        }
		else
        {
            return FALSE;
        }
    }   
    else if(MODE_AUDIO == byMode)
    {
        if(m_tConf.m_tStatus.IsAudAdapting() && IsMtNeedAdapt(ADAPT_TYPE_AUD, byMtId, &tMtSrc))
        {
            byAdpType = ADAPT_TYPE_AUD;
            return TRUE;
        }
        else if(m_tConf.m_tStatus.IsCasdAudAdapting() && IsMtNeedAdapt(ADAPT_TYPE_CASDAUD, byMtId, &tMtSrc))
        {
            byAdpType = ADAPT_TYPE_CASDAUD;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }    
    
    return FALSE;
}


/*=============================================================================
�� �� ���� IsIsMtSrcVmp2
��    �ܣ� �ն�Դ�Ƿ�vmp�ڶ�·����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMtId
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/9  4.0			������                  ����
=============================================================================*/
BOOL32 CMcuVcInst::IsMtSrcVmp2(u8 byMtId) 
{
    if (0 == m_tConf.GetSecBitRate() || 
        MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() ||       
        !(m_tVidBrdSrc == m_tVmpEqp) ||
        m_ptMtTable->GetMtReqBitrate(byMtId) == m_tConf.GetBitRate())
    {
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================
    �� �� ���� StartAdapt
    ��    �ܣ� ��������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8  byAdaptType     ��������
               u16 wBitRate        ��������
               TSimCapSet *pDstSCS ����������������
               TSimCapSet *pSrcSCS ����ǰ��ԭʼ������������
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/03/08  3.6			����                  ����
    2007/01/30  4.0         ����                 8000B��������
=============================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::StartAdapt(u8 byAdaptType, u16 wBitRate, TSimCapSet *pDstSCS, TSimCapSet *pSrcSCS)
{
	u16 wWidth;
    u16 wHight;
	TSimCapSet tSrcSCS;
	TAdaptParam tAdaptParam;
    memset(&tAdaptParam, 0, sizeof(tAdaptParam));
    
    if (ADAPT_TYPE_AUD == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tAudBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

    if (ADAPT_TYPE_VID == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tVidBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

    if (ADAPT_TYPE_BR == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tBrBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

    if (ADAPT_TYPE_CASDAUD == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tCasdAudBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

    if (ADAPT_TYPE_CASDVID == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tCasdVidBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

    // guzh [1/30/2007] 8000B��������
    u16 wError = 0;
    if ( !CMcuPfmLmt::IsBasOprSupported( m_tConf, GetMixMtNumInGrp(), wError ) )
    {
        NotifyMcsAlarmInfo(0, wError);
        return FALSE;
    }

	if (NULL == pDstSCS || pDstSCS->IsNull())
	{
        if (ADAPT_TYPE_AUD == byAdaptType)
        {
		    tAdaptParam.SetVidType(MEDIA_TYPE_NULL);
		    tAdaptParam.SetAudType(m_tConf.GetMainAudioMediaType());
        }
        else
        {
            tAdaptParam.SetVidType(m_tConf.GetMainVideoMediaType());
            u8 byResolution = m_tConf.GetMainVideoFormat();
            m_tConf.GetVideoScale(m_tConf.GetMainVideoMediaType(), 
                                wWidth, wHight, &byResolution);
            tAdaptParam.SetResolution(wWidth, wHight);
		    tAdaptParam.SetAudType(MEDIA_TYPE_NULL);
        }
	}
	else
	{
		tAdaptParam.SetVidType(pDstSCS->GetVideoMediaType());	
		tAdaptParam.SetAudType(pDstSCS->GetAudioMediaType());
        u8 byResolution = pDstSCS->GetVideoResolution();
        m_tConf.GetVideoScale(pDstSCS->GetVideoMediaType(), wWidth, wHight, &byResolution);
        tAdaptParam.SetResolution(wWidth, wHight);
	}

	if (NULL == pSrcSCS || pSrcSCS->IsNull())
	{
		if (NULL == pDstSCS || pDstSCS->IsNull())
		{
			tSrcSCS.SetVideoMediaType(tAdaptParam.GetVidType());
			tSrcSCS.SetAudioMediaType(tAdaptParam.GetAudType());
		}
		else
		{
			tSrcSCS = *pDstSCS;
		}
	}
	else
	{
		tSrcSCS = *pSrcSCS;
	}

    if (ADAPT_TYPE_AUD != byAdaptType && ADAPT_TYPE_CASDAUD != byAdaptType)
    {
        if (0 != m_tConf.GetSecBitRate())//˫�ٻ���
        {
            // �����ڵ���¼�ƻ��߹㲥��ֱ�ӵ���Ϊ�ڶ��٣�����˫�ٻ����������٣�
            if ((m_tConf.GetConfAttrb().IsMulticastMode() && 
                 m_tConf.GetConfAttrb().IsMulcastLowStream()) || 
                 m_tRecPara.IsRecLowStream())
            {
                wBitRate = m_tConf.GetSecBitRate();
            }
            // �����������������ʣ������ʸ��ڻ���ڵڶ��٣�
            else
            {
                wBitRate = (wBitRate > m_tConf.GetSecBitRate()) ? wBitRate : m_tConf.GetSecBitRate();
            }
        }

        if (0 == wBitRate)
        {
            tAdaptParam.SetBitRate(m_tConf.GetBitRate());
        }
        else
        {
            if (wBitRate != m_tConf.GetBitRate())
            {
                tAdaptParam.SetBitRate(wBitRate);
            }
            else
            {
                tAdaptParam.SetBitRate(wBitRate);
            }
        }
        m_tConf.GetVideoScale(tAdaptParam.GetVidType(), wWidth, wHight);
        tAdaptParam.SetResolution(wWidth, wHight);

        if (ADAPT_TYPE_VID == byAdaptType)
        {
            m_wVidBasBitrate = wBitRate;            
        }
        else if (ADAPT_TYPE_BR == byAdaptType)
        {
            m_wBasBitrate = wBitRate;
        }
        ConfLog(FALSE, "Request start adapt(type.%d) and bitrate is:%u\n", byAdaptType, wBitRate);
    }
    else
    {
        tAdaptParam.SetBitRate(GetAudioBitrate(tAdaptParam.GetAudType()));
    }

	return StartBasAdapt(&tAdaptParam, byAdaptType, &tSrcSCS);
}

/*=============================================================================
    �� �� ���� ChangeAdapt
    ��    �ܣ� �ı�����(Ŀǰû�жԼ�������֧��)
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u16 wBitRate        ��������
               TSimCapSet *pDstSCS ����������������
               TSimCapSet *pSrcSCS ����ǰ��ԭʼ������������
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/3/9    3.5			����                  ����
=============================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::ChangeAdapt(u8 byAdaptType, u16 wBitRate, TSimCapSet *pDstSCS, TSimCapSet *pSrcSCS)
{
    u16 wWidth;
    u16 wHight;
	TSimCapSet tSrcSCS;
	TAdaptParam tAdaptParam;
    memset(&tAdaptParam, 0, sizeof(tAdaptParam));

    if (ADAPT_TYPE_AUD == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tAudBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

    if (ADAPT_TYPE_VID == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tVidBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

    if (ADAPT_TYPE_BR == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tBrBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

	if (NULL == pDstSCS || pDstSCS->IsNull())
	{
        if (ADAPT_TYPE_AUD == byAdaptType)
        {
		    tAdaptParam.SetVidType(MEDIA_TYPE_NULL);
		    tAdaptParam.SetAudType(m_tConf.GetMainAudioMediaType());
        }
        else
        {
            tAdaptParam.SetVidType(m_tConf.GetMainVideoMediaType());
		    tAdaptParam.SetAudType(MEDIA_TYPE_NULL);
            m_tConf.GetVideoScale(byAdaptType, wWidth, wHight);
            tAdaptParam.SetResolution(wWidth, wHight);
        }
	}
	else
	{
		tAdaptParam.SetVidType(pDstSCS->GetVideoMediaType());	
		tAdaptParam.SetAudType(pDstSCS->GetAudioMediaType());
        u8 byResolution = pDstSCS->GetVideoResolution();
        m_tConf.GetVideoScale(byAdaptType, wWidth, wHight, &byResolution);
        tAdaptParam.SetResolution(wWidth, wHight);
	}

    if (ADAPT_TYPE_AUD != byAdaptType && ADAPT_TYPE_CASDAUD != byAdaptType)
    {
//        if (0 != m_tConf.GetSecBitRate()) //˫�ٻ���
//        {
//            if ((m_tConf.GetConfAttrb().IsMulticastMode() && 
//                m_tConf.GetConfAttrb().IsMulcastLowStream()) || 
//                m_byRecLowStream)
//            {
//                wBitRate = m_tConf.GetSecBitRate();
//            }
//            else
//            {
//                wBitRate = (wBitRate > m_tConf.GetSecBitRate()) ? wBitRate : m_tConf.GetSecBitRate();
//            }
//        }

        if (0 == wBitRate)
        {
            tAdaptParam.SetBitRate(m_tConf.GetBitRate());
        }
        else
        {
            if (wBitRate != m_tConf.GetBitRate())
            {
                tAdaptParam.SetBitRate(wBitRate);
            }
            else
            {
                tAdaptParam.SetBitRate(wBitRate);
            }
        }
        m_tConf.GetVideoScale(tAdaptParam.GetVidType(), wWidth, wHight);
        tAdaptParam.SetResolution(wWidth, wHight);

        if (ADAPT_TYPE_VID == byAdaptType)
        {
            m_wVidBasBitrate = wBitRate;
        }
        else if (ADAPT_TYPE_BR == byAdaptType)
        {
            m_wBasBitrate = wBitRate;
        }

        ConfLog(TRUE, "Change adapt(type.%d) param and bitrate is:%u\n", byAdaptType, wBitRate);
    }
    else
    {
        tAdaptParam.SetBitRate(GetAudioBitrate(tAdaptParam.GetAudType()));
    }

	return ChangeBasAdaptParam(&tAdaptParam, byAdaptType, pSrcSCS);	

}

/*====================================================================
    ������      ��SendTrapMsg
    ����        ���� Trap ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/29    3.0         ������          ����
====================================================================*/
void CMcuVcInst::SendTrapMsg( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
    post( MAKEIID( AID_MCU_AGENT, 1 ), wEvent, pbyMsg, wLen );

	return;
}

/*====================================================================
    ������      ��ShowMtStat
    ����        ����ӡ�����ն�״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/11    3.0         ������          ����
====================================================================*/
void CMcuVcInst::ShowMtStat( void )
{
	if( !m_tConf.m_tStatus.IsOngoing() )
	{
		return;
	}

	//��ӡ�����ն�״̬
	TMtStatus   tMtStatus;
	TCapSupport tCapSupport;
	OspPrintf( TRUE, FALSE, "\n�����ն�״̬: McuId-%d\n", LOCAL_MCUID );
	for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
		{
			m_ptMtTable->GetMtStatus( byLoop, &tMtStatus );
			m_ptMtTable->GetMtCapSupport( byLoop, &tCapSupport );
			tMtStatus.Print();
			tCapSupport.Print();
		}
	}

	//��ӡ����Mc���ն�״̬
	for( u8 byLoop1 = 0; byLoop1 < MAXNUM_SUB_MCU; byLoop1++ )
	{
		u8 byMcuId = m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop1].m_byMcuId;
		if( byMcuId == 0  )
		{
			continue;
		}
		
		OspPrintf( TRUE, FALSE, "\n�����ն�״̬: McuId-%d\n", byMcuId );
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( byMcuId );
		for( u8 byIdx = 1; byIdx <= MAXNUM_CONF_MT; byIdx++ )
		{	
            if ( !ptConfMcInfo->m_atMtStatus[byIdx-1].IsNull() )
            {
                OspPrintf(TRUE, FALSE, "Postion Index: %d\t", byIdx-1 );
                ptConfMcInfo->m_atMtStatus[byIdx-1].Print();
            }			
		}
	}

	return;
}

/*====================================================================
    ������      ��ProcMpMessage
    ����        ������Mp����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/10    3.0         ������          ����
====================================================================*/
void CMcuVcInst::ProcMpMessage( CMessage * const pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TSwitchChannel *ptSwitchChannel;
	CMcuVcInst *pMcuVcInst = NULL;
    u8 byMpId = cServMsg.GetSrcDriId();
	
	if( CurState() != STATE_ONGOING )
	{
		return;
	}
	
	switch( pcMsg->event )
	{
	case MP_MCU_REG_REQ://Mpע��
        {
            // xsl [9/12/2006] ֪ͨmp���Ƿ��һ����
            TMp tMp = *(TMp*)cServMsg.GetMsgBody();
            u8 m_byUniformMode = m_tConf.GetConfAttrb().IsAdjustUniformPack();
			cServMsg.SetMsgBody((u8 *)&m_byUniformMode, sizeof(u8));
            cServMsg.SetConfIdx(m_byConfIdx);
			g_cMpSsnApp.SendMsgToMpSsn(tMp.GetMpId(), MCU_MP_CONFUNIFORMMODE_NOTIFY, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        
            // xsl [9/15/2006]�����Ž���
            AdjustSwitchBridge(tMp.GetMpId(), TRUE);
        }
		break;

	case MP_MCU_ADDSWITCH_ACK://�ɹ����ӽ���
	    ptSwitchChannel = (TSwitchChannel*)cServMsg.GetMsgBody();
	    m_ptSwitchTable->AddSwitchTable( cServMsg.GetSrcDriId(), ptSwitchChannel );	  
	    StartMtSendbyNeed( ptSwitchChannel );
		NotifyRecStartPlaybyNeed( ptSwitchChannel );
		break;

	case MP_MCU_REMOVESWITCH_ACK://�ɹ��Ƴ�����
	    ptSwitchChannel = (TSwitchChannel*)cServMsg.GetMsgBody();
        MpManagerLog( "[ProcMpMessage] MP_MCU_REMOVESWITCH_ACK : dst<%s@%d>\n", 
                       StrOfIP( ptSwitchChannel->GetDstIP() ), ptSwitchChannel->GetDstPort() );
	    m_ptSwitchTable->RemoveSwitchTable( cServMsg.GetSrcDriId(), ptSwitchChannel );
        // zgc, 2008-05-27, �ǹ㲥����Ŀ�Ĳ���Ҫ����Ƿ�Ҫͣ�㲥
        if ( ptSwitchChannel->GetRcvPort() != 0 )
        {
            StopMtSendbyNeed( ptSwitchChannel );
        }
		break;
	case MP_MCU_ADDMULTITOONESWITCH_ACK:		//MP����MCU�����Ӷ�㵽һ�㽻������
	    ptSwitchChannel = (TSwitchChannel*)cServMsg.GetMsgBody();
	    m_ptSwitchTable->ProcMultiToOneSwitch( ptSwitchChannel, 1 );
		break;
	case MP_MCU_REMOVEMULTITOONESWITCH_ACK:		//MP����MCU���Ƴ���㵽һ�㽻������
	    ptSwitchChannel = (TSwitchChannel*)cServMsg.GetMsgBody();
	    m_ptSwitchTable->ProcMultiToOneSwitch( ptSwitchChannel, 2 );
		break;
	case MP_MCU_STOPMULTITOONESWITCH_ACK:       //MP����MCU��ֹͣ��㵽һ�㽻������ 
	    ptSwitchChannel = (TSwitchChannel*)cServMsg.GetMsgBody();
	    m_ptSwitchTable->ProcMultiToOneSwitch( ptSwitchChannel, 3 );
		break;
		
	case MP_MCU_GETSWITCHSTATUS_ACK:
	case MP_MCU_ADDRECVONLYSWITCH_ACK:
	case MP_MCU_REMOVERECVONLYSWITCH_ACK:
	case MP_MCU_SETRECVSWITCHSSRC_ACK:
		break;	

	case MP_MCU_ADDSWITCH_NACK:
	case MP_MCU_ADDMULTITOONESWITCH_NACK:
		{
            // guzh [1/11/2007] ��ӽ���ʧ��
			ConfLog(FALSE, "Fail to Add switch. Mp.%d is full\n", cServMsg.GetSrcDriId());
		}
		break;
	case MP_MCU_REMOVESWITCH_NACK:
		{
            ConfLog(FALSE, "Fail to Remove switch at Mp.%d\n", cServMsg.GetSrcDriId());
		}
		break;
	case MP_MCU_GETSWITCHSTATUS_NACK:	
	case MP_MCU_REMOVEMULTITOONESWITCH_NACK:
	case MP_MCU_STOPMULTITOONESWITCH_NACK:
	case MP_MCU_ADDRECVONLYSWITCH_NACK:
	case MP_MCU_REMOVERECVONLYSWITCH_NACK:
	case MP_MCU_SETRECVSWITCHSSRC_NACK:	
		break;	
        
        // guzh [3/26/2007]�ɹ����ӹ㲥����
    case MP_MCU_ADDBRDSRCSWITCH_ACK:
        ptSwitchChannel = (TSwitchChannel*)cServMsg.GetMsgBody();       
        m_ptSwitchTable->ProcBrdSwitch( 1, ptSwitchChannel, 1, byMpId );       
        StartMtSendbyNeed( ptSwitchChannel );   
        break;
    case MP_MCU_ADDBRDDSTSWITCH_ACK:
        {
        /*
        TSwitchChannel atChnnels[MAXNUM_CONF_MT];            
        u8 byChnlNum = cServMsg.GetMsgBodyLen() / (sizeof(TSwitchChannel)+3*sizeof(u8));
        u8 *byPtr = cServMsg.GetMsgBody();
        for ( u8 byLoop = 0; byLoop < byChnlNum; byLoop++ )
        {
        atChnnels[byLoop] = *(TSwitchChannel*)byPtr;
        byPtr += (sizeof(TSwitchChannel)+3*sizeof(u8));
        }
            */
            TSwitchChannel *ptChnnel = (TSwitchChannel*)cServMsg.GetMsgBody();
            u8 byChnlNum = cServMsg.GetMsgBodyLen() / sizeof(TSwitchChannel);
            
            MpManagerLog("[ProcMpMessage] Mp Added broadcast destination MT n=%d!\n", byChnlNum);
            m_ptSwitchTable->ProcBrdSwitch( byChnlNum, ptChnnel, 11, byMpId );        
            break;
        }
        // �ɹ��Ƴ��㲥Դ
    case MP_MCU_REMOVEBRDSRCSWITCH_ACK:
        ptSwitchChannel = (TSwitchChannel*)cServMsg.GetMsgBody();
        m_ptSwitchTable->ProcBrdSwitch( 1, ptSwitchChannel, 2, byMpId );
        // guzh [4/3/2007] ������ն�,���ﻹ���ƽ���.�� MP_MCU_BRDSRCSWITCHCHANGE_NTF
        break;
        
    case MP_MCU_BRDSRCSWITCHCHANGE_NTF:    // �㲥Դ����Ƴ�֪ͨ
        ProcVidBrdSrcChanged(pcMsg);
        break;
        
    case MP_MCU_ADDBRDSRCSWITCH_NACK:
    case MP_MCU_REMOVEBRDSRCSWITCH_NACK:
    case MP_MCU_ADDBRDDSTSWITCH_NACK:
        ConfLog( FALSE, "MP switch failed. %d(%s)!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ) );
        break;
        
    default:
        ConfLog( FALSE, "Wrong message %u(%s) received in DaemonEntry!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ) );
        break;	
    }
    
    return;
}


/*====================================================================
������      ��ProcVidBrdSrcChanged
����        ������MP֪ͨ��Ƶ�㲥Դ�ѱ仯��֪ͨ
�㷨ʵ��    ��1. ֹͣ Mt-->Mp ����������
2. ֹͣ�Ž���(���ܰ����ڶ�·)
3. etc...
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg, �������Ϣ
����ֵ˵��  ����
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
07/03/27    4.0         ����          ����
====================================================================*/
void CMcuVcInst::ProcVidBrdSrcChanged( CMessage * const pcMsg )
{    
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    TMt tOldVidSrc = *(TMt*)cServMsg.GetMsgBody();
    u8 bySrcDriId = cServMsg.GetSrcDriId();
    
    if (tOldVidSrc.IsNull())
        return;
    
	// xliang [2/6/2009] �ж�֮ǰ�Ƿ�����VMP�㲥
	BOOL32 bNewVmpBrd = FALSE;
	if( tOldVidSrc == m_tVmpEqp )
	{
		TPeriEqpStatus tPeriEqpStatus; 
		g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
		u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
		if(byVmpSubType != VMP)
		{
			bNewVmpBrd = TRUE;
		}
	}
    TSwitchChannel tSwitchChannel;
    u32 dwMtSwitchIp;
    u16 wMtSwitchPort;
    u32 dwMtSrcIp;    
    g_cMpManager.GetSwitchInfo(tOldVidSrc,dwMtSwitchIp, wMtSwitchPort, dwMtSrcIp );
    // ֻ�������Խ���Դ��MP��Ϣ
    if ( bySrcDriId != g_cMcuVcApp.FindMp( dwMtSwitchIp ) )
    {
        return;
    }
    
    TLogicalChannel tLogicalChannel;
    
    //�������䲢�ҿ�����˫��, �ָ��ϵĵ�һ·����Դ�ķ�������,û�������������FlowControl����
    if ( !m_tDoubleStreamSrc.IsNull() &&
        TYPE_MT == tOldVidSrc.GetType() )
    {        
        if (m_ptMtTable->GetMtLogicChnnl(tOldVidSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
        {
            tLogicalChannel.SetFlowControl(m_ptMtTable->GetSndBandWidth( tOldVidSrc.GetMtId()));
            cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(tOldVidSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
        }
    } 
    
    u8 bySrcChnnl = 0;

    // zbq [08/23/2007] �鿴���ն��Ƿ��ڻش�ͨ���������ͣ���Ž���������ᵼ�»ش�ͨ��������
    if ( !m_tCascadeMMCU.IsNull() &&
         m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId())->m_tSpyMt == m_tVidBrdSrc )
    {
    }
    else
    {
        // �Ϲ㲥Դ���ն�������ͨ�������ڣ��򲻲���, zgc, 2008-05-23
        if ( tOldVidSrc.GetType() == TYPE_MCUPERI ||
            !m_ptSwitchTable->IsValidCommonSwitchSrcIp(dwMtSrcIp, wMtSwitchPort, MODE_VIDEO) )
        {
            if(bNewVmpBrd) // xliang [2/6/2009] ����VMP����Ҫ��3·������(�ڽ��ú���֮ǰ�Ͳ��)
			{
// 				bySrcChnnl = 0;
// 				g_cMpManager.RemoveSwitchBridge(tOldVidSrc, bySrcChnnl, MODE_VIDEO);
// 				g_cMpManager.RemoveSwitchBridge(tOldVidSrc, bySrcChnnl+1, MODE_VIDEO);
// 				g_cMpManager.RemoveSwitchBridge(tOldVidSrc, bySrcChnnl+2, MODE_VIDEO);
// 				CallLog("New VMP : RemoveSwitchBridge!\n");
			}
			else
			{
            // ���������
            bySrcChnnl = (tOldVidSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
            g_cMpManager.RemoveSwitchBridge(tOldVidSrc, bySrcChnnl, MODE_VIDEO);
			}
        }
    }
    
    //  xsl [3/10/2006] ˫�ٻ�˫��ʽ����㲥ԴΪvmpʱ������ڶ�·������
	// xliang [2/18/2009] mpu-vmp���������
    if (tOldVidSrc == m_tVmpEqp 
		&& !bNewVmpBrd 
		&& ((0 != m_tConf.GetSecBitRate() || MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType())))
    {
        // VMP����������ͨ���������ﲻ�����жϣ�zgc, 2008-05-23
        g_cMpManager.RemoveSwitchBridge(tOldVidSrc, bySrcChnnl+1, MODE_VIDEO);           
    }	
    
    // ֹͣ����Դ����                  
    tSwitchChannel.SetSrcMt( tOldVidSrc );        
    tSwitchChannel.SetSrcIp(dwMtSrcIp);
    tSwitchChannel.SetRcvIP(dwMtSwitchIp);
    tSwitchChannel.SetRcvPort(wMtSwitchPort);
    
    MpManagerLog("[ProcVidBrdSrcChanged]Recvied MT(%d,%d) %s switch removed notify.\n",
        tOldVidSrc.GetMcuId(), tOldVidSrc.GetMtId(), StrOfIP(dwMtSrcIp));
    
    m_ptSwitchTable->ProcBrdSwitch( 1, &tSwitchChannel, 2, bySrcDriId );
	if (TYPE_MT == tOldVidSrc.GetType())
    {    
        StopMtSendbyNeed(&tSwitchChannel);
    }    
}


/*====================================================================
    ������      ��DaemonProcMpFluxNotify
    ����        ��MP�������������Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/02/11    4.0         �ű���          ����
====================================================================*/
void CMcuVcInst::DaemonProcMpFluxNotify( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    switch( pcMsg->event )
    {
    case MP_MCU_FLUXSTATUS_NOTIFY:
        {
            u16 wMpBand = *(u16*)cServMsg.GetMsgBody();
            g_cMcuVcApp.m_atMpData[cServMsg.GetSrcDriId()-1].m_wNetBandReal = ntohs(*(u16*)cServMsg.GetMsgBody());
            if ( ntohs(*(u16*)cServMsg.GetMsgBody()) > g_cMcuVcApp.m_atMpData[cServMsg.GetSrcDriId()-1].m_wNetBandAllowed )
            {
                u32 dwMpIp = g_cMcuVcApp.m_atMpData[cServMsg.GetSrcDriId()-1].m_tMp.GetIpAddr();
                dwMpIp = htonl(dwMpIp);
                cServMsg.SetMsgBody( (u8*)&dwMpIp, sizeof(u32) );
                SendMsgToAllMcs( MCU_MCS_MPFLUXOVERRUN_NOTIFY, cServMsg );
                break;                        
            }
            // MpManagerLog("[DaemonProcMpFluxNotify] MP_MCU_FLUXSTATUS_NOTIFY recieved, Mp.%d's Band = %d !\n", 
            //         cServMsg.GetSrcDriId(), wMpBand );
            break;
        }
    default:
        OspPrintf( TRUE, FALSE, "[DaemonProcMpFluxNotify] unexpected msg.%d<%s> recieved !\n",
                                 pcMsg->event, OspEventDesc(pcMsg->event) );
        break;
    }
}


/*====================================================================
    ������      ��ProcMpDissconnected
    ����        ������Mp�Ķ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/06/08    3.0         ������          ����
====================================================================*/
void CMcuVcInst::ProcMpDissconnected( CMessage * const pcMsg )
{	
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMp tMp;
	TMt tMt;
	u8 byLoop;

 	switch( CurState() )
	{
	case STATE_ONGOING:

		//֪ͨ����ת������ն˵���
		tMp = *(TMp*)cServMsg.GetMsgBody();	 
		for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			if( m_tConfAllMtInfo.MtInConf( byLoop ) )
			{
				if( tMp.GetMpId() == m_ptMtTable->GetMpId( byLoop ) )
				{
					tMt = m_ptMtTable->GetMt( byLoop );
					RemoveJoinedMt( tMt, TRUE, MTLEFT_REASON_EXCEPT );
				}
			}
		}
        
        // xsl [9/15/2006]�����Ž���
        AdjustSwitchBridge(tMp.GetMpId(), FALSE);
		
		ConfLog( FALSE, "Mp %d disconnected, some mt droped!\n", tMp.GetMpId() );
		
        // xsl [11/14/2006] �����mp������¼
        m_ptSwitchTable->ClearSwitchTable(tMp.GetMpId());

        // zbq [02/14/2007] �����mp��Ӧ��data��Ϣ
        g_cMcuVcApp.m_atMpData[tMp.GetMpId()-1].SetNull();

		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*====================================================================
    ������      ��ProcMtAdpDissconnected
    ����        ������MtAdp�Ķ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/06/08    3.0         ������          ����
====================================================================*/
void CMcuVcInst::ProcMtAdpDissconnected( CMessage * const pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMtAdpReg tMtAdpReg;
	TMt tMt;
	u8 byLoop;

	//�������ն�
 	switch( CurState() )
	{
	case STATE_ONGOING:

		//֪ͨ�����������ն˵���
		tMtAdpReg =  *( TMtAdpReg* )cServMsg.GetMsgBody();
		for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			if( m_tConfAllMtInfo.MtInConf( byLoop ) )
			{
				tMt = m_ptMtTable->GetMt( byLoop );
				if( tMtAdpReg.GetDriId() == tMt.GetDriId() )
				{
					RemoveJoinedMt( tMt, FALSE, MTLEFT_REASON_EXCEPT );                    
				}
			}
		}

		ConfLog( FALSE, "Mtadp %d disconnected, some mt droped!\n", tMtAdpReg.GetDriId() );
		
		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}  

	return;
}

/*====================================================================
    ������      : GetVmpDynStyle
    ����        : �õ���̬�����Ļ���ϳɷ�ʽ
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: byMtNum �ն�����
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/31    3.0         ������        ����
    05/04/13    3.6         libo          ���������������̬�������߼�
    07/01/30    4.0         ����        8000B��������
	09/04/21    4.6			Ѧ��		  ������VMP���е���
====================================================================*/
u8 CMcuVcInst::GetVmpDynStyle( u8 byMtNum )
{
#ifdef _MINIMCU_
   
    // guzh [1/30/2007] ��������
    u16 wError = 0;
    // guzh [8/1/2007] ���ݴ�����ն������㣬���ǵ�ǰ���������ն�
    u8 byVmpCapStyle = CMcuPfmLmt::GetMaxDynVmpStyle(m_tConf, byMtNum, GetMixMtNumInGrp(), wError);

    return byVmpCapStyle;
    
#else   // 8000
    TPeriEqpStatus tPeriEqpStatus;
    g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
    u8 byEqpMaxChl = tPeriEqpStatus.m_tStatus.tVmp.m_byChlNum;
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;// xliang [2/13/2009] 

    if ( byEqpMaxChl == 0 || !ISTRUE(tPeriEqpStatus.m_byOnline) )
    {
        return VMP_STYLE_NONE;
    }

    // ��ȡ��������ķ��
    u8 byVmpCapChlNum = CMcuPfmLmt::GetMaxCapVMPByConfInfo(m_tConf);
    if (byVmpCapChlNum == 0)
    {
        return VMP_STYLE_NONE;
    } 

	// xliang [2/12/2009] ����VMP
	if( MPU_SVMP == byVmpSubType 
		|| MPU_DVMP == byVmpSubType
		|| EVPU_SVMP == byVmpSubType
		|| EVPU_DVMP == byVmpSubType
		)
	{
		if ( byMtNum > MAXNUM_MPUSVMP_MEMBER ) 
		{
			byMtNum = MAXNUM_MPUSVMP_MEMBER;
		}
	}
	else
	{
		if ( byMtNum > MAXNUM_SDVMP_MEMBER) 
		{
			byMtNum = MAXNUM_SDVMP_MEMBER;
		}
	}

    u8 byVmpStyle;
    switch(byMtNum)
	{
	case 0:
	case 1:
		byVmpStyle = VMP_STYLE_ONE;
		break;
	case 2:
		byVmpStyle = VMP_STYLE_VTWO;
		break;
	case 3:
		byVmpStyle = VMP_STYLE_THREE;
		break;
	case 4:
		byVmpStyle = VMP_STYLE_FOUR;
		break;
	case 5:
	case 6:
		byVmpStyle = VMP_STYLE_SIX;
		break;
	case 7:
		byVmpStyle = VMP_STYLE_SEVEN;
		break;
	case 8:
		byVmpStyle = VMP_STYLE_EIGHT;
		break;
	case 9:
		byVmpStyle = VMP_STYLE_NINE;
		break;
	case 10:
		byVmpStyle = VMP_STYLE_TEN;
		break;
	case 11:
	case 12:
	case 13:
		byVmpStyle = VMP_STYLE_THIRTEEN;
		break;
	case 14:
	case 15:
	case 16:
		byVmpStyle = VMP_STYLE_SIXTEEN;
		break;
	case 17:
	case 18:
	case 19:
	case 20:
		// xliang [2/13/2009] switch caseǰ��MtNum���ˣ����Դ˴���������vmp����
		byVmpStyle = VMP_STYLE_TWENTY;
		break;
	default:
		byVmpStyle = VMP_STYLE_NONE;
		break;
	}
    u8 byCurChlNum = GetVmpChlNumByStyle( byVmpStyle );	 //��ǰ�������Ҫ��ͨ����
    
    if (byCurChlNum > byVmpCapChlNum || byCurChlNum > byEqpMaxChl)
    {
        byVmpStyle = GetVmpDynStyle(byMtNum - 1);
    }    

	if ( byVmpStyle == VMP_STYLE_NONE )
	{
		return VMP_STYLE_NONE;
	}
    
	// ���� ������������Ҫ�� VMP֧�ֵ� �ϳɷ��, zgc, 2008-03-03	
	TPeriEqpStatus atEqpStatus[MAXNUM_PERIEQP];
	memset( atEqpStatus, 0, sizeof(atEqpStatus) );
	u8 byIdx = 0;
	u8 byPeriIdLoop = VMPID_MIN;
    u8 byPeriIdMax = VMPID_MAX;
	
	while( byPeriIdLoop <= byPeriIdMax && byIdx < MAXNUM_PERIEQP )
	{
		if( g_cMcuVcApp.IsPeriEqpValid( byPeriIdLoop ) )
		{
			g_cMcuVcApp.GetPeriEqpStatus( byPeriIdLoop, atEqpStatus+byIdx );
			byIdx++;
		}
		byPeriIdLoop++;
	}
    u8 byValidVmpNum = byIdx;

	u8 abyStyleArray[32];
    u8 bySize = 32;
	u8 byTempSize = bySize;
	CMcuPfmLmt::GetSupportedVmpStyles( m_tConf, m_byConfIdx, atEqpStatus, byValidVmpNum, abyStyleArray, bySize );

	if ( bySize <= byTempSize && bySize > 0 )
	{
		u8 byLop = 0;
		for ( byLop = 0; byLop < bySize; byLop++ )
		{
			// ����MT������ķ����Ա�VMP֧�֣���ֱ�ӷ���
			if ( abyStyleArray[byLop] == byVmpStyle )
			{
				return byVmpStyle;
			}
		}

		// ����֧�֣��򷵻�֧�ֵ������
		return abyStyleArray[bySize-1];
	}
	else
	{
		return VMP_STYLE_NONE;
	}

    return byVmpStyle;

#endif   
}

/*=============================================================================
�� �� ���� IsVMPSupportVmpStyle
��    �ܣ� ָ���Ļ���ϳ����Ƿ�֧��ĳ����ϳɷ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byStyle
           u8 byEqpId: �жϵ�vmpId
           u16 &wError ���صĴ�����
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/31  4.0			������                  ����
2006/5/09  4.0          ����                  �����豸����
=============================================================================*/
BOOL32 CMcuVcInst::IsVMPSupportVmpStyle(u8 byStyle, u8 byEqpId, u16 &wError)
{
#ifndef _MINIMCU_
	// 8000
    TPeriEqpStatus tPeriEqpStatus;  
    g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tPeriEqpStatus);

	if ( !CMcuPfmLmt::IsVmpStyleSupport( m_tConf, m_byConfIdx, 
							       		 &tPeriEqpStatus, 1, byStyle, 
									     byEqpId ) )
    {
		wError = ERR_INVALID_VMPSTYLE;
		return FALSE;
    }

	return TRUE;
	
#else
	// guzh [1/30/2007] ����8000B�����ܱ���
    if ( !CMcuPfmLmt::IsVmpOprSupported( m_tConf, byStyle, m_tConfAllMtInfo.GetLocalJoinedMtNum(), GetMixMtNumInGrp(), wError )  )
    {
        ConfLog( FALSE, "[IsVMPSupportVmpStyle] Eqp.%d: IsVmpOprSupported return false. Error=%d !\n", 
                        byEqpId, wError );
        return FALSE;
    }
    return TRUE;
#endif
}

/*=============================================================================
�� �� ���� IsMCUSupportVmpStyle
��    �ܣ� ����MCU�Ƿ�֧��ĳ����ϳɷ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byStyle
           u8 byEqpId: ֧�ָ÷���VMPID
           u8 byEqpType
           u16 &wError ���صĴ�����
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/12/27  4.0         �ű���                ����
=============================================================================*/
BOOL32 CMcuVcInst::IsMCUSupportVmpStyle( u8 byStyle, u8 &byEqpId, u8 byEqpType, u16 &wError )
{
#ifndef _MINIMCU_
    // 8000
    byEqpId = 0;
	
	TPeriEqpStatus atEqpStatus[MAXNUM_PERIEQP];
	memset( atEqpStatus, 0, sizeof(atEqpStatus) );
	u8 byIdx = 0;
	u8 byPeriIdLoop;
    u8 byPeriIdMax;
	
	if( EQP_TYPE_VMP == byEqpType )
	{
		byPeriIdLoop = VMPID_MIN;
        byPeriIdMax = VMPID_MAX;
    }
    else
    {
        byPeriIdLoop = VMPTWID_MIN;
        byPeriIdMax = VMPTWID_MAX;
    }
	
	while( byPeriIdLoop <= byPeriIdMax && byIdx < MAXNUM_PERIEQP )
	{
		if( g_cMcuVcApp.IsPeriEqpValid( byPeriIdLoop ) )
		{
			g_cMcuVcApp.GetPeriEqpStatus( byPeriIdLoop, atEqpStatus+byIdx );
			byIdx++;
		}
		byPeriIdLoop++;
	}
    u8 byValidVmpNum = byIdx;
	
	if ( !CMcuPfmLmt::IsVmpStyleSupport( m_tConf, m_byConfIdx, 
		atEqpStatus, byValidVmpNum, byStyle, 
		byEqpId ))
    {
		wError = ERR_MCU_ALLIDLEVMP_NO_SUPPORT;
		return FALSE;
    }
	
	return TRUE;
	
#else
	
    // guzh [7/25/2007] FIXME��8000Bֻ��Ҫ�ҿ��е�VMP���ɣ���ʱ��ô��
    u8 byIdleVMPNum = 0;
    u8 abyIdleVMPId[MAXNUM_PERIEQP];
    memset( &abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
    g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );    
    if ( byIdleVMPNum > 0 )
    {
        byEqpId = abyIdleVMPId[0];
    }
    else
    {
        ConfLog( FALSE, "[IsMCUSupportVmpStyle] no idle vmp found!\n" );
        return FALSE;        
    }
    
    // guzh [1/30/2007] ����8000B�����ܱ���
    if ( !CMcuPfmLmt::IsVmpOprSupported( m_tConf, byStyle, m_tConfAllMtInfo.GetLocalJoinedMtNum(), GetMixMtNumInGrp(), wError )  )
    {
        ConfLog( FALSE, "[IsMCUSupportVmpStyle] Eqp.%d: IsVmpOprSupported return false. Error=%d !\n", 
			byEqpId, wError );
        return FALSE;
    }
	
    return TRUE;
#endif
}



/*====================================================================
    ������      : HasJoinedSpeaker
    ����        : �жϷ������Ƿ����
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: ��
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/31    3.0         ������       ����
====================================================================*/
BOOL32 CMcuVcInst::HasJoinedSpeaker( void )
{
	if( !m_tConf.HasSpeaker() )
		return  FALSE;
    
	TMt	tSpeaker;
	tSpeaker = GetLocalSpeaker();
	if( tSpeaker.GetType() == TYPE_MT )
		return m_tConfAllMtInfo.MtJoinedConf( tSpeaker.GetMtId() );
	else if( tSpeaker.GetType() == TYPE_MCUPERI )
		return TRUE;
	else
		return FALSE;
}

/*====================================================================
    ������      : HasJoinedChairman
    ����        : �ж���ϯ�Ƿ����
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: ��
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/31    3.0         ������       ����
====================================================================*/
BOOL32 CMcuVcInst::HasJoinedChairman( void )
{	
	if( !m_tConf.HasChairman() )
	{
		return  FALSE;
	}
    TMt		tChairman = m_tConf.GetChairman();
    return m_tConfAllMtInfo.MtJoinedConf( tChairman.GetMtId() );
}
/*==============================================================================
������    :  IsSelectedbyOtherMt
����      :  Mt�Ƿ�ѡ��
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-3-18					Ѧ��							create
==============================================================================*/
BOOL32 CMcuVcInst::IsSelectedbyOtherMt(u8 bySrcMtId,u8 byMode)
{
	BOOL32 bSelected = FALSE;
	TMtStatus tMtStatus;
	TMt tSelectedMt;
	for(u8 byLoop =1 ; byLoop<= MAXNUM_CONF_MT;byLoop++) 
	{
		if(m_tConfAllMtInfo.MtJoinedConf( byLoop ))
		{
			m_ptMtTable->GetMtStatus(byLoop,&tMtStatus);
			//tSelectedMt = tMtStatus.GetVideoMt(); //��Ȼ����ԣ����������ѡ��Դ���õ���SetSelectMt���������¾�
			tSelectedMt = tMtStatus.GetSelectMt( byMode ); //��ȡѡ��Դ
			if(!tSelectedMt.IsNull() && tSelectedMt.GetMtId() == bySrcMtId)
			{
				bSelected = TRUE;
				break;
			}
		}
	}
	return bSelected;

}
/*=============================================================================
�� �� ���� GetMixMtNumInGrp
��    �ܣ� �õ�����ͨ�����ն˸���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u8 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/8/4    4.0			������                  ����
=============================================================================*/
u8 CMcuVcInst::GetMixMtNumInGrp( void )
{
    u8 byMixMemberNum = 0;
    if ( mcuNoMix == m_tConf.m_tStatus.GetMixerMode() )
    {
        return 0;
    }

    for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
    {
        if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) && m_ptMtTable->IsMtInMixGrp( byLoop )  )            
        {
            byMixMemberNum++;
        }
    }

	return byMixMemberNum;
}

/*=============================================================================
�� �� ���� GetMaxMixNum
��    �ܣ� ��ȡ����֧�ֵ�����������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u8  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/8/19  4.0			������                  ����
=============================================================================*/
u8  CMcuVcInst::GetMaxMixNum(void)
{
    return (m_tConf.GetConfAttrb().IsSatDCastMode() ? MAXNUM_MIXERCHNNL_SATDCONF : MAXNUM_MIXER_CHNNL);
}

/*=============================================================================
    �� �� ���� NtfMcsMcuCurListAllConf
    ��    �ܣ� ֪ͨ��ص�ǰMCU�Ļ�����Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CServMsg &cServMsg
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/01/05  4.0			����                  ����
    2006/06/22  4.0         ����                  ֧���û���
=============================================================================*/
BOOL32 CMcuVcInst::NtfMcsMcuCurListAllConf( CServMsg &cServMsg )
{
	TConfInfo  *ptConfInfo = NULL;
	TConfMtTable  tConfMtTable;
	TConfAllMtInfo tConfAllMtInfo;
	TConfEqpModule tConfEqpModule;
    
    cServMsg.SetNullConfId();

	for (u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
	{
		TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
		if (tMapData.IsTemUsed())
		{
			TTemplateInfo tTemConf;
			if (g_cMcuVcApp.GetTemplate(byConfIdx, tTemConf))
			{
				g_cMcuVcApp.TemInfo2Msg(tTemConf, cServMsg);
				SendOneMsgToMcs(cServMsg, MCU_MCS_TEMSCHCONFINFO_NOTIF);
			}
		}

		if (tMapData.IsValidConf())
		{
			CMcuVcInst *pcMcuVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
			if (NULL == pcMcuVcInst)
			{
				continue;
			}

			//���η���ÿ�������֪ͨ            
			if (NULL != g_cMcuVcApp.GetConfInstHandle(byConfIdx))
			{
				ptConfInfo = &g_cMcuVcApp.GetConfInstHandle(byConfIdx)->m_tConf;

				//��ȡ��Ϣ�ɹ���֪ͨ�������̨
				cServMsg.SetConfId( ptConfInfo->GetConfId() );
				cServMsg.SetConfIdx( byConfIdx );                

				if (ptConfInfo->m_tStatus.IsScheduled())
				{
					g_cMcuVcApp.ConfInfoMsgPack(pcMcuVcInst, cServMsg);

					cServMsg.SetConfIdx(byConfIdx);  
					cServMsg.SetConfId(ptConfInfo->GetConfId());

					SendOneMsgToMcs(cServMsg, MCU_MCS_TEMSCHCONFINFO_NOTIF);
				}
				else
				{
					cServMsg.SetMsgBody((u8 *)ptConfInfo, sizeof(TConfInfo));

					SendOneMsgToMcs(cServMsg, MCU_MCS_CONFINFO_NOTIF);

					//��ΪVCS���飬����ͬʱ����vcs����״̬
					if (VCS_CONF == ptConfInfo->GetConfSource())
					{
						cServMsg.SetMsgBody((u8* )&pcMcuVcInst->m_cVCSConfStatus, sizeof(CBasicVCCStatus));
						SendOneMsgToMcs(cServMsg, MCU_VCS_CONFSTATUS_NOTIF);
					}

					//��������ն���Ϣ��
					if (g_cMcuVcApp.GetConfAllMtInfo(byConfIdx, &tConfAllMtInfo))
					{
						cServMsg.SetConfId(ptConfInfo->GetConfId());
						cServMsg.SetConfIdx(byConfIdx);
						cServMsg.SetMsgBody((u8 *)&tConfAllMtInfo, sizeof(TConfAllMtInfo));
						SendOneMsgToMcs(cServMsg, MCU_MCS_CONFALLMTINFO_NOTIF);
					}

					//��������ն˱�
					if (!g_cMcuVcApp.GetConfMtTable(byConfIdx, &tConfMtTable))
					{
						continue;
					}
					//��Mcu
					TMcu tMcu;
					cServMsg.SetConfId(ptConfInfo->GetConfId());
					cServMsg.SetConfIdx(byConfIdx);
					tMcu.SetMcu(LOCAL_MCUID);
					cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
					cServMsg.CatMsgBody((u8 *)tConfMtTable.m_atMtExt, 
					                     tConfMtTable.m_byMaxNumInUse * sizeof(TMtExt));
					SendOneMsgToMcs(cServMsg, MCU_MCS_MTLIST_NOTIF);

					//����Mcu
					for (u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++)
					{
						if (tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].IsNull())
						{
							continue;
						}
						u8 byMcuId = tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId();
						TConfMcInfo *ptInfo = pcMcuVcInst->m_ptConfOtherMcTable->GetMcInfo(byMcuId);
						if (NULL == ptInfo)
						{
							continue;
						}
						tMcu.SetMcu(tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId());
                        
                        // guzh [5/18/2007] ���ﲻ��ʵ���У�Ҫ�����ϼ�MCU������tConfAllMtInfo.m_tMMCU
                        if ( !g_cMcuVcApp.IsShowMMcuMtList() && 
                            !tConfAllMtInfo.m_tMMCU.IsNull() && tConfAllMtInfo.m_tMMCU.GetMtId() == byMcuId )
                        {
                            // guzh [4/30/2007] �ϼ�MCU�б����
                            continue;
                        }
                        

						cServMsg.SetMsgBody((u8*)&tMcu, sizeof(tMcu));
						for (s32 nLoop = 0; nLoop < MAXNUM_CONF_MT; nLoop++)
						{
							if (ptInfo->m_atMtStatus[nLoop].IsNull() || 
								0 == ptInfo->m_atMtStatus[nLoop].GetMtId()) //�Լ�
							{
								continue;
							}
							cServMsg.CatMsgBody((u8 *)&(ptInfo->m_atMtExt[nLoop]), sizeof(TMtExt));
						}
						SendOneMsgToMcs(cServMsg, MCU_MCS_MTLIST_NOTIF);
					}
				}
			}
		}
	}
	
	//��Ӧ��
	SendOneMsgToMcs(cServMsg, MCU_MCS_LISTALLCONF_ACK);

	return TRUE;
}

/*=============================================================================
    �� �� ���� SendOneMsgToMcs
    ��    �ܣ� ����һ����Ϣ����� ����MCS��ָ�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CServMsg &cServMsg, 
	           u16 wEvent
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/01/05  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::SendOneMsgToMcs( CServMsg &cServMsg, u16 wEvent )
{
	if (IsMcCmdFromThisMcu(cServMsg))
	{
		SendReplyBack(cServMsg, wEvent);
		
	}
	else
	{
		SendMsgToAllMcs(wEvent, cServMsg);
	}
	return TRUE;
}


/*=============================================================================
    �� �� ���� GetInstState
    ��    �ܣ� �������ݵ���ʱ ��ȡ ʵ��״̬�� ״̬
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [OUT]  u8 &byState //STATE_IDLE STATE_SCHEDULED STATE_ONGOING
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::GetInstState( u8 &byState )
{
	byState = (u8)CurState();

	return TRUE;
}

/*=============================================================================
    �� �� ���� SetInstState
    ��    �ܣ� �������ݵ���ʱ �ָ� ʵ��״̬�� ״̬
	           ͬʱͬ������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [OUT]  u8 byState //STATE_IDLE STATE_SCHEDULED STATE_ONGOING
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::SetInstState( u8 byState )
{
    //  xsl [3/3/2006] STATE_WAITEQP Ϊ�м�״̬����ͬ��
	if (byState > STATE_ONGOING || STATE_WAITEQP == byState)
	{
		return FALSE;
	}

	if (STATE_IDLE == byState)
	{
		//��ԭ��ʵ��״̬����ΪSTATE_IDLE�����ͷŻ���
		if (STATE_IDLE != CurState())
		{
			//��������ͳһ���
			//g_cMcuVcApp.RemoveConf( m_byConfIdx, FALSE );

			DeleteAlias();
			
			//��ջ���ʵ��
			ClearVcInst();
		}
	}
	else
	{
		//��ԭ��ʵ��״̬��Ϊ STATE_IDLE �� STATE_WAITFOR���򱣴����
        //˵����STATE_IDLE    Ϊ����״̬
        //      STATE_WAITEQP Ϊ�ȴ�mp����ʱ��ʱ�����¸���ǿ�õ�״̬
		if (STATE_IDLE == CurState() || STATE_WAITEQP == CurState())
		{
			//�����ڴ�
			if (NULL == m_ptMtTable)
			{
				m_ptMtTable = new TConfMtTable;
				if (NULL == m_ptMtTable)
				{
					return FALSE;
				}
			}
			if (STATE_ONGOING == byState)
			{
				if (NULL == m_ptSwitchTable)
				{
					m_ptSwitchTable = new TConfSwitchTable;
					if (NULL == m_ptSwitchTable)
					{
						MCU_SAFE_DELETE(m_ptMtTable)
						return FALSE;
					}
				}
				if (NULL == m_ptConfOtherMcTable)
				{
					m_ptConfOtherMcTable = new TConfOtherMcTable;
					if (NULL == m_ptConfOtherMcTable)
					{
						MCU_SAFE_DELETE(m_ptMtTable)
						MCU_SAFE_DELETE(m_ptSwitchTable)
						return FALSE;
					}
				}
			}
		}
		
		g_cMcuVcApp.AddConf( this, FALSE );
	}
	NEXTSTATE( byState );

	return TRUE;
}

/*=============================================================================
    �� �� ���� HangupInstTimer
    ��    �ܣ� �������ݵ���ʱ ��ͣ ��ѭ��ԤԼ����Ȼ��鶨ʱ������STATE_IDLEʵ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::HangupInstTimer( void )
{
	if (STATE_IDLE == CurState())
	{
		return FALSE;
	}
	return TRUE;
}

/*=============================================================================
    �� �� ���� ResumeInstTimer
    ��    �ܣ� �������ݵ���ʱ �ָ� ��ѭ��ԤԼ����Ȼ��鶨ʱ�� ����STATE_IDLEʵ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::ResumeInstTimer( void )
{
	if (STATE_IDLE == CurState())
	{
		return FALSE;
	}
	return TRUE;
}

/*=============================================================================
    �� �� ���� GetConfMtTableData
    ��    �ܣ� �������ݵ���ʱ ��ȡ �����ն˱� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
               [OUT]     u32 &dwOutBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::GetConfMtTableData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (STATE_IDLE == CurState() || NULL == m_ptMtTable)
	{
		return FALSE;
	}
	if (NULL == pbyBuf || dwInBufLen < sizeof(TConfMtTable))
	{
		return FALSE;
	}
	//�ɶ��ն��б����pack����
	memcpy(pbyBuf, (s8*)m_ptMtTable, sizeof(TConfMtTable));
	dwOutBufLen = sizeof(TConfMtTable);

	return TRUE;
}

/*=============================================================================
    �� �� ���� SetTConfMtTableData
    ��    �ܣ� �������ݵ���ʱ �ָ� �����ն˱� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::SetConfMtTableData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (STATE_IDLE == CurState() || NULL == m_ptMtTable)
	{
		return FALSE;
	}
	//�������---	
	if (NULL == pbyBuf || dwInBufLen < sizeof(TConfMtTable)) //!= sizeof(TConfMtTable))
	{
		return FALSE;
	}
	//�ɶ��ն��б����pack����
	memcpy((s8*)m_ptMtTable, pbyBuf, sizeof(TConfMtTable));

	return TRUE;
}

/*=============================================================================
    �� �� ���� GetConfSwitchTableData
    ��    �ܣ� �������ݵ���ʱ ��ȡ ���齻���� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
               [OUT]     u32 &dwOutBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::GetConfSwitchTableData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (STATE_IDLE == CurState() || NULL == m_ptSwitchTable)
	{
		return FALSE;
	}
	if (NULL == pbyBuf || dwInBufLen < sizeof(TConfSwitchTable))
	{
		return FALSE;
	}
	//�ɶ��б����pack����
	memcpy(pbyBuf, (s8*)m_ptSwitchTable, sizeof(TConfSwitchTable));
	dwOutBufLen = sizeof(TConfSwitchTable);

	return TRUE;
}

/*=============================================================================
    �� �� ���� SetConfSwitchTableData
    ��    �ܣ� �������ݵ���ʱ �ָ� ���齻���� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::SetConfSwitchTableData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (STATE_IDLE == CurState() || NULL == m_ptSwitchTable)
	{
		return FALSE;
	}
	if (NULL == pbyBuf || dwInBufLen < sizeof(TConfSwitchTable))
	{
		return FALSE;
	}
	//�ɶ��б����pack����
	memcpy((s8*)m_ptSwitchTable, pbyBuf, sizeof(TConfSwitchTable));

	return TRUE;
}

/*=============================================================================
    �� �� ���� GetConfOtherMcTableData
    ��    �ܣ� �������ݵ���ʱ ��ȡ ����Mc�ն��б� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
               [OUT]     u32 &dwOutBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::GetConfOtherMcTableData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (STATE_IDLE == CurState() || NULL == m_ptConfOtherMcTable)
	{
		return FALSE;
	}
	if (NULL == pbyBuf || dwInBufLen < (sizeof(TMSVcOtherMcuState)+sizeof(TConfOtherMcTable)))
	{
		return FALSE;
	}

	TMSVcOtherMcuState *ptOtherMcuState = (TMSVcOtherMcuState*)pbyBuf;
	u8 *pbyMcuDataBuf = pbyBuf+sizeof(TMSVcOtherMcuState);
	u8  byOtherMcuNum = 0;
	memset(pbyBuf, 0, sizeof(TMSVcOtherMcuState));
	
	for (u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++)
	{
		if (0 == m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop].m_byMcuId)
		{
			ptOtherMcuState->m_abyOnline[byLoop] = 0;
		}
		else
		{
			//�ɶ��ն��б����pack����
			ptOtherMcuState->m_abyOnline[byLoop] = 1;
			memcpy(pbyMcuDataBuf, (u8*)(&m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]), sizeof(TConfMcInfo));
			pbyMcuDataBuf += sizeof(TConfMcInfo);

			byOtherMcuNum += 1;
		}
	}
	
	dwOutBufLen = sizeof(TMSVcOtherMcuState) + byOtherMcuNum*sizeof(TConfMcInfo);

	return TRUE;
}

/*=============================================================================
    �� �� ���� SetConfOtherMcTableData
    ��    �ܣ� �������ݵ���ʱ �ָ� ����Mc�ն��б� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::SetConfOtherMcTableData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (STATE_IDLE == CurState() || NULL == m_ptConfOtherMcTable)
	{
		return FALSE;
	}
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcOtherMcuState))
	{
		return FALSE;
	}

	TMSVcOtherMcuState *ptOtherMcuState = (TMSVcOtherMcuState*)pbyBuf;
	u8 *pbyMcuDataBuf = pbyBuf+sizeof(TMSVcOtherMcuState);
	
	//�ɶ��ն��б����pack����
	for (u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++)
	{
		if (1 == ptOtherMcuState->m_abyOnline[byLoop])
		{
			memcpy((s8*)(&m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]), pbyMcuDataBuf, sizeof(TConfMcInfo));
			pbyMcuDataBuf += sizeof(TConfMcInfo);	
		}
		else
		{
			m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop].SetNull();
		}
	}
	
	return TRUE;
}

/*=============================================================================
    �� �� ���� GetInstOtherData
    ��    �ܣ� �������ݵ���ʱ ��ȡ ��������̬������б���Ϣ���������ͬ�� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
			   [OUT]     u32 &dwOutBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::GetInstOtherData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (STATE_IDLE == CurState())
	{
		return FALSE;
	}
	
	dwOutBufLen  = 0;
	u32 dwOutLen = OprInstOtherData(pbyBuf, dwInBufLen, TRUE);
	if (0 == dwOutLen)
	{
		return FALSE;
	}

	dwOutBufLen = dwOutLen;
	return TRUE;
}

/*=============================================================================
    �� �� ���� SetInstOtherData
    ��    �ܣ� �������ݵ���ʱ �ָ� ��������̬������б���Ϣ���������ͬ�� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [IN]  BOOL32 bResumeTimer
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::SetInstOtherData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bResumeTimer )
{
	if (STATE_IDLE == CurState())
	{
		return FALSE;
	}

	if (0 == OprInstOtherData(pbyBuf, dwInBufLen, FALSE))
	{
		return FALSE;
	}

	//�Ƿ�ָ�����ʵ���漰�Ķ�ʱ����ֻ�ڵ�һ����������ʱ�ָ�
	if (bResumeTimer)
	{
		//��ʱ���� ���� ԤԼ����
		if (STATE_ONGOING == CurState())
		{
			//�ָ� ��ʱ�����Զ�����ʱ��
			if (0 != m_tConf.GetDuration())
			{
				KillTimer(MCUVC_ONGOING_CHECK_TIMER);
				SetTimer(MCUVC_ONGOING_CHECK_TIMER, TIMESPACE_ONGOING_CHECK);
			}

			//�ָ� �Զ������ն�ʱ��
			KillTimer(MCUVC_INVITE_UNJOINEDMT_TIMER);
			SetTimer(MCUVC_INVITE_UNJOINEDMT_TIMER, 1000*m_tConf.m_tStatus.GetCallInterval());
			
			//�ָ� MCU��ƵԴ������ʱ��
			KillTimer(MCUVC_MCUSRC_CHECK_TIMER);
			SetTimer(MCUVC_MCUSRC_CHECK_TIMER, TIMESPACE_MCUSRC_CHECK);
			
			//�ָ� VMP��ϯ���������ʱ��
			if (0)
			{
				KillTimer(MCUVC_CHANGE_VMPCHAN_CHAIRMAN_TIMER);
				SetTimer(MCUVC_CHANGE_VMPCHAN_CHAIRMAN_TIMER, TIMESPACE_CHANGEVMPPARAM, VMP_MEMBERTYPE_CHAIRMAN);
			}
			
			//�ָ� VMP�����˸��������ʱ��
			if (0)
			{
				KillTimer(MCUVC_CHANGE_VMPCHAN_SPEAKER_TIMER);
				SetTimer(MCUVC_CHANGE_VMPCHAN_SPEAKER_TIMER, TIMESPACE_CHANGEVMPPARAM, VMP_MEMBERTYPE_SPEAKER);
			}

			if (POLL_STATE_NORMAL == m_tConf.m_tStatus.GetPollState())
			{
				KillTimer(MCUVC_POLLING_CHANGE_TIMER);
				SetTimer(MCUVC_POLLING_CHANGE_TIMER, 1000*1);
			}

            if(POLL_STATE_NORMAL == m_tConf.m_tStatus.GetTvWallPollState())
            {
                KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER);
                SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER, 1000*1);
            }
            
			// fxh
			if (VCS_POLL_START == m_cVCSConfStatus.GetChairPollState())
			{
				KillTimer(MCUVC_VCS_CHAIRPOLL_TIMER);
				SetTimer(MCUVC_VCS_CHAIRPOLL_TIMER, 1000*m_cVCSConfStatus.GetPollIntval());
			}

            // guzh [4/17/2007] 
            // �Զ�¼��Ķ�ʱ�����þ��Զ�ʱ
		}
		else
		{
			//�ָ� ԤԼ������ʱ��
			SetTimer( MCUVC_SCHEDULED_CHECK_TIMER, TIMESPACE_SCHEDULED_CHECK );
		}
	}

	return TRUE;
}

/*=============================================================================
    �� �� ���� OprInstOtherData
    ��    �ܣ� �������ݵ���ʱ ͬ�� ��������̬������б���Ϣ���������ͬ�� ����
	           ����ͬ��������MCUӦ�û���һ�£�ͬ�������ݽṹ�ݲ����ǽ���ѹջ���⣺Pack Ingore
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
               [IN]      BOOL32 bGet TRUE ����ȡ FALSE - �ָ� 
    �� �� ֵ�� u32  ͬ���������ܳ���
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
u32 CMcuVcInst::OprInstOtherData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bGet )
{
	u32 dwOtherDataLen = GetInstOtherDataLen();
	//�������---
	if (NULL == pbyBuf || dwInBufLen < dwOtherDataLen)
	{
		return 0;
	}

	if (bGet)
	{
		memcpy(pbyBuf, &m_tConf, sizeof(m_tConf));
		pbyBuf += sizeof(m_tConf);
		memcpy(pbyBuf, &m_tConfAllMtInfo, sizeof(m_tConfAllMtInfo));
		pbyBuf += sizeof(m_tConfAllMtInfo);
		memcpy(pbyBuf, &m_tConfProtectInfo, sizeof(m_tConfProtectInfo));
		pbyBuf += sizeof(m_tConfProtectInfo);
		memcpy(pbyBuf, &m_tConfPollParam, sizeof(m_tConfPollParam));
		pbyBuf += sizeof(m_tConfPollParam);
		memcpy(pbyBuf, &m_tTvWallPollParam, sizeof(m_tTvWallPollParam));
		pbyBuf += sizeof(m_tTvWallPollParam);
		memcpy( pbyBuf, &m_tHduPollParam, sizeof(m_tHduPollParam) );  // add by jlb
        pbyBuf += sizeof(m_tHduPollParam);
		memcpy(pbyBuf, &m_tConfEqpModule, sizeof(m_tConfEqpModule));
		pbyBuf += sizeof(m_tConfEqpModule);
		memcpy(pbyBuf, &m_tVidBrdSrc, sizeof(m_tVidBrdSrc));
		pbyBuf += sizeof(m_tVidBrdSrc);
		memcpy(pbyBuf, &m_tAudBrdSrc, sizeof(m_tAudBrdSrc));
		pbyBuf += sizeof(m_tAudBrdSrc);
		memcpy(pbyBuf, &m_tDoubleStreamSrc, sizeof(m_tDoubleStreamSrc));
		pbyBuf += sizeof(m_tDoubleStreamSrc);
		memcpy(pbyBuf, &m_tH239TokenOwnerInfo, sizeof(m_tH239TokenOwnerInfo));
		pbyBuf += sizeof(m_tH239TokenOwnerInfo);
		
		memcpy(pbyBuf, &m_tVmpEqp, sizeof(m_tVmpEqp));
		pbyBuf += sizeof(m_tVmpEqp);
		memcpy(pbyBuf, &m_tVmpTwEqp, sizeof(m_tVmpTwEqp));
		pbyBuf += sizeof(m_tVmpTwEqp);
		memcpy(pbyBuf, &m_tMixEqp, sizeof(m_tMixEqp));
		pbyBuf += sizeof(m_tMixEqp);
		memcpy(pbyBuf, &m_tRecEqp, sizeof(m_tRecEqp));
		pbyBuf += sizeof(m_tRecEqp);
		memcpy(pbyBuf, &m_tPlayEqp, sizeof(m_tPlayEqp));
		pbyBuf += sizeof(m_tPlayEqp);
		memcpy(pbyBuf, &m_tPlayEqpAttrib, sizeof(m_tPlayEqpAttrib));
		pbyBuf += sizeof(m_tPlayEqpAttrib);
		memcpy(pbyBuf, &m_tRecPara, sizeof(m_tRecPara));
		pbyBuf += sizeof(m_tRecPara);
		memcpy(pbyBuf, &m_tAudBasEqp, sizeof(m_tAudBasEqp));
		pbyBuf += sizeof(m_tAudBasEqp);
		memcpy(pbyBuf, &m_tVidBasEqp, sizeof(m_tVidBasEqp));
		pbyBuf += sizeof(m_tVidBasEqp);
		memcpy(pbyBuf, &m_tBrBasEqp, sizeof(m_tBrBasEqp));
		pbyBuf += sizeof(m_tBrBasEqp);
		memcpy(pbyBuf, &m_tCasdAudBasEqp, sizeof(m_tCasdAudBasEqp));
		pbyBuf += sizeof(m_tCasdAudBasEqp);
		memcpy(pbyBuf, &m_tCasdVidBasEqp, sizeof(m_tCasdVidBasEqp));
		pbyBuf += sizeof(m_tCasdVidBasEqp);
		memcpy(pbyBuf, &m_tPrsEqp, sizeof(m_tPrsEqp));
		pbyBuf += sizeof(m_tPrsEqp);
		
		memcpy(pbyBuf, &m_byMixGrpId, sizeof(m_byMixGrpId));
		pbyBuf += sizeof(m_byMixGrpId);
		memcpy(pbyBuf, &m_byRecChnnl, sizeof(m_byRecChnnl));
		pbyBuf += sizeof(m_byRecChnnl);
		memcpy(pbyBuf, &m_byPlayChnnl, sizeof(m_byPlayChnnl));
		pbyBuf += sizeof(m_byPlayChnnl);
		memcpy(pbyBuf, &m_byPrsChnnl, sizeof(m_byPrsChnnl));
		pbyBuf += sizeof(m_byPrsChnnl);
		memcpy(pbyBuf, &m_byPrsChnnl2, sizeof(m_byPrsChnnl2));
		pbyBuf += sizeof(m_byPrsChnnl2);
		memcpy(pbyBuf, &m_byPrsChnnlAud, sizeof(m_byPrsChnnlAud));
		pbyBuf += sizeof(m_byPrsChnnlAud);
		memcpy(pbyBuf, &m_byPrsChnnlAudBas, sizeof(m_byPrsChnnlAudBas));
		pbyBuf += sizeof(m_byPrsChnnlAudBas);
		memcpy(pbyBuf, &m_byPrsChnnlVidBas, sizeof(m_byPrsChnnlVidBas));
		pbyBuf += sizeof(m_byPrsChnnlVidBas);
		memcpy(pbyBuf, &m_byPrsChnnlBrBas, sizeof(m_byPrsChnnlBrBas));
		pbyBuf += sizeof(m_byPrsChnnlBrBas);
		memcpy(pbyBuf, &m_byAudBasChnnl, sizeof(m_byAudBasChnnl));
		pbyBuf += sizeof(m_byAudBasChnnl);
		memcpy(pbyBuf, &m_byVidBasChnnl, sizeof(m_byVidBasChnnl));
		pbyBuf += sizeof(m_byVidBasChnnl);
		memcpy(pbyBuf, &m_byBrBasChnnl, sizeof(m_byBrBasChnnl));
		pbyBuf += sizeof(m_byBrBasChnnl);
		memcpy(pbyBuf, &m_byCasdAudBasChnnl, sizeof(m_byCasdAudBasChnnl));
		pbyBuf += sizeof(m_byCasdAudBasChnnl);
		memcpy(pbyBuf, &m_byCasdVidBasChnnl, sizeof(m_byCasdVidBasChnnl));
		pbyBuf += sizeof(m_byCasdVidBasChnnl);
		memcpy(pbyBuf, &m_wVidBasBitrate, sizeof(m_wVidBasBitrate));
		pbyBuf += sizeof(m_wVidBasBitrate);
		memcpy(pbyBuf, &m_wBasBitrate, sizeof(m_wBasBitrate));
		pbyBuf += sizeof(m_wBasBitrate);
		memcpy(pbyBuf, &m_awVMPBrdBitrate, sizeof(m_awVMPBrdBitrate));	// xliang [8/5/2009] 
		pbyBuf += sizeof(m_awVMPBrdBitrate);
        // guzh [8/28/2007] ��ʵMINIMCU û������������Ϊ��ֹ�Ժ�����
#ifdef _MINIMCU_
        memcpy(pbyBuf, &m_byIsDoubleMediaConf, sizeof(m_byIsDoubleMediaConf));
        pbyBuf += sizeof(m_byIsDoubleMediaConf);
#endif

//		memcpy(pbyBuf, &m_tBasCapSet, sizeof(m_tBasCapSet));
//		pbyBuf += sizeof(m_tBasCapSet);
				
//		memcpy(pbyBuf, &m_byRegGKDriId, sizeof(m_byRegGKDriId));
//		pbyBuf += sizeof(m_byRegGKDriId);

		memcpy(pbyBuf, &m_tRefreshParam, sizeof(m_tRefreshParam));
		pbyBuf += sizeof(m_tRefreshParam);
		memcpy(pbyBuf, &m_tVacLastSpeaker, sizeof(m_tVacLastSpeaker));
		pbyBuf += sizeof(m_tVacLastSpeaker);
		memcpy(pbyBuf, &m_tLastSpeaker, sizeof(m_tLastSpeaker));
		pbyBuf += sizeof(m_tLastSpeaker);      
        
		memcpy(pbyBuf, &m_tConfInStatus, sizeof(m_tConfInStatus));
		pbyBuf += sizeof(m_tConfInStatus);

		memcpy(pbyBuf, &m_tCascadeMMCU, sizeof(m_tCascadeMMCU));
		pbyBuf += sizeof(m_tCascadeMMCU);

		memcpy(pbyBuf, &m_dwSpeakerViewId, sizeof(m_dwSpeakerViewId));
		pbyBuf += sizeof(m_dwSpeakerViewId);
		memcpy(pbyBuf, &m_dwVmpViewId, sizeof(m_dwVmpViewId));
		pbyBuf += sizeof(m_dwVmpViewId);
		memcpy(pbyBuf, &m_dwSpeakerVideoId, sizeof(m_dwSpeakerVideoId));
		pbyBuf += sizeof(m_dwSpeakerVideoId);
		memcpy(pbyBuf, &m_dwVmpVideoId, sizeof(m_dwVmpVideoId));
		pbyBuf += sizeof(m_dwVmpVideoId);
		memcpy(pbyBuf, &m_dwSpeakerAudioId, sizeof(m_dwSpeakerAudioId));
		pbyBuf += sizeof(m_dwSpeakerAudioId);
		memcpy(pbyBuf, &m_dwMixerAudioId, sizeof(m_dwMixerAudioId));
		pbyBuf += sizeof(m_dwMixerAudioId);
		memcpy(pbyBuf, &m_abySerHdr, sizeof(m_abySerHdr));
		pbyBuf += sizeof(m_abySerHdr);

		memcpy(pbyBuf, &m_byConfIdx, sizeof(m_byConfIdx));
		pbyBuf += sizeof(m_byConfIdx);	
        memcpy(pbyBuf, &m_byDcsIdx, sizeof(m_byDcsIdx));
        pbyBuf += sizeof(m_byDcsIdx);

        memcpy(pbyBuf, m_abyCasChnCheckTimes, sizeof(m_abyCasChnCheckTimes));   // guzh [2/25/2007]
		pbyBuf += sizeof(m_abyCasChnCheckTimes);     

        memcpy(pbyBuf, &m_tLastSpyMt, sizeof(m_tLastSpyMt));
        pbyBuf += sizeof(m_tLastSpyMt);
		memcpy(pbyBuf, &m_tLastVmpParam, sizeof(m_tLastVmpParam));
		pbyBuf += sizeof(m_tLastVmpParam);
		memcpy(pbyBuf, &m_tLastVmpTwParam, sizeof(m_tLastVmpTwParam));
		pbyBuf += sizeof(m_tLastVmpTwParam);
        memcpy(pbyBuf, &m_tChargeSsnId, sizeof(m_tChargeSsnId));
        pbyBuf += sizeof(m_tChargeSsnId);

        memcpy(pbyBuf, &m_abyMixMtId, sizeof(m_abyMixMtId));        // zbq [03/19/2007]
        pbyBuf += sizeof(m_abyMixMtId);
        memcpy(pbyBuf, &m_byCreateBy, sizeof(m_byCreateBy));
        pbyBuf += sizeof(m_byCreateBy);

        memcpy(pbyBuf, &m_tPollSwitchParam, sizeof(m_tPollSwitchParam));    // guzh [7/21/2007] 
        pbyBuf += sizeof(m_tPollSwitchParam);
        memcpy(pbyBuf, &m_tRollCaller, sizeof(m_tRollCaller));
        pbyBuf += sizeof(m_tRollCaller);
        memcpy(pbyBuf, &m_tRollCallee, sizeof(m_tRollCallee));
        pbyBuf += sizeof(m_tRollCallee);

        memcpy(pbyBuf, &m_tLastMixParam, sizeof(m_tLastMixParam));
        pbyBuf += sizeof(m_tLastMixParam);

		memcpy(pbyBuf, &m_bySpeakerVidSrcSpecType, sizeof(m_bySpeakerVidSrcSpecType));	//zgc, 2008-04-16
		pbyBuf += sizeof(m_bySpeakerVidSrcSpecType);
		memcpy(pbyBuf, &m_bySpeakerAudSrcSpecType, sizeof(m_bySpeakerAudSrcSpecType));
		pbyBuf += sizeof(m_bySpeakerAudSrcSpecType);
    
        memcpy(pbyBuf, &m_cMtRcvGrp, sizeof(m_cMtRcvGrp));
        pbyBuf += sizeof(m_cMtRcvGrp);
        memcpy(pbyBuf, &m_cBasMgr, sizeof(m_cBasMgr));
        pbyBuf += sizeof(m_cBasMgr);

		memcpy(pbyBuf,&m_byLastDsSrcMtId,sizeof(m_byLastDsSrcMtId));  // xliang [12/19/2008] ˫����������
		pbyBuf += sizeof(m_byLastDsSrcMtId);
		memcpy(pbyBuf,&m_tLogicChnnl,sizeof(m_tLogicChnnl));
		pbyBuf += sizeof(m_tLogicChnnl);

		memcpy(pbyBuf,&m_byMtIdNotInvite,sizeof(m_byMtIdNotInvite)); // xliang [12/26/2008] 
		pbyBuf += sizeof(m_byMtIdNotInvite);

		// xliang [5/6/2009] prs �����Ķ�Ӧ�㲥Դ4����ͨ��
		memcpy(pbyBuf,&m_byPrsChnnlVmpOut1,sizeof(u8)); 
		pbyBuf += sizeof(u8);
		memcpy(pbyBuf,&m_byPrsChnnlVmpOut2,sizeof(u8)); 
		pbyBuf += sizeof(u8);
		memcpy(pbyBuf,&m_byPrsChnnlVmpOut3,sizeof(u8)); 
		pbyBuf += sizeof(u8);
		memcpy(pbyBuf,&m_byPrsChnnlVmpOut4,sizeof(u8)); 
		pbyBuf += sizeof(u8);
		memcpy(pbyBuf, &m_tHduBatchPollInfo, sizeof(m_tHduBatchPollInfo) );    // add by jlb
		pbyBuf += sizeof(m_tHduBatchPollInfo);
		memcpy(pbyBuf, &m_tHduPollSchemeInfo, sizeof(m_tHduPollSchemeInfo) );
		pbyBuf += sizeof(m_tHduPollSchemeInfo);

		// xliang [5/12/2009] VMP related
		memcpy(pbyBuf, &m_tVmpChnnlInfo,sizeof(m_tVmpChnnlInfo)); 
		pbyBuf += sizeof(m_tVmpChnnlInfo);
		memcpy(pbyBuf, &m_atVMPTmpMember, sizeof(m_atVMPTmpMember));
		pbyBuf += sizeof(m_atVMPTmpMember);
		memcpy(pbyBuf, &m_bNewMemberSeize, sizeof(m_bNewMemberSeize));
		pbyBuf += sizeof(m_bNewMemberSeize);
		memcpy(pbyBuf, &m_tLastVmpChnnlMemInfo, sizeof(m_tLastVmpChnnlMemInfo));
		pbyBuf += sizeof(m_tLastVmpChnnlMemInfo);
		memcpy(pbyBuf, &m_tVmpBatchPollInfo, sizeof(m_tVmpBatchPollInfo));
		pbyBuf += sizeof(m_tVmpBatchPollInfo);
		memcpy(pbyBuf, &m_tVmpPollParam, sizeof(m_tVmpPollParam));
		pbyBuf += sizeof(m_tVmpPollParam);
		memcpy(pbyBuf, &m_byVmpOperating, sizeof(m_byVmpOperating));
		pbyBuf += sizeof(m_byVmpOperating);

		// VCS����״̬��Ϣͬ��
		memcpy(pbyBuf, &m_cVCSConfStatus, sizeof(m_cVCSConfStatus));
		pbyBuf += sizeof(m_cVCSConfStatus);		
	}
	else
	{
		memcpy(&m_tConf, pbyBuf, sizeof(m_tConf));
		pbyBuf += sizeof(m_tConf);
		memcpy(&m_tConfAllMtInfo, pbyBuf, sizeof(m_tConfAllMtInfo));
		pbyBuf += sizeof(m_tConfAllMtInfo);

		//���������Ϣ��ǿ��ͬ��, ��Ϊͬһ��mcs������mpc�ϵ�Ssnid���ܲ�һ��
		TConfProtectInfo *ptConfProtectInfo = (TConfProtectInfo *)pbyBuf;
		m_tConfProtectInfo.SetLockByMcu(ptConfProtectInfo->GetLockedMcuId());
		//memcpy(&m_tConfProtectInfo, pbyBuf, sizeof(m_tConfProtectInfo));
		//����ǻ���������飬����������״̬�е�SSRC��������ͬID�Ļ�أ�������ڣ�
		//�ñ��ػ��ID����LockMcs��zgc, 2008-03-25
		if ( CONF_LOCKMODE_LOCK == m_tConf.m_tStatus.GetProtectMode() )
		{
			TMSVcMCState tMSVcMCState;
			g_cMSSsnApp.GetMSVcMCState( tMSVcMCState );

			if ( tMSVcMCState.m_abyOnline[ptConfProtectInfo->GetLockedMcSsnId()-1] != 1 )
			{
				// ��ӦId��MCSû�����ӵ���MPC, ��ȡ����������
				ConfLog( FALSE, "[OprInstOtherData][ERROR] Lock conf.%s mcs.%d isn't connecting active mcu, it's impossible!\n ",
							m_tConf.GetConfName(), ptConfProtectInfo->GetLockedMcSsnId() );
				m_tConf.m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );
			}
			else
			{
				u32 dwSSRC = tMSVcMCState.m_tMcsRegInfo[ptConfProtectInfo->GetLockedMcSsnId()-1].GetMcsSSRC();
				u8 byIdx = 0;
				for ( byIdx = 0; byIdx < MAXNUM_MCU_MC; byIdx++ )
				{
					if ( !g_cMcuVcApp.IsMcConnected( byIdx+1 ) )
					{
						continue;
					}
					TMcsRegInfo tMcsRegInfo;
					g_cMcuVcApp.GetMcsRegInfo( byIdx+1, &tMcsRegInfo );
					if ( dwSSRC == tMcsRegInfo.GetMcsSSRC() )
					{
						//����lockmcsΪ���ػ��ID
						m_tConfProtectInfo.SetLockByMcs( byIdx+1 );
						break;
					}
				}
				//û���ҵ�ͬSSRC��MCS���ӱ�MCU����ȡ����������
				if ( byIdx == MAXNUM_MCU_MC ) 
				{
					ConfLog( FALSE, "[OprInstOtherData][ERROR] Can't find mcs<SSRC.%d> connecting standby mcu, it's impossible!\n ",
							dwSSRC );
					m_tConf.m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );
				}
			}			
		}
		pbyBuf += sizeof(m_tConfProtectInfo);
		
		memcpy(&m_tConfPollParam, pbyBuf, sizeof(m_tConfPollParam));
		pbyBuf += sizeof(m_tConfPollParam);
		memcpy(&m_tTvWallPollParam, pbyBuf, sizeof(m_tTvWallPollParam));
		pbyBuf += sizeof(m_tTvWallPollParam);
        memcpy(&m_tHduPollParam, pbyBuf, sizeof(m_tHduPollParam));        // add by jlb
		pbyBuf += sizeof(m_tHduPollParam);
		memcpy(&m_tConfEqpModule, pbyBuf, sizeof(m_tConfEqpModule));
		pbyBuf += sizeof(m_tConfEqpModule);
		memcpy(&m_tVidBrdSrc, pbyBuf, sizeof(m_tVidBrdSrc));
		pbyBuf += sizeof(m_tVidBrdSrc);
		memcpy(&m_tAudBrdSrc, pbyBuf, sizeof(m_tAudBrdSrc));
		pbyBuf += sizeof(m_tAudBrdSrc);
		memcpy(&m_tDoubleStreamSrc, pbyBuf, sizeof(m_tDoubleStreamSrc));
		pbyBuf += sizeof(m_tDoubleStreamSrc);
		memcpy(&m_tH239TokenOwnerInfo, pbyBuf, sizeof(m_tH239TokenOwnerInfo));
		pbyBuf += sizeof(m_tH239TokenOwnerInfo);
		
		memcpy(&m_tVmpEqp, pbyBuf, sizeof(m_tVmpEqp));
		pbyBuf += sizeof(m_tVmpEqp);
		memcpy(&m_tVmpTwEqp, pbyBuf, sizeof(m_tVmpTwEqp));
		pbyBuf += sizeof(m_tVmpTwEqp);
		memcpy(&m_tMixEqp, pbyBuf, sizeof(m_tMixEqp));
		pbyBuf += sizeof(m_tMixEqp);
		memcpy(&m_tRecEqp, pbyBuf, sizeof(m_tRecEqp));
		pbyBuf += sizeof(m_tRecEqp);
		memcpy(&m_tPlayEqp, pbyBuf, sizeof(m_tPlayEqp));
		pbyBuf += sizeof(m_tPlayEqp);
		memcpy(&m_tPlayEqpAttrib, pbyBuf, sizeof(m_tPlayEqpAttrib));
		pbyBuf += sizeof(m_tPlayEqpAttrib);
		memcpy(&m_tRecPara, pbyBuf, sizeof(m_tRecPara));
		pbyBuf += sizeof(m_tRecPara);
		memcpy(&m_tAudBasEqp, pbyBuf, sizeof(m_tAudBasEqp));
		pbyBuf += sizeof(m_tAudBasEqp);
		memcpy(&m_tVidBasEqp, pbyBuf, sizeof(m_tVidBasEqp));
		pbyBuf += sizeof(m_tVidBasEqp);
		memcpy(&m_tBrBasEqp, pbyBuf, sizeof(m_tBrBasEqp));
		pbyBuf += sizeof(m_tBrBasEqp);
		memcpy(&m_tCasdAudBasEqp, pbyBuf, sizeof(m_tCasdAudBasEqp));
		pbyBuf += sizeof(m_tCasdAudBasEqp);
		memcpy(&m_tCasdVidBasEqp, pbyBuf, sizeof(m_tCasdVidBasEqp));
		pbyBuf += sizeof(m_tCasdVidBasEqp);
		memcpy(&m_tPrsEqp, pbyBuf, sizeof(m_tPrsEqp));
		pbyBuf += sizeof(m_tPrsEqp);
		
		memcpy(&m_byMixGrpId, pbyBuf, sizeof(m_byMixGrpId));
		pbyBuf += sizeof(m_byMixGrpId);
		memcpy(&m_byRecChnnl, pbyBuf, sizeof(m_byRecChnnl));
		pbyBuf += sizeof(m_byRecChnnl);
		memcpy(&m_byPlayChnnl, pbyBuf, sizeof(m_byPlayChnnl));
		pbyBuf += sizeof(m_byPlayChnnl);
		memcpy(&m_byPrsChnnl, pbyBuf, sizeof(m_byPrsChnnl));
		pbyBuf += sizeof(m_byPrsChnnl);
		memcpy(&m_byPrsChnnl2, pbyBuf, sizeof(m_byPrsChnnl2));
		pbyBuf += sizeof(m_byPrsChnnl2);
		memcpy(&m_byPrsChnnlAud, pbyBuf, sizeof(m_byPrsChnnlAud));
		pbyBuf += sizeof(m_byPrsChnnlAud);
		memcpy(&m_byPrsChnnlAudBas, pbyBuf, sizeof(m_byPrsChnnlAudBas));
		pbyBuf += sizeof(m_byPrsChnnlAudBas);
		memcpy(&m_byPrsChnnlVidBas, pbyBuf, sizeof(m_byPrsChnnlVidBas));
		pbyBuf += sizeof(m_byPrsChnnlVidBas);
		memcpy(&m_byPrsChnnlBrBas, pbyBuf, sizeof(m_byPrsChnnlBrBas));
		pbyBuf += sizeof(m_byPrsChnnlBrBas);
		memcpy(&m_byAudBasChnnl, pbyBuf, sizeof(m_byAudBasChnnl));
		pbyBuf += sizeof(m_byAudBasChnnl);
		memcpy(&m_byVidBasChnnl, pbyBuf, sizeof(m_byVidBasChnnl));
		pbyBuf += sizeof(m_byVidBasChnnl);
		memcpy(&m_byBrBasChnnl, pbyBuf, sizeof(m_byBrBasChnnl));
		pbyBuf += sizeof(m_byBrBasChnnl);
		memcpy(&m_byCasdAudBasChnnl, pbyBuf, sizeof(m_byCasdAudBasChnnl));
		pbyBuf += sizeof(m_byCasdAudBasChnnl);
		memcpy(&m_byCasdVidBasChnnl, pbyBuf, sizeof(m_byCasdVidBasChnnl));
		pbyBuf += sizeof(m_byCasdVidBasChnnl);
		memcpy(&m_wVidBasBitrate, pbyBuf, sizeof(m_wVidBasBitrate));
		pbyBuf += sizeof(m_wVidBasBitrate);
		memcpy(&m_wBasBitrate, pbyBuf, sizeof(m_wBasBitrate));
		pbyBuf += sizeof(m_wBasBitrate);
		memcpy(&m_awVMPBrdBitrate, pbyBuf, sizeof(m_awVMPBrdBitrate));
		pbyBuf += sizeof(m_awVMPBrdBitrate);
#ifdef _MINIMCU_
        memcpy(&m_byIsDoubleMediaConf, pbyBuf, sizeof(m_byIsDoubleMediaConf));
        pbyBuf += sizeof(m_byIsDoubleMediaConf);
#endif
//		memcpy(&m_tBasCapSet, pbyBuf, sizeof(m_tBasCapSet));
//		pbyBuf += sizeof(m_tBasCapSet);
		
//		memcpy(&m_byRegGKDriId, pbyBuf, sizeof(m_byRegGKDriId));
//		pbyBuf += sizeof(m_byRegGKDriId);

        memcpy(&m_tRefreshParam, pbyBuf, sizeof(m_tRefreshParam));
		pbyBuf += sizeof(m_tRefreshParam);

		memcpy(&m_tVacLastSpeaker, pbyBuf, sizeof(m_tVacLastSpeaker));
		pbyBuf += sizeof(m_tVacLastSpeaker);
		memcpy(&m_tLastSpeaker, pbyBuf, sizeof(m_tLastSpeaker));
		pbyBuf += sizeof(m_tLastSpeaker);       

		memcpy(&m_tConfInStatus, pbyBuf, sizeof(m_tConfInStatus));
		pbyBuf += sizeof(m_tConfInStatus);

		memcpy(&m_tCascadeMMCU, pbyBuf, sizeof(m_tCascadeMMCU));
		pbyBuf += sizeof(m_tCascadeMMCU);


		memcpy(&m_dwSpeakerViewId, pbyBuf, sizeof(m_dwSpeakerViewId));
		pbyBuf += sizeof(m_dwSpeakerViewId);
		memcpy(&m_dwVmpViewId, pbyBuf, sizeof(m_dwVmpViewId));
		pbyBuf += sizeof(m_dwVmpViewId);
		memcpy(&m_dwSpeakerVideoId, pbyBuf, sizeof(m_dwSpeakerVideoId));
		pbyBuf += sizeof(m_dwSpeakerVideoId);
		memcpy(&m_dwVmpVideoId, pbyBuf, sizeof(m_dwVmpVideoId));
		pbyBuf += sizeof(m_dwVmpVideoId);
		memcpy(&m_dwSpeakerAudioId, pbyBuf, sizeof(m_dwSpeakerAudioId));
		pbyBuf += sizeof(m_dwSpeakerAudioId);
		memcpy(&m_dwMixerAudioId, pbyBuf, sizeof(m_dwMixerAudioId));
		pbyBuf += sizeof(m_dwMixerAudioId);
		memcpy(&m_abySerHdr, pbyBuf, sizeof(m_abySerHdr));
		pbyBuf += sizeof(m_abySerHdr);

		memcpy(&m_byConfIdx, pbyBuf, sizeof(m_byConfIdx));
		pbyBuf += sizeof(m_byConfIdx);
        memcpy(&m_byDcsIdx, pbyBuf, sizeof(m_byDcsIdx));
        pbyBuf += sizeof(m_byDcsIdx);
        
        memcpy(m_abyCasChnCheckTimes, pbyBuf, sizeof(m_abyCasChnCheckTimes));   // guzh [2/25/2007]
		pbyBuf += sizeof(m_abyCasChnCheckTimes);        
        
        memcpy(&m_tLastSpyMt, pbyBuf, sizeof(m_tLastSpyMt));
        pbyBuf += sizeof(m_tLastSpyMt);
		memcpy(&m_tLastVmpParam, pbyBuf, sizeof(m_tLastVmpParam));
		pbyBuf += sizeof(m_tLastVmpParam);
		memcpy(&m_tLastVmpTwParam, pbyBuf, sizeof(m_tLastVmpTwParam));
		pbyBuf += sizeof(m_tLastVmpTwParam);
        memcpy(&m_tChargeSsnId, pbyBuf, sizeof(m_tChargeSsnId));
        pbyBuf += sizeof(m_tChargeSsnId);
        memcpy(&m_abyMixMtId, pbyBuf, sizeof(m_abyMixMtId));
        pbyBuf += sizeof(m_abyMixMtId);
        memcpy(&m_byCreateBy, pbyBuf, sizeof(m_byCreateBy));
        pbyBuf += sizeof(m_byCreateBy);
        
        memcpy(&m_tPollSwitchParam, pbyBuf, sizeof(m_tPollSwitchParam));    // guzh [7/21/2007] 
        pbyBuf += sizeof(m_tPollSwitchParam);
        memcpy(&m_tRollCaller, pbyBuf, sizeof(m_tRollCaller));
        pbyBuf += sizeof(m_tRollCaller);
        memcpy(&m_tRollCallee, pbyBuf, sizeof(m_tRollCallee));
        pbyBuf += sizeof(m_tRollCallee);

        memcpy(&m_tLastMixParam, pbyBuf, sizeof(m_tLastMixParam));
        pbyBuf += sizeof(m_tLastMixParam);

		memcpy(&m_bySpeakerVidSrcSpecType, pbyBuf, sizeof(m_bySpeakerVidSrcSpecType));	//zgc, 2008-04-16
		pbyBuf += sizeof(m_bySpeakerVidSrcSpecType);
		memcpy(&m_bySpeakerAudSrcSpecType, pbyBuf, sizeof(m_bySpeakerAudSrcSpecType));
		pbyBuf += sizeof(m_bySpeakerAudSrcSpecType);

        memcpy(&m_cMtRcvGrp, pbyBuf, sizeof(m_cMtRcvGrp));
		pbyBuf += sizeof(m_cMtRcvGrp);
        memcpy(&m_cBasMgr, pbyBuf, sizeof(m_cBasMgr));
        pbyBuf += sizeof(m_cBasMgr);

		memcpy(&m_byLastDsSrcMtId, pbyBuf,sizeof(m_byLastDsSrcMtId));	// xliang [12/19/2008] ˫����������
		pbyBuf += sizeof(m_byLastDsSrcMtId);
		memcpy(&m_tLogicChnnl, pbyBuf,sizeof(m_tLogicChnnl));
		pbyBuf += sizeof(m_tLogicChnnl);

		memcpy(&m_byMtIdNotInvite,pbyBuf,sizeof(m_byMtIdNotInvite)); // xliang [12/26/2008] 
		pbyBuf += sizeof(m_byMtIdNotInvite);

		// xliang [5/6/2009] prs �����Ķ�Ӧ�㲥Դ4����ͨ��
		memcpy(&m_byPrsChnnlVmpOut1, pbyBuf, sizeof(u8));
		pbyBuf += sizeof(u8);
		memcpy(&m_byPrsChnnlVmpOut2, pbyBuf, sizeof(u8));
		pbyBuf += sizeof(u8);
		memcpy(&m_byPrsChnnlVmpOut3, pbyBuf, sizeof(u8));
		pbyBuf += sizeof(u8);
		memcpy(&m_byPrsChnnlVmpOut4, pbyBuf, sizeof(u8));
		pbyBuf += sizeof(u8);

		memcpy(&m_tHduBatchPollInfo, pbyBuf, sizeof(m_tHduBatchPollInfo) );    // add by jlb
		pbyBuf += sizeof(m_tHduBatchPollInfo);
		memcpy(&m_tHduPollSchemeInfo, pbyBuf, sizeof(m_tHduPollSchemeInfo) );
		pbyBuf += sizeof(m_tHduPollSchemeInfo);
		
		//vmp related
		memcpy( &m_tVmpChnnlInfo, pbyBuf,sizeof(m_tVmpChnnlInfo)); 
		pbyBuf += sizeof(m_tVmpChnnlInfo);
		memcpy(&m_atVMPTmpMember, pbyBuf, sizeof(m_atVMPTmpMember));
		pbyBuf += sizeof(m_atVMPTmpMember);
		memcpy(&m_bNewMemberSeize, pbyBuf, sizeof(m_bNewMemberSeize));
		pbyBuf += sizeof(m_bNewMemberSeize);
		memcpy(&m_tLastVmpChnnlMemInfo, pbyBuf, sizeof(m_tLastVmpChnnlMemInfo));
		pbyBuf += sizeof(m_tLastVmpChnnlMemInfo);
		memcpy(&m_tVmpBatchPollInfo, pbyBuf, sizeof(m_tVmpBatchPollInfo));
		pbyBuf += sizeof(m_tVmpBatchPollInfo);
		memcpy(&m_tVmpPollParam, pbyBuf, sizeof(m_tVmpPollParam));
		pbyBuf += sizeof(m_tVmpPollParam);
		memcpy(&m_byVmpOperating, pbyBuf, sizeof(m_byVmpOperating));
		pbyBuf += sizeof(m_byVmpOperating);

		memcpy(&m_cVCSConfStatus, pbyBuf, sizeof(m_cVCSConfStatus));
		pbyBuf += sizeof(m_cVCSConfStatus);
	}

	return dwOtherDataLen;
}

/*=============================================================================
    �� �� ���� GetInstOtherDataLen
    ��    �ܣ� �������ݵ���ʱ ��ȡ ��������̬������б���Ϣ���������ͬ�� �����ܳ���
	           ����ͬ��������MCUӦ�û���һ�£�ͬ�������ݽṹ�ݲ����ǽ���ѹջ���⣺Pack Ingore
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� u32  �����ܳ���
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
u32 CMcuVcInst::GetInstOtherDataLen( void )
{
	u32 dwOtherDataLen = 0;

	//10
	dwOtherDataLen += sizeof(m_tConf);
	dwOtherDataLen += sizeof(m_tConfAllMtInfo);
	dwOtherDataLen += sizeof(m_tConfProtectInfo);
	dwOtherDataLen += sizeof(m_tConfPollParam);
	dwOtherDataLen += sizeof(m_tTvWallPollParam);
	dwOtherDataLen += sizeof(m_tHduPollParam);      //add by jlb
	dwOtherDataLen += sizeof(m_tConfEqpModule);
	dwOtherDataLen += sizeof(m_tVidBrdSrc);
	dwOtherDataLen += sizeof(m_tAudBrdSrc);
	dwOtherDataLen += sizeof(m_tDoubleStreamSrc);
	dwOtherDataLen += sizeof(m_tH239TokenOwnerInfo);
	
	//13
	dwOtherDataLen += sizeof(m_tVmpEqp);
	dwOtherDataLen += sizeof(m_tVmpTwEqp);
	dwOtherDataLen += sizeof(m_tMixEqp);
	dwOtherDataLen += sizeof(m_tRecEqp);
	dwOtherDataLen += sizeof(m_tPlayEqp);
	dwOtherDataLen += sizeof(m_tPlayEqpAttrib);
	dwOtherDataLen += sizeof(m_tRecPara);
	dwOtherDataLen += sizeof(m_tAudBasEqp);
	dwOtherDataLen += sizeof(m_tVidBasEqp);
	dwOtherDataLen += sizeof(m_tBrBasEqp);
	dwOtherDataLen += sizeof(m_tCasdAudBasEqp);
	dwOtherDataLen += sizeof(m_tCasdVidBasEqp);
	dwOtherDataLen += sizeof(m_tPrsEqp);
	
	//17
	dwOtherDataLen += sizeof(m_byMixGrpId);
	dwOtherDataLen += sizeof(m_byRecChnnl);
	dwOtherDataLen += sizeof(m_byPlayChnnl);
	dwOtherDataLen += sizeof(m_byPrsChnnl);
	dwOtherDataLen += sizeof(m_byPrsChnnl2);
	dwOtherDataLen += sizeof(m_byPrsChnnlAud);
	dwOtherDataLen += sizeof(m_byPrsChnnlAudBas);
	dwOtherDataLen += sizeof(m_byPrsChnnlVidBas);
	dwOtherDataLen += sizeof(m_byPrsChnnlBrBas);
	dwOtherDataLen += sizeof(m_byAudBasChnnl);
	dwOtherDataLen += sizeof(m_byVidBasChnnl);
	dwOtherDataLen += sizeof(m_byBrBasChnnl);
	dwOtherDataLen += sizeof(m_byCasdAudBasChnnl);
	dwOtherDataLen += sizeof(m_byCasdVidBasChnnl);
	dwOtherDataLen += sizeof(m_wVidBasBitrate);
	dwOtherDataLen += sizeof(m_wBasBitrate);
	dwOtherDataLen += sizeof(m_awVMPBrdBitrate);
#ifdef _MINIMCU_
    dwOtherDataLen += sizeof(m_byIsDoubleMediaConf);
#endif
//	dwOtherDataLen += sizeof(m_tBasCapSet);
	
	//14
//	dwOtherDataLen += sizeof(m_byRegGKDriId);

    dwOtherDataLen += sizeof(m_tRefreshParam);

	dwOtherDataLen += sizeof(m_tVacLastSpeaker);
	dwOtherDataLen += sizeof(m_tLastSpeaker);
	dwOtherDataLen += sizeof(m_tConfInStatus);
	dwOtherDataLen += sizeof(m_tCascadeMMCU);
	
	//12
	dwOtherDataLen += sizeof(m_dwSpeakerViewId);
	dwOtherDataLen += sizeof(m_dwVmpViewId);
	dwOtherDataLen += sizeof(m_dwSpeakerVideoId);
	dwOtherDataLen += sizeof(m_dwVmpVideoId);
	dwOtherDataLen += sizeof(m_dwSpeakerAudioId);
	dwOtherDataLen += sizeof(m_dwMixerAudioId);
	dwOtherDataLen += sizeof(m_abySerHdr);
    
	dwOtherDataLen += sizeof(m_byConfIdx);
    dwOtherDataLen += sizeof(m_byDcsIdx);
    
    dwOtherDataLen += sizeof(m_abyCasChnCheckTimes);    // guzh [2/25/2007]
    dwOtherDataLen += sizeof(m_tLastSpyMt);
	dwOtherDataLen += sizeof(m_tLastVmpParam);
	dwOtherDataLen += sizeof(m_tLastVmpTwParam);
    dwOtherDataLen += sizeof(m_tChargeSsnId);
    dwOtherDataLen += sizeof(m_abyMixMtId);
    dwOtherDataLen += sizeof(m_byCreateBy);

    dwOtherDataLen += sizeof(m_tPollSwitchParam);       // guzh [7/21/2007] 
    dwOtherDataLen += sizeof(m_tRollCaller);
    dwOtherDataLen += sizeof(m_tRollCallee);
    dwOtherDataLen += sizeof(m_tLastMixParam);

	dwOtherDataLen += sizeof(m_bySpeakerVidSrcSpecType);	// zgc, 2008-04-17
	dwOtherDataLen += sizeof(m_bySpeakerAudSrcSpecType);

    dwOtherDataLen += sizeof(m_cMtRcvGrp);
    dwOtherDataLen += sizeof(m_cBasMgr);

	dwOtherDataLen += sizeof(m_byLastDsSrcMtId);		// xliang [12/19/2008] ˫����������
	dwOtherDataLen += sizeof(m_tLogicChnnl);
	
	dwOtherDataLen += sizeof(m_byMtIdNotInvite);		// xliang [12/26/2008]
	
	dwOtherDataLen += sizeof(m_byPrsChnnlVmpOut1);
	dwOtherDataLen += sizeof(m_byPrsChnnlVmpOut2);
	dwOtherDataLen += sizeof(m_byPrsChnnlVmpOut3);
	dwOtherDataLen += sizeof(m_byPrsChnnlVmpOut4);
	dwOtherDataLen += sizeof(m_tHduBatchPollInfo);        // add by jlb
	dwOtherDataLen += sizeof(m_tHduPollSchemeInfo);       // add by jlb

	dwOtherDataLen += sizeof(m_tVmpChnnlInfo);
	dwOtherDataLen += sizeof(m_atVMPTmpMember);
	dwOtherDataLen += sizeof(m_bNewMemberSeize);
	dwOtherDataLen += sizeof(m_tLastVmpChnnlMemInfo);
	dwOtherDataLen += sizeof(m_tVmpBatchPollInfo);
	dwOtherDataLen += sizeof(m_tVmpPollParam);
	dwOtherDataLen += sizeof(m_byVmpOperating);

	dwOtherDataLen += sizeof(m_cVCSConfStatus);
	
	return dwOtherDataLen;
}

/*=============================================================================
  �� �� ���� ProcWaitEqpToCreateConf
  ��    �ܣ� �ȴ��������ߴ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/02/25  4.0			������                  ����
=============================================================================*/
void CMcuVcInst::ProcWaitEqpToCreateConf(CMessage * const pcMsg)
{
    if ( STATE_WAITEQP != CurState() &&
		 STATE_PERIOK != CurState() )
    {
        return;
    }
	TConfStore tConfStoreBuf;
	TPackConfStore *ptPackConfStore = (TPackConfStore *)&tConfStoreBuf;		
	u16 wAliasBufLen = 0;
	u8* pbyAliasBuf  = NULL; // �ն˱�������������
	u8* pbyModuleBuf = NULL; // ����ǽ//����ϳ�ģ�建��
	u8 byInsID = 0;

	u8 byConfIdx = (u8)(*(u32*)pcMsg->content);
	// ��ȡ��� tConfStore Ϊ TPackConfStore �ṹ�����ѽ���Pack����Ļ�������
	BOOL32 bRet = g_cMcuVcApp.HasConfStore( byConfIdx-MIN_CONFIDX );
    BOOL32 bRet1  =  ::GetConfFromFile(byConfIdx-MIN_CONFIDX, ptPackConfStore);
	if (bRet && bRet1)
	{
		wAliasBufLen = htons(ptPackConfStore->m_wAliasBufLen);
		pbyAliasBuf  = (u8 *)(ptPackConfStore+1);
		pbyModuleBuf = pbyAliasBuf+ptPackConfStore->m_wAliasBufLen;

        // guzh [4/11/2007] ��Ǵ��ļ�����
        ptPackConfStore->m_tConfInfo.m_tStatus.SetTakeFromFile(TRUE);
		
		CServMsg cServMsg;
		cServMsg.SetConfIdx( (u8)byConfIdx ); // confidx
		cServMsg.SetMsgBody((u8*)&ptPackConfStore->m_tConfInfo, sizeof(TConfInfo));
		cServMsg.CatMsgBody((u8*)&wAliasBufLen, 2 );
		cServMsg.CatMsgBody(pbyAliasBuf, ntohs(wAliasBufLen) );

		// ����ǽģ�� 
		if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
		{
			cServMsg.CatMsgBody(pbyModuleBuf, sizeof(TMultiTvWallModule));
			pbyModuleBuf += sizeof(TMultiTvWallModule);
		}
		// ����ϳ�ģ�� 
		if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasVmpModule())
		{
			cServMsg.CatMsgBody(pbyModuleBuf, sizeof(TVmpModule));
			pbyModuleBuf += sizeof(TVmpModule);
		}
		
        ::OspPost(MAKEIID( AID_MCU_VC, GetInsID() ), MCU_CREATECONF_FROMFILE, 
                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

	}
	McsLog("[Conf%d] Wait for eqp to create conference: %s\n", 
            GetInsID(), tConfStoreBuf.m_tConfInfo.GetConfName() );

	return;
}

/*=============================================================================
  �� �� ���� DaemonProcMcuRegGKFailedNtf
  ��    �ܣ� Mcu ����ע��GK ֪ͨ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CApp* pApp
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::DaemonProcMcuReRegisterGKNtf( const CMessage * pcMsg )
{
    m_tConfInStatus.SetRegGkNackNtf(TRUE);
    return;
}

/*=============================================================================
  �� �� ���� DaemonProcPowerOn
  ��    �ܣ� ʵ����ʼ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CApp* pApp
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::DaemonProcPowerOn( const CMessage * pcMsg )
{
	if (!g_cMcuVcApp.CreateTemplate())
	{
		OspPrintf(TRUE, FALSE, "[ProcReoPowerOn] allocate memory failed for conf template!!!\n");
		return;
	}

	TConfStore tConfStoreBuf;
	TPackConfStore *ptPackConfStore = (TPackConfStore *)&tConfStoreBuf;
	u8 byConfStoreNum = 0;

    // guzh [4/10/2007] Ҫ���ϵ�MakeTimee�ָ�,��֤CConfId����ʷΨһ��
    u32 dwOldConfIdMakeTimes = 0;
    u32 dwTmp;
	u8 byLoop = 0;
    CConfId cConfId;
	for (byLoop = 0; byLoop < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); byLoop++)
	{
		//��ȡ��� tConfStore Ϊ TPackConfStore �ṹ�����ѽ���Pack����Ļ�������
        if (::GetConfFromFile(byLoop, ptPackConfStore))
		{
			byConfStoreNum++;
			g_cMcuVcApp.SetConfStore(byLoop, TRUE);	
            
            cConfId = ptPackConfStore->m_tConfInfo.GetConfId();
            dwTmp = g_cMcuVcApp.GetMakeTimesFromConfId(cConfId);
            if (dwTmp > dwOldConfIdMakeTimes) 
            {
                dwOldConfIdMakeTimes = dwTmp;
            }
		}
	}
    g_cMcuVcApp.SetConfIdMakeTimes(dwOldConfIdMakeTimes);

    // xsl [11/28/2006] N+1 ����ģʽ�����л���ָ� 
    if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_IDLE ||
        g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_SWITCH)
    {        
        return;
    }
    
	//�������Զ��ָ��Ļ���Idx�б�����ģ�壩��������MP��ⶨʱ��
	if( byConfStoreNum > 0 )
	{
        // guzh [4/13/2007] ���ñ�Ǳ�ʾ��MCU������һ�λָ�
		SetTimer(MCUVC_WAIT_MPREG_TIMER, TIMESPACE_WAIT_MPREG, TRUE);
	}

	//���ǻ�����Ϣ ��ʱˢ������
	SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );

	//MCU MODEMע��
	for (byLoop =1 ; byLoop <= g_byMcuModemNum ; byLoop ++ )
	{
		TTransportAddr tAddr;;
		tAddr.SetIpAddr(g_atMcuModemTopo[byLoop-1].m_dwMcuModemIp );
		tAddr.SetPort( g_atMcuModemTopo[byLoop-1].m_wMcuModemPort );
		
		TMt tMt;
		tMt.SetNull();	//MCU���Ƶ�modem

		u8 byType = g_atMcuModemTopo[byLoop-1].m_byMcuModemType;

		CServMsg cServMsg;
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
		cServMsg.CatMsgBody( (u8*)&tAddr, sizeof(TTransportAddr) );
		cServMsg.CatMsgBody( &byType, sizeof(u8) );
		g_cModemSsnApp.SendMsgToModemSsn( byLoop, MODEM_MCU_REG_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	}

	return;
}

/*=============================================================================
�� �� ���� GetNPlusConfData
��    �ܣ� �ӻ�����Ϣ��ȡN+1������Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TNPlusConfData &tConfData
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/30  4.0			������                  ����
=============================================================================*/
void CMcuVcInst::ProcNPlusConfDataUpdate(const CMessage * pcMsg)
{
    if ( CurState() != STATE_ONGOING )
    {
        return;
    }

    // N+1���ݣ���֧��ԤԼ����ı��ݵ��� [12/20/2006-zbq]
    if ( m_tConf.m_tStatus.IsScheduled() )
    {
        ConfLog( FALSE, "[ProcNPlusConfDataUpdate] conf.%s will not be backup as a scheduled conf !\n",
                         m_tConf.GetConfName() );
        return;
    }
    
    TNPlusConfData tConfData;
    GetNPlusDataFromConf( tConfData );

    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_CONFDATAUPDATE_REQ);
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8*)&tConfData, sizeof(tConfData));    
    
    //��Ϊ��mcu��srcidΪdaemonʵ������Ϊ����mcu�ع�ģʽ��srcidΪ����ʵ��
    OspPost(pcMsg->srcid, VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

    //��Ϊ�ع�ģʽ����Դʵ������daemon���������ݻ���
    if (GETINS(pcMsg->srcid) != CInstance::DAEMON)
    {
        NPlusLog("[ProcNPlusConfDataUpdate] conf:%s rollback and release\n", m_tConf.GetConfName());
        ReleaseConf(FALSE);        
        NEXTSTATE( STATE_IDLE );
    }
    return;
}

/*=============================================================================
�� �� ���� GetNPlusDataFromConf
��    �ܣ� �ӻ�����N+1����������Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TNPlusConfData &tNPlusData
           BOOL32          bCharge   :��ԼƷ�ͬ����Ϣ�Ĵ�����Ϊ�Ʒ�ͬ������
                                      ������ ר�����N+1�Ĵ����֡����򣬻ᵼ
                                      ��ʱ�����⡣Ӱ��N+1�ı��ݽ����zbq [03/27/2007]

�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/12/26  4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::GetNPlusDataFromConf( TNPlusConfData &tConfData, BOOL32 bCharge )
{
    memcpy(&tConfData.m_tConf, &m_tConf, sizeof(TConfInfo));
    
    TMtAlias tTmpAlias;
    TMt tTmpMt = m_tConf.GetChairman();    
    if (!tTmpMt.IsNull())
    {
        if (!m_ptMtTable->GetMtAlias(tTmpMt.GetMtId(), mtAliasTypeTransportAddress, &tTmpAlias))
        {
            ConfLog(FALSE, "[GetNPlusDataFromConf] get chairman ip addr alias failed.\n");            
        }        
    }
    tConfData.m_tConf.SetChairAlias(tTmpAlias);
    
    tTmpAlias.SetNull();
    tTmpMt = m_tConf.GetSpeaker();
    if (!tTmpMt.IsNull())
    {
        if (!m_ptMtTable->GetMtAlias(tTmpMt.GetMtId(), mtAliasTypeTransportAddress, &tTmpAlias))
        {
            ConfLog(FALSE, "[GetNPlusDataFromConf] get speaker ip addr alias failed.\n");            
        }       
    }
    tConfData.m_tConf.SetSpeakerAlias(tTmpAlias);
    
    u8 byIdx = 0;
    u16 wDialBitrate;
    for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
    {
        //ֻ�������������������ն�
        if (!m_tConfAllMtInfo.MtJoinedConf(byMtId) || m_ptMtTable->IsNotInvited(byMtId))
        {
            continue;
        }
        
        if (m_ptMtTable->GetMtAlias(byMtId, mtAliasTypeTransportAddress, &tTmpAlias))
        {
            tConfData.m_atMtInConf[byIdx].SetMtAddr(tTmpAlias.m_tTransportAddr);
            wDialBitrate = m_ptMtTable->GetDialBitrate(byMtId);
            tConfData.m_atMtInConf[byIdx].SetCallBitrate(wDialBitrate);            
            byIdx++;
        }
        else
        {
            ConfLog(FALSE, "[GetNPlusDataFromConf] get mt.%d ip addr alias failed.\n", byMtId);
        }
        tConfData.m_byMtNum = byIdx;
    }
    
    if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE)
    {
        BOOL32 bHasVMPMem = FALSE;
        
        TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
        for (byIdx = 0; byIdx < tVmpParam.GetMaxMemberNum(); byIdx++)
        {
            TVMPMember *ptMemMt = tVmpParam.GetVmpMember(byIdx);
            if (!m_tConfAllMtInfo.MtJoinedConf(ptMemMt->GetMtId()) || m_ptMtTable->IsNotInvited(ptMemMt->GetMtId()))
            {
                continue;
            }
            
            if (m_ptMtTable->GetMtAlias(ptMemMt->GetMtId(), mtAliasTypeTransportAddress, &tTmpAlias))
            {
                tConfData.m_tVmpInfo.m_atMtInVmp[byIdx].SetMtAddr(tTmpAlias.m_tTransportAddr); 
                tConfData.m_tVmpInfo.m_abyMemberType[byIdx] = ptMemMt->GetMemberType();
                
                bHasVMPMem = TRUE;
            }
            else
            {
                ConfLog(FALSE, "[GetNPlusDataFromConf] get mt.%d ip addr alias failed.\n", tTmpMt.GetMtId());
            }
        }
        // zbq [03/27/2007] �Ʒ���Ϣ��ͬ����������N+1�����⴦��
        if ( !bCharge )
        {
            // ������ϳɵ�ǰû�г�Ա���򲻻ع� �ջ���ϳ�ģʽ [12/14/2006-zbq]
            if ( !bHasVMPMem )
            {
                tConfData.m_tConf.m_tStatus.SetVMPMode( CONF_VMPMODE_NONE );
            }
            m_tConf.SetHasVmpModule( bHasVMPMem );
        }

        // ������ϳ�ģʽΪ�Զ��ϳɣ���ģ��Ϊ�� [01/11/2007-zbq]
        if ( CONF_VMPMODE_AUTO == tConfData.m_tConf.m_tStatus.GetVMPMode() ) 
        {
            tConfData.m_tConf.SetHasVmpModule( FALSE );
        }
    }
    
    // ������Ʒѣ�����ؼƷ�SessionId [12/26/2006-zbq]
    if ( m_tConf.IsSupportGkCharge() )
    {
        memcpy(&tConfData.m_tSsnId, &m_tChargeSsnId, sizeof(m_tChargeSsnId));
    }
    
    return;
}

/*=============================================================================
�� �� ���� ProcNPlusConfInfoUpdate
��    �ܣ� N+1���ݻ�����Ϣͬ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
=============================================================================*/
void CMcuVcInst::ProcNPlusConfInfoUpdate(BOOL32 bStart)
{
    u8 byMode = bStart ? NPLUS_CONF_START : NPLUS_CONF_RELEASE;
    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_CONFINFOUPDATE_REQ);
    cServMsg.SetMsgBody(&byMode, sizeof(byMode));
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.CatMsgBody((u8*)&m_tConf, sizeof(m_tConf));
    g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

    return;
}

/*=============================================================================
�� �� ���� ProcNPlusConfMtInfoUpdate
��    �ܣ� N+1�����ն���Ϣͬ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
=============================================================================*/
void CMcuVcInst::ProcNPlusConfMtInfoUpdate(void)
{
    TMtInfo atMtInConf[MAXNUM_CONF_MT];
    u8 byIdx = 0;
    u16 wDialBitrate;
    TMtAlias tTmpAlias;

    for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
    {
        //ֻ���� �Ѿ��������ķ��ϼ�MCU �ն� [12/20/2006-zbq]
        if ( !m_tConfAllMtInfo.MtJoinedConf(byMtId) || 
             MT_TYPE_MMCU == m_ptMtTable->GetMt(byMtId).GetMtType() )
        {
            continue;
        }

        if (m_ptMtTable->GetMtAlias(byMtId, mtAliasTypeTransportAddress, &tTmpAlias))
        {          
            atMtInConf[byIdx].SetMtAddr(tTmpAlias.m_tTransportAddr);
            wDialBitrate = m_ptMtTable->GetDialBitrate(byMtId);
            atMtInConf[byIdx].SetCallBitrate(wDialBitrate);            
            byIdx++;
        }
        else
        {
            ConfLog(FALSE, "[ProcNPlusConfMtInfoUpdate] get mt.%d ip addr alias failed.\n", byMtId);
        }
    }

    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_CONFMTUPDATE_REQ);
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8*)atMtInConf, sizeof(TMtInfo)*byIdx);
    g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

    return;
}

/*=============================================================================
�� �� ���� ProcNPlusChairmanUpdate
��    �ܣ� N+1������ϯͬ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
=============================================================================*/
void CMcuVcInst::ProcNPlusChairmanUpdate(void)
{
    TMtAlias tTmpAlias;
    TMt tTmpMt = m_tConf.GetChairman();  
    if (!tTmpMt.IsNull())
    {
        if (!m_ptMtTable->GetMtAlias(tTmpMt.GetMtId(), mtAliasTypeTransportAddress, &tTmpAlias))        
        {
            ConfLog(FALSE, "[ProcNPlusChairmanUpdate] get chairman ip addr alias failed.\n");
        }
    }

    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_CHAIRUPDATE_REQ);
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8*)&tTmpAlias, sizeof(tTmpAlias));
    g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

    return;
}

/*=============================================================================
�� �� ���� ProcNPlusSpeakerUpdate
��    �ܣ� N+1���ݷ�����ͬ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
=============================================================================*/
void CMcuVcInst::ProcNPlusSpeakerUpdate(void)
{
    TMtAlias tTmpAlias;
    TMt tTmpMt = m_tConf.GetSpeaker();  
    if (!tTmpMt.IsNull())
    {
        if (!m_ptMtTable->GetMtAlias(tTmpMt.GetMtId(), mtAliasTypeTransportAddress, &tTmpAlias))
        {
            ConfLog(FALSE, "[ProcNPlusSpeakerUpdate] get speaker ip addr alias failed.\n");
        }
    }

    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_SPEAKERUPDATE_REQ);
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8*)&tTmpAlias, sizeof(tTmpAlias));
    g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

    return;
}

/*=============================================================================
�� �� ���� ProcNPlusVmpUpdate
��    �ܣ� N+1����vmp��Ϣͬ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
=============================================================================*/
void CMcuVcInst::ProcNPlusVmpUpdate(void)
{ 
    u8 byIdx = 0;
    TMtAlias tTmpAlias;
    TNPlusVmpInfo tVmpInfo;  
    TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
    for (byIdx = 0; byIdx < tVmpParam.GetMaxMemberNum(); byIdx++)
    {
        TVMPMember *ptMemMt = tVmpParam.GetVmpMember(byIdx);
        if (!m_tConfAllMtInfo.MtJoinedConf(ptMemMt->GetMtId()) || m_ptMtTable->IsNotInvited(ptMemMt->GetMtId()))
        {
            continue;
        }

        if (m_ptMtTable->GetMtAlias(ptMemMt->GetMtId(), mtAliasTypeTransportAddress, &tTmpAlias))
        {
            tVmpInfo.m_atMtInVmp[byIdx].SetMtAddr(tTmpAlias.m_tTransportAddr);                        
            tVmpInfo.m_abyMemberType[byIdx] = ptMemMt->GetMemberType();
        }
        else
        {
            ConfLog(FALSE, "[ProcNPlusVmpUpdate] get mt.%d ip addr alias failed.\n", ptMemMt->GetMtId());
        }
    }

    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_VMPUPDATE_REQ);
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8*)&tVmpInfo, sizeof(tVmpInfo)); 
    cServMsg.CatMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
    g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

    return;
}

/*=============================================================================
  �� �� ���� AdjustRecordSrcStream
  ��    �ܣ� ¼��� ¼���� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8    byMode   : MODE_VIDEO, MODE_AUDIO, MODE_SECVIDEO
  �� �� ֵ�� void 
  ---------------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/06/07    4.0			�ű���                  ����
  2007/06/12    4.0         ����                  �޸�
=============================================================================*/
void CMcuVcInst::AdjustRecordSrcStream( u8 byMode )
{
    TMt tNewSrc;
    u8  byRecChnlIdx = 0;
    // ����Ը��������ʱ��ܣ��Ժ���Ҫ�����ж��Ƿ���Ҫ������������, zgc, 2008-08-28
    BOOL32 bNeedBas = IsRecordSrcBas(byMode, tNewSrc, byRecChnlIdx);

    if (tNewSrc.IsNull())
    {
        return;
    }

    TSimCapSet tConfMainSCS = m_tConf.GetCapSupport().GetMainSimCapSet();   
    TSimCapSet tSrcMtSCS;
    if ( tNewSrc.GetType() == TYPE_MT )
    {
        tSrcMtSCS = m_ptMtTable->GetSrcSCS(tNewSrc.GetMtId());
    }
    else
    {
		// 2008-01-17, zgc, ˫��˫��ʽ���鸨��ʽ����¼��ʧ���޸ģ���R4 MP4�ϲ�
        if (!bNeedBas)
        {
            // ֱ��Ĭ������ʽ��������
            tSrcMtSCS.SetAudioMediaType( tConfMainSCS.GetAudioMediaType() );
            tSrcMtSCS.SetVideoMediaType( tConfMainSCS.GetVideoMediaType() );
            tSrcMtSCS.SetVideoResolution( tConfMainSCS.GetVideoResolution() );    
        }
        else
        {
            TMt tSpeaker = GetLocalSpeaker();
            if ( tSpeaker.GetType() == TYPE_MT )
            {
                tSrcMtSCS = m_ptMtTable->GetSrcSCS(tSpeaker.GetMtId());
            }
            else
            {
                // ���ڷ�������ֱ��Ĭ������ʽ��������
                tSrcMtSCS.SetAudioMediaType( tConfMainSCS.GetAudioMediaType() );
                tSrcMtSCS.SetVideoMediaType( tConfMainSCS.GetVideoMediaType() );
                tSrcMtSCS.SetVideoResolution( tConfMainSCS.GetVideoResolution() );    
            }
        }
    }

    if ( MODE_AUDIO == byMode )
    {
        if ( bNeedBas )
        {
            if (m_tConf.m_tStatus.IsAudAdapting())
            {
                ChangeAdapt(ADAPT_TYPE_AUD, 0, &tConfMainSCS, &tSrcMtSCS);
                StartSwitchToPeriEqp( tNewSrc, m_byAudBasChnnl, 
                    m_tRecEqp.GetEqpId(), m_byRecChnnl, byMode);
            }
            else
            {
                StartAdapt(ADAPT_TYPE_AUD, 0, &tConfMainSCS, &tSrcMtSCS);
            }      
        }
        else if ( tNewSrc == m_tMixEqp )
        {
            // ��������ģʽ ¼ ������
            StartSwitchToPeriEqp(tNewSrc, m_byMixGrpId, m_tRecEqp.GetEqpId(),
                                 m_byRecChnnl, byMode);
        }
        else
        {
            u8 bySrcChan = (tNewSrc == m_tPlayEqp ? m_byPlayChnnl : 0);
            StartSwitchToPeriEqp(tNewSrc, bySrcChan, m_tRecEqp.GetEqpId(), 
                                 m_byRecChnnl, byMode);
        }
    }
    
    if ( MODE_VIDEO == byMode )
    {
        if ( bNeedBas)
        {
            if ( tNewSrc == m_tVidBasEqp )
            {
                
                if (m_tConf.m_tStatus.IsVidAdapting())
                {
					if( m_tRecPara.IsRecLowStream() )
					{
                        // guzh [6/18/2007] FIXME: �Ƿ�ᱻ�����changeadapt���ߣ�
						ChangeAdapt(ADAPT_TYPE_VID, m_tConf.GetSecBitRate(), &tConfMainSCS, &tSrcMtSCS);
					}
                    StartSwitchToPeriEqp( m_tVidBasEqp, m_byVidBasChnnl, 
                                          m_tRecEqp.GetEqpId(), m_byRecChnnl, byMode );
                }
                else
                {
                    u16 wBitrate = m_tRecPara.IsRecLowStream() ? m_tConf.GetSecBitRate() : m_tConf.GetBitRate();
                    StartAdapt(ADAPT_TYPE_VID, wBitrate, &tConfMainSCS, &tSrcMtSCS);
                }
            }
            else if ( tNewSrc == m_tBrBasEqp )
            {
                if (m_tConf.m_tStatus.IsBrAdapting())
                {
                    ChangeAdapt(ADAPT_TYPE_BR, m_tConf.GetSecBitRate());
                    StartSwitchToPeriEqp( m_tBrBasEqp, m_byBrBasChnnl, 
                                          m_tRecEqp.GetEqpId(), m_byRecChnnl, byMode );                
                }
                else
                {
                    StartAdapt(ADAPT_TYPE_BR, m_tConf.GetSecBitRate());
                }
            }
			else
			{
				tNewSrc.SetConfIdx(m_byConfIdx);
				StartSwitchToPeriEqp(tNewSrc, byRecChnlIdx,
					                 m_tRecEqp.GetEqpId(), m_byRecChnnl, byMode);
			}
        }
        else
        {
            u8 bySrcChan = (tNewSrc == m_tPlayEqp ? m_byPlayChnnl : 0);
			if (tNewSrc == m_tVmpEqp)	// xliang [6/24/2009] ����ϳ���srcChan��������Ϊ0
			{
				u8 byMediaType = m_tConf.GetMainVideoMediaType();
				u8 byRes = m_tConf.GetMainVideoFormat();
				bySrcChan = GetVmpOutChnnlByRes(byRes, m_tVmpEqp.GetEqpId(), byMediaType);
			}
            StartSwitchToPeriEqp(tNewSrc, bySrcChan, m_tRecEqp.GetEqpId(), 
                                 m_byRecChnnl, byMode);
        }
    }

    if ( MODE_SECVIDEO == byMode )
    {
        if (m_tRecPara.IsRecDStream() && !m_tDoubleStreamSrc.IsNull())
        {
            u8 bySrcChnnl = (m_tDoubleStreamSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
            StartSwitchToPeriEqp(m_tDoubleStreamSrc, bySrcChnnl, m_tRecEqp.GetEqpId(), m_byRecChnnl, MODE_SECVIDEO); 
        }
    }

    return;

}

/*=============================================================================
  �� �� ���� NotifyMtFastUpdate
  ��    �ܣ� ����VCU��ֻ�ܷ��͸�ָ���ն�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� byMode:      MODE_VIDEO �� MODE_SECVIDEO
  �� �� ֵ�� void 
  ---------------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/07/27    4.0         ����                  �޸�
=============================================================================*/
void CMcuVcInst::NotifyMtFastUpdate( u8 byMtId, u8 byMode, BOOL32 bSetTimer )
{    
    u32 dwCurTick = OspTickGet();
	//xliang [080802] add print for vga ds test
	u32 dwCurTickInterval = dwCurTick - m_ptMtTable->GetLastTick(byMtId,(byMode==MODE_SECVIDEO));

	CallLog("[NotifyMtFastUpdate] the MtId is: %u, the video mode is: %u \n",byMtId,byMode);
	CallLog("[NotifyMtFastUpdate] the cur tick interval is: %u, the VCU interval is: %u\n",dwCurTickInterval,g_dwVCUInterval);
    if ( (byMode == MODE_VIDEO && dwCurTick - m_ptMtTable->GetLastTick(byMtId) > g_dwVCUInterval)
         || byMode == MODE_SECVIDEO )
    {
        CServMsg cServMsg;
        cServMsg.SetMsgBody(&byMode, sizeof(u8));
        
        //tandberg�����״�����
        if(MT_MANU_TAIDE == m_ptMtTable->GetManuId(byMtId))
        {
            bSetTimer = TRUE;
            CallLog("[NotifyMtFastUpdate] the MtId.%u Fastupdate has been delayed due to MANU.Tandberg\n",byMtId);
        }
        else
        {
            SendMsgToMt(byMtId, MCU_MT_FASTUPDATEPIC_CMD, cServMsg);
        }
        m_ptMtTable->SetLastTick(byMtId, dwCurTick, (byMode==MODE_SECVIDEO));
    }
    // ���ö�ʱ�����ȶ�ʱ����ǿ������
    if (bSetTimer)
    {
        if (MODE_VIDEO == byMode)
        {
            SetTimer( MCUVC_FASTUPDATE_TIMER_ARRAY+byMtId, 1200, 100*byMtId+1 );
        }
        else if (MODE_SECVIDEO == byMode)
        {
            SetTimer( MCUVC_SECVIDEO_FASTUPDATE_TIMER, 1200, 100*byMtId+1 );
        }
        else
        {
            ConfLog( FALSE, "[NotifyMtFastUpdate] unexpected mode.%d, ignore it\n", byMode );
        }
    }
}

/*=============================================================================
  �� �� ���� NotifyEqpFastUpdate
  ��    �ܣ� ����VCU��ֻ�ܷ�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
  ---------------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/07/27    4.0         ����                  �޸�
=============================================================================*/
void CMcuVcInst::NotifyEqpFastUpdate(const TMt &tDst, u8 byChnl, BOOL32 bSetTimer)
{
    CallLog("[NotifyEqpFastUpdate] the Peri is: %u, the chn is: %u !\n", tDst.GetEqpId(), byChnl);
    
    CServMsg cServMsg;
    u16 wEvent;
    
    u32 dwCurTick = OspTickGet();
    u32 dwLaskTick = 0;
    switch (tDst.GetEqpType())
    {
    case EQP_TYPE_VMP:
        {
            cServMsg.SetChnIndex(byChnl);
            wEvent = MCU_VMP_FASTUPDATEPIC_CMD;
            dwLaskTick = m_dwVmpLastVCUTick;
            if ( dwCurTick - dwLaskTick > g_dwVCUInterval )
            {
                m_dwVmpLastVCUTick = dwCurTick;
            }
            
            SendMsgToEqp(tDst.GetEqpId(), wEvent, cServMsg); 
            return;
            
            break;
        }
        
    case EQP_TYPE_VMPTW:
        wEvent = MCU_VMPTW_FASTUPDATEPIC_CMD;
        dwLaskTick = m_dwVmpTwLastVCUTick;
        if ( dwCurTick - dwLaskTick > g_dwVCUInterval )
        {
            m_dwVmpTwLastVCUTick = dwCurTick;
        }
        break;

    case EQP_TYPE_BAS:
        
        if ( !g_cMcuAgent.IsEqpBasHD( tDst.GetEqpId() ) )
        {
            cServMsg.SetChnIndex(byChnl);
            wEvent = MCU_BAS_FASTUPDATEPIC_CMD;
            dwLaskTick = GetBasLastVCUTick( byChnl );
            if ( dwCurTick - dwLaskTick > g_dwVCUInterval )
            {
                SetBasLastVCUTick( byChnl, dwCurTick );
            }
        }
        else
        {
            cServMsg.SetChnIndex(byChnl);
            wEvent = MCU_BAS_FASTUPDATEPIC_CMD;
            dwLaskTick = m_cBasMgr.GetChnVcuTick( tDst, byChnl );
            if ( dwCurTick - dwLaskTick > g_dwVCUInterval )
            {
                m_cBasMgr.SetChnVcuTick( tDst, byChnl, dwCurTick );
            }
        }
        break;
    case EQP_TYPE_RECORDER:
    default:
        return;
    }
    // ���Ӷ� �����跢������ؼ�֡����ı���, zgc, 2008-04-21
    if ( dwCurTick - dwLaskTick > g_dwVCUInterval )
    {
        SendMsgToEqp(tDst.GetEqpId(), wEvent, cServMsg);  
    }

    // ���ö�ʱ�����ȶ�ʱ����ǿ������
    if (bSetTimer)
    {
        u8 byCount = 1;
        u32 dwParam = tDst.GetEqpId() << 16 | byChnl << 8 | byCount;
        SetTimer( MCUVC_FASTUPDATE_TIMER_ARRAY4EQP+tDst.GetEqpId()-MAXNUM_PERIEQP*3, 1200, dwParam );
    }
    return;
}


/*=============================================================================
  �� �� ���� NotifyFastUpdate
  ��    �ܣ� ����VCU�����͸�ָ���նˡ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� byChnlMode     [in] �����ն��� MODE_VIDEO �� MODE_SECVIDEO
                                 ����BAS �� ͨ����
                                 ������������û������
  �� �� ֵ�� void 
  ---------------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/07/27    4.0         ����                  �޸�
=============================================================================*/
void CMcuVcInst::NotifyFastUpdate( const TMt &tDst, u8 byChnlMode, BOOL32 bSetTimer )
{
    if (tDst.GetType() == TYPE_MT)  // MT
    {
        NotifyMtFastUpdate(tDst.GetMtId(), byChnlMode, bSetTimer);
    }
    else    // Peri Eqp
    {
        NotifyEqpFastUpdate(tDst, byChnlMode, bSetTimer);
    }
}

/*=============================================================================
�� �� ���� GetVidBrdSrc
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� TMt 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/7/23   4.0		�ܹ��                  ����
=============================================================================*/
TMt CMcuVcInst::GetVidBrdSrc(void)
{
    return m_tVidBrdSrc;
}

/*=============================================================================
    �� �� ���� RefreshRcvGrp
    ��    �ܣ� 
    �㷨ʵ�֣� byMtId = 0 ˢ������
               
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� TMt 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/11/20  4.5		    �ű���                  ����
    2009/01/20  4.5         �ű���                  ���������������Խ���������
=============================================================================*/
void CMcuVcInst::RefreshRcvGrp(u8 byMtId)
{
    if (byMtId > MAXNUM_CONF_MT)
    {
        ConfLog(FALSE, "[RefreshRcvGrp] unexpected mtid.%d\n", byMtId);
        return;
    }

    BOOL32 bConfNeedAdp = m_tConf.GetConfAttrb().IsUseAdapter();

    TSimCapSet tMVSrcSim;
    tMVSrcSim.Clear();
    GetMVBrdSrcSim(tMVSrcSim);

    TDStreamCap tDSSrcSim;
    tDSSrcSim.Clear();
    GetDSBrdSrcSim(tDSSrcSim);
    
    if(0 == byMtId)
    {
        u8 byIdx = 1;
        for(; byIdx <= MAXNUM_CONF_MT; byIdx++)
        {
            BOOL32 bMVNeedAdp = FALSE;
            BOOL32 bDSNeedAdp = FALSE;

            if (m_tConfAllMtInfo.MtJoinedConf(byIdx))
            {
                TCapSupport tMtCap;
                TSimCapSet tMtMVSim;
                TDStreamCap tMtDSSim;

                m_ptMtTable->GetMtCapSupport(byIdx, &tMtCap);
                
                //��������
                TLogicalChannel tLogicChn;
                if (!m_ptMtTable->GetMtLogicChnnl(byIdx, LOGCHL_VIDEO, &tLogicChn, TRUE))
                {
                    CallLog("[RefreshRcvGrp] Mt.%d's Forward chn isn't open yet!\n", byIdx);
                    continue;
                }

                tMtMVSim = tMtCap.GetMainSimCapSet();
                if (tMtMVSim.GetVideoMediaType() != tLogicChn.GetChannelType())
                {
                    tMtMVSim = tMtCap.GetSecondSimCapSet();
                    if (tMtMVSim.GetVideoMediaType() != tLogicChn.GetChannelType())
                    {
                        ConfLog(FALSE, "[RefreshRcvGrp] Mt.%d's Forward chn isn't accord to cap, check it!\n", byIdx);
                        continue;
                    }
                }

                //��Ӧ���������������ǽ��ֱ��ʴ���ģ����뵽ͨ��
                if (tMtMVSim.GetVideoResolution() != tLogicChn.GetVideoFormat())
                {
                    tMtMVSim.SetVideoResolution(tLogicChn.GetVideoFormat());
                }

                //��Ӧ������������Ҳ����ʵ�����ʺ��еģ����뵽ǰ��ͨ��
                if (tMtMVSim.GetVideoMaxBitRate() != tLogicChn.GetFlowControl())
                {
                    tMtMVSim.SetVideoMaxBitRate(tLogicChn.GetFlowControl());
                }

                //˫������
                tMtDSSim = tMtCap.GetDStreamCapSet();

                if (bConfNeedAdp)
                {                    
                    bMVNeedAdp = tMtMVSim < tMVSrcSim ? TRUE : FALSE;
                    bDSNeedAdp = tMtDSSim < tDSSrcSim ? TRUE : FALSE;
                    
                    //zbq[02/06/2009] DS����������ǿ�б���H263p���ն˽���������
                    if (tMtDSSim.GetMediaType() == MEDIA_TYPE_H263PLUS &&
                        tDSSrcSim.GetMediaType() == MEDIA_TYPE_H263PLUS )
                    {
                        bDSNeedAdp = FALSE;
                    }
                    //zbq[03/03/2009] ���������ն˱�����Դ�������䵽��������
                    if (bMVNeedAdp &&
                        MT_TYPE_MT == m_ptMtTable->GetMtType(m_tVidBrdSrc.GetMtId()) &&
                        m_tVidBrdSrc.GetMtId() == byIdx)
                    {
                        bMVNeedAdp = FALSE;
                    }
                }
                m_cMtRcvGrp.AddMem(byIdx, tMtMVSim, !bMVNeedAdp);
                m_cMtRcvGrp.AddMem(byIdx, tMtDSSim, !bDSNeedAdp);
            }
        }
        return;
    }

    if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
    {
        TCapSupport tMtCap;
        TSimCapSet tMtMVSim;
        TDStreamCap tMtDSSim;
        
        BOOL32 bMVExcept = TRUE;
        BOOL32 bDSExcept = TRUE;

        m_ptMtTable->GetMtCapSupport(byMtId, &tMtCap);

        //��������
        TLogicalChannel tLogicChn;
        if (!m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_VIDEO, &tLogicChn, TRUE))
        {
            CallLog("[RefreshRcvGrp] Mt.%d's Forward chn isn't open yet!\n", byMtId);
            return;
        }
        
        tMtMVSim = tMtCap.GetMainSimCapSet();
        if (tMtMVSim.GetVideoMediaType() != tLogicChn.GetChannelType())
        {
            tMtMVSim = tMtCap.GetSecondSimCapSet();
            if (tMtMVSim.GetVideoMediaType() != tLogicChn.GetChannelType())
            {
                ConfLog(FALSE, "[RefreshRcvGrp] Mt.%d's Forward chn isn't accord to cap, check it!\n", byMtId);
                return;
            }
        }
        
        //��Ӧ���������������ǽ��ֱ��ʴ���ģ����뵽ͨ��
        if (tMtMVSim.GetVideoResolution() != tLogicChn.GetVideoFormat())
        {
            tMtMVSim.SetVideoResolution(tLogicChn.GetVideoFormat());
        }

        //˫������
        tMtDSSim = tMtCap.GetDStreamCapSet();

        if (bConfNeedAdp)
        {
            bMVExcept = tMtMVSim < tMVSrcSim ? FALSE : TRUE;
            bDSExcept = tMtDSSim < tDSSrcSim ? FALSE : TRUE;

            //zbq[02/06/2009] DS����������ǿ�б���H263p���ն˽���������
            if (tMtDSSim.GetMediaType() == MEDIA_TYPE_H263PLUS &&
                tDSSrcSim.GetMediaType() == MEDIA_TYPE_H263PLUS )
            {
                bDSExcept = TRUE;
            }
            //zbq[03/03/2009] ���������ն˱�����Դ�������䵽��������
            if (bMVExcept &&
                MT_TYPE_MT == m_ptMtTable->GetMtType(m_tVidBrdSrc.GetMtId()) &&
                m_tVidBrdSrc.GetMtId() == byMtId)
            {
                bMVExcept = TRUE;
            }

			//FIXME: merge
			if (m_tConf.m_tStatus.IsBrdstVMP())
			{
				bMVExcept = TRUE;
			}
        }
                
        m_cMtRcvGrp.AddMem(byMtId, tMtMVSim, bMVExcept);
        m_cMtRcvGrp.AddMem(byMtId, tMtDSSim, bDSExcept);
    }
    return;
}

/*=============================================================================
    �� �� ���� RefreshBasParam4MVBrd
    ��    �ܣ� 
    �㷨ʵ�֣����ݵ�ǰ�㲥Դ���������������������ֹ��ͼ���쵼�µ���ڱߵ��������
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� TMt 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/05/05  4.5		    �ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::RefreshBasParam4MVBrd()
{
    //ˢ�µ���̬
    m_cBasMgr.AssignBasChn(m_tConf, MODE_VIDEO);

    //Դ����
    TSimCapSet tMVSrcSim;
    GetMVBrdSrcSim(tMVSrcSim);

    //˫�ٻ��鲻������
    if (m_tConf.GetSecBitRate() != 0)
    {
        return TRUE;
    }

    //������Դ��������������
    TBasChn atBasChn[MAXNUM_CONF_MVCHN];
    u8 byNum = 0;
    m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_MVBRD);
    
    u8 byIdx = 0;
    for(; byIdx < byNum; byIdx ++)
    {
        THDBasVidChnStatus tStatus;
        memset(&tStatus, 0, sizeof(tStatus));
        
        TEqp tEqp = atBasChn[byIdx].GetEqp();
        u8 byChnId = atBasChn[byIdx].GetChnId();

        if (!m_cBasMgr.GetChnStatus(tEqp, byChnId, tStatus))
        {
            ConfLog(FALSE, "[RefreshBasParam4MVBrd] get eqp<%d, %d> status failed\n", 
                tEqp.GetEqpId(), byChnId);
            return FALSE;;
        }

        //������������ܵ��µ�ͼƫ��
        THDAdaptParam *ptAdpParam = tStatus.GetOutputVidParam(0);
        if (NULL == ptAdpParam)
        {
            continue;
        }
        
        //���ʽ���� ����Ҫ������ͼ
        if (tMVSrcSim.GetVideoMediaType() != ptAdpParam->GetVidType())
        {
            continue;
        }

        u8 byMVSrcRes = tMVSrcSim.GetVideoResolution();
        u16 wHeight = 0;
        u16 wWidth = 0;
        GetWHByRes(byMVSrcRes, wWidth, wHeight);
        
        if (0 == wHeight || 0 == wWidth)
        {
            continue;
        }
        
        if (ptAdpParam->GetHeight() <= wHeight &&
            ptAdpParam->GetWidth() <= wWidth)
        {
            continue;
        }
        
        ptAdpParam->SetResolution(wWidth, wHeight);
        
        if (!m_cBasMgr.UpdateChn(tEqp, byChnId, tStatus))
        {
            ConfLog(FALSE, "[RefreshBasParam4MVBrd] update status for Eqp.%d failed!\n", tEqp.GetEqpId());
            continue;
        }
    }

    return TRUE;
}

/*=============================================================================
    �� �� ���� RefreshBasParam4MVBrd
    ��    �ܣ� 
    �㷨ʵ�֣����ݵ�ǰ�㲥Դ�������������������
              ��Ҫ����mpu��������Դ������£���Դ�乫����Դ����Ҫ���ݲ�ͬ��Դ��̬ˢ��
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ��
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/05/05  4.5		    �ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::RefreshBasParam4DSBrd()
{
    //�ָ�����̬
    m_cBasMgr.AssignBasChn(m_tConf, MODE_SECVIDEO);

    if (!m_cBasMgr.IsBrdGrpMpu())
    {
        return TRUE;
    }

    //Դ����
    TDStreamCap tDSSrcSim;
    GetDSBrdSrcSim(tDSSrcSim);

    //��H263p��˫��Դ����ӳ�̬����
    if (MEDIA_TYPE_H263PLUS != tDSSrcSim.GetMediaType())
    {
        return TRUE;
    }

    //H263p��˫��Դ�������������
    TBasChn atBasChn[MAXNUM_CONF_DSCHN];
    u8 byChnNum = 0;
    m_cBasMgr.GetChnGrp(byChnNum, atBasChn, CHN_ADPMODE_DSBRD);
    if (0 == byChnNum)
    {
        ConfLog(FALSE, "[RefreshBasParam4DSBrd] GetChnGrp failed!\n");
        return FALSE;
    }

    TPeriEqpStatus tEqpStatus;
    u8 byEqpId = atBasChn[0].GetEqpId();
    if (!g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tEqpStatus))
    {
        ConfLog(FALSE, "[RefreshBasParam4DSBrd] GetPeriEqpStatus.%d failed!\n", byEqpId);
        return FALSE;
    }

    if (atBasChn[0].GetChnId() >= MAXNUM_MPU_CHN)
    {
        ConfLog(FALSE, "[RefreshBasParam4DSBrd] atBasChn[0].GetChnId().%d failed!\n", atBasChn[0].GetChnId());
        return FALSE;
    }

    THDAdaptParam tHDAdpParam0;
    THDAdaptParam tHDAdpParam1;
    tHDAdpParam0.Reset();
    tHDAdpParam1.Reset();

    //H263p��Դ����� ConfRes/ConfFps �� XGA/5fps
    
    //1��ConfRes/ConfFps
    tHDAdpParam0.SetVidType(MEDIA_TYPE_H264);
    tHDAdpParam0.SetBitRate(m_tConf.GetBitRate()*m_tConf.GetDStreamScale()/100);
    tHDAdpParam0.SetVidActiveType(GetActivePayload(m_tConf, MEDIA_TYPE_H264));
    
    tHDAdpParam0.SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());
    tHDAdpParam0.SetFrameRate(m_tConf.GetDStreamUsrDefFPS());
    
    u16 wWidth = 0;
    u16 wHeight = 0;
    GetWHByRes(m_tConf.GetDoubleVideoFormat(), wWidth, wHeight);
    tHDAdpParam0.SetResolution(wWidth, wHeight);

    //2��XGA/5fps
    tHDAdpParam0.SetVidType(MEDIA_TYPE_H264);
    tHDAdpParam0.SetBitRate(m_tConf.GetBitRate()*m_tConf.GetDStreamScale()/100);
    tHDAdpParam0.SetVidActiveType(GetActivePayload(m_tConf, MEDIA_TYPE_H264));
    
    tHDAdpParam0.SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());
    u8 byDStreamFPS = 5;
    tHDAdpParam0.SetFrameRate(byDStreamFPS);
    
    wWidth = 0;
    wHeight = 0;
    GetWHByRes(VIDEO_FORMAT_XGA, wWidth, wHeight);
    tHDAdpParam0.SetResolution(wWidth, wHeight);
    

    THDBasVidChnStatus tVidStatus;
    memset(&tVidStatus, 0, sizeof(tVidStatus));
    tVidStatus = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(atBasChn[0].GetChnId());

    //����һ�£�ֻ��0��
    if (VIDEO_FORMAT_XGA == m_tConf.GetDStreamUsrDefFPS() &&
        5 == m_tConf.GetDStreamUsrDefFPS())
    {
        tVidStatus.SetOutputVidParam(tHDAdpParam0, 0);
    }
    //0����Conf/Res��1����XGA/5fps
    else
    {
        tVidStatus.SetOutputVidParam(tHDAdpParam0, 0);
        tVidStatus.SetOutputVidParam(tHDAdpParam1, 1);
    }

    tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tVidStatus, atBasChn[0].GetChnId());
 
    g_cMcuVcApp.SetPeriEqpStatus(atBasChn[0].GetEqpId(), &tEqpStatus);

    return TRUE;
}

/*=============================================================================
    �� �� ���� RefreshBasParam
    ��    �ܣ� 
    �㷨ʵ�֣�
               
    ȫ�ֱ����� 
    ��    ���� BOOL32 bSet: �Ƿ����õ�bas���ȥ
    �� �� ֵ�� TMt 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/11/29  4.5		    �ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::RefreshBasParam4AllMt(BOOL32 bSet, BOOL32 bDual)
{
    for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
    {
        if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
        {
            continue;
        }
        RefreshBasParam(byMtId, bSet, bDual);
    }
    return TRUE;
}

/*=============================================================================
    �� �� ���� RefreshBasParam
    ��    �ܣ� 
    �㷨ʵ�֣�
               
    ȫ�ֱ����� 
    ��    ���� BOOL32 bSet: �Ƿ����õ�bas���ȥ
    �� �� ֵ�� TMt 
    -------------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/11/29  4.5		    �ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::RefreshBasParam(u8 byMtId, BOOL32 bSet, BOOL32 bDual)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        ConfLog(FALSE, "[RefreshBasParam] unexpected mtid.%d\n", byMtId);
        return FALSE;
    }

    if(!bDual)
        return RefreshBasMVAdpParam(byMtId, bSet);
    else
        return RefreshBasDSAdpParam(byMtId, bSet);

}

/*=============================================================================
    �� �� ���� RefreshBasMVAdpParam
    ��    �ܣ� 
    �㷨ʵ�֣�
               
    ȫ�ֱ����� 
    ��    ���� BOOL32 bSet: �Ƿ����õ�bas���ȥ
    �� �� ֵ�� TMt 
    -------------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/11/29  4.5		    �ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::RefreshBasMVAdpParam(u8 byMtId, BOOL32 bSet)
{
    if (!m_cMtRcvGrp.IsMtNeedAdp(byMtId))
    {
        return TRUE;
    }

    TLogicalChannel tLogicChn;
    if (!m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_VIDEO, &tLogicChn, TRUE))
    {
        return TRUE;
    }

    TCapSupport tMtCap;
    m_ptMtTable->GetMtCapSupport(byMtId, &tMtCap);
    
    TSimCapSet tMtMVSim = m_ptMtTable->GetDstSCS(byMtId);
    
    if (tMtMVSim.IsNull() ||
        MEDIA_TYPE_NULL == tMtMVSim.GetVideoMediaType())
    {
        ConfLog(FALSE, "[RefreshBasMVAdpParam] get mt.%d's mv cap failed!\n", byMtId);
        return FALSE;
    }

    TEqp tBas;
    u8 byChnId = 0;
    u8 byOutIdx = 0;
    BOOL32 bRet = FALSE;
    bRet = m_cBasMgr.GetBasResource(tMtMVSim.GetVideoMediaType(),
                                    tMtMVSim.GetVideoResolution(),
                                    tBas, byChnId, byOutIdx);
    if (!bRet)
    {
        if (tMtMVSim.GetVideoMediaType() == m_tConf.GetMainVideoMediaType() &&
            tMtMVSim.GetVideoResolution() == m_tConf.GetMainVideoFormat())
        {
            ConfLog(FALSE, "[RefreshBasMVAdpParam] no pos for<vidtype.%d, res.%d>, adjust later!\n",
                tMtMVSim.GetVideoMediaType(), tMtMVSim.GetVideoResolution());
        }
        else
        {
            ConfLog(FALSE, "[RefreshBasMVAdpParam] get bas pos for<vidtype.%d, res.%d> failed!\n",
                            tMtMVSim.GetVideoMediaType(), tMtMVSim.GetVideoResolution());
        }
        return FALSE;
    }

    THDBasVidChnStatus tStatus;
    bRet = m_cBasMgr.GetChnStatus(tBas, byChnId, tStatus);
    if (!bRet)
    {
        ConfLog(FALSE, "[RefreshBasMVAdpParam] get eqp.<%d,%d> mv chn failed!\n", tBas.GetEqpId(), byChnId);
        return FALSE;
    }

    //ˢ���������
    tBas = tStatus.GetEqp();
    tBas.SetConfIdx(m_byConfIdx);
    tStatus.SetEqp(tBas);

    //΢�����ʺ�֡��
    u8 byMtFrmRate = 0;
    if (MEDIA_TYPE_H264 == tMtMVSim.GetVideoMediaType())
    {
        byMtFrmRate = tMtMVSim.GetUserDefFrameRate();
    }
    else
    {
        byMtFrmRate = tMtMVSim.GetVideoFrameRate();
    }
    u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate(byMtId);

    THDAdaptParam tHDAdpParam = *tStatus.GetOutputVidParam(byOutIdx);

    //1���µ�
    if ( (wMtBitRate != 0 && tHDAdpParam.GetBitrate() > wMtBitRate) ||
         (byMtFrmRate != 0 && tHDAdpParam.GetFrameRate() > byMtFrmRate))
    {
        if (wMtBitRate != 0)
        {
            ConfLog(FALSE, "[RefreshBasMVAdpParam] Eqp.%d, chn.%d, out.%d adj due to Mt<BR.%d> LE <BR.%d>!\n",
                tBas.GetEqpId(), byChnId, byOutIdx, wMtBitRate, tHDAdpParam.GetBitrate());

            tHDAdpParam.SetBitRate(wMtBitRate);
        }
        if (byMtFrmRate != 0)
        {
            ConfLog(FALSE, "[RefreshBasMVAdpParam] Eqp.%d, chn.%d, out.%d adj due to Mt<Fr.%d> LE <Fr.%d>!\n",
                tBas.GetEqpId(), byChnId, byOutIdx, byMtFrmRate, tHDAdpParam.GetFrameRate());
            tHDAdpParam.SetFrameRate(byMtFrmRate);
        }
        tStatus.SetOutputVidParam(tHDAdpParam, byOutIdx);
        m_cBasMgr.UpdateChn(tBas, byChnId, tStatus);
        
        if (bSet)
        {
            ChangeHDAdapt(tBas, byChnId);
        }
        return TRUE;
    }
    //2���ϵ�
    else
    {
        u8 byMVType = tHDAdpParam.GetVidType();
        u16 wWidth = tHDAdpParam.GetWidth();
        u16 wHeight = tHDAdpParam.GetHeight();
        u8 byRes = GetResByWH(wWidth, wHeight);

        u8 byNum = 0;
        u8 abyMt[MAXNUM_CONF_MT];
        m_cMtRcvGrp.GetMVMtList(byMVType, byRes, byNum, abyMt);

        u8 byMinFrmRate = 0xff;
        u16 wMinBitRate = 0xffff;
        TCapSupport tMtCap;
        for (u8 byIdx = 0; byIdx < byNum; byIdx++)
        {
            memset(&tMtCap, 0, sizeof(tMtCap));
            m_ptMtTable->GetMtCapSupport(abyMt[byIdx], &tMtCap);
            
            TSimCapSet tCurMVSim;
            tCurMVSim = tMtCap.GetMainSimCapSet();
            if (tCurMVSim.IsNull() ||
                MEDIA_TYPE_NULL == tCurMVSim.GetVideoMediaType())
            {
                tCurMVSim = tMtCap.GetSecondSimCapSet();
            }
            if (tCurMVSim.IsNull() ||
                MEDIA_TYPE_NULL == tCurMVSim.GetVideoMediaType())
            {
                continue;
            }

            if (m_ptMtTable->GetMtReqBitrate(abyMt[byIdx]) != 0)
            {
                wMinBitRate = min(wMinBitRate, m_ptMtTable->GetMtReqBitrate(abyMt[byIdx]));
            }
            
            if (MEDIA_TYPE_H264 == tCurMVSim.GetVideoMediaType())
            {
                if (tCurMVSim.GetUserDefFrameRate() != 0)
                {
                    byMinFrmRate = min(tCurMVSim.GetUserDefFrameRate(), byMinFrmRate);
                }
            }
            else
            {
                if (tCurMVSim.GetVideoFrameRate() != 0)
                {
                    byMinFrmRate = min(tCurMVSim.GetVideoFrameRate(), byMinFrmRate);
                }
            }
        }

        if ((wMinBitRate != 0xffff && wMinBitRate > tHDAdpParam.GetBitrate())||
            (byMinFrmRate != 0xff && byMinFrmRate > tHDAdpParam.GetFrameRate()))
        {
            ConfLog(FALSE, "[RefreshBasMVAdpParam] Eqp.%d, chn.%d, out.%d \
adj due to Min<BR.%d, Fr.%d> GE <BR.%d, Fr.%d>!\n",
                            tBas.GetEqpId(),
                            byChnId,
                            byOutIdx,
                            wMinBitRate,
                            byMinFrmRate,
                            tHDAdpParam.GetBitrate(), tHDAdpParam.GetFrameRate());
            
            if (wMinBitRate != 0xffff)
            {
                tHDAdpParam.SetBitRate(wMinBitRate);
            }
            if (byMinFrmRate != 0xff)
            {
                tHDAdpParam.SetFrameRate(byMinFrmRate);
            }
            tStatus.SetOutputVidParam(tHDAdpParam, byOutIdx);
            m_cBasMgr.UpdateChn(tBas, byChnId, tStatus);
            
            if (bSet)
            {
                ChangeHDAdapt(tBas, byChnId);
            }
            return TRUE;
        }
    }
    return FALSE;
}

BOOL32 CMcuVcInst::RefreshBasMVSelAdpParam(const TEqp &tEqp, u8 byChnId)
{
    return TRUE;
}

BOOL32 CMcuVcInst::RefreshBasDSSelAdpParam(const TEqp &tEqp, u8 byChnId)
{
    return TRUE;
}

/*=============================================================================
    �� �� ���� RefreshBasDSAdpParam
    ��    �ܣ� 
    �㷨ʵ�֣�
               
    ȫ�ֱ����� 
    ��    ���� BOOL32 bSet: �Ƿ����õ�bas���ȥ
    �� �� ֵ�� TMt 
    -------------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/11/29  4.5		    �ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::RefreshBasDSAdpParam(u8 byMtId, BOOL32 bSet)
{
    if (!m_cMtRcvGrp.IsMtNeedAdp(byMtId, FALSE))
    {
        return TRUE;
    }

    TLogicalChannel tLogicChn;
    if (!m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_SECVIDEO, &tLogicChn, TRUE))
    {
        return TRUE;
    }

    TCapSupport tMtCap;
    m_ptMtTable->GetMtCapSupport(byMtId, &tMtCap);
    
    TDStreamCap tMtDSSim;
    tMtDSSim = tMtCap.GetDStreamCapSet();
    
    if (tMtDSSim.IsNull() ||
        MEDIA_TYPE_NULL == tMtDSSim.GetMediaType())
    {
        ConfLog(FALSE, "[RefreshBasDSAdpParam] get mt.%d's ds cap failed!\n", byMtId);
        return FALSE;
    }

    TEqp tBas;
    u8 byChnId = 0;
    u8 byOutIdx = 0;
    BOOL32 bRet = FALSE;
    bRet = m_cBasMgr.GetBasResource(tLogicChn.GetChannelType(),
                                    tLogicChn.GetVideoFormat(),
                                    tBas, 
                                    byChnId,
                                    byOutIdx,
                                    TRUE,
                                    IsDSSrcH263p() || IsConfDualEqMV(m_tConf));
    if (!bRet)
    {
        ConfLog(FALSE, "[RefreshBasDSAdpParam] get bas pos for<vidtype.%d, res.%d> failed!\n",
                        tMtDSSim.GetMediaType(), tMtDSSim.GetResolution());
        return FALSE;
    }

    THDBasVidChnStatus tStatus;
    memset(&tStatus, 0, sizeof(tStatus));

    bRet = m_cBasMgr.GetChnStatus(tBas, byChnId, tStatus);
    if (!bRet)
    {
        ConfLog(FALSE, "[RefreshBasDSAdpParam] get eqp.<%d.%d>ds chn failed!\n",
            tBas.GetEqpId(), byChnId);
        return FALSE;
    }

    //˫�� ����/˫��
    if (!tStatus.IsNull())
    {
        //ˢ���������
        tBas = tStatus.GetEqp();
        tBas.SetConfIdx(m_byConfIdx);
        tStatus.SetEqp(tBas);
        
        //΢�����ʺ�֡��
        u8 byMtFrmRate = 0;
        if (MEDIA_TYPE_H264 == tMtDSSim.GetMediaType())
        {
            byMtFrmRate = tMtDSSim.GetUserDefFrameRate();
        }
        else
        {
            byMtFrmRate = tMtDSSim.GetFrameRate();
        }
        u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate(byMtId, FALSE);
        
        THDAdaptParam tHDAdpParam = *tStatus.GetOutputVidParam(byOutIdx);
        
        //1���µ�
        if ((wMtBitRate != 0 && tHDAdpParam.GetBitrate() > wMtBitRate) ||
            (byMtFrmRate != 0 && tHDAdpParam.GetFrameRate() > byMtFrmRate))
        {
            if (0 != wMtBitRate)
            {
                ConfLog(FALSE, "[RefreshBasDSAdpParam] Eqp.%d, chn.%d, out.%d adj due to Mt.%d<BR.%d> LE <BR.%d>1!\n",
                                tBas.GetEqpId(), byChnId, byOutIdx, byMtId, wMtBitRate, tHDAdpParam.GetBitrate());

                tHDAdpParam.SetBitRate(wMtBitRate);
            }
            if (0 != byMtFrmRate)
            {
                ConfLog(FALSE, "[RefreshBasDSAdpParam] Eqp.%d, chn.%d, out.%d adj due to Mt.%d<Fr.%d> LE <Fr.%d>1!\n",
                    tBas.GetEqpId(), byChnId, byOutIdx, byMtId, byMtFrmRate, tHDAdpParam.GetFrameRate());

                tHDAdpParam.SetFrameRate(byMtFrmRate);
            }
            tStatus.SetOutputVidParam(tHDAdpParam, byOutIdx);
            m_cBasMgr.UpdateChn(tBas, byChnId, tStatus);
            
            if (bSet)
            {
                ChangeHDAdapt(tBas, byChnId);
            }
            return TRUE;
        }
        //2���ϵ�
        else
        {
            u8 byDSType = tHDAdpParam.GetVidType();
            u16 wWidth = tHDAdpParam.GetWidth();
            u16 wHeight = tHDAdpParam.GetHeight();
            u8 byRes = GetResByWH(wWidth, wHeight);
            
            u8 byNum = 0;
            u8 abyMt[MAXNUM_CONF_MT];
            m_cMtRcvGrp.GetDSMtList(byDSType, byRes, byNum, abyMt);
            
            u8 byMinFrmRate = 0xff;
            u16 wMinBitRate = 0xffff;
            for (u8 byIdx = 0; byIdx < byNum; byIdx++)
            {
                memset(&tMtCap, 0, sizeof(tMtCap));
                m_ptMtTable->GetMtCapSupport(abyMt[byIdx], &tMtCap);
                
                TDStreamCap tCurDSSim;
                tCurDSSim = tMtCap.GetDStreamCapSet();
                if (tCurDSSim.IsNull())
                {
                    continue;
                }
                
                if (0 != m_ptMtTable->GetMtReqBitrate(abyMt[byIdx], FALSE))
                {
                    wMinBitRate = min(wMinBitRate, m_ptMtTable->GetMtReqBitrate(abyMt[byIdx], FALSE));
                }
                
                if (MEDIA_TYPE_H264 == tCurDSSim.GetMediaType())
                {
                    if (tCurDSSim.GetUserDefFrameRate() != 0)
                    {
                        byMinFrmRate = min(tCurDSSim.GetUserDefFrameRate(), byMinFrmRate);
                    }
                }
                else
                {
                    if (tCurDSSim.GetFrameRate() != 0)
                    {
                        byMinFrmRate = min(tCurDSSim.GetFrameRate(), byMinFrmRate);
                    }
                }
            }
            
            if ((wMinBitRate != 0xffff && wMinBitRate > tHDAdpParam.GetBitrate())||
                (byMinFrmRate != 0xff && byMinFrmRate > tHDAdpParam.GetFrameRate()))
            {
                ConfLog(FALSE, "[RefreshBasDSAdpParam] Eqp.%d, chn.%d, out.%d \
                                adj due to Min<BR.%d, Fr.%d> GE <BR.%d, Fr.%d>2!\n",
                                tBas.GetEqpId(),
                                byChnId,
                                byOutIdx,
                                wMinBitRate,
                                byMinFrmRate,
                                tHDAdpParam.GetBitrate(), tHDAdpParam.GetFrameRate());
                
                if (wMinBitRate != 0xffff)
                {
                    tHDAdpParam.SetBitRate(wMinBitRate);
                }
                if (byMinFrmRate != 0xff)
                {
                    tHDAdpParam.SetFrameRate(byMinFrmRate);
                }
                tStatus.SetOutputVidParam(tHDAdpParam, byOutIdx);
                m_cBasMgr.UpdateChn(tBas, byChnId, tStatus);
                
                if (bSet)
                {
                    ChangeHDAdapt(tBas, byChnId);
                }
                return TRUE;
            }
        }
    }

    return TRUE;
}

/*=============================================================================
    �� �� ���� GetMVBrdSrcSim
    ��    �ܣ� ��ȡԴ������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� void
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/11/19  4.5		    �ű���                  ����
=============================================================================*/
void CMcuVcInst::GetMVBrdSrcSim(TSimCapSet &tSrcSCS)
{
    //FIXME: ����VMP �� HD BAS�Ļ���
    if (m_tConf.m_tStatus.IsBrdstVMP())
    {
        return;
    }

    if (!m_tConf.GetConfAttrb().IsUseAdapter())
    {
        return;
    }  	
	
	if (!HasJoinedSpeaker() && CONF_POLLMODE_VIDEO != m_tConf.m_tStatus.GetPollMode())
    {
        return;
    }


    TMt tSrcMt;
    tSrcMt.SetNull();

	//fxh Bug00018647 ֻ��Ҫ��ȡ��Ƶ�㲥Դ����,�ڷ�����ģʽ��,������ͬΪ��Ƶ�㲥Դ 
// 	if (HasJoinedSpeaker())
// 	{
//         tSrcMt = GetLocalSpeaker();
// 	}
//     else if (CONF_POLLMODE_VIDEO == m_tConf.m_tStatus.GetPollMode())
//     {
//         tSrcMt = m_tVidBrdSrc;
//     }
 	tSrcMt = m_tVidBrdSrc;

    if (tSrcMt.IsNull())
    {
        ConfLog(FALSE, "[GetMVBrdSrcSim] no src mt in conf\n");
        return;
    }

    //����¼���
    u8 byType = MEDIA_TYPE_NULL;
    u8 byAudType = MEDIA_TYPE_NULL;
    u16 wWidth = 0;
    u16 wHeight = 0;
    if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType())
    {
        m_tPlayFileMediaInfo.GetVideo( byType, wWidth, wHeight );
        byAudType = m_tPlayFileMediaInfo.GetAudio();
        EqpLog( "[GetMVBrdSrcSim] Type.%d, width.%d, wHeight.%d, AudType.%d\n", byType, wWidth, wHeight, byAudType );

        if ( byType != MEDIA_TYPE_NULL )
        {
            tSrcSCS.SetVideoMediaType( byType);
            tSrcSCS.SetVideoResolution( GetResByWH( wWidth, wHeight ) );
            tSrcSCS.SetVideoMaxBitRate( m_tConf.GetBitRate() );
        }
        else
        {
            tSrcSCS.SetVideoMediaType( m_tConf.GetMainVideoMediaType() );
            tSrcSCS.SetVideoResolution( m_tConf.GetMainVideoFormat() );
            tSrcSCS.SetVideoMaxBitRate( m_tConf.GetBitRate() );
        }

        if ( byAudType != MEDIA_TYPE_NULL )
        {
            tSrcSCS.SetAudioMediaType(byAudType);
        }
        else
        {
            tSrcSCS.SetAudioMediaType( m_tConf.GetMainAudioMediaType() );
        }

        u8 byConfFrame = 0;
        if ( m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264 )
        {
            byConfFrame = m_tConf.GetMainVidUsrDefFPS();
        }
        else
        {
            byConfFrame = m_tConf.GetMainVidFrameRate();
        }
        if ( tSrcSCS.GetVideoMediaType() == MEDIA_TYPE_H264 )
        {
            tSrcSCS.SetUserDefFrameRate( byConfFrame );
        }
        else
        {
            tSrcSCS.SetVideoFrameRate( byConfFrame );
        }
    }
    else
    {
        tSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());	

        //zbq[10/10/2008] ֡�ʱ���
        u8 byFrmRate = 0;
        if (MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType())
        {
            byFrmRate = tSrcSCS.GetUserDefFrameRate();
        }
        else
        {
            byFrmRate = tSrcSCS.GetVideoFrameRate();
        }
        if (0 == byFrmRate)
        {
            u8 byConfFrmRt = 0;
            
            if (tSrcSCS.GetVideoMediaType() == m_tConf.GetMainVideoMediaType())
            {
                if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType())
                {
                    byConfFrmRt = m_tConf.GetMainVidUsrDefFPS();
                }
                else
                {
                    byConfFrmRt = m_tConf.GetMainVidFrameRate();
                }
            }
            else if (tSrcSCS.GetVideoMediaType() == m_tConf.GetSecVideoMediaType())
            {
                if (MEDIA_TYPE_H264 == m_tConf.GetSecVideoMediaType())
                {
                    byConfFrmRt = m_tConf.GetSecVidUsrDefFPS();
                }
                else
                {
                    byConfFrmRt = m_tConf.GetSecVidFrameRate();
                }
            }
            else
            {
                ConfLog(FALSE, "[GetMVBrdSrcSim] SrcSCS's vid type.%d unmatched, ignore it!\n", 
                                tSrcSCS.GetVideoMediaType());
            }

            if (MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType())
            {
                tSrcSCS.SetUserDefFrameRate(byConfFrmRt);
            }
            else
            {
                tSrcSCS.SetVideoFrameRate(byConfFrmRt);
            }
        }
        if (tSrcSCS.IsNull())
        {
            ConfLog(FALSE, "[GetMVBrdSrcSim] �㲥Դδ��MCU����Ƶ�߼�ͨ��!\n");
            return;
        }
        tSrcSCS.SetVideoMaxBitRate( m_ptMtTable->GetMtSndBitrate( tSrcMt.GetMtId() ) );
    }

    return;
}


/*=============================================================================
    �� �� ���� GetDSSrcSim
    ��    �ܣ� ��ȡ˫��Դ������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� void
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/11/19  4.5		    �ű���                  ����
=============================================================================*/
void CMcuVcInst::GetDSBrdSrcSim(TDStreamCap &tDSim)
{
    TMt tSrcMt;
    tSrcMt.SetNull();
    
    if (!m_tDoubleStreamSrc.IsNull())
    {
        tSrcMt = m_tDoubleStreamSrc;
    }
    
    if (tSrcMt.IsNull())
    {
        CallLog("[GetDSSrcSim] no ds src mt in conf\n");
        return;
    }
    
    //����¼���
    u8 byType = MEDIA_TYPE_NULL;
    u16 wWidth = 0;
    u16 wHeight = 0;
    if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType())
    {
        m_tPlayFileMediaInfo.GetDVideo( byType, wWidth, wHeight );
		EqpLog( "[GetDSSrcSim] Type.%d, width.%d, wHeight.%d\n", byType, wWidth, wHeight );     
        
        if ( byType != MEDIA_TYPE_NULL )
        {
            tDSim.SetMediaType( byType);
            tDSim.SetResolution( GetResByWH( wWidth, wHeight ) );
            tDSim.SetMaxBitRate( GetDoubleStreamVideoBitrate(m_tConf.GetBitRate(), FALSE) );
        }
        else
        {
            tDSim.SetMediaType( m_tConf.GetMainVideoMediaType() );
            tDSim.SetResolution( m_tConf.GetMainVideoFormat() );
            tDSim.SetMaxBitRate( GetDoubleStreamVideoBitrate(m_tConf.GetBitRate(), FALSE) );
        }
        u8 byConfFrame = 0;
        if ( m_tConf.GetDStreamMediaType() == MEDIA_TYPE_H264 )
        {
            byConfFrame = m_tConf.GetDStreamUsrDefFPS();
        }
        else
        {
            byConfFrame = m_tConf.GetDStreamFrameRate();
        }
        if ( tDSim.GetMediaType() == MEDIA_TYPE_H264 )
        {
            tDSim.SetUserDefFrameRate( byConfFrame );
        }
        else
        {
            tDSim.SetFrameRate( byConfFrame );
        }
    }
    else
    {
        //zbq[01/09/2009] ˫��Դ����ȡ�����߼�ͨ��
        TLogicalChannel tDSRcvChn;
        if (m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_SECVIDEO, &tDSRcvChn, FALSE))
        {
            tDSim.SetMediaType(tDSRcvChn.GetChannelType());
            if (MEDIA_TYPE_H264 == tDSim.GetMediaType())
            {
                tDSim.SetUserDefFrameRate(tDSRcvChn.GetChanVidFPS());
            }
            else
            {
                tDSim.SetFrameRate(tDSRcvChn.GetChanVidFPS());
            }
            tDSim.SetMaxBitRate(tDSRcvChn.GetFlowControl());
            tDSim.SetResolution(tDSRcvChn.GetVideoFormat());
        }
        else
        {
            ConfLog(FALSE, "[GetDSSrcSim] DSSrc's rcv chn hasn't opened yet!\n");

            TCapSupport tCap;
            m_ptMtTable->GetMtCapSupport(tSrcMt.GetMtId(), &tCap);
            tDSim = tCap.GetDStreamCapSet();
            
            //zbq[10/10/2008] ֡�ʱ���
            u8 byFrmRate = 0;
            if (MEDIA_TYPE_H264 == tDSim.GetMediaType())
            {
                byFrmRate = tDSim.GetUserDefFrameRate();
            }
            else
            {
                byFrmRate = tDSim.GetFrameRate();
            }
            if (0 == byFrmRate)
            {
                if (MEDIA_TYPE_H264 == tDSim.GetMediaType())
                {
                    tDSim.SetUserDefFrameRate(m_tConf.GetMainVidUsrDefFPS());
                }
                else
                {
                    tDSim.SetFrameRate(m_tConf.GetMainVidUsrDefFPS());
                }
            }
            if (tDSim.IsNull())
            {
                ConfLog(FALSE, "[GetDSSrcSim] DSSrc's ds cap is NULL!\n");
                return;
            }
            tDSim.SetMaxBitRate( m_ptMtTable->GetMtSndBitrate( tSrcMt.GetMtId(), LOGCHL_SECVIDEO ) );
        }
    }
    
    return;
}


/*=============================================================================
    �� �� ���� GetSelSrcSim
    ��    �ܣ� ��ȡ˫��ѡ��Դ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� void
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/11/19  4.5		    �ű���                  ����
=============================================================================*/
void CMcuVcInst::GetSelSrcSim(const TEqp &tEqp, u8 byChnId, TDStreamCap &tDSim)
{

}


/*=============================================================================
    �� �� ���� GetSelSrcSim
    ��    �ܣ� ��ȡ��Ƶѡ��Դ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� void
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/11/19  4.5		    �ű���                  ����
=============================================================================*/
void CMcuVcInst::GetSelSrcSim(const TEqp &tEqp, u8 byChnId, TSimCapSet &tSim)
{

}


/*=============================================================================
    �� �� ���� IsMtInMcSrc
    ��    �ܣ� �ж��ն��ڱ��������Ƿ��ж�Ӧ��Դ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byChnRes: ��ͨ���õ�Res, ֻ���h264�Ļ��鴦��
               BOOL32 &bAccord2MainCap���Ƿ����������������
    �� �� ֵ�� void
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/12/30  4.5		    �ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::IsMtMatchedSrc(u8 byMtId, u8 &byChnRes, BOOL32 &bAccord2MainCap)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return FALSE;
    }
    if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
    {
        return FALSE;
    }

    TCapSupport tCap;
    if (!m_ptMtTable->GetMtCapSupport(byMtId, &tCap))
    {
        ConfLog(FALSE, "[IsMtMatchedSrc] get mt.%d's cap failed!\n", byMtId);
        return FALSE;
    }

    TSimCapSet tSimCap;
    tSimCap.Clear();
    tSimCap = tCap.GetMainSimCapSet();

    if (tSimCap.IsNull() ||
        MEDIA_TYPE_NULL == tSimCap.GetVideoMediaType())
    {
        tSimCap = tCap.GetSecondSimCapSet();
    }
    if (tSimCap.IsNull() ||
        MEDIA_TYPE_NULL == tSimCap.GetVideoMediaType())
    {
        ConfLog(FALSE, "[IsMtMatchedSrc] mt.%d cap is null already, ignore it!\n", byMtId);
        return FALSE;
    }
    
    u8 byMediaType = tSimCap.GetVideoMediaType();
    u8 byRes = tSimCap.GetVideoResolution();

    if (MEDIA_TYPE_H264 != byMediaType)
    {
        if (byMediaType == m_tConf.GetMainVideoMediaType() ||
            byMediaType == m_tConf.GetSecVideoMediaType() )
        {
            bAccord2MainCap = TRUE;
            byChnRes = m_tConf.GetMainVideoFormat();
            return TRUE;
        }
    }
    else
    {
        if (byMediaType != m_tConf.GetMainVideoMediaType())
        {
            return FALSE;
        }
        if (byRes == m_tConf.GetMainVideoFormat() ||
            (byRes == VIDEO_FORMAT_HD1080 && m_tConf.GetConfAttrbEx().IsResEx1080()) ||
            (byRes == VIDEO_FORMAT_HD720 && m_tConf.GetConfAttrbEx().IsResEx720()) ||
            (byRes == VIDEO_FORMAT_4CIF && m_tConf.GetConfAttrbEx().IsResEx4Cif()) ||
            (byRes == VIDEO_FORMAT_CIF && m_tConf.GetConfAttrbEx().IsResExCif()))
        {
            //zbq[05/22/2009] ������Դ����ģʽ�£�ͬ�������ֱ��ʽ��ٴ򿪲���֧�֣��˴��������ֱ��ʴ򿪴���.
            if (byRes == m_tConf.GetMainVideoFormat())
            {
                u16 wDialBR = m_ptMtTable->GetDialBitrate(byMtId);
                u16 wConfBR = m_tConf.GetBitRate();
                
                //���ٺ��д��Լ���
                BOOL32 bCallLowBR = (wConfBR - wDialBR) * 100 / wConfBR > 5;
                if (bCallLowBR)
                {
                    //����ȡ�ٽ��ֱ���
                }
                else
                {
                    bAccord2MainCap = TRUE;
                    byChnRes = byRes;
                    return TRUE;
                }
            }
            else
            {
                bAccord2MainCap = TRUE;
                byChnRes = byRes;
                return TRUE;
            }
        }

        //zbq[04/22/2009] ����ȡ����֧�ֵ��ٽ��ֱ���
        byChnRes = 0xff;
		if (byRes == VIDEO_FORMAT_HD1080)
		{
			if (m_tConf.GetConfAttrbEx().IsResEx720())
			{
				byChnRes = VIDEO_FORMAT_HD720;
			}
			else if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
			{
				byChnRes = VIDEO_FORMAT_4CIF;
			}
			else if (m_tConf.GetConfAttrbEx().IsResExCif())
			{
				byChnRes = VIDEO_FORMAT_CIF;
			}
		}
		else if (byRes == VIDEO_FORMAT_HD720)
		{
			if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
			{
				byChnRes = VIDEO_FORMAT_4CIF;
			}
			else if (m_tConf.GetConfAttrbEx().IsResExCif())
			{
				byChnRes = VIDEO_FORMAT_CIF;
			}
		}
		else if (byRes == VIDEO_FORMAT_4CIF)
		{
			if (m_tConf.GetConfAttrbEx().IsResExCif())
			{
				byChnRes = VIDEO_FORMAT_CIF;
			}
		}
		else
		{
			//Do nothing
		}
        
        if (0xff != byChnRes)
        {
            bAccord2MainCap = TRUE;
            return TRUE;
        }
    }
	
    //zbq[04/22/2009] ˫��ʽ֧�ֿ���
    tSimCap.Clear();
    tSimCap = tCap.GetSecondSimCapSet();
    if (!tSimCap.IsNull())
    {
        if (tSimCap.GetVideoMediaType() == m_tConf.GetSecVideoMediaType())
        {
            bAccord2MainCap = FALSE;
            byChnRes = tSimCap.GetVideoResolution();
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
    �� �� ���� GetProximalGrp
    ��    �ܣ� ��ȡ�������ٽ�Ⱥ��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� void
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    09/01/05    4.5         �ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::GetProximalGrp(u8 byMediaType, u8 byRes, u8 &byGrpType, u8 &byGrpRes)
{
    //��264û�п�ѡ���ٽ�Ⱥ��
    if (MEDIA_TYPE_H264 != byMediaType)
    {
        return FALSE;
    }

    byGrpRes = 0;
    byGrpType = byMediaType;

    switch (byRes)
    {
    case VIDEO_FORMAT_4CIF:
        
        if (m_tConf.GetConfAttrbEx().IsResExCif())
        {
            byGrpRes = VIDEO_FORMAT_CIF;
        }
        break;

    case VIDEO_FORMAT_HD720:

        if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
        {
            byGrpRes = VIDEO_FORMAT_4CIF;
        }
        else if (m_tConf.GetConfAttrbEx().IsResExCif())
        {
            byGrpRes = VIDEO_FORMAT_CIF;
        }
        break;

    case VIDEO_FORMAT_HD1080:

        if (m_tConf.GetConfAttrbEx().IsResEx720())
        {
            byGrpRes = VIDEO_FORMAT_HD720;
        }
        else if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
        {
            byGrpRes = VIDEO_FORMAT_4CIF;
        }
        else if (m_tConf.GetConfAttrbEx().IsResExCif())
        {
            byGrpRes = VIDEO_FORMAT_CIF;
        }
        break;

    case VIDEO_FORMAT_CIF:
    default:
        break;
    }

    if (byGrpRes != 0)
    {
        return TRUE;
    }
    return FALSE;
}


/*====================================================================
    ������      ��IsDelayVidBrdVCU
    ����        ���Ƿ��Ƴ���ƵԴ��VCU��youareseeing�ķ���
	              ֪ͨMC��MT��
    �㷨ʵ��    �����Tandberg��VMP��ѯ����ʱ����VMP����������� ���⴦�� �ж�ʵ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/25    3.0         ������        ����
====================================================================*/
BOOL32 CMcuVcInst::IsDelayVidBrdVCU()
{
    if (m_tVidBrdSrc.IsNull() ||
        MT_MANU_TAIDE == m_ptMtTable->GetManuId(m_tVidBrdSrc.GetMtId()))
    {
        return FALSE;
    }
    if (CONF_POLLMODE_VIDEO != m_tConf.m_tStatus.GetPollMode() &&
        CONF_POLLMODE_SPEAKER != m_tConf.m_tStatus.GetPollMode() )
    {
        return FALSE;
    }

    BOOL32 bVmpSelPoll = FALSE;

    u8 byIdx = 0;
    for (byIdx = 0; byIdx < MAXNUM_MPUSVMP_MEMBER; byIdx++)
    {
        TVMPMember *ptVmpMem = m_tConf.m_tStatus.m_tVMPParam.GetVmpMember(byIdx);
        if (ptVmpMem->IsNull())
        {
            continue;
        }
        if (ptVmpMem->GetMemberType() == VMP_MEMBERTYPE_POLL)
        {
            bVmpSelPoll = TRUE;
            break;
        }
    }

    return bVmpSelPoll;
}

/*=============================================================================
    �� �� ���� GetMtMatchedRes
    ��    �ܣ� ��ȡָ���ն���ָ����Ƶ��ʽ����ӽ�����֧�ֵķֱ���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� byMtId:     ָ���ն�
			   byChnType:  ָ����Ƶ��ʽ
			   byChnRes:   ����ָ����Ƶ��ʽ����ϻ���Ŀ�֧�ֱַ���
    �� �� ֵ�� void
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    09/05/19    4.5		    ���㻪                  ����
=============================================================================*/
BOOL32 CMcuVcInst::GetMtMatchedRes(u8 byMtId, u8 byChnType, u8& byChnRes)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return FALSE;
    }
    if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
    {
        return FALSE;
    }

    TCapSupport tCap;
    if (!m_ptMtTable->GetMtCapSupport(byMtId, &tCap))
    {
        ConfLog(FALSE, "[GetMtMatchedRes] get mt.%d's cap failed!\n", byMtId);
        return FALSE;
    }

	if (tCap.GetMainVideoType() == byChnType)
	{
		byChnRes  = tCap.GetMainVideoResolution();
	}
	else if (tCap.GetSecVideoType() == byChnType)
	{
		byChnRes  = tCap.GetSecVideoResolution();
	}
	else
	{
		ConfLog(FALSE, "[GetMtMatchedRes] mt not support this media type(%d)\n", byChnType);
		return FALSE;
	}

	u8 byConfRes = 0;
	if (m_tConf.GetMainVideoMediaType() == byChnType)
	{
		byConfRes = m_tConf.GetMainVideoFormat();
	}
	else
	{
		byConfRes = m_tConf.GetSecVideoFormat();
	}
	CallLog("[GetMtMatchedRes] conf res(%d) cap of mediatype(%d)\n", byConfRes, byChnType);

	// ����mpeg4����Ӧ���⴦��
	if (VIDEO_FORMAT_AUTO == byConfRes || VIDEO_FORMAT_AUTO == byChnRes)
	{
		CallLog("[GetMtMatchedRes] open mt video channl with mt res(%d)\n", byChnRes);
		return TRUE;
	}
	else if (IsResGE(byChnRes, byConfRes))
	{

        //zbq[05/22/2009] ������Դ����ģʽ�£�ͬ�������ֱ��ʽ��ٴ򿪲���֧�֣��˴��������ֱ��ʴ򿪴���.
        u16 wDialBR = m_ptMtTable->GetDialBitrate(byMtId);
        u16 wConfBR = m_tConf.GetBitRate();
        
        //���ٺ��д��Լ���
        BOOL32 bCallLowBR = (wConfBR - wDialBR) * 100 / wConfBR > 5;
        
        if (bCallLowBR && g_cMcuVcApp.IsAdpResourceCompact())
        {
            u8 byResTmp = byChnRes;

            //����ȡ�ٽ��ֱ���
            if (VIDEO_FORMAT_HD720 == byChnRes ||
                VIDEO_FORMAT_HD1080 == byChnRes)
            {
                if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
                {
                    byChnRes = VIDEO_FORMAT_4CIF;
                }
                else if (m_tConf.GetConfAttrbEx().IsResExCif())
                {
                    byChnRes = VIDEO_FORMAT_CIF;
                }
            }
            else if (byChnRes == VIDEO_FORMAT_4CIF)
            {
                if (m_tConf.GetConfAttrbEx().IsResExCif())
                {
                    byChnRes = VIDEO_FORMAT_CIF;
                }
            }
            else
            {
                //Do nothing
			}

            if ( byResTmp != byChnRes )
            {
                CallLog("[GetMtMatchedRes] open mt video channl adj res(%d) to res(%d)\n", byResTmp, byChnRes);
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            byChnRes = byConfRes;
            CallLog("[GetMtMatchedRes] open mt video channl with conf res(%d)\n", byChnRes);
            return TRUE;
        }
	}
	else
	{
        if (byChnType != MEDIA_TYPE_H264 || byChnType != m_tConf.GetMainVideoMediaType())
        {
            return FALSE;
        }

		if (g_cMcuVcApp.IsSendFakeCap2Polycom() &&
			MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byMtId))
		{
			byChnRes = VIDEO_FORMAT_4SIF;
			CallLog("[GetMtMatchedRes] Open Mt.%d video logic chnnl adjusted to 4SIF due to it's Polycom!\n", byMtId);
			return TRUE;
		}
		// ����VCS����,H264����,�������зֱ��ʵ��ն����,���ձȻ���С���ն�֧�ֵķֱ��ʿ�ͨ��
		// �������Ŀǰ�߻����߼�
		else if (VCS_CONF == m_tConf.GetConfSource() &&
			     !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
		{
			return TRUE;
		}
		else
		{
			//zbq[04/22/2009] ����ȡ����֧�ֵ��ٽ��ֱ���
			u8 byTmpChnRes = byChnRes;
            byChnRes = 0xff;
			if (VIDEO_FORMAT_HD720 == byTmpChnRes ||
                VIDEO_FORMAT_HD1080 == byTmpChnRes)
			{
				if (m_tConf.GetConfAttrbEx().IsResEx720())
				{
					byChnRes = VIDEO_FORMAT_HD720;
				}
				else if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
				{
					byChnRes = VIDEO_FORMAT_4CIF;
				}
				else if (m_tConf.GetConfAttrbEx().IsResExCif())
				{
					byChnRes = VIDEO_FORMAT_CIF;
				}
			}
			else if (byTmpChnRes == VIDEO_FORMAT_4CIF)
			{
				if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
				{
					byChnRes = VIDEO_FORMAT_4CIF;
				}
				else if (m_tConf.GetConfAttrbEx().IsResExCif())
				{
					byChnRes = VIDEO_FORMAT_CIF;
				}
			}
			else if (byTmpChnRes == VIDEO_FORMAT_CIF)
			{
				if (m_tConf.GetConfAttrbEx().IsResExCif())
				{
					byChnRes = VIDEO_FORMAT_CIF;
				}
			}
			else
			{
				//Do nothing
			}
        
			if (byChnRes != 0xff)
			{
				CallLog("[GetMtMatchedRes] open mt video channl with res(%d) with use of adp\n", byChnRes);
				return TRUE;
			}
		}
    }
	
    return FALSE;
}

/*====================================================================
    ������      ��FindAcceptCodeChnl
    ����        ������ָ���ն��Ƿ�ɽ���ָ������������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/06/01    4.0         ���㻪        ����
====================================================================*/
BOOL32 CMcuVcInst::FindAcceptCodeChnl(u8 byMtId, u8 byMode, u8 byEqpId, u8 byChnIdx, u8& byOutChnlIdx)
{
    TPeriEqpStatus tEqpStatus;
    if (!g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tEqpStatus))
    {
        OspPrintf(TRUE, FALSE, "[FindAcceptCodeChnl] get Eqp.%d status failed!\n", byEqpId);
        return FALSE;
    }
   
	TLogicalChannel tChnlInfo;
	if (byMode == MODE_VIDEO)
	{
		m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_VIDEO, &tChnlInfo, TRUE);
	}

	else if (byMode == MODE_AUDIO)
	{
		m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_AUDIO, & tChnlInfo, TRUE);
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[FindAcceptCodeChnl] wrong input param about byMode(%d)\n", byMode);
		return FALSE;
	}

	// �����ư汾:Ŀǰ��Ƶ����ֻѡ��ԭ����������,��Ƶ����ֻѡ�ø���������
	if (MODE_AUDIO == byMode)
	{
		if (byChnIdx >= tEqpStatus.m_tStatus.tBas.byChnNum)
		{
			OspPrintf(TRUE, FALSE, "[FindAcceptCodeChnl] wrong input chnlidx(%d) for sdbas(%d) with chnlnum %d\n",
				      byChnIdx, byEqpId, tEqpStatus.m_tStatus.tBas.byChnNum);
			return FALSE;
		}

		if (tChnlInfo.GetChannelType() == tEqpStatus.m_tStatus.tBas.tChnnl[byChnIdx].GetAudType())
		{
			byOutChnlIdx = byChnIdx;
			return TRUE;
		}		
		
		
	}
	else
	{
		THDBasVidChnStatus tVidChn;
		memset(&tVidChn, 0, sizeof(tVidChn));
		THDAdaptParam tParam;
		u8 byBasType = tEqpStatus.m_tStatus.tHdBas.GetEqpType();
		switch (byBasType)
		{
		case TYPE_MAU_NORMAL:
		case TYPE_MAU_H263PLUS:
			{
				if (byChnIdx >= MAXNUM_MAU_VCHN + MAXNUM_MAU_DVCHN)
				{
					OspPrintf(TRUE, FALSE, "[TMVChnGrp::AddChn] unexpected mau chnId.%d!\n", byChnIdx);
					return FALSE;
				}
            
				BOOL32 bChnMV = byChnIdx == 0 ? TRUE : FALSE;            
				if (bChnMV)
				{
					tVidChn = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus();
                
					for(u8 byIdx = 0; byIdx < MAXNUM_VOUTPUT; byIdx++)
					{
						tParam = *tVidChn.GetOutputVidParam(byIdx);
						if (tParam.GetVidType() == tChnlInfo.GetChannelType() &&
							IsResGE(tChnlInfo.GetVideoFormat(), GetResByWH(tParam.GetWidth(), tParam.GetHeight())))
						{
							byOutChnlIdx = byIdx;
							return TRUE;
						}
					}
				}
				else
				{
					tVidChn = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus();
                
					for(u8 byIdx = 0; byIdx < MAXNUM_VOUTPUT; byIdx++)
					{
						tParam = *tVidChn.GetOutputVidParam(byIdx);
						if (tParam.GetVidType() == tChnlInfo.GetChannelType() &&
							IsResGE(tChnlInfo.GetVideoFormat(), GetResByWH(tParam.GetWidth(), tParam.GetHeight())))
						{
							byOutChnlIdx = byIdx;
							return TRUE;
						}
					}
				}
			}
			break;
        
		case TYPE_MPU:
			{
				if (byChnIdx >= MAXNUM_MPU_CHN)
				{
					OspPrintf(TRUE, FALSE, "[TMVChnGrp::AddChn] unexpected mpu chnId.%d!\n", byChnIdx);
					return FALSE;
				}
				tVidChn = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx);
            
				for(u8 byIdx = 0; byIdx < MAXNUM_VOUTPUT; byIdx++)
				{
					tParam = *tVidChn.GetOutputVidParam(byIdx);
					if (tParam.GetVidType() == tChnlInfo.GetChannelType() &&
						IsResGE(tChnlInfo.GetVideoFormat(), GetResByWH(tParam.GetWidth(), tParam.GetHeight())))
					{
						byOutChnlIdx = byIdx;
						return TRUE;
					}
				}
			}
			break;
        
		default:
			break;
		}
	}


	return FALSE;
}

/*====================================================================
������      ��IsNeedNewSelAdp
����        ������ָ��ѡ���Ƿ���Ҫʹ���µ�������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����bySrcId:ѡ�������ն�
	          byDstId:��ѡ�������ն�
			  bySelMode:ѡ��������
			  ptEqpId:�ɸ���ʹ�õ�������ID
			  pOutChnlIdx:�ɸ���ʹ�õ����������ͨ������
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
09/06/01    4.0         ���㻪        ����
====================================================================*/
BOOL32 CMcuVcInst::IsNeedNewSelAdp(u8 bySrcId, u8 byDstId, u8 bySelMode, u8* pbyEqpId /*= NULL*/, u8* pbyOutChnlIdx /*= NULL*/)
{
	u8 byOccupChnlNum, byChnIdx = 0;
	u8 abyEqpId[MAXNUM_PERIEQP];
	u8 abyChnIdx[MAXNUM_PERIEQP];
	if (m_cSelChnGrp.FindSelSrc(bySrcId, bySelMode, byOccupChnlNum, abyEqpId, abyChnIdx))
	{
		for (u8 byIdx = 0; byIdx < byOccupChnlNum; byIdx++)
		{			
			if (FindAcceptCodeChnl(byDstId, bySelMode, abyEqpId[byIdx], abyChnIdx[byIdx], byChnIdx))
			{
				if (pbyEqpId != NULL)
				{
					*pbyEqpId = abyEqpId[byIdx];
				}
				if (pbyOutChnlIdx != NULL)
				{
					*pbyOutChnlIdx = /*byChnIdx*/abyChnIdx[byIdx];
				}
				EqpLog("[StartHdVidSelAdp] Find acceptable code from bas(id:%d, inputchnl:%d, outputchnl:%d) for mt(mtid:%d)",
					   abyEqpId[byIdx], abyChnIdx[byIdx], byChnIdx, byDstId);
				return FALSE;
			}
		}
	}
	return TRUE;
}


/************************************************************************/
/*                                                                      */
/*                          �ˡ����ǻ����������                        */
/*                                                                      */
/************************************************************************/


/*====================================================================
 ������      :DaemonProcSendMsgToNms
 ����        :����UDP������Ϣ
 �㷨ʵ��    
 ����ȫ�ֱ�����
 �������˵����
 ����ֵ˵��  ��
 ----------------------------------------------------------------------
 �޸ļ�¼    ��
 ��  ��      �汾        �޸���        �޸�����
 03/12/19   1.0          zhangsh        ����
 09/08/28   4.6          �ű���         �����ǰ汾��ֲ����
====================================================================*/
void CMcuVcInst::DaemonProcSendMsgToNms( const CMessage* pcMsg  )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    
    u32   dwIp;
    u16   wPort;
    SOCKET  m_hSocket;
    SOCKADDR_IN tUdpAddr;
    int         nTotalSendLen;
    
    g_cMcuVcApp.GetApplyFreqInfo( dwIp, wPort );
    
    memset( &tUdpAddr, 0, sizeof( tUdpAddr ) );
    tUdpAddr.sin_family = AF_INET; 
    tUdpAddr.sin_port = htons(wPort);
    tUdpAddr.sin_addr.s_addr = dwIp;
    
    m_hSocket = socket ( AF_INET, SOCK_DGRAM , 0 );
    if( m_hSocket == INVALID_SOCKET )
    {
        ConfLog( FALSE, "DaemonProcSendMsgToNms: Create UDP Socket Error.\n" );
        return ;
    }
    nTotalSendLen = sendto( m_hSocket,
                            (s8*)cServMsg.GetMsgBody(),
                            cServMsg.GetMsgBodyLen(),
                            0,
                            (struct sockaddr *)&tUdpAddr,
                            sizeof( tUdpAddr ) );
    SockClose( m_hSocket );

    if ( nTotalSendLen < cServMsg.GetMsgBodyLen() )
    {
        ConfLog( FALSE, "DaemonProcSendMsgToNms: Send Error.\n" );
        return ;
    }
}


/*====================================================================
 ������      ��ApplySatFrequence
 ����        ��Ϊ�����ٿ��Ļ�����������Ƶ��
 �㷨ʵ��    ��
 ����ȫ�ֱ�����
 �������˵����
 ����ֵ˵��  ��
 ----------------------------------------------------------------------
 �޸ļ�¼    ��
 ��  ��      �汾        �޸���        �޸�����
 09/08/28    4.6         �ű���        ����
====================================================================*/
void CMcuVcInst::ApplySatFrequence()
{
	u32 dwFreq[5];
	u32 dwBrdBitRate;
	u32 dwRcvBitRate;

    if ( m_tConf.GetConfAttrb().IsSatDCastMode() /*&& 
         !g_cMcuVcApp.IsConfGetAllFreq( m_byConfIdx )*/ )
    {
        g_cMcuVcApp.SetConfInfo( m_byConfIdx );					
        
        //��������㲥��
        dwBrdBitRate = GetAudioBitrate( m_tConf.GetMainAudioMediaType() );
        dwBrdBitRate = dwBrdBitRate * 5;//m_tConf.GetConfAttrbEx().GetSatDCastChnlNum();

		//˫��ʽ���飬�㲥Ƶ�ʵĳ������ӱ���������˫������
		if (MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType() ||
			0 == m_tConf.GetSecVideoMediaType())
		{
			dwBrdBitRate += m_tConf.GetBitRate();
		}
		else
		{
			dwBrdBitRate += m_tConf.GetBitRate() + m_tConf.GetSecBitRate();
		}
        
        
        //���Ҫ����ѡ���������
        if ( 1 )
        {
            //FIMXE: �ݲ����Ǹ�ѡ���������
        }

		//���������յ�
		if (MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType() ||
			0 == m_tConf.GetSecVideoMediaType())
		{
			dwRcvBitRate = m_tConf.GetBitRate();
        }
		else
		{
			dwRcvBitRate = m_tConf.GetBitRate() + m_tConf.GetSecBitRate();
		}
        
		dwBrdBitRate = dwBrdBitRate * 1024;
		dwRcvBitRate = dwRcvBitRate * 1024;

        u8 byBrdNum = 1;    //�㲥Դ����
        
        SendCmdToNms( MCU_NMS_APPLYCONFFREQ_REQ, 
                      byBrdNum,
                      dwBrdBitRate,
                      5,//m_tConf.GetConfAttrbEx().GetSatDCastChnlNum(),
                      dwRcvBitRate,
                      dwFreq, 0);

        g_cMcuVcApp.SetConfBitRate( m_byConfIdx, 0, dwRcvBitRate );
        g_cMcuVcApp.SetConfBitRate( m_byConfIdx, dwBrdBitRate, 0 );

		SetTimer( MCUVC_APPLYFREQUENCE_CHECK_TIMER, TIMESPACE_APPLYFREQUENCE_CHECK );

        //FIXME: ��ʱ�����������ܵ�״̬������
        //NEXTSTATE( STATE_WAITAUTH );
    }
    return;
}


/*====================================================================
 ������      ��ReleaseSatFrequence
 ����        ������������ͷ�����Ƶ��
 �㷨ʵ��    ��
 ����ȫ�ֱ�����
 �������˵����
 ����ֵ˵��  ��
 ----------------------------------------------------------------------
 �޸ļ�¼    ��
 ��  ��      �汾        �޸���        �޸�����
 09/08/28     4.6         �ű���        ����
====================================================================*/
void CMcuVcInst::ReleaseSatFrequence()
{
	u32 dwBrdBitRate;
	u32 dwRcvBitRate;
	u32 dwBrdFreq;
	u32 dwFreq[5];
	TMcuReleaseFreq  tReqFree;
	CServMsg cServMsg;
	u32 dwSn;
	dwSn = GetSerialId();
	switch( CurState() )
	{
	case STATE_IDLE:
	case STATE_MASTER:
	case STATE_ONGOING:
	case STATE_WAITAUTH:
	case STATE_PERIOK:
		
		memset( &tReqFree, 0, sizeof(TMcuReleaseFreq) );
		tReqFree.SetCmd( MCU_NMS_RELEASECONFFREQ_REQ );
		
		//�㲥��
		if ( g_cMcuVcApp.IsConfGetSendFreq( m_byConfIdx ) )
		{
			dwBrdBitRate = g_cMcuVcApp.GetConfSndBitRate( m_byConfIdx );
			dwBrdFreq = g_cMcuVcApp.GetConfSndFreq( m_byConfIdx );
			g_cMcuVcApp.SetConfGetSendFreq( m_byConfIdx, FALSE );
		}
		tReqFree.SetBrdBitRate( dwBrdBitRate );
		tReqFree.SetBrdFreq( dwBrdFreq );
		OspPrintf( TRUE,FALSE,"�ͷ�:�㲥���� - %u �㲥Ƶ�� - %u\n",dwBrdBitRate, dwBrdFreq );

		//���յ�
		if ( g_cMcuVcApp.IsConfGetReceiveFreq( m_byConfIdx ) )
		{
			dwRcvBitRate = g_cMcuVcApp.GetConfRcvBitRate( m_byConfIdx );
			for ( u8 byNum = 0; byNum < m_tConf.GetConfAttrbEx().GetSatDCastChnlNum(); byNum ++)
			{
				dwFreq[byNum] = g_cMcuVcApp.GetConfRcvFreq( m_byConfIdx, byNum );
				OspPrintf( TRUE,FALSE,"�ͷ�:����Ƶ�� - %u\n",dwFreq[byNum] );

				tReqFree.SetFreeFreq( byNum+1, dwFreq[byNum] );
			}
			g_cMcuVcApp.SetConfGetReceiveFreq( m_byConfIdx, FALSE );
		}
		g_cMcuVcApp.ReleaseConfInfo( m_byConfIdx );
		
		
		tReqFree.SetRcvBitRate( dwRcvBitRate );
		tReqFree.SetNum( m_tConf.GetConfAttrbEx().GetSatDCastChnlNum() );
		tReqFree.SetSN( dwSn );
		cServMsg.SetMsgBody( (u8*)&tReqFree, sizeof( TMcuReleaseFreq ) );
		cServMsg.SetConfIdx( m_byConfIdx );
		g_cMcuVcApp.SendMsgToDaemonConf( MCU_NMS_SENDNMSMSG_CMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		
		//SendCmdToNms( MCU_NMS_RELEASECONFFREQ_REQ, 1, dwBrdBitRate,m_tConf.GetConfLineNum(), dwRcvBitRate, dwFreq,dwBrdFreq );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMcuVcInst: Wrong release freq message received in state %u!\n", CurState() );
		break;
	}
	return;
}


/*====================================================================
 ������      :GetSerialId
 ����        :����ͨѶ���к�
 �㷨ʵ��    :
 ����ȫ�ֱ�����
 �������˵����
 ����ֵ˵��  ��
 ----------------------------------------------------------------------
 �޸ļ�¼    ��
 ��  ��      �汾        �޸���        �޸�����
 03/12/19   1.0          zhangsh         ����
 09/08/28   4.6          �ű���          �����ǰ汾��ֲ����
====================================================================*/
u32  CMcuVcInst::GetSerialId()
{
    u32 dwSn;
    u8 byInsId = (u8)GetInsID();
    m_dwSeq++;
    
    dwSn = byInsId << 16;
    dwSn = dwSn + ( m_dwSeq & 0x0000ffff );
    return dwSn;
}


/*====================================================================
������      ProcModemConfRegRsp
����        Modem����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
03/11/11   1.0          zhangsh       ����
====================================================================*/
void CMcuVcInst::ProcModemConfRegRsp( const CMessage * pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    TMt tMt = *(TMt*)cServMsg.GetMsgBody();
    
    switch( CurState() ) 
    {
    case STATE_MASTER:

    //FIXME: �Ժ����ϸ�״̬�� ���� ����
    case STATE_ONGOING:
        
        switch ( pcMsg->event )
        {
        case MODEM_MCU_REG_ACK:
            g_cMcuVcApp.SetMtModemConnected( tMt.GetMtId(), TRUE );
            break;
        case MODEM_MCU_REG_NACK:
            g_cMcuVcApp.SetMtModemConnected( tMt.GetMtId(), FALSE );
            break;
        }
        break;
    default:
        log( LOGLVL_EXCEPTION, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        break;
    }
}

/*====================================================================
������      DaemonProcModemReg
����        ����MODEM����״̬
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const CMessage * pcMsg, �������Ϣ
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
02/08/02    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::DaemonProcModemReg( const CMessage *pcMsg )
{
    u16 wModemId = *(u16*)pcMsg->content;
    if ( wModemId < 17 )
    {
        g_cMcuVcApp.SetMcuModemConnected( wModemId, TRUE );
        g_cMcuVcApp.SetModemSportNum( (u8)wModemId,(u8)wModemId );
    }
}

/*====================================================================
    ������      ��RefreshConfState
    ����        �����»���״̬����ҵ���еĻ�����Ϣת��ΪCSatConfInfo
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/19   1.0          JQL       ����
====================================================================*
void CMcuVcInst::RefreshConfState()
{
	CSatConfInfo cSatConfInfo;

	CServMsg	 cServMsg;
	u8			 bySpeakerId;

	//��������Ա��Ϣ
	u8	byIndex = LOCAL_MCUID /*m_tConf.FindMcu( g_cMcuAgent.GetId() )*;
	TConfMtInfo	tMtInfo;	
	
	if( byIndex == ( u8 )-1 )
	{
		ConfLog( FALSE, "CMcuVcInst: Unexcepted Error In RefreshConfState.\n" );
		return;
	}
	tMtInfo = m_tConfAllMtInfo.GetMtInfo( byIndex );

	cSatConfInfo.SetConfMtInfo( tMtInfo );
	cSatConfInfo.SetConfId( (u8)GetInsID() );

	bySpeakerId = m_tConf.GetSpeaker().GetMtId();

	//�����鲥��ַ MAXNUM_MCU_MT + GetInsId���鲥��ַ

	//GetMtMultiCastAddr( MAXNUM_MCU_MT + GetInsID(), &tSpeakerAddr );

    u32 dwMultiCastAddr = g_cMcuVcApp.GetExistSatCastIp();
    u16 wMultiCastPort = g_cMcuVcApp.GetExistSatCastPort();
	cSatConfInfo.SetBrdMeidaIpAddr( dwMultiCastAddr );
	cSatConfInfo.SetBrdMediaStartPort( wMultiCastPort );

	//���û������ϯ
	cSatConfInfo.SetChairmanId( m_tConf.GetChairman().GetMtId() );
	//���û���ķ�����
	cSatConfInfo.SetSpeakerId( bySpeakerId );
	//���û����ID
	cSatConfInfo.SetConfId( GetInsID() );
	//���û�����
	cSatConfInfo.SetConfName( m_tConf.GetConfName() );
	cSatConfInfo.SetSatIntervTime( m_tConf.GetDuration() / 60 );
	cSatConfInfo.SetConfLineNum( m_tConf.GetConfAttrbEx().GetSatDCastChnlNum() );
	
    //���û����Ƿ��ڻ���
	if( m_tConf.m_tStatus.IsMixing() )
	{
		cSatConfInfo.SetMixing();
	}
	else
	{
		cSatConfInfo.SetNoMixing();
	}

    if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
    {
		cSatConfInfo.SetVmpParam( m_tConf.m_tStatus.m_tVMPParam );
    }

	//���û����Ƿ��ڻ��渴��
	if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE  )//&& m_tConf.m_tStatus.GetVmpParam().IsBrdst() )
	{
		if ( m_tConf.m_tStatus.GetVmpParam().IsVMPBrdst() )
        {
			cSatConfInfo.SetVmpBrdst( TRUE );
        }
		else
        {
			cSatConfInfo.SetVmpBrdst( FALSE );
        }
		cSatConfInfo.SetVmp( TRUE );
	}
	else
	{
		cSatConfInfo.SetVmpBrdst( FALSE );
		cSatConfInfo.SetVmp( FALSE );
	}

	//�����ն˷���ip��ַ
    u8 byMpId = 0;
    for (byMpId = 1; byMpId <= MAXNUM_DRI; byMpId ++)
    {
        if ( g_cMcuVcApp.IsMpConnected(byMpId) )
        {
            break;
        }
    }
	cSatConfInfo.SetMpIpAddress( g_cMcuVcApp.GetMpIpAddr(byMpId) );
	
    //�ն��Ƿ��ڻ�����
	for ( u8 i = 0 ; i < MAXNUM_CONF_MT / 8 ; i++ )
    {
// 		cSatConfInfo.m_byMtInMix[i] = m_tConf.m_tStatus.m_byMtInMix[i];
    }
	//�ش��б�
	for ( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop ++ )
	{
// 		if ( m_tConf.IsMtCurrUpLoad( g_cMcuAgent.GetId(), byLoop ) )
//         {
// 			cSatConfInfo.AddCurrUpLoadMt( byLoop );
//         }
	}
	//�鲥������Ϣ
	cServMsg.SetEventId( 25804 *MCU_MT_CONFSTATUS_NOTIF* );
	cServMsg.SetMsgBody( (u8*)&cSatConfInfo, sizeof( CSatConfInfo ) );
	cServMsg.SetChnIndex( (u8)GetInsID() );
	
	g_cMtSsnApp.SendMultiCastMsg( cServMsg );

    return;
		
}*/


void CMcuVcInst::RefreshConfState( void )
{
	RefreshConfState(TRUE);
	RefreshConfState(FALSE);
}

/*====================================================================
    ������      ��RefreshConfState
    ����        �����»���״̬����ҵ���еĻ�����Ϣת��ΪCSatConfInfo
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/19   1.0          zhangsh       ����
====================================================================*/
void CMcuVcInst::RefreshConfState( BOOL32 bHdGrp )
{
	if (STATE_ONGOING != CurState())
	{
		return;
	}

	CServMsg cServMsg;
	u16 byLoop;
	u16 wPort;
	u8  byFormat;
	cServMsg.SetConfId( m_tConf.GetConfId() );
	
    //��Ϣͷ
	ITSatMsgHdr tMsgHdr;
	tMsgHdr.SetEventId( 25804 /*MCU_MT_CONFSTATUS_NOTIF*/ );
	//tMsgHdr.SetConfIdx((u8)GetInsID());
	tMsgHdr.SetConfIdx(m_byConfIdx);
	
	tMsgHdr.SetDstId( 0xFFFF );
	tMsgHdr.SetSrcId( 0 ); 
	//tMsgHdr.SetMsgSN( g_cMtSsnApp.GetSN() );
	cServMsg.SetMsgBody( (u8*)&tMsgHdr, sizeof( ITSatMsgHdr ) );
	
    //������Ϣ
	ITSatConfInfo   tSatConfInfo;

	if (bHdGrp)
	{
		tSatConfInfo.SetAudioType( m_tConf.GetMainAudioMediaType() );
		tSatConfInfo.SetVideoType( m_tConf.GetMainVideoMediaType() );
		tSatConfInfo.SetBitrate( m_tConf.GetBitRate() );
	}
	else
	{
		if (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() &&
			0 != m_tConf.GetSecVideoMediaType())
		{
			tSatConfInfo.SetVideoType( GetSatMediaType( m_tConf.GetSecVideoMediaType() ) );
		}
		else
		{
			tSatConfInfo.SetVideoType( GetSatMediaType( m_tConf.GetMainAudioMediaType() ) );
		}
		if (MEDIA_TYPE_NULL != m_tConf.GetSecAudioMediaType() &&
			0 != m_tConf.GetSecAudioMediaType())
		{
			tSatConfInfo.SetAudioType( GetSatMediaType( m_tConf.GetSecAudioMediaType() ) );
		}
		else
		{
			tSatConfInfo.SetAudioType( GetSatMediaType( m_tConf.GetMainAudioMediaType() ) );
		}
		
		if (m_tConf.GetSecBitRate() != 0)
		{
			tSatConfInfo.SetBitrate( m_tConf.GetSecBitRate() );
		}
		else
		{
			tSatConfInfo.SetBitrate( m_tConf.GetBitRate() );
		}
	}

	tSatConfInfo.SetChairId( m_tConf.GetChairman().GetMtId() );
	tSatConfInfo.SetConfLineNum( m_tConf.GetConfAttrbEx().GetSatDCastChnlNum() );
	tSatConfInfo.SetConfName( m_tConf.GetConfName() );
	tSatConfInfo.SetConfSwitchTime( m_tConf.GetDuration() );

	if (bHdGrp)
	{
		tSatConfInfo.SetResolution( m_tConf.GetMainVideoFormat() );
	}
	else
	{
		tSatConfInfo.SetResolution( GetSatRes(m_tConf.GetMainVideoFormat()) );
	}
	
	tSatConfInfo.SetSpeakerId( m_tConf.GetSpeaker().GetMtId() );
	
    //----��������
	if ( m_tConf.m_tStatus.IsNoMixing() )
    {
		tSatConfInfo.SetNoMixing();
    }
	else
    {
		tSatConfInfo.SetMixing();
    }
	
	if ( CONF_ENCRYPTMODE_NONE != m_tConf.GetCapSupport().GetEncryptMode() )
    {
		tSatConfInfo.SetConfEncrypted( TRUE );
    }

	if ( CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode() )
	{
		tSatConfInfo.SetVmping();
		tSatConfInfo.m_tVmpMt.SetVmpStyle( m_tConf.m_tStatus.GetVmpParam().GetVMPStyle() );
		tSatConfInfo.m_tVmpMt.SetVmpBrd( m_tConf.m_tStatus.GetVmpParam().IsVMPBrdst() );
		
        for ( byFormat=1 ; byFormat <= m_tConf.m_tStatus.GetVmpParam().GetMaxMemberNum() ; byFormat++ )
        {
			if (byFormat >= 16)
			{
				continue;
			}
			tSatConfInfo.m_tVmpMt.SetMtId( byFormat,
                                           m_tConf.m_tStatus.GetVmpParam().GetVmpMember( byFormat)->GetMtId() );
        }
	}
	else
    {
		tSatConfInfo.SetNoVmping();
    }
	
    //���������ն�
	for ( byLoop = 1; byLoop <= MAXNUM_CONF_MT ; byLoop ++ )
	{
		if ( m_tConfAllMtInfo.MtInConf( byLoop ) )
		{
			if ( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
			{
				BOOL32 bMtHd = ::topoGetMtInfo(LOCAL_MCUID, byLoop, g_atMtTopo, g_wMtTopoNum).IsMtHd();

				if (bMtHd && bHdGrp)
				{
					tSatConfInfo.m_tNonActiveMt.SetMember( byLoop );
					
					if ( m_ptMtTable->IsMtSigned( byLoop ) )
					{
						tSatConfInfo.m_tActiveMt.SetMember( byLoop );
					}
					if ( m_ptMtTable->IsMtCurrUpLoad( byLoop ) )
					{
						tSatConfInfo.m_tCanSendMt.SetMember( byLoop );
					}
				}
				if (!bMtHd && !bHdGrp)
				{
					tSatConfInfo.m_tNonActiveMt.SetMember( byLoop );
					
					if ( m_ptMtTable->IsMtSigned( byLoop ) )
					{
						tSatConfInfo.m_tActiveMt.SetMember( byLoop );
					}
					if ( m_ptMtTable->IsMtCurrUpLoad( byLoop ) )
					{
						tSatConfInfo.m_tCanSendMt.SetMember( byLoop );
					}
				}
			}
		}
		if ( m_ptMtTable->IsMtAudioDumb( byLoop ) )
        {
		    tSatConfInfo.m_tNearMuteMt.SetMember( byLoop );
        }
		if ( m_ptMtTable->IsMtAudioMute( byLoop ) )
        {
			tSatConfInfo.m_tFarMuteMt.SetMember( byLoop );
        }
	}
	
	cServMsg.CatMsgBody( (u8*)&tSatConfInfo, sizeof(ITSatConfInfo) );

	//��ַ��Ϣ
	ITSatConfAddrInfo tAddrInfo;
	tAddrInfo.tAudUnitcastSrcId.Set( 1100 );
	tAddrInfo.tVidUnitcastSrcId.Set( 1101 );

    
	tAddrInfo.tAudMulticastAddr.SetIpAddr(  g_cMcuVcApp.GetMcuMulticastDataIpAddr() );
	tAddrInfo.tAudMulticastAddr.SetPort( g_cMcuVcApp.GetMcuMulticastDataPort() + 2);

	//�ݲ�������Ƶ���䣬��Ƶά�ֵ�һ�鲥��ַ��
	//�Ǹ����ն˴ӵڶ��鲥��ַ������Ƶ����
	/*
	//FIXME: ���鲥
	if (m_tConf.GetSecVideoMediaType() == MEDIA_TYPE_NULL ||
		m_tConf.GetSecVideoMediaType() == 0)
	{
		tAddrInfo.tVidMulticastAddr.SetIpAddr( g_cMcuVcApp.GetMcuMulticastDataIpAddr() );
		tAddrInfo.tVidMulticastAddr.SetPort( g_cMcuVcApp.GetMcuMulticastDataPort() );
	}
	else
	{
		tAddrInfo.tVidMulticastAddr.SetIpAddr( g_cMcuVcApp.GetMcuMulticastDataIpAddr() );
		tAddrInfo.tVidMulticastAddr.SetPort( g_cMcuVcApp.GetMcuMulticastDataPort() );

		tAddrInfo.tSecVidMulticastAddr.SetIpAddr( g_cMcuVcApp.GetMcuSecMulticastIpAddr() );
		tAddrInfo.tSecVidMulticastAddr.SetPort( g_cMcuVcApp.GetMcuMulticastDataPort() );
	}
	*/

	if (IsHDConf(m_tConf))
	{
		if (!bHdGrp)
		{
			tAddrInfo.tVidMulticastAddr.SetIpAddr( g_cMcuVcApp.GetMcuSecMulticastIpAddr() );
			tAddrInfo.tVidMulticastAddr.SetPort( g_cMcuVcApp.GetMcuMulticastDataPort() );
		}
		else
		{
			tAddrInfo.tVidMulticastAddr.SetIpAddr( g_cMcuVcApp.GetMcuMulticastDataIpAddr() );
			tAddrInfo.tVidMulticastAddr.SetPort( g_cMcuVcApp.GetMcuMulticastDataPort() );
		}
	}
	else
	{
		tAddrInfo.tVidMulticastAddr.SetIpAddr( g_cMcuVcApp.GetMcuMulticastDataIpAddr() );
		tAddrInfo.tVidMulticastAddr.SetPort( g_cMcuVcApp.GetMcuMulticastDataPort() );
	}

    u8 byMpId = 0;
	u32 dwMpIp = 0xffffffff;
	for(byMpId = 1; byMpId <= MAXNUM_DRI; byMpId ++)
	{
		if (g_cMcuVcApp.IsMpConnected(byMpId))
		{
			dwMpIp = g_cMcuVcApp.GetMpIpAddr(byMpId);
		}
	}
	if (dwMpIp == 0xffffffff)
	{
		OspPrintf(TRUE, FALSE, "[RefreshConfState] get mp failed, check it!\n");
	}
	tAddrInfo.tMcuRcvMediaAddr.SetIpAddr( dwMpIp );
	
	wPort = g_cMcuVcApp.GetMcuRcvMtMediaStartPort();
	tAddrInfo.tMcuRcvMediaAddr.SetPort( wPort );

	wPort = g_cMcuVcApp.GetMtRcvMcuMediaPort();
	tAddrInfo.tAudUnitcastAddr.SetPort( wPort + 2 );
	tAddrInfo.tVidUnitcastAddr.SetPort( wPort );

	//tAddrInfo.tAudUnitcastAddr.SetIpAddr( dwMpIp );
	//tAddrInfo.tVidUnitcastAddr.SetIpAddr( dwMpIp );


	//���ն˵Ľ������
	if ( m_tConf.HasSpeaker() )
    {
		wPort = m_tConf.GetSpeaker().GetMtId();
    }
	else
    {
		wPort = 0;
    }
	for ( byLoop = 1; byLoop <= MAXNUM_CONF_MT ; byLoop ++ )
	{
		if ( MTSEE_BRDCAST == GetMtSrc( byLoop, MODE_VIDEO) )
        {
			//FIXME: ��ʱ���Ե���Ч��
			tAddrInfo.tVidMulticastMmbList.SetMember( byLoop );
        }
		else if ( MTSEE_UNICAST == GetMtSrc( byLoop, MODE_VIDEO) )
        {
			tAddrInfo.tVidUnitcastMmbList.SetMember( byLoop );
        }
		
		/*
		if ( m_tConf.m_tStatus.IsMonitorOutPut() && byLoop == m_tConf.GetChairman().GetMtId() )
		{
			tAddrInfo.tVidUnitcastMmbList.SetMember( byLoop );
			tAddrInfo.tVidMulticastMmbList.RemoveMember( byLoop );
		}*/


		if ( MTSEE_BRDCAST == GetMtSrc( byLoop, MODE_AUDIO) )
        {
			tAddrInfo.tAudMulticastMmbList.SetMember( byLoop );
        }
		else if ( MTSEE_UNICAST == GetMtSrc( byLoop, MODE_AUDIO) )
        {
			tAddrInfo.tAudUnitcastMmbList.SetMember( byLoop );
        }
	}
	if ( !m_tConf.m_tStatus.IsNoMixing() )
    {
		tAddrInfo.tAudMulticastSrcId.Set( BRDCST_MIXN );
    }
	else if ( m_tConf.HasSpeaker() )
    {
		tAddrInfo.tAudMulticastSrcId.Set( m_tConf.GetSpeaker().GetMtId() );
    }
	else
    {
		tAddrInfo.tAudMulticastSrcId.Set( 0 );
    }

	if ( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE && m_tConf.m_tStatus.GetVmpParam().IsVMPBrdst() )
    {
		tAddrInfo.tVidMulticastSrcId.Set( BRDCST_VMP );
    }
	else if ( m_tConf.HasSpeaker() )
    {
		tAddrInfo.tVidMulticastSrcId.Set( m_tConf.GetSpeaker().GetMtId() );
    }
	else
    {
		tAddrInfo.tVidMulticastSrcId.Set( 0 );
    }

	if (bHdGrp)
	{
		//FIXME: ��ʱ�ֱ��޸ģ��Ժ�ͳһ����
		ITSatConfAddrInfoHd tAddrInfoHd;
		memset(&tAddrInfoHd, 0, sizeof(tAddrInfoHd));
		memcpy(&tAddrInfoHd, &tAddrInfo, sizeof(ITSatConfAddrInfo));

		//����˫������
		tAddrInfoHd.tDSMulticastSrcId.Set(m_tDoubleStreamSrc.GetMtId());
		
		tAddrInfoHd.tDSMulticastAddr.SetIpAddr(g_cMcuVcApp.GetMcuMulticastDataIpAddr());
		tAddrInfoHd.tDSMulticastAddr.SetPort(g_cMcuVcApp.GetMcuMulticastDataPort() + 4);
		
		for ( byLoop = 1; byLoop <= MAXNUM_CONF_MT ; byLoop ++ )
		{
			BOOL32 bMtHd = ::topoGetMtInfo(LOCAL_MCUID, byLoop, g_atMtTopo, g_wMtTopoNum).IsMtHd();

			if ( bMtHd &&
				 MTSEE_BRDCAST == GetMtSrc( byLoop, MODE_SECVIDEO) &&
				 byLoop != m_tDoubleStreamSrc.GetMtId())
			{
				tAddrInfoHd.tDSMulticastMmbList.SetMember( byLoop );
			}
		}

		cServMsg.CatMsgBody( (u8*)&tAddrInfoHd, sizeof(ITSatConfAddrInfoHd) );
	}
	else
	{
		cServMsg.CatMsgBody( (u8*)&tAddrInfo, sizeof(ITSatConfAddrInfo) );
	}
	
	if (bHdGrp)
	{
		cServMsg.SetChnIndex(SIGNAL_CHN_HD);
	}
	else
	{
		cServMsg.SetChnIndex(SIGNAL_CHN_SD);
	}
    g_cMtSsnApp.SendMultiCastMsg( cServMsg );

	SatLog("[RefreshConfState] Conf Info refreshed once manualy!\n" );

    return;
}

/*====================================================================
    ������      ��GetMtSrc
    ����        �����ǻ����ȡ �ն˵�ǰԴ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/11/10    4.6         zhangbq       ����
====================================================================*/
u8 CMcuVcInst::GetMtSrc(u8 byMtId, u8 byMode)
{
	u8 bySeen = MTSEE_NONE;

	if ( 0 == byMtId || byMtId > MAXNUM_CONF_MT )
	{
		ConfLog( FALSE, "[GetMtSrc] unexpected MtId.%d\n", byMtId );
		return bySeen;
	}
	if (byMode != MODE_AUDIO &&
		byMode != MODE_VIDEO &&
		byMode != MODE_SECVIDEO )
	{
		ConfLog( FALSE, "[GetMtSrc] unexpected Mode.%d for MtId.%d\n", byMode, byMtId );
		return bySeen;
	}

	BOOL32 bRet = FALSE;
	TMtStatus tMtStatus;
	bRet = m_ptMtTable->GetMtStatus(byMtId, &tMtStatus);
	if (!bRet)
	{
		ConfLog(FALSE, "[GetMtSrc] get Mt.%d status failed !\n", byMtId);
		return bySeen;
	}

	switch (byMode)
	{
	case MODE_VIDEO:
		if (tMtStatus.GetSelectMt(byMode).IsNull() && !m_tVidBrdSrc.IsNull())
		{
			bySeen = MTSEE_BRDCAST;
		}
		else if (!tMtStatus.GetSelectMt(MODE_VIDEO).IsNull())
		{
			bySeen = MTSEE_UNICAST;
		}
		break;

	case MODE_AUDIO:
		if (tMtStatus.GetSelectMt(byMode).IsNull() && !m_tAudBrdSrc.IsNull())
		{
			bySeen = MTSEE_BRDCAST;
		}
		else if (!tMtStatus.GetSelectMt(MODE_AUDIO).IsNull())
		{
			bySeen = MTSEE_UNICAST;
		}
		break;

	case MODE_SECVIDEO:
		if (!m_tDoubleStreamSrc.IsNull())
		{
			bySeen = MTSEE_BRDCAST;
		}
		break;
	}
	return bySeen;
}

/*====================================================================
    ������      ��DaemonProcTimerMultiCast
    ����        ������MCU������Ϣ
    �㷨ʵ��    ����ʼ�鲥�����б��ն�����
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/19   1.0          zhangsh       ����
====================================================================*/
void CMcuVcInst::DaemonProcTimerMultiCast( const CMessage * pcMsg, CApp* pcApp )
{
	CServMsg cServMsg;
	ITSatMsgHdr tMsgHdr;
	u16	     byIndex;
	u8		 bySend;
	BOOL	bFind = FALSE;
	BOOL	bFindKey = FALSE;
	BOOL bResult = FALSE;
	//�������ԭ�ж�ʱ�������ܴ˺������ò����������Ķ�ʱ������
	KillTimer( MCUVC_CONFINFO_MULTICAST_TIMER );
	m_wTicks ++;
	//������Ϣ
	bySend = m_wTicks % 4;
	if ( bySend == 0 )
	{
		for( byIndex = 0; byIndex < MAXNUM_MCU_CONF+1; byIndex++ )
		{
			CMcuVcInst * pInst = ( CMcuVcInst*)pcApp->GetInstance( byIndex );
			if( pInst != NULL && pInst->CurState() != STATE_IDLE )
			{
				bFind = TRUE;
				break;
			}
		}
		if ( bFind )
			m_bySendBitMap |= 0x01;
		else
			m_bySendBitMap &=0xFE;
	}
	//�����б���Ϣ
	bySend = m_wTicks % 6;
	if ( bySend == 0 )
	{
		m_bySendBitMap |= 0x04;
	}

	//10s��Ҫ����������Ϣ
	if ( m_wTimes == 3 )
	{
		if ( m_wMtKeyNum == 0 )
		{
			m_wTimes++;
			m_wMtKeyNum = 1;
		}
	}
	//20s��Ҫ����TOPO��Ϣ
	if ( m_wTimes == 6 )
	{
		if ( m_wMtTopoNum == 0 )
			m_wMtTopoNum = 1;
	}

	if ( m_wTicks == 7 )
	{
		m_wTicks = m_wTicks % 6;
		m_wTimes ++;
		if ( m_wTimes == 7 )
			m_wTimes = 0;
	}

	//ǿ�Ʒ������鲥��Ϣ
	if (g_cMcuVcApp.IsConfRefreshNeeded())
	{
		for( byIndex = 1; byIndex <= MAXNUM_MCU_CONF; byIndex++ )
		{
			CMcuVcInst * pInst = ( CMcuVcInst*)pcApp->GetInstance( byIndex );
			if( pInst != NULL && pInst->CurState() != STATE_IDLE )
			{
				u8 byRefreshTimes = g_cMcuVcApp.GetConfRefreshTimes(pInst->m_byConfIdx);
				if (byRefreshTimes != 0)
				{
					byRefreshTimes --;
					g_cMcuVcApp.SetConfRefreshTimes(pInst->m_byConfIdx, byRefreshTimes);
					pInst->RefreshConfState();
					
					SatLog( "[TimerMulticast] Conf Info refreshed once, amoung the three times!\n" );
				}
			}
		}
		
		m_bySendBitMap &=0xFE;
		m_bySendBitMap |= 0x02;
		SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );
		return;
		
	}
	
	//���ͻ�����Ϣ�鲥��֪ͨ
	if ( (m_bySendBitMap & 0x01) == 0x01 )
	{
		for( byIndex = 1; byIndex <= MAXNUM_MCU_CONF; byIndex++ )
		{
			CMcuVcInst * pInst = ( CMcuVcInst*)pcApp->GetInstance( byIndex );
			if( pInst != NULL && pInst->CurState() != STATE_IDLE )
			{
				pInst->RefreshConfState();
				SatLog( "[TimerMulticast] Conf Info refreshed once!\n" );
			}
		}

		m_bySendBitMap &=0xFE;
		m_bySendBitMap |= 0x02;
		SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );
		return;
	}


	//������Ϣ���ͺ�Ҫ��������
	if ( ( m_bySendBitMap & 0x02 ) == 0x02 )
	{
		for( byIndex = 1; byIndex < MAXNUM_MCU_CONF+1; byIndex++ )
		{
			CMcuVcInst * pInst = ( CMcuVcInst*)pcApp->GetInstance( byIndex );
			if( pInst != NULL && pInst->CurState() != STATE_IDLE )
			{
				pInst->RefreshMtBitRate();
				SatLog("[TimerMulticast] MT bitrate refreshed once!\n" );
				break;
			}
		}
		m_bySendBitMap &= 0xFD;
		SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );
		return;
	}
	//���ͻ����б�֪ͨ��
	if ( ( m_bySendBitMap & 0x04 ) == 0x04 )
	{

		SatLog( "[TimerMulticast] ConfList refreshed once!\n" );

		tMsgHdr.SetEventId( 25802/*MCU_MT_CONFLIST_NOTIF*/ );
		tMsgHdr.SetConfIdx( 0xFF );
		tMsgHdr.SetDstId( 0xFFFF );
		tMsgHdr.SetSrcId( 0 ); 
		//tMsgHdr.SetMsgSN( g_cMtSsnApp.GetSN() );
		cServMsg.SetMsgBody( (u8*)&tMsgHdr, sizeof( ITSatMsgHdr) );
		
		ITSatTransAddr tMcuSigAddr;
		u32 dwIp;
		u16 wPort;
		dwIp = htonl(g_cMcuAgent.GetMpcIp());
		wPort = g_cMcuVcApp.GetRcvMtSignalPort();
		tMcuSigAddr.SetIpAddr( dwIp );
		tMcuSigAddr.SetPort( wPort );
		cServMsg.CatMsgBody( (u8*)&tMcuSigAddr, sizeof( ITSatTransAddr) );
		
		ITSatConfList acSatConfList[MAXNUM_MCU_CONF];
		memset( &acSatConfList, 0 ,sizeof(acSatConfList) );
		
		u8 byConfNum = 0;
		for( byIndex = 0; byIndex < MAXNUM_MCU_CONF; byIndex++ )
		{
			CMcuVcInst* pInst = ( CMcuVcInst* )pcApp->GetInstance( byIndex );
			
			if( pInst != NULL && pInst->CurState() == STATE_ONGOING )
			{
				acSatConfList[byConfNum].SetConfIdx( (u8)pInst->m_byConfIdx );
				acSatConfList[byConfNum].SetConfName( pInst->m_tConf.GetConfName() );
				byConfNum++;
			}
		}
		cServMsg.SetEventId( 25802/*MCU_MT_CONFLIST_NOTIF*/ );
		cServMsg.CatMsgBody( (u8*)acSatConfList, byConfNum * sizeof( acSatConfList[0] ) );
		cServMsg.SetChnIndex(SIGNAL_CHN_BOTH);
		g_cMtSsnApp.SendMultiCastMsg( cServMsg );
		m_bySendBitMap &=0xFB;
		SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );
		return;
	}

	//����������Ϣ
	if ( m_wMtKeyNum != 0 )
	{
		for( byIndex = 0; byIndex < MAXNUM_MCU_CONF+1; byIndex++ )
		{
			CMcuVcInst* pInst = ( CMcuVcInst* )pcApp->GetInstance( byIndex );
			
			if( pInst != NULL && pInst->CurState() != STATE_IDLE )
			{
				bResult |= pInst->RefreshConfMtKey();
				bFindKey = TRUE;
			}
		}

		if ( bResult )
		{
			m_wMtKeyNum = 0;
		}
		if ( bFindKey )
		{
			SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );
			return;
		}

		
	}

	//��������֪ͨ��	
	if( m_wMtTopoNum != 0 )
	{
		OspPrintf( TRUE, FALSE, "ConfTop - %u\n", m_wMtTopoNum );
		tMsgHdr.SetEventId( 25801/*MCU_MT_TOPO_NOTIF*/ );
		tMsgHdr.SetConfIdx( 0xFF );
		tMsgHdr.SetDstId( 0xFFFF );
		tMsgHdr.SetSrcId( 0 ); 
		//tMsgHdr.SetMsgSN( g_cMtSsnApp.GetSN() );
		cServMsg.SetMsgBody( (u8*)&tMsgHdr, sizeof( ITSatMsgHdr) );
		ITSatMultiPackHeader tHeadr;
		ITSatMtInfo	g_atSatMtTopo[40];
		u8 temp = 0;
		temp = g_wMtTopoNum % 40;
		byIndex = g_wMtTopoNum / 40 ;
		if (temp > 0)
			byIndex += 1;
		tHeadr.SetTotalNum( (u8)byIndex );

		temp = m_wMtTopoNum % 40;
		byIndex = m_wMtTopoNum / 40 ;
		if (temp > 0)
			byIndex += 1;
		tHeadr.SetCurrNum( (u8)byIndex );
		
		temp = 0;
		//��ʼ���
		for( byIndex = 0 ;byIndex < 40 ; byIndex++ )
		{
			g_atSatMtTopo[byIndex].SetMtId( g_atMtTopo[m_wMtTopoNum-1].GetMtId() );
			g_atSatMtTopo[byIndex].SetMtAlias( g_atMtTopo[m_wMtTopoNum-1].GetAlias() );
			g_atSatMtTopo[byIndex].SetMtConnectType( g_atMtTopo[m_wMtTopoNum-1].GetConnected() );
			g_atSatMtTopo[byIndex].SetMtModemIp( htonl( g_atMtTopo[m_wMtTopoNum-1].GetModemIp() ) );
			g_atSatMtTopo[byIndex].SetMtModemPort( g_atMtTopo[m_wMtTopoNum-1].GetModemPort() );
			g_atSatMtTopo[byIndex].SetMtModemType( g_atMtTopo[m_wMtTopoNum-1].GetModemType() );
			g_atSatMtTopo[byIndex].SetMtIp( htonl( g_atMtTopo[m_wMtTopoNum-1].GetIpAddr() ) );
			temp ++;
			m_wMtTopoNum ++;
			if ( (m_wMtTopoNum-1)== g_wMtTopoNum )
			{
				m_wMtTopoNum = 0;
				break;
			}
			if ( temp == 40 )
				break;
		}
		
		tHeadr.SetContentNum( temp );
		//cServMsg.CatMsgBody( (u8*)&g_dwStartTime, sizeof(u32) );

		//FIXME: StartTime ��ʱ���账��
		u32 dwStartTime = 0x12345678;
		cServMsg.CatMsgBody( (u8*)&dwStartTime, sizeof(u32) );
		cServMsg.CatMsgBody( (u8*)&tHeadr, sizeof( ITSatMultiPackHeader ) );
		cServMsg.CatMsgBody( (u8*)&g_atSatMtTopo, sizeof(ITSatMtInfo)*temp );
		cServMsg.SetEventId( 25801/*MCU_MT_TOPO_NOTIF*/ );
		cServMsg.SetChnIndex(SIGNAL_CHN_BOTH);
		g_cMtSsnApp.SendMultiCastMsg( cServMsg );
		SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );
		return;
	}
	//���ö�ʱ��
	SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );
	return;

}

/*====================================================================
 ������      RefreshMtBitRate
 ����        ˢ�������ն�����
 �㷨ʵ��    ��
 ����ȫ�ֱ�����
 �������˵����
 ����ֵ˵��  ��
 ----------------------------------------------------------------------
 �޸ļ�¼    ��
 ��  ��      �汾        �޸���        �޸�����
 03/12/19   1.0          zhangsh       ����
====================================================================*/
void CMcuVcInst::RefreshMtBitRate()
{
	if (STATE_ONGOING != CurState())
	{
		return;
	}

	ITSatMsgHdr tMsgHdr;
	CServMsg cServMsg;
	ITSatBitrate tBitRate;
	u16 wCurAudFlowRate;
	u16 wCurVidFlowRate;
	
	tMsgHdr.SetEventId( 25231/*MCU_MT_SETBITRATE_CMD*/ );
	tMsgHdr.SetConfIdx( 0xFF );
	tMsgHdr.SetDstId( 0xFFFF );
	tMsgHdr.SetSrcId( 0 ); 
	//tMsgHdr.SetMsgSN( g_cMtSsnApp.GetSN() );
	
	cServMsg.SetMsgBody( (u8*)&tMsgHdr, sizeof(ITSatMsgHdr) );
	
	for ( u8 byLoop = 1 ; byLoop < MAXNUM_CONF_MT; byLoop ++ )
	{
		TLogicalChannel tLogicChnVid;
		TLogicalChannel tLogicChnAud;

		BOOL32 bVidRet = m_ptMtTable->GetMtLogicChnnl(byLoop,
												      LOGCHL_VIDEO,
												      &tLogicChnVid, FALSE);

		BOOL32 bAudRet = m_ptMtTable->GetMtLogicChnnl(byLoop,
													  LOGCHL_AUDIO,
													  &tLogicChnAud, FALSE);
		if (!bVidRet && !bAudRet)
		{
			continue;
		}
		wCurAudFlowRate = tLogicChnAud.GetFlowControl();
		wCurVidFlowRate = tLogicChnVid.GetFlowControl();

		if ( wCurAudFlowRate != 0 ||
			 wCurVidFlowRate != 0 )
		{
			//wCurFlowRate = g_cMcuVcApp.GetMtAllowFlowRate( byLoop );
			//wCurFlowRate = m_ptMtTable->GetDialBitrate(byLoop);
			SatLog("[RefreshMtBitRate] Mt.%d's BR has been refreshed to.Aud.%d, Vid.%d!\n", byLoop, wCurAudFlowRate, wCurVidFlowRate );
		}
		else
		{
			//�����˱���
			if (m_tVidBrdSrc == m_ptMtTable->GetMt(byLoop))
			{
				ConfLog( FALSE, "[RefreshMtBitRate] Mt.%d's BR has been refreshed to<Vid.%d, Aud.%d> ignored due to it's speaker!\n", byLoop, wCurVidFlowRate, wCurAudFlowRate );
				continue;
			}
		}

		//FIXME: ��ʱ���ף��Ժ��ն˸�, ��Ϊ��������128k
		if (wCurVidFlowRate == 0)
		{
			wCurAudFlowRate *= 3;
		}


		tBitRate.Set( wCurAudFlowRate + wCurVidFlowRate );

		//zbq ����������ֿ�������ʡ�ŵ�
		cServMsg.SetChnIndex(SIGNAL_CHN_BOTH);
		cServMsg.CatMsgBody( (u8*)&tBitRate, sizeof( ITSatBitrate ) );
	}
	g_cMtSsnApp.SendMultiCastMsg( cServMsg );

	return;
}


/*====================================================================
������      RefreshConfMtKey
����        ����MT��������Ϣ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const CMessage * pcMsg, �������Ϣ
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
03/01/03    1.0        zhangsh        ����
====================================================================*/
BOOL CMcuVcInst::RefreshConfMtKey( )
{
	if (STATE_ONGOING != CurState())
	{
		return FALSE;
	}
	CServMsg cServMsg;
	ITSatMsgHdr tMsgHdr;
	u16 wMtNum;
	BOOL bResult = FALSE;
	
	tMsgHdr.SetEventId( 25007/*MCU_MT_KEY_NOTIF*/ );
	tMsgHdr.SetConfIdx( 0xFF );
	tMsgHdr.SetDstId( 0xFFFF );
	tMsgHdr.SetSrcId( 0 ); 
	//tMsgHdr.SetMsgSN( g_cMtSsnApp.GetSN() );
	cServMsg.SetMsgBody( (u8*)&tMsgHdr, sizeof( ITSatMsgHdr) );
	
	ITSatMultiPackHeader tHeadr;
	ITSatMtKey	g_atSatMtKey[66];
	
	u8 InstId = (u8)GetInsID();
	
	
	u8 temp = 0;
	temp = g_wMtTopoNum % 66;
	u8 byIndex = g_wMtTopoNum / 66 ;
	if (temp > 0)
		byIndex += 1;
	tHeadr.SetTotalNum( (u8)byIndex );
	
	temp = m_wMtKeyNum % 66;
	byIndex = m_wMtKeyNum / 66 ;
	if (temp > 0)
		byIndex += 1;
	tHeadr.SetCurrNum( (u8)byIndex );
	
	temp = 0;
	//��ʼ���
	for( byIndex = 0 ;byIndex < 66; byIndex++ )
	{
		for ( wMtNum = m_wMtKeyNum; wMtNum < MAXNUM_CONF_MT ; wMtNum ++ )
		{
			//FIXME: ��ʱ������MtKey���
			/*
			if ( g_cMcuVcApp.GetMtConfId( wMtNum ) == InstId )
			{
				g_atSatMtKey[byIndex].SetMtKey( wMtNum, (char*)g_cMcuVcApp.GetMtKey( wMtNum ) );
				temp ++;
				m_wMtKeyNum ++;
				break;
			}
			else
			{
				m_wMtKeyNum ++;
			}*/
		}
		if ( temp == 66 )
			break;
	}
	SatLog( "Conf %u MtKey - %u\n" ,InstId, m_wMtKeyNum);
	if ( m_wMtKeyNum == MAXNUM_MCU_MT )
	{
		bResult = TRUE;
		m_wMtKeyNum = 0;
	}
	tHeadr.SetContentNum( temp );
	cServMsg.CatMsgBody( (u8*)&tHeadr, sizeof( ITSatMultiPackHeader ) );
	cServMsg.CatMsgBody( (u8*)&g_atSatMtKey, sizeof(ITSatMtKey)*temp );
	cServMsg.SetEventId( 25007/*MCU_MT_KEY_NOTIF*/ );
	cServMsg.SetChnIndex(SIGNAL_CHN_BOTH);
	g_cMtSsnApp.SendMultiCastMsg( cServMsg );

	return bResult;
}


/*====================================================================
������      ProcModemMcuStatusNotif
����        MODEM״̬�ı�֪ͨ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
03/11/11   1.0          zhangsh       ����
====================================================================*/
void CMcuVcInst::ProcModemMcuStatusNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt tMt = *(TMt*)cServMsg.GetMsgBody();
	BOOL bLastState = FALSE; //g_cMcuVcApp.IsMtModemError( tMt.GetMtId() );
	BOOL bNeedFresh = FALSE;
	switch( CurState() ) 
	{
	case STATE_MASTER:
		switch ( pcMsg->event )
		{
		case MODEM_MCU_ERRSTAT_NOTIF:
			g_cMcuVcApp.SetMtModemStatus( tMt.GetMtId(), TRUE );
			if ( bLastState != TRUE )
			{
				bNeedFresh = TRUE;
			}
			break;
		case MODEM_MCU_RIGHTSTAT_NOTIF:
			g_cMcuVcApp.SetMtModemStatus( tMt.GetMtId(), FALSE );
			if ( bLastState != FALSE )
			{
				bNeedFresh = TRUE;
			}
			break;
		}
		break;
		default:
			log( LOGLVL_EXCEPTION, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
				pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
			break;
	}
	//if ( bNeedFresh )
	MtStatusChange( tMt.GetMtId() );
	//RefreshMtState( tMt.GetMtId() );
}

//END FILE
