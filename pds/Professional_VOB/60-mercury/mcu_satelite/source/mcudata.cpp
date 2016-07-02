/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcudata.cpp
   ����ļ�    : mcudata.h
   �ļ�ʵ�ֹ���: MCU����ģ��Ӧ������ͨ��������
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/05/29  0.9         ����        ����
   2005/02/19  3.6         ����      �����޸ġ���3.5�汾�ϲ�
******************************************************************************/

#include "evmcumcs.h"
#include "evmcumt.h"
#include "evmcueqp.h"
#include "evmcu.h"
#include "evmp.h"
#include "mcuvc.h"
#include "mcudata.h"
#include "mcuerrcode.h"
#include "mcuutility.h"
#include "mtadpssn.h"
#include "mcsssn.h"
#include "vcsssn.h"

#if defined(_LINUX_)
#include "boardwrapper.h"
#else
#include "brddrvlib.h"
#endif

#define CHECK_CONFIDX(byConfIdx)                                    \
    if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)          \
    {                                                               \
        OspPrintf(TRUE, FALSE, "[CheckConfIdx] invalid confidx :%d\n", byConfIdx); \
        return FALSE;                                               \
    }

u32  g_dwVCUInterval;

/*--------------------------------------------------------------------------*/
/*                                CMcuVcData                                */
/*--------------------------------------------------------------------------*/

CMcuVcData::CMcuVcData()
{
	//����
	ClearVcData();
    g_dwVCUInterval = 3*OspClkRateGet();    //3���tick��
}

CMcuVcData::~CMcuVcData()
{
    MCU_SAFE_DELETE(m_ptTemplateInfo);
}

void CMcuVcData::ClearVcData()
{
	memset( m_apConfInst, 0, sizeof( m_apConfInst ) );
	memset( m_atPeriEqpTable, 0, sizeof( m_atPeriEqpTable ) );
	memset( m_atMcTable, 0, sizeof( m_atMcTable ) );
	memset( m_atMpData, 0, sizeof(m_atMpData) );
	memset( m_atMtAdpData, 0, sizeof(m_atMtAdpData) );
	memset( m_atRecvMtPort, 0, sizeof(m_atRecvMtPort) );
	memset( m_atMulticastPort, 0, sizeof(m_atMulticastPort) );
	memset( m_abyConfStoreInfo, 0, sizeof(m_abyConfStoreInfo) );
	memset( m_abyConfRegState, 0, sizeof(m_abyConfRegState) );
	memset( (void*)&m_tGKID, 0, sizeof(m_tGKID) );
	memset( (void*)&m_tEPID, 0, sizeof(m_tEPID) );
	// memset( (void*)&m_tMcuDebugVal, 0, sizeof(m_tMcuDebugVal) ); // guzh �й��캯��,no memeset
	memset( m_atMtCallInterface, 0, sizeof(m_atMtCallInterface) );
	memset( m_atPeriDcsTable, 0, sizeof(m_atPeriDcsTable) );
	memset( m_abyConfRefreshTimes, 0, sizeof(m_abyConfRefreshTimes) );
	
	m_dwMakeConfIdTimes = 0;
	m_byRegGKDriId  = 0;
    m_byChargeRegOK = 0;
	m_dwMtCallInterfaceNum = 0;

	m_ptTemplateInfo = NULL;

	m_cVmpMemVidLmt.Init(); //VMP ��ͨ����Ա�������� 
 	memset( m_adwEqpIp, 0, sizeof(m_adwEqpIp) );
   	
	return;
}

/*====================================================================
    ������      ��IsPeriEqpConnected
    ����        ����ȡ��������״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpId, ���� ID
    ����ֵ˵��  ��TRUE: connected FALSE: unnnected
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/04/06    1.0         ���         ����
====================================================================*/
BOOL32 CMcuVcData::IsPeriEqpConnected( u8 byEqpId )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP )
	{
		return FALSE;
	}

	return( m_atPeriEqpTable[byEqpId - 1].m_tPeriEqpStatus.m_byOnline );
}

/*====================================================================
    ������      ��GetPeriEqpLogicChnnl
    ����        ����ȡ������߼�ͨ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpId, ���� ID
				  u8 byMediaType, ý������
				  TLogicalChannel * ptChnnl, ���ص��߼�ͨ������
				  u8 * pbyChnnlNum, ���ص�MCU������ͨ����
				  BOOL32 bForwardChnnl, ����
    ����ֵ˵��  ��TRUE: success	FALSE: fail
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/04/06    1.0         ���         ����
====================================================================*/
BOOL32 CMcuVcData::GetPeriEqpLogicChnnl( u8 byEqpId, u8 byMediaType, u8 * pbyChnnlNum, TLogicalChannel * ptStartChnnl, BOOL32 bForwardChnnl )
{
	if( 0 == byEqpId || byEqpId > MAXNUM_MCU_PERIEQP || NULL == pbyChnnlNum || NULL == ptStartChnnl )
	{
		return FALSE;
	}

	switch( byMediaType )
	{
	case MODE_VIDEO:
		if( bForwardChnnl )
		{
			*pbyChnnlNum = m_atPeriEqpTable[byEqpId - 1].m_byFwdChannelNum;
			*ptStartChnnl = m_atPeriEqpTable[byEqpId - 1].m_tFwdVideoChannel;
		}
		else
		{
			*pbyChnnlNum = m_atPeriEqpTable[byEqpId - 1].m_byRvsChannelNum;
			*ptStartChnnl = m_atPeriEqpTable[byEqpId - 1].m_tRvsVideoChannel;
		}
		break;
	case MODE_AUDIO:
		if( bForwardChnnl )		
		{
			*pbyChnnlNum = m_atPeriEqpTable[byEqpId - 1].m_byFwdChannelNum;
			*ptStartChnnl = m_atPeriEqpTable[byEqpId - 1].m_tFwdAudioChannel;
		}
		else
		{
			*pbyChnnlNum = m_atPeriEqpTable[byEqpId - 1].m_byRvsChannelNum;
			*ptStartChnnl = m_atPeriEqpTable[byEqpId - 1].m_tRvsAudioChannel;
		}
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::GetPeriEqpLogicChnnl(): wrong Mode %u!\n", byMediaType );
		return( FALSE );
	}

	return( TRUE );
}

/*====================================================================
    ������      ��SetEqpRtcpDstAddr
    ����        �����������ͨ��Ŀ��Rtcp��ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpId, ���� ID
				  u8 byChnnl, ͨ����
				  u32 dwDstIp, Ŀ��Ip
				  u16 wDstPort, Ŀ�Ķ˿�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/10/19    3.0         ������         ����
====================================================================*/
void CMcuVcData::SetEqpRtcpDstAddr(u8 byEqpId, u8 byChnnl, u32 dwDstIp, u16 wDstPort, u8 byMode)
{
    if (byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || byChnnl > MAXNUM_PERIEQP_CHNNL)
    {
        return;
    }

    if (MODE_VIDEO == byMode)
    {
        m_atPeriEqpTable[byEqpId - 1].m_tVideoRtcpDstAddr[byChnnl].SetIpAddr(dwDstIp);
        m_atPeriEqpTable[byEqpId - 1].m_tVideoRtcpDstAddr[byChnnl].SetPort(wDstPort);
    }
    else if (MODE_AUDIO == byMode)
    {
        m_atPeriEqpTable[byEqpId - 1].m_tAudioRtcpDstAddr[byChnnl].SetIpAddr(dwDstIp);
        m_atPeriEqpTable[byEqpId - 1].m_tAudioRtcpDstAddr[byChnnl].SetPort(wDstPort);
    }
}

/*====================================================================
    ������      ��GetEqpRtcpDstAddr
    ����        ���õ������ͨ��Ŀ��Rtcp��ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpId, ���� ID
				  u8 byChnnl, ͨ����
				  u32 dwDstIp, Ŀ��Ip
				  u16 wDstPort, Ŀ�Ķ˿�
    ����ֵ˵��  ��TRUE: success	FALSE: fail
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/10/19    3.0         ������         ����
====================================================================*/
BOOL32 CMcuVcData::GetEqpRtcpDstAddr(u8 byEqpId, u8 byChnnl, u32 &dwDstIp, u16 &wDstPort, u8 byMode)
{
    if (byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || byChnnl > MAXNUM_PERIEQP_CHNNL)
    {
        return FALSE;
    }

    if (MODE_VIDEO == byMode)
    {
        dwDstIp = m_atPeriEqpTable[byEqpId - 1].m_tVideoRtcpDstAddr[byChnnl].GetIpAddr();
        wDstPort = m_atPeriEqpTable[byEqpId - 1].m_tVideoRtcpDstAddr[byChnnl].GetPort();
    }
    else if (MODE_AUDIO == byMode)
    {
        dwDstIp = m_atPeriEqpTable[byEqpId - 1].m_tAudioRtcpDstAddr[byChnnl].GetIpAddr();
        wDstPort = m_atPeriEqpTable[byEqpId - 1].m_tAudioRtcpDstAddr[byChnnl].GetPort();
    }

    return ( dwDstIp == 0 || wDstPort == 0 ) ? FALSE : TRUE;
}

/*====================================================================
    ������      ��GetPeriEqpStatus
    ����        ���������״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpId, ���� ID
				  TPeriEqpStatus * ptStatus, ���ص�����״̬
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/10    1.0         ���         ����
====================================================================*/
BOOL32 CMcuVcData::GetPeriEqpStatus( u8 byEqpId, TPeriEqpStatus * ptStatus )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || NULL == ptStatus )
	{
		return( FALSE );
	}

	*ptStatus = m_atPeriEqpTable[byEqpId - 1].m_tPeriEqpStatus;

	return( TRUE );
}
/*====================================================================
    ������      ��SendPeriEqpStatusToMcs
    ����        ������ָ������״̬�����н���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpId:���� ID
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/06/05                ���㻪         ����
====================================================================*/
void CMcuVcData::SendPeriEqpStatusToMcs(u8 byEqpId)
{
	TPeriEqpStatus tEqpStatus;
	if (GetPeriEqpStatus(byEqpId, &tEqpStatus))
	{
		CServMsg cServMsg;
		cServMsg.SetMsgBody((u8 *)&tEqpStatus, sizeof(tEqpStatus));
		CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, 
			                          cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
	}

}
/*====================================================================
    ������      ��IsMcConnected
    ����        ����ȡMC����״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wMcInstId, MC��Ӧʵ��
    ����ֵ˵��  ��TRUE: connected FALSE: unnnected
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/04/06    1.0         ���         ����
====================================================================*/
BOOL32 CMcuVcData::IsMcConnected( u16 wMcInstId )
{
	if( 0 == wMcInstId || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) )
	{
		return FALSE;
	}

	return m_atMcTable[wMcInstId-1].m_bConnected;
}

/*====================================================================
    ������      ��GetMcLogicChnnl
    ����        ����ȡMC���߼�ͨ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wMcInstId, MC ID
				  u8 byMediaType, ý������
				  TLogicalChannel * ptStartChnnl, ���ص��߼�ͨ������
				   u8 * pbyChnnlNum, ���ص�MCU�����ͨ����
    ����ֵ˵��  ��TRUE: success FALSE: fail
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/04/06    1.0         ���         ����
====================================================================*/
BOOL32 CMcuVcData::GetMcLogicChnnl( u16 wMcInstId, u8 byMediaType, u8 * pbyChnnlNum, TLogicalChannel * ptStartChnnl )
{
	if( wMcInstId == 0 || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) || NULL == ptStartChnnl || NULL == pbyChnnlNum )
	{
		return FALSE;
	}

	*pbyChnnlNum = m_atMcTable[ wMcInstId-1 ].m_byFwdChannelNum;
	
	switch( byMediaType )
	{
	case MODE_VIDEO:
		*ptStartChnnl = m_atMcTable[ wMcInstId-1 ].m_tFwdVideoChannel;
		break;
	case MODE_AUDIO:
		*ptStartChnnl = m_atMcTable[ wMcInstId-1 ].m_tFwdAudioChannel;
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::GetMcLogicChnnl(): wrong Mode %u!\n", byMediaType );
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
    ������      ��GetMcSrc
    ����        ����ȡMC�ķ���/������Ƶ����ƵԴ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����uu16 wMcInstId, MC��
				  TMt * ptSrc, ���ص��ն�Դ
				  u8 byChnnlNo, ͨ����
				  u8 byMode, ��ģʽMODE_VIDEO/MODE_AUDIO
    ����ֵ˵��  ��TRUE: success FALSE: fail
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/04/06    1.0         ���         ����
====================================================================*/
BOOL32 CMcuVcData::GetMcSrc( u16 wMcInstId, TMt * ptSrc, u8 byChnnlNo, u8 byMode )
{
	if( wMcInstId == 0 || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) || NULL == ptSrc || byChnnlNo >= MAXNUM_MC_CHANNL )
	{
		return FALSE;
	}

	switch( byMode )
	{
	case MODE_VIDEO:
		*ptSrc = m_atMcTable[ wMcInstId-1 ].m_atVidSrc[byChnnlNo];
		break;
	case MODE_AUDIO:
		*ptSrc = m_atMcTable[ wMcInstId-1 ].m_atAudSrc[byChnnlNo];
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::GetMcSrc(): wrong Mode %u!\n", byMode );
		return FALSE;	
	}

	return TRUE;
}

/*====================================================================
    ������      ��GetPeriEqpSrc
    ����        ����ȡ����Ľ�����Ƶ����ƵԴ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8  periEqp, �����
				  TMt * ptSrc, ���ص��ն�Դ
				  u8 byChnnlNo, ͨ����
				  u8 byMode, ��ģʽMODE_VIDEO/MODE_AUDIO
    ����ֵ˵��  ��TRUE: success FALSE: fail
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/030   1.0         JQL         ����
====================================================================*/
BOOL32 CMcuVcData::GetPeriEqpSrc(u8 byEqpId, TMt * ptSrc, u8 byChnnlNo, u8 byMode )
{
	
	if( byEqpId == 0 || byEqpId > MAXNUM_PERIEQP_CHNNL 
		|| NULL == ptSrc || byChnnlNo >= MAXNUM_PERIEQP_CHNNL )
	{
		return FALSE;
	}

	switch( byMode )
	{
	case MODE_VIDEO:
		*ptSrc = m_atPeriEqpTable[ byEqpId - 1 ].m_atVidSrc[byChnnlNo];
		break;
	case MODE_AUDIO:
		*ptSrc = m_atPeriEqpTable[ byEqpId - 1 ].m_atAudSrc[byChnnlNo];
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::GetMcSrc(): wrong Mode %u!\n", byMode );
		return FALSE;	
	}
	return TRUE;
}

/*====================================================================
    ������      ��MsgPassCheck
    ����        �������Ϣ�Ƿ���Է�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/20    3.0         ������         ����
====================================================================*/
BOOL32 CMcuVcData::MsgPassCheck(u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	BOOL32 bReturn = TRUE;
	CServMsg cServMsg( pbyMsg, wLen );
	u8 byConfIdx, bySrcSsnId, byConfProtectMode;
	CConfId cConfId;
	TConfProtectInfo tProtectInfo;
    TConfMapData tMapData;
 
    switch(wEvent)
	{
	//�κ�����¶���ͨ������Ϣ
    case MCS_MCU_GETMCUSTATUS_CMD:
	case MCS_MCU_GETMCUSTATUS_REQ:	        //��ѯMCU״̬���� 
	case MCS_MCU_LISTALLCONF_REQ:	        //�г���MCU�����л�����Ϣ
	case MCS_MCU_STOPSWITCHMC_REQ:	        //�������ֹ̨ͣ��������
	case MCS_MCU_STOP_SWITCH_TW_REQ:	    //�������̨������ֹͣ��������
	case MCS_MCU_GETMCUPERIEQPSTATUS_REQ:	//��ѯMCU����״̬
	case MCS_MCU_GETRECSTATUS_REQ:	        //��ѯ¼���״̬����
	case MCS_MCU_GETMIXERSTATUS_REQ:	    //��ѯ������״̬����
	case MCS_MCU_CONNECT_REQ:		        //����̨֪ͨMCU����׼��
	case MCS_MCU_CURRTIME_REQ :		        //����̨��ѯmcu��ǰʱ��
	case MCS_MCU_CREATECONF_REQ:            //�������̨��MCU�ϴ���һ������ 
	case MCS_MCU_ENTERCONFPWD_ACK:          //�������̨��ӦMCU����������   
	case MCS_MCU_ENTERCONFPWD_NACK:         //�������̨�ܾ�MCU����������
	case MCU_MCSCONNECTED_NOTIF:            //�������̨����
	case MCU_MCSDISCONNECTED_NOTIF:         //�������̨����
    case MCU_CREATECONF_FROMFILE:           //���ļ���������
	case MCS_MCU_GETMTLIST_REQ:             //�������̨����MCU�ĵõ��ն��б�����
	case MCS_MCU_REFRESHMCU_CMD:            //�������̨����MCU��ˢ������MCU������
	case MCS_MCU_GETCONFINFO_REQ:           //�������̨��MCU��ѯ������Ϣ  
    case MCS_MCU_GETMAUSTATUS_REQ:          //�������̨��MCU��ѯMAU��Ϣ  
	case MCS_MCU_GETPOLLPARAM_REQ:          //�������̨��MCU��ѯ������ѯ����
    case MCS_MCU_GETTWPOLLPARAM_REQ:
	//case MCS_MCU_GETDISCUSSPARAM_REQ:     //��ѯ���۲�������
    case MCS_MCU_GETMIXPARAM_REQ:           //��ѯ���۲�������
	case MCS_MCU_GETVMPPARAM_REQ:			//��ѯ��Ƶ���ϳ�Ա����
	case MCS_MCU_GETCONFSTATUS_REQ:         //�������̨��MCU��ѯ����״̬
	case MCS_MCU_GETMTSTATUS_REQ:           //�����MCU��ѯ�ն�״̬
	case MCS_MCU_GETALLMTSTATUS_REQ:		//�����MCU��ѯ�����ն�״̬
	case MCS_MCU_GETMTALIAS_REQ:            //�����MCU��ѯ�ն˱���
	case MCS_MCU_GETALLMTALIAS_REQ:			//�����MCU��ѯ�����ն˱���
	case MCS_MCU_GETLOCKINFO_REQ:           //�õ����������Ϣ
	case MCS_MCU_MCUMEDIASRC_REQ:
	case MCS_MCU_GETMCULOCKSTATUS_REQ:        
    case MCS_MCU_SETCHAIRMODE_CMD:          //�������̨���û������ϯ��ʽ  // guzh [7/25/2006] ��ʹ����������Ҫ�����Ҳ����ֱ�Ӳ�������Ϊ��ֻ��һ���ڲ����������û�����
	case MCS_MCU_SAVECONFTOTEMPLATE_REQ:	//�������̨���󱣴浱ǰ����Ϊ����ģ��(Ԥ��)  FIXME: Ȩ�ޱ���δ����, zgc, 2007/04/20
    case MCS_MCU_GETMTEXTINFO_REQ:          //��ȡ�ն˵Ķ�����չ��Ϣ
	case MCS_MCU_STOP_SWITCH_HDU_REQ:       //ֹͣHDU����   4.6.1 �¼Ӱ汾  jlb
	case MCS_MCU_VMPPRISEIZE_ACK:			// xliang [12/12/2008] 
	case MCS_MCU_VMPPRISEIZE_NACK:
		break;
		
    //��Ҫ�жϵ���Ϣ
	case MCS_MCU_SENDRUNMSG_CMD:            //�������̨����MCU���ն˷��Ͷ���Ϣ���ն˺�Ϊ0��ʾ���������ն�
	case MCS_MCU_STARTPOLL_CMD:             //�������̨����û��鿪ʼ��ѯ�㲥
	case MCS_MCU_STOPPOLL_CMD:              //�������̨����û���ֹͣ��ѯ�㲥  
	case MCS_MCU_PAUSEPOLL_CMD:             //�������̨����û�����ͣ��ѯ�㲥   
	case MCS_MCU_RESUMEPOLL_CMD:            //�������̨����û��������ѯ�㲥
    case MCS_MCU_CHANGEPOLLPARAM_CMD:       //�������̨����û��������ѯ�б�
	case MCS_MCU_SPECPOLLPOS_REQ:			//�������ָ̨����ѯλ��
    case MCS_MCU_STARTTWPOLL_CMD:           //��ʼ����ǽ��ѯ
    case MCS_MCU_STOPTWPOLL_CMD:
    case MCS_MCU_PAUSETWPOLL_CMD:
    case MCS_MCU_RESUMETWPOLL_CMD:

    case MCS_MCU_STARTHDUPOLL_CMD:           //��ʼhdu��ѯ
    case MCS_MCU_STOPHDUPOLL_CMD:
    case MCS_MCU_PAUSEHDUPOLL_CMD:
    case MCS_MCU_RESUMEHDUPOLL_CMD:

	case MCS_MCU_SETMTBITRATE_CMD:          //�������̨����MCU�����ն�����
	case MCS_MCU_MTCAMERA_CTRL_CMD:         //�������̨�����ն��������ͷ�˶�
	case MCS_MCU_MTCAMERA_CTRL_STOP:        //�������̨�����ն��������ͷֹͣ�˶�
	case MCS_MCU_MTCAMERA_RCENABLE_CMD:     //�������̨�����ն������ң��ʹ��	
	case MCS_MCU_MTCAMERA_SAVETOPOS_CMD:    //�������̨�����ն����������ǰλ����Ϣ����ָ��λ��	
	case MCS_MCU_MTCAMERA_MOVETOPOS_CMD:    //�������̨�����ն������������ָ��λ��
	
	case MCS_MCU_MTSEESPEAKER_CMD:          //ǿ�ƹ㲥������
    case MCS_MCU_ADDMIXMEMBER_CMD:          //�ı������Ա
    case MCS_MCU_REMOVEMIXMEMBER_CMD:
	case MCS_MCU_SETMTVOLUME_CMD:			//���ʵʱ�����ն�����,zgc,2006-12-26
	case MCS_MCU_REPLACEMIXMEMBER_CMD:      //�滻������Ա
        cConfId = cServMsg.GetConfId();
        byConfIdx = GetConfIdx( cConfId );
        if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
        {
            OspPrintf(TRUE, FALSE, "[MsgPassCheck] invalid cConfId :");
            cConfId.Print();
            return FALSE;
        }	    
        tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
        if(!tMapData.IsValidConf()) 
        {
            OspPrintf(TRUE, FALSE, "[MsgPassCheck] invalid instance id :%d\n", tMapData.GetInsId());
            return FALSE;
        }
        GetConfProtectInfo( byConfIdx, &tProtectInfo );
        bySrcSsnId = cServMsg.GetSrcSsnId();
		byConfProtectMode = m_apConfInst[tMapData.GetInsId()-1]->m_tConf.m_tStatus.GetProtectMode();
		if( ( byConfProtectMode == CONF_LOCKMODE_LOCK && 
              (tProtectInfo.IsLockByMcs() && tProtectInfo.GetLockedMcSsnId() != bySrcSsnId || tProtectInfo.IsLockByMcu()) 
             ) || 
            ( byConfProtectMode == CONF_LOCKMODE_NEEDPWD && 
              !tProtectInfo.IsMcsPwdPassed(bySrcSsnId) 
            ) 
          )
		{         
            bReturn = FALSE;
			if(byConfProtectMode == CONF_LOCKMODE_NEEDPWD)
			{
				//������
				cServMsg.SetMsgBody();
                CMcsSsn::SendMsgToMcsSsn( bySrcSsnId, MCU_MCS_ENTERCONFPWD_REQ, 
					                          cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			}
			else
			{
                if ( tProtectInfo.IsLockByMcu() )				
                {
                    u8 byLockedMcuId = tProtectInfo.GetLockedMcuId();
                    TMtAlias tMtAlias;
                    m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable->GetMtAlias(byLockedMcuId, 
                        mtAliasTypeTransportAddress,
                        &tMtAlias);
                    
                    cServMsg.SetMsgBody( (u8*)&tMtAlias.m_tTransportAddr.m_dwIpAddr, sizeof(u32) );
                    if(!m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable->GetMtAlias(byLockedMcuId, 
                        mtAliasTypeH323ID,
                        &tMtAlias))
                    {
                        if(!m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable->GetMtAlias(byLockedMcuId, 
                            mtAliasTypeE164,
                            &tMtAlias))
                        {
                            strncpy( tMtAlias.m_achAlias, "mmcu", sizeof(tMtAlias.m_achAlias) );
                        }
                    }
                    tMtAlias.m_achAlias[MAXLEN_PWD-1] = 0;
                    cServMsg.CatMsgBody( (u8*)tMtAlias.m_achAlias, MAXLEN_PWD );										
				}
				else if( tProtectInfo.IsLockByMcs() && tProtectInfo.GetLockedMcSsnId() != bySrcSsnId )
                {
                    TMcsRegInfo tMcsRegInfo;
                    GetMcsRegInfo( tProtectInfo.GetLockedMcSsnId(), &tMcsRegInfo );
                    cServMsg.SetMsgBody( (u8*)&tMcsRegInfo.m_dwMcsIpAddr, sizeof(u32) );
                    cServMsg.CatMsgBody( (u8*)tMcsRegInfo.m_achUser, MAXLEN_PWD );
				}

				CMcsSsn::SendMsgToMcsSsn( bySrcSsnId, MCU_MCS_LOCKUSERINFO_NOTIFY, cServMsg.GetServMsg(), 
					            cServMsg.GetServMsgLen() );				
			}            
		}
        break;

	//��Ҫ�жϲ��п���Ҫ��Ӧ����Ϣ
	case MCS_MCU_RELEASECONF_REQ:           //�������̨����MCU����һ������
	case MCS_MCU_CHANGECONFPWD_REQ:         //�������̨����MCU���Ļ�������
	case MCS_MCU_SAVECONF_REQ:              //�������̨����MCU�������
	case MCS_MCU_MODIFYCONF_REQ:	        //����޸�ԤԼ����
	case MCS_MCU_DELAYCONF_REQ:             //�������̨����MCU�ӳ�����
	case MCS_MCU_CHANGEVACHOLDTIME_REQ:		//�������̨����MCU�ı����������л�ʱ��
	case MCS_MCU_CHANGECONFLOCKMODE_REQ:    //�������̨����MCU��������
	case MCS_MCU_SPECCHAIRMAN_REQ:          //�������ָ̨��һ̨�ն�Ϊ��ϯ
	case MCS_MCU_CANCELCHAIRMAN_REQ:        //�������̨ȡ����ǰ������ϯ
	case MCS_MCU_SPECSPEAKER_REQ:           //�������ָ̨��һ̨�ն˷���
	case MCS_MCU_CANCELSPEAKER_REQ:         //�������̨ȡ������Speaker
    case MCS_MCU_SPECOUTVIEW_REQ:           //�������ָ̨���ش�ͨ��
	case MCS_MCU_ADDMT_REQ:                 //�������̨�����ն����	
	case MCS_MCU_DELMT_REQ:                 //�������̨�����ն����
	case MCS_MCU_STARTSWITCHMC_REQ:         //���ѡ���ն�
	case MCS_MCU_STARTVAC_REQ:              //�������̨����MCU��ʼ�����������Ʒ���		
	case MCS_MCU_STOPVAC_REQ:               //�������̨����MCUֹͣ�����������Ʒ���
	case MCS_MCU_STARTDISCUSS_REQ:          //��ʼ������������ - ���ڱ�����ʼ���۲���
	case MCS_MCU_STOPDISCUSS_REQ:           //���������������� - ���ڱ�����ʼ���۲���

    //�������---����
    case MCS_MCU_STARTROLLCALL_REQ:
    case MCS_MCU_STOPROLLCALL_REQ:
    case MCS_MCU_CHANGEROLLCALL_REQ:

    //vmp
	case MCS_MCU_STARTVMP_REQ:              //��ʼ��Ƶ��������
	case MCS_MCU_STOPVMP_REQ:               //������Ƶ��������
	case MCS_MCU_CHANGEVMPPARAM_REQ:        //�������̨����MCU�ı���Ƶ���ϲ���
    //vmptw
	case MCS_MCU_STARTVMPTW_REQ:            //��ʼ���ϵ���ǽ����
	case MCS_MCU_STOPVMPTW_REQ:             //�������ϵ���ǽ����
	case MCS_MCU_CHANGEVMPTWPARAM_REQ:      //�������̨����MCU�ı临�ϵ���ǽ����
    //
	case MCS_MCU_CALLMT_REQ:                //�������̨����MCU�����ն�
	case MCS_MCU_SETCALLMTMODE_REQ:         //�������̨����MCU�����ն˷�ʽ
	case MCS_MCU_DROPMT_REQ:                //�������̨�Ҷ��ն�����
	case MCS_MCU_STARTSWITCHMT_REQ:         //���ǿ��Ŀ���ն�ѡ��Դ�ն�
	case MCS_MCU_STOPSWITCHMT_REQ:          //���ȡ��Ŀ���ն�ѡ��Դ�ն�
	case MCS_MCU_SETMTVIDSRC_CMD:           //���Ҫ��MCU�����ն���ƵԴ
	case MCS_MCU_MTAUDMUTE_REQ:             //���Ҫ��MCU�����ն˾���
	case MCS_MCU_MATRIX_SAVESCHEME_CMD:     //������󷽰�����        
    case MCS_MCU_MATRIX_GETALLSCHEMES_CMD:  //�ն����þ������
    case MCS_MCU_MATRIX_GETONESCHEME_CMD:
    case MCS_MCU_MATRIX_SETCURSCHEME_CMD:
    case MCS_MCU_MATRIX_GETCURSCHEME_CMD:
    case MCS_MCU_EXMATRIX_GETINFO_CMD:      //��ȡ�ն����þ�������
    case MCS_MCU_EXMATRIX_SETPORT_CMD:      //�������þ������Ӷ˿ں�
    case MCS_MCU_EXMATRIX_GETPORT_REQ:      //�����ȡ���þ������Ӷ˿�
    case MCS_MCU_EXMATRIX_SETPORTNAME_CMD:  //�������þ������Ӷ˿���
    case MCS_MCU_EXMATRIX_GETALLPORTNAME_CMD://�����ȡ���þ�������ж˿���        
    case MCS_MCU_GETVIDEOSOURCEINFO_CMD://�ն���չ��ƵԴ
    case MCS_MCU_SETVIDEOSOURCEINFO_CMD:
    case MCS_MCU_SELECTEXVIDEOSRC_CMD:
	case MCS_MCU_STARTREC_REQ:              //�����MCU����¼��
	case MCS_MCU_PAUSEREC_REQ:              //�����MCU��ͣ¼��
	case MCS_MCU_RESUMEREC_REQ:             //�����MCU�ָ�¼��
	case MCS_MCU_STOPREC_REQ:               //�����MCUֹͣ¼��
	case MCS_MCU_STARTPLAY_REQ:             //��ؿ�ʼ��������
	case MCS_MCU_PAUSEPLAY_REQ:             //�����ͣ��������
	case MCS_MCU_RESUMEPLAY_REQ:            //��ػָ���������
	case MCS_MCU_STOPPLAY_REQ:              //���ֹͣ��������
	case MCS_MCU_FFPLAY_REQ:                //��ؿ����������
	case MCS_MCU_FBPLAY_REQ:                //��ؿ��˷�������
	case MCS_MCU_SEEK_REQ:                  //��ص��������������
    case MCS_MCU_GETRECPROG_CMD:            //��ز�ѯ¼�����
    case MCS_MCU_GETPLAYPROG_CMD:           //��ز�ѯ�������
	case MCS_MCU_START_SWITCH_TW_REQ:       //�������̨����ָ��Mt��ͼ�񽻻���ָ�������ָ��������ͨ����
	case MCS_MCU_STARTVMPBRDST_REQ:         //�������̨����MCU��ʼ�ѻ���ϳ�ͼ��㲥���ն�
	case MCS_MCU_STOPVMPBRDST_REQ:          //�������̨����MCUֹͣ�ѻ���ϳ�ͼ��㲥���ն�
	case MCS_MCU_LOCKSMCU_REQ:		
    case MCS_MCU_CHANGEMIXDELAY_REQ:
	case MCS_MCU_START_VMPBATCHPOLL_REQ:
	case MCS_MCU_PAUSE_VMPBATCHPOLL_CMD:
	case MCS_MCU_STOP_VMPBATCHPOLL_CMD:
	case MCS_MCU_STARTSWITCHVMPMT_REQ:
	case MCS_MCU_START_SWITCH_HDU_REQ:      //4.6.1  �¼Ӱ汾 jlb
		cConfId = cServMsg.GetConfId();
        byConfIdx = GetConfIdx( cConfId );
        if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
        {
            OspPrintf(TRUE, FALSE, "[MsgPassCheck] invalid cConfId :");
            cConfId.Print();
            return FALSE;
        }
        tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
        if(!tMapData.IsValidConf())
        {
            OspPrintf(TRUE, FALSE, "[MsgPassCheck] invalid instance id :%d\n", tMapData.GetInsId());
            return FALSE;
        }
	    GetConfProtectInfo( byConfIdx, &tProtectInfo );
        bySrcSsnId = cServMsg.GetSrcSsnId();
		byConfProtectMode = m_apConfInst[tMapData.GetInsId()-1]->m_tConf.m_tStatus.GetProtectMode();
		if( ( byConfProtectMode == CONF_LOCKMODE_LOCK && 
			  ( tProtectInfo.IsLockByMcs() && tProtectInfo.GetLockedMcSsnId() != bySrcSsnId || tProtectInfo.IsLockByMcu() ) 
             ) || 
		    ( byConfProtectMode == CONF_LOCKMODE_NEEDPWD && 
			  !tProtectInfo.IsMcsPwdPassed(bySrcSsnId)
            ) 
           )
        { 
            bReturn = FALSE;
			if( byConfProtectMode == CONF_LOCKMODE_NEEDPWD )
			{
				//������
				cServMsg.SetMsgBody();
                CMcsSsn::SendMsgToMcsSsn( bySrcSsnId, MCU_MCS_ENTERCONFPWD_REQ, 
					                          cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			}
			else
			{
				//Nack, ����������
                cServMsg.SetErrorCode( ERR_MCU_CTRLBYOTHER );
				CMcsSsn::SendMsgToMcsSsn( bySrcSsnId, wEvent+2, cServMsg.GetServMsg(), 
					                          cServMsg.GetServMsgLen() );

				 if (tProtectInfo.IsLockByMcu())
                 {
					TMtAlias tMtAlias;
                    u8 byLockedMcuId = tProtectInfo.GetLockedMcuId();
					m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable->GetMtAlias(byLockedMcuId, 
																		mtAliasTypeTransportAddress,
																		&tMtAlias);

					cServMsg.SetMsgBody( (u8*)&tMtAlias.m_tTransportAddr.m_dwIpAddr, sizeof(u32) );
					if(!m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable->GetMtAlias(byLockedMcuId, 
																			mtAliasTypeH323ID,
																			&tMtAlias))
					{
						if(!m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable->GetMtAlias(byLockedMcuId, 
																				mtAliasTypeE164,
																				&tMtAlias))
						{
							strncpy( tMtAlias.m_achAlias, "mmcu", sizeof(tMtAlias.m_achAlias) );
						}
					}
					tMtAlias.m_achAlias[MAXLEN_PWD-1] = 0;
					cServMsg.CatMsgBody( (u8*)tMtAlias.m_achAlias, MAXLEN_PWD );										
                 }
                 else if(tProtectInfo.IsLockByMcs() && tProtectInfo.GetLockedMcSsnId() != bySrcSsnId )
                 {
                     TMcsRegInfo tMcsRegInfo;
                     GetMcsRegInfo( tProtectInfo.GetLockedMcSsnId(), &tMcsRegInfo );
                     cServMsg.SetMsgBody( (u8*)&tMcsRegInfo.m_dwMcsIpAddr, sizeof(u32) );
                     cServMsg.CatMsgBody( (u8*)tMcsRegInfo.m_achUser, MAXLEN_PWD );
                 }

				CMcsSsn::SendMsgToMcsSsn( bySrcSsnId, MCU_MCS_LOCKUSERINFO_NOTIFY, cServMsg.GetServMsg(), 
					                          cServMsg.GetServMsgLen() );
			}

		}
	 break;

	default:
		break;
	}
	return bReturn;
}

/*====================================================================
    ������      ��BroadcastToAllConf
    ����        ������Ϣ�����л���ʵ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ�룬ȱʡΪNULL
				  u16 wLen, ��Ϣ���ȣ�ȱʡΪ0
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/30    1.0         LI Yi         ����
	05/12/20	4.0			�ű���		  ����T120��Ϣ
====================================================================*/
void CMcuVcData::BroadcastToAllConf( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//�������߻��߱�����δ��ȫͬ������(MTADP/MP/MC/���� ������Ϣ����)��
	//����������ҵ��ģ��Ͷ����Ϣ
	if ( MTADP_MCU_REGISTER_REQ != wEvent && 
		 MCU_MTADP_DISCONNECTED_NOTIFY != wEvent && 
		 MP_MCU_REG_REQ != wEvent && 
		 MCU_MP_DISCONNECTED_NOTIFY != wEvent && 
		 MCU_MCSCONNECTED_NOTIF != wEvent && 
		 MCU_MCSDISCONNECTED_NOTIF != wEvent && 
	 	 MCU_EQPCONNECTED_NOTIF != wEvent && 
 		 MCU_EQPDISCONNECTED_NOTIF != wEvent && 
		 MCU_RECCONNECTED_NOTIF != wEvent && 
		 MCU_RECDISCONNECTED_NOTIF != wEvent && 
		 MCU_BASCONNECTED_NOTIF != wEvent && 
		 MCU_BASDISCONNECTED_NOTIF != wEvent && 
		 MCU_MIXERCONNECTED_NOTIF != wEvent && 
		 MCU_MIXERDISCONNECTED_NOTIF != wEvent && 
		 MCU_VMPCONNECTED_NOTIF != wEvent && 
		 MCU_VMPDISCONNECTED_NOTIF != wEvent && 
		 MCU_VMPTWCONNECTED_NOTIF != wEvent && 
		 MCU_VMPTWDISCONNECTED_NOTIF != wEvent && 
		 MCU_PRSCONNECTED_NOTIF != wEvent && 
		 MCU_PRSDISCONNECTED_NOTIF != wEvent && 
		 MCU_TVWALLCONNECTED_NOTIF != wEvent && 
		 MCU_TVWALLDISCONNECTED_NOTIF != wEvent && 
		 MCU_DCSCONNCETED_NOTIF != wEvent &&
		 MCU_DCSDISCONNECTED_NOTIF != wEvent &&
		 MCU_HDUCONNECTED_NOTIF != wEvent &&         //4.6 �¼�  jlb
		 MCU_HDUDISCONNECTED_NOTIF!= wEvent &&      //4.6 �¼�  jlb
		 FALSE == g_cMSSsnApp.JudgeRcvMsgPass() )
	{
		return;
	}

	if (MsgPassCheck(wEvent, pbyMsg, wLen))
	{
		if (0 != ::OspPost(MAKEIID(AID_MCU_VC, CInstance::EACH), wEvent, pbyMsg, wLen))
		{
			OspPrintf(TRUE, FALSE, "[BroadcastToAllConf] msg.%d<%s> broad failed!\n", wEvent, OspEventDesc(wEvent));
		}
	}

	return;
}

/*====================================================================
    ������      ��SendMsgToConf
    ����        ������Ϣ��ָ���Ļ����Ӧ�Ļ���ʵ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx, ����������
				  u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ�룬ȱʡΪNULL
				  u16 wLen, ��Ϣ���ȣ�ȱʡΪ0
    ����ֵ˵��  ���ɹ�����TRUE�����޸û��鷵��FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/28    1.0         LI Yi         ����
	05/12/20	4.0			�ű���		  ����T120��Ϣ
====================================================================*/
BOOL32 CMcuVcData::SendMsgToConf( u8 byConfIdx, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//�������߻��߱�����δ��ȫͬ������(MTADP/MP/MC/���� ������Ϣ����)��
	//����������ҵ��ģ��Ͷ����Ϣ
	if (MTADP_MCU_REGISTER_REQ != wEvent && 
		MCU_MTADP_DISCONNECTED_NOTIFY != wEvent && 
		MP_MCU_REG_REQ != wEvent && 
		MCU_MP_DISCONNECTED_NOTIFY != wEvent && 
		MCU_MCSCONNECTED_NOTIF != wEvent && 
		MCU_MCSDISCONNECTED_NOTIF != wEvent && 
		MCU_EQPCONNECTED_NOTIF != wEvent && 
		MCU_EQPDISCONNECTED_NOTIF != wEvent && 
		MCU_RECCONNECTED_NOTIF != wEvent && 
		MCU_RECDISCONNECTED_NOTIF != wEvent && 
		MCU_BASCONNECTED_NOTIF != wEvent && 
		MCU_BASDISCONNECTED_NOTIF != wEvent && 
		MCU_MIXERCONNECTED_NOTIF != wEvent && 
		MCU_MIXERDISCONNECTED_NOTIF != wEvent && 
		MCU_VMPCONNECTED_NOTIF != wEvent && 
		MCU_VMPDISCONNECTED_NOTIF != wEvent && 
		MCU_VMPTWCONNECTED_NOTIF != wEvent && 
		MCU_VMPTWDISCONNECTED_NOTIF != wEvent && 
		MCU_PRSCONNECTED_NOTIF != wEvent && 
		MCU_PRSDISCONNECTED_NOTIF != wEvent && 
		MCU_TVWALLCONNECTED_NOTIF != wEvent && 
		MCU_TVWALLDISCONNECTED_NOTIF != wEvent && 
		MCU_DCSCONNCETED_NOTIF != wEvent &&
		MCU_DCSDISCONNECTED_NOTIF != wEvent &&
		MCU_HDUCONNECTED_NOTIF != wEvent &&          //4.6  �¼�  jlb 
		MCU_HDUDISCONNECTED_NOTIF != wEvent &&
		FALSE == g_cMSSsnApp.JudgeRcvMsgPass())
	{
		return TRUE;
	}
	
	if (0 != byConfIdx)
	{
		if (MsgPassCheck(wEvent, pbyMsg, wLen))
		{		
		    ::OspPost(MAKEIID(AID_MCU_VC, GetConfMapInsId(byConfIdx)), wEvent, pbyMsg, wLen);
		}
		return TRUE;
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[SendMsgToConf] ConfIdx is 0! event:%u(%s)\n", wEvent, OspEventDesc(wEvent));
	}
	
    return FALSE;
}

/*====================================================================
    ������      ��SendMsgToConf
    ����        ������Ϣ��ָ���Ļ����Ӧ�Ļ���ʵ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CConfId & cConfId, ����ţ�0��ʾ����IDLEʵ��
				  u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ�룬ȱʡΪNULL
				  u16 wLen, ��Ϣ���ȣ�ȱʡΪ0
    ����ֵ˵��  ���ɹ�����TRUE�����޸û��鷵��FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/28    1.0         LI Yi         ����
	05/12/20	4.0			�ű���		  ����T120��Ϣ
====================================================================*/
BOOL32 CMcuVcData::SendMsgToConf( const CConfId & cConfId, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//�������߻��߱�����δ��ȫͬ������(MTADP/MP/MC/���� ������Ϣ����)��
	//����������ҵ��ģ��Ͷ����Ϣ
	if (MTADP_MCU_REGISTER_REQ != wEvent && 
		MCU_MTADP_DISCONNECTED_NOTIFY != wEvent && 
		MP_MCU_REG_REQ != wEvent && 
		MCU_MP_DISCONNECTED_NOTIFY != wEvent && 
		MCU_MCSCONNECTED_NOTIF != wEvent && 
		MCU_MCSDISCONNECTED_NOTIF != wEvent && 
		MCU_EQPCONNECTED_NOTIF != wEvent && 
		MCU_EQPDISCONNECTED_NOTIF != wEvent && 
		MCU_RECCONNECTED_NOTIF != wEvent && 
		MCU_RECDISCONNECTED_NOTIF != wEvent && 
		MCU_BASCONNECTED_NOTIF != wEvent && 
		MCU_BASDISCONNECTED_NOTIF != wEvent && 
		MCU_MIXERCONNECTED_NOTIF != wEvent && 
		MCU_MIXERDISCONNECTED_NOTIF != wEvent && 
		MCU_VMPCONNECTED_NOTIF != wEvent && 
		MCU_VMPDISCONNECTED_NOTIF != wEvent && 
		MCU_VMPTWCONNECTED_NOTIF != wEvent && 
		MCU_VMPTWDISCONNECTED_NOTIF != wEvent && 
		MCU_PRSCONNECTED_NOTIF != wEvent && 
		MCU_PRSDISCONNECTED_NOTIF != wEvent && 
		MCU_TVWALLCONNECTED_NOTIF != wEvent && 
		MCU_TVWALLDISCONNECTED_NOTIF != wEvent && 
		MCU_DCSCONNCETED_NOTIF != wEvent &&
		MCU_DCSDISCONNECTED_NOTIF != wEvent &&
		MCU_HDUCONNECTED_NOTIF != wEvent && 
		MCU_HDUDISCONNECTED_NOTIF != wEvent && 
		FALSE == g_cMSSsnApp.JudgeRcvMsgPass())
	{
		return TRUE;
	}

	if (cConfId.IsNull())
	{
//		if (MsgPassCheck(wEvent, pbyMsg, wLen))
//		{		
//		    ::OspPost(MAKEIID(AID_MCU_VC, CInstance::PENDING), wEvent, pbyMsg, wLen);
//		}

        OspPrintf(TRUE, FALSE, "[SendMsgToConf] ConfId is Null! event:%u(%s)\n", wEvent, OspEventDesc(wEvent));

		return TRUE;
	}
    
    if (MsgPassCheck(wEvent, pbyMsg, wLen))
	{
		u8 byConfIdx = GetConfIdx(cConfId);
		if (0 != byConfIdx)
		{
            u8 byInsId = GetConfMapInsId(byConfIdx);
            if (0 == byInsId || byInsId > MAXNUM_MCU_CONF)
            {
                OspPrintf(TRUE, FALSE, "[SendMsgToConf] Send msg to conf failure, invalid confidx %d!\n", byConfIdx);
            }
			else
			{
                if (OSP_OK != ::OspPost( MAKEIID( AID_MCU_VC, byInsId), wEvent, pbyMsg, wLen ))
                {
                    OspPrintf(TRUE, FALSE, "[SendMsgToConf] Send msg to conf by confIdx failure!\n");
                }
			}
		}
        else
        {
            s8 achBuf[MAXLEN_CONFID*2+1] = {0};
            cConfId.GetConfIdString( achBuf, sizeof(achBuf) );
            OspPrintf(TRUE, FALSE, "[SendMsgToConf] cConfId %s invalid, Event = %s(%d)!\n", 
                      achBuf, 
                      OspEventDesc(wEvent),
                      wEvent);            
        }
	}
	
    return TRUE;
}

/*====================================================================
    ������      ��SendMsgToDaemonConf
    ����        ������Ϣ��ҵ��Daemonʵ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ�룬ȱʡΪNULL
				  u16 wLen, ��Ϣ���ȣ�ȱʡΪ0
    ����ֵ˵��  ���ɹ�����TRUE�����޸û��鷵��FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/05/30    1.0         JQL           ����
	05/12/20	4.0			�ű���		  ����T120��Ϣ
====================================================================*/
BOOL32 CMcuVcData::SendMsgToDaemonConf( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//�������߻��߱�����δ��ȫͬ������(MTADP/MP/MC/���� ������Ϣ����)��
	//����������ҵ��ģ��Ͷ����Ϣ
	if (MTADP_MCU_REGISTER_REQ != wEvent && 
		MCU_MTADP_DISCONNECTED_NOTIFY != wEvent && 
		MP_MCU_REG_REQ != wEvent && 
		MCU_MP_DISCONNECTED_NOTIFY != wEvent && 
		MCU_MCSCONNECTED_NOTIF != wEvent && 
		MCU_MCSDISCONNECTED_NOTIF != wEvent && 
		MCU_EQPCONNECTED_NOTIF != wEvent && 
		MCU_EQPDISCONNECTED_NOTIF != wEvent && 
		MCU_RECCONNECTED_NOTIF != wEvent && 
		MCU_RECDISCONNECTED_NOTIF != wEvent && 
		MCU_BASCONNECTED_NOTIF != wEvent && 
		MCU_BASDISCONNECTED_NOTIF != wEvent && 
		MCU_MIXERCONNECTED_NOTIF != wEvent && 
		MCU_MIXERDISCONNECTED_NOTIF != wEvent && 
		MCU_VMPCONNECTED_NOTIF != wEvent && 
		MCU_VMPDISCONNECTED_NOTIF != wEvent && 
		MCU_VMPTWCONNECTED_NOTIF != wEvent && 
		MCU_VMPTWDISCONNECTED_NOTIF != wEvent && 
		MCU_PRSCONNECTED_NOTIF != wEvent && 
		MCU_PRSDISCONNECTED_NOTIF != wEvent && 
		MCU_TVWALLCONNECTED_NOTIF != wEvent && 
		MCU_TVWALLDISCONNECTED_NOTIF != wEvent && 
		MCU_DCSCONNCETED_NOTIF != wEvent &&
		MCU_DCSDISCONNECTED_NOTIF != wEvent &&
		MCU_HDUCONNECTED_NOTIF != wEvent && 
		MCU_HDUDISCONNECTED_NOTIF != wEvent && 
		FALSE == g_cMSSsnApp.JudgeRcvMsgPass())
	{
		return TRUE;
	}

    if (MsgPassCheck( wEvent, pbyMsg, wLen ))
	{
        ::OspPost(MAKEIID( AID_MCU_VC, CInstance::DAEMON ), wEvent, pbyMsg, wLen);
	}

    return TRUE;
}

/*====================================================================
    ������      ��RefreshHtml
    ����        ��ˢ��Htmlҳ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/04/19    3.0         ������        �޸�
    06/04/19    4.0         ����        NMS1д�ļ�Ҫ�����������ַ
    08/06/27    5.0         ����          WriteHtmlҪ��tStreamChannel����AAC�������
====================================================================*/
void CMcuVcData::RefreshHtml( void )
{
	TStreamChannel tStreamChannel[64];
	memset( tStreamChannel, 0, sizeof(tStreamChannel) );
	u8  byChlNum = 0;
	TCapSupport tCapSupport;
    TMediaEncrypt tMediaEncrypt;
    u16 wMultiCastPort;

	for( u8 byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++ )
	{
		if( m_apConfInst[byLoop] != NULL && 
			m_apConfInst[byLoop]->m_tConf.GetConfAttrb().IsMulticastMode() && 
			m_apConfInst[byLoop]->m_tConf.m_tStatus.IsOngoing() )
		{
            tMediaEncrypt = m_apConfInst[byLoop]->m_tConf.GetMediaKey();
			tCapSupport = m_apConfInst[byLoop]->m_tConf.GetCapSupport();

			strncpy( tStreamChannel[byChlNum].m_aszChannelName, m_apConfInst[byLoop]->m_tConf.GetConfName(), 254 );
			strncpy( tStreamChannel[byChlNum].m_aszPassWord, m_apConfInst[byLoop]->m_tConf.GetConfPwd(), 32 );
            
            // �����������ַ
			tStreamChannel[byChlNum].m_dwIp  = AssignMulticastIp(m_apConfInst[byLoop]->m_byConfIdx);
            wMultiCastPort = AssignMulticastPort( (u8)m_apConfInst[byLoop]->m_byConfIdx, 0 );
            tStreamChannel[byChlNum].m_wPort[0] = wMultiCastPort;

            //���ü�����Կ
            s32 nTmpLen;
            //��Ƶ
            tStreamChannel[byChlNum].m_tEncrypt[0].m_tEncryptKey[0].m_byMode = tMediaEncrypt.GetEncryptMode();
            tMediaEncrypt.GetEncryptKey(tStreamChannel[byChlNum].m_tEncrypt[0].m_tEncryptKey[0].m_abyKey, &nTmpLen);
            tStreamChannel[byChlNum].m_tEncrypt[0].m_tEncryptKey[0].m_byKeyLen = (u8)nTmpLen;

            //��Ƶ
            tStreamChannel[byChlNum].m_tEncrypt[0].m_tEncryptKey[1].m_byMode = tMediaEncrypt.GetEncryptMode();
            tMediaEncrypt.GetEncryptKey(tStreamChannel[byChlNum].m_tEncrypt[0].m_tEncryptKey[1].m_abyKey, &nTmpLen);
            tStreamChannel[byChlNum].m_tEncrypt[0].m_tEncryptKey[1].m_byKeyLen = (u8)nTmpLen;
            
            //Ĭ��д��˫����Ϣ
            tStreamChannel[byChlNum].m_wPort[1] = wMultiCastPort + 4;
            tStreamChannel[byChlNum].m_dwStreamNum = 2; 
            //��Ƶ
            tStreamChannel[byChlNum].m_tEncrypt[1].m_tEncryptKey[0].m_byMode = tMediaEncrypt.GetEncryptMode();
            tMediaEncrypt.GetEncryptKey(tStreamChannel[byChlNum].m_tEncrypt[1].m_tEncryptKey[0].m_abyKey, &nTmpLen);
            tStreamChannel[byChlNum].m_tEncrypt[1].m_tEncryptKey[0].m_byKeyLen = (u8)nTmpLen;

            //˫��û����Ƶ������������

			//��һ·��ƵΪH264ʱ���ڶ�̬�غ�֧�֣�Ŀǰ�ݲ�֧�ּ�����ý��
			if( MEDIA_TYPE_H264 == tCapSupport.GetMainVideoType() || 
				( !tCapSupport.GetSecondSimCapSet().IsNull() && 
				  MEDIA_TYPE_H264 == tCapSupport.GetSecVideoType() ) )
			{
                tStreamChannel[byChlNum].m_tEncrypt[0].m_byRealVideoPT = MEDIA_TYPE_H264;
				tStreamChannel[byChlNum].m_tEncrypt[0].m_byVideoEncType = 
                    GetActivePayload(m_apConfInst[byLoop]->m_tConf, MEDIA_TYPE_H264);
			}
			else
			{
				tStreamChannel[byChlNum].m_tEncrypt[0].m_byRealVideoPT  = tCapSupport.GetMainVideoType();
				tStreamChannel[byChlNum].m_tEncrypt[0].m_byVideoEncType = tCapSupport.GetMainVideoType();
			}
			tStreamChannel[byChlNum].m_tEncrypt[0].m_byRealAudioPT  = tCapSupport.GetMainAudioType();
			tStreamChannel[byChlNum].m_tEncrypt[0].m_byAudioEncType = tCapSupport.GetMainAudioType();

            //�ڶ�·��Ƶ
            tStreamChannel[byChlNum].m_tEncrypt[1].m_byRealVideoPT  = tCapSupport.GetDStreamMediaType();
            tStreamChannel[byChlNum].m_tEncrypt[1].m_byVideoEncType =
                GetActivePayload(m_apConfInst[byLoop]->m_tConf, tCapSupport.GetDStreamMediaType());

            tStreamChannel[byChlNum].m_tEncrypt[1].m_byRealAudioPT  = MEDIA_TYPE_NULL;
			tStreamChannel[byChlNum].m_tEncrypt[1].m_byAudioEncType = MEDIA_TYPE_NULL;

            // zw [06/27/2008] �����AAC���飬���AAC��Ҫ������
            if ( MEDIA_TYPE_AACLC == m_apConfInst[byLoop]->m_tConf.GetMainAudioMediaType() )
            {
                tStreamChannel[byChlNum].m_tEncrypt[0].m_byAACSamplePerSecond = AAC_SAMPLE_FRQ_32;
                tStreamChannel[byChlNum].m_tEncrypt[0].m_byAACChannels        = AAC_CHNL_TYPE_SINGLE; 
            }

			byChlNum++;
		}
	}

	s8	   achWebFileName[KDV_MAX_PATH];
	sprintf( achWebFileName, "%s/%s", DIR_WEB, MCUHTMLFILENAME );
    McsLog("[RefreshHtml] Writing VOD Channel info(%d chnl) into file %s.\n", byChlNum, achWebFileName);
	WriteHtml( achWebFileName, tStreamChannel, byChlNum );

	return;
}

/*====================================================================
    ������      ��AddConf
    ����        ��������������һ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TConfFullInfo	*ptConf, ����������Ϣ
	              BOOL32 bRefreshHtml =TRUE �Ƿ�ˢ����ý���ļ�
    ����ֵ˵��  ���ɹ�����TRUE��ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/30    1.0         LI Yi         ����
	03/05/29	1.2			Jiaql         �޸�
	03/11/08    3.0         ������        �޸�
====================================================================*/
BOOL32 CMcuVcData::AddConf( CMcuVcInst *pConfInst, BOOL32 bRefreshHtml /*=TRUE*/ )
{
	if( pConfInst->GetInsID() > MAXNUM_MCU_CONF )
	{
		return FALSE;
	}

    m_apConfInst[(u8)pConfInst->GetInsID()-1] = pConfInst;

	if (bRefreshHtml)
	{
		RefreshHtml();
	}

	return TRUE;
}

/*====================================================================
    ������      ��GetConfInstHandle
    ����        �����ݻ���������� ��ѯ����ʵ��ָ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx, �����������
    ����ֵ˵��  ���ɹ�����TRUE��ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/30    1.0         LI Yi         ����
	03/11/08    3.0         ������        �޸�
====================================================================*/
CMcuVcInst *CMcuVcData::GetConfInstHandle( u8 byConfIdx )
{
	if (byConfIdx > MAX_CONFIDX || byConfIdx < MIN_CONFIDX)
	{
		return NULL;
	}
	
    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if (!tMapData.IsValidConf() || NULL == m_apConfInst[tMapData.GetInsId()-1])
    {
        return NULL;
    }
    
	return (CMcuVcInst *)m_apConfInst[tMapData.GetInsId()-1];

}

/*====================================================================
    ������      ��RemoveConf
    ����        ����������ɾ��һ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx, �����������
	              BOOL32 bRefreshHtml =TRUE �Ƿ�ˢ����ý���ļ�
    ����ֵ˵��  ���ɹ�����TRUE��ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/30    1.0         LI Yi         ����
	03/11/08    3.0         ������        �޸�
====================================================================*/
BOOL32 CMcuVcData::RemoveConf( u8 byConfIdx, BOOL32 bRefreshHtml /*=TRUE*/ )
{
    CHECK_CONFIDX(byConfIdx)	
	
    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if(!tMapData.IsValidConf())
    {
        return FALSE;
    }
    m_apConfInst[tMapData.GetInsId()-1] = NULL;

	if (bRefreshHtml)
	{
		RefreshHtml();
	}

	return TRUE;
}

/*====================================================================
    ������      ��GetConfIdx
    ����        ����û�����ڻ�����е����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CConfId& cConfId �����    
    ����ֵ˵��  ��0 û���ҵ�����Ŷ�Ӧ����ţ���ʾ���鲻����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/05/24    1.0         LI Yi         ����
	03/11/08    3.0         ������        �޸�
====================================================================*/
u8 CMcuVcData::GetConfIdx( const CConfId& cConfId ) const
{	
	u8 byLoop;	    
    for( byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++ )
    {
        if( m_apConfInst[byLoop] != NULL &&
            m_apConfInst[byLoop]->m_tConf.GetConfId() == cConfId )
        {
            return m_apConfInst[byLoop]->m_byConfIdx;
        }
    }

    for( byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++ )
    {
        if (!m_ptTemplateInfo[byLoop].IsEmpty() &&
            m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfId() == cConfId)
        {
            return m_ptTemplateInfo[byLoop].m_byConfIdx;
        }
    }

   return 0;
}

/*====================================================================
    ������      ��GetConfId
    ����        �������ڻ�����е���Ż�û����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/05/24    1.0         LI Yi         ����
	03/11/08    3.0         ������        �޸�
====================================================================*/
CConfId CMcuVcData::GetConfId(const u8 &byConfIdx) const
{
    CConfId cConfId;
    cConfId.SetNull();

    u8 byLoop;	    
    for( byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++ )
    {
        if( m_apConfInst[byLoop] != NULL &&
            m_apConfInst[byLoop]->m_byConfIdx == byConfIdx )
        {
            return m_apConfInst[byLoop]->m_tConf.GetConfId();
        }
    }
    return cConfId;
}

/*====================================================================
    ������      ��MakeConfId
    ����        �����ݱ�MCU��/��������/��������ֵ/����Ų������� ���������
                  ֧���û�����Ϣ
				  ֧�ֻ��鴴ʼ����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  �������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/01    1.0         LI Yi         ����
    06/06/21    4.0         ����        ֧���û�����Ϣ
====================================================================*/
CConfId CMcuVcData::MakeConfId(u8 byConfIdx, u8 byTemplate, u8 byUsrGrpId, u8 byConfSource /*= MCS_CONF*/ )
{
	CConfId	cConfId;
	u8	abyTConfId[8];
	u16	wTemp;

	//����Ų������������ۼ�,��֤ǰ������Ļ���ŵ�Ψһ��
	m_dwMakeConfIdTimes++;

	//2 bytes
	wTemp = htons( LOCAL_MCUID );
	memcpy( abyTConfId, &wTemp, 2 );
	//2 bytes
	abyTConfId[2] = byTemplate;
	abyTConfId[3] = byConfIdx;
	//4 bytes
    u32 dwTimes = htonl(m_dwMakeConfIdTimes);
	memcpy(abyTConfId+4, &dwTimes, sizeof(dwTimes));
	
	cConfId.SetConfId(abyTConfId, sizeof(abyTConfId));
    cConfId.SetUsrGrpId( byUsrGrpId );
	cConfId.SetConfSource( byConfSource );

	return( cConfId );
}

/*====================================================================
    ������      ��GetMakeTimesFromConfId
    ����        ����CConfIdȡ���Ѿ������˶��ٴε�MakeConfId
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/04/10    4.0         ����          ����
====================================================================*/
u32 CMcuVcData::GetMakeTimesFromConfId(const CConfId& cConfId) const
{
    u32 dwTimes;
    u8	abyConfId[sizeof(CConfId)];
    cConfId.GetConfId( abyConfId, sizeof(abyConfId) );
    memcpy(&dwTimes, abyConfId+4, sizeof(u32));
    dwTimes = ntohl(dwTimes);
    return dwTimes;
}

/*====================================================================
    ������      ��IsConfNameRepeat
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPCSTR lpszConfName �µĻ�����    
    ����ֵ˵��  ��TRUE-�������ظ� FALSE-���ظ�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/08    3.0         ������        ����
====================================================================*/
BOOL32 CMcuVcData::IsConfNameRepeat( LPCSTR lpszConfName, BOOL32 IsTemple )
{
	u8 byLoop;
	
    if(IsTemple)
    {
        for(byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
        {
            if(!m_ptTemplateInfo[byLoop].IsEmpty() && 
                0 == memcmp(m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfName(), lpszConfName, MAXLEN_CONFNAME ))
            {
                return TRUE;
            }
        }
    }
    else
    {
        for( byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++ )
        {
            if( m_apConfInst[byLoop] != NULL )
            {
                if (0 == memcmp( m_apConfInst[byLoop]->m_tConf.GetConfName(), lpszConfName, MAXLEN_CONFNAME ))                    
                {
                    return TRUE;
                }
            }
        }
    }	

	return FALSE;
}

/*====================================================================
    ������      ��IsConfNameRepeat
    ����        ���Ƿ����E164�����ظ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPCSTR lpszConfE164 �����E164��   
    ����ֵ˵��  ��TRUE-�������ظ� FALSE-���ظ�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/07/25    3.0         ������        ����
====================================================================*/
BOOL32 CMcuVcData::IsConfE164Repeat( LPCSTR lpszConfE164, BOOL32 IsTemple )
{
	u8 byLoop;
	char abyMcuE164[MAXLEN_E164];
	g_cMcuAgent.GetE164Number( abyMcuE164, MAXLEN_E164 );
	if( memcmp( abyMcuE164, lpszConfE164, MAXLEN_E164 ) == 0 )
	{
		return TRUE;
	}
	
    if(IsTemple)
    {
        for(byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
        {
            if(!m_ptTemplateInfo[byLoop].IsEmpty() && 
               0 == memcmp(m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfE164(), lpszConfE164, MAXLEN_E164 ))
            {
                return TRUE;
            }
        }
    }
    else
    {
        for( byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++ )
        {
            if ( m_apConfInst[byLoop] != NULL )
            {
                if(0 == memcmp( m_apConfInst[byLoop]->m_tConf.GetConfE164(), lpszConfE164, MAXLEN_E164 ))
                {
                    return TRUE;	
                }
            }
        }
	}

	return FALSE;	
}

/*====================================================================
    ������      ��GetConfIdxByE164
    ����        ���ӻ����E164����õ������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPCSTR lpszConfE164 �����E164�� 
				  pbyOngoningConfIdx ���ص�ongonig conf index
				  pbyTemplateConfIdx ���ص�template conf idex

    ����ֵ˵��  ��TRUE-�������ظ� FALSE-���ظ�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/07/25    3.0         ������        ����
====================================================================*/
u8  CMcuVcData::GetOngoingConfIdxByE164( LPCSTR lpszConfE164 )
{
    if( NULL == lpszConfE164 )
    {
        return 0;
    }

    for (u8 byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++)
    {
        if (m_apConfInst[byLoop] == NULL)
        {
            continue;
        }
        
        if (0 == memcmp(m_apConfInst[byLoop]->m_tConf.GetConfE164(), lpszConfE164, MAXLEN_E164)) 
        {
            return m_apConfInst[byLoop]->m_byConfIdx;
        }
    }
    
    return 0;
}

u8  CMcuVcData::GetTemConfIdxByE164( LPCSTR lpszConfE164 )
{
    if( NULL == lpszConfE164 )
    {
        return 0;
    }

    for( u8 byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
    {
        if (m_ptTemplateInfo[byLoop].IsEmpty())
        {
            continue;
        }
        
        if (0 == memcmp(m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfE164(), lpszConfE164, MAXLEN_E164))
        {
            return m_ptTemplateInfo[byLoop].m_byConfIdx;
        } 
    }

    return 0;
}

/*=============================================================================
  �� �� ���� GetConfRateByConfIdx
  ��    �ܣ� �ɻ������ȡ��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u16 wConfIdx
             u16& wFirstRate
             u16& wSecondRate
             BOOL32 bTemplate /* = FALSE  */
//  �� �� ֵ�� u16 
// =============================================================================*/
u16 CMcuVcData::GetConfRateByConfIdx( u16 wConfIdx, u16& wFirstRate, u16& wSecondRate, BOOL32 bTemplate /* = FALSE  */)
{
    if( 0 == wConfIdx)
    {
        return 0;
    }
    u16 wAudRate = 0;
    if( FALSE == bTemplate )
    {
        for (u8 byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++)
        {
            if (NULL == m_apConfInst[byLoop] )
            {
                continue;
            }
    
            if ( m_apConfInst[byLoop]->m_byConfIdx == (u8)wConfIdx ) 
            {
                wFirstRate = m_apConfInst[byLoop]->m_tConf.GetBitRate();
                wSecondRate = m_apConfInst[byLoop]->m_tConf.GetSecBitRate();
                TCapSupport m_tCapSupport = m_apConfInst[byLoop]->m_tConf.GetCapSupport();
                wAudRate = GetAudioBitrate(m_tCapSupport.GetMainAudioType() );
                wFirstRate += wAudRate;
                wSecondRate = ( 0 != wSecondRate ) ? (wSecondRate+wAudRate): 0;
                
                break;
            }
        }
    }
    else
    {
        for( u8 byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
        {
            if (m_ptTemplateInfo[byLoop].IsEmpty())
            {
                continue;
            }
    
            if ( m_ptTemplateInfo[byLoop].m_byConfIdx == wConfIdx )
            {
                wFirstRate = m_ptTemplateInfo[byLoop].m_tConfInfo.GetBitRate();
                wSecondRate = m_ptTemplateInfo[byLoop].m_tConfInfo.GetSecBitRate();
                wAudRate = GetAudioBitrate( m_ptTemplateInfo[byLoop].m_tConfInfo.GetMainAudioMediaType() );
                wFirstRate += wAudRate;
                wSecondRate = ( 0 != wSecondRate ) ? (wSecondRate+wAudRate): 0;
            } 
        }
    }
    return 0;
}

/*=============================================================================
  �� �� ���� GetConfNameByConfId
  ��    �ܣ� ���ݻ���IDȡ��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� u8 ConfIdx, 0 - û���ҵ�
 =============================================================================*/
u8 CMcuVcData::GetConfNameByConfId( const CConfId cConfId, LPCSTR &lpszConfName )
{
    lpszConfName = NULL;
    for (u8 byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++)
    {
        if (NULL == m_apConfInst[byLoop] )
        {
            continue;
        }

        if ( m_apConfInst[byLoop]->m_tConf.GetConfId() == cConfId ) 
        {
            lpszConfName = m_apConfInst[byLoop]->m_tConf.GetConfName();
            return m_apConfInst[byLoop]->m_byConfIdx;
        }
    }    
    return 0;
}


/*=============================================================================
  �� �� ���� GetConfIdByName
  ��    �ܣ� ���ݻ�������ȡCConfId
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  
  �� �� ֵ�� u8 ConfIdx, 0 - û���ҵ�
 =============================================================================*/
CConfId CMcuVcData::GetConfIdByName(LPCSTR lpszConfName, BOOL32 bTemplate )
{
    CConfId cConfId;
    cConfId.SetNull();
    u8 byLoop = 0;

    if (!bTemplate)
    {
        for (byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++)
        {
            if (NULL == m_apConfInst[byLoop] )
            {
                continue;
            }

            if ( strncmp(m_apConfInst[byLoop]->m_tConf.GetConfName(), lpszConfName, MAXLEN_CONFNAME) == 0) 
            {
                cConfId = m_apConfInst[byLoop]->m_tConf.GetConfId();
                break;
            }
        }  
    }
    else
    {
        for( byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
        {
            if (m_ptTemplateInfo[byLoop].IsEmpty())
            {
                continue;
            }
    
            if ( strncmp(m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfName(), lpszConfName, MAXLEN_CONFNAME) == 0) 
            {
                cConfId = m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfId();
                break;                
            } 
        }        
    }
    
    return cConfId;
}

/*=============================================================================
  �� �� ���� GetConfIdByE164
  ��    �ܣ� ���ݻ�������ȡCConfId
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  
  �� �� ֵ�� u8 ConfIdx, 0 - û���ҵ�
 =============================================================================*/
CConfId CMcuVcData::GetConfIdByE164( LPCSTR lpszConfE164, BOOL32 bTemplate )
{
    CConfId cConfId;
    cConfId.SetNull();
    u8 byLoop = 0;

    if (!bTemplate)
    {
        for (byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++)
        {
            if (NULL == m_apConfInst[byLoop] )
            {
                continue;
            }

            if ( strncmp(m_apConfInst[byLoop]->m_tConf.GetConfE164(), lpszConfE164, MAXLEN_E164) == 0) 
            {
                cConfId = m_apConfInst[byLoop]->m_tConf.GetConfId();
                break;
            }
        }  
    }
    else
    {
        for( byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
        {
            if (m_ptTemplateInfo[byLoop].IsEmpty())
            {
                continue;
            }
    
            if ( strncmp(m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfE164(), lpszConfE164, MAXLEN_E164) == 0) 
            {
                cConfId = m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfId();
                break;                
            } 
        }        
    }
    
    return cConfId;
}

/*====================================================================
    ������      ��GetConfNum
    ����        ����ѯ��ǰ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bIncOngoing,           ����ʱ����
                  BOOL32 bIncSched,             ��ԤԼ����
                  BOOL32 bIncTempl,             ��ģ��  
				  u8 byConfSource               ���ĳһ���𷽵���Ϣͳ��
    ����ֵ˵��  ��ʵ�ʷ��صĻ�����Ŀ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/03/19    1.0         Qzj           ����
	03/11/08    3.0         ������        �޸�
    07/01/19    4.0         ����        �޸�
	08/11/28                ���㻪        �޸�
====================================================================*/
u8 CMcuVcData::GetConfNum( BOOL32 bIncOngoing, BOOL32 bIncSched, BOOL32 bIncTempl, u8 byConfSource /*= MCS_CONF*/ ) const
{
	u8		byLoop;
	u8		byNum = 0;
	for( byLoop = 0; byLoop < MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE; byLoop++ )
	{
        TConfMapData tMapData = m_atConfMapData[byLoop];
        if ( (tMapData.IsValidConf() && NULL != m_apConfInst[tMapData.GetInsId()-1] ) )
        {
			if (byConfSource != ALL_CONF && m_apConfInst[tMapData.GetInsId()-1]->m_tConf.GetConfSource() != byConfSource)
			{
				break;
			}

            if ( bIncOngoing && m_apConfInst[tMapData.GetInsId()-1]->m_tConf.m_tStatus.IsOngoing() )
            {
                byNum ++;
            }
            if ( bIncSched && m_apConfInst[tMapData.GetInsId()-1]->m_tConf.m_tStatus.IsScheduled() )
            {
                byNum ++;
            }
        }
        if ( bIncTempl && tMapData.IsTemUsed() )
		{
			if (byConfSource != ALL_CONF && m_ptTemplateInfo[tMapData.GetTemIndex()].m_tConfInfo.GetConfSource() != byConfSource)
			{
				break;
			}

			byNum ++;
		}            
	}

	return( byNum );
}

/*====================================================================
    ������      ��GetConfAllMtInfo
    ����        ���õ�ָ������λ�õ�һ������������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx������λ��
				  TConfAllMtInfo * pConfAllMtInfo����������ݿռ�
    ����ֵ˵��  ��TRUE - �ɹ� FALSE - ������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/08/12    3.0         ������        ����
====================================================================*/
BOOL32 CMcuVcData::GetConfAllMtInfo( u8 byConfIdx, TConfAllMtInfo * pConfAllMtInfo ) const
{
    CHECK_CONFIDX(byConfIdx)

	if(pConfAllMtInfo == NULL )
	{
		return FALSE;
	}

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
	if( tMapData.IsValidConf() && m_apConfInst[tMapData.GetInsId() - 1] != NULL )
	{
		memcpy( pConfAllMtInfo, &m_apConfInst[tMapData.GetInsId() - 1]->m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
		return TRUE;
	}

	return FALSE;
}

/*====================================================================
    ������      ��GetConfMtTable
    ����        ���õ�ָ������λ�õ�һ�������ն˱�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx������λ��
				  TConfMtTable * ptMtTable����������ݿռ�
    ����ֵ˵��  ��TRUE - �ɹ� FALSE - ������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/08    3.0         ������        ����
====================================================================*/
BOOL32 CMcuVcData::GetConfMtTable( u8 byConfIdx, TConfMtTable * ptMtTable ) const
{
    CHECK_CONFIDX(byConfIdx)

	if( ptMtTable == NULL )
	{
		return FALSE;
	}

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
	if( tMapData.IsValidConf() && m_apConfInst[tMapData.GetInsId() - 1] != NULL )
	{
		memcpy( ptMtTable, m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable, sizeof( TConfMtTable ) );
		return TRUE;
	}

    return FALSE;
}

/*====================================================================
    ������      ��GetConfProtectInfo
    ����        ���õ�ָ������λ�õ�һ�����鱣����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx������λ�ã�Ϊ�������ʧ��
				  TConfProtectInfo *tConfProtectInfo����������ݿռ�
    ����ֵ˵��  ��TRUE - �ɹ� FALSE - ������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/08    3.0         ������        ����
====================================================================*/
BOOL32 CMcuVcData::GetConfProtectInfo( u8 byConfIdx, TConfProtectInfo *ptConfProtectInfo ) const
{
    CHECK_CONFIDX(byConfIdx)
	
	if(ptConfProtectInfo == NULL )
	{
		return FALSE;
	}

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
	if( tMapData.IsValidConf() && m_apConfInst[tMapData.GetInsId() - 1] != NULL )
	{
		memcpy( ptConfProtectInfo, &m_apConfInst[tMapData.GetInsId() - 1]->m_tConfProtectInfo , sizeof( TConfProtectInfo ) );
		return TRUE;
	}

    return FALSE;
}

/*====================================================================
    ������      ��GetConfMtTable
    ����        ���õ�ָ������λ�õ�һ�������ն˱�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx������λ��
    ����ֵ˵��  ��������� - �����ն˱�ָ�� ���鲻���� - NULL
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/03/19    1.0         Qzj           ����
	03/11/08    3.0         ������        �޸�
====================================================================*/
TConfMtTable * CMcuVcData::GetConfMtTable( u8 byConfIdx ) const
{
	if ( byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX )
	{
		OspPrintf( TRUE, FALSE, "[DATA]: serious error occur in GetConfMtTable(), ConfIdx: %d \n", byConfIdx );
		return NULL;
	}

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
	if( tMapData.IsValidConf() && NULL != m_apConfInst[tMapData.GetInsId() - 1] )
	{
		return m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable;
	}
	
	return NULL;

}

/*====================================================================
    ������      ��GetConfSwitchTable
    ����        ���õ�ָ������λ�õ�һ�����齻����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx������λ�ã�Ϊ�������ʧ��
    ����ֵ˵��  ��������� - ���齻����ָ�� ���鲻���� - NULL
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/08    3.0         ������        ����
====================================================================*/
TConfSwitchTable * CMcuVcData::GetConfSwitchTable( u8 byConfIdx ) const
{
	if( byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX )
	{
		return NULL;
	}

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if( tMapData.IsValidConf() && NULL != m_apConfInst[tMapData.GetInsId() - 1] )
    {
        return m_apConfInst[tMapData.GetInsId() - 1]->m_ptSwitchTable;        
    }

	return NULL;
}

/*====================================================================
    ������      ��GetConfEqpModule
    ����        ���õ�ָ������λ�õ�һ�������豸ģ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx������λ�ã�Ϊ�������ʧ��
    ����ֵ˵��  ��������� - ���齻����ָ�� ���鲻���� - NULL
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/08    3.0         ������        ����
====================================================================*/
TConfEqpModule *CMcuVcData::GetConfEqpModule( u8 byConfIdx ) const
{
    CHECK_CONFIDX(byConfIdx)
	
    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if( tMapData.IsValidConf() && m_apConfInst[tMapData.GetInsId() - 1]!=NULL )
	{
		return &m_apConfInst[tMapData.GetInsId() - 1]->m_tConfEqpModule;
	}

	return NULL;
}

/*====================================================================
    ������      ��GetConfAllMtInfo
    ����        ���õ�ָ������λ�õ�һ�����������ն���Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx������λ�ã�Ϊ�������ʧ��
    ����ֵ˵��  ��������� - ���齻����ָ�� ���鲻���� - NULL
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/08    3.0         ������        ����
====================================================================*/
TConfAllMtInfo *CMcuVcData::GetConfAllMtInfo( u8 byConfIdx ) const
{
    CHECK_CONFIDX(byConfIdx)

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
	if( tMapData.IsValidConf() &&  m_apConfInst[tMapData.GetInsId() - 1]!=NULL )
	{
		return &m_apConfInst[tMapData.GetInsId() - 1]->m_tConfAllMtInfo;
	}

	return NULL;
}

/*====================================================================
    ������      ��GetConfProtectInfo
    ����        ���õ�ָ������λ�õ�һ�����鱣����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx������λ�ã�Ϊ�������ʧ��
    ����ֵ˵��  ��������� - ���齻����ָ�� ���鲻���� - NULL
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/08    3.0         ������        ����
====================================================================*/
TConfProtectInfo *CMcuVcData::GetConfProtectInfo( u8 byConfIdx ) const
{
    CHECK_CONFIDX(byConfIdx)

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
	if( tMapData.IsValidConf() && m_apConfInst[tMapData.GetInsId() - 1]!=NULL )
	{
		return &m_apConfInst[tMapData.GetInsId() - 1]->m_tConfProtectInfo;
	}

	return NULL;
}
	
/*====================================================================
    ������      ��SaveConfToFile
    ����        ���ѻ���洢���ļ� 
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx������λ��
    ����ֵ˵��  �������ɹ� - TRUE  ����ʧ�� - FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/08    3.0         ������        �޸�
====================================================================*/
BOOL32 CMcuVcData::SaveConfToFile( u8 byConfIdx, BOOL32 bTemplate, BOOL32 bDefaultConf )
{
	CHECK_CONFIDX(byConfIdx)

	TConfStore tConfStore;

	if (!bTemplate)
	{        
		if (m_atConfMapData[byConfIdx-MIN_CONFIDX].IsValidConf())
		{
			//get tConfInfo
			if (NULL == GetConfInstHandle(byConfIdx))
			{
				OspPrintf(TRUE, FALSE, "[CMcuVcData]: GetConfInstHandle NULL! -- %d\n", byConfIdx);
				return FALSE;
			}
			TConfInfo *ptConfFullInfo = &GetConfInstHandle(byConfIdx)->m_tConf;
			tConfStore.m_tConfInfo = *ptConfFullInfo;
			if (CONF_LOCKMODE_LOCK == tConfStore.m_tConfInfo.m_tStatus.GetProtectMode())
			{
				tConfStore.m_tConfInfo.m_tStatus.SetProtectMode(CONF_LOCKMODE_NONE);
			}

            //���漴ʱ����ʱ����յ�ǰ����״̬�е�����״̬
            tConfStore.m_tConfInfo.m_tStatus.SetNoMixing();
            tConfStore.m_tConfInfo.m_tStatus.SetVACing(FALSE);
            tConfStore.m_tConfInfo.m_tStatus.SetNoPlaying();
            tConfStore.m_tConfInfo.m_tStatus.SetNoRecording();
            tConfStore.m_tConfInfo.m_tStatus.SetVMPMode(CONF_VMPMODE_NONE);
            tConfStore.m_tConfInfo.m_tStatus.SetVmpTwMode(CONF_VMPTWMODE_NONE);
            tConfStore.m_tConfInfo.m_tStatus.SetPollMode(CONF_POLLMODE_NONE);
            tConfStore.m_tConfInfo.m_tStatus.SetTvWallPollState(POLL_STATE_NONE);
            
            //zbq [09/24/2007] ���漴ʱ���飬��յ�ǰ��BAS״̬
            if ( tConfStore.m_tConfInfo.m_tStatus.IsAudAdapting() )
            {
                tConfStore.m_tConfInfo.m_tStatus.SetAdaptMode(CONF_BASMODE_AUD, FALSE);
            }
            if ( tConfStore.m_tConfInfo.m_tStatus.IsVidAdapting() )
            {
                tConfStore.m_tConfInfo.m_tStatus.SetAdaptMode(CONF_BASMODE_VID, FALSE);
            }
            if ( tConfStore.m_tConfInfo.m_tStatus.IsBrAdapting() )
            {
                tConfStore.m_tConfInfo.m_tStatus.SetAdaptMode(CONF_BASMODE_BR, FALSE);
            }
            if ( tConfStore.m_tConfInfo.m_tStatus.IsCasdAudAdapting() )
            {
                tConfStore.m_tConfInfo.m_tStatus.SetAdaptMode(CONF_BASMODE_CASDAUD, FALSE);
            }
            if ( tConfStore.m_tConfInfo.m_tStatus.IsCasdVidAdapting() )
            {
                tConfStore.m_tConfInfo.m_tStatus.SetAdaptMode(CONF_BASMODE_CASDVID, FALSE);
            }
            // �����������״̬, zgc, 2008-08-09
            if ( tConfStore.m_tConfInfo.m_tStatus.IsHdVidAdapting() )
            {
                tConfStore.m_tConfInfo.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_VID, FALSE);
            }
            
			TConfEqpModule tConfEqpModule = *GetConfEqpModule(byConfIdx);
			tConfStore.m_tMultiTvWallModule = tConfEqpModule.m_tMultiTvWallModule; 
			tConfStore.m_atVmpModule = tConfEqpModule.GetVmpModule();
            
            tConfStore.EmptyAllTvMember();
            tConfStore.EmptyAllVmpMember();

			//get alias array
            u8 byMemberType = 0;
			TMt tMt;
			TMtAlias tMtAlias;
			TConfMtTable *ptMtTable = GetConfMtTable(byConfIdx);
            // guzh [5/31/2007] ������ʱ�������նˣ���Ҫ���Ᵽ������
            u8 byStoreIdx = 0;
			for (u8 byLoop = 0; byLoop < ptMtTable->m_byMaxNumInUse; byLoop++)
			{
				tMt = ptMtTable->GetMt(byLoop+1);
				if (!tMt.IsNull())
				{
					if (MT_TYPE_MMCU == tMt.GetMtType())
					{
						continue;
					}

                    //zbq [08/09/2007] ���ȱ����ն˵ĺ��б�����Ȼ�����α���E164/H323ID��IP.
                    if (ptMtTable->GetDialAlias((byLoop+1), &tMtAlias))
                    {
                        tConfStore.m_atMtAlias[byStoreIdx] = tMtAlias;
                    }
                    else
                    {
                        //����mtAliasTypeH320Alias���Ͳ�������������Ϣ�����Բ����б���
					    if (ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeH320ID, &tMtAlias))
					    {
						    tConfStore.m_atMtAlias[byStoreIdx] = tMtAlias;
					    }
					    else if (ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeE164, &tMtAlias))
					    {
						    tConfStore.m_atMtAlias[byStoreIdx] = tMtAlias;			
					    }
					    else if (ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeH323ID, &tMtAlias))
                        {
                            tConfStore.m_atMtAlias[byStoreIdx] = tMtAlias;
					    }
					    else if (ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeTransportAddress, &tMtAlias))
					    {
						    tConfStore.m_atMtAlias[byStoreIdx] = tMtAlias;
					    }
                    }

                    tConfStore.m_awMtDialBitRate[byStoreIdx] = ptMtTable->GetDialBitrate(byLoop+1);
            
					//����ģ��ӳ���ϵ
					if (tConfStore.m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
					{
						for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL; byTvLp++)
                        {                            
                            u8 byTvId = tConfEqpModule.m_tTvWallInfo[byTvLp].m_tTvWallEqp.GetEqpId();
                            for(u8 byTvChlLp = 0; byTvChlLp < MAXNUM_PERIEQP_CHNNL; byTvChlLp++)
                            {
                                if(TRUE == tConfEqpModule.IsMtInTvWallChannel(byTvId, byTvChlLp, tMt, byMemberType) )
                                {
                                    tConfStore.SetMtInTvChannel(byTvId, byTvChlLp, byStoreIdx+1, byMemberType);
                                }
                            }
                        }
					}
					if (tConfStore.m_tConfInfo.GetConfAttrb().IsHasVmpModule())
					{
                        for (u8 byVmpIdx = 0; byVmpIdx < MAXNUM_MPUSVMP_MEMBER; byVmpIdx++)
                        {
                            if( tConfEqpModule.IsMtAtVmpChannel(byVmpIdx, tMt, byMemberType) )
						    {
                                tConfStore.m_atVmpModule.SetVmpMember(byVmpIdx, byStoreIdx+1, byMemberType);
						    }
                        }
					}            
                    byStoreIdx ++;
				}                
			}
            tConfStore.m_byMtNum = byStoreIdx;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if(!m_atConfMapData[byConfIdx-MIN_CONFIDX].IsTemUsed())
		{
			return FALSE;
		}
        // zbq [09/06/2007] ����ģ����Ϣ�������bFromFile
        u8 bySaveTmpIdx = m_atConfMapData[byConfIdx-MIN_CONFIDX].GetTemIndex();
        m_ptTemplateInfo[bySaveTmpIdx].m_tConfInfo.m_tStatus.SetTakeFromFile(FALSE);
		memcpy(&tConfStore, &m_ptTemplateInfo[bySaveTmpIdx], sizeof(TConfStore));
	}

	return AddConfToFile(tConfStore, bDefaultConf);
}

/*====================================================================
    ������      ��RemoveConfFromFile
    ����        �����ļ�ɾ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CConfId cConfId Ҫɾ���Ļ����
    ����ֵ˵��  �������ɹ� - TRUE  ����ʧ�� - FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/08    3.0         ������        �޸�
====================================================================*/
BOOL32 CMcuVcData::RemoveConfFromFile( CConfId cConfId )
{
	if (cConfId.IsNull())
	{
		return FALSE;
	}

	return DeleteConfFromFile(cConfId);
}

/*====================================================================
    ������      ��GetConfFullInfo
    ����        ���õ�ָ������λ�õ�һ������������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[IN]  u8 byConfIdx������λ��
	              [IN]  TPackConfStore *ptPackConfStore ��СΪԤ���� sizeof(TConfStore)
    ����ֵ˵��  �������ɹ� - TRUE  ����ʧ�� - FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/08    3.0         ������        �޸�
====================================================================*/
BOOL32 CMcuVcData::GetConfFromFile( u8 byConfIdx, TPackConfStore *ptPackConfStore )
{
    return ::GetConfFromFile((byConfIdx-MIN_CONFIDX), ptPackConfStore);
}

/*=============================================================================
  �� �� ���� InitPeriDcsList
  ��    �ܣ� ��ʼ��DCS�б�����֧�ֲ�����Ŀǰֱ�ӹ����ʼ�ṹ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/15    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcData::InitPeriDcsList()
{
	u8  byDcsId = 0;
    u32 dwDcsIp = 0;
	memset( m_atPeriDcsTable, 0, sizeof(m_atPeriDcsTable) );
	
    for ( byDcsId = 1; byDcsId <= MAXNUM_MCU_DCS; byDcsId ++ )
    {
        if ( 0 != g_cMcuAgent.GetDcsIp() )
        {
            m_atPeriDcsTable[byDcsId-1].m_bIsValid = TRUE;
            m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.SetType( TYPE_MCUPERI );
            m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.SetEqpType( EQP_TYPE_DCS );
            m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.SetEqpId(byDcsId);
            m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.SetMcuId(LOCAL_MCUID);        
            m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.m_byOnline = FALSE;
            
            //MCU��DCS�����������
            g_cMcuVcApp.SetDcsAlias( byDcsId );
            m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.SetDcsAlias(g_cMcuVcApp.GetDcsAlias(byDcsId));
        }        
    }
    return;
}

/*=============================================================================
  �� �� ���� SetPeriDcsConnected
  ��    �ܣ� ����DCS������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byDcsId
             BOOL32 bConnected
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/15    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcData::SetPeriDcsConnected( u8 byDcsId, BOOL32 bConnected )
{
	if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS )
	{
		return;
	}	
	m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.m_byOnline = bConnected;
	
	return;
}

/*=============================================================================
  �� �� ���� IsPeriDcsConnected
  ��    �ܣ� �ж�DCS�Ƿ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byDcsId
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/15    4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsPeriDcsConnected( u8 byDcsId )
{
	if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS )
	{
		return FALSE;
	}
	return( m_atPeriDcsTable[byDcsId - 1].m_tDcsStatus.m_byOnline );
}

/*=============================================================================
  �� �� ���� SetPeriDcsValid
  ��    �ܣ� ����DCS����״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byDcsId
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/26    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcData::SetPeriDcsValid( u8 byDcsId )
{
    if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS )
	{
		return;
	}

	m_atPeriDcsTable[byDcsId-1].m_bIsValid = TRUE;
}

/*=============================================================================
  �� �� ���� IsPeriDcsValid
  ��    �ܣ� �ж�DCS�Ƿ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byDcsId
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/15    4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsPeriDcsValid( u8 byDcsId )
{
    if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS )
	{
		return FALSE;
	}

	return m_atPeriDcsTable[byDcsId-1].m_bIsValid;
}

/*=============================================================================
  �� �� ���� SetDcsAlias
  ��    �ܣ� ����DCS�ı���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byDcsId
             LPCSTR lpszDcsAlias
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/15    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcData::SetDcsAlias( u8 byDcsId )
{
	if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS )
	{
		return;
	}
	s8 achDcsName[MAXLEN_EQP_ALIAS];
	sprintf( achDcsName, "%s%d", "dcs", byDcsId );
	m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.SetDcsAlias( achDcsName );
}

/*=============================================================================
  �� �� ���� GetDcsAlias
  ��    �ܣ� ���DCS�ı���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byDcsId
  �� �� ֵ�� LPCSTR 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/15    4.0			�ű���                  ����
=============================================================================*/
LPCSTR CMcuVcData::GetDcsAlias( u8 byDcsId )
{
	if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS )
	{
		return NULL;
	}
	return m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.GetDcsAlias();
}

/*=============================================================================
  �� �� ���� IsDcsConfiged
  ��    �ܣ� DCS�Ƿ�������
  �㷨ʵ�֣� Ŀǰ����ֻ֧��һ��DCS
  ȫ�ֱ����� 
  ��    ���� u32 dwDcsIp
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/1/10    4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsPeriDcsConfiged( u32 dwDcsIp )
{
	for( s32 nIndex = 0; nIndex < MAXNUM_MCU_DCS; nIndex ++ )
	{
		if ( dwDcsIp ==  ntohl(g_cMcuAgent.GetDcsIp()) )
		{
			return TRUE;
		}
	}
	return FALSE;
}


/*=============================================================================
    ������      ��GetPeriDcsStatus
    ����        �����DCS��״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byDcsId��DCS ID
				  TPeriDcsStatus * ptStatus, ���ص�DCS״̬
    ����ֵ˵��  ����
-------------------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��       �汾           �޸���                �޸�����
    2006/6/12    4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetPeriDcsStatus( u8 byDcsId, TPeriDcsStatus * ptStatus )
{
	if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS || NULL == ptStatus )
	{
		return FALSE;
	}

	*ptStatus = m_atPeriDcsTable[byDcsId - 1].m_tDcsStatus;

	return TRUE;
}

/*=============================================================================
    ������      ��SetPeriDcsStatus
    ����        ������DCS��״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byDcsId��DCS ID
				  TPeriDcsStatus * ptStatus
    ����ֵ˵��  ����
-------------------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��       �汾           �޸���                �޸�����
    2006/6/12    4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMcuVcData::SetPeriDcsStatus( u8 byDcsId, const TPeriDcsStatus * ptStatus )
{
	if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS || NULL == ptStatus )
	{
		return FALSE;
	}

	m_atPeriDcsTable[byDcsId - 1].m_tDcsStatus = *ptStatus;
    return TRUE;
}

/*=============================================================================
    ������      ��GetDcs
    ����        �����DCS�ṹ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byDcsId��DCS ID
    ����ֵ˵��  ��TEqp
-------------------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��       �汾           �޸���                �޸�����
    2006/6/12    4.0			�ű���                  ����
=============================================================================*/
TEqp CMcuVcData::GetDcs( u8 byDcsId )
{
    TEqp tEqp;
    tEqp.SetNull();
    if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS || !IsPeriDcsValid( byDcsId ) )
    {
        return tEqp;
    }
    
    return (TEqp)m_atPeriDcsTable[byDcsId-1].m_tDcsStatus;
}

/*====================================================================
    ������      ��SetPeriEqpConnected
    ����        ��������������״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpId, ���� ID
				  BOOL32 bConnected, �Ƿ�����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/04/06    1.0         ���         ����
====================================================================*/
void CMcuVcData::SetPeriEqpConnected( u8 byEqpId, BOOL32 bConnected )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP )
	{
		return;
	}

	m_atPeriEqpTable[byEqpId - 1].m_tPeriEqpStatus.m_byOnline = bConnected;

	return;
}

/*====================================================================
    ������      ��SetPeriEqpLogicChnnl
    ����        ������������߼�ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpId, ���� ID
				  u8 byMediaType, ý������
				  u8 byChnnlNum, MCU������ͨ����
				  TLogicalChannel * ptChnnl, �߼�ͨ������
				  BOOL32 bForwardChnnl, ����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/04/06    1.0         ���         ����
====================================================================*/
void CMcuVcData::SetPeriEqpLogicChnnl( u8 byEqpId, u8 byMediaType, u8 byChnnlNum, const TLogicalChannel * ptStartChnnl, BOOL32 bForwardChnnl )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || NULL == ptStartChnnl )
	{
		return ;
	}

	switch( byMediaType )
	{
	case MODE_VIDEO:
		if( bForwardChnnl )
		{
			m_atPeriEqpTable[byEqpId - 1].m_byFwdChannelNum = byChnnlNum;
			m_atPeriEqpTable[byEqpId - 1].m_tFwdVideoChannel = *ptStartChnnl;
		}
		else
		{
			m_atPeriEqpTable[byEqpId - 1].m_byRvsChannelNum = byChnnlNum;
			m_atPeriEqpTable[byEqpId - 1].m_tRvsVideoChannel = *ptStartChnnl;
		}
		break;
	case MODE_AUDIO:
		if( bForwardChnnl )		
		{
			m_atPeriEqpTable[byEqpId - 1].m_byFwdChannelNum = byChnnlNum;
			m_atPeriEqpTable[byEqpId - 1].m_tFwdAudioChannel = *ptStartChnnl;
		}
		else
		{
			m_atPeriEqpTable[byEqpId - 1].m_byRvsChannelNum = byChnnlNum;
			m_atPeriEqpTable[byEqpId - 1].m_tRvsAudioChannel = *ptStartChnnl;
		}
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::SetPeriEqpLogicChnnl(): wrong Mode %u!\n", byMediaType );
		break;
	}

	return;

}

/*====================================================================
    ������      ��SetPeriEqpStatus
    ����        ����������״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpId, ���� ID
				  const TPeriEqpStatus * ptStatus, ����״̬
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/10    1.0         ���         ����
====================================================================*/
void CMcuVcData::SetPeriEqpStatus( u8 byEqpId, const TPeriEqpStatus * ptStatus )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || NULL == ptStatus )
	{
		return ;
	}

	m_atPeriEqpTable[byEqpId - 1].m_tPeriEqpStatus = *ptStatus;
	
}

/************************************************************************/
/* ������״̬��Ϣ�Ķ�д����                                             */
/************************************************************************/
/*====================================================================
    ������      ��SetBasChanStatus
    ����        ������BASͨ��״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpId, ���� ID
				  u8 byChanNo, EBasStatus eStatus
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/09/07    4.0         libo          ����
====================================================================*/
//modify bas 2
void CMcuVcData::SetBasChanStatus(u8 byEqpId, u8 byChanNo, u8 byStatus)
{
	if (0 == byEqpId || byEqpId > MAXNUM_MCU_PERIEQP || byChanNo > MAXNUM_BAS_CHNNL)
	{
		return;
	}

    TBasStatus *ptBasStatus;
    ptBasStatus = &m_atPeriEqpTable[byEqpId - 1].m_tPeriEqpStatus.m_tStatus.tBas;
    ptBasStatus->tChnnl[byChanNo].SetStatus(byStatus);
}

/*====================================================================
    ������      ��SetBasChanReserved
    ����        ������BASͨ��Ԥ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpId, ���� ID
				  u8 byChanNo, EBasStatus eStatus
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/09/07    4.0         libo          ����
====================================================================*/
void CMcuVcData::SetBasChanReserved(u8 byEqpId, u8 byChanNo, BOOL32 bReserved)
{
    if (0 == byEqpId || byEqpId > MAXNUM_MCU_PERIEQP || byChanNo > MAXNUM_BAS_CHNNL)
	{
		return;
	}

    TBasStatus *ptBasStatus;
    ptBasStatus = &m_atPeriEqpTable[byEqpId - 1].m_tPeriEqpStatus.m_tStatus.tBas;
    ptBasStatus->tChnnl[byChanNo].SetReserved(bReserved);
}

/*====================================================================
    ������      ��SetMcConnected
    ����        ������MC����״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wMcInstId, �������̨ʵ����
				  BOOL32 bConnected, �Ƿ�����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/04/06    1.0         ���         ����
====================================================================*/
void CMcuVcData::SetMcConnected( u16 wMcInstId, BOOL32 bConnected )
{
	if( wMcInstId == 0 || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) )
	{
		return;
	}

	m_atMcTable[wMcInstId-1].m_bConnected = bConnected;

	return;
}

/*====================================================================
    ������      ��SetMcConnected
    ����        ������MC����״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wMcInstId, �������̨ʵ����
				  BOOL32 bConnected, �Ƿ�����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/04/06    1.0         ���         ����
====================================================================*/
void CMcuVcData::SetMcsRegInfo( u16 wMcInstId, TMcsRegInfo tMcsRegInfo )
{
	if( wMcInstId == 0 || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) )
	{
		return;
	}

	m_atMcTable[wMcInstId-1].m_tMcsRegInfo = tMcsRegInfo;
}

/*====================================================================
    ������      ��SetMcConnected
    ����        ������MC����״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wMcInstId, �������̨ʵ����
				  BOOL32 bConnected, �Ƿ�����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/04/06    1.0         ���         ����
====================================================================*/
void CMcuVcData::GetMcsRegInfo( u16 wMcInstId, TMcsRegInfo *ptMcsRegInfo )
{
	if( wMcInstId == 0 || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) || ptMcsRegInfo == NULL )
	{
		return;
	}

	*ptMcsRegInfo = m_atMcTable[wMcInstId-1].m_tMcsRegInfo;
}

/*====================================================================
    ������      ��SetMcLogicChnnl
    ����        ������MC���߼�ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wMcInstId, MC��
				  u8 byMediaType, ý������
				  u8 byChnnlNum, MCU��MCͨ����
				  TLogicalChannel * ptStartChnnl, �߼�ͨ������
    �������˵��:		��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/04/06    1.0         ���         ����
====================================================================*/
void CMcuVcData::SetMcLogicChnnl( u16 wMcInstId, u8 byMediaType, u8 byChnnlNum, const TLogicalChannel * ptStartChnnl )
{
	if( wMcInstId == 0 || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) || NULL == ptStartChnnl )
	{
		return ;
	}

	switch( byMediaType )
	{
	case MODE_VIDEO:
		m_atMcTable[ wMcInstId-1 ].m_tFwdVideoChannel = *ptStartChnnl;
		break;
	case MODE_AUDIO:
		m_atMcTable[ wMcInstId-1 ].m_tFwdAudioChannel = *ptStartChnnl;
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::SetMcLogicChnnl(): wrong Mode %u!\n", byMediaType );
		break;
	}
	m_atMcTable[ wMcInstId-1 ].m_byFwdChannelNum = byChnnlNum;

	return;	
}


/*====================================================================
    ������      ��SetMcSrc
    ����        ������MC�ķ���/������Ƶ����ƵԴ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wMcInstId, MC��
				  TMt * ptSrc, �ն�Դ
				  u8 byChnnlNo, ͨ����
				  u8 byMode, ��ģʽMODE_VIDEO/MODE_AUDIO
    ����ֵ˵��  ��none
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/04/06    1.0         ���         ����
====================================================================*/
void CMcuVcData::SetMcSrc( u16 wMcInstId, const TMt * ptSrc, u8 byChnnlNo, u8 byMode )
{
	if( wMcInstId == 0 || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) || NULL == ptSrc || byChnnlNo >= MAXNUM_MC_CHANNL )
	{
		return ;
	}

	switch( byMode )
	{
	case MODE_VIDEO:
		m_atMcTable[ wMcInstId-1 ].m_atVidSrc[byChnnlNo] = *ptSrc;
		break;
	case MODE_AUDIO:
		m_atMcTable[ wMcInstId-1 ].m_atAudSrc[byChnnlNo] = *ptSrc;
		break;
	case MODE_BOTH:
		m_atMcTable[ wMcInstId-1 ].m_atVidSrc[byChnnlNo] = *ptSrc;	
		m_atMcTable[ wMcInstId-1 ].m_atAudSrc[byChnnlNo] = *ptSrc;
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::SetMcSrc(): wrong Mode %u!\n", byMode );
		break;
	}

	return;
}

/*====================================================================
    ������      ��SetPeriEqpSrc
    ����        ������PeriEqp�ķ���/������Ƶ����ƵԴ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 periEqpId, Eqp��
				  TMt * ptSrc, �ն�Դ
				  u8 byChnnlNo, ͨ����
				  u8 byMode, ��ģʽMODE_VIDEO/MODE_AUDIO
    ����ֵ˵��  ��none
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/30    1.0         JQL           ����
====================================================================*/
void CMcuVcData::SetPeriEqpSrc( u8 byEqpId, const TMt * ptSrc, u8 byChnnlNo, u8 byMode )
{
	
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP 
		|| NULL == ptSrc || byChnnlNo >= MAXNUM_PERIEQP_CHNNL )
	{
		return ;
	}

	switch( byMode )
	{
	case MODE_VIDEO:
		m_atPeriEqpTable[ byEqpId - 1 ].m_atVidSrc[byChnnlNo] = *ptSrc;
		break;
	case MODE_AUDIO:
		m_atPeriEqpTable[ byEqpId - 1 ].m_atAudSrc[byChnnlNo] = *ptSrc;
		break;
	case MODE_BOTH:
		m_atPeriEqpTable[ byEqpId - 1 ].m_atVidSrc[byChnnlNo] = *ptSrc;
		m_atPeriEqpTable[ byEqpId - 1 ].m_atAudSrc[byChnnlNo] = *ptSrc;
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::SetMcSrc(): wrong Mode %u!\n", byMode );
		break;
	}

	return;
}


/*====================================================================
    ������      ��AddMp
    ����        ������һ��Mp
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tMp Mp��Ϣ
    ����ֵ˵��  ���ɹ�TRUE,ʧ��FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
BOOL32 CMcuVcData::AddMp( TMp tMp )
{
    if (0 == tMp.GetMpId() || tMp.GetMpId() > MAXNUM_DRI)
    {
        return FALSE;
    }

    m_atMpData[tMp.GetMpId() - 1].m_bConnected = TRUE;
	m_atMpData[tMp.GetMpId() - 1].m_tMp = tMp; 
    
    // guzh [1/17/2007] ����MP����ȷ�����������Ƶ�����
    u16 wMaxBand  = 0;
    u32 dwMaxPkNum = 0;
#ifdef _MINIMCU_
    // ����8000B��Ҫ���� MPC/MDSC/HDSC
    if ( tMp.GetAttachMode() == 1 )
    {
        wMaxBand = g_cMcuVcApp.GetPerfLimit().m_wMpcMaxMpAbility;
        dwMaxPkNum = g_cMcuVcApp.GetPerfLimit().m_dwMpcMaxPkNum;
    }
    else
    {
        // Ϊ�˷�ֹ�û����ô���ֱ�Ӳ����ڲ�Ĭ��ֵ
        u8 byType = g_cMcuAgent.GetRunBrdTypeByIdx( tMp.GetMpId() );
        if ( byType == 0xFF )
        {
            wMaxBand = 0;
            dwMaxPkNum = 0;
        }
        else if ( byType == DSL8000_BRD_HDSC )
        {
            wMaxBand = MAXLIMIT_CRI_MP_HDSC;
            dwMaxPkNum = MAXLIMIT_CRI_MP_HDSC_PMNUM * 1024;
        }
        else
        {
            wMaxBand = MAXLIMIT_CRI_MP_MDSC;
            dwMaxPkNum = MAXLIMIT_CRI_MP_MDSC_PMNUM * 1024;
        }
    }
#else
    if ( tMp.GetAttachMode() == 1 )
    {
        wMaxBand = g_cMcuVcApp.GetPerfLimit().m_wMpcMaxMpAbility;
        dwMaxPkNum = g_cMcuVcApp.GetPerfLimit().m_dwMpcMaxPkNum;
    }
    else
    {
        wMaxBand = g_cMcuVcApp.GetPerfLimit().m_wCriMaxMpAbility;
        dwMaxPkNum = g_cMcuVcApp.GetPerfLimit().m_dwCriMaxPkNum;
    }
#endif
    m_atMpData[tMp.GetMpId()-1].m_wNetBandAllowed = wMaxBand;
    m_atMpData[tMp.GetMpId()-1].m_dwPkNumAllowed = dwMaxPkNum;

	return TRUE;
}

/*====================================================================
    ������      ��RemoveMp
    ����        ���Ƴ�Mp
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byMpId Mp���	          
    ����ֵ˵��  ���ɹ�TRUE,ʧ��FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
BOOL32 CMcuVcData::RemoveMp( u8 byMpId )
{
	if(!IsMpConnected(byMpId))
	{
		return FALSE;
	}

    m_atMpData[byMpId - 1].m_bConnected = FALSE;
	memset( &m_atMpData[byMpId - 1].m_tMp, 0, sizeof(TMp) );
    m_atMpData[byMpId - 1].m_wMtNum = 0;
    memset( m_atMpData[byMpId - 1].m_abyMtId, 0, sizeof(m_atMpData[byMpId - 1].m_abyMtId) );
    m_atMpData[byMpId - 1].m_wNetBandAllowed = 0;

	return TRUE;
}

/*====================================================================
    ������      ��IsMpConnected
    ����        ���ж�Mp�Ƿ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byMpId Mp���
    ����ֵ˵��  ��TRUE - ����, FALSE - δ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
BOOL32 CMcuVcData::IsMpConnected( u8 byMpId )
{
    if (0 == byMpId || byMpId > MAXNUM_DRI)
    {
        return FALSE;
    }

    return m_atMpData[byMpId - 1].m_bConnected;
}

/*====================================================================
    ������      ��GetMpIpAddr
    ����        ���õ�Mp��Ip��ַ 
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byMpId Mp���
	              byIpNO �ڼ���IP
    ����ֵ˵��  ���ɹ�����IP��ַ, ʧ��0
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
u32  CMcuVcData::GetMpIpAddr( u8 byMpId )
{
    if(!IsMpConnected(byMpId))
	{
		return 0;
	}

	return m_atMpData[byMpId - 1].m_tMp.GetIpAddr( );
}

/*====================================================================
    ������      ��GetMpNum
    ����        ���õ�Mp����Ŀ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��Mp������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
u8   CMcuVcData::GetMpNum( void )
{
	u8 byMpNum = 0;
    for(u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++ )
	{
        if(m_atMpData[byLoop].m_bConnected)
		{
		    byMpNum++;
		}
	}
	return byMpNum;
}

/*====================================================================
    ������      ��FindMp
    ����        ������Mp
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 dwIp   ip��ַ��������
    ����ֵ˵��  ������Mp��Id
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
u8  CMcuVcData::FindMp( u32 dwIp )
{
	u8 byMpId = 0;
	int nMpId = 0;
	while( nMpId < MAXNUM_DRI && byMpId == 0)
	{
		//����δ���ӵ�
		if(!m_atMpData[nMpId].m_bConnected)
		{
			nMpId++;
			continue;
		}

		//�����Ƿ��Ǵ�Mp
		if( m_atMpData[nMpId].m_tMp.GetIpAddr( ) == dwIp )
		{
			byMpId = nMpId+1;
			break;
		}
		nMpId++;
	}

	return byMpId;
}

/*====================================================================
    ������      ��GetValidMp
    ����        ���õ�һ��Mp
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ������Mp��Id
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/10/21    3.0         ������        ����
====================================================================*/
u32  CMcuVcData::GetAnyValidMp( void )
{
	int nMpId = 0;
	while( nMpId < MAXNUM_DRI )
	{
		//����δ���ӵ�
		if(!m_atMpData[nMpId].m_bConnected)
		{
			nMpId++;
			continue;
		}

		return m_atMpData[nMpId].m_tMp.GetIpAddr( );

	}

	return 0;	
}

u8   CMcuVcData::GetMpMulticast(u8 byMpId)
{
    if (byMpId == 0 || byMpId > MAXNUM_DRI || !m_atMpData[byMpId - 1].m_bConnected)
    {
        return 0;
    }

    return m_atMpData[byMpId - 1].m_tMp.GetMulticast();
}

/*====================================================================
    ������      ��ShowDri
    ����        ����ʾDRI��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��u8 byDriId DRI���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/04/02    3.0         ������        ����
    06/11/09    4.0         ������        �޸�
====================================================================*/
void  CMcuVcData::ShowDri( u8 byDriId )
{	
	if( byDriId == 0 )
	{
		for( u8 byLoop = 1; byLoop <= MAXNUM_DRI; byLoop++ )
		{	        
			if( m_atMpData[byLoop-1].m_bConnected )
			{
				OspPrintf( TRUE, FALSE, "\n============= MP%d: TotalMtNum:%d ===========\n", 
                    byLoop, m_atMpData[byLoop-1].m_wMtNum );

                for( u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++ )
	            { 		
		            if( NULL != g_cMcuVcApp.GetConfInstHandle( byConfIdx ))
		            { 
                        TConfInfo  *ptConfFullInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
			            OspPrintf( TRUE, FALSE, "Conf: %s \n", ptConfFullInfo->GetConfName( ) );

                        for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
                        {
                            if (m_atMpData[byLoop-1].m_abyMtId[byConfIdx-1][byIdx] > 0)
                            {
                                OspPrintf(TRUE, FALSE, "Mt.%d ", m_atMpData[byLoop-1].m_abyMtId[byConfIdx-1][byIdx]);
                            }
                        }
                        OspPrintf( TRUE, FALSE, "\n");
                    }
                }                
			}
			else if( m_atMpData[byLoop-1].m_wMtNum )
			{
				OspPrintf( TRUE, FALSE, "MP%d: %d TotalMtNum:%d (Not Online)\n", byLoop, m_atMpData[byLoop-1].m_wMtNum );
			}
			
			if( m_atMtAdpData[byLoop-1].m_bConnected )
			{
				OspPrintf( TRUE, FALSE, "============== MtAdp%d: TotalMtNum:%d  ProtocolID:%d ==========\n", 
                            byLoop, m_atMtAdpData[byLoop-1].m_wMtNum, 
						    m_atMtAdpData[byLoop-1].m_byProtocolType );	

                for( u8 byConfIdx = MIN_CONFIDX; byConfIdx < MAX_CONFIDX; byConfIdx++ )
	            { 		
		            if( NULL != g_cMcuVcApp.GetConfInstHandle( byConfIdx ))
		            { 
                        TConfInfo  *ptConfFullInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
			            OspPrintf( TRUE, FALSE, "Conf: %s \n", ptConfFullInfo->GetConfName( ) );

                        for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
                        {
                            if (m_atMtAdpData[byLoop-1].m_abyMtId[byConfIdx-1][byIdx] > 0)
                            {
                                OspPrintf(TRUE, FALSE, "Mt.%d ", m_atMtAdpData[byLoop-1].m_abyMtId[byConfIdx-1][byIdx]);
                            }
                        }
                        OspPrintf( TRUE, FALSE, "\n");
                    }
                }                
			}
			else if( m_atMtAdpData[byLoop-1].m_wMtNum )
			{
				OspPrintf( TRUE, FALSE, "MtAdp%d: TotalMtNum:%d ProtocolID:%d (Not Online)\n", byLoop, 
						   m_atMtAdpData[byLoop-1].m_wMtNum, 
						   m_atMtAdpData[byLoop-1].m_byProtocolType );	
			}
		}
	}
	else if (byDriId <= MAXNUM_DRI)
	{
		if( m_atMpData[byDriId-1].m_bConnected )
		{
			OspPrintf( TRUE, FALSE, "\n\n=========== MP%d: TotalMtNum:%d =============\n", 
                        byDriId, m_atMpData[byDriId-1].m_wMtNum );
            
            for( u8 byConfIdx = MIN_CONFIDX; byConfIdx < MAX_CONFIDX; byConfIdx++ )
	        { 		
		        if( NULL != g_cMcuVcApp.GetConfInstHandle( byConfIdx ))
		        { 
                    TConfInfo  *ptConfFullInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
			        OspPrintf( TRUE, FALSE, "Conf: %s \n", ptConfFullInfo->GetConfName( ) );

                    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
                    {
                        if (m_atMpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx] > 0)
                        {
                            OspPrintf(TRUE, FALSE, "Mt.%d ", m_atMpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx]);
                        }
                    }
                    OspPrintf( TRUE, FALSE, "\n");
                }
            }            
		}
		else if( m_atMpData[byDriId-1].m_wMtNum )
		{
			OspPrintf( TRUE, FALSE, "MP%d: TotalMtNum:%d (Not Online)\n", byDriId, m_atMpData[byDriId-1].m_wMtNum );
		}
		
		if( m_atMtAdpData[byDriId-1].m_bConnected )
		{
			OspPrintf( TRUE, FALSE, "============ MtAdp%d: TotalMtNum:%d ProtocolID:%d ============\n", 
                        byDriId, m_atMtAdpData[byDriId-1].m_wMtNum, 
					    m_atMtAdpData[byDriId-1].m_byProtocolType );	

            for( u8 byConfIdx = MIN_CONFIDX; byConfIdx < MAX_CONFIDX; byConfIdx++ )
	        { 		
		        if( NULL != g_cMcuVcApp.GetConfInstHandle( byConfIdx ))
		        { 
                    TConfInfo  *ptConfFullInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
			        OspPrintf( TRUE, FALSE, "Conf: %s \n", ptConfFullInfo->GetConfName( ) );

                    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
                    {
                        if (m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx] > 0)
                        {
                            OspPrintf(TRUE, FALSE, "Mt.%d ", m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx]);
                        }
                    }
                    OspPrintf( TRUE, FALSE, "\n");
                }
            }            
		}
		else if( m_atMtAdpData[byDriId-1].m_wMtNum )
		{
			OspPrintf( TRUE, FALSE, "MtAdp%d: TotalMtNum:%d ProtocolID:%d (Not Online)\n", byDriId, 
					   m_atMtAdpData[byDriId-1].m_wMtNum, 
					   m_atMtAdpData[byDriId-1].m_byProtocolType );	
		}
	}
}

/*====================================================================
    ������      ��IncMtAdpMtNum
    ����        ������DRI�ն���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��u8 byDriId DRI���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/04/02    3.0         ������        ����
====================================================================*/
void CMcuVcData::IncMtAdpMtNum( u8 byDriId, u8 byConfIdx, u8 byMtId )
{
	if( 0 == byDriId || byDriId > MAXNUM_DRI || 0 == byMtId || 0 == byConfIdx || byConfIdx > MAX_CONFIDX )	
	{
        return;		
	}
	// xliang [1/4/2009] ���޷Ƕ�ֵ MAXNUM_CONF_MT
	u8 byDriMaxNum = m_atMtAdpData[byDriId-1].m_byMaxMtNum;
	
    //�Ȳ����Ƿ��Ѿ����ڣ�����¼��λ��
    u8 byEmptyIdx = byDriMaxNum/*MAXNUM_CONF_MT*/;
    for(u8 byIdx = 0; byIdx < byDriMaxNum; byIdx++)
    {      
        if ( m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx] == byMtId )
        {
            return;
        }

        if (0 == m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx] && byDriMaxNum == byEmptyIdx)
        {
            byEmptyIdx = byIdx;
        }
    }

    //������, ����mtid��¼�����Ӽ���
    if (byEmptyIdx < byDriMaxNum)
    {
        m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byEmptyIdx] = byMtId;
        m_atMtAdpData[byDriId-1].m_wMtNum++;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[IncMtAdpMtNum] ConfIdx.%d MtNum.%d is full.\n", 
            byConfIdx, m_atMtAdpData[byDriId-1].m_wMtNum);
    }
    return;
}

/*====================================================================
    ������      ��DecMtAdpMtNum
    ����        ������DRI�ն���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bHDMt			[in] ��HD�ն� 
				  BOOL32 bOnlyAlterNum	[in] ������������Ŀ
    ����ֵ˵��  ��u8 byDriId DRI���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/04/02    3.0         ������        ����
	1/4/2009				Ѧ��		  �޸�	 
====================================================================*/
void CMcuVcData::DecMtAdpMtNum( u8 byDriId, u8 byConfIdx, u8 byMtId, BOOL32 bHDMt, BOOL32 bOnlyAlterNum)
{
    if( 0 == byDriId || byDriId > MAXNUM_DRI || 0 == byMtId || 0 == byConfIdx || byConfIdx > MAX_CONFIDX ||
        0 == m_atMtAdpData[byDriId-1].m_wMtNum)	
	{
        return;		
	}    
    if(bOnlyAlterNum)
	{
		m_atMtAdpData[byDriId-1].m_wMtNum--;
		return;
	}
    //�Ȳ����Ƿ��Ѿ�����
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
    {      
        if ( m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx] == byMtId )
        {
            m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx] = 0;
            m_atMtAdpData[byDriId-1].m_wMtNum--;
#ifdef _LINUX_
			// xliang [10/27/2008] �����HD��MT����mcu���ý����
			if(bHDMt)
			{
				TMtAdpHDChnnlInfo tHdChnnlInfo;
				tHdChnnlInfo.SetConfIdx(byConfIdx);
				tHdChnnlInfo.SetHDMtId(byMtId);

				for(u8 byIndex=0; byIndex<MAXHDLIMIT_MPC_MTADP; byIndex ++)
				{
					if(m_atMtAdpData[byDriId-1].m_atHdChnnlInfo[byIndex] == tHdChnnlInfo)
					{
						m_atMtAdpData[byDriId-1].m_atHdChnnlInfo[byIndex].SetNull();
						break;
					}
				}
			}
#endif
            return;
        }        
    }   
    return;	
}

/*====================================================================
    ������      ��IncMpMtNum
    ����        ������MP�ն���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wConfBR: ��������
    ����ֵ˵��  ��BOOL32
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/04/02    3.0         ������        ����
    07/02/09    4.0         �ű���        ����MP��Pk���Ʋ���  
====================================================================*/
BOOL32 CMcuVcData::IncMpMtNum( u8 byMpId, u8 byConfIdx, u8 byMtId, u16 wConfBR )
{
    // guzh [6/14/2007] ����Ƶ�����������Ϊ0��ҲӦ���ܷ���
    if( 0 == byMpId || byMpId > MAXNUM_DRI || 0 == byMtId || 0 == byConfIdx || byConfIdx > MAX_CONFIDX )	
	{
        OspPrintf( TRUE, FALSE, "[IncMpMtNum] param err: byMpId.%d, byMtId.%d, byConfIdx.%d, wConfBR.%d !\n",
                                 byMpId, byMtId, byConfIdx, wConfBR );
        return FALSE;		
	}

    //�Ȳ����Ƿ��Ѿ����ڣ�����¼��λ��
    u8 byEmptyIdx = MAXNUM_CONF_MT;
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
    {      
        if ( m_atMpData[byMpId-1].m_abyMtId[byConfIdx-1][byIdx] == byMtId )
        {
			//zbq[09/11/2008]���Ѿ����ڵ��ظ���ӣ�Ӧ����ΪOK
            //return FALSE;
			return TRUE;
        }

        if (0 == m_atMpData[byMpId-1].m_abyMtId[byConfIdx-1][byIdx] && MAXNUM_CONF_MT == byEmptyIdx)
        {
            byEmptyIdx = byIdx;
        }
    }

    //������, ����mtid��¼�����Ӽ���
    if (byEmptyIdx < MAXNUM_CONF_MT)
    {
        // ���Ӹ�MP�ϸ��ص�Pk��, 1M���¸��ذ�1M���� [07/02/09-zbq]
        m_atMpData[byMpId-1].m_dwPkNumReal += wConfBR >= 1024 ? wConfBR : 1024;
        m_atMpData[byMpId-1].m_abyMtId[byConfIdx-1][byEmptyIdx] = byMtId;
        m_atMpData[byMpId-1].m_wMtNum++;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[IncMpMtNum] ConfIdx.%d MtNum.%d is full.\n", 
                                byConfIdx, m_atMpData[byMpId-1].m_wMtNum);
        return FALSE;
    }    
    return TRUE;
}


/*====================================================================
    ������      ��DecMpMtNum
    ����        ������MP�ն���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��u8 byMpId MP���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/04/02    3.0         ������        ����
====================================================================*/
void CMcuVcData::DecMpMtNum( u8 byMpId, u8 byConfIdx, u8 byMtId, u16 wConfBR )
{
	if( 0 == byMpId || byMpId > MAXNUM_DRI || 0 == byMtId || 0 == byConfIdx || byConfIdx > MAX_CONFIDX ||
        0 == m_atMpData[byMpId-1].m_wMtNum)	
	{
        return;		
	}

    //�Ȳ����Ƿ��Ѿ�����
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
    {      
        if ( m_atMpData[byMpId-1].m_abyMtId[byConfIdx-1][byIdx] == byMtId )
        {
            m_atMpData[byMpId-1].m_abyMtId[byConfIdx-1][byIdx] = 0;
            m_atMpData[byMpId-1].m_wMtNum--;
            m_atMpData[byMpId-1].m_dwPkNumReal -= wConfBR >= 1024 ? wConfBR : 1024;
            return;
        }        
    }   
    return;	
}

/*====================================================================
    ������      ��AssignMpIdByBurden
    ����        �����ݸ��ط���MpId�����Ȳ�ָ�����ñ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8        byConfIdx:
                  TMtAlias &tMtAlias : ����������IP��E164+IP
                  u8        byMtId   :
                  u16       wConfBR : ��������
    ����ֵ˵��  ��Mp��Id 
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
    06/12/29    4.0         guzh          ���ӱ������룬����������IP��E164+IP
    07/02/12    4.0         �ű���        �� ��æָ�� ���¹滮���ؾ���
====================================================================*/
u8 CMcuVcData::AssignMpIdByBurden( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId, u16 wConfBR )
{
    // guzh [12/29/2006] ���ȳ��Դ������ļ������ȡ
    u32 dwMtadpIpAddr = 0;
    u32 dwMpIpAddr = 0;
    
    u8 byMpId = 0;

    BOOL32 bFoundInTable = FALSE;
    if ( tMtAlias.m_AliasType == mtAliasTypeE164 ) 
    {
        // ����ƥ��E164
        if ( g_cMcuVcApp.GetMpIdAndH323MtDriIdFromMtAlias(tMtAlias, dwMtadpIpAddr, dwMpIpAddr) )
        {
            bFoundInTable = TRUE;

            byMpId = g_cMcuVcApp.FindMp( dwMpIpAddr );             
        }
        else
        {
            // ��IP����
            tMtAlias.m_AliasType = mtAliasTypeTransportAddress;
        }
    }
    if (!bFoundInTable)
    {
        // ��ƥ��IP        
        if ( g_cMcuVcApp.GetMpIdAndH323MtDriIdFromMtAlias(tMtAlias, dwMtadpIpAddr, dwMpIpAddr) )
        {
            bFoundInTable = TRUE;

            byMpId = g_cMcuVcApp.FindMp( dwMpIpAddr );             
        }
    }   
    
    // �û�û��ָ��, Ѱ�� ��ǰ�����е�MP
    if (!bFoundInTable)
    {
        // ��æָ�����ܳ���100%(��������ת)�������һ����ȫ����
        u8  byLeastBusyPercent = 200;
        u8  byMaxPkLeftNO = 0;
        
        for(u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++ )
        {
            if(m_atMpData[byLoop].m_bConnected)
            {
                // ��æָ����С �� ��Pk���ܷ�Χ֮���� ��֮
                if ( m_atMpData[byLoop].GetCurBusyPercent() < byLeastBusyPercent &&
                     m_atMpData[byLoop].IsThePkSupport( wConfBR ) ) 
                {
                    byMaxPkLeftNO = byLoop;
                    byLeastBusyPercent = m_atMpData[byLoop].GetCurBusyPercent();
                }
            }
        }
        byMpId = byMaxPkLeftNO+1;
    }	

    if ( byMpId != 0 && m_atMpData[byMpId-1].m_bConnected )
    {
        if ( !IncMpMtNum(byMpId, byConfIdx, byMtId, wConfBR) ) 
        {
            byMpId = 0;
        }
    }
    else
    {
        byMpId = 0;
    }

	return byMpId;
}

/*====================================================================
    ������      ��AssignH323MtDriId
    ����        �������ն˵�ַ����H323�����õ�DRI�� 
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId
    ����ֵ˵��  �����ص������õ�DRI��ID 
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
	07/01/08    4.0         �ܹ��		  �޸�
====================================================================*/
u8 CMcuVcData::AssignH323MtDriId(u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId)
{
	u8 byDriId = 0;	
    
    u32 dwMtadpIpAddr = 0;
    u32 dwMpIpAddr = 0;

    if( 0 == byMtId || 0 == byConfIdx || byConfIdx > MAX_CONFIDX )
    {
        byDriId = 0;
        OspPrintf( TRUE, FALSE, "[AssignH323MtDriId] assign mtadp id, param byConfIdx.%d error! \n", byConfIdx );
        return byDriId;
    }

    // find the conf
    TConfInfo *ptConf = NULL;
    if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return FALSE;
    }
    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if(tMapData.IsValidConf())
    {            
        ptConf = &m_apConfInst[tMapData.GetInsId()-1]->m_tConf;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[AssignH323MtDriId] can't get confinfo.%d\n", byConfIdx);
        return FALSE;
    }
    BOOL bConfHD = CMcuPfmLmt::IsConfFormatHD(*ptConf);

    u8 byMtNum = 0;
    u8 bySMcuNum = 0;

	// xliang [12/17/2008] ������HD���������Ľ������Ŀ��
	// ��HDI�����Ŀ����Ƕ����壬CRI�������SupportHD��
	// xliang [2/27/2009] �����������HDI�壬����Ȩ����0���򵱸�HDI�岻����

	s32 nHdMtAdpNum = 0;
	for( u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if( g_cMcuVcApp.m_atMtAdpData[byLoop].m_bConnected 
			&& g_cMcuVcApp.m_atMtAdpData[byLoop].m_byIsSupportHD
			&& g_cMcuVcApp.m_atMtAdpData[byLoop].m_byMaxMtNum >0)
		{
			nHdMtAdpNum++;
		}		
	}
	CallLog("The num of access boards supported HD is %d\n", nHdMtAdpNum);
	
	BOOL32 bNeglectedForLowPri = FALSE; // ��ʱ������Ƕ�����ķ��䣬���ȷ��䵽HDI��

    if (GetMpIdAndH323MtDriIdFromMtAlias(tMtAlias, dwMtadpIpAddr, dwMpIpAddr))
    {
        byDriId = FindMtAdp( dwMtadpIpAddr, PROTOCOL_TYPE_H323 );

        // ����ʱ,��������ն�,�������Ƿ�����.
        if ( //GetPerfLimit().IsLimitMtNum() &&
             //zbq[01/09/2008]�����ж��ն˷ֲ�
             GetMtNumOnDri(byDriId, FALSE, byMtNum, bySMcuNum) &&
             GetPerfLimit().IsLimitMtNum() )
        {
            byDriId = 0;
        }

        //guzh [2008/03/27] ����������ơ�8000Bͨ��BrdID���ƣ���8000B-HD�����������       
#ifndef _MINIMCU_
        if ( bConfHD && !m_atMtAdpData[byDriId-1].m_byIsSupportHD )
        {
            // xliang [12/16/2008] ������Ƕ����HD�ն����������й���
			BOOL32 bEmbededAccessHdMtFull = TRUE; //��Ƕ����HD�ն������Ƿ�����
			u8 byIdx = 0;
			for(byIdx = 0; byIdx < MAXHDLIMIT_MPC_MTADP; byIdx ++)
			{
				if(g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_atHdChnnlInfo[byIdx].GetHDMtId() == 0)
				{
					bEmbededAccessHdMtFull = FALSE;
					break;
				}
			}

			//zbq[10/28/2008] �����ն˽��������ſ�
            if ((byDriId == MCU_BOARD_MPC ||
				byDriId == MCU_BOARD_MPCD) &&
				!bEmbededAccessHdMtFull)// xliang [12/16/2008] ��Ƕ����HD�ն�����δ���ŷŹ�
            {
                CallLog("[AssignH323MtDriId] User specified dri.%d is mpc!\n", byDriId);
            }
			else if(bEmbededAccessHdMtFull && nHdMtAdpNum == 0)
			{
				CallLog("[AssignH323MtDriId] No HDI��Auto specified dri.%d is mpc!\n",byDriId);
			}
            else
            {
                CallLog("[AssignH323MtDriId] User specified dri.%d is not HDI.", byDriId);
                byDriId = 0;
            }
        }            
#endif

    }
    else
    {
        u8 byMinDriNO = 0;
        u8 byMinMtNum = MAXNUM_DRI_MT;
		u8 byMaxMtNumLeft = 0;	// xliang [2/16/2009] �����������ʣ����
        for(u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++ )
        {
            if ( //GetPerfLimit().IsLimitMtNum() &&
                 //zbq[01/09/2008]�����ж��ն˷ֲ�
                 GetMtNumOnDri(byLoop+1, FALSE, byMtNum, bySMcuNum) &&
                 GetPerfLimit().IsLimitMtNum() )
            {
                // ������������
                continue;
            }

            if( m_atMtAdpData[byLoop].m_bConnected && 
                PROTOCOL_TYPE_H323 == m_atMtAdpData[byLoop].m_byProtocolType )
            {
                //guzh [2008/03/27] ����������ơ�8000Bͨ��BrdID���ƣ���8000B-HD�����������
#ifndef _MINIMCU_
                if ( bConfHD && !m_atMtAdpData[byLoop].m_byIsSupportHD )
                {
					// xliang [2/27/2009] ֻҪ�Ǹ�����������HDI�壨�����HDI��Ļ�����
					//������ǶHD���������Ƿ�����
                    if ((byLoop+1 == MCU_BOARD_MPC 
                        || byLoop+1 == MCU_BOARD_MPCD) 
						&& nHdMtAdpNum > 0 
						) 
					{
						//��Ǵ˴�"��ʱ����"
						bNeglectedForLowPri = TRUE;
						continue;
					} 
                }
#endif
                /*if( byMtNum < byMinMtNum )
                {
                    byMinDriNO = byLoop;
                    byMinMtNum = byMtNum;
                }*/
				// xliang [2/16/2009] ѡʣ�������������һ��
				u8 byMtNumLeft = m_atMtAdpData[byLoop].m_byMaxMtNum - byMtNum;
				if( byMtNumLeft > byMaxMtNumLeft)
				{
					byMaxMtNumLeft = byMtNumLeft;
					byMinDriNO = byLoop;
				}
            }
        }
        
        //if( byMinMtNum == MAXNUM_DRI_MT )
        if( byMaxMtNumLeft == 0 ) // xliang [2/16/2009] 
        {
            if(bNeglectedForLowPri) // xliang [1/15/2009] ֮ǰ������Ƕ����壬�����ٻع�ͷ���Է���
			{
				CallLog("[AssignH323MtDriId]Try to assign mt to embeded dri which was neglected before!\n");
				
				if ( !GetMtNumOnDri(MCU_BOARD_MPC, FALSE, byMtNum, bySMcuNum) 
					||!GetPerfLimit().IsLimitMtNum() )
				{
					byDriId = MCU_BOARD_MPC;
				}
				else
				{
					OspPrintf( TRUE, FALSE, "[AssignH323MtDriId] fail to assign mtadp id, All DRI full! \n" );
					byDriId = 0;
				}
			}
			else
			{
				OspPrintf( TRUE, FALSE, "[AssignH323MtDriId] fail to assign mtadp id, All DRI full! \n" );
				byDriId = 0;
			}
        }   
        else
        {
            byDriId = byMinDriNO + 1;            
        }
    }
    

    if ( byDriId > 0 )
    {
        IncMtAdpMtNum(byDriId, byConfIdx, byMtId);
    }
   
	return byDriId;
}

/*====================================================================
    ������      ��AssignMpIdAndH320MtDriId
    ����        �������ն�Ip��ַ���佻�����ݵ�mp���Լ�H320�����õ�DRI�� 
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byMpId ���ص�mp��ID
    ����ֵ˵��  �����ص������õ�DRI��ID 
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
u8 CMcuVcData::AssignH320MtDriId( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId )
{
	u8 byDriId = 0;    
	u8 byLayer   = 0; 
	u8 bySlot    = 0;
	u8 byChannel = 0;
	u32 dwDriNo  = 0;
	tMtAlias.GetH320Alias( byLayer, bySlot, byChannel );
	dwDriNo  = byLayer << 4;
	dwDriNo += bySlot;
	if( dwDriNo <= MAXNUM_DRI_MT )
	{
		byDriId = (u8)(dwDriNo+1);
		if( m_atMtAdpData[byDriId-1].m_bConnected && 
			PROTOCOL_TYPE_H320 == m_atMtAdpData[byDriId-1].m_byProtocolType)
		{
			if( (byChannel+1) > m_atMtAdpData[byDriId-1].m_byMaxMtNum )
			{
				byDriId = 0;
				OspPrintf( TRUE, FALSE, "[AssignMpIdAndH320MtDriId] invalid Channel! \n" );	
			}
			else
			{
				IncMtAdpMtNum(byDriId, byConfIdx, byMtId);
			}
		}
	}
	else
	{
		OspPrintf( TRUE, FALSE, "[AssignH320MtDriId] invalid dri! \n" );
	}	
    
	return byDriId;
}

/*=============================================================================
�� �� ���� IsMtAssignInDri
��    �ܣ� �ն��Ƿ��Ѿ������ڴ�mtadp��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u8 byDriId
           u8 byConfIdx
           u8 byMtId
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/13  4.0			������                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsMtAssignInDri( u8 byDriId, u8 byConfIdx, u8 byMtId )
{
    if (0 == byDriId || byDriId > MAXNUM_DRI || byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return FALSE;
    }

    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
    {
        if (m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx] == byMtId)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
�� �� ���� IsMtAssignInMp
��    �ܣ� �ն��Ƿ��Ѿ������ڴ�mp��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u8 byMpId
           u8 byConfIdx
           u8 byMtId
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/13  4.0			������                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsMtAssignInMp( u8 byMpId, u8 byConfIdx, u8 byMtId )
{
    if (0 == byMpId || byMpId > MAXNUM_DRI || byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return FALSE;
    }

    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
    {
        if (m_atMpData[byMpId-1].m_abyMtId[byConfIdx-1][byIdx] == byMtId)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*====================================================================
    ������      ��GetRegConfDriId
    ����        ���õ�ע������MtAdp��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��MtAdpId ID
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
u8 CMcuVcData::GetRegConfDriId( void )
{
	u8  byDriId  = 0;
	int nMtAdpId = 0;
	u8  byRegNum = MAXNUM_MCU_CONF;
	while( nMtAdpId < MAXNUM_DRI )
	{
		//����δ���ӵ�
		if( !m_atMtAdpData[nMtAdpId].m_bConnected || 
			PROTOCOL_TYPE_H323 == m_atMtAdpData[nMtAdpId].m_byProtocolType )
		{
			nMtAdpId++;
		}
		else
		{
			if( m_atMtAdpData[nMtAdpId].m_byRegGKConfNum < byRegNum )
			{
				byRegNum  = m_atMtAdpData[nMtAdpId].m_byRegGKConfNum;
				byDriId = nMtAdpId+1;
			}
			nMtAdpId++;
		}
	}

	return byDriId<=MAXNUM_DRI ? byDriId : 0;	
}

/*====================================================================
    ������      ChangeDriRegConfNum
    ����        ���ı�Dri��ע��GK�Ļ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��Mp������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
void CMcuVcData::ChangeDriRegConfNum( u8 byDriId, BOOL32 bInc  )
{
    if( byDriId >= MAXNUM_DRI )
	{
		return;
	}

	if( bInc )
	{
		m_atMtAdpData[byDriId].m_byRegGKConfNum++;
	}
	else
	{
		m_atMtAdpData[byDriId].m_byRegGKConfNum--;
	}
}

/*=============================================================================
    �� �� ���� RegisterConfToGK
    ��    �ܣ� ��MCU���߻���ʵ��ע�ᵽGK��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byConfIdx
               u8 byDriId
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/6    3.6			����                  ����
=============================================================================*/
void CMcuVcData::RegisterConfToGK(u8 byConfIdx, u8 byDriId, BOOL32 bTemplate, BOOL32 bUnReg)
{
    TMtAlias tMcuH323Id;
    TMtAlias tMcuE164;
    CServMsg cServMsg;

    if (0 == g_cMcuAgent.GetGkIpAddr() || 
        0 == byDriId || 
        byDriId > MAXNUM_DRI)
    {
        return;
    }
    if (0 == byConfIdx)
    {
        s8 achMcuE164[MAXLEN_ALIAS];
        s8 achMcuH323Id[MAXLEN_ALIAS];
        memset(achMcuE164, 0, MAXLEN_ALIAS);
        memset(achMcuH323Id, 0, MAXLEN_ALIAS);
        g_cMcuAgent.GetE164Number(achMcuE164, MAXLEN_ALIAS);
        g_cMcuAgent.GetMcuAlias(achMcuH323Id, MAXLEN_ALIAS);
        tMcuE164.SetE164Alias(achMcuE164);
        tMcuH323Id.SetH323Alias(achMcuH323Id);
        cServMsg.SetMcuId(LOCAL_MCUID);
        cServMsg.SetConfIdx(0);
        cServMsg.SetDstDriId(byDriId);
        cServMsg.SetMsgBody((u8*)&tMcuE164, sizeof(TMtAlias));
        cServMsg.CatMsgBody((u8*)&tMcuH323Id, sizeof(TMtAlias));
    }
    else
    {		
        TConfInfo *ptConf = NULL;
        if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
        {
            return;
        }
        TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
        if(bTemplate && tMapData.IsTemUsed())
        {           
            ptConf = &m_ptTemplateInfo[tMapData.GetTemIndex()].m_tConfInfo;
        }
        else if(tMapData.IsValidConf())
        {            
            ptConf = &m_apConfInst[tMapData.GetInsId()-1]->m_tConf;
        }
        else
        {
            OspPrintf(TRUE, FALSE, "[RegisterConfToGK] can't get confinfo for register\n");
            return;
        }
        
        tMcuE164.SetE164Alias(ptConf->GetConfE164());
        cServMsg.SetConfId(ptConf->GetConfId());
        cServMsg.SetConfIdx(byConfIdx);
        cServMsg.SetDstDriId(byDriId);
        cServMsg.SetMsgBody((u8*)&tMcuE164, sizeof(TMtAlias));
        cServMsg.CatMsgBody((u8*)&tMcuH323Id, sizeof(TMtAlias));
    }

    //Э�������ַ
    BOOL32 bHasMtadp = FALSE;
    TTransportAddr tMtadpAddr;
    for (u8 byIdx = 0; byIdx < MAXNUM_DRI; byIdx++)
    {
        if (m_atMtAdpData[byIdx].m_bConnected && m_atMtAdpData[byIdx].m_dwIpAddr != 0)
        {
            tMtadpAddr.SetNetSeqIpAddr(m_atMtAdpData[byIdx].m_dwIpAddr);            
            tMtadpAddr.SetPort(m_atMtAdpData[byIdx].m_wQ931Port);
            cServMsg.CatMsgBody((u8*)&tMtadpAddr, sizeof(tMtadpAddr));
            bHasMtadp = TRUE;
        }
    }
    if (!bHasMtadp)
    {
        OspPrintf(TRUE, FALSE, "[RegisterConfToGK] no online mtadp for register\n");
        return;
    }
    
    if(bUnReg)
    {
        g_cMtAdpSsnApp.SendMsgToMtAdpSsn(byDriId, MCU_MT_UNREGISTERGK_REQ, cServMsg);
    }
    else
    {
        g_cMtAdpSsnApp.SendMsgToMtAdpSsn(byDriId, MCU_MT_REGISTERGK_REQ, cServMsg);
    }    

    return;
}


/*=============================================================================
    �� �� ���� ConfChargeByGK
    ��    �ܣ� ����Ʒ�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byConfIdx
               u8 byDriId
               BOOL32 bStopCharge = FALSE
               u8 byCreateBy = CONF_CREATE_MCS
               u8 byMtNum
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/11/09  4.0			�ű���                  ����
=============================================================================*/
void CMcuVcData::ConfChargeByGK( u8 byConfIdx, u8 byDriId, BOOL32 bStopCharge, u8 byCreateBy, u8 byMtNum )
{
    TConfInfo *ptConf = NULL;
    if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return;
    }
    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if(tMapData.IsValidConf())
    {            
        ptConf = &m_apConfInst[tMapData.GetInsId()-1]->m_tConf;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[ConfChargeByGK] can't get confinfo for charge\n");
        return;
    }

    CServMsg cServMsg;
    cServMsg.SetConfId(ptConf->GetConfId());
    cServMsg.SetConfIdx(byConfIdx);
    cServMsg.SetDstDriId(byDriId);

    if ( !bStopCharge )
    {
        TNetWorkInfo tNetInfo;
        g_cMcuAgent.GetNetWorkInfo( &tNetInfo );

        TConfChargeInfo tChargeInfo;
        tChargeInfo.SetGKUsrName( ptConf->GetConfAttrbEx().GetGKUsrName() );
        tChargeInfo.SetGKPwd( ptConf->GetConfAttrbEx().GetGKPwd() );

        if ( CONF_CREATE_NPLUS == byCreateBy )
        {
            tChargeInfo.SetConfName( ptConf->GetConfName() );
            tChargeInfo.SetConfE164( ptConf->GetConfE164() );
            // GK�Ʒ����ջ���ȫ���ʣ�VID+AUD��, zgc, 2008-09-23
            //tChargeInfo.SetBitRate( ptConf->GetBitRate() );
            tChargeInfo.SetBitRate( ptConf->GetBitRate() + GetAudioBitrate(ptConf->GetMainAudioMediaType()) );
            
            BOOL32 bDiscussConf = FALSE;
            bDiscussConf = ptConf->GetConfAttrb().IsDiscussConf();
            tChargeInfo.SetMixerNum( bDiscussConf ? 1 : 0 );
            
            tChargeInfo.SetTerNum( byMtNum );
            
            BOOL32 bVMPAuto = ptConf->m_tStatus.GetVmpParam().IsVMPAuto();
            tChargeInfo.SetVMPNum( bVMPAuto ? 1 : min(16, byMtNum) );
            tChargeInfo.SetBasNum( 0 != ptConf->GetSecBitRate() ? 1 : 0 );  //����������
            
            BOOL32 bDataConf = FALSE;
            u8 byMode = ptConf->GetConfAttrb().GetDataMode();
            if ( CONF_DATAMODE_VAANDDATA == byMode ||
                CONF_DATAMODE_DATAONLY  == byMode )
            {
                bDataConf = TRUE;
            }
            tChargeInfo.SetIsUseDataConf( bDataConf );
            
            BOOL32 bDualVideo = FALSE;
            byMode = ptConf->GetConfAttrb().GetDualMode();
            if ( CONF_DUALMODE_SPEAKERONLY  == byMode ||
                CONF_DUALMODE_EACHMTATWILL == byMode )
            {
                bDualVideo = TRUE;
            }
            tChargeInfo.SetIsUseDualVideo( bDualVideo );
            
            BOOL32 bEncrypt = FALSE;
            byMode = ptConf->GetConfAttrb().GetEncryptMode();
            if ( CONF_ENCRYPTMODE_NONE != byMode )
            {
                bEncrypt = TRUE;
            }
            tChargeInfo.SetIsUseEncrypt( bEncrypt );
            tChargeInfo.SetIsUseCascade( ptConf->GetConfAttrb().IsSupportCascade() );
            tChargeInfo.SetIsUseStreamBroadCast( ptConf->GetConfAttrb().IsMulticastMode() );
        }
        else
        {            
            TTemplateInfo tTemInfo;
            GetTemplate( byConfIdx, tTemInfo );
       
            tChargeInfo.SetConfName( tTemInfo.m_tConfInfo.GetConfName() );
            tChargeInfo.SetConfE164( tTemInfo.m_tConfInfo.GetConfE164() );
            // GK�Ʒ����ջ���ȫ���ʣ�VID+AUD��, zgc, 2008-09-23
            //tChargeInfo.SetBitRate( tTemInfo.m_tConfInfo.GetBitRate() );
            tChargeInfo.SetBitRate( tTemInfo.m_tConfInfo.GetBitRate() + GetAudioBitrate(tTemInfo.m_tConfInfo.GetMainAudioMediaType()) );
            
            BOOL32 bDiscussConf = FALSE;
            bDiscussConf = tTemInfo.m_tConfInfo.GetConfAttrb().IsDiscussConf();
            tChargeInfo.SetMixerNum( bDiscussConf ? 1 : 0 );
            
            tChargeInfo.SetTerNum( tTemInfo.m_byMtNum );
            
            BOOL32 bVMPAuto = tTemInfo.m_atVmpModule.GetVmpParam().IsVMPAuto();
            tChargeInfo.SetVMPNum( bVMPAuto ? 1 : min(16, tTemInfo.m_byMtNum) );
            tChargeInfo.SetBasNum( 0 != tTemInfo.m_tConfInfo.GetSecBitRate() ? 1 : 0 );  //����������
            
            BOOL32 bDataConf = FALSE;
            u8 byMode = tTemInfo.m_tConfInfo.GetConfAttrb().GetDataMode();
            if ( CONF_DATAMODE_VAANDDATA == byMode ||
                CONF_DATAMODE_DATAONLY  == byMode )
            {
                bDataConf = TRUE;
            }
            tChargeInfo.SetIsUseDataConf( bDataConf );
            
            BOOL32 bDualVideo = FALSE;
            byMode = tTemInfo.m_tConfInfo.GetConfAttrb().GetDualMode();
            if ( CONF_DUALMODE_SPEAKERONLY  == byMode ||
                CONF_DUALMODE_EACHMTATWILL == byMode )
            {
                bDualVideo = TRUE;
            }
            tChargeInfo.SetIsUseDualVideo( bDualVideo );
            
            BOOL32 bEncrypt = FALSE;
            byMode = tTemInfo.m_tConfInfo.GetConfAttrb().GetEncryptMode();
            if ( CONF_ENCRYPTMODE_NONE != byMode )
            {
                bEncrypt = TRUE;
            }
            tChargeInfo.SetIsUseEncrypt( bEncrypt );
            tChargeInfo.SetIsUseCascade( tTemInfo.m_tConfInfo.GetConfAttrb().IsSupportCascade() );
            tChargeInfo.SetIsUseStreamBroadCast( tTemInfo.m_tConfInfo.GetConfAttrb().IsMulticastMode() );
        }
        cServMsg.SetMsgBody( (u8*)&tChargeInfo, sizeof(tChargeInfo) );
        g_cMtAdpSsnApp.SendMsgToMtAdpSsn(byDriId, MCU_MT_CONF_STARTCHARGE_REQ, cServMsg);
    }
    else
    {
        g_cMtAdpSsnApp.SendMsgToMtAdpSsn(byDriId, MCU_MT_CONF_STOPCHARGE_REQ, cServMsg);
    }

    return;
}

/*====================================================================
    ������      ��AddMtAdp
    ����        ������һ��MtAdp
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tMtAdpReg MtAdp��Ϣ
    ����ֵ˵��  ���ɹ�TRUE,ʧ��FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
    08/03/27    4.5         ����        ����HDI֧��
====================================================================*/
void CMcuVcData::AddMtAdp( TMtAdpReg &tMtAdpReg )
{
    m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_bConnected     = TRUE;
	m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_byMtAdpId      = tMtAdpReg.GetDriId(); 
	m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_dwIpAddr       = htonl( tMtAdpReg.GetIpAddr() );
	m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_byProtocolType = tMtAdpReg.GetProtocolType(); 	
    m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_wQ931Port      = tMtAdpReg.GetQ931Port();
    m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_wRasPort       = tMtAdpReg.GetRasPort();
    m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_byIsSupportHD  = tMtAdpReg.IsSupportHD() ? 1: 0;

    // guzh [1/17/2007] �ۺϿ���MtAdp������ҵ������
    u8 byMaxMtNum = tMtAdpReg.GetMaxMtNum();
    u8 byMaxSMcuNum = MAXLIMIT_CRI_MTADP_SMCU;
    TMcuPerfLimit tLimit = GetPerfLimit ();
    if ( tLimit.IsLimitMtNum() )
    {
        if ( tMtAdpReg.GetAttachMode() == 1)
        {
            byMaxMtNum = min ( byMaxMtNum, tLimit.m_byMpcMaxMtAdpConnMtAbility );
            byMaxSMcuNum = tLimit.m_byMpcMaxMtAdpConnSMcuAbility;
        }
        else
        {
#ifndef _MINIMCU_
//             if (tMtAdpReg.IsSupportHD())
//             {
//                 byMaxMtNum = min ( byMaxMtNum, tLimit.m_byHdiMaxMtAdpConnMtAbility );
//             }
//             else
//             {
//                 byMaxMtNum = min ( byMaxMtNum, tLimit.m_byTuiMaxMtAdpConnMtAbility );
//             }
			byMaxMtNum = min ( byMaxMtNum, MAXNUM_DRI_MT);
#else
            byMaxMtNum = min ( byMaxMtNum, tLimit.m_byTuiMaxMtAdpConnMtAbility );
#endif            
            byMaxSMcuNum = tLimit.m_byTuiMaxMtAdpConnSMcuAbility;
        }
    }
    m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_byMaxMtNum     = byMaxMtNum;
    m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_byMaxSMcuNum   = byMaxSMcuNum;
}

/*====================================================================
    ������      ��RemoveMtAdp
    ����        ���Ƴ�MtAdp
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byMtAdpId MtAdp���	          
    ����ֵ˵��  ���ɹ�TRUE,ʧ��FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
void CMcuVcData::RemoveMtAdp( u8 byMtAdpId )
{
    if (!IsMtAdpConnected(byMtAdpId))
    {
        return;
    }
    
    m_atMtAdpData[byMtAdpId - 1].m_bConnected     = FALSE;
	m_atMtAdpData[byMtAdpId - 1].m_byMtAdpId      = 0;
	m_atMtAdpData[byMtAdpId - 1].m_dwIpAddr       = 0;
	m_atMtAdpData[byMtAdpId - 1].m_byProtocolType = 0;
	m_atMtAdpData[byMtAdpId - 1].m_byMaxMtNum     = 0;
    m_atMtAdpData[byMtAdpId - 1].m_byMaxSMcuNum   = 0;
    m_atMtAdpData[byMtAdpId - 1].m_wQ931Port      = 0;
    m_atMtAdpData[byMtAdpId - 1].m_wRasPort       = 0;
    m_atMtAdpData[byMtAdpId - 1].m_byRegGKConfNum = 0;
    m_atMtAdpData[byMtAdpId - 1].m_wMtNum         = 0;
    memset(m_atMtAdpData[byMtAdpId - 1].m_abyMtId, 0, sizeof(m_atMtAdpData[byMtAdpId - 1].m_abyMtId));
}

/*====================================================================
    ������      ��IsMtAdpConnected
    ����        ���ж�MtAdp�Ƿ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byMtAdpId MtAdp���
    ����ֵ˵��  ��TRUE - ����, FALSE - δ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
BOOL32 CMcuVcData::IsMtAdpConnected( u8 byMtAdpId )
{
    if (0 == byMtAdpId || byMtAdpId > MAXNUM_DRI)
    {
        return FALSE;
    }

    return m_atMtAdpData[byMtAdpId - 1].m_bConnected;
}

/*=============================================================================
    �� �� ���� GetMtAdpProtocalType
    ��    �ܣ� ��ȡ�����֧�ֵ�Э������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byMtAdpId
    �� �� ֵ�� u8 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/1    3.5			����                  ����
=============================================================================*/
u8 CMcuVcData::GetMtAdpProtocalType( u8 byMtAdpId )
{
    if (!IsMtAdpConnected(byMtAdpId))
    {
        return 0;
    }

    return m_atMtAdpData[byMtAdpId - 1].m_byProtocolType;
}

/*=============================================================================
    �� �� ���� GetMtAdpSupportMtNum
    ��    �ܣ� ��ȡ�����֧�ֵ�������ͨ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byMtAdpId
    �� �� ֵ�� u8 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/1    3.5			����                  ����
=============================================================================*/
u8 CMcuVcData::GetMtAdpSupportMtNum( u8 byMtAdpId )
{
    if (!IsMtAdpConnected(byMtAdpId))
    {
        return 0;
    }

    return m_atMtAdpData[byMtAdpId - 1].m_byMaxMtNum;
}

/*=============================================================================
    �� �� ���� GetMtAdpSupportSMcuNum
    ��    �ܣ� ��ȡ�����֧�ֵ�����¼�MCU��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byMtAdpId
    �� �� ֵ�� u8 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/1/17   4.0			����                  ����
=============================================================================*/
u8 CMcuVcData::GetMtAdpSupportSMcuNum( u8 byMtAdpId )
{
    if (!IsMtAdpConnected(byMtAdpId))
    {
        return 0;
    }

    return m_atMtAdpData[byMtAdpId - 1].m_byMaxSMcuNum;
}

/*====================================================================
    ������      ��GetMtAdpIpAddr
    ����        ���õ�MtAdp��Ip��ַ 
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byMtAdpId MtAdp���
    ����ֵ˵��  ���ɹ�����IP��ַ, ʧ��0
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
u32 CMcuVcData::GetMtAdpIpAddr( u8 byMtAdpId )
{
    if (!IsMtAdpConnected(byMtAdpId))
    {
        return 0;
    }

	return ntohl( m_atMtAdpData[byMtAdpId - 1].m_dwIpAddr );
}

/*====================================================================
    ������      ��GetMtAdpNum
    ����        ���õ�MtAdp����Ŀ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byProtocolType = PROTOCOL_TYPE_H323 Э������
    ����ֵ˵��  ��MtAdp������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
u8 CMcuVcData::GetMtAdpNum( u8 byProtocolType )
{
	u8 byMpNum = 0;
    for( u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++ )
	{
        if( m_atMtAdpData[byLoop].m_bConnected && 
			m_atMtAdpData[byLoop].m_byProtocolType == byProtocolType )
		{
		    byMpNum++;
		}
	}
	return byMpNum;
}

/*====================================================================
    ������      ��FindMtAdp
    ����        ������MtAdp
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 dwIp ip��ַ��������
	              u8 byProtocolType = PROTOCOL_TYPE_H323 Э������
    ����ֵ˵��  ������MtAdp��Id
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
u8 CMcuVcData::FindMtAdp( u32 dwIp, u8 byProtocolType )
{
   u8 byMtAdpId = 0;
   int nMtAdpId = 0;
   while( nMtAdpId < MAXNUM_DRI && byMtAdpId == 0)
   {
	   //����δ���ӵ�
	   if(!m_atMtAdpData[nMtAdpId].m_bConnected)
	   {
		   nMtAdpId++;
		   continue;
	   }

	  //�����Ƿ��Ǵ�MtAdp
      if( m_atMtAdpData[nMtAdpId].m_dwIpAddr == htonl(dwIp) && 
		  m_atMtAdpData[nMtAdpId].m_byProtocolType == byProtocolType )
	  {
         byMtAdpId = nMtAdpId+1;
		 break;
	  }

	   nMtAdpId++;
   }

   return byMtAdpId;
}

/*====================================================================
    ������      ��InitPeriEqpList
    ����        ����ʼ�������б�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/21    3.0         zmy           ����
====================================================================*/
void CMcuVcData::InitPeriEqpList()
{
    u8  byEqpId;
    u32 dwIP;
    u8  byType;

    memset(m_atPeriEqpTable, 0, sizeof(m_atPeriEqpTable));

    for (byEqpId = 1; byEqpId < MAXNUM_MCU_PERIEQP; byEqpId++)
    {
        m_atPeriEqpTable[byEqpId-1].m_bIsValid = (SUCCESS_AGENT == g_cMcuAgent.GetPeriInfo(byEqpId, &dwIP, &byType));
        m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.SetMcuId(LOCAL_MCUID);
        m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.SetEqpId(byEqpId);
        m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.SetEqpType(byType);
        m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_byOnline = FALSE;
    }
}

/*====================================================================
    ������      ��IsPeriEqpValid
    ����        �������Ƿ���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/21    3.0         zmy           ����
====================================================================*/
BOOL32 CMcuVcData::IsPeriEqpValid( u8   byEqpId )
{
	return m_atPeriEqpTable[byEqpId-1].m_bIsValid;
}

/*=============================================================================
�� �� ���� SetPeriEqpIsValid
��    �ܣ� ����ָ��������Ч
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byEqpId
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/10/29   4.0		�ܹ��                  ����
=============================================================================*/
void CMcuVcData::SetPeriEqpIsValid( u8 byEqpId )
{
	m_atPeriEqpTable[byEqpId-1].m_bIsValid = TRUE;
}

/*====================================================================
    ������      ��GetEqp
    ����        ���õ�����ṹ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/12/04    3.0         ������           ����
====================================================================*/
TEqp CMcuVcData::GetEqp( u8 byEqpId )
{
	TEqp tEqp;
	tEqp.SetNull();
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || !IsPeriEqpValid( byEqpId ) )
	{
		return tEqp;
	}

	return (TEqp)m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus;
}

/*====================================================================
    ������      ��GetEqpType
    ����        ���õ���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/12/04    3.0         ������           ����
====================================================================*/
u8   CMcuVcData::GetEqpType( u8 byEqpId )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || !IsPeriEqpValid( byEqpId ) )
	{
		return 0;
	}

	return m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.GetEqpType();
}
/*====================================================================
    ������      :GetEqpData
    ����        :�õ���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/12/04    3.0         zhangsh           ����
====================================================================*/
TPeriEqpData *CMcuVcData::GetEqpData( u8 byEqpId )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || !IsPeriEqpValid( byEqpId ) )
	{
		return NULL;
	}
	return &m_atPeriEqpTable[byEqpId-1];
}

/*====================================================================
    ������      ��GetIdleVMP
    ����        ���õ����еĻ���ϳ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 * pbyVMPBuf IN OUT : IDLE VMP id buf
                  u8 & byNum    OUT : IDLE VMP num
                  u8   byVMPLen IN  : buf len
    ����ֵ˵��  ��BOOL32
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/12/03    3.0         ������        ����
    06/12/27    4.0         �ű���        ��ȡ���еĿ���VMP  
====================================================================*/
BOOL32 CMcuVcData::GetIdleVMP( u8 * pbyVMPBuf, u8 &byNum, u8 byVMPLen )
{
    if ( NULL == pbyVMPBuf || byVMPLen < MAXNUM_PERIEQP )
    {
        OspPrintf( TRUE, FALSE, "[GetIdleVMP] param err: pbyBuf.0x%x, byVMPLen.%d !\n", pbyVMPBuf, byVMPLen );
        return FALSE;
    }
    byNum = 0;
	u8    byEqpId = VMPID_MIN;
    while( byEqpId <= VMPID_MAX )
	{
		if( IsPeriEqpValid( byEqpId ) &&
			m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_byOnline && 
			m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::IDLE)
		{
            pbyVMPBuf[byNum] = byEqpId;
            byNum ++;
		}
        byEqpId++;
	}

	if (g_bPrintEqpMsg)
	{
		OspPrintf(TRUE, FALSE, "[GetIdleVMP]idle vmp:\n");
		for (u8 byIdx = 0; byIdx < byNum; byIdx++)
		{
			OspPrintf(TRUE, FALSE, "%d.eqpid%d\n", pbyVMPBuf[byIdx]);
		}
		OspPrintf(TRUE, FALSE, "[GetIdleVMP]no other idle vmp\n");

	}

	return TRUE;
}

/*====================================================================
    ������      ��GetIdleVmpTw
    ����        ���õ����еĸ��ϵ���ǽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 * pbyMPWBuf IN OUT : IDLE VMP id buf
                  u8 & byNum    OUT : IDLE VMP num
                  u8   byMPWLen IN  : buf len
    ����ֵ˵��  ��BOOL32
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/10/08    4.0         libo          ����
    06/12/27    4.0         �ű���        ��ȡ���еĿ���MPW
====================================================================*/
BOOL32 CMcuVcData::GetIdleVmpTw( u8 * pbyMPWBuf, u8 &byNum, u8 byMPWLen )
{
    if ( NULL == pbyMPWBuf || byMPWLen < MAXNUM_PERIEQP )
    {
        OspPrintf( TRUE, FALSE, "[GetIdleVMP] param err: pbyBuf.0x%x, byVMPLen.%d !\n", pbyMPWBuf, byMPWLen );
        return FALSE;
    }
    byNum = 0;
    u8    byEqpId = VMPTWID_MIN;
    while( byEqpId <= VMPTWID_MAX )
    {
        if( IsPeriEqpValid( byEqpId ) &&
            m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_byOnline && 
            m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::IDLE)
        {
            pbyMPWBuf[byNum] = byEqpId;
            byNum ++;
        }
        byEqpId++;
    }
    return TRUE;
}

/*====================================================================
    ������      ��GetIdlePrsChl
    ����        ���õ����еİ��ش�ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��0-δ�ҵ� ��0-EqpId
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/03/26    3.0         ������           ����
====================================================================*/
u8   CMcuVcData::GetIdlePrsChl(u8 &byEqpId, u8 &byChlId, u8 &byChlId2, u8 &byChlIdAud)
{
    u8  byLoopEqpId = 1;
    u8  byLoopChlId = 0; 
    TPrsStatus tPrsStatus;

    while (byLoopEqpId < MAXNUM_MCU_PERIEQP)
    {
        if (IsPeriEqpValid(byLoopEqpId))
        {
            if (EQP_TYPE_PRS == GetEqpType(byLoopEqpId))
            {			
                if (m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus.m_byOnline)
                {
                    byLoopChlId = 0;
                    tPrsStatus = m_atPeriEqpTable[byLoopEqpId - 1].m_tPeriEqpStatus.m_tStatus.tPrs;
                    while (byLoopChlId < MAXNUM_PRS_CHNNL)
                    {
                        if (!tPrsStatus.m_tPerChStatus[byLoopChlId].IsReserved() &&
                            !tPrsStatus.m_tPerChStatus[byLoopChlId + 1].IsReserved() &&
                            !tPrsStatus.m_tPerChStatus[byLoopChlId + 2].IsReserved())
                        {
                            byEqpId = byLoopEqpId;
                            byChlId = byLoopChlId;
                            byChlId2 = byLoopChlId + 1;
                            byChlIdAud = byLoopChlId + 2;
                            return byEqpId;
                        }
                        byLoopChlId++;
                    }
                }
            }
        }
        byLoopEqpId++;
    }

    OspPrintf(TRUE, FALSE, "[GetIdlePrsChl]: No Idle prs chl can get!\n");

    return 0;
}

/*=============================================================================
  �� �� ���� GetIdlePrsChl
  ��    �ܣ� ��ȡһ�����еİ��ش�ͨ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byEqpId
  �� �� ֵ�� u8  
=============================================================================*/
u8   CMcuVcData::GetIdlePrsChl(u8 &byEqpId, u8 &byChlId)
{
    u8  byLoopEqpId = 1;
    u8  byLoopChlId = 0; 
    TPrsStatus tPrsStatus;
    
    while (byLoopEqpId < MAXNUM_MCU_PERIEQP)
    {
        if (IsPeriEqpValid(byLoopEqpId))
        {
            if (EQP_TYPE_PRS == GetEqpType(byLoopEqpId))
            {			
                if (m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus.m_byOnline)
                {
                    byLoopChlId = 0;
                    tPrsStatus = m_atPeriEqpTable[byLoopEqpId - 1].m_tPeriEqpStatus.m_tStatus.tPrs;
                    while (byLoopChlId < MAXNUM_PRS_CHNNL)
                    {
                        if (!tPrsStatus.m_tPerChStatus[byLoopChlId].IsReserved())                            
                        {
                            byEqpId = byLoopEqpId;
                            byChlId = byLoopChlId;                            
                            return byEqpId;
                        }
                        byLoopChlId++;
                    }
                }
            }
        }
        byLoopEqpId++;
    }
    
    OspPrintf(TRUE, FALSE, "[GetIdlePrsChl]: No Idle prs chl can get!\n");
    
    return 0;
}
/*====================================================================
    ������      : GetIdlePrsChls
    ����        ������������Prs�еõ����е�ͨ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byChannelNum ��Ҫ��ͨ����
				  TPrsChannel& tPrsChannel ͨ���ṹ
    ����ֵ˵��  ��FALSE-δ�ҵ� TRUE-�ҵ�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/02/22    4.0         john           ����
====================================================================*/
BOOL32 CMcuVcData::GetIdlePrsChls(u8 byChannelNum, TPrsChannel& tPrsChannel)
{
	u8  byEqpPrsId;
    TPrsStatus tPrsStatus;
    BOOL32 bRet = FALSE;
	u8 byIdleNum = GetMaxIdleChlsPrsId( byEqpPrsId ); // �õ�����Prs��������ͨ����
	if( byChannelNum <= byIdleNum ) // ���㹻��ͨ������������
	{
		u8 byChlIdx = 0;
		tPrsStatus = m_atPeriEqpTable[byEqpPrsId - 1].m_tPeriEqpStatus.m_tStatus.tPrs;
		for(u8 byChlLp = 0; byChlLp < MAXNUM_PRS_CHNNL; byChlLp++)
		{
			if (!tPrsStatus.m_tPerChStatus[byChlLp].IsReserved())
			{
				tPrsChannel.m_abyPrsChannels[byChlIdx++] = byChlLp;
				if ( byChlIdx == byChannelNum ) // �õ��㹻��ͨ����
				{
					break;
				}
			}
		}
		tPrsChannel.SetChannelNum( byChannelNum );
		tPrsChannel.SetPrsId( byEqpPrsId );
		bRet = TRUE;
	}
	return bRet;
}
/*====================================================================
    ������      : GetIdlePrsChls
    ����        ���õ����еĻ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byPrsId      ָ��PrsId
				  u8 byChannelNum ��Ҫ��ͨ����
				  TPrsChannel& tPrsChannel ͨ���ṹ
    ����ֵ˵��  ��FALSE-δ�ҵ� TRUE-�ҵ�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/02/22    4.0         john           ����
====================================================================*/
BOOL32 CMcuVcData::GetIdlePrsChls(u8 byPrsId, u8 byChannelNum, TPrsChannel& tPrsChannel)
{
	BOOL32 bRet = FALSE;
	TPrsStatus tPrsStatus = m_atPeriEqpTable[byPrsId - 1].m_tPeriEqpStatus.m_tStatus.tPrs;
	if ( 1 == m_atPeriEqpTable[byPrsId-1].m_tPeriEqpStatus.m_byOnline ) // ����
	{
		if( byChannelNum <= tPrsStatus.GetIdleChlNum() )
		{
			u8 byChlIdx = 0;
			for(u8 byChlLp = 0; byChlLp < MAXNUM_PRS_CHNNL; byChlLp++)
			{
				if (!tPrsStatus.m_tPerChStatus[byChlLp].IsReserved())
				{
					tPrsChannel.m_abyPrsChannels[byChlIdx++] = byChlLp;
					if ( byChlIdx == byChannelNum ) // �õ��㹻��ͨ����
					{
						break;
					}
				}
			}
			bRet = TRUE;
		}
	}
	return bRet;
}
/*====================================================================
    ������      GetMaxIdleChlsPrsId
    ����        ���õ�����Prs�п���ͨ��������PrsId
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpPrsId PrsId
    ����ֵ˵��  ������ͨ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/02/22    4.0         john           ����
====================================================================*/
u8 CMcuVcData::GetMaxIdleChlsPrsId(u8& byEqpPrsId )
{
    u8  byMaxChls = 0; 
    TPrsStatus tPrsStatus;
    u8  byLoopEqpId = PRSID_MIN;
    while (byLoopEqpId >= PRSID_MIN && byLoopEqpId <= PRSID_MAX)
    {
        if ( 1 == m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus.m_byOnline ) // ����
		{
			if (IsPeriEqpValid(byLoopEqpId) 
				&& EQP_TYPE_PRS == GetEqpType(byLoopEqpId) ) // �Ϸ��Լ��
			{
				tPrsStatus = m_atPeriEqpTable[byLoopEqpId - 1].m_tPeriEqpStatus.m_tStatus.tPrs;
				if( byMaxChls <= tPrsStatus.GetIdleChlNum() ) // ��ͬ������ʱ��ȡ����ġ�
				{
					byEqpPrsId = byLoopEqpId;
					byMaxChls = tPrsStatus.GetIdleChlNum();
				}
            }
        }
		byLoopEqpId++;
    }
	return byMaxChls;
}
/*====================================================================
    ������      ��GetIdleMixGroup
    ����        ���õ����еĻ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 &byEqpId ���ص������
	              u8 &byGrpId ���صĻ�����ID
    ����ֵ˵��  ��0-δ�ҵ� ��0-EqpId
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/12/03    3.0         ������           ����
====================================================================*/
u8   CMcuVcData::GetIdleMixGroup( u8 &byEqpId, u8 &byGrpId )
{
	u8  byLoopEqpId = 1;
	u8  byLoopGrpId = 0;
    TMixerStatus tMixerStatus;

    while( byLoopEqpId <= MAXNUM_MCU_PERIEQP )
	{
		if( IsPeriEqpValid( byLoopEqpId ) )
		{
			if( GetEqpType( byLoopEqpId ) == EQP_TYPE_MIXER )
			{
			    if( m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus.m_byOnline )
				{
					byLoopGrpId = 0;
					tMixerStatus = m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer;
					while( byLoopGrpId < tMixerStatus.m_byGrpNum )
					{
						if( tMixerStatus.m_atGrpStatus[byLoopGrpId].m_byGrpState == TMixerGrpStatus::READY )
						{
							byEqpId = byLoopEqpId;
							byGrpId = byLoopGrpId;
							return byEqpId;
						}

						byLoopGrpId++;				
					}
			
				}
			}
		}
		
        byLoopEqpId++;
	}

	return 0;	
}

/*====================================================================
    ������      ��GetIdleBasChl
    ����        ���õ����е�������ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 &byEqpId ���ص������
	              u8 &byChlId ���ص�ͨ��ID
    ����ֵ˵��  ��0-δ�ҵ� ��0-EqpId
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/03/20    3.0         ������           ����
====================================================================*/
u8 CMcuVcData::GetIdleBasChl(u8 byAdaptType, u8 &byEqpId, u8 &byChlId)
{
    u8 byLoopGrpId = 0;
    u8 byMinChnl;
    u8 byMaxChnl;
    TPeriEqpStatus *ptStatus;

    for (u8 byLoopEqpId = 1; byLoopEqpId < MAXNUM_MCU_PERIEQP; byLoopEqpId++)
    {
        if (!IsPeriEqpValid(byLoopEqpId))
        {
            continue;
        }
        if (EQP_TYPE_BAS != GetEqpType(byLoopEqpId))
        {
            continue;
        }

        if ( g_cMcuAgent.IsEqpBasHD(byLoopEqpId) )
        {
            continue;   
        }

        ptStatus = &(m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus);
        if (!ISTRUE(ptStatus->m_byOnline))
        {
            continue;
        }

        byMinChnl = 0;
        byMaxChnl = ptStatus->m_tStatus.tBas.byChnNum;
        
        //���ҿ���ͨ��
        for (u8 byChnIdx =byMinChnl; byChnIdx < byMaxChnl; byChnIdx++)
        {            
            if (!ptStatus->m_tStatus.tBas.tChnnl[byChnIdx].IsReserved())
            {                
                BOOL32 bFlag = FALSE;
                u8 byBasChnnlType = ptStatus->m_tStatus.tBas.tChnnl[byChnIdx].GetChannelType();
                if (BAS_CHAN_AUDIO == byBasChnnlType)
                {
                    if (ADAPT_TYPE_AUD == byAdaptType)
                    {
                        bFlag = TRUE;
                    }                    
                }
                else if (BAS_CHAN_VIDEO == byBasChnnlType)
                {
                    if (ADAPT_TYPE_VID == byAdaptType || ADAPT_TYPE_BR == byAdaptType)
                    {
                        bFlag = TRUE;
                    }                    
                }

                if (bFlag)
                {                    
                    ptStatus->m_tStatus.tBas.tChnnl[byChnIdx].SetReserved(TRUE);
                    byEqpId = byLoopEqpId;
                    byChlId = byChnIdx;
                    return byEqpId;
                }
            }
        }			
    }

	return 0;
}
/*=============================================================================
�� �� ���� IsIdleHDBasVidChlExist
��    �ܣ� �����Ƿ��п��е�MAU����Ƶ����ͨ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����
�� �� ֵ�� BOOL32
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		������		�߶���    �޸�����
09/06/03                ���㻪             
=============================================================================*/
BOOL32 CMcuVcData::IsIdleHDBasVidChlExist(u8* pbyEqpId /*= NULL*/, u8* pbyChlIdx /*= NULL*/)
{
    TPeriEqpStatus *ptStatus;
    for (u8 byLoopEqpId = BASID_MIN; byLoopEqpId <= BASID_MAX; byLoopEqpId++)
    {
        if (!IsPeriEqpValid(byLoopEqpId))
        {
            continue;
        }
        if ( !g_cMcuAgent.IsEqpBasHD(byLoopEqpId) )
        {
            continue;   
        }

        ptStatus = &(m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus);
        if (!ISTRUE(ptStatus->m_byOnline))
        {
            continue;
        }

        u8 byEqpType = ptStatus->m_tStatus.tHdBas.GetEqpType();
        if (TYPE_MAU_H263PLUS == byEqpType)
        {
            continue;
		}

        u8 byChnIdx = 0;
        THDBasVidChnStatus tVidChn;

        //���ҿ���ͨ��
        switch (byEqpType)
        {
        case TYPE_MPU:
            for (byChnIdx = 0; byChnIdx < MAXNUM_MPU_CHN; byChnIdx++)
            {            
                memset( &tVidChn, 0, sizeof(tVidChn) );
                memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx), sizeof(tVidChn) );
                if (!tVidChn.IsReserved() &&
                    !tVidChn.IsTempReserved() &&
                    tVidChn.GetStatus() != THDBasChnStatus::IDLE )
                {
                    if (pbyEqpId != NULL)
                    {
                        *pbyEqpId = byLoopEqpId;
                    }
                    if (pbyChlIdx != NULL)
                    {
                        *pbyChlIdx = byChnIdx;
                    }
                    return TRUE;
                }
            }

        	break;

        case TYPE_MAU_NORMAL:
            for (byChnIdx = 0; byChnIdx < MAXNUM_MAU_VCHN; byChnIdx++)
            {            
                memset( &tVidChn, 0, sizeof(tVidChn) );
                memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(byChnIdx), sizeof(tVidChn) );
                if (!tVidChn.IsReserved() &&
                    !tVidChn.IsTempReserved() &&
                    tVidChn.GetStatus() != THDBasChnStatus::IDLE )
                {
                    if (pbyEqpId != NULL)
                    {
                        *pbyEqpId = byLoopEqpId;
                    }
                    if (pbyChlIdx != NULL)
                    {
                        *pbyChlIdx = byChnIdx;
                    }
                    return TRUE;
                }
            }

            break;
        default:
            OspPrintf(TRUE, FALSE, "[IsIdleHDBasVidChlExist] unexpected type.%d for eqp.%d!\n", byEqpType, byLoopEqpId);
            break;
        }		
	}
	return FALSE;
}
/*=============================================================================
�� �� ���� GetIdleHDBasVidChl
��    �ܣ� Ϊ��һ·��������Ƶ��ȡ���е�MAU����ͨ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
           u8 &byEqpId
           u8 &byChlIdx
           BOOL32 bIsTemp
�� �� ֵ�� BOOL32
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/8/4   4.0		    �ܹ��                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetIdleHDBasVidChl( u8 &byEqpId, u8 &byChlIdx )
{
    TPeriEqpStatus *ptStatus;
    
    for (u8 byLoopEqpId = BASID_MIN; byLoopEqpId <= BASID_MAX; byLoopEqpId++)
    {
        if (!IsPeriEqpValid(byLoopEqpId))
        {
            continue;
        }
        if ( !g_cMcuAgent.IsEqpBasHD(byLoopEqpId) )
        {
            continue;   
        }
       
        ptStatus = &(m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus);
		if (TYPE_MAU_H263PLUS == ptStatus->m_tStatus.tHdBas.GetEqpType())
		{
			continue;
		}

        if (!ISTRUE(ptStatus->m_byOnline))
        {
            continue;
        }
        
        u8 byEqpType = ptStatus->m_tStatus.tHdBas.GetEqpType();

        u8 byChnIdx = 0;
        THDBasVidChnStatus tVidChn;

        //���ҿ���ͨ��
        switch (byEqpType)
        {
        case TYPE_MPU:

            for (byChnIdx = 0; byChnIdx < MAXNUM_MPU_CHN; byChnIdx++)
            {            
                memset( &tVidChn, 0, sizeof(tVidChn) );
                memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx), sizeof(tVidChn) );
                if (!tVidChn.IsReserved() &&
                    !tVidChn.IsTempReserved() &&
                    tVidChn.GetStatus() != THDBasChnStatus::IDLE )
                {          
                    tVidChn.SetIsReserved( TRUE );
                    ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus( tVidChn, byChnIdx );
                    byEqpId = byLoopEqpId;
                    byChlIdx = byChnIdx;
                    return TRUE;
                }
            }	
        	break;

        case TYPE_MAU_NORMAL:

            for (byChnIdx = 0; byChnIdx < MAXNUM_MAU_VCHN; byChnIdx++)
            {            
                memset( &tVidChn, 0, sizeof(tVidChn) );
                memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(), sizeof(tVidChn) );
                if (!tVidChn.IsReserved() &&
                    !tVidChn.IsTempReserved() &&
                    tVidChn.GetStatus() != THDBasChnStatus::IDLE )
                {          
                    tVidChn.SetIsReserved( TRUE );
                    ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus( tVidChn );
                    byEqpId = byLoopEqpId;
                    byChlIdx = byChnIdx;
                    return TRUE;
                }
            }	
            break;
        default:
            OspPrintf(TRUE, FALSE, "[GetIdleHDBasVidChl] unexpected eqp type.%d for eqp.%d!\n", byEqpType, byLoopEqpId);
            break;
        }
    }
    
	return FALSE;
}

/*====================================================================
    ������      ��GetIdleMauChn
    ����        ����ȡ���е�MAUͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/09    4.5         �ű���        ����
====================================================================*/
BOOL32 CMcuVcData::GetIdleMauChn(u8 byChnType, u8 &byMauId, u8 &byChnId)
{
    byMauId = 0xff;
    byChnId = 0xff;

    TPeriEqpStatus *ptStatus;

    BOOL32 bMVChn = FALSE;
    BOOL32 bDSChn = FALSE;

    switch (byChnType)
    {
    case MAU_CHN_NORMAL:
    case MAU_CHN_263TO264:
        bMVChn = TRUE;
        break;
    case MAU_CHN_VGA:
    case MAU_CHN_264TO263:
        bDSChn = TRUE;
    	break;
    default:
        OspPrintf(TRUE, FALSE, "[GetIdleMauChn] unexpected chn type.%d\n", byChnType);
        return FALSE;
    }

    for (u8 byEqpId = BASID_MIN; byEqpId <= BASID_MAX; byEqpId++)
    {
        if (!IsPeriEqpValid(byEqpId))
        {
            continue;
        }
        if ( !g_cMcuAgent.IsEqpBasHD(byEqpId) )
        {
            continue;   
        }
        
        ptStatus = &(m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus);
        if (!ISTRUE(ptStatus->m_byOnline))
        {
            continue;
        }
        
        if ((MAU_CHN_VGA == byChnType || MAU_CHN_NORMAL == byChnType) &&
            ptStatus->m_tStatus.tHdBas.GetEqpType() != TYPE_MAU_NORMAL)
        {
            continue;
        }
        if ((MAU_CHN_263TO264 == byChnType || MAU_CHN_264TO263 == byChnType) &&
            ptStatus->m_tStatus.tHdBas.GetEqpType() != TYPE_MAU_H263PLUS)
        {
            continue;
        }

        if (bMVChn)
        {
            THDBasVidChnStatus tVidChn;
            u8 byChnIdx = 0;
            for (; byChnIdx < MAXNUM_MAU_VCHN; byChnIdx++)
            {            
                memset( &tVidChn, 0, sizeof(tVidChn) );
                memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(byChnIdx), sizeof(tVidChn) );
                if (!tVidChn.IsReserved() &&
                    !tVidChn.IsTempReserved() &&
                    tVidChn.GetStatus() == THDBasChnStatus::READY )
                {
                    tVidChn.SetIsReserved(TRUE);
                    ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tVidChn, byChnIdx );
                    byMauId = byEqpId;
                    byChnId = 0;
                    break;
                }
                else
                {
                    EqpLog("[GetIdleMau] Eqp.%d mv failed due to<ChnIdx.%d, Res.%d, Status.%d>\n",
                        byEqpId, byChnIdx, tVidChn.IsReserved(), tVidChn.GetStatus());
                    continue;
                }
            }
        }
        else if (bDSChn)
        {
            THDBasDVidChnStatus tDVidChn;
            u8 byChnIdx = 0;
            for (; byChnIdx < MAXNUM_MAU_VCHN; byChnIdx++)
            {            
                memset( &tDVidChn, 0, sizeof(tDVidChn) );
                memcpy( &tDVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(byChnIdx), sizeof(tDVidChn) );
                if (!tDVidChn.IsReserved() &&
                    !tDVidChn.IsTempReserved() &&
                    tDVidChn.GetStatus() == THDBasChnStatus::READY )
                {
                    tDVidChn.SetIsReserved(TRUE);
                    ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tDVidChn, byChnIdx );
                    byMauId = byEqpId;
                    byChnId = 1;
                    break;
                }
                else
                {
                    EqpLog("[GetIdleMau] Eqp.%d mv failed due to<ChnIdx.%d, Res.%d, Status.%d>\n",
                        byEqpId, byChnIdx, tDVidChn.IsReserved(), tDVidChn.GetStatus());
                    continue;
                }
            }
        }

        if (0xff != byMauId)
        {
            break;
        }
    }

    if (0xff != byMauId && 0xff != byChnId)
    {
        return TRUE;
    }
    return FALSE;
}

/*====================================================================
    ������      ��GetIdleMpuChn
    ����        ����ȡ���е�Mpuͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/05/19    4.6         �ű���        ����
====================================================================*/
BOOL32 CMcuVcData::GetIdleMpuChn(u8 &byEqpId, u8 &byChnId)
{
    byEqpId = 0xff;
    byChnId = 0xff;

    TPeriEqpStatus *ptStatus;

    for (u8 byIdx = BASID_MIN; byIdx <= BASID_MAX; byIdx++)
    {
        if (!IsPeriEqpValid(byIdx))
        {
            continue;
        }
        if ( !g_cMcuAgent.IsEqpBasHD(byIdx) )
        {
            continue;   
        }
        
        ptStatus = &(m_atPeriEqpTable[byIdx-1].m_tPeriEqpStatus);
        if (!ISTRUE(ptStatus->m_byOnline))
        {
            continue;
        }
        if (ptStatus->m_tStatus.tHdBas.GetEqpType() != TYPE_MPU)
        {
            continue;
        }

        THDBasVidChnStatus tVidChn;
        u8 byChnIdx = 0;
        for (; byChnIdx < MAXNUM_MPU_CHN; byChnIdx++)
        {
            memset( &tVidChn, 0, sizeof(tVidChn) );
            memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx), sizeof(tVidChn) );
            if (!tVidChn.IsReserved() &&
                !tVidChn.IsTempReserved() &&
                tVidChn.GetStatus() == THDBasChnStatus::READY )
            {
                tVidChn.SetIsReserved(TRUE);
                ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tVidChn, byChnIdx );
                byEqpId = byIdx;
                byChnId = byChnIdx;
                break;
            }
            else
            {
                EqpLog("[GetIdleMpuChn] Eqp.%d failed due to<ChnIdx.%d, Res.%d, Status.%d>\n",
                    byIdx, byChnIdx, tVidChn.IsReserved(), tVidChn.GetStatus());
                continue;
            }
        }
        if (0xff != byEqpId && 0xff != byChnId)
        {
            break;
        }
    }

    if (0xff != byEqpId && 0xff != byChnId)
    {
        return TRUE;
    }
    return FALSE;
}

/*====================================================================
    ������      ��ReleaseHDBasChn
    ����        ���ͷ�ռ�õ�HD-BASͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/09    4.5         �ű���        ����
====================================================================*/
void CMcuVcData::ReleaseHDBasChn(u8 byEqpId, u8 byChnId)
{
    TPeriEqpStatus *ptStatus;
    
    if (!IsPeriEqpValid(byEqpId))
    {
        return;
    }
    if ( !g_cMcuAgent.IsEqpBasHD(byEqpId) )
    {
        return;   
    }
    
    ptStatus = &(m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus);
    
    //zbq[11/27/2008] �����ߵ�EQPҲҪ����������
    //if (!ISTRUE(ptStatus->m_byOnline))
    //{
    //    return;
    //}

    u8 byAdpType = ptStatus->m_tStatus.tHdBas.GetEqpType();

    switch (byAdpType)
    {
    case TYPE_MAU_NORMAL:
    case TYPE_MAU_H263PLUS:
        {
            if (byChnId >= MAXNUM_MAU_VCHN + MAXNUM_MAU_DVCHN)
            {
                OspPrintf(TRUE, FALSE, "[ReleaseHDBasChn] unexpected mau chnId.%d\n", byChnId);
                return;
            }
            BOOL32 bReleaseMV = byChnId == 0 ? TRUE : FALSE;
         
            if (bReleaseMV)
            {
                THDBasVidChnStatus tVidChn;
                memset( &tVidChn, 0, sizeof(tVidChn) );
                memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(), sizeof(tVidChn) );          

                tVidChn.SetIsReserved(FALSE);
                tVidChn.SetStatus(THDBasChnStatus::READY);
				tVidChn.ClrOutputVidParam();
                ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tVidChn);
            }
            else
            {
                THDBasVidChnStatus tVidChn;
                memset( &tVidChn, 0, sizeof(tVidChn) );
                memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(), sizeof(tVidChn) );
                
                tVidChn.SetIsReserved(FALSE);
                tVidChn.SetStatus(THDBasChnStatus::READY);
				tVidChn.ClrOutputVidParam();
                ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tVidChn);
            }
        }
        break;

    case TYPE_MPU:
        {
            if (byChnId >= MAXNUM_MPU_VCHN)
            {
                OspPrintf(TRUE, FALSE, "[ReleaseHDBasChn] unexpected mpu chnId.%d\n", byChnId);
                return;
            }
            THDBasVidChnStatus tVidChn;
            memset( &tVidChn, 0, sizeof(tVidChn) );
            memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnId), sizeof(tVidChn) );
            
            tVidChn.SetIsReserved(FALSE);
            tVidChn.SetStatus(THDBasChnStatus::READY);
			tVidChn.ClrOutputVidParam();
            ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tVidChn, byChnId );  
        }
    	break;
    default:
        OspPrintf(TRUE, FALSE, "[ReleaseHDBasChn] unexpected EqpType.%d!\n", byAdpType);
        break;
    }
	SendPeriEqpStatusToMcs(byEqpId);
    return;
}

/*====================================================================
    ������      ��ResetHDBasChn
    ����        �����ͨ���������������ֹ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/05/20    4.6         �ű���        ����
====================================================================*/
void CMcuVcData::ResetHDBasChn(u8 byEqpId, u8 byChnId)
{
    TPeriEqpStatus tEqpStatus;
    if (!g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tEqpStatus))
    {
        OspPrintf(TRUE, FALSE, "[ResetHDBasChn] get Eqp.%d status failed!\n", byEqpId);
        return;
    }
    
    THDBasVidChnStatus tVidChn;
    memset(&tVidChn, 0, sizeof(tVidChn));
    
    u8 byBasType = tEqpStatus.m_tStatus.tHdBas.GetEqpType();
    switch (byBasType)
    {
    case TYPE_MAU_NORMAL:
    case TYPE_MAU_H263PLUS:
        {
            if (byChnId >= MAXNUM_MAU_VCHN + MAXNUM_MAU_DVCHN)
            {
                OspPrintf(TRUE, FALSE, "[TMVChnGrp::AddChn] unexpected mau chnId.%d!\n", byChnId);
                return;
            }
            
            BOOL32 bChnMV = byChnId == 0 ? TRUE : FALSE;
            
            if (bChnMV)
            {
                tVidChn = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus();
                
                for(u8 byIdx = 0; byIdx < MAXNUM_VOUTPUT; byIdx++)
                {
                    THDAdaptParam tParam = *tVidChn.GetOutputVidParam(byIdx);
                    tParam.Reset();
                    tVidChn.SetOutputVidParam(tParam, byIdx);
                }
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tVidChn);
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tEqpStatus);
            }
            else
            {
                tVidChn = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus();
                
                for(u8 byIdx = 0; byIdx < MAXNUM_VOUTPUT; byIdx++)
                {
                    THDAdaptParam tParam = *tVidChn.GetOutputVidParam(byIdx);
                    tParam.Reset();
                    tVidChn.SetOutputVidParam(tParam, byIdx);
                }
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tVidChn);
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tEqpStatus);
            }
        }
        break;
        
    case TYPE_MPU:
        {
            if (byChnId >= MAXNUM_MPU_CHN)
            {
                OspPrintf(TRUE, FALSE, "[TMVChnGrp::AddChn] unexpected mpu chnId.%d!\n", byChnId);
                return;
            }
            tVidChn = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnId);
            
            for(u8 byIdx = 0; byIdx < MAXNUM_VOUTPUT; byIdx++)
            {
                THDAdaptParam tParam = *tVidChn.GetOutputVidParam(byIdx);
                tParam.Reset();
                tVidChn.SetOutputVidParam(tParam, byIdx);
            }
            tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tVidChn, byChnId);
            g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tEqpStatus);
        }
        break;
        
    default:
        break;
    }

    g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tEqpStatus);
    
    return;
}

/*====================================================================
    ������      ��GetIdleRecorder
    ����        �������Ƿ��п��õ�¼���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/07/18    4.0         ����        ����
====================================================================*/
BOOL32 CMcuVcData::IsRecorderOnline (u8 byEqpId)
{
    if( byEqpId < MAXNUM_MCU_PERIEQP )
	{
		if( GetEqpType( byEqpId ) == EQP_TYPE_RECORDER &&
            m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_byOnline )
		{
            // Ŀǰ�ͼ�����
			return TRUE;
		}
	}    
    return FALSE;
}

/*====================================================================
    ������      ��SetEqpAlias
    ����        �������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPCSTR strAlias �������	           
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/02/25    3.0         ������           ����
====================================================================*/
void   CMcuVcData::SetEqpAlias( u8 byEqpId, LPCSTR lpszAlias )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP )
	{
		return;
	}

	if( lpszAlias != NULL )
	{
		strncpy( m_atPeriEqpTable[byEqpId-1].m_achAliase, lpszAlias, MAXLEN_EQP_ALIAS );
		m_atPeriEqpTable[byEqpId-1].m_achAliase[MAXLEN_EQP_ALIAS - 1] = '\0';
	}
	else
	{
		memset( m_atPeriEqpTable[byEqpId-1].m_achAliase, 0, MAXLEN_EQP_ALIAS );
	}
}

/*====================================================================
    ������      ��GetEqpAlias
    ����        ���õ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPCSTR strAlias �������	           
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/02/25    3.0         ������           ����
====================================================================*/
LPCSTR CMcuVcData::GetEqpAlias( u8 byEqpId )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP )
	{
		return NULL;
	}

	return m_atPeriEqpTable[byEqpId-1].m_achAliase;
}

/*====================================================================
    ������      ��IsEqpH263pMau
    ����        ���õ�mau �Ƿ�ΪH263p
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/01/08    4.5         �ű���           ����
====================================================================*/
BOOL32 CMcuVcData::IsEqpH263pMau(u8 byEqpId)
{
    if (!g_cMcuAgent.IsEqpBasHD(byEqpId))
    {
        return FALSE;
    }

    TPeriEqpStatus tStatus;
    GetPeriEqpStatus(byEqpId, &tStatus);

    if (TYPE_MAU_H263PLUS == tStatus.m_tStatus.tHdBas.GetEqpType())
    {
        return TRUE;
    }
    return FALSE;
}

/*====================================================================
    ������      ��GetIdleMau
    ����        ���õ�mau �Ƿ�ΪH263p
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/01/08    4.5         �ű���           ����
====================================================================*/
void CMcuVcData::GetIdleMau(u8 &byNVChn, u8 &byVGAChn, u8 &byH263pChn)
{
    BOOL32 bNVReserved = FALSE;
    BOOL32 bVGAReserved = FALSE;
    BOOL32 bH263p = FALSE;

    for(u8 byEqpId = BASID_MIN; byEqpId <= BASID_MAX; byEqpId++)
    {
        TPeriEqpStatus tStatus;
        if (GetPeriEqpStatus(byEqpId, &tStatus))
        {
            //hd only
            if (g_cMcuVcApp.IsPeriEqpConnected(byEqpId) &&
                g_cMcuAgent.IsEqpBasHD(byEqpId) &&
                !tStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->IsNull() &&
                !tStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->IsNull())
            {
                u8 byEqpType = tStatus.m_tStatus.tHdBas.GetEqpType();
                if (TYPE_MPU == byEqpType)
                {
                    continue;
                }

                u8 byConfIdx = tStatus.GetConfIdx();
                
                //one chnnl only
                bNVReserved = tStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->IsReserved();
                bVGAReserved = tStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->IsReserved();
                
                //h263p
                bH263p = byEqpType == TYPE_MAU_H263PLUS ? TRUE : FALSE;
                
                if (bH263p)
                {
                    if (!bNVReserved)
                    {
                        byH263pChn++;
                    }
                }
                else
                {
                    if (!bNVReserved)
                    {
                        byNVChn ++;
                    }
                    if (!bVGAReserved)
                    {
                        byVGAChn++;
                    }
                }
            }
        }
    }

    return;
}

/*====================================================================
    ������      ��GetIdleMpu
    ����        ���õ����е�mpuͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/05/19    4.6         �ű���           ����
====================================================================*/
void CMcuVcData::GetIdleMpu(u8 &byChnNum)
{
    byChnNum = 0;
    BOOL32 bReserved = FALSE;
    
    for(u8 byEqpId = BASID_MIN; byEqpId <= BASID_MAX; byEqpId++)
    {
        TPeriEqpStatus tStatus;
        memset(&tStatus, 0, sizeof(tStatus));
        
        if (!GetPeriEqpStatus(byEqpId, &tStatus))
        {
            continue;
        }

        if (!g_cMcuVcApp.IsPeriEqpConnected(byEqpId) ||
            !g_cMcuAgent.IsEqpBasHD(byEqpId))
        {
            continue;
        }
        u8 byEqpType = tStatus.m_tStatus.tHdBas.GetEqpType();
        if (TYPE_MPU != byEqpType)
        {
            continue;
        }

        for(u8 byChnIdx = 0; byChnIdx < MAXNUM_MPU_CHN; byChnIdx ++)
        {
            THDBasVidChnStatus tVidChn;
            memset(&tVidChn, 0, sizeof(tVidChn));
            tVidChn = *tStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx);
            bReserved = tVidChn.IsReserved();
            
            if (!bReserved)
            {
                byChnNum ++;
            }
        }
    }
    
    return;
}

/*====================================================================
    ������      ��GetMpuChn
    ����        ���õ�mpu����ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/05/19    4.6         �ű���           ����
====================================================================*/
void CMcuVcData::GetMpuNum(u8 &byMpuNum)
{
    byMpuNum = 0;
    BOOL32 bReserved = FALSE;
    
    for(u8 byEqpId = BASID_MIN; byEqpId <= BASID_MAX; byEqpId++)
    {
        TPeriEqpStatus tStatus;
        memset(&tStatus, 0, sizeof(tStatus));
        
        if (!GetPeriEqpStatus(byEqpId, &tStatus))
        {
            continue;
        }
        
        if (!g_cMcuVcApp.IsPeriEqpConnected(byEqpId) ||
            !g_cMcuAgent.IsEqpBasHD(byEqpId))
        {
            continue;
        }
        u8 byEqpType = tStatus.m_tStatus.tHdBas.GetEqpType();
        if (TYPE_MPU != byEqpType)
        {
            continue;
        }
        byMpuNum += 1;
    }

    return;
}

/*====================================================================
    ������      ��ShowBasInfo
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/04/11    4.5         �ű���           ����
====================================================================*/
void CMcuVcData::ShowBasInfo()
{
    for(u8 byEqpId = BASID_MIN; byEqpId < BASID_MAX; byEqpId++)
    {
        if (!IsPeriEqpValid(byEqpId))
        {
            continue;
        }
        if (!IsPeriEqpConnected(byEqpId))
        {
            continue;
        }
        //��ʱ��֧�ַǸ���bas
        if (!g_cMcuAgent.IsEqpBasHD(byEqpId))
        {
            continue;
        }
        
        u8 byBasType = m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.GetEqpType();

        if (TYPE_MPU == byBasType)
        {
            OspPrintf(TRUE, FALSE, "\n---- MPU-BAS.%d info is as follows: ----\n\n", byEqpId);
            m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.Print();
            OspPrintf(TRUE, FALSE, "\n---- MPU-BAS.%d info is end         ----\n", byEqpId);
        }
        else
        {
            OspPrintf(TRUE, FALSE, "\n---- MAU-BAS.%d info is as follows: ----\n\n", byEqpId);
            m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.Print();
            OspPrintf(TRUE, FALSE, "\n---- MAU-BAS.%d info is end         ----\n", byEqpId);
        }
    }
}

/*====================================================================
    ������      ��GetTvWallOutputMode
    ����        ���õ�����ǽ���ģʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpId �����	           
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/10/18    3.6         libo           ����
====================================================================*/
u8 CMcuVcData::GetTvWallOutputMode(u8 byEqpId)
{
    if (byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP)
    {
        return TW_OUTPUTMODE_NONE;
    }

    if (EQP_TYPE_TVWALL != m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.GetEqpType())
    {
        return TW_OUTPUTMODE_NONE;
    }

    return m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tTvWall.byOutputMode;
}

//4.6.1  �¼�  jlb
/*====================================================================
    ������      : GetHduOutputMode
    ����        : �õ�Hdu���ģʽ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u8 byEqpId �����	           
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
	09/01/19    4.6         ���ֱ�           ����
====================================================================*/
u8 CMcuVcData::GetHduOutputMode( u8 byEqpId )    
{
    if (byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP)
    {
        return HDU_OUTPUTMODE_NONE;
    }
	
    if (EQP_TYPE_HDU != m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.GetEqpType())
    {
        return HDU_OUTPUTMODE_NONE;
    }
	
    return m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdu.byOutputMode;
}

/*====================================================================
    ������      ��EqpStatusShow
    ����        ����ʾ�����״̬��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpId ���ص������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/01/09    3.0         zmy            ����
====================================================================*/
void CMcuVcData::EqpStatusShow( u8   byEqpId )
{
    s32 nChnnlNum;
    s32 nChnIdx, nGrpIdx;
    s8  achEqpType[][12]={"UNKNOW","MIXER","VMP","RECORDER","BAS","TVWALL","DCS","PRS","FILEENC","VMPTW","HDU" };
    OspPrintf( TRUE, FALSE, "_____________________________________________________________\n");
    if (byEqpId < 1) 
    {
        OspPrintf( TRUE, FALSE, "Eqp Id must from 1 - 140\n");
        OspPrintf( TRUE, FALSE, "Dcs Id must from 1 - 16 ( support only one now )\n");
        OspPrintf( TRUE, FALSE, "<<<<<<<<<<<<<    ��ǰ����������Ϣ    >>>>>>>>>>>>>\n");
        for(s32 nIdx =1; nIdx <= MAXNUM_MCU_PERIEQP; nIdx++)
        {
            if (m_atPeriEqpTable[nIdx-1].m_bIsValid)
            {
                OspPrintf(TRUE, FALSE, " %s (Eqp.%2d) %10s %s \n",
                          m_atPeriEqpTable[nIdx-1].m_tPeriEqpStatus.m_byOnline ? "��":"��",
                          nIdx,
                          achEqpType[min(GetEqpType(nIdx), sizeof(achEqpType)/sizeof(achEqpType[0]))],
                          m_atPeriEqpTable[nIdx-1].m_tPeriEqpStatus.m_byOnline ? "����":"����");
            }
        }
        u8 byDcsNum = 0;
        for(u8 byIdx = 1; byIdx <= MAXNUM_MCU_DCS; byIdx ++ )
        {            
            if (m_atPeriDcsTable[byIdx-1].m_bIsValid) 
            {
                byDcsNum ++ ;
                if ( 1 == byDcsNum )
                {
                    OspPrintf( TRUE, FALSE, "\n" );
                }
                OspPrintf(TRUE, FALSE, " %s (Dcs.%2d) %10s %s \n",
                          m_atPeriDcsTable[byIdx-1].m_tDcsStatus.m_byOnline ? "��":"��",
                          byIdx,
                          achEqpType[EQP_TYPE_DCS],
                          m_atPeriDcsTable[byIdx-1].m_tDcsStatus.m_byOnline ? "����":"����");
            }
        }
        OspPrintf(TRUE, FALSE, "_____________________________________________________________\n\n");
        return;
    }

	OspPrintf(TRUE, FALSE, "\nEqpId = %d    ", byEqpId);
	if (!m_atPeriEqpTable[byEqpId-1].m_bIsValid)
	{
		OspPrintf(TRUE, FALSE, "NOT CONFIG\n");
		return;
	}

	if( !m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_byOnline )
	{
		OspPrintf( TRUE ,FALSE ,"    Offline\n");
		return;
	}
    
	OspPrintf(TRUE ,FALSE, "\n");
	switch( GetEqpType( byEqpId) ) 
	{
	case EQP_TYPE_BAS://����������
		{
            if ( !g_cMcuAgent.IsEqpBasHD(byEqpId) )
            {
			    nChnnlNum = m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tBas.byChnNum;
			    OspPrintf( TRUE ,FALSE ," Status:  IDLE(0),READY(1),RUNING(2)\n");
			    OspPrintf( TRUE ,FALSE ," BAS.%d has %d channels\n\n", byEqpId, nChnnlNum);
			    OspPrintf( TRUE ,FALSE ,"%3s%7s%10s%14s%10s\n","CH","Status","Type A/V","  Resolution","Bitrate");
			    for(nChnIdx =0 ;nChnIdx <  min(MAXNUM_BAS_CHNNL,nChnnlNum);nChnIdx++)
			    {
				    OspPrintf( TRUE ,FALSE ,"%3d %6d   %2d/%2d   %5d X %-5d%10d\n",nChnIdx,
					    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tBas.tChnnl[nChnIdx].GetStatus(),
					    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tBas.tChnnl[nChnIdx].GetAudType(),
					    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tBas.tChnnl[nChnIdx].GetVidType(),
					    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tBas.tChnnl[nChnIdx].GetWidth(),
					    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tBas.tChnnl[nChnIdx].GetHeight(),
					    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tBas.tChnnl[nChnIdx].GetBitrate());
			    }
            }
            else
            {
                OspPrintf( TRUE ,FALSE ," Status:  IDLE(0),READY(1),RUNING(2)\n");
                OspPrintf( TRUE ,FALSE ," Reserve type:  IDLE(0),Reserved(1),Temp reserved(2)\n");
                OspPrintf( TRUE ,FALSE ," %-6s %-3s %-5s %-10s %-14s%\n","EQPID","CH","Type","Status","Reservetype");
                for ( nChnIdx = 0; nChnIdx < MAXNUM_MAU_VCHN; nChnIdx++ )
                {
                    OspPrintf( TRUE ,FALSE ," %-6d %-3d %-5s %-10d %-4d\n",
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(nChnIdx)->GetEqpId(),
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(nChnIdx)->GetChnIdx(),
                    "VID",
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(nChnIdx)->GetStatus(),
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(nChnIdx)->GetReservedType());
                }
                for ( nChnIdx = 0; nChnIdx < MAXNUM_MAU_DVCHN; nChnIdx++ )
                {
                    OspPrintf( TRUE ,FALSE ," %-6d %-3d %-5s %-10d %-4d\n",
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(nChnIdx)->GetEqpId(),
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(nChnIdx)->GetChnIdx(),
                    "DVID",
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(nChnIdx)->GetStatus(),
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(nChnIdx)->GetReservedType());
                }
            }
		}
		break;
	case EQP_TYPE_MIXER://������
		{
			u8   byIdx;
			u8   byMmbNum;
			u8   abyActiveMmb[MAXNUM_MIXER_DEPTH];
			u8   abyMixMmb[256];
			static char status[3][7]={"  IDLE"," READY","MIXING"};
			OspPrintf( TRUE ,FALSE ,"Mixer.%d has %d Groups \n",byEqpId,m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_byGrpNum);

			for( nGrpIdx=0 ;
			nGrpIdx< m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_byGrpNum;
			nGrpIdx++)
			{
				
				OspPrintf( TRUE ,FALSE ,"\n--------------------  [ Grp.%d ]--------------------\n",m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[nGrpIdx].m_byGrpId );
				OspPrintf( TRUE ,FALSE ,"Status : %s               Depth  : %d\n",
					status[m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[nGrpIdx].m_byGrpState],
					m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[nGrpIdx].m_byGrpMixDepth);
				OspPrintf( TRUE ,FALSE ,"Active Mmb:");
				
				byMmbNum =m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[nGrpIdx].GetActiveMmb( abyActiveMmb ,MAXNUM_MIXER_DEPTH);
				for( byIdx=0 ;byIdx< byMmbNum ;byIdx++)
				{
					OspPrintf( TRUE ,FALSE ," %d",abyActiveMmb[byIdx]);		
				}
				OspPrintf( TRUE ,FALSE ,"\n");
				OspPrintf( TRUE ,FALSE ,"Mixing Member Id\n");
				
				byMmbNum = m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[nGrpIdx].GetMixMmb( abyMixMmb ,200);
				for( byIdx=0 ;byIdx< byMmbNum ;byIdx++)
				{
					OspPrintf( TRUE ,FALSE ," %3d",abyActiveMmb[byIdx]);
					if( ((byIdx+1)%10)==0 )
						OspPrintf( TRUE ,FALSE ,"\n");
				}
			}
		}

		break;
	case EQP_TYPE_RECORDER://¼���
		{
			TRecChnnlStatus tChnStatus;
			u8 byRecChnNum ,byPlayChnNum ,byChnIdx;
			TRecStatus * ptRecStatus = &m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tRecorder;
			byRecChnNum  = ptRecStatus->GetRecChnnlNum();
			byPlayChnNum = ptRecStatus->GetPlayChnnlNum();

			OspPrintf(TRUE,FALSE ,"_____________________ Recorder.%d ___________________\n",byEqpId);
			OspPrintf(TRUE,FALSE,"%-6s %-12s %-10s %6s/%-6s %s\n","  CH","    Status","    Mode","Cur(S)","Tot(S)","Recording Name ");
			for( byChnIdx = 0 ;byChnIdx < byRecChnNum ;byChnIdx++ )
			{
				ptRecStatus->GetChnnlStatus( byChnIdx ,TRecChnnlStatus::TYPE_RECORD ,&tChnStatus);
				OspPrintf( TRUE ,FALSE ,"CH.%2d  %12s %10s %6d/%-6d %s\n",byChnIdx 
					,tChnStatus.GetStatusStr( tChnStatus.m_byState)
					,tChnStatus.m_byRecMode == TRecChnnlStatus::MODE_REALTIME ?"NORMAL":"SKIP"
					,tChnStatus.m_tProg.GetCurProg(),tChnStatus.m_tProg.GetTotalTime() 
					,tChnStatus.GetRecordName());
			}

			OspPrintf(TRUE,FALSE,"\t__________________________________________\n");
			OspPrintf(TRUE,FALSE,"%-6s %-12s %6s/%-6s %s\n","  CH","    Status","Cur(S)","Tot(S)","Playing Name ");
			for( byChnIdx = 0 ;byChnIdx < byRecChnNum ;byChnIdx++ )
			{
				ptRecStatus->GetChnnlStatus( byChnIdx ,TRecChnnlStatus::TYPE_PLAY ,&tChnStatus);
				OspPrintf( TRUE ,FALSE ,"CH.%2d  %12s %6d/%-6d %s\n",byChnIdx 
					,tChnStatus.GetStatusStr( tChnStatus.m_byState) 
					,tChnStatus.m_tProg.GetCurProg(),tChnStatus.m_tProg.GetTotalTime()
					,tChnStatus.GetRecordName());
			}
		}
		break;
	case EQP_TYPE_TVWALL:
		{
			TTvWallStatus *ptTWStatus;
			ptTWStatus = &m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tTvWall;
			OspPrintf(TRUE,FALSE,"_________________%d channels TV Wall ________________\n",ptTWStatus->byChnnlNum);
			for( u8 byChnIdx=0 ;byChnIdx < ptTWStatus->byChnnlNum ;byChnIdx++)
			{
				OspPrintf(TRUE,FALSE,"Chn.%d Playing Mcu.%d MT.%d\n",byChnIdx
					,ptTWStatus->atVideoMt[byChnIdx].GetMcuId()
					,ptTWStatus->atVideoMt[byChnIdx].GetMtId());
			}
		}
		break;

	default:
		break;
	};

}

/*====================================================================
    ������      ��AssignMtPort
    ����        �������ն˶˿�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx ����������
				  u8 byMtId �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/03/02    3.0         ������          ����
====================================================================*/
u16  CMcuVcData::AssignMtPort( u8 byConfIdx, u8 byMtId )
{
	u16 wPort = 0;
	u16 wPortIndex = 0;
	while( wPortIndex < MAXNUM_MCU_MT )
	{
		if( m_atRecvMtPort[wPortIndex].m_byPortInUse )
		{
			wPortIndex++;
		}
		else
		{
			break;
		}
	}
	
	if( wPortIndex < MAXNUM_MCU_MT )
	{
		wPort = g_cMcuAgent.GetRecvStartPort() + PORTSPAN * wPortIndex;
		m_atRecvMtPort[wPortIndex].m_byPortInUse = TRUE;
		m_atRecvMtPort[wPortIndex].m_byConfIdx = byConfIdx;
		m_atRecvMtPort[wPortIndex].m_byMtId = byMtId;
	}
	else
	{
		wPort = 0;
	}

	return wPort;
}

/*====================================================================
    ������      ��ReleaseMtPort
    ����        ���ͷ��ն˶˿�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx ����������
				  u8 byMtId �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/03/02    3.0         ������          ����
====================================================================*/
void CMcuVcData::ReleaseMtPort( u8 byConfIdx, u8 byMtId )
{
	u16 wPortIndex = 0;
	while( wPortIndex < MAXNUM_MCU_MT )
	{
		if( m_atRecvMtPort[wPortIndex].m_byConfIdx == byConfIdx 
			&& m_atRecvMtPort[wPortIndex].m_byMtId == byMtId )
		{
			m_atRecvMtPort[wPortIndex].m_byPortInUse = FALSE;
			m_atRecvMtPort[wPortIndex].m_byConfIdx = 0;
			m_atRecvMtPort[wPortIndex].m_byMtId = 0;
			break;
		}
		else
		{
			wPortIndex++;
		}
	}
}

/*====================================================================
    ������      ��AssignMulticastIp
    ����        �������鲥Ip��ַ
    �㷨ʵ��    ��231.255.255.255 --- 233.0.0.0Ϊ������Χ
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��������鲥Ip��ַ - ������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	05/09/27    4.0         libo          ����
    06/04/06    4.0         ����        ���ݻ���ģ�棬�������Զ�����
====================================================================*/
u32  CMcuVcData::AssignMulticastIp(u8 byConfIdx)
{
    u32 dwMultiCastAddr = 0;
    CMcuVcInst* pVcInst = GetConfInstHandle(byConfIdx);
    if ( NULL != pVcInst )
    {
        // ����@2006.4.8 ֱ�Ӵӻ��������ȡ
        dwMultiCastAddr = pVcInst->m_tConf.GetConfAttrb().GetMulticastIp();
    }
    else
    {
        // Fatal error!
        OspPrintf(TRUE, FALSE, "Cannot get VC Instance(conf idx: %d) while AssignMulticastIp!\n", 
                            byConfIdx);
    }

    return dwMultiCastAddr;
}

/*====================================================================
    ������      ��AssignMulticastPort
    ����        �������鲥�˿�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx ����������
				  u8 byMtId �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/03/02    3.0         ������        ����
    06/04/06    4.0         ����        ���ݻ���ģ�棬�������Զ�����
====================================================================*/
u16  CMcuVcData::AssignMulticastPort( u8 byConfIdx, u8 byMtId )
{
	u16 wMultiCastPort;
	u16 wPort = 0;
	u16 wPortIndex = 0;

    CMcuVcInst* pVcInst = GetConfInstHandle(byConfIdx);
    if ( NULL == pVcInst )
    {
        OspPrintf(TRUE, FALSE, "Cannot get VC Instance(conf idx: %d) while AssignMulticastPort!\n", 
                            byConfIdx);
        return 0;
    }

    // ����@2006.4.8 ֱ�Ӵӻ��������ȡ
    wMultiCastPort = pVcInst->m_tConf.GetConfAttrb().GetMulticastPort();    
    
    // xsl [9/25/2006] �����鲥������port
    if (0 == byMtId)
    {
        return wMultiCastPort;
    }
    
	//�ö˿��Ƿ��ѷ����
	while( wPortIndex < MAXNUM_MCU_MT )
	{
		if( m_atMulticastPort[wPortIndex].m_byConfIdx == byConfIdx 
			&& m_atMulticastPort[wPortIndex].m_byMtId == byMtId )
		{           
			wPort = wMultiCastPort + 4 * wPortIndex;
			return wPort;
		}
		else
		{
			wPortIndex++;
		}
	}

    //���ҿ��ж˿�
	wPortIndex = 0;
	while( wPortIndex < MAXNUM_MCU_MT )
	{
		if( m_atMulticastPort[wPortIndex].m_byPortInUse )
		{
			wPortIndex++;
		}
		else
		{
			break;
		}
	}
	
	//����˿�
	if( wPortIndex < MAXNUM_MCU_MT )
	{
		wPort = wMultiCastPort + 4 * wPortIndex;
		m_atMulticastPort[wPortIndex].m_byPortInUse = TRUE;
		m_atMulticastPort[wPortIndex].m_byConfIdx = byConfIdx;
		m_atMulticastPort[wPortIndex].m_byMtId = byMtId;
	}
	else
	{
		wPort = 0;
	}

	return wPort;
}

/*====================================================================
    ������      ��IsMulticastAddrOccupied
    ����        ���������еļ�ʱ���飬�Ƿ���������Ӧ�ĵ�ַ
    �㷨ʵ��    ����ʱһ������ռ��һ��IP��ַ������������ռ��1��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    06/04/06    4.0         ����        ����
====================================================================*/
BOOL32 CMcuVcData::IsMulticastAddrOccupied(u32 dwCastIp, u16 wCastPort)
{
    // ��ʱ�����Ƕ˿�
	CMcuVcInst* pcVcInst = NULL;
	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_CONF; byInstID++)
	{
		pcVcInst = m_apConfInst[byInstID-1];
		if (NULL != pcVcInst)
		{
            if (pcVcInst->m_tConf.m_tStatus.IsOngoing() &&
                pcVcInst->m_tConf.GetConfAttrb().GetMulticastIp() == dwCastIp )
            {
                // �����Ѿ���ռ��
                return TRUE;
            }
		}	
	}    
    return FALSE;
}

/*====================================================================
    ������      ��ReleaseMulticastPort
    ����        ���ͷ��鲥�˿�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx ����������
				  u8 byMtId �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/03/02    3.0         ������          ����
====================================================================*/
void CMcuVcData::ReleaseMulticastPort( u8 byConfIdx, u8 byMtId )
{
	u16 wPortIndex = 0;
	while( wPortIndex < MAXNUM_MCU_MT )
	{
		if( m_atMulticastPort[wPortIndex].m_byConfIdx == byConfIdx 
			&& m_atMulticastPort[wPortIndex].m_byMtId == byMtId )
		{
			m_atMulticastPort[wPortIndex].m_byPortInUse = FALSE;
			m_atMulticastPort[wPortIndex].m_byConfIdx = 0;
			m_atMulticastPort[wPortIndex].m_byMtId = 0;
			break;
		}
		else
		{
			wPortIndex++;
		}
	}
}

/*====================================================================
    ������      ��IsDistrConfCastAddrOccupied
    ����        ���������еļ�ʱ���飬�Ƿ���������Ӧ�ķ�ɢ�����鲥��ַ
    �㷨ʵ��    ��ֻ�е���ַ�Ͷ˿ڶ���ռ�ò���Ϊ��ռ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/04/12    4.0         ����        ����
====================================================================*/
BOOL32 CMcuVcData::IsDistrConfCastAddrOccupied( u32 dwCastIp, u16 wCastPort) 
{
	CMcuVcInst* pcVcInst = NULL;
	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_CONF; byInstID++)
	{
		pcVcInst = m_apConfInst[byInstID-1];
		if (NULL != pcVcInst)
		{
            if (pcVcInst->m_tConf.m_tStatus.IsOngoing() &&
                pcVcInst->m_tConf.GetConfAttrb().GetSatDCastIp() == dwCastIp &&
                pcVcInst->m_tConf.GetConfAttrb().GetSatDCastPort() == wCastPort)
            {
                // �����Ѿ���ռ��
                return TRUE;
            }
		}	
	}    
    return FALSE;
}

/*=============================================================================
    �� �� ���� GetBaseInfoFromDebugFile
    ��    �ܣ� ��ȡָ��MCU Debug�ļ��л���������Ϣ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/22  4.0			����                  ����
=============================================================================*/
void CMcuVcData::GetBaseInfoFromDebugFile( )
{
	McuGetDebugKeyValue(m_tMcuDebugVal);

	return;
}

/*=============================================================================
    �� �� ���� GetSatInfoFromDebugFile
    ��    �ܣ� ��ȡ���ǻ�������������Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/9/11   4.6			�ű���                  ����
=============================================================================*/
void CMcuVcData::GetSatInfoFromDebugFile()
{
    McuGetSateliteConfigValue(m_tSatInfo);
}

/*=============================================================================
    �� �� ���� GetMcuTelnetPort
    ��    �ܣ� ��ȡָ��MCU Debug�ļ���MCU_TELNET�˿�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� u16 ������ 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/22  4.0			����                  ����
=============================================================================*/
u16 CMcuVcData::GetMcuTelnetPort( )
{
	return m_tMcuDebugVal.m_wMcuTelnetPort;
}

/*=============================================================================
    �� �� ���� GetBaseInfoFromDebugFile
    ��    �ܣ� ��ȡָ��MCU Debug�ļ���MCU_LISTEN�˿�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� u16 ������ 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/22  4.0			����                  ����
=============================================================================*/
u16 CMcuVcData::GetMcuListenPort( )
{
	return m_tMcuDebugVal.m_wMcuListenPort;
}

/*=============================================================================
    �� �� ���� GetMcsRefreshInterval
    ��    �ܣ� ��ȡָ��MCU Debug�ļ���MCS���ն�ת̨ˢ�¼������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� u32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/22  4.0			����                  ����
=============================================================================*/
u32 CMcuVcData::GetMcsRefreshInterval( )
{
	return m_tMcuDebugVal.m_dwMcsRefreshInterval;
}

/*=============================================================================
    �� �� ���� IsWatchDogEnable
    ��    �ܣ� ��ȡָ��MCU Debug�ļ��п��Ź��Ƿ񼤻�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/22  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsWatchDogEnable( )
{
	return m_tMcuDebugVal.m_bWDEnable;
}

/*=============================================================================
�� �� ���� GetMsSynTime
��    �ܣ� �������ͬ��ʱ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/4/2  4.0			�ܹ��                  ����
=============================================================================*/
u16	 CMcuVcData::GetMsSynTime(void)
{
	return m_tMcuDebugVal.m_wMsSynTime;
}

/*=============================================================================
    �� �� ���� GetMtCallInterfaceInfoFromDebugFile
    ��    �ܣ� ��ȡָ��MCU Debug�ļ�����ָ���ն˵�Э���������Դ������ת������Դ������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/25   3.6			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetMtCallInterfaceInfoFromDebugFile( )
{
	m_dwMtCallInterfaceNum = MAXNUM_MCU_MT;
	if( FALSE == McuGetMtCallInterfaceInfo(m_atMtCallInterface, m_dwMtCallInterfaceNum) )
	{
		memset(m_atMtCallInterface, 0, sizeof(m_atMtCallInterface));
		m_dwMtCallInterfaceNum = 0;

		return FALSE;
	}

	return TRUE;	
}

/*=============================================================================
    �� �� ���� GetMpIdAndH323MtDriIdFromMtAlias
    ��    �ܣ� �����ն˱��� ��ȡ ���ն˵�Э���������Դ������ת������Դ������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u32      [in] tMtAlias       //�ն˵ı���
	           u32      [out]&dwMtadpIpAddr //Э�������IP��ַ, ������
	           u32      [out]&dwMpIpAddr    //����ת����IP��ַ, ������
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/25   3.6			����                  ����
    2006/12/20  4.0-R4      ����                    ����E164�����֧��
=============================================================================*/
BOOL32 CMcuVcData::GetMpIdAndH323MtDriIdFromMtAlias( TMtAlias &tMtAlias, u32 &dwMtadpIpAddr, u32 &dwMpIpAddr )
{
	for( u32 dwLoop = 0; dwLoop < m_dwMtCallInterfaceNum; dwLoop++ )
	{
        switch (m_atMtCallInterface[dwLoop].m_byAliasType)
        {
        case mtAliasTypeTransportAddress:
            if( tMtAlias.m_AliasType == mtAliasTypeTransportAddress &&
                 m_atMtCallInterface[dwLoop].m_tIpSeg.IsIpIn( tMtAlias.m_tTransportAddr.GetNetSeqIpAddr() ) )
            {
			    dwMtadpIpAddr = m_atMtCallInterface[dwLoop].m_dwMtadpIpAddr;
			    dwMpIpAddr    = m_atMtCallInterface[dwLoop].m_dwMpIpAddr;
                return TRUE;
            }
            break;
        case mtAliasTypeE164:
            //�ж�E164�����Ƿ��ں������
            if( tMtAlias.m_AliasType == mtAliasTypeE164 &&
                m_atMtCallInterface[dwLoop].IsE164Same( tMtAlias.m_achAlias ) )
             {
                MtLog("[GetMpIdAndH323MtDriIdFromMtAlias]: E164=%s in E164 seg=%s.\n", tMtAlias.m_achAlias, m_atMtCallInterface[dwLoop].m_szE164);
			    dwMtadpIpAddr = m_atMtCallInterface[dwLoop].m_dwMtadpIpAddr;
			    dwMpIpAddr    = m_atMtCallInterface[dwLoop].m_dwMpIpAddr;
                return TRUE;
            }
            break;
        default:
            break;
        }
	}
	return FALSE;
}

/*=============================================================================
    �� �� ���� GetMSDetermineType
    ��    �ܣ� ��ȡָ��MCU Debug�ļ��� �����ý����ȡ��ʽ ����
	           vxworks��Ӳ��ƽ̨֧�������ù��ܣ���ֱ����os��ȡ�����ý����Ĭ�ϲ�֧��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/26  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetMSDetermineType( )
{
	return m_tMcuDebugVal.m_bMSDetermineType;
}


/*=============================================================================
�� �� ���� GetBitrateScale
��    �ܣ� ��ȡ�������ʱ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/4/11  4.0			������                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetBitrateScale()
{
    return m_tMcuDebugVal.m_byBitrateScale;
}


/*=============================================================================
�� �� ���� IsMMcuSpeaker
��    �ܣ� �Ƿ�ָ���ϼ���������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/4/11  4.0			������                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsMMcuSpeaker() const
{
    return (0 != m_tMcuDebugVal.m_byIsMMcuSpeaker);
}

/*=============================================================================
�� �� ���� IsShowMMcuMtList
��    �ܣ� ����ʱ�Ƿ���ʾ�ϼ�MCU���ն��б�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/04/30  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsShowMMcuMtList() const
{
    return (0 != m_tMcuDebugVal.m_byShowMMcuMtList);
}

/*=============================================================================
�� �� ���� SetSMcuCasPort
��    �ܣ� ���������ӵ��¼�MCU�������˿ڡ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/6/15  4.0			����                  ����
=============================================================================*/
void CMcuVcData::SetSMcuCasPort(u16 wPort)
{
    m_tMcuDebugVal.m_wSMcuCasPort = wPort;
}

/*=============================================================================
�� �� ���� GetSMcuCasPort
��    �ܣ� ���������ӵ��¼�MCU�������˿ڡ���ֵ��telnet����ͨ��SetSMcuCasPort����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� u16 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/6/15  4.0			����                  ����
=============================================================================*/
u16 CMcuVcData::GetSMcuCasPort() const
{
    return m_tMcuDebugVal.m_wSMcuCasPort;
}

/*=============================================================================
�� �� ���� IsAutoDetectMMcuDupCall
��    �ܣ� ���ϼ�MCU���º���ʱ���Ƿ����Ƿ���ͬһ���ϼ�MCU
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/6/22  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsAutoDetectMMcuDupCall() const
{
    return (0 != m_tMcuDebugVal.m_byIsAutoDetectMMcuDupCall);
}

/*=============================================================================
�� �� ���� GetCascadeAliasType
��    �ܣ� ��ȡ����ʱMCU�ڻ����б�������ʾ��ʽ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 0 - MCU Alias+Conf H.323ID,  1 - Mcu Alias Only, 2 - Conf H.323ID Only
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/4/30   4.0			����                  ����
=============================================================================*/
u8 CMcuVcData::GetCascadeAliasType() const
{
    return m_tMcuDebugVal.m_byCascadeAliasType;
}

/*=============================================================================
�� �� ���� IsApplyChairToZxMcu
��    �ܣ� �Ƿ�������mcu������ϯ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/6/22  4.0			������                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsApplyChairToZxMcu() const
{
	return (0 != m_tMcuDebugVal.m_byIsApplyChairToZxMcu);
}

/*=============================================================================
�� �� ���� IsTransmitMtShortMsg
��    �ܣ� �Ƿ�ת���ն˶���Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/3  4.0			������                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsTransmitMtShortMsg() const
{
    return (0 != m_tMcuDebugVal.m_byIsTransmitMtShortMsg);
}

/*=============================================================================
�� �� ���� IsChairDisplayMtApplyInfo
��    �ܣ� �Ƿ��ն�������ϯ�����˵���ʾ��Ϣ������ϯ�ն�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/3  4.0			������                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsChairDisplayMtApplyInfo() const
{
    return (0 != m_tMcuDebugVal.m_byIsChairDisplayMtApplyInfo);
}

/*=============================================================================
�� �� ���� IsSelInDoubleMediaConf
��    �ܣ� �Ƿ�����˫��ʽ����ѡ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/10  4.0			������                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsSelInDoubleMediaConf() const
{
    return (0 != m_tMcuDebugVal.m_byIsSelInDoubleMediaConf);
}

/*=============================================================================
�� �� ���� IsLimitAccessByMtModal
��    �ܣ� �Ƿ�����ն˺�MCU�����������ն˺���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/05/14  4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsLimitAccessByMtModal() const
{
    return (0 == m_tMcuDebugVal.m_byIsNotLimitAccessByMtModal);
}

/*=============================================================================
�� �� ���� IsSupportSecDSCap
��    �ܣ� �Ƿ����֧�ֵڶ�˫������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/15  4.5			�ű���                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsSupportSecDSCap() const
{
    return (0 != m_tMcuDebugVal.m_byIsSupportSecDSCap);
}

/*=============================================================================
�� �� ���� IsAllowVmpMemRepeated
��    �ܣ� �Ƿ�����ͨ����VMP��Ա�ظ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��  
��  ��		�汾		�޸���		�߶���    �޸�����
2009/03/21  4.5			�ű���                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsAllowVmpMemRepeated() const
{
    return (0 != m_tMcuDebugVal.m_byIsAllowVmpMemRepeated);
}
/*=============================================================================
�� �� ���� IsSendFakeCap2Polycom
��    �ܣ� �Ƿ�Լ������polycom��������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/05/14  4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsSendFakeCap2Polycom() const
{
    return (1 == m_tMcuDebugVal.m_byIsSendFakeCap2Polycom);
}
/*=============================================================================
�� �� ���� IsSendFakeCap2Taide
��    �ܣ� �Ƿ�Լ������polycom��������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/05/14  4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsSendFakeCap2Taide() const
{
    return (1 == m_tMcuDebugVal.m_byIsSendFakeCap2Taide);
}
/*=============================================================================
�� �� ���� IsSendFakeCap2TaideHD
��    �ܣ� �Ƿ��ֶ���������polycom��������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/08/31  4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsSendFakeCap2TaideHD() const
{
    return (1 == m_tMcuDebugVal.m_byIsSendFakeCap2TaideHD);
}

/*=============================================================================
 �� �� ���� IsAdpResourceCompact
 ��    �ܣ� �Ƿ����ʹ�õ�ǰ��Ƶ��Դ
 �㷨ʵ�֣� 
 ȫ�ֱ����� 
 ��    ���� 
 �� �� ֵ�� BOOL32 
 ----------------------------------------------------------------------
 �޸ļ�¼    ��
 ��  ��		�汾		�޸���		�߶���    �޸�����
 2009/05/22  4.5		�ű���                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsAdpResourceCompact() const
{
    return (0 != m_tMcuDebugVal.m_byIsAdpResourceCompact);
}

/*=============================================================================
 �� �� ���� IsSVmpOutput1080i
 ��    �ܣ� �Ƿ�ǿ��SVMP�������1080i����ͬʱ����720p
 �㷨ʵ�֣� 
 ȫ�ֱ����� 
 ��    ���� 
 �� �� ֵ�� BOOL32 
 ----------------------------------------------------------------------
 �޸ļ�¼    ��
 ��  ��		    �汾		�޸���		�߶���    �޸�����
 2009/07/26     4.5		    �ű���                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsSVmpOutput1080i() const
{
    return (0 != m_tMcuDebugVal.m_byIsSVmpOutput1080i);
}


BOOL32 CMcuVcData::IsConfAdpManually() const
{
	return (0 != m_tMcuDebugVal.m_byConfAdpManually);
}

/*=============================================================================
 �� �� ���� GetBandWidthReserved4HdBas
 ��    �ܣ� ��ȡΪHD-BASԤ���Ĵ���
 �㷨ʵ�֣� 
 ȫ�ֱ����� 
 ��    ���� 
 �� �� ֵ�� BOOL32 
 ----------------------------------------------------------------------
 �޸ļ�¼    ��
 ��  ��		    �汾		�޸���		�߶���    �޸�����
 2009/07/26     4.6		    �ű���                  ����
=============================================================================*/
u8 CMcuVcData::GetBandWidthReserved4HdBas() const
{
    return m_tMcuDebugVal.m_byBandWidthReserved4HdBas;
}

/*=============================================================================
 �� �� ���� GetBandWidthReserved4HdVmp
 ��    �ܣ� ��ȡΪHD-VMPԤ���Ĵ���
 �㷨ʵ�֣� 
 ȫ�ֱ����� 
 ��    ���� 
 �� �� ֵ�� BOOL32 
 ----------------------------------------------------------------------
 �޸ļ�¼    ��
 ��  ��		    �汾		�޸���		�߶���    �޸�����
 2009/07/26     4.6		    �ű���                  ����
=============================================================================*/
u8 CMcuVcData::GetBandWidthReserved4HdVmp() const
{
    return m_tMcuDebugVal.m_byBandWidthReserved4HdVmp;
}


BOOL32 CMcuVcData::IsDistinguishHDSDMt() const
{
	return (1 == m_tMcuDebugVal.m_byIsDistingtishSDHDMt);
}

BOOL32 CMcuVcData::IsVidAdjustless4Polycom() const
{
	return (1 == m_tMcuDebugVal.m_byIsVidAdjustless4Polycom);
}

BOOL32 CMcuVcData::IsSelAccord2Adp() const
{
    return (1 == m_tMcuDebugVal.m_byIsSelAccord2Adp);
}

/*=============================================================================
�� �� ���� GetPerfLimit
��    �ܣ� ��ȡ���ܲ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� TMcuPerfLimit
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/1/17  4.0			����                  ����
=============================================================================*/
TMcuPerfLimit& CMcuVcData::GetPerfLimit()
{
    return m_tMcuDebugVal.m_tPerfLimit;
}

/*=============================================================================
�� �� ���� ShowDebugInfo
��    �ܣ� ��ӡDebug�ļ�����Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/9/05   4.0			����                ����
2006/12/20  4.0          ����                 ����E164��������
2007/01/26  4.0			�ܹ��				  ���Ӷ�Licnese������ʾ���	
=============================================================================*/
void CMcuVcData::ShowDebugInfo()
{
    m_tMcuDebugVal.Print();

    OspPrintf(TRUE, FALSE, "\n");

#ifdef _MINIMCU_
    TMcu8kbPfmFilter tFilter = CMcuPfmLmt::GetFilter();
    tFilter.Print();
#endif

    OspPrintf(TRUE, FALSE, "\nMt Call Interface Table: %d\n", m_dwMtCallInterfaceNum);
    if ( m_dwMtCallInterfaceNum > 0 )
    {
        OspPrintf(TRUE, FALSE, "IP Start(E164)\tIP End\t\tMTAdp\t\tMP\n");
        for (u32 nLoop = 0; nLoop < m_dwMtCallInterfaceNum; nLoop ++)
        {
            if (m_atMtCallInterface[nLoop].m_byAliasType == mtAliasTypeTransportAddress)
            {
                OspPrintf(TRUE, FALSE, "0x%x\t0x%x\t0x%x\t0x%x\n",
                    ntohl(m_atMtCallInterface[nLoop].m_tIpSeg.dwIpStart),
                    ntohl(m_atMtCallInterface[nLoop].m_tIpSeg.dwIpEnd),
                    (m_atMtCallInterface[nLoop].m_dwMtadpIpAddr),
                    (m_atMtCallInterface[nLoop].m_dwMpIpAddr));
            }
            else if  (m_atMtCallInterface[nLoop].m_byAliasType == mtAliasTypeE164)
            {
                OspPrintf(TRUE, FALSE, "%s\t\t\t0x%x\t0x%x\n",
                    (m_atMtCallInterface[nLoop].m_szE164),
                    (m_atMtCallInterface[nLoop].m_dwMtadpIpAddr),
                    (m_atMtCallInterface[nLoop].m_dwMpIpAddr));
            }
            else
            {
                OspPrintf(TRUE, FALSE, "Unknown Alias: %s\t%s\n",
                    (m_atMtCallInterface[nLoop].m_szE164) );
            }
        }               
    }
    OspPrintf(TRUE, FALSE, "\n\n");

    TMcuStatus tStatus;
    GetMcuCurStatus(tStatus);
    tStatus.Print();   
}

/*=============================================================================
    �� �� ���� GetMcuCurStatus
    ��    �ܣ� ��ȡ��ǰMCU״̬
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/31  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetMcuCurStatus( TMcuStatus &tMcuStatus )
{
	memset( &tMcuStatus, 0 ,sizeof(TMcuStatus) );
	tMcuStatus.SetMcu( LOCAL_MCUID );
	u32 dwEqpIP = 0;
	u8  byEqpType = 0;
	u8  byEqpId = 1;
    
	//����״̬(������������)
	while (byEqpId <= MAXNUM_MCU_PERIEQP)
	{
		if (IsPeriEqpValid(byEqpId))
		{
			g_cMcuAgent.GetPeriInfo(byEqpId, &dwEqpIP, &byEqpType);
			tMcuStatus.m_atPeriEqp[tMcuStatus.m_byPeriEqpNum] = GetEqp(byEqpId);
			tMcuStatus.m_byEqpOnline[tMcuStatus.m_byPeriEqpNum]  = IsPeriEqpConnected(byEqpId);
            tMcuStatus.m_dwPeriEqpIpAddr[tMcuStatus.m_byPeriEqpNum] = GetEqpIp(byEqpId);
            
			tMcuStatus.m_byPeriEqpNum++;
		}
		byEqpId++;
	}

    u32 dwDcsIP = 0;
    u8  byDcsType = 0;
    u8  byDcsId = 1;

    //DCS״̬(��������DCS)
    while (byDcsId <= MAXNUM_MCU_DCS) 
    {
        if (IsPeriDcsValid(byDcsId)) 
        {
            TPeriDcsStatus tDcsStatus;
            tMcuStatus.m_atPeriDcs[tMcuStatus.m_byPeriDcsNum] = GetDcs(byDcsId);
            tMcuStatus.m_byDcsOnline[tMcuStatus.m_byPeriDcsNum] = IsPeriDcsConnected(byDcsId);
            //Ŀǰֻ֧��һ���������� MCU������չ
            tMcuStatus.m_dwPeriDcsIpAddr[tMcuStatus.m_byPeriDcsNum] = g_cMcuAgent.GetDcsIp();
            tMcuStatus.m_byPeriDcsNum ++ ;
        }
        byDcsId ++ ;
    }

	//��ʱ�������, ԤԼ�������, ���л�������ն�����
	CApp* pcApp = &g_cMcuVcApp;
	CMcuVcInst* pcVcInst = NULL;
	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_CONF; byInstID++)
	{
		pcVcInst = (CMcuVcInst *)pcApp->GetInstance(byInstID);
		if (NULL != pcVcInst)
		{            
            if ( pcVcInst->CurState() == CMcuVcInst::STATE_ONGOING )
			{
				tMcuStatus.m_byOngoingConfNum++;
				tMcuStatus.m_wAllJoinedMtNum += pcVcInst->m_tConfAllMtInfo.GetLocalJoinedMtNum();
			}
            if ( pcVcInst->CurState() == CMcuVcInst::STATE_SCHEDULED )
			{
				tMcuStatus.m_byScheduleConfNum++;
			}
		}
	}
	tMcuStatus.m_wAllJoinedMtNum = htons(tMcuStatus.m_wAllJoinedMtNum);

    // ���� [5/29/2006] ����MCUע��GK�����
    tMcuStatus.m_byRegedGk = GetConfRegState(0);

    if ( g_cMSSsnApp.IsDoubleLink() )
    {
        tMcuStatus.m_tMSSynState.SetEntityType(TMSSynState::emNone);
        if ( g_cMSSsnApp.IsRemoteMpcConnected() )
        {
            if ( !g_cMSSsnApp.IsMSSynOK() ) 
            {
                tMcuStatus.m_tMSSynState = g_cMSSsnApp.GetCurMSSynState();
            }
        }
        else
        {
            // guzh [9/25/2006] ���Ϊδ����
            tMcuStatus.m_tMSSynState.SetEntityType(TMSSynState::emMpc);
        }
    }
	
    // License ����
	tMcuStatus.m_wLicenseNum = htons( GetLicenseNum() );

    // ���ߵ�Mp��MtAdp
    tMcuStatus.m_byRegedMpNum = GetMpNum();
    tMcuStatus.m_byRegedMtAdpNum = GetMtAdpNum(PROTOCOL_TYPE_H323);

    //N+1��������ͬ��״̬
    tMcuStatus.m_dwNPlusReplacedMcuIp = 0;
    switch (g_cNPlusApp.GetLocalNPlusState())
    {
    case MCU_NPLUS_IDLE:
        tMcuStatus.m_byNPlusState = NPLUS_NONE;
        break;
    case MCU_NPLUS_MASTER_IDLE:
        tMcuStatus.m_byNPlusState = NPLUS_BAKCLIENT_IDLE;
        break;
    case MCU_NPLUS_MASTER_CONNECTED:
        tMcuStatus.m_byNPlusState = NPLUS_BAKCLIENT_CONNECT;
        break;
    case MCU_NPLUS_SLAVE_IDLE:
        tMcuStatus.m_byNPlusState = NPLUS_BAKSERVER_IDLE;
        break;
    case MCU_NPLUS_SLAVE_SWITCH:
        tMcuStatus.m_byNPlusState = NPLUS_BAKSERVER_SWITCHED;
        if (!g_cNPlusApp.GetNPlusSynOk())
        {
            tMcuStatus.m_byNPlusState = NPLUS_BAKSERVER_SWITCHERR;
        }
        tMcuStatus.m_dwNPlusReplacedMcuIp = htonl(g_cNPlusApp.GetMcuSwitchedIp());
        break;
    default:
        tMcuStatus.m_byNPlusState = NPLUS_NONE;
        break;
    }
	
	//MCU�Ƿ����ù��ı�ʶ zgc [12/21/2006] for֧��MCU������������
	tMcuStatus.m_byMcuIsConfiged = g_cMcuAgent.IsMcuConfiged() ? 1 : 0;
	
	// �Ƿ���MP zgc [07/25/2007]
	if (tMcuStatus.m_byRegedMpNum > 0)
	{
		tMcuStatus.SetIsExistMp( TRUE );
	}
	else
	{
		tMcuStatus.SetIsExistMp( FALSE );
	}
	
	// �Ƿ���mtadp zgc [07/25/2007]
	if ( tMcuStatus.m_byRegedMtAdpNum > 0 )
	{ 
		tMcuStatus.SetIsExistMtadp( TRUE );
	}
	else
	{
		tMcuStatus.SetIsExistMtadp( FALSE );
	}
	
	// mcu������Ϣ���ż���  zgc [07/25/2007]
	tMcuStatus.m_byMcuCfgLevel = g_cMcuAgent.GetMcuCfgInfoLevel();

#ifdef _MINIMCU_
    // ���Ǽ����ϰ汾������DSC������IsExistDSC, ���������ɻ�زദ��, zgc, 2008-06-27
	u8 byDSCType = BRD_TYPE_UNKNOW;
	if ( g_cMcuAgent.IsDscReged(byDSCType) )
	{
		switch(byDSCType)
		{
		case BRD_TYPE_DSC:
			tMcuStatus.SetIsExistDSC( TRUE );
			break;
		case BRD_TYPE_MDSC:
            tMcuStatus.SetIsExistDSC( TRUE );
			tMcuStatus.SetIsExistMDSC( TRUE );
			break;
		case BRD_TYPE_HDSC:
            tMcuStatus.SetIsExistDSC( TRUE );
			tMcuStatus.SetIsExistHDSC( TRUE );
			break;
		default:
			OspPrintf( TRUE, FALSE, "[GetCurMcuStatus] unexpected reged dsc type.%d\n", byDSCType );
			break;
		}
	}
#endif

    //����MCU״̬λ zgc [07/25/2007]
    // guzh [8/2/2007] Ŀǰ�ı�׼����License����MP/MtAdp ������
    BOOL32 bRunOk = tMcuStatus.IsExistMp() && 
                  tMcuStatus.IsExistMtadp() && 
                  (ntohs(tMcuStatus.m_wLicenseNum) > 0);
#ifdef _MINIMCU_		
#ifndef WIN32
    // ����� MINIMCU ���ڷ��������ý����ת��������£������û����£����������ע���DSCģ��
    if ( !g_cMcuAgent.GetIsUseMpcTranData() && !g_cMcuAgent.GetIsUseMpcStack() )
    {
        bRunOk &= tMcuStatus.IsExistDSC();
    }	
#endif
#endif
    tMcuStatus.SetIsMcuRunOk( bRunOk );

    //zbq[12/13/2007] MCU��������ʱ���ȡ
    tMcuStatus.SetPersistantRunningTime(OspTickGet()/OspClkRateGet());

	// xliang [11/20/2008] HDI ��������
#ifndef _MINIMCU_
	u16 wAllHdiAccessMtNum = 0;
	for(u8 byHdiId=1; byHdiId <= MAXNUM_DRI; byHdiId++)
	{
		if(!IsMtAdpConnected(byHdiId) || 
			byHdiId == MCU_BOARD_MPC  ||
			byHdiId == MCU_BOARD_MPCD ||
            !g_cMcuVcApp.m_atMtAdpData[byHdiId-1].m_byIsSupportHD)
		{
			continue;
		}
		wAllHdiAccessMtNum += m_atMtAdpData[byHdiId-1].m_byMaxMtNum;
	}
	tMcuStatus.m_wAllHdiAccessMtNum = wAllHdiAccessMtNum;
#else
	tMcuStatus.m_wAllHdiAccessMtNum = ~0;  //��65535��ʾ:  8000b������HDI
#endif

	// xliang [11/20/2008] CRI(��������)�������� (�ݲ�֧�֣�Ԥ��)
	tMcuStatus.m_wStdCriAccessMtNum = ~0;

    if ( g_bPrintCfgMsg )
    {
        tMcuStatus.Print();
    }

	return TRUE;
}

/*=============================================================================
    �� �� ���� GetMcuCurUserList
    ��    �ܣ� ��ȡ��ǰMCU�û��б�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  CUsrManage& cUsrManageObj, �������û��������
			   [IN]  u8 byGrpId,                �û���ID
               [OUT] u8 *pbyBuf,                ������
	           [IN/OUT] u8 &byUsrItr,           �������ָ��
	           [IN/OUT] u8 &byUserNumInPack     �˰��е���Ч�û���

    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/31  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetMcuCurUserList(CUsrManage& cUsrManageObj, u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack )
{    
    //��ΪN+1���ݹ���ģʽ�����л��󣩣�ȡ�ڴ��е���Ϣ
    if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
    {
        u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
        if (0 == byInsId || byInsId > MAXNUM_NPLUS_MCU)
        {
            OspPrintf(TRUE, FALSE, "[GetMcuCurUserList] McuSwitchedInsId is invalid in NPlus switched mode.\n");
            return FALSE;
        }
        else
        {
            CApp *pcApp = &g_cNPlusApp;
            CNPlusInst *pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL != pcInst)
            {
                return pcInst->GetGrpUserList(byGrpId, pbyBuf, byUsrItr, byUserNumInPack);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[GetMcuCurUserList] GetInstance(%d) failed.\n", byInsId);
                return FALSE;
            }
        }                
    }

    byUserNumInPack = 0;
	if (NULL == pbyBuf)
	{
		return FALSE;
	}

	CExUsrInfo cUserInfo;

    s32 nTotalCount = cUsrManageObj.GetUserNum();
	for (; byUserNumInPack < USERNUM_PERPKT && byUsrItr < nTotalCount; byUsrItr ++ )
	{
		cUserInfo.Empty();
		if ( cUsrManageObj.GetUserFullInfo(&cUserInfo, byUsrItr) && 
			 !cUserInfo.IsEmpty() )
		{

            if ( byGrpId == USRGRPID_SADMIN ||
                 byGrpId == cUserInfo.GetUsrGrpId()  )
            {
			    memcpy(pbyBuf + byUserNumInPack * sizeof(CExUsrInfo), &cUserInfo, sizeof(CExUsrInfo));
			    byUserNumInPack++;
            }
		}
	}

	if (0 == byUserNumInPack)
	{
		return FALSE;
	}
	
	return TRUE;
}
/*=============================================================================
    �� �� ���� GetMCSCurUserList
    ��    �ܣ� ��ȡ��ǰMCS�û��б�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ����
			   [IN]  u8 byGrpId,                �û���ID
               [OUT] u8 *pbyBuf,                ������
	           [IN/OUT] u8 &byUsrItr,           �������ָ��
	           [IN/OUT] u8 &byUserNumInPack     �˰��е���Ч�û���

    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    08/11/21     		    ���㻪                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetMCSCurUserList(u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack )
{
	return GetMcuCurUserList(g_cUsrManage, byGrpId, pbyBuf, byUsrItr, byUserNumInPack);
}

/*=============================================================================
    �� �� ���� GetVCSCurUserList
    ��    �ܣ� ��ȡ��ǰVCS�û��б�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ����
			   [IN]  u8 byGrpId,                �û���ID
               [OUT] u8 *pbyBuf,                ������
	           [IN/OUT] u8 &byUsrItr,           �������ָ��
	           [IN/OUT] u8 &byUserNumInPack     �˰��е���Ч�û���

    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    08/11/21     		    ���㻪                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetVCSCurUserList(u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack )
{
	return GetMcuCurUserList(g_cVCSUsrManage, byGrpId, pbyBuf, byUsrItr, byUserNumInPack);
}

/*=============================================================================
  �� �� ���� CreateTemplate
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::CreateTemplate(void)
{
    if(NULL == m_ptTemplateInfo)
    {
        m_ptTemplateInfo = new TTemplateInfo[MAXNUM_MCU_TEMPLATE];
        if(NULL == m_ptTemplateInfo)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*=============================================================================
  �� �� ���� AddTemplate
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� [IN/OUT]       TTemplateInfo &tTemInfo
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::AddTemplate(TTemplateInfo &tTemInfo)
{
    // guzh [4/11/2007] ����Ƿ���ļ��ָ�
    BOOL32 bTemplateFromFile =  tTemInfo.m_tConfInfo.m_tStatus.IsTakeFromFile();

    //zbq [09/06/2007] ���ֶ���SaveConfToFile��ͳһ����
    //tTemInfo.m_tConfInfo.m_tStatus.SetTakeFromFile(FALSE);

	u8 byIndex = MAXNUM_MCU_TEMPLATE;
	u8 byVCSTempNum = 0;
	u8 byMCSTempNum = 0;
    for(u8 byInd = 0; byInd < MAXNUM_MCU_TEMPLATE; byInd++)
    {

        if(m_ptTemplateInfo[byInd].IsEmpty())
        {
			if (MAXNUM_MCU_TEMPLATE == byIndex)
			{
				byIndex = byInd;
			}
        }
		else
		{
            if (VCS_CONF == m_ptTemplateInfo[byInd].m_tConfInfo.GetConfSource())
            {
				byVCSTempNum++;
            }
			else if (MCS_CONF == m_ptTemplateInfo[byInd].m_tConfInfo.GetConfSource())
			{
				byMCSTempNum++;
			}
		}
    }	

	if (VCS_CONF == tTemInfo.m_tConfInfo.GetConfSource() && byVCSTempNum >= MAXNUM_MCU_VCSTEMPLATE )
	{
		return FALSE;
	}
	else if (MCS_CONF == tTemInfo.m_tConfInfo.GetConfSource() && byMCSTempNum >= MAXNUM_MCU_MCSTEMPLATE )
	{
		return FALSE;
	}

    //template array full
    if(MAXNUM_MCU_TEMPLATE == byIndex)
    {
        return FALSE;
    }

    u8 byConfIdx = GetOngoingConfIdxByE164(tTemInfo.m_tConfInfo.GetConfE164());
    if(0 == byConfIdx)
    {
        byConfIdx = GetIdleConfidx();
        if(0 == byConfIdx)
        {
            return FALSE;
        }
    }    

    //save template info
    if(CONF_LOCKMODE_LOCK == tTemInfo.m_tConfInfo.m_tStatus.GetProtectMode())
    {
        tTemInfo.m_tConfInfo.m_tStatus.SetProtectMode(CONF_LOCKMODE_NONE);
    }    
    tTemInfo.m_tConfInfo.m_tStatus.SetRegToGK(FALSE);
    // guzh [4/11/2007] ������ļ��ָ�,������ԭ����ConfId

    if (!bTemplateFromFile)    
    {
		// ����ID�б�����������MCS���õ�����Դ��Ϣ
		CConfId cConfID = MakeConfId(byConfIdx, 1, tTemInfo.m_tConfInfo.GetUsrGrpId(),
			                         tTemInfo.m_tConfInfo.GetConfSource());
        tTemInfo.m_tConfInfo.SetConfId(cConfID);
    }
    
    tTemInfo.m_byConfIdx = byConfIdx;
    memcpy(&m_ptTemplateInfo[byIndex], &tTemInfo, sizeof(tTemInfo));

    //save map info
    m_atConfMapData[byConfIdx-MIN_CONFIDX].SetTemIndex(byIndex);    

    //save to file
    SaveConfToFile(byConfIdx, TRUE);

    //reg to gk
    RegisterConfToGK(byConfIdx, GetRegGKDriId(), TRUE);

    return TRUE;
}

/*=============================================================================
  �� �� ���� ModifyTemplate
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TTemplateInfo &tTemInfo
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::ModifyTemplate(TTemplateInfo &tTemInfo, BOOL32 bSameE164AndName)
{
    if(tTemInfo.m_byConfIdx < MIN_CONFIDX || tTemInfo.m_byConfIdx > MAX_CONFIDX)
    {
        return FALSE;
    }
  
//     if(bSameE164AndName)
//     {
        if(CONF_LOCKMODE_LOCK == tTemInfo.m_tConfInfo.m_tStatus.GetProtectMode())
        {
            tTemInfo.m_tConfInfo.m_tStatus.SetProtectMode(CONF_LOCKMODE_NONE);       
        }        
        tTemInfo.m_tConfInfo.m_tStatus.SetTemplate();

		u8 byConfIdx = tTemInfo.m_byConfIdx;
        TConfMapData tMapData = GetConfMapData(byConfIdx);
        if(!tMapData.IsTemUsed())
        {
            return FALSE;
        }

		// ����164�Ÿı�Ļ���ģ�壬����Ҫ���ԭģ����GK�ϵĽ�ע�ἰ��ģ���ע��
		// ������ȡɾ��������������ӵĲ��ԣ�û��Ҫ���ҵ���confid�ı�Ӱ���û�������Ϣ
		if (bSameE164AndName)
		{
			// ��ע��
			CMcuVcInst *pcInst = GetConfInstHandle(byConfIdx);
			if (!m_atConfMapData[byConfIdx-MIN_CONFIDX].IsValidConf() || 
			   (NULL != pcInst && pcInst->m_tConf.m_tStatus.IsScheduled()))
			{
				if ( 0 != g_cMcuVcApp.GetConfRegState( byConfIdx ) )
				{
					RegisterConfToGK(byConfIdx, GetRegGKDriId(), TRUE, TRUE);
				}
			} 
			
		}

		// ��ģ��ע��
		tTemInfo.m_tConfInfo.m_tStatus.SetRegToGK(FALSE);
        memcpy(&m_ptTemplateInfo[tMapData.GetTemIndex()], &tTemInfo, sizeof(tTemInfo));
		RegisterConfToGK(byConfIdx, GetRegGKDriId(), TRUE);
			
        //save to file
        SaveConfToFile(byConfIdx, TRUE);
//     }
//     else
//     {
//         if(!DelTemplate(tTemInfo.m_byConfIdx))
//         {
//             return FALSE;
//         }
//         
//         if(!AddTemplate(tTemInfo))
//         {
//             return FALSE;
//         }   
//     }    

    return TRUE;
}

/*=============================================================================
  �� �� ���� DelTemplate
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::DelTemplate(u8 byConfIdx)
{
    if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return FALSE;
    }

    //clear template info
    u8 byIndex = m_atConfMapData[byConfIdx-MIN_CONFIDX].GetTemIndex();
    if(byIndex >= MAXNUM_MCU_TEMPLATE) 
    {
        return FALSE;
    }
            
    //unreg to gk
    CMcuVcInst *pcInst = GetConfInstHandle(byConfIdx);
    if (!m_atConfMapData[byConfIdx-MIN_CONFIDX].IsValidConf() || 
       (NULL != pcInst && pcInst->m_tConf.m_tStatus.IsScheduled()))
    {
        // zbq [03/30/2007] ģ��δע��ɹ�������ע��
        if ( 0 != g_cMcuVcApp.GetConfRegState( byConfIdx ) )
        {
            RegisterConfToGK(byConfIdx, GetRegGKDriId(), TRUE, TRUE);
        }
    }    
    
    //remove to file
    if(!RemoveConfFromFile(m_ptTemplateInfo[byIndex].m_tConfInfo.GetConfId()))
    {
        OspPrintf(TRUE, FALSE, "[DelTemplate] Remove Conf From File failed!\n");
    }

    m_ptTemplateInfo[byIndex].Clear();

    //clear map info
    m_atConfMapData[byConfIdx-MIN_CONFIDX].SetTemIndex(MAXNUM_MCU_TEMPLATE); 

    return TRUE;
}

/*=============================================================================
  �� �� ���� GetTemplate
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
             TTemplateInfo &tTemInfo
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::GetTemplate(u8 byConfIdx, TTemplateInfo &tTemInfo)
{
    if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return FALSE;
    }

    u8 byIndex = m_atConfMapData[byConfIdx-MIN_CONFIDX].GetTemIndex();
    if(byIndex >= MAXNUM_MCU_TEMPLATE) 
    {
        return FALSE;
    }
    memcpy(&tTemInfo, &m_ptTemplateInfo[byIndex], sizeof(TTemplateInfo));

    return TRUE;
}

u32 CMcuVcData::GetExistSatCastIp()
{
    u32 dwSatCastIp = 0xffffffff;

    for (u8 byConfIdx = 1; byConfIdx <= MAX_CONFIDX; byConfIdx++)
    {
        if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
        {
            continue;
        }
        
        u8 byIndex = m_atConfMapData[byConfIdx-MIN_CONFIDX].GetTemIndex();
        if(byIndex >= MAXNUM_MCU_TEMPLATE) 
        {
            continue;
        }
        dwSatCastIp = m_ptTemplateInfo[byIndex].m_tConfInfo.GetConfAttrb().GetSatDCastIp();
        if (dwSatCastIp == 0)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    return dwSatCastIp;
}

u16 CMcuVcData::GetExistSatCastPort()
{
    u16 wSatCastPort = 0xffff;
    
    for (u8 byConfIdx = 1; byConfIdx <= MAX_CONFIDX; byConfIdx++)
    {
        if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
        {
            continue;
        }
        
        u8 byIndex = m_atConfMapData[byConfIdx-MIN_CONFIDX].GetTemIndex();
        if(byIndex >= MAXNUM_MCU_TEMPLATE) 
        {
            continue;
        }
        wSatCastPort = m_ptTemplateInfo[byIndex].m_tConfInfo.GetConfAttrb().GetSatDCastPort();
        if (wSatCastPort == 0)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    return wSatCastPort;
}

/*=============================================================================
  �� �� ���� SetConfMapInsId
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
             u8 byInsId
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::SetConfMapInsId(u8 byConfIdx, u8 byInsId)
{
    if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return FALSE;
    }

    m_atConfMapData[byConfIdx-MIN_CONFIDX].SetInsId(byInsId);

    return TRUE;
}

/*=============================================================================
  �� �� ���� GetConfMapInsId
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
  �� �� ֵ�� u8  
=============================================================================*/
u8  CMcuVcData::GetConfMapInsId(u8 byConfIdx)
{
    if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return 0;
    }

    return m_atConfMapData[byConfIdx-MIN_CONFIDX].GetInsId();
}

/*=============================================================================
  �� �� ���� GetConfMapData
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
  �� �� ֵ�� TConfMapData  
=============================================================================*/
TConfMapData  CMcuVcData::GetConfMapData(u8 byConfIdx)
{
    return m_atConfMapData[byConfIdx-MIN_CONFIDX];
}

/*=============================================================================
  �� �� ���� SetTemRegGK
  ��    �ܣ� ����ģ��ע��gk״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� BOOL32 bReg
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::SetTemRegGK(u8 byConfIdx, BOOL32 bReg)
{
    CHECK_CONFIDX(byConfIdx)

    u8 byIndex = m_atConfMapData[byConfIdx-MIN_CONFIDX].GetTemIndex();
    if(byIndex >= MAXNUM_MCU_TEMPLATE) 
    {
        return FALSE;
    }
    
    m_ptTemplateInfo[byIndex].m_tConfInfo.m_tStatus.SetRegToGK(bReg);
    return TRUE;
}

/*=============================================================================
  �� �� ���� GetTemRegGK
  ��    �ܣ� ģ��ע��gk״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::IsTemRegGK(u8 byConfIdx)
{
    CHECK_CONFIDX(byConfIdx)

    u8 byIndex = m_atConfMapData[byConfIdx-MIN_CONFIDX].GetTemIndex();
    if(byIndex >= MAXNUM_MCU_TEMPLATE) 
    {
        return FALSE;
    } 

    return m_ptTemplateInfo[byIndex].m_tConfInfo.m_tStatus.IsRegToGK();
}

/*=============================================================================
  �� �� ���� GetIdleConfidx
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� u8  
=============================================================================*/
u8    CMcuVcData::GetIdleConfidx(void)
{    
    for(u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
    {
        if (!m_atConfMapData[byConfIdx-MIN_CONFIDX].IsValidConf() && 
            !m_atConfMapData[byConfIdx-MIN_CONFIDX].IsTemUsed())
        {
            return byConfIdx;
        }
    }

    return 0;
}

/*=============================================================================
  �� �� ���� SetLicenseNum
  ��    �ܣ� ����License
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wLicenseValue
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::SetLicenseNum( u16 wLicenseValue )
{
	m_tMcuDebugVal.m_tPerfLimit.m_wMaxConnMtNum = wLicenseValue;
	return TRUE;
}

/*=============================================================================
  �� �� ���� GetLicenseNum
  ��    �ܣ� ȡLicense
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� u16 
=============================================================================*/
u16 CMcuVcData::GetLicenseNum( void )
{
	return GetPerfLimit().m_wMaxConnMtNum;
}

/*=============================================================================
  �� �� ���� SetVCSAccessNum
  ��    �ܣ� ����Licenseд���VCS������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wAccessNum
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::SetVCSAccessNum( u16 wAccessNum )
{
	m_tMcuDebugVal.m_tPerfLimit.m_wMaxVCSAccessNum = wAccessNum;
	return TRUE;
}

/*=============================================================================
  �� �� ���� GetVCSAccessNum
  ��    �ܣ� ȡLicenseд���VCS������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� u16 
=============================================================================*/
u16 CMcuVcData::GetVCSAccessNum( void )
{
	return GetPerfLimit().m_wMaxVCSAccessNum;
}

/*=============================================================================
�� �� ���� SetMcuExpireDate
��    �ܣ� ����mcu license ��ʱʹ������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TKdvTime &tExpireDate
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/10/20  4.0			������                  ����
=============================================================================*/
void CMcuVcData::SetMcuExpireDate( TKdvTime &tExpireDate )
{
    m_tMcuDebugVal.m_tExpireDate = tExpireDate;
}

/*=============================================================================
�� �� ���� GetMcuExpireDate
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� TKdvTime 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/10/20  4.0			������                  ����
=============================================================================*/
TKdvTime CMcuVcData::GetMcuExpireDate( void )
{
    return m_tMcuDebugVal.m_tExpireDate;
}

/*=============================================================================
�� �� ���� IsMcuExpiredDate
��    �ܣ� mcu license�Ƿ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/10/20  4.0			������                  ����
=============================================================================*/
BOOL32 CMcuVcData::IsMcuExpiredDate(void)
{
    if (m_tMcuDebugVal.m_tExpireDate.GetYear() == 0)
    {
        return FALSE;
    }

    time_t tExpireDate;
    m_tMcuDebugVal.m_tExpireDate.GetTime(tExpireDate);

    return (time(NULL) > tExpireDate);
}


/*=============================================================================
  �� �� ���� IsMtNumOverLicense
  ��    �ܣ� �Ƿ񳬹�����ն˽�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuVcData::IsMtNumOverLicense(void)
{
    u16 wMtNum = 0;
    for(u8 byIndex = 0; byIndex < MAXNUM_MCU_CONF; byIndex++)
    {
        CMcuVcInst *pcIns = m_apConfInst[byIndex];
        if(NULL != pcIns && pcIns->m_tConf.m_tStatus.IsOngoing())
        {
            wMtNum += pcIns->m_tConfAllMtInfo.GetLocalJoinedMtNum();
        }
    }

    return (wMtNum >= GetLicenseNum());
}

/*=============================================================================
  �� �� ���� IsVCSNumOverLicense
  ��    �ܣ� �Ƿ񳬹����VCS��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuVcData::IsVCSNumOverLicense(void)
{
    u16 wVCSAccessNum = 0;
	for (u16 wInstIdx = MAXNUM_MCU_MC + 1; wInstIdx <= (MAXNUM_MCU_VC + MAXNUM_MCU_MC); wInstIdx++)
	{
		if (g_cMcuVcApp.IsMcConnected(wInstIdx))
		{
			wVCSAccessNum++;
		}
	}
	
	CfgLog("[IsVCSNumOverLicense]current vcs access num:%d\n", wVCSAccessNum);

    return (wVCSAccessNum >= GetVCSAccessNum());
}

/*=============================================================================
  �� �� ���� IsConfNumOverCap
  ��    �ܣ� ���������Ƿ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� bOngoing               - �������Ļ����Ǽ�ʱ����
             byConfSource           - ָ������MCS_CONF��VCS_CONF��ALL_CONF
  �� �� ֵ�� BOOL32  
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/1/18     4.0			����                  ����
=============================================================================*/
BOOL32  CMcuVcData::IsConfNumOverCap ( BOOL32 bOngoing, u8 byConfSource /*= MCS_CONF*/)
{
	// ���MCU�����𷽼�ʱ��ԤԼ�����Ƿ񳬹���mcu��������
    u8 byAllConfNum = GetConfNum( TRUE, TRUE, FALSE, ALL_CONF );

    // ������������
    if (byAllConfNum >= MAXNUM_MCU_CONF )
    {
        return TRUE;    
    }

    if ( bOngoing )
    {
        // �����ʱ�������� 
		// �鿴ָ���ķ��𷽻����Ƿ񳬹�����������𷽵ļ�ʱ���������
        u8 byConfNum = GetConfNum( TRUE, FALSE, FALSE, byConfSource );

		if (VCS_CONF == byConfSource && byConfNum >= MAXNUM_ONGO_VCSCONF)
		{
			return TRUE;
		}
		else if (MCS_CONF == byConfSource && byConfNum >= (MAXNUM_ONGO_CONF - MAXNUM_ONGO_VCSCONF))
		{
			return TRUE;
		}
		else if (ALL_CONF == byConfSource && byConfNum >= MAXNUM_ONGO_CONF)
		{
			return TRUE;
		}
    }

    return FALSE;
}

/*=============================================================================
  �� �� ���� GetMcuCasConfNum
  ��    �ܣ� ��ȡ������������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32  
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/01/19    4.0			����                  ����
=============================================================================*/
u8  CMcuVcData::GetMcuCasConfNum ( )
{
    u8 byCasConfNum = 0;
    for(u8 byIndex = 0; byIndex < MAXNUM_MCU_CONF; byIndex++)
    {
        CMcuVcInst *pcIns = m_apConfInst[byIndex];
        if (NULL == pcIns)
            continue;
        if ( !pcIns->m_tConf.m_tStatus.IsOngoing() )
            continue;
        
        if ( pcIns->m_tConfAllMtInfo.GetCascadeMcuNum() > 0 )
        {
            byCasConfNum ++;
        }            
    }

    return byCasConfNum;
}

/*=============================================================================
  �� �� ���� IsCasConfOverCap
  ��    �ܣ� �������������Ƿ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32  
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/01/19    4.0			����                  ����
=============================================================================*/
BOOL32  CMcuVcData::IsCasConfOverCap ( )
{
    return ( GetMcuCasConfNum() >= MAXLIMIT_MCU_CASCONF );
}

/*=============================================================================
  �� �� ���� GetMtNumOnDri
  ��    �ܣ� ���ĳ��DRI MtAdp�����ߵ��ն˻���Mcu�����������Ƿ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� byDriId    [in]
             bOnilne    [in]    �Ƿ���Onlineֵ
             byMtNum    [out]
             bySMcuNum  [out]
  �� �� ֵ�� BOOL32  �Ƿ���(�Ҳ����������ߵȶ���Ϊ����)
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/1/18     4.0			����                  ����
=============================================================================*/
BOOL32  CMcuVcData::GetMtNumOnDri( u8 byDriId, BOOL32 bOnline, u8 &byMtNum, u8 &byMcuNum )
{
    if ( byDriId == 0 || byDriId > MAXNUM_DRI )
        return TRUE;

    u8 byIdx = byDriId - 1;
    if ( !m_atMtAdpData[byIdx].m_bConnected || 
         PROTOCOL_TYPE_H323 != m_atMtAdpData[byIdx].m_byProtocolType ) 
    {
         return TRUE;
    }

    
    if (!bOnline)
    {
        // Mt������ͳ��ֵ
        byMtNum = (u8)m_atMtAdpData[byIdx].m_wMtNum;
    }    
    else
    {
        byMtNum = 0;
    }

    // ��������SMcu����
    byMcuNum = 0;
    for(u8 byIndex = 0; byIndex < MAXNUM_MCU_CONF; byIndex++)
    {
        CMcuVcInst *pcIns = m_apConfInst[byIndex];
        if (NULL == pcIns)
            continue;
        if ( !pcIns->m_tConf.m_tStatus.IsOngoing() )
            continue;

        if ( !pcIns->m_tConf.GetConfAttrb().IsSupportCascade() )
            continue;

        for ( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
        {
            if ( pcIns->m_ptMtTable->GetDriId(byMtId) == byDriId )
            {
                if ( bOnline && pcIns->m_tConfAllMtInfo.MtJoinedConf( byMtId ) )
                {
                    byMtNum ++;
                }
                if ( pcIns->m_tConfAllMtInfo.MtJoinedConf( byMtId ) &&
                     ( pcIns->m_ptMtTable->GetMtType( byMtId ) == MT_TYPE_SMCU ) )
                {
                    // MCU ��Զ���online
                    byMcuNum ++;
                }
            }
        }
    }

    if ( byMtNum >= m_atMtAdpData[byIdx].m_byMaxMtNum || 
         byMcuNum >= m_atMtAdpData[byIdx].m_byMaxSMcuNum )
    {
         return TRUE;
    }

    return FALSE;
    
}


/*=============================================================================
  �� �� ���� IsSavingBandwidth
  ��    �ܣ� �Ƿ��ʡ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::IsSavingBandwidth(void)         
{
    TLocalInfo tLocalInfo;
    if(SUCCESS_AGENT != g_cMcuAgent.GetLocalInfo(&tLocalInfo))
    {
        return FALSE;
    }

    return (1 == tLocalInfo.GetIsSaveBand());
}

/*=============================================================================
  �� �� ���� Msg2TemInfo
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CServMsg &cMsg
             TTemplateInfo &tTemInfo
  �� �� ֵ�� void  
=============================================================================*/
void CMcuVcData::Msg2TemInfo(CServMsg &cMsg, TTemplateInfo &tTemInfo, 
					    	 s8* *pszUnProcInfoHead /*= NULL*/, u16* pwUnProcLen /*= NULL*/)
{
    // guzh [5/17/2007] �����
    tTemInfo.Clear();

	TConfInfo *ptConfInfo = (TConfInfo *)cMsg.GetMsgBody();
	u16 wAliasBufLen = ntohs( *(u16*)(cMsg.GetMsgBody() + sizeof(TConfInfo)) );
	s8 *pszAliaseBuf = (s8*)(cMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16));
	s8 *pszModBuf = (s8*)(cMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16) + wAliasBufLen);

    u8 byMtNum = tTemInfo.m_byMtNum;
	UnPackTMtAliasArray(pszAliaseBuf, wAliasBufLen, ptConfInfo, tTemInfo.m_atMtAlias,
		                tTemInfo.m_awMtDialBitRate, byMtNum);
    tTemInfo.m_byMtNum = byMtNum;
	memcpy(&tTemInfo.m_tConfInfo, ptConfInfo, sizeof(TConfInfo));

	if (ptConfInfo->GetConfAttrb().IsHasTvWallModule())
	{
		TMultiTvWallModule *ptMultiTvWallModule = (TMultiTvWallModule *)pszModBuf;
		tTemInfo.m_tMultiTvWallModule = *ptMultiTvWallModule;
		pszModBuf += sizeof(TMultiTvWallModule);
	}
	if (ptConfInfo->GetConfAttrb().IsHasVmpModule())
	{
		memcpy(&tTemInfo.m_atVmpModule, pszModBuf, sizeof(TVmpModule));
		pszModBuf += sizeof(TVmpModule);
	}

	// ����VCS�����Ļ���ģ�壬��������Ҫ�Ķ�����Ϣ 
	//                  + 1byte(u8: 0 1  �Ƿ������˸������ǽ)
	//                  +(��ѡ, THDTvWall)
	//                  + 1byte(u8: ��������HDU��ͨ������)
	//                  + (��ѡ, THduModChnlInfo+...)	
	//                  + 1byte(�Ƿ�Ϊ��������)��(��ѡ��2byte[u16 ���������ô�����ܳ���]+�¼�mcu����[1byte(�ն�����)+1byte(��������)+xbyte(�����ַ���)+2byte(��������)...)])
	//                  + 1byte(�Ƿ�֧�ַ���)��(��ѡ��2byte(u16 ��������Ϣ�ܳ�)+����(1byte(����)+n��[1TVCSGroupInfo��m��TVCSEntryInfo])
	if (VCS_CONF == ptConfInfo->GetConfSource())
	{
		s8* pszVCSInfoEx = pszModBuf;
		// �����˸������ǽ
		if (*pszVCSInfoEx++)
		{
			memcpy(&tTemInfo.m_tHDTWInfo, pszVCSInfoEx, sizeof(THDTvWall));
			pszVCSInfoEx += sizeof(THDTvWall);			
		}
		else
		{
			tTemInfo.m_tHDTWInfo.SetNull();
		}

		// ����HDU
		tTemInfo.m_tHduModule.SetNull();
		u8 byChnlNum = *pszVCSInfoEx++;
		if (byChnlNum)
		{
			tTemInfo.m_tHduModule.SetHduModuleInfo(byChnlNum, pszVCSInfoEx);
			pszVCSInfoEx += byChnlNum * sizeof(THduModChnlInfo);
		}

		// �������¼�mcu
		if (*pszVCSInfoEx++)
		{
			u16 wSMCUInfoLen = ntohs(*(u16*)pszVCSInfoEx);
			pszVCSInfoEx = (s8*)(pszVCSInfoEx + sizeof(u16));
			UnPackTMtAliasArray(pszVCSInfoEx, wSMCUInfoLen, &tTemInfo.m_tConfInfo,
								tTemInfo.m_tVCSSMCUCfg.m_atSMCUAlias, 
								tTemInfo.m_tVCSSMCUCfg.m_awSMCUDialBitRate, 
								tTemInfo.m_tVCSSMCUCfg.m_wSMCUNum);
			pszVCSInfoEx = (s8*)(pszVCSInfoEx + wSMCUInfoLen);
		}
		else
		{
			tTemInfo.m_tVCSSMCUCfg.SetNull();
		}

		// �����˷���
		if (*pszVCSInfoEx++)
		{
			tTemInfo.m_byMTPackExist = TRUE;
			if (pszUnProcInfoHead != NULL)
			{
				*pszUnProcInfoHead = pszVCSInfoEx;
				*pwUnProcLen = ntohs(*(u16*)pszVCSInfoEx) + sizeof(u16);
			}
	 

		}	
	}

	return;
}

/*=============================================================================
  �� �� ���� TemInfo2Msg
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TTemplateInfo &tTemInfo
             CServMsg &cMsg
  �� �� ֵ�� void  
=============================================================================*/
void CMcuVcData::TemInfo2Msg(TTemplateInfo &tTemInfo, CServMsg &cMsg)
{
	s8  achAliasBuf[SERV_MSG_LEN];
	u16 wAliasBufLen = 0;

	cMsg.SetMsgBody( (u8*)&tTemInfo.m_tConfInfo, sizeof(TConfInfo) );

	//�ն˱��������� 
	PackTMtAliasArray(tTemInfo.m_atMtAlias, tTemInfo.m_awMtDialBitRate, 
		              tTemInfo.m_byMtNum, achAliasBuf, wAliasBufLen);
	wAliasBufLen = htons(wAliasBufLen);
	cMsg.CatMsgBody((u8*)&wAliasBufLen, sizeof(wAliasBufLen));
	cMsg.CatMsgBody((u8*)achAliasBuf, ntohs(wAliasBufLen));

	//����ǽģ��
	if( tTemInfo.m_tConfInfo.GetConfAttrb().IsHasTvWallModule() )
	{			
		cMsg.CatMsgBody( (u8*)&tTemInfo.m_tMultiTvWallModule, sizeof(TMultiTvWallModule) );
	}
	//����ϳ�ģ�� 
	if( tTemInfo.m_tConfInfo.GetConfAttrb().IsHasVmpModule() )
	{			
		cMsg.CatMsgBody( (u8*)&tTemInfo.m_atVmpModule, sizeof(TVmpModule) );
	}
	// ����VCS�����Ļ���ģ�壬��������Ҫ�Ķ�����Ϣ 
	//                  + 1byte(u8: 0 1  �Ƿ������˸������ǽ)
	//                  +(��ѡ, THDTvWall)
	//                  + 1byte(u8: ��������HDU��ͨ������)
	//                  + (��ѡ, THduModChnlInfo+...)
	//                  + 1byte(�Ƿ�Ϊ��������)��(��ѡ��2byte[u16 ���������ô�����ܳ���]+�¼�mcu����[1byte(�ն�����)+1byte(��������)+xbyte(�����ַ���)+2byte(��������)...)])
	//                  + 1byte(�Ƿ�֧�ַ���)��(��ѡ��2byte(u16 ��������Ϣ�ܳ�)+����(1byte(����)+n��[1TVCSGroupInfo��m��TVCSEntryInfo])
	if (VCS_CONF == tTemInfo.m_tConfInfo.GetConfSource())
	{
		u8 byIsHDTWCfg = (u8)tTemInfo.IsHDTWCfg();
		cMsg.CatMsgBody(&byIsHDTWCfg, sizeof(u8));
		// �����˸������ǽ
		if (byIsHDTWCfg)
		{
			cMsg.CatMsgBody((u8*)&tTemInfo.m_tHDTWInfo, sizeof(THDTvWall));
		}
		// ������Hdu
		u8 byHduChnlNum = tTemInfo.m_tHduModule.GetHduChnlNum();
		cMsg.CatMsgBody(&byHduChnlNum, sizeof(u8));
		if (byHduChnlNum)
		{
			cMsg.CatMsgBody(tTemInfo.m_tHduModule.GetHduModuleInfo(), byHduChnlNum * sizeof(THduModChnlInfo));
		}

		// �������¼�mcu
		u8 byIsVCSSMCUCfg = (u8)tTemInfo.IsVCSMCUCfg();
		cMsg.CatMsgBody(&byIsVCSSMCUCfg, sizeof(u8));
		if (byIsVCSSMCUCfg)
		{
			PackTMtAliasArray(tTemInfo.m_tVCSSMCUCfg.m_atSMCUAlias, tTemInfo.m_tVCSSMCUCfg.m_awSMCUDialBitRate,
							  tTemInfo.m_tVCSSMCUCfg.m_wSMCUNum, achAliasBuf, wAliasBufLen);
			wAliasBufLen = htons(wAliasBufLen);
			cMsg.CatMsgBody((u8*)&wAliasBufLen, sizeof(u16));
			cMsg.CatMsgBody((u8*)achAliasBuf, ntohs(wAliasBufLen));
		}
		// �����˷���		
		if (tTemInfo.m_byMTPackExist)
		{
			u8 achMTPackInfo[SERV_MSG_LEN - SERV_MSGHEAD_LEN];  //��ŷ�����Ϣ�Ļ���
			memset(achMTPackInfo, 0, sizeof(achMTPackInfo));
			
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
				OspPrintf(TRUE, FALSE, "[TemInfo2Msg] confinfo_head.dat has no temp with specified confid\n");
			}
			else
			{
				if (GetUnProConfDataToFile(byConfPos, (s8*)achMTPackInfo, wAliasBufLen))
				{
					// �ɹ���ȡ��Ӧ����
					cMsg.CatMsgBody((u8*)&tTemInfo.m_byMTPackExist, sizeof(u8));
					cMsg.CatMsgBody(achMTPackInfo, wAliasBufLen);
				}
				else
				{
					// �ļ����ܱ�ɾ��
					tTemInfo.m_byMTPackExist =  FALSE;
					cMsg.CatMsgBody((u8*)&tTemInfo.m_byMTPackExist, sizeof(u8));
				}

			}
		}
		else
		{
			cMsg.CatMsgBody((u8*)&tTemInfo.m_byMTPackExist, sizeof(u8));

		}
			
	}

	cMsg.SetConfId(tTemInfo.m_tConfInfo.GetConfId());

	return;
}

/*=============================================================================
  �� �� ���� ConfInfoMsgPack
  ��    �ܣ� ������Ϣ��Ϣ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMcuVcInst *pcSchInst
             CServMsg &cServMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcData::ConfInfoMsgPack(CMcuVcInst *pcSchInst, CServMsg &cServMsg)
{
    if(NULL == pcSchInst)
    {
        return;
    }

    u8  byLoop;
    TMt tMt;
    TMtAlias  tMtAlias;
    TConfInfo tNewConfInfo;
    TMtAlias  atMtAlias[MAXNUM_CONF_MT];
    u16       awMtDialBitRate[MAXNUM_CONF_MT];
    
    s8  achAliasBuf[SERV_MSG_LEN];
    u16 wAliasBufLen = 0;
    u8  byMtPos = 0;
    
    TMultiTvWallModule tMultiTvWallModule;
    pcSchInst->m_tConfEqpModule.GetMultiTvWallModule(tMultiTvWallModule);
    TVmpModule tVmpModule = pcSchInst->m_tConfEqpModule.GetVmpModule();   

    memset(tVmpModule.m_abyVmpMember, 0, sizeof(tVmpModule.m_abyVmpMember));
    
    //���GKע����Ϣ����ʱģ�屾��ע��ɹ�
    tNewConfInfo = pcSchInst->m_tConf;
    tNewConfInfo.m_tStatus.SetRegToGK( FALSE );
    if( tNewConfInfo.m_tStatus.GetProtectMode() == CONF_LOCKMODE_LOCK )
    {
        tNewConfInfo.m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );
    }    
    
    //�ն��б�
    BOOL32 bExist = FALSE;
    u8 byMemberType = 0;
    for( byLoop = 0; byLoop < pcSchInst->m_ptMtTable->m_byMaxNumInUse; byLoop++)
    {
        tMt = pcSchInst->m_ptMtTable->GetMt(byLoop+1);
        if(!tMt.IsNull())
        {			
            //����mtAliasTypeH320Alias���Ͳ�������������Ϣ�����Բ�����
            if( pcSchInst->m_ptMtTable->GetMtAlias( (byLoop+1), mtAliasTypeH320ID, &tMtAlias ) )
            {
                atMtAlias[byLoop] = tMtAlias;
            }
            else if( pcSchInst->m_ptMtTable->GetMtAlias( (byLoop+1), mtAliasTypeTransportAddress, &tMtAlias ) )
            {
                atMtAlias[byLoop] = tMtAlias;			
            }
            else 
            {
                if( pcSchInst->m_ptMtTable->GetMtAlias( (byLoop+1), mtAliasTypeE164, &tMtAlias ) )
                {
                    atMtAlias[byLoop] = tMtAlias;			
                } 
                else 
                {
                    if( pcSchInst->m_ptMtTable->GetMtAlias( (byLoop+1), mtAliasTypeH323ID, &tMtAlias ) )
                    {
                        atMtAlias[byLoop] = tMtAlias;
                    }
                }
            }
            
            awMtDialBitRate[byLoop] = pcSchInst->m_ptMtTable->GetDialBitrate( (byLoop+1) );
            
            //����ģ��ӳ���ϵ
            if(tNewConfInfo.GetConfAttrb().IsHasTvWallModule())
            {
                for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL; byTvLp++)
                {
                    u8 byTvId = pcSchInst->m_tConfEqpModule.m_tTvWallInfo[byTvLp].m_tTvWallEqp.GetEqpId();
                    bExist = pcSchInst->m_tConfEqpModule.GetTvWallMemberByMt(byTvId, tMt, byMtPos, byMemberType);
                    if( bExist )
                    {
                        tMultiTvWallModule.SetTvWallMember(byTvId, byMtPos, byLoop+1, byMemberType);
                    }
                }
            }

            if(tNewConfInfo.GetConfAttrb().IsHasVmpModule())
            {
                for (u8 byVmpIdx = 0; byVmpIdx < MAXNUM_MPUSVMP_MEMBER; byVmpIdx++)
                {
                    if( pcSchInst->m_tConfEqpModule.IsMtAtVmpChannel(byVmpIdx, tMt, byMemberType) )
					{
						tVmpModule.SetVmpMember(byVmpIdx, byLoop+1, byMemberType);
					}
                }                
            }            
        }
    }   
    
    //�ն˱��������� 
    PackTMtAliasArray(atMtAlias, awMtDialBitRate, 
		              pcSchInst->m_ptMtTable->m_byMaxNumInUse, achAliasBuf, wAliasBufLen);	
    wAliasBufLen = htons(wAliasBufLen);
	cServMsg.SetMsgBody((u8 *)&tNewConfInfo, sizeof(TConfInfo));
    cServMsg.CatMsgBody((u8*)&wAliasBufLen, sizeof(wAliasBufLen));
    cServMsg.CatMsgBody((u8*)achAliasBuf, ntohs(wAliasBufLen));
    
    //����ǽģ��
    if( tNewConfInfo.GetConfAttrb().IsHasTvWallModule() )
    {			
        cServMsg.CatMsgBody( (u8*)&tMultiTvWallModule, sizeof(TMultiTvWallModule) );
    }
    //����ϳ�ģ�� 
    if( tNewConfInfo.GetConfAttrb().IsHasVmpModule() )
    {			
        cServMsg.CatMsgBody( (u8*)&tVmpModule, sizeof(TVmpModule) );
    }

    return;
}

/*=============================================================================
  �� �� ���� ShowTemplate
  ��    �ܣ� ��ӡģ�������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void  
=============================================================================*/
void   CMcuVcData::ShowTemplate(void)
{
    s8 szInfo[255];
    OspPrintf(TRUE, FALSE, "\nconf template info as followed:\n");
    
    for(u8 byIndex = 0; byIndex < MAXNUM_MCU_TEMPLATE; byIndex++)
    {
        if(m_ptTemplateInfo[byIndex].IsEmpty())
        {
            continue;
        }

        TConfInfo *ptConfInfo = &m_ptTemplateInfo[byIndex].m_tConfInfo;
        ptConfInfo->GetConfId().GetConfIdString(szInfo, sizeof(szInfo));
        OspPrintf(TRUE, FALSE, "\nTemplate name: %s, E164: %s\n", 
                  ptConfInfo->GetConfName(), ptConfInfo->GetConfE164());
        OspPrintf(TRUE, FALSE, "temindex: %d, insid: %d confidx: %d, usrgrp: %d\n  confid: %s\n", 
                  byIndex, GetConfMapInsId(m_ptTemplateInfo[byIndex].m_byConfIdx), m_ptTemplateInfo[byIndex].m_byConfIdx, 
                  ptConfInfo->GetUsrGrpId(), szInfo);
        
        OspPrintf(TRUE, FALSE, "MTs(number %d) in template:\n", m_ptTemplateInfo[byIndex].m_byMtNum);
        for(u8 byMtIndex = 0; byMtIndex < MAXNUM_CONF_MT; byMtIndex++)
        {
            if(!m_ptTemplateInfo[byIndex].m_atMtAlias[byMtIndex].IsNull())
            {
                OspPrintf(TRUE, FALSE, "Mt%d :", byMtIndex);
                m_ptTemplateInfo[byIndex].m_atMtAlias[byMtIndex].Print();
            }
        }
        
        if(ptConfInfo->GetConfAttrb().IsHasTvWallModule())
        {
            for(u8 byTvLp = 0; byTvLp < m_ptTemplateInfo->m_tMultiTvWallModule.m_byTvModuleNum; byTvLp++)
            {
                TTvWallModule *ptTW = &m_ptTemplateInfo[byIndex].m_tMultiTvWallModule.m_atTvWallModule[byTvLp];
                OspPrintf(TRUE, FALSE, "TvWall Module <%d, %d>:\n", ptTW->m_tTvWall.GetMcuId(), ptTW->m_tTvWall.GetMtId());
                memset(szInfo, 0, sizeof(szInfo));
                for(u8 byTWIndex = 0; byTWIndex < MAXNUM_PERIEQP_CHNNL; byTWIndex++)
                {
                    sprintf(szInfo, "%s:%d", szInfo, ptTW->m_abyTvWallMember[byTWIndex]);
                }
                OspPrintf(TRUE, FALSE, "TvWall mt index %s", szInfo);
            }
        }

        if(ptConfInfo->GetConfAttrb().IsHasVmpModule())
        {
            TVmpModule *ptVmp = &m_ptTemplateInfo[byIndex].m_atVmpModule;
            OspPrintf(TRUE, FALSE, "Vmp Module:\n");
            memset(szInfo, 0, sizeof(szInfo));
            for(u8 byVmpIndex = 0; byVmpIndex < MAXNUM_MPUSVMP_MEMBER; byVmpIndex++)
            {
                sprintf(szInfo, "%s:%d", szInfo, ptVmp->m_abyVmpMember[byVmpIndex]);
            }
            OspPrintf(TRUE, FALSE, "Vmp mt index %s", szInfo);
        }
    }

    return;
}

/*=============================================================================
  �� �� ���� ShowConfMap
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void  
=============================================================================*/
void   CMcuVcData::ShowConfMap(void)
{
    OspPrintf(TRUE, FALSE, "\n---------VC Map Data---------\n");

    s8 szConfId[64] = {0};
    TConfInfo *ptConf;
    for(u8 byIndex = 0; byIndex < MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE; byIndex++)
    {
        if ( !m_atConfMapData[byIndex].IsTemUsed() && 
             !m_atConfMapData[byIndex].IsValidConf() )
             continue;

        OspPrintf(TRUE, FALSE, "Map Index: %3d\tTemplateIdx: %2d\tVCInstID: %2d\n", 
                  byIndex+1,
                  m_atConfMapData[byIndex].GetTemIndex(), 
                  m_atConfMapData[byIndex].GetInsId() );

        if ( m_atConfMapData[byIndex].IsTemUsed() )
        {
            u8 byTemIdx = m_atConfMapData[byIndex].GetTemIndex();
            m_ptTemplateInfo[byTemIdx].m_tConfInfo.GetConfId().GetConfIdString(szConfId, 63);
            OspPrintf(TRUE, FALSE, "\tTmpt Name: %s\tConfID:%s\n", 
                      m_ptTemplateInfo[byTemIdx].m_tConfInfo.GetConfName(),
                      szConfId);
        }

        if ( m_atConfMapData[byIndex].IsValidConf() )
        {
            u8 byInsId =  m_atConfMapData[byIndex].GetInsId()-MIN_CONFIDX;
            ptConf = &m_apConfInst[byInsId]->m_tConf;
            ptConf->GetConfId().GetConfIdString(szConfId, 63);
            OspPrintf(TRUE, FALSE, "\tConf Name: %s\tConfID:%s\n", 
                      ptConf->GetConfName(),
                      szConfId);
        }
    }
}


/*=============================================================================
    �� �� ���� GetVcDeamonEnvState
    ��    �ܣ� �������ݵ���ʱ ��ȡ ͬ��ǰ��MTADP/MP/����/��� �Ȼ�����Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN/OUT]  TMSSynEnvState &tMSSynEnvState
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/12  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetVcDeamonEnvState( TMSSynEnvState &tMSSynEnvState )
{
	u8 byLoop = 0;
	memset(&tMSSynEnvState, 0, sizeof(tMSSynEnvState));

	//���ʵ������״̬
	for (byLoop = 0; byLoop < (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byLoop++)
	{
		if (m_atMcTable[byLoop].m_bConnected)
		{
			tMSSynEnvState.m_tMCState.m_abyOnline[byLoop] = 1;
			memcpy((s8*)&tMSSynEnvState.m_tMCState.m_tMcsRegInfo[byLoop], 
				   (s8*)&m_atMcTable[byLoop].m_tMcsRegInfo, sizeof(TMcsRegInfo));
		}
	}
	for (byLoop = 0; byLoop < MAXNUM_MCU_PERIEQP; byLoop++)
	{	
		if (m_atPeriEqpTable[byLoop].m_tPeriEqpStatus.m_byOnline)
		{
			tMSSynEnvState.m_tEqpState.m_abyOnline[byLoop] = 1;
		}
	}
	//Mpʵ������״̬
	for (byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if (m_atMpData[byLoop].m_bConnected)
		{
			tMSSynEnvState.m_tMpState.m_abyOnline[byLoop] = 1;
		}	
	}
	//MTADPʵ������״̬
	for (byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if (m_atMtAdpData[byLoop].m_bConnected)
		{
			tMSSynEnvState.m_tMtAdpState.m_abyOnline[byLoop] = 1;
		}	
	}
    //DCSʵ������״̬
	for (byLoop = 0; byLoop < MAXNUM_MCU_DCS; byLoop++)
	{
		if (m_atPeriDcsTable[byLoop].m_tDcsStatus.m_byOnline)
		{
			tMSSynEnvState.m_tDcsState.m_abyOnline[byLoop] = 1;
		}	
	}
	return TRUE;
}

/*=============================================================================
    �� �� ���� IsEqualToVcDeamonEnvState
    ��    �ܣ� �������ݵ���ʱ �Ƚ��Ƿ�һ�� ͬ��ǰ��MTADP/MP/����/��� �Ȼ�����Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN/OUT]  TMSSynEnvState *ptMSSynEnvState ���Ƚϵ��ⲿ������Ϣ               
	           [IN]      BOOL32 bPrintErr                  �Ƿ��ӡʧ����Ϣ
               [OUT]     TMSEnvConflict &tMSEnvConflict  ��ϸʧ����Ϣ
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                ����
    2006/06/01  4.0         �ű���                ����ʧ����Ϣ�ϱ���ش���
=============================================================================*/
BOOL32 CMcuVcData::IsEqualToVcDeamonEnvState( TMSSynEnvState *ptMSSynEnvState, BOOL32 bPrintErr, TMSSynState *ptMSSynState )
{
	if (NULL == ptMSSynEnvState)
	{
		return FALSE;
	}

	//MTADP/Mp/���� EqpID ��MCUָ������ӦInstIDΨһ�����Կɲ���InstIDΨһ��ʶ����������״̬У�飻
	//��� ��EqpID��IDֵָ������ӦInstIDҲ��Ψһ���ɻ�ص��ṩ�������ֵdwMcsSSRCΨһ��ʶ�����TMcsRegInfo��������״̬У�飻

	u8 byLoop  = 0;
	u8 byState = 0;

	//���ʵ������״̬
	//�����Ч������ У��
	u8 byLocalNum  = 0;  //���˻����Ч������
	u8 byRemoteNum = 0;  //Զ�˻����Ч������
	for (byLoop = 0; byLoop < (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byLoop++)
	{
		if (m_atMcTable[byLoop].m_bConnected)
		{			
			byLocalNum++; 
		}
		if (1 == ptMSSynEnvState->m_tMCState.m_abyOnline[byLoop])
		{			
			byRemoteNum++; 
		}
	}
	if (byLocalNum != byRemoteNum)
	{
        //���ó�ͻ����
        ptMSSynState->SetEntityType( TMSSynState::emMC );
        
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "Mcs Online_Num is not Equal: LocalNum.%d RemoteNum.%d\n", 
				      byLocalNum, byRemoteNum);
		}
		return FALSE;
	}

	//��Ч��ص����Ӳ��� У��
	if (byRemoteNum > 0)
	{
		for (byLoop = 0; byLoop < (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byLoop++)
		{
			if (1 != ptMSSynEnvState->m_tMCState.m_abyOnline[byLoop])
			{
				continue;
			}
			u8 byLocalPos = 0;
			for (byLocalPos = 0; byLocalPos < (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byLocalPos++)
			{
				if (m_atMcTable[byLocalPos].m_bConnected && 
					m_atMcTable[byLocalPos].m_tMcsRegInfo == ptMSSynEnvState->m_tMCState.m_tMcsRegInfo[byLoop])
				{
					break;
				}
			}
			if (MAXNUM_MCU_MC + MAXNUM_MCU_VC == byLocalPos)
			{
                //���ó�ͻ��Ϣ
                ptMSSynState->SetEntityType( TMSSynState::emMC );

				if (bPrintErr)
				{
					OspPrintf(TRUE, FALSE, "McsInstId.%d McsRegInfo no match with LocalMC's: RemoteMcsIp.0x%0x McsSSRC.0x%0x\n", 
						      (byLoop+1), ptMSSynEnvState->m_tMCState.m_tMcsRegInfo[byLoop].GetMcsIpAddr(), 
						      ptMSSynEnvState->m_tMCState.m_tMcsRegInfo[byLoop].GetMcsSSRC());
				}
				return FALSE;
			}
		}
	}

	//����ʵ������״̬
	for (byLoop = 0; byLoop < MAXNUM_MCU_PERIEQP; byLoop++)
	{
		if (m_atPeriEqpTable[byLoop].m_tPeriEqpStatus.m_byOnline != 
			ptMSSynEnvState->m_tEqpState.m_abyOnline[byLoop])
		{
            //���ó�ͻ��Ϣ
            ptMSSynState->SetEntityType( TMSSynState::emPeriEqp );
            ptMSSynState->SetEntityId( byLoop + 1 ); 
            
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "EqpId.%d Online_State is not Equal:Local.%d Remote.%d\n", 
					      (byLoop+1), m_atPeriEqpTable[byLoop].m_tPeriEqpStatus.m_byOnline, 
						  ptMSSynEnvState->m_tEqpState.m_abyOnline[byLoop]);
			}
			return FALSE;
		}
	}
	//Mpʵ������״̬
	for (byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		byState = m_atMpData[byLoop].m_bConnected ? 1 : 0;
		if (byState != ptMSSynEnvState->m_tMpState.m_abyOnline[byLoop])
		{
            //���ó�ͻ��Ϣ
            ptMSSynState->SetEntityType( TMSSynState::emMp );
            
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "MpId.%d Online_State is not Equal:Local.%d Remote.%d\n", 
					      (byLoop+1), byState, ptMSSynEnvState->m_tMpState.m_abyOnline[byLoop]);
			}
			return FALSE;
		}	
	}
	//MTADPʵ������״̬
	for (byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		byState = m_atMtAdpData[byLoop].m_bConnected ? 1 : 0;
		if (byState != ptMSSynEnvState->m_tMtAdpState.m_abyOnline[byLoop])
		{
            //���ó�ͻ��Ϣ
            ptMSSynState->SetEntityType( TMSSynState::emMtAdp );

			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "MtadpId.%d Online_State is not Equal:Local.%d Remote.%d\n", 
					      (byLoop+1), byState, ptMSSynEnvState->m_tMtAdpState.m_abyOnline[byLoop]);
			}
			return FALSE;
		}	
	}
	//DCSʵ������״̬
	for (byLoop = 0; byLoop < MAXNUM_MCU_DCS; byLoop++)
	{
		if (m_atPeriDcsTable[byLoop].m_tDcsStatus.m_byOnline != 
			ptMSSynEnvState->m_tDcsState.m_abyOnline[byLoop])
		{
            //���ó�ͻ��Ϣ
            ptMSSynState->SetEntityType( TMSSynState::emDcs );
            ptMSSynState->SetEntityId( byLoop + 1 );            

			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "DcsId.%d Online_State is not Equal:Local.%d Remote.%d\n", 
					      (byLoop+1), m_atPeriDcsTable[byLoop].m_tDcsStatus.m_byOnline, 
						  ptMSSynEnvState->m_tDcsState.m_abyOnline[byLoop]);
			}
			return FALSE;
		}
	}	
	return TRUE;
}

/*=============================================================================
    �� �� ���� GetVcDeamonPeriEqpData
    ��    �ܣ� �������ݵ���ʱ ��ȡ ����Ӧ�õĹ�����������������Ϣ��(������������) ��Ϣ
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
BOOL32 CMcuVcData::GetVcDeamonPeriEqpData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < (sizeof(TMSVcPeriEqpStateHead)+sizeof(m_atPeriEqpTable)))
	{
		return FALSE;
	}
	
	TMSVcPeriEqpStateHead *ptPeriEqpState = (TMSVcPeriEqpStateHead*)pbyBuf;
	u8 *pbyEqpDataBuf = pbyBuf+sizeof(TMSVcPeriEqpStateHead);
	u8  byPeriEqpNum  = 0;
	memset(pbyBuf, 0, sizeof(TMSVcPeriEqpState));
		
	for (u8 byLoop = 0; byLoop < MAXNUM_MCU_PERIEQP; byLoop++)
	{
		if (m_atPeriEqpTable[byLoop].m_bIsValid) //������������
		{
			ptPeriEqpState->m_abyValid[byLoop] = 1;
			memcpy(pbyEqpDataBuf, (s8*)(&m_atPeriEqpTable[byLoop]), sizeof(TPeriEqpData));
			pbyEqpDataBuf += sizeof(TPeriEqpData);
			
			byPeriEqpNum += 1;
		}
		else
		{
			ptPeriEqpState->m_abyValid[byLoop] = 0;
		}
	}
	
	dwOutBufLen = sizeof(TMSVcPeriEqpStateHead) + byPeriEqpNum*sizeof(TPeriEqpData);
	
	return TRUE;
}

/*=============================================================================
    �� �� ���� SetVcDeamonPeriEqpData
    ��    �ܣ� �������ݵ���ʱ �ָ� ����Ӧ�õĹ�����������������Ϣ��(������������) ��Ϣ
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
BOOL32 CMcuVcData::SetVcDeamonPeriEqpData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcPeriEqpStateHead))
	{
		return FALSE;
	}
	
	TMSVcPeriEqpStateHead *ptPeriEqpState = (TMSVcPeriEqpStateHead*)pbyBuf;
	u8 *pbyEqpDataBuf = pbyBuf+sizeof(TMSVcPeriEqpStateHead);
	
	//zbq[06/05/2008] ����״̬������������
	for (u8 byLoop = 0; byLoop < MAXNUM_MCU_PERIEQP; byLoop++)
	{
		if (1 == ptPeriEqpState->m_abyValid[byLoop])
		{
            //zbq[2006/10/11]���������״̬����������Ϊ�����������������е���
            u8 byOnline = m_atPeriEqpTable[byLoop].m_tPeriEqpStatus.m_byOnline;
			
			//zbq[2008/07/24]���������豸״̬�����(����������ʱ����������)
			u8 byMixerStatus = TMixerGrpStatus::IDLE;
			if (byLoop <= MIXERID_MAX)
			{
				byMixerStatus = m_atPeriEqpTable[byLoop].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[0].m_byGrpState;
			}
			
			memcpy((s8*)(&m_atPeriEqpTable[byLoop]), pbyEqpDataBuf, sizeof(TPeriEqpData));
			
            m_atPeriEqpTable[byLoop].m_tPeriEqpStatus.m_byOnline = byOnline;
			if (byLoop <= MIXERID_MAX &&
				byMixerStatus == TMixerGrpStatus::WAIT_BEGIN &&
				byMixerStatus == TMixerGrpStatus::WAIT_START_SPECMIX &&
				byMixerStatus == TMixerGrpStatus::WAIT_START_AUTOMIX &&
				byMixerStatus == TMixerGrpStatus::WAIT_START_VAC &&
				byMixerStatus == TMixerGrpStatus::WAIT_STOP )
			{
				m_atPeriEqpTable[byLoop].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[0].m_byGrpState = byMixerStatus;
			}
			
			pbyEqpDataBuf += sizeof(TPeriEqpData);	
		}
		else
		{
			memset((s8*)(&m_atPeriEqpTable[byLoop]), 0, sizeof(TPeriEqpData));
		}
	}

	return TRUE;
}

/*=============================================================================
    �� �� ���� GetVcDeamonMCData
    ��    �ܣ� �������ݵ���ʱ ��ȡ ����Ӧ�õĹ����������Ļ����Ϣ�� ��Ϣ
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
BOOL32 CMcuVcData::GetVcDeamonMCData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < (sizeof(TMSVcMCStateHead)+sizeof(m_atMcTable)))
	{
		return FALSE;
	}
	
	TMSVcMCStateHead *ptMCStateHead = (TMSVcMCStateHead*)pbyBuf;
	u8 *pbyMCDataBuf = pbyBuf+sizeof(TMSVcMCStateHead);
	u8  byMCNum = 0;
	memset(pbyBuf, 0, sizeof(TMSVcMCStateHead));
		
	for (u8 byLoop = 0; byLoop < (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byLoop++)
	{
		if (m_atMcTable[byLoop].m_bConnected)
		{
			ptMCStateHead->m_abyValid[byLoop] = 1;
			memcpy(pbyMCDataBuf, (s8*)(&m_atMcTable[byLoop]), sizeof(TMcData));
			pbyMCDataBuf += sizeof(TMcData);
			
			byMCNum += 1;
		}
		else
		{
			ptMCStateHead->m_abyValid[byLoop] = 0;
		}
	}
	
	dwOutBufLen = sizeof(TMSVcMCStateHead) + byMCNum*sizeof(TMcData);
	
	return TRUE;
}

/*=============================================================================
    �� �� ���� SetVcDeamonMCData
    ��    �ܣ� �������ݵ���ʱ �ָ� ����Ӧ�õĹ����������Ļ����Ϣ�� ��Ϣ
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
BOOL32 CMcuVcData::SetVcDeamonMCData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcMCStateHead))
	{
		return FALSE;
	}
	
	BOOL32 bRet = TRUE;
	TMSVcMCStateHead *ptMCStateHead = (TMSVcMCStateHead*)pbyBuf;
	TMcData *ptMCData = (TMcData *)(pbyBuf+sizeof(TMSVcMCStateHead));

	//MTADP/Mp/���� EqpID ��MCUָ������ӦInstIDΨһ�����Կɲ���InstIDΨһ��ʶ����������״̬У�飻
	//��� ��EqpID��IDֵָ������ӦInstIDҲ��Ψһ���ɻ�ص��ṩ�������ֵdwMcsSSRCΨһ��ʶ�����TMcsRegInfo��������״̬У�飻
	
	for (u8 byLoop = 0; byLoop < MAXNUM_MCU_MC + MAXNUM_MCU_VC; byLoop++)
	{
		if (1 != ptMCStateHead->m_abyValid[byLoop])
		{
			continue;
		}
		u8 byLocalPos = 0;
		for (byLocalPos = 0; byLocalPos < (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byLocalPos++)
		{
			if (m_atMcTable[byLocalPos].m_bConnected && ptMCData->m_bConnected && 
				m_atMcTable[byLocalPos].m_tMcsRegInfo == ptMCData->m_tMcsRegInfo)
			{
				break;
			}
		}
		if ((MAXNUM_MCU_MC + MAXNUM_MCU_VC) == byLocalPos)
		{
			OspPrintf(TRUE, FALSE, "McsInstId.%d McsRegInfo no match with LocalMC's: Connected.%d RemoteMcsIp.0x%0x McsSSRC.0x%0x\n", 
					  (byLoop+1), ptMCData->m_bConnected, 
					  ptMCData->m_tMcsRegInfo.GetMcsIpAddr(), ptMCData->m_tMcsRegInfo.GetMcsSSRC());
			bRet = FALSE;
		}
		else
		{
            //MC������״̬����������Ϊ�����������������е��� 2006-10-11
            BOOL32 bConnected = m_atMcTable[byLocalPos].m_bConnected;
			memcpy((s8*)(&m_atMcTable[byLocalPos]), (s8*)ptMCData, sizeof(TMcData));
            m_atMcTable[byLocalPos].m_bConnected = bConnected;
		}
		ptMCData += 1;
	}
	
	return bRet;
}

/*=============================================================================
    �� �� ���� GetVcDeamonMpData
    ��    �ܣ� �������ݵ���ʱ ��ȡ ����Ӧ�õĹ�����������MP��Ϣ�� ��Ϣ
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
BOOL32 CMcuVcData::GetVcDeamonMpData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < (sizeof(TMSVcMpState)+sizeof(m_atMpData)))
	{
		return FALSE;
	}
	
	TMSVcMpState *ptMpState = (TMSVcMpState*)pbyBuf;
	u8 *pbyMpDataBuf = pbyBuf+sizeof(TMSVcMpState);
	u8  byMpNum = 0;
	memset(pbyBuf, 0, sizeof(TMSVcMpState));
		
	for (u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if (m_atMpData[byLoop].m_bConnected)
		{
			ptMpState->m_abyOnline[byLoop] = 1;
			memcpy(pbyMpDataBuf, (s8*)(&m_atMpData[byLoop]), sizeof(TMpData));
			pbyMpDataBuf += sizeof(TMpData);
			
			byMpNum += 1;
		}
		else
		{
			ptMpState->m_abyOnline[byLoop] = 0;
		}
	}
	
	dwOutBufLen = sizeof(TMSVcMpState) + byMpNum*sizeof(TMpData);
	
	return TRUE;
}

/*=============================================================================
    �� �� ���� SetVcDeamonMpData
    ��    �ܣ� �������ݵ���ʱ �ָ� ����Ӧ�õĹ�����������MP��Ϣ�� ��Ϣ
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
BOOL32 CMcuVcData::SetVcDeamonMpData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcMpState))
	{
		return FALSE;
	}
	
	TMSVcMpState *ptMpState = (TMSVcMpState*)pbyBuf;
	u8 *pbyMpDataBuf = pbyBuf+sizeof(TMSVcMpState);
	
	//zbq[06/05/2008] ����״̬������������
	for (u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if (1 == ptMpState->m_abyOnline[byLoop])
		{ 
            //MP������״̬����������Ϊ�����������������е��� 2006-10-11
            BOOL32 bConnected = m_atMpData[byLoop].m_bConnected;
			memcpy((s8*)(&m_atMpData[byLoop]), pbyMpDataBuf, sizeof(TMpData)); 
            m_atMpData[byLoop].m_bConnected = bConnected;

			pbyMpDataBuf += sizeof(TMpData);	
		}
		else
		{
			memset((s8*)(&m_atMpData[byLoop]), 0, sizeof(TMpData));
		}
	}

	return TRUE;
}

/*=============================================================================
    �� �� ���� GetVcDeamonMtadpData
    ��    �ܣ� �������ݵ���ʱ ��ȡ ����Ӧ�õĹ�����������MTADP��Ϣ�� ��Ϣ
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
BOOL32 CMcuVcData::GetVcDeamonMtadpData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < (sizeof(TMSVcMtAdpState)+sizeof(m_atMtAdpData)))
	{
		return FALSE;
	}
	
	TMSVcMtAdpState *ptMtadpState = (TMSVcMtAdpState*)pbyBuf;
	u8 *pbyMtadpDataBuf = pbyBuf+sizeof(TMSVcMtAdpState);
	u8  byMtadpNum = 0;
	memset(pbyBuf, 0, sizeof(TMSVcMtAdpState));
		
	for (u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if (m_atMtAdpData[byLoop].m_bConnected)
		{
			ptMtadpState->m_abyOnline[byLoop] = 1;
			memcpy(pbyMtadpDataBuf, (s8*)(&m_atMtAdpData[byLoop]), sizeof(TMtAdpData));
			pbyMtadpDataBuf += sizeof(TMtAdpData);
			
			byMtadpNum += 1;
		}
		else
		{
			ptMtadpState->m_abyOnline[byLoop] = 0;
		}
	}
	
	dwOutBufLen = sizeof(TMSVcMtAdpState) + byMtadpNum*sizeof(TMtAdpData);
	
	return TRUE;
}

/*=============================================================================
    �� �� ���� SetVcDeamonMtadpData
    ��    �ܣ� �������ݵ���ʱ �ָ� ����Ӧ�õĹ�����������MTADP��Ϣ�� ��Ϣ
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
BOOL32 CMcuVcData::SetVcDeamonMtadpData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcMtAdpState))
	{
		return FALSE;
	}
	
	TMSVcMtAdpState *ptMtadpState = (TMSVcMtAdpState*)pbyBuf;
	u8 *pbyMtadpDataBuf = pbyBuf+sizeof(TMSVcMtAdpState);
	
	//zbq[06/05/2008] ����״̬������������
	for (u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if (1 == ptMtadpState->m_abyOnline[byLoop])
		{ 
            //MtAdp������״̬����������Ϊ�����������������е��� 2006-10-11
            BOOL32 bConnected = m_atMtAdpData[byLoop].m_bConnected;
			memcpy((s8*)(&m_atMtAdpData[byLoop]), pbyMtadpDataBuf, sizeof(TMtAdpData));
            m_atMtAdpData[byLoop].m_bConnected = bConnected;

			pbyMtadpDataBuf += sizeof(TMtAdpData);	
		}
		else
		{
			memset((s8*)(&m_atMtAdpData[byLoop]), 0, sizeof(TMtAdpData));
		}
	}

	return TRUE;
}

/*=============================================================================
    �� �� ���� GetVcDeamonTemplateData
    ��    �ܣ� �������ݵ���ʱ ��ȡ ����Ӧ�õĹ�����������ģ����Ϣ�� ��Ϣ
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
BOOL32 CMcuVcData::GetVcDeamonTemplateData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < (sizeof(TMSVcTemplateState)+MAXNUM_MCU_TEMPLATE*sizeof(TTemplateInfo)))
	{
		return FALSE;
	}
	
	TMSVcTemplateState *ptTemplateState = (TMSVcTemplateState*)pbyBuf;
	u8 *pbyTemplateDataBuf = pbyBuf+sizeof(TMSVcTemplateState);
	u8  byTemplateNum = 0;
	memset(pbyBuf, 0, sizeof(TMSVcTemplateState));
	
	for (u8 byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
	{
		if (m_ptTemplateInfo[byLoop].IsEmpty())
		{
			ptTemplateState->m_abyValid[byLoop] = 0;
		}
		else
		{
			ptTemplateState->m_abyValid[byLoop] = 1;
			memcpy(pbyTemplateDataBuf, (s8*)(&m_ptTemplateInfo[byLoop]), sizeof(TTemplateInfo));
			pbyTemplateDataBuf += sizeof(TTemplateInfo);
			
			byTemplateNum += 1;
		}
	}
	
	dwOutBufLen = sizeof(TMSVcTemplateState) + byTemplateNum*sizeof(TTemplateInfo);
	
	return TRUE;
}

/*=============================================================================
    �� �� ���� SetVcDeamonTemplateData
    ��    �ܣ� �������ݵ���ʱ �ָ� ����Ӧ�õĹ�����������ģ����Ϣ�� ��Ϣ
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
BOOL32 CMcuVcData::SetVcDeamonTemplateData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcTemplateState))
	{
		return FALSE;
	}
	
	TMSVcTemplateState *ptTemplateState = (TMSVcTemplateState*)pbyBuf;
	u8 *ptTemplateData = pbyBuf+sizeof(TMSVcTemplateState);
	
	for (u8 byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
	{
		if (1 == ptTemplateState->m_abyValid[byLoop])
		{
			memcpy((s8*)(&m_ptTemplateInfo[byLoop]), ptTemplateData, sizeof(TTemplateInfo));
			ptTemplateData += sizeof(TTemplateInfo);	
		}
		else
		{
			m_ptTemplateInfo[byLoop].Clear();
		}
	}
	
	return TRUE;
}

/*=============================================================================
    �� �� ���� GetVcDeamonOtherData
    ��    �ܣ� �������ݵ���ʱ ��ȡ ����Ӧ�õĹ����������ĳ�EQP/MC/MP/MTADP��Ϣ���������ͬ�� ��Ϣ
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
BOOL32 CMcuVcData::GetVcDeamonOtherData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	dwOutBufLen  = 0;
	u32 dwOutLen = OprVcDeamonOtherData(pbyBuf, dwInBufLen, TRUE);
	if (0 == dwOutLen)
	{
		return FALSE;
	}
	
	dwOutBufLen = dwOutLen;
	return TRUE;
}

/*=============================================================================
    �� �� ���� SetVcDeamonOtherData
    ��    �ܣ� �������ݵ���ʱ �ָ� ����Ӧ�õĹ����������ĳ�EQP/MC/MP/MTADP��Ϣ���������ͬ�� ��Ϣ
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
BOOL32 CMcuVcData::SetVcDeamonOtherData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (0 == OprVcDeamonOtherData(pbyBuf, dwInBufLen, FALSE))
	{
		return FALSE;
	}
	
	return TRUE;
}

/*=============================================================================
    �� �� ���� OprVcDataOtherData
    ��    �ܣ� �������ݵ���ʱ ͬ�� ����Ӧ�õĹ����������ĳ�EQP/MC/MP/MTADP��Ϣ���������ͬ�� ����
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
    2005/11/18  4.0			����                 ����
    2006/08/07  4.0         ����                 ����ͬ��License����
=============================================================================*/
u32 CMcuVcData::OprVcDeamonOtherData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bGet )
{
	u32 dwOtherDataLen = GetVcDeamonOtherDataLen();
	//�������---
	if (NULL == pbyBuf || dwInBufLen < dwOtherDataLen)
	{
		return 0;
	}

	if (bGet)
	{
		memcpy(pbyBuf, (s8*)&m_atRecvMtPort, sizeof(m_atRecvMtPort));
		pbyBuf += sizeof(m_atRecvMtPort);
//		memcpy(pbyBuf, (s8*)&m_atMulticastIp, sizeof(m_atMulticastIp));
//		pbyBuf += sizeof(m_atMulticastIp);
		memcpy(pbyBuf, (s8*)&m_atMulticastPort, sizeof(m_atMulticastPort));
		pbyBuf += sizeof(m_atMulticastPort);
		//����ͬ���������ֹ�����
		//memcpy(pbyBuf, (s8*)&m_abyConfStoreInfo, sizeof(m_abyConfStoreInfo));
		//pbyBuf += sizeof(m_abyConfStoreInfo);
		memcpy(pbyBuf, (s8*)&m_abyConfRegState, sizeof(m_abyConfRegState));
		pbyBuf += sizeof(m_abyConfRegState);
		memcpy(pbyBuf, (s8*)&m_byRegGKDriId, sizeof(m_byRegGKDriId));
		pbyBuf += sizeof(m_byRegGKDriId);
		memcpy(pbyBuf, (s8*)&m_tGKID, sizeof(m_tGKID));
		pbyBuf += sizeof(m_tGKID);
		memcpy(pbyBuf, (s8*)&m_tEPID, sizeof(m_tEPID));
		pbyBuf += sizeof(m_tEPID);
		memcpy(pbyBuf, (s8*)&m_dwMakeConfIdTimes, sizeof(m_dwMakeConfIdTimes));
		pbyBuf += sizeof(m_dwMakeConfIdTimes);
		memcpy(pbyBuf, (s8*)&m_tMcuDebugVal, sizeof(m_tMcuDebugVal));
		pbyBuf += sizeof(m_tMcuDebugVal);
		memcpy(pbyBuf, (s8*)&m_dwMtCallInterfaceNum, sizeof(m_dwMtCallInterfaceNum));
		pbyBuf += sizeof(m_dwMtCallInterfaceNum);
		memcpy(pbyBuf, (s8*)m_atMtCallInterface, m_dwMtCallInterfaceNum*sizeof(TMtCallInterface));
		pbyBuf += m_dwMtCallInterfaceNum*sizeof(TMtCallInterface);
		memcpy(pbyBuf, (s8*)m_atConfMapData, sizeof(m_atConfMapData));
		pbyBuf += sizeof(m_atConfMapData);
        memcpy(pbyBuf, (s8*)m_atPeriDcsTable, sizeof(m_atPeriDcsTable));
        pbyBuf += sizeof(m_atPeriDcsTable);
        memcpy(pbyBuf, (s8*)&m_byChargeRegOK, sizeof(m_byChargeRegOK));
        pbyBuf += sizeof(m_byChargeRegOK);
	}
	else
	{
		memcpy((s8*)&m_atRecvMtPort, pbyBuf, sizeof(m_atRecvMtPort));
		pbyBuf += sizeof(m_atRecvMtPort);
//		memcpy((s8*)&m_atMulticastIp, pbyBuf, sizeof(m_atMulticastIp));
//		pbyBuf += sizeof(m_atMulticastIp);
		memcpy((s8*)&m_atMulticastPort, pbyBuf, sizeof(m_atMulticastPort));
		pbyBuf += sizeof(m_atMulticastPort);
		//����ͬ���������ֹ�����
		//memcpy((s8*)&m_abyConfStoreInfo, pbyBuf, sizeof(m_abyConfStoreInfo));
		//pbyBuf += sizeof(m_abyConfStoreInfo);
		memcpy((s8*)&m_abyConfRegState, pbyBuf, sizeof(m_abyConfRegState));
		pbyBuf += sizeof(m_abyConfRegState);
		memcpy((s8*)&m_byRegGKDriId, pbyBuf, sizeof(m_byRegGKDriId));
		pbyBuf += sizeof(m_byRegGKDriId);
		memcpy((s8*)&m_tGKID, pbyBuf, sizeof(m_tGKID));
		pbyBuf += sizeof(m_tGKID);
		memcpy((s8*)&m_tEPID, pbyBuf, sizeof(m_tEPID));
		pbyBuf += sizeof(m_tEPID);
		memcpy((s8*)&m_dwMakeConfIdTimes, pbyBuf, sizeof(m_dwMakeConfIdTimes));
		pbyBuf += sizeof(m_dwMakeConfIdTimes);        

		memcpy((s8*)&m_tMcuDebugVal, pbyBuf, sizeof(m_tMcuDebugVal));
		pbyBuf += sizeof(m_tMcuDebugVal);
        
		memcpy((s8*)&m_dwMtCallInterfaceNum, pbyBuf, sizeof(m_dwMtCallInterfaceNum));
		pbyBuf += sizeof(m_dwMtCallInterfaceNum);
		memcpy((s8*)m_atMtCallInterface, pbyBuf, m_dwMtCallInterfaceNum*sizeof(TMtCallInterface));
		pbyBuf += m_dwMtCallInterfaceNum*sizeof(TMtCallInterface);
		memcpy((s8*)m_atConfMapData, pbyBuf, sizeof(m_atConfMapData));
		pbyBuf += sizeof(m_atConfMapData);

		//zbq[06/05/2008] ����״̬������������
		/*
        //DCS������״̬����������Ϊ�����������������е��� 2006-10-11
        u8 byIndex = 0;
        u8 abyDcsOnline[MAXNUM_MCU_DCS] = { 0 }; 
        for( ; byIndex < MAXNUM_MCU_DCS; byIndex ++ )
        {
            abyDcsOnline[byIndex] = m_atPeriDcsTable[byIndex].m_tDcsStatus.m_byOnline;
        }
        memcpy((s8*)&m_atPeriDcsTable, pbyBuf, sizeof(m_atPeriDcsTable));
        for( byIndex = 0; byIndex < MAXNUM_MCU_DCS; byIndex ++ )
        {
            m_atPeriDcsTable[byIndex].m_tDcsStatus.m_byOnline = abyDcsOnline[byIndex];
        }
		*/
        pbyBuf += sizeof(m_atPeriDcsTable);

        memcpy((s8*)&m_byChargeRegOK, pbyBuf, sizeof(m_byChargeRegOK));
        pbyBuf += sizeof(m_byChargeRegOK);
	}

	return dwOtherDataLen;
}

/*=============================================================================
    �� �� ���� GetVcDeamonOtherDataLen
    ��    �ܣ� �������ݵ���ʱ ��ȡ ����Ӧ�õĹ����������ĳ�EQP/MC/MP/MTADP��Ϣ���������ͬ�� �����ܳ���
	           ����ͬ��������MCUӦ�û���һ�£�ͬ�������ݽṹ�ݲ����ǽ���ѹջ���⣺Pack Ingore
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� u32  �����ܳ���
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                 ����
    2006/08/07  4.0         ����                 ����ͬ��License����
=============================================================================*/
u32 CMcuVcData::GetVcDeamonOtherDataLen( void )
{
	u32 dwOtherDataLen = 0;
	
	//12
	dwOtherDataLen += sizeof(m_atRecvMtPort);
//	dwOtherDataLen += sizeof(m_atMulticastIp);
	dwOtherDataLen += sizeof(m_atMulticastPort);
	//dwOtherDataLen += sizeof(m_abyConfStoreInfo);
	dwOtherDataLen += sizeof(m_abyConfRegState);
	dwOtherDataLen += sizeof(m_byRegGKDriId);
	dwOtherDataLen += sizeof(m_tGKID);
	dwOtherDataLen += sizeof(m_tEPID);
	dwOtherDataLen += sizeof(m_dwMakeConfIdTimes);
	dwOtherDataLen += sizeof(m_tMcuDebugVal);
	dwOtherDataLen += sizeof(m_dwMtCallInterfaceNum);
	dwOtherDataLen += m_dwMtCallInterfaceNum*sizeof(TMtCallInterface);
	dwOtherDataLen += sizeof(m_atConfMapData);
    dwOtherDataLen += sizeof(m_atPeriDcsTable);
    dwOtherDataLen += sizeof(m_byChargeRegOK);
	
	return dwOtherDataLen;
}

/*=============================================================================
    �� �� ���� HangupAllVcInstTimer
    ��    �ܣ� �������ݵ���ʱ ��ͣ ���л���ʵ�� ��ѭ��ԤԼ����Ȼ��鶨ʱ������STATE_IDLEʵ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::HangupAllVcInstTimer( void )
{
	BOOL32 bRet = TRUE;
	
	CMcuVcInst* pcVcInst = NULL;
	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_CONF; byInstID++)
	{
		pcVcInst = m_apConfInst[byInstID-1];
		if (NULL != pcVcInst)
		{
			if (FALSE == pcVcInst->HangupInstTimer())
			{
				bRet = FALSE;
			}
		}	
	}
	
	return bRet;
}

/*=============================================================================
    �� �� ���� ResumeAllVcInstTimer
    ��    �ܣ� �������ݵ���ʱ �ָ� ���л���ʵ�� ��ѭ��ԤԼ����Ȼ��鶨ʱ������STATE_IDLEʵ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::ResumeAllVcInstTimer( void )
{
	BOOL32 bRet = TRUE;
	
	CMcuVcInst* pcVcInst = NULL;
	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_CONF; byInstID++)
	{
		pcVcInst = m_apConfInst[byInstID-1];
		if (NULL != pcVcInst)
		{
			if (FALSE == pcVcInst->ResumeInstTimer())
			{
				bRet = FALSE;
			}
		}	
	}
	
	return bRet;
}

/*=============================================================================
    �� �� ���� GetAllVcInstState
    ��    �ܣ� �������ݵ���ʱ ��ȡ ���л���ʵ�� ״̬�� ״̬
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN/OUT]  TMSVcInstState &tVcInstState ״̬����
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetAllVcInstState( TMSVcInstState &tVcInstState )
{
	BOOL32 bRet = TRUE;
	
	u8 byState  = 0;
	CApp* pcApp = &g_cMcuVcApp;
	CMcuVcInst* pcVcInst = NULL;
	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_CONF; byInstID++)
	{
		pcVcInst = (CMcuVcInst*)pcApp->GetInstance(byInstID);
		if (NULL != pcVcInst)
		{
			pcVcInst->GetInstState(byState);	
		}
		else
		{
			byState = 0;
			bRet = FALSE;
		}
		tVcInstState.m_abyState[byInstID-1] = byState;
	}
	
	return bRet;
}

/*=============================================================================
    �� �� ���� SetAllVcInstState
    ��    �ܣ� �������ݵ���ʱ �ָ� ���л���ʵ�� ״̬�� ״̬
	           ͬʱͬ������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN/OUT]  TMSVcInstState &tVcInstState ״̬����
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::SetAllVcInstState( TMSVcInstState &tVcInstState )
{
	BOOL32 bRet = TRUE;

	CApp* pcApp = &g_cMcuVcApp;
	CMcuVcInst* pcVcInst = NULL;

	memset(m_apConfInst, 0, sizeof(m_apConfInst));

	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_CONF; byInstID++)
	{
		pcVcInst = (CMcuVcInst*)pcApp->GetInstance(byInstID);
		if (NULL != pcVcInst)
		{
			pcVcInst->SetInstState(tVcInstState.m_abyState[byInstID-1]);
		}
		else
		{
			bRet = FALSE;
		}
	}

	RefreshHtml();
	
	return bRet;
}

/*=============================================================================
    �� �� ���� FindNextConfInstIDOfNotIdle
    ��    �ܣ� ������һ������ʵ��ID ��ѯ���ڴ�ID����һ���ǿ��л���ʵ��ID
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN] u8  byPreInstID  ��Preview No-Idle-Inst ID
    �� �� ֵ�� u8 - Next No-Idle-Inst ID  [1, MAXNUM_MCU_CONF]  0��Not Find
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
u8 CMcuVcData::FindNextConfInstIDOfNotIdle( u8 byPreInstID )
{
	u8 byNextInstID = 0;
	u8 byState = 0;
	CMcuVcInst* pcVcInst = NULL;
	
	for (u8 byInstID = (byPreInstID+1); byInstID < MAXNUM_MCU_CONF; byInstID++)
	{	
		pcVcInst = m_apConfInst[byInstID-1];
		if (NULL != pcVcInst)
		{
			pcVcInst->GetInstState(byState);
			if (0 != byState)
			{
				byNextInstID = byInstID;
				break;
			}
		}
		
	}
	
	return byNextInstID;
}


/*=============================================================================
�� �� ���� GetTakeModeOfInstId
��    �ܣ� ���ݻ���ʵ��ID ��ѯ������ٿ�ģʽ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ��
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/04/18  4.0			����                  ����
=============================================================================*/
u8 CMcuVcData::GetTakeModeOfInstId( u8 byInstId )
{
    u8 byNextInstID = 0;
    u8 byState = 0;
    CMcuVcInst* pcVcInst = NULL;
    
    if ( byInstId == 0 || byInstId >= MAXNUM_MCU_CONF )
    {
        return 0;   // ����0����Ϊ��Scheduleû�й�ϵ
    }
    
    pcVcInst = m_apConfInst[byInstId-1];
    if (NULL != pcVcInst)
    {
        return pcVcInst->m_tConf.m_tStatus.GetTakeMode();
    }
    else
    {
        return 0;
    }
}

/*=============================================================================
    �� �� ���� GetOneVcInstConfMtTableData
    ��    �ܣ� �������ݵ���ʱ ��ȡ ָ��ʵ���� �����ն˱� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]      u8  byInstID
               [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
               [OUT]     u32 &dwOutBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetOneVcInstConfMtTableData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}
	
	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}
	
	return pcVcInst->GetConfMtTableData(pbyBuf, dwInBufLen, dwOutBufLen);
}

/*=============================================================================
    �� �� ���� SetOneVcInstConfMtTableData
    ��    �ܣ� �������ݵ���ʱ �ָ� ָ��ʵ���� �����ն˱� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8  byInstID
               [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::SetOneVcInstConfMtTableData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}
	
	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}
	
	return pcVcInst->SetConfMtTableData(pbyBuf, dwInBufLen);
}

/*=============================================================================
    �� �� ���� GetOneVcInstConfSwitchTableData
    ��    �ܣ� �������ݵ���ʱ ��ȡ ָ��ʵ���� ���齻���� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]      u8  byInstID
               [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
               [OUT]     u32 &dwOutBufLen
    �� �� ֵ�� BOOL32 
	-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetOneVcInstConfSwitchTableData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}
	
	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}
	
	return pcVcInst->GetConfSwitchTableData(pbyBuf, dwInBufLen, dwOutBufLen);
}

/*=============================================================================
    �� �� ���� SetOneVcInstConfSwitchTableData
    ��    �ܣ� �������ݵ���ʱ �ָ� ָ��ʵ���� ���齻���� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8  byInstID
               [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    �� �� ֵ�� BOOL32 
	-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::SetOneVcInstConfSwitchTableData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}
	
	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}
	
	return pcVcInst->SetConfSwitchTableData(pbyBuf, dwInBufLen);
}

/*=============================================================================
    �� �� ���� GetOneVcInstConfOtherMcTableData
    ��    �ܣ� �������ݵ���ʱ ��ȡ ָ��ʵ���� ����Mc�ն��б� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]      u8  byInstID
               [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
               [OUT]     u32 &dwOutBufLen
    �� �� ֵ�� BOOL32 
	-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetOneVcInstConfOtherMcTableData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}
	
	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}
	
	return pcVcInst->GetConfOtherMcTableData(pbyBuf, dwInBufLen, dwOutBufLen);
}

/*=============================================================================
    �� �� ���� SetOneVcInstConfOtherMcTableData
    ��    �ܣ� �������ݵ���ʱ �ָ� ָ��ʵ���� ����Mc�ն��б� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8  byInstID
               [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    �� �� ֵ�� BOOL32 
	-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::SetOneVcInstConfOtherMcTableData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}
	
	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}
	
	return pcVcInst->SetConfOtherMcTableData(pbyBuf, dwInBufLen);
}

/*=============================================================================
    �� �� ���� GetOneVcInstOtherData
    ��    �ܣ� �������ݵ���ʱ ��ȡ ָ��ʵ���� ��������̬������б���Ϣ���������ͬ�� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]      u8  byInstID
               [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
			   [OUT]     u32 &dwOutBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetOneVcInstOtherData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}
	
	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}
	
	return pcVcInst->GetInstOtherData(pbyBuf, dwInBufLen, dwOutBufLen);
}

/*=============================================================================
    �� �� ���� SetOneVcInstOtherData
    ��    �ܣ� �������ݵ���ʱ �ָ� ָ��ʵ���� ��������̬������б���Ϣ���������ͬ�� ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8  byInstID
               [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [IN]  BOOL32 bResumeTimer
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::SetOneVcInstOtherData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen, BOOL32 bResumeTimer )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}

	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}

	return pcVcInst->SetInstOtherData(pbyBuf, dwInBufLen, bResumeTimer);
}

/*=============================================================================
    �� �� ���� GetCfgFileData
    ��    �ܣ� �������ݵ���ʱ ��ȡ mcu/conf�µ� mcucfg.ini �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
	           [IN]  BOOL32 bPrintErr �Ƿ��ӡʧ����Ϣ
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetCfgFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr )
{
	if (NULL == pbyBuf)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[GetCfgFileData] NULL == pbyBuf\n");
		}
		return FALSE;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME);
	
	FILE *hFile  = fopen(achFullName, "rb");
	s32 nFileLen = 0;
	if (NULL != hFile)
	{
		fseek(hFile, 0, SEEK_END);
		nFileLen = ftell(hFile);
		if (nFileLen > (s32)dwInBufLen)
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[GetCfgFileData] mcucfg.ini FileLen(%d)>%d\n", nFileLen, dwInBufLen);			
			}
			return FALSE;
		}
		if (nFileLen > 0)
		{
			fseek(hFile, 0, SEEK_SET);
			fread(pbyBuf, nFileLen, 1, hFile);
		}
		fclose(hFile);
		hFile = NULL;
	}
	
	//�����ļ�����Ϊ0���ļ�������ͬ����ո��ļ�
	dwOutBufLen = (u32)nFileLen;

	return TRUE;
}

/*=============================================================================
    �� �� ���� SetCfgFileData
    ��    �ܣ� �������ݵ���ʱ �ָ� mcu/conf�µ� mcucfg.ini �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
	           [IN]  BOOL32 bPrintErr �Ƿ��ӡʧ����Ϣ
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::SetCfgFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr )
{
	BOOL32 bRet = FALSE;
	
	//�����ļ�����Ϊ0���ļ�������ͬ����ո��ļ�
	if (NULL == pbyBuf && dwInBufLen > 0)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[SetCfgFileData] NULL == pbyBuf && InBufLen(%d) > 0\n", dwInBufLen);
		}
		return bRet;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME);
	
	BOOL32 bReWrite = TRUE;
	if (0 == dwInBufLen)
	{
		FILE *hFile = fopen(achFullName, "rb");
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
		else
		{
			bReWrite = FALSE;
			bRet = TRUE;
		}
	}
	if (bReWrite)
	{
		FILE *hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			if (0 != dwInBufLen)
			{
				fwrite(pbyBuf, 1, dwInBufLen, hFile);
#ifdef WIN32    //VX�²�������� fflush ����
				fflush(hFile);
#endif
			}
			fclose(hFile);
			hFile = NULL;
			bRet = TRUE;
		}
		else
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[SetCfgFileData] save mcucfg.ini failed\n");
			}
		}
	}

	return bRet;
}

/*=============================================================================
    �� �� ���� GetDebugFileData
    ��    �ܣ� �������ݵ���ʱ ��ȡ mcu/conf�µ� mcudebug.ini �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
	           [IN]  BOOL32 bPrintErr �Ƿ��ӡʧ����Ϣ
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetDebugFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr )
{
	if (NULL == pbyBuf)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[GetDebugFileData] NULL == pbyBuf\n");
		}
		return FALSE;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUDEBUGFILENAME);
	
	FILE *hFile  = fopen(achFullName, "rb");
	s32 nFileLen = 0;
	if (NULL != hFile)
	{
		fseek(hFile, 0, SEEK_END);
		nFileLen = ftell(hFile);
		if (nFileLen > (s32)dwInBufLen)
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[GetDebugFileData] mcudebug.ini FileLen(%d)>%d\n", nFileLen, dwInBufLen);			
			}
			return FALSE;
		}
		if (nFileLen > 0)
		{
			fseek(hFile, 0, SEEK_SET);
			fread(pbyBuf, nFileLen, 1, hFile);
		}
		fclose(hFile);
		hFile = NULL;
	}
	
	//�����ļ�����Ϊ0���ļ�������ͬ����ո��ļ�
	dwOutBufLen = (u32)nFileLen;

	return TRUE;
}

/*=============================================================================
    �� �� ���� SetDebugFileData
    ��    �ܣ� �������ݵ���ʱ �ָ� mcu/conf�µ� mcudebug.ini �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
	           [IN]  BOOL32 bPrintErr �Ƿ��ӡʧ����Ϣ
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::SetDebugFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr )
{
	BOOL32 bRet = FALSE;
	
	//�����ļ�����Ϊ0���ļ�������ͬ����ո��ļ�
	if (NULL == pbyBuf && dwInBufLen > 0)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[SetDebugFileData] NULL == pbyBuf && InBufLen(%d) > 0\n", dwInBufLen);
		}
		return bRet;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUDEBUGFILENAME);
	
	BOOL32 bReWrite = TRUE;
	if (0 == dwInBufLen)
	{
		FILE *hFile = fopen(achFullName, "rb");
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
		else
		{
			bReWrite = FALSE;
			bRet = TRUE;
		}
	}
	if (bReWrite)
	{
		FILE *hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			if (0 != dwInBufLen)
			{
				fwrite(pbyBuf, 1, dwInBufLen, hFile);
#ifdef WIN32    //VX�²�������� fflush ����
				fflush(hFile);
#endif
			}
			fclose(hFile);
			hFile = NULL;
			bRet = TRUE;
		}
		else
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[SetDebugFileData] save mcudebug.ini failed\n");
			}
		}
	}

	return bRet;
}

/*=============================================================================
    �� �� ���� GetAddrbookFileData
    ��    �ܣ� �������ݵ���ʱ ��ȡ mcu/conf�µ� addrbook.kdv �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
	           [IN]  BOOL32 bPrintErr �Ƿ��ӡʧ����Ϣ
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetAddrbookFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr )
{
	if (NULL == pbyBuf)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[GetAddrbookFileData] NULL == pbyBuf\n");
		}
		return FALSE;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, MCUADDRFILENAME);
	
	FILE *hFile  = fopen(achFullName, "rb");
	s32 nFileLen = 0;
	if (NULL != hFile)
	{
		fseek(hFile, 0, SEEK_END);
		nFileLen = ftell(hFile);
		if (nFileLen > (s32)dwInBufLen)
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[GetAddrbookFileData] addrbook.kdv FileLen(%d)>%d\n", nFileLen, dwInBufLen);			
			}
			return FALSE;
		}
		if (nFileLen > 0)
		{
			fseek(hFile, 0, SEEK_SET);
			fread(pbyBuf, nFileLen, 1, hFile);
		}
		fclose(hFile);
		hFile = NULL;
	}
	
	//�����ļ�����Ϊ0���ļ�������ͬ����ո��ļ�
	dwOutBufLen = (u32)nFileLen;

	return TRUE;
}

/*=============================================================================
    �� �� ���� SetAddrbookFileData
    ��    �ܣ� �������ݵ���ʱ �ָ� mcu/conf�µ� addrbook.kdv �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
	           [IN]  BOOL32 bPrintErr �Ƿ��ӡʧ����Ϣ
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::SetAddrbookFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr )
{
	BOOL32 bRet = FALSE;

	//�����ļ�����Ϊ0���ļ�������ͬ����ո��ļ�
	if (NULL == pbyBuf && dwInBufLen > 0)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[SetAddrbookFileData] NULL == pbyBuf && InBufLen(%d) > 0\n", dwInBufLen);
		}
		return bRet;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, MCUADDRFILENAME);
	
	BOOL32 bReWrite = TRUE;
	if (0 == dwInBufLen)
	{
		FILE *hFile = fopen(achFullName, "rb");
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
		else
		{
			bReWrite = FALSE;
			bRet = TRUE;
		}
	}
	if (bReWrite)
	{
		FILE *hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			if (0 != dwInBufLen)
			{
				fwrite(pbyBuf, 1, dwInBufLen, hFile);
#ifdef WIN32    //VX�²�������� fflush ����
				fflush(hFile);
#endif
			}
			fclose(hFile);
			hFile = NULL;
			bRet = TRUE;
		}
		else
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[SetAddrbookFileData] save addrbook.kdv failed\n");
			}
		}
	}

	return bRet;
}

/*=============================================================================
    �� �� ���� GetConfinfoFileData
    ��    �ܣ� �������ݵ���ʱ ��ȡ mcu/data�µ� confinfo.dat �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
	           [IN]  BOOL32 bPrintErr �Ƿ��ӡʧ����Ϣ
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr )
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcConfState)) //?? �����С��Ҫ�б�
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[GetConfinfoFileData] NULL==pbyBuf InBufLen.%d\n", dwInBufLen);
		}
		return FALSE;
	}

	TMSVcConfState *ptConfState = (TMSVcConfState*)pbyBuf;
	u8 *pbyConfDataBuf = pbyBuf+sizeof(TMSVcConfState);
	u32 dwConfDataTotalLen = 0;
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	memset(pbyBuf, 0, sizeof(TMSVcConfState));	
	memset(acConfId, 0, sizeof(acConfId));
	
	//��ȡ ����ͷ��Ϣ��ȱʡ������Ϣ_�洢�ļ�
	if (TRUE == GetAllConfHeadFromFile(acConfId, sizeof(acConfId)))
	{
		u8  byLoop = 0;
		u16 wConfDataLen = 0;
		
		//�������һ��λ���ϵ�ȱʡ������Ϣ
		for (byLoop = 0; byLoop < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1); byLoop++)
		{
			if (FALSE == acConfId[byLoop].IsNull())
			{
				if (TRUE == ::GetConfDataFromFile(byLoop, acConfId[byLoop], 
					                              pbyConfDataBuf, sizeof(TConfStore), wConfDataLen)) 
				{
					ptConfState->m_abyValid[byLoop] = 1;
					ptConfState->m_awConfDataLen[byLoop] = htons(wConfDataLen);
					pbyConfDataBuf += wConfDataLen;
					dwConfDataTotalLen += wConfDataLen;

                    // ���� [4/28/2006]
                    // OspPrintf( TRUE, FALSE, "[GuZhenhua MS Dump] Loaded Confinfo_%d!\n", byLoop );
				}
				else
				{
					OspPrintf(TRUE, FALSE, "[GetConfinfoFileData] GetConfDataFromFile Err: Loop.%d\n", byLoop);
				}
			}
		}
	}
	else
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[GetConfinfoFileData] GetAllConfHeadFromFile Err\n");
		}
	}
	
	//�����ļ�����Ϊ0���ļ�������ͬ�����
	dwOutBufLen = sizeof(TMSVcConfState)+dwConfDataTotalLen;

    // ���� [4/28/2006] ����ΪʲôҪдһ�飿����
    // ��ʱע��
	// SetConfinfoFileData(pbyBuf, dwOutBufLen, TRUE);

	return TRUE;
}

/*=============================================================================
    �� �� ���� SetConfinfoFileData
    ��    �ܣ� �������ݵ���ʱ �ָ� mcu/data�µ� confinfo.dat �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
	           [IN]  BOOL32 bPrintErr �Ƿ��ӡʧ����Ϣ
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::SetConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr )
{
	//�����ļ�����Ϊ0���ļ�������ͬ����ո��ļ�
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcConfState))
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[SetConfinfoFileData] NULL==pbyBuf || InBufLen(%d)>0\n", dwInBufLen);
		}
		return FALSE;
	}
	
	TMSVcConfState *ptConfState = (TMSVcConfState*)pbyBuf;
	u8 *pbyConfDataBuf = pbyBuf+sizeof(TMSVcConfState);
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	memset(acConfId, 0, sizeof(acConfId));
	    
	u8  byLoop = 0;
	u16 wConfDataLen = 0;
	CConfId cConfId;
	cConfId.SetNull();
	for (byLoop = 0; byLoop < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1); byLoop++)
	{
		wConfDataLen = ntohs(ptConfState->m_awConfDataLen[byLoop]);
		if (1 == ptConfState->m_abyValid[byLoop])
		{
			if (TRUE == ::SetConfDataToFile(byLoop, cConfId, pbyConfDataBuf, wConfDataLen))
			{
                // ���� [4/28/2006]
                // OspPrintf( TRUE, FALSE, "[GuZhenhua MS Dump] Writed Confinfo_%d!\n", byLoop );
				acConfId[byLoop] = cConfId;
			}
			else
			{
				OspPrintf(TRUE, FALSE, "[SetConfinfoFileData] SetConfDataToFile Err: Loop.%d\n", byLoop);
			}
			pbyConfDataBuf += wConfDataLen;
		}
	}

	if (FALSE == SetAllConfHeadToFile(acConfId, sizeof(acConfId)))
	{
		OspPrintf(TRUE, FALSE, "[SetConfinfoFileData] SetAllConfHeadToFile Err\n");
	}

    // ���� [4/28/2006]
    // OspPrintf( TRUE, FALSE, "[GuZhenhua MS Dump] Writed Confinfo_head!\n" );

	return TRUE;
}

/*=============================================================================
    �� �� ���� GetLoguserFileData
    ��    �ܣ� �������ݵ���ʱ ��ȡ mcu/data�µ� login.usr �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
	           [IN]  BOOL32 bPrintErr �Ƿ��ӡʧ����Ϣ
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::GetLoguserFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr )
{
	if (NULL == pbyBuf)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[GetLoguserFileData] NULL == pbyBuf\n");
		}
		return FALSE;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, USERINFOFILENAME);
	
	FILE *hFile  = fopen(achFullName, "rb");
	s32 nFileLen = 0;
	if (NULL != hFile)
	{
		fseek(hFile, 0, SEEK_END);
		nFileLen = ftell(hFile);
		if (nFileLen > (s32)dwInBufLen)
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[GetLoguserFileData] login.usr FileLen(%d)>%d\n", nFileLen, dwInBufLen);
			}
			return FALSE;
		}
		if (nFileLen > 0)
		{
			fseek(hFile, 0, SEEK_SET);
			fread(pbyBuf, nFileLen, 1, hFile);
		}
		fclose(hFile);
		hFile = NULL;
	}
	
	//�����ļ�����Ϊ0���ļ�������ͬ����ո��ļ�
	dwOutBufLen = (u32)nFileLen;

	return TRUE;
}

/*=============================================================================
    �� �� ���� SetLoguserFileData
    ��    �ܣ� �������ݵ���ʱ �ָ� mcu/data�µ� login.usr �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
	           [IN]  BOOL32 bPrintErr �Ƿ��ӡʧ����Ϣ
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuVcData::SetLoguserFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr )
{
	BOOL32 bRet = FALSE;

	//�����ļ�����Ϊ0���ļ�������ͬ����ո��ļ�
	if (NULL == pbyBuf && dwInBufLen > 0)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[SetLoguserFileData] NULL == pbyBuf && InBufLen(%d) > 0\n", dwInBufLen);
		}
		return bRet;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, USERINFOFILENAME);
	
	BOOL32 bReWrite = TRUE;
	if (0 == dwInBufLen)
	{
		FILE *hFile = fopen(achFullName, "rb");
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
		else
		{
			bReWrite = FALSE;
			bRet = TRUE;
		}
	}
	if (bReWrite)
	{
		FILE *hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			if (0 != dwInBufLen)
			{
				fwrite(pbyBuf, 1, dwInBufLen, hFile);
#ifdef WIN32    //VX�²�������� fflush ����
				fflush(hFile);
#endif
			}
			fclose(hFile);
			hFile = NULL;
			bRet = TRUE;
		}
		else
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[SetLoguserFileData] save login.usr failed\n");
			}
		}
	}

	return bRet;
}

/*=============================================================================
    �� �� ���� GetUserExFileData
    ��    �ܣ� �������ݵ���ʱ ��ȡ mcu/data�µ� usrgrp.usr �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
	           [IN]  BOOL32 bPrintErr �Ƿ��ӡʧ����Ϣ
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/06/22  4.0			����                ����
=============================================================================*/
BOOL32 CMcuVcData::GetUserExFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr )
{
	if (NULL == pbyBuf)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[GetUserExFileData] NULL == pbyBuf\n");
		}
		return FALSE;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, USERGRPINFOFILENAME);
	
	FILE *hFile  = fopen(achFullName, "rb");
	s32 nFileLen = 0;
	if (NULL != hFile)
	{
		fseek(hFile, 0, SEEK_END);
		nFileLen = ftell(hFile);
		if (nFileLen > (s32)dwInBufLen)
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[GetUserExFileData] usrgrp.usr FileLen(%d)>%d\n", nFileLen, dwInBufLen);
			}
			return FALSE;
		}
		if (nFileLen > 0)
		{
			fseek(hFile, 0, SEEK_SET);
			fread(pbyBuf, nFileLen, 1, hFile);
		}
		fclose(hFile);
		hFile = NULL;
	}
	
	//�����ļ�����Ϊ0���ļ�������ͬ����ո��ļ�
	dwOutBufLen = (u32)nFileLen;

	return TRUE;
}

/*=============================================================================
    �� �� ���� SetUserExFileData
    ��    �ܣ� �������ݵ���ʱ �ָ� mcu/data�µ� usrgrp.usr �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
	           [IN]  BOOL32 bPrintErr �Ƿ��ӡʧ����Ϣ
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/06/22  4.0			����                ����
=============================================================================*/
BOOL32 CMcuVcData::SetUserExFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr )
{
	BOOL32 bRet = FALSE;

	//�����ļ�����Ϊ0���ļ�������ͬ����ո��ļ�
	if (NULL == pbyBuf && dwInBufLen > 0)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[SetUserExFileData] NULL == pbyBuf && InBufLen(%d) > 0\n", dwInBufLen);
		}
		return bRet;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, USERGRPINFOFILENAME);
	
	BOOL32 bReWrite = TRUE;
	if (0 == dwInBufLen)
	{
		FILE *hFile = fopen(achFullName, "rb");
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
		else
		{
			bReWrite = FALSE;
			bRet = TRUE;
		}
	}
	if (bReWrite)
	{
		FILE *hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			if (0 != dwInBufLen)
			{
				fwrite(pbyBuf, 1, dwInBufLen, hFile);
#ifdef WIN32    //VX�²�������� fflush ����
				fflush(hFile);
#endif
			}
			fclose(hFile);
			hFile = NULL;
			bRet = TRUE;
		}
		else
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[SetUserExFileData] save usrgrp.usr failed\n");
			}
		}
	}

	return bRet;
}
/*=============================================================================
    �� �� ���� GetUnProcConfinfoFileData
    ��    �ܣ� �������ݵ���ʱ ��ȡ mcu/data�µ� unprocconfinfo.dat �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/06/22  4.0			fxh                   ����
=============================================================================*/
BOOL32 CMcuVcData::GetUnProcConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen)
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcConfState)) 
	{
		OspPrintf(TRUE, FALSE, "[GetUnProcConfinfoFileData] NULL==pbyBuf InBufLen.%d\n", dwInBufLen);
		return FALSE;
	}

	TMSVcConfState *ptConfState = (TMSVcConfState*)pbyBuf;
	u8 *pbyConfDataBuf = pbyBuf + sizeof(TMSVcConfState);
	u32 dwConfDataTotalLen = 0;
	CConfId acConfId[MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1];
	memset(pbyBuf, 0, sizeof(TMSVcConfState));	
	memset(acConfId, 0, sizeof(acConfId));
	
	//��ȡ ����ͷ��Ϣ��ȱʡ������Ϣ_�洢�ļ�
	if (TRUE == GetAllConfHeadFromFile(acConfId, sizeof(acConfId)))
	{
		u16 wUnProcConfDataLen = 0;		
		for (u8 byLoop = 0; byLoop < (MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE); byLoop++)
		{
			if (FALSE == acConfId[byLoop].IsNull())
			{
				if (TRUE == ::GetUnProConfDataToFile(byLoop, (s8*)pbyConfDataBuf, wUnProcConfDataLen)) 
				{
					ptConfState->m_abyValid[byLoop] = 1;
					ptConfState->m_awConfDataLen[byLoop] = htons(wUnProcConfDataLen);
					pbyConfDataBuf += wUnProcConfDataLen;
					dwConfDataTotalLen += wUnProcConfDataLen;
				}
				else
				{
					OspPrintf(TRUE, FALSE, "[GetUnProcConfinfoFileData] GetUnProConfDataToFile Err: Loop.%d\n", byLoop);
				}
			}
		}
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[GetUnProcConfinfoFileData] GetAllConfHeadFromFile Err\n");
	}
	
	dwOutBufLen = sizeof(TMSVcConfState) + dwConfDataTotalLen;
	return TRUE;
}
/*=============================================================================
    �� �� ���� SetUnProcConfinfoFileData
    ��    �ܣ� �������ݵ���ʱ ����mcu/data�µ� unprocconfinfo.dat �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/06/22  4.0			fxh                   ����
=============================================================================*/
BOOL32 CMcuVcData::SetUnProcConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen)
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcConfState))
	{
		OspPrintf(TRUE, FALSE, "[SetUnProcConfinfoFileData] NULL==pbyBuf || InBufLen(%d)< sizeof(TMSVcConfState)\n", dwInBufLen);
		return FALSE;
	}
	
	TMSVcConfState *ptConfState = (TMSVcConfState*)pbyBuf;
	u8 *pbyConfDataBuf = pbyBuf + sizeof(TMSVcConfState);    
	u16 wConfDataLen = 0;
	for (u8 byLoop = 0; byLoop < (MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE); byLoop++)
	{
		wConfDataLen = ntohs(ptConfState->m_awConfDataLen[byLoop]);
		if (1 == ptConfState->m_abyValid[byLoop])
		{
			if (FALSE == ::SetUnProConfDataToFile(byLoop, (s8*)pbyConfDataBuf, wConfDataLen))
			{
				OspPrintf(TRUE, FALSE, "[SetUnProcConfinfoFileData] SetUnProConfDataToFile Err: Loop.%d\n", byLoop);
			}
			pbyConfDataBuf += wConfDataLen;
		}
	}
	return TRUE;
}
/*=============================================================================
    �� �� ���� GetVCSLoguserFileData
    ��    �ܣ� �������ݵ���ʱ ��ȡ mcu/data�µ� vcslogin.usr �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/04/03  4.0			fxh                   ����
=============================================================================*/
BOOL32 CMcuVcData::GetVCSLoguserFileData(u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen)
{
	if (NULL == pbyBuf)
	{
		OspPrintf(TRUE, FALSE, "[GetVCSLoguserFileData] NULL == pbyBuf\n");
		return FALSE;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, MCU_VCSUSER_FILENAME);
	
	FILE *hFile  = fopen(achFullName, "rb");
	s32 nFileLen = 0;
	if (NULL != hFile)
	{
		fseek(hFile, 0, SEEK_END);
		nFileLen = ftell(hFile);
		if (nFileLen > (s32)dwInBufLen)
		{
			OspPrintf(TRUE, FALSE, "[GetVCSLoguserFileData] vcslogin.usr FileLen(%d)>%d\n", nFileLen, dwInBufLen);
			fclose(hFile);
			hFile = NULL;
			return FALSE;
		}

		if (nFileLen > 0)
		{
			fseek(hFile, 0, SEEK_SET);
			fread(pbyBuf, nFileLen, 1, hFile);
		}
		fclose(hFile);
		hFile = NULL;
		dwOutBufLen = (u32)nFileLen;
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[GetVCSLoguserFileData] open vcslogin.usr Fail\n");
		return FALSE;
	}
	return TRUE;
}
/*=============================================================================
    �� �� ���� SetVCSLoguserFileData
    ��    �ܣ� �������ݵ���ʱ ����mcu/data�µ� vcslogin.usr �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/06/22  4.0			fxh                   ����
=============================================================================*/
BOOL32 CMcuVcData::SetVCSLoguserFileData(u8 *pbyBuf, u32 dwInBufLen)
{
	BOOL32 bRet = FALSE;
	if (NULL == pbyBuf || dwInBufLen < 0)
	{
		OspPrintf(TRUE, FALSE, "[SetVCSLoguserFileData] NULL == pbyBuf || InBufLen(%d) < 0 \n", dwInBufLen);
		return bRet;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, MCU_VCSUSER_FILENAME);
	
	BOOL32 bReWrite = TRUE;
	if (0 == dwInBufLen)
	{
		FILE *hFile = fopen(achFullName, "rb");
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
		else
		{
			bReWrite = FALSE;
			bRet = TRUE;
		}
	}

	if (bReWrite)
	{
		FILE *hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			if (0 != dwInBufLen)
			{
				fwrite(pbyBuf, 1, dwInBufLen, hFile);
#ifdef WIN32    //VX�²�������� fflush ����
				fflush(hFile);
#endif
			}
			fclose(hFile);
			hFile = NULL;
			bRet = TRUE;
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[SetVCSLoguserFileData] fopen vcslogin.usr failed\n");
		}
	}

	return bRet;
}
/*=============================================================================
    �� �� ���� GetVCSUserTaskFileData
    ��    �ܣ� �������ݵ���ʱ ��ȡ mcu/data�µ� usrtask.dat �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/04/03  4.0			fxh                   ����
=============================================================================*/
BOOL32 CMcuVcData::GetVCSUserTaskFileData(u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen)
{
	if (NULL == pbyBuf)
	{
		OspPrintf(TRUE, FALSE, "[GetVCSUserTaskFileData] NULL == pbyBuf\n");
		return FALSE;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, USERTASKINFOFILENAME);
	
	FILE *hFile  = fopen(achFullName, "rb");
	s32 nFileLen = 0;
	if (NULL != hFile)
	{
		fseek(hFile, 0, SEEK_END);
		nFileLen = ftell(hFile);
		if (nFileLen > (s32)dwInBufLen)
		{
			OspPrintf(TRUE, FALSE, "[GetVCSUserTaskFileData] usrtask.dat FileLen(%d)>%d\n", nFileLen, dwInBufLen);
			return FALSE;
		}
		if (nFileLen > 0)
		{
			fseek(hFile, 0, SEEK_SET);
			fread(pbyBuf, nFileLen, 1, hFile);
		}
		fclose(hFile);
		hFile = NULL;
		dwOutBufLen = (u32)nFileLen;
	}
	return TRUE;
}
/*=============================================================================
    �� �� ���� SetVCSUserTaskFileData
    ��    �ܣ� �������ݵ���ʱ ����mcu/data�µ� usrtask.dat �ļ���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/06/22  4.0			fxh                   ����
=============================================================================*/
BOOL32 CMcuVcData::SetVCSUserTaskFileData(u8 *pbyBuf, u32 dwInBufLen)
{
	BOOL32 bRet = FALSE;
	if (NULL == pbyBuf || dwInBufLen < 0)
	{
		OspPrintf(TRUE, FALSE, "[SetVCSUserTaskFileData] NULL == pbyBuf || InBufLen(%d) < 0 \n", dwInBufLen);
		return bRet;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, USERTASKINFOFILENAME);
	
	BOOL32 bReWrite = TRUE;
	if (0 == dwInBufLen)
	{
		FILE *hFile = fopen(achFullName, "rb");
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
		else
		{
			bReWrite = FALSE;
			bRet = TRUE;
		}
	}

	if (bReWrite)
	{
		FILE *hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			if (0 != dwInBufLen)
			{
				fwrite(pbyBuf, 1, dwInBufLen, hFile);
#ifdef WIN32    //VX�²�������� fflush ����
				fflush(hFile);
#endif
			}
			fclose(hFile);
			hFile = NULL;
			bRet = TRUE;
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[SetVCSUserTaskFileData] save usrtask.dat failed\n");
		}
	}

	return bRet;
}
/*=============================================================================
    �� �� ���� ReloadMcuUserList
    ��    �ܣ� �û��б�ģ�����´��ļ�װ���û��б�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/01/05  4.0			����                  ����
    2006/06/22  4.0         ����                  �����û���֧��
=============================================================================*/
BOOL32 CMcuVcData::ReloadMcuUserList( void )
{
	BOOL32 bRet = g_cUsrManage.GetAllUserFromFile();    
    g_cUsrManage.SetSingle(FALSE);

    CUserFullInfo cUserInfo;

    s32 nNum = g_cUsrManage.GetUserNum();
    if (nNum == 0)
    {
        printf("[UserMgrLoad]Load User ret: %d. User Number: %d\n", bRet, nNum);
    }    

    // ����һ��admin

    for (s32 nLoop = 0; nLoop < nNum; nLoop ++)
    {        
        g_cUsrManage.GetUserFullInfo( &cUserInfo,  nLoop );

        if ( cUserInfo.IsEqualName("admin") )
        {
            // �����admin�Ļ�����Ҫ����һ�¡��������Զ����ɵ�
            // Ҫ������������
            CExUsrInfo cNewExnfo = cUserInfo;

            char *szDesc = cUserInfo.GetDiscription();
            if ( szDesc !=  NULL &&
                 (u8)szDesc[0] != USRGRPID_SADMIN )
            {
                cNewExnfo.SetDiscription( szDesc );
                cNewExnfo.SetUsrGrpId( USRGRPID_SADMIN );
                if (!g_cUsrManage.ModifyInfo( &cNewExnfo ))
                {
                    OspPrintf(TRUE, FALSE, "[ReloadMcuUserList] Modify Admin error: %d!\n", g_cUsrManage.GetLastError());
                }
            }

            break;
        }
    }

    // ��ȡ�û�����Ϣ
    bRet = bRet & m_cUsrGrpInfo.Load();

    return bRet;
}

/*=============================================================================
  �� �� ���� GetMcuEqpCapacity
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� [OUT] TEqpCapacity& tMcuEqpCap
  �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/02/25  4.0			������                  ����
=============================================================================*/
void CMcuVcData::GetMcuEqpCapacity(TEqpCapacity& tMcuEqpCap)
{

	u8  byVmpNum = 0;
	u8  byMixNum = 0;
	u8  byTvNum = 0;
	u8  byMpwNum = 0;
	u8 byEqpIdLp = 0;
	
	TBasReqInfo tBasCap;
	GetBasCapacity( tBasCap );  // ȡBas����ͨ����

    // ȡ����BAS����ͨ����
    THDBasReqInfo tHDBasCap;
    GetHDBasCapacity( tHDBasCap );

	TEqpReqInfo tPrsCap;
	GetPrsCapacity( tPrsCap ); // ȡPrs����ͨ����
	
	// ����ǽ����ͨ���� ?
	
    while( byEqpIdLp < MAXNUM_MCU_PERIEQP )
	{
		if( IsPeriEqpValid( byEqpIdLp )
			&& m_atPeriEqpTable[byEqpIdLp-1].m_tPeriEqpStatus.m_byOnline )
		{
			// mix
			if( EQP_TYPE_MIXER == GetEqpType(byEqpIdLp) )
			{
                //zbq[04/10/2008] ������û��׼���ò���Ϊ�û���������. ֻ��¼����������.
                TPeriEqpStatus tEqpStatus;
                if (!GetPeriEqpStatus(byEqpIdLp, &tEqpStatus))
                {
                    OspPrintf(TRUE, FALSE, "[GetMcuEqpCapacity] get Mixer.%d's status failed\n", byEqpIdLp);
                    continue;
                }
                u8 byLoopGrpId = 0;
                TMixerStatus tMixerStatus = tEqpStatus.m_tStatus.tMixer;
                while( byLoopGrpId < tMixerStatus.m_byGrpNum )
                {
                    if( tMixerStatus.m_atGrpStatus[byLoopGrpId].m_byGrpState == TMixerGrpStatus::READY )
                    {
                        byMixNum++;
                    }
                    byLoopGrpId++;				
                }

				//byMixNum++; // ֻ��¼�˻���������
			}

			// recorder 
			if( EQP_TYPE_RECORDER == GetEqpType(byEqpIdLp) )
			{
				// do nothing;
			}
						
			// tv
			if( EQP_TYPE_TVWALL == GetEqpType(byEqpIdLp) )
			{
				byTvNum++;
			}
						
			// bas
			if( EQP_TYPE_BAS == GetEqpType(byEqpIdLp) )
			{
				// do nothing;
			}
			
			// vmp
			if(EQP_TYPE_VMP == GetEqpType( byEqpIdLp )
				&& m_atPeriEqpTable[byEqpIdLp-1].m_tPeriEqpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::IDLE )
			{
				byVmpNum++;
			}

			// mpw 
			if( EQP_TYPE_VMPTW == GetEqpType( byEqpIdLp) )
			{
				byMpwNum++;
			}
			// prs
			if( EQP_TYPE_PRS == GetEqpType( byEqpIdLp ) )
			{
				// get prs channels
			}
		}		
        byEqpIdLp++;
	}

    // mix
	if( 0 != byMixNum )
	{
		tMcuEqpCap.m_tMixCap.SetNeedEqp(TRUE);
		tMcuEqpCap.m_tMixCap.SetNeedChannles( byMixNum );
	}
	else
	{
		tMcuEqpCap.m_tMixCap.SetNeedEqp(FALSE);
	}
	// tvwall
	if( 0 != byTvNum )
	{
		tMcuEqpCap.m_tTvCap.SetNeedEqp(TRUE);
		tMcuEqpCap.m_tTvCap.SetNeedChannles( byTvNum );
	}
	else
	{
		tMcuEqpCap.m_tTvCap.SetNeedEqp(FALSE);
	}
	// vmp
	if( 0!= byVmpNum)
	{
		tMcuEqpCap.m_tVmpCap.SetNeedEqp( TRUE );
		tMcuEqpCap.m_tVmpCap.SetNeedChannles( byVmpNum );
	}
	else
	{
		tMcuEqpCap.m_tVmpCap.SetNeedEqp( FALSE );
	}
	// mpw
	if( 0 != byMpwNum )
	{
		tMcuEqpCap.m_tMpwCap.SetNeedEqp( TRUE );
		tMcuEqpCap.m_tMpwCap.SetNeedChannles( byMpwNum );
	}
	else
	{
		tMcuEqpCap.m_tMpwCap.SetNeedEqp( FALSE );
	}
	
	// bas
	tMcuEqpCap.m_tBasCap = tBasCap;

    // HDBas
    tMcuEqpCap.m_tHDBasCap = tHDBasCap;

	// prs
	tMcuEqpCap.m_tPrsCap = tPrsCap;

	return;
}

/*=============================================================================
  �� �� ���� GetConfEqpDemand
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� [IN] TConfInfo& tConfInfo
             [OUT] TEqpCapacity& tConfEqpDemand
  �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/02/25  4.0			������                  ����
=============================================================================*/
void CMcuVcData::GetConfEqpDemand(TConfInfo& tConfInfo, TEqpCapacity& tConfEqpDemand )
{
	TConfAttrb tConfAttribute = tConfInfo.GetConfAttrb();
	tConfEqpDemand.m_tVmpCap.SetNeedEqp( tConfAttribute.IsHasVmpModule() || CONF_VMPMODE_AUTO == tConfInfo.m_tStatus.GetVMPMode() ); 
	tConfEqpDemand.m_tPrsCap.SetNeedEqp( tConfAttribute.IsResendLosePack() );// prs

    // ����BAS�͸���BAS����, zgc, 2008-08-11
    BOOL32 bIsNeedBas = FALSE;
    BOOL32 bIsNeedHDBas = FALSE;
    if ( IsHDConf(tConfInfo) && tConfAttribute.IsUseAdapter() )
    {   
        bIsNeedHDBas = TRUE;
    }
    tConfEqpDemand.m_tHDBasCap.SetNeedEqp( bIsNeedHDBas );
    
    if ( tConfAttribute.IsUseAdapter() &&
         (( tConfInfo.GetSecAudioMediaType() != MEDIA_TYPE_NULL && 
           tConfInfo.GetMainAudioMediaType() != tConfInfo.GetSecAudioMediaType() ) ||
          ( tConfInfo.GetSecVideoMediaType() != MEDIA_TYPE_NULL &&
            tConfInfo.GetMainVideoMediaType() != tConfInfo.GetSecVideoMediaType() ) ||
          ( tConfInfo.GetSecBitRate() != 0 && 
            tConfInfo.GetBitRate() != tConfInfo.GetSecBitRate())  ) )
    {
        bIsNeedBas = TRUE;
    }
    tConfEqpDemand.m_tBasCap.SetNeedEqp( FALSE );
	//tConfEqpDemand.m_tBasCap.SetNeedEqp( tConfAttribute.IsUseAdapter() ); // bas
	tConfEqpDemand.m_tMixCap.SetNeedEqp( tConfAttribute.IsDiscussConf()  ); // mixer

	tConfEqpDemand.m_tMpwCap.SetNeedEqp( FALSE );
	tConfEqpDemand.m_tRecCap.SetNeedEqp( FALSE );

	// tvwall(�����ǽû�п���) (����ǽͨ�������ڻ�����Ϣ�У������������ж�)
	tConfEqpDemand.m_tTvCap.SetNeedEqp( tConfAttribute.IsHasTvWallModule() ); // tv
	
	u8 byNeedPrsChannles = DEFAULT_PRS_CHANNELS;
	u8 byNeedVidBasChannels = 0;
	u8 byNeedAudBasChannels = 0;

	// bas (����ͨ����Ŀ)
    // guzh [8/28/2007] ����� 8000B/8000C�������Ƿ���˫ý����飨ֻ��Ҫ1·��Ƶ����)
#ifdef _MINIMCU_
    BOOL32 bNeedBas = FALSE;
#endif
    // ����BAS����ͨBAS�ֿ�, zgc, 2008-08-11
	//if( tConfAttribute.IsUseAdapter() )
    if ( bIsNeedBas )
	{
		u8 byMainMediaType   = tConfInfo.GetMainAudioMediaType();
		u8 bySecondMediaType = tConfInfo.GetSecAudioMediaType();
		if (MEDIA_TYPE_NULL != bySecondMediaType && byMainMediaType != bySecondMediaType)
		{
			byNeedPrsChannles++;
			byNeedAudBasChannels++;
		}

		byMainMediaType = tConfInfo.GetMainVideoMediaType();
		bySecondMediaType = tConfInfo.GetSecVideoMediaType();
		u8 byMainVidFormat = tConfInfo.GetMainVideoFormat();
		u8 bySecVidFormat = tConfInfo.GetSecVideoFormat();
		if ( (MEDIA_TYPE_NULL != bySecondMediaType && byMainMediaType != bySecondMediaType)
			||(byMainMediaType == bySecondMediaType && byMainVidFormat != bySecVidFormat) )
		{
			byNeedPrsChannles++;
#ifdef _MINIMCU_
            bNeedBas = TRUE;
#else
			byNeedVidBasChannels++;
#endif
		}

		if( 0 != tConfInfo.GetSecBitRate() )
		{
			byNeedPrsChannles++;
#ifdef _MINIMCU_
            bNeedBas = TRUE;
#else
			byNeedVidBasChannels++;
#endif
		}

		tConfEqpDemand.m_tBasCap.SetAudChannels( byNeedAudBasChannels );
#ifdef _MINIMCU_
        tConfEqpDemand.m_tBasCap.SetNeedChannles( bNeedBas ? 1: 0 );
#else
		tConfEqpDemand.m_tBasCap.SetNeedChannles( byNeedVidBasChannels );
#endif
	}

    // ����BAS
    if ( bIsNeedHDBas )
    {
        u8 byNVChn = 0;
        u8 byDSChnVGA = 0;
        u8 byDSChnH263p = 0;

        CBasMgr cBasMgr;
        cBasMgr.GetNeededMau(tConfInfo, byNVChn, byDSChnH263p, byDSChnVGA);

        tConfEqpDemand.m_tHDBasCap.SetNeedChannles( byNVChn );
        tConfEqpDemand.m_tHDBasCap.SetDVidChannels( byDSChnVGA );
        tConfEqpDemand.m_tHDBasCap.SetDVidH263pChn( byDSChnH263p );
    }

	// prs (����ͨ����Ŀ)
	if(tConfAttribute.IsResendLosePack())
	{
		tConfEqpDemand.m_tPrsCap.SetNeedChannles( byNeedPrsChannles );
	}
	return;
}

/*=============================================================================
  �� �� ���� AnalyEqpCapacity
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  [IN] TEqpCapacity& tConfEqpDemand
              [IN] TEqpCapacity& tMcuSupportCap
  �� �� ֵ�� BOOL32: TRUE: Mcu֧�ָû������������ FALSE����֧��
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/02/25  4.0			������                  ����
=============================================================================*/
BOOL32 CMcuVcData::AnalyEqpCapacity( TEqpCapacity& tConfEqpDemand, TEqpCapacity& tMcuSupportCap)
{
	BOOL32 bRet = TRUE;
	// bas
	if( tConfEqpDemand.m_tBasCap.IsNeedEqp() ) // need bas
	{
		if( tMcuSupportCap.m_tBasCap.IsNeedEqp() ) // support bas
		{
			if( tMcuSupportCap.m_tBasCap.GetAudChannels() < tConfEqpDemand.m_tBasCap.GetAudChannels() ||
				tMcuSupportCap.m_tBasCap.GetSupportChannels() < tConfEqpDemand.m_tBasCap.GetSupportChannels() )
			{
				bRet = FALSE;
			}
		}
		else
		{
			bRet = FALSE;
		}
	}

    // hdbas, zgc, 2008-08-11
    if( tConfEqpDemand.m_tHDBasCap.IsNeedEqp() ) // need bas
    {
        if( tMcuSupportCap.m_tHDBasCap.IsNeedEqp() ) // support bas
        {
            //������MPU�Ƿ����㣬����MAU�Ƿ����㣬��ռ�õ�˳�򱣳ֶ���
            if (tMcuSupportCap.m_tHDBasCap.GetMpuChn() > 0 &&
                tMcuSupportCap.m_tHDBasCap.GetMpuChn() >= ( tConfEqpDemand.m_tHDBasCap.GetDVidChannels() +
                                                            tConfEqpDemand.m_tHDBasCap.GetSupportChannels() + 
                                                            tConfEqpDemand.m_tHDBasCap.GetDVidH263pChn() )
               )
            {
                bRet = TRUE;
            }
            else if( tMcuSupportCap.m_tHDBasCap.GetDVidChannels() >= tConfEqpDemand.m_tHDBasCap.GetDVidChannels() &&
                     tMcuSupportCap.m_tHDBasCap.GetSupportChannels() >= tConfEqpDemand.m_tHDBasCap.GetSupportChannels() &&
                     tMcuSupportCap.m_tHDBasCap.GetDVidH263pChn() >= tConfEqpDemand.m_tHDBasCap.GetDVidH263pChn())
            {
                bRet = TRUE;
            }
            else
            {
                bRet = FALSE;
            }
        }
        else
        {
            bRet = FALSE;
        }
	}

	// prs
	if( tConfEqpDemand.m_tPrsCap.IsNeedEqp() )
	{
		if( tMcuSupportCap.m_tPrsCap.IsNeedEqp() )
		{
			if( tMcuSupportCap.m_tPrsCap.GetSupportChannels() < tConfEqpDemand.m_tPrsCap.GetSupportChannels() )
			{
				bRet = FALSE;
			}
		}
		else
		{
			bRet = FALSE;
		}
	}
	
	// tv
	if( tConfEqpDemand.m_tTvCap.IsNeedEqp() )
	{
		if( !tMcuSupportCap.m_tTvCap.IsNeedEqp() )
		{
			bRet = FALSE;
		}
	}

	// vmp
	if( tConfEqpDemand.m_tVmpCap.IsNeedEqp() )
	{
		if( !tMcuSupportCap.m_tVmpCap.IsNeedEqp() )
		{
			bRet = FALSE;
		}
	}

	// mixer
	if( tConfEqpDemand.m_tMixCap.IsNeedEqp()  )
	{
		if( !tMcuSupportCap.m_tMixCap.IsNeedEqp() )
		{
			bRet = FALSE;
		}
	}
/*
	// mpw
	if( tConfEqpDemand.m_tMpwCap.IsNeedEqp()  )
	{
		if( !tMcuSupportCap.m_tMpwCap.IsNeedEqp() )
		{
			bRet = FALSE;
		}
	}
*/
	return bRet;
}

/*=============================================================================
  �� �� ���� GetBasCapacity
  ��    �ܣ� ȡBas������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TBasReqInfo& tBasCap
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcData::GetBasCapacity(TBasReqInfo& tBasCap )
{
	u8 byAudChannels = 0;
	u8 byVidChannels = 0;
	u8 byBasIdLp = BASID_MIN;
	TBasStatus *ptStatus = NULL;
	
	while( byBasIdLp >= BASID_MIN && byBasIdLp <= BASID_MAX )
	{
		if( IsPeriEqpValid( byBasIdLp )
            && !g_cMcuAgent.IsEqpBasHD(byBasIdLp)
			&& m_atPeriEqpTable[byBasIdLp-1].m_tPeriEqpStatus.m_byOnline )
		{
			ptStatus = &(m_atPeriEqpTable[byBasIdLp-1].m_tPeriEqpStatus.m_tStatus.tBas);
			u8 byMinChnl = 0;
			u8 byMaxChnl = ptStatus->byChnNum;

			//���ҿ���ͨ��
			for (u8 byChnIdx = byMinChnl; byChnIdx < byMaxChnl; byChnIdx++)
			{
				if (!ptStatus->tChnnl[byChnIdx].IsReserved())
				{
					u8 byBasChnnlType = ptStatus->tChnnl[byChnIdx].GetChannelType();
					if (BAS_CHAN_AUDIO == byBasChnnlType)
					{
						byAudChannels++;
					}
					else if (BAS_CHAN_VIDEO == byBasChnnlType)
					{
						byVidChannels++;
					}
				}
			}
		}
		byBasIdLp++;
	}
	
	tBasCap.SetNeedEqp(FALSE);
	tBasCap.SetAudChannels( 0 );
	tBasCap.SetNeedChannles( 0 );

	if( 0 != byAudChannels )
	{
		tBasCap.SetNeedEqp(TRUE);
		tBasCap.SetAudChannels( byAudChannels );
	}

	if( 0!= byVidChannels )
	{
		tBasCap.SetNeedEqp( TRUE );
		tBasCap.SetNeedChannles( byVidChannels );
	}

	return;
}

/*=============================================================================
�� �� ���� GetHDBasCapacity
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� THDBasReqInfo& tHDBasCap
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/8/11   4.0		    �ܹ��                  ����
2008/12/12  4.5         �ű���                  ����H263p����
=============================================================================*/
void CMcuVcData::GetHDBasCapacity( THDBasReqInfo& tHDBasCap )
{
    u8 byBasIdLp = BASID_MIN;
	THdBasStatus *ptStatus = NULL;

    u8 byHDBasVidChnnls = 0;
    u8 byHDBasDVidChnnls = 0;
    u8 byHDBasDVidH263p = 0;
    u8 byHDBasMpuChns  = 0;
    
    u8 byLoop = 0;
    while( byBasIdLp >= BASID_MIN && byBasIdLp <= BASID_MAX )
    {
        if( IsPeriEqpValid( byBasIdLp ) &&
            g_cMcuAgent.IsEqpBasHD( byBasIdLp ) &&
            m_atPeriEqpTable[byBasIdLp-1].m_tPeriEqpStatus.m_byOnline )
        {
            ptStatus = &(m_atPeriEqpTable[byBasIdLp-1].m_tPeriEqpStatus.m_tStatus.tHdBas);

            if (TYPE_MAU_NORMAL == ptStatus->GetEqpType())
            {
                for ( byLoop = 0; byLoop < MAXNUM_MAU_VCHN; byLoop++ )
                {
                    if (!ptStatus->tStatus.tMauBas.GetVidChnStatus( byLoop )->IsReserved() &&
                        !ptStatus->tStatus.tMauBas.GetVidChnStatus( byLoop )->IsTempReserved() )
                    {
                        byHDBasVidChnnls++;
                    }
                }
                for ( byLoop = 0; byLoop < MAXNUM_MAU_DVCHN; byLoop++ )
                {
                    if (!ptStatus->tStatus.tMauBas.GetDVidChnStatus( byLoop )->IsReserved() &&
                        !ptStatus->tStatus.tMauBas.GetDVidChnStatus( byLoop )->IsTempReserved() )
                    {
                        byHDBasDVidChnnls++;
                    }
                }
            }
            else if(TYPE_MAU_H263PLUS == ptStatus->GetEqpType())
            {
                byHDBasDVidH263p ++;
            }
            else if (TYPE_MPU == ptStatus->GetEqpType())
            {
                for ( byLoop = 0; byLoop < MAXNUM_MPU_VCHN; byLoop++ )
                {
                    if (!ptStatus->tStatus.tMpuBas.GetVidChnStatus( byLoop )->IsReserved() &&
                        !ptStatus->tStatus.tMpuBas.GetVidChnStatus( byLoop )->IsTempReserved() )
                    {
                        byHDBasMpuChns++;
                    }
                }
            }
        }
        byBasIdLp++;
	}

    tHDBasCap.SetNeedEqp( FALSE );
    tHDBasCap.SetNeedChannles( 0 );
    tHDBasCap.SetDVidChannels( 0 );
    tHDBasCap.SetMpuChn(0);

    if ( byHDBasVidChnnls > 0 )
    {
        tHDBasCap.SetNeedEqp( TRUE );
        tHDBasCap.SetNeedChannles( byHDBasVidChnnls );
    }
    if ( byHDBasDVidChnnls > 0 )
    {
        tHDBasCap.SetNeedEqp( TRUE );
        tHDBasCap.SetDVidChannels( byHDBasDVidChnnls );
    }
    if (byHDBasDVidH263p > 0)
    {
        tHDBasCap.SetNeedEqp( TRUE );
        tHDBasCap.SetDVidH263pChn( byHDBasDVidH263p );
    }
    if (byHDBasMpuChns > 0)
    {
        tHDBasCap.SetNeedEqp( TRUE );
        tHDBasCap.SetMpuChn(byHDBasMpuChns);
    }

    return;
}

/*=============================================================================
  �� �� ���� GetPrsCapacity
  ��    �ܣ� ȡPrs������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpReqInfo& tPrsCap
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcData::GetPrsCapacity(TEqpReqInfo& tPrsCap )
{
	u8 byPrsIdLp = PRSID_MIN;
	u8 byPrsChannels = 0;

	tPrsCap.SetNeedEqp( FALSE );
	tPrsCap.SetNeedChannles( 0 );

	TPrsStatus *ptStatus = NULL;
	while (byPrsIdLp >= PRSID_MIN && byPrsIdLp <= PRSID_MAX)
    {
        if ( 1 == m_atPeriEqpTable[byPrsIdLp-1].m_tPeriEqpStatus.m_byOnline ) // ����
		{
			if (IsPeriEqpValid(byPrsIdLp) 
				&& EQP_TYPE_PRS == GetEqpType(byPrsIdLp) ) // �Ϸ��Լ��
			{
				ptStatus = &(m_atPeriEqpTable[byPrsIdLp - 1].m_tPeriEqpStatus.m_tStatus.tPrs);
				byPrsChannels += ptStatus->GetIdleChlNum();
            }
        }
		byPrsIdLp++;
    }

	if( 0 != byPrsChannels )
	{
		tPrsCap.SetNeedEqp(TRUE);
		tPrsCap.SetNeedChannles( byPrsChannels ); 
	}
	return;
}

/*=============================================================================
  �� �� ���� GetConfFECType
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� [IN] u8 byConfIdx
             [IN] u8 byMode
  �� �� ֵ�� u8: 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/10/11  4.0			�ű���                ����
=============================================================================*/
u8 CMcuVcData::GetConfFECType( u8 byConfIdx, u8 byMode )
{
    u8 byFECType = FECTYPE_NONE;

    if ( byConfIdx > MAX_CONFIDX ) 
    {
        OspPrintf( TRUE, FALSE, "[GetConfFECType] ConfIdx.%d, unexpected, ignore it\n", byConfIdx );
        return byFECType;
    }

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if(!tMapData.IsValidConf()) 
    {
        OspPrintf(TRUE, FALSE, "[GetConfFECType] invalid instance id :%d\n", tMapData.GetInsId());
        return byFECType;
    }
    
    if ( NULL == m_apConfInst[tMapData.GetInsId()-1] )
    {
        OspPrintf(TRUE, FALSE, "[GetConfFECType] Inst.%d has no conf, unexpected\n", tMapData.GetInsId());
        return byFECType;
    }
    switch( byMode )
    {
    case MODE_AUDIO:
        byFECType = m_apConfInst[tMapData.GetInsId()-1]->m_tConf.GetCapSupportEx().GetAudioFECType();
        break;

    case MODE_VIDEO:
        byFECType = m_apConfInst[tMapData.GetInsId()-1]->m_tConf.GetCapSupportEx().GetVideoFECType();
    	break;

    case MODE_BOTH:
        {
        u8 byFECTypeA = m_apConfInst[tMapData.GetInsId()-1]->m_tConf.GetCapSupportEx().GetAudioFECType();
        u8 byFECTypeV = m_apConfInst[tMapData.GetInsId()-1]->m_tConf.GetCapSupportEx().GetVideoFECType();
        byFECType = byFECTypeA == byFECTypeV ? byFECTypeV : FECTYPE_NONE;
        }
        break;

    case MODE_SECVIDEO:
        byFECType = m_apConfInst[tMapData.GetInsId()-1]->m_tConf.GetCapSupportEx().GetDVideoFECType();
        break;

    default:
        OspPrintf( TRUE, FALSE, "[GetConfFECType] unexpected mode.%d, ignore it\n", byMode );
        break;
    }

    return byFECType;
}

/*=============================================================================
  �� �� ���� GetConfEncryptMode
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� [IN] u8 byConfIdx
  �� �� ֵ�� u8: 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/10/16  4.0			�ű���                ����
=============================================================================*/
u8 CMcuVcData::GetConfEncryptMode( u8 byConfIdx )
{
    u8 byEncMode = CONF_ENCRYPTMODE_NONE;

    if ( byConfIdx > MAX_CONFIDX ) 
    {
        OspPrintf( TRUE, FALSE, "[GetConfEncryptMode] ConfIdx.%d, unexpected, ignore it\n", byConfIdx );
        return byEncMode;
    }
    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if(!tMapData.IsValidConf()) 
    {
        OspPrintf(TRUE, FALSE, "[GetConfEncryptMode] invalid instance id :%d\n", tMapData.GetInsId());
        return byEncMode;
    }
    
    if ( NULL == m_apConfInst[tMapData.GetInsId()-1] )
    {
        OspPrintf( TRUE, FALSE, "[GetConfEncryptMode] ConfIdx.%d unexist, ignore it\n", byConfIdx );
        return byEncMode;
    }

    return m_apConfInst[tMapData.GetInsId()-1]->m_tConf.GetMediaKey().GetEncryptMode();
}

/********************************************

        �û�����չ

  *******************************************/


/*=============================================================================
  �� �� ���� GetGrpById
  ��    �ܣ�����GroupId�����飬û�ҵ�����False
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32       FALSE û���ҵ�
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
=============================================================================*/
BOOL32 CUsrGrpsInfo::GetGrpById(u8 byGrpId, TUsrGrpInfo &tGrpInfo) const
{
    tGrpInfo.SetNull();
    if (byGrpId == USRGRPID_INVALID ||
        byGrpId > MAXNUM_USRGRP)
    {
        // �Ƿ�
        return FALSE;
    }
    tGrpInfo = m_atInfo[byGrpId-1];
    return TRUE;
    
    /*
    for ( s32 nLoop = 0; nLoop < m_nGrpNum; nLoop ++ )
    {
        if ( m_atInfo[ nLoop ].GetUsrGrpId() == byGrpId )
        {
            tGrpInfo = m_atInfo[nLoop];
            return TRUE;
        }
    }    
    */
    
}


/*=============================================================================
  �� �� ���� GetGrpByName
  ��    �ܣ��������������飬û�ҵ�����False
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32       FALSE û���ҵ�
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
=============================================================================*/
BOOL32 CUsrGrpsInfo::GetGrpByName(LPCSTR szName,  TUsrGrpInfo &tGrpInfo) const
{
    tGrpInfo.SetNull();
    for ( s32 nLoop = 0; nLoop < MAXNUM_USRGRP; nLoop ++ )
    {
        if ( strncmp( m_atInfo[ nLoop ].GetUsrGrpName(), szName, MAX_CHARLENGTH ) == 0 )
        {
            tGrpInfo = m_atInfo[nLoop];
            return TRUE;
        }
    }    
    return FALSE;    
}


/*=============================================================================
  �� �� ���� AddGrp
  ��    �ܣ�����顣������Id���������� USRGRPID_INVALID
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u8         ��Id
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
    06/09/07    4.0         ����        �������ԣ������û��Լ�������ID
=============================================================================*/
u8 CUsrGrpsInfo::AddGrp ( const TUsrGrpInfo &tGrpInfo )
{
/*
    if ( m_nGrpNum >= MAXNUM_USRGRP )
    {
        return USRGRPID_INVALID;
    }

    TUsrGrpInfo tTargetInfo = tGrpInfo;
    // ���ҵ�һ�����õ���ID
    tTargetInfo.byGrpId = USRGRPID_INVALID;
    
    BOOL32 bIdUsed[MAXNUM_USRGRP + 1];
    memset( &bIdUsed, 0, sizeof(bIdUsed) );
    u8 byLoop = 0;
    for (; byLoop < m_nGrpNum; byLoop ++)
    {
        bIdUsed [ m_atInfo[byLoop].byGrpId ] = TRUE;
    }

    for ( byLoop = 1; byLoop < MAXNUM_USRGRP + 1; byLoop ++)
    {
        if ( !bIdUsed[byLoop] )
        {
            // ��һ��δʹ�õ�
            break;
        }
    }

    if ( byLoop > MAXNUM_USRGRP )
    {
        // û�п��У�Ӧ�ò�����
        return USRGRPID_INVALID;           
    }
    
    tTargetInfo.byGrpId = byLoop;
    m_atInfo[m_nGrpNum] = tTargetInfo;
    m_nGrpNum ++;
    return tTargetInfo.byGrpId;
*/
    
    u8 byTargetId = tGrpInfo.GetUsrGrpId();
    TUsrGrpInfo tOldOne;
    if ( GetGrpById(byTargetId, tOldOne) )
    {
        if (!tOldOne.IsFree())
        {
            // ��ID����
            return USRGRPID_INVALID;
        }
    }
    else
    {
        // �Ƿ�
        return USRGRPID_INVALID;
    }

    // ����
    m_atInfo[byTargetId-1] = tGrpInfo;
    return byTargetId;
}


/*=============================================================================
  �� �� ���� ChgGrp
  ��    �ܣ��޸�������������Ƿ��ҵ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
=============================================================================*/
BOOL32 CUsrGrpsInfo::ChgGrp ( const TUsrGrpInfo &tGrpInfo )
{
    TUsrGrpInfo tOldOne;
    if ( !GetGrpById(tGrpInfo.GetUsrGrpId(), tOldOne) ) 
    {
        return FALSE;
    }
    if (tOldOne.IsFree())
    {
        return FALSE;
    }

    m_atInfo[tGrpInfo.GetUsrGrpId()-1] = tGrpInfo;

    return TRUE;
}

/*=============================================================================
  �� �� ���� DelGrp
  ��    �ܣ� ɾ������������������������Դ�ͷŲ����������Ƿ��ҵ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
=============================================================================*/
BOOL32 CUsrGrpsInfo::DelGrp ( u8 byGrpId )
{
    /*
    for (s32 nLoop = 0; nLoop < m_nGrpNum; nLoop ++)
    {
        if ( m_atInfo[nLoop].byGrpId == byGrpId )
        {
            // �ҵ���ɾ��
            // �ƶ������
            for ( s32 nLoop2 = nLoop + 1; nLoop2 < m_nGrpNum; nLoop2 ++ )
            {
                m_atInfo[nLoop2 - 1] = m_atInfo[nLoop2];
            }

            m_atInfo[m_nGrpNum-1].SetNull();
            m_nGrpNum --;

            return TRUE;
        }
    }
    return FALSE;    
    */

    TUsrGrpInfo tOldOne;
    if ( !GetGrpById(byGrpId, tOldOne) ) 
    {
        return FALSE;
    }
    if (tOldOne.IsFree())
    {
        return FALSE;
    }

    m_atInfo[byGrpId-1].SetNull();
    return TRUE;
}

/*=============================================================================
  �� �� ���� Save
  ��    �ܣ� ���浽�ļ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
=============================================================================*/
BOOL32 CUsrGrpsInfo::Save() const
{
    s8 szPath[KDV_MAX_PATH] = {0};
    
    sprintf(szPath, "%s/%s", DIR_DATA, USERGRPINFOFILENAME);

    FILE *fp;
    fp = fopen( szPath, "wb" );
    if (fp == NULL)
    {
        return FALSE;
    }

    //fwrite( &m_nGrpNum, sizeof(s32), 1, fp );  
    //fwrite( m_atInfo, sizeof( TUsrGrpInfo ), m_nGrpNum, fp );
    s32 nTmp = MAXNUM_USRGRP;
    fwrite( &nTmp, sizeof(s32), 1, fp );  
    fwrite( m_atInfo, sizeof( TUsrGrpInfo ), MAXNUM_USRGRP, fp );

#ifdef WIN32
    fflush(fp);
#endif

    fclose(fp);

    return TRUE;       
}


/*=============================================================================
  �� �� ���� Load
  ��    �ܣ� �û�����Ϣ���ļ���ȡ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
=============================================================================*/
BOOL32 CUsrGrpsInfo::Load()
{
    s8 szPath[KDV_MAX_PATH] = {0};
    
    sprintf(szPath, "%s/%s", DIR_DATA, USERGRPINFOFILENAME);

    FILE *fp;
    fp = fopen( szPath, "rb" );
    if (fp == NULL)
    {
        return FALSE;
    }

    fread( &m_nGrpNum, sizeof(s32), 1, fp );  
    fread( m_atInfo, sizeof( TUsrGrpInfo ), m_nGrpNum, fp );
    m_nGrpNum = MAXNUM_USRGRP;

    fclose(fp);

    return TRUE;
}

/*=============================================================================
  �� �� ���� Print
  ��    �ܣ� ���Դ�ӡ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
=============================================================================*/
void CUsrGrpsInfo::Print() const
{
    OspPrintf(TRUE, FALSE, "User Group Info, Total Number: %d\n", m_nGrpNum);
    for (s32 nIndex = 0; nIndex < m_nGrpNum; nIndex ++)
    {
        if (m_atInfo[nIndex].IsFree())
        {
            continue;
        }
        OspPrintf(TRUE, FALSE, "User Group Info Index: %d\n", nIndex);
        m_atInfo[nIndex].Print();
        // ��Щ�û����������

        OspPrintf(TRUE, FALSE, "\tUsers in Group:\n");
        CExUsrInfo cInfo;
        for ( s32 nUserLoop = 0; nUserLoop < g_cUsrManage.GetUserNum(); nUserLoop ++ )
        {
            g_cUsrManage.GetUserFullInfo( &cInfo, nUserLoop );
            if ( cInfo.GetUsrGrpId() == m_atInfo[nIndex].GetUsrGrpId() )
            {
                OspPrintf(TRUE, FALSE, "\t\t%s\n", cInfo.GetName());
            }
        }
    }
}

/*=============================================================================
  �� �� ���� GetUsrGrpUserCount
  ��    �ܣ� ��ȡ�����û���ǰ���������ֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� cUsrGrpObj: �������û��������
			 cUsrGrp:    �������û���������             
			 byGrpId��   ָ�����û����
             byMaxNum��  ָ����������������û���
			 byNum��     ָ����Ŀǰ���û���
  �� �� ֵ�� BOOL32              FALSE û���ҵ�
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
	08/11/21                ���㻪        �����������̨��VCS MCS����ȡ���಻ͬ���û��������
	                                      ���Բ������Ե��û�����
=============================================================================*/
BOOL32  CMcuVcData::GetUsrGrpUserCount(CUsrManage& cUsrManageObj, 
									   CUsrGrpsInfo& cUsrGrpObj, u8 byGrpId, 
									   u8 &byMaxNum, u8 &byNum)
{
    //��ΪN+1���ݹ���ģʽ�����л��󣩣�ȡ�ڴ��е���Ϣ
    if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
    {
        u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
        if (0 == byInsId || byInsId > MAXNUM_NPLUS_MCU)
        {
            OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] McuSwitchedInsId is invalid in NPlus switched mode.\n");
            return FALSE;
        }
        else
        {
            CApp *pcApp = &g_cNPlusApp;
            CNPlusInst *pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL != pcInst)
            {
                return pcInst->GetGrpUsrCount(byGrpId, byMaxNum, byNum);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] GetInstance(%d) failed.\n", byInsId);
                return FALSE;
            }
        }                
    }

    if (byGrpId == USRGRPID_SADMIN)
    {
        // ����ǳ�������Ա����ϵͳֵ
        byMaxNum = MAXNUM_GRPUSRNUM;
        byNum = cUsrManageObj.GetUserNum();
        return TRUE;
    }

    TUsrGrpInfo tGrpInfo;
    BOOL32 bRet = cUsrGrpObj.GetGrpById( byGrpId, tGrpInfo );

    byNum = 0;
    byMaxNum = 0;

    if ( !bRet )
    {
        // û�ҵ�
        return FALSE;
    }

    byMaxNum = tGrpInfo.GetMaxUsrNum();

    s32 nTotalUserNum = cUsrManageObj.GetUserNum();

    CExUsrInfo cUsrInfo;

    for (s32 nUsrLoop = 0; nUsrLoop < nTotalUserNum; nUsrLoop ++)
    {
        cUsrManageObj.GetUserFullInfo( &cUsrInfo, nUsrLoop );
        if ( byGrpId == cUsrInfo.GetUsrGrpId() )
        {
            byNum ++;
        }
    }
    return TRUE;
}

/*=============================================================================
  �� �� ���� GetMCSUsrGrpUserCount
  ��    �ܣ� ��ȡMCS����̨��Ӧ�û�����û���ǰ���������ֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����        
			 byGrpId��   ָ�����û����
             byMaxNum��  ָ����������������û���
			 byNum��     ָ����Ŀǰ���û���
  �� �� ֵ�� BOOL32              FALSE û���ҵ�
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/21                ���㻪        ����
=============================================================================*/
BOOL32  CMcuVcData::GetMCSUsrGrpUserCount(u8 byGrpId, u8 &byMaxNum, u8 &byNum)
{
	return GetUsrGrpUserCount(g_cUsrManage, m_cUsrGrpInfo, byGrpId, byMaxNum, byNum);
}
/*=============================================================================
  �� �� ���� GetVCSUsrGrpUserCount
  ��    �ܣ� ��ȡVCS����̨��Ӧ�û�����û���ǰ���������ֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����        
			 byGrpId��   ָ�����û����
             byMaxNum��  ָ����������������û���
			 byNum��     ָ����Ŀǰ���û���
  �� �� ֵ�� BOOL32              FALSE û���ҵ�
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/21                ���㻪        ����
=============================================================================*/
BOOL32  CMcuVcData::GetVCSUsrGrpUserCount(u8 byGrpId, u8 &byMaxNum, u8 &byNum)
{
	return GetUsrGrpUserCount(g_cVCSUsrManage, m_cVCSUsrGrpInfo, byGrpId, byMaxNum, byNum);
}

/*=============================================================================
  �� �� ���� AddUserGroup
  ��    �ܣ� ����һ���û���
  �㷨ʵ�֣� CUsrGrpsInfo: �������û������
             TUsrGrpInfo:  ��ӵ��û�����Ϣ
			 wErrorNo��    ʧ��ԭ��
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u8              ������ID����USRGRPID_INVALID ��ʾ���ʧ�ܣ���дwErrorNo��
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
    06/09/07    4.0         ����        �������ԣ������û��Լ������û���ID
=============================================================================*/
u8 CMcuVcData::AddUserGroup( CUsrGrpsInfo &cUsrGrpObj, const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo  )
{
    wErrorNo = 0;
    /*
    TUsrGrpInfo tOldOne;
    if ( m_cUsrGrpInfo.GetGrpByName( tGrpInfo.szName, tOldOne ) )
    {
        // �Ѵ���
        wErrorNo = UM_EX_USRGRPDUP;
        return USRGRPID_INVALID;
    }

    u8 byGrpId = m_cUsrGrpInfo.AddGrp( tGrpInfo );

    if (byGrpId == USRGRPID_INVALID)
    {
        // �û�����
        wErrorNo = UM_EX_USRGRPFULL;
    }
    else
    {
        m_cUsrGrpInfo.Save();
    } 
    */
    
    if (tGrpInfo.GetUsrGrpId() == USRGRPID_INVALID ||
        tGrpInfo.GetUsrGrpId() > MAXNUM_USRGRP)
    {
        // �Ƿ�����
        wErrorNo = UM_EX_USRGRPIDINV;
        return USRGRPID_INVALID;
    }
    TUsrGrpInfo tOldOne;
    if ( cUsrGrpObj.GetGrpByName( tGrpInfo.GetUsrGrpName(), tOldOne ) )
    {
        //  �����Ѵ���
        wErrorNo = UM_EX_USRGRPDUP;
        return USRGRPID_INVALID;
    }

    u8 byGrpId = cUsrGrpObj.AddGrp( tGrpInfo );

    if (byGrpId == USRGRPID_INVALID)
    {
        // ��ID�Ѵ���
        wErrorNo = UM_EX_USRGRPIDDUP;
    }
    else
    {
        cUsrGrpObj.Save();
    } 

    return byGrpId;
}

/*=============================================================================
  �� �� ���� AddMCSUserGroup
  ��    �ܣ� ����һ��MCS�û���
  �㷨ʵ�֣�
             TUsrGrpInfo:  ��ӵ��û�����Ϣ
			 wErrorNo��    ʧ��ԭ��
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u8              ������ID����USRGRPID_INVALID ��ʾ���ʧ�ܣ���дwErrorNo��
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/21                ���㻪        ����
=============================================================================*/
u8 CMcuVcData::AddMCSUserGroup( const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo  )
{
    return AddUserGroup(m_cUsrGrpInfo, tGrpInfo, wErrorNo);
}

/*=============================================================================
  �� �� ���� AddVCSUserGroup
  ��    �ܣ� ����һ��VCS�û���
  �㷨ʵ�֣�
             TUsrGrpInfo:  ��ӵ��û�����Ϣ
			 wErrorNo��    ʧ��ԭ��
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u8              ������ID����USRGRPID_INVALID ��ʾ���ʧ�ܣ���дwErrorNo��
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/21                ���㻪        ����
=============================================================================*/
u8 CMcuVcData::AddVCSUserGroup( const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo  )
{
    return AddUserGroup(m_cVCSUsrGrpInfo, tGrpInfo, wErrorNo);
}

/*=============================================================================
  �� �� ���� ChgUserGroup
  ��    �ܣ� �޸�һ���û��飬���ʧ����д wErrorNo
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CUsrGrpsInfo: �������û������
             TUsrGrpInfo:  ��ӵ��û�����Ϣ
			 wErrorNo��    ʧ��ԭ��
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
=============================================================================*/
BOOL32 CMcuVcData::ChgUserGroup(CUsrGrpsInfo &cUsrGrpObj, const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo )
{
    wErrorNo = 0;

    if ( !cUsrGrpObj.ChgGrp( tGrpInfo ) )
    {
        // û���ҵ�
        wErrorNo = UM_EX_USRGRPNOEXIST;
        return FALSE;
    }

    cUsrGrpObj.Save();

    return TRUE;    
}
/*=============================================================================
  �� �� ���� ChgMCSUserGroup
  ��    �ܣ� �޸�MCS��һ���û��飬���ʧ����д wErrorNo
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TUsrGrpInfo:  �޸ĵ��û�����Ϣ
			 wErrorNo��    ʧ��ԭ��
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/21                ���㻪        ����
=============================================================================*/
BOOL32 CMcuVcData::ChgMCSUserGroup(const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo )
{
    return ChgUserGroup(m_cUsrGrpInfo, tGrpInfo, wErrorNo);  
}

/*=============================================================================
  �� �� ���� ChgVCSUserGroup
  ��    �ܣ� �޸�VCS��һ���û��飬���ʧ����д wErrorNo
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TUsrGrpInfo:  ��ӵ��û�����Ϣ
			 wErrorNo��    ʧ��ԭ��
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/21                ���㻪        ����
=============================================================================*/
BOOL32 CMcuVcData::ChgVCSUserGroup(const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo )
{
    return ChgUserGroup(m_cVCSUsrGrpInfo, tGrpInfo, wErrorNo);  
}
/*=============================================================================
  �� �� ���� DelMCSUserGroup
  ��    �ܣ� ɾ��һ��MCS�û��飬ͬʱ�����ص���Դ�ͷš����ʧ����д wErrorNo
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
=============================================================================*/
BOOL32 CMcuVcData::DelMCSUserGroup( u8 byGrpId, u16 &wErrorNo )
{
    wErrorNo = 0;

    if ( ! m_cUsrGrpInfo.DelGrp( byGrpId ) )
    {
        // û���ҵ�
        wErrorNo = UM_EX_USRGRPNOEXIST;
        return FALSE;
    }

    // ɾ��������ص���Ϣ�������û���ģ�桢�����
    CServMsg cServMsg;
    cServMsg.SetMcuId(LOCAL_MCUID);
    cServMsg.SetSrcMtId(0);
    cServMsg.SetSrcSsnId(0);
    
    cServMsg.SetMsgBody();
    


    s32 nUserNum = g_cUsrManage.GetUserNum();
    CExUsrInfo cInfo;
    for (s32 nLoop = nUserNum -1 ;nLoop >= 0; nLoop --)
    {
        g_cUsrManage.GetUserFullInfo( &cInfo, nLoop);
        if (cInfo.GetUsrGrpId() == byGrpId)
        {
            g_cUsrManage.DelUser( &cInfo );

            // ֪ͨ
            cServMsg.SetEventId(MCU_MCS_DELUSER_NOTIF);
            cServMsg.SetMsgBody( (u8*)cInfo.GetName(), MAX_CHARLENGTH );
			g_cMcsSsnApp.Broadcast2SpecGrpMcsSsn( byGrpId, MCU_MCS_DELUSER_NOTIF, 
											      cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
    }


    cServMsg.SetMsgBody();

	for (u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
	{
		TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
		if (tMapData.IsTemUsed())
		{
			TTemplateInfo tTemConf;
			if (g_cMcuVcApp.GetTemplate(byConfIdx, tTemConf))
			{
                if ( byGrpId == tTemConf.m_tConfInfo.GetUsrGrpId() &&
					 MCS_CONF == tTemConf.m_tConfInfo.GetConfSource())
                {
                    // ����ģ��                            
                    cServMsg.SetEventId(MCS_MCU_DELTEMPLATE_REQ);
                    cServMsg.SetConfId( tTemConf.m_tConfInfo.GetConfId() );
                    g_cMcuVcApp.SendMsgToDaemonConf(MCS_MCU_DELTEMPLATE_REQ,
                                                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen());                    
                }
			}
		}

        
		if (tMapData.IsValidConf())
		{
			CMcuVcInst *pcMcuVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
			if (NULL == pcMcuVcInst)
			{
				continue;
			}

			TConfInfo* ptConfInfo = &pcMcuVcInst->m_tConf;
            if ( byGrpId == ptConfInfo->GetUsrGrpId() && 
				 MCS_CONF == ptConfInfo->GetConfSource())
            {
                // ��������
                cServMsg.SetEventId(MCS_MCU_RELEASECONF_REQ);
                cServMsg.SetConfId( ptConfInfo->GetConfId() );
                g_cMcuVcApp.SendMsgToConf(cServMsg.GetConfId(), MCS_MCU_RELEASECONF_REQ,
                                          cServMsg.GetServMsg(), cServMsg.GetServMsgLen());  
            }
		}
	}        

    // TODO: ɾ���û�����ʱ��ɾ����ػ���¼��

    m_cUsrGrpInfo.Save();

    return TRUE;
}
/*=============================================================================
  �� �� ���� DelVCSUserGroup
  ��    �ܣ� ɾ��һ��VCS�û��飬ͬʱ�����ص���Դ�ͷš����ʧ����д wErrorNo
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
=============================================================================*/
BOOL32 CMcuVcData::DelVCSUserGroup( u8 byGrpId, u16 &wErrorNo )
{
    wErrorNo = 0;

    if ( ! m_cVCSUsrGrpInfo.DelGrp( byGrpId ) )
    {
        // û���ҵ�
        wErrorNo = UM_EX_USRGRPNOEXIST;
        return FALSE;
    }

    // ɾ��������ص���Ϣ�������û���ģ�桢�����
    CServMsg cServMsg;
    cServMsg.SetMcuId(LOCAL_MCUID);
    cServMsg.SetSrcMtId(0);
    cServMsg.SetSrcSsnId(0);    
    cServMsg.SetMsgBody();
    


    s32 nUserNum = g_cVCSUsrManage.GetUserNum();
    CExUsrInfo cInfo;
    for (s32 nLoop = nUserNum -1 ;nLoop >= 0; nLoop --)
    {
        g_cVCSUsrManage.GetUserFullInfo( &cInfo, nLoop);
        if (cInfo.GetUsrGrpId() == byGrpId)
        {
            g_cVCSUsrManage.DelUser( &cInfo );

            // ֪ͨ
            cServMsg.SetEventId(MCU_MCS_DELUSER_NOTIF);
            cServMsg.SetMsgBody( (u8*)cInfo.GetName(), MAX_CHARLENGTH );
			g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn( byGrpId, MCU_MCS_DELUSER_NOTIF, 
											      cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
    }


    cServMsg.SetMsgBody();

	for (u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
	{
		TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
		if (tMapData.IsTemUsed())
		{
			TTemplateInfo tTemConf;
			if (g_cMcuVcApp.GetTemplate(byConfIdx, tTemConf))
			{
                if ( byGrpId == tTemConf.m_tConfInfo.GetUsrGrpId() &&
					 VCS_CONF == tTemConf.m_tConfInfo.GetConfSource())
                {
                    // ����ģ��                            
                    cServMsg.SetEventId(MCS_MCU_DELTEMPLATE_REQ);
                    cServMsg.SetConfId(tTemConf.m_tConfInfo.GetConfId());
                    g_cMcuVcApp.SendMsgToDaemonConf(MCS_MCU_DELTEMPLATE_REQ,
                                                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen());                    
                }
			}
		}

        
		if (tMapData.IsValidConf())
		{
			CMcuVcInst *pcMcuVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
			if (NULL == pcMcuVcInst)
			{
				continue;
			}

			TConfInfo* ptConfInfo = &pcMcuVcInst->m_tConf;
            if ( byGrpId == ptConfInfo->GetUsrGrpId() && 
				 VCS_CONF == ptConfInfo->GetConfSource())
            {
                // ��������
                cServMsg.SetEventId(MCS_MCU_RELEASECONF_REQ);
                cServMsg.SetConfId( ptConfInfo->GetConfId() );
                g_cMcuVcApp.SendMsgToConf(cServMsg.GetConfId(), MCS_MCU_RELEASECONF_REQ,
                                          cServMsg.GetServMsg(), cServMsg.GetServMsgLen());  
            }
		}
	}        

    // TODO: ɾ���û�����ʱ��ɾ����ػ���¼��

    m_cVCSUsrGrpInfo.Save();

    return TRUE;
}

/*=============================================================================
  �� �� ���� GetMCSUserGroupInfo
  ��    �ܣ� ��ȡ�����û�����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
=============================================================================*/
void  CMcuVcData::GetMCSUserGroupInfo ( u8 &byNum, TUsrGrpInfo **ptInfo )
{
    //��ΪN+1���ݹ���ģʽ�����л��󣩣�ȡ�ڴ��е���Ϣ
    if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
    {
        u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
        if (0 == byInsId || byInsId > MAXNUM_NPLUS_MCU)
        {
            OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] McuSwitchedInsId is invalid in NPlus switched mode.\n");
        }
        else
        {
            CApp *pcApp = &g_cNPlusApp;
            CNPlusInst *pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL != pcInst)
            {
                CUsrGrpsInfo* pcUsrGrpsInfo = pcInst->GetUsrGrpInfo();
                byNum = pcUsrGrpsInfo->GetGrpNum();
                *ptInfo = pcUsrGrpsInfo->m_atInfo;                
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] GetInstance(%d) failed.\n", byInsId);
            }
        }                
        return ;
    }

    // ��ͨģʽ
    byNum = (u8)m_cUsrGrpInfo.GetGrpNum();

    *ptInfo = m_cUsrGrpInfo.m_atInfo;

}
/*=============================================================================
  �� �� ���� GetVCSUserGroupInfo
  ��    �ܣ� ��ȡVCS�����û�����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/21                ���㻪        ����
=============================================================================*/
void  CMcuVcData::GetVCSUserGroupInfo ( u8 &byNum, TUsrGrpInfo **ptInfo )
{
    //��ΪN+1���ݹ���ģʽ�����л��󣩣�ȡ�ڴ��е���Ϣ
//     if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
//     {
//         u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
//         if (0 == byInsId || byInsId > MAXNUM_NPLUS_MCU)
//         {
//             OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] McuSwitchedInsId is invalid in NPlus switched mode.\n");
//         }
//         else
//         {
//             CApp *pcApp = &g_cNPlusApp;
//             CNPlusInst *pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
//             if (NULL != pcInst)
//             {
//                 CUsrGrpsInfo* pcUsrGrpsInfo = pcInst->GetUsrGrpInfo();
//                 byNum = pcUsrGrpsInfo->GetGrpNum();
//                 *ptInfo = pcUsrGrpsInfo->m_atInfo;                
//             }
//             else
//             {
//                 OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] GetInstance(%d) failed.\n", byInsId);
//             }
//         }                
//         return ;
//     }

    // ��ͨģʽ
    byNum = (u8)m_cVCSUsrGrpInfo.GetGrpNum();

    *ptInfo = m_cVCSUsrGrpInfo.m_atInfo;

}
/*=============================================================================
�� �� ���� GetUserGroupInfo
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� CUsrGrpsInfo * 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/23  4.0			������                  ����
=============================================================================*/
CUsrGrpsInfo * CMcuVcData::GetUserGroupInfo( void )
{
    //��ΪN+1���ݹ���ģʽ�����л��󣩣�ȡ�ڴ��е���Ϣ
    if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
    {
        u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
        if (0 == byInsId || byInsId > MAXNUM_NPLUS_MCU)
        {
            OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] McuSwitchedInsId is invalid in NPlus switched mode.\n");
            return NULL;
        }
        else
        {
            CApp *pcApp = &g_cNPlusApp;
            CNPlusInst *pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL != pcInst)
            {
                return pcInst->GetUsrGrpInfo();
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] GetInstance(%d) failed.\n", byInsId);
                return NULL;
            }
        }                
    }

    // ��ͨģʽ
    return &m_cUsrGrpInfo;
}


/*=============================================================================
  �� �� ���� IsMtIpInAllowSeg
  ��    �ܣ� ָ��IP�Ƿ���ָ���û��������Χ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
=============================================================================*/
BOOL32 CMcuVcData::IsMtIpInAllowSeg ( u8 byGrpId, u32 dwIp )
{
    TUsrGrpInfo tGroup;
    
    if (byGrpId == USRGRPID_SADMIN)
    {
        // guzh [7/26/2006] ����ǳ����û��飬ֱ����Ϊ������
        return FALSE;
    }
    if (byGrpId == USRGRPID_INVALID)
    {
        return TRUE;
    }

    if ( !m_cUsrGrpInfo.GetGrpById(byGrpId, tGroup) )
    {
        return FALSE;
    }
    if ( tGroup.IsFree() )
    {
        // �û����
        return FALSE;
    }

    return tGroup.IsMtIpInAllowSeg(dwIp);
}

/*=============================================================================
  �� �� ���� PrtUserGroup
  ��    �ܣ� ���Դ�ӡ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/06/23    4.0         ����        ����
=============================================================================*/
void CMcuVcData::PrtUserGroup()
{
    m_cUsrGrpInfo.Print();
}

/*=============================================================================
  �� �� ���� AddUserTaskInfo
  ��    �ܣ� ����û������������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� FALSE:�û��Ѵ���/�û�������
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/11/24    4.5         ���㻪        ����
=============================================================================*/
BOOL  CUsersTaskInfo::AddUserTaskInfo(TUserTaskInfo& tUserTaskInfo)
{
	u16 wSpacePos = MAXNUM_VCSUSERNUM;
	for (u16 wIndex = 0; wIndex < MAXNUM_VCSUSERNUM; wIndex++)
	{
		if (0 == strcmp(tUserTaskInfo.GetUserName(), m_tUserTaskInfo[wIndex].GetUserName()))
		{
			// �û��Ѵ���
			return FALSE;
		}
		// �ҵ����е�λ�÷��µ��û���Ϣ����С�����
		if (!m_abyUsed[wIndex] && wSpacePos == MAXNUM_VCSUSERNUM)
		{
			wSpacePos = wIndex;
		}
	}

	if (MAXNUM_VCSUSERNUM == wSpacePos)
	{
		return FALSE;
	}
	else
	{
		m_abyUsed[wSpacePos] = 1;
		memcpy(&m_tUserTaskInfo[wSpacePos], &tUserTaskInfo, sizeof(TUserTaskInfo));
		SaveUsersTaskInfo();
		return TRUE;
	}
}

/*=============================================================================
  �� �� ���� DelUserTaskInfo
  ��    �ܣ� ɾ���û������������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� FALSE:ָ��ɾ�����û�������
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/11/24    4.5         ���㻪        ����
=============================================================================*/
BOOL  CUsersTaskInfo::DelUserTaskInfo(s8* pachUserName)
{
	u16 wUserPos = MAXNUM_VCSUSERNUM;
	for (u16 wIndex = 0; wIndex < MAXNUM_VCSUSERNUM; wIndex++)
	{
		if (0 == strcmp(pachUserName, m_tUserTaskInfo[wIndex].GetUserName()))
		{
			wUserPos = wIndex;
			break;
		}
	}

	if (MAXNUM_VCSUSERNUM == wUserPos)
	{
		return FALSE;
	}
	else
	{
		m_abyUsed[wUserPos] = 0;
		memset(&m_tUserTaskInfo[wUserPos], 0, sizeof(TUserTaskInfo));
		SaveUsersTaskInfo();
		return TRUE;
	}
}
/*=============================================================================
  �� �� ���� DelSpecTaskInfo
  ��    �ܣ� �������û��е�ָ������ɾ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� [IN]      cConfId����ɾ����������Ϣ
             [IN/OUT]pdwChgIdx����ɾ�����û�����
			 [IN/OUT] dwChgNum����ɾ�����û���
  �� �� ֵ�� 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/11/24    4.5         ���㻪        ����
=============================================================================*/
void  CUsersTaskInfo::DelSpecTaskInfo(CConfId cConfId, u16* const pdwChgIdx, u16& dwChgNum)
{
	u16* pdwTemp = pdwChgIdx;
	dwChgNum = 0;
	for (u16 dwIdx = 0; dwIdx < MAXNUM_VCSUSERNUM; dwIdx++)
	{
		if (m_abyUsed[dwIdx] && m_tUserTaskInfo[dwIdx].IsYourTask(cConfId, TRUE) && pdwChgIdx != NULL)
		{
			*pdwTemp++ = dwIdx;
			dwChgNum++;				
		}
	}
	SaveUsersTaskInfo();
}

/*=============================================================================
  �� �� ���� ChgUserTaskInfo
  ��    �ܣ� �޸��û������������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� FALSE:ָ���޸ĵ��û�������
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/11/24    4.5         ���㻪        ����
=============================================================================*/
BOOL  CUsersTaskInfo::ChgUserTaskInfo(TUserTaskInfo& tUserTaskInfo)
{
	u16 wUserPos = MAXNUM_VCSUSERNUM;
	for (u16 wIndex = 0; wIndex < MAXNUM_VCSUSERNUM; wIndex++)
	{
		if (0 == strcmp(tUserTaskInfo.GetUserName(), m_tUserTaskInfo[wIndex].GetUserName()))
		{
			wUserPos = wIndex;
			break;
		}
	}

	if (MAXNUM_VCSUSERNUM == wUserPos)
	{
		return FALSE;
	}
	else
	{
		memcpy(&m_tUserTaskInfo[wUserPos], &tUserTaskInfo, sizeof(TUserTaskInfo));
		SaveUsersTaskInfo();
		return TRUE;
	}
}
/*=============================================================================
  �� �� ���� GetSpecUserTaskInfo
  ��    �ܣ� �����û�����ȡָ���û���������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� FALSE:ָ�����û�������
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/11/24    4.5         ���㻪        ����
=============================================================================*/
BOOL  CUsersTaskInfo::GetSpecUserTaskInfo(const s8* pachUserName, TUserTaskInfo& tUserTaskInfo) 
{
	u16 wUserPos = MAXNUM_VCSUSERNUM;
	for (u16 wIndex = 0; wIndex < MAXNUM_VCSUSERNUM; wIndex++)
	{
		if (0 == strcmp(m_tUserTaskInfo[wIndex].GetUserName(), pachUserName))
		{
			wUserPos = wIndex;
			break;
		}
	}

	if (MAXNUM_VCSUSERNUM == wUserPos)
	{
		return FALSE;
	}
	else
	{
		tUserTaskInfo = m_tUserTaskInfo[wUserPos];
		return TRUE;
	}
}
/*=============================================================================
  �� �� ���� GetSpecUserTaskInfo
  ��    �ܣ� ����������ȡָ���û���������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� FALSE:ָ�����û�������
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/11/24    4.5         ���㻪        ����
=============================================================================*/
BOOL  CUsersTaskInfo::GetSpecUserTaskInfo(u16 dwIdx, TUserTaskInfo& tUserTaskInfo)
{
	if (m_abyUsed[dwIdx])
	{
		tUserTaskInfo = m_tUserTaskInfo[dwIdx];
		return TRUE;
	}
	return FALSE;
}

/*=============================================================================
  �� �� ���� IsYourTask
  ��    �ܣ� �жϸ��û��Ƿ��в��������Ȩ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� [IN]pachUserName: �û���
             [IN]cConfId:      ����ID
  �� �� ֵ�� FALSE:ָ�����û����ܲ����û���
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/11/24    4.5         ���㻪        ����
=============================================================================*/
BOOL  CUsersTaskInfo::IsYourTask(const s8* pachUserName, const CConfId& cConfId)
{
	TUserTaskInfo tTaskInfo;
	if(GetSpecUserTaskInfo(pachUserName, tTaskInfo))
	{
		const CConfId* pcConfId = tTaskInfo.GetUserTaskInfo();
		for (u8 byTaskIndex = 0; byTaskIndex < tTaskInfo.GetUserTaskNum(); byTaskIndex++)
		{
			if (cConfId == *pcConfId)
			{
				return TRUE;
			}
			pcConfId++;
		}
	}
	return FALSE;
}

/*=============================================================================
  �� �� ���� SaveUsersTaskInfo
  ��    �ܣ� ���������û���������Ϣ���ļ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/11/24    4.5         ���㻪        ����
=============================================================================*/
BOOL  CUsersTaskInfo::SaveUsersTaskInfo() const
{
   s8 szPath[KDV_MAX_PATH] = {0};
    
    sprintf(szPath, "%s/%s", DIR_DATA, USERTASKINFOFILENAME);

    FILE *fp;
    fp = fopen( szPath, "wb" );
    if (fp == NULL)
    {
        return FALSE;
    }

	fwrite(m_abyUsed, sizeof(u8), MAXNUM_VCSUSERNUM, fp);
	fwrite(m_tUserTaskInfo, sizeof(TUserTaskInfo), MAXNUM_VCSUSERNUM, fp);

#ifdef WIN32
    fflush(fp);
#endif

    fclose(fp);

    return TRUE;    
}
/*=============================================================================
  �� �� ���� LoadUsersTaskInfo
  ��    �ܣ� ���������û���������Ϣ���ļ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/11/24    4.5         ���㻪        ����
=============================================================================*/
BOOL  CUsersTaskInfo::LoadUsersTaskInfo()
{
   s8 szPath[KDV_MAX_PATH] = {0};
    
    sprintf(szPath, "%s/%s", DIR_DATA, USERTASKINFOFILENAME);

    FILE *fp;
    fp = fopen( szPath, "rb" );
    if (fp == NULL)
    {
        return FALSE;
    }

 	fread(m_abyUsed, sizeof(u8), MAXNUM_VCSUSERNUM, fp);
	fread(m_tUserTaskInfo, sizeof(TUserTaskInfo), MAXNUM_VCSUSERNUM, fp);

    fclose(fp);
    return TRUE;    
}

/*=============================================================================
  �� �� ���� AddVCSUserTaskInfo
  ��    �ܣ� ���VCS�û������������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� FALSE:�û��Ѵ���/�û�������
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/11/24    4.5         ���㻪        ����
=============================================================================*/
BOOL  CMcuVcData::AddVCSUserTaskInfo(TUserTaskInfo& tUserTaskInfo)
{
	return m_cVCSUsrTaskInfo.AddUserTaskInfo(tUserTaskInfo);
}

/*=============================================================================
  �� �� ���� DelVCSUserTaskInfo
  ��    �ܣ� ɾ��VCS�û������������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� FALSE:�û��Ѵ���/�û�������
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/11/24    4.5         ���㻪        ����
=============================================================================*/
BOOL  CMcuVcData::DelVCSUserTaskInfo(s8* pachUserName)
{
	return m_cVCSUsrTaskInfo.DelUserTaskInfo(pachUserName);
}
/*=============================================================================
  �� �� ���� DelVCSSpecTaskInfo
  ��    �ܣ� �������û���ɾ����ָ�����񣬲�������Ϣͨ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� [IN]      cConfId����ɾ����������Ϣ
  �� �� ֵ�� FALSE:�û��Ѵ���/�û�������
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/11/24    4.5         ���㻪        ����
=============================================================================*/
void  CMcuVcData::DelVCSSpecTaskInfo(CConfId cConfId)
{
	u16 dwChgIdx[MAXNUM_VCSUSERNUM];
	u16 *pdwChgIdx = dwChgIdx;
	memset(dwChgIdx, 0, sizeof(dwChgIdx));
	u16  dwChgNum = 0;


	m_cVCSUsrTaskInfo.DelSpecTaskInfo(cConfId, pdwChgIdx, dwChgNum);
	for (u16 dwIdx = 0; dwIdx <	dwChgNum; dwIdx++)
	{
		TUserTaskInfo tTaskInfo;
		CServMsg      cMsg;
		if(m_cVCSUsrTaskInfo.GetSpecUserTaskInfo(*pdwChgIdx++, tTaskInfo))
		{
			s32 nUsrNum = g_cVCSUsrManage.GetUserNum();
			CVCSUsrInfo cUsrInfo;
			for (s32 nUsrLoop = 0; nUsrLoop < nUsrNum; nUsrLoop ++)
			{
				g_cVCSUsrManage.GetUserFullInfo( &cUsrInfo, nUsrLoop );
				if ( strcmp(cUsrInfo.GetName(), tTaskInfo.GetUserName()))
				{
					cUsrInfo.AssignTask(tTaskInfo.GetUserTaskNum(), tTaskInfo.GetUserTaskInfo());

					cMsg.SetMcuId(LOCAL_MCUID);
					cMsg.SetSrcMtId(0);
					cMsg.SetSrcSsnId(0);
					cMsg.SetMsgBody((u8*)&cUsrInfo, sizeof(CVCSUsrInfo));
					g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn(cUsrInfo.GetUsrGrpId(), MCU_MCS_CHANGEUSER_NOTIF,
						                             cMsg.GetServMsg(), cMsg.GetServMsgLen());
					break;
				}
			}
			

		}


	}
}

/*=============================================================================
  �� �� ���� ChgVCSUserTaskInfo
  ��    �ܣ� �޸�VCS�û������������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� FALSE:�û��Ѵ���/�û�������
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/11/24    4.5         ���㻪        ����
=============================================================================*/
BOOL  CMcuVcData::ChgVCSUserTaskInfo(TUserTaskInfo& tUserTaskInfo)
{
	return m_cVCSUsrTaskInfo.ChgUserTaskInfo(tUserTaskInfo);
}
/*=============================================================================
  �� �� ���� GetSpecVCSUserTaskInfo
  ��    �ܣ� ��ȡָ���û���������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� [IN]pachUserName:��ѯ���û���
             [OUT]tUserTaskInfo:���ո��û���������Ϣ
  �� �� ֵ�� �Ƿ�Ϊ���û�ָ������
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/11/24    4.5         ���㻪        ����
=============================================================================*/
BOOL  CMcuVcData::GetSpecVCSUserTaskInfo(s8* pachUserName, TUserTaskInfo& tUserTaskInfo)
{
	return m_cVCSUsrTaskInfo.GetSpecUserTaskInfo(pachUserName, tUserTaskInfo);
}
/*=============================================================================
  �� �� ���� LoadVCSUsersTaskInfo
  ��    �ܣ� �ļ��ж�ȡ�����û���������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� �Ƿ�Ϊ���û�ָ������
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/11/24    4.5         ���㻪        ����
=============================================================================*/
BOOL  CMcuVcData::LoadVCSUsersTaskInfo()
{
	return m_cVCSUsrTaskInfo.LoadUsersTaskInfo();
}
/*=============================================================================
  �� �� ���� IsYourTask
  ��    �ܣ� �ж�ָ�������Ƿ�Ϊָ���û�������Χ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� �Ƿ�Ϊ���û�ָ��������
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  08/12/15    4.5         ���㻪        ����
=============================================================================*/
BOOL  CMcuVcData::IsYourTask(const s8* pachUserName, const CConfId& cConfId)
{
	return m_cVCSUsrTaskInfo.IsYourTask(pachUserName, cConfId);
}

/********************************************

        ����������Դ����ʵ��

  *******************************************/

/*=============================================================================
  �� �� ���� CBasMgr
  ��    �ܣ� ��������� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/09    4.5         �ű���        ����
=============================================================================*/
CBasMgr::CBasMgr()
{
    Clear();
}

/*=============================================================================
  �� �� ���� CBasMgr
  ��    �ܣ� ��������� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/09    4.5         �ű���        ����
=============================================================================*/
CBasMgr::~CBasMgr()
{
    Clear();
}

/*=============================================================================
  �� �� ���� GetNeededMau
  ��    �ܣ� ��������� ��ȡĳ��������Ҫ��bas
  �㷨ʵ�֣� ���Ծ�̬˫��
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/09    4.5         �ű���        ����
=============================================================================*/
void CBasMgr::GetNeededMau(const TConfInfo &tConfInfo, u8 &byNVChn, u8 &byH263Chn, u8 &byVGAChn)
{
    byNVChn = 0;
    byH263Chn = 0;
    byVGAChn = 0;
    
    //zbq[03/24/2009] ˫��֧�ַ�H264����
    SetDualDouble(IsConfDoubleDual(tConfInfo));
    
    //VGA˫�����ԣ�ֻȡh263plus(˫������󶨳���)
    if (IsConfDoubleDual(tConfInfo))
    {
        byH263Chn ++ ;
    }

    if (MEDIA_TYPE_H264 != tConfInfo.GetMainVideoMediaType())
    {
        return;
    }

    u8 byHDDSNeed = 0;    //˫����������ͨ����������
    u8 byHDNeeded = 0;    //��������Ⱥ���������
    u8 bySDNeeded = 0;    //��������Ⱥ���������

    //˫��
    if (tConfInfo.GetSecBitRate() != 0 &&
        VIDEO_FORMAT_CIF != tConfInfo.GetMainVideoFormat())
    {
        //zbq[05/22/2009] �������䲻Ϊ��Ԥ��
        if (!g_cMcuVcApp.IsAdpResourceCompact())
        {
            byHDNeeded ++;
        }
    }

    //˫��ʽ
    u8 bySecVidType = tConfInfo.GetSecVideoMediaType();
    if (MEDIA_TYPE_MP4 == bySecVidType ||
        MEDIA_TYPE_H263 == bySecVidType )
    {
        bySDNeeded ++;
    }
    else if (MEDIA_TYPE_NULL != bySecVidType)
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::GetNeededMau] unexpected sec type.%d\n", bySecVidType);
    }

    //ͬ����Ƶ˫��
    SetDualEqMv(IsConfDualEqMV(tConfInfo));
    SetDualVGA(!IsDualEqMv() && !IsDualDouble());

    if (IsDualEqMv() &&
        VIDEO_FORMAT_CIF != tConfInfo.GetMainVideoFormat()
        /*&&
        //zbq[04/02/2009] ͬ����Ƶ��˫����ҪRes֧��cif��4cif��720p�����ͬ����Ƶ������Ҫ���䣻1080pû�У�
        (tConfInfo.GetConfAttrbEx().IsResEx4Cif() || tConfInfo.GetConfAttrbEx().IsResExCif())*/)
    {
        byHDDSNeed ++;
    }

    //�û�ָ��
    if (tConfInfo.GetConfAttrbEx().IsResEx1080())
    {
        byHDNeeded ++;
    }
    if (tConfInfo.GetConfAttrbEx().IsResEx4Cif())
    {
        byHDNeeded ++;
    }
    if (tConfInfo.GetConfAttrbEx().IsResEx720())
    {
        byHDNeeded ++;
    }
    if (tConfInfo.GetConfAttrbEx().IsResExCif())
    {
        bySDNeeded ++;
    }

    byNVChn = max(byHDNeeded, bySDNeeded) + byHDDSNeed;

    //������֧�����䣬˫����Ԥ��������Դ
    if (!IsDualEqMv() && byNVChn >= 1 &&
        (tConfInfo.GetDStreamUsrDefFPS() > 5 || IsResG(tConfInfo.GetDoubleVideoFormat(), VIDEO_FORMAT_XGA)))
    {
        byVGAChn = 1;
    }

    return;
}

/*=============================================================================
  �� �� ���� GetNeededMpu
  ��    �ܣ� ��������� ��ȡĳ��������Ҫ��bas
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/05/19    4.5         �ű���        ����
=============================================================================*/
void CBasMgr::GetNeededMpu(const TConfInfo &tConfInfo, u8 &byNVChn, u8 &byDSChn)
{
    byNVChn = 0;
    byDSChn = 0;

    u8 byH263Chn = 0;
    u8 byVGAChn = 0;
    
    GetNeededMau(tConfInfo, byNVChn, byH263Chn, byVGAChn);

    //ͬ����Ƶ ˫��
    if (IsConfDualEqMV(tConfInfo) &&
        (VIDEO_FORMAT_CIF != tConfInfo.GetMainVideoFormat() &&
         MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType()))
    {
        byNVChn = byNVChn - 1;
        byDSChn = 1;
    }
    else
    {
        byDSChn = max(byH263Chn, byVGAChn);
        byDSChn = min(byDSChn, 1);
    }
    return;
}

/*=============================================================================
  �� �� ���� IsMauSufficient
  ��    �ܣ� ��������� ��ǰMCU���е�MAU��Դ�Ƿ�������û����ģ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/09    4.5         �ű���        ����
=============================================================================*/
BOOL32 CBasMgr::IsMauSufficient(const TConfInfo &tConfInfo)
{
    u8 byNVIdle = 0;
    u8 byH263Idle = 0;
    u8 byVGAIdle = 0;
    g_cMcuVcApp.GetIdleMau(byNVIdle, byVGAIdle, byH263Idle);

    u8 byNVNeeded = 0;
    u8 byH263Needed = 0;
    u8 byVGANeeded = 0;
    GetNeededMau(tConfInfo, byNVNeeded, byH263Needed, byVGANeeded);

    if (byNVIdle >= byNVNeeded && byH263Idle >= byH263Needed && byVGAIdle >= byVGANeeded)
    {
        return TRUE;
    }
    OspPrintf(TRUE, FALSE, "[IsMauSufficient] Idle<nv.%d,h263p.%d,vga.%d>, need<%d,%d,%d>\n",
                            byNVIdle, byH263Idle, byVGAIdle, byNVNeeded, byH263Needed, byVGANeeded);
    
    return FALSE;
}

/*=============================================================================
  �� �� ���� IsHdBasSufficient
  ��    �ܣ� ��������� ��ǰMCU���е�BAS��Դ�Ƿ�������û����ģ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/09    4.5         �ű���        ����
=============================================================================*/
BOOL32 CBasMgr::IsHdBasSufficient(const TConfInfo &tConfInfo)
{
    return IsMpuSufficient(tConfInfo) || IsMauSufficient(tConfInfo) ;
}

/*=============================================================================
  �� �� ���� OcuppyHdBasChn
  ��    �ܣ� ���������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/09    4.5         �ű���        ����
=============================================================================*/
BOOL32 CBasMgr::OcuppyHdBasChn(const TConfInfo &tConfInfo)
{
    if (IsMpuSufficient(tConfInfo))
    {
        return OcuppyMpu(tConfInfo);
    }
    else if (IsMauSufficient(tConfInfo))
    {
        return OcuppyMau(tConfInfo);
    }
    return FALSE;
}

/*=============================================================================
  �� �� ���� IsMpuSufficient
  ��    �ܣ� ��������� ��ǰMCU���е�Mpu��Դ�Ƿ�������û����ģ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/05/19    4.6         �ű���        ����
=============================================================================*/
BOOL32 CBasMgr::IsMpuSufficient(const TConfInfo &tConfInfo)
{
    u8 byChnIdle = 0;
    g_cMcuVcApp.GetIdleMpu(byChnIdle);

    u8 byNVChn = 0;
    u8 byDSChn = 0;
    GetNeededMpu(tConfInfo, byNVChn, byDSChn);

    if (byChnIdle >= byNVChn + byDSChn)
    {
        return TRUE;
    }
    OspPrintf(TRUE, FALSE, "[IsMpuSufficient] Idle<nv.%d>, need<NV.%d, DS.%d>\n", byChnIdle, byNVChn, byDSChn);

    return FALSE;
}


/*=============================================================================
  �� �� ���� GetHdBasStatus
  ��    �ܣ� ��������� ��ȡ����mcu�Ѿ����õ�bas
  �㷨ʵ�֣� ���Ծ�̬˫��ͨ��. ���ӿڹ��ϱ�MCS��.
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/09    4.5         �ű���        ����
=============================================================================*/
void CBasMgr::GetHdBasStatus(TMcuHdBasStatus &tHdBasStatus, TConfInfo &tCurConf)
{
    tHdBasStatus.Clear();

    for(u8 byEqpId = BASID_MIN; byEqpId <= BASID_MAX; byEqpId++)
    {
        TPeriEqpStatus tStatus;
        if (!g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus))
        {
            continue;
        }
        //hd only
        if (!g_cMcuAgent.IsEqpBasHD(byEqpId) ||
            !g_cMcuVcApp.IsPeriEqpConnected(byEqpId))
        {
            continue;
        }

        u8 byConfIdx = tStatus.GetConfIdx();
        u8 byEqpType = tStatus.m_tStatus.tHdBas.GetEqpType();
        
        //get ip
        u32 dwEqpIp = 0;
        u8  byType = 0;
        g_cMcuAgent.GetPeriInfo(byEqpId, &dwEqpIp, &byType);

        u8 byChnIdx = 0;

        switch (byEqpType)
        {
        case TYPE_MAU_NORMAL:
        case TYPE_MAU_H263PLUS:
            {
                //ignore vga
                byChnIdx = 0;
                BOOL32 bReserved = tStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(byChnIdx)->IsReserved();
                BOOL32 bH263p = tStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MAU_H263PLUS ? TRUE : FALSE;
                
                if (bReserved)
                {
                    CConfId cConfId = g_cMcuVcApp.GetConfId(byConfIdx);
                    tHdBasStatus.SetConfMau(cConfId, dwEqpIp, bH263p);
                }
                else
                {
                    tHdBasStatus.SetIdleMau(dwEqpIp, bH263p);
                }
            }
            break;

        case TYPE_MPU:
            for (byChnIdx = 0; byChnIdx < MAXNUM_MPU_CHN; byChnIdx++)
            {
                BOOL32 bReserved = tStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->IsReserved();
                
                if (bReserved)
                {
                    CConfId cConfId = g_cMcuVcApp.GetConfId(byConfIdx);
                    tHdBasStatus.SetConfMpuChn(cConfId, dwEqpIp, byChnIdx);
                }
                else
                {
                    tHdBasStatus.SetIdleMpuChn(dwEqpIp, byChnIdx);
                }
            }
            break;

        default:
            break;
        }
    }

    u8 byNormalMau = 0;
    u8 byH263pMau = 0;
    u8 byVGAMau = 0;
    GetNeededMau(tCurConf, byNormalMau, byH263pMau, byVGAMau);
    tHdBasStatus.SetCurMauNeeded(byNormalMau, byH263pMau);

    u8 byNVChn = 0;
    u8 byDSChn = 0;
    GetNeededMpu(tCurConf, byNVChn, byDSChn);
    tHdBasStatus.SetCurMpuChnNeeded(byNVChn + byDSChn);

    return;
}


/*=============================================================================
  �� �� ���� OcuppyMau
  ��    �ܣ� ��������� ռ�ñ�������Ҫ��bas����VGA��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/09    4.5         �ű���        ����
=============================================================================*/
BOOL32 CBasMgr::OcuppyMau(const TConfInfo &tConfInfo)
{
    u8 byNVIdle = 0;
    u8 byH263Idle = 0;
    u8 byVGAIdle = 0;   //Ŀǰ��δ����

    g_cMcuVcApp.GetIdleMau(byNVIdle, byVGAIdle, byH263Idle);
    
    u8 byNVNeeded = 0;
    u8 byH263Needed = 0;
    u8 byVGANeeded = 0;

    GetNeededMau(tConfInfo, byNVNeeded, byH263Needed, byVGANeeded);
    
    if (byNVIdle < byNVNeeded || byH263Idle < byH263Needed || byVGAIdle < byVGANeeded)
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::OcuppyMau] failed due to idle<%d,%d,%d>, needed<%d,%d,%d>\n",
                                byNVIdle, byH263Idle, byVGAIdle, byNVNeeded, byH263Needed, byVGANeeded);
        return FALSE;
    }
    
    BOOL32 bH263Needed = byH263Needed != 0 ? TRUE : FALSE;
    BOOL32 bDualEqMV = IsConfDualEqMV(tConfInfo);

    //����ռ��
    u8 byEqpId = 0;
    u8 byChnId = 0;

    TEqp tEqp;
    tEqp.SetNull();

    while (byH263Needed > 0)
    {
        if (g_cMcuVcApp.GetIdleMauChn(MAU_CHN_263TO264, byEqpId, byChnId))
        {
            tEqp = g_cMcuVcApp.GetEqp(byEqpId);
            AddBrdChn(tEqp, byChnId, MAU_CHN_263TO264, TRUE);
        }
        else
        {
            break;
        }
        if (g_cMcuVcApp.GetIdleMauChn(MAU_CHN_264TO263, byEqpId, byChnId))
        {
            tEqp = g_cMcuVcApp.GetEqp(byEqpId);
            AddBrdChn(tEqp, byChnId, MAU_CHN_264TO263, TRUE);
        }
        else
        {
            break;
        }
        byH263Needed--;
    }
    
    while (byNVNeeded > 0)
    {
        tEqp.SetNull();
        byEqpId = 0;
        byChnId = 0;

        if (bDualEqMV)
        {
            if (g_cMcuVcApp.GetIdleMauChn(MAU_CHN_NORMAL, byEqpId, byChnId))
            {
                tEqp = g_cMcuVcApp.GetEqp(byEqpId);
                AddBrdChn(tEqp, byChnId, MAU_CHN_NORMAL, TRUE);
            }
            else
            {
                break;
            }
            bDualEqMV = FALSE;
        }
        else
        {
            if (g_cMcuVcApp.GetIdleMauChn(MAU_CHN_NORMAL, byEqpId, byChnId))
            {
                tEqp = g_cMcuVcApp.GetEqp(byEqpId);
                AddBrdChn(tEqp, byChnId, MAU_CHN_NORMAL);
            }
            else
            {
                break;
            }
        }

        byNVNeeded--;
    }

    while(byVGANeeded > 0)
    {
        tEqp.SetNull();
        byEqpId = 0;
        byChnId = 0;

        if (g_cMcuVcApp.GetIdleMauChn(MAU_CHN_VGA, byEqpId, byChnId))
        {
            tEqp = g_cMcuVcApp.GetEqp(byEqpId);
            AddBrdChn(tEqp, byChnId, MAU_CHN_VGA, TRUE);
        }
        else
        {
            break;
        }
        byVGANeeded --;
    }

    if (byH263Needed != 0 || byNVNeeded != 0 || byVGANeeded != 0)
    {
        ReleaseHdBasChn();
        OspPrintf(TRUE, FALSE, "[CBasMgr::OcuppyMau] failed as h263.%d, nv.%d, vga.%d\n", 
            byH263Needed, byNVNeeded, byNVNeeded);
    }
    else
    {
        AssignBasChn(tConfInfo, MODE_VIDEO);
        AssignBasChn(tConfInfo, MODE_SECVIDEO);
    }

    return byH263Needed == 0 && byNVNeeded == 0 && byVGANeeded == 0;

}

/*=============================================================================
  �� �� ���� OcuppyMpu
  ��    �ܣ� ��������� ռ�ñ�������Ҫ��mpu-bas����VGA��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/05/19    4.6         �ű���        ����
=============================================================================*/
BOOL32 CBasMgr::OcuppyMpu(const TConfInfo &tConfInfo)
{
    u8 byChnIdle = 0;    
    g_cMcuVcApp.GetIdleMpu(byChnIdle);
    
    u8 byNVChn = 0;
    u8 byDSChn = 0;
    GetNeededMpu(tConfInfo, byNVChn, byDSChn);
    
    if (byChnIdle < byNVChn + byDSChn)
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::OcuppyMpu] failed due to idle<%d>, needed<%d,%d>\n", byChnIdle, byNVChn, byDSChn);
        return FALSE;
    }

    //����ռ��
    u8 byEqpId = 0;
    u8 byChnId = 0;
    
    TEqp tEqp;
    tEqp.SetNull();
    
    while (byNVChn > 0)
    {
        if (g_cMcuVcApp.GetIdleMpuChn(byEqpId, byChnId))
        {
            tEqp = g_cMcuVcApp.GetEqp(byEqpId);			
            AddBrdChn(tEqp, byChnId, MPU_CHN_NORMAL);
        }
        else
        {
            break;
        }
        byNVChn--;
    }

    while (byDSChn > 0)
    {
        if (g_cMcuVcApp.GetIdleMpuChn(byEqpId, byChnId))
        {
            tEqp = g_cMcuVcApp.GetEqp(byEqpId);			
            AddBrdChn(tEqp, byChnId, MPU_CHN_NORMAL, TRUE);
        }
        else
        {
            break;
        }
        byDSChn--;
    }

    if (byDSChn != 0 || byNVChn != 0)
    {
        ReleaseHdBasChn();
        OspPrintf(TRUE, FALSE, "[CBasMgr::OcuppyMpu] failed lack of NV.%d, DS.%d!\n", byNVChn, byDSChn);
    }
    else
    {
        AssignBasChn(tConfInfo, MODE_VIDEO);
        AssignBasChn(tConfInfo, MODE_SECVIDEO);
    }
    
    return byNVChn == 0 && byDSChn == 0;
}

/*=============================================================================
  �� �� ���� AssignBasChn
  ��    �ܣ� ��������� ���ݻ����������ռ�õ�basͨ�����乤���������������������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/09    4.5         �ű���          ����
    09/04/19    4.6         �ű���          ��д������mpu
=============================================================================*/
void CBasMgr::AssignBasChn(const TConfInfo &tConfInfo, u8 byMode)
{
    if (MODE_VIDEO == byMode)
    {
        AssignMVBasChn(tConfInfo);
    }
    else if (MODE_SECVIDEO == byMode)
    {
        AssignDSBasChn(tConfInfo);
    }
    return;
}

/*=============================================================================
  �� �� ���� AssignBasChn
  ��    �ܣ� ��������� �����������ͨ��������ʼ����
  �㷨ʵ�֣� 1������basͨ�����γ��ԣ������������䡢����ֱ������䣨�Ӵ�С����H263���䣻
             2�����������֡�ʺ�������Ҫ��������΢����
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  
    Fixme: �������⣺
        1���������������������ͣ˫��ʱ���б䴦�����ʣ�
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/04/19    4.6         �ű���          ��д������mpu
=============================================================================*/
void CBasMgr::AssignMVBasChn(const TConfInfo &tConfInfo)
{
    u8 byMVChnIdx = 0;

    THDBasVidChnStatus tStatus;
    memset(&tStatus, 0, sizeof(tStatus));

    BOOL32 bBRAdpIgnored = FALSE;

    //1��˫��
    if (0 != tConfInfo.GetSecBitRate())
    {
        //zbq[05/23/2009]��Ƶ��Դ����ʹ��ʱ���Դ���������
        if (g_cMcuVcApp.IsAdpResourceCompact())
        {
            bBRAdpIgnored = TRUE;
        }
        else
        {
	        THDAdaptParam tHDAdpParam;
	        memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));
	        
	        u8 byMediaType = tConfInfo.GetMainVideoMediaType();
	        tHDAdpParam.SetVidType(byMediaType);
	        tHDAdpParam.SetBitRate(tConfInfo.GetSecBitRate());
	        tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, byMediaType));
	        
	        u16 wWidth = 0;
	        u16 wHeight = 0;
	        GetWHByRes(tConfInfo.GetMainVideoFormat(), wWidth, wHeight);
	        tHDAdpParam.SetResolution(wWidth, wHeight);
	        
	        tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
	        tHDAdpParam.SetFrameRate(tConfInfo.GetMainVidUsrDefFPS());


	        m_tMVBrdChn.GetChn(byMVChnIdx, tStatus);
	        if (!tStatus.IsNull())
	        {
	            tStatus.SetOutputVidParam(tHDAdpParam, 0);
	            m_tMVBrdChn.UpdateChn(byMVChnIdx, tStatus);
	        }
	        else
	        {
	            OspPrintf(TRUE, FALSE, "[CBasMgr::AssignMVBasChn] MVChnIdx.%d is not ready \
yet, unexpected(DBR)\n", byMVChnIdx);
	        }

	        byMVChnIdx ++;
        }
    }

    //2��½���ķֱ�������(1080������)
    if (tConfInfo.GetConfAttrbEx().IsResEx720())
    {
        THDAdaptParam tHDAdpParam;
        memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));

        u8 byMediaType = tConfInfo.GetMainVideoMediaType();
        tHDAdpParam.SetVidType(byMediaType);
        
        //zbq[05/23/2009] ������Դ���պ󣬵ͷֱ���ͳͳ���٣����佨��ʱ����΢��
        if (bBRAdpIgnored)
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetSecBitRate());
        }
        else
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate());
        }
        
        tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, byMediaType));

        u16 wWidth = 0;
        u16 wHeight = 0;
        GetWHByRes(VIDEO_FORMAT_HD720, wWidth, wHeight);
        tHDAdpParam.SetResolution(wWidth, wHeight);

        tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
        tHDAdpParam.SetFrameRate(tConfInfo.GetMainVidUsrDefFPS());

        memset(&tStatus, 0, sizeof(tStatus));
        m_tMVBrdChn.GetChn(byMVChnIdx, tStatus);
        if (!tStatus.IsNull())
        {
            tStatus.SetOutputVidParam(tHDAdpParam, 0);
            m_tMVBrdChn.UpdateChn(byMVChnIdx, tStatus);
        }
        else
        {
            OspPrintf(TRUE, FALSE, "[CBasMgr::AssignMVBasChn] MVChnIdx.%d is not ready \
yet, unexpected(720)\n", byMVChnIdx);
        }

        byMVChnIdx ++;
    }
    if (tConfInfo.GetConfAttrbEx().IsResEx4Cif())
    {
        THDAdaptParam tHDAdpParam;
        memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));

        u8 byMediaType = tConfInfo.GetMainVideoMediaType();
        tHDAdpParam.SetVidType(byMediaType);
        
        //zbq[05/23/2009] ������Դ���պ󣬵ͷֱ���ͳͳ���٣����佨��ʱ����΢��
        if (bBRAdpIgnored)
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetSecBitRate());
        }
        else
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate());
        }

        tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, byMediaType));
        
        u16 wWidth = 0;
        u16 wHeight = 0;
        GetWHByRes(VIDEO_FORMAT_4CIF, wWidth, wHeight);
        tHDAdpParam.SetResolution(wWidth, wHeight);
        
        tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
        tHDAdpParam.SetFrameRate(tConfInfo.GetMainVidUsrDefFPS());
        
        memset(&tStatus, 0, sizeof(tStatus));
        m_tMVBrdChn.GetChn(byMVChnIdx, tStatus);
        if (!tStatus.IsNull())
        {
            tStatus.SetOutputVidParam(tHDAdpParam, 0);
            m_tMVBrdChn.UpdateChn(byMVChnIdx, tStatus);
        }
        else
        {
            OspPrintf(TRUE, FALSE, "[CBasMgr::AssignMVBasChn] MVChnIdx.%d is not ready \
yet, unexpected(4CIF)\n", byMVChnIdx);
        }

        byMVChnIdx ++;
    }

    //zbq[04/22/2009] ˫��ʽ������������գ�����������ʽ�ͷֱ��ʣ��Ը�ʽ������������֧��
    if (0 == byMVChnIdx)
    {
        if (0 != tConfInfo.GetSecVideoMediaType() &&
            MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType())
        {
            THDAdaptParam tHDAdpParam;
            memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));
            
            u8 byMediaType = tConfInfo.GetMainVideoMediaType();
            tHDAdpParam.SetVidType(byMediaType);
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate());
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, byMediaType));
            
            u16 wWidth = 0;
            u16 wHeight = 0;
            GetWHByRes(tConfInfo.GetMainVideoFormat(), wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
            tHDAdpParam.SetFrameRate(tConfInfo.GetMainVidUsrDefFPS());
            
            memset(&tStatus, 0, sizeof(tStatus));
            m_tMVBrdChn.GetChn(byMVChnIdx, tStatus);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 0);
                m_tMVBrdChn.UpdateChn(byMVChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignMVBasChn] MVChnIdx.%d is not ready \
yet, unexpected(MainVid)\n", byMVChnIdx);
            }
            
            byMVChnIdx ++;
        }
    }

    //3��Other��CIF��˳�ιҽ�
    byMVChnIdx = 0;

    // other
    if (MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType())
    {
        THDAdaptParam tHDAdpParam;
        memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));

        u8 byMediaType = tConfInfo.GetSecVideoMediaType();
        
        //zbq[05/23/2009] ������Դ���պ󣬵ͷֱ���ͳͳ���٣����佨��ʱ����΢��
        if (bBRAdpIgnored)
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetSecBitRate());
        }
        else
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate());
        }

        tHDAdpParam.SetVidType(byMediaType);
        tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, byMediaType));

        u16 wWidth = 0;
        u16 wHeight = 0;
        u8 bySecVidFormat = tConfInfo.GetSecVideoFormat();
        if (VIDEO_FORMAT_AUTO == bySecVidFormat)
        {
            u16 wSecBR = tConfInfo.GetSecBitRate();
            if (wSecBR > 2048)
            {
                bySecVidFormat = VIDEO_FORMAT_4CIF;
            }
            else
            {
                bySecVidFormat = VIDEO_FORMAT_CIF;
            }
        }
        GetWHByRes(bySecVidFormat, wWidth, wHeight);
        tHDAdpParam.SetResolution(wWidth, wHeight);
        
        tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
        tHDAdpParam.SetFrameRate(tConfInfo.GetSecVidFrameRate());
        
        memset(&tStatus, 0, sizeof(tStatus));
        m_tMVBrdChn.GetChn(byMVChnIdx, tStatus);
        if (!tStatus.IsNull())
        {
            tStatus.SetOutputVidParam(tHDAdpParam, 1);
            m_tMVBrdChn.UpdateChn(byMVChnIdx, tStatus);
        }
        else
        {
            OspPrintf(TRUE, FALSE, "[CBasMgr::AssignMVBasChn] MVChnIdx.%d is not ready \
yet, unexpected(other)\n", byMVChnIdx);
        }

        byMVChnIdx ++;
    }
    // cif
    if (tConfInfo.GetConfAttrbEx().IsResExCif())
    {
        THDAdaptParam tHDAdpParam;
        memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));

        u8 byMediaType = tConfInfo.GetMainVideoMediaType();
        
        //zbq[05/23/2009] ������Դ���պ󣬵ͷֱ���ͳͳ���٣����佨��ʱ����΢��
        if (bBRAdpIgnored)
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetSecBitRate());
        }
        else
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate());
        }

        tHDAdpParam.SetVidType(byMediaType);
        tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, byMediaType));
        
        u16 wWidth = 0;
        u16 wHeight = 0;
        GetWHByRes(VIDEO_FORMAT_CIF, wWidth, wHeight);
        tHDAdpParam.SetResolution(wWidth, wHeight);
        
        tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
        tHDAdpParam.SetFrameRate(tConfInfo.GetMainVidUsrDefFPS());

        memset(&tStatus, 0, sizeof(tStatus));
        m_tMVBrdChn.GetChn(byMVChnIdx, tStatus);
        if (!tStatus.IsNull())
        {
            tStatus.SetOutputVidParam(tHDAdpParam, 1);
            m_tMVBrdChn.UpdateChn(byMVChnIdx, tStatus);
        }
        else
        {
            OspPrintf(TRUE, FALSE, "[CBasMgr::AssignMVBasChn] MVChnIdx.%d is not ready \
yet, unexpected(cif)\n", byMVChnIdx);
        }

        byMVChnIdx ++;
    }
    return;
}

/*=============================================================================
  �� �� ���� AssignDSBasChn
  ��    �ܣ� ��������� ˫���㲥���� ��ʼ��������
  �㷨ʵ�֣� 1��˫��basͨ��ֱ�Ӹ���̬˫��������о�̬˫������
             2��˫�������֡�ʺ�����Ҳ��Ҫ��������΢����
             3��mau��˫��������Դ��ʱ��ƽ��ռ����ص�����ͨ����
             4��mpu��˫��������Դ��ʱ����������µ�˫������һ��mpuͨ����ʵʱ�л�����
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  
    Fixme: �������⣺
        1��˫��h263p[0]: 264����������ĵ������ֱ��ʣ�
        2��˫��h263p[1]: 263p����������ĵ������ֱ��ʣ�
        3��ͬ����Ƶ˫��: �ֱ���΢��
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/04/19    4.6         �ű���          ��д������mpu
=============================================================================*/
void CBasMgr::AssignDSBasChn(const TConfInfo &tConfInfo)
{
    THDBasVidChnStatus tStatus;
    memset(&tStatus, 0, sizeof(tStatus));

    u8 byDSChnIdx = 0;

    //MAU����ģʽ�£�
    //  vga˫���ͨ��������MVChnIdx�£�
    //  h263plus˫������ͨ���͸�ͨ��������ͨ����������
    //  ͬ����Ƶ˫������ͨ������ͨ��������
    
    //MPU����ģʽ�£�
    //  ˫��ֻռһ��ͨ������Ҫ����˫��Դ���ô���(˫˫��ģʽ�� ��Ҫ��̬�л�)

    TEqp tEqp;
	u8 byConfIdx = g_cMcuVcApp.GetConfIdx(tConfInfo.GetConfId());
    if (IsDualDouble())
    {
        if (!m_tDSBrdChn.IsGrpMpu())
        {
            //4.1 H263p���������ȷ��
            THDAdaptParam tHDAdpParam;
            memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));
            
            //���h264������ͨ����
            tHDAdpParam.SetVidType(MEDIA_TYPE_H264);
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate()*tConfInfo.GetDStreamScale()/100);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H264));
            
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
            u8 byDStreamFPS = 5/*tConfInfo.GetDStreamUsrDefFPS()*/;
            tHDAdpParam.SetFrameRate(byDStreamFPS);
            
            u16 wWidth = 0;
            u16 wHeight = 0;
            GetWHByRes(VIDEO_FORMAT_XGA, wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            byDSChnIdx = 0;
            memset(&tStatus, 0, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 0);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] MVChnIdx.%d is not ready \
yet, unexpected(h263p: h264)\n", byDSChnIdx);
            }
            
            byDSChnIdx ++;
            
            
            //���h263plus���߸�ͨ����
            tHDAdpParam.SetVidType(MEDIA_TYPE_H263PLUS);
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate()*tConfInfo.GetDStreamScale()/100);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H263PLUS));
            tHDAdpParam.SetFrameRate(tConfInfo.GetCapSupportEx().GetSecDSFrmRate());
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
            
            GetWHByRes(VIDEO_FORMAT_XGA, wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            memset(&tStatus, 0, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 0);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] DSChnIdx.%d is not ready \
yet, unexpected(h263p: h263p)\n", byDSChnIdx);
            }
            
            byDSChnIdx ++;
            
            //4.2 ��̬˫�������ȷ��
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate()*tConfInfo.GetDStreamScale()/100);
            tHDAdpParam.SetVidType(MEDIA_TYPE_H264);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H264));
            
            wWidth = 0;
            wHeight = 0;
            GetWHByRes(VIDEO_FORMAT_XGA, wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            u8 byUsrDefFps = 5;
            tHDAdpParam.SetFrameRate(byUsrDefFps);
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
            
            memset(&tStatus, byDSChnIdx, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 0);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] DSChnIdx.%d is not ready \
yet, unexpected(h263p: vga)\n", byDSChnIdx);
            }
            
            byDSChnIdx ++;
        }
        else
        {
            //mpuģʽ��ֻռһ��ͨ����Ĭ�ϳ���H264 XGA + H263p XGA
			//					   ���ǰ汾��H264 XGA + H263  SVGA
            //4.1 H263p���������ȷ��
            THDAdaptParam tHDAdpParam;
            memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));
            
            //���h264������ͨ����
            tHDAdpParam.SetVidType(MEDIA_TYPE_H264);
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate()*tConfInfo.GetDStreamScale()/100);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H264));
            
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
            u8 byDStreamFPS = 5;
            tHDAdpParam.SetFrameRate(byDStreamFPS);
            
            u16 wWidth = 0;
            u16 wHeight = 0;
            GetWHByRes(VIDEO_FORMAT_XGA, wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            byDSChnIdx = 0;
            memset(&tStatus, 0, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 0);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] MVChnIdx.%d is not ready \
yet, unexpected(h263p: h264-mpu)\n", byDSChnIdx);
            }            
            
#ifndef _SATELITE_
            //���h263plus���߸�ͨ����
            tHDAdpParam.SetVidType(MEDIA_TYPE_H263PLUS);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H263PLUS));
			GetWHByRes(VIDEO_FORMAT_XGA, wWidth, wHeight);
#else
            //�������h263���߸�ͨ����
            tHDAdpParam.SetVidType(MEDIA_TYPE_H263);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H263));
			//GetWHByRes(VIDEO_FORMAT_SVGA, wWidth, wHeight);
			//ǿ�Ʊ�CIF
			GetWHByRes(VIDEO_FORMAT_CIF, wWidth, wHeight);
#endif
			tHDAdpParam.SetBitRate(tConfInfo.GetBitRate()*tConfInfo.GetDStreamScale()/100);
            tHDAdpParam.SetFrameRate(tConfInfo.GetCapSupportEx().GetSecDSFrmRate());
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());

            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            memset(&tStatus, 0, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 1);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] DSChnIdx.%d is not ready \
yet, unexpected(h263p: h263p-mpu)\n", byDSChnIdx);
            }
        }
    }
    else
    {
        if (IsDualEqMv())
        {
            //4.1 ͬ����Ƶ˫����ȷ��
            byDSChnIdx = 0;
            THDAdaptParam tHDAdpParam;
            memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));

            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate()*tConfInfo.GetDStreamScale()/100);
            tHDAdpParam.SetVidType(MEDIA_TYPE_H264);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H264));
            
            u16 wWidth = 0;
            u16 wHeight = 0;
            GetWHByRes(VIDEO_FORMAT_4CIF, wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
            u8 byDStreamFPS = tConfInfo.GetDStreamUsrDefFPS();
            tHDAdpParam.SetFrameRate(byDStreamFPS);
            
            byDSChnIdx = 0;
            memset(&tStatus, 0, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 0);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] DSChnIdx.%d \
is not ready yet, unexpected(Eq MV-4cif)\n", byDSChnIdx);
            }

            wWidth = 0;
            wHeight = 0;
            GetWHByRes(VIDEO_FORMAT_CIF, wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
            byDStreamFPS = tConfInfo.GetDStreamUsrDefFPS();
            tHDAdpParam.SetFrameRate(byDStreamFPS);
            
            byDSChnIdx = 0;
            memset(&tStatus, 0, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 1);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
            OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] DSChnIdx.%d \
is not ready yet, unexpected(Eq MV-cif)\n", byDSChnIdx);
            }
        }
        else
        {
            //4.1 ��̬˫�������ȷ��
            byDSChnIdx = 0;
            THDAdaptParam tHDAdpParam;
            memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));

            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate()*tConfInfo.GetDStreamScale()/100);
            tHDAdpParam.SetVidType(MEDIA_TYPE_H264);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H264));

            u16 wWidth = 0;
            u16 wHeight = 0;
            GetWHByRes(VIDEO_FORMAT_XGA, wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());

            u8 byUsrDefFps = 5;
            tHDAdpParam.SetFrameRate(byUsrDefFps);
            
            byDSChnIdx = 0;

            memset(&tStatus, 0, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 0);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] DSChnIdx.%d is not ready \
yet, unexpected(vga)\n", byDSChnIdx);
            }
        }
    }

    return;
}

/*=============================================================================
  �� �� ���� GetChnGrp
  ��    �ܣ� ��ȡĳbasͨ���� ������Ϣ
  �㷨ʵ�֣� FIXME: ��δ֧��ѡ����Ļ�ȡ(ѡ�����ȡû�����壿)
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/04/17    4.6         �ű���        ����
=============================================================================*/
void CBasMgr::GetChnGrp(u8 &byNum, TBasChn *ptBasChn, u8 byAdpMode)
{
    if (NULL == ptBasChn)
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::GetChnGrp] ptBasChn.0x%x\n", ptBasChn);
        return;
    }

    if (CHN_ADPMODE_MVBRD != byAdpMode && CHN_ADPMODE_DSBRD != byAdpMode)
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::GetChnGrp] unexpected adp mode.%d!\n", byAdpMode);
        return;

    }
    byNum = 0;
    
    if (CHN_ADPMODE_DSBRD == byAdpMode)
    {
        m_tDSBrdChn.GetChn(byNum, ptBasChn);
    }
    else
    {
        m_tMVBrdChn.GetChn(byNum, ptBasChn);
    }

    return;
}

/*=============================================================================
  �� �� ���� GetChnGrp
  ��    �ܣ� ��ȡĳbasͨ���� ����ģʽ��MVBRD/DSBRD/MVSEL/DSSEL
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/04/17    4.6         �ű���        ����
=============================================================================*/
u8 CBasMgr::GetChnMode(const TEqp &tEqp, u8 byChnId)
{
    u8 byChnMode = CHN_ADPMODE_NONE;

    u8 byType = GetChnType(tEqp, byChnId);

    if (m_tMVBrdChn.IsChnExist(tEqp, byChnId, byType))
    {
        byChnMode = CHN_ADPMODE_MVBRD;
    }
    else if (m_tDSBrdChn.IsChnExist(tEqp, byChnId, byType))
    {
        byChnMode = CHN_ADPMODE_DSBRD;
    }
    else if (m_tMVSelChn.IsChnExist(tEqp, byChnId, byType))
    {
        byChnMode = CHN_ADPMODE_MVSEL;
    }
    else if (m_tDSSelChn.IsChnExist(tEqp, byChnId, byType))
    {
        byChnMode = CHN_ADPMODE_DSSEL;
    }

    return byChnMode;
}

/*=============================================================================
  �� �� ���� ReleaseHdBasChn
  ��    �ܣ� ��������� �ͷű������Ѿ�ռ�õ�����bas
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/09    4.5         �ű���        ����
=============================================================================*/
void CBasMgr::ReleaseHdBasChn()
{
    u8 byIdx = 0;
    for(; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        THDBasVidChnStatus tStatus;
        memset(&tStatus, 0, sizeof(tStatus));
        if (m_tMVBrdChn.GetChn(byIdx, tStatus) && tStatus.IsReserved())
        {
            g_cMcuVcApp.ReleaseHDBasChn(tStatus.GetEqpId(), tStatus.GetChnIdx());
        }
    }
    m_tMVBrdChn.Clear();

    for(byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        THDBasVidChnStatus tStatus;
        memset(&tStatus, 0, sizeof(tStatus));
        if (m_tDSBrdChn.GetChn(byIdx, tStatus) && tStatus.IsReserved())
        {
            g_cMcuVcApp.ReleaseHDBasChn(tStatus.GetEqpId(), tStatus.GetChnIdx());
        }
    }

    m_tDSBrdChn.Clear();

    return;
}


/*=============================================================================
  �� �� ���� AddBrdChn
  ��    �ܣ� ��������� ����ռ��һ��Bas, д���֧��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� BOOL32 bH263p
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/09    4.5         �ű���        ����
=============================================================================*/
void CBasMgr::AddBrdChn(const TEqp &tEqp, u8 byChnId, u8 byChnType, BOOL32 bDual)
{
    //h263plus������������ͬ����Ƶ˫��Ҳ����������
    if (!bDual)
    {
        m_tMVBrdChn.AddChn(tEqp, byChnId, byChnType);
    }
    //˫��
    else
    {
        m_tDSBrdChn.AddChn(tEqp, byChnId, byChnType);
    }
    return;
}

/*=============================================================================
  �� �� ���� GetChnId
  ��    �ܣ� ��������� ��ȡ�������������
  �㷨ʵ�֣� �㲥����ͨ�� ��1��ʼ��6,
             ����ѡ��ͨ����7��ʼ
             ˫��ѡ��Ҫ��������ѡ����16��4 �͹㲥����ͨ��
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/18    4.5         �ű���        ����
=============================================================================*/
u8 CBasMgr::GetChnId(const TEqp &tEqp, u8 byChId)
{
    u8 byChnIdx = 0;
    u8 byChnType = GetChnType(tEqp, byChId);

    if (m_tMVBrdChn.IsChnExist(tEqp, byChId, byChnType))
    {
        byChnIdx = m_tMVBrdChn.GetChnPos(tEqp, byChId, byChnType);
    }
    else if (m_tDSBrdChn.IsChnExist(tEqp, byChId, byChnType))
    {
        byChnIdx = m_tDSBrdChn.GetChnPos(tEqp, byChId, byChnType) + MAXNUM_CONF_MVCHN;
    }
    else if (m_tMVSelChn.IsChnExist(tEqp, byChId, byChnType))
    {
        byChnIdx = m_tMVSelChn.GetChnPos(tEqp, byChId, byChnType) + MAXNUM_CONF_MVCHN + MAXNUM_CONF_DSCHN;
    }
    else if (m_tDSSelChn.IsChnExist(tEqp, byChId, byChnType))
    {
        byChnIdx = m_tMVSelChn.GetChnPos(tEqp, byChId, byChnType) + MAXNUM_CONF_MVCHN + MAXNUM_CONF_DSCHN + MAXNUM_PERIEQP * MAXNUM_MPU_VCHN;
    }

    return byChnIdx;
}


/*=============================================================================
  �� �� ���� GetActivePL
  ��    �ܣ� ��������� ��ȡ��̬�غɷ�װ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/18    4.5         �ű���        ����
=============================================================================*/
u8 CBasMgr::GetActivePL(const TConfInfo &tConfInfo, u8 byMediaType)
{
    u8 byConfIdx = g_cMcuVcApp.GetConfIdx(tConfInfo.GetConfId());
    return GetActivePayload(tConfInfo, byMediaType);
}

/*=============================================================================
  �� �� ���� GetChnStatus
  ��    �ܣ� ��������� ��ȡͨ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/04/17    4.6         �ű���        ����
=============================================================================*/
BOOL32 CBasMgr::GetChnStatus(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tStatus)
{
    u8 byType = GetChnType(tEqp, byChnId);
    
    if (MAU_CHN_NONE == byType)
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::GetChnStatus] byType.%d, reject it\n", byType);
        return FALSE;
    }
    
    if (m_tMVBrdChn.IsChnExist(tEqp, byChnId, byType) ||
        m_tDSBrdChn.IsChnExist(tEqp, byChnId, byType) ||
        m_tMVSelChn.IsChnExist(tEqp, byChnId, byType) ||
        m_tDSSelChn.IsChnExist(tEqp, byChnId, byType) )
    {
        TPeriEqpStatus tEqpStatus;
        if (!g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tEqpStatus))
        {
            return FALSE;
        }

        THDBasVidChnStatus *ptVidChn = NULL;

        u8 byType = GetChnType(tEqp, byChnId);
        if (MPU_CHN_NORMAL == byType)
        {
            ptVidChn= tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnId);
        }
        else
        {
            if (0 == byChnId)
            {
                ptVidChn= tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus();
            }
            else if (1 == byChnId)
            {
                ptVidChn= tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus();
            }
        }
        if (ptVidChn->IsNull())
        {
            return FALSE;
        }
        
        memcpy(&tStatus, ptVidChn, sizeof(tStatus));
        return TRUE;
    }

    OspPrintf(TRUE, FALSE, "[CBasMgr::GetChnStatus] Eqp<%d, %d> failed\n", tEqp.GetEqpId(), byChnId);

    return FALSE;
}

/*=============================================================================
  �� �� ���� UpdateChn
  ��    �ܣ� ��������� ����ͨ��״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/04/17    4.6         �ű���        ����
=============================================================================*/
BOOL32 CBasMgr::UpdateChn(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tStatus)
{
    if(tStatus.IsNull())
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::UpdateChn] tStatus.IsNull for Eqp<%d, %d>, reject it\n", tEqp.GetEqpId(), byChnId);
        return FALSE;
    }
    
    u8 byType = GetChnType(tEqp, byChnId);
    if (MAU_CHN_NONE == byType)
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::UpdateChn] byChnType.%d, reject it\n", byType);
        return FALSE;
    }

    if (m_tMVBrdChn.IsChnExist(tEqp, byChnId, byType) ||
        m_tDSBrdChn.IsChnExist(tEqp, byChnId, byType) ||
        m_tMVSelChn.IsChnExist(tEqp, byChnId, byType) ||
        m_tDSSelChn.IsChnExist(tEqp, byChnId, byType) )
    {
        TPeriEqpStatus tEqpStatus;
        if (!g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tEqpStatus))
        {
            return FALSE;
        }

        u8 byType = GetChnType(tEqp, byChnId);
        if (MPU_CHN_NORMAL == byType)
        {
            tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tStatus, byChnId);
        }
        else
        {
            if (0 == byChnId)
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tStatus);
            }
            else if (1 == byChnId)
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tStatus);
            }
        }
        g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tEqpStatus);
        
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
  �� �� ���� GetLowBREqp
  ��    �ܣ� ��������� ��ȡ����ͨ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/12/11    4.5         �ű���        ����
=============================================================================*/
BOOL32 CBasMgr::GetLowBREqp(const TConfInfo &tConfInfo, TEqp &tEqp, u8 &byChnId)
{
    BOOL32 bExist = FALSE;

    if (0 == tConfInfo.GetSecBitRate())
    {
        OspPrintf(TRUE, FALSE, "[GetLowBREqp] conf.%s has no sec bitrate, ignore it!\n", tConfInfo.GetConfName());
        return FALSE;
    }
    for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        THDBasVidChnStatus tVidChn;
        m_tMVBrdChn.GetChn(byIdx, tVidChn);
        if (tVidChn.IsNull())
        {
            continue;
        }
        //��������
        u16 wHeight = tVidChn.GetOutputVidParam(0)->GetHeight();
        u16 wWidth = tVidChn.GetOutputVidParam(0)->GetWidth();
        u8 byRes = GetResByWH(wWidth, wHeight);
        if(byRes == tConfInfo.GetMainVideoFormat() &&
           tVidChn.GetOutputVidParam(0)->GetBitrate() < tConfInfo.GetBitRate())
        {
            tEqp = tVidChn.GetEqp();
            byChnId = tVidChn.GetChnIdx();
            bExist = TRUE;
            break;
        }
    }

    return bExist;
}

/*=============================================================================
  �� �� ���� GetBasResource
  ��    �ܣ� ���������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����u8 byMediaType
            u8 byRes
            TEqp &tHDBas, u8 &byChnId, u8 &byOutIdx, BOOL32 bDual
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/27    4.5         �ű���        ����
=============================================================================*/
BOOL32 CBasMgr::GetBasResource(u8 byMediaType, u8 byRes,
                               TEqp &tHDBas, u8 &byChnId,
                               u8 &byOutIdx, BOOL32 bDual, BOOL32 bH263p)
{
    if (!bDual)
        return m_tMVBrdChn.GetBasResource(byMediaType, byRes, tHDBas, byChnId, byOutIdx);
    else
        return m_tDSBrdChn.GetBasResource(byMediaType, byRes, tHDBas, byChnId, byOutIdx, bH263p);
}

/*=============================================================================
  �� �� ���� GetChnVcuTick
  ��    �ܣ� ���������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/27    4.5         �ű���        ����
=============================================================================*/
u32 CBasMgr::GetChnVcuTick(const TEqp &tEqp, u8 byChnIdx)
{
    u8 byType = GetChnType(tEqp, byChnIdx);

    if (m_tMVBrdChn.IsChnExist(tEqp, byChnIdx, byType))
    {
        return m_tMVBrdChn.GetChnVcuTick(tEqp, byChnIdx);
    }
    else if (m_tDSBrdChn.IsChnExist(tEqp, byChnIdx, byType))
    {
        return m_tDSBrdChn.GetChnVcuTick(tEqp, byChnIdx);
    }
    OspPrintf(TRUE, FALSE, "[GetChnVcuTick] tEqp<%d, %d> get tick failed!\n", tEqp.GetEqpId(), byChnIdx);
    return 0;
}

/*=============================================================================
  �� �� ���� IsBrdGrpMpu
  ��    �ܣ� ���������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/27    4.5         �ű���        ����
=============================================================================*/
BOOL32 CBasMgr::IsBrdGrpMpu()
{
    return m_tDSBrdChn.IsGrpMpu();
}

/*=============================================================================
  �� �� ���� SetChnVcuTick
  ��    �ܣ� ���������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/27    4.5         �ű���        ����
=============================================================================*/
BOOL32 CBasMgr::SetChnVcuTick(const TEqp &tEqp, u8 byChnIdx, u32 dwCurTicks)
{
    u8 byType = GetChnType(tEqp, byChnIdx);

    if (m_tMVBrdChn.IsChnExist(tEqp, byChnIdx, byType))
    {
        return m_tMVBrdChn.SetChnVcuTick(tEqp, byChnIdx, dwCurTicks);
    }
    else if (m_tDSBrdChn.IsChnExist(tEqp, byChnIdx, byType))
    {
        return m_tDSBrdChn.SetChnVcuTick(tEqp, byChnIdx, dwCurTicks);
    }

    OspPrintf(TRUE, FALSE, "[SetChnVcuTick] tEqp<%d, %d> set tick failed!\n", tEqp.GetEqpId(), byChnIdx);
    return FALSE;
}

/*=============================================================================
  �� �� ���� Clear
  ��    �ܣ� ��������� ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/09    4.5         �ű���        ����
=============================================================================*/
void CBasMgr::Clear()
{
    m_emType = emBgin;
}

/*=============================================================================
  �� �� ���� Print
  ��    �ܣ� ��������� ��ӡ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/09    4.5         �ű���        ����
=============================================================================*/
void CBasMgr::Print()
{
    THDBasVidChnStatus tStatus;

    u8 byIdx = 0;
    for (; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        memset(&tStatus, 0, sizeof(tStatus));

        m_tMVBrdChn.GetChn(byIdx, tStatus);
        if (!tStatus.IsNull())
        {
            OspPrintf(TRUE, FALSE, "[MV-Part]:\nTBasStatus.%d as follows:\n\n", tStatus.GetEqpId());
            tStatus.PrintInfo();
        }
        OspPrintf(TRUE, FALSE, "\n");
    }

    for (byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        memset(&tStatus, 0, sizeof(tStatus));

        m_tDSBrdChn.GetChn(byIdx, tStatus);
        if (!tStatus.IsNull())
        {
            OspPrintf(TRUE, FALSE, "[DS-Part]:\nTBasStatus.%d as follows:\n\n", tStatus.GetEqpId());
            tStatus.PrintInfo();
        }
        OspPrintf(TRUE, FALSE, "\n");
    }
    return;
}


/*=============================================================================
  �� �� ���� TMVChnGrp
  ��    �ܣ� ����ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/19    4.5         �ű���        ����
=============================================================================*/
TMVChnGrp::TMVChnGrp()
{
    Clear();
}

/*=============================================================================
  �� �� ���� Clear
  ��    �ܣ� ����ͨ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/19    4.5         �ű���        ����
=============================================================================*/
void TMVChnGrp::Clear()
{
    memset(this, 0, sizeof(TMVChnGrp));
}

/*=============================================================================
  �� �� ���� GetBasResource
  ��    �ܣ� ����ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/19    4.5         �ű���        ����
=============================================================================*/
BOOL32 TMVChnGrp::GetBasResource(u8 byMediaType, u8 byRes, TEqp &tHDBas, u8 &byChnId, u8 &byOutIdx)
{
    for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        for(u8 byOut = 0; byOut < MAXNUM_VOUTPUT; byOut++)
        {
            TEqp tMau = m_atChn[byIdx].GetEqp();
            if (tMau.IsNull())
            {
                continue;
            }
            TPeriEqpStatus tEqpStatus;
            if (!g_cMcuVcApp.GetPeriEqpStatus(tMau.GetEqpId(), &tEqpStatus))
            {
                continue;
            }

            u8 byEqpType = tEqpStatus.m_tStatus.tHdBas.GetEqpType();
            
            THDBasVidChnStatus *ptVidChn;
            
            switch (byEqpType)
            {
            case TYPE_MAU_NORMAL:
            case TYPE_MAU_H263PLUS:
                if (m_atChn[byIdx].GetChnId() == 0)
                {
                    ptVidChn = tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus();
                }
                else if (m_atChn[byIdx].GetChnId() == 1)
                {
                    ptVidChn = tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus();
                }
                break;
            case TYPE_MPU:
                ptVidChn = tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(m_atChn[byIdx].GetChnId());
                break;
            }

            if (ptVidChn->IsNull())
            {
                continue;
            }

            THDAdaptParam tHDAdpParam = *ptVidChn->GetOutputVidParam(byOut);

            if (!tHDAdpParam.IsNull() &&
                tHDAdpParam.GetVidType() == byMediaType &&
                //zbq[04/29/2009] mp4���Էֱ��ʣ����зֱ��ʹ�һ��CIF
                (byMediaType == MEDIA_TYPE_MP4 ||
                 GetResByWH(tHDAdpParam.GetWidth(), tHDAdpParam.GetHeight()) == byRes))
            {
                byChnId = m_atChn[byIdx].GetChnId(); 
                tHDBas = m_atChn[byIdx].GetEqp();
                byOutIdx = byOut;
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*=============================================================================
  �� �� ���� SetChnVcuTick
  ��    �ܣ� ����ͨ�� vcu tick����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/12/11    4.5         �ű���        ����
=============================================================================*/
BOOL32 TMVChnGrp::SetChnVcuTick(const TEqp &tEqp, u8 byChnIdx, u32 dwCurTicks)
{
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        if (tEqp == m_atChn[byIdx].GetEqp() &&
            byChnIdx == m_atChn[byIdx].GetChnId() )
        {
            m_adwLastVCUTick[byIdx] = dwCurTicks;
            return TRUE;
        }
    }
    OspPrintf(TRUE, FALSE, "[SetChnVcuTick] set eqp<%d, %d> vcu tick failed\n", tEqp.GetEqpId(), byChnIdx);
    return FALSE;
}

/*=============================================================================
  �� �� ���� GetChnVcuTick
  ��    �ܣ� ����ͨ�� vcu tick
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/12/11    4.5         �ű���        ����
=============================================================================*/
u32 TMVChnGrp::GetChnVcuTick(const TEqp &tEqp, u8 byChnIdx)
{
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        if (tEqp == m_atChn[byIdx].GetEqp() &&
            byChnIdx == m_atChn[byIdx].GetChnId() )
        {
            return m_adwLastVCUTick[byIdx];
        }
    }
    OspPrintf(TRUE, FALSE, "[GetChnVcuTick] get eqp<%d, %d> vcu tick failed\n", tEqp.GetEqpId(), byChnIdx);
    return 0;
}

/*=============================================================================
  �� �� ���� IsChnExist
  ��    �ܣ� ����ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/20    4.5         �ű���        ����
=============================================================================*/
BOOL32 TMVChnGrp::IsChnExist(const TEqp &tEqp, u8 byChnIdx, u8 byType)
{
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        if (m_atChn[byIdx].GetEqp() == tEqp &&
            m_atChn[byIdx].GetChnId() == byChnIdx &&
            m_atChn[byIdx].GetType() == byType)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
  �� �� ���� GetChnPos
  ��    �ܣ� ����ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/20    4.5         �ű���        ����
=============================================================================*/
u8 TMVChnGrp::GetChnPos(const TEqp &tEqp, u8 byChnIdx, u8 byType)
{
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        if (m_atChn[byIdx].GetEqp() == tEqp &&
            m_atChn[byIdx].GetChnId() == byChnIdx &&
            m_atChn[byIdx].GetType() == byType)
        {
            return byIdx;
        }
    }
    
    return 0xff;
}

/*=============================================================================
  �� �� ���� AddChn
  ��    �ܣ� ����ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/19    4.5         �ű���        ����
=============================================================================*/
BOOL32 TMVChnGrp::AddChn(const TEqp &tEqp, u8 byChnId, u8 byType)
{
    if (IsChnExist(tEqp, byChnId, byType))
    {
        return TRUE;
    }
    u8 byPos = 0xff;
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        if (m_atChn[byIdx].IsNull())
        {
            byPos = byIdx;
            break;
        }
    }
    if (byPos == 0xff)
    {
        OspPrintf(TRUE, FALSE, "[TMVChnGrp::AddChn] no pos for new chn<Eqp.%d, Chn.%d>\n",
                                tEqp.GetEqpId(), byChnId);
        return FALSE;
    }
    m_atChn[byPos].SetEqp(tEqp);
    m_atChn[byPos].SetChnId(byChnId);
    m_atChn[byPos].SetType(byType);

    //��ͨ����������ն�Ӧ�������
    g_cMcuVcApp.ResetHDBasChn(tEqp.GetEqpId(), byChnId);

    return TRUE;
}

/*=============================================================================
  �� �� ���� GetChn
  ��    �ܣ� ����ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/19    4.5         �ű���        ����
=============================================================================*/
BOOL32 TMVChnGrp::GetChn(u8 byIdx, THDBasVidChnStatus &tChn)
{
    if (byIdx >= MAXNUM_CONF_MVCHN)
    {
        OspPrintf(TRUE, FALSE, "[TMVChnGrp::GetChn] unexpected idx.%d\n", byIdx);
        return FALSE;
    }

    TPeriEqpStatus tEqpStatus;
    if (!g_cMcuVcApp.GetPeriEqpStatus(m_atChn[byIdx].GetEqpId(), &tEqpStatus))
    {
        return FALSE;
    }

    u8 byChnId = m_atChn[byIdx].GetChnId();
    THdBasStatus tHdBasStatus = tEqpStatus.m_tStatus.tHdBas;
    u8 byChnType = GetChnType(m_atChn[byIdx].GetEqp(), byChnId);
    
    THDBasVidChnStatus *ptVidStatus = NULL;
    switch (byChnType)
    {
    case MAU_CHN_NORMAL:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetVidChnStatus();
        break;
    case MAU_CHN_VGA:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetDVidChnStatus();
        break;
    case MAU_CHN_264TO263:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetDVidChnStatus();
        break;
    case MAU_CHN_263TO264:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetVidChnStatus();
        break;
    case MPU_CHN_NORMAL:
        if (NULL == ptVidStatus)
        {
            ptVidStatus = tHdBasStatus.tStatus.tMpuBas.GetVidChnStatus(byChnId);
        }
        break;
    }
    
    if (NULL == ptVidStatus || ptVidStatus->IsNull())
    {
        return FALSE;
    }

    memcpy(&tChn, ptVidStatus, sizeof(tChn));
    return TRUE;
}

/*=============================================================================
  �� �� ���� GetChn
  ��    �ܣ� ����ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/19    4.5         �ű���        ����
=============================================================================*/
BOOL32 TMVChnGrp::GetChn(u8 &byNum, TBasChn *ptBasChn)
{
    if (NULL == ptBasChn)
    {
        OspPrintf(TRUE, FALSE, "[TMVChnGrp::GetChn] NULL == ptBasChn!\n");
        return FALSE;
    }
    byNum = 0;
    
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        if (m_atChn[byIdx].IsNull())
        {
            continue;
        }
        ptBasChn[byNum] = m_atChn[byIdx];
        byNum++;
    }
    return TRUE;
}

/*=============================================================================
  �� �� ���� UpdateChn
  ��    �ܣ� ����ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/19    4.5         �ű���        ����
=============================================================================*/
BOOL32 TMVChnGrp::UpdateChn(u8 byIdx, THDBasVidChnStatus &tChn)
{
    u8 byPos = 0xff;
    u8 byCurPos = 0xff;

    THDBasChnStatus tMVChn = (THDBasChnStatus)tChn;
    if (tChn.IsNull())
    {
        return FALSE;
    }

    TPeriEqpStatus tStatus;

    if (!g_cMcuVcApp.GetPeriEqpStatus(m_atChn[byIdx].GetEqpId(), &tStatus))
    {
        return FALSE;
    }
    
    u8 byChnId = m_atChn[byIdx].GetChnId();
    THdBasStatus tHdBasStatus = tStatus.m_tStatus.tHdBas;
    u8 byChnType = GetChnType(m_atChn[byIdx].GetEqp(), byChnId);
    
    switch (byChnType)
    {
    case MAU_CHN_NORMAL:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tChn);
        break;
    case MAU_CHN_VGA:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tChn);
        break;
    case MAU_CHN_264TO263:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tChn);
        break;
    case MAU_CHN_263TO264:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tChn);
        break;
    case MPU_CHN_NORMAL:
        tStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tChn, byChnId);
        break;
    }

    g_cMcuVcApp.SetPeriEqpStatus(m_atChn[byIdx].GetEqpId(), &tStatus);
	g_cMcuVcApp.SendPeriEqpStatusToMcs(m_atChn[byIdx].GetEqpId());

    return TRUE;
}

/*=============================================================================
  �� �� ���� TDSChnGrp
  ��    �ܣ� ˫��ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/19    4.5         �ű���        ����
=============================================================================*/
TDSChnGrp::TDSChnGrp()
{
    Clear();
}

/*=============================================================================
  �� �� ���� Clear
  ��    �ܣ� ˫��ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/19    4.5         �ű���        ����
=============================================================================*/
void TDSChnGrp::Clear()
{
    memset(this, 0, sizeof(TDSChnGrp));
}

/*=============================================================================
  �� �� ���� IsChnExist
  ��    �ܣ� ˫��ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/20    4.5         �ű���        ����
=============================================================================*/
BOOL32 TDSChnGrp::IsChnExist(const TEqp &tEqp, u8 byChnIdx, u8 byType)
{
    u8 byIdx = 0;
    for(byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        if (m_atChn[byIdx].GetEqp() == tEqp &&
            m_atChn[byIdx].GetChnId() == byChnIdx &&
            m_atChn[byIdx].GetType() == byType)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
  �� �� ���� GetChnPos
  ��    �ܣ� ˫��ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/20    4.5         �ű���        ����
=============================================================================*/
u8 TDSChnGrp::GetChnPos(const TEqp &tEqp, u8 byChnIdx, u8 byType)
{
    u8 byIdx = 0;
    for(byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        if (m_atChn[byIdx].GetEqp() == tEqp &&
            m_atChn[byIdx].GetChnId() == byChnIdx &&
            m_atChn[byIdx].GetType() == byType)
        {
            return byIdx;
        }
    }
    
    return 0xff;
}

/*=============================================================================
  �� �� ���� GetBasResource
  ��    �ܣ� ˫��ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/27    4.5         �ű���        ����
=============================================================================*/
BOOL32 TDSChnGrp::GetBasResource(u8     byMediaType, 
                                 u8     byRes,
                                 TEqp  &tHDBas,
                                 u8    &byChnId,
                                 u8    &byOutIdx,
                                 BOOL32 bH263p)
{
    u8 byIdx = 0;
    u8 byOut = 0;

    for (byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        for(byOut = 0; byOut < MAXNUM_VOUTPUT; byOut++)
        {
            TEqp tMau = m_atChn[byIdx].GetEqp();
            if (tMau.IsNull())
            {
                continue;
            }
            TPeriEqpStatus tEqpStatus;
            if (!g_cMcuVcApp.GetPeriEqpStatus(tMau.GetEqpId(), &tEqpStatus))
            {
                continue;
            }

            u8 byEqpType = tEqpStatus.m_tStatus.tHdBas.GetEqpType();

            THDBasVidChnStatus *ptVidChn;

            switch (byEqpType)
            {
            case TYPE_MAU_NORMAL:
            case TYPE_MAU_H263PLUS:
                if (m_atChn[byIdx].GetChnId() == 0)
                {
                    ptVidChn = tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus();
                }
                else if (m_atChn[byIdx].GetChnId() == 1)
                {
                    ptVidChn = tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus();
                }
            	break;
            case TYPE_MPU:
                ptVidChn = tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(m_atChn[byIdx].GetChnId());
                break;
            }
            
            if (ptVidChn->IsNull())
            {
                continue;
            }

            u8 byChnType = GetChnType(m_atChn[byIdx].GetEqp(), m_atChn[byIdx].GetChnId());

            THDAdaptParam tHDAdpParam = *ptVidChn->GetOutputVidParam(byOut);
            if (!tHDAdpParam.IsNull() &&
                tHDAdpParam.GetVidType() == byMediaType &&
                GetResByWH(tHDAdpParam.GetWidth(), tHDAdpParam.GetHeight()) == byRes)
            {
                if (bH263p && byChnType != MAU_CHN_263TO264 && byChnType != MPU_CHN_NORMAL)
                {
                    continue;
                }
                byChnId = m_atChn[byIdx].GetChnId(); 
                tHDBas = m_atChn[byIdx].GetEqp();
                byOutIdx = byOut;
                return TRUE;
            }
        }
    }

    return FALSE;
}


/*=============================================================================
  �� �� ���� IsGrpMpu
  ��    �ܣ� ˫���㲥�������Ƿ����MPU
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/06/02    4.5         �ű���        ����
=============================================================================*/
BOOL32 TDSChnGrp::IsGrpMpu()
{
    u8 byIdx = 0;
    
    for (byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        TEqp tMau = m_atChn[byIdx].GetEqp();
        if (tMau.IsNull())
        {
            continue;
        }
        TPeriEqpStatus tEqpStatus;
        if (!g_cMcuVcApp.GetPeriEqpStatus(tMau.GetEqpId(), &tEqpStatus))
        {
            continue;
        }
        
        u8 byEqpType = tEqpStatus.m_tStatus.tHdBas.GetEqpType();
        if (TYPE_MPU == byEqpType)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
  �� �� ���� SetChnVcuTick
  ��    �ܣ� ˫��ͨ�� VCU TICK
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/12/12    4.5         �ű���        ����
=============================================================================*/
BOOL32 TDSChnGrp::SetChnVcuTick(const TEqp &tEqp, u8 byChnIdx, u32 dwCurTicks)
{
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        if (tEqp == m_atChn[byIdx].GetEqp() &&
            byChnIdx == m_atChn[byIdx].GetChnId())
        {
            m_adwLastVCUTick[byIdx] = dwCurTicks;
            return TRUE;
        }
    }
    OspPrintf(TRUE, FALSE, "[SetChnVcuTick] set eqp<%d, %d> vcu tick failed(dual)\n", tEqp.GetEqpId(), byChnIdx);
    return FALSE;
}

/*=============================================================================
  �� �� ���� GetChnVcuTick
  ��    �ܣ� ˫��ͨ�� VCU TICK
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/12/12    4.5         �ű���        ����
=============================================================================*/
u32 TDSChnGrp::GetChnVcuTick(const TEqp &tEqp, u8 byChnIdx)
{
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        if (tEqp == m_atChn[byIdx].GetEqp() &&
            byChnIdx == m_atChn[byIdx].GetChnId())
        {
            return m_adwLastVCUTick[byIdx];
        }
    }
    OspPrintf(TRUE, FALSE, "[GetChnVcuTick] set eqp<%d, %d> vcu tick failed(dual)\n", tEqp.GetEqpId(), byChnIdx);
    return FALSE;
}


/*=============================================================================
  �� �� ���� AddChn
  ��    �ܣ� ˫��ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/19    4.5         �ű���        ����
=============================================================================*/
BOOL32 TDSChnGrp::AddChn(const TEqp &tEqp, u8 byChnId, u8 byType)
{
    if (IsChnExist(tEqp, byChnId, byType))
    {
        return TRUE;
    }
    u8 byPos = 0xff;

    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        if (m_atChn[byIdx].IsNull())
        {
            byPos = byIdx;
            break;
        }
    }
    if (0xff == byPos)
    {
        OspPrintf(TRUE, FALSE, "[TDSChnGrp::AddChn] no pos for new chn<Eqp.%d, Chn.%d>\n",
            tEqp.GetEqpId(), byChnId);
        return FALSE;
    }
    m_atChn[byPos].SetEqp(tEqp);
    m_atChn[byPos].SetChnId(byChnId);
    m_atChn[byPos].SetType(byType);

    //��ͨ����������ն�Ӧ�������
    g_cMcuVcApp.ResetHDBasChn(tEqp.GetEqpId(), byChnId);

    return FALSE;
}

/*=============================================================================
  �� �� ���� GetChn
  ��    �ܣ� ˫��ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/19    4.5         �ű���        ����
=============================================================================*/
BOOL32 TDSChnGrp::GetChn(u8 byIdx, THDBasVidChnStatus &tChn)
{
    if (byIdx >= MAXNUM_CONF_DSCHN)
    {
        OspPrintf(TRUE, FALSE, "[TDSChnGrp::GetChn] unexpected idx.%d\n", byIdx);
        return FALSE;
    }
    TPeriEqpStatus tStatus;
    g_cMcuVcApp.GetPeriEqpStatus(m_atChn[byIdx].GetEqpId(), &tStatus);
    
    u8 byChnId = m_atChn[byIdx].GetChnId();
    THdBasStatus tHdBasStatus = tStatus.m_tStatus.tHdBas;
    u8 byChnType = GetChnType(m_atChn[byIdx].GetEqp(), byChnId);

    THDBasVidChnStatus *ptVidStatus = NULL;
    switch (byChnType)
    {
    case MAU_CHN_NORMAL:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetVidChnStatus();
        break;
    case MAU_CHN_VGA:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetDVidChnStatus();
        break;
    case MAU_CHN_264TO263:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetDVidChnStatus();
        break;
    case MAU_CHN_263TO264:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetVidChnStatus();
        break;
    case MPU_CHN_NORMAL:
        if (NULL == ptVidStatus)
        {
            ptVidStatus = tHdBasStatus.tStatus.tMpuBas.GetVidChnStatus(byChnId);
        }
        break;
    }

    if (NULL == ptVidStatus || ptVidStatus->IsNull())
    {
        return FALSE;
    }
    memcpy(&tChn, ptVidStatus, sizeof(tChn));

    return TRUE;
}

/*=============================================================================
  �� �� ���� GetChn
  ��    �ܣ� ˫��ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/19    4.5         �ű���        ����
=============================================================================*/
BOOL32 TDSChnGrp::GetChn(u8 &byNum, TBasChn *ptBasChn)
{
    if (NULL == ptBasChn)
    {
        OspPrintf(TRUE, FALSE, "[TDSChnGrp::GetChn] NULL == ptBasChn!\n");
        return FALSE;
    }
    byNum = 0;
    
    u8 byIdx = 0;
    for(byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        if (m_atChn[byIdx].IsNull())
        {
            continue;
        }
        ptBasChn[byNum] = m_atChn[byIdx];
        byNum++;
    }

    return TRUE;
}

/*=============================================================================
  �� �� ���� UpdateChn
  ��    �ܣ� ˫��ͨ�� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/19    4.5         �ű���        ����
=============================================================================*/
BOOL32 TDSChnGrp::UpdateChn(u8 byIdx, THDBasVidChnStatus &tChn)
{
    u8 byPos = 0xff;
    THDBasChnStatus tDSChn = (THDBasChnStatus)tChn;

    if (tChn.IsNull())
    {
        OspPrintf(TRUE, FALSE, "[TDSChnGrp::UpdateChn] Idx.%d set null rejected!\n", byIdx);
        return FALSE;
    }

    TPeriEqpStatus tStatus;

    if (!g_cMcuVcApp.GetPeriEqpStatus(m_atChn[byIdx].GetEqpId(), &tStatus))
    {
        return FALSE;
    }

    u8 byChnId = m_atChn[byIdx].GetChnId();
    THdBasStatus tHdBasStatus = tStatus.m_tStatus.tHdBas;
    u8 byChnType = GetChnType(m_atChn[byIdx].GetEqp(), byChnId);
    
    switch (byChnType)
    {
    case MAU_CHN_NORMAL:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tChn);
        break;
    case MAU_CHN_VGA:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tChn);
        break;
    case MAU_CHN_264TO263:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tChn);
        break;
    case MAU_CHN_263TO264:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tChn);
        break;
    case MPU_CHN_NORMAL:
        tStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tChn, byChnId);
        break;
    }

    g_cMcuVcApp.SetPeriEqpStatus(m_atChn[byIdx].GetEqpId(), &tStatus);
    g_cMcuVcApp.SendPeriEqpStatusToMcs(m_atChn[byIdx].GetEqpId());

    return TRUE;
}


/********************************************

        �������Ⱥ�����ʵ��

  *******************************************/

/*=============================================================================
  �� �� ���� AddExcept
  ��    �ܣ� Ⱥ������� �����������Ա��Ⱥ������ǰ�����ȴ���˹���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void CRcvGrp::AddExcept(u8 byMtId, BOOL32 bDual)
{
    if (!bDual)
        m_tMVGrp.AddExcept(byMtId);
    else
        m_tDSGrp.AddExcept(byMtId);
    return;
}

/*=============================================================================
  �� �� ���� AddMem
  ��    �ܣ� Ⱥ������� ����˫����Ա
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� bExceptEither���Ƿ�ͬʱ�������ӵ���������
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void CRcvGrp::AddMem(u8 byMtId, TDStreamCap &tDCap, BOOL32 bExc)
{
    u8 byRes = GetDSRes(tDCap.GetMediaType(), tDCap.GetResolution());
    m_tDSGrp.AddMem(byMtId, (TDSRcvGrp::emType)byRes, bExc);
    return;
}

/*=============================================================================
  �� �� ���� AddMem
  ��    �ܣ� Ⱥ������� ����������Ա
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� bExceptEither���Ƿ�ͬʱ�������ӵ���������
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void CRcvGrp::AddMem(u8 byMtId, TSimCapSet &tSim, BOOL32 bExc)
{
    u8 byRes = GetMVRes(tSim.GetVideoMediaType(), tSim.GetVideoResolution());
    m_tMVGrp.AddMem(byMtId, (TMVRcvGrp::emType)byRes, bExc);
    return;
}

/*=============================================================================
  �� �� ���� RemoveMem
  ��    �ܣ� Ⱥ������� ����������Ա
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� bExceptEither���Ƿ�ͬʱ�������ӵ���������
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void CRcvGrp::RemoveMem(u8 byMtId, TDStreamCap &tDCap)
{
    u8 byRes = GetDSRes(tDCap.GetMediaType(), tDCap.GetResolution());
    m_tDSGrp.RemoveMem(byMtId, (TDSRcvGrp::emType)byRes);
    return;
}

/*=============================================================================
  �� �� ���� RemoveMem
  ��    �ܣ� Ⱥ������� ����������Ա
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� bExceptEither���Ƿ�ͬʱ�������ӵ���������
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void CRcvGrp::RemoveMem(u8 byMtId, TSimCapSet &tSim)
{
    u8 byRes = GetMVRes(tSim.GetVideoMediaType(), tSim.GetVideoResolution());
    m_tMVGrp.RemoveMem(byMtId, (TMVRcvGrp::emType)byRes);
    return;
}
/*=============================================================================
  �� �� ���� ClearExpt
  ��    �ܣ� ����Ⱥ�� ���������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/02/06    4.5         Ѧ��          ����
=============================================================================*/
void CRcvGrp::ClearExpt(BOOL32 bDual)
{
	m_tMVGrp.ClearExpt();
	if(bDual)
	{
		m_tDSGrp.ClearExpt();
	}
    return;
}
/*=============================================================================
  �� �� ���� GetMVRes
  ��    �ܣ� Ⱥ������� ��ȡ����Ⱥ��λ��
  �㷨ʵ�֣� Ŀǰֻ�����ʽ�ͷֱ��ʣ�֡�ʺ������ݲ�����
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
u8 CRcvGrp::GetMVRes(u8 byType, u8 byRes)
{
    u8 byMVRes = TMVRcvGrp::emBegin;

    switch (byType)
    {
    case MEDIA_TYPE_H264:
        switch (byRes)
        {
        case VIDEO_FORMAT_CIF:  byMVRes = TMVRcvGrp::emCif;     break;
        case VIDEO_FORMAT_4CIF: byMVRes = TMVRcvGrp::em4Cif;    break;
        case VIDEO_FORMAT_HD720:    byMVRes = TMVRcvGrp::em720p;    break;
        case VIDEO_FORMAT_HD1080:   byMVRes = TMVRcvGrp::em1080;    break;
        default:    
            OspPrintf(TRUE, FALSE, "[CRcvGrp][GetMVRes] unexpected h264 Res.%d\n", byRes);
            break;
        }
        break;

    case MEDIA_TYPE_H263:
        byMVRes = TMVRcvGrp::emH263;
        break;
    case MEDIA_TYPE_MP4:
        byMVRes = TMVRcvGrp::emMp4;
        break;
    default:
        OspPrintf(TRUE, FALSE, "[CRcvGrp][GetMVRes] unexpected mtype.%d\n", byType);
        break;
    }
    return byMVRes;
}

void TMVRcvGrp::ResIn2Out(u8 byRes, u8 &byConfRes, u8 &byConfType)
{
    switch (byRes)
    {
    case emCif:  byConfRes = VIDEO_FORMAT_CIF; byConfType = MEDIA_TYPE_H264; break;
    case em4Cif: byConfRes = VIDEO_FORMAT_4CIF; byConfType = MEDIA_TYPE_H264; break;
    case em720p: byConfRes = VIDEO_FORMAT_HD720; byConfType = MEDIA_TYPE_H264; break;
    case em1080: byConfRes = VIDEO_FORMAT_HD1080; byConfType = MEDIA_TYPE_H264; break;
    case emH263: byConfRes = VIDEO_FORMAT_CIF; byConfType = MEDIA_TYPE_H263; break;
    case emMp4:  byConfRes = VIDEO_FORMAT_CIF; byConfType = MEDIA_TYPE_MP4; break;
    	break;
    default:
        OspPrintf(TRUE, FALSE, "[CRcvGrp][ResMVIn2Out] unexpected res.%d\n", byRes);
        break;
    }
}

void TDSRcvGrp::ResIn2Out(u8 byRes, u8 &byConfRes, u8 &byConfType)
{
    switch (byRes)
    {
    case emCif:  byConfRes = VIDEO_FORMAT_CIF; byConfType = MEDIA_TYPE_H264; break;
    case em4Cif: byConfRes = VIDEO_FORMAT_4CIF; byConfType = MEDIA_TYPE_H264; break;
    case em720p: byConfRes = VIDEO_FORMAT_HD720; byConfType = MEDIA_TYPE_H264; break;
    case emUXGA: byConfRes = VIDEO_FORMAT_HD1080; byConfType = MEDIA_TYPE_H264; break;
    case emSXGA: byConfRes = VIDEO_FORMAT_SXGA; byConfType = MEDIA_TYPE_H264; break;
    case emXGA:  byConfRes = VIDEO_FORMAT_XGA; byConfType = MEDIA_TYPE_H264; break;
    case emSVGA: byConfRes = VIDEO_FORMAT_SVGA; byConfType = MEDIA_TYPE_H264; break;
    case emVGA:  byConfRes = VIDEO_FORMAT_VGA; byConfType = MEDIA_TYPE_H264; break;
    case emH263plus: byConfRes = VIDEO_FORMAT_XGA; byConfType = MEDIA_TYPE_H263PLUS; break;
    	break;
    default:
        OspPrintf(TRUE, FALSE, "[CRcvGrp][ResDSIn2Out] unexpected res.%d\n", byRes);
        break;
    }
}

/*=============================================================================
  �� �� ���� GetDSRes
  ��    �ܣ� Ⱥ������� ��ȡ˫��Ⱥ��λ��
  �㷨ʵ�֣� Ŀǰֻ�����ʽ�ͷֱ��ʣ�֡�ʺ������ݲ�����
  ȫ�ֱ����� 
  ��    ���� TSimCapSet &tSim
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
u8 CRcvGrp::GetDSRes(u8 byType, u8 byRes)
{
    u8 byDSRes = TMVRcvGrp::emBegin;
    
    switch (byType)
    {
    case MEDIA_TYPE_H264:
        switch (byRes)
        {
        case VIDEO_FORMAT_CIF:  byDSRes = TDSRcvGrp::emCif;     break;
        case VIDEO_FORMAT_4CIF: byDSRes = TDSRcvGrp::em4Cif;    break;
        case VIDEO_FORMAT_HD720:    byDSRes = TDSRcvGrp::em720p;    break;
        case VIDEO_FORMAT_UXGA:     byDSRes = TDSRcvGrp::emUXGA;    break;
        case VIDEO_FORMAT_SXGA:     byDSRes = TDSRcvGrp::emSXGA;    break;
        case VIDEO_FORMAT_XGA:      byDSRes = TDSRcvGrp::emXGA;     break;
        case VIDEO_FORMAT_SVGA:     byDSRes = TDSRcvGrp::emSVGA;    break;
        case VIDEO_FORMAT_VGA:      byDSRes = TDSRcvGrp::emVGA;     break;
        default:    
            OspPrintf(TRUE, FALSE, "[CRcvGrp][GetDSRes] unexpected h264 Res.%d\n", byRes);
            break;
        }
        break;
	
	case MEDIA_TYPE_H263:	//��ʱ����֧��Polycom
    case MEDIA_TYPE_H263PLUS:
        byDSRes = TDSRcvGrp::emH263plus;
        break;
    default:
        OspPrintf(TRUE, FALSE, "[CRcvGrp][GetDSRes] unexpected mtype.%d\n", byType);
        break;
    }
    return byDSRes;
}

/*=============================================================================
  �� �� ���� GetMVMtList
  ��    �ܣ� Ⱥ������� ��ȡĳ�����ն˵Ĵ������б���������������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/23    4.5         �ű���        ����
=============================================================================*/
void CRcvGrp::GetMVMtList(u8 byType, u8 byRes, u8 &byNum, u8 *pbyMt, BOOL32 bForce)
{
    u8 byResNeed = GetMVRes(byType, byRes);
    m_tMVGrp.GetMtList(byResNeed, byNum, pbyMt, bForce);
    return;
}

/*=============================================================================
  �� �� ���� GetDSMtList
  ��    �ܣ� Ⱥ������� ��ȡĳ�����ն˵Ĵ������б���������������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/23    4.5         �ű���        ����
=============================================================================*/
void CRcvGrp::GetDSMtList(u8 byType, u8 byRes, u8 &byNum, u8 *pbyMt)
{
    u8 byResNeed = GetDSRes(byType, byRes);
    m_tDSGrp.GetMtList(byResNeed, byNum, pbyMt);
    return;
}

/*=============================================================================
  �� �� ���� IsMtNeedAdp
  ��    �ܣ� Ⱥ������� ��ȡĳ�ն��Ƿ�����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/23    4.5         �ű���        ����
=============================================================================*/
BOOL32 CRcvGrp::IsMtNeedAdp(u8 byMtId, BOOL32 bMV)
{
    if (bMV)
        return !m_tMVGrp.IsMtInExcept(byMtId);
    else
        return !m_tDSGrp.IsMtInExcept(byMtId);
}

/*=============================================================================
  �� �� ���� IsMtNeedAdp
  ��    �ܣ� Ⱥ������� ��ȡȺ���Ƿ���Ҫ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/27    4.5         �ű���        ����
=============================================================================*/
BOOL32 CRcvGrp::IsNeedAdp(BOOL32 bMV)
{
    if (bMV)
        return m_tMVGrp.IsNeedAdp();
    else
        return m_tDSGrp.IsNeedAdp();
}


/*=============================================================================
  �� �� ���� GetMtMediaRes
  ��    �ܣ� Ⱥ�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/23    4.5         �ű���        ����
=============================================================================*/
BOOL32 CRcvGrp::GetMtMediaRes(u8 byMtId, u8 &byMediaType, u8 &byRes, BOOL32 bDual)
{
    if (!bDual)
        return m_tMVGrp.GetMtMediaRes(byMtId, byMediaType, byRes);
    else
        return m_tDSGrp.GetMtMediaRes(byMtId, byMediaType, byRes);
}

/*=============================================================================
  �� �� ���� Clear
  ��    �ܣ� Ⱥ������� Clear
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void CRcvGrp::Clear()
{
    m_tMVGrp.Clear();
    m_tDSGrp.Clear();
}

/*=============================================================================
  �� �� ���� Print
  ��    �ܣ� Ⱥ������� Print
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void CRcvGrp::Print()
{
    m_tMVGrp.Print();
    m_tDSGrp.Print();
}

/*=============================================================================
  �� �� ���� AddMem
  ��    �ܣ� ��������Ⱥ�� ���ӳ�Ա
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� bExceptEither���Ƿ�ͬʱ�������ӵ���������
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void TMVRcvGrp::AddMem(u8 byMtId, emType type, BOOL32 bExceptEither)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT ||
        type >= emEnd || type <= emBegin)
    {
        OspPrintf(TRUE, FALSE, "[TMVRcvGrp] [AddMem] unexpected mt.%d or type.%d\n", byMtId, type);
        return;
    }
    m_aabyGrp[type][(byMtId-1)/8] |= 1 << ((byMtId-1) % 8);

    for(u8 byTypeId = emBegin+1; byTypeId < emEnd; byTypeId++)
    {
        if (byTypeId == type)
        {
            continue;
        }
        m_aabyGrp[byTypeId][(byMtId-1)/8] &= ~(1 << ((byMtId-1) % 8));
    }

    if (bExceptEither)
    {
        m_abyExcept[(byMtId-1)/8] |= 1 << ((byMtId-1) % 8);
    }
    else
    {
        m_abyExcept[(byMtId-1)/8] &= ~(1 << ((byMtId-1) % 8));
    }
    return;
}

/*=============================================================================
  �� �� ���� AddExcept
  ��    �ܣ� ��������Ⱥ�� �����������Ա
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void TMVRcvGrp::AddExcept(u8 byMtId)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT)
    {
        OspPrintf(TRUE, FALSE, "[TMVRcvGrp] [AddExcept] unexpected mt.%d\n", byMtId);
        return;
    }
    m_abyExcept[(byMtId-1)/8] |= 1 << ((byMtId-1) % 8);
    return;
}

/*=============================================================================
  �� �� ���� RemoveMem
  ��    �ܣ� ��������Ⱥ�� ɾ����Ա����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void TMVRcvGrp::RemoveMem(u8 byMtId, emType type)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT ||
        type >= emEnd || type <= emBegin)
    {
        OspPrintf(TRUE, FALSE, "[TMVRcvGrp] [RemvoeMem] unexpected mt.%d or type.%d\n", byMtId, type);
        return;
    }
    m_aabyGrp[type][(byMtId-1)/8] &= ~(1 << ((byMtId-1)%8));
    m_abyExcept[(byMtId-1)/8] &= ~(1 << ((byMtId-1)%8));
    return;
}

/*=============================================================================
  �� �� ���� ClearExpt
  ��    �ܣ� ��������Ⱥ�� ���������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/02/06    4.5         Ѧ��          ����
=============================================================================*/
void TMVRcvGrp::ClearExpt()
{
	memset(m_abyExcept,0,sizeof(m_abyExcept));
    return;
}
/*============================================================================
  �� �� ���� IsMtInExcept
  ��    �ܣ� ��������Ⱥ�� �ж��ն��Ƿ�����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
BOOL32 TMVRcvGrp::IsMtInExcept(u8 byMtId)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT)
    {
        OspPrintf(TRUE, FALSE, "[TMVRcvGrp] [IsMtInExcept] unexpected mt.%d\n", byMtId);
        return FALSE;
    }
    return m_abyExcept[(byMtId-1)/8] & (1 << ((byMtId-1)%8));
}

/*=============================================================================
  �� �� ���� IsMtInExcept
  ��    �ܣ� ��������Ⱥ�� �ж��ն���������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/27    4.5         �ű���        ����
=============================================================================*/
BOOL32 TMVRcvGrp::GetMtMediaRes(u8 byMtId, u8 &byMediaType, u8 &byRes)
{
    u8 byResId = 0;
    for (byResId = emBegin+1; byResId < emEnd; byResId ++)
    {
        if (m_aabyGrp[byResId][(byMtId-1)/8] & (1 << ((byMtId-1)%8)))
        {
            break;
        }
    }
    if (byResId != emEnd)
    {
        ResIn2Out(byResId, byRes, byMediaType);
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
  �� �� ���� IsGrpNull
  ��    �ܣ� ��������Ⱥ�� ĳȺ���Ƿ�Ϊ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/01/05    4.5         �ű���        ����
=============================================================================*/
BOOL32 TMVRcvGrp::IsGrpNull(emType type)
{
    if(type == emBegin || type > emEnd)
    {
        OspPrintf(TRUE, FALSE, "[TMVRcvGrp::IsGrpNull] unexpected type.%d\n", type);
        return TRUE;
    }

    for(u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
    {
        if ((m_aabyGrp[type][(byMtId-1)/8] & (1 << ((byMtId-1)%8))))
        {
            return FALSE;
        }
    }
    return TRUE;
}

/*=============================================================================
  �� �� ���� IsNeedAdp
  ��    �ܣ� ��������Ⱥ�� �ж�����Ⱥ���Ƿ���Ҫ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/27    4.5         �ű���        ����
=============================================================================*/
BOOL32 TMVRcvGrp::IsNeedAdp(void)
{
    u8 byType = 0;
    u8 byRes = 0;
    for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
    {
        if (GetMtMediaRes(byMtId, byType, byRes) &&
            !IsMtInExcept(byMtId))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
  �� �� ���� GetMtList
  ��    �ܣ� ��������Ⱥ�� ��ȡ�ն���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void TMVRcvGrp::GetMtList(u8 byRes, u8 &byNum, u8 *pbyMt, BOOL32 bForce)
{
    if (emEnd <= byRes || emBegin == byRes)
    {
        OspPrintf(TRUE, FALSE, "[TMVRcvGrp::GetMtList] unexpected res.%d\n", byRes);
        return;
    }
    for(u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
    {
        if ((m_aabyGrp[byRes][(byMtId-1)/8] & (1 << ((byMtId-1)%8))) 
			&& (!IsMtInExcept(byMtId) || bForce) )
        {
            pbyMt[byNum] = byMtId;
            byNum++;
        }
    }
    return;
}


/*=============================================================================
  �� �� ���� Print
  ��    �ܣ� ��������Ⱥ�� ��ӡ��ǰȺ���Ա
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void TMVRcvGrp::Print()
{
    u8 byRes = 0;
    OspPrintf(TRUE, FALSE, "MVGrp:\n");
    for(byRes = emBegin+1; byRes < emEnd; byRes ++)
    {
        u8 byNum = 0;
        u8 abyMt[MAXNUM_CONF_MT];
        memset(&abyMt, 0, sizeof(abyMt));
        GetMtList(byRes, byNum, &abyMt[0]);
        if (byNum != 0)
        {
            u8 byConfRes = 0;
            u8 byMediaType = 0;
            ResIn2Out(byRes, byConfRes, byMediaType);
            OspPrintf(TRUE, FALSE, "\tRes.%s, Media.%d\n", GetResStr(byConfRes), byMediaType);
        }
        for(u8 byMtIdx = 0; byMtIdx < byNum; byMtIdx++)
        {
            OspPrintf(TRUE, FALSE, "\t    Mt.%d\n", abyMt[byMtIdx]);
        }
        OspPrintf(TRUE, FALSE, "\n");
    }

    OspPrintf(TRUE, FALSE, "\tExcept:\n");
    for(u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
    {
        if (IsMtInExcept(byMtId))
        {
            OspPrintf(TRUE, FALSE, "\t    Mt.%d\n", byMtId);
        }
    }

    OspPrintf(TRUE, FALSE, "\n");
}

/*=============================================================================
  �� �� ���� AddMem
  ��    �ܣ� ˫������Ⱥ�� ���ӳ�Ա
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� bExceptEither���Ƿ�ͬʱ�������ӵ���������
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void TDSRcvGrp::AddMem(u8 byMtId, emType type, BOOL32 bExceptEither)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT ||
        type >= emEnd || type <= emBegin)
    {
        OspPrintf(TRUE, FALSE, "[TDSRcvGrp] [AddMem] unexpected mt.%d or type.%d\n", byMtId, type);
        return;
    }
    m_aabyGrp[type][(byMtId-1)/8] |= 1 << ((byMtId-1) % 8);

    for(u8 byTypeId = emBegin+1; byTypeId < emEnd; byTypeId++)
    {
        if (byTypeId == type)
        {
            continue;
        }
        m_aabyGrp[byTypeId][(byMtId-1)/8] &= ~(1 << ((byMtId-1) % 8));
    }

    if (bExceptEither)
    {
        m_abyExcept[(byMtId-1)/8] |= 1 << ((byMtId-1) % 8);
    }
    else
    {
        m_abyExcept[(byMtId-1)/8] &= ~(1 << ((byMtId-1) % 8));
    }
    return;
}

/*=============================================================================
  �� �� ���� AddExcept
  ��    �ܣ� ˫������Ⱥ�� ��������������Ա
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void TDSRcvGrp::AddExcept(u8 byMtId)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT)
    {
        OspPrintf(TRUE, FALSE, "[TDSRcvGrp] [AddExcept] unexpected mt.%d\n", byMtId);
        return;
    }
    m_abyExcept[(byMtId-1)/8] |= 1 << ((byMtId-1) % 8);
    return;
}

/*=============================================================================
  �� �� ���� AddExcept
  ��    �ܣ� ˫������Ⱥ�� ɾ����Ա������������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void TDSRcvGrp::RemoveMem(u8 byMtId, emType type)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT ||
        type >= emEnd || type <= emBegin)
    {
        OspPrintf(TRUE, FALSE, "[TDSRcvGrp] [RemvoeMem] unexpected mt.%d or type.%d\n", byMtId, type);
        return;
    }
    m_aabyGrp[type][(byMtId-1)/8] &= ~(1 << ((byMtId-1)%8));
    m_abyExcept[(byMtId-1)/8] &= ~(1 << ((byMtId-1)%8));
    return;
}
/*=============================================================================
  �� �� ���� ClearExpt
  ��    �ܣ� ˫������Ⱥ�� ���������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/02/06    4.5         Ѧ��          ����
=============================================================================*/
void TDSRcvGrp::ClearExpt()
{
	memset(m_abyExcept,0,sizeof(m_abyExcept));
    return;
}
/*=============================================================================
  �� �� ���� IsMtInExcept
  ��    �ܣ� ˫������Ⱥ�� �ж��ն��Ƿ�����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
BOOL32 TDSRcvGrp::IsMtInExcept(u8 byMtId)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT)
    {
        OspPrintf(TRUE, FALSE, "[TDSRcvGrp] [IsMtInExcept] unexpected mt.%d\n", byMtId);
        return FALSE;
    }
    return m_abyExcept[(byMtId-1)/8] & (1 << ((byMtId-1)%8));
}

/*=============================================================================
  �� �� ���� GetMtMediaRes
  ��    �ܣ� ˫������Ⱥ�� ��ȡ�ն˵�Ⱥ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
BOOL32 TDSRcvGrp::GetMtMediaRes(u8 byMtId, u8 &byMediaType, u8 &byRes)
{
    u8 byResId = 0;
    for (byResId = emBegin+1; byResId < emEnd; byResId ++)
    {
        if (m_aabyGrp[byResId][(byMtId-1)/8] & (1 << ((byMtId-1)%8)))
        {
            break;
        }
    }
    if (byResId != emEnd)
    {
        ResIn2Out(byResId, byRes, byMediaType);
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
  �� �� ���� IsNeedAdp
  ��    �ܣ� ˫������Ⱥ�� ��ȡ˫������ô
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/27    4.5         �ű���        ����
=============================================================================*/
BOOL32 TDSRcvGrp::IsNeedAdp()
{
    u8 byType = 0;
    u8 byRes = 0;
    for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
    {
        if (GetMtMediaRes(byMtId, byType, byRes) &&
            !IsMtInExcept(byMtId))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
  �� �� ���� GetMtList
  ��    �ܣ� ˫������Ⱥ�� ��ȡ�ն��б�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void TDSRcvGrp::GetMtList(u8 byRes, u8 &byNum, u8 *pbyMt)
{
    if (emEnd <= byRes || emBegin == byRes)
    {
        OspPrintf(TRUE, FALSE, "[TDSRcvGrp::GetMtList] unexpected res.%d\n", byRes);
        return;
    }
    for(u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
    {
        if ((m_aabyGrp[byRes][(byMtId-1)/8] & (1 << ((byMtId-1)%8))) &&
            !IsMtInExcept(byMtId))
        {
            pbyMt[byNum] = byMtId;
            byNum++;
        }
    }
    return;
}

/*=============================================================================
  �� �� ���� IsMtInExcept
  ��    �ܣ� ˫������Ⱥ�� ��ӡ�������Ա������������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/07    4.5         �ű���        ����
=============================================================================*/
void TDSRcvGrp::Print()
{
    OspPrintf(TRUE, FALSE, "DSGrp:\n");
    for(u8 byRes = emBegin+1; byRes < emEnd; byRes ++)
    {
        u8 byNum = 0;
        u8 abyMt[MAXNUM_CONF_MT];
        memset(&abyMt, 0, sizeof(abyMt));
        GetMtList(byRes, byNum, &abyMt[0]);
        if (byNum != 0)
        {
            u8 byConfRes = 0;
            u8 byMediaType = 0;
            ResIn2Out(byRes, byConfRes, byMediaType);
            OspPrintf(TRUE, FALSE, "\tRes.%s, Media.%d\n", GetResStr(byConfRes), byMediaType);
        }
        for(u8 byMtIdx = 0; byMtIdx < byNum; byMtIdx++)
        {
            OspPrintf(TRUE, FALSE, "\t    Mt.%d\n", abyMt[byMtIdx]);
        }
    }
    OspPrintf(TRUE, FALSE, "\n");
    OspPrintf(TRUE, FALSE, "\tExcept:\n");
    for(u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
    {
        if (IsMtInExcept(byMtId))
        {
            OspPrintf(TRUE, FALSE, "\t    Mt.%d\n", byMtId);
        }
    }
    OspPrintf(TRUE, FALSE, "\n");
    return;
}

/*=============================================================================
  �� �� ���� GetApplyFreqInfo
  ��    �ܣ� ��ȡ���ǻ��������Ƶ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  09/08/28    4.6         �ű���        ����
=============================================================================*/
void CMcuVcData::GetApplyFreqInfo(u32 &dwSatIp, u16 &wSatPort)
{
    if (0 == m_tSatInfo.GetApplyIp() || 0 == m_tSatInfo.GetApplyPort())
    {
        OspPrintf(TRUE, FALSE, "[GetApplyFreqInfo] sat apply addr unexist!\n");
        return;
    }
    dwSatIp = htonl(m_tSatInfo.GetApplyIp());
    wSatPort = m_tSatInfo.GetApplyPort();
    return;
}


/*====================================================================
    ������      SetConfAllocFreq
    ����       ���û������Ƶ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
    09/08/28    4.6         �ű���          ��������ֲ����
====================================================================*/
void CMcuVcData::SetConfGetSendFreq( u8 byConfIdx, BOOL bTrue )
{
	if ( bTrue )
	{
		m_abySendConfFreq[byConfIdx-1] = 1;
	}
	else
	{
		m_abySendConfFreq[byConfIdx-1] = 0;
	}
}

/*====================================================================
    ������      SetConfAllocFreq
    ����       ���û������Ƶ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
    09/08/28    4.6         �ű���          ��������ֲ����
====================================================================*/
void CMcuVcData::SetConfGetReceiveFreq( u8 byConfIdx, BOOL bTrue )
{
	if ( bTrue )
	{
		m_abyReceiveConfFreq[byConfIdx-1] = 1;
	}
	else
	{
		m_abyReceiveConfFreq[byConfIdx-1] = 0;
	}
}

/*====================================================================
    ������      IsConfAllocFreq
    ����       �������Ƿ����Ƶ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
    09/08/28    4.6         �ű���          ��������ֲ����
====================================================================*/
BOOL CMcuVcData::IsConfGetAllFreq( u8 byConfIdx )
{
	return ( IsConfGetSendFreq( byConfIdx ) && IsConfGetReceiveFreq( byConfIdx ) );
}

/*====================================================================
    ������      IsConfAllocFreq
    ����       �������Ƿ����Ƶ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
    09/08/28    4.6         �ű���          ��������ֲ����
====================================================================*/
BOOL CMcuVcData::IsConfGetSendFreq( u8 byConfIdx )
{
	return ( m_abySendConfFreq[byConfIdx-1] == 1 );
}
/*====================================================================
    ������      IsConfAllocFreq
    ����       �������Ƿ����Ƶ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
    09/08/28    4.6         �ű���          ��������ֲ����
====================================================================*/
BOOL CMcuVcData::IsConfGetReceiveFreq( u8 byConfIdx )
{
	return ( m_abyReceiveConfFreq[byConfIdx-1] == 1 );
}


/*====================================================================
  ������      SetConfBitRate
  ����       ���鷢��/��������
  �㷨ʵ��    ��
  ����ȫ�ֱ�����
  �������˵����

  ����ֵ˵��  ����
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  04/04/06    1.0         zhangsh         ����
  09/08/28    4.6         �ű���          ��������ֲ����
====================================================================*/
void CMcuVcData::SetConfBitRate( u8 byConfIdx, u32 dwSend, u32 dwRecv )
{
    for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
    {
        if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
        {
            if ( dwSend != 0 )
			{
                m_atConfFreq[byLoop].dwSendBitRate = dwSend;
			}
            if ( dwRecv != 0 )
			{
                m_atConfFreq[byLoop].dwRevBitRate = dwRecv;
			}
            return;
        }
    }
    return ;
}


/*====================================================================
    ������      SetConfInfo
    ����       �õ�����Ƶ�ʣ�������Ϣ�Ŀսṹ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
    09/08/28    4.6         �ű���          ��������ֲ����
====================================================================*/
void CMcuVcData::SetConfInfo( u8 byConfIdx )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == 0 )
		{
			m_atConfFreq[byLoop].byConfIdx = byConfIdx;
			break;
		}
	}
	return;
}

/*====================================================================
    ������      ReleaseConfInfo
    ����       �ͷŻ���Ƶ�ʣ�������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
    09/08/28    4.6         �ű���          ��������ֲ����
====================================================================*/
void CMcuVcData::ReleaseConfInfo( u8 byConfIdx )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			memset( &m_atConfFreq[byLoop], 0, sizeof( TConfFreqBitRate ) );
			break;
		}
	}
	return;
}

/*====================================================================
    ������      SetConfFreq
    ����       ���鷢��/����Ƶ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
    09/08/28    4.6         �ű���          ��������ֲ����
====================================================================*/
void CMcuVcData::SetConfFreq( u8 byConfIdx, u32 dwSend, u8 byPos, u32 dwRecv )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			if ( dwSend != 0 )
			{
				m_atConfFreq[byLoop].dwSendFreq = dwSend;
			}
			if ( dwRecv != 0 )
			{
				m_atConfFreq[byLoop].dwRevFreq[byPos] = dwRecv;
			}
			return;
		}
	}
	return ;
}

/*====================================================================
    ������      GetConfSndFreq
    ����       �õ����鷢��Ƶ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
    09/08/28    4.6         �ű���          ��������ֲ����
====================================================================*/
u32 CMcuVcData::GetConfSndFreq( u8 byConfIdx )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			return m_atConfFreq[byLoop].dwSendFreq;
		}
	}
	return 0;
}

/*====================================================================
    ������      GetConfRcvFreq
    ����       �õ��������Ƶ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
    09/08/28    4.6         �ű���          ��������ֲ����
====================================================================*/
u32 CMcuVcData::GetConfRcvFreq( u8 byConfIdx, u8 byPos )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			return m_atConfFreq[byLoop].dwRevFreq[byPos];
		}
	}
	return 0;
}

/*====================================================================
    ������      GetConfSndBitRate
    ����       �õ����鷢������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
    09/08/28    4.6         �ű���          ��������ֲ����
====================================================================*/
u32 CMcuVcData::GetConfSndBitRate( u8 byConfIdx )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			return m_atConfFreq[byLoop].dwSendBitRate;
		}
	}
	return 0;
}
/*====================================================================
    ������      GetConfRcvBitRate
    ����       �õ������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
    09/08/28    4.6         �ű���          ��������ֲ����
====================================================================*/
u32 CMcuVcData::GetConfRcvBitRate( u8 byConfIdx )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			return m_atConfFreq[byLoop].dwRevBitRate;
		}
	}
	return 0;
}

/*====================================================================
    ������      GetConfRcvNum
    ����       �õ��������Ƶ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
    09/08/28    4.6         �ű���          ��������ֲ����
====================================================================*/
u8 CMcuVcData::GetConfRcvNum( u8 byConfIdx )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			return m_atConfFreq[byLoop].byNum;
		}
	}
	return 0;
}

/*====================================================================
    ������      SetConfRcvNum
    ����       ���û������Ƶ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
    09/08/28    4.6         �ű���          ��������ֲ����
====================================================================*/
void CMcuVcData::SetConfRcvNum( u8 byConfIdx, u8 byTotal )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			m_atConfFreq[byLoop].byNum = byTotal;
		}
	}
	return ;
}

/*====================================================================
    ������      GetRcvMtSignalPort
    ����        ��ȡ�����ն˲�����Э������Ķ˿�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/08/28    4.6         �ű���          ����
====================================================================*/
u16 CMcuVcData::GetRcvMtSignalPort( void ) const
{
    return m_tSatInfo.GetRcvMtSignalPort();
}

u16 CMcuVcData::GetApplyRcvPort( void ) const
{
    return m_tSatInfo.GetApplyRcvPort();
}

u32 CMcuVcData::GetTimeRefreshIpAddr( void ) const
{
    return m_tSatInfo.GetTimeRefreshIpAddr();
}

u16 CMcuVcData::GetTimeRefreshPort( void ) const
{
    return m_tSatInfo.GetTimeRefreshPort();
}


/*====================================================================
    ������      SetMcuModemConnected
    ����       Mcu Modem��������״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
====================================================================*/
void CMcuVcData::SetMcuModemConnected( u16 wModemId, BOOL bConnected )
{
	if ( wModemId == 0 || wModemId > MAXNUM_MCU_MODEM )
		return;
	m_atModemData[wModemId - 1 ].bConnected = bConnected;
	return;
}
/*====================================================================
    ������      GetIdleMcuModemForSnd
    ����       ���Է��͵�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
====================================================================*/
u8 CMcuVcData::GetIdleMcuModemForSnd()
{
	for ( u8 byLoop = 1 ; byLoop < MAXNUM_MCU_MODEM+1 ; byLoop ++ )
	{
		if ( m_atModemData[byLoop-1].wConfIdxSnd == 0 && m_atModemData[byLoop-1].bConnected == TRUE )
		{
			return byLoop;
		}
	}
	return 0;
}
/*====================================================================
    ������      ReleaseConfModem
    ����       �ͷŻ����е�MODEM
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
====================================================================*/
void CMcuVcData::ReleaseSndMcuModem( u8 byModemId )
{
	if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM )
		return;
	m_atModemData[byModemId-1].wConfIdxSnd = 0;
	m_atModemData[byModemId-1].tMtSnd.SetNull();
	m_atModemData[byModemId-1].bConnected = TRUE;
}


/*====================================================================
    ������      GetIdleMcuModemForRcv
    ����       �õ�һ��MCU��δʹ��MODEM������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
====================================================================*/
u8 CMcuVcData::GetIdleMcuModemForRcv()
{
	for ( u8 byLoop = 1 ; byLoop < MAXNUM_MCU_MODEM+1 ; byLoop ++ )
	{
		if ( m_atModemData[byLoop-1].wConfIdxRcv == 0 && m_atModemData[byLoop-1].bConnected == TRUE )
		{
			return byLoop;
		}
	}
	return 0;
}
/*====================================================================
    ������      ReleaseRcvMcuModem
    ����       �ͷŻ����е�MODEM
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
====================================================================*/
void CMcuVcData::ReleaseRcvMcuModem( u8 byModemId )
{
	if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM )
		return;
	m_atModemData[byModemId-1].wConfIdxRcv = 0;
	//m_atModemData[byModemId-1].wMtId = 0;
	m_atModemData[byModemId-1].tMtRcv.SetNull();
	m_atModemData[byModemId-1].bConnected = TRUE;
}

/*====================================================================
    ������      SetMcuModemSndData
    ����       ����MCU MDOEM������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
====================================================================*/
void CMcuVcData::SetMcuModemSndData( u8 byModemId, u8 byConfIdx, u32 dwSendFreq, u32 dwSendBit)
{
	if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM )
		return;
	m_atModemData[byModemId-1].wConfIdxSnd = byConfIdx;
	m_atModemData[byModemId-1].dwSendBitRate = dwSendBit;
	m_atModemData[byModemId-1].dwSendFrequece = dwSendFreq;
	return;
}

/*====================================================================
    ������      GetMcuModemRcvData
    ����       �õ�һ��MCU��MODEM������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
====================================================================*/
void CMcuVcData::GetMcuModemSndData( u8 byModemId , u32 &dwRevFreq, u32 &dwRevBit )
{
	if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM )
	{
		dwRevFreq = 0;
		dwRevBit = 0;
		return;
	}
	dwRevBit = m_atModemData[byModemId-1].dwSendBitRate;
	dwRevFreq= m_atModemData[byModemId-1].dwSendFrequece ;
	return;
}
/*====================================================================
    ������      SetMcuModemRcvData
    ����       ����MCU MDOEM������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
====================================================================*/
void CMcuVcData::SetMcuModemRcvData( u8 byModemId, u8 byConfIdx, u32 dwRevFreq, u32 dwRevBit )
{
	if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM )
    {
		return;
    }
	m_atModemData[byModemId-1].wConfIdxRcv = byConfIdx;
	m_atModemData[byModemId-1].dwRevBitRate = dwRevBit;
	m_atModemData[byModemId-1].dwRevFrequece = dwRevFreq;
	return;
}

/*====================================================================
    ������      GetMcuModemRcvData
    ����       �õ�һ��MCU��MODEM������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
====================================================================*/
void CMcuVcData::GetMcuModemRcvData( u8 byModemId , u32 &dwRevFreq, u32 &dwRevBit )
{
	if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM )
	{
		dwRevFreq = 0;
		dwRevBit = 0;
		return;
	}
	dwRevBit = m_atModemData[byModemId-1].dwRevBitRate;
	dwRevFreq= m_atModemData[byModemId-1].dwRevFrequece ;
	return;
}

/*====================================================================
    ������      SetModemSportNum
    ����       :
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
====================================================================*/
void CMcuVcData::SetModemSportNum(u8 byModemId, u8 byNum )
{
    if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM+1 )
	{
        return;
	}
    m_atModemData[byModemId-1].bySPortNum = byNum;
    return ;
}

/*====================================================================
    ������      GetModemSportNum
    ����       :
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
====================================================================*/
u8 CMcuVcData::GetModemSportNum( u8 byModemId )
{
    if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM+1 )
	{
        return 0;
	}
    return m_atModemData[byModemId-1].bySPortNum;
}

/*====================================================================
    ������      GetConfRcvModem
    ����       :
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
====================================================================*/
u8 CMcuVcData::GetConfRcvModem(u8 byConfIdx )
{
	for ( u8 byLoop = 1; byLoop <= MAXNUM_MCU_MODEM ; byLoop ++ )
	{
		if ( m_atModemData[byLoop-1].wConfIdxRcv == byConfIdx )
		{
			return byLoop;
		}
	}
	return 0;
}

/*====================================================================
    ������      GetConfSndModem
    ����       :
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
				  
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/06    1.0         zhangsh         ����
====================================================================*/
u8 CMcuVcData::GetConfSndModem(u8 byConfIdx )
{
	for ( u8 byLoop = 1; byLoop <= MAXNUM_MCU_MODEM; byLoop ++ )
	{
		if ( m_atModemData[byLoop-1].wConfIdxSnd == byConfIdx )
		{
			return byLoop;
		}
	}
	return 0;
}

/*====================================================================
  ������      DisplayModemInfo
  ����       ��ʾMCU MODEM��Ϣ
  �㷨ʵ��    ��
  ����ȫ�ֱ�����
  �������˵����

  ����ֵ˵��  ����
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  04/04/06    1.0         zhangsh         ����
====================================================================*/
void CMcuVcData::DisplayModemInfo()
{
    OspPrintf( TRUE, FALSE, "ConfIdx(Snd)\tConfIdx(Rcv)\tRcvType\tRcvMt/EqpId\tSndType\tSndMt/EqpId\tSndFrq\t\tRcvFrq\t\tSendBR\t\tRcvBR\t\tSPort\n" );
   
    for ( u8 byLoop = 0; byLoop < MAXNUM_MCU_MODEM ; byLoop ++ )
    {
        OspPrintf( TRUE, FALSE,"%u \t\t%u \t\t%u \t%u \t%9u \t%9u \t%9u \t%9u\n",
                                m_atModemData[byLoop].wConfIdxSnd,
                                m_atModemData[byLoop].wConfIdxRcv,
								m_atModemData[byLoop].tMtRcv.GetType(),
                                m_atModemData[byLoop].tMtRcv.GetMtId(),
								m_atModemData[byLoop].tMtSnd.GetType(),
								m_atModemData[byLoop].tMtSnd.GetMtId(),
                                m_atModemData[byLoop].dwSendFrequece,
                                m_atModemData[byLoop].dwRevFrequece,
                                m_atModemData[byLoop].dwSendBitRate,
                                m_atModemData[byLoop].dwRevBitRate,
                                m_atModemData[byLoop].bySPortNum ); 
    }
    return;
}

void CMcuVcData::ShowConfFreqInfo()
{
	for (u8 byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx ++)
	{
		if (m_atConfFreq[byIdx].byConfIdx != 0)
		{
			m_atConfFreq[byIdx].Print();
		}
	}
}

void CMcuVcData::ShowSatInfo()
{
	m_tSatInfo.Print();
}


/*====================================================================
  ������      SetMtModemConnected
  ����       ����MODEM����״̬
  �㷨ʵ��    ��
  ����ȫ�ֱ�����
  �������˵����

  ����ֵ˵��  ����
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  04/04/06    1.0         zhangsh         ����
====================================================================*/
void CMcuVcData::SetMtModemConnected( u16 wMtId, BOOL bConnected)
{
    if( wMtId == 0 || wMtId > MAXNUM_CONF_MT )
    {
        return;
    }	
    m_abyMtModemConnected[wMtId-1] = bConnected;
    return;
}

/*====================================================================
  ������      IsMtModemConnected
  ����       MODEM����
  �㷨ʵ��    ��
  ����ȫ�ֱ�����
  �������˵����

  ����ֵ˵��  ����
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  04/04/06    1.0         zhangsh         ����
====================================================================*/
BOOL CMcuVcData::IsMtModemConnected( u16 wMtId )
{
    if( wMtId == 0 || wMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }	
    return m_abyMtModemConnected[wMtId-1];
    
}

/*====================================================================
  ������      SetMtModemStatus
  ����       ����Modem״̬
  �㷨ʵ��    ��
  ����ȫ�ֱ�����
  �������˵����

  ����ֵ˵��  ����
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  04/04/06    1.0         zhangsh         ����
====================================================================*/
void CMcuVcData::SetMtModemStatus( u16 wMtId, BOOL bError )
{
    if( wMtId == 0 || wMtId > MAXNUM_CONF_MT )
    {
        return;
    }	
    m_abyMtModemStatus[wMtId-1] = bError;
    return;
}

u32 CMcuVcData::GetMcuMulticastDataIpAddr( void )
{
	return m_tSatInfo.GetMcuMulticastDataIpAddr();
}

u16 CMcuVcData::GetMcuMulticastDataPort(void)
{
	return m_tSatInfo.GetMcuMulticastDataPort();
}

u16 CMcuVcData::GetMcuRcvMtMediaStartPort(void)
{
	return m_tSatInfo.GetMcuRcvMtMediaStartPort();
}

u16 CMcuVcData::GetMtRcvMcuMediaPort(void)
{
	return m_tSatInfo.GetMtRcvMcuMediaPort();
}


u32 CMcuVcData::GetMcuSecMulticastIpAddr( void )
{
	return m_tSatInfo.GetMcuSecMulticastIpAddr();
}


/*====================================================================
  ������      SetMtOrEqpUseMcuModem
  ����        ���ն�ʹ��ĳ��MCU��MODEM���н���/���� 
  �㷨ʵ��    ��
  ����ȫ�ֱ�����
  �������˵����

  ����ֵ˵��  ����
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  04/04/06    1.0         zhangsh         ����
  09/11/18    4.6         zhangbq         ���Ӷ෢�ʹ���
====================================================================*/
void CMcuVcData::SetMtOrEqpUseMcuModem(TMt tMt, u8 byModem, BOOL bRcv, BOOL bUsed)
{
	if ( byModem == 0 || byModem > MAXNUM_MCU_MODEM )
		return;
	
	if (bRcv)
	{
		if (  bUsed )
		{
			m_atModemData[byModem - 1].tMtRcv = tMt;;
		}
		else
		{
			m_atModemData[byModem - 1].tMtRcv.SetNull();
		}
	}
	else
	{
		if (  bUsed )
		{
			m_atModemData[byModem - 1].tMtSnd = tMt;;
		}
		else
		{
			m_atModemData[byModem - 1].tMtSnd.SetNull();
		}
	}
	return;
}


/*====================================================================
  ������      GetConfMtUsedMcuModem
  ����       �õ�һ��MCU��MODEM�������ն�
  �㷨ʵ��    ��
  ����ȫ�ֱ�����
  �������˵����

  ����ֵ˵��  ����
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  04/04/06    1.0         zhangsh         ����
  09/11/18    4.6         zhangbq         ���Ӷ෢�ʹ���
====================================================================*/
u8 CMcuVcData::GetConfMtUsedMcuModem(u8 byConfIdx, TMt tMt, BOOL bRcv)
{
	for ( u8 byLoop = 1; byLoop <= MAXNUM_MCU_MODEM ; byLoop ++ )
	{
		if (bRcv)
		{
			if ( m_atModemData[byLoop-1].wConfIdxRcv == byConfIdx &&
				 m_atModemData[byLoop-1].tMtRcv.GetType() == tMt.GetType() &&
				 m_atModemData[byLoop-1].tMtRcv.GetMtId() == tMt.GetMtId() )
			{
				return byLoop;
			}
		}
		else
		{
			if ( m_atModemData[byLoop-1].wConfIdxSnd == byConfIdx &&
				 m_atModemData[byLoop-1].tMtSnd.GetType() == tMt.GetType() &&
				 m_atModemData[byLoop-1].tMtSnd.GetMtId() == tMt.GetMtId() )
			{
				return byLoop;
			}
		}
	}
	return 0;
}

/*====================================================================
  ������      ReleaseConfUsedMcuModem
  ����       �õ�һ��MCU��MODEM�������ն�
  �㷨ʵ��    ��
  ����ȫ�ֱ�����
  �������˵����

  ����ֵ˵��  ����
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  04/04/06    1.0         zhangsh         ����
  09/11/18    4.6         zhangbq         ���Ӷ�㲥����
====================================================================*/
u8 CMcuVcData::ReleaseConfUsedMcuModem(u8 byConfIdx, TMt tMt, BOOL bRcv)
{
	for ( u8 byLoop = 1; byLoop < MAXNUM_MCU_MODEM+1 ; byLoop ++ )
	{
		if (bRcv)
		{
			if (m_atModemData[byLoop-1].wConfIdxRcv == byConfIdx &&
				m_atModemData[byLoop-1].tMtRcv.GetMtId() == tMt.GetMtId() &&
				m_atModemData[byLoop-1].tMtRcv.GetType() == tMt.GetType() )
			{
				m_atModemData[byLoop-1].tMtRcv.SetNull();
				return byLoop;
			}
		}
		else
		{
			if (m_atModemData[byLoop-1].wConfIdxSnd == byConfIdx &&
				m_atModemData[byLoop-1].tMtSnd.GetMtId() == tMt.GetMtId() &&
				m_atModemData[byLoop-1].tMtSnd.GetType() == tMt.GetType() )
			{
				m_atModemData[byLoop-1].tMtSnd.SetNull();
				return byLoop;
			}
		}
	}
	return 0;
}

/*====================================================================
  ������      GetConfIdleMcuModem
  ����       �õ�һ��MCU��MODEM������/��������
  �㷨ʵ��    ��
  ����ȫ�ֱ�����
  �������˵����

  ����ֵ˵��  ����
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  04/04/06    1.0         zhangsh         ����
  09/11/18    4.6         zhangbq         ���Ӷ�㲥����
====================================================================*/
u8 CMcuVcData::GetConfIdleMcuModem(u8 byConfIdx, BOOL bRcv)
{
	for ( u8 byLoop = 1; byLoop <= MAXNUM_MCU_MODEM ; byLoop ++ )
	{
		if (bRcv)
		{
			if (m_atModemData[byLoop-1].wConfIdxRcv == byConfIdx &&
				m_atModemData[byLoop-1].tMtRcv.GetMtId() == 0 )
			{
				return byLoop;
			}
		}
		else
		{
			if (m_atModemData[byLoop-1].wConfIdxSnd == byConfIdx &&
				m_atModemData[byLoop-1].tMtSnd.GetMtId() == 0 )
			{
				return byLoop;
			}
		}
	}
	return 0;
}

void CMcuVcData::SetConfRefreshTimes(u8 byConfIdx, u8 byTimes)
{
	if (byConfIdx == 0 || byConfIdx > MAX_CONFIDX)
	{
		return;
	}
	m_abyConfRefreshTimes[byConfIdx-1] = byTimes;
}

u8 CMcuVcData::GetConfRefreshTimes(u8 byConfIdx)
{
	if (byConfIdx == 0 || byConfIdx > MAX_CONFIDX)
	{
		return 0;
	}
	return m_abyConfRefreshTimes[byConfIdx-1];
}

BOOL CMcuVcData::IsConfRefreshNeeded( void )
{
	for( u8 byConfIdx = 1; byConfIdx <= MAX_CONFIDX; byConfIdx ++)
	{
		if (GetConfRefreshTimes(byConfIdx) > 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//END FILE
