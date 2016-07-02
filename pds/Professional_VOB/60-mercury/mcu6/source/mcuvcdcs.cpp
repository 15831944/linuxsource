/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcuvcdcs.cpp
   ����ļ�    : mcuvc.h
   �ļ�ʵ�ֹ���: MCU��DCSҵ�񽻻�����
   ����        : �ű���
   �汾        : V4.0  Copyright(C) 2005-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/12/14  4.0		   �ű���	   ����                                   
******************************************************************************/

#include "evmcudcs.h"
#include "evmcumcs.h"
#include "evmcueqp.h"
#include "evmcu.h"
#include "mcuvc.h"
//#include "mcudata.h"
//#include "mcustruct.h"
//#include "mcudcspdu.h"

/*=============================================================================
  �� �� ���� DaemonProcMcuDcsConnectedNtf
  ��    �ܣ� (1) ����DCSע��ɹ���Ϣ
			 (2) 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/14    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::DaemonProcMcuDcsConnectedNtf( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TDcsInfo tDcsInfo = *(TDcsInfo*)cServMsg.GetMsgBody();
	
	//ȡDCS��ID��
	if ( 0 == tDcsInfo.m_byDcsId )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "[DcsNtf] the DCS id should not be NULL, ignore it !\n");
		return;
	}
	u8 byDcsId = tDcsInfo.m_byDcsId;	

	//��DCS�ṹ����
	BOOL32 bNoSpace = TRUE;
    s32 nIndex = 0;
	for( ; nIndex < MAXNUM_MCU_DCS; nIndex ++ )
	{
		if ( g_cMcuVcApp.m_atPeriDcsTable[nIndex].IsNull() )
		{
			g_cMcuVcApp.m_atPeriDcsTable[nIndex].m_byDcsId = byDcsId;
			bNoSpace = FALSE;
			break;
		}
	}
	//û�пռ�, �쳣
	if ( TRUE == bNoSpace || nIndex >= MAXNUM_MCU_DCS )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "[DcsNtf] there is no space for new DCS.%d which is impossible, check it !\n", byDcsId );
		return;
	}
	
	//��ȡ��ǰע��DCS�ĵ�ַ�Ͷ˿�
	g_cMcuVcApp.m_atPeriDcsTable[nIndex].m_dwDcsIp  = tDcsInfo.m_dwDcsIp;
	g_cMcuVcApp.m_atPeriDcsTable[nIndex].m_wDcsPort = tDcsInfo.m_wDcsPort;	
	
	//ˢ��DCS��ǰ��״̬
	g_cMcuVcApp.SetPeriDcsConnected( g_cMcuVcApp.m_atPeriDcsTable[nIndex].m_byDcsId, TRUE );
	
	//�����õ�DCS, ��״̬(Ŀǰ����ֻ֧��һ��)
	if ( tDcsInfo.m_dwDcsIp == ntohl( g_cMcuAgent.GetDcsIp()) ) 
	{
		g_cMcuVcApp.SetPeriDcsValid( byDcsId );
	}
	else
	{
		ConfPrint( LOG_LVL_ERROR, MID_MCU_CONF, "Dcs %d register failed due to no config !\n", byDcsId );
		return;
	}

/*
	//����֧�ֶ��DCS
	BOOL32 bValid = FALSE;
	u32 adwDcsIp[MAXNUM_MCU_DCS];
	adwDcsIp = ntohl( g_cMcuAgent.GetDcsIp() );
	for( s32 nIndex = 0; nIndex < MAXNUM_MCU_DCS; nIndex ++ )
	{
		if ( adwDcsIp[nIndex] == tDcsData.m_dwDcsIp )
		{
			g_cMcuVcApp.SetPeriDcsValid( byDcsId );
			bValid = TRUE;
			break;
		}
	}
	if ( FALSE == bValid )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get dcs ip: %s from agent failed !\n", tDcsData.m_dwDcsIp );
		return;
	}
    
    //����DCS����(����ת��DCS�б��ʼ��ʱ����)
	g_cMcuVcApp.SetDcsAlias( g_cMcuVcApp.m_atPeriDcsTable[nIndex].m_byDcsId );
*/
	
    //�ϱ����
    TPeriDcsStatus tDcsStatus;
    if (g_cMcuVcApp.GetPeriDcsStatus(byDcsId, &tDcsStatus)) 
    {
        CServMsg cTempServMsg;
        cTempServMsg.SetEventId(MCU_MCS_MCUPERIDCSSTATUS_NOTIF);
        cTempServMsg.SetMsgBody( (u8*)&tDcsStatus, sizeof(TPeriDcsStatus) );
        SendMsgToAllMcs( cTempServMsg.GetEventId(), cTempServMsg );
    }

    // N+1ģʽ��֪ͨ��mcu���ݻ���������� [12/21/2006-zbq]
    if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_MASTER_CONNECTED)
    {
		//��֯packed EqpCap����[1/11/2012 chendaiwei]
		//����Ŀǰpack��ʽ��pack��Buffer���ֵ
		u8 abyBuffer[NPLUS_PACK_EXINFO_BUF_LEN] = {0};
		u16 wLen = 0;
		g_cNPlusApp.PackNplusExInfo(g_cNPlusApp.GetMcuEqpCapEx(),abyBuffer,wLen);		
        TNPlusEqpCap tEqpCap = g_cNPlusApp.GetMcuEqpCap();
        cServMsg.SetEventId(MCU_NPLUS_EQPCAP_NOTIF);
        cServMsg.SetMsgBody((u8 *)&tEqpCap, sizeof(tEqpCap));
		cServMsg.CatMsgBody((u8*)&abyBuffer[0],wLen);
        g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }

    ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Dcs 0x%x register success !\n", tDcsInfo.m_dwDcsIp );
    return;

}

/*=============================================================================
  �� �� ���� DaemonProcMcuDcsDisconnectedNtf
  ��    �ܣ� (1) ��յ�ǰ״̬
			 (2) ֪ͨ���еĻ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/15    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::DaemonProcMcuDcsDisconnectedNtf( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TDcsInfo tDcsInfo = *(TDcsInfo*)cServMsg.GetMsgBody();
	
	//ȡDCS��ID��
	if ( 0 == tDcsInfo.m_byDcsId )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "[DcsNtf] the DCS id should not be NULL, ignore it !\n");
		return;
	}
	u8 byDcsId = tDcsInfo.m_byDcsId;	
	
	//ȡ��ַ�Ͷ˿�
	u32 dwDcsDisconnectIp  = tDcsInfo.m_dwDcsIp;
	//u16 wDcsDisconnectPort = tDcsInfo.m_wDcsPort;
    
	//���Ҷ�Ӧ�Ľṹ��Ա, ���״̬
	for( s32 nIndex = 0; nIndex < MAXNUM_MCU_DCS; nIndex ++ )
	{
		if ( !g_cMcuVcApp.m_atPeriDcsTable[nIndex].IsNull() )
		{
			if ( byDcsId == g_cMcuVcApp.m_atPeriDcsTable[nIndex].m_byDcsId &&
				 dwDcsDisconnectIp == g_cMcuVcApp.m_atPeriDcsTable[nIndex].m_dwDcsIp  )
			{
                g_cMcuVcApp.SetPeriDcsConnected( byDcsId, FALSE );
				g_cMcuVcApp.m_atPeriDcsTable[nIndex].SetNull();
				break;
			}
		}
	}

	//����Ϣ֪ͨ���л���
	cServMsg.Init();
	cServMsg.SetEventId( MCU_DCSDISCONNECTED_NOTIF );
	cServMsg.SetMsgBody( &byDcsId, sizeof(u8) );
	g_cMcuVcApp.BroadcastToAllConf( MCU_DCSDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );

    //�ϱ����
    TPeriDcsStatus tDcsStatus;
    if (g_cMcuVcApp.GetPeriDcsStatus(byDcsId, &tDcsStatus)) 
    {
        CServMsg cTempServMsg;
        cTempServMsg.SetEventId(MCU_MCS_MCUPERIDCSSTATUS_NOTIF);
        cTempServMsg.SetMsgBody( (u8*)&tDcsStatus, sizeof(TPeriDcsStatus) );
        SendMsgToAllMcs( cTempServMsg.GetEventId(), cTempServMsg );
    }  

    // N+1ģʽ��֪ͨ��mcu���ݻ���������� [12/21/2006-zbq]
    if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_MASTER_CONNECTED)
    {
		//��֯packed EqpCap����[1/11/2012 chendaiwei]
		//����Ŀǰpack��ʽ��pack��Buffer���ֵ
		u8 abyBuffer[NPLUS_PACK_EXINFO_BUF_LEN] = {0};
		u16 wLen = 0;
		g_cNPlusApp.PackNplusExInfo(g_cNPlusApp.GetMcuEqpCapEx(),abyBuffer,wLen);	
        TNPlusEqpCap tEqpCap = g_cNPlusApp.GetMcuEqpCap();
        cServMsg.SetEventId(MCU_NPLUS_EQPCAP_NOTIF);
        cServMsg.SetMsgBody((u8 *)&tEqpCap, sizeof(tEqpCap));
		cServMsg.CatMsgBody((u8*)&abyBuffer[0],wLen);
        g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }
    
    return;
}

/*=============================================================================
  �� �� ���� DaemonProcDcsMcuReleaseConfRsp
  ��    �ܣ� ��Daemon�ﴦ��DCS������Ӧ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/16    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::DaemonProcDcsMcuReleaseConfRsp( const CMessage * pcMsg )
{
	//��Ϣ���
	if ( NULL == pcMsg )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get release conf rsp msg failed! \n");
		return;
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs release conf rsp msg: %d<%s> received in inst %d !\n", 
			pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
	}
	
	//��Ϣ����
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	switch( pcMsg->event )
	{
	case DCSSSN_MCU_RELEASECONF_ACK:
		{
			//�ϱ� MCS  DEL 
		}
		break;

	case DCSSSN_MCU_RELEASECONF_NACK:
		{
			//��������
			u8 byConfIdx = *( cServMsg.GetMsgBody() );

			//��������
			u8 byErrorId = *( cServMsg.GetMsgBody() + sizeof(u8) );
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "release data conf %d failed due to reason: %d !\n", byConfIdx, byErrorId );

			//�ϱ� MCS  DEL
		}
		break;

	case DCSSSN_MCU_CONFRELEASED_NOTIF:
		{
			//���״̬
			m_byDcsIdx = 0;
            m_tConfInStatus.SetDataConfOngoing(FALSE);

			//��������
			u8 byConfIdx = *( cServMsg.GetMsgBody() );

			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Data conf %d released succeed on Dcs !\n", byConfIdx );
			
			//�ϱ� MCS ...DEL
		}
		break;

	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected dcs release conf rsp msg received, ignore it !\n");
		break;
	}
}

/*=============================================================================
  �� �� ���� ProcMcuDcsCreateConfReq
  ��    �ܣ� ����DCS�Ự��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/16    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::SendMcuDcsCreateConfReq()
{	
	//��������
	switch( m_tConf.GetConfAttrb().GetDataMode() )
	{
	//ֻ֧������Ƶ
	case CONF_DATAMODE_VAONLY:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "Conf data mode should not be va only !\n" );
		break;
		
	//֧�����ݺ�����Ƶ
	case CONF_DATAMODE_VAANDDATA:	
		{		
			//û��DCS����
			if ( FALSE == FindOnlineDcs() )
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "data conf: %s created failed due to no DCS registered yet !\n", m_tConf.GetConfName() );
				
				//�ȴ�DCS����, ���´���
				SetTimer( MCUVC_RECREATE_DATACONF_TIMER, TIMESPACE_RECREATE_DATACONF );

				//�ϱ�MCS: ��DCS, ���ݻ��鴴��ʧ��
			}
			else
			{
				CServMsg cServMsg;
				memset( &cServMsg, 0, sizeof(cServMsg) );
				
				TDcsConfInfo tDcsConfInfo;
				memset( &tDcsConfInfo, 0, sizeof(tDcsConfInfo) );
				
				//����id
				cServMsg.SetConfIdx( m_byConfIdx );
				
				//����
				s8 achConfName[MAXLEN_CONFNAME];
				strncpy( achConfName, m_tConf.GetConfName(), sizeof(achConfName) );
				tDcsConfInfo.SetConfName( achConfName );
				
				//����
				if ( NULL != m_tConf.GetConfPwd() )
				{
					tDcsConfInfo.SetConfPwd( m_tConf.GetConfPwd() );
				}
				//����
				u16 wBandWidth = m_tConf.GetDcsBitRate();
				if ( 0 == wBandWidth )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get dsc data conf bitrate failed !\n");
					return;
				}
				tDcsConfInfo.SetBandwidth( wBandWidth );
				
				//�Ƿ������������, ��Ĭ��Ϊ FALSE
				//Ӧ����MCS������, �ڽ���������ѡ��, ��Ӧ�Ļ������Ը���, �ڴ˴��ж�
				tDcsConfInfo.SetSupportJoinMt( FALSE );
				
				cServMsg.SetEventId( MCU_DCSSSN_CREATECONF_REQ );
				cServMsg.SetMsgBody( (u8*)&tDcsConfInfo, sizeof(tDcsConfInfo) );

				SendMsgToDcsSsn( m_byDcsIdx, cServMsg );
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "send to dcsssn create conf %s req success !\n", m_tConf.GetConfName() );
			}
		}
		break;

	//ֻ֧������
	case CONF_DATAMODE_DATAONLY:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "Current system couldn't support data only conf !\n");
		break;
		
	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected data conf mode: %d, ignore it!\n", m_tConf.GetConfAttrb().GetDataMode() );
		break;
	}
	
}

/*=============================================================================
  �� �� ���� ProcDcsMcuCreateConfRsp
  ��    �ܣ� ����DCS�Դ����Ӧ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/16    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcDcsMcuCreateConfRsp( const CMessage * pcMsg )
{
	//����״̬
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in create conf rsp!\n" );
		return;
	}
	//��Ϣ���
	if ( NULL == pcMsg )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get create conf rsp msg failed! \n");
		return;
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs create conf rsp msg: %d<%s> received in inst %d !\n", 
			pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
	}	
	//��Ϣ����
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	switch( pcMsg->event) 
	{
	case DCSSSN_MCU_CREATECONF_ACK:
		{
			//У��confidx
			if ( m_byConfIdx != cServMsg.GetConfIdx() )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx %d in create Data conf ack should be %d !\n", 
						   cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			
			 //�ϱ� MCS 	 
		}
		break;

	case DCSSSN_MCU_CREATECONF_NACK:
		{
			//У��confidx
			if ( m_byConfIdx != cServMsg.GetConfIdx() )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx %d in create Data conf Nack should be %d !\n", 
					       cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//��������
			u8 byErrorId = *cServMsg.GetMsgBody();
			
			if ( !m_tConfInStatus.IsDataConfRecreated() )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "create data conf %s failed due reason: %d", m_tConf.GetConfName(), byErrorId );
			}
			else
			{
				m_tConfInStatus.SetDataConfRecreated(FALSE);
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "Recreate data conf %s failed due reason: %d", m_tConf.GetConfName(), byErrorId );
			}
			
			//1. �����DCS������, ��Ѱ����һ�����е�DCS
			//2. �������Ϳ��ǻ�ȡ����
			
			//�ϱ� MCS 
			
		}
		break;

	case DCSSSN_MCU_CONFCREATED_NOTIF:
		{
			//У��confidx
			if ( cServMsg.GetConfIdx() != m_byConfIdx )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx %d in create Data conf Notify should be %d !\n", 
						   cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//��ȡ���ݻ�����Ϣ
			TDcsConfInfo tDcsConfInfo = *(TDcsConfInfo*)cServMsg.GetMsgBody();

			//������
			u32 dwRet = memcmp( tDcsConfInfo.GetConfName(), m_tConf.GetConfName(), sizeof(u8) * MAXLEN_CONFNAME );
			//��ƥ��
			if ( 0 !=dwRet )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf name %s in create Data conf Notify should be %s !\n",
						   tDcsConfInfo.GetConfName(), m_tConf.GetConfName() );
				//����
				SendMcuDcsReleaseConfReq();
				return;
			}

			//��������
			if ( NULL != m_tConf.GetConfPwd() ) 
			{
				dwRet = memcmp( tDcsConfInfo.GetConfPwd(), m_tConf.GetConfPwd(), sizeof(u8) * MAXLEN_PWD );
				//��ƥ��
				if ( 0 != dwRet )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf pwd %d in create conf Notify should be %s !\n",
							   tDcsConfInfo.GetConfPwd(), m_tConf.GetConfPwd() );
					//����
					SendMcuDcsReleaseConfReq();
					return;
				}
			}
			
			//��������
			if ( 0 != tDcsConfInfo.GetBandwidth() )
			{
				//MCUָ��DCS�������ʴ���64, DCSȡָ��ֵ
				if ( 64 < m_tConf.GetDcsBitRate() ) 
				{
					if ( tDcsConfInfo.GetBandwidth() != m_tConf.GetDcsBitRate() )
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf bandwidth %d in create conf Notify should be %d !\n", 
								   tDcsConfInfo.GetBandwidth(), m_tConf.GetDcsBitRate() );
						//����
						SendMcuDcsReleaseConfReq();
						return;
					}
				}
				//MCUָ��DCS��������С��64, DCS�Զ�����Ϊ64
				else	
				{
					if ( 64 != tDcsConfInfo.GetBandwidth() )
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf bandwidth %d in create conf Notify should be 64 !\n", 
							tDcsConfInfo.GetBandwidth() );
						//����
						SendMcuDcsReleaseConfReq();
						return;
					}
				}
			}
			//���ʿ�
			else
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf bandwidth %d in create conf Notify should be %d !\n", 
						 tDcsConfInfo.GetBandwidth(), m_tConf.GetDcsBitRate() );
				//����
				SendMcuDcsReleaseConfReq();
				return;
			}
			//�Ƿ������������
			//ȡMCS��������, ��֪ͨ�Ĳ������Ƚ�
			/*			
			if ( ����TRUE  )
			{
				if ( !tDcsConfInfo.IsSupportJoinMt() ) 
				{
					//����
					SendMcuDcsReleaseConfReq();
					return;
				}
			}
			else
			{
				if ( tDcsConfInfo.IsSupportJoinMt() ) 
				{
					//����
					SendMcuDcsReleaseConfReq();
					return;
				}
			}
			*/
			
            m_tConfInStatus.SetDataConfOngoing(TRUE);

			//��������´����Ļ���
			if ( m_tConfInStatus.IsDataConfRecreated() )
			{
				m_tConfInStatus.SetDataConfRecreated(FALSE);
				KillTimer( MCUVC_RECREATE_DATACONF_TIMER );

				//��������ԭ�����������ն�
				ReInviteHasBeenInDataConfMt();

				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "Data Conf %s Recreated and started on Dcs %d belong to Mcu Inst %d !\n",
					 m_tConf.GetConfName(), m_byDcsIdx, GetInsID() );
			}
			else
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "Data Conf %s created and started on Dcs %d belong to Mcu Inst %d !\n",
					 m_tConf.GetConfName(), m_byDcsIdx, GetInsID() );
			}
						
			 //	�ϱ�MCS: �������ݻ���ɹ�
			
		}
		break;

	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected dcs create conf rsp msg %d<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
}

/*=============================================================================
  �� �� ���� ProcMcuDcsReleaseConfReq
  ��    �ܣ� ����DCS�Ự��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/16    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::SendMcuDcsReleaseConfReq()
{
	//����״̬
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "conf state isn't ongoing, release data conf failed !\n" );
		return;
	}	
	//DCS״̬
	if ( !g_cMcuVcApp.IsPeriDcsValid(m_byDcsIdx) || !g_cMcuVcApp.IsPeriDcsConnected(m_byDcsIdx) )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs %d not exist, data conf %s release failed !\n", m_byConfIdx, m_tConf.GetConfName() );
		return;
	}
	
	//��������Ϣ
	CServMsg cServMsg;
	cServMsg.SetConfIdx( m_byConfIdx );
	cServMsg.SetEventId( MCU_DCSSSN_RELEASECONF_REQ );
	cServMsg.SetMsgBody();
	SendMsgToDcsSsn( m_byDcsIdx, cServMsg );

}

/*=============================================================================
  �� �� ���� SendMcuDcsAddMtReq
  ��    �ܣ� ����DCS�����ն˵�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMtId
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/16    4.0			�ű���                  ����
  2005/01/06	4.0			�ű���					�����޸�
=============================================================================*/
void CMcuVcInst::SendMcuDcsAddMtReq( u8 byMtId, u32 dwMtIp, u16 wPort )
{
	//����״̬
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in del mt req !\n" );
		return;
	}	
	//���˵�DCS ip���
	if ( 0 == g_cMcuVcApp.m_atPeriDcsTable[m_byDcsIdx - 1].m_dwDcsIp ) 
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "Get current dcs Ip failed !\n");
		return;
	}
	//DCS״̬
	if ( !g_cMcuVcApp.IsPeriDcsValid(m_byDcsIdx) || !g_cMcuVcApp.IsPeriDcsConnected(m_byDcsIdx) )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs %d not exist, data conf %s release failed !\n", m_byDcsIdx, m_tConf.GetConfName() );
		return;
	}

	TDcsMtInfo tDcsMtInfo;
	memset( &tDcsMtInfo, 0, sizeof(tDcsMtInfo) );

	//�ն˺ź͵�ַ
	tDcsMtInfo.m_byMtId = byMtId;
	tDcsMtInfo.m_dwMtIp = m_ptMtTable->GetIPAddr( byMtId );

	//�Զ�����
	if ( m_ptMtTable->IsNotInvited( byMtId ) )
	{
		//��������
		if ( !m_ptMtTable->IsMtIsMaster( byMtId ) )
		{
			//�ȴ��¼�����
			tDcsMtInfo.m_emJoinedType = TDcsMtInfo::emBelowJoin;
			tDcsMtInfo.m_wTimeOut	  = TIME_WAITFORJOIN;	//�ȴ��¼����볬ʱֵ(s)	
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "mcu data conf %s add mt%d as master request send out !\n", m_tConf.GetConfName(), byMtId );
		}
		//��������
		else
		{
			//�ȴ��ϼ�MCU����
			tDcsMtInfo.m_emJoinedType = TDcsMtInfo::emAboveInvite;
			tDcsMtInfo.m_wTimeOut	  = TIME_WAITFORINVITE;	//�ȴ��ϼ����볬ʱֵ(s)	
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "mcu join mt%d's data conf %s as slave request send out !\n", byMtId, m_tConf.GetConfName() );
		}

	}
	//��������
	else
	{
		//�����¼�����
		tDcsMtInfo.m_emJoinedType = TDcsMtInfo::emInviteBelow;
		tDcsMtInfo.m_dwMtIp  = dwMtIp;
		tDcsMtInfo.m_wPort   = wPort;
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "mcu data conf %s invite mt%d as master request send out !\n", m_tConf.GetConfName(), byMtId );
	}
	
	//��������Ϣ
	CServMsg cServMsg;
	cServMsg.SetConfIdx( m_byConfIdx );
	cServMsg.SetEventId( MCU_DCSSSN_ADDMT_REQ );
	cServMsg.SetMsgBody( (u8*)&tDcsMtInfo, sizeof(tDcsMtInfo) );

	SendMsgToDcsSsn( m_byDcsIdx, cServMsg );

	return;
}

/*=============================================================================
  �� �� ���� ProcDcsMcuAddMtRsp
  ��    �ܣ� ����DCS�����ն˵�Ӧ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/16    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcDcsMcuAddMtRsp( const CMessage * pcMsg )
{
	//����״̬
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in add mt rsp!\n" );
		return;
	}
	//��Ϣ���
	if ( NULL == pcMsg )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get add mt rsp msg failed! \n");
		return;
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs add mt rsp msg: %d<%s> received in inst %d !\n", 
			pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
	}
	//��Ϣ����
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	switch( pcMsg->event )
	{
	case DCSSSN_MCU_ADDMT_ACK:
		{
			//У��confidx
			if ( cServMsg.GetConfIdx() != m_byConfIdx )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx %d received in add mt ack should be %d", 
						 cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//�ն�ID
			u8 byMtId = *cServMsg.GetMsgBody();
			if ( 0 == byMtId )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong mt id %d received in add mt ack !\n", byMtId );
				return;
			}

			// �ϱ�MCS
		}
		break;

	case DCSSSN_MCU_ADDMT_NACK:
		{
			//У��confidx
			if ( cServMsg.GetConfIdx() != m_byConfIdx )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx %d received in add mt Nack should be %d\n", 
						   cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//�ն�ID
			u8 byMtId = *cServMsg.GetMsgBody();
			if ( 0 == byMtId )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong mt id %d received in add mt nack !\n", byMtId );
				return;
			}
			//��������
			u8 byErrorId = *(cServMsg.GetMsgBody() + sizeof(u8));
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "conf %s add mt%d failed due to reason: %d \n", 
					   m_tConf.GetConfName(), byMtId, byErrorId );

			// �ϱ�MCS
		}
		break;

	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected msg %d<%s> received in add mt rsp 1\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

}

/*=============================================================================
  �� �� ���� SendMcuDcsDelMtReq
  ��    �ܣ� ����DCSɾ���ն˵�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/16    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::SendMcuDcsDelMtReq( u8 byMtId )
{
	//����״̬
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in add mt rsp!\n" );
		return;
	}
	//DCS״̬
	if ( !g_cMcuVcApp.IsPeriDcsValid(m_byDcsIdx) || !g_cMcuVcApp.IsPeriDcsConnected(m_byDcsIdx) )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs %d not exist, del data mt%d failed !\n", m_byDcsIdx, byMtId );
		return;
	}

	//��ɾ����Ϣ
	CServMsg cServMsg;
	cServMsg.SetConfIdx( m_byConfIdx );
	cServMsg.SetEventId( MCU_DCSSSN_DELMT_REQ );
	cServMsg.SetMsgBody( &byMtId, sizeof(u8) );
	SendMsgToDcsSsn( m_byDcsIdx, cServMsg );
	return;
}

/*=============================================================================
  �� �� ���� ProcDcsMcuDelMtRsp
  ��    �ܣ� ����DCSɾ���ն˵�Ӧ�� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/16    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcDcsMcuDelMtRsp( const CMessage * pcMsg )
{
	//����״̬
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in del mt rsp!\n" );
		return;
	}
	//��Ϣ���
	if ( NULL == pcMsg )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get del mt rsp msg failed! \n");
		return;
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs del mt rsp msg: %d<%s> received in inst %d !\n", 
			       pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
	}
	//��Ϣ����
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	switch( pcMsg->event )
	{
	case DCSSSN_MCU_DELMT_ACK:
		{
			//У��confidx
			if ( cServMsg.GetConfIdx() != m_byConfIdx )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx %d received in del mt ack should be %d !\n", 
						 cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//�ն�ID
			u8 byMtId = *cServMsg.GetMsgBody();
			if ( 0 == byMtId )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong mt id %d received in del mt ack !\n", byMtId );
				return;
			}

			//�ϱ�MCS
		}
		break;

	case DCSSSN_MCU_DELMT_NACK:
		{
			//У��confidx
			if ( cServMsg.GetConfIdx() != m_byConfIdx )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx %d received in del mt Nack should be %d !\n", 
						   cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//�ն�ID
			u8 byMtId = *cServMsg.GetMsgBody();
			if ( 0 == byMtId )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong mt id %d received in del mt Nack !\n", byMtId );
				return;
			}
			//��������
			u8 byErrorId = *(cServMsg.GetMsgBody() + sizeof(u8) );
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "Conf %s del mt%d failed due to reason: %d !\n", m_tConf.GetConfName(), byErrorId );

			//�ϱ�MCS

		}
		break;

	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected msg %d<%s> received in del mt rsp !", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

}

/*=============================================================================
  �� �� ���� ProcDcsMcuMtJoinedNtf
  ��    �ܣ� ����DCS�ն�����֪ͨ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/16    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcDcsMcuMtJoinedNtf( const CMessage * pcMsg )
{
	//����״̬
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in mt joined notify !\n" );
		return;
	}
	//��Ϣ���
	if ( NULL == pcMsg )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get mt joined notify msg failed! \n");
		return;
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs mt joined notify msg: %d<%s> received in inst %d !\n", 
			pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
	}
	//��Ϣ����
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TDcsMtInfo tDcsMtInfo = *(TDcsMtInfo*)cServMsg.GetMsgBody();

	switch( pcMsg->event )
	{
	case DCSSSN_MCU_MTJOINED_NOTIF:
		{
			//У��confidx
			if ( m_byConfIdx != cServMsg.GetConfIdx() )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx: %d received in mt joined conf notify should be %d \n",
						 cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//�ն�ID
			u8 byMtId = tDcsMtInfo.m_byMtId;
			if ( 0 == byMtId )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong mt id %d received in mt joined conf notify !\n");
				return;
			}
			
			//�����ն���MCU
			if ( TDcsMtInfo::emT120Mcu == tDcsMtInfo.m_byDeviceType )
			{
				//��������, �����ϼ�
				if ( m_ptMtTable->IsMtIsMaster(byMtId) )
				{
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "mt%d joined data conf %s as Master_Dcs !\n", byMtId, m_tConf.GetConfName() );
				}
				//��������, �¼�MCU���뱾��
				else
				{
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "mt%d joined data conf %s as Slave_Dcs !\n", byMtId, m_tConf.GetConfName() );
				}
			}
			//�����ն���MT
			else
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "mt%d joined data conf %s as a general DCMT !\n", byMtId, m_tConf.GetConfName() );
			}

			//�����ն˵�ǰ״̬: �����ݻ�����
			if ( FALSE == m_ptMtTable->IsMtInDataConf( byMtId ) )
			{
				m_ptMtTable->SetMtInDataConf( byMtId, TRUE );	
			}
			else
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "Mt%d should not in data conf %s already !\n", byMtId, m_tConf.GetConfName() );
				return;
			}

			// �ϱ�MCS
		}
		break;

	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected msg %d<%s> received in mt joined notify !\n" );
		break;
	}
}

/*=============================================================================
  �� �� ���� ProcDcsMcuMtLeftNtf
  ��    �ܣ� ����DCS�ն�����֪ͨ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/16    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcDcsMcuMtLeftNtf( const CMessage * pcMsg )
{
	//����״̬
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in mt left notify !\n" );
		return;
	}
	//��Ϣ���
	if ( NULL == pcMsg )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get mt left notify msg failed! \n");
		return;
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs mt left notify msg: %d<%s> received in inst %d !\n", 
			pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
	}
	//��Ϣ����
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	switch( pcMsg->event )
	{
	case DCSSSN_MCU_MTLEFT_NOTIF:
		{
			//У��confidx
			if ( m_byConfIdx != cServMsg.GetConfIdx() )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx: %d received in mt left conf notify should be %d \n",
						 cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//�ն�ID
			u8 byMtId = *cServMsg.GetMsgBody();
			if ( 0 == byMtId )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong mt id %d received in mt left conf notify !\n");
				return;
			}			
			//�����ն˵�����: ������չ ?
			
			//�����ն˵�ǰ״̬: �����ݻ���
			if ( TRUE == m_ptMtTable->IsMtInDataConf( byMtId ) )
			{
				m_ptMtTable->SetMtInDataConf( byMtId, FALSE );
			}
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Mt%d left Data Conf %s belong to MCU Inst %d !\n", byMtId, m_tConf.GetConfName(), GetInsID() );
			
			// �ϱ�MCS

		}
		break;
		
	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected msg %d<%s> received in mt left notify !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
}

/*=============================================================================
  �� �� ���� ProcDcsMcuStatusNotif
  ��    �ܣ� DCS������֪ͨ 
			 (1) Ŀǰֻ��������, ���״̬
			 (2) ���´������ݻ���
			 (3) ������չ�����￼��DCS�������Ĵ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/29    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcDcsMcuStatusNotif( const CMessage * pcMsg )
{
	//����״̬
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in dcs disconnect notify !\n" );
		return;
	}
	//��Ϣ���
	if ( NULL == pcMsg )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get dcs status notify msg failed! \n");
		return;
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs status notify msg: %d<%s> received in inst %d !\n", 
			pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
	}
	//��Ϣ����
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	switch( pcMsg->event )
	{
	case MCU_DCSDISCONNECTED_NOTIF:
		{
			//�����齨�������ݻ���
			if ( 0 != m_byDcsIdx )
			{
				TDcsInfo tDcsInfo = *(TDcsInfo*)cServMsg.GetMsgBody();
				
				//ȡDCS��ID��
				if ( 0 == tDcsInfo.m_byDcsId )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "[DcsNtf] the DCS id should not be NULL, ignore it !\n");
					return;
				}
				u8 byDcsId = tDcsInfo.m_byDcsId;
				
				//�������DCS����
				if ( byDcsId == m_byDcsIdx )
				{
					//���״̬
					m_byDcsIdx = 0;
                    m_tConfInStatus.SetDataConfOngoing(FALSE);

					//���´������ݻ���
					SetTimer( MCUVC_RECREATE_DATACONF_TIMER, TIMESPACE_RECREATE_DATACONF );                  
				}
			}

            
		}
		break;
	case MCU_DCSCONNCETED_NOTIF:
		{
			//�����Ϣ��DAEMON���Ѿ�����˴���
			//������չ���Ƿ�����������صĴ���             
		}
		break;

		//DCS������, �ڴ˴������֪ͨ, �����״̬. ��������;:
		//  (1)  ����ٽ���DCS�Ĵ���, ���ز��Ҵ�DCS
		//	(2)  ֱ���յ�����δ��֪ͨ, �����Լ��

	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected dcs status notify msg %d<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
}

/*=============================================================================
  �� �� ���� ProcTimerRecreateDataConf
  ��    �ܣ� ���²���DCS, ���󴴽����ݻ���

  �㷨ʵ�֣� (1) �����Ƕ���, ����DCS��ע�ᵼ�µ����´���, ��һ�����´��᲻�ɹ�
                 �Ŀ����Ժ�С,  ������û�гɹ�, ������������ԭ��( ��: ��Ϊ�ж�
				 ����), ��ʱ, Ϊ��СMCU����, ���������´���ʱ�����ӳ�һ��, ��10s
			 (2) �û������´���״̬, �Զ�MCU����������ն˷�����������
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/1/10    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcTimerRecreateDataConf( void )
{
	if ( FALSE == FindOnlineDcs() )
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,"There is no DCS online still, recreate data conf %s failed !\n", m_tConf.GetConfName() );

		KillTimer( MCUVC_RECREATE_DATACONF_TIMER );
		SetTimer( MCUVC_RECREATE_DATACONF_TIMER, TIMESPACE_RECREATE_DATACONF * 2 );
		return;
	}
	else
	{
        m_tConfInStatus.SetDataConfRecreated(TRUE);
		SendMcuDcsCreateConfReq();
	}
}

/*=============================================================================
  �� �� ���� ReInviteHasBeenInDataConfMt
  ��    �ܣ� ��������ԭ�������ݻ����е��ն�
  �㷨ʵ�֣� (1) ����ֻ�ָ�MCU�������������ն�
			 (2) �ԶԶ��������������ն����ް취
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/1/10    4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ReInviteHasBeenInDataConfMt()
{			
	for ( s32 nIndex = 1; nIndex <= MAXNUM_CONF_MT; nIndex ++ )
	{
		//��ǰ����ն��Ƿ��������ݻ�����
		if ( m_ptMtTable->IsMtInDataConf( nIndex ) )
		{
			//�������
			if ( !m_ptMtTable->IsNotInvited( nIndex ) )
			{
				u32 dwDcsIp  = 0;
				u16 wDcsPort = 0;
				if ( TRUE == m_ptMtTable->GetMtDcsAddr( nIndex, dwDcsIp, wDcsPort ) )
				{
					//���״̬
					m_ptMtTable->SetMtInDataConf( nIndex, FALSE );
					m_ptMtTable->SetMtDcsAddr( nIndex, 0, 0 );

					//�ظ������ն����
					SendMcuDcsAddMtReq( nIndex, dwDcsIp, wDcsPort );
				}
			}
			else
			{
				//�����������ն�, �����޷����������� (���޷��õ���DCS��ַ)
				//���KEDA�ն�, ���Զ�����Ϣ, �����ټ�һ��, ��������û����
			}
		}
		m_ptMtTable->SetMtInDataConf( nIndex, FALSE );
	}
}

/*=============================================================================
  �� �� ���� FindOnlineDcs
  ��    �ܣ� �����ߵ�DCS
  �㷨ʵ�֣� �������һ���׶�, û����ȫʵ�ֶ�����DCS��������̽
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/1/10    4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMcuVcInst::FindOnlineDcs()
{
	for( s32 nIndex = 1; nIndex <= MAXNUM_MCU_DCS; nIndex ++ )
	{
		if ( g_cMcuVcApp.IsPeriDcsValid(nIndex) &&
			 g_cMcuVcApp.IsPeriDcsConnected(nIndex) )
		{
			//���浱ǰDCS��
			m_byDcsIdx = nIndex;
			return TRUE;		
		}
	}
	return FALSE;

}

// END OF FILE
