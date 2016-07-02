/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcuvceqp.cpp
   ����ļ�    : mcuvc.h
   �ļ�ʵ�ֹ���: MCUҵ�񽻻�����
   ����        : ������
   �汾        : V2.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/06  2.0         ������      ����
   2003/12/17  3.0         zmy         �޸���������������
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
#include "mtadpssn.h"
#include "mpmanager.h"



/*------------------------------------------------------------------*/
/*                                Common                            */
/*------------------------------------------------------------------*/

/*====================================================================
    ������      ��DaemonProcMcsMcuGetPeriEqpStatusReq
    ����        ����ѯ����״̬��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/20    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::DaemonProcMcsMcuGetPeriEqpStatusReq( const CMessage * pcMsg )
{
    CServMsg	cServMsg( pcMsg->content, pcMsg->length );
    TEqp		*ptEqp = ( TEqp * )cServMsg.GetMsgBody();
    TPeriEqpStatus	tEqpStatus;
    TPeriDcsStatus  tDcsStatus;
    
    /*
    //not belong to this MCU, nack
    if( ptEqp->GetMcuId() != LOCAL_MCUID )
    {
        ConfLog( FALSE, "CMcuVcInst: Wrong! The specified periequipment not belong to this MCU!\n" );
        cServMsg.SetErrorCode( ERR_MCU_VISIT_NONCONNEQP );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }
    */

    //��������
    if ( MCS_MCU_GETPERIDCSSTATUS_REQ != pcMsg->event )
    {
        //ERR_MCU_WRONGEQP
        if (ptEqp->GetEqpId() == 0 || ptEqp->GetEqpId() > MAXNUM_MCU_PERIEQP)
        {
            ConfLog(FALSE, "CMcuVcInst: Wrong! The specified periequipment id.%d invaild!\n", ptEqp->GetEqpId());
            cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
        
        //not online
        if( !g_cMcuVcApp.IsPeriEqpConnected( ptEqp->GetEqpId() ) )
        {
            memcpy( &tEqpStatus, ptEqp, sizeof( TEqp ) );
            tEqpStatus.m_byOnline = FALSE;
        }
        else
        {
            g_cMcuVcApp.GetPeriEqpStatus( ptEqp->GetEqpId(), &tEqpStatus );
        }
        
        //send ack
        cServMsg.SetMsgBody( ( u8 * )&tEqpStatus, sizeof( tEqpStatus ) );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
    }
    //DCS
    else
    {
        //ERR_MCU_WRONGDCS
        if (ptEqp->GetEqpId() == 0 || ptEqp->GetEqpId() > MAXNUM_MCU_DCS)
        {
            ConfLog(FALSE, "CMcuVcInst: Wrong! The specified peri dcs id.%d invaild!\n", ptEqp->GetEqpId());
            cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
        
        //not online
        if( !g_cMcuVcApp.IsPeriDcsConnected(ptEqp->GetEqpId()) )
        {
            memcpy( &tDcsStatus, ptEqp, sizeof( TEqp ) );
            tDcsStatus.m_byOnline = FALSE;
        }
        else
        {
            g_cMcuVcApp.GetPeriDcsStatus( ptEqp->GetEqpId(), &tDcsStatus );
        }
        
        //send ack
        cServMsg.SetMsgBody( ( u8 * )&tDcsStatus, sizeof( tDcsStatus ) );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
    }
}

/*====================================================================
    ������      ��DaemonProcPeriEqpMcuStatusNotif
    ����        ������״̬�ϱ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/10    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::DaemonProcPeriEqpMcuStatusNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TPeriEqpStatus tStatus = *(TPeriEqpStatus *)cServMsg.GetMsgBody();

    if (tStatus.GetMcuId() != LOCAL_MCUID ||
        tStatus.GetEqpId() == 0 || tStatus.GetEqpId() > MAXNUM_MCU_PERIEQP)
    {
        OspPrintf(TRUE, FALSE, "[PeriEqpStatusNotif]TPeriEqpStatus content error! GetMcuId() = %d GetEqpId() = %d\n",
        tStatus.GetMcuId(), tStatus.GetEqpId());
        return;
    }

    TPeriEqpStatus tOldStatus;
    memset(&tOldStatus, 0, sizeof(TPeriEqpStatus));
    TPeriEqpStatus tNewHduStatus = tStatus;
	// ֪ͨN+1���ݻ�, zgc, 2008-04-07
	BOOL32 bNotifNPlus = FALSE;

    if (VMP_MCU_VMPSTATUS_NOTIF == pcMsg->event ||
        VMPTW_MCU_VMPTWSTATUS_NOTIF == pcMsg->event)
    {
		//alias //fixme-- zw
		if ( g_cMcuAgent.IsSVmp( tStatus.GetEqpId() ) )
		{				
			TEqpSvmpInfo tEqpSvmpInfo;
			g_cMcuAgent.GetEqpSvmpCfgById( tStatus.GetEqpId(), &tEqpSvmpInfo );
			
			tStatus.SetAlias( tEqpSvmpInfo.GetAlias() );
		}
		
        g_cMcuVcApp.GetPeriEqpStatus(tStatus.GetEqpId(), &tOldStatus);
        if (tOldStatus.IsNull())
        {
            g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tStatus);
			EqpLog( "VMP.%u sub type is:%u\n",tStatus.GetEqpId(),tStatus.m_tStatus.tVmp.m_bySubType);
			EqpLog( "VMP.%u Version is:%u\n", tStatus.GetEqpId(), tStatus.m_tStatus.tVmp.m_byBoardVer);
			// ֪ͨN+1���ݻ�, zgc, 2008-04-07
			bNotifNPlus = TRUE;
        }
        else
        {
			// ֪ͨN+1���ݻ�, zgc, 2008-04-07
			if ( tOldStatus.m_tStatus.tVmp.m_byChlNum != tStatus.m_tStatus.tVmp.m_byChlNum )
			{
				bNotifNPlus = TRUE;
			}
	
            tOldStatus.m_tStatus.tVmp.m_byChlNum = tStatus.m_tStatus.tVmp.m_byChlNum;
            tOldStatus.SetAlias(tStatus.GetAlias());

			tOldStatus.SetEqpType(tStatus.GetEqpType());// xliang [2/12/2009]ȷ��eqptypeֵͳһ�����HDVMP bug
			tOldStatus.m_tStatus.tVmp.m_bySubType = tStatus.m_tStatus.tVmp.m_bySubType;//������ 
			tOldStatus.m_tStatus.tVmp.m_byBoardVer = tStatus.m_tStatus.tVmp.m_byBoardVer;	//A/B������
			tStatus = tOldStatus;
            g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tOldStatus);

			EqpLog( "VMP.%u sub type is:%u\n",tStatus.GetEqpId(),tStatus.m_tStatus.tVmp.m_bySubType);
			EqpLog( "VMP.%u Version is:%u\n", tStatus.GetEqpId(), tStatus.m_tStatus.tVmp.m_byBoardVer);
        }
    }
    else if (TVWALL_MCU_STATUS_NOTIF == pcMsg->event)
    {
        u8 byMtConfIdx;
        g_cMcuVcApp.GetPeriEqpStatus(tStatus.GetEqpId(), &tOldStatus);
        for (u8 byLoop = 0; byLoop < tStatus.m_tStatus.tTvWall.byChnnlNum; byLoop++)
        {
            if (tOldStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType != 0)
            {
                tStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 
                                tOldStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType;

                byMtConfIdx = tOldStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
                tStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(byMtConfIdx);
            }
        }
        g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tStatus);
    }
    else if (HDU_MCU_STATUS_NOTIF == pcMsg->event)    //4.6 jlb
    {
        memset(&tOldStatus, 0, sizeof(tOldStatus));
        g_cMcuVcApp.GetPeriEqpStatus(tStatus.GetEqpId(), &tOldStatus);
        for(u8 byLoop = 0; byLoop < MAXNUM_HDU_CHANNEL; byLoop++)
        {
            if (tOldStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType != 0)
            {
                tStatus.m_tStatus.tHdu.atVideoMt[byLoop] =
                    tOldStatus.m_tStatus.tHdu.atVideoMt[byLoop];
            }
			if ( tOldStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].GetIsMute() || 
				  tOldStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].GetVolume() != 0 )
			{
				tStatus.m_tStatus.tHdu.atHduChnStatus[byLoop] = 
					tOldStatus.m_tStatus.tHdu.atHduChnStatus[byLoop];
				tNewHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop] = 
					tOldStatus.m_tStatus.tHdu.atHduChnStatus[byLoop];
			}
			tStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetChnIdx( byLoop );
			tNewHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetChnIdx( byLoop );
        }
		if (tOldStatus.GetEqpType() != EQP_TYPE_HDU)
		{
			tStatus.SetEqpType(EQP_TYPE_HDU);
			tNewHduStatus.SetEqpType( EQP_TYPE_HDU );
		}

        g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tStatus);
    }
    else if (MIXER_MCU_MIXERSTATUS_NOTIF == pcMsg->event)
    {
        TMixerGrpStatus tOldMixerGrpStatus;
        g_cMcuVcApp.GetPeriEqpStatus(tStatus.GetEqpId(), &tOldStatus);

        for (u8 byLoop = 0; byLoop < tStatus.m_tStatus.tMixer.m_byGrpNum; byLoop++)
        {
            tOldMixerGrpStatus = tOldStatus.m_tStatus.tMixer.m_atGrpStatus[byLoop];
            tStatus.m_tStatus.tMixer.m_atGrpStatus[byLoop].m_byGrpId = tOldMixerGrpStatus.m_byGrpId;
            tStatus.m_tStatus.tMixer.m_atGrpStatus[byLoop].m_byGrpMixDepth = tOldMixerGrpStatus.m_byGrpMixDepth;
            //tStatus.m_tStatus.tMixer.m_atGrpStatus[byLoop].m_byGrpState = tOldMixerGrpStatus.m_byGrpState;
            tStatus.m_tStatus.tMixer.m_atGrpStatus[byLoop].m_byConfId = tOldMixerGrpStatus.m_byConfId;
        }

        g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tStatus);
    }
    else if (PRS_MCU_PRSSTATUS_NOTIF == pcMsg->event)
    {
        TPeriEqpStatus tOldPrsStatus;
        g_cMcuVcApp.GetPeriEqpStatus(tStatus.GetEqpId(), &tOldPrsStatus);

        // ���� [6/14/2006] �ָ��Ƿ��� IsReserved �ֶΡ����ֶι�MCU����Prs��֪��
        for (u8 byLoop = 0; byLoop < MAXNUM_PRS_CHNNL; byLoop ++)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[byLoop].  \
                SetReserved( tOldPrsStatus.m_tStatus.tPrs.m_tPerChStatus[byLoop].IsReserved() );
			//�ָ�ÿ��ͨ������Ļ���Idx, zgc, 2007/04/24
			tStatus.m_tStatus.tPrs.SetChnConfIdx( byLoop, tOldPrsStatus.m_tStatus.tPrs.GetChnConfIdx( byLoop ) );
        }

        g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tStatus);
    }
    else
    {
        // ���� [6/14/2006] ĿǰBasֻ��������ʱ�ᷢ��״̬֪ͨ��������Ϊ�����mcu�ڲ�������߼���ɳ��
        // ��˲���Ҫ��PRSһ��
        if ( !g_cMcuAgent.IsEqpBasHD( tStatus.GetEqpId() ) )
        {
            g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tStatus);
        }
        else
        {
            //������������״ֻ̬֪ͨ��EQP,�Ƿ����ߺͱ�������Ч��, zgc, 2008-09-03
            TPeriEqpStatus tOldStatus;
            g_cMcuVcApp.GetPeriEqpStatus(tStatus.GetEqpId(), &tOldStatus);
            tOldStatus.SetMcuEqp( tStatus.GetMcuId(), tStatus.GetEqpId(), tStatus.GetEqpType() );
            tOldStatus.m_byOnline = tStatus.m_byOnline;
            tOldStatus.SetAlias( tStatus.GetAlias() );
            
            //����mau����
            if (tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->IsReserved() ||
                tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->IsReserved())
            {
                if (tOldStatus.m_tStatus.tHdBas.GetEqpType() != tStatus.m_tStatus.tHdBas.GetEqpType())
                {
                    tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->SetIsReserved(FALSE);
                    tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->SetIsReserved(FALSE);
                }
            }
            if (THDBasChnStatus::IDLE == tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->GetStatus())
            {
                tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->SetStatus(THDBasChnStatus::READY);
            }
            if (THDBasChnStatus::IDLE == tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->GetStatus())
            {
                tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->SetStatus(THDBasChnStatus::READY);
            }
            tOldStatus.m_tStatus.tHdBas.SetEqpType(tStatus.m_tStatus.tHdBas.GetEqpType());
            g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tOldStatus);
        }
		// ֪ͨN+1���ݻ�
		bNotifNPlus = TRUE;
    }

    //status notification
	if ( EQP_TYPE_HDU == tStatus.GetEqpType())
	{
		cServMsg.SetMsgBody( (u8*)&tNewHduStatus, sizeof(tNewHduStatus) );
	}
	else
	{
        cServMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
    }

	SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);

	// zgc 2008-04-02 N+1ģʽ��֪ͨ��mcu��������
    // zgc 2008-08-08 ����֪ͨ��������(Ŀǰ��ʱע��)
    if ( VMP_MCU_VMPSTATUS_NOTIF == pcMsg->event &&
		 BAS_MCU_BASSTATUS_NOTIF == pcMsg->event &&
         HDBAS_MCU_BASSTATUS_NOTIF == pcMsg->event && 
		g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_MASTER_CONNECTED)
    {
		if ( bNotifNPlus )
        {
			TNPlusEqpCap tEqpCap = g_cNPlusApp.GetMcuEqpCap();
			cServMsg.SetEventId(MCU_NPLUS_EQPCAP_NOTIF);
			cServMsg.SetMsgBody((u8 *)&tEqpCap, sizeof(tEqpCap));
			g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
    }

    g_cMcuVcApp.BroadcastToAllConf(pcMsg->event, pcMsg->content, pcMsg->length);

    return;
}

/*====================================================================
    ������      ��DaemonProcMcuEqpConnectedNotif
    ����        ������ע��ɹ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/10    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::DaemonProcMcuEqpConnectedNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TPeriEqpRegReq tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();
    TLogicalChannel tLogicalChannel;

    //not connected periequipment
    if (LOCAL_MCUID != tRegReq.GetMcuId())
    {
        return;
    }

    //�õ������������ݵ�Mp��ַ
    u32 dwRecvIpAddr;
    u32 dwEqpAddr;
    u16 wRecvPort;
    TEqp tEqp = (TEqp)tRegReq;
    g_cMpManager.GetSwitchInfo(tEqp, dwRecvIpAddr, wRecvPort, dwEqpAddr);
	CallLog("[DaemonProcMcuEqpConnectedNotif] MP RecvPort is %d\n",wRecvPort);

    //����ǰ����Ƶͨ��
    tLogicalChannel.m_tRcvMediaChannel.SetIpAddr(ntohl(tRegReq.GetPeriEqpIpAddr()));
    tLogicalChannel.m_tRcvMediaChannel.SetPort(tRegReq.GetStartPort());
    tLogicalChannel.m_tRcvMediaCtrlChannel.SetIpAddr(ntohl(tRegReq.GetPeriEqpIpAddr()));
    tLogicalChannel.m_tRcvMediaCtrlChannel.SetPort(tRegReq.GetStartPort() + 1);
    tLogicalChannel.m_tSndMediaCtrlChannel.SetIpAddr(dwRecvIpAddr);	//��0����ʱ����
    tLogicalChannel.m_tSndMediaCtrlChannel.SetPort(tRegReq.GetStartPort() + 1);
    g_cMcuVcApp.SetPeriEqpLogicChnnl(tRegReq.GetEqpId(), MODE_VIDEO,
                                     tRegReq.GetChnnlNum(), &tLogicalChannel, TRUE);

    u32 dwIpAddr = tRegReq.GetPeriEqpIpAddr();
    g_cMcuVcApp.SetEqpIp(tRegReq.GetEqpId(), dwIpAddr);

    //����ǰ����Ƶͨ��
    //����PRS���ԣ�һ��PRS��֧�ֶ������Ƶͨ��(Ĭ���ܹ�16��),
    //��ͨ�����ն˿�ͳһΪPRS��ʼ���ն˿ڣ���ͨ��������ʵ�ʵĽ��ն˿�
    if (EQP_TYPE_PRS == tRegReq.GetEqpType())
    {
        tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( ntohl(tRegReq.GetPeriEqpIpAddr()) );
        tLogicalChannel.m_tRcvMediaChannel.SetPort( tRegReq.GetStartPort() );
        tLogicalChannel.m_tRcvMediaCtrlChannel.SetIpAddr( ntohl(tRegReq.GetPeriEqpIpAddr()) );
        tLogicalChannel.m_tRcvMediaCtrlChannel.SetPort( tRegReq.GetStartPort() + 1 );
        tLogicalChannel.m_tSndMediaCtrlChannel.SetIpAddr( dwRecvIpAddr );	//��0����ʱ����
        tLogicalChannel.m_tSndMediaCtrlChannel.SetPort( tRegReq.GetStartPort() + 1 );
    }
    else
    {
        tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( ntohl(tRegReq.GetPeriEqpIpAddr()) );
        tLogicalChannel.m_tRcvMediaChannel.SetPort( tRegReq.GetStartPort() + 2 );
        tLogicalChannel.m_tRcvMediaCtrlChannel.SetIpAddr( ntohl(tRegReq.GetPeriEqpIpAddr()) );
        tLogicalChannel.m_tRcvMediaCtrlChannel.SetPort( tRegReq.GetStartPort() + 3 );
        tLogicalChannel.m_tSndMediaCtrlChannel.SetIpAddr( dwRecvIpAddr );	//��0����ʱ����
        tLogicalChannel.m_tSndMediaCtrlChannel.SetPort( tRegReq.GetStartPort() + 3 );
    }       
    g_cMcuVcApp.SetPeriEqpLogicChnnl(tRegReq.GetEqpId(), MODE_AUDIO,
                                     tRegReq.GetChnnlNum(), &tLogicalChannel, TRUE);

    //��������״̬
    g_cMcuVcApp.SetPeriEqpConnected(tRegReq.GetEqpId(), TRUE);
    g_cMcuVcApp.SetEqpAlias(tRegReq.GetEqpId(), tRegReq.GetEqpAlias());

    //֪ͨ���л���
    switch(tRegReq.GetEqpType())
    {
    case EQP_TYPE_MIXER:
        g_cMcuVcApp.BroadcastToAllConf(MCU_MIXERCONNECTED_NOTIF, pcMsg->content, pcMsg->length);
        break;
    case EQP_TYPE_BAS:
        g_cMcuVcApp.BroadcastToAllConf(MCU_BASCONNECTED_NOTIF, pcMsg->content, pcMsg->length);
        break;
    case EQP_TYPE_PRS:
        g_cMcuVcApp.BroadcastToAllConf(MCU_PRSCONNECTED_NOTIF, pcMsg->content, pcMsg->length);
        break;
    case EQP_TYPE_TVWALL:   //tvwall2
        g_cMcuVcApp.BroadcastToAllConf(MCU_TVWALLCONNECTED_NOTIF, pcMsg->content, pcMsg->length);
        break;
    case EQP_TYPE_RECORDER:
        g_cMcuVcApp.BroadcastToAllConf(MCU_RECCONNECTED_NOTIF, pcMsg->content, pcMsg->length);
        break;
    case EQP_TYPE_VMP:   //vmp
        g_cMcuVcApp.BroadcastToAllConf(MCU_VMPCONNECTED_NOTIF, pcMsg->content, pcMsg->length);
        break;
	case EQP_TYPE_HDU:    //4.6  �¼� jlb
		g_cMcuVcApp.BroadcastToAllConf( MCU_HDUCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;
    case EQP_TYPE_VMPTW:   //vmptw
        g_cMcuVcApp.BroadcastToAllConf(MCU_VMPTWCONNECTED_NOTIF, pcMsg->content, pcMsg->length);
        break;
    default:
        break;
    }

    // xsl [11/29/2006] N+1ģʽ��֪ͨ��mcu��������
    if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_MASTER_CONNECTED)
    {
        TNPlusEqpCap tEqpCap = g_cNPlusApp.GetMcuEqpCap();
        cServMsg.SetEventId(MCU_NPLUS_EQPCAP_NOTIF);
        cServMsg.SetMsgBody((u8 *)&tEqpCap, sizeof(tEqpCap));
        g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }

    // zbq[08/18/2009] ���ؾ������ش���
    u8 byBandReserved = 0;
    u8 bySwitchMpId = 0;
    if (
        (EQP_TYPE_BAS == tRegReq.GetEqpType() &&
         g_cMcuAgent.IsEqpBasHD(tRegReq.GetEqpId()) &&
         g_cMcuVcApp.GetBandWidthReserved4HdBas())
         ||
        (EQP_TYPE_VMP == tRegReq.GetEqpType() &&
         g_cMcuAgent.IsSVmp(tRegReq.GetEqpId()) &&
         g_cMcuVcApp.GetBandWidthReserved4HdVmp())
       )
    {
        byBandReserved = g_cMcuVcApp.GetBandWidthReserved4HdBas();
        if (0 == byBandReserved)
        {
            return;
        }
        bySwitchMpId = g_cMcuVcApp.FindMp(dwRecvIpAddr);
        g_cMcuVcApp.m_atMpData[bySwitchMpId-1].m_wNetBandReserved += byBandReserved;

        //�����ۼ�Ԥ�����µġ���������������ʾ
        if (byBandReserved <= g_cMcuVcApp.m_atMpData[bySwitchMpId-1].m_wNetBandAllowed &&
            g_cMcuVcApp.m_atMpData[bySwitchMpId-1].m_wNetBandReserved >
            g_cMcuVcApp.m_atMpData[bySwitchMpId-1].m_wNetBandAllowed)
        {
            NotifyMcsAlarmInfo(0, ERR_MCU_MPRESERVEDBANDWIDTH_FULL);
        }
    }
    
    return;
}

/*====================================================================
    ������      ��DaemonProcMcuEqpDisconnectedNotif
    ����        ���������֪ͨ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/10    1.0         LI Yi         ����
	04/03/27    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::DaemonProcMcuEqpDisconnectedNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TEqp	tEqp = *( TEqp * )cServMsg.GetMsgBody();
	TLogicalChannel tLogicalChannel;
	TPeriEqpStatus	tStatus;

    if (0 == tEqp.GetEqpId() || MAXNUM_MCU_PERIEQP < tEqp.GetEqpId())
    {
        OspPrintf(TRUE, FALSE, "[PeriEqpDisconnectedNotif]TPeriEqpStatus content error! GetEqpId() = %d\n",
                                tStatus.GetEqpId());
        return;
    }

	//�����Ϣ
	g_cMcuVcApp.SetPeriEqpConnected( tEqp.GetEqpId(), FALSE );

	//֪ͨ���
	tStatus.SetMcuEqp( tEqp.GetMcuId(), tEqp.GetEqpId(), tEqp.GetEqpType() );
	tStatus.m_byOnline = FALSE;
	cServMsg.SetMsgBody( ( u8 * )&tStatus, sizeof( tStatus ) );
	SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );

	if( EQP_TYPE_VMP == tEqp.GetEqpType() )
	{
		//tStatus.SetNull();
		//g_cMcuVcApp.SetPeriEqpStatus( tEqp.GetEqpId(), &tStatus );
	}

	//֪ͨ���л��� 
	switch( tEqp.GetEqpType() )
	{
	case EQP_TYPE_RECORDER:
		g_cMcuVcApp.BroadcastToAllConf( MCU_RECDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;
	case EQP_TYPE_BAS:
		g_cMcuVcApp.BroadcastToAllConf( MCU_BASDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;
	case EQP_TYPE_MIXER:
		g_cMcuVcApp.BroadcastToAllConf( MCU_MIXERDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;
	case EQP_TYPE_VMP:
	    g_cMcuVcApp.BroadcastToAllConf( MCU_VMPDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;    
	case EQP_TYPE_VMPTW:
		g_cMcuVcApp.BroadcastToAllConf( MCU_VMPTWDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;
	case EQP_TYPE_PRS:
		g_cMcuVcApp.BroadcastToAllConf( MCU_PRSDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;
	case EQP_TYPE_TVWALL:
		g_cMcuVcApp.BroadcastToAllConf( MCU_TVWALLDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;
	case EQP_TYPE_HDU:
		g_cMcuVcApp.BroadcastToAllConf( MCU_HDUDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
	    break;

	default:
		break;
	}
    
    // N+1ģʽ��֪ͨ��mcu����������� [12/21/2006-zbq]
    if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_MASTER_CONNECTED)
    {
        TNPlusEqpCap tEqpCap = g_cNPlusApp.GetMcuEqpCap();
        cServMsg.SetEventId(MCU_NPLUS_EQPCAP_NOTIF);
        cServMsg.SetMsgBody((u8 *)&tEqpCap, sizeof(tEqpCap));
        g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }
}



/*------------------------------------------------------------------*/
/*                              Recorder                            */
/*------------------------------------------------------------------*/

/*====================================================================
    ������      ��ProcMcsMcuStartRecReq
    ����        ����ʼ����¼������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/13    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuStartRecReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8   byEqpType;
    u32  dwIpAddress;
    BOOL32 bRecordConf;
    
    TMt tOldRecordMt = *(TMt *)cServMsg.GetMsgBody();
    TMt tRecordMt = GetLocalMtFromOtherMcuMt(tOldRecordMt);
    
    TEqp tEqp = *(TEqp *)(cServMsg.GetMsgBody()+sizeof(TMt));
    TRecStartPara tRecPara = *(TRecStartPara *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TEqp));
    
    //���������ʸ���¼���
    tRecPara.SetBitRate(m_tConf.GetBitRate());
    tRecPara.SetSecBitRate(GetDoubleStreamVideoBitrate(m_tConf.GetBitRate(), FALSE));
    // guzh [6/9/2007] ֻ֧��¼����ʽ
    tRecPara.SetIsRecMainVideo(TRUE);
    tRecPara.SetIsRecMainAudio(TRUE);

    s8 aszRecName[KDV_MAX_PATH] = {0};
    strncpy(aszRecName,
            (s8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TEqp) + sizeof(TRecStartPara)),
            KDV_MAX_PATH-1);
    
    aszRecName[KDV_MAX_PATH-1] = 0;
    
    s8 szRecFullName[KDV_MAX_PATH] = {0};
    // ƴ��·����¼���
    //sprintf(szRecFullName, "%s/%s", m_tConf.GetConfName(), aszRecName);
    // guzh [9/7/2006] ����֧��·��
    sprintf(szRecFullName, "%s", aszRecName);

    u8   byNeedPrs;
    TRecRtcpBack tRtcpBack;
    TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
    TDoublePayload tDVPayload;   //��Ƶ�غ�ֵ
    TDoublePayload tDAPayload;   //��Ƶ�غ�ֵ
    TDoublePayload tDDVPayload;  //˫����Ƶ�غ�ֵ
    u8 byConfVideoType;
    u8 byConfAudioType;
	TCapSupportEx tCapSupportEx;

    // ���� [7/18/2006]  ע�⣬��ʵĿǰֻ֧��¼����ʽ
    if (tRecPara.IsRecMainVideo())
    {
        byConfVideoType = m_tConf.GetMainVideoMediaType();
    }
    else
    {
        byConfVideoType = m_tConf.GetSecVideoMediaType();
    }

    if (tRecPara.IsRecMainAudio())
    {
        byConfAudioType = m_tConf.GetMainAudioMediaType();
    }
    else
    {
        byConfAudioType = m_tConf.GetSecAudioMediaType();
    }

    //�ж��Ƿ��ǶԻ���¼��
    bRecordConf = (tRecordMt.GetMcuId() == 0);    
	
	switch(CurState())
	{	
	case STATE_ONGOING:
        {
		//����Ϸ����ж�
		g_cMcuAgent.GetPeriInfo(tEqp.GetEqpId(), &dwIpAddress, &byEqpType);
		
		if (byEqpType != EQP_TYPE_RECORDER || !g_cMcuVcApp.IsPeriEqpConnected(tEqp.GetEqpId()))
		{
			EqpLog( "CMcuVcInst: This MCU's recorder is invalid or offline now!\n");		
			cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
			cServMsg.SetMsgBody((u8*)&tRecordMt, sizeof(tRecordMt));
			cServMsg.CatMsgBody((u8*)&tEqp, sizeof(tEqp) );
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
		}
		
		if (bRecordConf)
		{
            //recording now, Nack			
			if (!m_tConf.m_tStatus.IsNoRecording())
			{
				McsLog( "CMcuVcInst: This conference is recording now!\n");
				cServMsg.SetErrorCode(ERR_MCU_CONFRECORDING);
				cServMsg.SetMsgBody((u8*)&tRecordMt, sizeof(tRecordMt));
				cServMsg.CatMsgBody((u8*)&tEqp, sizeof(tEqp));
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}

#ifdef _MINIMCU_
            if ( ISTRUE(m_byIsDoubleMediaConf) && tRecPara.IsRecLowStream() )
            {
                McsLog( "CMcuVcInst: This conference not support record low stream!\n");
                cServMsg.SetErrorCode(ERR_MCU_RECLOWNOSUPPORT);
                cServMsg.SetMsgBody((u8*)&tRecordMt, sizeof(tRecordMt));
                cServMsg.CatMsgBody((u8*)&tEqp, sizeof(tEqp));
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
            }
#endif
            // guzh [6/9/2007] �������¼�����
            m_tRecPara = tRecPara;

            //����¼��ʱ������˫��ʽ��¼����ʽ�����������ڶ�̬�غɣ�������ʽ�л�ʱ���ܴ�������룿��
			if (MEDIA_TYPE_H264 == byConfVideoType || 
				MEDIA_TYPE_H263PLUS == byConfVideoType || 
				CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] ��ӦFEC֧��������ʽ
                m_tConf.GetCapSupportEx().IsVideoSupportFEC())
			{
				tDVPayload.SetRealPayLoad(byConfVideoType );
				tDVPayload.SetActivePayload(GetActivePayload(m_tConf, byConfVideoType));
			}
			else
			{
				tDVPayload.SetRealPayLoad(byConfVideoType);
				tDVPayload.SetActivePayload(byConfVideoType);
			}
			if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] ��ӦFEC֧��������ʽ
                m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
			{
				tDAPayload.SetRealPayLoad(byConfAudioType);
				tDAPayload.SetActivePayload(GetActivePayload(m_tConf, byConfAudioType));
			}
			else
			{
				tDAPayload.SetRealPayLoad(byConfAudioType);
				tDAPayload.SetActivePayload(byConfAudioType);
			}
		}
		else    // �ն�¼��
		{
			if (!m_tConfAllMtInfo.MtJoinedConf(tRecordMt.GetMtId()))
			{
				ConfLog(FALSE, "[ProcMcsMcuStartRecReq] This mt %d is not joined in conference.\n", tRecordMt.GetMtId() );
				cServMsg.SetErrorCode(ERR_MCU_MT_NOTINCONF);
				cServMsg.SetMsgBody((u8*)&tRecordMt, sizeof(tRecordMt));
				cServMsg.CatMsgBody((u8*)&tEqp, sizeof(tEqp));
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;			
			}

			if (!m_ptMtTable->IsMtNoRecording(tRecordMt.GetMtId()))
			{
				ConfLog(FALSE, "[ProcMcsMcuStartRecReq] This mt %d is recording now.\n", tRecordMt.GetMtId());
				cServMsg.SetErrorCode(ERR_MCU_MTRECORDING);
				cServMsg.SetMsgBody((u8*)&tRecordMt, sizeof(tRecordMt));
				cServMsg.CatMsgBody((u8*)&tEqp, sizeof(tEqp) );
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}

            // xsl [7/20/2006] ���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                if (IsOverSatCastChnnlNum(tRecordMt.GetMtId()))
                {
                    ConfLog(FALSE, "[ProcMcsMcuStartRecReq] over max upload mt num. nack!\n");            
                    cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                    cServMsg.SetMsgBody((u8*)&tRecordMt, sizeof(tRecordMt));
				    cServMsg.CatMsgBody((u8*)&tEqp, sizeof(tEqp));
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
            }

			//��̬�غ���Ϣ
			TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS(tRecordMt.GetMtId());
			if (MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType() ||
				MEDIA_TYPE_H263PLUS == tSrcSCS.GetVideoMediaType() ||
				CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] ��ӦFEC֧��������ʽ
                m_tConf.GetCapSupportEx().IsVideoSupportFEC() )
			{
				tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType());
				tDVPayload.SetActivePayload(GetActivePayload(m_tConf, tSrcSCS.GetVideoMediaType()));
			}
			else
			{
				tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType());
				tDVPayload.SetActivePayload(tSrcSCS.GetVideoMediaType());
			}
			if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] ��ӦFEC֧��������ʽ
                m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
			{
				tDAPayload.SetRealPayLoad(tSrcSCS.GetAudioMediaType());
				tDAPayload.SetActivePayload(GetActivePayload(m_tConf,  tSrcSCS.GetAudioMediaType()));
			}
			else
			{
				tDAPayload.SetRealPayLoad(tSrcSCS.GetAudioMediaType());
				tDAPayload.SetActivePayload(tSrcSCS.GetAudioMediaType());
			}
		}
	
        //tDDVPayload ��̬�غ���Ϣ
        if (tRecPara.IsRecDStream())
        {
            u8 DStreamType = m_tConf.GetCapSupport().GetDStreamMediaType();
            
			if ( m_tConf.GetCapSupportEx().IsDDStreamCap() ) // ˫˫��ʱ��̬�غ���Ϊ0; jlb [2009/03/31]
			{
                tDDVPayload.SetActivePayload( 0 );
			}
			else if (MEDIA_TYPE_H264 == DStreamType ||
                MEDIA_TYPE_H263PLUS == DStreamType ||
                CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] ��ӦFEC֧��������ʽ
                m_tConf.GetCapSupportEx().IsDVideoSupportFEC())
            {
                tDDVPayload.SetActivePayload(GetActivePayload(m_tConf, DStreamType));
            }
            else
            {
                tDDVPayload.SetActivePayload(DStreamType);
            }
            tDDVPayload.SetRealPayLoad(DStreamType);
            EqpLog("DoubleStreamSrc MediaType = %d\n", DStreamType);
        }

        //prs
        ///FIXME: guzh [6/12/2007] �������PrsԴ��©��������û�п��ǹ㲥Դ�仯�������������ʱ��֧��
        /*
        if (m_tConf.GetConfAttrb().IsResendLosePack())
        {
            TMt tRecSrc;
            u32 dwVideoSwitchIp;
            u32 dwAudioSwitchIp;
            u32 dwDStreamSwitchIp;
            u16 wMtSwitchPort;
            u32 dwMtSrcIp;

            if (bRecordConf)
            {                                
                //video
                IsRecordSrcBas(MODE_VIDEO, tRecSrc);
                u8 bySrcChnnl = (tRecSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
                g_cMpManager.GetSwitchInfo(tRecSrc, dwVideoSwitchIp, wMtSwitchPort, dwMtSrcIp);

                //double
                if (!m_tDoubleStreamSrc.IsNull() && m_tRecPara.IsRecDStream())
                {
                    g_cMpManager.GetSwitchInfo(m_tDoubleStreamSrc, dwDStreamSwitchIp, wMtSwitchPort, dwMtSrcIp);
                }
							
				//audio
                IsRecordSrcBas(MODE_AUDIO, tRecSrc);
                g_cMpManager.GetSwitchInfo(tRecSrc, dwAudioSwitchIp, wMtSwitchPort, dwMtSrcIp);
            }
            else
            {
                g_cMpManager.GetSwitchInfo(tRecordMt, dwVideoSwitchIp, wMtSwitchPort, dwMtSrcIp);
                dwAudioSwitchIp = dwVideoSwitchIp;
            }

            tRtcpBack.SetVideoAddr(dwVideoSwitchIp, 0);
            McsLog("[SetVideoAddr]IpAddr(0x%x), Port(%d)\n", dwVideoSwitchIp, 0);
        
            tRtcpBack.SetAudioAddr(dwAudioSwitchIp, 0);
            McsLog("[SetAudioAddr]IpAddr(0x%x), Port(%d)\n", dwAudioSwitchIp, 0);

            if (!m_tDoubleStreamSrc.IsNull() && m_tRecPara.IsRecDStream())
            {
                tRtcpBack.SetDStreamAddr(dwDStreamSwitchIp, 0);
                McsLog("[SetDStreamAddr]IpAddr(0x%x), Port(%d)\n", dwDStreamSwitchIp, 0);
            }

            byNeedPrs = 1;
        }
        else
        */
        {
            byNeedPrs = 0;
            memset(&tRtcpBack, 0, sizeof(tRtcpBack));
        }

        cServMsg.SetMsgBody((u8 *)&tOldRecordMt, sizeof(TMt));
		cServMsg.CatMsgBody((u8 *)&tEqp, sizeof(tEqp));
		cServMsg.CatMsgBody((u8 *)&tRecPara, sizeof(tRecPara));
		cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs));
		cServMsg.CatMsgBody((u8 *)&tRtcpBack, sizeof(tRtcpBack));
		cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //video encrypt
		cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload)); //video payload
		cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //audio encrypt
		cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload)); //audio payload
		cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //double video encrypt
		cServMsg.CatMsgBody((u8 *)&tDDVPayload, sizeof(tDDVPayload)); //double video payload

        // MCUǰ�����, zgc, 2007-09-28
        tCapSupportEx = m_tConf.GetCapSupportEx();
        cServMsg.CatMsgBody((u8 *)&tCapSupportEx, sizeof(tCapSupportEx));

        u16 wRecNameLen = htons(strlen(szRecFullName) + 1);
        cServMsg.CatMsgBody((u8 *)&wRecNameLen, sizeof(u16));
		cServMsg.CatMsgBody((u8 *)szRecFullName, strlen(szRecFullName) + 1);
        
        // zbq[11/12/2007] ��ʢOEM¼�����չ֧��
        u16 wConfInfoLen = htons(sizeof(TConfInfo));
        cServMsg.CatMsgBody((u8 *)&wConfInfoLen, sizeof(u16));
        cServMsg.CatMsgBody((u8 *)&m_tConf, sizeof(TConfInfo));

		SendMsgToEqp(tEqp.GetEqpId(), MCU_REC_STARTREC_REQ, cServMsg);
		break;
        }
	default:
		ConfLog(FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n",
			    pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
		break;
	}

}

/*====================================================================
    ������      ��ProcMcsMcuPauseRecReq
    ����        ����ͣ����¼������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/14    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuPauseRecReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = *( TMt* )cServMsg.GetMsgBody();
	TEqp		tRecorder;
	u8			byRecChannl;
	BOOL32		bRecConf = tMt.GetMcuId() == 0;


	switch( CurState() )
	{
	case STATE_ONGOING:
	
		if( bRecConf )
		{	
			EqpLog( "CMcuVcInst: Received request to record conference now!\n" );			
			
			if( !m_tConf.m_tStatus.IsRecording() )
			{
				EqpLog( "CMcuVcInst: This conference is not recording now!\n" );
				cServMsg.SetErrorCode( ERR_MCU_CONFNOTRECORDING );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
			tRecorder = m_tRecEqp;
			byRecChannl = m_byRecChnnl;
		}
		else
		{
			EqpLog( "CMcuVcInst: Receive request to record mt%d-%d.\n", tMt.GetMcuId(), tMt.GetMtId() );
			
			if( !m_ptMtTable->IsMtRecording( tMt.GetMtId() ) )
			{
				EqpLog( "CMcuVcInst: The Mt %d is not recording !\n", tMt.GetMtId() );
				cServMsg.SetErrorCode( ERR_MCU_MTNOTRECORDING );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;				
			}	
			m_ptMtTable->GetMtRecordInfo( tMt.GetMtId(), &tRecorder, &byRecChannl );

			if( 0 == tRecorder.GetEqpId() )
			{
				ConfLog( FALSE, "CMcuVcInst: The Mt %d is use record with Eqp id = 0, check program.\n",  tMt.GetMtId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}		
		}

		//send it to recorder
		cServMsg.SetChnIndex( byRecChannl );
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof( tMt ) );
		cServMsg.CatMsgBody( (u8*)&tRecorder, sizeof( tRecorder ) );
		SendMsgToEqp( tRecorder.GetEqpId(), MCU_REC_PAUSEREC_REQ, cServMsg );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuResumeRecReq
    ����        ���ָ�����¼������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/14    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuResumeRecReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = *( TMt* )cServMsg.GetMsgBody();	
	TEqp		tRecorder;
	u8			byRecChannl;
	BOOL32		bRecConf = tMt.GetMcuId() == 0;


	switch( CurState() )
	{
	case STATE_ONGOING:
	
		if( bRecConf )
		{
			EqpLog( "CMcuVcInst: Received request to resume recording conference now!\n" );
			
			if( !m_tConf.m_tStatus.IsRecPause() )
			{
				EqpLog( "CMcuVcInst: This conference is not recording now!\n" );
				cServMsg.SetErrorCode( ERR_MCU_CONFNOTRECPAUSE );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
			tRecorder   = m_tRecEqp;
			byRecChannl = m_byRecChnnl;
		}
		else
		{
			EqpLog( "CMcuVcInst: Receive request to resume recording mt%d-%d.\n", tMt.GetMcuId(), tMt.GetMtId() );
			
			if( !m_ptMtTable->IsMtRecPause( tMt.GetMtId() ) )
			{
				EqpLog( "CMcuVcInst: The Mt %d is not recording pause !\n", tMt.GetMtId() );
				cServMsg.SetErrorCode( ERR_MCU_MTNOTRECPAUSE );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;				
			}	
			m_ptMtTable->GetMtRecordInfo( tMt.GetMtId(), &tRecorder, &byRecChannl );

			if( 0 == tRecorder.GetEqpId() )
			{
				ConfLog( FALSE, "CMcuVcInst: The Mt %d is use record with Eqp id = 0, check program.\n",  tMt.GetMtId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}		
		}

		//send it to recorder
		cServMsg.SetChnIndex( byRecChannl );
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof( tMt ) );
		cServMsg.CatMsgBody( (u8*)&tRecorder, sizeof( tRecorder ) );
		SendMsgToEqp( tRecorder.GetEqpId(), MCU_REC_RESUMEREC_REQ, cServMsg );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuStopRecReq
    ����        ��ֹͣ����¼������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/16    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuStopRecReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = *( TMt* )cServMsg.GetMsgBody();
	TEqp		tRecorder;
	u8			byRecChannl;
	BOOL32		bRecConf = tMt.GetMcuId() == 0;


	switch( CurState() )
	{
	case STATE_ONGOING:
	
		if( bRecConf )
		{
			EqpLog( "CMcuVcInst: Received request to stop recording conference now!\n" );
			
			if( m_tConf.m_tStatus.IsNoRecording() )
			{
				EqpLog( "CMcuVcInst: This conference is not in recording state!\n" );
				cServMsg.SetErrorCode( ERR_MCU_CONFNOTRECORDING );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
			tRecorder = m_tRecEqp;
			byRecChannl = m_byRecChnnl;
		}
		else
		{
			EqpLog( "CMcuVcInst: Receive request to stop recording mt%d-%d.\n", tMt.GetMcuId(), tMt.GetMtId() );
			
			if( m_ptMtTable->IsMtNoRecording( tMt.GetMtId() ) )
			{
				EqpLog( "CMcuVcInst: The Mt %d is not recording  !\n", tMt.GetMtId() );
				cServMsg.SetErrorCode( ERR_MCU_MTNOTRECORDING );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;				
			}	
			m_ptMtTable->GetMtRecordInfo( tMt.GetMtId(), &tRecorder, &byRecChannl );

			if( 0 == tRecorder.GetEqpId() )
			{
				ConfLog( FALSE, "CMcuVcInst: The Mt %d is use record with Eqp id = 0, check program.\n",  tMt.GetMtId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}		
		}

		//send it to recorder
		cServMsg.SetChnIndex( byRecChannl );
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof( tMt ) );
		cServMsg.CatMsgBody( (u8*)&tRecorder, sizeof( tRecorder ) );
		SendMsgToEqp( tRecorder.GetEqpId(), MCU_REC_STOPREC_REQ, cServMsg );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

}

/*====================================================================
    ������      ��ProcVcctrlMcuChangeRecModeReq
    ����        ���ı����¼��ģʽ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/28    1.0         JQL         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuChangeRecModeReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = *( TMt* )cServMsg.GetMsgBody();
	u8			byMode = *( u8* )( cServMsg.GetMsgBody() + sizeof( TMt ) );
	
	TEqp		tRecEqp;
	u8			byRecChannel;
	BOOL32      bRecordConf = tMt.GetMcuId() == 0;
				

	switch( CurState() )
	{
	case STATE_ONGOING:
		if( bRecordConf )
		{
			EqpLog( "CMcuVcInst: Received request to change recording mode conference now! Channel is %u\n", 
				m_byRecChnnl );

			//not recording now, Nack
			if( m_tConf.m_tStatus.IsNoRecording() )
			{
				EqpLog( "CMcuVcInst: This conference is not recording now!\n" );
				cServMsg.SetErrorCode( ERR_MCU_CONFNOTRECORDING );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
			tRecEqp = m_tRecEqp;
			byRecChannel = m_byRecChnnl;

		}
		else
		{
			EqpLog( "CMcuVcInst: Received request to change recording mode Mcu%d-Mt%d now!\n", tMt.GetMcuId(), tMt.GetMtId() );

			//not recording now, Nack
			if( m_ptMtTable->IsMtNoRecording( tMt.GetMtId() ) )
			{
				EqpLog( "CMcuVcInst: The Mt %d is not in recording state!\n", tMt.GetMtId() );
				cServMsg.SetErrorCode( ERR_MCU_MTNOTRECORDING );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}

			m_ptMtTable->GetMtRecordInfo( tMt.GetMtId(), &tRecEqp, &byRecChannel );
			if( 0 == tRecEqp.GetEqpId() )
			{
				ConfLog( FALSE, "CMcuVcInst: The Mt %d is use record with Eqp id = 0, check program.\n",  tMt.GetMtId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}		
			
		}

		cServMsg.SetChnIndex( byRecChannel );
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof( tMt ) );
		cServMsg.CatMsgBody( (u8*)&tRecEqp, sizeof( tRecEqp ) );
		cServMsg.CatMsgBody( (u8*)&byMode, sizeof( byMode ) );
		SendMsgToEqp( tRecEqp.GetEqpId(), MCU_REC_CHANGERECMODE_REQ, cServMsg );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

}

/*====================================================================
    ������      ��ProcMcsMcuListAllRecordReq
    ����        ����ѯ¼�����¼������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/12    3.0         ������         ����
    06/08/22    4.0         ����         ��������
====================================================================*/
void CMcuVcInst::ProcMcsMcuListAllRecordReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TEqp		tEqp = *( TEqp* )cServMsg.GetMsgBody();
		
	//�ж������Ƿ�Ϸ���������
	if( !g_cMcuVcApp.IsPeriEqpValid( tEqp.GetEqpId() ) || 
		!g_cMcuVcApp.IsPeriEqpConnected( tEqp.GetEqpId() ) )
	{
		EqpLog( "CMcuVcInst: This MCU's recorder is invalid or offline now!\n" );
		cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//���͸�¼���
    cServMsg.SetMsgBody( ( u8 * )&tEqp, sizeof( tEqp ) );

    // ����ID�����¼���
    u8 byUsrGrpId = CMcsSsn::GetUserGroup( cServMsg.GetSrcSsnId() );
    cServMsg.CatMsgBody( (u8*)&byUsrGrpId, sizeof(u8) );
    /*
    // ���� [7/20/2006] ֧������Ŀ¼
    CConfId cConfId = cServMsg.GetConfId();
    u8 byConfMsg = 0;
    u8 byUsrGrpId = CMcsSsn::GetUserGroup( cServMsg.GetSrcSsnId() );

    if (!cConfId.IsNull())
    {
        // ��������
        byConfMsg = 1;
        cServMsg.CatMsgBody( (u8*)&byConfMsg, sizeof(u8) );
        // ����ID�����¼���
        cServMsg.CatMsgBody( (u8*)&byUsrGrpId, sizeof(u8) );

        // ���CConfId ��Null�����ϸû�������ָ�¼���
        LPCSTR lpszName = NULL;
        if ( g_cMcuVcApp.GetConfNameByConfId(cConfId, lpszName) != 0)
        {
            s8 szConfName[MAXLEN_CONFNAME] = {0};
            strncpy( szConfName, lpszName, MAXLEN_CONFNAME-1 );
            cServMsg.CatMsgBody( (u8*)szConfName, MAXLEN_CONFNAME);
        }
        else
        {
            ConfLog(FALSE, "[ProcMcsMcuListAllRecordReq] Cannot find conf name for confid: ");
            cConfId.Print();
        }
        
    }
    else
    {
        // guzh [8/22/2006] �ǻ�������
        byConfMsg = 0;
        cServMsg.CatMsgBody( (u8*)&byConfMsg, sizeof(u8) );
        
        // ����ID�����¼���
        cServMsg.CatMsgBody( (u8*)&byUsrGrpId, sizeof(u8) );

        if (byUsrGrpId == USRGRPID_SADMIN)
        {
            // ����ǳ�������Ա������Ҫ�����κλ�����
        }
        else
        {
            // �������������û�������л������ģ��(����Ϊ��)
            s8 aszConfNameBuf[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE][MAXLEN_CONFNAME] = {0};
            u8 byConfNum = 0;
    
            TConfMapData tMapData;
            CMcuVcInst* pcInst = NULL;
            TTemplateInfo tTemInfo;
            u8 byIndex;
            for ( byIndex = MIN_CONFIDX; byIndex <= MAX_CONFIDX; byIndex++)
            {
                tMapData = g_cMcuVcApp.GetConfMapData( byIndex );
                if (tMapData.IsValidConf())
                {
                    pcInst = g_cMcuVcApp.GetConfInstHandle(byIndex);
                    if (pcInst == NULL)
                    {
                        continue;
                    }
                    if (pcInst->m_tConf.GetUsrGrpId() == byUsrGrpId ||
                        pcInst->m_tConf.GetUsrGrpId() == USRGRPID_INVALID)  // �ն˴��ᶼ�ܿ���
                    {
                        strncpy( aszConfNameBuf[byConfNum], pcInst->m_tConf.GetConfName(), MAXLEN_CONFNAME-1 );
                        byConfNum ++;
                    }
                }
                else if (tMapData.IsTemUsed())
                {
                    g_cMcuVcApp.GetTemplate(byIndex, tTemInfo);
                    if (tTemInfo.IsEmpty())
                    {
                        continue;
                    }
                    if (tTemInfo.m_tConfInfo.GetUsrGrpId() == byUsrGrpId ||
                        tTemInfo.m_tConfInfo.GetUsrGrpId() == USRGRPID_INVALID)
                    {
                        // ����Ƿ��Ѿ���ͬ�����飬���ޣ������
                        LPCSTR lpszName = tTemInfo.m_tConfInfo.GetConfName();
                        CConfId cConfId = g_cMcuVcApp.GetConfIdByName( lpszName, FALSE );
                        if (cConfId.IsNull())
                        {
                            strncpy( aszConfNameBuf[byConfNum], lpszName, MAXLEN_CONFNAME-1 );
                            byConfNum ++;
                        }
                    }
                }
            }
            for ( byIndex = 0; byIndex < byConfNum; byIndex ++ )
            {
                cServMsg.CatMsgBody( (u8*)aszConfNameBuf[byIndex], MAXLEN_CONFNAME);    
            }
        }
    }
    */
	
	SendMsgToEqp( tEqp.GetEqpId(), MCU_REC_LISTALLRECORD_REQ, cServMsg );

}

/*====================================================================
    ������      ��ProcRecMcuListAllRecNotif
    ����        ��¼���¼֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/12    3.0         ������         ����
    06/07/24    4.0         ����         R3�༶Ŀ¼֧��
====================================================================*/
void CMcuVcInst::ProcRecMcuListAllRecNotif( const CMessage * pcMsg )
{
    // guzh [8/22/2006] �ı���ԣ�ҵ��಻�ڴ˽��й��ˣ�������¼�������ʱ

    CServMsg	cServMsg( pcMsg->content, pcMsg->length );
    cServMsg.SetNullConfId();    // ��ConfId��գ���ҵ��ͨ��
    SendMsgToMcs(  cServMsg.GetSrcSsnId(), MCU_MCS_LISTALLRECORD_NOTIF, cServMsg );

/*
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
    CServMsg    newServMsg( pcMsg->content, pcMsg->length );
    
    // ����û�ָ����CConfId
    if ( !cServMsg.GetConfId().IsNull() )
    {
        // ֱ�ӷ���
    }
    else
    {
        //��Ҫ�����û����������Ϣ����������Ϣ�������Ļ���¼���Ƿ�Ҫ֪ͨMCS
        s8 szConfName [MAXLEN_CONFNAME] = {0};

        strncpy(szConfName, 
                (s8*)(cServMsg.GetMsgBody() + sizeof(TEqp)),
                MAXLEN_CONFNAME);
        
        if (strlen(szConfName) > 0)
        {
            // �ֱ�������ģ������
            CConfId cConfId = g_cMcuVcApp.GetConfIdByName(szConfName, FALSE);
            CConfId cTemplId = g_cMcuVcApp.GetConfIdByName(szConfName, TRUE);

            if (cConfId.IsNull() && cTemplId.IsNull())
            {
                // ����Ѿ�û�иû����ģ�棬��ֻ��admin�ܿ���
                cConfId = g_cMcuVcApp.MakeConfId( 0, 0, USRGRPID_SADMIN );
                newServMsg.SetConfId( cConfId );
            }
            else if (!cConfId.IsNull())
            {
                newServMsg.SetConfId( cConfId );
            }
            else if (!cTemplId.IsNull())
            {
                newServMsg.SetConfId( cTemplId );
            }
        }

    }
  
    newServMsg.SetMsgBody( cServMsg.GetMsgBody(), sizeof(TEqp) );
    newServMsg.CatMsgBody( cServMsg.GetMsgBody() + sizeof(TEqp) + MAXLEN_CONFNAME, 
                           sizeof(TRecFileListNotify ) );

	SendMsgToMcs(  newServMsg.GetSrcSsnId(), MCU_MCS_LISTALLRECORD_NOTIF, newServMsg );
*/
}

/*====================================================================
    ������      ��ProcMcsMcuDeleteRecordReq
    ����        �������ص�MCU��ɾ���ļ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/09/22    4.0         libo          ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuDeleteRecordReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TEqp		tEqp = *( TEqp* )cServMsg.GetMsgBody();
		
	//�ж������Ƿ�Ϸ���������
	if( !g_cMcuVcApp.IsPeriEqpValid( tEqp.GetEqpId() ) || 
		!g_cMcuVcApp.IsPeriEqpConnected( tEqp.GetEqpId() ) )
	{
		EqpLog( "CMcuVcInst: This MCU's recorder is invalid or offline now!\n" );
		cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//���͸�¼���
	SendMsgToEqp( tEqp.GetEqpId(), MCU_REC_DELETERECORD_REQ, cServMsg );

}

/*====================================================================
    ������      ��ProcMcsMcuRenameRecordReq
    ����        �������ص�MCU�ĸ����ļ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/09/22    4.0         libo          ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuRenameRecordReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp     tEqp = *(TEqp *)cServMsg.GetMsgBody();

    //�ж������Ƿ�Ϸ���������
    if (!g_cMcuVcApp.IsPeriEqpValid(tEqp.GetEqpId()) || 
        !g_cMcuVcApp.IsPeriEqpConnected(tEqp.GetEqpId()))
    {
        EqpLog("CMcuVcInst: This MCU's recorder is invalid or offline now!\n");
        cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
        return;
    }

    //���͸�¼���
    SendMsgToEqp(tEqp.GetEqpId(), MCU_REC_RENAMERECORD_REQ, cServMsg);
}

/*====================================================================
    ������      ��ProcVcctrlMcuPublishRecReq
    ����        �������ص�MCU��¼�񷢲�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/04    1.0         JQL         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuPublishRecReq( const CMessage * pcMsg )
{

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TEqp	*	ptEqp      = ( TEqp* )cServMsg.GetMsgBody();
		
	//�ж������Ƿ�Ϸ���������
	if( !g_cMcuVcApp.IsPeriEqpValid( ptEqp->GetEqpId() ) || 
		!g_cMcuVcApp.IsPeriEqpConnected( ptEqp->GetEqpId() ) )
	{
		EqpLog( "CMcuVcInst: This MCU's recorder is invalid or offline now!\n" );
		cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	if( !g_cMcuVcApp.IsPeriEqpConnected( ptEqp->GetEqpId() ) )
	{
		ConfLog( FALSE, "CMcuVcInst: Eqp %d-%d doesnot connect yet.\n" );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	SendMsgToEqp( ptEqp->GetEqpId(), MCU_REC_PUBLISHREC_REQ, cServMsg );

}

void CMcuVcInst::ProcMcsMcuCancelPublishRecReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TEqp		tEqp = *( TEqp* )cServMsg.GetMsgBody();
		
	//�ж������Ƿ�Ϸ���������
	if( !g_cMcuVcApp.IsPeriEqpValid( tEqp.GetEqpId() ) || 
		!g_cMcuVcApp.IsPeriEqpConnected( tEqp.GetEqpId() ) )
	{
		EqpLog( "CMcuVcInst: This MCU's recorder is invalid or offline now!\n" );
		cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//���͸�¼���
	SendMsgToEqp( tEqp.GetEqpId(), MCU_REC_CANCELPUBLISHREC_REQ, cServMsg );
	
}

/*====================================================================
    ������      ��ProcMcsMcuStartPlayReq
    ����        ����ʼ�����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/09/28    4.0         libo          ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuStartPlayReq(const CMessage * pcMsg)
{
    CServMsg cTempServMsg(pcMsg->content, pcMsg->length);
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TRecProg tRecProg;

    memset(&tRecProg, 0, sizeof(TRecProg));

    u8 byIsDStreamPlay = *(u8 *)(cTempServMsg.GetMsgBody() + sizeof(TEqp));

    m_tPlayEqpAttrib.SetDStreamPlay(byIsDStreamPlay);

    cServMsg.SetMsgBody((u8 *)cTempServMsg.GetMsgBody(), sizeof(TEqp));
    cServMsg.CatMsgBody((u8 *)&tRecProg, sizeof(TRecProg));                             //��ʼ����

    s8 szRecName[KDV_MAX_PATH] = {0};
    //�����ļ���
    strncpy(szRecName,
            (s8 *)(cTempServMsg.GetMsgBody() + sizeof(u8) + sizeof(TEqp)),
            KDV_MAX_PATH-1);    
    szRecName[KDV_MAX_PATH-1] = 0;
    
    s8 szRecFullName[KDV_MAX_PATH] = {0};
    // ƴ��·����¼���
    // guzh [8/29/2006] mcs �Լ���·��
    //sprintf(szRecFullName, "%s/%s", m_tConf.GetConfName(), szRecName);
    sprintf(szRecFullName, "%s",  szRecName);

    cServMsg.CatMsgBody( (u8*)szRecFullName, strlen(szRecFullName)+1 );


    StartPlayReq(cServMsg);
}

/*====================================================================
    ������      ��StartPlayReq
    ����        ����ʼ�����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CServMsg &cServMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/09/28    4.0         libo          ����
====================================================================*/
void CMcuVcInst::StartPlayReq(CServMsg &cServMsg)
{
    CServMsg cTempMsg;
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();

    TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
    u8 byConfVideoType = m_tConf.GetMainVideoMediaType();
    u8 byConfAudioType = m_tConf.GetMainAudioMediaType();

	// ˫����ʽ
	u8 byDStreamMediaType = m_tConf.GetCapSupport().GetDStreamMediaType();

    TDoublePayload tDVPayload, tSndDVPayload;
    TDoublePayload tDAPayload, tSndDAPayload;

    tSndDVPayload.SetRealPayLoad(m_tConf.GetSecVideoMediaType());
    tSndDAPayload.SetRealPayLoad(m_tConf.GetSecAudioMediaType());

	TCapSupportEx tCapSupportEx;
	u16 wFileInfoLen = 0;
	
    TCapSupport tCapSupport;
    switch (CurState())
    {
    case STATE_ONGOING:
        //ǿ�Ʒ�����ʱ���������
        /*        
        if (m_tConf.m_tStatus.IsMustSeeSpeaker())
        {
            ConfLog(FALSE, "CMcuVcInst: Must See Speaker, can't play recorder!\n" );
            cServMsg.SetErrorCode(ERR_MUSTSEESPEAKER_NOTPLAY);
            cServMsg.SetMsgBody((u8*)&tEqp, sizeof(TEqp));
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }*/

        //�ж������Ƿ�Ϸ���������
        if (!g_cMcuVcApp.IsPeriEqpValid(tEqp.GetEqpId()) || 
            !g_cMcuVcApp.IsPeriEqpConnected(tEqp.GetEqpId()))
        {
            ConfLog(FALSE, "CMcuVcInst: This MCU's recorder is invalid or offline now!\n");
            cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            cServMsg.SetMsgBody((u8*)&tEqp, sizeof(TEqp));
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        //����¼��,NACK
        if (!m_tConf.m_tStatus.IsNoPlaying())
        {
            ConfLog(FALSE, "CMcuVcInst: This conference is playing now!\n");
            cServMsg.SetErrorCode( ERR_MCU_CONFPLAYING);
            cServMsg.SetMsgBody((u8*)&tEqp, sizeof(TEqp));
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        //����¼���
        cTempMsg.SetMsgBody(cServMsg.GetMsgBody() + sizeof(TEqp),
                            cServMsg.GetMsgBodyLen() - sizeof(TEqp));
        tEqp.SetMcuEqp( (u8)LOCAL_MCUID, tEqp.GetEqpId(), EQP_TYPE_RECORDER);

        if (MEDIA_TYPE_H264 == byConfVideoType || 
            MEDIA_TYPE_H263PLUS == byConfVideoType || 
            CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
            // zbq [10/29/2007] ��ӦFEC֧��������ʽ
            m_tConf.GetCapSupportEx().IsVideoSupportFEC())
        {
            tDVPayload.SetRealPayLoad(byConfVideoType);
            tDVPayload.SetActivePayload(GetActivePayload(m_tConf, byConfVideoType));
        }
        else
        {
            tDVPayload.SetRealPayLoad(byConfVideoType);
            tDVPayload.SetActivePayload(byConfVideoType);
        }

        if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
            // zbq [10/29/2007] ��ӦFEC֧��������ʽ
            m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
        {
            tDAPayload.SetRealPayLoad(byConfAudioType);
            tDAPayload.SetActivePayload(GetActivePayload(m_tConf, byConfAudioType));
        }
        else
        {
            tDAPayload.SetRealPayLoad(byConfAudioType);
            tDAPayload.SetActivePayload(byConfAudioType);
        }

        //�ļ���ʽ����������ʽ������ͬ����rplib������и�ʽ�б𣬲�������ܾ�!!!
        cServMsg.SetMsgBody((u8 *)&tEqp, sizeof(tEqp));
        cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //video
        cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload)); //video
        cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //audio
        cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload)); //audio
        cServMsg.CatMsgBody((u8 *)&tSndDVPayload, sizeof(tSndDVPayload));
        cServMsg.CatMsgBody((u8 *)&tSndDAPayload, sizeof(tSndDAPayload));
		cServMsg.CatMsgBody(&byDStreamMediaType, sizeof(byDStreamMediaType)); //DStream, zgc, 2007-08-23

        // MCUǰ�����, zgc, 2007-09-28
        tCapSupportEx = m_tConf.GetCapSupportEx();
        cServMsg.CatMsgBody((u8*)&tCapSupportEx, sizeof(tCapSupportEx));
		
		wFileInfoLen = cTempMsg.GetMsgBodyLen();
		cServMsg.CatMsgBody((u8*)&wFileInfoLen, sizeof(u16));
        cServMsg.CatMsgBody(cTempMsg.GetMsgBody(), cTempMsg.GetMsgBodyLen()); //��ʼ���ȣ������ļ���
        tCapSupport = m_tConf.GetCapSupport();
		cServMsg.CatMsgBody((u8*)&tCapSupport, sizeof(TCapSupport));//���Ӱ�֪ͨ����������

        SendMsgToEqp(tEqp.GetEqpId(), MCU_REC_STARTPLAY_REQ, cServMsg);
        break;

    default:
        //ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
        //	pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        break;
    }
}

/*====================================================================
    ������      ��ProcMcsMcuPausePlayReq
    ����        ����ͣ�����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/14    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuPausePlayReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:
		EqpLog( "CMcuVcInst: Received request to pause playing conference now! Channel is %u\n", 
			m_byPlayChnnl );

		//not playing now, Nack
		if( !m_tConf.m_tStatus.IsPlaying() )
		{
			EqpLog( "CMcuVcInst: This conference is not playing now!\n" );
			cServMsg.SetErrorCode( ERR_MCU_CONFNOTPLAYING );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}

		//send it to recorder
		cServMsg.SetMsgBody( ( u8 * )&m_tPlayEqp, sizeof( m_tPlayEqp ) );	//set TEqp
		cServMsg.SetChnIndex( m_byPlayChnnl );
		SendMsgToEqp( m_tPlayEqp.GetEqpId(), MCU_REC_PAUSEPLAY_REQ, cServMsg );


		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuResumePlayReq
    ����        ���ָ������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/14    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuResumePlayReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:
		EqpLog( "CMcuVcInst: Received request to resume recording conference now! Channel is %u\n", 
			m_byPlayChnnl );

		//not pausing now, Nack
		if( !m_tConf.m_tStatus.IsPlayPause() )
		{
			EqpLog( "CMcuVcInst: This conference is not play paused now!\n" );
			cServMsg.SetErrorCode( ERR_MCU_CONFNOTPLAYPAUSE );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}

		//send it to recorder
		cServMsg.SetMsgBody( ( u8 * )&m_tPlayEqp, sizeof( m_tPlayEqp ) );	//set TEqp
		cServMsg.SetChnIndex( m_byPlayChnnl );
		SendMsgToEqp( m_tPlayEqp.GetEqpId(), MCU_REC_RESUMEPLAY_REQ, cServMsg );


		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}



/*====================================================================
    ������      ��ProcMcsMcuStopPlayReq
    ����        ��ֹͣ�����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/16    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuStopPlayReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:
		EqpLog( "CMcuVcInst: Received request to stop playing conference now! Channel is %u\n", 
			m_byPlayChnnl );

		//not playing now, Nack
		if( m_tConf.m_tStatus.IsNoPlaying() )
		{
			EqpLog( "CMcuVcInst: This conference is not playing now!\n" );
			cServMsg.SetErrorCode( ERR_MCU_CONFNOTPLAYING );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}

		//send it to recorder
		cServMsg.SetMsgBody( ( u8 * )&m_tPlayEqp, sizeof( m_tPlayEqp ) );	//set TEqp
		cServMsg.SetChnIndex( m_byPlayChnnl );
		SendMsgToEqp( m_tPlayEqp.GetEqpId(), MCU_REC_STOPPLAY_REQ, cServMsg );

		//����״̬�ı�͸ı䷢�����յ��ŵ��ı�֪ͨ���ٴ�������
        // ������ļ���ʽ��¼, zgc, 2008-08-28
        m_tPlayFileMediaInfo.clear();

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*=============================================================================
  �� �� ���� ProcMcsMcuGetRecPlayProgCmd
  ��    �ܣ� ��ز�ѯ¼�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::ProcMcsMcuGetRecPlayProgCmd( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    switch( CurState() )
    {
    case STATE_ONGOING:
        {
            //��ȡ¼�����
            if (MCS_MCU_GETRECPROG_CMD == pcMsg->event)
            {
                TMt tMt = *(TMt *)cServMsg.GetMsgBody();
                TEqp tRecEqp;
                u8  byRecChnnl;
                
                //����¼��
                if(tMt.IsNull())
                {
                    if (m_tConf.m_tStatus.IsNoRecording())
                    {
                        return;
                    }

                    tRecEqp = m_tRecEqp;
                    byRecChnnl = m_byRecChnnl;
                }
                //�ն�¼��
                else
                {
                    if (m_ptMtTable->IsMtNoRecording(tMt.GetMtId()))
                    {
                        return;
                    }

                    if(!m_ptMtTable->GetMtRecordInfo(tMt.GetMtId(), &tRecEqp, &byRecChnnl))
                    {
                        return;
                    }
                }               

                cServMsg.SetChnIndex(byRecChnnl);
                SendMsgToEqp(tRecEqp.GetEqpId(), MCU_REC_GETRECPROG_CMD, cServMsg);
            }
            //��ȡ�������
            else
            {
                if (m_tConf.m_tStatus.IsNoPlaying())
                {
                    return;
                }                
                cServMsg.SetChnIndex( m_byPlayChnnl );
                SendMsgToEqp( m_tPlayEqp.GetEqpId(), MCU_REC_GETPLAYPROG_CMD, cServMsg );
            }
        }
        break;

    default:
        ConfLog( FALSE, "[ProcMcsMcuGetRecPlayProgCmd] Wrong message %u(%s) received in state %u!\n", 
                pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        break;
    }

    return;
}

/*====================================================================
    ������      ��ProcMcsMcuSeekReq
    ����        ������������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/20    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuSeekReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length ), cTempMsg;
	u16		wEv;

	switch( CurState() )
	{
	case STATE_ONGOING:
		switch( pcMsg->event )
		{
		case MCS_MCU_FFPLAY_REQ:
			EqpLog( "CMcuVcInst: Received request to fast forward now! Channel is %u\n", 
				m_byPlayChnnl );
			wEv = MCU_REC_FFPLAY_REQ;
			break;
		case MCS_MCU_FBPLAY_REQ:
			EqpLog( "CMcuVcInst: Received request to fast backward now! Channel is %u\n", 
				m_byPlayChnnl );
			wEv = MCU_REC_FBPLAY_REQ;
			break;
		case MCS_MCU_SEEK_REQ:
			EqpLog( "CMcuVcInst: Received request to seek now! Channel is %u\n", 
				m_byPlayChnnl );
			wEv = MCU_REC_SEEK_REQ;
			break;
		default:
			break;
		}

		//not playing now, Nack
		if( m_tConf.m_tStatus.IsNoPlaying() )
		{
			EqpLog( "CMcuVcInst: This conference is not playing now!\n" );
			cServMsg.SetErrorCode( ERR_MCU_CONFNOTPLAYING );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}

		//send it to recorder
		cTempMsg.SetMsgBody( cServMsg.GetMsgBody() + sizeof( TEqp ), cServMsg.GetMsgBodyLen() - sizeof( TEqp ) );
		cServMsg.SetMsgBody( ( u8 * )&m_tPlayEqp, sizeof( m_tPlayEqp ) );	//set TEqp
		cServMsg.CatMsgBody( cTempMsg.GetMsgBody(), cTempMsg.GetMsgBodyLen() );	//set record name
		cServMsg.SetChnIndex( m_byPlayChnnl );
		SendMsgToEqp( m_tPlayEqp.GetEqpId(), wEv, cServMsg );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*=============================================================================
�� �� ���� ProcRecPlayWaitMpAckTimer
��    �ܣ� �ȴ����񽻻�������ʱ����ʱ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/3/26   4.0			�ܹ��                  ����
=============================================================================*/
void CMcuVcInst::ProcRecPlayWaitMpAckTimer(const CMessage * pcMsg)
{
	KillTimer( MCUVC_RECPLAY_WAITMPACK_TIMER );
	CServMsg cServMsg;
	cServMsg.SetChnIndex( m_byPlayChnnl );
	m_tConf.m_tStatus.SetPlaying();
	SendMsgToEqp( m_tPlayEqp.GetEqpId() , MCU_EQP_SWITCHSTART_NOTIF, cServMsg);
}

/*====================================================================
    ������      ��DaemonProcRecMcuChnnlStatusNotif
    ����        ��¼����ŵ�״̬֪ͨ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/18    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::DaemonProcRecMcuChnnlStatusNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp *ptEqp;
    TRecChnnlStatus *ptChnnlStatus;
    CConfId cConfId = cServMsg.GetConfId();
    u8 byChnnl = cServMsg.GetChnIndex();
    TPeriEqpStatus tRecStatus;

    if (pcMsg->event == REC_MCU_RECORDCHNSTATUS_NOTIF)
    {
        ptEqp = (TEqp *)(cServMsg.GetMsgBody() + sizeof(TMt));
        ptChnnlStatus = (TRecChnnlStatus *)(cServMsg.GetMsgBody() + sizeof(TEqp) + sizeof(TMt));	
    }
    else
    {
        ptEqp = (TEqp *)cServMsg.GetMsgBody();
        ptChnnlStatus = (TRecChnnlStatus *)(cServMsg.GetMsgBody() + sizeof(TEqp));
    }

    //save to periequipment status
    if (!g_cMcuVcApp.GetPeriEqpStatus(ptEqp->GetEqpId(), &tRecStatus))
    {
        return;
    }
    tRecStatus.m_tStatus.tRecorder.SetChnnlStatus(byChnnl, ptChnnlStatus->m_byType, ptChnnlStatus);
    g_cMcuVcApp.SetPeriEqpStatus(ptEqp->GetEqpId(), &tRecStatus);

    //analyze conference id and forward it to conference
    if (0 != g_cMcuVcApp.GetConfIdx(cConfId))
    {
        g_cMcuVcApp.SendMsgToConf(cConfId, pcMsg->event, pcMsg->content, pcMsg->length);
    }
}

/*====================================================================
    ������      ��DaemonProcRecMcuProgNotif
    ����        ��¼����ŵ�����֪ͨ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/20    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::DaemonProcRecMcuProgNotif( const CMessage * pcMsg )
{
    CServMsg	cServMsg( pcMsg->content, pcMsg->length);
    u8          * pbyMsgBody  =  cServMsg.GetMsgBody();   
    TMt         * ptMt  = NULL;
    TEqp		* ptEqp = NULL;
    TRecProg	* ptRecProg = NULL;
    TRecChnnlStatus		tChnnlStatus;
    CConfId		cConfId = cServMsg.GetConfId();
    u8			byChnnl = cServMsg.GetChnIndex(), byChnnlType;
    TPeriEqpStatus	tRecStatus;

    //save to periequipment status
    if (pcMsg->event == REC_MCU_PLAYPROG_NOTIF)
    {
        byChnnlType = TRecChnnlStatus::TYPE_PLAY;
    }
    else
    {
        byChnnlType = TRecChnnlStatus::TYPE_RECORD;

        ptMt = (TMt*)pbyMsgBody;
        pbyMsgBody  += sizeof(TMt);
    }

    ptEqp       = (TEqp*)pbyMsgBody;
    pbyMsgBody += sizeof(TEqp);

    ptRecProg   = (TRecProg *)pbyMsgBody;

    if (!g_cMcuVcApp.GetPeriEqpStatus(ptEqp->GetEqpId(), &tRecStatus))
    {
        return;
    }

    if (!tRecStatus.m_tStatus.tRecorder.GetChnnlStatus(byChnnl, byChnnlType, &tChnnlStatus))
    {
        return;
    }
    tChnnlStatus.m_tProg = *ptRecProg;
    tRecStatus.m_tStatus.tRecorder.SetChnnlStatus(byChnnl, byChnnlType, &tChnnlStatus);
    g_cMcuVcApp.SetPeriEqpStatus(ptEqp->GetEqpId(), &tRecStatus);

	//analyze conference id and forward it to conference
    if (0 != g_cMcuVcApp.GetConfIdx(cConfId))
    {
        g_cMcuVcApp.SendMsgToConf(cConfId, pcMsg->event, pcMsg->content, pcMsg->length);
    }
}


/*====================================================================
    ������      ��ProcRecMcuPlayChnnlStatusNotif
    ����        ��¼��������ŵ��ı䴦����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/19    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcRecMcuPlayChnnlStatusNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TEqp		*ptEqp = ( TEqp * )cServMsg.GetMsgBody();
	TRecChnnlStatus		* ptChnnlStatus = ( TRecChnnlStatus * )( cServMsg.GetMsgBody() + sizeof( TEqp ) );
	CConfId		cConfId = cServMsg.GetConfId();
	u8			byChnnl = cServMsg.GetChnIndex();

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		switch( ptChnnlStatus->m_byState )
		{
		case TRecChnnlStatus::STATE_PLAYREADY:	//ֹͣ����
            //change speaker if it's recorder
            if( m_tConf.GetSpeaker() == *ptEqp )
            {
                //�Ƿ�Ϊ�����������Ʒ��ԣ�����ָ�
                if (m_tConf.m_tStatus.IsVACing() && !m_tVacLastSpeaker.IsNull())
                {
                    EqpLog("Vac speaker restored, speaker is: Mt%d\n",
                                   m_tVacLastSpeaker.GetMtId());

                    ChangeSpeaker(&m_tVacLastSpeaker);
                }
                else
                {
                    ChangeSpeaker(NULL);
                }

				// xliang [8/19/2008] modify A:�˲����߼��Ƴ���if�⣬��"modify B"����
				//���ڴ˴�����������bug����ͨ���Ϸ����˶�����ͣ������龰�£�ǰ�����if�ǽ�������
				//��ͻᵼ�������˫������Ļ�����ͣ���Ϸ����˻�ͣ����˫��
// 				 if (TYPE_MCUPERI == m_tDoubleStreamSrc.GetType())
// 				 {
// 					 StopDoubleStream(FALSE, TRUE);
// 				 }
            }

			// xliang [8/19/2008] modify B �Ľ��жϣ������ϸ�
			//if (TYPE_MCUPERI == m_tDoubleStreamSrc.GetType())
			if(m_tDoubleStreamSrc == *ptEqp) 
			{
				StopDoubleStream(FALSE, TRUE);
			}
			//change status
			m_byPlayChnnl = 0;
			memset( &m_tPlayEqp, 0, sizeof( m_tPlayEqp ) );
            m_tPlayEqpAttrib.Reset();
			m_tConf.m_tStatus.SetNoPlaying();

			break;
		case TRecChnnlStatus::STATE_PLAYING:		//��ʼ�����ָ�����
		
			//change speaker if it's not speaker now			
//			if( m_tConf.m_tStatus.IsNoPlaying() )
//			{
//				m_byPlayChnnl = byChnnl;
//				m_tPlayEqp = *ptEqp;
//				m_tPlayEqp.SetConfIdx( m_byConfIdx );
//				ChangeSpeaker( &m_tPlayEqp );
//			}
			m_tConf.m_tStatus.SetPlaying();

			break;
		// zgc, 2007-02-27, �������ؼ�֡�ӳ�����
		case TRecChnnlStatus::STATE_PLAYREADYPLAY:	//׼����ʼ����
//			m_tConf.m_tStatus.SetPlayReady();
			break;
		// zgc, 2007-02-27, end
			
		case TRecChnnlStatus::STATE_PLAYPAUSE:		//��ͣ����
			//change status
			m_tConf.m_tStatus.SetPlayPause();
			break;
		case TRecChnnlStatus::STATE_FF:				//���
			//change status
			m_tConf.m_tStatus.SetPlayFF();
			break;
		case TRecChnnlStatus::STATE_FB:				//����
			//change status
			m_tConf.m_tStatus.SetPlayFB();
			break;
		default:
			ConfLog( FALSE, "CMcuVcInst: Wrong channel state %u!\n", ptChnnlStatus->m_byState );
			break;
		}

		ConfModeChange();
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcRecMcuRecChnnlStatusNotif
    ����        ��¼���¼���ŵ��ı䴦����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/19    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcRecMcuRecChnnlStatusNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TMt      tRecMt = *(TMt *)cServMsg.GetMsgBody();
    TEqp    *ptEqp  = (TEqp *)(cServMsg.GetMsgBody() + sizeof(TMt));
    TRecChnnlStatus *ptChnnlStatus = (TRecChnnlStatus *)(ptEqp + 1);
    CConfId  cConfId = cServMsg.GetConfId();
    u8       byChnnl = cServMsg.GetChnIndex();
    u8       bySrcChnnl = 0;
    BOOL32   bRecordConf = (tRecMt.GetMcuId() == 0);

	switch (CurState())
	{
	case STATE_ONGOING:
		
		if ( bRecordConf )
		{			
			switch( ptChnnlStatus->m_byState )
			{
			case TRecChnnlStatus::STATE_RECREADY:	//ֹͣ¼��
            {            
				//stop switch	
				StopSwitchToPeriEqp( m_tRecEqp.GetEqpId(), byChnnl, FALSE );
                // guzh [8/30/2006] ͣ˫��¼��
                if ( m_tRecPara.IsRecDStream() && !m_tDoubleStreamSrc.IsNull())
                {
                    StopSwitchToPeriEqp( m_tRecEqp.GetEqpId(), byChnnl, FALSE, MODE_SECVIDEO );
                }

				//change status
				m_byRecChnnl = 0;
				memset( &m_tRecEqp, 0, sizeof( m_tRecEqp ) );
                TRecStartPara tOldRecParam = m_tRecPara;
                m_tRecPara.Reset();
				m_tConf.m_tStatus.SetNoRecording();

                //¼���������������ϵ�������
                if (tOldRecParam.IsRecLowStream() && m_tConf.m_tStatus.IsBrAdapting())
                {
                    // guzh [8/29/2007] ���ϵ���Ϊ�������䣨��Ҫ��������ʽ�����ReqBitrate��
                    ChangeAdapt(ADAPT_TYPE_BR, GetLeastMtReqBitrate(TRUE, m_tConf.GetMainVideoMediaType()));
                }

                // guzh [6/13/2007]
                TSimCapSet tDstSCS;
                TSimCapSet tSrcSCS;
                u16 wAdaptBitrate;
                if (  m_tConf.m_tStatus.IsVidAdapting() && 
                     (IsNeedVidAdapt(tDstSCS, tSrcSCS,wAdaptBitrate) || 
                      IsNeedCifAdp())
                   )
                {
                    ChangeAdapt(ADAPT_TYPE_VID, wAdaptBitrate, &tDstSCS, &tSrcSCS);
                }
                
                /*
                // zbq [06/07/2007] �������ָ�һ������
                if (m_tRecPara.IsRecLowStream() && m_tConf.m_tStatus.IsVidAdapting())
                {
                    u8 byConfMainVType = m_tConf.GetMainVideoMediaType();
                    u8 byConfSecVType  = m_tConf.GetMainAudioMediaType();

                    TSimCapSet tSSim;
                    TMt tSpeaker = GetLocalSpeaker();

                    if (tSpeaker.GetMtType() == MT_TYPE_MT)
                    {
                        tSSim = m_ptMtTable->GetDstSCS(GetLocalSpeaker().GetMtId());
                    }
                    else // ����ͳͳ��Ϊ������ʽ FIXME: ¼���˫������ ?
                    {
                        tSSim.SetVideoMediaType(byConfMainVType);
                    }

                    BOOL32 bPrimary = tSSim.GetVideoMediaType() == byConfMainVType ? TRUE : FALSE;
                    u8 byAdaptDstType = bPrimary ? byConfSecVType : byConfMainVType;
                    
                    ChangeAdapt(ADAPT_TYPE_VID, GetLeastMtReqBitrate(bPrimary, byAdaptDstType));
                }     
                */
            }
			break;
			case TRecChnnlStatus::STATE_RECORDING:	//��ʼ��ָ�¼��

                //�������
                m_byRecChnnl = byChnnl;
                m_tRecEqp = *ptEqp;

                //change status
                if (ptChnnlStatus->m_byRecMode == REC_MODE_SKIPFRAME)
                {
                    m_tConf.m_tStatus.m_tConfMode.SetRecSkipFrame(TRUE);
                }
                else
                {
                    m_tConf.m_tStatus.m_tConfMode.SetRecSkipFrame(FALSE);
                }

				//�ָ�¼��ʱ�����ؽ�����
				if (m_tConf.m_tStatus.IsNoRecording())
				{
					//��ʼ¼��
					//ͼ��
                    AdjustRecordSrcStream( MODE_VIDEO );
                    //����
                    AdjustRecordSrcStream( MODE_AUDIO );
                    //˫��
                    AdjustRecordSrcStream( MODE_SECVIDEO );
                    //����˫��Դ�ؼ�֡
                    if ( m_tRecPara.IsRecDStream() && 
                         !m_tDoubleStreamSrc.IsNull() && m_tDoubleStreamSrc.GetType() == TYPE_MT )
                    {
                        /*
                        if (m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE))
                        {
                            tDStreamAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                        }
                        g_cMpManager.GetSwitchInfo(m_tDoubleStreamSrc, dwDStreamSwitchIp, wMtSwitchPort, dwMtSrcIp);
                        */

                        NotifyFastUpdate(m_tDoubleStreamSrc, MODE_SECVIDEO);
                    }
                    
                    ///FIXME: guzh [6/12/2007] �������PrsԴû�п��ǹ㲥Դ�仯���������ʱ��֧��
                    // ��ע��ָ�ʱ���������Ҫ�޸� tRecSrc
                    /*
                    TMt tRecSrc;                    
                    u32 dwVideoSwitchIp;
                    u32 dwAudioSwitchIp;
                    u32 dwDStreamSwitchIp;
                    u16 wMtSwitchPort;
                    u32 dwMtSrcIp;
                    TTransportAddr tVideoAddr;
                    TTransportAddr tAudioAddr;
                    TTransportAddr tDStreamAddr;
                    TLogicalChannel tLogicalChannel;

                    //��ƵRTCP
                    // ������ն�
                    if (tRecSrc.GetType() == TYPE_MT)
                    {                                                        
                        if (m_ptMtTable->GetMtLogicChnnl(tRecSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
                        {
                            tVideoAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                        }
                    }
                    // �����������(VMP, Player or Bas)
                    else if (tRecSrc.GetEqpType() == EQP_TYPE_BAS ||
                             tRecSrc.GetEqpType() == EQP_TYPE_VMP)
                    {
                        // ����Ǵ�Bas/Vmp
                        u8 byChlNum;
                        g_cMcuVcApp.GetPeriEqpLogicChnnl(tRecSrc.GetEqpId(), MODE_VIDEO, &byChlNum, &tLogicalChannel, FALSE);
                        tVideoAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                    } 

                    if(!g_cMpManager.GetSwitchInfo(tRecSrc, dwVideoSwitchIp, wMtSwitchPort, dwMtSrcIp))
                    {
                        ConfLog(FALSE, "[ProcRecMcuRecChnnlStatusNotif] GetSwitchInfo for video src failed. tRecSrc<%d,%d>\n",
                                        tRecSrc.GetMcuId(), tRecSrc.GetMtId());
                        // ���ﲻ��Ҫreturn
                    }

                    //��ƵRTCP
                    if (tRecSrc.GetType() == TYPE_MT)
                    {
                        // ������ն˽���
                        if (m_ptMtTable->GetMtLogicChnnl(tRecSrc.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE))
                        {
                            tAudioAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                        }                        
                    }
                    else if (tRecSrc.GetEqpType() == EQP_TYPE_BAS)
                    {
                        // ���������������, FIXME: ���������������Ĳ���PRS ?
                        u8 byChlNum;
                        g_cMcuVcApp.GetPeriEqpLogicChnnl(tRecSrc.GetEqpId(), MODE_AUDIO, &byChlNum, &tLogicalChannel, FALSE);
                        tAudioAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                    }
                    g_cMpManager.GetSwitchInfo(tRecSrc, dwAudioSwitchIp, wMtSwitchPort, dwMtSrcIp);
                    
                    //prs
                    if (m_tConf.GetConfAttrb().IsResendLosePack())
                    {
                        u8  byRecorderId = ptEqp->GetEqpId();
                        u32 dwSwitchIpAddr;
                        u16 wSwitchPort;
                        g_cMpManager.GetRecorderSwitchAddr(byRecorderId, dwSwitchIpAddr, wSwitchPort);

                        wSwitchPort = wSwitchPort + PORTSPAN * byChnnl;

                        //Video
                        g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwVideoSwitchIp, 0,
                                                         dwVideoSwitchIp, wSwitchPort + 6,
                                                         tVideoAddr.GetIpAddr(), tVideoAddr.GetPort());
                        //Audio
                        g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwAudioSwitchIp, 0,
                                                         dwAudioSwitchIp, wSwitchPort + 7,
                                                         tAudioAddr.GetIpAddr(), tAudioAddr.GetPort());

                        //DStream
                        if (!m_tDoubleStreamSrc.IsNull() && m_tRecPara.IsRecDStream())
                        {
                            g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwDStreamSwitchIp, 0,
                                                             dwDStreamSwitchIp, wSwitchPort + 8,
                                                             tDStreamAddr.GetIpAddr(), tDStreamAddr.GetPort());
                        }
                    }
                    */
				}
                else
                {
                    //�ָ�¼�����ն�����ؼ�֡, zgc, 2008-07-17
                    TMt tSrc;
                    u8  byRecChnIdx = 0;
                    BOOL32 bNeedBas = IsRecordSrcBas(MODE_VIDEO, tSrc, byRecChnIdx);
                    if ( !tSrc.IsNull() && tSrc.GetType() == TYPE_MT)
                    {                                   
                        NotifyFastUpdate(tSrc, MODE_VIDEO);
                    }
                    if ( m_tRecPara.IsRecDStream() && 
                        !m_tDoubleStreamSrc.IsNull() && 
                        m_tDoubleStreamSrc.GetType() == TYPE_MT )
                    {
                        NotifyFastUpdate(m_tDoubleStreamSrc, MODE_SECVIDEO);
                    }
                }

				m_tConf.m_tStatus.SetRecording();
				break;
			
			case TRecChnnlStatus::STATE_RECPAUSE:	//��ͣ¼��	
				//change status
				m_tConf.m_tStatus.SetRecPause();
				break;	
			}
			ConfModeChange();	

		}
        //�ն�¼��
		else
		{
            TMt tSMcuMt = tRecMt;
            tRecMt = GetLocalMtFromOtherMcuMt(tRecMt);
            
            switch (ptChnnlStatus->m_byState)
            {
            case TRecChnnlStatus::STATE_RECREADY:	//ֹͣ¼��
                
                //ͣ����
                if (m_tConfAllMtInfo.MtJoinedConf(tRecMt.GetMtId()))
                {
                    StopSwitchToPeriEqp(ptEqp->GetEqpId(), byChnnl, FALSE);
                }
                //��״̬
                m_ptMtTable->SetMtNoRecording(tRecMt.GetMtId());		
                break;

            case TRecChnnlStatus::STATE_RECORDING:	//��ʼ��ָ�¼��
                
                //��ʼ¼�������ն�״̬����¼�����������
                if (m_ptMtTable->IsMtNoRecording(tRecMt.GetMtId()))
                {		
                    //��¼���ն��Ѳ��ڻ��飬ֹͣ�ն�¼��
                    if (!m_tConfAllMtInfo.MtJoinedConf(tRecMt.GetMtId()))
                    {
                        CServMsg cServMsg;
                        cServMsg.SetConfId(m_tConf.GetConfId());
                        cServMsg.SetMsgBody((u8 *)ptEqp, sizeof(*ptEqp));	//set TEqp
                        cServMsg.SetChnIndex(byChnnl);
                        SendMsgToEqp(ptEqp->GetEqpId(), MCU_REC_STOPREC_REQ, cServMsg);
                    }
                    StartSwitchToPeriEqp(tRecMt, 0, ptEqp->GetEqpId(), byChnnl, MODE_BOTH);

                    //���������� [01/25/2007-zbq]
                    // �����ͨ����Ӧ����дԴ��ͨ����, zgc, 2008-07-02
                    g_cMpManager.SetSwitchBridge(tRecMt, 0/*byChnnl*/, MODE_BOTH);

                    if (m_tConf.GetConfAttrb().IsResendLosePack())
                    {
                        u8  byRecorderId = ptEqp->GetEqpId();
                        u32 dwSwitchIpAddr;
                        u16 wSwitchPort;
                        u32 dwMtSwitchIp;
                        u16 wMtSwitchPort;
                        u32 dwMtSrcIp;
                        g_cMpManager.GetRecorderSwitchAddr(byRecorderId, dwSwitchIpAddr, wSwitchPort);

                        g_cMpManager.GetSwitchInfo(tRecMt, dwMtSwitchIp, wMtSwitchPort, dwMtSrcIp);

                        wSwitchPort = wSwitchPort + PORTSPAN*byChnnl;

                        TTransportAddr  tAddr;
                        TLogicalChannel tLogicalChannel;

                        //Video
                        if (m_ptMtTable->GetMtLogicChnnl(tRecMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
                        {
                            tAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                            g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwMtSwitchIp, 0,
                                                             dwMtSwitchIp, wSwitchPort + 6,
                                                             tAddr.GetIpAddr(), tAddr.GetPort());
                        }

                        //Audio
                        if (m_ptMtTable->GetMtLogicChnnl(tRecMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE))
                        {
                            tAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                            g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwMtSwitchIp, 0,
                                                             dwMtSwitchIp, wSwitchPort + 7,
                                                             tAddr.GetIpAddr(), tAddr.GetPort());
                        }

                        /*/DStream
                        if (!m_tDoubleStreamSrc.IsNull() &&
                            m_ptMtTable->GetMtLogicChnnl(tRecMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE))
                        {
                            tAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                            g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwMtSwitchIp, 0,
                                                             dwMtSwitchIp, wSwitchPort + 8,
                                                             tAddr.GetIpAddr(), tAddr.GetPort());
                        }*/
                    }
                    if ( tSMcuMt.IsLocal() )
                    {
                        //֪ͨ�����ն˿�ʼ����
                        NotifyMtSend(tRecMt.GetMtId());
                    }
                    else
                    {
                        //�����¼��ն˻ش�����
                        OnMMcuSetIn( tSMcuMt, 0, SWITCH_MODE_SELECT );
                    }
				}
                else
                {
                    //�ָ�¼�����ն�����ؼ�֡, zgc, 2008-07-17                    
                    if ( tRecMt.GetType() == TYPE_MT)
                    {                                   
                        NotifyFastUpdate(tRecMt, MODE_VIDEO);
                    }
                    if ( m_tRecPara.IsRecDStream() && 
                        !m_tDoubleStreamSrc.IsNull() && 
                        m_tDoubleStreamSrc.GetType() == TYPE_MT )
                    {
                        NotifyFastUpdate(m_tDoubleStreamSrc, MODE_SECVIDEO);
                    }
                }

                //����Ϊ¼��״̬
                m_ptMtTable->SetMtRecording(tRecMt.GetMtId(), *ptEqp, byChnnl);

                if (ptChnnlStatus->m_byRecMode == REC_MODE_SKIPFRAME)
                {
                    m_ptMtTable->SetMtRecSkipFrame(tRecMt.GetMtId(), TRUE);
                }
                else
                {
                    m_ptMtTable->SetMtRecSkipFrame(tRecMt.GetMtId(), FALSE);
                }
				break;

			case TRecChnnlStatus::STATE_RECPAUSE:	//��ͣ¼��	
				
                //��״̬
				m_ptMtTable->SetMtRecPause( tRecMt.GetMtId() );
				break;

			default:
				ConfLog( FALSE, "Unexcept State Recevied In RecordMtNotify.\n" );
				return;
			}

            TMtStatus tMtStatus;
            m_ptMtTable->GetMtStatus(tRecMt.GetMtId(), &tMtStatus);

            MtStatusChange(tRecMt.GetMtId(), TRUE);

			if( tMtStatus.m_tRecState.IsNoRecording() )
			{
				EqpLog( "Mt %d rec state change, new state: IsNoRecording!\n", tRecMt.GetMtId() );
			}
			else if( tMtStatus.m_tRecState.IsRecording() )
			{
				EqpLog( "Mt %d rec state change, new state: IsRecording!\n", tRecMt.GetMtId());
			}
			else if( tMtStatus.m_tRecState.IsRecPause() )
			{
				EqpLog( "Mt %d rec state change, new state: IsRecPause!\n", tRecMt.GetMtId());
			}
			else if( tMtStatus.m_tRecState.IsRecSkipFrame() )
			{
				EqpLog( "Mt %d rec state change, new state: IsRecSkipFrame!\n",tRecMt.GetMtId());
			}

		}
		break;		

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcRecMcuProgNotif
    ����        ��¼�������֪ͨ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/20    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcRecMcuProgNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TRecProg	* ptRecProg = NULL;
    TEqp        tEqp;
    TMt         tMt;
	u16         wMcEvent = 0;
    
	switch( CurState() )
	{
	case STATE_ONGOING:
		if( pcMsg->event == REC_MCU_PLAYPROG_NOTIF )
		{
			ptRecProg = ( TRecProg * )( cServMsg.GetMsgBody() + sizeof( TEqp ) );
			m_tConf.m_tStatus.m_tPlayProg = *ptRecProg;
			wMcEvent = MCU_MCS_PLAYPROG_NOTIF;
		}
		else
		{
		    tMt  = *( TMt* )cServMsg.GetMsgBody();
            tEqp = *( TMt* )( cServMsg.GetMsgBody() + sizeof( TMt ) );
			ptRecProg = ( TRecProg * )( cServMsg.GetMsgBody() + sizeof( TMt ) + sizeof( TEqp ) );
			if( tMt.GetMcuId() == 0 )
			{
				m_tConf.m_tStatus.m_tRecProg = *ptRecProg;				
			}
			else
			{
                if ( !tMt.IsLocal() )
                {
                    tMt = GetLocalMtFromOtherMcuMt(tMt);

                    //���¹��� cServMsg
                    cServMsg.SetMsgBody( (u8*)&tMt,  sizeof(TMt) );
                    cServMsg.CatMsgBody( (u8*)&tEqp, sizeof(TEqp) );
                    cServMsg.CatMsgBody( (u8*)ptRecProg, sizeof(TRecProg) );
                }
                m_ptMtTable->SetMtRecProg( tMt.GetMtId() , *ptRecProg );
			}
			wMcEvent = MCU_MCS_RECPROG_NOTIF;
		}

		///////////////////
		if( pcMsg->event == REC_MCU_PLAYPROG_NOTIF )
		{
			m_tConf.m_tStatus.m_tPlayProg = *ptRecProg;
		}
		else
		{
			m_tConf.m_tStatus.m_tRecProg = *ptRecProg;
		}

    	SendMsgToAllMcs( wMcEvent, cServMsg );
		break;
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			             pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ProcRecMCUNeedIFrameCmd
    ����        ��¼�������ؼ�֡
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/12/18    4.0         ���ֱ�          ����
====================================================================*/
void CMcuVcInst::ProcRecMcuNeedIFrameCmd(const CMessage * pcMsg)
{	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TMt tSrcMt;
    TEqp tBasSrc;
    u8 byChnId = 0;

    switch(CurState())
    {
    case STATE_ONGOING:

        switch( pcMsg->event ) 
        {			
        case REC_MCU_NEEDIFRAME_CMD:
			// xliang [3/6/2009] ������VMP����ؼ�֡����VMP��speaker����ʱ��¼��¼VMP��ͼ��
			if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE
				&& m_tConf.m_tStatus.IsBrdstVMP())
			{
				tSrcMt = m_tVmpEqp;
			
				u8	byMtMediaType = m_tConf.GetMainVideoMediaType();
				u8	byMtRes = m_tConf.GetMainVideoFormat();
		 		byChnId = GetVmpOutChnnlByRes(byMtRes, m_tVmpEqp.GetEqpId(), byMtMediaType);

				NotifyEqpFastUpdate(tSrcMt, byChnId);
			}
            else if (IsRecordSrcBas(MODE_VIDEO, tBasSrc, byChnId))
            {
                //¼����ᶨʱ�������Σ��˴�����ʱ
                NotifyEqpFastUpdate(tBasSrc, byChnId, FALSE);
            }
            else if(m_tConf.HasSpeaker())
            {
                tSrcMt = GetLocalSpeaker();
                NotifyFastUpdate(tSrcMt, MODE_VIDEO);
            }
            else
            {
                ConfLog(FALSE, "[ProcRecMCUNeedIFrameCmd] Err: No VMP mode or speaker for REC_MCU_NEEDIFRAME_CMD\n");
            }
            break;
        default:
			ConfLog(FALSE, "[ProcRecMCUNeedIFrameCmd]:The Message type %u(%s) is not fit", pcMsg->event, ::OspEventDesc(pcMsg->event) );
            break;
        }
        break;

    default:
        ConfLog(FALSE, "[ProcRecMCUNeedIFrameCmd]Wrong message %u(%s) received in state %u!\n", 
                        pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
    }
	return;
}

/*====================================================================
    ������      ��ProcMcuRecConnectedNotif
    ����        ��¼����ϵ�֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/09/28    4.0         libo          ����
====================================================================*/
void CMcuVcInst::ProcMcuRecConnectedNotif(const CMessage * pcMsg)
{
    if (STATE_ONGOING != CurState())
    {
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TEqp tEqp;
    TPeriEqpStatus tRecStatus;
    TRecChnnlStatus tRecChanStatus;
    TPeriEqpRegReq tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();

    
    if (tRegReq.GetEqpId() != m_tPlayEqp.GetEqpId() ||
        EQP_CHANNO_INVALID == m_byPlayChnnl)
    {
        /*
        OspPrintf(TRUE, FALSE, "tRegReq.GetEqpId().%d != m_tPlayEqp.GetEqpId().%d\n",
                               tRegReq.GetEqpId(), m_tPlayEqp.GetEqpId());
        */
        return;
    }

    // ����ָ�
    tEqp.SetMcuEqp(tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());

    g_cMcuVcApp.GetPeriEqpStatus(tRegReq.GetEqpId(), &tRecStatus);

    tRecStatus.m_tStatus.tRecorder.GetChnnlStatus(m_byPlayChnnl,
                                                  TRecChnnlStatus::TYPE_PLAY,
                                                  &tRecChanStatus);

    cServMsg.SetMsgBody((u8 *)&tEqp, sizeof(TEqp));
    cServMsg.CatMsgBody((u8 *)&tRecChanStatus.m_tProg, sizeof(TRecProg));
    cServMsg.CatMsgBody((u8 *)tRecChanStatus.GetRecordName(),
                        strlen(tRecChanStatus.GetRecordName()));

    StartPlayReq(cServMsg);

    return;
}

/*====================================================================
    ������      ��ProcMcuRecDisconnectedNotif
    ����        ��¼�������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/26    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMcuRecDisconnectedNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8      byMtId;
	TEqp	tEqp = *( TEqp * )cServMsg.GetMsgBody();

	switch( CurState() )
	{
	case STATE_ONGOING:

		//ֹͣ����¼�񽻻�
		if( !m_tConf.m_tStatus.IsNoRecording() && (m_tRecEqp == tEqp) )
        {
			StopSwitchToPeriEqp( m_tRecEqp.GetEqpId() ,m_byRecChnnl);
			m_byRecChnnl = 0;
			memset( &m_tRecEqp, 0, sizeof( m_tRecEqp ) );
			m_tConf.m_tStatus.SetNoRecording();
			ConfModeChange( );
            log( LOGLVL_IMPORTANT, "CMcuVcInst: Conferece %s recording state cancelled due to MCU%u's VOD%u disconnected!\n", 
                m_tConf.GetConfName(), tEqp.GetMcuId(), tEqp.GetEqpId() );
        }

		cServMsg.SetConfId( m_tConf.GetConfId() );

		//�ж��Ƿ��Ƿ�������
		if( !m_tConf.m_tStatus.IsNoPlaying() && (m_tPlayEqp==tEqp) )
		{
			//change status
			//m_byPlayChnnl = 0;
			//memset( &m_tPlayEqp, 0, sizeof( m_tPlayEqp ) );
			m_tConf.m_tStatus.SetNoPlaying();
			ConfModeChange();
            
            if (TYPE_MCUPERI      == m_tDoubleStreamSrc.GetType() &&
                EQP_TYPE_RECORDER == m_tDoubleStreamSrc.GetEqpType())
            {
                StopDoubleStream(FALSE, TRUE);
            }

			//change speaker
			ChangeSpeaker( NULL );
		}

		//�ն�¼��
		for( byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
		{				
			
			if( m_tConfAllMtInfo.MtJoinedConf( byMtId ) && !m_ptMtTable->IsMtNoRecording( byMtId ) )
			{
				TEqp tRecEqp;
				u8   byRecChn;
				TMtStatus tMtStatus;

				m_ptMtTable->GetMtRecordInfo( byMtId ,&tRecEqp,&byRecChn );

                //�ն�¼���¼����Ƕ�����¼���
                if ( tRecEqp == tEqp )
                {
                    StopSwitchToPeriEqp( tRecEqp.GetEqpId(),byRecChn);                    
                    m_ptMtTable->SetMtNoRecording(byMtId);
                    MtStatusChange(byMtId, TRUE);
                }				
			}
		}
			
		break;

	case STATE_SCHEDULED:
		break;
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcRecMcuResp
    ����        ������¼�����MCU�Ļ�Ӧ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/09/23    4.0         libo          ����
====================================================================*/
void CMcuVcInst::ProcRecMcuResp(const CMessage * pcMsg)
{
    if (STATE_ONGOING != CurState())
    {
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();
	CConfId	cConfId = cServMsg.GetConfId();
	u8 byChnnl = cServMsg.GetChnIndex();

    switch(pcMsg->event)
    {
    case REC_MCU_STARTPLAY_ACK:
        {
            TPlayFileAttrib tPlayFileAttrib;
            tPlayFileAttrib = *(TPlayFileAttrib *)(cServMsg.GetMsgBody() + sizeof(TEqp));
            m_tPlayEqpAttrib.SetFileType(tPlayFileAttrib.GetFileType());
            m_tPlayEqpAttrib.SetAudioType(tPlayFileAttrib.GetAudioType());
            m_tPlayEqpAttrib.SetVideoType(tPlayFileAttrib.GetVideoType());
            m_tPlayEqpAttrib.SetDVideoType(tPlayFileAttrib.GetDVideoType());            
            
            
            // zgc, 2008-08-28, ��¼�����ļ�ý����Ϣ
            if ( cServMsg.GetMsgBodyLen() > sizeof(TEqp) + sizeof(TPlayFileAttrib) )
            {
                m_tPlayFileMediaInfo = *(TPlayFileMediaInfo *)( cServMsg.GetMsgBody() + sizeof(TEqp) + sizeof(TPlayFileAttrib) );
                u8 byType = 0;
                u16 wWidth = 0;
                u16 wHeight = 0;
                m_tPlayFileMediaInfo.GetVideo( byType, wWidth, wHeight );
                EqpLog( "[ProcRecMcuResp]PlayFileMediaInfo: Type.%d, Width.%d, Height.%\n", 
                            byType, wWidth, wHeight );
            }
            else
            {
                EqpLog( "[ProcRecMcuResp] Receive old version recorder REC_MCU_STARTPLAY_ACK\n" );
            }
            
			if (m_tPlayEqpAttrib.GetDVideoType() == m_tConf.GetDStreamMediaType()
				 ||( m_tConf.GetCapSupportEx().IsDDStreamCap()
				     && ( MEDIA_TYPE_H264 == m_tPlayEqpAttrib.GetDVideoType() 
					      || MEDIA_TYPE_H263PLUS == m_tPlayEqpAttrib.GetDVideoType()
						 )
				   )
			   )
            {
                if (m_tPlayEqpAttrib.IsDStreamPlay() && m_tPlayEqpAttrib.IsDStreamFile())
                {
                    TLogicalChannel tLogicChnnl;
                    tLogicChnnl.SetChannelType(m_tPlayEqpAttrib.GetDVideoType());
                    tLogicChnnl.SetSupportH239(m_tConf.GetCapSupport().IsDStreamSupportH239());

                    tEqp.SetConfIdx(m_byConfIdx);
                    StartDoubleStream(tEqp, tLogicChnnl);
                }
            }
			//��������ˢ�½��� ��ʱ֧�� �Ժ�����ʶ�ָ�״̬���걸��			
			//change speaker if it's not speaker now			
			//if( m_tConf.m_tStatus.IsNoPlaying() )
			{
				m_byPlayChnnl = byChnnl;
				m_tPlayEqp = tEqp;
				m_tPlayEqp.SetConfIdx( m_byConfIdx );
				ChangeSpeaker( &m_tPlayEqp );
                
                //zbq [09/05/2007] ¼���˫�����󣬴˴��ſ��Խ���
                if ( m_tDoubleStreamSrc == tEqp )
                {
                    g_cMpManager.SetSwitchBridge(tEqp, byChnnl, MODE_SECVIDEO);
                }

				// �����ȴ�����������ʱ��, zgc, 2008-03-27
				SetTimer( MCUVC_RECPLAY_WAITMPACK_TIMER, 4*1000 );
			}
        }
	
		//SendMsgToEqp( m_tPlayEqp.GetEqpId() , MCU_EQP_SWITCHSTART_NOTIF, cServMsg);
        //��mcs��ʼ�����ack
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
    	break;
    case REC_MCU_STARTREC_ACK:
    case REC_MCU_STOPREC_ACK:
        {
            // guzh [9/8/2006] ��ʼ/ֹͣ¼��ɹ�����Ҫ����������
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

            u16 wError = cServMsg.GetErrorCode();
            if (wError != 0)
            {
                NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), wError );
            }
        }
        break;
    default:
        break;
    }


    return;
}

/*------------------------------------------------------------------*/
/*                                 BAS                              */
/*------------------------------------------------------------------*/

/*====================================================================
    ������      : ProcBasChnnlStatusNotif
    ����        ������������ͨ��״̬֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/18    3.0         zmy           Create
====================================================================*/
void CMcuVcInst::ProcBasChnnlStatusNotif(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8  byChnIdx;
    u8  byEqpId;
    u8 *pbyMsgBody;
    TBasChnStatus *ptChnStatus;
    TPeriEqpStatus tEqpStatus;
    CConfId cConfId = cServMsg.GetConfId();

    byChnIdx    = cServMsg.GetChnIndex();
    pbyMsgBody  = (u8 *)cServMsg.GetMsgBody();
    byEqpId     = *pbyMsgBody;
    pbyMsgBody += sizeof(u8);
    ptChnStatus = (TBasChnStatus*)pbyMsgBody;

    //save to periequipment status
    if (!g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tEqpStatus))
    {
        return;
    }

    // xsl [9/29/2006] reserved�ֶ�Ϊmcu�ϲ����
    BOOL32 bReserved = tEqpStatus.m_tStatus.tBas.tChnnl[byChnIdx].IsReserved();
    tEqpStatus.m_tStatus.tBas.tChnnl[byChnIdx] = *ptChnStatus;
    tEqpStatus.m_tStatus.tBas.tChnnl[byChnIdx].SetReserved(bReserved);

    EqpLog("[Bas ChanIdx.%d]GetChannelType() = %d\n", byChnIdx, ptChnStatus->GetChannelType());

    g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tEqpStatus);

    //analyze conference id and forward it to conference
    cServMsg.SetMsgBody((u8 *)&tEqpStatus, sizeof(tEqpStatus));
    SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
    
    return;
}

/*=============================================================================
 �� �� ���� DaemonProcHDBasChnnlStatusNotif 
 ��    �ܣ� 
 �㷨ʵ�֣� 
 ȫ�ֱ����� 
 ��    ���� const CMessage * pcMsg
 �� �� ֵ�� void 
 ----------------------------------------------------------------------
 �޸ļ�¼    ��
 ��  ��		�汾		�޸���		�߶���    �޸�����
 2008/8/8   4.0		    �ܹ��                  ����
 2009/5/2   4.6		    �ű���                  ����mpu
=============================================================================*/
void CMcuVcInst::DaemonProcHDBasChnnlStatusNotif( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
    CConfId cConfId = cServMsg.GetConfId();
    u8  byChnIdx = cServMsg.GetChnIndex();
    u8  byChnType = *(u8*)cServMsg.GetMsgBody();;
    
    THDBasVidChnStatus tVidChnStatus;
    memset(&tVidChnStatus, 0, sizeof(tVidChnStatus));
    memcpy( &tVidChnStatus, cServMsg.GetMsgBody() + sizeof(u8), sizeof(THDBasVidChnStatus) );
    
    THDBasChnStatus tChnStatus = (THDBasChnStatus)tVidChnStatus;
    u8 byEqpId = tVidChnStatus.GetEqpId();
    u8 byStatus = tVidChnStatus.GetStatus();

    TPeriEqpStatus tEqpStatus;
    g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tEqpStatus );

    switch (byChnType)
    {
    case HDBAS_CHNTYPE_MAU_MV:
        {
            THDBasVidChnStatus tMVStatus = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus();
            BOOL32 bIsReserved = tMVStatus.IsReserved();
            BOOL32 bIsTmpReserved = tMVStatus.IsTempReserved();
            
            tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tVidChnStatus);
            
            if ( bIsReserved )
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->SetIsReserved( TRUE );
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tMVStatus);
            }
            else if ( bIsTmpReserved )
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->SetIsTempReserved( TRUE );
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tMVStatus);
            }
            else
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->UnReserved();
            }
            g_cMcuVcApp.SetPeriEqpStatus( byEqpId, &tEqpStatus );
			g_cMcuVcApp.SendPeriEqpStatusToMcs(byEqpId);
        }
    	break;

    case HDBAS_CHNTYPE_MAU_DS:
        {
            THDBasDVidChnStatus tDSStatus = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus();
            BOOL32 bIsReserved = tDSStatus.IsReserved();
            BOOL32 bIsTmpReserved = tDSStatus.IsTempReserved();
            
            tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tVidChnStatus);
            
            if ( bIsReserved )
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->SetIsReserved( TRUE );
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tDSStatus);
            }
            else if ( bIsTmpReserved )
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->SetIsTempReserved( TRUE );
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tDSStatus);
            }
            else
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->UnReserved();
            }
            g_cMcuVcApp.SetPeriEqpStatus( byEqpId, &tEqpStatus );
			g_cMcuVcApp.SendPeriEqpStatusToMcs(byEqpId);
        }
        break;

    case HDBAS_CHNTYPE_MPU:
        {
            if (byChnIdx >= MAXNUM_MPU_CHN)
            {
                break;
            }
            THDBasVidChnStatus tMVStatus = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx);
            BOOL32 bIsReserved = tMVStatus.IsReserved();
            BOOL32 bIsTmpReserved = tMVStatus.IsTempReserved();
            
            tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tVidChnStatus, byChnIdx);
            
            if ( bIsReserved )
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->SetIsReserved( TRUE );
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tMVStatus, byChnIdx);
            }
            else if ( bIsTmpReserved )
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->SetIsTempReserved( TRUE );
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tMVStatus, byChnIdx);
            }
            else
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->UnReserved();
            }
            g_cMcuVcApp.SetPeriEqpStatus( byEqpId, &tEqpStatus );
			g_cMcuVcApp.SendPeriEqpStatusToMcs(byEqpId);
        }
        break;
    default:
        ConfLog( FALSE, "[DaemonProcHDBasChnnlStatusNotif] unexpected chnType.%d!\n", byChnType );
        break;
    }

    //ת�������л���
    if ( tChnStatus.GetStatus() == THDBasChnStatus::READY )
    {
        g_cMcuVcApp.BroadcastToAllConf(pcMsg->event, pcMsg->content, pcMsg->length);
    }

    EqpLog( "[DaemonProcHDBasChnnlStatusNotif] EqpId.%d, ChnIdx.%d, ChnType.%d, ChnStatus.%d\n" ,
             byEqpId, byChnIdx, byChnType, byStatus );
    return;
}

/*=============================================================================
    �� �� ���� DaemonProcHduMcuChnnlStatusNotif
    ��    �ܣ� 
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/1/13   4.6		    �ű���                  ����
=============================================================================*/
void CMcuVcInst::DaemonProcHduMcuChnnlStatusNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp tEqp;
    u8 byChnnl;
	u8 bFirstNotif;
	THduChnStatus *ptChnnlStatus;
    CConfId cConfId = cServMsg.GetConfId();
    TPeriEqpStatus tHduStatus;

    u8 bOldMute = FALSE;
	u8 byOldVolume = 0;
    ptChnnlStatus = (THduChnStatus *)cServMsg.GetMsgBody();	
	bFirstNotif = *(u8*)(cServMsg.GetMsgBody() + sizeof(THduChnStatus));

	tEqp = ptChnnlStatus->GetEqp();
    byChnnl = ptChnnlStatus->GetChnIdx();

    if (!g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus))
    {
		EqpLog("[DaemonProcHduMcuChnnlStatusNotif]:GetPeriEqpStatus failure!\n");
        return;
    }
	if ( bFirstNotif )
	{
        bOldMute = tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnnl].GetIsMute();
		byOldVolume = tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnnl].GetVolume();
		memcpy((void*)&tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnnl], (void*)ptChnnlStatus, sizeof(THduChnStatus));
	    tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnnl].SetIsMute( bOldMute );
		tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnnl].SetVolume( byOldVolume );
		THduVolumeInfo tHduVolumeInfo;
		tHduVolumeInfo.SetIsMute( bOldMute );
		tHduVolumeInfo.SetVolume( byOldVolume );
		tHduVolumeInfo.SetChnlIdx( byChnnl );
		cServMsg.SetMsgBody( (u8*)&tHduVolumeInfo, sizeof(tHduVolumeInfo) );
		SendMsgToEqp( tEqp.GetEqpId(), MCU_HDU_CHANGEVOLUME_CMD, cServMsg );
	}
	else
	{
		memcpy((void*)&tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnnl], (void*)ptChnnlStatus, sizeof(THduChnStatus));
	}

	g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);
	g_cMcuVcApp.BroadcastToAllConf( pcMsg->event, pcMsg->content, pcMsg->length );

    return;
}

/*====================================================================
    ������      ��ProcBasMcuRsp
    ����        ��������������ʼ����Ӧ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/26    1.0         LI Yi         ����
	03/11/25    3.0         zmy           Modify
	04/03/22    3.0         ������        �޸�
====================================================================*/
//modify bas 2
void CMcuVcInst::ProcBasMcuRsp(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TPeriEqpStatus tStatus;
    TConfMtInfo tConfMtInfo;
    TSimCapSet tSimCapSet;
    //TEqp *ptEqp;
    TMt tAdaptSrc;
    u8 bySrcChnnl;
    u8 byBasMode;
    u8 byAdaptType;
    u8 byMediaMode;

    u8 byEqpChlIdx = cServMsg.GetChnIndex();
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();

    tAdaptSrc.SetNull();

    //ȷ����������������Ӧ��
    if (tEqp.GetEqpId() == m_tVidBasEqp.GetEqpId() && byEqpChlIdx == m_byVidBasChnnl && m_tConf.m_tStatus.IsVidAdapting() )
    {
        byMediaMode = MODE_VIDEO;
        byAdaptType = ADAPT_TYPE_VID;
        byBasMode = CONF_BASMODE_VID;
        tEqp = m_tVidBasEqp;
        tAdaptSrc = m_tVidBrdSrc;
    }
    else if (tEqp.GetEqpId() == m_tBrBasEqp.GetEqpId() && byEqpChlIdx == m_byBrBasChnnl  && m_tConf.m_tStatus.IsBrAdapting())
    {
        byMediaMode = MODE_VIDEO;
        byAdaptType = ADAPT_TYPE_BR;
        byBasMode = CONF_BASMODE_BR;
        tEqp = m_tBrBasEqp;
        tAdaptSrc = m_tVidBrdSrc;
    }
    else if (tEqp.GetEqpId() == m_tAudBasEqp.GetEqpId() && byEqpChlIdx == m_byAudBasChnnl && m_tConf.m_tStatus.IsAudAdapting() )
    {
        byMediaMode = MODE_AUDIO;
        byAdaptType = ADAPT_TYPE_AUD;
        byBasMode = CONF_BASMODE_AUD;
        tEqp = m_tAudBasEqp;
        tAdaptSrc = m_tAudBrdSrc;
    }
    else if (tEqp.GetEqpId() == m_tCasdVidBasEqp.GetEqpId() && byEqpChlIdx == m_byCasdVidBasChnnl && m_tConf.m_tStatus.IsCasdVidAdapting() )
    {
        byMediaMode = MODE_VIDEO;
        byAdaptType = ADAPT_TYPE_CASDVID;
        byBasMode = CONF_BASMODE_CASDVID;
        tEqp = m_tCasdVidBasEqp;
        tAdaptSrc = m_tVidBrdSrc;
    }
    else if (tEqp.GetEqpId() == m_tCasdAudBasEqp.GetEqpId() && byEqpChlIdx == m_byCasdAudBasChnnl && m_tConf.m_tStatus.IsCasdAudAdapting() )
    {
        byMediaMode = MODE_AUDIO;
        byAdaptType = ADAPT_TYPE_CASDAUD;
        byBasMode = CONF_BASMODE_CASDAUD;
        tEqp = m_tCasdAudBasEqp;
        tAdaptSrc = m_tAudBrdSrc;
    }
    else
    {
        ConfLog(FALSE, "Unexpected %d(%s) received or audseladp, EqpId: %d!\n",
                pcMsg->event, OspEventDesc(pcMsg->event), tEqp.GetEqpId());
        return;
    }

    switch (pcMsg->event)
    {
    case BAS_MCU_STARTADAPT_ACK:

        //ȡ��������Ӧ��ʱ
        KillTimer(MCUVC_WAITBASRRSP_TIMER + byAdaptType - 1);
        EqpLog("[BAS_MCU_STARTADAPT_ACK]KillTimer(%d)\n", MCUVC_WAITBASRRSP_TIMER + byAdaptType - 1);

        if (ADAPT_TYPE_CASDAUD == byAdaptType || ADAPT_TYPE_CASDVID == byAdaptType)
        {
            return;
        }

        //��������
        if (tAdaptSrc.IsNull())
        {
            ConfLog(FALSE, "Start media type adapt but has no video broadcast source!\n");
        }
        else
        {
            //ͼ�񽻻�������������
            if (ADAPT_TYPE_VID == byAdaptType)
            {
                bySrcChnnl = (tAdaptSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
                StartSwitchToPeriEqp(tAdaptSrc, bySrcChnnl, tEqp.GetEqpId(), byEqpChlIdx, MODE_VIDEO);
            }

            //ͼ�񽻻�������������
            if (ADAPT_TYPE_BR == byAdaptType)
            {
                bySrcChnnl = (tAdaptSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
                StartSwitchToPeriEqp(tAdaptSrc, bySrcChnnl, tEqp.GetEqpId(), byEqpChlIdx, MODE_VIDEO);
            }

            //��������������������
            if (ADAPT_TYPE_AUD == byAdaptType)
            {
                bySrcChnnl = (tAdaptSrc == m_tMixEqp) ? m_byMixGrpId : 0;
                StartSwitchToPeriEqp(tAdaptSrc, bySrcChnnl, tEqp.GetEqpId(), byEqpChlIdx, MODE_AUDIO);
            }

            //byMediaMode = MODE_VIDEO;

            //�����������Ľ�����
            g_cMpManager.SetSwitchBridge(tEqp, byEqpChlIdx, byMediaMode);
            
            bySrcChnnl = (tAdaptSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
            
            //��������������prs
            if (m_tConf.GetConfAttrb().IsResendLosePack())
            {               
                if (ADAPT_TYPE_AUD == byAdaptType)
                {
                    StartSwitchToPeriEqp(tEqp, byEqpChlIdx, m_tPrsEqp.GetEqpId(), m_byPrsChnnlAudBas, MODE_AUDIO);

                    ChangePrsSrc(m_tAudBrdSrc, PRSCHANMODE_AUDBAS, TRUE);                    
                }
                else if (ADAPT_TYPE_VID == byAdaptType)
                {
                    StartSwitchToPeriEqp(tEqp, byEqpChlIdx, m_tPrsEqp.GetEqpId(), m_byPrsChnnlVidBas, MODE_VIDEO);

                    ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_VIDBAS, TRUE);                    
                }
                else if (ADAPT_TYPE_BR == byAdaptType)
                {
                    StartSwitchToPeriEqp(tEqp, byEqpChlIdx, m_tPrsEqp.GetEqpId(), m_byPrsChnnlBrBas, MODE_VIDEO);

                    ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_BRBAS, TRUE);                                    }               
            }

            //�����������������ն�            
            StartSwitchToAllSubMtJoinedConfNeedAdapt(tAdaptSrc, bySrcChnnl, byMediaMode, byAdaptType);	
            
            if (ADAPT_TYPE_BR == byAdaptType)
            {
                //�鲥
                if (m_tConf.GetConfAttrb().IsMulticastMode() &&
                    m_tConf.GetConfAttrb().IsMulcastLowStream())
                {
                    // guzh [8/29/2007] ��֧��˫ý���������鲥
/*
                    // guzh [8/29/2007] ��˫ý����飬��BrBas�鲥
#ifdef _MINIMCU_
                    if ( !ISTRUE(m_byIsDoubleMediaConf) )
#endif                    
*/
                        g_cMpManager.StartMulticast(tEqp, byEqpChlIdx, byMediaMode);	
                }                

                // ����¼����Ҫ����BrBas
                TEqp tRecSrc;
                u8 byRecChnlIdx = 0;
                if (!m_tConf.m_tStatus.IsNoRecording() && 
                    IsRecordSrcBas(MODE_VIDEO, tRecSrc, byRecChnlIdx) &&
                    tEqp == tRecSrc )
                {
                    StartSwitchToPeriEqp(tEqp, byEqpChlIdx, m_tRecEqp.GetEqpId(), m_byRecChnnl, byMediaMode);
                }
            }

            else if (ADAPT_TYPE_VID == byAdaptType)
            {
                // ����¼����Ҫ����VidBas
                TEqp tRecSrc;
                u8   byRecChnlIdx = 0;
                if (!m_tConf.m_tStatus.IsNoRecording() &&
                    IsRecordSrcBas(MODE_VIDEO, tRecSrc, byRecChnlIdx) &&
                    tEqp == tRecSrc )
                {
                    StartSwitchToPeriEqp(tEqp, byEqpChlIdx, m_tRecEqp.GetEqpId(), m_byRecChnnl, byMediaMode);
                }
                // guzh [8/29/2007] ��֧��˫ý���������鲥
/*
#ifdef _MINIMCU_
                // guzh [8/29/2007] ˫ý����飬��VidBas�鲥
                if ( ISTRUE(m_byIsDoubleMediaConf) )
                {
                    g_cMpManager.StartMulticast(tEqp, byEqpChlIdx, byMediaMode);
                }
#endif                        
*/
            }
            else if (ADAPT_TYPE_AUD == byAdaptType)
            {
                // ����¼����Ҫ����AudBas
                TEqp tRecSrc;
                u8   byRecChnlIdx = 0;
                if (!m_tConf.m_tStatus.IsNoRecording() &&
                    IsRecordSrcBas(MODE_AUDIO, tRecSrc, byRecChnlIdx) &&
                    tEqp == tRecSrc)
                {
                    StartSwitchToPeriEqp(tEqp, byEqpChlIdx, m_tRecEqp.GetEqpId(), m_byRecChnnl, byMediaMode);
                }
            }    
            // ����@2006.4.12 �����鲥��ʱ��֧��˫�ٻ���
        }
        break;

    //�ͷ�ͨ��Ҫ����
    case BAS_MCU_STARTADAPT_NACK:
        
        KillTimer(MCUVC_WAITBASRRSP_TIMER + byAdaptType - 1);
        EqpLog("[BAS_MCU_STARTADAPT_NACK]KillTimer(%d), adapttype :%d\n", 
                MCUVC_WAITBASRRSP_TIMER + byAdaptType - 1, byAdaptType);

        g_cMcuVcApp.SetBasChanStatus(tEqp.GetEqpId(), byEqpChlIdx, TBasChnStatus::READY);

        //�ı��������״̬
        m_tConf.m_tStatus.SetAdaptMode(byBasMode, FALSE);
		ConfModeChange();
        break;

    default:
        break;
    }
}


/*====================================================================
    ������      ��ProcMcuBasConnectedNotif
    ����        ����������ǼǴ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/26    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMcuBasConnectedNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TPeriEqpRegReq tEqp = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();

    switch(CurState())
    {
    case STATE_ONGOING:

        if (EQP_TYPE_BAS == tEqp.GetEqpType() && m_tConf.GetConfAttrb().IsUseAdapter())
        {
            u16 wAdaptBitRate = 0;
            TSimCapSet tSrcSimCapSet;
            TSimCapSet tDstSimCapSet;

            if (!m_tVidBrdSrc.IsNull())
            {
                //����ָ��ȣ������Ժ�
                if ( IsHDConf( m_tConf ) )
                {
                }
                else
                {
                    if (tEqp == m_tVidBasEqp && !m_tConf.m_tStatus.IsVidAdapting() &&
                        IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
                    {
                        StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }
                    else if(tEqp == m_tVidBasEqp && !m_tConf.m_tStatus.IsVidAdapting()
                        && IsNeedCifAdp() )
                    {
                        IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate);
                        StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }

                    if (tEqp == m_tBrBasEqp && !m_tConf.m_tStatus.IsBrAdapting() &&
                        IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
                    {
                        StartAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);	
                    }
                
                    //zbq[04/08/2008] BAS�������������Իָ���Ƶ��ؽ�����������ˢһ��. ��ͬ.
                    if ( (tEqp == m_tVidBasEqp &&
                          (IsNeedCifAdp() ||
                           IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate)))
                         ||
                         (tEqp == m_tBrBasEqp &&
                          IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate)) )
                    {
                        TMt tVidSrc = m_tVidBrdSrc;
                        ChangeVidBrdSrc(&tVidSrc);
                    }
                }
            }

            if (!m_tAudBrdSrc.IsNull())
            {
                if (tEqp == m_tAudBasEqp && !m_tConf.m_tStatus.IsAudAdapting() &&
                    IsNeedAudAdapt(tDstSimCapSet, tSrcSimCapSet))
                {
                    StartAdapt(ADAPT_TYPE_AUD, 0, &tDstSimCapSet, &tSrcSimCapSet);

                    //zbq[04/08/2008] BAS�������������Իָ���Ƶ��ؽ���
                    TMt tAudSrc = m_tAudBrdSrc;
                    ChangeAudBrdSrc(&tAudSrc);
                }
            }
        }
        break;

    default:
        ConfLog(FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
                pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
    }
}

/*====================================================================
    ������      ��ProcMcuBasDisconnectedNotif
    ����        �������������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/27    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMcuBasDisconnectedNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();

    switch(CurState())
    {
    case STATE_ONGOING:

        if ( EQP_TYPE_BAS == tEqp.GetEqpType() &&
             m_tConf.m_tStatus.IsConfAdapting() )
        {
            ConfLog(FALSE, "BAS %u disconnected message received in conference %s!\n", 
                            tEqp.GetEqpId(), m_tConf.GetConfName());

            //ֹͣ��Ƶ��ʽ��������
            if (tEqp.GetEqpId() == m_tVidBasEqp.GetEqpId() && m_tConf.m_tStatus.IsVidAdapting() )
            {
                StopBasAdapt(ADAPT_TYPE_VID);
            }

            //ֹͣ��������
            if (tEqp.GetEqpId() == m_tBrBasEqp.GetEqpId() && m_tConf.m_tStatus.IsBrAdapting())
            {
                StopBasAdapt(ADAPT_TYPE_BR);
            }

            //ֹͣ��Ƶ��ʽ��������
            if (tEqp.GetEqpId() == m_tAudBasEqp.GetEqpId() && m_tConf.m_tStatus.IsAudAdapting())
            {
                StopBasAdapt(ADAPT_TYPE_AUD);
            }

            //ֹͣ������Ƶ��ʽ��������
            if (tEqp.GetEqpId() == m_tCasdVidBasEqp.GetEqpId() && m_tConf.m_tStatus.IsCasdVidAdapting())
            {
                StopBasAdapt(ADAPT_TYPE_CASDVID);
            }

            //ֹͣ������Ƶ��ʽ��������
            if (tEqp.GetEqpId() == m_tCasdAudBasEqp.GetEqpId() && m_tConf.m_tStatus.IsCasdAudAdapting())
            {
                StopBasAdapt(ADAPT_TYPE_CASDAUD);
            }

        }

        //��������: ֻͣ���������ͷ�ͨ������ֹͣbas
        if ( EQP_TYPE_BAS == tEqp.GetEqpType() &&
             g_cMcuAgent.IsEqpBasHD(tEqp.GetEqpId()))
        {

            if (m_tConf.m_tStatus.IsHdVidAdapting())
            {
				TPeriEqpStatus tStatus;
				g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tStatus);
				u8 byEqpType = tStatus.m_tStatus.tHdBas.GetEqpType();
				u8 tChnlNum;
				if (TYPE_MPU == byEqpType)
				{
					tChnlNum = MAXNUM_MPU_CHN;
				}
				else if (TYPE_MAU_NORMAL == byEqpType ||
					     TYPE_MAU_H263PLUS == byEqpType)
				{
					tChnlNum = 2;
				}
				else
				{
					tChnlNum = 0;
				}

				for (u8 byIdx = 0; byIdx < tChnlNum; byIdx++)
				{
					StopHdBasSwitch(tEqp, byIdx);
				}
//                StopHdBasSwitch(tEqp, FALSE);
            }
//             else
//             {
//                 StopHdBasSwitch(tEqp, TRUE);
//             }
        }
        break;

    default:
        ConfLog(FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
                        pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
    }
}

/*=============================================================================
    �� �� ���� StartBasAdapt
    ��    �ܣ� ������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TAdaptParam* ptParam ���������ʲ���
			   u8  byAdaptType      ��������
               TSimCapSet *pSrcSCS  ����ǰ��ԭʼ������������
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/03/08  3.6			����                  ����
=============================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::StartBasAdapt(TAdaptParam *ptParam, u8 byAdaptType, TSimCapSet *pSrcSCS)
{
    u8 byEqpId;
    u8 byChnIdx;
    TPeriEqpStatus tStatus;

    if (NULL == ptParam || NULL == pSrcSCS)
    {
        return FALSE;
    }

    //����ý���������䣬����Ԥ����������
    if (ADAPT_TYPE_VID == byAdaptType)
    {
        byEqpId  = m_tVidBasEqp.GetEqpId();
        byChnIdx = m_byVidBasChnnl;
    }
    else if (ADAPT_TYPE_BR == byAdaptType)
    {
        byEqpId  = m_tBrBasEqp.GetEqpId();
        byChnIdx = m_byBrBasChnnl;
    }
    else if (ADAPT_TYPE_AUD == byAdaptType)
    {
        byEqpId  = m_tAudBasEqp.GetEqpId();
        byChnIdx = m_byAudBasChnnl;
    }
    else if (ADAPT_TYPE_CASDVID == byAdaptType)
    {
        byEqpId  = m_tCasdVidBasEqp.GetEqpId();
        byChnIdx = m_byCasdVidBasChnnl;
    }
    else if (ADAPT_TYPE_CASDAUD == byAdaptType)
    {
        byEqpId  = m_tCasdAudBasEqp.GetEqpId();
        byChnIdx = m_byCasdAudBasChnnl;
    }
    else
    {
        //���ҿ��е������� 
        if (!g_cMcuVcApp.GetIdleBasChl(byAdaptType, byEqpId, byChnIdx))
        {
            EqpLog("CMcuVcInst: all BAS busy!\n");
            return FALSE;
        }
        else
        {
            //m_tBrBasEqp.SetMcuEqp( LOCAL_MCUID, byEqpId, EQP_TYPE_BAS );
		    //m_tBrBasEqp.SetConfIdx( (u8)GetInsID() );
		    //m_byBrBasChnnl = byChnIdx;
        }
    }

    //��ռ��
    g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
    tStatus.m_tStatus.tBas.tChnnl[byChnIdx].SetStatus(TBasChnStatus::RUNING);
    g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);		

    //�ı�����״̬
    m_tConf.m_tStatus.SetAdaptMode(byAdaptType, TRUE);
	// zgc, 2008-01-22, �ϱ�ģʽ
	ConfModeChange();

    TMediaEncrypt  tEncrypt = m_tConf.GetMediaKey();
    TDoublePayload tDVPayload;
    TDoublePayload tDAPayload;

    if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode())
    {
        ptParam->SetAudActiveType(GetActivePayload(m_tConf, ptParam->GetAudType()));
        ptParam->SetVidActiveType(GetActivePayload(m_tConf, ptParam->GetVidType()));
    }
    else
    {
        ptParam->SetAudActiveType(ptParam->GetAudType());
        ptParam->SetVidActiveType(ptParam->GetVidType());
    }

    if (ADAPT_TYPE_AUD != byAdaptType)
    {
        if (MEDIA_TYPE_H264 == pSrcSCS->GetVideoMediaType() || 
            MEDIA_TYPE_H263PLUS == pSrcSCS->GetVideoMediaType() || 
            CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
            // zbq [10/29/2007] ��ӦFEC֧��������ʽ
            m_tConf.GetCapSupportEx().IsVideoSupportFEC() )
        {
            tDVPayload.SetActivePayload(GetActivePayload(m_tConf, pSrcSCS->GetVideoMediaType()));
        }
        else
        {
            tDVPayload.SetActivePayload(pSrcSCS->GetVideoMediaType());
        }
        tDVPayload.SetRealPayLoad(pSrcSCS->GetVideoMediaType());
    }

    if (ADAPT_TYPE_AUD == byAdaptType)
    {
        //��Ƶ��ʱû�в������µĶ�̬�غ�����
        if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
            // zbq [10/29/2007] ��ӦFEC֧��������ʽ
            m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
        {
            tDAPayload.SetActivePayload(GetActivePayload(m_tConf, pSrcSCS->GetAudioMediaType()));
        }
        else
        {
            tDAPayload.SetActivePayload(pSrcSCS->GetAudioMediaType());
        }
        tDAPayload.SetRealPayLoad(pSrcSCS->GetAudioMediaType());
    }

    ptParam->SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());

    CServMsg cServMsg;							
    cServMsg.SetChnIndex(byChnIdx);
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8 *)ptParam, sizeof(TAdaptParam));
    cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //video
    cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload)); //dvideopaload
    cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //audio
    cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload)); //dApayload 

	// MCUǰ�����, zgc, 2007-09-28
	TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
	cServMsg.CatMsgBody((u8 *)&tCapSupportEx, sizeof(tCapSupportEx));	//FECType

    SendMsgToEqp(byEqpId, MCU_BAS_STARTADAPT_REQ, cServMsg);

    EqpLog("CMcuVcInst: BAS.%d Chn.%d is ready, and expropriate now!\n", byEqpId, byChnIdx);

    //���ö�ʱ
    SetTimer(MCUVC_WAITBASRRSP_TIMER+byAdaptType-1, 20*1000, byAdaptType);
    EqpLog("CMcuVcInst.%d Set timer %d\n", GetInsID(), MCUVC_WAITBASRRSP_TIMER+byAdaptType-1);

    return TRUE;		

}

/*=============================================================================
    �� �� ���� ChangeBasAdaptParam
    ��    �ܣ� ����������(Ŀǰû�жԼ�������֧��)
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const TAdaptParam* ptParam ���������ʲ���
			   BOOL32 bRTP                  �Ƿ�ΪRTP����
               TSimCapSet *pSrcSCS        ����ǰ��ԭʼ������������
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/3/9    3.5			����                  ����
=============================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::ChangeBasAdaptParam(TAdaptParam *ptParam, u8 byAdaptType, TSimCapSet *pSrcSCS)
{
	CServMsg cServMsg;

	TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
	TDoublePayload tDVPayload;
	TDoublePayload tDAPayload;
	u8 byChnIdx;
    u8 byEqpId;

    if (ADAPT_TYPE_AUD == byAdaptType)
    {
        byChnIdx = m_byAudBasChnnl;
        byEqpId = m_tAudBasEqp.GetEqpId();
    }
    else if (ADAPT_TYPE_VID == byAdaptType)
    {
        byChnIdx = m_byVidBasChnnl;
        byEqpId = m_tVidBasEqp.GetEqpId();
    }
    else if (ADAPT_TYPE_BR == byAdaptType)
    {
        byChnIdx = m_byBrBasChnnl;
        byEqpId = m_tBrBasEqp.GetEqpId();
    }
    else
    {
    }

    if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode())
    {
        ptParam->SetAudActiveType(GetActivePayload(m_tConf, ptParam->GetAudType()));
        ptParam->SetVidActiveType(GetActivePayload(m_tConf, ptParam->GetVidType()));
    }
    else
    {
        ptParam->SetAudActiveType(ptParam->GetAudType());
        ptParam->SetVidActiveType(ptParam->GetVidType());
    }

    if (NULL != pSrcSCS)
    {
        if (ADAPT_TYPE_AUD != byAdaptType)
        {
            if (MEDIA_TYPE_H264 == pSrcSCS->GetVideoMediaType() ||
                MEDIA_TYPE_H263PLUS == pSrcSCS->GetVideoMediaType() ||
                CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] ��ӦFEC֧��������ʽ
                m_tConf.GetCapSupportEx().IsVideoSupportFEC() )
            {
                tDVPayload.SetRealPayLoad(pSrcSCS->GetVideoMediaType());
                tDVPayload.SetActivePayload(GetActivePayload(m_tConf, pSrcSCS->GetVideoMediaType()));
            }
            else
            {
                tDVPayload.SetRealPayLoad(pSrcSCS->GetVideoMediaType());
                tDVPayload.SetActivePayload(pSrcSCS->GetVideoMediaType());
            }
        }

        if (ADAPT_TYPE_AUD == byAdaptType)
        {
            //��Ƶ��ʱû�в������µĶ�̬�غ�����
            if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] ��ӦFEC֧��������ʽ
                m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
            {
                tDAPayload.SetRealPayLoad(pSrcSCS->GetAudioMediaType());
                tDAPayload.SetActivePayload(GetActivePayload(m_tConf,  pSrcSCS->GetAudioMediaType()));
            }
            else
            {
                tDAPayload.SetRealPayLoad(pSrcSCS->GetAudioMediaType());
                tDAPayload.SetActivePayload(pSrcSCS->GetAudioMediaType());
            }
        }
    }
    else
    {
        if (ADAPT_TYPE_AUD != byAdaptType)
        {
            if (MEDIA_TYPE_H264 == ptParam->GetVidType() || 
                MEDIA_TYPE_H263PLUS == ptParam->GetVidType() || 
                CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] ��ӦFEC֧��������ʽ
                m_tConf.GetCapSupportEx().IsVideoSupportFEC())
            {
                tDVPayload.SetRealPayLoad(ptParam->GetVidType());
                tDVPayload.SetActivePayload(GetActivePayload(m_tConf, ptParam->GetVidType()));
            }
            else
            {
                tDVPayload.SetRealPayLoad(ptParam->GetVidType());
                tDVPayload.SetActivePayload(ptParam->GetVidType());
            }
        }

        if (ADAPT_TYPE_AUD == byAdaptType)
        {
            //��Ƶ��ʱû�в������µĶ�̬�غ�����
            if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] ��ӦFEC֧��������ʽ
                m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
            {
                tDAPayload.SetRealPayLoad(ptParam->GetAudType());
                tDAPayload.SetActivePayload(GetActivePayload(m_tConf, ptParam->GetAudType()));
            }
            else
            {
                tDAPayload.SetRealPayLoad(ptParam->GetAudType());
                tDAPayload.SetActivePayload(ptParam->GetAudType());
            }
        }
    }

    ptParam->SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());

    cServMsg.SetChnIndex(byChnIdx);
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8 *)ptParam, sizeof(TAdaptParam));
    cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //video
    cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload)); //dvideopaload
    cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //audio
    cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload)); //dApayload 

    SendMsgToEqp(byEqpId, MCU_BAS_SETADAPTPARAM_CMD, cServMsg);

    return TRUE;
}

/*====================================================================
    ������      : StopBasSwitch
    ����        ��ֹͣ��������������
    ����        ��
    ���� ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/22    3.0         zmy           Create
	04/03/20    3.0         ������          �޸�
====================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::StopBasSwitch(u8 byAdaptType)
{
    TEqp tBasEqp;
    u8 byBasChnnl;
    u8 byMode;

    if (ADAPT_TYPE_AUD == byAdaptType)
    {
        byBasChnnl = m_byAudBasChnnl;
        tBasEqp = m_tAudBasEqp;
        byMode = MODE_AUDIO;
    }
    else if (ADAPT_TYPE_VID == byAdaptType)
    {
        byBasChnnl = m_byVidBasChnnl;
        tBasEqp = m_tVidBasEqp;
        byMode = MODE_VIDEO;
    }
    else if (ADAPT_TYPE_BR == byAdaptType)
    {
        byBasChnnl = m_byBrBasChnnl;
        tBasEqp = m_tBrBasEqp;
        byMode = MODE_VIDEO;
    }
    else if (ADAPT_TYPE_CASDAUD == byAdaptType)
    {
        byBasChnnl = m_byCasdAudBasChnnl;
        tBasEqp = m_tCasdAudBasEqp;
        byMode = MODE_AUDIO;
    }
    else if (ADAPT_TYPE_CASDVID == byAdaptType)
    {
        byBasChnnl = m_byCasdVidBasChnnl;
        tBasEqp = m_tCasdVidBasEqp;
        byMode = MODE_VIDEO;
    }

    //����������Ľ�����
    g_cMpManager.RemoveSwitchBridge(tBasEqp, byBasChnnl, byMode);

    //�����������prs�Ľ���
    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
        if (ADAPT_TYPE_AUD == byAdaptType)
        {
            StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlAudBas, FALSE, MODE_AUDIO);
        }
        else if (ADAPT_TYPE_VID == byAdaptType)
        {
            StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlVidBas, FALSE, MODE_VIDEO);
        }
        else if (ADAPT_TYPE_BR == byAdaptType)
        {
            StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlBrBas, FALSE, MODE_VIDEO);
        }
    }

    //������������ն˵Ľ���
    StopSwitchToAllSubMtJoinedConfNeedAdapt(FALSE, byMode, byAdaptType);

    //������������Ľ���
    StopSwitchToPeriEqp(tBasEqp.GetEqpId(), byBasChnnl);

    return TRUE;
}

/*====================================================================
    ������      : StopBasAdapt
    ����        ��ֹͣ��������
    ����        ��
    ���� ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/22    3.0         zmy           Create
	04/02/25    3.0         ������         �޸�
====================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::StopBasAdapt(u8 byAdaptType)
{
    TEqp *ptBasEqp = NULL;
    u8 byBasChnnl = EQP_CHANNO_INVALID;
    u8 byBasMode = CONF_BASMODE_NONE;

    if (ADAPT_TYPE_AUD == byAdaptType)
    {
        byBasMode = CONF_BASMODE_AUD;
        byBasChnnl = m_byAudBasChnnl;
        ptBasEqp = &m_tAudBasEqp;

        //ֹͣ��Ҫ��Ƶ�����prs,�󶨴���        
        if(m_tConfInStatus.IsPrsChnlAudBasStart())
        {
            StopPrs(PRSCHANMODE_AUDBAS);
        }        
    }
    else if (ADAPT_TYPE_VID == byAdaptType)
    {
        byBasMode = CONF_BASMODE_VID;
        byBasChnnl = m_byVidBasChnnl;
        ptBasEqp = &m_tVidBasEqp;

        if(m_tConfInStatus.IsPrsChnlVidBasStart())
        {
            StopPrs(PRSCHANMODE_VIDBAS);
        }    
    }
    else if (ADAPT_TYPE_BR == byAdaptType)
    {
        byBasMode = CONF_BASMODE_BR;
        byBasChnnl = m_byBrBasChnnl;
        ptBasEqp = &m_tBrBasEqp;

        if(m_tConfInStatus.IsPrsChnlBrBasStart())
        {
            StopPrs(PRSCHANMODE_BRBAS);
        }    
    }
    else if (ADAPT_TYPE_CASDAUD == byAdaptType)
    {
        byBasMode = CONF_BASMODE_CASDAUD;
        byBasChnnl = m_byCasdAudBasChnnl;
        ptBasEqp = &m_tCasdAudBasEqp;
    }
    else if (ADAPT_TYPE_CASDVID == byAdaptType)
    {
        byBasMode = CONF_BASMODE_CASDVID;
        byBasChnnl = m_byCasdVidBasChnnl;
        ptBasEqp = &m_tCasdVidBasEqp;
    }

    if (EQP_CHANNO_INVALID == byBasChnnl || NULL == ptBasEqp)
    {
        ConfLog(FALSE, "[StopBasAdapt] invalid byBasChnnl.%d or byAdaptType.%d\n", byBasChnnl, byAdaptType);
        return FALSE;
    }

    CServMsg cServMsg;
    cServMsg.SetChnIndex(byBasChnnl);
    cServMsg.SetConfId(m_tConf.GetConfId());
    TPeriEqpStatus tStatus;

    //�������������
    StopBasSwitch(byAdaptType);

    //������������ֹͣ��Ϣ
    SendMsgToEqp(ptBasEqp->GetEqpId(), MCU_BAS_STOPADAPT_REQ, cServMsg);

    //change info
    //u8 byBasMode = byAdaptType;
    m_tConf.m_tStatus.SetAdaptMode(byBasMode, FALSE);
	// zgc, 2008-01-22, �ϱ�ģʽ
	ConfModeChange();

    //�ͷ�ͨ��
    if (!g_cMcuVcApp.GetPeriEqpStatus(ptBasEqp->GetEqpId(), &tStatus))
    {
        return FALSE;
    }

    tStatus.m_tStatus.tBas.tChnnl[byBasChnnl].SetStatus(TBasChnStatus::READY);
    g_cMcuVcApp.SetPeriEqpStatus(ptBasEqp->GetEqpId(), &tStatus);

    return TRUE;
}

/*====================================================================
    ������      : ProcBasStartupTimeout
    ����        ��������������Ӧ��ʱ��Ϣ������
    ����        ��
    ���� ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/24    3.0         zmy           Create
====================================================================*/
//modify bas 2
void CMcuVcInst::ProcBasStartupTimeout(const CMessage *pcMsg)
{
    TPeriEqpStatus tStatus;
    u8 byAdaptType = *(u8 *)pcMsg->content;

    u8 byBasChnnl = EQP_CHANNO_INVALID;
    TEqp *ptBasEqp = NULL;

    switch(byAdaptType)
    {
    case ADAPT_TYPE_AUD:
        byBasChnnl = m_byAudBasChnnl;
        ptBasEqp = &m_tAudBasEqp;
    	break;

    case ADAPT_TYPE_VID:
        byBasChnnl = m_byVidBasChnnl;
        ptBasEqp = &m_tVidBasEqp;
    	break;

    case ADAPT_TYPE_BR:
        byBasChnnl = m_byBrBasChnnl;
        ptBasEqp = &m_tBrBasEqp;
        break;

    case ADAPT_TYPE_CASDAUD:
        byBasChnnl = m_byCasdAudBasChnnl;
        ptBasEqp = &m_tCasdAudBasEqp;
        break;

    case ADAPT_TYPE_CASDVID:
        byBasChnnl = m_byCasdVidBasChnnl;
        ptBasEqp = &m_tCasdVidBasEqp;
        break;

    default:
        ConfLog(FALSE, "[ProcBasStartupTimeout] invalid byAdaptType.%d\n", byAdaptType);
        return;
        break;
    }  

    if (EQP_CHANNO_INVALID == byBasChnnl || ptBasEqp->IsNull())
    {
        ConfLog(FALSE, "[ProcBasStartupTimeout] invalid byBasChnnl.%d\n", byBasChnnl);
        return;
    }

    //�ͷ�ͨ��
    if (!g_cMcuVcApp.GetPeriEqpStatus(ptBasEqp->GetEqpId(), &tStatus))
    {
        //ptBasEqp->SetNull();
        return;
    }

    tStatus.m_tStatus.tBas.tChnnl[byBasChnnl].SetStatus(TBasChnStatus::READY);
    g_cMcuVcApp.SetPeriEqpStatus(ptBasEqp->GetEqpId(), &tStatus);

    //�ı�����״̬
    m_tConf.m_tStatus.SetAdaptMode(byAdaptType, FALSE);
	// zgc, 2008-01-22, �ϱ�ģʽ
	ConfModeChange();

    //����ֹͣ��Ϣ����ֹӦ����Ϣ�����ڶ�����
    CServMsg cServMsg;
    cServMsg.SetChnIndex(byBasChnnl);
    cServMsg.SetConfId(m_tConf.GetConfId());
    SendMsgToEqp(ptBasEqp->GetEqpId(), MCU_BAS_STOPADAPT_REQ, cServMsg);

    //ptBasEqp->SetNull();

    return;
}

/*=============================================================================
    �� �� ���� ProcHduMcuNeedIFrameCmd
    ��    �ܣ� 
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/3/9    4.6		    ���ֱ�                  ����
=============================================================================*/
void CMcuVcInst::ProcHduMcuNeedIFrameCmd(const CMessage *pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TMt tSrcMt;

	TPeriEqpStatus tHduInfo;

    switch(CurState())
	{
    case STATE_ONGOING:	
	    switch( pcMsg->event ) 
		{			
	    case HDU_MCU_NEEDIFRAME_CMD:
		{
			u8 byChnlIdx = cServMsg.GetChnIndex();
			g_cMcuVcApp.GetPeriEqpStatus(cServMsg.GetSrcSsnId(), &tHduInfo);
            tSrcMt = (TMt)tHduInfo.m_tStatus.tHdu.atVideoMt[byChnlIdx];
		
			TLogicalChannel tLogicChn;
			if (tSrcMt.IsNull() ||
				(MT_TYPE_MT == tSrcMt.GetMtType() &&
				!m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicChn, FALSE)))
			{ 
				CallLog("[ProcHduMcuNeedIFrameCmd] tMt/tEqp.%d unexist or logicChn unexist!\n", tSrcMt.GetMtId());
			}
			else
			{
				if (TYPE_MT == tSrcMt.GetType())
				{
				    tSrcMt = m_ptMtTable->GetMt(tSrcMt.GetMtId());
					NotifyFastUpdate(tSrcMt, MODE_VIDEO);
				}
				else if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_VMP == tSrcMt.GetEqpType())
				{
					u8 bySrcChnnl = 0;
					u8 byMtMediaType = 0;
					u8 byMtRes = 0;			

					byMtMediaType = m_tConf.GetMainVideoMediaType();

					byMtRes = m_tConf.GetMainVideoFormat();

		 			bySrcChnnl = GetVmpOutChnnlByRes(byMtRes, m_tVmpEqp.GetEqpId(), byMtMediaType);
                    
					EqpLog("[ProcHduMcuNeedIFrameCmd]Req iframe to eqp(eqpid:%d chnlidx:%d)\n", 
						
						   tSrcMt.GetEqpId(), bySrcChnnl);

					NotifyEqpFastUpdate(tSrcMt,bySrcChnnl);
				} 
				else
				{
					EqpLog("[ProcHduMcuNeedIFrameCmd]src(mcuid:%d mtid:%d type:%d) in hdu channel%d has wrong type\n",
						   tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSrcMt.GetType(), byChnlIdx);
				}
			}
		}
		    break;
	    default:
		    ConfLog(FALSE, "[ProcHduMcuNeedIFrameCmd]:The Message type %u(%s) is not fit", pcMsg->event, ::OspEventDesc(pcMsg->event) );
		    break;
		}
	    break;
    default:
        ConfLog(FALSE, "[ProcHduMcuNeedIFrameCmd]Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
	}
	return;
}

/*=============================================================================
    �� �� ���� ProcBasMcuCommand
    ��    �ܣ� 
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/9/18   4.0		    �ܹ��                  ����
=============================================================================*/
void CMcuVcInst::ProcBasMcuCommand(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TMt tSrcMt;

    switch(CurState())
    {
    case STATE_ONGOING:

        switch( pcMsg->event ) 
        {			
        case BAS_MCU_NEEDIFRAME_CMD:

            if ( !IsHDConf(m_tConf) )
            {
                if (m_tConf.HasSpeaker())
                {
                    tSrcMt = GetLocalSpeaker();
                    
                    if (cServMsg.GetChnIndex() != m_byVidBasChnnl &&
                        cServMsg.GetChnIndex() != m_byBrBasChnnl &&
                        cServMsg.GetChnIndex() != m_byCasdVidBasChnnl )
                    {
                        ConfLog(FALSE, "[ProcBasMcuCommand]cServMsg.GetChnIndex()[%d] != m_byAnyBasChnnl\n",
                            cServMsg.GetChnIndex());
                        break;
                    }
                    
                    NotifyFastUpdate(tSrcMt, MODE_VIDEO);
                }
                else
                {
                    EqpLog("[ProcBasMcuCommand] Err: No speaker for VCU from eqp.%d\n", cServMsg.GetSrcSsnId());
                }
            }
            else
            {
				if (VCS_CONF == m_tConf.GetConfSource())
				{
					NotifyFastUpdate(m_tConf.GetChairman(), MODE_VIDEO, TRUE);
					NotifyFastUpdate(m_cVCSConfStatus.GetCurVCMT(), MODE_VIDEO, TRUE);
					return;
				}
                u8 byEqpId = cServMsg.GetSrcSsnId();
                u8 byChnIdx = cServMsg.GetChnIndex();
                EqpLog( "[ProcBasMcuCommand] Receive bas.%d<chn.%d> BAS_MCU_NEEDIFRAME_CMD!\n", byEqpId, byChnIdx );

                if ( !g_cMcuAgent.IsEqpBasHD(byEqpId) )
                {
                    ConfLog( FALSE, "[ProcBasMcuCommand] The bas.%d is common bas, the conf.%s is HD conf, It's impossible!\n",
                                byEqpId, m_tConf.GetConfName() );
                    break;
                }

                TEqp tEqp = g_cMcuVcApp.GetEqp(byEqpId);
                u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnIdx);

                if ( CHN_ADPMODE_MVBRD == byChnMode )
                {
                    if ( m_tConf.HasSpeaker() )
                    {
                        tSrcMt = GetLocalSpeaker();                      
                        NotifyFastUpdate(tSrcMt, MODE_VIDEO);
                    }
                    else
                    {
                        EqpLog("[ProcBasMcuCommand] Err: No speaker for VCU from eqp.%d\n", cServMsg.GetSrcSsnId());
                    }
                    break;
                }
                else if ( CHN_ADPMODE_DSBRD == byChnMode)
                {
                    if ( !m_tDoubleStreamSrc.IsNull() )
                    {
                        tSrcMt = m_tDoubleStreamSrc;
                        NotifyFastUpdate(tSrcMt, MODE_SECVIDEO);
                    }
                    else
                    {
                        EqpLog("[ProcBasMcuCommand] Err: No ds src for VCU from eqp.%d\n", cServMsg.GetSrcSsnId());
                    }
                    break;
                }
                else if ( CHN_ADPMODE_MVSEL == byChnMode )
                {
                    //��m_cBasMgr��ȡ��Ӧ��ѡ��Դ������Ӧ�Ĺؼ�֡����
                }
                else if ( CHN_ADPMODE_DSSEL == byChnMode )
                {
                    //��m_cBasMgr��ȡ��Ӧ��ѡ��Դ������Ӧ�Ĺؼ�֡����
                }
            }
            break;

        default:
            break;
        }
        break;

    default:
        ConfLog(FALSE, "[ProcBasMcuCommand]Wrong message %u(%s) received in state %u!\n", 
                        pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
    }
}

/*===========================================================================
�� �� ���� ProcHduMcuChnnlStatusNotif
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
-----------------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/02/26  4.6.1		���ֱ�                  ����

=============================================================================*/
void CMcuVcInst::ProcHduMcuChnnlStatusNotif( const CMessage * pcMsg )
{
	if (STATE_ONGOING != CurState())
    {
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

	THduChnStatus tHduChnStatus = *(THduChnStatus*)cServMsg.GetMsgBody();
	
    TPeriEqpStatus tHduStatus;
	
    THduSwitchInfo tHduSwitchInfo;
    u8 byMemberType;
    u8 bySrcMtConfIdx;
    u8 byChnlIdx = tHduChnStatus.GetChnIdx();
    TEqp tEqp;
	
    tEqp.SetMcuEqp(tHduChnStatus.GetEqp().GetMcuId(), tHduChnStatus.GetEqpId(), tHduChnStatus.GetEqp().GetEqpType() );
	
    g_cMcuVcApp.GetPeriEqpStatus(tHduChnStatus.GetEqpId(), &tHduStatus);
	
	if ( THduChnStatus::eREADY ==  tHduChnStatus.GetStatus() )
	{
		bySrcMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx();
		if (m_byConfIdx == bySrcMtConfIdx)
		{
			byMemberType = tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType;
			if (0 != byMemberType)
			{
				cServMsg.SetMsgBody( (u8*)&tHduStatus, sizeof(tHduStatus) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );

				tHduSwitchInfo.SetMemberType(byMemberType);
				tHduSwitchInfo.SetSrcMt((TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx]);
				tHduSwitchInfo.SetDstMt(tEqp);
				tHduSwitchInfo.SetDstChlIdx(byChnlIdx);
				
				::OspPost(MAKEIID(AID_MCU_VC, GetInsID()), MCUVC_STARTSWITCHHDU_NOTIFY, 
					(u8*)&tHduSwitchInfo, sizeof(tHduSwitchInfo));
			}
		}

	}
	
	cServMsg.SetMsgBody( (u8*)&tHduStatus, sizeof( tHduStatus ) );
	SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );
	
    return;
}

/*------------------------------------------------------------------*/
/*                                 mau                              */
/*------------------------------------------------------------------*/


/*=============================================================================
    �� �� ���� ProcHdBasChnnlStatusNotif
    ��    �ܣ� ����BASͨ��״̬֪ͨ���鴦����
    �㷨ʵ�֣� �����ڸ���BAS����ʱ���������������ʱ���ϵ����⣬����������ʧ��, ��˵������յ�ͨ��״̬ʱ����
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/9/3    4.0		    �ܹ��                  ����
    2008/11/19  4.5         �ű���                  ��������༯�ɵ�����H263+�������
=============================================================================*/
void CMcuVcInst::ProcHdBasChnnlStatusNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    u8 *pbyMsgBody  = (u8 *)cServMsg.GetMsgBody();
    u8  byChnType = *pbyMsgBody;
    pbyMsgBody += sizeof(u8);

    //ֻȡ����
    THDBasChnStatus tChnStatus;
    memcpy( &tChnStatus, pbyMsgBody, sizeof(THDBasChnStatus) );
    if ( tChnStatus.GetStatus() != THDBasChnStatus::READY )
    {
        return;
    }

    EqpLog( "[ProcHdBasChnnlStatusNotif] EqpId.%d, ChnIdx.%d, ChnType.%d, Status.%d\n",
             tChnStatus.GetEqpId(), 
             tChnStatus.GetChnIdx(), 
             byChnType,
             tChnStatus.GetStatus() );

    u8 byEqpId = tChnStatus.GetEqpId();
    u8 byChnIdx = tChnStatus.GetChnIdx();

    BOOL32 bIsReserved = FALSE;

    TPeriEqpStatus tEqpStatus;
    g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tEqpStatus);

    //��ȡ����״̬
    THDBasVidChnStatus *ptChnStatus = NULL;
    u8 byBasType = tEqpStatus.m_tStatus.tHdBas.GetEqpType();
    if (TYPE_MPU == byBasType)
    {
        ptChnStatus = tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx);
    }
    else if (TYPE_MAU_NORMAL == byBasType ||
             TYPE_MAU_H263PLUS == byBasType )
    {
        BOOL32 bChnMV = 0 == byChnIdx ? TRUE : FALSE;
        if (bChnMV)
        {
            ptChnStatus = tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus();
        }
        else
        {
            ptChnStatus = tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus();
        }
    }
    
    if (NULL == ptChnStatus)
    {
        ConfLog(FALSE, "[ProcHdBasChnnlStatusNotif] ptChnStatus.%d.IsNull, check it!\n", byChnIdx);
        return;
    }
    
    bIsReserved = ptChnStatus->GetReservedType();
    if (!bIsReserved)
    {
        EqpLog("[ProcHdBasChnnlStatusNotif] Eqp<%d,%d> isn't reserved, ignore it\n", byEqpId, byChnIdx);
        return;
    }

    u8 byMediaMode = MODE_NONE;
    u8 bySwitchMode = SWITCH_MODE_NONE;
    u8 byChnMode = m_cBasMgr.GetChnMode(tChnStatus.GetEqp(), byChnIdx);
    if(!CheckHdBasChnMode(byChnMode, byMediaMode, bySwitchMode))
    {
        EqpLog("[ProcHdBasChnnlStatusNotif] HdBas<%d, %d> isn't reseverd for conf.%d!\n", 
            byEqpId, byChnIdx, m_byConfIdx);
        return;
    }

    switch (byChnMode)
    {
    case CHN_ADPMODE_MVBRD:

        if (!m_tConf.m_tStatus.IsHdVidAdapting())
        {
            ConfLog(FALSE, "[ProcHdBasChnnlStatusNotif] HdBas<%d,%d> should be \
restore mv but not due to status changed!\n", byEqpId, byChnIdx);
            return;
        }
        ChangeHDAdapt(tChnStatus.GetEqp(), byChnIdx, TRUE);
        break;

    case CHN_ADPMODE_DSBRD:

        if (!m_tConf.m_tStatus.IsHDDoubleVidAdapting())
        {
            ConfLog(FALSE, "[ProcHdBasChnnlStatusNotif] HdBas<%d,%d> should be \
restore ds but not due to status changed!\n", byEqpId, byChnIdx);
            return;
        }
        ChangeHDAdapt(tChnStatus.GetEqp(), byChnIdx, TRUE);
        break;

    case CHN_ADPMODE_MVSEL:
    case CHN_ADPMODE_DSSEL:
        ChangeHDAdapt(tChnStatus.GetEqp(), byChnIdx, TRUE);
    	break;

    default:
        ConfLog(FALSE, "[ProcHdBasChnnlStatusNotif] unexpected type.%d\n", byChnMode);
        break;
    }

    return;
}

/*=============================================================================
�� �� ���� ProcHdBasMcuRsp
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/8/7    4.0			�ܹ��                  ����
2008/11/20  4.5         �ű���                  ����basȺ�鼯��
2009/04/19  4.6         �ű���                  ��д������mpu��ѡ������
=============================================================================*/
void CMcuVcInst::ProcHdBasMcuRsp( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    cServMsg.SetEventId( pcMsg->event );
    
    u8 byChnIdx = cServMsg.GetChnIndex();
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();

    EqpLog( "[ProcHdBasMcuRsp] Receive bas.%d(chn.%d) rsp<msg.%s>!\n", 
             tEqp.GetEqpId(), byChnIdx, ::OspEventDesc( pcMsg->event));

    u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnIdx);
    if (CHN_ADPMODE_NONE == byChnMode)
    {
        ConfLog(FALSE, "[ProcHdBasMcuRsp] HdBas<%d,%d> unexist, check it\n", tEqp.GetEqpId(), byChnIdx);
        return;
    }
    switch (byChnMode)
    {
    case CHN_ADPMODE_MVBRD:
    case CHN_ADPMODE_DSBRD:
        ProcHdBasMcuBrdAdpRsp( cServMsg );
        break;

    case CHN_ADPMODE_MVSEL:
    case CHN_ADPMODE_DSSEL:
        ProcHdBasMcuSelRsp( cServMsg );
    	break;
    default:
        //FIXME: ѡ������δ������ɣ���ʱ����
        ProcHdBasMcuSelRsp( cServMsg );
        break;        
    }
    return;
}

/*=============================================================================
�� �� ���� ProcHdBasMcuBrdAdpRsp
��    �ܣ� �㲥����ͨ�������ɹ���Ӧ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CServMsg &cServMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/8/21   4.0			�ܹ��                  ����
2008/11/20  4.5         �ű���                  ����basȺ�鼯��
2009/06/05  4.5         ���㻪                  ��������˫��ͨ�����óɹ�ͳһ��Ӧ
=============================================================================*/
void CMcuVcInst::ProcHdBasMcuBrdAdpRsp( CServMsg &cServMsg )
{   
    u8 byChlIdx = cServMsg.GetChnIndex();
    TEqp tEqp = *(TEqp*)cServMsg.GetMsgBody();
    
    tEqp.SetConfIdx(m_byConfIdx);
    u32 dwTimerId = MCUVC_WAITHDVIDBASRSP_TIMER + m_cBasMgr.GetChnId(tEqp, byChlIdx);
    
    //ȡ��������Ӧ��ʱ
    KillTimer(dwTimerId);
    EqpLog("[HDBAS_MCU_STARTADAPT_ACK] KillTimer(%d)\n", dwTimerId);

    switch (cServMsg.GetEventId())
    {
    case HDBAS_MCU_STARTADAPT_ACK:
        {
			THDBasVidChnStatus tStatus;
			if (!m_cBasMgr.GetChnStatus(tEqp, byChlIdx, tStatus))
			{
				ConfLog(FALSE, "[ProcHdBasMcuMVRsp] get eqp<%d, %d> status failed\n", 
														  tEqp.GetEqpId(), byChlIdx);
				return;
			}

			//����ͨ��״̬Ϊrunning      
			tStatus.SetStatus(THDBasChnStatus::RUNING);
			if (!m_cBasMgr.UpdateChn(tEqp, byChlIdx, tStatus))
			{
				ConfLog(FALSE, "[ProcHdBasMcuMVRsp] UpdateMVChn<%d, %d> failed\n", 
													   tEqp.GetEqpId(), byChlIdx);
				return;
			}
			g_cMcuVcApp.SendPeriEqpStatusToMcs(tEqp.GetEqpId());

			//������ؽ���
			StartHdBasSwitch(tEqp, byChlIdx);
        }
        break;

    case HDBAS_MCU_STARTADAPT_NACK:
		{
			//zbq[11/20/2008] ������鿪�����ٱ�������ǽ��
			/*
			//�ı��������״̬
			m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_VID, FALSE);
			ConfModeChange();*/
			u8 byChnlMode = m_cBasMgr.GetChnMode(tEqp, byChlIdx); 
			if (byChnlMode != CHN_ADPMODE_NONE)
			{
				TPeriEqpStatus tHDBasStatus;
				g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tHDBasStatus);
				THDBasVidChnStatus * ptChanStatus = NULL;
				if (CHN_ADPMODE_MVBRD == byChnlMode)
				{
					ptChanStatus = tHDBasStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(byChlIdx);
				}
				else if (CHN_ADPMODE_DSBRD == byChnlMode)
				{
					ptChanStatus = tHDBasStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(byChlIdx);
				}

				if ( NULL != ptChanStatus )
				{
					TEqp tHDBas;
					tHDBas = ptChanStatus->GetEqp();
					tHDBas.SetConfIdx(0);
					ptChanStatus->SetEqp(tHDBas);
					g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tHDBasStatus);
					g_cMcuVcApp.SendPeriEqpStatusToMcs(tEqp.GetEqpId());
				}
			}

		}
        break;

    default:
        break;
    }
}

/*=============================================================================
    �� �� ���� ProcHdBasMcuSelRsp
    ��    �ܣ� ����ѡ������ͨ����Ϣ��Ӧ����������ѡ����˫��ѡ�������汾ֻ֧�ּ������䣩
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CServMsg &cServMsg
    �� �� ֵ�� void 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/03/26  4.5         �ű���                  ����
    2009/04/19  4.6         �ű���                  ������ͨ��ѡ�������߼�
=============================================================================*/
void CMcuVcInst::ProcHdBasMcuSelRsp(CServMsg &cServMsg)
{
    u8 byChlIdx = cServMsg.GetChnIndex();
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();
    tEqp.SetConfIdx(m_byConfIdx);

    switch (cServMsg.GetEventId())
    {
    case HDBAS_MCU_STARTADAPT_ACK:
        {
            if (tEqp.GetEqpId() != m_tCasdVidBasEqp.GetEqpId())
            {
                ConfLog(FALSE, "[ProcHdBasMcuSelRsp] Eqp.%d doesn't adp for cascade EQP.%d\n", 
                    tEqp.GetEqpId(), m_tCasdVidBasEqp.GetEqpId());
                return;
            }
            
            if( m_tCascadeMMCU.IsNull() )
            {
                 ConfLog(FALSE, "[ProcHdBasMcuSelRsp] m_tCascadeMMCU.IsNull()!\n");
                 return;
            }

            //ͼ�񽻻�������������
            TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
            if( ptConfMcInfo == NULL || ptConfMcInfo->m_tSpyMt.IsNull() )
            {
                ConfLog(FALSE, "[ProcHdBasMcuSelRsp] ptConfMcInfo.0x%x!\n");
                return;
            }

            TMt tAdaptSrc = ptConfMcInfo->m_tSpyMt;
            u8 bySrcChnnl = (tAdaptSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
            StartSwitchToPeriEqp(tAdaptSrc, bySrcChnnl, 
                                 tEqp.GetEqpId(), byChlIdx, 
                                 MODE_VIDEO, SWITCH_MODE_SELECT, TRUE);
            
            //zbq[05/07/2009]�����ϼ�MCU��������ȷ���ش���������
            u8 byOutIdx = 0;
            TSimCapSet tDstSimCap = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());

            if (MEDIA_TYPE_H264 != tDstSimCap.GetVideoMediaType() ||
                (MEDIA_TYPE_H264 == tDstSimCap.GetVideoMediaType() &&
                 VIDEO_FORMAT_CIF == tDstSimCap.GetVideoResolution()))
            {
                byOutIdx = 1;
            }

            //�����������Ľ�����
            u8 byBasOutputChn = byChlIdx * MAXNUM_VOUTPUT;
            
            BOOL32 bChn = g_cMpManager.SetSwitchBridge(tEqp, byBasOutputChn+byOutIdx, MODE_VIDEO, TRUE);
            
            if ( !bChn )
            {
                ConfLog( FALSE, "[ProcHdBasMcuDualRsp] SetSwitchBridge chn.%d failed!\n", byBasOutputChn );
            }
            
            //��������������prs
            if (m_tConf.GetConfAttrb().IsResendLosePack())
            {
                // FIXME���������䶪���ش�δ���
            }

            //���������������ϼ�
            StartSwitchToSubMt(tEqp, byBasOutputChn + byOutIdx, m_tCascadeMMCU.GetMtId(), MODE_VIDEO);
            
            NotifyEqpFastUpdate(tEqp, m_byCasdVidBasChnnl, TRUE);
        }
        break;
        
    case HDBAS_MCU_STARTADAPT_NACK:
        /*
        KillTimer(dwTimerId);
        EqpLog("[HDBAS_MCU_STARTADAPT_ACK]KillTimer(%d)\n", dwTimerId);
        */
        //�ı��������״̬
        m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_CASDVID, FALSE);
        ConfModeChange();
        
        break;
        
    default:
        break;
    }

    return;
}


/*=============================================================================
    �� �� ���� StartHDMVBrdAdapt
    ��    �ܣ� ��������㲥����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/11/18  4.5		    �ű���                  ����
    2009/04/17  4.6         �ű���                  �������
=============================================================================*/
BOOL32 CMcuVcInst::StartHDMVBrdAdapt()
{
    //0.��������Դ
    RefreshBasParam4MVBrd();

    //1.΢���������
    RefreshBasParam4AllMt();

    //2.�������
    TBasChn atBasChn[MAXNUM_CONF_MVCHN];
    u8 byNum = 0;
    m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_MVBRD);

    u8 byIdx = 0;
    for(; byIdx < byNum; byIdx ++)
    {
        ChangeHDAdapt(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId(), TRUE);
    }

    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_VID, TRUE);
    ConfModeChange();

    return TRUE;
}

/*=============================================================================
    �� �� ���� StopHDMVBrdAdapt
    ��    �ܣ� ֹͣ����㲥����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/04/17  4.6		    �ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::StopHDMVBrdAdapt()
{
    if (!m_tConf.m_tStatus.IsHdVidAdapting())
    {
        return FALSE;
    }

    TBasChn atBasChn[MAXNUM_CONF_MVCHN];
    u8 byNum = 0;
    m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_MVBRD);
    
    u8 byIdx = 0;
    for(; byIdx < byNum; byIdx ++)
    {
        StopHDAdapt(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId());
    }
    
    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_VID, TRUE);
    ConfModeChange();
    
    return TRUE;
}

/*=============================================================================
    �� �� ���� StartHDDSBrdAdapt
    ��    �ܣ� ��������˫���㲥����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/04/17  4.6		    �ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::StartHDDSBrdAdapt()
{
    //0.������������
    RefreshBasParam4DSBrd();

    //1.΢���������
    RefreshBasParam4AllMt(FALSE, TRUE);
    
    //2.�������
    TBasChn atBasChn[MAXNUM_CONF_MVCHN];
    u8 byNum = 0;
    m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_DSBRD);
    
    u8 byIdx = 0;
    for(; byIdx < byNum; byIdx ++)
    {
        ChangeHDAdapt(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId(), TRUE);
    }
    
    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_DOUBLEVID, TRUE);
    ConfModeChange();
    
    return TRUE;
}

/*=============================================================================
    �� �� ���� StopHDDSBrdAdapt
    ��    �ܣ� ֹͣ����˫���㲥����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/04/17  4.6		    �ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::StopHDDSBrdAdapt()
{
    if (!m_tConf.m_tStatus.IsHDDoubleVidAdapting())
    {
        return FALSE;
    }
    
    TBasChn atBasChn[MAXNUM_CONF_MVCHN];
    u8 byNum = 0;
    m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_DSBRD);
    
    u8 byIdx = 0;
    for(; byIdx < byNum; byIdx ++)
    {
        StopHDAdapt(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId());
    }
    
    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_DOUBLEVID, TRUE);
    ConfModeChange();
    
    return TRUE;
}

/*=============================================================================
    �� �� ���� ChangeHDAdapt
    ��    �ܣ� 
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/11/18  4.5		    �ű���                  ����
    2009/04/17  4.6         �ű���                  �޸�
=============================================================================*/
BOOL32 CMcuVcInst::ChangeHDAdapt(const TEqp &tEqp, u8 byChnId, BOOL32 bStart)
{
    CServMsg cServMsg;
    cServMsg.SetMsgBody();

    u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnId);
    
    //ѡ�����䶯̬��ȡ�͵�������Ҫ��̬����ͨ��״̬���㲥�����������忪��ˢ�£��򵥸�����
    BOOL32 bRet = TRUE;
    if (CHN_ADPMODE_MVSEL == byChnMode)
    {
        bRet = RefreshBasMVSelAdpParam(tEqp, byChnId);
    }
    else if (CHN_ADPMODE_DSSEL == byChnMode)
    {
        bRet = RefreshBasDSSelAdpParam(tEqp, byChnId);
    }
    if (!bRet)
    {
        ConfLog(FALSE, "[ChangeHDAdapt] tEqp<%d, %d> refresh ds sel param failed\n", 
            tEqp.GetEqpId(), byChnId);
        return FALSE;
    }

    
    THDBasVidChnStatus tVidChn;
    if (!m_cBasMgr.GetChnStatus(tEqp, byChnId, tVidChn))
    {
        return FALSE;
    }
    
    THDAdaptParam tAdaptParam;
    
    u8 byOutIdx = 0;
    for (; byOutIdx < MAXNUM_VOUTPUT; byOutIdx ++)
    {
        tAdaptParam.Reset();
        if (!tVidChn.GetOutputVidParam(byOutIdx)->IsNull())
        {
            memcpy(&tAdaptParam, tVidChn.GetOutputVidParam(byOutIdx), sizeof(tAdaptParam));
        }
        else
        {
            //����adpparam������baphd: ˫�������߼���Ȼ
            tAdaptParam.SetVidType(MEDIA_TYPE_H264);
            tAdaptParam.SetVidActiveType(GetActivePayload(m_tConf, MEDIA_TYPE_H264));
            tAdaptParam.SetBitRate(m_tConf.GetBitRate());
            tAdaptParam.SetFrameRate(m_tConf.GetMainVidUsrDefFPS());
            tAdaptParam.SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());

            //����Ⱥ�鲻��Ҫ ��1·���䣬��ʱдΪ4CIF/H264
            if (0 == byOutIdx)
            {
                u16 wHeight = 0;
                u16 wWidth = 0;
                GetWHByRes(VIDEO_FORMAT_4CIF, wWidth, wHeight);
                tAdaptParam.SetResolution(wWidth, wHeight);
            }
            //����Ⱥ�鲻��Ҫ ��2·���䣬��ʱдΪCIF/H264
            else
            {
                u16 wHeight = 0;
                u16 wWidth = 0;
                GetWHByRes(VIDEO_FORMAT_CIF, wWidth, wHeight);
                tAdaptParam.SetResolution(wWidth, wHeight);
            }
        }
        cServMsg.CatMsgBody((u8*)&tAdaptParam, sizeof(tAdaptParam));
    }
    
    TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
    cServMsg.CatMsgBody( (u8*)&tEncrypt, sizeof(tEncrypt) );
    
    TSimCapSet tSrcSCS;
    TDStreamCap tDSim;
    
    u8 byBasDecPT = MEDIA_TYPE_NULL;
    u8 byBasAudDecPT = MEDIA_TYPE_NULL;
    
    switch (byChnMode)
    {
    case CHN_ADPMODE_MVBRD:
        GetMVBrdSrcSim(tSrcSCS);
        byBasDecPT = tSrcSCS.GetVideoMediaType();
        byBasAudDecPT = tSrcSCS.GetAudioMediaType();
        break;
    case CHN_ADPMODE_DSBRD:
        GetDSBrdSrcSim(tDSim);
        byBasDecPT = tDSim.GetMediaType();
        break;
    case CHN_ADPMODE_MVSEL:
        GetSelSrcSim(tEqp, byChnId, tSrcSCS);
        byBasDecPT = tSrcSCS.GetVideoMediaType();
        byBasAudDecPT = tSrcSCS.GetAudioMediaType();
        break;
    case CHN_ADPMODE_DSSEL:
        GetSelSrcSim(tEqp, byChnId, tDSim);
        byBasDecPT = tDSim.GetMediaType();
        break;
    }
    
    if (MEDIA_TYPE_NULL == byBasDecPT)
    {
        ConfLog(FALSE, "[ChangeHDAdapt] get byBasDecPT.%d failed!\n", byBasDecPT);
        return FALSE;
    }
    
    TDoublePayload tDVPayload;
    if (MEDIA_TYPE_H264 == byBasDecPT || 
        MEDIA_TYPE_H263PLUS == byBasDecPT || 
        CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() )
    {
        tDVPayload.SetActivePayload(GetActivePayload(m_tConf, byBasDecPT));
    }
    else
    {
        tDVPayload.SetActivePayload(byBasDecPT);
    }
    tDVPayload.SetRealPayLoad(byBasDecPT); 
    cServMsg.CatMsgBody( (u8*)&tDVPayload, sizeof(tDVPayload) );
    cServMsg.SetChnIndex( byChnId );

    if (MEDIA_TYPE_NULL != byBasAudDecPT)
    {
        cServMsg.CatMsgBody((u8*)&byBasAudDecPT, sizeof(u8));
    }
    
    if (bStart)
    {
        SendMsgToEqp( tEqp.GetEqpId(), MCU_HDBAS_STARTADAPT_REQ, cServMsg );
        EqpLog("[StartHDAdapt] BAS.%d Chn.%d: MCU_HDBAS_STARTADAPT_REQ!\n", 
                tEqp.GetEqpId(), byChnId);
        
        //���ö�ʱ
        u8 byChnIdx = m_cBasMgr.GetChnId(tEqp, byChnId);
        SetTimer(MCUVC_WAITHDVIDBASRSP_TIMER + byChnIdx, 20*1000);
        
        EqpLog("[StartHDAdapt] CMcuVcInst.%d Set timer %d\n", 
                GetInsID(), MCUVC_WAITHDVIDBASRSP_TIMER+byChnIdx);
    }
    else
    {
        SendMsgToEqp( tVidChn.GetEqpId(), MCU_HDBAS_SETADAPTPARAM_CMD, cServMsg );
        EqpLog("[ChangeHDAdapt] BAS.%d Chn.%d: MCU_HDBAS_SETADAPTPARAM_CMD!\n", 
            tEqp.GetEqpId(), byChnId);
    }

    return TRUE;
}

/*=============================================================================
    �� �� ���� StopHDAdapt
    ��    �ܣ� 
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/11/18  4.5		    �ű���                  ����
    2009/04/17  4.6         �ű���                  �������
=============================================================================*/
BOOL32 CMcuVcInst::StopHDAdapt(const TEqp &tEqp, u8 byChnId)
{
    u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnId);

    if (CHN_ADPMODE_NONE == byChnMode)
    {
        ConfLog(FALSE, "[StopHDAdapt] Bas<%d, %d> unexist in Grp!\n", tEqp.GetEqpId(), byChnId);
        return FALSE;
    }
    CServMsg cServMsg;
    cServMsg.SetChnIndex(byChnId);
    cServMsg.SetConfId(m_tConf.GetConfId());

    SendMsgToEqp(tEqp.GetEqpId(), MCU_HDBAS_STOPADAPT_REQ, cServMsg);
    StopHdBasSwitch(tEqp, byChnId);

    return TRUE;
}

/*=============================================================================
    �� �� ���� StartHDCascadeAdp
    ��    �ܣ� 
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/04/13  4.5		    �ű���                �����ش�����֧��
=============================================================================*/
BOOL32 CMcuVcInst::StartHDCascadeAdp(TMt *ptSrcMt)
{
    if (m_tCascadeMMCU.IsNull())
    {
        return FALSE;
    }

    TMt tSrcMt;
    tSrcMt.SetNull();
    if (ptSrcMt != NULL && !ptSrcMt->IsNull())
    {
        tSrcMt = *ptSrcMt;
    }
    else
    {
        TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
        if(ptConfMcInfo == NULL)
        {
            ConfLog(FALSE, "[StartHDCascadeAdp] CasMcu.%d's McInfo is Null!\n", m_tCascadeMMCU.GetMtId());
            return FALSE;
        }
        tSrcMt = ptConfMcInfo->m_tSpyMt;
        if (tSrcMt.IsNull())
        {
            ConfLog(FALSE, "[StartHDCascadeAdp] CasMcu.%d's spymt is null!\n", m_tCascadeMMCU.GetMtId());
            return FALSE;
        }
    }


    TSimCapSet tSrcSimCap = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());
    TSimCapSet tDstSimCap = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());

    //�����������
    CServMsg cServMsg;
    cServMsg.SetMsgBody();
    
    THDAdaptParam tAdaptParam;
    tAdaptParam.SetVidType(tDstSimCap.GetVideoMediaType());
    tAdaptParam.SetVidActiveType(GetActivePayload(m_tConf, tDstSimCap.GetVideoMediaType()));
    tAdaptParam.SetBitRate(m_tConf.GetBitRate());
    tAdaptParam.SetFrameRate(30);
    tAdaptParam.SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());
    
    u16 wHeight = 0;
    u16 wWidth = 0;
    GetWHByRes(tDstSimCap.GetVideoResolution(), wWidth, wHeight);
    tAdaptParam.SetResolution(wWidth, wHeight);
    
    THDBasVidChnStatus tVidChn;
    memset(&tVidChn, 0, sizeof(tVidChn));
    if (MEDIA_TYPE_H264 != tDstSimCap.GetVideoMediaType() ||
        (MEDIA_TYPE_H264 == tDstSimCap.GetVideoMediaType() &&
        VIDEO_FORMAT_CIF == tDstSimCap.GetVideoResolution()))
    {
        tVidChn.SetOutputVidParam(tAdaptParam, 1);
    }
    else
    {
        tVidChn.SetOutputVidParam(tAdaptParam, 0);
    }
    
    //������Ϣ�����������
    u8 byOutIdx = 0;
    for (; byOutIdx < MAXNUM_VOUTPUT; byOutIdx ++)
    {
        tAdaptParam.Reset();
        if (!tVidChn.GetOutputVidParam(byOutIdx)->IsNull())
        {
            memcpy(&tAdaptParam, tVidChn.GetOutputVidParam(byOutIdx), sizeof(tAdaptParam));
        }
        else
        {
            //����adpparam������baphd
            tAdaptParam.SetVidType(MEDIA_TYPE_H264);
            tAdaptParam.SetVidActiveType(GetActivePayload(m_tConf, MEDIA_TYPE_H264));
            tAdaptParam.SetBitRate(m_tConf.GetBitRate());
            tAdaptParam.SetFrameRate(m_tConf.GetMainVidUsrDefFPS());
            tAdaptParam.SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());
            
            //����Ⱥ�鲻��Ҫ ��2·���䣬��ʱдΪCIF/H264
            if (1 == byOutIdx)
            {
                u16 wHeight = 0;
                u16 wWidth = 0;
                GetWHByRes(VIDEO_FORMAT_CIF, wWidth, wHeight);
                tAdaptParam.SetResolution(wWidth, wHeight);
            }
            //����Ⱥ�鲻��Ҫ ��1·���䣬��ʱдΪ4CIF/H264
            else
            {
                u16 wHeight = 0;
                u16 wWidth = 0;
                GetWHByRes(VIDEO_FORMAT_4CIF, wWidth, wHeight);
                tAdaptParam.SetResolution(wWidth, wHeight);
            }
        }
        cServMsg.CatMsgBody((u8*)&tAdaptParam, sizeof(tAdaptParam));
    }
    
    TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
    cServMsg.CatMsgBody( (u8*)&tEncrypt, sizeof(tEncrypt) );
    
    TDoublePayload tDVPayload;
    if (MEDIA_TYPE_H264 == tSrcSimCap.GetVideoMediaType() || 
        MEDIA_TYPE_H263PLUS == tSrcSimCap.GetVideoMediaType() || 
        CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() )
    {
        tDVPayload.SetActivePayload(GetActivePayload(m_tConf, tSrcSimCap.GetVideoMediaType()));
    }
    else
    {
        tDVPayload.SetActivePayload(tSrcSimCap.GetVideoMediaType());
    }
    tDVPayload.SetRealPayLoad(tSrcSimCap.GetVideoMediaType()); 
    cServMsg.CatMsgBody( (u8*)&tDVPayload, sizeof(tDVPayload) );
    cServMsg.SetChnIndex(m_byCasdVidBasChnnl);
    
    //FIXME: ��ʱ���������ǳ�ʱ��ʱ. �Ժ���BasMgrͳһ�����������е�ReservedBas��ID.
    if (m_tConf.m_tStatus.IsCasdVidAdapting())
    {
        SendMsgToEqp( m_tCasdVidBasEqp.GetEqpId(), MCU_HDBAS_CHANGEPARAM_REQ, cServMsg );
    }
    else
    {
        SendMsgToEqp( m_tCasdVidBasEqp.GetEqpId(), MCU_HDBAS_STARTADAPT_REQ, cServMsg );
    }
    
    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_CASDVID, TRUE);
    
    return TRUE;
}

/*=============================================================================
    �� �� ���� StopHDCascaseAdp
    ��    �ܣ� 
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/04/13  4.5		    �ű���                ����
=============================================================================*/
BOOL32 CMcuVcInst::StopHDCascaseAdp()
{
    if (m_tCasdVidBasEqp.IsNull())
    {
        return FALSE;
    }

    CServMsg cServMsg;
    cServMsg.SetChnIndex(0);
    cServMsg.SetConfId(m_tConf.GetConfId());
    
    SendMsgToEqp(m_tCasdVidBasEqp.GetEqpId(), MCU_HDBAS_STOPADAPT_REQ, cServMsg);

    StopSwitchToPeriEqp(m_tCasdVidBasEqp.GetEqpId(), m_byCasdVidBasChnnl, TRUE, MODE_VIDEO);
    g_cMpManager.RemoveSwitchBridge(m_tCasdVidBasEqp, m_byCasdVidBasChnnl*MAXNUM_VOUTPUT, MODE_VIDEO, TRUE);
    g_cMpManager.RemoveSwitchBridge(m_tCasdVidBasEqp, m_byCasdVidBasChnnl*MAXNUM_VOUTPUT+1, MODE_VIDEO, TRUE);

    StopSwitchToSubMt(m_tCascadeMMCU.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);

    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_CASDVID, FALSE);
    ConfModeChange();

    g_cMcuVcApp.ReleaseHDBasChn(m_tCasdVidBasEqp.GetEqpId(), m_byCasdVidBasChnnl);
    m_tCasdVidBasEqp.SetNull();

    return TRUE;
}

/*=============================================================================
    �� �� ���� StartHdVidSelAdp
    ��    �ܣ� 
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� BOOL32 bIsVGA
                TEqp &tEqp
                u8 &byChnIdx
    �� �� ֵ�� BOOL32 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/5/23   4.5		    �ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::StartHdVidSelAdp(TMt tSrc, TMt tDst, u8 bySelMode)
{
    //��ռBAS: FIXME. Ŀǰ��֧�ֱ�������
    
    //FIXME: ��ʱδ֧��mpu �� ͨ�ù����Ժ�
   
	TMt tOrigSrc = tSrc;
	if (!tSrc.IsLocal())
	{
		tSrc = GetLocalMtFromOtherMcuMt(tSrc);
	}
	if (!tDst.IsLocal())
	{
		tDst = GetLocalMtFromOtherMcuMt(tDst);
	}

	u8 byBasId  = 0xff;
	u8 byChnIdx = 0xff;
	// ��ѯ�Ƿ�֮ǰ����ͬԴ�ն�ռ��BAS,���������������Ϊ�¼�����ն�������
	BOOL32 byNeedNewBas = IsNeedNewSelAdp(tSrc.GetMtId(), tDst.GetMtId(), MODE_VIDEO, &byBasId, &byChnIdx);
	if (byNeedNewBas)
	{
		BOOL32 bRet = g_cMcuVcApp.GetIdleHDBasVidChl( byBasId, byChnIdx);
		if ( bRet )
		{
			TPeriEqpStatus tHDBasStatus;
			g_cMcuVcApp.GetPeriEqpStatus( byBasId, &tHDBasStatus );
			THDBasVidChnStatus* ptChanStatus = NULL;
			if (TYPE_MPU == tHDBasStatus.m_tStatus.tHdBas.GetEqpType())
			{
				ptChanStatus = tHDBasStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus( byChnIdx );
			}
			else
			{
				ptChanStatus = tHDBasStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus( byChnIdx );
			}

			if ( NULL != ptChanStatus )
			{
				TEqp tHDBas;
				tHDBas = ptChanStatus->GetEqp();
				tHDBas.SetConfIdx( m_byConfIdx );
				ptChanStatus->SetEqp( tHDBas );
				g_cMcuVcApp.SetPeriEqpStatus(byBasId, &tHDBasStatus);
				g_cMcuVcApp.SendPeriEqpStatusToMcs(byBasId);
				EqpLog( "[StartHdVidSelAdp] Occupy HDBas.%d, chn.%d\n", byBasId, byChnIdx );

				//����Դ��Ŀ��׼����������������
				CServMsg cServMsg;
				TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
				TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS(tDst.GetMtId());

				u8 abyMediaType[MAXNUM_VOUTPUT] = {MEDIA_TYPE_NULL, MEDIA_TYPE_NULL};
				u8 abyRes[MAXNUM_VOUTPUT]       = {VIDEO_FORMAT_CIF, VIDEO_FORMAT_CIF};
				if (MEDIA_TYPE_H264 == tDstSCS.GetVideoMediaType())
				{
					abyMediaType[0] = MEDIA_TYPE_H264;
					abyMediaType[1] = MEDIA_TYPE_H264;
					if (VIDEO_FORMAT_CIF == tDstSCS.GetVideoResolution())
					{
						abyRes[0] = VIDEO_FORMAT_4CIF;
						abyRes[1] = VIDEO_FORMAT_CIF;
					}
					else
					{
						abyRes[0] = tDstSCS.GetVideoResolution();
						abyRes[1] = VIDEO_FORMAT_CIF;
					}

				}
				else
				{
					if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() ||
						MEDIA_TYPE_H264 == m_tConf.GetSecVideoMediaType())
					{
						abyMediaType[0] = MEDIA_TYPE_H264;
						abyRes[0]       = VIDEO_FORMAT_4CIF;
					}
					abyMediaType[1] = tDstSCS.GetVideoMediaType();
					abyRes[1]       = tDstSCS.GetVideoResolution();
				}

				u16 wOutBitRate = m_tConf.GetBitRate();
				if (m_tConf.GetSecBitRate() != 0)
				{
					wOutBitRate = min(m_tConf.GetSecBitRate(), wOutBitRate);
				}

				EqpLog("[StartHdVidSelAdp] mtid:%d sel mtid:%d using hdbas(firstout:(%d, %d) secout:(%d, %d) bitrate:(%d)\n", 
						tDst.GetMtId(), tSrc.GetMtId(), abyMediaType[0], abyRes[0], abyMediaType[1], abyRes[1], wOutBitRate);


				//��������Ŀ�Ĳ���
				THDAdaptParam tHDAdpParam;
				u16 wWidth , wHeight = 0;
				u8 byFrameRate = 0;
				for (u8 byIdx = 0; byIdx < MAXNUM_VOUTPUT; byIdx++)
				{
					tHDAdpParam.SetVidType(abyMediaType[byIdx]);
					tHDAdpParam.SetBitRate(wOutBitRate);
					tHDAdpParam.SetVidActiveType(GetActivePayload(m_tConf, abyMediaType[byIdx]));    

					GetWHByRes(abyRes[byIdx], wWidth, wHeight);
					tHDAdpParam.SetResolution(wWidth, wHeight);

					if (MEDIA_TYPE_H264 == abyMediaType[byIdx])
					{
						byFrameRate = ((abyMediaType[byIdx] == m_tConf.GetSecVideoMediaType()) ? m_tConf.GetSecVidUsrDefFPS() : m_tConf.GetMainVidUsrDefFPS());
					}
					else
					{
						byFrameRate = ((abyMediaType[byIdx] == m_tConf.GetSecVideoMediaType()) ? m_tConf.GetSecVidFrameRate() : m_tConf.GetMainVidFrameRate());
					}
					tHDAdpParam.SetFrameRate(byFrameRate);

					tHDAdpParam.SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());    
					cServMsg.CatMsgBody((u8*)&tHDAdpParam, sizeof(tHDAdpParam));
					ptChanStatus->SetOutputVidParam(tHDAdpParam, byIdx);	
				}

				TMediaEncrypt  tEncrypt = m_tConf.GetMediaKey();
				cServMsg.CatMsgBody( (u8*)&tEncrypt, sizeof(tEncrypt) );

				TDoublePayload tDVPayload;
				if (MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType() || 
					MEDIA_TYPE_H263PLUS == tSrcSCS.GetVideoMediaType() || 
					CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() )
				{
					tDVPayload.SetActivePayload(GetActivePayload(m_tConf, tSrcSCS.GetVideoMediaType()));
				}
				else
				{
					tDVPayload.SetActivePayload(tSrcSCS.GetVideoMediaType());
				}
				tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType()); 
				cServMsg.CatMsgBody( (u8*)&tDVPayload, sizeof(tDVPayload) );
				cServMsg.SetChnIndex( byChnIdx );

				SendMsgToEqp( byBasId, MCU_HDBAS_STARTADAPT_REQ, cServMsg );

				EqpLog("[StartHdVidSelAdp] CMcuVcInst: BAS.%d Chn.%d start adp!\n", byBasId, byChnIdx);


				//����������Ƚ�������4.6.1�淶����
				TEqp tHDAdpt = g_cMcuVcApp.GetEqp(byBasId);
				tHDAdpt.SetConfIdx(m_byConfIdx);
				g_cMpManager.SetSwitchBridge(tSrc, 0, MODE_VIDEO);
				g_cMpManager.SetSwitchBridge(tHDAdpt, byChnIdx * 2, MODE_VIDEO, TRUE);
				g_cMpManager.SetSwitchBridge(tHDAdpt, byChnIdx * 2 + 1, MODE_VIDEO, TRUE);
				//ѡ�����䲻��������Ӧ,�򵥽�ͨ����Ϊ���������е�״̬
				ptChanStatus->SetStatus(TBasChnStatus::RUNING);
				g_cMcuVcApp.SetPeriEqpStatus(byBasId, &tHDBasStatus);
				g_cMcuVcApp.SendPeriEqpStatusToMcs(byBasId);

				g_cMpManager.StartSwitchToPeriEqp(tSrc, 0, tHDAdpt.GetEqpId(), byChnIdx, MODE_VIDEO, SWITCH_MODE_SELECT,
												  FALSE, TRUE, (VCS_CONF == m_tConf.GetConfSource()));
			}
			else
			{
				ConfLog( FALSE, "[StartHdVidSelAdp] Get HDBas.%d Chan.%d status failed!\n", byBasId, byChnIdx );
				return FALSE;
			}
		}
		else
		{
			ConfLog(FALSE, "[StartHdVidSelAdp] no Idle HdBas to ocuppy!\n");
			return FALSE;
		}
	}
    
    //����ͨ����д��SelGrp
    TEqp tAdpBas = g_cMcuVcApp.GetEqp(byBasId);
	tAdpBas.SetConfIdx(m_byConfIdx);
    m_cSelChnGrp.AddSel(tSrc.GetMtId(), tDst.GetMtId(), MODE_VIDEO, tAdpBas, byChnIdx);


    //Ŀ�ķ�H264��264 cif���ӵڶ�����������ߵ�һ��
	TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS(tDst.GetMtId());
    if (MEDIA_TYPE_H264 != tDstSCS.GetVideoMediaType() ||
		(MEDIA_TYPE_H264 == tDstSCS.GetVideoMediaType() && VIDEO_FORMAT_CIF == tDstSCS.GetVideoResolution()))
    {
        g_cMpManager.StartSwitchToSubMt(tAdpBas, byChnIdx * 2 + 1, tDst, MODE_VIDEO);
    }
    else
    {
        g_cMpManager.StartSwitchToSubMt(tAdpBas, byChnIdx * 2, tDst, MODE_VIDEO);
    }
    
    //ѡ����ʶ��λ
    m_ptMtTable->SetMtSrc(tDst.GetMtId(), &tOrigSrc, MODE_VIDEO);

    //֪ͨ����
    NotifyMtSend(tSrc.GetMtId(), MODE_VIDEO);
    NotifyMtReceive(tSrc, tDst.GetMtId());
    NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);

    //��kdc�����й㲥Դ�ҽ�ʡ����ʱ��֪ͨmtֹͣ��������
//     if(g_cMcuVcApp.IsSavingBandwidth() && 
//         MT_MANU_KDC != m_ptMtTable->GetManuId( tDst.GetMtId()) &&
//         MT_MANU_KDCMCU != m_ptMtTable->GetManuId( tDst.GetMtId()))
//     {
//         NotifyOtherMtSend(tDst.GetMtId(), FALSE);
//     }

    //ˢ�½���
    TMtStatus tMtStatus;

    //����Ŀ���ն�Ϊ�ϼ�mcu�����
    m_ptMtTable->GetMtStatus( tDst.GetMtId(), &tMtStatus );
    if( tSrc.GetType() == TYPE_MT && tSrc.GetMtId() != tDst.GetMtId() && 
        (m_tCascadeMMCU.GetMtId() == 0 || tDst.GetMtId() != m_tCascadeMMCU.GetMtId()) )
    {
        TMtStatus tSrcMtStatus;
        u8 byAddSelByMcsMode = MODE_NONE;
        m_ptMtTable->GetMtStatus( tSrc.GetMtId(), &tSrcMtStatus );
        
        //zbq[12/06/2007] VMP����ʱ������
        if( ((!(tSrc == m_tVidBrdSrc)) || (tSrc == m_tVidBrdSrc && 
            ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() &&
            m_tRollCaller == m_tVidBrdSrc)) &&
            tSrcMtStatus.IsSendVideo() && 
            ( MODE_VIDEO == bySelMode || MODE_BOTH == bySelMode ) )
        {
            byAddSelByMcsMode = MODE_VIDEO;
        }
        if( !(tSrc == m_tAudBrdSrc) && tSrcMtStatus.IsSendAudio() && 
            ( MODE_AUDIO == bySelMode || MODE_BOTH == bySelMode ) )
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
            tMtStatus.AddSelByMcsMode( bySelMode );
        }
        else
        {
            tMtStatus.RemoveSelByMcsMode( bySelMode );
        }
    }
    else
    {
        tMtStatus.RemoveSelByMcsMode( bySelMode );
    }
	tMtStatus.SetSelectMt(tOrigSrc, bySelMode);
    m_ptMtTable->SetMtStatus( tDst.GetMtId(), &tMtStatus );    
    MtStatusChange(tDst.GetMtId(), TRUE);

    //�ݲ�֧��PRS
    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
    }

    return TRUE;
}


/*=============================================================================
    �� �� ���� StopHdVidSelAdp
    ��    �ܣ� 
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ����  TMt &tSrc
                TMt &tDst
                u8 bySelMode
    �� �� ֵ�� BOOL32 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/5/23   4.5		    �ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::StopHdVidSelAdp(TMt tSrc, TMt tDst, u8 bySelMode)
{
    TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
    TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS(tDst.GetMtId());

	if (!tSrc.IsLocal())
	{
		tSrc = GetLocalMtFromOtherMcuMt(tSrc);
	}
	if (!tDst.IsLocal())
	{
		tDst = GetLocalMtFromOtherMcuMt(tDst);
	}

    //������ѡ��
    TEqp tBas;
    tBas.SetNull();
    u8 byChnIdx = 0;
    BOOL32 bRlsChn = FALSE;
    BOOL32 bGetAdpChn = FALSE;
    bGetAdpChn = m_cSelChnGrp.GetSelBasChn(tSrc.GetMtId(), tDst.GetMtId(), MODE_VIDEO, tBas, byChnIdx, bRlsChn);
    if (!bGetAdpChn)
    {
        ConfLog(FALSE, "[StopHdVidSelAdp] get adp chn failed for src.%d, dst.%d, mode.%d!\n",
                        tSrc.GetMtId(), tDst.GetMtId(), bySelMode);
        return FALSE;
    }
    else
    {
        EqpLog("[StopHdVidSelAdp] Chn<%d,%d> for sel<src.%d,dst.%d>, mode.%d, Rls.%d has been checked out!\n",
                tBas.GetEqpId(), byChnIdx, tSrc.GetMtId(), tDst.GetMtId(), bySelMode, bRlsChn);
    }

    //�𽻻�
    TEqp tHDAdpt = g_cMcuVcApp.GetEqp(tBas.GetEqpId());
  
	if (bRlsChn)
	{
		g_cMpManager.RemoveSwitchBridge(tSrc, 0, MODE_VIDEO);
		g_cMpManager.RemoveSwitchBridge(tHDAdpt, 0, MODE_VIDEO);
		g_cMpManager.RemoveSwitchBridge(tHDAdpt, 1, MODE_VIDEO);    
		g_cMpManager.StopSwitchToPeriEqp(m_byConfIdx, tHDAdpt.GetEqpId(), byChnIdx, MODE_VIDEO);

        g_cMcuVcApp.ReleaseHDBasChn(tBas.GetEqpId(), byChnIdx);
		StopHDAdapt(tBas, byChnIdx);
	}

    g_cMpManager.StopSwitchToSubMt(tDst, MODE_VIDEO);
    
    //ѡ����ʶ��λ
    TMt tMtNull;
    tMtNull.SetNull();

	TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus(tDst.GetMtId(), &tMtStatus);
    tMtStatus.RemoveSelByMcsMode(bySelMode);
	tMtStatus.SetSelectMt(tMtNull, bySelMode);
	m_ptMtTable->SetMtStatus(tDst.GetMtId(), &tMtStatus);

    m_ptMtTable->SetMtSrc(tDst.GetMtId(), &tMtNull, bySelMode);

    //�����ָ�
    RestoreRcvMediaBrdSrc(tDst.GetMtId(), bySelMode, TRUE);

    
    //�ݲ�֧��PRS
    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
    }

    //���������
    if (!m_cSelChnGrp.RemoveSel(tSrc.GetMtId(), tDst.GetMtId(), MODE_VIDEO))
    {
        ConfLog(FALSE, "[StopHdVidSelAdp] remove sel<src.%d, dst.%d> failed!\n", tSrc.GetMtId(), tDst.GetMtId());
    }
    
    return TRUE;
}
/*=============================================================================
    �� �� ���� ReleaseResbySel
    ��    �ܣ� 
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMt tSelDstMt
    �� �� ֵ�� void 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/5/23   4.5		    ���㻪                  ����
=============================================================================*/
void CMcuVcInst::ReleaseResbySel(TMt tSelDstMt, u8 bySelMode)
{
	if (!tSelDstMt.IsLocal())
	{
		tSelDstMt = GetLocalMtFromOtherMcuMt(tSelDstMt);
	}
	TMtStatus tStatus;
	m_ptMtTable->GetMtStatus(tSelDstMt.GetMtId(), &tStatus);
	if (MODE_VIDEO == bySelMode || MODE_BOTH == bySelMode)
	{
		TMt tVSelMt = tStatus.GetSelectMt(MODE_VIDEO);
		if (!tVSelMt.IsNull() && !tVSelMt.IsLocal())
		{
			tVSelMt = GetLocalMtFromOtherMcuMt(tVSelMt);
		}
		if (!tVSelMt.IsNull() &&
			IsNeedSelApt(tVSelMt.GetMtId(), tSelDstMt.GetMtId(), MODE_VIDEO))
		{
			StopHdVidSelAdp(tVSelMt, tSelDstMt, MODE_VIDEO);
		}
	}

	if (MODE_AUDIO == bySelMode || MODE_BOTH == bySelMode)
	{
		TMt tASelMt = tStatus.GetSelectMt(MODE_AUDIO);
		if (!tASelMt.IsNull() && !tASelMt.IsLocal())
		{
			tASelMt = GetLocalMtFromOtherMcuMt(tASelMt);
		}

		if (!tASelMt.IsNull() &&
			IsNeedSelApt(tASelMt.GetMtId(), tSelDstMt.GetMtId(), MODE_AUDIO))
		{
			StopAudSelAdp(tASelMt, tSelDstMt);
		}	
	}

}
/*=============================================================================
    �� �� ���� StartAudSelAdp
    ��    �ܣ� 
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ����
    �� �� ֵ�� BOOL32 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/5/23   4.5		    ���㻪                  ����
=============================================================================*/
BOOL32 CMcuVcInst::StartAudSelAdp(TMt tSrc, TMt tDst)
{  
	TMt tOrigSrc = tSrc;
	if (!tSrc.IsLocal())
	{
		tSrc = GetLocalMtFromOtherMcuMt(tSrc);
	}
	if (!tDst.IsLocal())
	{
		tDst = GetLocalMtFromOtherMcuMt(tDst);
	}

	u8 byBasId  = 0xff;
	u8 byChnIdx = 0xff;
	// ��ѯ�Ƿ�֮ǰ����ͬԴ�ն�ռ��BAS,���������������Ϊ�¼�����ն�������
	BOOL32 byNeedNewBas = IsNeedNewSelAdp(tSrc.GetMtId(), tDst.GetMtId(), MODE_AUDIO, &byBasId, &byChnIdx);
	if (byNeedNewBas)
	{
		// Ŀǰ��Ƶ����ֻʹ�ñ���������
		BOOL32 bRet = g_cMcuVcApp.GetIdleBasChl(ADAPT_TYPE_AUD, byBasId, byChnIdx);
		if ( bRet )
		{
			TPeriEqpStatus tBasStatus;
			g_cMcuVcApp.GetPeriEqpStatus( byBasId, &tBasStatus );
			TBasChnStatus tBasChnStatus = tBasStatus.m_tStatus.tBas.tChnnl[byChnIdx];
			//ռ��
			tBasChnStatus.SetStatus(TBasChnStatus::RUNING);
			g_cMcuVcApp.SetPeriEqpStatus(byBasId, &tBasStatus);	
			g_cMcuVcApp.SendPeriEqpStatusToMcs(byBasId);
			EqpLog( "[StartAudSelAdp] Occupy HDBas.%d, chn.%d\n", byBasId, byChnIdx );

			//����Դ��Ŀ��׼����������������
			TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
			TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS(tDst.GetMtId());

			TAdaptParam tAdaptParam;
			tAdaptParam.Clear();
			tAdaptParam.SetVidType(MEDIA_TYPE_NULL);
		    tAdaptParam.SetAudType(tDstSCS.GetAudioMediaType());
			TMediaEncrypt  tEncrypt = m_tConf.GetMediaKey();
			TDoublePayload tDVPayload;
			TDoublePayload tDAPayload;
			if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode())
			{
				tAdaptParam.SetAudActiveType(GetActivePayload(m_tConf, tDstSCS.GetAudioMediaType()));
				tDAPayload.SetActivePayload(GetActivePayload(m_tConf, tDstSCS.GetAudioMediaType()));
			}
			else
			{
				tAdaptParam.SetAudActiveType(tDstSCS.GetAudioMediaType());
				tDAPayload.SetActivePayload(tDstSCS.GetAudioMediaType());
			}
            tDAPayload.SetRealPayLoad(tDstSCS.GetAudioMediaType());
			tAdaptParam.SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());

			CServMsg cServMsg;							
			cServMsg.SetChnIndex(byChnIdx);
			cServMsg.SetConfId(m_tConf.GetConfId());
			cServMsg.SetMsgBody((u8 *)&tAdaptParam, sizeof(TAdaptParam));
			cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //video
			cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload)); //dvideopaload
			cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //audio
			cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload)); //dApayload 
    		TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
			cServMsg.CatMsgBody((u8 *)&tCapSupportEx, sizeof(tCapSupportEx));	//FECType
			SendMsgToEqp(byBasId, MCU_BAS_STARTADAPT_REQ, cServMsg);
			EqpLog("[StartAudSelAdp] CMcuVcInst: BAS.%d Chn.%d start adp!\n", byBasId, byChnIdx);


			//����������Ƚ�������4.6.1�淶����
			TEqp tHDAdpt = g_cMcuVcApp.GetEqp(byBasId);
			tHDAdpt.SetConfIdx(m_byConfIdx);
			g_cMpManager.SetSwitchBridge(tSrc, 0, MODE_AUDIO);
			g_cMpManager.SetSwitchBridge(tHDAdpt, byChnIdx, MODE_AUDIO);
			g_cMpManager.StartSwitchToPeriEqp(tSrc, 0, tHDAdpt.GetEqpId(), byChnIdx, MODE_AUDIO, SWITCH_MODE_SELECT,
											  FALSE, TRUE, (VCS_CONF == m_tConf.GetConfSource()));
		}
		else
		{
			ConfLog(FALSE, "[StartAudSelAdp] no Idle HdBas to ocuppy!\n");
			return FALSE;
		}
	}
    
    //����ͨ����д��SelGrp
    TEqp tAdpBas = g_cMcuVcApp.GetEqp(byBasId);
	tAdpBas.SetConfIdx(m_byConfIdx);
    m_cSelChnGrp.AddSel(tSrc.GetMtId(), tDst.GetMtId(), MODE_AUDIO, tAdpBas, byChnIdx);

    g_cMpManager.StartSwitchToSubMt(tAdpBas, byChnIdx, tDst, MODE_AUDIO);
    
    //ѡ����ʶ��λ
    m_ptMtTable->SetMtSrc(tDst.GetMtId(), &tOrigSrc, MODE_AUDIO);

    //֪ͨ����
    NotifyMtSend(tSrc.GetMtId(), MODE_AUDIO);
    NotifyMtReceive(tSrc, tDst.GetMtId());
    NotifyFastUpdate(tSrc, MODE_AUDIO, TRUE);

    TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus( tDst.GetMtId(), &tMtStatus );      
	tMtStatus.SetSelectMt(tOrigSrc, MODE_AUDIO);
    m_ptMtTable->SetMtStatus( tDst.GetMtId(), &tMtStatus );    
    MtStatusChange(tDst.GetMtId(), TRUE);

    return TRUE;
}
/*=============================================================================
    �� �� ���� StopAudSelAdp
    ��    �ܣ� 
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ����
    �� �� ֵ�� BOOL32 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/5/23   4.5		    ���㻪                  ����
=============================================================================*/
BOOL32 CMcuVcInst::StopAudSelAdp(TMt tSrc, TMt tDst)
{
    TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
    TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS(tDst.GetMtId());

	if (!tSrc.IsLocal())
	{
		tSrc = GetLocalMtFromOtherMcuMt(tSrc);
	}
	if (!tDst.IsLocal())
	{
		tDst = GetLocalMtFromOtherMcuMt(tDst);
	}

    //������ѡ��
    TEqp tBas;
    tBas.SetNull();
    u8 byChnIdx = 0;
    BOOL32 bRlsChn = FALSE;
    BOOL32 bGetAdpChn = FALSE;
    bGetAdpChn = m_cSelChnGrp.GetSelBasChn(tSrc.GetMtId(), tDst.GetMtId(), MODE_AUDIO, tBas, byChnIdx, bRlsChn);
    if (!bGetAdpChn)
    {
        ConfLog(FALSE, "[StopAudSelAdp] get adp chn failed for src.%d, dst.%d!\n",
                        tSrc.GetMtId(), tDst.GetMtId());
        return FALSE;
    }
    else
    {
        EqpLog("[StopAudSelAdp] Chn<%d,%d> for sel<src.%d,dst.%d>, Rls.%d has been checked out!\n",
                tBas.GetEqpId(), byChnIdx, tSrc.GetMtId(), tDst.GetMtId(), bRlsChn);
    }

    //�𽻻�
    TEqp tHDAdpt = g_cMcuVcApp.GetEqp(tBas.GetEqpId());
  
	if (bRlsChn)
	{
		g_cMpManager.RemoveSwitchBridge(tSrc, 0, MODE_AUDIO);
		g_cMpManager.RemoveSwitchBridge(tHDAdpt, byChnIdx, MODE_AUDIO);
		g_cMpManager.StopSwitchToPeriEqp(m_byConfIdx, tHDAdpt.GetEqpId(), byChnIdx, MODE_AUDIO);

		TPeriEqpStatus tBasStatus;
		g_cMcuVcApp.GetPeriEqpStatus( tHDAdpt.GetEqpId(), &tBasStatus );
		TBasChnStatus tBasChnStatus = tBasStatus.m_tStatus.tBas.tChnnl[byChnIdx];
		tBasChnStatus.SetStatus(TBasChnStatus::READY);
		tBasChnStatus.SetReserved(FALSE);
		tBasStatus.m_tStatus.tBas.tChnnl[byChnIdx] = tBasChnStatus;
		g_cMcuVcApp.SetPeriEqpStatus(tHDAdpt.GetEqpId(), &tBasStatus);	
		g_cMcuVcApp.SendPeriEqpStatusToMcs(tHDAdpt.GetEqpId());

		CServMsg cServMsg;
		cServMsg.SetChnIndex(byChnIdx);
		cServMsg.SetConfId(m_tConf.GetConfId());
		SendMsgToEqp(tHDAdpt.GetEqpId(), MCU_BAS_STOPADAPT_REQ, cServMsg);
	}

    g_cMpManager.StopSwitchToSubMt(tDst, MODE_AUDIO);
    
    //ѡ����ʶ��λ
    TMt tMtNull;
    tMtNull.SetNull();

	TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus(tDst.GetMtId(), &tMtStatus);
    tMtStatus.RemoveSelByMcsMode(MODE_AUDIO);
	tMtStatus.SetSelectMt(tMtNull, MODE_AUDIO);
	m_ptMtTable->SetMtStatus(tDst.GetMtId(), &tMtStatus);

    m_ptMtTable->SetMtSrc(tDst.GetMtId(), &tMtNull, MODE_AUDIO);

    //�����ָ�
    RestoreRcvMediaBrdSrc(tDst.GetMtId(), MODE_AUDIO, TRUE);
    
   //���������
    if (!m_cSelChnGrp.RemoveSel(tSrc.GetMtId(), tDst.GetMtId(), MODE_AUDIO))
    {
        ConfLog(FALSE, "[StopAudSelAdp] remove sel<src.%d, dst.%d> failed!\n", tSrc.GetMtId(), tDst.GetMtId());
    }
    
    return TRUE;
}
/*------------------------------------------------------------------*/
/*                                 Mixer                            */
/*------------------------------------------------------------------*/


/*====================================================================
    ������      ��StartMixing
    ����        ����ʼ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMixMode ����ģʽ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         ������         ����
====================================================================*/
BOOL32 CMcuVcInst::StartMixing( u8 byMixMode )
{
    u8 byEqpId = 0;
	u8 byGrpId = 0;
	u8 byAudioType = m_tConf.GetMainAudioMediaType();
    u8 byAudioType2 = m_tConf.GetSecAudioMediaType();
	TPeriEqpStatus tPeriEqpStatus;

	//�õ����еĻ�����
    byEqpId = g_cMcuVcApp.GetIdleMixGroup( byEqpId, byGrpId );
	if( byEqpId == 0 )
	{
		return FALSE;//�޿��õĻ�����
	}
	m_tMixEqp = g_cMcuVcApp.GetEqp( byEqpId );
	m_tMixEqp.SetConfIdx( m_byConfIdx );
	m_byMixGrpId = byGrpId;

	//ռ�û�����
	g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tPeriEqpStatus );
    // guzh [11/9/2007] 
	tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[byGrpId].m_byGrpMixDepth = GetMaxMixerDepth();
    u8 byMixerGrpState = TMixerGrpStatus::WAIT_BEGIN;
    switch (byMixMode)
    {
    case mcuPartMix:
        byMixerGrpState = TMixerGrpStatus::WAIT_START_SPECMIX;
        break;
    case mcuWholeMix:
        byMixerGrpState = TMixerGrpStatus::WAIT_START_AUTOMIX;
        break;
    case mcuVacMix:
        byMixerGrpState = TMixerGrpStatus::WAIT_START_VAC;
        break;
    default:
        break;
    }

    //��ռ��,��ʱ��δ�ɹ��ٷ���
	tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[byGrpId].m_byGrpState = byMixerGrpState;
    EqpLog("[StartMixing] Eqp.%d ConfIdx.%d GrpState.%d\n",
            m_tMixEqp.GetEqpId(), m_byConfIdx, byMixerGrpState);

	g_cMcuVcApp.SetPeriEqpStatus( byEqpId, &tPeriEqpStatus );

	TMixerStart tMixer;
	tMixer.SetAudioEncrypt(m_tConf.GetMediaKey());
	tMixer.SetAudioMode(byAudioType);
    tMixer.SetSecAudioMode(byAudioType2);
//	tMixer.SetIsAllMix(!m_tConf.m_tStatus.IsMixSpecMt());
    tMixer.SetIsAllMix(TRUE); // xsl [8/24/2006] �µĶ��ƻ�������������֪ͨ��ǰ���������ͨ��
	tMixer.SetMixGroupId(byGrpId);
    //zbq [11/22/2007] ����ǰ�ķǿջ������
    tMixer.SetMixDepth(GetMaxMixerDepth());

    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
        tMixer.SetIsNeedByPrs(TRUE);
    }
    else
    {
        tMixer.SetIsNeedByPrs(FALSE);
    }

	TDoublePayload tDPayload;
	tDPayload.SetRealPayLoad(byAudioType);
	tDPayload.SetActivePayload(GetActivePayload(m_tConf, byAudioType));

	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetMsgBody( (u8*)&tMixer, sizeof(tMixer) );
	cServMsg.CatMsgBody( (u8*)&tDPayload, sizeof(tDPayload));

	// MCUǰ�����, zgc, 2007-09-27
	TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
	cServMsg.CatMsgBody( (u8*)&tCapSupportEx, sizeof(tCapSupportEx) );

	//������������ʼ��Ϣ
	SendMsgToEqp( byEqpId, MCU_MIXER_STARTMIX_REQ, cServMsg );

	EqpLog( "[MCU_MIXER_STARTMIX_REQ] AudioType.%d!\n", byAudioType );

	//���õȴ�Ӧ��ʱ��
	SetTimer( MCUVC_MIX_WAITMIXERRSP_TIMER, 6*1000 );

	return TRUE;
}

/*====================================================================
    ������      ��StopMixing
    ����        ��ֹͣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         ������         ����
====================================================================*/
void CMcuVcInst::StopMixing(void)
{
	CServMsg cServMsg;
	cServMsg.SetConfId(m_tConf.GetConfId());

	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (m_tConfAllMtInfo.MtJoinedConf(byMtId))            
		{
            // guzh [11/7/2007] 
			// fxh[09/06/04]�ȴ���������Ӧ�����������״̬
			// ������Ӧδ��ǰ,�Ҷϲ���������ն�,�������ն˵��������Ľ���δ��
//             m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);
// 			if (m_ptMtTable->IsMtInMixing(byMtId))
// 			{
// 				m_ptMtTable->SetMtInMixing(byMtId, FALSE);
// 				u8 byMix = 0;
// 				cServMsg.SetMsgBody((u8*)&byMix, 1);
// 				SendMsgToMt(byMtId, MCU_MT_MTADDMIX_NOTIF, cServMsg);
// 			}

			if (m_tConf.GetConfAttrb().IsResendLosePack())
			{
				u8  byChlNum;
				u16 wRtcpSwitchPort;
				TTransportAddr  tAddr;
				TTransportAddr  tEqpAddr;
				TLogicalChannel tLogicalChannel;
				TLogicalChannel tEqpLogicalChannel;

				m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_AUDIO, &tLogicalChannel, FALSE);

				g_cMcuVcApp.GetPeriEqpLogicChnnl(m_tMixEqp.GetEqpId(), MODE_AUDIO, &byChlNum, &tEqpLogicalChannel, TRUE);

				//�Ƴ�����Rtcp��������
				
				tAddr = tLogicalChannel.GetRcvMediaCtrlChannel();
				
				tEqpAddr = tEqpLogicalChannel.GetRcvMediaCtrlChannel();
				
				wRtcpSwitchPort = (tEqpAddr.GetPort() - 2) + 
					              MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId + PORTSPAN*GetMixChnPos(byMtId) + 4;

				g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, tEqpAddr.GetIpAddr(), wRtcpSwitchPort);


				//�Ƴ�����Rtcp��������
				tAddr = tLogicalChannel.GetSndMediaCtrlChannel();

				g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, tAddr.GetIpAddr(), tAddr.GetPort());

			}
		}
	}

    // guzh [11/9/2007]  ���õȴ�
    u8 byEqpId = m_tMixEqp.GetEqpId();    
    TPeriEqpStatus tPeriEqpStatus;
    g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tPeriEqpStatus );
    tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = TMixerGrpStatus::WAIT_STOP;
    g_cMcuVcApp.SetPeriEqpStatus( byEqpId, &tPeriEqpStatus );

	cServMsg.SetMsgBody((u8 *)&m_byMixGrpId, 1);
	SendMsgToEqp(m_tMixEqp.GetEqpId(), MCU_MIXER_STOPMIX_REQ, cServMsg);

    // guzh [11/9/2007] 
    SetTimer(MCUVC_MIX_WAITMIXERRSP_TIMER, 6*1000);

	EqpLog("Mixer stop working\n");
	return;
}

/*====================================================================
    ������      ��SwitchMixMember
    ����        �������������������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt *ptMt ��Ա�ն�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/05/31    3.6         LIBO          ����
    06/08/04    4.0         xsl           ����Nģʽ����
====================================================================*/
void CMcuVcInst::SwitchMixMember(const TMt *ptMt, BOOL32 bNMode/*=false*/)
{
    //�Ƿ�����Ƶ����
    TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus(ptMt->GetMtId(), &tMtStatus);
    if (tMtStatus.IsMediaLoop(MODE_AUDIO))
    {
        return;
    }

    // xsl [8/19/2006] Nģʽ��ɢ����֪ͨ�ն˸ı���յ�ַ�Ͷ˿�Ϊ�鲥
    if (bNMode && m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        ChangeSatDConfMtRcvAddr(ptMt->GetMtId(), LOGCHL_AUDIO);
        return;
    }
     
	TTransportAddr  tAddr;
	TLogicalChannel tLogicalChannel;
	if (m_ptMtTable->GetMtLogicChnnl(ptMt->GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE))
	{
		tAddr = tLogicalChannel.GetRcvMediaChannel();
	}
	else
	{
		return;
	}

	u8  byMixerId = m_tMixEqp.GetEqpId();
	u32 dwSwitchIpAddr;
	u16 wSwitchPort;
	u32 dwMtSwitchIp;
	u16 wMtSwitchPort;
	u32 dwMtSrcIp;

	g_cMpManager.GetMixerSwitchAddr(byMixerId, dwSwitchIpAddr, wSwitchPort);
	g_cMpManager.GetSwitchInfo(*ptMt, dwMtSwitchIp, wMtSwitchPort, dwMtSrcIp);

	//����Ӧ����ͬһ���������ڶ��������������Դ���ն˿ڼ�������
    if (bNMode)
    {
        wSwitchPort = wSwitchPort + MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId + 2;
    }
    else
    {
        wSwitchPort = wSwitchPort + MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId + PORTSPAN*GetMixChnPos(ptMt->GetMtId()) + 2;
    }
	
	//��������ʱ�����ð�������������
    // xsl [7/29/2006] N-1ģʽֻ�������������ն˽�������
	if (m_tConf.m_tStatus.IsMixing() && 
        (m_ptMtTable->IsMtInMixGrp(ptMt->GetMtId())&&!bNMode || bNMode))
	{
        u32 dwRcvIp, dwSrcIp;
        u16 wRcvPort;        
        //�õ�����Դ��ַ
        if( !g_cMpManager.GetSwitchInfo( m_tMixEqp, dwRcvIp, wRcvPort, dwSrcIp ) )
        {
            EqpLog("SwitchMixMember() get switch info failed!");
        }
        
		TMt tSrcMt;
		tSrcMt.SetNull();
		g_cMpManager.StartSwitch(tSrcMt, m_byConfIdx, dwSrcIp, 0, 
								 dwMtSwitchIp, wSwitchPort, tAddr.GetIpAddr(), tAddr.GetPort());

        // guzh [8/31/2006] �����ն˵���ƵԴ�ǻ�����
        m_ptMtTable->SetMtSrc( ptMt->GetMtId(), &m_tMixEqp, MODE_AUDIO );
        // ���ѡ��״̬
        m_ptMtTable->GetMtStatus(ptMt->GetMtId(), &tMtStatus);
        tMtStatus.RemoveSelByMcsMode(MODE_AUDIO);
        m_ptMtTable->SetMtStatus(ptMt->GetMtId(), &tMtStatus);
	}

	return;
}

/*====================================================================
    ������      ��AddMixMember
    ����        �����ӻ�����Ա
    �㷨ʵ��    ��֪ͨmixer����ͨ������vacģʽ��mixer����������mt���������н���
    ����ȫ�ֱ�����
    �������˵����const TMt *ptMt ��Ա�ն�
                  u8 byVolume ��Ա����
                  BOOL32 bForce �Ƿ���ǿ�� 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         ������         ����
====================================================================*/
BOOL32 CMcuVcInst::AddMixMember( const TMt *ptMt, u8 byVolume, BOOL32 bForce )
{
    //�Ƿ�����Ƶ����
    TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus(ptMt->GetMtId(), &tMtStatus);
    if (tMtStatus.IsMediaLoop(MODE_AUDIO))
    {
        return FALSE;
    }

    // xsl [8/19/2006] ��ɢ����֪ͨ�ն˸ı���յ�ַ�Ͷ˿�Ϊ����
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        ChangeSatDConfMtRcvAddr(ptMt->GetMtId(), LOGCHL_AUDIO, FALSE);
    }

	TMixMember tMixMember;
	memset( &tMixMember, 0, sizeof(tMixMember) );
	tMixMember.m_byVolume = byVolume;
	tMixMember.m_tMember  = *ptMt;

	TTransportAddr  tAddr;
	TLogicalChannel tLogicalChannel;
	if (m_ptMtTable->GetMtLogicChnnl(ptMt->GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE))
	{
		tAddr = tLogicalChannel.GetRcvMediaChannel();
	}
	else
	{
		return FALSE;
	}

    tMixMember.m_byAudioType = tLogicalChannel.GetChannelType();

	u8  byMixerId = m_tMixEqp.GetEqpId();
	u32 dwSwitchIpAddr;
	u16 wSwitchPort;
	u32 dwMtSwitchIp;
	u16 wMtSwitchPort;
	u32 dwMtSrcIp;
	g_cMpManager.GetMixerSwitchAddr(byMixerId, dwSwitchIpAddr, wSwitchPort);

	g_cMpManager.GetSwitchInfo(*ptMt, dwMtSwitchIp, wMtSwitchPort, dwMtSrcIp);

	//����Ӧ����ͬһ���������ڶ��������������Դ���ն˿ڼ�������
	wSwitchPort = wSwitchPort + 
		          MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId + PORTSPAN * GetMixChnPos(ptMt->GetMtId()) + 2;

	tMixMember.m_tAddr.SetIpAddr(dwMtSwitchIp);
	tMixMember.m_tAddr.SetPort(wSwitchPort);
	tMixMember.m_tRtcpBackAddr.SetIpAddr(dwMtSwitchIp);
	tMixMember.m_tRtcpBackAddr.SetPort(wSwitchPort + 1);

	CServMsg cServMsg;
	cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetChnIndex(GetMixChnPos(ptMt->GetMtId()));
	cServMsg.SetMsgBody((u8*)&m_byMixGrpId, 1);
	cServMsg.CatMsgBody((u8*)&tMixMember, sizeof(TMixMember));
	cServMsg.CatMsgBody((u8*)&tLogicalChannel.m_byChannelType, sizeof(u8));

	if (bForce)
	{
		SendMsgToEqp(m_tMixEqp.GetEqpId(), MCU_MIXER_FORCEACTIVE_REQ, cServMsg);
        // xsl [8/4/2006] ֱ�ӷ��ؼ���
        return TRUE;
	}

    // xsl [7/29/2006] ���ڻ������ڵ�mt�ż��������
    if (!m_ptMtTable->IsMtInMixGrp(ptMt->GetMtId()))
    {
        SendMsgToEqp(m_tMixEqp.GetEqpId(), MCU_MIXER_ADDMEMBER_REQ, cServMsg);
        
        m_ptMtTable->SetMtInMixGrp(ptMt->GetMtId(), TRUE);     
    }

	//��������ʱ�����ð���������������
	//����ģʽ�£���������������ֽ����������ٽ��������ն�
	if (m_tConf.m_tStatus.IsMixing())
	{
        u32 dwRcvIp, dwSrcIp;
        u16 wRcvPort;        
        //�õ�����Դ��ַ
        if( !g_cMpManager.GetSwitchInfo( m_tMixEqp, dwRcvIp, wRcvPort, dwSrcIp ) )
        {
            EqpLog("AddMixMember() get switch info failed!");
        }

		TMt tSrcMt;
		tSrcMt.SetNull();
		g_cMpManager.StartSwitch(tSrcMt, m_byConfIdx, 
								 dwSrcIp, 0, 
								 tMixMember.m_tAddr.GetIpAddr(), tMixMember.m_tAddr.GetPort(), 
								 tAddr.GetIpAddr(), tAddr.GetPort());

        // guzh [8/31/2006] �����ն˵���ƵԴ�ǻ�����
        m_ptMtTable->SetMtSrc( ptMt->GetMtId(), &m_tMixEqp, MODE_AUDIO );

        // ���ѡ��״̬
        m_ptMtTable->GetMtStatus(ptMt->GetMtId(), &tMtStatus);
        tMtStatus.RemoveSelByMcsMode(MODE_AUDIO);
        m_ptMtTable->SetMtStatus(ptMt->GetMtId(), &tMtStatus);
	}

	if (m_tConf.GetConfAttrb().IsResendLosePack())
	{
		u8  byChlNum = 0;
		u16 wRtcpSwitchPort = 0;
		TTransportAddr  tEqpAddr;
		TLogicalChannel tEqpLogicalChannel;

		m_ptMtTable->GetMtLogicChnnl(ptMt->GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE);

		g_cMcuVcApp.GetPeriEqpLogicChnnl(m_tMixEqp.GetEqpId(), MODE_AUDIO, &byChlNum, &tEqpLogicalChannel, TRUE);

		//�������Rtcp��������
		tAddr    = tLogicalChannel.GetRcvMediaCtrlChannel();
		tEqpAddr = tEqpLogicalChannel.GetRcvMediaCtrlChannel();		
		
        // xsl [7/24/2006] +2 Ϊ���������ն���˿ڣ�+4Ϊ���������Ͷ���˿�
		wRtcpSwitchPort = (tEqpAddr.GetPort() - 2) + 
			              MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId + PORTSPAN*GetMixChnPos(ptMt->GetMtId()) + 4;

		g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, 
										 tAddr.GetIpAddr(), 0, 
										 tAddr.GetIpAddr(), tAddr.GetPort(), 
										 tEqpAddr.GetIpAddr(), wRtcpSwitchPort);

		//�������Rtcp��������
		tAddr = tLogicalChannel.GetSndMediaCtrlChannel();

		g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, 
										 tMixMember.m_tRtcpBackAddr.GetIpAddr(), 0, 
										 tMixMember.m_tRtcpBackAddr.GetIpAddr(), 
										 tMixMember.m_tRtcpBackAddr.GetPort(), 
										 tAddr.GetIpAddr(), tAddr.GetPort());
	}

	return TRUE;
}

/*====================================================================
    ������      ��RemoveMixMember
    ����        ���Ƴ�������Ա
    �㷨ʵ��    ��֪ͨmixer�Ƴ�ͨ��
    ����ȫ�ֱ�����
    �������˵����const TMt *ptMt ��Ա�ն�
                  BOOL32 bForce �Ƿ���ǿ�� 
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         ������         ����
====================================================================*/
void CMcuVcInst::RemoveMixMember( const TMt *ptMt, BOOL32 bForce )
{
	TMixMember tMixMember;
	tMixMember.m_byVolume = DEFAULT_MIXER_VOLUME;
	tMixMember.m_tMember  = *ptMt;

	TLogicalChannel tLogicalChannel;
	if( m_ptMtTable->GetMtLogicChnnl( ptMt->GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )
	{
		tMixMember.m_tAddr = tLogicalChannel.GetRcvMediaChannel();	
	}
	tMixMember.m_byAudioType = tLogicalChannel.GetChannelType();

	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );
    cServMsg.SetChnIndex( GetMixChnPos(ptMt->GetMtId()) );
	cServMsg.SetMsgBody( (u8*)&m_byMixGrpId, 1 );
	cServMsg.CatMsgBody( (u8*)&tMixMember, sizeof(TMixMember) );

	if( bForce )
	{
		SendMsgToEqp( m_tMixEqp.GetEqpId(), MCU_MIXER_CANCELFORCEACTIVE_REQ, cServMsg );
	}
	else
	{
        // xsl [7/29/2006] ֻ���ڻ������ڲ��Ƴ�ͨ��
        if (m_ptMtTable->IsMtInMixGrp(ptMt->GetMtId()))
        {
            SendMsgToEqp( m_tMixEqp.GetEqpId(), MCU_MIXER_REMOVEMEMBER_REQ, cServMsg );
            m_ptMtTable->SetMtInMixGrp(ptMt->GetMtId(), FALSE);
        }        
	}

	//��ֹͣ����ʱͳһ�Ƴ�������Rtcp��������

	return;
}


/*====================================================================
    ������      ��GetMixChnPos
    ����        ����ȡ������Աͨ��λ�ã��������ֽ����˿ں�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8     byMtId
                  BOOL32 bRemove
    ����ֵ˵��  ��u8: pos
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/03/19    4.0         �ű���         ����
====================================================================*/
u8 CMcuVcInst::GetMixChnPos( u8 byMtId, BOOL32 bRemove )
{
    u8 byDstPos = 0;
    if ( byMtId == 0 )
    {
        ConfLog( FALSE, "[GetMixChnPos] param err: byMtId.%d \n", byMtId );
        return byDstPos;
    }
    
    if ( !bRemove )
    {
        BOOL32 bExist = FALSE;
        u8     byFirstIdle = 0xFF;

        u8 byPos = 0;
        
        for( byPos = 1; byPos < MAXNUM_MIXER_CHNNL+1; byPos++ )
        {
            // guzh [5/12/2007] Find the old one
            if ( byMtId == m_abyMixMtId[byPos])
            {
                bExist = TRUE;
                MpManagerLog( "[GetMixChnPos] Mt.%d already exist at byPos.%d !\n", byMtId, byPos );
                byDstPos = byPos;
                break;
            }
            else if ( 0 == m_abyMixMtId[byPos] && 0xFF == byFirstIdle ) 
            {
                byFirstIdle = byPos;
            }
        }
        if ( !bExist )
        {
            if ( 0xFF != byFirstIdle )
            {
                m_abyMixMtId[byFirstIdle] = byMtId;
                MpManagerLog( "[GetMixChnPos] Mt.%d set succeed, byPos.%d !\n", byMtId, byFirstIdle );
                byDstPos = byFirstIdle;
            }
            else
            {
                ConfLog(FALSE, "[GetMixChnPos] Mt.%d set to m_abyMixMtId failed !\n", byMtId );
            }
        }
    }
    else
    {
        BOOL32 bExist = FALSE;
        u8 byPos = 1;
        for( ; byPos < MAXNUM_MIXER_CHNNL+1; byPos++ )
        {
            if ( byMtId == m_abyMixMtId[byPos] )
            {
                m_abyMixMtId[byPos] = 0;
                bExist = TRUE;
                MpManagerLog( "[GetMixChnPos] Mt.%d remve m_abyMixMtId succeed, byPos.%d !\n", byMtId, byPos );
                break;
            }
        }
        if ( !bExist )
        {
            ConfLog( FALSE, "[GetMixChnPos] Mt.%d remve m_abyMixMtId failed !\n", byMtId );
        }
    }
    return byDstPos;
}

/*====================================================================
    ������      ��NotifyMixerSendToMt
    ����        ��֪ͨ���������ն˷�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bSend �Ƿ���
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         ������         ����
====================================================================*/
BOOL32 CMcuVcInst::NotifyMixerSendToMt( BOOL32 bSend )
{
	u8 bySend = ( bSend == FALSE ) ? 0 : 1;

	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetMsgBody( (u8*)&m_byMixGrpId, 1 );
	cServMsg.CatMsgBody( (u8*)&bySend, 1 );

	//������������ʼ��Ϣ
	SendMsgToEqp( m_tMixEqp.GetEqpId() , MCU_MIXER_SEND_NOTIFY, cServMsg );

	return TRUE;
}

/*====================================================================
    ������      ��NotifyMixerSendToMt
    ����        ��֪ͨ���������ն˷�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bSend �Ƿ���
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         ������         ����
====================================================================*/
void CMcuVcInst::SetMixerSensitivity( u8 bySensVal )
{
	u32 dwSens = bySensVal;
	dwSens = htonl( dwSens );

	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetMsgBody( (u8*)&m_byMixGrpId, 1 );
	cServMsg.CatMsgBody( (u8*)&dwSens, sizeof(u32) );

	//������������ʼ��Ϣ
	SendMsgToEqp( m_tMixEqp.GetEqpId() , MCU_MIXER_VACKEEPTIME_CMD, cServMsg );

	return;
}

/*=============================================================================
    �� �� ���� SetMixDelayTime
    ��    �ܣ� ���û�����ʱʱ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u16 wDelayTime
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/1/10    3.6			����                  ����
=============================================================================*/
void CMcuVcInst::SetMixDelayTime(u16 wDelayTime)
{
	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetMsgBody(&m_byMixGrpId, sizeof(u8));
	cServMsg.CatMsgBody((u8*)&wDelayTime, sizeof(u16));

	//������������Ϣ
	SendMsgToEqp( m_tMixEqp.GetEqpId() , MCU_MIXER_CHANGEMIXDELAY_CMD, cServMsg );

	EqpLog("[SetMixDelayTime] MixEqpId :%d, MixGrpId :%d, DelayTime :%u\n", 
		   m_tMixEqp.GetEqpId(), m_byMixGrpId, wDelayTime);

	return;
}

/*====================================================================
    ������      ��SetMemberVolume
    ����        �����ó�Ա����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt *ptMt ������Ա
	              u8 byVolume ����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         ������         ����
====================================================================*/
void CMcuVcInst::SetMemberVolume( const TMt *ptMt, u8 byVolume )
{
	TMixMember  tMixMember;
	tMixMember.m_byVolume = byVolume;
	tMixMember.m_tMember  = *ptMt;

	TLogicalChannel tLogicalChannel;
	if ( m_ptMtTable->GetMtLogicChnnl( ptMt->GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )
	{
		tMixMember.m_tAddr = tLogicalChannel.GetRcvMediaChannel();	
	}
	tMixMember.m_byAudioType = tLogicalChannel.GetChannelType();

	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );
    cServMsg.SetChnIndex( GetMixChnPos(ptMt->GetMtId()) );
	cServMsg.SetMsgBody( (u8*)&m_byMixGrpId, 1 );
	cServMsg.CatMsgBody( (u8*)&tMixMember, sizeof(TMixMember) );
	SendMsgToEqp( m_tMixEqp.GetEqpId(), MCU_MIXER_SETCHNNLVOL_CMD, cServMsg );

	return;
}

/*====================================================================
    ������      ��ProcMixerRspWaitTimer
    ����        ��MCU�ȴ�������Ӧ��ʱ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/08    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ProcMixerRspWaitTimer( const CMessage * pcMsg )
{
	// fxh
	KillTimer(MCUVC_MIX_WAITMIXERRSP_TIMER);
	if (m_tMixEqp.IsNull())
	{
		EqpLog("[ProcMixerRspWaitTimer]m_tMixEqp Is Null!\n");
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	TPeriEqpStatus tPeriEqpStatus;
	u8 byMixGrpState;

	switch (CurState())
	{
	case STATE_ONGOING:

		//����������
		g_cMcuVcApp.GetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
        byMixGrpState = tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState;
		tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = TMixerGrpStatus::READY;
		g_cMcuVcApp.SetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );

		switch(byMixGrpState)
		{
		case TMixerGrpStatus::WAIT_START_VAC:
			// guzh [11/9/2007]
			//NACK
			cServMsg.SetErrorCode( ERR_MCU_TIMEROUT );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			break;
		case TMixerGrpStatus::WAIT_STOP:
			ProcMixStopSucRsp();			
		    break;
		default:
		    break;
		}

        m_tMixEqp.SetNull();
        m_byMixGrpId = 0;

		break;

	default:
		ConfLog(FALSE, "Wrong message %u(%s) received in state %u in ProcMixerRspWaitTimer!\n", 
				pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState());
		break;
	}

	return;
}

/*====================================================================
    ������      ��ProcMixerMcuRsp
    ����        ����������ӦMCU��Ϣ������
    �㷨ʵ��    ��.
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         ������        ����
	06/02/17	4.0			�ű���		  ����״̬�ָ�����
    06/04/20	4.0			�ű���		  ֹͣ������ָ�ѡ��״̬
====================================================================*/
void CMcuVcInst::ProcMixerMcuRsp(const CMessage * pcMsg)
{
	CServMsg       cServMsg(pcMsg->content, pcMsg->length);	
	TPeriEqpStatus tPeriEqpStatus;
	TConfMtInfo	   tConfMtInfo;
	TMt            tMt;
	u8             byMixGrpState;
	u8             byMtId;
	TMixMember     tMixMember;
    u8             byMixMtNum;

	switch (CurState())
	{
	case STATE_ONGOING:

		switch (pcMsg->event)
		{
		case MIXER_MCU_STARTMIX_ACK:        //ͬ�⿪ʼʹ�û�����Ӧ��

			//�����ʱ
			KillTimer(MCUVC_MIX_WAITMIXERRSP_TIMER);

			//���ò���
			g_cMcuVcApp.GetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);
			byMixGrpState = tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState;

			EqpLog("[MIXER_MCU_STARTMIX_ACK] Eqp.%d MixGrpState.%d\n", 
        		   m_tMixEqp.GetEqpId(), byMixGrpState);
        
            tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = TMixerGrpStatus::MIXING;
            g_cMcuVcApp.SetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);
    
			//�ı����״̬
            if (byMixGrpState == TMixerGrpStatus::WAIT_START_VAC)
			{
                m_tConf.m_tStatus.SetVACing();
				//֪ͨ��ϯ�����л��
				SendMsgToAllMcs(MCU_MCS_STARTVAC_NOTIF, cServMsg);
				if (HasJoinedChairman())
				{
					SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STARTVAC_NOTIF, cServMsg);
				}

				//֪ͨ������ֹͣ���ն˷�����
				NotifyMixerSendToMt(FALSE);

			}
			else//����������
			{
                if (byMixGrpState == TMixerGrpStatus::WAIT_START_SPECMIX)
                {
                    m_tConf.m_tStatus.SetSpecMixing();
                }
                else
                {
                    m_tConf.m_tStatus.SetAutoMixing();
                }

				TMcu tMcu;
				tMcu.SetMcuId(LOCAL_MCUID);
				cServMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) ); 
                u8 byMixDepth = GetMaxMixNum();
				cServMsg.CatMsgBody( (u8*)&byMixDepth, 1 );
				if (!m_tCascadeMMCU.IsNull())
				{
					cServMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
					SendMsgToMt(m_tCascadeMMCU.GetMtId(),  MCU_MCU_STARTMIXER_NOTIF, cServMsg);
				}
				if (HasJoinedChairman())
				{
					cServMsg.SetMsgBody((u8*)&byMixDepth, 1);
					SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STARTDISCUSS_NOTIF, cServMsg);
				}

                ChangeAudBrdSrc(&m_tMixEqp);                
			}

            //����״̬֪ͨ,֪ͨ��ϯ�����л��
            MixerStatusChange();            

            SetMixerSensitivity(m_tConf.GetTalkHoldTime());
            
			// xsl [8/4/2006] ��ӻ�����Ա, ����������������
            byMixMtNum = 0;
			for (byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
            {
                if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
                {
                    tMt = m_ptMtTable->GetMt(byMtId);    

                    if ((!m_tConf.m_tStatus.IsSpecMixing() ||
                        (m_tConf.m_tStatus.IsSpecMixing() && m_ptMtTable->IsMtInMixing(byMtId)))
                        && byMixMtNum < GetMaxMixNum())
                    {
                        //֪ͨmixer����ͨ������vacģʽ��mixer����������mt���������н���                
                        AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE);

                        StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
								             (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())), 
								             MODE_AUDIO, SWITCH_MODE_SELECT);
                        byMixMtNum++;
                    }  
           
                    //���ƻ������ڻ����б�����Nģʽ����
                    if (m_tConf.m_tStatus.IsSpecMixing() && !m_ptMtTable->IsMtInMixing(byMtId))
                    {
                        SwitchMixMember(&tMt, TRUE);
                    }
                }           
            }  

            // xsl [8/19/2006] �����nģʽ��ɢ������Ҫ�������鲥��ַ
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                g_cMpManager.StartDistrConfCast(m_tMixEqp, MODE_AUDIO);
            }

            // guzh [11/7/2007] 
			//�ڻ���״̬��λ���ٷ�����������
			if ( m_tLastMixParam.GetMode() == mcuVacMix )
			{
				CServMsg cServMsg;				
                cServMsg.SetEventId(MCS_MCU_STARTVAC_REQ);				
                MixerVACReq(cServMsg);
                //�ϴλ������Ĺ�������ָ�����Ϊֹ
				m_tLastMixParam.SetMode(mcuNoMix);
			}

			if (m_tConf.m_tStatus.IsMixing())
			{
				//֪ͨ��������ʼ���ն˷�����
				NotifyMixerSendToMt(TRUE);
			}

			if (m_tConf.m_tStatus.IsSpecMixing())
			{
				AddRemoveSpecMixMember();         
			}
			else 
			{
				MtStatusChange();
			}
			break;

		case MIXER_MCU_STOPMIX_ACK:         //ͬ��ֹͣ����Ӧ��	
			
			// fxh �����ʱ��
			KillTimer(MCUVC_MIX_WAITMIXERRSP_TIMER);
			// ���״̬��
			if ((m_tConf.m_tStatus.IsVACing() || m_tConf.m_tStatus.IsMixing()) &&
				!m_tMixEqp.IsNull())
			{
				TPeriEqpStatus tPeriStatus;
				g_cMcuVcApp.GetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriStatus);
				if (tPeriStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState != TMixerGrpStatus::WAIT_STOP)
				{
					EqpLog("[CMcuVcInst%d][ProcMixerMcuRsp]stop mix(%d) ack recv at wrong state(%s)\n", 
						   GetInsID(), m_tMixEqp.GetMtId(), tPeriStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState);
					return;
				}

			}
            
            ProcMixStopSucRsp();
			break;

		case MIXER_MCU_ADDMEMBER_ACK:       //�����ԱӦ����Ϣ

			tMixMember =  *((TMixMember*)(cServMsg.GetMsgBody() + sizeof(u8)));
			if (tMixMember.m_tMember == GetLocalSpeaker())//ǿ�ƻ���
			{
				AddMixMember(&tMixMember.m_tMember, DEFAULT_MIXER_VOLUME, TRUE);
			}

			break;

		case MIXER_MCU_REMOVEMEMBER_ACK:    //ɾ����ԱӦ����Ϣ

			tMixMember =  *((TMixMember*)(cServMsg.GetMsgBody() + sizeof(u8)));
			if (tMixMember.m_tMember == GetLocalSpeaker())// xsl [8/4/2006]ǿ�ƻ���(��ʱ�ն˿����Ѿ����Ƿ�����)
			{
				RemoveMixMember(&tMixMember.m_tMember, TRUE);
			}

			break;

		case MIXER_MCU_FORCEACTIVE_ACK:     //ǿ�Ƴ�Ա����Ӧ��

			break;

		case MIXER_MCU_CANCELFORCEACTIVE_ACK:    //ȡ����Աǿ�ƻ���Ӧ��

			break;

		case MIXER_MCU_ADDMEMBER_NACK:      //�����ԱӦ����Ϣ
		case MIXER_MCU_REMOVEMEMBER_NACK:   //ɾ����ԱӦ����Ϣ
		case MIXER_MCU_FORCEACTIVE_NACK:    //ǿ�Ƴ�Ա�����ܾ�
		case MIXER_MCU_CANCELFORCEACTIVE_NACK:   //ȡ����Աǿ�ƻ����ܾ�
		case MIXER_MCU_STARTMIX_NACK:       //�ܾ���ʼ����Ӧ��
		case MIXER_MCU_STOPMIX_NACK:        //�ܾ�ֹͣ����Ӧ��
		case MIXER_MCU_SETMIXDEPTH_NACK:    //���û�����Ⱦܾ���Ϣ
			break;

		default:
			ConfLog(FALSE, "Wrong message %u(%s) received in state %u in ProcMixerMcuRsp!\n", 
					pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState());
			break;
		}

		break;

	default:
		ConfLog(FALSE, "Wrong message %u(%s) received in state %u in ProcMixerMcuRsp!\n", 
				pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState());
		break;
	}

	return;
}

/*=============================================================================
  �� �� ���� MixerStatusChange
  ��    �ܣ� ����״̬֪ͨ, ֪ͨ���л�غ���ϯ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  07/11/07	  4.0		  ����		����Ϊ���л�����״̬�ı��֪ͨ
=============================================================================*/
void CMcuVcInst::MixerStatusChange(void)
{    
    //TMcu tMcu;
    //tMcu.SetMcuId(LOCAL_MCUID);
    
    CServMsg cServMsg;
    //cServMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) ); 
    u8 byMixerMode = m_tConf.m_tStatus.GetMixerMode();
    cServMsg.SetMsgBody( &byMixerMode, sizeof(u8));
    SendMsgToAllMcs(MCU_MCS_MIXPARAM_NOTIF, cServMsg);
    
    // guzh [11/7/2007] �Ƿ����ʡ�ԣ�
    //SendConfInfoToChairMt();

    EqpLog("[MixerStatusChange] MixerMode: %d\n", byMixerMode );
    return;
}

/*====================================================================
    ������      ��ProcMixerMcuRsp
    ����        ����������MCU֪ͨ��Ϣ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ProcMixerMcuNotif(const CMessage * pcMsg)
{
    if (STATE_ONGOING != CurState())
    {
        ConfLog(FALSE, "[ProcMixerMcuNotif] Wrong message %u(%s) received in state %u!\n", 
                pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState());
        return;
    }
    
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8 *pbyVACMember;
    u8 byMtId;
    TPeriEqpStatus tPeriEqpStatus;
    TMixerGrpStatus tMixerGrpStatus;
    TEqp tEqp;
    u8   byExciteChn;
    TPeriEqpRegReq tRegReq;
    
    switch (pcMsg->event)
    {
    case MIXER_MCU_ACTIVEMMBCHANGE_NOTIF:  //����������Ա�ı�֪ͨ

        //�õ����۳�Ա
        pbyVACMember = (u8 *)cServMsg.GetMsgBody();       

        byExciteChn = *(pbyVACMember + MAXNUM_MIXER_DEPTH);
        if (byExciteChn > 0 && byExciteChn <= MAXNUM_CONF_MT)
        {
            m_tVacLastSpeaker = m_ptMtTable->GetMt(byExciteChn);
        }
       
        //�����������Ƹı�
        VACChange(m_tConf.m_tStatus.m_tMixParam, byExciteChn);

        break;

    case MIXER_MCU_CHNNLVOL_NOTIF:      //ĳͨ������֪ͨ��Ϣ   			
        break;

    case MIXER_MCU_GRPSTATUS_NOTIF:     //������״̬֪ͨ
        tMixerGrpStatus = *(TMixerGrpStatus *)cServMsg.GetMsgBody();
        if (tMixerGrpStatus.m_byGrpId == m_byMixGrpId)
        {
            TMixerGrpStatus tOldMixerGrpStatus;
            g_cMcuVcApp.GetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);
            tOldMixerGrpStatus = tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId];
            tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId] = tMixerGrpStatus;
            tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpId = tOldMixerGrpStatus.m_byGrpId;
            tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpMixDepth = tOldMixerGrpStatus.m_byGrpMixDepth;
            tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byConfId = m_byConfIdx;
            // guzh [11/9/2007] ҵ�����л�
            tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = tOldMixerGrpStatus.m_byGrpState;

            g_cMcuVcApp.SetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);

            cServMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
            SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
        }

        break;

    case MIXER_MCU_MIXERSTATUS_NOTIF:
        {
            // guzh [11/6/2007] ���ݻ���������ǰ�Ĺ���״̬�����λָ�
            // �Ƿ��� ������Ȼ����VAC
            EqpLog("Last mixer mode = %d\n", m_tLastMixParam.GetMode());        
            if (mcuNoMix == m_tLastMixParam.GetMode())
            {
                return;
            }
            
            if (m_tLastMixParam.GetMode() == mcuWholeMix ||
                m_tLastMixParam.GetMode() == mcuVacWholeMix)
            {
                CServMsg cServMsg;
                cServMsg.SetEventId(MCS_MCU_STARTDISCUSS_REQ);
                ProcMixStart(cServMsg);
                
                // ��ǽ�������VAC
                if ( m_tLastMixParam.GetMode() == mcuVacWholeMix ) 
                {
                    m_tLastMixParam.SetMode(mcuVacMix);
                }
            }
            else if (m_tLastMixParam.GetMode() == mcuPartMix ||
                     m_tLastMixParam.GetMode() == mcuVacPartMix)
            {
                CServMsg cServMsg;

                //�ӱ�����б��л�ȡ
                for(u8 byLoop = 0; byLoop < m_tLastMixParam.GetMemberNum(); byLoop++)
                {
                    u8 byLastMixMtId = m_tLastMixParam.GetMemberByIdx(byLoop);
                    if ( byLastMixMtId != 0 &&
                         m_tConfAllMtInfo.MtJoinedConf(byLastMixMtId) )
                    {
                        TMt tMixMt;
                        tMixMt = m_ptMtTable->GetMt(m_tLastMixParam.GetMemberByIdx(byLoop));
                        cServMsg.CatMsgBody((u8 *)&tMixMt, sizeof(TMt));
                    }
                }
                m_tLastMixParam.ClearAllMembers();
                //m_tConf.m_tStatus.SetMixerParam(m_tLastMixParam);
                
                ProcMixStart(cServMsg);
                
                // ��ǽ�������VAC
                if ( m_tLastMixParam.GetMode() == mcuVacPartMix ) 
                {
                    m_tLastMixParam.SetMode(mcuVacMix);
                }            
            }
            else if (m_tLastMixParam.GetMode() == mcuVacMix )
            {
                CServMsg cServMsg;
                cServMsg.SetEventId(MCS_MCU_STARTVAC_REQ);
                MixerVACReq(cServMsg);
            }
            else
            {
                ConfLog(FALSE, "[MCU_MIXERCONNECTED_NOTIF] - ERROR mixer mode: %d\n\n", m_tLastMixParam.GetMode());
            }
            break;
        }

    case MCU_MIXERCONNECTED_NOTIF:   //�����������ɹ�֪ͨ
        tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();

        tEqp.SetMcuEqp(tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());

        EqpLog("m_tMixEqp.GetMtId(%d) m_tMixEqp.GetMcuId(%d) m_tMixEqp.GetType(%d)\n",
                                m_tMixEqp.GetMtId(), m_tMixEqp.GetMcuId(), m_tMixEqp.GetType());
        EqpLog("tEqp.GetMtId(%d) tEqp.GetMcuId(%d) tEqp.GetType(%d)\n",
                                tEqp.GetMtId(),tEqp.GetMcuId(), tEqp.GetType());        
        
        if (m_tMixEqp.GetEqpId() != tEqp.GetEqpId() ||
            m_tMixEqp.GetMcuId() != tEqp.GetMcuId() ||
            m_tMixEqp.GetType() != tEqp.GetType())
        {
            return;
        }

        /*
        // guzh [11/6/2007] ���ݻ���������ǰ�Ĺ���״̬�����λָ�
        // �Ƿ��� ������Ȼ����VAC
        EqpLog("Last mixer mode = %d\n", m_byLastMixerMode);        
        if (mcuNoMix == m_byLastMixerMode)
        {
            return;
        }

        if (m_byLastMixerMode == mcuWholeMix ||
            m_byLastMixerMode == mcuVacWholeMix)
        {
            CServMsg cServMsg;
            cServMsg.SetEventId(MCS_MCU_STARTDISCUSS_REQ);
            ProcMixStart(cServMsg);

            // ��ǽ�������VAC
			if ( m_byLastMixerMode == mcuVacWholeMix ) 
			{
				m_byLastMixerMode = mcuVacMix;
			}
        }
        else if (m_byLastMixerMode == mcuPartMix ||
                 m_byLastMixerMode == mcuVacPartMix)
        {
            CServMsg cServMsg;
            //�ӱ�����б��л�ȡ
            TMixParam tLastParam = m_tConf.m_tStatus.GetMixerParam();
            for(u8 byLoop = 0; byLoop < tLastParam.GetMemberNum(); byLoop++)
            {
                if ( tLastParam.GetMemberByIdx(byLoop) != 0 )
                {
                    TMt tMixMt;
                    tMixMt = m_ptMtTable->GetMt(tLastParam.GetMemberByIdx(byLoop));
                    cServMsg.CatMsgBody((u8 *)&tMixMt, sizeof(TMt));
                }
            }
            tLastParam.ClearAllMembers();
            m_tConf.m_tStatus.SetMixerParam(tLastParam);

            ProcMixStart(cServMsg);
            
            // ��ǽ�������VAC
            if ( m_byLastMixerMode == mcuVacPartMix ) 
            {
                m_byLastMixerMode = mcuVacMix;
			}            
        }
        else if (m_byLastMixerMode == mcuVacMix )
        {
            CServMsg cServMsg;
            cServMsg.SetEventId(MCS_MCU_STARTVAC_REQ);
            MixerVACReq(cServMsg);
        }
        else
        {
            ConfLog(FALSE, "[MCU_MIXERCONNECTED_NOTIF] - ERROR mixer mode: %d\n\n", m_byLastMixerMode);
        }
        */
        break;

    case MCU_MIXERDISCONNECTED_NOTIF:   //����������֪ͨ

        tEqp = *(TEqp *)cServMsg.GetMsgBody();

        //���Ǳ�����Ļ���������
        if (tEqp.GetEqpId() != m_tMixEqp.GetEqpId())
        {
            return;
        }

        // guzh [11/7/2007] ������������������������ն��б�����գ�����TMtStatus
        m_tLastMixParam.SetMode(m_tConf.m_tStatus.GetMixerMode());

        //�ı����״̬
        if (m_tConf.m_tStatus.IsVACing())
        {
            //֪ͨ��ϯ�����л��
            SendMsgToAllMcs(MCU_MCS_STOPVAC_NOTIF, cServMsg);
            if (HasJoinedChairman())
            {
                SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cServMsg);
            }
            //ֹͣ��������������ݣ����ָ������㲥Դ
            for (byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
            {
                if(m_tConfAllMtInfo.MtJoinedConf(byMtId))            
                {
                    StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
							            (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(byMtId)), FALSE, MODE_AUDIO);
                    
                    // xsl [7/29/2006]
                    m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);
                }
            }
        }
		
        if(m_tConf.m_tStatus.IsMixing())//����(����)
        {
            //֪ͨ��ϯ�����л�� 
            TMcu tMcu;
            tMcu.SetMcuId(LOCAL_MCUID);
            cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
            if (!m_tCascadeMMCU.IsNull())
            {
                cServMsg.SetDstMtId(m_tCascadeMMCU.GetMtId());
                SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_STOPMIXER_NOTIF, cServMsg);
            }
            if (HasJoinedChairman())
            {
                SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STOPDISCUSS_NOTIF, cServMsg);
            }

            //ֹͣ��������������ݣ����ָ������㲥Դ
            TMixParam tMixParam = m_tConf.m_tStatus.GetMixerParam();
            
            //zbq[04/10/2008] ���ƻ�����������ն��б�
            if (mcuPartMix == tMixParam.GetMode() ||
                mcuVacPartMix == tMixParam.GetMode() )
            {
                for(u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
                {
                    if (m_tConfAllMtInfo.MtJoinedConf(byMtIdx) &&
                        m_ptMtTable->IsMtInMixing(byMtIdx))
                    {
                        m_tLastMixParam.AddMember(byMtIdx);
                    }
                }
            }

            tMixParam.ClearAllMembers();
            for (byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
            {
                if (m_tConfAllMtInfo.MtJoinedConf(byMtId))            
                {
                    StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
							            (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(byMtId)), FALSE, MODE_AUDIO );

                    StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);	

                    // xsl [9/21/2006] �ָ����鲥��ַ����
                    if (m_tConf.GetConfAttrb().IsSatDCastMode() && m_ptMtTable->IsMtInMixGrp(byMtId))
                    {
                        ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_AUDIO);
                    }

                    // xsl [7/29/2006]
                    m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);

                    if (m_ptMtTable->IsMtInMixing(byMtId))            
                    {
                        m_ptMtTable->SetMtInMixing(byMtId, FALSE);

                        // guzh [11/7/2007] ���浽��ʱ�б��У��Ȼ�������������
                        tMixParam.AddMember(byMtId);
                        
                        u8 byMix = 0;
                        cServMsg.SetMsgBody((u8*)&byMix, 1);
                        SendMsgToMt(byMtId, MCU_MT_MTADDMIX_NOTIF, cServMsg);
                    }
                }                
            }

            MtStatusChange();

            //�ı���ƵԴ
            if (HasJoinedSpeaker())
            {
                TMt tSpeakerMt = GetLocalSpeaker();
                ChangeAudBrdSrc(&tSpeakerMt);
            }
            else
            {
                ChangeAudBrdSrc(NULL);
            }

            //���ϼ�MCU���ն�����
            if (!m_tCascadeMMCU.IsNull())
            {
                TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
                if (ptInfo != NULL&& 
                    m_tConfAllMtInfo.MtJoinedConf(ptInfo->m_tSpyMt.GetMcuId(), ptInfo->m_tSpyMt.GetMtId()))
                {
                    StartSwitchToSubMt(ptInfo->m_tSpyMt, 0, m_tCascadeMMCU.GetMtId(), MODE_AUDIO, SWITCH_MODE_SELECT);
                }
            }
        }

        //zbq[12/05/2007] ͣ����
        if ( ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
        {
            CServMsg cMsg;
            RollCallStop(cMsg);
        }
        
        //����������
        g_cMcuVcApp.GetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);
        tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = TMixerGrpStatus::READY;
        g_cMcuVcApp.SetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);

        // �������������״̬
        m_tConf.m_tStatus.SetVACing(FALSE);
        m_tConf.m_tStatus.SetNoMixing();
        MixerStatusChange();

        break; 

    default:
        break;
    }

#ifdef _SATELITE_
    RefreshConfState();
#endif

    return;
}
		
/*====================================================================
    ������      ��ProcMcsMcuAddMixMemberCmd
    ����        ��������Ҫ�����ӻ�����Ա����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/10/14    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuAddMixMemberCmd( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
    
    STATECHECK;

    if ( m_tConf.m_tStatus.IsSpecMixing() )
    {
        TMt  * ptMt  = (TMt *)(cServMsg.GetMsgBody());;
        u8     byMtNum = (cServMsg.GetMsgBodyLen()) / sizeof(TMt);
        AddRemoveSpecMixMember( ptMt, byMtNum );
    }
    else
    {
        ProcMixStart(cServMsg);
    }
	return;
}

/*====================================================================
    ������      ��ProcMcsMcuRemoveMixMemberCmd
    ����        ��������Ҫ���Ƴ�������Ա����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/10/14    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuRemoveMixMemberCmd( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
    
    STATECHECK;
    
    TMt  *ptMt  = (TMt*)(cServMsg.GetMsgBody());
    u8 byMtNum  = (cServMsg.GetMsgBodyLen())/sizeof(TMt);
    AddRemoveSpecMixMember( ptMt, byMtNum, FALSE );

    return;
}

/*====================================================================
    ������      ��ProcMcsMcuReplaceMixMemberCmd
    ����        ��������Ҫ���滻������Ա����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/11/17    4.0         �ű���         ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuReplaceMixMemberCmd( const CMessage * pcMsg )
{
    STATECHECK;

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TMt tOldMt = *(TMt*)cServMsg.GetMsgBody();
    TMt tNewMt = *(TMt*)(cServMsg.GetMsgBody() + sizeof(TMt));

    if ( tNewMt.IsNull() )
    {
        ConfLog(FALSE, "[ReplaceMixMemberCmd] tNewMt.IsNull, ignore it\n");
        return;
    }
    if ( tOldMt.IsNull() )
    {
        ConfLog(FALSE, "[ReplaceMixMemberCmd] tOldMt.IsNull, ignore it\n");
        return;
    }
    if (!m_tConf.m_tStatus.IsSpecMixing())
    {
        ConfLog(FALSE, "[ReplaceMixMemberCmd] conf mode.%d, unexpected opr, ignore it\n", m_tConf.m_tStatus.GetMixerMode());
        return;
    }
    if (tOldMt.IsLocal() &&
        !m_ptMtTable->IsMtInMixing(tOldMt.GetMtId()))
    {
        ConfLog(FALSE, "[ReplaceMixMemberCmd] tOldMt.%d isn't in mixing, ignore it\n", tOldMt.GetMtId());
        return;
    }

    AddRemoveSpecMixMember(&tOldMt, 1, FALSE, TRUE);
    AddRemoveSpecMixMember(&tNewMt, 1, TRUE);
    
    return;
}

/*====================================================================
    ������      ��AddRemoveSpecMixMember
    ����        ������Ƴ�������Ա
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt *ptMt ��Ա�ն�
                  u8 byMtNum ��Ա����
				  BOOL32 bAdd TRUE-��� FALSE-�Ƴ�
                  BOOL32 bStopMixNonMem: �Ƴ��ն� �ƿ��Ƿ�ͣ����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         ������         ����
====================================================================*/
void CMcuVcInst::AddRemoveSpecMixMember(TMt *ptMt, u8 byMtNum, BOOL32 bAdd, BOOL32 bStopMixNonMem)
{
    u8  byLoop;
    CServMsg cServMsg;
    
    if (!m_tConf.m_tStatus.IsSpecMixing())
    {
        ConfLog(FALSE, "[AddRemoveSpecMix] conf mode.%d, unexpected opr, ignore it\n", m_tConf.m_tStatus.GetMixerMode());
        return;
    }
    
    for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && m_ptMtTable->IsMtInMixing(byLoop))            
        {
            if (NULL == ptMt)
            {
                u8 byMix = 1;
                cServMsg.SetMsgBody((u8 *)&byMix, sizeof(u8));
                SendMsgToMt(byLoop, MCU_MT_MTADDMIX_NOTIF, cServMsg);
            }
        }
    }

    if (NULL != ptMt)
    {
        // xsl [8/4/2006] ��Ϊadd���жϵ�ǰ�Ƿ񳬹�������ͨ����
        if (bAdd && GetMixMtNumInGrp() >= GetMaxMixNum())
        {
            ConfLog(FALSE, "[AddRemoveSpecMixMember] current mixing mt num exceed %d, ingnore mt.%d\n", 
                    GetMaxMixNum(), ptMt->GetMtId());
            NotifyMcsAlarmInfo(0, ERR_MCU_OVERMAXMIXERCHNNL);
            return;
        }
        // guzh [1/30/2007] 8000B ��������
        u16 wError = 0;
        if ( bAdd && 
             !CMcuPfmLmt::IsMixOprSupported(m_tConf, GetMixMtNumInGrp(), byMtNum, wError) )
        {
            NotifyMcsAlarmInfo(0, wError);
            return;
        }

        TMt atAllMixMt[MAXNUM_MIXING_MEMBER];
        TMt * ptAllMt = &atAllMixMt[0];
        memcpy( ptAllMt, ptMt, byMtNum * sizeof(TMt) );
        
        TMtListDiscard tOtherMcuMtList;

        for (byLoop = 0; byLoop < byMtNum; byLoop++)
        {
            EqpLog("[AddRemoveSpecMixMember] nIndex.%d Mt<%d,%d> bAdd.%d\n",
                    byLoop+1, ptAllMt->GetMcuId(), ptAllMt->GetMtId(), bAdd);

            if (ptAllMt->IsLocal())
            {
                TMt tMt = GetLocalMtFromOtherMcuMt(*ptAllMt);
                u8 byMix = bAdd ? 1 : 0; 
                m_ptMtTable->SetMtInMixing(tMt.GetMtId(), bAdd);

                ptAllMt++;            
                
                cServMsg.SetMsgBody((u8 *)&byMix, 1);
                SendMsgToMt(tMt.GetMtId(), MCU_MT_MTADDMIX_NOTIF, cServMsg);
                
                // xsl [8/4/2006] ���ڸı������Աʱ����
                if (bAdd)
                {
                    if (!m_ptMtTable->IsMtInMixGrp(tMt.GetMtId()))
                    {
                        // zbq [05/14/2007] �Ƿ������л����µĻ�����Ա���ӣ����ж���
                        // ��Ա��Ч�ԣ����򽫽����������Ѳ��������������������ֹ�󽨡�
                        TLogicalChannel tLogicChan;
                        if ( m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tLogicChan, TRUE) )
                        {
                            AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE);
                            StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
                                                 (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())),
                                                 MODE_AUDIO, SWITCH_MODE_SELECT);
                        }
                    }
                }
                else 
                {
                    if (m_ptMtTable->IsMtInMixGrp(tMt.GetMtId()))
                    {
                        RemoveMixMember(&tMt, FALSE);
                        StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
                                            (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())),
                                            FALSE, MODE_AUDIO );
                        g_cMpManager.StopSwitchToSubMt(tMt, MODE_AUDIO);    // xsl [8/19/2006] ֹͣ���ն˵Ľ���
                        SwitchMixMember(&tMt, TRUE);
                    }                
                }
            }
            else
            {   
                tOtherMcuMtList.SetNewMt(*ptAllMt);
            }
        }

        u8  byGrpNum = 0;
        TMt atMt[MAXNUM_SUB_MCU * MAXNUM_MIXING_MEMBER];
        u8  abyMixNum[MAXNUM_SUB_MCU] = { 0 };
        TMt *ptMixMt = &atMt[0];
        u8  *pbyMixNum = &abyMixNum[0];

        tOtherMcuMtList.GetMtList( sizeof(atMt), byGrpNum, pbyMixNum, ptMixMt );

        if ( 0 != byGrpNum )
        {
            for( u8 byGrpIdx = 0; byGrpIdx < byGrpNum; byGrpIdx ++)
            {
                TMcu tMcu;
                tMcu.SetMcu(ptMixMt->GetMcuId());
                
                CServMsg cMsg;
                cMsg.SetMsgBody((u8*)&tMcu, sizeof(TMcu));
                cMsg.CatMsgBody((u8*)ptMixMt, (sizeof(TMt) * *pbyMixNum));

                //zbq[11/29/2007] ���ӱ�ʶ���������Ƿ�Ϊ�滻���ӣ��Ǽ������滻.
                u8 byReplace = 0;
                if ( !m_ptMtTable->IsMtInMixing(ptMixMt->GetMcuId()) )
                {
                    byReplace = 1;
                }
                cMsg.CatMsgBody((u8*)&byReplace, sizeof(u8));

                if ( *pbyMixNum > 0 )
                {
                    OnAddRemoveMixToSMcu(&cMsg, bAdd);
                }
                ptMixMt += *pbyMixNum;
                pbyMixNum ++ ;
            }
        }
        else
        {
            EqpLog("[AddRemoveSpecMix] No other mcu mem exist, ignore\n");
        }

        // guzh [11/7/2007] 
        /*
        TMcu tMcu;
        tMcu.SetMcuId(LOCAL_MCUID);
        cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
        cServMsg.CatMsgBody((u8*)&tMixParam, sizeof(TMixParam));
        SendMsgToAllMcs(MCU_MCS_MIXPARAM_NOTIF, cServMsg);
        if (!m_tCascadeMMCU.IsNull())
        {
            cServMsg.SetDstMtId(m_tCascadeMMCU.GetMtId());            
            SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_MIXERPARAM_NOTIF, cServMsg);
        }
        */
    }

    // xsl [7/31/2006] �ն˻���״̬֪ͨ�ϼ�mcu
    if (!m_tCascadeMMCU.IsNull())
    {
        OnNtfMtStatusToMMcu( m_tCascadeMMCU.GetMtId());
    }

    /*
    // xsl [8/24/2006] �µĶ��ƻ�������������֪ͨ��ǰ���������ͨ��
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8*)&m_byMixGrpId, 1);
    cServMsg.CatMsgBody(abyMtInMixing, sizeof(abyMtInMixing));
    SendMsgToEqp(m_tMixEqp.GetEqpId(), MCU_MIXER_SETMEMBER_CMD, cServMsg);
    */

    u8 byMtIdx = 0;

    // zbq [11/22/2007] ����ˢ״̬�ģ���ѭ��ʱˢ�¼�����������ʵʱ�ϱ�
    if ( NULL == ptMt )
    {
        MtStatusChange( 0, TRUE );
    }
    else
    {
        for( byMtIdx = 0; byMtIdx < byMtNum; byMtIdx++ )
        {
            TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptMt);
            MtStatusChange( tLocalMt.GetMtId(), TRUE );
            ptMt ++;
        }
    }
    
    // zbq [11/01/2007] �ָ�֧�� ���ն�ͣ��������
    // zbq [06/27/2007] �޳�Աͣ�������� �ݲ�֧��
    // guzh [4/19/2007] �����ǰû���ն˲����������ֹͣ����
    u8 byCurMixNum = 0;
    for( byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
    {
        if ( m_ptMtTable->IsMtInMixing(byMtIdx) )
        {
            byCurMixNum ++;
        }
    }
    if ( 0 == byCurMixNum && !bStopMixNonMem )
    {
        CServMsg cServMsg;
        ProcMixStop(cServMsg);
    }

    if ( 0 == byMtNum && !bAdd )
    {
        CServMsg cServMsg;
        ProcMixStop(cServMsg);        
    }
    return;
}

/*====================================================================
    ������      ��ProcMtMcuApplyMixNotify
    ����        �������ն�Ҫ��μӻ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/10/14    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ProcMtMcuApplyMixNotify( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	TMt			tChairman;

	switch( CurState() )
	{
	case STATE_ONGOING:

		//δ���
		//[8/6/2008 xueliang] ���ӹ��ˣ�����δ������Ƶ����ͨ��������ն�����廰����֮
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) || 
			m_tConf.m_tStatus.IsNoMixing() ||
			!m_ptMtTable->IsLogicChnnlOpen(tMt.GetMtId(), LOGCHL_AUDIO, FALSE)
			 
			)
		{
			return;
		}

		//δ���ڶ��ƻ���ģʽ��
		if( !m_tConf.m_tStatus.IsSpecMixing() )
		{
			return;
		}	
		
		// ��ѯ�Ƿ��Ѿ����ڻ�������
		if( LOCAL_MCUID != tMt.GetMcuId() || 
			TRUE == m_ptMtTable->IsMtInMixing( tMt.GetMtId() ) )
		{
			return;
		}

		//����ϯ
		if( HasJoinedChairman() )
		{
			tChairman = m_tConf.GetChairman();
			
    
			if( tMt.GetMtId() == tChairman.GetMtId() )
			{
				//��ϯ����廰,ͬ��
				AddRemoveSpecMixMember( &tChairman, 1, TRUE );
				return;
			}
			else
			{
				//֪ͨ��ϯ
				cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
				cServMsg.SetDstMtId( tChairman.GetMtId() );			
				SendMsgToMt( tChairman.GetMtId(), MCU_MT_MTAPPLYMIX_NOTIF, cServMsg );			
			}
		}

		//֪ͨ���
		cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
		SendMsgToAllMcs( MCU_MCS_MTAPPLYMIX_NOTIF, cServMsg );

		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMixStart
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/11/09    4.0         �ű���          ����
====================================================================*/
void CMcuVcInst::ProcMixStart( CServMsg &cServMsg )
{
#define REJECT_MIXER_START( wError )	\
	{ \
		if ( cServMsg.GetEventId() != MCU_MCU_STARTMIXER_CMD && \
			 cServMsg.GetEventId() != MCS_MCU_ADDMIXMEMBER_CMD ) \
		{ \
			cServMsg.SetErrorCode( wError );	\
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2); \
		} \
		else \
		{ \
			NotifyMcsAlarmInfo( 0, wError ); \
		} \
	}

    u8   byMixDepth;
    u8   byLoop;
    TMt *ptMt = NULL;
    u8   byMtIdx = 0;

    //�Ѵ���ǿ�ƹ㲥״̬��NACK
    if ( m_tConf.m_tStatus.IsMustSeeSpeaker() ) 
    {
        ConfLog(FALSE, "[MixerCommonReq] IsMustSeeSpeaker is true. nack\n");
        REJECT_MIXER_START(0);
        return;
    }

    s8 *lpMsgBody = NULL;
    u8  byMtNum = 0;
    
    //zbq[11/26/2007] �������ơ����ܻ���������ˢ���¼�֧��
    BOOL32 bMMcuMix = FALSE;

    if (MCU_MCU_STARTMIXER_CMD == cServMsg.GetEventId())
    {
        lpMsgBody = (s8*)(cServMsg.GetMsgBody() + sizeof(TMcu) + sizeof(u8));
        byMtNum = (cServMsg.GetMsgBodyLen()-sizeof(TMcu)-sizeof(u8))/sizeof(TMt);
        bMMcuMix = TRUE;
    }
    else
    {
        lpMsgBody = (s8*)cServMsg.GetMsgBody();
        byMtNum = cServMsg.GetMsgBodyLen()/sizeof(TMt);
    }

    //zbq[11/29/2007] ��Ӧ���ܻ����Ͷ��ƻ����໥�л�֧��
    BOOL32 bReplaceMixMode = FALSE;
    if ( byMtNum == 0 && m_tConf.m_tStatus.IsSpecMixing() )
    {
        NotifyMcsAlarmInfo( 0, ERR_MCU_SPECMIX_CANCEL_NTF );
        bReplaceMixMode = TRUE;
    }
    else if ( byMtNum > 0 && m_tConf.m_tStatus.IsAutoMixing() )
    {
        NotifyMcsAlarmInfo( 0, ERR_MCU_AUTOMIX_CANCEL_NTF );
        bReplaceMixMode = TRUE;
    }

    ptMt = (TMt*)lpMsgBody;

    if (MCU_MCU_STARTMIXER_CMD == cServMsg.GetEventId())
    {
        //���۵Ļ�����ȱ������1
        byMixDepth = *(u8*)(cServMsg.GetMsgBody()+sizeof(TMcu));
        if (byMixDepth < 1)
        {
            //FIXME��Ѱ���µĴ�����
            //cServMsg.SetErrorCode( ERR_MCU_POOLMEMBERNUM );
            //SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            return;
        }
        else
        {
            if( byMixDepth == 1)
            {
                byMixDepth = 2;
            }
        }
    }
    // guzh [11/7/2007] ���Ի����������
    byMixDepth = GetMaxMixerDepth();

	u8 byEqpId = 0;
	u8 byGrpId = 0;
	if( !bReplaceMixMode &&
        !m_tConf.m_tStatus.IsVACing() && 
		0 == g_cMcuVcApp.GetIdleMixGroup( byEqpId, byGrpId ) )
	{
		REJECT_MIXER_START( ERR_MCU_NOIDLEMIXER_INCONF )
		return;
	}

	//����ָ����Ա����ʱ�Ļ�����Ա
    u8  byJoinedMtNum = m_tConfAllMtInfo.GetLocalJoinedMtNum();

    u16 wError = 0;
    if ( ! CMcuPfmLmt::IsMixOprSupported( m_tConf, GetMixMtNumInGrp(), 
                                          ( byMtNum == 0 ? byJoinedMtNum : byMtNum ) , wError ) )
    {
        REJECT_MIXER_START(wError);
        return;
    }

    TMtListDiscard tOtherMcuMtList;
    
	if( byMtNum > 0 )
	{
        // xsl [7/24/2006] ���ز��������������붨�ƻ�������; �������ϼ������ȼ�.
        if (m_tConf.m_tStatus.IsVACing())
        {
            if ( bMMcuMix )
            {
                NotifyMcsAlarmInfo(0, ERR_MCU_MMCUSPECMIX_VAC);
                ConfLog( FALSE, "[MixerCommonReq] Local VAC mode has been canceled due to cas specmixing\n" );
                
                //FIXME��MixParamNtf
                CServMsg cMsg;
                cMsg.SetEventId(MCU_MCS_STOPVAC_NOTIF);
                SendMsgToAllMcs(MCU_MCS_STOPVAC_NOTIF, cMsg);
                if ( HasJoinedChairman() )
                {
                    cMsg.SetEventId(MCU_MT_STOPVAC_NOTIF);
                    SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cMsg);
                }
            }
            else
            {
                ConfLog(FALSE, "[MixerCommonReq] IsVACMode is true. nack\n"); 
                REJECT_MIXER_START(ERR_MCU_VACMUTEXSPECMIX)
                return;
            }
        }

        //zbq[11/29/2007] �������ϼ�MCU�����ػ�����
        if( byMtNum > 0 && bMMcuMix && !m_ptMtTable->IsMtInMixing(m_tCascadeMMCU.GetMtId()) )
        {
            m_ptMtTable->SetMtInMixing(m_tCascadeMMCU.GetMtId(), TRUE);
        }

        // xsl [8/4/2006] ���ƻ������Ϊ56��
        if (byMtNum > GetMaxMixNum())
        {
            ConfLog(FALSE, "[MixerCommonReq] spec MtNum(%d) can't exceed %d. nack\n", 
                            byMtNum, GetMaxMixNum());
            REJECT_MIXER_START(ERR_MCU_OVERMAXMIXERCHNNL)
			return;
        }

        // zbq [11/26/2007] �������ƻ����屾�ض��ƻ���
        if ( bMMcuMix &&
             m_tConf.m_tStatus.IsSpecMixing() )
        {
            for( byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
            {
                if ( m_tConfAllMtInfo.MtInConf(byMtIdx) &&
                     m_ptMtTable->IsMtInMixing(byMtIdx))
                {
                    TMt tMt = m_ptMtTable->GetMt(byMtIdx);
                    AddRemoveSpecMixMember(&tMt, 1, FALSE, TRUE);
                }
            }
        }
            
		for( byLoop = 0; byLoop < byMtNum; byLoop++ )
		{
            TMt tMt = *ptMt;
            if ( tMt.IsLocal() )
            {
                //ֱ�����ӱ����ն�
                if ( !m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
                {
                    continue;
                }
                // zbq [11/26/2007] �������Ƴ屾�����ƣ��˴���ɱ����ն˵�����
                if ( bMMcuMix &&
                     m_tConf.m_tStatus.IsSpecMixing() )
                {
                    AddRemoveSpecMixMember( &tMt, 1 );
                }
                else
                {
                    m_ptMtTable->SetMtInMixing( tMt.GetMtId(), TRUE );
                }
            }
            else
            {
                //���ӻش�ͨ��
                tMt = GetLocalMtFromOtherMcuMt( *ptMt );
                if ( !m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
                {
                    continue;
                }
                m_ptMtTable->SetMtInMixing( tMt.GetMtId(), TRUE );

                //������Ӧ�¼�MCU�Ļ���
                TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMt->GetMcuId());
                if(ptMcInfo == NULL)
                {
                    ConfLog(FALSE, "[ProcMixStart] SMcu.%d unexist, impossible\n", ptMt->GetMcuId());
                    continue;
                }
                tOtherMcuMtList.SetNewMt(*ptMt);
            }
			ptMt++;
		}

        u8  byGrpNum = 0;
        TMt atMt[MAXNUM_SUB_MCU * MAXNUM_MIXING_MEMBER];
        u8  abyMixNum[MAXNUM_SUB_MCU] = { 0 };
        TMt *ptMt = &atMt[0];
        u8  *pbyMixNum = &abyMixNum[0];
        
        tOtherMcuMtList.GetMtList( sizeof(atMt), byGrpNum, pbyMixNum, ptMt );
        
        if ( 0 != byGrpNum )
        {
            for( u8 byGrpIdx = 0; byGrpIdx < byGrpNum; byGrpIdx ++)
            {
                TMcu tMcu;
                tMcu.SetMcu(ptMt->GetMcuId());
                
                CServMsg cMsg;
                cMsg.SetEventId(MCU_MCU_ADDMIXMEMBER_CMD);
                cMsg.SetMsgBody((u8*)&tMcu, sizeof(TMcu));
                cMsg.CatMsgBody((u8*)ptMt, (sizeof(TMt) * *pbyMixNum));

                //zbq[11/29/2007] ���ӱ�ʶ���������Ƿ�Ϊ�滻���ӣ��Ǽ������滻.
                u8 byReplace = 1;
                cMsg.CatMsgBody((u8*)&byReplace, sizeof(u8));
                
                if ( cMsg.GetMsgBodyLen() > sizeof(TMcu)+sizeof(u8) )
                {
                    OnAddRemoveMixToSMcu(&cMsg, TRUE);
                    //OnStartMixToSMcu(&cMsg);
                }
                ptMt += *pbyMixNum;
                pbyMixNum ++ ;
            }
        }
        else
        {
            EqpLog("[AddRemoveSpecMix] No other mcu mem exist, ignore\n");
        }
	}
	else
	{
        // xsl [8/19/2006] ���Ƿ�ɢ���鲻֧�ֻ�������
        if (m_tConf.GetConfAttrb().IsSatDCastMode())
        {
		    REJECT_MIXER_START( ERR_MCU_DCAST_NOOP )            
            ConfLog( FALSE, "[MixerCommonReq] distributed conf not supported discuss\n");
            return;
        }

        // xsl [8/4/2006] �������۲��ܳ���56���ն�                
        if (byJoinedMtNum > GetMaxMixNum())
        {
            ConfLog(FALSE, "[MixerCommonReq] discuss MtNum(%d) can't exceed %d. nack\n", 
                    byJoinedMtNum, GetMaxMixNum());
            REJECT_MIXER_START(ERR_MCU_OVERMAXMIXERCHNNL)
			return;
        }

        // xsl [1/10/2006] ����������ֱ���¼�MCUȫ�����                 
        TMcu tMcu;
        tMcu.SetMcuId(LOCAL_MCUID);	
        CServMsg cMsg;
        cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) ); 
        cMsg.CatMsgBody( (u8*)&byMixDepth, 1 );
        
        OnStartDiscussToAllSMcu( &cMsg );
	}

    // zbq [06/27/2007] �����ۻ��ƻ���ͣ��ǰ��������ն˵���Ƶѡ��
    for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
    {
        if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) )
        {
            TMtStatus tMtStatus;
            if ( m_ptMtTable->GetMtStatus(byMtId, &tMtStatus) &&
                 !tMtStatus.GetSelectMt(MODE_AUDIO).IsNull() )
            {
                TMt tMt = m_ptMtTable->GetMt(byMtId);
                StopSelectSrc(tMt, MODE_AUDIO);
            }
        }
    }

    //ACK
    if ( cServMsg.GetEventId() != MCU_MCU_STARTMIXER_CMD &&
         cServMsg.GetEventId() != MCS_MCU_ADDMIXMEMBER_CMD )
    {
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
    }

	//��ʼ����            
	if (m_tConf.m_tStatus.IsVACing() &&
        m_tConf.m_tStatus.IsNoMixing())
	{
        //֪ͨ��ϯ�����л��
        if (byMtNum > 0)
        {
            m_tConf.m_tStatus.SetSpecMixing();
            m_tConf.m_tStatus.SetVACing(FALSE);
        }
        else
        {
            m_tConf.m_tStatus.SetAutoMixing();
        }

        TMcu tMcu;
        tMcu.SetMcu(LOCAL_MCUID);					
        cServMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) ); 
        cServMsg.CatMsgBody( (u8*)&byMixDepth, 1 );
        if(!m_tCascadeMMCU.IsNull())
        {
			cServMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
            SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_STARTMIXER_NOTIF, cServMsg);						
        }
        if (HasJoinedChairman())
        {
            cServMsg.SetMsgBody((u8*)&byMixDepth, 1);
            SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STARTDISCUSS_NOTIF, cServMsg);
        }

        //���Ӷ��ƻ�����Ա
        for( byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
        {
            if ( m_tConfAllMtInfo.MtJoinedConf(byMtIdx) &&
                 m_ptMtTable->IsMtInMixing(byMtIdx))
            {
                TMt tMt = m_ptMtTable->GetMt(byMtIdx);
                AddRemoveSpecMixMember(&tMt, 1);
            }
        }

        //��ӻ�����Ա, ����������������mt
        for(u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
        {
            if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
            {
                TMt tMt = m_ptMtTable->GetMt(byMtId);
                StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);	
                SwitchMixMember(&tMt);
            }
        }

        //֪ͨ��������ʼ���ն˷�����
        NotifyMixerSendToMt(TRUE);

        MixerStatusChange();
	}
    else if (m_tConf.m_tStatus.IsSpecMixing())
    {
        //zbq[11/26/2007] ���ػ������ܻ��� �� ����
        if ( byMtNum == 0 )
        {
            //�Ƴ���ǰ���еĶ��ƻ�����Ա
            for( byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
            {
                if ( m_tConfAllMtInfo.MtInConf(byMtIdx) &&
                     m_ptMtTable->IsMtInMixing(byMtIdx))
                {
                    TMt tMt = m_ptMtTable->GetMt(byMtIdx);
                    AddRemoveSpecMixMember(&tMt, 1, FALSE, TRUE);
                }
            }
            
            m_tConf.m_tStatus.SetAutoMixing(TRUE);

            //��ӻ�����Ա, ����������������
            u8 byMixMtNum = 0;
            for (byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
            {
                if (m_tConfAllMtInfo.MtJoinedConf(byMtIdx))
                {
                    TMt tMt = m_ptMtTable->GetMt(byMtIdx);    
                    
                    if (byMixMtNum < GetMaxMixNum())
                    {
                        //֪ͨmixer����ͨ������vacģʽ��mixer����������mt���������н���                
                        AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE);
                        
                        StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
                                             (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())), 
                                             MODE_AUDIO, SWITCH_MODE_SELECT);
                        byMixMtNum++;
                    }  
                }
            }
            
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                g_cMpManager.StartDistrConfCast(m_tMixEqp, MODE_AUDIO);
            }

            //����Nģʽ�Ľ���
            for (byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
            {
                if (m_tConfAllMtInfo.MtJoinedConf(byMtIdx) &&
                    !m_ptMtTable->IsMtInMixGrp(byMtIdx))
                {
                    TMt tMt = m_ptMtTable->GetMt(byMtIdx);
                    SwitchMixMember(&tMt, TRUE);
                }           
            }
        }
        else
        {
            // ���ػ������ƻ����嶨�ƣ������Ѿ����.
        }
        MixerStatusChange();
    }
    else if(m_tConf.m_tStatus.IsAutoMixing())
    {
        //zbq[11/26/2007] ���ػ������ƻ��� �� ����
        if ( byMtNum > 0 )
        {
            m_tConf.m_tStatus.SetSpecMixing(TRUE);

            //����ģʽ����������ָ���ǰ�Ľ���
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

            //ͣ��������
            //zbq[12/06/2007] ��Բ�ͬ����������Ƿ�ͣ�¼��Ļ���
            CServMsg cMsg;
            cMsg.SetEventId(MCU_MCU_STOPMIXER_CMD);
            
            TConfMcInfo* ptMcInfo = NULL;
            for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
            {
                ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
                if( ( 0 == ptMcInfo->m_byMcuId ) || 
                    ( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) ||
                    tOtherMcuMtList.IsMcuMixing(ptMcInfo->m_byMcuId) )
                {
                    continue;
                }
                cMsg.SetDstMtId( ptMcInfo->m_byMcuId );
                SendMsgToMt(ptMcInfo->m_byMcuId, MCU_MCU_STOPMIXER_CMD, cMsg);
            }

            // ���Ӷ��ƻ�����Ա
            for( byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
            {
                if ( m_tConfAllMtInfo.MtJoinedConf(byMtIdx) &&
                     m_ptMtTable->IsMtInMixing(byMtIdx))
                {
                    TMt tMt = m_ptMtTable->GetMt(byMtIdx);
                    AddRemoveSpecMixMember(&tMt, 1);
                }
            }
            // FIXME: ���û�������ϼ�MCUΪ������Ա���������ֶ�����֮

            //����Nģʽ�Ľ���
            for (byMtIdx = 1; byMtIdx < MAXNUM_CONF_MT; byMtIdx++)
            {
                if (m_tConfAllMtInfo.MtJoinedConf(byMtIdx))
                {
                    TMt tMt = m_ptMtTable->GetMt(byMtIdx);
                    if (!m_ptMtTable->IsMtInMixing(byMtIdx))
                    {
                        SwitchMixMember(&tMt, TRUE);
                    }
                }           
            }
        }
        else
        {
            // ���¼����ܻ�������Ȼ����. �Ҽ���͸�������Ѿ����
        }
        MixerStatusChange();
    }
	else
	{
        StartMixing( byMtNum > 0 ? mcuPartMix : mcuWholeMix );    //��ʼ����
	}

    return;
#undef REJECT_MIXER_START
}

/*====================================================================
    ������      ��ProcMixStop
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/11/09    4.0         �ű���          ����
====================================================================*/
void CMcuVcInst::ProcMixStop( CServMsg &cServMsg, BOOL32 bRCStop )
{
    //��δ���ڻ���״̬, NACK 
    if ( m_tConf.m_tStatus.IsNoMixing() )
    {
        cServMsg.SetErrorCode( ERR_MCU_CONFNOTINDISCUSSCMODE );
        if ( cServMsg.GetEventId() != MCU_MCU_STOPMIXER_CMD )
        {
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        }
        return;
    }
    
    //ACK
    if ( MCU_MCU_STOPMIXER_CMD != cServMsg.GetEventId() )
    {
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
    }

    if ( m_tConf.m_tStatus.IsAutoMixing() )
    {
        OnStopDiscussToAllSMcu(&cServMsg);        
    }
    else if ( m_tConf.m_tStatus.IsSpecMixing() )
    {
        for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
        {
            TMt tMt = m_ptMtTable->GetMt(byMtId);
            if ( MT_TYPE_SMCU == tMt.GetMtType() &&
                 ( m_ptMtTable->IsMtInMixing(byMtId) || bRCStop ))
            {
                TMt tSMcu;
                tSMcu.SetMcu(byMtId);
                CServMsg cMsg;
                cMsg.SetMsgBody((u8*)&tSMcu, sizeof(TMt));
                OnStopMixToSMcu( &cMsg );
            }
        }
    }
    
    if (m_tConf.m_tStatus.IsVACing())
    {
        // guzh [11/7/2007] ֻ����״̬
        m_tConf.m_tStatus.SetNoMixing();
        
        //֪ͨ������ֹͣ���ն˷�����
        NotifyMixerSendToMt(FALSE);
        
        //ֹͣ���������ն˵Ľ���
        for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
        {
            if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
            {                        
                StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);
            }
        }
        
        //֪ͨ��ϯ�����л��
        MixerStatusChange();
        
        TMt tMcu;
        tMcu.SetMcu(LOCAL_MCUID);
        cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
        if(!m_tCascadeMMCU.IsNull())
        {
            cServMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
            SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_STOPMIXER_NOTIF, cServMsg);
        }
        if( HasJoinedChairman() )
        {
            SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STOPDISCUSS_NOTIF, cServMsg );
        }
        
        //�ı���ƵԴ
        if( HasJoinedSpeaker() )
        {
            TMt tSpeakerMt = GetLocalSpeaker();
            ChangeAudBrdSrc( &tSpeakerMt );
        }
        else
        {
            ChangeAudBrdSrc( NULL );
        }                                            
    }
    else
    {
        StopMixing();//ֹͣ��������ACK��������״̬
    }

    return;
}


/*------------------------------------------------------------------*/
/*                                 VMP                              */
/*------------------------------------------------------------------*/

/*====================================================================
    ������      ��IsDynamicVmp
    ����        ������ϳ��Ƿ��Ƕ�̬����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/01    3.0         ������         ����
====================================================================*/
BOOL32 CMcuVcInst::IsDynamicVmp( void )
{
//	TPeriEqpStatus tPeriEqpStatus;
//	TVMPParam tVMPParam;

	if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_AUTO )
	{
		return FALSE;
	}

/*
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );  
	tVMPParam = tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam;	
	if( tVMPParam.GetVMPStyle() != VMP_STYLE_DYNAMIC )
	{
		return FALSE;
	}
*/

	return TRUE;
}

/*====================================================================
    ������      ��SetVmpChnnl
    ����        �����û���ϳ���ͨ�� 
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/04    3.0         ������         ����
====================================================================*/
void CMcuVcInst::SetVmpChnnl(TMt tMt, u8 byChnnl, u8 byMemberType, BOOL32 bMsgMcs)
{
    CServMsg cServMsg;

    TMt tLocalMt = GetLocalMtFromOtherMcuMt(tMt);

    // guzh [5/23/2007] �������ն������Ƿ�������Ϊ��󱣻�
    if ( tMt.IsNull() ||
         !m_tConfAllMtInfo.MtJoinedConf(tMt.GetMcuId(), tMt.GetMtId()) || 
         !m_ptMtTable->IsLogicChnnlOpen(tLocalMt.GetMtId(), LOGCHL_VIDEO, FALSE) )
    {
        EqpLog("[SetVmpChnnl] Mt.%d not available for vmp chnl.%d.", tLocalMt.GetMtId(), byChnnl );
        return;
    }

    //��ʼ��������
    // guzh [4/24/2007] ����Ŀǰ��֧�ַ������vmp
    //u8 bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
    u8 bySrcChnnl = (tLocalMt == m_tPlayEqp) ? m_byPlayChnnl : 0;
		 
    // xliang [4/14/2009] ���ݾ������������. 
	// ���ж϶���ģ��������ϳɵ���������ã����Խ������ǵý���ֻ���޸�bStopBeforeStartֵ
	BOOL32 bStopBeforeStart = TRUE;
	if (m_tLastVmpParam.IsMtInMember(tMt) 
		&& m_tLastVmpParam.GetChlOfMtInMember(tMt) == byChnnl)
	{
		bStopBeforeStart = FALSE;
		EqpLog("Mt.%u in the channel.%u has no change, so no bStopBeforeStart!\n",
			tMt.GetMtId(), byChnnl);
	}

	StartSwitchToPeriEqp(tLocalMt, bySrcChnnl, m_tVmpEqp.GetEqpId(),
		byChnnl, MODE_VIDEO, SWITCH_MODE_SELECT, FALSE, 
		bStopBeforeStart);

    if (!tMt.IsLocal())
    {
        OnMMcuSetIn(tMt, 0, SWITCH_MODE_SELECT);
    }

    //�ı�״̬
    TVMPMember tVMPMember;
    tVMPMember.SetMemberTMt(tMt);
    tVMPMember.SetMemberType(byMemberType);
    m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byChnnl, tVMPMember);

    //֪ͨ��ؼ���ϯ
    if (bMsgMcs)
    {
        cServMsg.SetMsgBody((u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam));
        SendMsgToAllMcs(MCU_MCS_VMPPARAM_NOTIF, cServMsg);
        if( HasJoinedChairman() )
        {
            SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_VMPPARAM_NOTIF, cServMsg );
        }	
    }

    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
        //��Ŀ�ĵ�Rtcp������Դ
        TLogicalChannel tLogicalChannel;
        m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);

        u32 dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
        u16 wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();

        //��¼����Ŀ��
        SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tVmpEqp, byChnnl, MODE_VIDEO, SWITCH_MODE_SELECT);
    }
}

/*====================================================================
    ������      ��GetVmpChnnl
    ����        ���õ�����ϳ���ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/04    3.0         ������         ����
====================================================================*/
u8 CMcuVcInst::GetVmpChnnl( void )
{
	u8 byChnnl = 0;
	TVMPMember tVMPMember;
	u8 byMemberNum = m_tConf.m_tStatus.GetVmpParam().GetMaxMemberNum();

    //�п���ͨ��,���ؿ���ͨ��
	while( byChnnl < byMemberNum )
	{
        tVMPMember = *m_tConf.m_tStatus.GetVmpParam().GetVmpMember( byChnnl );
		if( tVMPMember.IsNull() )
		{
			return byChnnl;
		}

        byChnnl ++;
	}

	//û�п���ͨ��,����һ��δ��������������е�ͨ��
	u8 byChl = 0;
	byChnnl = 0;
	while( byChnnl < byMemberNum )
	{
        tVMPMember = *m_tConf.m_tStatus.GetVmpParam().GetVmpMember( byChl );
		//if( !m_tConf.m_tStatus.m_tDiscussParam.IsMtInMember( (TMt)tVMPMember ) )
        if ( m_ptMtTable->IsMtInMixing(tVMPMember.GetMtId()))
		{
			return byChl;
		}

		byChnnl++;
        byChl ++;
		if( byChl == byMemberNum )
		{
            byChl = 0;
		}
	}
	

	return byChl;
}


/*====================================================================
    ������      ��ChangeVmpChannelParam
    ����        ���ı���黭��ϳɷ�ʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/05/28    3.6         libo          ����    
	7/8/2009    4.6         xliang		  ����+�Ż�����FIXME��
====================================================================*/
void CMcuVcInst::ChangeVmpChannelParam(TMt * ptNewSrc, u8 byMemberType, TMt * ptOldSrc)
{
    TVMPMember tTmpVmpMember;
    TVMPMember tVmpMember;
    TVMPParam  tVmpParam;    
    TMt        tMt;
    u8         byMaxMemberNum;
    u8         byNewMemberChnl;
    u8         byMemberChnl;
    u8         byLoop;
    memset( &tVmpParam, 0, sizeof(TVMPParam) );
    tVmpParam      = m_tConf.m_tStatus.GetVmpParam();
    byMaxMemberNum = tVmpParam.GetMaxMemberNum();

    // zbq [06/01/2007] �Ƿ�����������ͨ������
    BOOL32 bVmpParamChged = FALSE;

    if (!m_tConf.m_tStatus.GetVmpParam().IsVMPAuto())
    {
		// xliang [3/20/2009] Ϊ��֤��������£�MPUû��ͬһ��MT�����ͨ�����Է����˸��������Ե���
		TPeriEqpStatus tPeriEqpStatus; 
		g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
		u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
		BOOL bConfHD = CMcuPfmLmt::IsConfFormatHD(m_tConf);
		if(	(byMemberType == VMP_MEMBERTYPE_SPEAKER 
			/*|| byMemberType == VMP_MEMBERTYPE_POLL*/)
			&& (byVmpSubType != VMP)
			//&& bConfHD // ��ȥ����������ͳһ
			)
		{
			AdjustVmpParambyMemberType(ptNewSrc, tVmpParam, byMemberType, ptOldSrc);
			bVmpParamChged = TRUE;
		}
		else
		{
			for (byLoop = 0; byLoop < byMaxMemberNum; byLoop++)
			{
				if ( tVmpParam.GetVmpMember(byLoop)->GetMemberType() == byMemberType )
				{
					StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
					tVmpParam.ClearVmpMember(byLoop);

					// ���� [5/23/2006] ֻ�����ն˽���ͨ��������������豸
					if (ptNewSrc == NULL || ptNewSrc->GetType() == TYPE_MCUPERI)
					{ 
						tMt.SetNull();
					}
					else
					{
						tMt = *ptNewSrc;
					}

					tVmpMember.SetMemberTMt(tMt);
					tVmpMember.SetMemberType(byMemberType);
					tVmpParam.SetVmpMember(byLoop, tVmpMember);

					bVmpParamChged = TRUE;
				}

				//zgc, 2007-12-24, û�и���ģʽʱ�Ϻϳ��ն�Ϊ�����˻���ϯ
				if ( byMemberType == VMP_MEMBERTYPE_SPEAKER || byMemberType == VMP_MEMBERTYPE_CHAIRMAN )
				{
					if ( ( ptNewSrc != NULL && !ptNewSrc->IsNull() && ptNewSrc->GetType() != TYPE_MCUPERI && tVmpParam.IsMtInMember( *ptNewSrc ) )
						|| ( ptOldSrc != NULL && !ptOldSrc->IsNull() && ptOldSrc->GetType() != TYPE_MCUPERI && tVmpParam.IsMtInMember( *ptOldSrc ) )
						)
					{
						bVmpParamChged = TRUE;
					}
				}
			} 
		}
    }        
    else
    {     
        if (byMemberType == VMP_MEMBERTYPE_SPEAKER)
        {
            byNewMemberChnl = 0;
        }
        else if (byMemberType == VMP_MEMBERTYPE_CHAIRMAN)
        {
            if(HasJoinedSpeaker() && GetLocalSpeaker().GetType() != TYPE_MCUPERI)
			{
				byNewMemberChnl = 1;
			}
			else
			{
				byNewMemberChnl = 0;
			}
			
        }
        else if (byMemberType == VMP_MEMBERTYPE_POLL)
        {
            byNewMemberChnl = 2;
        }

        if (ptNewSrc != NULL)
        {
            if (!ptNewSrc->IsNull())
			{
				if (tVmpParam.IsMtInMember(*ptNewSrc))
				{
					if (byMemberType == VMP_MEMBERTYPE_CHAIRMAN 
						&&
						( (!ptNewSrc->IsNull() && ptNewSrc->GetMtId() == GetLocalSpeaker().GetMtId())
						|| byMaxMemberNum < 2 )
						)
					{
						if(byMaxMemberNum >= 2)
						{
							ConfLog( FALSE, "[ChangeVmpChannelParam] Mt.%u is both speaker and chairman, ignore change!\n", ptNewSrc->GetMtId());
						}
						return;
					}
					
					tVmpMember    = *(tVmpParam.GetVmpMember(byNewMemberChnl)); 
					byMemberChnl  = tVmpParam.GetChlOfMtInMember(*ptNewSrc);
					tTmpVmpMember = *(tVmpParam.GetVmpMember(byMemberChnl));	
					
					if(tVmpMember.GetMtId() == tTmpVmpMember.GetMtId())
					{
						ConfLog( FALSE, "[ChangeVmpChannelParam] vmp member has no change at all, so no need to change vmp param!\n");
						return;	//��Աʵ���ޱ仯��ֱ��return
					}
					
					//��ձ�Ҫ�Ľ���������ԭ��1��2��3��4��3-->chairman; ˳���Ϊ3��1��2��4��
					//����ֻ���ǰ3��ͨ�������һ��ͨ�����ò�
					for(byLoop = 0; byLoop <= byMemberChnl; byLoop++)
					{
						StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
					}
				}
				else
				{
					//��յ�ǰ���г�Ա����
					for(byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop ++)
					{
						if( !((tVmpParam.GetVmpMember(byLoop))->IsNull()))
						{
							StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
						}
						
					}
				}
				//�������Ա���岻��.

				bVmpParamChged = TRUE;
			}
			else if(ptOldSrc != NULL && !ptOldSrc->IsNull())	//cancel chairman or speaker. 
			{
				//FIXME��Ŀǰ������������, ������MPU�й���
				bVmpParamChged = TRUE;
			}
			else
			{
				//Do nothing
			}
			
			/*
			if (tVmpParam.IsMtInMember(*ptNewSrc))
            {
                if (byMemberType == VMP_MEMBERTYPE_CHAIRMAN 
					&&
                    ( (!ptNewSrc->IsNull() && ptNewSrc->GetMtId() == GetLocalSpeaker().GetMtId())
						|| byMaxMemberNum < 2 )
					)
                {
					if(byMaxMemberNum >= 2)
					{
						ConfLog( FALSE, "[ChangeVmpChannelParam] Mt.%u is both speaker and chairman, ignore change!\n", ptNewSrc->GetMtId());
					}
                    return;
                }
                
                tVmpMember    = *(tVmpParam.GetVmpMember(byNewMemberChnl)); //mt1
                byMemberChnl  = tVmpParam.GetChlOfMtInMember(*ptNewSrc);	//3
                tTmpVmpMember = *(tVmpParam.GetVmpMember(byMemberChnl));	//mt3

                StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byNewMemberChnl, TRUE, MODE_VIDEO);
                StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byMemberChnl, TRUE, MODE_VIDEO);
                
                tVmpParam.SetVmpMember(byMemberChnl, tVmpMember);
                
                tMt = *ptNewSrc;
            }
            else
            {
                StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byNewMemberChnl, TRUE, MODE_VIDEO);
                tVmpParam.ClearVmpMember(byNewMemberChnl);
                
                tMt = *ptNewSrc;
            }

            // ���� [5/23/2006] ֻ�����ն˽���ͨ��������������豸
            if (tMt.GetType() == TYPE_MCUPERI)
            {
                tMt.SetNull();
            }
            tVmpMember.SetMemberTMt(tMt);
            tVmpMember.SetMemberType(byMemberType);
            tVmpParam.SetVmpMember(byNewMemberChnl, tVmpMember);

            bVmpParamChged = TRUE;
			ConfLog(FALSE, "[ChangeVmpChannelParam] bVmpParamChged is 1!\n");
		*/	
        }
    }

    // zbq [06/01/2007] ͨ��������δ�������򲻸�֪VMP�޸Ĳ��������������Ĵ˴�
    // ��ν���޸Ľ����½������µ�������8000C��˵�������������ƣ��ϳɻ��潫����ƽ������
    if ( bVmpParamChged )
    {
        m_tConf.m_tStatus.SetVmpParam(tVmpParam);
        
        TPeriEqpStatus tPeriEqpStatus;
        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
        tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
        
		AdjustVmpParam(&tVmpParam);
    }

    return;
}

/*====================================================================
    ������      ��ChangeVmpStyle
    ����        ���ı���鶯̬����ϳɷ�ʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt tMt ���ӻ��Ƴ����ն�
				  BOOL32 bAddMt TRUE-���� FALSE-�Ƴ�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/31    3.0         ������          ����
====================================================================*/
void CMcuVcInst::ChangeVmpStyle(TMt tMt, BOOL32 bAddMt, BOOL32 bChangeVmpParam)
{
	//���״̬�Ƿ��Ƕ�̬����				
	if( !IsDynamicVmp() )
	{
		return;
	}
	//��ʱ VCS��̬����ϳɸ��ݵ�ǰ���������ն˽��кϳ�
	//���Ƴ��ն�,�����ն�״̬��Ϊ����,����ʱ���ĸ�״̬,�����Ļ���ϳɷ���ԭ
	u8 byTempChgMtStat = FALSE;

	TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpParam();

	// ����vcs��vmp��̬�ϳɳ�Ա���ڴ˴���ȡ
	if (m_tConf.GetConfSource() != VCS_CONF)
	{
		BOOL32 bNeedRemove = FALSE;
		u8 byMemberChl;
		
		TPeriEqpStatus tPeriEqpStatus;
		TVMPMember tVMPMember;
		
		if( tVMPParam.IsMtInMember( tMt ) )
		{
			bNeedRemove = TRUE;
			byMemberChl = tVMPParam.GetChlOfMtInMember( tMt );
		}
		
		//�õ���ǰ�ն���
		u8 byMtNum = m_tConfAllMtInfo.GetLocalJoinedMtNum();	//����ɾ���ն˽���ú������˴�ͳ�Ƶ��ն���Ŀ�ǽ�Ҫ��ɾ���ն˼������ڵ�
		
		if( !bAddMt )
		{
			byMtNum--;
			m_tConfAllMtInfo.RemoveJoinedMt(tMt.GetMtId());
		}
		
		u8 byVmpStyle = GetVmpDynStyle( byMtNum ); 
		if (VMP_STYLE_NONE == byVmpStyle)
		{
			EqpLog("[ChangeVmpStyle] GetVmpDynStyle failed, byMtNum :%d\n", byMtNum);
			return;
		}    
		
		//�ж��Ƿ���Ҫ�ı�Style
		if( byVmpStyle != m_tConf.m_tStatus.GetVmpStyle() )
		{
			tVMPParam.SetVMPStyle( byVmpStyle );
			u8 byMaxMemberNum = tVMPParam.GetMaxMemberNum();
			if( bAddMt )
			{
				u8 byLoop = 0;
				for( byLoop = 0; byLoop < byMaxMemberNum; byLoop++ )
				{
					tVMPMember = *tVMPParam.GetVmpMember( byLoop );
					if( tVMPMember.IsNull() )
					{
						tVMPMember.SetMemberTMt( tMt );
						tVMPMember.SetMemberType( VMP_MEMBERTYPE_VAC );
						tVMPParam.SetVmpMember( byLoop, tVMPMember );
						break;
					}			
				}
				
				if (byLoop >= byMaxMemberNum && bNeedRemove)
				{               
					tVMPParam.ClearVmpMember(byMemberChl);
					
					if (byMemberChl < byMaxMemberNum)
					{
						tVMPMember.SetMemberTMt( tMt );
						tVMPMember.SetMemberType( VMP_MEMBERTYPE_VAC );
						tVMPParam.SetVmpMember( byMemberChl, tVMPMember );
						m_tConf.m_tStatus.SetVmpParam( tVMPParam );                   
					}
					bNeedRemove = FALSE;
				}
			}
			else
			{
				if( bNeedRemove && byMemberChl <= byMaxMemberNum )
				{
					//�Ƴ���ǰͨ���󣬰Ѻ���ͨ��ǰ��
					tVMPParam.ClearVmpMember( byMemberChl );
					if( byMemberChl < byMaxMemberNum )
					{
						for( u8 byLoop = byMaxMemberNum; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
						{
							tVMPMember = *tVMPParam.GetVmpMember( byLoop );
							if( !tVMPMember.IsNull() )
							{
								tVMPParam.SetVmpMember( byMemberChl, tVMPMember );
								break;
							}
						}
					}
					
				}
			}
			m_tConf.m_tStatus.SetVmpParam( tVMPParam );
		}
		else
		{
			if( bAddMt )
			{
				u8 byMaxMemberNum = tVMPParam.GetMaxMemberNum();
				u8 byLoop = 0;
				for( ; byLoop < tVMPParam.GetMaxMemberNum(); byLoop++ )
				{
					tVMPMember = *tVMPParam.GetVmpMember( byLoop );
					if( tVMPMember.IsNull() )
					{
						tVMPMember.SetMemberTMt( tMt );
						tVMPMember.SetMemberType( VMP_MEMBERTYPE_VAC );
						tVMPParam.SetVmpMember( byLoop, tVMPMember );
						m_tConf.m_tStatus.SetVmpParam( tVMPParam );
						break;
					}			
				}
				
				if (byLoop >= byMaxMemberNum && bNeedRemove)
				{               
					tVMPParam.ClearVmpMember(byMemberChl);
					
					if (byMemberChl < byMaxMemberNum)
					{
						tVMPMember.SetMemberTMt( tMt );
						tVMPMember.SetMemberType( VMP_MEMBERTYPE_VAC );
						tVMPParam.SetVmpMember( byMemberChl, tVMPMember );
						m_tConf.m_tStatus.SetVmpParam( tVMPParam );                   
					}
					bNeedRemove = FALSE;
				}
			}
			else
			{
				if( bNeedRemove )
				{
					u8 byMaxMemberNum = tVMPParam.GetMaxMemberNum();
					u8 byLoop = byMaxMemberNum-1;
					
					tVMPMember = *tVMPParam.GetVmpMember(byMemberChl);
					
					tVMPParam.ClearVmpMember( byMemberChl );				
					
					if (byMemberChl < byMaxMemberNum)
					{
						for(; byLoop > byMemberChl; byLoop--)
						{
							tVMPMember = *tVMPParam.GetVmpMember(byLoop);
							if (!tVMPMember.IsNull())
							{
								tVMPParam.SetVmpMember(byMemberChl, tVMPMember);
								//ͬʱɾ��ԭ�н���
								StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
								tVMPParam.ClearVmpMember(byLoop);
								break;
							}
						}
					}
					
					if (byMtNum >= byMaxMemberNum)
					{
						for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
						{
							if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
							{
								continue;
							}
							TMt tMtTmp = m_ptMtTable->GetMt(byMtId);
							if (byMtId != tMt.GetMtId() && !tVMPParam.IsMtInMember(tMtTmp))
							{
								tVMPMember.SetMemberTMt(tMtTmp);
								tVMPMember.SetMemberType(VMP_MEMBERTYPE_VAC);
								tVMPParam.SetVmpMember(byLoop, tVMPMember);
								break;
							}
						}
					}
					m_tConf.m_tStatus.SetVmpParam( tVMPParam );
				}
			}
		}
		
		if (bAddMt && bNeedRemove)
		{
			u8 byMaxMemberNum = tVMPParam.GetMaxMemberNum();
			
			tVMPParam.ClearVmpMember(byMemberChl);
			
			if (byMemberChl < byMaxMemberNum)
			{
				for(u8 byLoop = byMaxMemberNum-1; byLoop >= byMemberChl+1; byLoop--)
				{
					tVMPMember = *tVMPParam.GetVmpMember(byLoop);
					if (!tVMPMember.IsNull())
					{
						tVMPParam.SetVmpMember(byMemberChl, tVMPMember);
						//ͬʱɾ��ԭ�н���
						StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
						tVMPParam.ClearVmpMember(byLoop);
						break;
					}
				}
			}
			m_tConf.m_tStatus.SetVmpParam(tVMPParam);
        }
    }
	else
	{
		// ��ʱ VCS��̬����ϳɸ��ݵ�ǰ���������ն˽��кϳ�
		if (FALSE == bAddMt && m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
		{
			m_tConfAllMtInfo.RemoveJoinedMt(tMt.GetMtId());
			byTempChgMtStat = TRUE;
		}
	}

    if (bChangeVmpParam)
    {
	    //ChangeVmpParam(&tVMPParam);
		
		// xliang [1/6/2009] ��������VMP��VMP param
		AdjustVmpParam(&tVMPParam);
    }
    else
    {
        SetTimer(MCUVC_CHANGE_VMPPARAM_TIMER, TIMESPACE_CHANGEVMPPARAM);
    }

	if (byTempChgMtStat)
	{
		m_tConfAllMtInfo.AddJoinedMt(tMt.GetMtId());
	}
}

/*====================================================================
    ������      ��ChangeVmpSwitch
    ����        ���ı仭��ϳ�����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt * ptNewSrc��Դ,NULLΪ����ͨ��
	              u8 bySrcType        ����Ա����
                  u8 byState          ��VMPĿ��״̬
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/04    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ChangeVmpSwitch( u8 bySrcType, u8 byState)
{
	u8         byLoop;
	TVMPMember tVMPMember;
	TVMPParam  tVMPParam   = m_tConf.m_tStatus.GetVmpParam();    
	u8         byMemberNum = tVMPParam.GetMaxMemberNum();
	u8		   byVmpStyle  = tVMPParam.GetVMPStyle();// xliang [2/20/2009] ȡstyle

	// xliang [12/10/2008] ȡvmp������
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
	
	BOOL32 bNoneKeda = FALSE; // xliang [12/10/2008] vmp��Ա�Ƿ�Ϊ��keda��
	
	// xliang [12/17/2008] ���VMP��ռͨ����Ϣ��vmp��Ա�����仯ʱ�����г�Ա�������±�����
	m_tVmpChnnlInfo.clear();
//	memset(m_abyMtVmpChnnl,0,sizeof(m_abyMtVmpChnnl));
	
	//����ͨ��ֹͣ��VMP����
	if (byState == VMP_STATE_STOP)
	{
        for (byLoop = 0; byLoop < byMemberNum; byLoop++)
		{
            //ֹͣ��VMP�Ľ���
			StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
            
            //ȡ����ϳɳ�Ա
            tVMPMember = *(tVMPParam.GetVmpMember(byLoop));

            //[1]. ͣ���ܶ����ָ� 
            if (m_tConf.GetConfAttrb().IsResendLosePack())
            {
                //��Ŀ�ĵ�Rtcp������Դ
                TLogicalChannel tLogicalChannel;
                m_ptMtTable->GetMtLogicChnnl(tVMPMember.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);

                u32 dwDstIp  = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
                u16 wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();

                g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIp, wDstPort);
            }
            
            //[2].�ָ��ն˷ֱ���
			// xliang [12/16/2008] ��������� vmp�Ļָ��ն˷ֱ��ʴ���
			ChangeMtVideoFormat( (TMt)tVMPMember, &m_tLastVmpParam, FALSE, FALSE );
		}

		//����VMP�㲥�����VMP����ǰ�㲥�ն˵Ľ���Ҳͣ�����з����˴��ڵĻ��������˿�ʼ�㲥
		if (tVMPParam.IsVMPBrdst())	
		{
			if (HasJoinedSpeaker())
			{
				TMt tMt = GetLocalSpeaker();
				ChangeVidBrdSrc(&tMt);
			}
			else
			{
				ChangeVidBrdSrc(NULL);
			}
		}

		// xliang [4/14/2009] ��ϯѡ��VMP�Ľ���ͣ��(Ҫ������)
		if (tVMPParam.IsVMPSeeByChairman())
		{
			EqpLog("[ChangeVmpSwitch] cancel VMP -> chairman!\n");
			StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO);
		}
		return;
	}

	//��ؿ���ģʽ 
	if (CONF_VMPMODE_CTRL == m_tConf.m_tStatus.GetVMPMode())
	{
		for (byLoop = 0; byLoop < byMemberNum; byLoop++)
		{
			tVMPMember = *tVMPParam.GetVmpMember(byLoop);
			
			if (0 == tVMPMember.GetMemberType() && tVMPMember.IsNull())
			{
				StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
				// xliang [12/15/2008] ���Mt��Ӧ��HDͨ��(������ڶ�Ӧͨ��).��ǰ����������˴�����
			}
			else
			{
				// xliang [12/10/2008] 
				u8 byManuId = m_ptMtTable->GetManuId(tVMPMember.GetMtId());
				if( (MT_MANU_KDC != byManuId) 
					&& (MT_MANU_KDCMCU != byManuId) )
				{
					bNoneKeda = TRUE;
				}
				switch (tVMPMember.GetMemberType()) 
				{
				case VMP_MEMBERTYPE_MCSSPEC:
					{
						if (m_tConfAllMtInfo.MtJoinedConf(tVMPMember.GetMtId()))
						{
							// xliang [12/10/2008] ����vmpʱ���ǿƴ�Ĳ��μӻ���ϳɡ�evpu�����
							if( (byVmpSubType == VMP /*|| byVmpSubType == MPU_EVPU*/) 
								&& bNoneKeda == TRUE )
							{
								EqpLog("Old Vmp,not support None Keda Mt!\n");
								
								StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
								TMt tMt;
								tMt.SetNull();
								tVMPMember.SetMemberTMt(tMt);
								tVMPMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);
								m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byLoop, tVMPMember);
								
								// xliang [12/16/2008] �ϱ������mcs
								NotifyMcsAlarmInfo(0, ERR_MCU_NONKEDAMT_JOINVMP);
								
							}
							else
							{
								if(byVmpSubType == VMP)
								{
									SetVmpChnnl((TMt)tVMPMember, byLoop, VMP_MEMBERTYPE_MCSSPEC);
									
									//�����ն˵ķֱ���
									ChangeMtVideoFormat( (TMt)tVMPMember, &tVMPParam );
								}
								else
								{
									BOOL32 bRet = FALSE;
									bRet = ChangeMtVideoFormat((TMt)tVMPMember, &tVMPParam);
									
									if(bRet)
									{
										SetVmpChnnl((TMt)tVMPMember, byLoop, VMP_MEMBERTYPE_MCSSPEC);
									}
									else
									{
										//��¼tVMPMember��Ӧ��chnnl���Ա���ռ�ɹ���,��ͨ��������
										//Ŀǰ���ڷ���ռ��ص�����ԭ�������û��SetVmpChnnl������ 
										//(��ǿƴ��HD�ն�ʹ��evpu��)��Ҳһ����¼��Chnnl.
										//������ռʧ���������������byLoop������Ϊ0�� Ϊ0��϶��õ�����HDǰ����ͨ����VMP
										//m_abyMtVmpChnnl[tVMPMember.GetMtId()-1] = byLoop;
									}
								}
							}
						}
						break;
					}
				case VMP_MEMBERTYPE_SPEAKER://ע��ı䷢����ʱ�ı佻��
                    // ���� [5/23/2006] ֻ�����ն˽���ͨ��������������豸
					if (HasJoinedSpeaker() && GetLocalSpeaker().GetType() != TYPE_MCUPERI)
					{
						// xliang [12/10/2008] ����vmpʱ���ǿƴ�Ĳ��μӻ���ϳ�
						if( (byVmpSubType == VMP /*|| byVmpSubType == MPU_EVPU*/) 
							&& bNoneKeda == TRUE )
						{
							ConfLog(FALSE,"Old Vmp not support None Keda Mt!\n");
							StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
							TMt tMt;
							tMt.SetNull();
							tVMPMember.SetMemberTMt(tMt);
							tVMPMember.SetMemberType(VMP_MEMBERTYPE_SPEAKER);
							m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byLoop, tVMPMember);
						}
						else
						{
							// xliang [12/11/2008] 
							if(byVmpSubType == VMP)
							{
								SetVmpChnnl(GetLocalSpeaker(), byLoop, VMP_MEMBERTYPE_SPEAKER);
								
								//�����ն˵ķֱ���
								ChangeMtVideoFormat( (TMt)tVMPMember, &tVMPParam );
							}
							else
							{
								BOOL32 bRet = ChangeMtVideoFormat((TMt)tVMPMember, &tVMPParam);
								if(bRet)
								{
									SetVmpChnnl(GetLocalSpeaker(), byLoop, VMP_MEMBERTYPE_SPEAKER);
								}
								else
								{
									//��¼tVMPMember��Ӧ��chnnl���Ա���ռ�ɹ���,��ͨ������
									//m_abyMtVmpChnnl[tVMPMember.GetMtId()-1] = byLoop;
								}
							}
						}
					}
					else
					{
						StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
						TMt tMt;
						tMt.SetNull();
						tVMPMember.SetMemberTMt(tMt);
						tVMPMember.SetMemberType(VMP_MEMBERTYPE_SPEAKER);
						m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byLoop, tVMPMember);
					}
					break;

				case VMP_MEMBERTYPE_CHAIRMAN://ע��ı���ϯʱ�ı佻��
					if (HasJoinedChairman())
					{
						// xliang [12/10/2008] ����vmp��evpuʱ���ǿƴ�Ĳ��μӻ���ϳ�
						if( (byVmpSubType == VMP ) 
							&& bNoneKeda == TRUE )
						{
							ConfLog(FALSE,"Using old Vmp or EVPU, doesn't support None Keda Mt!\n");
							
							StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
							TMt tMt;
							tMt.SetNull();
							tVMPMember.SetMemberTMt(tMt);
							tVMPMember.SetMemberType(VMP_MEMBERTYPE_CHAIRMAN);
							m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byLoop, tVMPMember);
						}
						else
						{
							if(byVmpSubType == VMP)
							{
								SetVmpChnnl(m_tConf.GetChairman(), byLoop, VMP_MEMBERTYPE_CHAIRMAN);
								
								//�����ն˵ķֱ���
								ChangeMtVideoFormat( (TMt)tVMPMember, &tVMPParam );
							}
							else
							{
								BOOL32 bRet = ChangeMtVideoFormat((TMt)tVMPMember, &tVMPParam);
								if(bRet)
								{
									SetVmpChnnl(m_tConf.GetChairman(), byLoop, VMP_MEMBERTYPE_CHAIRMAN);
								}
								else
								{
									//��¼tVMPMember��Ӧ��chnnl���Ա���ռ�ɹ���,��ͨ������
									//m_abyMtVmpChnnl[tVMPMember.GetMtId()-1] = byLoop;
								}
							}
						}
					}
					else
					{
						StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
						TMt tMt;
						tMt.SetNull();
						tVMPMember.SetMemberTMt(tMt);
						tVMPMember.SetMemberType(VMP_MEMBERTYPE_CHAIRMAN);
						m_tConf.m_tStatus.m_tVMPParam.SetVmpMember( byLoop, tVMPMember);
					}
					break;

				case VMP_MEMBERTYPE_POLL://ע��ı���ѯ�㲥Դʱ�ı佻��
					if (m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO || 
						m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_SPEAKER)
					{	
						// xliang [12/10/2008] ����vmp���ǿƴ�Ĳ��μӻ���ϳ�
						if( (byVmpSubType == VMP ) 
							&& bNoneKeda == TRUE )
						{
							ConfLog(FALSE,"Using old Vmp or EVPU, doesn't support None Keda Mt!\n");
							
							StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
							TMt tMt;
							tMt.SetNull();
							tVMPMember.SetMemberTMt(tMt);
							tVMPMember.SetMemberType(VMP_MEMBERTYPE_POLL);
							m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byLoop, tVMPMember);
						}
						else
						{
							TMt tMt = (TMt)m_tConf.m_tStatus.GetMtPollParam();
							if(byVmpSubType == VMP)
							{
								SetVmpChnnl(tMt, byLoop, VMP_MEMBERTYPE_POLL);								
								//�����ն˵ķֱ���
								ChangeMtVideoFormat( (TMt)tVMPMember, &tVMPParam );
							}
							else
							{
								BOOL32 bRet = ChangeMtVideoFormat((TMt)tVMPMember, &tVMPParam);
								if(bRet)
								{
									SetVmpChnnl(tMt, byLoop, VMP_MEMBERTYPE_POLL);
								}
								else
								{
									//FIXME���ݲ�������
								}
							}
						}
					}
					else
					{
						StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);

						TMt tMt;
						tMt.SetNull();
						tVMPMember.SetMemberTMt(tMt);
						tVMPMember.SetMemberType(VMP_MEMBERTYPE_POLL);
						m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byLoop, tVMPMember);
					}
					break;
					
				default:
					break;
				}
			}
		}	
		
		// xliang [12/22/2008] FIXME:��VMP�Ļ���ģ�崦��
		//�л����豸ģ��ʱ�Ĵ���, ֻ�ڻ��鿪ʼʱʹ�ã�ֹͣVMP���ٴθ��ϣ�����ʹ��
		if( byState == VMP_STATE_START && 
			m_tConf.GetConfAttrb().IsHasVmpModule() && 
            !m_tConfInStatus.IsVmpModuleUsed() )
		{
            u8 byMemberType = 0;			
			for( u8 byLoop = 0; byLoop < byMemberNum; byLoop++ )
			{
                // ���� [5/11/2006] ����֧��ָ��ͨ������
				TMt tMt = m_tConfEqpModule.m_atVmpMember[byLoop];
                byMemberType = m_tConfEqpModule.m_tVmpModule.m_abyMemberType[byLoop];
				if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) ||
                    byMemberType != VMP_MEMBERTYPE_MCSSPEC)
				{
                    // ������
                    if (m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ))
                    {
                        SetVmpChnnl( tMt, byLoop, byMemberType, TRUE );

                        //�����ն˵ķֱ���
                        ChangeMtVideoFormat( (TMt)tVMPMember, &tVMPParam );
                    }
                    else
                    {
                        // ��������״̬
						tMt.SetNull();
						tVMPMember.SetMemberTMt(tMt);
						tVMPMember.SetMemberType(byMemberType);
						m_tConf.m_tStatus.m_tVMPParam.SetVmpMember( byLoop, tVMPMember);
                    }					
				}
			}			
		}

        if (VMP_STATE_START == byState && m_tConf.m_tStatus.IsBrdstVMP())
        {
            ChangeVidBrdSrc(&m_tVmpEqp);
        }
	}
  
	//�Զ����ó�Աģʽ
	// xliang [7/3/2009] ������
	if( m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_AUTO )
	{
		u8 byMember = 0;
		if( VMP_STATE_START  == byState ||
            VMP_STATE_CHANGE == byState )
		{                
			//��ճ�Ա
			for( byLoop = 0; byLoop < byMemberNum; byLoop++ )
			{
				if (m_tConf.GetConfAttrb().IsSatDCastMode())
				{
					StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
				}
                
				m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );			
			} 

			for (byLoop = 0; byLoop < byMemberNum; byLoop++)
			{
				tVMPMember = *tVMPParam.GetVmpMember(byLoop);
				
				if (tVMPMember.IsNull())
				{
					StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
				}
				else
				{
					if (m_tConfAllMtInfo.MtJoinedConf(tVMPMember.GetMtId()))
					{
						BOOL32 bRet = FALSE;
						bRet = ChangeMtVideoFormat((TMt)tVMPMember, &tVMPParam);
						if(bRet)
						{
							SetVmpChnnl((TMt)tVMPMember, byLoop, tVMPMember.GetMemberType());
						}
					}
				}
			}
			if (VCS_CONF == m_tConf.GetConfSource())
			{
				// �Ҷϱ��߳�����ϳ����ĳ�Ա
				CServMsg cServMsg;
				if ( CONF_VMPMODE_NONE != m_tLastVmpParam.GetVMPMode() )
				{
					for( s32 nIndex = 0; nIndex < m_tLastVmpParam.GetMaxMemberNum(); nIndex ++ )
					{
						TMt tVMPMemberOld = *(TMt *)m_tLastVmpParam.GetVmpMember(nIndex);
        
						if ( !m_tConf.m_tStatus.m_tVMPParam.IsMtInMember(tVMPMemberOld) &&
							m_tConfAllMtInfo.MtJoinedConf(tVMPMemberOld.GetMcuId(), tVMPMemberOld.GetMtId())) 
						{
							if (!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
							{
								VCSDropMT(tVMPMemberOld);
							}
							m_cVCSConfStatus.SetCurUseVMPChanInd(nIndex + 1);
						}
					}             
				}
			}
			/*
			if (VCS_CONF == m_tConf.GetConfSource())
			{
				TMt atMtMember[MAXNUM_VMP_MEMBER];
				u8 byStyle = GetVCSAutoVmpMember(atMtMember);
				if ( byStyle != VMP_STYLE_NONE)
				{
					SetVCSVmpMemInChanl(atMtMember, byStyle);
				}
			}
			else
			{
				//��ճ�Ա
				for( byLoop = 0; byLoop < byMemberNum; byLoop++ )
				{
					if (m_tConf.GetConfAttrb().IsSatDCastMode())
					{
						StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
					}
                
					m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );			
				} 
				// xliang [12/16/2008] ������vmp����ԭ�в���
				if(byVmpSubType == VMP) 
				{
					//���з�����,ռ�õ�ǰ��������ͨ��,����0ͨ��
					// guzh [8/25/2006] ����ҲҪ�жϷ��񲻽���ͨ��
					if (HasJoinedSpeaker() && GetLocalSpeaker().GetType() != TYPE_MCUPERI)
					{
						SetVmpChnnl(GetLocalSpeaker(), byMember, VMP_MEMBERTYPE_SPEAKER);
						byMember++;
						
						//���������˵ķֱ���
						ChangeMtVideoFormat( GetLocalSpeaker(), &tVMPParam );
					}
					
					//������ϯ,ռ�õ�ǰ��������ͨ��(�޷�����Ϊ0ͨ��,����Ϊ1ͨ��)
					if (HasJoinedChairman() &&
						m_tConf.GetChairman().GetMtId() != GetLocalSpeaker().GetMtId())
					{
						SetVmpChnnl(m_tConf.GetChairman(), byMember, VMP_MEMBERTYPE_CHAIRMAN);
						byMember++;
						
						//������ϯ�ķֱ���
						ChangeMtVideoFormat( m_tConf.GetChairman(), &tVMPParam );
					}               
					
					//�����ն�����ռ������ͨ��
					for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
					{		
						if (byMember >= byMemberNum)
						{
							break;
						}
						
						if (m_tConfAllMtInfo.MtJoinedConf(byLoop))
						{
							if (byLoop == m_tConf.GetChairman().GetMtId())
							{
								// ��ϯ��ǰ��
								continue;
							}
							if (byLoop == GetLocalSpeaker().GetMtId() &&
								GetLocalSpeaker().GetType() != TYPE_MCUPERI)
							{
								// ��������ǰ��
								continue;
							}
							
							TMt tMt = m_ptMtTable->GetMt(byLoop);
							SetVmpChnnl(tMt, byMember, VMP_MEMBERTYPE_VAC);
							byMember++;
							
							//�����ն˵ķֱ���
							ChangeMtVideoFormat( tMt, &tVMPParam );
						}				
					}
				}
				else // xliang [12/16/2008] ���evpu, mpu�Ĵ���
				{
					/*
					//���з�����,ռ�õ�ǰ��������ͨ��,����0ͨ��
					// guzh [8/25/2006] ����ҲҪ�жϷ��񲻽���ͨ��
					BOOL32 bRet = TRUE;
					if (HasJoinedSpeaker() && GetLocalSpeaker().GetType() != TYPE_MCUPERI)
					{
						// xliang [12/17/2008] ��������VMP��Ա�����б��е��ն�
						if(!IsMtNeglectedbyVmp(GetLocalSpeaker().GetMtId()) )
						{
							bRet = ChangeMtVideoFormat(GetLocalSpeaker(), &tVMPParam);
							
							if(bRet)
							{
								SetVmpChnnl(GetLocalSpeaker(), byMember, VMP_MEMBERTYPE_SPEAKER);
								byMember++;
							}
							else
							{
								//m_abyMtVmpChnnl[GetLocalSpeaker().GetMtId()-1] = byMember;
								byMember++;
							}
						}
					}
					
					//������ϯ,ռ�õ�ǰ��������ͨ��(�޷�����Ϊ0ͨ��,����Ϊ1ͨ��)
					if (HasJoinedChairman() &&
						m_tConf.GetChairman().GetMtId() != GetLocalSpeaker().GetMtId())
					{
						// xliang [12/17/2008] ��������VMP��Ա�����б��е��ն�
						if(!IsMtNeglectedbyVmp(m_tConf.GetChairman().GetMtId()) )
						{
							bRet = ChangeMtVideoFormat(m_tConf.GetChairman(), &tVMPParam);
							if(bRet)
							{
								SetVmpChnnl(m_tConf.GetChairman(), byMember, VMP_MEMBERTYPE_CHAIRMAN);
								byMember++;
							}
							else
							{
								//m_abyMtVmpChnnl[m_tConf.GetChairman().GetMtId()-1] = byMember;
								byMember++;
							}

						}
					}               
					
					//�����ն�����ռ������ͨ��
					for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
					{		
						if (byMember >= byMemberNum)
						{
							break;
						}
						
						if (m_tConfAllMtInfo.MtJoinedConf(byLoop))
						{
							if (byLoop == m_tConf.GetChairman().GetMtId())
							{
								// ��ϯ��ǰ��
								continue;
							}
							if (byLoop == GetLocalSpeaker().GetMtId() &&
								GetLocalSpeaker().GetType() != TYPE_MCUPERI)
							{
								// ��������ǰ��
								continue;
							}
							
							// xliang [12/17/2008] ��������VMP��Ա�����б��е��ն�
							if (IsMtNeglectedbyVmp(byLoop))
							{
								continue;
							}
							TMt tMt = m_ptMtTable->GetMt(byLoop);
							bRet = ChangeMtVideoFormat(tMt, &tVMPParam);
							if(bRet)
							{
								SetVmpChnnl(tMt, byMember, VMP_MEMBERTYPE_VAC);
								byMember++;
							}
							else
							{
								//m_abyMtVmpChnnl[tMt.GetMtId()-1] = byMember;
								byMember++;
							}
						}		
					}
				}
			}*/

		}
		else //ֹͣ�Զ�����ϳ�
		{
			for( byLoop = 0; byLoop < byMemberNum; byLoop++ )
			{
				tVMPMember = *tVMPParam.GetVmpMember( byLoop );
				if( !tVMPMember.IsNull() && m_tConfAllMtInfo.MtJoinedConf( tVMPMember.GetMtId() ) )
				{
					SetVmpChnnl( (TMt)tVMPMember, byLoop, tVMPMember.GetMemberType() );

                    //�ָ��ն˵ķֱ���
					// xliang [12/16/2008] ��������� vmp�Ļָ��ն˷ֱ��ʴ���
					ChangeMtVideoFormat((TMt)tVMPMember,&m_tLastVmpParam, FALSE);
				}			
			} 				
		}
        
        if (VMP_STATE_START == byState && m_tConf.m_tStatus.IsBrdstVMP())
        {
            ChangeVidBrdSrc(&m_tVmpEqp); 
        }
	}
	// xliang [4/14/2009] ��ϯѡ��VMP����
	if ( tVMPParam.IsVMPSeeByChairman() )
	{
		CServMsg cMsg;
		cMsg.SetNoSrc();
		cMsg.SetEventId(MT_MCU_STARTSWITCHVMPMT_REQ);
		ChairmanSeeVmp(m_tConf.GetChairman(), cMsg);
	}

    //��ʡ����ʱ���ı仭��ϳɳ�Աʱ����Ҫ���Ʒ�keda����mt�����Ƿ���...
	
	CServMsg cServMsg;
	cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam) );
	SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg ); //���߻��ˢ����
    
	// ���ĺϳɷֱ��ʲ���
	if (VMP_STATE_CHANGE == byState)
    {
        if ( CONF_VMPMODE_NONE != m_tLastVmpParam.GetVMPMode() )
        {
            // xliang [3/13/2009] ���ϴ���ͨ���ڵ�Mt������������HD����ͨ����ռ�������Ҫ���MT��VMP�Ľ���
// 			for( u8 byIdx = 1; byIdx <= MAXNUM_CONF_MT; byIdx ++)
// 			{
// 				if( m_abyMtVmpChnnl[byIdx-1] !=0 )
// 				{
// 					StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), m_abyMtVmpChnnl[byIdx-1], FALSE, MODE_VIDEO);
// 				}
// 			}
			for( s32 nIndex = 0; nIndex < m_tLastVmpParam.GetMaxMemberNum(); nIndex ++ )
            {
                TMt tVMPMemberOld = *(TMt *)m_tLastVmpParam.GetVmpMember(nIndex);
                
                //��ɾ���˵ĺϳɳ�Ա�ָ��䷢�ͷֱ���
                if ( !m_tConf.m_tStatus.m_tVMPParam.IsMtInMember(tVMPMemberOld) &&
                    m_tConfAllMtInfo.MtJoinedConf(tVMPMemberOld.GetMtId()) ) 
                {
                    // xliang [12/16/2008] ��������� vmp�Ļָ��ն˷ֱ��ʴ���
					ChangeMtVideoFormat(tVMPMemberOld, &m_tLastVmpParam, FALSE, FALSE);
				}
            }             
        }   
    }

	//���汾�λ���ϳɲ���������h264��̬�ֱ��ʻָ�
	memcpy(&m_tLastVmpParam, &m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam));

    if(g_bPrintEqpMsg)
    {
        OspPrintf(TRUE, FALSE, "[ChangeVmpSwitch] The result vmp param:");
        m_tConf.m_tStatus.m_tVMPParam.Print();
    }
    
    if( HasJoinedChairman() )
    {
        SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_VMPPARAM_NOTIF, cServMsg );        
    }
	
	// xliang [4/20/2009] ��һ�δ���ı�VMP param�����У��û�������������һ��change vmp param
	// (ʵ�ʳ�����Ҫ���л���������ѡ/ȡ��"�㲥����"��ť), ���ܻᵼ��ʵ��Ч�������������
	// Ϊ��������⣬�ϸ�����Ӧ��VMP�Ӹı���������꽻��(�㲥/�ǹ㲥)��װ��ԭ�Ӳ������ڼ���ص���������Ӧ��[FIXME]
	// ������һ��, ��VMP�Ӹı��������������������� ��Ϊԭ��
	m_byVmpOperating = 0;

}

/*=============================================================================
    �� �� ���� CancelOneVmpFollowSwitch
    ��    �ܣ� ��ջ��ģʽ�µ�ָ���������͵���Ӧ���Ͻ�����״̬
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byMemberType
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/03/21  3.6			����                  ����
=============================================================================*/
void CMcuVcInst::CancelOneVmpFollowSwitch( u8 byMemberType, BOOL32 bNotify )
{
	//��ؿ���ģʽ 
	if( CONF_VMPMODE_CTRL != m_tConf.m_tStatus.GetVMPMode() || 
		VMP_MEMBERTYPE_MCSSPEC == byMemberType )
	{
		return;
	}

	u8 byLoop;
	TVMPMember tVMPMember;
	TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpParam();
	u8 byMemberNum = tVMPParam.GetMaxMemberNum();

	for( byLoop = 0; byLoop < byMemberNum; byLoop++ )
	{
		tVMPMember = *m_tConf.m_tStatus.GetVmpParam().GetVmpMember( byLoop );
		if( !tVMPMember.IsNull() && 
			byMemberType == tVMPMember.GetMemberType() )
		{
			StopSwitchToPeriEqp( m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO );
			
			TMt tMt;
			tMt.SetNull();
			tVMPMember.SetMemberTMt( tMt );
			tVMPMember.SetMemberType( byMemberType );
			m_tConf.m_tStatus.m_tVMPParam.SetVmpMember( byLoop, tVMPMember);

			//ͬ�����踴��״̬
			TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
			tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
			g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
		}
	}

	if( bNotify )
	{
		CServMsg cServMsg;
		cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam) );

		SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
		
		TPeriEqpStatus tVmpEqpStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tVmpEqpStatus );
		cServMsg.SetMsgBody( ( u8 * )&tVmpEqpStatus, sizeof( tVmpEqpStatus ) );
		SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );
	}

	return;
}

/*====================================================================
    ������      ��ChangeVmpBitRate
    ����        ���ı仭��ϳɵ����� 
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wBitRate, ����ϳɵ�����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/05/21    3.0         ������         ����
====================================================================*/
//modify bas 2
void CMcuVcInst::ChangeVmpBitRate(u16 wBitRate, u8 byVmpChanNo/* = 1*/)
{
    TPeriEqpStatus tVmpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpStatus);
    u8 byVmpSubType = tVmpStatus.m_tStatus.tVmp.m_bySubType;
	if( MPU_SVMP == byVmpSubType)
	{
		AdjustVmpBrdBitRate();
		return;
	}

	u16 wVmpBitRate = htons(wBitRate);

    CServMsg cServMsg;
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetChnIndex(byVmpChanNo);
    cServMsg.SetMsgBody((u8*)&wVmpBitRate, sizeof(u16));	
    SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_SETCHANNELBITRATE_REQ, cServMsg);

    u16 wOriBitrate = 0;

    if (1 == byVmpChanNo)
    {
        wOriBitrate = m_wVidBasBitrate;
        m_wVidBasBitrate = wBitRate;
    }
    else if (2 == byVmpChanNo)
    {
        wOriBitrate = m_wBasBitrate;
        m_wBasBitrate = wBitRate;
    }

    EqpLog("[ChangeVmpBitrate] ChanNo.%d be changed from BR.%d to BR.%d\n", byVmpChanNo, wOriBitrate, wBitRate);

    return;
}

/*====================================================================
    ������      ��AdjustVmpBrdBitRate
    ����        ����������ϳɵĵ�ǰ�������
    �㷨ʵ��    ����������VMP������ĳͨ������ͽ�������
    ����ȫ�ֱ�����
    �������˵����TMt *		[in]	��VMP������ͨ���ŵĲο�������MPU������
    ����ֵ˵��  ��u8		������VMP���ص�����ʵ��ͨ���ţ�����MPU������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/08/04    4.6         Ѧ��          ����
====================================================================*/
u8 CMcuVcInst::AdjustVmpBrdBitRate( TMt *ptMt /*= NULL*/)
{
    //������ֻ��Ե��ٻ���VMP��������, byVmpChanNoҲ��Ϊ������չԤ����.
//     if ( 0 != m_tConf.GetSecBitRate() )
//     {
//         return;
//     }
    
	u16 wMinMtReqBitRate = 0;
	u8  byMediaType = 0;
	TPeriEqpStatus tVmpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpStatus);
    u8 byVmpSubType = tVmpStatus.m_tStatus.tVmp.m_bySubType;
	if( VMP == byVmpSubType)
	{
		if( NULL == ptMt || ptMt->IsNull() )
		{
			EqpLog("[AdjustVmpBrdBitRate] ptMt is NULL as using old VMP!\n ");
			return 0;
		}
		u8 byVmpChanNo = 0;
		TSimCapSet tDstSimCapSet = m_ptMtTable->GetDstSCS(ptMt->GetMtId());
		byMediaType = m_tConf.GetMainVideoMediaType();
		//˫�ٻ���Ҫ�������ʱ�ĺ�������
        if ( 0 != m_tConf.GetSecBitRate() && 
            MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
        {
            if (m_ptMtTable->GetDialBitrate(ptMt->GetMtId()) == m_tConf.GetBitRate())
            {
                byVmpChanNo = 1;
            }
            else
            {
                byVmpChanNo = 2;
            }
        }
        //���ٻ�˫��ʽ����
        else
        {
            if (tDstSimCapSet.GetVideoMediaType() == byMediaType)
            {
                byVmpChanNo = 1;
            }
            else
            {
                byVmpChanNo = 2;
            }
        }

		u8 byEncType = m_tConf.GetCapSupport().GetMainSimCapSet().GetVideoMediaType();
		wMinMtReqBitRate = GetLeastMtReqBitrate( TRUE, byEncType );
		
		u16 wOriBitrate = 0;
		if ( wMinMtReqBitRate != m_awVMPBrdBitrate[byVmpChanNo-1] )
		{
			wOriBitrate = m_awVMPBrdBitrate[byVmpChanNo-1];
			m_awVMPBrdBitrate[byVmpChanNo-1] = wMinMtReqBitRate;
			u16 wVmpBitRate = htons(wMinMtReqBitRate);
			
			CServMsg cServMsg;
			cServMsg.SetConfId(m_tConf.GetConfId());
			cServMsg.SetChnIndex(byVmpChanNo);
			cServMsg.SetMsgBody((u8*)&wVmpBitRate, sizeof(u16));	
			SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_SETCHANNELBITRATE_REQ, cServMsg);
			
			EqpLog("[AdjustVmpBrdBitrate] ChanNo.%d be changed from BR.%d to BR.%d\n", byVmpChanNo, wOriBitrate, wMinMtReqBitRate);
		}

		return (byVmpChanNo - 1);
	}

	u8 byBoardVer	= tVmpStatus.m_tStatus.tVmp.m_byBoardVer;
	u8 byVmpOutChnnl = 0;
	u8 byChnnlRes	= 0;

	for (; byVmpOutChnnl < MAXNUM_MPU_OUTCHNNL; byVmpOutChnnl ++)
	{
		
		if (byBoardVer == MPU_BOARD_A128 && byVmpOutChnnl == 3) //A��ֻ��3·
		{
			ConfLog( FALSE, "MPU A version, only 3 out channel!\n" );
			break;
		}
		byChnnlRes = GetResByVmpOutChnnl(byMediaType, byVmpOutChnnl, m_tConf, m_tVmpEqp.GetEqpId());
		
		wMinMtReqBitRate = GetLstRcvGrpMtBr(TRUE, byMediaType, byChnnlRes);

		u16 awOriBitrate[MAXNUM_MPU_OUTCHNNL] = {0};
		if ( wMinMtReqBitRate != m_awVMPBrdBitrate[byVmpOutChnnl] )
		{
			awOriBitrate[byVmpOutChnnl] = m_awVMPBrdBitrate[byVmpOutChnnl];
			m_awVMPBrdBitrate[byVmpOutChnnl] = wMinMtReqBitRate;
			u16 wVmpBitRate = htons(wMinMtReqBitRate);
			
			CServMsg cServMsg;
			cServMsg.SetConfId(m_tConf.GetConfId());
			cServMsg.SetChnIndex(byVmpOutChnnl);
			cServMsg.SetMsgBody((u8*)&wVmpBitRate, sizeof(u16));	
			SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_SETCHANNELBITRATE_REQ, cServMsg);
			
			EqpLog("[AdjustVmpBrdBitrate] ChanNo.%d be changed from BR.%d to BR.%d\n", 
				byVmpOutChnnl, awOriBitrate[byVmpOutChnnl], wMinMtReqBitRate);
		}
	}
    return 1;
}

/*====================================================================
    ������      ��UpdataVmpEncryptParam
    ����        ���ı仭��ϳɵı�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt tMt
                  u8  byChnlNo
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/04/18    3.6         libo          ����
====================================================================*/
void CMcuVcInst::UpdataVmpEncryptParam(TMt tMt, u8 byChnlNo)
{
    TVMPParam tConfVMPParam;
    TMediaEncrypt  tVideoEncrypt;
    TDoublePayload tDoublePayload;

    tConfVMPParam = m_tConfEqpModule.m_tVmpModule.m_tVMPParam;

	if (m_tConfEqpModule.m_atVmpMember[byChnlNo].GetMtId() != tMt.GetMtId())
    {
        return;
    }

    BOOL32 bRet = FALSE;
    TLogicalChannel tChnnl;

    bRet = m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tChnnl, FALSE);
    if (bRet == TRUE)
    {
        tVideoEncrypt = tChnnl.GetMediaEncrypt();
        tDoublePayload.SetRealPayLoad(tChnnl.GetChannelType());
	    tDoublePayload.SetActivePayload(tChnnl.GetActivePayload());
    }

    CServMsg cServMsg;
    cServMsg.SetConfId( m_tConf.GetConfId() );
    cServMsg.SetMsgBody( (u8*)&byChnlNo, sizeof(u8) );
    cServMsg.CatMsgBody( (u8*)&tVideoEncrypt, sizeof(TMediaEncrypt) );
    cServMsg.CatMsgBody( (u8*)&tDoublePayload, sizeof(TDoublePayload) );

    SendMsgToEqp( m_tVmpEqp.GetEqpId(), MCU_VMP_UPDATAVMPENCRYPTPARAM_REQ, cServMsg );

}

/*====================================================================
    ������      ��ChangeVmpParam
    ����        ������ϳɲ����ı�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TVMPParam *ptVMPParam, ����ϳɲ���
				  BOOL32 bStart �Ƿ��ǿ�ʼ����ϳ�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/04    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ChangeVmpParam(TVMPParam *ptVMPParam, BOOL32 bStart)
{
	if (ptVMPParam == NULL)
	{
		return;
	}

    // guzh [7/25/2007] ��ǰ����ĺϳɲ��������������ᱣ����Ȳ��������ǵ�VMP��Ӧ�ٱ���
    // �ڴ�֮ǰ�� SetPeriEqpStatus ����ʱ����
    // ��VMP��Ӧ֮ǰ��GetVmpParam�᷵����ǰ��Param    
	TVMPParam tConfVMPParam = m_tConf.m_tStatus.GetVmpParam();
    
    u16 wMinMtReqBitrate = 0;
    u8  byRes;

    //���ò���
    CKDVVMPParam cKDVVMPParam;
    memset(&cKDVVMPParam, 0, sizeof(cKDVVMPParam));
    cKDVVMPParam.m_byVMPStyle = ptVMPParam->GetVMPStyle();
    cKDVVMPParam.m_byEncType = m_tConf.GetMainVideoMediaType();
    
    //���ж�˫�ٵ���ʽ������������һͨ��Ϊ�������ʣ��ڶ�ͨ��Ϊ�ǻ������ʵ�������ʣ�>=���鸨�����ʣ�
    if (0 != m_tConf.GetSecBitRate() && 
        MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
    {
        wMinMtReqBitrate = m_tConf.GetBitRate();
    }
    else
    {
        wMinMtReqBitrate = GetLeastMtReqBitrate(TRUE, cKDVVMPParam.m_byEncType);
        //m_awVMPBrdBitrate[0] = wMinMtReqBitrate;
    }
    m_awVMPBrdBitrate[0] = wMinMtReqBitrate;
//     if (wMinMtReqBitrate < m_wVidBasBitrate || 0 == m_wVidBasBitrate)
//     {
//         m_wVidBasBitrate = wMinMtReqBitrate;
//     }
    cKDVVMPParam.m_wBitRate = htons(wMinMtReqBitrate);

    
    u16 wWidth = 0;
    u16 wHeight = 0;
    m_tConf.GetVideoScale(cKDVVMPParam.m_byEncType,
                          wWidth,
                          wHeight);
    cKDVVMPParam.m_wVideoWidth = wWidth;
    cKDVVMPParam.m_wVideoHeight = wHeight;


    //����Լ��16CIF����2CIF,������AUTO����, xsl [8/11/2006] mpeg2 4cif��2cif����
    byRes = m_tConf.GetVideoFormat(cKDVVMPParam.m_byEncType);
    if( VIDEO_FORMAT_16CIF == byRes ||
        (MEDIA_TYPE_H262 == cKDVVMPParam.m_byEncType && VIDEO_FORMAT_4CIF == byRes))
    {
        cKDVVMPParam.m_wVideoWidth  = 352;
        cKDVVMPParam.m_wVideoHeight = 576;
    }
    
	cKDVVMPParam.m_wVideoWidth  = htons(cKDVVMPParam.m_wVideoWidth);
	cKDVVMPParam.m_wVideoHeight = htons(cKDVVMPParam.m_wVideoHeight);
	cKDVVMPParam.m_byMemberNum  = ptVMPParam->GetMaxMemberNum();

    TMt tSpeakerMt = GetLocalSpeaker();
    // ���� [5/23/2006] ֻ�����ն˽���ͨ��������������豸
    if (tSpeakerMt.GetType() == TYPE_MCUPERI)
    {
        memset(&tSpeakerMt, 0, sizeof(TMt));
    }
    TMt tChairMt = m_tConf.GetChairman();
    
    u8 byMemberNum = ptVMPParam->GetMaxMemberNum();
    u8 byMtId = 1;
    u8 byMember = 0;
    u8 byLoop = 0;
	// ����VCS���黭��ϳɳ�Աѡ��
	if (VCS_CONF == m_tConf.GetConfSource())
	{
		TMt tVmpMember[MAXNUM_VMP_MEMBER];
		memset(tVmpMember, 0, sizeof(TMt) * MAXNUM_VMP_MEMBER);
		if (ptVMPParam->IsVMPAuto())
		{
			u8 byStyle = GetVCSAutoVmpMember(tVmpMember);
			if (byStyle != VMP_STYLE_NONE)
			{
				ptVMPParam->SetVMPStyle(byStyle);
				cKDVVMPParam.m_byVMPStyle = byStyle;
			}
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[ChangeVmpParam]only auto vmp to vcs currently");
			return;
		}

		for (u8 byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++)
		{
			if (!tVmpMember[byLoop].IsNull())
			{
				cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(tVmpMember[byLoop].GetMtId()) );
				cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(tVmpMember[byLoop]);
				cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_AUDIENCE);				
				ptVMPParam->SetVmpMember(byMember, cKDVVMPParam.m_atMtMember[byMember]);
				byMember ++;
			}
		}
		cKDVVMPParam.m_byMemberNum = byMember;

	}
	else
	{
		if(ptVMPParam->IsVMPAuto())
		{        
			//������ˣ�������ϯ���������
			if(!tSpeakerMt.IsNull() && m_tConfAllMtInfo.MtJoinedConf(tSpeakerMt.GetMtId()))
			{
				// �ն˱���
				cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(tSpeakerMt.GetMtId()) );
				cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(tSpeakerMt);
				cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_SPEAKER);
				byMember ++;
			}
			if(!tChairMt.IsNull() &&
			   !(tChairMt == tSpeakerMt) &&
			   m_tConfAllMtInfo.MtJoinedConf(tChairMt.GetMtId()))
			{
				// �ն˱���
				cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(tChairMt.GetMtId()) );
				cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(tChairMt);
				cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_CHAIRMAN);
				byMember ++;
			}
			for (byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
			{
				if (byMember >= byMemberNum)
				{
					// ������
					break;
				}
				if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
				{
					if (byMtId == m_tConf.GetChairman().GetMtId())
					{
						// ��ϯ��ǰ��
						continue;
					}
					if (byMtId == GetLocalSpeaker().GetMtId() &&
						GetLocalSpeaker().GetType() != TYPE_MCUPERI)
					{
						// ��������ǰ��
						continue;
					}

					// �ն˱���
					cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(byMtId) );
					cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(m_ptMtTable->GetMt(byMtId));
					cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_AUDIENCE);
					byMember ++;
				}
			}

			for(byLoop = 0; byLoop < cKDVVMPParam.m_byMemberNum; byLoop ++)
			{
				ptVMPParam->SetVmpMember(byLoop, cKDVVMPParam.m_atMtMember[byLoop]);
			}
		}
		else
		{
			for (byLoop = 0; byLoop < ptVMPParam->GetMaxMemberNum(); byLoop++)
			{
				cKDVVMPParam.m_atMtMember[byLoop].SetMember(*ptVMPParam->GetVmpMember(byLoop)); 

				TMt tTempMt = *(TMt *)ptVMPParam->GetVmpMember(byLoop);    
				if( tTempMt.GetMtId() != 0 )
				{
					byMember ++;
				}
				if (tChairMt == tTempMt && ptVMPParam->GetVmpMember(byLoop)->GetMemberType() != VMP_MEMBERTYPE_SPEAKER )
				{
					cKDVVMPParam.m_atMtMember[byLoop].SetMemStatus(MT_STATUS_CHAIRMAN);
				}
				else if (tSpeakerMt == tTempMt)
				{
					cKDVVMPParam.m_atMtMember[byLoop].SetMemStatus(MT_STATUS_SPEAKER);
				}
				else
				{
					cKDVVMPParam.m_atMtMember[byLoop].SetMemStatus(MT_STATUS_AUDIENCE);
				}
				// �ն˱���
				cKDVVMPParam.m_atMtMember[byLoop].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(tTempMt.GetMtId()) );
			}
    
			//���������ն�
			if( !m_tConfAllMtInfo.MtJoinedConf( cKDVVMPParam.m_atMtMember[byLoop].GetMtId()))
			{
				cKDVVMPParam.m_atMtMember[byLoop].SetNull();
			}
		}
	}

	//�������ͨ���Ľ���
	u8 byMaxVMPMember = MAXNUM_MPUSVMP_MEMBER;
	if (ptVMPParam->IsVMPAuto() || VCS_CONF == m_tConf.GetConfSource())
	{
		byLoop = byMember;	
	}
	else
	{
		byLoop = ptVMPParam->GetMaxMemberNum();
	}
	
	for (; byLoop < byMaxVMPMember; byLoop++)
	{
		StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
		ptVMPParam->ClearVmpMember(byLoop);
		//tConfVMPParam.ClearVmpMember(byLoop);// meaning?
	}

	//���ϳɳ�Ա���µ�TPeriEqpStatus��
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = *ptVMPParam;
	g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );

    //���غ����͵Ȳ���
    TLogicalChannel tChnnl;
    BOOL32 bRet = FALSE;
    for (byLoop = 0; byLoop < /*ptVMPParam->GetMaxMemberNum()*/MAXNUM_SDVMP_MEMBER; byLoop++)
    {
	     //zbq[03/03/2009] �缶�ն˵��غ�ȡ��Ӧ��mcu
        TMt tTmpMt = (TMt)cKDVVMPParam.m_atMtMember[byLoop];
		if (tTmpMt.IsNull())
		{
			continue;
		}

        if (!tTmpMt.IsLocal())
        {
            tTmpMt = GetLocalMtFromOtherMcuMt(tTmpMt);
        }	

        bRet = m_ptMtTable->GetMtLogicChnnl(tTmpMt.GetMtId(), LOGCHL_VIDEO, &tChnnl, FALSE);
        if (bRet)
        {
            cKDVVMPParam.m_tVideoEncrypt[byLoop] = tChnnl.GetMediaEncrypt();
            cKDVVMPParam.m_tDoublePayload[byLoop].SetRealPayLoad(tChnnl.GetChannelType());
            cKDVVMPParam.m_tDoublePayload[byLoop].SetActivePayload(tChnnl.GetActivePayload());
        }
    }
	
	u8 byNeedPrs = m_tConf.GetConfAttrb().IsResendLosePack();
    
    if(g_bPrintEqpMsg)
    {
        OspPrintf(TRUE, FALSE, "[ChangeVmpParam] vmp param is followed:\n");
        cKDVVMPParam.Print();
    }    

	CServMsg cServMsg;
	cServMsg.SetConfId(m_tConf.GetConfId());
	cServMsg.SetMsgBody((u8*)&cKDVVMPParam, sizeof(cKDVVMPParam));
	
    //˫�ٵ���ʽ����ʱ�ڶ�ͨ������ý���ʽ
    if (0 != m_tConf.GetSecBitRate() && 
        MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
    {
        cKDVVMPParam.m_byEncType = m_tConf.GetMainVideoMediaType();
    }
    else
    {
        cKDVVMPParam.m_byEncType = m_tConf.GetSecVideoMediaType();
        
        //xsl [8/11/2006] mpeg2 4cif��2cif����
        m_tConf.GetVideoScale(cKDVVMPParam.m_byEncType, wWidth, wHeight);
        cKDVVMPParam.m_wVideoWidth = wWidth;
        cKDVVMPParam.m_wVideoHeight = wHeight;
        byRes = m_tConf.GetVideoFormat(cKDVVMPParam.m_byEncType);
        if( VIDEO_FORMAT_16CIF == byRes ||
            (MEDIA_TYPE_H262 == cKDVVMPParam.m_byEncType && VIDEO_FORMAT_4CIF == byRes))
        {
            cKDVVMPParam.m_wVideoWidth  = 352;
            cKDVVMPParam.m_wVideoHeight = 576;
        }        
        cKDVVMPParam.m_wVideoWidth  = htons(cKDVVMPParam.m_wVideoWidth);
        cKDVVMPParam.m_wVideoHeight = htons(cKDVVMPParam.m_wVideoHeight);
    }
    
    //modify bas 2
    // ���ٵ���ʽ���飬������0, zgc, 2008-03-19
    if ( MEDIA_TYPE_NULL == cKDVVMPParam.m_byEncType )
    {
        wMinMtReqBitrate = 0;
    }
    else
    {
        wMinMtReqBitrate = GetLeastMtReqBitrate(TRUE, cKDVVMPParam.m_byEncType);
        if (wMinMtReqBitrate < m_wBasBitrate || 0 == m_wBasBitrate)
        {
            m_wBasBitrate = wMinMtReqBitrate;
        }
    }
    cKDVVMPParam.m_wBitRate = htons(wMinMtReqBitrate);

	cServMsg.CatMsgBody((u8 *)&cKDVVMPParam, sizeof(cKDVVMPParam));
	cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs)); 

    //����ϳɷ��
    u8 bySchemeId = ptVMPParam->GetVMPSchemeId();
    TVmpStyleCfgInfo tMcuVmpStyle;    
    if(0 == bySchemeId)
    {
        EqpLog("[ChangeVmpParam] SchemeId :%d, use default\n", bySchemeId);
        tMcuVmpStyle.ResetDefaultColor();		
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
                ConfLog( FALSE, "[ChangeVmpParam] Get vmp cfg failed! use default(NoIdx)\n");                
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
                
                EqpLog("[ChangeVmpParam] get vmpstyle info success, SchemeId :%d\n", bySchemeId);
                EqpLog("[ChangeVmpParam] GRDColor.0x%x, AudFrmColor.0x%x, ChairFrmColor.0x%x, SpeakerFrmColor.0x%x\n",
                        tMcuVmpStyle.GetBackgroundColor(),
                        tMcuVmpStyle.GetFrameColor(),
                        tMcuVmpStyle.GetChairFrameColor(),
                        tMcuVmpStyle.GetSpeakerFrameColor() );
            }
        }
        else
        {
            tMcuVmpStyle.ResetDefaultColor();
            OspPrintf(TRUE, FALSE, "[ChangeVmpParam] Get vmp cfg failed! use default\n");
        }
    }
    tMcuVmpStyle.SetIsRimEnabled( ptVMPParam->GetIsRimEnabled() );

    tConfVMPParam.SetVMPSchemeId(bySchemeId);
	// �����Ƿ��б߿�ı�־, zgc, 2007-05-16
	tConfVMPParam.SetIsRimEnabled( ptVMPParam->GetIsRimEnabled() );

    m_tConf.m_tStatus.SetVmpParam(tConfVMPParam);

    cServMsg.CatMsgBody((u8*)&tMcuVmpStyle, sizeof(tMcuVmpStyle));

	//����Ϣ
	if (bStart)
	{
		// MCUǰ������޸�, zgc, 2007-09-27
		TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
		cServMsg.CatMsgBody((u8*)&tCapSupportEx, sizeof(tCapSupportEx));

		SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, 6*1000);
		SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_STARTVIDMIX_REQ, cServMsg);
	}
	else
	{
		SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_CHANGEVIDMIXPARAM_REQ, cServMsg);
	}

    m_tConfInStatus.SetVmpNotify(FALSE);
}

/*====================================================================
    ������      ��ProcVmpMcuMsg
    ����        ������ϳ�����Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/04    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ProcVmpMcuRsp( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:

        switch( pcMsg->event ) 
		{		
        case VMP_MCU_STARTVIDMIX_ACK:       //VMP��MCU��ʼ����ȷ��
        case VMP_MCU_STARTVIDMIX_NACK:      //VMP��MCU��ʼ�����ܾ�
		case VMP_MCU_STOPVIDMIX_ACK:        //VMP��MCUֹͣ����ȷ��
		case VMP_MCU_STOPVIDMIX_NACK:       //VMP��MCUֹͣ�����ܾ�
		case VMP_MCU_CHANGEVIDMIXPARAM_ACK: //VMP��MCU�ı临�ϲ���ȷ��
		case VMP_MCU_CHANGEVIDMIXPARAM_NACK://VMP��MCU�ı临�ϲ����ܾ�			
		case VMP_MCU_GETVIDMIXPARAM_ACK:    //VMP��MCU���͸��ϲ���ȷ��			
		case VMP_MCU_GETVIDMIXPARAM_NACK:   //VMP��MCU���͸��ϲ����ܾ�
			break;

        default:
			break;
        }

		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}	
}

/*====================================================================
    ������      ��ProcVmpRspWaitTimer
    ����        ��MCU�ȴ�VMPӦ��ʱ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/08    3.0         ������         ����
====================================================================*/
void CMcuVcInst::ProcVmpRspWaitTimer( const CMessage * pcMsg )
{
	KillTimer(MCUVC_VMP_WAITVMPRSP_TIMER);

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TPeriEqpStatus tPeriEqpStatus;

	switch( CurState() )
	{
	case STATE_ONGOING:

		if (!m_tVmpEqp.IsNull() &&
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
			tPeriEqpStatus.GetConfIdx() == m_byConfIdx)
		{
			if (TVmpStatus::WAIT_START== tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
			{
				tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
				g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
				// xliang [5/4/2009] �ϱ����
				cServMsg.SetMsgBody();
				cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );

				cServMsg.SetMsgBody();
				SendMsgToEqp( m_tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cServMsg );

				m_tVmpEqp.SetNull();
			}
			else if (TVmpStatus::WAIT_STOP== tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
			{
				ProcVMPStopSucRsp();
			}
			else
			{
				EqpLog("[ProcVmpRspWaitTimer] no steps needed in state(%d) for vmp(%d) in conf(%d)\n",
					   tPeriEqpStatus.m_tStatus.tVmp.m_byUseState, m_tVmpEqp.GetEqpId(), m_byConfIdx);

			}
		}
		break;

	default:
		ConfLog( FALSE, "[VmpRspWaitTimer]Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcVmpMcuNotif
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
void CMcuVcInst::ProcVmpMcuNotif(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	TVMPParam tVMPParam;
	memset( &tVMPParam, 0, sizeof(TVMPParam) );

    TEqp tEqp;
	TPeriEqpStatus tPeriEqpStatus;
	TMt tSrcMt;
	u8 byChlIdx;
    TPeriEqpRegReq tRegReq;

	switch( CurState() )
	{
	case STATE_ONGOING:

        switch( pcMsg->event ) 
		{
		case VMP_MCU_STARTVIDMIX_NOTIF:     //VMP��MCU��ʼ����֪ͨ
			{
			//�����ʱ
			KillTimer( MCUVC_VMP_WAITVMPRSP_TIMER );
			if (!m_tVmpEqp.IsNull() &&
				g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
				tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
				TVmpStatus::WAIT_START == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
			{
				tVMPParam = tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam;
				m_tConf.m_tStatus.SetVmpParam( tVMPParam );
				m_tConf.m_tStatus.SetVMPMode( tVMPParam.IsVMPAuto() ? CONF_VMPMODE_AUTO : CONF_VMPMODE_CTRL );

				//��ʼ�������� 
				ChangeVmpSwitch();

				// xliang [12/22/2008] ������ѯ��ʱ
				if (m_tConf.m_tStatus.m_tVMPParam.IsVMPBatchPoll() 
					//! 
					)
				{
					u32 dwTimerT = m_tVmpBatchPollInfo.GetFirstPollT();
					
					if(dwTimerT < VMP_BATCHPOLL_TMIN)
					{
						dwTimerT = VMP_BATCHPOLL_TMIN;
					}
					else if(dwTimerT > VMP_BATCHPOLL_TMAX)
					{
						dwTimerT = VMP_BATCHPOLL_TMAX;
					}
					
					SetTimer(MCUVC_VMPBATCHPOLL_TIMER,dwTimerT);
					
					EqpLog("SetTimer MCUVC_VMPBATCHPOLL_TIMER for T:%d s\n",dwTimerT/1000);
					
				}
				//ͬ�����踴��״̬
				tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
				tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::START;
				g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );

				
				//֪ͨ��ϯ�����л��
				cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam) );
				SendMsgToAllMcs( MCU_MCS_STARTVMP_NOTIF, cServMsg );

				if( HasJoinedChairman() )
				{
					SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STARTVMP_NOTIF, cServMsg );
				}

				m_tConfInStatus.SetVmpNotify(TRUE);       
    
				//FastUpdate to Vmp
				NotifyFastUpdate(m_tVmpEqp, 0);
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				u8 byMpuBoardVer = tPeriEqpStatus.m_tStatus.tVmp.m_byBoardVer;
				if (MPU_SVMP == byVmpSubType && MPU_BOARD_B256 == byMpuBoardVer)
				{
					NotifyFastUpdate(m_tVmpEqp, 1);
					NotifyFastUpdate(m_tVmpEqp, 2);
					NotifyFastUpdate(m_tVmpEqp, 3);
				}

				//n+1���ݸ���vmp��Ϣ
				if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
				{
					ProcNPlusVmpUpdate();
				}
			}
			else
			{
				OspPrintf(TRUE, FALSE, "[ProcVmpMcuNotif]Recv VMP_MCU_STARTVIDMIX_NOTIF in wrong state\n");
			}
            break;
			}

		case VMP_MCU_STOPVIDMIX_NOTIF:      //VMP��MCUֹͣ����֪ͨ
			KillTimer(MCUVC_VMP_WAITVMPRSP_TIMER);
			if (!m_tVmpEqp.IsNull() &&
				g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
				tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
				TVmpStatus::WAIT_STOP == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
			{
				ProcVMPStopSucRsp();
			}
			
			break;
			
		case VMP_MCU_CHANGESTATUS_NOTIF:    //VMP��MCU�ı临�ϲ���֪ͨ 
			{
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
				tVMPParam = tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam;
				m_tConf.m_tStatus.SetVmpParam( tVMPParam );
				m_tConf.m_tStatus.SetVMPMode( tVMPParam.IsVMPAuto() ? CONF_VMPMODE_AUTO : CONF_VMPMODE_CTRL );
				
				
				//�ı����ݽ���
				ChangeVmpSwitch( 0, VMP_STATE_CHANGE );            
				
				// xliang [12/23/2008] ������ѯ��ʱt.
				if(m_tConf.m_tStatus.m_tVMPParam.IsVMPBatchPoll())
				{
					u32 dwTimert = 0;
					if(m_tVmpBatchPollInfo.GetTmpt() != 0)//������ն˻�����ʱΪ0
					{
						dwTimert = m_tVmpBatchPollInfo.GetTmpt();
					}
					else
					{
						dwTimert = m_tVmpBatchPollInfo.GetPollt();
					}
					SetTimer(MCUVC_VMPBATCHPOLL_TIMER,dwTimert);
				}
				//ͬ�����踴��״̬
				tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
				g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );     
				
				//֪ͨmcs
				cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(m_tConf.m_tStatus.m_tVMPParam) );
				SendMsgToAllMcs(MCU_MCS_CHANGEVMPPARAM_NOTIF, cServMsg );
				
				m_tConfInStatus.SetVmpNotify(TRUE);
				
				//FastUpdate to Vmp
				NotifyFastUpdate(m_tVmpEqp, 0);
				u8 byVmpSubType	= tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				u8 byMpuBoardVer = tPeriEqpStatus.m_tStatus.tVmp.m_byBoardVer;
				if (MPU_SVMP == byVmpSubType && MPU_BOARD_B256 == byMpuBoardVer)
				{
					NotifyFastUpdate(m_tVmpEqp, 1);
					NotifyFastUpdate(m_tVmpEqp, 2);
					NotifyFastUpdate(m_tVmpEqp, 3);
				}
				
				// ����VCS�������������ϳɵ�ģʽ��,������Ա�뻭��ϳɳ�Ա��
				if (VCS_CONF == m_tConf.GetConfSource() && 
					VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode() &&
					m_tConf.m_tStatus.IsMixing())
				{
					TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
					u8 byMemNum = tVmpParam.GetMaxMemberNum();
					TMt tMt;
					TMt atMixMember[MAXNUM_CONF_MT];
					u8  byMixMemNum = 0;
					for (u8 byVMPMemIdx = 0; byVMPMemIdx < byMemNum; byVMPMemIdx++)
					{
						tMt = *(TMt*)(tVmpParam.GetVmpMember(byVMPMemIdx));
						if (!tMt.IsNull())
						{
							atMixMember[byMixMemNum++] = tMt;				
						}
						
					}
					ChangeSpecMixMember(atMixMember, byMixMemNum);
					
				}
				
				//n+1���ݸ���vmp��Ϣ
				if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
				{
					ProcNPlusVmpUpdate();
				}
				break;
			}

        case MCU_VMPCONNECTED_NOTIF:
            tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();

            tEqp.SetMcuEqp(tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());

            if (m_tVmpEqp.GetMtId() != tEqp.GetMtId() ||
                m_tVmpEqp.GetMcuId() != tEqp.GetMcuId() ||
                m_tVmpEqp.GetType() != tEqp.GetType())
            {
				// xliang [4/17/2009] ���쳣���ߵ�VMP��ָ�ǰ���û���������VMP�壬֮��
				// ��ǰ���ߵ�VMP�����߽���Ϊ����VMP�屸�á���ʱҪ����״̬��Ϊ"δʹ��"������й©
				/*
				if ( !m_tLastVmpEqp.IsNull() )
				{
					g_cMcuVcApp.GetPeriEqpStatus(m_tLastVmpEqp.GetEqpId(), &tPeriEqpStatus);
					tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = FALSE;
					g_cMcuVcApp.SetPeriEqpStatus( m_tLastVmpEqp.GetEqpId() , &tPeriEqpStatus );
				}*/
                return;
            }

            //����״̬			
            g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tPeriEqpStatus);  
            tVMPParam = tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam;

            if ( g_bPrintEqpMsg )
            {
                OspPrintf(TRUE, FALSE, "[Vmp Connected] tVMPParam is follows:\n");
                tVMPParam.Print();
            }
			
            cServMsg.SetEventId(MCS_MCU_STARTVMP_REQ);
            cServMsg.SetMsgBody((u8 *)&tVMPParam, sizeof(TVMPParam));
            VmpCommonReq(cServMsg);
            break;

		case MCU_VMPDISCONNECTED_NOTIF:
			{
	           tEqp = *(TEqp *)cServMsg.GetMsgBody();

				if (m_tVmpEqp.GetMtId() != tEqp.GetMtId() ||
					m_tVmpEqp.GetMcuId() != tEqp.GetMcuId() ||
					m_tVmpEqp.GetType() != tEqp.GetType())
				{
					return;
				}

				// xliang [4/17/2009] ��¼�쳣���ߵ�VMP
				// m_tLastVmpEqp = m_tVmpEqp;

				//ֹͣ���渴�Ϻ�VMPģ�岻����Ч
				m_tConfInStatus.SetVmpModuleUsed(TRUE);
				m_tConfInStatus.SetVmpNotify(FALSE);

				//zbq[12/05/2007] ͣ��Ӧģʽ�µĵ���
				if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
				{
					CServMsg cMsg;
					RollCallStop(cMsg);
				}

				m_tConf.m_tStatus.SetVMPMode( CONF_VMPMODE_NONE );	// xliang [4/17/2009] ����ChangeVmpSwitch֮ǰ
				
				//ֹͣ��������
				ChangeVmpSwitch( 0, VMP_STATE_STOP );

				//��ϯѡ��VMPͣ��
	// 			if(m_tConf.m_tStatus.GetVmpParam().IsVMPSeeByChairman())
	// 			{
	// 				OspPrintf(1,0,"cancel vmp to mt!\n");
	// 				StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO);
	// 			}

				//��������״̬��tPeriEqpStatus�����󲿷����ݣ��Ա��ָ���		
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
				tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
				g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
				
				//����״̬�ϱ����
				cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );
				m_tConf.m_tStatus.SetVmpParam( tVMPParam );
				m_tConf.m_tStatus.SetVMPMode( CONF_VMPMODE_NONE );

				//֪ͨ��ϯ�����л�� 
				cServMsg.SetMsgBody();
				cServMsg.SetMsgBody( (u8*)&tVMPParam, sizeof(TVMPParam) );
				SendMsgToAllMcs( MCU_MCS_STOPVMP_NOTIF, cServMsg );
				if( HasJoinedChairman() )
				{
					SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVMP_NOTIF, cServMsg );
				}
				
				SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
            
				if ( g_bPrintEqpMsg )
				{
					OspPrintf(TRUE, FALSE, "[Vmp Disconnected] m_tVMPParam is follows:\n");
					m_tConf.m_tStatus.m_tVMPParam.Print();
				}

				//����VCS���飬���ڻ���ϳ�״̬��ָ�����������
				// �����������ϳ�״̬��ָ���������ģʽ
				if (VCS_CONF == m_tConf.GetConfSource() &&
					(VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() ||
					 VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode()))
				{
					if (VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode())
					{
						RestoreVCConf(VCS_SINGLE_MODE);
					}
					else if (VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode())
					{
						RestoreVCConf(VCS_GROUPSPEAK_MODE);
					}

					// fxh �ͷŻ���ϳ���,�úϳ����������Զ���������ϳ�
					m_tVmpEqp.SetNull();
				}
				
				//n+1���ݸ���vmp��Ϣ
				if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
				{
					ProcNPlusVmpUpdate();
				}

				//ֹͣѡ����vmp��hdu
				TPeriEqpStatus tHduStatus;
				u8 byHduId = HDUID_MIN;
				while( byHduId >= HDUID_MIN && byHduId <= HDUID_MAX )
				{
					if (g_cMcuVcApp.IsPeriEqpValid(byHduId))
					{
						g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
						if (tHduStatus.m_byOnline == 1)
						{
							u8 byTmpMtId = 0;
							u8 byMtConfIdx = 0;
							u8 byMemberNum = tHduStatus.m_tStatus.tHdu.byChnnlNum;
							for(u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
							{
								byTmpMtId = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetMtId();
								byMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
								if (m_tVmpEqp.GetEqpId() == byTmpMtId && m_byConfIdx == byMtConfIdx)
								{
									StopSwitchToPeriEqp(byHduId, byLoop);
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = 0;
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
                    			}
               				}
							g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);
					
							cServMsg.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
							SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
						}
					}
					byHduId++;
				}	
				
				ConfStatusChange();	// xliang [5/4/2009] ����״̬�ϱ�

			}


				
			break;
			
		case VMP_MCU_NEEDIFRAME_CMD:

            byChlIdx = cServMsg.GetChnIndex();
            tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
            tSrcMt = *(TMt*)tVMPParam.GetVmpMember( byChlIdx );
            //�������ն�
            NotifyMtFastUpdate(tSrcMt.GetMtId(), MODE_VIDEO);
            break;
				
        default:
			break;
        }

		break;

	default:
		ConfLog( FALSE, "[VmpMcuNotif]Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

#ifdef _SATELITE_
    RefreshConfState();

#endif

}

/*------------------------------------------------------------------*/
/*                          VMP TVWall                              */
/*------------------------------------------------------------------*/

/*====================================================================
    ������      ��SetVmpChnnl
    ����        �����û���ϳ���ͨ�� 
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/10/08    4.0         libo          ����
====================================================================*/
void CMcuVcInst::SetVmpTwChnnl(TMt tMt, u8 byChnnl, u8 byMemberType, BOOL32 bMsgMcs)
{
    CServMsg cServMsg;

    TMt tLocalMt = GetLocalMtFromOtherMcuMt(tMt);

    //��ʼ��������
    StartSwitchToPeriEqp(tLocalMt, 0, m_tVmpTwEqp.GetEqpId(),
                         byChnnl, MODE_VIDEO, SWITCH_MODE_SELECT);

    if (!tMt.IsLocal())
    {
        OnMMcuSetIn(tMt, 0, SWITCH_MODE_SELECT);
    }

    //�ı�״̬
    TVMPMember tVMPMember;
    tVMPMember.SetMemberTMt(tMt);
    tVMPMember.SetMemberType(byMemberType);
    m_tConf.m_tStatus.m_tVmpTwParam.SetVmpMember(byChnnl, tVMPMember);

    //֪ͨ���
    if (bMsgMcs)
    {
        cServMsg.SetMsgBody((u8*)&m_tConf.m_tStatus.m_tVmpTwParam, sizeof(TVMPParam));
        SendMsgToAllMcs(MCU_MCS_VMPTWPARAM_NOTIF, cServMsg);

        SendConfInfoToChairMt();	
    }

    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
        //��Ŀ�ĵ�Rtcp������Դ
        TLogicalChannel tLogicalChannel;
        m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);

        u32 dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
        u16 wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();

        //��¼����Ŀ��
        SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tVmpTwEqp, byChnnl, SWITCH_MODE_SELECT);
    }
}

/*====================================================================
    ������      ��ChangeVmpTwSwitch
    ����        ���ı临�ϵ���ǽ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt * ptNewSrc, Դ,NULLΪ����ͨ��
	              u8 bySrcType ��Ա����
                  u8 byState �Ƿ���ֹͣ����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/10/08    4.0         libo          ����
====================================================================*/
void CMcuVcInst::ChangeVmpTwSwitch(u8 bySrcType, u8 byState)
{
    u8 byLoop;
    TVMPMember tVMPMember;
    TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpTwParam();
    u8 byMemberNum = tVMPParam.GetMaxMemberNum();

    //����ͨ��ֹͣ��VMP����
    if (VMPTW_STATE_STOP == byState)
    {
        for (byLoop = 0; byLoop < byMemberNum; byLoop++)
        {
            StopSwitchToPeriEqp(m_tVmpTwEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);

			tVMPMember = *(tVMPParam.GetVmpMember(byLoop));

            //Stop Resend Lose Pack
            if (m_tConf.GetConfAttrb().IsResendLosePack())
            {                
                //��Ŀ�ĵ�Rtcp������Դ
                TLogicalChannel tLogicalChannel;
                m_ptMtTable->GetMtLogicChnnl(tVMPMember.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);

                u32 dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
                u16 wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();

                g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIp, wDstPort);
            }

			ChangeMtVideoFormat( tVMPMember, &m_tLastVmpTwParam, FALSE );
        }
        return;
    }

    //��ؿ���ģʽ 
    TMt tMt;
    memset(&tMt, 0, sizeof(TMt));
    if (CONF_VMPTWMODE_CTRL == m_tConf.m_tStatus.GetVmpTwMode())
    {
        for(byLoop = 0; byLoop < byMemberNum; byLoop++)
        {
            tVMPMember = *tVMPParam.GetVmpMember(byLoop);
            // ���� [5/23/2006] �ο�Vmp����,���� MemberType���ж�
            if (0 == tVMPMember.GetMemberType() && tVMPMember.IsNull())
            {
                StopSwitchToPeriEqp(m_tVmpTwEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
            }
            else
            {
                switch(tVMPMember.GetMemberType()) 
                {
                case VMPTW_MEMBERTYPE_MCSSPEC:
                    if (m_tConfAllMtInfo.MtJoinedConf(tVMPMember.GetMtId()))
                    {
                        SetVmpTwChnnl((TMt)tVMPMember, byLoop, VMPTW_MEMBERTYPE_MCSSPEC);

						ChangeMtVideoFormat( tVMPMember, &tVMPParam );
                    }
                    break;

                case VMPTW_MEMBERTYPE_SPEAKER://ע��ı䷢����ʱ�ı佻��
                    // ���� [5/23/2006] ֻ�����ն˽���ͨ��������������豸
					if( HasJoinedSpeaker()  && GetLocalSpeaker().GetType() != TYPE_MCUPERI )
					{
						SetVmpTwChnnl( GetLocalSpeaker(), byLoop, VMPTW_MEMBERTYPE_SPEAKER );

						ChangeMtVideoFormat( tVMPMember, &tVMPParam );
					}
					else
					{
						StopSwitchToPeriEqp(m_tVmpTwEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
						
						tMt.SetNull();
						tVMPMember.SetMemberTMt(tMt);
						tVMPMember.SetMemberType(VMPTW_MEMBERTYPE_SPEAKER);
						m_tConf.m_tStatus.m_tVmpTwParam.SetVmpMember(byLoop, tVMPMember);
					}
					break;

				case VMPTW_MEMBERTYPE_CHAIRMAN://ע��ı���ϯʱ�ı佻��
					if( HasJoinedChairman() )
					{
						SetVmpTwChnnl( m_tConf.GetChairman(), byLoop, VMPTW_MEMBERTYPE_CHAIRMAN );

						ChangeMtVideoFormat( tVMPMember, &tVMPParam );
					}
					else
					{
						StopSwitchToPeriEqp( m_tVmpTwEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO );
						tMt.SetNull();
						tVMPMember.SetMemberTMt( tMt );
						tVMPMember.SetMemberType( VMPTW_MEMBERTYPE_CHAIRMAN );
						m_tConf.m_tStatus.m_tVmpTwParam.SetVmpMember( byLoop, tVMPMember);
					}
					break;

				case VMPTW_MEMBERTYPE_POLL://ע��ı���ѯ�㲥Դʱ�ı佻��
					if( m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO ||
						m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_SPEAKER)
					{	
						tMt = (TMt)m_tConf.m_tStatus.GetMtPollParam();
						
						SetVmpTwChnnl( tMt, byLoop, VMPTW_MEMBERTYPE_POLL );

						ChangeMtVideoFormat( tVMPMember, &tVMPParam );
					}
					else
					{
						StopSwitchToPeriEqp( m_tVmpTwEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO );

						tMt.SetNull();
						tVMPMember.SetMemberTMt( tMt );
						tVMPMember.SetMemberType( VMPTW_MEMBERTYPE_POLL );
						m_tConf.m_tStatus.m_tVmpTwParam.SetVmpMember( byLoop, tVMPMember);
					}
					break;

                default:
                    break;
                }
            }
        }
    }
	
	//���ĺϳɷֱ��ʲ���
	if (VMP_STATE_CHANGE == byState)
    {
        if ( CONF_VMPMODE_NONE != m_tLastVmpTwParam.GetVMPMode() )
        {
            for( s32 nIndex = 0; nIndex < m_tLastVmpTwParam.GetMaxMemberNum(); nIndex ++ )
			{
				TMt tVMPMemberOld = *(TMt *)m_tLastVmpTwParam.GetVmpMember(nIndex);
            
				//��ɾ���˵ĺϳɳ�Ա�ָ��䷢�ͷֱ���
				if ( !m_tConf.m_tStatus.m_tVmpTwParam.IsMtInMember(tVMPMemberOld) &&
					 m_tConfAllMtInfo.MtJoinedConf(tVMPMemberOld.GetMtId()) ) 
				{
					ChangeMtVideoFormat( tVMPMemberOld, &m_tLastVmpTwParam, FALSE );
				}
			}                          
        }
    }

	//���汾�λ���ϳɲ��������ڶ�̬�ֱ��ʻָ�
	memcpy(&m_tLastVmpTwParam, &m_tConf.m_tStatus.m_tVmpTwParam, sizeof(TVMPParam));
	
    if(g_bPrintEqpMsg)
    {
        m_tConf.m_tStatus.m_tVmpTwParam.Print();
    }

	CServMsg cServMsg;
	cServMsg.SetMsgBody((u8 *)&m_tConf.m_tStatus.m_tVmpTwParam, sizeof(TVMPParam));
	SendMsgToAllMcs(MCU_MCS_VMPTWPARAM_NOTIF, cServMsg);

    SendConfInfoToChairMt();

	return;
}


/*====================================================================
    ������      ��ChangeVmpTwChannelParam
    ����        ���ı���黭��ϳɷ�ʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/10/27    4.0         libo          ����
====================================================================*/
void CMcuVcInst::ChangeVmpTwChannelParam(TMt * ptNewSrc, u8 byMemberType)
{    
    //TVMPMember tTmpVmpMember;
    TVMPMember tVmpMember;
    TVMPParam  tVmpTwParam;    
    TMt        tMt;
    u8         byMaxMemberNum;
    //u8         byNewMemberChnl;
    //u8         byMemberChnl;

    BOOL32 bVmpTwParamChged = FALSE;

    memset(&tMt, 0, sizeof(TMt));
    tVmpTwParam = m_tConf.m_tStatus.GetVmpTwParam();
    byMaxMemberNum = tVmpTwParam.GetMaxMemberNum();

    for (u8 byLoop = 0; byLoop < byMaxMemberNum; byLoop++)
    {
        if (tVmpTwParam.GetVmpMember(byLoop)->GetMemberType() == byMemberType)
        {
            StopSwitchToPeriEqp(m_tVmpTwEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
            tVmpTwParam.ClearVmpMember(byLoop);

            // ���� [5/23/2006] ֻ�����ն˽���ͨ��������������豸
            if (ptNewSrc == NULL || ptNewSrc->GetType() == TYPE_MCUPERI)
            {
                tMt.SetNull();
            }
            else
            {
                tMt = *ptNewSrc;
            }

            tVmpMember.SetMemberTMt(tMt);
            tVmpMember.SetMemberType(byMemberType);
            tVmpTwParam.SetVmpMember(byLoop, tVmpMember);

            bVmpTwParamChged = TRUE;
        }
    }

    // zbq [06/05/2007] ͨ������������ʵ�ʸ��Ĳ��޸�ͨ������
    if ( bVmpTwParamChged )
    {
        m_tConf.m_tStatus.SetVmpTwParam(tVmpTwParam);
        
        TPeriEqpStatus tPeriEqpStatus;
        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId() , &tPeriEqpStatus);  
        tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpTwParam();
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId() , &tPeriEqpStatus);  
        
        ChangeVmpTwParam(&tVmpTwParam);
    }

    return;
}

/*====================================================================
    ������      ��ChangeVmpTwParam
    ����        �����ϵ���ǽ�����ı�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TVMPParam *ptVMPParam, ���ϵ���ǽ�ϳɲ���
				  BOOL32 bStart �Ƿ��ǿ�ʼ����ϳ�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/10/08    4.0         libo          ����
====================================================================*/
void CMcuVcInst::ChangeVmpTwParam(TVMPParam *ptVmpParam, BOOL32 bStart)
{
	if (ptVmpParam == NULL)
	{
		return;
	}

	TVMPParam tConfVmpParam;

    //���ò���
    CKDVVMPParam cVmpParam;
    memset(&cVmpParam, 0, sizeof(CKDVVMPParam));
    cVmpParam.m_byVMPStyle = ptVmpParam->GetVMPStyle();
    cVmpParam.m_byEncType = MEDIA_TYPE_H261;

    cVmpParam.m_wBitRate = htons(768);
    //CIF
    cVmpParam.m_wVideoWidth  = htons(352);
	cVmpParam.m_wVideoHeight = htons(288);
	cVmpParam.m_byMemberNum  = ptVmpParam->GetMaxMemberNum();

	for (u8 byLoop = 0; byLoop < ptVmpParam->GetMaxMemberNum(); byLoop++)
	{
		cVmpParam.m_atMtMember[byLoop].SetMember(*ptVmpParam->GetVmpMember(byLoop)) ;
		//���������ն�
		if (!m_tConfAllMtInfo.MtJoinedConf(cVmpParam.m_atMtMember[byLoop].GetMtId()))
		{
			cVmpParam.m_atMtMember[byLoop].SetNull();
		}
	}

	//�������ͨ���Ľ���
	tConfVmpParam = m_tConf.m_tStatus.GetVmpTwParam();
	for (u8 byLoop1 = ptVmpParam->GetMaxMemberNum(); byLoop1 < MAXNUM_SDVMP_MEMBER; byLoop1++)
	{
		StopSwitchToPeriEqp(m_tVmpTwEqp.GetEqpId(), byLoop1, TRUE, MODE_VIDEO);
		tConfVmpParam.ClearVmpMember(byLoop1);
	}
	m_tConf.m_tStatus.SetVmpTwParam(tConfVmpParam);

	u8 byNeedPrs = m_tConf.GetConfAttrb().IsResendLosePack();

    u8 byMtId = 1;
    for(u8 byIdx = 0; byIdx < ptVmpParam->GetMaxMemberNum(); byIdx++)
	{
        BOOL32 bRet = FALSE;
        TLogicalChannel tChnnl;

		TVMPMember tVMPMember = *ptVmpParam->GetVmpMember(byIdx);
		if (tVMPMember.GetMtId() != 0)
		{
            bRet = m_ptMtTable->GetMtLogicChnnl(tVMPMember.GetMtId(), LOGCHL_VIDEO, &tChnnl, FALSE);
            if (TRUE == bRet)
            {
                cVmpParam.m_tVideoEncrypt[byIdx] = tChnnl.GetMediaEncrypt();
                cVmpParam.m_tDoublePayload[byIdx].SetRealPayLoad(tChnnl.GetChannelType());
	            cVmpParam.m_tDoublePayload[byIdx].SetActivePayload(tChnnl.GetActivePayload());
            }
            byMtId++;   
		}      
	}

	CServMsg cServMsg;
	cServMsg.SetConfId(m_tConf.GetConfId());
	cServMsg.SetMsgBody((u8*)&cVmpParam, sizeof(cVmpParam));
	cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs));

	//����Ϣ
	if (bStart)
	{
		// MCUǰ�����, zgc, 2007-09-27
		TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
		cServMsg.CatMsgBody((u8*)&tCapSupportEx, sizeof(tCapSupportEx));

		//SetTimer(MCU_VMP_WAITVMPRSP_TIMER, 6*1000);
		SendMsgToEqp(m_tVmpTwEqp.GetEqpId(), MCU_VMPTW_STARTVIDMIX_REQ, cServMsg);
	}
	else
	{
		SendMsgToEqp(m_tVmpTwEqp.GetEqpId(), MCU_VMPTW_CHANGEVIDMIXPARAM_REQ, cServMsg);
	}
}


/*====================================================================
    ������      ��ProcVmpTwMcuRsp
    ����        �����ϵ���ǽ��Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/10/08    4.0         libo          ����
====================================================================*/
void CMcuVcInst::ProcVmpTwMcuRsp(const CMessage * pcMsg)
{
    if (STATE_ONGOING != CurState())
    {
        ConfLog(FALSE, "Wrong message %u(%s) received in state %u!\n", 
                       pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    switch( pcMsg->event ) 
    {
    case VMPTW_MCU_STARTVIDMIX_ACK:       //VMPTW��MCU��ʼ����ȷ��
    case VMPTW_MCU_STARTVIDMIX_NACK:      //VMPTW��MCU��ʼ�����ܾ�
    case VMPTW_MCU_STOPVIDMIX_ACK:        //VMPTW��MCUֹͣ����ȷ��
    case VMPTW_MCU_STOPVIDMIX_NACK:       //VMPTW��MCUֹͣ�����ܾ�
    case VMPTW_MCU_CHANGEVIDMIXPARAM_ACK: //VMPTW��MCU�ı临�ϲ���ȷ��
    case VMPTW_MCU_CHANGEVIDMIXPARAM_NACK://VMPTW��MCU�ı临�ϲ����ܾ�			
    case VMPTW_MCU_GETVIDMIXPARAM_ACK:    //VMPTW��MCU���͸��ϲ���ȷ��			
    case VMPTW_MCU_GETVIDMIXPARAM_NACK:   //VMPTW��MCU���͸��ϲ����ܾ�
        break;

    default:
        break;
    }
}


/*====================================================================
    ������      ��ProcVmpTwMcuNotif
    ����        ������Vmp��Mcu��֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/10/08    4.0         libo          ����
====================================================================*/
void CMcuVcInst::ProcVmpTwMcuNotif(const CMessage * pcMsg)
{
    if (STATE_ONGOING != CurState())
    {
        ConfLog(FALSE, "Wrong message %u(%s) received in state %u!\n", 
                       pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TVMPParam tVmpTwParam;
    memset(&tVmpTwParam, 0, sizeof(TVMPParam));

    TEqp tEqp;
    TPeriEqpStatus tPeriEqpStatus;
    TMt tSrcMt;
    u8  byChlIdx;
    TPeriEqpRegReq tRegReq;

    switch(pcMsg->event) 
    {
    case VMPTW_MCU_STARTVIDMIX_NOTIF:     //VMP��MCU��ʼ����֪ͨ
        //�����ʱ
        //KillTimer(MCU_VMP_WAITVMPRSP_TIMER);

        //����״̬			
        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);  
        tVmpTwParam = tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam;
        tVmpTwParam.SetVMPStyle(m_tConf.m_tStatus.GetVmpTwStyle());
        m_tConf.m_tStatus.SetVmpTwParam(tVmpTwParam);
        m_tConf.m_tStatus.SetVmpTwMode(CONF_VMPTWMODE_CTRL);

        //��ʼ�������� 
        ChangeVmpTwSwitch();

        //ͬ�����踴��״̬
        tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVmpTwParam;
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);

        //֪ͨ��ϯ�����л��
        cServMsg.SetMsgBody((u8*)&m_tConf.m_tStatus.m_tVmpTwParam, sizeof(TVMPParam));
        SendMsgToAllMcs(MCU_MCS_STARTVMPTW_NOTIF, cServMsg);  
        
        SendConfInfoToChairMt();

        NotifyFastUpdate(m_tVmpTwEqp, MODE_VIDEO);
        break;

    case VMPTW_MCU_STOPVIDMIX_NOTIF:      //VMP��MCUֹͣ����֪ͨ
        //����״̬			
        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);
        tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);

        m_tConf.m_tStatus.SetVmpTwMode(CONF_VMPTWMODE_NONE);

        //ֹͣ��������
        ChangeVmpTwSwitch(0, VMP_STATE_STOP);

        m_tConf.m_tStatus.SetVmpTwParam(tVmpTwParam);

        //ͬ�����踴��״̬
        tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVmpTwParam;
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);

        //֪ͨ��ϯ�����л��
        cServMsg.SetMsgBody((u8 *)&tVmpTwParam, sizeof(TVMPParam));
        SendMsgToAllMcs(MCU_MCS_STOPVMPTW_NOTIF, cServMsg); 
        
        SendMsgToAllMcs(MCU_MCS_VMPTWPARAM_NOTIF, cServMsg);
        
        SendConfInfoToChairMt();

        // guzh [6/9/2007] �����¼
        memcpy(&m_tLastVmpTwParam, &tVmpTwParam, sizeof(TVMPParam));
        break;

    case VMPTW_MCU_CHANGESTATUS_NOTIF:    //VMP��MCU�ı临�ϲ���֪ͨ 

        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);
        tVmpTwParam = tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam;
        m_tConf.m_tStatus.SetVmpTwParam(tVmpTwParam);
        m_tConf.m_tStatus.SetVmpTwMode(CONF_VMPTWMODE_CTRL);

        //�ı����ݽ���
        ChangeVmpTwSwitch( 0, VMP_STATE_CHANGE );

        //ͬ�����踴��״̬
        tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVmpTwParam;
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);        
        break;

    case MCU_VMPTWCONNECTED_NOTIF:
        tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();

        tEqp.SetMcuEqp(tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());

        //g_cMcuVcApp.GetPeriEqpStatus(tRegReq.GetEqpId(), &tTvWallStatus);
        if (m_tVmpTwEqp.GetMtId() != tEqp.GetMtId() ||
            m_tVmpTwEqp.GetMcuId() != tEqp.GetMcuId() ||
            m_tVmpTwEqp.GetType() != tEqp.GetType())
        {
            return;
        }

        //����״̬			
        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);  
        tVmpTwParam = tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam;

        cServMsg.SetEventId(MCS_MCU_STARTVMPTW_REQ);
        cServMsg.SetMsgBody((u8 *)&tVmpTwParam, sizeof(TVMPParam));
        VmpTwCommonReq(cServMsg);
        break;

    case MCU_VMPTWDISCONNECTED_NOTIF: //��Ҫ�޸ĵ�

        tEqp = *(TEqp *)cServMsg.GetMsgBody();

        if (m_tVmpTwEqp.GetMtId() != tEqp.GetMtId() ||
            m_tVmpTwEqp.GetMcuId() != tEqp.GetMcuId() ||
            m_tVmpTwEqp.GetType() != tEqp.GetType())
        {
            return;
        }

        //ֹͣ��������
        ChangeVmpTwSwitch(0, VMP_STATE_STOP);

        //����״̬			
        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);
        tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);
        //m_tConf.m_tStatus.SetVmpTwParam(tVmpTwParam);
        m_tConf.m_tStatus.SetVmpTwMode(CONF_VMPTWMODE_NONE);

        //֪ͨ��ϯ�����л�� 
        //cServMsg.SetMsgBody((u8*)&tVmpTwParam, sizeof(TVMPParam));
        cServMsg.SetMsgBody();
        SendMsgToAllMcs(MCU_MCS_STOPVMPTW_NOTIF, cServMsg);   
        
        memset(&tVmpTwParam, 0, sizeof(TVMPParam));
        tVmpTwParam.SetVMPMode(CONF_VMPTWMODE_NONE);
        cServMsg.SetMsgBody((u8*)&tVmpTwParam, sizeof(TVMPParam));
        SendMsgToAllMcs( MCU_MCS_VMPTWPARAM_NOTIF, cServMsg );
        
        SendConfInfoToChairMt();
        break;

    case VMPTW_MCU_NEEDIFRAME_CMD:

        byChlIdx = cServMsg.GetChnIndex();
        tVmpTwParam = m_tConf.m_tStatus.m_tVmpTwParam;
        tSrcMt = *(TMt *)tVmpTwParam.GetVmpMember(byChlIdx);
        //�������ն�
        NotifyMtFastUpdate(tSrcMt.GetMtId(), MODE_VIDEO);
        break;

    default:
        break;
    }
	
}

/*------------------------------------------------------------------*/
/*                           TV Wall                                */
/*------------------------------------------------------------------*/

/*====================================================================
    ������      ��ChangeTvWallSwitch
    ����        ����ʼ�����ǽ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt * ptSrc, Դ
                  u8 byEqpId,
                  u8 byChnlIdx, 
	              u8 bySrcType ��Ա����
                  u8 byState 
                  BOOL32 bNotify :�Ƿ������ǽ���跢��ֹͣ��Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/05/23    3.6         libo          ����
====================================================================*/
void CMcuVcInst::ChangeTvWallSwitch(TMt * ptSrc, u8 byTvWallId, u8 byChnlIdx,u8 byTWMemberType, u8 byState, BOOL32 bEqpStop)
{
    CServMsg      cServMsg;
    TTWSwitchInfo tTWSwitchInfo;
    TPeriEqpStatus tTWStatus;    
    BOOL32 bSndMtStatus = FALSE; 
    BOOL32 bMultiInTv = FALSE;    
    
    u8 byMode;
    u8 byTvWallOutputMode = g_cMcuVcApp.GetTvWallOutputMode(byTvWallId);

    if (TW_OUTPUTMODE_AUDIO == byTvWallOutputMode)
    {
        byMode = MODE_AUDIO;
    }
    else if (TW_OUTPUTMODE_VIDEO == byTvWallOutputMode)
    {
        byMode = MODE_VIDEO;
    }
    else if (TW_OUTPUTMODE_BOTH == byTvWallOutputMode)
    {
        byMode = MODE_BOTH;
    }
    else
    {
        byMode = MODE_VIDEO;
    }

    StopSwitchToPeriEqp(byTvWallId, byChnlIdx, FALSE, byMode);
        
    g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTWStatus);
    
    TMt tOldMt = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx];        
    if(!tOldMt.IsNull() && IsMtNotInOtherTvWallChnnl(tOldMt.GetMtId(), byTvWallId, byChnlIdx))
    {
        m_ptMtTable->SetMtInTvWall(tOldMt.GetMtId(), FALSE);
        bSndMtStatus = TRUE;
    }    
    // ���� [5/23/2006] ��Ա�������նˣ�������������
    if(NULL != ptSrc && !ptSrc->IsNull() && ptSrc->GetType() != TYPE_MCUPERI)
    {
        //��ack�и��±ȽϺ�
        m_ptMtTable->SetMtInTvWall(ptSrc->GetMtId()); 
        bSndMtStatus = TRUE;
    }
    if(bSndMtStatus)
    {
        MtStatusChange();
    }
        
    //������ͨ����Ϣ
    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType = byTWMemberType;
    // ���� [5/23/2006] ��Ա�������նˣ�������������
    if (ptSrc != NULL && ptSrc->GetType() == TYPE_MCUPERI)
    {
        ptSrc->SetNull();
    }
    if (ptSrc == NULL || ptSrc->IsNull())
    {
        tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].SetNull();
        tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].SetConfIdx(m_byConfIdx);
    }
    else
    {
        tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].SetMt(*ptSrc);
    }
    g_cMcuVcApp.SetPeriEqpStatus(byTvWallId, &tTWStatus);

    //status notification
    cServMsg.SetMsgBody((u8 *)&tTWStatus, sizeof(tTWStatus));
    SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);

    if ((TW_STATE_START == byState || TW_STATE_CHANGE == byState) && ptSrc != NULL)
    {
        TEqp tEqp = g_cMcuVcApp.GetEqp(byTvWallId);

        tTWSwitchInfo.SetSrcMt(*ptSrc);
        tTWSwitchInfo.SetDstMt(tEqp);
        tTWSwitchInfo.SetMode(byMode);

        tTWSwitchInfo.SetDstChlIdx(byChnlIdx);
        cServMsg.SetEventId(MCS_MCU_START_SWITCH_TW_REQ);
        SwitchSrcToDst(tTWSwitchInfo, cServMsg);
    }
    
    // ���� [5/25/2006] ���ﵱ������ն���NULL�������,����ֹͣ������ǽͨ��
    // ����ҵ��͵���ǽ�ڲ���¼��ͨ���ն˲�һ��
    // ������ǽ�ٴο�ʼ����ͨ������ʱ,���MCU���ʹ���ĵ���ǽ��Ա����
    // �����Ա�����������ҵ�����Ѿ��ÿյ��ǵ��Ӳ���֪��ֹͣ��ͨ��
    // FIXME: ���ڵ�����������Stop�Ŀ����Ƿ�ܴ�, Ϊʲôԭ���Ĵ���
    //        ֻ���� bEqpStop ������²�ȥֹͣ
    if ( (TW_STATE_STOP == byState ) || ptSrc == NULL )
    {
        cServMsg.SetChnIndex(byChnlIdx);
        cServMsg.SetEventId(MCU_TVWALL_STOP_PLAY_REQ);
        SendMsgToEqp(byTvWallId, MCU_TVWALL_STOP_PLAY_REQ, cServMsg);        
    }

    return;
}

/*------------------------------------------------------------------*/
/*                              HDU                                 */
/*------------------------------------------------------------------*/
/*====================================================================
    ������      ��StopHduBatchPollSwitch
    ����        ��һ���������ѯ������𽻻�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bStopPlay: �Ƿ��ڲ𽻻���ͬʱֹͣHDU����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/04/13    4.6.1         ���ֱ�        ����
====================================================================*/
void   CMcuVcInst::StopHduBatchPollSwitch( BOOL32 bStopPlay )
{
	u8 byHduChlNum = 0;
	u8 byHduEqpId = 0;
	u8 byChnlIdx = 0;
	u8 byChnlPollNum = m_tHduBatchPollInfo.GetChnlPollNum();
	TPeriEqpStatus tHduStatus;
	CServMsg cServMsg;
	for ( ; byHduChlNum < byChnlPollNum; byHduChlNum ++ )
	{
		byHduEqpId = m_tHduBatchPollInfo.m_tChnlBatchPollInfo[byHduChlNum].GetEqpId();
		byChnlIdx = m_tHduBatchPollInfo.m_tChnlBatchPollInfo[byHduChlNum].GetChnlIdx();
		if ( byHduEqpId >= HDUID_MIN && byHduEqpId <= HDUID_MAX )
		{
			g_cMcuVcApp.GetPeriEqpStatus( byHduEqpId, &tHduStatus );
			tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnlIdx].SetSchemeIdx( 0 );
			g_cMcuVcApp.SetPeriEqpStatus( byHduEqpId, &tHduStatus );

			if ( 0 == tHduStatus.m_byOnline )    // hdu������
			{
				tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetNull();
				g_cMcuVcApp.SetPeriEqpStatus( byHduEqpId, &tHduStatus );
				cServMsg.SetMsgBody( (u8*)&tHduStatus, sizeof(tHduStatus) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF,  cServMsg);
			}
			else
			{
				TMt tMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx];
				tMt = GetLocalMtFromOtherMcuMt( tMt );
                
                if (bStopPlay)
                {
                    ChangeHduSwitch( &tMt, byHduEqpId, byChnlIdx, TW_MEMBERTYPE_BATCHPOLL, TW_STATE_STOP );	
                }
                else
                {
                    ChangeHduSwitch( &tMt, byHduEqpId, byChnlIdx, TW_MEMBERTYPE_BATCHPOLL, TW_STATE_STOP, MODE_BOTH, TRUE );	
                }
			}
		}
		else
		{
			ConfLog( FALSE, "[StopHduBatchPollSwitch] stop hdu(%d) batch poll error!\n", byHduEqpId);
			return;
		}
	}

}

/*====================================================================
    ������      ��HduBatchPollOfOneCycle
    ����        ��hduһ�������������ѯ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����bFirstCycle --> �Ƿ��ǵ�һ���������ѯ
    ����ֵ˵��  ��TRUE --> �ɹ�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/04/13    4.6.1         ���ֱ�        ����
====================================================================*/
BOOL32 CMcuVcInst::HduBatchPollOfOneCycle( BOOL32 bFirstCycle )
{	
	CServMsg cServMsg;
	THduChnlPollInfo *ptHduChnlPollInfo = (THduChnlPollInfo*)m_tHduBatchPollInfo.m_tChnlBatchPollInfo;
	u8 byChnlPollNum = m_tHduBatchPollInfo.GetChnlPollNum();
	u8 byMtId = m_tHduBatchPollInfo.GetCurrentMtId() + 1;
	TPeriEqpStatus tHduStatus;
	u8 byLoop = 0;
	u8 byIndex = 0;
	u8 byEqpIdTemp = 0;
    u8 byChnlIdxTemp = 0;

	//�����ǰ�����������κ�����նˣ���������ѯ����
	for (byLoop = 1; byLoop<= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtInConf( byLoop ) )
		{
			break;
		}
	}
	if( MAXNUM_CONF_MT < byLoop )
	{
		return FALSE;
	}

	while(  byIndex < byChnlPollNum  )
	{

		for ( byLoop = byMtId; byLoop <= MAXNUM_CONF_MT; byLoop ++ )
		{
			if ( !m_tConfAllMtInfo.MtInConf( byLoop ) )
			{
				continue;
			}
			else
			{
				byMtId = byLoop;
				break;
			}
		}
		
// 		if ( 0 == byIndex && byLoop == MAXNUM_CONF_MT + 1)   //û�������ն�
// 		{
//             return FALSE;
// 		}

		// һ�����ڽ�����������һ��������
		if ( byMtId == m_tHduBatchPollInfo.GetCurrentMtId() + 1 && byMtId != byLoop)
		{
            if ( 0 != m_tHduBatchPollInfo.GetCycles() 
				 && m_tHduBatchPollInfo.GetCurrentCycle() < m_tHduBatchPollInfo.GetCycles()
				)
            {
				m_tHduBatchPollInfo.SetNextCycle();
				u8 byKeepCycles = m_tHduBatchPollInfo.GetCycles() - m_tHduBatchPollInfo.GetCurrentCycle();
                m_tHduPollSchemeInfo.SetCycles( byKeepCycles );
				m_tHduBatchPollInfo.SetCurrentMtId( 0 );
				byMtId = m_tHduBatchPollInfo.GetCurrentMtId() + 1;
				byIndex = 0;
				byLoop = 0;
				m_tConf.m_tStatus.GetHduPollInfo()->SetPollNum( byKeepCycles );
				ConfStatusChange();
                // ���߻����ѯ�����ݼ�
				cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof(m_tHduPollSchemeInfo) );
		        SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );
            }
			else if ( 0 == m_tHduBatchPollInfo.GetCycles() )      // ������ѯ
			{
                m_tHduBatchPollInfo.SetCurrentMtId( 0 );
                byMtId = m_tHduBatchPollInfo.GetCurrentMtId() + 1;
				byIndex = 0;
				byLoop = 0;
			}
			else if ( m_tHduBatchPollInfo.GetCurrentCycle() == m_tHduBatchPollInfo.GetCycles() )   // ��ѯ����
			{
				m_tHduPollSchemeInfo.SetCycles( 0 );
				m_tConf.m_tStatus.GetHduPollInfo()->SetPollNum( 0 );
				ConfStatusChange();
				cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof(m_tHduPollSchemeInfo) );
		        SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );
				return TRUE;
			}

			continue;
		}

		//  ���������ն˲���һ������
		if ( byLoop > MAXNUM_CONF_MT )
		{
			while( byIndex < byChnlPollNum )
			{
				byEqpIdTemp = ptHduChnlPollInfo[byIndex].GetEqpId();
				byChnlIdxTemp = ptHduChnlPollInfo[byIndex].GetChnlIdx();

           		g_cMcuVcApp.GetPeriEqpStatus( byEqpIdTemp, &tHduStatus );
                TMt  tSrcMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdxTemp];
				tSrcMt = GetLocalMtFromOtherMcuMt( tSrcMt ); 

                ChangeHduSwitch( &tSrcMt, byEqpIdTemp, \
						byChnlIdxTemp, TW_MEMBERTYPE_BATCHPOLL, TW_STATE_STOP, MODE_BOTH, TRUE );

                byIndex ++;				
			}

			break;
		}
		
		THduMember tHduMember;     //  ���治�ܽ��������ն�
		memset(&tHduMember, 0x0, sizeof(THduMember));
		tHduMember.byMemberType = TW_MEMBERTYPE_BATCHPOLL;
		
		BOOL32 bSendMtStatus = FALSE;
		TMt tOldMt;
		memset( &tOldMt, 0x0, sizeof( TMt ) );
		byEqpIdTemp = ptHduChnlPollInfo[byIndex].GetEqpId();
		byChnlIdxTemp = ptHduChnlPollInfo[byIndex].GetChnlIdx();
		g_cMcuVcApp.GetPeriEqpStatus( byEqpIdTemp, &tHduStatus );
		if ( 0 == tHduStatus.m_byOnline )  // Ϊ��hduͨ��Ԥ����Ӧ�նˣ���hdu���ߺ��Զ��ָ�����״̬
		{
			tOldMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdxTemp];        
			if(!tOldMt.IsNull() 
				&& IsMtNotInOtherHduChnnl(tOldMt.GetMtId(), byEqpIdTemp, byChnlIdxTemp)
				&& TYPE_MT == tOldMt.GetType())
			{
				m_ptMtTable->SetMtInHdu(tOldMt.GetMtId(), FALSE);
				bSendMtStatus = TRUE;
			}    
			
			if ( bSendMtStatus )
			{
			    MtStatusChange(0, TRUE);
			}
			
			memcpy( &tHduMember, &m_ptMtTable->GetMt( byMtId ), sizeof(TMt) );
			tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdxTemp] = tHduMember;               
			g_cMcuVcApp.SetPeriEqpStatus( byEqpIdTemp, &tHduStatus );

		}
		else
		{
			if ( m_tConfAllMtInfo.MtJoinedConf( byMtId ) )   // ����ն�ֱ�ӽ�����
			{
				TMt tLocalMt = GetLocalMtFromOtherMcuMt( m_ptMtTable->GetMt( byMtId ) ); 
				if ( bFirstCycle)
				{
					ChangeHduSwitch( &tLocalMt, byEqpIdTemp, \
						byChnlIdxTemp, TW_MEMBERTYPE_BATCHPOLL, TW_STATE_START, MODE_BOTH, TRUE);
				}
				else
				{
					ChangeHduSwitch( &tLocalMt, byEqpIdTemp, \
						byChnlIdxTemp, TW_MEMBERTYPE_BATCHPOLL, TW_STATE_CHANGE, MODE_BOTH, TRUE);
				}
			}
			else    
			{
				tOldMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdxTemp];        
				if(!tOldMt.IsNull() 
					&& IsMtNotInOtherHduChnnl(tOldMt.GetMtId(), byEqpIdTemp, byChnlIdxTemp)
					&& TYPE_MT == tOldMt.GetType())
				{					
					m_ptMtTable->SetMtInHdu(tOldMt.GetMtId(), FALSE);
					bSendMtStatus = TRUE;
				}    
                
				if ( bSendMtStatus )
				{
					MtStatusChange(0, TRUE);
				}

				memcpy( &tHduMember, &(m_ptMtTable->GetMt( byMtId )), sizeof(TMt) );
				tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdxTemp] = tHduMember;
				
				g_cMcuVcApp.SetPeriEqpStatus( byEqpIdTemp, &tHduStatus );

                cServMsg.SetMsgBody( (u8*)&tHduStatus, sizeof(tHduStatus) );
                SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );
				
			}
		}

        byMtId ++;
		byIndex ++;
	}

    m_tHduBatchPollInfo.SetCurrentMtId( (byMtId - 1) % MAXNUM_CONF_MT );
	
	return TRUE;	

}

/*====================================================================
    ������      ��ChangeHduSwitch
    ����        ����ʼ��Hdu��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt * ptSrc, Դ
                  u8 byEqpId,
                  u8 byChnlIdx, 
				  u8 byHduMemberType ��Ա����
                  u8 byState 
                  BOOL32 bNotify :�Ƿ������ǽ���跢��ֹͣ��Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/01/19    4.6.1         ���ֱ�        ����
====================================================================*/
void CMcuVcInst::ChangeHduSwitch(TMt * ptSrc,
                                 u8    byHduId,
                                 u8    byChnlIdx,
                                 u8    byHduMemberType,
                                 u8    byState,
                                 u8    byMode/* = MODE_BOTH*/,
                                 BOOL32 bBatchPoll/* = FALSE*/ )
{
	if ( NULL == ptSrc )
	{
		ConfLog( FALSE, "[CMcuVcInst] ptSrc is Null!\n");
	}

    CServMsg      cServMsg;
    THduSwitchInfo tHduSwitchInfo;
    TPeriEqpStatus tHduStatus;    
    BOOL32 bSndMtStatus = FALSE; 

	g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
    
    u8 byHduOutputMode = tHduStatus.m_tStatus.tHdu.byOutputMode;
	u8 byOldMode;

    if (HDU_OUTPUTMODE_BOTH == byHduOutputMode)
    {
        byOldMode = MODE_BOTH;
    }
    else if (HDU_OUTPUTMODE_VIDEO == byHduOutputMode)
    {
        byOldMode = MODE_VIDEO;
    }
    else if (HDU_OUTPUTMODE_AUDIO == byHduOutputMode)
    {
        byOldMode = MODE_AUDIO;
    }
    else
    {
        byOldMode = MODE_VIDEO;
    }
		
    StopSwitchToPeriEqp(byHduId, byChnlIdx, FALSE, byOldMode);
	
	if ( MODE_BOTH == byMode )
	{
		byHduOutputMode = HDU_OUTPUTMODE_BOTH;
	}
	else if ( MODE_VIDEO == byMode )
	{
		byHduOutputMode = HDU_OUTPUTMODE_VIDEO;
	}
	else if ( MODE_AUDIO == byMode )
	{
		byHduOutputMode = HDU_OUTPUTMODE_AUDIO;
	}
	else
	{
		byHduOutputMode = HDU_OUTPUTMODE_VIDEO;
	}
	
    tHduStatus.m_tStatus.tHdu.byOutputMode = byHduOutputMode;

    TMt tOldMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx];        
    if(!tOldMt.IsNull() 
		&& IsMtNotInOtherHduChnnl(tOldMt.GetMtId(), byHduId, byChnlIdx)
		&& TYPE_MT == tOldMt.GetType())
    {
        m_ptMtTable->SetMtInHdu(tOldMt.GetMtId(), FALSE);
        bSndMtStatus = TRUE;
    }    

    //��ack�и��±ȽϺ�
	if (NULL!=ptSrc 
		&& MT_TYPE_MT == ptSrc->GetMtType()
		&& TW_STATE_STOP != byState)
	{
		m_ptMtTable->SetMtInHdu(ptSrc->GetMtId()); 
		bSndMtStatus = TRUE;
	}
    
	if(bSndMtStatus)
    {
		if ( bBatchPoll )
		{
            MtStatusChange(0, TRUE);
		}
		else
		{
			MtStatusChange();
		}
    }
	
    //����ͨ����Ϣ
    tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType = byHduMemberType;
	tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnlIdx].SetChnIdx(byChnlIdx);
	if ( tHduStatus.m_tStatus.tHdu.byChnnlNum < MAXNUM_HDU_CHANNEL)
	{
	    tHduStatus.m_tStatus.tHdu.byChnnlNum++;
	}
    // ��Ա�������նˣ�������������, VMP ����
    if (ptSrc != NULL && ptSrc->GetType() == TYPE_MCUPERI && EQP_TYPE_VMP != ptSrc->GetEqpType())
    {
        ptSrc->SetNull();
    }
    if (ptSrc == NULL || ptSrc->IsNull())
    {
        tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetNull();
		tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetEqpType( 0 );
		tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetType( 0 );
        tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetConfIdx(m_byConfIdx);
    }
    else
    {
		if ( ptSrc->GetType() == TYPE_MCUPERI)
		{
            tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetMcuEqp(ptSrc->GetMcuId(), ptSrc->GetEqpId(), ptSrc->GetEqpType());
            tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetConfIdx( ptSrc->GetConfIdx() );
		}
		else
		{
			TMt tSrct = m_ptMtTable->GetMt( ptSrc->GetMtId() );
            memcpy( &tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx], &tSrct, sizeof(TMt));
		    tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType = byHduMemberType;
		}
	}
    
	if ( ( TW_STATE_STOP == byState || ptSrc == NULL )
		 &&(TW_MEMBERTYPE_MCSSPEC == byHduMemberType 
		    || TW_MEMBERTYPE_SWITCHVMP == byHduMemberType
			|| TW_MEMBERTYPE_TWPOLL == byHduMemberType
			)
		)
	{
		tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType = 0;
		tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetNull();
        tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetConfIdx( (u8)0 );
	}

	if ( TW_STATE_STOP == byState 
		&& ( TW_MEMBERTYPE_CHAIRMAN == byHduMemberType  
		     || TW_MEMBERTYPE_SPEAKER == byHduMemberType
			 || TW_MEMBERTYPE_BATCHPOLL == byHduMemberType
			)
		)
	{
		tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetNull();
	}

    g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);

    //status notification
    cServMsg.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
    SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
	
	if ((TW_STATE_START == byState || TW_STATE_CHANGE == byState) && ptSrc != NULL)
    {
        TEqp tEqp = g_cMcuVcApp.GetEqp(byHduId);
		
        tHduSwitchInfo.SetSrcMt(*ptSrc);
        tHduSwitchInfo.SetDstMt(tEqp);
        tHduSwitchInfo.SetMode(byMode);
		
        tHduSwitchInfo.SetDstChlIdx(byChnlIdx);
		u8 bySrcChnnl = 0; 
		if(TW_MEMBERTYPE_SWITCHVMP == byHduMemberType)	// xliang [6/23/2009] ѡ��VMP
		{	
			u8 byMediaType = m_tConf.GetMainVideoMediaType();
			u8 byRes = m_tConf.GetMainVideoFormat();
			bySrcChnnl = GetVmpOutChnnlByRes(byRes, m_tVmpEqp.GetEqpId(), byMediaType);
		}
		tHduSwitchInfo.SetSrcChlIdx(bySrcChnnl);

		cServMsg.SetEventId(MCS_MCU_START_SWITCH_HDU_REQ);

        SwitchSrcToDst(tHduSwitchInfo, cServMsg, (TW_STATE_CHANGE == byState) & bBatchPoll);
    }
    

    if (!bBatchPoll && 
        (TW_STATE_STOP == byState || ptSrc == NULL))
    {
        cServMsg.SetChnIndex(byChnlIdx);
        cServMsg.SetEventId(MCU_HDU_STOP_PLAY_REQ);
        SendMsgToEqp(byHduId, MCU_HDU_STOP_PLAY_REQ, cServMsg); 
    }
	
    return;
}

/*====================================================================
    ������      : IsMtNotInOtherHduChnnl
    ����        �Ƿ���Ҫ����ն���hduͨ����״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId   �ն�ID
	              u8 byHduId    HDUId
				  u8 byChnlId   ͨ��ID
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/01/19    4.6.1         ���ֱ�        ����
====================================================================*/
BOOL32 CMcuVcInst::IsMtNotInOtherHduChnnl(u8 byMtId, u8 byHduId, u8 byChnlId)
{
	TPeriEqpStatus tHduStatus;
    g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
	
    //�Ƿ��ڱ�����ǽ������ͨ����
    for(u8 byLp = 0; byLp < tHduStatus.m_tStatus.tHdu.byChnnlNum; byLp++)
    {
        if(byLp != byChnlId)
        {
            TMt tMtInTv = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byLp];
			// �����ж��ն����ڻ���ţ�����Ὣ����������ն�����
            if(tMtInTv.GetMtId() == byMtId && tMtInTv.GetConfIdx() == m_byConfIdx)
            {
                return FALSE;
            }
        }
    }
	
    //�Ƿ�����������ǽ��ͨ����
    for(u8 byEqpHduId = HDUID_MIN; byEqpHduId <= HDUID_MAX; byEqpHduId++)
    {
        if(byHduId == byEqpHduId)
            continue;
        
        g_cMcuVcApp.GetPeriEqpStatus(byEqpHduId, &tHduStatus);
        if( tHduStatus.m_byOnline )
        {
            for(u8 byLp = 0; byLp < tHduStatus.m_tStatus.tHdu.byChnnlNum; byLp++)
            {
                TMt tMtInTv = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byLp];
				// �����ж��ն����ڻ���ţ�����Ὣ����������ն�����
                if(tMtInTv.GetMtId() == byMtId && tMtInTv.GetConfIdx() == m_byConfIdx)
                {
                    return FALSE;
                }
            }
        }
    }

	return TRUE;
}

/*------------------------------------------------------------------*/
/*                              PRS                                 */
/*------------------------------------------------------------------*/
/*====================================================================
    ������      ��OccupyPrsChnl
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/07/15    4.0         ���㻪        ����
====================================================================*/
void CMcuVcInst::OccupyPrsChnl(u8 byPrsId, u8 byPrsChnl)
{
	TPeriEqpStatus tPrsStaus;
	g_cMcuVcApp.GetPeriEqpStatus(byPrsId, &tPrsStaus);
	tPrsStaus.m_tStatus.tPrs.SetChnConfIdx(byPrsChnl, m_byConfIdx);
	tPrsStaus.m_tStatus.tPrs.m_tPerChStatus[byPrsChnl].SetReserved(TRUE);
	g_cMcuVcApp.SetPeriEqpStatus(byPrsId, &tPrsStaus);

	CServMsg cServMsg;
	cServMsg.SetMsgBody((u8*)&tPrsStaus, sizeof(tPrsStaus));
	SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
}
/*====================================================================
    ������      ��RlsPrsChnl
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/07/15    4.0         ���㻪        ����
====================================================================*/
void CMcuVcInst::RlsPrsChnl(u8 byPrsId, u8 byPrsChnl)
{
	TPeriEqpStatus tPrsStaus;
	g_cMcuVcApp.GetPeriEqpStatus(byPrsId, &tPrsStaus);
	tPrsStaus.m_tStatus.tPrs.SetChnConfIdx(byPrsChnl, 0);
	tPrsStaus.m_tStatus.tPrs.m_tPerChStatus[byPrsChnl].SetReserved(FALSE);
	g_cMcuVcApp.SetPeriEqpStatus(byPrsId, &tPrsStaus);

	CServMsg cServMsg;
	cServMsg.SetMsgBody((u8*)&tPrsStaus, sizeof(tPrsStaus));
	SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);

	m_tConfPrsInfo.RemovePrsChnl(byPrsId, byPrsChnl);
}
/*====================================================================
    ������      ��ChangePrsSrc
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[IN] u8 byPrsId:      �����ĵ�PRS�豸��
	              [IN] u8 byPrsChnlPos: �����ĵ�PRSͨ����
				  [IN] TMt& tEqp:       ��Ӧ���յ��ź�ԴID
				  [IN] u8 byEqpOutChnl: ��Ӧ�ź�Դ���ͨ����
				  [IN] u8 byEqpInChnl:  ��Ӧ�ź�Դ����ͨ����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/07/15    4.0         ���㻪        ����
====================================================================*/
BOOL CMcuVcInst::ChangePrsSrc(u8 byPrsId, u8 byPrsChnlPos, 
							  const TMt& tEqp,  u8 byEqpOutChnl /*= 0*/, u8 byEqpInChnl /*= 0*/)
{
	u8 byChlNum = 0;
	TLogicalChannel tEqpLogChnl;
    if (!g_cMcuVcApp.GetPeriEqpLogicChnnl(byPrsId, MODE_VIDEO, &byChlNum, &tEqpLogChnl, TRUE))
    {
        ConfLog(FALSE, "[ChangePrsSrc] Can't get prs(%d) logic channel, ChangePrsSrc failure!\n",
			    byPrsId);
        return FALSE;
    }

	TPrsParam tPrsParam;
	TTransportAddr tLocAddr;
	TTransportAddr tRemAddr;
	u32 dwRtcpSwitchIp;
	u16 wRtcpSwitchPort;
	switch(tEqp.GetEqpType())
	{
	// Ŀǰֻ���HDbas
	case EQP_TYPE_BAS:
        //bas ����RTCP��������ַ
        g_cMpManager.GetBasSwitchAddr(tEqp.GetEqpId(), dwRtcpSwitchIp, wRtcpSwitchPort);
        tRemAddr.SetIpAddr(dwRtcpSwitchIp);
        tRemAddr.SetPort(wRtcpSwitchPort + PORTSPAN*(byEqpInChnl * MAXNUM_VOUTPUT + byEqpOutChnl) + 1);
        tPrsParam.SetRemoteAddr(tRemAddr);
        
        //���ö����ش�������RTP���ĵ�ַ
        tLocAddr.SetIpAddr(tEqpLogChnl.GetRcvMediaChannel().GetIpAddr());
        tLocAddr.SetPort(tEqpLogChnl.GetRcvMediaChannel().GetPort() + PORTSPAN * byPrsChnlPos);
        tPrsParam.SetLocalAddr(tLocAddr);
		break;
	default:
		ConfLog(FALSE,  "[ChangePrsSrc] unknown type(%d) of prssrc\n", tEqp.GetEqpType());
		return FALSE;
	    break;
	}

	EqpLog("[ChangePrsSrc] Set src(id:%d, type:%d) for prs(id:%d, chnl:%d)\n", 
		   tEqp.GetEqpId(), tEqp.GetEqpType(), byPrsId, byPrsChnlPos);
	CServMsg cServMsg;
    cServMsg.SetChnIndex(byPrsChnlPos);
    cServMsg.SetMsgBody((u8*)&tPrsParam, sizeof(tPrsParam));
    SendMsgToEqp(byPrsId, MCU_PRS_SETSRC_REQ, cServMsg);
	
	return TRUE;
}
/*====================================================================
    ������      ��ChangePrsSrc
    ����        ���ı䶪���ش��Ľ���Դ(�ն˽���RTCP�������ĵ�ַ, �����ش�������RTP���ĵ�ַ)
    �㷨ʵ��    ����ack�н��㲥Դrtp -> prs, �����MT��RTCP -> PRS,����ֻ�ı䲻��bas�����prsԴ��Ϣ��
                  ����prsԴ��Ϣ��bas�󶨴���,����ʼbasʱ�ı�prsԴ��ֹͣbasʱ
    ����ȫ�ֱ�����
    �������˵���� TMt tMt �㲥Դ�ն�
                    BOOL32 bSrcBas  Դ�Ƿ�Ϊbas
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/03/27    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ChangePrsSrc(TMt tMt, u8 byPrsChanMode, BOOL32 bSrcBas)
{
    EqpLog("[ChangePrsSrc] Start MtId.%d PrsChanMode.%d\n", tMt.GetMtId(), byPrsChanMode);

    CServMsg  cServMsg;
    TPrsParam tPrsParam;
    TPrsParam tPrsParam2;
    TPrsParam tPrsParamAud;
    TPrsParam tPrsParamAudBas;
    TPrsParam tPrsParamVidBas;
    TPrsParam tPrsParamBrBas;
    //TPrsParam tPrsParamHDVidBas;    // ������������Ƶ�����ش�����, zgc, 2008-08-13
    TLogicalChannel tMtLogicalChannel;
    TLogicalChannel tMtLogicalChannel2;
    TLogicalChannel tMtLogicalChannelAud;
    TLogicalChannel tEqpLogicalChannel;

    u8  byEqpType;
    u8  byChlNum;
    u32 dwEqpIP;
    TTransportAddr tLocalAddr;
    TTransportAddr tDstAddr;
    u32 dwRtcpSwitchIp;
    u16 wRtcpSwitchPort;

    //�Ƿ���Ԥ����Դ
    if (m_tPrsEqp.IsNull())
    {
        ConfLog(FALSE, "[ChangePrsSrc] End1 MtId.%d PrsChanMode.%d\n", tMt.GetMtId(), byPrsChanMode);

        return;
    }

    if (!g_cMcuVcApp.GetPeriEqpLogicChnnl(m_tPrsEqp.GetEqpId(), MODE_VIDEO, &byChlNum, &tEqpLogicalChannel, TRUE))
    {
        ConfLog(FALSE, "[ChangePrsSrc] Can't get mt logic channel, ChangePrsSrc failure!\n");
        return;
    }

    //��IP��ַ
    if (SUCCESS_AGENT == g_cMcuAgent.GetPeriInfo(m_tPrsEqp.GetEqpId(), &dwEqpIP, &byEqpType))
    {
        tLocalAddr.SetNetSeqIpAddr(dwEqpIP);
        tLocalAddr.SetPort(0 );
        tPrsParam.SetLocalAddr(tLocalAddr);
        tPrsParam2.SetLocalAddr(tLocalAddr);
        tPrsParamAud.SetLocalAddr(tLocalAddr);
        tPrsParamAudBas.SetLocalAddr(tLocalAddr);
        tPrsParamVidBas.SetLocalAddr(tLocalAddr);
        tPrsParamBrBas.SetLocalAddr(tLocalAddr);
    }

    //����Դ�Ƿ�Ϊbas
    if(bSrcBas)
    {
        TLogicalChannel tSrcLogicalChannel;
        
        if(EQP_CHANNO_INVALID != m_byPrsChnnlVidBas && !m_tVidBasEqp.IsNull())
        {                        
            //bas ����RTCP��������ַ
            g_cMpManager.GetBasSwitchAddr(m_tVidBasEqp.GetEqpId(), dwRtcpSwitchIp, wRtcpSwitchPort);
            tDstAddr.SetIpAddr(dwRtcpSwitchIp);
            tDstAddr.SetPort(wRtcpSwitchPort + PORTSPAN*m_byVidBasChnnl + 1);
            tPrsParamVidBas.SetRemoteAddr(tDstAddr);
            
            //���ö����ش�������RTP���ĵ�ַ
            tLocalAddr = tPrsParam.GetLocalAddr();
            tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVidBas);
            tPrsParamVidBas.SetLocalAddr(tLocalAddr);
        }
        
        if(EQP_CHANNO_INVALID != m_byPrsChnnlBrBas && !m_tBrBasEqp.IsNull())
        {
            //bas ����RTCP��������ַ
            g_cMpManager.GetBasSwitchAddr(m_tBrBasEqp.GetEqpId(), dwRtcpSwitchIp, wRtcpSwitchPort);
            tDstAddr.SetIpAddr(dwRtcpSwitchIp);
            tDstAddr.SetPort(wRtcpSwitchPort + PORTSPAN*m_byBrBasChnnl + 1);
            tPrsParamBrBas.SetRemoteAddr(tDstAddr);
            
            //���ö����ش�������RTP���ĵ�ַ
            tLocalAddr = tPrsParam.GetLocalAddr();
            tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlBrBas);
            tPrsParamBrBas.SetLocalAddr(tLocalAddr);
        }
        
        if(EQP_CHANNO_INVALID != m_byPrsChnnlAudBas && !m_tAudBasEqp.IsNull())
        {
            //bas ����RTCP��������ַ
            g_cMpManager.GetBasSwitchAddr(m_tAudBasEqp.GetEqpId(), dwRtcpSwitchIp, wRtcpSwitchPort);
            tDstAddr.SetIpAddr(dwRtcpSwitchIp);
            tDstAddr.SetPort(wRtcpSwitchPort + PORTSPAN*m_byAudBasChnnl + 3);
            tPrsParamAudBas.SetRemoteAddr(tDstAddr);
            
            //���ö����ش�������RTP���ĵ�ַ
            tLocalAddr = tPrsParam.GetLocalAddr();
            tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlAudBas);
            tPrsParamAudBas.SetLocalAddr(tLocalAddr);
        }

        //�������ش�������Ϣ
        if (PRSCHANMODE_AUDBAS == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlAudBas)
        {
            if (TYPE_MT == tMt.GetType())
            {
                cServMsg.SetSrcMtId(tMt.GetMtId());
                cServMsg.SetChnIndex(m_byPrsChnnlAudBas);
                cServMsg.SetMsgBody((u8*)&tPrsParamAudBas, sizeof(tPrsParamAudBas));
                SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_SETSRC_REQ, cServMsg);
                m_tConfInStatus.SetPrsChnlAudBasStart(TRUE);

                //��bas��Rtcp������Դmt
                m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tSrcLogicalChannel, FALSE);                    
                u32 dwDstIp = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
                u16 wDstPort = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetPort();
                
                SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tAudBasEqp, m_byAudBasChnnl, MODE_AUDIO, SWITCH_MODE_BROADCAST);
            }
        }
        
        //�������ش�������Ϣ
        if (PRSCHANMODE_VIDBAS == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlVidBas)
        {
            if(TYPE_MT == tMt.GetType())
            {
                cServMsg.SetSrcMtId(tMt.GetMtId());
                cServMsg.SetChnIndex(m_byPrsChnnlVidBas);
                cServMsg.SetMsgBody((u8*)&tPrsParamVidBas, sizeof(tPrsParamVidBas));
                SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_SETSRC_REQ, cServMsg);
                m_tConfInStatus.SetPrsChnlVidBasStart(TRUE);

                //��bas��Rtcp������Դmt
                m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tSrcLogicalChannel, FALSE);                    
                u32 dwDstIp = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
                u16 wDstPort = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetPort();
                
                SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tVidBasEqp, m_byVidBasChnnl, MODE_VIDEO, SWITCH_MODE_BROADCAST);
            }
        }
        
        //�������ش�������Ϣ
        if(PRSCHANMODE_BRBAS == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlBrBas)
        {
            if(TYPE_MT == tMt.GetType())
            {
                cServMsg.SetSrcMtId(tMt.GetMtId());
                cServMsg.SetChnIndex(m_byPrsChnnlBrBas);
                cServMsg.SetMsgBody((u8*)&tPrsParamBrBas, sizeof(tPrsParamBrBas));
                SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_SETSRC_REQ, cServMsg);
                m_tConfInStatus.SetPrsChnlBrBasStart(TRUE);

                //��bas��Rtcp������Դmt
                m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tSrcLogicalChannel, FALSE);                    
                u32 dwDstIp = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
                u16 wDstPort = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetPort();
                
                SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tBrBasEqp, m_byBrBasChnnl, MODE_VIDEO, SWITCH_MODE_BROADCAST);
            }
        }

        // FIXME���������䶪���ش�δ���,zgc
        if ( PRSCHANMODE_HDBAS_VID == byPrsChanMode /*&& m_cConfBasChnMgr.GetVidChnUseNum() > 0*/ )
        {
            if(TYPE_MT == tMt.GetType())
            {
                cServMsg.SetSrcMtId(tMt.GetMtId());
                cServMsg.SetChnIndex(m_byPrsChnnlBrBas);
                cServMsg.SetMsgBody((u8*)&tPrsParamBrBas, sizeof(tPrsParamBrBas));
                SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_SETSRC_REQ, cServMsg);
                m_tConfInStatus.SetPrsChnlBrBasStart(TRUE);
                
                //��bas��Rtcp������Դmt
                m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tSrcLogicalChannel, FALSE);                    
                u32 dwDstIp = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
                u16 wDstPort = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetPort();
                
                SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tBrBasEqp, m_byBrBasChnnl, MODE_VIDEO, SWITCH_MODE_BROADCAST);
            }
        }
        
        return;
    }

    //vmp�ڶ�·����prs����
    if (PRSCHANMODE_VMP2 == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlBrBas)
    {
        //vmp ����RTCP��������ַ            
        u32 dwEqpIP,dwRcvIp;
        u16 wRcvPort;
        g_cMpManager.GetSwitchInfo(tMt, dwRcvIp, wRcvPort, dwEqpIP);
        
        TEqpVMPInfo tVmpInfo;
        g_cMcuAgent.GetEqpVMPCfg( tMt.GetEqpId(), &tVmpInfo);

        tDstAddr.SetIpAddr(dwEqpIP);          
        tDstAddr.SetPort(tVmpInfo.GetEqpRecvPort() - PORTSPAN*2 + 1);          
        
        tPrsParam.SetRemoteAddr(tDstAddr);
        
        //���ö����ش�������RTP���ĵ�ַ
        tLocalAddr = tPrsParam.GetLocalAddr();
        tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlBrBas);
        tPrsParam.SetLocalAddr(tLocalAddr);

        cServMsg.SetSrcMtId(0);
        cServMsg.SetChnIndex(m_byPrsChnnlBrBas);
        cServMsg.SetMsgBody((u8*)&tPrsParam, sizeof(tPrsParam));
        SendMsgToEqp(m_tPrsEqp.GetEqpId() , MCU_PRS_SETSRC_REQ, cServMsg);
        m_tConfInStatus.SetPrsChnlBrBasStart(TRUE);
        EqpLog("[ChangePrsSrc] Msg MCU_PRS_SETSRC_REQ send to prs eqp %d chl %d!\n", m_tPrsEqp.GetEqpId(), m_byPrsChnnlBrBas);
    }
	
	// xliang [4/30/2009] ��·�����㲥Դ4������prs����
    if (PRSCHANMODE_VMPOUT1 == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut1)
    {
        //�㲥Դ����RTCP��������ַ            
        u32 dwEqpIP,dwRcvIp;
        u16 wRcvPort;
        g_cMpManager.GetSwitchInfo(tMt, dwRcvIp, wRcvPort, dwEqpIP);
        
        TEqpVMPInfo tVmpInfo;
        g_cMcuAgent.GetEqpVMPCfg( tMt.GetEqpId(), &tVmpInfo);
		
        tDstAddr.SetIpAddr(dwEqpIP);				
		//tDstAddr.SetIpAddr(tVmpInfo.GetIpAddr());	
        tDstAddr.SetPort(tVmpInfo.GetEqpRecvPort() + 1);          
        
        tPrsParam.SetRemoteAddr(tDstAddr);
        
        //���ö����ش�������RTP���ĵ�ַ
        tLocalAddr = tPrsParam.GetLocalAddr();
        tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut1);
        tPrsParam.SetLocalAddr(tLocalAddr);
		
        cServMsg.SetSrcMtId(0);
        cServMsg.SetChnIndex(m_byPrsChnnlVmpOut1);
        cServMsg.SetMsgBody((u8*)&tPrsParam, sizeof(tPrsParam));
        SendMsgToEqp(m_tPrsEqp.GetEqpId() , MCU_PRS_SETSRC_REQ, cServMsg);
        m_tConfInStatus.SetPrsChnlVmpOut1Start(TRUE); 
        EqpLog("[ChangePrsSrc] Msg MCU_PRS_SETSRC_REQ send to prs eqp %d chl %d(m_byPrsChnnlVmpOut1)!\n", m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut1);
    }

	if (PRSCHANMODE_VMPOUT2 == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut2)
    {
        //�㲥Դ����RTCP��������ַ            
        u32 dwEqpIP,dwRcvIp;
        u16 wRcvPort;
        g_cMpManager.GetSwitchInfo(tMt, dwRcvIp, wRcvPort, dwEqpIP);
        
        TEqpVMPInfo tVmpInfo;
        g_cMcuAgent.GetEqpVMPCfg( tMt.GetEqpId(), &tVmpInfo);
		
        tDstAddr.SetIpAddr(dwEqpIP);          
        tDstAddr.SetPort(tVmpInfo.GetEqpRecvPort() + PORTSPAN*1 + 1);          
        
        tPrsParam.SetRemoteAddr(tDstAddr);
        
        //���ö����ش�������RTP���ĵ�ַ
        tLocalAddr = tPrsParam.GetLocalAddr();
        tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut2);
        tPrsParam.SetLocalAddr(tLocalAddr);
		
        cServMsg.SetSrcMtId(0);
        cServMsg.SetChnIndex(m_byPrsChnnlVmpOut2);
        cServMsg.SetMsgBody((u8*)&tPrsParam, sizeof(tPrsParam));
        SendMsgToEqp(m_tPrsEqp.GetEqpId() , MCU_PRS_SETSRC_REQ, cServMsg);
		m_tConfInStatus.SetPrsChnlVmpOut2Start(TRUE);
        EqpLog("[ChangePrsSrc] Msg MCU_PRS_SETSRC_REQ send to prs eqp %d chl %d(m_byPrsChnnlVmpOut2)!\n", m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut2);
    }

	if (PRSCHANMODE_VMPOUT3 == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut3)
    {
        //�㲥Դ����RTCP��������ַ            
        u32 dwEqpIP,dwRcvIp;
        u16 wRcvPort;
        g_cMpManager.GetSwitchInfo(tMt, dwRcvIp, wRcvPort, dwEqpIP);
        
        TEqpVMPInfo tVmpInfo;
        g_cMcuAgent.GetEqpVMPCfg( tMt.GetEqpId(), &tVmpInfo);
		
        tDstAddr.SetIpAddr(dwEqpIP);          
        tDstAddr.SetPort(tVmpInfo.GetEqpRecvPort() + PORTSPAN*2 + 1);          
        
        tPrsParam.SetRemoteAddr(tDstAddr);
        
        //���ö����ش�������RTP���ĵ�ַ
        tLocalAddr = tPrsParam.GetLocalAddr();
        tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut3);
        tPrsParam.SetLocalAddr(tLocalAddr);
		
        cServMsg.SetSrcMtId(0);
        cServMsg.SetChnIndex(m_byPrsChnnlVmpOut3);
        cServMsg.SetMsgBody((u8*)&tPrsParam, sizeof(tPrsParam));
        SendMsgToEqp(m_tPrsEqp.GetEqpId() , MCU_PRS_SETSRC_REQ, cServMsg);
        m_tConfInStatus.SetPrsChnlVmpOut3Start(TRUE); 
        EqpLog("[ChangePrsSrc] Msg MCU_PRS_SETSRC_REQ send to prs eqp %d chl %d(m_byPrsChnnlVmpOut3)!\n", m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut3);
    }
	
	if (PRSCHANMODE_VMPOUT4 == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut4)
    {
        //�㲥Դ����RTCP��������ַ            
        u32 dwEqpIP,dwRcvIp;
        u16 wRcvPort;
        g_cMpManager.GetSwitchInfo(tMt, dwRcvIp, wRcvPort, dwEqpIP);
        
        TEqpVMPInfo tVmpInfo;
        g_cMcuAgent.GetEqpVMPCfg( tMt.GetEqpId(), &tVmpInfo);
		
        tDstAddr.SetIpAddr(dwEqpIP);          
        tDstAddr.SetPort(tVmpInfo.GetEqpRecvPort() + PORTSPAN*3 + 1);          
        
        tPrsParam.SetRemoteAddr(tDstAddr);
        
        //���ö����ش�������RTP���ĵ�ַ
        tLocalAddr = tPrsParam.GetLocalAddr();
        tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut4);
        tPrsParam.SetLocalAddr(tLocalAddr);
		
        cServMsg.SetSrcMtId(0);
        cServMsg.SetChnIndex(m_byPrsChnnlVmpOut4);
        cServMsg.SetMsgBody((u8*)&tPrsParam, sizeof(tPrsParam));
        SendMsgToEqp(m_tPrsEqp.GetEqpId() , MCU_PRS_SETSRC_REQ, cServMsg);
        m_tConfInStatus.SetPrsChnlVmpOut4Start(TRUE); 
        EqpLog("[ChangePrsSrc] Msg MCU_PRS_SETSRC_REQ send to prs eqp %d chl %d(m_byPrsChnnlVmpOut4)!\n", m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut4);
    }

    if (!tMt.IsNull())
    {	
        //���Կƴ��ն���Ч
        if (tMt.GetType() == TYPE_MT)
        {
            if (m_ptMtTable->GetManuId(tMt.GetMtId()) != MT_MANU_KDC && 
                m_ptMtTable->GetManuId(tMt.GetMtId()) != MT_MANU_KDCMCU)
            {
                EqpLog("[ChangePrsSrc] End2 MtId.%d PrsChanMode.%d\n", tMt.GetMtId(), byPrsChanMode);

                return;
            }

            //��һ·��Ƶ
            if (m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, FALSE))
            {

                //�����ն˽���RTCP�������ĵ�ַ
                tDstAddr = tMtLogicalChannel.GetSndMediaCtrlChannel();
                tPrsParam.SetRemoteAddr(tDstAddr);

                //���ö����ش�������RTP���ĵ�ַ(��MCU���ո��ն˵�RTP����ַһ��)
                tLocalAddr = tPrsParam.GetLocalAddr();
                tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnl);
                tPrsParam.SetLocalAddr(tLocalAddr);                    
            }

            //�ڶ�·��Ƶ
            if (m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_SECVIDEO, &tMtLogicalChannel2, FALSE))
            {
                //�����ն˽���RTCP�������ĵ�ַ
                tDstAddr = tMtLogicalChannel2.GetSndMediaCtrlChannel();
                tPrsParam2.SetRemoteAddr(tDstAddr);

                //���ö����ش�������RTP���ĵ�ַ(��MCU���ո��ն˵�RTP����ַһ��)
                tLocalAddr = tPrsParam2.GetLocalAddr();
                tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnl2);
                tPrsParam2.SetLocalAddr(tLocalAddr);
            }

            //��Ƶ
            if (m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tMtLogicalChannelAud, FALSE))
            {
                //�����ն˽���RTCP�������ĵ�ַ
                tDstAddr = tMtLogicalChannelAud.GetSndMediaCtrlChannel();
                tPrsParamAud.SetRemoteAddr(tDstAddr);

                //���ö����ش�������RTP���ĵ�ַ(��MCU���ո��ն˵�RTP����ַһ��)
                tLocalAddr = tPrsParamAud.GetLocalAddr();
                tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlAud);
                tPrsParamAud.SetLocalAddr(tLocalAddr);                    
            }              
        }
        else
        {
            //�����ն˽���RTCP�������ĵ�ַ
            u32 dwEqpIP,dwRcvIp;
            u16 wRcvPort;
            g_cMpManager.GetSwitchInfo(tMt, dwRcvIp, wRcvPort, dwEqpIP);
            tDstAddr.SetIpAddr(dwEqpIP);            
            if (tMt.GetEqpType() == EQP_TYPE_VMP)
            {               
                TEqpVMPInfo tVmpInfo;
                g_cMcuAgent.GetEqpVMPCfg( tMt.GetEqpId(), &tVmpInfo);
                    
                tDstAddr.SetPort(tVmpInfo.GetEqpRecvPort() - PORTSPAN + 1);
            }
            else
            {
                tDstAddr.SetPort(wRcvPort + 1);
            }            
            tPrsParam.SetRemoteAddr(tDstAddr);

            //���ö����ش�������RTP���ĵ�ַ(��MCU���ո��ն˵�RTP����ַһ��)
            tLocalAddr = tPrsParam.GetLocalAddr();
            tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnl);
            tPrsParam.SetLocalAddr(tLocalAddr);			
        }
    }
    else
    {
        memset(&tPrsParam, 0, sizeof(tPrsParam));
        memset(&tPrsParam2, 0, sizeof(tPrsParam2));
        memset(&tPrsParamAud, 0, sizeof(tPrsParamAud));
    }
	
    //�������ش�������Ϣ
    if ((PRSCHANMODE_FIRST == byPrsChanMode || PRSCHANMODE_BOTH == byPrsChanMode) &&
        (m_byPrsChnnl != EQP_CHANNO_INVALID))
    {
        if(tMt.GetType() == TYPE_MT)
        {
            cServMsg.SetSrcMtId(tMt.GetMtId());
        }
        else
        {
            cServMsg.SetSrcMtId(0);
        }
        cServMsg.SetChnIndex(m_byPrsChnnl);
        cServMsg.SetMsgBody((u8*)&tPrsParam, sizeof(tPrsParam));
        SendMsgToEqp(m_tPrsEqp.GetEqpId() , MCU_PRS_SETSRC_REQ, cServMsg);
        m_tConfInStatus.SetPrsChnl1Start(TRUE);
        EqpLog("[ChangePrsSrc] Msg MCU_PRS_SETSRC_REQ send to prs eqp %d chl %d!\n", m_tPrsEqp.GetEqpId(), m_byPrsChnnl);
    }

    //�������ش�������Ϣ
    if ((PRSCHANMODE_SECOND == byPrsChanMode || PRSCHANMODE_BOTH == byPrsChanMode) &&
        (m_byPrsChnnl2 != EQP_CHANNO_INVALID))
    {
        //�ڶ�·��Ƶ
        if (TYPE_MT == tMt.GetType())
        {
            if (m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_SECVIDEO, &tMtLogicalChannel2, FALSE))
            {
                //�������ش�������Ϣ
                cServMsg.SetSrcMtId(tMt.GetMtId());
                cServMsg.SetChnIndex(m_byPrsChnnl2);
                cServMsg.SetMsgBody((u8*)&tPrsParam2, sizeof(tPrsParam2));
                SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_SETSRC_REQ, cServMsg);
                m_tConfInStatus.SetPrsChnl2Start(TRUE);
            }
        }
    }

    //�������ش�������Ϣ
    if ((PRSCHANMODE_AUDIO == byPrsChanMode) && EQP_CHANNO_INVALID != m_byPrsChnnlAud)
    {
        //��Ƶ
        if (TYPE_MT == tMt.GetType())
        {
            if (m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tMtLogicalChannelAud, FALSE))
            {
                //�������ش�������Ϣ
                cServMsg.SetSrcMtId(tMt.GetMtId());
                cServMsg.SetChnIndex(m_byPrsChnnlAud);
                cServMsg.SetMsgBody((u8*)&tPrsParamAud, sizeof(tPrsParamAud));
                SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_SETSRC_REQ, cServMsg);
                m_tConfInStatus.SetPrsChnlAudStart(TRUE);
            }
        }
    }    
}
/*====================================================================
    ������      ��AddRemovePrsMember
    ����        �������Ƴ������ش��ĳ�Ա(MT.RTCP -> PRS)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/07/17    4.0         ���㻪        ����
====================================================================*/
void CMcuVcInst::AddRemovePrsMember(u8 byMemId, u8 byPrsId, u8 byPrsChl, u8 byPrsMode, u8 bAdd)
{
	u32 dwSrcRtcpIP, dwPrsRtcpIP, dwMapIpAddr = 0;
	u16 wSrcRtcpPort, wPrsRtcpPort, wMapPort  = 0;

	u8 byChlNum = 0;
	TLogicalChannel tPrsLogChnl;
	if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byPrsId, MODE_VIDEO, &byChlNum, &tPrsLogChnl, TRUE))
	{
		dwPrsRtcpIP  = tPrsLogChnl.GetRcvMediaChannel().GetIpAddr();
		wPrsRtcpPort = tPrsLogChnl.GetRcvMediaChannel().GetPort() + PORTSPAN*byPrsChl + 2;
	}
	else
	{
		EqpLog("[AddRemovePrsMember] Fail to find prs(%d) logchnl\n", byPrsId);
		return;
	}

	TLogicalChannel tLogChnl;
	u8  byFindMemRtcpChnl = FALSE;
	if (MODE_VIDEO == byPrsMode &&
		m_ptMtTable->GetMtLogicChnnl(byMemId, LOGCHL_VIDEO, &tLogChnl, TRUE))
	{
		byFindMemRtcpChnl = TRUE;
	}
	else if (MODE_SECVIDEO == byPrsMode &&
		     m_ptMtTable->GetMtLogicChnnl(byMemId, LOGCHL_SECVIDEO, &tLogChnl, TRUE))
	{
		byFindMemRtcpChnl = TRUE;
	}
	else if (MODE_AUDIO == byPrsMode &&
		     m_ptMtTable->GetMtLogicChnnl(byMemId, LOGCHL_AUDIO, &tLogChnl, TRUE))
	{
		byFindMemRtcpChnl = TRUE;
	}

	if (byFindMemRtcpChnl)
	{
		dwSrcRtcpIP  = tLogChnl.GetSndMediaCtrlChannel().GetIpAddr();
		wSrcRtcpPort = tLogChnl.GetSndMediaCtrlChannel().GetPort();
        dwMapIpAddr  = tLogChnl.GetRcvMediaChannel().GetIpAddr();
        wMapPort     = tLogChnl.GetRcvMediaChannel().GetPort();
	}
	else
	{
		EqpLog("[AddRemovePrsMember] Fail to find the logchnl(%d) for mt(%d)\n",
			    byPrsMode, byMemId);
		return;
	}

	// �Ƴ�ԭ����
    g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwSrcRtcpIP, wSrcRtcpPort, dwPrsRtcpIP, wPrsRtcpPort);

	// ��Դ��������ж��һ��������

    if (bAdd)
    {
        g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwSrcRtcpIP, 0, dwSrcRtcpIP,wSrcRtcpPort, 
					                     dwPrsRtcpIP, wPrsRtcpPort, 0, 0, dwMapIpAddr, wMapPort);
		EqpLog("[AddRemovePrsMember](%s, %d)--->", 
			   StrOfIP(dwSrcRtcpIP), wSrcRtcpPort);
		EqpLog("(%s, %d)\n", StrOfIP(dwPrsRtcpIP), wPrsRtcpPort);
    }

}
/*====================================================================
    ������      ��AddRemovePrsMember
    ����        �������Ƴ������ش��ĳ�Ա(MT.RTCP -> PRS)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt tMt �㲥Դ�ն�
				  BOOL32 bAdd TRUE-���� FALSE-�Ƴ�
				  BOOL32 bOnly2 �Ƿ�Ϊ�ڶ�·
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/03/27    3.0         ������        ����
====================================================================*/
void CMcuVcInst::AddRemovePrsMember( TMt tMt, BOOL32 bAdd, u8 byPrsChanMode )
{
    //�Ƿ���Ԥ����Դ
    if (m_tPrsEqp.IsNull() || tMt.IsNull()/* || !m_tConf.m_tStatus.IsPrsing()*/)
    {
        return;
    }

    u8  byChlNum  = 0;
    u16 wRecvPort = 0;
    u16 wMapPort  = 0;
    u16 wDstPort  = 0;
    u32 dwSrcIpAddr  = 0;
    u32 dwRecvIpAddr = 0;
    u32 dwDstIpAddr  = 0;
    TLogicalChannel tMtLogicalChannel;
    TLogicalChannel tMtLogicalChannel2;
    TLogicalChannel tMtLogicalChannelAud;
    TLogicalChannel tEqpLogicalChannel;

    //���Կƴ��ն���Ч
    u8 byManuID = m_ptMtTable->GetManuId(tMt.GetMtId());
    if (byManuID != MT_MANU_KDC && byManuID != MT_MANU_KDCMCU)
    {
        return;
    }

    //����PRS���ԣ�һ��PRS��֧�ֶ������Ƶͨ��(Ĭ���ܹ�16��),
    //��ͨ�����ն˿�ͳһΪPRS��ʼ���ն˿ڣ���ͨ��������ʵ�ʵĽ��ն˿�
    BOOL32 bPrsChan = g_cMcuVcApp.GetPeriEqpLogicChnnl(m_tPrsEqp.GetEqpId(), MODE_VIDEO, &byChlNum, &tEqpLogicalChannel, TRUE);

    if (PRSCHANMODE_FIRST == byPrsChanMode || PRSCHANMODE_BOTH == byPrsChanMode)
    {
        if (m_tConfInStatus.IsPrsChnl1Start() && bPrsChan &&
            m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, TRUE))
        {
            dwSrcIpAddr  = tMtLogicalChannel.GetRcvMediaChannel().GetIpAddr();      //�ն˵�rtp���յ�ַ��˿�
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //ת����rtcp���յ�ַ��˿�
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs��rtcp���յ�ַ��˿�
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnl+2; //�˶˿ڷ���������prsԴʱ����Ķ˿��йأ�prsģ����:����˿ڣ�2��

            if (bAdd)
            {
                //�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

    if (PRSCHANMODE_SECOND == byPrsChanMode || PRSCHANMODE_BOTH == byPrsChanMode)
    {
        //�ڶ�·��Ƶ
        if (m_tConfInStatus.IsPrsChnl2Start() && bPrsChan && 
            m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_SECVIDEO, &tMtLogicalChannel2, TRUE))
        {
            dwSrcIpAddr  = tMtLogicalChannel2.GetRcvMediaChannel().GetIpAddr();
            wMapPort     = tMtLogicalChannel2.GetRcvMediaChannel().GetPort();
            dwRecvIpAddr = tMtLogicalChannel2.GetSndMediaCtrlChannel().GetIpAddr();
            wRecvPort    = tMtLogicalChannel2.GetSndMediaCtrlChannel().GetPort();            
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnl2+2;

            if (bAdd)
            {
                //�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
                g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwSrcIpAddr, 0, dwRecvIpAddr,wRecvPort, 
					                             dwDstIpAddr, wDstPort, 0, 0, dwSrcIpAddr, wMapPort);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

    if (PRSCHANMODE_AUDIO == byPrsChanMode)
    {
        //��Ƶ
        if (m_tConfInStatus.IsPrsChnlAudStart() && bPrsChan && 
            m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tMtLogicalChannelAud, TRUE))
        {
            dwSrcIpAddr  = tMtLogicalChannelAud.GetRcvMediaChannel().GetIpAddr();
            wMapPort     = tMtLogicalChannelAud.GetRcvMediaChannel().GetPort();
            dwRecvIpAddr = tMtLogicalChannelAud.GetSndMediaCtrlChannel().GetIpAddr();
            wRecvPort    = tMtLogicalChannelAud.GetSndMediaCtrlChannel().GetPort();            
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlAud+2;

            if (bAdd)
            {
                //�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
                g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwSrcIpAddr, 0, dwRecvIpAddr,wRecvPort, 
					                             dwDstIpAddr, wDstPort, 0, 0, dwSrcIpAddr, wMapPort);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

    if (PRSCHANMODE_VIDBAS == byPrsChanMode)
    {
        if (m_tConfInStatus.IsPrsChnlVidBasStart() && bPrsChan &&
            m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, TRUE))
        {
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //ת����rtcp���յ�ַ��˿�
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs��rtcp���յ�ַ��˿�
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVidBas+2; //�˶˿ڷ���������prsԴʱ����Ķ˿��йأ�prsģ����:����˿ڣ�2��
            
            if (bAdd)
            {
                //�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

    //vmp.2
    if (PRSCHANMODE_BRBAS == byPrsChanMode || PRSCHANMODE_VMP2 == byPrsChanMode)
    {
        if (m_tConfInStatus.IsPrsChnlBrBasStart() && bPrsChan &&
            m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, TRUE))
        {            
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //ת����rtcp���յ�ַ��˿�
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs��rtcp���յ�ַ��˿�
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlBrBas+2; //�˶˿ڷ���������prsԴʱ����Ķ˿��йأ�prsģ����:����˿ڣ�2��
            
            if (bAdd)
            {
                //�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

    if (PRSCHANMODE_AUDBAS == byPrsChanMode)
    {
        if (m_tConfInStatus.IsPrsChnlAudBasStart() && bPrsChan &&
            m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tMtLogicalChannel, TRUE))
        {            
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //ת����rtcp���յ�ַ��˿�
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs��rtcp���յ�ַ��˿�
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlAudBas+2; //�˶˿ڷ���������prsԴʱ����Ķ˿��йأ�prsģ����:����˿ڣ�2��
            
            if (bAdd)
            {
                //�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_AUDIO, SWITCH_MODE_BROADCAST, TRUE);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

    // FIXME���������䶪���ش�δ���,zgc

	// xliang [4/30/2009] �������Ĺ㲥Դ�����ش�
	if (PRSCHANMODE_VMPOUT1 == byPrsChanMode)
    {
        if ( m_tConfInStatus.IsPrsChnlVmpOut1Start() 
			&& bPrsChan 
			&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, TRUE))
        {            
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //ת����rtcp���յ�ַ��˿�
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs��rtcp���յ�ַ��˿�
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut1+2; //�˶˿ڷ���������prsԴʱ����Ķ˿��йأ�prsģ����:����˿ڣ�2��
            
            if (bAdd)
            {
				EqpLog("begin to RTCP switch Mt.%u to PRS(Mode:%u, ip:%d, port:%d)!\n",tMt.GetMtId(), byPrsChanMode, dwDstIpAddr, wDstPort );
                //�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);

            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

	if (PRSCHANMODE_VMPOUT2 == byPrsChanMode)
    {
        if ( m_tConfInStatus.IsPrsChnlVmpOut2Start() 
			&& bPrsChan 
			&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, TRUE))
        {            
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //ת����rtcp���յ�ַ��˿�
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs��rtcp���յ�ַ��˿�
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut2+2; //�˶˿ڷ���������prsԴʱ����Ķ˿��йأ�prsģ����:����˿ڣ�2��
            
            if (bAdd)
            {
				EqpLog("begin to RTCP switch Mt.%u to PRS(Mode:%u, ip:%d, port:%d)!\n",tMt.GetMtId(), byPrsChanMode, dwDstIpAddr, wDstPort );
                //�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

	if (PRSCHANMODE_VMPOUT3 == byPrsChanMode)
    {
        if ( m_tConfInStatus.IsPrsChnlVmpOut3Start() 
			&& bPrsChan 
			&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, TRUE))
        {            
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //ת����rtcp���յ�ַ��˿�
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs��rtcp���յ�ַ��˿�
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut3+2; //�˶˿ڷ���������prsԴʱ����Ķ˿��йأ�prsģ����:����˿ڣ�2��
            
            if (bAdd)
            {
				EqpLog("begin to RTCP switch Mt.%u to PRS(Mode:%u, ip:%d, port:%d)!\n",tMt.GetMtId(), byPrsChanMode, dwDstIpAddr, wDstPort );
                //�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }
	if (PRSCHANMODE_VMPOUT4 == byPrsChanMode)
    {
        if ( m_tConfInStatus.IsPrsChnlVmpOut4Start() 
			&& bPrsChan 
			&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, TRUE))
        {            
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //ת����rtcp���յ�ַ��˿�
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs��rtcp���յ�ַ��˿�
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut4+2; //�˶˿ڷ���������prsԴʱ����Ķ˿��йأ�prsģ����:����˿ڣ�2��
            
            if (bAdd)
            {
				EqpLog("begin to RTCP switch Mt.%u to PRS(Mode:%u, ip:%d, port:%d)!\n",tMt.GetMtId(), byPrsChanMode, dwDstIpAddr, wDstPort );
                //�� MT.RTCP -> PRS ʱ�����ǵ�����ǽ֧�ţ��轫��������Դip��portӳ��Ϊ MT.RTP���Ա�PRS�ش�
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

    return;
}
/*====================================================================
    ������      ��ReRtcpSwithForPrs
    ����        �����µ�PRS��RTCP����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/07/17    4.0         ���㻪        ����
====================================================================*/
void CMcuVcInst::BuildRtcpAndRtpSwithForPrs(u8 byPrsId, u8 byPrsChl)
{
	TMt tPrsSrc;
	u8  byPrsSrcOutChnl = 0;
	u8  byPrsMode       = MODE_NONE;
	if (m_tConfPrsInfo.FindPrsChnlSrc(byPrsId, byPrsChl, byPrsMode, tPrsSrc, byPrsSrcOutChnl))
	{
		StartSwitchToPeriEqp(tPrsSrc, byPrsSrcOutChnl, 
								 byPrsId, byPrsChl, byPrsMode, SWITCH_MODE_SELECT);

		TMt tRealSrc;
		u8  byRealSrcOChnl = 0;
		for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{                
			if(m_tConfAllMtInfo.MtJoinedConf( byLoop ) && 
			   GetMtRealSrc(byLoop, byPrsMode, tRealSrc, byRealSrcOChnl) &&
			   tRealSrc == tPrsSrc && byRealSrcOChnl == byPrsSrcOutChnl)
			{
				AddRemovePrsMember( byLoop, byPrsId, byPrsChl, byPrsMode, TRUE);
			}
// 			else
// 			{
// 				AddRemovePrsMember( byLoop, byPrsId, byPrsChl, byPrsMode, FALSE);
// 			}
		}
	}
}
/*====================================================================
    ������      ��StopPrs
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/07/17    4.0         ���㻪        ����
====================================================================*/
void CMcuVcInst::StopPrs(u8 byPrsId, u8 byPrsChnl)
{
	CServMsg cServMsg;
    cServMsg.SetChnIndex(byPrsChnl);
    SendMsgToEqp(byPrsId, MCU_PRS_REMOVEALL_REQ, cServMsg);
     

    StopSwitchToPeriEqp(byPrsId, byPrsChnl, FALSE, MODE_VIDEO, SWITCH_MODE_SELECT);

	u32 dwPrsRtcpIP  = 0;
	u16 wPrsRtcpPort = 0;
	u8 byChlNum = 0;
	TLogicalChannel tPrsLogChnl;
	if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byPrsId, MODE_VIDEO,
		                                 &byChlNum, &tPrsLogChnl, TRUE))
	{
        dwPrsRtcpIP  = tPrsLogChnl.GetRcvMediaChannel().GetIpAddr();
        wPrsRtcpPort = tPrsLogChnl.GetRcvMediaChannel().GetPort() + PORTSPAN * byPrsChnl + 2;
        g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwPrsRtcpIP, wPrsRtcpPort);
        EqpLog("[StopPrs] Stop rtcp switch to (ip:%d port:%d) for prs(eqpid:%d, chnlid:%d)\n", 
			   StrOfIP(dwPrsRtcpIP), wPrsRtcpPort, byPrsId, byPrsChnl);

	}

}
/*====================================================================
    ������      ��StopPrs
    ����        ��ֹͣ�����ش�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/03/27    3.0         ������        ����
====================================================================*/
void CMcuVcInst::StopPrs(u8 byPrsChanMode, BOOL32 bNotify)
{
    u8  byChlNum;
    u16 wDstPort;
    u32 dwDstIpAddr;
    TLogicalChannel tEqpLogicalChannel;

    //�Ƿ���Ԥ����Դ
    if (m_tPrsEqp.IsNull() || !m_tConf.m_tStatus.IsPrsing())
    {
        return;
    }

    //�������ش�������Ϣ
    CServMsg cServMsg;
    TPeriEqpStatus tStatus;
    BOOL32 bLogicalChan = g_cMcuVcApp.GetPeriEqpLogicChnnl(m_tPrsEqp.GetEqpId(), MODE_VIDEO, &byChlNum, &tEqpLogicalChannel, TRUE);
    g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );

    //��һ·��Ƶ
    if ((byPrsChanMode == PRSCHANMODE_FIRST || byPrsChanMode == PRSCHANMODE_BOTH) &&
        m_tConfInStatus.IsPrsChnl1Start() && m_byPrsChnnl != EQP_CHANNO_INVALID)
    {		
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnl);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }        

        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnl, FALSE, MODE_VIDEO);

        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnl+2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlVid] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnl1Start(FALSE);
    }

    //�ڶ�·��Ƶ
    if ((byPrsChanMode == PRSCHANMODE_SECOND || byPrsChanMode == PRSCHANMODE_BOTH ) &&
        m_tConfInStatus.IsPrsChnl2Start() && m_byPrsChnnl2 != EQP_CHANNO_INVALID)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnl2);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }

        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnl2, FALSE, MODE_VIDEO);

        if (bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnl2+2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlVid2] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnl2Start(FALSE);
    }

    //��Ƶ
    if (byPrsChanMode == PRSCHANMODE_AUDIO && 
        m_tConfInStatus.IsPrsChnlAudStart() && EQP_CHANNO_INVALID != m_byPrsChnnlAud)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlAud);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }

        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlAud, FALSE, MODE_AUDIO);

        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlAud + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlAud] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlAudStart(FALSE);
    }

    //���������Ƶ�ش�
    if (PRSCHANMODE_AUDBAS == byPrsChanMode && 
        m_tConfInStatus.IsPrsChnlAudBasStart() && EQP_CHANNO_INVALID != m_byPrsChnnlAudBas)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlAudBas);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }
        
        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlAudBas, FALSE, MODE_AUDIO);
        
        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlAudBas + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlAudBas] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlAudBasStart(FALSE);
    }

    //��ý���������Ƶ�ش�
    if (PRSCHANMODE_VIDBAS == byPrsChanMode && 
        m_tConfInStatus.IsPrsChnlVidBasStart() && EQP_CHANNO_INVALID != m_byPrsChnnlVidBas)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlVidBas);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }
        
        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlVidBas, FALSE, MODE_VIDEO);
        
        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlVidBas + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlVidBas] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlVidBasStart(FALSE);
    }

    //�������������Ƶ�ش� or vmp.2
    if ((PRSCHANMODE_BRBAS == byPrsChanMode || PRSCHANMODE_VMP2 == byPrsChanMode) &&
        m_tConfInStatus.IsPrsChnlBrBasStart() && EQP_CHANNO_INVALID != m_byPrsChnnlBrBas)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlBrBas);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }
        
        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlBrBas, FALSE, MODE_VIDEO);
        
        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlBrBas + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlBrBas] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlBrBasStart(FALSE);
    }

    // FIXME���������䶪���ش�δ���,zgc
	
	// xliang [4/30/2009] ��·�����㲥Դ��Ƶ�ش�
    if (PRSCHANMODE_VMPOUT1 == byPrsChanMode 
		&& m_tConfInStatus.IsPrsChnlVmpOut1Start()  
		&& EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut1
		)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlVmpOut1);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }
        
        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut1, FALSE, MODE_VIDEO);
        
        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlVmpOut1 + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlBrd1080] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlVmpOut1Start(FALSE);
    }

	if (PRSCHANMODE_VMPOUT2 == byPrsChanMode 
		&& m_tConfInStatus.IsPrsChnlVmpOut2Start()  
		&& EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut2
		)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlVmpOut2);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }
        
        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut2, FALSE, MODE_VIDEO);
        
        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlVmpOut2 + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlBrd720] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlVmpOut2Start(FALSE);
    }

	if (PRSCHANMODE_VMPOUT3 == byPrsChanMode 
		&& m_tConfInStatus.IsPrsChnlVmpOut3Start()  
		&& EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut3
		)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlVmpOut3);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }
        
        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut3, FALSE, MODE_VIDEO);
        
        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlVmpOut3 + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlBrd4Cif] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlVmpOut3Start(FALSE);
    }

	if (PRSCHANMODE_VMPOUT4 == byPrsChanMode 
		&& m_tConfInStatus.IsPrsChnlVmpOut4Start()  
		&& EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut4
		)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlVmpOut4);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }
        
        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut4, FALSE, MODE_VIDEO);
        
        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlVmpOut4 + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlBrdCifOrOther] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlVmpOut4Start(FALSE);
    }

    g_cMcuVcApp.SetPeriEqpStatus(m_tPrsEqp.GetEqpId(), &tStatus);

    
	if ( (IsHDConf(m_tConf) && !m_tConfInStatus.IsHDPrsChnlAnyStart() )
		|| ( !IsHDConf(m_tConf) && !m_tConfInStatus.IsPrsChnlAnyStart() )
		)
    {
        m_tConf.m_tStatus.SetPrsing(FALSE);
    }

    //�Ƴ��������ݽ���

}

/*====================================================================
    ������      ��ProcPrsMcuRsp
    ����        �������ش���Ӧ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/03/27    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcPrsConnectedNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TPeriEqpRegReq tEqp = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();

    u8 byEqpId;
    u8 byLoop;
    TPeriEqpStatus tStatus;

	switch(CurState())
	{
	case STATE_ONGOING:

		//���״̬
		g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tStatus);
		tStatus.SetConfIdx(m_byConfIdx);
		for(byLoop=0; byLoop<MAXNUM_PRS_CHNNL; byLoop++)
		{
			tStatus.m_tStatus.tPrs.m_tPerChStatus[byLoop].SetReserved(FALSE);
		}
		g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tStatus);
		
		//��ʼ������Ҫ����
		if (m_tConf.GetConfAttrb().IsResendLosePack() && !m_tConf.m_tStatus.IsPrsing())
		{
			// if (g_cMcuVcApp.GetIdlePrsChl(byEqpId, byChnIdx, byChnIdx2, byChnIdxAud))
			TPrsChannel tPrsChannel;
			if( g_cMcuVcApp.GetIdlePrsChls( DEFAULT_PRS_CHANNELS, tPrsChannel) ) // 
			{
				byEqpId = tPrsChannel.GetPrsId();
				m_tPrsEqp.SetMcuEqp(LOCAL_MCUID, tPrsChannel.GetPrsId(), EQP_TYPE_PRS);
				m_tPrsEqp.SetConfIdx(m_byConfIdx);
				u8 byChnIdx = 0;
				m_byPrsChnnl  = tPrsChannel.m_abyPrsChannels[byChnIdx++];
				m_byPrsChnnl2 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
				m_byPrsChnnlAud = tPrsChannel.m_abyPrsChannels[byChnIdx];

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
				
				if (!m_tVidBrdSrc.IsNull())
				{
					ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_FIRST);
				}
				if (!m_tDoubleStreamSrc.IsNull())
				{
					ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_SECOND);					
				}

                if (!m_tAudBrdSrc.IsNull())
				{
					ChangePrsSrc(m_tAudBrdSrc, PRSCHANMODE_AUDIO);
				}

				ConfLog(FALSE, "Idle prs channel %d %d %d get success!\n",
                                m_byPrsChnnl, m_byPrsChnnl2, m_byPrsChnnlAud);
				
			}
		}
				
		break;

	default:
		ConfLog(FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState());
		break;
	}
}


/*====================================================================
    ������      ��ProcPrsMcuRsp
    ����        �������ش���Ӧ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/03/27    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcPrsDisconnectedNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		if( m_tConf.m_tStatus.IsPrsing() )
		{
			StopPrs( PRSCHANMODE_BOTH );
			StopPrs(PRSCHANMODE_AUDIO);
            StopPrs(PRSCHANMODE_VIDBAS);
            StopPrs(PRSCHANMODE_AUDBAS);
            StopPrs(PRSCHANMODE_BRBAS);
            //zbq [07/18/2007]
            StopPrs(PRSCHANMODE_VMP2);
			m_tConfInStatus.SetPrsChnlAllStop();
			
			// xliang [5/6/2009] 
			StopPrs(PRSCHANMODE_VMPOUT1);
			StopPrs(PRSCHANMODE_VMPOUT2);
			StopPrs(PRSCHANMODE_VMPOUT3);
			StopPrs(PRSCHANMODE_VMPOUT4);
			m_tConfInStatus.SetHDPrsChnlAllStop();
            
			m_tConf.m_tStatus.SetPrsing( FALSE );

            // FIXME���������䶪���ش�δ���,zgc
		}

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
		
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcPrsMcuRsp
    ����        �������ش���Ӧ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/03/27    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcPrsMcuRsp( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TLogicalChannel tMtLogicalChannel, tMtLogicalChannel2, tEqpLogicalChannel;
	u8  byMtId, byLoop;
	TMt tMt;
	u8  byPrsChl = cServMsg.GetChnIndex();
    u8  byAdpType;

	switch( pcMsg->event ) 
	{
	case PRS_MCU_SETSRC_ACK:			//PRS��MCU������ϢԴȷ��
		{
			u8  byPrsId  = *cServMsg.GetMsgBody();

			if( !m_tConf.m_tStatus.IsPrsing() )
			{
				m_tConf.m_tStatus.SetPrsing( TRUE );
			}
			//�ı����״̬
			if( byPrsChl == m_byPrsChnnl )
			{				
				for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
				{                
					if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) &&
						!IsMtSrcBas(byLoop, MODE_VIDEO, byAdpType) &&
						!IsMtSrcVmp2(byLoop))
					{
						tMt = m_ptMtTable->GetMt( byLoop );
						AddRemovePrsMember( tMt, TRUE, PRSCHANMODE_FIRST);
					}
				}
			}
			if( byPrsChl == m_byPrsChnnl2 )
			{
				for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
				{
					if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
					{
						tMt = m_ptMtTable->GetMt( byLoop );
						AddRemovePrsMember( tMt, TRUE, PRSCHANMODE_SECOND);
					}
				}
			}
			if (byPrsChl == m_byPrsChnnlAud)
			{
				for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
				{
					if (m_tConfAllMtInfo.MtJoinedConf(byLoop) &&
						!IsMtSrcBas(byLoop, MODE_AUDIO, byAdpType))
					{
						tMt = m_ptMtTable->GetMt(byLoop);
						AddRemovePrsMember(tMt, TRUE, PRSCHANMODE_AUDIO);
					}
				}
			}
			
			byMtId = cServMsg.GetSrcMtId();
			if (byMtId > 0)
			{
				tMt = m_ptMtTable->GetMt(byMtId);
			}
			
			//��������
			if (!m_tVidBrdSrc.IsNull())
			{
				//Դ��PRS: ֻ����Դ��PRS��RTP��,PRS��Դ�ķ���RTCP���ݲ�����
				if (byPrsChl == m_byPrsChnnl)
				{
					StartSwitchToPeriEqp( m_tVidBrdSrc, 0, m_tPrsEqp.GetEqpId(), m_byPrsChnnl, MODE_VIDEO );
				}
			}
			if (!m_tDoubleStreamSrc.IsNull())
			{
				if (byMtId > 0 && m_tDoubleStreamSrc.GetMtId() == byMtId)
				{
					if( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tMtLogicalChannel2, FALSE ) )
					{
						//StartSwitchToPeriEqp( m_tDoubleStreamSrc, 1, m_tPrsEqp.GetEqpId(), m_byPrsChnnl2, MODE_VIDEO );
						StartSwitchToPeriEqp(m_tDoubleStreamSrc, 0, m_tPrsEqp.GetEqpId(), m_byPrsChnnl2, MODE_SECVIDEO);
					}
				}
			}
			
			if (!m_tAudBrdSrc.IsNull())
			{
				//Դ��PRS: ֻ����Դ��PRS��RTP��,PRS��Դ�ķ���RTCP���ݲ�����
				if (byPrsChl == m_byPrsChnnlAud)
				{
					StartSwitchToPeriEqp(m_tAudBrdSrc, 0, m_tPrsEqp.GetEqpId(), m_byPrsChnnlAud, MODE_AUDIO);
				}
			}
			
			//vmp.2 �ڶ�·����������prs
			if (0 != m_tConf.GetSecBitRate() && 
				MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType() &&
				m_tVidBrdSrc == m_tVmpEqp)
			{
				if (byPrsChl == m_byPrsChnnlBrBas)
				{
					StartSwitchToPeriEqp( m_tVidBrdSrc, 1, m_tPrsEqp.GetEqpId(), m_byPrsChnnlBrBas, MODE_VIDEO );
				}            
			}
			
			// xliang [4/30/2009] ����VMP 4��
			u8 abyPrsChnnl[MAXNUM_MPU_OUTCHNNL] = {m_byPrsChnnlVmpOut1, m_byPrsChnnlVmpOut2, m_byPrsChnnlVmpOut3, m_byPrsChnnlVmpOut4};
			for( byLoop = 0; byLoop < MAXNUM_MPU_OUTCHNNL; byLoop++)
			{
				if(byPrsChl == abyPrsChnnl[byLoop])
				{
					StartSwitchToPeriEqp( m_tVidBrdSrc, 0+byLoop, m_tPrsEqp.GetEqpId(), abyPrsChnnl[byLoop], MODE_VIDEO );
					break;
				}
			}

			BuildRtcpAndRtpSwithForPrs(byPrsId, byPrsChl);

			break;
		}
	case PRS_MCU_REMOVEALL_ACK:			//PRS��MCU�ı临�ϲ���ȷ��	
		break;

	case PRS_MCU_SETSRC_NACK:			//PRS��MCU������ϢԴ�ܾ�
//	case PRS_MCU_ADDRESENDCH_NACK:		//PRS��MCUֹͣ�����ܾ�
//	case PRS_MCU_REMOVERESENDCH_NACK:	//PRS��MCU�ı临�ϲ����ܾ�
	case PRS_MCU_REMOVEALL_NACK:		//PRS��MCU�ı临�ϲ����ܾ�
		break;

	default:
		break;
	}
}

/*====================================================================
    ������      ��RestoreMtSelectStatus
    ����        ��ѡ��״̬�Ļָ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId   : Ҫ�ָ����ն�ID
                  u8 bySelMode: ѡ��ģʽ MODE_AUDIO, MODE_VIDEO
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/04/20    4.0         �ű���          ����
====================================================================*/
void CMcuVcInst::RestoreMtSelectStatus( u8 byMtId, u8 bySelMode )
{
/*    TSwitchInfo tSwitchInfo;
    TMtStatus tMtStatus;
    
    //�жϻָ�ԭѡ������
    if ( m_ptMtTable->GetMtStatus(byMtId, &tMtStatus) )
    {        
        TMt tMtSelect;
        tMtSelect = tMtStatus.GetSelectMt(bySelMode);

        //ѡ����Ϣ�ǿ�
        if ( !tMtSelect.IsNull() )
        {
            //��ѡ���ն����ߣ��ѽ����ָ���ѡ��ģʽ
            if (m_tConfAllMtInfo.MtJoinedConf(tMtSelect.GetMtId()))
            {
                tSwitchInfo.SetSrcMt( tMtSelect );
                tSwitchInfo.SetDstMt( m_ptMtTable->GetMt(byMtId) );
                tSwitchInfo.SetMode( bySelMode );
                
                CServMsg cServMsg;
                
                //���ｫ EventId ��Ϊ MCS_MCU_STARTSWITCHMT_REQ û��ʵ�����壬
                //  ֻ��Ϊ����Ӧ SwitchSrcToDst() �н���������������Ҳ������Ϊ
                //  MT_MCU_STARTSELMT_CMD �� MCS_MCU_STARTSWITCHMT_REQ. 
                cServMsg.SetEventId( MCS_MCU_STARTSWITCHMT_REQ );
                
                SwitchSrcToDst( tSwitchInfo, cServMsg );
            }
            //��ѡ���ն˵��ߣ����ѡ����Ϣ
            else
            {
                tMtSelect.SetNull();
                tMtStatus.SetSelectMt(tMtSelect, bySelMode);
                m_ptMtTable->SetMtStatus(byMtId, &tMtStatus);
            }
        }                        
    }*/

    return;
}
/*====================================================================
    ������      ��ChangeMtVideoFormat
    ����        ��H264���黭��ϳɣ��ֱ��ʵĵ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt&              tMt: Ҫ�������ն�
                  TVMPParam* ptVMPParam: ������ķֱ���
				  BOOL32         bStart: ������ָ�
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/04/29    4.0         ���㻪        ����
====================================================================*/
BOOL32 CMcuVcInst::ChangeMtVideoFormat( TMt tMt, TVMPParam * ptVMPParam, BOOL32 bStart/* = TRUE */,
									 BOOL32 bSeize/* = TRUE */, BOOL32 bConsiderVmpBrd /* = TRUE */)
{
	u8 byMtId = (tMt.IsLocal() ? tMt.GetMtId() : tMt.GetMcuId());

	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

	if(byVmpSubType == VMP)
	{
		ChgMtFormatInSdVmp(byMtId, ptVMPParam, bStart);
		return TRUE;
	}
	else
	{
		return ChgMtFormatInMpu(byMtId, ptVMPParam, bStart, bSeize, bConsiderVmpBrd);
	}
}


/*=============================================================================
�� �� ���� IsRecordSrcBas
��    �ܣ� ��ȡ����¼��������Դ���ж��Ƿ���������
�㷨ʵ�֣� ȱʡ��Ϊ����¼����Զ¼����ʽ��ͨ���ȽϹ㲥Դ����������¼��Ĳ�������
ȫ�ֱ����� 
��    ���� u8 byType        [In]  MODE_VIDEO, MODE_AUDIO
           TMt &tRetRecSrc  [Out]
           //u8  &byBasType   [Out] ADAPT_TYPE_VID, ADAPT_TYPE_BR ��/
           [IN/OUT]u8&   byRecChnlIdx:¼���ʵ�ʽ���ͨ�� 

�� �� ֵ�� BOOL32 �Ƿ��Bas¼��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
07/06/12    4.0         ����         ��д
09/07/13    4.0         ���㻪         ����HD bas
=============================================================================*/
BOOL32 CMcuVcInst::IsRecordSrcBas(  u8 byType, TMt &tRetRecSrc, /*u8 &byBasType*/ u8 byRecChnlIdx)
{
    BOOL32 bNeedBas = FALSE;
    TSimCapSet tConfMainSCS = m_tConf.GetCapSupport().GetMainSimCapSet();    
    TSimCapSet tSrcMtSCS;
    TMt tRecSrc;

#define END_RET_VALUE( bVal ) \
    {   \
        bNeedBas = bVal; \
        goto lbl_EndIsRecordSrcBas;    \
    }

	byRecChnlIdx = 0;
    if ( m_tConf.m_tStatus.IsBrdstVMP() && byType == MODE_VIDEO)
    {
        // ����ϳɹ㲥����Ҫ
        tRetRecSrc = m_tVmpEqp;
        END_RET_VALUE(FALSE);
    }
    if ( m_tConf.m_tStatus.IsMixing() && byType == MODE_AUDIO)
    {
        //�ڻ�������Ҫ
        tRetRecSrc = m_tMixEqp;
        END_RET_VALUE(FALSE);
    }
    if (byType == MODE_SECVIDEO)
    {
        //˫������Ҫ
        tRetRecSrc = m_tDoubleStreamSrc;
        END_RET_VALUE(FALSE);
    }
    
    // Ĭ���ǻ���㲥Դ���������ն˻��߻������
	// zgc, 2008-04-28, ������Ƶ��ѯ¼��ʧ��
    //tRecSrc = GetLocalSpeaker();
	if ( byType == MODE_VIDEO )
	{
		tRecSrc = m_tVidBrdSrc;
	}
	else if ( byType == MODE_AUDIO )
	{
		tRecSrc = m_tAudBrdSrc;
	}

    tRetRecSrc = tRecSrc;

    if ( !m_tConf.GetConfAttrb().IsUseAdapter() )
    {
        //û���������䣨���ٵ���ʽ������Ҫ
        END_RET_VALUE(FALSE);
    }

    if ( tRecSrc.GetType() == TYPE_MT )
    {
        // �ն�
        tSrcMtSCS = m_ptMtTable->GetSrcSCS(tRecSrc.GetMtId());
    }
    else
    {
        // ������(������ֱ��Ĭ������ʽ��������)
        tSrcMtSCS.SetAudioMediaType( tConfMainSCS.GetAudioMediaType() );
        tSrcMtSCS.SetVideoMediaType( tConfMainSCS.GetVideoMediaType() );
        tSrcMtSCS.SetVideoResolution( tConfMainSCS.GetVideoResolution() );
    }   

    switch (byType)
    {
    case MODE_AUDIO:
        {
            if (MEDIA_TYPE_NULL == m_tConf.GetSecAudioMediaType()) 
            {
                END_RET_VALUE(FALSE);
            }
            
            if ( tConfMainSCS.GetAudioMediaType() != tSrcMtSCS.GetAudioMediaType() )
            {
                tRetRecSrc   = m_tAudBasEqp;
				byRecChnlIdx = m_byAudBasChnnl;
                END_RET_VALUE(TRUE);
            }
            else
            {
                END_RET_VALUE(FALSE);
            }
        }
        break;
    case MODE_VIDEO:
        {
			if (IsHDConf(m_tConf))
			{
				TMt tHdBas;
				u8  byInChnlIdx, byOutChnIdx;
				u8  byMediaType = tConfMainSCS.GetVideoMediaType();
				u8  byRes       = tConfMainSCS.GetVideoResolution();
				if (tConfMainSCS.GetVideoMediaType() != tSrcMtSCS.GetVideoMediaType() ||
					m_tRecPara.IsRecLowStream())
				{
					do 
					{
						if (m_cBasMgr.GetBasResource(byMediaType, byRes,
													 tHdBas, byInChnlIdx, byOutChnIdx))
						{
							tRetRecSrc   = tHdBas;
							byRecChnlIdx = byInChnlIdx * MAXNUM_VOUTPUT + byOutChnIdx;
							END_RET_VALUE(TRUE);
						}
					} while(GetProximalGrp(byMediaType, byRes, byMediaType, byRes));
				}

				END_RET_VALUE(FALSE);
			}
			else
			{
				// ����
				if (m_tConf.GetSecBitRate() == 0)
				{
					if (tConfMainSCS.GetVideoMediaType() != tSrcMtSCS.GetVideoMediaType()   ||
						tConfMainSCS.GetVideoResolution() != tSrcMtSCS.GetVideoResolution())
					{
						tRetRecSrc   = m_tVidBasEqp;
						byRecChnlIdx = m_byVidBasChnnl; 
						END_RET_VALUE(TRUE);
					}    
					else
					{
						END_RET_VALUE(FALSE);                     
					}
				}
            
				// ˫�ٵ���ʽ
				if (MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType()) 
				{
					if ( !m_tRecPara.IsRecLowStream() )
					{
						END_RET_VALUE(FALSE);
					}
					// zgc, 2008-01-30, ���Դ�ķ������ʵ��ڻ���ڶ����ʣ�����Ҫ����
					else if ( tRecSrc.GetType() == TYPE_MT && m_ptMtTable->GetMtSndBitrate(tRecSrc.GetMtId()) == m_tConf.GetSecBitRate() )
					{
						END_RET_VALUE(FALSE);
					}
					else
					{
						tRetRecSrc   = m_tBrBasEqp;
						byRecChnlIdx = m_byBrBasChnnl; 
						END_RET_VALUE(TRUE);
					}
				}

				// ˫��˫��ʽ
				if (tConfMainSCS.GetVideoMediaType() == tSrcMtSCS.GetVideoMediaType() &&
					tConfMainSCS.GetVideoResolution() == tSrcMtSCS.GetVideoResolution() &&
					!m_tRecPara.IsRecLowStream())
				{
					// ��ʽ��ͬ����¼���٣�����Ҫ����
					END_RET_VALUE(FALSE);
				}
				else
				{
					// ¼���٣���Ҫ��ʽ����
					if (!m_tRecPara.IsRecLowStream())
					{
						tRetRecSrc   = m_tVidBasEqp;
						byRecChnlIdx = m_byVidBasChnnl;
						END_RET_VALUE(TRUE);
					}
					// ¼���٣����ֹ㲥Դ��������
					if ( tConfMainSCS.GetVideoMediaType() == tSrcMtSCS.GetVideoMediaType() &&
						 tConfMainSCS.GetVideoResolution() == tSrcMtSCS.GetVideoResolution() )
					{
						// zgc, 2008-01-30, ���Դ�ķ������ʵ��ڻ���ڶ����ʣ�����Ҫ����
						if ( tRecSrc.GetType() == TYPE_MT && m_ptMtTable->GetMtSndBitrate(tRecSrc.GetMtId()) == m_tConf.GetSecBitRate() )
						{
							END_RET_VALUE(FALSE);
						}
						else
						{
							tRetRecSrc   = m_tBrBasEqp;
							byRecChnlIdx = m_byBrBasChnnl;                   
						}
					}
					else
					{
						tRetRecSrc   = m_tVidBasEqp;
						byRecChnlIdx = m_byVidBasChnnl;
					}

					END_RET_VALUE(TRUE);
				}
			}
		}
        break;
    default:
        break;
    }

lbl_EndIsRecordSrcBas:

    EqpLog("[IsRecordSrcBas] byType.%d IsNeed.%d, RecSrc.(Type.%d, Id.%d), RecChnlIdx.%d\n",
           byType, bNeedBas, tRetRecSrc.GetType(), tRetRecSrc.GetEqpId(), byRecChnlIdx);

    return bNeedBas;

#undef END_RET_VALUE
}

/*=============================================================================
�� �� ���� IsRecordSrcHDBas
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/8/27   4.0		    �ܹ��                  ����
=============================================================================*/
BOOL32 CMcuVcInst::IsRecordSrcHDBas( void )
{
    BOOL32 bNeedBas = FALSE;
    TSimCapSet tConfMainSCS = m_tConf.GetCapSupport().GetMainSimCapSet();    
    TSimCapSet tSrcMtSCS;
    TMt tRecSrc;

#define END_RET_VALUE( bVal ) \
    {   \
        bNeedBas = bVal; \
        goto lbl_EndIsRecordSrcBas;    \
    }

    if ( !IsHDConf(m_tConf) )
    {
        // �Ǹ�����鲻��Ҫ
        END_RET_VALUE(FALSE);
    }

    if ( m_tConf.m_tStatus.IsBrdstVMP() )
    {
        // ����ϳɹ㲥����Ҫ
        END_RET_VALUE(FALSE);
    }

    if ( !m_tConf.GetConfAttrb().IsUseAdapter() )
    {
        //û���������䣨���ٵ���ʽ������Ҫ
        END_RET_VALUE(FALSE);
    }
    
    // Ĭ���ǻ�����Ƶ�㲥Դ���������ն˻��߻������
    tRecSrc = m_tVidBrdSrc;
    
    if ( tRecSrc.GetType() == TYPE_MT )
    {
        // �ն�
        tSrcMtSCS = m_ptMtTable->GetSrcSCS(tRecSrc.GetMtId());
    }
    else
    {
        // ������(������ֱ��Ĭ������ʽ��������)
        tSrcMtSCS.SetAudioMediaType( tConfMainSCS.GetAudioMediaType() );
        tSrcMtSCS.SetVideoMediaType( tConfMainSCS.GetVideoMediaType() );
        tSrcMtSCS.SetVideoResolution( tConfMainSCS.GetVideoResolution() );
    }   

    // ����
    if (m_tConf.GetSecBitRate() == 0)
    {
        END_RET_VALUE(FALSE);                     
    }
    
    // ˫�ٵ���ʽ
    if (MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType()) 
    {
        if ( !m_tRecPara.IsRecLowStream() )
        {
            END_RET_VALUE(FALSE);
        }
		// ���Դ�ķ������ʵ��ڻ���ڶ����ʣ�����Ҫ����
		else if ( tRecSrc.GetType() == TYPE_MT && m_ptMtTable->GetMtSndBitrate(tRecSrc.GetMtId()) == m_tConf.GetSecBitRate() )
		{
			END_RET_VALUE(FALSE);
		}
        else
        {
            END_RET_VALUE(TRUE);
        }
    }

    // ˫��˫��ʽ
    // FIXME��Ŀǰ������鲻֧�ִ��ڷ�H264˫��ʽ�Ļ���, zgc, 2008-08-27


lbl_EndIsRecordSrcBas:

    return bNeedBas;

#undef END_RET_VALUE
}

/*=============================================================================
    �� �� ���� IsDSSrcH263p
    ��    �ܣ� ˫��Դ�Ƿ�ΪH263p
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/12/12  4.5		    �ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::IsDSSrcH263p(void)
{
    TDStreamCap tDSrcCap;
    GetDSBrdSrcSim(tDSrcCap);
    return tDSrcCap.GetMediaType() == MEDIA_TYPE_H263PLUS ? TRUE : FALSE;
}

/*=============================================================================
�� �� ���� GetBasLastVCUTick
��    �ܣ� ����ͨ���Ż����һ�η��͹ؼ�֡�����TICK��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byBasChnnl
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/21   4.0		�ܹ��                  ����
=============================================================================*/
u32	CMcuVcInst::GetBasLastVCUTick( u8 byBasChnnl )
{
	u32 dwLastTick = 0;

	if ( byBasChnnl == m_byAudBasChnnl)
	{
		dwLastTick = m_dwAudBasChnnlLastVCUTick;
	}
	else if ( byBasChnnl == m_byVidBasChnnl )
	{
		dwLastTick = m_dwVidBasChnnlLastVCUTick;
	}
	else if ( byBasChnnl == m_byBrBasChnnl )
	{
		dwLastTick = m_dwBrBasChnnlLastVCUTick;
	}
	else if ( byBasChnnl == m_byCasdAudBasChnnl )
	{
		dwLastTick = m_dwCasdAudBasChnnlLastVCUTick;
	}
	else if ( byBasChnnl == m_byCasdVidBasChnnl )
	{
		dwLastTick = m_dwCasdVidBasChnnlLastVCUTick;
	}
	else
	{
		ConfLog( FALSE, "[SetBasLastVCUTick] Input bas channal(%d) is error! Last tick = 0\n", byBasChnnl );
		dwLastTick = 0;
	}

	return dwLastTick;
}

/*=============================================================================
�� �� ���� SetBasLastVCUTick
��    �ܣ� ����ͨ���Ż�����һ�η��͹ؼ�֡�����TICK��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u8 byBasChnnl
           u32 dwTick
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/21   4.0		�ܹ��                  ����
=============================================================================*/
void CMcuVcInst::SetBasLastVCUTick( u8 byBasChnnl, u32 dwTick )
{
	if ( byBasChnnl == m_byAudBasChnnl)
	{
		m_dwAudBasChnnlLastVCUTick = dwTick;
	}
	else if ( byBasChnnl == m_byVidBasChnnl )
	{
		m_dwVidBasChnnlLastVCUTick = dwTick;
	}
	else if ( byBasChnnl == m_byBrBasChnnl )
	{
		m_dwBrBasChnnlLastVCUTick = dwTick;
	}
	else if ( byBasChnnl == m_byCasdAudBasChnnl )
	{
		m_dwCasdAudBasChnnlLastVCUTick = dwTick;
	}
	else if ( byBasChnnl == m_byCasdVidBasChnnl )
	{
		m_dwCasdVidBasChnnlLastVCUTick = dwTick;
	}
	else
	{
		ConfLog( FALSE, "[SetBasLastVCUTick] Input bas channal(%d) is error!\n", byBasChnnl );
	}

	return;
}


/*====================================================================
    ������      ��ShowConfEqp
    ����        ����ӡ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/11    3.0         ������          ����
====================================================================*/
void CMcuVcInst::ShowConfEqp(void)
{
    //����ϳ�
    if (CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode())
    {
        ConfLog(FALSE, "  VMP:   Vmp eqp id %d\n", m_tVmpEqp.GetEqpId());        
    }

    //�໭�����ǽ
    if (CONF_VMPTWMODE_NONE != m_tConf.m_tStatus.GetVmpTwMode())
    {
        ConfLog(FALSE, "  VMPTW: VmpTw eqp id %d\n", m_tVmpTwEqp.GetEqpId());
    }

    //����
    if (m_tConf.m_tStatus.IsMixing() || m_tConf.m_tStatus.IsVACing())
    {
        ConfLog(FALSE, "  MIXER: Mixer eqp id %d, Mixer group id %d, MixMode %d \n",
                m_tMixEqp.GetEqpId(), m_byMixGrpId, m_tConf.m_tStatus.GetMixerMode());
        
        // guzh [6/8/2007]         
        if ( m_tConf.m_tStatus.IsSpecMixing() )
        {
            ConfLog(FALSE, "  MIXER: Specified Mix Member List: \n");
            for (u8 byLoop = 0; byLoop < MAXNUM_MIXER_CHNNL+1; byLoop ++)
            {
                if ( m_abyMixMtId[byLoop] != 0)
                {
                    OspPrintf(TRUE, FALSE, "\t\tIndex.%d Mt.%d\n",
                                            byLoop, m_abyMixMtId[byLoop]);
                }
            }
        }
    }

    //¼��
    if (!m_tConf.m_tStatus.IsNoRecording())
    {
        ConfLog(FALSE, "  REC:   Recorder eqp id %d, recorder channel %d \n  REC:   Recorder param:\n",
                m_tRecEqp.GetEqpId(), m_byRecChnnl);
        m_tRecPara.Print();
    }

    //����
    if (!m_tConf.m_tStatus.IsNoPlaying())
    {
        ConfLog(FALSE, "  REC:   Player eqp id %d, player channel %d \n  REC:   Player Attrib:\n",
                m_tPlayEqp.GetEqpId(), m_byPlayChnnl);
        m_tPlayEqpAttrib.Print();
    }

    //��Ƶ����
    if (m_tConf.m_tStatus.IsAudAdapting())
    {
        ConfLog(FALSE, "  BAS:   Audio adapter eqp id %d, Audio adapter channel %d \n",
                m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl);
    }

    //��Ƶ����
    if (m_tConf.m_tStatus.IsVidAdapting())
    {
        ConfLog(FALSE, "  BAS:   Video adapter eqp id %d, Video adapter channel %d, bitrate %d \n",
                m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, m_wVidBasBitrate);
    }

    //��������
    if (m_tConf.m_tStatus.IsBrAdapting())
    {
        ConfLog(FALSE, "  BAS:   BitRate adapter eqp id %d, BitRate adapter channel %d, bitrate %d \n",
                m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, m_wBasBitrate);
    }

    //������Ƶ����
    if (m_tConf.m_tStatus.IsCasdAudAdapting())
    {
        ConfLog(FALSE, "  BAS:   Cascade Audio adapter eqp id %d, Audio adapter channel %d\n",
            m_tCasdAudBasEqp.GetEqpId(), m_byCasdAudBasChnnl);
    }
    
    //������Ƶ����
    if (m_tConf.m_tStatus.IsCasdVidAdapting())
    {
        ConfLog(FALSE, "  BAS:   Cascade Video adapter eqp id %d, Video adapter channel %d\n",
            m_tCasdVidBasEqp.GetEqpId(), m_byCasdVidBasChnnl);
    }

    //�����ش�
    if( m_tConf.m_tStatus.IsPrsing())
    {
        ConfLog(FALSE, "  PRS:   Prs eqp id %d, VidChnnl %d, VidChnnl2 %d, AudChnnl %d, AudBasChnnl %d, VidBasChnnl %d, BrBasChnnl %d \n",
                m_tPrsEqp.GetEqpId(), m_byPrsChnnl, m_byPrsChnnl2, m_byPrsChnnlAud, 
                m_byPrsChnnlAudBas, m_byPrsChnnlVidBas, m_byPrsChnnlBrBas);
    }

    if ( m_tConf.GetConfAttrb().GetDataMode() != CONF_DATAMODE_VAONLY )
    {
        ConfLog(FALSE, "  DCS:   DcsIdx.%d, DataConfOnGoing.%d, DataConfReCreated.%d\n",
                m_byDcsIdx, m_tConfInStatus.IsDataConfOngoing(), m_tConfInStatus.IsDataConfRecreated());        
    }

    if (m_tLastVmpParam.GetVMPMode() != CONF_VMPMODE_NONE )
    {
        ConfLog(FALSE, "        m_tLastVmpParam:\n");  
        m_tLastVmpParam.Print();
    }   
    
    if (m_tLastVmpTwParam.GetVMPMode() != CONF_VMPMODE_NONE )
    {
        ConfLog(FALSE, "        m_tLastVmpTwParam:\n");  
        m_tLastVmpTwParam.Print();
    }      
}

/*====================================================================
    ������      ��SwitchDiscuss2SpecMix
    ����        ���Զ��ѻ�������(����VAC)�л��ɶ��ƻ���(����ֹͣVAC)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/01/31    4.0         ����          ����
====================================================================*/
void CMcuVcInst::SwitchDiscuss2SpecMix()
{
#ifdef _MINIMCU_
    if ( !m_tConfInStatus.IsSwitchDiscuss2Mix() )
    {
        // Step.1 ��ֹͣ���ܻ���
        if ( !m_tConf.m_tStatus.IsMixing() )
        {
            return;
        }
         
        BOOL32 bWholeDiscuss = m_tConf.m_tStatus.IsAutoMixing();
        if ( bWholeDiscuss && !m_tConf.IsDiscussAutoStop())
        {
            // �����Ҫ�Զ��л�
            m_tConfInStatus.SetSwitchDiscuss2Mix(TRUE);
        } 
        
        // guzh [11/7/2007] ��ʹ��VACҲֻ�ܱ�ֹͣ
        StopMixing();       
    }
    else
    {
        // Step.2 �л������ƻ���,�Զ�ѡ��ǰ n ���ն�
        m_tConfInStatus.SetSwitchDiscuss2Mix(FALSE);
        
        CServMsg cServMsg;

        // �����ն�
        TMt tMt;
        u8 byCount = 0;
        u16 wError = 0;
        u8 byMaxCount = CMcuPfmLmt::GetMaxOprMixNum( m_tConf, wError );
        if (byMaxCount == 0)
        {
            NotifyMcsAlarmInfo(0, wError);
            return;
        }

        for ( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
        {
            if ( !m_tConfAllMtInfo.MtJoinedConf(byMtId) )
                continue;
            tMt = m_ptMtTable->GetMt(byMtId);
            cServMsg.CatMsgBody( (u8*)&tMt, sizeof(TMt) );
            byCount ++ ;

            if ( byCount >= byMaxCount )
                break;
        }
        if ( byCount > 0 )
        {
            AddRemoveSpecMixMember((TMt*)cServMsg.GetMsgBody(), byCount);
        }
    }
#endif    
}

/*====================================================================
    ������      ��GetMaxMixerDepth
    ����        ����ȡ�����������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/01/31    4.0         ����          ����
====================================================================*/
u8 CMcuVcInst::GetMaxMixerDepth() const
{
#ifdef _MINIMCU_
    if ( MEDIA_TYPE_G729 == m_tConf.GetMainAudioMediaType() )
        return MAXNUM_MIXER_DEPTH_8000B_G729;
    else
        return MAXNUM_MIXER_DEPTH_8000B;
#else
    return MAXNUM_MIXER_DEPTH;
#endif
}

/*====================================================================
    ������      ��ChgMtFormatInSdVmp
    ����        ��H264���黭��ϳɣ��ֱ��ʵĵ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId    : Ҫ�������ն�ID
                  u8 byNewFormt: ������ķֱ���
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/05    4.0         �ű���          ����
	09/04/29    4.0         ���㻪          ֧�ֵ����¼�MCU�лش��ն˵ķֱ���
====================================================================*/
void CMcuVcInst::ChgMtFormatInSdVmp(	u8 byMtId,
										TVMPParam *ptVmpParam,
										BOOL32 bStart
										)
{
	if ( NULL == ptVmpParam )
    {
        return;
    }
    
    // guzh [9/1/2006] ���鲻Ҫ������Ӧ�ֱ���
    if ( !m_tConf.IsVmpAutoAdapt() )
    {
        return;
    }

    TMt tMt = m_ptMtTable->GetMt(byMtId);
    if (tMt.IsNull()) 
    {
        return;
    }

    if ((tMt.GetType() != TYPE_MT && tMt.GetType() != TYPE_MCU) ||
		(m_ptMtTable->GetManuId(tMt.GetMtId()) != MT_MANU_KDC && m_ptMtTable->GetManuId(tMt.GetMtId()) != MT_MANU_KDCMCU))
    {
        ConfLog(FALSE, "[ChangeMtVideoFormat] dstmt.%d is not mt(mcu) or kdc mt(mcu). type.%d manu.%d\n",
                tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return;
    }

    //Ŀǰֻ�ı��һ·��Ƶͨ���ķֱ���
    u8 byChnnlType = LOGCHL_VIDEO;   
   
    TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( byMtId ) ||
         !m_ptMtTable->GetMtLogicChnnl( byMtId, byChnnlType, &tLogicChannel, FALSE ) )
    {
        return;
    }

    u8 byMtStandardFormat = tLogicChannel.GetVideoFormat(); // ����ԭ�������ķֱ���

    u8 byLogicChannlType = tLogicChannel.GetChannelType(); // �ŵ����� xliang [080731] �����ж϶���õ�
    u8 byChannlVidFPS = tLogicChannel.GetChanVidFPS();// ͨ������֡�� xliang [080731]

    u8 byNewFormat; //�ֱ�������
    u8 byVmpType = ptVmpParam->GetVMPStyle();   
    
    //���ն˻��ڶ໭�����ǽ���vmp���Ҳ��Ƿ����ˣ��򲻻ָ��ֱ���
    BOOL32 bSpeaker = (GetLocalSpeaker() == tMt);
    
    // 8000B 
    // H264:          sqcif<-->ԭ��(cif)
    // MPEG4, MPEG2:  �Ļ�������qcif<-->ԭ��
    // һ���治�ģ�1��3���淢���˲���
#ifdef _MINIMCU_
    //FIXME: MINIMCU��δ�������VMP
    if (MEDIA_TYPE_H264 == byLogicChannlType)
    {     
        if (VMP_STYLE_ONE == byVmpType)
        {
            byNewFormat = bStart ? VIDEO_FORMAT_CIF:byMtStandardFormat;
        }
        else if (VMP_STYLE_VTWO == byVmpType || VMP_STYLE_HTWO == byVmpType || 
                VMP_STYLE_THREE == byVmpType || VMP_STYLE_FOUR == byVmpType || 
                VMP_STYLE_SPECFOUR == byVmpType)
        {
                //zbq[12/27/2007]ͨ�ûָ���ԭ�ֱ���
                byNewFormat = bStart ? VIDEO_FORMAT_QCIF:byMtStandardFormat;
        }
        else
        {
            u8 byResSqcif = (VMP_STYLE_NINE == byVmpType ? VIDEO_FORMAT_SQCIF_112x96 : VIDEO_FORMAT_SQCIF_96x80);            

            //zbq[12/27/2007]ͨ�ûָ���ԭ�ֱ���
            byNewFormat = bStart ? byResSqcif : byMtStandardFormat;
        }       

        // xsl [11/1/2006] 8000b mcu h264�����Ļ��漰���Ϸ�����ҲҪ�ı�ֱ���
        if (VMP_STYLE_ONE == byVmpType || VMP_STYLE_VTWO == byVmpType || VMP_STYLE_THREE == byVmpType)
        {
            //zbq[04/07/2008] ������ͳһ���� �ֱ���
            //byNewFormat = bSpeaker ? byMtStandardFormat : byNewFormat;
        }        
    }
    else if (MEDIA_TYPE_MP4  == byLogicChannlType ||
             MEDIA_TYPE_H262 == byLogicChannlType )
    {
        if (VMP_STYLE_ONE == byVmpType)
        {
            byNewFormat = byMtStandardFormat;
        }
        else if (VMP_STYLE_VTWO == byVmpType || VMP_STYLE_HTWO == byVmpType || 
                VMP_STYLE_THREE == byVmpType || VMP_STYLE_FOUR == byVmpType || 
                VMP_STYLE_SPECFOUR == byVmpType)
        {
            byNewFormat = bStart ? VIDEO_FORMAT_CIF:byMtStandardFormat;
        }
        else
        {
            byNewFormat = bStart ? VIDEO_FORMAT_QCIF:byMtStandardFormat;
        }

        //zbq[04/07/2008] ������ͳһ���� �ֱ���
        //�����˱���ԭ���ķֱ���
        //byNewFormat = bSpeaker ? byMtStandardFormat : byNewFormat;
    }
    else
    {
        return;
    }
#else
    // 8000 
    // �Ļ���: CIF�����²�����H264��HD/4CIF����CIF
    // H264:         qcif<-->ԭ��(cif)
    // MPEG4, MPEG2: cif<-->ԭ��

    if (MEDIA_TYPE_H264 == byLogicChannlType)
    {        
        if (VMP_STYLE_ONE == byVmpType || VMP_STYLE_VTWO == byVmpType ||
            VMP_STYLE_HTWO == byVmpType || VMP_STYLE_THREE == byVmpType ||
            VMP_STYLE_FOUR == byVmpType || VMP_STYLE_SPECFOUR == byVmpType ||
			//fxh [02/05/2009] ���VCS���⶯̬VMP����ʱ���� 
			(VCS_CONF == m_tConf.GetConfSource() &&
			 VMP_STYLE_DYNAMIC == byVmpType && IsHDConf(m_tConf)) )
        {

            //zbq[12/27/2007] ����CIF
            if (VIDEO_FORMAT_CIF != m_tConf.GetMainVideoFormat())
            {
                byNewFormat = bStart ? VIDEO_FORMAT_CIF : byMtStandardFormat;
            }
            else
            {
                byNewFormat = byMtStandardFormat;
            }                
        }
        else
        {
            //zbq[12/27/2007]ͨ�ûָ���ԭ�ֱ���
            byNewFormat = bStart ? VIDEO_FORMAT_QCIF:byMtStandardFormat;
        }        
    }
    else if (MEDIA_TYPE_MP4  == byLogicChannlType ||
             MEDIA_TYPE_H262 == byLogicChannlType )
    {       
        if (VMP_STYLE_ONE == byVmpType || VMP_STYLE_VTWO == byVmpType ||
            VMP_STYLE_HTWO == byVmpType || VMP_STYLE_THREE == byVmpType ||
            VMP_STYLE_FOUR == byVmpType || VMP_STYLE_SPECFOUR == byVmpType)
        {
            byNewFormat = byMtStandardFormat;
        }
        else
        {
            byNewFormat = bStart ? VIDEO_FORMAT_CIF:byMtStandardFormat;
        }        
    }
    else
    {
        return;   
    }  

    //�����˱���ԭ���ķֱ���
    //byNewFormat = bSpeaker ? byMtStandardFormat : byNewFormat;
#endif
    
	if ( !bStart )    
	{
		if ( ((m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE && m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tMt)) ||
			 (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE && m_tConf.m_tStatus.GetVmpTwParam().IsMtInMember(tMt))) )
		{
			EqpLog("[ChangeMtVideoFormat] mt.%d is still in VMP or MPW, keep its format !\n", tMt.GetMtId());
			return;
		}
	}   

    // ���ڸ�������Ƶ����, zgc, 2008-08-21
    if ( m_tVidBrdSrc == tMt )
    {
        //zbq[12/11/2008] ��ƵԴ�ֱ����л������䲻������
        //AdjustHDVidAdapt();
    }
    
	// fxh ����¼�MCU�����ֱ���,ʵ��Ϊ����ش�ͨ���е��ն˵���
	if (MT_TYPE_SMCU == tMt.GetMtType())
	{
		TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMtId());
		if (ptMcInfo != NULL)
		{
			EqpLog("[ChangeMtVideoFormat]change mt(mcuid:%d, mtid:%d) in sub mcu%d\n", 
				   ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId(), tMt.GetMtId());
		}

		if (ptMcInfo != NULL &&
			!ptMcInfo->m_tMMcuViewMt.IsNull())
		{
			CascadeAdjMtRes(ptMcInfo->m_tMMcuViewMt, byNewFormat, bStart);			
		}
	}
	else
	{
		CServMsg cServMsg;
		//xliang [080731] modify msgbody
		cServMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
		cServMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
		//cServMsg.CatMsgBody( &byLogicChannlType,sizeof(u8) );
		cServMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
		//cServMsg.CatMsgBody( &byChannlVidFPS, sizeof(u8) );
		SendMsgToMt( byMtId, cServMsg.GetEventId(), cServMsg );
	}

    EqpLog( "[ChangeMtVideoFormat] chnnl type: %d, send videoformat<%d> change msg to mt<mcuid:%d, mtid:%d, mttype:%d>!\n",
            tLogicChannel.GetChannelType(), byNewFormat, tMt.GetMcuId(), tMt.GetMtId(), tMt.GetMtType() );

    return;
}

/*====================================================================
    ������      ChgMtFormatInMpu
    ����        ���ֱ��ʵĵ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId				:[in] Ҫ�������ն�ID
                  TVMPParam &tVmpParam	:[in] VMP Param
				  BOOL32 bStart			:[in] �Ƿ�ʼ����������ָ��ֱ��ʣ�

    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/10/2008				Ѧ��	        ����          
====================================================================*/
BOOL32 CMcuVcInst::ChgMtFormatInMpu( u8 byMtId,
									 TVMPParam *ptVmpParam,
									 BOOL32 bStart,
									 BOOL32 bSeize,
                                     BOOL32 bConsiderVmpBrd
									 )
{
	
	if ( NULL == ptVmpParam )
    {
        return FALSE;
    }

	TMt tMt = m_ptMtTable->GetMt(byMtId);
    if (tMt.IsNull()) 
    {
        return FALSE;
    }

    if ( !m_tConf.IsVmpAutoAdapt() )	//��ǰ���鲻�ǻ���ϳ�����Ӧ
    {
        return TRUE;					//�޹��˲���������֮���ս�ͨ�������Է���TURE
    }

	if (tMt.GetType() != TYPE_MT)
	{
		ConfLog(FALSE, "[ChgMtFormatInMpu] dstmt.%d is not mt. type.%d manu.%d\n",
			tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return FALSE;
	}
	
	TMtStatus tMtStatus;
	m_ptMtTable->GetMtStatus( tMt.GetMtId(), &tMtStatus );
	if( tMtStatus.IsInTvWall() )
	{
		return TRUE;					//Mt����DEC5�У��򲻵��ֱ��ʣ�����TRUE��֮���ս�ͨ��
	}

	//Ŀǰֻ�ı��һ·��Ƶͨ���ķֱ���
    u8 byChnnlType = LOGCHL_VIDEO;   

    TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( byMtId ) ||
		!m_ptMtTable->GetMtLogicChnnl( byMtId, byChnnlType, &tLogicChannel, FALSE ) )
    {
        return FALSE;
    }

	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
	u8 byMpuBoardVer = tPeriEqpStatus.m_tStatus.tVmp.m_byBoardVer;
	u8 byPos = ptVmpParam->GetChlOfMtInMember( tMt );
	u8 byVmpStyle = ptVmpParam->GetVMPStyle();
    u8 byMtStandardFormat = tLogicChannel.GetVideoFormat(); // ����ԭ�������ķֱ���
    u8 byLogicChannlType = tLogicChannel.GetChannelType();	// �ŵ����� 
    u8 byChannlVidFPS = tLogicChannel.GetChanVidFPS();		// ͨ������֡�� 
    u8 byNewFormat = 0;										// ������ķֱ���
	BOOL32 bNeedAdjustRes = FALSE;							// �Ƿ������Ҫ�����ֱ���
    
	if(!bStart) //�ָ��ֱ���
	{
		if ( ((m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE && m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tMt)) ||
			(m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE && m_tConf.m_tStatus.GetVmpTwParam().IsMtInMember(tMt))) )
		{
			EqpLog("[ChgMtFormatInMpu] mt.%d is still in VMP or MPW, keep its format !\n", tMt.GetMtId());
			return FALSE; //FIXME:return TRUE����ô��
		}
		else
		{
			byNewFormat = byMtStandardFormat;
			EqpLog("[ChgMtFormatInMpu] mt.%u recover it's format!\n",tMt.GetMtId());
			//Send Extra NoneStandard Message to Mt //ĿǰMT����֧��
// 			CServMsg cServMsg;
// 			SendMsgToMt(byMtId, MCU_MT_VIDEOPARAMRECOVER_CMD, cServMsg);
		}
	}
	else //����MT�ֱ���
	{
		if( byVmpStyle == VMP_STYLE_ONE)	//1���治���ֱ���
		{
			// xliang [7/14/2009] ��Ϊ�ն˵�ǰʵ�ʱ�ķֱ���MCU����֪�������Ի��ǵõ�һ��
			if( MEDIA_TYPE_H264 != byLogicChannlType ) 
			{
				byNewFormat = VIDEO_FORMAT_CIF;	//mp4 auto ��ֱ�ӵ���CIF
				EqpLog("[ChgMtFormatInMpu] 1 vmp adjust resolution CIF for none h264!\n");
			}
			else
			{
				byNewFormat = byMtStandardFormat;
				EqpLog("[ChgMtFormatInMpu] 1 vmp need not adjust resolution!\n");
			}
		}	
		else
		{
			u8 byReqRes	= 0;										//ĳ�����ĳλ������������ֱ���	
			u16 wBR		= 0;										//���ʣ�����mcu������������
			s32 nResCmpRst = 3;										//�ֱ��ʱȽϽ������ʼ��3,�����Ƚ�������
			
			//��������ж�
			BOOL32 bSpeaker = (GetLocalSpeaker() == tMt);									//�Ƿ��Ƿ�����
			BOOL32 bNoneKeda = ( MT_MANU_KDC != m_ptMtTable->GetManuId(tMt.GetMtId())  
				&& MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tMt.GetMtId()) );	//�Ƿ��keda�ն�
			BOOL32 bSelected = IsSelectedbyOtherMt(tMt.GetMtId());							//�Ƿ�ѡ��
			
			BOOL32 bSelectedByRoll = FALSE;		// xliang [5/7/2009] ������ɵ�ѡ������Ϊʱ�����⣬�������⴦����
			if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
				&& ( m_tRollCaller == tMt 
				|| m_tRollCallee == tMt ) )
			{
				bSelectedByRoll = TRUE;
			}
			
			//����VMPǰ����ͨ����������h264CIFͨ������
			u8 byMaxHdChnnlNum	= 0;
			u8 byMaxCifChnnlNum  = 0;
			
			switch(byVmpSubType)
			{
			case MPU_DVMP:
			case EVPU_DVMP:
				{
					byMaxHdChnnlNum	= MAXNUM_DVMP_HDCHNNL;
					break;
				}
			case MPU_SVMP:
			case EVPU_SVMP:
				{
					if(byMpuBoardVer == MPU_BOARD_B256)
					{
						byMaxHdChnnlNum	= MAXNUM_SVMPB_HDCHNNL;
						byMaxCifChnnlNum = MAXNUM_SVMP_NON264CIFCHNNL;
					}
					else
					{
						byMaxHdChnnlNum	= MAXNUM_SVMP_HDCHNNL;
						byMaxCifChnnlNum = MAXNUM_SVMP_NON264CIFCHNNL;
					}
					
					break;
				}
			default:
				break;
			}
			/*
			if(m_tVmpChnnlInfo.m_byAllChnnlNum > byMaxAllChnnlNum)
			{
			EqpLog("[ChangeMtVideoFormat]all channel has been occupied!\n");
			return FALSE;
		}*/
			
			
			if(byLogicChannlType != MEDIA_TYPE_H264) 
			{
				u8 byNon264CifChnnlNum = m_tVmpChnnlInfo.GetCifChnnlNum();
				if (byNon264CifChnnlNum >= byMaxCifChnnlNum)
				{
				EqpLog("[ChgMtFormatInMpu] VMP channels for none h264 Cif reach the maximum(%u),\
					so Mt.%u won't enter VMP\n", byMaxCifChnnlNum, byMtId );
				return FALSE;
				}
				
				if (byMtStandardFormat == VIDEO_FORMAT_4CIF && bNoneKeda)
				{
					EqpLog("[ChgMtFormatInMpu] NoneKeda Mt.%u which is MPEG4 with 4CIF in format is not support by VMP!\n",
						byMtId );
					return FALSE;
				}
				
				byNewFormat = VIDEO_FORMAT_CIF;
				m_tVmpChnnlInfo.SetCifChnnlNum(++byNon264CifChnnlNum);
				EqpLog("[ChgMtFormatInMpu] Force to change Mt.%u 's format to CIF as media type other than H264!\n", byMtId);
			}
			else // H264��ʽ�Ĵ���
			{
				//�ж��Ƿ�Ҫ�����ֱ����Լ�����Ҫ�������ķֱ��ʴ�С
				bNeedAdjustRes = VidResAdjust( byVmpStyle, byPos, byMtStandardFormat, byReqRes);
				BOOL32 bNeedAdjust = bNeedAdjustRes;	//�����ǽ�ǰ����ͨ�����ֱ����Ƿ���Ҫ����
				
				// xliang [3/24/2009] ռǰ����ͨ�� ���پ�����HD����������EVPU��4CIFҲ�ɽ�ǰ����ͨ��
				
				//��vmp������ѯ�У��ǿƴ��MT������Ҫ�����ֱ��ʣ������ 
				// 			if(m_tConf.m_tStatus.m_tVMPParam.IsVMPBatchPoll() && bNoneKeda && bNeedAdjust) 
				// 			{
				// 				EqpLog("[ChangeMtVideoFormat] Batch polling: The Mt is none keda's!\n");
				// 				return FALSE;
				// 			}
				
				
				// xliang [4/7/2009] Res �Ƚ�����û��Ҫ���ֱ��ʵ�MT����ʹ��������ݵ�Ҳ��ռǰ����ͨ��
				//���з����ܻ����Ƶ����ռ���з���
				if( bNeedAdjust 
					&&
					(  ( bSpeaker && !m_tConf.m_tStatus.IsBrdstVMP() )
					|| ( bSelected && (!m_tConf.m_tStatus.IsBrdstVMP() || !bConsiderVmpBrd) )
					|| bNoneKeda 
					|| bSelectedByRoll )
					)
				{
					TChnnlMemberInfo tChnnlMemInfo;
					
					u8 byHdChnnlNum = m_tVmpChnnlInfo.m_byHDChnnlNum;
					//tChnnlMemInfo.m_byMtId = byMtId;
					tChnnlMemInfo.SetMt(tMt);
					if(bSpeaker && !m_tConf.m_tStatus.IsBrdstVMP())
					{
						tChnnlMemInfo.SetAttrSpeaker();
					}
					if(bNoneKeda)
					{
						tChnnlMemInfo.SetAttrNoneKeda();
					}
					if( ( bSelected 
						&& ( (!m_tConf.m_tStatus.IsBrdstVMP() && !bSpeaker)	//��VMP�㲥�·����˻���߱�ѡ��
						|| !bConsiderVmpBrd ) )
						|| bSelectedByRoll									//������ɵ�ѡ����ͬVMP�㲥����޹�
						)
					{
						tChnnlMemInfo.SetAttrSelected();
					}
					
					if ( byHdChnnlNum < byMaxHdChnnlNum					//��ʣ��HDǰ����ͨ��������ռ֮
						|| ( m_tVmpChnnlInfo.GetChnnlIdxCtnMt(tMt) != -1 )	//��Ա�Ѿ���ǰ����ͨ���У����ܻ���ˢ��Ա����
						)
					{
						//��������channel������ͬ��MT
						s32 nRet = m_tVmpChnnlInfo.AddInfoToHdChnnl(&tChnnlMemInfo);
						if(nRet == -1) //��HD MT�Ѿ���������HDͨ�����������ٽ���֮ǰ�ڶ�vmpParam���й��ˣ������ٱ���һ��
						{
							//��Ȼ�Ѿ���ͨ�����ˣ�����Ա���Կ��ܷ����仯(�磺�ӱ�ѡ���ı�Ϊ������)����������ˢ��һ��ͨ����Ա����
							//��AddInfoToHdChnnl�ӿ������Ӹô���
							EqpLog("[ChgMtFormatInMpu]The same Hd Mt.%u is not allowed to enter multiple vmp channel!\n",byMtId);
							return FALSE;
						}
						//��vmp����ǰ����ͨ�����ֱ��ʲ��õ���
						bNeedAdjustRes = FALSE;
						EqpLog("[ChgMtFormatInMpu]Mt.%u -> HD adapter channel, no need to change it's video format.\n",
							byMtId);
					}
					else
					{
						if(m_tConf.m_tStatus.m_tVMPParam.IsVMPBatchPoll())
						{
							//vmp��ѯ��Զ��������ռ
							EqpLog("[ChgMtFormatInMpu]no seize operation in VMP Poll mode!\n");
							if (bNoneKeda)
							{
							EqpLog("[ChgMtFormatInMpu]The Mt.%u is none keda and VMP VIP channel is full! \
								Since VMP mode is [Batch Poll] mode, the Mt will not enter any channel.\n",
								byMtId );
							return FALSE;	
							}
						}
						else if(m_tConf.m_tStatus.m_tVMPParam.IsVMPAuto())
						{
							//�Զ�����ϳɲ���ռ
							EqpLog("[ChgMtFormatInMpu]no seize operation in VMP Auto mode!\n");
							if (bNoneKeda)
							{
							EqpLog("[ChgMtFormatInMpu]The Mt.%u is none keda and VMP VIP channel is full! \
								Since VMP mode is [AUTO VMP] mode, the Mt will not enter any channel.\n",
								byMtId );
							return FALSE;	
							}
						}
						else
						{	
							//��ռ��ʾ
							if (bSeize)
							{
								CServMsg cServMsg;
								//��������ռ��HD��ͨ��
								cServMsg.SetMsgBody((u8*)&tMt,sizeof(tMt));//��Ҫ��ռ��������ͨ�����ն�ID
								for(u8 byChnnlIdx = 0; byChnnlIdx<byHdChnnlNum; byChnnlIdx++)
								{
									m_tVmpChnnlInfo.GetHdChnnlInfo(byChnnlIdx,&tChnnlMemInfo);
									cServMsg.CatMsgBody((u8 *)&tChnnlMemInfo,sizeof(tChnnlMemInfo));
								}
								SendMsgToAllMcs(MCU_MCS_VMPPRISEIZE_REQ, cServMsg);
								
								return FALSE;//require Hd channel but fail to occupy it now, so enter no channel at all.
							}
							else
							{
								//��ռhdͨ����δ����ռʱ��(��HDͨ���ͷ�δ������)��
								//������ض�ʱ���ٴδ�����ռ����Ҫ�ٴγ�����ռ��ʾ��
								//����ʱ�����ﵽһ������ʱ�����ٴγ�����ռ��ʾ(Ӧ������2����ռ��
								//��������ͬһ��MT�����һ��ʼ��û�����ɹ������)
								
								// xliang [4/21/2009] �ò��������������VMPʱ����һ�α��������Գ���������ռ�ĸ����൱��
								// ( ��������һ����Ա��changevmpparam�������ԣ���������������ռ������
								// �������з������к�С���ʷ���������ռ����ȥ�����ڽ�ǰ������Ż�������ʹ���ʽ��͵�0 )
								
								// xliang [7/14/2009] Ŀǰ��else�Ĵ��ڵ����壨bSeize���ڵ����壩
								// ��������������������ʱ����ϣ����ռ����������ֱ�����������ֱ��ʡ�
								// һ������������DEC5�е�MT�˳�DEC5ʱҪ�ָ�����������ֱ���
								EqpLog("[ChgMtFormatInMpu]Make Mt.%u seize no adapter channel on purpose!\n",byMtId);
							}
							//���ﲻ�賬ʱ
							
							//return FALSE;//require Hd channel but fail to occupy it now, so enter no channel at all.
						}
					}
				}
				else //��������ݵĸ���MT������ݺϳɷ��ͨ��λ�õ����ֱ���
				{
					//ԭ����������ݣ�����ռ��������ͷ�ԭ��ռ�õ�ǰ������ͨ��
					BOOL32 nRet = m_tVmpChnnlInfo.ClearOneChnnl(m_ptMtTable->GetMt(byMtId));
					if(nRet)
					{
						EqpLog("[ChgMtFormatInMpu]Mt.%u's format change: get out of the Hd Channel.\n",byMtId);
					}
					else
					{
						// Do nothing
					}
				}
				
				if(bNeedAdjustRes)
				{
					byNewFormat = byReqRes;
				}
				else
				{
					byNewFormat = byMtStandardFormat;
				}
				
				//GetResStr()����ֱ���������ã��ڲ�ά����һ��static varible.
				EqpLog("--------��Mt.%u (Channel.%u): Resolution compare��------\n",byMtId, byPos);
				EqpLog("Original Res: \t%s\n",GetResStr(byMtStandardFormat));
				EqpLog("Require Res: \t%s\n",GetResStr(byReqRes));
				EqpLog("======================================================\n");
				if( bNeedAdjust && !bNeedAdjustRes )
				{
					EqpLog("Need Adjust: \t0 (Enter HD adapter channel)\n");
				}
				else if(!bNeedAdjust)
				{
					EqpLog("Need Adjust: \t0 \n");
				}
				else
				{
					EqpLog( "Need Adjust: \t1 \n");
				}	
			}
		}	
    }
	
	// xliang [4/11/2009] �ֱ��ʶ�Ҫ��һ�£����������;�з��������������
	//��Ϊ�Ƚ����������ResΪ��׼��
	if (MT_TYPE_SMCU == tMt.GetMtType())
	{
		TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMtId());
		if (ptMcInfo != NULL)
		{
			EqpLog("[ChgMtFormatInMpu]change mt(mcuid:%d, mtid:%d) in sub mcu%d\n", 
				ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId(), tMt.GetMtId());
		}
		
		if (ptMcInfo != NULL &&
			!ptMcInfo->m_tMMcuViewMt.IsNull())
		{
			// xliang [4/29/2009] FIXMEĿǰֻ��һ���ش�ͨ��,������(2,2)��VMP�����ϼ�ѡ���¼�(2,1) ��������. 
			// �����¼ӵĻش�Դ������ʾtSrcMt. (FIXME:��VCS��,��������ͳһ)
			// �ش�Դ�仯����Ҫ��(2,1)�ֱ��ʣ����ûָ�(2,2)�ֱ��ʡ�
			// FIXME: �ش�Դ���������ɽ��лָ�����
			TMt tSrcMt = ptMcInfo->m_tMMcuViewMt;
			TMt tLastSrcMt = ptMcInfo->m_tLastMMcuViewMt;
			if (!tLastSrcMt.IsNull())
			{
				//�ָ��ϴλش�ԴMt�ķֱ���
				CascadeAdjMtRes( tLastSrcMt, byNewFormat, FALSE);
			}
			
			CascadeAdjMtRes( tSrcMt, byNewFormat, bStart, byVmpStyle, byPos);
			
			return FALSE;	//����FALSE��ζ�������������¼�MCU��Res����Ok���ٲ�������		
		}
	}
	else
	{
		CServMsg cServMsg;
		cServMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
		cServMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
		cServMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
		SendMsgToMt( byMtId, cServMsg.GetEventId(), cServMsg );
		
		EqpLog( "[ChgMtFormatInMpu] chnnl type: %d, send videoformat<%d>(%s) change msg to mt<%d>!\n",
			tLogicChannel.GetChannelType(), byNewFormat, GetResStr(byNewFormat), byMtId );
	}
	
    return TRUE;
}

/*==============================================================================
������    :  RecoverMtResInMpu
����      :  �ָ��ն˵ķֱ��ʣ�ͬʱ���Ǹ�Mt�ڻ���ϳ����е����
�㷨ʵ��  :  
����˵��  :  TMt	tMt	[in]: Ҫ�ָ��ֱ��ʵ��ն�
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-7-13	 4.6           Ѧ��                             create
==============================================================================*/
BOOL32 CMcuVcInst::RecoverMtResInMpu(TMt tMt)
{
	if( tMt.IsNull())
	{
		return FALSE;
	}

	if (tMt.GetType() != TYPE_MT)
	{
		ConfLog(FALSE, "[RecoverMtResInMpu] dstmt.%d is not mt. type.%d manu.%d\n",
			tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return FALSE;
	}

	u8 byMtId = tMt.GetMtId();
	u8 byChnnlType = LOGCHL_VIDEO;   
	
    TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( byMtId )
		|| !m_ptMtTable->GetMtLogicChnnl( byMtId, byChnnlType, &tLogicChannel, FALSE )
		|| MEDIA_TYPE_H264 != tLogicChannel.GetChannelType()	//Ŀǰ�ĵ����ֱ��ʾ������h264
		)
    {
        return FALSE;
    }

	u8 byLogicChannlType = tLogicChannel.GetChannelType();	// �ŵ�����
    u8 byMtStandardFormat = tLogicChannel.GetVideoFormat(); // ԭʼ�ֱ���
    u8 byNewFormat = 0;										// Ҫ�ָ����ķֱ���
	
	//ԭʼ�ֱ��ʾ���CIF�����ûָ���
	if( VIDEO_FORMAT_CIF == byMtStandardFormat )
	{
		ConfLog(FALSE, "[RecoverMtResInMpu] The original resolution is Cif, so no need to recover resolution!\n");
		return FALSE;
	}
	
	BOOL32 bRecoverToOrg = FALSE;
	TVMPParam tVmpParam;
	//���ն��Ƿ�����VMP��
	if(m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE && m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tMt))
	{
		tVmpParam = m_tConf.m_tStatus.GetVmpParam();
		u8 byVmpStyle = tVmpParam.GetVMPStyle();
		if( byVmpStyle == VMP_STYLE_ONE)	//1���治���ֱ���
		{
			EqpLog("[RecoverMtResInMpu] 1 vmp need not adjust resolution further, so recover to its original resolution!\n");
			bRecoverToOrg = TRUE;
		}
	}
	else
	{
		bRecoverToOrg = TRUE;
	}

	if( bRecoverToOrg )
	{
		byNewFormat = byMtStandardFormat;
		CServMsg cServMsg;
		cServMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
		cServMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
		cServMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
		SendMsgToMt( byMtId, cServMsg.GetEventId(), cServMsg );
		
		EqpLog( "[ChgMtFormatInMpu] chnnl type: %d, send videoformat<%d>(%s) change msg to mt<%d>!\n",
			tLogicChannel.GetChannelType(), byNewFormat, GetResStr(byNewFormat), byMtId );
	}
	else
	{
		ChangeMtVideoFormat( tMt, &tVmpParam, TRUE, FALSE );
	}

	return TRUE;
}

/*==============================================================================
������    :  AddToVmpNeglected
����      :  ��ĳ�ն�ID���뵽VMP��Ա�����б�
�㷨ʵ��  :  
����˵��  :  u8 byMtId	[in]
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2008-12-17					Ѧ��							����
==============================================================================*/
void CMcuVcInst::AddToVmpNeglected( u8 byMtId)
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)return;
	m_abyMtNeglectedByVmp[(byMtId-1) / 8] |= 1 << ( (byMtId-1) % 8 );
}

/*==============================================================================
������    :  IsMtNeglectedbyVmp
����      :  �ж�ĳ���ն��Ƿ���VMP��Ա�����б���
�㷨ʵ��  :  
����˵��  :  u8 byMtId	[in]
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2008-12-17					Ѧ��							create
==============================================================================*/
BOOL32 CMcuVcInst::IsMtNeglectedbyVmp( u8 byMtId)
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return FALSE;
	}
	return ( ( m_abyMtNeglectedByVmp[(byMtId-1) / 8] & ( 1 << ( (byMtId-1) % 8 ) ) ) != 0 );
}

/*====================================================================
    ������      AdjustVmpParam
    ����        ������ϳɲ����ı�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TVMPParam *ptVMPParam	[in]����ϳɲ���
				  BOOL32 bStart			[in]�Ƿ��ǿ�ʼ����ϳ�, default: FALSE
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
05/18/2009		4.6			Ѧ��			create				
====================================================================*/
void CMcuVcInst::AdjustVmpParam(TVMPParam *ptVMPParam, BOOL32 bStart)
{
	if (m_tVmpEqp.GetEqpId() < VMPID_MIN || m_tVmpEqp.GetEqpId() > VMPID_MAX)
	{
		CfgLog(FALSE, "[AdjustVmpParam] it hasn't assign correct vmp(id:%d) yet\n", m_tVmpEqp.GetEqpId());
		return;
	}

	TPeriEqpStatus tPeriEqpStatus;	
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
	if(byVmpSubType == VMP)
	{
		ChangeVmpParam(ptVMPParam, bStart);
	}
	else
	{
		ChangeNewVmpParam(ptVMPParam, bStart);
	}
}

/*====================================================================
    ������      ��ChangeNewVmpParam
    ����        ������ϳɲ����ı�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TVMPParam *ptVMPParam, ����ϳɲ���
				  BOOL32 bStart �Ƿ��ǿ�ʼ����ϳ�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
12/31/2008						
====================================================================*/
void CMcuVcInst::ChangeNewVmpParam(TVMPParam *ptVMPParam, BOOL32 bStart)
{
	if (ptVMPParam == NULL)
	{
		return;
	}
   
	//tConfVMPParamֻ��¼SchemeId, RimEnable��ֵ, ���ڳ�Ա״̬û�вο���ֵ���ɿ��Ƿϳ�
	TVMPParam tConfVMPParam = m_tConf.m_tStatus.GetVmpParam();	

	CServMsg cServMsg;
	cServMsg.SetConfId(m_tConf.GetConfId());

    //����CKDVNewVMPParam[x]����
    CKDVNewVMPParam cKDVVMPParam;
    memset(&cKDVVMPParam, 0, sizeof(cKDVVMPParam));
    cKDVVMPParam.m_byVMPStyle = ptVMPParam->GetVMPStyle();
    cKDVVMPParam.m_byEncType = m_tConf.GetMainVideoMediaType();
    
	// xliang [2/25/2009] ��vmp�ò�����GetMaxMemberNum���µ���ʵ�ʲμ�VMP�ĳ�Ա��
	//cKDVVMPParam.m_byMemberNum  = ptVMPParam->GetMaxMemberNum(); 
	
	TMt tSpeakerMt = GetLocalSpeaker();
	//ֻ�����ն˽���ͨ��������������豸
	if (tSpeakerMt.GetType() == TYPE_MCUPERI)
	{
		memset(&tSpeakerMt, 0, sizeof(TMt));
	}
	TMt tChairMt = m_tConf.GetChairman();
	
	u8 byMemberNum = ptVMPParam->GetMaxMemberNum();	//��ǰ������ܹ��μӵ�����Ա��
	u8 byMtId = 1;
	u8 byMember = 0;								//ʵ�ʲμ�VMP�ĳ�Ա��
	u8 byLoop = 0;

	// ����VCS���黭��ϳɳ�Աѡ��
	if (VCS_CONF == m_tConf.GetConfSource())
	{
		TMt atVmpMember[MAXNUM_VMP_MEMBER];
		memset(atVmpMember, 0, sizeof(TMt) * MAXNUM_VMP_MEMBER);
		if (ptVMPParam->IsVMPAuto())
		{
			u8 byStyle = GetVCSAutoVmpMember(atVmpMember);
			if (byStyle != VMP_STYLE_NONE)
			{
				ptVMPParam->SetVMPStyle(byStyle);
				cKDVVMPParam.m_byVMPStyle = byStyle;
			}
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[ChangeVmpParam]only auto vmp to vcs currently");
			return;
		}
		
		for (; byLoop < ptVMPParam->GetMaxMemberNum(); byLoop++)
		{
			if (!atVmpMember[byLoop].IsNull() && m_tConfAllMtInfo.MtJoinedConf(atVmpMember[byLoop].GetMtId()))
			{
				cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(atVmpMember[byLoop].GetMtId()) );
				cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(atVmpMember[byLoop]);
				cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_AUDIENCE);
				ptVMPParam->SetVmpMember(byMember, cKDVVMPParam.m_atMtMember[byMember]);
				byMember ++;
			}
		}
		cKDVVMPParam.m_byMemberNum  = byMember;		
	}
	else
	{
		if(ptVMPParam->IsVMPAuto())
		{        
			//������ˣ�������ϯ���������
			if(!tSpeakerMt.IsNull() && m_tConfAllMtInfo.MtJoinedConf(tSpeakerMt.GetMtId()))
			{
				// �ն˱���
				cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(tSpeakerMt.GetMtId()) );
				cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(tSpeakerMt);
				cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_SPEAKER);
				cKDVVMPParam.m_atMtMember[byMember].SetMemberType(VMP_MEMBERTYPE_SPEAKER);
				byMember ++;
			}
			if(!tChairMt.IsNull() &&
				!(tChairMt == tSpeakerMt) &&
				m_tConfAllMtInfo.MtJoinedConf(tChairMt.GetMtId()))
			{
				// �ն˱���
				cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(tChairMt.GetMtId()) );
				cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(tChairMt);
				cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_CHAIRMAN);
				cKDVVMPParam.m_atMtMember[byMember].SetMemberType(VMP_MEMBERTYPE_CHAIRMAN);
				byMember ++;
			}
			for (byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
			{
				if (byMember >= byMemberNum)
				{
					// ������
					break;
				}
				if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
				{
					if (byMtId == m_tConf.GetChairman().GetMtId())
					{
						// ��ϯ��ǰ��
						continue;
					}
					if (byMtId == GetLocalSpeaker().GetMtId() &&
						GetLocalSpeaker().GetType() != TYPE_MCUPERI)
					{
						// ��������ǰ��
						continue;
					}
					
					// �ն˱���
					cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(byMtId) );
					cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(m_ptMtTable->GetMt(byMtId));
					cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_AUDIENCE);
					cKDVVMPParam.m_atMtMember[byMember].SetMemberType(VMP_MEMBERTYPE_VAC);
					byMember ++;
				}
			}
			cKDVVMPParam.m_byMemberNum = byMember; //�Զ�VMPʱ��byMemberNumӦ�����ն���һ��
			
			for(u8 byLp = 0; byLp < cKDVVMPParam.m_byMemberNum; byLp ++)
			{
				ptVMPParam->SetVmpMember(byLp, cKDVVMPParam.m_atMtMember[byLp]);
			}
		}
		else
		{
			for (byLoop = 0; byLoop < ptVMPParam->GetMaxMemberNum(); byLoop++)
			{
				cKDVVMPParam.m_atMtMember[byLoop].SetMember(*ptVMPParam->GetVmpMember(byLoop)); 
				
				TMt tTempMt = *(TMt *)ptVMPParam->GetVmpMember(byLoop);
				if( tTempMt.GetMtId() != 0 )
				{
					byMember ++;
				}
				if (tChairMt == tTempMt && ptVMPParam->GetVmpMember(byLoop)->GetMemberType() != VMP_MEMBERTYPE_SPEAKER )
				{
					cKDVVMPParam.m_atMtMember[byLoop].SetMemStatus(MT_STATUS_CHAIRMAN);
				}
				else if (tSpeakerMt == tTempMt)
				{
					cKDVVMPParam.m_atMtMember[byLoop].SetMemStatus(MT_STATUS_SPEAKER);
				}
				else
				{
					cKDVVMPParam.m_atMtMember[byLoop].SetMemStatus(MT_STATUS_AUDIENCE);
				}
				// �ն˱���
				cKDVVMPParam.m_atMtMember[byLoop].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(tTempMt.GetMtId()) );
			}
			
			//���������ն�  // xliang [12/24/2008] FIXME?д���������
			if( !m_tConfAllMtInfo.MtJoinedConf( cKDVVMPParam.m_atMtMember[byLoop].GetMtId()))
			{
				cKDVVMPParam.m_atMtMember[byLoop].SetNull();
			}
			cKDVVMPParam.m_byMemberNum = byMember;
		} 
	}

	//�������ͨ���Ľ���
	u8 byMaxVMPMember = MAXNUM_MPUSVMP_MEMBER;
	if (ptVMPParam->IsVMPAuto() || VCS_CONF == m_tConf.GetConfSource())
	{
		byLoop = byMember;	
	}
	else
	{
		byLoop = ptVMPParam->GetMaxMemberNum();
	}

	for (; byLoop < byMaxVMPMember; byLoop++)
	{
		StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
		ptVMPParam->ClearVmpMember(byLoop);
		//tConfVMPParam.ClearVmpMember(byLoop);// meaning?
	}

	//���ϳɳ�Ա���µ�TPeriEqpStatus��
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = *ptVMPParam;
	g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	

	//���غ����͵Ȳ���
	TLogicalChannel tChnnl;
	BOOL32 bRet = FALSE;
	for (byLoop = 0; byLoop < ptVMPParam->GetMaxMemberNum(); byLoop++)
	{
		bRet = m_ptMtTable->GetMtLogicChnnl(cKDVVMPParam.m_atMtMember[byLoop].GetMtId(), LOGCHL_VIDEO, &tChnnl, FALSE);
		if (bRet)
		{
			cKDVVMPParam.m_tVideoEncrypt[byLoop] = tChnnl.GetMediaEncrypt();
			cKDVVMPParam.m_tDoublePayload[byLoop].SetRealPayLoad(tChnnl.GetChannelType());
			cKDVVMPParam.m_tDoublePayload[byLoop].SetActivePayload(tChnnl.GetActivePayload());
		}
	}
	

	//���ʽ�����ʣ��ֱ�����Ϣ 
	u16 wMinMtReqBitrate = 0;
	if (0 != m_tConf.GetSecBitRate() && 
		MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
	{
		wMinMtReqBitrate = m_tConf.GetBitRate();
	}
	else
	{
		wMinMtReqBitrate = GetLeastMtReqBitrate(TRUE, cKDVVMPParam.m_byEncType);
//		m_wVMPBrdBitrate = wMinMtReqBitrate;
	}
	
	if (wMinMtReqBitrate < m_wVidBasBitrate || 0 == m_wVidBasBitrate)
	{
		m_wVidBasBitrate = wMinMtReqBitrate;
	}
	//cKDVVMPParam.SetBitRate(wMinMtReqBitrate);

	// xliang [5/8/2009] ����RES
	u16 wWidth = 0;
	u16 wHeight = 0;
	u8 byChnnlRes  = 0;
	u8 byMediaType = 0;
	u8 byVmpOutChnnl = 0;
	TPeriEqpStatus tVmpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpStatus);
    u8 byVmpSubType = tVmpStatus.m_tStatus.tVmp.m_bySubType;	
	u8 byBoardVer	= tVmpStatus.m_tStatus.tVmp.m_byBoardVer;

	for (byVmpOutChnnl; byVmpOutChnnl < MAXNUM_MPU_OUTCHNNL; byVmpOutChnnl ++)
	{
		
		if (byBoardVer == MPU_BOARD_A128 && byVmpOutChnnl == 3) //A��ֻ��3·
		{
			ConfLog( FALSE, "MPU A version, only 3 out channel!\n" );
			break;
		}
		byChnnlRes = GetResByVmpOutChnnl(byMediaType, byVmpOutChnnl, m_tConf, m_tVmpEqp.GetEqpId());

		GetWHByRes(byChnnlRes, wWidth, wHeight);
		cKDVVMPParam.SetVideoWidth(wWidth);
		cKDVVMPParam.SetVideoHeight(wHeight);
		cKDVVMPParam.m_byEncType = byMediaType;
		
		wMinMtReqBitrate = GetLstRcvGrpMtBr(TRUE, byMediaType, byChnnlRes);
		//wMinMtReqBitrate = GetLeastMtReqBitrate(TRUE, cKDVVMPParam.m_byEncType);
		m_awVMPBrdBitrate[byVmpOutChnnl] = wMinMtReqBitrate;

		cKDVVMPParam.SetBitRate(wMinMtReqBitrate);

// 		if(g_bPrintEqpMsg)
// 		{
// 			OspPrintf(TRUE, FALSE, "[ChangeNewVmpParam] vmp param[%u] is followed:\n",byVmpOutChnnl);
// 			cKDVVMPParam.Print();
// 		} 

        //zbq[07/27/2009] �Ƿ�ǿ�Ʊ������1080i, ����720p
        if (g_cMcuVcApp.IsSVmpOutput1080i())
        {
            if (VIDEO_FORMAT_HD1080 == byChnnlRes)
            {
                cKDVVMPParam.SetVideoHeight(544);
            }
            else if (VIDEO_FORMAT_HD720 == byChnnlRes)
            {
                cKDVVMPParam.SetBitRate(0);
            }
        }
		
		// xliang [7/28/2009] �������û�и������������SVMP��EVPU��
		if( VIDEO_FORMAT_HD1080 != m_tConf.GetMainVideoFormat() 
			&& VIDEO_FORMAT_HD720 != m_tConf.GetMainVideoFormat() )
		{
			if( VIDEO_FORMAT_HD1080 == byChnnlRes || VIDEO_FORMAT_HD720 == byChnnlRes )
			{
				cKDVVMPParam.SetBitRate(0);
			}
		}
		cServMsg.CatMsgBody((u8 *)&cKDVVMPParam, sizeof(cKDVVMPParam));
	}
    
	//�Ƿ���ҪPRS
	u8 byNeedPrs = m_tConf.GetConfAttrb().IsResendLosePack();
	cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs));

	//����ϳɷ��
    u8 bySchemeId = ptVMPParam->GetVMPSchemeId();
    TVmpStyleCfgInfo tMcuVmpStyle;    
    if(0 == bySchemeId)
    {
        EqpLog("[ChangeVmpParam] SchemeId :%d, use default\n", bySchemeId);
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
                ConfLog( FALSE, "[ChangeVmpParam] Get vmp cfg failed! use default(NoIdx)\n");                
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
                
                EqpLog("[ChangeVmpParam] get vmpstyle info success, SchemeId :%d\n", bySchemeId);

            }
        }
        else
        {
            tMcuVmpStyle.ResetDefaultColor();
            OspPrintf(TRUE, FALSE, "[ChangeVmpParam] Get vmp cfg failed! use default\n");
        }
    }
    EqpLog("[ChangeVmpParam] GRDColor.0x%x, AudFrmColor.0x%x, ChairFrmColor.0x%x, SpeakerFrmColor.0x%x\n",
            tMcuVmpStyle.GetBackgroundColor(),
            tMcuVmpStyle.GetFrameColor(),
			
            tMcuVmpStyle.GetChairFrameColor(),
            tMcuVmpStyle.GetSpeakerFrameColor() );
	
    tMcuVmpStyle.SetIsRimEnabled( ptVMPParam->GetIsRimEnabled() );

    tConfVMPParam.SetVMPSchemeId(bySchemeId);
	tConfVMPParam.SetIsRimEnabled( ptVMPParam->GetIsRimEnabled() );

    m_tConf.m_tStatus.SetVmpParam(tConfVMPParam);

    cServMsg.CatMsgBody((u8*)&tMcuVmpStyle, sizeof(tMcuVmpStyle));


	//print:
	if(g_bPrintEqpMsg)
	{
		EqpLog("PeriEqpStatus vmpParam is follow:\n===========\n");
		ptVMPParam->Print();
	}


// 	EqpLog("ConfVmpParam is follow:\n===============\n");
// 	tConfVMPParam.Print();

	//����Ϣ
	if (bStart)
	{
		SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, 6*1000);
		SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_STARTVIDMIX_REQ, cServMsg);
	}
	else
	{
		SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_CHANGEVIDMIXPARAM_REQ, cServMsg);
	}

    m_tConfInStatus.SetVmpNotify(FALSE);
}

BOOL32 CMcuVcInst::VidResAdjust(u8 byVmpStyle, u8 byPos, u8 byMtStandardFormat, u8 &byReqRes)
{
	u16 wBR = 0;		//bitrate
	s32 nResCmpRst = 3;	//3 indicate no meaning for comparing resolution
	BOOL32 bNeedAdjRst = FALSE;
	
	/*
	if(byVmpSubType == MPU_SVMP)
	{
		if( (byMtStandardFormat == VIDEO_FORMAT_HD1080 || byMtStandardFormat == VIDEO_FORMAT_HD720)
			&& (byVmpStyle == VMP_STYLE_HTWO 
			|| byVmpStyle == VMP_STYLE_VTWO
			|| byVmpStyle == VMP_STYLE_THREE
			|| byVmpStyle == VMP_STYLE_FOUR 
			|| byVmpStyle == VMP_STYLE_SPECFOUR )
			)
		{
			return FALSE;
		}
	}
	else if(byVmpSubType == MPU_DVMP)
	{
		//FIXME
	}
	else //EVPU
	{
		//FIXME
	}
	*/

	if((byMtStandardFormat == VIDEO_FORMAT_HD1080 || byMtStandardFormat == VIDEO_FORMAT_HD720)
		/*&& byVmpSubType != MPU_EVPU*/ )
	{
		g_cMcuVcApp.m_cVmpMemVidLmt.GetMtVidLmtForHd(byVmpStyle, byPos, byReqRes, wBR);
	}
	else if( byMtStandardFormat == VIDEO_FORMAT_4CIF || byMtStandardFormat == VIDEO_FORMAT_CIF
		/*|| byVmpSubType == MPU_EVPU*/ )
	{
		g_cMcuVcApp.m_cVmpMemVidLmt.GetMtVidLmtForStd(byVmpStyle, byPos, byReqRes, wBR);	
	}
	
	nResCmpRst = ResWHCmp(byMtStandardFormat,byReqRes);
	
	if( nResCmpRst == 2 //value 2 indicates the former resolution is bigger than the latter both in width and height.
		|| nResCmpRst == -1 //value -1 indicates the former resolution is only bigger in height
		|| nResCmpRst == 1	//1 indicates the former is only bigger in width
		)
	{
		bNeedAdjRst = TRUE;
	}

	EqpLog( "Original Res :%s,",GetResStr(byMtStandardFormat));
	EqpLog( "require Res: %s,", GetResStr(byReqRes));
	EqpLog("Compare result is: %d \n", nResCmpRst);

	return bNeedAdjRst;
}
/*==============================================================================
������    :  AdjustVmpParambyMemberType
����      :  VMP�����˸��棬��ѯ�������
�㷨ʵ��  :  �µķ������Ѿ���vmpĳ�����Ƿ����˸����ͨ�������Ųλ�������˸���ͨ����
		     ԭ��ռ��ͨ��λ�����,������ʷ��¼�����´������·��������ָ�
����˵��  :  
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-3-20					Ѧ��							create
==============================================================================*/
void CMcuVcInst::AdjustVmpParambyMemberType(TMt *ptSpeaker, TVMPParam &tVmpParam, u8 byMemberType ,TMt *ptOldSrc)
{
	TMt tMt;
	TVMPMember tVmpMember;
	u8 byLoop = 0;
	u8 bytmpLastSpeakerVmpChnnl = ~0; //��ʼ��Ϊ������ֵ
	u8 bytmpLastVmpMemberType	= 0;
	u8 byMaxMemberNum			= tVmpParam.GetMaxMemberNum();

	//ֻ�����ն˽���ͨ��������������豸
	if (ptSpeaker == NULL || ptSpeaker->GetType() == TYPE_MCUPERI)
	{ 
		tMt.SetNull();
	}
	else
	{
		tMt = *ptSpeaker;
	}
	for (byLoop = 0; byLoop < byMaxMemberNum; byLoop++)
	{
		//�µķ������Ѿ���vmpĳ�����Ƿ����˸����ͨ�������Ųλ�������˸���ͨ��
		//ԭ��ռ��ͨ��λ�����,������ʷ��¼�����´������·��������ָ�
		tVmpMember = *tVmpParam.GetVmpMember(byLoop);
		if( !tMt.IsNull()
			&& (tVmpMember.GetMtId() == tMt.GetMtId())
			&& (tVmpMember.GetMemberType() != byMemberType)
			)
		{
			
			StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
			tVmpParam.ClearVmpMember(byLoop);
			bytmpLastSpeakerVmpChnnl = byLoop;	//��Ϊʱ�򲻶����ȴ�����ʱ����
			bytmpLastVmpMemberType = tVmpMember.GetMemberType(); //membertype Ҳ����
		}
		
		if ( tVmpMember.GetMemberType() == byMemberType )
		{
			//�����˸���ͨ�������
			StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
			tVmpParam.ClearVmpMember(byLoop);
			
			//֮ǰ���ڸ���λMT����ʷͨ����¼�����ø�MT�ָ�����ʷͨ����
			if(m_tLastVmpChnnlMemInfo.GetLastVmpMemChnnl() < MAXNUM_MPUSVMP_MEMBER) 
			{
				//��ʷͨ���г�ԱΪ�ղŻָ�
				if( (tVmpParam.GetVmpMember(m_tLastVmpChnnlMemInfo.GetLastVmpMemChnnl())->IsNull())
					&& (ptOldSrc != NULL && !ptOldSrc->IsNull())
					&& (!tVmpParam.IsMtInMember(*ptOldSrc)) //Ҳ����������ͨ������ظ�
					)
				{
					tVmpMember.SetMemberTMt(*ptOldSrc); //�ָ��ľ�ֻ�����Ǿɷ�����
					tVmpMember.SetMemberType(m_tLastVmpChnnlMemInfo.GetLastVmpMemType());
					tVmpParam.SetVmpMember(m_tLastVmpChnnlMemInfo.GetLastVmpMemChnnl(), tVmpMember);
				}
			}
			
			// �·����˷ŵ������˸���ͨ����
			tVmpMember.SetMemberTMt(tMt);
			tVmpMember.SetMemberType(byMemberType);
			tVmpParam.SetVmpMember(byLoop, tVmpMember);

		}
	}

	m_tLastVmpChnnlMemInfo.SetLastVmpMemChnnl(bytmpLastSpeakerVmpChnnl);
	m_tLastVmpChnnlMemInfo.SetLastVmpMemType(bytmpLastVmpMemberType);

	EqpLog("[AdjustVmpParambyMemberType] m_byLastVmpMemInChnnl:%u\n",bytmpLastSpeakerVmpChnnl);

}

/*==============================================================================
������    :  AssignPrsChnnl4HdConf
����      :  ����PRS��Դ ����������ã�
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-4-30	4.6				Ѧ��							create
==============================================================================*/
BOOL32 CMcuVcInst::AssignPrsChnnl4HdConf()
{
	u8 byEqpId;
	u8 byChnIdx;
	TPeriEqpStatus tStatus;
	u8 byNeedPrsChls = DEFAULT_PRS_CHANNELS;

	// HD����MAXԤ��PRS��ͨ������ȱʡ��3· + �㲥ԴMAX 4· + ������Ƶ1· + ����˫��1·  ��Max��ͨ��ռ9·��

	//1, ȷ��PRSͨ��Ԥ����Ŀ, VMP 4����Ԥ��
/*	//�㲥ԴMAX 4��[1080][720][4cif][cif/other],���ݻ����ʽ�ֱ���ȷ��������Ŀ
	u8 byBrd1080Chnnl	= EQP_CHANNO_INVALID;
	u8 byBrd720Chnnl	= EQP_CHANNO_INVALID;
	u8 byBrd4CifChnnl	= EQP_CHANNO_INVALID;
	u8 byBrdCifOthChnnl	= EQP_CHANNO_INVALID;
	u8 byBrdOtherChnnl	= EQP_CHANNO_INVALID;
	
	if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType())
	{
		if( VIDEO_FORMAT_HD1080 == m_tConf.GetMainVideoFormat())
		{
			byNeedPrsChls += 1;
			byBrd1080Chnnl = 1;
			
			if (m_tConf.GetConfAttrbEx().IsResEx720())
			{	
				byNeedPrsChls += 1;
				byBrd720Chnnl = 1;
			}
			if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
			{
				byNeedPrsChls += 1;
				byBrd4CifChnnl = 1;
			}
			if (m_tConf.GetConfAttrbEx().IsResExCif())
			{
				byNeedPrsChls += 1;
				byBrdCifOthChnnl = 1;
			}
		}
		else if (VIDEO_FORMAT_HD720 == m_tConf.GetMainVideoFormat())
		{
			byNeedPrsChls += 1;
			byBrd720Chnnl = 1;
			
			if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
			{
				byNeedPrsChls += 1;
				byBrd4CifChnnl = 1;
			}
			if (m_tConf.GetConfAttrbEx().IsResExCif())
			{
				byNeedPrsChls += 1;
				byBrdCifOthChnnl = 1;
			}
		}
		else if ( VIDEO_FORMAT_4CIF == m_tConf.GetMainVideoFormat())
		{
			byNeedPrsChls += 1;
			byBrd4CifChnnl = 1;
			
			if (m_tConf.GetConfAttrbEx().IsResExCif())
			{
				byNeedPrsChls += 1;
				byBrdCifOthChnnl = 1;
			}
		}
		else if (VIDEO_FORMAT_CIF == m_tConf.GetMainVideoFormat())
		{
			byNeedPrsChls += 1;
			byBrdCifOthChnnl = 1;
		}
		else
		{
			//do nothing
		}
	}
	else //��h264��ʽ 
	{
		if( EQP_CHANNO_INVALID == byBrdCifOthChnnl ) //other��ʽ�ĺ�CIF�����棬���Ը���
		{
			byNeedPrsChls += 1;
			byBrdCifOthChnnl = 1;
		}
	}
*/	
	byNeedPrsChls += MAXNUM_MPU_OUTCHNNL;
	//������Ƶ FIXME
	
	
	//����˫�� FIXME
	
	//2, PRSͨ����Ŀȷ���󣬽��и�ͨ������
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
		
		m_byPrsChnnlVmpOut1 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut1].SetReserved(TRUE);
		tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut1, m_byConfIdx );

		m_byPrsChnnlVmpOut2 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut2].SetReserved(TRUE);
		tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut2, m_byConfIdx );

		m_byPrsChnnlVmpOut3 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut3].SetReserved(TRUE); 
		tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut3, m_byConfIdx );

		m_byPrsChnnlVmpOut4 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut4].SetReserved(TRUE);
		tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut4, m_byConfIdx );

		/*
		if(EQP_CHANNO_INVALID != byBrd1080Chnnl)
		{
			m_byPrsChnnlVmpOut1 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut1].SetReserved(TRUE);
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut1, m_byConfIdx );
		}                  
		
		if(EQP_CHANNO_INVALID != byBrd720Chnnl)
		{
			m_byPrsChnnlVmpOut2 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut2].SetReserved(TRUE);
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut2, m_byConfIdx );
		}       
		
		if(EQP_CHANNO_INVALID != byBrd4CifChnnl)
		{
			m_byPrsChnnlVmpOut3 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut3].SetReserved(TRUE); 
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut3, m_byConfIdx );
		}
		
		if(EQP_CHANNO_INVALID != byBrdCifOthChnnl)
		{
			m_byPrsChnnlVmpOut4 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut4].SetReserved(TRUE);
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut4, m_byConfIdx );
		}
		*/
		// �����2· FIXME
		//  ...
		
		g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
		
		ConfLog(FALSE, "Prs eqp %d chl %d, chl2 %d, chlaud %d, chl1080 %d, chl720 %d, ch4Cif %d, chCifOther %d,\
			chlAudBas %d, chlDsBas %d reserved for conf %s\n",
			byEqpId, 
			m_byPrsChnnl, m_byPrsChnnl2, m_byPrsChnnlAud, 
			m_byPrsChnnlVmpOut1, m_byPrsChnnlVmpOut2, m_byPrsChnnlVmpOut3, m_byPrsChnnlVmpOut4,
			m_byPrsChnnlAudBas, m_byPrsChnnlDsVidBas, 
			m_tConf.GetConfName());

		return TRUE;
	}	
	else
	{
		return FALSE;
	}

}

/*==============================================================================
������    :  AssignPrsChnnl4SdConf
����      :  ����PRS��Դ ����������ã�
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-4-30	4.6				Ѧ��							create
==============================================================================*/
BOOL32 CMcuVcInst::AssignPrsChnnl4SdConf( void )
{
	u8 byEqpId;
	u8 byChnIdx;
	TPeriEqpStatus tStatus;

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
		//g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
		
		if(EQP_CHANNO_INVALID != m_byVidBasChnnl)
		{
			m_byPrsChnnlVidBas = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			//g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVidBas].SetReserved(TRUE);
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVidBas, m_byConfIdx );
			//tStatus.SetConfIdx(m_byConfIdx);                        
			//g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
		}                  
		
		if(EQP_CHANNO_INVALID != m_byBrBasChnnl)
		{
			m_byPrsChnnlBrBas = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			//g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlBrBas].SetReserved(TRUE);
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlBrBas, m_byConfIdx );
			//tStatus.SetConfIdx(m_byConfIdx);                        
			//g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
		}   
		
		if(EQP_CHANNO_INVALID != m_byAudBasChnnl)
		{
			m_byPrsChnnlAudBas = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			//g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAudBas].SetReserved(TRUE);
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlAudBas, m_byConfIdx );
			//tStatus.SetConfIdx(m_byConfIdx);                        
			//g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
		}                                        
		g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
		
		ConfLog(FALSE, "Prs eqp %d chl %d, chl2 %d, chlaud %d, chlVidBas %d, chlBrBas %d, chlAudBas %d reserved for conf %s\n",
			byEqpId, m_byPrsChnnl,m_byPrsChnnl2,m_byPrsChnnlAud, m_byPrsChnnlVidBas, 
			m_byPrsChnnlBrBas, m_byPrsChnnlAudBas, m_tConf.GetConfName());

		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

/*==============================================================================
������    :  AssignPrsChnnl
����      :  ����PRS��Դ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-4-30	4.6				Ѧ��							create
==============================================================================*/
BOOL32 CMcuVcInst::AssignPrsChnnl( void )
{
	
	// HD����MAXԤ��PRS��ͨ������ȱʡ��3· + �㲥ԴMAX 4· + ������Ƶ1· + ����˫��1·  ��Max��ͨ��ռ9·��
	if ( IsHDConf(m_tConf) )
	{
		return AssignPrsChnnl4HdConf();
	}
	else	//��������Prs��ԴԤ��
	{
		return AssignPrsChnnl4SdConf();
	}
	
}
/*=============================================================================
�� �� ���� GetMtRealSrc
��    �ܣ� ��ȡָ���ն˵�ʵ�ʽ���Դ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
090423      4.0	        fxh					  ����
=============================================================================*/
BOOL32 CMcuVcInst::GetMtRealSrc(u8 byMtId, u8 byMode, TMt& tMtSrc, u8& byOutChnl)
{
	tMtSrc.SetNull();
	byOutChnl = 0;
	if (m_ptMtTable->GetMtSrc(byMtId, &tMtSrc, byMode) &&
		(tMtSrc.IsLocal() ? (tMtSrc.GetMtId() != byMtId) : (tMtSrc.GetMcuId() != byMtId)))
	{
		u8 byType = tMtSrc.GetType();
		if (TYPE_MT == byType)
		{
			TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(tMtSrc.GetMtId());
			TSimCapSet tDstCap = m_ptMtTable->GetDstSCS(byMtId);
			if (MODE_AUDIO == byMode)
			{
				if (tSrcCap.GetAudioMediaType() != tDstCap.GetAudioMediaType() &&
					!m_tAudBasEqp.IsNull())
				{
					tMtSrc = m_tAudBasEqp;
					byOutChnl = m_byAudBasChnnl;
					return TRUE;
				}
			}else if (MODE_VIDEO == byMode)
			{
				if (tSrcCap != tDstCap)
				{
					if (tMtSrc == m_tVidBrdSrc)
					{
					    TEqp tBas;
						u8 byFindSrc   = FALSE;
						u8 byInChnlId  = 0;
						u8 byOutChnlId = 0;
						u8 byAdpType   = ADAPT_TYPE_NONE;
						if (m_cMtRcvGrp.IsMtNeedAdp(byMtId))
						{
							u8 byRes = 0;
							u8 byMediaType = MEDIA_TYPE_NULL;
							if (m_cMtRcvGrp.GetMtMediaRes(byMtId, byMediaType, byRes) &&
								m_cBasMgr.GetBasResource(byMediaType, byRes, tBas, byInChnlId, byOutChnlId))
							{
								byFindSrc = TRUE;
							}
							else
							{
								u8 byDstType = tDstCap.GetVideoMediaType();
								u8 byDstRes  = tDstCap.GetVideoResolution();
								if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
									MEDIA_TYPE_H264 == byDstType)
								{
									while (GetProximalGrp(byDstType, byDstRes, byDstType, byDstRes))
									{
										if (m_cBasMgr.GetBasResource(byDstType, byDstRes,
											                         tBas, byInChnlId, byOutChnlId))
										{
											byFindSrc = TRUE;
											break;
										}
										
									}
								}
							}

							if (byFindSrc)
							{
								tMtSrc = tBas;
								byOutChnl = byInChnlId * MAXNUM_VOUTPUT + byOutChnlId;
								EqpLog("[GetMtRealSrc] hdbas(id:%d, outchnl:%d)--->dstmt(%d)\n",
								        tMtSrc.GetEqpId(), byOutChnl, byMtId);
							    return TRUE;	
							}
						}
						// �����ն��Ƿ��ڱ���������
						else if (IsMtSrcBas(byMtId, MODE_VIDEO, byAdpType))
						{
							if (ADAPT_TYPE_VID == byAdpType)
							{
								tMtSrc = m_tVidBasEqp;
								byOutChnl = m_byVidBasChnnl;
							}
							else if (ADAPT_TYPE_BR == byAdpType)
							{
								tMtSrc = m_tBrBasEqp;
								byOutChnl = m_byBrBasChnnl;
							}            
							else if (ADAPT_TYPE_CASDVID == byAdpType)
							{
								tMtSrc = m_tCasdVidBasEqp;
								byOutChnl = m_byCasdVidBasChnnl;
							}
							EqpLog("[GetMtRealSrc] sdbas(id:%d, outchnl:%d)--->dstmt(%d)\n",
								   tMtSrc.GetEqpId(), byOutChnl, byMtId);
							return TRUE;	
						}
						else
						{
							EqpLog("[GetMtRealSrc] Fail to find actual broadbassrc for mt%d\n",
								   byMtId);
						}
					}
					else
					{
						u8 bySelSrcId = (tMtSrc.IsLocal() ? tMtSrc.GetMtId() : tMtSrc.GetMcuId());
						TEqp tBasEqp;
						u8 byChnl;
						if (m_cSelChnGrp.IsMtInSelAdpGroup(bySelSrcId, byMtId, &tBasEqp, &byChnl))
						{
							return TRUE;
						}						
					}

				}
			}
			else if (MODE_SECVIDEO == byMode)
			{
			}
			else
			{
				
			}
		}
		else if (TYPE_MCUPERI == byType)
		{
		}
		else
		{
			ConfLog(FALSE, "[GetMtRealSrc]unproc src type(%d)\n", byType);
		}
	}
	return FALSE;

}
/*=============================================================================
�� �� ���� ProcMixStopSucRsp
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
090423      4.0	        fxh					  ��ȡֹͣ�����ɹ�ҵ����Ӧ����
=============================================================================*/
void CMcuVcInst::ProcMixStopSucRsp(void)
{
	CServMsg cServMsg;
	if (m_tConf.m_tStatus.IsVACing())//����
	{
        m_tConf.m_tStatus.SetVACing(FALSE);            
		//֪ͨ��ϯ�����л��
		SendMsgToAllMcs( MCU_MCS_STOPVAC_NOTIF, cServMsg );
		if (HasJoinedChairman())
		{
			SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cServMsg );
		}

		//ֹͣ��������������ݣ����ָ������㲥Դ
		for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
		{
			if (m_tConfAllMtInfo.MtJoinedConf(byMtId))            
			{
				StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
							        (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(byMtId)), FALSE, MODE_AUDIO );
			
                // xsl [7/29/2006]
                m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);
            }
		}
	}
	
	if(m_tConf.m_tStatus.IsMixing())//����(����)
	{
        m_tConf.m_tStatus.SetNoMixing();

		TMcu tMcu;
		tMcu.SetMcuId(LOCAL_MCUID);
		cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
		if (!m_tCascadeMMCU.IsNull())
		{
			cServMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
			SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_STOPMIXER_NOTIF, cServMsg);
		}
		if (HasJoinedChairman())
		{
			SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STOPDISCUSS_NOTIF, cServMsg );
		}
        
		//ֹͣ��������������ݣ����ָ������㲥Դ
		for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
		{
			if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
			{
				StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
							        (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(byMtId)), FALSE, MODE_AUDIO);

				StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);                       
                
                // xsl [9/21/2006] �ָ����鲥��ַ����
                if (m_tConf.GetConfAttrb().IsSatDCastMode() && m_ptMtTable->IsMtInMixGrp(byMtId))
                {
                    ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_AUDIO);
                }

                // xsl [7/29/2006]
                m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);

                // guzh [11/7/2007]
                m_ptMtTable->SetMtInMixing(byMtId, FALSE);

                // zbq [11/22/2007] ʵʱˢ�ն�״̬
                MtStatusChange(byMtId, TRUE);
			}
		}
        
        // guzh [5/12/2007] ���ӳ���(��ֹ����)
        memset( m_abyMixMtId, 0, sizeof(m_abyMixMtId) );

		//�ı���ƵԴ
		if (HasJoinedSpeaker())
		{
			TMt tSpeakerMt = GetLocalSpeaker();
			ChangeAudBrdSrc(&tSpeakerMt);
		}
		else
		{
			ChangeAudBrdSrc( NULL );
		}

		//֪ͨ������ֹͣ���ն˷�����
		NotifyMixerSendToMt( FALSE );

		//���ϼ�MCU���ն�����
		if (!m_tCascadeMMCU.IsNull())
		{
			TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
			if (NULL != ptInfo && 
				m_tConfAllMtInfo.MtJoinedConf(ptInfo->m_tSpyMt.GetMcuId(), ptInfo->m_tSpyMt.GetMtId()))
			{
				StartSwitchToSubMt( ptInfo->m_tSpyMt, 0, m_tCascadeMMCU.GetMtId(), MODE_AUDIO, SWITCH_MODE_SELECT);
			}
		}
	}
    //֪ͨ��ϯ�����л�� 
    MixerStatusChange();

    //zbq[11/29/2007]֪ͨ�ϼ�MCU��ǰ�ն�״̬
    if ( !m_tCascadeMMCU.IsNull() )
    {
        OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId());
    }

	//����������
	ConfLog(FALSE, "give up mixer group.%u!\n",m_byMixGrpId);
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
	tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = TMixerGrpStatus::READY;
	g_cMcuVcApp.SetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
	cServMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
    SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
    
	m_tVacLastSpeaker.SetNull();

    // ������Զ��л�����-->����
    if ( !m_tConf.IsDiscussAutoStop() && m_tConfInStatus.IsSwitchDiscuss2Mix() )
    {
        SwitchDiscuss2SpecMix();
    }
}
/*=============================================================================
�� �� ���� ProcVMPStopSucRsp
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
090423      4.0	        fxh					  ��ȡֹͣ����ϳ�ҵ����
=============================================================================*/
void CMcuVcInst::ProcVMPStopSucRsp(void)
{
	//����״̬
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tPeriEqpStatus);
	tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;

	m_tConf.m_tStatus.SetVMPMode(CONF_VMPMODE_NONE);

	//ֹͣ���渴�Ϻ�VMPģ�岻����Ч
	m_tConfInStatus.SetVmpModuleUsed(TRUE);
	// xliang [12/23/2008] ��ʽ����һ��
	//m_tConf.m_tStatus.m_tVMPParam.SetVMPBatchPoll(0);

	//ֹͣ��������
	ChangeVmpSwitch( 0, VMP_STATE_STOP );

	TVMPParam tVMPParam;
	memset(&tVMPParam, 0, sizeof(tVMPParam));
	m_tConf.m_tStatus.SetVmpParam( tVMPParam );

	//ͬ�����踴��״̬
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
	g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );

	// xliang [5/4/2009] ���踴��״̬�ϱ����
	CServMsg cServMsg;
	cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
	SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );

	//֪ͨ��ϯ�����л��
	cServMsg.SetMsgBody();
	cServMsg.SetMsgBody( (u8*)&tVMPParam, sizeof(TVMPParam) );
	SendMsgToAllMcs( MCU_MCS_STOPVMP_NOTIF, cServMsg );
	if( HasJoinedChairman() )
	{
		SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVMP_NOTIF, cServMsg );
	}

    SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
	
    m_tConfInStatus.SetVmpNotify(FALSE);

    // guzh [6/9/2007] �����¼
    memcpy(&m_tLastVmpParam, &tVMPParam, sizeof(TVMPParam));

	// xliang [12/16/2008] ���vmp channel������Ϣ��(������vmp��û�����channel�������һ�����)
	m_tVmpChnnlInfo.clear();
//			memset(m_abyMtVmpChnnl,0,sizeof(m_abyMtVmpChnnl));
	memset(m_abyMtNeglectedByVmp,0,sizeof(m_abyMtNeglectedByVmp));
	memset(&m_tVmpPollParam,0,sizeof(m_tVmpPollParam));
	memset(&m_tVmpBatchPollInfo,0,sizeof(m_tVmpBatchPollInfo));
	m_tLastVmpChnnlMemInfo.Init();
	//m_byLastVmpMemInChnnl = ~0;
	memset(m_atVMPTmpMember,0,sizeof(m_atVMPTmpMember));

	//m_tVmpCasMemInfo.Init();

    m_byVmpOperating = 0;

	memset(&m_tVmpEqp, 0, sizeof(m_tVmpEqp));

    //n+1���ݸ���vmp��Ϣ
    if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
    {
        ProcNPlusVmpUpdate();
    }

	//�ر�ѡ����vmp��hduͨ��
    TPeriEqpStatus tHduStatus;
    u8 byHduId = HDUID_MIN;
    while( byHduId >= HDUID_MIN && byHduId <= HDUID_MAX )
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byHduId))
		{
			g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
            if (tHduStatus.m_byOnline == 1)
			{
                u8 byTmpMtId = 0;
                u8 byMtConfIdx = 0;
                u8 byMemberNum = tHduStatus.m_tStatus.tHdu.byChnnlNum;
				for(u8 byLoop = 0; byLoop < MAXNUM_HDU_CHANNEL; byLoop++)
				{
                    byTmpMtId = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetMtId();
                    byMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
					if (m_tVmpEqp.GetEqpId() == byTmpMtId && m_byConfIdx == byMtConfIdx)
					{
						StopSwitchToPeriEqp(byHduId, byLoop);
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = 0;
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
                    }
               	}
                g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);
		
                cServMsg.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
			}
		}
        byHduId++;
	}	
}
			
// END OF FILE

