/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcuvcmt.cpp
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
//#include "mcuerrcode.h"
//#include "mpmanager.h"

/*====================================================================
    ������      ��AddMt
    ����        ������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMtAlias &tMtAlias �ն˱���
	              u16   wMtDialRate        ��������(������)
				  BOOL  bPassiveAdd        �Ƿ����ն�����������ӡ�������ն��������룬�򱻶���ӱ��ΪTRUE
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/01/10    3.0         ������         ����
====================================================================*/
u8 CMcuVcInst::AddMt( TMtAlias &tMtAlias, u16 wMtDialRate, u8 byCallMode, BOOL bPassiveAdd )
{
	u8 byMtId = 0xff;
    if( tMtAlias.m_AliasType == 0 ||
		(tMtAlias.m_AliasType > mtAliasTypeOthers &&
		 tMtAlias.m_AliasType != puAliasTypeIPPlusAlias))
	{
		return 0;
	}

	if(wMtDialRate == 0 || wMtDialRate > m_tConf.GetBitRate())
	{
		wMtDialRate = m_tConf.GetBitRate();
	}       

	//zjj20130502 �ָ�ԭ�������ʣ���TConfMtTable::GetDialBitrate�������ٸ���ʵ�ʵ���Ƶͨ��ȥ��ʵ��ֵ
	wMtDialRate += GetAudioBitrate( m_tConf.GetMainAudioMediaType() );

    
	if( mtAliasTypeTransportAddress == tMtAlias.m_AliasType || 
		mtAliasTypeE164 == tMtAlias.m_AliasType || 
		mtAliasTypeH323ID == tMtAlias.m_AliasType ||
		// PU��������
		puAliasTypeIPPlusAlias == tMtAlias.m_AliasType)
	{
		TMtAlias tTempAlias;
		if(tMtAlias.m_AliasType == puAliasTypeIPPlusAlias)
		{
			// PU�������IP+ALIAS
			tTempAlias.m_AliasType = puAliasTypeIPPlusAlias;
			
			s8* pbyAlias = (s8*)tMtAlias.m_achAlias;
			tTempAlias.m_tTransportAddr.SetIpAddr(ntohl(*(u32*)pbyAlias));
			
			pbyAlias += sizeof(u32);
			u32 wActualAlias = strlen(pbyAlias);
			//u32 wMaxAlias = sizeof(tTempAlias.m_achAlias);
			if( wActualAlias > sizeof(tTempAlias.m_achAlias) )
			{
				wActualAlias = sizeof(tTempAlias.m_achAlias);
			}

			memcpy(tTempAlias.m_achAlias,pbyAlias,wActualAlias);
		}
		else
		{
			tTempAlias = tMtAlias;
		}
		
		byMtId = m_ptMtTable->GetMtIdByAlias( &tTempAlias );
        
        BOOL bRepeat = FALSE;
        // guzh [2008/01/22]  �ƺ��ն��Ѵ���
		if (byMtId > 0)
        {
            // ����������ӣ�������������
            if ( !bPassiveAdd )
            {
                bRepeat = TRUE;
            }
            // �����ն˺�����ӣ��������ԭ�ն˲����߾Ϳ����滻������������·���һ��
            else if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
            {
                bRepeat = TRUE;
            }

            if (bRepeat)
		    {
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[AddMt] Mt.%d existed, replace.\n", byMtId);
                return byMtId;
		    }
        }

		// ԭ�������ڵ��ն�,����һ���µĸ���		
		byMtId = m_ptMtTable->AddMt( &tMtAlias, bRepeat );
		if(byMtId > 0)
		{
			m_ptMtTable->SetDialAlias( byMtId, &tMtAlias );
			m_ptMtTable->SetDialBitrate( byMtId, wMtDialRate );
			

			m_ptMtTable->SetMcuId( byMtId, LOCAL_MCUIDX);
			m_ptMtTable->SetConfIdx( byMtId, m_byConfIdx );
			m_ptMtTable->SetCallMode( byMtId, byCallMode );
			m_ptMtTable->SetMtNoRecording(byMtId);
						
			if( !bPassiveAdd )
			{
				m_tConfAllMtInfo.AddMt( byMtId );
			}

			//�����ն�״̬
			TMtStatus tMtStatus;
			tMtStatus.SetTMt( m_ptMtTable->GetMt( byMtId ) );		
			m_ptMtTable->SetMtStatus( tMtStatus.GetMtId(), &tMtStatus );
		} 
	}
	else if( mtAliasTypeH320ID == tMtAlias.m_AliasType )
	{
		byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAlias );

        BOOL bRepeat = FALSE;
        // guzh [2008/01/22]  �ƺ��ն��Ѵ���
        if (byMtId > 0)
        {
            // ����������ӣ�������������
            if ( !bPassiveAdd )
            {
                bRepeat = TRUE;
            }
            // �����ն˺�����ӣ��������ԭ�ն˲����߾Ϳ����滻������������·���һ��
            else if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
            {
                bRepeat = TRUE;
            }
            
            if (bRepeat)
            {
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[AddMt] Mt.%d existed, replace.\n", byMtId);
                return byMtId;
            }
        }

		//ԭ�������ڵ��նˣ�����һ���µĸ���		
		u8 byLayer   = 0; 
		u8 bySlot    = 0;
		u8 byChannel = 0;
		byMtId = m_ptMtTable->AddMt( &tMtAlias, bRepeat );
		if(byMtId > 0 && tMtAlias.GetH320Alias( byLayer, bySlot, byChannel ) )
		{
			m_ptMtTable->SetDialAlias( byMtId, &tMtAlias );
			m_ptMtTable->SetDialBitrate( byMtId, wMtDialRate );


			m_ptMtTable->SetMcuId( byMtId, LOCAL_MCUIDX);
			m_ptMtTable->SetConfIdx( byMtId, m_byConfIdx );
			m_ptMtTable->SetCallMode( byMtId, byCallMode );
			m_ptMtTable->SetMtNoRecording(byMtId);
			
			if( !bPassiveAdd )
			{
				m_tConfAllMtInfo.AddMt( byMtId );
			}

			//�����ն�״̬
			TMtStatus tMtStatus;
			tMtStatus.SetTMt( m_ptMtTable->GetMt( byMtId ) );		
			m_ptMtTable->SetMtStatus( tMtStatus.GetMtId(), &tMtStatus );
		}
	}

	return byMtId;
}

/*====================================================================
    ������      ��AssignMpForMt
    ����        �����������ն˷���MP
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt &tMt, ���������ն�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	07/05/31    4.0         �ű���        ����
====================================================================*/
BOOL32 CMcuVcInst::AssignMpForMt( TMt & tMt )
{
    CServMsg	cServMsg;
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetConfIdx(m_byConfIdx);

    u8 byMpId = m_ptMtTable->GetMpId(tMt.GetMtId());
    
    if ( !g_cMcuVcApp.IsMpConnected(byMpId) || !g_cMcuVcApp.IsMtAssignInMp(byMpId, m_byConfIdx, tMt.GetMtId()) )
    {
        // guzh [12/29/2006] ��ȡ���ն˵ı���
        TMtAlias tMtAlias;        
        if ( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeE164, &tMtAlias ) )
        {
            tMtAlias.m_tTransportAddr.SetIpAddr( m_ptMtTable->GetIPAddr(tMt.GetMtId()) );                    
        }
        else
        {
            m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias);
        }
        
        // MP�������� PM�� Լ��֧�� [02/09/2007-zbq]
        byMpId = g_cMcuVcApp.AssignMpIdByBurden(m_byConfIdx, tMtAlias, tMt.GetMtId(), m_tConf.GetBitRate());
        
        if ( byMpId == 0 ) 
        {
            // guzh [3/30/2007] ֪ͨMCS
            u32 dwIp = htonl(0);
            cServMsg.SetMsgBody((u8*)&dwIp, sizeof(u32));
            cServMsg.SetEventId(MCU_MCS_MPFLUXOVERRUN_NOTIFY);
            SendMsgToAllMcs( MCU_MCS_MPFLUXOVERRUN_NOTIFY, cServMsg );
            
            ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "AssignMpForMt(%d,%d) failed\n", tMt.GetMcuId(), tMt.GetMtId());
            RemoveJoinedMt( tMt, TRUE );
        }
        if (!g_cMcuVcApp.IsMpConnected(byMpId))
        {
            ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[AddJoinedMt] byMpId.%d is OFFline for MT.%d\n", byMpId, tMt.GetMtId());
            return FALSE;
        }
        m_ptMtTable->SetMpId(tMt.GetMtId(), byMpId);
    }
    
    return TRUE;
}

/*====================================================================
    ������      ��AddJoinedMt
    ����        ��������Ϣ�м�������նˣ�
	              ����Mt�����Ȼ��飻
	              ֪ͨMC��MT��SMCU��
				  ����з������MT��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt &tMt, ���������ն�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/01/02    1.0         LI Yi         ����
	04/01/12    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::AddJoinedMt( TMt &tMt )
{
	CServMsg	cServMsg;
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetConfIdx(m_byConfIdx);

	BOOL32 bHasMsgMcsAlias = FALSE;

    // xsl [11/8/2006] ����ת����id
    if ( !AssignMpForMt(tMt) )
    {
        ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[AddJoinedMt] AssignMpForMt.%d failed !\n", tMt.GetMtId() );
        return;
    }

	//�ж��Ƿ��Ѽ���(�Ƿ��ն˴���)
	if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{
        //�ն˴��᣺�����ϱ�
        SendMtAliasToMcs( tMt );

        //�ն˴��᣺������֪ͨ
        cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
        SendMsgToMt( tMt.GetMtId(), MCU_MT_CONF_NOTIF, cServMsg );

		return;
	}
	
	//֪ͨ�����ն�
	cServMsg.SetMsgBody( ( u8 * )&tMt, sizeof( tMt ) );
	BroadcastToAllSubMtJoinedConf( MCU_MT_MTJOINED_NOTIF, cServMsg );

	//bHasMsgMcsAlias = m_tConfAllMtInfo.MtInConf( tMt.GetMtId() ); 

	//��������ն��б�
	m_tConfAllMtInfo.AddJoinedMt( tMt.GetMtId() );

	m_ptMtTable->InitMtVidAlias( tMt.GetMtId() );

	if (m_ptMtTable->GetAddMtMode(tMt.GetMtId()) == ADDMTMODE_CHAIRMAN)
	{
		m_ptMtTable->SetAddMtMode(tMt.GetMtId(), ADDMTMODE_MCS);
	}

    //������ϯ����
    if( m_tConf.HasChairman() &&  m_tConf.GetChairman() == tMt )
    {
        ChangeChairman( &tMt );
    }

	//��չ㲥��¼
	m_ptMtTable->ClearMonitorSrcAndDst( tMt.GetMtId() );
	m_ptMtTable->SetMtMulticasting( tMt.GetMtId(), FALSE );

	//������֪ͨ
	TMt tSpeaker = m_tConf.GetSpeaker();
	m_tConf.SetSpeaker( GetLocalSpeaker() );
	cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
	SendMsgToMt( tMt.GetMtId(), MCU_MT_CONF_NOTIF, cServMsg );
	m_tConf.SetSpeaker( tSpeaker );
	
	MtOnlineChange( tMt, TRUE, 0 );
	
	//�Ƿ�ȫ�徲��������ǿ���ն˾���
	if( TRUE == m_tConf.GetConfAttrb().IsAllInitDumb() )
	{
		if(m_ptMtTable->GetManuId(tMt.GetMtId()) == MT_MANU_KDC)
		{
			u8 byMuteOpenFlag = 1;
			cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
			cServMsg.CatMsgBody( &byMuteOpenFlag, sizeof(byMuteOpenFlag) );
			cServMsg.SetDstMtId( tMt.GetMtId() );
			SendMsgToMt( tMt.GetMtId(), MCU_MT_MTDUMB_CMD, cServMsg );
		}
		else
		{
			TMtStatus tMtStatus;
			m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus);
			tMtStatus.SetCaptureMute(TRUE);
			m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tMtStatus);
			NotifyMtSend( tMt.GetMtId(), MODE_AUDIO, FALSE );
            MtStatusChange( &tMt, TRUE );
		}
	}

    //[2011/05/25 zhushz]VCS������鱻�������Զ��л��ɺ�������״̬
    if (MT_TYPE_MMCU == tMt.GetMtType() &&
        VCS_CONF == m_tConf.GetConfSource() &&
        ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
        !m_cVCSConfStatus.IsGroupModeLock())
    {
        m_cVCSConfStatus.SetGroupModeLock(TRUE);
        VCSConfStatusNotif();
    }
	//֪ͨ���
//	cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
//	SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
	
	//������ػ��������ն���Ϣ
	//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
	//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//��������ն˱�

	SendMtListToMcs(LOCAL_MCUIDX);
	
// 	if( !bHasMsgMcsAlias )
// 	{		
		SendMtAliasToMcs( tMt );
/*	}*/
	
	// ��Trap��Ϣ MCU�����ն����, ����ն���Ϣ��Trap��ʽ���͸�����
	TMtNotify tMtNotify;
	CConfId   cConfId;
	TMtAlias  tMtAlias;
	u8        byLen = 0;
	cConfId = m_tConf.GetConfId();
	memset( &tMtNotify, 0, sizeof(tMtNotify) );
	memcpy( tMtNotify.m_abyConfId, &cConfId, 16 );

	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias, StrOfIP( tMtAlias.m_tTransportAddr.GetIpAddr() ) );		
	}
	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias + byLen, "(%s)%c", tMtAlias.m_achAlias, 0 );
	}
	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias + byLen, "(%s)%c", tMtAlias.m_achAlias, 0 );
	}
	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320Alias, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias + byLen, "(%s)%c", tMtAlias.m_achAlias, 0 );
	}
	tMtNotify.m_abyMtAlias[sizeof(tMtNotify.m_abyMtAlias)-1] = 0;
	SendTrapMsg( SVC_AGT_ADD_MT, (u8*)&tMtNotify, sizeof(tMtNotify) );

    //n+1���ݸ����ն���Ϣ
    if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
    {
        ProcNPlusConfMtInfoUpdate();
    }

    //zbq[12/18/2007] ��ȡ�ն˰汾��Ϣ
//     if ( tMt.GetMtType() == MT_TYPE_MT )
//     {
//         CServMsg cMsg;
//         cMsg.SetEventId(MCU_MT_GETMTVERID_REQ);
//         SendMsgToMt(tMt.GetMtId(), MCU_MT_GETMTVERID_REQ, cMsg);        
//     }

	//��ӡ��Ϣ
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt%d: 0x%x(Dri:%d) joined conf success!\n", 
			  tMt.GetMtId(), m_ptMtTable->GetIPAddr(tMt.GetMtId()), tMt.GetDriId() );
}

/*====================================================================
    ������      ��RemoveMt
    ����        ���������ն˴ӻ�����ɾ�������Զ����������ն���ɾ����
	              ֪ͨMC��MT��SMCU��
	              ֹͣ���MT������
				  ������������ն��Զ��ͷŸû���;
				  ���Ϊֱ���նˣ���������Ȼ��飻
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt &tMt, Ҫɾ�����ն�
				  BOOL32 bMsg, �Ƿ���������ն�Ҫ���˳�
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/01/03    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::RemoveMt( TMt &tMt, BOOL32 bMsg, BOOL32 bIsSendMsgToMcs /*= TRUE*/ )
{
	CServMsg	cServMsg;
	BOOL32      bNeedNotifyMt = FALSE;
		
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetConfIdx( m_byConfIdx );
	
    tMt = m_ptMtTable->GetMt(tMt.GetMtId());
	
	// ��¼����״̬
	BOOL32 bIsJoinedConf = m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId());
	u8 byType = m_ptMtTable->GetMtType( tMt.GetMtId() );
	m_ptMtTable->SetNotInvited( tMt.GetMtId(), FALSE );
	RemoveJoinedMt( tMt, bMsg );

	ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "RemoveMt(%d,%d), bMsg.%d\n", tMt.GetMcuId(), tMt.GetMtId(), bMsg);

	// ɾ���������ն�ʱ������
	if (!bIsJoinedConf)
	{
		RemoveUnjoinedMtFormPeriInfo(tMt);
	}

	//�ж��Ƿ�����ϯ���߷����ն�
	if( tMt == m_tConf.GetChairman() )
	{
		bNeedNotifyMt = TRUE;
		m_tConf.SetNoChairman();
	}

	//�ж��Ƿ����ն�
	if( tMt == GetLocalSpeaker() )
	{
		bNeedNotifyMt = TRUE;
		m_tConf.SetNoSpeaker();
	}

	//�ӻ�����Ϣ��ɾ��
	m_tConfAllMtInfo.RemoveMt( tMt.GetMtId() );
	if( MT_TYPE_SMCU == byType )
	{
		TConfMtInfo *pcInfo = m_tConfAllMtInfo.GetMtInfoPtr(GetMcuIdxFromMcuId(tMt.GetMtId()));
		if(pcInfo != NULL)
		{
			pcInfo->SetNull();
		}
	}
			
	//֪ͨ�����¼��ն�
	cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
	if( bNeedNotifyMt )
	{		
		TMt tSpeaker = m_tConf.GetSpeaker();
		m_tConf.SetSpeaker( GetLocalSpeaker() );
		cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
		BroadcastToAllSubMtJoinedConf( MCU_MT_CONF_NOTIF, cServMsg );
		m_tConf.SetSpeaker( tSpeaker );
	}

	//��ӡ��Ϣ
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt%d: 0x%x(Dri:%d) removed from conf!\n", 
			       tMt.GetMtId(), m_ptMtTable->GetIPAddr(tMt.GetMtId()), tMt.GetDriId() );

	//�ӻ����ն˱���ɾ��
	m_ptMtTable->DelMt( tMt.GetMtId() );

	NotifyMcuDelMt(tMt);

	//zjj20110118 �������ն���ɾ��ʱӦ�����,���������������ն˺����������ն˴���
	if( m_byMtIdNotInvite == tMt.GetMtId() )
	{
		m_byMtIdNotInvite = 0;
	}

	if( m_byOtherConfMtId == tMt.GetMtId() )
	{
		SetInOtherConf( FALSE );
	}

	if( VCS_CONF == m_tConf.GetConfSource() )
	{
		BOOL32 bDeled = m_cVCSConfStatus.OprNewMt(tMt, FALSE);
	}

	if( bIsSendMsgToMcs )
	{
		if( VCS_CONF == m_tConf.GetConfSource() )
		{			
			VCSConfStatusNotif();
		}
		//������ػ��������ն���Ϣ
		//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
		//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
		SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

		// [11/21/2011 liuxu] ����
		//��������ն˱�
		SendMtListToMcs(LOCAL_MCUIDX);
	}

	//������ػ�����Ϣ��
	cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
	if(m_tConf.HasConfExInfo())
	{
		u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
		u16 wPackDataLen = 0;
		PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
		cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
	}
	
	SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

    // zbq [03/23/2007] �������ѯ�б�����նˣ���֪ͨ���ˢ��ѯ�б�����Լ�ά����
    u8 byMtPos;
    if ( m_tConfPollParam.IsExistMt(tMt, byMtPos) )
    {
        m_tConfPollParam.RemoveMtFromList(tMt);
		m_tConfPollParam.SpecPollPos(byMtPos);
    }
	//���ն��ڵ���ǽ��ѯ�б��������
	for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
	{
		if (m_tTWMutiPollParam.m_tTWPollParam[byIdx].IsExistMt(tMt,byMtPos))
		{
			m_tTWMutiPollParam.m_tTWPollParam[byIdx].RemoveMtFromList(tMt);
		}
	}
	//���������ն��Զ���������
	if( (m_tConfAllMtInfo.GetLocalMtNum() == 0 && m_tConf.GetConfAttrb().IsReleaseNoMt()))
	{
		ConfPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, "There's no MT in conference %s now. Release it!\n", 
				 m_tConf.GetConfName() );
		ReleaseConf( TRUE );
		NEXTSTATE( STATE_IDLE );
	}

    //VCS�����иû���Ϊ�¼����飬�ϼ��������,
	//�޸Ļ���Ĵ���������,ͨ��VCS���鼶���������ͷ�,���������Ȩ�����½���VCS	
	if ( VCS_CONF == m_tConf.GetConfSource() && 
		 MT_TYPE_MMCU == tMt.GetMtType() &&
		 CONF_CREATE_MT == m_byCreateBy)
	{
		RestoreVCConf(VCS_SINGLE_MODE);
 		m_byCreateBy = CONF_CREATE_MCS;
		cServMsg.SetEventId(MCUVC_VCSSN_MMCURLSCTRL_CMD);
		CMcsSsn::SendMsgToMcsSsn( CInstance::DAEMON, MCUVC_VCSSN_MMCURLSCTRL_CMD,
			                      cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	}

}

/*=============================================================================
  �� �� ���� RestoreVidSrcBitrate
  ��    �ܣ� �ָ���ƵԴ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMt tMt
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::RestoreVidSrcBitrate(TMt tMt, u8 byMode)
{
	u16 wMinBitRate = 0;

	TSimCapSet tDstSimCapSet;
	
    //  ��Ϊ�ڶ�ͨ������ֱ�ӷ���flowcontrol���е���
	if( MODE_SECVIDEO == byMode )
	{
		if(!m_tDoubleStreamSrc.IsNull())
		{
            // guzh [3/13/2007] ͬʱ���Ƿ��Ͷ���������
            u16 wSndBitrate = GetDoubleStreamVideoBitrate(
                                m_ptMtTable->GetDialBitrate(m_tDoubleStreamSrc.GetMtId()),
                                FALSE);
            wMinBitRate = GetLeastMtReqBitrate(FALSE, MEDIA_TYPE_NULL, m_tDoubleStreamSrc.GetMtId()); 
            wMinBitRate = min(wSndBitrate, wMinBitRate);
            AdjustMtVideoSrcBR( tMt.GetMtId(), wMinBitRate, MODE_SECVIDEO );
			return;
		}
	}

    //modify bas 2 -- �Ƿ���Ҫ���ֲ�ͬ��ʽ����Сֵ
    tDstSimCapSet = m_ptMtTable->GetDstSCS(tMt.GetMtId());
    if (MODE_VIDEO == byMode)
    {
        wMinBitRate = GetLeastMtReqBitrate(TRUE, tDstSimCapSet.GetVideoMediaType(), tMt.GetMtId());
    }        

	//���������ڹ㲥����ϳ�ͼ��,������ϳ�����
    if (g_cMcuVcApp.GetVMPMode(m_tVmpEqp) != CONF_VMPMODE_NONE 
		&& g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp)
		&& MODE_VIDEO == byMode
		)
    {
        /*
		u16 wBasBitrate;
        u8  byVmpChanNo;
        u8  byMediaType = m_tConf.GetMainVideoMediaType();
        
        //˫�ٻ���Ҫ�������ʱ�ĺ�������
        if ( 0 != m_tConf.GetSecBitRate() && 
            MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
        {
            if (m_ptMtTable->GetDialBitrate(tMt.GetMtId()) == m_tConf.GetBitRate())
            {
                byVmpChanNo = 1;
                wBasBitrate = m_wVidBasBitrate;
            }
            else
            {
                byVmpChanNo = 2;
                wBasBitrate = m_wBasBitrate;
            }
        }
        //���ٻ�˫��ʽ����
        else
        {
            if (tDstSimCapSet.GetVideoMediaType() == byMediaType)
            {
                byVmpChanNo = 1;
                wBasBitrate = m_wVidBasBitrate;
            }
            else
            {
                byVmpChanNo = 2;
                wBasBitrate = m_wBasBitrate;
            }
        }
        
        if (wMinBitRate != wBasBitrate && MODE_VIDEO == byMode)
        {
            ChangeVmpBitRate(wMinBitRate, byVmpChanNo);                
        }
		*/
		AdjustVmpBrdBitRate(&tMt);
        return;
    }
   
    //����������,ֱ�ӽ�����������
	if( !m_tConf.GetConfAttrb().IsUseAdapter() )
	{
		AdjustMtVideoSrcBR( tMt.GetMtId(), wMinBitRate, byMode );
	}
	//��������߼������ã����ӿ�ֻ����ɾ���ն˻����ն�ǿ�����ʿ��ƻ����
	//�����ֳ����¶������RefreshBasParam,�����������ٴ�ˢ����
//     else
//     {
//         u16 wAdaptBitRate = 0;      
//            
//         //˫��ʽ���������������ն˽���������
//         if (IsMtNeedAdapt(ADAPT_TYPE_VID, tMt.GetMtId())  &&
//             (IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate) || IsNeedCifAdp()))
//         {
//             if (wMinBitRate != m_wVidBasBitrate)
//             {
//                 if (m_tConf.m_tStatus.IsVidAdapting())
//                 {
//                     ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd1] change vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
//                            wMinBitRate, m_wVidBasBitrate);
//                     ChangeAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
//                 }                
//             }            
//         }
//         //˫�ٻ��齵�ٻ���٣���Ҫ���䣩
//         else if (IsMtNeedAdapt(ADAPT_TYPE_BR, tMt.GetMtId()) &&
//             IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
//         {
//             if (wMinBitRate != m_wBasBitrate)
//             {
//                 if (m_tConf.m_tStatus.IsBrAdapting())
//                 {
//                     ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd3]change br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
//                            wMinBitRate, m_wBasBitrate);
//                     ChangeAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
//                 }
//                 else
//                 {
//                     ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd3]start br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
//                            wMinBitRate, m_wBasBitrate);
//                     StartAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
//                 }                   
//                 
//                 StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
//             }
//             else if (m_tConf.m_tStatus.IsBrAdapting())//������Ҫ�����������ʣ�ֱ�ӽ�����������������mt(�������������ն˵Ľ������ʸ��͵����)
//             {
//                 ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd1] switch br bas to mt<%d>\n", tMt.GetMtId());
//                 StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
//             }
//         }         
//         //˫��ʽ�������������ն˽�����ƵԴ���ʣ����ܻὫ����ѹ�����
//         else if (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() || IsNeedCifAdp())
//         {
//             AdjustMtVideoSrcBR(tMt.GetMtId(), wMinBitRate, byMode);
//         }
//     }	

	return;
}

/*====================================================================
    ������      ��RemoveJoinedMt
    ����        ��������ն˴ӻ�����ɾ����
	              ֪ͨMC��MT��SMCU��
	              ֹͣ���MT����
				  ������ն��ǻ�����Ա��ɾ�����Ӧ�Ļ���channel
				  ����������ն�, �������ݻ�����, ֪ͨDCSɾ��֮
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt &tMt, Ҫɾ�����ն�
				  BOOL32 bMsg, �Ƿ���������ն�Ҫ���˳�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/01/03    1.0         LI Yi         ����
	05/12/20	4.0			�ű���		  T120�����ն�ɾ��
	10/11/25    5.0         ������        Trap��Ϣ����(����5.0������ʾ)
====================================================================*/
void CMcuVcInst::RemoveJoinedMt( TMt & tMt, BOOL32 bMsg, u8 byReason,BOOL32 bNeglectMtType )
{
	CServMsg	cServMsg;
    cServMsg.SetConfId( m_tConf.GetConfId() );
    cServMsg.SetConfIdx( m_byConfIdx );
	
	u8			byMode;
	
	//BOOL32		bIsExceptionReason = FALSE;

	// [11/29/2011 liuxu] ������ȼ�,���ڲ���
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "RemoveJoinedMt(%d,%d), Reason.%d, NeglectMtType.%d\n", tMt.GetMcuId(), tMt.GetMtId(), byReason, bNeglectMtType);
	
	//ˢ�µ�һ·�������
	if (m_tConf.m_tStatus.IsVidAdapting())
	{
		RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_VIDEO, TRUE);
	}	
	
	//ˢ�µڶ�·�������
	if (!m_tDoubleStreamSrc.IsNull() && 
		!(m_tDoubleStreamSrc == tMt))
	{
		RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_SECVIDEO, TRUE);
	}
	
    // xsl [11/10/2006] �ͷŽ����ת����Դ
    ReleaseMtRes( tMt.GetMtId());
	

	if( tMt.GetMtId() == m_byNewTokenReqMtid )
	{
		m_byNewTokenReqMtid = 0;
	}
	
	//�Ƿ����
	if (!m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
	{
		TConfAttrb tConfattrb = m_tConf.GetConfAttrb();
		// 2011-9-1 add by pgf: �����VMPģ�壬����ɾ��ģ���е���Ϣ
		if ( !m_tConfInStatus.IsVmpModuleUsed()	// ���ⳡ�����������飬û��VMP����ʱɾ��VMPģ��ĳ�Ա�������¿���VMPȱ�ٸó�Ա
			&& tConfattrb.IsHasVmpModule()
			&& m_tConfEqpModule.IsMtInVmp(tMt) )
		{
			//ɾ��ģ���Ա
			m_tConfEqpModule.RemoveSpecMtFromModul(tMt);
		}
		
		RemoveMcu( tMt );

		//[5/8/2012 chendaiwei]�ͷŶ˿�
		g_cMcuVcApp.ReleaseMtPort( m_byConfIdx, tMt.GetMtId() );
		m_ptMtTable->ClearMtSwitchAddr( tMt.GetMtId() );

		
		m_tConfEqpModule.RemoveMtInHduVmpModule(tMt);

		//֪ͨ
		if (bMsg)
		{
			SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
		}		
		return;
	}
	
	//��ֹͣ�����������ŵ����档��Ϊ������ն�Ҳ�ڷ���˫���Ļ�����StopDoubleStream�лָ������������ᵼ�¼�������
	/*if (m_tConf.GetConfAttrb().IsSatDCastMode())
	{
		NotifyMtSend(tMt.GetMtId(),MODE_VIDEO,FALSE);
		NotifyMtSend(tMt.GetMtId(),MODE_SECVIDEO,FALSE);
	}*/
	//ɾ�����ڻ���ģ���е���ǽ��Ա����Ϣ
	RemoveMtInMutiTvWallMoudle(tMt);

	if( m_byCreateBy == CONF_CREATE_NPLUS )
	{
		g_cMcuVcApp.NplusRemoveVmpMember(tMt.GetMtId(),m_byConfIdx);
	}
	
	//zjj20091014 �ն�����ԭ���ж�
	if( MTLEFT_REASON_EXCEPT == byReason ||
		MTLEFT_REASON_RTD == byReason ||
		MTLEFT_REASON_UNREACHABLE == byReason )	 
	{		
		TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMt );
		m_ptMtTable->SetDisconnectReason( tMt.GetMtId(),byReason );
		//bIsExceptionReason = TRUE;		
	}
	
	// 	//�Ƴ�
	// 	m_tConfAllMtInfo.RemoveJoinedMt( tMt.GetMtId() );  
	
	// VCS���鴦���账����ϯ����
	if (VCS_CONF == m_tConf.GetConfSource() && tMt == m_tConf.GetChairman())
	{
		VCSChairOffLineProc();
	}
	
    // guzh [4/29/2007] 
    if ( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) )
    {
        KillTimer(MCU_SMCUOPENDVIDEOCHNNL_TIMER);
    }	
	
	//songkun,20110530,ע�����ź�����У��ҵ����޷�����������
	//�ָ�Դ���ʣ�Դ����Ϊbas��vmp��mt��
	// 	if (m_tConf.GetSecBitRate() == 0)
	// 	{
	// 		//��ֹȡ��С����ʱȡ��
	// 		m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), m_tConf.GetBitRate(), LOGCHL_VIDEO);
	// 	}	
	// 	RestoreVidSrcBitrate(tMt, MODE_VIDEO);
	//     if (!m_tDoubleStreamSrc.IsNull() && !(tMt == m_tDoubleStreamSrc))
	//     {
	//         //��ֹȡ��С����ʱȡ��
	//         m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), m_tConf.GetBitRate(), LOGCHL_SECVIDEO);
	//         RestoreVidSrcBitrate(tMt, MODE_SECVIDEO);
	//     }    
	
    //���dump	
	if ((TYPE_MT == tMt.GetType() &&
		(MT_TYPE_MMCU == tMt.GetMtType() ||
		MT_TYPE_SMCU == tMt.GetMtType())) //û�з������ش���㲥��MCUҲҪ��dump
        ||
        m_ptMtTable->IsMtAudioSending(tMt.GetMtId()))
	{
		g_cMpManager.StopRecvMt( tMt, MODE_AUDIO );
	}
	
    if ((TYPE_MT == tMt.GetType() &&
		(MT_TYPE_MMCU == tMt.GetMtType() ||
		MT_TYPE_SMCU == tMt.GetMtType())) //û�з������ش���㲥��MCUҲҪ��dump
        ||
        m_ptMtTable->IsMtVideoSending(tMt.GetMtId()))
	{
		g_cMpManager.StopRecvMt(tMt, MODE_VIDEO);
	}
	
    //��������� // libo [5/13/2005]
    g_cMpManager.RemoveSwitchBridge(tMt, 0);
	
    // [pengjie 2010/4/17]  ����ն��������е���Ϣ���ı����������״̬��������Ӧ����
	RemoveMtFormPeriInfo( tMt, byReason );	
	
	//�뿪���ն��Ƿ�����
	if( GetLocalSpeaker() == tMt )
	{
		//����ģʽ��ֻ��VIDEO���ֽ��д���
		byMode =  m_tConf.m_tStatus.IsMixing() ? MODE_VIDEO : MODE_BOTH;
		
		g_cMpManager.RemoveSwitchBridge( tMt, 0, byMode);
		
		if( !m_tConf.m_tStatus.IsNoRecording() && !g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) )
		{
			StopSwitchToPeriEqp( m_tRecEqp.GetEqpId(), m_byRecChnnl, FALSE, byMode );
		}
		
        //changevid ʱ��������������û��Ҫ��
		//         if (m_tConf.m_tStatus.IsAudAdapting())
		//         {
		//             StopSwitchToPeriEqp(m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl, FALSE, byMode);
		//         }
		//         if (m_tConf.m_tStatus.IsVidAdapting())
		//         {
		//             StopSwitchToPeriEqp(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, FALSE, byMode);
		//         }
		//         if (m_tConf.m_tStatus.IsBrAdapting())
		//         {
		//             StopSwitchToPeriEqp(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, FALSE, byMode);
		//         }
	}
	
    if (MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tMt.GetMtId()))
    {
        ClearH239TokenOwnerInfo( &tMt );
    }
    
	
	
    //ͣ���ڶ�·��Ƶ����ͨ��
    if(m_tDoubleStreamSrc == m_ptMtTable->GetMt(tMt.GetMtId()))
    {
        StopDoubleStream( TRUE, TRUE );
    }
    else
    {
        //sony G70SP��H263-H239ʱ���û˫��,�Ҳ�ʹ�����ƶ�ʹ��FlowControlIndication���з��ͼ����ر� ����
        //POLYCOM7000�ľ�̬˫��ʱ��ʹ�����ƽ��з��ͼ����ر�
        //���ں��н���ʱ����,֮������ڼ䱣��ͨ����ֱ���˳�����
        TLogicalChannel tH239LogicChnnl;
        if( ( ( MT_MANU_SONY == m_ptMtTable->GetManuId(tMt.GetMtId()) && 
            MEDIA_TYPE_H263 == m_tConf.GetCapSupport().GetDStreamMediaType() &&
            m_tConf.GetCapSupport().IsDStreamSupportH239()) || 
            MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) ) && 
            m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ) )			
        {
            u8 byOut = 0;
            TLogicalChannel tNullLogicChnnl;
            cServMsg.SetMsgBody( ( u8 * )&tH239LogicChnnl, sizeof( tH239LogicChnnl ) );	
            cServMsg.CatMsgBody( &byOut, sizeof(byOut) );
            SendMsgToMt( tMt.GetMtId(), MCU_MT_CLOSELOGICCHNNL_CMD, cServMsg );
            m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tNullLogicChnnl, FALSE );
        }
    }
    
    //����ڶ�·��Ƶ����ͨ��
    //fix:���Ƿ�ɢ���飬�𵥲��ն˽���
    if ( !m_tDoubleStreamSrc.IsNull()
		// �����ն�ͳһ���� [pengguofeng 3/5/2013]
// 		&& 
//          ( !m_tConf.GetConfAttrb().IsSatDCastMode() ||
//            !IsMultiCastMt( tMt.GetMtId() ) 
//          ) 
       )
    {
		// 		if (IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO))
		// 		{
		// 			StopDSAdapt(m_tDoubleStreamSrc, tMt);
		// 		}
		// 		else
		{
			//zjl 20110510 Mp: StopSwitchToSubMt �ӿ������滻 
			//g_cMpManager.StopSwitchToSubMt(tMt, MODE_SECVIDEO, TRUE);
			g_cMpManager.StopSwitchToSubMt(m_byConfIdx, 1, &tMt, MODE_SECVIDEO);
		}     
    }

	//��������ڡ���ϯ��ѯѡ����ģʽ�£��Ҷϻ���ɾ����ϯ�ն�ʱҪͣ��ϯ��ѯѡ��
	//��ϯ��ѯ�����У���ϯ�����ˣ���Ҫͣ��ϯ��ѯ
	if( /*(MTLEFT_REASON_NORMAL == byReason || MTLEFT_REASON_LOCAL == byReason) && */
		((CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode())||
		(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode())) && 
		(tMt == m_tConf.GetChairman()) )
	{
		ProcStopConfPoll();
	}
	//vcs���飬��ǰ��ϯ���б�����ϯ�Ļ��Ѿ��й��ˣ�Ϊ�����նˣ�û������ϯ���߱�����ϯ�����ߣ�ͣ����ǰ����ϯ��ѯ
	if ( VCS_CONF == m_tConf.GetConfSource() && tMt == m_tConf.GetChairman()
			&& m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP 
		)
	{
		//��ʱ����ϯ����Ϊ�����ߵģ�VcsStopChairmanPoll��������ߵ���ϯ���ܻ�ȥѡ����ǰ�����նˡ�
		//��ʱ��������ߣ��������ָ������ߴ����ں��洦��
		m_tConfAllMtInfo.RemoveJoinedMt( tMt.GetMtId() );
		VcsStopChairManPoll();
		m_tConfAllMtInfo.AddJoinedMt( tMt.GetMtId() );
	}
	
	//�ж��Ƿ�����ϯ�ն�
	if( tMt == m_tConf.GetChairman() )
	{
		// ��ʱ�����ն˻���Ϊ�����ߵģ������ն˼�����ϯ���Ƿ�����ʱ���Զ�����ϳ�ʱ���ᵼ�»���ϳɵ�0ͨ����Ϊ�����ˡ�
		// ׷�ӽӿڣ������Ѿ�������������������������
		ChangeChairman( NULL , FALSE);
		if( MT_TYPE_MMCU == tMt.GetMtType() )
		{
			TMt tNullMt;
			tNullMt.SetNull();
			m_tConf.SetChairman( tNullMt );
		}
		else
		{
			m_tConf.SetChairman( tMt );
		}
	}
	
    // ����@2006.4.17 Ҫ��鱾�ն��Ƿ�����ѯ
    BOOL32 bIsMtInPolling = FALSE;
    if ( CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollState() )
    {
        // guzh [6/14/2007]
        if (tMt == (TMt)m_tConf.m_tStatus.GetMtPollParam() ||
            (!m_tConf.m_tStatus.GetMtPollParam().IsLocal() && 
			MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) &&
			IsMtInMcu( tMt,(TMt)m_tConf.m_tStatus.GetMtPollParam() )
            /*m_tConf.m_tStatus.GetMtPollParam().GetMcuId() == tMt.GetMtId()*/
			)
			)
        {
            bIsMtInPolling = TRUE;
        }
    }    
	
	//�ж��Ƿ�Ϊ�����ն�
	//zjj20120904 ���ط����˵��ߺ������߻ָ����¼��ն˻��ϼ���Ϊ�����˾Ͳ��ָ�
	if( tMt == GetLocalSpeaker() )
	{
		TMt tSpeaker = m_tConf.GetSpeaker();
		ChangeSpeaker( NULL,  bIsMtInPolling, FALSE);//������vmp������RemoveMtFormPeriInfo�е���
		
		if( MT_TYPE_MMCU == tMt.GetMtType() )
		{			
			m_tConf.SetNoSpeaker();
		}
		else
		{
			if( tSpeaker.IsLocal() )
			{
				m_tConf.SetSpeaker( tMt );
			}			
		}
	}
	else if (tMt == GetSecVidBrdSrc())
	{
		TMt tNullMt;
		ChangeSecSpeaker(tNullMt, FALSE);
	}

    // zbq [03/12/2007] ��������ڱ���ѯ���նˣ�ֱ��������һ����ѯ�ն�
    if (bIsMtInPolling 
		&& m_tConf.m_tStatus.GetPollState() == POLL_STATE_NORMAL // [5/7/2010 xliang] ��ѯ��̬ͣ��Ҫ�趨ʱ
		)
    {
        SetTimer( MCUVC_POLLING_CHANGE_TIMER, 500 );
    }

	//���VCS�Ƿ�����ϯ��ѯ��������ѯ��������һ����ѯ�ն� [6/7/2012 chendaiwei]
	if ( m_cVCSConfStatus.GetChairPollState() == VCS_POLL_START 
		&& tMt.IsLocal()
		&& tMt == m_cVCSConfStatus.GetCurChairPollMt() )
	{
		SetTimer(MCUVC_VCS_CHAIRPOLL_TIMER, 10);

		ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[VcsChairmanPoll] poll to next mt for cur mt %d removed!\n",tMt.GetMtId());
	}

	//ֹͣ����̨�ն˽���
    //fix:���Ƿ�ɢ���飬�𵥲��ն˽���
//     if ( !m_tConf.GetConfAttrb().IsSatDCastMode() ||
//          !IsMultiCastMt( tMt.GetMtId() ))
//     {
	//zjj20100422 ���ڹҶ��ϼ�mcu�����Ȳ��𽻻�,�ں���𽻻�
	if( m_tCascadeMMCU.IsNull() || !(m_tCascadeMMCU == tMt) )
	{
		//zjl 20110510 StopSwitchToSubMt �ӿ������滻
		//StopSwitchToSubMt( tMt.GetMtId(), MODE_BOTH );
		StopSwitchToSubMt(1, &tMt, MODE_BOTH);
	}
    /*}	
    else
    {
        // ���ǻ�������ʾ�ն˿��Լ�
        NotifyMtReceive( tMt, tMt.GetMtId() );
    }*/
	
	//�ж��Ƿ�Ϊ�ش�ͨ��,��������ش�����
	if( !m_tCascadeMMCU.IsNull() && !(m_tCascadeMMCU == tMt) )
	{
		u16 wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx);
		if( ptConfMcInfo != NULL && 
			( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
		{
			if( ptConfMcInfo->m_tSpyMt.GetMtId() == tMt.GetMtId() &&
				ptConfMcInfo->m_tSpyMt.GetMcuId() == tMt.GetMcuId() )
			{
				u8 byStopMode = MODE_BOTH;
				if (m_tConf.m_tStatus.IsMixing())
				{
					byStopMode = MODE_VIDEO;
				}
				//zjl 20110510 StopSwitchToSubMt �ӿ������滻
				//StopSwitchToSubMt( m_tCascadeMMCU.GetMtId(), byMode );
				StopSwitchToSubMt(1, &m_tCascadeMMCU, byStopMode);
				//δ���ţ�������������������
			}
		}
	}
	
	//�����鲥��¼
	u8 byMtList[MAXNUM_MT_CHANNL];
	u8 byMtNum = m_ptMtTable->GetMonitorSrcMtList( tMt.GetMtId(), byMtList );
	TMt tSrcMt;
	for( u8 byLoop = 0; byLoop < byMtNum; byLoop++ )
	{
		m_ptMtTable->RemoveMonitorDstMt( byMtList[byLoop], tMt.GetMtId() );
		if( !m_ptMtTable->HasMonitorDstMt( byMtList[byLoop] ) )
		{
			tSrcMt = m_ptMtTable->GetMt( byMtList[byLoop] );
			g_cMpManager.StopMulticast( tSrcMt, 0, MODE_BOTH, FALSE );	
			m_ptMtTable->SetMtMulticasting( byMtList[byLoop], FALSE );
		}
	}
	
	//ͣ�鲥
	if( m_ptMtTable->IsMtMulticasting( tMt.GetMtId() ) )
	{
		//[1/24/2013 chendaiwei]�����鲥ʱ���ҶϷ����˵��»���ϳ����鲥��ַ�Ľ��������
		if ( m_tConf.GetConfAttrb().IsMulticastMode() && tMt.GetMtId() == GetLocalVidBrdSrc().GetMtId())
		{
			g_cMpManager.StopMulticast( tMt, 0, MODE_VIDEO );
		}
		
		if ( m_tConf.GetConfAttrb().IsMulticastMode() && tMt.GetMtId() == GetLocalAudBrdSrc().GetMtId())
		{
			g_cMpManager.StopMulticast( tMt, 0, MODE_AUDIO );
		}
		
		m_ptMtTable->SetMtMulticasting( tMt.GetMtId(), FALSE );
	}
    
	// ��Trap��Ϣ ɾ��MTʱ��MT�����Ϣ��Trap��ʽ�ϱ�������
	TMtNotify tMtNotify;
	TMtAlias tMtAlias;
	CConfId cConfId;
	u8 byLen = 0;
	cConfId = m_tConf.GetConfId();
	memset( &tMtNotify, 0, sizeof(tMtNotify) );
	memcpy( tMtNotify.m_abyConfId, &cConfId, 16 );
	
	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias, StrOfIP( tMtAlias.m_tTransportAddr.GetIpAddr() ) );		
	}
	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias + byLen, "(%s)%c", tMtAlias.m_achAlias, 0 );
	}
	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias + byLen, "(%s)%c", tMtAlias.m_achAlias, 0 );
	}
	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320Alias, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias + byLen, "(%s)%c", tMtAlias.m_achAlias, 0 );
	}
	tMtNotify.m_abyMtAlias[sizeof(tMtNotify.m_abyMtAlias)-1] = 0;
	SendTrapMsg( SVC_AGT_DEL_MT, (u8*)&tMtNotify, sizeof(tMtNotify) );
	
	// ���vmp��ͨ����ѯ,�ն˵���ˢ����ѯ�б�,�������µ���ѯ�ն�
	ProcVmpPollingByRemoveMt( tMt );

	u16 wMcuIdx = INVALID_MCUIDX;
	// �Ҷϻ�ɾ���¼�MCU����¼��ն˴���ѯ�б���ɾ��, MCS�Լ�ά���б�����֪ͨ, zgc, 2007-03-29
	if( MT_TYPE_SMCU == tMt.GetMtType() )
	{
		ProcConfPollingByRemoveMt( tMt );
		/*wMcuIdx = GetMcuIdxFromMcuId( tMt.GetMtId() );
		if( POLL_STATE_NONE != m_tConf.m_tStatus.GetPollState() )
		{
		TMtPollParam atMtPollNew[MAXNUM_CONF_MT];
		TMtPollParam tTmpMt;
		u8 byPos;
		u8 byNewNum= 0;
		BOOL32 bPollingMtInSMcu = IsMtInMcu( tMt,(TMt)m_tConf.m_tStatus.GetMtPollParam() );// ( m_tConf.m_tStatus.GetMtPollParam().GetMcuIdx() == wMcuIdxtMt.GetMtId() ) ? TRUE : FALSE ;
		u8 byNextPos = bPollingMtInSMcu ? POLLING_POS_START : 0;     // ��һ��Ҫ��ѯ���ն�
		for( byPos = 0; byPos < m_tConfPollParam.GetPolledMtNum(); byPos++ )
		{
		tTmpMt = *(m_tConfPollParam.GetPollMtByIdx(byPos));
		if( !IsMtInMcu( tMt,(TMt)tTmpMt ) )//wMcuIdxtMt.GetMtId() != tTmpMt.GetMcuIdx() )
		{
		atMtPollNew[byNewNum] = tTmpMt;
		if( byPos > m_tConfPollParam.GetCurrentIdx() && byNextPos == POLLING_POS_START )
		{
		//��¼��һ������ѯ���ն������б��е�λ��, zgc, 2007-04-07
		byNextPos = byNewNum; 
		}
		byNewNum ++;
		}
		}
		m_tConfPollParam.SetPollList( byNewNum, atMtPollNew );
		// ������ߵ��¼�MCU���е�ǰ��ѯ���նˣ��������л�
		if ( bPollingMtInSMcu )
		{
		// ָ���µ���ѯλ��, zgc, 2007-04-07
		if( byNextPos == POLLING_POS_START )
		{
		byNextPos = m_tConfPollParam.GetPolledMtNum();
		}
		m_tConfPollParam.SpecPollPos( byNextPos );
		SetTimer( MCUVC_POLLING_CHANGE_TIMER, 10 );
		}
	}*/
		//lukunpeng �ͷű�������Ĵ�mcu�����еĻش��ն˼�ͨ����Ϣ
		//		if( IsLocalAndSMcuSupMultSpy( wMcuIdx ) )
		//		{
		//			m_cSMcuSpyMana.FreeSpyChannlInfoByMcuId( GetMcuIdxFromMcuId( tMt.GetMtId() ),g_cMcuVcApp.GetCMultiSpyMgr() );
		//		}
	}
    
	if( MT_TYPE_MMCU == tMt.GetMtType() )
	{
		// [7/31/2010 xliang] �ϴ���ѯֹͣ
		u8 byPollMode = m_tConf.m_tStatus.GetPollMode();
		if (CONF_POLLMODE_VIDEO_SPY == byPollMode
			|| CONF_POLLMODE_SPEAKER_SPY == byPollMode)
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT, " tmt.%u is MMCU, so stop SpyPoll!\n", tMt.GetMtId());		
            ProcStopConfPoll();
		}
	}
	
	
	//zjj20110218�Ҷϱ����������ڵ�mcu,����ҵ��Ҫֹͣ
	if ( MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) &&
		ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() 
		)
    {
        if ( !m_tRollCallee.IsLocal() && IsMtInMcu(tMt,m_tRollCallee) )
        {
            NotifyMcsAlarmInfo( 0, ERR_MCU_ROLLCALL_CALLEELEFT );
            RollCallStop(cServMsg);
        }
    }
	
	//zbq [12/05/2007] ��ѡ��
	//zjj20100422 ���ڹҶ��ϼ�mcu�����Ȳ��𽻻�,�ں���𽻻�
	if( m_tCascadeMMCU.IsNull() || !( m_tCascadeMMCU == tMt ) )
	{
		StopSelectSrc(tMt, MODE_BOTH, FALSE);
		StopSelectSrc(tMt, MODE_VIDEO2SECOND, FALSE);
	}
	//fxh ���ѡ�����ն˵�״̬
	// 	TMt tNullMt;
	// 	tNullMt.SetNull();
	// 	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	// 	{
	// 		TMtStatus tStat;
	// 		m_ptMtTable->GetMtStatus(byMtId, &tStat);
	// 		if (tStat.GetSelectMt(MODE_VIDEO) == tMt)
	// 		{
	// 			tStat.SetSelectMt(tNullMt, MODE_VIDEO);
	// 		}
	// 		if (tStat.GetSelectMt(MODE_AUDIO) == tMt)
	// 		{
	// 			tStat.SetSelectMt(tNullMt, MODE_AUDIO);
	// 		}
	// 		m_ptMtTable->SetMtStatus(byMtId, &tStat);
	// 	}
    
    //zbq [12/05/2007] �������
	//     if ( ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
	//     {
	//         if ( tMt == m_tRollCaller )
	//         {
	//             NotifyMcsAlarmInfo( 0, ERR_MCU_ROLLCALL_CALLERLEFT );
	//             RollCallStop(cServMsg);
	//         }
	//         else if ( tMt == m_tRollCallee )
	//         {
	//             NotifyMcsAlarmInfo( 0, ERR_MCU_ROLLCALL_CALLEELEFT );
	//             RollCallStop(cServMsg);
	//         }
	//     }
	
	NotifyMcuDropMt(tMt);
	
	
	//����ϼ�MCU����
	if( m_tConf.m_tStatus.GetProtectMode() == CONF_LOCKMODE_LOCK &&
		tMt.GetMtId() == m_tConfProtectInfo.GetLockedMcuId() &&
		!m_tCascadeMMCU.IsNull() && m_tCascadeMMCU == tMt )
	{
		m_tConf.m_tStatus.SetProtectMode(CONF_LOCKMODE_NONE);
        m_tConfProtectInfo.SetLockByMcu(0);
        m_tConfProtectInfo.SetLockByMcs(0);
	}
	
	//����ϼ�MCU
	if(!m_tCascadeMMCU.IsNull() && m_tCascadeMMCU == tMt)
	{	
		wMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		if( IsLocalAndSMcuSupMultSpy( wMcuIdx )/*IsSupportMultiSpy()*/ )
		{
			StopAllLocalSpySwitch();
		}
		// 		else
		// 		{
		// 			StopSpyMtCascaseSwitch();
		// 			//zjl 20110510 StopSwitchToSubMt �ӿ������滻
		// 			//StopSwitchToSubMt( tMt.GetMtId(), MODE_BOTH );
		// 			StopSwitchToSubMt(1, &tMt, MODE_BOTH);
		// 		}
		
		StopSpyMtCascaseSwitch();
        //�ͷŶ�Ӧ����������Դ
        //StopHDCascaseAdp();
		
		
		m_tCascadeMMCU.SetNull();
		m_tConfAllMtInfo.m_tMMCU.SetNull();
		m_tConfInStatus.SetNtfMtStatus2MMcu(FALSE);
	}
	
	
    // ֪ͨ�ն�����ԭ��
	// fxh ��RestoreAllSubMtJoinedConfWatchingSrcMtǰ������ȷ��VCS����״̬,�Ƿ��е�ǰ�����ն�
    MtOnlineChange( tMt, FALSE, byReason );	
	//���ڹҶ��նˣ������ǰ�ն˿��Ľ���ͨ����mpeg4 auto���ж��Ƿ���Ҫ�ı�MP4����Ĳ���
	BOOL32 bMepg4AutoMt = FALSE;
	TLogicalChannel tForwadChnnl;
	u16 wMtBitRate = 0;
	if (tMt.IsLocal() && m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId() ,LOGCHL_VIDEO,&tForwadChnnl,TRUE))
	{
		if (tForwadChnnl.GetChannelType() == MEDIA_TYPE_MP4 &&tForwadChnnl.GetVideoFormat() == VIDEO_FORMAT_AUTO ) 
		{
			bMepg4AutoMt = TRUE;
			wMtBitRate = m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE);
		}
	}

	//����������նˣ��ڽ��ն���Ϊ������֮ǰ�����ն�ֹͣ�����������Ա������ȷ��
	//������ն���˫��Դ��������StopDoubleStream�����ն�ֹͣ����˫�����ڴ˾Ͳ�������
	if (m_tConf.GetConfAttrb().IsSatDCastMode())
	{
		NotifyMtSend(tMt.GetMtId(),MODE_VIDEO,FALSE);
	}

	//�Ƴ�

	m_tConfAllMtInfo.RemoveJoinedMt( tMt.GetMtId() );
	if ( bMepg4AutoMt && m_tConf.GetSecBitRate() != 0) 
	{
		u8 byOutChnl = GetVmpOutChnnlByRes(m_tVmpEqp.GetEqpId(), VIDEO_FORMAT_AUTO,MEDIA_TYPE_MP4);
		u16 wMinMp4GrpBirate = GetMinMtRcvBitByVmpChn(m_tVmpEqp.GetEqpId(), TRUE,byOutChnl);
		
		if ( wMtBitRate < wMinMp4GrpBirate )
		{
			//�ı�mp4 ����·�������
			TPeriEqpStatus tVmpStatus;
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpStatus);
			TVMPParam_25Mem tVmpParam = tVmpStatus.m_tStatus.tVmp.GetVmpParam();
			if (tVmpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::WAIT_START || 
				tVmpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::START) 
			{
				ChangeVmpParam(m_tVmpEqp.GetEqpId(), &tVmpParam);
			}
		}
	}

	RestoreAllSubMtJoinedConfWatchingSrcMt( tMt );
	RestoreAllSubMtJoinedConfWatchingSrcMt(tMt, MODE_VIDEO2SECOND);
	AdjustSwitchToMonitorWatchingSrc( tMt );			//ͣ�������
	AdjustSwitchToAllMcWatchingSrcMt( tMt );
	StopSwitchToAllPeriEqpWatchingSrcMt( tMt );
	
	//[2011/11/14/zhangli]��RestoreAllSubMtJoinedConfWatchingSrcMt���Ƶ��˴�
	if( !m_tCascadeMMCU.IsNull() && !(m_tCascadeMMCU==tMt) )
	{
		wMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		if( IsLocalAndSMcuSupMultSpy( wMcuIdx ) )
		{		
			TSimCapSet tSrcCap;
			CSendSpy *ptSndSpy = NULL;
			TLogicalChannel tDstAudLgc,tSrcAudLgc;	
			m_ptMtTable->GetMtLogicChnnl(m_tCascadeMMCU.GetMtId(), MODE_AUDIO, &tDstAudLgc, TRUE);
			
			
			for (u8 bySpyIdx = 0; bySpyIdx < MAXNUM_CONF_SPY; bySpyIdx++)
			{
				ptSndSpy = m_cLocalSpyMana.GetSendSpy(bySpyIdx);
				if (NULL == ptSndSpy)
				{
					continue;
				}
				if (ptSndSpy->GetSpyMt().IsNull() || ptSndSpy->GetSimCapset().IsNull() || !(ptSndSpy->GetSpyMt() == tMt))
				{
					continue;
				}
				tSrcCap = m_ptMtTable->GetSrcSCS(GetLocalMtFromOtherMcuMt(ptSndSpy->GetSpyMt()).GetMtId());
				if(ptSndSpy->GetSimCapset() < tSrcCap)
				{
					StopSpyAdapt(ptSndSpy->GetSpyMt(), ptSndSpy->GetSimCapset(), MODE_VIDEO);
				}
				/*if( m_ptMtTable->GetMtLogicChnnl(GetLocalMtFromOtherMcuMt(ptSndSpy->GetSpyMt()).GetMtId(), MODE_AUDIO, &tSrcAudLgc, FALSE) )
				{
					if ( ptSndSpy->GetSimCapset().GetAudioMediaType() != MEDIA_TYPE_NULL &&
						tSrcCap.GetAudioMediaType() != MEDIA_TYPE_NULL && 
						( ptSndSpy->GetSimCapset().GetAudioMediaType() != tSrcCap.GetAudioMediaType() || 
						tSrcAudLgc.GetAudioTrackNum() != tDstAudLgc.GetAudioTrackNum() )
						)
					{
						StopSpyAdapt(ptSndSpy->GetSpyMt(), ptSndSpy->GetSimCapset(), MODE_AUDIO);
					}
				}*/

				if( IsNeedSpyAdpt(ptSndSpy->GetSpyMt(), ptSndSpy->GetSimCapset(), MODE_AUDIO) )
				{
					StopSpyAdapt(ptSndSpy->GetSpyMt(), ptSndSpy->GetSimCapset(), MODE_AUDIO);
				}
				
				StopSwitchToSubMt(1, &m_tCascadeMMCU, MODE_BOTH,TRUE,TRUE,TRUE,ptSndSpy->m_tSpyAddr.GetPort());

				m_cLocalSpyMana.FreeSpyChannlInfo( tMt,MODE_BOTH );

				ProcMMcuBandwidthNotify( m_tCascadeMMCU,GetRealSndSpyBandWidth() );
				break;
			}
		}
		
	}

	//zjj20100210 ��ش����� �����ش�����
	/*u16 wSpyPort = SPY_CHANNL_NULL;	
	CSendSpy cSendSpy;
	wSpyPort = SPY_CHANNL_NULL;
	u8 byMediaMode = MODE_NONE;
	byMediaMode = m_cSMcuSpy.GetMode(tMt);
	if( MODE_NONE != byMediaMode &&
	!m_tCascadeMMCU.IsNull() && 
	m_cSMcuSpy.GetSpyChannlInfo(tMt, cSendSpy) )
	{
	wSpyPort = cSendSpy.m_wSpyStartPort;		
	StopSwitchToSubMt( m_tCascadeMMCU.GetMtId(), byMediaMode, SWITCH_MODE_BROADCAST, TRUE,TRUE,FALSE,wSpyPort );
	}	*/
	
	//wMcuIdx = GetMcuIdxFromMcuId( tMt.GetMtId() );
	RemoveMcu( tMt );
	
	
	//����߼�ͨ��
    StopH460PinHoleNotifyByMtId( tMt.GetMtId() );
	m_ptMtTable->ClearMtLogicChnnl( tMt.GetMtId() );
    m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_NONE );
    //��״̬
    TMtStatus tMtStatus;
    if ( m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus) )
    {
		BOOL32 byIsMtInMixing = m_ptMtTable->IsMtInMixing( tMt.GetMtId() );
		BOOL32 byIsMtInHdu = m_ptMtTable->IsMtInHdu( tMt.GetMtId() );		
		
        tMtStatus.Clear();
		tMtStatus.SetMtId(tMt.GetMtId());
		tMtStatus.SetMcuIdx(tMt.GetMcuId());
		tMtStatus.SetConfIdx(m_byConfIdx);
		
		tMtStatus.SetMtDisconnectReason( byReason );
		tMtStatus.SetInMixing(byIsMtInMixing);
		if (byIsMtInHdu && m_tConf.m_tStatus.m_tConfMode.GetHduInBatchPoll() != POLL_STATE_NONE)
		{
			tMtStatus.SetInHdu(TRUE);
		}
        m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tMtStatus);
		MtStatusChange(&tMt, TRUE);
    }
	
    //֪ͨ
	if( bMsg )
	{
        cServMsg.SetMsgBody();
		SendMsgToMt( tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg );
	}
	
	//�������ն�
	if ( m_ptMtTable->IsMtInDataConf( tMt.GetMtId() ) )
	{
		//֪ͨDCSɾ��
		SendMcuDcsDelMtReq( tMt.GetMtId() );
	}    
	
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
	
	//������ػ��������ն���Ϣ
	//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
	//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
	
	//֪ͨ�����ն�
	cServMsg.SetMsgBody( ( u8 * )&tMt, sizeof( tMt ) );	
	BroadcastToAllSubMtJoinedConf( MCU_MT_MTLEFT_NOTIF, cServMsg );
	
    // xsl [10/11/2006] �ͷŶ˿�
	g_cMcuVcApp.ReleaseMtPort( m_byConfIdx, tMt.GetMtId() );
    m_ptMtTable->ClearMtSwitchAddr( tMt.GetMtId() );
	
    // zbq [02/18/2008] �����ն���ᣬ�ָ�VMP�㲥������
	// zjj20101115 �����VMP�㲥�ͻָ��㲥Դ����,����ʱ�������ڷ���������Ϣǰ������ն�E1״̬
    if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
		m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()) )
    {
		if(g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) )
		{
			AdjustVmpBrdBitRate();
		}
		else 
		{
			TMt tSrc = GetLocalVidBrdSrc();
			if( !tSrc.IsNull() )
			{				
				m_ptMtTable->SetMtTransE1(tMt.GetMtId(), FALSE);
				m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
				NotifyMtSend( tSrc.GetMtId(), MODE_VIDEO );
			}			
		}
    }
	
    //20110527,songkun,VMP�㲥�ͻָ��㲥ԭ����,VMP���٣������ն��뿪����
    //�ָ�Դ���ʣ�Դ����Ϊbas��vmp��mt��
    if (m_tConf.GetSecBitRate() == 0)
	{
		//��ֹȡ��С����ʱȡ��
		m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), m_tConf.GetBitRate(), LOGCHL_VIDEO);
	}	
	RestoreVidSrcBitrate(tMt, MODE_VIDEO);
	if (!m_tDoubleStreamSrc.IsNull() && !(tMt == m_tDoubleStreamSrc))
	{
		//��ֹȡ��С����ʱȡ��
		m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), m_tConf.GetBitRate(), LOGCHL_SECVIDEO);
		RestoreVidSrcBitrate(tMt, MODE_SECVIDEO);
	} 
	
	
    //�ָ�E1״̬
    m_ptMtTable->SetMtTransE1(tMt.GetMtId(), FALSE);
    m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
	
    //n+1���ݸ����ն���Ϣ
    if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
    {
        ProcNPlusConfMtInfoUpdate();
    }
	
    //���Ⱥ��
    TCapSupport tCap;
    if (m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tCap))
    {
        TSimCapSet tMainCap = tCap.GetMainSimCapSet();
        TSimCapSet tSecCap = tCap.GetSecondSimCapSet();
        TDStreamCap tDCap = tCap.GetDStreamCapSet();
		//        m_cMtRcvGrp.RemoveMem(tMt.GetMtId(), tMainCap);
		//        m_cMtRcvGrp.RemoveMem(tMt.GetMtId(), tSecCap);
		//        m_cMtRcvGrp.RemoveMem(tMt.GetMtId(), tDCap);
    }
	
	if (m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt)
	{
		ClearH239TokenOwnerInfo(NULL);
	}
	
	//zbq [05/13/2010] ����ȴ��������
	BOOL32 bIsSendToChairman = FALSE;
    if (m_tApplySpeakQue.IsMtInQueue(tMt) )
    {
        m_tApplySpeakQue.ProcQueueInfo(tMt, bIsSendToChairman, FALSE);
        NotifyMcsApplyList( bIsSendToChairman );
    }
	
	if( VCS_CONF == m_tConf.GetConfSource() && MTLEFT_REASON_NORMAL == byReason &&
		!(tMt == m_tConf.GetChairman()) &&
		!(tMt == m_cVCSConfStatus.GetVcsBackupChairMan()) 
		)
	{		
		m_ptMtTable->SetCallMode( tMt.GetMtId(),CONF_CALLMODE_NONE );
	}

	if( m_byOtherConfMtId == tMt.GetMtId() )
	{
		SetInOtherConf( FALSE );
	}
	//��ӡ��Ϣ
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d: 0x%x(Dri:%d) droped from conf for the reason.%d!\n", 
		tMt.GetMtId(), m_ptMtTable->GetIPAddr(tMt.GetMtId()), tMt.GetDriId(), byReason );
	
	return;
}

/*====================================================================
    ������      ��ProcMtMcuMtStatusNotif
    ����        ���ն�״̬�ϱ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/05    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMtMcuMtStatusNotif(const CMessage * pcMsg)
{
    CServMsg   cServMsg( pcMsg->content, pcMsg->length);
    TMtStatus  tMtOldStatus;
    TMtStatus *ptMtStatus = (TMtStatus *)cServMsg.GetMsgBody();
    BOOL32     bStatusChange = FALSE;

    switch (CurState())
    {
    case STATE_ONGOING:
		{
        //�õ��ն�״̬
        m_ptMtTable->GetMtStatus( cServMsg.GetSrcMtId(), &tMtOldStatus );

        if (ptMtStatus->IsEnableFECC() != tMtOldStatus.IsEnableFECC() || 
            ptMtStatus->GetCurVideo() != tMtOldStatus.GetCurVideo()   || 
            ptMtStatus->GetCurAudio() != tMtOldStatus.GetCurAudio()   || 
            ptMtStatus->GetMtBoardType() != tMtOldStatus.GetMtBoardType() ||
            //zbq[08/02/2007] ��������Ҫ��������ΪVideoLose�ڱ��汾δ���ã���ԭ���ϴ˴�Ӧ��֧�ֵ���
            ptMtStatus->IsVideoLose() != tMtOldStatus.IsVideoLose() )
        {
            bStatusChange = TRUE;
        }

        //����ն�δ�ϱ���
        ptMtStatus->SetTMt(m_ptMtTable->GetMt(cServMsg.GetSrcMtId()));
        //ptMtStatus->SetVideoFreeze(tMtOldStatus.IsVideoFreeze());
        ptMtStatus->SetSelByMcsDrag(tMtOldStatus.GetSelByMcsDragMode());
        ptMtStatus->m_tVideoMt  = tMtOldStatus.m_tVideoMt;
        ptMtStatus->m_tAudioMt  = tMtOldStatus.m_tAudioMt;
        ptMtStatus->m_tRecState = tMtOldStatus.m_tRecState;
        ptMtStatus->SetSendVideo(tMtOldStatus.IsSendVideo());

        // ����˫��״̬��ʶ [02/05/2007-zbq]
		BOOL32 bSndVid2 = (m_tDoubleStreamSrc.GetType() == ptMtStatus->GetType() && m_tDoubleStreamSrc.GetMtId() == ptMtStatus->GetMtId()) ? TRUE : FALSE;
        ptMtStatus->SetSndVideo2( bSndVid2 );

        ptMtStatus->SetSendAudio(tMtOldStatus.IsSendAudio());
        ptMtStatus->SetInMixing(tMtOldStatus.IsInMixing());
        ptMtStatus->SetIsInMixGrp(tMtOldStatus.IsInMixGrp());
        ptMtStatus->SetReceiveVideo(tMtOldStatus.IsReceiveVideo());
        ptMtStatus->SetReceiveAudio(tMtOldStatus.IsReceiveAudio());        
        ptMtStatus->SetInTvWall(tMtOldStatus.IsInTvWall());
		ptMtStatus->SetInHdu(tMtOldStatus.IsInHdu());

        // ����Ҫ��¼ԭ��ѡ��
        ptMtStatus->SetSelectMt( tMtOldStatus.GetSelectMt(MODE_AUDIO), MODE_AUDIO );
        ptMtStatus->SetSelectMt( tMtOldStatus.GetSelectMt(MODE_VIDEO), MODE_VIDEO );
        
        //�����ն�״̬
        m_ptMtTable->SetMtStatus(cServMsg.GetSrcMtId(), ptMtStatus);
        
        if (!m_tCascadeMMCU.IsNull() && bStatusChange)
        {
            OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(), ptMtStatus->GetMtId() );
        }

        //֪ͨ���
		// [pengjie 2011/1/11] ����Ӧ��ֻ�ϱ����ն˵ľͿ�����
		//MtStatusChange();
		TMt tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
        MtStatusChange( &tMt );

        break;
	}
    default:
        ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
                pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
	}
}

/*====================================================================
    ������      ��ProcMtMcuInviteMtAck
    ����        ���ն˽������봦����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/05    1.0         LI Yi         ����
	06/01/06	4.0			�ű���		  ����ն˺��з���
====================================================================*/
void CMcuVcInst::ProcMtMcuInviteMtAck( const CMessage * pcMsg )
{
	STATECHECK
		
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	TMtAlias tMtH323Alias = *(TMtAlias*)cServMsg.GetMsgBody();
	TMtAlias tMtAddr = *((TMtAlias*)cServMsg.GetMsgBody()+1);
	u8 byType = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMtAlias)*2 );
    u8 byEncrypt = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMtAlias)*2 +1);

	if(byEncrypt == 0 && m_tConf.GetConfAttrb().GetEncryptMode() != CONF_ENCRYPTMODE_NONE)
	{
		//drop call
		cServMsg.SetMsgBody();
		SendMsgToMt( tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg );
		
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
		cServMsg.SetErrorCode(ERR_MCU_CONF_REJECT_NOENCYPT_MT);
		cServMsg.SetConfId( m_tConf.GetConfId() );
		SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg );
		
		return;
	}

	//�����ն�����
	if( byType == TYPE_MCU )
	{
		if( m_tConf.GetConfAttrb().IsSupportCascade() )
		{
			m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_SMCU );
		}
		else
		{
			m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_MT);
			SetInOtherConf( TRUE,tMt.GetMtId() );
		}		
	}
	else
	{
		// ׷����¼��ʵ������֧��,��Mtid��Ӧ��¼����ģʽ��Ϊ¼��������Ϊvrs��¼��ʵ��
		if (m_ptMtTable->GetRecChlType(tMt.GetMtId()) == TRecChnnlStatus::TYPE_RECORD
			|| m_ptMtTable->GetRecChlType(tMt.GetMtId()) == TRecChnnlStatus::TYPE_PLAY)
		{
			m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_VRSREC);
		}
		else
		{
			m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_MT); 
		}
	}

	//�����ն˱���
    TMtAlias tDialAlias;
    m_ptMtTable->GetDialAlias( tMt.GetMtId(), &tDialAlias );
    
    if(!tDialAlias.IsNull() && tDialAlias.m_AliasType ==  puAliasTypeIPPlusAlias)
    {
		//����ͬIP�����ϱ���323������һ�£�Ӧ�������ʱ�ı���[1/10/2013 chendaiwei]
        tDialAlias.m_AliasType = mtAliasTypeH323ID;
        m_ptMtTable->SetMtAlias( tMt.GetMtId(), &tDialAlias);
        m_ptMtTable->SetMtAlias( tMt.GetMtId(), &tMtAddr );
    }
    else
    {
		m_ptMtTable->SetMtAlias( tMt.GetMtId(), &tMtAddr );
		m_ptMtTable->SetMtAlias( tMt.GetMtId(), &tMtH323Alias);
	}

	//�����ն�IP��ַ
	if( tMtAddr.m_AliasType == mtAliasTypeTransportAddress )
	{
		m_ptMtTable->SetIPAddr( tMt.GetMtId(), tMtAddr.m_tTransportAddr.GetIpAddr() );
	}

	//����ն�Ϊ��������
	//m_ptMtTable->SetMtCallingIn( tMt.GetMtId(), FALSE );

	//�ն�����ʱͳһ�ϱ�[9/13/2012 chendaiwei]
	//SendMtAliasToMcs( tMt );
}

/*=============================================================================
  �� �� ���� SendMtAliasToMcs
  ��    �ܣ� �����ն˱�����mcs
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMt tMt
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::SendMtAliasToMcs(TMt tMt)
{
    CServMsg cServMsg;
    TMtAlias tMtH323IDAlias;
    TMtAlias tMtH320IDAlias;
    TMtAlias tMtH320Alias;
    TMtAlias tMtE164Alias;

    if( m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH320ID, &tMtH320IDAlias) )
    {
        cServMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));
        cServMsg.CatMsgBody((u8*)&tMtH320IDAlias, sizeof(tMtH320IDAlias));
        
        m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH320Alias, &tMtH320Alias);
        cServMsg.CatMsgBody((u8*)&tMtH320Alias, sizeof(tMtH320Alias));
    }
    else
    {
        m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH323ID, &tMtH323IDAlias);
        m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeE164, &tMtE164Alias);
        cServMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));
        cServMsg.CatMsgBody((u8*)&tMtH323IDAlias, sizeof(tMtH323IDAlias));
        cServMsg.CatMsgBody((u8*)&tMtE164Alias, sizeof(tMtE164Alias));
    }
    SendMsgToAllMcs(MCU_MCS_MTALIAS_NOTIF, cServMsg);

    return;
}

/*====================================================================
    ������      ��ProcMtMcuInviteMtNack
    ����        ���ն˾ܾ����봦����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/05    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMtMcuInviteMtNack( const CMessage * pcMsg )
{
    STATECHECK;

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );    
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
    u8 byReason = 0;
    u16 wErrorCode = cServMsg.GetErrorCode();
    if ( cServMsg.GetMsgBodyLen() > 0 )
    {
        byReason = *(cServMsg.GetMsgBody()+sizeof(TMtAlias));
    }

    ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Mt%d Invite Nack-%s.%d,Err%u\n", tMt.GetMtId(), 
             GetMtLeftReasonStr(byReason), byReason, wErrorCode );
    
	// �������¼��ʵ�壬�����¼�������Ϣ
	TRecChnnlStatus tRecStatus = m_ptMtTable->GetRecChnnlStatus(tMt.GetMtId());
	if (tRecStatus.m_byType == TRecChnnlStatus::TYPE_RECORD 
		|| tRecStatus.m_byType == TRecChnnlStatus::TYPE_PLAY)
	{
		ReleaseVrsMt(tMt.GetMtId(), wErrorCode);
		return;
	}

    CallFailureNotify(cServMsg);



// [pengjie 2010/6/3] �������ϱ��ܾ����ԭ��
// [zhushz 2010/11/20] �ܾ����ԭ���Ѿ��ϱ�
/*
	if( ERR_MCU_CALLMCUERROR_CONFISHOLDING == wErrorCode ||
		ERR_MCU_CASADEBYOTHERHIGHLEVELMCU == wErrorCode )
	{
		//NotifyMcsAlarmInfo( 0, wErrorCode );		
		cServMsg.SetEventId(MCU_MCS_ALARMINFO_NOTIF);
		cServMsg.SetErrorCode(wErrorCode);
		cServMsg.SetConfIdx( 0 );
		SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cServMsg);
	}
*/
// End
}


/*====================================================================
    ������      ��ProcMtMcuGetMtAliasReq
    ����        ���ն˲�ѯ�����ն˱���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/21    3.0         JQL           ����
	04/03/11    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::ProcMtMcuGetMtAliasReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = *( TMt* )cServMsg.GetMsgBody();
	TMtAlias    tMtAlias;

	switch( CurState() )
	{
	case STATE_SCHEDULED:
	case STATE_ONGOING:

		if( MCS_MCU_GETMTALIAS_REQ == cServMsg.GetEventId() && !tMt.IsLocal() )
		{
			cServMsg.SetErrorCode( ERR_MCU_OPRONLYLOCALMT );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	
			return;
		}

		cServMsg.SetMsgBody( ( u8* )&tMt, sizeof( tMt ) );
		
		//�����ն˱���ѯ�ն˱���
		if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias ) )
		{			
			cServMsg.CatMsgBody( ( u8* )&tMtAlias, sizeof( tMtAlias ) );
            
            if(m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH320Alias, &tMtAlias))
            {
                cServMsg.CatMsgBody( ( u8* )&tMtAlias, sizeof( tMtAlias ) );
            }
		}
		else
        {
			if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias ) )
			{
				cServMsg.CatMsgBody( ( u8* )&tMtAlias, sizeof( tMtAlias ) );
			}
			if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeE164, &tMtAlias ) )
			{
				cServMsg.CatMsgBody( ( u8* )&tMtAlias, sizeof( tMtAlias ) );
			}
            if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias ) )
		    {			
			    cServMsg.CatMsgBody( ( u8* )&tMtAlias, sizeof( tMtAlias ) );
		    }
		}

		if( cServMsg.GetMsgBodyLen() > sizeof(TMt) )
		{
			SendReplyBack( cServMsg, pcMsg->event + 1 );
		}
		else
		{
			SendReplyBack( cServMsg, pcMsg->event + 2 );
		}

		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}	
}

/*====================================================================
    ������      ��ProcMcsMcuGetAllMtAliasReq
    ����        ����ѯ�����ն˱���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/21    3.0         JQL           ����
	04/04/27    3.0         ������        �޸�
	05/01/29	3.6			Jason         �޸�
    06/12/14	4.0			����        �µĴ����ʽ���������б���
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetAllMtAliasReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
    u8          abyBuf[SERV_MSG_LEN-SERV_MSGHEAD_LEN] = {0};
    u8*         ptrBuf = abyBuf;
    u16         wBufLen = 0;

	TMcu        tMcu;

    u8 byMtId = 0;
    u8 byAliasNum = 0;
    u8 byValidMtNum = 0;

	u16 wMcuIdx = INVALID_MCUIDX;

	switch( CurState() )
	{
	case STATE_ONGOING:

	    SendReplyBack( cServMsg, pcMsg->event + 1 );

		//�õ�MCU
		tMcu = *(TMcu*)cServMsg.GetMsgBody();         

		//�����ն�״̬
		if( ( INVALID_MCUIDX == tMcu.GetMcuIdx() ) &&
			 m_ptConfOtherMcTable != NULL)
		{
            // �Ǳ���ֻ��һ������
            byAliasNum = 1;
            u8 byAliasType = mtAliasTypeH323ID;

			//�ȷ�����MC
			SendMtListToMcs(INVALID_MCUIDX);
/*			TConfMcInfo *ptConfMcInfo = NULL;
			for( u16 wLoop = 0; wLoop < TConfOtherMcTable::GetMaxMcuNum(); wLoop++ )
			{                
				ptConfMcInfo = (m_ptConfOtherMcTable->GetMcInfo(wLoop));
				if(	NULL == ptConfMcInfo
					|| !IsValidSubMcuId(ptConfMcInfo->GetMcuIdx()))
				{
					continue;
				}

                // McuID
                ptrBuf = abyBuf ;
				wMcuIdx = ptConfMcInfo->GetMcuIdx();
				wMcuIdx = htons( wMcuIdx );
                memcpy(ptrBuf, &wMcuIdx, sizeof(u16));

                // ����MtNum
                ptrBuf += 3;
                wBufLen = 3;

                byValidMtNum = 0;
                
				for( u8 byLoop1 = 0; byLoop1 < MAXNUM_CONF_MT; byLoop1++ )
				{
					TMtExt *ptMtExt = &(ptConfMcInfo->m_atMtExt[byLoop1]);
					if ( ptMtExt->IsNull() ||
					  	 ptMtExt->GetMtId() == 0) //�����Լ�
					{
						continue;
					}
                    byMtId = ptMtExt->GetMtId();
                    byValidMtNum ++;
										
                    // MtID
                    memcpy(ptrBuf, &byMtId, sizeof(u8));
                    ptrBuf++;                    
                    // Alias Num = 1
                    memcpy(ptrBuf, &byAliasNum, sizeof(u8));
                    ptrBuf++;       
                    // Alias Type = H323ID
                    memcpy(ptrBuf, &byAliasType, sizeof(u8));
                    ptrBuf++;
                    wBufLen += 3;

                    // Alias Len
					u8 byAliasLen = strlen(ptMtExt->GetAlias());
                    memcpy(ptrBuf, &byAliasLen, sizeof(u8));
                    ptrBuf++;
                    wBufLen++;
					if (byAliasLen>0)
                    {
                        // Alias
                        memcpy(ptrBuf, (u8 *)ptMtExt->GetAlias(), byAliasLen);
                        ptrBuf += byAliasLen;
                        wBufLen += byAliasLen;
                    }
				}
                abyBuf[2] = byValidMtNum;
                cServMsg.SetMsgBody( abyBuf, wBufLen );
				SendReplyBack( cServMsg, MCU_MCS_ALLMTALIAS_NOTIF );
			}
*/
			//���������
			tMcu.SetNull();
			tMcu.SetMcu( LOCAL_MCUID );

			tMcu.SetMcuIdx( LOCAL_MCUIDX );
		}

		//���Ǳ�����MCU
		if( ! tMcu.IsLocal() && m_ptConfOtherMcTable != NULL)
		{
			TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMcu.GetMcuId());
			if( ptConfMcInfo == NULL )
			{
				return;
			}
			if( ptConfMcInfo->IsNull() )
			{
				return;
			}

            byAliasNum = 1;
            u8 byAliasType = mtAliasTypeH323ID;

            // McuID
            ptrBuf = abyBuf ;
			wMcuIdx = ptConfMcInfo->GetMcuIdx();
			wMcuIdx = htons( wMcuIdx );
            memcpy(ptrBuf, &wMcuIdx, sizeof(u16));

            // ����MtNum
            ptrBuf += 3;
            wBufLen = 3;

            byValidMtNum = 0;
            
			for( u8 byLoop1 = 0; byLoop1 < MAXNUM_CONF_MT; byLoop1++ )
			{
				TMtExt *ptMtExt = &(ptConfMcInfo->m_atMtExt[byLoop1]);
				if ( ptMtExt->IsNull() ||
					 ptMtExt->GetMtId() == 0) //�����Լ�
				{
					continue;
				}
                byMtId = ptMtExt->GetMtId();
                byValidMtNum ++;
									
                // MtID
                memcpy(ptrBuf, &byMtId, sizeof(u8));
                ptrBuf++;                    
                // Alias Num = 1
                memcpy(ptrBuf, &byAliasNum, sizeof(u8));
                ptrBuf++;       
                // Alias Type = H323ID
                memcpy(ptrBuf, &byAliasType, sizeof(u8));
                ptrBuf++;
                wBufLen += 3;

                // Alias Len
				u8 byAliasLen = strlen(ptMtExt->GetAlias());
                memcpy(ptrBuf, &byAliasLen, sizeof(u8));
                ptrBuf++;
                wBufLen++;

				if (byAliasLen>0)
                {
                    // Alias
                    memcpy(ptrBuf, (u8 *)ptMtExt->GetAlias(), byAliasLen);
                    ptrBuf += byAliasLen;
                    wBufLen += byAliasLen;
                }
			}

            abyBuf[1] = byValidMtNum;
            cServMsg.SetMsgBody( abyBuf, wBufLen );
            SendReplyBack( cServMsg, MCU_MCS_ALLMTALIAS_NOTIF );
		}
		else
		{
            // McuID
            ptrBuf = abyBuf ;
            //abyBuf[0] = LOCAL_MCUID;


			wMcuIdx = LOCAL_MCUIDX;
			wMcuIdx = htons( wMcuIdx );
			memcpy(ptrBuf, &wMcuIdx, sizeof(u16));

            // ����MtNum
            ptrBuf += 3;
            wBufLen = 3;

            byValidMtNum = 0;
            
            u8* ptrBufTmp = NULL;
            TMtAlias tMtAlias;
            byAliasNum = 0;
			for( u8 byLoopMtId = 1; byLoopMtId <= MAXNUM_CONF_MT; byLoopMtId++ )
			{
				if( m_tConfAllMtInfo.MtInConf( byLoopMtId ) )
				{
					byValidMtNum ++;
                    
                    // MtID
                    memcpy(ptrBuf, &byLoopMtId, sizeof(u8));
                    ptrBuf++;  
                    wBufLen++;
                    
                    // ����Alias Num
                    ptrBufTmp = ptrBuf;
                    ptrBuf++;  
                    wBufLen++;
                    byAliasNum = 0;

                    for (u8 byLoopAliasType = mtAliasTypeE164; byLoopAliasType < mtAliasTypeOthers; byLoopAliasType ++)
                    {
                        tMtAlias.SetNull();
                        if (m_ptMtTable->GetMtAlias(byLoopMtId, (mtAliasType)byLoopAliasType, &tMtAlias))
                        {
                            byAliasNum ++;

                            // Alias Type
                            memcpy(ptrBuf, &byLoopAliasType, sizeof(u8));
                            ptrBuf++;
                            wBufLen++;     
                            
                            // Alias Len
                            u8 byAliasLen = strlen(tMtAlias.m_achAlias);
                            memcpy(ptrBuf, &byAliasLen, sizeof(u8));
                            ptrBuf++;
                            wBufLen++;

				            if (byAliasLen>0)
                            {
                                // Alias
                                memcpy(ptrBuf, (u8 *)tMtAlias.m_achAlias, byAliasLen);
                                ptrBuf += byAliasLen;
                                wBufLen += byAliasLen;
                            }
                        }
                    }

                    // ����Alias Num
                    memcpy(ptrBufTmp, &byAliasNum, sizeof(u8));
                }
			}

            // ����MtNum
            abyBuf[2] = byValidMtNum;
            cServMsg.SetMsgBody( abyBuf, wBufLen );
            SendReplyBack( cServMsg, MCU_MCS_ALLMTALIAS_NOTIF );
		}

		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}	
}

/*=============================================================================
  �� �� ���� ProcMcsMcuGetMtBitrateReq
  ��    �ܣ� ��ز�ѯ�ն�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage *pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::ProcMcsMcuGetMtBitrateReq( const CMessage *pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    if(cServMsg.GetMsgBodyLen() != sizeof(TMt))
    {
        cServMsg.SetEventId(pcMsg->event+2);
        SendReplyBack(cServMsg, pcMsg->event+2);
        return;
    }

    TMtStatus tMtStatus;
    TMt tMt = *(TMt *)cServMsg.GetMsgBody();

    switch( CurState() )
    {
    case STATE_ONGOING:
    case STATE_SCHEDULED:
        if(tMt.IsNull())//�����ն�
        {
            cServMsg.SetMsgBody(NULL, 0);
            for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
            {
                if( m_tConfAllMtInfo.MtInConf( byLoop ) ) 
                {
                    //�����ն�������Ϣ
                    SendMsgToMt(byLoop, MCU_MT_GETBITRATEINFO_REQ, cServMsg);
                }
            }
        }
        else//�ض��ն�
        {
            if( m_tConfAllMtInfo.MtInConf( tMt.GetMtId() )) 
            {
                cServMsg.SetMsgBody( (u8*)((TMtBitrate *)&tMtStatus), sizeof(TMtBitrate) );
                
                //�����ն�������Ϣ
                SendMsgToMt(tMt.GetMtId(), MCU_MT_GETBITRATEINFO_REQ, cServMsg);
            }
        }

        SendReplyBack(cServMsg, pcMsg->event+1); 
        break;

    default:
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        break;
    }

    return;   
}

/*====================================================================
    ������      ��ProcMcsMcuGetMtExtInfoReq
    ����        ���ն˲�ѯ�ն˵���չ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/12/18    4.5         �ű���           ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetMtExtInfoReq( const CMessage *pcMsg )
{
    STATECHECK;

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TMt tMt = *(TMt*)cServMsg.GetMsgBody();
    if ( !tMt.IsNull() )
    {
        if ( m_tConfAllMtInfo.MtInConf(tMt.GetMtId()))
        {
            if ( 0 == m_ptMtTable->GetHWVerID(tMt.GetMtId()) ||
                 NULL == m_ptMtTable->GetSWVerID(tMt.GetMtId()))
            {
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[GetMtExtInfo] Mt.%d verInfo unexist, nack\n", tMt.GetMtId() );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            }
            else
            {
                TMtExt2 tMtExt2;
                tMtExt2.SetMt(tMt);
                tMtExt2.SetHWVerId(m_ptMtTable->GetHWVerID(tMt.GetMtId()));
                tMtExt2.SetSWVerId(m_ptMtTable->GetSWVerID(tMt.GetMtId()));
                cServMsg.SetMsgBody((u8*)&tMtExt2, sizeof(tMtExt2));
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
            }            
        }
    }
    else
    {
        for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
        {
            if ( m_tConfAllMtInfo.MtInConf(byMtId))
            {
                tMt = m_ptMtTable->GetMt(byMtId);
            
                if ( 0 == m_ptMtTable->GetHWVerID(tMt.GetMtId()) ||
                     NULL == m_ptMtTable->GetSWVerID(tMt.GetMtId()))
                {
                    ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[GetMtExtInfo] Mt.%d verInfo unexist, nack\n", tMt.GetMtId() );
                    continue;
                }
                TMtExt2 tMtExt2;
                tMtExt2.SetMt(tMt);
                tMtExt2.SetHWVerId(m_ptMtTable->GetHWVerID(byMtId));
                tMtExt2.SetSWVerId(m_ptMtTable->GetSWVerID(byMtId));
            
                if ( 0 != cServMsg.GetMsgBodyLen() )
                {
                    cServMsg.CatMsgBody((u8*)&tMtExt2, sizeof(tMtExt2));
                }
                else
                {
                    cServMsg.SetMsgBody((u8*)&tMtExt2, sizeof(tMtExt2));
                }
            }
        }
        if ( cServMsg.GetMsgBodyLen() > 0 )
        {
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
        }
    }

    return;
}

/*====================================================================
    ������      ��ProcMtMcuJoinedMtListReq
    ����        ���ն˲�ѯ��������ն˺�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/21    3.0         JQL           ����
====================================================================*/
void CMcuVcInst::ProcMtMcuJoinedMtListReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt;
	u8			byMtId;

	switch( CurState() )
	{
	case STATE_ONGOING:
		if( !m_tConfAllMtInfo.MtJoinedConf( cServMsg.GetSrcMtId() ) )	//not in conference
		{
			SendReplyBack( cServMsg, pcMsg->event + 2 );
			break;
		}

		cServMsg.SetMsgBody( NULL, 0 );
		//�������Ա��Ϣ���͸��ն�����Ự
		for( byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
		{
			if( m_tConfAllMtInfo.MtJoinedConf( byMtId ) )
			{
				tMt.SetMt( LOCAL_MCUID, byMtId );
				cServMsg.CatMsgBody( (u8*)&tMt, sizeof( tMt ) );
			}

		}
		
		SendReplyBack( cServMsg, pcMsg->event + 1 );
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}	

}

/*====================================================================
    ������      ��ProcMtMcuJoinedMtListIdReq
    ����        ���ն˲�ѯ��������ն˺�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/21    3.0         JQL           ����
====================================================================*/
void CMcuVcInst::ProcMtMcuJoinedMtListIdReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			atMt[MAXNUM_CONF_MT];
	TMtAlias	atMtAlias[MAXNUM_CONF_MT];
	u8			abyBuff[MAXNUM_CONF_MT*( sizeof(TMt) + sizeof(TMtAlias) )];
	u8			byMtNum = 0;
	u8			byMtId;
	u32			dwBuffSize = 0;

	switch( CurState() )
	{
	case STATE_ONGOING:

		memset( atMt, 0, sizeof(atMt) );
		memset( atMtAlias, 0, sizeof(atMtAlias) );
		memset( abyBuff, 0, sizeof(abyBuff) );
		for( byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
		{
			if( m_tConfAllMtInfo.MtJoinedConf( byMtId ) )
			{
				atMt[byMtNum] = m_ptMtTable->GetMt( byMtId );
				atMtAlias[byMtNum].SetNull();
                if(!m_ptMtTable->GetMtAlias(byMtId, mtAliasTypeH320Alias, &atMtAlias[byMtNum]))
                {
                    if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeH320ID, &atMtAlias[byMtNum] ))
                    {
                        if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeH323ID, &atMtAlias[byMtNum] ))
                        {
                            if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeE164, &atMtAlias[byMtNum] ))
                            {					
                                m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeTransportAddress, &atMtAlias[byMtNum] );
                            }
                        }
                    }
                }				
				byMtNum++;
			}
		}
		
		cServMsg.SetMsgBody( NULL, 0 );

		dwBuffSize = PackMtInfoListIntoBuf( atMt, atMtAlias, byMtNum, abyBuff, sizeof( abyBuff ) );
		//Attation please, the buffer size may exceed ONE CServMsg that can hold//
		cServMsg.SetMsgBody( abyBuff, (u16)dwBuffSize );
		SendReplyBack( cServMsg, pcMsg->event + 1 );
		
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}	

}

/*====================================================================
    ������      ��PackMtInfoListIntoBuf
    ����        ���ն˱����б���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt atMt[] �ն�����
	              const TMtAlias atMtAlias[] �ն˱������� 
				  u8 byArraySize  �����С
				  u8 *pbyBuf ������
				  u32 dwBufLen ���泤��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/21    3.0         JQL           ����
====================================================================*/
u32 CMcuVcInst::PackMtInfoListIntoBuf( const TMt atMt[], const TMtAlias atMtAlias[], u8 byArraySize, u8 *pbyBuf, u32 dwBufLen )
{

#define  TRY_MEM_COPY( dst, src, len, limit, result ) \
	do\
	{\
		if((u32)(dst) + (len) > (u32)(limit)) \
		{\
			OspLog(LOGLVL_DEBUG1, "insufficient memory to store the data.\n"); \
			result = FALSE; \
		} \
		memcpy((dst), (src), (len) ); \
		result = TRUE;\
	}\
	while (0)	
	
	
	u8 *pbyTmp = pbyBuf;
	u8 *pbyMemLimit = pbyBuf + dwBufLen;
	BOOL32 bResult;

	for(int i = 0; i < byArraySize; i++)
	{
		TRY_MEM_COPY(pbyTmp, (void*)&atMt[i], sizeof(TMt), pbyMemLimit, bResult);
		if( !bResult )
			return (pbyTmp - pbyBuf);
		pbyTmp += sizeof(TMt);

		u8 type = atMtAlias[i].m_AliasType;		
		TRY_MEM_COPY(pbyTmp, &type, sizeof(type), pbyMemLimit, bResult );
		if( !bResult )
			return (pbyTmp - pbyBuf);
		pbyTmp += sizeof(type);			

		if(type == mtAliasTypeH323ID || type == mtAliasTypeE164 || type == mtAliasTypeH320ID || type == mtAliasTypeH320Alias)
		{
			u16 aliasLen = htons( strlen(atMtAlias[i].m_achAlias) );
			TRY_MEM_COPY(pbyTmp, &aliasLen, sizeof(aliasLen), pbyMemLimit,bResult);
			if( !bResult )
				return (pbyTmp - pbyBuf);
			pbyTmp += sizeof(aliasLen);

			u16 wHostValue = ntohs(aliasLen);
			TRY_MEM_COPY(pbyTmp, (void*)atMtAlias[i].m_achAlias, wHostValue, pbyMemLimit,bResult);
			if( !bResult )
				return (pbyTmp - pbyBuf);
			pbyTmp += ntohs(aliasLen);
		}
		else if(type == mtAliasTypeTransportAddress)
		{
			TRY_MEM_COPY(pbyTmp, (void*)&(atMtAlias[i].m_tTransportAddr), 
				sizeof(TTransportAddr), pbyMemLimit, bResult );
			if( !bResult )
				return (pbyTmp - pbyBuf);
			pbyTmp += sizeof(TTransportAddr);
		}
	}
	
	return (pbyTmp - pbyBuf);

#undef TRY_MEM_COPY
	
}

/*====================================================================
    ������      ��ProcMtMcuApplySpeakerNotif
    ����        ���ն����뷢��
    �㷨ʵ��    �������������ύ����ϯ
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/21    3.0          JQL          ����
====================================================================*/
void CMcuVcInst::ProcMtMcuApplySpeakerNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

    TMt tMt;
    TMt tChairman;
	
    
	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );


	switch( CurState() )
	{
	case STATE_ONGOING:
        {
			//δ���
			if( !m_tConfAllMtInfo.MtJoinedConf( tMt ) )
			{								
				return;
			}			
					
			//����ϯ
			if( HasJoinedChairman() )
			{
				tChairman = m_tConf.GetChairman();
				
				//��ϯ���뷢��,ͬ��
            
				if( tMt.IsLocal() && 
					tChairman.GetMtId() == tMt.GetMtId() &&
					//zbq[06/11/2010] VCS����ͨ��UIȷ�ϲſ���
					VCS_CONF != m_tConf.GetConfSource() )
				{
					// xsl [8/22/2006]���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
					if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsMultiCastMt(tMt.GetMtId())
						&& //IsSatMtOverConfDCastNum(tMt))
						!IsSatMtCanContinue(tChairman.GetMtId(),emChairMan))
					{
						if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
						{
							cServMsg.SetEventId(pcMsg->event + 2);
							SendReplyBack(cServMsg, cServMsg.GetEventId());
						}

						ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuApplySpeakerNotif] over max upload mt num. ignore it!\n");
					}
					else
					{
						if( !tMt.IsLocal() )
						{
							OnMMcuSetIn( tMt,m_cVCSConfStatus.GetCurSrcSsnId(),SWITCH_MODE_BROADCAST );
						}			

						ChangeSpeaker( &tMt );						
					}				
					return;
				}

				// xsl [11/3/2006] �������ļ������Ƿ��ն�������Ϣ֪ͨ��ϯ�ն�
				if (g_cMcuVcApp.IsChairDisplayMtApplyInfo())
				{
					//֪ͨ��ϯ
					cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
					cServMsg.SetDstMtId( tChairman.GetMtId() );			
					SendMsgToMt( tChairman.GetMtId(), MCU_MT_APPLYSPEAKER_NOTIF, cServMsg );
				}            
				else
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuApplySpeakerNotif] not permit send message to chair mt for mt apply speaker\n");
				}
			}			

			//֪ͨ���
			cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
			SendMsgToAllMcs( MCU_MCS_MTAPPLYSPEAKER_NOTIF, cServMsg );

        }
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

}

/*====================================================================
������      ��NotifyMcsApplyList
����        ��ˢUI��ǰ���µ�LIST
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const CMessage * pcMsg, �������Ϣ
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
05/13/2010  4.6         �ű���         ����
====================================================================*/
void CMcuVcInst::NotifyMcsApplyList( BOOL32 bSendToChairman /*= FALSE*/ )
{
    u8 byLen = MAXNUM_CONF_MT;
    TMt atMt[MAXNUM_CONF_MT];
    m_tApplySpeakQue.GetQueueList(atMt, byLen);
    
	
    CServMsg cServMsg;
	cServMsg.SetConfIdx( m_byConfIdx );
	cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8*)&byLen, sizeof(u8));
	if (0 != byLen)
	{
		cServMsg.CatMsgBody((u8*)atMt, sizeof(TMt) * byLen);
	}
    
    SendMsgToAllMcs( MCU_MCS_MTAPPLYSPEAKERLIST_NOTIF, cServMsg );
	if( bSendToChairman && VCS_CONF == m_tConf.GetConfSource() && 
		CONF_SPEAKMODE_ANSWERINSTANTLY == m_tConf.GetConfSpeakMode() 
		)
	{
		CRollMsg cRollMsg;
		cRollMsg.Reset();
		TMt tDstMt = m_tConf.GetChairman();
		u8 achTemp[255];
		memset( &achTemp[0],0,sizeof(achTemp) );
		cRollMsg.SetType( ROLLMSG_TYPE_SMS );
		cRollMsg.SetRollTimes( 0xff );//��Ϊ0���������޹�����������һ����һ���ֵ
		cRollMsg.SetRollRate( 1 );
		cRollMsg.SetMsgSrcMtId( LOCAL_MCUID,0 );
		cRollMsg.SetRollMsgContent();

		
		if( byLen > 0 )
		{
			strcat( (s8*)&achTemp[0],"��ǰ�����б�:" );
			for( u8 byLoop = 0;byLoop < MAXNUM_ANSWERINSTANTLY_MT && byLoop < byLen;++byLoop )
			{			
				if( byLoop > 0 )
				{
					strcat( (s8*)&achTemp[0],"��" );
				}
				GetMtAlias( atMt[byLoop],(s8*)(&achTemp[0] + strlen((s8*)&achTemp[0])) );			
			}
			cRollMsg.SetRollMsgContent( &achTemp[0],strlen((s8*)&achTemp[0]) + 1 );		
		}
		
		
		ProcSingleMtSmsOpr( cServMsg,&tDstMt,&cRollMsg );
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT,  "[NotifyMcsApplyList] Send Msg To Chairman,Msg(%s)\n",&achTemp[0] );
	}
    
    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[NotifyMcsApplyList] MCU_MCS_MTAPPLYSPEAKERLIST_NOTIF with Len.%d!\n", byLen);
	
    return;
}

/*====================================================================
    ������      ��ProcMtMcuApplyChairmanReq
    ����        ���ն�������ϯ������
    �㷨ʵ��    ���������ϯֱ�Ӹ�����ϯ���ƣ����ܾ���֪ͨ��ϯ
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/01/02    1.0         LI Yi         ����
====================================================================*/
void CMcuVcInst::ProcMtMcuApplyChairmanReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	TMt	tChairman;

	switch( CurState() )
	{
	case STATE_ONGOING:

		//����ϯģʽ,�ܾ�
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) || m_tConf.m_tStatus.IsNoChairMode() )
		{
			cServMsg.SetErrorCode( ERR_MCU_NOCHAIRMANMODE );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				
			return;
		}
		
		//����ϯʱ
		if( HasJoinedChairman() && m_tConf.GetChairman().GetMtId() != tMt.GetMtId() )
		{
			
			cServMsg.SetErrorCode( ERR_MCU_CHAIRMANEXIST );
			SendReplyBack( cServMsg, pcMsg->event + 2 );

            // xsl [11/3/2006] �������ļ������Ƿ��ն�������Ϣ֪ͨ��ϯ�ն�
            if (g_cMcuVcApp.IsChairDisplayMtApplyInfo())
            {
                tChairman = m_tConf.GetChairman();
			    cServMsg.SetSrcMtId( tMt.GetMtId() );
			    cServMsg.SetDstMtId( tChairman.GetMtId() );
			    cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
			    SendMsgToMt( tChairman.GetMtId(), MCU_MT_APPLYCHAIRMAN_NOTIF, cServMsg );
            }	
            else
            {
                ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuApplyChairmanReq] not permit send message to chair mt for mt apply chair\n");
            }

			//���������
			cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
			SendMsgToAllMcs( MCU_MCS_MTAPPLYCHAIRMAN_NOTIF, cServMsg );
		}
		
		//����ϯ
		else
		{
			SendReplyBack( cServMsg, pcMsg->event + 1 );
			ChangeChairman( &tMt );
		}

		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��DaemonProcMtMcuApplyJoinReq
    ����        ���ն����������鴦����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/06/20    4.0         ����   	  ����
====================================================================*/
void CMcuVcInst::DaemonProcMtMcuApplyJoinReq(BOOL32 bLowLevleMcuCalledIn, const CConfId &cConfId, const CMessage * pcMsg)
{
    if (!bLowLevleMcuCalledIn)
    {
        g_cMcuVcApp.SendMsgToConf( cConfId, pcMsg->event, pcMsg->content, pcMsg->length );
    }
    else
    {
        CServMsg cServMsg(pcMsg->content, pcMsg->length);
        TMtAlias tMtH323Alias = *(TMtAlias *)cServMsg.GetMsgBody();
        TMtAlias tMtE164Alias = *(TMtAlias *)(cServMsg.GetMsgBody() + sizeof(TMtAlias));
        TMtAlias tMtAddr      = *(TMtAlias *)(cServMsg.GetMsgBody() + sizeof(TMtAlias)*2);
	    u16      wCallRate = *(u16*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)));

        // guzh [6/19/2007] �¼�MCU�������У���Ϊ�����¼�MCU�������
        TAddMtInfo tAddMtInfo;
        u8 byOnGoingConfIdx = g_cMcuVcApp.GetConfIdx(cConfId);
        cServMsg.SetConfIdx(byOnGoingConfIdx);            
        cServMsg.SetConfId(cConfId);
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
        tAddMtInfo.SetCallBitRate(wCallRate);//mtadpʵ�����������͸�ֵ[1/16/2013 chendaiwei]
        TMcu tLocalMcu;
        tLocalMcu.SetMcu(LOCAL_MCUID);
        cServMsg.SetMsgBody((u8*)&tLocalMcu, sizeof(TMcu));
        cServMsg.CatMsgBody((u8*)&tAddMtInfo, sizeof(TAddMtInfo));
		// xliang [9/20/2008] ���ӱ��
		cServMsg.CatMsgBody((u8*)&bLowLevleMcuCalledIn,sizeof(BOOL32));
        g_cMcuVcApp.SendMsgToConf( byOnGoingConfIdx, MCS_MCU_ADDMT_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

    }
}
/*====================================================================
    ������      ��DaemonProcMtMcuApplyJoinReq
    ����        ���ն����������鴦���������������б�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/21    1.0         JQL			  ����
	04/02/19    3.0         ������        �޸�
    06/06/22    4.0         ����        �����ն�IP����
====================================================================*/
void CMcuVcInst::DaemonProcMtMcuApplyJoinReq(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	TMtAlias tMtH323Alias = *(TMtAlias *)cServMsg.GetMsgBody();
	TMtAlias tMtE164Alias = *(TMtAlias *)(cServMsg.GetMsgBody() + sizeof(TMtAlias));
	TMtAlias tMtAddr      = *(TMtAlias *)(cServMsg.GetMsgBody() + sizeof(TMtAlias)*2);
	TMtAlias tConfE164    = *(TMtAlias *)(cServMsg.GetMsgBody() + sizeof(TMtAlias)*3);
	u8       byType    = *(cServMsg.GetMsgBody()+4*sizeof(TMtAlias));
	u8       byEncrypt = *(cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8));
	u16      wCallRate = *(u16*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)));
    // guzh [6/19/2007] 
    BOOL32   bLowLevelMcuCalledIn = *(BOOL32*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8))+sizeof(u16));
	
	TConfInfo    *ptConfInfo = NULL;
	TConfMtTable *ptConfMtTable = NULL;
	u8 byNum = 0;
	u8 byMtId = 0;
	CConfId       cConfId = cServMsg.GetConfId();

    // �Ƿ���Ҫ��ʼȱʡ����
    BOOL32        bStartDefConf = FALSE;

	// ��ʱ�������
	byNum = g_cMcuVcApp.GetConfNum( TRUE, FALSE, FALSE );

	//MCU���޻���
	if (0 == byNum)
	{
        bStartDefConf = TRUE;
    }

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq] ConfNum.%d\n", byNum);

	// ��ĳһ����Ļ����ƥ�䣬���û��Ѿ�ѡ����ĳ������
	if (!cConfId.IsNull() && 0 != g_cMcuVcApp.GetConfIdx(cConfId))
	{
        // ֪ͨ���
		g_cMcuVcApp.SendMsgToConf( cConfId, pcMsg->event, pcMsg->content, pcMsg->length );
		return;
	}
    
	// ��ĳһ����������ն��б��У��ҵ�һ������
	for (u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
	{
		if (NULL != g_cMcuVcApp.GetConfInstHandle(byConfIdx))
		{
			ptConfInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
			ptConfMtTable = g_cMcuVcApp.GetConfMtTable(byConfIdx);

            // zbq [08/08/2007] �����µ��ն˼���ʽ������Ҫ����ϸ�϶�������Ͷ����飬��ɺ�������
            for ( u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++ )
            {
                TMtAlias tDialAlias;
                if ( ptConfMtTable->GetDialAlias(byMtIdx, &tDialAlias) &&
                     !tDialAlias.IsNull() )
                {
                    TMtAlias tInConfMtAddr;
                    
                    if ( mtAliasTypeE164 == tDialAlias.m_AliasType &&
                         tMtE164Alias == tDialAlias &&
                         //��ʹ�к���E164����ƥ�䣬ҲҪУ��һ��Ip����ֹδע��GK(��ע���˲�ͬ�ķ��ھ�GK)���ն�ð������
                         ptConfMtTable->GetMtAlias(byMtIdx, mtAliasTypeTransportAddress, &tInConfMtAddr) &&
                         tInConfMtAddr.m_tTransportAddr.GetIpAddr() == tMtAddr.m_tTransportAddr.GetIpAddr())
                    {
                        byMtId = byMtIdx;
                        break;
                    }
                    else if ( mtAliasTypeH323ID == tDialAlias.m_AliasType &&
                              tMtH323Alias == tDialAlias &&
                              //��ʹ�к���E164����ƥ�䣬ҲҪУ��һ��Ip����ֹδע��GK(��ע���˲�ͬ�ķ��ھ�GK)���ն�ð������
                              ptConfMtTable->GetMtAlias(byMtIdx, mtAliasTypeTransportAddress, &tInConfMtAddr) &&
                              tInConfMtAddr.m_tTransportAddr.GetIpAddr() == tMtAddr.m_tTransportAddr.GetIpAddr())
                    {
                        byMtId = byMtIdx;
                        break;
                    }
                    else if( mtAliasTypeTransportAddress == tDialAlias.m_AliasType &&
                             tMtAddr.m_tTransportAddr.GetIpAddr() == tDialAlias.m_tTransportAddr.GetIpAddr() )
                    {
                        byMtId = byMtIdx;
                        break;
                    }
                }
            }
			//byMtId = ptConfMtTable->GetMtIdByAlias( &tMtAddr );
            
			TConfAllMtInfo *pConfAllMt = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConfAllMtInfo;
			if (byMtId > 0 && pConfAllMt->MtInConf( byMtId ))
			{
// 				g_cMcuVcApp.SendMsgToConf( ptConfInfo->GetConfId(), pcMsg->event, pcMsg->content, pcMsg->length ); 
				DaemonProcMtMcuApplyJoinReq(bLowLevelMcuCalledIn, ptConfInfo->GetConfId(), pcMsg );
				return;
			}
            /*
			byMtId = ptConfMtTable->GetMtIdByAlias( &tMtE164Alias );
			if (byMtId > 0 && pConfAllMt->MtInConf( byMtId ))
			{
				g_cMcuVcApp.SendMsgToConf( ptConfInfo->GetConfId(), pcMsg->event, pcMsg->content, pcMsg->length );                
				return;
			}
			byMtId = ptConfMtTable->GetMtIdByAlias( &tMtH323Alias );
			if (byMtId > 0 && pConfAllMt->MtInConf( byMtId ))
			{
				g_cMcuVcApp.SendMsgToConf( ptConfInfo->GetConfId(), pcMsg->event, pcMsg->content, pcMsg->length );                
				return;
			}
            */
		}
	}

	TConfNameInfo  tConfList;
    s32 nConfNum = 0;

	CMcuVcInst *pInstance = NULL;
	// ׼�������б�
	cServMsg.SetMsgBody( );
    
	for (u8 byConfIdx1 = MIN_CONFIDX; byConfIdx1 <= MAX_CONFIDX; byConfIdx1++)
	{
		pInstance = g_cMcuVcApp.GetConfInstHandle( byConfIdx1 );
		if (NULL != pInstance )
		{
		
			TConfInfo    * ptLoopConfInfo = &(pInstance->m_tConf);	
			if (ptLoopConfInfo == NULL )
			{
				 ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq] ptConfInfo is Null confidx.%d\n", byConfIdx1);
				 continue;
			}

			if ( !ptLoopConfInfo->m_tStatus.IsOngoing() )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq]confidx.%d is not a ongoing conf,not countin\n", byConfIdx1);
				continue;
			}
            ptConfInfo = ptLoopConfInfo;
			if ( CONF_OPENMODE_CLOSED != ptConfInfo->GetConfAttrb().GetOpenMode() && // ���������ն����
                 ( byType == TYPE_MCU ||    //admin�û����Ļ������������������ն˺���
                   g_cMcuVcApp.IsMtIpInAllowSeg( ptConfInfo->GetUsrGrpId(), tMtAddr.m_tTransportAddr.GetNetSeqIpAddr() ) )  ) // �����ն��ڸ���������
			{
                nConfNum ++;

				strncpy( tConfList.achConfName, ptConfInfo->GetConfName(), sizeof( tConfList.achConfName ) );
				tConfList.achConfName[sizeof( tConfList.achConfName) - 1 ] = 0;					
				tConfList.m_cConfId = ptConfInfo->GetConfId();
				cServMsg.CatMsgBody( (u8*)&tConfList, sizeof( tConfList) );	

                ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq] conf list. Index.%d, confname.%s\n", 
                        nConfNum, tConfList.GetConfName());
			}
		}
	}

    // ���õĻ�����=0��ֱ���ٿ�ȱʡ����
    if (0 == nConfNum)
    {
        bStartDefConf = TRUE;
    }
    // ���õĻ�����=1��ֱ�Ӽ���
	else if (1 == nConfNum)
	{
		//////////////////////////////////////////////////////////////////////////				
		//zjj20100113�Ѿ����������������Ļ��鲻���ٱ�����
		if( !bLowLevelMcuCalledIn && TYPE_MCU == byType 
			&& ptConfInfo
			&& MCS_CONF == ptConfInfo->GetConfSource() 
			&& pInstance
			&& !pInstance->m_tCascadeMMCU.IsNull() 
			)
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq] conf has Cascade by a high adminLevel mcu, so can't Cascade by other mcu!!!\n");
			cServMsg.SetErrorCode( ERR_MCU_CASADEBYOTHERHIGHLEVELMCU );
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			return;
		}		
		//////////////////////////////////////////////////////////////////////////	
		if(ptConfInfo)
			DaemonProcMtMcuApplyJoinReq(bLowLevelMcuCalledIn, ptConfInfo->GetConfId(), pcMsg );
//         g_cMcuVcApp.SendMsgToConf( tConfList.m_cConfId, pcMsg->event, pcMsg->content, pcMsg->length );
        return;
    }
    else
    {
        // �����ն��б�
	    cServMsg.SetErrorCode( ERR_MCU_NULLCID );
	    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq] send conf list to mt. nConfNum.%d\n", nConfNum);
        return;
    }

    // ��ʼȱʡ����
	// �������鶼��ͨ���ڴ���ȡ�ģ�ȱʡ����ֻ���ļ��ж�ȡ [pengguofeng 7/9/2013]
    if (bStartDefConf)
    {
		if( !g_cMcuVcApp.IsHoldDefaultConf() )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq] Can't start def conf,because g_cMcuVcApp.IsHoldDefaultConf is false!\n");
			cServMsg.SetErrorCode(ERR_MCU_NULLCID);
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);           
			return;
		}
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq] start def conf!\n");
		TConfStore tConfStore;
		TConfStore tConfStoreBuf;
		TPackConfStore *ptPackConfStore = (TPackConfStore *)&tConfStoreBuf;
		//��ȡ��� tConfStore Ϊ TPackConfStore �ṹ�����ѽ���Pack����Ļ�������

		//����ȱʡ���鴴��
		if (!GetConfFromFile(MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE, ptPackConfStore))
		{
			cServMsg.SetErrorCode(ERR_MCU_NULLCID);
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

            ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "Conference %s failure because Get Conf.%d From File failed!\n", 
                            tConfStore.m_tConfInfo.GetConfName(), MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE);
			return;
		}
		//�� �ѽ���Pack����Ļ������� ����UnPack����
		u16 wPackConfDataLen = 0;
		if (!UnPackConfStore(ptPackConfStore, tConfStore,wPackConfDataLen))
		{
			cServMsg.SetErrorCode(ERR_MCU_NULLCID);
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

            ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "Conference %s failure because UnPackConf.%d From File failed!\n", 
                            tConfStore.m_tConfInfo.GetConfName(), MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE);
            return;
		}

		if (0 == byEncrypt && 
			CONF_ENCRYPTMODE_NONE != tConfStore.m_tConfInfo.GetConfAttrb().GetEncryptMode())
		{
			cServMsg.SetErrorCode(ERR_MCU_SETENCRYPT);
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

			ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "Conference %s failure because encrypt setting is confused - byEncrypt.%d EncryptMode.%d!\n", 
					        tConfStore.m_tConfInfo.GetConfName(), byEncrypt,
                            tConfStore.m_tConfInfo.GetConfAttrb().GetEncryptMode());
			return;
		}

		TMtAlias atMtAlias[MAXNUM_CONF_MT+1];
		u16      awMtDialBitRate[MAXNUM_CONF_MT+1];
		u8       byMtNumInUse = 0;

		atMtAlias[0] = tMtAddr;
		awMtDialBitRate[0] = wCallRate;
		byMtNumInUse = 1;

        // ˢ���ն��б�����ǽ�ͻ���ϳ�ģ��
        TMultiTvWallModule tNewTvwallModule = tConfStore.m_tMultiTvWallModule;
        TVmpModule         tNewVmpModule    = tConfStore.m_atVmpModule;
        BOOL32 bRepeatThisLoop;
		// xliang [12/26/2008] (modify for MT calling MCU initially) 
		//cancel limit here, but need to input password later
        BOOL32 bInMtTable = FALSE; 

		TConfInfoEx tConfInfoEx;
		THduVmpModuleOrTHDTvWall tHduVmpModule;
		TVmpModuleInfo tVmpModuleEx25;
		u16 wPackConfExInfoLength = 0;
		BOOL32 bUnkownConfInfo = FALSE;
		u8 byEncoding = emenCoding_GBK;
		UnPackConfInfoEx(tConfInfoEx,tConfStore.m_byConInfoExBuf, wPackConfExInfoLength,
						bUnkownConfInfo, &tHduVmpModule, &tVmpModuleEx25, &byEncoding);
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq]byEncoding:%d\n", byEncoding);

		for (u8 byLoop = 0; byLoop < tConfStore.m_byMtNum; byLoop++)
		{
            bRepeatThisLoop = FALSE;
			if (mtAliasTypeH320ID == tConfStore.m_atMtAlias[byLoop].m_AliasType)
			{
				//����뽨���ն�һ�£����ظ����
				if (0 == memcmp(tConfStore.m_atMtAlias[byLoop].m_achAlias, tMtH323Alias.m_achAlias, MAXLEN_ALIAS))
				{
					bRepeatThisLoop = TRUE;
					bInMtTable = TRUE;
					atMtAlias[0] = tMtH323Alias;
				}				
			}
			else if (mtAliasTypeTransportAddress == tConfStore.m_atMtAlias[byLoop].m_AliasType)
			{
				//����뽨���ն�һ�£����ظ����
				if (tConfStore.m_atMtAlias[byLoop].m_tTransportAddr.m_dwIpAddr == tMtAddr.m_tTransportAddr.m_dwIpAddr
                    // zbq [09/19/2007] �²��Բ��ٱȽ϶˿�
					// tConfStore.m_atMtAlias[byLoop].m_tTransportAddr.m_wPort == tMtAddr.m_tTransportAddr.m_wPort
                    )
				{
					bRepeatThisLoop = TRUE;
					bInMtTable = TRUE;
					if ( tMtAddr.m_tTransportAddr.GetPort() != tConfStore.m_atMtAlias[byLoop].m_tTransportAddr.GetPort() )
					{
						tMtAddr.m_tTransportAddr.SetPort(tConfStore.m_atMtAlias[byLoop].m_tTransportAddr.GetPort());
					}
                    atMtAlias[0] = tMtAddr;
				}		
			}
			else if (mtAliasTypeE164 == tConfStore.m_atMtAlias[byLoop].m_AliasType)
			{
				//����뽨���ն�һ�£����ظ����
				if (0 == memcmp(tConfStore.m_atMtAlias[byLoop].m_achAlias, tMtE164Alias.m_achAlias, MAXLEN_ALIAS))
				{
					bRepeatThisLoop = TRUE;
					bInMtTable = TRUE;
					atMtAlias[0] = tMtE164Alias;
				}		
			} 
			else if (mtAliasTypeH323ID == tConfStore.m_atMtAlias[byLoop].m_AliasType)
			{
				//����뽨���ն�һ�£����ظ����
				if (0 == memcmp(tConfStore.m_atMtAlias[byLoop].m_achAlias, tMtH323Alias.m_achAlias, MAXLEN_ALIAS))
				{
					bRepeatThisLoop = TRUE;
					bInMtTable = TRUE;
					atMtAlias[0] = tMtH323Alias;
				}
			}
			else if (puAliasTypeIPPlusAlias == tConfStore.m_atMtAlias[byLoop].m_AliasType)
			{
				u32 dwNetIpAddr = *(u32*)tConfStore.m_atMtAlias[byLoop].m_achAlias; //������
				TMtAlias tTmpAlias;
				memcpy(tTmpAlias.m_achAlias,&tConfStore.m_atMtAlias[byLoop].m_achAlias[sizeof(dwNetIpAddr)],strlen(tConfStore.m_atMtAlias[byLoop].m_achAlias)-sizeof(dwNetIpAddr));
				
				if(tMtAddr.m_tTransportAddr.m_dwIpAddr == dwNetIpAddr 
					&& (strcmp(tMtH323Alias.m_achAlias,tTmpAlias.m_achAlias) == 0
					|| strcmp(tMtE164Alias.m_achAlias,tTmpAlias.m_achAlias) == 0)
					)
				{
					bRepeatThisLoop = TRUE;
					bInMtTable = TRUE;
					atMtAlias[0] = tConfStore.m_atMtAlias[byLoop];
				}
			}

            if (!bRepeatThisLoop)
            {
                atMtAlias[byMtNumInUse] = tConfStore.m_atMtAlias[byLoop];
                awMtDialBitRate[byMtNumInUse] = tConfStore.m_awMtDialBitRate[byLoop];
			    byMtNumInUse++;
            }

            //ÿ��ѭ�����һ���ն˱�����Ҫ����ԭ���ڻ���ϳɺ͵���ǽ������±���£�
            //��������������������봴������ͬ�������ڵ��±���1����ͬ�����±��ǵ�ǰ���ն�ĩβ
            UpdateConfStoreModule(tConfStore, 
                                  byLoop+1, bRepeatThisLoop ? 1 : byMtNumInUse,
                                  tNewTvwallModule, tNewVmpModule,tHduVmpModule,tVmpModuleEx25);         
		}

		//�ն������� MAXNUM_CONF_MT����������ĩβһ���ն�
		if (byMtNumInUse > MAXNUM_CONF_MT)
		{
			byMtNumInUse = MAXNUM_CONF_MT;
		}

		const u8 byConfOpenMode = tConfStore.m_tConfInfo.GetConfAttrb().GetOpenMode();
		if ( CONF_OPENMODE_CLOSED == byConfOpenMode )
		{
			if ( !bInMtTable )
			{
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
				
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CALL, 
					"Conference %s failure because def conf mode is CLOSED and the mt is not in conf mt list!\n", 
					tConfStore.m_tConfInfo.GetConfName());
				return;
			}
		}

		if ( byEncoding != emenCoding_Utf8 )
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq]default conference trans encoding from byEncoding.%d to UTF8\n", byEncoding);
			//confinfo 
			s8 achName[MAXLEN_ALIAS];
			memset(achName, 0, sizeof(achName));

			u32 dwLen = gb2312_to_utf8(tConfStore.m_tConfInfo.GetConfName(), achName, MAXLEN_CONFNAME+1);
			if ( dwLen > 0)
			{
				tConfStore.m_tConfInfo.SetConfName(achName);
			}
			else
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq]default conf:trans failed: conf name:%s\n", tConfStore.m_tConfInfo.GetConfName());
			}
			 
			//mt323alias ps:E164�Ų���������
			// �����ն˹�����ʱ���Ѿ���UTF8�ˣ��ⲽ����Э��ջ���� [pengguofeng 7/23/2013]
/*			memset(achName, 0, sizeof(achName));
			dwLen = gb2312_to_utf8(tMtH323Alias.m_achAlias, achName, MAXLEN_ALIAS);
			if ( dwLen > 0)
			{
				memcpy(tMtH323Alias.m_achAlias, achName, dwLen);
				tMtH323Alias.m_achAlias[dwLen] = 0;
			}
			else
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq]default conf:trans failed: tMtH323Alias:%s\n", tMtH323Alias.m_achAlias);
			}

			//TMtAddr
			// ����MT�ĵ�ַ��alias����Ϊ�գ�transport��IP+�˿ڣ�û��Ҫת [pengguofeng 7/23/2013]
			memset(achName, 0, sizeof(achName));
			dwLen = gb2312_to_utf8(tMtAddr.m_achAlias, achName, MAXLEN_ALIAS);
			if ( dwLen > 0)
			{
				memcpy(tMtAddr.m_achAlias, achName, dwLen);
				tMtAddr.m_achAlias[dwLen] = 0;
			}
			else
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq]default conf:trans failed: tMtAddr:%s\n", tMtAddr.m_achAlias);
			}
*/
			for (u8 byMtIdx = 0;byMtIdx < byMtNumInUse;byMtIdx++)
			{
				memset(achName, 0, sizeof(achName));
				dwLen = gb2312_to_utf8(atMtAlias[byMtIdx].m_achAlias, achName, MAXLEN_ALIAS);
				if ( dwLen > 0)
				{
					memcpy(atMtAlias[byMtIdx].m_achAlias, achName, dwLen);
					atMtAlias[byMtIdx].m_achAlias[dwLen] = 0;
				}
				else
				{
					LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq]default conf:trans failed: atMtAlias[idx.%d]:%s\n", byMtIdx, atMtAlias[byMtIdx].m_achAlias);
				}
			}
		}

		//�ն˱��������� 
		s8  achAliasBuf[SERV_MSG_LEN];
		u16 wAliasBufLen = 0;
		PackTMtAliasArray(atMtAlias, awMtDialBitRate, byMtNumInUse, achAliasBuf, wAliasBufLen);
		wAliasBufLen = htons(wAliasBufLen);

		tConfStore.m_tConfInfo.m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );

		cServMsg.SetMsgBody((u8 *)&tConfStore.m_tConfInfo, sizeof(TConfInfo));
		cServMsg.CatMsgBody((u8 *)&wAliasBufLen, sizeof(wAliasBufLen));
		cServMsg.CatMsgBody((u8 *)achAliasBuf, ntohs(wAliasBufLen));
		TConfAttrb tConfAttrb = tConfStore.m_tConfInfo.GetConfAttrb();
		if (tConfAttrb.IsHasTvWallModule())
		{
			cServMsg.CatMsgBody((u8*)&tNewTvwallModule, sizeof(TMultiTvWallModule));
		}
		if (tConfAttrb.IsHasVmpModule())
		{
			cServMsg.CatMsgBody((u8*)&tNewVmpModule, sizeof(TVmpModule));
		}
		
		//���������HDUvmpģ��ͺ�5���vmpģ����[5/30/2013 chendaiwei]
		u16 wConfInfoExLength = 0;
		PackConfInfoEx(tConfInfoEx, &tConfStore.m_byConInfoExBuf[0],
			wConfInfoExLength,&tHduVmpModule,&tVmpModuleEx25);
		if( wConfInfoExLength > CONFINFO_EX_BUFFER_LENGTH )
		{
			ConfPrint(LOG_LVL_WARNING,MID_MCU_CONF,"[DaemonProcMtMcuApplyJoinReq] conf.%s ConfInfEx.%s > CONFINFO_EX_BUFFER_LENGTH, error!\n",tConfStore.m_tConfInfo.GetConfName(),wConfInfoExLength);
			
			return;
		}
		else
		{
			cServMsg.CatMsgBody((u8*)&tConfStore.m_byConInfoExBuf[0],wConfInfoExLength);
		}
		
		cServMsg.CatMsgBody((u8 *)&tMtH323Alias, sizeof(tMtH323Alias));
		cServMsg.CatMsgBody((u8 *)&tMtE164Alias, sizeof(tMtE164Alias));
		cServMsg.CatMsgBody((u8 *)&tMtAddr, sizeof(tMtAddr));
		cServMsg.CatMsgBody((u8 *)&byType, sizeof(byType));
		// xliang [12/26/2008] (modify for MT call MCU initially) add bInMtTalbe Info
		cServMsg.CatMsgBody((u8*)&bInMtTable,sizeof(bInMtTable));

		//print test
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "bInMtTable is %d\n",bInMtTable);

        u8 byInsID = AssignIdleConfInsID();
        if(0 != byInsID)
        {
            ::OspPost(MAKEIID(AID_MCU_VC, byInsID), MT_MCU_CREATECONF_REQ,
                      cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        else
        {
            ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[DaemonProcMtMcuApplyJoinReq] assign instance id failed!\n");
        }
		
	}

	return;
}

/*====================================================================
    ������      ��ProcMtMcuApplyJoinReq
    ����        ���ն����������鴦����
    �㷨ʵ��    ���������ʽ����ֱ��������룬����ܾ���֪ͨ��ϯ����MCU����̨
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/11/06    1.0         Liaoweijiang  ����
	04/07/20    3.0         ������        �޸�
	06/01/06	4.0			�ű���		  T120���ɱ���ն˺��з���
====================================================================*/
void CMcuVcInst::ProcMtMcuApplyJoinReq( const CMessage * pcMsg )
{
	CServMsg    cServMsg( pcMsg->content, pcMsg->length );
	TMtAlias    tMtH323Alias = *( TMtAlias * )cServMsg.GetMsgBody();
	TMtAlias    tMtE164Alias = *( TMtAlias * )( cServMsg.GetMsgBody() + sizeof(TMtAlias) );
	TMtAlias    tMtAddr      = *( TMtAlias * )( cServMsg.GetMsgBody() + sizeof(TMtAlias)*2 );
	TMtAlias    tConfE164    = *( TMtAlias * )( cServMsg.GetMsgBody() + sizeof(TMtAlias)*3 );
	u8          byType       = *(cServMsg.GetMsgBody()+sizeof(TMtAlias)*4);
	u8          byEncrypt    = *(cServMsg.GetMsgBody()+sizeof(TMtAlias)*4+sizeof(u8));
	u16         wCallRate    = *(u16*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)));
	u8          byMtId       = 0;
	u8          byRepeateMtId= 0;

	switch( CurState() )
	{
	case STATE_ONGOING:
    {
		if( byEncrypt == 0 && 
			m_tConf.GetConfAttrb().GetEncryptMode() != CONF_ENCRYPTMODE_NONE )
		{
			cServMsg.SetConfIdx( m_byConfIdx );
			cServMsg.SetConfId( m_tConf.GetConfId() );

			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "Mt 0x%x join conf %s request was refused because encrypt!\n", 
				     tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
            return;
		}
        
        // byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAddr );

		//����ģʽӦ�ð��ջ��飬�������������е��ն�[12/12/2012 chendaiwei]
		byEncrypt = (m_tConf.GetConfAttrb().GetEncryptMode() != CONF_ENCRYPTMODE_NONE) ? 1 : 0;

        //���ն�����ĺ��뷽ʽ
        TMtAlias tFstAlias;
        
        // zbq [08/07/2007] �ն˼�����飬Ҫ�ȶԺ��б���ȷ���µ������ն��Ƿ��������б���
        for ( u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++ )
        {
            TMtAlias tDialAlias;
            if ( m_tConfAllMtInfo.MtInConf(byMtIdx) &&
                 m_ptMtTable->GetDialAlias(byMtIdx, &tDialAlias) )
            {
                TMtAlias tInConfMtAddr;

                if ( mtAliasTypeE164 == tDialAlias.m_AliasType &&
                     tMtE164Alias == tDialAlias )                     
                {
					//��ʹ�к���E164����ƥ�䣬ҲҪУ��һ��Ip����ֹδע��GK(��ע���˲�ͬ�ķ��ھ�GK)���ն�ð������
                    if(TYPE_MT == byType && m_ptMtTable->GetMtAlias(byMtIdx, mtAliasTypeTransportAddress, &tInConfMtAddr) )
					{
						if( tInConfMtAddr.m_tTransportAddr.GetIpAddr() == tMtAddr.m_tTransportAddr.GetIpAddr() )
						{
							byMtId = byMtIdx;
							tFstAlias = tMtE164Alias;
						}
					}
					else
					{
						byMtId = byMtIdx;
						tFstAlias = tMtE164Alias;
					}                                         
                    break;
                }
                else if ( mtAliasTypeH323ID == tDialAlias.m_AliasType &&
                          tMtH323Alias == tDialAlias)
                {
					//��ʹ�к���H323����ƥ�䣬ҲҪУ��һ��Ip����ֹδע��GK(��ע���˲�ͬ�ķ��ھ�GK)���ն�ð������
                    if(TYPE_MT == byType && m_ptMtTable->GetMtAlias(byMtIdx, mtAliasTypeTransportAddress, &tInConfMtAddr) )
					{
						if( tInConfMtAddr.m_tTransportAddr.GetIpAddr() == tMtAddr.m_tTransportAddr.GetIpAddr() )
						{
							byMtId = byMtIdx;
							tFstAlias = tMtH323Alias;
						}
					}
					else
					{
						byMtId = byMtIdx;
						tFstAlias = tMtH323Alias;
					}                                         
                    break;
                }
                else if( mtAliasTypeTransportAddress == tDialAlias.m_AliasType &&
                         tMtAddr.m_tTransportAddr.GetIpAddr() == tDialAlias.m_tTransportAddr.GetIpAddr() )
                {
                    byMtId = byMtIdx;
                    tFstAlias = tMtAddr;
                    break;
                }
				else if (puAliasTypeIPPlusAlias == tDialAlias.m_AliasType)
				{					
					if(tMtAddr.m_tTransportAddr.m_dwIpAddr == tDialAlias.m_tTransportAddr.m_dwIpAddr 
						&& (strcmp(tMtH323Alias.m_achAlias,tDialAlias.m_achAlias) == 0
						|| strcmp(tMtE164Alias.m_achAlias,tDialAlias.m_achAlias) == 0)
						)
					{
						byMtId = byMtIdx;
						
						//���͵�VCS��Alias����ȷ��[12/14/2012 chendaiwei]
						tFstAlias.m_AliasType = puAliasTypeIPPlusAlias;
						memcpy(tFstAlias.m_achAlias,(s8*)&tMtAddr.m_tTransportAddr.m_dwIpAddr,sizeof(tMtAddr.m_tTransportAddr.m_dwIpAddr));
						u32 dwMaxAliasSize = sizeof(tFstAlias.m_achAlias)-sizeof(tMtAddr.m_tTransportAddr.m_dwIpAddr);
						u32 dwActualAliasSize = strlen(tDialAlias.m_achAlias);
						memcpy(&tFstAlias.m_achAlias[sizeof(tMtAddr.m_tTransportAddr.m_dwIpAddr)],tDialAlias.m_achAlias,min(dwMaxAliasSize,dwActualAliasSize));
						
						break;
					}
				}
            }
        }

        BOOL32 bMtInConf = ( byMtId > 0 /*&& m_tConfAllMtInfo.MtInConf( byMtId )*/ );

        BOOL32 bMtCallingInWithSegIp = FALSE;

        //���������ն�
        if ( !bMtInConf )
        {
            //��ջ���, �ܾ�
		    if( m_tConf.GetConfAttrb().GetOpenMode() == CONF_OPENMODE_CLOSED )
		    {			
			    cServMsg.SetConfIdx( m_byConfIdx );
			    cServMsg.SetConfId( m_tConf.GetConfId() );

			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

			    ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "Mt 0x%x join conf %s request was refused because conf is closed!\n", 
					      tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
			    return;
		    }

            //[5/19/2011 zhushengze]�������״̬�ܾ��ն��������
            if ( VCS_CONF == m_tConf.GetConfSource() && TYPE_MT == byType &&
                ( CONF_CREATE_MT == m_byCreateBy ||
                ( ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) && m_cVCSConfStatus.IsGroupModeLock() )
					)
                )
            {
                cServMsg.SetConfIdx( m_byConfIdx );
                cServMsg.SetConfId( m_tConf.GetConfId() );
                
                g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
                
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "Mt 0x%x join conf %s request was refused because GroupMode conf lock!\n", 
                    tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
			    return;
            }

            // xsl [8/2/2006] ����mcu��������
            if ( byType != TYPE_MCU )
            {
                //������û����������õ�ַ���ն˼���, �ܾ�
				// xliang [12/25/2008] ȡ���û�������
				//zjj20130821 ����ipʱ��ҪУ�飬���л���e164��ʱ����ҪУ��
                /*if ( !g_cMcuVcApp.IsMtIpInAllowSeg( m_tConf.GetUsrGrpId(), tMtAddr.m_tTransportAddr.GetNetSeqIpAddr()) )
                {
                    cServMsg.SetConfIdx( m_byConfIdx );
                    cServMsg.SetConfId( m_tConf.GetConfId() );
                    
                    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
                    
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt 0x%x join conf %s request was refused because usr group %d not allowed!\n", 
                              tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName(), m_tConf.GetUsrGrpId() );
                    return;
                }
                else*/
                {
                    //zbq[08/09/2007] FIXME:���ں�����ն˹Ҷϼ����Ƴ������ﲻ��
                    //Ҫ����MCU�������º��е����,�ʿ��Բ������Ƿ�ע��GK��������
                    //�����������͵ı����������������Ȼδ���ǵ�������δע��GK��
                    //�ն˵�E164��ͬ�����������С�������������E164�ĸ߶�Ψһ�ԡ�

                    //��Ϊ�����ַ�ε��նˣ���¼�����������E164��H323ID�����Ip��
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

                    bMtCallingInWithSegIp = TRUE;
                }
            }
            else
            {
                //����ʼ�ո����ϼ�����Ľ�����ַ����ID
                tFstAlias = tMtAddr;
            }
        }

		if( VCS_CONF == m_tConf.GetConfSource() && 
					CONF_CREATE_MT != m_byCreateBy && byType == TYPE_MT //&&
					//VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
					//!ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() ) 
					 )
		{
            u8 byMtInConf = bMtInConf ? 1 : 0;
            if (!ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode()))
            {
                if( m_cVCSConfStatus.GetReqVCMT().IsNull() )
                {
                    if( !m_cVCSConfStatus.GetCurVCMT().IsNull() )
                    {
                        u8 bySrcDriId = cServMsg.GetSrcDriId();	
                        u8 bySrcSsnId = cServMsg.GetSrcSsnId();
                        cServMsg.SetEventId( MCU_VCS_MTJOINCONF_REQ );
                        cServMsg.SetMsgBody(&byEncrypt, sizeof(u8));						
                        cServMsg.CatMsgBody( (u8*)&bySrcDriId,sizeof(u8) );	
                        cServMsg.CatMsgBody( (u8*)&tFstAlias,sizeof(TMtAlias) );
                        cServMsg.CatMsgBody( (u8*)&tMtH323Alias,sizeof(TMtAlias) );
                        cServMsg.CatMsgBody( (u8*)&tMtE164Alias,sizeof(TMtAlias) );
                        cServMsg.CatMsgBody( (u8*)&tMtAddr,sizeof(TMtAlias) );
                        wCallRate = ntohs(wCallRate);
                        cServMsg.CatMsgBody( (u8*)&wCallRate,sizeof(u16) );
                        cServMsg.CatMsgBody( (u8*)&bySrcSsnId,sizeof(u8) );
                        cServMsg.CatMsgBody( (u8*)&byMtInConf,sizeof(byMtInConf) );
                        SendMsgToAllMcs( MCU_VCS_MTJOINCONF_REQ,cServMsg );
                        return;
                    }
                }
                else
                {
                    cServMsg.SetConfIdx( m_byConfIdx );
                    cServMsg.SetConfId( m_tConf.GetConfId() );
                    cServMsg.SetErrorCode( ERR_MCU_CONFNUM_EXCEED );
                    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
                    
                    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMtMcuApplyJoinReq] Mt join conf request was refused because vcs conf has reqMt(%d)!\n", 
                        m_cVCSConfStatus.GetReqVCMT().GetMtId() );
                    return;
			    }	
            }
            else
            {
                if( !bMtInConf )
                {
                    u8 bySrcDriId = cServMsg.GetSrcDriId();	
                    u8 bySrcSsnId = cServMsg.GetSrcSsnId();
                    cServMsg.SetEventId( MCU_VCS_MTJOINCONF_REQ );
                    cServMsg.SetMsgBody(&byEncrypt, sizeof(u8));						
                    cServMsg.CatMsgBody( (u8*)&bySrcDriId,sizeof(u8) );	
                    cServMsg.CatMsgBody( (u8*)&tFstAlias,sizeof(TMtAlias) );
                    cServMsg.CatMsgBody( (u8*)&tMtH323Alias,sizeof(TMtAlias) );
                    cServMsg.CatMsgBody( (u8*)&tMtE164Alias,sizeof(TMtAlias) );
                    cServMsg.CatMsgBody( (u8*)&tMtAddr,sizeof(TMtAlias) );
                    wCallRate = ntohs(wCallRate);
                    cServMsg.CatMsgBody( (u8*)&wCallRate,sizeof(u16) );
                    cServMsg.CatMsgBody( (u8*)&bySrcSsnId,sizeof(u8) );
                    cServMsg.CatMsgBody( (u8*)&byMtInConf,sizeof(byMtInConf) );
                    SendMsgToAllMcs( MCU_VCS_MTJOINCONF_REQ,cServMsg );
                    return;
                }
                
			}
					
		}
   
		wCallRate -= GetAudioBitrate( m_tConf.GetMainAudioMediaType() );               
        
		//�����ն�ID��
		byMtId = AddMt( tFstAlias, wCallRate, CONF_CALLMODE_NONE, TRUE );

		//�ն˺�����ᣬ��ز��������б����Ϊ��������������Ϊ��������նˣ���ͨ���򿪺����ϯ����ѡ��
		if ( VCS_CONF == m_tConf.GetConfSource() && !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
			CONF_CREATE_MT != m_byCreateBy && byType == TYPE_MT
			)
		{
			TMt tMt = m_ptMtTable->GetMt(byMtId);
			m_cVCSConfStatus.SetReqVCMT( tMt );
			SetTimer( MCUVC_VCMTOVERTIMER_TIMER, (g_cMcuVcApp.GetVcsMtOverTimeInterval() + 5)*1000  );			
		}
		

        // zbq [08/09/2007] ��������Ip�κ���,�����б����ֲ���Ip���ն�,�˴��豣����Ip
        if ( bMtCallingInWithSegIp && !(tFstAlias == tMtAddr) )
        {
            m_ptMtTable->SetMtAlias( byMtId, &tMtAddr );
		}

		//[1/10/2013 chendaiwei]IPΪ0������IP
		if(0 == m_ptMtTable->GetIPAddr( byMtId))
		{
			m_ptMtTable->SetIPAddr( byMtId, tMtAddr.m_tTransportAddr.GetIpAddr() );
		}

        // xsl [11/8/2006] ������ն˼�����1
		// xliang [2/14/2009] ������MT����MCU,������
		u8 byDriId = cServMsg.GetSrcDriId();
// 		u16 wExtraNum = (byType == TYPE_MCU)? 1: 0;
//         g_cMcuVcApp.IncMtAdpMtNum( cServMsg.GetSrcDriId(), m_byConfIdx, byMtId,wExtraNum);
        g_cMcuVcApp.IncMtAdpMtNum( cServMsg.GetSrcDriId(), m_byConfIdx, byMtId,byType);
        m_ptMtTable->SetDriId(byMtId, byDriId);
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
            if ( !bMtInConf )
            {
                m_ptMtTable->SetNotInvited( byMtId, TRUE );
            }
            //m_ptMtTable->SetAddMtMode(byMtId, ADDMTMODE_MTSELF);
			
			if(byType == TYPE_MCU)
			{
				if( m_tConf.GetConfAttrb().IsSupportCascade() )
				{
					m_ptMtTable->SetMtType(byMtId, MT_TYPE_MMCU);
				}
				else
				{
					SetInOtherConf( TRUE,byMtId );
					m_ptMtTable->SetMtType( byMtId, MT_TYPE_MT );
				}
				
// 				if (g_cMcuVcApp.IsMMcuSpeaker())
// 				{
// 					m_bMMcuSpeakerValid = TRUE;
// 				}
			}
			else if(byType == TYPE_MT)
			{
				m_ptMtTable->SetMtType( byMtId, MT_TYPE_MT );
			}
			
			m_ptMtTable->SetMtAlias(byMtId, &tMtH323Alias);
			m_ptMtTable->SetMtAlias(byMtId, &tMtE164Alias);
			if( /*!bMtInConf*/
				!m_tConfAllMtInfo.MtJoinedConf( byMtId )&& 
				VCS_CONF == m_tConf.GetConfSource() && 
				TYPE_MT == byType &&
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
			//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
			//u8 byAudioTrackNum = tAudioType.GetAudioTrackNum();
			//cServMsg.CatMsgBody(&byAudioTrackNum,sizeof(byAudioTrackNum));
			TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//��Ƶ����
			//�ӻ���������ȡ������֧�ֵ���Ƶ����
			m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
			cServMsg.CatMsgBody((u8*)&atAudioTypeDesc[0], sizeof(TAudioTypeDesc)* MAXNUM_CONF_AUDIOTYPE);
			
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+1, cServMsg);			
			
			g_cMcuVcApp.UpdateAgentAuthMtNum();
		}
		else
		{
			cServMsg.SetConfIdx( m_byConfIdx );
			cServMsg.SetConfId( m_tConf.GetConfId() );
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

			ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "Mt 0x%x join conf %s request was refused because conf full!\n", 
					  tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
		}

		break;
    }
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*====================================================================
    ������      ��ProcMtMcuSendMcMsgReq
    ����        ���ն˷�����MCU����̨�Ķ���Ϣ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾 1.0         Liaoweijiang  ����
====================================================================*/
void CMcuVcInst::ProcMtMcuSendMcMsgReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	
	switch( CurState() )
	{
	case STATE_ONGOING:
		//������MCU���еĿ���̨
		SendMsgToAllMcs( MCU_MCS_SENDRUNMSG_NOTIF, cServMsg );
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMtMcuStartSwitchMtReq
    ����        ���ն˽���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	02/11/26	1.0			LI Yi         ����
	03/02/26    1.0         Qzj           ��ǿԴ�ն���������ж�
	04/02/21    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::ProcMtMcuStartSwitchMtReq( const CMessage * pcMsg )
{    
    STATECHECK;
    
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	
	ProcStartSelSwitchMt(cServMsg);
	
    return;
}

/*====================================================================
������      ��ProcStartSelSwitchMt
����        ��ѡ���ն˴�����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵���� CServMsg &cServMsg �������Ϣ
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2010/09/15	4.6			Ѧ��		  ��������VMPǰ�����޸�			   	
====================================================================*/
void CMcuVcInst::ProcStartSelSwitchMt(CServMsg &cServMsg)
{
	TSwitchInfo	tSwitchInfo;
	TMt	tDstMt, tSrcMt;
	u8  byDstMtId;
	CServMsg  cMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	TMtStatus tSrcMtStatus;
    TMtStatus tDstMtStatus;    

    tSwitchInfo = *( TSwitchInfo * )cServMsg.GetMsgBody();	
    tSrcMt = tSwitchInfo.GetSrcMt();
    tDstMt = tSwitchInfo.GetDstMt();
	
	TMt tMcAudCurSrc;
	TMt tMcVidCurSrc;
	g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcAudCurSrc, tSwitchInfo.GetDstChlIdx(), MODE_AUDIO );
	g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcVidCurSrc, tSwitchInfo.GetDstChlIdx(), MODE_VIDEO );

	// ���ǻ������ƣ��Ƶ�ǰ�����жϣ�����Ķ�ɾ�� [pengguofeng 1/16/2013]
    m_ptMtTable->GetMtStatus(tDstMt.GetMtId(), &tDstMtStatus);
    TMt tVidSrc/* = tDstMtStatus.GetSelectMt(MODE_VIDEO)*/;
	u8 bySkipVcChnnl = 0xff;
	emSatReplaceType emSatType = emDefault;
	if ( cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMC_REQ )
	{
		tVidSrc.SetNull();
		tVidSrc.SetEqpId(cServMsg.GetSrcSsnId());
		bySkipVcChnnl = tSwitchInfo.GetDstChlIdx();
		emSatType = emStartOrChangeMonitor;
	}
	else
	{
		tVidSrc = tDstMt;
		emSatType = emSelect;
	}
	if ( /*!tVidSrc.IsNull()*/
		( tSwitchInfo.GetMode() == MODE_VIDEO || tSwitchInfo.GetMode() == MODE_BOTH ) //ֻ�е�ѡ������Ƶʱ���ж�
		//&& !(tVidSrc == tSrcMt)
		&& IsMultiCastMt(tSrcMt.GetMtId())
		&& m_tConf.GetConfAttrb().IsSatDCastMode())
	{
		//if ( IsSatMtOverConfDCastNum(tSrcMt, emSatType, bySkipVcId, bySkipVcChnnl, 0xff, 0xff, 0xff, bySkipMtId))
		if(!IsSatMtCanContinue(GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId(),emSatType,&tVidSrc,bySkipVcChnnl))
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[ProcStartSelSwitchMt]Select Src:%d over Max Sat BandSwitch, cancel\n", tSrcMt.GetMtId());
			cServMsg.SetErrorCode(ERR_MCU_DCAST_OVERCHNNLNUM);
			SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
			return;
		}
	}

	//˫ѡ���ݲ�֧�ֻش�
	if (MODE_VIDEO2SECOND == tSwitchInfo.GetMode() && !tSrcMt.IsLocal())
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[ProcStartSelSwitchMt] tSrcMt(%d,%d) is not local!\n", 
			tSrcMt.GetMcuId(), tSrcMt.GetMtId());
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return;
	}

	//zhouyiliang 20120831 ��ϯ��ѯģʽ�� Ŀ�Ķ�Ϊ��ϯ�ն�ʱѡ����ͣ��ѯ
	// miaoqingsong [2011/08/09] ��ϯ��ѯģʽ��
	if ( ( (CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode() && tSwitchInfo.GetMode() != MODE_AUDIO ) ||
		    (CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode())
		  )  
		  && tDstMt == m_tConf.GetChairman() && !(tSrcMt == m_tConf.m_tStatus.GetMtPollParam().GetTMt()) 
		)
	{
// 		ConfPrint( LOG_LVL_ERROR, MID_MCU_MT, "[ProcStartSelSwitchMt] Can't start chairman's select because Chairman Polling!\n" );
// 		cServMsg.SetErrorCode( ERR_MCU_SPECCHAIREMAN_NACK );
// 		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	//nack
// 		return;
		ProcStopConfPoll();
	}
	
	//20120330 yhz ������,���ѡ��vmp
	if (cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMC_REQ &&
		tSrcMt.GetType() == TYPE_MCUPERI && EQP_TYPE_VMP == tSrcMt.GetEqpType())
	{
		//���鲻��vmp��
		if (!IsVmpIdInVmpList(tSrcMt.GetEqpId()) ||g_cMcuVcApp.GetVMPMode(tSrcMt) == CONF_VMPMODE_NONE)	
		{
			cServMsg.SetErrorCode(ERR_MCU_VMPNOTSTART);
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ProcStartSelSwitchMt] The Conf is not in the VMP mode!\n");
			SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
			return ;
		}
		
		//��VMP��֧��ѡ��
		if(GetVmpSubType(tSrcMt.GetEqpId()) == VMP)
		{
			cServMsg.SetErrorCode(ERR_FUNCTIONNOTSUPPORTBYOLDVMP);
			SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
			return ;
		}

		/*�����eqp���ǵ�ǰ����ϳ�������
		if (!(tSrcMt == m_tVmpEqp))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ProcStartSelSwitchMt] The tSrcMt[%d] is not m_tVmpEqp[%d]!\n", tSrcMt.GetEqpId(), m_tVmpEqp.GetEqpId());
			SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
			return ;
		}*/
	}

	//�տ�̨��������Ϣtmt��Ϣ��Ҫת��
	if (MT_MCU_STARTSELMT_CMD == cServMsg.GetEventId() ||
		MT_MCU_STARTSELMT_REQ == cServMsg.GetEventId())
	{
		//[nizhijun 2010/11/12] �տ�̨��ѯ������ƵԴ����
		BOOL32 bVideoLose = FALSE;
		if (tSrcMt.IsLocal())
        {
            TMtStatus tMtStatus;
            m_ptMtTable->GetMtStatus(tSrcMt.GetMtId(), &tMtStatus);
            bVideoLose = tMtStatus.IsVideoLose();
        }
        else
        {
			TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tSrcMt.GetMcuId());
            if (NULL != ptMcInfo)
            {
				TMcMtStatus *pMcMtStatus = ptMcInfo->GetMtStatus((TMt &)tSrcMt);
                bVideoLose = pMcMtStatus->IsVideoLose();
            }
        }
		if (bVideoLose)
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "Mt(%d,%d) Select see Mt(%d,%d), returned with Nack,because of VideoLose!\n", 
                tDstMt.GetMcuId(), 
                tDstMt.GetMtId(),
                tSrcMt.GetMcuId(), 
                tSrcMt.GetMtId());

			// ��NACK
			SendReplyBack( cServMsg, MCU_MT_STARTSELMT_NACK );
			return;
		}

	}

	if ( MT_MCU_STARTSELMT_CMD == cServMsg.GetEventId() ||
		 MT_MCU_STARTSELMT_REQ == cServMsg.GetEventId() || 
		 MCS_MCU_STARTSWITCHMT_REQ == cServMsg.GetEventId())
	{
		if(m_ptMtTable->GetMtStatus(tDstMt.GetMtId(), &tDstMtStatus))
		{
			//ָ��Ŀ���ն���ѡ����Ƶ
			if (!tDstMtStatus.GetSelectMt(MODE_VIDEO).IsNull() &&
				tDstMtStatus.GetSelectMt(MODE_VIDEO) == tSrcMt)
			{
				//��ѡ��ģʽΪvideo,��ѡ��ģʽΪvideo��both��ֱ��return
				if (tDstMtStatus.HasModeInSelMode(MODE_VIDEO) && MODE_VIDEO == tSwitchInfo.GetMode())
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt] OldSel[Mode.%d] is equal or larger than NewSel[Mode.%d]!\n",
						tDstMtStatus.GetSelByMcsDragMode(), tSwitchInfo.GetMode());

					SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
					return;
				}
				//��ѡ��ģʽΪboth,��ѡ��ģʽΪvideo��both��������Ƶ���ĳ���ѡ����Ƶ
				else if (tDstMtStatus.HasModeInSelMode(MODE_VIDEO) && MODE_BOTH == tSwitchInfo.GetMode())
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt]  NewSel[Mode.%d] change to NewSel[Mode.%d]]!\n",
						tSwitchInfo.GetMode(), MODE_AUDIO);

					tSwitchInfo.SetMode(MODE_AUDIO);
				}
			}
			
			if (!tDstMtStatus.GetSelectMt(MODE_AUDIO).IsNull() &&
				tDstMtStatus.GetSelectMt(MODE_AUDIO) == tSrcMt)
			{
				//��ѡ��ģʽΪaudio,��ѡ��ģʽΪaudio��both��ֱ��return
				if (tDstMtStatus.HasModeInSelMode(MODE_AUDIO) && MODE_AUDIO == tSwitchInfo.GetMode())
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt] OldSel[Mode.%d] is equal or larger than NewSel[Mode.%d]!\n",
						tDstMtStatus.GetSelByMcsDragMode(), tSwitchInfo.GetMode());

					SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
					return;
				}
				//��ѡ��ģʽΪboth,��ѡ��ģʽΪaudio��both��������Ƶ���ĳ���ѡ����Ƶ
				else if (tDstMtStatus.HasModeInSelMode(MODE_AUDIO) && MODE_BOTH == tSwitchInfo.GetMode())
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt]  NewSel[Mode.%d] change to NewSel[Mode.%d]]!\n",
						tSwitchInfo.GetMode(), MODE_VIDEO);
					
					tSwitchInfo.SetMode(MODE_VIDEO);
				}
			}
		}

		if (MODE_VIDEO2SECOND == tSwitchInfo.GetMode() 
			&& !(m_ptMtTable->GetMtSelMtByMode(tDstMt.GetMtId(), MODE_VIDEO2SECOND).IsNull())
			&& m_ptMtTable->GetMtSelMtByMode(tDstMt.GetMtId(), MODE_VIDEO2SECOND) == tSrcMt)
		{
			tSwitchInfo.SetMode(MODE_NONE);
		}
	}


	//��ֹSendReplyBack����Ϣ����Ӧ�𣬽�Src��ΪNULL
	if( cServMsg.GetEventId() == MT_MCU_STARTSELMT_CMD )
	{
		cServMsg.SetNoSrc();
        cServMsg.SetSrcMtId(0);
	}


	//������ǻ��ѡ���նˣ�ǿ�ƹ㲥�µķ���ϯѡ����NACK
	if( cServMsg.GetEventId() != MCS_MCU_STARTSWITCHMC_REQ && 
		m_tConf.m_tStatus.IsMustSeeSpeaker() && 
        !(tDstMt == m_tConf.GetChairman()) )
	{
		cServMsg.SetErrorCode( ERR_MCU_MUSTSEESPEAKER );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

    // ���� [5/30/2006]
    // ������¼�����ѡ���ϼ�����ֱ�ӷ���, ��NACK
    if (cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMT_REQ &&
        (tDstMt.IsNull() || !tDstMt.IsLocal()) )
    {
        ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Mt(%d,%d) Select see Mt(%d,%d), returned with ack!\n", 
                tDstMt.GetMcuId(), 
                tDstMt.GetMtId(),
                tSrcMt.GetMcuId(), 
                tSrcMt.GetMtId());

        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
        return;
    }

	if( tDstMt.GetType() == TYPE_MT)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "Mt(%d,%d) select see Mt(%d,%d)\n", tDstMt.GetMcuId(), tDstMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId() );

		tDstMt = GetLocalMtFromOtherMcuMt( tDstMt );				
		tDstMt = m_ptMtTable->GetMt(tDstMt.GetMtId());
		tSwitchInfo.SetDstMt( tDstMt );		
	}

    TMt tLocalMt;
	u8 bySwitchMode = tSwitchInfo.GetMode();	// ��¼��ʼMODE
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "Select tSrcMt(%d, %d)!\n", tSrcMt.GetMcuId(), tSrcMt.GetMtId());
	// ԴΪ����ʱ,����ѡ��vmp
	if (tSrcMt.GetType() == TYPE_MCUPERI)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcStartSelSwitchMt] The Src is EQP, EqpType:%d, EqpID:%d!\n",
			tSrcMt.GetEqpType(), tSrcMt.GetEqpId());
		if (tSrcMt.GetEqpType() == EQP_TYPE_VMP)
		{
			// ����ϳ�ֻ����ƵԴ
			bySwitchMode = MODE_VIDEO;
		}
	}
	else
	{
		tLocalMt = GetLocalMtFromOtherMcuMt(tSrcMt);
		byDstMtId = tDstMt.GetMtId();
		//Դ�ն�δ����飬NACK
		if( !m_tConfAllMtInfo.MtJoinedConf( tSrcMt ) &&
			m_tConf.GetConfSource() != VCS_CONF)
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Select source Mt(%u,%u) has not joined current conference!\n",
				tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
			cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}    
		
		//�ն�״̬��ѡ��ģʽ��ƥ��, NACK
		// zgc, 2008-07-10, �޸��жϷ�ʽ, �����չ��˽��ΪMODE_NONEʱ���ܾ�ѡ�������������ʾ
		GetMtStatus(tLocalMt, tSrcMtStatus);
		m_ptMtTable->GetMtStatus(byDstMtId, &tDstMtStatus);
		if( (!tSrcMtStatus.IsSendVideo() ||
			(tDstMt.GetType() == TYPE_MT && !tDstMtStatus.IsReceiveVideo()) ) &&
			(bySwitchMode == MODE_VIDEO || bySwitchMode == MODE_BOTH || bySwitchMode == MODE_VIDEO2SECOND) )
		{
			bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_AUDIO : MODE_NONE;
		}	
		
		if( (!tSrcMtStatus.IsSendAudio() ||
			(tDstMt.GetType() == TYPE_MT && !tDstMtStatus.IsReceiveAudio()) ) &&
			( bySwitchMode == MODE_AUDIO || bySwitchMode == MODE_BOTH ) )
		{
			bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_VIDEO : MODE_NONE;
		}
	}

	// ��MCS��֪ͨ
	u8 byMcsId = 0;
	if ( cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMT_REQ || 
		cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMC_REQ )
	{
		byMcsId = cServMsg.GetSrcSsnId();
	}

	//20110818 zjl ����Ƶ���ջ���ͨ����û�򿪣�ֱ��nack
    if ( MODE_NONE == bySwitchMode )
    {
        cServMsg.SetErrorCode( ERR_MCU_SRCISRECVONLY );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
	}
	//��Ƶ����Ƶ(���ջ���)��һ��û�򿪣���ͨ��mcs
	else if(bySwitchMode !=  tSwitchInfo.GetMode())
	{
		NotifyMcsAlarmInfo(byMcsId, ERR_MCU_SRCISRECVONLY);
	}


    //zbq[05/22/2009] �ն�ѡ������֧�� FIXME: ��δ�����������Ķ�̬��ռ
	BOOL32 bVidAdp = FALSE;
	BOOL32 bAudAdp = FALSE;
	BOOL32 bSecVidAdp = FALSE;
    //�ն�ѡ��
    if( TYPE_MT == tDstMt.GetType() )
    {
		//����Ǳ����ն˻򵥻ش����ж��ϼ��Ƿ���Ҫ���䣬��ش�������ϼ�ֱ��ָ���ն˿ɽ��ֱܷ������¼�����
		if(tSrcMt.IsLocal() || (!tSrcMt.IsLocal() && !IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId())))
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
						cServMsg.SetErrorCode( ERR_MCU_NOTSAMEMEDIATYPE );
						SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
						if (MODE_VIDEO == bySwitchMode)
						{
							return;
						}
						else if (MODE_BOTH == bySwitchMode)
						{
							bySwitchMode = MODE_AUDIO;
						}
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
						cServMsg.SetErrorCode( ERR_MCU_NOTSAMEMEDIATYPE );
						SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
						if (MODE_AUDIO == bySwitchMode)
						{
							return;
						}
						else if (MODE_BOTH == bySwitchMode)
						{
							bySwitchMode = MODE_VIDEO;
						}
					}
				}
			}

			TLogicalChannel tLogicalChannel;
			if (MODE_VIDEO2SECOND == bySwitchMode
				&& m_ptMtTable->GetMtLogicChnnl(tDstMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, TRUE))
			{
				if (IsNeedSelAdpt(tSrcMt, tDstMt, MODE_VIDEO2SECOND))
				{
					if (g_cMcuVcApp.IsSelAccord2Adp())
					{
						bSecVidAdp = TRUE;
					}
					else
					{
						cServMsg.SetErrorCode(ERR_MCU_NOTSAMEMEDIATYPE);
						SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
						return;
					}					
				}
			}
		}	
	}
	
	// ��MCS��֪ͨ
// 	u8 byMcsId = 0;
// 	if ( cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMT_REQ || 
// 		 cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMC_REQ )
// 	{
// 		byMcsId = cServMsg.GetSrcSsnId();
// 	}

//  if ( MODE_VIDEO == bySwitchMode && MODE_BOTH == tSwitchInfo.GetMode() )
// 	{
// 		NotifyMcsAlarmInfo( byMcsId, ERR_MCU_SELMODECHANGE_AUDIOFAIL );
// 	}
// 
//     if ( MODE_AUDIO == bySwitchMode && MODE_BOTH == tSwitchInfo.GetMode() )
// 	{
// 		NotifyMcsAlarmInfo( byMcsId, ERR_MCU_SELMODECHANGE_VIDEOFAIL );
// 	}

	// ���ڲ�ѡ��������Ҫ����mcsdragmod
	BOOL32 bInnerSelect = FALSE;

	//[2011/10/14/zhangli]��Ϊ��ͣĿ���ն˾ɵ�ѡ������ͼ��������StopSelectSrc���һ������ΪFALSE��ֻ���ѡ��״̬����ͣ����
	//�����¼Ŀ���ն˾ɵ�ѡ��Դ�����ѡ��ʧ�ܣ�����ͣ���ɵ�ѡ��
	TMt tOldSelVid;
	TMt tOldSelAdu;
	TMt tOldSelSecVid;

    tSwitchInfo.SetMode( bySwitchMode );

    switch( cServMsg.GetEventId() )
    {
	case MCS_MCU_STARTSWITCHMT_REQ:
	case MT_MCU_STARTSELMT_CMD:
	case MT_MCU_STARTSELMT_REQ:
        {
			if ( cServMsg.GetSrcSsnId() == 0   )		
			{
				bInnerSelect = TRUE;
			}
			//[�־ 2012/02/22] ��ϯѡ����ѯʱ,����ѯ�ն��ǵ�ǰ������,�����˷�����
			if (tSrcMt == m_tConf.GetSpeaker() &&
				tDstMt == m_tConf.GetChairman() &&
				(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode() ||
				 CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()))
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "It's Chairman select pollmode, Select source Mt(%u,%u) is speaker!\n",
					tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
			}
			//[��־�� 2010/12/01] ����speaker			
			else if(tSrcMt  == m_tConf.GetSpeaker() ||
				( tSrcMt.IsLocal() && m_ptMtTable->GetMtType(tSrcMt.GetMtId()) == MT_TYPE_SMCU &&
					!IsLocalAndSMcuSupMultSpy( GetMcuIdxFromMcuId(tSrcMt.GetMtId()) ) &&
					tSrcMt == GetLocalMtFromOtherMcuMt(m_tConf.GetSpeaker()) 
					|| tSrcMt == GetLocalMtFromOtherMcuMt(GetSecVidBrdSrc()))
				)
			{
				TMtStatus tStatus;
				m_ptMtTable->GetMtStatus( tDstMt.GetMtId(),&tStatus );
				TMt tSelVMt = tStatus.GetSelectMt( MODE_VIDEO );
				TMt tSelAMt = tStatus.GetSelectMt( MODE_AUDIO );
				TMt tVSrc,tASrc;
				m_ptMtTable->GetMtSrc( tDstMt.GetMtId(), &tVSrc, MODE_VIDEO );
				m_ptMtTable->GetMtSrc( tDstMt.GetMtId(), &tASrc, MODE_AUDIO );				

				if(MODE_BOTH == bySwitchMode)
				{
					if(GetVidBrdSrc().GetType() != TYPE_MCUPERI && (tSelVMt == GetVidBrdSrc() || tVSrc == GetVidBrdSrc())
						|| tSrcMt == GetSecVidBrdSrc())
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt] Now DstMt(%d.%d) Is See SrcMt(%d.%d),so change Mode to MODE_AUDIO.\n",
							tDstMt.GetMcuId(),tDstMt.GetMtId(),tSrcMt.GetMcuId(),tSrcMt.GetMtId());
						bySwitchMode = MODE_AUDIO;
					}
					
					if(GetAudBrdSrc().GetType() != TYPE_MCUPERI 
						&& (tSelAMt == GetAudBrdSrc() || tASrc == GetAudBrdSrc()))
					{
						bySwitchMode = (bySwitchMode == MODE_BOTH) ? MODE_VIDEO : MODE_NONE;
						
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt] Now DstMt(%d.%d) Is Listen SrcMt(%d.%d),so change Mode to %d.\n",
							tDstMt.GetMcuId(),tDstMt.GetMtId(),tSrcMt.GetMcuId(),tSrcMt.GetMtId(), bySwitchMode);
					}
				}
				else if(MODE_VIDEO == bySwitchMode)
				{
					if(GetVidBrdSrc().GetType() != TYPE_MCUPERI && (tSelVMt == GetVidBrdSrc() || tVSrc == GetVidBrdSrc())
						|| tSrcMt == GetSecVidBrdSrc())
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt] Now DstMt(%d.%d) Is See SrcMt(%d.%d),so change Mode to MODE_AUDIO.\n",
							tDstMt.GetMcuId(),tDstMt.GetMtId(),tSrcMt.GetMcuId(),tSrcMt.GetMtId());
						bySwitchMode = MODE_NONE;
					}
				}
				else if(MODE_AUDIO == bySwitchMode)
				{
					if(GetAudBrdSrc().GetType() != TYPE_MCUPERI 
						&& (tSelAMt == GetAudBrdSrc() || tASrc == GetAudBrdSrc()))
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt] Now DstMt(%d.%d) Is Listen SrcMt(%d.%d),so change Mode to MODE_NONE.\n",
							tDstMt.GetMcuId(),tDstMt.GetMtId(),tSrcMt.GetMcuId(),tSrcMt.GetMtId());
						bySwitchMode = MODE_NONE;
					}
				}
				else if (MODE_VIDEO2SECOND == bySwitchMode)
				{
					if(tSrcMt == GetSecVidBrdSrc())
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt] Now DstMt(%d.%d) Is Listen SrcMt(%d.%d),so change Mode to MODE_NONE.\n",
							tDstMt.GetMcuId(),tDstMt.GetMtId(),tSrcMt.GetMcuId(),tSrcMt.GetMtId());
						bySwitchMode = MODE_NONE;
					}
				}

				if( MODE_NONE == bySwitchMode )
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Specified SrcMt(%u,%u) has been SPEAKER,so no need to Selct Src!\n", 
						tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
					SendReplyBack( cMsg, cMsg.GetEventId() + 1 );
					return;
				}
				tSwitchInfo.SetMode( bySwitchMode );
			}

            //Ŀ���ն�δ����飬NACK
            if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
            {
                ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Specified Mt(%u,%u) not joined conference!\n", 
                    tDstMt.GetMcuId(), tDstMt.GetMtId() );
                cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }            
            
            // xsl [7/20/2006] ���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
            /*if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                //��ȡ��Ŀ���ն���һ��ѡ�����նˣ����ж��Ƿ�Ϊ�滻
                BOOL32 bOverNum = FALSE;
                TMtStatus tDstStatus;
                m_ptMtTable->GetMtStatus(tDstMt.GetMtId(), &tDstStatus);
                TMt tLastMt = tDstStatus.GetSelectMt(MODE_VIDEO);
                if (!tLastMt.IsNull())
                {
                    if (IsMtSendingVidToOthers(tLastMt, FALSE, FALSE, tDstMt.GetMtId()) && IsOverSatCastChnnlNum(0))
                    {
                        bOverNum = TRUE;
                    }                    
                }
                else if (IsOverSatCastChnnlNum(tLocalMt.GetMtId()))
                {
                    bOverNum = TRUE;
                }

                if (bOverNum)
                {
                    ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuStartSwitchMtReq] over max upload mt num. nack!\n");
                    cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
            }*/
        
			//����ʱ����ѡ��ģʽ
			if( m_tConf.m_tStatus.IsMixing()  )
			{
                //ѡ����Ƶ��NACK
				if( !(tSwitchInfo.GetDstMt() == m_tConf.GetChairman())  )
				{
					if( tSwitchInfo.GetMode() == MODE_AUDIO  )
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "CMcuVcInst: Conference %s is mixing now. Cannot switch only audio!\n", 
                             m_tConf.GetConfName() );
						cServMsg.SetErrorCode( ERR_MCU_SELAUDIO_INMIXING );
						SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
						return;
					}
					else if( tSwitchInfo.GetMode() == MODE_BOTH )
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "CMcuVcInst: Conference %s is mixing now. Cannot switch only audio!\n", 
                             m_tConf.GetConfName() );
						NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(),ERR_MCU_SELAUDIO_INMIXING );
						tSwitchInfo.SetMode( MODE_VIDEO );
					}					
				}
                
			}
            
			// [9/15/2010 xliang] vmp adapter channel seize prompt��
			if (MODE_VIDEO2SECOND != tSwitchInfo.GetMode())
			{
				TEqp tVmpEqp;
				if(!CheckSelSrcVmpAdaptChnl(tSrcMt, tDstMt, tSwitchInfo.GetMode(), tVmpEqp))
				{
					TVmpPriSeizeInfo tPriSeiInfo = g_cMcuVcApp.GetVmpPriSeizeInfo(tVmpEqp);
					//��¼��ռ�����ĺ�������
					if(tPriSeiInfo.m_tPriSeizeMember.IsNull())
					{
						tPriSeiInfo.m_tPriSeizeMember.SetMt(tSrcMt);
						tPriSeiInfo.m_wEventId = MCS_MCU_STARTSWITCHMT_REQ;
						//tSwitchInfo.SetMode(MODE_VIDEO); �˴�������tSwitchInfo������������ֱ��return
						tPriSeiInfo.m_tSwitchInfo = tSwitchInfo;
						tPriSeiInfo.m_tSwitchInfo.SetMode(MODE_VIDEO);
						tPriSeiInfo.m_bySrcSsnId = cServMsg.GetSrcSsnId();//��¼Դ�Ự��,���������Ƿ�mcs��קѡ��
						g_cMcuVcApp.SetVmpPriSeizeInfo(tVmpEqp, tPriSeiInfo);
					}
					
					if (MODE_VIDEO == tSwitchInfo.GetMode())
					{
						return;
					}
					else
					{
						tSwitchInfo.SetMode(MODE_AUDIO);
					}
				}
			}
			//��Ҫ�����ն˲���ѡ��
            //zbq[05/22/2009] ��Ҫ�����ѡ������������

            if((MODE_BOTH == tSwitchInfo.GetMode() || MODE_VIDEO == tSwitchInfo.GetMode()) &&
				!bVidAdp)
            {
                u16 wSrcMtBitrate   = m_ptMtTable->GetMtSndBitrate(tLocalMt.GetMtId());
                u16 wDstMtBitrate   = m_ptMtTable->GetMtReqBitrate(tDstMt.GetMtId());
                u16 wSrcDialBitrate = m_ptMtTable->GetSndBandWidth(tLocalMt.GetMtId());
                u16 wDstDialBitrate = m_ptMtTable->GetRcvBandWidth(tDstMt.GetMtId());      
                
                if (wSrcDialBitrate <= wDstDialBitrate && wSrcMtBitrate == wSrcDialBitrate &&
                    !m_tDoubleStreamSrc.IsNull())
                {
                    CServMsg cTmpServMsg;
                    TLogicalChannel tLogicalChannel;
                    if (m_ptMtTable->GetMtLogicChnnl(tLocalMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
                    {
                        tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(wSrcDialBitrate));
                        cTmpServMsg.SetMsgBody((u8 *)&tLogicalChannel, sizeof(tLogicalChannel));
                        SendMsgToMt(tLocalMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cTmpServMsg);
                    }                
                }
            }

			//�Ե�ǰѡ����Ŀ���նˣ�����ǰ��ѡ��Դ����Ŀǰ��ѡ��Դ�����ȶ���ǰ��ѡ�����д���
			//(��������MT�ֱ��ʵ�)
			// [12/28/2009 xliang] �ô������ҪŲ��"����ѡ��"����ǰ
			TMt tPreviousMt;	
			m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );
			tPreviousMt = tDstMtStatus.GetSelectMt(MODE_VIDEO);
			if( tPreviousMt.GetType() == TYPE_MT && !(tPreviousMt == tSrcMt) 
				&& (tSwitchInfo.GetMode() == MODE_VIDEO || tSwitchInfo.GetMode() == MODE_BOTH)
				)
			{
				//�����ֱ���
				if( ChgMtVidFormatRequired(tPreviousMt) )
				{
					//tPreviousMt = GetLocalMtFromOtherMcuMt( tPreviousMt);
					ChangeMtVideoFormat(tPreviousMt);
				}
				
				// ���µ���ѡ��Դ�ķ�������
				TMtStatus tSrcStatus;
				GetMtStatus(tPreviousMt, tSrcStatus);
				if ( m_tConfAllMtInfo.MtJoinedConf( tPreviousMt ) 
					&& tSrcStatus.IsSendVideo() 
					)
				{
					NotifyMtSend( tPreviousMt.GetMtId(), MODE_VIDEO );
				}
				//StopSelectSrc(tDstMt, MODE_VIDEO, FALSE);
			}

			//  [11/17/2009 pengjie] ������ش��ж�
			//zjj20100201 ��ش�����ϲ�
			if( !tSrcMt.IsLocal() )
			{
				if( IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()) )
				{
					TPreSetInReq tSpySrcInitInfo;
					tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt(tSrcMt);
					tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_tDstMt = tDstMt;
					tSpySrcInitInfo.m_bySpyMode = tSwitchInfo.GetMode();
					tSpySrcInitInfo.SetEvId(cServMsg.GetEventId());
					TMcsRegInfo tMcRegInfo;
					if (g_cMcuVcApp.GetMcsRegInfo(byMcsId, &tMcRegInfo))
					{
						tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.SetMcIp(tMcRegInfo.GetMcsIpAddr());
						tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.SetMcSSRC(tMcRegInfo.GetMcsSSRC());
					}				
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcStartSelSwitchMt] McsIP.%s, McSSrc.%x!\n", 
												 StrOfIP(tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.GetMcIp()),
												 tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.GetMcSSRC());

					// [pengjie 2010/8/25] �ϼ�ͬһ�ն�ѡ���¼���ͬ�ն��滻֧��
					TMtStatus tMtStatus;
					TMt tMtVidSelSrc;
					TMt tMtAudSelSrc;
					m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
					tMtVidSelSrc = tMtStatus.GetSelectMt( MODE_VIDEO );
					tMtAudSelSrc = tMtStatus.GetSelectMt( MODE_AUDIO );
// 					if( tMtVidSelSrc == tMtAudSelSrc && !( tMtVidSelSrc == tSrcMt ) )
// 					{
// 						tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtVidSelSrc;
// 						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_BOTH;
// 						tSpySrcInitInfo.m_tReleaseMtInfo.m_swCount = 0;
// 						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
// 				        tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
// 					}
					// End

					// [pengjie 2010/9/13] ��Ŀ�Ķ�����
					TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));	
					//zjl20101116 �����ǰ�ն��ѻش���������Ҫ���ѻش�Ŀ��������ȡС
					TSimCapSet tDstCapSet = m_ptMtTable->GetDstSCS(tDstMt.GetMtId());
					tSimCapSet.SetVideoMaxBitRate(tDstCapSet.GetVideoMaxBitRate());
					if(!GetMinSpyDstCapSet(tSrcMt, tSimCapSet))
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcStartSelSwitchMt] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
							tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
						return;
					}
					
					tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		            // End

					//[nizhijun 2011/01/15]������ش�ѡ���滻֧���޸�						
					//1.ԭ����ѡ��Դ��aud��vid��ͬ��
					if ( !tMtVidSelSrc.IsNull() && !tMtAudSelSrc.IsNull() && !(tMtVidSelSrc==tMtAudSelSrc))
					{
						//1.1ѡ�����ն˵�����Ƶ����Ҫ������presetin��Ϣ
						if (MODE_BOTH == tSwitchInfo.GetMode())
						{
							tSpySrcInitInfo.m_bySpyMode = MODE_VIDEO;													
							tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtVidSelSrc;
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_VIDEO;
							tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
							TPreSetInReq tTempPreSetInReq = tSpySrcInitInfo;
							OnMMcuPreSetIn( tTempPreSetInReq );

							tSpySrcInitInfo.m_bySpyMode = MODE_AUDIO;						
							tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtAudSelSrc;
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_AUDIO;
							tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
							OnMMcuPreSetIn( tSpySrcInitInfo );
						}
						//1.2 ��һѡ��vid��aud����ѡ��ģʽ����һ��presetin��Ϣ
						else
						{
							if (MODE_VIDEO == tSwitchInfo.GetMode())
							{
								tSpySrcInitInfo.m_bySpyMode = MODE_VIDEO;
								if (!tMtVidSelSrc.IsNull())
								{
									tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtVidSelSrc;
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_VIDEO;
									tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
								}
						    	OnMMcuPreSetIn( tSpySrcInitInfo );
							}
							else if (MODE_AUDIO == tSwitchInfo.GetMode())
							{
								tSpySrcInitInfo.m_bySpyMode = MODE_AUDIO;
								if (!tMtAudSelSrc.IsNull())
								{
									tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtAudSelSrc;
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_AUDIO;
									tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
								}
						    	OnMMcuPreSetIn( tSpySrcInitInfo );
							}					
						}
					}
					//2.ԭ����ѡ��Դ��aud��vid��ͬ��������ֻ����һ��,ֻ��һ��presetin��Ϣ
					else
					{
						tSpySrcInitInfo.m_bySpyMode = tSwitchInfo.GetMode();
						//������ѡ������Ƶ�������Ŀǰ���Ƿ�һ����Ϣ��releaseMT������ɵ�ѡ��VIDԴ
						if ( MODE_BOTH == tSwitchInfo.GetMode() )
						{							
							if (!tMtVidSelSrc.IsNull() && !tMtAudSelSrc.IsNull() )
							{
								tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtVidSelSrc;
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_BOTH;
								tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
							}
							else
							{
								if (!tMtVidSelSrc.IsNull())
								{
									tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtVidSelSrc;
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_VIDEO;
									tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
								}								
								else if (!tMtAudSelSrc.IsNull())
								{
									tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtAudSelSrc;
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_AUDIO;
									tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
								}
							}
							OnMMcuPreSetIn( tSpySrcInitInfo );
						}
						else if ( MODE_VIDEO == tSwitchInfo.GetMode())
						{							
							if (!tMtVidSelSrc.IsNull())
							{
								tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtVidSelSrc;
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_VIDEO;
								tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
							}
							OnMMcuPreSetIn( tSpySrcInitInfo );
						}
						else if (MODE_AUDIO == tSwitchInfo.GetMode())
						{
							if (!tMtAudSelSrc.IsNull())
							{
								tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtAudSelSrc;
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_AUDIO;
								tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
							}
							OnMMcuPreSetIn( tSpySrcInitInfo );
						}				
					}
					// End
					//OnMMcuPreSetIn( tSpySrcInitInfo );
									
					if ( MT_MCU_STARTSELMT_CMD != cServMsg.GetEventId() )
					{
						SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
					}  
					return;
				}
			}
			//zjj20100201 ��ش�����ϲ� ֮��ô������Ҫ��ֲ
			
			//[2011/10/14]
			tOldSelVid = tDstMtStatus.GetSelectMt(MODE_VIDEO);
			tOldSelAdu = tDstMtStatus.GetSelectMt(MODE_AUDIO);
			tOldSelSecVid = m_ptMtTable->GetMtSelMtByMode(tDstMt.GetMtId(), MODE_VIDEO2SECOND);

			/* xliang [4/2/2009] ȡ����ϯѡ��VMP��״̬, ��stopSelectSrc�н���
			if(tDstMt == m_tConf.GetChairman() 
				&& m_tConf.m_tStatus.IsVmpSeeByChairman() )
			{
				StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO);
			}
			else*/
			{
				StopSelectSrc( tDstMt,tSwitchInfo.GetMode(),FALSE,FALSE );
			}

			//����ѡ��			
            if ( !tSrcMt.IsLocal() )
            {
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[ProcStartSelSwitchMt]unlocal Mt(%d,%d) start setin!\n", tSrcMt.GetMcuId(), tSrcMt.GetMtId());
                //�鿴MC
                TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tSrcMt.GetMcuId());
                if(ptMcInfo == NULL)
                {
                    cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
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
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }

                OnMMcuSetIn( tSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
            }
			
            // ����@2006.4.17 �����ѡ��(��Ƶ������Ƶ)����Ҫ��¼����
            if(MODE_AUDIO == tSwitchInfo.GetMode() 
				|| MODE_VIDEO == tSwitchInfo.GetMode() 
				|| MODE_BOTH  == tSwitchInfo.GetMode() 
				|| MODE_VIDEO2SECOND  == tSwitchInfo.GetMode()
			  )
            {
				m_ptMtTable->SetMtSelMtByMode(tDstMt.GetMtId(), tSwitchInfo.GetMode(), tSrcMt);
            }
        
            if ( MT_MCU_STARTSELMT_CMD != cServMsg.GetEventId() )
            {
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
            }  
			else
			{
				//֪ͨ�ն�
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Send MCU_MT_STARTSELMT_ACK to mt.%d\n",tSwitchInfo.GetDstMt().GetMtId());
				SendMsgToMt(tSwitchInfo.GetDstMt().GetMtId(), MCU_MT_STARTSELMT_ACK, cServMsg);
			}

            //�ϼ�MCU
            if( cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMT_REQ &&
                (tDstMt.GetMtType() == MT_TYPE_SMCU || tDstMt.GetMtType() == MT_TYPE_MMCU) )
            {
                TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tDstMt.GetMtId()));
                if(ptConfMcInfo != NULL)
                {
                    ptConfMcInfo->m_tSpyMt = tLocalMt;
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
            
            //for h320 mcu cascade select
            if( MT_MCU_STARTSELMT_REQ == cServMsg.GetEventId() && 
                MT_TYPE_SMCU == m_ptMtTable->GetMtType(tDstMt.GetMtId()) )
            {            
                m_tLastSpyMt = tLocalMt;
            } 
			
			//����ѡ����MT�ڻ���ϳ��У���ȥ����ռVMP����ǰ����ͨ��
			if(MODE_VIDEO2SECOND != tSwitchInfo.GetMode() && ChgMtVidFormatRequired(tSrcMt))
			{ 
				ChangeMtVideoFormat(tSrcMt, FALSE); 
			}
            // xsl [7/21/2006] ֪ͨ�ն˽��յ�ַ
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

			//˫ѡ������Ŀ���ն�˫��ͨ��δ����ͨ���������һ��Դ������
            if (MODE_VIDEO2SECOND == tSwitchInfo.GetMode())
			{
				//ͨ��δ���ȿ�ͨ��
				TLogicalChannel tLogicalChannel;
				if (!m_ptMtTable->GetMtLogicChnnl(tDstMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, TRUE))
				{
					//ȡѡ��Դ����Ƶ������Ŀ���ն˵�˫��ͨ��
					if (m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
					{
						//����ͨ���Ƿ����H239��ǩ
						TCapSupport tCapSupport;
						if (m_ptMtTable->GetMtCapSupport(tDstMt.GetMtId(), &tCapSupport))
						{
							tLogicalChannel.SetSupportH239(tCapSupport.IsDStreamSupportH239());
						}
						else
						{
							tLogicalChannel.SetSupportH239(m_tConf.GetCapSupport().IsDStreamSupportH239());
						}
						
						McuMtOpenDoubleStreamChnnl(tDstMt, tLogicalChannel);
					}
					return;
				}
				
				//����һ��Դ�����ʣ�����
				BOOL32 bIsBrdNeedAdp = FALSE;
				if (!GetVidBrdSrc().IsNull() && GetVidBrdSrc().GetType() == TYPE_MT)
				{
					bIsBrdNeedAdp = IsNeedAdapt(GetVidBrdSrc(), tDstMt, MODE_VIDEO);
				}
				AdjustFitstSrcMtBit(tDstMt.GetMtId(), bIsBrdNeedAdp);
            }
        }            
		break;
        
    case MCS_MCU_STARTSWITCHMC_REQ:
        {
            // xsl [7/20/2006] ���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
           /* if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                if (IsOverSatCastChnnlNum(tSrcMt))
                {
                    ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuStartSwitchMtReq] over max upload mt num. nack!\n");
                    cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
            }*/

            //�����Լ���̬�غ���Ϣ
            TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
            TSimCapSet    tSrcSCS  = m_ptMtTable->GetSrcSCS( tLocalMt.GetMtId() ); 
            TDoublePayload tDVPayload;
            TDoublePayload tDAPayload;
			// ѡ������ϳ�ʱ,�û�������ʽ����
			if (tSrcMt.GetEqpType() == EQP_TYPE_VMP)
			{
				tSrcSCS  = m_tConf.GetMainSimCapSet(); 
				tSrcSCS.SetAudioMediaType(MEDIA_TYPE_NULL);
			}
            
            if( MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType() || 
                MEDIA_TYPE_H263PLUS == tSrcSCS.GetVideoMediaType() || 
                CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] ��ӦFEC֧��������ʽ
                m_tConf.GetCapSupportEx().IsVideoSupportFEC())
            {
                tDVPayload.SetRealPayLoad( tSrcSCS.GetVideoMediaType() );
                tDVPayload.SetActivePayload( GetActivePayload(m_tConf, tSrcSCS.GetVideoMediaType() ) );
            }
            else
            {
                tDVPayload.SetRealPayLoad( tSrcSCS.GetVideoMediaType() );
                tDVPayload.SetActivePayload( tSrcSCS.GetVideoMediaType() );
            }
            if( CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] ��ӦFEC֧��������ʽ
                m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
            {
                tDAPayload.SetRealPayLoad( tSrcSCS.GetAudioMediaType() );
                tDAPayload.SetActivePayload( GetActivePayload(m_tConf, tSrcSCS.GetAudioMediaType() ) );
            }
            else
            {
                tDAPayload.SetRealPayLoad( tSrcSCS.GetAudioMediaType() );
                tDAPayload.SetActivePayload( tSrcSCS.GetAudioMediaType() );
            }
            
            cMsg.SetMsgBody(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
            cMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
            cMsg.CatMsgBody( (u8 *)&tDVPayload, sizeof(tDVPayload));
            cMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
            cMsg.CatMsgBody( (u8 *)&tDAPayload, sizeof(tDAPayload));

            // zw [06/26/2008] ��ӦAAC LC��ʽ
			if ( MEDIA_TYPE_AACLC == tSrcSCS.GetAudioMediaType() )
			{
                TAudAACCap tAudAACCap;
                tAudAACCap.SetMediaType(MEDIA_TYPE_AACLC);
                tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
				TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
                tAudAACCap.SetChnlType(GetAACChnlTypeByAudioTrackNum(tAudioType.GetAudioTrackNum()));
                tAudAACCap.SetBitrate(96);
                tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
                
				cMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
			}
			//tianzhiyong 2010/04/02 ��ӦAAC LD��ʽ
			if ( MEDIA_TYPE_AACLD == tSrcSCS.GetAudioMediaType() )
			{
                TAudAACCap tAudAACCap;
                tAudAACCap.SetMediaType(MEDIA_TYPE_AACLD);
                tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
				TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
                tAudAACCap.SetChnlType(GetAACChnlTypeByAudioTrackNum(tAudioType.GetAudioTrackNum()));
                tAudAACCap.SetBitrate(96);
                tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
                
				cMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
			}

            SendReplyBack( cMsg, cMsg.GetEventId() + 1 );
			
			// [9/13/2010 xliang] if UI ask the same Src in the same MC channel, MCU should do nothing.
// 			TMt tMcAudCurSrc;
// 			TMt tMcVidCurSrc;
// 			g_cMcuVcApp.GetMcSrc( byMcsId, &tMcAudCurSrc, tSwitchInfo.GetDstChlIdx(), MODE_AUDIO );
// 			g_cMcuVcApp.GetMcSrc( byMcsId, &tMcVidCurSrc, tSwitchInfo.GetDstChlIdx(), MODE_VIDEO );
			//zjj20101119������岻ͬ����Ƶģʽ
			if( MODE_VIDEO == bySwitchMode &&  tMcVidCurSrc == tSrcMt )
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Mt(%d,%d) video already in Mc channel.%d, Do nothing!\n", tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSwitchInfo.GetDstChlIdx());
				return;
			}
			if( MODE_AUDIO == bySwitchMode && tMcAudCurSrc == tMcVidCurSrc )
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Mt(%d,%d) audio already in Mc channel.%d, Do nothing!\n", tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSwitchInfo.GetDstChlIdx());
				return;
			}
			if( MODE_BOTH == bySwitchMode && tMcVidCurSrc == tSrcMt && tMcAudCurSrc == tMcVidCurSrc )
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Mt(%d,%d) video and audio already in Mc channel.%d, Do nothing!\n", tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSwitchInfo.GetDstChlIdx());
				return;
			}

			// [1/27/2011 xliang] further filter mode
			if( MODE_BOTH == bySwitchMode && tMcVidCurSrc == tSrcMt && (!(tMcAudCurSrc == tSrcMt)) )
			{
				bySwitchMode = MODE_AUDIO;
				
			}
			else if( MODE_BOTH == bySwitchMode && (!(tMcVidCurSrc == tSrcMt)) && tMcAudCurSrc == tSrcMt )
			{
				bySwitchMode = MODE_VIDEO;
			}
			tSwitchInfo.SetMode(bySwitchMode);
			
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcStartSelSwitchMt] Switch Mt(%d.%d) To Mc(%d) Channel(%d)	Mode:(%d)\n",
				tSrcMt.GetMcuId(),tSrcMt.GetMtId(),
				cServMsg.GetSrcSsnId(),tSwitchInfo.GetDstChlIdx(),
				tSwitchInfo.GetMode()
				);
			
			// Դ�ǻ���ϳ�ʱ
			if (tSrcMt.GetEqpType() == EQP_TYPE_VMP)
			{
				// vmpֻ����Ƶû����Ƶ,�赥��ͣ��ԭ����Ƶ
				StopSwitchToMc( cServMsg.GetSrcSsnId(), tSwitchInfo.GetDstChlIdx(), FALSE, MODE_AUDIO );

				u8 bySrcChnnl = GetVmpOutChnnlByRes(tSrcMt.GetEqpId(),
					tSrcSCS.GetVideoResolution(),
					tSrcSCS.GetVideoMediaType(),
					tSrcSCS.GetUserDefFrameRate(),
					tSrcSCS.GetVideoProfileType());
				if ( 0xFF == bySrcChnnl ) //û�ҵ���Ӧ��ͨ��
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ProcStartSelSwitchMt] Get MCS vmp outchannl failed \n");
					return;
				}
				
				// 8kh�����ⳡ��
				if (VMP_8KH == GetVmpSubType(tSrcMt.GetEqpId()))
				{
					// ������1080p30fpsBP��������720p30fpsHP�������ʱ,Ϊ����vmp�㲥�ǹ㲥�л�ʱͨ���ı�,ȡ1ͨ��
					if (IsConfExcludeDDSUseAdapt())
					{
						if (VIDEO_FORMAT_HD1080 == tSrcSCS.GetVideoResolution() && 
							tSrcSCS.GetUserDefFrameRate() < 50 &&
							tSrcSCS.GetVideoProfileType() == emBpAttrb)
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
						if (VIDEO_FORMAT_HD720 == tSrcSCS.GetVideoResolution() && 
							tSrcSCS.GetUserDefFrameRate() < 50 &&
							tSrcSCS.GetVideoProfileType() == emHpAttrb)
						{
							if (IsMSSupportCapEx(VIDEO_FORMAT_HD720, tSrcSCS.GetUserDefFrameRate(), emBpAttrb))
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
				else if (VMP_8KI == GetVmpSubType(tSrcMt.GetEqpId()))
				{
					// ������1080p30fps��720p60fps�������ʱ,Ϊ����vmp�㲥�ǹ㲥�л�ʱͨ���ı�,ȡ1ͨ��
					if (IsConfExcludeDDSUseAdapt()) //�������
					{
						if ((m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 && m_tConf.GetMainVidUsrDefFPS() <= 30) ||
							(m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD720 && m_tConf.GetMainVidUsrDefFPS() > 30))
						{
							TKDVVMPOutParam tVMPOutParam = g_cMcuVcApp.GetVMPOutParam(tSrcMt);
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
				//����Դchnnl
				tSwitchInfo.SetSrcChlIdx(bySrcChnnl);
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[MCSSeeVmp] VMP --> MCS.%u, SrcChnnlIdx is %u\n",byMcsId, bySrcChnnl);
			}

			//  [11/17/2009 pengjie] ������ش��ж�
			if( !tSrcMt.IsLocal() )
			{
				if( IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()) )
				{
					TPreSetInReq tSpySrcInitInfo;
					tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt(tSrcMt);
					tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_tDstMt = tDstMt;
					tSpySrcInitInfo.m_bySpyMode = bySwitchMode;
					tSpySrcInitInfo.SetEvId(cServMsg.GetEventId());
					tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_byDstChlIdx = tSwitchInfo.GetDstChlIdx();
					tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_bySrcChlIdx = tSwitchInfo.GetSrcChlIdx();
					TMcsRegInfo tMcRegInfo;
					if (g_cMcuVcApp.GetMcsRegInfo(cServMsg.GetSrcSsnId(), &tMcRegInfo))
					{
						tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.SetMcIp(tMcRegInfo.GetMcsIpAddr());
						tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.SetMcSSRC(tMcRegInfo.GetMcsSSRC());
					}
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcStartSelSwitchMt:2] McsIP.%s, McSSrc.%x!\n",
												 StrOfIP(tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.GetMcIp()),
												 tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.GetMcSSRC());
					

					// [pengjie 2010/9/13] ��Ŀ�Ķ�����
					TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));
					//zjl20101116 �����ǰ�ն��ѻش���������Ҫ���ѻش�Ŀ��������ȡС
					if(!GetMinSpyDstCapSet(tSrcMt, tSimCapSet))
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcStartSelSwitchMt] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
							tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
						return;
					}
					
					tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		            // End

				    //zhouyiliang 20100814 ���ϴ������ͨ�����ն���ɿ��滻�ն�
					TMt tMcSrc;
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_NONE;
					TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);//�����ݽ�����VCS
		
					g_cMcuVcApp.GetMcSrc( byMcsId, &tMcSrc, tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_byDstChlIdx, MODE_VIDEO );
					if( !tMcSrc.IsNull() )
					{
						if( tMcSrc == tSrcMt )
						{
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "StartSwitchToMc GetMcSrc == NewMt, MODE_VIDEO\n" );
						}
						else
						{
							//[2011/11/24/zhangli]������vcs��ͬһmcu�����������������������Դ����ʱ�Կ��ͷŻ����Ƚ����У�
							//���䲻��ʱ��ProcMcuMcuPreSetInAck���ȰѼ��ͣ���ˣ��˲�������ȥ������������Ȼ������ȷ�Ļ���
							//��ͨ���������ҵ��Ҳ��������߼������Ǽ������ҵ��ÿ��VCS�ɶ����������ʲ����ۼӻ���
							u8 byMtInMcNum = 1;
							if( VCS_CONF == m_tConf.GetConfSource() )
							{
								byMtInMcNum = GetCanRlsNumInVcsMc(tMcSrc, MODE_VIDEO);
							}
							
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode += MODE_VIDEO;
							tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMcSrc;
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = byMtInMcNum;
							if( ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() ||
									ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() ) &&
									m_tRollCallee == tSpySrcInitInfo.m_tSpyMtInfo.GetSpyMt() )
							{								
								u8 byChlPos = tConfVmpParam.GetChlOfMtInMember(tMcSrc);
								if(	tConfVmpParam.GetVMPMode() == CONF_VMPMODE_CTRL  &&
									MAXNUM_VMP_MEMBER != byChlPos )
								{
									++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
								}
								TMtStatus tStatus;
								m_ptMtTable->GetMtStatus( m_tRollCaller.GetMtId(),&tStatus );
								if( !tStatus.GetSelectMt(MODE_VIDEO).IsNull() )
								{
									if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
									{
										++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
									}
									//zjj20101203 ����ʱ������,������ģʽ��,��ֱ���¼��ı������˻ش�Э��ʱҪ��1·���ͷ���Ƶ,�Ǹ������˿�����Ƶ
									if( ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() &&
										!m_tConfAllMcuInfo.IsSMcuByMcuIdx(m_tRollCallee.GetMcuId()) 
										)
									{
										++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
									}
								}
							}
							
							//zjj20101203 ����ʱ������,��������ģʽ��,��ֱ���¼��ı������˻ش�Э��ʱҪ��1·���ͷ���Ƶ,��1·���ͷ���Ƶ,��Ϊǰһ�����������Ƿ�����
							if( ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() &&
								m_tRollCallee == tSpySrcInitInfo.m_tSpyMtInfo.GetSpyMt() &&
								!m_tConfAllMcuInfo.IsSMcuByMcuIdx(m_tRollCallee.GetMcuId()) )
							{
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode += MODE_AUDIO;
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
							}
						}
					}
		
					g_cMcuVcApp.GetMcSrc( byMcsId, &tMcSrc, tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_byDstChlIdx, MODE_AUDIO );
					if( !tMcSrc.IsNull() )
					{
						if( tMcSrc == tSrcMt )
						{
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "StartSwitchToMc GetMcSrc == NewMt, MODE_AUDIO !\n" );
						}
						else
						{
							if( tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode == MODE_VIDEO ||
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode == MODE_NONE)
							{
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode += MODE_AUDIO;
							}		
							u8 byMtInMcNum = 1;
							if( VCS_CONF == m_tConf.GetConfSource() )
							{
								byMtInMcNum = GetCanRlsNumInVcsMc(tMcSrc, MODE_AUDIO);
							}
							
							tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMcSrc;
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum += byMtInMcNum;
						}
					}
					
					// [pengjie 2011/1/15] ��vcs������飬����˫����ģʽ���ͷ���Ƶ�ش�������һ(��һ·���ͷŵ���ش��ڵ�)
					if ( m_tConf.GetConfSource() == VCS_CONF && tConfVmpParam.IsVMPAuto() &&
						m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE &&
						VCS_VCMT_MCCHLIDX == tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_byDstChlIdx )  
					{
						++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
					}
			
					//zhouyiliang 20101015 ����˫��������������ϯѡ���նˣ���Ҫ�ټ�1
					if ( m_tConf.GetConfSource() == VCS_CONF && m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE ) 
					{
						//�ж���ϯѡ�����ն��Ƿ��ǿ��滻�ն�
						TMt tChairman = m_tConf.GetChairman();
						if ( !tChairman.IsNull() ) 
						{
							TMtStatus tStat;
							m_ptMtTable->GetMtStatus(tChairman.GetMtId(), &tStat );
							if (tStat.GetSelectMt( MODE_VIDEO ) == tMcSrc  ) 
							{
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
							}
							if (tStat.GetSelectMt( MODE_AUDIO ) == tMcSrc  ) 
							{
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
							}

						}

						
					}
					
					/*if (MODE_NONE != byReleaseMode  ) 
					{
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode = byReleaseMode;
						if( MODE_BOTH == tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode )
						{
							++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 
							++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
						}
						else if( MODE_VIDEO == tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode )
						{
							++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
						}
						else if( MODE_AUDIO == tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode )
						{
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
						}
				
						
													
					}*/
					OnMMcuPreSetIn( tSpySrcInitInfo );
					return;
				}
				//2010/05/12 lukunpeng �ڷǶ�ش�����£�Ҳ��Ҫ����setin��֪ͨ�¼�mcu�����ն˽��ϴ�ͨ��
				else
				{
				
					//zhouyiliang 20110225 ���¼��е�ǰ�����նˣ���ͬһ���¼��������ն˱�ѡ������NACK
					if( m_tConf.GetConfSource() == VCS_CONF && !m_cVCSConfStatus.GetCurVCMT().IsNull()
						&&  IsMtInMcu(GetLocalMtFromOtherMcuMt(tSrcMt), m_cVCSConfStatus.GetCurVCMT()) 
						&& !( m_cVCSConfStatus.GetCurVCMT() == tSrcMt ))
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ProcVcsMcuMtInTwMsg] CurVCMT(%d.%d) and Mt(%d.%d) is in same smcu,Mt can't be selected \n ",
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
					
					// ����ó�Ա����smcu���ϴ�ͨ���ն�,�����ٴη���SetIn��Ϣ
					TMt tMcu = GetLocalMtFromOtherMcuMt(tSrcMt);
					if (!(tSrcMt == GetSMcuViewMt(tMcu, TRUE)))
					{
						OnMMcuSetIn( tSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
					}
				}
			}
        }
        break;
        
    default:
        ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
                 cServMsg.GetEventId(), ::OspEventDesc( cServMsg.GetEventId() ), CurState() );
        break;    
    }
	
	//[nizhijun 2010/11/30]����
	//[nizhijun 2010/11/17]tSrcMt == GetVidBrdSrc()��֧�ֶ�ش�����£�ѡ���¼�MCU
    // ������Ҫ�����Ƿ��ǹ㲥����, zgc, 2008-06-03
    // BUG2354, ��Ҫ��ֹ��ؼ�ؽ���, zgc, 2008-06-12
	//zjj20101201ѡ�����������Ըı䣬������ɾ��
    /*if( tSrcMt == GetVidBrdSrc() && cServMsg.GetEventId() != MCS_MCU_STARTSWITCHMC_REQ &&
        ( !( (tSrcMt==m_tRollCaller && GetLocalMtFromOtherMcuMt(tDstMt)==GetLocalMtFromOtherMcuMt(m_tRollCallee)) 
		|| (tSrcMt==GetLocalMtFromOtherMcuMt(m_tRollCallee) && tDstMt==m_tRollCaller) ) ) &&
        ( MODE_VIDEO == tSwitchInfo.GetMode() || MODE_BOTH == tSwitchInfo.GetMode() ) )
    {
        ConfLog( FALSE, "Dst(Mt.%d) receive Src(Mt.%d) as video broadcast src!\n", tDstMt.GetMtId(), tSrcMt.GetMtId() );
        
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
	}*/

	//[2011/10/14/zhangli]
	//�ϼ�MT1ѡ���¼�MT1��Ȼ���ϼ�MT2ѡ���¼�MT2�����ش���Ҫ���ϼ�MT1��MT2�����¼�MT2
	//�޸��ն˵�ѡ��Դ����������䣬��ĵ�������ռ�õ�bas ��Դ
	//R3_Full�Ȱ汾����setin�·���ûProcMcuMcuSetOutNotify ���������Ҫ�����ﴦ��
	if (!tSrcMt.IsLocal() && !IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuIdx()))
	{
		TMtStatus tCurrMtStatus;
		TMt tCurrMt;
		TMt tSelVidMt;
		TMt tSelAudMt;
		
		TBasOutInfo tOutInfo;

		for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
		{
			if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
			{
				continue;
			}
			tCurrMt = m_ptMtTable->GetMt(byMtId);
			m_ptMtTable->GetMtStatus(byMtId, &tCurrMtStatus);
			tSelVidMt = tCurrMtStatus.GetSelectMt(MODE_VIDEO);
			tSelAudMt = tCurrMtStatus.GetSelectMt(MODE_AUDIO);
			
			if (!tSelVidMt.IsNull() && tSrcMt.GetMcuId() == tSelVidMt.GetMcuId() && !(tSelVidMt == tSrcMt))
			{	
				if (IsNeedSelAdpt(tSelVidMt, tCurrMt, MODE_VIDEO))
				{
					if (FindBasChn2SelForMt(tSelVidMt, tCurrMt, MODE_VIDEO, tOutInfo))
					{
						SetBasChnSrc(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId, tSrcMt);
					}
				}
				tCurrMtStatus.SetVideoMt(tSrcMt);
				tCurrMtStatus.SetSelectMt(tSrcMt, MODE_VIDEO);
				m_ptMtTable->SetMtStatus(byMtId, &tCurrMtStatus);
			}
			
			if (!tSelAudMt.IsNull() && tSrcMt.GetMcuId() == tSelAudMt.GetMcuId() && !(tSelAudMt == tSrcMt))
			{	
				if (IsNeedSelAdpt(tSelAudMt, tCurrMt, MODE_AUDIO))
				{
					if (FindBasChn2SelForMt(tSelVidMt, tCurrMt, MODE_AUDIO, tOutInfo))
					{
						SetBasChnSrc(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId, tSrcMt);
					}
				}
				tCurrMtStatus.SetAudioMt(tSrcMt);
				tCurrMtStatus.SetSelectMt(tSrcMt, MODE_AUDIO);
				m_ptMtTable->SetMtStatus(byMtId, &tCurrMtStatus);
			}
		}
	}

	TSwitchInfo tTempSwitchInfo;
	if(MODE_BOTH == tSwitchInfo.GetMode() || MODE_VIDEO == tSwitchInfo.GetMode())
	{
		if (bVidAdp)
		{
			if(!StartSelAdapt(tSwitchInfo.GetSrcMt(), tSwitchInfo.GetDstMt(), MODE_VIDEO, bInnerSelect))
			{
				TMtStatus tMtStatus;
				m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
				tMtStatus.SetSelectMt( tOldSelVid, MODE_VIDEO);
                m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tMtStatus );
				/*20110610 zjl Bug00055391 ���ǰ����ͨ��û������ô�������ռǰ����ͨ��������Ƶѡ������������䲻�㵼��ѡ��ʧ�ܣ�
						   ��ȡ��ѡ���ͷ�ǰ����ͨ�����������ն�ѡ��״̬�������������ռ������ʧ��Ҫ�ͷ���ռ�õ�ǰ����ͨ��
			   */
				ClearAdpChlInfoInAllVmp(tSwitchInfo.GetSrcMt());
				//m_tVmpChnnlInfo.ClearChnlByMt( tSwitchInfo.GetSrcMt() );

				if (!tOldSelVid.IsNull())
				{
					StopSelectSrc(tDstMt, MODE_VIDEO);
				}
			}
		}
		else
		{
			memcpy(&tTempSwitchInfo, &tSwitchInfo, sizeof(tTempSwitchInfo));
			tTempSwitchInfo.SetMode(MODE_VIDEO);
			SwitchSrcToDst(tTempSwitchInfo, cServMsg);
		}
	}
	
	if (MODE_BOTH == tSwitchInfo.GetMode() || MODE_AUDIO == tSwitchInfo.GetMode())
	{
		if (bAudAdp)
		{
			if(!StartSelAdapt(tSwitchInfo.GetSrcMt(), tSwitchInfo.GetDstMt(), MODE_AUDIO, bInnerSelect))
			{
				TMtStatus tMtStatus;
				m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
				tMtStatus.SetSelectMt( tOldSelAdu, MODE_AUDIO);
                m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tMtStatus );

				if (!tOldSelAdu.IsNull())
				{
					StopSelectSrc(tDstMt, MODE_AUDIO);
				}
			}
		}
		else
		{
			memcpy(&tTempSwitchInfo, &tSwitchInfo, sizeof(tTempSwitchInfo));
			tTempSwitchInfo.SetMode(MODE_AUDIO);
			SwitchSrcToDst(tTempSwitchInfo, cServMsg);
		}
	}

	if (MODE_SECVIDEO == tSwitchInfo.GetMode())
	{
		SwitchSrcToDst(tSwitchInfo, cServMsg);
	}

	if(MODE_VIDEO2SECOND == tSwitchInfo.GetMode())
	{
		if (bSecVidAdp)
		{
			if (!StartSelAdapt(tSwitchInfo.GetSrcMt(), tSwitchInfo.GetDstMt(), MODE_VIDEO2SECOND, bInnerSelect))
			{
				m_ptMtTable->SetMtSelMtByMode(tDstMt.GetMtId(), MODE_VIDEO2SECOND, tOldSelSecVid);
				
				if (!tOldSelSecVid.IsNull())
				{
					StopSelectSrc(tDstMt, MODE_VIDEO2SECOND);
				}
			}
		}
		else
		{
			memcpy(&tTempSwitchInfo, &tSwitchInfo, sizeof(tTempSwitchInfo));
			SwitchSrcToDst(tTempSwitchInfo, cServMsg);
		}
	}
	
// #ifdef _8KE_
// 	if (bSelAccordAdp)
// #else
//     if (bSelAccordAdp && g_cMcuVcApp.IsSelAccord2Adp())
// #endif
//     {
//         if (MODE_BOTH == tSwitchInfo.GetMode())
//         {
//             tSwitchInfo.SetMode(MODE_AUDIO);
//             SwitchSrcToDst(tSwitchInfo, cServMsg);
//             tSwitchInfo.SetMode(MODE_VIDEO);
//         }
//         SwitchSrcToDstAccord2Adp( tSwitchInfo, cServMsg );
//     }
//     else
//     {
// 	    SwitchSrcToDst( tSwitchInfo, cServMsg );
//     }

	
	// ���÷�����ԴΪ�ǻ����Զ�ָ��, zgc, 2008-04-14
	if ( HasJoinedSpeaker() && tDstMt == GetLocalSpeaker() )
	{
		SetSpeakerSrcSpecType( tSwitchInfo.GetMode(), SPEAKER_SRC_MCSDRAGSEL );
	}
    
    return;
}

/*==============================================================================
������    :  ProcMcsMcuStartSwitchVmpMtReq
����      :  ������ϯ�ն�ѡ��VMP����
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  void
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-1-7					Ѧ��							����
==============================================================================*/
void CMcuVcInst::ProcMcsMcuStartSwitchVmpMtReq(const CMessage * pcMsg)
{
	STATECHECK;
    
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt tDstMt;

	if ( cServMsg.GetEventId() == MT_MCU_STARTSWITCHVMPMT_REQ )
	{
		tDstMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	}
	else
	{
		tDstMt = *(TMt *)cServMsg.GetMsgBody();
	}
	
	// ֧�ַ���ϯ�ն�ѡ��vmp
	TMtSeeVmp(cServMsg);
	/*if( HasJoinedChairman() && m_tConf.GetChairman().GetMtId() == tDstMt.GetMtId() )
	{
		ChairmanSeeVmp(cServMsg);
	}
	else
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcsMcuStartSwitchVmpMtReq] tDstMt.%d isn't ConfChairman.%d!\n", 
			tDstMt.GetMtId(), m_tConf.GetChairman().GetMtId() );
	}*/
	
	return;
}

/*====================================================================
    ������      ��GetMtNumSeeVmp
    ����        �����ѡ��vmpָ��ͨ���ն˵ĸ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byChnnl [in]		ָ��ͨ��
				  u8 byNeglectMtId [in]	�����ն�
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	5/09/2012 	4.7			�־        ����
====================================================================*/
u8 CMcuVcInst::GetMtNumSeeVmp(u8 byVmpId, u8 byChnnl, u8 byNeglectMtId)
{
	u8 byMtNum = 0;
	if (byChnnl >= MAXNUM_MPU2_OUTCHNNL)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[GetMtNumSeeVmp] The byChnnl(%d) is wrong!\n", byChnnl);
		return byMtNum;
	}
	
	TLogicalChannel tFwLogicChnnl;
	TMtStatus tMtStatus;
	TMt tSelectSrc;
	for ( u8 byMtLoop = 1; byMtLoop <= MAXNUM_CONF_MT; byMtLoop ++)
	{
		if (m_tConfAllMtInfo.MtJoinedConf(byMtLoop) && byNeglectMtId != byMtLoop)
        {
			tFwLogicChnnl.Clear();
			tMtStatus.Clear();
			if (!m_ptMtTable->GetMtLogicChnnl( byMtLoop,LOGCHL_VIDEO,&tFwLogicChnnl,TRUE ) )
			{
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"[GetMtNumSeeVmp]Get DstMt��%d forward logicalchnnl failed \n",byMtLoop );
				continue;
			}
			m_ptMtTable->GetMtStatus(byMtLoop, &tMtStatus);
			tSelectSrc = tMtStatus.GetSelectMt(MODE_VIDEO);
			// ѡ��Դ��������vmpʱ,����ͳ��
			if (!(TYPE_MCUPERI == tSelectSrc.GetType() && EQP_TYPE_VMP == tSelectSrc.GetEqpType()))
			{
				continue;
			}
			// ѡ��Դ����ָ��vmpʱ,����ͳ��
			if (tSelectSrc.GetEqpId() != byVmpId)
			{
				continue;
			}

			u8 byMtRecVmpChnnl  = GetVmpOutChnnlByRes(byVmpId,
				tFwLogicChnnl.GetVideoFormat(),
				tFwLogicChnnl.GetChannelType(),
				tFwLogicChnnl.GetChanVidFPS(), 
				tFwLogicChnnl.GetProfileAttrb());
			
            if (byMtRecVmpChnnl != byChnnl )
			{
				continue;
			}

			// ����+1
			byMtNum++;
		}
	}

	return byMtNum;
}

/*====================================================================
    ������      ��TMtSeeVmp
    ����        ���ն�ѡ��vmp����������״̬�Ĳ���,������ϯ�����ϯ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CServMsg &cServMsg
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	4/14/2009 	4.6			Ѧ��          ����
	5/09/2012 	4.7			�־        ��ͨ�ն�ѡ��vmp֧��
====================================================================*/
void CMcuVcInst::TMtSeeVmp(CServMsg &cServMsg, BOOL32 bNeglectVmpMode /* = FALSE */)
{
	/*����ϯ�ն˲���ѡ��VMP
	if (!HasJoinedChairman())
	{
		cServMsg.SetErrorCode(ERR_MCU_INVALID_OPER);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ChairmanSeeVmp] Only chairman can select see VMP!\n");
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}*/
	
	//���鲻��vmp��
	u8 byVmpCount = GetVmpCountInVmpList();
	if (byVmpCount == 0)	
	{
		cServMsg.SetErrorCode(ERR_MCU_VMPNOTSTART);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[TMtSeeVmp] The Conf is not in the VMP mode!\n");
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}
	
	TMt tDstMt;
	u8 byVmpId;
	if ( cServMsg.GetEventId() == MT_MCU_STARTSWITCHVMPMT_REQ )
	{
		tDstMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
		if (!HasJoinedChairman() || m_tConf.GetChairman().GetMtId() != tDstMt.GetMtId())
		{
			cServMsg.SetErrorCode(ERR_MCU_INVALID_OPER);
			ConfPrint( LOG_LVL_WARNING, MID_MCU_MCS, "[TMtSeeVmp] tDstMt.%d isn't ConfChairman.%d!\n", 
				tDstMt.GetMtId(), m_tConf.GetChairman().GetMtId() );
			SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
			return ;
		} 
		byVmpId = GetTheOnlyVmpIdFromVmpList();//��õ�ǰΨһ�ϳ�������
	}
	else
	{
		byVmpId = cServMsg.GetEqpId();
		tDstMt = *(TMt *)cServMsg.GetMsgBody();
	}

	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	u16 wError = 0;
	BOOL32 bIsChairManSeeVmp = FALSE;
	if (!CheckTMtSeeVmp(tVmpEqp, tDstMt, bIsChairManSeeVmp, wError))
	{
		cServMsg.SetErrorCode(wError);
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}

	SendReplyBack(cServMsg,cServMsg.GetEventId()+1);
	
	if (bIsChairManSeeVmp)
	{
		if(cServMsg.GetEventId() != MT_MCU_STARTSWITCHVMPMT_REQ)
		{
			// xliang [7/3/2009] ����ϯ�ն�Ҳ��Ӧ���Ӷ�ʹmcs��mtc״̬һ��
			SendMsgToMt( tDstMt.GetMtId(), MCU_MT_STARTSWITCHVMPMT_ACK, cServMsg);
		}
		
		//m_tConf.m_tStatus.SetVmpSeebyChairman(TRUE);
		/*tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
		g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );//ˢ��tPeriEqpStatus*/
	}

	// xliang [4/15/2009] ����ϯ�ն˴�ʱ����ѡ��ĳ��MT����Ҫȡ���Ǹ���ϯѡ��Mt��
	// �Ǹ���ѡ����MT���û������MT��ѡ����������MT��ѡ��ȡ�����������˳�ǰ����
	u8 byMode = MODE_VIDEO;
	TMtStatus tDstMtStatus;
	m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );
	if ( !(tDstMtStatus.GetSelectMt(byMode).IsNull()) && tDstMtStatus.GetSelectMt(byMode).GetType() == TYPE_MT )
	{
		StopSelectSrc(tDstMt, byMode);
	}

	// StopSelectSrc�л����MtStatus��Ϣ,�˴�������ȡ����MtStatus
	m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );
	tDstMtStatus.SetSelectMt( tVmpEqp, byMode ); //��¼�µ�ѡ��Դ
	m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );

	if(g_cMcuVcApp.IsBrdstVMP(tVmpEqp))
	{
		//����ϳɱ������ڹ㲥�������˴������ٽ�����
		//ѡ��״̬�ڴ˴��ϱ�
		MtStatusChange( &tDstMt, TRUE );
	}
	else
	{
//		TLogicalChannel tFwLogicChnnel;
//		//zhouyiliang 20101222 ����vmp���ն˽�����������ͨ��m_cMtRcvGrp
//		if (!m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), byChnnlType, &tFwLogicChnnel, TRUE) ) 
//		{
//			ConfLog(FALSE,"[ChairmanSeeVmp]Get DstMt(%d,%d) forward logicalchnnl failed \n",tDstMt.GetMcuId(),tDstMt.GetMtId() );
//			return;
//		}
//		
//		u8 bySrcChnnl  = GetVmpOutChnnlByRes( tFwLogicChnnel.GetVideoFormat(), 
//												m_tVmpEqp.GetEqpId(),
//												m_tConf, 
//												tFwLogicChnnel.GetChannelType() );
		u8 bySrcChnnl = GetVmpOutChnnlByDstMtId( tDstMt.GetMtId(), tVmpEqp.GetEqpId());
		if ( 0xFF == bySrcChnnl ) //û�ҵ���Ӧ��ͨ��
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[TMtSeeVmp] Get DstMt(%d,%d) vmp outchannl failed \n",tDstMt.GetMcuId(),tDstMt.GetMtId() );
			return;
		}

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[TMtSeeVmp] VMP --> MT.%u, SrcChnnlIdx is %u\n",tDstMt.GetMtId(), bySrcChnnl);

		// ������ͨ������
		AdjustVmpSelBitRate(tVmpEqp);

		//������
		TSwitchInfo tSwitchInfo;
		tSwitchInfo.SetDstMt(tDstMt);
		tSwitchInfo.SetSrcMt(tVmpEqp);
		tSwitchInfo.SetSrcChlIdx(bySrcChnnl);
		tSwitchInfo.SetMode(MODE_VIDEO);

		SwitchSrcToDst( tSwitchInfo, cServMsg );
	}
}

/*==============================================================================
������    :  CheckTMtSeeVmp
����      :  �ն�ѡ������ϳ�Check
�㷨ʵ��  :  
����˵��  :  u8 byVmpId
			 const TMt &tDstMt
			 u16 &wError
����ֵ˵��:  BOOL32
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
130530					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::CheckTMtSeeVmp(const TEqp &tVmpEqp, const TMt &tDstMt, BOOL32 &bIsChairManSeeVmp, u16 &wError)
{
	if (tVmpEqp.IsNull() || !IsValidVmpId(tVmpEqp.GetEqpId()) || !IsVmpIdInVmpList(tVmpEqp.GetEqpId()))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[TMtSeeVmp] VmpId(%d) is wrong!\n", tVmpEqp.GetEqpId());
		return FALSE;
	}
	
	//��VMP��֧��ѡ��
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;			//ȡvmp������
	if(byVmpSubType == VMP)
	{
		wError = ERR_FUNCTIONNOTSUPPORTBYOLDVMP;
		return FALSE;
	}
	
	//vmp�㲥״̬��,�ն�ѡ��vmpû������,��Nack
	if (g_cMcuVcApp.IsBrdstVMP(tVmpEqp))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[TMtSeeVmp] tMt can't select see VMP because the VMP is Brdst!\n");
		return FALSE;
	}
	
	// �Ǳ����ն˲�֧��ѡ������ϳ�
	if (tDstMt.IsNull() || !tDstMt.IsLocal())
	{
		wError = ERR_MCU_OPRONLYLOCALMT;
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[TMtSeeVmp] tDstMt is null or is not local. can't select see VMP because Chairman Polling!\n");
		return FALSE;
	}
	
	bIsChairManSeeVmp = FALSE;
	//����������"��ϯ��ѯѡ��"ģʽ��������ѡ��VMP
	if (HasJoinedChairman() && m_tConf.GetChairman().GetMtId() == tDstMt.GetMtId())
	{
		bIsChairManSeeVmp = TRUE;
		//20110414_miaoqs ��ϯ��ѯѡ���������
		//20120903 zhouyiliang ��ϯѡ��vmp��ͣ��ѯ
		if ( (CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()) 
			||(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()) )
		{
			// 			cServMsg.SetErrorCode(ERR_MCU_SPECCHAIREMAN_NACK);
			// 			ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[TMtSeeVmp] Chairman can't select see VMP because Chairman Polling!\n");
			// 			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	//nack
			// 			return;
			ProcStopConfPoll();
		}
	}
	
    //�Ѵ���ǿ�ƹ㲥״̬��������ѡ������ϳ�
    if ( m_tConf.m_tStatus.IsMustSeeSpeaker() && !bIsChairManSeeVmp) 
    {
		wError = ERR_MCU_MUSTSEESPEAKER;
		return FALSE;
    }
	
	u8 byChnnlType = LOGCHL_VIDEO;   
    TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) ||
		!m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), byChnnlType, &tLogicChannel, TRUE ) )
    {
		wError = ERR_MCU_GETMTLOGCHN_FAIL;
        ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ChairmanSeeVmp] Mt.%u isn't online in conf or forward logical channel not open!\n",tDstMt.GetMtId());
		return FALSE;
    }

	// ƥ�䲻������ϳ�ͨ����ͬ��Nack,�˴����Һ���ͨ��ʱ�����Ե�ǰѡ��Դ
	u8 byVmpOutChl  = GetVmpOutChnnlByRes(tVmpEqp.GetEqpId(), 
		tLogicChannel.GetVideoFormat(), 
		tLogicChannel.GetChannelType(),
		tLogicChannel.GetChanVidFPS(), 
		tLogicChannel.GetProfileAttrb());
	if (0XFF == byVmpOutChl)
	{
		wError = ERR_MCU_VCS_NOVCMTVID;
        ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[TMtSeeVmp] Mt.%u can't get correct vmp out channel!\n",tDstMt.GetMtId());
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
    ������      MCSSeeVmp
    ����        �����ѡ������������״̬�Ĳ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CServMsg &cServMsg
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	3/29/2012 	4.7			yanghuaizhi     ����
====================================================================*/
void CMcuVcInst::MCSSeeVmp(CServMsg &cServMsg, BOOL32 bNeglectVmpMode /* = FALSE */)
{
	TSwitchInfo	tSwitchInfo;
	TMt	tDstMt, tSrcMt;
	CServMsg  cMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() ); 
	
    tSwitchInfo = *( TSwitchInfo * )cServMsg.GetMsgBody();
    tSrcMt = tSwitchInfo.GetSrcMt();
    tDstMt = tSwitchInfo.GetDstMt();
	u8 bySwitchMode = tSwitchInfo.GetMode();	// ��¼��ʼMODE
	
	if (tSrcMt.GetType() != TYPE_MCUPERI || tSrcMt.GetEqpType() != EQP_TYPE_VMP)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[MCSSeeVmp] tSrcMt(%d) is not VmpEqp(%d)!\n",tSrcMt.GetEqpId(),tSrcMt.GetEqpId());
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//���鲻��vmp��
	u8 byVmpCount = GetVmpCountInVmpList();
	if (0 == byVmpCount || IsVmpIdInVmpList(tSrcMt.GetEqpId()))	
	{
		cServMsg.SetErrorCode(ERR_MCU_VMPNOTSTART);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ChairmanSeeVmp] The Conf is not in the VMP mode!\n");
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}
	
	//��VMP��֧��ѡ��
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( tSrcMt.GetEqpId(), &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;			//ȡvmp������
	if(byVmpSubType == VMP)
	{
		cServMsg.SetErrorCode(ERR_FUNCTIONNOTSUPPORTBYOLDVMP);
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}
	
	if (cServMsg.GetEventId() != MCS_MCU_STARTSWITCHMC_REQ)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[MCSSeeVmp] EventId[%d] is not MCS_MCU_STARTSWITCHMC_REQ!\n",cServMsg.GetEventId());
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	// ѡ��vmp,mode������Ϊaudio
	if (MODE_NONE == bySwitchMode || MODE_AUDIO == bySwitchMode)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[MCSSeeVmp] SwitchMode(%d) is wrong!\n", bySwitchMode);
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	/* ���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ���� ��ȷ���Ƿ���Ҫ
	if (m_tConf.GetConfAttrb().IsSatDCastMode())
	{
		if (IsOverSatCastChnnlNum(tSrcMt))
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuStartSwitchMtReq] over max upload mt num. nack!\n");
			cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
	}*/

    //�����Լ���̬�غ���Ϣ
	u8 byMcsId = cServMsg.GetSrcSsnId();
    TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
    TSimCapSet    tSrcSCS  = m_tConf.GetMainSimCapSet(); 
	tSrcSCS.SetAudioMediaType(MEDIA_TYPE_NULL);
    TDoublePayload tDVPayload;
    TDoublePayload tDAPayload;
    
    if( MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType() || 
        MEDIA_TYPE_H263PLUS == tSrcSCS.GetVideoMediaType() || 
        CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
        // zbq [10/29/2007] ��ӦFEC֧��������ʽ
        m_tConf.GetCapSupportEx().IsVideoSupportFEC())
    {
        tDVPayload.SetRealPayLoad( tSrcSCS.GetVideoMediaType() );
        tDVPayload.SetActivePayload( GetActivePayload(m_tConf, tSrcSCS.GetVideoMediaType() ) );
    }
    else
    {
        tDVPayload.SetRealPayLoad( tSrcSCS.GetVideoMediaType() );
        tDVPayload.SetActivePayload( tSrcSCS.GetVideoMediaType() );
    }
    if( CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
        // zbq [10/29/2007] ��ӦFEC֧��������ʽ
        m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
    {
        tDAPayload.SetRealPayLoad( tSrcSCS.GetAudioMediaType() );
        tDAPayload.SetActivePayload( GetActivePayload(m_tConf, tSrcSCS.GetAudioMediaType() ) );
    }
    else
    {
        tDAPayload.SetRealPayLoad( tSrcSCS.GetAudioMediaType() );
        tDAPayload.SetActivePayload( tSrcSCS.GetAudioMediaType() );
    }
    
    cMsg.SetMsgBody(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
    cMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
    cMsg.CatMsgBody( (u8 *)&tDVPayload, sizeof(tDVPayload));
    cMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
    cMsg.CatMsgBody( (u8 *)&tDAPayload, sizeof(tDAPayload));

    SendReplyBack( cMsg, cMsg.GetEventId() + 1 );
	
	// [9/13/2010 xliang] if UI ask the same Src in the same MC channel, MCU should do nothing.
	TMt tMcAudCurSrc;
	TMt tMcVidCurSrc;
	g_cMcuVcApp.GetMcSrc( byMcsId, &tMcAudCurSrc, tSwitchInfo.GetDstChlIdx(), MODE_AUDIO );
	g_cMcuVcApp.GetMcSrc( byMcsId, &tMcVidCurSrc, tSwitchInfo.GetDstChlIdx(), MODE_VIDEO );
	//zjj20101119������岻ͬ����Ƶģʽ
	if( MODE_VIDEO == bySwitchMode &&  tMcVidCurSrc == tSrcMt )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Mt(%d,%d) video already in Mc channel.%d, Do nothing!\n", tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSwitchInfo.GetDstChlIdx());
		return;
	}
	if( MODE_BOTH == bySwitchMode && tMcVidCurSrc == tSrcMt && tMcAudCurSrc == tMcVidCurSrc )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Mt(%d,%d) video and audio already in Mc channel.%d, Do nothing!\n", tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSwitchInfo.GetDstChlIdx());
		return;
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcStartSelSwitchMt] Switch Mt(%d.%d) To Mc(%d) Channel(%d)	Mode:(%d)\n",
		tSrcMt.GetMcuId(),tSrcMt.GetMtId(),
		cServMsg.GetSrcSsnId(),tSwitchInfo.GetDstChlIdx(),
		tSwitchInfo.GetMode()
		);

	u8 bySrcChnnl = GetVmpOutChnnlByRes(tSrcMt.GetEqpId(),
		tSrcSCS.GetVideoResolution(),
		tSrcSCS.GetVideoMediaType(),
		tSrcSCS.GetUserDefFrameRate(),
		tSrcSCS.GetVideoProfileType());
	if ( 0xFF == bySrcChnnl ) //û�ҵ���Ӧ��ͨ��
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[MCSSeeVmp] Get MCS vmp outchannl failed \n");
		return;
	}

	// 8kh�����ⳡ��
	if (VMP_8KH == GetVmpSubType(tSrcMt.GetEqpId()))
	{
		// ������1080p30fpsBP��������720p30fpsHP�������ʱ,Ϊ����vmp�㲥�ǹ㲥�л�ʱͨ���ı�,ȡ1ͨ��
		if (IsConfExcludeDDSUseAdapt())
		{
			if (VIDEO_FORMAT_HD1080 == tSrcSCS.GetVideoResolution() && 
				tSrcSCS.GetUserDefFrameRate() < 50 &&
				tSrcSCS.GetVideoProfileType() == emBpAttrb)
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
			if (VIDEO_FORMAT_HD720 == tSrcSCS.GetVideoResolution() && 
				tSrcSCS.GetUserDefFrameRate() < 50 &&
				tSrcSCS.GetVideoProfileType() == emHpAttrb)
			{
				if (IsMSSupportCapEx(VIDEO_FORMAT_HD720, tSrcSCS.GetUserDefFrameRate(), emBpAttrb))
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
	else if (VMP_8KI == GetVmpSubType(tSrcMt.GetEqpId()))
	{
		// ������1080p30fps��720p60fps�������ʱ,Ϊ����vmp�㲥�ǹ㲥�л�ʱͨ���ı�,ȡ1ͨ��
		if (IsConfExcludeDDSUseAdapt()) //�������
		{
			if ((m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 && m_tConf.GetMainVidUsrDefFPS() <= 30) ||
				(m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD720 && m_tConf.GetMainVidUsrDefFPS() > 30))
			{
				TKDVVMPOutParam tVMPOutParam = g_cMcuVcApp.GetVMPOutParam(tSrcMt);
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
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[MCSSeeVmp] VMP --> MCS.%u, SrcChnnlIdx is %u\n",byMcsId, bySrcChnnl);
	
	//������
	TSwitchInfo tTempSwitchInfo;
	memcpy(&tTempSwitchInfo, &tSwitchInfo, sizeof(tTempSwitchInfo));
	tTempSwitchInfo.SetSrcChlIdx(bySrcChnnl);
	tTempSwitchInfo.SetMode(MODE_VIDEO);
	
	SwitchSrcToDst( tTempSwitchInfo, cServMsg );

}

/*====================================================================
    ������      ��ProcMtMcuStopSwitchMtReq
    ����        ��ֹͣ�ն�ѡ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	02/12/19	1.0			LI Yi         ����
	04/02/26    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::ProcMtMcuStopSwitchMtReq(const CMessage * pcMsg)
{
    STATECHECK;

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TMt tDstMt = *(TMt*)cServMsg.GetMsgBody();

    /*@ 20110603 zjl Mcsͣѡ��ͨ�ŽṹΪTSwitchInfo(��ѡ��ģʽ)
                     �ն�ͣѡ��ͨ�ŽṹΪTMt,
                     ��������������Ϣ����
                     ��ֱ�ӽ�ѡ��ģʽȫͣ��*/
    u8 byStopMode = MODE_BOTH;
    if( cServMsg.GetEventId() == MCS_MCU_STOPSWITCHMT_REQ )
    {
        TSwitchInfo tSwitchInfo = *(TSwitchInfo*)cServMsg.GetMsgBody();
        byStopMode = tSwitchInfo.GetMode();
    }     

    // miaoqingsong [2011/08/09] ��ϯ��ѯģʽ�²�����ȡ����Ŀ�Ķˡ�Ϊ��ϯ�ն˵�ѡ��
	if ( ((CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()) ||
		(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode())) &&
		tDstMt == m_tConf.GetChairman() && cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId())
	{
        // [1/25/2013 liaokang] ��������ϯ��ѯ������Ƶ����£�����ȡ��ѡ��
        if( CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode() || 
            ( CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode() && !( MODE_AUDIO == byStopMode ) ) )
        {
            ConfPrint( LOG_LVL_ERROR, MID_MCU_MT, "Can't stop chairman's select because Chairman Polling!\n" );
            cServMsg.SetErrorCode( ERR_MCU_SPECCHAIREMAN_NACK );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	//nack
            return;
        }		
	}

    if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
    {
        ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcMtMcuStopSwitchMtReq] Dst Mt%u-%u not joined conference!\n", 
                         tDstMt.GetMcuId(), tDstMt.GetMtId() );
        cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }
    
    if( cServMsg.GetEventId() == MCS_MCU_STOPSWITCHMT_REQ )
    {
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
    } 
	//�����ϯ��ѡ��,�з�����ʱ,ĳЩ���ն�(��8010A)�յ�h_ctrl_SimpleConfInfoInd��ᷢMT_MCU_STOPSELMT_CMD��Ϣ��MCU
	//������ϯѡ����ȡ��,���ն�ԭ��,�˴�������Ӧ.���ն˶������ٻظ�MT_MCU_STOPSELMT_CMD��Ϣ����ȡ��ѡ��.

	if (byStopMode & MODE_AUDIO)
	{
		StopSelectSrc(tDstMt, MODE_AUDIO);
	}
	
	if (byStopMode & MODE_VIDEO)
	{
		StopSelectSrc(tDstMt, MODE_VIDEO);
	}
	
	if (byStopMode & MODE_VIDEO2SECOND)
	{
		StopSelectSrc(tDstMt, MODE_VIDEO2SECOND);
	}

	// ���÷�����ԴΪ��Դ, zgc, 2008-04-14
	if ( HasJoinedSpeaker() && tDstMt == GetLocalSpeaker() )
	{
		SetSpeakerSrcSpecType( MODE_BOTH, SPEAKER_SRC_NOTSEL );
	}
    
    return;
}

/*====================================================================
    ������      ��ProcMtMcuOpenLogicChnnlRsp
    ����        ���ն˷����Ĵ��߼�ͨ��Ӧ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/01/23    1.0         LI Yi         ����
	04/02/12    3.0         ������        ������
	05/12/19	4.0			�ű���		  T120����ͨ�������޸�
====================================================================*/
void CMcuVcInst::ProcMtMcuOpenLogicChnnlRsp( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt;
	u8	bySrcChnnl, byChannel = 0;
	TLogicalChannel	tLogicChnnl;

	switch( CurState() )
	{
	case STATE_ONGOING:
    {
		TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();
		tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );
		TMt tSpeaker = m_tConf.GetSpeaker();

		if( pcMsg->event == MT_MCU_OPENLOGICCHNNL_NACK )
		{
			// vrs��¼��֧�֣�ͨ����ʧ�ܣ��Ҷ�vrs��¼��ʵ��
			if (tMt.GetMtType() == MT_TYPE_VRSREC)
			{
				// �ͷ�vrs��¼����Դ�������Ϣ
				ReleaseVrsMt(tMt.GetMtId());

				return;
			}

			if(tLogicChnnl.GetMediaType() == MODE_VIDEO)
			{
                //[9/27/2011 zhushengze]��һ�δ򿪲��ɹ��ٳ����ø���ʽ��
                TMultiCapSupport tComCap;
                if (m_ptMtTable->GetMtMultiCapSupport( tMt.GetMtId(), &tComCap))
                {
                    TSimCapSet tSecCap = tComCap.GetSecondSimCapSet();
                    
                    if ( MEDIA_TYPE_NULL != tSecCap.GetVideoMediaType() &&
                         tLogicChnnl.GetChannelType() != tSecCap.GetVideoMediaType() )
                    {
                        tLogicChnnl.SetChannelType(tSecCap.GetVideoMediaType());
                        tLogicChnnl.SetVideoFormat(tSecCap.GetVideoResolution());
                        tLogicChnnl.SetChanVidFPS(tSecCap.GetUserDefFrameRate());
						//����Profile(HP/BP)����[12/9/2011 chendaiwei]
                        tLogicChnnl.SetProfieAttrb(tSecCap.GetVideoCap().GetH264ProfileAttrb());
                        CServMsg cOpenMainChnlMsg;
                        u8 byRes = tLogicChnnl.GetVideoFormat();
                        cOpenMainChnlMsg.SetMsgBody((u8*)&tLogicChnnl, sizeof(TLogicalChannel));
                        cOpenMainChnlMsg.CatMsgBody(&byRes, sizeof(u8));
                        SendMsgToMt(tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cOpenMainChnlMsg);
                        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp]Open MT:%d Main Logic Channel again with SecMain!\n", tMt.GetMtId());
                        
                        return;
                    }
                }

				m_ptMtTable->SetMtVideoRecv( tMt.GetMtId(), FALSE );
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mcu open Mt%d primary video logicChannel(0x%x:%d%s) error!\n", 
				tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
				tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
			}
			if(tLogicChnnl.GetMediaType() == MODE_AUDIO)
			{
				m_ptMtTable->SetMtAudioRecv( tMt.GetMtId(), FALSE );
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mcu open Mt%d audio logicChannel(0x%x:%d%s:%d) error!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ),tLogicChnnl.GetAudioTrackNum() );
			}
			if(tLogicChnnl.GetMediaType() == MODE_SECVIDEO)
			{
                //[6/16/2011 zhushengze]��һ�δ򿪲��ɹ��ٳ����õڶ���ͬ˫����
                TMultiCapSupport tMultiComCap;
                if (m_ptMtTable->GetMtMultiCapSupport( tMt.GetMtId(), &tMultiComCap))
                {
                    TDStreamCap tSecDSCap = tMultiComCap.GetSecComDStreamCapSet();

                    if (/*tSecDSCap.GetMediaType() == MEDIA_TYPE_H264 &&*/
                        MEDIA_TYPE_NULL != tSecDSCap.GetMediaType() &&
                        tLogicChnnl.GetVideoFormat() != tSecDSCap.GetResolution())
                    {
                        tLogicChnnl.SetVideoFormat(tSecDSCap.GetResolution());
                        tLogicChnnl.SetChanVidFPS(tSecDSCap.GetUserDefFrameRate());
                        tLogicChnnl.SetProfieAttrb(tSecDSCap.GetH264ProfileAttrb());
                        CServMsg cOpenSecChnlMsg;
                        u8 byRes = tLogicChnnl.GetVideoFormat();
                        cOpenSecChnlMsg.SetMsgBody((u8*)&tLogicChnnl, sizeof(TLogicalChannel));
                        cOpenSecChnlMsg.CatMsgBody(&byRes, sizeof(u8));
                        SendMsgToMt(tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cOpenSecChnlMsg);
                        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp]Open MT:%d DS Logic Channel again with SecCom DS!\n", tMt.GetMtId());
                        
                        return;
                    }
                }
                
				m_ptMtTable->SetMtVideo2Recv( tMt.GetMtId(), FALSE );
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mcu open Mt%d second video logicChannel(0x%x:%d%s) error!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
                
                //  xsl [6/15/2006] ��˫��ʧ��ʱ���ն˻ָ����մ����������Զ��������򲻻ָ�
                if (m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE) == GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())))
                {
                    u16 wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(tMt.GetMtId());
                    m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), wMtDialBitrate, LOGCHL_VIDEO);
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuOpenLogicChnnlRsp]tDstMt.GetMtId() = %d, MtReqBitrate = %d\n",
                        tMt.GetMtId(), wMtDialBitrate);
                }            

				//�����л������õ�һ·
				StartSwitchDStreamToFirstLChannel( tMt );

				if (!(m_ptMtTable->GetMtSelMtByMode(tMt.GetMtId(), MODE_VIDEO2SECOND).IsNull()))
				{
					m_ptMtTable->RemoveMtSelMtByMode(tMt.GetMtId(), MODE_VIDEO2SECOND);
					
					//���ն�״̬��˫ѡ��״̬
					MtStatusChange(&tMt, TRUE);
					MtSecSelSrcChange(tMt);
				}
			}

			//�����T120����ͨ��ʧ��
			if ( MODE_DATA == tLogicChnnl.GetMediaType() )
			{
				if ( MEDIA_TYPE_T120 == tLogicChnnl.GetChannelType() )
				{
					if ( m_ptMtTable->IsMtIsMaster( tMt.GetMtId() ) && m_ptMtTable->IsNotInvited( tMt.GetMtId() ))
					{
						SendMcuDcsDelMtReq( tMt.GetMtId() );
						ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Mcu open mt%d t120 logicchannel (0x%x:%d%s) as master failed !\n",
							tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
							tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) ); 
					}		
					//����Զ�����, ����KDV MCU, ������������
					else
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "Mcu open mt%d t120 logicchannel (0x%x:%d%s) as slave failed, its legal affair if KEDA MCU as master !\n",
							tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
							tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) ); 
					}
				}
			}

			//����VCS����, ����ʾ���ն��޷�����ͼ��
			u8 byMediaType = tLogicChnnl.GetVideoFormat();
			if (VCS_CONF == m_tConf.GetConfSource() && 
				(MODE_VIDEO == byMediaType || MODE_AUDIO == byMediaType))
			{
				VCSMTAbilityNotif(tMt, byMediaType);
			}
			return;
		}

		//vcs���ٺ��йҶ��¼������ϼ����¼��Ľ���δ����Ϊ��Ϣ��Ϣ��osp�����ж�����
		//�ϼ��Ҷ��¼��󣬲��յ�֮ǰ���¼�ʱ���¼����ϼ����߼�ͨ����Ӧ��
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_CALL,"[ProcMtMcuOpenLogicChnnlRsp] Mt%d doesnot join conf, then doesnot deal open chl rsp\n", 
				tMt.GetMtId() );
			return;
		}

        // �����ǳɹ����߼�ͨ���Ĵ���
		switch( tLogicChnnl.GetMediaType() )
		{
		case MODE_VIDEO:
			{
				m_ptMtTable->SetMtVideoRecv( tMt.GetMtId(), TRUE, tLogicChnnl.GetFlowControl() );
				byChannel = LOGCHL_VIDEO;
				MtStatusChange( &tMt, TRUE );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mcu open Mt%d primary video logicChannel([IP]:0x%x:%d,[Type]:%s,[Res]:%s,[Fps]:%d,[BR]:%d) success!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ),
					GetResStr(tLogicChnnl.GetVideoFormat()), tLogicChnnl.GetChanVidFPS(),
					tLogicChnnl.GetFlowControl());

				//�����ǰ�ն˿��Ľ���ͨ����mpeg4 auto���ж��Ƿ���Ҫ�ı�MP4����Ĳ���
				BOOL32 bMepg4AutoMt = FALSE;
				u16 wMtBitRate = 0;
				if (tMt.IsLocal() )
				{
					if (tLogicChnnl.GetChannelType() == MEDIA_TYPE_MP4 && tLogicChnnl.GetVideoFormat() == VIDEO_FORMAT_AUTO ) 
					{
						bMepg4AutoMt = TRUE;
						wMtBitRate = m_ptMtTable->GetDialBitrate(tMt.GetMtId());
					}
				}
				if ( bMepg4AutoMt && m_tConf.GetSecBitRate() != 0) 
				{
					u8 byOutChnl = GetVmpOutChnnlByRes(m_tVmpEqp.GetEqpId(), VIDEO_FORMAT_AUTO,MEDIA_TYPE_MP4);
					if (0XFF != byOutChnl)
					{
						u16 wMinMp4GrpBirate = GetMinMtRcvBitByVmpChn(m_tVmpEqp.GetEqpId(), TRUE,byOutChnl);
						if ( wMtBitRate < wMinMp4GrpBirate )
						{
							//�ı�mp4 ����·�������
							TPeriEqpStatus tVmpStatus;
							g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpStatus);
							TVMPParam_25Mem tVmpParam = tVmpStatus.m_tStatus.tVmp.GetVmpParam();
							if (tVmpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::WAIT_START || 
								tVmpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::START) 
							{
								ChangeVmpParam(m_tVmpEqp.GetEqpId(), &tVmpParam);
							}
						}
					}
				}

				// xliang [11/10/2008] �򿪵�һ·ͨ���ɹ���ֱ�Ӵ�Polycom��MCU�ڶ�·ͨ��
				if ( ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) ||
					   MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId()) )
					&& 
					 ( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
					   MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) )
					)
				{
					SetTimer( EV_MCU_OPENSECVID_TIMER, TIMESPACE_WAIT_FSTVIDCHL_SUC, tMt.GetMtId() );
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[OpenLogicChnnlRsp] Mt.%d Fst chnnl open succeed, try the second one immediately due to Polycom\n", tMt.GetMtId() );
				}
			}
			break;

		case MODE_SECVIDEO:
			//zjj20120801 2��mcu�����������ն˷���,�Ҷ�����˫��,�ϼ������¼�,�����ϼ���Ϊ�¼�Ϊ����Դ,�¼���Ϊ�ϼ�Ϊ˫��Դ,����ȫ��˫���޷�����
			if( !m_tDoubleStreamSrc.IsNull() && tMt.GetMtId() == m_tDoubleStreamSrc.GetMtId() &&
				m_ptMtTable->GetManuId(tMt.GetMtId()) == MT_MANU_KDCMCU )
			{				
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp] m_tDoubleStreamSrc.%d can't open forward sec chl\n",
								m_tDoubleStreamSrc.GetMtId() );
				u8 byOut = 1;
				cServMsg.SetEventId( MCU_MT_CLOSELOGICCHNNL_CMD );
				cServMsg.SetMsgBody( ( u8 * )&tLogicChnnl, sizeof( tLogicChnnl ) );	
				cServMsg.CatMsgBody( &byOut, sizeof(byOut) );				
				SendMsgToMt( tMt.GetMtId(), MCU_MT_CLOSELOGICCHNNL_CMD, cServMsg );
				return;				
			}
			m_ptMtTable->SetMtVideo2Recv( tMt.GetMtId(), TRUE );
			byChannel = LOGCHL_SECVIDEO;
			MtStatusChange( &tMt, TRUE );
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "Mcu open Mt%d second video logicChannel([IP]:0x%x:%d,[H239]:%d,[type]:%s,[Res]:%s,[Fps]:%d) success!\n", 
                tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
                tLogicChnnl.m_tRcvMediaChannel.GetPort(), tLogicChnnl.IsSupportH239(), 
                GetMediaStr(tLogicChnnl.GetChannelType()),
                GetResStr(tLogicChnnl.GetVideoFormat()),
                    tLogicChnnl.GetChanVidFPS());
        
			//zbq[09/02/2008] �����ϵ��նˣ�ResΪ�գ������������루��ֻ����˫����
			if (0 == tLogicChnnl.GetVideoFormat())
			{
				TCapSupport tMtCap;
				if (!m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tMtCap))
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[OpenLogicChanRsp] get mt.%d's cap failed, check it\n", tMt.GetMtId());
				}
				tLogicChnnl.SetVideoFormat(tMtCap.GetDStreamCapSet().GetResolution());
			}
			// xliang [11/14/2008]  �򿪵ڶ�·ͨ���ɹ����жϵ�ǰ�Ƿ���˫��Դ�������������Ʋ���˫������
			if ( (MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) ||
				 MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId()) )
				&&
				(MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
				MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ) &&
				!m_tDoubleStreamSrc.IsNull() && !(tMt == m_tDoubleStreamSrc) )
			{
				//��������
				TH239TokenInfo tH239TokenInfo;
				
				CServMsg cSendServMsg;
				cSendServMsg.SetEventId(MCU_POLY_GETH239TOKEN_REQ);
				cSendServMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));
				cSendServMsg.CatMsgBody((u8*)&tH239TokenInfo, sizeof(tH239TokenInfo));
				SendMsgToMt(tMt.GetMtId(), MCU_POLY_GETH239TOKEN_REQ, cSendServMsg );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[OpenChnnlRsp] MCU_POLY_GETH239TOKEN_REQ to PolyMCU.%d\n", tMt.GetMtId());
				
			}
			break;

		case MODE_AUDIO:
			m_ptMtTable->SetMtAudioRecv( tMt.GetMtId(), TRUE );
			byChannel = LOGCHL_AUDIO;
			MtStatusChange( &tMt, TRUE );
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mcu open Mt%d audio logicChannel(0x%x:%d:%s:%d) success!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ),tLogicChnnl.GetAudioTrackNum() );
			break;
		case MODE_DATA:
			if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_H224 )
			{
				byChannel = LOGCHL_H224DATA;
				TMtStatus tMtStatus;
				m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus);
				tMtStatus.SetIsEnableFECC(TRUE);
				m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tMtStatus);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mcu open Mt%d data H224 logicChannel(0x%x:%d:%s) success!\n", 
						tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
						tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
			}

			//t120 ����ͨ���򿪳ɹ�, �߼�ͨ���ı���
			else if ( MEDIA_TYPE_T120 == tLogicChnnl.GetChannelType() ) 
			{					
				//�����������, ��Ҫ����Զ˵ĵ�ַ�Ͷ˿�
				if ( !m_ptMtTable->IsNotInvited( tMt.GetMtId() ) )
				{	
					u32 dwDcsIp  = tLogicChnnl.m_tRcvMediaChannel.GetIpAddr();
					u16 wDcsPort = tLogicChnnl.m_tRcvMediaChannel.GetPort();

					byChannel = LOGCHL_T120DATA;
					
					//��ַ��Ч
					if ( 0 != dwDcsIp && 0 != wDcsPort )
					{
						//����õ�ַ, ����ʱ����������֮��
						m_ptMtTable->SetMtDcsAddr( tMt.GetMtId(), dwDcsIp, wDcsPort );

						//����DCS���Ӹ��ն�, ���ӷ�ʽΪ�����¼�
						SendMcuDcsAddMtReq( tMt.GetMtId(), dwDcsIp, wDcsPort );

						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mcu open Mt%d data T.120 logicChannel(0x%x:%d:%s) as master success!\n", 
							tMt.GetMtId(), dwDcsIp, wDcsPort, GetMediaStr( tLogicChnnl.GetChannelType() ) );
					}
					else
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mcu open Mt%d data T.120 logicChannel(0x%x:%d:%s) as master failed!\n", 
							tMt.GetMtId(), dwDcsIp, wDcsPort, GetMediaStr( tLogicChnnl.GetChannelType() ) );
					}
				}
				//������˱���
				else
				{
					//����ֻ���յ�MT��ACK��Ϣ, ������Ϊͨ���򿪳ɹ�
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mcu open Mt%d data T.120 logicChannel as master success!\n", tMt.GetMtId() );
				}
			}
			break;
		default:
			break;
		}

        // [2013/04/08 chenbing] H460���߼�����
        //ǰ��ͨ������rtcp��
        if( cServMsg.GetMsgBodyLen()  >= sizeof(BOOL32) + sizeof(TLogicalChannel))
        {
            BOOL32 bIsPinHole = *( BOOL32 * )( cServMsg.GetMsgBody() + sizeof(TLogicalChannel));
            if ( TRUE == bIsPinHole &&
                 ( MODE_VIDEO == tLogicChnnl.GetMediaType() || 
                   MODE_AUDIO == tLogicChnnl.GetMediaType() ||
                   MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
               )
            {
                Starth460PinHoleNotify(tLogicChnnl.m_tSndMediaCtrlChannel.GetIpAddr(),
                    tLogicChnnl.m_tSndMediaCtrlChannel.GetPort(),
                    tLogicChnnl.m_tRcvMediaCtrlChannel.GetIpAddr(),
                    tLogicChnnl.m_tRcvMediaCtrlChannel.GetPort(),
                    pinhole_rtcp,
                    tLogicChnnl.m_byActivePayload);
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_bIsPinHole = TRUE;
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  
                    "[ProcMtMcuOpenLogicChnnlRsp] H460 LocalRtcpAddr:%d LocalRtcpPort:%d RemoteRtcpAddr:%d RemoteRtcpPort:%d Payload:%d!\n", 
                tLogicChnnl.m_tSndMediaCtrlChannel.GetIpAddr(), 
                tLogicChnnl.m_tSndMediaCtrlChannel.GetPort(), 
                tLogicChnnl.m_tRcvMediaCtrlChannel.GetIpAddr(), 
                tLogicChnnl.m_tRcvMediaCtrlChannel.GetPort(), 
                tLogicChnnl.m_byActivePayload);
                
            }
        }

		BOOL32 bIsBrdVidNeedAdp = FALSE;	//˫�㲥�Ƿ������

		//�����߼�ͨ��
		if( tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO )
		{           
            //�ն����д����ʼ��
			
			//zbq[09/10/2008] ���д��������������ѭ�����õĴ���
			if (!m_ptMtTable->GetRcvBandAdjusted(tMt.GetMtId()))
			{
				m_ptMtTable->SetRcvBandWidth(tMt.GetMtId(), m_ptMtTable->GetDialBitrate(tMt.GetMtId()));
			}

// 			TDri2E1Cfg tDri2E1Cfg[MAXNUM_SUB_MCU];
// 			g_cMcuVcApp.GetDri2E1CfgTable(tDri2E1Cfg);
// 			u16 wRealBandwidth = 0;
// 			for ( u8 byCfgIndx = 0; byCfgIndx < MAXNUM_SUB_MCU; byCfgIndx++)
// 			{
// 				u32 dwConIpAddr = tDri2E1Cfg[byCfgIndx].GetIpAddr();
// 				//�ж�MMCU(SMCU)�Ƿ��������ļ�������[10/25/2012 chendaiwei]
// 				if( g_cMcuVcApp.GetRealBandwidthByIp(dwConIpAddr,wRealBandwidth)
// 					&& dwConIpAddr == m_ptMtTable->GetIPAddr(tMt.GetMtId())
// 					&& (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU
// 					|| m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MMCU))
// 				{
// 					m_ptMtTable->SetMtTransE1(tMt.GetMtId(),TRUE);
// 					
// 					u16 wActualBR = wRealBandwidth - GetAudioBitrate(m_tConf.GetMainAudioMediaType());
// 					m_ptMtTable->SetRcvBandWidth(tMt.GetMtId(),wActualBR);
// 					
// 					break;
// 				}
// 			}
			
            if (!m_tDoubleStreamSrc.IsNull() || !GetSecVidBrdSrc().IsNull())
            {
                //zbq[02/01/2008] ���ٺ���ն˵������ʲ������ں��룬ֱ��ȡ���ٺ�Ĵ���
                if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
                     m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()) ) 
                {
                    tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId())) );
                }
                else
                {
                    //tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())) );
					LogPrint(LOG_LVL_DETAIL,MID_MCU_CALL,"[ProcMtMcuOpenLogicChnnlRsp] Main FlowControl adjust to %d\n",tLogicChnnl.GetFlowControl());
                }
                //����FlowControl��ʱ��, ��ֹ˫�ٻ��������ն����ʵ��ڵڶ�·����, 07-01-31
				//�ն����ʲ�ͬ����������Զ�����ͨ��ȡС����Դ�ն�����
				//SetTimer( MCUVC_SENDFLOWCONTROL_TIMER, 1200 );
            }
            /*else
            {
                tLogicChnnl.SetFlowControl( m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) );
            }*/

			// [12/11/2009 xliang] send flow control indication to Aethra's MMCU
			if ( MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId())
				&& MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId())
				)
			{
				cServMsg.SetMsgBody( ( u8 * )&tLogicChnnl, sizeof( tLogicChnnl ) );
				
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq]Send flowcontrol indication to Mt.%u due to Eathra! br: %d\n", 
					tMt.GetMtId(), tLogicChnnl.GetFlowControl());
				
				SendMsgToMt(tMt.GetMtId(), MCU_MT_FLOWCONTROLINDICATION_NOTIF, cServMsg);
				
			}
		}
		
        //  xsl [4/28/2006] ����˫�����ʱ���������Ƶ����
        else if ( tLogicChnnl.GetMediaType() == MODE_SECVIDEO && byChannel == LOGCHL_SECVIDEO )
        {
			// ��һ·�㲥�Ƿ������ ˫ѡ��ʱ��Ҫ���ն˵�һ·���ʣ�˫��ͨ���򿪺�����е�һ·�������Ƿ�����䣬�ڱ����������ǰ�ж���
			if (m_tConf.GetConfAttrb().IsUseAdapter()
				&& !GetVidBrdSrc().IsNull() && GetVidBrdSrc().GetType() == TYPE_MT)
			{
				bIsBrdVidNeedAdp = IsNeedAdapt(GetVidBrdSrc(), tMt, MODE_VIDEO);
			}

            //zbq[01/31/2008] �����ն˷������Ļ�׼Ӧ���ǽ��ٺ�Ĵ���, �ǽ����ն�Ϊ���д���
            if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
                 m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()) )
            {
                tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId()), FALSE) );
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp] Lowed TMt.%d set DSVBR.%d\n", tMt.GetMtId(), tLogicChnnl.GetFlowControl());
                
                //�����ն˴˴�ͬʱ���������һ·��Ƶ����.
                m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId()), TRUE), LOGCHL_VIDEO );
            }
            else
            {
                tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE) );
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp] TMt.%d set DSVBR.%d\n", tMt.GetMtId(), tLogicChnnl.GetFlowControl());
            }
            
            //zbq[02/01/2008] ����FlowControl��ʱ��, ��ֹ���ն�ΪE1�����նˣ�����������ǰ����������Ĵ���
			//�ն����ʲ�ͬ����������Զ�����ͨ��ȡС����Դ�ն�����
            //SetTimer( MCUVC_SENDFLOWCONTROL_TIMER, 1200 );            

            //zbq[02/18/2007] ��˫��ͨ����������ǰ��VMP�㲥����
//             if ( m_tConf.m_tStatus.IsBrdstVMP() )
//             {
//                 AdjustVmpBrdBitRate();
//             }

            /*
            // ��ӽ�������, zgc, 2008-08-07
            u8 emType = m_cConfVidTypeMgr.ConvertOut2In( tLogicChnnl.GetChannelType(), tLogicChnnl.GetVideoFormat(), TRUE );
            m_cConfVidTypeMgr.AddType( emType, TRUE );*/

			//zbq[09/12/2008]д��֡��
			//zjl[10/21/2010]Ŀǰ�ն��ϱ��ﺬ��֡��
// 			TCapSupport tCap;
// 			m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tCap);
// 			if (tCap.GetDStreamCapSet().IsFrameRateUserDefined())
// 			{
// 				tLogicChnnl.SetChanVidFPS(tCap.GetDStreamCapSet().GetUserDefFrameRate());
// 			}
// 			else
// 			{
// 				tLogicChnnl.SetChanVidFPS(tCap.GetDStreamCapSet().GetFrameRate());
// 			}
        }
		tLogicChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
		tLogicChnnl.SetActivePayload(GetActivePayload(m_tConf,tLogicChnnl.GetChannelType()));
		m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannel, &tLogicChnnl, TRUE );
	
		if ( g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) )
		{
			AdjustVmpBrdBitRate();
        }

		//���˼���mcu��Ϣ����������б���
		if( MEDIA_TYPE_MMCU == tLogicChnnl.GetChannelType() )
		{
			u16 wMcuIdx = INVALID_MCUIDX;
			u8 abyMcuId[MAX_CASCADEDEPTH-1];
			memset( &abyMcuId[0],0,sizeof(abyMcuId) );
			abyMcuId[0] = tMt.GetMtId();
			m_tConfAllMcuInfo.AddMcu( &abyMcuId[0],2,&wMcuIdx );
			m_ptConfOtherMcTable->AddMcInfo( wMcuIdx );//tMt.GetMtId() );	
			m_tConfAllMtInfo.AddMcuInfo( wMcuIdx/*tMt.GetMtId()*/, tMt.GetConfIdx() );
	
			//������ػ��������ն���Ϣ
			//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
			//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
			SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
		
			// 2011-10-14 add by pgf:����ProcMtMcuOpenLogicChnnlReq������ʱ,�¼�MCU��δ���뵼��δ��VMP,�˴����벹�䴦��
			TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
			if ( g_cMcuVcApp.GetVMPMode(m_tModuleVmpEqp) != CONF_VMPMODE_NONE
				&& tConfAttrb.IsHasVmpModule() && m_tConfEqpModule.IsMtInVmp(tMt))
			{
				TPeriEqpStatus tPeriEqpStatus;
				g_cMcuVcApp.GetPeriEqpStatus(m_tModuleVmpEqp.GetEqpId() , &tPeriEqpStatus);
				TVMPParam_25Mem tParamInModule = GetVmpParam25MemFromModule();
				TVMPParam_25Mem tParamInEqpStatus = tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam();
				TVMPParam_25Mem tParamInConf = g_cMcuVcApp.GetConfVmpParam(m_tModuleVmpEqp);
				if ( !tParamInConf.IsMtInMember(tMt)
					&& tParamInModule.IsMtInMember(tMt))//conf��Ϣ��û�У�ģ������
				{
					if (tParamInEqpStatus.IsMtInMember(tMt))//����״̬���еĻ�����ʾ�¹��Σ�ģ����Ϣδ��
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "add smcu<%d %d> into VMP cause it is in module but not in conf now!\n",
							tMt.GetMcuIdx(), tMt.GetMtId());
						ChangeVmpParam(m_tModuleVmpEqp.GetEqpId(), &tParamInEqpStatus);//����һ�βΣ�ΪSetvmpchnl
					}
					else//����״̬��û�еĻ�����ʾ�ϳɳ�Ա��Ϣ�б仯�������ָ�
					{
						/* �˴�����ֱ����tParamInModule�����ܻ���ĺϳ���Ϣ�Ѹı䣬������Ϊһ���ն˽�������Ϣ�ָ���ģ���ʼ״̬
						// ����ģ��Ϊ�㲥��������ȡ���㲥���ն�����ֱ����ģ����Ϣ�����ǲ��Եġ�
						TVMPMember *ptConfMember = NULL;
						TVMPMember *ptModuleMember = NULL;
						BOOL32 bChgVmp = FALSE;
						for (u8 byIdx=0; byIdx<tParamInConf.GetMaxMemberNum() && byIdx<tParamInModule.GetMaxMemberNum(); byIdx++)
						{
							ptConfMember = tParamInConf.GetVmpMember(byIdx);
							ptModuleMember = tParamInModule.GetVmpMember(byIdx);
							if (NULL != ptModuleMember && !ptModuleMember->IsNull() && ptModuleMember->GetMtId() == tMt.GetMtId())
							{
								if (NULL != ptConfMember && ptConfMember->IsNull())
								{
									*ptConfMember = *ptModuleMember;
									bChgVmp = TRUE;
								}
							}
						}
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "add smcu<%d %d> into VMP cause it is in module but not in conf now!\n",
							tMt.GetMcuIdx(), tMt.GetMtId());
						if (bChgVmp)
						{
							ChangeVmpParam(m_tModuleVmpEqp.GetEqpId(), &tParamInConf);
						}*/
					}
				}
			}
			
			CServMsg cTmpMsg;
			//��������ȫ�������£������Զ������¼�mcu�Ļ�������			
            if( m_tConf.m_tStatus.IsMixing() )
			{                
			    TMcu tSMcu;			
			    tSMcu.SetMcuId( tMt.GetMtId() );
                cTmpMsg.SetMsgBody( (u8*)&tSMcu, sizeof(tSMcu) );
                if (m_tConf.m_tStatus.IsSpecMixing())
                {
                    // xsl [9/27/2006] ����һ���¼��������״̬��mcs����ˢ��
                    OnGetMixParamToSMcu(&cTmpMsg);
                }
                else //�����¼�mcu�Ļ�������
                {
                    u8 byDepth = MAXNUM_MIXER_DEPTH;
				    cTmpMsg.CatMsgBody( &byDepth, sizeof(byDepth) );
				    OnStartMixToSMcu(&cTmpMsg);
                }				
			}     
			
			if( VCS_CONF == m_tConf.GetConfSource() )
			{
				// [9/22/2011 liuxu] ֪ͨ�¼��Ƿ�����
				VCSMTMute(tMt, m_cVCSConfStatus.IsRemMute(), VCS_AUDPROC_MUTE);
				VCSMTMute(tMt, m_cVCSConfStatus.IsRemSilence(), VCS_AUDPROC_SILENCE);

				if (m_cVCSConfStatus.GetConfSpeakMode() == CONF_SPEAKMODE_ANSWERINSTANTLY )
				{
					u8 bySpeakMode = CONF_SPEAKMODE_ANSWERINSTANTLY;
					cTmpMsg.SetMsgBody( &bySpeakMode,sizeof(bySpeakMode) );
					cTmpMsg.SetEventId( MCU_MCU_SPEAKERMODE_NOTIFY );
					SendMsgToMt( tMt.GetMtId(),MCU_MCU_SPEAKERMODE_NOTIFY,cTmpMsg );
				}
			}	
		}

		BOOL32 bIsMultiCastMt = IsMultiCastMt(tMt.GetMtId());
		
		// vrs��¼����ͨ���ɹ���ֱ�ӽ�������������������������
		if (tMt.GetMtType() == MT_TYPE_VRSREC)
		{
			TRecChnnlStatus tVrsChlStatus = m_ptMtTable->GetRecChnnlStatus(tMt.GetMtId());
			// ״̬Ϊ����ʱ���������´�����ִֹ�ж��
			if (tVrsChlStatus.m_byType == TRecChnnlStatus::TYPE_RECORD)
			{
				BOOL32 bStartVrsRec = TRUE;
				if (tVrsChlStatus.m_byState != TRecChnnlStatus::STATE_CALLING)
				{
					// �Ǻ���״̬���룬��������������ֻ֤����һ��
					bStartVrsRec = FALSE;
				}
				TLogicalChannel tTmpLogicChanl;
				if (!m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_AUDIO, &tTmpLogicChanl, TRUE ))
				{
					bStartVrsRec = FALSE;
				}
				if (!m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tTmpLogicChanl, TRUE ))
				{
					bStartVrsRec = FALSE;
				}
				if (!m_tDoubleStreamSrc.IsNull())
				{
					if (m_ptMtTable->GetRecSrc(tMt.GetMtId()).IsNull())
					{
						// ����¼��
						if (!m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tTmpLogicChanl, TRUE ))
						{
							bStartVrsRec = FALSE;
						}
					}
					else
					{
						if (GetLocalMtFromOtherMcuMt(m_tDoubleStreamSrc) == GetLocalMtFromOtherMcuMt(m_ptMtTable->GetRecSrc(tMt.GetMtId())))
						{
							// �ն�¼��
							if (!m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tTmpLogicChanl, TRUE ))
							{
								bStartVrsRec = FALSE;
							}
						}
					}
				}
				if (bStartVrsRec)
				{
					// ¼��ͨ��
					TStartRecMsgInfo tStartVrsMsgInfo;
					if (PrepareStartRecMsgInfo(tMt, tStartVrsMsgInfo))
					{
						StartVrsRec(tStartVrsMsgInfo, TRecChnnlStatus::STATE_RECREADY);
					}
					else
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMtMcuOpenLogicChnnlRsp] PrepareStartRecMsgInfo error, Cann't start vrs recorder!\n");
					}
				}
			}
			else if (tVrsChlStatus.m_byType == TRecChnnlStatus::TYPE_PLAY)
			{
				// ����ͨ����mcu�������������vrs��¼��ǰ��ͨ��
				return;
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMtMcuOpenLogicChnnlRsp] tMt(%d,%d) RecChnnlStatus.%d is invalid, Cann't start vrs recorder!\n",
					tMt.GetMcuIdx(), tMt.GetMtId(), tVrsChlStatus.m_byType);
				ReleaseVrsMt(tMt.GetMtId());//�ͷŸ�vrsʵ��
				return;
			}
		}

		//��Ƶ������ʩ
		if( tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO )
		{
            //zbq[04/22/2009] ���� ����ͨ����̬ˢ����Ⱥ��λ��
//            RefreshRcvGrp(tMt.GetMtId());

			//20110913 zjl �ȴ��ն˵�һ·ͨ���򿪺��ٳ��Դ���ڶ�·˫��ͨ��
			if ((!m_tDoubleStreamSrc.IsNull() || !GetSecVidBrdSrc().IsNull()) &&
				!m_ptMtTable->IsLogicChnnlOpen(tMt.GetMtId(), LOGCHL_SECVIDEO, TRUE) &&
				m_tConf.GetBitRate() >= 64)
			{
				TLogicalChannel tDVLogicalChnnl;
				BOOL32 bOpenSec = FALSE;
				if ( TYPE_MCUPERI == m_tDoubleStreamSrc.GetType() &&
					EQP_TYPE_RECORDER == m_tDoubleStreamSrc.GetEqpType() )
				{
					tDVLogicalChnnl.SetChannelType( m_tPlayEqpAttrib.GetDVideoType() );
					tDVLogicalChnnl.SetSupportH239( m_tConf.GetCapSupport().IsDStreamSupportH239() );
					u16 wFileDSW = 0; 
					u16 wFileDSH = 0;
					u8  byFileDSType = 0;
					m_tPlayFileMediaInfo.GetDVideo(byFileDSType, wFileDSW, wFileDSH);
					u8 byFileDSRes = GetResByWH(wFileDSW, wFileDSH);
					tDVLogicalChnnl.SetVideoFormat(byFileDSRes);
					bOpenSec = TRUE;
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp:Rec] Try to Open Mt.%d DVchn after MVChn opened!\n", tMt.GetMtId());
				}
				else if(m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tDVLogicalChnnl, FALSE))
				{
					bOpenSec = TRUE;
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp:MT] Try to Open Mt.%d DVchn after MVChn opened!\n", tMt.GetMtId());
				}
				else if(m_ptMtTable->GetMtLogicChnnl(GetSecVidBrdSrc().GetMtId(), LOGCHL_VIDEO, &tDVLogicalChnnl, FALSE))
				{
					bOpenSec = TRUE;
					//����ͨ���Ƿ����H239��ǩ
					TCapSupport tCapSupport;
					if (m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tCapSupport))
					{
						tDVLogicalChnnl.SetSupportH239(tCapSupport.IsDStreamSupportH239());
					}
					else
					{
						tDVLogicalChnnl.SetSupportH239(m_tConf.GetCapSupport().IsDStreamSupportH239());
					}
					
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp:MT] Try to Open Mt.%d DVchn after MVChn opened!\n", tMt.GetMtId());
				}
				else
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp] GetMtLogicChnnl fail!\n");
				}	
				// vrs��¼��֧��,���ֻ���¼�����ն�¼��
				if (bOpenSec)
				{
					if (tMt.GetMtType() == MT_TYPE_VRSREC && m_ptMtTable->GetRecChlType(tMt.GetMtId()) == TRecChnnlStatus::TYPE_RECORD)
					{
						if (m_ptMtTable->GetRecSrc(tMt.GetMtId()).IsNull())
						{
							TVideoStreamCap tDSCap;
							if (m_cRecAdaptMgr.GetRecVideoCapSet(MODE_SECVIDEO, tDSCap))
							{
								tDVLogicalChnnl.SetChannelType(tDSCap.GetMediaType());
								tDVLogicalChnnl.SetVideoFormat(tDSCap.GetResolution());
							}
						}
						else
						{
							//�ն�¼����˫��Դ�Ƿ���¼���ն�
							if (!(m_tDoubleStreamSrc == m_ptMtTable->GetRecSrc(tMt.GetMtId())))
							{
								bOpenSec = FALSE;
							}
						}
					}
				}
				if (bOpenSec)
				{
					McuMtOpenDoubleStreamChnnl(tMt, tDVLogicalChnnl);	
				}							 			
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp] bSecSrcIsNull.%d, bDstMTDVOpened.%d, ConfBR.%d\n",
					m_tDoubleStreamSrc.IsNull(),
					m_ptMtTable->IsLogicChnnlOpen(tMt.GetMtId(), LOGCHL_SECVIDEO, TRUE),
					m_tConf.GetBitRate());
			}
			
			// vrs��¼�����ڴ˴�������
			if (tMt.GetMtType() == MT_TYPE_VRSREC)
			{
				return;
			}

            // xsl [7/21/2006] ��������鲥��ַ����
            /*if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {                
                ChangeSatDConfMtRcvAddr(tMt.GetMtId(), LOGCHL_VIDEO);
            }*/
			if (m_tConf.GetConfAttrb().IsUseAdapter()
				&& ( m_ptMtTable->GetMtType(tMt.GetMtId() ) == MT_TYPE_SMCU || 
						!(GetLocalVidBrdSrc() == tMt) )
				)
			{
				//�Ƿ���Ҫ����
				if (IsNeedAdapt(GetLocalVidBrdSrc(), tMt, MODE_VIDEO))
				{
					// [2013/01/31 chenbing] ��ǰ�����ն�Ϊ�����ն˲�������
					// ��ǰ�����ն�Ϊ�������ն˹�����
					if (m_tConf.m_tStatus.IsVidAdapting())
					{
                        if(!bIsMultiCastMt )
                        {
                            //1.������ˢ����
                            RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_VIDEO);
						    StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_VIDEO);
                        }
					}		
					else
					{
						StartBrdAdapt(MODE_VIDEO);
					}
				}
			}

			if( !tLocalVidBrdSrc.IsNull() )
			{
				// xliang [3/19/2009] �㲥Դ������VMP������ͨ����
				BOOL32 bNewVmpBrd = FALSE;
				if(tLocalVidBrdSrc == m_tVmpEqp)
				{
					// ��ͨ�ն���Ҫ�����ʣ������ն�����Ҫ [pengguofeng 1/14/2013]
					//���������յ������նˣ�Ҳ��Ҫ��VMP������
					if ( !bIsMultiCastMt
						|| IsCanSatMtReceiveBrdSrc(tMt))
					{
						TPeriEqpStatus tPeriEqpStatus; 
						g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
						u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
						if(byVmpSubType == VMP) 
						{
							AdjustVmpBrdBitRate(&tMt);
						}
						else
						{
							//8000E-vmp Ҳ����ͨ����
							bNewVmpBrd = TRUE;
							AdjustVmpBrdBitRate();//songkun,20110527,���е����նˣ�VMP�㲥������
						}
					}
				}
				if( bNewVmpBrd &&
					(	VCS_CONF != m_tConf.GetConfSource() ||					
						( ( m_byCreateBy == CONF_CREATE_MT ||
							!(tMt == m_tConf.GetChairman() ) ||
							m_cVCSConfStatus.GetCurVCMT().IsNull() ||							
							( VCS_GROUPROLLCALL_MODE != m_cVCSConfStatus.GetCurVCMode() && 
							  VCS_GROUPCHAIRMAN_MODE != m_cVCSConfStatus.GetCurVCMode() )	
							  // 2011-9-28 comment by pgf : ����������ѯ�ҹ㲥Դ��VMPʱ����ʱ�����ն���ᣬӦ�ô�VMP�Ľ���������������������Ĺ㲥
							) /*&& VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState()*/ ) 
						)
					)
				{
					// �������鲥���ս����������ն���ȥ�鲥��ַ [pengguofeng 1/14/2013]
					//���յ������ն�Ҳ�߽�������·��
					if ( !bIsMultiCastMt 
						|| IsCanSatMtReceiveBrdSrc(tMt))
					{
						SwitchNewVmpToSingleMt(tMt);
					}
				}
				else
				{
					if (CanMtRecvVideo(tMt, tLocalVidBrdSrc))
					{
						//��Ӷ����ش��ն�
						TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
						if( tConfAttrb.IsResendLosePack())
						{
							u8 byPrsId = 0;
							u8 byPrsChnId = 0;
							if (!(tMt == m_tCascadeMMCU) &&
								FindPrsChnForBrd(MODE_VIDEO, byPrsId, byPrsChnId) &&
								IsPrsChnStart(byPrsId, byPrsChnId))
							{
								AddRemovePrsMember(tMt.GetMtId(), byPrsId, byPrsChnId, MODE_VIDEO, TRUE);
							}
						}
					}

					bySrcChnnl = (tLocalVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
                    //���ǻ����з��鲥�ն˽�����
					if ( !bIsMultiCastMt
						|| IsCanSatMtReceiveBrdSrc(tMt))
					{
						// zbq[12/18/2008] ֱ���߹㲥�������������.Ϊʲô�����޸ģ����ӹ���֮.
						// �й㲥ԴӦ�߹㲥����, zgc, 2008-03-06
						//StartSwitchToSubMt( m_tVidBrdSrc, bySrcChnnl, tMt.GetMtId(), MODE_VIDEO );
						// [pengjie 2010/4/15] ��ش�֧�֣������˺ͻ���ϳɹ㲥����
						if (!IsNeedAdapt(GetLocalVidBrdSrc(), tMt, MODE_VIDEO))
						{
							tSpeaker = m_tConf.GetSpeaker();
							u16 wSpyPort = SPY_CHANNL_NULL;
							CRecvSpy tSpyResource;	
							if( !(tLocalVidBrdSrc == m_tPlayEqp) && !tSpeaker.IsLocal() &&
								m_cSMcuSpyMana.GetRecvSpy( tSpeaker, tSpyResource )
								)
							{
								wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
							}
							if( !tSpeaker.IsNull() && !(tSpeaker == tMt) &&
								!(tLocalVidBrdSrc.GetType() == TYPE_MCUPERI && tLocalVidBrdSrc.GetEqpType() == EQP_TYPE_BAS) )
							{
								StartSwitchFromBrd( tSpeaker, bySrcChnnl, 1, &tMt,wSpyPort );
							}
							else
							{
								StartSwitchFromBrd( tLocalVidBrdSrc, bySrcChnnl, 1, &tMt );
							}
						}
                        else
                        {
                            NotifyMtReceive(tLocalVidBrdSrc, tMt.GetMtId());
                        }
					}
					else
					{
						if( !bIsMultiCastMt )
						{
							NotifyMtReceive( tLocalVidBrdSrc, tMt.GetMtId() );
						}
						else
						{
							NotifyMtReceive( tMt, tMt.GetMtId() );
						}					
					}					
				}
				//�����ϯ�ն˵��������ߣ���Ƶ�����ﴦ��
				if( VCS_CONF == m_tConf.GetConfSource() &&
					tMt == m_tConf.GetChairman() &&
					( VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode() || 
						VCS_GROUPCHAIRMAN_MODE == m_cVCSConfStatus.GetCurVCMode() ) &&
						m_byCreateBy != CONF_CREATE_MT &&
						VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState() &&
						!m_cVCSConfStatus.GetCurVCMT().IsNull()
					)
				{
					TSwitchInfo tSwitchInfo;							
					tSwitchInfo.SetSrcMt(m_cVCSConfStatus.GetCurVCMT());
					tSwitchInfo.SetDstMt(m_tConf.GetChairman());
					tSwitchInfo.SetMode(MODE_VIDEO);							
					//��ϯѡ����ǰ�����ն�					
					VCSConfSelMT(tSwitchInfo);												
				}
			}
			else
			{
				//֪ͨ�ն��տ�����
				cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
				SendMsgToMt( tMt.GetMtId(), MCU_MT_YOUARESEEING_NOTIF, cServMsg );
				//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
				//�Լ�����Ƶ�������Լ�
				//StartSwitchToSubMt( tMt, 0, tMt.GetMtId(), MODE_VIDEO );
				if( m_ptMtTable->GetManuId( tMt.GetMtId() ) != MT_MANU_KDC )
				{
					TSwitchGrp tSwitchGrp;
					tSwitchGrp.SetSrcChnl(0);
					tSwitchGrp.SetDstMtNum(1);
					tSwitchGrp.SetDstMt(&tMt);
					StartSwitchToAll(tMt, 1, &tSwitchGrp, MODE_VIDEO);
				}

				//����VCS���鵱ǰ�����ն˿���ϯ
				if( VCS_CONF == m_tConf.GetConfSource() &&
					!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) )
				{
					GoOnSelStep(tMt, MODE_VIDEO, TRUE); 
				}
				if( !m_tDoubleStreamSrc.IsNull() && 
					m_ptMtTable->GetManuId( tMt.GetMtId() ) != MT_MANU_KDC &&
					m_ptMtTable->GetSrcSCS(tMt.GetMtId()).GetVideoMediaType() != MEDIA_TYPE_NULL )
				{
					TLogicalChannel tMvLogicalChannel;
					if (m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tMvLogicalChannel, FALSE) &&
						tMvLogicalChannel.GetFlowControl() > 0)
					{
						u16 wBitrate = 0;
						const u16 wSendBand = m_ptMtTable->GetSndBandWidth(tMt.GetMtId());
						const u16 wRcvBand = m_ptMtTable->GetRcvBandWidth(tMt.GetMtId());
						wBitrate = min (wSendBand ,  wRcvBand);
						wBitrate = GetDoubleStreamVideoBitrate(wBitrate);
						tMvLogicalChannel.SetFlowControl(wBitrate);
						cServMsg.SetMsgBody((u8*)&tMvLogicalChannel, sizeof(tMvLogicalChannel));
						SendMsgToMt(tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
					}
				}
			}
	
			// ʹ���µķ�����ѡ������, zgc, 2008-03-06
			if( HasJoinedSpeaker() && 
				!g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) && 
				!( m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO ) &&
				( tMt == GetLocalSpeaker() || tMt == m_tConf.GetChairman() || tMt == m_tLastSpeaker ) )
			{
				// �·���������Դ�����߼�, zgc, 2008-04-12
				ChangeSpeakerSrc(MODE_VIDEO, emReasonChangeSpeaker);
			}
			
			//E1 MCU��Ҫ����Դ��ǰ��ͨ������[10/25/2012 chendaiwei]
			// 			TDri2E1Cfg tDri2E1Cfg[MAXNUM_SUB_MCU];
			// 			g_cMcuVcApp.GetDri2E1CfgTable(tDri2E1Cfg);
			// 			u16 wRealBandwidth = 0;
			// 			for ( u8 byCfgIndx = 0; byCfgIndx < MAXNUM_SUB_MCU; byCfgIndx++)
			// 			{
			// 				u32 dwConIpAddr = tDri2E1Cfg[byCfgIndx].GetIpAddr();
			// 				//�ж�MMCU(SMCU)�Ƿ��������ļ�������[10/25/2012 chendaiwei]
			// 				if( g_cMcuVcApp.GetRealBandwidthByIp(dwConIpAddr,wRealBandwidth)
			// 					&& dwConIpAddr == m_ptMtTable->GetIPAddr(tMt.GetMtId())
			// 					&& (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU
			// 					|| m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MMCU))
			// 				{
			// 					m_ptMtTable->SetMtTransE1(tMt.GetMtId(),TRUE);
			// 					
			// 					u16 wActualBR = wRealBandwidth - GetAudioBitrate(m_tConf.GetMainAudioMediaType());
			// 					m_ptMtTable->SetRcvBandWidth(tMt.GetMtId(),wActualBR);
			// 					
			// 					break;
			// 				}
			// 			}

			TDri2E1Cfg tDri2E1Cfg[MAXNUM_SUB_MCU];
			g_cMcuVcApp.GetDri2E1CfgTable(tDri2E1Cfg);
			u16 wRealBandwidth = 0;
			if(m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU
			  ||m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MMCU)
			{
				for ( u8 byCfgIndx = 0; byCfgIndx < MAXNUM_SUB_MCU; byCfgIndx++)
				{
					u32 dwConIpAddr = tDri2E1Cfg[byCfgIndx].GetIpAddr();
					//�ж�MMCU(SMCU)�Ƿ��������ļ�������[10/25/2012 chendaiwei]
					if( g_cMcuVcApp.GetRealBandwidthByIp(dwConIpAddr,wRealBandwidth)
						&& dwConIpAddr == m_ptMtTable->GetIPAddr(tMt.GetMtId()))
					{
						m_ptMtTable->SetMtTransE1(tMt.GetMtId(),TRUE);
						
						u16 wActualBR = wRealBandwidth - GetAudioBitrate(m_tConf.GetMainAudioMediaType());
						m_ptMtTable->SetRcvBandWidth(tMt.GetMtId(),wActualBR);
						
						if(m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MMCU)
						{
							continue;
						}
						
						u32 dwLocalDriIp = tDri2E1Cfg[byCfgIndx].GetDriIp();
						
						//��˿ڴ���
						u16 wBandWidthEachPort[MAXNUM_E1PORT];
						memset(wBandWidthEachPort, 0,  sizeof(wBandWidthEachPort));
						
						//��˿�����E1����
						u8 byPortE1Num[MAXNUM_E1PORT];
						memset(byPortE1Num, 0, sizeof(byPortE1Num));
						
						for(u8 byIdx = 0; byIdx < MAXNUM_SUB_MCU; byIdx++)
						{
							//�ҵ��͸�MMCU(SMCU)��ͬ��DRI2������������Ϣ[10/25/2012 chendaiwei]
							if(tDri2E1Cfg[byIdx].GetDriIp() == dwLocalDriIp)
							{
								u8 byPortIdx = tDri2E1Cfg[byIdx].GetPortNO();
								wBandWidthEachPort[byPortIdx] = tDri2E1Cfg[byIdx].GetRealBandWidth();
								byPortE1Num[byPortIdx] = tDri2E1Cfg[byIdx].GetCfgE1Num();
							}
						}
						
						dwLocalDriIp = htonl(dwLocalDriIp);
						u8 byBuf[sizeof(wBandWidthEachPort)+sizeof(byPortE1Num)+sizeof(u32)] = {0};
						memcpy(&byBuf[0],&wBandWidthEachPort[0],sizeof(wBandWidthEachPort));
						memcpy(&byBuf[sizeof(wBandWidthEachPort)],&byPortE1Num[0],sizeof(byPortE1Num));
						memcpy(&byBuf[sizeof(wBandWidthEachPort)+sizeof(byPortE1Num)],&dwLocalDriIp,sizeof(dwLocalDriIp));
						
						CMessage cMsg;
						cMsg.content = &byBuf[0];
						cMsg.length = sizeof(byBuf);
						ProcAgtSvgE1BandWidthNotif(&cMsg);
						
						ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuOpenLogicChnnlRsp]SMCU<%d> connected by E1, adjust bandwidth.\n",tMt.GetMtId());
						
						break;
					}
				}
			}

		}

		//˫������
        if (tLogicChnnl.GetMediaType() == MODE_SECVIDEO && byChannel == LOGCHL_SECVIDEO)
        {
			//�Ƿ�Ҫת��˫������
            TLogicalChannel tH239LogicalChannel;
            
			if (!m_tConf.GetConfAttrb().IsSatDCastMode() ||
                !bIsMultiCastMt )
            {
				TMt tSrc = m_ptMtTable->GetMtSelMtByMode(tMt.GetMtId(), MODE_VIDEO2SECOND);
				if (!tSrc.IsNull())					//˫ѡ��
				{
					if (IsNeedSelAdpt(tSrc, tMt, MODE_VIDEO2SECOND))
					{
						if (!StartSelAdapt(tSrc, tMt, MODE_VIDEO2SECOND))
						{
							RestoreRcvSecMediaBrdSrc(1, &tMt);
							m_ptMtTable->RemoveMtSelMtByMode(tMt.GetMtId(), MODE_VIDEO2SECOND);
						}
					}
					else
					{
						TSwitchGrp tSwitchGrp;
						tSwitchGrp.SetSrcChnl(0);
						tSwitchGrp.SetDstMtNum(1);
						tSwitchGrp.SetDstMt(&tMt);

						StartSwitchToAll(tSrc, 1, &tSwitchGrp, MODE_VIDEO2SECOND, SWITCH_MODE_SELECT);

						TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
						if (tConfAttrb.IsResendLosePack())
						{
							BuildRtcpSwitchForSrcToDst(tMt, tSrc, MODE_VIDEO2SECOND);
						}
						
						//�����������һ��Դ�����ʣ�����
						AdjustFitstSrcMtBit(tMt.GetMtId(), bIsBrdVidNeedAdp);
					}
				}
				else if (!m_tSecVidBrdSrc.IsNull()					//˫�㲥
					&& m_ptMtTable->GetMtLogicChnnl(GetSecVidBrdSrc().GetMtId(), LOGCHL_VIDEO, &tH239LogicalChannel, FALSE))
				{
					if (IsNeedAdapt(m_tSecVidBrdSrc, tMt, MODE_VIDEO2SECOND))
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[ProcMtMcuOpenLogicChnnlRsp]tDst(%d-%d),need adapt!\n", 
							tMt.GetMcuId(), tMt.GetMtId());
					}
					else
					{
						TSwitchGrp tSwitchGrp;
						tSwitchGrp.SetSrcChnl(0);
						tSwitchGrp.SetDstMtNum(1);
						tSwitchGrp.SetDstMt(&tMt);
						g_cMpManager.StartSwitchToAll(m_tSecVidBrdSrc, 1, &tSwitchGrp, MODE_VIDEO2SECOND);

						TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
						if (tConfAttrb.IsResendLosePack() && m_tSecVidBrdSrc.GetMtId() != tMt.GetMtId())
						{
							u8 byPrsId = 0;
							u8 byPrsChnId = 0;
							if (FindPrsChnForBrd(MODE_SECVIDEO, byPrsId, byPrsChnId) && IsPrsChnStart(byPrsId, byPrsChnId))
							{
								AddRemovePrsMember(tMt.GetMtId(), byPrsId, byPrsChnId, MODE_VIDEO2SECOND, TRUE);
							}
						}
					}
				}
				else if (TYPE_MCUPERI == m_tDoubleStreamSrc.GetType()		//˫������
					|| m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicalChannel, FALSE))
				{
					if (IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO))
					{
						if (m_tConf.m_tStatus.IsDSAdapting())
						{
							//1.������ˢ����
							RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_SECVIDEO);
							StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_SECVIDEO);
						}		
						else
						{
							StartBrdAdapt(MODE_SECVIDEO);
						}
					}
					else
					{						
						// zbq [07/26/2007] ¼�����˫�����������������ͨ��
						u8 byDSSrcChnnl = m_tDoubleStreamSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
						
						TSwitchGrp tSwitchGrp;
						tSwitchGrp.SetSrcChnl(byDSSrcChnnl);
						tSwitchGrp.SetDstMtNum(1);
						tSwitchGrp.SetDstMt(&tMt);
						
						g_cMpManager.StartSwitchToAll(m_tDoubleStreamSrc, 1, &tSwitchGrp, MODE_SECVIDEO);

						TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
						if (tConfAttrb.IsResendLosePack() && m_tSecVidBrdSrc.GetMtId() != tMt.GetMtId())
						{
							u8 byPrsId = 0;
							u8 byPrsChnId = 0;
							if (FindPrsChnForBrd(MODE_SECVIDEO, byPrsId, byPrsChnId) && IsPrsChnStart(byPrsId, byPrsChnId))
							{
								AddRemovePrsMember(tMt.GetMtId(), byPrsId, byPrsChnId, MODE_SECVIDEO, TRUE);
							}
						}
					}
				}
            }

            // xsl [7/21/2006] ��������鲥��ַ����
            if (m_tConf.GetConfAttrb().IsSatDCastMode() &&
                bIsMultiCastMt)
            {
				if ( !(tMt == m_tDoubleStreamSrc) )
				{
					ChangeSatDConfMtRcvAddr(tMt.GetMtId(), LOGCHL_SECVIDEO);
				}
            }
        }

		//��Ƶ������ʩ
		if( tLogicChnnl.GetMediaType() == MODE_AUDIO )
		{
            //����ն˵���Ƶ�����ش�
			TConfAttrb tAudioConfAttrb = m_tConf.GetConfAttrb();
			if (tAudioConfAttrb.IsResendLosePack())
			{
				u8 byPrsId = 0;
				u8 byPrsChnId = 0;
				if (!(tMt == m_tCascadeMMCU) &&
					FindPrsChnForBrd(MODE_AUDIO, byPrsId, byPrsChnId) &&
					IsPrsChnStart(byPrsId, byPrsChnId))
				{
					AddRemovePrsMember(tMt.GetMtId(), byPrsId, byPrsChnId, MODE_AUDIO, TRUE);
				}
			}

			//vrs��¼������Ƶ�߼�ͨ����������������
			if (tMt.GetMtType() == MT_TYPE_VRSREC)
			{
				return;
			}

            // xsl [7/21/2006] ��������鲥��ַ����
            if (m_tConf.GetConfAttrb().IsSatDCastMode() && 
                bIsMultiCastMt &&
				!GetAudBrdSrc().IsNull() &&
                !(tMt == GetAudBrdSrc()/*GetLocalSpeaker()*/))  // [1/24/2013 liaokang] ���˷����ˣ���ֹ���������Լ�
            {               
                ChangeSatDConfMtRcvAddr(tMt.GetMtId(), LOGCHL_AUDIO);
            }

			if( HasJoinedSpeaker() )
			{
				bySrcChnnl = (tLocalVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;

				if (m_tConf.m_tStatus.IsNoMixing() || m_tConf.m_tStatus.IsMustSeeSpeaker())
				{
                    // ���� [5/29/2006]
                    tSpeaker = m_tConf.GetSpeaker();
					if ( !bIsMultiCastMt
						 || IsCanSatMtReceiveBrdSrc(tMt, MODE_AUDIO))
                    {
						//tSpeaker = m_tConf.GetSpeaker();
						// ����Ҫ�����䣬����Ҫ��������������ʱ��ֱ�Ӵӹ㲥Դ����
						if(!IsNeedAdapt(tSpeaker, tMt, MODE_AUDIO) || m_tConf.m_tStatus.IsAudAdapting())
						{
							//StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_AUDIO);
							RestoreRcvMediaBrdSrc(tMt.GetMtId(),MODE_AUDIO);
						}		
						else
						{
							StartBrdAdapt(MODE_AUDIO);
						}
						/*if (IsNeedAdapt(tSpeaker, tMt, MODE_AUDIO))
						{
							if (m_tConf.m_tStatus.IsAudAdapting())
							{
								StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_AUDIO);
							}		
							else
							{
								StartBrdAdapt(MODE_AUDIO);
							}
						}
						else
						{
							u16 wSpyPort = SPY_CHANNL_NULL;
							CRecvSpy tSpyResource;	
							if( !(tLocalVidBrdSrc == m_tPlayEqp) && !tSpeaker.IsLocal() &&
								m_cSMcuSpyMana.GetRecvSpy( tSpeaker, tSpyResource )
								)
							{
								wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
							}
							//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
							//                        StartSwitchToSubMt( tSpeaker, bySrcChnnl, 
							//							tMt.GetMtId(), MODE_AUDIO,SWITCH_MODE_BROADCAST,TRUE,TRUE,FALSE,wSpyPort );
							TSwitchGrp tSwitchGrp;
							tSwitchGrp.SetSrcChnl(bySrcChnnl);
							tSwitchGrp.SetDstMtNum(1);
							tSwitchGrp.SetDstMt(&tMt);
							StartSwitchToAll(tSpeaker, 1, &tSwitchGrp, MODE_AUDIO, SWITCH_MODE_BROADCAST, TRUE, TRUE, wSpyPort);
						}*/
                    }
                    else
                    {
                        // ��������ǻ��飬���û�����������                        
                    }
					
				}
				// ʹ���µķ�����ѡ������, zgc, 2008-03-06
				if( m_tConf.m_tStatus.IsNoMixing() && 
					m_tConf.GetConfAttrb().GetSpeakerSrcMode() == MODE_BOTH &&
					( tMt == GetLocalSpeaker() || tMt == m_tConf.GetChairman() || tMt == m_tLastSpeaker ) )
				{
					//�·���������Դ�����߼���zgc, 2008-04-12
					ChangeSpeakerSrc(MODE_AUDIO, emReasonChangeSpeaker);
				}
			}
            
			TMtStatus tMtStatus;
			m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus);

			// xsl [8/4/2006] ���ƻ�����nģʽ�����������ն�(�����˳���)
			if( m_tConf.m_tStatus.IsMixing() &&
				// zjj20090911����VCS����������Զ�˾������򲻽����������������¼�MCU
				!((MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId())) 
				&& (/*(VCS_CONF == m_tConf.GetConfSource() && m_cVCSConfStatus.IsRemSilence())
				//lukp [02/03/2010]��VCS������������¼�mcu�Ҵ�mcu�Ǿ���,�������������¼�mcu�Ľ���
				||*/ ((VCS_CONF != m_tConf.GetConfSource()) && tMtStatus.IsDecoderMute()))))
		
			{
                if (m_tConf.m_tStatus.IsSpecMixing())
                {
                    if ( (tMt == GetLocalSpeaker() || m_ptMtTable->IsMtInMixing( tMt.GetMtId() ) )
						&& (GetMixMtNumInGrp() < GetMaxMixNum(m_tMixEqp.GetEqpId()) ||
						(GetMixMtNumInGrp()== GetMaxMixNum(m_tMixEqp.GetEqpId()) && m_ptMtTable->IsMtInMixGrp(tMt.GetMtId()))) 
						&& !(tMt == m_cVCSConfStatus.GetVcsBackupChairMan())   //zhouyiliang 20101213 ������ϯ��������
						)
                    {
                        // guzh [7/14/2007] ������ChangeSpeaker�ﱣ����������Ҫ����AddRemoveһ�Ρ�����InMixingͼ��᲻��
                        // ԭ��Ϊʲô AddMixMember��
                        //AddSpecMixMember(&tMt, 1, m_ptMtTable->IsMtAutoInSpec(tMt.GetMtId())); 
						AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE);
						StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
							(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt)),
								MODE_AUDIO, SWITCH_MODE_SELECT);
						m_ptMtTable->SetMtInMixing(tMt.GetMtId(),TRUE,m_ptMtTable->IsMtAutoInSpec(tMt.GetMtId()));
                    }
                    else
                    {
                        SwitchMixMember(&tMt, TRUE);
                    }   
                }
                else 
                {   
					TLogicalChannel tLog;
					if ( m_tConf.m_tStatus.IsAutoMixing() && 
						 !m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tLog, FALSE) )
					{
						// ��Ȼ�ն����ߺ�ֻ��N����
						SwitchMixMember(&tMt, TRUE);
					}
					// miaoqingsong [05/11/2011] ��������ģʽ�����ն���Ŀ��������������ʱ�������л�Ϊ���ƻ����߼���
					// ͬʱ����"��ӻ�����Ա�Ƿ�ɹ�"���жϣ���ֹ����һЩ"���ն˵�������"����Ч����
                    if (GetMixMtNumInGrp() < GetMaxMixNum(m_tMixEqp.GetEqpId()) ||
						(GetMixMtNumInGrp()== GetMaxMixNum(m_tMixEqp.GetEqpId()) && 
						m_ptMtTable->IsMtInMixGrp(tMt.GetMtId())))
                    {
						
						if(  m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tLog, FALSE) &&
							AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE) )
						{
							StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
								(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt)),
								MODE_AUDIO, SWITCH_MODE_SELECT);
						}
                    }
                    else
					{
// 						if (m_tConf.m_tStatus.GetMixerMode() != mcuVacMix)
// 						{
// 							//�������л�Ϊ����ͳһ���ն˵ĺ������߼�ͨ��ʱ����
// 							SwitchDiscuss2SpecMix();
// 							u16 wError = ERR_MCU_DISCUSSAUTOCHANGE2SPECMIX;
// 							NotifyMcsAlarmInfo(0, wError);   // ���ô�����֪ͨMCS
// 						}
						if (m_tConf.m_tStatus.GetMixerMode() == mcuVacMix)
						{
							StopMixing();
						}
					}
                }
            }
			else
			{
				// ����VCS����ǻ���ģʽ�£��ն�����ϯ
				if( VCS_CONF == m_tConf.GetConfSource() &&
					!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) )
				{
					GoOnSelStep(tMt, MODE_AUDIO, TRUE); 
				}

				//lukunpeng[03/05/2010]����û���յ��߼�ͨ���򿪻ظ�ǰ����ӻ�����Ա����ʧ�ܵ�
				//��Ҫ�ڴ˴������һ��
				if (m_tConf.m_tStatus.IsVACing())
				{
                    if (GetMixMtNumInGrp() < GetMaxMixNum(m_tMixEqp.GetEqpId()))
                    {
                        AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE);
                    }  
				}
			}

			//�����ϯ�ն˵��������ߣ���Ƶ�����ﴦ��
			if( VCS_CONF == m_tConf.GetConfSource() )
			{			
				if(	m_byCreateBy != CONF_CREATE_MT &&				
					ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() ) &&
					tMt == m_tConf.GetChairman() &&
					!m_tConf.m_tStatus.IsMixing() && 
					m_tMixEqp.IsNull()
				 )	
				{
					m_ptMtTable->SetMtInMixing( tMt.GetMtId(),TRUE, TRUE );
					m_ptMtTable->SetMtInMixing( GetLocalSpeaker().GetMtId(),TRUE, TRUE );
					//tianzhiyong 2010/03/21  ���ӿ���ģʽ����
					BOOL32 dwStartResult = StartMixing(mcuPartMix);							
					if (!dwStartResult)
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcMtMcuOpenLogicChnnlRsp] Find no mixer\n");	
						cServMsg.SetMsgBody();
						cServMsg.SetErrorCode(ERR_MCU_VCS_NOMIXSPEAKER);					
						SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cServMsg);
					}				
				}
				if( m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MT &&
					m_tConf.GetChairman().GetMtId() != tMt.GetMtId() )
				{
					VCSMTMute(tMt, m_cVCSConfStatus.IsRemMute(), VCS_AUDPROC_MUTE);					
					VCSMTMute(tMt, m_cVCSConfStatus.IsRemSilence(), VCS_AUDPROC_SILENCE);
				}
			}
		}

		if (!m_tCascadeMMCU.IsNull() &&
			m_tConfAllMtInfo.MtJoinedConf(m_tCascadeMMCU.GetMtId()) &&
			(tLogicChnnl.GetMediaType() == MODE_VIDEO ||
			 tLogicChnnl.GetMediaType() == MODE_AUDIO))
		{
			OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(), tMt.GetMtId());
		}
        
		break;
    }		
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	
	return;
}


/*====================================================================
    ������      ��ProcMtMcuMsdRsp
    ����        �����Ӿ���֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/10/09    2.5         ������         ����
====================================================================*/
void CMcuVcInst::ProcMtMcuMsdRsp( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() ); 	     
}

/*====================================================================
    ������      ��ProcMtMcuOpenLogicChnnlReq
    ����        ���ն˷����Ĵ��߼�ͨ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/01/23    1.0         LI Yi         ����
	04/02/12    3.0         ������        ������
	05/12/19	4.0			�ű���		  T120�����߼�ͨ������
	06/03/06	4.0			�ű���		  �ж�˫������Դ
====================================================================*/
void CMcuVcInst::ProcMtMcuOpenLogicChnnlReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt;
	u32	dwRcvIp;
	u16	wRcvPort;
	u8  byChannel = 0;
    u8  byAudioType;

    //TCapSupportEx tCapSupportEx;
	TLogicalChannel	tLogicChnnl;

	switch( CurState() )
	{
	case STATE_ONGOING:
        {
		tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );
		
		//�������߼�ͨ��������������ʱ�����(Ŀǰ��ֻ����˫��)
        if (MT_MCU_LOGICCHNNLOPENED_NTF == pcMsg->event)
        {
            if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		    {
                //sony G70SP��H263-H239ʱ���û˫��,�Ҳ�ʹ�����ƶ�ʹ��FlowControlIndication���з��ͼ����ر� ����
                //POLYCOM7000�ľ�̬˫��ʱ��ʹ�����ƽ��з��ͼ����ر�
                //���ں��н���ʱ����,֮������ڼ䱣��ͨ����ֱ���˳�����
                if( ( MT_MANU_SONY == m_ptMtTable->GetManuId(tMt.GetMtId()) && 
                      MEDIA_TYPE_H263 == m_tConf.GetCapSupport().GetDStreamMediaType() &&
                      m_tConf.GetCapSupport().IsDStreamSupportH239()) || 
                      (MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
					  MT_TYPE_MT == m_ptMtTable->GetMtType(tMt.GetMtId()) ) ||
                     MT_MANU_CHAORAN == m_ptMtTable->GetManuId(tMt.GetMtId()))
                {
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq]Special Manu(%d) Mt(%d)!!!\n",m_ptMtTable->GetManuId(tMt.GetMtId()), tMt.GetMtId());
                }
                else
                {                
                    //�ж�˫���ķ���Դ// xliang [10/10/2009] 
					if (CheckDualMode(tMt))
                    {       
						//��IsMMcuSpeaker�ж���Ч�Ա����False��ֻ��ÿ�α���������ĵ�һ���ж���Ч
						if( m_bMMcuSpeakerValid && tMt.GetMtType() == MT_TYPE_MMCU )
						{
							m_bMMcuSpeakerValid = FALSE;
						}

                        // xliang [11/14/2008]  polycomMCU ��˫��ͨ������������˫����������flowctrl������Ӧ����
						if ( (MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) ||
							MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId()) )
							&&
							( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
							MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId())) )
						{
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] PolyMcu or AethraMcu.%d open DVideo req here\n", tMt.GetMtId());
						}
						else
						{
							//UpdateH239TokenOwnerInfo( tMt );
							StartDoubleStream( tMt, tLogicChnnl );
                            ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Chn connected ntf rcved, start ds!\n");
						}
                    }
                    else
                    {
                        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Chn connected ntf rcved, no start ds due to DualMode.%d, tMt.%d\n",
                            m_tConf.GetConfAttrb().GetDualMode(), tMt.GetMtId());
                    }
    			}
            }
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq]MT_MCU_LOGICCHNNLOPENED_NTF BUT NOT MODE_SECVIDEO,DO NOTHING!\n");
			}
            return;
        }

		// �������ն˿�ʼ�����MCUͨ��ʱ����Ϊ�õ����ն˵��ȳɹ����ɽ�����һ���ն˵ĵ���
		if ( VCS_CONF == m_tConf.GetConfSource() &&
			tMt == m_cVCSConfStatus.GetReqVCMT() &&
			!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) 
			)
		{		 	
			// �˴�ֻ��Ե�������ģʽ����,�෽�໭���ͨ����ȫ����������
			if (VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode())
			{
				KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
				//yrl20131112 ���������£��ն˽�����ǽ������VCS���ȸ��նˣ�����Ӳ���(�Ƿ�ٽ�ǽ)����ʱ��Ϊ�ն˷Ǽٽ�ǽ
				if (!IsMtNotInOtherHduChnnl(tMt,0,0,FALSE,FALSE) || !IsMtNotInOtherTvWallChnnl(tMt,0,0,FALSE,FALSE))
				{
					TMt tMtNull;
					tMtNull.SetNull();
					m_cVCSConfStatus.SetReqVCMT( tMtNull );
				}
				else
				{
					ChgCurVCMT(tMt);
				}
			}
			/*if( VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
				(!IsMtNotInOtherHduChnnl(tMt,0,0) || !IsMtNotInOtherTvWallChnnl(tMt,0,0)) )
			{
				TMt tMtNull;
				tMtNull.SetNull();
				m_cVCSConfStatus.SetReqVCMT( tMtNull );
				KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
			}
			else
			{
				// Timer�ڴ˴���ֱ��kill��,�ǿƴ��ն˽��෽�໭�������������Ӧ����
				KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
				if( VCS_MULVMP_MODE != m_cVCSConfStatus.GetCurVCMode()  ||
					IsKedaMt(tMt, TRUE) )
				{
					//KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
					ChgCurVCMT(tMt);
				}	
			}	*/		
		}
		
		
        //����Ϊ�������߼�ͨ��ACK/NACK����
		if(tLogicChnnl.GetMediaType() == MODE_VIDEO)
		{
            m_ptMtTable->SetMtVideoSend(tMt.GetMtId(), FALSE);

            //  xsl [3/17/2006] 263+��ʽ������ݶ�̬�غɹ���
			if (!m_tConf.GetCapSupport().IsSupportMediaType(tLogicChnnl.GetChannelType())
				// [091027]������Ժ֧�ֵ�һ·H263+����
				/*&& MEDIA_TYPE_H263PLUS != tLogicChnnl.GetChannelType()*/)
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) not support video(%s), nack!\n",
					tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , GetMediaStr(tLogicChnnl.GetChannelType()));
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
            // ����@2006.4.13 ���<64K���ʣ���ܾ����߼�ͨ��
            if (m_tConf.GetBitRate() < 64)
            {
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Bitrate less than 64K, open video logic chnl nack!\n");
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
            }
		}
		if(tLogicChnnl.GetMediaType() == MODE_AUDIO)
		{
            m_ptMtTable->SetMtAudioSend(tMt.GetMtId(), FALSE);

			//if(!m_tConf.GetCapSupport().IsSupportMediaType(tLogicChnnl.GetChannelType()))
			TAudioTypeDesc tAudioTypeDesc(tLogicChnnl.GetChannelType(),tLogicChnnl.GetAudioTrackNum());
			if(!m_tConfEx.IsSupportAudioMediaType(tAudioTypeDesc))
			{                                                                                                                         
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) not support audio(%s,%d), nack!\n",
					tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , 
					GetMediaStr(tLogicChnnl.GetChannelType()),tLogicChnnl.GetAudioTrackNum());
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}			
		}
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
			if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_NULL
				|| (m_tConf.GetDStreamMediaType() != tLogicChnnl.GetChannelType()
				    && m_tConf.GetCapSupportEx().GetSecDSType() != tLogicChnnl.GetChannelType()))
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq]Src:MT%d MediaType: %u, unmatched with conf, refuse!\n",tMt.GetMtId(),tLogicChnnl.GetMediaType());
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );

                return;
			}

            m_ptMtTable->SetMtVideo2Send(tMt.GetMtId(), FALSE);
			
			// xliang [12/18/2008]  ˫��ͨ���ֱ���ƥ��У��
			u8 bySrcRes = tLogicChnnl.GetVideoFormat();
			u8 byDstRes = m_tConf.GetCapSupport().GetDStreamCapSet().GetResolution();
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq]Src:MT%d MediaType: %u, Src Res: %u, Fps:%u\n", 
                tMt.GetMtId(), 
                tLogicChnnl.GetChannelType(), 
                bySrcRes, 
                tLogicChnnl.GetChanVidFPS());
			if (!IsDSResMatched(bySrcRes, byDstRes))
			{
                if (MT_MANU_TAIDE == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
                    VIDEO_FORMAT_CIF == bySrcRes)
                {
                    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Mt.%d's DS Format.%d adjust to Format.%d due to Tand!\n",
                        tMt.GetMtId(), bySrcRes, byDstRes);
                    tLogicChnnl.SetVideoFormat(byDstRes);
                }
                else
                {
                    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Mt.%d's DS Res dismatched<src.%d, dst.%d>, but still go on",
                              tMt.GetMtId(), bySrcRes, byDstRes);
                }

// 				if (MT_MANU_KDC == m_ptMtTable->GetManuId(tMt.GetMtId()))
// 				{
// 					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] keda DsSrc'Res.%d dismatched with Conf DsRes.%d, but go on!\n",
// 								bySrcRes, byDstRes);
// 				}
// 				else if (MT_MANU_TAIDE == m_ptMtTable->GetManuId(tMt.GetMtId()))
//                 {
//                     if (VIDEO_FORMAT_CIF == bySrcRes)
//                     {
//                         ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Mt.%d's DS Format.%d adjust to Format.%d due to Tand!\n",
//                             tMt.GetMtId(), bySrcRes, byDstRes);
//                         tLogicChnnl.SetVideoFormat(byDstRes);
//                     }
//                 }
// 				// cheat 1: let it go
// 				else if(MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId()) ||
// 					    MT_MANU_CODIAN == m_ptMtTable->GetManuId(tMt.GetMtId()))
// 				{
// 					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Mt.%d's DS Res dismatched<src.%d, dst.%d>, but still go on due to Eathra or codian!\n",
//                             tMt.GetMtId(), bySrcRes, byDstRes);
// 					
// 					// cheat 2: let src = dst
// 					
// 					if (VIDEO_FORMAT_CIF == bySrcRes)
//                     {
//                         ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Mt.%d's DS Format.%d adjust to Format.%d due to Tand!\n",
//                             tMt.GetMtId(), bySrcRes, byDstRes);
//                         tLogicChnnl.SetVideoFormat(byDstRes);
//                     }
					

// 				}
//[5/4/2011 zhushengze]VCS���Ʒ����˷�˫��
// 				else
// 				{
// 					ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Mt.%d's DS res dismatched<src.%d, dst.%d>, ignore it\n", 
// 						tMt.GetMtId(), bySrcRes, byDstRes);
// 					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
// 					return;
// 				}
			}
            
            //[10/19/2011 zhushengze]ͬ����Ƶʱ���ж�239�Ƿ�ƥ�䣬ͬ����Ƶʱͬʱ֧��239�ͷ�239
			//ͬ����Ƶ��Ҫ�ж�239�Ƿ�ƥ��[4/13/2012 chendaiwei]
            if( ( /*m_tConf.GetCapSupport().GetDStreamType() != VIDEO_DSTREAM_MAIN && */
                 tLogicChnnl.IsSupportH239() != m_tConf.GetCapSupport().IsDStreamSupportH239()) ||
                (tLogicChnnl.GetChannelType() != m_tConf.GetCapSupport().GetDStreamMediaType() &&
               //zbq[01/04/2009] ˫˫�����˷ſ�
                tLogicChnnl.GetChannelType() != m_tConf.GetCapSupportEx().GetSecDSType()))
            {   
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CALL, "MT%d:H239(%d),ChanType(%s) not match with conf, NACK\n",
                          tMt.GetMtId(), tLogicChnnl.IsSupportH239(), GetMediaStr(tLogicChnnl.GetChannelType()));
//                 ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) not support second video (%s), nack!\n",
//                     tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , GetMediaStr(tLogicChnnl.GetChannelType()));
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
            // ����@2006.4.13 ���<64K���ʣ���ܾ����߼�ͨ��
            if (m_tConf.GetBitRate() < 64)
            {
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Bitrate less than 64K, open sec video logic chnl, nack!\n");
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
            }            			
		}

		//�����������, ֱ�Ӿݵ��Զ�T120ͨ��������
		if ( MODE_DATA == tLogicChnnl.GetMediaType() )
		{
			if ( MEDIA_TYPE_T120 == tLogicChnnl.GetChannelType() )
			{
				if ( !m_ptMtTable->IsMtIsMaster( tMt.GetMtId() ) )
				{	
					ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Mt%d open mcu T.120 logicchannel as slave, nack it directly!\n", tMt.GetMtId() );
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					return;
                }
			}
		}
		
		//��������
		if(tLogicChnnl.GetMediaEncrypt().GetEncryptMode()!= m_tConf.GetConfAttrb().GetEncryptMode())
		{
			if( tLogicChnnl.GetMediaType() == MODE_VIDEO || 
				tLogicChnnl.GetMediaType() == MODE_SECVIDEO || 
				tLogicChnnl.GetMediaType() == MODE_AUDIO || 
				tLogicChnnl.GetChannelType() == MEDIA_TYPE_H224 )
			{			
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) not encrypt, nack!\n",
					tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;	
			}
		}

		// FECType����, zgc, 2007-10-10
        // ȡ��FEC֧�֣�guzh 2008-02-19
        /*
		if ( tLogicChnnl.GetMediaType() == MODE_VIDEO 
			|| tLogicChnnl.GetMediaType() == MODE_SECVIDEO
			|| tLogicChnnl.GetMediaType() == MODE_AUDIO )
		{
			TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
			u8 byLocalFECType;
			u8 byRemoteFECType;
			if ( tLogicChnnl.GetMediaType() == MODE_VIDEO )
			{
				byLocalFECType = tCapSupportEx.GetVideoFECType();
			}
			else if ( tLogicChnnl.GetMediaType() == MODE_SECVIDEO )
			{
				byLocalFECType = tCapSupportEx.GetDVideoFECType();
			}
			else
			{
				byLocalFECType = tCapSupportEx.GetAudioFECType();
			}
			byRemoteFECType = tLogicChnnl.GetFECType();

			if ( byLocalFECType != byRemoteFECType )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt%d 0x%x(Dri:%d) FECType(%d), diff from local.%d, nack!\n",
					      tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId(), byRemoteFECType, byLocalFECType );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;	
			}
		}
        */

		//�����߼�ͨ��
		if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
		{
			dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
			if( dwRcvIp == 0 )
			{
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) mp not connected, nack!\n",
						 tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;			    
			}

			g_cMcuVcApp.FindMatchedMpIpForMt(m_ptMtTable->GetIPAddr(tMt.GetMtId()),dwRcvIp);

			wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
			m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );

			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] GetMtSwitchAddr for Mt.%d failed, so AssignMtPort and get Port is:%d\n",
				tMt.GetMtId(), wRcvPort);
			
		}

		//�������߼�ͨ���򿪴���
		tLogicChnnl.m_tRcvMediaChannel.SetIpAddr( dwRcvIp );
		tLogicChnnl.m_tRcvMediaCtrlChannel.SetIpAddr( dwRcvIp );

		if( MODE_NONE == tLogicChnnl.GetMediaType() )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Mt.%d open mcu logical chl.Mediatype is none.\n",tMt.GetMtId() );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}

		// vrs��¼��֧�֣�����¼���豸���ܾ���������ͨ��
		if (tMt.GetMtType() == MT_TYPE_VRSREC && m_ptMtTable->GetRecChlType(tMt.GetMtId()) == TRecChnnlStatus::TYPE_RECORD)
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] VrsMt.%d open mcu logical chl.RecChlType is TYPE_RECORD.\n",tMt.GetMtId() );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}

		//zbq[09/02/2008] HD˫������ʱ���ǣ��Ӻ�˫�������
		BOOL32 bStartDS = FALSE;

		switch( tLogicChnnl.GetMediaType() )
		{
		case MODE_VIDEO:
			//��һ·��������H263+��ʽ��
			byChannel = LOGCHL_VIDEO;
			tLogicChnnl.m_tRcvMediaChannel.SetPort( wRcvPort );
			tLogicChnnl.m_tRcvMediaCtrlChannel.SetPort( wRcvPort + 1 );

			// ͨ�����ʳ����������ʣ��ܾ�
            // xsl [7/28/2006] �Ƚ�ʱ������Ƶ�����ӵ�ԣ������ֹ�ϼ�������Ƶ����С�ڱ�����Ƶ���ʵ�������ܾ�
            byAudioType = m_tConf.GetMainAudioMediaType();
			if( tLogicChnnl.GetFlowControl() > m_tConf.GetBitRate() + GetAudioBitrate(byAudioType) )
			{
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] remote bitrate %d, local bitrate %d. nack\n", 
                                tLogicChnnl.GetFlowControl(), 
                                m_tConf.GetBitRate() + GetAudioBitrate(byAudioType));
				return;	
			}

			// vrs��¼��֧�֣���¼��������Flowcontrolֵ��ֹͣ�ָ�����ʱ��Ҫ
			if (tMt.GetMtType() == MT_TYPE_VRSREC && m_tPlayEqp.GetMtId() == tMt.GetMtId())
			{
				m_tPlayEqpAttrib.SetMStremBR(tLogicChnnl.GetFlowControl());
			}

			// [091027]������Ժ֧�ֵ�һ·H263+����
// 			if( MEDIA_TYPE_H263PLUS == tLogicChnnl.GetChannelType() && 
// 				MEDIA_TYPE_H263PLUS == tLogicChnnl.GetActivePayload() )
// 			{
// 				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
//                 ConfLog(FALSE, "[ProcMtMcuOpenLogicChnnlReq] ChnType.%d, ActivePayload.%d, nack\n", 
//                                 tLogicChnnl.GetChannelType(),
//                                 tLogicChnnl.GetActivePayload() );
// 				return;
// 			}

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt%d open mcu primary video logicChannel(0x%x:%d:%s)\n", 
					tMt.GetMtId(), dwRcvIp, wRcvPort, GetMediaStr( tLogicChnnl.GetChannelType() ) );
					
			break;

		case MODE_SECVIDEO:
				
			byChannel = LOGCHL_SECVIDEO;
			tLogicChnnl.m_tRcvMediaChannel.SetPort( wRcvPort + 4 );
			tLogicChnnl.m_tRcvMediaCtrlChannel.SetPort( wRcvPort + 5 );		
			
			//sony G70SP��H263-H239ʱ���û˫��,�Ҳ�ʹ�����ƶ�ʹ��FlowControlIndication���з��ͼ����ر� ����
			//POLYCOM7000�ľ�̬˫��ʱ��ʹ�����ƽ��з��ͼ����ر�
			//���ں��н���ʱ����,֮������ڼ䱣��ͨ����ֱ���˳�����
			if( ( MT_MANU_SONY == m_ptMtTable->GetManuId(tMt.GetMtId()) && 
				MEDIA_TYPE_H263 == m_tConf.GetCapSupport().GetDStreamMediaType() &&
				m_tConf.GetCapSupport().IsDStreamSupportH239()) || 
				MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) )
			{
			}
			else
			{                
                //�ж�˫���ķ���Դ 
				if(CheckDualMode(tMt) ||
				//���ڿƴ�MCU��˫��ͨ�������������
				MT_MANU_KDCMCU == m_ptMtTable->GetManuId(tMt.GetMtId()))
                {
					//��IsMMcuSpeaker�ж���Ч�Ա����False��ֻ��ÿ�α���������ĵ�һ���ж���Ч
					//���µ���λ������NTF�н���
// 					if( m_bMMcuSpeakerValid && tMt.GetMtType() == MT_TYPE_MMCU )
// 					{
// 						m_bMMcuSpeakerValid = FALSE;
// 					}

                    // xsl [7/20/2006]���Ƿ�ɢ����ʱ��Ҫ���ǻش�ͨ����
                    if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsMultiCastMt(tMt.GetMtId())
						&& //IsSatMtOverConfDCastNum(tMt)
						!IsSatMtCanContinue(tMt.GetMtId(),emstartDs)
						/*IsOverSatCastChnnlNum(tMt.GetMtId())*/)
                    {
                        ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] over max upload mt num.\n");
                        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                        return;
                    }
					//�ӳٵ�connect����
					/*UpdateH239TokenOwnerInfo( tMt );
					bStartDS = TRUE;
					StartDoubleStream( tMt, tLogicChnnl );*/
                }
                else
                {

                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mt%d open mcu second video logicChannel(0x%x:%d:H239.%d:%s.Fps:%d) with wrong Dualmode, NACK !\n",
                        tMt.GetMtId(), dwRcvIp, wRcvPort + 4, tLogicChnnl.IsSupportH239(), GetMediaStr(tLogicChnnl.GetChannelType()), tLogicChnnl.GetChanVidFPS());
                    return;	
                }
			}

			// zbq [09/12/2008] Mtadp����ȡ����֡�ʣ��˴�����Ϊ����
			// zjl [10/21/2010] Ŀǰ�򿪶Զ�ͨ������֡��
// 			if (tLogicChnnl.GetChanVidFPS() > 25)
// 			{
// 				if (MEDIA_TYPE_H264 == tLogicChnnl.GetChannelType())
// 				{
// 					tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamUsrDefFPS());
// 				}
// 				else
// 				{
// 					tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamFrameRate());
// 				}
// 			}
            // zbq [09/05/2007] FIXME: H264˫���ķֱ����ݲ����ǣ���������
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d open mcu second video logicChannel([address]0x%x:%d, [H239]%d, [type]%s, [Res]%s, [Fps]%d), Dualmode: %d\n", 
                tMt.GetMtId(), 
                dwRcvIp, wRcvPort + 4, 
                tLogicChnnl.IsSupportH239(), 
                GetMediaStr(tLogicChnnl.GetChannelType()), 
                GetResStr(tLogicChnnl.GetVideoFormat()),
                tLogicChnnl.GetChanVidFPS(),                    
                m_tConf.GetConfAttrb().GetDualMode() );			
			break;

		case MODE_AUDIO:
			byChannel = LOGCHL_AUDIO;
			tLogicChnnl.m_tRcvMediaChannel.SetPort( wRcvPort + 2 );
			tLogicChnnl.m_tRcvMediaCtrlChannel.SetPort( wRcvPort + 3 );
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d open mcu audio logicChannel(0x%x:%d:%s)\n", 
				tMt.GetMtId(), dwRcvIp, wRcvPort+2, GetMediaStr( tLogicChnnl.GetChannelType() ) );
			// [pengjie 2011/12/17] ���е绰�ն�֧�֣��ڿ�����Ƶͨ��ʱ������Ϊ����ok
			if( IsPhoneMt(tMt) )
			{
				KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
				
				if( VCS_CONF == m_tConf.GetConfSource() && 
					( VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() ||
					(m_byCreateBy != CONF_CREATE_MT && VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() ) ))
				{
					ChgCurVCMT( tMt );
				}
				
				if( VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() )
				{
					ChangeVmpStyle(tMt, TRUE);
				}

				// MCS �Զ�����ϳ�,�����ϳɳ�Ա
				if (MCS_CONF == m_tConf.GetConfSource())
				{
					ChangeVmpStyle(tMt, TRUE);//�����Զ�����ϳ�
					//�ն˿�ͨ������ģ�濪���Ļ��ָ������ϳ�
					AdjustCtrlModeVmpModuleByMt(tMt);
				}

			}
			break;
			
		case MODE_DATA:
			if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_H224 )
			{
				byChannel = LOGCHL_H224DATA;
				dwRcvIp = g_cMcuVcApp.GetMtAdpIpAddr( tMt.GetDriId() );
				tLogicChnnl.m_tRcvMediaChannel.SetIpAddr( dwRcvIp );
				tLogicChnnl.m_tRcvMediaChannel.SetPort( wRcvPort + 6 );
				tLogicChnnl.m_tRcvMediaCtrlChannel.SetPort( wRcvPort + 7 );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d open mcu H.224 logicChannel(0x%x:%d:%s)\n", 
					tMt.GetMtId(), dwRcvIp, wRcvPort + 6 , GetMediaStr( tLogicChnnl.GetChannelType() ) );
				
			}
			//�Զ�����򿪱��˵�T120����ͨ��
			else if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_T120 )
			{
				//��ʱ, ����ֻ���Ǳ���. �Զ������KEDA MCU, ���ַӦ��Ϊ��
				//��д���˵�DCS������ַ, �ظ�ACK, �ɶԶ���MCU��������
				if ( !m_ptMtTable->IsNotInvited( tMt.GetMtId() ) )
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mt%d should be calling in !\n", tMt.GetMtId() );
					return;
				}
				else
				{
					byChannel = LOGCHL_T120DATA;
					
					//ȡ����DCS��ַ�Ͷ˿�
					u32 dwDcsIp  = g_cMcuVcApp.m_atPeriDcsTable[m_byDcsIdx-1].m_dwDcsIp;
					u16 wDcsPort = g_cMcuVcApp.m_atPeriDcsTable[m_byDcsIdx-1].m_wDcsPort;

					if ( 0 != dwDcsIp && 0 != wDcsPort )
					{
						tLogicChnnl.m_tRcvMediaChannel.SetIpAddr( dwDcsIp );
						tLogicChnnl.m_tRcvMediaChannel.SetPort( wDcsPort );

						//�˴�, ������ΪT120�߼�ͨ���򿪳ɹ�
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mcu open Mt%d data T.120 logicChannel(0x%x:%d:%s) as slave success!\n", 
							tMt.GetMtId(), dwDcsIp, wDcsPort, GetMediaStr( tLogicChnnl.GetChannelType() ) );		
					}
					else
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Get local DCS Ip failed in open logic channel ack !\n" );
						return;
					}
				}				
			} 
			else if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_MMCU )
			{			
				if( !m_tConf.GetConfAttrb().IsSupportCascade() ||
					tMt.GetMtType() != MT_TYPE_MMCU || 
					m_tConfAllMcuInfo.GetSMcuNum() >= MAXNUM_SUB_MCU)
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) not support mmcu(%s), nack!\n",
						tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , GetMediaStr(tLogicChnnl.GetChannelType()));
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					return;	
				}
                else if ( !m_tCascadeMMCU.IsNull() )
                {
                    // guzh [4/12/2007] ����Ѿ����ϼ�MCU�������Ƿ���Ǹ�MCU
                    // ���ﲻ�ܾ��Բ���IP�Ƚϣ��ϼ����ܻ�����塣ֻ�����ȳ��Ա�������E164���п��ܻᷢ������)
                    BOOL32 bRejectAndRemoveMMcu = FALSE;
					BOOL32 bJoinedConf = FALSE;
                    if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
                    {
						bJoinedConf = TRUE;
                        TMtAlias tMtAlias;
                        TMtAlias tMMcuAlias;
                        if ( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias) &&
                             m_ptMtTable->GetMtAlias( m_tCascadeMMCU.GetMtId(), mtAliasTypeH323ID, &tMMcuAlias) )
                        {
                            if ( tMtAlias == tMMcuAlias )
                            {
                                bRejectAndRemoveMMcu = TRUE;
                            }
                        }
                        else if ( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeE164, &tMtAlias) &&
                                  m_ptMtTable->GetMtAlias( m_tCascadeMMCU.GetMtId(), mtAliasTypeE164, &tMMcuAlias) )
                        {
                            if ( tMtAlias == tMMcuAlias )
                            {
                                bRejectAndRemoveMMcu = TRUE;
                            }
                        }
                        else if ( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias) &&
                                  m_ptMtTable->GetMtAlias( m_tCascadeMMCU.GetMtId(), mtAliasTypeTransportAddress, &tMMcuAlias) )
                        {
                            if ( tMtAlias == tMMcuAlias )
                            {
                                bRejectAndRemoveMMcu = TRUE;
                            }
                        }
                    }
                    if (bRejectAndRemoveMMcu && g_cMcuVcApp.IsAutoDetectMMcuDupCall())
                    {
                        // ���������������,�����Ѹ��ϼ�MCU�Ҷ�(�ܿ����ǲ���) ,Ȼ��ȴ��Զ��غ�
                        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) calling in seems to be the MMCU, Drop both!",
                                tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId());

                        SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
                        SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);   
                    }
                    else
                    {
						if (bJoinedConf)
						{
							ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Has current MMcu, del it!\n");
							RemoveJoinedMt(tMt, TRUE);
						}
                        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d 0x%x(Dri:%d) calling in, but current has MMCU, nack!\n",
                            tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );
					    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );            
                    }
                    return;
                }

				dwRcvIp = g_cMcuVcApp.GetMtAdpIpAddr( tMt.GetDriId() );
				tLogicChnnl.m_tRcvMediaChannel.SetIpAddr( dwRcvIp );
				tLogicChnnl.m_tRcvMediaChannel.SetPort( 3337 );
				byChannel =  LOGCHL_MMCUDATA;
				//�����ϼ�MCU������Ϊ�������նˣ��Ͽ����Զ�ɾ�����б�
				m_ptMtTable->SetNotInvited( tMt.GetMtId(), TRUE );
				
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d open mcu mmcu logicChannel(0x%x:%d:%s)\n", 
					tMt.GetMtId(), dwRcvIp, wRcvPort+9, GetMediaStr( tLogicChnnl.GetChannelType() ) );

				//���ն��Ƿ��Ѿ�ͨ��������֤
				if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
				{
					m_tCascadeMMCU = tMt;
					m_tConfAllMtInfo.m_tMMCU = tMt;
                    m_tConfInStatus.SetNtfMtStatus2MMcu(TRUE);
					m_ptMtTable->SetMtType(tMt.GetMtId(), MT_TYPE_MMCU);
					
					//���˼���mcu��Ϣ����������б���
					u16 wMcuIdx = INVALID_MCUIDX;
					u8 abyMcuId[MAX_CASCADEDEPTH-1];
					memset( &abyMcuId[0],0,sizeof(abyMcuId) );
					abyMcuId[0] = tMt.GetMtId();
					m_tConfAllMcuInfo.AddMcu( &abyMcuId[0],2,&wMcuIdx );
					m_ptConfOtherMcTable->AddMcInfo( wMcuIdx );	
					m_tConfAllMtInfo.AddMcuInfo( wMcuIdx/*tMt.GetMtId()*/, tMt.GetConfIdx() );

					//֪ͨ���
					cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
					if(m_tConf.HasConfExInfo())
					{
						u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
						u16 wPackDataLen = 0;
						PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
						cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
					}
					SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
					
					//������ػ��������ն���Ϣ
					//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
					//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
					SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

					//�¼������Զ����ϼ���Ϊ������
					if(m_tConf.GetSpeaker().IsNull() && g_cMcuVcApp.IsMMcuSpeaker())
					{
						ChangeSpeaker(&m_tCascadeMMCU);
					}
				}

			}
			break;
		default:
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CALL,"Mt.%d open mcu logical chl.mediatype(%d)\n",tMt.GetMtId(),tLogicChnnl.GetMediaType() );
			break;
		}

		
        
        // zbq [10/11/2007] ǰ�����̬�غ��������� ��Ƶ/��Ƶ/˫�� ֧��
        BOOL32 bLogicChanFEC = FALSE;
        if ( tLogicChnnl.GetFECType() != FECTYPE_NONE )
        {
            bLogicChanFEC = TRUE;
            tLogicChnnl.SetActivePayload(MEDIA_TYPE_FEC);
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "Mt.%d set ActivePayload.%d due to FEC\n", tMt.GetMtId(), MEDIA_TYPE_FEC);
        }

		//������ģʽ����mcuΪ����,h263plus��H239(H263��/H264)��h264����Ҫ��̬�غɣ�Ӧǿ�Ʒ���Դ����mcuԼ�������䶯̬�غ�
		//�����ǵ�Polycom7000����֧��H263˫����ǿ�ƺ������� �Լ� ��ʹǿ��Ҳ���ܲ�����ǣ���sony�����ʲ�ǿ��
		//����h263plus�ķǱ�׼��̬�غ�ֵ�� ��������һ�����м��Թ淶����
		if(!m_ptMtTable->IsMtIsMaster(tMt.GetMtId())&& 
            // guzh zbq [07/25/2007] �ն˻�����̬�غ� �߼�����
            (tLogicChnnl.GetMediaEncrypt().GetEncryptMode() != CONF_ENCRYPTMODE_NONE ||
             MEDIA_TYPE_H264     == tLogicChnnl.GetChannelType() || 
             MEDIA_TYPE_H263PLUS == tLogicChnnl.GetChannelType() ||
             // guzh [9/26/2007] G7221.C ��Ҫ֧�� 
             MEDIA_TYPE_G7221C   == tLogicChnnl.GetChannelType() ||
             // guzh [02/20/2008] AAC֧��
             MEDIA_TYPE_AACLC   == tLogicChnnl.GetChannelType() ||
             MEDIA_TYPE_AACLD   == tLogicChnnl.GetChannelType()
            ) 
          )
		{
			tLogicChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
            if (!bLogicChanFEC)
            {
                tLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, tLogicChnnl.GetChannelType()));
            }
		}
        
		if(tLogicChnnl.GetMediaType() == MODE_VIDEO)
		{
			TDri2E1Cfg tDri2E1Cfg[MAXNUM_SUB_MCU];
			g_cMcuVcApp.GetDri2E1CfgTable(tDri2E1Cfg);
			u16 wRealBandwidth = 0;

			if(m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU
			  || m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MMCU)
			{
				for ( u8 byCfgIndx = 0; byCfgIndx < MAXNUM_SUB_MCU; byCfgIndx++)
				{
					u32 dwConIpAddr = tDri2E1Cfg[byCfgIndx].GetIpAddr();
					//�ж�MMCU(SMCU)�Ƿ��������ļ�������[10/25/2012 chendaiwei]
					if( g_cMcuVcApp.GetRealBandwidthByIp(dwConIpAddr,wRealBandwidth)
						&& dwConIpAddr == m_ptMtTable->GetIPAddr(tMt.GetMtId()))
					{
						m_ptMtTable->SetMtTransE1(tMt.GetMtId(),TRUE);
						
						u16 wActualBR = wRealBandwidth - GetAudioBitrate(m_tConf.GetMainAudioMediaType());
						if( wActualBR < tLogicChnnl.GetFlowControl())
						{
							tLogicChnnl.SetFlowControl(wActualBR);
							
							ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuOpenLogicChnnlReq]Adjust SMCU<%d> RL from %d to %d\n",tMt.GetMtId(),tLogicChnnl.GetFlowControl(),wActualBR);
						}
						
						break;
					}
				}
			}
		}

		//����
		m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannel, &tLogicChnnl, FALSE );
		ConfPrint(LOG_LVL_DETAIL,MID_MCU_MT2,"[ProcMtMcuOpenLogicChnnlReq]tLogicChnnl.GetMediaType(): %d, tLogicChnnl.GetChannelType(): %d\n",
			tLogicChnnl.GetMediaType(), tLogicChnnl.GetChannelType());
	
		BOOL32 bIsCanInVmp = TRUE;
		if( VCS_CONF == m_tConf.GetConfSource() && VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() &&
			tMt == m_cVCSConfStatus.GetReqVCMT() &&
			( tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO )
			)
		{
			//�෽�໭��ƴ��ն�Ҳ�ڴ˴�����,��֤�߼�ͨ�������򿪺�,��ChgCurVCMT
			//�ն˻ᰴ�����Ӵ�С�������,ֱ���ɹ�.
			KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
			if (IsKedaMt(tMt, TRUE))
			{
				ChgCurVCMT( tMt );
			}
			//zhouyiliang 20110309 ����8000G��˵����keda>h264 cif �ն˲�������ϳ�
			//8000A�Ļ��������Ƿ���ǰ����ͨ��
			else //if( !IsKedaMt(tMt, TRUE) )
			{
				if( !IsNoneKedaMtCanInMpu(tMt) )
				{
/*#ifdef _8KE_				
					if( IsMtNotInOtherTvWallChnnl(tMt,0,0) && IsMtNotInOtherHduChnnl(tMt,0,0) )
					{
						NotifyMcsAlarmInfo(0,ERR_VMP_8000E_NOTSPPORT_NONEKEDAMT);
						VCSDropMT( tMt );
						return;
					}
					else
					{
						if( !m_cVCSConfStatus.GetMtInTvWallCanMixing() )
						{
							RemoveSpecMixMember( &tMt, 1,TRUE,TRUE );
						}
						bIsCanInVmp = FALSE;
					}
#else*/
					u8 byVmpSubType = GetVmpSubType(m_tVmpEqp.GetEqpId());
					TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(m_tVmpEqp);
					//8000A(MPU/MPU2) 8000H ��ǰ�����vmp,8KGǰ�������Ϊ0
					// vpuͬ����֧�ַ�keda>h264 cif �ն˽�����ϳ�
					if( VMP == byVmpSubType
						|| tVmpChnnlInfo.GetHDChnnlNum() >= tVmpChnnlInfo.GetMaxNumHdChnnl())
					{
						if( IsMtNotInOtherTvWallChnnl(tMt,0,0) && IsMtNotInOtherHduChnnl(tMt,0,0) )
						{
							//û��ǰ����ͨ����
							NotifyMcsAlarmInfo(0,ERR_VMP_NO_VIP_VMPCHNNL);//ERR_VMP_NONEKEDAMT_CANNOTINMPU
							VCSDropMT( tMt );
							return;
						}
						else
						{
							if( !m_cVCSConfStatus.GetMtInTvWallCanMixing() )
							{
								RemoveSpecMixMember( &tMt, 1,TRUE,TRUE );
							}
							bIsCanInVmp = FALSE;
						}
					}
//#endif
				}

				ChgCurVCMT( tMt );
			}
		}

		//zbq[09/02/2008] ˫���ӳٵ����﷢��
// 		if (MODE_SECVIDEO == tLogicChnnl.GetMediaType() && bStartDS)
// 		{
// 			StartDoubleStream( tMt, tLogicChnnl );
// 		}

        //�ն����д����ʼ��
        if(tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO)
        {
            if( m_ptMtTable->IsLogicChnnlOpen(tMt.GetMtId(),LOGCHL_AUDIO,FALSE) )
			{
				u16 wDialRate = m_ptMtTable->GetDialBitrate( tMt.GetMtId());
				u16 wFlowCtrl = tLogicChnnl.GetFlowControl();
				m_ptMtTable->SetSndBandWidth( tMt.GetMtId(),min(wDialRate,wFlowCtrl));
			}
			else
			{
				m_ptMtTable->SetSndBandWidth(tMt.GetMtId(), tLogicChnnl.GetFlowControl());
			}
        }

		if( tLogicChnnl.GetMediaType() == MODE_AUDIO && byChannel == LOGCHL_AUDIO )
		{
			if( m_ptMtTable->IsLogicChnnlOpen(tMt.GetMtId(),LOGCHL_VIDEO,FALSE) )
			{
				u16 wDialRate = m_ptMtTable->GetDialBitrate( tMt.GetMtId());
				u16 wOriginalBW = m_ptMtTable->GetSndBandWidth(tMt.GetMtId());
				m_ptMtTable->SetSndBandWidth( tMt.GetMtId(),min(wDialRate,wOriginalBW));
			}			
		}


		//[nizhijun 2010/12/21] �¼��ն����¼�����˫������ʱ���ϼ����յ��¼���239���ͨ�棬
		//������ʱ��Ӧ����changeprssrcȥ����prs���ϼ����ն˵�RTP��������Ϊ��ʱ����ϼ���˵
		//m_tDoubleStreamSrc�ǿյģ����ܳɹ�����RTP����
		//��ˣ����ڽ���˫��PRS�����ն˵�RTP����Ӧ�÷���startdoublestream�У���Ϊ��ʱ��m_tDoubleStreamSrc�Ǵ��ڵ�
		//�ڶ�·��Ƶ�Ķ����ش�
// 		if( byChannel == LOGCHL_SECVIDEO)
// 		{
// 			if(m_tConf.GetConfAttrb().IsResendLosePack())
// 			{
// 				ChangePrsSrc( tMt, PRSCHANMODE_SECOND);
// 			}
// 		}
	
        //����Ƶ��˫����
        if ( cServMsg.GetMsgBodyLen()  >= sizeof(BOOL32) + sizeof(TTransportAddr) + sizeof(TLogicalChannel) &&
             ( tLogicChnnl.GetMediaType() == MODE_VIDEO ||
               tLogicChnnl.GetMediaType() == MODE_AUDIO ||
			   tLogicChnnl.GetMediaType() == MODE_SECVIDEO ))
        {
            BOOL32 bIsPinHole = *( BOOL32 * )( cServMsg.GetMsgBody() + sizeof(TLogicalChannel));
            TTransportAddr tRemoteRtpSndAddr = *( TTransportAddr * )( cServMsg.GetMsgBody() + sizeof(TLogicalChannel) + sizeof(BOOL32));
            
            if (tLogicChnnl.GetMediaType() == MODE_AUDIO)
            {
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tAudRtpSndTransportAddr.SetIpAddr(tRemoteRtpSndAddr.GetIpAddr());
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tAudRtpSndTransportAddr.SetPort(tRemoteRtpSndAddr.GetPort());
            }
            else if(tLogicChnnl.GetMediaType() == MODE_VIDEO)
            {
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tVidRtpSndTransportAddr.SetIpAddr(tRemoteRtpSndAddr.GetIpAddr());
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tVidRtpSndTransportAddr.SetPort(tRemoteRtpSndAddr.GetPort());                
            }
            else if (tLogicChnnl.GetMediaType() == MODE_SECVIDEO)
            {
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tSecRtpSndTransportAddr.SetIpAddr(tRemoteRtpSndAddr.GetIpAddr());
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tSecRtpSndTransportAddr.SetPort(tRemoteRtpSndAddr.GetPort());                                
            }
            
            if ( TRUE == bIsPinHole )
            {
                //rtp��
                Starth460PinHoleNotify( tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(),
                                        tLogicChnnl.m_tRcvMediaChannel.GetPort(),
                                        tRemoteRtpSndAddr.GetIpAddr(),
                                        htons(tRemoteRtpSndAddr.GetPort()),
                                        pinhole_rtp,
                                        tLogicChnnl.m_byActivePayload);
                ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] rtp PinHole Mt:%d, Local(0x%x:%d),Remote(0x%x:%d),payLoad:%d !\n", 
                           tMt.GetMtId(), 
                           tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(),
                           tLogicChnnl.m_tRcvMediaChannel.GetPort(),
                           tRemoteRtpSndAddr.GetIpAddr(),
                           htons(tRemoteRtpSndAddr.GetPort()),
                           tLogicChnnl.m_byActivePayload);
                
                //rtcp��
                Starth460PinHoleNotify( tLogicChnnl.m_tRcvMediaCtrlChannel.GetIpAddr(),
                                        tLogicChnnl.m_tRcvMediaCtrlChannel.GetPort(),
                                        tLogicChnnl.m_tSndMediaCtrlChannel.GetIpAddr(),
                                        tLogicChnnl.m_tSndMediaCtrlChannel.GetPort(),
                                        pinhole_rtcp,
                                        tLogicChnnl.m_byActivePayload);
                ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] rtcp PinHole Mt:%d, Local(0x%x:%d),Remote(0x%x:%d),payLoad:%d !\n", 
                           tMt.GetMtId(), 
                           tLogicChnnl.m_tRcvMediaCtrlChannel.GetIpAddr(),
                           tLogicChnnl.m_tRcvMediaCtrlChannel.GetPort(),
                           tLogicChnnl.m_tSndMediaCtrlChannel.GetIpAddr(),
                           tLogicChnnl.m_tSndMediaCtrlChannel.GetPort(),
                           tLogicChnnl.m_byActivePayload);               
                
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_bIsPinHole = TRUE;
            }                
        }
        
		//Ӧ��
		if(tLogicChnnl.GetMediaType() == MODE_VIDEO)
		{
			m_ptMtTable->SetMtVideoSend(tMt.GetMtId(), TRUE);			
		}
		else if(tLogicChnnl.GetMediaType() == MODE_AUDIO)
		{
			m_ptMtTable->SetMtAudioSend(tMt.GetMtId(), TRUE);
		} 
		else if(tLogicChnnl.GetMediaType() == MODE_SECVIDEO)
		{
            KillTimer(MCUVC_WAITOPENDSCHAN_TIMER);
		}

		cServMsg.SetMsgBody( ( u8 * )&tLogicChnnl, sizeof( tLogicChnnl ) );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );


		ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "when mt.%d olc(mediatype:%d) mcu, tLogicChnnl's flowcontrol is: %d; conf's br is: %d!\n",
					tMt.GetMtId(),tLogicChnnl.GetMediaType(), tLogicChnnl.GetFlowControl(), m_tConf.GetBitRate());

		TMt tNullMt;
		if( tLogicChnnl.GetMediaType() == MODE_SECVIDEO )
		{
			if(MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId())
				//&& MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId())
				)
			{
				// [12/11/2009 xliang] send 2 flow control cmd when Aethra OLC(DV)
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq]Send flowcontrol cmd to Mt.%u(V) due to Aethra\n", tMt.GetMtId());

				TLogicalChannel tLogicVChnnl;
				m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tLogicVChnnl, FALSE);
					
				tLogicVChnnl.SetMediaType(MODE_VIDEO);
				tLogicVChnnl.SetFlowControl(m_tConf.GetBitRate()/2);	//���ʰ������۰���
				cServMsg.SetMsgBody( ( u8 * )&tLogicVChnnl, sizeof( tLogicVChnnl ) );
				u8 byDirection = 1; //outgoing:0 ;	incoming: 1
				cServMsg.CatMsgBody( &byDirection, sizeof(u8) );
				SendMsgToMt(tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
				
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq]Send flowcontrol cmd to Mt.%u(SecV) due to Eathra\n", tMt.GetMtId());
				tLogicChnnl.SetMediaType(MODE_SECVIDEO);
				tLogicChnnl.SetFlowControl( m_tConf.GetBitRate()/2 );
				cServMsg.SetMsgBody();
				cServMsg.SetMsgBody( ( u8 * )&tLogicChnnl, sizeof( tLogicChnnl ) );
				cServMsg.CatMsgBody( &byDirection, sizeof(u8) );
				SendMsgToMt(tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
			}
			//���������£������ն��ڷ���˫��֮ǰ��Ҫ�����ն�ֹͣ����˫��������ᵼ�¼�������
			if(IsMultiCastMt(tMt.GetMtId()))
			{
				// ��¼��ǰ���ǵ������˴���ʼ��flowcontrol 0�����������ǵ���
				s16 byCurSatMtNum = GetCurSatMtNum();
				CServMsg cTempServMsg;
				tLogicChnnl.SetFlowControl(0);
				cTempServMsg.SetMsgBody((u8*)&tLogicChnnl, sizeof(tLogicChnnl)); 
				SendMsgToMt(tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cTempServMsg); 
				// ��֤���������ǵ���
				SetCurSatMtNum(byCurSatMtNum);
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[ProcMtMcuOpenLogicChnnlReq]DV RemoveMt(%d) SatMtNum(%d)\n",
					tMt.GetMtId(),GetCurSatMtNum());
			}
		}

		if( tLogicChnnl.GetMediaType() == MODE_VIDEO ||
			 tLogicChnnl.GetMediaType() == MODE_AUDIO ||
			 tLogicChnnl.GetMediaType() == MODE_SECVIDEO) 
		{
			if ( !m_tCascadeMMCU.IsNull() &&
				m_tConfAllMtInfo.MtJoinedConf(m_tCascadeMMCU.GetMtId()) )
			{
				OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(), tMt.GetMtId());
			}			

			MtStatusChange( &tMt,TRUE );
		}
	
		//���ն˿���δͨ��������֤
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mt%d doesnot join conf, then doesnot deal audio/video switch ... \n", 
					  tMt.GetMtId() );
			return;
		}

        //zbq[09/24/2007] ����VMP��ϯ���濼�ǣ��˴����Ե�����ϯ��ؽ���
        if( m_tConf.HasChairman() &&  m_tConf.GetChairman() == tMt )
        {
            //AdjustChairmanSwitch();
			//zgc [04/21/2008] ��Ƶ��Ƶ�ֿ�����
			// ��Ƶͨ��
			if ( tLogicChnnl.GetMediaType() == MODE_AUDIO )
			{
				AdjustChairmanAudSwitch();
			}
			
			// ��Ƶͨ��
			//���ǻ��鿼�ǣ�����ϯ�ָ��ŵ�֮����
// 			if ( tLogicChnnl.GetMediaType() == MODE_VIDEO )
// 			{
// 				AdjustChairmanVidSwitch();
// 			}
        }

		//������Ƶ��������
		if( tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO )
		{
			//��������, ת��dump����ֹicmp
			g_cMpManager.StartRecvMt( tMt, MODE_VIDEO );
            
            //zbq[07/18/2007] SSRC ���޸��ɹ�һ����ͳһ����
			//g_cMpManager.ResetRecvMtSSRC( tMt, MODE_VIDEO );

			BOOL32 bIsStopSpeakerMtSend = FALSE;
			//���Ƿ����˼���
			if(GetLocalSpeaker() == tMt )
			{
				u16 wBitRate = 0;
				BOOL32 bIsChangeSpeaker = TRUE;
				wBitRate = tLogicChnnl.GetFlowControl();
				tLogicChnnl.SetFlowControl( 0 );
				m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannel, &tLogicChnnl, FALSE );
                // xsl [8/22/2006]���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
                if (m_tConf.GetConfAttrb().IsSatDCastMode()
					&& IsMultiCastMt(tMt.GetMtId()) )
                {
					//if( IsSatMtOverConfDCastNum(tMt) ) 
					if( !IsSatMtCanContinue(tMt.GetMtId(),emSpeaker) ) 
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] over max upload mt num. cancel speaker!\n");
						//ChangeSpeaker(NULL);
						bIsChangeSpeaker = FALSE;
					}   
					//tLogicChnnl.SetFlowControl( wBitRate );
					//m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannel, &tLogicChnnl, FALSE );
                }
                if( bIsChangeSpeaker )
                {
					//zhouyiliang 20121016����ǻָ������ˣ���ǰ������ѯ����Ƶ������ͣ����ʱbPolling ҪΪTRUE����ͣ��ѯ
					BOOL32 bPolling = FALSE;
					if ( m_tConf.m_tStatus.GetPollMedia() == MODE_BOTH )
					{
						bPolling = TRUE;
					}
					// �л�������֮ǰ,�Ƚ����鷢�����ÿ�,��ֹ�ı䷢����ʱ��Ϊ�¾ɷ��������
					m_tConf.SetSpeaker(tNullMt);
                    ChangeSpeaker( &tMt,bPolling );
                }
				tLogicChnnl.SetFlowControl( wBitRate );
				m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannel, &tLogicChnnl, FALSE );
				if (!IsSpeakerCanBrdVid( &tMt ) || !bIsChangeSpeaker )
				{
					bIsStopSpeakerMtSend = TRUE;
				}
			}

			if( !( GetLocalSpeaker() == tMt ) || bIsStopSpeakerMtSend )
			{
				if( tMt.GetMtType() == MT_TYPE_MMCU || tMt.GetMtType() == MT_TYPE_SMCU )
				{				
					NotifyMtSend( tMt.GetMtId(), MODE_VIDEO, TRUE );
				}
				else
				{
					// ������ʱ��¼���ǵ���
					s16 byCurSatMtNum = 0;
                    //  xsl [1/18/2006] �����vmp��tvwallģ���ﲻ��flowcontrol 0                   
                    if( (!m_tConfEqpModule.IsMtInVmp(tMt) && 
                         !m_tConfEqpModule.IsMtInTvwall(tMt) &&
						 !m_tConfEqpModule.IsMtInHduVmpChnnl(tMt)) || 
						 bIsStopSpeakerMtSend )
                    {
						// ��¼��ǰ���ǵ������˴���ʼ��flowcontrol 0�����������ǵ���
						byCurSatMtNum = GetCurSatMtNum();
                        NotifyMtSend( tMt.GetMtId(), MODE_VIDEO, FALSE );
						// ��֤���������ǵ���
						if (GetCurSatMtNum() > 0 && IsMultiCastMt(tMt.GetMtId()))
						{
							SetCurSatMtNum(byCurSatMtNum);
							LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[ProcMtMcuOpenLogicChnnlReq]RemoveMt(%d) SatMtNum(%d)\n",
								tMt.GetMtId(),GetCurSatMtNum());
						}
                    }
					else
					{
						// ��vmp��tvwallģ���У���ռ�����ǵ���
						if (IsMultiCastMt(tMt.GetMtId()))
						{
							// ���е�������ռ��
							if (GetCurSatMtNum() > 0)
							{
								SetCurSatMtNum(GetCurSatMtNum() - 1);
								LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[ProcMtMcuOpenLogicChnnlReq]Mt is in ConfEqpModule,RemoveMt(%d) SatMtNum(%d)\n",
															tMt.GetMtId(),GetCurSatMtNum());
							}
							else // ���޵���������������ն˷���flowcontrol 0
							{
								// ��¼��ǰ���ǵ������˴���ʼ��flowcontrol 0�����������ǵ���
								byCurSatMtNum = GetCurSatMtNum();
								// ģ�汣��ʱ�б������˳����������
								NotifyMtSend( tMt.GetMtId(), MODE_VIDEO, FALSE );
								// ��֤���������ǵ���
								SetCurSatMtNum(byCurSatMtNum);
								LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[ProcMtMcuOpenLogicChnnlReq]SatMtNum is not enough,RemoveMt(%d) SatMtNum(%d)\n",
									tMt.GetMtId(),GetCurSatMtNum());
							}
						}
					}
				}			
			}
			AdjustChairmanVidSwitch();


			//zjj20091026�������� �������ǽͨ�����г�Ա��Ϣ˵���ó�Ա�Ѿ��������ͨ��,
			//�ն˺���ͨ���򿪺�Ҫ������
			if( m_tConf.GetConfSource() == VCS_CONF )
			{
				// [6/2/2011 liuxu] �Ե���ǽ��hduҵ���������

				// һ������֧�ֵĵ���ǽͨ��������
				const u8 byMaxTvwChnnlNum = MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL + MAXNUM_PERIEQP_CHNNL * MAXNUM_MAP;
				CConfTvwChnnl acTvwChnnlFind[byMaxTvwChnnlNum];	// ��Ѱ�ҵĵ���ǽͨ��
				u8 byEqpId, byChnnlIdx;							// ����ǽͨ����Ӧ���豸�ź����豸�е�ͨ����
				
				TMt tTvwMt;										// ���˳�����ǽͨ�����ն�
				TTvwMember tTvwMember;							// ����ǽͨ����Ա
				
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
					
					// ������, �򲻽��в���
					if (!g_cMcuVcApp.IsPeriEqpConnected(byEqpId))
					{
						continue;
					}

					tTvwMember = acTvwChnnlFind[byLoop].GetMember();
					tTvwMt	   = (TMt)tTvwMember;
					tTvwMt     = GetLocalMtFromOtherMcuMt( tTvwMt );

					// �����ն˻�mcu����, ����¼�mcu����,���������ն�����ڵ���ǽ��, ��ָ�����ǽ����
					if (tTvwMt.IsNull() 
						|| tTvwMt.GetMtId() != GetLocalMtFromOtherMcuMt(tMt).GetMtId())
					{
						continue;
					}
					
					if( ( TW_MEMBERTYPE_VCSSPEC == tTvwMember.byMemberType || TW_MEMBERTYPE_VCSAUTOSPEC == tTvwMember.byMemberType ) 
						&& tTvwMember.GetConfIdx() == m_byConfIdx )
					{ 
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, 
							"[ProcMtMcuOpenLogicChnnlReq] VcsConf mt(%d.%d) in tvwall member channeltype(%d) format(%d) \n" ,
							tTvwMember.GetMcuId(), tTvwMember.GetMtId(),
							tLogicChnnl.GetChannelType(),tLogicChnnl.GetVideoFormat());
						
						// [8/19/2011 liuxu] ��ֹ�ն˽������ǽʧ��,ȴ�����˻���
						if (IsValidHduChn(byEqpId, byChnnlIdx))
						{
							if( !ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() ) 
								&& !CheckHduAbility( tMt, byEqpId, byChnnlIdx) 
								&& !(tMt == m_cVCSConfStatus.GetCurVCMT())
								)
							{
								ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  
									"!GroupMode, tMt(%d.%d) Can't switch to hdu(%d), so Drop it\n",
									tMt.GetMcuId(), tMt.GetMtId(), byEqpId);
								ClearTvwChnnlStatus(acTvwChnnlFind[byLoop].GetEqpId(), acTvwChnnlFind[byLoop].GetChnnlIdx());
								VCSDropMT( tMt );
								return;
							}
							else 
							{
								// [2013/03/11 chenbing] VCS���鲻֧��HDU�໭��,��ͨ����0
								ChangeHduSwitch( &tMt, byEqpId, byChnnlIdx, 0, tTvwMember.byMemberType,
									TW_STATE_START );
							}
						}
						else
						{
							ChangeTvWallSwitch( &tTvwMt, byEqpId, byChnnlIdx, tTvwMember.byMemberType, TW_STATE_START);
						}

						if( !tTvwMember.IsLocal() )
						{
							OnMMcuSetIn( tTvwMt, m_cVCSConfStatus.GetCurSrcSsnId(), SWITCH_MODE_SELECT ); 
						}
					}
				}
			}

            //�йػ���ϳɵĴ���
            if( GetVmpCountInVmpList() > 0 )
            {
				//�����Զ�����ϳ�
				if (bIsCanInVmp)
				{
					ChangeVmpStyle(tMt, TRUE);
				}

				//�ն˿�ͨ������ģ�濪���Ļ��ָ������ϳ�
				AdjustCtrlModeVmpModuleByMt(tMt);

				if( m_byCreateBy == CONF_CREATE_NPLUS)
				{
					TEqp tVmpEqp;
					u8 byVmpSubType;
					TVMPParam_25Mem tVmpParam;
					BOOL32 bAdjustVmpMember = FALSE;
					u8 byVmpId;
					u8 byChIdx;
					TMt tSpeaker = m_tConf.GetSpeaker();
					u8 byNplusMemberType;
					TVMPMember tVMPMember;
					//s8 achAlias[VALIDLEN_ALIAS+1] = {0};
                    s8 achAlias[MAXLEN_ALIAS] = {0};// ���� [7/4/2013 liaokang]                    
					CServMsg cMsg;
					TPeriEqpStatus tVmpPeriEqpStatus;
					for (byVmpId = VMPID_MIN; byVmpId <= VMPID_MAX; byVmpId++)
					{
						tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
						byVmpSubType = GetVmpSubType(byVmpId);
						tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);

						//VMP��δ�����ɹ������Զ�VMP
						if(tVmpParam.GetVMPMode() == CONF_VMPMODE_NONE || IsDynamicVmp(tVmpEqp.GetEqpId()))
						{
							continue;
						}

						bAdjustVmpMember = FALSE;
						for (byChIdx = 0; byChIdx < MAXNUM_VMP_MEMBER; byChIdx++)
						{
							tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);//confvmpparam�����
							byNplusMemberType = 0;
							if(g_cMcuVcApp.NPlusIsMtInVmpModule(tMt.GetMtId(),byChIdx,m_byConfIdx,byVmpId,byNplusMemberType))
							{
								// ���ն�����ǰ,��ͨ����Ա�ѱ�,�����ٻָ���ͨ����Ա
								tVMPMember = *tVmpParam.GetVmpMember(byChIdx);
								if (VMP_MEMBERTYPE_NULL != tVMPMember.GetMemberType()
									&& byNplusMemberType != tVMPMember.GetMemberType())
								{
									ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq][N+1]vmp member.%d membertype: %d, is not nplus module membertype%d, can't into vmp!\n", 
										byChIdx, tVMPMember.GetMemberType(), byNplusMemberType);
									
									continue;
								}
								if (!tVMPMember.IsNull() && !(tVMPMember == tMt))
								{
									ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq][N+1]vmp member.%d info: mt(%d,%d), is not openlogic mt(%d,%d), can't into vmp!\n", 
										byChIdx, tVMPMember.GetMcuId(), tVMPMember.GetMtId(), tMt.GetMcuId(), tMt.GetMtId());
									
									continue;
								}
								// ��Է����˸���,����ǰ�������Ѳ���ģ�淢�����ն�,���ָ������˸���ͨ����Ա
								// ����:ģ�濪��ʱ,�������ն�δ����,�ı䷢������ȡ�������˺�,���ն�������,���ն˲���䷢����,��ģ�淢���˸���ͨ�����д��ն�
								if (VMP_MEMBERTYPE_SPEAKER == tVMPMember.GetMemberType() && 
									!(tMt == tSpeaker))
								{
									ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq][N+1]vmp member.%d info: SpeakerMt(%d,%d), is not openlogic mt(%d,%d), can't into vmp!\n", 
										byChIdx, tSpeaker.GetMcuId(), tSpeaker.GetMtId(), tMt.GetMcuId(), tMt.GetMtId());
									
									continue;
								}

								UpdataVmpEncryptParam(tMt, byChIdx,byVmpId);
								//�������¼��ϣ�����SetVmpChnnl()ʱ���Զ����벢���µ�������
								memset(&tVMPMember, 0, sizeof(tVMPMember));
								tVMPMember.SetMemberTMt(tMt);
								tVMPMember.SetMemberType(byNplusMemberType);
								tVMPMember.SetMemStatus(MT_STATUS_AUDIENCE);
								tVmpParam.SetVmpMember(byChIdx,tVMPMember);
								g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tVmpParam);
															
								// 8KE,8KH,MPU2����Ҫ���ͱ�����Ϣ,Ŀǰֻ��vpu���跢�ͱ�����Ϣ,mpuҪ֧��ͨ�����ݱ�����ʾͼƬ
								if (byVmpSubType != VMP)
								{
									// [9/7/2010 xliang] 8000G ���跢��һ�α�����Ϣ��20110331 zhouyiliang 8000Hһ��
									memset(achAlias, 0, sizeof(achAlias));
									if( /*GetMtAliasInVmp(tMt, achAlias)*/
                                        GetMtAliasToVmp(tMt, achAlias) ) // ���� [7/4/2013 liaokang]
									{
										ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq][N+1]vmp member.%d info: mt(%d,%d), alias is %s!\n", 
											byChIdx, tMt.GetMcuId(), tMt.GetMtId(), achAlias);
										
										cMsg.Init();
										cMsg.SetChnIndex(byChIdx);
										cMsg.SetMsgBody( (u8*)achAlias, sizeof(achAlias) );
										cMsg.CatMsgBody(&byChIdx, sizeof(u8));
										SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_CHANGEMEMALIAS_CMD, cMsg);
									}
								}
								
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS, "[ProcMtMcuOpenLogicChnnlReq][N+1] tMt<%d, %d>, byChnnl:%d, mode:%d\n",
									tMt.GetMcuId(),tMt.GetMtId(), byChIdx, byNplusMemberType);
								

								//[Bug00056410]songkun,20110610,�趨VMPģ�壬֮ǰ�����ն�û���ߣ�����VMP�ϳɲ��ɹ���������˳�Ա��Ϣ
								// [2/22/2010 xliang] VMP��MPU����ͳһ���̣��ȵ��ֱ����ٽ�����
								if( ChangeMtVideoFormat( tMt) )
								{
									SetVmpChnnl(byVmpId, tMt, byChIdx, byNplusMemberType, TRUE );
								}

								bAdjustVmpMember = TRUE;

								//n+1���ݸ���vmp��Ϣ
								if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
								{
									ProcNPlusVmpUpdate();
								}  
							}
						}

						g_cMcuVcApp.NplusRemoveVmpMember( tMt.GetMtId(),m_byConfIdx,byVmpId);

						if(bAdjustVmpMember)
						{
							//ͬ�����踴��״̬
							g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tVmpPeriEqpStatus );
							tVmpPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);
							g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tVmpPeriEqpStatus );

						}
					}
					
				}
			}

			//�ж��Ƿ�Ϊ�ش�ͨ��,����ָ��ش�����
			if( !m_tCascadeMMCU.IsNull() && !(m_tCascadeMMCU == tMt) )
			{
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
				if( ptConfMcInfo != NULL && 
					( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
				{
					if( ptConfMcInfo->m_tSpyMt.GetMtId() == tMt.GetMtId() &&
						ptConfMcInfo->m_tSpyMt.GetMcuId() == tMt.GetMcuId() )
					{
						ptConfMcInfo->m_tSpyMt.SetNull();
						//StartSwitchToMcu( tMt, 0, m_tCascadeMMCU.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);
						//ptConfMcInfo->m_tSpyMt = tMt;
						// ֱ�ӵ���OnSetOutView����,�������ϼ�����Ϣ
						OnSetOutView(tMt, MODE_VIDEO);
					}
				}
			}

			//�е���ǽģ�岢����������豸�����ӵĻ�
			TConfAttrb tTmpConfAttrb = m_tConf.GetConfAttrb();
            if (tTmpConfAttrb.IsHasTvWallModule() )
            {
                u8 byMemberType = 0;
                for(u8 byTvPos = 0; byTvPos < MAXNUM_PERIEQP_CHNNL; byTvPos++)
                {
                    u8 byTvWallId = m_tConfEqpModule.m_tTvWallInfo[byTvPos].GetTvWallEqp().GetEqpId();
					u8 byEqpType  = m_tConfEqpModule.m_tTvWallInfo[byTvPos].GetTvWallEqp().GetEqpType();
				
					//zjl[20091208]����ģ������������hduԤ���������ն˳�Ա
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
							
							for (u8 byTvChlLp = 0; byTvChlLp < MAXNUM_TVWALL_CHNNL_INSMOUDLE; byTvChlLp++)
							{
								if( m_tConfEqpModule.IsMtInTvWallChannel( byTvWallId, byTvChlLp, tMt, byMemberType) &&
									g_cMcuVcApp.IsPeriEqpConnected(atHduChnlInfo[byTvChlLp].GetEqpId()))
								{
									ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq] HDU_SCHEME:%d, EQPID:%d, CHNNL:%d, MT:%d, MEMBERTYPE:%d\n",
										byTvWallId, atHduChnlInfo[byTvChlLp].GetEqpId(), atHduChnlInfo[byTvChlLp].GetChnlIdx(), tMt.GetMtId(), byMemberType);
									
									// [2013/03/11 chenbing] ��HDUģ�岻֧��HDU�໭��,��ͨ����0
									ChangeHduSwitch(&tMt, atHduChnlInfo[byTvChlLp].GetEqpId(),
										atHduChnlInfo[byTvChlLp].GetChnlIdx(), 0, byMemberType, TW_STATE_START);
								}
							}
						}
						else
						{
							ConfPrint(LOG_LVL_WARNING, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq] ReadHduSchemeTable failed!\n");
						}
					}
					else
					{
						if( 0 != byTvWallId && g_cMcuVcApp.IsPeriEqpConnected(byTvWallId) )
						{
							for(u8 byTvChlLp = 0; byTvChlLp < MAXNUM_PERIEQP_CHNNL; byTvChlLp++)
							{
								// ͬ�ϣ�������ʱֻ�����˱��ն˳�����Tw��Ա���档
								// �����ָ�����������棬��Ӧ���ڸı���ϯ/�����˵�ʱ���Զ������
								if( m_tConfEqpModule.IsMtInTvWallChannel( byTvWallId, byTvChlLp, tMt, byMemberType) )
								{
									if (!IsMultiCastMt(GetLocalMtFromOtherMcuMt(tMt).GetMtId()))
									{
										// ���� [5/11/2006] ֧��ָ�����͵Ŀ�ʼ��ʽ
										//ChangeTvWallSwitch(&tMt, byTvWallId, byTvChlLp, TW_MEMBERTYPE_MCSSPEC, TW_STATE_START);
										ChangeTvWallSwitch(&tMt, byTvWallId, byTvChlLp, byMemberType, TW_STATE_START);
									} 
									else
									{
										ChangeTvWallSwitch(NULL, byTvWallId, byTvChlLp, byMemberType, TW_STATE_STOP);
									}
								}
							}
						}
					}
                }

				//ɾ�����ڻ���ģ���е���ǽ��Ա����Ϣ
				RemoveMtInMutiTvWallMoudle(tMt);
            }

			//Hdu�໭��ͨ����Ա�����߼�[3/11/2013 chendaiwei]
			if(m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo())
			{
				for( u8 byHduEqpId = HDUID_MIN; byHduEqpId <= HDUID_MAX; byHduEqpId++ )
				{
					if(g_cMcuVcApp.IsPeriEqpConnected(byHduEqpId))
					{
						for(u8 byHduChIdx = 0; byHduChIdx < MAXNUM_HDU_CHANNEL; byHduChIdx++)
						{
							for(u8 bySubChIdx = 0; bySubChIdx <HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChIdx++)
							{
								u8 byVmpChnlMemberType = 0;
								if(m_tConfEqpModule.IsMtInHduVmpChnnl(byHduEqpId,byHduChIdx,bySubChIdx,tMt,byVmpChnlMemberType))
								{
									StaticLog("[ProcMtMcuOpenLogicChnnlReq]byHduEqpId.%d byHduChIdx.%d bySubChIdx.%d MtId.%d Type.%d\n",
										byHduEqpId,byHduChIdx,bySubChIdx,tMt.GetMtId(),byVmpChnlMemberType);
									// [2013/03/11 chenbing] HDU�໭��,ֻ������Ƶ(���һ�����������ķ��)
									ChangeHduSwitch(&tMt, byHduEqpId, byHduChIdx, bySubChIdx, byVmpChnlMemberType, TW_STATE_START,
										MODE_VIDEO, FALSE, TRUE, TRUE, HDUCHN_MODE_FOUR);
								}
							}
						}
					}
				}

				m_tConfEqpModule.RemoveMtInHduVmpModule(tMt);
			}

			u8 byReason = m_ptMtTable->GetDisconnectReason( GetLocalMtFromOtherMcuMt( tMt ).GetMtId());

			if( MTLEFT_REASON_EXCEPT == byReason ||
				MTLEFT_REASON_RTD == byReason ||
				MTLEFT_REASON_UNREACHABLE == byReason ||
				m_tConf.m_tStatus.m_tConfMode.GetHduInBatchPoll() != POLL_STATE_NONE  )	 
			{	
				BOOL32 bCanRestore = TRUE;
				if ( m_tConf.GetConfAttrb().IsSatDCastMode())
				{
					if ( IsMultiCastMt( tMt.GetMtId())
						&& GetCurSatMtNum() <= 0)
					{
						// ��¼��ǰ���ǵ������˴���ʼ��flowcontrol 0�����������ǵ���
						s16 byCurSatMtNum = GetCurSatMtNum();
						LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtMcuOpenLogicChnnlReq]CurSatMtNum is 0, cannot restore vmpmem or hdumem.\n");
						bCanRestore = FALSE;
						// followctrl��0
                        //zjj20131030 �����ն�ǰ���Ѿ������
						if( !(GetLocalSpeaker() == tMt) )
						{
							NotifyMtSend( tMt.GetMtId(), MODE_VIDEO, FALSE );
						}
						// ��֤���������ǵ���
						SetCurSatMtNum(byCurSatMtNum);
						LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[ProcMtMcuOpenLogicChnnlReq]RemoveMt(%d) SatMtNum(%d)\n",
							tMt.GetMtId(),GetCurSatMtNum());
					}
				}
				//if (bCanRestore)
				{
					//zjj20091014�쳣�Ҷϵĳ�Ա��Ȼ������������ڻ���ϳɵĳ�Ա��Ϣ����Ȼ����
					//�����ʱ����ϳ������ڹ㲥����ָ�����ն��뻭��ϳɽ���
					//���ﴦ���ֻ�ǹ̶������µĻָ����Զ��ϳ�ʱ�Ļָ�������
					//�����������:�Զ��ϳ�ʱ�Ƿ�Ҫ��ԭ����ͨ���лָ�����û���Թ�
					// [pengjie 2010/5/28] �����ǲ��ǻ���ϳɹ㲥����Ӧ�ûָ��ն�
					u8 byTmpVmpId;
					TEqp tTmpVmpEqp;
					TPeriEqpStatus tPeriEqpStatus;
					TVMPParam_25Mem tVmpParam;
					u8 byVmpSubType;
					u8 byChnnl;
					TVMPMember *ptVmpMember = NULL;
					s8 achAlias[MAXLEN_ALIAS] = {0};
					CServMsg cMsg;
					for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
					{
						if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
						{
							continue;
						}
						byTmpVmpId = m_abyVmpEqpId[byIdx];
						tTmpVmpEqp = g_cMcuVcApp.GetEqp( byTmpVmpId );
						
						if( !IsDynamicVmp(byTmpVmpId) &&
							(MTLEFT_REASON_EXCEPT == byReason ||
							MTLEFT_REASON_RTD == byReason ||
							MTLEFT_REASON_UNREACHABLE == byReason ) 
							)
						{
							g_cMcuVcApp.GetPeriEqpStatus( byTmpVmpId, &tPeriEqpStatus );
							byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
							
							tVmpParam = g_cMcuVcApp.GetConfVmpParam(tTmpVmpEqp);
							if( tPeriEqpStatus.m_byOnline != 0   // VMP����
								&& tVmpParam.IsMtInMember(tMt) ) // ���߳�Ա��VMP��
							{
								// 2011-11-3 add by pgf: ����tMt�ڶ��VMPͨ��ʱ������ָ�������Щͨ���Ľ���
								for (byChnnl = 0; byChnnl < MAXNUM_VMP_MEMBER; byChnnl++)
								{
									ptVmpMember = tVmpParam.GetVmpMember(byChnnl);
									
									if ( ptVmpMember == NULL || ptVmpMember->IsNull()
										|| !((TMt)(*ptVmpMember) == tMt))
									{
										continue;
									}
									// �ָܻ��򽨽��������ָܻ������Ա
									if (bCanRestore)
									{
										// �ն��쳣���ߣ����ߺ������ı�����Ȼ���ٺ�����飬�����иı���ϳ���ͨ������ʾ�����±������ٴη��ͱ���������
										// 8KE,8KH,MPU2����Ҫ���ͱ�����Ϣ,Ŀǰֻ��vpu���跢�ͱ�����Ϣ,mpuҪ֧��ͨ�����ݱ�����ʾͼƬ
										if (byVmpSubType != VMP)
										{
											memset(achAlias, 0, sizeof(achAlias));
											if(GetMtAliasToVmp(tMt, achAlias))
											{
												ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq]vmp member.%d info: mt(%d,%d), alias is %s!\n", 
													byChnnl, tMt.GetMcuId(), tMt.GetMtId(), achAlias);
												
												cMsg.Init();
												cMsg.SetChnIndex(byChnnl);
												cMsg.SetMsgBody( (u8*)achAlias, sizeof(achAlias) );
												cMsg.CatMsgBody(&byChnnl, sizeof(u8));
												SendMsgToEqp(tTmpVmpEqp.GetEqpId(), MCU_VMP_CHANGEMEMALIAS_CMD, cMsg);
											}
										}

										SetVmpChnnl(byTmpVmpId, tMt, byChnnl, ptVmpMember->GetMemberType(), TRUE);
									}
									else
									{
										ClearOneVmpMember(byTmpVmpId, byChnnl, tVmpParam);
									}
								}
								
								if ( byVmpSubType != VMP && bCanRestore)
								{
									ChangeMtVideoFormat(tMt);
								}
							}
						}
					}
					
					//zjj20091014����쳣�Ҷϵĳ�Ա�ָ�����ǽ����
					//�쳣�Ҷϵĳ�Ա��Ȼ������������ڵ���ǽ�ĳ�Ա��Ϣ����Ȼ����
					if (bCanRestore)
					{
						RestoreMtInTvWallOrHdu( tMt );
					}
					else
					{
						FindConfHduHduChnnlAndStop(&tMt, TW_MEMBERTYPE_NULL, TRUE);
					}
				}

				//��յ���ԭ��
				m_ptMtTable->SetDisconnectReason( GetLocalMtFromOtherMcuMt( tMt ).GetMtId(), 0 );
			}
			if( !m_tDoubleStreamSrc.IsNull() && 
				m_ptMtTable->GetManuId( tMt.GetMtId() ) != MT_MANU_KDC &&
				m_ptMtTable->GetSrcSCS(tMt.GetMtId()).GetVideoMediaType() != MEDIA_TYPE_NULL )
			{
				TLogicalChannel tMvLogicalChannel;
				if (m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tMvLogicalChannel, TRUE) &&
					tMvLogicalChannel.GetFlowControl() > 0)
				{
					tMvLogicalChannel = tLogicChnnl;
					u16 wBitrate = 0;
					const u16 wSendBand = m_ptMtTable->GetSndBandWidth(tMt.GetMtId());
					const u16 wRcvBand = m_ptMtTable->GetRcvBandWidth(tMt.GetMtId());
					wBitrate = min (wSendBand ,  wRcvBand);
					wBitrate = GetDoubleStreamVideoBitrate(wBitrate);
					tMvLogicalChannel.SetFlowControl(wBitrate);
					cServMsg.SetMsgBody((u8*)&tMvLogicalChannel, sizeof(tMvLogicalChannel));
					SendMsgToMt(tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
				}
			}
			/* �˶β���Ҫ������ ����VMP�Զ�������Ὠ�õ�
			if( VCS_CONF == m_tConf.GetConfSource() && tMt == m_tConf.GetChairman() )
			{
				TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpParam();
				if( (VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode() ||
					VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode()) &&
					tVMPParam.IsMtInMember( tMt )
					)
				{
					
					BOOL32 bRet = ChangeMtVideoFormat( tMt, &tVMPParam, 
											TRUE, FALSE );
					if(bRet)
					{
						SetVmpChnnl(tMt, tVMPParam.GetChlOfMtInMember(tMt), 
							tVMPParam.GetVmpMember( tVMPParam.GetChlOfMtInMember(tMt) )->GetMemberType()
							);
					}															
				}
			}*/

			// vrs��¼������֧��
			if (tMt.GetMtType() == MT_TYPE_VRSREC)
			{
				if (m_tPlayEqp.GetMtId() == tMt.GetMtId())
				{
					if (!(GetLocalSpeaker() == tMt))
					{
						//������¼��ʵ��Ϊ������
						ChangeSpeaker(&tMt, FALSE, FALSE, FALSE, TRUE);
					}
					//׼������
					m_tConf.m_tStatus.SetPlayReady();
					m_ptMtTable->SetRecChlState(tMt.GetMtId(), TRecChnnlStatus::STATE_PLAYREADY);
					// �����ȴ�����������ʱ��
					SetTimer( MCUVC_RECPLAY_WAITMPACK_TIMER, TIMESPACE_WAIT_PLAY_SWITCHOK );
				}
				else
				{
					ReleaseVrsMt(tMt.GetMtId());
				}
			}

		}
	
		//������Ƶ��������	
		if( tLogicChnnl.GetMediaType() == MODE_AUDIO )
		{
			if( VCS_CONF == m_tConf.GetConfSource() && 
				m_cVCSConfStatus.GetMtInTvWallCanMixing() &&
				(  VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() || 
				   VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() 
				))
			{
				m_ptMtTable->SetMtInMixing(tMt.GetMtId(),TRUE,FALSE);
			}
			//��������, ת��dump����ֹicmp
			g_cMpManager.StartRecvMt( tMt, MODE_AUDIO );
            
            //zbq[07/18/2007] SSRC ���޸��ɹ�һ����ͳһ����
			//g_cMpManager.ResetRecvMtSSRC( tMt, MODE_AUDIO );

			//���Ƿ����˼���, ���ǵ�����Ƶʱ�ķ�����Ҳ��˴���
			if(GetLocalSpeaker() == tMt )
			{
				//����Ƶ���迼����Ƶ�������
				//zhouyiliang 20121016����ǻָ������ˣ���ǰ������ѯ����Ƶ������ͣ����ʱbPolling ҪΪTRUE����ͣ��ѯ
				BOOL32 bPolling = FALSE;
				if ( m_tConf.m_tStatus.GetPollMedia() == MODE_BOTH )
				{
					bPolling = TRUE;
				}
				ChangeSpeaker( &tMt, bPolling, FALSE );		
			}

            // xsl [8/4/2006] vac������ģʽ�½����ն˼������ͨ�������������������Ľ���
			if(   m_tConf.m_tStatus.IsVACing() ||
                    m_tConf.m_tStatus.IsAutoMixing() || 
                    ( m_tConf.m_tStatus.IsSpecMixing() && (tMt == GetLocalSpeaker() || m_ptMtTable->IsMtInMixing( tMt.GetMtId())) &&
						// zhouyiliang 20101213 vcs���鱸����ϯ��������
						(m_tConf.GetConfSource() != VCS_CONF || !(tMt == m_cVCSConfStatus.GetVcsBackupChairMan()) )
						)
						)
			{
                // guzh [1/31/2007] 8000B ��������
                u16 wError = 0;
                if ( CMcuPfmLmt::IsMtOprSupported( m_tConf, m_tConfAllMtInfo.GetLocalJoinedMtNum(), GetMixMtNumInGrp(), wError ) )
                {
                    if (GetMixMtNumInGrp() < GetMaxMixNum(m_tMixEqp.GetEqpId()) ||
						(GetMixMtNumInGrp()== GetMaxMixNum(m_tMixEqp.GetEqpId()) && 
						m_ptMtTable->IsMtInMixGrp(tMt.GetMtId())))
					{
						// miaoqingsong [05/11/2011] ����"������Ա����Ƿ�ɹ�"�жϣ���ֹ�������ն˵�����������Ч����
						if( AddMixMember( &tMt, DEFAULT_MIXER_VOLUME, FALSE ) ||
							m_tConf.m_tStatus.IsVACing())
						{
							StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
								(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt)),
								MODE_AUDIO, SWITCH_MODE_SELECT);
						}
					} 
					else
					{
						// ������ʾ�������趨���Ծ���ִ��
						//20100708_tzy ֮ǰ���¼��ն�����������������ʱ���ᱣ��ԭ�����䡣
						//�������Ϊ��������ʱ�¼��ն���ʹ�����������������򽫻������۱�Ϊ���ƻ�������������ʾ
						//�������������������������ֹͣ��������Ӧ��ʾ
						//ֻ�е����ڻ������ۻ��߻�����������������ͬʱ����ʱ���Ż�����л�
						if (m_tConf.m_tStatus.GetMixerMode() != mcuVacMix)
						{                        
							SwitchDiscuss2SpecMix();
							wError = ERR_MCU_DISCUSSAUTOCHANGE2SPECMIX;
							NotifyMcsAlarmInfo(0, wError);  // ��ʾMCS
						}
						else
						{
							StopMixing();
						}
					}
                }
			}
			else
			{
				NotifyMtSend( tMt.GetMtId(), MODE_AUDIO, FALSE );
			}

			//�ж��Ƿ�Ϊ�ش�ͨ��,����ָ��ش�����
			if( !m_tCascadeMMCU.IsNull() && !(m_tCascadeMMCU == tMt) && 
				!m_tConf.m_tStatus.IsMixing() )
			{
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
				if( ptConfMcInfo != NULL && 
					( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
				{
					if( ptConfMcInfo->m_tSpyMt.GetMtId() == tMt.GetMtId() &&
						ptConfMcInfo->m_tSpyMt.GetMcuId() == tMt.GetMcuId() )
					{
						ptConfMcInfo->m_tSpyMt.SetNull();
						StartSwitchToMcu( tMt, 0, m_tCascadeMMCU.GetMtId(), MODE_AUDIO, SWITCH_MODE_SELECT);
						ptConfMcInfo->m_tSpyMt = tMt;
					}
				}
			}
			if( VCS_CONF ==	m_tConf.GetConfSource() &&
				MT_TYPE_MT == m_ptMtTable->GetMtType(tMt.GetMtId()) &&
				(m_tConf.GetChairman().GetMtId() != tMt.GetMtId() || !m_tCascadeMMCU.IsNull()))
			{
				VCSMTMute(tMt, m_cVCSConfStatus.IsRemMute(), VCS_AUDPROC_MUTE);					
				VCSMTMute(tMt, m_cVCSConfStatus.IsRemSilence(), VCS_AUDPROC_SILENCE);
			}
			
			// [2013/04/27 chenbing] ���ģ�忪��������û����Ƶ���� 
			if (MCS_CONF ==	m_tConf.GetConfSource())
			{
				//��Ҫ�ж��Ƿ���HDUͨ����
				TDoublePayload tDAPayload;
				TPeriEqpStatus tHduStatus;
				CServMsg cSMsg;
				for (u8 byHduId=HDUID_MIN; byHduId<=HDUID_MAX; byHduId++)
				{
					g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
					if (!tHduStatus.m_byOnline)
					{
						continue;
					}

					for (u8 byHduChnId=0; byHduChnId<MAXNUM_HDU_CHANNEL; byHduChnId++)
					{
						TMt tMtInHduVmp = (TMt)tHduStatus.m_tStatus.tHdu.GetChnMt(byHduChnId);
						if( (tMtInHduVmp.GetConfIdx() == m_byConfIdx) && (tMtInHduVmp == tMt) )
						{
							u8 byAudioNum = tLogicChnnl.GetAudioTrackNum();
							u8 bIsNeedPrs = m_tConf.GetConfAttrb().IsResendLosePack();

							tDAPayload.SetRealPayLoad(m_ptMtTable->GetSrcSCS(tMt.GetMtId()).GetAudioMediaType());
							tDAPayload.SetActivePayload(m_ptMtTable->GetSrcSCS(tMt.GetMtId()).GetAudioMediaType());
							cSMsg.SetChnIndex(byHduChnId);
							cSMsg.SetMsgBody((u8 *)&byAudioNum, sizeof(u8));
							cSMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload));

							ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq] MCU_HDU_FRESHAUDIO_PAYLOAD_CMD\n");
							ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_EQP, "HduId(%d) ChnId(%d) AudioNum(%d) AudioRealPayLoad(%d) AudioActivePayLoad(%d)!!!\n",
										byHduId, byHduChnId, byAudioNum, tDAPayload.GetRealPayLoad(), tDAPayload.GetActivePayload());
							//������Ƶ�غ�
							SendMsgToEqp(byHduId, MCU_HDU_FRESHAUDIO_PAYLOAD_CMD, cSMsg);
						}
					}
				}
			}
		}	
		
		//��ĳ�߼�ͨ�������������,������radvision ˫������
		if( tLogicChnnl.GetMediaType() == MODE_VIDEO &&
			m_ptMtTable->GetManuId(tMt.GetMtId()) == MT_MANU_RADVISION)
		{
			TCapSupport tCapSupport = m_tConf.GetCapSupport();
			// MCUǰ�����, zgc, 2007-09-27
			TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();

            TSimCapSet tSim = tCapSupport.GetMainSimCapSet();
            tSim.SetVideoMaxBitRate(m_ptMtTable->GetDialBitrate( tMt.GetMtId()));
            tCapSupport.SetMainSimCapSet(tSim);
            tSim = tCapSupport.GetSecondSimCapSet();
            if(!tSim.IsNull())
            {
                tSim.SetVideoMaxBitRate(m_ptMtTable->GetDialBitrate( tMt.GetMtId()));
                tCapSupport.SetSecondSimCapSet(tSim);
            }			
            tCapSupport.SetDStreamMaxBitRate(m_ptMtTable->GetDialBitrate( tMt.GetMtId()));
            
			u8 bySupportActiveH263 = 1; // �Ƿ����û�غɱ�ʶ
			u8 bySendMSDetermine   = 0; // �Ƿ������Ӿ���
	
			cServMsg.SetMsgBody( ( u8 * )&tCapSupport, sizeof( tCapSupport ) );
			cServMsg.CatMsgBody( ( u8 * )&bySupportActiveH263, sizeof( bySupportActiveH263 ) );
			cServMsg.CatMsgBody( ( u8 * )&bySendMSDetermine, sizeof( bySendMSDetermine ) );
			// MCUǰ�����, zgc, 2007-09-27
			cServMsg.CatMsgBody( ( u8 * )&tCapSupportEx, sizeof( tCapSupportEx ) );

            //[6/16/2011 zhushz]iMax����ȡ�����ļ�������֡��
            /*u8 byFpsAdp = 0;
            if (m_tConf.GetConfAttrbEx().IsImaxModeConf())
            {
                byFpsAdp = g_cMcuVcApp.GetFpsAdp();
            }            
	        cServMsg.CatMsgBody(&byFpsAdp, sizeof(u8));
            */

			// ��֯��չ��������ѡ��Mtadp [12/8/2011 chendaiwei]
			TVideoStreamCap atMSVideoCap[MAX_CONF_CAP_EX_NUM];
			u8 byCapNum = MAX_CONF_CAP_EX_NUM;
			m_tConfEx.GetMainStreamCapEx(atMSVideoCap,byCapNum);
			
			TVideoStreamCap atDSVideoCap[MAX_CONF_CAP_EX_NUM];
			u8 byDSCapNum = MAX_CONF_CAP_EX_NUM;
			m_tConfEx.GetDoubleStreamCapEx(atDSVideoCap,byDSCapNum);

			cServMsg.CatMsgBody((u8*)&atMSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
			cServMsg.CatMsgBody((u8*)&atDSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
			
			//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
			//u8 byAudioTrackNum = tAudioType.GetAudioTrackNum();
			//cServMsg.CatMsgBody(&byAudioTrackNum,sizeof(byAudioTrackNum));
			TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//��Ƶ����
			//�ӻ���������ȡ������֧�ֵ���Ƶ����
			m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);	
			cServMsg.CatMsgBody((u8*)&atAudioTypeDesc[0], sizeof(TAudioTypeDesc)* MAXNUM_CONF_AUDIOTYPE);

			SendMsgToMt( tMt.GetMtId(), MCU_MT_CAPBILITYSET_NOTIF, cServMsg );
		}

		// ����VCS���飬��һ·��Ƶ����Ƶ����ͨ���򿪺󣬼���������һ�����Ȳ���
		if (VCS_CONF == m_tConf.GetConfSource() &&
			!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
		{
			u8 tMediaType = tLogicChnnl.GetMediaType();
			if (MODE_VIDEO == tMediaType || MODE_AUDIO == tMediaType)
			{
				GoOnSelStep(tMt, tMediaType, FALSE);
			}			
			
			if( m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE && tMt == m_tConf.GetChairman() &&
				CONF_CREATE_MT != m_byCreateBy )
			{
				TSwitchInfo tSwitchInfo;
				TMt tMtDst;
				for( u8 byLoop = 1;byLoop <= MAXNUM_CONF_MT; ++byLoop )
				{
					if( !m_tConfAllMtInfo.MtJoinedConf(byLoop) ||
						m_cVCSConfStatus.GetCurVCMT().GetMtId() == byLoop ||
						m_cVCSConfStatus.GetVcsBackupChairMan().GetMtId() == byLoop )
					{
						continue;
					}
					
					tMtDst = m_ptMtTable->GetMt( byLoop );
					tSwitchInfo.SetMode( tMediaType );
					tSwitchInfo.SetDstMt( tMtDst );
					tSwitchInfo.SetSrcMt( tMt );
					if (!VCSConfSelMT(tSwitchInfo))
					{
						NotifyMtReceive(tMtDst, tMtDst.GetMtId());
					}
				}
			}
		}

		break;
        }
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*==============================================================================
������    :  AdjustCtrlModeVmpModuleByMt
����      :  �ն˿�ͨ������ģ�濪���Ļ��ָ������ϳ�
			 �����ն˿��߼�ͨ��ʱ����ģ�濪���Ļ���ϳɣ�����Ӧͨ��������
�㷨ʵ��  :  
����˵��  :  TMt &tMt
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013-04					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::AdjustCtrlModeVmpModuleByMt(const TMt &tMt)
{
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();

	// ֻ��Ա����ն˿�ͨ��ʱ����,ProcMtMcuOpenLogicChnnlReq����
	if (tMt.IsNull() || !tMt.IsLocal())
	{
		return;
	}

	// ֻ����ģ�濪�����Զ�����ϳ�
    if( tConfAttrb.IsHasVmpModule() && 
        !m_tConfInStatus.IsVmpModuleUsed() &&
		g_cMcuVcApp.GetVMPMode(m_tModuleVmpEqp) != CONF_VMPMODE_NONE &&
		!IsDynamicVmp(m_tModuleVmpEqp.GetEqpId())
		)
    {
		u8 byVmpSubType = GetVmpSubType(m_tModuleVmpEqp.GetEqpId());
		TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tModuleVmpEqp);
        u8 byChlNum = tVmpParam.GetMaxMemberNum();
        u8 byMemberType = 0;
		u8 byVmpStyle = tVmpParam.GetVMPStyle();
		TMt tSpeaker = m_tConf.GetSpeaker();
		TVMPMember tVMPMember;
		TVMPMember *ptVmpMem = NULL;
		//s8 achAlias[VALIDLEN_ALIAS+1] = {0};
        s8 achAlias[MAXLEN_ALIAS] = {0}; // ���� [7/4/2013 liaokang]
		CServMsg cMsg;
		TMt tNullMt;
        for( u8 byLoop = 0; byLoop < byChlNum; byLoop++ )
        {
			tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tModuleVmpEqp);//�ϳɳ�Ա�����
            if( m_tConfEqpModule.IsMtAtVmpChannel( byLoop, tMt, byMemberType ) )
            {
				// ���ն�����ǰ,��ͨ����Ա�ѱ�,�����ٻָ���ͨ����Ա
				tVMPMember = *tVmpParam.GetVmpMember(byLoop);
				if (VMP_MEMBERTYPE_NULL != tVMPMember.GetMemberType()
					&& byMemberType != tVMPMember.GetMemberType())
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq]vmp member.%d membertype: %d, is not Module membertype%d, can't into vmp!\n", 
						byLoop, tVMPMember.GetMemberType(), byMemberType);
					m_tConfEqpModule.SetVmpMember( byLoop, tNullMt );//���ٻָ�vmpģ���еĴ�ͨ��
					continue;
				}
				if (!tVMPMember.IsNull() && !(tVMPMember == tMt))
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq]vmp member.%d info: mt(%d,%d), is not openlogic mt(%d,%d), can't into vmp!\n", 
						byLoop, tVMPMember.GetMcuId(), tVMPMember.GetMtId(), tMt.GetMcuId(), tMt.GetMtId());
					m_tConfEqpModule.SetVmpMember( byLoop, tNullMt );//���ٻָ�vmpģ���еĴ�ͨ��
					continue;
				}
				// ��Է����˸���,����ǰ�������Ѳ���ģ�淢�����ն�,���ָ������˸���ͨ����Ա
				// ����:ģ�濪��ʱ,�������ն�δ����,�ı䷢������ȡ�������˺�,���ն�������,���ն˲���䷢����,��ģ�淢���˸���ͨ�����д��ն�
				if (VMP_MEMBERTYPE_SPEAKER == tVMPMember.GetMemberType() && 
					!(tMt == tSpeaker))
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq]vmp member.%d info: SpeakerMt(%d,%d), is not openlogic mt(%d,%d), can't into vmp!\n", 
						byLoop, tSpeaker.GetMcuId(), tSpeaker.GetMtId(), tMt.GetMcuId(), tMt.GetMtId());
					m_tConfEqpModule.SetVmpMember( byLoop, tNullMt );//���ٻָ�vmpģ���еĴ�ͨ��
					continue;
				}
				// ����confVmpParam�ϳɳ�Ա
                memset(&tVMPMember, 0, sizeof(tVMPMember));
                tVMPMember.SetMemberTMt(tMt);
                tVMPMember.SetMemberType(byMemberType);
                tVMPMember.SetMemStatus(MT_STATUS_AUDIENCE);
                tVmpParam.SetVmpMember(byLoop,tVMPMember);
				// ���ǻ����ж��ն����� [pengguofeng 1/16/2013]
				if ( m_tConf.GetConfAttrb().IsSatDCastMode())
				{
					if ( IsMultiCastMt( tMt.GetMtId())
						//&& IsSatMtOverConfDCastNum(tMt))
						&& !IsSatMtCanContinue(tMt.GetMtId(),emVmpChnChange,&m_tModuleVmpEqp,byLoop, 0xFF, &tVmpParam))
					{
						LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtMcuOpenLogicChnnlReq]vmp chnnl:%d member:%d is Sat Mt and Over Max, clear it\n",
							byLoop, tMt.GetMtId());
						continue;
					}
				}
				// ����confVmpParam�ϳɳ�Ա
				g_cMcuVcApp.SetConfVmpParam(m_tModuleVmpEqp, tVmpParam);
                UpdataVmpEncryptParam(tMt, byLoop);
				
				// 8KE,8KH,MPU2����Ҫ���ͱ�����Ϣ,Ŀǰֻ��vpu���跢�ͱ�����Ϣ,mpuҪ֧��ͨ�����ݱ�����ʾͼƬ
				if (byVmpSubType != VMP)
				//if (GetVmpSubType(m_tVmpEqp.GetEqpId()) == VMP_8KE || GetVmpSubType(m_tVmpEqp.GetEqpId()) == VMP_8KH)
				{
					// [9/7/2010 xliang] 8000G ���跢��һ�α�����Ϣ��20110331 zhouyiliang 8000Hһ��
					memset(achAlias, 0, sizeof(achAlias));
					if( /*GetMtAliasInVmp(tMt, achAlias) */
                        GetMtAliasToVmp(tMt, achAlias) )
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq]vmp member.%d info: mt(%d,%d), alias is %s!\n", 
							byLoop, tMt.GetMcuId(), tMt.GetMtId(), achAlias);
						
						cMsg.Init();
						cMsg.SetChnIndex(byLoop);
						cMsg.SetMsgBody( (u8*)achAlias, sizeof(achAlias) );
						cMsg.CatMsgBody(&byLoop, sizeof(u8));
						SendMsgToEqp(m_tModuleVmpEqp.GetEqpId(), MCU_VMP_CHANGEMEMALIAS_CMD, cMsg);
					}
				}

                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuOpenLogicChnnlReq] tMt<%d, %d>, byChnnl:%d, mode:%d\n",
                    tMt.GetMcuId(),tMt.GetMtId(), byLoop, byMemberType);

				// ������ϳ�ͨ��ʹ���·�ʽ�����ֱ���+������
				ChangeMtResFpsInVmp(m_tModuleVmpEqp.GetEqpId(), tMt, &tVmpParam, TRUE, tVMPMember.GetMemberType(), byLoop, TRUE);

				//n+1���ݸ���vmp��Ϣ
				if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
				{
					ProcNPlusVmpUpdate();
				}                     
            }
        }		
		
		//ͬ�����踴��״̬
		TPeriEqpStatus tVmpPeriEqpStatus;
        g_cMcuVcApp.GetPeriEqpStatus( m_tModuleVmpEqp.GetEqpId(), &tVmpPeriEqpStatus );
        tVmpPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);
        g_cMcuVcApp.SetPeriEqpStatus( m_tModuleVmpEqp.GetEqpId(), &tVmpPeriEqpStatus );
    }
	return;
}

/*=============================================================================
  �� �� ���� ProcMtMcuMediaLoopOpr
  ��    �ܣ� �ն�Զ�˻�����Ϣ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage *pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::ProcMtMcuMediaLoopOpr( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TLogicalChannel *ptChan = (TLogicalChannel *)cServMsg.GetMsgBody();
    TMt tLoopMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
    u8 byMode = ptChan->GetMediaType();
    TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus(tLoopMt.GetMtId(), &tMtStatus);

	switch( CurState() )
	{
	case STATE_ONGOING:
		//����
		if (pcMsg->event == MT_MCU_MEDIALOOPON_REQ)
		{              
            //˫��ģʽ�²�������
            if (!m_tDoubleStreamSrc.IsNull())
            {
                cServMsg.SetEventId(pcMsg->event+2);
                SendMsgToMt(cServMsg.GetSrcMtId(), pcMsg->event+2, cServMsg);
                ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuMediaLoopOpr] mt can't loop in double stream mode.\n");
                return;
            }
			//zjl 20110510 Mp:StartSwitchToAll �滻 StartSwitchToSubMt
            //if (!g_cMpManager.StartSwitchToSubMt(tLoopMt, 0, tLoopMt, byMode, SWITCH_MODE_SELECT))
			TSwitchGrp tSwitchGrp;
			tSwitchGrp.SetSrcChnl(0);
			tSwitchGrp.SetDstMtNum(1);
			tSwitchGrp.SetDstMt(&tLoopMt);
			if (!g_cMpManager.StartSwitchToAll(tLoopMt, 1, &tSwitchGrp, byMode, SWITCH_MODE_SELECT))
            {
                cServMsg.SetEventId(pcMsg->event+2);
                SendMsgToMt(cServMsg.GetSrcMtId(), pcMsg->event+2, cServMsg);
                ConfPrint(LOG_LVL_WARNING, MID_MCU_MT,  "[ProcMtMcuMediaLoopOpr] start switch failed, byMode<%d>!\n", byMode );
                return;
            }

            //ack
            cServMsg.SetEventId(pcMsg->event+1);
            SendMsgToMt(cServMsg.GetSrcMtId(), pcMsg->event+1, cServMsg);
            
            NotifyMtReceive(tLoopMt, tLoopMt.GetMtId());

            //�û��ر��
            tMtStatus.SetMediaLoop(byMode, TRUE);
            m_ptMtTable->SetMtStatus(tLoopMt.GetMtId(), &tMtStatus);

            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT,  "[ProcMtMcuMediaLoopOpr] mt<%d> mode<%d> start media loop.\n", tLoopMt.GetMtId(), byMode );
		}
		//�ر�
		else
		{
            if (tMtStatus.IsMediaLoop(byMode))
            {
                tMtStatus.SetMediaLoop(byMode, FALSE);
                m_ptMtTable->SetMtStatus(tLoopMt.GetMtId(), &tMtStatus);

                //�ָ�����ý��Դ
                RestoreRcvMediaBrdSrc(tLoopMt.GetMtId(), byMode, FALSE);

                //�ָ����ջ�����
                // xsl [8/4/2006] ���ƻ�������nģʽ��������������������ͨ��
                if (m_tConf.m_tStatus.IsSpecMixing())
                {
                    SwitchMixMember(&tLoopMt, TRUE);
                }
                else
                {
                    if (GetMixMtNumInGrp() < GetMaxMixNum(m_tMixEqp.GetEqpId()))
                    {
                        AddMixMember( &tLoopMt, DEFAULT_MIXER_VOLUME, FALSE );
						StartSwitchToPeriEqp(tLoopMt, 0, m_tMixEqp.GetEqpId(), 
											 (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tLoopMt)), 
											 MODE_AUDIO, SWITCH_MODE_SELECT);	
                    }                    
                }           

                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT,  "[ProcMtMcuMediaLoopOpr] mt<%d> mode<%d> stop media loop.\n", tLoopMt.GetMtId(), byMode );
            }
	        else
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT,  "[ProcMtMcuMediaLoopOpr] mt<%d> mode<%d> not in media loop. ignore it!\n", 
                        tLoopMt.GetMtId(), byMode );
            }
		}
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcMtMcuMediaLoopOpr] Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*=============================================================================
    �� �� ���� ProcMtMcuGetH239TokenReq
    ��    �ܣ� �ն˸�MCU�� ��ȡ H239���� Ȩ������
    �㷨ʵ�֣� 
			   20110525 zjl 

			   1. ����������ƻ��ߺͱ�����ǰ���ƻ�ӵ������ͬ����ܾ���

			   2. �������߱������ģ�����õ�˫�����(�������˻������ն�)����������ʧ��

			   3. �ͷ��ϵ����ƻ�ӵ���ߣ����������ն˻���mcu

	              ע�⣺
	              a.��������Ϣ���նˣ��ն���Ȼ��Ϊ�Լ������ƻ�ӵ���ߣ�������µ�˫��Դʱ���ᵼ��mcu����˫��ͨ�����ܾ���

	              b.����ſ��ƴ�mcu�����ƣ���Ҫ��Ϊ�˼�������˫��ʱ����˫����mcu������һ��mcuȥ�ͷ����ƻ���

	               ��Ϣreleasetoken��mtadp��ת��tokenreq, ��mcu�յ�����Ϣ��ֱ�����ñ�����ǰ��˫�����ն��ͷ����ƻ���

	           4. ֹͣ����˫��, ע��StopDoubleStream�л���� ClearH239TokenOwnerInfo�������ٷ�һ��releasetoken��
				  ���ڱ��ӿ����Ѿ��ͷŹ�һ����, ��ѱ��ӿ��µ�releasetokenɾ�����������ԭ���Ƿ��б�����ã���ʱ������
				  ������Ҳû���⣬���ӿ���ڻᱣ����
	
			   5. �������ƻ�ӵ���߲�ͨ�����������նˡ�
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/6/8    3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMtMcuGetH239TokenReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	TH239TokenInfo tSrcH239Info = *((PTH239TokenInfo)(cServMsg.GetMsgBody()));
	
	//[2012/22/8 zhangli]����˫ѡ���ܾ��ٷ���˫�� 
	if (!GetSecVidBrdSrc().IsNull())
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMtMcuGetH239TokenReq] m_tSecVidBrdSrc is not NULL,nack!\n");
		
		cServMsg.SetErrorCode(ERR_MCU_SECSPEAKER_SECSPEAKERNOTNULL);
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		
		return;
	}

	// MCU����Ϊ��Token�Ĺ���Ȩ�ޣ�����Ҫ��������ȷ��
// 	if (MT_MANU_KDCMCU == m_ptMtTable->GetManuId(tMt.GetMtId()))
// 	{
// 		return;
// 	}

	if( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) && 
		!g_cMcuVcApp.IsPolyComMtCanSendDoubleStream() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT2, "[ProcMtMcuGetH239TokenReq] Polycom Mt.%d can't send double stream\n",tMt.GetMtId() );
		cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
	    cServMsg.CatMsgBody((u8 *)&tSrcH239Info, sizeof(TH239TokenInfo));
		cServMsg.SetEventId(MCU_MT_GETH239TOKEN_NACK);	
	    SendMsgToMt( tMt.GetMtId(), MCU_MT_GETH239TOKEN_NACK, cServMsg);
		return;
	}
	
	if (m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt)
	{
		if( MT_MANU_KDCMCU != m_ptMtTable->GetManuId(m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId()) 
				 && MT_MANU_KDC != m_ptMtTable->GetManuId(m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId())
				 && m_tH239TokenOwnerInfo.GetChannelId() != tSrcH239Info.GetChannelId())
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuGetH239TokenReq] No keda Mt(Mcu) req H239 Token after ds has started!\n");
		}
		else
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMtMcuGetH239TokenReq] MT:%d has been tokenMT, so return!\n", tMt.GetMtId());
			
			return;
		}
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMtMcuGetH239TokenReq] Rcv MT:%d tokenReq!\n", tMt.GetMtId());

    //�ж�˫���ķ���Դ
	if (CheckDualMode(tMt)) 
	{
    }
    else
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT2,  "[ProcMtMcuGetH239TokenReq] MtId.%d, ManuId.%d, DStreamMtId.%d open double stream with Wrong DualMode, NACK !\n", 
			tMt.GetMtId(), m_ptMtTable->GetManuId( tMt.GetMtId() ), m_tDoubleStreamSrc.GetMtId() );	

		cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
	    cServMsg.CatMsgBody((u8 *)&tSrcH239Info, sizeof(TH239TokenInfo));
        cServMsg.SetEventId(MCU_MT_GETH239TOKEN_NACK);	
	    SendMsgToMt( tMt.GetMtId(), MCU_MT_GETH239TOKEN_NACK, cServMsg);
        return;
	}

	//REQUEST RELEASING TOKEN OF LAST MT OWNER
	if( !m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() )
	{
		if( !(m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt) /*&& 
			  MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tMt.GetMtId())*/)
		{
			TH239TokenInfo tH239Info;
			tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
			tH239Info.SetSymmetryBreaking( 0 );
			cServMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
			cServMsg.SetMsgBody((u8 *)&m_tH239TokenOwnerInfo.m_tH239TokenMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId(), MCU_MT_RELEASEH239TOKEN_CMD, cServMsg);
		}
		/*������KEDAMCU�ϵ�ĳ�����鷢��˫����û��������һ�����ƻ�֪ͨ��ʱ����������MCU�������ƻ�֪ͨ������������KEDAMCU�յ���֪ͨʱ���������������˫����
		�����������ܻᵼ�¼��������еĸ����������˫����һ�¡���Ը����������һ��ȷ�ϻ��ơ���֤���������˫��һ���ԡ�
		��������:�����ǰ���ƻ�ӵ������KEDAMCU���µ��ն����������ƻ�ʱֻ�Ǽ�¼�¸����������ߵ�ID��������������ACK�����������ʱ��˫��Դ����KEDAMCU�������ͷ�����������Ϣ��
		����Ϣ��MTADP���ת���ɻ�ȡ����������Ϣ���Է��յ���һ�����ƻ�����ȷ����Ϣ���յ�����MTADP���ת�����ͷ�����֪ͨ����������ҵ���յ��������ͷ�֪ͨ���������������¼
		�ն˻�ȷ����Ϣ���Ա��ø��ն���˳������˫����*/
		if (MT_MANU_KDCMCU == m_ptMtTable->GetManuId(m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId()))
		{
			//�����û��֮ǰ�������Ƶ��ն˻�����ȷ��ʱ���յ��������ն˵������������Ժ�����Ϊ׼�����ܾ�֮ǰ�ն˵���������
			if( 0 != m_byNewTokenReqMtid &&  m_byNewTokenReqMtid != tMt.GetMtId() &&
				MT_TYPE_MT == m_ptMtTable->GetManuId(m_byNewTokenReqMtid) )
			{
				cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
				cServMsg.CatMsgBody((u8 *)&m_tH239TokenOwnerInfo, sizeof(TH239TokenInfo));
				cServMsg.SetEventId(MCU_MT_GETH239TOKEN_NACK);	
				SendMsgToMt( m_byNewTokenReqMtid, MCU_MT_GETH239TOKEN_NACK, cServMsg);	
			}
			m_byNewTokenReqMtid = tMt.GetMtId();
			//����µ����������ն˲���KEDAMCU��ֻ�Ǽ�¼���ն�ID�����յ�ԭ˫��Դ�������ͷ�֪ͨ���ٸ����ն˻�����ȷ����Ϣ
			if( MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tMt.GetMtId()) )
			{
				m_tH239TokenOwnerInfo.SetChannelId( tSrcH239Info.GetChannelId() );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2,"[ProcMtMcuGetH239TokenReq]Current TokenMt(%d) Is KEDAMCU New Token ReqMt(%d)chnIdx(%d),So Return!!\n",
					m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId(),m_byNewTokenReqMtid,tSrcH239Info.GetChannelId());	
				return;
			}
		}
	}
	SetTimer( MCUVC_WAITOPENDSCHAN_TIMER, 3000);
	if( !m_tDoubleStreamSrc.IsNull() )
	{
		//SONY:------FIRST OPENCHANNEL,NEXT REQUEST TOKEN
		//CLOSE LAST CHANNELS
		if( !(m_tDoubleStreamSrc == tMt) )
		{
			StopDoubleStream( TRUE, FALSE, FALSE);//˫��Դ�л�ʱ��ͣ˫��������vmp����˫��Դ����ʱ�����
		}
	}	

    cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
	cServMsg.CatMsgBody((u8 *)&tSrcH239Info, sizeof(TH239TokenInfo));	
		
	//POLYCOM7000�ľ�̬˫��ʱ,����ͨ����һ��ʼ�򿪺�һֱ���رգ�ʹ��H239Token���з��ͼ����ر�
	//IF CHANNEL HAS BEEN OPENED, START DOUBLESTREAM,
	//ELSE IF TOKEN HAS BEEN OWNED, START DOUBLESTREAM WHEN RECEIVED OPENLOGICAL_ERQ...
	TLogicalChannel tH239LogicChnnl;
	if( ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) ||
		 ( MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
		  MT_TYPE_MT != m_ptMtTable->GetMtType(tMt.GetMtId()) ) )
		 && m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ) )
	{
        // xsl [7/20/2006]���Ƿ�ɢ����ʱ��Ҫ���ǻش�ͨ����
        if (m_tConf.GetConfAttrb().IsSatDCastMode() && /*IsOverSatCastChnnlNum(tMt.GetMtId())*/
			IsMultiCastMt(tMt.GetMtId())
			&& //IsSatMtOverConfDCastNum(tMt))
			!IsSatMtCanContinue(tMt.GetMtId(),emstartDs))
        {
            ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuGetH239TokenReq] over max upload mt num.\n");
            cServMsg.SetEventId(MCU_MT_GETH239TOKEN_NACK);	
			cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tSrcH239Info, sizeof(TH239TokenInfo));
	        SendMsgToMt( tMt.GetMtId(), MCU_MT_GETH239TOKEN_NACK, cServMsg);
            return;
        }

		StartDoubleStream( tMt, tH239LogicChnnl );
	}	

    //RESPONSE TOKEN TO NEW MT OWNER
	cServMsg.SetEventId(MCU_MT_GETH239TOKEN_ACK);	
	SendMsgToMt( tMt.GetMtId(), MCU_MT_GETH239TOKEN_ACK, cServMsg);

    //SEND OTHER MT TOKEN INDICATION
    m_tH239TokenOwnerInfo.SetChannelId( tSrcH239Info.GetChannelId() );
	m_tH239TokenOwnerInfo.SetSymmetryBreaking( tSrcH239Info.GetSymmetryBreaking() );
	m_tH239TokenOwnerInfo.SetTokenMt( tMt );
	NotifyH239TokenOwnerInfo( NULL );
    
	return;
}

/*=============================================================================
    �� �� ���� ProcMtMcuOwnH239TokenNotify
    ��    �ܣ� �ն˸�MCU�� ӵ�� H239���� Ȩ��֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/6/8    3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMtMcuOwnH239TokenNotify( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	TH239TokenInfo tSrcH239Info = *((PTH239TokenInfo)(cServMsg.GetMsgBody()));

	//SEND OTHER MT TOKEN INDICATION
	if( m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt )
	{
		NotifyH239TokenOwnerInfo( NULL );
	}
	else
	{
		//���m_byNewTokenReqMtid��Ϊ0�����벻�ǵ�ǰ֪ͨMCU��ID����˵����ǰ�������ն����������ƣ���MCU��δ��׼�䷢��˫������ʱ����������֪ͨ�����ⱻ���
		if (m_byNewTokenReqMtid != 0 && m_byNewTokenReqMtid != tMt.GetMtId())
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2,  "[ProcMtMcuOwnH239TokenNotify]m_byNewTokenReqMtid(%d) != tMt(%d),So Return!\n",m_byNewTokenReqMtid,tMt.GetMtId());
			return;
		}
		// xliang [11/14/2008] polycomMCU����ΪkedaMCU��Token����Ȩ���ʲ�����Token.
		//��ֱ���ڴ˷�TokenOwnerInd�ͱ�ʾ��ǰPolycomMCUҪ��˫�������������...
	// 	if ( (( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) || MT_MANU_AETHRA ==m_ptMtTable->GetManuId(tMt.GetMtId()))
	// 		  && ( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) || MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId())) )
	   // ��MCU��Ϊ˫��Դ��1)��ʵ������Ϊ��Token���Ĺ���Ȩ�ޣ�����������ֱ����ͨ����ʽ��֪
	   //                  2)MCU���˫��ͨ���򿪺󲻹رգ�ͨ�����ؽ�������
	   //                  3)�ж��Ƿ����㷢��˫�����������������˫��ԴΪMCU����������ת����˫��
		TLogicalChannel tFstLogicChnnl;
		TLogicalChannel tSecLogicChnnl;
		if ((MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) || MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) || MT_MANU_KDCMCU == m_ptMtTable->GetManuId( tMt.GetMtId() )) 
			&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tFstLogicChnnl, FALSE)
			&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_SECVIDEO, &tSecLogicChnnl, FALSE)
			&& !(m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt)
			&& CheckDualMode(tMt))
		{
 			u16 wDailBR = m_ptMtTable->GetDialBitrate(tMt.GetMtId());
			u16 wSecBR = tSecLogicChnnl.GetFlowControl();
			
			if ( wSecBR * 100 / wDailBR > 10 )
			{
				if ( m_tDoubleStreamSrc == tMt )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcMtMcuOwnH239TokenNotify] DS has been started, ignore the same Ind\n" );
					if (m_tH239TokenOwnerInfo.GetTokenMt().IsNull())
					{
						//[20101224 zjl]������������һ�������˷�˫�����ڶ�������Ϊ�����ն˷�˫����
						//�������ն˷�˫�����ڶ�����ͨ����ͷ����䱾����˫��������TokenMtΪ��
						//�ϵ������ն�����һ�������ˣ����µڶ���mcuʼ���޷���TokenMt��֪��һ����
						//���յ�һ���޷��ɹ�����˫��
						m_tH239TokenOwnerInfo.SetTokenMt(tMt);
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtMcuOwnH239TokenNotify] DS has been started and Token Mt is null!\n");
					}
				}
				else
				{
					if(!m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() &&
					   !(m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt))
					{
						ClearH239TokenOwnerInfo(&m_tH239TokenOwnerInfo.m_tH239TokenMt);
					}
					//�ͷ����ƻ�
					//ClearH239TokenOwnerInfo(NULL);

					//�������ƻ�ӵ������Ϣ
					m_tH239TokenOwnerInfo.SetTokenMt(tMt);
					NotifyH239TokenOwnerInfo(NULL);

					StartDoubleStream(tMt, tSecLogicChnnl);	
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcMtMcuOwnH239TokenNotify] tMt.%d, wSecBR.%d, wDailBR.%d, StartDS\n",
						tMt.GetMtId(), wSecBR, wDailBR );
				}

			}
			else
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "[ProcMtMcuOwnH239TokenNotify] tMt.%d, wSecBR.%d, wDailBR.%d, should but no StartDS due to bad band distribution.%d\n",
					tMt.GetMtId(), wSecBR, wDailBR);
			}		
		}
		else
		{
			tFstLogicChnnl.Clear();
			tSecLogicChnnl.Clear();
			//����δ����˫���ɹ���������MCU˫������ͨ���������ʼ���MCU��������ͨ������ҲӦ�õ���[1/6/2013 chendaiwei]
			if ( (MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) 
				|| MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId())) 
				&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_SECVIDEO, &tSecLogicChnnl, FALSE)
				&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tFstLogicChnnl, FALSE)
				&& tFstLogicChnnl.GetFlowControl() == m_ptMtTable->GetSndBandWidth(tMt.GetMtId()))
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMtMcuOwnH239TokenNotify] Adjust Mt<%d> Main Stream Send BR from %d to %d due to DStream chnnl opened!\n",
					tMt.GetMtId(),tFstLogicChnnl.GetFlowControl(),GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth(tMt.GetMtId())));
				
				tFstLogicChnnl.SetFlowControl(GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth(tMt.GetMtId())));
				m_ptMtTable->SetMtLogicChnnl(tMt.GetMtId(),LOGCHL_VIDEO,&tFstLogicChnnl,FALSE);		
			}

			ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcMtMcuOwnH239TokenNotify] tmt.%d(type:%d) should not start ds due to no condition(checkdualmode:%d, tokenowner(%d))\n",
				  tMt.GetMtId(), m_ptMtTable->GetMainType(tMt.GetMtId()), CheckDualMode(tMt), m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId());
		}
	}

	return;
}

/*=============================================================================
    �� �� ���� ProcMtMcuReleaseH239TokenNotify
    ��    �ܣ� �ն˸�MCU�� �ͷ� H239���� Ȩ��֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/6/8    3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMtMcuReleaseH239TokenNotify( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	
	if( m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt )
	{
		TH239TokenInfo tH239TokenInfo;
		tH239TokenInfo.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
		
		m_tH239TokenOwnerInfo.Clear();

		//POLYCOM7000�ľ�̬˫��ʱ,����ͨ����һ��ʼ�򿪺�һֱ���رգ�ʹ��H239Token���з��ͼ����ر�
		if( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) && 
			m_tDoubleStreamSrc == tMt )
		{
			StopDoubleStream( FALSE, FALSE );
		}

		TMt tMtPolycom;
		CServMsg cMsg;
		// xliang [11/14/2008] ͬʱ�ͷ� POLYMCU ��TOKEN
		for ( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
		{
// 			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byMtId) &&
// 				( MT_TYPE_SMCU == m_ptMtTable->GetMtType(byMtId) ||
			// 				MT_TYPE_MMCU == m_ptMtTable->GetMtType(byMtId) ) )
			// �ͷ����ƻ���Ӧ��֪ͨ�ǿƴ�MCU(���ն�),��Ӧ��ֻ���ڱ���ͨMCU [6/24/2013 chendaiwei]
			//[H239]11.2.3  End-user system owns the token and wants to release the token 
			//The end-user system shall send presentationTokenRelease.
			if ( MT_MANU_KDC != m_ptMtTable->GetManuId(byMtId)
				&& MT_MANU_KDCMCU != m_ptMtTable->GetManuId(byMtId))
			{
				tMtPolycom = m_ptMtTable->GetMt( byMtId );
				cMsg.SetMsgBody((u8*)&tMtPolycom, sizeof(TMt));
				cMsg.CatMsgBody((u8*)&tH239TokenInfo, sizeof(TH239TokenInfo));
				cMsg.SetEventId(MCU_POLY_RELEASEH239TOKEN_CMD);
				SendMsgToMt(byMtId, MCU_POLY_RELEASEH239TOKEN_CMD, cServMsg);
			}
		}
	}
	else
	{
		//��������ͷ�֪ͨ�ķ�������KEDAMCU�������������¼��Ϊ0����˵���������ն���������ƣ�����û�и���������ȷ�ϣ���ô��Ҫ�ڴ˸�������Ӧȷ����Ϣ���Ա�����˳������˫��
		if (MT_MANU_KDCMCU == m_ptMtTable->GetManuId(tMt.GetMtId()) && m_byNewTokenReqMtid != 0)
		{
			TMt tNewTokenMt = m_ptMtTable->GetMt(m_byNewTokenReqMtid);
			if (MT_MANU_KDCMCU != m_ptMtTable->GetMtType(m_byNewTokenReqMtid))
			{
				if( !m_tDoubleStreamSrc.IsNull() && m_tDoubleStreamSrc.GetMtId() != tNewTokenMt.GetMtId())
				{
					StopDoubleStream( TRUE, FALSE );
				}
				m_byNewTokenReqMtid = 0;
				TH239TokenInfo tSrcH239Info = m_tH239TokenOwnerInfo;
				tSrcH239Info.SetTokenMt(tNewTokenMt);
				TLogicalChannel tH239LogicChnnl;
				SetTimer( MCUVC_WAITOPENDSCHAN_TIMER, 3000);
				cServMsg.SetMsgBody((u8 *)&tNewTokenMt, sizeof(TMt));
				cServMsg.CatMsgBody((u8 *)&tSrcH239Info, sizeof(TH239TokenInfo));
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2,"[ProcMtMcuReleaseH239TokenNotify]Current TokenMt(%d) chnIdx(%d) break(%d),So Return!!\n",
					tNewTokenMt.GetMtId(),tSrcH239Info.GetChannelId(),tSrcH239Info.GetSymmetryBreaking());	
				if( ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tNewTokenMt.GetMtId()) ||
					( MT_MANU_AETHRA == m_ptMtTable->GetManuId(tNewTokenMt.GetMtId()) &&
					MT_TYPE_MT != m_ptMtTable->GetMtType(tNewTokenMt.GetMtId()) ) )
					&& m_ptMtTable->GetMtLogicChnnl( tNewTokenMt.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ) )
				{
					if (m_tConf.GetConfAttrb().IsSatDCastMode() /*&& IsOverSatCastChnnlNum(tNewTokenMt.GetMtId())*/
						&& IsMultiCastMt(tNewTokenMt.GetMtId())
						&& //IsSatMtOverConfDCastNum(tNewTokenMt))
						!IsSatMtCanContinue(tNewTokenMt.GetMtId(),emstartDs))
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuReleaseH239TokenNotify] over max upload mt num.\n");
						cServMsg.SetEventId(MCU_MT_GETH239TOKEN_NACK);	
						SendMsgToMt( tNewTokenMt.GetMtId(), MCU_MT_GETH239TOKEN_NACK, cServMsg);
						return;
					}
					StartDoubleStream( tNewTokenMt, tH239LogicChnnl );
				}	
				
				cServMsg.SetEventId(MCU_MT_GETH239TOKEN_ACK);	
				SendMsgToMt( tNewTokenMt.GetMtId(), MCU_MT_GETH239TOKEN_ACK, cServMsg);

				m_tH239TokenOwnerInfo.SetChannelId( tSrcH239Info.GetChannelId() );
				m_tH239TokenOwnerInfo.SetSymmetryBreaking( tSrcH239Info.GetSymmetryBreaking() );
				m_tH239TokenOwnerInfo.SetTokenMt( tNewTokenMt );
				NotifyH239TokenOwnerInfo( NULL );
			}
		}
	}
	
	return;
}

/*=============================================================================
    �� �� ���� procWaitOpenDSChanTimeout
    ��    �ܣ� �ն�����239token��δ��˫���߼�ͨ����ʱ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2011/9/22    3.6		��ʤ��                  ����
=============================================================================*/
void CMcuVcInst::procWaitOpenDSChanTimeout( void )
{
    //�����ǰ˫��Դ˫��ͨ�������ͷ�239ӵ����
    TLogicalChannel tLogicalChannel;
    if ( !m_tDoubleStreamSrc.IsNull() &&
         m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ))
    {
        LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[procWaitOpenDSChanTimeout]DsChan has be open!\n");
        return;
    }
    else if(!m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull())
    {
        LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[procWaitOpenDSChanTimeout]mt:%d open DsChan time out after 239 requst!\n",
            m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId() );
        ClearH239TokenOwnerInfo( &m_tH239TokenOwnerInfo.m_tH239TokenMt );  
        NotifyH239TokenOwnerInfo( NULL );
    }

	// ˫��������ʱ����vmp˫������ͨ��
	TEqp tVmpEqp;
	TVMPParam_25Mem tConfVmpParam;
	TPeriEqpStatus tPeriEqpStatus;
	TVMPParam_25Mem tVmpParam;
	u8 byDSChl;
	TVMPMember tVmpMember;
	TVmpChnnlInfo tVmpChnnlInfo;
	TVMPParam_25Mem tLastVmpParam;
	CServMsg cServMsg;
	u8 byTmp = 0XFF;//���ֶα���,���mpu��������,Ϊ����,��һ���ֽڱ�ʶ�ر�ͨ��
	u8 byAdd = 0;//��ʶ�ر�ͨ��
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
				//��˫������ͨ��
				byDSChl = tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_DSTREAM);
				if (byDSChl < MAXNUM_VMP_MEMBER)
				{
					tVmpMember = *tVmpParam.GetVmpMember(byDSChl);
					if (!tVmpMember.IsNull())
					{
						tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
						//ͣ����
						StopSwitchToPeriEqp(tVmpEqp.GetEqpId(), byDSChl, FALSE, MODE_SECVIDEO);
						tVmpChnnlInfo.ClearChnlByVmpPos(byDSChl); //���ͨ��ǰ����
						g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
						//˫���������ͨ��
						tVmpMember.SetNull();
						tVmpParam.SetVmpMember(byDSChl, tVmpMember);
						tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);	//����״̬����
						g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
						//���»���vmpparam
						if (byDSChl == tConfVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_DSTREAM))
						{
							tConfVmpParam.SetVmpMember(byDSChl, tVmpMember);
							//m_tConf.m_tStatus.SetVmpParam(tVmpParam);
							g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tConfVmpParam);
							// ����LastVmpParamǰ,ˢ��vmp˫������ͨ��̨��
							RefreshVmpChlMemalias(tVmpEqp.GetEqpId());
							tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
							tLastVmpParam.SetVmpMember(byDSChl, tVmpMember);//����m_tLastVmpParam
							g_cMcuVcApp.SetLastVmpParam(tVmpEqp, tLastVmpParam);
						}
						//�������������ͨ��
						{
							LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[StopDoubleStream]Send MCU_VMP_ADDREMOVECHNNL_CMD To Vmp, byChnnl:%d.\n", byDSChl);
							cServMsg.Init();
							cServMsg.SetChnIndex(byDSChl);
							cServMsg.SetMsgBody( &byTmp, sizeof(byTmp) );
							cServMsg.CatMsgBody( &byAdd, sizeof(byAdd) );
							SendMsgToEqp( tVmpEqp.GetMtId(), MCU_VMP_ADDREMOVECHNNL_CMD, cServMsg);
						}
						cServMsg.Init();
						cServMsg.SetEqpId(tVmpEqp.GetEqpId());
						cServMsg.SetMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
						SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
						/*cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
						SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );*/
					}
				}
			}
		}

	}
    return;    
}

/*=============================================================================
    �� �� ���� ProcMcsMcuSetMtVolumeCmd
    ��    �ܣ� ��ظ�MCU�� �����ն����� ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/12/26  4.0			�ܹ��                  ����
=============================================================================*/
void CMcuVcInst::ProcMcsMcuSetMtVolumeCmd(const CMessage * pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt tMt = *(TMt *)cServMsg.GetMsgBody();

	if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{
		return;
	}

	u8 byVolumeType = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMt) );
	u8 byVolume = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) );
	cServMsg.SetDstMtId( tMt.GetMtId() );
	cServMsg.SetEventId( MCU_MT_SETMTVOLUME_CMD );
	cServMsg.SetMsgBody( &byVolumeType, sizeof(u8) );
	cServMsg.CatMsgBody( &byVolume, sizeof(u8) );
	SendMsgToMt( tMt.GetMtId(), MCU_MT_SETMTVOLUME_CMD, cServMsg );
	return;
}

/*=============================================================================
    �� �� ���� ProcPolyMCUH239Rsp
    ��    �ܣ� �����polycom���ƻ�ȡ��Ӧ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/3/20   4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcPolyMCUH239Rsp( const CMessage * pcMsg )
{
	STATECHECK;

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	TMt tPolyMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());

	switch(pcMsg->event)
	{
	case POLY_MCU_GETH239TOKEN_ACK:			//��ȡPolyMCU��H239TOKEN ͬ��Ӧ��
		{
			TLogicalChannel tSecChnnl;
			if (m_ptMtTable->GetMtLogicChnnl(tPolyMt.GetMtId(), LOGCHL_SECVIDEO, &tSecChnnl, TRUE))
			{
				//zjl 20110510 Mp:StartSwitchToAll �滻 StartSwitchToSubMt ����ԭ������bH239chnΪʲôû�����������Ӧ�ý���������
				//g_cMpManager.StartSwitchToSubMt(m_tDoubleStreamSrc, 0, tPolyMt, MODE_SECVIDEO);
				TSwitchGrp tSwitchGrp;
				tSwitchGrp.SetSrcChnl(0);
				tSwitchGrp.SetDstMtNum(1);
				tSwitchGrp.SetDstMt(&tPolyMt);
				g_cMpManager.StartSwitchToAll(m_tDoubleStreamSrc, 1, &tSwitchGrp, MODE_SECVIDEO);
			}
			else
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcPolyMCUH239Rsp] PolyMcu.%d Fwd chnnl should not be closed, reinvite it, please\n", tPolyMt.GetMtId() );
			}
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcPolyMCUH239Rsp] PolyMcu.%d accept TokenReq, POLY_MCU_GETH239TOKEN_ACK received\n", tPolyMt.GetMtId() );
			break;
		}
	case POLY_MCU_GETH239TOKEN_NACK:		//��ȡPolyMCU��H239TOKEN �ܾ�Ӧ��
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcPolyMCUH239Rsp] GETH239TOKEN_NACK\n");
			break;
		}
	case POLY_MCU_OWNH239TOKEN_NOTIF:		//PolyMCU֪ͨ��ǰ��TOKEN��ӵ����
		{
			//FIXME����Ϣ����ʲô����ӡһ�£�Ҫ��Ҫ����Ӧ
			//˭ӵ����H239
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcPolyMCUH239Rsp] POLY_MCU_OWNH239TOKEN_NOTIF\n");
			break;
		}
	case POLY_MCU_RELEASEH239TOKEN_CMD:		//PolyMCU�ͷ�H329TOKEN ����
		{
			//˫���͵�һ·Flowctrl��ȥ
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcPolyMCUH239Rsp] POLY_MCU_RELEASEH239TOKEN_CMD\n");
			break;
		}
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcPolyMCUH239Rsp] unexpected msg.%d<%s>\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
	return;
}

/*====================================================================
    ������      ��ProcMtMcuCloseLogicChnnlCmd
    ����        ���ն˷����Ĺر��߼�ͨ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/04/04    3.0         ������          ����
====================================================================*/
void CMcuVcInst::ProcMtMcuCloseLogicChnnlNotify( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt;
	u8  byChannelType = 0;
	TLogicalChannel	tLogicChnnl, tH239LogicChnnl, tNullLogicChnnl;

	STATECHECK

	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );
	BOOL32 bOut = ISTRUE(*(cServMsg.GetMsgBody()+sizeof(TLogicalChannel)));
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt%d close Out.%d logical channel received!\n", tMt.GetMtId(), bOut );

	if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{
		return;
	}

	//�ı�ͨ��״̬
	if( bOut )
	{
		if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
		{
			m_ptMtTable->SetMtVideoRecv( tMt.GetMtId(), FALSE );
		}
		if( MODE_AUDIO == tLogicChnnl.GetMediaType() )
		{
			m_ptMtTable->SetMtAudioRecv( tMt.GetMtId(), FALSE );
		}
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
			m_ptMtTable->SetMtVideo2Recv( tMt.GetMtId(), FALSE );
			//20100708_tzy ���ڶ�·��Ƶ�ر�ʱ��Ӧ�ָ�ԭ����һ·��Ƶ�����ʣ���֪ͨ�ն˵�һ·��Ƶ���ʻָ�������
			TLogicalChannel tLogicalChannel;
			if (m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, TRUE ))
			{
				u16 wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(tMt.GetMtId());
				//[nizhijun 2010/11/2] Bug00040303
				//tLogicalChannel.SetFlowControl( wMtDialBitrate );
				//cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
				//SendMsgToMt( tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );
				m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), wMtDialBitrate, LOGCHL_VIDEO);
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuCloseLogicChnnlNotify]tMt.GetMtId() = %d, MtBitrate = %d LOGCHL_VIDEO ReStore\n",
					tMt.GetMtId(), wMtDialBitrate);	
			}
		}

		// �����ն�ֱ�ӵ��˽ӿ� [pengguofeng 3/5/2013]
		if ( IsMultiCastMt(tMt.GetMtId()))
		{
			StopSwitchToSubMt(1, &tMt, tLogicChnnl.GetMediaType());
		}
	}
	else
	{
		if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
		{
			m_ptMtTable->SetMtVideoSend( tMt.GetMtId(), FALSE );
		}
		if( MODE_AUDIO == tLogicChnnl.GetMediaType() )
		{
			m_ptMtTable->SetMtAudioSend( tMt.GetMtId(), FALSE );
		}
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
			m_ptMtTable->SetMtVideo2Send( tMt.GetMtId(), FALSE );
		}
		MtStatusChange( &tMt,TRUE );
	}

	//��ռ������ϼ��ı�ʶ
	if(MEDIA_TYPE_MMCU == tLogicChnnl.GetChannelType() )
	{
		byChannelType = LOGCHL_MMCUDATA;
		if(tMt == m_tCascadeMMCU)
		{
			//����ϼ�MCU����
			if( m_tConf.m_tStatus.GetProtectMode() == CONF_LOCKMODE_LOCK &&
				tMt.GetMtId() == m_tConfProtectInfo.GetLockedMcuId() )
			{
				m_tConf.m_tStatus.SetProtectMode(CONF_LOCKMODE_NONE);
                m_tConfProtectInfo.SetLockByMcu(0);
                m_tConfProtectInfo.SetLockByMcs(0);
			}

			//lukunpeng 2010/06/17 �˴��������ͷţ���RemoveJoinedMTʱ����Ҫ�˱����𽻻�
			/*
			m_tCascadeMMCU.SetNull();
			m_tConfAllMtInfo.m_tMMCU.SetNull();
            m_tConfInStatus.SetNtfMtStatus2MMcu(FALSE);
			*/
		}
	}
	else if( MEDIA_TYPE_H224 == tLogicChnnl.GetChannelType() )
	{
		byChannelType = LOGCHL_H224DATA;
		TMtStatus tMtStatus;
		m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus);
		tMtStatus.SetIsEnableFECC(FALSE);
		m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tMtStatus);
	}
	else if( MEDIA_TYPE_T120 == tLogicChnnl.GetChannelType() )
	{
		byChannelType = LOGCHL_T120DATA;
	}
	else if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
	{
		byChannelType = LOGCHL_SECVIDEO;
		
        // zbq [08/31/2007] ���������ж�ͨ�����򣬷���������
		if(!bOut && 
            m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ))
		{
			TLogicalChannel tFirstChnnl;
			if( m_tDoubleStreamSrc == tMt )
			{
				StopDoubleStream( FALSE, FALSE );
				//���ǻ����£�����������ն�������ᣬ˫��Դ��Ҫ�ø��ն�ֹͣ����˫����������������
				if( IsMultiCastMt(tMt.GetMtId()) )
				{
					CServMsg cTempServMsg;
					tH239LogicChnnl.SetFlowControl(0);
					cTempServMsg.SetMsgBody((u8*)&tH239LogicChnnl, sizeof(tH239LogicChnnl));
					SendMsgToMt(tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cTempServMsg);
				}
			}
			else if(m_tDoubleStreamSrc.IsNull() 
				&& (tMt.GetMtType() == MT_TYPE_MMCU || tMt.GetMtType() == MT_TYPE_SMCU)
				&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(),LOGCHL_VIDEO,&tFirstChnnl,FALSE))
			{
				
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuCloseLogicChnnlNotify]adjust MT<%d> MSream send BR from.%d to.%d due to DStream chnnl closed!\n",
					tMt.GetMtId(),tFirstChnnl.GetFlowControl(),m_ptMtTable->GetSndBandWidth(tMt.GetMtId()));
				
				tFirstChnnl.SetFlowControl(m_ptMtTable->GetSndBandWidth(tMt.GetMtId()));
				m_ptMtTable->SetMtLogicChnnl(tMt.GetMtId(),LOGCHL_VIDEO,&tFirstChnnl,FALSE);
			}

			if (m_tSecVidBrdSrc == tMt)
			{
				TMt tNullMt;
				ChangeSecSpeaker(tNullMt);
			}
		}
        // zbq [08/31/2007] ���������ж�ͨ�����򣬷���������
		else if(bOut &&
                m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, TRUE ))
		{
            // ���� [4/30/2006] ����ն������ر����Ƿ�������ͨ������ͣ����
            // ���򣬾��Ǹ��ն˳��Դ�˫��ͨ��
            // ����������˫��Դ��NCU�ܾ���Ĺر�֪ͨ
            // MCU����Ҫ������

            // ���ǻ��鲻ͣ���� [6/2/2006]
            if ( !m_tConf.GetConfAttrb().IsSatDCastMode() ||
                 !IsMultiCastMt(tMt.GetMtId()))
            {
				//zjl 20110510 Mp: StopSwitchToSubMt �ӿ������滻 
                //g_cMpManager.StopSwitchToSubMt( tMt, MODE_SECVIDEO, TRUE  );
				g_cMpManager.StopSwitchToSubMt(m_byConfIdx, 1, &tMt, MODE_SECVIDEO);
            }
		}
	}
	else if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
	{
		byChannelType = LOGCHL_VIDEO;
        // ���ǻ��鲻ͣ���� [6/2/2006]
        if ( !m_tConf.GetConfAttrb().IsSatDCastMode() ||
             !IsMultiCastMt(tMt.GetMtId()) )
        {
			TMtStatus tDstStatus;
			tDstStatus.SetNull();
		    if(bOut)
		    {
				//1.ͣĿ���ն˵�prs��rtcp����(StopSwitchToSubMt����뵱ǰĿ���ն�Դ��Ϣ��������ǰ����)
				StopPrsMemberRtcp(tMt, MODE_VIDEO);
				

				//3.ͣѡ��
				m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tDstStatus);
				TMt tSelSrc = tDstStatus.GetSelectMt(MODE_VIDEO);
				if( !tSelSrc.IsNull() && IsNeedSelAdpt(tSelSrc, tMt, MODE_VIDEO) )
				{
					//[2011/08/30/zhangli]Bug00063241�������ѡ�������¼��նˣ���ҪFreeRecvSpy
					StopSelAdapt(tSelSrc, tMt, MODE_VIDEO);
					FreeRecvSpy(tSelSrc, MODE_VIDEO);
					NotifyMtReceive( tMt,tMt.GetMtId() );
				}
				else
				{
					StopSwitchToSubMt(1, &tMt, MODE_VIDEO);
					TConfAttrb tConfattrb = m_tConf.GetConfAttrb();
					if (tConfattrb.IsResendLosePack())
					{
						u8 bySrcChn = 0;
						if (EQP_TYPE_VMP == tSelSrc.GetEqpType())
						{
							bySrcChn = GetVmpOutChnnlByDstMtId( tMt.GetMtId(), tSelSrc.GetEqpId());
						}
						if(0xff != bySrcChn)
						{
							BuildRtcpSwitchForSrcToDst(tMt, tSelSrc, MODE_VIDEO, 0, bySrcChn, FALSE);
						}
					}
				}
		    }
		    else 
		    {
				//2.ͣ���ж��һ����
				StopRtcpSwitchAlltoOne(tMt, 0, MODE_VIDEO);
 				TMt tOrgSrc;
				TMt tLocalSrc;
				tOrgSrc.SetNull();
				for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
				{
					if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
					{
						continue;
					}
					if (byMtId == tMt.GetMtId())
					{
						continue;
					}
					tDstStatus.SetNull();
					m_ptMtTable->GetMtStatus(byMtId, &tDstStatus);
					tOrgSrc   = tDstStatus.GetSelectMt(MODE_VIDEO);
					tLocalSrc = tOrgSrc;
					tLocalSrc = GetLocalMtFromOtherMcuMt(tLocalSrc);
					if ( tLocalSrc == tMt )
					{
						TMt tDstMt = m_ptMtTable->GetMt(byMtId);
						StopSelectSrc(tDstMt,MODE_VIDEO);
					}
				}

				//�����ǰ�ն�ͬʱ�ش����ϼ���������������ͣ����
				if(IsSupportMultiSpy())
				{
					TSimCapSet tSrcCap;
					CSendSpy *ptSndSpy = NULL;
					for (u8 bySpyIdx = 0; bySpyIdx < MAXNUM_CONF_SPY; bySpyIdx++)
					{
						ptSndSpy = m_cLocalSpyMana.GetSendSpy(bySpyIdx);
						if (NULL == ptSndSpy)
						{
							continue;
						}
						if (ptSndSpy->GetSpyMt().IsNull() || ptSndSpy->GetSimCapset().IsNull() || !(ptSndSpy->GetSpyMt() == tMt))
						{
							continue;
						}
						tSrcCap = m_ptMtTable->GetSrcSCS(GetLocalMtFromOtherMcuMt(ptSndSpy->GetSpyMt()).GetMtId());
						if(ptSndSpy->GetSimCapset() < tSrcCap)
						{
							StopSpyAdapt(ptSndSpy->GetSpyMt(), ptSndSpy->GetSimCapset(), MODE_VIDEO);
						}
						break;
					}
				}
		    }
        }
	}
	else if( MODE_AUDIO == tLogicChnnl.GetMediaType() )
	{
		byChannelType = LOGCHL_AUDIO;
        // ���ǻ��鲻ͣ���� [6/2/2006]
        if ( !m_tConf.GetConfAttrb().IsSatDCastMode() ||
             !IsMultiCastMt(tMt.GetMtId()) )
        {
			TMtStatus tDstStatus;
			tDstStatus.SetNull();
			if(bOut)
			{				
				//1.ͣĿ���ն˵�prs��rtcp����(StopSwitchToSubMt����뵱ǰĿ���ն�Դ��Ϣ��������ǰ����)
				StopPrsMemberRtcp(tMt, MODE_AUDIO);
				

				//3.ͣѡ��
				m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tDstStatus);
				TMt tSelSrc = tDstStatus.GetSelectMt(MODE_AUDIO);
				if( !tSelSrc.IsNull() && IsNeedSelAdpt(tSelSrc, tMt, MODE_AUDIO) )
				{
					StopSelAdapt(tSelSrc, tMt, MODE_AUDIO);
					FreeRecvSpy(tSelSrc, MODE_AUDIO);
					NotifyMtReceive( tMt,tMt.GetMtId() );
				}
				else
				{
					StopSwitchToSubMt(1, &tMt, MODE_AUDIO);
					TConfAttrb tTmpConfattrb = m_tConf.GetConfAttrb();
					if (tTmpConfattrb.IsResendLosePack())
					{
						u8 bySrcChn = 0;
						if (EQP_TYPE_VMP == tSelSrc.GetEqpType())
						{
							bySrcChn = GetVmpOutChnnlByDstMtId( tMt.GetMtId(), tSelSrc.GetEqpId());
						}
						if(0xff != bySrcChn)
						{
							BuildRtcpSwitchForSrcToDst(tMt, tSelSrc, MODE_AUDIO, 0, bySrcChn, FALSE);
						}
					}
				}
			}
			else 
			{
				//2.ͣ���ж��һ����
				StopRtcpSwitchAlltoOne(tMt, 0, MODE_AUDIO);
				TMt tOrgSrc;
				TMt tLocalSrc;
				tOrgSrc.SetNull();
				for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
				{
					if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
					{
						continue;
					}
					if (byMtId == tMt.GetMtId())
					{
						continue;
					}
					tDstStatus.SetNull();
					m_ptMtTable->GetMtStatus(byMtId, &tDstStatus);
					tOrgSrc   = tDstStatus.GetSelectMt(MODE_AUDIO);
					tLocalSrc = tOrgSrc;
					tLocalSrc = GetLocalMtFromOtherMcuMt(tLocalSrc);
					if ( tLocalSrc == tMt )
					{
						TMt tDstMt = m_ptMtTable->GetMt(byMtId);
						StopSelectSrc(tDstMt,MODE_AUDIO);
					}
				}

				//�����ǰ�ն�ͬʱ�ش����ϼ���������������ͣ����
				if(IsSupportMultiSpy())
				{
					TSimCapSet tSrcCap;
					CSendSpy *ptSndSpy = NULL;
					for (u8 bySpyIdx = 0; bySpyIdx < MAXNUM_CONF_SPY; bySpyIdx++)
					{
						ptSndSpy = m_cLocalSpyMana.GetSendSpy(bySpyIdx);
						if (NULL == ptSndSpy)
						{
							continue;
						}
						if (ptSndSpy->GetSpyMt().IsNull() || ptSndSpy->GetSimCapset().IsNull() || !(ptSndSpy->GetSpyMt() == tMt))
						{
							continue;
						}
						tSrcCap = m_ptMtTable->GetSrcSCS(GetLocalMtFromOtherMcuMt(ptSndSpy->GetSpyMt()).GetMtId());
						/*if (ptSndSpy->GetSimCapset().GetAudioMediaType() != MEDIA_TYPE_NULL &&
							tSrcCap.GetAudioMediaType() != MEDIA_TYPE_NULL && 
							ptSndSpy->GetSimCapset().GetAudioMediaType() != tSrcCap.GetAudioMediaType())
						{
							StopSpyAdapt(ptSndSpy->GetSpyMt(), ptSndSpy->GetSimCapset(), MODE_AUDIO);
						}*/
						if( IsNeedSpyAdpt( ptSndSpy->GetSpyMt(),tSrcCap,MODE_AUDIO) )
						{
							StopSpyAdapt(ptSndSpy->GetSpyMt(), ptSndSpy->GetSimCapset(), MODE_AUDIO);
						}
						break;
					}
				}
		    }
        }
	}


	if ( bOut )
	{
		//ˢ�µ�һ·�������
		if (m_tConf.m_tStatus.IsVidAdapting() && MODE_VIDEO == tLogicChnnl.GetMediaType() )
		{
			RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_VIDEO, TRUE);
		}	
		
		//ˢ�µڶ�·�������
		if (!m_tDoubleStreamSrc.IsNull() && 
			!(m_tDoubleStreamSrc == tMt) &&
			MODE_SECVIDEO == tLogicChnnl.GetMediaType()
			)
		{
			RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_SECVIDEO, TRUE);
		}
	}

    //�ر�ͨ��Ҫ��h460��Խ����
    if ( ( MODE_SECVIDEO == tLogicChnnl.GetMediaType() ||
           MODE_AUDIO == tLogicChnnl.GetMediaType() ||
           MODE_VIDEO == tLogicChnnl.GetMediaType() ) &&
           m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_bIsPinHole)
    {
        u8 byMpId = m_ptMtTable->GetMpId( tMt.GetMtId() );

        if( bOut )
        {//ǰ��ͨ��

            //rtcp
            StopH460PinHole(byMpId, 
                tLogicChnnl.m_tSndMediaCtrlChannel.GetPort(), 
                tLogicChnnl.m_tRcvMediaCtrlChannel.GetIpAddr(),
                tLogicChnnl.m_tRcvMediaCtrlChannel.GetPort()); 
        }
        else
        {//����ͨ��

            //rtp
            u32 dwRtpSndIp = 0;
            u16 wRtpSndPort = 0;
            if (MODE_AUDIO == tLogicChnnl.GetMediaType())
            {
                dwRtpSndIp = m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tAudRtpSndTransportAddr.GetIpAddr();
                wRtpSndPort = htons( m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tAudRtpSndTransportAddr.GetPort() );
            }
            else if(MODE_VIDEO == tLogicChnnl.GetMediaType())
            {
                dwRtpSndIp = m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tVidRtpSndTransportAddr.GetIpAddr();
                wRtpSndPort = htons(m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tVidRtpSndTransportAddr.GetPort());
            }
            else if(MODE_SECVIDEO == tLogicChnnl.GetMediaType())
            {
                dwRtpSndIp = m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tSecRtpSndTransportAddr.GetIpAddr();
                wRtpSndPort = htons( m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tSecRtpSndTransportAddr.GetPort() );
            }
            StopH460PinHole(byMpId, 
                tLogicChnnl.m_tRcvMediaChannel.GetPort(), 
                dwRtpSndIp,
                wRtpSndPort);   
            
            //rtcp
            StopH460PinHole(byMpId, 
                tLogicChnnl.m_tRcvMediaCtrlChannel.GetPort(), 
                tLogicChnnl.m_tSndMediaCtrlChannel.GetIpAddr(),
                tLogicChnnl.m_tSndMediaCtrlChannel.GetPort()); 
        }
//         StopH460PinHole(byMpId, 
//             tLogicChnnl.m_tRcvMediaChannel.GetPort(), 
//             m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tSecRtpSndTransportAddr.GetIpAddr(),
//             htons(m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tSecRtpSndTransportAddr.GetPort()));    
//         StopH460PinHole(byMpId, 
//             tLogicChnnl.m_tRcvMediaCtrlChannel.GetPort(), 
//             tLogicChnnl.m_tSndMediaCtrlChannel.GetIpAddr(),
//             tLogicChnnl.m_tSndMediaCtrlChannel.GetPort());  
    }

	//lukunpeng 2010/06/17 �˴��������ͷţ���RemoveJoinedMTʱ����ҪClear��ͨ��
    //[9/21/2011 zhushengze]���������ر�˫��,˫��ͨ�����ᱻ��֮��Clear������˫���ж�
	if (tMt.GetMtType() != MT_TYPE_MMCU || MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuCloseLogicChnnlNotify]MediaType: %d ChannelType: %d\n",
			byChannelType, tNullLogicChnnl.GetChannelType());
		//���ǻ����£������ն������˻ᣬ�ں��������߼�ͨ�����ǰӦ���ø������ն�ͣ������������ᵼ�¼�������
		if(MODE_VIDEO == tLogicChnnl.GetMediaType() && IsMultiCastMt(tMt.GetMtId()) && !bOut)
		{
			NotifyMtSend(tMt.GetMtId(),MODE_VIDEO,FALSE);
		}
		m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannelType, &tNullLogicChnnl, bOut );
	}
	
	return;
}

/*====================================================================
    ������      ��ProcMtCapSet
    ����        �������ն�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]  TMt tMt �ն�
	              [in]  TCapSupport tCapSupport �ն�������
                  [in]  TCapSupport tCapSupportEx �ն���չ������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/04/04    3.0         ������          ����
	2005/12/16	4.0			�ű���			T120����ͨ���򿪴���
    2006/04/11  4.0         ����          ֧�ֽ�����ɢ���飬�������鲥��ַ
    2007/08/28  4.0         ����          8000B/8000C ˫ý����������֧��
    2007/10/13  4.0         �ű���          �ն���չ������/FEC ����֧��
====================================================================*/
void CMcuVcInst::ProcMtCapSet(TMt tMt, TMultiCapSupport &tCapSupport, u8 bMtHasVidCap /* = 1 */)
{
    TLogicalChannel tDataLogicChnnl;

    //zbq[04/09/2009] Tandberg ��������
    if (MT_MANU_TAIDE == m_ptMtTable->GetManuId((tMt.GetMtId())) &&
        IsHDConf(m_tConf))
    {		
        if (g_cMcuVcApp.IsSendFakeCap2Taide() && 
			0 == strcmp("58", m_ptMtTable->GetVersionId(tMt.GetMtId())))
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
        }
        else if (g_cMcuVcApp.IsSendFakeCap2TaideHD())
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_HD720);
        }
    }

    //zbq[09/12/2009] ��Ϊ��������
    if (MT_MANU_HUAWEI == m_ptMtTable->GetManuId(tMt.GetMtId()))
    {
        /*if (g_cMcuVcApp.IsSendFakeCap2HuaweiSDEp() &&
            0 == strcmp("Huawei H.323 Stack Verion 1.1", m_ptMtTable->GetVersionId(tMt.GetMtId())))
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
        }*/
		u8 byVideoFormat = 0;
        if ( m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&			
            g_cMcuVcApp.GetFakeCap2HuaweiSDEp(m_ptMtTable->GetIPAddr(tMt.GetMtId()),byVideoFormat) )
        {
			if(m_tConf.GetConfAttrbEx().IsResExCif() && VIDEO_FORMAT_CIF == byVideoFormat )
			{
				tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
			}
			if( m_tConf.GetConfAttrbEx().IsResEx4Cif() && VIDEO_FORMAT_4CIF == byVideoFormat )
			{
				tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_4CIF);
			}
			if( m_tConf.GetConfAttrbEx().IsResEx720() && VIDEO_FORMAT_HD720 == byVideoFormat )
			{
				tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_HD720);
			}            
        }
    }

	TMt tNullMt;
	tNullMt.SetNull();
	u8 byDriId = m_ptMtTable->GetDriId(tMt.GetMtId());

	//[liu lijiu][20101028] USBKEY License ��Ȩ����У��
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
#ifdef  _LINUX_
#ifndef _NOUSBKEY_
	u16 wUSBKEYError = 0;
	if( !g_cMcuVcApp.GetLicesenUSBKEYFlag(wUSBKEYError))
	{    
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Invalid USBKEY license data, the error is :%d\n", wUSBKEYError);
		NotifyMcsAlarmInfo( 0, wUSBKEYError);
		RemoveJoinedMt( tMt, TRUE );
		return;
	}
#endif//_NOUSB_ ends
#endif//_LINUX_ ends
#endif//_8KE_ ends

	u16 wError = 0;
	//���������������
    if(!g_cMcuVcApp.IncAudMtAdpNum(m_byConfIdx, tMt.GetMtId(), bMtHasVidCap, wError))
	{
		CServMsg cSendServMsg;
		cSendServMsg.SetEventId( MCU_MCS_CALLMTMODE_NOTIF );
		cSendServMsg.SetMsgBody( (u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt) );
        SendMsgToAllMcs( MCU_MCS_CALLMTMODE_NOTIF, cSendServMsg );
		NotifyMcsAlarmInfo( 0, wError);
		RemoveJoinedMt(tMt, TRUE);
		return;
	}

	if(g_cMcuVcApp.IsMtNumOverMcuLicense(m_byConfIdx, tMt, tCapSupport, wError))
	{    
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Call Mt failed due to license limit, the error is :%d\n", wError);
		
		//�޸��ն˺���ģʽΪ�ֶ����У�����һֱ��������ʧ����ʾ
		m_ptMtTable->SetCallMode( tMt.GetMtId(),CONF_CALLMODE_NONE );
		
		//ˢ�½����ն˺��з�ʽ
		CServMsg cSendServMsg;
		cSendServMsg.SetEventId( MCU_MCS_CALLMTMODE_NOTIF );
		cSendServMsg.SetMsgBody( (u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt) );
        SendMsgToAllMcs( MCU_MCS_CALLMTMODE_NOTIF, cSendServMsg );
		
		NotifyMcsAlarmInfo( 0, wError);
		RemoveJoinedMt( tMt, TRUE );
		return;
	}

#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
	// �����������飬��8000E���岻�󣬿�ʡ
	if ( g_cMcuVcApp.GetPerfLimit().IsLimitMtNum() )
	{
		u8 by8KeDriId = tMt.GetDriId();
		
		// �ж��ն������Ƿ񳬹�����������
		u8 byMtNum = 0;
		u8 byMcuNum = 0;
		//BOOL32 bOverCap = g_cMcuVcApp.GetMtNumOnDri( by8KeDriId, TRUE, byMtNum, byMcuNum );
		//  [12/22/2010 chendaiwei] ����ӿڶ�GetMtNumOnDri���з�װ������GetMtNumOnDri��ͬ�ط����õ�����
		BOOL32 bOverCap = g_cMcuVcApp.IsMtNumOverDriLimit( by8KeDriId, TRUE, byMtNum, byMcuNum );
		if ( bOverCap )
		{
			CServMsg cServMsg;
			cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
			cServMsg.SetErrorCode(ERR_MCU_OVERMAXCONNMT);
			cServMsg.SetConfId( m_tConf.GetConfId() );
			SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg );
			
			cServMsg.SetMsgBody();
			SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
			
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "[ProcMtMcuMtJoinNotif] mtadp.%d ability(Max=%d) is full! \n", 
				by8KeDriId,
				g_cMcuVcApp.GetMtAdpSupportMtNum(by8KeDriId));
			return;
		}
	}       


	// xliang [10/27/2008] ����HD MT����Ƕ��������µĴ���
	if(tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 &&
		(tCapSupport.GetMainVideoResolution() == VIDEO_FORMAT_HD1080 ||
		 tCapSupport.GetMainVideoResolution() == VIDEO_FORMAT_HD720) )
	{

#ifdef _LINUX_
		//��Ƕ����	
		if (byDriId == MCU_BOARD_MPC ||
			byDriId == MCU_BOARD_MPCD )
		{
			u8 byIdx;
			u8 byEmptyIdx = MAXHDLIMIT_MPC_MTADP;
			BOOL32 bAlreadyExist = FALSE;
			TMtAdpHDChnnlInfo tHdChnnlInfo;
			tHdChnnlInfo.SetConfIdx(m_byConfIdx);
			tHdChnnlInfo.SetHDMtId(tMt.GetMtId());
			for (byIdx = 0; byIdx < MAXHDLIMIT_MPC_MTADP; byIdx ++)
			{
				if(g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_atHdChnnlInfo[byIdx] == tHdChnnlInfo)
				{
					bAlreadyExist = TRUE;
					break;
				}
				if(g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_atHdChnnlInfo[byIdx].GetHDMtId() == 0 &&
					byEmptyIdx == MAXHDLIMIT_MPC_MTADP)
				{
					byEmptyIdx = byIdx;
				}
			}
			if( !bAlreadyExist)
			{
				if(byEmptyIdx < MAXHDLIMIT_MPC_MTADP )
				{
					g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_atHdChnnlInfo[byEmptyIdx].SetHDMtId(tMt.GetMtId());
					g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_atHdChnnlInfo[byEmptyIdx].SetConfIdx(m_byConfIdx);
				}
				else //�������������ϱ������mcs
				{
                    //�޸��ն˺���ģʽΪ�ֶ����У�����һֱ��������ʧ����ʾ
                    m_ptMtTable->SetCallMode( tMt.GetMtId(),CONF_CALLMODE_NONE );

                    //ˢ�½����ն˺��з�ʽ
                    CServMsg cSendServMsg;
                    cSendServMsg.SetEventId( MCU_MCS_CALLMTMODE_NOTIF );
                    cSendServMsg.SetMsgBody( (u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt) );
                    SendMsgToAllMcs( MCU_MCS_CALLMTMODE_NOTIF, cSendServMsg );

					NotifyMcsAlarmInfo( 0, ERR_MCU_CALLHDMTFAIL);
					RemoveJoinedMt( tMt, TRUE );
					return;
				}
			}
			else
			{
				//�Ѽ�����HD����������Ϣ��,��������
			}
		}
#endif
		
		//  [8/5/2011 chendaiwei]��8KE,��8000b���µĸ����������߼�
#ifndef _MINIMCU_
		//  [7/28/2011 chendaiwei]����������Ǹ�����ն���Ҫռ��һ����������,�Ǹ���MCUռ��������������
		BOOL32 bIsMcu = FALSE;
		
		if( tMt.GetMtType() == MT_TYPE_SMCU || tMt.GetMtType() == MT_TYPE_MMCU )
		{
			bIsMcu = TRUE;
		}
		
		if( !g_cMcuVcApp.IncMpcCurrentHDMtNum(m_byConfIdx, tMt.GetMtId(),bIsMcu) )
		{
            m_ptMtTable->SetCallMode( tMt.GetMtId(),CONF_CALLMODE_NONE );
            CServMsg cSendServMsg;
            cSendServMsg.SetEventId( MCU_MCS_CALLMTMODE_NOTIF );
            cSendServMsg.SetMsgBody( (u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt) );
            SendMsgToAllMcs( MCU_MCS_CALLMTMODE_NOTIF, cSendServMsg );
			NotifyMcsAlarmInfo( 0, ERR_MCU_CALLHDMTFAIL);
			RemoveJoinedMt( tMt, TRUE );
			
			return;
		}
#endif
	}
	
#endif


	// xliang [2/4/2009] ����Mt���ͣ�����岹����
	// ====================��˵����======================
	//1,��������������壬����֪�ն˵�����(MT/MCU),ֻ���ڴ˴�֪��MT���ͺ�
	//�����ն�����(MT/MCU)�������ֵ��(����MCU����������ֵռ2��)
	//����ֻ���ն�������SMCU������(outgoing)���в�����������MMCU(�Զ�incoming)��������
	//( ��mcu��mtadplib�ж��ڼ���������ȡ�˲��Գƴ�����֤����/���еļ�ʱ�� )
	//2,��û�ж����λ��ռ����
	//a)��ʾ����
	//------
	//��FIXME��:���ڽ����������ԭ����û������ģ�����Ƕ������HDI�干��ʱ
	//			��HD���������£�����岢�����ϸ��������,����Mt���ȷ��䵽HDI���ϣ�
	//			�����ͻ��������Ƕ���������2����λ�ɷ��䣬��ȴ������MCU�������
	//��solution��:�ı�ԭ�н����ľ������ԭ��(ѡ������Ŀ��С��һ��)��
	//             ��Ϊѡʣ�������������һ��// xliang [2/16/2009] 
	//------
	//b)��MT��Դ����װ�ں�����ִ�У�����Ҫ����Mt���ͣ�����ֵͳһ-1��������������MCU��-2.
	//3, Ŀǰ��֧��2����ͬ�������ṩ1��������������2����MCUռ2��λ��
	if( tMt.GetMtType() == MT_TYPE_SMCU /*|| tMt.GetMtType() == MT_TYPE_MMCU*/ )
	{
		// [9/9/2010 xliang] ������VCSǿ����ƣ�������жϿ������С��ҽ���崦�й��ˣ����ﲹ�����϶��ǿ��Եġ�
// 		if ( g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum +1 > g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_byMaxMtNum )
// 		{
// 			// xliang [2/16/2009]  ���ж�Ӧ�ý�����,��mtadp���ѽ��й��ˡ�
// 			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtCapSet]Take MCU as 2 into account of Mtadp access capabilty: fail! CRI.%d(cur: %d, max:%d)\n",
// 				byDriId, 
// 				g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum, 
// 				g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_byMaxMtNum );
// 			NotifyMcsAlarmInfo( 0, ERR_MCU_CALLHDMTFAIL);
// 			RemoveJoinedMt( tMt, TRUE, MTLEFT_REASON_NORMAL, TRUE);
// 			return;
// 		}
// 		else
// 		{
            g_cMcuVcApp.IncMtAdpMtNum( byDriId, m_byConfIdx, tMt.GetMtId(), TYPE_MCU);
			g_cMcuVcApp.UpdateAgentAuthMtNum();
			//g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum++;
			//AddMtExtraPoint(tMt.GetMtId());
// 		}
	}
//#endif

	//֪ͨ����MCU new mt
    NotifyMcuNewMt(tMt); 

    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMtCapSet] Mt main video cap: type.%d, Res.%d, HP.%d \n", tCapSupport.GetMainVideoType(), tCapSupport.GetMainVideoResolution(),tCapSupport.GetMainStreamProfileAttrb() );
	
	// vrs��¼��֧�֣���¼��¼��ʱ��Ҫ��vrs����ͨ�����й�ͬ�����Ƿ�����
	if (tMt.GetMtType() == MT_TYPE_VRSREC)
	{
		if (m_ptMtTable->GetRecChlType(tMt.GetMtId()) == TRecChnnlStatus::TYPE_RECORD)
		{
			if (!CheckVrsMtCapSet(tMt, tCapSupport))
			{
				//��ͬ������ƥ�䣬�Ҷ�vrsʵ��
				ReleaseVrsMt(tMt.GetMtId());
				return;
			}
			// ����vrs��ͬ���������ڽ������Ŀ�ͨ��
			m_ptMtTable->SetMtMultiCapSupport(tMt.GetMtId(), &tCapSupport);
		}
		else
		{
			return;
		}
	}

	u32	dwRcvIp;
	u16	wRcvPort;
	if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
	{
		dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
		if( dwRcvIp == 0 )
		{
			return;			    
		}
		
		g_cMcuVcApp.FindMatchedMpIpForMt(m_ptMtTable->GetIPAddr(tMt.GetMtId()),dwRcvIp);
		
		wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
		m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
	}

	
	/*TLogicalChannel	tAudioLogicChnnl;
    TLogicalChannel tVideoLogicChnnl;
	tAudioLogicChnnl.SetMediaType( MODE_AUDIO );
	tVideoLogicChnnl.SetMediaType( MODE_VIDEO );
	TCapSupport	tConfCapSupport = m_tConf.GetCapSupport();
	TSimCapSet tConfMainSCS = tConfCapSupport.GetMainSimCapSet();
	TSimCapSet tConfSecSCS  = tConfCapSupport.GetSecondSimCapSet();
    // guzh [8/28/2007] ���㲽������8000B/8000C������˫ý�����
#ifdef _MINIMCU_
    if ( ISTRUE(m_byIsDoubleMediaConf) )
    {
        // ��Ƶ�������ʲ�������ʽ�������ʲ��ø���ʽ�����ⴿ����������䣩
        u16 wMainBitrate = m_tConf.GetBitRate();
        u16 wSecBitrate = m_tConf.GetSecBitRate();
        u16 wDialBitrate = m_ptMtTable->GetDialBitrate(tMt.GetMtId());
        if ( wDialBitrate == wMainBitrate && 
             tCapSupport.IsSupportMediaType(tConfMainSCS.GetVideoMediaType()) ) 
        {
            tVideoLogicChnnl.SetChannelType(tConfMainSCS.GetVideoMediaType());	
        }
        else if ( wSecBitrate == wDialBitrate &&
                  tCapSupport.IsSupportMediaType(tConfSecSCS.GetVideoMediaType()) )
        {
            tVideoLogicChnnl.SetChannelType(tConfSecSCS.GetVideoMediaType());
        }
        else
        {
            tVideoLogicChnnl.SetChannelType(MEDIA_TYPE_NULL);
        }
        // ��Ƶ����ԭ����ͬ
        if (tCapSupport.IsSupportMediaType(tConfMainSCS.GetAudioMediaType()))
        {
            tAudioLogicChnnl.SetChannelType(tConfMainSCS.GetAudioMediaType());
        }			
        else if (tCapSupport.IsSupportMediaType(tConfSecSCS.GetAudioMediaType()))
        {
            tAudioLogicChnnl.SetChannelType(tConfSecSCS.GetAudioMediaType());
        }
        else
        {
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtCapSet] (MCU8000b)  tAudioLogicChnnl Set NULL!\n");
            tAudioLogicChnnl.SetChannelType(MEDIA_TYPE_NULL);
        }        
    }    
#endif
    //����������Ⱥ��
    BOOL32 bAddAccordMSC = TRUE;
    //��һ�����ж϶Է���ͬʱ�������������Ƿ��н���
    if( tConfMainSCS == tCapSupport.GetMainSimCapSet() || 
	    tConfMainSCS == tCapSupport.GetSecondSimCapSet() )
	{
		tAudioLogicChnnl.SetChannelType( tConfMainSCS.GetAudioMediaType() );
		tVideoLogicChnnl.SetChannelType( tConfMainSCS.GetVideoMediaType() );
	}
	else if( tConfSecSCS == tCapSupport.GetMainSimCapSet() || 
			 tConfSecSCS == tCapSupport.GetSecondSimCapSet() )
	{
		tAudioLogicChnnl.SetChannelType( tConfSecSCS.GetAudioMediaType() );
		tVideoLogicChnnl.SetChannelType( tConfSecSCS.GetVideoMediaType() );
        bAddAccordMSC = FALSE;
    }
	//�ڶ�����û�н�����ȷ���򿪶Է�����Ƶ����Ƶ����
	else
	{
		if( tCapSupport.IsSupportMediaType( tConfMainSCS.GetAudioMediaType() ) && 
			tCapSupport.IsSupportMediaType( tConfMainSCS.GetVideoMediaType() ) )
		{
			tAudioLogicChnnl.SetChannelType( tConfMainSCS.GetAudioMediaType() );
			tVideoLogicChnnl.SetChannelType( tConfMainSCS.GetVideoMediaType() );
		}
		else if( tCapSupport.IsSupportMediaType( tConfSecSCS.GetAudioMediaType() ) && 
			     tCapSupport.IsSupportMediaType( tConfSecSCS.GetVideoMediaType() ) )
		{
			tAudioLogicChnnl.SetChannelType( tConfSecSCS.GetAudioMediaType() );
			tVideoLogicChnnl.SetChannelType( tConfSecSCS.GetVideoMediaType() );	

            bAddAccordMSC = FALSE;
		}
		//��������ȷ�����ն˵�һ���߼�ͨ��������
		else
		{
            if (tCapSupport.IsSupportMediaType(tConfMainSCS.GetAudioMediaType()))
            {
                tAudioLogicChnnl.SetChannelType(tConfMainSCS.GetAudioMediaType());
            }			
            else if (tCapSupport.IsSupportMediaType(tConfSecSCS.GetAudioMediaType()))
            {
                tAudioLogicChnnl.SetChannelType(tConfSecSCS.GetAudioMediaType());
            }
            else
            {
                tAudioLogicChnnl.SetChannelType(MEDIA_TYPE_NULL);
            }
            if (tCapSupport.IsSupportMediaType(tConfMainSCS.GetVideoMediaType())) 
            {
                tVideoLogicChnnl.SetChannelType(tConfMainSCS.GetVideoMediaType());
            }
            else if (tCapSupport.IsSupportMediaType(tConfSecSCS.GetVideoMediaType()))
            {
                tVideoLogicChnnl.SetChannelType(tConfSecSCS.GetVideoMediaType());

                bAddAccordMSC = FALSE;
            }
		}
	}
    TSimCapSet tSimCap;
    tSimCap.Clear();
    if (bAddAccordMSC)
    {
        tSimCap = tCapSupport.GetMainSimCapSet();
    }
    else
    {
        tSimCap = tCapSupport.GetSecondSimCapSet();
    }
	*/

	TLogicalChannel	tAudioLogicChnnl;
    TLogicalChannel tVideoLogicChnnl;
	tAudioLogicChnnl.SetMediaType( MODE_AUDIO );
	tAudioLogicChnnl.SetChannelType(tCapSupport.GetMainAudioType());
	tAudioLogicChnnl.SetAudioTrackNum( tCapSupport.GetMainAudioTrackNum() );
	tVideoLogicChnnl.SetMediaType( MODE_VIDEO );
	tVideoLogicChnnl.SetChannelType(tCapSupport.GetMainVideoType());



	TCapSupport	tConfCapSupport = m_tConf.GetCapSupport();
#ifdef _MINIMCU_
	TSimCapSet tConfMainSCS = tConfCapSupport.GetMainSimCapSet();
	TSimCapSet tConfSecSCS  = tConfCapSupport.GetSecondSimCapSet();
    if ( ISTRUE(m_byIsDoubleMediaConf) )
    {
        // ��Ƶ�������ʲ�������ʽ�������ʲ��ø���ʽ�����ⴿ����������䣩
        u16 wMainBitrate = m_tConf.GetBitRate();
        u16 wSecBitrate = m_tConf.GetSecBitRate();
        u16 wDialBitrate = m_ptMtTable->GetDialBitrate(tMt.GetMtId());
        if ( wDialBitrate == wMainBitrate && 
			tCapSupport.IsSupportMediaType(tConfMainSCS.GetVideoMediaType()) ) 
        {
            tVideoLogicChnnl.SetChannelType(tConfMainSCS.GetVideoMediaType());	
        }
        else if ( wSecBitrate == wDialBitrate &&
			tCapSupport.IsSupportMediaType(tConfSecSCS.GetVideoMediaType()) )
        {
            tVideoLogicChnnl.SetChannelType(tConfSecSCS.GetVideoMediaType());
        }
        else
        {
            tVideoLogicChnnl.SetChannelType(MEDIA_TYPE_NULL);
        }       
    }    
	
#endif
    
    //zbq[10/13/2007] FEC�������㣬�ܾ����ն˵�ͨ��
	BOOL32 bFECOpenAudioChnnl = TRUE;
	BOOL32 bFECOpenVideoChnnl = TRUE;
	CServMsg cServMsg;	
	u16 wAudioBand = 0;

	
	//���ʹ��ն������߼�ͨ����Ϣ
	//�����޸�  ȡ��MTADP��ƥ�����������Ƶ���������ո���Ƶ�����򿪶Է��߼�ͨ��
	if( bFECOpenAudioChnnl &&
        tAudioLogicChnnl.GetChannelType() != MEDIA_TYPE_NULL )
	{		
		wAudioBand = GetAudioBitrate( tAudioLogicChnnl.GetChannelType() );
		tAudioLogicChnnl.SetFlowControl( wAudioBand );
		tAudioLogicChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
		tAudioLogicChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 3 );
		tAudioLogicChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
		tAudioLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, tAudioLogicChnnl.GetChannelType()));
		
		cServMsg.SetMsgBody( ( u8 * )&tAudioLogicChnnl, sizeof( tAudioLogicChnnl ) );
		SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );

		//��ӡ��Ϣ
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open audio logicChannel request send to Mt%d, type: %s ChnNum(%d)\n", 
				tMt.GetMtId(), GetMediaStr( tAudioLogicChnnl.GetChannelType() ) ,tAudioLogicChnnl.GetAudioTrackNum() );
		
	}
	else
	{
		VCSMTAbilityNotif(tMt, MODE_AUDIO);
		//��ӡ��Ϣ
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "[ProcMtCapSet] Mt%d not support any conf audio media type, NO open audio logicChannel request send to it!\n", tMt.GetMtId() );
	}	   

    //  xsl [4/26/2006] �ж��Ƿ���ն˵���Ƶ�߼�ͨ��
    BOOL32 bOpenVideoChnnl = TRUE;

    // guzh [2/26/2007] ͨ���ն��������ʼ��㣬    
    u16 wCallRate = m_ptMtTable->GetDialBitrate( tMt.GetMtId() );
    u16 wMtMaxRecvBitrate = 0;

    // ֻ��Keda�ն˲��������Ĳ��ԣ�����������ն�(Ŀǰֻ��Polycon)����MtAdp����    
    if ( MT_MANU_KDC == m_ptMtTable->GetManuId(tMt.GetMtId()) )
    {
		TSimCapSet tMtMainSCS = tCapSupport.GetMainSimCapSet();
		TSimCapSet tMtSecSCS  = tCapSupport.GetSecondSimCapSet();
        if ( !tMtMainSCS.IsNull() )
        {
            wMtMaxRecvBitrate = tMtMainSCS.GetVideoMaxBitRate();
        }
        if ( !tMtSecSCS.IsNull() )
        {
			const u16 wVideoMaxBitR = tMtSecSCS.GetVideoMaxBitRate();
            wMtMaxRecvBitrate = max( wMtMaxRecvBitrate, wVideoMaxBitR );
        }
        // guzh [7/16/2007] �ϰ汾�ն˵���Ƶ�߼�ͨ����������-64
        wMtMaxRecvBitrate += 64;

        wMtMaxRecvBitrate = min ( wMtMaxRecvBitrate, wCallRate );
    }
    else
    {
        wMtMaxRecvBitrate = wCallRate;
    }        

	wAudioBand = GetMaxAudioBiterate(m_tConfEx);
	if( wCallRate < wAudioBand )
	{
		bOpenVideoChnnl = FALSE;
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Open Mt.%d video logic chnnl failed due to wCallRate.%d < wAudioBand.%d\n",
            tMt.GetMtId(),wCallRate,wAudioBand );
	}

    
    //zbq[12/18/2007]�߼�ͨ���򿪵��ж��������д�������
    if ( wMtMaxRecvBitrate < (wCallRate-wAudioBand) )
    {
        bOpenVideoChnnl = FALSE;
        ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Open Mt.%d video logic chnnl failed due to ChnnlRcvBR.%d, DailBR.%d\n",
            tMt.GetMtId(), wMtMaxRecvBitrate, (wCallRate-wAudioBand) );
    }
	else
	{
		wMtMaxRecvBitrate = (wCallRate-wAudioBand);
	}

    /*
    if ( 0 == m_tConf.GetSecBitRate() )// ���ٻ���
    {
        if ( wMtMaxRecvBitrate < m_tConf.GetBitRate() ) 
        {
            bOpenVideoChnnl = FALSE;
        }
    }
    else //˫�ٻ���
    {
        if( wMtMaxRecvBitrate < m_tConf.GetSecBitRate() )
        {
            bOpenVideoChnnl = FALSE;
        } 
    }
    */
    /*
    //zbq[12/30/2008] �鿴�������Ƿ�֧�ָý���Ⱥ��
	u8 byChnRes = 0;
    BOOL32 bAccord2MainCap = FALSE;
    if (!IsMtMatchedSrc(tMt.GetMtId(), byChnRes, bAccord2MainCap))
    {
        bOpenVideoChnnl = FALSE;
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "Open Mt.%d video logic chnnl failed due to src<%d-%s> not matched!\n", 
            tMt.GetMtId(), tSimCap.GetVideoMediaType(), GetResStr(tSimCap.GetVideoResolution()));
    }*/

	//���ʹ��ն���Ƶ�߼�ͨ����Ϣ
	if( bFECOpenVideoChnnl &&
        bOpenVideoChnnl &&
        tVideoLogicChnnl.GetChannelType() != MEDIA_TYPE_NULL && 
		m_tConf.GetBitRate() >= 64 )
	{
		tVideoLogicChnnl.SetFlowControl( wMtMaxRecvBitrate );
		tVideoLogicChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
		tVideoLogicChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 1 );
		tVideoLogicChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
		tVideoLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, tVideoLogicChnnl.GetChannelType()));
        
		//ֱ��ȡ�ֱ��������߼�ͨ��[12/9/2011 chendaiwei]
		u8 byChnRes = 0;
		if (!GetMtMatchedRes(tMt.GetMtId(), tVideoLogicChnnl.GetChannelType(), byChnRes))
		{
			//zbq[04/22/2009] ��������ʹ��Ҳ�������������£����°������������
            if (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType())
            {
                tVideoLogicChnnl.SetChannelType(m_tConf.GetSecVideoMediaType());
                tVideoLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, tVideoLogicChnnl.GetChannelType()));
                byChnRes = m_tConf.GetSecVideoFormat();

                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtCapSet] no proper res for mt(%d) with mediatype(%d), try SecVidCap!\n", 
                    tMt.GetMtId(), tVideoLogicChnnl.GetChannelType());
            }
            else
            {
                bOpenVideoChnnl = FALSE;
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtCapSet] no proper res for mt(%d) with mediatype(%d)!\n", 
                    tMt.GetMtId(), tVideoLogicChnnl.GetChannelType());
            }
		}
		

		BOOL32 bAdjustCapEx = FALSE;
		TVideoStreamCap tSrcCapEx;
		TVideoStreamCap tAdjCapEx;

		//���֧�ָ����ն˺������ʵ����ֱ���(���ÿ���)
		if(bOpenVideoChnnl  
			&&  0 != m_tConf.GetSecBitRate() 
			&&  g_cMcuVcApp.IsSupportChgLGCResAcdBR()) 
		{
			//DialBR�ڴ洢ʱʡȥ��Ƶ��׼�����ؼ��в��ԣ���ʱ��������
            u16 wDailBR = m_ptMtTable->GetDialBitrate(tMt.GetMtId()) + 
                          GetAudioBitrate(m_tConf.GetMainAudioMediaType());

			if(tVideoLogicChnnl.GetChannelType() == MEDIA_TYPE_H264
				&& m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264)
			{
				tSrcCapEx = tCapSupport.GetMainSimCapSet().GetVideoCap();
				tSrcCapEx.SetResolution(byChnRes);

				if(GetMtLegalCapExAccord2CallBR(wDailBR,tSrcCapEx,tAdjCapEx))
				{
					bAdjustCapEx = TRUE;
					byChnRes = tAdjCapEx.GetResolution();
				}
			}
			
			if(bAdjustCapEx)
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtCapSet] Mt.%d's Fwd chn Media(%d)Res adjust from <%s,%d> to <%s,%d> due to CallBR.%d!\n",
					tMt.GetMtId(),
					tVideoLogicChnnl.GetChannelType(),
					GetResStr(tSrcCapEx.GetResolution()),
					tSrcCapEx.GetUserDefFrameRate(),
					GetResStr(tAdjCapEx.GetResolution()),
					tAdjCapEx.GetUserDefFrameRate());
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtCapSet]  not adjust Mt.%d's Fwd chn <Media(%d),Res(%d)> due to CallBR.%d!\n",
								tMt.GetMtId(), tVideoLogicChnnl.GetChannelType(), byChnRes, wDailBR);
			}
		}

		
		if ( bOpenVideoChnnl )
		{
			if(g_cMcuVcApp.IsSendFakeCap2Polycom() &&
			   MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
			   VIDEO_FORMAT_4CIF == tCapSupport.GetMainVideoResolution() &&
			   VIDEO_FORMAT_4CIF != m_tConf.GetMainVideoFormat() &&
			   IsHDConf(m_tConf))
			{
				byChnRes = VIDEO_FORMAT_HD720;
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtCapSet] Mt.%d 's video open res has been adjusted from 4CIF to 720p!\n");
			}


            //[4/20/2011 zhushengze]IMAX֡��ȡ��ͬ��������֡��
			// [pengjie 2010/6/2] �򿪶Զ��߼�ͨ��ʱ����֡��
			//                    ������1080/720 50/60֡�Ļ��飬���Ҫ�򿪶Զ�4cif/cif��ͨ������֡�ʽ���30֡��
			//                    Ҳ����mtadp�����ƣ��������²��mcuҵ��ĸ���,��֤mtadp�಻��֪,���Ի�����mcu��Ķ�
			// 1��ȡ֡��
			u8 byFps = 0;   
			u8 byChannelType = tVideoLogicChnnl.GetChannelType();

			if( byChannelType == m_tConf.GetSecVideoMediaType() )
			{
				if( MEDIA_TYPE_H264 == byChannelType )
				{
					// byFps = m_tConf.GetSecVidUsrDefFPS();
                    byFps = tCapSupport.GetSecVidUsrDefFPS();
				}
				else
				{
					// byFps = m_tConf.GetSecVidFrameRate();
                    byFps = tCapSupport.GetSecVidFrmRate();
				}
			}
			else
			{
				if( MEDIA_TYPE_H264 == byChannelType )
				{
					// byFps = m_tConf.GetMainVidUsrDefFPS();
                    byFps = tCapSupport.GetMainVidUsrDefFPS();
				}
				else
				{
					// byFps = m_tConf.GetMainVidFrameRate();
                    byFps = tCapSupport.GetMainVidFrmRate();
				}
			}

			// 2������֡��, ����������
			if( byFps > 30 )
			{
				if( VIDEO_FORMAT_4CIF == byChnRes || VIDEO_FORMAT_CIF == byChnRes || VIDEO_FORMAT_MPEG4 == byChnRes)
				{
					tVideoLogicChnnl.SetChanVidFPS( 25 ); 
				}
				else
				{
					tVideoLogicChnnl.SetChanVidFPS( byFps );
				}
			}
			else
			{
				tVideoLogicChnnl.SetChanVidFPS( byFps );
			}
			
			//3.��Ժ������ʿ��ܵ�����֡�ʷֱ���
			if(bAdjustCapEx)
			{
				byFps = tAdjCapEx.GetUserDefFrameRate();
				byChnRes = tAdjCapEx.GetResolution();
				tVideoLogicChnnl.SetChanVidFPS( byFps );
			}

			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMtCapSet] Open Mt%d Video LogicChannel,Set VidFPS: %d!\n",
				tMt.GetMtId(), tVideoLogicChnnl.GetChanVidFPS() );
			// End

			tVideoLogicChnnl.SetVideoFormat(byChnRes);

			
			//����HP/BP����
			if( VIDEO_FORMAT_4CIF == byChnRes || 
				VIDEO_FORMAT_CIF == byChnRes )
			{
				tVideoLogicChnnl.SetProfieAttrb( emBpAttrb );
			}
			else
			{
				tVideoLogicChnnl.SetProfieAttrb(tCapSupport.GetMainStreamProfileAttrb());
			}
			
			cServMsg.SetMsgBody( ( u8 * )&tVideoLogicChnnl, sizeof( tVideoLogicChnnl ) );
			cServMsg.CatMsgBody( &byChnRes, sizeof(u8) );
			SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
			
			//��ӡ��Ϣ
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMtCapSet] Open Video LogicChannel Request send to Mt%d, type: %s, res.%s, Fps:%d, HP:%d, BR:%d\n", 
				tMt.GetMtId(), GetMediaStr( tVideoLogicChnnl.GetChannelType() ), GetResStr(tVideoLogicChnnl.GetVideoFormat()), tVideoLogicChnnl.GetChanVidFPS(),tVideoLogicChnnl.GetProfileAttrb(),tVideoLogicChnnl.GetFlowControl() );
		}
		else
		{
			VCSMTAbilityNotif(tMt, MODE_VIDEO);			
		}
	}
	else
	{
		VCSMTAbilityNotif(tMt, MODE_VIDEO);		

		//��ӡ��Ϣ
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "[ProcMtCapSet] Mt%d not support any conf video media type, NO open video logicChannel request send to it!\n", tMt.GetMtId() );
	}


	//���ն˵�˫��ͨ�� 
// 	if( bFECOpenVideoChnnl &&
//         bOpenVideoChnnl &&
//         //m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) &&
// 		!m_tDoubleStreamSrc.IsNull() &&
// 		m_tConf.GetBitRate() >= 64 )
// 	{	
// 		// xliang [11/14/2008] polycomMCU��˫��ͨ���Ѿ��򿪣��˴������ظ�
// 		if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
// 			( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
// 			MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ) )
// 		{
// 			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "PolyMcu.%d's secvideo chan has been open somewhere else, ignore it\n");
// 		}
// 		if (MT_TYPE_MMCU == tMt.GetMtType())
// 		{
// 			if( m_tDoubleStreamSrc.GetMtId() != tMt.GetMtId() )
// 			{
// 				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "Mt%d is mmcu, settimer open double video chan. \n", tMt.GetMtId());
// 				SetTimer(MCU_SMCUOPENDVIDEOCHNNL_TIMER, 3000, tMt.GetMtId());
// 			}	
// 		}
// 		else
// 		{
//             BOOL32 bDStreamSrcLegal = FALSE;
// 
//             //zbq[01/11/2008] ˫��������Ҫαװ����ͨ��
//             if ( TYPE_MCUPERI == m_tDoubleStreamSrc.GetType() &&
//                  EQP_TYPE_RECORDER == m_tDoubleStreamSrc.GetEqpType() )
//             {
//                 tLogicalChannel.SetChannelType( m_tPlayEqpAttrib.GetDVideoType() );
//                 tLogicalChannel.SetSupportH239( m_tConf.GetCapSupport().IsDStreamSupportH239() );
// 				// fxh ��Ҫ˫��Դ�ķֱ�����Ϣ,�����ں�����ͨ��ʱ�ж��Ƿ�Ŀ�Ķ���֮ƥ��
// 				u16 wFileDSW = 0; 
// 				u16 wFileDSH = 0;
// 				u8  byFileDSType = 0;
// 				m_tPlayFileMediaInfo.GetDVideo(byFileDSType, wFileDSW, wFileDSH);
// 		     	u8 byFileDSRes = GetResByWH(wFileDSW, wFileDSH);
// 				tLogicalChannel.SetVideoFormat(byFileDSRes);
//                 bDStreamSrcLegal = TRUE;
//             }
//             else
//             {
//                 if ( m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ))
//                 {
//                     bDStreamSrcLegal = TRUE;
//                 }
//                 else
//                 {
//                     ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtCapSet] Get DS Src.%d's logic chnnl failed\n", m_tDoubleStreamSrc.GetMtId());
//                 }
//             }
// 
//             if ( bDStreamSrcLegal )
//             {
//                 McuMtOpenDoubleStreamChnnl(tMt, tLogicalChannel/*, tCapSupport*/);
//             }
// 		}
// 	}
//     else
//     {
//         ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mt%d not support any conf DVideo media type, NO open DVideo logicChannel request send to it!\n", tMt.GetMtId() );
//     }

	//�����ն˵������������ն˵�H.224����ͨ��
	if( tCapSupport.IsSupportH224())
	{
		tDataLogicChnnl.SetMediaType( MODE_DATA );
		tDataLogicChnnl.SetChannelType( MEDIA_TYPE_H224 );
		tDataLogicChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
		tDataLogicChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 7 );
		tDataLogicChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
		tDataLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, tDataLogicChnnl.GetChannelType()));
		
		cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );			
		SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
		
		//��ӡ��Ϣ
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open Data LogicChannel Request send to Mt%d, type: %s\n", 
			tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );			
	}

	// ��������ͨ��
    // guzh [1/19/2007] �����ж��Ƿ���������������,�û����¼�MCU�Ƿ���
	u8 byMtType = m_ptMtTable->GetMtType(tMt.GetMtId());
	if ( byMtType == MT_TYPE_SMCU &&
	 	 tCapSupport.IsSupportMMcu() &&
		 m_tConf.GetConfAttrb().IsSupportCascade() &&
         !g_cMcuVcApp.IsCasConfOverCap() &&         
		 m_tConfAllMcuInfo.GetSMcuNum() < MAXNUM_SUB_MCU )
	{
		//��addmcu���¼�mcu�ӵ���mcu��Ϣ���У�ģ�崴����ж���¼���ʱ�����֪�������϶��ٸ��¼�mcu��
		u16 wMcuIdx = INVALID_MCUIDX;
		u8 abyMcuId[MAX_CASCADEDEPTH-1];
		memset( &abyMcuId[0],0,sizeof(abyMcuId) );
		abyMcuId[0] = tMt.GetMtId();
		m_tConfAllMcuInfo.AddMcu( &abyMcuId[0],2,&wMcuIdx );
		m_ptConfOtherMcTable->AddMcInfo( wMcuIdx );	
		m_tConfAllMtInfo.AddMcuInfo( wMcuIdx, tMt.GetConfIdx() );

		tDataLogicChnnl.SetMediaType( MODE_DATA );
		tDataLogicChnnl.SetChannelType( MEDIA_TYPE_MMCU );

		//�򿪼���ͨ��ʱ����Ӧtcp������ip��port����Ϊ����弶����ַ
		TMt tDstMt = m_ptMtTable->GetMt(tMt.GetMtId());
		u32 dwCasIp = g_cMcuVcApp.GetMtAdpIpAddr(tDstMt.GetDriId()); //������
		tDataLogicChnnl.m_tRcvMediaChannel.SetIpAddr(dwCasIp);

        // ���� [6/15/2006] Ϊ�˲��ԡ��ϼ������¼�MCUʱ�������ü����˿�        
		tDataLogicChnnl.m_tRcvMediaChannel.SetPort( g_cMcuVcApp.GetSMcuCasPort() );
		
		cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );			
		SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
        
		// [6/14/2011 xliang] no need to set timer
// 		u16 wTimerSpan = g_cMcuVcApp.GetMmcuOlcTimerOut();	//��Ӧ������"MmcuOLCTimerOut"
// 		if( 0 != wTimerSpan )
// 		{
// 			m_abyCasChnCheckTimes[tMt.GetMtId()-1] = 1;
// 			SetTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER, wTimerSpan );
// 			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "SetTimer MCUVC_WAIT_CASCADE_CHANNEL_TIMER: %d (ms)\n", wTimerSpan );
// 		}

		//��ӡ��Ϣ
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open Data LogicChannel Request send to Mt%d, type: %s\n", 
				tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );			
	}
    else
    {
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Not send MMcu OLC to Mt%d for MtType.%d, RemoteCap.%d, ConfAtrb.%d, CasConfNum.%d, CasMcuNum.%d \n", 
                 tMt.GetMtId(),
                 byMtType,
                 tCapSupport.IsSupportMMcu(),
                 m_tConf.GetConfAttrb().IsSupportCascade(),
                 g_cMcuVcApp.GetMcuCasConfNum(),
                 m_tConfAllMcuInfo.GetSMcuNum() );
    }

	//���ݶԶ����������򿪶Զ˵�T.120����ͨ��
	if( tCapSupport.IsSupportT120() && 
		tConfCapSupport.IsSupportT120() && 
		( m_tConf.GetConfAttrb().GetDataMode() == CONF_DATAMODE_VAANDDATA ||
		  m_tConf.GetConfAttrb().GetDataMode() == CONF_DATAMODE_DATAONLY ) )
	{
		//�����������, ������ַ�򿪶Զ�ͨ��
		if ( !m_ptMtTable->IsNotInvited( tMt.GetMtId() ) )
		{
			tDataLogicChnnl.SetMediaType( MODE_DATA );
			tDataLogicChnnl.SetChannelType( MEDIA_TYPE_T120 );

			tDataLogicChnnl.m_tRcvMediaChannel.SetIpAddr(0);
			tDataLogicChnnl.m_tRcvMediaChannel.SetPort(0);
			
			cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );				
			SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );

			//��ӡ��Ϣ
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open Data LogicChannel Request send to Mt%d, type: %s as Master \n", 
					  tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );
		}

		// ������˱���, ����ַ��. ��KEDA MCU, ������.
		else
		{	
			tDataLogicChnnl.SetMediaType( MODE_DATA );
			tDataLogicChnnl.SetChannelType( MEDIA_TYPE_T120 );
		
			//ȡ�������DCS��ַ�Ͷ˿�
			u32 dwDcsIp  = g_cMcuVcApp.m_atPeriDcsTable[m_byDcsIdx-1].m_dwDcsIp;
			u16 wDcsPort = g_cMcuVcApp.m_atPeriDcsTable[m_byDcsIdx-1].m_wDcsPort;
			
			//��ַ��Ч
			if ( 0 != dwDcsIp && 0 != wDcsPort ) 
			{
				tDataLogicChnnl.m_tRcvMediaChannel.SetIpAddr(dwDcsIp);
				tDataLogicChnnl.m_tRcvMediaChannel.SetPort(wDcsPort);
				
				cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );			
				SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
				
				//�����������, �ȴ��¼��ն˴򿪱���ͨ��
				if ( !m_ptMtTable->IsMtIsMaster( tMt.GetMtId() ) )
				{
					//��ӡ��Ϣ
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open Data LogicChannel Request send to Mt%d, type: %s as master with no address ! \n", 
						      tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );
				}
				//��������, �ȴ��ϼ�MCU�򿪱���ͨ��
				else
				{
					//��ӡ��Ϣ
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open Data LogicChannel Request send to Mt%d, type: %s as slave with no address ! \n", 
						      tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );
				}				
			}
			else
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Get DCS Address 0x%x:%d failed in Open T120 channel !\n", dwDcsIp, wDcsPort );
			}	
		}
	}

    return;
}

/*====================================================================
    ������      ��ProcTimerReopenCascadeChannel
    ����        �������ٴδ򿪼���ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	07/01/11	4.0			�ű���		  ����
====================================================================*/
// void CMcuVcInst::ProcTimerReopenCascadeChannel()
// {
//     BOOL32 bNonOpenStill = FALSE;
//     BOOL32 bFailedThreeTimes = FALSE;
//     TLogicalChannel tDataLogicChnnl;
//     for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
//     {
//         if ( m_abyCasChnCheckTimes[byMtId-1] == 0 )
//         {
//             continue;
//         }
//         //��ೢ������ (3 * 10 + 3 * 60) s
//         if ( m_abyCasChnCheckTimes[byMtId-1] <= 2*MAXNUM_CHECK_CAS_TIMES )
//         {
//             m_abyCasChnCheckTimes[byMtId-1] ++;
// 
//             //�����ٴδ򿪼�������ͨ��
//             u8 byMtType = m_ptMtTable->GetMtType(byMtId);
//             // guzh [1/19/2007] �����ж��Ƿ���������������,�û����¼�MCU�Ƿ���
// 	        if ( byMtType == MT_TYPE_SMCU &&
//                 !g_cMcuVcApp.IsCasConfOverCap() &&                
// 		        m_tConfAllMcuInfo.GetSMcuNum() < MAXNUM_SUB_MCU )
//             {
//                 tDataLogicChnnl.SetMediaType( MODE_DATA );
//                 tDataLogicChnnl.SetChannelType( MEDIA_TYPE_MMCU );
//     
//                 //�򿪼���ͨ��ʱ����Ӧtcp������ip��port����Ϊ����弶����ַ
//                 TMt tDstMt = m_ptMtTable->GetMt(byMtId);
//                 u32 dwCasIp = g_cMcuVcApp.GetMtAdpIpAddr(tDstMt.GetDriId()); //������
//                 tDataLogicChnnl.m_tRcvMediaChannel.SetIpAddr(dwCasIp);
//     
//                 tDataLogicChnnl.m_tRcvMediaChannel.SetPort( g_cMcuVcApp.GetSMcuCasPort() );
//                 
//                 CServMsg cServMsg;
//                 cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );			
//                 SendMsgToMt( byMtId, MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
//                 
//                 if ( m_abyCasChnCheckTimes[byMtId-1] > MAXNUM_CHECK_CAS_TIMES )
//                 {
//                     bFailedThreeTimes = TRUE;
//                 }
//                 bNonOpenStill = TRUE;
//                 ConfLog( FALSE, "[ProcTimerReopenCascadeChannel] Open Mt.%d Cascade Channel failed %d time(s), retry now !\n", 
//                                  byMtId, m_abyCasChnCheckTimes[byMtId-1] );
//             }
//         }
//         else
//         {
//             m_abyCasChnCheckTimes[byMtId-1]  = 0;
//             ConfLog( FALSE, "[ProcTimerReopenCascadeChannel] Open Mt.%d Cascade Channel failed entirely !\n", byMtId );
//         }
//     }
// 
//     if ( bNonOpenStill )
//     {
//         if ( bFailedThreeTimes )
//         {
//             SetTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER, TIMESPACE_WAIT_CASCADE_CHECK * 6 );
//         }
//         else
//         {
//             SetTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER, TIMESPACE_WAIT_CASCADE_CHECK );
//         }
//     }
//     else
//     {
//         KillTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER );
//     }
// 	return;
// }

/*====================================================================
    ������      ��ProcMtMcuCapSetNotif
    ����        ���ն˷���MCU��������֪ͨ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/04/25    1.0         ���         ����
	04/05/17    3.0         ������         �޸�
	06/01/05	4.0			�ű���		  T120����
====================================================================*/
void CMcuVcInst::ProcMtMcuCapSetNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
    TMultiCapSupport tMultiCapSupport = *(TMultiCapSupport *)( cServMsg.GetMsgBody() );

    TCapSupportEx tCapSupportEx = *(TCapSupportEx*)(cServMsg.GetMsgBody()+sizeof(TCapSupport));
	u8   bMtHasVidCap = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMultiCapSupport) + sizeof(tCapSupportEx));

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, 
				"[ProcMtMcuCapSetNotif] MT.%d has vidcap(%d)!", tMt.GetMtId(), bMtHasVidCap);

// 	if( VCS_CONF == m_tConf.GetConfSource() && m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE )
// 	{
// 		if( tMultiCapSupport.GetMainVideoType() == MEDIA_TYPE_NULL)
// 		{
// 			TCapExtraSet tCapExtraSet;
// 			tMultiCapSupport.GetCapExtraSet(tCapExtraSet,0);
// 			if( tCapExtraSet.GetVideoStreamCap().GetMediaType() != MEDIA_TYPE_NULL )
// 			{
// 				TSimCapSet tMainSimCapSet;
// 				memcpy(&tMainSimCapSet,&tCapExtraSet,sizeof(TSimCapSet));
// 				tMultiCapSupport.SetMainSimCapSet(tMainSimCapSet);
// 			}
// 		}
// 	}

	u32 dwBitRate = 0;

	switch( CurState() )
	{
	case STATE_ONGOING:
		{
			//���ø��ն˵�������
			u8 byMtId = tMt.GetMtId();
			// m_ptMtTable->SetMtCapSupport(byMtId, &tCapSupport);
            m_ptMtTable->SetMtMultiCapSupport(byMtId, &tMultiCapSupport);
			m_ptMtTable->SetMtCapSupportEx(byMtId, &tCapSupportEx);
            if (g_bPrintMtMsg)
            {
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMtMcuCapSetNotif]capability specified by mt:\n");
                tMultiCapSupport.Print();
            }
			if (m_ptMtTable->IsMtCapSpecByMCS(byMtId))
			{
				TMultiCapSupport tCap;
				TVideoStreamCap tMainVideoCap;
				TVideoStreamCap tDStreamVidoCap; 

				m_ptMtTable->GetMtCapSupport(byMtId, &tCap);	
				tMainVideoCap   = m_ptMtTable->m_tMTInfoEx[byMtId].GetMainVideoCap();
				tDStreamVidoCap = m_ptMtTable->m_tMTInfoEx[byMtId].GetDStreamVideoCap();

				// ����MCSָ�����ն�����Ƶ��˫��������
				tCap.SetMainVideoType(tMainVideoCap.GetMediaType());
				tCap.SetMainVideoResolution(tMainVideoCap.GetResolution());
				if (tMainVideoCap.IsFrameRateUserDefined())
				{
					tCap.SetMainVidUsrDefFPS(tMainVideoCap.GetUserDefFrameRate());
				}
				else
				{
					tCap.SetMainVidFrmRate(tMainVideoCap.GetFrameRate());
				}

				tCap.SetDStreamMediaType(tDStreamVidoCap.GetMediaType());
				tCap.SetDStreamResolution(tDStreamVidoCap.GetResolution());
				if (tDStreamVidoCap.IsFrameRateUserDefined())
				{
					tCap.SetDStreamUsrDefFPS(tDStreamVidoCap.GetUserDefFrameRate());
				}
				else
				{
					tCap.SetDStreamFrameRate(tDStreamVidoCap.GetFrameRate());
				}

				//m_ptMtTable->SetMtCapSupport(byMtId, &tCap);
                m_ptMtTable->SetMtMultiCapSupport(byMtId, &tCap);
				
				m_ptMtTable->GetMtCapSupport(byMtId, &tMultiCapSupport);			
			}

            if (g_bPrintMtMsg)
            {
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMtMcuCapSetNotif]final mt capability:\n");
                tMultiCapSupport.Print();
            }
			
			//ȷ�����ն��Ѿ����
			if( m_tConfAllMtInfo.MtJoinedConf( byMtId ) )
			{
				//�����ն�������
				ProcMtCapSet( tMt, tMultiCapSupport, bMtHasVidCap);

				//��������
				if ( !m_ptMtTable->IsNotInvited( byMtId ) )
				{
					//��Ҫ�õ���ַ�Ͷ˿�
				}
				//���˱���
				else
				{
					//�ն˺ͻ����֧�����ݹ���
					if ( tMultiCapSupport.IsSupportT120() && 
						 m_tConf.GetCapSupport().IsSupportT120() && 
						 m_tConf.GetConfAttrb().GetDataMode() != CONF_DATAMODE_VAONLY )
					{
						//����DCS���Ӹ��ն�
						SendMcuDcsAddMtReq( byMtId );
					}	
				}
			}
			break;
		}


	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "CMcuVcInst: ProcMtMcuCapSetNotif() Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    ������      ��ProcMtMcuMtAliasNotif
    ����        ���ն˷��͸�MCU�ı���֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/31    3.0         
====================================================================*/
void CMcuVcInst::ProcMtMcuMtAliasNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	u8 byMtId = cServMsg.GetSrcMtId();
	TMtAlias tMtAlias = *( TMtAlias* )cServMsg.GetMsgBody();
	TMt tMt;
	TMcu  tMcu;

	TMt			atMt[MAXNUM_CONF_MT];
	TMtAlias	atMtAlias[MAXNUM_CONF_MT];
	u8			abyBuff[MAXNUM_CONF_MT*( sizeof(TMt) + sizeof(TMtAlias) )];
	u8			byMtNum = 0;
	u8			byLoop;
	u32			dwBuffSize;

	switch( CurState()) 
	{
	case STATE_ONGOING:
		
		m_ptMtTable->SetMtAlias( byMtId, &tMtAlias );

		//����ط��ͱ���֪ͨ��Ϣ 
		tMt = m_ptMtTable->GetMt( byMtId );
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof( TMt ) );
		cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof(tMtAlias ) );
		SendMsgToAllMcs( MCU_MCS_MTALIAS_NOTIF, cServMsg ); 		

		for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			if( m_tConfAllMtInfo.MtInConf( byLoop ) )
			{
				atMt[byMtNum] = m_ptMtTable->GetMt( byLoop );
				atMtAlias[byMtNum].SetNull();
                if(!m_ptMtTable->GetMtAlias(byLoop, mtAliasTypeH320Alias, &atMtAlias[byMtNum]))
                {
                    if( !m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeH320ID, &atMtAlias[byMtNum] ) )
                    {
                        if(!m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeH323ID, &atMtAlias[byMtNum] ))
                        {
                            if(!m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeE164, &atMtAlias[byMtNum] ))
                            {
                                m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeTransportAddress, &atMtAlias[byMtNum] );
                            }
                        }
                    }
                }				
				byMtNum++;
			}			
		}
		
		tMcu.SetMcu( LOCAL_MCUID );
		cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
		dwBuffSize = PackMtInfoListIntoBuf( atMt, atMtAlias, byMtNum, abyBuff, sizeof( abyBuff ) );
		cServMsg.CatMsgBody( abyBuff, (u16)dwBuffSize );
		SendMsgToAllMcs( MCU_MCS_GETALLMTALIAS_ACK, cServMsg );

        byMtNum = 0;
		memset( atMtAlias, 0, sizeof(atMtAlias) );
		memset( abyBuff, 0, sizeof(abyBuff) );
		for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
			{
				atMt[byMtNum] = m_ptMtTable->GetMt( byLoop );
				atMtAlias[byMtNum].SetNull();
                if(!m_ptMtTable->GetMtAlias(byLoop, mtAliasTypeH320Alias, &atMtAlias[byMtNum]))
                {
                    if(!m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeH320ID, &atMtAlias[byMtNum] ))
                    {
                        if(!m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeH323ID, &atMtAlias[byMtNum] ))
                        {
                            if(!m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeE164, &atMtAlias[byMtNum] ))
                            {
                                m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeTransportAddress, &atMtAlias[byMtNum] );
                            }
                        }
                    }
                }				
				byMtNum++;
			}			
		}

		dwBuffSize = PackMtInfoListIntoBuf( atMt, atMtAlias, byMtNum, abyBuff, sizeof( abyBuff ) );
		cServMsg.SetMsgBody( abyBuff, (u16)dwBuffSize );

		cServMsg.SetConfIdx( m_byConfIdx );
		cServMsg.SetConfId( m_tConf.GetConfId() );
		cServMsg.SetDstMtId( 0 );
		g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn( MCU_MT_JOINEDMTLISTID_ACK, cServMsg );
					
		break;
		
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: ProcMtMcuMtAliasNotif() Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );		
	}
}
/*====================================================================
    ����        ���Ƿ������ĳ�����͵��ն���ƭ�ն˻�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtBoardType, �ն�����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/02/05				�ܾ���
====================================================================*/
BOOL32 CMcuVcInst::IsMtCanCapabilityCheat( s8 *pachProductID )
{
	if( NULL == pachProductID )
	{
		return FALSE;
	}
	//MT_BOARD_8010
	return g_cMcuVcApp.IsMtBoardTypeCanCapabilityCheat( pachProductID );
}

/*====================================================================
    ����        ���ն˵Ǽǳɹ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/05    1.0         LI Yi         ����
    04/02/14    3.0         ������        �޸�
	11/08/04    4.6         �����        Bug00059851�޸�
====================================================================*/
void CMcuVcInst::ProcMtMcuConnectedNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	
    u8 byManuId = *(u8*)cServMsg.GetMsgBody();
    s8 achProductId[MAXLEN_PRODUCTID];
    s8 achVersionId[MAXLEN_VERSIONID];

    //������ȡ�Զ˵� VersionId �� ProductId
    if (cServMsg.GetMsgBodyLen() > sizeof(u8))
    {
        u8 *pbyMsgBody = cServMsg.GetMsgBody() + sizeof(byManuId);

        //versionId
        u8 byVersionIdLen = *pbyMsgBody;
        pbyMsgBody += sizeof(byVersionIdLen);
        
        memset(achVersionId, 0, sizeof(achVersionId));
        memcpy(achVersionId, pbyMsgBody, min(byVersionIdLen, MAXLEN_VERSIONID));
        
        m_ptMtTable->SetVersionId(tMt.GetMtId(), achVersionId);

        pbyMsgBody += byVersionIdLen;

        //productId
        u8 byProductIdLen = *pbyMsgBody;
        pbyMsgBody += sizeof(byProductIdLen);

        memset(achProductId, 0, sizeof(achProductId));
        memcpy(achProductId, pbyMsgBody, min(byProductIdLen, MAXLEN_PRODUCTID));

        m_ptMtTable->SetProductId(tMt.GetMtId(), achProductId);
    }
	
	TCapSupport tCapSupport;
	TCapSupportEx tCapSupportEx;
	TSimCapSet tSimCapSet;

	//���ó�����Ϣ
	m_ptMtTable->SetManuId( tMt.GetMtId(), byManuId );
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d: 0x%x(Dri:%d) connected with manu.%s, verId.%s, productId.%s!\n", 
		     tMt.GetMtId(), 
             m_ptMtTable->GetIPAddr(tMt.GetMtId()),
             tMt.GetDriId(),
             GetManufactureStr( byManuId ),
             achVersionId,
             achProductId);


    // xsl [10/27/2006] ��ɢ���鲻֧�ּ���
    if (m_tConf.GetConfAttrb().IsSatDCastMode() &&
        MT_TYPE_MT != m_ptMtTable->GetMtType(tMt.GetMtId()))
    {
        SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
        ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] sat conf don't support cascade.\n");  
        return;
    }

	tCapSupport = m_tConf.GetCapSupport();
	tCapSupportEx = m_tConf.GetCapSupportEx();

    TSimCapSet tSim = tCapSupport.GetMainSimCapSet();
    //  xsl [4/26/2006] ��������ʣ�����mcu�շ�����
//  tSim.SetVideoMaxBitRate(m_ptMtTable->GetDialBitrate( tMt.GetMtId()));
    if (tSim.GetVideoMaxBitRate() == 0)
    {
        tSim.SetVideoMaxBitRate(m_tConf.GetBitRate());
        tCapSupport.SetMainSimCapSet(tSim);
    }
    tSim = tCapSupport.GetSecondSimCapSet();
    if(!tSim.IsNull() && tSim.GetVideoMaxBitRate() == 0)
    {
//        tSim.SetVideoMaxBitRate(m_ptMtTable->GetDialBitrate( tMt.GetMtId()));
        tSim.SetVideoMaxBitRate(m_tConf.GetBitRate());
        tCapSupport.SetSecondSimCapSet(tSim);
    }	
//    tCapSupport.SetDStreamMaxBitRate(m_ptMtTable->GetDialBitrate( tMt.GetMtId()));
    tCapSupport.SetDStreamMaxBitRate(m_tConf.GetBitRate());

#ifdef _MINIMCU_
    if ( ISTRUE(m_byIsDoubleMediaConf) )
    {
        // �����ʲ�������ʽ�������ʲ��ø���ʽ�����ⴿ����������䣩
        // ������ʱ��������Ƶ��˫��ʽ����Ϊ˫��Ƶ��ʽ�Ļ���Ӧ�����޷�ͬ˫ý��ͬʱ�����ģ�BASͨ��������
        u16 wMainBitrate = m_tConf.GetBitRate();
        u16 wSecBitrate = m_tConf.GetSecBitRate();
        u16 wDialBitrate = m_ptMtTable->GetDialBitrate(tMt.GetMtId());
        if ( wDialBitrate == wMainBitrate ) 
        {
            tSim.Clear();
            tCapSupport.SetSecondSimCapSet(tSim);
        }
        else if ( /*wSecBitrate == wDialBitrate*/wDialBitrate < wMainBitrate && wDialBitrate >= wSecBitrate)
        {
            tSim = tCapSupport.GetSecondSimCapSet();
			// [12/31/2009 xliang] ������ʽ����Ƶ���˴����б���
			if (tCapSupport.GetSecAudioType() == MEDIA_TYPE_NULL)
			{
				tSim.SetAudioMediaType(tCapSupport.GetMainAudioType());
			}
            tCapSupport.SetMainSimCapSet(tSim);
            tSim.Clear();
            tCapSupport.SetSecondSimCapSet(tSim);
        }
        else
        {
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuConnectedNotif]Call mt%d with uncorrect bitrate(%d confbitrate(%d, %d))\n",
				      tMt.GetMtId(), wDialBitrate, wSecBitrate, wMainBitrate);
            tSim.Clear();
            tCapSupport.SetSecondSimCapSet(tSim);
        }           
    }    
#else 
	//////////////////////////////////////////////////////////////////////////
	//zjj20100205�侯����������ƭ����˫��˫��ʽ������������ʽδmp4��h264ʱ�Ͷ�ȡ�����ļ���
	//������ն��Ǹ��ٺ�����Ҹ��ն��ͺ��������ļ��оͷ���mp4-16cif�������նˣ��Դ��ն˺���ͨ��Ϊmp4-16cif
	//zjj20100208 ����Ǹ���������Ǻ�����ն��ͺ����������ļ������õĸ����ն��ͺžͲ�������ƭ
	if( m_ptMtTable->GetMtType( tMt.GetMtId() ) == MT_TYPE_MT &&
			IsConfDbCapDbBitrateCapH264OrMp4() &&
			!( IsHDConf(m_tConf) && g_cMcuVcApp.IsHDMt( achProductId ) )
		)
	{		
		/*if( m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) < m_tConf.GetBitRate() )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcMtMcuConnectedNotif] double speed and double mediaType,mt(%d) is low speed call,so send single mediaType\n",
				tMt.GetMtId()
				);

			TSimCapSet tSecCapset = tCapSupport.GetSecondSimCapSet();
			if( !tSecCapset.IsNull() )
			{
				if( MEDIA_TYPE_NULL == tSecCapset.GetAudioMediaType() )
				{
					tSecCapset.SetAudioMediaType( tCapSupport.GetMainAudioType() );
				}
				tCapSupport.SetMainSimCapSet( tSecCapset );
			}	
			TSimCapSet tNullSimCap;
			tNullSimCap.Clear();
			tCapSupport.SetSecondSimCapSet( tNullSimCap );
		}*/
		TSimCapSet tNullSimCap;
		TMtStatus tMtStatus;
		memset( &tMtStatus,0,sizeof( tMtStatus ) );
		if( m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) == m_tConf.GetBitRate() &&
			IsMtCanCapabilityCheat( achProductId ) )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcMtMcuConnectedNotif] MT PrdouctID:%s is in cheat list.",achProductId );
			if( ( tCapSupport.GetMainSimCapSet().GetVideoMediaType() == MEDIA_TYPE_MP4 &&
				tCapSupport.GetMainSimCapSet().GetVideoResolution() == VIDEO_FORMAT_16CIF ) ||
				( tCapSupport.GetSecondSimCapSet().GetVideoMediaType() == MEDIA_TYPE_MP4 &&
					tCapSupport.GetSecondSimCapSet().GetVideoResolution() == VIDEO_FORMAT_16CIF  )
				)
			{	
				
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "so send mp4-16cif conf cap to mt. SecAudioType(%d)\n",
					tCapSupport.GetSecAudioType() 
					);

				if( tCapSupport.GetSecondSimCapSet().GetVideoMediaType() == MEDIA_TYPE_MP4  )
				{
					tNullSimCap = tCapSupport.GetSecondSimCapSet();
					if (tCapSupport.GetSecAudioType() == MEDIA_TYPE_NULL)
					{
						tNullSimCap.SetAudioMediaType(tCapSupport.GetMainAudioType());
					}
					tCapSupport.SetMainSimCapSet( tNullSimCap );
				}
				tCapSupport.SetMainVideoType( MEDIA_TYPE_MP4 );
				tCapSupport.SetMainVideoResolution( VIDEO_FORMAT_16CIF );

				
				tNullSimCap.Clear();
				tCapSupport.SetSecondSimCapSet( tNullSimCap );
			}			
		}	
		else
		{
			if( m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) < m_tConf.GetBitRate() )
			{
				if( g_cMcuVcApp.IsLowCalledMtChlOpenMp4() )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcMtMcuConnectedNotif] Low Called Mt Request Open Mp4 Forward Channel.\n" );
					if( tCapSupport.GetSecondSimCapSet().GetVideoMediaType() == MEDIA_TYPE_MP4  )
					{
						tNullSimCap = tCapSupport.GetSecondSimCapSet();
						if (tCapSupport.GetSecAudioType() == MEDIA_TYPE_NULL)
						{
							tNullSimCap.SetAudioMediaType(tCapSupport.GetMainAudioType());
						}
						tCapSupport.SetMainSimCapSet( tNullSimCap );
					}
					tCapSupport.SetMainVideoType( MEDIA_TYPE_MP4 );
					//tCapSupport.SetMainVideoResolution( VIDEO_FORMAT_16CIF );
					tNullSimCap.Clear();
					tCapSupport.SetSecondSimCapSet( tNullSimCap );
				}
				else if( g_cMcuVcApp.IsLowCalledMtChlOpenH264() )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcMtMcuConnectedNotif] Low Called Mt Request Open H264 Forward Channel.\n" );
					if( tCapSupport.GetSecondSimCapSet().GetVideoMediaType() == MEDIA_TYPE_H264  )
					{
						tNullSimCap = tCapSupport.GetSecondSimCapSet();
						if (tCapSupport.GetSecAudioType() == MEDIA_TYPE_NULL)
						{
							tNullSimCap.SetAudioMediaType(tCapSupport.GetMainAudioType());
						}
						tCapSupport.SetMainSimCapSet( tNullSimCap );
					}
					tCapSupport.SetMainVideoType( MEDIA_TYPE_H264 );					
					tNullSimCap.Clear();
					tCapSupport.SetSecondSimCapSet( tNullSimCap );
				}
			}
		}
	}
#endif
	//////////////////////////////////////////////////////////////////////////    
    
    //zbq[02/04/2009] �����������Polycom����480p�ı���
    if (g_cMcuVcApp.IsSendFakeCap2Polycom() &&
        MT_MANU_POLYCOM == m_ptMtTable->GetManuId((tMt.GetMtId())) &&
        IsHDConf(m_tConf))
    {
        tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_4CIF);
    }

    //tandberg����debug����
    if (MT_MANU_TAIDE == m_ptMtTable->GetManuId((tMt.GetMtId())) &&
        IsHDConf(m_tConf))
    {
        if (g_cMcuVcApp.IsSendFakeCap2Taide() &&
			0 == strcmp("58", m_ptMtTable->GetVersionId(tMt.GetMtId())))
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] tanberg sd mt video resolution modify cif, versionid 58.\n");
        }
        else if (g_cMcuVcApp.IsSendFakeCap2TaideHD())
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_HD720);
        }
    }

	BOOL32 bClearCapEx = FALSE;
	
	if (MT_MANU_TAIDE == m_ptMtTable->GetManuId((tMt.GetMtId())) /*&& IsHDConf(m_tConf)*/)
    {
		//����
		if( tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 &&
			tCapSupport.GetSecVideoType() == MEDIA_TYPE_H263 )
		{
			TSimCapSet tMainSimCap;
			TSimCapSet tSecSimCap;
			ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_CALL,"[ProcMtMcuConnectNotif] Tanberg MT VersionId: %s\n",m_ptMtTable->GetVersionId(tMt.GetMtId()));

			//���ĳЩ�����ͺŵ�̩���նˣ�����H263�������Զˣ��Զ˻Ὺh263+ͨ�������º���ͨ������ʧ�ܣ�ѡ��H264�������͸��Զ� [8/4/2012 chendaiwei]
			if( 0 == strcmp("512", m_ptMtTable->GetVersionId(tMt.GetMtId())) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_CALL,"[ProcMtMcuConnectedNotif] Tanberg MT.send 264 ability to mt(%d)\n",tMt.GetMtId() );
			}
			else
			{
				tMainSimCap = tCapSupport.GetSecondSimCapSet();
				tMainSimCap.SetAudioMediaType(tCapSupport.GetMainAudioType());
				tCapSupport.SetMainSimCapSet( tMainSimCap );   // �ѵڶ�·�����һ·
			}

			tCapSupport.SetSecondSimCapSet(tSecSimCap);    // �ڶ�·��Ϊ��
		}
		if ( tCapSupport.GetSecVideoType() == MEDIA_TYPE_H264 )
		{
			tCapSupport.SetSecVideoType( MEDIA_TYPE_NULL );
		}
		
		//˫��
		if( tCapSupport.GetDStreamMediaType() == MEDIA_TYPE_H264 &&
			m_tConf.GetCapSupportEx().IsDDStreamCap() )
		{
			//��������Ǩ��
			TDStreamCap tDSSim;
			tDSSim.SetSupportH239(tCapSupport.IsDStreamSupportH239());
			tDSSim.SetMediaType(m_tConf.GetCapSupportEx().GetSecDSType());
			tDSSim.SetMaxBitRate(m_tConf.GetCapSupportEx().GetSecDSBitRate());
			tDSSim.SetFrameRate(m_tConf.GetCapSupportEx().GetSecDSFrmRate());
			tDSSim.SetResolution(m_tConf.GetCapSupportEx().GetSecDSRes());
			
			tCapSupport.SetDStreamCapSet(tDSSim);
			
			//�����չ
			bClearCapEx = TRUE;
		}
    }
	
	//huawei����debug����
	if (MT_MANU_HUAWEI == m_ptMtTable->GetManuId((tMt.GetMtId())))
    {
        /*if (g_cMcuVcApp.IsSendFakeCap2HuaweiSDEp() &&
            0 == strcmp("Huawei H.323 Stack Verion 1.1", m_ptMtTable->GetVersionId(tMt.GetMtId())))
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] huawei sd mt video resolution modify cif\n");
        }*/
		u8 byVideoFormat = 0;
        if ( m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&			 		
            g_cMcuVcApp.GetFakeCap2HuaweiSDEp(m_ptMtTable->GetIPAddr(tMt.GetMtId()),byVideoFormat) )
        {
			if(m_tConf.GetConfAttrbEx().IsResExCif() && VIDEO_FORMAT_CIF == byVideoFormat )
			{
				tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
			}
			if( m_tConf.GetConfAttrbEx().IsResEx4Cif() && VIDEO_FORMAT_4CIF == byVideoFormat )
			{
				tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_4CIF);
			}
			if( m_tConf.GetConfAttrbEx().IsResEx720() && VIDEO_FORMAT_HD720 == byVideoFormat )
			{
				tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_HD720);
			}
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,"[ProcMtMcuConnectedNotif] huawei sd mt video resolution modify to %d\n",byVideoFormat );
        }
    }

	// [12/11/2009 xliang] Aethra ��������
	if ( MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId()) )
	{
		//zbq 20091207 modify for suzhou renfang, ǿ��ȡ��AethraMt��H239��AethraMcu��Ȼ����
		TDStreamCap tDSCap = tCapSupport.GetDStreamCapSet();
		if (MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
			MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()))
		{
		}
		else
		{
			tDSCap.SetSupportH239(FALSE);
		}
		
		//˫˫�� ǿ�Ʒ��� ��һ˫����ֻ��Aethra�ڶ�˫��
		if (MEDIA_TYPE_H264 == tCapSupport.GetDStreamMediaType() &&
			tCapSupport.IsDStreamSupportH239() &&
			m_tConf.GetCapSupportEx().IsDDStreamCap())
		{
			TCapSupportEx tCapEx = m_tConf.GetCapSupportEx();
			
			tDSCap.SetMaxBitRate(tCapEx.GetSecDSBitRate());
			tDSCap.SetFrameRate(tCapEx.GetSecDSFrmRate());
			tDSCap.SetMediaType(tCapEx.GetSecDSType());
			tDSCap.SetResolution(tCapEx.GetSecDSRes());
			
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] dual cap adj from ExCap due to Aethra!\n");
		}
		
		tCapSupport.SetDStreamCapSet(tDSCap);
	}
	
	//[10/11/25 zhushz] �������а���H264����polycom��ACK
	if ( MEDIA_TYPE_H264 == tCapSupport.GetMainVideoType() && 
		 MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
		 !strcmp(achVersionId, "Release 7.5.4 - 04 Mar 2005"))
	{
		TSimCapSet tTempSimCap = tCapSupport.GetSecondSimCapSet();
		tCapSupport.SetMainSimCapSet(tTempSimCap);
		tCapSupport.SetSecVideoType(MEDIA_TYPE_NULL);
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] polycom mt can't support h264!\n");
	}

	//zjj20110802 �⳧���ն�û�к������ʺͻ������ʵ�ȡС����,���Է��ͻ�������ʱ�����������
	if( MT_MANU_KDC != m_ptMtTable->GetManuId(tMt.GetMtId()) &&
		MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tMt.GetMtId()) &&
		m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) != m_tConf.GetBitRate() )
	{
		tSimCapSet = tCapSupport.GetMainSimCapSet();
		tSimCapSet.SetVideoMaxBitRate( m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) );
		tCapSupport.SetMainSimCapSet( tSimCapSet );
		tSimCapSet = tCapSupport.GetSecondSimCapSet();
		if( MEDIA_TYPE_NULL != tSimCapSet.GetVideoMediaType() )
		{
			tSimCapSet.SetVideoMaxBitRate( m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) );
			tCapSupport.SetSecondSimCapSet( tSimCapSet );
		}
	}

	// ������˫����ͨ�������ļ����� [pengguofeng 7/24/2012]
	TSimCapSet tMainCapSet = tCapSupport.GetMainSimCapSet();
	TDStreamCap tDSCapSet = tCapSupport.GetDStreamCapSet();
	u32 dwIp = m_ptMtTable->GetIPAddr(tMt.GetMtId());

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] Main Conf Capset Type:%d Res:%d Fps:%d BR:%d\n",
		tMainCapSet.GetVideoMediaType(), tMainCapSet.GetVideoResolution(),tMainCapSet.GetUserDefFrameRate(), tMainCapSet.GetVideoMaxBitRate());
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] Double Conf Capset Type:%d Res:%d Fps:%d BR:%d\n",
		tDSCapSet.GetMediaType(), tDSCapSet.GetResolution(),tDSCapSet.GetUserDefFrameRate(), tDSCapSet.GetMaxBitRate());

	//     u32 dwIp = INET_ADDR("172.16.227.63");
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif]Mt:%d ManuId:%d IP:0x%x\n",
		tMt.GetMtId(), byManuId, dwIp);
	
	BOOL32 bSetFake = FALSE;
	BOOL32 bAuioFake = FALSE;
	TAudioTypeDesc tFakeAudioTypeDesc;
	u8 byFakeMask = 0;
	if( g_cMcuVcApp.GetFakeCapParamByMtInfo(dwIp,achProductId,byManuId,tMainCapSet, tDSCapSet,tFakeAudioTypeDesc,byFakeMask))
	{
		bSetFake = TRUE;

		u16 wMainStreamBR = tMainCapSet.GetVideoMaxBitRate();
		u16 wMaxAudioBR = GetMaxAudioBiterate(m_tConfEx);
		if ( wMainStreamBR > wMaxAudioBR)
		{
			wMainStreamBR -= wMaxAudioBR;
			u16 wMainAudioBitrate = GetAudioBitrate(m_tConf.GetMainAudioMediaType());
			//��������ӦС�ڵ��ڻ������ʼ�ȥ�����Ƶ����[6/25/2013 chendaiwei]
			if( wMainStreamBR > m_tConf.GetBitRate()+wMainAudioBitrate-wMaxAudioBR)
			{
				wMainStreamBR = m_tConf.GetBitRate()+wMainAudioBitrate-wMaxAudioBR;
			}
		}
		else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuConnectedNotif]Mt:%d faked MStream BitRate is too small<%d> error!\n",
				tMt.GetMtId(),wMainStreamBR);
			
			return;
		}
		
		tMainCapSet.SetVideoMaxBitRate(wMainStreamBR);

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif]Faked MainCapset Type:%d Res:%d Fps:%d BR:%d HP:%d\n",
			tMainCapSet.GetVideoMediaType(), tMainCapSet.GetVideoResolution(),
			tMainCapSet.GetUserDefFrameRate(), tMainCapSet.GetVideoMaxBitRate(),
			tMainCapSet.GetVideoCap().GetH264ProfileAttrb());
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif]Faked DoubleCapset Type:%d Res:%d Fps:%d BR:%d HP:%d\n",
			tDSCapSet.GetMediaType(), tDSCapSet.GetResolution(),
			tDSCapSet.GetUserDefFrameRate(), tDSCapSet.GetMaxBitRate(),
			tDSCapSet.GetH264ProfileAttrb());
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,"[ProcMtMcuConnectedNotif]Faked AudioTypeDesc type:%d,trackNum:%d\n",tFakeAudioTypeDesc.GetAudioMediaType()
			,tFakeAudioTypeDesc.GetAudioTrackNum());

		TVideoStreamCap tOrignalCap = tCapSupport.GetMainSimCapSet().GetVideoCap();
		TVideoStreamCap tOrignalSecCap = tCapSupport.GetSecondSimCapSet().GetVideoCap();
		TDStreamCap tOrignalDSCap = tCapSupport.GetDStreamCapSet();
		TVideoStreamCap tOrignalSecDSCap = m_tConf.GetCapSupportEx().GetSecDSVideoCap();
		//  [8/30/2012 chendaiwei]�ж�Faked�����������Ƿ����
		if( IS_MAINSTREAMCAP_FAKE(byFakeMask) &&
			( tMainCapSet.GetVideoMediaType() == MEDIA_TYPE_NULL || tOrignalCap == tMainCapSet.GetVideoCap())
			|| m_tConfEx.IsMSSupportCapEx(tMainCapSet.GetVideoResolution(),tMainCapSet.GetVideoMediaType() == MEDIA_TYPE_H264?tMainCapSet.GetUserDefFrameRate():tMainCapSet.GetVideoFrameRate(),tMainCapSet.GetVideoProfileType())
			|| tOrignalSecCap == tMainCapSet.GetVideoCap())
		{
			//����
			tCapSupport.SetMainSimCapSet(tMainCapSet);
			TSimCapSet tNullsimCapset;
			tCapSupport.SetSecondSimCapSet(tNullsimCapset);
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] Faked Main Capset unmatch with MCU Capbility!\n");
		}

		if ( IS_DOUBLESTREAMCAP_FAKE(byFakeMask) && 
			( tDSCapSet.GetMediaType() == MEDIA_TYPE_NULL ||
			(tOrignalDSCap.IsSupportH239() == tDSCapSet.IsSupportH239()
			&& tOrignalDSCap.GetVideoStremCap() == tDSCapSet.GetVideoStremCap())
				|| m_tConfEx.IsDSSupportCapEx(tDSCapSet.GetVideoStremCap().GetResolution(),tDSCapSet.GetMediaType() == MEDIA_TYPE_H264?tDSCapSet.GetUserDefFrameRate():tDSCapSet.GetFrameRate(),tDSCapSet.GetH264ProfileAttrb())
				|| (tOrignalSecDSCap.GetMediaType() == tDSCapSet.GetMediaType()
					&& tOrignalSecDSCap.GetResolution() == tDSCapSet.GetResolution()
						&& tOrignalSecDSCap.GetFrameRate() == tDSCapSet.GetFrameRate()) )
			)
		{
			//˫��
			if ( m_tConf.GetCapSupport().GetDStreamMediaType() != MEDIA_TYPE_NULL )
			{
				tCapSupport.SetDStreamCapSet(tDSCapSet);
				tCapSupportEx.SetSecDSType( MEDIA_TYPE_NULL );
			}
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] Faked DS Capset unmatch with MCU Capbility!\n");
		}

		if ( IS_AUDIODESC_FAKE(byFakeMask) )
		{
			bAuioFake = TRUE;
		}

	}

	cServMsg.SetMsgBody( ( u8 * )&tCapSupport, sizeof( tCapSupport ) );
	u8 bySupportActiveH263 = 0; // �Ƿ����û�غɱ�ʶ
	u8 bySendMSDetermine   = 1; // �Ƿ������Ӿ���
	if(m_ptMtTable->GetManuId(tMt.GetMtId()) != MT_MANU_RADVISION)
	{
		bySupportActiveH263 = 1;
	}
	cServMsg.CatMsgBody( ( u8 * )&bySupportActiveH263, sizeof( bySupportActiveH263 ) );
	cServMsg.CatMsgBody( ( u8 * )&bySendMSDetermine, sizeof( bySendMSDetermine ) );

	if (bClearCapEx)
	{
		/*		memset(&tCapSupportEx, 0, sizeof(tCapSupportEx));*/
		//  [7/7/2011 chendaiwei]memset����ֵΪ0������������Ϊ��
		tCapSupportEx.Clear();
	}

	// MCUǰ�����, zgc, 2007-09-27
	cServMsg.CatMsgBody( ( u8 * )&tCapSupportEx, sizeof( tCapSupportEx ) );

    //[6/16/2011 zhushz]iMax����ȡ�����ļ�������֡��
    /*u8 byFpsAdp = 0;
    if (m_tConf.GetConfAttrbEx().IsImaxModeConf())
    {
        byFpsAdp = g_cMcuVcApp.GetFpsAdp();
    }
    
	cServMsg.CatMsgBody(&byFpsAdp, sizeof(u8));
	*/

	// ��֯��չ��������ѡ��Mtadp [12/8/2011 chendaiwei]
	TVideoStreamCap atMSVideoCap[MAX_CONF_CAP_EX_NUM];
	u8 byCapNum = MAX_CONF_CAP_EX_NUM;
	//Fake�ն��Ѿ�α����������������ѡ[8/2/2012 chendaiwei]
	if(!bSetFake)
	{
		m_tConfEx.GetMainStreamCapEx(atMSVideoCap,byCapNum);
	}

	TVideoStreamCap atDSVideoCap[MAX_CONF_CAP_EX_NUM];
	u8 byDSCapNum = MAX_CONF_CAP_EX_NUM;
	//Fake�ն��Ѿ�α����������������ѡ[8/2/2012 chendaiwei]
	if(!bSetFake)
	{
		m_tConfEx.GetDoubleStreamCapEx(atDSVideoCap,byDSCapNum);
	}

	cServMsg.CatMsgBody((u8*)&atMSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
	cServMsg.CatMsgBody((u8*)&atDSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);

	//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
	//u8 byAudioTrackNum = tAudioType.GetAudioTrackNum();
	//cServMsg.CatMsgBody(&byAudioTrackNum,sizeof(byAudioTrackNum));
	TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//��Ƶ����
	//�ӻ���������ȡ������֧�ֵ���Ƶ����
	if ( bAuioFake )
	{
		if ( m_tConfEx.IsSupportAudioMediaType(tFakeAudioTypeDesc) )
		{
			atAudioTypeDesc[0].SetAudioMediaType(tFakeAudioTypeDesc.GetAudioMediaType());
			atAudioTypeDesc[0].SetAudioTrackNum(tFakeAudioTypeDesc.GetAudioTrackNum());
		}
		//����û��Ҫ���ٵ�������ֱ�ӱ����յ���Ƶ���ն�
	}
	else
	{
		m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
	}
		
	cServMsg.CatMsgBody((u8*)&atAudioTypeDesc[0], sizeof(TAudioTypeDesc)* MAXNUM_CONF_AUDIOTYPE);

	SendMsgToMt( tMt.GetMtId(), MCU_MT_CAPBILITYSET_NOTIF, cServMsg );
	
	//MCUͬ���ն������û���Ļ������룬���ںϲ������������֤У��
	CServMsg cServMsgNtf;
	cServMsgNtf.SetMsgBody((u8*)m_tConf.GetConfPwd(), MAXLEN_PWD);
	cServMsgNtf.SetEventId(MCU_MT_CONFPWD_NTF);
	BroadcastToAllSubMtJoinedConf( MCU_MT_CONFPWD_NTF, cServMsgNtf );

	return;
}

/*====================================================================
    ������      ��ProcMcuMtDisonnectedNotif
    ����        ���ն˶���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/05    1.0         LI Yi         ����
    04/02/14    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::ProcMtMcuDisconnectedNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8  byReason = *(u8*)cServMsg.GetMsgBody();     // MTLEFT_REASON_*
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );	
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d Disced for %s.%d\n", tMt.GetMtId(), 
             GetMtLeftReasonStr(byReason), byReason );
	TMtStatus tMtStatus;

	TMt tVcsVCMT = m_cVCSConfStatus.GetCurVCMT();
	
	switch( CurState() )
	{
		
	case STATE_ONGOING:	


		
		// ��ǰ������ն˶����������kill��ʱ�������������һ����������
		if(VCS_CONF == m_tConf.GetConfSource() &&
		   tMt == m_cVCSConfStatus.GetReqVCMT())
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt.%d is vcs ReqMt,Kill req timer\n",tMt.GetMtId() );
			KillTimer(MCUVC_VCMTOVERTIMER_TIMER);

			TMt tNull;
			tNull.SetNull();
			m_cVCSConfStatus.SetReqVCMT(tNull);
		}

		//zjj20100330 vcs��������������Ҷϣ��Ͱѳ�ʱ�Ҷ�����Ϊ�ն˲��ɴ�
		if( VCS_CONF == m_tConf.GetConfSource() &&
			MTLEFT_REASON_LOCAL == byReason )
		{
			
			if( m_ptMtTable->GetMtStatus( tMt.GetMtId(),&tMtStatus ) )
			{
				if( tMtStatus.GetMtDisconnectReason() != MTLEFT_REASON_NORMAL )
				{
					if( tMtStatus.GetMtDisconnectReason() != MTLEFT_REASON_UNREACHABLE )
					{
						tMtStatus.SetMtDisconnectReason( MTLEFT_REASON_UNREACHABLE );
						m_ptMtTable->SetMtStatus( tMt.GetMtId(),&tMtStatus );
						if( !m_tCascadeMMCU.IsNull() )
						{
							OnNtfMtStatusToMMcu( m_tCascadeMMCU.GetMtId(),tMt.GetMtId() );
						}
						else
						{
							MtStatusChange( &tMt,TRUE );
						}
					}
				}
			}
		}

		if( MTLEFT_REASON_NORMAL   == byReason ||
            MTLEFT_REASON_REJECTED == byReason ||
            // zbq [05/14/2007] �ն˺������Ͳ�ƥ�䴦��
            MTLEFT_REASON_UNMATCHTYPE == byReason )
		{
            // guzh [4/16/2007] ������¼�֧�ּ���ϼ�MCU�ظ����У����޸ĺ��в��ԣ������ظ�����
            if ( (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU &&
                  m_ptMtTable->GetManuId(tMt.GetMtId()) == MT_MANU_KDCMCU &&
                  g_cMcuVcApp.IsAutoDetectMMcuDupCall()) ||
				  // VCS�������ģʽ���ض����壬���������޸ģ����Զ˾ܾ�ʱ������Ϊ�ֶ�ģʽ�����ⲻ�ϵ�����ʾ��
				  (VCS_CONF == m_tConf.GetConfSource() && (tMt == m_tConf.GetChairman() || tMt == m_cVCSConfStatus.GetVcsBackupChairMan() )
				  ) )
            {
            }
            else
            {
				if( CONF_CALLMODE_NONE != m_ptMtTable->GetCallMode( tMt.GetMtId()) )
				{
					//����ն˺���ģʽ
					m_ptMtTable->SetCallMode( tMt.GetMtId(),  CONF_CALLMODE_NONE );
            
					//�������ģʽ����������չ�ṹ����Ҫ����ӦRejectԭ���µ��ն���Ϣ���ܵ����������
					//ʵ������Normalԭ���£��ն���ᴦ��Ҳ�ᷢ����Ϣ����Ϊ��֤��ض��ն�״̬�����һ���ԣ����ﲻ����ǡ�
					cServMsg.SetEventId( MCU_MCS_CALLMTMODE_NOTIF );
					cServMsg.SetMsgBody( (u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt) );
					SendMsgToAllMcs( cServMsg.GetEventId(), cServMsg );
				}
            }
        }

		

		if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
		{
			NotifyMcuDropMt(tMt);				
		}
		
		// vrs��¼��ʵ�壬����¼���߼�
		if (tMt.GetType() == TYPE_MT && tMt.GetMtType() == MT_TYPE_VRSREC)
		{
			ReleaseVrsMt(tMt.GetMtId());
			return;
		}
        else if( MTLEFT_REASON_NORMAL != byReason  )
		{
            if (MTLEFT_REASON_LOCAL == byReason && 
				m_tConf.HasChairman() && 
				m_ptMtTable->GetAddMtMode(tMt.GetMtId()) == ADDMTMODE_CHAIRMAN)
            {
                TMtAlias tMtAlias;
                BOOL32 bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias);
				if (!bRet)
                {
					bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias);
					if (!bRet)
					{
						bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias);
						if (!bRet)
						{
							bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeE164, &tMtAlias);
							if (!bRet)
							{
								ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "m_ptMtTable->GetMtAlias Fail, mtid = %d\n", tMt.GetMtId());
							}
						}
					}
                }
				if (bRet)
				{
					cServMsg.SetEventId(MCU_MT_ADDMT_NACK);
					cServMsg.SetMsgBody((u8 *)&tMtAlias, sizeof(TMtAlias));
					SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_ADDMT_NACK, cServMsg);
				}

				//֪ͨɾ���ն�
				RemoveMt(tMt, FALSE);
            }
            else if (MT_TYPE_MMCU == tMt.GetMtType() ||
                     m_ptMtTable->IsNotInvited(tMt.GetMtId()) ||
					 MTLEFT_REASON_REMOTERECONNECT == byReason
                     /*m_ptMtTable->GetAddMtMode(tMt.GetMtId()) == ADDMTMODE_MTSELF*/)
            {
				//�����ϼ�MCU������Ϊ�������նˣ��Ͽ����Զ�ɾ�����б�
				RemoveMt(tMt, FALSE);
            }
            else
            {
			    RemoveJoinedMt( tMt, FALSE, byReason );
            }
		}
		else
		{
            if ( MT_TYPE_MMCU == tMt.GetMtType() ||
                 m_ptMtTable->IsNotInvited(tMt.GetMtId()) )
			{
				//֪ͨɾ���ն�
				RemoveMt(tMt, FALSE);
			}
			else
			{
				//֪ͨ�Ҷ��ն�
				RemoveJoinedMt( tMt, FALSE, byReason );
			}
			
			//��������ն˱�, ������Ӧ״̬

			SendMtListToMcs( LOCAL_MCUIDX );
		}

        //[9/14/2011 zhushengze]�ն�mtc-box���뷢��״̬�㲥���¼���֪�����ϼ������б�
        //������¼��쳣�Ͽ����ϼ�ͨ�淢���������������ϼ��Ͽ�ͳһ���㲥��
        if ( tMt.GetMtType() == MT_TYPE_MMCU )
        {
            for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
            {
                if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) )
                {
                    TMt tMt2 = m_ptMtTable->GetMt( byMtId );
                    if (tMt2.GetMtType() == MT_TYPE_MT)
                    {
                        NotifyMtSpeakStatus(tMt2, emDenid);
                    }
                    else
                    {
                        TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tMt2.GetMtId()));

						// 2011-11-2 add by pgf: ��ָ��������
						if ( !ptConfMcInfo)
						{
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "Get Mc Info Of MtId:%d Error\n", byMtId);
							continue;
						}

                        for (u8 bySmcuMtId = 1; bySmcuMtId <= MAXNUM_CONF_MT; bySmcuMtId++)
                        {
                            if (m_tConfAllMtInfo.MtJoinedConf((TMt)ptConfMcInfo->m_atMtExt[bySmcuMtId - 1]))
                            {
                                NotifyMtSpeakStatus((TMt)ptConfMcInfo->m_atMtExt[bySmcuMtId - 1], emDenid);
                            }
                        }
                    }                    
                }                
            }
            ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuDisconnectedNotif]mmcu disconnect notify all mt speakstatus!\n");
        }

        // xsl [11/8/2006] �ͷŽ�����ת������Դ
        ReleaseMtRes(tMt.GetMtId());
		
		// [11/29/2011 liuxu] 
		RemoveMcu(tMt);
		//�����ն˱��ط����ˣ���ձ��ط��������
		if ( VCS_CONF == m_tConf.GetConfSource() && tMt == tVcsVCMT )
		{
//			if(  && 
//				!m_tConf.GetChairman().IsNull() &&
//				( VCS_GROUPCHAIRMAN_MODE == m_cVCSConfStatus.GetCurVCMode() ||
//				VCS_GROUPSPEAK_MODE == m_cVCSConfStatus.GetCurVCMode() )
//				)
			{
				TMt tSpeakMt;
				
				if( CONF_SPEAKMODE_ANSWERINSTANTLY == m_tConf.GetConfSpeakMode() &&
					!m_tApplySpeakQue.IsQueueNull() )
				{
					if( m_tApplySpeakQue.GetQueueHead(tSpeakMt) ) 
					{
						//ChangeSpeaker( &tSpeakMt );
						
						//tSpeakMt.SetConfIdx( m_byConfIdx );
						//m_cVCSConfStatus.SetCurVCMT( tSpeakMt );
						
						//VCSConfStatusNotif();
						
						//NotifyMcsApplyList();
						ChgCurVCMT( tSpeakMt );
					}
					else
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMcuMcuDropMtNotify] Fail to get Queue Head!\n" );
					}
				}
				else
				{
					//tSpeakMt = m_tConf.GetChairman();
					//ChangeSpeaker( &tSpeakMt );
					tSpeakMt.SetNull();
					ChgCurVCMT( tSpeakMt );
					
					//tSpeakMt.SetNull();
					//m_cVCSConfStatus.SetCurVCMT( tSpeakMt );
					//VCSConfStatusNotif();
				}
				
			}		
		}
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcMcuMtDisconnectedNotif] Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMtMcuCallFailureNotify
    ����        ���ն˺���ʧ��֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/11/23    3.5        ������        ����
====================================================================*/
void CMcuVcInst::ProcMtMcuCallFailureNotify(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

	if( STATE_ONGOING == CurState() )
	{
		CallFailureNotify(cServMsg);
	}
	else
	{
//		ConfLog( FALSE, "Message %u(%s) received in state %u!\n", 
//			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
	}

	return;
}

/*=============================================================================
    �� �� ���� CallFailureNotify
    ��    �ܣ� ����ʧ��֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CServMsg	&cServMsg
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/09/08  4.0			�                   ����
=============================================================================*/
void CMcuVcInst::CallFailureNotify(CServMsg	&cServMsg)
{	
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	
	u8* pbyMsgBody = cServMsg.GetMsgBody();
	pbyMsgBody += sizeof(TMtAlias);
	u8 byReason = *(u8*)pbyMsgBody;
	pbyMsgBody += sizeof(u8);

	// ��ǰ������ն˺���ʧ�ܣ������kill��ʱ�������������һ����������
	if(VCS_CONF == m_tConf.GetConfSource() &&
	   tMt == m_cVCSConfStatus.GetReqVCMT())
	{
		KillTimer(MCUVC_VCMTOVERTIMER_TIMER);

		TMt tNull;
		tNull.SetNull();
		m_cVCSConfStatus.SetReqVCMT(tNull);		
	}	
	
	if( ( MTLEFT_REASON_REJECTED == byReason || 
		MTLEFT_REASON_UNREACHABLE == byReason ||
		ERR_MCU_RAS_GK_REJECT == cServMsg.GetErrorCode() ) &&
		VCS_CONF == m_tConf.GetConfSource() &&
		!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) )
	{
		VCSClearTvWallChannelByMt( tMt,TRUE );
	}
	
	BOOL32 bIsDisconnectReasonChg = FALSE;
	BOOL32 bIsCallModeChg = FALSE;
	
	//zjj20100327
	TMtStatus tMtStatus;
	if( m_ptMtTable->GetMtStatus(tMt.GetMtId(),&tMtStatus) )
	{
		if( tMtStatus.GetMtDisconnectReason() != byReason )
		{
			bIsDisconnectReasonChg = TRUE;
			tMtStatus.SetMtDisconnectReason( byReason );
			m_ptMtTable->SetMtStatus( tMt.GetMtId(),&tMtStatus );
		}		
	}	
	//zhouyiliang 20110117 ������ϯҲҪ���ֶ�ʱ����
	if( m_tConf.GetConfSource() == VCS_CONF &&
		( MTLEFT_REASON_BUSY == byReason || MTLEFT_REASON_REJECTED == byReason ) &&
		!( tMt==m_tConf.GetChairman() || tMt == m_cVCSConfStatus.GetVcsBackupChairMan() )
		 )
	{
		if( m_ptMtTable->GetCallMode( tMt.GetMtId() ) != CONF_CALLMODE_NONE )
		{
			bIsCallModeChg = TRUE;
			m_ptMtTable->SetCallMode( tMt.GetMtId(),CONF_CALLMODE_NONE );
		}		
	}

	u8 byConfLevel = 0;
	s8 achConfName[MAXLEN_CONFNAME] = {0};
	if (MTLEFT_REASON_BUSYEXT == byReason)
	{
		byConfLevel = *(u8*)pbyMsgBody;
		pbyMsgBody += sizeof(byConfLevel);

		memcpy(achConfName, pbyMsgBody, MAXLEN_CONFNAME);

		// ��ǰ�ն��ڼ�����ߵĻ����в�������ռ
		if (byConfLevel > m_tConf.GetConfLevel())
		{
			byReason = MTLEFT_REASON_BUSY;
			cServMsg.SetErrorCode(ERR_MCU_MTBUSY);

			//zjj20100927 ����ն��ڸߵȼ�������һ�ɸĳ��ֶ�����( except for chairman and backup-chairman )
			if( m_tConf.GetConfSource() == VCS_CONF 
				&& CONF_CREATE_MT != m_byCreateBy
				&& !( tMt == m_tConf.GetChairman() ) 
				&& !(tMt == m_cVCSConfStatus.GetVcsBackupChairMan())
				)
			{
				if( m_ptMtTable->GetCallMode( tMt.GetMtId() ) != CONF_CALLMODE_NONE )
				{
					bIsCallModeChg = TRUE;
					m_ptMtTable->SetCallMode( tMt.GetMtId(),CONF_CALLMODE_NONE );
				}
					
			}	

			if( VCS_CONF == m_tConf.GetConfSource() && !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
			{
				VCSClearTvWallChannelByMt( tMt,TRUE );
			}
		}
	}
	
	cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
	if (MTLEFT_REASON_BUSYEXT == byReason)
	{
		cServMsg.CatMsgBody((u8*)achConfName, sizeof(achConfName));
	}

	// ���ڱ����ն�ǿ����������,ֱ�ӽ���ǿ��
	u8 byAutoForceCall = FALSE;
	if (VCS_CONF == m_tConf.GetConfSource() &&
		MTLEFT_REASON_BUSYEXT == byReason &&
		( tMt == m_tConf.GetChairman()  ||
			 CONF_CREATE_MT == m_byCreateBy ||
			 tMt == m_cVCSConfStatus.GetVcsBackupChairMan()
			 )		
		)
	{
		byAutoForceCall = TRUE;
	}	
	else
	{
	    SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg ); 
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[CallFailureNotify] tMt(%d.%d) Is byAutoForceCall(%d)\n",
		tMt.GetMcuId(),
		tMt.GetMtId(),
		byAutoForceCall 
		);

	//zjj20100327
	//vcs�ϱ��ն˺���ʧ��ԭ��
	if( VCS_CONF == m_tConf.GetConfSource() &&		 
		0 != byReason &&
		( bIsDisconnectReasonChg || bIsCallModeChg )
		)
	{		
		if( CONF_CREATE_MT == m_byCreateBy &&
			!m_tCascadeMMCU.IsNull() )
		{
			OnNtfMtStatusToMMcu( m_tCascadeMMCU.GetMtId(),tMt.GetMtId() );
		}
		else
		{
			MtStatusChange( &tMt,TRUE );
		}
	}
	         
    if (m_tConf.HasChairman() &&
        m_ptMtTable->GetAddMtMode(tMt.GetMtId()) == ADDMTMODE_CHAIRMAN )
    {
        TMtAlias tMtAlias;
        BOOL32 bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias);
		if (!bRet)
        {
			bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias);
			if (!bRet)
			{
				bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias);
				if (!bRet)
				{
					bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeE164, &tMtAlias);
					if (!bRet)
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "m_ptMtTable->GetMtAlias Fail, mtid = %d\n", tMt.GetMtId());
					}
				}
			}
		}
		if (bRet)
		{
			cServMsg.SetEventId(MCU_MT_ADDMT_NACK);
			cServMsg.SetMsgBody((u8 *)&tMtAlias, sizeof(TMtAlias));
			SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_ADDMT_NACK, cServMsg);
		}

		//֪ͨɾ���ն�
		RemoveMt(tMt, FALSE);
    }

    // xsl [11/8/2006] �ͷŽ�����ת������Դ
    ReleaseMtRes(tMt.GetMtId()); 

	if (byAutoForceCall &&
		// ����ƽ���������Զ�ǿ��,����ȴ��Զ��ͷ�
			( tMt == m_tConf.GetChairman()  ||
			 CONF_CREATE_MT == m_byCreateBy || 
			 tMt == m_cVCSConfStatus.GetVcsBackupChairMan() ||
			 byConfLevel != m_tConf.GetConfLevel() )
		)
	{
		cServMsg.SetConfId( m_tConf.GetConfId() );
		cServMsg.SetNoSrc();
		cServMsg.SetMsgBody( NULL, 0 );
		InviteUnjoinedMt(cServMsg, &tMt, FALSE, FALSE, VCS_FORCECALL_CMD);
	}

	return;
}

/*=============================================================================
    �� �� ���� ProcMtMcuMtTypeNotif
    ��    �ܣ� �ն�����֪ͨ��320����ʱ�Ķ���֪ͨ��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/19  3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMtMcuMtTypeNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	u8 byMtType = *(cServMsg.GetMsgBody());

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		//�����ն�����
		if( TYPE_MCU == byMtType )
		{
			m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_SMCU );

			TLogicalChannel	tDataLogicChnnl;
			u32	dwRcvIp  = 0;
			u16	wRcvPort = 0;
			TMtAlias tMtAlias;
			if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
			{
				dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
				if( dwRcvIp == 0 )
				{
					return;			    
				}
				g_cMcuVcApp.FindMatchedMpIpForMt(m_ptMtTable->GetIPAddr(tMt.GetMtId()),dwRcvIp);

				wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
				m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
			}

			//��������ͨ��
			if( m_ptMtTable->GetDialAlias( tMt.GetMtId(), &tMtAlias ) && 
				mtAliasTypeH320ID == tMtAlias.m_AliasType && 
				m_tConf.GetConfAttrb().IsSupportCascade() &&
				m_tConfAllMcuInfo.GetSMcuNum() < MAXNUM_SUB_MCU )
			{
				tDataLogicChnnl.SetMediaType( MODE_DATA );
				tDataLogicChnnl.SetChannelType( MEDIA_TYPE_MMCU );
				
				tDataLogicChnnl.m_tRcvMediaChannel.SetIpAddr(dwRcvIp);
				tDataLogicChnnl.m_tRcvMediaChannel.SetPort(wRcvPort+9);
				
				cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );			
				SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
				
				//��ӡ��Ϣ
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Open Data LogicChannel Request send to Mt%d, type: %s\n", 
					      tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );
			}

			SendMtListToMcs( LOCAL_MCUIDX );

			//  xsl [10/24/2005] �������ˡ���ϯ״̬֪ͨ,����ͬ���¼�mcu״̬
			TSimConfInfo tSimConfInfo;
			tSimConfInfo.m_tSpeaker  = GetLocalSpeaker();
			tSimConfInfo.m_tChairMan = m_tConf.GetChairman();            
			cServMsg.SetMsgBody( ( u8 * )&tSimConfInfo, sizeof( tSimConfInfo ) );
			SendMsgToMt(tMt.GetMtId(), MCU_MT_SIMPLECONF_NOTIF, cServMsg );
		}
		else
		{
			m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_MT); 
		}
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMtMcuMtJoinNotif
    ����        ���ն˳ɹ����֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/19    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcMtMcuMtJoinNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	u8 byMcuMaster = *(cServMsg.GetMsgBody());

	switch( CurState() )
	{
	case STATE_ONGOING:
		{
			
			// xsl [10/20/2006] ���ﵽMcu LicenseʧЧ���ڣ� �������ն����
			if (g_cMcuVcApp.IsMcuExpiredDate())
			{
				cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
				cServMsg.SetErrorCode(ERR_MCU_OVERMAXCONNMT);
				
				SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg );
				
				cServMsg.SetMsgBody();
				SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
				
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuMtJoinNotif] MCU License EXPIRED.\n");  
				
				return;
			}
			
			// [7/9/2010 xliang] mcu license ��Ȩ������Լ�����������鲻�ڴ˴������ˣ��ȵ���ȡ�ն�������ͳһУ��
			
			// xsl [11/16/2006] ������֧��gk�Ʒѣ�����֤ʧ�ܣ��������ն����
			if (m_tConf.IsSupportGkCharge() && !m_tConf.m_tStatus.IsGkCharge())
			{
				SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuMtJoinNotif] conf %s support gk charge, but gk charge failed.\n", m_tConf.GetConfName());  
				return;
			}
			
			m_ptMtTable->SetMtIsMaster(cServMsg.GetSrcMtId(), byMcuMaster == 1?FALSE:TRUE);
			
			if( m_ptMtTable->GetCallMode(cServMsg.GetSrcMtId()) == CONF_CALLMODE_TIMER )
			{
				m_ptMtTable->SetCallLeftTimes(cServMsg.GetSrcMtId(), m_tConf.m_tStatus.GetCallTimes());
			}
			
			// [9/13/2011 liuxu] Ϊ�¼�����ն����confidx
			TMtStatus tStatus;
			tStatus.Clear();
			if ( m_ptMtTable && m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tStatus) )
			{
				tStatus.SetConfIdx(tMt.GetConfIdx());
				m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tStatus);
			}
			
			//��Ҫ����Ļ��飬�Է��������ն���ֱ�ӽ��� 
			if( m_tConf.GetConfAttrb().GetOpenMode() == CONF_OPENMODE_NEEDPWD && 
				m_ptMtTable &&
				m_ptMtTable->IsNotInvited( tMt.GetMtId() ) )
			{
				// zbq [05/31/2007] �ȷ���MP�����նˣ���֤������߼�ͨ��������
				AssignMpForMt(tMt);
				SendMsgToMt( tMt.GetMtId(), MCU_MT_ENTERPASSWORD_REQ, cServMsg );							
			}
			else
			{
				//�������
				AddJoinedMt( tMt );
				
				if( tMt.GetMtType() == MT_TYPE_MMCU && g_cMcuVcApp.IsMMcuSpeaker() )
				{
					m_bMMcuSpeakerValid = TRUE;
				}
			}

			// ��¼������֧��
			if (tMt.GetMtType() == MT_TYPE_VRSREC)
			{
				TMtAlias tVrsDialAlias;
				m_ptMtTable->GetDialAlias(tMt.GetMtId(), &tVrsDialAlias);
				if (m_tPlayEqpAttrib.GetReqType() == TPlayEqpAttrib::TYPE_FILELIST)
				{
					//�б�����
					ProcGetVrsListRecord(tVrsDialAlias, tMt, m_tPlayEqpAttrib.GetReqType(), 
						m_tPlayEqpAttrib.GetGroupId(), m_tPlayEqpAttrib.GetPageNo(), m_tPlayEqpAttrib.GetListNum());
				}
				else if (m_tPlayEqpAttrib.GetReqType() == TPlayEqpAttrib::TYPE_STARTPLAY)
				{
					//������������
					StartVrsPlay(tVrsDialAlias, m_tPlayEqpAttrib);
				}
			}
			
			// [7/9/2010 xliang] ��֪ͨ��mcu license��ȨУ����˺��ٷ�
			//֪ͨ����MCU new mt
			//NotifyMcuNewMt(tMt);     
		}
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMtMcuEnterPwdRsp
    ����        ���ն�Ӧ��ѯ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/19    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcMtMcuEnterPwdRsp( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	s8 *pszPwd  = (s8 *)cServMsg.GetMsgBody();
	s32 nPwdLen = cServMsg.GetMsgBodyLen();
	
	if( tMt.IsNull() )
	{
		return;
	}

	switch( CurState() )
	{
	case STATE_ONGOING:
		if (m_tConfAllMtInfo.MtJoinedConf( tMt.GetMcuId(), tMt.GetMtId() ))
		{
			break;
		}
		if( MT_MCU_ENTERPASSWORD_NACK == pcMsg->event)
		{
			//ʧ��Ӧ��ʵ���ϲ����������������
			if( MT_TYPE_SMCU == tMt.GetMtType() || MT_TYPE_MMCU == tMt.GetMtType() )
			{
				RemoveMt( tMt, TRUE );
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mcu%d password wrong, drop it!\n", tMt.GetMtId() );
			}
			else
			{
				SendMsgToMt( tMt.GetMtId(), MCU_MT_ENTERPASSWORD_REQ, cServMsg );
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mt%d password wrong, request once more!\n", tMt.GetMtId() );
			}
		}
		else
		{
			DealMtMcuEnterPwdRsp(tMt, pszPwd, nPwdLen);
		}
		break;
		
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*=============================================================================
    �� �� ���� DealMtMcuEnterPwdRsp
    ��    �ܣ� �����ն�Ӧ��ѯ������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMt tMt
               s8 *pszPwd
               s32 nPwdLen
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/11/3   3.6			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::DealMtMcuEnterPwdRsp(TMt tMt, s8 *pszPwd, s32 nPwdLen)
{
	if (NULL == pszPwd || 0 == nPwdLen)
	{
		return FALSE;
	}
	
	BOOL32 bRet = FALSE;	
	CServMsg cServMsg;
	TMultiCapSupport tCapSupport;
    TCapSupportEx tCapSupportEx;
	TLogicalChannel tLogicalChannel;
	s8 achConfPwd [MAXLEN_PWD+1] = {0};
	strncpy( achConfPwd, pszPwd, sizeof( achConfPwd ) );
	achConfPwd[sizeof( achConfPwd ) - 1] = '\0';
	
	if( nPwdLen != (s32)strlen(m_tConf.GetConfPwd()) ||
		0 != strncmp( achConfPwd, m_tConf.GetConfPwd(), MAXLEN_PWD) )
	{
		if ( MT_TYPE_SMCU == tMt.GetMtType() || MT_TYPE_MMCU ==  tMt.GetMtType() )
		{
			RemoveMt( tMt, TRUE );
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mcu%d password wrong, drop it!\n", tMt.GetMtId() );
		}
		else
		{
			SendMsgToMt( tMt.GetMtId(), MCU_MT_ENTERPASSWORD_REQ, cServMsg );
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mt%d password wrong, request once more!\n", tMt.GetMtId() );
		}
	}
	else
	{
		//�������
		AddJoinedMt( tMt );

		if( tMt.GetMtType() == MT_TYPE_MMCU && g_cMcuVcApp.IsMMcuSpeaker() )
		{
			m_bMMcuSpeakerValid = TRUE;
		}

		//�����ն�������
		m_ptMtTable->GetMtMultiCapSupport( tMt.GetMtId(), &tCapSupport );
        m_ptMtTable->GetMtCapSupportEx( tMt.GetMtId(), &tCapSupportEx);

		ProcMtCapSet( tMt, tCapSupport);

		//������Ƶ��������
		if( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE ) )
		{			
			//��̬�����ı���
			if( g_cMcuVcApp.GetVMPMode(m_tVmpEqp) != CONF_VMPMODE_NONE )
			{
				ChangeVmpStyle( tMt, TRUE );
			}

			//���Ƿ����˼���
			if( m_tConf.HasSpeaker() && GetLocalSpeaker() == tMt )
			{
                // xsl [8/22/2006]���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
                if (m_tConf.GetConfAttrb().IsSatDCastMode() /*&& IsOverSatCastChnnlNum(tMt.GetMtId())*/
					&& IsMultiCastMt(tMt.GetMtId())
					&& //IsSatMtOverConfDCastNum(tMt))
					!IsSatMtCanContinue(tMt.GetMtId(),emSpeaker))
                {
                    ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[DealMtMcuEnterPwdRsp] over max upload mt num. cancel speaker!\n");
                    ChangeSpeaker(NULL);
                }
                else
                {
                    ChangeSpeaker( &tMt );		
                }				
			}

			if( !( GetLocalSpeaker() == tMt ) )
			{
				NotifyMtSend( tMt.GetMtId(), MODE_VIDEO, FALSE );
			}
		}
	
		//������Ƶ��������	
		if( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE ) )
		{
			if( m_tConf.m_tStatus.IsMixing() )
			{				
                // xsl [8/4/2006] ���ƻ�������nģʽ��������������������ͨ��
                if (m_tConf.m_tStatus.IsSpecMixing())
                {
                    SwitchMixMember(&tMt, TRUE);
                }
                else
                {
                    if (GetMixMtNumInGrp() < GetMaxMixNum(m_tMixEqp.GetEqpId()))
                    {
                        AddMixMember( &tMt, DEFAULT_MIXER_VOLUME, FALSE );
						StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
											 (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt)), 
											 MODE_AUDIO, SWITCH_MODE_SELECT);
                    }                    
                }                				             			
			}
			else
			{
				NotifyMtSend( tMt.GetMtId(), MODE_AUDIO, FALSE );
			}
		}

		//���ն��Ѿ�ͨ��������֤, ���ͼ����б���	
		if( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_MMCUDATA, &tLogicalChannel, FALSE ) )
		{
			m_tCascadeMMCU = tMt;
			m_tConfAllMtInfo.m_tMMCU = tMt;
            m_tConfInStatus.SetNtfMtStatus2MMcu(TRUE);
			
			//���˼���mcu��Ϣ����������б���
			u16 wMcuIdx = INVALID_MCUIDX;
			u8 abyMcuId[MAX_CASCADEDEPTH-1];
			memset( &abyMcuId[0],0,sizeof(abyMcuId) );
			abyMcuId[0] = tMt.GetMtId();
			m_tConfAllMcuInfo.AddMcu( &abyMcuId[0],2,&wMcuIdx );
			m_ptConfOtherMcTable->AddMcInfo( wMcuIdx );	
			m_tConfAllMtInfo.AddMcuInfo( wMcuIdx, tMt.GetConfIdx() );
			
			//֪ͨ���
			cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
			if(m_tConf.HasConfExInfo())
			{
				u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
				u16 wPackDataLen = 0;
				PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
				cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
			}
			SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
			
			//������ػ��������ն���Ϣ
			//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
			//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
			SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
			
			//auto speak
			if(m_tConf.GetSpeaker().IsNull() && g_cMcuVcApp.IsMMcuSpeaker())
			{
				ChangeSpeaker(&m_tCascadeMMCU);
			}
		}

		bRet = TRUE;
	}

	return bRet;
}

/*====================================================================
    ������      ��ProcMtMcuEnterPwdReq
    ����        ���ն�ѯ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/19    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcMtMcuEnterPwdReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );


	switch( CurState() )
	{
	case STATE_ONGOING:
		if(strlen(m_tConf.GetConfPwd()) > 0)
		{
			cServMsg.SetMsgBody((u8 *)(m_tConf.GetConfPwd()), strlen(m_tConf.GetConfPwd()));
			SendMsgToMt( tMt.GetMtId(), MT_MCU_ENTERPASSWORD_ACK,  cServMsg);
		}
		else
		{
			SendMsgToMt( tMt.GetMtId(), MT_MCU_ENTERPASSWORD_NACK,  cServMsg);
		}
		break;
	default:
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*=============================================================================
    �� �� ���� ProcMtMcuFlowControlCmd
    ��    �ܣ� �ն�Ҫ��ı�������� - ������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
               BOOL32 bNtfPhyBandWidth: �Ƿ���Ӧ�ն��������֪ͨ
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/21   3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMtMcuFlowControlCmd( const CMessage * pcMsg, BOOL32 bNtfPhyBandWidth )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt;
	TLogicalChannel	tLogicChnnl;
	u16 wBitRate, wMinBitRate = 0;

	BOOL bStartDoubleStream = FALSE;
    BOOL32 bMtLowedAlready = FALSE;

	BOOL bIsPrimaryUp = TRUE;
	BOOL bIsDoubleUp = TRUE;

	BOOL32 bIsNeedPrimaryBrdAdapt = FALSE;
	BOOL32 bIsNeedDoubleBrdAdapt = FALSE;
	TMt tBrdSrc = GetVidBrdSrc();

	BOOL32 bIsAdjustMStreamWithDiscount = FALSE; //��ʶ���������������Ƿ��Ѿ�����.TRUE��ʾ�Ѿ����ۣ�FALSE��ʾδ����.
	u16 wRealMtReqBR = 0; //bIsAdjustMStreamWithDiscountΪTRUE��wRealMtReqBR������ۺ���������ʣ�ΪFALSE������δ���۵���������[9/27/2012 chendaiwei]
	u16	wCurMtReqBR = 0;

	BOOL32 bIsE1Mcu = FALSE;

	switch( CurState() )
	{
	case STATE_ONGOING:
		{
		tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
		if((m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MMCU|| 
			m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU)
			&& m_ptMtTable->GetMtTransE1(tMt.GetMtId()))
		{
			bIsE1Mcu = TRUE;
		}

		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );
		wBitRate = tLogicChnnl.GetFlowControl();

		//////////////////////////////////////////////
		//FlowControlToZero
		if( !m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()) || 0 == wBitRate )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] 0 == wBitRate MT.%d\n",
                   tMt.GetMtId());
			return;
		}

		if( MODE_AUDIO == tLogicChnnl.GetMediaType() )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] flowcontrol audio channel.MT.%d\n",
                   tMt.GetMtId());
			return;
		}

		

		//��������������ն˵�FlowControl
		if( ( m_ptMtTable->GetManuId( tMt.GetMtId() ) != MT_MANU_KDC && 
				m_ptMtTable->GetManuId( tMt.GetMtId() ) != MT_MANU_KDCMCU ) /*||
				wBitRate > m_tConf.GetBitRate() */)
		{
			// xliang [11/14/2008] ��������PolycomMCU��flowctrl����Ӧ��˫����ͣ����
			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
				( MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
				MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId())))
			{
				ProcPolycomMcuFlowctrlCmd(tMt.GetMtId(), tLogicChnnl);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMtMcuFlowControlCmd] PolycomMcu.%d flowctrl received\n", tMt.GetMtId() );
			}
			return;
		}

		//����E1�����¼����ϼ�����ʧ��[7/6/2012 chendaiwei]
// 		// xsl [8/7/2006] �¼�mcu���͵ĵ�һ·flowcontrol������
// 		if( MODE_SECVIDEO != tLogicChnnl.GetMediaType() &&
// 			m_ptMtTable->GetManuId( tMt.GetMtId() ) == MT_MANU_KDCMCU )
// 		{
// 			if (m_tCascadeMMCU.IsNull() || m_tCascadeMMCU.GetMtId() != tMt.GetMtId())
//             {
//                 ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] ignore smcu MtReqBitrate :%d from mcu<%d>\n",
//                    wBitRate, tMt.GetMtId());
//                 return;
//             }
// 		}

		if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
		{
			if (wBitRate > m_tConf.GetBitRate())
			{
				wBitRate = m_tConf.GetBitRate();
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "wBitRate is larger than conf bitrate, modify it equal to conf's bitrate! \n");
			}

			//���ۺ�����������п���С��˫�ٵ��١��˴��ж�����[10/22/2012 chendaiwei]
// 			if ( 0 != m_tConf.GetSecBitRate() && wBitRate < m_tConf.GetSecBitRate() )
// 			{
// 				wBitRate = m_tConf.GetSecBitRate();
// 				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "wBitRate is smaller than conf sec bitrate, modify it equal to conf's sec bitrate! \n");
// 			}

			//����ǰ��ͨ����MCU������������ͬ������[9/27/2012 chendaiwei]
			if(wBitRate == m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE))
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] wBitRate(%d) is not change.MT.%d\n",
                   wBitRate,tMt.GetMtId() );
				return;	
			}
			else if( !m_ptMtTable->GetMtTransE1(tMt.GetMtId())
				     && wBitRate < m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE))
			{
				//����MCU��Դ���п������¼�MCU˫�����ʰٷֱȲ�ͬ������wBitRate��m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE)
				//��һ��[9/27/2012 chendaiwei]
				bIsAdjustMStreamWithDiscount = TRUE;
			}
			else if (!m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
					wBitRate > m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE)
				   && wBitRate < (m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE)+m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(),FALSE)))
			{
				bIsAdjustMStreamWithDiscount = TRUE;
			}

			if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) )
			{
				u16 wRcvBandWidth = m_ptMtTable->GetRcvBandWidth(tMt.GetMtId());            
				if (wRcvBandWidth < wBitRate)
				{
					m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
				}
				else
				{
					if ( abs(wBitRate-wRcvBandWidth)*100 / wRcvBandWidth > 20 )
					{
						m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), TRUE);
					}
					else
					{
						m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
					}
				}
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuFlowControlCmd] mt.%u bLowed: %d (wRcvBandWidth:%d,wBitRate:%d, conf bitrate:%d)\n",
					tMt.GetMtId(), m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()),
					wRcvBandWidth, wBitRate,m_tConf.GetBitRate());			
			}

			if( !m_tDoubleStreamSrc.IsNull() )
			{
				if( !bIsAdjustMStreamWithDiscount && GetDoubleStreamVideoBitrate( wBitRate,TRUE ) == m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),TRUE ) 
					&& !bIsE1Mcu)
				{
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] Fst wBitRate(%d) is not change.MT.%d\n",
							GetDoubleStreamVideoBitrate( wBitRate,TRUE ),tMt.GetMtId() );
					return;
				}
				if( !bIsAdjustMStreamWithDiscount && GetDoubleStreamVideoBitrate( wBitRate,FALSE ) == m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),FALSE )
					&& !bIsE1Mcu)
				{
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] Sec wBitRate(%d) is not change.MT.%d\n",
							GetDoubleStreamVideoBitrate( wBitRate,FALSE ),tMt.GetMtId() );
					return;
				}
				if( (!bIsAdjustMStreamWithDiscount && GetDoubleStreamVideoBitrate( wBitRate,TRUE ) < m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),TRUE )) 
					|| (bIsAdjustMStreamWithDiscount && wBitRate < m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),TRUE ))
					)
				{
					bIsPrimaryUp = FALSE;
				}
				if( (!bIsAdjustMStreamWithDiscount && GetDoubleStreamVideoBitrate( wBitRate,FALSE ) < m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),FALSE))
					|| (bIsAdjustMStreamWithDiscount && wBitRate < m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),FALSE ))
					)
				{
					bIsDoubleUp = FALSE;
				}
				
			}
			else
			{
				if( wBitRate == m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),TRUE ) )
				{
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] wBitRate(%d) is not change.MT.%d\n",
							wBitRate,tMt.GetMtId() );
					return;
				}
				if(  wBitRate < m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),TRUE ) )
				{
					bIsPrimaryUp = FALSE;
				}
			}

			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] bIsPrimaryUp.%d bIsDoubleUp.%d .MT.%d BitRate.%d\n",
							bIsPrimaryUp,bIsDoubleUp,tMt.GetMtId(),wBitRate );
			
			wCurMtReqBR = m_ptMtTable->GetMtReqBitrate(tMt.GetMtId());
			
			if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) && m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MT)
			{
				if( !m_tDoubleStreamSrc.IsNull() )
				{			
					//����˫�� �����նˣ���¼����
					if ( bNtfPhyBandWidth )
					{
						wRealMtReqBR = wBitRate;
					}
					//˫���� E1�ն˽��٣����¼���, �������һ·�͵ڶ�·�Ĵ�����֪ͨ˫��Դ���·��ʹ���
					else
					{
						wRealMtReqBR = wBitRate * 2;
						if( wRealMtReqBR > m_tConf.GetBitRate() )
						{
							wRealMtReqBR = m_tConf.GetBitRate();
						}
					}
				}
				else
				{
					wRealMtReqBR = wBitRate;
				}	
				if( m_ptMtTable->GetMtBRBeLowed( tMt.GetMtId() ) )
				{
					m_ptMtTable->SetLowedRcvBandWidth(tMt.GetMtId(), wRealMtReqBR);
				}
			}
			else
			{
				wRealMtReqBR = wBitRate;			
			}

			if( !tBrdSrc.IsNull() && tBrdSrc.GetType() != TYPE_MCUPERI)
			{				
				bIsNeedPrimaryBrdAdapt = IsNeedAdapt( tBrdSrc,tMt,MODE_VIDEO );				
			}
			if( !m_tDoubleStreamSrc.IsNull() )
			{
				bIsNeedDoubleBrdAdapt = IsNeedAdapt( m_tDoubleStreamSrc,tMt,MODE_SECVIDEO );
			}

			//E1MCU����ʼ����δ���۴���[10/29/2012 chendaiwei]
			if( !m_tDoubleStreamSrc.IsNull() 
				&& ( bIsE1Mcu || !bIsAdjustMStreamWithDiscount))
			{
				m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wRealMtReqBR, TRUE) , LOGCHL_VIDEO );
				m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wRealMtReqBR, FALSE) , LOGCHL_SECVIDEO );		
			}
			else
			{			
				m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wRealMtReqBR, LOGCHL_VIDEO );
			}
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] CurMtReqBR.%d while RealReqBR.%d, adjust to Fst.%d, Sec.%d bIsNeedPrimaryBrdAdapt.%d bIsNeedDoubleBrdAdapt.%d bIsFlowCtrlMainStream.%d\n",
				wCurMtReqBR, wRealMtReqBR, 
				m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), LOGCHL_VIDEO),
				m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), LOGCHL_SECVIDEO),
				bIsNeedPrimaryBrdAdapt,
				bIsNeedDoubleBrdAdapt,
					bIsAdjustMStreamWithDiscount);
			

		//}


		//if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
		//{
			//��ʶ�����ն��Ƿ���ͨ����ش��ϴ���ȫ�ַ�����[11/27/2012 chendaiwei]
			BOOL32 bHasGlobalSpeakerMultiSpy = FALSE;
			TMt tLocalGlobalSpeaker;
			u8 byGlobalSpeakerId = 0;
			CSendSpy *ptSndSpy = NULL;
			TSimCapSet tDstCap;
			if (!m_tCascadeMMCU.IsNull() && tMt == m_tConf.GetSpeaker() && m_tCascadeMMCU == tMt )
			{
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
				
				if( ptConfMcInfo != NULL						
					&& ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.IsMcuIdLocal()
					)
				{			
					byGlobalSpeakerId = ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId();
					tLocalGlobalSpeaker = m_ptMtTable->GetMt(byGlobalSpeakerId);
					tLocalGlobalSpeaker.SetMcuId(LOCAL_MCUID);
					tLocalGlobalSpeaker.SetMtId(byGlobalSpeakerId);
					
					u16 wMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
					if( !IsValidMcuId( wMcuIdx ) )
					{
						ConfPrint( LOG_LVL_ERROR, MID_MCU_MT2, "[ProMtMcuFlowCtrl] wMcuIdx.%d is invalid\n",wMcuIdx );
						return;
					}
					
					BOOL32 bIsMMcuSupMultSpy = IsLocalAndSMcuSupMultSpy( wMcuIdx );
					u8 bySpyNum = 0;
					u32 dwSpyBandWidth = 0;
					u8 bySpyIdx = 0;
					if( bIsMMcuSupMultSpy )
					{
						for (bySpyIdx = 0; bySpyIdx < MAXNUM_CONF_SPY; bySpyIdx++)
						{
							tDstCap.Clear();
							ptSndSpy = m_cLocalSpyMana.GetSendSpy(bySpyIdx);
							if (NULL == ptSndSpy)
							{
								continue;
							}
							if( ptSndSpy->GetSpyMt().IsNull() )
							{
								continue;
							}
							if( !m_tConfAllMtInfo.MtJoinedConf( GetLocalMtFromOtherMcuMt( ptSndSpy->GetSpyMt() ).GetMtId() ) )
							{
								continue;
							}
							
							TMt tLocalSpyMt = GetLocalMtFromOtherMcuMt( ptSndSpy->GetSpyMt() );
							if(tLocalSpyMt.GetMtId()!=byGlobalSpeakerId)
							{
								continue;
							}
							else
							{
								bHasGlobalSpeakerMultiSpy = TRUE;
								tDstCap = ptSndSpy->GetSimCapset();
								ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2,"[ProcMtMcuFlowCrlCmd] Mt(%d) is GlobalSpeaker\n",byGlobalSpeakerId );
								break;
							}
						}
					}	
				}
			}

			TMtStatus tStatus;
			if( !m_ptMtTable->GetMtStatus(tMt.GetMtId(),&tStatus) )
			{
				return;
			}
			
			TMt tSelMt = tStatus.GetSelectMt( MODE_VIDEO );
			if( tSelMt.IsNull() )
			{
				TMt tVidSrc = tStatus.GetVideoMt();
				if( tMt.GetMtId() == m_tCascadeMMCU.GetMtId() && !tVidSrc.IsNull())
				{
					if( !(tVidSrc == tMt))
					{
						if( !IsNeedSelAdpt( tVidSrc,tMt,MODE_VIDEO ) )
						{
							AdjustMtVideoSrcBR( tMt.GetMtId(),wRealMtReqBR,MODE_VIDEO,TRUE );
						}
						else
						{
							if ( TRUE == bIsPrimaryUp )
							{
								RefreshSelBasParam( tVidSrc,tMt,MODE_VIDEO,TRUE,TRUE );
							}
							else
							{
								RefreshSelBasParam( tVidSrc,tMt,MODE_VIDEO );
							}
						}
					}	
				}
				//�¼����ϼ�FlowCtrl �¼���ʵ�㲥Դ�Ǳ����ն�[11/27/2012 chendaiwei]
				else if( GetLocalMtFromOtherMcuMt(tBrdSrc)== tMt 
					&& tMt.GetMtId() == m_tCascadeMMCU.GetMtId()
					&& bHasGlobalSpeakerMultiSpy)
				{
					if( !IsNeedSpyAdpt( tLocalGlobalSpeaker,tDstCap,MODE_VIDEO ))
					{
						TLogicalChannel tLogic;
						CServMsg cMsg;
						if(m_ptMtTable->GetMtLogicChnnl(tLocalGlobalSpeaker.GetMtId(),LOGCHL_VIDEO,&tLogic,FALSE))
						{
							u16 wAcualBR = 0;
							if( !m_tDoubleStreamSrc.IsNull() && !bIsAdjustMStreamWithDiscount/* && m_tDoubleStreamSrc == tBrdSrc*/ )
							{
								wAcualBR = GetDoubleStreamVideoBitrate(wRealMtReqBR,TRUE);
							}
							else
							{
								wAcualBR = wRealMtReqBR;
							}

							tLogic.SetFlowControl(wAcualBR);
							cMsg.SetMsgBody( (u8*)&tLogic, sizeof( tLogic ) );			

							SendMsgToMt( tLocalGlobalSpeaker.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cMsg );
						}
					}
					else
					{
						tDstCap.SetVideoMaxBitRate( m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(), TRUE ));
						if( ptSndSpy != NULL)
						{
							m_cLocalSpyMana.SaveSpySimCap(ptSndSpy->GetSpyMt(), tDstCap);
						}

						if ( TRUE == bIsPrimaryUp )
						{
							RefreshSpyBasParam( tLocalGlobalSpeaker,tDstCap,MODE_VIDEO,TRUE,TRUE );
						}
						else
						{
							RefreshSpyBasParam( tLocalGlobalSpeaker,tDstCap,MODE_VIDEO);
						}

						ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowCtrl] SpyMt(%d.%d) refresh bas to wModifiedBandWidth.%d!\n", 
							tLocalGlobalSpeaker.GetMcuId(),tLocalGlobalSpeaker.GetMtId(),m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(), TRUE ) );
	
					}
				}
				else if( !tBrdSrc.IsNull() && 
					      ( m_tCascadeMMCU.IsNull() || tMt.GetMtId() != m_tCascadeMMCU.GetMtId()) &&
						  //CMDԴ�͹㲥Դͬʱ���¼�MCU[11/27/2012 chendaiwei]
					      ( !(GetLocalMtFromOtherMcuMt(tBrdSrc)==tMt)
						  || (m_ptMtTable->GetMtType(tMt.GetMtId())== MT_TYPE_SMCU && GetLocalMtFromOtherMcuMt(tBrdSrc)==tMt))
					   )
				{
					if( tBrdSrc.GetType() == TYPE_MCUPERI )
					{
						 if( tBrdSrc.GetEqpType() == EQP_TYPE_RECORDER )
						 {
							ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] brd is recorder.so not handle. mt.%d\n",tMt.GetMtId() );
						 }
						 else if( tBrdSrc.GetEqpType() == EQP_TYPE_VMP )
						 {
							ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] brd is vmp.adjust vmp birrate.mt.%d\n",tMt.GetMtId() );
							AdjustVmpBrdBitRate( &tMt );
						 }
					}
					else
					{
						if( 0 == m_tConf.GetSecBitRate() )
						{
							if( !bIsNeedPrimaryBrdAdapt )
							{
								if( tBrdSrc.IsLocal() 
									|| (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU && GetLocalMtFromOtherMcuMt(tBrdSrc)==tMt))
								{
									if( !m_tDoubleStreamSrc.IsNull() && !bIsAdjustMStreamWithDiscount/* && m_tDoubleStreamSrc == tBrdSrc*/ )
									{
										AdjustMtVideoSrcBR( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wRealMtReqBR,TRUE), MODE_VIDEO,TRUE );
									}
									else
									{
										AdjustMtVideoSrcBR( tMt.GetMtId(),wRealMtReqBR,MODE_VIDEO,TRUE );
									}
								}
								else
								{
									//�¼�E1���м��
								}
							}
							else
							{
								if ( TRUE == bIsPrimaryUp  )
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_VIDEO,TRUE,TRUE );
								}
								else
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_VIDEO);
								}
							
							}
						}
						else
						{
							u16 wSpyPort = SPY_CHANNL_NULL;
							CRecvSpy tSpyResource;	
							if( !tBrdSrc.IsLocal() &&
								m_cSMcuSpyMana.GetRecvSpy( tBrdSrc, tSpyResource )
								)
							{
								wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
							}				
							if( !bIsNeedPrimaryBrdAdapt )
							{												
								if( IsNeedAdapt( tBrdSrc,tMt,MODE_VIDEO ) )
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_VIDEO );
									StartSwitchFromBrd( tBrdSrc,0,1,&tMt,wSpyPort );									
								}
								else
								{
									ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] before after all not adapt,it's impossible.mt(%d)\n",tMt.GetMtId() );
								}
							}
							else
							{
								if( !IsNeedAdapt( tBrdSrc,tMt,MODE_VIDEO ) )
								{
									ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] before nedd adapt,after not adapt,rehandle switch.mt(%d)\n",tMt.GetMtId() );
									StartSwitchFromBrd( tBrdSrc,0,1,&tMt,wSpyPort );
									
									u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),TRUE );
									m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(),wMtBitRate - 10,LOGCHL_VIDEO);
									//�ָ��ղŽ���
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_VIDEO,TRUE,TRUE );
									m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(),wMtBitRate,LOGCHL_VIDEO);
								}
								else
								{
									if ( TRUE == bIsPrimaryUp  )
									{
										RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_VIDEO,TRUE,TRUE );
									}
									else
									{
										RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_VIDEO);
									}
								}
							}
						}
					}
				}
				//E1 SMCU�������㲥Դ���䷢���߼�ͨ�����������[11/6/2012 chendaiwei]
				else if(GetLocalVidBrdSrc()== tMt && bIsE1Mcu)
				{
					TLogicalChannel tMainLogicChnnl;
					if (m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tMainLogicChnnl, FALSE))
					{
						u16 wMainBR = m_tDoubleStreamSrc.IsNull()?wRealMtReqBR:GetDoubleStreamVideoBitrate(wRealMtReqBR,TRUE);
						tMainLogicChnnl.SetFlowControl(wMainBR);
						m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tMainLogicChnnl, FALSE);
						
						LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] SMCU<%d> is BRD SRC.Adjust RL BR to%d\n",tMt.GetMtId(),wMainBR);
					}
				}
			}//!tSelMt.IsNull()
			else
			{
				if( tSelMt.GetType() == TYPE_MCUPERI && tSelMt.GetEqpType() == EQP_TYPE_VMP )
				{
					ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] sel vmp.so ajdust vmp mt(%d)\n",tMt.GetMtId() );
					AdjustVmpSelBitRate( tSelMt );
				}
				else
				{
					//�Ƿ���Ҫ�������Ƿ�˫��ռ��[11/27/2012 chendaiwei]
					if( !(tSelMt == tMt))
					{
						if( !IsNeedSelAdpt( tSelMt,tMt,MODE_VIDEO ) )
						{
							AdjustMtVideoSrcBR( tMt.GetMtId(),wRealMtReqBR,MODE_VIDEO,TRUE );
						}
						else
						{
							if ( TRUE == bIsPrimaryUp )
							{
								RefreshSelBasParam( tSelMt,tMt,MODE_VIDEO,TRUE,TRUE );
							}
							else
							{
								RefreshSelBasParam( tSelMt,tMt,MODE_VIDEO );
							}
						}
					}					
				}
			}
			if ( !m_tDoubleStreamSrc.IsNull()  && !(m_tDoubleStreamSrc == m_tPlayEqp) )
            {
				if(!(tMt == m_tDoubleStreamSrc) )
				{
					TMt tMtNull;
					tMtNull.SetNull();
					//ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] E1 Lowed Mt restore, adjust DS Src.\n");
					if( 0 == m_tConf.GetSecBitRate() )
					{
						if( !bIsNeedDoubleBrdAdapt )
						{
							if( !m_tDoubleStreamSrc.IsNull() && m_tDoubleStreamSrc == tBrdSrc )
							{
								//AdjustMtVideoSrcBR( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wRealMtReqBR,FALSE), MODE_SECVIDEO );
								RestoreVidSrcBitrate(tMtNull, MODE_SECVIDEO);
							}
							else
							{
								//AdjustMtVideoSrcBR( tMt.GetMtId(),wRealMtReqBR,MODE_SECVIDEO );
								RestoreVidSrcBitrate(tMtNull, MODE_SECVIDEO);
							}								
						}
						else
						{
							if ( TRUE == bIsDoubleUp  )
							{
								RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
							}
							else
							{
								RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO);
							}
						}
					}
					else
					{
						if( !bIsNeedDoubleBrdAdapt )
						{
							if( IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO)/*m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE)*/)
							{							
								if ( TRUE == bIsDoubleUp  )
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								}
								else
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO);
								}
								StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_SECVIDEO);
							}
							else
							{
								ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] before after all not Sec adapt,it's impossible.mt(%d)\n",tMt.GetMtId() );
							}
						}					
						else
						{
							if( !IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO) )
							{			
								ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] before need Sec adapt,after not Sec adapt,rehandle switch.mt(%d)\n",tMt.GetMtId() );
								TSwitchGrp tSwitchGrp;
								tSwitchGrp.SetSrcChnl(0);
								tSwitchGrp.SetDstMtNum(1);
								tSwitchGrp.SetDstMt(&tMt);
								g_cMpManager.StartSwitchToAll(m_tDoubleStreamSrc, 1, &tSwitchGrp, MODE_SECVIDEO);

								u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),FALSE );
								m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(),wMtBitRate - 10,LOGCHL_SECVIDEO );
								//�ָ��ղŽ���
								RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(),wMtBitRate,LOGCHL_SECVIDEO );
							}
							else
							{
								if ( TRUE == bIsDoubleUp  )
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								}
								else
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO);
								}
							}
						}
					}
				}
				//E1 SMCU��˫��Դ���䷢���߼�ͨ�����������[11/6/2012 chendaiwei]
				else if(tMt == m_tDoubleStreamSrc && bIsE1Mcu)
				{
					TLogicalChannel tSecLogicChnnl;
					if (m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tSecLogicChnnl, FALSE))
					{
						tSecLogicChnnl.SetFlowControl(GetDoubleStreamVideoBitrate(wRealMtReqBR,FALSE));
						m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tSecLogicChnnl, FALSE);

						LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] SMCU<%d> is DS SRC.Adjust RL BR to%d\n",tMt.GetMtId(),GetDoubleStreamVideoBitrate(wRealMtReqBR,FALSE));
					}
				}
				/*else
				{
					RestoreVidSrcBitrate(tMtNull, MODE_SECVIDEO);
				}*/				
			}
		}

		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
			if (tMt.GetMtId() == m_tCascadeMMCU.GetMtId())
			{
				OnNtfDsMtStatusToMMcu();
			}
			if( m_tDoubleStreamSrc.GetType() == TYPE_MCUPERI && m_tDoubleStreamSrc.GetEqpType() == EQP_TYPE_RECORDER )
			{
				ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2," [ProcMtMcuFlowControlCmd]m_tDoubleStreamSrc == EQP_TYPE_RECORDER\n" );
				return;
			}
			if( m_tDoubleStreamSrc == tMt )
			{
				m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_SECVIDEO );
				ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2," [ProcMtMcuFlowControlCmd]m_tDoubleStreamSrc == tMt.%d,so not adjust!\n",tMt.GetMtId() );
				return;
			}

			//����������ͬ����Ϣ
            if (wBitRate == m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), FALSE))
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv snd ds MtReqBitrate :%d from mt<%d>,no change,ignore it.\n",
                    wBitRate, tMt.GetMtId());

				return;
			}
			else if (!m_tDoubleStreamSrc.IsNull() && wBitRate > GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE))
            {
				bIsDoubleUp = TRUE;
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv snd ds MtReqBitrate :%d from mt<%d>, bIsDoubleUp.%d\n",
                    wBitRate, tMt.GetMtId(),bIsDoubleUp);
            }
			else if(!m_tDoubleStreamSrc.IsNull() && wBitRate < GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE))
			{
				bIsDoubleUp = FALSE;
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv snd ds MtReqBitrate :%d from mt<%d>, bIsDoubleUp.%d\n",
                    wBitRate, tMt.GetMtId(),bIsDoubleUp);
			}
			
			if( !m_tDoubleStreamSrc.IsNull() )
			{
				bIsNeedDoubleBrdAdapt = IsNeedAdapt( m_tDoubleStreamSrc,tMt,MODE_SECVIDEO );
			}
			
			m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_SECVIDEO );
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv snd doublestream .MtReqBitrate :%d from mt<%d>\n",
                    wBitRate, tMt.GetMtId());

            if ( !m_tDoubleStreamSrc.IsNull())
            {
				if(!(tMt == m_tDoubleStreamSrc) )
				{
					TMt tMtNull;
					tMtNull.SetNull();
					if( 0 == m_tConf.GetSecBitRate() )
					{
						if( !bIsNeedDoubleBrdAdapt )
						{
							AdjustMtVideoSrcBR(tMt.GetMtId(),wBitRate,MODE_SECVIDEO,TRUE);						
						}
						else
						{
							if ( TRUE == bIsDoubleUp  )
							{
								RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
							}
							else
							{
								RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO);
							}
						}
					}
					else
					{
						if( !bIsNeedDoubleBrdAdapt )
						{
							if( IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO)/*m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE)*/)
							{							
								if ( TRUE == bIsDoubleUp  )
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								}
								else
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO);
								}
								StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_SECVIDEO);
							}
							else
							{
								ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd][2]before after all not Sec adapt,it's impossible.mt(%d)\n",tMt.GetMtId() );
							}
						}					
						else
						{
							if( !IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO) )
							{			
								ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] before need Sec adapt,after not Sec adapt,rehandle switch.mt(%d)\n",tMt.GetMtId() );
								TSwitchGrp tSwitchGrp;
								tSwitchGrp.SetSrcChnl(0);
								tSwitchGrp.SetDstMtNum(1);
								tSwitchGrp.SetDstMt(&tMt);
								g_cMpManager.StartSwitchToAll(m_tDoubleStreamSrc, 1, &tSwitchGrp, MODE_SECVIDEO);
								
								u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),FALSE );
								m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(),wMtBitRate - 10,LOGCHL_SECVIDEO );
								//�ָ��ղŽ���
								RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(),wMtBitRate,LOGCHL_SECVIDEO );
							}
							else
							{
								if ( TRUE == bIsDoubleUp  )
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								}
								else
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO);
								}
							}
						}
					}
				}
            }

		}


		//return;

		
		/*tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );

		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );
		wBitRate = tLogicChnnl.GetFlowControl();

        //zbq[02/19/2008] �ն˱����Ƿ񱻽��٣���������Դ����µķ��ʹ�������.
        if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
             m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()))
        {
            bMtLowedAlready = TRUE;
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "bMtLowedAlready is %d\n",bMtLowedAlready);
        }

        //E1�ն������������־ ˢ��
        if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) )
        {
            u16 wRcvBandWidth = m_ptMtTable->GetRcvBandWidth(tMt.GetMtId());
            // xliang [2/16/2009] ��ǰ��wRcvBandwidth��Ŀǰ��(wBitRate)�Ƚ�
			if (wRcvBandWidth < wBitRate)
			{
				m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
			}
			else
			{
				if ( abs(wBitRate-wRcvBandWidth)*100 / wRcvBandWidth > 20 )
				{
					m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), TRUE);
				}
				else
				{
					m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
				}
			}
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuFlowControlCmd] mt.%u bLowed: %d (wRcvBandWidth:%d,wBitRate:%d, conf bitrate:%d)\n",
				tMt.GetMtId(), m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()),
				wRcvBandWidth, wBitRate,m_tConf.GetBitRate());
			
        }
		
		//��������������ն˵�FlowControl
		if( ( m_ptMtTable->GetManuId( tMt.GetMtId() ) != MT_MANU_KDC && 
		m_ptMtTable->GetManuId( tMt.GetMtId() ) != MT_MANU_KDCMCU ) )
		{
			// xliang [11/14/2008] ��������PolycomMCU��flowctrl����Ӧ��˫����ͣ����
			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
				( MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
				MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId())))
			{
				ProcPolycomMcuFlowctrlCmd(tMt.GetMtId(), tLogicChnnl);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMtMcuFlowControlCmd] PolycomMcu.%d flowctrl received\n", tMt.GetMtId() );
			}
			return;
		}
		
		if (wBitRate > m_tConf.GetBitRate())
		{
			wBitRate = m_tConf.GetBitRate();
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "wBitRate is larger than conf bitrate, modify it equal to conf's bitrate! \n");
		}

        // xsl [8/7/2006] �¼�mcu���͵ĵ�һ·flowcontrol������
		if( MODE_SECVIDEO != tLogicChnnl.GetMediaType() &&
			m_ptMtTable->GetManuId( tMt.GetMtId() ) == MT_MANU_KDCMCU )
		{
			if (m_tCascadeMMCU.IsNull() || m_tCascadeMMCU.GetMtId() != tMt.GetMtId())
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] ignore smcu MtReqBitrate :%d from mcu<%d>\n",
                   wBitRate, tMt.GetMtId());
                return;
            }
		}

		//FlowControlToZero,ͣ��
		if( 0 == wBitRate )
		{
			return;
		}

		if( MODE_AUDIO == tLogicChnnl.GetMediaType() )
		{
			return;
		}
				
		//����˫��ʱ�������ȡ����FlowControl�������Ѿ��۰���
		if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
		{
            //zbq[02/01/2008] �����ն���Ҫ���������
            BOOL32 bMtE1Lowed = FALSE;

            if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
                 m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()))
            {
                bMtE1Lowed = TRUE;
            }
            //����������ͬ����Ϣ
            if (wBitRate == m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE) ||
                 ( !m_tDoubleStreamSrc.IsNull() && 
                   GetDoubleStreamVideoBitrate(wBitRate) > GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())) &&
                   !bMtE1Lowed) )
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] same bitrate.%d and ReqBitRate.%d for mt.%d!\n", 
                    wBitRate, m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE), tMt.GetMtId());
                return;
            }
            if ( !bMtE1Lowed )
            {
                //��ǰ��˫��ԴҪ�����ʴ���
                if ( !m_tDoubleStreamSrc.IsNull() )
                {
                    m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wBitRate, TRUE) , LOGCHL_VIDEO );
                    m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wBitRate, FALSE) , LOGCHL_SECVIDEO );
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv fst MtReqBitrate :%d sec MtReqBitrate :%d from mt<%d>\n", 
                        GetDoubleStreamVideoBitrate(wBitRate, TRUE), GetDoubleStreamVideoBitrate(wBitRate, FALSE), tMt.GetMtId());
                }
                else
                {
                    m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_VIDEO );
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv fst MtReqBitrate :%d from mt<%d>\n", 
                        wBitRate, tMt.GetMtId());
                }
                //m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_VIDEO );
                //ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv fst MtReqBitrate :%d from mt<%d>\n", wBitRate, tMt.GetMtId());

                //zbq[02/19/2008] ������ѡ������Դ���ն˴�������,�������䷢�ʹ���
                if ( bMtLowedAlready )
                {
                    LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuFlowControlCmd]MtE1Lowed is %d, MtLowedAlready is %d!\n");
                    //˫��
                    if ( tMt == m_tDoubleStreamSrc )
                    {
                        TMt tMtNull;
                        tMtNull.SetNull();
                        RestoreVidSrcBitrate(tMtNull, MODE_SECVIDEO);
                        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] E1 Lowed Mt restore, adjust DS Src.\n");
                    }
                    //��Ƶѡ������
                    for( u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++)
                    {
                        if ( !m_tConfAllMtInfo.MtJoinedConf(byMtIdx) )
                        {
                            continue;
                        }

                        if ( m_ptMtTable->GetMtSrc(byMtIdx, &tMtSrc, MODE_VIDEO) &&
                             tMt == tMtSrc )
                        {
                            TMt tMtRcv = m_ptMtTable->GetMt(byMtIdx);
                            RestoreVidSrcBitrate(tMtRcv, MODE_VIDEO);
                            ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] E1 Lowed Mt restore, adjust FstVid Src.\n");
                            break;
                        }
                    }
                }
            }
            else
            {
                u16 wRealMtReqBR = 0;

                //zbq[02/01/2008] ���������ն˴�����Ϊ��ǿ��ȡ50%. ����Ҫ�Լ���һ��.
                if ( !m_tDoubleStreamSrc.IsNull() )
                {
                    u16 wCurMtReqBR = 0;

                    //����˫�� �����նˣ���¼����
                    if ( bNtfPhyBandWidth )
                    {
                        wCurMtReqBR = m_ptMtTable->GetMtReqBitrate(tMt.GetMtId());
                        wRealMtReqBR = wBitRate;
                    }
                    //˫���� E1�ն˽��٣����¼���, �������һ·�͵ڶ�·�Ĵ�����֪ͨ˫��Դ���·��ʹ���
                    else
                    {
                        wCurMtReqBR = m_ptMtTable->GetMtReqBitrate(tMt.GetMtId());
                        wRealMtReqBR = wBitRate * 2;
                    }
                    
                    m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wRealMtReqBR, TRUE) , LOGCHL_VIDEO );
                    m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wRealMtReqBR, FALSE) , LOGCHL_SECVIDEO );
                    
                    //����������ʱ������ֹ�ڶ�·���Ͷ˴���ƫ��
                    //SetTimer( MCUVC_SENDFLOWCONTROL_TIMER, 1200 );
                    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] CurMtReqBR.%d while RealReqBR.%d, adjust for E1MtLowed to Fst.%d, Sec.%d\n",
                            wCurMtReqBR, wRealMtReqBR, 
                            m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), LOGCHL_VIDEO),
                            m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), LOGCHL_SECVIDEO) );                    
                }
                else
                {
                    u16 wCurMtReqBR = m_ptMtTable->GetMtReqBitrate(tMt.GetMtId());
                    m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_VIDEO );
                    wRealMtReqBR = wBitRate;
                    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] CurMtReqBR.%d while RealReqBR.%d, adjust for E1MtLowed None DS\n", wCurMtReqBR, wBitRate);
                }
                m_ptMtTable->SetLowedRcvBandWidth(tMt.GetMtId(), wRealMtReqBR);
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] Lowed Mt.%d's LowedRcvBandWidth adjust to.%d\n", tMt.GetMtId(), wRealMtReqBR);
            }

            //zbq[02/18/2008] �����Ƿ�Ϊ�����ն˵�flowctrl�����Ե���Vmp�Ĺ㲥����
			// xliang [8/4/2009] ������ͬchangeVmpBitRate�ϲ�
//             if ( m_tConf.m_tStatus.IsBrdstVMP() )
//             {
//                 AdjustVmpBrdBitRate();
//             }

            //zbq[12/11/2008] ����������������նˣ�ˢ���������
            if (IsNeedAdapt(GetLocalVidBrdSrc(), tMt, MODE_VIDEO))
            {
                RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_VIDEO);
            }
		}
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
            //����������ͬ����Ϣ
            if (wBitRate == m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), FALSE) ||
                (!m_tDoubleStreamSrc.IsNull() && wBitRate > GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE)))
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv snd MtReqBitrate :%d from mt<%d>, ignore it.\n",
                    wBitRate, tMt.GetMtId());
                return;
            }

			m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_SECVIDEO );
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv snd MtReqBitrate :%d from mt<%d>\n",
                    wBitRate, tMt.GetMtId());
            
            //zbq[12/11/2008] ����������������նˣ�ˢ���������
            if (IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO))
            {
                RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_SECVIDEO);
            }
		}
		
		if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
		{
			m_ptMtTable->GetMtSrc( tMt.GetMtId(), &tMtSrc, MODE_VIDEO );
			if(tMtSrc.IsNull())
			{
                //zjj20101111 ����ʱ������E1�ն˱�����ʱ������ƵԴ��δ��ֵ���Ա���һ��				
				tMtSrc = GetLocalVidBrdSrc();				
				if( tMtSrc.IsNull() )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] Mt.%d 's Vid src is Null, ignore it!\n", tMt.GetMtId());
					return;
				}				
			}
			if( tMtSrc == tMt )
			{			
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] Mt.%d 's Vid src is itself, ignore it!\n", tMt.GetMtId());
				return;			
			}
			if( !(tMtSrc == GetLocalVidBrdSrc()) )
			{
				//����ѡ�����նˣ�ֱ��ת������ӦԴ
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2,  "[ProcMtMcuFlowControlCmd] SelectSee SrcMtId.%d, DstMtId.%d, wBitRate.%d, ChnnlType.%d\n", 
						tMtSrc.GetMtId(), tMt.GetMtId(), wBitRate, tLogicChnnl.GetMediaType() );

				AdjustMtVideoSrcBR( tMt.GetMtId(), wBitRate, tLogicChnnl.GetMediaType() );
				return;
			}
		}

        //  ��Ϊ�ڶ�ͨ������ֱ�ӷ���flowcontrol���е���
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
			if(!m_tDoubleStreamSrc.IsNull())
			{
                wMinBitRate = GetLeastMtReqBitrate(FALSE, MEDIA_TYPE_NULL, m_tDoubleStreamSrc.GetMtId()); 
                AdjustMtVideoSrcBR( tMt.GetMtId(), wMinBitRate, MODE_SECVIDEO );
				return;
			}
		}

        //modify bas 2 -- �Ƿ���Ҫ���ֲ�ͬ��ʽ����Сֵ
        tDstSimCapSet = m_ptMtTable->GetDstSCS(tMt.GetMtId());
        if (MODE_VIDEO == tLogicChnnl.GetMediaType())
        {
            wMinBitRate = GetLeastMtReqBitrate(TRUE, tDstSimCapSet.GetVideoMediaType());
			if( bMtLowedAlready &&
				wBitRate < wMinBitRate )
			{
				wMinBitRate = wBitRate;
			}
        }        

		//���������ڹ㲥����ϳ�ͼ��,������ϳ�����
		// xliang [8/4/2009] ���½ӿ�
        if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE 
            && m_tConf.m_tStatus.IsBrdstVMP()
			&& MODE_VIDEO == tLogicChnnl.GetMediaType()
			)
        {
            
			AdjustVmpBrdBitRate(&tMt);
            return;
        }

        if (m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tFirstLogicChnnl, TRUE) &&
            m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE))
        {
            bStartDoubleStream = TRUE;
        }

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2,  "[ProcMtMcuFlowControlCmd] IsUseAdapter.%d, IsStartDStream.%d, SrcMtId.%d, DstMtId.%d, wBitRate.%d, wMinBitRate.%d, ChnnlType.%d\n", 
			    m_tConf.GetConfAttrb().IsUseAdapter(), bStartDoubleStream, tMtSrc.GetMtId(), tMt.GetMtId(), wBitRate, wMinBitRate, tLogicChnnl.GetMediaType() );

        //����������,ֱ�ӵ�������������
		if( !m_tConf.GetConfAttrb().IsUseAdapter() )
		{
			AdjustMtVideoSrcBR( tMt.GetMtId(), wMinBitRate, tLogicChnnl.GetMediaType() );
			return;
		}
        else
        {
            u16 wAdaptBitRate = 0;            

            //˫�ٻ���,��������ʱ,
            //˫���£�����µ��ն˽������ʵ��ڵڶ�����/2�����ֹ���ն˵���Ƶת��
            //��˫���£�����µ��ն˽������ʵ��ڵڶ����ʣ����ֹ���ն˵���Ƶת��
            //��������ת��
            if (0 != m_tConf.GetSecBitRate())
            {			
                u16 wFirstMinBitRate =  m_tConf.GetSecBitRate();
                if (TRUE == bStartDoubleStream && !IsVidBrdSrcNull())
                {
                    wFirstMinBitRate /= 2;
                }
                u16 wFabsBitrate;
                u16 wMainAudioBitrate = GetAudioBitrate(m_tConf.GetMainAudioMediaType());
                u16 wSecAudioBitrate = GetAudioBitrate(m_tConf.GetSecAudioMediaType());
                if (wMainAudioBitrate < wSecAudioBitrate)
                {
                    wFabsBitrate = wSecAudioBitrate - wMainAudioBitrate;
                }
                else
                {
                    wFabsBitrate = wMainAudioBitrate - wSecAudioBitrate;
                }
                if (wBitRate < wFirstMinBitRate - wFabsBitrate)
                {

                    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuFlowControlCmd] req bitrate < second bitrate, ignore it\n"); 
                    return;
                }            
            }

//            RefreshRcvGrp(tMt.GetMtId());
			switch(tLogicChnnl.GetMediaType())
			{
			case MODE_VIDEO:
				{
					//�㲥Դ
					if (!tMtSrc.IsNull())
					{
						if (tMtSrc == GetLocalVidBrdSrc())
						{
							if (IsNeedAdapt(tMtSrc, tMt, MODE_VIDEO))
							{
								if (m_tConf.m_tStatus.IsVidAdapting())
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_VIDEO);
									StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_VIDEO);
								}
								else
								{
									StartBrdAdapt(MODE_VIDEO);
								}
							}
							else
							{
								AdjustMtVideoSrcBR( tMt.GetMtId(), wMinBitRate, MODE_VIDEO );
							}
						}
						//ѡ��Դ(�Ժ�)
						else
						{
							
						}
					}					
				}
				break;
			case MODE_AUDIO:
				{
					//�㲥Դ
					if (!tMtSrc.IsNull())
					{
						if (tMtSrc == GetLocalAudBrdSrc())
						{
							if (IsNeedAdapt(tMtSrc, tMt, MODE_AUDIO))
							{
								if (m_tConf.m_tStatus.IsAudAdapting())
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_AUDIO);
									StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_AUDIO);
								}
								else
								{
									StartBrdAdapt(MODE_AUDIO);
								}
							}
						}
						//ѡ��Դ(�Ժ�)
						else
						{
							
						}
					}				
				}
				break;
			case MODE_SECVIDEO:
				{
					//�㲥Դ
					if (!tMtSrc.IsNull())
					{
						if (tMtSrc == GetLocalVidBrdSrc())
						{
							if (IsNeedAdapt(tMtSrc, tMt, MODE_SECVIDEO))
							{
//								StartDSAdapt(tMtSrc, tMt);
								if (m_tConf.m_tStatus.IsVidAdapting())
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_SECVIDEO);
									StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_SECVIDEO);
								}
								else
								{
									StartBrdAdapt(MODE_SECVIDEO);
								}
							}
						}
					}				
				}
				break;
			default:
				ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcMtMcuFlowControlCmd] unexpected lgc type:%d!\n", tLogicChnnl.GetMediaType());
				break;
			}

        }	*/	
		}	
		break;
		
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: ProcMtMcuFlowControlCmd() Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

    return;
}
/*=============================================================================
    �� �� ���� ProcPolycomMcuFlowctrlCmd
    ��    �ܣ� ��Ӧ����polycom�ն˵�flowctrl����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byPolyMCUId
			   TLogicalChannel &tLogicChnnl
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2008/3/31   4.0			�ű���                  ����
=============================================================================*/
void CMcuVcInst::ProcPolycomMcuFlowctrlCmd(u8 byPolyMCUId, TLogicalChannel &tLogicChnnl)
{	
	if ( MT_MANU_POLYCOM != m_ptMtTable->GetManuId(byPolyMCUId) ||
		 ( MT_TYPE_MMCU != m_ptMtTable->GetMtType(byPolyMCUId) &&
		   MT_TYPE_SMCU != m_ptMtTable->GetMtType(byPolyMCUId)))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] No polycom MCU.%d deal with, ignore it\n", byPolyMCUId );
		return;
	}

	TMt tPolyMCU = m_ptMtTable->GetMt(byPolyMCUId);
	
	//Polymcuֻ�ڸ����ʱ��һ��˫��ͨ��
	if ( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
	{
		if ( tPolyMCU == m_tDoubleStreamSrc )
		{
			StopDoubleStream( TRUE, FALSE );
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcPolycomMcuFlowctrlCmd] StopDS(MODE_SECVIDEO)\n" );
		}

		TLogicalChannel tFstLogicChnnl;
		TLogicalChannel tSecLogicChnnl;
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstLogicChnnl, TRUE) )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed\n", byPolyMCUId );
			return;
		}
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE) )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed\n", byPolyMCUId );
			return;
		}

		u16 wSecBR = tLogicChnnl.GetFlowControl();
		tSecLogicChnnl.SetFlowControl(wSecBR);
		u16 wFstBR = m_ptMtTable->GetDialBitrate(byPolyMCUId) - wSecBR;
		tFstLogicChnnl.SetFlowControl(wFstBR);

		m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstLogicChnnl, TRUE);
		m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE);

		ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "[ProcPolycomMcuFlowctrlCmd] MODE_SECVIDEO has been ctrled, wBR.%d\n", tLogicChnnl.GetFlowControl() );
	}
	else if ( MODE_VIDEO == tLogicChnnl.GetMediaType() )
	{
		
		//���Լ���
		u16 wDailBR = m_ptMtTable->GetDialBitrate(byPolyMCUId);

		TLogicalChannel tFstLogicChnnl;
		TLogicalChannel tSecLogicChnnl;
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstLogicChnnl, TRUE) )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed(1)\n", byPolyMCUId );
			return;
		}
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE) )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed(1)\n", byPolyMCUId );
			return;
		}

		u16 wFstBR = tLogicChnnl.GetFlowControl();
		tFstLogicChnnl.SetFlowControl(wFstBR);
		
		u16 wSecBR = m_ptMtTable->GetDialBitrate(byPolyMCUId) - wFstBR;
		if ( wSecBR * 100 / wDailBR < 5 )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] wSecBR.%d, wDailBR.%d, sec BR adjust to 0\n", wSecBR, wDailBR );
			wSecBR = 0;
		}
		tSecLogicChnnl.SetFlowControl(wSecBR);

		m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstLogicChnnl, TRUE);
		m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE);

		u16 wBitRate = tLogicChnnl.GetFlowControl();


		// �ָ�����ֹͣ˫��
		if ( abs(wBitRate-(wDailBR-64))*100 / wDailBR < 10 )
		{
			if ( tPolyMCU == m_tDoubleStreamSrc )
			{
				StopDoubleStream( TRUE, FALSE );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, StopDS\n", wBitRate, wDailBR );
			}
		}
		//����˫��
		else 
		{
			if ( CONF_DUALMODE_EACHMTATWILL == m_tConf.GetConfAttrb().GetDualMode() ||
				 ( CONF_DUALMODE_SPEAKERONLY == m_tConf.GetConfAttrb().GetDualMode() &&
				   tPolyMCU == GetLocalSpeaker() ) )
			{
				//��ʹ��ǰ��˫��Դ����Polymcuһ���ߵ����ȼ�
				TLogicalChannel tSecVidChnnl;
				if ( m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecVidChnnl, FALSE))
				{
					StartDoubleStream(tPolyMCU, tSecVidChnnl);
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, StartDS\n", wBitRate, wDailBR );
				}
				else
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, should but no StartDS due to get sec chnnl failed\n", wBitRate, wDailBR );
				}
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, should but no StartDS due to dualmode.%d\n",
								 wBitRate, wDailBR, m_tConf.GetConfAttrb().GetDualMode());
			}
		}
	}
	else
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] unexpected chnnl mode.%d\n", tLogicChnnl.GetMediaType() );
	}

	//��flowctrl�����ն˵�ǰ��ͨ������Ӧ�ڶ�polymcu�����⴦��ͬʱ���������ͨ��
	TLogicalChannel tFstFwdChnnl;
	TLogicalChannel tFstRcvChnnl;
	TLogicalChannel tSecFwdChnnl;
	TLogicalChannel tSecRcvChnnl;

	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstFwdChnnl, TRUE) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Fwd VIDEO chnnl failed\n", byPolyMCUId );
		return;
	}
	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstRcvChnnl, FALSE) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Rcv VIDEO chnnl failed\n", byPolyMCUId );
		return;
	}
	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecFwdChnnl, TRUE) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Fwd LOGCHL_SECVIDEO chnnl failed\n", byPolyMCUId );
		return;
	}
	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecRcvChnnl, FALSE) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Rcv LOGCHL_SECVIDEO chnnl failed\n", byPolyMCUId );
		return;
	}

	u16 wFstRcvBR = tFstRcvChnnl.GetFlowControl();
	u16 wSecRcvBR = tSecRcvChnnl.GetFlowControl();

	tFstRcvChnnl.SetFlowControl(tFstFwdChnnl.GetFlowControl());
	tSecRcvChnnl.SetFlowControl(tSecFwdChnnl.GetFlowControl());

	m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstRcvChnnl, FALSE);
	m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecRcvChnnl, FALSE);

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d FstRcvBR adjust from.%d to %d, SecRcvBR adjust from.%d to %d\n",
			  byPolyMCUId, 
			  wFstRcvBR, tFstRcvChnnl.GetFlowControl(),
			  wSecRcvBR, tSecRcvChnnl.GetFlowControl());
	return;
}
/*=============================================================================
    �� �� ���� ProcMtMcuFlowControlIndication
    ��    �ܣ� �ն�Ҫ��ı䷢������ - ��������ָ����sony G70SP�˫����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/21   3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ProcMtMcuFlowControlIndication( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt;
	TLogicalChannel	tLogicChnnl;
	TLogicalChannel tH239LogicChnnl;
	u16 wBitRate = 0;

	switch( CurState() )
	{
	case STATE_ONGOING:

		tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );

		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );
		wBitRate = tLogicChnnl.GetFlowControl();

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2,  "[Mt2McuFlowctrlInd] MtId.%d, ManuId.%d, DStreamMtId.%d, wBitRate.%d, ChnnlType.%d\n", 
				tMt.GetMtId(), m_ptMtTable->GetManuId( tMt.GetMtId() ), 
				m_tDoubleStreamSrc.GetMtId(), wBitRate, tLogicChnnl.GetMediaType() );
		
		// xliang [11/14/2008] ���Ӵ����polycom��ָʾ��Ӧ
		if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId( tMt.GetMtId() ) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMtMcuFlowControlIndication] PolycomMt.%d Ind ChnType.%d BR.%d\n",
				tMt.GetMtId(), tLogicChnnl.GetMediaType(), wBitRate );
			return;
		}
		//sony G70SP��H263-H239ʱ���û˫��,ʹ��FlowControlIndication���з��ͼ����ر�
		//�������������µ�FlowControlIndication
		if( MT_MANU_SONY != m_ptMtTable->GetManuId( tMt.GetMtId() ) || 
			MEDIA_TYPE_H263 != m_tConf.GetCapSupport().GetDStreamMediaType() || 
			!m_tConf.GetCapSupport().IsDStreamSupportH239() ||
			MODE_SECVIDEO != tLogicChnnl.GetMediaType() ||  
			!m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ) )
		{
			return;
		}

		//FlowControlIndication To Zero, Stop Active DStream
		if( 0 == wBitRate )
		{
			if( m_tDoubleStreamSrc == tMt )
			{
				StopDoubleStream( FALSE, FALSE );
			}
		}
		else
		{
			//�ж�˫���ķ���Դ
			if ( CONF_DUALMODE_EACHMTATWILL == m_tConf.GetConfAttrb().GetDualMode() ||
				 (CONF_DUALMODE_SPEAKERONLY == m_tConf.GetConfAttrb().GetDualMode() && tMt == GetLocalSpeaker()) ) 
			{
                // xsl [7/20/2006]���Ƿ�ɢ����ʱ��Ҫ���ǻش�ͨ����
                if (m_tConf.GetConfAttrb().IsSatDCastMode()/* && IsOverSatCastChnnlNum(tMt.GetMtId())*/
					&& IsMultiCastMt(tMt.GetMtId())
					&& //IsSatMtOverConfDCastNum(tMt))
					!IsSatMtCanContinue(tMt.GetMtId(),emstartDs))
                {
                    ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[Mt2McuFlowctrlInd] over max upload mt num.\n");
                    return;
                }
               
				//�˫��ʱ�������������Ч��
				//UpdateH239TokenOwnerInfo( tMt );
				StartDoubleStream( tMt, tH239LogicChnnl );
			}
			else
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2,  "[Mt2McuFlowctrlInd] MtId.%d, ManuId.%d, DStreamMtId.%d, wBitRate.%d, ChnnlType.%d open double stream with wrong DualMode, NACK !\n", 
					tMt.GetMtId(), m_ptMtTable->GetManuId( tMt.GetMtId() ), m_tDoubleStreamSrc.GetMtId(), wBitRate, tLogicChnnl.GetMediaType() );
			}
		}
		break;
		
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[Mt2McuFlowctrlInd] Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��HasJoinedSubMtNeedAdapt
    ����        ���������Ƿ���ֱ������ն���Ҫ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/18    3.0         ������          ����
====================================================================*/
/*u8   CMcuVcInst::GetNeedAdaptMtNum( u8 byAdaptType )
{
	u8 byMtNum = 0;
	u8	byLoop;

	for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
		{
			if( IsMtNeedAdapt(byAdaptType, byLoop) )
			{
				byMtNum++;
			}
		}
	}
	
	return byMtNum;
}*/
/*====================================================================
    ������      :GetMinMtRcvBitByVmpChn
    ����        ���õ����ն˽��յ�VMP�������СҪ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bPrimary [IN]����Ƶ�ŵ���־�������õ��ն˽��յ�
                     ����Ƶ�ŵ����ʱ�־
                  u8 byVmpOutChnnl[IN]Vmp���ĳͨ����
                  u8 bySrcMtId    [IN]ԴMtId��������ʱû�У�����������
                     �����ն��б�ʱ�����㿪ʼ
    ����ֵ˵��  ������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    11/05/30    4.6         ���         ����
====================================================================*/
u16 CMcuVcInst::GetMinMtRcvBitByVmpChn(u8 byVmpId, BOOL32 bPrimary,u8 byVmpOutChnnl,u8 bySrcMtId /* = 0*/)
{
    u16 wMinBitRate = m_tConf.GetBitRate();
    u16 wMtBitRate;
    u8  byScale = 100;
    
	if (!IsValidVmpId(byVmpId))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"[GetMinMtRcvBitByVmpChn]VmpId��%d is wrong \n",byVmpId );
		return wMinBitRate;
	}
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
    // xsl [8/7/2006] ���ش�ͨ�������նˣ���mcu�������ʲ����бȽ�(��Ϊmcu���������ɻش��ն˷������ʾ���)
    // ��֤�ϼ�������ʱ����Ӱ���¼��㲥Դ����
    BOOL32 bMMcuSpyMtOprSkip = FALSE;
    if( !m_tCascadeMMCU.IsNull() )
    {
        TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
        // guzh [3/26/2007] �����������Ϊ�˴������µ����⣺
        // ���ϼ�MCU���� �����Ļش��ն� m_tSpyMt flowcontrol ����ʱ
        // һ������Ҫ����ģ��������ܱ�֤��˫��������¼��ش��ĵڶ�·���ʺ��ʣ�˫�������䣩
        // �����������Իᵼ���ϼ�������ʱ��ͬʱ�����¼����Եĸ��նˣ���Ӱ���¼��㲥Դ����Ҳͬʱ�½�
        // ����Ŀǰ�Ĳ����ǣ������˫�������������ֻҪ�����˲��ǻش��նˣ��Ͳ���������MMCU��
        // ���������(bMMcuSpyMtOprSkip=TRUE)
        if ( ptConfMcInfo != NULL && 
            !ptConfMcInfo->m_tSpyMt.IsNull() && 
            bySrcMtId == ptConfMcInfo->m_tSpyMt.GetMtId() )
        {
            if ( !m_tDoubleStreamSrc.IsNull())
                bMMcuSpyMtOprSkip = FALSE;
            else if ( !HasJoinedSpeaker() || GetLocalSpeaker().GetMtId() != ptConfMcInfo->m_tSpyMt.GetMtId() ) 
                bMMcuSpyMtOprSkip = FALSE;
            else
                bMMcuSpyMtOprSkip = TRUE;
        }
    }

	//����vmp���ն˵Ľ���
	TLogicalChannel tFwLogicChnnl;
	TMtStatus tMtStatus;
	TMt tSelectSrc;
	BOOL32 bIsVMPBrdst = FALSE;
	TPeriEqpStatus tPeriEqpStatus; 
	u8 byMtRecVmpChnnl;
	g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	if (tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam().IsVMPBrdst() || 
		g_cMcuVcApp.IsBrdstVMP(tVmpEqp))
	{
		bIsVMPBrdst = TRUE;
	}
	for(u8 byMtLoop = 0; byMtLoop < MAXNUM_CONF_MT; byMtLoop ++)
	{
	    if (m_tConfAllMtInfo.MtJoinedConf(byMtLoop) && bySrcMtId != byMtLoop)
        {
			tFwLogicChnnl.Clear();
			tMtStatus.Clear();
			if (m_tCascadeMMCU.GetMtId() == byMtLoop && bMMcuSpyMtOprSkip )
            {
				continue;
            }
			if (!m_ptMtTable->GetMtLogicChnnl( byMtLoop,LOGCHL_VIDEO,&tFwLogicChnnl,TRUE ) )
			{
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"[GetMinMtRcvBitByVmpChn]Get DstMt��%d forward logicalchnnl failed \n",byMtLoop );
				continue;
			}        
			// vmp�ǹ㲥״̬��,����ѡ��Դ����vmp���ն�
			if( !bIsVMPBrdst )
			{
				m_ptMtTable->GetMtStatus(byMtLoop, &tMtStatus);
				tSelectSrc = tMtStatus.GetSelectMt(MODE_VIDEO);
				// ѡ��Դ��������vmpʱ,����ͳ��
				if (!(TYPE_MCUPERI == tSelectSrc.GetType() && EQP_TYPE_VMP == tSelectSrc.GetEqpType() && tSelectSrc.GetEqpId() == tVmpEqp.GetEqpId()))
				{
					continue;
				}
			}

      	    byMtRecVmpChnnl  = GetVmpOutChnnlByRes(tVmpEqp.GetEqpId(),
				tFwLogicChnnl.GetVideoFormat(),
				tFwLogicChnnl.GetChannelType(),
				tFwLogicChnnl.GetChanVidFPS(), 
				tFwLogicChnnl.GetProfileAttrb());
			
            if (byMtRecVmpChnnl != byVmpOutChnnl )
			{
				continue;
			}           
	
			wMtBitRate = m_ptMtTable->GetMtReqBitrate(byMtLoop, bPrimary);
					
			if (wMtBitRate != 0 && wMtBitRate < wMinBitRate)
			{
				wMinBitRate = wMtBitRate;
			}
		}
	}
       

    if (0 != m_tConf.GetSecBitRate() && bIsVMPBrdst)   //˫�ٻ���
    {
        // guzh [7/10/2007] ����¼����Ҫ�����Ƿ��BrBas¼        
        BOOL32 bSecRateDirectly = FALSE;

        if (m_tConf.GetConfAttrb().IsMulticastMode() && m_tConf.GetConfAttrb().IsMulcastLowStream())
        {
			if ( !m_tVmpEqp.IsNull() && g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus ))
			{
				if (tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam().IsVMPBrdst())
				{
					TVideoStreamCap tStrCap = m_tConf.GetMainSimCapSet().GetVideoCap();
					u8 byChnnl4MultiCast = GetVmpOutChnnlByRes (m_tVmpEqp.GetEqpId(),
						tStrCap.GetResolution(), 
						tStrCap.GetMediaType(),
						tStrCap.GetUserDefFrameRate(),
						tStrCap.GetH264ProfileAttrb());
					if (  byVmpOutChnnl == byChnnl4MultiCast )
					{
						bSecRateDirectly = TRUE;
					}
				}
				
			}

        }

        if (!bSecRateDirectly && m_tRecPara.IsRecLowStream())
        {
			TMt tRecSrc;
            u8 byRecChnlIdx = 0;
			u8 byBasChnId   = 0;
            // guzh [7/27/2007] ��Ҫ���ֻ�ȡ������ʽ���Ƿ�����ʽ��������ʣ���Ϊ��Ϊ��Զ¼����ʽ��
            if (IsRecordSrcBas(MODE_VIDEO, tRecSrc, byBasChnId, byRecChnlIdx))
            {
                {
                    bSecRateDirectly = TRUE;
                }
            } //¼�������¼�񣬱��������û���������
			else if (!m_tConf.m_tStatus.IsNoRecording())
			{
				TVideoStreamCap tStrCap = m_tConf.GetMainSimCapSet().GetVideoCap();
				u8 byChnnl = GetVmpOutChnnlByRes (m_tVmpEqp.GetEqpId(),
					tStrCap.GetResolution(), 
					tStrCap.GetMediaType(),
					tStrCap.GetUserDefFrameRate(),
					tStrCap.GetH264ProfileAttrb());
				if (byVmpOutChnnl == byChnnl)
				{
					bSecRateDirectly = TRUE;
				}
			}
        }

        if (bSecRateDirectly)
        {
            wMinBitRate = m_tConf.GetSecBitRate()*byScale/100;
        }
        else
        {
			const u16 wSecBitrate = m_tConf.GetSecBitRate()*byScale/100;
            wMinBitRate = kmax(wMinBitRate, wSecBitrate);
        }
    }

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[GetLeastMtReqBitrate] Bitrate.%d, primary.%d, chnl.%d\n", 
            wMinBitRate, bPrimary, byVmpOutChnnl);

    return wMinBitRate;
}
/*====================================================================
    ������      GetLstRcvMediaResMtBr
    ����        ���õ�ĳ������ĳ�ֱַ��ʽ����ն�����СҪ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/08/04    4.6         Ѧ��          ����
====================================================================*/
u16 CMcuVcInst::GetLstRcvMediaResMtBr(BOOL32 bPrimary, u8 byMediaType, u8 byRes, u8 bySrcMtId /* = 0*/)
{
    u16 wMinBitRate = m_tConf.GetBitRate();
    u16 wMtBitRate;
	//    u8	byLoop;
    u8  byScale = 100;
    
    // xsl [8/7/2006] ���ش�ͨ�������նˣ���mcu�������ʲ����бȽ�(��Ϊmcu���������ɻش��ն˷������ʾ���)
    // ��֤�ϼ�������ʱ����Ӱ���¼��㲥Դ����
    BOOL32 bMMcuSpyMtOprSkip = FALSE;
    if( !m_tCascadeMMCU.IsNull() )
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
        // guzh [3/26/2007] �����������Ϊ�˴������µ����⣺
        // ���ϼ�MCU���� �����Ļش��ն� m_tSpyMt flowcontrol ����ʱ
        // һ������Ҫ����ģ��������ܱ�֤��˫��������¼��ش��ĵڶ�·���ʺ��ʣ�˫�������䣩
        // �����������Իᵼ���ϼ�������ʱ��ͬʱ�����¼����Եĸ��նˣ���Ӱ���¼��㲥Դ����Ҳͬʱ�½�
        // ����Ŀǰ�Ĳ����ǣ������˫�������������ֻҪ�����˲��ǻش��նˣ��Ͳ���������MMCU��
        // ���������(bMMcuSpyMtOprSkip=TRUE)
		if ( ptConfMcInfo != NULL && 
			!ptConfMcInfo->m_tSpyMt.IsNull() && 
			bySrcMtId == ptConfMcInfo->m_tSpyMt.GetMtId() )
        {
            if ( !m_tDoubleStreamSrc.IsNull())
                bMMcuSpyMtOprSkip = FALSE;
            else if ( !HasJoinedSpeaker() || GetLocalSpeaker().GetMtId() != ptConfMcInfo->m_tSpyMt.GetMtId() ) 
                bMMcuSpyMtOprSkip = FALSE;
            else
                bMMcuSpyMtOprSkip = TRUE;
        }
    }
	
    if (!m_tDoubleStreamSrc.IsNull())
    {
        if (bPrimary)
        {
            byScale = 100 - m_tConf.GetDStreamScale();       
        }
        else
        {
            byScale = m_tConf.GetDStreamScale();        
        }
    }
    wMinBitRate = wMinBitRate*byScale/100;
	
	
	//zhouyiliang 20101222 ����vmp���ն˵Ľ���
	//	u8  byNum = 0;
	//	u8	abyMtId[MAXNUM_CONF_MT] = { 0 };
	//m_cMtRcvGrp.GetMtMediaRes(tMt.GetMtId(), byMVType, byRes);
	
	//m_cMtRcvGrp.GetMVMtList(byMediaType, byRes, byNum, abyMtId, TRUE);//ǿ��ȡ����MT�б�������������
	
	//	if(byNum > 0)
	//	{
	for(u8 byMtLoop = 0; byMtLoop < MAXNUM_CONF_MT; byMtLoop ++)
	{
		//byMtLoop = abyMtId[byLoop];
		
		if (m_tConfAllMtInfo.MtJoinedConf(byMtLoop) && bySrcMtId != byMtLoop)
		{
			if (m_tCascadeMMCU.GetMtId() == byMtLoop && bMMcuSpyMtOprSkip )
				continue;
			TLogicalChannel tFwLogicChnnl;
			if (!m_ptMtTable->GetMtLogicChnnl( byMtLoop,LOGCHL_VIDEO,&tFwLogicChnnl,TRUE ) )
			{
				continue;
			}
			if (byMediaType != tFwLogicChnnl.GetChannelType() || byRes != tFwLogicChnnl.GetVideoFormat() )
			{
				continue;
			}
			//				tMtDstSCS = m_ptMtTable->GetDstSCS(byMtLoop);
			//				if (MEDIA_TYPE_NULL == byMediaType || tMtDstSCS.GetVideoMediaType() == byMediaType)
			//				{
			//zbq[07/28/2009] ˫��flowctrl����������Ӧ���������ն˵�flowctrl��AdpParam��Ȼ����
			//					if (bPrimary &&
			//						m_cMtRcvGrp.IsMtNeedAdp(byMtLoop))
			//					{
			//						continue;
			//					}
			//					if (!bPrimary &&
			//						m_cMtRcvGrp.IsMtNeedAdp(byMtLoop, FALSE))
			//					{
			//						continue;
			//					}
			
			wMtBitRate = m_ptMtTable->GetMtReqBitrate(byMtLoop, bPrimary);
			
			if (wMtBitRate != 0 && wMtBitRate < wMinBitRate)
			{
				wMinBitRate = wMtBitRate;
			}
			//				}
		}
	}
	//    }       
	
    if (0 != m_tConf.GetSecBitRate())   //˫�ٻ���
    {
        // guzh [7/10/2007] ����¼����Ҫ�����Ƿ��BrBas¼        
        BOOL32 bSecRateDirectly = FALSE;
		
        if (m_tConf.GetConfAttrb().IsMulticastMode() && m_tConf.GetConfAttrb().IsMulcastLowStream())
        {
		/*
		#ifdef _MINIMCU_
		// guzh [8/29/2007] ˫ý����飬���ִ�BrBas��VidBas�鲥
		if ( !ISTRUE(m_byIsDoubleMediaConf) )
		{
		bSecRateDirectly = TRUE;
		}
		else if ( MEDIA_TYPE_NULL == byMediaType ||
		( MEDIA_TYPE_NULL != byMediaType &&
		m_tConf.GetSecVideoMediaType() == byMediaType ) 
		) 
		{
		bSecRateDirectly = TRUE;
		}
		#else
			*/
			// [5/9/2011 xliang] �鲥��ַ�յ��ǳ�����������ʽ���ֱ����ն˽��յ��ǳ�Ӧ������ͬ�ģ�
			// ��������Ϊ�յ�������ʽ���ֱ��ʵ��ǳ�����Ϊ�е�VMP�����಻���ǳ�����eg:
			// conf: 1080, 720, 8kg VMP can only output 720, in that case, 720 also multicast.
			// 			if ( byMediaType == m_tConf.GetMainVideoMediaType() 
			// 				&& byRes == m_tConf.GetMainVideoFormat() )
			// 			{
			// 				bSecRateDirectly = TRUE;
			// 			}
			
			TPeriEqpStatus tPeriEqpStatus;
			if ( !m_tVmpEqp.IsNull() && g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus ))
			{
				if (tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam().IsVMPBrdst())
				{
					u8 byChnnl = GetVmpOutChnnlByRes(m_tVmpEqp.GetEqpId(), byRes, byMediaType); //�˺���δ���õ�
					TVideoStreamCap tStrCap = m_tConf.GetMainSimCapSet().GetVideoCap();
					u8 byChnnl4MultiCast = GetVmpOutChnnlByRes (m_tVmpEqp.GetEqpId(),
						tStrCap.GetResolution(), 
						tStrCap.GetMediaType(),
						tStrCap.GetUserDefFrameRate(),
						tStrCap.GetH264ProfileAttrb());
					if ( byChnnl == byChnnl4MultiCast )
					{
						bSecRateDirectly = TRUE;
					}
				}
				
			}
            // ˫ý����鲻֧�ֵ����鲥
            //bSecRateDirectly = TRUE;
			/*
			#endif
			*/
        }
		
        if (!bSecRateDirectly && m_tRecPara.IsRecLowStream())
        {
			TMt tRecSrc;
            u8 byRecChnlIdx = 0;
			u8 byBasChnId   = 0;
            // guzh [7/27/2007] ��Ҫ���ֻ�ȡ������ʽ���Ƿ�����ʽ��������ʣ���Ϊ��Ϊ��Զ¼����ʽ��
            if (IsRecordSrcBas(MODE_VIDEO, tRecSrc, byBasChnId, byRecChnlIdx))
            {
				//                 if ( ( tRecSrc == m_tBrBasEqp &&
				//                        ( MEDIA_TYPE_NULL == byMediaType || m_tConf.GetMainVideoMediaType() == byMediaType ) )
				//                    || ( tRecSrc == m_tVidBasEqp &&
				//                         MEDIA_TYPE_NULL != byMediaType &&
				//                         m_tConf.GetSecVideoMediaType() == byMediaType )
				//                    )
                {
                    bSecRateDirectly = TRUE;
                }
            }
        }
		
        if (bSecRateDirectly)
        {
            wMinBitRate = m_tConf.GetSecBitRate()*byScale/100;
        }
        else
        {
			const u16 wSecBitrate = m_tConf.GetSecBitRate()*byScale/100;
            wMinBitRate = kmax(wMinBitRate, wSecBitrate);
        }
    }
	
    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[GetLeastMtReqBitrate] Bitrate.%d, primary.%d, media.%d\n", 
		wMinBitRate, bPrimary, byMediaType);
	
    return wMinBitRate;
}
/*====================================================================
    ������      ��GetLeastMtReqBitrate
    ����        ���õ���С���ն�Ҫ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bPrimary = TRUE
    ����ֵ˵��  ������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/05/21    3.0         ������          ����
====================================================================*/
//modify bas 2
u16 CMcuVcInst::GetLeastMtReqBitrate(BOOL32 bPrimary, u8 byMediaType, u8 bySrcMtId)
{
    u16 wMinBitRate = m_tConf.GetBitRate();
    u16 wMtBitRate;
    u8	byLoop;
    TSimCapSet tMtDstSCS;
    u8  byScale = 100;
	TMt tSrcMt = m_ptMtTable->GetMt( bySrcMtId );
    
    // xsl [8/7/2006] ���ش�ͨ�������նˣ���mcu�������ʲ����бȽ�(��Ϊmcu���������ɻش��ն˷������ʾ���)
    // ��֤�ϼ�������ʱ����Ӱ���¼��㲥Դ����
    BOOL32 bMMcuSpyMtOprSkip = FALSE;
    if( !m_tCascadeMMCU.IsNull() )
	{
		u16 wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() ); 		
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMMcuIdx );//m_tCascadeMMCU.GetMtId());
        // guzh [3/26/2007] �����������Ϊ�˴������µ����⣺
        // ���ϼ�MCU���� �����Ļش��ն� m_tSpyMt flowcontrol ����ʱ
        // һ������Ҫ����ģ��������ܱ�֤��˫��������¼��ش��ĵڶ�·���ʺ��ʣ�˫�������䣩
        // �����������Իᵼ���ϼ�������ʱ��ͬʱ�����¼����Եĸ��նˣ���Ӱ���¼��㲥Դ����Ҳͬʱ�½�
        // ����Ŀǰ�Ĳ����ǣ������˫�������������ֻҪ�����˲��ǻش��նˣ��Ͳ���������MMCU��
        // ���������(bMMcuSpyMtOprSkip=TRUE)
		if ( ptConfMcInfo != NULL && 
             !ptConfMcInfo->m_tSpyMt.IsNull() && 
             bySrcMtId == ptConfMcInfo->m_tSpyMt.GetMtId() )
        {
            if ( !m_tDoubleStreamSrc.IsNull())
                bMMcuSpyMtOprSkip = FALSE;
            else if ( !HasJoinedSpeaker() || GetLocalSpeaker().GetMtId() != ptConfMcInfo->m_tSpyMt.GetMtId() ) 
                bMMcuSpyMtOprSkip = FALSE;
            else
                bMMcuSpyMtOprSkip = TRUE;
        }
    }

    if (!m_tDoubleStreamSrc.IsNull() || !m_tSecVidBrdSrc.IsNull())
    {
        if (bPrimary)
        {
            byScale = 100 - m_tConf.GetDStreamScale();       
        }
        else
        {
            byScale = m_tConf.GetDStreamScale();        
        }

		//�ȳ˻�����˫�����ʣ����ü������������������GetDoubleStreamVideoBitrate��������һ��
		//��������С�������1bitrate�����[9/17/2012 chendaiwei]
		//wMinBitRate = wMinBitRate - wMinBitRate*m_tConf.GetDStreamScale()/100;
    }

    wMinBitRate = wMinBitRate*byScale/100;

    for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && bySrcMtId != byLoop)
        {
            if (m_tCascadeMMCU.GetMtId() == byLoop && bMMcuSpyMtOprSkip )
                continue;
            
            tMtDstSCS = m_ptMtTable->GetDstSCS(byLoop);
            if (MEDIA_TYPE_NULL == byMediaType || tMtDstSCS.GetVideoMediaType() == byMediaType)
            {
				if (bPrimary && IsNeedAdapt(GetVidBrdSrc(), m_ptMtTable->GetMt(byLoop), MODE_VIDEO))
				{
					continue;
				}

				if (!bPrimary && !m_tDoubleStreamSrc.IsNull()
					&& IsNeedAdapt(m_tDoubleStreamSrc, m_ptMtTable->GetMt(byLoop), MODE_SECVIDEO))
				{
					continue;
				}
				
				if (!bPrimary && !GetSecVidBrdSrc().IsNull()
					&& IsNeedAdapt(GetSecVidBrdSrc(), m_ptMtTable->GetMt(byLoop), MODE_VIDEO2SECOND))
				{
					continue;
				}

                wMtBitRate = m_ptMtTable->GetMtReqBitrate(byLoop, bPrimary);

				//zjj20120511 E1�����ٲ��Ե���,עȥ
				/*if ( m_ptMtTable->GetMtTransE1(byLoop) &&
						m_ptMtTable->GetMtBRBeLowed(byLoop) && bPrimary &&
						tSrcMt == GetLocalVidBrdSrc()
						)
				{					
					wMinBitRate = m_ptMtTable->GetLowedRcvBandWidth( byLoop );

					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[GetLeastMtReqBitrate] mt.%d is Transport by E1 Lowed Bitrate.%d\n",
							byLoop,wMinBitRate );
				}*/
				
                if (wMtBitRate != 0 && wMtBitRate < wMinBitRate)
                {
                    wMinBitRate = wMtBitRate;
                }
            }
        }
    }       

    if (0 != m_tConf.GetSecBitRate())   //˫�ٻ���
    {
        // guzh [7/10/2007] ����¼����Ҫ�����Ƿ��BrBas¼        
        BOOL32 bSecRateDirectly = FALSE;

        if (m_tConf.GetConfAttrb().IsMulticastMode() && m_tConf.GetConfAttrb().IsMulcastLowStream())
        {
/*
#ifdef _MINIMCU_
            // guzh [8/29/2007] ˫ý����飬���ִ�BrBas��VidBas�鲥
            if ( !ISTRUE(m_byIsDoubleMediaConf) )
            {
                bSecRateDirectly = TRUE;
             }
            else if ( MEDIA_TYPE_NULL == byMediaType ||
                      ( MEDIA_TYPE_NULL != byMediaType &&
                        m_tConf.GetSecVideoMediaType() == byMediaType ) 
                    ) 
            {
                bSecRateDirectly = TRUE;
            }
#else
*/
            // ˫ý����鲻֧�ֵ����鲥
            bSecRateDirectly = TRUE;
/*
#endif
*/
        }

        if (!bSecRateDirectly && m_tRecPara.IsRecLowStream())
        {
			TMt tRecSrc;
            u8  byRecChnlIdx = 0;
			u8  byBasChnId	 = 0;
            // guzh [7/27/2007] ��Ҫ���ֻ�ȡ������ʽ���Ƿ�����ʽ��������ʣ���Ϊ��Ϊ��Զ¼����ʽ��
            if (IsRecordSrcBas(MODE_VIDEO, tRecSrc, byBasChnId, byRecChnlIdx))
            {
//                 if ( ( tRecSrc == m_tBrBasEqp &&
//                        ( MEDIA_TYPE_NULL == byMediaType || m_tConf.GetMainVideoMediaType() == byMediaType ) )
//                    || ( tRecSrc == m_tVidBasEqp &&
//                         MEDIA_TYPE_NULL != byMediaType &&
//                         m_tConf.GetSecVideoMediaType() == byMediaType )
//                    )
                {
                    bSecRateDirectly = TRUE;
                }
            }
        }

        if (bSecRateDirectly)
        {
            wMinBitRate = m_tConf.GetSecBitRate()*byScale/100;
        }
        else
        {
			const u16 wSecBitrate = m_tConf.GetSecBitRate()*byScale/100;
            wMinBitRate = kmax(wMinBitRate, wSecBitrate);
        }
    }

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[GetLeastMtReqBitrate] Bitrate.%d, primary.%d, media.%d\n", 
            wMinBitRate, bPrimary, byMediaType);

    return wMinBitRate;
}

/*====================================================================
    ������      ��GetLeastMtReqBR
    ����        ���õ���С���ն�Ҫ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32      bPrimary
                  TSimCapSet &tSrcSCS 
                  u8          bySrtMtId
    ����ֵ˵��  ������
                  Ŀǰ�ݲ�֧��˫���ı���ȶԻ�ȡ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/10/2008  4.5         �ű���          ����
====================================================================*/
u16 CMcuVcInst::GetLeastMtReqBR(TSimCapSet &tSrcSCS, 
                                u8          bySrcMtId /* = 0 */,
                                BOOL32      bPrimary  /* = TRUE */)
{
    u16 wMinBitRate = m_tConf.GetBitRate();
    u16 wMtBitRate;
    u8	byLoop;
    TSimCapSet tMtDstSCS;
    u8  byScale = 100;

    u8 byMediaType = tSrcSCS.GetVideoMediaType();
    u8 byMediaRes = tSrcSCS.GetVideoResolution();
    
    // xsl [8/7/2006] ���ش�ͨ�������նˣ���mcu�������ʲ����бȽ�(��Ϊmcu���������ɻش��ն˷������ʾ���)
    // ��֤�ϼ�������ʱ����Ӱ���¼��㲥Դ����
    BOOL32 bMMcuSpyMtOprSkip = FALSE;
    if( !m_tCascadeMMCU.IsNull() )
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
        // guzh [3/26/2007] �����������Ϊ�˴������µ����⣺
        // ���ϼ�MCU���� �����Ļش��ն� m_tSpyMt flowcontrol ����ʱ
        // һ������Ҫ����ģ��������ܱ�֤��˫��������¼��ش��ĵڶ�·���ʺ��ʣ�˫�������䣩
        // �����������Իᵼ���ϼ�������ʱ��ͬʱ�����¼����Եĸ��նˣ���Ӱ���¼��㲥Դ����Ҳͬʱ�½�
        // ����Ŀǰ�Ĳ����ǣ������˫�������������ֻҪ�����˲��ǻش��նˣ��Ͳ���������MMCU��
        // ���������(bMMcuSpyMtOprSkip=TRUE)
		if ( ptConfMcInfo != NULL && 
             !ptConfMcInfo->m_tSpyMt.IsNull() && 
             bySrcMtId == ptConfMcInfo->m_tSpyMt.GetMtId() )
        {
            if ( !m_tDoubleStreamSrc.IsNull())
                bMMcuSpyMtOprSkip = FALSE;
            else if ( !HasJoinedSpeaker() || GetLocalSpeaker().GetMtId() != ptConfMcInfo->m_tSpyMt.GetMtId() ) 
                bMMcuSpyMtOprSkip = FALSE;
            else
                bMMcuSpyMtOprSkip = TRUE;
        }
    }

    if (!m_tDoubleStreamSrc.IsNull())
    {
        if (bPrimary)
        {
            byScale = 100 - m_tConf.GetDStreamScale();       
        }
        else
        {
            byScale = m_tConf.GetDStreamScale();        
        }
    }
    wMinBitRate = wMinBitRate*byScale/100;

    for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && bySrcMtId != byLoop)
        {
            if (m_tCascadeMMCU.GetMtId() == byLoop && bMMcuSpyMtOprSkip )
                continue;
            
            tMtDstSCS = m_ptMtTable->GetDstSCS(byLoop);
            if (tMtDstSCS.GetVideoMediaType() == byMediaType &&
                tMtDstSCS.GetVideoResolution() == byMediaRes)
            {
                wMtBitRate = m_ptMtTable->GetMtReqBitrate(byLoop, bPrimary);
                if (wMtBitRate != 0 && wMtBitRate < wMinBitRate)
                {
                    wMinBitRate = wMtBitRate;
                }
            }
        }
    }       

    if (0 != m_tConf.GetSecBitRate())   //˫�ٻ���
    {
        // guzh [7/10/2007] ����¼����Ҫ�����Ƿ��BrBas¼        
        BOOL32 bSecRateDirectly = FALSE;

        if (m_tConf.GetConfAttrb().IsMulticastMode() && m_tConf.GetConfAttrb().IsMulcastLowStream())
        {
/*
#ifdef _MINIMCU_
            // guzh [8/29/2007] ˫ý����飬���ִ�BrBas��VidBas�鲥
            if ( !ISTRUE(m_byIsDoubleMediaConf) )
            {
                bSecRateDirectly = TRUE;
             }
            else if ( MEDIA_TYPE_NULL == byMediaType ||
                      ( MEDIA_TYPE_NULL != byMediaType &&
                        m_tConf.GetSecVideoMediaType() == byMediaType ) 
                    ) 
            {
                bSecRateDirectly = TRUE;
            }
#else
*/
            // ˫ý����鲻֧�ֵ����鲥
            bSecRateDirectly = TRUE;
/*
#endif
*/
        }

        if (!bSecRateDirectly && m_tRecPara.IsRecLowStream())
        {
            TMt tRecSrc;
            u8 byRecChnlIdx = 0;
			u8 byBasChnId   = 0;
            // guzh [7/27/2007] ��Ҫ���ֻ�ȡ������ʽ���Ƿ�����ʽ��������ʣ���Ϊ��Ϊ��Զ¼����ʽ��
            if (IsRecordSrcBas(MODE_VIDEO, tRecSrc, byBasChnId, byRecChnlIdx))
            {
//                 if ( ( tRecSrc == m_tBrBasEqp &&
//                        ( MEDIA_TYPE_NULL == byMediaType || m_tConf.GetMainVideoMediaType() == byMediaType ) )
//                    || ( tRecSrc == m_tVidBasEqp &&
//                         MEDIA_TYPE_NULL != byMediaType &&
//                         m_tConf.GetSecVideoMediaType() == byMediaType )
//                    )
                {
                    bSecRateDirectly = TRUE;
                }
            }
        }

        if (bSecRateDirectly)
        {
            wMinBitRate = m_tConf.GetSecBitRate()*byScale/100;
        }
        else
        {
			const u16 wSecBitrate = m_tConf.GetSecBitRate()*byScale/100;
            wMinBitRate = kmax(wMinBitRate, wSecBitrate);
        }
    }

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[GetLeastMtReqBitrate] Bitrate.%d, primary.%d, media.%d\n", 
            wMinBitRate, bPrimary, byMediaType);

    return wMinBitRate;
}

/*=============================================================================
�� �� ���� GetLeastMtReqFramerate
��    �ܣ� �õ���С���ն�Ҫ��֡��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMediaType
           u8 bySrcMtId
�� �� ֵ�� u8 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/8/12   4.0		�ܹ��                  ����
=============================================================================*/
u8 CMcuVcInst::GetLeastMtReqFramerate(u8 byMediaType, u8 bySrcMtId)
{
    u8 byMinFrameRate = 0xff;
    u8 byMtFrameRate;
    u8	byLoop;
    TSimCapSet tMtDstSCS;

    for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && bySrcMtId != byLoop)
        {           
            tMtDstSCS = m_ptMtTable->GetDstSCS(byLoop);
            if (MEDIA_TYPE_NULL == byMediaType || tMtDstSCS.GetVideoMediaType() == byMediaType)
            {
                if ( MEDIA_TYPE_H264 == tMtDstSCS.GetVideoMediaType() )
                {
                    byMtFrameRate = tMtDstSCS.GetUserDefFrameRate();
                }
                else
                {
                    byMtFrameRate = tMtDstSCS.GetVideoFrameRate();
                }
                if ( byMtFrameRate != 0 && byMtFrameRate < byMinFrameRate)
                {
                    byMinFrameRate = byMtFrameRate;
                }
            }
        }
    }       

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[GetLeastMtReqFramerate] FrameRate.%d, media.%d\n", byMinFrameRate, byMediaType);

    return byMinFrameRate;
}

/*==============================================================================
������    :  ShowVmpAdaptMember
����      :  ��ʾռVMPǰ����ͨ���ĳ�Ա��Ϣ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-3-27
==============================================================================*/
void CMcuVcInst::ShowVmpAdaptMember (void)
{
	TVmpChnnlInfo tVmpChnnlInfo;
	TEqp tVmpEqp;
	u8 byAdpChnnlNum;
	TChnnlMemberInfo tChnnlMemInfo;
	u16 wMcuIdx;
	u8 byMtId;
	BOOL32 bSpeaker = FALSE;
	BOOL32 bSelected= FALSE;
	BOOL32 bNoneKeda= FALSE;
	BOOL32 bDStream = FALSE;
	BOOL32 bIsMMcu	= FALSE;
	BOOL32 bIPC = FALSE;
	u8 byLoop;
	StaticLog("\n---���� %s�� VMPǰ����ͨ����Ա��Ϣ---\n",m_tConf.GetConfName() );
	for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			continue;
		}
		tVmpEqp = g_cMcuVcApp.GetEqp( m_abyVmpEqpId[byIdx] );
		tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
		byAdpChnnlNum = tVmpChnnlInfo.GetHDChnnlNum();
		StaticLog("   VMP[%d]���֧��%u��񲻽��ֱ��ʽ�vmp,��ǰ����%u��Mtռ��ǰ����ͨ��\n", tVmpEqp.GetEqpId(), tVmpChnnlInfo.GetMaxStyleNum(), byAdpChnnlNum);
		StaticLog("--------------------------------------\n");
		for(byLoop = 0; byLoop < tVmpChnnlInfo.GetMaxNumHdChnnl(); byLoop ++)
		{
			tVmpChnnlInfo.GetHdChnnlInfo(byLoop,&tChnnlMemInfo);
			wMcuIdx = tChnnlMemInfo.GetMt().GetMcuIdx();
			byMtId = tChnnlMemInfo.GetMtId();
			bSpeaker = tChnnlMemInfo.IsAttrSpeaker();
			bSelected = tChnnlMemInfo.IsAttrSelected();
			bNoneKeda = tChnnlMemInfo.IsAttrNoneKeda();
			bDStream = tChnnlMemInfo.IsAttrDstream();
			bIsMMcu = tChnnlMemInfo.IsAttrMMcu();
			bIPC = tChnnlMemInfo.IsAttrBeIPC();
			StaticLog("[ID.%u]:(%u,%u)\tbSpeaker:%d\tbSelected:%d\tbNoneKeda:%d\tbDStream:%d\tbIsSpecMcu:%d\tbIsIPC:%d\n",
				byLoop+1, wMcuIdx, byMtId, bSpeaker, bSelected, bNoneKeda, bDStream, bIsMMcu, bIPC);
		}
	}
}
/*====================================================================
    ������      ��ShowConfMt
    ����        ����ӡ�����ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId    = 0
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/11    3.0         ������          ����
====================================================================*/
void CMcuVcInst::ShowConfMt(u8 byMtId)
{
	BOOL32 bInConf,bInJoinedConf;
	TMtAlias tMtAliasH323id, tMtAliasE164, tMtAliasH320id, tMtAliasH320Alias;
	char achTemp[255];
	TLogicalChannel tLogicalChannel;
	u8 byLoop;

	StaticLog( "\n---���� %s �ն���Ϣ---\n", m_tConf.GetConfName() );
	StaticLog("IsTemplate %d\n", m_tConf.m_tStatus.IsTemplate() );
	StaticLog("InstID %d\n", GetInsID() );
    StaticLog("ConfIdx %d\n", m_byConfIdx );
    m_tConf.GetConfId().GetConfIdString(achTemp, sizeof(achTemp));
    StaticLog("cConfId %s\n", achTemp );
	StaticLog("Current Audio Mt Num.%d\n",g_cMcuVcApp.GetCurrentAudMtNum());
	StaticLog("Current SatMt Num.%d\n",GetCurSatMtNum());

	StaticLog("�����ն�: McId-%d\n", LOCAL_MCUID );

	for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if(NULL == m_ptMtTable)
		{
			break;
		}
		bInConf = m_tConfAllMtInfo.MtInConf( byLoop );
		bInJoinedConf = m_tConfAllMtInfo.MtJoinedConf( byLoop );

        if ( byMtId != 0 )
        {
            if ( byLoop != byMtId )
            {
                continue;
            }
            if ( !bInConf )
            {
                StaticLog("MtId.%d unexist, mishanding ? ...\n", byMtId );
                break;
            }
        }

		if(bInConf)
		{
			if( bInJoinedConf )
			{
				sprintf( achTemp, "�� %c", 0 );
			}
			else
			{
				sprintf( achTemp, "�� %c", 0 ); 
			}

			char achHDMtFlag[5];
			
			if( g_cMcuVcApp.IsOccupyHDAccessPoint(m_byConfIdx, byLoop))
			{
				sprintf( achHDMtFlag, "(HD)");
			}
			else
			{
				sprintf( achHDMtFlag, "");
			}

			char achAudioMt[6];
			if( g_cMcuVcApp.IsOccupyAudAccessPoint(m_byConfIdx, byLoop))
			{
				sprintf( achAudioMt, "(Aud)");
			}
			else
			{
				sprintf( achAudioMt, "");
			}
				
			tMtAliasH323id.SetNull();
			tMtAliasE164.SetNull();
            tMtAliasH320id.SetNull();
            tMtAliasH320Alias.SetNull();
			m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeH323ID, &tMtAliasH323id );
			m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeE164, &tMtAliasE164 );
			m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeH320ID, &tMtAliasH320id );
            m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeH320Alias, &tMtAliasH320Alias);
			TMt tMt = m_ptMtTable->GetMt( byLoop );

            TMtAlias tDailAlias;
            BOOL32 bRet = m_ptMtTable->GetDialAlias(tMt.GetMtId(), &tDailAlias);
            if ( !bRet )
            {
                StaticLog("[ShowConfMt] GetMt.%d DailAlias failed\n", tMt.GetMtId() );
            }

            if ( !bRet )
            {
                StaticLog("%sMT%d:0x%x(Dri:%d Mp:%d),St:%d Type: %d Manu: %s, 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s,ProductId:%s\n",
                                        achTemp, byLoop, m_ptMtTable->GetIPAddr( byLoop ), tMt.GetDriId(), 
                                        m_ptMtTable->GetMpId( tMt.GetMtId() ),IsMultiCastMt(byLoop),
                                        m_ptMtTable->GetMtType( tMt.GetMtId() ),
                                        GetManufactureStr( m_ptMtTable->GetManuId( byLoop ) ), 
                                        tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                        tMtAliasH320Alias.m_achAlias,
										m_ptMtTable->GetProductId(tMt.GetMtId()));
            }
            else
            {
                if ( mtAliasTypeTransportAddress == tDailAlias.m_AliasType )
                {
                    StaticLog("%sMT%d%s%s:0x%x(Dri:%d Mp:%d),St:%d Type: %d Manu: %s, 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s, DialAlias.%s@%d,ProductId:%s\n",
                                            achTemp, byLoop, achHDMtFlag,achAudioMt,m_ptMtTable->GetIPAddr( byLoop ), tMt.GetDriId(), 
                                            m_ptMtTable->GetMpId( tMt.GetMtId() ),IsMultiCastMt(byLoop),
                                            m_ptMtTable->GetMtType( tMt.GetMtId() ),
                                            GetManufactureStr( m_ptMtTable->GetManuId( byLoop ) ), 
                                            tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                            tMtAliasH320Alias.m_achAlias,
                                            StrOfIP(tDailAlias.m_tTransportAddr.GetIpAddr()), tDailAlias.m_tTransportAddr.GetPort(),
											m_ptMtTable->GetProductId(tMt.GetMtId()));
                }
                else
                {
                    StaticLog("%sMT%d%s%s:0x%x(Dri:%d Mp:%d),St:%d Type: %d Manu: %s, 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s, DialAlias.%s,ProductId:%s\n",
                                            achTemp, byLoop, achHDMtFlag,achAudioMt, m_ptMtTable->GetIPAddr( byLoop ), tMt.GetDriId(), 
                                            m_ptMtTable->GetMpId( tMt.GetMtId() ),IsMultiCastMt(byLoop),
                                            m_ptMtTable->GetMtType( tMt.GetMtId() ),
                                            GetManufactureStr( m_ptMtTable->GetManuId( byLoop ) ), 
                                            tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                            tMtAliasH320Alias.m_achAlias,
                                            tDailAlias.m_achAlias,
											m_ptMtTable->GetProductId(tMt.GetMtId()));
                }
            }

			if( bInJoinedConf )
			{
				//�����߼�ͨ����Ϣ
				memset( achTemp, ' ', 255 );
				int l = sprintf( achTemp, "   FL Aud:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)(Num:%d)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                       GetMediaStr( tLogicalChannel.GetChannelType() ),
															   tLogicalChannel.GetAudioTrackNum());
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 33;
				l += sprintf( achTemp+l, "Vid:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )
				{
                    // ���ӷֱ��ʴ�ӡ, zgc, 2008-08-09
					l += sprintf( achTemp+l, "%d(%s)(%s)(%dFps)(%dK)(%s)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                       GetMediaStr( tLogicalChannel.GetChannelType() ),
                                                               GetResStr( tLogicalChannel.GetVideoFormat() ),
															   tLogicalChannel.GetChanVidFPS(),
															   tLogicalChannel.GetFlowControl(),
															   tLogicalChannel.GetProfileAttrb()==emHpAttrb?"HP":"BP");
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 75;//70 //50;
				l += sprintf( achTemp+l, "H239:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tLogicalChannel, TRUE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)(%s)(%dFps)<%dK>(%s)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
                                                            GetMediaStr( tLogicalChannel.GetChannelType() ),
															GetResStr(tLogicalChannel.GetVideoFormat()),
                                                            tLogicalChannel.GetChanVidFPS(),
															tLogicalChannel.GetFlowControl(),
															tLogicalChannel.GetProfileAttrb()==emHpAttrb?"HP":"BP");
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}
				
				memset( achTemp+l, ' ', 255-l );
				l = 117;//86//68;
				l += sprintf( achTemp+l, "T120:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_T120DATA, &tLogicalChannel, TRUE ) )
				{
					l += sprintf( achTemp+l, "%d", tLogicalChannel.GetRcvMediaChannel().GetPort() );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 126;//97//79;
				l += sprintf( achTemp+l, "H224:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_H224DATA, &tLogicalChannel, TRUE ) )
				{
					l += sprintf( achTemp+l, "%d", tLogicalChannel.GetRcvMediaChannel().GetPort() );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

                memset( achTemp+l, ' ', 255-l );
                l = 136;//109//91;
                l += sprintf(achTemp+l, "VidBand:%dK", m_ptMtTable->GetRcvBandWidth(byLoop));                

				sprintf( achTemp+l, "%c", 0 );
				StaticLog("%s\n", achTemp );

				//�����߼�ͨ����Ϣ
				memset( achTemp, ' ', 255 );
				l = sprintf( achTemp, "   RL Aud:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, FALSE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)(%d)(Num:%d)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                    GetMediaStr( tLogicalChannel.GetChannelType() ),
															tLogicalChannel.GetFlowControl(),
															tLogicalChannel.GetAudioTrackNum());
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 33;
				l += sprintf( achTemp+l, "Vid:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tLogicalChannel, FALSE ) )
				{
                    // ���ӷֱ��ʴ�ӡ, zgc, 2008-08-09
					l += sprintf( achTemp+l, "%d(%s)(%s)(%dFps)(%dK)(%s)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                       GetMediaStr( tLogicalChannel.GetChannelType() ),
                                                               GetResStr( tLogicalChannel.GetVideoFormat() ),
															   tLogicalChannel.GetChanVidFPS(),
                                                               tLogicalChannel.GetFlowControl(),
															   tLogicalChannel.GetProfileAttrb()==emHpAttrb?"HP":"BP");
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 75;//70;//50
				l += sprintf( achTemp+l, "H239:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)(%s)(%dFps)<%dK>(%s)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
                                                            GetMediaStr( tLogicalChannel.GetChannelType() ),
															GetResStr(tLogicalChannel.GetVideoFormat()),
                                                            tLogicalChannel.GetChanVidFPS(),
															tLogicalChannel.GetFlowControl(),
															tLogicalChannel.GetProfileAttrb()==emHpAttrb?"HP":"BP");
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 117;//86//68;
				l += sprintf( achTemp+l, "T120:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_T120DATA, &tLogicalChannel, FALSE ) )
				{
					l += sprintf( achTemp+l, "%d", tLogicalChannel.GetRcvMediaChannel().GetPort() );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 126;//97//79;
				l += sprintf( achTemp+l, "H224:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_H224DATA, &tLogicalChannel, FALSE ) )
				{
					l += sprintf( achTemp+l, "%d", tLogicalChannel.GetRcvMediaChannel().GetPort() );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

                memset( achTemp+l, ' ', 255-l );
                l = 136;//109//91;
                l += sprintf(achTemp+l, "VidBand:%dK", m_ptMtTable->GetSndBandWidth(byLoop));     

				sprintf( achTemp+l, "%c", 0 );
				StaticLog("%s\n", achTemp );
			}
            
            if ( byMtId != 0 )
            {
                StaticLog("\nMT.%d's Cap is as follows:\n\n", byMtId );
                TCapSupport tCap;
                if ( m_ptMtTable->GetMtCapSupport(byMtId, &tCap) )
                {
                    tCap.Print();
                }
                TCapSupportEx tCapEx;
                if ( m_ptMtTable->GetMtCapSupportEx(byMtId, &tCapEx))
                {
                    tCapEx.Print();
                }
            }
		}
	}

	if (NULL == m_ptConfOtherMcTable)
	{
		return;
	}

	u16 wMcuIdx;
	//u8 byMcuId = 0,bySecMcuId = 0;
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	memset( &abyMcuId[0],0,sizeof( abyMcuId ) );
	
	for( u16 wLoop1 = 0; wLoop1 < m_tConfAllMtInfo.GetMaxMcuNum(); wLoop1++ )
	{
		wMcuIdx = m_tConfAllMtInfo.GetMtInfo(wLoop1).GetMcuIdx();
		if( !IsValidMcuId( wMcuIdx ))
		{
			continue;
		}


        if ( m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] ) &&
			 byMtId != 0 &&
             abyMcuId[0] != byMtId )
        {
            continue;
        }		

		if( 0 == abyMcuId[0] )
		{
			continue;
		}

		
		if( m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
		{
			StaticLog("�����ն�: McuId-%d IsSupportMultiSpy:%d IsSupportMultiCascade:%d\n", abyMcuId[0],
				IsLocalAndSMcuSupMultSpy( wMcuIdx ), m_ptConfOtherMcTable->IsMcuSupMultCas(wMcuIdx)
				);
		}
		else
		{
			StaticLog("������ֱ���ն�: McuId-%d SMcuId-%d IsSupportMultiSpy:%d IsSupportMultiCascade:%d\n", abyMcuId[0],
				abyMcuId[1],
				IsLocalAndSMcuSupMultSpy( wMcuIdx ),
				 m_ptConfOtherMcTable->IsMcuSupMultCas(wMcuIdx)
				);
		}
		
		TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
		if(ptMcInfo == NULL)
		{
			continue;
		}
		TConfMtInfo *ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr(wMcuIdx); 
		if(ptConfMtInfo == NULL)
		{
			continue;
		}
		for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{	
			TMtExt tMtExt = ptMcInfo->m_atMtExt[byLoop-1];
			if(tMtExt.GetMtId() == 0)
			{
				continue;//�Լ�
			}
			bInConf = ptConfMtInfo->MtInConf( tMtExt.GetMtId() );
			bInJoinedConf = ptConfMtInfo->MtJoinedConf( tMtExt.GetMtId() );

			if(!bInConf)
			{
				continue;
			}

			if( bInJoinedConf )
			{
				sprintf( achTemp, "�� %c", 0 );
			}
			else
			{
				sprintf( achTemp, "�� %c", 0 ); 
			}
	
			if( 0 == abyMcuId[1] )
			{
				StaticLog("%sMcu%dMT%d:0x%x(Dri:%d), MainType: %d SubType: %d Manu: %s, Alias: %s, DialBitRate: %d. \n",
						achTemp, abyMcuId[0], tMtExt.GetMtId(), tMtExt.GetIPAddr(), tMtExt.GetDriId(), 
						tMtExt.GetType(),tMtExt.GetMtType(),
						GetManufactureStr( tMtExt.GetManuId() ), 
						tMtExt.GetAlias(), 
						tMtExt.GetDialBitRate() );

			}
			else
			{
				StaticLog("%sMcu%dMcu%dMT%d:0x%x(Dri:%d), MainType: %d SubType: %d Manu: %s, Alias: %s, DialBitRate: %d. \n",
					achTemp, abyMcuId[0], abyMcuId[1],tMtExt.GetMtId(), tMtExt.GetIPAddr(), tMtExt.GetDriId(), 
					tMtExt.GetType(),tMtExt.GetMtType(),
					GetManufactureStr( tMtExt.GetManuId() ), 
					tMtExt.GetAlias(), 
					tMtExt.GetDialBitRate() );
			}
		}				
	}

    return;
}
/*====================================================================
    ������      ��ShowConfMcu
    ����        ����ӡ��������mcu��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/08/02    4.6         �ܾ���          ����
====================================================================*/
void CMcuVcInst::ShowConfMcu( void )
{
	//m_tConfAllMcuInfo.AddMcu()

	u16 wLoop = 0;
	u8 abyMcuId[MAX_CASCADEDEPTH-1] = {0};
	TConfMcInfo *ptMcInfo = NULL;
	TConfMtInfo *ptConfMtInfo = NULL;
	TMtExt tMtExt;
	TMtAlias tDailAlias,tMtAliasH323id,tMtAliasE164,tMtAliasH320id,tMtAliasH320Alias;

	u8 byLoop = 0;
	while ( wLoop < MAXNUM_CONFSUB_MCU ) 
	{
		m_tConfAllMcuInfo.GetMcuIdByIdx( wLoop, &abyMcuId[0] );
		if( 0 == abyMcuId[0] )
		{
			++wLoop;
			continue;
		}

		ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wLoop);
		if (NULL == ptMcInfo)
		{
			++wLoop;
			continue;
		}

		if( 0 == abyMcuId[1] )
		{
			tMtAliasH323id.SetNull();
			tMtAliasE164.SetNull();
            tMtAliasH320id.SetNull();
            tMtAliasH320Alias.SetNull();
			m_ptMtTable->GetMtAlias( abyMcuId[0], mtAliasTypeH323ID, &tMtAliasH323id );
			m_ptMtTable->GetMtAlias( abyMcuId[0], mtAliasTypeE164, &tMtAliasE164 );
			m_ptMtTable->GetMtAlias( abyMcuId[0], mtAliasTypeH320ID, &tMtAliasH320id );
            m_ptMtTable->GetMtAlias( abyMcuId[0], mtAliasTypeH320Alias, &tMtAliasH320Alias);			
            
            BOOL32 bRet = m_ptMtTable->GetDialAlias(abyMcuId[0], &tDailAlias);
            
            if ( !bRet )
            {
				StaticLog("McuIdx(%d) McuId(%d) ViewMt(%d,%d) LastViewMt(%d,%d) IP:0x%08x 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s\n,",
								wLoop, abyMcuId[0], 
								ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId(), 
								ptMcInfo->m_tLastMMcuViewMt.GetMcuId(), ptMcInfo->m_tLastMMcuViewMt.GetMtId(),
								m_ptMtTable->GetIPAddr( abyMcuId[0] ),
								tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                tMtAliasH320Alias.m_achAlias
								);
            }
            else
            {
                if ( mtAliasTypeTransportAddress == tDailAlias.m_AliasType )
                {
                    StaticLog("McuIdx(%d) McuId(%d) ViewMt(%d,%d) LastViewMt(%d,%d) IP:0x%08x 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s DialAlias.%s@%d\n",
								wLoop, abyMcuId[0], 
								ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId(), 
								ptMcInfo->m_tLastMMcuViewMt.GetMcuId(), ptMcInfo->m_tLastMMcuViewMt.GetMtId(), 
								m_ptMtTable->GetIPAddr( abyMcuId[0] ),
                                tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                tMtAliasH320Alias.m_achAlias,
                                StrOfIP(tDailAlias.m_tTransportAddr.GetIpAddr()), tDailAlias.m_tTransportAddr.GetPort());
                }
                else
                {
					StaticLog("McuIdx(%d) McuId(%d) ViewMt(%d,%d) LastViewMt(%d,%d) IP:0x%08x 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s DialAlias.%s\n,",
								wLoop, abyMcuId[0], 
								ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId(), 
								ptMcInfo->m_tLastMMcuViewMt.GetMcuId(), ptMcInfo->m_tLastMMcuViewMt.GetMtId(), 
								m_ptMtTable->GetIPAddr( abyMcuId[0] ),
                                tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                tMtAliasH320Alias.m_achAlias,
                                tDailAlias.m_achAlias
								);
                }
			}		
		}
		else
		{
			u16 wMcuIdx = INVALID_MCUIDX;
			m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],1,&wMcuIdx );
			if( INVALID_MCUIDX == wMcuIdx )
			{
				++wLoop;
				continue;
			}
			
			ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
			if (NULL == ptMcInfo)
			{
				++wLoop;
				continue;
			}

			ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr(wMcuIdx); 

			m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],2,&wMcuIdx );
			TConfMcInfo *ptSMcuMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);

			if( NULL != ptConfMtInfo)
			{
				for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
				{	
					tMtExt = ptMcInfo->m_atMtExt[byLoop-1];
					if( tMtExt.GetMtId() == abyMcuId[1] )
					{
						if( ptConfMtInfo->MtInConf( tMtExt.GetMtId() ) && NULL != ptSMcuMcInfo  )
						{
							StaticLog("McuIdx(%d) McuId(%d-%d) ViewMt(%d,%d) LastViewMt(%d,%d) IP:0x%08x Alias.%s\n",
								wLoop, abyMcuId[0], abyMcuId[1], 
								ptSMcuMcInfo->m_tMMcuViewMt.GetMcuId(), ptSMcuMcInfo->m_tMMcuViewMt.GetMtId(), 
								ptSMcuMcInfo->m_tLastMMcuViewMt.GetMcuId(), ptSMcuMcInfo->m_tLastMMcuViewMt.GetMtId(), 
								tMtExt.GetIPAddr(), tMtExt.GetAlias()
								);
						}
						
						break;
					}
				}
			}
		}
		
		++wLoop;
	}
}

/*====================================================================
    ������      ��ShowConfMix
    ����        ����ӡ���������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId    = 0
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/11    3.0         ������          ����
====================================================================*/
void CMcuVcInst::ShowConfMix()
{
	char achTemp[255];
	u8 byLoop = 0;
	StaticLog("\n---���� %s ������Ϣ---\n", m_tConf.GetConfName() );
	StaticLog("IsTemplate %d\n", m_tConf.m_tStatus.IsTemplate() );
	StaticLog("InstID %d\n", GetInsID() );
    StaticLog("ConfIdx %d\n", m_byConfIdx );
    m_tConf.GetConfId().GetConfIdString(achTemp, sizeof(achTemp));
    StaticLog("ConfId %s\n", achTemp );

	StaticLog("\n");
	for( u8 byPos = 0; byPos <= MAXNUM_MIXER_CHNNL; byPos++ )
	{
		if (!m_atMixMt[byPos].IsNull())
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Mt(%d, %d) has used MIXER.%d Chnl(%d)\n",
				m_atMixMt[byPos].GetMcuIdx(), m_atMixMt[byPos].GetMtId(), m_tMixEqp.GetMtId(), byPos - 1 );
		}
	}

	static u8 s_achMixMode[][16] = {"NoMix", "WholeMix", "PartMix", "VacMix", "VacWholeMix", "VacPartMix"}; 
	u8 byMixKind = sizeof(s_achMixMode) / 16;
	u8 byMixMode = m_tConf.m_tStatus.GetMixerMode();
	if (byMixMode >= byMixKind)
	{
		StaticLog("ERROR: The Current Mix Mode is %d\n", byMixMode );
		return;
	}

	StaticLog("\n");
	StaticLog("The Current Mix Mode: %s\n", s_achMixMode[byMixMode] );
	if (m_byLastMixMode >= byMixKind)
	{
		StaticLog("ERROR: The Last Mix Mode is %d\n", m_byLastMixMode );
		return;
	}
	StaticLog("The Last Mix Mode: %s\n", s_achMixMode[m_byLastMixMode] );
	StaticLog("The MixMemNum: %d\n", m_tConf.m_tStatus.GetMixerParam().GetMemberNum());
	StaticLog("The MixMaxMemNum: %d\n",m_tConf.m_tStatus.GetMixerParam().GetMtMaxNum());
	
	if (mcuNoMix == byMixMode)
	{
		return;
	}

	TLogicalChannel tLogicalChannel;
	u32	dwVSndIp = 0, dwASndIp = 0, dwVRcvIp = 0,dwARcvIp = 0;
	u16 wVSndPort = 0, wASndPort = 0, wVRcvPort = 0, wARcvPort = 0;
	u8 byFwdChannelNum = 0;
	u8 byRvsChannelNum = 0;
	
	StaticLog("\n");
	StaticLog("��ǰʹ�õĻ�������Ϣ:\n");
	StaticLog("EQPID  SNDAIP          SPORT CHNNUM RCVAIP           RPORT CHNNUM\n");
	StaticLog("------ --------------- ----- ------ --------------- ----- ------\n");

	u8 byIndex = m_tMixEqp.GetEqpId();
	
	if( g_cMcuVcApp.IsPeriEqpConnected( byIndex ) )
	{
		if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byIndex, MODE_AUDIO, &byFwdChannelNum, &tLogicalChannel, TRUE) )
		{
			dwASndIp = tLogicalChannel.m_tSndMediaCtrlChannel.GetIpAddr();
			wASndPort = tLogicalChannel.m_tSndMediaCtrlChannel.GetPort();
		}
		
		if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byIndex, MODE_AUDIO, &byRvsChannelNum, &tLogicalChannel, FALSE) )
		{
			dwARcvIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
			wARcvPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
		}
		StaticLog("%6u %15s %5u %6u %15s %5u %6u\n", byIndex,
			StrOfIP(dwASndIp), wASndPort, byFwdChannelNum, StrOfIP(dwARcvIp), wARcvPort, byRvsChannelNum);
	}

	if (!m_tConf.m_tStatus.IsSpecMixing())
	{
		return;
	}

	StaticLog("\n����������ն��б�:\n");
	TMtStatus tMixMtStatus;
	u32 dwCnt = 0;
	StaticLog(" MTID        MTIP           FL Aud            RL Aud       AutoInMix \n");
	StaticLog("------ --------------- --------------- ------------------- --------- \n");

	for(byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		m_ptMtTable->GetMtStatus(byLoop, &tMixMtStatus);

		if (tMixMtStatus.IsInMixing())
		{
			dwCnt = 0;
			memset( achTemp, 0, 255 );
			dwCnt += (u32)sprintf( achTemp, "%6u %15s ", byLoop, StrOfIP(m_ptMtTable->GetIPAddr(byLoop)));

			//ǰ��ͨ��
			if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )
			{
				dwCnt += (u32)sprintf( achTemp + dwCnt, "%6u(%6s)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
					GetMediaStr( tLogicalChannel.GetChannelType() ) );
			}
			else
			{
				dwCnt += (u32)sprintf( achTemp + dwCnt, "NO" );
			}
			//����ͨ��
			dwCnt += (u32)sprintf( achTemp + dwCnt, " ");
			if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, FALSE ) )
			{
				dwCnt += (u32)sprintf( achTemp + dwCnt, "%6u(%6s)(%3d)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
					GetMediaStr( tLogicalChannel.GetChannelType() ), tLogicalChannel.GetFlowControl());
			}
			else
			{
				dwCnt += (u32)sprintf( achTemp + dwCnt, "NO" );
			}
			dwCnt += (u32)sprintf( achTemp + dwCnt, " %6u", m_ptMtTable->IsMtAutoInSpec(byLoop));
			StaticLog("%s\n", achTemp);
		}
	}

	//ȫ���ն�
	StaticLog("\n����ȫ����ն��б�:\n");
	TMtStatus tNRcvMtStatus;
	u32 dwNCnt = 0;
	StaticLog(" MTID        MTIP           FL Aud            RL Aud       AutoInMix \n");
	StaticLog("------ --------------- --------------- ------------------- --------- \n");
	for(byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		m_ptMtTable->GetMtStatus(byLoop, &tNRcvMtStatus);
		if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && !tNRcvMtStatus.IsInMixing() && m_tConf.m_tStatus.IsMixing())
		{
			dwNCnt = 0;
			memset(achTemp, 0, sizeof(achTemp));
			dwNCnt += (u32)sprintf( achTemp, "%6u %15s ", byLoop, StrOfIP(m_ptMtTable->GetIPAddr(byLoop)));

			//ǰ��ͨ��
			if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )
			{
				dwNCnt += (u32)sprintf( achTemp + dwNCnt, "%6u(%6s)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
					GetMediaStr( tLogicalChannel.GetChannelType() ) );
			}
			else
			{
				dwNCnt += (u32)sprintf( achTemp + dwNCnt, "NO" );
			}
			//����ͨ��
			dwNCnt += (u32)sprintf( achTemp + dwNCnt, " ");
			if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, FALSE ) )
			{
				dwNCnt += (u32)sprintf( achTemp + dwNCnt, "%6u(%6s)(%3d)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
					GetMediaStr( tLogicalChannel.GetChannelType() ), tLogicalChannel.GetFlowControl());
			}
			else
			{
				dwNCnt += (u32)sprintf( achTemp + dwNCnt, "NO" );
			}
			//�Ƿ��Զ�����
			dwNCnt += (u32)sprintf( achTemp + dwNCnt, " %6u", m_ptMtTable->IsMtAutoInSpec(byLoop));
			StaticLog("%s\n", achTemp);
		}
	}
}

/*====================================================================
    ������      ��ShowBasInfo
    ����        ����ӡ����bas��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/09/03    4.5         �ű���          ����
====================================================================*/
void CMcuVcInst::ShowBasInfo( u8 byBasId )
{
	if ( byBasId != 0 )
	{
		if (byBasId > BASID_MAX || byBasId < BASID_MIN)
		{
			StaticLog("Illegal bas id.%d, mishanding ? ...\n", byBasId );
			return;
		}
		if (!g_cMcuVcApp.IsPeriEqpConnected(byBasId))
		{
			StaticLog("Bas.%d is not online yet, try another ...\n", byBasId );
			return;
		}
	}
	PrintBas();
	return;
}


/*====================================================================
    ������      ��ShowConfMod
    ����        ����ӡ����ģ����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/11    3.0         ������          ����
====================================================================*/
void CMcuVcInst::ShowConfMod()
{
	TVmpModuleInfo tVmpModule = m_tConfEqpModule.m_tVmpModuleInfo;

	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
    if( tConfAttrb.IsHasVmpModule() )
	{
		StaticLog("Vmp module info:\n" );
		StaticLog("tVmpModule.m_tVMPParam.m_byVMPAuto = %d\n", tVmpModule.m_byVMPAuto  );
		StaticLog("tVmpModule.m_tVMPParam.m_byVMPBrdst = %d\n", tVmpModule.m_byVMPBrdst  );
		StaticLog("tVmpModule.m_tVMPParam.m_byVMPStyle = %d\n", tVmpModule.m_byVMPStyle );
		for( u8 byLoop = 0; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++ )
		{
			if( m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byLoop] != 0)
			{
				StaticLog("Vmp Chl%d(mt%d)\n", byLoop, m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byLoop] );
			}
		}
	}
    
    TMultiTvWallModule tMultiTvWallModule = m_tConfEqpModule.m_tMultiTvWallModule;
    TTvWallModule tOneModule;
	TConfAttrb tConfattrb = m_tConf.GetConfAttrb();
    if ( tConfattrb.IsHasTvWallModule() )
    {
        StaticLog("Tvwall module info:\n" );
        StaticLog("tMultiTvWallModule.m_byTvModuleNum %d\n", tMultiTvWallModule.GetTvModuleNum());
        for ( u8 byLoop = 0; byLoop < tMultiTvWallModule.GetTvModuleNum(); byLoop++ )
        {
            tMultiTvWallModule.GetTvModuleByIdx(byLoop, tOneModule);
            StaticLog("Tvwall module %d:\n", byLoop );            
            StaticLog("Tvwall EqpId.%d\n", tOneModule.GetTvEqp().GetEqpId() );
            
            for ( u8 byLoop2 = 0; byLoop2 < MAXNUM_PERIEQP_CHNNL; byLoop2 ++ )
            {
                if ( tOneModule.m_abyTvWallMember[byLoop2] == 0 || 
                     tOneModule.m_abyMemberType[byLoop2] == 0 )
                     break;

                StaticLog("\tMember %d: Mt.%d, type: %d\n", 
                           byLoop2,
                           tOneModule.m_abyTvWallMember[byLoop2],
                           tOneModule.m_abyMemberType[byLoop2] );
            }
            
        }
    }

	StaticLog("HDU vmp Chnnl module info:\n" );
	m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.Print();
}

/*====================================================================
    ������      ��ShowMtMonitor
    ����        ����ӡ�ն˼����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/28    3.0         ������          ����
====================================================================*/
void CMcuVcInst::ShowMtMonitor()
{
	char achTemp[255];
	StaticLog("\n---���� %s �ն˼����Ϣ---\n", m_tConf.GetConfName() );
    int l = 0;

	for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
		{
			if( m_ptMtTable->IsMtMulticasting( byLoop ) )
			{
				sprintf( achTemp, "�� %c", 0 );
			}
			else
			{
				sprintf( achTemp, "�� %c", 0 ); 
			}

			StaticLog("MT%d-0x%x multicasting: %s \n",
				       byLoop, m_ptMtTable->GetIPAddr( byLoop ), achTemp );

			u8 byMtList[MAXNUM_MT_CHANNL];
			u8 byMtNum = m_ptMtTable->GetMonitorSrcMtList( byLoop, byMtList );
			if( byMtNum > 0 )
			{
				l = sprintf( achTemp, "SrcMtList: " );
				for( u8 byIndex = 0; byIndex < byMtNum; byIndex++ )
				{
					l = l + sprintf( achTemp+l, " %d", byMtList[byIndex] );
				}
				StaticLog("%s\n", achTemp );
			}
		}
	}
}

/*=============================================================================
    �� �� ���� InviteUnjoinedMt
    ��    �ܣ� ����δ����ն����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CServMsg & cServMsgHdr,	ҵ����Ϣͷ
			   const TMt * ptMt, �����ն�ָ�룬NULL��ʾ�������������ֱ��δ����ն�
               BOOL32 bSendAlert  �Ƿ�����mcu����Alert����֪ͨ
               BOOL32 bBrdSend    �Ƿ�������mcu�㲥��Alert����֪ͨ����ֻ�ǻظ�������mcu
			   u8     byCallType  ��������(��������(VCS_FORCECALL_REQ) ����ǿ������(VCS_FORCECALL_CMD))
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/5/25    3.5			����                  ����
=============================================================================*/
void CMcuVcInst::InviteUnjoinedMt( const CServMsg& cServMsgHdr, const TMt* ptMt, 
								   BOOL32 bSendAlert, BOOL32 bBrdSend, 
								   u8 byCallType/* = VCS_FORCECALL_REQ*/,
								   BOOL bLowLevelMcuCalledIn/* = FALSE*/)
{
	CServMsg	cServMsg;
	TMtAlias	tMtAlias;
	TMtAlias	tConfAlias;

	tConfAlias.SetH323Alias( m_tConf.GetConfName() );

	//send invite message to connected MTs
	cServMsg.SetServMsg( cServMsgHdr.GetServMsg(), SERV_MSGHEAD_LEN );
    cServMsg.SetConfIdx( m_byConfIdx );
    cServMsg.SetConfId( m_tConf.GetConfId() );

	u16 wAudioBand = GetAudioBitrate( m_tConf.GetMainAudioMediaType() );
	u8  byEncrypt  =  (m_tConf.GetConfAttrb().GetEncryptMode() != CONF_ENCRYPTMODE_NONE) ? 1 : 0;

	if( ptMt != NULL )
	{
		//�ж��ն��Ƿ��Ѿ����
		if( m_tConfAllMtInfo.MtJoinedConf( ptMt->GetMtId() ) )
		{
			cServMsg.SetErrorCode( ERR_MCU_ADDEDMT_INCONF );
			
			post( MAKEIID( GetAppID(), GetInsID() ), MT_MCU_INVITEMT_NACK, 
					    cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			return;
		}

        if (!m_ptMtTable->GetDialAlias( ptMt->GetMtId(), &tMtAlias ))
        {
            cServMsg.SetErrorCode( ERR_MCU_CALLEDMT_NOADDRINFO);
            post( MAKEIID( GetAppID(), GetInsID() ), MT_MCU_INVITEMT_NACK, 
                        cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
            
            ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[InviteUnjoinedMt] GetDialAlias failed, byMtId.%d\n", ptMt->GetMtId());
            return;
        }

		//����H320�ն�
		if( mtAliasTypeH320ID == tMtAlias.m_AliasType )
		{
            // xsl [11/8/2006] ����320�����id
            u8 byDriId = m_ptMtTable->GetDriId(ptMt->GetMtId());

            if ( !g_cMcuVcApp.IsMtAdpConnected(byDriId) || !g_cMcuVcApp.IsMtAssignInDri(byDriId, m_byConfIdx, ptMt->GetMtId()) )
            {
                byDriId = g_cMcuVcApp.AssignH320MtDriId(m_byConfIdx, tMtAlias, ptMt->GetMtId());			
			    if( !g_cMcuVcApp.IsMtAdpConnected(byDriId) || 
				    PROTOCOL_TYPE_H320 != g_cMcuVcApp.GetMtAdpProtocalType(byDriId) )
			    {
				    cServMsg.SetDstMtId( ptMt->GetMtId() );
				    cServMsg.SetErrorCode( ERR_MCU_MTUNREACHABLE );
				    SendMsgToAllMcs( MCU_MCS_CALLMT_NACK, cServMsg );
                				    
				    ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[InviteUnjoinedMt] Cannot Get IsMtAdpConnected|GetMtAdpProtocalType For Mt.%d.\n", ptMt->GetMtId() );
				    return;
			    }
                m_ptMtTable->SetDriId( ptMt->GetMtId(), byDriId );
            }            
            m_ptMtTable->SetProtocolType(ptMt->GetMtId(), PROTOCOL_TYPE_H320);
		}
		else
		{            
			m_ptMtTable->SetProtocolType(ptMt->GetMtId(), PROTOCOL_TYPE_H323);

            /*
            //zbq [09/19/2007] �������߼������� DialAlias ����

			//�����ն˱���ѯ�ն˱���
			if( m_tConf.m_tStatus.IsRegToGK() )
			{
				if( ptMt->GetMtType() == MT_TYPE_SMCU|| ptMt->GetMtType() == MT_TYPE_MMCU)
				{
					if( !m_ptMtTable->GetMtAlias( ptMt->GetMtId(), mtAliasTypeE164, &tMtAlias ) && 
						!m_ptMtTable->GetDialAlias( ptMt->GetMtId(), &tMtAlias )  )
					{
						cServMsg.SetErrorCode( ERR_MCU_CALLEDMT_NOADDRINFO);
						post( MAKEIID( GetAppID(), GetInsID() ), MT_MCU_INVITEMT_NACK, 
							cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
						
						ConfLog( FALSE, "[InviteUnjoinedMt] Cannot Get E164MtAlias|DialAlias For Mt.%d.\n", ptMt->GetMtId() );
						return;
					}
				}
				else
				{
					if( !m_ptMtTable->GetDialAlias( ptMt->GetMtId(), &tMtAlias ) )
					{
						cServMsg.SetErrorCode( ERR_MCU_CALLEDMT_NOADDRINFO);
						post( MAKEIID( GetAppID(), GetInsID() ), MT_MCU_INVITEMT_NACK, 
							cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
						
						ConfLog( FALSE, "[InviteUnjoinedMt] Cannot Get DialAlias For Mt.%d.\n", ptMt->GetMtId() );
						return;
					}
				}
			}
			else
			{
				if( !m_ptMtTable->GetMtAlias( ptMt->GetMtId(), mtAliasTypeTransportAddress, &tMtAlias) )
				{
					cServMsg.SetErrorCode( ERR_MCU_CALLEDMT_NOADDRINFO);
					post( MAKEIID( GetAppID(), GetInsID() ), MT_MCU_INVITEMT_NACK, 
						cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
					
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[InviteUnjoinedMt] Cannot Get IP For Mt.%d.\n", ptMt->GetMtId() );
					return;
				}			
			}
            */

            // xsl [11/8/2006] ����323�����id            
            u8 byDriId = m_ptMtTable->GetDriId(ptMt->GetMtId());   
            u8 byMtNum = 0;
            u8 byMcuNum = 0;
            if ( !g_cMcuVcApp.IsMtAdpConnected(byDriId) || 
                 !g_cMcuVcApp.IsMtAssignInDri(byDriId, m_byConfIdx, ptMt->GetMtId()) ||
                 g_cMcuVcApp.GetMtNumOnDri(byDriId, FALSE, byMtNum, byMcuNum)  // guzh [1/18/2007] �����DRI�Ѿ����������·���
               )
            {
                byDriId = g_cMcuVcApp.AssignH323MtDriId(m_byConfIdx, tMtAlias, ptMt->GetMtId());
                if ( !g_cMcuVcApp.IsMtAdpConnected(byDriId) )
                {
                    ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[InviteUnjoinedMt] DriId.%d for MT.%d is OFFLine.\n", byDriId, ptMt->GetMtId());
                    g_cMcuVcApp.DecMtAdpMtNum(byDriId, m_byConfIdx, ptMt->GetMtId());
                    
                    if ( byDriId > MAXNUM_DRI || 0 == byDriId )
                    {
                        // guzh [3/1/2007] ������Ҳ������Է���Ŀ���DRI������ʾMCS������������
                        cServMsg.SetMsgBody( (u8*)ptMt, sizeof(TMt) );                        
                        cServMsg.SetErrorCode( ERR_MCU_OVERMAXCONNMT );
                        SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg ); 
                    }
                    return;
                }            
                m_ptMtTable->SetDriId(ptMt->GetMtId(), byDriId);
            }            
		}		

		//�����ն˻��ϼ�MCU
		// xliang [12/26/2008] modify��for MT call MCU initially. 
		if(ptMt->GetMtId()  == m_byMtIdNotInvite ||
		   bLowLevelMcuCalledIn) // xliang [8/29/2008] �����������)
		{
			m_ptMtTable->SetNotInvited( ptMt->GetMtId(), TRUE );
		}
		else
		{
			m_ptMtTable->SetNotInvited( ptMt->GetMtId(), FALSE );
		}
	
		cServMsg.SetMsgBody( (u8*)ptMt, sizeof( TMt ) );
		LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[InviteUnjoinedMt]: mt.%d aliasType.%d\n", ptMt->GetMtId(), tMtAlias.m_AliasType);
		cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
		cServMsg.CatMsgBody( (u8*)&tConfAlias, sizeof( tConfAlias ) );	
		cServMsg.CatMsgBody( (u8*)&byEncrypt, sizeof(byEncrypt));
		//u16 wCallBand = htons( m_ptMtTable->GetDialBitrate( ptMt->GetMtId() ) + wAudioBand );
		u16 wCallBand = htons( m_ptMtTable->GetDialBitrate( ptMt->GetMtId() ) );
		cServMsg.CatMsgBody( (u8*)&wCallBand, sizeof(u16)  );
		
        TCapSupport tCapSupport = m_tConf.GetCapSupport();
		TSimCapSet tSim = tCapSupport.GetMainSimCapSet();
        // guzh [6/6/2007] ���������
        if (tSim.GetVideoMaxBitRate() == 0)
        {
            tSim.SetVideoMaxBitRate(m_tConf.GetBitRate());
            tCapSupport.SetMainSimCapSet(tSim);
        }
        tSim = tCapSupport.GetSecondSimCapSet();
        if (!tSim.IsNull() && tSim.GetVideoMaxBitRate() == 0)
        {
            tSim.SetVideoMaxBitRate(m_tConf.GetBitRate());
            tCapSupport.SetSecondSimCapSet(tSim);
        }		
        tCapSupport.SetDStreamMaxBitRate(m_ptMtTable->GetDialBitrate( ptMt->GetMtId()));
       
		cServMsg.CatMsgBody( (u8*)&tCapSupport, sizeof(tCapSupport) );
		//ǿ�� 
		u16 wForceCallInfo = MAKEWORD(m_tConf.GetConfLevel(), byCallType);
		wForceCallInfo = htons(wForceCallInfo);
		cServMsg.CatMsgBody( (u8*)&wForceCallInfo, sizeof(u16));


		u8 byAdminLevel = g_cMcuVcApp.GetCascadeAdminLevel();
		//vcs���鲻��֪��������,��Զ����ߵȼ�ȥ�����ն�
		if( VCS_CONF == m_tConf.GetConfSource() || !m_tConf.GetConfAttrb().IsSupportCascade() )
		{
			byAdminLevel = MAX_ADMINLEVEL;
		}
		cServMsg.CatMsgBody( (u8*)&byAdminLevel, sizeof(u8));

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
		//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
		//u8 byAudioTrackNum = 1;
		//cServMsg.CatMsgBody(&byAudioTrackNum,sizeof(byAudioTrackNum));

		TMtAlias tConfE164Alias;
		tConfE164Alias.SetE164Alias( m_tConf.GetConfE164() );
		cServMsg.CatMsgBody( (u8 *)&tConfE164Alias, sizeof(tConfE164Alias) );

		u8 byNPlusSmcuAliasFlag = 0; //FlagΪ1����ʶN+1�����¼��������������E164�ţ���
		//ʵ����IP����.[11/16/2012 chendaiwei]
		if(g_cMcuAgent.GetGkIpAddr()==0 
			&& m_byCreateBy == CONF_CREATE_NPLUS
			&& tMtAlias.m_AliasType == mtAliasTypeE164
			&& !tMtAlias.m_tTransportAddr.IsNull())
		{
			byNPlusSmcuAliasFlag = 1;
		}
		cServMsg.CatMsgBody( (u8 *)&byNPlusSmcuAliasFlag, sizeof(byNPlusSmcuAliasFlag) );

		TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//��Ƶ����
		//�ӻ���������ȡ������֧�ֵ���Ƶ����
		m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
		cServMsg.CatMsgBody((u8*)&atAudioTypeDesc[0], sizeof(TAudioTypeDesc)* MAXNUM_CONF_AUDIOTYPE);
		SendMsgToMt( ptMt->GetMtId(), MCU_MT_INVITEMT_REQ, cServMsg );

		if( TRUE == bSendAlert )
		{
			TMsgHeadMsg tHeadMsg;			
			
			// [12/8/2011 liuxu] ��ֹ�޸�cServMsgHdr
			CServMsg cTempMsg;
			cTempMsg.SetServMsg( cServMsgHdr.GetServMsg(), SERV_MSGHEAD_LEN );
			cTempMsg.SetConfIdx( m_byConfIdx );
			cTempMsg.SetConfId( m_tConf.GetConfId() );

			TMt tInviteMt = *ptMt;
			tInviteMt.SetMcuId( LOCAL_MCUID );
			cTempMsg.SetMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
			cTempMsg.CatMsgBody( (u8*)&tInviteMt, sizeof( TMt ) );
			cTempMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
			if( bBrdSend )
			{
				BroadcastToAllMcu( MCU_MCU_CALLALERTING_NOTIF, cTempMsg );
			}
			else
			{
				SendReplyBack( cTempMsg, MCU_MCU_CALLALERTING_NOTIF );
			}
		}
	}
	//���������նˣ����ڴ���ɹ��������նˣ������ն˺���ģʽ������֤����һ��
	//fxh ��VCSֻ����һ�κ���ģʽΪ��ʱ���е��ն�
	else
	{
		for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			TMt tInviteMt;
			if( !m_tConfAllMtInfo.MtJoinedConf( byLoop ) && m_tConfAllMtInfo.MtInConf( byLoop ) )
			{
				tInviteMt = m_ptMtTable->GetMt( byLoop );	
				if (!m_ptMtTable->GetDialAlias( tInviteMt.GetMtId(), &tMtAlias ))
                {
                    continue;
                }
  
                // guzh [5/23/2007] ����ն˴�������ȥ�������Լ�
                if( CONF_CREATE_MT == m_byCreateBy && byLoop == 1)
                {
					// xliang [1/16/2009]  �����ն˴˴�������������Ҫ����NotInvited��־λ��
					// �������������Ч
					if( byLoop == m_byMtIdNotInvite)
					{
						m_ptMtTable->SetNotInvited( byLoop, TRUE );
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "Mt.%u is not invited!\n",m_byMtIdNotInvite);
					}
					else
					{
						m_ptMtTable->SetNotInvited( byLoop, FALSE );
					}
					continue;  
					
                }

				// fxh ����VCS�����������У�ֻ���к���ģʽΪ��ʱ���е��ն�
				// �����ֶ�����ʵ�壬�������Զ�����
				if (VCS_CONF == m_tConf.GetConfSource() 
					&& m_ptMtTable->GetCallMode(byLoop) != CONF_CALLMODE_TIMER)
				{
					continue;
				}
				

				//����H320�ն�
				if(mtAliasTypeH320ID == tMtAlias.m_AliasType )
				{
                    // xsl [11/8/2006] ����320�����id
                    u8 byDri = m_ptMtTable->GetDriId(tInviteMt.GetMtId());
                    if (!g_cMcuVcApp.IsMtAdpConnected(byDri) || !g_cMcuVcApp.IsMtAssignInDri(byDri, m_byConfIdx, tInviteMt.GetMtId()))
                    {
                        byDri = g_cMcuVcApp.AssignH320MtDriId(m_byConfIdx, tMtAlias, tInviteMt.GetMtId());
					    if( !g_cMcuVcApp.IsMtAdpConnected(byDri) || 
						    PROTOCOL_TYPE_H320 != g_cMcuVcApp.GetMtAdpProtocalType(byDri) )
					    {
						    cServMsg.SetDstMtId( tInviteMt.GetMtId() );
						    cServMsg.SetErrorCode( ERR_MCU_MTUNREACHABLE );
						    SendMsgToAllMcs( MCU_MCS_CALLMT_NACK, cServMsg );
						    continue;
					    }
                        m_ptMtTable->SetDriId(tInviteMt.GetMtId(), byDri);
                    }					
                    m_ptMtTable->SetProtocolType(tInviteMt.GetMtId(), PROTOCOL_TYPE_H320);
				}
				else
				{                  
					//��ȡ�ն˶�Ӧ����
					if( m_tConf.m_tStatus.IsRegToGK() )
					{
						if( tInviteMt.GetMtType() == MT_TYPE_SMCU|| tInviteMt.GetMtType() == MT_TYPE_MMCU)
						{
							if( !m_ptMtTable->GetMtAlias( tInviteMt.GetMtId(), mtAliasTypeE164, &tMtAlias ) && 
								!m_ptMtTable->GetDialAlias( tInviteMt.GetMtId(), &tMtAlias )  )
							{
								continue;
							}
						}
						else
						{
							if( !m_ptMtTable->GetDialAlias( tInviteMt.GetMtId(), &tMtAlias ) )
							{
								continue;
							}
						}
					}
					else
					{
						if( m_byCreateBy == CONF_CREATE_NPLUS)
						{
							if(!m_ptMtTable->GetDialAlias( tInviteMt.GetMtId(), &tMtAlias ) )
							{
								continue;
							}
						}
						else if( !m_ptMtTable->GetMtAlias( tInviteMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias ) )
						{
							continue;
						}
					}

                    // xsl [11/8/2006] ����323�����id
                    u8 byDri = m_ptMtTable->GetDriId(tInviteMt.GetMtId());
                    if ( !g_cMcuVcApp.IsMtAdpConnected(byDri) || !g_cMcuVcApp.IsMtAssignInDri(byDri, m_byConfIdx, tInviteMt.GetMtId()) )
                    {
                        byDri = g_cMcuVcApp.AssignH323MtDriId(m_byConfIdx, tMtAlias, tInviteMt.GetMtId());
                        if (!g_cMcuVcApp.IsMtAdpConnected(byDri))
                        {
                            g_cMcuVcApp.DecMtAdpMtNum(byDri, m_byConfIdx, tInviteMt.GetMtId());

                            if (byDri > MAXNUM_DRI || byDri == 0)
                            {
                                // guzh [3/1/2007] һ���Ժ���ʧ��ʱ��ʱ��������ʾ
                            }
                            continue;
                        }
                        m_ptMtTable->SetDriId(tInviteMt.GetMtId(), byDri);
                    }                        
                    m_ptMtTable->SetProtocolType(tInviteMt.GetMtId(), PROTOCOL_TYPE_H323);
				}

				cServMsg.SetConfIdx( m_byConfIdx );

				//������Ϣ���ն�����Ự
				// xliang [12/26/2008] modify��for MT call MCU initially. 
				if( tInviteMt.GetMtId() == m_byMtIdNotInvite)
				{
					m_ptMtTable->SetNotInvited( tInviteMt.GetMtId(), TRUE );
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "Mt.%u is not invited!\n",m_byMtIdNotInvite);
				}
				else
				{
					m_ptMtTable->SetNotInvited( tInviteMt.GetMtId(), FALSE );
				}
			
				cServMsg.SetMsgBody( (u8*)&tInviteMt, sizeof( tInviteMt ) );
				LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[InviteUnjoinedMt]: mt.%d aliasType.%d\n", tInviteMt.GetMtId(), tMtAlias.m_AliasType);
				cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
				cServMsg.CatMsgBody( (u8*)&tConfAlias, sizeof( tConfAlias ) );
				cServMsg.CatMsgBody( (u8*)&byEncrypt, sizeof(byEncrypt));
				//u16 wCallBand = htons( m_ptMtTable->GetDialBitrate( tInviteMt.GetMtId() ) + wAudioBand );
				u16 wCallBand = htons( m_ptMtTable->GetDialBitrate( tInviteMt.GetMtId() ) );
				cServMsg.CatMsgBody( (u8*)&wCallBand, sizeof(u16)  );
				
                TCapSupport tCapSupport = m_tConf.GetCapSupport();
                TSimCapSet tSim = tCapSupport.GetMainSimCapSet();
                tSim.SetVideoMaxBitRate(m_ptMtTable->GetDialBitrate( tInviteMt.GetMtId()));
                tCapSupport.SetMainSimCapSet(tSim);
                tSim = tCapSupport.GetSecondSimCapSet();
                if(!tSim.IsNull())
                {
                    tSim.SetVideoMaxBitRate(m_ptMtTable->GetDialBitrate( tInviteMt.GetMtId()));
                    tCapSupport.SetSecondSimCapSet(tSim);
                }		
                tCapSupport.SetDStreamMaxBitRate(m_ptMtTable->GetDialBitrate( tInviteMt.GetMtId()));

                cServMsg.CatMsgBody( (u8*)&tCapSupport, sizeof(tCapSupport));

				//ǿ�� 
				u16 wForceCallInfo = MAKEWORD(m_tConf.GetConfLevel(), byCallType);
				wForceCallInfo = htons(wForceCallInfo);
				cServMsg.CatMsgBody( (u8*)&wForceCallInfo, sizeof(u16));

				u8 byAdminLevel = g_cMcuVcApp.GetCascadeAdminLevel();
				//vcs���鲻��֪��������,��Զ����ߵȼ�ȥ�����ն�
				if( VCS_CONF == m_tConf.GetConfSource() || !m_tConf.GetConfAttrb().IsSupportCascade() )
				{
					byAdminLevel = MAX_ADMINLEVEL;
				}
				cServMsg.CatMsgBody( (u8*)&byAdminLevel, sizeof(u8));
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

				//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
				//u8 byAudioTrackNum = 1;
				//cServMsg.CatMsgBody(&byAudioTrackNum,sizeof(byAudioTrackNum));

				TMtAlias tConfE164Alias;
				tConfE164Alias.SetE164Alias( m_tConf.GetConfE164() );
				cServMsg.CatMsgBody( (u8 *)&tConfE164Alias, sizeof(tConfE164Alias) );
				
				u8 byNPlusSmcuAliasFlag = 0; //FlagΪ1����ʶN+1�����¼��������������E164�ţ���
				//ʵ����IP����.[11/16/2012 chendaiwei]
				if(g_cMcuAgent.GetGkIpAddr()==0 
					&& m_byCreateBy == CONF_CREATE_NPLUS
					&& tMtAlias.m_AliasType == mtAliasTypeE164
					&& !tMtAlias.m_tTransportAddr.IsNull())
				{
					byNPlusSmcuAliasFlag = 1;
				}
				cServMsg.CatMsgBody((u8*)&byNPlusSmcuAliasFlag,sizeof(byNPlusSmcuAliasFlag));
				TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//��Ƶ����
				m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
				cServMsg.CatMsgBody((u8*)&atAudioTypeDesc[0], sizeof(TAudioTypeDesc)* MAXNUM_CONF_AUDIOTYPE);
				SendMsgToMt( tInviteMt.GetMtId(), MCU_MT_INVITEMT_REQ, cServMsg );					
			}
		}
	}
	
	g_cMcuVcApp.UpdateAgentAuthMtNum();
}

/*====================================================================
    ������      ��ProcBuildCaseSpecialMessage
    ����        ������ϵͳ������Ϣ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    2.1         ������         ����
====================================================================*/
void CMcuVcInst::ProcBuildCaseSpecialMessage(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TSwitchInfo tSwitchInfo;
    TMt tSrcMt,tDstMt;
    TMt tMediaSrcMt;
    u32  dwMultiCastAddr,dwRcvIp;
    u16  wMultiCastPort, wRcvPort;
    TTransportAddr tVidAddr, tAudAddr;

    TMediaEncrypt  tEncrypt = m_tConf.GetMediaKey();
    TSimCapSet     tSrcSCS; 
    TDoublePayload tDVPayload;
    TDoublePayload tDAPayload;

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

    //TMt(���鲥���ն�)+TTransportAddr(��Ƶ���鲥��ַ)+TTransportAddr(��Ƶ���鲥��ַ)
    switch(CurState())
    {
    case STATE_ONGOING:
        switch(pcMsg->event)
        {
        case MT_MCU_STARTBROADCASTMT_REQ://��MCU�鲥�����ն˵�����

            tSrcMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
            tDstMt = *(TMt *)(cServMsg.GetMsgBody());
            m_ptMtTable->GetMtSwitchAddr(tSrcMt.GetMtId(), dwRcvIp, wRcvPort);

            // �õ�MCU���鲥��ַ
            // ����@2006.4.6 ����Ҫ�ӻ�������ȡ
            // dwMultiCastAddr = g_cMcuAgent.GetCastIpAddr();
            // wMultiCastPort = g_cMcuAgent.GetCastPort();
            dwMultiCastAddr = g_cMcuVcApp.AssignMulticastIp(m_byConfIdx);
            wMultiCastPort = g_cMcuVcApp.AssignMulticastPort(m_byConfIdx, tDstMt.GetMtId());

            tVidAddr.SetNetSeqIpAddr(dwMultiCastAddr);
            tVidAddr.SetPort(wMultiCastPort);
            tAudAddr.SetNetSeqIpAddr(dwMultiCastAddr);
            tAudAddr.SetPort(wMultiCastPort + 2);

            tSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());
            if (MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType() || 
                MEDIA_TYPE_H263PLUS == tSrcSCS.GetVideoMediaType() || 
                CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] ��ӦFEC֧��������ʽ
                m_tConf.GetCapSupportEx().IsVideoSupportFEC())
            {
                tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType());
                tDVPayload.SetActivePayload(GetActivePayload(m_tConf, tSrcSCS.GetVideoMediaType()));
            }
            else
            {
                tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType());
                tDVPayload.SetActivePayload(tSrcSCS.GetVideoMediaType());
            }
            if(CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
               // zbq [10/29/2007] ��ӦFEC֧��������ʽ
               m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
            {
                tDAPayload.SetRealPayLoad(tSrcSCS.GetAudioMediaType());
                tDAPayload.SetActivePayload(GetActivePayload(m_tConf, tSrcSCS.GetAudioMediaType()));
            }
            else
            {
                tDAPayload.SetRealPayLoad(tSrcSCS.GetAudioMediaType());
                tDAPayload.SetActivePayload(tSrcSCS.GetAudioMediaType());
            }

            cServMsg.CatMsgBody((u8 *)&tVidAddr, sizeof(tVidAddr));
            cServMsg.CatMsgBody((u8 *)&tAudAddr, sizeof(tAudAddr));
            cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));
            cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload));
            cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));
            cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload));
            SendReplyBack(cServMsg, pcMsg->event+1);

            //��¼
            m_ptMtTable->AddMonitorSrcMt(tSrcMt.GetMtId(), tDstMt.GetMtId());
            m_ptMtTable->AddMonitorDstMt(tDstMt.GetMtId(), tSrcMt.GetMtId());

            //��ʼ�鲥	
            if (!m_ptMtTable->IsMtMulticasting(tDstMt.GetMtId()))
            {
                //֪ͨ�ն˿�ʼ����
                if (tDstMt.GetType() == TYPE_MT)
                {
                    NotifyMtSend(tDstMt.GetMtId());
                    NotifyFastUpdate(tDstMt, MODE_VIDEO);
                }
                //case �������ն����ֶ���鲥�˿�,��caseֻ��һ���鲥��ַ
                g_cMpManager.StartMulticast(tDstMt, 0, MODE_BOTH, FALSE);
                m_ptMtTable->SetMtMulticasting(tDstMt.GetMtId());
                //����@2006.4.12 �߷���Ŀ��֧�������鲥
            }
            break;

        case MT_MCU_STOPBROADCASTMT_CMD://��MCUֹͣ�鲥�����ն˵�����

            tSrcMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
            tDstMt = *(TMt*)( cServMsg.GetMsgBody() );

            //��¼
            m_ptMtTable->RemoveMonitorSrcMt( tSrcMt.GetMtId(), tDstMt.GetMtId() );
            m_ptMtTable->RemoveMonitorDstMt( tDstMt.GetMtId(), tSrcMt.GetMtId() );

            if( !m_ptMtTable->HasMonitorDstMt( tDstMt.GetMtId() ) )
            {
                g_cMpManager.StopMulticast( tDstMt, 0, MODE_BOTH, FALSE );	
                //����@2006.4.13 �߷���Ŀ��֧�������鲥
                m_ptMtTable->SetMtMulticasting( tDstMt.GetMtId(), FALSE );
            }
            break;

        case MT_MCU_STARTMTSELME_REQ://�������ն�ѡ���Լ�������

            tSwitchInfo = *(TSwitchInfo *)cServMsg.GetMsgBody();
            tSrcMt = tSwitchInfo.GetSrcMt();
            tDstMt = tSwitchInfo.GetDstMt();

            m_ptMtTable->GetMtSrc(tDstMt.GetMtId(), &tMediaSrcMt, MODE_VIDEO);

            if (0 == tMediaSrcMt.GetMtId())
            {
                //֪ͨ�ն˿�ʼ����
                if (tSrcMt.IsLocal() && tSrcMt.GetType() == TYPE_MT)
                {
                    NotifyMtSend(tSrcMt.GetMtId());
                }
					
				//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
                //StartSwitchToSubMt(tSrcMt, 0, tDstMt.GetMtId(), MODE_BOTH, SWITCH_MODE_SELECT);
				TSwitchGrp tSwitchGrp;
				tSwitchGrp.SetSrcChnl(0);
				tSwitchGrp.SetDstMtNum(1);
				tSwitchGrp.SetDstMt(&tDstMt);
				StartSwitchToAll(tSrcMt, 1, &tSwitchGrp, MODE_BOTH, SWITCH_MODE_SELECT);

                //ACK				 
                SendReplyBack(cServMsg, pcMsg->event+1);
            }
            else
            {
                //NACK
                SendReplyBack(cServMsg, pcMsg->event+2);
            }
            break;

        case MT_MCU_GETMTSELSTUTS_REQ://�õ������ն�ѡ�����������

            tDstMt = *(TMt *)(cServMsg.GetMsgBody()+sizeof(TMt));

            m_ptMtTable->GetMtSrc(tDstMt.GetMtId(), &tSrcMt, MODE_VIDEO);
            cServMsg.CatMsgBody((u8*)&tSrcMt, sizeof(TMt));

            //ACK				 
            SendReplyBack(cServMsg, pcMsg->event+1);

            break;

		default:
			break;
        }
        break;
    default:
        break;
    }
}

/*====================================================================
    ������      : ProcMtMcuGetChairmanReq
    ����        ����ѯ��������ϯ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/18    3.0         JQL           ����
====================================================================*/
void CMcuVcInst::ProcMtMcuGetChairmanReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt      tMt;
	u16      wEvent;

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		if( m_tConf.HasChairman() )
		{
			//������ϯ�ն������Ϣ
			TMtAlias tMtAlias;
			tMt.SetMt( m_tConf.GetChairman().GetMcuId(), m_tConf.GetChairman().GetMtId() );
			if( !m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias ) )
			{
				if(!m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320Alias, &tMtAlias ))
                {
                    m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias );
                }
			}
			
			cServMsg.SetMsgBody( ( u8* )&tMt, sizeof( tMt ) );
			cServMsg.CatMsgBody( ( u8* )&tMtAlias, sizeof( tMtAlias ) );
			
			wEvent = pcMsg->event + 1;		
		}
		else
			wEvent = pcMsg->event + 2;

		//�ظ���Ӧ
		SendReplyBack( cServMsg, wEvent );
		break;
		
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
	}
}

/*====================================================================
    ������      : ProcMcuMtFastUpdatePic
    ����        �����ٸ���ͼ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/13    ������        JQL           ����
====================================================================*/
// void CMcuVcInst::ProcMcuMtFastUpdatePic( const CMessage * pcMsg )
// {
//     //CServMsg cServMsg( pcMsg->content, pcMsg->length );
//     return;
// }

/*====================================================================
    ������      ��ProcMtMcuVidAliasNtf
    ����        ���ն���ƵԴ����֪ͨ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	201104112   4.6         pengjie		   create
====================================================================*/
void CMcuVcInst::ProcMtMcuVidAliasNtf( const CMessage * pcMsg )
{
	STATECHECK;
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	u8 byMtId = cServMsg.GetSrcMtId();
	u8 byMtVidPortNum = *(u8 *)(cServMsg.GetMsgBody());
	u8 *pbyVidInfo = (u8 *)(cServMsg.GetMsgBody() + sizeof(u8) );

	if( !m_tConfAllMtInfo.MtJoinedConf( byMtId ) )
	{
		return;
	}

	CServMsg cSendtoMcsMsg;
	u8 byMtNum = 1;
	cSendtoMcsMsg.SetMsgBody( (u8 *)&byMtNum, sizeof(u8) );

	TMt tMt = m_ptMtTable->GetMt( byMtId );
	cSendtoMcsMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );

	TVidSrcAliasInfo atVidSrcAliasInfo[MT_MAXNUM_VIDSOURCE];
	byMtVidPortNum = min(byMtVidPortNum, MT_MAXNUM_VIDSOURCE);
	cSendtoMcsMsg.CatMsgBody( (u8 *)&byMtVidPortNum, sizeof(u8) );

	for( u8 byLoop = 0; byLoop < byMtVidPortNum; byLoop++ )
	{
		// 1����ȡ��ƵԴ�˿�����
		atVidSrcAliasInfo[byLoop].byVidPortIdx = *pbyVidInfo;
		cSendtoMcsMsg.CatMsgBody( (u8 *)&(atVidSrcAliasInfo[byLoop].byVidPortIdx), sizeof(u8) );

		// 2����ȡ��������
		u8 byVidAliasLen = *(pbyVidInfo + sizeof(u8));
		cSendtoMcsMsg.CatMsgBody( (u8 *)&byVidAliasLen, sizeof(u8) );

		// 3����ȡ�����ֶ�
		memcpy( atVidSrcAliasInfo[byLoop].achVidAlias, (s8 *)(pbyVidInfo + sizeof(u8) * 2), byVidAliasLen );
		cSendtoMcsMsg.CatMsgBody( (u8 *)atVidSrcAliasInfo[byLoop].achVidAlias, byVidAliasLen );

		// 4��ƫ�Ƶ���һ����ƵԴ������Ϣ��ַ
		pbyVidInfo = pbyVidInfo + (sizeof(u8) * 2) + byVidAliasLen;
	}

	if( byMtVidPortNum != 0 )
	{
		// �洢
		m_ptMtTable->SetMtVidAlias( byMtId, byMtVidPortNum, atVidSrcAliasInfo );
		// ֪ͨ����
        SendMsgToAllMcs( MCU_MCS_MTVIDEOALIAS_NOTIF, cSendtoMcsMsg );
	}

	return;
}

/*====================================================================
    ������      ��ProcMcsMcuGetMtVidAliaseReq
    ����        ��mcs�����ն���ƵԴ������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	201104112   4.6         pengjie		   create
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetMtVidAliaseReq( const CMessage * pcMsg )
{
	STATECHECK;
	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	u8 byMtNum = *(u8 *)(cServMsg.GetMsgBody());
	TMt *ptMt = (TMt *)(cServMsg.GetMsgBody() + sizeof(u8) );

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	CServMsg cSendtoMcsMsg;
	cSendtoMcsMsg.SetMsgBody( (u8 *)&byMtNum, sizeof(u8) );
	
	// 1����֯�ն���ƵԴ������Ϣ
	u8 byOnlinMtNum = 0;
	if( byMtNum == 0 ) // 1.1 ���������ն�
	{
		for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
		{
			if( m_tConfAllMtInfo.MtJoinedConf( byMtId ) ) // �����ն�
			{
				//SendMsgToMt( byMtId, MCU_MT_GETMTVIDEOALIAS_CMD, cSendMsg );
				TMt tMt = m_ptMtTable->GetMt( byMtId );
				cSendtoMcsMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );
				u8 byMtVidNum = m_ptMtTable->GetMtVidPortNum( byMtId );
				cSendtoMcsMsg.CatMsgBody( (u8 *)&byMtVidNum, sizeof(u8) );

				for( u8 byLoop = 0; byLoop < byMtVidNum; byLoop++ )
				{
					TVidSrcAliasInfo tVidAliasInfo = m_ptMtTable->GetMtVidAliasbyVidIdx( byMtId, byLoop );
					cSendtoMcsMsg.CatMsgBody( (u8 *)&tVidAliasInfo.byVidPortIdx, sizeof(u8) );

					const u8 byAcutualLen = strlen(tVidAliasInfo.achVidAlias);
					u8 byAliasLen = min( byAcutualLen, MT_MAX_PORTNAME_LEN );
					cSendtoMcsMsg.CatMsgBody( (u8 *)&byAliasLen, sizeof(byAliasLen) );
					cSendtoMcsMsg.CatMsgBody( (u8 *)tVidAliasInfo.achVidAlias, byAliasLen );
				}
				byOnlinMtNum++;
			}
		}

	}
	else       // 1.2 ���󲿷��ն�
	{
		for( u8 byLoop = 0; byLoop < byMtNum; byLoop++ )
		{
			if( ptMt != NULL && !ptMt->IsNull() && m_tConfAllMtInfo.MtJoinedConf( *ptMt ) )
			{
				if( ptMt->IsLocal() )
				{
					//SendMsgToMt( ptMt->GetMtId(), MCU_MT_GETMTVIDEOALIAS_CMD, cSendMsg );
					cSendtoMcsMsg.CatMsgBody( (u8 *)ptMt, sizeof(TMt) );
					u8 byMtVidNum = m_ptMtTable->GetMtVidPortNum( ptMt->GetMtId() );
					cSendtoMcsMsg.CatMsgBody( (u8 *)&byMtVidNum, sizeof(u8) );
					
					for( u8 byLoop2 = 0; byLoop2 < byMtVidNum; byLoop2++ )
					{
						TVidSrcAliasInfo tVidAliasInfo = m_ptMtTable->GetMtVidAliasbyVidIdx( ptMt->GetMtId(), byLoop2 );
						cSendtoMcsMsg.CatMsgBody( (u8 *)&tVidAliasInfo.byVidPortIdx, sizeof(u8) );

						const u8 byRealLen = strlen(tVidAliasInfo.achVidAlias);
						u8 byAliasLen = min( byRealLen, MT_MAX_PORTNAME_LEN );
						cSendtoMcsMsg.CatMsgBody( (u8 *)&byAliasLen, sizeof(byAliasLen) );
						cSendtoMcsMsg.CatMsgBody( (u8 *)tVidAliasInfo.achVidAlias, byAliasLen );
					}
				}
				else //���������¼��ն�
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,"[ProcMcsMcuGetMtVidAliaseReq] Get mcuid.%dmtid%d VidSrcAlias !\n", \
						ptMt->GetMcuId(), ptMt->GetMtId() );
				}
				byOnlinMtNum++;
			}
			
			if(ptMt) ptMt++;
		}
	}

	// 2���ϱ�mcs
	if( byOnlinMtNum != 0 ) //����Ϊʵ���ն�����
	{
		u8 *pbyMtNum = cSendtoMcsMsg.GetMsgBody();
		*pbyMtNum = byOnlinMtNum;
		SendMsgToAllMcs( MCU_MCS_MTVIDEOALIAS_NOTIF, cSendtoMcsMsg );
	}

	return;
}

/*====================================================================
    ������      ��ProcMcsMcuGetMtStatusReq
    ����        ����ѯ�����ն�״̬������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/06    1.0         LI Yi         ����
	02/11/01	1.1			Liaoweijiang  �޸�
	04/05/10	3.0			������        �޸�
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetMtStatusReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMtStatus	tMtStatus;
	TMt			tMt = *( TMt* )cServMsg.GetMsgBody();

	switch( CurState() )
	{
	case STATE_ONGOING:

		//Ӧ��
		if( m_tConfAllMtInfo.MtInConf( tMt.GetMtId() ) && 
			m_ptMtTable->GetMtStatus( tMt.GetMtId(), &tMtStatus ) )
		{            
			tMtStatus.SetTMt( tMt );
			cServMsg.SetMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}
		else
		{
			cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            SendMsgToMt( tMt.GetMtId(), MCU_MT_GETMTSTATUS_REQ, cServMsg );
		}
		
		//��ѯ�ն�״̬
		//SendMsgToMt( tMt.GetMtId(), MCU_MT_GETMTSTATUS_REQ, cServMsg );

		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
	}
}

/*====================================================================
    ������      ��ProcMcsMcuGetAllMtStatusReq
    ����        ����ѯ�����ն�״̬������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/04/27	3.0			������         �޸�
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetAllMtStatusReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMcu tMcu;
	u8 byLoop;

	switch( CurState() )
	{
	case STATE_ONGOING: 
		
		//�Ƿ��ڱ���������
		tMcu = *(TMcu*)cServMsg.GetMsgBody(); 

		//�����ն�״̬
		if( tMcu.GetMcuIdx() == INVALID_MCUIDX )
		{
			//�ȷ�����MC
			for( u16 wLoop = 0; wLoop < m_tConfAllMtInfo.GetMaxMcuNum(); wLoop++ )
			{
				u16 wMcuIdx = m_tConfAllMtInfo.GetMtInfo(wLoop).GetMcuIdx();
				if( !IsValidMcuId( wMcuIdx ))
				{
					continue;
				}

				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );
				if(ptConfMcInfo == NULL)
				{
					continue;
				}
				tMcu.SetNull();
				tMcu.SetMcu( 0 );	
				tMcu.SetMcuIdx( wMcuIdx );	
				cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
				
				for(s32 nLoop =0; nLoop<MAXNUM_CONF_MT; nLoop++)
				{
					if(ptConfMcInfo->m_atMtStatus[nLoop].IsNull()|| 
						ptConfMcInfo->m_atMtStatus[nLoop].GetMtId() == 0) //�Լ�
					{
						continue;
					}
					TMtStatus tMtStatus = ptConfMcInfo->m_atMtStatus[nLoop].GetMtStatus();
					cServMsg.CatMsgBody((u8 *)&tMtStatus, sizeof(TMtStatus));
				}
				SendReplyBack( cServMsg, MCU_MCS_ALLMTSTATUS_NOTIF );
			}

			//���������
			SendAllLocalMtStatus( MCU_MCS_ALLMTSTATUS_NOTIF );
		}		

		//���Ǳ�����MCU
		cServMsg.SetMsgBody( (u8*)&tMcu,sizeof(tMcu) );

		if( tMcu.IsLocal())
		{
			TMtStatus tMtStatus;
			//�õ�״̬�б�
			for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
			{
				if( m_tConfAllMtInfo.MtInConf( byLoop ) && 
					m_ptMtTable->GetMtStatus( byLoop, &tMtStatus ) ) 
				{
					tMtStatus.SetTMt( m_ptMtTable->GetMt( tMtStatus.GetMtId() ) );
					cServMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
				}
			}
		}
		else
		{
			u16 wMcuIdx = tMcu.GetMcuIdx();
			if( !m_tConfAllMtInfo.m_tLocalMtInfo.MtJoinedConf( GetFstMcuIdFromMcuIdx( wMcuIdx ) ) )
			{
				cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}

			TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );
			if (NULL == ptConfMcInfo)
			{
				cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}

			cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
			for(s32 nLoop =0; nLoop<MAXNUM_CONF_MT; nLoop++)
			{
				if( ptConfMcInfo->m_atMtStatus[nLoop].IsNull()|| 
					ptConfMcInfo->m_atMtStatus[nLoop].GetMtId() == 0) //�Լ�
				{
					continue;
				}
				TMtStatus tMtStatus = ptConfMcInfo->m_atMtStatus[nLoop].GetMtStatus();
                cServMsg.CatMsgBody((u8*)&tMtStatus, sizeof(TMtStatus));
			}
		}	

		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 ); 

		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
	}
}

/*=============================================================================
    �� �� ���� MtVideoSourceSwitched
    ��    �ܣ� �����ն˵���ƵԴ�仯
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CServMsg & cServMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/04/06  3.6			�                   ����
=============================================================================*/
void CMcuVcInst::MtVideoSourceSwitched(CServMsg & cServMsg)
{
    //CServMsg	cServMsg( pcMsg->content, pcMsg->length );
    u8 bySrcMtId = cServMsg.GetSrcMtId();
    TMt tMt = *(TMt*)cServMsg.GetMsgBody();
    TMt	tDstMt;
    u16	wEvent;

    if (STATE_ONGOING == CurState())
    {
        u8 byCurrVidNo = *(cServMsg.GetMsgBody() + sizeof(TMt));

        m_ptMtTable->SetCurrVidSrcNo(bySrcMtId, byCurrVidNo);

        if (m_tConf.HasChairman())
        {
            TMt tSpeakerMt = m_tConf.GetSpeaker();
            TMt tChairmanMt = m_tConf.GetChairman();
            if (tSpeakerMt.GetMtId() == bySrcMtId)
            {
                tDstMt = m_tConf.GetChairman();
                wEvent = MCU_MT_VIDEOSOURCESWITCHED_CMD;
            }
            else if (tChairmanMt.GetMtId() == bySrcMtId)
            {
                tDstMt = m_tConf.GetSpeaker();
                wEvent = MCU_MT_VIDEOSOURCESWITCHED_CMD;
            }
            else
            {
                return;
            }
        }
        else
        {
            return;
        }

        //send messge
        cServMsg.SetDstMtId( tDstMt.GetMtId() );
        SendMsgToMt( tDstMt.GetMtId(), wEvent, cServMsg );
    
    }
    else
    {
        ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
                        cServMsg.GetEventId(), ::OspEventDesc(cServMsg.GetEventId()), CurState());
    }
}

/*=============================================================================
    �� �� ���� ProcMtMcuVideoSourceSwitched
    ��    �ܣ� �����ն˵���ƵԴ�仯
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CServMsg & cServMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/04/06  3.6			�                   ����
=============================================================================*/
void CMcuVcInst::ProcMtMcuVideoSourceSwitched(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    MtVideoSourceSwitched(cServMsg);
}

/*====================================================================
    ������      ��ProcMcsMcuCamCtrlCmd
    ����        ���ն˲���ͳһ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/04/02    3.0         ������        ����
    2006.4.18   4.0         ����        �й����ƴ������
====================================================================*/
void CMcuVcInst::ProcMcsMcuCamCtrlCmd( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 bySrcMtId = cServMsg.GetSrcMtId();
	TMt tMt = *(TMt*)cServMsg.GetMsgBody();
	TMt	tDstMt;
	u16	wEvent;

	// [pengjie 2010/8/12] ����Զҡ
	u8 byFeccParam  = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt));
	// End

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] step 1 bySrcMtId.%d - MtMcuId.%d MtMtId.%d\n", 
			 bySrcMtId, tMt.GetMcuId(), tMt.GetMtId() );

	switch( cServMsg.GetEventId() )
	{
		case MT_MCU_MTCAMERA_CTRL_CMD:		    //��ϯ�����ն�����ͷ�ƶ�
		case MT_MCU_MTCAMERA_CTRL_STOP:		    //��ϯ�����ն�����ͷֹͣ�ƶ�
		case MT_MCU_MTCAMERA_RCENABLE_CMD:	    //��ϯ�����ն�����ͷң����ʹ��
		case MT_MCU_MTCAMERA_MOVETOPOS_CMD:	    //��ϯ�����ն�����ͷ������ָ��λ��
		case MT_MCU_MTCAMERA_SAVETOPOS_CMD:	    //��ϯ�����ն�����ͷ���浽ָ��λ��
		case MT_MCU_SELECTVIDEOSOURCE_CMD:           //���Ҫ��MCU�����ն���ƵԴ
			tMt = m_ptMtTable->GetMt( tMt.GetMtId() );
			break;
		default:
			break;
	}
	

	switch( CurState() )
	{
	case STATE_ONGOING:
        // ���Ǳ���MCU�µ��ն�
        if(!tMt.IsLocal()&& (!tMt.IsNull()) )
		{
            if ( bySrcMtId != 0 )
            {
                // ���󣺸���MCU�ϵ���ϯֻ�ܶԱ���MCU�µ��ն˽��п��ƣ����ܿ缶����
                // �ն�û��Ȩ���л��¼�MCU���ն˵���ƵԴ
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] MT.%d no permission to operate, return\n", 
		                 bySrcMtId );					
                return;                
            }
// 			// �޸�Ϊң�ضԷ�MCU
// 			tDstMt.SetMtId(tMt.GetMcuId());
// 			tDstMt.SetMcuId(LOCAL_MCUID);
			// [pengjie 2010/8/12] ����Զҡ�¼��ն�֧��
			else
			{
				if(IsLocalAndSMcuSupMultSpy( tMt.GetMcuId() ))
				{
					tDstMt = m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx(tMt.GetMcuId()) );
				}
				else
				{
					tDstMt.SetMtId(GetFstMcuIdFromMcuIdx(tMt.GetMcuId()));
					tDstMt.SetMcuId(LOCAL_MCUID);
				}
			}
		}
        // ������ն���Ϣ
		else if( bySrcMtId != 0 )
		{	          
			if( tMt.IsNull() )
			{
				m_ptMtTable->GetMtSrc( bySrcMtId, &tDstMt, MODE_VIDEO );
				if( !tDstMt.IsNull() )
				{
					if( tDstMt.IsLocal() )
					{
						if( tDstMt.GetMtId() == bySrcMtId && MT_MANU_KDCMCU == m_ptMtTable->GetManuId(bySrcMtId))
						{
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] bySrcMtId(MCU).%d src is self.return\n", bySrcMtId );
							return;
						}
						else
						{
							tDstMt = m_ptMtTable->GetMt( tDstMt.GetMtId() );
						}
							
					}
					else
					{
						if(IsLocalAndSMcuSupMultSpy( tDstMt.GetMcuId() ))
						{
							tMt = tDstMt;
							tDstMt = m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx(tDstMt.GetMcuId()) );
						}
						else
						{
							tDstMt.SetMtId(GetFstMcuIdFromMcuIdx(tDstMt.GetMcuId()));
							tDstMt.SetMcuId(LOCAL_MCUID);
						}
					}					
				}
			}
			else
			{
                tDstMt = m_ptMtTable->GetMt( tMt.GetMtId() );

				if(tDstMt.IsNull())
				{
					m_ptMtTable->GetMtSrc( bySrcMtId, &tDstMt, MODE_VIDEO );
					if(tDstMt.IsNull())
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] bySrcMtId.%d tDstMt.IsNull() return\n", bySrcMtId );

						return;
					}
				}                
			}

			// �ն�Զң��Ҫ�б����, ��ϯ/�ϼ�mcu/�򵥼���MCU����Զң

			if( // ���Ա�����ϯ
                bySrcMtId != m_tConf.GetChairman().GetMtId() && 
                // �ϲ�����ʱ�����ϼ�MCU
				bySrcMtId != m_tCascadeMMCU.GetMtId()        &&
                // �򵥼���ʱ����MCU
                MT_MANU_KDCMCU != m_ptMtTable->GetManuId(bySrcMtId) )
			{
                // �������ͨ�նˣ������Ƿ���ѡ��ҪԶң���ն�
                TMtStatus tMtStatus;

                if ( m_ptMtTable->GetMtStatus( bySrcMtId, &tMtStatus) )
                {
                    if ( ( GetLocalSpeaker().GetMtId() == tDstMt.GetMtId() && GetLocalSpeaker().GetType() != TYPE_MCUPERI ) ||
                         tMtStatus.GetVideoMt().GetMtId() != tDstMt.GetMtId() )
                    {
                        // ����ѡ�����նˣ�����ң��
				        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] bySrcMtId.%d is not chairman or inspecter to Dst%d, return\n", 
						         bySrcMtId, tDstMt.GetMtId() );

                        // ֪ͨ��������ϯ
                        SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );

                        return;
                    }                    
                }               
			}

            // ����ϲ�����Ҫ�����ϼ����ܾ�
            if ( !m_tCascadeMMCU.IsNull() &&
                 tDstMt.GetMtId() == m_tCascadeMMCU.GetMtId() )
            {
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] cannot operate mmcu, return\n" );
				return;
            }
			
		}
		else
		{
			// ���Զң �� bySrcMtId == 0
			if( !tMt.IsNull())
			{
				tDstMt = m_ptMtTable->GetMt( tMt.GetMtId() );
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] bySrcMtId.%d tDstMt.IsNull() return\n", bySrcMtId );

				return;
			}
            // guzh [6/7/2007] ����ϲ�����Ҫ�����ϼ����ܾ�
            if ( !m_tCascadeMMCU.IsNull() &&
                tDstMt.GetMtId() == m_tCascadeMMCU.GetMtId() )
            {
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] cannot operate mmcu, return\n" );
                return;
            }
		}

		//not joined conference
		if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] step 3 bySrcMtId.%d - event.%d DstMcuId.%d DstMtId.%d\n", 
				     bySrcMtId, pcMsg->event, tDstMt.GetMcuId(), tDstMt.GetMtId() );

			return;
		}

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] step 4 bySrcMtId.%d - event.%d DstMcuId.%d DstMtId.%d\n", 
				  bySrcMtId, pcMsg->event, tDstMt.GetMcuId(), tDstMt.GetMtId() );

		switch( pcMsg->event )
		{
		//�ն�����ͷ����
		case MCS_MCU_MTCAMERA_CTRL_CMD:		//�ն�����ͷ�ƶ�
		case MT_MCU_MTCAMERA_CTRL_CMD:		//��ϯ�����ն�����ͷ�ƶ�
			wEvent = MCU_MT_MTCAMERA_CTRL_CMD;
			break;
		case MCS_MCU_MTCAMERA_CTRL_STOP:		//�ն�����ͷֹͣ�ƶ�
		case MT_MCU_MTCAMERA_CTRL_STOP:		//��ϯ�����ն�����ͷֹͣ�ƶ�
			wEvent = MCU_MT_MTCAMERA_CTRL_STOP;
			break;
		case MCS_MCU_MTCAMERA_RCENABLE_CMD:	//�ն�����ͷң����ʹ��
		case MT_MCU_MTCAMERA_RCENABLE_CMD:	//��ϯ�����ն�����ͷң����ʹ��
			wEvent = MCU_MT_MTCAMERA_RCENABLE_CMD;
			break;
		case MCS_MCU_MTCAMERA_MOVETOPOS_CMD:	//�ն�����ͷ������ָ��λ��
		case MT_MCU_MTCAMERA_MOVETOPOS_CMD:	//��ϯ�����ն�����ͷ������ָ��λ��
			wEvent = MCU_MT_MTCAMERA_MOVETOPOS_CMD;
			break;
		case MCS_MCU_MTCAMERA_SAVETOPOS_CMD:	//�ն�����ͷ���浽ָ��λ��
		case MT_MCU_MTCAMERA_SAVETOPOS_CMD:	//��ϯ�����ն�����ͷ���浽ָ��λ��
			wEvent = MCU_MT_MTCAMERA_SAVETOPOS_CMD;
			break;
        case MCS_MCU_SETMTVIDSRC_CMD:           //ѡ���ն���ƵԴ
        // libo [4/4/2005]
        case MT_MCU_SELECTVIDEOSOURCE_CMD:
        // libo [4/4/2005]end
            wEvent = MCU_MT_SETMTVIDSRC_CMD;
			break;
		default:
			return;
		}

		// send messge
		if( bySrcMtId != 0 && bySrcMtId == m_tCascadeMMCU.GetMtId() && 
			bySrcMtId == tDstMt.GetMtId() )
		{
			//�����ϼ�mcuԶң�¼�mcu�նˣ����ش�ͨ��Ϊ�ϼ�mcuʱ�����˴�����
            // remark by ���񻪣��ƺ�������������ܹ��˵��������ȱ���
		}
		else
		{
			// [pengjie 2010/8/12] ����Զҡ֧��
			cServMsg.SetDstMtId( tDstMt.GetMtId() );
			if( !tMt.IsNull() && !tMt.IsLocal() && IsLocalAndSMcuSupMultSpy( tMt.GetMcuId() ) )
			{
				TMsgHeadMsg tHeadMsg;
				TMt tMcuMt;
				tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt, tMcuMt );
				u16 wNetEventId = htons(wEvent);
				cServMsg.SetEventId( MCU_MCU_FECC_CMD );
				cServMsg.SetMsgBody();
				cServMsg.SetMsgBody( (u8 *)&tMcuMt, sizeof(tMcuMt) );
				cServMsg.CatMsgBody( (u8 *)&byFeccParam, sizeof(byFeccParam) );
				cServMsg.CatMsgBody( (u8 *)&(wNetEventId), sizeof(wNetEventId) );
				cServMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );

				SendMsgToMt( (u8)tMcuMt.GetMcuId(), cServMsg.GetEventId(), cServMsg);
			}
			else
			{
				SendMsgToMt( tDstMt.GetMtId(), wEvent, cServMsg );
			}
			// End
		}
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMMcuMcuCamCtrlCmd
    ����        ���ϼ�MCU���Ʊ����ն�����ͷ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/04/02    3.0         ������        ����
	20100812    4.6         pengjie	      ����Զҡ֧��
====================================================================*/
void CMcuVcInst::ProcMMcuMcuCamCtrlCmd( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	TMt tMt = *(TMt *)(cServMsg.GetMsgBody());
	u8 byFeccParam = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt));
	u16 wFeccEventId = *(u16 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8));
	wFeccEventId = ntohs(wFeccEventId);

	if( !tMt.IsMcuIdLocal())
	{
		TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(u16));
		TMt tDstMt = GetMtFromMultiCascadeMtInfo(tHeadMsg.m_tMsgDst, tMt);

		if (IsLocalAndSMcuSupMultSpy(tDstMt.GetMcuId()))
		{
			cServMsg.SetEventId( MCU_MCU_FECC_CMD );
			SendMsgToMt( u8(tMt.GetMcuId()), MCU_MCU_FECC_CMD, cServMsg );
			
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMMcuMcuCamCtrlCmd] Unlocal tmt, send MCU_MCU_FECC_CMD to smcu!\n" );
		}
		else
		{		
			cServMsg.SetEventId( wFeccEventId );
			SendMsgToMt( GetFstMcuIdFromMcuIdx(tDstMt.GetMcuIdx()), wFeccEventId, cServMsg );

			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMMcuMcuCamCtrlCmd] Unlocal tmt, send .%s to smcu.%d!\n", OspEventDesc(wFeccEventId), GetFstMcuIdFromMcuIdx(tDstMt.GetMcuIdx()));
		}		
		return;
	}

// 	TMt tDstMt = *(TMt *)cServMsg.GetMsgBody();
// 	u8 byFeccParam = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt));
// 	u16 wFeccEventId = *(u16 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8));
// 	wFeccEventId = ntohs(wFeccEventId);

	SendMsgToMt( tMt.GetMtId(), wFeccEventId, cServMsg );

	return;
}

/*====================================================================
    ������      ��ProcMtMcuMatrixMsg
    ����        ���ն˾������Ӧ����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/03/08    3.0         ������        ����
====================================================================*/
void CMcuVcInst::ProcMtMcuMatrixMsg( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 bySrcMtId = cServMsg.GetSrcMtId();
	u16	wEvent = 0;
	TMt tSrcMt;
	CServMsg cMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:

		switch( pcMsg->event )
		{	
	    //���þ���
        case MT_MCU_MATRIX_ALLSCHEMES_NOTIF:    
            wEvent = MCU_MCS_MATRIX_ALLSCHEMES_NOTIF;
            break;

        case MT_MCU_MATRIX_ONESCHEME_NOTIF:
            wEvent = MCU_MCS_MATRIX_ONESCHEME_NOTIF;
            break;

        case MT_MCU_MATRIX_SAVESCHEME_NOTIF:
            wEvent = MCU_MCS_MATRIX_SAVESCHEME_NOTIF;
            break;

        case MT_MCU_MATRIX_SETCURSCHEME_NOTIF:
            wEvent = MCU_MCS_MATRIX_SETCURSCHEME_NOTIF;
            break;

        case MT_MCU_MATRIX_CURSCHEME_NOTIF:
            wEvent = MCU_MCS_MATRIX_CURSCHEME_NOTIF;
            break;

            //���þ���
        case MT_MCU_EXMATRIXINFO_NOTIFY:  
            wEvent = MCU_MCS_EXMATRIXINFO_NOTIFY;
            break;
        
        case MT_MCU_EXMATRIX_GETPORT_NOTIF:
            wEvent = MCU_MCS_EXMATRIX_GETPORT_NOTIF;
            break;        
     
        case MT_MCU_EXMATRIX_PORTNAME_NOTIF:
            wEvent = MCU_MCS_EXMATRIX_PORTNAME_NOTIF;
            break;
        case MT_MCU_EXMATRIX_ALLPORTNAME_NOTIF:
            wEvent = MCU_MCS_EXMATRIX_ALLPORTNAME_NOTIF;
            break;

            //��չ��ƵԴ
        case MT_MCU_ALLVIDEOSOURCEINFO_NOTIF:
            wEvent = MCU_MCS_ALLVIDEOSOURCEINFO_NOTIF;
            break;
        case MT_MCU_VIDEOSOURCEINFO_NOTIF:
            wEvent = MCU_MCS_VIDEOSOURCEINFO_NOTIF;
            break;
		
        case MT_MCU_EXMATRIX_GETPORT_ACK:
        case MT_MCU_EXMATRIX_GETPORT_NACK:
			return;
			
		default:
			return;
		}

		//send messge
		tSrcMt = m_ptMtTable->GetMt( bySrcMtId );
		cMsg.SetMsgBody( (u8*)&tSrcMt, sizeof(tSrcMt) );
		cMsg.CatMsgBody( cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		SendMsgToAllMcs( wEvent, cMsg );
		break;

	default:
		ConfPrint( LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMMcuBandwidthNotify
    ����        ������E1�ϼ�����ָʾ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt& tMMcu			Դ�ն�
				  const u32 dwBandWidth		����				  
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	2012/05/12   4.7.1        �ܾ���        ����
====================================================================*/
void CMcuVcInst::ProcMMcuBandwidthNotify( const TMt& tMMcu,const u32 dwBandWidth )
{	
	switch( CurState() )
	{
	case STATE_ONGOING:
		{		
			if( m_tCascadeMMCU.IsNull() || m_tCascadeMMCU.GetMtId() != tMMcu.GetMtId() )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] mt<%d> is not mmcu.so not handle.\n", 
						tMMcu.GetMtId() );
				return;
			}
			if( !m_ptMtTable->GetMtTransE1(m_tCascadeMMCU.GetMtId()) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] mt<%d> is mmcu.but not e1 trans.so not handle.\n", 
						tMMcu.GetMtId() );
				return;
			}
			if( 0 == dwBandWidth )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] dwBandWidth is zero.so not handle.\n" );
				return;
			}
			CServMsg cServMsg(NULL,0);
			

			//u16 wBandWidth = ntohs(*(u16 *)cServMsg.GetMsgBody());
			/*u16 wDailBandWidth = m_ptMtTable->GetRcvBandWidth( m_tCascadeMMCU.GetMtId() );

			
			u16 wRealBandWidth = wBandWidth;
			if( wRealBandWidth > m_tConf.GetBitRate() )
			{
				wRealBandWidth = m_tConf.GetBitRate();
			}
			if( wRealBandWidth < m_tConf.GetSecBitRate() )
			{
				wRealBandWidth = m_tConf.GetSecBitRate();
			}
			if( wRealBandWidth > wDailBandWidth )
			{
				wRealBandWidth = wDailBandWidth;
			}*/
			u32 dwRealBandWidth = dwBandWidth;
			u16 wMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
			if( !IsValidMcuId( wMcuIdx ) )
			{
				ConfPrint( LOG_LVL_ERROR, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] wMcuIdx.%d is invalid\n",wMcuIdx );
				return;
			}
			BOOL32 bIsMMcuSupMultSpy = IsLocalAndSMcuSupMultSpy( wMcuIdx );
		
			if( bIsMMcuSupMultSpy )
			{
				if( dwRealBandWidth > m_tConf.GetSndSpyBandWidth() )
				{
					dwRealBandWidth = m_tConf.GetSndSpyBandWidth();
				}

				SetRealSndSpyBandWidth( dwRealBandWidth );
			}
			else
			{	
				SetRealSndSpyBandWidth( dwRealBandWidth );
			}
			
			
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] mt<%d> dwBandWidth(%d) is adjust to dwBandWidth(%d).\n", m_tCascadeMMCU.GetMtId(),
						 dwBandWidth,dwRealBandWidth );

			CSendSpy *ptSndSpy = NULL;
			TSimCapSet tDstCap;
			
			u8 bySpyNum = 0;
			u32 dwSpyBandWidth = 0;
			u8 bySpyIdx = 0;
			if( bIsMMcuSupMultSpy )
			{
				for (bySpyIdx = 0; bySpyIdx < MAXNUM_CONF_SPY; bySpyIdx++)
				{
					tDstCap.Clear();
					ptSndSpy = m_cLocalSpyMana.GetSendSpy(bySpyIdx);
					if (NULL == ptSndSpy)
					{
						continue;
					}
					if( ptSndSpy->GetSpyMt().IsNull() )
					{
						continue;
					}
					if( !m_tConfAllMtInfo.MtJoinedConf( GetLocalMtFromOtherMcuMt( ptSndSpy->GetSpyMt() ).GetMtId() ) )
					{
						continue;
					}
					
					tDstCap = ptSndSpy->GetSimCapset();
					if( ptSndSpy->GetSpyMode() == MODE_VIDEO  || MODE_BOTH == ptSndSpy->GetSpyMode() )
					{
						++bySpyNum;
						dwSpyBandWidth += tDstCap.GetVideoMaxBitRate();
					}
					if( ptSndSpy->GetSpyMode() == MODE_AUDIO  || MODE_BOTH == ptSndSpy->GetSpyMode() )
					{	
						dwSpyBandWidth += GetAudioBitrate( tDstCap.GetAudioMediaType() );
					}
				}
			}
			
			//˫����������һ·����
			TLogicalChannel tLogChn;
			TLogicalChannel tSecLogChn;
			TMtStatus tStatus;
			if( m_ptMtTable->GetMtLogicChnnl(m_tCascadeMMCU.GetMtId(),LOGCHL_SECVIDEO,&tSecLogChn,TRUE)
				&& !m_tDoubleStreamSrc.IsNull() )
			{
				bySpyNum++;
			}
			else
			{
				m_ptMtTable->GetMtStatus( m_tCascadeMMCU.GetMtId(),&tStatus );
				TMt tVideoSrcMt = tStatus.GetVideoMt();
				if(  !tVideoSrcMt.IsNull() )
				{
					++bySpyNum;
					tVideoSrcMt = GetLocalMtFromOtherMcuMt(tVideoSrcMt);
					dwSpyBandWidth += m_ptMtTable->GetMtReqBitrate( tVideoSrcMt.GetMtId(),TRUE );
					tVideoSrcMt = tStatus.GetAudioMt();
					if( !tVideoSrcMt.IsNull() )
					{
						tVideoSrcMt = GetLocalMtFromOtherMcuMt(tVideoSrcMt);					
						if( m_ptMtTable->GetMtLogicChnnl( tVideoSrcMt.GetMtId(),LOGCHL_AUDIO,&tLogChn,TRUE) )
						{						
							dwSpyBandWidth += GetAudioBitrate( tLogChn.GetChannelType() );
						}					
					}
				}
				else
				{
					tVideoSrcMt = tStatus.GetAudioMt();
					if( !tVideoSrcMt.IsNull() )
					{
						++bySpyNum;
						tVideoSrcMt = GetLocalMtFromOtherMcuMt(tVideoSrcMt);					
						if( m_ptMtTable->GetMtLogicChnnl( tVideoSrcMt.GetMtId(),LOGCHL_AUDIO,&tLogChn,TRUE) )
						{						
							dwSpyBandWidth += GetAudioBitrate( tLogChn.GetChannelType() );
						}
					}				
				}
			}


			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] all spy BandWidth(%d)   E1 wBandWidth(%d)!\n", 
						 dwSpyBandWidth,dwRealBandWidth );
					
			u16 wSingleBandWidth = 0;
			if( 0 != bySpyNum )
			{
				wSingleBandWidth = (u16)(dwRealBandWidth / bySpyNum);
			}
			else
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] bySpyNum is zero,needn't handle!\n" );
				return;
			}
			
			u16 wModifiedBandWidth = wSingleBandWidth;
			BOOL32 bIsUp = FALSE;
			TMt tSelMt;
			for (bySpyIdx = 0; bySpyIdx < MAXNUM_CONF_SPY; bySpyIdx++)
			{
				wModifiedBandWidth = wSingleBandWidth;
				bIsUp = FALSE;
				tDstCap.Clear();
				ptSndSpy = m_cLocalSpyMana.GetSendSpy(bySpyIdx);
				if (NULL == ptSndSpy)
				{
					continue;
				}
				if( ptSndSpy->GetSpyMt().IsNull() )
				{
					continue;
				}
				if( !m_tConfAllMtInfo.MtJoinedConf( GetLocalMtFromOtherMcuMt( ptSndSpy->GetSpyMt() ).GetMtId() ) )
				{
					continue;
				}
				
				tDstCap = ptSndSpy->GetSimCapset();
				if( ptSndSpy->GetSpyMode() == MODE_AUDIO )
				{
					continue;
				}

				if( wModifiedBandWidth == tDstCap.GetVideoMaxBitRate() )
				{
					continue;
				}
				tSelMt = GetLocalMtFromOtherMcuMt(ptSndSpy->GetSpyMt());
				if( wModifiedBandWidth > m_ptMtTable->GetDialBitrate(tSelMt.GetMtId()) )
				{
					wModifiedBandWidth = m_ptMtTable->GetDialBitrate(tSelMt.GetMtId());
				}

				if( !m_tDoubleStreamSrc.IsNull() )
				{
					wModifiedBandWidth = GetDoubleStreamVideoBitrate( wModifiedBandWidth,FALSE );
				}

				if( ptSndSpy->GetSpyMode() == MODE_BOTH )
				{
					wModifiedBandWidth -= GetAudioBitrate( tDstCap.GetAudioMediaType() );
				}

				if( wModifiedBandWidth == tDstCap.GetVideoMaxBitRate() )
				{
					continue;
				}
				if( wModifiedBandWidth > tDstCap.GetVideoMaxBitRate() )
				{
					bIsUp = TRUE;
				}
				tDstCap.SetVideoMaxBitRate( wModifiedBandWidth );
				m_cLocalSpyMana.SaveSpySimCap(ptSndSpy->GetSpyMt(), tDstCap);
				if( IsNeedSpyAdpt( ptSndSpy->GetSpyMt(),tDstCap,MODE_VIDEO ) )
				{				
					if ( TRUE == bIsUp )
					{
						RefreshSpyBasParam( ptSndSpy->GetSpyMt(),tDstCap,MODE_VIDEO,TRUE,TRUE );
					}
					else
					{
						RefreshSpyBasParam( ptSndSpy->GetSpyMt(),tDstCap,MODE_VIDEO);
					}
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] SpyMt(%d.%d) refresh bas to wModifiedBandWidth.%d!\n", 
						ptSndSpy->GetSpyMt().GetMcuId(),ptSndSpy->GetSpyMt().GetMtId(),wModifiedBandWidth );
				}
				else
				{
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] SpyMt(%d.%d) refresh mt wModifiedBandWidth.%d!\n", 
											ptSndSpy->GetSpyMt().GetMcuId(),ptSndSpy->GetSpyMt().GetMtId(),wModifiedBandWidth );
					if( ptSndSpy->GetSpyMt().IsLocal() )
					{					
						if (m_ptMtTable->GetMtLogicChnnl(ptSndSpy->GetSpyMt().GetMtId(), LOGCHL_VIDEO, &tLogChn, FALSE))
						{							
							tLogChn.SetFlowControl(wModifiedBandWidth);							
							cServMsg.SetMsgBody((u8*)&tLogChn, sizeof(tLogChn)); 
							SendMsgToMt( ptSndSpy->GetSpyMt().GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );  
						}
					}
					else
					{
						CascadeAdjMtBitrate( ptSndSpy->GetSpyMt(),wModifiedBandWidth );
					}
				}				
			}
			wModifiedBandWidth = wSingleBandWidth;
			if( wModifiedBandWidth - GetAudioBitrate(m_tConf.GetMainAudioMediaType()) != m_ptMtTable->GetMtReqBitrate( m_tCascadeMMCU.GetMtId(),TRUE ) )
			{				
				BOOL32 bIsNeedDoubleBrdAdapt = IsNeedAdapt( m_tDoubleStreamSrc,m_tCascadeMMCU,MODE_SECVIDEO );
				BOOL32 bIsDoubleUp = FALSE;
				bIsUp = FALSE;
				tSelMt = tStatus.GetVideoMt();
				if(  !tSelMt.IsNull() && !(tSelMt == m_tCascadeMMCU) )
				{
					tSelMt = GetLocalMtFromOtherMcuMt(tSelMt);
					if( wModifiedBandWidth > m_ptMtTable->GetDialBitrate(tSelMt.GetMtId()) )
					{
						wModifiedBandWidth = m_ptMtTable->GetDialBitrate(tSelMt.GetMtId());
					}
					tSelMt = tStatus.GetAudioMt();
					if( !tSelMt.IsNull() )
					{
						tSelMt = GetLocalMtFromOtherMcuMt(tSelMt);					
						if( m_ptMtTable->GetMtLogicChnnl( tSelMt.GetMtId(),LOGCHL_AUDIO,&tLogChn,TRUE) )
						{						
							wModifiedBandWidth -= GetAudioBitrate( tLogChn.GetChannelType() );
						}					
					}
					tSelMt = tStatus.GetVideoMt();				

					if( !m_tDoubleStreamSrc.IsNull() )
					{
						bIsUp = GetDoubleStreamVideoBitrate(wModifiedBandWidth,TRUE) > m_ptMtTable->GetMtReqBitrate(m_tCascadeMMCU.GetMtId(),TRUE);
						m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),GetDoubleStreamVideoBitrate(wModifiedBandWidth,TRUE),LOGCHL_VIDEO );
						m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),GetDoubleStreamVideoBitrate(wModifiedBandWidth,FALSE),LOGCHL_SECVIDEO );	
						
						if( GetDoubleStreamVideoBitrate(wModifiedBandWidth,FALSE) > m_ptMtTable->GetMtReqBitrate( m_tCascadeMMCU.GetMtId(),FALSE ) )
						{
							bIsDoubleUp = TRUE;
						}
					}
					else
					{
						bIsUp = wModifiedBandWidth > m_ptMtTable->GetMtReqBitrate(m_tCascadeMMCU.GetMtId(),TRUE);
						m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),wModifiedBandWidth,LOGCHL_VIDEO );
					}

					if( IsNeedSelAdpt(tSelMt,m_tCascadeMMCU,MODE_VIDEO) )
					{
						ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] tSelMt(%d.%d) refresh bas. wModifiedBandWidth.%d bIsUp.%d!\n", 
											tSelMt.GetMcuId(),tSelMt.GetMtId(),m_ptMtTable->GetMtReqBitrate(m_tCascadeMMCU.GetMtId(),TRUE),bIsUp );
						if ( TRUE == bIsUp )
						{
							RefreshSelBasParam( tSelMt,m_tCascadeMMCU,MODE_VIDEO,TRUE,TRUE );
						}
						else
						{
							RefreshSelBasParam( tSelMt,m_tCascadeMMCU,MODE_VIDEO );
						}
					}
					else
					{
						ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] tSelMt(%d.%d) refresh mt. wModifiedBandWidth.%d!\n", 
											tSelMt.GetMcuId(),tSelMt.GetMtId(),wModifiedBandWidth );
						if( tSelMt.IsLocal() )
						{
							AdjustMtVideoSrcBR( m_tCascadeMMCU.GetMtId(),wModifiedBandWidth,MODE_VIDEO );
						}
						else
						{
							CascadeAdjMtBitrate( tSelMt,wModifiedBandWidth );
						}
					}
				}
				else
				{
					wModifiedBandWidth = wModifiedBandWidth - GetAudioBitrate(m_tConf.GetMainAudioMediaType());
					if( !m_tDoubleStreamSrc.IsNull() &&
						GetDoubleStreamVideoBitrate(wModifiedBandWidth,FALSE) > m_ptMtTable->GetMtReqBitrate( m_tCascadeMMCU.GetMtId(),FALSE ) )
					{
						bIsDoubleUp = TRUE;
					}

					if( !m_tDoubleStreamSrc.IsNull() )
					{						
						m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),GetDoubleStreamVideoBitrate(wModifiedBandWidth,TRUE),LOGCHL_VIDEO );
						m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),GetDoubleStreamVideoBitrate(wModifiedBandWidth,FALSE),LOGCHL_SECVIDEO );						
					}
					else
					{					
						m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),wModifiedBandWidth,LOGCHL_VIDEO );
					}
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] tSelMt not exists.set reqbitrate. wModifiedBandWidth.%d !\n", 
											tSelMt.GetMcuId(),tSelMt.GetMtId(),m_ptMtTable->GetMtReqBitrate(m_tCascadeMMCU.GetMtId(),TRUE),bIsUp );
				}
				
				if( !m_tDoubleStreamSrc.IsNull() && !(m_tDoubleStreamSrc == m_tPlayEqp) )
				{					
					tSelMt.SetNull();
					
					if( 0 == m_tConf.GetSecBitRate() )
					{
						if( !bIsNeedDoubleBrdAdapt )
						{							
							RestoreVidSrcBitrate(tSelMt, MODE_SECVIDEO);														
						}
						else
						{
							if ( TRUE == bIsDoubleUp  )
							{
								RefreshBrdBasParamForSingleMt(m_tCascadeMMCU.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
							}
							else
							{
								RefreshBrdBasParamForSingleMt(m_tCascadeMMCU.GetMtId(),MODE_SECVIDEO);
							}
						}
					}
					else
					{
						if( !bIsNeedDoubleBrdAdapt )
						{
							if( IsNeedAdapt(m_tDoubleStreamSrc, m_tCascadeMMCU, MODE_SECVIDEO)/*m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE)*/)
							{
								if ( TRUE == bIsDoubleUp  )
								{
									RefreshBrdBasParamForSingleMt(m_tCascadeMMCU.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								}
								else
								{
									RefreshBrdBasParamForSingleMt(m_tCascadeMMCU.GetMtId(),MODE_SECVIDEO);
								}
								StartSwitchToSubMtFromAdp(m_tCascadeMMCU.GetMtId(), MODE_SECVIDEO);
							}
							else
							{
								ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMMcuBandwidthNotify] before after all not Sec adapt,it's impossible.mt(%d)\n",m_tCascadeMMCU.GetMtId() );
							}
						}					
						else
						{
							if( !IsNeedAdapt(m_tDoubleStreamSrc, m_tCascadeMMCU, MODE_SECVIDEO) )
							{			
								ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMMcuBandwidthNotify] before need Sec adapt,after not Sec adapt,rehandle switch.mt(%d)\n",m_tCascadeMMCU.GetMtId() );
								TSwitchGrp tSwitchGrp;
								tSwitchGrp.SetSrcChnl(0);
								tSwitchGrp.SetDstMtNum(1);
								tSwitchGrp.SetDstMt(&m_tCascadeMMCU);
								g_cMpManager.StartSwitchToAll(m_tDoubleStreamSrc, 1, &tSwitchGrp, MODE_SECVIDEO);

								u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( m_tCascadeMMCU.GetMtId(),FALSE );
								m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),wMtBitRate - 10,LOGCHL_SECVIDEO );
								
								RefreshBrdBasParamForSingleMt(m_tCascadeMMCU.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),wMtBitRate,LOGCHL_SECVIDEO );
							}
							else
							{
								if ( TRUE == bIsDoubleUp  )
								{
									RefreshBrdBasParamForSingleMt(m_tCascadeMMCU.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								}
								else
								{
									RefreshBrdBasParamForSingleMt(m_tCascadeMMCU.GetMtId(),MODE_SECVIDEO);
								}
							}
						}
					}
				}
			}

			
		}
		break;
	default:
		break;
	}
	
}
/*=============================================================================
�� �� ���� ProcMtMcuBandwidthNotif
��    �ܣ� �ն˴���ָʾ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage *pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/21   4.0			������                ����
2008/1/29   4.0         �ű���                ���ñ�ָʾ�����д���תͶFlowctrlCmd����
=============================================================================*/
void CMcuVcInst::ProcMtMcuBandwidthNotif(const CMessage *pcMsg)
{

    if (STATE_ONGOING != CurState())
    {
        ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: ProcMtMcuBandwidthNotif received in state %u, ignore it\n", CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

	
    u8  byDstMtId = cServMsg.GetSrcMtId();
    
    //u16 wMinBitRate;    
    //TMt tMtSrc;
    //TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );

    u16 wRcvBandWidth = ntohs(*(u16 *)cServMsg.GetMsgBody());
    u16 wSndBandWidth = ntohs(*(u16 *)(cServMsg.GetMsgBody() + sizeof(u16)));

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif] mt<%d> wRcvBandWidth :%d, wSndBandWidth :%d\n", 
            cServMsg.GetSrcMtId(), wRcvBandWidth, wSndBandWidth);

	

	//zbq[09/10/2008] ������ʶ���ӣ�����ʱ������
	m_ptMtTable->SetRcvBandAdjusted(cServMsg.GetSrcMtId(), TRUE);

    TLogicalChannel tLogicChan;
    /*tLogicChan.SetMediaType(MODE_VIDEO);
    tLogicChan.SetFlowControl(wRcvBandWidth);
    
    cServMsg.SetMsgBody((u8*)&tLogicChan, sizeof(TLogicalChannel));*/

    //��ǻ�ָ���Ǹ��ն˵����������
    u16 wDailBandWidth = m_ptMtTable->GetRcvBandWidth(byDstMtId);
    
    //ͨ�����ܻ�û���ü��򿪳ɹ�
    if ( 0 == wDailBandWidth )
    {
        wDailBandWidth = m_ptMtTable->GetDialBitrate(byDstMtId);
		m_ptMtTable->SetRcvBandWidth(byDstMtId, wDailBandWidth);
    }

    BOOL32 bE1Switch2ETH = FALSE;

    //��ʱ���Թ��ƣ�Ӧ�ù���. ���С��5����Ϊ�ָ���������Ϊ����. ֻ���E1�ն�.
    //��̫�ն�ʼ�ղ���Ϊ����, ��������·�������8M��һ��Ϊ10M.
    if ( wRcvBandWidth > 1024 * 8 )
    {
        if (m_ptMtTable->GetMtTransE1( byDstMtId ) )
        {
            bE1Switch2ETH = TRUE;
        }
        m_ptMtTable->SetMtTransE1( byDstMtId, FALSE );
    }
    else
    {
        m_ptMtTable->SetMtTransE1( byDstMtId, TRUE );

		//zbq[9/27/2008] ��������С��E1��ǰ�����������Ϊ��������
        if (wDailBandWidth < wRcvBandWidth)
        {
            m_ptMtTable->SetMtBRBeLowed(byDstMtId, FALSE);
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuBandwidthNotif] DstMt.%d NOT be set lowed due to<Dail.%d, Rcv.%d>(less)\n",
                      byDstMtId, wDailBandWidth, wRcvBandWidth );            
        }
        else
        {
			if (abs(wDailBandWidth-wRcvBandWidth)*100 / wDailBandWidth > 20)
			{
				m_ptMtTable->SetMtBRBeLowed(byDstMtId, TRUE);
				m_ptMtTable->SetLowedRcvBandWidth(byDstMtId, wRcvBandWidth);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuBandwidthNotif] DstMt.%d be set lowed due to<Dail.%d, Rcv.%d>\n",
					byDstMtId, wDailBandWidth, wRcvBandWidth );

			}
			else
			{
				m_ptMtTable->SetMtBRBeLowed(byDstMtId, FALSE);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuBandwidthNotif] DstMt.%d NOT be set lowed due to<Dail.%d, Rcv.%d>(greater less than 20)\n",
					byDstMtId, wDailBandWidth, wRcvBandWidth );            
			}
        }
    }

    //zbq[05/25/2009] ��E1�ն˲�����ͨ������Ϣˢ����E1�л���ETH����ͨ������Ϣ��������
    //��Ϊ��BR=10240���ᱣ����ConfBR���Ӷ�Ӱ����������ETH�ն˵Ĵ������
    if (!bE1Switch2ETH &&
        !m_ptMtTable->GetMtTransE1(byDstMtId))
    {
        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuBandwidthNotif] DstMt.%d BandWidth ntf has been ignored due to ETH!\n", byDstMtId);
        return;
    }

	u16 wDialBitRate = m_ptMtTable->GetDialBitrate(byDstMtId);
	if( wRcvBandWidth > wDialBitRate )
	{
		wRcvBandWidth = wDialBitRate;
	}
	if( wRcvBandWidth < m_tConf.GetSecBitRate() )
	{
		wRcvBandWidth = m_tConf.GetSecBitRate();
	}
	if( wSndBandWidth > wDialBitRate )
	{
		wSndBandWidth = wDialBitRate;
	}
	if( wSndBandWidth < m_tConf.GetSecBitRate() )
	{
		wSndBandWidth = m_tConf.GetSecBitRate();
	}

	tLogicChan.SetMediaType(MODE_VIDEO);

	u16 wVideoBandWidth = 0;
	if( wRcvBandWidth > GetAudioBitrate(m_tConf.GetMainAudioMediaType()))
	{
		wVideoBandWidth = wRcvBandWidth - GetAudioBitrate(m_tConf.GetMainAudioMediaType());
	}
	tLogicChan.SetFlowControl(wVideoBandWidth);
    
    cServMsg.SetMsgBody((u8*)&tLogicChan, sizeof(TLogicalChannel));


	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif] Modify mt<%d> wRcvBandWidth :%d, wSndBandWidth :%d\n", 
            cServMsg.GetSrcMtId(), wRcvBandWidth, wSndBandWidth);    

    CMessage cMsg;
    cMsg.content = cServMsg.GetServMsg();
    cMsg.length = cServMsg.GetServMsgLen();
    ProcMtMcuFlowControlCmd(&cMsg, TRUE);

    /*
    u16 wAudBandWidth = GetAudioBitrate(m_tConf.GetCapSupport().GetMainSimCapSet().GetAudioMediaType());
    wSndBandWidth -= wAudBandWidth; 
    wRcvBandWidth -= wAudBandWidth;

    //1. �ն����д�����
    if (wSndBandWidth != m_ptMtTable->GetSndBandWidth(tMt.GetMtId()) &&
        wSndBandWidth <= m_tConf.GetBitRate() && wSndBandWidth >= m_tConf.GetSecBitRate())
    {
        m_ptMtTable->SetSndBandWidth(tMt.GetMtId(), wSndBandWidth); 
                
        //�Ƿ�Ӧ�ø�Դ�ն˷��ͽ��մ���ָʾ**
        if (IsMtSendingVideo(tMt.GetMtId()))
        {
            TLogicalChannel tLogicChnnl;
            if( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tLogicChnnl, FALSE ) )
            {
                if (!m_tDoubleStreamSrc.IsNull())
                {
                    tLogicChnnl.SetFlowControl( wSndBandWidth/2 );
                }
                else
                {
                    tLogicChnnl.SetFlowControl( wSndBandWidth );
                }				
                cServMsg.SetMsgBody( (u8*)&tLogicChnnl, sizeof( tLogicChnnl ) );
                SendMsgToMt( tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );
            }
        }
    }   
	
    //2. �ն����д�����
    if( wRcvBandWidth > m_tConf.GetBitRate() || wRcvBandWidth < m_tConf.GetSecBitRate() ||
        wRcvBandWidth == m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE))
    {
        return;
    }    
    
    m_ptMtTable->SetRcvBandWidth( tMt.GetMtId(), wRcvBandWidth );
    m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), wRcvBandWidth, LOGCHL_VIDEO);

    //����ѡ��Դ
    m_ptMtTable->GetMtSrc( tMt.GetMtId(), &tMtSrc, MODE_VIDEO );
    if(tMtSrc.IsNull())
    {
        return;
    }
    if( tMtSrc == tMt )
    {
        return;
    }
    if( !(tMtSrc == m_tVidBrdSrc) )
    {
        //����ѡ�����նˣ�ֱ��ת������ӦԴ
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2,  "[ProcMtMcuBandwidthNotif] SelectSee SrcMtId.%d, DstMtId.%d, wBitRate.%d\n", 
            tMtSrc.GetMtId(), tMt.GetMtId(), wRcvBandWidth);
        
        AdjustMtVideoSrcBR( tMt.GetMtId(), wRcvBandWidth );
        return;
    }
	
    TSimCapSet tSrcSimCapSet;
    TSimCapSet tDstSimCapSet;
    tDstSimCapSet = m_ptMtTable->GetDstSCS(tMt.GetMtId());
    wMinBitRate = GetLeastMtReqBitrate(TRUE, tDstSimCapSet.GetVideoMediaType());    

	//���������ڹ㲥����ϳ�ͼ��,������ϳ�����
    if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE &&
        m_tConf.m_tStatus.IsBrdstVMP())
    {
        u16 wBasBitrate;
        u8  byVmpChanNo;
        u8  byMediaType = m_tConf.GetCapSupport().GetMainSimCapSet().GetVideoMediaType();
        
        //˫�ٻ���Ҫ�������ʱ�ĺ�������
        if ( 0 != m_tConf.GetSecBitRate() && 
            MEDIA_TYPE_NULL == m_tConf.GetCapSupport().GetSecondSimCapSet().GetVideoMediaType())
        {
            if (m_ptMtTable->GetDialBitrate(tMt.GetMtId()) == m_tConf.GetBitRate())
            {
                byVmpChanNo = 1;
                wBasBitrate = m_wVidBasBitrate;
            }
            else
            {
                byVmpChanNo = 2;
                wBasBitrate = m_wBasBitrate;
            }
        }
        //���ٻ�˫��ʽ����
        else
        {
            if (tDstSimCapSet.GetVideoMediaType() == byMediaType)
            {
                byVmpChanNo = 1;
                wBasBitrate = m_wVidBasBitrate;
            }
            else
            {
                byVmpChanNo = 2;
                wBasBitrate = m_wBasBitrate;
            }
        }
        
        if (wMinBitRate != wBasBitrate)
        {
            ChangeVmpBitRate(wMinBitRate, byVmpChanNo);                
        }
        return;
    }   

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2,  "[ProcMtMcuBandwidthNotif] IsUseAdapter.%d, SrcMtId.%d, DstMtId.%d, wBitRate.%d, wMinBitRate.%d\n", 
			m_tConf.GetConfAttrb().IsUseAdapter(), tMtSrc.GetMtId(), tMt.GetMtId(), wRcvBandWidth, wMinBitRate );

    //����������,ֱ�ӽ�����������
	if( !m_tConf.GetConfAttrb().IsUseAdapter() )
	{
        if (!m_tDoubleStreamSrc.IsNull())
        {
            TLogicalChannel tLogicalChannel;         
            if (m_ptMtTable->GetMtLogicChnnl(tMtSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
            {
                if (!m_tVidBrdSrc.IsNull() && m_tVidBrdSrc.GetMtId() != tMtSrc.GetMtId())
                {
                    m_ptMtTable->SetMtSndBitrate(tMtSrc.GetMtId(), wMinBitRate);                            
                }
            }
        }

		AdjustMtVideoSrcBR( tMt.GetMtId(), wMinBitRate );
	}
    else
    {
        u16 wAdaptBitRate = 0;            

        //˫�ٻ���,��������ʱ,
        //˫���£�����µ��ն˽������ʵ��ڵڶ�����/2�����ֹ���ն˵���Ƶת��
        //��˫���£�����µ��ն˽������ʵ��ڵڶ����ʣ����ֹ���ն˵���Ƶת��
        //��������ת��
        if (0 != m_tConf.GetSecBitRate())
        {			
            u16 wFirstMinBitRate =  m_tConf.GetSecBitRate();
            if (!m_tDoubleStreamSrc.IsNull() && !m_tVidBrdSrc.IsNull())
            {
                wFirstMinBitRate = GetFstVideoBitrate(wFirstMinBitRate);
            }
            u16 wFabsBitrate;
            u16 wMainAudioBitrate = GetAudioBitrate(m_tConf.GetMainAudioMediaType());
            u16 wSecAudioBitrate = GetAudioBitrate(m_tConf.GetSecAudioMediaType());
            if (wMainAudioBitrate < wSecAudioBitrate)
            {
                wFabsBitrate = wSecAudioBitrate - wMainAudioBitrate;
            }
            else
            {
                wFabsBitrate = wMainAudioBitrate - wSecAudioBitrate;
            }
            if (wRcvBandWidth < wFirstMinBitRate - wFabsBitrate)
            {
/ *                     if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
                {
                   StopSwitchToSubMt( tMt.GetMtId(), MODE_VIDEO );
                    if( TRUE == bStartDoubleStream && !m_tVidBrdSrc.IsNull() )
                    {
                        g_cMpManager.StopSwitchToSubMt( tMt, MODE_VIDEO, TRUE  );
                    }
                }
                else
                {
                    g_cMpManager.StopSwitchToSubMt( tMt, MODE_VIDEO, TRUE  );
                    if( TRUE == bStartDoubleStream && !m_tVidBrdSrc.IsNull() )
                    {
                        StopSwitchToSubMt( tMt.GetMtId(), MODE_VIDEO );
                    }
                }* /
                
                return;
            }            
        }
        
        //˫��ʽ���������������ն˽���������
        if (IsMtNeedAdapt(ADAPT_TYPE_VID, tMt.GetMtId())  &&
            (IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate) || IsNeedCifAdp()))
        {
            if (wMinBitRate != m_wVidBasBitrate)
            {
                if (m_tConf.m_tStatus.IsVidAdapting())
                {
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif] change vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wVidBasBitrate);
                    ChangeAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
                else
                {
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif] start vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wVidBasBitrate);
                    StartAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }                   

                StartSwitchToSubMt(m_tVidBasEqp, m_byVidBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
            else if (m_tConf.m_tStatus.IsVidAdapting())//������Ҫ�����������ʣ�ֱ�ӽ�����������������mt(�������������ն˵Ľ������ʸ��͵����)
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif] switch vid bas to mt<%d>\n", tMt.GetMtId());
                StartSwitchToSubMt(m_tVidBasEqp, m_byVidBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
        }
        //˫�ٻ��齵�ٻ���٣���Ҫ���䣩
        else if (IsMtNeedAdapt(ADAPT_TYPE_BR, tMt.GetMtId()) &&
            IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
        {
            if (wMinBitRate != m_wBasBitrate)
            {
                if (m_tConf.m_tStatus.IsBrAdapting())
                {
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif]change br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wBasBitrate);
                    ChangeAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
                else
                {
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif]start br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wBasBitrate);
                    StartAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }                   
                
                StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
            else if (m_tConf.m_tStatus.IsBrAdapting())//������Ҫ�����������ʣ�ֱ�ӽ�����������������mt(�������������ն˵Ľ������ʸ��͵����)
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif] switch br bas to mt<%d>\n", tMt.GetMtId());
                StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
        }            
        //���ٵ�ԭʼ���ʣ��������Ϊ������
        else if (0 != m_tConf.GetSecBitRate())
        {
            StartSwitchToSubMt(tMtSrc, 0, tMt.GetMtId(), MODE_VIDEO);
        }
        //˫��ʽ�������������ն˽�����ƵԴ���ʣ����ܻὫ����ѹ�����
        else if (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() || IsNeedCifAdp())
        {
            AdjustMtVideoSrcBR(tMt.GetMtId(), wMinBitRate);
        }
    }*/

    return;
}

/*====================================================================
    ������      ��ProcMtMcuOtherMsg
    ����        ���ն�������Ϣ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
====================================================================*/
void CMcuVcInst::ProcMtMcuOtherMsg( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 bySrcMtId = cServMsg.GetSrcMtId();
    TMt tMt = m_ptMtTable->GetMt(bySrcMtId);

	switch( CurState() )
	{
	case STATE_ONGOING:

		//�Ƿ�������
		if( !m_tConfAllMtInfo.MtJoinedConf( bySrcMtId ))
		{
			return;
		}

		switch( pcMsg->event )
		{	 
            //��ϯ�ն˲�ѯ�ն�״̬Ӧ��
		case MT_MCU_GETMTSTATUS_ACK:	
            {
                TMt tChairMt = m_tConf.GetChairman();
                if(!tChairMt.IsNull())
                {
                    TMtStatus tMtStatus = *(TMtStatus *)cServMsg.GetMsgBody();
                    tMtStatus.SetMt(tMt);
                    cServMsg.SetMsgBody((u8*)&tMtStatus, sizeof(tMtStatus));
                    SendMsgToMt(tChairMt.GetMtId(), MCU_MT_GETMTSTATUS_ACK, cServMsg);
                }                
            }
			break;
			
		case MT_MCU_GETMTSTATUS_NACK:			
			break;       

        case MT_MCU_GETBITRATEINFO_ACK:
        case MT_MCU_GETBITRATEINFO_NACK:
            break;

        case MT_MCU_GETBITRATEINFO_NOTIF:
            {
                TMtBitrate tMtBitrate = *(TMtBitrate *)cServMsg.GetMsgBody();
                tMtBitrate.SetMt(tMt);
                cServMsg.SetMsgBody((u8*)&tMtBitrate, sizeof(tMtBitrate));
                SendMsgToAllMcs(MCU_MCS_MTBITRATE_NOTIF, cServMsg); 
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuOtherMsg] Mt<%d,%d> bitrate : %u, %u, %u, %u\n", 
                    tMt.GetMcuId(), tMt.GetMtId(), tMtBitrate.GetRecvBitRate(), tMtBitrate.GetSendBitRate(), 
                    tMtBitrate.GetH239RecvBitRate(), tMtBitrate.GetH239SendBitRate());
            }            
            break;

//         case MT_MCU_GETMTVERID_ACK:
//             {
//                 u8 byHWVerId = *(u8*)cServMsg.GetMsgBody();
//                 LPCSTR lpszSWVerId = (LPCSTR)(cServMsg.GetMsgBody() + sizeof(u8));
//                 m_ptMtTable->SetHWVerID(tMt.GetMtId(), byHWVerId);
//                 m_ptMtTable->SetSWVerID(tMt.GetMtId(), lpszSWVerId);
//             }
//             break;
            
        case MT_MCU_GETMTVERID_NACK:
            {
                //FIXME: �첽������ȡ�����Ǵ������ϱ�
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "[ProcMtMcuOtherMsg] Get mt.%d VerId failed due to reason.%d\n", tMt.GetMtId(), cServMsg.GetErrorCode() );
            }
            break;
            
		default:
			return;
		}

		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

    return;
}


/*====================================================================
    ������      ��ProcMcsMcuMtOperCmd
    ����        ���ն˲���ͳһ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/14    1.0         LI Yi         ����
	03/02/27    1.0         Qzj           �ն���ѯʹ������֧��
	04/03/08    3.0         ������        �޸�
    04/04/15    4.0         �ű���        ��ϯ�ն˾����¼�MCU֧��
====================================================================*/
void CMcuVcInst::ProcMcsMcuMtOperCmd( const CMessage * pcMsg )
{
	CServMsg   cServMsg( pcMsg->content, pcMsg->length );	
	TMt tMt        = *(TMt*)cServMsg.GetMsgBody();
    u8  byOpenFlag = *( cServMsg.GetMsgBody() + sizeof(TMt) );
    u8  bySrcMtId  =  cServMsg.GetSrcMtId();
	TLogicalChannel tLogicalChannel;
	u16	wEvent;
	u16 wBitRate;
    BOOL32 bDecordMute = FALSE;
    
	switch( CurState() )
	{
	case STATE_ONGOING:
		//���Է���ϯ�նˣ�nack
		if( bySrcMtId != 0 && bySrcMtId != m_tConf.GetChairman().GetMtId() )
		{
			SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
            //guzh [2008/03/27]
            return; 
		}

		//�Ƿ�������
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
		{
			return;
		}

		switch( pcMsg->event )
		{				
		case MCS_MCU_SETMTBITRATE_CMD:	  //�����������ն�����
			wEvent = MCU_MT_FLOWCONTROL_CMD;
			wBitRate = *(u16*)( cServMsg.GetMsgBody()+ sizeof(TMt) );
			m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE );
            tLogicalChannel.SetFlowControl( ntohs( wBitRate ) );
			cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "MCS set mt%d bitrate to %d !\n", tMt.GetMtId(), ntohs( wBitRate ) );
			break;

		case MT_MCU_MTMUTE_CMD:                    //��ϯ�ն�����MCU�����ն˾�������
			wEvent = MCU_MT_MTMUTE_CMD;           
            bDecordMute = TRUE;
			break;
		case MT_MCU_MTDUMB_CMD:                    //��ϯ�ն�����MCU�����ն���������
			wEvent = MCU_MT_MTDUMB_CMD;
            bDecordMute = FALSE;
			break;
		default:
			return;
		}

        if ( MT_MANU_KDC != m_ptMtTable->GetManuId(tMt.GetMtId()) &&
             ( MCU_MT_MTMUTE_CMD == wEvent || MCU_MT_MTDUMB_CMD == wEvent ) )
        {
            //������KEDA�նˣ�KEDAMCU��Tandberg������������
            AdjustKedaMcuAndTaideMuteInfo( &tMt, bDecordMute, byOpenFlag );          
        }
        else
        {
            cServMsg.SetDstMtId( tMt.GetMtId() );
            SendMsgToMt( tMt.GetMtId(), wEvent, cServMsg ); 
        }
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			             pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuMtOperReq
    ����        ���ն˲���ͳһ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/14    1.0         LI Yi         ����
	03/02/27    1.0         Qzj           �ն���ѯʹ������֧��
	04/04/02    3.0         ������        �޸�
====================================================================*/
void CMcuVcInst::ProcMcsMcuMtOperReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 bySrcMtId = cServMsg.GetSrcMtId();
	//u8  byMuteType;
	//u8  byMuteOpenFlag;
	u8  byMtNum = 0;
	u8  byIndex = 0;
	TMt* ptDstMt = NULL;
	u16	wEvent;
	TPollInfo tPollInfo;
	memset( &tPollInfo, 0, sizeof(tPollInfo) );
	TMt tDstMt = *(TMt*)cServMsg.GetMsgBody();

	switch( CurState() )
	{
	case STATE_ONGOING:
		//���Է���ϯ�նˣ�nack
		if( bySrcMtId != 0 && bySrcMtId != m_tConf.GetChairman().GetMtId() )
		{
			SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
		}

		//�Ƿ�������
		if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMcuIdx(), tDstMt.GetMtId() ) )
		{
			return;
		}

		if( pcMsg->event == MCS_MCU_MTAUDMUTE_REQ || 		
            pcMsg->event == MCS_MCU_EXMATRIX_GETPORT_REQ)
		{
			SendReplyBack( cServMsg, pcMsg->event+1 );
		}

		switch( pcMsg->event )
		{
            //�ն����þ���
		case MCS_MCU_MATRIX_SAVESCHEME_CMD:     //������󷽰�	
			wEvent = MCU_MT_MATRIX_SAVESCHEME_CMD;
			break;		

        case MCS_MCU_MATRIX_GETALLSCHEMES_CMD:  //���о��󷽰�����
            wEvent = MCU_MT_MATRIX_GETALLSCHEMES_CMD;
            break;

        case MCS_MCU_MATRIX_GETONESCHEME_CMD: //�������󷽰�����
            wEvent = MCU_MT_MATRIX_GETONESCHEME_CMD;
            break;

        case MCS_MCU_MATRIX_SETCURSCHEME_CMD://ָ����ǰ����
            wEvent = MCU_MT_MATRIX_SETCURSCHEME_CMD;
            break;

        case MCS_MCU_MATRIX_GETCURSCHEME_CMD://��ȡ��ǰ����
            wEvent = MCU_MT_MATRIX_GETCURSCHEME_CMD;
            break;
            
            //�ն����þ���
        case MCS_MCU_EXMATRIX_GETINFO_CMD:      //��ȡ�ն����þ�������
            wEvent = MCU_MT_EXMATRIX_GETINFO_CMD;
            break;
        case MCS_MCU_EXMATRIX_SETPORT_CMD:      //�������þ������Ӷ˿ں�
            wEvent = MCU_MT_EXMATRIX_SETPORT_CMD;
            break;
        case MCS_MCU_EXMATRIX_GETPORT_REQ:      //�����ȡ���þ������Ӷ˿�
            wEvent = MCU_MT_EXMATRIX_GETPORT_REQ;
            break;
        case MCS_MCU_EXMATRIX_SETPORTNAME_CMD:  //�������þ������Ӷ˿���
            wEvent = MCU_MT_EXMATRIX_SETPORTNAME_CMD;
            break;
        case MCS_MCU_EXMATRIX_GETALLPORTNAME_CMD://�����ȡ���þ�������ж˿���
            wEvent = MCU_MT_EXMATRIX_GETALLPORTNAME_CMD;
            break;

            //�ն���չ��ƵԴ
        case MCS_MCU_GETVIDEOSOURCEINFO_CMD:
            wEvent = MCU_MT_GETVIDEOSOURCEINFO_CMD;
            break;
        case MCS_MCU_SETVIDEOSOURCEINFO_CMD:
            wEvent = MCU_MT_SETVIDEOSOURCEINFO_CMD;
            break;
            
            //�ն��л���չ��ƵԴ
        case MCS_MCU_SELECTEXVIDEOSRC_CMD:
            wEvent = MCU_MT_SELECTEXVIDEOSRC_CMD;
            break;

		case MCS_MCU_MTAUDMUTE_REQ:             //���Ҫ��MCU�����ն˾���
			ProcMtMuteDumbReq(cServMsg);
			return;

		default:
			return;
		}

		//send messge
		cServMsg.SetDstMtId( tDstMt.GetMtId() );
		if( pcMsg->event != MCS_MCU_MTAUDMUTE_REQ )
		{
			CServMsg cTempMsg = cServMsg;
			cServMsg.SetMsgBody( cTempMsg.GetMsgBody() + sizeof(TMt), cTempMsg.GetMsgBodyLen() - sizeof(TMt) );
		}

		SendMsgToMt( tDstMt.GetMtId(), wEvent, cServMsg );
		
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
������      ��ProcSingleMtSmsOpr
����        ���Ե����ն˽��ж���Ϣ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵���� 
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
 10/08/12    4.6			XL			create
====================================================================*/
void CMcuVcInst::ProcSingleMtSmsOpr(const CServMsg &cServMsg, TMt *ptDstMt, CRollMsg *ptROLLMSG)
{
	if( ptDstMt == NULL )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcSingleMtSmsOpr] param-ptDstMt is NULL, return\n");
		return;
	}

	TMt tMt = *ptDstMt;
	BOOL32 bIsLocal = FALSE;

	TMsgHeadMsg tHeadMsg;
	memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );

	ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "tmt info: GetMcuIdx = %d, GetMcuId = %d, GetMtId = %d, GetMtType = %d\n",
		tMt.GetMcuIdx(), tMt.GetMcuId(), tMt.GetMtId(), tMt.GetMtType());

	if(cServMsg.GetEventId() == MCU_MCU_SENDMSG_NOTIF)
	{
		bIsLocal = tMt.IsMcuIdLocal();
	}
	else
	{
		bIsLocal = tMt.IsLocal();
	}
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT, "[ProcSingleMtSmsOpr] bLocal is %d!\n", bIsLocal);

	if(!bIsLocal)
	{
		
		if(cServMsg.GetEventId() != MCU_MCU_SENDMSG_NOTIF)
		{
			memset( &tHeadMsg, 0, sizeof(TMsgHeadMsg) );
			tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt, tMt );
			//tMt = tHeadMsg.m_tMsgDst.m_tMt;
										
		}
		else
		{
			tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody() + sizeof(TMt) + ptROLLMSG->GetTotalMsgLen() );
			//tHeadMsg.m_tMsgDst.m_tMt = tMt;

			// if ack is needed, build HeadMsgAck info here�� 
			//tHeadMsgAck.m_tMsgSrc = tHeadMsg.m_tMsgDst;
			//tHeadMsgAck.m_tMsgDst = tHeadMsg.m_tMsgSrc;
		}
		
		CServMsg cMsg;
		cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
		cMsg.CatMsgBody( (u8*)&tMt, sizeof(TMt) );
		cMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );

		
		SendMsgToMt( (u8)tMt.GetMcuId(), MCU_MCU_SENDMSG_NOTIF, cMsg );

	}
	else
	{
		//local�Ĵ���
		CServMsg cSendMsg;
		TMt tDstMt = m_ptMtTable->GetMt( tMt.GetMtId() );
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT,  "tmt in mtTable --mttype is: %u; tmt-- mttype is: %u\n", tDstMt.GetMtType(), tMt.GetMtType());
		if(MT_TYPE_MT == tDstMt.GetMtType() )
		{
			// �����ն�

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT,  "MCU_MT_SENDMSG_NOTIF to mt.(%d,%d)\n", tDstMt.GetMcuId(), tDstMt.GetMtId());
			cSendMsg.SetMsgBody( (u8*)ptDstMt, sizeof(TMt) );
			cSendMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );
			SendMsgToMt( tMt.GetMtId(), MCU_MT_SENDMSG_NOTIF, cSendMsg );
		}
		else
		{
			// ����mcu
			TMt tMtNull;
			tMtNull.SetNull();
			//tHeadMsg.m_tMsgDst.m_tMt = tMtNull;
			cSendMsg.SetMsgBody( (u8*)&tHeadMsg, sizeof(TMsgHeadMsg) );
			cSendMsg.CatMsgBody( (u8*)&tMtNull, sizeof(TMt) );
			cSendMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT,  "MCU_MT_SENDMSG_NOTIF to mcu.(%d,%d)\n",tDstMt.GetMcuId(), tDstMt.GetMtId());
			SendMsgToMt( tMt.GetMtId(), MCU_MCU_SENDMSG_NOTIF, cSendMsg );
		}
	}

	return;
}
/*====================================================================
������      ��ProcSingleMtMuteDumbOpr
����        ���Ե����ն˽��о�����������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵���� 
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
  10/08/06      4.6			XL			  ����
====================================================================*/
void CMcuVcInst::ProcSingleMtMuteDumbOpr(CServMsg &cServMsg, TMt *ptDstMt, u8 byMuteOpenFlag, u8 byMuteType)
{
	if (CurState() != STATE_ONGOING)
	{
		return;
	}
		
	if( ptDstMt == NULL )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcSingleMtMuteDumbOpr] param-ptDstMt is NULL, return\n");
		return;
	}

	TMt tMt = *ptDstMt;
	BOOL32 bIsLocal = FALSE;
	//u16 wMcuIdx;
	//u8  byMtId;
	TMsgHeadMsg tHeadMsg;
	memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT, "tmt info: GetMcuIdx = %d, GetMcuId = %d, GetMtId = %d\n",
		tMt.GetMcuIdx(), tMt.GetMcuId(), tMt.GetMtId() );

	// [9/21/2011 liuxu] vcs�����ϼ������¼�������
	if ( tMt.IsNull() 
		 && VCS_CONF == m_tConf.GetConfSource()
		 && cServMsg.GetSrcMtId() == m_tCascadeMMCU.GetMtId())
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "mmcu req to mute all\n");
		CServMsg cMsg;
		memset(&cMsg, 0, sizeof(CServMsg));
		cMsg.SetEventId(VCS_MCU_MUTE_REQ);

		u8 byMode = m_cVCSConfStatus.GetCurVCMode();
		cMsg.SetMsgBody(&byMode, sizeof(u8));
		cMsg.CatMsgBody(&byMuteOpenFlag, sizeof(u8));
		
		u8  byOprObj = VCS_OPR_REMOTE;
		cMsg.CatMsgBody(&byOprObj, sizeof(u8));
		cMsg.CatMsgBody(&byMuteType, sizeof(u8));

		u8 byFromMcuFlag = 1;
		cMsg.CatMsgBody(&byFromMcuFlag, sizeof(u8));

		ProcVcsMcuMuteReq(cMsg);
		return;		
	}

	if(cServMsg.GetEventId() == MCU_MCU_MTAUDMUTE_REQ)
	{
		bIsLocal = tMt.IsMcuIdLocal();
		//wMcuIdx = GetMcuIdxFromMcuId( (u8)tMt.GetMcuId() );
		//byMtId = tMt.GetMtId();
	}
	else
	{
		bIsLocal = tMt.IsLocal();
		//wMcuIdx = tMt.GetMcuIdx();
		//byMtId = tMt.GetMtId();
	}
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT, "[ProcSingleMtMuteDumbOpr] bLocal is %d!\n", bIsLocal);

	if(!bIsLocal)
	{
		if(!IsSupportMultCas(tMt.GetMcuId()))
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "tMt(mcuid:%d, mtId:%d)not support multicas!\n", tMt.GetMcuId(), tMt.GetMtId());
			NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_OPRINVALID_NOTSUPPORTMULTICASCADE);
			return;
		}
		if(cServMsg.GetEventId() != MCU_MCU_MTAUDMUTE_REQ)
		{
			memset( &tHeadMsg, 0, sizeof(TMsgHeadMsg) );
			tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt, tMt );
			//tMt = tHeadMsg.m_tMsgDst.m_tMt;
										
		}
		else
		{
			tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody() );
			//tHeadMsg.m_tMsgDst.m_tMt = tMt;

			// if ack is needed, build HeadMsgAck info here�� 
			//tHeadMsgAck.m_tMsgSrc = tHeadMsg.m_tMsgDst;
			//tHeadMsgAck.m_tMsgDst = tHeadMsg.m_tMsgSrc;
		}
		
		CServMsg cMsg;
		cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
		cMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		cMsg.CatMsgBody( (u8 *)&byMuteOpenFlag, sizeof(byMuteOpenFlag));
		cMsg.CatMsgBody( (u8 *)&byMuteType, sizeof(byMuteType));
		
		SendMsgToMt( (u8)tMt.GetMcuId(), MCU_MCU_MTAUDMUTE_REQ, cMsg );
		
// 		if(cServMsg.GetEventId() == MCU_MCU_XXXREQ)
// 		{
// 			tMt = m_ptMtTable->GetMt( (u8)tMt.GetMcuId() );
// 			tHeadMsgAck.m_tMsgSrc.m_tMt = tMt;
// 			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
// 		}
		//SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

		return;
	}
	
	//local�Ĵ���
	u8 byVendor = m_ptMtTable->GetManuId(ptDstMt->GetMtId());
	if(byVendor == MT_MANU_KDC)
	{
		u16 wEvent;
		CServMsg cMsg;
		if( byMuteType == 1 )
		{
			wEvent = MCU_MT_MTMUTE_CMD;
		}
		else
		{
			wEvent = MCU_MT_MTDUMB_CMD;
		}	
		cMsg.SetMsgBody( (u8*)ptDstMt, sizeof(TMt) );
		cMsg.CatMsgBody( &byMuteOpenFlag, sizeof(byMuteOpenFlag) );
		cMsg.SetDstMtId( ptDstMt->GetMtId() );
		SendMsgToMt( ptDstMt->GetMtId(), wEvent, cMsg );
	}
	else
	{                    
		// ��� byMuteType: 2������, 1������
		// �տ� byMuteType: 0������, 1������ ---- ����һ�£�ͳһ����
		if ( 2 == byMuteType )
		{
			byMuteType = 0;
		}
		
		//������KEDA�նˣ�KEDAMCU��Tandberg������������
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcSingleMtMuteDumbOpr]Call AdjustKedaMcuAndTaideMuteInfo here!\n");
		AdjustKedaMcuAndTaideMuteInfo( ptDstMt, byMuteType, byMuteOpenFlag );
	}

}

/*====================================================================
    ������      ProcMtMuteDumbReq
    ����        �����ն˽��о�����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CServMsg &cServMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
  10/08/06      4.6			XL			  ����
====================================================================*/
void CMcuVcInst::ProcMtMuteDumbReq(CServMsg &cServMsg)
{

	u8  byMuteType;
	u8  byMuteOpenFlag;
	u8  byMtNum = 0;
	u8  byIndex = 0;
	TMt* ptDstMt = NULL;

	//u8 bySrcMtId = cServMsg.GetSrcMtId();
	//TMt tDstMt = *(TMt*)cServMsg.GetMsgBody();

	//��ͬʱ������mt��/��������
	byMtNum = cServMsg.GetMsgBodyLen() / (sizeof(TMt)+2);
	for( byIndex=0; byIndex<byMtNum; byIndex++ )
	{
		
		ptDstMt = (TMt*)( cServMsg.GetMsgBody() + byIndex*(sizeof(TMt)+2) );
		byMuteOpenFlag = *( (u8*)ptDstMt + sizeof(TMt) );
		byMuteType     = *( (u8*)ptDstMt + sizeof(TMt) + 1 );

		ProcSingleMtMuteDumbOpr(cServMsg, ptDstMt, byMuteOpenFlag, byMuteType);
		
	}
}
/*====================================================================
    ������      ��ProcMcsMcuCallMtReq
    ����        ����غ����ն�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
  03/12/09      3.0			JQL			  ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuCallMtReq( const CMessage * pcMsg )
{
	STATECHECK
		
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt;
	TMsgHeadMsg tHeadMsg,tHeadMsgAck;
	BOOL32 bIsLocal = TRUE;
	u16 wMcuIdx = INVALID_MCUIDX;
	u8 byMtId = 0;
	if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ)
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
		//m_tConfAllMcuInfo.GetIdxByMcuId( tMt.GetMcuId(),0,&wMcuIdx );
		byMtId = tMt.GetMtId();
	}
	else
	{
		tMt = *( TMt * )cServMsg.GetMsgBody();
		bIsLocal = tMt.IsLocal();
		wMcuIdx = tMt.GetMcuIdx();
		byMtId = tMt.GetMtId();
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuCallMtReq] Call Mt(%d.%d.%d) level.%d\n",wMcuIdx,byMtId,
						tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],tHeadMsg.m_tMsgDst.m_byCasLevel 
						);

	

	//���������ն��б���, NACK!
    TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tMt.GetMcuId());
	//�������û��
	if(tMtInfo.IsNull())
	{
		if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ)
		{
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
		}
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//�Ѿ��������,NACK
	if( tMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{	
		if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ)
		{
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
		}
		cServMsg.SetErrorCode( ERR_MCU_CALLEDMT_JOINEDCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//���ڱ�MCU��
	if( !bIsLocal )
	{
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
			sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
		astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
			sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));
		if(cServMsg.GetEventId() != MCU_MCU_REINVITEMT_REQ)
		{
			//u8 byFstMcuId = 0,bySecMcuId = 0;
			u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
			memset( &abyMcuId[0],0,sizeof( abyMcuId ) );
			memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
			tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
			/*tMt = tHeadMsg.m_tMsgDst.m_tMt;
			if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
			{			
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
		cServMsg.CatMsgBody((u8 *)&tMt, sizeof(tMt));		
		cServMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
		//SendMsgToMt( tMt.GetMcuId(), MCU_MCU_REINVITEMT_REQ, cServMsg );
		SendMsgToMt( GetFstMcuIdFromMcuIdx(wMcuIdx), MCU_MCU_REINVITEMT_REQ, cServMsg );
		
		if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ)
		{
			//tMt = m_ptMtTable->GetMt( (u8)tMt.GetMcuId() );
			//tHeadMsgAck.m_tMsgSrc.m_tMt = tMt;
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
			cServMsg.CatMsgBody( (u8*)&tMt,sizeof( tMt ) );
		}		
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
	}

	if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ)
	{		
		//tHeadMsgAck.m_tMsgSrc.m_tMt = tMt;
		cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
		cServMsg.CatMsgBody( (u8*)&tMt,sizeof( tMt ) );
	}
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
 
	//zjj20100928 ���´������弰�䲻��ȷ,��ע��
	//ɾ�����ն�
	/*if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ &&
	   // vcs��������Դɾ��������ֻ���к��С��Ҷϲ���
	   // ɾ���������¼�PU��IP��������Ϣ��ɾ�������ں�����callalertͨ�����޷����������PU������Ϣ
	   m_tConf.GetConfSource() != VCS_CONF)
	{
		cServMsg.SetMsgBody((u8 *)&tMt, sizeof(tMt));
		SendReplyBack( cServMsg, MCU_MCU_DELMT_NOTIF );
	}*/

	//�����ն�
	if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ)
	{
		//���غ��ļ���mcu���ͺ���֪ͨ
		if (VCS_CONF == m_tConf.GetConfSource())
		{
	 		InviteUnjoinedMt( cServMsg, &tMt, FALSE, FALSE );
		}
		else
		{
			InviteUnjoinedMt( cServMsg, &tMt, TRUE, FALSE );
		}
	}
	else
	{
		InviteUnjoinedMt( cServMsg, &tMt );
	}

	return;
}

/*====================================================================
    ������      ��ProcMcsMcuDropMtReq
    ����        ����عҶ��ն�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
  03/12/09      3.0			JQL			  ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuDropMtReq( const CMessage * pcMsg )
{
	STATECHECK

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt;
	TMsgHeadMsg tHeadMsg,tHeadMsgAck;
	BOOL32 bIsLocal = TRUE;
	u16 wMcuIdx = INVALID_MCUIDX;
	u8 byMtId = 0;
	if(cServMsg.GetEventId() == MCU_MCU_DROPMT_REQ)
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
		//m_tConfAllMcuInfo.GetIdxByMcuId( tMt.GetMcuId(),0,&wMcuIdx );
		byMtId = tMt.GetMtId();
	}
	else
	{
		tMt = *( TMt * )cServMsg.GetMsgBody();
		bIsLocal = tMt.IsLocal();
		wMcuIdx = tMt.GetMcuIdx();
		byMtId = tMt.GetMtId();
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuDropMtReq] Drop Mt(%d.%d.%d) level.%d\n",wMcuIdx,byMtId,
					tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],tHeadMsg.m_tMsgDst.m_byCasLevel 
					);

	//���������ն��б���, NACK!
	TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tMt.GetMcuId());
	//�������û��
	if(tMtInfo.IsNull())
	{
		if(cServMsg.GetEventId() == MCU_MCU_DROPMT_REQ)
		{
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
		}
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//�Ѿ����ڻ�����,NACK
	if( !tMtInfo.MtJoinedConf( byMtId ) )//tMt.GetMtId()
	{	
		if(cServMsg.GetEventId() == MCU_MCU_DROPMT_REQ)
		{
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
		}
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//���ն��Ƿ�ֱ���ն�
	if( !bIsLocal )//!tMt.IsLocal())
	{
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy( tReq.m_szUserName, tMcsReg.m_achUser, 
				  sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser) );
		astrncpy( tReq.m_szUserPwd, tMcsReg.m_achPwd, 
				  sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd) );

		if(cServMsg.GetEventId() != MCU_MCU_DROPMT_REQ)
		{
			//u8 byFstMcuId = 0,bySecMcuId = 0;
			u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
			memset( &abyMcuId[0],0,sizeof(abyMcuId) );
			memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
			tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
			/*tMt = tHeadMsg.m_tMsgDst.m_tMt;
			if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
			{			
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
			}	*/								
		}
		/*else
		{
			tHeadMsg.m_tMsgDst.m_tMt = tMt;
		}*/
		
		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		cServMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
		
		SendMsgToMt( (u8)tMt.GetMcuId(), MCU_MCU_DROPMT_REQ, cServMsg );

		if(cServMsg.GetEventId() == MCU_MCU_DROPMT_REQ)
		{
			//tMt = m_ptMtTable->GetMt( (u8)tMt.GetMcuId() );
			//tHeadMsgAck.m_tMsgSrc.m_tMt = tMt;
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
			cServMsg.CatMsgBody( (u8*)&tMt,sizeof( tMt ) );
		}
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

		return;
	}
	if(cServMsg.GetEventId() == MCU_MCU_DROPMT_REQ)
	{
		//tHeadMsgAck.m_tMsgSrc.m_tMt = tMt;
		cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
		cServMsg.CatMsgBody( (u8*)&tMt,sizeof( tMt ) );
	}
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	tMt = m_ptMtTable->GetMt(tMt.GetMtId());		
	//�����ն˺���ģʽ
	// ����VCS�������ģʽ��������ĺ���(���̶ֹ���Դ�Ϳɵ�����Դ)���ɸ���
	if (m_tConf.GetConfSource() != VCS_CONF)
	{
		m_ptMtTable->SetCallMode( tMt.GetMtId(),  CONF_CALLMODE_NONE);
	}
	else
	{
		//zjj20100329 vcs���飬�¼��ֶ��ҶϺ�����Ϊ�ֶ�����
		if( !( tMt==m_tConf.GetChairman() ) &&
			!(tMt == m_cVCSConfStatus.GetVcsBackupChairMan())
			)//&& CONF_CREATE_MT == m_byCreateBy)
		{
			m_ptMtTable->SetCallMode( tMt.GetMtId(),  CONF_CALLMODE_NONE);
			//zjj20100327
			TMtStatus tMtStatus;
			if( m_ptMtTable->GetMtStatus(tMt.GetMtId(),&tMtStatus) )
			{
				tMtStatus.SetMtDisconnectReason( MTLEFT_REASON_NORMAL );
				m_ptMtTable->SetMtStatus( tMt.GetMtId(),&tMtStatus );
			}
		}
		//vcs���鲻��ɾ����ϯ
		if( tMt == m_tConf.GetChairman() ||
			tMt == m_cVCSConfStatus.GetVcsBackupChairMan() )
		{
			return;
		}
	}

    cServMsg.SetMsgBody((u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt));
    SendMsgToAllMcs(MCU_MCS_CALLMTMODE_NOTIF, cServMsg);
		
	RemoveJoinedMt( tMt, TRUE );		
}

/*====================================================================
    ������      ��ProcMcuMcuSetCallMtModeReq
    ����        �������ն˺���ģʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
  03/12/09      3.0			JQL			  ����
====================================================================*/
void CMcuVcInst::ProcMcsMcuSetCallMtModeReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = *( TMt * )cServMsg.GetMsgBody();
	u8			byCallMode = *( u8* )( cServMsg.GetMsgBody() + sizeof(TMt) );

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		if( !m_tConfAllMtInfo.MtInConf( tMt.GetMtId() ) )	//not in conference
		{
			cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}

		//��Ч�Ļ�����в���,�ܾ�
		if( m_tConf.m_tStatus.GetCallMode() > CONF_CALLMODE_TIMER )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_CALLMODE );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
		
		m_ptMtTable->SetCallMode( tMt.GetMtId(), byCallMode );
		if( CONF_CALLMODE_TIMER == byCallMode )
		{
			m_ptMtTable->SetCallLeftTimes( tMt.GetMtId(), m_tConf.m_tStatus.GetCallTimes() );
		}
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		
	    cServMsg.SetMsgBody((u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt));
        SendMsgToAllMcs(MCU_MCS_CALLMTMODE_NOTIF, cServMsg);	
		//sgx20100703[Bug00033516]���ϼ�mcu���ͱ���ֱ��ĳ�ն˵�״̬����
		OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(),tMt.GetMtId());

		break;
		
	case STATE_SCHEDULED:
		break;
	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
		
}

/*====================================================================
    ������      ��ProcMtMcuFastUpDate
    ����        ���ն�����ؼ�֡
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/06/03      3.0			������		  ����
====================================================================*/
void CMcuVcInst::ProcMtMcuFastUpdatePic( const CMessage * pcMsg )
{
	STATECHECK

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	u8 bySrcMtId = cServMsg.GetSrcMtId();
	u8 byMode    = *(cServMsg.GetMsgBody());

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

    // ����ؼ�֡���ն�, zgc, 2008-08-15
    TMt tMt = m_ptMtTable->GetMt( bySrcMtId );
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "Mt%u req KF.%u\n", bySrcMtId, byMode );

	TMt  tSrcMt;
	tSrcMt.SetNull();
	//��ȡ�����ն˵�Դ��Ϣ
	BOOL32 bRet = m_ptMtTable->GetMtSrc(bySrcMtId, &tSrcMt, byMode);

	// [pengjie 2010/9/7] �����ն˱�����û�м�¼���ն˵�˫����ƵԴ������������˫���Ͳ���return��
	// �ں�����˫��Դ�Ƿ�Ϊ�գ�Ϊ����return
	if (!bRet && byMode != MODE_SECVIDEO)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuFastUpdatePic] GetMtSrc failed!\n");
		return;
	}

	switch (byMode)
	{
	case  MODE_AUDIO:
		{
			//�Ƿ�Ҫ������
			// [pengjie 2010/11/3] �ϼ�mcu������ؼ�֡Ӧ����ѡ�������䣩
			if (tSrcMt == GetVidBrdSrc() && !(m_tCascadeMMCU == tMt))
			{
				if (IsNeedAdapt(tSrcMt, tMt, MODE_AUDIO))
				{
					TBasOutInfo tOutInfo;
					//bas��ؼ�֡
					if(FindBasChn2BrdForMt(tMt.GetMtId(), MODE_AUDIO, tOutInfo))
					{
						NotifyFastUpdate(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId);
					}
				}	
				else
				{
					//֪ͨ�ն˱�ؼ�֡
					NotifyFastUpdate(tSrcMt, MODE_AUDIO);
				}
			}			
			else
			{
				//ѡ���Ժ�
				//ѡ���Ժ�
			//[nizhijun 2010/11/10] ���ڼ������飬�ϼ�ѡ���¼�MCU����Ϊ�¼���GetSelectMt���ܻ��selectsrc
			//	TMtStatus tMtStatus;
			//	m_ptMtTable->GetMtStatus(bySrcMtId, &tMtStatus);
			//	TMt tSelectSrc = tMtStatus.GetSelectMt(MODE_AUDIO);
				if (!tSrcMt.IsNull())
				{
					if (IsNeedSelAdpt(tSrcMt, tMt, MODE_AUDIO))
					{
						TBasOutInfo tOutInfo;
						if(FindBasChn2SelForMt(tSrcMt, tMt, MODE_AUDIO,tOutInfo))
						{
							NotifyFastUpdate(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId);
						}
					}
					else
					{
						//֪ͨ�ն˱�ؼ�֡
						NotifyFastUpdate(tSrcMt, MODE_AUDIO);
					}
				}
				else
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic:Aud] Mt(McuId:%d, MtId:%d)'s SelectSrc is null!\n",
						tSrcMt.GetMcuId(), 	tSrcMt.GetMtId());
				}
			}
		}
		break;
	case MODE_VIDEO:
		{
			BOOL32 bNeglect = FALSE;
			
			if( VCS_CONF == m_tConf.GetConfSource() &&
				m_byCreateBy != CONF_CREATE_MT &&
				m_tConf.GetChairman().GetMtId() == bySrcMtId )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic] bySrcMtId.%d is chariman,so not neglect fastupdate\n",bySrcMtId );
				bNeglect = TRUE;
			}

			//zjj20120309 �ֻ�ʱ���ں��Թؼ�֡����fix2����ϲ�
			if( !bNeglect && 0 != g_cMcuVcApp.GetMtFastUpdateNeglectNum() &&
				( ( TYPE_MCUPERI == tSrcMt.GetType() &&
						EQP_TYPE_VMP == tSrcMt.GetEqpType() && g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) ) ||
					( tSrcMt == GetVidBrdSrc() && !(m_tCascadeMMCU == tMt) ) 
					)
				)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic] In FastUpdate Interval Logical OspClkRateGet().%d\n",OspClkRateGet() );
				if( 0 == m_dwBrdFastUpdateTick )
				{
					m_dwBrdFastUpdateTick = OspTickGet();
					m_byFastNeglectNum = 1;							
				}
				else
				{
					u32 dwTickCount = OspTickGet();
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic] m_dwBrdFastUpdateTick.%d minus dwTickCount.%d is lager than OspClkRateGet(%d) * g_cMcuVcApp.GetMtFastUpdateInterval(%d) neglect.%d m_byFastNeglectNum.%d\n",
									m_dwBrdFastUpdateTick,dwTickCount,OspClkRateGet(),g_cMcuVcApp.GetMtFastUpdateInterval(),
									g_cMcuVcApp.GetMtFastUpdateNeglectNum(),m_byFastNeglectNum
									);
					if( ( dwTickCount - m_dwBrdFastUpdateTick ) > ( OspClkRateGet() * g_cMcuVcApp.GetMtFastUpdateInterval() )						
						)
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic] Clear FastUpdateTick\n" );
						m_dwBrdFastUpdateTick = OspTickGet();
						m_byFastNeglectNum = 1;						
					}
					else
					{
						if( m_byFastNeglectNum++ > g_cMcuVcApp.GetMtFastUpdateNeglectNum() )
						{
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic] FastNeglectNum(%d) is large.Fail to fast update pic\n",m_byFastNeglectNum );
							return;
						}
						
					}
				}
			}			

		   // pengjie ��������ϳ��¼�����ؼ�֡���⣬���л���ϳ�
			if (TYPE_MCUPERI == tSrcMt.GetType() &&
				EQP_TYPE_VMP == tSrcMt.GetEqpType())
			{
				// ��������1·

				u8 bySrcChnnl = GetVmpOutChnnlByDstMtId( tMt.GetMtId(), tSrcMt.GetEqpId());
				if (0xFF == bySrcChnnl) 
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic] get mt.%d vmp out chnnl failed!\n", 
					tMt.GetMtId() );
					return;
				}
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic] Ask VMP.%d (chnnl.%d) to setfastupdate!\n", 
					tSrcMt.GetEqpId(), bySrcChnnl);
				NotifyFastUpdate(tSrcMt, bySrcChnnl); 
				
				return;
			}

			//�Ƿ�Ҫ������
			// [pengjie 2010/11/3] �ϼ�mcu������ؼ�֡Ӧ����ѡ�������䣩
			if (tSrcMt == GetVidBrdSrc() && !(m_tCascadeMMCU == tMt))
			{
				if (IsNeedAdapt(tSrcMt, tMt, MODE_VIDEO))
				{
					TBasOutInfo tOutInfo;
					//bas��ؼ�֡
					if(FindBasChn2BrdForMt(tMt.GetMtId(), MODE_VIDEO, tOutInfo))
					{
						NotifyFastUpdate(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId);
					}
				}				
				else
				{
					//֪ͨ�ն˱�ؼ�֡
					NotifyFastUpdate(tSrcMt, MODE_VIDEO);
				}
			}			
			else
			{
				//ѡ���Ժ�
			//[nizhijun 2010/11/10] ���ڼ������飬�ϼ�ѡ���¼�MCU����Ϊ�¼���GetSelectMt���ܻ��selectsrc
			//	TMtStatus tMtStatus;
			//	m_ptMtTable->GetMtStatus(bySrcMtId, &tMtStatus);
			//	TMt tSelectSrc = tMtStatus.GetSelectMt(MODE_VIDEO);
				if (!tSrcMt.IsNull())
				{
					if (IsNeedSelAdpt(tSrcMt, tMt, MODE_VIDEO))
					{
						TBasOutInfo tOutInfo;
						if(FindBasChn2SelForMt(tSrcMt, tMt, MODE_VIDEO, tOutInfo))
						{
							NotifyFastUpdate(tOutInfo.m_tBasEqp,tOutInfo.m_byChnId);
						}
					}
					else
					{
						//֪ͨ�ն˱�ؼ�֡
						NotifyFastUpdate(tSrcMt, MODE_VIDEO);
					}
				}
				else
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic:Vid] Mt(McuId:%d, MtId:%d)'s SelectSrc is null!\n",
									tSrcMt.GetMcuId(), 	tSrcMt.GetMtId());
				}
			}
		}
		break;
	case MODE_SECVIDEO:
		{
			tSrcMt = m_ptMtTable->GetMtSelMtByMode(tMt.GetMtId(), MODE_VIDEO2SECOND);
			
			//˫ѡ��
			if (!tSrcMt.IsNull())
			{
				if (IsNeedSelAdpt(tSrcMt, tMt, MODE_VIDEO2SECOND))
				{
					TBasOutInfo	tOutInfo;
					//bas��ؼ�֡
					if(FindBasChn2SelForMt(tSrcMt, tMt, MODE_VIDEO2SECOND, tOutInfo))
					{
						NotifyFastUpdate(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId);
					}
				}
				else
				{
					NotifyFastUpdate(tSrcMt, MODE_VIDEO);
				}
				return;
			}
			
			//�Ƿ�Ҫ������
			if (!m_tDoubleStreamSrc.IsNull())
			{
				if (IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO))
				{
					TBasOutInfo	tOutInfo;
					//bas��ؼ�֡
					if(FindBasChn2BrdForMt(tMt.GetMtId(), MODE_SECVIDEO, tOutInfo))
					{
						NotifyFastUpdate(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId);
					}
				}	
				else
				{
					//֪ͨ�ն˱�ؼ�֡
					NotifyFastUpdate(m_tDoubleStreamSrc, MODE_SECVIDEO);
				}
			}
			else if (!m_tSecVidBrdSrc.IsNull())
			{
				TBasOutInfo tBasOutInfo;
				if (m_tConf.GetConfAttrb().IsUseAdapter() 
					&& IsNeedAdapt(m_tSecVidBrdSrc, tMt, MODE_VIDEO2SECOND)
					&& FindBasChn2BrdForMt(tMt.GetMtId(), MODE_VIDEO2SECOND, tBasOutInfo))
				{
				}	
				else
				{
					NotifyFastUpdate(m_tSecVidBrdSrc, MODE_VIDEO);
				}
			}
			else
			{
				//ѡ���Ժ�
			}
		}

		break;
	default:
		break;

	}

}

/*====================================================================
    ������      ��ProcMtMcuMixReq
    ����        ������mt->mcu��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
  05/03/08      3.6			Jason		  ����
====================================================================*/
void CMcuVcInst::ProcMtMcuMixReq( const CMessage * pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );

    STATECHECK

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
	
	// [11/10/2010 xueliang] translate mt msg
	if( MT_MCU_STARTDISCUSS_REQ == cServMsg.GetEventId() )
	{
		u8 byMtNum = (cServMsg.GetMsgBodyLen()-  sizeof(u8)) / sizeof(TMt);
		if (byMtNum != 0)
		{
			CServMsg cTransMsg = cServMsg;
			u8 byDissMtNum = (cServMsg.GetMsgBodyLen()-  sizeof(u8)) / sizeof(TMt);
			cTransMsg.SetMsgBody((u8*)&byDissMtNum, sizeof(byDissMtNum));
			TMt tMt;
			for( u8 bylp = 0; bylp < byMtNum; bylp ++ )
			{
				TMt *ptMt = (TMt *)(cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TMt) * bylp);
				if (ptMt == NULL)
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MIXER, "[ProcMtMcuMixReq]ptMt == NULL,So Return!\n");
					return;
				}
				tMt = *ptMt;
				tMt.SetMcuId(m_ptMtTable->GetMt(tMt.GetMtId()).GetMcuId());
				cTransMsg.CatMsgBody((u8*)&tMt, sizeof(TMt));
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, "[ProcMtMcuMixReq] tmt after trans is (%d,%d)\n", tMt.GetMcuId(), tMt.GetMtId());
			}
			cServMsg = cTransMsg;
		}
	}

	switch(pcMsg->event)
	{
	case MT_MCU_STARTDISCUSS_REQ:      //�ն˿�ʼ������������
		{
            ProcMixStart(cServMsg);
			break;
		}
    case MT_MCU_STOPDISCUSS_REQ:      //�������̨����MCU������������
		{
            ProcMixStop(cServMsg);
			break;
		}
	 default:	break;
	}
}

/*=============================================================================
    �� �� ���� StopDoubleStream
    ��    �ܣ� 
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� BOOL32 bMsgSrcMt �Ƿ�֪ͨ˫��Դ�ر�ͨ����mcu�����ر�ͨ��ʱʹ��
	           BOOL32 bForceClose �Ƿ�ǿ��˫��Դ�ر�ͨ�����˳�����ʱʹ��
			   BOOL32 bClearLastDsInfo  �Ƿ�����ϴ�˫��Դ����Ϣ(m_byLastDsSrcMtId �� m_tLogicChnnl)
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/02/19  3.6			����                  ����
=============================================================================*/
void CMcuVcInst::StopDoubleStream( BOOL32 bMsgSrcMt, BOOL32 bForceClose, BOOL32 bIsNeedAdjustVmpParam  )
{
	KillTimer( MCUVC_NOTIFYMCUH239TOKEN_TIMER );

	TMt tOldDStreamSrc = m_tDoubleStreamSrc;

	if(m_tDoubleStreamSrc.IsNull())
	{
		return;
	}

    TLogicalChannel tNullLogicChnnl;
	TLogicalChannel tH239LogicChnnl;
	CServMsg cServMsg;
	
	// ����KEDAMCU��˫��ͨ���Ĺرս���˫��Դ������ƣ�����˫�������ɹرո�ͨ��,����Ȩֹͣת��������
	if (MT_MANU_KDCMCU == m_ptMtTable->GetManuId(m_tDoubleStreamSrc.GetMtId()))
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[StopDoubleStream]not permit to operate the dual chanl of mcu\n");
	}
	//sony G70SP��H263-H239ʱ���û˫��,�Ҳ�ʹ�����ƶ�ʹ��FlowControlIndication���з��ͼ����ر� ����
	//POLYCOM7000�ľ�̬˫��ʱ��ʹ�����ƽ��з��ͼ����ر�
	//���ں��н���ʱ����,֮������ڼ䱣��ͨ����ֱ���˳�����
	else if( ( MT_MANU_SONY == m_ptMtTable->GetManuId(m_tDoubleStreamSrc.GetMtId()) && 
		  MEDIA_TYPE_H263 == m_tConf.GetCapSupport().GetDStreamMediaType() &&
          m_tConf.GetCapSupport().IsDStreamSupportH239()) || 
		MT_MANU_POLYCOM == m_ptMtTable->GetManuId(m_tDoubleStreamSrc.GetMtId()) )
	{
        if( TRUE == bMsgSrcMt && 
            m_tConfAllMtInfo.MtJoinedConf( m_tDoubleStreamSrc.GetMtId() ) && 
            m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ) )			
        {
            if( TRUE == bForceClose )
            {
                //��ʹǿ�ƹ�ͨ����Ҳ����polycomMCU��˫��ͨ��
				if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(m_tDoubleStreamSrc.GetMtId()) &&
					( MT_TYPE_MMCU == m_ptMtTable->GetMtType(m_tDoubleStreamSrc.GetMtId()) ||
					MT_TYPE_SMCU == m_ptMtTable->GetMtType(m_tDoubleStreamSrc.GetMtId()) ) )
				{				
					// xliang [11/14/2008]  Poly
					u16 wBitRate =  0;		
					tH239LogicChnnl.SetFlowControl( wBitRate );
					cServMsg.SetMsgBody( (u8*)&tH239LogicChnnl, sizeof( tH239LogicChnnl ) );
					SendMsgToMt( m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );
				}
				else
				{
					u8 byOut = 0;
					cServMsg.SetMsgBody( ( u8 * )&tH239LogicChnnl, sizeof( tH239LogicChnnl ) );	
					cServMsg.CatMsgBody( &byOut, sizeof(byOut) );
					SendMsgToMt( m_tDoubleStreamSrc.GetMtId(), MCU_MT_CLOSELOGICCHNNL_CMD, cServMsg );
					m_ptMtTable->SetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tNullLogicChnnl, FALSE );
				}
			}
            else
            {
                u16 wBitRate =  0;		
                tH239LogicChnnl.SetFlowControl( wBitRate );
                cServMsg.SetMsgBody( (u8*)&tH239LogicChnnl, sizeof( tH239LogicChnnl ) );
                SendMsgToMt( m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );
            }
		}
	}
	else
	{
		if( TRUE == bMsgSrcMt && 
			m_tConfAllMtInfo.MtJoinedConf( m_tDoubleStreamSrc.GetMtId() ) && 
			m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ) )			
		{
			u8 byOut = 0;
			cServMsg.SetMsgBody( ( u8 * )&tH239LogicChnnl, sizeof( tH239LogicChnnl ) );	
			cServMsg.CatMsgBody( &byOut, sizeof(byOut) );
			SendMsgToMt( m_tDoubleStreamSrc.GetMtId(), MCU_MT_CLOSELOGICCHNNL_CMD, cServMsg );
			//���ǻ����£�����������ն�������ᣬ˫��Դ��Ҫ�ø��ն�ֹͣ����˫����������������
			if( IsMultiCastMt(m_tDoubleStreamSrc.GetMtId()) )
			{
				CServMsg cTempServMsg;
				tH239LogicChnnl.SetFlowControl(0);
				cTempServMsg.SetMsgBody((u8*)&tH239LogicChnnl, sizeof(tH239LogicChnnl));
				SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cTempServMsg);
			}
			m_ptMtTable->SetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tNullLogicChnnl, FALSE );
		}
	}

    // zbq [09/05/2007] ¼���˫����������Ӧ��˫����
    u8 bySrcChn = m_tDoubleStreamSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
    g_cMpManager.RemoveSwitchBridge(m_tDoubleStreamSrc, bySrcChn, MODE_SECVIDEO);
    //g_cMpManager.RemoveSwitchBridge( m_tDoubleStreamSrc, 1, MODE_VIDEO );

    //�鲥�����鲥����
    if (m_tConf.GetConfAttrb().IsMulticastMode())
    {
        g_cMpManager.StopMulticast(m_tDoubleStreamSrc, 0, MODE_SECVIDEO);
    }
    //�����鲥����
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        g_cMpManager.StopDistrConfCast(m_tDoubleStreamSrc, MODE_SECVIDEO);
    }

    // ֪ͨ����ն�˫��״̬�ı� [02/05/2007-zbq]
    if ( TYPE_MCUPERI != m_tDoubleStreamSrc.GetType() )
    {
        // guzh [3/1/2007]
		//����KDCMCU���ڵȵ��ر��߼�ͨ��������״̬		
		if (  MT_MANU_KDCMCU  != m_ptMtTable->GetManuId( m_tDoubleStreamSrc.GetMtId() ))
		{
			m_ptMtTable->SetMtVideo2Send( m_tDoubleStreamSrc.GetMtId(), FALSE );
		}
		else
		{
			TMt tRealDsMt = GetConfRealDsMt();
			if (!tRealDsMt.IsNull())
			{
				TMtStatus tMtStatus;
				GetMtStatus(tRealDsMt,tMtStatus);
				tMtStatus.SetSndVideo2(FALSE);
				SetMtStatus(tRealDsMt,tMtStatus);
				MtStatusChange(&tRealDsMt,TRUE);
				if (!m_tConfAllMcuInfo.IsSMcuByMcuIdx(tRealDsMt.GetMcuIdx()))
				{
					u8 abyMcuId[MAX_CASCADEDEPTH-1];
					memset( &abyMcuId[0],0,sizeof(abyMcuId) );
					m_tConfAllMcuInfo.GetMcuIdByIdx(tRealDsMt.GetMcuIdx(),abyMcuId);
					tRealDsMt.SetMcuIdx( GetMcuIdxFromMcuId( &abyMcuId[0] ));
					tRealDsMt.SetMtId( abyMcuId[1] );
					GetMtStatus(tRealDsMt,tMtStatus);
					tMtStatus.SetSndVideo2(FALSE);
					SetMtStatus(tRealDsMt,tMtStatus);
					MtStatusChange(&tRealDsMt,TRUE);
				}
			}
		}
        /*TMtStatus tMtStatus;
        m_ptMtTable->GetMtStatus(m_tDoubleStreamSrc.GetMtId(), &tMtStatus);
                
        CServMsg cSendServMsg;
        cSendServMsg.SetSrcMtId( m_tDoubleStreamSrc.GetMtId() );
        cSendServMsg.SetEventId( MT_MCU_MTSTATUS_NOTIF );
        cSendServMsg.SetMsgBody( (u8*)&tMtStatus, sizeof(TMtStatus) );
        g_cMcuVcApp.SendMsgToConf( m_tConf.GetConfId(), MT_MCU_MTSTATUS_NOTIF,
                            cSendServMsg.GetServMsg(), cSendServMsg.GetServMsgLen() );*/
    }

	TMt atDstMt[MAXNUM_CONF_MT];
	u8 byDstMtNum = 0;
	TMt tDstMt;
	//�ر������ն˵�H.239ͨ��
    u8 byLoop = 1;
	for( ; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		//E1 MCU��˫��Դ��ͣ˫��ʱ����Ҫ�ָ�ǰ������[10/30/2012 chendaiwei]
		if( m_ptMtTable->GetMt( byLoop ) == tOldDStreamSrc
			&&  m_ptMtTable->GetMtTransE1(byLoop)
			&& (m_ptMtTable->GetMtType(byLoop) == MT_TYPE_SMCU
			||m_ptMtTable->GetMtType(byLoop) == MT_TYPE_MMCU))
		{
			u16 wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(byLoop);
			m_ptMtTable->SetMtReqBitrate(byLoop, wMtDialBitrate, LOGCHL_VIDEO);
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[StopDoubleStream] Double Src.%d is E1 MCU.restore FL BR to%d.\n",byLoop,wMtDialBitrate);
			
			continue;
		}

		if(m_tConfAllMtInfo.MtJoinedConf(byLoop)
			&& m_ptMtTable->GetMtLogicChnnl(byLoop, LOGCHL_SECVIDEO, &tH239LogicChnnl, TRUE)
			&& m_ptMtTable->GetMtSelMtByMode(byLoop, MODE_VIDEO2SECOND).IsNull())
		{	
			// xliang [11/14/2008] PolycomMCU ˫��ͨ�����أ����Է�˫��Դ��polycom�նˣ����ͷ�����(Ŀǰ֧��polyMCU������keda������ȡ���Ʒ�ʽ)
// 			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byLoop) &&
// 				( MT_TYPE_MMCU == m_ptMtTable->GetMtType(byLoop) ||
			// 				MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) ) )
			// �ͷ����ƻ���Ӧ��֪ͨ�ǿƴ�MCU(���ն�),��Ӧ��ֻ���ڱ���ͨMCU���ο����� [6/24/2013 chendaiwei]
			//[H239]11.2.3  End-user system owns the token and wants to release the token 
			//The end-user system shall send presentationTokenRelease.
			if ( MT_MANU_KDC != m_ptMtTable->GetManuId(byLoop)
				&& MT_MANU_KDCMCU != m_ptMtTable->GetManuId(byLoop))
			{
				if ( !(tOldDStreamSrc == m_ptMtTable->GetMt(byLoop)) )
				{
					TH239TokenInfo tH239TokenInfo;
					CServMsg cMsg;
					cMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
					cMsg.SetMsgBody((u8*)&tOldDStreamSrc, sizeof(TMt));
					cMsg.CatMsgBody((u8*)&tH239TokenInfo, sizeof(TH239TokenInfo));
					SendMsgToMt(byLoop, MCU_POLY_RELEASEH239TOKEN_CMD, cMsg);
				}
				
                //�����polycom��MCU����ͨ��,
                //�����⳧�ն˵�ͨ���ص���������أ���������𣬲�����˫��Դ����˫����ͨ���Ѿ��򿪣��������½�����
                //��polycom��MCU������239token������ͷŵ�ʱ��𽨽�����
                if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byLoop) &&
                    ( MT_TYPE_MMCU == m_ptMtTable->GetMtType(byLoop) ||
                    MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) ))
                {
                    continue;
                }
			}
			u8 byOut = 1;
			cServMsg.SetMsgBody( ( u8 * )&tH239LogicChnnl, sizeof( tH239LogicChnnl ) );	
			cServMsg.CatMsgBody( &byOut, sizeof(byOut) );
			SendMsgToMt( byLoop, MCU_MT_CLOSELOGICCHNNL_CMD, cServMsg );
			
			tDstMt = m_ptMtTable->GetMt( byLoop );
			m_ptMtTable->SetMtVideo2Recv(byLoop, FALSE);
			MtStatusChange(&tDstMt, TRUE);
			//zjl 20110510 Mp: StopSwitchToSubMt �ӿ������滻 
            //g_cMpManager.StopSwitchToSubMt(tDstMt, MODE_SECVIDEO, TRUE);
			atDstMt[byDstMtNum] = tDstMt;
			byDstMtNum ++;

			//m_ptMtTable->SetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tNullLogicChnnl, TRUE );

            // xsl [4/28/2006] ����˫��ʱ�����н���˫�����ն˻ָ����մ����Զ��������ն�������flowcontrol���µ���
            // if (m_ptMtTable->GetMtReqBitrate(tDstMt.GetMtId(), TRUE) == GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tDstMt.GetMtId())))
            if ( m_ptMtTable->GetMtTransE1(tDstMt.GetMtId()) && 
                 m_ptMtTable->GetMtBRBeLowed(tDstMt.GetMtId()) )
            {
                //zbq[01/30/2008] �Զ��������ն˴�������²���������ָ��䵱ǰ���ٴ���
                //u16 wCurFstBR = m_ptMtTable->GetMtReqBitrate(tDstMt.GetMtId(), TRUE);
                //u8  byDStreamScale = m_tConf.GetDStreamScale();
                //u16 wRealBR = wCurFstBR * 100 / (100-byDStreamScale);
                u16 wRealBR = m_ptMtTable->GetLowedRcvBandWidth(tDstMt.GetMtId());
                m_ptMtTable->SetMtReqBitrate(tDstMt.GetMtId(), wRealBR, LOGCHL_VIDEO);
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[StopDoubleStream]tDstMt.%d, MtReqBitrate = %d, adjust restore\n", tDstMt.GetMtId(), wRealBR);                
            }
            else
            {
                u16 wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(tDstMt.GetMtId());
                m_ptMtTable->SetMtReqBitrate(tDstMt.GetMtId(), wMtDialBitrate, LOGCHL_VIDEO);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[StopDoubleStream]tDstMt.%d, MtReqBitrate = %d, normally restore\n", tDstMt.GetMtId(), wMtDialBitrate);
            }
		}
		else if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) && 
			     !m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tH239LogicChnnl, TRUE ) )

		{
			tDstMt = m_ptMtTable->GetMt( byLoop );
			if( !(tOldDStreamSrc == tDstMt) )
			{
				StopSwitchDStreamToFirstLChannel( tDstMt );
			}
		}       
	}

	if (byDstMtNum > 0)
	{
		g_cMpManager.StopSwitchToSubMt(m_byConfIdx, byDstMtNum, atDstMt, MODE_SECVIDEO);
		for (u8 byIdx = 0; byIdx < byDstMtNum; byIdx++)
		{
			m_ptMtTable->SetMtLogicChnnl( atDstMt[byIdx].GetMtId(), LOGCHL_SECVIDEO, &tNullLogicChnnl, TRUE );
		}
	}

    //zbq[02/18/2008] ֹͣ˫����������VMP�Ĺ㲥����
    if ( g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) )
    {
        AdjustVmpBrdBitRate();
    }
    // guzh [6/28/2007] ͣ��ʱ��
    KillTimer(MCU_SMCUOPENDVIDEOCHNNL_TIMER);   
    

    // ͣ��¼�����˫��
    if ( m_tRecPara.IsRecDStream() &&  !m_tRecEqp.IsNull() )
    {
        StopSwitchToPeriEqp(m_tRecEqp.GetEqpId(), m_byRecChnnl, FALSE, MODE_SECVIDEO );
    }
    
 	// [5/27/2011 liuxu] �������ն�¼���¼���ͨ������˫�������Ĵ���
 	OnMtRecDStreamSrcChanged(FALSE);

	//�ڶ�·��Ƶ�Ķ����ش�
	StopBrdPrs(MODE_SECVIDEO);

	//ֹͣhduѡ��˫��
	TPeriEqpStatus tHduStatus;
	for (u8 byHduId = HDUID_MIN; byHduId <= HDUID_MAX; byHduId++)
	{
		if (!g_cMcuVcApp.IsPeriEqpConnected(byHduId))
		{
			continue;
		}
		if (!g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus))
		{
			continue;
		}
		u8 byChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byHduId));
		for(u8 byChnIdx = 0; byChnIdx < min(byChnNum,MAXNUM_HDU_CHANNEL); byChnIdx++)
		{
			if (TW_MEMBERTYPE_DOUBLESTREAM == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].byMemberType)
			{
				TMt tNullMt;
				tNullMt.SetNull();
				// [2013/03/11 chenbing] HDU�໭��Ŀǰ��֧��ѡ��˫��,��ͨ����0
				ChangeHduSwitch(&tNullMt, byHduId, byChnIdx, 0, TW_MEMBERTYPE_DOUBLESTREAM, TW_STATE_STOP, MODE_SECVIDEO);
			}
		}
	}

	//ͣ˫������
	StopAllBasSwitch(MODE_SECVIDEO);
	//[nizhijun 2010/9/9] ͣ�㲥����
	StopBrdAdapt(MODE_SECVIDEO);
	//���û�������ģʽ
	m_tConf.m_tStatus.SetAdaptMode(ADAPT_MODE_SECVID, FALSE);

	//ֹͣ˫��ʱͬʱֹͣ˫�����
	u8 byChannelNum = 0;
	BOOL bStopSecMc = FALSE;
	TMt tSecSrc;
	tSecSrc.SetNull();
	for (u8 byMCIdx = 1; byMCIdx <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byMCIdx++)
	{
		for( u8 byChanId = 0;byChanId < MAXNUM_MC_CHANNL;++byChanId )
		{
			g_cMcuVcApp.GetMcSrc( byMCIdx, &tSecSrc, byChanId , MODE_SECVIDEO);
			if( tSecSrc == m_tDoubleStreamSrc )
			{				
				bStopSecMc = TRUE;
				StopSwitchToMc( byMCIdx, byChanId, TRUE, MODE_SECVIDEO );
				break;
			}
		}
		if (bStopSecMc)
		{
			break;
		}
	}
	
    // xsl [4/29/2006] �ָ�����������
//     TSimCapSet tDstSCS, tSrcSCS;
//     if (m_tConf.m_tStatus.IsVidAdapting())
//     {        
//         //Ŀǰ�ݲ�����˫�ٻ���ֱ�������ָ�...
//         if (IsNeedVidAdapt(tDstSCS, tSrcSCS, m_wVidBasBitrate))
//         {
//             ChangeAdapt(ADAPT_TYPE_VID, m_wVidBasBitrate, &tDstSCS, &tSrcSCS);
//         }        
//     }
//     if (m_tConf.m_tStatus.IsBrAdapting())
//     {
//         if (IsNeedBrAdapt(tDstSCS, tSrcSCS, m_wBasBitrate))
//         {
//             ChangeAdapt(ADAPT_TYPE_BR, m_wBasBitrate, &tDstSCS, &tSrcSCS);
//         }        
//     }

    //zbq[12/11/2008] �������䣺ͣ˫�����������������������
//    StopAllHdBasSwitch(TRUE);
//    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_DOUBLEVID, FALSE);

//     if (m_tConf.m_tStatus.IsHdVidAdapting())
//     {
//         RefreshBasParam4AllMt(MODE_VIDEO);
//     }

	//�黹���ƻ�	
    if( MT_MANU_KDCMCU == m_ptMtTable->GetManuId(m_tDoubleStreamSrc.GetMtId()) )
    {
        ClearH239TokenOwnerInfo(NULL);
    }
	else
	{
		ClearH239TokenOwnerInfo(&m_tDoubleStreamSrc);
	}

	if ( !m_tCascadeMMCU.IsNull() && !IsMcu(m_tDoubleStreamSrc) &&  TYPE_MCUPERI != m_tDoubleStreamSrc.GetType() &&
		m_tConfAllMtInfo.MtJoinedConf(m_tCascadeMMCU.GetMtId()) )
	{
		OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(), m_tDoubleStreamSrc.GetMtId());
	}
	//zbq[09/12/2008] ʱ�����
	m_tDoubleStreamSrc.SetNull();
	
	// ����ϳ�˫���������
	// ͣ˫��ʱ,ֱ�Ӳ�˫��Դ��vmp˫������ͨ���Ľ���,ˢ��vmpParam��Ա,֪ͨ����,��ȫ��vmp,
	// ��ֹһ�������������vmp,���º����Ǵ�vmp��vmpnotifyδ�ظ������ܵ���.
	
	// ˫��������ʱ����vmp˫������ͨ��
	//1.Ĭ����˫������ͨ���еĳ�Ա��
	//2.˫��Դ�л�ʱ������ͣ�ٿ���ͣ˫��ʱ������vmp������ʱ�����
	if (bIsNeedAdjustVmpParam)
	{
		TEqp tVmpEqp;
		TVMPParam_25Mem tConfVmpParam;
		TPeriEqpStatus tPeriEqpStatus;
		TVMPParam_25Mem tVmpParam;
		u8 byDSChl;
		TVMPMember tVmpMember;
		TVmpChnnlInfo tVmpChnnlInfo;
		TVMPParam_25Mem tLastVmpParam;
		CServMsg cTmpMsg;
		u8 byTmp = 0XFF;//���ֶα���,���mpu��������,Ϊ����,��һ���ֽڱ�ʶ�ر�ͨ��
		u8 byAdd = 0;//��ʶ�ر�ͨ��
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
					//��˫������ͨ��
					byDSChl = tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_DSTREAM);
					if (byDSChl < MAXNUM_VMP_MEMBER)
					{
						tVmpMember = *tVmpParam.GetVmpMember(byDSChl);
						if (!tVmpMember.IsNull())
						{
							tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
							//ͣ����
							StopSwitchToPeriEqp(tVmpEqp.GetEqpId(), byDSChl, FALSE, MODE_SECVIDEO);
							tVmpChnnlInfo.ClearChnlByVmpPos(byDSChl); //���ͨ��ǰ����
							g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
							//˫���������ͨ��
							tVmpMember.SetNull();
							tVmpParam.SetVmpMember(byDSChl, tVmpMember);
							tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);	//����״̬����
							g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
							//���»���vmpparam
							if (byDSChl == tConfVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_DSTREAM))
							{
								tConfVmpParam.SetVmpMember(byDSChl, tVmpMember);
								//m_tConf.m_tStatus.SetVmpParam(tVmpParam);
								g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tConfVmpParam);
								// ����LastVmpParamǰ,ˢ��vmp˫������ͨ��̨��
								RefreshVmpChlMemalias(tVmpEqp.GetEqpId());
								tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
								tLastVmpParam.SetVmpMember(byDSChl, tVmpMember);//����m_tLastVmpParam
								g_cMcuVcApp.SetLastVmpParam(tVmpEqp, tLastVmpParam);
							}
							//�������������ͨ��
							{
								LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[StopDoubleStream]Send MCU_VMP_ADDREMOVECHNNL_CMD To Vmp, byChnnl:%d.\n", byDSChl);
								cTmpMsg.Init();
								cTmpMsg.SetChnIndex(byDSChl);
								cTmpMsg.SetMsgBody( &byTmp, sizeof(byTmp) );
								cTmpMsg.CatMsgBody( &byAdd, sizeof(byAdd) );
								SendMsgToEqp( tVmpEqp.GetMtId(), MCU_VMP_ADDREMOVECHNNL_CMD, cTmpMsg);
							}
							cTmpMsg.Init();
							cTmpMsg.SetEqpId(tVmpEqp.GetEqpId());
							cTmpMsg.SetMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
							SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cTmpMsg );
							/*cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
							SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );*/
						}
					}
				}
			}
		}
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[StopDoubleStream]bIsNeedAdjustVmpParam:%d, Not adjust VmpParam.\n", bIsNeedAdjustVmpParam);
	}

	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();

    //�����˫��ʱ���ն˷����ˣ���Ӧ�������ն˵ķ������ʻָ�
	TLogicalChannel tLogicalChannel;
	if (!tLocalVidBrdSrc.IsNull() && TYPE_MT == tLocalVidBrdSrc.GetType() 
		//˫��Դ��MCU������MCU����ͣ˫�������(�����յ�MT_MCU_CLOSELOGICCHNNL_NOTIF
		//�����)�����ո�˫��������MCU˫��ͨ������ͨ�����رգ�����������Ҳ��Ӧ�õ���[1/5/2013 chendaiwei]
		&& (MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tOldDStreamSrc.GetMtId())
		|| !bMsgSrcMt))
    {
        if (m_ptMtTable->GetMtLogicChnnl(tLocalVidBrdSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
        {            
            tLogicalChannel.SetFlowControl(m_ptMtTable->GetSndBandWidth(tLocalVidBrdSrc.GetMtId()));
            cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(tLocalVidBrdSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
        }
    }    

	//ˢ�µ�һ·�������,���ʻָ�	
	if (m_tConf.m_tStatus.IsVidAdapting())
	{
		StartBrdAdapt(MODE_VIDEO);
		//�����¼�MCU�Ľ���
		//�˴������������������˹�����¼����˫��������ͬʱ��һ��ʼ�¼��չ㲥������
		//�������ڷ�˫�������¼�˫�������ֲ�ͬ�������¼��������չ㲥������Ҫ�������䣬���������佻����
		//��ʱ����ֹͣ˫���������¼��������ʻָ�����Ҫ���ղ������������������Ҫ��������
		//ƽ�����ǣ������ǰ��MCU�Ѿ����չ㲥��������ô�ظ�������Ӧ����ƽ����
		//�ó���ֻ��Ҫ˫�ٻ��鿼�ǣ���Ϊ���ٻ���Ļ������¼�˫��������ͬ��ͬʱҲ������㲥Դ���ʣ�����û����
		if ( m_tConf.GetSecBitRate() != 0 )
		{
			u8 byMtId = 0 ;
			TMt tTempMcu;
			TMt tVidSrc = GetVidBrdSrc();
			for ( u8 byMcuIdx =0; byMcuIdx < MAXNUM_SUB_MCU; byMcuIdx++ )
			{
				byMtId = GetFstMcuIdFromMcuIdx( byMcuIdx );
				if ( byMtId >0 && byMtId<=MAXNUM_CONF_MT )
				{
					tTempMcu = m_ptMtTable->GetMt( byMtId );
					StartSwitchFromBrd(tVidSrc,0,1,&tTempMcu);
					ConfPrint(LOG_LVL_DETAIL,MID_MCU_CONF,"[StopDoubleStream]rebuild brd switch to submcu:%d\n",byMtId);
				}
			}
		}
	}   
	else
	{
		RefreshBasBrdAdpParam(MODE_VIDEO);
		RefreshBrdBasParam4AllMt(MODE_VIDEO);
	}
    
	//����˫���ն����ڷ��͵�һ·�������ָ�����
	if(IsDStreamMtSendingVideo(tOldDStreamSrc))
	{
		if (m_ptMtTable->GetMtLogicChnnl(tOldDStreamSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE)
			//˫������Դ��ͬһMCU��ͣ˫�������ո�˫��������MCU˫��ͨ������ͨ�����رգ�����������Ҳ��Ӧ�õ���[1/5/2013 chendaiwei]
			&& (MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tOldDStreamSrc.GetMtId())
		     || tOldDStreamSrc.GetMtId() != tLocalVidBrdSrc.GetMtId()))
        {            
            tLogicalChannel.SetFlowControl(m_ptMtTable->GetSndBandWidth(tOldDStreamSrc.GetMtId()));
            cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(tOldDStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
        }
	}

	// ����˫��������ָ�˫������, zgc, 2007-05-30
	if ( TYPE_MCUPERI != tOldDStreamSrc.GetType() || EQP_TYPE_RECORDER != tOldDStreamSrc.GetEqpType())
	{
		if ( !m_tPlayEqp.IsNull() && !m_tConf.m_tStatus.IsNoPlaying() && m_tPlayEqpAttrib.IsDStreamPlay() && m_tPlayEqpAttrib.IsDStreamFile())
		{
			TLogicalChannel tLogicChnnl;
			tLogicChnnl.SetChannelType(m_tPlayEqpAttrib.GetDVideoType());
			tLogicChnnl.SetSupportH239(m_tConf.GetCapSupport().IsDStreamSupportH239());
			StartDoubleStream(m_tPlayEqp, tLogicChnnl);
		}
	}
	
    // guzh [3/19/2007] ���������ն�ѡ�������
    TMtStatus tStatus;
    TMt tOtherMt;
    for( byLoop = 1 ; byLoop <= MAXNUM_CONF_MT; byLoop++ )
    {
        if ( m_tConfAllMtInfo.MtJoinedConf( byLoop ) && 
             m_ptMtTable->GetMtStatus(byLoop, &tStatus) )
        {
			//�������˫ѡ����������һѡ��Դ�ķ���
			if (!m_ptMtTable->GetMtSelMtByMode(byLoop, MODE_VIDEO2SECOND).IsNull())
			{
				continue;
			}

			//˫������Դ��ͬһMCU��ͣ˫�������ո�˫��������MCU˫��ͨ������ͨ�����رգ�����������Ҳ��Ӧ�õ���[1/5/2013 chendaiwei]
			if(MT_MANU_KDCMCU == m_ptMtTable->GetManuId(tOldDStreamSrc.GetMtId())
				&& tOldDStreamSrc.GetMtId() == tLocalVidBrdSrc.GetMtId()
				&& byLoop == tOldDStreamSrc.GetMtId())
			{
				continue;
			}

			if(MT_MANU_KDC != m_ptMtTable->GetManuId(byLoop))
			{
				m_ptMtTable->GetMtLogicChnnl(byLoop, LOGCHL_VIDEO, &tLogicalChannel, FALSE);
				tLogicalChannel.SetCurrFlowControl(0xFFFF);
				tLogicalChannel.SetFlowControl(m_ptMtTable->GetDialBitrate(byLoop));
				cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
                SendMsgToMt(byLoop, MCU_MT_FLOWCONTROL_CMD, cServMsg);
			}
            tOtherMt = tStatus.GetSelectMt(MODE_VIDEO);
			tOtherMt = GetLocalMtFromOtherMcuMt(tOtherMt);
            if ( !tOtherMt.IsNull() )
            {
                // �ָ�Flowcontrol��ֵ���ھ��巢��ʱ����
                m_ptMtTable->GetMtLogicChnnl(tOtherMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);
                tLogicalChannel.SetCurrFlowControl(0xFFFF);
                tLogicalChannel.SetFlowControl(m_ptMtTable->GetDialBitrate(tOtherMt.GetMtId()));
                cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
                SendMsgToMt(tOtherMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
            }
        }
    }

	// ��vmp˫��������������ģ��п�����ʱ�������ն���Ϊ�����˻�û����Ϊ�㲥Դ [pengguofeng 3/2/2013]
	/*TMt tSpeaker = m_tConf.GetSpeaker();
	if ( IsSpeakerCanBrdVid( &tSpeaker ) &&
		IsMultiCastMt( tSpeaker.GetMtId())
		&& !(tSpeaker == GetLocalVidBrdSrc() )
		&& !IsSatMtSend(tSpeaker)
		&& !IsSatMtOverConfDCastNum(tSpeaker))
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[StopDoubleStream]Sat Conf recover vid brd src:%d when canceling dual src\n",
			tSpeaker.GetMtId());
		ChangeVidBrdSrc(&tSpeaker);
	}*/
//    RefreshRcvGrp();
	return;
}

/*=============================================================================
    �� �� ���� StartDoubleStream
    ��    �ܣ� ����˫��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMt &tDstMt
               TLogicalChannel &tLogicChnnl 
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/02/19  3.6			����                ����
	2006/01/19	4.0			�ű���				  ¼���¼������������������
=============================================================================*/
void CMcuVcInst::StartDoubleStream(TMt &tMt, TLogicalChannel &tLogicChnnl)
{
	if (!m_tDoubleStreamSrc.IsNull())
	{
		// xliang [12/19/2008] ���ӱ��
		// zjl    [03/04/2010] ����ϼ�mcu��˫��Դ,�򲻼�¼��mtid�� ��֤�¼��ն˷�˫����ռʱ��˳�����ϼ�˫��ͨ��
		//��ԭ��������ʱ���Թ��ˣ����ϼ����͹ر�ǰһ��˫��ͨ��ʱ�ų��Դ��ϼ�˫��ͨ�������ϼ���ʱ���������ر�ǰ��˫��ͨ����
		if (MT_MANU_KDCMCU != m_ptMtTable->GetManuId(m_tDoubleStreamSrc.GetMtId()))
		{
			// [pengjie 2010/9/9] �ſ�����˫���������Ӻ����һ��˫��Դ��ͨ��������
// 			
// 			m_byLastDsSrcMtId = m_tDoubleStreamSrc.GetMtId(); 
		}		
		else
		{	
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[StartDoubleStream] not record last double stream mtid:% because it's keda mcu!\n", tMt.GetMtId());
		}
		StopDoubleStream(TRUE, FALSE, FALSE);//�л�˫��Դʱ��ͣ˫�����õ���vmp
	}

	/*20110805 zjl �ն˷����ߵ��Ǳ�׼���ƻ����������ƻ�ʱmcu���Ѿ�������token
				   �����˫������ʱ�������������ƻ���������Ҫ�ж����������˫����
				   ����tokenΪ�գ���token��Ϊ�µ�˫��Ԥ��
	*/
	if (m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull())
	{
		m_tH239TokenOwnerInfo.SetTokenMt(tMt);
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[StartDoubleStream] Set New Token<Type:%d, McuId:%d, MtId:%d>!\n",
									tMt.GetType(), tMt.GetMcuId(), tMt.GetMtId());
	}

    // zbq [09/04/2007] ¼���˫�����󣬴˴������ܽ�����
    if ( TYPE_MCUPERI == tMt.GetType() && EQP_TYPE_RECORDER == tMt.GetEqpType())
    {
    }
    else
    {
        g_cMpManager.SetSwitchBridge(tMt, 0, MODE_SECVIDEO);
    }

	// [pengjie 2010/3/10] MCU��ʱ������MCU��239���ƻ��������ǲ���239��˫��
    SetTimer( MCUVC_NOTIFYMCUH239TOKEN_TIMER, 10000);
    TCapSupport tCapSupport;
	//�������ն˵�H.239ͨ��
	for(u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
	{
		//E1 MCU��˫��Դ������������ͨ��ҲӦ�õ���Ϊ��ȥ˫��ռ����Ĵ��������������������㲥Դʱ�������ҵ�ʵ��
		//�����ն�ǰ��ͨ������Сֵ���Ӷ���ȷ���������㲥Դ����[10/30/2012 chendaiwei]
		if( byLoop == tMt.GetMtId() 
			&& m_ptMtTable->GetMtTransE1(byLoop)
			&& (m_ptMtTable->GetMtType(byLoop) == MT_TYPE_SMCU ||
			    m_ptMtTable->GetMtType(byLoop) == MT_TYPE_MMCU))
		{
			m_ptMtTable->SetMtReqBitrate(byLoop,GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(byLoop)),LOGCHL_VIDEO);

			continue;
		}

        if (byLoop == tMt.GetMtId() && TYPE_MCUPERI != tMt.GetType())
        {
            continue;
        }

        if (!m_tConfAllMtInfo.MtJoinedConf(byLoop))
        {
            continue;
        }

		// ����vrs�ն�¼��ʵ����¼��Դ���Ǹ��ն˵�ʵ��
		if (m_ptMtTable->GetMtType(byLoop) == MT_TYPE_VRSREC)
		{
			TMt tRecSrc = m_ptMtTable->GetRecSrc(byLoop);
			if (m_ptMtTable->GetRecChlType(byLoop) == TRecChnnlStatus::TYPE_RECORD
				&& !tRecSrc.IsNull())
			{
				if (!(GetLocalMtFromOtherMcuMt(tRecSrc) == GetLocalMtFromOtherMcuMt(tMt)))
				{
					// ��¼���ն˲�һ�£���¼˫��
					continue;
				}
			}
		}

		//[2012/8/31 zhangli]�������˫ѡ������ͣ˫ѡ�� 
		if (!m_ptMtTable->GetMtSelMtByMode(byLoop, MODE_VIDEO2SECOND).IsNull())
		{
			StopSelectSrc(m_ptMtTable->GetMt(byLoop), MODE_VIDEO2SECOND);
		}

        TMt tOtherMt = m_ptMtTable->GetMt(byLoop);

		//zbq[09/18/2008] ˫������αװ. ֡����ʱȡ��������. ��֡���ļ�������.
		if ( TYPE_MCUPERI == tMt.GetType() && EQP_TYPE_RECORDER == tMt.GetEqpType() ) 
		{
			u16 wFileDSW = 0;
			u16 wFileDSH = 0;
			u8 byFileDSType = 0;
			m_tPlayFileMediaInfo.GetDVideo(byFileDSType, wFileDSW, wFileDSH);
			u8 byFileDSRes = GetResByWH(wFileDSW, wFileDSH);

			tLogicChnnl.SetVideoFormat(byFileDSRes);

			if (MEDIA_TYPE_H264 == byFileDSType)
			{
				tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamUsrDefFPS());
			}
			else
			{
				tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamUsrDefFPS());
			}

		}

        //��ͨ�ն˴�˫��
        if(!m_ptMtTable->GetMtCapSupport( tOtherMt.GetMtId(), &tCapSupport ))
        {
            continue;
        }
		if (MEDIA_TYPE_NULL == tCapSupport.GetDStreamMediaType())
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[StartDoubleStream] Mt.%d's DsCap is null so continue!\n",
						tOtherMt.GetMtId());
			continue;
		}
        
		// xliang [11/14/2008]  ���polycom˫�������⴦��
		//polycom��˫��ͨ�����ն˺���ʱ��ȷ��������˴򿪲���;
		//�˴��Է�����Polycom��MT��ֱ��flowctrl�������. ��Polycom��MCU�������������ƣ��������Ƴɹ�����flowctrl
		if ( (MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byLoop) ||
              MT_MANU_AETHRA == m_ptMtTable->GetManuId(byLoop) )
			&&
			( MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) || 
			  MT_TYPE_MMCU == m_ptMtTable->GetMtType(byLoop)) )
		{
			CServMsg cServMsg;
			TMt tPolyMCU = m_ptMtTable->GetMt(byLoop);
			TH239TokenInfo tH239TokenInfo;
			
			//��������Ҫ�ж� ��ǰ��ͨ���Ƿ���ڣ� ����ᵼ��ȡǰ��ͨ������쳣
			TLogicalChannel tSecVidChnnl;
			if ( m_ptMtTable->GetMtLogicChnnl(byLoop, LOGCHL_SECVIDEO, &tSecVidChnnl, TRUE) )
			{
				cServMsg.SetEventId(MCU_POLY_GETH239TOKEN_REQ);
				cServMsg.SetMsgBody((u8*)&tPolyMCU, sizeof(TMt));
				cServMsg.CatMsgBody((u8*)&tH239TokenInfo, sizeof(tH239TokenInfo));
				SendMsgToMt(byLoop, MCU_POLY_GETH239TOKEN_REQ, cServMsg );
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[StartDoubleStream] MCU_POLY_GETH239TOKEN_REQ to PolyMCU.%d\n", byLoop);
			}
			else
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[StartDoubleStream] No send MCU_POLY_GETH239TOKEN_REQ to polyMCU due to Forward chnnl closed, try to reopen it, start DS once more, please\n" );
			}
		}
		else
		{
			McuMtOpenDoubleStreamChnnl(tOtherMt, tLogicChnnl/*, tCapSupport*/);  
		}
	}

	// fxh[090213]��ȡԭ˫��Դ��������
	TDStreamCap tOldDSCap;
	if (!m_tDoubleStreamSrc.IsNull())
	{
		GetDSBrdSrcSim(tOldDSCap);
	}

    if (TYPE_MCUPERI == tMt.GetType())
    {
	    m_tDoubleStreamSrc = tMt;
    }
    else
    {
        m_tDoubleStreamSrc = m_ptMtTable->GetMt(tMt.GetMtId());
        // guzh [3/1/2007]
        m_ptMtTable->SetMtVideo2Send( m_tDoubleStreamSrc.GetMtId(), TRUE );
		MtStatusChange(&m_tDoubleStreamSrc,TRUE);

        // ֪ͨ����ն�˫��״̬�ı� [02/05/2007-zbq]
        /*TMtStatus tMtStatus;
        m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus);
        
        CServMsg cServMsg;
        cServMsg.SetSrcMtId( tMt.GetMtId() );
        cServMsg.SetEventId( MT_MCU_MTSTATUS_NOTIF );
        cServMsg.SetMsgBody( (u8*)&tMtStatus, sizeof(TMtStatus) );
        g_cMcuVcApp.SendMsgToConf( m_tConf.GetConfId(), MT_MCU_MTSTATUS_NOTIF,
                            cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );*/
    }

	//[nizhijun 2010/12/21] �ı�PRS˫��Դ
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (tConfAttrb.IsResendLosePack() &&!m_tDoubleStreamSrc.IsNull())
	{	
		StartBrdPrs(MODE_SECVIDEO);				
	}

   TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();
    
	//��������, ��ǿ�Ƶ�����һ·����Դ�ķ��������۰�,û�������������FlowControl����
    if (!tLocalVidBrdSrc.IsNull() && TYPE_MT == tLocalVidBrdSrc.GetType())
    {
        CServMsg cServMsg;
        TLogicalChannel tLogicalChannel;
        if (TRUE == m_ptMtTable->GetMtLogicChnnl(tLocalVidBrdSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
        {
            //�����˫��ʱ���ն˷����ˣ���Ӧ�������ն˵ķ������ʼ��룬��������ͨ����Ϣ��
            tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth(tLocalVidBrdSrc.GetMtId())));
            cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(tLocalVidBrdSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
        }
    }

	//����MCU��˫��ͨ�������ı�������˫�����ȵ��ϼ�MCU��˫��Դ�������ʣ��������¼�
	//˫�����ʲ�һ�£����±����ն˿�˫������ͨ��������ʱ����Ҫ������[3/21/2013 chendaiwei]
	CServMsg cServMsg;
	TLogicalChannel tDSLogicChannel;
	if (TRUE == m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tDSLogicChannel, FALSE)
		&& (m_tDoubleStreamSrc.GetMtType() == MT_TYPE_SMCU || m_tDoubleStreamSrc.GetMtType() == MT_TYPE_MMCU))
	{
		tDSLogicChannel.SetFlowControl(GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth(m_tDoubleStreamSrc.GetMtId()),FALSE));
		cServMsg.SetMsgBody((u8*)&tDSLogicChannel, sizeof(tDSLogicChannel));
		SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
	}

//	RefreshRcvGrp();

    //˫��¼��
    //[2011/07/16/zhanlgi]����¼�����RTP/RTCP����
	AdjustConfRecordSrcStream(MODE_SECVIDEO, TRUE);

	// [5/27/2011 liuxu] �������ն�¼���¼���ͨ������˫�������Ĵ���
	OnMtRecDStreamSrcChanged(TRUE);

    //hdu˫��ѡ������
	TPeriEqpStatus tHduStatus;
	for (u8 byHduId = HDUID_MIN; byHduId <= HDUID_MAX; byHduId++)
	{
		if (!g_cMcuVcApp.IsPeriEqpConnected(byHduId))
		{
			continue;
		}
		if (!g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus))
		{
			continue;
		}
		u8 byChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byHduId));
		for(u8 byChnIdx = 0; byChnIdx < min(byChnNum,MAXNUM_HDU_CHANNEL); byChnIdx++)
		{
			if (TW_MEMBERTYPE_DOUBLESTREAM == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].byMemberType)
			{
				// [2013/03/11 chenbing] HDU�໭��Ŀǰ��֧��ѡ��˫��,��ͨ����0
				ChangeHduSwitch(&m_tDoubleStreamSrc, byHduId, byChnIdx, 0, TW_MEMBERTYPE_DOUBLESTREAM, TW_STATE_START, MODE_SECVIDEO);
			}
		}
	}

	u8 byVmpId;
	TEqp tVmpEqp;
	TPeriEqpStatus tPeriEqpStatus;
	TVMPParam_25Mem tVmpParam;
	u16 wError = 0;
	for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			continue;
		}
		byVmpId = m_abyVmpEqpId[byIdx];
		tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
		
		// ����ϳ�˫���������
		if (g_cMcuVcApp.GetVMPMode(tVmpEqp) == CONF_VMPMODE_CTRL)
		{
			if (g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus))
			{
				tVmpParam = tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam();
				//��˫������ͨ��
				if (tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_DSTREAM) < MAXNUM_VMP_MEMBER)
				{
					wError = 0;
					if (CheckVmpParam(byVmpId, tVmpParam, wError))
					{
						// ˫����ǣ�漶����ش�
						AdjustVmpParam(byVmpId, &tVmpParam, FALSE, FALSE);
					}
					if (wError)
					{
						tVmpParam.Print(LOG_LVL_KEYSTATUS);
						ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[StartDoubleStream] CheckVmpParam failed, wError[%d].\n", wError);
					}
				}
			}
		}
	}
	//���۵�ǰ˫�������Ƿ�����ÿ�ζ�����, ��������£��������������ٸ���
	StartBrdAdapt(MODE_SECVIDEO);

//     if (m_cMtRcvGrp.IsNeedAdp(FALSE))
//     {
		//���۵�ǰ˫�������Ƿ�����ÿ�ζ�����, ��������£��������������ٸ���
//		StartBrdAdapt(MODE_SECVIDEO);
//         if (!m_tConf.m_tStatus.IsDSAdapting())
//         {
//             StartBrdAdapt(MODE_SECVIDEO);
//         }
//         else
//         {
// 			// fxh[20090213]bas��������˫��Դ�л�ʱ���ж��������������Ƿ����޸�
// 			TDStreamCap tNewDSCap;
// 			GetDSBrdSrcSim(tNewDSCap);
// 			if (tOldDSCap.GetMediaType() != tNewDSCap.GetMediaType())
// 			{
// 				u8 byDSBasNum = 0;
// 
//                 //������Դ��������������
//                 TBasChn atBasChn[MAXNUM_CONF_MVCHN];
//                 m_pcBasMgr->GetChnGrp(byDSBasNum, atBasChn, CHN_ADPMODE_DSBRD);
// 
// 				for(u8 byIdx = 0; byIdx < byDSBasNum; byIdx ++)
// 				{
//                     if (!atBasChn[byIdx].IsNull())
//                     {
//                         ChangeHDAdapt(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId());
//                     }
//                 }
// 			}
// 			// ���������������
//             RefreshBasParam4AllMt(MODE_SECVIDEO);
//             StartAllBasSwitch(MODE_SECVIDEO);
//         }
//    }

	// ˢ�µ�һ·�������
    if ( m_tConf.m_tStatus.IsVidAdapting() )
    {
		StartBrdAdapt(MODE_VIDEO);
	}
    
    //�鲥�����鲥����
    if (m_tConf.GetConfAttrb().IsMulticastMode())
    {
        g_cMpManager.StartMulticast(m_tDoubleStreamSrc, 0, MODE_SECVIDEO);
    }

    //�����鲥
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        g_cMpManager.StartDistrConfCast(m_tDoubleStreamSrc, MODE_SECVIDEO);      
    }

    // guzh [10/30/2007] 
    if (m_tDoubleStreamSrc.GetType() == TYPE_MT)
    {
        //�µ�˫����������ʱ��ǿ�ƹؼ�֡
        SetTimer(MCUVC_SECVIDEO_FASTUPDATE_TIMER, 1200, 100*m_tDoubleStreamSrc.GetMtId()+1);	       
    }

    // guzh [10/30/2007] ע�����Ϣ�Ĵ�������������˫��Դ���͵���������������һ��Ҫ���ö�ʱ��
    //�ȴ�1s��˫�������ն˷���flowcontrol����˫������
    SetTimer(MCUVC_SENDFLOWCONTROL_TIMER, 1200);

	//���µ�ǰ��˫��״̬
    if (VCS_CONF == m_tConf.GetConfSource())
    {
//        m_cVCSConfStatus.SetConfDualing(TRUE);
        VCSConfStatusNotif();

        //[11/18/2011 zhushengze]֪ͨVCS�¼���ʼ��˫����VCS�յ���Ϣ������󽨽�����
        //������VCS�¼��ն�����˫��
        if (m_tDoubleStreamSrc.GetMtType() == MT_TYPE_SMCU)
        {   
			cServMsg.SetEventId( MCU_VCS_STARTDOUBLESTREAM_NOTIFY );
            cServMsg.SetMsgBody( (u8*)&m_tDoubleStreamSrc, sizeof(TMt) );
            SendMsgToAllMcs(MCU_VCS_STARTDOUBLESTREAM_NOTIFY, cServMsg);
            LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_VCS, "send vcs start double stream notify, DSSrc is %d\n", m_tDoubleStreamSrc.GetMtId());
        }
    }

	if ( !m_tCascadeMMCU.IsNull() && /*!IsMcu(m_tDoubleStreamSrc) &&*/  TYPE_MCUPERI != m_tDoubleStreamSrc.GetType() &&
		m_tConfAllMtInfo.MtJoinedConf(m_tCascadeMMCU.GetMtId()) )
	{
		OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(), m_tDoubleStreamSrc.GetMtId());
	}
	m_byNewTokenReqMtid = 0;
    return;
}


/*=============================================================================
�� �� ���� ProcSendFlowctrlToDSMtTimeout
��    �ܣ� ����flowcontrol��˫������Դ����Ϊ�ն�Ҳ��Ҫ����˫�����ʣ����еȴ�1s���ڷ���flowcontrol����ֹ�ն˽����ʸĻ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/4/27  4.0			������                  ����
=============================================================================*/
void CMcuVcInst::ProcSendFlowctrlToDSMtTimeout( void )
{
    if (m_tDoubleStreamSrc.IsNull())
    {
        ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcSendFlowctrlToDSMtTimeout] no double stream source mt!\n");
        return;
    }

    CServMsg cServMsg0, cServMsg1;
    TLogicalChannel tLogicalChannel;
    if ( ( !IsMultiCastMt(m_tDoubleStreamSrc.GetMtId()) || 
           IsDStreamMtSendingVideo(m_tDoubleStreamSrc) ) &&
         m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE) )
    {
        tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth(m_tDoubleStreamSrc.GetMtId())));
        tLogicalChannel.SetCurrFlowControl(0xffff);//���ϴα����flowcontrol�ı䣬��ֹ������
        cServMsg0.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));   
		SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg0); 
    }
	if (m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE))
    {
		u16 wDSBitrate = GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth( m_tDoubleStreamSrc.GetMtId() ), FALSE);
        tLogicalChannel.SetFlowControl(wDSBitrate);
        tLogicalChannel.SetCurrFlowControl(0xffff);//���ϴα����flowcontrol�ı䣬��ֹ������
        cServMsg1.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel)); 
		SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg1);  
    }

	TMtStatus tStatus;
	u8 byLoop = 1;
	//�ն����ʲ�ͬ����������Զ�����ͨ��ȡС����Դ�ն�����
    
    // guzh [3/19/2007] ���²���ÿ���ն˵�ѡ��Դ�������Ƿ�Ҫ��������
    TMt tOtherMt;
    u16 wBitrate;
    for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if ( !m_tConfAllMtInfo.MtJoinedConf(byLoop) )
        {
            continue;
        }      
        m_ptMtTable->GetMtStatus(byLoop, &tStatus);
        tOtherMt = tStatus.GetSelectMt(MODE_VIDEO);
		tOtherMt = GetLocalMtFromOtherMcuMt(tOtherMt);
        if ( !tOtherMt.IsNull() &&
             !(tOtherMt == m_tDoubleStreamSrc) )
        {
			// �����ն˵�ǰ�ķ������ʡ�˫�����շ�����ȷ��flowcontrol��ֵ
            m_ptMtTable->GetMtLogicChnnl( tOtherMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);
            
			const u16 wSendBand = m_ptMtTable->GetSndBandWidth(tOtherMt.GetMtId());
			const u16 wRcvBand = m_ptMtTable->GetRcvBandWidth(byLoop);
			wBitrate = min (wSendBand ,  wRcvBand);
            wBitrate = GetDoubleStreamVideoBitrate(wBitrate);

			const u16 wFlowCtrl = tLogicalChannel.GetFlowControl() ;
            wBitrate = min( wBitrate, wFlowCtrl);
            tLogicalChannel.SetFlowControl(wBitrate);
			cServMsg0.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(tOtherMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg0);
        }
		//������KEDA�ն������������Ա��KEDA�ն��������Ի��ɹ���˫�������������˫��Դ���������գ����Ҫ�����������������ʣ���
		if (MT_MANU_KDC != m_ptMtTable->GetManuId(byLoop) && MT_TYPE_MT == m_ptMtTable->GetMtType(byLoop))
		{
			m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tLogicalChannel, FALSE);
			const u16 wSendBand = m_ptMtTable->GetSndBandWidth(tOtherMt.GetMtId());
			const u16 wRcvBand = m_ptMtTable->GetRcvBandWidth(byLoop);
			wBitrate = min (wSendBand ,  wRcvBand);
            wBitrate = GetDoubleStreamVideoBitrate(wBitrate);
			const u16 wFlowCtrl = tLogicalChannel.GetFlowControl() ;
            wBitrate = min( wBitrate, wFlowCtrl);
            tLogicalChannel.SetFlowControl(wBitrate);
			cServMsg0.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(tOtherMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg0);
		}
    }

    return;
}

/*=============================================================================
  �� �� ���� ProcSmcuOpenDVideoChnnlTimer
  ��    �ܣ� �¼�mcu���ϼ���˫��ͨ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::ProcSmcuOpenDVideoChnnlTimer(const CMessage * pcMsg)
{
	u32 dwMtId = *(u32 *)pcMsg->content;
	TLogicalChannel tLogicalChannel;
	TCapSupport tCapSupport;
	TMt tMt = m_ptMtTable->GetMt((u8)dwMtId);
	if( m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) 
		&& m_ptMtTable->GetMtCapSupport((u8)dwMtId, &tCapSupport) )
	{
		McuMtOpenDoubleStreamChnnl(tMt, tLogicalChannel/*, tCapSupport*/);
	}

	return;
}

/*=============================================================================
  �� �� ���� OpenDoubleStreamChnnl
  ��    �ܣ� ��˫��ͨ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMt &tMt
             TLogicalChannel &tLogicChnnl
             TCapSupport &tCapSupport
  �� �� ֵ�� void 
=============================================================================*/
void CMcuVcInst::McuMtOpenDoubleStreamChnnl(TMt &tMt, 
                                            const TLogicalChannel &tLogicChnnl/*, 
                                            const TCapSupport &tCapSupport*/)
{
	if (TYPE_MCUPERI != m_tDoubleStreamSrc.GetType() && tMt.GetMtId() == m_tDoubleStreamSrc.GetMtId()
		&& m_ptMtTable->GetMtSelMtByMode(tMt.GetMtId(), MODE_VIDEO2SECOND).IsNull())
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[McuMtOpenDoubleStreamChnnl]Mtid(%d)m_tDoubleStreamSrc(Mtid:%d,Type:%d)!So Return!\n",
						tMt.GetMtId(),m_tDoubleStreamSrc.GetMtId(),m_tDoubleStreamSrc.GetType());
		return;
	}

	//[2011/04/25 zhushz] ��һ·�߼�ͨ��δ���򲻿��ڶ�·
	TLogicalChannel tMainVideoLogicalChannel;
	m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMainVideoLogicalChannel, TRUE);
	if ( tMainVideoLogicalChannel.GetChannelType() == MEDIA_TYPE_NULL ||
		tMainVideoLogicalChannel.GetMediaType() == MODE_NONE )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[McuMtOpenDoubleStreamChnnl] Not Open Mt.%d Second Channel because its first Channel not opened!\n", tMt.GetMtId());
		
		return;
    }

	u32 dwRcvIp = 0;
    u16 wRcvPort = 0;
    TLogicalChannel tDStreamChnnl;
    CServMsg cMsg;

    TCapSupport tCapSupport;
    m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tCapSupport);

	// 1.xliang [11/14/2008]  ���polycom˫�������⴦��
	if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId())
		|| ( MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId()) //��AethraMcu Ҳ�����⴦��
			&& MT_TYPE_MT != m_ptMtTable->GetMtType(tMt.GetMtId()) )
		)
	{
		//Polycom��˫�����������ģ�ͨ����Ϣ����У�飬ֱ�Ӵ�
		if ( tCapSupport.IsDStreamSupportH239() 
			&& m_tConf.GetCapSupport().GetDStreamCapSet().GetMediaType() == tCapSupport.GetDStreamMediaType())
		{
			if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
			{
				dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
				if( dwRcvIp == 0 )
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) mp not connected(Polycom)!\n",
						tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );		    
				}
				g_cMcuVcApp.FindMatchedMpIpForMt(m_ptMtTable->GetIPAddr(tMt.GetMtId()),dwRcvIp);
				wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
				m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
			}
			
			tDStreamChnnl.SetSupportH239( tCapSupport.IsDStreamSupportH239() );
			NotifyH239TokenOwnerInfo( &tMt );
			
			tDStreamChnnl.SetMediaType( MODE_SECVIDEO );
			tDStreamChnnl.SetChannelType( tCapSupport.GetDStreamMediaType() );
			
			//zbq[03/20/2008]FIXME: û��˫��Դ�����������٣�
			
			//zbq[01/31/2008] FIXME: �����˫��ͨ���������������
			//tDStreamChnnl.SetFlowControl(  );
			
			// guzh [2008/03/07] ��˿���E1 ���ٺ�Polycom���������̶���ͨ�����ʵ��ϸ�У��
			// ��һ���ֿ�����MtAdp��
			u16 wChanBitrate = m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(), FALSE );
			if ( !( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) && m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId())) )
			{
				if ( wChanBitrate == 0 )
				{
					wChanBitrate = GetDoubleStreamVideoBitrate(m_ptMtTable->GetDialBitrate( tMt.GetMtId() ), FALSE);
				}
				else
				{
					const u16 wDSVBit = GetDoubleStreamVideoBitrate(m_ptMtTable->GetDialBitrate(tMt.GetMtId()), FALSE);
					wChanBitrate = min(wChanBitrate, wDSVBit);
				}
			}
			tDStreamChnnl.SetFlowControl(wChanBitrate);
			
			tDStreamChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
			tDStreamChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 5 );        
			tDStreamChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
			tDStreamChnnl.SetActivePayload( GetActivePayload( m_tConf,tDStreamChnnl.GetChannelType() ) );    
			
			// [pengjie 2010/6/2] �򿪶Զ�˫��ͨ��ʱ����֡��
			u8 byFps = 0;
			u8 byChannelType = tDStreamChnnl.GetChannelType();
			
			if (byChannelType == m_tConf.GetDStreamMediaType())
            {
                if (MEDIA_TYPE_H264 == byChannelType)
                {
                    byFps = m_tConf.GetDStreamUsrDefFPS();
                }
                else
                {
                    byFps = m_tConf.GetDStreamFrameRate();
                }
            }
            else if (byChannelType == m_tConf.GetCapSupportEx().GetSecDSType())
            {
                byFps = m_tConf.GetCapSupportEx().GetSecDSFrmRate();
            }
            else
            {
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[McuMtOpenDoubleStreamChnnl] unexpected ds MediaType.%d, Set Fps = 0 !\n", byChannelType);
            }
			
			tDStreamChnnl.SetChanVidFPS( byFps );
			//����HP/BP����[12/9/2011 chendaiwei]
			tDStreamChnnl.SetProfieAttrb(m_tConf.GetProfileAttrb());
			// End

			cMsg.SetMsgBody( ( u8 * )&tDStreamChnnl, sizeof( tDStreamChnnl ) );
			
			u8 byVideoFormat = m_tConf.GetVideoFormat(tDStreamChnnl.GetChannelType(), LOGCHL_SECVIDEO);
			cMsg.CatMsgBody( (u8*)&byVideoFormat, sizeof(u8) );
			
			SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cMsg );
			
			//FIXME: TEST
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[McuMtOpenDoubleStreamChnnl] Mt.%d 's CurReqBR.%d, GetDSVBR.%d(polycom)\n",
				tMt.GetMtId(),
				m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE),
				GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())));
			
			// zbq [02/01/2008] �����ն˲����˴�������˫��ͨ������ɺ���֮��ͬ˵��zbq[01/30/2008]
			if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
				m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()))
			{
				//zbq[01/30/2008] ��˫���Զ��������ն˵�����������ͨ���򿪳ɹ���������㴦��
				//m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), GetDoubleStreamVideoBitrate(m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE)));            
			}
			else
			{
				// xsl [4/28/2006] ��˫��ͨ��ʱ����˫�����ʱ���������Ƶ����
				if (m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE) >= GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())))
				{
					//zbq[01/30/2008] ˫�����ʱ����Ĳο�ֵӦ���ǵ�ǰ�Ľ��մ�����Ӧ�Զ������٣�
					//m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), GetDoubleStreamVideoBitrate(
					//    /*m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()))*/m_ptMtTable->GetMtReqBitrate(tMt.GetMtId())), TRUE);
					m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())), LOGCHL_VIDEO);
				}            
			}
			
			//��ӡ��Ϣ
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open Second Video LogicChannel Request send to Mt%d, H239.%d, BR.%d, type: %s, HP:%d\n", 
				tMt.GetMtId(), tDStreamChnnl.IsSupportH239(),
				tDStreamChnnl.GetMediaType(), GetMediaStr( tDStreamChnnl.GetChannelType() ),tDStreamChnnl.GetProfileAttrb() );
		}
		else
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[McuMtOpenDoubleStreamChnnl] IsH239.%d, CapDSType.%d. ConfDSType.%d\n",
				tCapSupport.IsDStreamSupportH239(),
				tCapSupport.GetDStreamMediaType(),
				m_tConf.GetCapSupport().GetDStreamCapSet().GetMediaType());
		}
		return;
	}

    TLogicalChannel tLogicChnAdj = tLogicChnnl; //  pengjie[8/11/2009]
    //TMultiCapSupport tMultiCapSupport;
    //m_ptMtTable->GetMtMultiCapSupport(tMt.GetMtId(), &tMultiCapSupport);
    //TDStreamCap tSecComDSCap =  tMultiCapSupport.GetSecComDStreamCapSet();
    //u8 bySecDstRes = tSecComDSCap.GetResolution();

	//2.������ڵڶ���ͬ˫��������ͨ��Դ�жϽ��ն˶�̬��̬˫������ͨ��[12/9/2011 chendaiwei]
	/*u8 bySrcRes = tLogicChnnl.GetVideoFormat();
	u8 byDstRes = tCapSupport.GetDStreamCapSet().GetResolution();
	//zbq[09/02/2008] ˫��ͨ���ֱ���ƥ��У��
	if (!IsDSResMatched(bySrcRes, byDstRes))
	{
        if( IsDSResMatched(bySrcRes, bySecDstRes))
        {
            TDStreamCap tDSCap = tCapSupport.GetDStreamCapSet();
            tCapSupport.SetDStreamCapSet(tSecComDSCap);

            //�����������浽�ն˱���
            tMultiCapSupport.SetDStreamCapSet(tSecComDSCap);
            tMultiCapSupport.SetSecComDStreamCapSet(tDSCap);
            m_ptMtTable->SetMtMultiCapSupport(tMt.GetMtId(), &tMultiCapSupport);
			m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tCapSupport);
        }

        if (MT_MANU_TAIDE == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
            VIDEO_FORMAT_CIF == byDstRes)
        {
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[McuMtOpenDoubleStreamChnnl] Mt.%d's DS Format.%d adjust to Format.%d due to Tand!\n",
                tMt.GetMtId(), bySrcRes, byDstRes);
			tLogicChnAdj.SetVideoFormat(byDstRes);
        }
        else
        {
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[McuMtOpenDoubleStreamChnnl] DsSrc's Res.%d dismatched with DstMt%d's Res.%d, but go on!\n",
				bySrcRes, tMt.GetMtId(), byDstRes);
        }
	}*/

	//3.У�鷢�Ͷ˺ͽ��ն��Ƿ�ͬ����Ƶ�������Ƿ���H239��ǩ�������ն˽���˫��ͨ��[12/9/2011 chendaiwei]
	//˫���Ƿ���ͬ����Ƶ
    BOOL32 bIsEvMain = FALSE;
    TCapSupport tConfCapSupport = m_tConf.GetCapSupport();
    if ( tConfCapSupport.GetDStreamType() == VIDEO_DSTREAM_MAIN  || tConfCapSupport.GetDStreamType() == VIDEO_DSTREAM_MAIN_H239)
    {
        bIsEvMain = TRUE;
    }
    
	//tLogicChnAdj�ͻ����Ƿ�֧��H239��һ�µġ��������ն�˫��ͨ�������������
	//1.����H239��ǩ���ն�һ��
	//2.���鲻��H239��ǩ,�ն���H239��ǩ[6/28/2012 chendaiwei]
    if( (bIsEvMain || 
		tLogicChnAdj.IsSupportH239() == tCapSupport.IsDStreamSupportH239() ||
		(!m_tConf.GetCapSupport().IsDStreamSupportH239() && tCapSupport.IsDStreamSupportH239()) ))
    {
        if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
        {
            dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
            if( dwRcvIp == 0 )
            {
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) mp not connected!\n",
                    tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );		    
            }
			g_cMcuVcApp.FindMatchedMpIpForMt(m_ptMtTable->GetIPAddr(tMt.GetMtId()),dwRcvIp);
            wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
            m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
        }
        
        if( tLogicChnAdj.IsSupportH239())
        {
            tDStreamChnnl.SetSupportH239( tCapSupport.IsDStreamSupportH239() );
            NotifyH239TokenOwnerInfo( &tMt );
        }

        tDStreamChnnl.SetMediaType( MODE_SECVIDEO );
        tDStreamChnnl.SetChannelType( tCapSupport.GetDStreamMediaType() );

		if (MEDIA_TYPE_H264 == tCapSupport.GetDStreamMediaType())
		{
			tDStreamChnnl.SetChanVidFPS(tCapSupport.GetDStreamUsrDefFPS());
		}
		else
		{
			tDStreamChnnl.SetChanVidFPS(tCapSupport.GetDStreamFrmRate());
		}

		tDStreamChnnl.SetVideoFormat(tCapSupport.GetDStreamResolution());

        //zbq[01/31/2008] FIXME: �����˫��ͨ���������������
        //tDStreamChnnl.SetFlowControl(  );

        // guzh [2008/03/07] ��˿���E1 ���ٺ�Polycom���������̶���ͨ�����ʵ��ϸ�У��
        // ��һ���ֿ�����MtAdp��
        u16 wChanBitrate = m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(), FALSE );

		if( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) 
			&& (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MMCU || 
			m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU))
		{
			wChanBitrate = GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth( tMt.GetMtId() ),FALSE);
		}
        else if ( !( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) && m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId())) )
        {
			//zbq[09/10/2008] �ǽ��ٵ�E1�ն˵�˫�������ȡҪȡʵ�ʽ���ֵ
			if (m_ptMtTable->GetMtTransE1(tMt.GetMtId()))
			{
				if ( wChanBitrate == 0 )
				{
					wChanBitrate = GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth( tMt.GetMtId() ), FALSE);
				}
				else
				{
					const u16 wSecDVidBit = GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE);
					wChanBitrate = min(wChanBitrate, wSecDVidBit);
				}
			}
			else
			{
				if ( wChanBitrate == 0 )
				{
					wChanBitrate = GetDoubleStreamVideoBitrate(m_ptMtTable->GetDialBitrate( tMt.GetMtId() ), FALSE);
				}
				else
				{
					const u16 wDSVidBit = GetDoubleStreamVideoBitrate(m_ptMtTable->GetDialBitrate(tMt.GetMtId()), FALSE);
					wChanBitrate = min(wChanBitrate, wDSVidBit);
				}
			}

        }
		else
		{
			//zbq[07/17/2008] E1�����ն�ͨ���򿪴�����
			if ( wChanBitrate == 0 )
			{
				wChanBitrate = GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId()), FALSE);
			}
			else
			{
				const u16 wDSVidBit = GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId()), FALSE);
				wChanBitrate = min(wChanBitrate, wDSVidBit);
			}
		}
        tDStreamChnnl.SetFlowControl(wChanBitrate);

        tDStreamChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
        tDStreamChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 5 );        
        tDStreamChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
        tDStreamChnnl.SetActivePayload( GetActivePayload(m_tConf, tDStreamChnnl.GetChannelType() ) );        
        
		//zbq[09/02/2008] Format���ն������򿪣�����ֻ����������
        u8 byVideoFormat = tDStreamChnnl.GetVideoFormat();
		if (byVideoFormat == 0)
		{
			byVideoFormat = m_tConf.GetVideoFormat(tDStreamChnnl.GetChannelType(), LOGCHL_SECVIDEO);
		}
		tDStreamChnnl.SetVideoFormat(byVideoFormat);
		//����HP/BP����[12/9/2011 chendaiwei]
		tDStreamChnnl.SetProfieAttrb(tCapSupport.GetDStreamCapSet().GetH264ProfileAttrb());
		cMsg.SetMsgBody( ( u8 * )&tDStreamChnnl, sizeof( tDStreamChnnl ) );
        cMsg.CatMsgBody( (u8*)&byVideoFormat, sizeof(u8) );
        
        SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cMsg );

        //FIXME: TEST
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[McuMtOpenDoubleStreamChnnl] Mt.%d 's CurReqBR.%d, GetDSVBR.%d, Format.%d, Fps:%d\n",
                 tMt.GetMtId(),
				 m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE),
				 GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())),
				 byVideoFormat, tDStreamChnnl.GetChanVidFPS());

        // zbq [02/01/2008] �����ն˲����˴�������˫��ͨ������ɺ���֮��ͬ˵��zbq[01/30/2008]
        if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
             m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()))
        {
            //zbq[01/30/2008] ��˫���Զ��������ն˵�����������ͨ���򿪳ɹ���������㴦��
            //m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), GetDoubleStreamVideoBitrate(m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE)));            
        }
        else
        {
            // xsl [4/28/2006] ��˫��ͨ��ʱ����˫�����ʱ���������Ƶ����
            if (m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE) >= GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())))
            {
                //zbq[01/30/2008] ˫�����ʱ����Ĳο�ֵӦ���ǵ�ǰ�Ľ��մ�����Ӧ�Զ������٣�
                //m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), GetDoubleStreamVideoBitrate(
                //    /*m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()))*/m_ptMtTable->GetMtReqBitrate(tMt.GetMtId())), TRUE);
                m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())), LOGCHL_VIDEO);
            }            
        }
                
        //��ӡ��Ϣ
        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open Second Video LogicChannel Request send to Mt%d, DStream.%d, type: %s, HP:%d\n", 
                tMt.GetMtId(), tDStreamChnnl.IsSupportH239(), GetMediaStr( tDStreamChnnl.GetChannelType() ),tDStreamChnnl.GetProfileAttrb() );
    }
    else
    {
        //��ӡ��Ϣ
        ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "Mt%d <%d-H239.%d> not support any conf second video media type<%d-H239.%d>, \
                 no open video logicChannel request send to it!\n", 
                 tMt.GetMtId(),
                 tCapSupport.GetDStreamMediaType(), tCapSupport.IsDStreamSupportH239(),
                 tLogicChnAdj.GetChannelType(), tLogicChnAdj.IsSupportH239());
    }

    return;
}

/*=============================================================================
    �� �� ���� StartSwitchDStreamToFirstLChannel
    ��    �ܣ� ��ʼ ��˫�����ݽ�����ָ���ն˵ĵ�һ·��Ƶͨ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMt &tDstMt
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/6/4    3.6			����                  ����
=============================================================================*/
void CMcuVcInst::StartSwitchDStreamToFirstLChannel( TMt &tDstMt )
{
	//˫������ ������Ƶ��ʽһ�� �ķ�ʽ����֧�ֵ� ������˫�����ݽ����õ�һ·��Ƶͨ����
	if( m_tConf.GetCapSupport().IsDStreamSupportH239() ||
		m_tDoubleStreamSrc.IsNull() )
	{
		return;
	}

	TLogicalChannel tLogicChnnl;
	if( MT_MANU_SONY == m_ptMtTable->GetManuId(tDstMt.GetMtId()) &&
        !m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicChnnl, TRUE ) && 
		m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_VIDEO, &tLogicChnnl, TRUE ) && 
		tLogicChnnl.GetChannelType() == m_tConf.GetCapSupport().GetDStreamMediaType() )
	{	
		//zjl 20110510 StopSwitchToSubMt �ӿ������滻
		//�������
		//StopSwitchToSubMt( tDstMt.GetMtId(), MODE_VIDEO );
		StopSwitchToSubMt(1, &tDstMt, MODE_VIDEO);

		m_ptMtTable->SetMtVideo2Recv( tDstMt.GetMtId(), TRUE );

        //zbq [08/23/2007] ¼�����˫��������Ҫ���ݷ���ͨ������
        u8 bySrcChn = m_tDoubleStreamSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
		//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
		//g_cMpManager.StartSwitchToSubMt( m_tDoubleStreamSrc, bySrcChn, tDstMt, MODE_SECVIDEO, SWITCH_MODE_BROADCAST, FALSE );
		TSwitchGrp tSwitchGrp;
		tSwitchGrp.SetSrcChnl(bySrcChn);
		tSwitchGrp.SetDstMtNum(1);
		tSwitchGrp.SetDstMt(&tDstMt);
		g_cMpManager.StartSwitchToAll(m_tDoubleStreamSrc, 1, &tSwitchGrp, MODE_SECVIDEO);


		//�����ն�״̬
		m_ptMtTable->SetMtSrc( tDstMt.GetMtId(), ( const TMt * )&m_tDoubleStreamSrc, MODE_VIDEO );

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "StartSwitchDStreamToFirstLChannel Mt%d\n", tDstMt.GetMtId() );
	}

	return;
}

/*=============================================================================
    �� �� ���� StopSwitchDStreamToFirstLChannel
    ��    �ܣ� ֹͣ ��˫�����ݽ�����ָ���ն˵ĵ�һ·��Ƶͨ����,�ָ�Ϊ����ƵԴ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMt &tDstMt
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/6/4    3.6			����                  ����
=============================================================================*/
void CMcuVcInst::StopSwitchDStreamToFirstLChannel( TMt &tDstMt )
{
	if( m_tConf.GetCapSupport().IsDStreamSupportH239() )
	{
		return;
	}
	
	TLogicalChannel tLogicChnnl;
	if( MT_MANU_SONY == m_ptMtTable->GetManuId(tDstMt.GetMtId()) &&
        !m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicChnnl, TRUE ) && 
		m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_VIDEO, &tLogicChnnl, TRUE ) && 
		tLogicChnnl.GetChannelType() == m_tConf.GetCapSupport().GetDStreamMediaType() )
	{
		//�ָ�Ϊ����ƵԴ
		RestoreRcvMediaBrdSrc( tDstMt.GetMtId(), MODE_VIDEO, TRUE );
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "StopSwitchDStreamToFirstLChannel Mt%d\n", tDstMt.GetMtId() );
	}

	return;
}

/*=============================================================================
    �� �� ���� JudgeIfSwitchFirstLChannel
    ��    �ܣ� �б�Ŀ���ն��Ƿ�Ϊ �ѽ��õ�һ·��Ƶͨ������˫�����ն�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMt &tSrc
               u8 bySrcChnnl
               TMt &tDstMt
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/6/4    3.6			����                  ����
=============================================================================*/
BOOL32 CMcuVcInst::JudgeIfSwitchFirstLChannel( TMt &tSrc, u8 bySrcChnnl, TMt &tDstMt )
{
	BOOL32 bRet = FALSE;

	if( m_tConf.GetCapSupport().IsDStreamSupportH239() ||  
		m_tDoubleStreamSrc.IsNull() )
	{
		return bRet;
	}
	
	//Ŀǰ��ʱ����SONY�նˣ�֮�������ͨ��������֧������б�
	TLogicalChannel tLogicChnnl;
	u8 byManuID = m_ptMtTable->GetManuId(tDstMt.GetMtId());
	if( MT_MANU_SONY == byManuID &&
		!m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicChnnl, TRUE ) && 
		m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_VIDEO, &tLogicChnnl, TRUE ) && 
		tLogicChnnl.GetChannelType() == m_tConf.GetCapSupport().GetDStreamMediaType() )
	{
		bRet = TRUE;
	}

	ConfPrint(LOG_LVL_DETAIL,MID_MCU_MPMGR,"[JudgeIfSwitchFirstLChannel] tSrc<mcuId.%d MtId.%d>bySrcChnnl = %d\n",tSrc.GetMcuId(),tSrc.GetMtId(),bySrcChnnl);

	return bRet;
}

/*=============================================================================
    �� �� ���� ClearH239TokenOwnerInfo
    ��    �ܣ� �����ǰ����� H239˫������ӵ���ն���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMt *ptMt�ǿ�ʱ����֪ͨ
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/6/9    3.6			����                  ����
=============================================================================*/
void CMcuVcInst::ClearH239TokenOwnerInfo( TMt *ptMt )
{
	if( m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() )
	{
		return;
	}

	if(  NULL == ptMt || 
		(NULL != ptMt && m_tH239TokenOwnerInfo.m_tH239TokenMt == *ptMt) )
	{
		//REQUEST RELEASING TOKEN OF LAST MT OWNER
		//zjl [02/25/2010] �����ǰ���ƻ�ӵ�������ϼ�mcu���ͷ����ƻ���Ϣ��mtadp��ת�����ƻ������ϼ�mcu
		//r6mcu�����������˸�����r2�򲻹��ˣ�ֱ����Ϊ���µ��ն˻�mcu�������ƻ�������������˫��Դtoken������
		// [pengjie 2010/2/26] �����˫�����񣬼����ƻ�ӵ���������裬����֪ͨ��ֻClear���ͺ�
		if( NULL != ptMt && /*MT_MANU_KDCMCU != m_ptMtTable->GetManuId(m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId()) &&*/
			m_tH239TokenOwnerInfo.m_tH239TokenMt.GetType() != TYPE_MCUPERI)
		{
			CServMsg cServMsg;
			TH239TokenInfo tH239Info;
			tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
			tH239Info.SetSymmetryBreaking( 0 );
			cServMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
			cServMsg.SetMsgBody((u8 *)&m_tH239TokenOwnerInfo.m_tH239TokenMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId(), MCU_MT_RELEASEH239TOKEN_CMD, cServMsg);
		}
		u8 byTokenMtId = m_tH239TokenOwnerInfo.GetTokenMt().GetMtId();
		u8 byChnId = m_tH239TokenOwnerInfo.GetChannelId();
		m_tH239TokenOwnerInfo.Clear();
		if( m_byNewTokenReqMtid != 0 &&
			MT_MANU_KDCMCU == m_ptMtTable->GetManuId(byTokenMtId))
		{
			m_tH239TokenOwnerInfo.SetChannelId(byChnId);
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ClearH239TokenOwnerInfo]Old DsSrc(%d) is KEDAMCU AND HAS NEWTOKENMT(%d) ,So Reccord The Chniid!!\n",m_tDoubleStreamSrc.GetMtId(),byChnId);
		}
	}
	
	return;
}

/*=============================================================================
    �� �� ���� UpdateH239TokenOwnerInfo
    ��    �ܣ� ���µ�ǰ����� H239˫������ӵ���ն���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��   -------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/6/9    3.6			����                  ����
=============================================================================*/
void CMcuVcInst::UpdateH239TokenOwnerInfo(TMt &tMt)
{
	if( !m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() )
	{
		//TD:------FIRST REQUEST TOKEN,NEXT OPENCHANNEL
		if( m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt )
		{
		}
		//REQUEST RELEASING TOKEN OF LAST MT OWNER
		else
		{
			if (MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tMt.GetMtId()))
			{
				CServMsg cServMsg;
				TH239TokenInfo tH239Info;
				tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
				tH239Info.SetSymmetryBreaking( 0 );
				cServMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
				cServMsg.SetMsgBody((u8 *)&m_tH239TokenOwnerInfo.m_tH239TokenMt, sizeof(TMt));
				cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
				SendMsgToMt( m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId(), MCU_MT_RELEASEH239TOKEN_CMD, cServMsg);

			}

			m_tH239TokenOwnerInfo.Clear();
		}
	}
	
	return;
}

/*=============================================================================
    �� �� ���� NotifyH239TokenOwnerInfo
    ��    �ܣ� ֪ͨ����ֱ���ն� ��ǰ����� H239˫������ӵ���ն���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMt *ptMt
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/6/9    3.6			����                  ����
=============================================================================*/
void CMcuVcInst::NotifyH239TokenOwnerInfo(TMt *ptMt)
{
	if( m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() )
	{
		return;
	}

	TMt tLocalMt;
	CServMsg cServMsg;
	TH239TokenInfo tH239Info;
	tH239Info.Clear();

	if( NULL == ptMt )
	{
		for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			// [pengjie 2010/2/27] Modify ������ƻ���ӵ���������裬�򷢸������նˣ��������˵�����id��ͬ���Ǹ�
			//  ��ΪGetMtId()�ͻὫ�����ID����ĳ���ն˵�ID����������ĳ���ն˿����ղ�������֪ͨ			
			if( m_tH239TokenOwnerInfo.m_tH239TokenMt.GetType() != TYPE_MCUPERI )
			{
				if( byLoop == m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId() )
				{
					continue;
				}
			}
			// End

			if( !m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
			{
				continue;
			}
			
			tLocalMt = m_ptMtTable->GetMt( byLoop );

			tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
			cServMsg.SetEventId(MCU_MT_OWNH239TOKEN_NOTIF);
			cServMsg.SetMsgBody((u8 *)&tLocalMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( tLocalMt.GetMtId(), MCU_MT_OWNH239TOKEN_NOTIF, cServMsg);
		}
	}
	else
	{
		// [pengjie 2010/2/26] Modify ������ƻ�ӵ���������裬��ֱ�ӷ���Ŀ���նˣ����ù���
		if( m_tH239TokenOwnerInfo.m_tH239TokenMt.GetType() == TYPE_MCUPERI )
		{
			tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
			cServMsg.SetEventId(MCU_MT_OWNH239TOKEN_NOTIF);
			cServMsg.SetMsgBody((u8 *)ptMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( ptMt->GetMtId(), MCU_MT_OWNH239TOKEN_NOTIF, cServMsg);
		}
		// ����Ҫ�ж����ƻ�ӵ�����Ƿ���Ҫ֪ͨ���Ǹ��նˣ��������ų�
		else
		{   
			if( ptMt->GetMtId() != m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId() )
			{
				tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
				cServMsg.SetEventId(MCU_MT_OWNH239TOKEN_NOTIF);
				cServMsg.SetMsgBody((u8 *)ptMt, sizeof(TMt));
				cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
				SendMsgToMt( ptMt->GetMtId(), MCU_MT_OWNH239TOKEN_NOTIF, cServMsg);
			}
		}
	}

	return;
}
/*=============================================================================
    �� �� ���� ProcMtSelectdByOtherMt
    ��    �ܣ� ����ָ���ն˱������ն�ѡ����������Keda�ն���Ƶʱ��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byMtId     ��ѡ�����ն�ID
	           BOOL32 bStartSwitch �Ƿ񽻻���ȥ
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/02/23  4.0			������                  ����
=============================================================================*/
void CMcuVcInst::ProcMtSelectdByOtherMt( u8 byMtId, BOOL32 bStartSwitch )
{
	TMt tSrcMt = m_ptMtTable->GetMt( byMtId );
	if( tSrcMt.IsNull() )
	{
		return;
	}

	TMt atDstMt[MAXNUM_CONF_MT];
	u8 byDstMtNum = 0;

	TMtStatus tMtStatus;
	TMt  tDstMt;
	u16 wSpyPort = SPY_CHANNL_NULL;
	for ( u8 byMtIdxLp = 1; byMtIdxLp <= MAXNUM_CONF_MT; byMtIdxLp ++ )
	{
		if( byMtIdxLp != byMtId ) // �ų�����
		{
			tDstMt = m_ptMtTable->GetMt( byMtIdxLp );
			if( !tDstMt.IsNull() ) // ���ն��Ƿ���Ч
			{
				m_ptMtTable->GetMtStatus(byMtIdxLp, &tMtStatus);
				// �����¼���MCU���ն����ϼ��ն˻���ѡ����ʵ��Ϊ�¼�MCU���ϼ��ն˻���ѡ��
				TMt tSelMt = tMtStatus.GetSelectMt(MODE_AUDIO);

				// [1/4/2011 xliang] filter null tSelMt
				if ( tSelMt.IsNull() )
				{
					continue;
				}

				if( (!tSelMt.IsLocal() && GetFstMcuIdFromMcuIdx( tSelMt.GetMcuId() ) == byMtId) ||
					tMtStatus.GetSelectMt(MODE_AUDIO) == tSrcMt ) // ������Ƶ����
				{
					if( !tSelMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tSelMt.GetMcuId()) )
					{
						CRecvSpy cRecvSpy;
						if( m_cSMcuSpyMana.GetRecvSpy( tSelMt, cRecvSpy ) )
						{
							wSpyPort = cRecvSpy.m_tSpyAddr.GetPort();
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT, "[ProcMtSelectdByOtherMt] GetspyPort is��%d\n", wSpyPort);
						}
					}
					if( bStartSwitch )
					{
						if ( IsNeedSelAdpt(tSelMt, tDstMt, MODE_AUDIO)  )
						{
							StartSelAdapt(tSelMt, tDstMt, MODE_AUDIO);
						}
						else
						{
							//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt�������п��ܶ��spyport
							//StartSwitchToSubMt( tSrcMt, 0,  byMtIdxLp, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE, TRUE, FALSE, wSpyPort );
							TSwitchGrp tSwitchGrp;
							tSwitchGrp.SetSrcChnl(0);
							tSwitchGrp.SetDstMtNum(1);
							tSwitchGrp.SetDstMt(&tDstMt);
							StartSwitchToAll(tSrcMt, 1, &tSwitchGrp, MODE_AUDIO, SWITCH_MODE_SELECT, TRUE,  TRUE, wSpyPort);
						}
					}
					else
					{
						//zjl 20110510 StopSwitchToSubMt �ӿ������滻
						//StopSwitchToSubMt( tDstMt.GetMtId(), MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE, TRUE, TRUE, wSpyPort);
						atDstMt[byDstMtNum] = tDstMt;
						byDstMtNum++;
					}
				}
			}
		}
	}
 
	if (byDstMtNum > 0)
	{
		StopSwitchToSubMt(byDstMtNum, atDstMt, MODE_AUDIO, TRUE, FALSE, TRUE);
	}


	// ����VCS MCS��ǰѡ���ն�Ϊ����������ʱ��Ҫ������Ӧ�Ľ��𽻻�
	//zjj20120717 �����洦��,����ע��
	/*TMt tMCAudSrc;
	tMCAudSrc.SetNull();
	for (u8 byMCIdx = 1; byMCIdx <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byMCIdx++)
	{
		for (u8 byChnl = 0; byChnl < MAXNUM_MC_CHANNL; byChnl++)
		{
			if (g_cMcuVcApp.GetMcSrc(byMCIdx, &tMCAudSrc, byChnl, MODE_AUDIO) &&
				((!tMCAudSrc.IsLocal() && GetFstMcuIdFromMcuIdx(tMCAudSrc.GetMcuId()) == tSrcMt.GetMtId()) ||
				  tMCAudSrc == tSrcMt))
			{
				if (bStartSwitch)
				{
				    g_cMpManager.StartSwitchToMc(tMCAudSrc, 0, byMCIdx, byChnl, MODE_AUDIO, wSpyPort);
				}
				else
				{
					g_cMpManager.StopSwitchToMc(m_byConfIdx, byMCIdx, byChnl, MODE_AUDIO);
				}
			}
		}		
	}*/


	if( !m_tCascadeMMCU.IsNull() )
	{		
		m_ptMtTable->GetMtStatus(m_tCascadeMMCU.GetMtId(), &tMtStatus);		
    
		tDstMt = GetLocalMtFromOtherMcuMt( tMtStatus.GetAudioMt() );
		if( !tDstMt.IsNull() && byMtId == tDstMt.GetMtId() )
		{
			if( bStartSwitch )
			{
				tDstMt.SetNull();
				m_ptMtTable->SetMtSrc( m_tCascadeMMCU.GetMtId(),&tDstMt,MODE_AUDIO );
				StartSwitchToMcu(tMtStatus.GetVideoMt(), 0, m_tCascadeMMCU.GetMtId(), MODE_AUDIO, SWITCH_MODE_SELECT);
			}
			else
			{
				StopSpyMtCascaseSwitch( MODE_AUDIO );
				tDstMt = tMtStatus.GetAudioMt();
				m_ptMtTable->SetMtSrc( m_tCascadeMMCU.GetMtId(),&tDstMt,MODE_AUDIO );
			}
		}		
	}
	

	return;
}

/*=============================================================================
    �� �� ���� FindMtAndAdjustMtRecSrcStream
    ��    �ܣ� ������KDC��̩�£���Ƶ��������������ֵ����㲥��ƵԴ��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 tMcu     Ҫ���ҵ��ն����ڵ�mcu
               u8 byMode  Ҫ����������������Ƶģʽ
               BOOL32 bIsStart   �Ƿ�𽨽���
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2012/04/11  4.7			�ܾ���                  ����
=============================================================================*/
void CMcuVcInst::FindMtAndAdjustMtRecSrcStream( const TMt &tMcu,const u8 byMode,const BOOL32 bIsStart )
{
	/*if( !IsMcu(tMcu) )
	{
		return;
	}*/

	u8 byRecId = RECORDERID_MIN;
	TPeriEqpStatus tRecStatus;
	TRecChnnlStatus tStatus;
	u8 byRecChnNum = 0;
	TEqp tEqp;
	TMt tLocalMt;
    while( byRecId >= RECORDERID_MIN && byRecId <= RECORDERID_MAX )
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byRecId))
		{
			g_cMcuVcApp.GetPeriEqpStatus(byRecId, &tRecStatus);
            if (tRecStatus.m_byOnline == 1)
            {                
                byRecChnNum = tRecStatus.m_tStatus.tRecorder.GetRecChnnlNum();
				if (0 == byRecChnNum)
				{
					++byRecId;
					ConfPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[RemoveMtFormPeriInfo] byRecId just is zero!\n");
					continue;
				}

				tEqp = (TEqp)tRecStatus;
				for(u8 byLoop = 0; byLoop < byRecChnNum; byLoop++)
				{
					if(FALSE == tRecStatus.m_tStatus.tRecorder.GetChnnlStatus(byLoop, TRecChnnlStatus::TYPE_RECORD, &tStatus))
					{
						continue;
					}

					if (tStatus.m_tSrc.IsNull())
					{
						continue;
					}

					if (tStatus.m_tSrc.GetConfIdx() != m_byConfIdx)
					{
						continue;
					}
					tLocalMt = GetLocalMtFromOtherMcuMt(tStatus.m_tSrc);
					if ( tMcu.GetMtId() == tLocalMt.GetMtId() )
					{						
						AdjustMtRecSrcStream(byMode, tEqp, byLoop, TMt(tStatus.m_tSrc), bIsStart);
					}					
				}				
			}			
		}
		++byRecId;
	}
}

/*=============================================================================
    �� �� ���� AdjustTaideAudioSwitch
    ��    �ܣ� ������KDC��̩�£���Ƶ��������������ֵ����㲥��ƵԴ��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byDstMtId     ���������ն�ID
               BOOL32 bDecodeAud  TRUE-���뾲������ FALSE-������������
               BOOL32 bOPenFlag   TRUE-��         FALSE-�ر�
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/03/25  3.6			����                  ����
=============================================================================*/
void CMcuVcInst::AdjustKedaMcuAndTaideAudioSwitch( u8 byDstMtId, BOOL32 bDecodeAud, BOOL32 bOpenFlag )
{
    // xsl [9/20/2006] ��Ҫ���ǹ㲥ԴΪ�����������
	//zhouyiliang 20101116 ��������local����srcmt���������½���Ƶ������ʱ����ȷ��
	//TMt tAudSrcMt = GetLocalAudBrdSrc();
	TMt tAudSrcMt = GetAudBrdSrc();
	u16 wSpyStartPort = SPY_CHANNL_NULL;
	CRecvSpy cRcvSpy;

	//�ն˾�����ֹͣ�ⲿ����ն˽�����Ƶ
	if( TRUE == bDecodeAud && TRUE == bOpenFlag )
	{
        //��ȡ��ѡ��
		//StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE, FALSE );
		TMt tMt = m_ptMtTable->GetMt(byDstMtId);
		StopSwitchToSubMt(1, &tMt, MODE_AUDIO, FALSE, FALSE);
	}
		
	TMtStatus tMtStatus;
	m_ptMtTable->GetMtStatus(byDstMtId, &tMtStatus);

	//ȡ���ն˾������ָ��ⲿ����ն˽�����Ƶ
	if( TRUE == bDecodeAud && FALSE == bOpenFlag )
	{
		TMt tSelectMt = tMtStatus.GetSelectMt(MODE_AUDIO);

        //û����Ƶѡ��
		if(tSelectMt.IsNull() && !tAudSrcMt.IsNull())
		{
            // xsl [9/26/2006] �л������Ȼָ�������
			//tianzhiyong 2010/03/21 ����EAPU���ͻ�����
            if (tAudSrcMt.GetType() == TYPE_MCUPERI && tAudSrcMt.GetEqpType() == EQP_TYPE_MIXER )
            {
                TMt tDstMt = m_ptMtTable->GetMt(byDstMtId);
				
				// fxh[Bug00014234��������]���ƻ����ǻ������ն�ȡ������Nģʽ����˿�������
				// �����ƻ������к�����ģʽ���ն�ȡN-1ģʽ����˿�����
                if (m_tConf.m_tStatus.IsSpecMixing() && !m_ptMtTable->IsMtInMixing(byDstMtId))
                {
                    SwitchMixMember(&tDstMt, TRUE);
                }
                else
                {
                    SwitchMixMember(&tDstMt, FALSE);
                }                
            }
            else
            {
				//zhouyiliang 20101116 ��local����srcmtҪ��spyport���룬�������½���Ƶ������ʱ����ȷ��				
				if ( !tAudSrcMt.IsLocal() )
				{					
					if ( m_cSMcuSpyMana.GetRecvSpy(tAudSrcMt,cRcvSpy) ) 
					{
						wSpyStartPort = cRcvSpy.m_tSpyAddr.GetPort();
					}
				}
				//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
                //StartSwitchToSubMt( tAudSrcMt, 0,  byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE ,TRUE,FALSE,wSpyStartPort);
				TMt tMt = m_ptMtTable->GetMt(byDstMtId);
				if ( IsNeedAdapt(tAudSrcMt, tMt, MODE_AUDIO) &&
					m_tConf.m_tStatus.IsAudAdapting()
					)
				{
					StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_AUDIO);
				}
				else
				{
					TSwitchGrp tSwitchGrp;
					tSwitchGrp.SetSrcChnl(0);
					tSwitchGrp.SetDstMtNum(1);
					tSwitchGrp.SetDstMt(&tMt);
					StartSwitchToAll(tAudSrcMt, 1, &tSwitchGrp, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE, TRUE, wSpyStartPort);
				}
			}			     
		}
		else
		{	
			//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
            //�ָ���Ƶѡ��
			//StartSwitchToSubMt( tSelectMt, 0,  byDstMtId, MODE_AUDIO, SWITCH_MODE_SELECT, FALSE );
			TMt tMt = m_ptMtTable->GetMt(byDstMtId);
			TSwitchGrp tSwitchGrp;
			if( !tSelectMt.IsNull() )
			{
				if ( !tSelectMt.IsLocal() )
				{					
					if ( m_cSMcuSpyMana.GetRecvSpy(tSelectMt,cRcvSpy) ) 
					{
						wSpyStartPort = cRcvSpy.m_tSpyAddr.GetPort();
					}
				}				
				if ( IsNeedSelAdpt(tSelectMt, tMt, MODE_AUDIO) )
				{
					StartSelAdapt(tSelectMt, tMt, MODE_AUDIO);
				}
				else
				{
					tSwitchGrp.SetSrcChnl(0);
					tSwitchGrp.SetDstMtNum(1);				
					tSwitchGrp.SetDstMt(&tMt);
					StartSwitchToAll(tSelectMt, 1, &tSwitchGrp,  MODE_AUDIO, SWITCH_MODE_SELECT, FALSE,FALSE,wSpyStartPort );
				}
			}
			if( !m_tCascadeMMCU.IsNull() && byDstMtId == m_tCascadeMMCU.GetMtId() )
			{
				m_ptMtTable->GetMtStatus(m_tCascadeMMCU.GetMtId(), &tMtStatus);    
				tMt = tMtStatus.GetAudioMt();
				if( !tMt.IsNull() )
				{
					if ( !tMt.IsLocal() )
					{					
						if ( m_cSMcuSpyMana.GetRecvSpy(tMt,cRcvSpy) ) 
						{
							wSpyStartPort = cRcvSpy.m_tSpyAddr.GetPort();
						}
					}
					tSelectMt = tMt;
					tMt = m_tCascadeMMCU;
					if ( IsNeedSelAdpt(tSelectMt, tMt, MODE_AUDIO)  )
					{
						StartSelAdapt(tSelectMt, tMt, MODE_AUDIO);
					}
					else
					{
						tSwitchGrp.SetSrcChnl(0);
						tSwitchGrp.SetDstMtNum(1);				
						tSwitchGrp.SetDstMt( &tMt );
						StartSwitchToAll(tSelectMt, 1, &tSwitchGrp,  MODE_AUDIO, SWITCH_MODE_SELECT, FALSE,FALSE,wSpyStartPort );
					}
				}
			}
		}
	}

	//�ն�������ֹͣ���ն����ⲿ������Ƶ
	if( FALSE == bDecodeAud && TRUE == bOpenFlag )
	{
		//ֹͣ���������Ľ���
		if (!m_tMixEqp.IsNull() && (m_tConf.m_tStatus.IsMixing() || m_tConf.m_tStatus.IsVACing()) && m_ptMtTable->IsMtInMixGrp(byDstMtId))
		{
			TMt tmt = m_ptMtTable->GetMt(byDstMtId);
			StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
				(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tmt)), 
				FALSE, MODE_AUDIO, SWITCH_MODE_BROADCAST);
		}
		if( tAudSrcMt.IsNull() )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[AdjustKedaMcuAndTaideAudioSwitch] tAudSrcMt is NULL\n");

			//[2011/12/23/zhangli]����㲥��ƵԴΪNULL��������ն˱�ѡ��
			ProcMtSelectdByOtherMt(byDstMtId, FALSE);
		}
		else
		{
			if (tAudSrcMt.GetType() == TYPE_MCUPERI)
			{
			}
			else
			{
				//�������ն��Ƿ�����
				if( ( tAudSrcMt.IsLocal() && tAudSrcMt.GetMtId() == byDstMtId ) 
					|| ( !tAudSrcMt.IsLocal() && GetFstMcuIdFromMcuIdx( tAudSrcMt.GetMcuId() ) == byDstMtId) 
					)
					//if( (tAudSrcMt.GetMtId() == byDstMtId) && (m_tConf.GetSpeaker().IsLocal() && IsSupportMultiSpy() || !IsSupportMultiSpy()))
				{
					TMt atDstMt[MAXNUM_CONF_MT];
					memset(atDstMt, 0, sizeof(atDstMt));
					u8  byDstMtNum = 0;
					TMt tMtSel;
					TMtStatus status;
					
					for( u8 byMtLoop = 1; byMtLoop <= MAXNUM_CONF_MT; byMtLoop++ )
					{
						if( m_tConfAllMtInfo.MtJoinedConf( byMtLoop ) )
						{	
							//������������¼�MCU���Ը�MCU��������ʱҲҪ����Լ�����Լ�����Ƶ���������¼������յ�����
							if( tAudSrcMt.IsLocal() && tAudSrcMt.GetMtId() == byMtLoop && 
								m_ptMtTable->GetMtType(byMtLoop) == MT_TYPE_MT)
							{
								continue;
							}
							m_ptMtTable->GetMtStatus( byMtLoop,&status );
							tMtSel = status.GetSelectMt( MODE_AUDIO );
							if( !tMtSel.IsNull() && 
								( !( tMtSel == tAudSrcMt ) && !( GetLocalMtFromOtherMcuMt(tMtSel) == GetLocalMtFromOtherMcuMt(tAudSrcMt) ) )
								)
							{
								continue;
							}
							//zjl 20110510 StopSwitchToSubMt �ӿ������滻
							//StopSwitchToSubMt( byMtLoop, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE );
							atDstMt[byDstMtNum] = m_ptMtTable->GetMt(byMtLoop);
							byDstMtNum ++;
						}
					}

					if (byDstMtNum > 0)
					{
						StopSwitchToSubMt(byDstMtNum, atDstMt, MODE_AUDIO, FALSE);
					}
					
				}
				else
				{
					//������ն˱������ն�ѡ��
					ProcMtSelectdByOtherMt(byDstMtId, FALSE);
				}
			}
		}
        
		TMt tMt = m_ptMtTable->GetMt( byDstMtId );
		
		if (m_tConf.GetConfSource() == VCS_CONF)
		{
			VCSFindMtInTvWallAndChangeSwitch( tMt,TRUE,MODE_AUDIO );
		} 
		else
		{
			MCSFindMtInTvWallAndChangeSwitch( tMt,TRUE,MODE_AUDIO );
		}	

		if( !m_tConf.m_tStatus.IsNoRecording() && tAudSrcMt.GetType() != TYPE_MCUPERI &&
			GetLocalAudBrdSrc().GetMtId() == byDstMtId )
		{
			AdjustConfRecordSrcStream(MODE_AUDIO, FALSE);
		}
		
		FindMtAndAdjustMtRecSrcStream( tMt,MODE_AUDIO,FALSE );		

		AdjustSwitchToAllMcWatchingSrcMt( tMt,FALSE,MODE_AUDIO,TRUE );

		AdjustSwitchToMonitorWatchingSrc( tMt,MODE_AUDIO,TRUE,TRUE );

		CSendSpy cSendSpy;
		if( !m_tCascadeMMCU.IsNull() && m_cLocalSpyMana.GetSpyChannlInfo( tMt,cSendSpy ) )
		{
			if( MODE_BOTH == cSendSpy.GetSpyMode() || MODE_AUDIO == cSendSpy.GetSpyMode() )
			{
				TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(tMt.GetMtId());
				if (cSendSpy.GetSimCapset().GetAudioMediaType() != tSrcCap.GetAudioMediaType() &&
					m_ptMtTable->IsLogicChnnlOpen( tMt.GetMtId(), LOGCHL_AUDIO, FALSE ) &&
					!cSendSpy.GetSimCapset().IsNull() )
				{
					StopSpyAdapt(tMt, cSendSpy.GetSimCapset(), MODE_AUDIO);
				}				
				g_cMpManager.StopSwitchToSubMt(m_byConfIdx, 1, &m_tCascadeMMCU, MODE_AUDIO, cSendSpy.m_tSpyAddr.GetPort());	
			}			
		}
	}

	//ȡ���ն��������ָ����ն����ⲿ������Ƶ
	if( FALSE == bDecodeAud && FALSE == bOpenFlag )
	{
		//�ָ����������Ľ���
		if (!m_tMixEqp.IsNull() && (m_tConf.m_tStatus.IsMixing() || m_tConf.m_tStatus.IsVACing()) && m_ptMtTable->IsMtInMixGrp(byDstMtId))
		{
			TMt tSrcMt = m_ptMtTable->GetMt(byDstMtId);
			StartSwitchToPeriEqp(tSrcMt, 0, m_tMixEqp.GetEqpId(), 
				(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tSrcMt)),
				MODE_AUDIO, SWITCH_MODE_SELECT);
        }
		if( tAudSrcMt.IsNull() )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[AdjustKedaMcuAndTaideAudioSwitch] tAudSrcMt is NULL\n");
		}

        if (tAudSrcMt.GetType() == TYPE_MCUPERI)
        {
        }
        else
        {
            //�������ն��Ƿ�����
			if( tAudSrcMt.GetType() == TYPE_MT && 
				( ( tAudSrcMt.IsLocal() && tAudSrcMt.GetMtId() == byDstMtId ) ||
				  ( !tAudSrcMt.IsLocal() && GetFstMcuIdFromMcuIdx( tAudSrcMt.GetMcuId() ) == byDstMtId) )
				)
		    {
				u16 wSpyPort = SPY_CHANNL_NULL;
				if( !tAudSrcMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tAudSrcMt.GetMcuId()) )
				{
					CRecvSpy cRecvSpy;
					if( m_cSMcuSpyMana.GetRecvSpy( tAudSrcMt, cRecvSpy ) )
					{
						wSpyPort = cRecvSpy.m_tSpyAddr.GetPort();
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[AdjustKedaMcuAndTaideAudioSwitch] GetspyPort is��%d\n", wSpyPort);
					}
				}

				//zjj20120627 �ն�ȡ������,������������
				if ( tAudSrcMt == GetVidBrdSrc() )
				{
					StartSwitchAud2Perieqp(tAudSrcMt);		
				}

			    StartSwitchToAllSubMtJoinedConf( tAudSrcMt, 0, wSpyPort);
				//ĿǰStartSwitchToAllSubMtJoinedConf����ֱ�ӽ�����Ƶ���佻��������Ƶ����㲥�������н���
				if ( m_tConf.m_tStatus.IsAudAdapting()  )
				{
					u8 byNum = 0;
					CBasChn *apcBasChn[MAXNUM_PERIEQP]={NULL};
					if (GetBasBrdChnGrp(byNum, apcBasChn, MODE_AUDIO))
					{
						TEqp tTempBas;
						for ( u8 byBasLoop =0; byBasLoop<byNum; byBasLoop++ )
						{
							tTempBas = apcBasChn[byBasLoop]->GetBas();
							StartSwitchToSubMtNeedAdp(tTempBas, apcBasChn[byBasLoop]->GetChnId());
							StartBasPrsSupport(tTempBas, apcBasChn[byBasLoop]->GetChnId());
						}
					}
				}
		    }
// 		    else
// 		    {
//                 //������ն˱������ն�ѡ��
// 			    ProcMtSelectdByOtherMt(byDstMtId, TRUE);
// 		    }
        }
		
		//[2011/12/23/zhangli]����ѡ���������������û����Ƶ�㲥Դ��ѡ����Ҫ����
		ProcMtSelectdByOtherMt(byDstMtId, TRUE);

		TMt tMt = m_ptMtTable->GetMt( byDstMtId );
		
			
		if (m_tConf.GetConfSource() == VCS_CONF)
		{
			VCSFindMtInTvWallAndChangeSwitch( tMt,FALSE,MODE_AUDIO );
		} 
		else
		{
			MCSFindMtInTvWallAndChangeSwitch( tMt,FALSE,MODE_AUDIO );
		}

		if( !m_tConf.m_tStatus.IsNoRecording() && tAudSrcMt.GetType() != TYPE_MCUPERI && 
						GetLocalAudBrdSrc().GetMtId() == byDstMtId )
		{
			AdjustConfRecordSrcStream(MODE_AUDIO, TRUE);
		}

		FindMtAndAdjustMtRecSrcStream( tMt,MODE_AUDIO,TRUE );
		
		AdjustSwitchToAllMcWatchingSrcMt( tMt,FALSE,MODE_AUDIO,FALSE );

		AdjustSwitchToMonitorWatchingSrc( tMt,MODE_AUDIO,FALSE,TRUE );
		
		CSendSpy cSendSpy;
		if( !m_tCascadeMMCU.IsNull() && m_cLocalSpyMana.GetSpyChannlInfo( tMt,cSendSpy ) )
		{
			if( MODE_BOTH == cSendSpy.GetSpyMode() || MODE_AUDIO == cSendSpy.GetSpyMode() )
			{				
				u16 wErrorCode = 0;
				StartSwitchToMcuByMultiSpy( tMt, 0, m_tCascadeMMCU.GetMtId(), cSendSpy.GetSimCapset(), wErrorCode,
											MODE_AUDIO, SWITCH_MODE_SELECT, FALSE );
			}
		}
		
	}

	return;
}

/*=============================================================================
    �� �� ���� IsSelModeAndCapMatched
    ��    �ܣ� �ж�ѡ��ģʽ�Ͷ�Ӧ�������Ƿ�ƥ��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
               u8 &bySwitchMode       : IN/OUT��ѡ��ģʽ
               TSimCapSet tSimCapSrc  : Դ��������
               TSimCapSet tSimCapDst  : Ŀ�Ķ�������
               BOOL32     bAccord2Adp : �����Ƶ���Ƿ���Ҫ��������ѡ�� FIXME:��δ֧�ֱ������Ŀ�����ѡ��
    �� �� ֵ�� BOOL32     ѡ���Ƿ���Լ���(ģʽ��ȫƥ���������ƥ��)
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/04/14  4.0			�ű���                ����
    2008/07/10  4.5         �ܹ��                �޸�
    2009/05/22  4.5         �ű���                ��Ƶѡ����ʽ����֧�֡���ֱ�������֧��
=============================================================================*/
BOOL32 CMcuVcInst::IsSelModeAndCapMatched( u8               &bySwitchMode, 
                                           const TSimCapSet &tSimCapSrc, 
                                           const TSimCapSet &tSimCapDst,
                                           BOOL32           &bAccord2Adp)
{
    bAccord2Adp = FALSE;

    // xsl [11/10/2006] �������ļ�����˫��ʽ����ѡ�����򲻽����ж�
    if (g_cMcuVcApp.IsSelInDoubleMediaConf() && !IsHDConf(m_tConf))
    {
        return TRUE;
    }

// #ifdef _8KE_		
// 	//8KE��ר�ŵ���Ƶѡ��ͨ��,���ڸ߱��������Զ�������Ƶѡ������
// 	if(tSimCapSrc.GetVideoMediaType() != tSimCapDst.GetVideoMediaType() ||
// 		IsResG(tSimCapSrc.GetVideoResolution(), tSimCapDst.GetVideoResolution()))
// 	{
// 		ConfLog(FALSE, "[IsSelModeAndCapMatched] 8KE permit Dst<VidType:%d, Res:%d> sel to see Src<VidType:%d, Res:%d> across Bas!\n",
// 			tSimCapDst.GetVideoMediaType(), tSimCapDst.GetVideoResolution(),
// 			tSimCapSrc.GetVideoMediaType(), tSimCapSrc.GetVideoResolution());			
// 		bAccord2Adp = TRUE;
// 	}		
// 	return TRUE;
// #else

    //ѡ������Ƶ
    if ( MODE_BOTH == bySwitchMode )
    {
        //��Ƶ����Ƶ�����ƥ��
        if ( tSimCapSrc.GetAudioMediaType() != tSimCapDst.GetAudioMediaType() )
        {
            bySwitchMode = MODE_VIDEO;
        }
        if ( tSimCapSrc.GetVideoMediaType() != tSimCapDst.GetVideoMediaType() ) 
        {
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
            if (!IsHDConf(m_tConf) || !g_cMcuVcApp.IsSelAccord2Adp())
            {
                bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_AUDIO : MODE_NONE;

                ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap audio<%d> video<%d> vs DstCap audio<%d> video<%d>\n",
                    tSimCapSrc.GetAudioMediaType(), tSimCapSrc.GetVideoMediaType(),
                    tSimCapDst.GetAudioMediaType(), tSimCapDst.GetVideoMediaType() );
            }
            else
#endif // ifndef _8KE_
            {
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[IsSelModeAndCapMatched] capbility match loss, need adp: \n\t\tSrcCap audio<%d> video<%d> vs DstCap audio<%d> video<%d>\n",
                    tSimCapSrc.GetAudioMediaType(), tSimCapSrc.GetVideoMediaType(),
                    tSimCapDst.GetAudioMediaType(), tSimCapDst.GetVideoMediaType() );
                
                bAccord2Adp = TRUE;
            }
        }
        if ( MODE_NONE == bySwitchMode )
        {           
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap audio<%d> video<%d> vs DstCap audio<%d> video<%d>\n",
				     tSimCapSrc.GetAudioMediaType(), tSimCapSrc.GetVideoMediaType(),
                     tSimCapDst.GetAudioMediaType(), tSimCapDst.GetVideoMediaType() );
            return FALSE;
        }
    }
    //ѡ����Ƶ
    else if ( MODE_VIDEO == bySwitchMode )
    {
        //��Ƶ�����ƥ��
        if ( tSimCapSrc.GetVideoMediaType() != tSimCapDst.GetVideoMediaType() )
        {
#if !defined(_8KE_) && !defined(_8KH_)	&& !defined(_8KI_)
			if (!IsHDConf(m_tConf) || !g_cMcuVcApp.IsSelAccord2Adp())
            {
                bySwitchMode = MODE_NONE;
				ConfPrint( LOG_LVL_WARNING, MID_MCU_MT, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap video<%d> vs DstCap video<%d>\n",
					tSimCapSrc.GetVideoMediaType(), tSimCapDst.GetVideoMediaType() );
                return FALSE;
            }
            else
#endif //ifndef _8KE_
            {
				ConfPrint( LOG_LVL_WARNING, MID_MCU_MT, "[IsSelModeAndCapMatched] capbility match loss, need adp: \n\t\tSrcCap video<%d> vs DstCap video<%d>\n",
                    tSimCapSrc.GetVideoMediaType(), tSimCapDst.GetVideoMediaType() );
                bAccord2Adp = TRUE;
            }          
        }
    }
    //ѡ����Ƶ
    else if ( MODE_AUDIO == bySwitchMode )
    {
        //��Ƶ�����ƥ��
        if ( tSimCapSrc.GetAudioMediaType() != tSimCapDst.GetAudioMediaType() )
        {
            ConfPrint( LOG_LVL_WARNING, MID_MCU_MT, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap audio<%d> vs DstCap audio<%d>\n",
                     tSimCapSrc.GetAudioMediaType(), tSimCapDst.GetAudioMediaType() );
            bySwitchMode = MODE_NONE;
            return FALSE;
        }
    }

    // ��ѡ����Ƶ, �ֱ��ʲ�һ�£�NACK
    // zgc, 2008-07-10, �Ƶ� IsSelModeAndCapMatched ��
    if( ( MODE_VIDEO == bySwitchMode || MODE_BOTH  == bySwitchMode) &&
        tSimCapSrc.GetVideoResolution() != tSimCapDst.GetVideoResolution() ) 
    {
        // xsl [10/10/2006] �����ն˷ֱ���Ϊ����Ӧʱ����ѡ��Դ��Ŀ���ն˷ֱ��ʲ�һ��
        if( VIDEO_FORMAT_AUTO != tSimCapDst.GetVideoResolution() &&
            ( MEDIA_TYPE_H264 != tSimCapDst.GetVideoMediaType() || 
              IsResGE(tSimCapSrc.GetVideoResolution(), tSimCapDst.GetVideoResolution()))) 
        {
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)	
            if (!IsHDConf(m_tConf) || !g_cMcuVcApp.IsSelAccord2Adp())
            {
                bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_AUDIO : MODE_NONE;
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap VidRes<%d> vs DstCap VidRes<%d>\n",
                    tSimCapSrc.GetVideoResolution(), tSimCapDst.GetVideoResolution() );
            }
            else
#endif //ifndef _8KE_
            {
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT,  "[IsSelModeAndCapMatched] capbility match loss, need adp: \n\t\tSrcCap VidRes<%d> vs DstCap VidRes<%d>\n",
                    tSimCapSrc.GetVideoResolution(), tSimCapDst.GetVideoResolution() );
                bAccord2Adp = TRUE;
            }
        }
        if ( MODE_NONE == bySwitchMode )
        {
            return FALSE;
        }
    }
    
    return TRUE;
// #endif //ifdef _8KE_
}

/*=============================================================================
    �� �� ���� AdjustKedaMcuAndTaideMuteInfo
    ��    �ܣ� ������KEDA�նˣ�KEDAMCU��Tandberg������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMt * ptMt       : ��������ն˽ṹ
               BOOL32 bDecodeAud: 1�����룬0���ɼ�
               BOOL32 bOpenFlag : 1������, 0���Ǿ���
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/04/15  4.0			�ű���                ����
=============================================================================*/
void CMcuVcInst::AdjustKedaMcuAndTaideMuteInfo( TMt * ptMt, BOOL32 bDecodeAud, BOOL32 bOpenFlag )
{
    if ( NULL == ptMt )
    {
        ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[AdjustKedaMcuAndTaideMuteInfo] ptMt NULL ! \n");
        return;
    }
    
    //�ϱ�MCS
    TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus(ptMt->GetMtId(), &tMtStatus);
    if( TRUE == bDecodeAud ) 
    {
        tMtStatus.SetDecoderMute( bOpenFlag ); //�����Ƿ���	        
    }
    else
    {
        tMtStatus.SetCaptureMute( bOpenFlag );	//�ɼ��Ƿ���(�Ƿ�����)
    }
    m_ptMtTable->SetMtStatus(ptMt->GetMtId(), &tMtStatus);

    MtStatusChange( ptMt, TRUE );
    
    //����Taidberg��KDCMCU�����������������
    AdjustKedaMcuAndTaideAudioSwitch( ptMt->GetMtId(), bDecodeAud, bOpenFlag );
    
    return;
}

/*=============================================================================
�� �� ���� ProcPolycomSecVidChnnl
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� void
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/03/20  4.0			�ű���                ����
=============================================================================*/
void CMcuVcInst::ProcPolycomSecVidChnnl(CMessage * const pcMsg)
{
	u8 byPolycomMtId = (u8)(*(u32*)pcMsg->content);
	if ( byPolycomMtId < 1 || byPolycomMtId > MAXNUM_CONF_MT )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[PolycomSecChnnl] byPolycomMtId.%d ignore it\n", byPolycomMtId );
		return;
	}
	
	TCapSupport tPolyMcuCap;
	BOOL32 bRet = m_ptMtTable->GetMtCapSupport(byPolycomMtId, &tPolyMcuCap);
	if ( !bRet )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[PolycomSecChnnl] get PolyMt.%d's cap failed\n", byPolycomMtId );
		return;
	}
	TLogicalChannel tLogicChan;
	TMt tPolyMt = m_ptMtTable->GetMt(byPolycomMtId);
	McuMtOpenDoubleStreamChnnl(tPolyMt, tLogicChan/*, tPolyMcuCap*/);
	
	return;
}

/*==============================================================================
������    :  SwitchNewVmpToSingleMt
����      :  ����VMP�������������MT
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-3-31	4.6				Ѧ��							create
==============================================================================*/
void CMcuVcInst::SwitchNewVmpToSingleMt(TMt tMt)
{
	if (tMt.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[SwitchNewVmpToSingleMt] illegal tMt!\n");
		return;
	}

	if (CONF_VMPMODE_NONE == g_cMcuVcApp.GetVMPMode(m_tVmpEqp))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP,"[SwitchNewVmpToSingleMt] No VMP is used!\n");
		return;
	}
	

	u8 bySrcChnnl = GetVmpOutChnnlByDstMtId( tMt.GetMtId(), m_tVmpEqp.GetEqpId());


	if( bySrcChnnl == (u8)~0 )
	{
		ConfPrint(LOG_LVL_WARNING,  MID_MCU_VMP, "NewVmpToMt Mt.%u cannot rcv vmp brd for unmatched fmt or res!\n");
	}
	else
	{
		g_cMpManager.SetSwitchBridge(m_tVmpEqp, bySrcChnnl, MODE_VIDEO);	//����
		//zjl 20110510 StartSwitchToAll �滻 StartSwitchToSubMt
		//StartSwitchToSubMt(m_tVmpEqp, bySrcChnnl, tMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode ��Ĭ��ֵ����SWITCH_MODE_SELECT
		TSwitchGrp tSwitchGrp;
		tSwitchGrp.SetSrcChnl(bySrcChnnl);
		tSwitchGrp.SetDstMtNum(1);
		tSwitchGrp.SetDstMt(&tMt);
		StartSwitchToAll(m_tVmpEqp, 1, &tSwitchGrp, MODE_VIDEO, SWITCH_MODE_SELECT);
	}

	//vmp�㲥ʱ�����ӵ����ն˵�PRS����
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (tConfAttrb.IsResendLosePack())
	{
		if ( 0xFF != bySrcChnnl ) 
		{
			u8 byPrsId    = 0;
			u8 byPrsChnId = 0;
			if (FindPrsChnForSrc(m_tVmpEqp, bySrcChnnl, MODE_VIDEO, byPrsId, byPrsChnId))
			{
				AddRemovePrsMember(tMt.GetMtId(), byPrsId, byPrsChnId, MODE_VIDEO, TRUE);
			}
		}
	}
	
/*	u8 byOutIdx = 0;
	u8 abyVmpFormat[3] = {	VIDEO_FORMAT_HD720,  //tempariry value.  720��mtҲ����1080
		0, 
		VIDEO_FORMAT_CIF 
	};
	if(m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_4CIF)
	{
		abyVmpFormat[0] = VIDEO_FORMAT_4CIF;
		abyVmpFormat[1] = 0;
	}
	
	for(byOutIdx = 0; byOutIdx < 3; byOutIdx++)
	{
		byVmpRes = abyVmpFormat[byOutIdx];
		if(byVmpRes <= byMtRes) 
		{
			bySrcChnnl = byOutIdx;
			break;
		}
	}
	if( byOutIdx >= 3 
		|| byMtMediaType != MEDIA_TYPE_H264 //Ŀǰ��֧�ַ�h264
		)
	{
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp] Mt.%u cannot receive vmp broardcast \
		because unmatched format or resolution!\n");
	}
	else
	{
		g_cMpManager.SetSwitchBridge(m_tVidBrdSrc, bySrcChnnl, MODE_VIDEO);	//����
		StartSwitchToSubMt(m_tVidBrdSrc, bySrcChnnl, tMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode ��Ĭ��ֵ����SWITCH_MODE_SELECT
	}
*/

}

/*====================================================================
    ������      ��PackAndSendMtList2Mcs
    ����        �������Ϣ����MCU_MCS_GETMTLIST_NOTIF������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMtExtU *atMtExtU,���ն��б�
				��u8 &byMtNum, �ն�����
				��u16 &wMcuIdx������mcu
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2013/7/10   4.7.2       �����          ����
====================================================================*/
void CMcuVcInst::PackAndSendMtList2Mcs( u8 &byMtNum, TMtExtU *ptMtExtU, const u16 &wMcuIdx)
{
	if ( !ptMtExtU )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[PackAndSendMtList2Mcs]invalid ptMtExtU\n");
		return;
	}
	CServMsg cMsg2Mcs;
	cMsg2Mcs.SetConfId(m_tConf.GetConfId());
	cMsg2Mcs.SetConfIdx(m_byConfIdx);
	cMsg2Mcs.SetEventId(MCU_MCS_GETMTLIST_NOTIF);

	cMsg2Mcs.SetMsgBody(&byMtNum, sizeof(u8));
	u16 wSndMcuIdx = htons(wMcuIdx);
	cMsg2Mcs.CatMsgBody((u8 *)&wSndMcuIdx, sizeof(u16));
	u8 byHasHeadInfo = 1; //�Ƿ���ͷ����Ϣ
	cMsg2Mcs.CatMsgBody(&byHasHeadInfo, sizeof(u8));

	u32 dwHeadLen = ptMtExtU[0].GetTotalMemLen()+sizeof(u8); //��Ա����+��Ա����
	LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp] dwHeadLen:%d\n", dwHeadLen);
	dwHeadLen = htonl(dwHeadLen);
	cMsg2Mcs.CatMsgBody((u8 *)&dwHeadLen, sizeof(u32));

	u8 byMemNum = ptMtExtU[0].GetMemNum();
	cMsg2Mcs.CatMsgBody(&byMemNum, sizeof(u8));
	u16 wMemLen = 0;
	LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp] struct member:\n", dwHeadLen);
	for ( u8 byMemId = 1; byMemId <= byMemNum;byMemId++)
	{
		wMemLen = ptMtExtU[0].GetMemLen(byMemId);
		LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "%d ", wMemLen);
		wMemLen = htons(wMemLen);
		cMsg2Mcs.CatMsgBody((u8*)&wMemLen, sizeof(u16));
	}

	for ( u8 byMtId = 0;byMtId < byMtNum;byMtId++)
	{
		cMsg2Mcs.CatMsgBody((u8*)&ptMtExtU[byMtId], sizeof(TMtExtU));
	}

	SendMsgToAllMcs(MCU_MCS_GETMTLIST_NOTIF, cMsg2Mcs);

	return;
}

/*====================================================================
    ������      ��ProcMtAdpMcuMtListRsp
    ����        �������Ϣ����MCU_MCS_GETMTLIST_NOTIF������ AND TO MMCU
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg,����Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2013/7/1    4.7.2       �����          ����
====================================================================*/
void CMcuVcInst::ProcMtAdpMcuMtListRsp(const CMessage * pcMsg )
{
	// ���״̬У�� [pengguofeng 7/23/2013]
	STATECHECK 

	if ( !pcMsg )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]pcMsg == NULL\n");
		return;
	}
	
	CServMsg cServMsg(pcMsg->content,pcMsg->length);

	if ( cServMsg.GetEventId() != MTADP_MCU_GETMTLIST_NOTIF )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]invalid Event(NOT MTADP_MCU_GETMTLIST_NOTIF)\n");
		return; //��ACK��NOTIF����Ϣ����Ҫ����
	}
	
	u8 abyMcuIdx[MAX_CASCADEDEPTH-1];
	abyMcuIdx[0] = cServMsg.GetSrcMtId();
	
	// ��������SMcu [pengguofeng 7/1/2013]
	u8 *pMsg = cServMsg.GetMsgBody();
	u32 dwMsgRLen = 0;

	u8 bySend2MMcu = 0;
	while ( cServMsg.GetMsgBodyLen() > dwMsgRLen )
	{
		bySend2MMcu = *(u8 *)(pMsg + dwMsgRLen);
		dwMsgRLen += sizeof(u8);

		abyMcuIdx[1] = *(u8 *)(pMsg + dwMsgRLen);
		dwMsgRLen += sizeof(u8);
		
		u16 wMcuIdx = INVALID_MCUIDX;
		if( !m_tConfAllMcuInfo.AddMcu( abyMcuIdx,2,&wMcuIdx ) )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF,  "[ProcMtAdpMcuMtListRsp] Fail to Add Mcu<%d %d> Info.McuId(%d)\n",
				abyMcuIdx[0], abyMcuIdx[1], wMcuIdx);
			return; //��һ������ͱ���return
		}
		else
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF,  "[ProcMtAdpMcuMtListRsp] Add Mcu<%d %d> Info.McuId(%d) successd\n",
				abyMcuIdx[0], abyMcuIdx[1], wMcuIdx);
		}
	
		u8 byMtNum = *(u8 *)(pMsg + dwMsgRLen);

		dwMsgRLen += sizeof(u8);
		
		u8 byMtIdx = 0;
		u8 byMtId = 0;
		TMtExtU atMtExtU[MAXNUM_CONF_MT];
		memset(atMtExtU, 0, sizeof(atMtExtU));

		if ( !m_ptConfOtherMcTable ) // ��ֹ���ʱ������ [pengguofeng 7/16/2013]
		{
			LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]m_ptConfOtherMcTable is NULL state.%d\n", CurState());
			return;
		}
		
		TConfMcInfo* ptMcMtInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
		if ( !ptMcMtInfo )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF,  "[ProcMtAdpMcuMtListRsp]2 Fail to Add Mcu Info.McuId(%d)\n", wMcuIdx);
			return;
		}
		else
		{
			LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]2 add wMcuIdx.%d succeed\n", wMcuIdx);
		}
		
		LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]recv %d mt from mtadp\n", byMtNum);

		s8 achMtName[VALIDLEN_ALIAS_UTF8];
		u16 wMtNameLen = 0;
/*		CServMsg cMsg2Mcs;
		cMsg2Mcs.SetConfId(m_tConf.GetConfId());
		cMsg2Mcs.SetConfIdx(m_byConfIdx);
		cMsg2Mcs.SetEventId(MCU_MCS_GETMTLIST_NOTIF);
		
		cMsg2Mcs.SetMsgBody(&byMtNum, sizeof(u8));
		u16 wSndMcuIdx = htons(wMcuIdx);
		cMsg2Mcs.CatMsgBody((u8 *)&wSndMcuIdx, sizeof(u16));
		u8 byHasHeadInfo = 1;
		cMsg2Mcs.CatMsgBody(&byHasHeadInfo, sizeof(u8));
		u32 dwHeadLen = atMtExtU[0].GetTotalMemLen()+sizeof(u8);
 		LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp] dwHeadLen:%d\n", dwHeadLen);
		dwHeadLen = htonl(dwHeadLen);
		cMsg2Mcs.CatMsgBody((u8 *)&dwHeadLen, sizeof(u32));
		
		u8 byMemNum = atMtExtU[0].GetMemNum();
		cMsg2Mcs.CatMsgBody(&byMemNum, sizeof(u8));
		u16 wMemLen = 0;
 		LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp] struct member:\n", dwHeadLen);
		for ( u8 byMemId = 1; byMemId <= byMemNum;byMemId++)
		{
			wMemLen = atMtExtU[0].GetMemLen(byMemId);
 			LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "%d ", wMemLen);
			wMemLen = htons(wMemLen);
			cMsg2Mcs.CatMsgBody((u8*)&wMemLen, sizeof(u16));
		}
*/
		TMtExt *ptMtExt = NULL;
		TMt tMt;
		tMt.SetNull();
		tMt.SetMcuId(wMcuIdx);

		CServMsg cMsg2Mcu; // ����Ϣ��MMcu [pengguofeng 7/10/2013]
		cMsg2Mcu.SetConfId(m_tConf.GetConfId());
		cMsg2Mcu.SetConfIdx(m_byConfIdx);
		cMsg2Mcu.SetEventId(MCU_MCU_MTLIST_ACK);
		u8 byLastPack = 1;
		cMsg2Mcu.SetMsgBody(&byLastPack, sizeof(u8));

		TMcuMcuMtInfo tMtInfo[MAXNUM_CONF_MT];
		memset(&tMtInfo, 0, sizeof(tMtInfo)); //�������ϼ���

		while (byMtIdx < byMtNum)
		{
			byMtId = *(u8 *)(pMsg + dwMsgRLen);
			if (/* byMtId == 0 ||*/ byMtId > MAXNUM_CONF_MT )
			{
				LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]invalid MtId.%d\n", byMtId);
				return;
			}

			dwMsgRLen += sizeof(u8);
			wMtNameLen = *(u16 *)(pMsg + dwMsgRLen);
			dwMsgRLen += sizeof(u16);
			wMtNameLen = ntohs(wMtNameLen);
			u16 wOrgNameLen = wMtNameLen;

			if ( wMtNameLen > VALIDLEN_ALIAS_UTF8-1)
			{
				wMtNameLen = VALIDLEN_ALIAS_UTF8-1;
			}
			memcpy(achMtName, pMsg+ dwMsgRLen, wMtNameLen);
			achMtName[wMtNameLen] = 0;
			dwMsgRLen += wOrgNameLen; //��Ϣ���ƶ�����ֹ�����޸��˳���

			tMt.SetMtId(byMtId);
			ptMtExt = ptMcMtInfo->GetMtExt(tMt);
			if ( !ptMtExt )
			{
				LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]Mt<%d.%d> not found in other mc table\n", wMcuIdx, byMtId);
				return;
			}

 			LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp] copy Mt.%d Name:(%s )Len:(%d)\n", byMtId, achMtName, wMtNameLen);
			// ע��mt table���±��byMtId����һһ��Ӧ�� [pengguofeng 7/3/2013]
			memcpy(&atMtExtU[byMtIdx/*byMtId-1*/], ptMtExt/*&(ptMcMtInfo->m_atMtExt[byMtId-1])*/, sizeof(TMt) + sizeof(u8)*2+sizeof(u16) + sizeof(u32)*2 );
			atMtExtU[byMtIdx].SetProtocolType(ptMtExt->GetProtocolType());
			atMtExtU[byMtIdx].SetAlias(achMtName);
// 			cMsg2Mcs.CatMsgBody((u8*)&atMtExtU[byMtIdx], sizeof(TMtExtU));

 			SetMtInfoByMtExtU(wMcuIdx, atMtExtU[byMtIdx], tMtInfo[byMtIdx]); //��mcu�����
			byMtIdx++;
		}
		
//		SendMsgToAllMcs(MCU_MCS_GETMTLIST_NOTIF, cMsg2Mcs);
		PackAndSendMtList2Mcs(byMtNum, atMtExtU, wMcuIdx);

		// �ϱ�MMCU [pengguofeng 7/4/2013]
		cMsg2Mcu.CatMsgBody((u8 *)tMtInfo, byMtNum*sizeof(TMcuMcuMtInfo));

		LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]bySend2MMcu:%d from mcu.%d MMcu.%d\n",
			bySend2MMcu, abyMcuIdx[0], m_tCascadeMMCU.GetMtId());
		if ( bySend2MMcu != 0 && !m_tCascadeMMCU.IsNull() 
			&& abyMcuIdx[0] != m_tCascadeMMCU.GetMtId() ) // �ϼ��Լ������ûع�ȥ [pengguofeng 7/10/2013]
		{
			LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]Pack mcu.%d and send to MMCU.%d\n", wMcuIdx, m_tCascadeMMCU.GetMtId());
			SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_MTLIST_ACK, cMsg2Mcu);

			SendAllSMcuMtStatusToMMcu(); // ����MtStatus [pengguofeng 7/26/2013]
			SendSMcuUpLoadMtToMMcu();
		}

		// �ϱ�SMCU [pengguofeng 7/4/2013]
/*		if ( g_cMcuVcApp.IsShowMMcuMtList() )
		{
			for ( u8 bySMcuMtId = 1; bySMcuMtId <= MAXNUM_CONF_MT; bySMcuMtId++)
			{
				if ( m_ptMtTable->GetMtType(bySMcuMtId) == MT_TYPE_SMCU )
				{
					LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]Pack mcu.%d and send to MMCU.%d\n", wMcuIdx, bySMcuMtId);
					SendMsgToMt(bySMcuMtId, MCU_MCU_MTLIST_ACK, cMsg2Mcu);
				}
			}
		}
*/	}
}

/*====================================================================
    ������      ��ProcMtAdpMcuSmcuMtAliasRsp
    ����        ������¼�mcu�ն˱���֪ͨ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg,����Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2013/8/8    4.7.2      yanghuaizhi      ����
====================================================================*/
void CMcuVcInst::ProcMtAdpMcuSmcuMtAliasRsp(const CMessage * pcMsg )
{
	// ���״̬У��
	STATECHECK 

	if ( !pcMsg )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuSmcuMtAliasRsp]pcMsg == NULL\n");
		return;
	}
	
	CServMsg cServMsg(pcMsg->content,pcMsg->length);
	if ( cServMsg.GetEventId() != MTADP_MCU_GETSMCUMTALIAS_NOTIF )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuSmcuMtAliasRsp]invalid Event(NOT MTADP_MCU_GETSMCUMTALIAS_NOTIF)\n");
		return; //��ACK��NOTIF����Ϣ����Ҫ����
	}
	
	u8 abyMcuIdx[MAX_CASCADEDEPTH-1];
	abyMcuIdx[0] = cServMsg.GetSrcMtId();
	
	// ������Ϣ��byEqpid + byMcuId + byMtNum + byMtNum*[ byMtId + byChlNum + byChlNum*[byChlIdx] + byNameLen + pName]
	u8 *pbyBuf = (u8 *)cServMsg.GetMsgBody();
	u8 byEqpid = *pbyBuf;// byEqpid
	pbyBuf++;
	abyMcuIdx[1] = *pbyBuf;
	pbyBuf++;
	u8 byMtNum = *pbyBuf;
	pbyBuf++;
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF,  "[ProcMtAdpMcuSmcuMtAliasRsp] Eqpid.%d MtNum.%d\n",
				byEqpid, byMtNum);

	u16 wMcuIdx = INVALID_MCUIDX;
	m_tConfAllMcuInfo.GetIdxByMcuId(abyMcuIdx,2,&wMcuIdx);
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF,  "[ProcMtAdpMcuSmcuMtAliasRsp] Mcu<%d %d> Info.McuId(%d)\n",
				abyMcuIdx[0], abyMcuIdx[1], wMcuIdx);

	TMt tMt;
	tMt.SetNull();
	tMt.SetMcuId(wMcuIdx);
	// ��eqpid�ǻ���ϳɵĻ����߻���ϳ��߼�
	if (IsValidVmpId(byEqpid))
	{
		TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byEqpid );
		TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
		TVMPMember tVmpMember;
		// ���ݿ��ǣ���MCU_VMP_CHANGEMEMALIAS_CMD��Ϣʱ����һ���ն������ChnIndex�������ն�����MsgBody��
		BOOL32 bFristMt = TRUE;
		
		u8 byMtId;
		u8 byChlNum;
		s8 achAlias[MAXLEN_ALIAS] = {0};
		u8 byNameLen = 0;
		u8 byMtIdx;
		u8 byChlIdx;
		u8 abyVmpChl[MAXNUM_VMP_MEMBER];
		CServMsg cMsg;
		cMsg.SetMsgBody();
		for (byMtIdx=0; byMtIdx<byMtNum;byMtIdx++)
		{
			memset(achAlias, 0, sizeof(achAlias));
			memset(abyVmpChl, 0, sizeof(abyVmpChl));
			//���Mtid
			byMtId = *pbyBuf;
			pbyBuf++;
			if (byMtId > MAXNUM_CONF_MT)
			{
				LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuSmcuMtAliasRsp]invalid MtId.%d\n", byMtId);
				return;
			}
			else
			{
				tMt.SetMtId(byMtId);
			}
			byChlNum = *pbyBuf;
			pbyBuf++;
			if (byChlNum > MAXNUM_VMP_MEMBER)
			{
				LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuSmcuMtAliasRsp]invalid VmpChlNum.%d\n", byChlNum);
				return;
			}
			// ��ȡvmpͨ������
			memcpy(abyVmpChl, pbyBuf, byChlNum);
			pbyBuf += byChlNum;
			// ��ȡ�ն˱�����Ϣ
			byNameLen = *pbyBuf;
			pbyBuf++;
			if (byNameLen > VALIDLEN_ALIAS_UTF8-1)
			{
				byNameLen = VALIDLEN_ALIAS_UTF8-1;
			}
			memcpy(achAlias, pbyBuf, byNameLen);
			pbyBuf += byNameLen;
			for (byChlIdx=0; byChlIdx<byChlNum;byChlIdx++)
			{
				if (abyVmpChl[byChlIdx] >= MAXNUM_VMP_MEMBER)
				{
					LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuSmcuMtAliasRsp]invalid VmpChl.%d\n", abyVmpChl[byChlIdx]);
					return;
				}
				tVmpMember = *tConfVmpParam.GetVmpMember(abyVmpChl[byChlIdx]);
				// �ն����Ӧvmpͨ��һ�£��Ÿ����跢��ˢ�±�����Ϣ
				if (tMt.GetMcuId() == tVmpMember.GetMcuId() &&
					tMt.GetMtId() == tVmpMember.GetMtId())
				{
					if (bFristMt)
					{
						bFristMt = FALSE;
						cMsg.SetChnIndex(abyVmpChl[byChlIdx]);
					}
					cMsg.CatMsgBody( (u8*)achAlias, sizeof(achAlias) );
					cMsg.CatMsgBody( (u8*)&abyVmpChl[byChlIdx], sizeof(u8));
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMtAdpMcuSmcuMtAliasRsp]vmp member.%d info: mt(%d,%d), alias is %s!\n", 
						abyVmpChl[byChlIdx], tMt.GetMcuId(), tMt.GetMtId(), achAlias);
				}
			}
		}

		// ��ָ��ͨ��������Ϣ
		if (cMsg.GetMsgBodyLen() > 0)
		{
			// ˢ�±�����Ϣ���ݣ�[����128 + ChlIdx] * ͨ������
			SendMsgToEqp(tVmpEqp.GetEqpId(), MCU_VMP_CHANGEMEMALIAS_CMD, cMsg);
		}
	}
	else
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuSmcuMtAliasRsp]Eqpid.%d is not vmp.\n", byEqpid);
		return;
	}


}

/********************************************************************
	created:	2013/07/04
	created:	4:7:2013   9:36
	name: 		SetMtInfoByMtExtU
	author:		peng guofeng
	
	purpose:	����wMcuIdx��tMtExtU������tMtInfo����ҪΪ����Է���MtList_Ack
*********************************************************************/
/*====================================================================
    ������      ��SetMtInfoByMtExtU
    ����        ������wMcuIdx��tMtExtU������tMtInfo����ҪΪ����Է���MtList_Ack
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const u16 &wMcuIdx,��mcu
				��const TMtExtU &tMtExtU�������MtExt��Ϣ
				��TMcuMcuMtInfo &tMtInfo���ϱ��Ľṹ��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2013/7/1    4.7.2       �����          ����
====================================================================*/
BOOL32 CMcuVcInst::SetMtInfoByMtExtU(const u16 &wMcuIdx, 
								   const TMtExtU &tMtExtU, TMcuMcuMtInfo &tMtInfo)
{
	if ( wMcuIdx == INVALID_MCUIDX )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[SetMtInfoByMtExtU]wMcuIdx == INVALID\n");
		return FALSE;
	}

	if ( !m_ptConfOtherMcTable ) // ��ֹ���ʱ������ [pengguofeng 7/16/2013]
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[SetMtInfoByMtExtU]m_ptConfOtherMcTable is NULL state.%d\n", CurState());
		return FALSE;
	}
	
	//0. get mcu info
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
	
	if ( !ptMcInfo )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[PackMtListAckMsg]Get mcu.%d mcinfo Failed\n", wMcuIdx);
		return false;
	}

	//1. get mcuId
	u8 abyMcuId[MAX_CASCADEDEPTH-1];
	memset( &abyMcuId[0],0,sizeof(abyMcuId) );
	m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] );

	//2. copy
	strncpy((s8 *)(tMtInfo.m_szMtName), tMtExtU.GetAlias(), sizeof(tMtInfo.m_szMtName) - 1);

	TMt tMt;
	memcpy(&tMt, &tMtExtU, sizeof(TMt));
	tMt.SetMcuIdx(wMcuIdx);
// 	tMt.SetMcuIdx(abyMcuId[0]); //�˴�����Idx��
	tMtInfo.m_tMt = tMt;
	tMtInfo.m_tMt.SetMcuIdx(abyMcuId[0]); // ����ȥ��һ���õ���ȷ��MtId [pengguofeng 7/26/2013]

	tMtInfo.m_dwMtIp = ntohl(tMtExtU.GetIPAddr());
	tMtInfo.m_byMtType = tMtExtU.GetMtType();
	tMtInfo.m_byManuId = tMtExtU.GetManuId();
	tMtInfo.m_byProtocolType = tMtExtU.GetProtocolType();
	strncpy((s8 *)tMtInfo.m_szMtDesc, "notsupport", sizeof(tMtInfo.m_szMtDesc)-1);

	// online
	TConfMtInfo *ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr(wMcuIdx);
	if ( !ptConfMtInfo )
	{
		tMtInfo.m_byIsConnected = 0;
	}
	else
	{
		tMtInfo.m_byIsConnected = ptConfMtInfo->MtJoinedConf( tMtExtU.GetMtId() );
	}

	//3 logic channel
	// Bug:�˴�Ӧ��ȡ�¼�MCU��״̬ [pengguofeng 7/26/2013]
	TLogicalChannel tChannel;
	TMcMtStatus *ptSubMtSts = ptMcInfo->GetMtStatus(tMt);	
	if ( !ptSubMtSts )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[SetMtInfoByMtExtU]get Mt<%d %d> status failed\n", tMt.GetMcuId(), tMt.GetMtId());
		return FALSE;
	}
	tMtInfo.m_byIsAudioMuteIn = ptSubMtSts->IsSendAudio() ? 0 : 1;
	tMtInfo.m_byIsAudioMuteOut = ptSubMtSts->IsReceiveAudio() ? 0: 1;
	tMtInfo.m_byIsVideoMuteIn = ptSubMtSts->IsSendVideo() ? 0:1;
	tMtInfo.m_byIsVideoMuteOut = ptSubMtSts->IsReceiveVideo()?0:1;
	
	tMtInfo.m_byVideoIn = ptSubMtSts->GetVideoIn();
	tMtInfo.m_byVideoOut = ptSubMtSts->GetVideoOut();
	tMtInfo.m_byAudioIn = ptSubMtSts->GetAudioIn();
	tMtInfo.m_byAudioOut = ptSubMtSts->GetAudioOut();
	tMtInfo.m_byVideo2In = ptSubMtSts->IsSndVideo2() ? 1: 0;
	tMtInfo.m_byVideo2Out = ptSubMtSts->IsRcvVideo2() ? 1:0;
	tMtInfo.m_byIsFECCEnable = ptSubMtSts->IsEnableFECC()? 1:0;
	TMtVideoInfo tMtVidInfo = ptSubMtSts->GetMtVideoInfo();
	memcpy(&tMtInfo.m_tPartVideoInfo, &tMtVidInfo, sizeof(TMtVideoInfo));
	tMtInfo.m_byIsDataMeeting = 0; 
	
	//ע������Ŀǰû���õ���������
	if ( tMt.GetMtId() == 0 ) // �¼�MCU [pengguofeng 7/26/2013]
	{
		tMtInfo.m_byIsDataMeeting = m_ptMtTable->GetMtLogicChnnl( abyMcuId[0], LOGCHL_T120DATA, &tChannel, TRUE ) ? 1:0;
		if ( tMtInfo.m_byIsDataMeeting == 0 )
		{
			tMtInfo.m_byIsDataMeeting =  m_ptMtTable->GetMtLogicChnnl( abyMcuId[0], LOGCHL_T120DATA, &tChannel, FALSE ) ? 1:0;
		}
	}
	
	// ��ӡ [pengguofeng 7/5/2013]
	LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[SetMtInfoByMtExtU]set mtinfo to mt<%d %d> online:%d\n",tMt.GetMcuId(), tMt.GetMtId(), tMtInfo.m_byIsConnected);
	
	return TRUE;
}

/*=============================================================================
�� �� ���� ProcMtMcuReleaseMtReq
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� void
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/07/14  4.0			���㻪                ����
=============================================================================*/
void CMcuVcInst::ProcMtMcuReleaseMtReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	TMt tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	s8  achConfName[MAXLEN_CONFNAME + 1] = {0};

	const u16 wMsgBodyLen = cServMsg.GetMsgBodyLen();
	memcpy(achConfName, cServMsg.GetMsgBody(), min(MAXLEN_CONFNAME, wMsgBodyLen));

	cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
	cServMsg.CatMsgBody((u8*)achConfName, strlen(achConfName));
	cServMsg.SetEventId(MCU_VCS_RELEASEMT_REQ);
	SendMsgToAllMcs(MCU_VCS_RELEASEMT_REQ, cServMsg);
	
}

/*=============================================================================
�� �� ���� RemoveMtInMutiTvWallMoudle
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� void
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/12/08  4.0			�ܼ���                ����
=============================================================================*/
void CMcuVcInst:: RemoveMtInMutiTvWallMoudle(TMt tMt)
{
	for(u8 byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++)
	{
		u8 byTvId = 0;
		m_tConfEqpModule.m_tTvWallInfo[byLoop].RemoveMtByMtIdx(tMt.GetMtId(), byTvId);
		if( 0 != byTvId )
		{
			m_tConfEqpModule.m_tMultiTvWallModule.RemoveMtByTvId(byTvId, tMt.GetMtId());
		}
	} 
}

/*=============================================================================
�� �� ���� RemoveMtFormPeriInfo
��    �ܣ� ��ɾ�����Ҷϡ��ն˵��ߵ������������������е���Ϣ������
           ������������Ӧ������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TMt tMt  Ҫɾ�����նˣ�֧�ַ�local��Tmt��Ϣ��
           bReason        ��ɾ����ԭ��
�� �� ֵ�� void
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
20100417    4.6		    pengjie                create
=============================================================================*/
void CMcuVcInst::RemoveMtFormPeriInfo( const TMt &tMt, u8 byReason )
{
	// [pengjie 2010/4/17] �ýӿ�֧�ִ���local��Tmt��Ϣ��������ҵ������
	//                     �ǲ����ж�ش��ģ������ⲿ��ҵ��ֻȡlocal����MCU��Ϣ
	TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMt );

	CServMsg	cServMsg;
	BOOL32		bIsExceptionReason = FALSE;

	//zjj20091014 �ն�����ԭ���ж�
	if( MTLEFT_REASON_EXCEPT == byReason ||
		MTLEFT_REASON_RTD == byReason ||
		MTLEFT_REASON_UNREACHABLE == byReason )	 
	{		
		m_ptMtTable->SetDisconnectReason( tMt.GetMtId(),byReason );
		bIsExceptionReason = TRUE;		
	}

	// 0�� [11/3/2010 xliang] �ȿ�������������Ӱ�������MIXER��VMP
	BOOL32 bRollIgnoreMixer = FALSE;
	BOOL32 bRollIgnoreVmp	= FALSE;
	if ( ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
    {
		if( tMt == m_tRollCaller || tMt == m_tRollCallee)
		{
			bRollIgnoreMixer = TRUE;
			if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
			{
				bRollIgnoreVmp = TRUE;
			}
			u16 wErrorCode = (tMt == m_tRollCaller)? ERR_MCU_ROLLCALL_CALLERLEFT: ERR_MCU_ROLLCALL_CALLEELEFT;
			NotifyMcsAlarmInfo(0, wErrorCode);
			RollCallStop(cServMsg);

		}
    }

	// 1���Ƴ��ն˶����ش������Ϣ 
	// Prsֻ����Ա����նˣ�������local������Ϣ
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (tConfAttrb.IsResendLosePack())
	{
		if(tMt.IsLocal())
		{
			if (tMt == GetAudBrdSrc())
			{
				StopBrdPrs(MODE_AUDIO);
			}
			else
			{
				TMt tMtSrc;
				u8 bySrcChn = 0;
				if(GetMtRealSrc(tLocalMt.GetMtId(), MODE_AUDIO, tMtSrc, bySrcChn))
				{
					u8 byPrsId = 0;
					u8 byPrsChnId = 0;
					if (tMtSrc == GetLocalAudBrdSrc())
					{
						if(FindPrsChnForBrd(MODE_AUDIO, byPrsId, byPrsChnId))
						{
							AddRemovePrsMember(tLocalMt.GetMtId(), byPrsId, byPrsChnId, MODE_AUDIO, FALSE);
						}
					}
					else
					{
						if(FindPrsChnForSrc(tMtSrc, bySrcChn, MODE_AUDIO, byPrsId, byPrsChnId))
						{
							AddRemovePrsMember(tLocalMt.GetMtId(), byPrsId, byPrsChnId, MODE_AUDIO, FALSE);
						}
					}
				}
				else
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[RemoveMtFormPeriInfo] GetMtRealSrc Mt.%d failed!\n", tLocalMt.GetMtId());
				}
			}

			if (tMt == GetVidBrdSrc())
			{
				StopBrdPrs(MODE_VIDEO);
			}
			else
			{
				TMt tMtSrc;
				u8 bySrcChn = 0;
				if(GetMtRealSrc(tLocalMt.GetMtId(), MODE_VIDEO, tMtSrc, bySrcChn))
				{
					u8 byPrsId = 0;
					u8 byPrsChnId = 0;
					if (tMtSrc == GetLocalVidBrdSrc())
					{
						if(FindPrsChnForBrd(MODE_VIDEO, byPrsId, byPrsChnId))
						{
							AddRemovePrsMember(tLocalMt.GetMtId(), byPrsId, byPrsChnId, MODE_VIDEO, FALSE);
						}
					}
					else
					{
						if(FindPrsChnForSrc(tMtSrc, bySrcChn, MODE_VIDEO, byPrsId, byPrsChnId))
						{
							AddRemovePrsMember(tLocalMt.GetMtId(), byPrsId, byPrsChnId, MODE_VIDEO, FALSE);
						}
					}
				}
				else
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[RemoveMtFormPeriInfo] GetMtRealSrc Mt.%d failed!\n", tLocalMt.GetMtId());
				}
			}

			if (tMt == m_tDoubleStreamSrc)
			{
				StopBrdPrs(MODE_SECVIDEO);
			}
			else
			{
				TMt tMtSrc;
				u8 bySrcChn = 0;
				if(GetMtRealSrc(tLocalMt.GetMtId(), MODE_SECVIDEO, tMtSrc, bySrcChn))
				{
					u8 byPrsId = 0;
					u8 byPrsChnId = 0;
					if (tMtSrc == m_tDoubleStreamSrc)
					{
						if(FindPrsChnForBrd(MODE_SECVIDEO, byPrsId, byPrsChnId))
						{
							AddRemovePrsMember(tLocalMt.GetMtId(), byPrsId, byPrsChnId, MODE_SECVIDEO, FALSE);
						}
					}
					else
					{
						if(FindPrsChnForSrc(tMtSrc, bySrcChn, MODE_SECVIDEO, byPrsId, byPrsChnId))
						{
							AddRemovePrsMember(tLocalMt.GetMtId(), byPrsId, byPrsChnId, MODE_SECVIDEO, FALSE);
						}
					}
				}
				else
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[RemoveMtFormPeriInfo] GetMtRealSrc Mt.%d failed!\n", tLocalMt.GetMtId());
				}
			}	
			
			if (GetLocalVidBrdSrc() == m_tVmpEqp)
			{
				u8 bySrcChnnl = GetVmpOutChnnlByDstMtId( tMt.GetMtId() , m_tVmpEqp.GetEqpId());
				if ( 0xFF != bySrcChnnl ) 
				{
					u8 byPrsId    = 0;
					u8 byPrsChnId = 0;
					if (FindPrsChnForSrc(m_tVmpEqp, bySrcChnnl, MODE_VIDEO, byPrsId, byPrsChnId))
					{
						AddRemovePrsMember(tMt.GetMtId(), byPrsId, byPrsChnId, MODE_VIDEO, FALSE);
					}
				}
				else
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[RemoveMtFormPeriInfo]Get tMt(%d,%d) vmp out chnnl failed! \n",tMt.GetMcuId(),tMt.GetMtId() );
				}
				TVMPParam_25Mem tVMPParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
				for (u8 byMtIdx = 0; byMtIdx < tVMPParam.GetMaxMemberNum(); byMtIdx++)
				{
					TVMPMember *ptVmpMember = tVMPParam.GetVmpMember(byMtIdx);
					if (ptVmpMember != NULL && *ptVmpMember == tMt)
					{
						StopRtcpSwitchAlltoOne(tMt, 0, MODE_VIDEO);
						break;
					}
				}	    	
			}	
		}
	}
			
	// 2��ֹͣ���� 
	if ( (m_tConf.m_tStatus.IsMixing() || m_tConf.m_tStatus.IsVACing() )
		&& !bRollIgnoreMixer && !tMt.IsNull() && tMt.IsLocal())
	{
		// xsl [8/4/2006] ���ƻ�����Ա����
		if (m_tConf.m_tStatus.IsSpecMixing())
		{
			if (m_ptMtTable->IsMtInMixing(tMt.GetMtId()))
			{
				//zjj20091014
				if( bIsExceptionReason )
				{
					//20101110_tzy ����쳣�����ն��ڻ������в��ҽ��и��նˣ���ôӦ��ֹͣ����
					if (GetCurConfMixMtNum() == 1)
					{
						for (u8 byMtid = 1; byMtid <= MAXNUM_CONF_MT; byMtid++)
						{
							if (m_ptMtTable->IsMtInMixing(byMtid) )
							{
								if (m_ptMtTable->GetMtType(byMtid) == MT_TYPE_MT
									&& tMt.GetMtId() ==  byMtid)
								{
									RemoveSpecMixMember(&tMt, 1, FALSE);
								}
							}
						}
					}
					else
					{
						RemoveSpecMixMember(&tMt, 1, FALSE);
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in mixing" );

						BOOL32 bKeepInMixing = TRUE; // �Ƿ����ڻ�����״̬���Ա��������ߵĻָ�
						if ( m_tConf.GetConfSource() == VCS_CONF)
						{
							bKeepInMixing = FALSE; // VCS����Ҫ�ָ�������Ϊ�������ȺͶ෽�໭���ǵ���֮�����������նˣ����ģʽ�£���Ȼ���������У������Ѿ����Ǳ������ն���
						}
						TMtStatus tRemeMtStatus;
						m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tRemeMtStatus);
						tRemeMtStatus.SetInMixing(bKeepInMixing);
						m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tRemeMtStatus);
					}
				}
				else
				{
					RemoveSpecMixMember(&tMt, 1, FALSE);
				}
				
			}            
		}
		else //vac�������Ƴ�ͨ��
		{            
			//�ڻ������ڴ˲������
			if (m_ptMtTable->IsMtInMixGrp(tMt.GetMtId()))
			{       
				StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
							(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt)), FALSE, MODE_AUDIO);
			}   
			RemoveMixMember(&tMt, FALSE); 
			GetMixChnPos(tMt,TRUE);

			//zjj20091014
			if( bIsExceptionReason )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in mixing group" );
				//m_ptMtTable->SetMtInMixGrp( tMt.GetMtId(), TRUE );
			}
		} 
	}

	// 3��ֹͣVMP
	if (GetVmpCountInVmpList() > 0)
	{
		u8 byVmpId;
		TEqp tVmpEqp;
		BOOL32 bStopVmp = TRUE;
		BOOL32 bChangeNotify = FALSE;
		BOOL32 bChangeStyle = TRUE;
		TVMPParam_25Mem tVMPParam;
		TVMPMember* ptVmpMember = NULL;
		TMt tMtNull;
		tMtNull.SetNull();
		BOOL32 bMemberInMcu = FALSE;
		u8 byMaxNum;
		u16 wVcsCurChIndx;
		BOOL32 bChangeVmpStyle = TRUE;//ChangeVmpStyle��������Զ�����ϳɣ��������1��
		for (u8 byVmpIdx=0; byVmpIdx<MAXNUM_CONF_VMP; byVmpIdx++)
		{
			if (!IsValidVmpId(m_abyVmpEqpId[byVmpIdx]))
			{
				continue;
			}
			byVmpId = m_abyVmpEqpId[byVmpIdx];
			tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );

			if( g_cMcuVcApp.GetVMPMode(tVmpEqp) != CONF_VMPMODE_NONE 
				&& !bRollIgnoreVmp )
			{
				bStopVmp = TRUE;
				bChangeNotify = FALSE;
				bChangeStyle = TRUE;
				tVMPParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);// m_tConf.m_tStatus.GetVmpParam();

				ptVmpMember = NULL;

				//lukunpeng 2010/07/09 VMP��Ա��������
				bMemberInMcu = FALSE;
				byMaxNum = tVMPParam.GetMaxMemberNum();
				wVcsCurChIndx = byMaxNum;
				//zhouyiliang 20101025 ��������񲻱������¹����������������նˣ���ǰ����ͨ���Ų�����maxnum
				//ֻ�ж෽�໭��������п��ܳ��ֹҶ��ն˻���ϳɷ�񲻱��������������ģʽ����ϳ����4���棬�ҶϺ���ı䣩
				if ( VCS_CONF == m_tConf.GetConfSource() && m_cVCSConfStatus.GetCurVCMode() == VCS_MULVMP_MODE )
				{
					//zhouyiliang 20101111�෽�໭�������ǰcurchIndx��maxnum��һ��,����ʵ�������ն�����ȷ��curchIndx
					if ( m_cVCSConfStatus.GetCurUseVMPChanInd() !=  byMaxNum ) 
					{
						wVcsCurChIndx = 1;
						for( u8 byMtIndex = 1; byMtIndex <= MAXNUM_CONF_MT; byMtIndex++ )
						{
							if ( !m_tConfAllMtInfo.MtJoinedConf(byMtIndex) 
							|| m_cVCSConfStatus.GetVcsBackupChairMan().GetMtId() == byMtIndex 
							|| m_tConf.GetChairman().GetMtId() == byMtIndex
							|| m_ptMtTable->GetMtType(byMtIndex) == MT_TYPE_VRSREC) //����vrs��¼��
							{
								continue;
							}
							wVcsCurChIndx++;
						}
					}
					
				}
				for (u8 byIndex = 0; byIndex < byMaxNum ; ++byIndex)
				{
					ptVmpMember = tVMPParam.GetVmpMember(byIndex);
					bMemberInMcu = FALSE;
					if (ptVmpMember == NULL)
					{
						continue;
					}

					if (ptVmpMember->IsNull())
					{
						continue;
					}

					//����Ǵ��ն��ڻ���ϳ��У����߹Ҷϵ����¼���mcu�����¼���mcuҲ��Ҫ������ϳ�
					//zhouyiliang 20100901 �Ҷϵ���mcu,�¼�mcu�µĻ���ϳɳ�ԱҪ��,�Ƚ���Ա����Ϊ�����ߣ��Զ�������Ա��ʱ��Ͳ���������ȥ
					bMemberInMcu = !tMt.IsNull() && IsMcu(tMt) && IsMtInMcu(tMt, (TMt)*ptVmpMember);
					if ( bMemberInMcu ) 
					{
						m_tConfAllMtInfo.RemoveJoinedMt( (TMt)*ptVmpMember );
					}
					if (*ptVmpMember == tMt
						|| bMemberInMcu
						)
					{
						//����ϳɳ�Ա����Ϊ�����ָ����ʱ����״̬����ɾ���䱾����������
						//�ϳɳ�Ա�����ڴ˲���������ֹ���������صĸ���ͨ����[03/21/2006-zbq]
						if ( VMP_MEMBERTYPE_MCSSPEC == ptVmpMember->GetMemberType() )
						{
							StopSwitchToPeriEqp( tVmpEqp.GetEqpId(), byIndex, FALSE, MODE_VIDEO );
						}
						if( !IsDynamicVmp(byVmpId) )
						{
							//zjj20091014
							//zhouyiliang 20101022 �¼�mcu�쳣���߻���������ʱ���mcu�µ�vmp��ԱҲҪ���
							if( bIsExceptionReason && !bMemberInMcu )
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveMtFromVmpInfo] bReason is true reseve mt in vmp" );
								bChangeStyle = FALSE;
							}
							else 
							{
								//zhouyiliang 20100901 ֻ�л��ָ����vmpmember ����clear�������緢���˸���ȱ���type
								if ( VMP_MEMBERTYPE_MCSSPEC == ptVmpMember->GetMemberType() )
								{
									tVMPParam.ClearVmpMember(byIndex);
								}
								else
								{
									ptVmpMember->SetMemberTMt(tMtNull);
								}
								bChangeNotify = TRUE;
							}
						}
						else//zhouyiliang 20100901 �Ƕ�̬���Զ�����ϳɣ�
						{
							//zjj20091014
							//TMt tRemoveMt = bMemberInMcu ? (TMt)*ptVmpMember : tMt;
							// ����ѭ������,��ChangeVmpStyle�Ƶ�forѭ������,bMemberInMcu��trueʱ,�����Ѿ����¼���Ա����RemoveJoinedMt
							// �˴���ChangeVmpStyle����*ptVmpMemberû��Ҫ,ֻ��Ҫ��forѭ���⽫mcu���������뼴��
							if( bChangeStyle )
							{
								//��̬�����ı���
								//zhouyiliang 20100907 vcs�Զ�����ϳɵ�ǰ����ͨ����Ӧ�ñ�
								//��ϯ�ն˵���ʱ,����Ҫ����CurUseVMPChanInd,�ȴ���ϯ�ն�����
								if ( VCS_CONF == m_tConf.GetConfSource() && tVMPParam.IsVMPAuto() 
									&& !(tMt == m_tConf.GetChairman()) )
								{
									
									wVcsCurChIndx--;
									if ( 0 == wVcsCurChIndx  ) 
									{
										wVcsCurChIndx =  1;
									}
									m_cVCSConfStatus.SetCurUseVMPChanInd( wVcsCurChIndx );
								}
								bChangeNotify = TRUE;
							}

						}
						
						//zjj20091014
						if( !bIsExceptionReason )
						{
							// zbq [04/25/2007] ������ն��ڻ���ģ���VMP��Ϣ�ֱ��ɾ��֮
							for( u8 byLoop = 0; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop ++ )
							{
								// �����Ǽ�������ϳɵ����
								if ( TYPE_MT == tMt.GetMtType() &&
									m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byLoop] == tMt.GetMtId() ) 
								{
									m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byLoop] = 0;
								}
							}
						}
					}
				}
			
				if( !IsDynamicVmp(byVmpId) )//zhouyiliang 20100901��̬������ʱ���Ѿ�����ChangeVmpStyle
				{
					g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tVMPParam);
				}
				
				if( TRUE == bChangeNotify )
				{
					if (IsDynamicVmp(byVmpId))
					{
						if (bChangeVmpStyle)
						{
							//��̬�����ڴ˴�ͳһ����
							ChangeVmpStyle( tMt , FALSE );
							bChangeVmpStyle = FALSE;//�Զ�vmpֻ��1��ChangeVmpStyle
						}
					}
					else
					{
						/*if (tVMPParam.GetVMPMemberNum() == 0
							&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_DSTREAM)	//˫������ʱ,��ͣvmp
							&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_VMPCHLPOLL)	//��ͨ����ѯʱ,Ҳ��ͣvmp
							&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
							&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER)) // 2011-10-12 add by pgf: ������и���ͨ������ͣVMP
						{
							//ֹͣ��������,�����ͳһ������ֹͣvmp
							ChangeVmpSwitch(byVmpId, VMP_STATE_STOP );
							
							//ֹͣ
							TPeriEqpStatus tPeriEqpStatus;
							if (!tVmpEqp.IsNull() &&
								g_cMcuVcApp.GetPeriEqpStatus(tVmpEqp.GetEqpId(), &tPeriEqpStatus) &&
								tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
								tPeriEqpStatus.m_tStatus.tVmp.m_byUseState != TVmpStatus::WAIT_STOP)
							{
								u8 byTimerVmpIdx = tVmpEqp.GetEqpId() - VMPID_MIN;
								SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER+byTimerVmpIdx, TIMESPACE_WAIT_VMPRSP);
								tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
								g_cMcuVcApp.SetPeriEqpStatus(tVmpEqp.GetEqpId() , &tPeriEqpStatus);	
								
								CServMsg cTempServ;
								SendMsgToEqp(tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cTempServ); 
							}
						}
						else*/
						{
							// �����ѯ���ڹҶϵ�ǰ��ѯ�ն�ʱ����������ѯ��һ�նˣ����и���ͨ�����˴β��ص�����
							// ��ѯ����һ�ն�ʱ�����������ֹ�������ο��ٵ���vmp
							if (CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode()
								&& m_tConf.m_tStatus.GetPollState() == POLL_STATE_NORMAL)
							{
								bMemberInMcu = !tMt.IsNull() && IsMcu(tMt) && IsMtInMcu(tMt, (TMt)m_tConf.m_tStatus.GetMtPollParam());//��ǰ��ѯ�ն��Ƿ��ڵ���mcu��
								if (CONF_POLLMODE_SPEAKER == m_tConf.m_tStatus.GetPollMode() ||
									CONF_POLLMODE_SPEAKER_BOTH == m_tConf.m_tStatus.GetPollMode())
								{
									if ((tMt == m_tConf.GetSpeaker() || bMemberInMcu) && 
										(tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
										|| tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER)))
									{
										bChangeNotify = FALSE;
									}
								}
								else if (CONF_POLLMODE_VIDEO == m_tConf.m_tStatus.GetPollMode() ||
									CONF_POLLMODE_VIDEO_BOTH == m_tConf.m_tStatus.GetPollMode())
								{
									if ((tMt == (TMt)m_tConf.m_tStatus.GetMtPollParam() || bMemberInMcu) &&
										tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_POLL))
									{
										bChangeNotify = FALSE;
									}
								}
							}
							// ��Ҫ����vmpʱ������vmp
							if (bChangeNotify)
							{
								AdjustVmpParam(byVmpId, &tVMPParam);
							}
						}
					}
				}
			}
		}
		ConfStatusChange();
	}

    //  4�� ͣ�໭�����ǽ
	// [pengjie 2010/4/17] Fixme���໭�����ǽҲ����ش���ȡlocal��Tmt��Ϣ
    if(m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE)
    {
        BOOL32 bStopVmpTw = TRUE;
        BOOL32 bChangeNotify = FALSE;
        TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpTwParam();
        for( u8 byLoop = 0; byLoop < MAXNUM_SDVMP_MEMBER; byLoop++  )
        {
            TMt tMembMt = (TMt)(*tVMPParam.GetVmpMember(byLoop));
            TMt tTempLocalMt = GetLocalMtFromOtherMcuMt( tMembMt );
            if( tTempLocalMt.GetMtId() == tMt.GetMtId() )
            {
                TVMPMember tVmpMember = *tVMPParam.GetVmpMember(byLoop);

                //����ϳɳ�Ա����Ϊ�����ָ����ʱ����״̬����ɾ���䱾����������
                //�ϳɳ�Ա�����ڴ˲���������ֹ���������صĸ���ͨ����[12/04/2006-zbq]
                if ( VMPTW_MEMBERTYPE_MCSSPEC == tVmpMember.GetMemberType() ) 
                {
                    StopSwitchToPeriEqp( m_tVmpTwEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO );
                    
                    TVMPMember tVMPMember;
                    memset( &tVMPMember, 0, sizeof(TVMPMember) );
					//zjj20091014
					if( bIsExceptionReason )
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in vmpTW" );						
					}
					else
					{
						tVMPParam.SetVmpMember( byLoop, tVMPMember );
					}
                    bChangeNotify = TRUE;
                }
            }
            
            TVMPMember tVmpMember = *tVMPParam.GetVmpMember(byLoop);            
            if (tVmpMember.GetMemberType() != 0 || !tVmpMember.IsNull())
            {
                bStopVmpTw = FALSE;
            }
        }
        m_tConf.m_tStatus.SetVmpTwParam( tVMPParam );               
                
        if( TRUE == bChangeNotify )
        {
            TPeriEqpStatus tVmpEqpStatus;
            g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tVmpEqpStatus);
            
            if (bStopVmpTw)
            {
                tVmpEqpStatus.m_tStatus.tVmp.m_byUseState = FALSE;
                m_tConf.m_tStatus.SetVmpTwMode(CONF_VMPTWMODE_NONE);
            }
            
            tVmpEqpStatus.m_tStatus.tVmp.SetVmpParam(m_tConf.m_tStatus.m_tVmpTwParam);
            g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tVmpEqpStatus);
            
            if (bStopVmpTw)
            {
                //ֹͣ��������
                ChangeVmpTwSwitch( VMPTW_STATE_STOP );
                
                //ֹͣ
                CServMsg cTempServ;
                SendMsgToEqp(m_tVmpTwEqp.GetEqpId(), MCU_VMPTW_STOPVIDMIX_REQ, cTempServ);         
            }
            else
            {
                ChangeVmpTwParam(&tVMPParam);
            }
        }   
        
        ConfStatusChange();
    }

	TMtStatus tMtStatus;
  // 5��ֹͣTvWall
    u8 byEqpId = TVWALLID_MIN;
	TPeriEqpStatus tTvwallStatus;
	TMt tTmpMt;
	tTmpMt.SetNull();
    while( byEqpId >= TVWALLID_MIN && byEqpId <= TVWALLID_MAX )
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byEqpId))
		{
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
            if (tTvwallStatus.m_byOnline == 1)
            {
                u8 byMemberNum = tTvwallStatus.m_tStatus.tTvWall.byChnnlNum;
				for(u8 byLoop = 0; byLoop < min(byMemberNum,MAXNUM_PERIEQP_CHNNL); byLoop++)
				{
					tTmpMt = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop];
					const u8 byOrigMmbType = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType;

                    //20100708_tzy ����������жϽ������ϣ�����ԭ���߼������������ѯ���������ǽ����ʱ�Ҷϲ���������ѯ״̬���նˣ�
                    //��ô����ǽҲ���Զ�������һ���ն�ȥ
					if (tMt == tTmpMt || (IsMcu(tMt) && IsMtInMcu(tMt,tTmpMt))
						&& m_byConfIdx == tTmpMt.GetConfIdx())
					{
						if (TW_MEMBERTYPE_POLL == byOrigMmbType)
						{
							TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
							
							ChangeTvWallSwitch( NULL, byEqpId, byLoop, TW_MEMBERTYPE_POLL, TW_STATE_CHANGE );
							g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
							tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byOrigMmbType;
							tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
        
							//zjj20091014
							if( bIsExceptionReason )
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true(TW_MEMBERTYPE_POLL) reseve mt in TVWall" );						
							}
						}
						else if ( TW_MEMBERTYPE_TWPOLL == byOrigMmbType )
						{
							// ������ʼ�µ���ѯ
							u32 dwTimerIdx = 0;
							if( m_tTWMutiPollParam.GetTimerIdx(byEqpId, byLoop, dwTimerIdx) )
							{
								SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx,100);
							}
						}else if ( TW_MEMBERTYPE_CHAIRMAN == byOrigMmbType 
							|| TW_MEMBERTYPE_SPEAKER == byOrigMmbType )
						{
							ChangeTvWallSwitch(NULL, byEqpId, byLoop, byOrigMmbType, TW_STATE_CHANGE);
							g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
							tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byOrigMmbType;
							tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
						}
						// [9/26/2011 liuxu] Vcs�����������ն˵���, ��Ҫ��յ���ǽ
						else if ( VCS_CONF == m_tConf.GetConfSource() 
							&& m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE
							&& TW_MEMBERTYPE_VCSSPEC == tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType )
						{
							ChangeTvWallSwitch(NULL,  byEqpId, byLoop, TW_MEMBERTYPE_VCSSPEC, TW_STATE_STOP);
							g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
						}
						else
						{
							StopSwitchToPeriEqp(byEqpId, byLoop);

							//zjj20091014
							if( bIsExceptionReason )
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in TVWall" );						
							}
							else
							{
								//20101111_tzy VCS����Ԥ��ģʽ�µ���ǽ���仯
								if ( (VCS_CONF == m_tConf.GetConfSource() 
									&& m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE))
								{
									tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byOrigMmbType;
									tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetMt(tTmpMt);
									tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
								}
								else
								{
									tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = TW_MEMBERTYPE_NULL;
									tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
									tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
									GetMtStatus(tMt, tMtStatus);
									tMtStatus.SetInTvWall(FALSE);
									SetMtStatus(tMt, tMtStatus);
									MtStatusChange(&tMt, TRUE);
								}
							}
						}
					}

					//20101206_tzy ֻҪ�Ҷϵ���mcu��������Ҫ�ָ�
					if (IsMcu(tMt) && IsMtInMcu(tMt,tTmpMt))
					{
						tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
						tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
						tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
					}
				}

                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tTvwallStatus);
                cServMsg.SetMsgBody((u8 *)&tTvwallStatus, sizeof(tTvwallStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);

				ConfStatusChange();
            }
		}

        byEqpId++;
	}

    // 6��ֹͣhdu
    u8 byHduId = HDUID_MIN;
	TPeriEqpStatus tHduStatus;

	//20101130_tzy Bug00042441R6B2:������ش����Զ�����ǽģʽ�£����ȸ����ն˽�����ǽ�������¼�MCU�����������ǽ�Ľ����������
	//���ñ�־λ�Ƶ�����������ĳЩ�Ǳ����ն�Ҳ���߳�����ǽ
	//BOOL32 bIsRemoveMtInSMcu = FALSE;
    while( byHduId >= HDUID_MIN && byHduId <= HDUID_MAX )
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byHduId))
		{
			g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
            if (tHduStatus.m_byOnline == 1)
            {
                u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byHduId));
				if (0 == byHduChnNum)
				{
					byHduId++;
					ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU, "[RemoveMtFormPeriInfo] GetHduChnNumAcd2Eqp failed!\n");
					continue;
				}
				for(u8 byLoop = 0; byLoop < min(byHduChnNum,MAXNUM_HDU_CHANNEL); byLoop++)
				{
					// [2013/03/11 chenbing]  
					if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byLoop) )
					{
						for (u8 byIndex=0; byIndex<HDU_MODEFOUR_MAX_SUBCHNNUM; byIndex++)
						{
							tTmpMt = tHduStatus.m_tStatus.tHdu.GetChnMt(byLoop, byIndex);
							if( tTmpMt.IsNull() || tTmpMt.GetConfIdx() != m_byConfIdx )
							{
								continue;
							}

							BOOL32 bIsRemoveMtInSMcu = FALSE;
							// �Ҷϵ���mcu�����µ��ն������ڵ���ǽ�е�Ҫ���
							if( !tMt.IsNull() && IsMcu(tMt) && IsMtInMcu(tMt, tTmpMt) )
							{
								bIsRemoveMtInSMcu = TRUE;
							}

							if ( (tMt == tTmpMt || bIsRemoveMtInSMcu ) 
								&& m_byConfIdx == tTmpMt.GetConfIdx()
							   )
							{
								// ֹͣ����ͨ���Ľ���
								//StopSwitchToPeriEqp(byHduId, byLoop, TRUE, MODE_VIDEO, SWITCH_MODE_BROADCAST, byIndex);
								//Bug00161975:�����������¼�MCU������ǽ����ϳ�ͨ�����Ҷ��¼���������ϴ�ͨ���е��ն˷ֱ��ʲ��ָ�
								//yrl20131119:HDU�ķָ񲻵���Ҫ�𽻻������зֱ��ʵ���
								const u8 byMemberType = tHduStatus.m_tStatus.tHdu.GetMemberType(byLoop, byIndex);
								ChangeHduSwitch( NULL, byHduId, byLoop, byIndex, byMemberType, TW_STATE_STOP );
	
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"[RemoveMtFormPeriInfo] remove mt(%d)(%d) from Hdu(%d)(%d)!\n",
										tTmpMt.GetMcuId(),tTmpMt.GetMtId() ,byHduId,byLoop);
									
								if( bIsExceptionReason )
								{
									ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in HDU" );	
								}
								else
								{
									tHduStatus.m_tStatus.tHdu.SetMemberType(TW_MEMBERTYPE_NULL, byLoop, byIndex);
									tHduStatus.m_tStatus.tHdu.SetChnNull(byLoop, byIndex);
									tHduStatus.m_tStatus.tHdu.SetSchemeIdx(0, byLoop, byIndex);
									GetMtStatus(tMt, tMtStatus);
									tMtStatus.SetInHdu(FALSE);
									SetMtStatus(tMt, tMtStatus);
									MtStatusChange(&tMt, TRUE);
								}
							}

							//ֻҪ�Ҷϵ���mcu��������Ҫ�ָ�
							if (bIsRemoveMtInSMcu)
							{
								tHduStatus.m_tStatus.tHdu.SetMemberType(TW_MEMBERTYPE_NULL, byLoop, byIndex);
								tHduStatus.m_tStatus.tHdu.SetChnNull(byLoop, byIndex);
								tHduStatus.m_tStatus.tHdu.SetSchemeIdx(0, byLoop, byIndex);
							}
						}
					}
					else
					{
						tTmpMt = (TMt)tHduStatus.m_tStatus.tHdu.GetChnMt(byLoop);
						const u8 byOrigMmbType = tHduStatus.m_tStatus.tHdu.GetMemberType(byLoop);

						if( tTmpMt.IsNull() || tTmpMt.GetConfIdx() != m_byConfIdx )
						{
							continue;
						}

						BOOL32 bIsRemoveMtInSMcu = FALSE;
						// [pengjie 2010/9/3] �Ҷϵ���mcu�����µ��ն������ڵ���ǽ�е�Ҫ���
						if( !tMt.IsNull() && IsMcu(tMt) && IsMtInMcu(tMt, tTmpMt) )
						{
							bIsRemoveMtInSMcu = TRUE;
						}

						//20100708_tzy ����������жϽ������ϣ�����ԭ���߼������������ѯ���������ǽ����ʱ�Ҷϲ���������ѯ״̬���նˣ�
						//��ô����ǽҲ���Զ�������һ���ն�ȥ
						if ( (tMt == tTmpMt
								|| ( IsMcu(tTmpMt) && GetSMcuViewMt(tTmpMt, TRUE) == tMt )
								|| bIsRemoveMtInSMcu 
								|| (byOrigMmbType == TW_MEMBERTYPE_DOUBLESTREAM && GetLocalMtFromOtherMcuMt(tMt) == tTmpMt) ) 
							&& m_byConfIdx == tTmpMt.GetConfIdx() )
						{
							if (TW_MEMBERTYPE_POLL == byOrigMmbType)
							{
								// [2013/03/11 chenbing] HDU�໭��Ŀǰ��֧����ѯ����,��ͨ����0
								ChangeHduSwitch( NULL, byHduId, byLoop, 0, TW_MEMBERTYPE_POLL, TW_STATE_STOP );

								// ��ȡͨ��״̬
								g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
								tHduStatus.m_tStatus.tHdu.SetMemberType(TW_MEMBERTYPE_POLL, byLoop);
								tHduStatus.m_tStatus.tHdu.SetConfIdx(m_byConfIdx, byLoop);
                        
								//zjj20091014
								if( bIsExceptionReason )
								{
									ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true(TW_MEMBERTYPE_POLL) reseve mt in HDU" );						
								}
							} 
							else if (TW_MEMBERTYPE_SPEAKER == byOrigMmbType)
							{
								// [pengjie 2010/4/17] ������һ�������ñ�local����
								TMt tSpeaker = m_tConf.GetSpeaker();
								// [2013/03/11 chenbing] HDU�໭��Ŀǰ��֧�ַ����˸���,��ͨ����0
								ChangeHduSwitch( NULL, byHduId, byLoop, 0, TW_MEMBERTYPE_SPEAKER, TW_STATE_STOP );

								// ��ȡͨ��״̬
								g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
								tHduStatus.m_tStatus.tHdu.SetMemberType(byOrigMmbType, byLoop);
								tHduStatus.m_tStatus.tHdu.SetConfIdx(m_byConfIdx, byLoop);

								//zjj20091014
								if( bIsExceptionReason )
								{
									ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true(TW_MEMBERTYPE_SPEAKER) reseve mt in HDU" );						
								}
							} 
							else if (TW_MEMBERTYPE_CHAIRMAN == byOrigMmbType)
							{
								// [pengjie 2010/4/17] ��ϯ�������¼��ն�����ȡlocal��Tmt��Ϣ
								TMt tLocalSpeaker = m_tConf.GetChairman();
								tLocalSpeaker = GetLocalMtFromOtherMcuMt( tLocalSpeaker );
								// [2013/03/11 chenbing] HDU�໭��Ŀǰ��֧����ϯ����,��ͨ����0
								ChangeHduSwitch( NULL, byHduId, byLoop, 0, TW_MEMBERTYPE_CHAIRMAN, TW_STATE_STOP);

								// ��ȡͨ��״̬
								g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
								tHduStatus.m_tStatus.tHdu.SetMemberType(byOrigMmbType, byLoop);
								tHduStatus.m_tStatus.tHdu.SetConfIdx(m_byConfIdx, byLoop);

								if( bIsExceptionReason )
								{
									ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true(TW_MEMBERTYPE_CHAIRMAN) reseve mt in HDU" );						
								}
							}
							else if ( TW_MEMBERTYPE_DOUBLESTREAM == byOrigMmbType )
							{
								// [pengjie 2010/9/9] �����ﲻͣ�����¼�mcu���ر�˫���߼�ͨ��ʱͳһ����
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  
									"[RemoveMtFormPeriInfo] Wait Mt.%d come to CloseDulChl then remove it from Hdu!\n",
									tTmpMt.GetMtId() );
							}
							else if ( TW_MEMBERTYPE_TWPOLL == byOrigMmbType )
							{
								u32 dwTimerIdx = 0;
								if( m_tTWMutiPollParam.GetTimerIdx(byHduId, byLoop, dwTimerIdx) )
								{
									SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx,100);
								}
							}
							// [9/26/2011 liuxu] Vcs�����������ն˵���, ��Ҫ��յ���ǽ
							else if ( VCS_CONF == m_tConf.GetConfSource() 
								&& m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE
								&& TW_MEMBERTYPE_VCSSPEC == byOrigMmbType )
							{
								// [2013/03/11 chenbing] VCS���鲻֧��HDU�໭��,��ͨ����0
								ChangeHduSwitch(NULL,  byHduId, byLoop, 0, TW_MEMBERTYPE_VCSSPEC, TW_STATE_STOP);
								g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
							}
							else
							{
								StopSwitchToPeriEqp(byHduId, byLoop);

								ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"[RemoveMtFormPeriInfo] remove mt(%d)(%d) from Hdu(%d)(%d)!\n",
									tTmpMt.GetMcuId(),tTmpMt.GetMtId() ,byHduId,byLoop);
								
								//zjj20091014
								if( bIsExceptionReason )
								{
									ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in HDU" );	
								}
								else
								{
									//20101111_tzy VCS����Ԥ��ģʽ�µ���ǽ���仯
									//20110420 ������ѯ״̬��Ҳ������״̬���������
									if ( POLL_STATE_NONE != m_tHduBatchPollInfo.GetStatus() 
										|| (VCS_CONF == m_tConf.GetConfSource() && m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE))
									{
										tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = byOrigMmbType;
										tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetMt(tTmpMt);
										tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
										if (MTLEFT_REASON_DELETE == byReason)
										{
											//����ɾ������Ҫ���״̬
											GetMtStatus(tMt, tMtStatus);
											tMtStatus.SetInHdu(FALSE);
											SetMtStatus(tMt, tMtStatus);
											MtStatusChange(&tMt, TRUE);
										}
									}
									else
									{
										tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = TW_MEMBERTYPE_NULL;
										tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
										tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
										tHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetSchemeIdx(0);
										GetMtStatus(tMt, tMtStatus);
										tMtStatus.SetInHdu(FALSE);
										SetMtStatus(tMt, tMtStatus);
										MtStatusChange(&tMt, TRUE);
									}
								}
							}
						}
						//20101206_tzy ֻҪ�Ҷϵ���mcu��������Ҫ�ָ�
						if (bIsRemoveMtInSMcu)
						{

							tHduStatus.m_tStatus.tHdu.SetMemberType(0, byLoop);
							tHduStatus.m_tStatus.tHdu.SetChnNull(byLoop);
							tHduStatus.m_tStatus.tHdu.SetConfIdx(0, byLoop);
						}
					}
				}

                g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);
                cServMsg.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
            }
			else/*HDU������ҲҪ���mt*/
			{
				for(u8 byLoop = 0; byLoop < MAXNUM_HDU_CHANNEL; byLoop++)
				{
					// �ķ��  
					if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byLoop) )
					{
						for (u8 byIndex=0; byIndex<HDU_MODEFOUR_MAX_SUBCHNNUM; byIndex++)
						{
							tTmpMt = tHduStatus.m_tStatus.tHdu.GetChnMt(byLoop, byIndex);
							if( tTmpMt.IsNull() || tTmpMt.GetConfIdx() != m_byConfIdx )
							{
								continue;
							}

							BOOL32 bIsRemoveMtInSMcu = FALSE;
							// �Ҷϵ���mcu�����µ��ն������ڵ���ǽ�е�Ҫ���
							if( !tMt.IsNull() && IsMcu(tMt) && IsMtInMcu(tMt, tTmpMt) )
							{
								bIsRemoveMtInSMcu = TRUE;
							}

							if ( (tMt == tTmpMt || bIsRemoveMtInSMcu ) 
								&& m_byConfIdx == tTmpMt.GetConfIdx()
							   )
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"[RemoveMtFormPeriInfo] remove mt(%d)(%d) from Hdu(%d)(%d)!\n",
										tTmpMt.GetMcuId(),tTmpMt.GetMtId() ,byHduId,byLoop);
									
								if( bIsExceptionReason )
								{
									ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in HDU" );	
								}
								else
								{
									tHduStatus.m_tStatus.tHdu.SetMemberType(TW_MEMBERTYPE_NULL, byLoop, byIndex);
									tHduStatus.m_tStatus.tHdu.SetChnNull(byLoop, byIndex);
									tHduStatus.m_tStatus.tHdu.SetSchemeIdx(0, byLoop, byIndex);
									if (byIndex)
									{
										tHduStatus.m_tStatus.tHdu.SetConfIdx(0, byLoop, byIndex);
									}
									GetMtStatus(tMt, tMtStatus);
									tMtStatus.SetInHdu(FALSE);
									SetMtStatus(tMt, tMtStatus);
									MtStatusChange(&tMt, TRUE);
								}
							}

							//ֻҪ�Ҷϵ���mcu��������Ҫ�ָ�
							if (bIsRemoveMtInSMcu)
							{
								tHduStatus.m_tStatus.tHdu.SetMemberType(TW_MEMBERTYPE_NULL, byLoop, byIndex);
								tHduStatus.m_tStatus.tHdu.SetChnNull(byLoop, byIndex);
								tHduStatus.m_tStatus.tHdu.SetSchemeIdx(0, byLoop, byIndex);
							}
						}
					}
					else
					{
						tTmpMt = (TMt)tHduStatus.m_tStatus.tHdu.GetChnMt(byLoop);
						const u8 byOrigMmbType = tHduStatus.m_tStatus.tHdu.GetMemberType(byLoop);

						if( tTmpMt.IsNull() || tTmpMt.GetConfIdx() != m_byConfIdx )
						{
							continue;
						}

						BOOL32 bIsRemoveMtInSMcu = FALSE;
						// [pengjie 2010/9/3] �Ҷϵ���mcu�����µ��ն������ڵ���ǽ�е�Ҫ���
						if( !tMt.IsNull() && IsMcu(tMt) && IsMtInMcu(tMt, tTmpMt) )
						{
							bIsRemoveMtInSMcu = TRUE;
						}

						//20100708_tzy ����������жϽ������ϣ�����ԭ���߼������������ѯ���������ǽ����ʱ�Ҷϲ���������ѯ״̬���նˣ�
						//��ô����ǽҲ���Զ�������һ���ն�ȥ
						if ( (tMt == tTmpMt
							 || bIsRemoveMtInSMcu 
							 || ( IsMcu(tTmpMt) && GetSMcuViewMt(tTmpMt, TRUE) == tMt ))
							&& m_byConfIdx == tTmpMt.GetConfIdx() )
						{
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"[RemoveMtFormPeriInfo] remove mt(%d)(%d) from Hdu(%d)(%d)!\n",
									tTmpMt.GetMcuId(),tTmpMt.GetMtId() ,byHduId,byLoop);
								
								//zjj20091014
							if( bIsExceptionReason )
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in HDU" );	
							}
							else
							{
								u8 byMeMberType = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType;
								if ( TW_MEMBERTYPE_SPEAKER == byMeMberType
									|| TW_MEMBERTYPE_CHAIRMAN == byMeMberType
									|| TW_MEMBERTYPE_POLL == byMeMberType)
								{
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
								}
								else
								{
									tHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetSchemeIdx(0);
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = TW_MEMBERTYPE_NULL;
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
								}
								
								g_cMcuVcApp.SetChnnlMMode(byHduId, byLoop, MODE_NONE);
								GetMtStatus(tMt, tMtStatus);
								tMtStatus.SetInHdu(FALSE);
								SetMtStatus(tMt, tMtStatus);
								MtStatusChange(&tMt, TRUE);
							}
						}
						//20101206_tzy ֻҪ�Ҷϵ���mcu��������Ҫ�ָ�
						if (bIsRemoveMtInSMcu)
						{

							tHduStatus.m_tStatus.tHdu.SetMemberType(0, byLoop);
							tHduStatus.m_tStatus.tHdu.SetChnNull(byLoop);
							tHduStatus.m_tStatus.tHdu.SetConfIdx(0, byLoop);
						}
					}
					g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);
				}
			}
		}
	
        byHduId++;
	}

	// [11/11/2011 liuxu] Ӧ���ɸ�ҵ���Լ�����, ���ﲻӦ���ٻ�������
// 	TMtStatus tMtStatus;
// 	GetMtStatus(tMt, tMtStatus);
// 
// 	SetMtStatus(tMt, tMtStatus);

	//[03/04/2010]	zjl modify (����ǽ��ͨ����ѯ����ϲ�)
	//����ǽ��ѯ��������
	u32 dwTwTimerIdx = 0;
	for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
	{
		//Ҫ�Ƴ��ն�����ѯ�б��е�λ��
		u8 byTmtPos = ~0;
		u8 byCurPollPos = m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetCurrentIdx();
		//������ѯ��ֹͣ״̬���Ҹ��ն�����ѯ�б���
		if (POLL_STATE_NONE != m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetPollState() && 
			m_tTWMutiPollParam.m_tTWPollParam[byIdx].IsExistMt(tMt,byTmtPos)&&
			m_tTWMutiPollParam.GetTimerIdx(m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetTvWall().GetEqpId(),
			m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetTWChnnl(), dwTwTimerIdx))
		{			
			//20100701_tzy ��ѯ����ѯ�б����Ƴ�,�Ƿ�Ҫ��ȷ��Щԭ��Ӧ���Ƴ���ѯ�б���Щԭ���Ƴ���
			//��Ϊ�������ϲ�̫һ��
			//m_tTWMutiPollParam.m_tTWPollParam[byIdx].RemoveMtFromList(tMt);
			//ɾ�����ն�Ϊ��ǰ��ѯ�նˣ�����ͣһ�£��Ա��´���ѵʱ�ӵ�ǰλ�ÿ�ʼ
			if (byCurPollPos == byTmtPos)
			{
				m_tTWMutiPollParam.m_tTWPollParam[byIdx].SetIsStartAsPause(TRUE);
				//���������ѯ����״̬��������ִ�ж��¸��ն˵���ѯ
				if (m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetPollState() == POLL_STATE_NORMAL)
				{
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTwTimerIdx, 1000*1, dwTwTimerIdx);
				}
			}
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[RemoveJoinedMt]adjust TW and hdu poll param TW<EqpId:%d, ChnId:%d> Current PollChnState %d!\n",
					m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetTvWall().GetEqpId(),
					m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetTWChnnl(),m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetPollState());
		}		
	}

	// 7��ֹͣ���
	AdjustSwitchToAllMcWatchingSrcMt( tMt );
	AdjustSwitchToMonitorWatchingSrc( tMt );	//�������

	// 8��ֹͣ�ն�¼��
	RemoveMtFromRec(&tMt);
	MtStatusChange(&tMt, TRUE);
	return;
}

/*=============================================================================
�� �� ���� RemoveMtFormPeriInfo
��    �ܣ� ɾ���������ն�ʱ,�����������Ϣ
		   �ն��쳣����ʱ,�ᱣ��������Ϣ�ȴ����߻ָ�,
		   ���ɾ���쳣�����ն�ʱ,��Ҫ�����������Ϣ,��֤��Ϣ��ȷ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TMt tMt  Ҫɾ�����ն�
�� �� ֵ�� void
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
20120712    4.7		  yanghuaizhi              create
=============================================================================*/
void CMcuVcInst::RemoveUnjoinedMtFormPeriInfo( const TMt &tMt)
{
	// �ն�Ϊ��
	if (tMt.IsNull() || !tMt.IsLocal())
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[RemoveUnjoinedMtFormPeriInfo]Mt(%d,%d) is none or not local!\n",
			tMt.GetMcuId(), tMt.GetMtId());
		return;
	}

	// �ն�����
	if (m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[RemoveUnjoinedMtFormPeriInfo]Mt(%d,%d) is Joined Conf!\n",
			tMt.GetMcuId(), tMt.GetMtId());
		return;
	}
	
	// ���ն���vmp��,���vmp��Ա,ֻ��Ի��ָ��ģʽ
	// �ն��쳣����ʱ,�ᱣ��vmpͨ����Ա,���ն�����ʱ��ָ�,�˴�Ϊ��Դ˳������vmp��Ա
	// ɾ�������ߵ��ն�,��ʱ���ն�δ����vmp�Ľ���,ֻ��Ҫ��vmpparam�ĳ�Ա��Ϣ�������
	// ���������һ���ն�,�������Ҫͣvmp
	if (GetVmpCountInVmpList() > 0)
	{
		TEqp tVmpEqp;
		BOOL32 bChangeNotify;
		TVMPParam_25Mem tVMPParam;
		TVMPMember* ptVmpMember;
		u8 byMaxNum;
		TMt tMtNull;
		tMtNull.SetNull();
		TVMPParam_25Mem tLastVmpParam;
		TPeriEqpStatus tPeriEqpStatus;
		//u8 byVmpIdx;
		CServMsg cTempServ;
		for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
		{
			if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
			{
				continue;
			}
			tVmpEqp = g_cMcuVcApp.GetEqp( m_abyVmpEqpId[byIdx] );
			
			if( g_cMcuVcApp.GetVMPMode(tVmpEqp) == CONF_VMPMODE_CTRL)
			{
				bChangeNotify = FALSE;
				tVMPParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);//m_tConf.m_tStatus.GetVmpParam();
				ptVmpMember = NULL;
				byMaxNum = tVMPParam.GetMaxMemberNum();
				
				tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
				for (u8 byIndex = 0; byIndex < byMaxNum ; ++byIndex)
				{
					ptVmpMember = tVMPParam.GetVmpMember(byIndex);
					if (ptVmpMember == NULL || ptVmpMember->IsNull())
					{
						continue;
					}
					
					if (*ptVmpMember == tMt)
					{
						bChangeNotify = TRUE;
						// ֻ�л��ָ����vmpmember ����clear�������緢���˸���ȱ���type
						if ( VMP_MEMBERTYPE_MCSSPEC == ptVmpMember->GetMemberType() )
						{
							tVMPParam.ClearVmpMember(byIndex);
						}
						else
						{
							ptVmpMember->SetMemberTMt(tMtNull);
						}
						tLastVmpParam.SetVmpMember(byIndex, *ptVmpMember);
					}
				}
				g_cMcuVcApp.SetLastVmpParam(tVmpEqp, tLastVmpParam);
				
				if (TRUE == bChangeNotify &&
					g_cMcuVcApp.GetPeriEqpStatus(tVmpEqp.GetEqpId(), &tPeriEqpStatus))
				{
					//m_tConf.m_tStatus.SetVmpParam( tVMPParam );
					g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tVMPParam);
					/*if (tVMPParam.GetVMPMemberNum() == 0
						&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_DSTREAM)
						&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_VMPCHLPOLL)
						&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
						&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER))
					{
						//ֹͣvmp,�����ͳһ������ֹͣvmp
						if (!tVmpEqp.IsNull() &&
							tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
							tPeriEqpStatus.m_tStatus.tVmp.m_byUseState != TVmpStatus::WAIT_STOP)
						{
							byVmpIdx = tVmpEqp.GetEqpId() - VMPID_MIN;
							SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER+byVmpIdx, TIMESPACE_WAIT_VMPRSP);
							tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
							g_cMcuVcApp.SetPeriEqpStatus(tVmpEqp.GetEqpId() , &tPeriEqpStatus);	
							
							SendMsgToEqp(tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cTempServ); 
						}
					}
					else*/
					{
						tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVMPParam);
						g_cMcuVcApp.SetPeriEqpStatus(tVmpEqp.GetEqpId() , &tPeriEqpStatus);
						// ˢ��״̬
						ConfStatusChange();
					}
				}
			}
		}
	}

	//ɾ�����ڻ���ģ���е���ǽ��Ա����Ϣ
	RemoveMtInMutiTvWallMoudle(tMt);
	return;
}

/*=============================================================================
    �� �� ���� ProcVcsApplyCancelSpeakerReq
    ��    �ܣ� VCS����MTC-BOX����������/ȡ����Ϣ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� void
 -----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2010/05/13  4.6			�ű���                ����
=============================================================================*/
void CMcuVcInst::ProcVcsApplyCancelSpeakerReq(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	TMt tMt;
	TMsgHeadMsg tHeadMsg;
	BOOL32 bIsApplySpeaker = FALSE;
	TMt tNormalMt;
	
	if( MT_MCU_APPLYCANCELSPEAKER_REQ == cServMsg.GetEventId() )
	{
		//SendReplyBack(cServMsg, pcMsg->event + 1);
		
		memset( &tHeadMsg,0,sizeof(tHeadMsg) );
		
		tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
		bIsApplySpeaker = (*(u8*)cServMsg.GetMsgBody()) == 1;
	}
	else if( MCU_MCU_CANCELMESPEAKER_REQ == cServMsg.GetEventId() )
	{
		tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
		tMt = *(TMt*)( cServMsg.GetMsgBody()+sizeof(TMsgHeadMsg) );
		tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tMt );
	}
	else if (pcMsg->event == MCU_MCU_APPLYSPEAKER_REQ)
    {
		tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();	
        tMt = *(TMt*)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
		tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tMt );
		bIsApplySpeaker = TRUE;
    }
	
	if( VCS_CONF != m_tConf.GetConfSource() ||
		( !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
		m_byCreateBy != CONF_CREATE_MT )
		)
	{
		//zjj20101126 ��������ģʽ��,�������ն��Ƿ���״̬,�����ն˶��������뷢��,��Ҫ��ȷ�ؾܾ�(��������ƻ���)
		//�෽�໭���ڵ���ǽ���ն˱��ڻ���ϳ�����MTC-BOX���Ե���,���Բ��ش���
		if( VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() )
		{
			if( !(tMt == m_cVCSConfStatus.GetCurVCMT()) &&
				( m_ptMtTable->IsMtInHdu(tMt.GetMtId()) || m_ptMtTable->IsMtInTvWall(tMt.GetMtId()) )
				)
			{
				NotifyMtSpeakStatus( tMt, emDenid );
			}
		}
		return;
	}
	
	if ( !m_tCascadeMMCU.IsNull() )
    {        
		if( bIsApplySpeaker )
		{
			if( MT_MCU_APPLYCANCELSPEAKER_REQ == cServMsg.GetEventId() )
			{
				cServMsg.SetEventId( MCU_MCU_APPLYSPEAKER_REQ );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				tMt = GetMcuIdMtFromMcuIdxMt( tMt );
				cServMsg.CatMsgBody( (u8*)&tMt,sizeof(tMt) );
			}
			SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_APPLYSPEAKER_REQ, cServMsg);	
		}
		else
		{			
			if( MT_MCU_APPLYCANCELSPEAKER_REQ == cServMsg.GetEventId() )
			{
				cServMsg.SetEventId( MCU_MCU_CANCELMESPEAKER_REQ );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				tMt = GetMcuIdMtFromMcuIdxMt( tMt );
				cServMsg.CatMsgBody( (u8*)&tMt,sizeof(tMt) );
			}			
			SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_CANCELMESPEAKER_REQ, cServMsg);
		}
        return;
    }
	
    //1.ȡ������
    if( bIsApplySpeaker )
	{
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt ) )
		{
			if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
			{
				memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
				tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );
				cServMsg.SetEventId(pcMsg->event + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
			}
			
			return;
		}
		
		BOOL32 bIsSendToChairman = FALSE;
		if (CONF_SPEAKMODE_ANSWERINSTANTLY == m_tConf.GetConfSpeakMode())
		{
			if( m_tConf.GetSpeaker() == tMt )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ProcVcsApplyCancelSpeakerReq] tMt(%d.%d) has already speaker,so not handle!\n",
					tMt.GetMcuId(),tMt.GetMtId()
					);					
				return;
			}
			//�����ǰ����������ϯ,��������
			if ( m_tConf.GetChairman() == m_tConf.GetSpeaker() )
			{
				if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
				{
					memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
					tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
					cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
					cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );						
				}						
// 				cServMsg.SetEventId(pcMsg->event + 1);							
// 				SendReplyBack(cServMsg, cServMsg.GetEventId());
				
				ChgCurVCMT( tMt );

                if ( !(m_tConf.GetSpeaker() == tMt) )
                {//����ģʽ���뷢��ʧ��
                    cServMsg.SetEventId(pcMsg->event + 2);							
				    SendReplyBack(cServMsg, cServMsg.GetEventId());
                    if (tMt.IsLocal())
                    {
                        NotifyMtSpeakStatus(tMt, emDenid);
                    }
                }
                else
                {//����ģʽ���뷢�Գɹ�
                    cServMsg.SetEventId(pcMsg->event + 1);							
				    SendReplyBack(cServMsg, cServMsg.GetEventId());
                    if (tMt.IsLocal())
                    {
                        NotifyMtSpeakStatus(tMt, emAgreed);
                    }
                }
				
				//֪ͨ���
				//cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));						
				//SendMsgToAllMcs( MCU_MCS_MTAPPLYSPEAKER_NOTIF, cServMsg );						
			}
			else
			{
				if (!m_tApplySpeakQue.IsQueueFull())
				{
					if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
					{
						memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
						tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
						cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
						cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );							
					}
					cServMsg.SetEventId(pcMsg->event + 1);
					SendReplyBack(cServMsg, cServMsg.GetEventId());
					
					m_tApplySpeakQue.ProcQueueInfo(tMt,bIsSendToChairman);
					NotifyMcsApplyList( bIsSendToChairman );
				}
				else
				{
					if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
					{
						memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
						tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
						cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
						cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );							
					}
					cServMsg.SetEventId(pcMsg->event + 2);
					SendReplyBack(cServMsg, cServMsg.GetEventId());
					
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcVcsApplyCancelSpeakerReq] over max speak queue num. ignore it!\n");
				}
			}
		}
		else
		{
			if (!m_tApplySpeakQue.IsQueueFull())
			{
				if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
				{
					memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
					tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
					cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
					cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );						
				}
				cServMsg.SetEventId(pcMsg->event + 1);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
                
				m_tApplySpeakQue.ProcQueueInfo(tMt,bIsSendToChairman);
				NotifyMcsApplyList( bIsSendToChairman );
			}
			else
			{
				if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
				{
					memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
					tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
					cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
					cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );						
				}
				cServMsg.SetEventId(pcMsg->event + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
                
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcVcsApplyCancelSpeakerReq] over max speak queue num. ignore it!\n");
			}
		}
	}
		//ȡ������
	else
	{
		if( !(m_cVCSConfStatus.GetCurVCMT() == tMt) )
		{
			if( m_tCascadeMMCU.IsNull() )
			{
				if( MT_MCU_APPLYCANCELSPEAKER_REQ != cServMsg.GetEventId() )
				{						
					memset( &tHeadMsg,0,sizeof(tHeadMsg) );
					tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
					cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(tHeadMsg) );		//Ҫ�����Mt
					cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(tMt) );
				}					
				cServMsg.SetEventId( cServMsg.GetEventId() + 2 );
				SendReplyBack( cServMsg,cServMsg.GetEventId() );
			}
			return;
		}
		TMt tOrgMt = tMt;
		if( CONF_SPEAKMODE_ANSWERINSTANTLY != m_tConf.GetConfSpeakMode() ||
			m_tApplySpeakQue.IsQueueNull() )
		{
			tMt.SetNull();
			ChgCurVCMT( tMt );				
		}
		else
		{
			if(  m_tApplySpeakQue.GetQueueHead( tMt ) )
			{
				if( !tMt.IsNull() )
				{						
					ChgCurVCMT( tMt );													
				}
				else
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcVcsApplyCancelSpeakerReq] Queue Head is null!\n" );
				}
			}
			else
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcVcsApplyCancelSpeakerReq] Fail to Get Mt From Queue!\n" );
			}
		}	
		
		if( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcVcsApplyCancelSpeakerReq] Now Is GroupVmp Mode,Can't Cancel mt,so not reply msg to mcu.\n" );
			return;
		}
		
		if( MCU_MCU_CANCELMESPEAKER_REQ == cServMsg.GetEventId() )
		{				
			memset( &tHeadMsg,0,sizeof(tHeadMsg) );
			tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tOrgMt,tOrgMt );
			cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(tHeadMsg) );		//Ҫ�����Mt
			cServMsg.CatMsgBody( (u8 *)&tOrgMt, sizeof(tOrgMt) );				
		}
		cServMsg.SetEventId( cServMsg.GetEventId() + 1 );
		SendReplyBack( cServMsg,cServMsg.GetEventId() );
	}
	
	return;
}
/*=============================================================================
    �� �� ���� ProcMcsApplyCancelSpeakerReq
    ��    �ܣ� MCS����MTC-BOX����������/ȡ����Ϣ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� void
 -----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2011/03/28  4.6			��ʤ��                ����
=============================================================================*/
void CMcuVcInst::ProcMcsApplyCancelSpeakerReq(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	TMsgHeadMsg tHeadMsg;
	TMt tMt;
	
	BOOL32 bIsApplySpeaker = FALSE;//TRUE:���뷢��, FALSE:ȡ������/����
	TMt tNormalMt;

	if( MT_MCU_APPLYCANCELSPEAKER_REQ == cServMsg.GetEventId() )
	{
		//SendReplyBack(cServMsg, pcMsg->event + 1);
		
		memset( &tHeadMsg,0,sizeof(tHeadMsg) );
		
		tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
		bIsApplySpeaker = (*(u8*)cServMsg.GetMsgBody()) == 1;
	}
	else if( MCU_MCU_CANCELMESPEAKER_REQ == cServMsg.GetEventId() )
	{
		tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
		tMt = *(TMt*)( cServMsg.GetMsgBody()+sizeof(TMsgHeadMsg) );
		tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tMt );
		bIsApplySpeaker = FALSE;
	}
	else if (pcMsg->event == MCU_MCU_APPLYSPEAKER_REQ)
    {
		tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();	
        tMt = *(TMt*)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
		tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tMt );
		bIsApplySpeaker = TRUE;
    }

	//�����ϼ�MCU����,�������Ϸ�
	if ( !m_tCascadeMMCU.IsNull() )
    {        
		if( bIsApplySpeaker )
		{
			if( MT_MCU_APPLYCANCELSPEAKER_REQ == cServMsg.GetEventId() )
			{
				cServMsg.SetEventId( MCU_MCU_APPLYSPEAKER_REQ );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				tMt = GetMcuIdMtFromMcuIdxMt( tMt );
				cServMsg.CatMsgBody( (u8*)&tMt,sizeof(tMt) );
			}
			SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_APPLYSPEAKER_REQ, cServMsg);	
		}
		else
		{			
			if( MT_MCU_APPLYCANCELSPEAKER_REQ == cServMsg.GetEventId() )
			{
				cServMsg.SetEventId( MCU_MCU_CANCELMESPEAKER_REQ );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				tMt = GetMcuIdMtFromMcuIdxMt( tMt );
				cServMsg.CatMsgBody( (u8*)&tMt,sizeof(tMt) );
			}			
			SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_CANCELMESPEAKER_REQ, cServMsg);
		}
        return;
    }
	
	BOOL32 bIsSendToChairman = FALSE;
	
	if (bIsApplySpeaker)
	{//���뷢��

		if(!m_tConfAllMtInfo.MtJoinedConf( tMt ))
		{
			if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
			{
				memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
				tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );
				cServMsg.SetEventId(pcMsg->event + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
			}
			return;
		}

		if (m_tConf.GetSpeaker() == tMt)
		{//��ǰ���ڷ���

			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMcsApplyCancelSpeakerReq]MT%d is speaker\n", tMt.GetMtId());
			return;
		}
		
		if (m_tApplySpeakQue.IsMtInQueue(tMt))
		{//������ڶ���������ΪҪȡ������

// 			m_tApplySpeakQue.ProcQueueInfo(tMt, bIsSendToChairman, FALSE, TRUE);
// 			NotifyMtSpeakStatus( tMt, emDenid );
//             NotifyMcsApplyList( bIsSendToChairman );

			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMcsApplyCancelSpeakerReq]MT%d has been in the ApplySpeak Queue!\n", tMt.GetMtId());
			return;
		}

		if (!m_tApplySpeakQue.IsQueueFull())
		{
			if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
			{
				memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
				tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );						
			}
			cServMsg.SetEventId(pcMsg->event + 1);
			SendReplyBack(cServMsg, cServMsg.GetEventId());
			
			m_tApplySpeakQue.ProcQueueInfo(tMt,bIsSendToChairman);
			NotifyMcsApplyList( bIsSendToChairman );
		}
		else
		{
			if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
			{
				memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
				tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );						
			}
			cServMsg.SetEventId(pcMsg->event + 2);
			SendReplyBack(cServMsg, cServMsg.GetEventId());
			
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMcsApplyCancelSpeakerReq] over max speak queue num. ignore it!\n");
		}					
	}
	else
	{//ȡ������

		if (m_tApplySpeakQue.IsMtInQueue(tMt))
		{
			m_tApplySpeakQue.ProcQueueInfo(tMt, bIsSendToChairman, FALSE, TRUE);
			NotifyMcsApplyList( bIsSendToChairman );
			NotifyMtSpeakStatus(tMt, emCanceled);
		}

		if ((m_tConf.GetSpeaker() == tMt))
		{
            //���������²���ȡ��������
            if (m_tConf.m_tStatus.IsVACing())
            {
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMcsApplyCancelSpeakerReq]MT%d can't cancel speak in Vac\n", tMt.GetMtId());
            }
            else
            {                
                tMt.SetNull();
			    ChangeSpeaker(&tMt);
            }			
		}
	}

	return;
}
/*=============================================================================
    �� �� ���� ProcMtMcuApplyCancelSpeakerReq
    ��    �ܣ� MTͨ��MTC-BOX����������/ȡ����Ϣ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� void
 -----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2011/03/28  4.6			��ʤ��                ����
=============================================================================*/
void CMcuVcInst::ProcMtMcuApplyCancelSpeakerReq(const CMessage * pcMsg)
{
	STATECHECK
		
	if(VCS_CONF == m_tConf.GetConfSource())
	{//VCS���鴦��
		ProcVcsApplyCancelSpeakerReq(pcMsg);
	}
	else
	{//MCS���鴦��
		ProcMcsApplyCancelSpeakerReq(pcMsg);
	}
	return;
}

/*==============================================================================
������    :  ShowMcOtherTable
����      :  ��ӡ�����ն���Ϣ
�㷨ʵ��  :  
����˵��  :  
			 
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2011-7-27     4.6          �ܼ���						   ����
==============================================================================*/
void CMcuVcInst::ShowMcOtherTable()
{
	if (NULL == m_ptConfOtherMcTable)
	{
		return;
	}
	TConfMcInfo *pConfMcInfo = NULL;
	for(u16 wIdx = 0; wIdx < MAXNUM_CONFSUB_MCU; wIdx++)
	{
		pConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wIdx);
		if (NULL == pConfMcInfo)
		{
			continue;
		}
		StaticLog("McuIdx.%d Info as follows\n", wIdx);
		StaticLog("\t\t AudioInfo [MixerCount.%d, Speaker(McuId.%d, MtId.%d)]\n",
						pConfMcInfo->m_tConfAudioInfo.m_byMixerCount,
						pConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMcuId(),
						pConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId());

		StaticLog("\t\t SpyMt(McuId.%d, MtId.%d), MMcuViewMt(McuId.%d, MtId.%d), LastMMcuViewMt(McuId.%d, MtId.%d)\n\n",
						pConfMcInfo->m_tSpyMt.GetMcuId(),
						pConfMcInfo->m_tSpyMt.GetMtId(),
						pConfMcInfo->m_tMMcuViewMt.GetMcuId(), 
						pConfMcInfo->m_tMMcuViewMt.GetMtId(),
						pConfMcInfo->m_tLastMMcuViewMt.GetMcuId(),
						pConfMcInfo->m_tLastMMcuViewMt.GetMtId());

	}
	return;
}

/*==============================================================================
������    :  IsCanGetSpecCapByMtIp
����      :  �ж��ܲ���ȡ��ָ��IP��������������ɹ�������pwCapSet��
�㷨ʵ��  :  
ע���    :  (1) ����mcudebug.iniȡ��,��Ҫ�ֶ��޸�
			 (2) ��������ɵ����߱�֤�ռ�
����˵��  :  s8* pIpAddr : [in]  ƥ���IP��ַ
			 u32 adwCapSet[] : [out] ���ȡ�õ�����������
			 BOOL32 bIsGetMainCapSet: [in] ȡ���ǲ�����������,Ĭ��TRUE

����ֵ˵��:  �ɹ�������TRUE
			 ʧ�ܣ�����FALSE
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
11-08-10     1.0           �����						   ����
==============================================================================*/
BOOL32 CMcuVcInst::IsCanGetSpecCapByMtIp(const s8* pIpAddr, u32 adwCapSet[] , BOOL32 bIsGetMainCapSet /* = TRUE */)
{
	// 0. �������
	if ( !pIpAddr)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[IsCanGetSpecCapByMtIp]Error: Invalid Parameter<pIpAddr>!\n");
		return FALSE;
	}

	// 1. ��[mcuCheatIp]ȡ��Ӧ��IP
	s8 achMcuCfgFile[64];
	memset(achMcuCfgFile, 0, sizeof(achMcuCfgFile));
	sprintf(achMcuCfgFile, "%s/%s", DIR_CONFIG, MCUDEBUGFILENAME);

	s32 nIPCnt = 0;						// ��¼IP����
	GetRegKeyInt(achMcuCfgFile, "mcuCheatIp", "EntryNum", 0, &nIPCnt);

	if ( 0 == nIPCnt)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[IsCanGetSpecCapByMtIp]Error: No Match <IP> in section<[mcuCheatIp]>!\n");
		return FALSE;
	}

	s8 achKeyName[6];					// ��ʱ�����Entry255, ��Cap255
	memset(achKeyName, 0, sizeof(achKeyName));
	strncpy(achKeyName, "Entry", 5);	// �ؼ��ֵ����ݶ��� Entry + XXX, XXX��ʾ���

	u16 wLen = strlen(pIpAddr);			// ����IP��ַ�ĳ���,���ڱȽ�
	u8 byCapSetIdx = 255;				// ���������������к�,��EntryXXX,��ֵΪ���,��ֹȡ����Ҳ�ǶԵ�

	for (u8 byLoop = 0; byLoop < nIPCnt; byLoop++)
	{
		achKeyName[5] = '0'+ byLoop;
		s8 achContent[24];				// ������󳤶�: 15λIP + tab + 3 + tab + 3 + \0
		memset(achContent, 0, sizeof(achContent));

		GetRegKeyString(achMcuCfgFile, "mcuCheatIp", achKeyName, 0, achContent, sizeof(achContent));
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[IsCanGetSpecCapByMtIp]get content(%s) of key(%s)\n", achContent, achKeyName);

		if (0 == strncmp(achContent, pIpAddr, wLen))
		{
			if ( bIsGetMainCapSet)
			{
				byCapSetIdx = achContent[wLen+1];
			}
			else
			{
				byCapSetIdx = achContent[wLen+3];
			}
			break;
		}
	}
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[IsCanGetSpecCapByMtIp]byCapSetIdx is (%d)\n", byCapSetIdx);

	// 2. ȡ������[�����ɵ�������������]
	if ( 'N' == byCapSetIdx || 255 == byCapSetIdx)
	{
		// ������Ը���ʽ��,���������ʽ,���鲻Ҫ��'N'
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[IsCanGetSpecCapByMtIp]Warning: No Need To Cheat<MainCapSet>!\n");
		return FALSE;
	}

	// 3. ��[NonKedaCap]ȡ��Ӧ������������Ҫѭ��
	// "CapXXX �� 106	3	25	768"����ʽ
	strncpy(achKeyName, "Cap", 3);
	achKeyName[3] = byCapSetIdx;
	achKeyName[4] = '\0';
	s8 achStore[20];		// ÿһ��������ʱ��Ϊ20���ַ�		
	memset(achStore, 0, sizeof(achStore));
	GetRegKeyString(achMcuCfgFile, "NonKedaCap", achKeyName, 0, achStore, sizeof(achStore)-1);

	s8 *pCur = &achStore[0];
	s8 *pStart = &achStore[0];
	s32 nStoreLen = strlen(achStore);	// ������U���͵�
	u8 byCapIdx = 0;
	s8 tmp[16]={0};			// ��ʱ����,�������tab��tab֮����ַ�,�˴����������ļ���ȡֵ,���ΪIP�ĳ���,���Զ�Ϊ15+1
	while ( nStoreLen-- > 0)
	{
		if ( '\t' != *pCur)
		{
			if ( nStoreLen == 0)
			{
				// �Ѿ������һ��
				memset(tmp, 0, sizeof(tmp));
				memcpy(tmp, pStart, strlen(pStart));
				adwCapSet[byCapIdx++] = strtoul(tmp, 0, 10);
			}
		}
		else
		{
			if ( pCur == pStart || (size_t(pCur-pStart) >= sizeof(tmp)) )
			{
				// 1.��ֹ����ֻ��һ��tab�����<ʵ����֤���ֻ��һ��tab�����,ת��10���ƾ���0>
				// 2.��ֹ2��ָ��֮��Ĳ�ֵ������ʱ������Դ�ŵĿռ�(����\0)
				break;
			}
			memset(tmp, 0, sizeof(tmp));
			memcpy(tmp, pStart, pCur-pStart);
			adwCapSet[byCapIdx++] = atoi(pStart);
			pStart = pCur+1; // ����++,�и�����
		}
		pCur++;
	}

	// ������ݶ�ûȡ�������
	if ( byCapIdx == 0)
	{
		return FALSE;
	}
	else 
	{
		return TRUE;
	}
}

/*=============================================================================
�� �� ���� ProcMtMcuTransParentMsgNotify
��    �ܣ� 
�㷨ʵ�֣� ���桢�ն���Ϣ͸��
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg 
�� �� ֵ�� void
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2/23/2012   4.6			��ʤ��                ����
=============================================================================*/
void CMcuVcInst::ProcMtMcuTransParentMsgNotify( const CMessage * pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    u8 byMtId = cServMsg.GetSrcMtId();
    TMt tMt = m_ptMtTable->GetMt( byMtId );
    
    CServMsg cMsgToMcs;
    cMsgToMcs.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
    cMsgToMcs.CatMsgBody( (u8*)cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
    cMsgToMcs.SetEventId(MCU_MCS_TRANSPARENTMSG_NOTIFY);
    SendMsgToAllMcs(MCU_MCS_TRANSPARENTMSG_NOTIFY, cMsgToMcs);
	
    return;    
}

/*=============================================================================
�� �� ���� ConstructDSCapExFor8KH8KE
��    �ܣ� ��ͬ����Ƶ��H264˫����8000H��8000G������˫����ѡ������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const TCapSupport & tCapsupport ����������
		   TVideoStreamCap *pTDSCapEx ˫��������ָ��
�� �� ֵ�� void
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
4/14/2012   4.7		    �´�ΰ               ����
=============================================================================*/
void CMcuVcInst::ConstructDSCapExFor8KH8KE(const TCapSupport & tCapSupport, TVideoStreamCap *pTDSCapEx)
{
	if(pTDSCapEx == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ConstructDSCapExFor8KH8KE]pTDSCapEx == NULL, error!");

		return;
	}
#if defined(_8KH_)	
	// ֻ֧��ת�� [4/14/2012 chendaiwei]
	if( (tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 && tCapSupport.GetDStreamResolution() == VIDEO_FORMAT_UXGA &&
		( tCapSupport.GetDStreamUsrDefFPS() == 15 ||
		  tCapSupport.GetDStreamUsrDefFPS() == 20 ||
		  tCapSupport.GetDStreamUsrDefFPS() == 30 ||
		  tCapSupport.GetDStreamUsrDefFPS() == 60 ) ) 
		||( tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 && tCapSupport.GetDStreamResolution() == VIDEO_FORMAT_SXGA &&
		   ( tCapSupport.GetDStreamUsrDefFPS() == 30 ||
		     tCapSupport.GetDStreamUsrDefFPS() == 60) )
		)
	{
		return;
	}
#endif


	
	BOOL32 bIgnoreDSBpCapEx = TRUE; //8000H����HP��BP��ѡ����£�˫����BP��ѡ[8/24/2012 chendaiwei]

#if defined(_8KH_) || defined(_8KI_)
	if( IsConfHasBpCapOrBpExCap(m_tConf,m_tConfEx) )
	{
		bIgnoreDSBpCapEx = FALSE;
	}
//	if( tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 )
//	{
//		if( tCapSupport.GetMainStreamProfileAttrb() == emHpAttrb )
//		{
//			TVideoStreamCap atMStreamCapEx[MAX_CONF_CAP_EX_NUM];
//			u8 byCapExNum = MAX_CONF_CAP_EX_NUM;
//			m_tConfEx.GetMainStreamCapEx(atMStreamCapEx,byCapExNum);
//
//			for( u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
//			{
//				if( atMStreamCapEx[byIdx].GetMediaType() == MEDIA_TYPE_H264 
//					&& atMStreamCapEx[byIdx].GetH264ProfileAttrb() == emBpAttrb)
//				{
//					bIgnoreDSBpCapEx = FALSE;
//					break;
//				}
//			}
//		}
//		else
//		{
//			bIgnoreDSBpCapEx = FALSE;
//		}
//	}
#endif

		BOOL32 bIsCanReserveXga = TRUE;

#ifdef _8KI_
		if( Is8KINeedDsAdaptH264SXGA(m_tConf) && IsConfDoubleDual(m_tConf) )
		{
			bIsCanReserveXga = FALSE;
		}
#endif

	// 8000H/8000G��ͬ����ƵH264˫�����카ѡ [4/13/2012 chendaiwei]
	if( tCapSupport.GetDStreamMediaType() == MEDIA_TYPE_H264 
		&& tCapSupport.GetDStreamType() != VIDEO_DSTREAM_MAIN 
		&& tCapSupport.GetDStreamType()!=VIDEO_DSTREAM_MAIN_H239 )
	{
		
		u8 byCount = 0;
		// 8KH HP���ֱ��ʹ�ѡ����BP [4/14/2012 chendaiwei]
#if defined(_8KH_) || defined(_8KI_)
		if(tCapSupport.GetMainStreamProfileAttrb() == emHpAttrb && !bIgnoreDSBpCapEx)
		{
			TVideoStreamCap tTempCapBp(MEDIA_TYPE_H264,tCapSupport.GetDStreamResolution(),tCapSupport.GetDStreamUsrDefFPS(),emBpAttrb,0);
			pTDSCapEx[byCount] = tTempCapBp;
			byCount ++;
		}
#endif

		u8 abyRes[] = { VIDEO_FORMAT_UXGA, VIDEO_FORMAT_SXGA, VIDEO_FORMAT_XGA};
		u8 byResNum = sizeof(abyRes)/sizeof(abyRes[0]);

		if( !bIsCanReserveXga )
		{
			abyRes[2] = VIDEO_FORMAT_INVALID;
		}
		
		//������Ƶ�����ֱ���
		u8 byLocalResPos;
		for (byLocalResPos = 0; byLocalResPos < byResNum; byLocalResPos++)
		{			

			if (tCapSupport.GetDStreamResolution() == abyRes[byLocalResPos])
			{
				break;
			}
			else
			{
				if ( byLocalResPos == byResNum - 1 )
				{					
					LogPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ConstructDSCapExFor8KH8KE]not find Res of h264 video cap of ds cap !");

					return;
				}
				else
				{
					continue;
				}				
			}
		}
		
		byLocalResPos ++; //��˫�����������빴ѡ

		for(; byLocalResPos < byResNum; byLocalResPos++ )
		{

			if( VIDEO_FORMAT_INVALID == abyRes[byLocalResPos] )
			{
				continue;
			}
			
#if defined(_8KH_) || defined(_8KI_)
			if(tCapSupport.GetMainStreamProfileAttrb() == emHpAttrb)
			{
				TVideoStreamCap tTempCapHp(MEDIA_TYPE_H264,abyRes[byLocalResPos],tCapSupport.GetDStreamUsrDefFPS(),emHpAttrb,0);
				pTDSCapEx[byCount] = tTempCapHp;
				byCount ++;
			}

			if( !bIgnoreDSBpCapEx )
			{
				TVideoStreamCap tTempCapBp(MEDIA_TYPE_H264,abyRes[byLocalResPos],tCapSupport.GetDStreamUsrDefFPS(),emBpAttrb,0);
				pTDSCapEx[byCount] = tTempCapBp;
				byCount ++;	
			}
#else
			TVideoStreamCap tTempCapBp(MEDIA_TYPE_H264,abyRes[byLocalResPos],tCapSupport.GetDStreamUsrDefFPS(),emBpAttrb,0);
			pTDSCapEx[byCount] = tTempCapBp;
			byCount ++;	
#endif //8000G
		}
#if defined(_8KI_)
		if( Is8KINeedDsAdaptH264SXGA(m_tConf) &&  tCapSupport.GetDStreamUsrDefFPS() > 20 )
		{
			if(tCapSupport.GetMainStreamProfileAttrb() == emHpAttrb) 
			{
				TVideoStreamCap tTempCapHp(MEDIA_TYPE_H264,VIDEO_FORMAT_SXGA,20,emHpAttrb,0);
				pTDSCapEx[byCount] = tTempCapHp;
				byCount++;
			}

			if( !bIgnoreDSBpCapEx )
			{
				TVideoStreamCap tTempCapBp(MEDIA_TYPE_H264,VIDEO_FORMAT_SXGA,20,emBpAttrb,0);
				pTDSCapEx[byCount] = tTempCapBp;
				byCount ++;	
			}
		}
		if( bIsCanReserveXga && tCapSupport.GetDStreamUsrDefFPS() > 5 )
		{
			if(tCapSupport.GetMainStreamProfileAttrb() == emHpAttrb) 
			{
				TVideoStreamCap tTempCapHp(MEDIA_TYPE_H264,VIDEO_FORMAT_XGA,5,emHpAttrb,0);
				pTDSCapEx[byCount] = tTempCapHp;
				byCount++;
			}

			if( !bIgnoreDSBpCapEx )
			{
				TVideoStreamCap tTempCapBp(MEDIA_TYPE_H264,VIDEO_FORMAT_XGA,5,emBpAttrb,0);
				pTDSCapEx[byCount] = tTempCapBp;
				byCount ++;	
			}
		}
		
#endif
	}

	return;
}
/*
 *	ͨ��MTID�ж�MT��IP����
 */
BOOL32 CMcuVcInst::IsMtIpV6(u8 byMtId )
{
	if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return FALSE;
	}

// 	u32 dwMtIp = 0;
// 	u16 wPort = 0;
// 	m_ptMtTable->GetMtSwitchAddr(byMtId, dwMtIp, wPort);

	return g_cMcuVcApp.IsMtIpV6(/*dwMtIp*/m_ptMtTable->GetIPAddr(byMtId));
}

//END OF FILE
