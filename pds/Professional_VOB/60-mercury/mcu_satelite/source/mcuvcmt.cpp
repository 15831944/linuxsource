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
#include "mcuerrcode.h"
#include "mpmanager.h"

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
u8 CMcuVcInst::AddMt( TMtAlias &tMtAlias, u16 wMtDialRate, u8 byCallMode, 
                      BOOL bPassiveAdd )
{
	u8 byMtId;
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
    
	if( mtAliasTypeTransportAddress == tMtAlias.m_AliasType || 
		mtAliasTypeE164 == tMtAlias.m_AliasType || 
		mtAliasTypeH323ID == tMtAlias.m_AliasType ||
		// PU��������
		puAliasTypeIPPlusAlias == tMtAlias.m_AliasType)
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
                ConfLog(FALSE, "[AddMt] Mt.%d existed, replace.\n", byMtId);
                return byMtId;
		    }
        }

		// ԭ�������ڵ��ն�,����һ���µĸ���		
		byMtId = m_ptMtTable->AddMt( &tMtAlias, bRepeat );
		if(byMtId > 0)
		{
			m_ptMtTable->SetDialAlias( byMtId, &tMtAlias );
			m_ptMtTable->SetDialBitrate( byMtId, wMtDialRate );
			m_ptMtTable->SetMcuId( byMtId, LOCAL_MCUID);
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
                ConfLog(FALSE, "[AddMt] Mt.%d existed, replace.\n", byMtId);
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
			m_ptMtTable->SetMcuId( byMtId, LOCAL_MCUID);
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
            
            RemoveJoinedMt( tMt, TRUE );
        }
        if (!g_cMcuVcApp.IsMpConnected(byMpId))
        {
            ConfLog(FALSE, "[AddJoinedMt] byMpId.%d is OFFline for MT.%d\n", byMpId, tMt.GetMtId());
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
        ConfLog( FALSE, "[AddJoinedMt] AssignMpForMt.%d failed !\n", tMt.GetMtId() );
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

	bHasMsgMcsAlias = m_tConfAllMtInfo.MtInConf( tMt.GetMtId() ); 

	//��������ն��б�
	m_tConfAllMtInfo.AddJoinedMt( tMt.GetMtId() );

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
            MtStatusChange( tMt.GetMtId(), TRUE );
		}

	}

	//֪ͨ���
//	cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
//	SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
	
	//������ػ��������ն���Ϣ
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//��������ն˱�
	SendMtListToMcs(LOCAL_MCUID);
	
	if( !bHasMsgMcsAlias )
	{		
		SendMtAliasToMcs( tMt );
	}

	//��Trap��Ϣ
	TMtNotify tMtNotify;
	CConfId   cConfId;
	TMtAlias  tMtAlias;
	u8 byLen = 0;
	cConfId = m_tConf.GetConfId( );
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
    if ( tMt.GetMtType() == MT_TYPE_MT )
    {
        CServMsg cMsg;
        cMsg.SetEventId(MCU_MT_GETMTVERID_REQ);
        SendMsgToMt(tMt.GetMtId(), MCU_MT_GETMTVERID_REQ, cMsg);        
    }

#ifdef _SATELITE_
    RefreshConfState();
#endif


	//��ӡ��Ϣ
	CallLog( "Mt%d: 0x%x(Dri:%d) joined conf success!\n", 
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
void CMcuVcInst::RemoveMt( TMt &tMt, BOOL32 bMsg )
{
	CServMsg	cServMsg;
	BOOL32      bNeedNotifyMt = FALSE;
		
	cServMsg.SetConfId( m_tConf.GetConfId() );
	
    tMt = m_ptMtTable->GetMt(tMt.GetMtId());

	m_ptMtTable->SetNotInvited( tMt.GetMtId(), FALSE );
	RemoveJoinedMt( tMt, bMsg );

	
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
	TConfMtInfo *pcInfo = m_tConfAllMtInfo.GetMtInfoPtr(tMt.GetMtId());
	if(pcInfo != NULL)
	{
		pcInfo->SetNull();
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
	CallLog( "Mt%d: 0x%x(Dri:%d) removed from conf!\n", 
			       tMt.GetMtId(), m_ptMtTable->GetIPAddr(tMt.GetMtId()), tMt.GetDriId() );

	//�ӻ����ն˱���ɾ��
	m_ptMtTable->DelMt( tMt.GetMtId() );

	NotifyMcuDelMt(tMt);

	//������ػ��������ն���Ϣ
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//��������ն˱�
//	SendMtListToMcs(LOCAL_MCUID);

	//������ػ�����Ϣ��
	cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
	SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

    // zbq [03/23/2007] �������ѯ�б�����նˣ���֪ͨ���ˢ��ѯ�б�����Լ�ά����
    u8 byMtPos;
    if ( m_tConfPollParam.IsExistMt(tMt, byMtPos) )
    {
        m_tConfPollParam.RemoveMtFromList(tMt);
		m_tConfPollParam.SpecPollPos(byMtPos);
    }

	//���������ն�,����VCS�����иû���Ϊ�¼����飬�ϼ�����������¼�����ͬ������
	if( (m_tConfAllMtInfo.GetLocalMtNum() == 0 && m_tConf.GetConfAttrb().IsReleaseNoMt()) ||
		(MT_TYPE_MMCU == tMt.GetMtType() && CONF_CREATE_MT == m_byCreateBy && VCS_CONF == m_tConf.GetConfSource()))
	{
		ConfLog( FALSE, "There's no MT or vcs mmcuconf is over in conference %s now. Release it!\n", 
				 m_tConf.GetConfName() );
		ReleaseConf( TRUE );
		NEXTSTATE( STATE_IDLE );
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
	u16 wMinBitRate;
	TSimCapSet tSrcSimCapSet;
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
    if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE 
		&& m_tConf.m_tStatus.IsBrdstVMP()
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
    else
    {
        u16 wAdaptBitRate = 0;      
           
        //˫��ʽ���������������ն˽���������
        if (IsMtNeedAdapt(ADAPT_TYPE_VID, tMt.GetMtId())  &&
            (IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate) || IsNeedCifAdp()))
        {
            if (wMinBitRate != m_wVidBasBitrate)
            {
                if (m_tConf.m_tStatus.IsVidAdapting())
                {
                    Mt2Log("[ProcMtMcuFlowControlCmd1] change vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wVidBasBitrate);
                    ChangeAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }                
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
                    Mt2Log("[ProcMtMcuFlowControlCmd3]change br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wBasBitrate);
                    ChangeAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
                else
                {
                    Mt2Log("[ProcMtMcuFlowControlCmd3]start br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wBasBitrate);
                    StartAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }                   
                
                StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
            else if (m_tConf.m_tStatus.IsBrAdapting())//������Ҫ�����������ʣ�ֱ�ӽ�����������������mt(�������������ն˵Ľ������ʸ��͵����)
            {
                Mt2Log("[ProcMtMcuFlowControlCmd1] switch br bas to mt<%d>\n", tMt.GetMtId());
                StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
        }         
        //˫��ʽ�������������ն˽�����ƵԴ���ʣ����ܻὫ����ѹ�����
        else if (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() || IsNeedCifAdp())
        {
            AdjustMtVideoSrcBR(tMt.GetMtId(), wMinBitRate, byMode);
        }
    }	

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
====================================================================*/
void CMcuVcInst::RemoveJoinedMt( TMt & tMt, BOOL32 bMsg, u8 byReason,BOOL32 bNeglectMtType )
{
	CServMsg	cServMsg;
    cServMsg.SetConfId( m_tConf.GetConfId() );
    cServMsg.SetConfIdx( m_byConfIdx );

	u8			byMode;

    // xsl [11/10/2006] �ͷŽ����ת����Դ
    ReleaseMtRes( tMt.GetMtId(), bNeglectMtType);

	//�Ƿ����
	if (!m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
	{
		//֪ͨ
		if (bMsg)
		{
			SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
		}
		return;
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

	// xsl [10/26/2006] �Ƴ������ش��նˣ�ֹͣʱ�迼��vmp������㲥Դ�����
	if (m_tConf.m_tStatus.IsPrsing())
	{
        if (tMt == m_tAudBrdSrc)
        {
            StopPrs(PRSCHANMODE_AUDIO);
        }
        else
        {
            AddRemovePrsMember(tMt, FALSE, PRSCHANMODE_AUDIO);
        }

        if (tMt == m_tVidBrdSrc)
        {
            StopPrs(PRSCHANMODE_FIRST);
        }
        else
        {
            AddRemovePrsMember(tMt, FALSE, PRSCHANMODE_FIRST);
        }

		if (tMt == m_tDoubleStreamSrc)
		{
			StopPrs(PRSCHANMODE_SECOND);
		}
        else
        {
            AddRemovePrsMember(tMt, FALSE, PRSCHANMODE_SECOND);
        }	
	}
	
	//�������һ̨�����ն�,ֹͣ���� //modify bas 2 -- ������Ҫ�����Ƿ�ֹͣ����
//	if( m_tConf.m_tStatus.IsMdtpAdapting() && 
//		IsMtNeedAdapt( tMt.GetMtId(), ADAPT_TYPE_MDTP ) && 
//		1 == GetNeedAdaptMtNum( ADAPT_TYPE_MDTP ) )
//	{
//		StopBasAdapt( ADAPT_TYPE_MDTP );	
//	}
//
//	if( m_tConf.m_tStatus.IsBrAdapting() && 
//		IsMtNeedAdapt( tMt.GetMtId(), ADAPT_TYPE_BR ) && 
//		1 == GetNeedAdaptMtNum( ADAPT_TYPE_BR ) )
//	{
//		StopBasAdapt( ADAPT_TYPE_BR );	
//	}		

	//ֹͣ����
	if (m_tConf.m_tStatus.IsMixing() || m_tConf.m_tStatus.IsVACing())
	{
        // xsl [8/4/2006] ���ƻ�����Ա����
        if (m_tConf.m_tStatus.IsSpecMixing())
        {
            if (m_ptMtTable->IsMtInMixing(tMt.GetMtId()))
            {
                AddRemoveSpecMixMember(&tMt, 1, FALSE);
            }            
        }
        else //vac�������Ƴ�ͨ��
        {            
            //�ڻ������ڴ˲������
            if (m_ptMtTable->IsMtInMixGrp(tMt.GetMtId()))
            {               
                StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
			                (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())), FALSE, MODE_AUDIO);
            }   
            RemoveMixMember(&tMt, FALSE); 
        }                    
	}

	//ֹͣVMP
	if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
	{
        BOOL32 bStopVmp = TRUE;
		BOOL32 bChangeNotify = FALSE;
		TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpParam();
		for( u8 byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++  )
		{
			TMt tMembMt = *(TMt*)tVMPParam.GetVmpMember(byLoop);
			TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMembMt );
			if( tLocalMt.GetMtId() == tMt.GetMtId() )
			{
                TVMPMember tVmpMember = *(TVMPMember*)tVMPParam.GetVmpMember(byLoop);

                //����ϳɳ�Ա����Ϊ�����ָ����ʱ����״̬����ɾ���䱾����������
                //�ϳɳ�Ա�����ڴ˲���������ֹ���������صĸ���ͨ����[03/21/2006-zbq]
                if ( VMP_MEMBERTYPE_MCSSPEC == tVmpMember.GetMemberType() )
                {
                    StopSwitchToPeriEqp( m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO );
                    if( !IsDynamicVmp() )
                    {
                        TVMPMember tVMPMember;
                        memset( &tVMPMember, 0, sizeof(TVMPMember) );
                        tVMPParam.SetVmpMember( byLoop, tVMPMember );
                        bChangeNotify = TRUE;
                    }
                }

                // zbq [04/25/2007] ������ն��ڻ���ģ���VMP��Ϣ�ֱ��ɾ��֮
                for( u8 byIndex = 0; byIndex < MAXNUM_MPUSVMP_MEMBER; byIndex ++ )
                {
                    // �����Ǽ�������ϳɵ����
                    if ( TYPE_MT == tMt.GetMtType() &&
                        m_tConfEqpModule.m_atVmpMember[byIndex].GetMtId() == tMt.GetMtId() ) 
                    {
                        m_tConfEqpModule.m_atVmpMember[byIndex].SetNull();
                    }
                }
			}

            TVMPMember tVmpMember = *tVMPParam.GetVmpMember(byLoop);
            if (tVmpMember.GetMemberType() != 0 || !tVmpMember.IsNull())
            {
                bStopVmp = FALSE;
            }
		}
		m_tConf.m_tStatus.SetVmpParam( tVMPParam );
        
		//��̬�����ı���
		ChangeVmpStyle( tMt, FALSE );
		
		if( TRUE == bChangeNotify )
		{
			TPeriEqpStatus tVmpEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpEqpStatus);

            if (bStopVmp)
            {
                //ֹͣ��������
			    ChangeVmpSwitch( 0, VMP_STATE_STOP );

                //ֹͣ
				TPeriEqpStatus tPeriEqpStatus;
				if (!m_tVmpEqp.IsNull() &&
					g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tPeriEqpStatus) &&
					tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
					tPeriEqpStatus.m_tStatus.tVmp.m_byUseState != TVmpStatus::WAIT_STOP)
				{
					SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, 6*1000);
					tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
					g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);	
				
					CServMsg cTempServ;
					SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cTempServ); 
				}
            }
            else
            {
                //ChangeVmpParam(&tVMPParam);
				// xliang [1/6/2009] ��������VMP��VMP param
				AdjustVmpParam(&tVMPParam);
            }

			tVmpEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpEqpStatus);

//			//֪ͨ��ϯ�����л��
//			cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam) );
//			SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
//			if( HasJoinedChairman() )
//			{
//				SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_VMPPARAM_NOTIF, cServMsg );
//			}
//
		}

        ConfStatusChange();
	}
    
    //  xsl [2/10/2006] ͣ�໭�����ǽ
    if(m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE)
    {
        BOOL32 bStopVmpTw = TRUE;
        BOOL32 bChangeNotify = FALSE;
        TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpTwParam();
        for( u8 byLoop = 0; byLoop < MAXNUM_SDVMP_MEMBER; byLoop++  )
        {
            TMt tMembMt = *(TMt*)tVMPParam.GetVmpMember(byLoop);
            TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMembMt );
            if( tLocalMt.GetMtId() == tMt.GetMtId() )
            {
                TVMPMember tVmpMember = *tVMPParam.GetVmpMember(byLoop);

                //����ϳɳ�Ա����Ϊ�����ָ����ʱ����״̬����ɾ���䱾����������
                //�ϳɳ�Ա�����ڴ˲���������ֹ���������صĸ���ͨ����[12/04/2006-zbq]
                if ( VMPTW_MEMBERTYPE_MCSSPEC == tVmpMember.GetMemberType() ) 
                {
                    StopSwitchToPeriEqp( m_tVmpTwEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO );
                    
                    TVMPMember tVMPMember;
                    memset( &tVMPMember, 0, sizeof(TVMPMember) );
                    tVMPParam.SetVmpMember( byLoop, tVMPMember );
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
            
            tVmpEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVmpTwParam;
            g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tVmpEqpStatus);
            
            if (bStopVmpTw)
            {
                //ֹͣ��������
                ChangeVmpTwSwitch( 0, VMPTW_STATE_STOP );
                
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

    //ֹͣTvWall
    u8 byEqpId = TVWALLID_MIN;
	TPeriEqpStatus tTvwallStatus;
    while( byEqpId >= TVWALLID_MIN && byEqpId <= TVWALLID_MAX )
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byEqpId))
		{
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
					if (tMt.GetMtId() == byTmpMtId 
						&& m_byConfIdx == byMtConfIdx
						&& TW_MEMBERTYPE_POLL != tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType
						)
					{
						StopSwitchToPeriEqp(byEqpId, byLoop);
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
                        
                        //���ն�״̬
                        m_ptMtTable->SetMtInTvWall(byTmpMtId, FALSE);
					}
					else if ( TW_MEMBERTYPE_POLL == tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType )
					{
						TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
						
						TMtPollParam *ptCurPollMt = NULL;
						u8 byPollIdx = m_tConfPollParam.GetCurrentIdx();
						ptCurPollMt = GetNextMtPolled(byPollIdx, tPollInfo);
						ChangeTvWallSwitch( ptCurPollMt, byEqpId, byLoop, TW_MEMBERTYPE_POLL, TW_STATE_CHANGE );
						
        
                        //���ն�״̬
                        m_ptMtTable->SetMtInHdu(byTmpMtId, FALSE);
					}


                    // zbq [04/25/2007] ���TvWallģ����Ӧ��Ա��Ϣ
                    for(u8 byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++)
                    {
                        u8 byTvId = 0;
                        m_tConfEqpModule.m_tTvWallInfo[byLoop].RemoveMtByMtIdx(byTmpMtId, byTvId);
                        if( 0 != byTvId )
                        {
                            m_tConfEqpModule.m_tMultiTvWallModule.RemoveMtByTvId(byTvId, byTmpMtId);
                        }
                    }                    
				}
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tTvwallStatus);

                cServMsg.SetMsgBody((u8 *)&tTvwallStatus, sizeof(tTvwallStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
            }
		}
        byEqpId++;
	}

    //ֹͣhdu
    u8 byHduId = HDUID_MIN;
	TPeriEqpStatus tHduStatus;
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
					if (tMt.GetMtId() == byTmpMtId 
						&& m_byConfIdx == byMtConfIdx 
						&&  TW_MEMBERTYPE_POLL != tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType
						&&  TW_MEMBERTYPE_SPEAKER != tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType
						&&  TW_MEMBERTYPE_CHAIRMAN != tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType
						&&  POLL_STATE_NONE == m_tHduBatchPollInfo.GetStatus()
						)
					{
						StopSwitchToPeriEqp(byHduId, byLoop);
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = 0;
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
                        
                        //���ն�״̬
                        m_ptMtTable->SetMtInHdu(byTmpMtId, FALSE);
					}
					else if ( TW_MEMBERTYPE_POLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType )
					{
						TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
						
						TMtPollParam *ptCurPollMt = NULL;
						u8 byPollIdx = m_tConfPollParam.GetCurrentIdx();
		                ptCurPollMt = GetNextMtPolled(byPollIdx, tPollInfo);
						ChangeHduSwitch( ptCurPollMt, byHduId, byLoop, TW_MEMBERTYPE_POLL, TW_STATE_CHANGE );
                        
                        //���ն�״̬
                        m_ptMtTable->SetMtInHdu(byTmpMtId, FALSE);
					}
					else if ( TW_MEMBERTYPE_SPEAKER == tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType )
					{ 
                        TMt tLocalSpeaker = m_tConf.GetSpeaker();
						tLocalSpeaker = GetLocalMtFromOtherMcuMt( tLocalSpeaker );
						ChangeHduSwitch( &tLocalSpeaker, byHduId, byLoop, TW_MEMBERTYPE_SPEAKER, TW_STATE_CHANGE );
					}
					else if ( TW_MEMBERTYPE_CHAIRMAN == tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType )
					{
                        TMt tLocalSpeaker = m_tConf.GetChairman();
						tLocalSpeaker = GetLocalMtFromOtherMcuMt( tLocalSpeaker );
						ChangeHduSwitch( &tLocalSpeaker, byHduId, byLoop, TW_MEMBERTYPE_SPEAKER, TW_STATE_CHANGE );
					}
					else
					{
						// do nothing
					}
                   
				}
                g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);
				
                cServMsg.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
            }
		}
        byHduId++;
	}	

	//����ǽ��ѯ��������
	if ( POLL_STATE_NONE != m_tConf.m_tStatus.GethduPollState() )
	{
		ConfLog(FALSE, "[RemoveJoinedMt] adjust hdu poll param!\n");
		m_tHduPollParam.RemoveMtFromList( tMt);
	}
      
	//ֹͣ�ն�¼��
	if( !m_ptMtTable->IsMtNoRecording( tMt.GetMtId() ) )
	{
		TEqp tEqp ;
		u8   byRecChnl;
		m_ptMtTable->GetMtRecordInfo( tMt.GetMtId() ,&tEqp ,&byRecChnl);
		StopSwitchToPeriEqp( tEqp.GetEqpId() ,byRecChnl);
		m_ptMtTable->SetMtNoRecording( tMt.GetMtId() );
		
		cServMsg.SetMsgBody( ( u8 * )&tEqp, sizeof( tEqp) );	//set TEqp
		cServMsg.SetChnIndex( byRecChnl );
		SendMsgToEqp( tEqp.GetEqpId(), MCU_REC_STOPREC_REQ, cServMsg );
	}
		
	//�뿪���ն��Ƿ�����
	if( GetLocalSpeaker() == tMt )
	{
		//����ģʽ��ֻ��VIDEO���ֽ��д���
		byMode =  m_tConf.m_tStatus.IsMixing() ? MODE_VIDEO : MODE_BOTH;

		g_cMpManager.RemoveSwitchBridge( tMt, 0, byMode);
		
		if( !m_tConf.m_tStatus.IsNoRecording() )
		{
			StopSwitchToPeriEqp( m_tRecEqp.GetEqpId(), m_byRecChnnl, FALSE );
		}

        //modify bas 2 -- ��֤����ֹͣ�����Ƿ���࣬changvidbrdsrc �Ƿ����
        if (m_tConf.m_tStatus.IsAudAdapting())
        {
            StopSwitchToPeriEqp(m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl, FALSE, byMode);
        }
        if (m_tConf.m_tStatus.IsVidAdapting())
        {
            StopSwitchToPeriEqp(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, FALSE, byMode);
        }
        if (m_tConf.m_tStatus.IsBrAdapting())
        {
            StopSwitchToPeriEqp(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, FALSE, byMode);
        }
	}

    ClearH239TokenOwnerInfo( &tMt );


    //ͣ���ڶ�·��Ƶ����ͨ��
    if(m_tDoubleStreamSrc == tMt)
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
    if (!m_tDoubleStreamSrc.IsNull() && !m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        // ���� [6/1/2006] ���������ǻ����ʱ��ͣ�ڶ�·
        g_cMpManager.StopSwitchToSubMt(tMt, MODE_SECVIDEO, TRUE);
    }

	//�ж��Ƿ�����ϯ�ն�
	if( tMt == m_tConf.GetChairman() )
	{
		ChangeChairman( NULL );
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
            m_tConf.m_tStatus.GetMtPollParam().GetMcuId() == tMt.GetMtId() ) )
        {
            bIsMtInPolling = TRUE;
        }
    }    

	//�ж��Ƿ�Ϊ�����ն�
	if( tMt == GetLocalSpeaker() )
	{
		ChangeSpeaker( NULL,  bIsMtInPolling);

		if( MT_TYPE_MMCU == tMt.GetMtType() )
		{
			TMt tNullMt;
			tNullMt.SetNull();
			m_tConf.SetSpeaker( tNullMt );
		}
		else
		{
			m_tConf.SetSpeaker( tMt );
		}
	}
    // zbq [03/12/2007] ��������ڱ���ѯ���նˣ�ֱ��������һ����ѯ�ն�
    if (bIsMtInPolling)
    {
        SetTimer( MCUVC_POLLING_CHANGE_TIMER, 10 );
    }

	//ֹͣ����̨�ն˽���
    if (!m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        StopSwitchToSubMt( tMt.GetMtId(), MODE_BOTH );
    }	
    else
    {
        // ���ǻ�������ʾ�ն˿��Լ�
        NotifyMtReceive( tMt, tMt.GetMtId() );
    }
	
	//�ж��Ƿ�Ϊ�ش�ͨ��,��������ش�����
	if( !m_tCascadeMMCU.IsNull() && !(m_tCascadeMMCU == tMt) )
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
		if( ptConfMcInfo != NULL && 
			( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
		{
			if( ptConfMcInfo->m_tSpyMt.GetMtId() == tMt.GetMtId() &&
				ptConfMcInfo->m_tSpyMt.GetMcuId() == tMt.GetMcuId() )
			{
				u8 byMode = MODE_BOTH;
				if (m_tConf.m_tStatus.IsMixing())
				{
					byMode = MODE_VIDEO;
				}					
				StopSwitchToSubMt( m_tCascadeMMCU.GetMtId(), byMode );
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
		g_cMpManager.StopMulticast( tMt, 0, MODE_BOTH );
		m_ptMtTable->SetMtMulticasting( tMt.GetMtId(), FALSE );
	}
	

	//��Trap��Ϣ
	TMtNotify tMtNotify;
	CConfId cConfId;
	TMtAlias tMtAlias;
	u8 byLen = 0;
	cConfId = m_tConf.GetConfId( );
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
	
	// �Ҷϻ�ɾ���¼�MCU����¼��ն˴���ѯ�б���ɾ��, MCS�Լ�ά���б�����֪ͨ, zgc, 2007-03-29
	if( MT_TYPE_SMCU == tMt.GetMtType() )
	{
		if( POLL_STATE_NONE != m_tConf.m_tStatus.GetPollState() )
		{
			TMtPollParam atMtPollNew[MAXNUM_CONF_MT];
            TMtPollParam tTmpMt;
			u8 byPos;
            u8 byNewNum= 0;
			BOOL32 bPollingMtInSMcu = ( m_tConf.m_tStatus.GetMtPollParam().GetMcuId() == tMt.GetMtId() ) ? TRUE : FALSE ;
			u8 byNextPos = bPollingMtInSMcu ? 0xFF : 0;     // ��һ��Ҫ��ѯ���ն�
			for( byPos = 0; byPos < m_tConfPollParam.GetPolledMtNum(); byPos++ )
			{
                tTmpMt = *(m_tConfPollParam.GetPollMtByIdx(byPos));
				if( tMt.GetMtId() != tTmpMt.GetMcuId() )
				{
                    atMtPollNew[byNewNum] = tTmpMt;
					if( byPos > m_tConfPollParam.GetCurrentIdx() && byNextPos == 0xFF )
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
				if( byNextPos == 0xFF )
				{
					byNextPos = m_tConfPollParam.GetPolledMtNum();
				}
				m_tConfPollParam.SpecPollPos( byNextPos );
				SetTimer( MCUVC_POLLING_CHANGE_TIMER, 10 );
			}
		}
	}
    
	//zbq [12/05/2007] ��ѡ��
    StopSelectSrc(tMt, MODE_BOTH, FALSE);
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
    if ( ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
    {
        if ( tMt == m_tRollCaller )
        {
            NotifyMcsAlarmInfo( 0, ERR_MCU_ROLLCALL_CALLERLEFT );
            RollCallStop(cServMsg);
        }
        else if ( tMt == GetLocalMtFromOtherMcuMt(m_tRollCallee) )
        {
            NotifyMcsAlarmInfo( 0, ERR_MCU_ROLLCALL_CALLEELEFT );
            RollCallStop(cServMsg);
        }
    }

	TConfMtInfo *pcInfo = m_tConfAllMtInfo.GetMtInfoPtr(tMt.GetMtId());
	if(pcInfo != NULL)
	{
		pcInfo->SetNull();
	}

	NotifyMcuDropMt(tMt);
	
	//���˼���mcu��Ϣ�Ƴ����б�
	m_ptConfOtherMcTable->RemoveMcInfo( tMt.GetMtId() );
	m_tConfAllMtInfo.RemoveMcuInfo( tMt.GetMtId(), tMt.GetConfIdx() );

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
        //�ͷŶ�Ӧ����������Դ
        StopHDCascaseAdp();

		m_tCascadeMMCU.SetNull();
		m_tConfAllMtInfo.m_tMMCU.SetNull();
		m_tConfInStatus.SetNtfMtStatus2MMcu(FALSE);
	}

    // ֪ͨ�ն�����ԭ��
	// fxh ��RestoreAllSubMtJoinedConfWatchingSrcMtǰ������ȷ��VCS����״̬,�Ƿ��е�ǰ�����ն�
    MtOnlineChange( tMt, FALSE, byReason );	
	//�Ƴ�
	m_tConfAllMtInfo.RemoveJoinedMt( tMt.GetMtId() );
	
	RestoreAllSubMtJoinedConfWatchingSrcMt( tMt );

	StopSwitchToAllMcWatchingSrcMt( tMt );
	StopSwitchToAllPeriEqpWatchingSrcMt( tMt );


	//����߼�ͨ��
	m_ptMtTable->ClearMtLogicChnnl( tMt.GetMtId() );
    m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_NONE );
    //��״̬
    TMtStatus tMtStatus;
    if ( m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus) )
    {
        tMtStatus.Clear();
		tMtStatus.SetMtId(tMt.GetMtId());
		tMtStatus.SetMcuId(tMt.GetMcuId());
        m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tMtStatus);
		MtStatusChange(tMt.GetMtId(), TRUE);
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
	SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
	
	//������ػ��������ն���Ϣ
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
	
	//֪ͨ�����ն�
	cServMsg.SetMsgBody( ( u8 * )&tMt, sizeof( tMt ) );	
	BroadcastToAllSubMtJoinedConf( MCU_MT_MTLEFT_NOTIF, cServMsg );

    // xsl [10/11/2006] �ͷŶ˿�
	g_cMcuVcApp.ReleaseMtPort( m_byConfIdx, tMt.GetMtId() );
    m_ptMtTable->ClearMtSwitchAddr( tMt.GetMtId() );

    // zbq [02/18/2008] �����ն���ᣬ�ָ�VMP�㲥������
    if ( m_tConf.m_tStatus.IsBrdstVMP() &&
         m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
         m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()) )
    {
        AdjustVmpBrdBitRate();
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
        m_cMtRcvGrp.RemoveMem(tMt.GetMtId(), tMainCap);
        m_cMtRcvGrp.RemoveMem(tMt.GetMtId(), tSecCap);
        m_cMtRcvGrp.RemoveMem(tMt.GetMtId(), tDCap);
    }

#ifdef _SATELITE_
    RefreshConfState();
#endif

	//��ӡ��Ϣ
	CallLog( "Mt%d: 0x%x(Dri:%d) droped from conf for the reason.%d!\n", 
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
        ptMtStatus->SetVideoFreeze(tMtOldStatus.IsVideoFreeze());
        ptMtStatus->SetSelByMcsDrag(tMtOldStatus.GetSelByMcsDragMode());
        ptMtStatus->m_tVideoMt  = tMtOldStatus.m_tVideoMt;
        ptMtStatus->m_tAudioMt  = tMtOldStatus.m_tAudioMt;
        ptMtStatus->m_tRecState = tMtOldStatus.m_tRecState;
        ptMtStatus->SetSendVideo(tMtOldStatus.IsSendVideo());

        // ����˫��״̬��ʶ [02/05/2007-zbq]
        ptMtStatus->SetSndVideo2( m_tDoubleStreamSrc.GetType() == ptMtStatus->GetType() && 
                                  m_tDoubleStreamSrc.GetMtId() == ptMtStatus->GetMtId() );

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
        MtStatusChange();

        break;

    default:
        ConfLog(FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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

#ifndef _SATELITE_
    TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
    TMtAlias tMtH323Alias = *(TMtAlias*)cServMsg.GetMsgBody();
    TMtAlias tMtAddr = *((TMtAlias*)cServMsg.GetMsgBody()+1);
    u8 byType = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMtAlias)*2 );
    u8 byEncrypt = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMtAlias)*2 +1);
#else
    TMt	tMt = *(TMt*)cServMsg.GetMsgBody();
    TTransportAddr tAddr = *(TTransportAddr*)(cServMsg.GetMsgBody() + sizeof(TMt));
    u8 byType = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TTransportAddr) );

    TMtAlias tMtH323Alias;
    TMtAlias tMtAddr;
    u8 byEncrypt = CONF_ENCRYPTMODE_NONE;
#endif

	if(byEncrypt == 0 && m_tConf.GetConfAttrb().GetEncryptMode() != CONF_ENCRYPTMODE_NONE)
	{
		//drop call
		cServMsg.SetMsgBody();
		SendMsgToMt( tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg );
		return;
	}
	//�����ն�����
	if(byType == TYPE_MCU )
	{
		m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_SMCU );
	}
	else
	{
		m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_MT); 
	}

	//�����ն˱���
	m_ptMtTable->SetMtAlias( tMt.GetMtId(), &tMtAddr );
	m_ptMtTable->SetMtAlias( tMt.GetMtId(), &tMtH323Alias);

	//�����ն�IP��ַ
	if( tMtAddr.m_AliasType == mtAliasTypeTransportAddress )
	{
		m_ptMtTable->SetIPAddr( tMt.GetMtId(), tMtAddr.m_tTransportAddr.GetIpAddr() );
	}

	//����ն�Ϊ��������
	//m_ptMtTable->SetMtCallingIn( tMt.GetMtId(), FALSE );

	SendMtAliasToMcs( tMt );
    
#ifdef _SATELITE_
    //���ǻ����ն�ע��ɹ��������´���
    
    //1��ģ���ն��ն����
    u8 byMaster = FALSE;    //�ݲ����Ǽ��������
    CServMsg cMsg;
    cMsg.SetSrcMtId(tMt.GetMtId());
    cMsg.SetMsgBody((u8*)&byMaster, sizeof(u8));
    OspPost( MAKEIID(GetAppID(), GetInsID()),
             MT_MCU_MTJOINCONF_NOTIF,
             cMsg.GetServMsg(), cMsg.GetServMsgLen() );

    //2�����ͻ������������������߼�ͨ��
    TCapSupport tCap = m_tConf.GetCapSupport();
	TCapSupportEx tCapEx = m_tConf.GetCapSupportEx();

    cMsg.SetMsgBody();
    cMsg.SetConfIdx( m_byConfIdx );
    cMsg.SetMsgBody( ( u8 * )&tMt, sizeof( tMt ) );
    cMsg.CatMsgBody( ( u8 * )&tCap, sizeof( TCapSupport ) );
	cMsg.CatMsgBody( ( u8 * )&tCapEx, sizeof( TCapSupportEx ) );
    SendMsgToMt( tMt.GetMtId(),
                 MCU_MT_CAPBILITYSET_NOTIF, cMsg );

	BOOL32 bMtHd = ::topoGetMtInfo( LOCAL_MCUID, tMt.GetMtId(), g_atMtTopo, g_wMtTopoNum ).IsMtHd();

	//������������
	if (!bMtHd &&
		MEDIA_TYPE_H264 == tCap.GetMainVideoType())
	{
		if (tCap.GetSecVideoType() != MEDIA_TYPE_NULL &&
			tCap.GetSecVideoType() != 0)
		{
			TSimCapSet tSimCap = tCap.GetSecondSimCapSet();
			tCap.SetMainSimCapSet(tSimCap);
			
			//ģ�ⵥ��Ƶ�������潻��
			u8 byMainAudType = tCap.GetMainAudioType();
			if (tCap.GetMainAudioType() == MEDIA_TYPE_NULL)
			{
				TSimCapSet tSimCapSet = tCap.GetMainSimCapSet();
				tSimCapSet.SetAudioMediaType(byMainAudType);
				tCap.SetMainSimCapSet(tSimCapSet);
			}
		}
	}


	//˫����������
	if (!bMtHd && tCapEx.IsDDStreamCap())
	{
		TDStreamCap tDStreamCap;
		tDStreamCap.SetMediaType(tCapEx.GetSecDSType());
		tDStreamCap.SetResolution(tCapEx.GetSecDSRes());
		tDStreamCap.SetFrameRate(tCapEx.GetSecDSFrmRate());
		tDStreamCap.SetMaxBitRate(tCapEx.GetSecDSBitRate());
		tDStreamCap.SetSupportH239(tCap.IsDStreamSupportH239());
		
		tCap.SetDStreamCapSet(tDStreamCap);
	}


    //3��ģ��д���ն�������
    m_ptMtTable->SetMtCapSupport(tMt.GetMtId(), &tCap);

    //4��֪ͨ�ն����, ����֪ͨ
    RefreshConfState();
	g_cMcuVcApp.SetConfRefreshTimes(m_byConfIdx, 3);
#endif
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

    CallLog("Mt.%d Invite Nack because of Reason \"%s\"(%d), ErrCode.%u.\n", tMt.GetMtId(), 
             GetMtLeftReasonStr(byReason), byReason, wErrorCode );
    
    CallFailureNotify(cServMsg);
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

		if( tMt.GetMcuId() != LOCAL_MCUID )
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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


	switch( CurState() )
	{
	case STATE_ONGOING:

	    SendReplyBack( cServMsg, pcMsg->event + 1 );

		//�õ�MCU
		tMcu = *(TMcu*)cServMsg.GetMsgBody();         

		//�����ն�״̬
		if( tMcu.GetMcuId() == 0 && m_ptConfOtherMcTable != NULL)
		{
            // �Ǳ���ֻ��һ������
            byAliasNum = 1;
            u8 byAliasType = mtAliasTypeH323ID;

			//�ȷ�����MC
			for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
			{                
				TConfMcInfo *ptConfMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
				if( ptConfMcInfo->m_byMcuId == 0 )
				{
					continue;
				}

                // McuID
                ptrBuf = abyBuf ;
                memcpy(ptrBuf, &(ptConfMcInfo->m_byMcuId), sizeof(u8));

                // ����MtNum
                ptrBuf += 2;
                wBufLen = 2;

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

			//���������
			tMcu.SetMcu( LOCAL_MCUID );
		}

		//���Ǳ�����MCU
		if( ! tMcu.IsLocal() && m_ptConfOtherMcTable != NULL)
		{
			TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMcu.GetMcuId());
			if( ptConfMcInfo == NULL )
			{
				return;
			}

            byAliasNum = 1;
            u8 byAliasType = mtAliasTypeH323ID;

            // McuID
            ptrBuf = abyBuf ;
            memcpy(ptrBuf, &ptConfMcInfo->m_byMcuId, sizeof(u8));

            // ����MtNum
            ptrBuf += 2;
            wBufLen = 2;

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
            abyBuf[0] = LOCAL_MCUID;

            // ����MtNum
            ptrBuf += 2;
            wBufLen = 2;

            byValidMtNum = 0;
            
            u8* ptrBufTmp = NULL;
            TMtAlias tMtAlias;
            u8 byAliasNum = 0;
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
            abyBuf[1] = byValidMtNum;
            cServMsg.SetMsgBody( abyBuf, wBufLen );
            SendReplyBack( cServMsg, MCU_MCS_ALLMTALIAS_NOTIF );
		}

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
        ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
                ConfLog( FALSE, "[GetMtExtInfo] Mt.%d verInfo unexist, nack\n", tMt.GetMtId() );
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
                TMt tMt = m_ptMtTable->GetMt(byMtId);
            
                if ( 0 == m_ptMtTable->GetHWVerID(tMt.GetMtId()) ||
                     NULL == m_ptMtTable->GetSWVerID(tMt.GetMtId()))
                {
                    ConfLog( FALSE, "[GetMtExtInfo] Mt.%d verInfo unexist, nack\n", tMt.GetMtId() );
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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

			TRY_MEM_COPY(pbyTmp, (void*)atMtAlias[i].m_achAlias, ntohs(aliasLen), pbyMemLimit,bResult);
			if( !bResult )
				return (pbyTmp - pbyBuf);
			pbyTmp += ntohs(aliasLen);
		}
		else if(type = mtAliasTypeTransportAddress)
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
	TMt			tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	TMt			tChairman;

	switch( CurState() )
	{
	case STATE_ONGOING:

		//δ���
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
		{
			return;
		}
				
		//����ϯ
		if( HasJoinedChairman() )
		{
			tChairman = m_tConf.GetChairman();
			
			//��ϯ���뷢��,ͬ��
			if( tChairman.GetMtId() == tMt.GetMtId() )
			{
                // xsl [8/22/2006]���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
                if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsOverSatCastChnnlNum(tMt.GetMtId()))
                {
                    ConfLog(FALSE, "[ProcMtMcuApplySpeakerNotif] over max upload mt num. ignore it!\n");
                }
                else
                {
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
                ConfLog(FALSE, "[ProcMtMcuApplySpeakerNotif] not permit send message to chair mt for mt apply speaker\n");
            }
		}        

		//֪ͨ���
		cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
		SendMsgToAllMcs( MCU_MCS_MTAPPLYSPEAKER_NOTIF, cServMsg );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

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
                ConfLog(FALSE, "[ProcMtMcuApplyChairmanReq] not permit send message to chair mt for mt apply chair\n");
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
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

    CallLog("[DaemonProcMtMcuApplyJoinReq] ConfNum.%d\n", byNum);

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
				g_cMcuVcApp.SendMsgToConf( ptConfInfo->GetConfId(), pcMsg->event, pcMsg->content, pcMsg->length );                
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

	// ׼�������б�
	cServMsg.SetMsgBody( );
    
	for (u8 byConfIdx1 = MIN_CONFIDX; byConfIdx1 <= MAX_CONFIDX; byConfIdx1++)
	{
		if (NULL != g_cMcuVcApp.GetConfInstHandle(byConfIdx1))
		{
			ptConfInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx1 )->m_tConf;
            
			if ( CONF_OPENMODE_CLOSED != ptConfInfo->GetConfAttrb().GetOpenMode() && // ���������ն����
                 ( byType == TYPE_MCU ||    //admin�û����Ļ������������������ն˺���
                   g_cMcuVcApp.IsMtIpInAllowSeg( ptConfInfo->GetUsrGrpId(), tMtAddr.m_tTransportAddr.GetNetSeqIpAddr() ) )  ) // �����ն��ڸ���������
			{
                nConfNum ++;

				strncpy( tConfList.achConfName, ptConfInfo->GetConfName(), sizeof( tConfList.achConfName ) );
				tConfList.achConfName[sizeof( tConfList.achConfName) - 1 ] = 0;					
				tConfList.m_cConfId = ptConfInfo->GetConfId();
				cServMsg.CatMsgBody( (u8*)&tConfList, sizeof( tConfList) );	

                CallLog("[DaemonProcMtMcuApplyJoinReq] conf list. Index.%d, confname.%s\n", 
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
        g_cMcuVcApp.SendMsgToConf( tConfList.m_cConfId, pcMsg->event, pcMsg->content, pcMsg->length );
        return;
    }
    else
    {
        // �����ն��б�
	    cServMsg.SetErrorCode( ERR_MCU_NULLCID );
	    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
        CallLog("[DaemonProcMtMcuApplyJoinReq] send conf list to mt. nConfNum.%d\n", nConfNum);
        return;
    }

    // ��ʼȱʡ����
    if (bStartDefConf)
    {
		CallLog("[DaemonProcMtMcuApplyJoinReq] start def conf!\n");
		TConfStore tConfStore;
		TConfStore tConfStoreBuf;
		TPackConfStore *ptPackConfStore = (TPackConfStore *)&tConfStoreBuf;
		//��ȡ��� tConfStore Ϊ TPackConfStore �ṹ�����ѽ���Pack����Ļ�������

		//����ȱʡ���鴴��
		if (!GetConfFromFile(MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE, ptPackConfStore))
		{
			cServMsg.SetErrorCode(ERR_MCU_NULLCID);
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

            ConfLog(FALSE, "Conference %s failure because Get Conf.%d From File failed!\n", 
                            tConfStore.m_tConfInfo.GetConfName(), MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE);
			return;
		}
		//�� �ѽ���Pack����Ļ������� ����UnPack����
		if (!UnPackConfStore(ptPackConfStore, tConfStore))
		{
			cServMsg.SetErrorCode(ERR_MCU_NULLCID);
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

            ConfLog(FALSE, "Conference %s failure because UnPackConf.%d From File failed!\n", 
                            tConfStore.m_tConfInfo.GetConfName(), MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE);
            return;
		}

		if (0 == byEncrypt && 
			CONF_ENCRYPTMODE_NONE != tConfStore.m_tConfInfo.GetConfAttrb().GetEncryptMode())
		{
			cServMsg.SetErrorCode(ERR_MCU_SETENCRYPT);
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

			ConfLog(FALSE, "Conference %s failure because encrypt setting is confused - byEncrypt.%d EncryptMode.%d!\n", 
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
			else 
			{
				if (mtAliasTypeE164 == tConfStore.m_atMtAlias[byLoop].m_AliasType)
				{
					//����뽨���ն�һ�£����ظ����
					if (0 == memcmp(tConfStore.m_atMtAlias[byLoop].m_achAlias, tMtE164Alias.m_achAlias, MAXLEN_ALIAS))
					{
						bRepeatThisLoop = TRUE;
						bInMtTable = TRUE;
						atMtAlias[0] = tMtE164Alias;
					}		
				} 
				else 
				{
					if (mtAliasTypeH323ID == tConfStore.m_atMtAlias[byLoop].m_AliasType)
					{
						//����뽨���ն�һ�£����ظ����
						if (0 == memcmp(tConfStore.m_atMtAlias[byLoop].m_achAlias, tMtH323Alias.m_achAlias, MAXLEN_ALIAS))
						{
							bRepeatThisLoop = TRUE;
							bInMtTable = TRUE;
							atMtAlias[0] = tMtH323Alias;
						}
					}
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
                                  tNewTvwallModule, tNewVmpModule);            
		}

		//�ն������� MAXNUM_CONF_MT����������ĩβһ���ն�
		if (byMtNumInUse > MAXNUM_CONF_MT)
		{
			byMtNumInUse = MAXNUM_CONF_MT;
		}

		//�ն˱��������� 
		s8  achAliasBuf[SERV_MSG_LEN];
		u16 wAliasBufLen = 0;
		PackTMtAliasArray(atMtAlias, awMtDialBitRate, byMtNumInUse, achAliasBuf, wAliasBufLen);
		wAliasBufLen = htons(wAliasBufLen);

		cServMsg.SetMsgBody((u8 *)&tConfStore.m_tConfInfo, sizeof(TConfInfo));
		cServMsg.CatMsgBody((u8 *)&wAliasBufLen, sizeof(wAliasBufLen));
		cServMsg.CatMsgBody((u8 *)achAliasBuf, ntohs(wAliasBufLen));
		if (tConfStore.m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
		{
			cServMsg.CatMsgBody((u8*)&tNewTvwallModule, sizeof(TMultiTvWallModule));
		}
		if (tConfStore.m_tConfInfo.GetConfAttrb().IsHasVmpModule())
		{
			cServMsg.CatMsgBody((u8*)&tNewVmpModule, sizeof(TVmpModule));
		}
		cServMsg.CatMsgBody((u8 *)&tMtH323Alias, sizeof(tMtH323Alias));
		cServMsg.CatMsgBody((u8 *)&tMtE164Alias, sizeof(tMtE164Alias));
		cServMsg.CatMsgBody((u8 *)&tMtAddr, sizeof(tMtAddr));
		cServMsg.CatMsgBody((u8 *)&byType, sizeof(byType));
		// xliang [12/26/2008] (modify for MT call MCU initially) add bInMtTalbe Info
		cServMsg.CatMsgBody((u8*)&bInMtTable,sizeof(bInMtTable));
		//print test
		CallLog("bInMtTable is %d\n",bInMtTable);

        u8 byInsID = AssignIdleConfInsID();
        if(0 != byInsID)
        {
            ::OspPost(MAKEIID(AID_MCU_VC, byInsID), MT_MCU_CREATECONF_REQ,
                      cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        else
        {
            ConfLog(FALSE, "[DaemonProcMtMcuApplyJoinReq] assign instance id failed!\n");
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
			
			CallLog( "Mt 0x%x join conf %s request was refused because encrypt!\n", 
				     tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
            return;
		}
        
        // byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAddr );

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
                     tMtE164Alias == tDialAlias &&
                     //��ʹ�к���E164����ƥ�䣬ҲҪУ��һ��Ip����ֹδע��GK(��ע���˲�ͬ�ķ��ھ�GK)���ն�ð������
                     m_ptMtTable->GetMtAlias(byMtIdx, mtAliasTypeTransportAddress, &tInConfMtAddr) &&
                     tInConfMtAddr.m_tTransportAddr.GetIpAddr() == tMtAddr.m_tTransportAddr.GetIpAddr())
                {
                    byMtId = byMtIdx;
                    tFstAlias = tMtE164Alias;
                    break;
                }
                else if ( mtAliasTypeH323ID == tDialAlias.m_AliasType &&
                          tMtH323Alias == tDialAlias &&
                          //��ʹ�к���E164����ƥ�䣬ҲҪУ��һ��Ip����ֹδע��GK(��ע���˲�ͬ�ķ��ھ�GK)���ն�ð������
                          m_ptMtTable->GetMtAlias(byMtIdx, mtAliasTypeTransportAddress, &tInConfMtAddr) &&
                          tInConfMtAddr.m_tTransportAddr.GetIpAddr() == tMtAddr.m_tTransportAddr.GetIpAddr())
                {
                    byMtId = byMtIdx;
                    tFstAlias = tMtH323Alias;
                    break;
                }
                else if( mtAliasTypeTransportAddress == tDialAlias.m_AliasType &&
                         tMtAddr.m_tTransportAddr.GetIpAddr() == tDialAlias.m_tTransportAddr.GetIpAddr() )
                {
                    byMtId = byMtIdx;
                    tFstAlias = tMtAddr;
                    break;
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

			    CallLog( "Mt 0x%x join conf %s request was refused because conf is closed!\n", 
					      tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
			    return;
		    }

            // xsl [8/2/2006] ����mcu��������
            if ( byType != TYPE_MCU )
            {
                //������û����������õ�ַ���ն˼���, �ܾ�
				// xliang [12/25/2008] ȡ���û�������
//                 if ( !g_cMcuVcApp.IsMtIpInAllowSeg( m_tConf.GetUsrGrpId(), tMtAddr.m_tTransportAddr.GetNetSeqIpAddr()) )
//                 {
//                     cServMsg.SetConfIdx( m_byConfIdx );
//                     cServMsg.SetConfId( m_tConf.GetConfId() );
//                     
//                     g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
//                     
//                     CallLog( "Mt 0x%x join conf %s request was refused because usr group %d not allowed!\n", 
//                               tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName(), m_tConf.GetUsrGrpId() );
//                     return;
//                 }
//                 else
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
   
		wCallRate -= GetAudioBitrate( m_tConf.GetMainAudioMediaType() );               
        
		//�����ն�ID��
		byMtId = AddMt( tFstAlias, wCallRate, CONF_CALLMODE_NONE, TRUE );

        // zbq [08/09/2007] ��������Ip�κ���,�����б����ֲ���Ip���ն�,�˴��豣����Ip
        if ( bMtCallingInWithSegIp && !(tFstAlias == tMtAddr) )
        {
            m_ptMtTable->SetMtAlias( byMtId, &tMtAddr );
            // guzh [10/29/2007] ����TMtExt �����IP��ַ
            m_ptMtTable->SetIPAddr( byMtId, tMtAddr.m_tTransportAddr.GetIpAddr() );
        }
        
        // xsl [11/8/2006] ������ն˼�����1
		// xliang [2/14/2009] ������MT����MCU,������
		u8 byDriId = cServMsg.GetSrcDriId();
        g_cMcuVcApp.IncMtAdpMtNum( cServMsg.GetSrcDriId(), m_byConfIdx, byMtId );
        m_ptMtTable->SetDriId(byMtId, byDriId);
		if(byType == TYPE_MCU)
		{
			//�������ն˼�����Ҫ+1
			//��mtadplib�Ƕ���ͨ���жϣ����Կ϶���ʣ����������ռ��
			g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum++;
		}
		
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
				m_ptMtTable->SetMtType( byMtId, MT_TYPE_MMCU );
			}
			else if(byType = TYPE_MT)
			{
				m_ptMtTable->SetMtType( byMtId, MT_TYPE_MT );
			}
			
			m_ptMtTable->SetMtAlias(byMtId, &tMtH323Alias);
			m_ptMtTable->SetMtAlias(byMtId, &tMtE164Alias);
			
			cServMsg.SetConfIdx( m_byConfIdx );
			cServMsg.SetConfId( m_tConf.GetConfId() );
			cServMsg.SetDstMtId( byMtId );
			cServMsg.SetMsgBody(&byEncrypt, sizeof(u8));
			TCapSupport tCap = m_tConf.GetCapSupport();
			cServMsg.CatMsgBody( (u8*)&tCap, sizeof(tCap));
			TMtAlias tAlias;
			tAlias.SetH323Alias(m_tConf.GetConfName());
			cServMsg.CatMsgBody((u8 *)&tAlias, sizeof(tAlias));
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+1, cServMsg);			
		}
		else
		{
			cServMsg.SetConfIdx( m_byConfIdx );
			cServMsg.SetConfId( m_tConf.GetConfId() );
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

			CallLog( "Mt 0x%x join conf %s request was refused because conf full!\n", 
					  tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
		}

		break;
    }
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
	TSwitchInfo	tSwitchInfo;
	TMt	tDstMt, tSrcMt;
	u8  byDstMtId;
	CServMsg  cMsg( pcMsg->content, pcMsg->length );
	TMtStatus tSrcMtStatus;
    TMtStatus tDstMtStatus;    

    tSwitchInfo = *( TSwitchInfo * )cServMsg.GetMsgBody();
    tSrcMt = tSwitchInfo.GetSrcMt();
    tDstMt = tSwitchInfo.GetDstMt();
	
	// xliang [4/2/2009] ȡ����ϯѡ��VMP��״̬
	if(tDstMt == m_tConf.GetChairman() 
		&& m_tConf.m_tStatus.IsVmpSeeByChairman() )
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

	//��ֹSendReplyBack����Ϣ����Ӧ�𣬽�Src��ΪNULL
	if( pcMsg->event == MT_MCU_STARTSELMT_CMD )
	{
		cServMsg.SetNoSrc();
        cServMsg.SetSrcMtId(0);
	}

	//������ǻ��ѡ���նˣ�ǿ�ƹ㲥�µķ���ϯѡ����NACK
	if( pcMsg->event != MCS_MCU_STARTSWITCHMC_REQ && 
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
        ConfLog(FALSE, "Mt(%d,%d) Select see Mt(%d,%d), returned with ack!\n", 
                tDstMt.GetMcuId(), 
                tDstMt.GetMtId(),
                tSrcMt.GetMcuId(), 
                tSrcMt.GetMtId());

        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
        return;
    }

	if( tDstMt.GetType() == TYPE_MT)
	{
		ConfLog( FALSE, "Mt(%d,%d) select see Mt(%d,%d)\n", tDstMt.GetMcuId(), tDstMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId() );

		tDstMt = GetLocalMtFromOtherMcuMt( tDstMt );				
		tDstMt = m_ptMtTable->GetMt(tDstMt.GetMtId());
		tSwitchInfo.SetDstMt( tDstMt );		
	}

    TMt tSrcMtSMcu = tSrcMt;    // ��¼�±�׼��ѡ��Դ���������¼��ն�
	tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);
	tSwitchInfo.SetSrcMt( tSrcMt );
	byDstMtId = tDstMt.GetMtId();

    //Դ�ն�δ����飬NACK
    if( !m_tConfAllMtInfo.MtJoinedConf( tSrcMt.GetMtId() ) &&
	    m_tConf.GetConfSource() != VCS_CONF)
    {
        ConfLog( FALSE, "Select source Mt(%u,%u) has not joined current conference!\n",
                 tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
        cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }    
		
    //�ն�״̬��ѡ��ģʽ��ƥ��, NACK
    // zgc, 2008-07-10, �޸��жϷ�ʽ, �����չ��˽��ΪMODE_NONEʱ���ܾ�ѡ�������������ʾ
	u8 bySwitchMode = tSwitchInfo.GetMode();	// ��¼��ʼMODE

	m_ptMtTable->GetMtStatus(tSrcMt.GetMtId(), &tSrcMtStatus);
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
        // �����ѡ��ģʽ��ȫ���ܾ�����NACK
        cServMsg.SetErrorCode( ERR_MCU_SRCISRECVONLY );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
	}


    //zbq[05/22/2009] �ն�ѡ������֧�� FIXME: ��δ�����������Ķ�̬��ռ
    BOOL32 bSelAccordAdp  = FALSE;

    //�ն�ѡ��
    if( TYPE_MT == tDstMt.GetType() )
    {
        //ȡԴ�ն���Ŀ���ն˵�������
        TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS( tSrcMt.GetMtId() );
        TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS( byDstMtId );
        
        //ѡ��ģʽ�Ͷ�Ӧ��������ƥ��, NACK
        if ( !IsSelModeAndCapMatched( bySwitchMode, tSrcSCS, tDstSCS, bSelAccordAdp) ) 
        {
            cServMsg.SetErrorCode( ERR_MCU_NOTSAMEMEDIATYPE );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            return;
        }
	}
	
	// ��MCS��֪ͨ
	u8 byMcsId = 0;
	if ( pcMsg->event == MCS_MCU_STARTSWITCHMT_REQ || 
		 pcMsg->event == MCS_MCU_STARTSWITCHMC_REQ )
	{
		byMcsId = cServMsg.GetSrcSsnId();
	}

    if ( MODE_VIDEO == bySwitchMode && MODE_BOTH == tSwitchInfo.GetMode() )
	{
		NotifyMcsAlarmInfo( byMcsId, ERR_MCU_SELMODECHANGE_AUDIOFAIL );
	}

    if ( MODE_AUDIO == bySwitchMode && MODE_BOTH == tSwitchInfo.GetMode() )
	{
		NotifyMcsAlarmInfo( byMcsId, ERR_MCU_SELMODECHANGE_VIDEOFAIL );
	}


    tSwitchInfo.SetMode( bySwitchMode );

    switch( cServMsg.GetEventId() )
    {
	case MCS_MCU_STARTSWITCHMT_REQ:
	case MT_MCU_STARTSELMT_CMD:
	case MT_MCU_STARTSELMT_REQ:
        {
            //Ŀ���ն�δ����飬NACK
            if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
            {
                ConfLog( FALSE, "Specified Mt(%u,%u) not joined conference!\n", 
                    tDstMt.GetMcuId(), tDstMt.GetMtId() );
                cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }            
            
            // xsl [7/20/2006] ���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
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
                else if (IsOverSatCastChnnlNum(tSrcMt.GetMtId()))
                {
                    bOverNum = TRUE;
                }

                if (bOverNum)
                {
                    ConfLog(FALSE, "[ProcMtMcuStartSwitchMtReq] over max upload mt num. nack!\n");
                    cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
            }
        
			//����ʱ����ѡ��ģʽ
			if( m_tConf.m_tStatus.IsMixing() )
			{
                //ѡ����Ƶ��NACK
				if( tSwitchInfo.GetMode() == MODE_AUDIO  )
				{
					ConfLog( FALSE, "CMcuVcInst: Conference %s is mixing now. Cannot switch only audio!\n", 
                             m_tConf.GetConfName() );
					cServMsg.SetErrorCode( ERR_MCU_SELAUDIO_INMIXING );
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
				}
                //������תΪѡ����Ƶ
				else
				{
                    //ѡ������Ƶ����ʾ��Ƶѡ��ʧ��
                    if ( MODE_BOTH == tSwitchInfo.GetMode() && cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMT_REQ )
                    {
                        ConfLog( FALSE, "CMcuVcInst: Conference %s is mixing now. Cannot switch audio!\n", 
                                 m_tConf.GetConfName() );
                        cServMsg.SetErrorCode( ERR_MCU_SELBOTH_INMIXING );
                        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );                        
                    }

					tSwitchInfo.SetMode( MODE_VIDEO );
				}
			}
            
			//��Ҫ�����ն˲���ѡ��
            //zbq[05/22/2009] ��Ҫ�����ѡ������������
            if (g_cMcuVcApp.IsSelAccord2Adp() && bSelAccordAdp)
            {
                //�Ժ�̬��ռ������
            }
            else
            {
                u16 wSrcMtBitrate   = m_ptMtTable->GetMtSndBitrate(tSrcMt.GetMtId());
                u16 wDstMtBitrate   = m_ptMtTable->GetMtReqBitrate(tDstMt.GetMtId());
                u16 wSrcDialBitrate = m_ptMtTable->GetSndBandWidth(tSrcMt.GetMtId());
                u16 wDstDialBitrate = m_ptMtTable->GetRcvBandWidth(tDstMt.GetMtId());      
                
                if (wSrcDialBitrate <= wDstDialBitrate && wSrcMtBitrate == wSrcDialBitrate &&
                    !m_tDoubleStreamSrc.IsNull())
                {
                    CServMsg cTmpServMsg;
                    TLogicalChannel tLogicalChannel;
                    if (m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
                    {
                        tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(wSrcDialBitrate));
                        cTmpServMsg.SetMsgBody((u8 *)&tLogicalChannel, sizeof(tLogicalChannel));
                        SendMsgToMt(tSrcMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cTmpServMsg);
                    }                
                }
            }

            //����ѡ��
            if ( !tSrcMtSMcu.IsLocal() )
            {
                //�鿴MC
                TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tSrcMtSMcu.GetMcuId());
                if(ptMcInfo == NULL)
                {
                    cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
                //����ѡ��Դ
                TMt tMt;
                tMt.SetMcuId(tSrcMtSMcu.GetMcuId());
                tMt.SetMtId(0);
                TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
                if(ptStatus == NULL)
                {
                    cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
            
                OnMMcuSetIn( tSrcMtSMcu, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
            }
        
            // ����@2006.4.17 �����ѡ��(��Ƶ������Ƶ)����Ҫ��¼����
            if( MODE_AUDIO == tSwitchInfo.GetMode() || 
                MODE_VIDEO == tSwitchInfo.GetMode() ||
                MODE_BOTH  == tSwitchInfo.GetMode() )
            {
                m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tDstMtStatus ); // ѡ��
                tDstMtStatus.SetSelectMt( tSrcMt, tSwitchInfo.GetMode() );
                m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );
            }
        
            if ( MT_MCU_STARTSELMT_CMD != cServMsg.GetEventId() )
            {
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
            }                

            //�ϼ�MCU
            if( cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMT_REQ &&
                (tDstMt.GetMtType() == MT_TYPE_SMCU || tDstMt.GetMtType() == MT_TYPE_MMCU) )
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
            
            //for h320 mcu cascade select
            if( MT_MCU_STARTSELMT_REQ == cServMsg.GetEventId() && 
                MT_TYPE_SMCU == m_ptMtTable->GetMtType(tDstMt.GetMtId()) )
            {            
                m_tLastSpyMt = tSrcMt;
            } 
			
			//����ѡ����MT�ڻ���ϳ��У���ȥ����ռVMP����ǰ����ͨ��
			if ( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE 
				&& m_tConf.m_tStatus.GetVmpParam().IsMtInMember( tSrcMt ) )
			{
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				
				if(byVmpSubType != VMP) //�õ�VMP��MPU/EVPU
				{
					TVMPParam	tVMPParam   = m_tConf.m_tStatus.GetVmpParam(); 
					
					// xliang [4/21/2009] �����ֱ��ʣ����䳢�Խ�VMPǰ����ͨ��
					u8	byVmpStyle  = tVMPParam.GetVMPStyle();
					u8	byChlPos	= tVMPParam.GetChlOfMtInMember(tSrcMt);	

					ChangeMtVideoFormat(tSrcMt, &tVMPParam, TRUE, TRUE, FALSE); 
				
				}
			}

            // xsl [7/21/2006] ֪ͨ�ն˽��յ�ַ
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                if (MODE_AUDIO == tSwitchInfo.GetMode() || MODE_BOTH == tSwitchInfo.GetMode())
                {
                    ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_AUDIO, FALSE);
                }

                if (MODE_VIDEO == tSwitchInfo.GetMode() || MODE_BOTH == tSwitchInfo.GetMode())
                {
                    ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_VIDEO, FALSE);
                }                
            }            
        }            
		break;
        
    case MCS_MCU_STARTSWITCHMC_REQ:
        {
            // xsl [7/20/2006] ���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                if (IsOverSatCastChnnlNum(tSrcMt.GetMtId()))
                {
                    ConfLog(FALSE, "[ProcMtMcuStartSwitchMtReq] over max upload mt num. nack!\n");
                    cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
            }
            // guzh [8/25/2006]  ����Ǽ���ѡ����Ҫ�л��ش�ͨ��
            if ( !tSrcMtSMcu.IsLocal() )
            {
                //�鿴MC
                TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tSrcMtSMcu.GetMcuId());
                if(ptMcInfo == NULL)
                {
                    cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
                    SendReplyBack( cMsg, cMsg.GetEventId() + 2 );
                    return;
                }
                //����ѡ��Դ
                TMt tMt;
                tMt.SetMcuId(tSrcMtSMcu.GetMcuId());
                tMt.SetMtId(0);
                TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
                if(ptStatus == NULL)
                {                    
                    cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
                    SendReplyBack( cMsg, cMsg.GetEventId() + 2 );
                    return;
                }
            
                OnMMcuSetIn( tSrcMtSMcu, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
            }

            //�����Լ���̬�غ���Ϣ
            TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
            TSimCapSet    tSrcSCS  = m_ptMtTable->GetSrcSCS( tSrcMt.GetMtId() ); 
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

            // zw [06/26/2008] ��ӦAAC LC��ʽ
			if ( MEDIA_TYPE_AACLC == tSrcSCS.GetAudioMediaType() )
			{
                TAudAACCap tAudAACCap;
                tAudAACCap.SetMediaType(MEDIA_TYPE_AACLC);
                tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
                tAudAACCap.SetChnlType(AAC_CHNL_TYPE_SINGLE);
                tAudAACCap.SetBitrate(96);
                tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
                
				cMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
			}

            SendReplyBack( cMsg, cMsg.GetEventId() + 1 );
        }
        break;
        
    default:
        ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
                 pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        break;    
    }

    // ������Ҫ�����Ƿ��ǹ㲥����, zgc, 2008-06-03
    // BUG2354, ��Ҫ��ֹ��ؼ�ؽ���, zgc, 2008-06-12
    if( tSrcMt == m_tVidBrdSrc && cServMsg.GetEventId() != MCS_MCU_STARTSWITCHMC_REQ &&
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
	}

    if (bSelAccordAdp && g_cMcuVcApp.IsSelAccord2Adp())
    {
        if (MODE_BOTH == tSwitchInfo.GetMode())
        {
            tSwitchInfo.SetMode(MODE_AUDIO);
            SwitchSrcToDst(tSwitchInfo, cServMsg);
            tSwitchInfo.SetMode(MODE_VIDEO);
        }
        SwitchSrcToDstAccord2Adp( tSwitchInfo, cServMsg );
    }
    else
    {
	    SwitchSrcToDst( tSwitchInfo, cServMsg );
    }

	
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
	
	ChairmanSeeVmp(tDstMt, cServMsg);
	
	return;

}

/*====================================================================
    ������      ChairmanSeeVmp
    ����        ����ϯѡ������������״̬�Ĳ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt tDstMt
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	4/14/2009 	4.6			Ѧ��          ����
====================================================================*/
void CMcuVcInst::ChairmanSeeVmp(TMt tDstMt, CServMsg &cServMsg)
{
	u8 byDstMtId = tDstMt.GetMtId();
	//����ϯ�ն˲���ѡ��VMP
	if (!HasJoinedChairman() ||
		m_tConf.GetChairman().GetMtId() != byDstMtId)
	{
		cServMsg.SetErrorCode(ERR_MCU_INVALID_OPER);
		ConfLog(FALSE, "[ProcMcsMcuStartSwitchVmpMtReq] Only chairman can select see VMP!\n");
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}

	//���鲻��vmp��
	if (m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_NONE )	
	{
		cServMsg.SetErrorCode(ERR_MCU_VMPNOTSTART);
		ConfLog(FALSE, "[ProcMcsMcuStartSwitchVmpMtReq] The Conf is not in the VMP mode!\n");
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}

	//��VMP��֧��ѡ��
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;			//ȡvmp������
	if(byVmpSubType == VMP)
	{
		cServMsg.SetErrorCode(ERR_FUNCTIONNOTSUPPORTBYOLDVMP);
		ConfLog(FALSE, "Old VMP doesn't support selected!\n");
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}

	u8 byChnnlType = LOGCHL_VIDEO;   
	
    TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( byDstMtId ) ||
		!m_ptMtTable->GetMtLogicChnnl( byDstMtId, byChnnlType, &tLogicChannel, FALSE ) )
    {
        ConfLog(FALSE,"[ProcMcsMcuStartSwitchVmpMtReq]Mt.%u isn't online in conf or backward logical channel not open!\n",byDstMtId);
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
    }

	SendReplyBack(cServMsg,cServMsg.GetEventId()+1);
	
	if(cServMsg.GetEventId() != MT_MCU_STARTSWITCHVMPMT_REQ)
	{
		// xliang [7/3/2009] ����ϯ�ն�Ҳ��Ӧ���Ӷ�ʹmcs��mtc״̬һ��
		SendMsgToMt( byDstMtId, MCU_MT_STARTSWITCHVMPMT_ACK, cServMsg);
	}

	m_tConf.m_tStatus.SetVmpSeebyChairman(TRUE);
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
	g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );//ˢ��tPeriEqpStatus

	// xliang [4/15/2009] ����ϯ�ն˴�ʱ����ѡ��ĳ��MT����Ҫȡ���Ǹ���ϯѡ��Mt��
	// �Ǹ���ѡ����MT���û������MT��ѡ����������MT��ѡ��ȡ�����������˳�ǰ����
	u8 byMode = MODE_VIDEO;
	TMtStatus tDstMtStatus;
	m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );
	if ( !(tDstMtStatus.GetSelectMt(byMode).IsNull()) )
	{
		StopSelectSrc(tDstMt, byMode);
	}

	tDstMtStatus.SetSelectMt( m_tVmpEqp, byMode ); //��¼�µ�ѡ��Դ
	m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );

	if(m_tConf.m_tStatus.IsBrdstVMP())
	{
		//����ϳɱ������ڹ㲥�������˴������ٽ�����
		//ѡ��״̬�ڴ˴��ϱ�
		MtStatusChange( tDstMt.GetMtId(), TRUE );
	}
	else
	{
		u8 bySrcChnnl = 0;
		u8 byMtMediaType = 0;
		u8 byMtRes = 0;
		m_cMtRcvGrp.GetMtMediaRes(tDstMt.GetMtId(), byMtMediaType, byMtRes);
		bySrcChnnl = GetVmpOutChnnlByRes(byMtRes, m_tVmpEqp.GetEqpId(), byMtMediaType);

//		u8 byMtFormat = tLogicChannel.GetVideoFormat(); // ��ȡ�ֱ���
		
// 		//���ݷֱ��ʾ���SrcChnnl,temperary modify to adapt mpu current ability
// 		u8 bySrcChnnl = 0;
// 		switch ( byMtFormat)
// 		{
// 		case VIDEO_FORMAT_HD1080:
// 		case VIDEO_FORMAT_HD720:
// 			bySrcChnnl = 0;
// 			break;
// 		case VIDEO_FORMAT_CIF:
// 			bySrcChnnl = 2;
// 			break;
// 		}

		CallLog("VMP --> MT.%u, SrcChnnlIdx is %u\n",byDstMtId, bySrcChnnl);

		//������
		TSwitchInfo tSwitchInfo;
		tSwitchInfo.SetDstMt(tDstMt);
		tSwitchInfo.SetSrcMt(m_tVmpEqp);
		tSwitchInfo.SetSrcChlIdx(bySrcChnnl);
		tSwitchInfo.SetMode(MODE_VIDEO);

		SwitchSrcToDst( tSwitchInfo, cServMsg );
	}
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
    if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
    {
        ConfLog( FALSE, "[ProcMtMcuStopSwitchMtReq] Dst Mt%u-%u not joined conference!\n", 
                         tDstMt.GetMcuId(), tDstMt.GetMtId() );
        cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }

    //Ack
    if( cServMsg.GetEventId() == MCS_MCU_STOPSWITCHMT_REQ )
    {
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		
		//ͬʱ֪ͨ��ϯ�նˣ�ֹͣѡ��VMP
		if(HasJoinedChairman() && m_tConf.GetChairman() == tDstMt)
		{
			TMtStatus tMtStatus;
			m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
			TMt tSrc = tMtStatus.GetSelectMt( MODE_VIDEO );
			if(tSrc.GetType() == TYPE_MCUPERI && tSrc.GetMtId() == m_tVmpEqp.GetEqpId())
			{
				SendMsgToMt(tDstMt.GetMtId(), MCU_MT_STARTSWITCHVMPMT_NACK,cServMsg);
			}
			m_tConf.m_tStatus.SetVmpSeebyChairman(FALSE);
			// ״̬ͬ��ˢ�µ�TPeriStatus��
			TPeriEqpStatus tVmpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
			tVmpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
			g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
		}
    } 

    StopSelectSrc(tDstMt, MODE_BOTH);

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
	TLogicalChannel	tLogicChnnl, tOtherLogicalChannel;

	switch( CurState() )
	{
	case STATE_ONGOING:
    {
		tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );

		if( pcMsg->event == MT_MCU_OPENLOGICCHNNL_NACK )
		{
			if(tLogicChnnl.GetMediaType() == MODE_VIDEO)
			{
				m_ptMtTable->SetMtVideoRecv( tMt.GetMtId(), FALSE );
				CallLog( "Mcu open Mt%d primary video logicChannel(0x%x:%d%s) error!\n", 
				tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
				tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
			}
			if(tLogicChnnl.GetMediaType() == MODE_AUDIO)
			{
				m_ptMtTable->SetMtAudioRecv( tMt.GetMtId(), FALSE );
				CallLog( "Mcu open Mt%d audio logicChannel(0x%x:%d%s) error!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
			}
			if(tLogicChnnl.GetMediaType() == MODE_SECVIDEO)
			{
				m_ptMtTable->SetMtVideo2Recv( tMt.GetMtId(), FALSE );
				CallLog( "Mcu open Mt%d second video logicChannel(0x%x:%d%s) error!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
                
                //  xsl [6/15/2006] ��˫��ʧ��ʱ���ն˻ָ����մ����������Զ��������򲻻ָ�
                if (m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE) == GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())))
                {
                    u16 wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(tMt.GetMtId());
                    m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), wMtDialBitrate, LOGCHL_VIDEO);
                    Mt2Log("[ProcMtMcuOpenLogicChnnlRsp]tDstMt.GetMtId() = %d, MtReqBitrate = %d\n",
                        tMt.GetMtId(), wMtDialBitrate);
                }            

				//�����л������õ�һ·
				StartSwitchDStreamToFirstLChannel( tMt );
			}

			//�����T120����ͨ��ʧ��
			if ( MODE_DATA == tLogicChnnl.GetMediaType() )
			{
				if ( MEDIA_TYPE_T120 == tLogicChnnl.GetChannelType() )
				{
					if ( m_ptMtTable->IsMtIsMaster( tMt.GetMtId() ) && m_ptMtTable->IsNotInvited( tMt.GetMtId() ))
					{
						SendMcuDcsDelMtReq( tMt.GetMtId() );
						ConfLog( FALSE, "Mcu open mt%d t120 logicchannel (0x%x:%d%s) as master failed !\n",
							tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
							tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) ); 
					}		
					//����Զ�����, ����KDV MCU, ������������
					else
					{
						ConfLog( FALSE, "Mcu open mt%d t120 logicchannel (0x%x:%d%s) as slave failed, its legal affair if KEDA MCU as master !\n",
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

        // �����ǳɹ����߼�ͨ���Ĵ���
		switch( tLogicChnnl.GetMediaType() )
		{
		case MODE_VIDEO:
			m_ptMtTable->SetMtVideoRecv( tMt.GetMtId(), TRUE );
			byChannel = LOGCHL_VIDEO;
			MtStatusChange();
			CallLog( "Mcu open Mt%d primary video logicChannel(0x%x:%d:%s) success!\n", 
				tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
				tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
			
			// xliang [11/10/2008] �򿪵�һ·ͨ���ɹ���ֱ�Ӵ�Polycom��MCU�ڶ�·ͨ��
			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
				(MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
				MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ))
			{
				SetTimer( EV_MCU_OPENSECVID_TIMER, TIMESPACE_WAIT_FSTVIDCHL_SUC, tMt.GetMtId() );
				CallLog( "[OpenLogicChnnlRsp] Mt.%d Fst chnnl open succeed, try the second one immediately due to Polycom\n", tMt.GetMtId() );
			}
			break;

		case MODE_SECVIDEO:
			m_ptMtTable->SetMtVideo2Recv( tMt.GetMtId(), TRUE );
			byChannel = LOGCHL_SECVIDEO;
			MtStatusChange();
			CallLog( "Mcu open Mt%d second video logicChannel(0x%x:%d:H239.%d:%s,Res.%s) success!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), tLogicChnnl.IsSupportH239(), 
					GetMediaStr(tLogicChnnl.GetChannelType()),
					GetResStr(tLogicChnnl.GetVideoFormat()));

			//zbq[09/02/2008] �����ϵ��նˣ�ResΪ�գ������������루��ֻ����˫����
			if (0 == tLogicChnnl.GetVideoFormat())
			{
				TCapSupport tMtCap;
				if (!m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tMtCap))
				{
					ConfLog(FALSE, "[OpenLogicChanRsp] get mt.%d's cap failed, check it\n", tMt.GetMtId());
				}
				tLogicChnnl.SetVideoFormat(tMtCap.GetDStreamCapSet().GetResolution());
			}
			// xliang [11/14/2008]  �򿪵ڶ�·ͨ���ɹ����жϵ�ǰ�Ƿ���˫��Դ�������������Ʋ���˫������
			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
				(MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
				MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ) &&
				!m_tDoubleStreamSrc.IsNull() && !(tMt == m_tDoubleStreamSrc) )
			{
				//��������
				TH239TokenInfo tH239TokenInfo;
				TLogicalChannel tSecVidChnnl;
				
				CServMsg cServMsg;
				cServMsg.SetEventId(MCU_POLY_GETH239TOKEN_REQ);
				cServMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));
				cServMsg.CatMsgBody((u8*)&tH239TokenInfo, sizeof(tH239TokenInfo));
				SendMsgToMt(tMt.GetMtId(), MCU_POLY_GETH239TOKEN_REQ, cServMsg );
				CallLog( "[OpenChnnlRsp] MCU_POLY_GETH239TOKEN_REQ to PolyMCU.%d\n", tMt.GetMtId());
				
			}
			break;

		case MODE_AUDIO:
			m_ptMtTable->SetMtAudioRecv( tMt.GetMtId(), TRUE );
			byChannel = LOGCHL_AUDIO;
			MtStatusChange();
			CallLog( "Mcu open Mt%d audio logicChannel(0x%x:%d:%s) success!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
			break;
		case MODE_DATA:
			if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_H224 )
			{
				byChannel = LOGCHL_H224DATA;
				TMtStatus tMtStatus;
				m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus);
				tMtStatus.SetIsEnableFECC(TRUE);
				m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tMtStatus);
				CallLog( "Mcu open Mt%d data H224 logicChannel(0x%x:%d:%s) success!\n", 
						tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
						tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
			}
			else if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_MMCU )
			{
				byChannel = LOGCHL_MMCUDATA;
				CallLog( "Mcu open Mt%d data mmcu logicChannel(0x%x:%d:%s) success!\n", 
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

						CallLog( "Mcu open Mt%d data T.120 logicChannel(0x%x:%d:%s) as master success!\n", 
							tMt.GetMtId(), dwDcsIp, wDcsPort, GetMediaStr( tLogicChnnl.GetChannelType() ) );
					}
					else
					{
						CallLog( "Mcu open Mt%d data T.120 logicChannel(0x%x:%d:%s) as master failed!\n", 
							tMt.GetMtId(), dwDcsIp, wDcsPort, GetMediaStr( tLogicChnnl.GetChannelType() ) );
					}
				}
				//������˱���
				else
				{
					//����ֻ���յ�MT��ACK��Ϣ, ������Ϊͨ���򿪳ɹ�
					CallLog( "Mcu open Mt%d data T.120 logicChannel as master success!\n", tMt.GetMtId() );
				}
			}
			break;
		}

		//�����߼�ͨ��
		if( tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO )
		{           
            //�ն����д����ʼ��
			
			//zbq[09/10/2008] ���д��������������ѭ�����õĴ���
			if (!m_ptMtTable->GetRcvBandAdjusted(tMt.GetMtId()))
			{
				m_ptMtTable->SetRcvBandWidth(tMt.GetMtId(), m_ptMtTable->GetDialBitrate(tMt.GetMtId()));
			}
			
            if ( !m_tDoubleStreamSrc.IsNull() )
            {
                //zbq[02/01/2008] ���ٺ���ն˵������ʲ������ں��룬ֱ��ȡ���ٺ�Ĵ���
                if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
                     m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()) ) 
                {
                    tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId())) );
                }
                else
                {
                    tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())) );
                }
                //����FlowControl��ʱ��, ��ֹ˫�ٻ��������ն����ʵ��ڵڶ�·����, 07-01-31
				SetTimer( MCUVC_SENDFLOWCONTROL_TIMER, 1200 );
            }
            else
            {
                tLogicChnnl.SetFlowControl( m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) );
            }
		}
        //  xsl [4/28/2006] ����˫�����ʱ���������Ƶ����
        else if ( tLogicChnnl.GetMediaType() == MODE_SECVIDEO && byChannel == LOGCHL_SECVIDEO )
        {
            //zbq[01/31/2008] �����ն˷������Ļ�׼Ӧ���ǽ��ٺ�Ĵ���, �ǽ����ն�Ϊ���д���
            if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
                 m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()) )
            {
                tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId()), FALSE) );
                CallLog("[ProcMtMcuOpenLogicChnnlRsp] Lowed TMt.%d set DSVBR.%d\n", tMt.GetMtId(), tLogicChnnl.GetFlowControl());
                
                //�����ն˴˴�ͬʱ���������һ·��Ƶ����.
                m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(),
											 GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId()), TRUE),
											 LOGCHL_VIDEO );
            }
            else
            {
                tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE) );
                CallLog("[ProcMtMcuOpenLogicChnnlRsp] TMt.%d set DSVBR.%d\n", tMt.GetMtId(), tLogicChnnl.GetFlowControl());
            }
            
            //zbq[02/01/2008] ����FlowControl��ʱ��, ��ֹ���ն�ΪE1�����նˣ�����������ǰ����������Ĵ���
            SetTimer( MCUVC_SENDFLOWCONTROL_TIMER, 1200 );            

            //zbq[02/18/2007] ��˫��ͨ����������ǰ��VMP�㲥����
            if ( m_tConf.m_tStatus.IsBrdstVMP() )
            {
                AdjustVmpBrdBitRate();
            }

            /*
            // ��ӽ�������, zgc, 2008-08-07
            u8 emType = m_cConfVidTypeMgr.ConvertOut2In( tLogicChnnl.GetChannelType(), tLogicChnnl.GetVideoFormat(), TRUE );
            m_cConfVidTypeMgr.AddType( emType, TRUE );*/

			//zbq[09/12/2008]д��֡��
			TCapSupport tCap;
			m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tCap);
			if (tCap.GetDStreamCapSet().IsFrameRateUserDefined())
			{
				tLogicChnnl.SetChanVidFPS(tCap.GetDStreamCapSet().GetUserDefFrameRate());
			}
			else
			{
				tLogicChnnl.SetChanVidFPS(tCap.GetDStreamCapSet().GetFrameRate());
			}
        }
		tLogicChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
		tLogicChnnl.SetActivePayload(GetActivePayload(m_tConf,tLogicChnnl.GetChannelType()));
		m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannel, &tLogicChnnl, TRUE );
		
		//���˼���mcu��Ϣ����������б���
		if( MEDIA_TYPE_MMCU == tLogicChnnl.GetChannelType() )
		{
			m_ptConfOtherMcTable->AddMcInfo( tMt.GetMtId() );	
			m_tConfAllMtInfo.AddMcuInfo( tMt.GetMtId(), tMt.GetConfIdx() );
	
			//������ػ��������ն���Ϣ
			cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
			SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
		
			//��������ȫ�������£������Զ������¼�mcu�Ļ�������			
            if( m_tConf.m_tStatus.IsMixing() )
			{
                CServMsg cTmpMsg;
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
		}
	
		//��Ƶ������ʩ
		if( tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO )
		{
            //zbq[04/22/2009] ���� ����ͨ����̬ˢ����Ⱥ��λ��
            RefreshRcvGrp(tMt.GetMtId());

			//��Ӷ����ش��ն�
			if( m_tConf.m_tStatus.IsPrsing() )
			{
				AddRemovePrsMember( tMt, TRUE, PRSCHANMODE_FIRST );
			}

            // xsl [7/21/2006] ��������鲥��ַ����
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {                
                ChangeSatDConfMtRcvAddr(tMt.GetMtId(), LOGCHL_VIDEO);
            }

			//��ʼý���������� - modify bas 2 -- ���Կ����Ƿ���ҪStartAdapt()
            TSimCapSet tSrcSimCapSet;
            TSimCapSet tDstSimCapSet;
            u16 wAdaptBitRate;
            if (m_tConf.GetConfAttrb().IsUseAdapter())
            {
                // ���Ǹ����������
                if ( IsHDConf( m_tConf ) )
                {
                    if (m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId()))
                    {
                        //zbq[04/11/2009] �����ն˶�̬����������
                        if (m_tConf.m_tStatus.IsHdVidAdapting())
                        {
                            RefreshBasParam(tMt.GetMtId(), TRUE);
                            StartSwitchToSubMtFromAdp(tMt.GetMtId());
                        }
                        else
                        {
                            StartHDMVBrdAdapt();
                        }
                    }
                }
                else
                // guzh [7/24/2007] �����Ҫ��ʽ���䣬���������
                // zbq [08/13/2007] ���ڽ��Ϳ������ǣ�ֻ�жϵ�ǰ�ն��Ƿ���Ҫ����
                if (IsMtNeedAdapt(ADAPT_TYPE_VID, tMt.GetMtId()) &&
                    IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
                {
                    // 1. ԭ������Ҫ����MT���ߵ��¿�����ʽ����
                    if (!m_tConf.m_tStatus.IsVidAdapting())
                    {
                        StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }
                    else                    
                    {
                        // 2. ԭ�����ڸ�ʽ���䣬�����Ƿ���Ҫ������
                        // zbq [08/11/2007] ��ֱ�ӽ����������
                        bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
                        StartSwitchToPeriEqp(m_tVidBrdSrc, bySrcChnnl, m_tVidBasEqp.GetEqpId(),
                                             m_byVidBasChnnl, MODE_VIDEO);

                        ChangeAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }
                }
                // zbq [08/13/2007] ���ڽ��Ϳ������ǣ�ֻ�жϵ�ǰ�ն��Ƿ���Ҫ����
                else if(IsMtNeedAdapt(ADAPT_TYPE_BR, tMt.GetMtId()) &&
                        IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate) )
                {
                    if (!m_tConf.m_tStatus.IsBrAdapting() )
                    {
                        StartAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }
                    else
                    {                        
                        bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
                        StartSwitchToPeriEqp(m_tVidBrdSrc, bySrcChnnl, m_tBrBasEqp.GetEqpId(),
                                             m_byBrBasChnnl, MODE_VIDEO);
                        
                        ChangeAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }

                }
            }
			if( !m_tVidBrdSrc.IsNull() )
			{
				// xliang [3/19/2009] �㲥Դ��mpu������ͨ����
				BOOL32 bNewVmpBrd = FALSE;
				if(m_tVidBrdSrc == m_tVmpEqp)
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
					SwitchNewVmpToSingleMt(tMt);
				}
				else
				{
					bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
					if (!m_tConf.GetConfAttrb().IsSatDCastMode())
					{
						// zbq[12/18/2008] ֱ���߹㲥�������������.Ϊʲô�����޸ģ����ӹ���֮.
						// �й㲥ԴӦ�߹㲥����, zgc, 2008-03-06
						//StartSwitchToSubMt( m_tVidBrdSrc, bySrcChnnl, tMt.GetMtId(), MODE_VIDEO );
						if (!(IsHDConf(m_tConf) && m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId())))
						{
							StartSwitchFromBrd( m_tVidBrdSrc, bySrcChnnl, 1, &tMt );
						}
                        else
                        {
                            NotifyMtReceive(m_tVidBrdSrc, tMt.GetMtId());
                        }
					}
					else
					{
						// ���� [5/29/2006]
						//���ǻ������֪ͨ���������ˣ�������ȥ��������
						NotifyMtReceive( m_tVidBrdSrc, tMt.GetMtId() );
					}
				}
			}
			else
			{
				//֪ͨ�ն��տ�����
				cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
				SendMsgToMt( tMt.GetMtId(), MCU_MT_YOUARESEEING_NOTIF, cServMsg );
				//�Լ�����Ƶ�������Լ�
				StartSwitchToSubMt( tMt, 0, tMt.GetMtId(), MODE_VIDEO );

				//����VCS���鵱ǰ�����ն˿���ϯ
				if (VCS_CONF == m_tConf.GetConfSource() &&
					!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
				{
					GoOnSelStep(tMt, MODE_VIDEO, TRUE); 
				}
			}
	
			// ʹ���µķ�����ѡ������, zgc, 2008-03-06
			if( HasJoinedSpeaker() && 
				!m_tConf.m_tStatus.IsBrdstVMP() && 
				!( m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO ) &&
				( tMt == GetLocalSpeaker() || tMt == m_tConf.GetChairman() || tMt == m_tLastSpeaker ) )
			{
				// �·���������Դ�����߼�, zgc, 2008-04-12
				ChangeSpeakerSrc(MODE_VIDEO, emReasonChangeSpeaker);
			}
		}

		//˫������
        if (tLogicChnnl.GetMediaType() == MODE_SECVIDEO && byChannel == LOGCHL_SECVIDEO)
        {
            //�Ƿ�Ҫת��˫������
            TLogicalChannel tH239LogicalChannel;
            if (TYPE_MCUPERI == m_tDoubleStreamSrc.GetType() ||
                m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicalChannel, FALSE))
            {
                // ����[5/25/2006] ����������Ƿ�ɢ����,��Ҫ����˫��
                if (!m_tConf.GetConfAttrb().IsSatDCastMode())                
                {
                    // ����Ҫ�����ֱ�ӽ�����, zgc, 2008-08-20
                    if ( !m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE))
                    {
                        // zbq [07/26/2007] ¼�����˫�����������������ͨ��
                        u8 bySrcChnnl = m_tDoubleStreamSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
#ifndef _SATELITE_
                        g_cMpManager.StartSwitchToSubMt(m_tDoubleStreamSrc, bySrcChnnl, tMt, MODE_SECVIDEO, SWITCH_MODE_BROADCAST, TRUE);
#else
						g_cMpManager.StartSatConfCast(m_tDoubleStreamSrc, CAST_FST, MODE_SECVIDEO,  bySrcChnnl);
#endif
                    }
                    else
                    {
                        StartSwitchToSubMtFromAdp(tMt.GetMtId(), TRUE);
                    }
                }               
            }

            //�ڶ�·��Ƶ�Ķ����ش�
            if (m_tConf.m_tStatus.IsPrsing() && m_tDoubleStreamSrc.GetMtId() != tMt.GetMtId())
            {
                // ����Ҫ�����ֱ�ӽ������ش�����, zgc, 2008-08-20
                if ( !m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE) )
                {
                    AddRemovePrsMember(tMt, TRUE, PRSCHANMODE_SECOND);
                }
            }

            // xsl [7/21/2006] ��������鲥��ַ����
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {                
                ChangeSatDConfMtRcvAddr(tMt.GetMtId(), LOGCHL_SECVIDEO);
            }
        }

		//��Ƶ������ʩ
		if( tLogicChnnl.GetMediaType() == MODE_AUDIO )
		{
            //����ն˵���Ƶ�����ش�
			if (m_tConf.m_tStatus.IsPrsing())
			{
				AddRemovePrsMember(tMt, TRUE, PRSCHANMODE_AUDIO);
			}

            // xsl [7/21/2006] ��������鲥��ַ����
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {               
                ChangeSatDConfMtRcvAddr(tMt.GetMtId(), LOGCHL_AUDIO);
            }

			if( HasJoinedSpeaker() )
			{
				bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;

				if (m_tConf.m_tStatus.IsNoMixing() || m_tConf.m_tStatus.IsMustSeeSpeaker())
				{
                    // ���� [5/29/2006]
                    if (!m_tConf.GetConfAttrb().IsSatDCastMode())
                    {
                        StartSwitchToSubMt( GetLocalSpeaker(), bySrcChnnl, tMt.GetMtId(), MODE_AUDIO );
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
            
			// xsl [8/4/2006] ���ƻ�����nģʽ�����������ն�(�����˳���)
			if( m_tConf.m_tStatus.IsMixing() )
			{
                if (m_tConf.m_tStatus.IsSpecMixing())
                {
                    if (tMt == GetLocalSpeaker() && GetMixMtNumInGrp() < GetMaxMixNum())
                    {
                        // guzh [7/14/2007] ������ChangeSpeaker�ﱣ����������Ҫ����AddRemoveһ�Ρ�����InMixingͼ��᲻��
                        // ԭ��Ϊʲô AddMixMember��
                        AddRemoveSpecMixMember(&tMt, 1, TRUE); 
                        //AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE);
                    }
                    else
                    {
                        SwitchMixMember(&tMt, TRUE);
                    }   
                }
                else 
                {
                    if (GetMixMtNumInGrp() < GetMaxMixNum())
                    {
                        AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE);
                    }                    
                }
            }
			else
			{
				// ����VCS����ǻ���ģʽ�£��ն�����ϯ
				if (VCS_CONF == m_tConf.GetConfSource() &&
					!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
				{
					GoOnSelStep(tMt, MODE_AUDIO, TRUE); 
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
	CServMsg	cSendMsg( pcMsg->content, pcMsg->length );
	TMt	tMt;
	u32	dwRcvIp;
	u16	wRcvPort;
	u8  byChannel;
    u8  byAudioType;

    TCapSupportEx tCapSupportEx;
	TLogicalChannel	tLogicChnnl, tVideoLogicChnnl, tH239LogicChnnl;

	switch( CurState() )
	{
	case STATE_ONGOING:
        {
		tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );

#ifdef _SATELITE_
		if (MODE_SECVIDEO == tLogicChnnl.GetMediaType())
		{
			//�����ն˷���˫�������ݻ�����˫����������TLogicChannel��8005��֧��˫��
			u8 byConfDSMainType = m_tConf.GetDStreamMediaType();
			tLogicChnnl.SetChannelType(byConfDSMainType);
			tLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, byConfDSMainType));
			
			if (MEDIA_TYPE_H264 == byConfDSMainType)
			{
				tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamUsrDefFPS());
			}
			else
			{
				tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamFrameRate());
			}
			
			tLogicChnnl.SetVideoFormat(m_tConf.GetDoubleVideoFormat());
			tLogicChnnl.SetSupportH239(TRUE);
		}
#endif
		
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
					   MT_TYPE_MT == m_ptMtTable->GetMtType(tMt.GetMtId()) ) )
                {
                }
                else
                {                
                    //�ж�˫���ķ���Դ 
                    if (CONF_DUALMODE_EACHMTATWILL == m_tConf.GetConfAttrb().GetDualMode() ||
                        (CONF_DUALMODE_SPEAKERONLY == m_tConf.GetConfAttrb().GetDualMode() && tMt == GetLocalSpeaker() ) ||
						// xliang [11/14/2008] PolycomMCU����������
						// xliang [11/14/2008] FIXME:������Ǳ��ʱ������
						MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId())) 
                    {                       
                        // xliang [11/14/2008]  polycomMCU ��˫��ͨ������������˫����������flowctrl������Ӧ����
						if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
							( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
							MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId())) )
						{
							CallLog("[ProcMtMcuOpenLogicChnnlReq] PolyMcu.%d open DVideo req here\n", tMt.GetMtId());
						}
						else
						{
							UpdateH239TokenOwnerInfo( tMt );
							StartDoubleStream( tMt, tLogicChnnl );
                            CallLog("[ProcMtMcuOpenLogicChnnlReq] Chn connected ntf rcved, start ds!\n");
						}
                    }
                    else
                    {
                        CallLog("[ProcMtMcuOpenLogicChnnlReq] Chn connected ntf rcved, no start ds due to DualMode.%d, tMt.%d\n",
                            m_tConf.GetConfAttrb().GetDualMode(), tMt.GetMtId());
                    }
    			}
            }
            return;
        }

		// �������ն˿�ʼ�����MCUͨ��ʱ����Ϊ�õ����ն˵��ȳɹ����ɽ�����һ���ն˵ĵ���
		if (VCS_CONF == m_tConf.GetConfSource() &&
			tMt == m_cVCSConfStatus.GetReqVCMT())
		{
		 	KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
			ChgCurVCMT(tMt);
		}
		
        //����Ϊ�������߼�ͨ��ACK/NACK����
		if(tLogicChnnl.GetMediaType() == MODE_VIDEO)
		{
            m_ptMtTable->SetMtVideoSend(tMt.GetMtId(), FALSE);

            //  xsl [3/17/2006] 263+��ʽ������ݶ�̬�غɹ���
			if (!m_tConf.GetCapSupport().IsSupportMediaType(tLogicChnnl.GetChannelType()) && 
                MEDIA_TYPE_H263PLUS != tLogicChnnl.GetChannelType())
			{
				ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) not support video(%s), nack!\n",
					tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , GetMediaStr(tLogicChnnl.GetChannelType()));
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
            // ����@2006.4.13 ���<64K���ʣ���ܾ����߼�ͨ��
            if (m_tConf.GetBitRate() < 64)
            {
				ConfLog( FALSE, "Bitrate less than 64K, open video logic chnl nack!\n");
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
            }
		}
		if(tLogicChnnl.GetMediaType() == MODE_AUDIO)
		{
            m_ptMtTable->SetMtAudioSend(tMt.GetMtId(), FALSE);

			if(!m_tConf.GetCapSupport().IsSupportMediaType(tLogicChnnl.GetChannelType()))
			{
				ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) not support audio(%s), nack!\n",
					tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , GetMediaStr(tLogicChnnl.GetChannelType()));
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}			
		}
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
            m_ptMtTable->SetMtVideo2Send(tMt.GetMtId(), FALSE);
			
			// xliang [12/18/2008]  ˫��ͨ���ֱ���ƥ��У��
			u8 bySrcRes = tLogicChnnl.GetVideoFormat();
			u8 byDstRes = m_tConf.GetCapSupport().GetDStreamCapSet().GetResolution();
			if (!IsDSResMatched(bySrcRes, byDstRes))
			{
				CallLog("[ProcMtMcuOpenLogicChnnlReq] Mt.%d's DS res dismatched<src.%d, dst.%d>, ignore it\n", 
					tMt.GetMtId(), bySrcRes, byDstRes);
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}

            if(tLogicChnnl.IsSupportH239() != m_tConf.GetCapSupport().IsDStreamSupportH239() &&
               tLogicChnnl.GetChannelType() != m_tConf.GetCapSupport().GetDStreamMediaType() &&
               //zbq[01/04/2009] ˫˫�����˷ſ�
               tLogicChnnl.GetChannelType() != m_tConf.GetCapSupportEx().GetSecDSType())
            {
                ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) not support second video (%s), nack!\n",
                    tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , GetMediaStr(tLogicChnnl.GetChannelType()));
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
            // ����@2006.4.13 ���<64K���ʣ���ܾ����߼�ͨ��
            if (m_tConf.GetBitRate() < 64)
            {
				ConfLog( FALSE, "Bitrate less than 64K, open sec video logic chnl, nack!\n");
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
					ConfLog( FALSE, "Mt%d open mcu T.120 logicchannel as slave, nack it directly!\n", tMt.GetMtId() );
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
				ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) not encrypt, nack!\n",
					tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;	
			}
		}


		//�����߼�ͨ��
		if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
		{
			dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
			if( dwRcvIp == 0 )
			{
				ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) mp not connected, nack!\n",
						 tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;			    
			}
			wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
			m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
		}

		//�������߼�ͨ���򿪴���
		tLogicChnnl.m_tRcvMediaChannel.SetIpAddr( dwRcvIp );
		tLogicChnnl.m_tRcvMediaCtrlChannel.SetIpAddr( dwRcvIp );

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
                ConfLog(FALSE, "[ProcMtMcuOpenLogicChnnlReq] remote bitrate %d, local bitrate %d. nack\n", 
                                tLogicChnnl.GetFlowControl(), 
                                m_tConf.GetBitRate() + GetAudioBitrate(byAudioType));
				return;	
			}

			if( MEDIA_TYPE_H263PLUS == tLogicChnnl.GetChannelType() && 
				MEDIA_TYPE_H263PLUS == tLogicChnnl.GetActivePayload() )
			{
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                ConfLog(FALSE, "[ProcMtMcuOpenLogicChnnlReq] ChnType.%d, ActivePayload.%d, nack\n", 
                                tLogicChnnl.GetChannelType(),
                                tLogicChnnl.GetActivePayload() );
				return;
			}

			CallLog( "Mt%d open mcu primary video logicChannel(0x%x:%d:%s)\n", 
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
                if (CONF_DUALMODE_EACHMTATWILL == m_tConf.GetConfAttrb().GetDualMode() ||
                    ( CONF_DUALMODE_SPEAKERONLY == m_tConf.GetConfAttrb().GetDualMode() && tMt == GetLocalSpeaker() ) ) 
                {
                    // xsl [7/20/2006]���Ƿ�ɢ����ʱ��Ҫ���ǻش�ͨ����
                    if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsOverSatCastChnnlNum(tMt.GetMtId()))
                    {
                        ConfLog(FALSE, "[ProcMtMcuOpenLogicChnnlReq] over max upload mt num.\n");
                        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );

                        return;
                    }
					//�ӳٵ�connect����
					/*UpdateH239TokenOwnerInfo( tMt );
					bStartDS = TRUE;
					StartDoubleStream( tMt, tLogicChnnl );*/
/*
#ifdef _SATELITE_
					//���ǻ����£�ȱ����һ������Ϣ�����������Ӻ���
					StartDoubleStream( tMt, tLogicChnnl );
#endif
*/
                }
                else
                {
#ifdef _SATELITE_
					//���ǻ���Ŀǰֻ�����ڴ˴��ܾ��ն˵�˫������
					CServMsg cMsg;
					cMsg.SetConfIdx(m_byConfIdx);
					cMsg.SetDstMtId(tMt.GetMtId());
					cMsg.SetEventId(30255 /*MCU_MT_STARTDS_REQ + 1*/);
					
					u8 byDSMsgMode = 1;	//1: open req/rsp; 0: close req/rsp
					u16 wBitRate = 0;	//reject
					cServMsg.SetMsgBody(&byDSMsgMode, sizeof(u8));
					cServMsg.CatMsgBody((u8*)&wBitRate, sizeof(u16));
					SendMsgToMt(tMt.GetMtId(), 30255 /*MCU_MT_STARTDS_REQ + 1*/, cMsg);
#endif
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    CallLog( "Mt%d open mcu second video logicChannel(0x%x:%d:H239.%d:%s) with wrong Dualmode, NACK !\n",
                        tMt.GetMtId(), dwRcvIp, wRcvPort + 4, tLogicChnnl.IsSupportH239(), GetMediaStr(tLogicChnnl.GetChannelType()));
                    return;	
                }
			}

			// zbq [09/12/2008] Mtadp����ȡ����֡�ʣ��˴�����Ϊ����
			if (tLogicChnnl.GetChanVidFPS() > 25)
			{
				if (MEDIA_TYPE_H264 == tLogicChnnl.GetChannelType())
				{
					tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamUsrDefFPS());
				}
				else
				{
					tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamFrameRate());
				}
			}
            // zbq [09/05/2007] FIXME: H264˫���ķֱ����ݲ����ǣ���������
			CallLog( "Mt%d open mcu second video logicChannel(0x%x:%d:H239.%d:%s), Dualmode: %d\n", 
					  tMt.GetMtId(), dwRcvIp, wRcvPort + 4, tLogicChnnl.IsSupportH239(), 
					  GetMediaStr(tLogicChnnl.GetChannelType()), m_tConf.GetConfAttrb().GetDualMode() );			
			break;

		case MODE_AUDIO:
			byChannel = LOGCHL_AUDIO;
			tLogicChnnl.m_tRcvMediaChannel.SetPort( wRcvPort + 2 );
			tLogicChnnl.m_tRcvMediaCtrlChannel.SetPort( wRcvPort + 3 );
			CallLog( "Mt%d open mcu audio logicChannel(0x%x:%d:%s)\n", 
					tMt.GetMtId(), dwRcvIp, wRcvPort+2, GetMediaStr( tLogicChnnl.GetChannelType() ) );
			break;
			
		case MODE_DATA:
			if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_H224 )
			{
				byChannel = LOGCHL_H224DATA;
				dwRcvIp = g_cMcuVcApp.GetMtAdpIpAddr( tMt.GetDriId() );
				tLogicChnnl.m_tRcvMediaChannel.SetIpAddr( dwRcvIp );
				tLogicChnnl.m_tRcvMediaChannel.SetPort( wRcvPort + 6 );
				tLogicChnnl.m_tRcvMediaCtrlChannel.SetPort( wRcvPort + 7 );
				CallLog( "Mt%d open mcu H.224 logicChannel(0x%x:%d:%s)\n", 
					tMt.GetMtId(), dwRcvIp, wRcvPort + 6 , GetMediaStr( tLogicChnnl.GetChannelType() ) );
				
			}
			//�Զ�����򿪱��˵�T120����ͨ��
			else if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_T120 )
			{
				//��ʱ, ����ֻ���Ǳ���. �Զ������KEDA MCU, ���ַӦ��Ϊ��
				//��д���˵�DCS������ַ, �ظ�ACK, �ɶԶ���MCU��������
				if ( !m_ptMtTable->IsNotInvited( tMt.GetMtId() ) )
				{
					CallLog( "Mt%d should be calling in !\n", tMt.GetMtId() );
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
						CallLog( "Mcu open Mt%d data T.120 logicChannel(0x%x:%d:%s) as slave success!\n", 
							tMt.GetMtId(), dwDcsIp, wDcsPort, GetMediaStr( tLogicChnnl.GetChannelType() ) );		
					}
					else
					{
						CallLog( "Get local DCS Ip failed in open logic channel ack !\n" );
						return;
					}
				}				
			} 
			else if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_MMCU )
			{			
				if( !m_tConf.GetConfAttrb().IsSupportCascade() ||
					tMt.GetMtType() != MT_TYPE_MMCU || 
					m_tConfAllMtInfo.GetCascadeMcuNum() >= MAXNUM_SUB_MCU)
				{
					ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) not support mmcu(%s), nack!\n",
						tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , GetMediaStr(tLogicChnnl.GetChannelType()));
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					return;	
				}
                else if ( !m_tCascadeMMCU.IsNull() )
                {
                    // guzh [4/12/2007] ����Ѿ����ϼ�MCU�������Ƿ���Ǹ�MCU
                    // ���ﲻ�ܾ��Բ���IP�Ƚϣ��ϼ����ܻ�����塣ֻ�����ȳ��Ա�������E164���п��ܻᷢ������)
                    BOOL32 bRejectAndRemoveMMcu = FALSE;
                    if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
                    {
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
                        CallLog("Mt%d 0x%x(Dri:%d) calling in seems to be the MMCU, Drop both!",
                                tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId());

                        SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
                        SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);   
                    }
                    else
                    {
                        CallLog( "Mt%d 0x%x(Dri:%d) calling in, but current has MMCU, nack!\n",
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
				
				CallLog( "Mt%d open mcu mmcu logicChannel(0x%x:%d:%s)\n", 
					tMt.GetMtId(), dwRcvIp, wRcvPort+9, GetMediaStr( tLogicChnnl.GetChannelType() ) );

				//���ն��Ƿ��Ѿ�ͨ��������֤
				if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
				{
					m_tCascadeMMCU = tMt;
					m_tConfAllMtInfo.m_tMMCU = tMt;
                    m_tConfInStatus.SetNtfMtStatus2MMcu(TRUE);
					
					//���˼���mcu��Ϣ����������б���
					m_ptConfOtherMcTable->AddMcInfo( tMt.GetMtId() );	
					m_tConfAllMtInfo.AddMcuInfo( tMt.GetMtId(), tMt.GetConfIdx() );

					//֪ͨ���
					cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
					SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
					
					//������ػ��������ն���Ϣ
					cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
					SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

					//�¼������Զ����ϼ���Ϊ������
					if(m_tConf.GetSpeaker().IsNull() && g_cMcuVcApp.IsMMcuSpeaker())
					{
						ChangeSpeaker(&m_tCascadeMMCU);
					}
				}

			}
			break;
		}
        
        // zbq [10/11/2007] ǰ�����̬�غ��������� ��Ƶ/��Ƶ/˫�� ֧��
        BOOL32 bLogicChanFEC = FALSE;
        if ( tLogicChnnl.GetFECType() != FECTYPE_NONE )
        {
            bLogicChanFEC = TRUE;
            tLogicChnnl.SetActivePayload(MEDIA_TYPE_FEC);
            CallLog("Mt.%d set ActivePayload.%d due to FEC\n", tMt.GetMtId(), MEDIA_TYPE_FEC);
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
        
		//����
		m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannel, &tLogicChnnl, FALSE );

		//zbq[09/02/2008] ˫���ӳٵ����﷢��
// 		if (MODE_SECVIDEO == tLogicChnnl.GetMediaType() && bStartDS)
// 		{
// 			StartDoubleStream( tMt, tLogicChnnl );
// 		}

#ifdef _SATELITE_
		//���ǻ����£�ȱ����һ������Ϣ�����������Ӻ���
		if (MODE_SECVIDEO == tLogicChnnl.GetMediaType())
		{
			StartDoubleStream( tMt, tLogicChnnl );
		}
#endif

        //�ն����д����ʼ��
        if(tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO)
        {
            m_ptMtTable->SetSndBandWidth(tMt.GetMtId(), tLogicChnnl.GetFlowControl());
        }

		//�ڶ�·��Ƶ�Ķ����ش�
		if( byChannel == LOGCHL_SECVIDEO)
		{
			if(m_tConf.GetConfAttrb().IsResendLosePack())
			{
				ChangePrsSrc( tMt, PRSCHANMODE_SECOND);
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
			m_ptMtTable->SetMtVideo2Send(tMt.GetMtId(), TRUE);
		}

		cServMsg.SetMsgBody( ( u8 * )&tLogicChnnl, sizeof( tLogicChnnl ) );

#ifndef _SATELITE_
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
#else
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
			//���ǻ���ACKӦ��
			CServMsg cMsg;
			cMsg.SetConfIdx(m_byConfIdx);
			cMsg.SetDstMtId(tMt.GetMtId());
			cMsg.SetEventId(30255 /*MCU_MT_STARTDS_REQ + 1*/);
			
			u16 wBitRate = GetDoubleStreamVideoBitrate(m_ptMtTable->GetDialBitrate(tMt.GetMtId()), FALSE);
			wBitRate = htons(wBitRate);
			u8 byDSMsgMode = 1;
			cMsg.SetMsgBody(&byDSMsgMode, sizeof(u8));
			cMsg.CatMsgBody((u8*)&wBitRate, sizeof(u16));
			SendMsgToMt(tMt.GetMtId(), 30255/*MCU_MT_STARTDS_REQ + 1*/, cMsg);

		}
#endif
		if (!m_tCascadeMMCU.IsNull() &&
			m_tConfAllMtInfo.MtJoinedConf(m_tCascadeMMCU.GetMtId()) &&
			(tLogicChnnl.GetMediaType() == MODE_VIDEO ||
			 tLogicChnnl.GetMediaType() == MODE_AUDIO))
		{
			OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(), tMt.GetMtId());
		}
	
		//���ն˿���δͨ��������֤
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
		{
			CallLog( "Mt%d doesnot join conf, then doesnot deal audio/video switch ... \n", 
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
			if ( tLogicChnnl.GetMediaType() == MODE_VIDEO )
			{
				AdjustChairmanVidSwitch();
			}
        }

		//������Ƶ��������
		if( tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO )
		{
			//��������, ת��dump����ֹicmp
			g_cMpManager.StartRecvMt( tMt, MODE_VIDEO );
            
            //zbq[07/18/2007] SSRC ���޸��ɹ�һ����ͳһ����
			//g_cMpManager.ResetRecvMtSSRC( tMt, MODE_VIDEO );

			//���Ƿ����˼���
			if(GetLocalSpeaker() == tMt )
			{
                // xsl [8/22/2006]���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
                if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsOverSatCastChnnlNum(tMt.GetMtId()))
                {
                    ConfLog(FALSE, "[ProcMtMcuOpenLogicChnnlReq] over max upload mt num. cancel speaker!\n");
                    ChangeSpeaker(NULL);
                }
                else
                {
                    ChangeSpeaker( &tMt );
                }				
			}

			if( !( GetLocalSpeaker() == tMt ) )
			{
				if( tMt.GetMtType() == MT_TYPE_MMCU || tMt.GetMtType() == MT_TYPE_SMCU )
				{				
					NotifyMtSend( tMt.GetMtId(), MODE_VIDEO, TRUE );
				}
				else
				{
                    //  xsl [1/18/2006] �����vmp��tvwallģ���ﲻ��flowcontrol 0                   
                    if (!m_tConfEqpModule.IsMtInVmp(tMt) && 
                        FALSE == m_tConfEqpModule.IsMtInTvwall(tMt))
                    {
                        NotifyMtSend( tMt.GetMtId(), MODE_VIDEO, FALSE );
                    }					
				}			
			}

            //�йػ���ϳɵĴ���
            if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
            {
				TPeriEqpStatus tPeriEqpStatus;
                g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

                if( IsDynamicVmp() )
                {
                    if (m_tConfInStatus.IsVmpNotify())
                    {
                        ChangeVmpStyle(tMt, TRUE);
                    }
                    else
                    {
                        ChangeVmpStyle(tMt, TRUE, FALSE);
                    }
                }
                else if( m_tConf.GetConfAttrb().IsHasVmpModule() && 
                         !m_tConfInStatus.IsVmpModuleUsed() )
                {
					TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
                    u8 byChlNum = tVmpParam.GetMaxMemberNum();
                    u8 byMemberType = 0;
					u8 byVmpStyle = m_tConf.m_tStatus.GetVmpStyle();
                    for( u8 byLoop = 0; byLoop < byChlNum; byLoop++ )
                    {
                        if( m_tConfEqpModule.IsMtAtVmpChannel( byLoop, tMt, byMemberType ) )
                        {
                            UpdataVmpEncryptParam(tMt, byLoop);
                            Mt2Log("[ProcMtMcuOpenLogicChnnlReq] tMt<%d, %d>, byChnnl:%d, mode:%d\n",
                                tMt.GetMcuId(),tMt.GetMtId(), byLoop, byMemberType);

                            //SetVmpChnnl( tMt, byLoop, VMP_MEMBERTYPE_MCSSPEC, TRUE );
                            // ���� [5/11/2006] ֧��ָ�����淽ʽ
                            // ����ֻ�����ն˳�����Vmp��Ա���档
                            // ����Ǹ���Ļ����ڸı���ϯ/������ʱ�ᴦ��
							// xliang [3/19/2009] FIXME�� ����MPU��������������MT�����ͽ�����VMP
							if( byVmpSubType == VMP )
							{
								SetVmpChnnl( tMt, byLoop, byMemberType, TRUE );
							}
							else
							{
								ChangeMtVideoFormat( tMt, &tVmpParam );
								SetVmpChnnl( tMt, byLoop, byMemberType, TRUE );
							}
                                                        
                        }
                    }
                    //ͬ�����踴��״̬
                    TPeriEqpStatus tPeriEqpStatus;
                    g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
                    tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
                    g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );

                }
            }

			//�ж��Ƿ�Ϊ�ش�ͨ��,����ָ��ش�����
			if( !m_tCascadeMMCU.IsNull() && !(m_tCascadeMMCU == tMt) )
			{
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
				if( ptConfMcInfo != NULL && 
					( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
				{
					if( ptConfMcInfo->m_tSpyMt.GetMtId() == tMt.GetMtId() &&
						ptConfMcInfo->m_tSpyMt.GetMcuId() == tMt.GetMcuId() )
					{
						ptConfMcInfo->m_tSpyMt.SetNull();
						StartSwitchToMcu( tMt, 0, m_tCascadeMMCU.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);
						ptConfMcInfo->m_tSpyMt = tMt;
					}
				}
			}

			//�е���ǽģ�岢����������豸�����ӵĻ�
            if (m_tConf.GetConfAttrb().IsHasTvWallModule() )
            {
                u8 byMemberType = 0;
                for(u8 byTvPos = 0; byTvPos < MAXNUM_PERIEQP_CHNNL; byTvPos++)
                {
                    u8 byTvWallId = m_tConfEqpModule.m_tTvWallInfo[byTvPos].GetTvWallEqp().GetEqpId();
                    if( 0 != byTvWallId && g_cMcuVcApp.IsPeriEqpConnected(byTvWallId) )
                    {
                        for(u8 byTvChlLp = 0; byTvChlLp < MAXNUM_PERIEQP_CHNNL; byTvChlLp++)
                        {
                            // ͬ�ϣ�������ʱֻ�����˱��ն˳�����Tw��Ա���档
                            // �����ָ�����������棬��Ӧ���ڸı���ϯ/�����˵�ʱ���Զ������
                            if( m_tConfEqpModule.IsMtInTvWallChannel( byTvWallId, byTvChlLp, tMt, byMemberType) )
                            {
                                // ���� [5/11/2006] ֧��ָ�����͵Ŀ�ʼ��ʽ
                                //ChangeTvWallSwitch(&tMt, byTvWallId, byTvChlLp, TW_MEMBERTYPE_MCSSPEC, TW_STATE_START);
                                ChangeTvWallSwitch(&tMt, byTvWallId, byTvChlLp, byMemberType, TW_STATE_START);
                            }
                        }
                    }
                }
            }
		}
	
		//������Ƶ��������	
		if( tLogicChnnl.GetMediaType() == MODE_AUDIO )
		{
			//��������, ת��dump����ֹicmp
			g_cMpManager.StartRecvMt( tMt, MODE_AUDIO );
            
            //zbq[07/18/2007] SSRC ���޸��ɹ�һ����ͳһ����
			//g_cMpManager.ResetRecvMtSSRC( tMt, MODE_AUDIO );

			//���Ƿ����˼���, ���ǵ�����Ƶʱ�ķ�����Ҳ��˴���
			if(GetLocalSpeaker() == tMt )
			{
				//����Ƶ���迼����Ƶ�������
				ChangeSpeaker( &tMt, FALSE, FALSE );		
			}

            // xsl [8/4/2006] vac������ģʽ�½����ն˼������ͨ�������������������Ľ���
			if ( (  m_tConf.m_tStatus.IsVACing() ||
                    m_tConf.m_tStatus.IsAutoMixing() || 
                    m_tConf.m_tStatus.IsSpecMixing() && tMt == GetLocalSpeaker() )
                 && GetMixMtNumInGrp() < GetMaxMixNum() )
			{
                // guzh [1/31/2007] 8000B ��������
                u16 wError = 0;
                if ( CMcuPfmLmt::IsMtOprSupported( m_tConf, m_tConfAllMtInfo.GetLocalJoinedMtNum(), GetMixMtNumInGrp(), wError ) )
                {
                    AddMixMember( &tMt, DEFAULT_MIXER_VOLUME, FALSE );
                    StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
                                         (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())),
                                         MODE_AUDIO, SWITCH_MODE_SELECT);
                }
                else
                {
                    // ������ʾ�������趨���Ծ���ִ��
                    SwitchDiscuss2SpecMix();
                    wError = ERR_MCU_DISCUSSAUTOCHANGE2SPECMIX;
                    NotifyMcsAlarmInfo(0, wError);  // ��ʾMCS
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
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
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

            //�е���ǽģ�岢����������豸�����ӵĻ�(��֧��ֻ����Ƶ���ն˵���ǽ����������)
/*            u8 byTvWallId = m_tConfEqpModule.GetTvWallEqp().GetEqpId();
            if (m_tConf.GetConfAttrb().IsHasTvWallModule() && 
                g_cMcuVcApp.IsPeriEqpConnected(byTvWallId))
            {
                for(u8 byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++)
                {
                    if (m_tConfEqpModule.IsMtAtTvwallChannel(byLoop, tMt))
                    {
                        ChangeTvWallSwitch(&tMt, byTvWallId, byLoop, TW_MEMBERTYPE_MCSSPEC, TW_STATE_START);
                    }
                }
            }*/
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
		}

		break;
        }
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
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
                MtLog("[ProcMtMcuMediaLoopOpr] mt can't loop in double stream mode.\n");
                return;
            }

            if (!g_cMpManager.StartSwitchToSubMt(tLoopMt, 0, tLoopMt, byMode, SWITCH_MODE_SELECT))
            {
                cServMsg.SetEventId(pcMsg->event+2);
                SendMsgToMt(cServMsg.GetSrcMtId(), pcMsg->event+2, cServMsg);
                MtLog( "[ProcMtMcuMediaLoopOpr] start switch failed, byMode<%d>!\n", byMode );
                return;
            }

            //ack
            cServMsg.SetEventId(pcMsg->event+1);
            SendMsgToMt(cServMsg.GetSrcMtId(), pcMsg->event+1, cServMsg);
            
            NotifyMtReceive(tLoopMt, tLoopMt.GetMtId());

            //�û��ر��
            tMtStatus.SetMediaLoop(byMode, TRUE);
            m_ptMtTable->SetMtStatus(tLoopMt.GetMtId(), &tMtStatus);

            MtLog( "[ProcMtMcuMediaLoopOpr] mt<%d> mode<%d> start media loop.\n", tLoopMt.GetMtId(), byMode );
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
                    if (GetMixMtNumInGrp() < GetMaxMixNum())
                    {
                        AddMixMember( &tLoopMt, DEFAULT_MIXER_VOLUME, FALSE );
				        StartSwitchToPeriEqp(tLoopMt, 0, m_tMixEqp.GetEqpId(), 
					                         (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tLoopMt.GetMtId())), 
									         MODE_AUDIO, SWITCH_MODE_SELECT);	
                    }                    
                }           

                MtLog( "[ProcMtMcuMediaLoopOpr] mt<%d> mode<%d> stop media loop.\n", tLoopMt.GetMtId(), byMode );
            }
	        else
            {
                MtLog( "[ProcMtMcuMediaLoopOpr] mt<%d> mode<%d> not in media loop. ignore it!\n", 
                        tLoopMt.GetMtId(), byMode );
            }
		}
		break;

	default:
		ConfLog( FALSE, "[ProcMtMcuMediaLoopOpr] Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*=============================================================================
    �� �� ���� ProcMtMcuGetH239TokenReq
    ��    �ܣ� �ն˸�MCU�� ��ȡ H239���� Ȩ������
    �㷨ʵ�֣� 
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
	
    	//�ж�˫���ķ���Դ
	if ( CONF_DUALMODE_EACHMTATWILL == m_tConf.GetConfAttrb().GetDualMode() ||
		 ( CONF_DUALMODE_SPEAKERONLY == m_tConf.GetConfAttrb().GetDualMode() &&
		   tMt == GetLocalSpeaker() ) ) 
	{
    }
    else
	{
		Mt2Log( "[ProcMtMcuGetH239TokenReq] MtId.%d, ManuId.%d, DStreamMtId.%d open double stream with Wrong DualMode, NACK !\n", 
			tMt.GetMtId(), m_ptMtTable->GetManuId( tMt.GetMtId() ), m_tDoubleStreamSrc.GetMtId() );	

        cServMsg.SetEventId(MCU_MT_GETH239TOKEN_NACK);	
	    SendMsgToMt( tMt.GetMtId(), MCU_MT_GETH239TOKEN_NACK, cServMsg);
        return;
	}

	//REQUEST RELEASING TOKEN OF LAST MT OWNER
	if( !m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() )
	{
		if( !(m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt) )
		{
			TH239TokenInfo tH239Info;
			tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
			tH239Info.SetSymmetryBreaking( 0 );
			cServMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
			cServMsg.SetMsgBody((u8 *)&m_tH239TokenOwnerInfo.m_tH239TokenMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId(), MCU_MT_RELEASEH239TOKEN_CMD, cServMsg);
		}
	}

	if( !m_tDoubleStreamSrc.IsNull() )
	{
		//SONY:------FIRST OPENCHANNEL,NEXT REQUEST TOKEN
		//CLOSE LAST CHANNELS
		if( !(m_tDoubleStreamSrc == tMt) )
		{
			StopDoubleStream( TRUE, FALSE );
		}
	}	

    cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
	cServMsg.CatMsgBody((u8 *)&tSrcH239Info, sizeof(TH239TokenInfo));	
		
	//POLYCOM7000�ľ�̬˫��ʱ,����ͨ����һ��ʼ�򿪺�һֱ���رգ�ʹ��H239Token���з��ͼ����ر�
	//IF CHANNEL HAS BEEN OPENED, START DOUBLESTREAM,
	//ELSE IF TOKEN HAS BEEN OWNED, START DOUBLESTREAM WHEN RECEIVED OPENLOGICAL_ERQ...
	TLogicalChannel tH239LogicChnnl;
	if( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) && 
		m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ) )
	{
        // xsl [7/20/2006]���Ƿ�ɢ����ʱ��Ҫ���ǻش�ͨ����
        if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsOverSatCastChnnlNum(tMt.GetMtId()))
        {
            ConfLog(FALSE, "[ProcMtMcuGetH239TokenReq] over max upload mt num.\n");
            cServMsg.SetEventId(MCU_MT_GETH239TOKEN_NACK);	
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
	
	// xliang [11/14/2008] polycomMCU����ΪkedaMCU��Token����Ȩ���ʲ�����Token.
	//��ֱ���ڴ˷�TokenOwnerInd�ͱ�ʾ��ǰPolycomMCUҪ��˫�������������...
	if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
		( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
		MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId())) )
	{
		//����Ϸ�������˫��
		TLogicalChannel tFstLogicChnnl;
		TLogicalChannel tSecLogicChnnl;
		
		if ( m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tFstLogicChnnl, FALSE) )
		{
			u16 wFstBR = tFstLogicChnnl.GetFlowControl();
			u16 wDailBR = m_ptMtTable->GetDialBitrate(tMt.GetMtId());
			u16 wSecBR = wDailBR - wFstBR;
			
			m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_SECVIDEO, &tSecLogicChnnl, FALSE);
			tSecLogicChnnl.SetFlowControl(wSecBR);
			
			if ( wSecBR * 100 / wDailBR > 10 )
			{
				//��polycomMCU�����˫�����������
				if ( m_tDoubleStreamSrc == tMt )
				{
					CallLog( "[ProcMtMcuOwnH239TokenNotify] DS has been started, ignore the same Ind\n" );
				}
				else
				{
					//�Ƿ����㵱ǰ��˫��Լ��
					if ( CONF_DUALMODE_EACHMTATWILL == m_tConf.GetConfAttrb().GetDualMode() ||
						( CONF_DUALMODE_SPEAKERONLY == m_tConf.GetConfAttrb().GetDualMode() &&
						tMt == GetLocalSpeaker() ))
					{
						StartDoubleStream(tMt, tSecLogicChnnl);	
						CallLog( "[ProcMtMcuOwnH239TokenNotify] tMt.%d, wSecBR.%d, wDailBR.%d, StartDS\n",
							tMt.GetMtId(), wSecBR, wDailBR );
					}
					else
					{
						CallLog( "[ProcMtMcuOwnH239TokenNotify] tMt.%d, wSecBR.%d, wDailBR.%d, should but no StartDS due to dual mode.%d\n",
							tMt.GetMtId(), wSecBR, wDailBR, m_tConf.GetConfAttrb().GetDualMode() );
					}
				}
			}
			else
			{
				CallLog(  "[ProcMtMcuOwnH239TokenNotify] tMt.%d, wSecBR.%d, wDailBR.%d, No StartDS\n",
					tMt.GetMtId(), wSecBR, wDailBR );
			}
		}
		
		//�ͷŵ�ǰ��˫��Դ(��polycomMCU)
		if (!m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() &&
			!(m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt))
		{
			TH239TokenInfo tH239Info;
			tH239Info.SetChannelId( tSrcH239Info.GetChannelId() );
			tH239Info.SetSymmetryBreaking( 0 );
			cServMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
			cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( tMt.GetMtId(), MCU_MT_RELEASEH239TOKEN_CMD, cServMsg);
		}
		return;
	}

	if( m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() )
	{
		return;
	}
	//SEND OTHER MT TOKEN INDICATION
	else if( m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt )
	{
		NotifyH239TokenOwnerInfo( NULL );
	}
	//REQUEST RELEASING TOKEN OF LAST MT OWNER
	else
	{
		TH239TokenInfo tH239Info;
		tH239Info.SetChannelId( tSrcH239Info.GetChannelId() );
		tH239Info.SetSymmetryBreaking( 0 );
		cServMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
		cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
		cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
		SendMsgToMt( tMt.GetMtId(), MCU_MT_RELEASEH239TOKEN_CMD, cServMsg);
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
		m_tH239TokenOwnerInfo.Clear();

		//POLYCOM7000�ľ�̬˫��ʱ,����ͨ����һ��ʼ�򿪺�һֱ���رգ�ʹ��H239Token���з��ͼ����ر�
		if( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) && 
			m_tDoubleStreamSrc == tMt )
		{
			StopDoubleStream( FALSE, FALSE );
		}

		// xliang [11/14/2008] ͬʱ�ͷ� POLYMCU ��TOKEN
		for ( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
		{
			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byMtId) &&
				( MT_TYPE_SMCU == m_ptMtTable->GetMtType(byMtId) ||
				MT_TYPE_MMCU == m_ptMtTable->GetMtType(byMtId) ) )
			{
				CServMsg cMsg;
				cMsg.SetEventId(MCU_POLY_RELEASEH239TOKEN_CMD);
				SendMsgToMt(byMtId, MCU_POLY_RELEASEH239TOKEN_CMD, cServMsg);
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
				g_cMpManager.StartSwitchToSubMt(m_tDoubleStreamSrc, 0, tPolyMt, MODE_SECVIDEO);
			}
			else
			{
				ConfLog( FALSE, "[ProcPolyMCUH239Rsp] PolyMcu.%d Fwd chnnl should not be closed, reinvite it, please\n", tPolyMt.GetMtId() );
			}
			CallLog( "[ProcPolyMCUH239Rsp] PolyMcu.%d accept TokenReq, POLY_MCU_GETH239TOKEN_ACK received\n", tPolyMt.GetMtId() );
			break;
		}
	case POLY_MCU_GETH239TOKEN_NACK:		//��ȡPolyMCU��H239TOKEN �ܾ�Ӧ��
		{
			ConfLog(FALSE, "[ProcPolyMCUH239Rsp] GETH239TOKEN_NACK\n");
			break;
		}
	case POLY_MCU_OWNH239TOKEN_NOTIF:		//PolyMCU֪ͨ��ǰ��TOKEN��ӵ����
		{
			//FIXME����Ϣ����ʲô����ӡһ�£�Ҫ��Ҫ����Ӧ
			//˭ӵ����H239
			ConfLog(FALSE, "[ProcPolyMCUH239Rsp] POLY_MCU_OWNH239TOKEN_NOTIF\n");
			break;
		}
	case POLY_MCU_RELEASEH239TOKEN_CMD:		//PolyMCU�ͷ�H329TOKEN ����
		{
			//˫���͵�һ·Flowctrl��ȥ
			ConfLog(FALSE, "[ProcPolyMCUH239Rsp] POLY_MCU_RELEASEH239TOKEN_CMD\n");
			break;
		}
	default:
		ConfLog( FALSE, "[ProcPolyMCUH239Rsp] unexpected msg.%d<%s>\n", pcMsg->event, OspEventDesc(pcMsg->event) );
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
	
	CallLog( "Mt%d close Out.%d logical channel received!\n", tMt.GetMtId(), bOut );

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
			// xliang [12/19/2008] �����ϴε�˫��ԴMT���ڴ˴�OLC(ds) 
			if(tMt.GetMtId() == m_byLastDsSrcMtId)
			{
				TCapSupport tCapSupport;
				
				if(!m_ptMtTable->GetMtCapSupport( tMt.GetMtId(), &tCapSupport ))
				{
					ConfLog(FALSE,"[ProcMtMcuCloseLogicChnnlNotify]can't get last Ds Src Mt.%u's Cap, Fail to Open ds olc!\n",tMt.GetMtId());
				}
				else
				{
					McuMtOpenDoubleStreamChnnl(tMt, m_tLogicChnnl, tCapSupport);
					CallLog("[ProcMtMcuCloseLogicChnnlNotify]after delay,start ds! \t tLogicChnnl's VideoFormat is%u.\n",m_tLogicChnnl.GetVideoFormat());
				}
			}
		}
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
			m_tCascadeMMCU.SetNull();
			m_tConfAllMtInfo.m_tMMCU.SetNull();
            m_tConfInStatus.SetNtfMtStatus2MMcu(FALSE);
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
			if( m_tDoubleStreamSrc == tMt )
			{
				StopDoubleStream( FALSE, FALSE );
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
            if ( !m_tConf.GetConfAttrb().IsSatDCastMode() )
            {
                g_cMpManager.StopSwitchToSubMt( tMt, MODE_SECVIDEO, TRUE  );
            }
		}
	}
	else if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
	{
		byChannelType = LOGCHL_VIDEO;
        // ���ǻ��鲻ͣ���� [6/2/2006]
        if ( !m_tConf.GetConfAttrb().IsSatDCastMode() )
        {
		    if(bOut)
		    {
			    StopSwitchToSubMt( tMt.GetMtId(), MODE_VIDEO );
		    }
		    else
		    {
		    }
        }
	}
	else if( MODE_AUDIO == tLogicChnnl.GetMediaType() )
	{
		byChannelType = LOGCHL_AUDIO;
        // ���ǻ��鲻ͣ���� [6/2/2006]
        if ( !m_tConf.GetConfAttrb().IsSatDCastMode() )
        {
		    if(bOut)
		    {
			    StopSwitchToSubMt( tMt.GetMtId(), MODE_AUDIO );
		    }
		    else
		    {
		    }
        }
	}

	m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannelType, &tNullLogicChnnl, bOut );

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
void CMcuVcInst::ProcMtCapSet( TMt tMt, TCapSupport &tCapSupport, const TCapSupportEx &tCapSupportEx )
{
    TLogicalChannel tDataLogicChnnl;
    TLogicalChannel tLogicalChannel;
    TLogicalChannel tH239LogicChnnl;
    

    //zbq[04/09/2009] Tandberg ��������
    if (MT_MANU_TAIDE == m_ptMtTable->GetManuId((tMt.GetMtId())) &&
        IsHDConf(m_tConf))
    {
        if (g_cMcuVcApp.IsSendFakeCap2Taide())
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
        }
        else if (g_cMcuVcApp.IsSendFakeCap2TaideHD())
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_HD720);
        }
    }

	TMt tNullMt;
	tNullMt.SetNull();
	u8 byDriId = m_ptMtTable->GetDriId(tMt.GetMtId());
#ifdef _LINUX_
	// xliang [10/27/2008] ����HD MT����Ƕ��������µĴ���
	if(tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 &&
		(tCapSupport.GetMainVideoResolution() == VIDEO_FORMAT_HD1080 ||
		 tCapSupport.GetMainVideoResolution() == VIDEO_FORMAT_HD720) )
	{
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
		if ( g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum +1 > g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_byMaxMtNum )
		{
			// xliang [2/16/2009]  ���ж�Ӧ�ý�����,��mtadp���ѽ��й��ˡ�
			CallLog("[ProcMtCapSet]Take MCU as 2 into account of Mtadp access capabilty: fail!\n");
			NotifyMcsAlarmInfo( 0, ERR_MCU_CALLHDMTFAIL);
			RemoveJoinedMt( tMt, TRUE, MTLEFT_REASON_NORMAL, TRUE);
			return;
		}
		else
		{
			g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum++;
		}
	}
    CallLog( "[ProcMtCapSet] Mt main video cap: type.%d, Res.%d \n", tCapSupport.GetMainVideoType(), tCapSupport.GetMainVideoResolution() );

	u32	dwRcvIp;
	u16	wRcvPort;
	if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
	{
		dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
		if( dwRcvIp == 0 )
		{
			return;			    
		}
		wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
		m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
	}

	TLogicalChannel	tAudioLogicChnnl;
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
    
    //zbq[10/13/2007] FEC�������㣬�ܾ����ն˵�ͨ��
    BOOL32 bFECOpenAudioChnnl = TRUE;
    BOOL32 bFECOpenVideoChnnl = TRUE;
    BOOL32 bFECOpenDVideoChnnl = TRUE;

	CServMsg cServMsg;	
	//���ʹ��ն������߼�ͨ����Ϣ
	if( bFECOpenAudioChnnl &&
        tAudioLogicChnnl.GetChannelType() != MEDIA_TYPE_NULL )
	{
		u16 wAudioBand = GetAudioBitrate( m_tConf.GetMainAudioMediaType() );
		tAudioLogicChnnl.SetFlowControl( wAudioBand );
		tAudioLogicChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
		tAudioLogicChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 3 );
		tAudioLogicChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
		tAudioLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, tAudioLogicChnnl.GetChannelType()));
		
		cServMsg.SetMsgBody( ( u8 * )&tAudioLogicChnnl, sizeof( tAudioLogicChnnl ) );
		SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );

		//��ӡ��Ϣ
		CallLog( "Open audio logicChannel request send to Mt%d, type: %s\n", 
				tMt.GetMtId(), GetMediaStr( tAudioLogicChnnl.GetChannelType() ) );
		
	}
	else
	{
		VCSMTAbilityNotif(tMt, MODE_AUDIO);
		//��ӡ��Ϣ
		CallLog( "[ProcMtCapSet] Mt%d not support any conf audio media type, NO open audio logicChannel request send to it!\n", tMt.GetMtId() );
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
            wMtMaxRecvBitrate = max( wMtMaxRecvBitrate, tMtSecSCS.GetVideoMaxBitRate() );
        }
        // guzh [7/16/2007] �ϰ汾�ն˵���Ƶ�߼�ͨ����������-64
        wMtMaxRecvBitrate += 64;

        wMtMaxRecvBitrate = min ( wMtMaxRecvBitrate, wCallRate );
    }
    else
    {
        wMtMaxRecvBitrate = wCallRate;
    }        
    
    //zbq[12/18/2007]�߼�ͨ���򿪵��ж��������д�������
    if ( wMtMaxRecvBitrate < m_ptMtTable->GetDialBitrate(tMt.GetMtId()))
    {
        bOpenVideoChnnl = FALSE;
        CallLog("Open Mt.%d video logic chnnl failed due to ChnnlRcvBR.%d, DailBR.%d\n",
            tMt.GetMtId(), wMtMaxRecvBitrate, m_ptMtTable->GetDialBitrate(tMt.GetMtId()));
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
        CallLog("Open Mt.%d video logic chnnl failed due to src<%d-%s> not matched!\n", 
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
        
		u8 byChnRes = 0;
		if (!GetMtMatchedRes(tMt.GetMtId(), tVideoLogicChnnl.GetChannelType(), byChnRes))
		{
			//zbq[04/22/2009] ��������ʹ��Ҳ�������������£����°������������
            if (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType())
            {
                tVideoLogicChnnl.SetChannelType(m_tConf.GetSecVideoMediaType());
                tVideoLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, tVideoLogicChnnl.GetChannelType()));
                byChnRes = m_tConf.GetSecVideoFormat();

                CallLog("[ProcMtCapSet] no proper res for mt(%d) with mediatype(%d), try SecVidCap!\n", 
                    tMt.GetMtId(), tVideoLogicChnnl.GetChannelType());
            }
            else
            {
                bOpenVideoChnnl = FALSE;
                CallLog("[ProcMtCapSet] no proper res for mt(%d) with mediatype(%d)!\n", 
                    tMt.GetMtId(), tVideoLogicChnnl.GetChannelType());
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
				CallLog("[ProcMtCapSet] Mt.%d 's video open res has been adjusted from 4CIF to 720p!\n");
			}

			tVideoLogicChnnl.SetVideoFormat(byChnRes);
			cServMsg.SetMsgBody( ( u8 * )&tVideoLogicChnnl, sizeof( tVideoLogicChnnl ) );
			cServMsg.CatMsgBody( &byChnRes, sizeof(u8) );
			SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
			
			//��ӡ��Ϣ
			CallLog( "[ProcMtCapSet] Open Video LogicChannel Request send to Mt%d, type: %s, res.%d\n", 
				tMt.GetMtId(), GetMediaStr( tVideoLogicChnnl.GetChannelType() ), tVideoLogicChnnl.GetVideoFormat() );
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
		CallLog( "[ProcMtCapSet] Mt%d not support any conf video media type, NO open video logicChannel request send to it!\n", tMt.GetMtId() );
	}


	//���ն˵�˫��ͨ�� 
	if( bFECOpenVideoChnnl &&
        bOpenVideoChnnl &&
        //m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) &&
		!m_tDoubleStreamSrc.IsNull() &&
		m_tConf.GetBitRate() >= 64 )
	{	
		// xliang [11/14/2008] polycomMCU��˫��ͨ���Ѿ��򿪣��˴������ظ�
		if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
			( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
			MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ) )
		{
			CallLog("PolyMcu.%d's secvideo chan has been open somewhere else, ignore it\n");
		}
		if (MT_TYPE_MMCU == tMt.GetMtType())
		{
			CallLog("Mt%d is mmcu, settimer open double video chan. \n", tMt.GetMtId());
			SetTimer(MCU_SMCUOPENDVIDEOCHNNL_TIMER, 3000, tMt.GetMtId());
		}
		else
		{
            BOOL32 bDStreamSrcLegal = FALSE;

            //zbq[01/11/2008] ˫��������Ҫαװ����ͨ��
            if ( TYPE_MCUPERI == m_tDoubleStreamSrc.GetType() &&
                 EQP_TYPE_RECORDER == m_tDoubleStreamSrc.GetEqpType() )
            {
                tLogicalChannel.SetChannelType( m_tPlayEqpAttrib.GetDVideoType() );
                tLogicalChannel.SetSupportH239( m_tConf.GetCapSupport().IsDStreamSupportH239() );
				// fxh ��Ҫ˫��Դ�ķֱ�����Ϣ,�����ں�����ͨ��ʱ�ж��Ƿ�Ŀ�Ķ���֮ƥ��
				u16 wFileDSW = 0; 
				u16 wFileDSH = 0;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
				u8  byFileDSType = 0;
				m_tPlayFileMediaInfo.GetDVideo(byFileDSType, wFileDSW, wFileDSH);
		     	u8 byFileDSRes = GetResByWH(wFileDSW, wFileDSH);
				tLogicalChannel.SetVideoFormat(byFileDSRes);
                bDStreamSrcLegal = TRUE;
            }
            else
            {
                if ( m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ))
                {
                    bDStreamSrcLegal = TRUE;
                }
                else
                {
                    CallLog("[ProcMtCapSet] Get DS Src.%d's logic chnnl failed\n", m_tDoubleStreamSrc.GetMtId());
                }
            }

            if ( bDStreamSrcLegal )
            {
                McuMtOpenDoubleStreamChnnl(tMt, tLogicalChannel, tCapSupport);
            }
		}
	}
    else
    {
        CallLog( "Mt%d not support any conf DVideo media type, NO open DVideo logicChannel request send to it!\n", tMt.GetMtId() );
    }

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
		CallLog( "Open Data LogicChannel Request send to Mt%d, type: %s\n", 
			tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );			
	}

	// ��������ͨ��
    // guzh [1/19/2007] �����ж��Ƿ���������������,�û����¼�MCU�Ƿ���
	u8 byMtType = m_ptMtTable->GetMtType(tMt.GetMtId());
	if ( byMtType == MT_TYPE_SMCU &&
	 	 tCapSupport.IsSupportMMcu() &&
		 m_tConf.GetConfAttrb().IsSupportCascade() &&
         !g_cMcuVcApp.IsCasConfOverCap() &&
         m_tConfAllMtInfo.GetCascadeMcuNum() < MAXLIMIT_CONF_SMCU &&
		 m_tConfAllMtInfo.GetCascadeMcuNum() < MAXNUM_SUB_MCU )
	{
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
        
        // �ϼ�MCU���¼�����ͨ�����Կ��ܷ���ʧ�ܵ�������������´�[01/11/2007_zbq]
        m_abyCasChnCheckTimes[tMt.GetMtId()-1] = 1;
        SetTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER, TIMESPACE_WAIT_CASCADE_CHECK );
		
		//��ӡ��Ϣ
		CallLog( "Open Data LogicChannel Request send to Mt%d, type: %s\n", 
				tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );			
	}
    else
    {
        CallLog( "Not send MMcu OLC to Mt%d for MtType.%d, RemoteCap.%d, ConfAtrb.%d, CasConfNum.%d, CasMcuNum.%d \n", 
                 tMt.GetMtId(),
                 byMtType,
                 tCapSupport.IsSupportMMcu(),
                 m_tConf.GetConfAttrb().IsSupportCascade(),
                 g_cMcuVcApp.GetMcuCasConfNum(),
                 m_tConfAllMtInfo.GetCascadeMcuNum() );
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
			CallLog( "Open Data LogicChannel Request send to Mt%d, type: %s as Master \n", 
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
					CallLog( "Open Data LogicChannel Request send to Mt%d, type: %s as master with no address ! \n", 
						      tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );
				}
				//��������, �ȴ��ϼ�MCU�򿪱���ͨ��
				else
				{
					//��ӡ��Ϣ
					CallLog( "Open Data LogicChannel Request send to Mt%d, type: %s as slave with no address ! \n", 
						      tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );
				}				
			}
			else
			{
				CallLog("Get DCS Address 0x%x:%d failed in Open T120 channel !\n", dwDcsIp, wDcsPort );
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
void CMcuVcInst::ProcTimerReopenCascadeChannel()
{
    BOOL32 bNonOpenStill = FALSE;
    BOOL32 bFailedThreeTimes = FALSE;
    TLogicalChannel tDataLogicChnnl;
    for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
    {
        if ( m_abyCasChnCheckTimes[byMtId-1] == 0 )
        {
            continue;
        }
        //��ೢ������ (3 * 10 + 3 * 60) s
        if ( m_abyCasChnCheckTimes[byMtId-1] <= 2*MAXNUM_CHECK_CAS_TIMES )
        {
            m_abyCasChnCheckTimes[byMtId-1] ++;

            //�����ٴδ򿪼�������ͨ��
            u8 byMtType = m_ptMtTable->GetMtType(byMtId);
            // guzh [1/19/2007] �����ж��Ƿ���������������,�û����¼�MCU�Ƿ���
	        if ( byMtType == MT_TYPE_SMCU &&
                !g_cMcuVcApp.IsCasConfOverCap() &&
                m_tConfAllMtInfo.GetCascadeMcuNum() < MAXLIMIT_CONF_SMCU &&
		        m_tConfAllMtInfo.GetCascadeMcuNum() < MAXNUM_SUB_MCU )
            {
                tDataLogicChnnl.SetMediaType( MODE_DATA );
                tDataLogicChnnl.SetChannelType( MEDIA_TYPE_MMCU );
    
                //�򿪼���ͨ��ʱ����Ӧtcp������ip��port����Ϊ����弶����ַ
                TMt tDstMt = m_ptMtTable->GetMt(byMtId);
                u32 dwCasIp = g_cMcuVcApp.GetMtAdpIpAddr(tDstMt.GetDriId()); //������
                tDataLogicChnnl.m_tRcvMediaChannel.SetIpAddr(dwCasIp);
    
                tDataLogicChnnl.m_tRcvMediaChannel.SetPort( g_cMcuVcApp.GetSMcuCasPort() );
                
                CServMsg cServMsg;
                cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );			
                SendMsgToMt( byMtId, MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
                
                if ( m_abyCasChnCheckTimes[byMtId-1] > MAXNUM_CHECK_CAS_TIMES )
                {
                    bFailedThreeTimes = TRUE;
                }
                bNonOpenStill = TRUE;
                ConfLog( FALSE, "[ProcTimerReopenCascadeChannel] Open Mt.%d Cascade Channel failed %d time(s), retry now !\n", 
                                 byMtId, m_abyCasChnCheckTimes[byMtId-1] );
            }
        }
        else
        {
            m_abyCasChnCheckTimes[byMtId-1]  = 0;
            ConfLog( FALSE, "[ProcTimerReopenCascadeChannel] Open Mt.%d Cascade Channel failed entirely !\n", byMtId );
        }
    }

    if ( bNonOpenStill )
    {
        if ( bFailedThreeTimes )
        {
            SetTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER, TIMESPACE_WAIT_CASCADE_CHECK * 6 );
        }
        else
        {
            SetTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER, TIMESPACE_WAIT_CASCADE_CHECK );
        }
    }
    else
    {
        KillTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER );
    }
	return;
}

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
    TCapSupport tCapSupport = *(TCapSupport *)( cServMsg.GetMsgBody() );
    TCapSupportEx tCapSupportEx = *(TCapSupportEx*)(cServMsg.GetMsgBody()+sizeof(TCapSupport));
    
	u32 dwBitRate = 0;

	switch( CurState() )
	{
	case STATE_ONGOING:
		{
			//���ø��ն˵�������
			u8 byMtId = tMt.GetMtId();
			m_ptMtTable->SetMtCapSupport(byMtId, &tCapSupport);
			m_ptMtTable->SetMtCapSupportEx(byMtId, &tCapSupportEx);
            if (g_bPrintMtMsg)
            {
                MtLog("[ProcMtMcuCapSetNotif]capability specified by mt:\n");
                tCapSupport.Print();
            }
			if (m_ptMtTable->IsMtCapSpecByMCS(byMtId))
			{
				TCapSupport tCap;
				TDStreamCap tDStreamCap;
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

				m_ptMtTable->SetMtCapSupport(byMtId, &tCap);
				
				m_ptMtTable->GetMtCapSupport(byMtId, &tCapSupport);			
			}

            if (g_bPrintMtMsg)
            {
                MtLog("[ProcMtMcuCapSetNotif]final mt capability:\n");
                tCapSupport.Print();
            }
			
			//ȷ�����ն��Ѿ����
			if( m_tConfAllMtInfo.MtJoinedConf( byMtId ) )
			{
				//�����ն�������
				ProcMtCapSet( tMt, tCapSupport, tCapSupportEx );

				//��������
				if ( !m_ptMtTable->IsNotInvited( byMtId ) )
				{
					//��Ҫ�õ���ַ�Ͷ˿�
				}
				//���˱���
				else
				{
					//�ն˺ͻ����֧�����ݹ���
					if ( tCapSupport.IsSupportT120() && 
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
		ConfLog( FALSE, "CMcuVcInst: ProcMtMcuCapSetNotif() Wrong message %u(%s) received in state %u!\n", 
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
		ConfLog( FALSE, "CMcuVcInst: ProcMtMcuMtAliasNotif() Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );		
	}
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
====================================================================*/
void CMcuVcInst::ProcMtMcuConnectedNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	u8 byManuId = *(u8*)cServMsg.GetMsgBody();
	TCapSupport tCapSupport;
	TCapSupportEx tCapSupportEx;
	TSimCapSet tSimCapSet;

	//���ó�����Ϣ
	m_ptMtTable->SetManuId( tMt.GetMtId(), byManuId );
	CallLog( "Mt%d: 0x%x(Dri:%d) connected and it's manufacture is %s!\n", 
		     tMt.GetMtId(), m_ptMtTable->GetIPAddr(tMt.GetMtId()), tMt.GetDriId(), GetManufactureStr( byManuId ) );


    // xsl [10/27/2006] ��ɢ���鲻֧�ּ���
    if (m_tConf.GetConfAttrb().IsSatDCastMode() &&
        MT_TYPE_MT != m_ptMtTable->GetMtType(tMt.GetMtId()))
    {
        SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
        CallLog("[ProcMtMcuConnectedNotif] sat conf don't support cascade.\n");  
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
        else if ( wSecBitrate == wDialBitrate )
        {
            tSim = tCapSupport.GetSecondSimCapSet();
            tCapSupport.SetMainSimCapSet(tSim);
            tSim.Clear();
            tCapSupport.SetSecondSimCapSet(tSim);
        }
        else
        {
            tSim.Clear();
            tCapSupport.SetSecondSimCapSet(tSim);
        }           
    }    
#endif    
    
    //zbq[02/04/2009] �����������Polycom����480p�ı���
    if (g_cMcuVcApp.IsSendFakeCap2Polycom() &&
        MT_MANU_POLYCOM == m_ptMtTable->GetManuId((tMt.GetMtId())) &&
        IsHDConf(m_tConf))
    {
        tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_4CIF);
    }

    if (MT_MANU_TAIDE == m_ptMtTable->GetManuId((tMt.GetMtId())) &&
        IsHDConf(m_tConf))
    {
        if (g_cMcuVcApp.IsSendFakeCap2Taide())
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
        }
        else if (g_cMcuVcApp.IsSendFakeCap2TaideHD())
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_HD720);
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

	// MCUǰ�����, zgc, 2007-09-27
	cServMsg.CatMsgBody( ( u8 * )&tCapSupportEx, sizeof( tCapSupportEx ) );
	
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
	
	CallLog( "Mt.%d Disconnected because of Reason \"%s\"(%d).\n", tMt.GetMtId(), 
             GetMtLeftReasonStr(byReason), byReason );
	
	switch( CurState() )
	{
		
	case STATE_ONGOING:		
        
		// ��ǰ������ն˶����������kill��ʱ�������������һ����������
		if(VCS_CONF == m_tConf.GetConfSource() &&
		   tMt == m_cVCSConfStatus.GetReqVCMT())
		{
			KillTimer(MCUVC_VCMTOVERTIMER_TIMER);

			TMt tNull;
			tNull.SetNull();
			m_cVCSConfStatus.SetReqVCMT(tNull);
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
				  (VCS_CONF == m_tConf.GetConfSource() && byReason != MTLEFT_REASON_REJECTED))
            {
            }
            else
            {
                //����ն˺���ģʽ
                m_ptMtTable->SetCallMode( tMt.GetMtId(),  CONF_CALLMODE_NONE );
            
                //�������ģʽ����������չ�ṹ����Ҫ����ӦRejectԭ���µ��ն���Ϣ���ܵ����������
                //ʵ������Normalԭ���£��ն���ᴦ��Ҳ�ᷢ����Ϣ����Ϊ��֤��ض��ն�״̬�����һ���ԣ����ﲻ����ǡ�
                CServMsg cServMsg;
                cServMsg.SetEventId( MCU_MCS_CALLMTMODE_NOTIF );
                cServMsg.SetMsgBody( (u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt) );
                SendMsgToAllMcs( cServMsg.GetEventId(), cServMsg );
            }
        }

		if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
		{
			NotifyMcuDropMt(tMt);				
		}
		
		if( MTLEFT_REASON_NORMAL != byReason  )
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
								OspPrintf(TRUE, FALSE, "m_ptMtTable->GetMtAlias Fail, mtid = %d\n", tMt.GetMtId());
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
                     m_ptMtTable->IsNotInvited(tMt.GetMtId())
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
			SendMtListToMcs( LOCAL_MCUID );
		}

        // xsl [11/8/2006] �ͷŽ�����ת������Դ
        ReleaseMtRes(tMt.GetMtId());
		break;

	default:
		ConfLog( FALSE, "[ProcMcuMtDisconnectedNotif] Wrong message %u(%s) received in state %u!\n", 
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

	// ��ǰ������ն˺���ʧ�ܣ������kill��ʱ�������������һ����������
	if(VCS_CONF == m_tConf.GetConfSource() &&
	   tMt == m_cVCSConfStatus.GetReqVCMT())
	{
		KillTimer(MCUVC_VCMTOVERTIMER_TIMER);

		TMt tNull;
		tNull.SetNull();
		m_cVCSConfStatus.SetReqVCMT(tNull);
	}

	u8* pbyMsgBody = cServMsg.GetMsgBody();
	pbyMsgBody += sizeof(TMtAlias);
	u8 byReason = *(u8*)pbyMsgBody;
	pbyMsgBody += sizeof(u8);

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
		tMt == m_tConf.GetChairman() && 
		MTLEFT_REASON_BUSYEXT == byReason)
	{
		byAutoForceCall = TRUE;
	}
	else
	{
	    SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg ); 
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
						OspPrintf(TRUE, FALSE, "m_ptMtTable->GetMtAlias Fail, mtid = %d\n", tMt.GetMtId());
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
		byConfLevel != m_tConf.GetConfLevel())
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
				wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
				m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
			}

			//��������ͨ��
			if( m_ptMtTable->GetDialAlias( tMt.GetMtId(), &tMtAlias ) && 
				mtAliasTypeH320ID == tMtAlias.m_AliasType && 
				m_tConf.GetConfAttrb().IsSupportCascade() &&
				m_tConfAllMtInfo.GetCascadeMcuNum() < MAXNUM_SUB_MCU )
			{
				tDataLogicChnnl.SetMediaType( MODE_DATA );
				tDataLogicChnnl.SetChannelType( MEDIA_TYPE_MMCU );
				
				tDataLogicChnnl.m_tRcvMediaChannel.SetIpAddr(dwRcvIp);
				tDataLogicChnnl.m_tRcvMediaChannel.SetPort(wRcvPort+9);
				
				cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );			
				SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
				
				//��ӡ��Ϣ
				CallLog( "Open Data LogicChannel Request send to Mt%d, type: %s\n", 
					      tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );
			}

			SendMtListToMcs( LOCAL_MCUID );

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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
        /*
        // License �����������
        if(g_cMcuVcApp.IsMtNumOverLicense())
        {       
			//����֪ͨ��س������������澯 zgc 06-12-21
			cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
			cServMsg.SetErrorCode(ERR_MCU_OVERMAXCONNMT);
			
			SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg );
			
            cServMsg.SetMsgBody();
            SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);

            CallLog("[ProcMtMcuMtJoinNotif] Licensed Max connected mt num(%d) is overflow.\n", 
                     g_cMcuVcApp.GetPerfLimit().m_wMaxConnMtNum );
            return;
        }
        // xsl [10/20/2006] ���ﵽMcu LicenseʧЧ���ڣ� �������ն����
        if (g_cMcuVcApp.IsMcuExpiredDate())
        {
            cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
            cServMsg.SetErrorCode(ERR_MCU_OVERMAXCONNMT);
            
            SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg );
            
            cServMsg.SetMsgBody();
            SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
            
            CallLog("[ProcMtMcuMtJoinNotif] MCU License EXPIRED.\n");  
            
            return;
        }
        
		if ( g_cMcuVcApp.GetPerfLimit().IsLimitMtNum() )
		{
			u8 byDriId = tMt.GetDriId();
            
            // guzh [1/16/2007] �ж��ն������Ƿ���
            u8 byMtNum = 0;
            u8 byMcuNum = 0;
            BOOL32 bOverCap = g_cMcuVcApp.GetMtNumOnDri( byDriId, TRUE, byMtNum, byMcuNum );
			if ( bOverCap )
			{
				cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
				cServMsg.SetErrorCode(ERR_MCU_OVERMAXCONNMT);
				cServMsg.SetConfId( m_tConf.GetConfId() );
				SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg );

				cServMsg.SetMsgBody();
				SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);

				CallLog( "[ProcMtMcuMtJoinNotif] mtadp.%d ability(Max=%d) is full! \n", 
                        byDriId,
                        g_cMcuVcApp.GetMtAdpSupportMtNum(byDriId));
				return;
			}
		}        
        */

        // xsl [11/16/2006] ������֧��gk�Ʒѣ�����֤ʧ�ܣ��������ն����
        if (m_tConf.IsSupportGkCharge() && !m_tConf.m_tStatus.IsGkCharge())
        {
            SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
            CallLog("[ProcMtMcuMtJoinNotif] conf %s support gk charge, but gk charge failed.\n", m_tConf.GetConfName());  
            return;
        }
		
		m_ptMtTable->SetMtIsMaster(cServMsg.GetSrcMtId(), byMcuMaster == 1?FALSE:TRUE);
	
		if( m_ptMtTable->GetCallMode(cServMsg.GetSrcMtId()) == CONF_CALLMODE_TIMER )
		{
			m_ptMtTable->SetCallLeftTimes(cServMsg.GetSrcMtId(), m_tConf.m_tStatus.GetCallTimes());
		}

		//��Ҫ����Ļ��飬�Է��������ն���ֱ�ӽ��� 
		if( m_tConf.GetConfAttrb().GetOpenMode() == CONF_OPENMODE_NEEDPWD && 
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
		}

        //���ǻ����ݲ����Ǽ���

    #ifndef _SATELITE_
		//֪ͨ����MCU new mt
        NotifyMcuNewMt(tMt);        
    #endif

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
				CallLog( "Mcu%d password wrong, drop it!\n", tMt.GetMtId() );
			}
			else
			{
				SendMsgToMt( tMt.GetMtId(), MCU_MT_ENTERPASSWORD_REQ, cServMsg );
				CallLog( "Mt%d password wrong, request once more!\n", tMt.GetMtId() );
			}
		}
		else
		{
			DealMtMcuEnterPwdRsp(tMt, pszPwd, nPwdLen);
		}
		break;
		
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
	TCapSupport tCapSupport;
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
			CallLog( "Mcu%d password wrong, drop it!\n", tMt.GetMtId() );
		}
		else
		{
			SendMsgToMt( tMt.GetMtId(), MCU_MT_ENTERPASSWORD_REQ, cServMsg );
			CallLog( "Mt%d password wrong, request once more!\n", tMt.GetMtId() );
		}
	}
	else
	{
		//�������
		AddJoinedMt( tMt );

		//�����ն�������
		m_ptMtTable->GetMtCapSupport( tMt.GetMtId(), &tCapSupport );
        m_ptMtTable->GetMtCapSupportEx( tMt.GetMtId(), &tCapSupportEx);

		ProcMtCapSet( tMt, tCapSupport, tCapSupportEx );

		//������Ƶ��������
		if( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE ) )
		{			
			//��̬�����ı���
			if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
			{
				ChangeVmpStyle( tMt, TRUE );
			}

			//���Ƿ����˼���
			if( m_tConf.HasSpeaker() && GetLocalSpeaker() == tMt )
			{
                // xsl [8/22/2006]���Ƿ�ɢ����ʱ��Ҫ�жϻش�ͨ����
                if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsOverSatCastChnnlNum(tMt.GetMtId()))
                {
                    ConfLog(FALSE, "[DealMtMcuEnterPwdRsp] over max upload mt num. cancel speaker!\n");
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
                    if (GetMixMtNumInGrp() < GetMaxMixNum())
                    {
                        AddMixMember( &tMt, DEFAULT_MIXER_VOLUME, FALSE );
				        StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
					                         (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())), 
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
			m_ptConfOtherMcTable->AddMcInfo( tMt.GetMtId() );	
			m_tConfAllMtInfo.AddMcuInfo( tMt.GetMtId(), tMt.GetConfIdx() );
			
			//֪ͨ���
			cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
			SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
			
			//������ػ��������ն���Ϣ
			cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
			SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
			
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
	TMt	tMt, tMtSrc;;
	TLogicalChannel	tLogicChnnl;
	u16 wBitRate, wMinBitRate;
	TSimCapSet tSrcSimCapSet;
	TSimCapSet tDstSimCapSet;

	TLogicalChannel tFirstLogicChnnl;
	TLogicalChannel tSecLogicChnnl;
	BOOL bStartDoubleStream = FALSE;
    BOOL32 bMtLowedAlready = FALSE;

	switch( CurState() )
	{
	case STATE_ONGOING:

		tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );

		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );
		wBitRate = tLogicChnnl.GetFlowControl();

        //zbq[02/19/2008] �ն˱����Ƿ񱻽��٣���������Դ����µķ��ʹ�������.
        if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
             m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()))
        {
            bMtLowedAlready = TRUE;
			CallLog("bMtLowedAlready is %d\n",bMtLowedAlready);
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
			Mt2Log("[ProcMtMcuFlowControlCmd] mt.%u bLowed: %d (wRcvBandWidth:%d,wBitRate:%d, conf bitrate:%d)\n",
				tMt.GetMtId(), m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()),
				wRcvBandWidth, wBitRate,m_tConf.GetBitRate());
			/*
            if ( m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()) )
            {
                if (wBitRate > wRcvBandWidth)
				{
					if (abs(wBitRate-wRcvBandWidth)*100 / wRcvBandWidth < 20)
					{
						m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
					}
				}
				else
				{
					m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
				}
            }
            else
            {
				if (wBitRate > wRcvBandWidth)
				{
					if ( abs(wBitRate-wRcvBandWidth)*100 / wRcvBandWidth > 20 )
					{
						m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), TRUE);
					}
				}
				else
				{
					m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
				}
            }
*/
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
				CallLog( "[ProcMtMcuFlowControlCmd] PolycomMcu.%d flowctrl received\n", tMt.GetMtId() );
			}
			return;
		}
		
		if (wBitRate > m_tConf.GetBitRate())
		{
			wBitRate = m_tConf.GetBitRate();
			CallLog("wBitRate is larger than conf bitrate, modify it equal to conf's bitrate! \n");
		}

        // xsl [8/7/2006] �¼�mcu���͵ĵ�һ·flowcontrol������
		if( MODE_SECVIDEO != tLogicChnnl.GetMediaType() &&
			m_ptMtTable->GetManuId( tMt.GetMtId() ) == MT_MANU_KDCMCU )
		{
			if (m_tCascadeMMCU.IsNull() || m_tCascadeMMCU.GetMtId() != tMt.GetMtId())
            {
                Mt2Log("[ProcMtMcuFlowControlCmd] ignore smcu MtReqBitrate :%d from mcu<%d>\n",
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
                   wBitRate > GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())) &&
                   !bMtE1Lowed) )
            {
                Mt2Log("[ProcMtMcuFlowControlCmd] same bitrate.%d for mt.%d!\n", wBitRate, tMt.GetMtId());
                return;
            }
            if ( !bMtE1Lowed )
            {
                m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_VIDEO );
                Mt2Log("[ProcMtMcuFlowControlCmd] rcv fst MtReqBitrate :%d from mt<%d>\n", wBitRate, tMt.GetMtId());

                //zbq[02/19/2008] ������ѡ������Դ���ն˴�������,�������䷢�ʹ���
                if ( bMtLowedAlready )
                {
                    //˫��
                    if ( tMt == m_tDoubleStreamSrc )
                    {
                        TMt tMtNull;
                        tMtNull.SetNull();
                        RestoreVidSrcBitrate(tMtNull, MODE_SECVIDEO);
                        Mt2Log("[ProcMtMcuFlowControlCmd] E1 Lowed Mt restore, adjust DS Src.\n");
                    }
                    //��Ƶѡ������
                    for( u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++)
                    {
                        if ( !m_tConfAllMtInfo.MtJoinedConf(byMtIdx) )
                        {
                            continue;
                        }
                        TMt tMtSrc;
                        if ( m_ptMtTable->GetMtSrc(byMtIdx, &tMtSrc, MODE_VIDEO) &&
                             tMt == tMtSrc )
                        {
                            TMt tMtRcv = m_ptMtTable->GetMt(byMtIdx);
                            RestoreVidSrcBitrate(tMtRcv, MODE_VIDEO);
                            Mt2Log("[ProcMtMcuFlowControlCmd] E1 Lowed Mt restore, adjust FstVid Src.\n");
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
                    SetTimer( MCUVC_SENDFLOWCONTROL_TIMER, 1200 );
                    Mt2Log("[ProcMtMcuFlowControlCmd] CurMtReqBR.%d while RealReqBR.%d, adjust for E1MtLowed to Fst.%d, Sec.%d\n",
                            wCurMtReqBR, wRealMtReqBR, 
                            m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), LOGCHL_VIDEO),
                            m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), LOGCHL_SECVIDEO) );                    
                }
                else
                {
                    u16 wCurMtReqBR = m_ptMtTable->GetMtReqBitrate(tMt.GetMtId());
                    m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_VIDEO );
                    wRealMtReqBR = wBitRate;
                    Mt2Log("[ProcMtMcuFlowControlCmd] CurMtReqBR.%d while RealReqBR.%d, adjust for E1MtLowed None DS\n", wCurMtReqBR, wBitRate);
                }
                m_ptMtTable->SetLowedRcvBandWidth(tMt.GetMtId(), wRealMtReqBR);
                Mt2Log("[ProcMtMcuFlowControlCmd] Lowed Mt.%d's LowedRcvBandWidth adjust to.%d\n", tMt.GetMtId(), wRealMtReqBR);
            }

            //zbq[02/18/2008] �����Ƿ�Ϊ�����ն˵�flowctrl�����Ե���Vmp�Ĺ㲥����
			// xliang [8/4/2009] ������ͬchangeVmpBitRate�ϲ�
//             if ( m_tConf.m_tStatus.IsBrdstVMP() )
//             {
//                 AdjustVmpBrdBitRate();
//             }

            //zbq[12/11/2008] ����������������նˣ�ˢ���������
            if (m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId()))
            {
                RefreshBasParam(tMt.GetMtId(), TRUE);
            }
		}
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
            //����������ͬ����Ϣ
            if (wBitRate == m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), FALSE) ||
                (!m_tDoubleStreamSrc.IsNull() && wBitRate > GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE)))
            {
                Mt2Log("[ProcMtMcuFlowControlCmd] rcv snd MtReqBitrate :%d from mt<%d>, ignore it.\n",
                    wBitRate, tMt.GetMtId());
                return;
            }

			m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_SECVIDEO );
            Mt2Log("[ProcMtMcuFlowControlCmd] rcv snd MtReqBitrate :%d from mt<%d>\n",
                    wBitRate, tMt.GetMtId());
            
            //zbq[12/11/2008] ����������������նˣ�ˢ���������
            if (m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE))
            {
                RefreshBasParam(tMt.GetMtId(), TRUE, TRUE);
            }
		}
		
		if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
		{
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
				Mt2Log( "[ProcMtMcuFlowControlCmd] SelectSee SrcMtId.%d, DstMtId.%d, wBitRate.%d, ChnnlType.%d\n", 
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
        }        

		//���������ڹ㲥����ϳ�ͼ��,������ϳ�����
		// xliang [8/4/2009] ���½ӿ�
        if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE 
            && m_tConf.m_tStatus.IsBrdstVMP()
			&& MODE_VIDEO == tLogicChnnl.GetMediaType()
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
            
            if (wMinBitRate != wBasBitrate && 
                MODE_VIDEO == tLogicChnnl.GetMediaType())
            {
                ChangeVmpBitRate(wMinBitRate, byVmpChanNo);                
            }*/
			AdjustVmpBrdBitRate(&tMt);
            return;
        }

        if (m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tFirstLogicChnnl, TRUE) &&
            m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE))
        {
            bStartDoubleStream = TRUE;
        }

		Mt2Log( "[ProcMtMcuFlowControlCmd] IsUseAdapter.%d, IsStartDStream.%d, SrcMtId.%d, DstMtId.%d, wBitRate.%d, wMinBitRate.%d, ChnnlType.%d\n", 
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
                if (TRUE == bStartDoubleStream && !m_tVidBrdSrc.IsNull())
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
/*                     if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
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
                    }*/
                    OspPrintf(TRUE, FALSE, "[ProcMtMcuFlowControlCmd] req bitrate < second bitrate, ignore it\n"); 
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
                        Mt2Log("[ProcMtMcuFlowControlCmd1] change vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                               wMinBitRate, m_wVidBasBitrate);
                        ChangeAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }
                    else
                    {
                        Mt2Log("[ProcMtMcuFlowControlCmd1] start vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                               wMinBitRate, m_wVidBasBitrate);
                        StartAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }                   

                    StartSwitchToSubMt(m_tVidBasEqp, m_byVidBasChnnl, tMt.GetMtId(), MODE_VIDEO);
                }
                else if (m_tConf.m_tStatus.IsVidAdapting())//������Ҫ�����������ʣ�ֱ�ӽ�����������������mt(�������������ն˵Ľ������ʸ��͵����)
                {
                    Mt2Log("[ProcMtMcuFlowControlCmd1] switch vid bas to mt<%d>\n", tMt.GetMtId());
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
                        Mt2Log("[ProcMtMcuFlowControlCmd3]change br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                               wMinBitRate, m_wBasBitrate);
                        ChangeAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }
                    else
                    {
                        Mt2Log("[ProcMtMcuFlowControlCmd3]start br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                               wMinBitRate, m_wBasBitrate);
                        StartAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }                   
                    
                    StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
                }
                else if (m_tConf.m_tStatus.IsBrAdapting())//������Ҫ�����������ʣ�ֱ�ӽ�����������������mt(�������������ն˵Ľ������ʸ��͵����)
                {
                    Mt2Log("[ProcMtMcuFlowControlCmd1] switch br bas to mt<%d>\n", tMt.GetMtId());
                    StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
                }
            }            
            //���ٵ�ԭʼ���ʣ��������Ϊ������
            else if (0 != m_tConf.GetSecBitRate())
            {
                StartSwitchToSubMt(tMtSrc, 0, tMt.GetMtId(), MODE_VIDEO);
                NotifyMtSend(tMtSrc.GetMtId(), MODE_VIDEO, TRUE);

				//�ָ���������
				if (IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate) && m_tConf.m_tStatus.IsBrAdapting())
				{
					ChangeAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
				}
            }
            //˫��ʽ�������������ն˽�����ƵԴ���ʣ����ܻὫ����ѹ�����
            else if (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() || IsNeedCifAdp())
            {
                AdjustMtVideoSrcBR(tMt.GetMtId(), wMinBitRate, tLogicChnnl.GetMediaType());
            }

            return;
        }				
		break;
		
	default:
		ConfLog( FALSE, "CMcuVcInst: ProcMtMcuFlowControlCmd() Wrong message %u(%s) received in state %u!\n", 
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
		ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] No polycom MCU.%d deal with, ignore it\n", byPolyMCUId );
		return;
	}

	TMt tPolyMCU = m_ptMtTable->GetMt(byPolyMCUId);
	
	//Polymcuֻ�ڸ����ʱ��һ��˫��ͨ��
	if ( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
	{
		if ( tPolyMCU == m_tDoubleStreamSrc )
		{
			StopDoubleStream( TRUE, FALSE );
			CallLog( "[ProcPolycomMcuFlowctrlCmd] StopDS(MODE_SECVIDEO)\n" );
		}

		TLogicalChannel tFstLogicChnnl;
		TLogicalChannel tSecLogicChnnl;
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstLogicChnnl, TRUE) )
		{
			ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed\n", byPolyMCUId );
			return;
		}
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE) )
		{
			ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed\n", byPolyMCUId );
			return;
		}

		u16 wSecBR = tLogicChnnl.GetFlowControl();
		tSecLogicChnnl.SetFlowControl(wSecBR);
		u16 wFstBR = m_ptMtTable->GetDialBitrate(byPolyMCUId) - wSecBR;
		tFstLogicChnnl.SetFlowControl(wFstBR);

		m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstLogicChnnl, TRUE);
		m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE);

		CallLog( "[ProcPolycomMcuFlowctrlCmd] MODE_SECVIDEO has been ctrled, wBR.%d\n", tLogicChnnl.GetFlowControl() );
	}
	else if ( MODE_VIDEO == tLogicChnnl.GetMediaType() )
	{
		
		//���Լ���
		u16 wDailBR = m_ptMtTable->GetDialBitrate(byPolyMCUId);

		TLogicalChannel tFstLogicChnnl;
		TLogicalChannel tSecLogicChnnl;
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstLogicChnnl, TRUE) )
		{
			ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed(1)\n", byPolyMCUId );
			return;
		}
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE) )
		{
			ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed(1)\n", byPolyMCUId );
			return;
		}

		u16 wFstBR = tLogicChnnl.GetFlowControl();
		tFstLogicChnnl.SetFlowControl(wFstBR);
		
		u16 wSecBR = m_ptMtTable->GetDialBitrate(byPolyMCUId) - wFstBR;
		if ( wSecBR * 100 / wDailBR < 5 )
		{
			ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] wSecBR.%d, wDailBR.%d, sec BR adjust to 0\n", wSecBR, wDailBR );
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
				CallLog( "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, StopDS\n", wBitRate, wDailBR );
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
					CallLog( "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, StartDS\n", wBitRate, wDailBR );
				}
				else
				{
					ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, should but no StartDS due to get sec chnnl failed\n", wBitRate, wDailBR );
				}
			}
			else
			{
				CallLog( "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, should but no StartDS due to dualmode.%d\n",
								 wBitRate, wDailBR, m_tConf.GetConfAttrb().GetDualMode());
			}
		}
	}
	else
	{
		ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] unexpected chnnl mode.%d\n", tLogicChnnl.GetMediaType() );
	}

	//��flowctrl�����ն˵�ǰ��ͨ������Ӧ�ڶ�polymcu�����⴦��ͬʱ���������ͨ��
	TLogicalChannel tFstFwdChnnl;
	TLogicalChannel tFstRcvChnnl;
	TLogicalChannel tSecFwdChnnl;
	TLogicalChannel tSecRcvChnnl;

	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstFwdChnnl, TRUE) )
	{
		ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Fwd VIDEO chnnl failed\n", byPolyMCUId );
		return;
	}
	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstRcvChnnl, FALSE) )
	{
		ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Rcv VIDEO chnnl failed\n", byPolyMCUId );
		return;
	}
	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecFwdChnnl, TRUE) )
	{
		ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Fwd LOGCHL_SECVIDEO chnnl failed\n", byPolyMCUId );
		return;
	}
	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecRcvChnnl, FALSE) )
	{
		ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Rcv LOGCHL_SECVIDEO chnnl failed\n", byPolyMCUId );
		return;
	}

	u16 wFstRcvBR = tFstRcvChnnl.GetFlowControl();
	u16 wSecRcvBR = tSecRcvChnnl.GetFlowControl();

	tFstRcvChnnl.SetFlowControl(tFstFwdChnnl.GetFlowControl());
	tSecRcvChnnl.SetFlowControl(tSecFwdChnnl.GetFlowControl());

	m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstRcvChnnl, FALSE);
	m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecRcvChnnl, FALSE);

	CallLog( "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d FstRcvBR adjust from.%d to %d, SecRcvBR adjust from.%d to %d\n",
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

		Mt2Log( "[Mt2McuFlowctrlInd] MtId.%d, ManuId.%d, DStreamMtId.%d, wBitRate.%d, ChnnlType.%d\n", 
				tMt.GetMtId(), m_ptMtTable->GetManuId( tMt.GetMtId() ), 
				m_tDoubleStreamSrc.GetMtId(), wBitRate, tLogicChnnl.GetMediaType() );
		
		// xliang [11/14/2008] ���Ӵ����polycom��ָʾ��Ӧ
		if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId( tMt.GetMtId() ) )
		{
			CallLog( "[ProcMtMcuFlowControlIndication] PolycomMt.%d Ind ChnType.%d BR.%d\n",
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
                if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsOverSatCastChnnlNum(tMt.GetMtId()))
                {
                    ConfLog(FALSE, "[Mt2McuFlowctrlInd] over max upload mt num.\n");
                    return;
                }
               
				//�˫��ʱ�������������Ч��
				UpdateH239TokenOwnerInfo( tMt );
				StartDoubleStream( tMt, tH239LogicChnnl );
			}
			else
			{
				Mt2Log( "[Mt2McuFlowctrlInd] MtId.%d, ManuId.%d, DStreamMtId.%d, wBitRate.%d, ChnnlType.%d open double stream with wrong DualMode, NACK !\n", 
					tMt.GetMtId(), m_ptMtTable->GetManuId( tMt.GetMtId() ), m_tDoubleStreamSrc.GetMtId(), wBitRate, tLogicChnnl.GetMediaType() );
			}
		}
		break;
		
	default:
		ConfLog( FALSE, "[Mt2McuFlowctrlInd] Wrong message %u(%s) received in state %u!\n", 
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
    ������      GetLstRcvGrpMtBr
    ����        ���õ�ĳ����Ⱥ������СҪ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/08/04    4.6         Ѧ��          ����
====================================================================*/
u16 CMcuVcInst::GetLstRcvGrpMtBr(BOOL32 bPrimary, u8 byMediaType, u8 byRes, u8 bySrcMtId /* = 0*/)
{
    u16 wMinBitRate = m_tConf.GetBitRate();
    u16 wMtBitRate;
    u8	byLoop;
    TSimCapSet tMtDstSCS;
    u8  byScale = 100;
    
    // xsl [8/7/2006] ���ش�ͨ�������նˣ���mcu�������ʲ����бȽ�(��Ϊmcu���������ɻش��ն˷������ʾ���)
    // ��֤�ϼ�������ʱ����Ӱ���¼��㲥Դ����
    BOOL32 bMMcuSpyMtOprSkip = FALSE;
    if( !m_tCascadeMMCU.IsNull() )
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
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



	u8  byNum = 0;
	u8	abyMtId[MAXNUM_CONF_MT] = { 0 };
	//m_cMtRcvGrp.GetMtMediaRes(tMt.GetMtId(), byMVType, byRes);
	m_cMtRcvGrp.GetMVMtList(byMediaType, byRes, byNum, abyMtId, TRUE);//ǿ��ȡ����MT�б�������������
	if(byNum > 0)
	{
		u8 byMtLoop = 0;
		for(byLoop = 0; byLoop < byNum; byLoop ++)
		{
			byMtLoop = abyMtId[byLoop];

			if (m_tConfAllMtInfo.MtJoinedConf(byMtLoop) && bySrcMtId != byMtLoop)
			{
				if (m_tCascadeMMCU.GetMtId() == byMtLoop && bMMcuSpyMtOprSkip )
					continue;
				
				tMtDstSCS = m_ptMtTable->GetDstSCS(byMtLoop);
				if (MEDIA_TYPE_NULL == byMediaType || tMtDstSCS.GetVideoMediaType() == byMediaType)
				{
					//zbq[07/28/2009] ˫��flowctrl����������Ӧ���������ն˵�flowctrl��AdpParam��Ȼ����
					if (bPrimary &&
						m_cMtRcvGrp.IsMtNeedAdp(byMtLoop))
					{
						continue;
					}
					if (!bPrimary &&
						m_cMtRcvGrp.IsMtNeedAdp(byMtLoop, FALSE))
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
            // guzh [7/27/2007] ��Ҫ���ֻ�ȡ������ʽ���Ƿ�����ʽ��������ʣ���Ϊ��Ϊ��Զ¼����ʽ��
            if (IsRecordSrcBas(MODE_VIDEO, tRecSrc, byRecChnlIdx))
            {
                if ( ( tRecSrc == m_tBrBasEqp &&
                       ( MEDIA_TYPE_NULL == byMediaType || m_tConf.GetMainVideoMediaType() == byMediaType ) )
                   || ( tRecSrc == m_tVidBasEqp &&
                        MEDIA_TYPE_NULL != byMediaType &&
                        m_tConf.GetSecVideoMediaType() == byMediaType )
                   )
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
            wMinBitRate = kmax(wMinBitRate, m_tConf.GetSecBitRate()*byScale/100);
        }
    }

    Mt2Log("[GetLeastMtReqBitrate] Bitrate.%d, primary.%d, media.%d\n", 
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
    
    // xsl [8/7/2006] ���ش�ͨ�������նˣ���mcu�������ʲ����бȽ�(��Ϊmcu���������ɻش��ն˷������ʾ���)
    // ��֤�ϼ�������ʱ����Ӱ���¼��㲥Դ����
    BOOL32 bMMcuSpyMtOprSkip = FALSE;
    if( !m_tCascadeMMCU.IsNull() )
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
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
            if (MEDIA_TYPE_NULL == byMediaType || tMtDstSCS.GetVideoMediaType() == byMediaType)
            {
                //zbq[07/28/2009] ˫��flowctrl����������Ӧ���������ն˵�flowctrl��HD-AdpParam��Ȼ����
                if (IsHDConf(m_tConf))
                {
                    if (bPrimary &&
                        m_cMtRcvGrp.IsMtNeedAdp(byLoop))
                    {
                        continue;
                    }
                    if (!bPrimary &&
                        m_cMtRcvGrp.IsMtNeedAdp(byLoop, FALSE))
                    {
                        continue;
                    }
                }

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
            // guzh [7/27/2007] ��Ҫ���ֻ�ȡ������ʽ���Ƿ�����ʽ��������ʣ���Ϊ��Ϊ��Զ¼����ʽ��
            if (IsRecordSrcBas(MODE_VIDEO, tRecSrc, byRecChnlIdx))
            {
                if ( ( tRecSrc == m_tBrBasEqp &&
                       ( MEDIA_TYPE_NULL == byMediaType || m_tConf.GetMainVideoMediaType() == byMediaType ) )
                   || ( tRecSrc == m_tVidBasEqp &&
                        MEDIA_TYPE_NULL != byMediaType &&
                        m_tConf.GetSecVideoMediaType() == byMediaType )
                   )
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
            wMinBitRate = kmax(wMinBitRate, m_tConf.GetSecBitRate()*byScale/100);
        }
    }

    Mt2Log("[GetLeastMtReqBitrate] Bitrate.%d, primary.%d, media.%d\n", 
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
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
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
            u8  byRecChnlIdx = 0;
            // guzh [7/27/2007] ��Ҫ���ֻ�ȡ������ʽ���Ƿ�����ʽ��������ʣ���Ϊ��Ϊ��Զ¼����ʽ��
            if (IsRecordSrcBas(MODE_VIDEO, tRecSrc, byRecChnlIdx))
            {
                if ( ( tRecSrc == m_tBrBasEqp &&
                       ( MEDIA_TYPE_NULL == byMediaType || m_tConf.GetMainVideoMediaType() == byMediaType ) )
                   || ( tRecSrc == m_tVidBasEqp &&
                        MEDIA_TYPE_NULL != byMediaType &&
                        m_tConf.GetSecVideoMediaType() == byMediaType )
                   )
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
            wMinBitRate = kmax(wMinBitRate, m_tConf.GetSecBitRate()*byScale/100);
        }
    }

    Mt2Log("[GetLeastMtReqBitrate] Bitrate.%d, primary.%d, media.%d\n", 
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

    Mt2Log("[GetLeastMtReqFramerate] FrameRate.%d, media.%d\n", byMinFrameRate, byMediaType);

    return byMinFrameRate;
}

/*====================================================================
    ������      ��IsNeedAudAdapt
    ����        ���Ƿ���Ҫ��Ƶ��������
    �㷨ʵ��    ����û������Ƶ�㲥ԴΪ��������¼��������
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/09/02    4.0         libo          ����
====================================================================*/
BOOL32 CMcuVcInst::IsNeedAudAdapt(TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS, TMt *ptSrcMt/* = NULL*/)
{
    TSimCapSet tBrdSrcSCS;
    u8 byMainType;
    u8 bySecondType;
    TMt tSrcMt;
    tSrcMt.SetNull();

    bySecondType = m_tConf.GetSecAudioMediaType();
    if (MEDIA_TYPE_NULL == bySecondType)
    {
        return FALSE;
    }

    if (!HasJoinedSpeaker())
    {
        return FALSE;
    }

    //�õ��㲥Դ��SCS
    if (NULL != ptSrcMt)
    {
        tSrcMt = *ptSrcMt;
    }
    else if (HasJoinedSpeaker())
    {
        tSrcMt = GetLocalSpeaker();
    }

    if (tSrcMt.IsNull())
    {
        ConfLog(FALSE, "[IsNeedAudAdapt] src mt not in conf\n");
        return FALSE;
    }

    //����¼���
    if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType())
    {
        tBrdSrcSCS.SetAudioMediaType(m_tPlayEqpAttrib.GetAudioType());
    }
    else
    {
        tBrdSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());
        if (tBrdSrcSCS.IsNull())
        {
            ConfLog(FALSE, "�㲥Դδ��MCU����Ƶ�߼�ͨ��!\n");
            return FALSE;
        }
    }
    
    tSrcSCS.Clear();
    tSrcSCS.SetAudioMediaType(tBrdSrcSCS.GetAudioMediaType());

    tDstSCS.Clear();
    byMainType = m_tConf.GetMainAudioMediaType();
    if (tBrdSrcSCS.GetAudioMediaType() == byMainType)
    {
        tDstSCS.SetAudioMediaType(bySecondType);
    }
    else if (tBrdSrcSCS.GetAudioMediaType() == bySecondType)
    {
        tDstSCS.SetAudioMediaType(byMainType);
    }
    else
    {
        ConfLog(FALSE, "[IsNeedAudAdapt]�㲥Դ��ͬʱ����������鲻��!\n");
        return FALSE;
    }

    //�Ƿ���Ŀ��SCS��Դ��ͬ
    TSimCapSet tMtDstSCS;
    for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && tSrcMt.GetMtId() != byLoop)
        {
            tMtDstSCS = m_ptMtTable->GetDstSCS(byLoop);
            if (tMtDstSCS.GetAudioMediaType() != tSrcSCS.GetAudioMediaType())
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}


/*====================================================================
    ������      ��IsNeedVidAdapt
    ����        ���Ƿ���Ҫ��Ƶ��������
    �㷨ʵ��    ����û�п�����ƵԴΪ¼��������
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/09/02    4.0         libo          ����
====================================================================*/
BOOL32 CMcuVcInst::IsNeedVidAdapt(TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS, u16 &wAdaptBitRate, TMt *ptSrcMt/* = NULL*/)
{
	TSimCapSet tBrdSrcSCS;
    u8 byMainType;
    u8 bySecondType;
    TMt tSrcMt;
    tSrcMt.SetNull();

    bySecondType = m_tConf.GetSecVideoMediaType();
	if (MEDIA_TYPE_NULL == bySecondType)
	{
		return FALSE;
	}

    if (m_tConf.m_tStatus.IsBrdstVMP())
    {
        return FALSE;
    }

    if (!HasJoinedSpeaker() && CONF_POLLMODE_VIDEO != m_tConf.m_tStatus.GetPollMode())
    {
        return FALSE;
    }

	//�õ��㲥Դ��SCS
    if (NULL != ptSrcMt)
    {
        tSrcMt = *ptSrcMt;
    }
	else if (HasJoinedSpeaker())
	{
        tSrcMt = GetLocalSpeaker();
	}
    else if (CONF_POLLMODE_VIDEO == m_tConf.m_tStatus.GetPollMode())
    {
        tSrcMt = m_tVidBrdSrc;
    }

    if (tSrcMt.IsNull())
    {
        ConfLog(FALSE, "[IsNeedVidAdapt] no src mt in conf\n");
        return FALSE;
    }

    //����¼���
    if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType())
    {
        tBrdSrcSCS.SetVideoMediaType(m_tPlayEqpAttrib.GetVideoType());        
    }
    else
    {
        tBrdSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());	
        if (tBrdSrcSCS.IsNull())
        {
            ConfLog(FALSE, "�㲥Դδ��MCU����Ƶ�߼�ͨ��!\n");
            return FALSE;
        }
    }
    
    tSrcSCS.Clear();
	tSrcSCS.SetVideoMediaType(tBrdSrcSCS.GetVideoMediaType());
    tSrcSCS.SetVideoResolution( tBrdSrcSCS.GetVideoResolution() );

    tDstSCS.Clear();
    byMainType = m_tConf.GetMainVideoMediaType();
    if (tBrdSrcSCS.GetVideoMediaType() == byMainType)
    {
        tDstSCS.SetVideoMediaType(bySecondType);
    }
    else if (tBrdSrcSCS.GetVideoMediaType() == bySecondType)
    {
        tDstSCS.SetVideoMediaType(byMainType);
    }
    else
    {
        ConfLog(FALSE, "[IsNeedVidBrAdapt]�㲥Դ��ͬʱ����������鲻��!\n");
        return FALSE;
    }

    u8 byConfMainResolution = m_tConf.GetMainVideoFormat();
    u8 byConfSecResolution = m_tConf.GetSecVideoFormat();
    if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType())
    {
        //�ֱ�������Ϊ�������ֱ��ʷ�ֹ�������䣬��¼����ӿڿ��Ի�ȡ�ֱ�����Ϣʱ�ٸ���¼������÷ֱ���
        tDstSCS.SetVideoResolution(byConfMainResolution);
		tSrcSCS.SetVideoResolution(byConfMainResolution);
    }
	else
	{
		TLogicalChannel tMtLogicChannel;
		memset(&tMtLogicChannel, 0, sizeof(tMtLogicChannel));
		m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicChannel, FALSE);
		u8 bySrcMtResolution = tMtLogicChannel.GetVideoFormat();
		if( bySrcMtResolution == byConfMainResolution)
		{
			tDstSCS.SetVideoResolution( byConfSecResolution );
		}
		else if (bySrcMtResolution == byConfSecResolution)
		{
			tDstSCS.SetVideoResolution( byConfMainResolution );
		}
		else
		{
			tDstSCS.SetVideoResolution( byConfMainResolution );
		}
	}

    //�Ƿ���Ŀ��SCS��Դ��ͬ
    BOOL32 bRet = FALSE;
    TSimCapSet tMtDstSCS;
    for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && tSrcMt.GetMtId() != byLoop)
        {
            tMtDstSCS = m_ptMtTable->GetDstSCS(byLoop);
            // guzh [6/1/2007] ���û�и��ն˴��߼�ͨ�������뿼��
            if (tMtDstSCS.GetVideoMediaType() != MEDIA_TYPE_NULL &&
                tMtDstSCS.GetVideoMediaType() != tSrcSCS.GetVideoMediaType())
            {
                bRet = TRUE;
                break;
            }
        }
    }

    // guzh [8/29/2007] FIXME�������Ҫ֧�ֶ�̬����ʽ�ĵ��������鲥��¼�񣩣�
    // ������黹û���ն���Ҫ���䣬�����û��ͳ�Ƶ���ֱ��Return FALSE
    // �ᵼ�²������䣬�޷�¼��/�����鲥��
    if ( !bRet)
    {
        return FALSE;
    }

    wAdaptBitRate = GetLeastMtReqBitrate(TRUE, tDstSCS.GetVideoMediaType(), tSrcMt.GetMtId());
    return bRet;
}

/*=============================================================================
  �� �� ���� IsNeedCifAdp
  ��    �ܣ� �Ƿ���Ҫ�ֱ�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TSimCapSet &tDstSCS
             TSimCapSet &tSrcSCS
  �� �� ֵ�� BOOL32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  07/08/20    4.0         �ű���        H264��D1(4CIF)�ֱ��ʲ������� ֧��
  08/02/21    4.5         ����        ȡ��������޸ģ�H264���䲻֧���������ط�����
=============================================================================*/
BOOL32 CMcuVcInst::IsNeedCifAdp( void )
{
    BOOL32 bRet = FALSE;
    u8 byConfFstVidType = m_tConf.GetMainVideoMediaType();
    u8 byConfSecVidType = m_tConf.GetSecVideoMediaType();
    u8 byConfFstFormat  = m_tConf.GetMainVideoFormat();
    u8 byConfSecFormat  = m_tConf.GetSecVideoFormat();

    if(byConfFstVidType == byConfSecVidType &&
       byConfFstFormat  != byConfSecFormat )
    {
        bRet = TRUE;
    }
    return bRet;
}

//modify bas 2
/*====================================================================
    ������      ��IsNeedBrAdapt
    ����        ���Ƿ���Ҫ������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/09/02    4.0         libo          ����
====================================================================*/
BOOL32 CMcuVcInst::IsNeedBrAdapt(TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS, u16 &wAdaptBitRate, TMt *ptSrcMt/* = NULL*/)
{
	TSimCapSet tBrdSrcSCS;
    TMt tSrcMt;
	u8 bySrcMtId;
    tSrcMt.SetNull();

	if (0 == m_tConf.GetSecBitRate())
	{
		return FALSE;
	}

    if (m_tConf.m_tStatus.IsBrdstVMP())
    {
        return FALSE;
    }

    if (!HasJoinedSpeaker() && CONF_POLLMODE_VIDEO != m_tConf.m_tStatus.GetPollMode())
    {
        return FALSE;
    }

	//�õ��㲥Դ��SCS
    if (NULL != ptSrcMt)
    {
        tSrcMt = *ptSrcMt;
    }
	else if (HasJoinedSpeaker())
	{
        tSrcMt = GetLocalSpeaker();
	}
    else if (CONF_POLLMODE_VIDEO == m_tConf.m_tStatus.GetPollMode())
    {
        tSrcMt = m_tVidBrdSrc;
    }

    if (tSrcMt.IsNull())
    {
        ConfLog(FALSE, "[IsNeedBrAdapt] no src mt in conf\n");
        return FALSE;
    }

	bySrcMtId = tSrcMt.GetMtId();

    if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType())
    {
        tBrdSrcSCS.SetVideoMediaType(m_tPlayEqpAttrib.GetVideoType());
		bySrcMtId = 0;
    }
	else
	{
		tBrdSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());
		if (tBrdSrcSCS.IsNull())
		{
			ConfLog(FALSE, "�㲥Դδ��MCU������Ƶ�߼�ͨ�� -- ��������!\n");
			return FALSE;
		}
	}
	
    tSrcSCS.Clear();
	tSrcSCS.SetVideoMediaType(tBrdSrcSCS.GetVideoMediaType());
    tSrcSCS.SetVideoResolution(tBrdSrcSCS.GetVideoResolution());

    //�Ƿ���Ҫ��������	
    u16 wMinBitrate = GetLeastMtReqBitrate(TRUE, tSrcSCS.GetVideoMediaType(), bySrcMtId);

    //zbq [08/26/2007] �Ƿ���Ҫ��������Ҫ�뵱ǰ����ƵԴ�ķ������ʱȽ�
	u16 wSrcSndBitrate = 0;
	if ( TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType() )
	{
		wSrcSndBitrate = m_tConf.GetBitRate();
	}
	else
	{
		wSrcSndBitrate = m_ptMtTable->GetMtSndBitrate(tSrcMt.GetMtId());
	}
    if (wMinBitrate >= wSrcSndBitrate /*m_tConf.GetBitRate()*/)
    {
        return FALSE;
    }

    wAdaptBitRate = wMinBitrate;
    tDstSCS.Clear();

    // zbq [06/07/2007] ȡ��ȷ��Ŀ��SCS
    TSimCapSet tBrdDstSCS;
    tBrdDstSCS = m_ptMtTable->GetDstSCS(tSrcMt.GetMtId());
    tDstSCS.SetVideoMediaType(tBrdDstSCS.GetVideoMediaType());

	if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType())
	{
		tDstSCS.SetVideoResolution(m_tConf.GetMainVideoFormat());
	}
	else
	{
		TLogicalChannel tMtLogicChannel;
		memset(&tMtLogicChannel, 0, sizeof(tMtLogicChannel));
		m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicChannel, FALSE);
		tDstSCS.SetVideoResolution( tMtLogicChannel.GetVideoFormat() );
	}

    return TRUE;
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
	u8 byAdpChnnlNum = m_tVmpChnnlInfo.m_byHDChnnlNum;
	TChnnlMemberInfo tChnnlMemInfo;
	u8 byMcuId;
	u8 byMtId;
	BOOL32 bSpeaker = FALSE;
	BOOL32 bSelected= FALSE;
	BOOL32 bNoneKeda= FALSE;
	u8 byLoop;
	OspPrintf(TRUE,FALSE,"\n---���� %s�� VMPǰ����ͨ����Ա��Ϣ---\n",m_tConf.GetConfName() );
	OspPrintf(TRUE,FALSE,"   ��ǰ����%u��Mtռ��ǰ����ͨ��\n",byAdpChnnlNum);
	OspPrintf(TRUE,FALSE,"--------------------------------------\n");
	for(byLoop = 0; byLoop < MAXNUM_SVMPB_HDCHNNL; byLoop ++)
	{
		m_tVmpChnnlInfo.GetHdChnnlInfo(byLoop,&tChnnlMemInfo);
		byMcuId = tChnnlMemInfo.GetMt().GetMcuId();
		byMtId = tChnnlMemInfo.GetMtId();
		bSpeaker = tChnnlMemInfo.IsAttrSpeaker();
		bSelected = tChnnlMemInfo.IsAttrSelected();
		bNoneKeda = tChnnlMemInfo.IsAttrNoneKeda();
		OspPrintf(TRUE, FALSE, "[ID.%u]:(%u,%u)\tbSpeaker:%d\tbSelected:%d\tbNoneKeda:%d\n",
			byLoop+1, byMcuId, byMtId, bSpeaker, bSelected, bNoneKeda);
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
	TConfMtInfo tConfMtInfo;

	OspPrintf( TRUE, FALSE, "\n---����(%d) %s �ն���Ϣ---\n", CurState(), m_tConf.GetConfName() );
	OspPrintf( TRUE, FALSE, "IsTemplate %d\n", m_tConf.m_tStatus.IsTemplate() );
	OspPrintf( TRUE, FALSE, "InstID %d\n", GetInsID() );
    OspPrintf( TRUE, FALSE, "ConfIdx %d\n", m_byConfIdx );
    m_tConf.GetConfId().GetConfIdString(achTemp, sizeof(achTemp));
    OspPrintf( TRUE, FALSE, "cConfId %s\n", achTemp );

	OspPrintf( TRUE, FALSE, "�����ն�: McId-%d\n", LOCAL_MCUID );

	for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
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
                OspPrintf( TRUE, FALSE, "MtId.%d unexist, mishanding ? ...\n", byMtId );
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
                OspPrintf( TRUE, FALSE, "[ShowConfMt] GetMt.%d DailAlias failed\n", tMt.GetMtId() );
            }

            if ( !bRet )
            {
                OspPrintf( TRUE, FALSE, "%sMT%d:0x%x(Dri:%d Mp:%d), Type: %d Manu: %s, 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s\n",
                                        achTemp, byLoop, m_ptMtTable->GetIPAddr( byLoop ), tMt.GetDriId(), 
                                        m_ptMtTable->GetMpId( tMt.GetMtId() ),
                                        m_ptMtTable->GetMtType( tMt.GetMtId() ),
                                        GetManufactureStr( m_ptMtTable->GetManuId( byLoop ) ), 
                                        tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                        tMtAliasH320Alias.m_achAlias);
            }
            else
            {
                if ( mtAliasTypeTransportAddress == tDailAlias.m_AliasType )
                {
                    OspPrintf( TRUE, FALSE, "%sMT%d:0x%x(Dri:%d Mp:%d), Type: %d Manu: %s, 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s, DialAlias.%s@%d\n",
                                            achTemp, byLoop, m_ptMtTable->GetIPAddr( byLoop ), tMt.GetDriId(), 
                                            m_ptMtTable->GetMpId( tMt.GetMtId() ),
                                            m_ptMtTable->GetMtType( tMt.GetMtId() ),
                                            GetManufactureStr( m_ptMtTable->GetManuId( byLoop ) ), 
                                            tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                            tMtAliasH320Alias.m_achAlias,
                                            StrOfIP(tDailAlias.m_tTransportAddr.GetIpAddr()), tDailAlias.m_tTransportAddr.GetPort());
                }
                else
                {
                    OspPrintf( TRUE, FALSE, "%sMT%d:0x%x(Dri:%d Mp:%d), Type: %d Manu: %s, 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s, DialAlias.%s\n",
                                            achTemp, byLoop, m_ptMtTable->GetIPAddr( byLoop ), tMt.GetDriId(), 
                                            m_ptMtTable->GetMpId( tMt.GetMtId() ),
                                            m_ptMtTable->GetMtType( tMt.GetMtId() ),
                                            GetManufactureStr( m_ptMtTable->GetManuId( byLoop ) ), 
                                            tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                            tMtAliasH320Alias.m_achAlias,
                                            tDailAlias.m_achAlias);
                }
            }

			if( bInJoinedConf )
			{
				//�����߼�ͨ����Ϣ
				memset( achTemp, ' ', 255 );
				int l = sprintf( achTemp, "   FL Aud:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                       GetMediaStr( tLogicalChannel.GetChannelType() ) );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 26;
				l += sprintf( achTemp+l, "Vid:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )
				{
                    // ���ӷֱ��ʴ�ӡ, zgc, 2008-08-09
					l += sprintf( achTemp+l, "%d(%s)(%s)(%dK)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                       GetMediaStr( tLogicalChannel.GetChannelType() ),
                                                               GetResStr( tLogicalChannel.GetVideoFormat() ),
															   tLogicalChannel.GetFlowControl() );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 60;//50;
				l += sprintf( achTemp+l, "H239:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tLogicalChannel, TRUE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)<%dK>", tLogicalChannel.GetRcvMediaChannel().GetPort(),
															GetResStr(tLogicalChannel.GetVideoFormat()),
															tLogicalChannel.GetFlowControl() );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}
				
				memset( achTemp+l, ' ', 255-l );
				l = 86;//68;
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
				l = 97;//79;
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
                l = 109;//91;
                l += sprintf(achTemp+l, "VidBand:%dK", m_ptMtTable->GetRcvBandWidth(byLoop));                

				sprintf( achTemp+l, "%c", 0 );
				OspPrintf( TRUE, FALSE, "%s\n", achTemp );

				//�����߼�ͨ����Ϣ
				memset( achTemp, ' ', 255 );
				l = sprintf( achTemp, "   RL Aud:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, FALSE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)(%d)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                    GetMediaStr( tLogicalChannel.GetChannelType() ),
															tLogicalChannel.GetFlowControl());
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 26;
				l += sprintf( achTemp+l, "Vid:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tLogicalChannel, FALSE ) )
				{
                    // ���ӷֱ��ʴ�ӡ, zgc, 2008-08-09
					l += sprintf( achTemp+l, "%d(%s)(%s)(%dK)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                       GetMediaStr( tLogicalChannel.GetChannelType() ),
                                                               GetResStr( tLogicalChannel.GetVideoFormat() ),
                                                               tLogicalChannel.GetFlowControl());
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 60;//50
				l += sprintf( achTemp+l, "H239:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)<%dK>", tLogicalChannel.GetRcvMediaChannel().GetPort(), 
															GetResStr(tLogicalChannel.GetVideoFormat()),
															tLogicalChannel.GetFlowControl() );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 86;//68;
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
				l = 97;//79;
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
                l = 109;//91;
                l += sprintf(achTemp+l, "VidBand:%dK", m_ptMtTable->GetSndBandWidth(byLoop));     

				sprintf( achTemp+l, "%c", 0 );
				OspPrintf( TRUE, FALSE, "%s\n", achTemp );
			}
            
            if ( byMtId != 0 )
            {
                OspPrintf( TRUE, FALSE, "\nMT.%d's Cap is as follows:\n\n", byMtId );
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

	u8 byMcuId;
	for( u8 byLoop1 = 0; byLoop1 < MAXNUM_SUB_MCU; byLoop1++ )
	{
		byMcuId = m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop1].GetMcuId();
		if( byMcuId == 0  )
		{
			continue;
		}

        if ( byMtId != 0 &&
             byMcuId != byMtId )
        {
            continue;
        }
		
		OspPrintf( TRUE, FALSE, "�����ն�: McuId-%d\n", byMcuId );
		TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(byMcuId);
		if(ptMcInfo == NULL)
		{
			continue;
		}
		TConfMtInfo *ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr(byMcuId); 
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
	
			
			OspPrintf( TRUE, FALSE, "%sMcu%dMT%d:0x%x(Dri:%d), Type: %d Manu: %s, Alias: %s, DialBitRate: %d. \n",
					achTemp, byMcuId, tMtExt.GetMtId(), tMtExt.GetIPAddr(), tMtExt.GetDriId(), 
					tMtExt.GetMtType(),
					GetManufactureStr( tMtExt.GetManuId() ), 
					tMtExt.GetAlias(), 
					tMtExt.GetDialBitRate() );
		}				
	}

    return;
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
			OspPrintf( TRUE, FALSE, "Illegal bas id.%d, mishanding ? ...\n", byBasId );
			return;
		}
		if (!g_cMcuVcApp.IsPeriEqpConnected(byBasId))
		{
			OspPrintf( TRUE, FALSE, "Bas.%d is not online yet, try another ...\n", byBasId );
			return;
		}
	}
    m_cBasMgr.Print();
    m_cMtRcvGrp.Print();
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
	TVmpModule tVmpModule = m_tConfEqpModule.m_tVmpModule;

    if( m_tConf.GetConfAttrb().IsHasVmpModule() )
	{
		OspPrintf( TRUE, FALSE, "Vmp module info:\n" );
		OspPrintf( TRUE, FALSE, "tVmpModule.m_tVMPParam.m_byVMPAuto = %d\n", tVmpModule.m_tVMPParam.IsVMPAuto()  );
		OspPrintf( TRUE, FALSE, "tVmpModule.m_tVMPParam.m_byVMPBrdst = %d\n", tVmpModule.m_tVMPParam.IsVMPBrdst()  );
		OspPrintf( TRUE, FALSE, "tVmpModule.m_tVMPParam.m_byVMPStyle = %d\n", tVmpModule.m_tVMPParam.GetVMPStyle() );
		for( u8 byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
		{
			if( !m_tConfEqpModule.m_atVmpMember[byLoop].IsNull() )
			{
				OspPrintf( TRUE, FALSE, "Vmp Chl%d(mt%d):", byLoop, m_tConfEqpModule.m_atVmpMember[byLoop].GetMtId() );
			}
		}
	}
    
    TMultiTvWallModule tMultiTvWallModule = m_tConfEqpModule.m_tMultiTvWallModule;
    TTvWallModule tOneModule;
    if ( m_tConf.GetConfAttrb().IsHasTvWallModule() )
    {
        OspPrintf( TRUE, FALSE, "Tvwall module info:\n" );
        OspPrintf( TRUE, FALSE, "tMultiTvWallModule.m_byTvModuleNum %d\n", tMultiTvWallModule.GetTvModuleNum());
        for ( u8 byLoop = 0; byLoop < tMultiTvWallModule.GetTvModuleNum(); byLoop++ )
        {
            tMultiTvWallModule.GetTvModuleByIdx(byLoop, tOneModule);
            OspPrintf( TRUE, FALSE, "Tvwall module %d:\n", byLoop );            
            OspPrintf( TRUE, FALSE, "Tvwall EqpId.%d\n", tOneModule.GetTvEqp().GetEqpId() );
            
            for ( u8 byLoop2 = 0; byLoop2 < MAXNUM_PERIEQP_CHNNL; byLoop2 ++ )
            {
                if ( tOneModule.m_abyTvWallMember[byLoop2] == 0 || 
                     tOneModule.m_abyMemberType[byLoop2] == 0 )
                     break;

                OspPrintf( TRUE, FALSE, "\tMember %d: Mt.%d, type: %d\n", 
                           byLoop2,
                           tOneModule.m_abyTvWallMember[byLoop2],
                           tOneModule.m_abyMemberType[byLoop2] );
            }
            
        }

    }
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
	ConfLog( FALSE, "\n---���� %s �ն˼����Ϣ---\n", m_tConf.GetConfName() );
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

			ConfLog( FALSE, "MT%d-0x%x multicasting: %s \n",
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
				ConfLog( FALSE, "%s\n", achTemp );
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
void CMcuVcInst::InviteUnjoinedMt( CServMsg& cServMsgHdr, const TMt* ptMt, 
								   BOOL32 bSendAlert, BOOL32 bBrdSend, 
								   u8 byCallType/* = VCS_FORCECALL_REQ*/)
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
	u8  byEncrypt  =  (m_tConf.GetConfAttrb().GetEncryptMode() != CONF_ENCRYPTMODE_NONE)?1:0;

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
            
            ConfLog(FALSE, "[InviteUnjoinedMt] GetDialAlias failed, byMtId.%d\n", ptMt->GetMtId());
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
                				    
				    ConfLog( FALSE, "[InviteUnjoinedMt] Cannot Get IsMtAdpConnected|GetMtAdpProtocalType For Mt.%d.\n", ptMt->GetMtId() );
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
					
					McsLog( "[InviteUnjoinedMt] Cannot Get IP For Mt.%d.\n", ptMt->GetMtId() );
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
                    ConfLog(FALSE, "[InviteUnjoinedMt] DriId.%d for MT.%d is OFFLine.\n", byDriId, ptMt->GetMtId());
                    g_cMcuVcApp.DecMtAdpMtNum(byDriId, m_byConfIdx, ptMt->GetMtId());
                    
                    if ( 0 == byDriId )
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
		if(ptMt->GetMtId()  == m_byMtIdNotInvite)
		{
			m_ptMtTable->SetNotInvited( ptMt->GetMtId(), TRUE );
		}
		else
		{
			m_ptMtTable->SetNotInvited( ptMt->GetMtId(), FALSE );
		}
	
		cServMsg.SetMsgBody( (u8*)ptMt, sizeof( TMt ) );
		cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
		cServMsg.CatMsgBody( (u8*)&tConfAlias, sizeof( tConfAlias ) );	
		cServMsg.CatMsgBody( (u8*)&byEncrypt, sizeof(byEncrypt));
		u16 wCallBand = htons( m_ptMtTable->GetDialBitrate( ptMt->GetMtId() ) + wAudioBand );
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
		SendMsgToMt( ptMt->GetMtId(), MCU_MT_INVITEMT_REQ, cServMsg );

		if( TRUE == bSendAlert )
		{
			cServMsgHdr.SetMsgBody( (u8*)ptMt, sizeof( TMt ) );
			cServMsgHdr.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
			if( bBrdSend )
			{
				BroadcastToAllMcu( MCU_MCU_CALLALERTING_NOTIF, cServMsgHdr );
			}
			else
			{
				SendReplyBack( cServMsgHdr, MCU_MCU_CALLALERTING_NOTIF );
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
						CallLog("Mt.%u is not invited!\n",m_byMtIdNotInvite);
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
						if( !m_ptMtTable->GetMtAlias( tInviteMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias ) )
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

                            if (byDri == 0)
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
					CallLog("Mt.%u is not invited!\n",m_byMtIdNotInvite);
				}
				else
				{
					m_ptMtTable->SetNotInvited( tInviteMt.GetMtId(), FALSE );
				}
			
				cServMsg.SetMsgBody( (u8*)&tInviteMt, sizeof( tInviteMt ) );
				cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
				cServMsg.CatMsgBody( (u8*)&tConfAlias, sizeof( tConfAlias ) );
				cServMsg.CatMsgBody( (u8*)&byEncrypt, sizeof(byEncrypt));
				u16 wCallBand = htons( m_ptMtTable->GetDialBitrate( tInviteMt.GetMtId() ) + wAudioBand );
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

				SendMsgToMt( tInviteMt.GetMtId(), MCU_MT_INVITEMT_REQ, cServMsg );					
			}
		}
	}		
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
    TLogicalChannel	tLogicChnnl;
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
                if (tSrcMt.GetMcuId() == LOCAL_MCUID && tSrcMt.GetType() == TYPE_MT)
                {
                    NotifyMtSend(tSrcMt.GetMtId());
                }

                StartSwitchToSubMt(tSrcMt, 0, tDstMt.GetMtId(), MODE_BOTH, SWITCH_MODE_SELECT);

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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
void CMcuVcInst::ProcMcuMtFastUpdatePic( const CMessage * pcMsg )
{
    //CServMsg cServMsg( pcMsg->content, pcMsg->length );
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
            TMt tMt = m_ptMtTable->GetMt( tMtStatus.GetMtId() );
			tMtStatus.SetTMt( tMt );
			cServMsg.SetMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}
		else
		{
			cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		}
		
		//��ѯ�ն�״̬
		SendMsgToMt( tMt.GetMtId(), MCU_MT_GETMTSTATUS_REQ, cServMsg );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
	TMtStatus	tMtStatus;
	TMcu tMcu;
	u8 byLoop;

	switch( CurState() )
	{
	case STATE_ONGOING: 
		
		//�Ƿ��ڱ���������
		tMcu = *(TMcu*)cServMsg.GetMsgBody(); 

		//�����ն�״̬
		if( tMcu.GetMcuId() == 0 )
		{
			//�ȷ�����MC
			for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
			{
				u8 byMcuId = m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId();
				if( byMcuId == 0 )
				{
					continue;
				}
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( byMcuId );
				if(ptConfMcInfo == NULL)
				{
					continue;
				}
				tMcu.SetMcu( byMcuId );	
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
			tMcu.SetMcu( LOCAL_MCUID );
					
		}		

		//���Ǳ�����MCU
		cServMsg.SetMsgBody( (u8*)&tMcu,sizeof(tMcu) );

		if( tMcu.IsLocal())
		{
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
			u8 byMcuId = tMcu.GetMcuId();
			if( !m_tConfAllMtInfo.m_tLocalMtInfo.MtJoinedConf( byMcuId ) )
			{
				cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}

			TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( byMcuId );
			cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
			for(s32 nLoop =0; nLoop<MAXNUM_CONF_MT; nLoop++)
			{
				if(ptConfMcInfo->m_atMtStatus[nLoop].IsNull()|| 
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
        ConfLog(FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
	TLogicalChannel tLogicalChannel;
	u16	wEvent;

	MMcuLog( "[ProcMcsMcuCamCtrlCmd] step 1 bySrcMtId.%d - MtMcuId.%d MtMtId.%d\n", 
			 bySrcMtId, tMt.GetMcuId(), tMt.GetMtId() );

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
                MMcuLog( "[ProcMcsMcuCamCtrlCmd] MT.%d no permission to operate, return\n", 
		                 bySrcMtId );					
                return;                
            }
			// �޸�Ϊң�ضԷ�MCU
			tDstMt.SetMtId(tMt.GetMcuId());
			tDstMt.SetMcuId(LOCAL_MCUID);
		}
        // ������ն���Ϣ
		else if( bySrcMtId != 0 )
		{	          
			if( tMt.IsNull() )
			{
				m_ptMtTable->GetMtSrc( bySrcMtId, &tDstMt, MODE_VIDEO );
				if( !tDstMt.IsNull() )
				{
					tDstMt = m_ptMtTable->GetMt( tDstMt.GetMtId() );
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
						MMcuLog( "[ProcMcsMcuCamCtrlCmd] bySrcMtId.%d tDstMt.IsNull() return\n", bySrcMtId );

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
				        MMcuLog( "[ProcMcsMcuCamCtrlCmd] bySrcMtId.%d is not chairman or inspecter to Dst%d, return\n", 
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
				MMcuLog( "[ProcMcsMcuCamCtrlCmd] cannot operate mmcu, return\n" );
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
				MMcuLog( "[ProcMcsMcuCamCtrlCmd] bySrcMtId.%d tDstMt.IsNull() return\n", bySrcMtId );

				return;
			}
            // guzh [6/7/2007] ����ϲ�����Ҫ�����ϼ����ܾ�
            if ( !m_tCascadeMMCU.IsNull() &&
                tDstMt.GetMtId() == m_tCascadeMMCU.GetMtId() )
            {
                MMcuLog( "[ProcMcsMcuCamCtrlCmd] cannot operate mmcu, return\n" );
                return;
            }
		}

		//not joined conference
		if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
		{
			MMcuLog( "[ProcMcsMcuCamCtrlCmd] step 3 bySrcMtId.%d - event.%d DstMcuId.%d DstMtId.%d\n", 
				     bySrcMtId, pcMsg->event, tDstMt.GetMcuId(), tDstMt.GetMtId() );

			return;
		}

		MMcuLog( "[ProcMcsMcuCamCtrlCmd] step 4 bySrcMtId.%d - event.%d DstMcuId.%d DstMtId.%d\n", 
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
			cServMsg.SetDstMtId( tDstMt.GetMtId() );
			SendMsgToMt( tDstMt.GetMtId(), wEvent, cServMsg );
		}
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
====================================================================*/
void CMcuVcInst::ProcMMcuMcuCamCtrlCmd( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt tDstMt = *(TMt*)cServMsg.GetMsgBody();
	u8 byOperType = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) );
	u16	wEvent;

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		switch( byOperType )
		{
		//�ն�����ͷ����
		case 1:		//�ն�����ͷ�ƶ�
			wEvent = MCU_MT_MTCAMERA_CTRL_CMD;
			break;
		case 2:		//�ն�����ͷֹͣ�ƶ�
			wEvent = MCU_MT_MTCAMERA_CTRL_STOP;
			break;
		case 3:	    //�ն�����ͷң����ʹ��
			wEvent = MCU_MT_MTCAMERA_RCENABLE_CMD;
			break;
		case 4:	    //�ն�����ͷ������ָ��λ��
			wEvent = MCU_MT_MTCAMERA_MOVETOPOS_CMD;
			break;
		case 5:	    //�ն�����ͷ���浽ָ��λ��
			wEvent = MCU_MT_MTCAMERA_SAVETOPOS_CMD;
			break;
		case 6:	    //ѡ���ն���ƵԴ 
			wEvent = MCU_MT_SETMTVIDSRC_CMD;
			break;
		default:
			return;
		}

		//send messge
		cServMsg.SetDstMtId( tDstMt.GetMtId() );
		SendMsgToMt( tDstMt.GetMtId(), wEvent, cServMsg );
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
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
	u16	wEvent;
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
			break;
			
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
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
        ConfLog(FALSE, "CMcuVcInst: ProcMtMcuBandwidthNotif received in state %u, ignore it\n", CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
    //u16 wMinBitRate;    
    //TMt tMtSrc;
    //TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );

    u16 wRcvBandWidth = ntohs(*(u16 *)cServMsg.GetMsgBody());
    u16 wSndBandWidth = ntohs(*(u16 *)(cServMsg.GetMsgBody() + sizeof(u16)));
    Mt2Log("[ProcMtMcuBandwidthNotif] mt<%d> wRcvBandWidth :%d, wSndBandWidth :%d\n", 
            cServMsg.GetSrcMtId(), wRcvBandWidth, wSndBandWidth);

	//zbq[09/10/2008] ������ʶ���ӣ�����ʱ������
	m_ptMtTable->SetRcvBandAdjusted(cServMsg.GetSrcMtId(), TRUE);

    TLogicalChannel tLogicChan;
    tLogicChan.SetMediaType(MODE_VIDEO);
    tLogicChan.SetFlowControl(wRcvBandWidth);
    
    cServMsg.SetMsgBody((u8*)&tLogicChan, sizeof(TLogicalChannel));

    //��ǻ�ָ���Ǹ��ն˵����������
    u8  byDstMtId = cServMsg.GetSrcMtId();
    u16 wDailBandWidth = m_ptMtTable->GetRcvBandWidth(byDstMtId);
    
    //ͨ�����ܻ�û���ü��򿪳ɹ�
    if ( 0 == wDailBandWidth )
    {
        wDailBandWidth = m_ptMtTable->GetDialBitrate(byDstMtId);
		m_ptMtTable->SetRcvBandWidth(byDstMtId, wDailBandWidth);
    }

    //��ʱ���Թ��ƣ�Ӧ�ù���. ���С��5����Ϊ�ָ���������Ϊ����. ֻ���E1�ն�.
    //��̫�ն�ʼ�ղ���Ϊ����, ��������·�������8M��һ��Ϊ10M.
    if ( wRcvBandWidth > 1024 * 8 )
    {
        m_ptMtTable->SetMtTransE1( byDstMtId, FALSE );
    }
    else
    {
        m_ptMtTable->SetMtTransE1( byDstMtId, TRUE );

		//zbq[9/27/2008] ��������С��E1��ǰ�����������Ϊ��������
        if (wDailBandWidth < wRcvBandWidth)
        {
            m_ptMtTable->SetMtBRBeLowed(byDstMtId, FALSE);
            CallLog("[ProcMtMcuBandwidthNotif] DstMt.%d NOT be set lowed due to<Dail.%d, Rcv.%d>(less)\n",
                      byDstMtId, wDailBandWidth, wRcvBandWidth );            
        }
        else
        {
			if (abs(wDailBandWidth-wRcvBandWidth)*100 / wDailBandWidth > 20)
			{
				m_ptMtTable->SetMtBRBeLowed(byDstMtId, TRUE);
				m_ptMtTable->SetLowedRcvBandWidth(byDstMtId, wRcvBandWidth);
				CallLog("[ProcMtMcuBandwidthNotif] DstMt.%d be set lowed due to<Dail.%d, Rcv.%d>\n",
					byDstMtId, wDailBandWidth, wRcvBandWidth );

			}
			else
			{
				m_ptMtTable->SetMtBRBeLowed(byDstMtId, FALSE);
				CallLog("[ProcMtMcuBandwidthNotif] DstMt.%d NOT be set lowed due to<Dail.%d, Rcv.%d>(greater less than 20)\n",
					byDstMtId, wDailBandWidth, wRcvBandWidth );            
			}
        }
    }

    //zbq[05/25/2009] ��E1�ն˲�����ͨ������Ϣˢ����
    //��Ϊ��BR=10240���ᱣ����ConfBR���Ӷ�Ӱ����������ETH�ն˵Ĵ������
    if (!m_ptMtTable->GetMtTransE1(byDstMtId))
    {
        CallLog("[ProcMtMcuBandwidthNotif] DstMt.%d BandWidth ntf has been ignored due to ETH!\n", byDstMtId);
        return;
    }

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
        Mt2Log( "[ProcMtMcuBandwidthNotif] SelectSee SrcMtId.%d, DstMtId.%d, wBitRate.%d\n", 
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

	Mt2Log( "[ProcMtMcuBandwidthNotif] IsUseAdapter.%d, SrcMtId.%d, DstMtId.%d, wBitRate.%d, wMinBitRate.%d\n", 
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
                OspPrintf(TRUE, FALSE, "[ProcMtMcuBandwidthNotif] req bitrate < second bitrate, ignore it\n"); 
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
                    Mt2Log("[ProcMtMcuBandwidthNotif] change vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wVidBasBitrate);
                    ChangeAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
                else
                {
                    Mt2Log("[ProcMtMcuBandwidthNotif] start vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wVidBasBitrate);
                    StartAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }                   

                StartSwitchToSubMt(m_tVidBasEqp, m_byVidBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
            else if (m_tConf.m_tStatus.IsVidAdapting())//������Ҫ�����������ʣ�ֱ�ӽ�����������������mt(�������������ն˵Ľ������ʸ��͵����)
            {
                Mt2Log("[ProcMtMcuBandwidthNotif] switch vid bas to mt<%d>\n", tMt.GetMtId());
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
                    Mt2Log("[ProcMtMcuBandwidthNotif]change br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wBasBitrate);
                    ChangeAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
                else
                {
                    Mt2Log("[ProcMtMcuBandwidthNotif]start br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wBasBitrate);
                    StartAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }                   
                
                StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
            else if (m_tConf.m_tStatus.IsBrAdapting())//������Ҫ�����������ʣ�ֱ�ӽ�����������������mt(�������������ն˵Ľ������ʸ��͵����)
            {
                Mt2Log("[ProcMtMcuBandwidthNotif] switch br bas to mt<%d>\n", tMt.GetMtId());
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
                Mt2Log("[ProcMtMcuOtherMsg] Mt<%d,%d> bitrate : %u, %u, %u, %u\n", 
                    tMt.GetMcuId(), tMt.GetMtId(), tMtBitrate.GetRecvBitRate(), tMtBitrate.GetSendBitRate(), 
                    tMtBitrate.GetH239RecvBitRate(), tMtBitrate.GetH239SendBitRate());
            }            
            break;

        case MT_MCU_GETMTVERID_ACK:
            {
                u8 byHWVerId = *(u8*)cServMsg.GetMsgBody();
                LPCSTR lpszSWVerId = (LPCSTR)(cServMsg.GetMsgBody() + sizeof(u8));
                m_ptMtTable->SetHWVerID(tMt.GetMtId(), byHWVerId);
                m_ptMtTable->SetSWVerID(tMt.GetMtId(), lpszSWVerId);
            }
            break;
            
        case MT_MCU_GETMTVERID_NACK:
            {
                //FIXME: �첽������ȡ�����Ǵ������ϱ�
                CallLog( "[ProcMtMcuOtherMsg] Get mt.%d VerId failed due to reason.%d\n", tMt.GetMtId(), cServMsg.GetErrorCode() );
            }
            break;
            
		default:
			return;
		}

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
			ConfLog( FALSE, "MCS set mt%d bitrate to %d !\n", tMt.GetMtId(), ntohs( wBitRate ) );
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
    CServMsg    cMsg( pcMsg->content, pcMsg->length );
	u8 bySrcMtId = cServMsg.GetSrcMtId();
	TLogicalChannel tLogicalChannel;
	u8  byMuteType;
	u8  byMuteOpenFlag;
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
		if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
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

			//��ͬʱ������mt��/��������
			byMtNum = cServMsg.GetMsgBodyLen() / (sizeof(TMt)+2);
			for( byIndex=0; byIndex<byMtNum; byIndex++ )
			{
				ptDstMt = (TMt*)( cServMsg.GetMsgBody() + byIndex*(sizeof(TMt)+2) );
				byMuteOpenFlag = *( (u8*)ptDstMt + sizeof(TMt) );
				byMuteType     = *( (u8*)ptDstMt + sizeof(TMt) + 1 );
				
				u8 byVendor = m_ptMtTable->GetManuId(ptDstMt->GetMtId());
				if(byVendor == MT_MANU_KDC)
				{
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
                    AdjustKedaMcuAndTaideMuteInfo( ptDstMt, byMuteType, byMuteOpenFlag );
				}
                
			}
			return;
				
			break;	

		default:
			return;
		}

		//send messge
		cServMsg.SetDstMtId( tDstMt.GetMtId() );
		if( pcMsg->event != MCS_MCU_MTAUDMUTE_REQ )
		{
			CServMsg cMsg = cServMsg;
			cServMsg.SetMsgBody( cMsg.GetMsgBody() + sizeof(TMt), cMsg.GetMsgBodyLen() - sizeof(TMt) );
		}

		SendMsgToMt( tDstMt.GetMtId(), wEvent, cServMsg );
		
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
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
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt;
	if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ)
	{
		tMt = *( TMt * )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));
	}
	else
	{
		tMt = *( TMt * )cServMsg.GetMsgBody();
	}

	STATECHECK

	//���������ն��б���, NACK!
    TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMcuId());
	//�������û��
	if(tMtInfo.IsNull())
	{
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//�Ѿ��������,NACK
	if( tMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{	
		cServMsg.SetErrorCode( ERR_MCU_CALLEDMT_JOINEDCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//���ڱ�MCU��
	if( !tMt.IsLocal())
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
		SendMsgToMt( tMt.GetMcuId(), MCU_MCU_REINVITEMT_REQ, cServMsg );
		
		cServMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt));
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
	}	

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
 
	//ɾ�����ն�
	if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ &&
	   // vcs��������Դɾ��������ֻ���к��С��Ҷϲ���
	   // ɾ���������¼�PU��IP��������Ϣ��ɾ�������ں�����callalertͨ�����޷����������PU������Ϣ
	   m_tConf.GetConfSource() != VCS_CONF)
	{
		cServMsg.SetMsgBody((u8 *)&tMt, sizeof(tMt));
		SendReplyBack( cServMsg, MCU_MCU_DELMT_NOTIF );
	}

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
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt;
	if(cServMsg.GetEventId() == MCU_MCU_DROPMT_REQ)
	{
		tMt = *( TMt * )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));
	}
	else
	{
		tMt = *( TMt * )cServMsg.GetMsgBody();
	}

	STATECHECK

	//���������ն��б���, NACK!
	TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMcuId());
	//�������û��
	if(tMtInfo.IsNull())
	{
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//�Ѿ����ڻ�����,NACK
	if( !tMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{	
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
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
		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		
		SendMsgToMt( tMt.GetMcuId(), MCU_MCU_DROPMT_REQ, cServMsg );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

		return;
	}
	
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
			
	//�����ն˺���ģʽ
	// ����VCS�������ģʽ��������ĺ���(���̶ֹ���Դ�Ϳɵ�����Դ)���ɸ���
	if (m_tConf.GetConfSource() != VCS_CONF)
	{
		m_ptMtTable->SetCallMode( tMt.GetMtId(),  CONF_CALLMODE_NONE);
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
	STATECHECK;

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	u8 bySrcMtId = cServMsg.GetSrcMtId();
	u8 byMode    = *(cServMsg.GetMsgBody());

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

    // ����ؼ�֡���ն�, zgc, 2008-08-15
    TMt tMt = m_ptMtTable->GetMt( bySrcMtId );
	Mt2Log("[ProcMtMcuFastUpdatePic] Accept keyframe(mode:%d) require from mt(%d)\n", byMode, bySrcMtId);
	if(byMode == MODE_SECVIDEO)
	{
		if( m_tDoubleStreamSrc.IsNull() )
		{
			return;
		}

        if ( IsHDConf( m_tConf ) )
        {
            TSimCapSet tSrcSCS;
            if (m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE))
            {
                u8   byMediaType = MEDIA_TYPE_NULL;
                TEqp tHdBas;
                u8   byChnId;
                u8   byRecvIdx;
                u8   byRes = 0;
                if (m_cMtRcvGrp.GetMtMediaRes(tMt.GetMtId(), byMediaType, byRes, TRUE) &&
                    m_cBasMgr.GetBasResource(byMediaType,
                                             byRes,
                                             tHdBas,
                                             byChnId,
                                             byRecvIdx,
                                             TRUE, 
                                             IsDSSrcH263p() /*|| IsConfDualEqMV(m_tConf)*/))
                {
                    NotifyFastUpdate(tHdBas, byChnId);
                }
                else
                {
                    //zbq[01/17/2009] h263p����������Դ����ʱ ��������
                    TDStreamCap tDSCap;
                    memset(&tDSCap, 0, sizeof(tDSCap));
                    GetDSBrdSrcSim(tDSCap);
                    if (MEDIA_TYPE_H263PLUS == tDSCap.GetMediaType())
                    {
                        //��������̶����
                        u8 byProximalType = MEDIA_TYPE_H264;
                        u8 byProximalRes = VIDEO_FORMAT_XGA;
                        
                        //ȡ���������bas��Դ
                        BOOL32 bRet = m_cBasMgr.GetBasResource(byProximalType,
                                                               byProximalRes,
                                                               tHdBas,
                                                               byChnId,
                                                               byRecvIdx, TRUE, TRUE);
                        tHdBas.SetConfIdx(m_byConfIdx);
                        if (!bRet)
                        {
                            ConfLog(FALSE, "[ProcMtMcuFastUpdatePic] get proximal bas src failed(dual)!\n");
                            return;
                        }
                        //�����BAS���� �ؼ�֡
                        NotifyFastUpdate(tHdBas, byChnId);
                    }
                    else
                    {
                        ConfLog(FALSE, "[ProcMtMcuFastUpdatePic] get bas src for mt.%d<%d, %s> failed, h263p.%d!\n",
                            tMt.GetMtId(), byMediaType, GetResStr(byRes), IsDSSrcH263p());
                    }
                }
            }
            else
            {
                NotifyFastUpdate(m_tDoubleStreamSrc, MODE_SECVIDEO);
            }
        }
        else
        {
            //zbq[05/20/2009] ����H263p����ؼ�֡���� ֧��
            if (m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE))
            {
                u8   byMediaType = MEDIA_TYPE_NULL;
                TEqp tHdBas;
                u8   byChnId;
                u8   byRecvIdx;
                u8   byRes = 0;

                if (m_cMtRcvGrp.GetMtMediaRes(tMt.GetMtId(), byMediaType, byRes, TRUE) &&
                    m_cBasMgr.GetBasResource(byMediaType,
                                             byRes,
                                             tHdBas,
                                             byChnId,
                                             byRecvIdx,
                                             TRUE, 
                                             IsDSSrcH263p() /*|| IsConfDualEqMV(m_tConf))*/))
                {
                    NotifyFastUpdate(tHdBas, byChnId);
                }
                else
                {
                    ConfLog(FALSE, "[ProcMtMcuFastUpdatePic] get bas dual for mt.%d<%d, %s> failed, h263p.%d!\n",
                        tMt.GetMtId(), byMediaType, GetResStr(byRes), IsDSSrcH263p());
                }
            }
            else
            {
                NotifyFastUpdate(m_tDoubleStreamSrc, MODE_SECVIDEO);
            }
        }
	}
	else
	{
        //  xsl [5/22/2006] �Ƿ�ԴΪbas
		TMt  tSrcMt;
		tSrcMt.SetNull();
        BOOL32 bRet = m_ptMtTable->GetMtSrc(bySrcMtId, &tSrcMt, MODE_VIDEO);
        u8 byAdpType;
		// ���㲥Դ
		if (tSrcMt == m_tVidBrdSrc)
		{
			// �����ն��Ƿ��ڸ�����������
			if (m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId()))
			{
	            TEqp tHdBas;
                u8 byChnId = 0;
                u8 byOutIdx = 0;
                u8 byRes = 0;
                u8 byMediaType = MEDIA_TYPE_NULL;

                if (m_cMtRcvGrp.GetMtMediaRes(tMt.GetMtId(), byMediaType, byRes) &&
                    m_cBasMgr.GetBasResource(byMediaType, byRes, tHdBas, byChnId, byOutIdx))
                {
					Mt2Log("mt(mtid:%d) its srcmt(mcuid:%d, mtid:%d) require framekey to hdbas(id:%d, inputchnl:%d, outputchnl:%d)\n",
						   tMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tHdBas.GetEqpId(), byChnId, byOutIdx);
                    NotifyFastUpdate(tHdBas, byChnId);
                    return;
                }
                else
                {
                    //zbq[01/05/2009] ��ʱ���������������� ���뱣��
                    TSimCapSet tSrcSim;
                    memset(&tSrcSim, 0, sizeof(tSrcSim));
                    GetMVBrdSrcSim(tSrcSim);
                    if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
                        MEDIA_TYPE_H264 != tSrcSim.GetVideoMediaType() 
                        //ȡ����������
                        /*&&
                        0 == m_tConf.GetSecBitRate()*/)
                    {
                        //FIXME: ��ʱȡmain������ˢ��RcvChn
                        TCapSupport tCap;
                        m_ptMtTable->GetMtCapSupport(bySrcMtId, &tCap);

                        u8 byAdjType = m_tConf.GetMainVideoMediaType();
                        u8 byAdjRes = m_tConf.GetMainVideoFormat();

                        if (byAdjType == tCap.GetMainVideoType() &&
                            byAdjRes == tCap.GetMainVideoResolution())
                        {
                            //���ն��Ƿ���Ҫ�߽�������
                            u8 byProximalType = 0;
                            u8 byProximalRes = 0;
                            BOOL32 bRet = GetProximalGrp(byAdjType, byAdjRes, 
                                                         byProximalType, byProximalRes);
                            if (!bRet)
                            {
                                ConfLog(FALSE, "[ProcMtMcuFastUpdatePic] no proximal grp for <%d, %d\n", byAdjType, byAdjRes);
                                return;
                            }
                            //ȡ���������bas��Դ
                            BOOL32 bRet1 = m_cBasMgr.GetBasResource(byProximalType, 
                                                                    byProximalRes,
                                                                    tHdBas, byChnId, byOutIdx);
                            tHdBas.SetConfIdx(m_byConfIdx);
                            if (!bRet1)
                            {
                                ConfLog(FALSE, "[ProcMtMcuFastUpdatePic] get proximal bas src failed!\n");
                                return;
                            }
                            //����յ�bas��Դ����ؼ�֡
                            NotifyFastUpdate(tHdBas, byChnId);
                            return;
                        }
                    }
                }
			}
			// �����ն��Ƿ��ڱ���������
			else if (IsMtSrcBas(bySrcMtId, MODE_VIDEO, byAdpType))
			{
			    // ���� [6/2/2006] Ҫ��ȡ��Ӧ�������TEqp�ṹ
				// tSrcMt �����յ�Դ�����������������
				TEqp tBasEqp;
				u8 byChnl;
				if (ADAPT_TYPE_VID == byAdpType)
				{
					tBasEqp = m_tVidBasEqp;
					byChnl = m_byVidBasChnnl;
				}
				else if (ADAPT_TYPE_BR == byAdpType)
				{
					tBasEqp = m_tBrBasEqp;
					byChnl = m_byBrBasChnnl;
				}            
				else if (ADAPT_TYPE_CASDVID == byAdpType)
				{
					tBasEqp = m_tCasdVidBasEqp;
					byChnl = m_byCasdVidBasChnnl;
				}
				Mt2Log("mt(mtid:%d its srcmt(mcuid:%d, mtid:%d) require framekey to sdbas(id:%d, chnl:%d)\n",
					   tMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tBasEqp.GetEqpId(), byChnl);
				NotifyFastUpdate(tBasEqp, byChnl);
				return;
			}
            
		}
		// ��ѡ��Դ
		else
		{
			u8 bySelSrcId = (tSrcMt.IsLocal() ? tSrcMt.GetMtId() : tSrcMt.GetMcuId());
			u8 bySelDstId = tMt.GetMtId();
			TEqp tBasEqp;
			u8 byChnl;
			if (m_cSelChnGrp.IsMtInSelAdpGroup(bySelSrcId, bySelDstId, &tBasEqp, &byChnl))
			{
				Mt2Log("mt(mtid:%d its srcmt(mcuid:%d, mtid:%d) require framekey to bas(id:%d, chnl:%d) for seladp\n",
					   tMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tBasEqp.GetEqpId(), byChnl);
				NotifyFastUpdate(tBasEqp, byChnl);
				return;
			}
		}

        
        if (!bRet ||
            TYPE_MT != tSrcMt.GetType() ||
            !m_tConfAllMtInfo.MtJoinedConf(tSrcMt.GetMtId()))
        {
            if (TYPE_MCUPERI == tSrcMt.GetType() &&
                EQP_TYPE_VMP == tSrcMt.GetEqpType())
            {
				// ��������1·
				u8 bySrcChnnl = 0;
				u8 byMtMediaType = 0;
				u8 byMtRes = 0;
				m_cMtRcvGrp.GetMtMediaRes(tMt.GetMtId(), byMtMediaType, byMtRes);
				bySrcChnnl = GetVmpOutChnnlByRes(byMtRes, m_tVmpEqp.GetEqpId(), byMtMediaType);
				NotifyFastUpdate(tSrcMt, bySrcChnnl); 
            }
            return;
        } 

		if (bRet && !tSrcMt.IsNull() && m_tConfAllMtInfo.MtJoinedConf(tSrcMt.GetMcuId(), tSrcMt.GetMtId()))
		{
			Mt2Log("mt(mtid:%d require framekey to mt(mcuid:%d, mtid:%d)\n",
				   tMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId());
			NotifyFastUpdate(tSrcMt, MODE_VIDEO);
		}
        
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
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/02/19  3.6			����                  ����
=============================================================================*/
void CMcuVcInst::StopDoubleStream( BOOL32 bMsgSrcMt, BOOL32 bForceClose )
{
	TMt tOldDStreamSrc = m_tDoubleStreamSrc;

	if(m_tDoubleStreamSrc.IsNull())
	{
		return;
	}

    TLogicalChannel tNullLogicChnnl;
	TLogicalChannel tH239LogicChnnl;
	CServMsg cServMsg;
	
	//sony G70SP��H263-H239ʱ���û˫��,�Ҳ�ʹ�����ƶ�ʹ��FlowControlIndication���з��ͼ����ر� ����
	//POLYCOM7000�ľ�̬˫��ʱ��ʹ�����ƽ��з��ͼ����ر�
	//���ں��н���ʱ����,֮������ڼ䱣��ͨ����ֱ���˳�����
	if( ( MT_MANU_SONY == m_ptMtTable->GetManuId(m_tDoubleStreamSrc.GetMtId()) && 
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
        m_ptMtTable->SetMtVideo2Send( m_tDoubleStreamSrc.GetMtId(), FALSE );

        TMtStatus tMtStatus;
        m_ptMtTable->GetMtStatus(m_tDoubleStreamSrc.GetMtId(), &tMtStatus);
                
        CServMsg cServMsg;
        cServMsg.SetSrcMtId( m_tDoubleStreamSrc.GetMtId() );
        cServMsg.SetEventId( MT_MCU_MTSTATUS_NOTIF );
        cServMsg.SetMsgBody( (u8*)&tMtStatus, sizeof(TMtStatus) );
        g_cMcuVcApp.SendMsgToConf( m_tConf.GetConfId(), MT_MCU_MTSTATUS_NOTIF,
                            cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
    }

	TMt tDstMt;
	//�ر������ն˵�H.239ͨ��
    u8 byLoop = 1;
	for( ; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) && 
			m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tH239LogicChnnl, TRUE ) )
		{	
			// xliang [11/14/2008] PolycomMCU ˫��ͨ�����أ����Է�˫��Դ��polycom�նˣ����ͷ�����(Ŀǰ֧��polyMCU������keda������ȡ���Ʒ�ʽ)
			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byLoop) &&
				( MT_TYPE_MMCU == m_ptMtTable->GetMtType(byLoop) ||
				MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) ) )
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
				continue;
			}
			u8 byOut = 1;
			cServMsg.SetMsgBody( ( u8 * )&tH239LogicChnnl, sizeof( tH239LogicChnnl ) );	
			cServMsg.CatMsgBody( &byOut, sizeof(byOut) );
			SendMsgToMt( byLoop, MCU_MT_CLOSELOGICCHNNL_CMD, cServMsg );
			
			tDstMt = m_ptMtTable->GetMt( byLoop );
            
            g_cMpManager.StopSwitchToSubMt(tDstMt, MODE_SECVIDEO, TRUE);
			m_ptMtTable->SetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tNullLogicChnnl, TRUE );

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
                Mt2Log("[StopDoubleStream]tDstMt.%d, MtReqBitrate = %d, adjust restore\n", tDstMt.GetMtId(), wRealBR);                
            }
            else
            {
                u16 wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(tDstMt.GetMtId());
                m_ptMtTable->SetMtReqBitrate(tDstMt.GetMtId(), wMtDialBitrate, LOGCHL_VIDEO);
                Mt2Log("[StopDoubleStream]tDstMt.%d, MtReqBitrate = %d, normally restore\n", tDstMt.GetMtId(), wMtDialBitrate);
            }

			//���ǻ���ָ�����������������
#ifdef _SATELITE_
			
			BOOL32 bHd = ::topoGetMtInfo(LOCAL_MCUID, tDstMt.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
			if (!bHd && !m_tVidBrdSrc.IsNull())
			{
				StartSwitchToSubMtFromAdp(tDstMt.GetMtId(), FALSE);
			}
#endif


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

    //zbq[02/18/2008] ֹͣ˫����������VMP�Ĺ㲥����
    if ( m_tConf.m_tStatus.IsBrdstVMP() )
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
    
	//�ڶ�·��Ƶ�Ķ����ش�
	if( m_tConf.m_tStatus.IsPrsing() )
	{
		StopPrs( PRSCHANMODE_SECOND );
	}

    // xsl [4/29/2006] �ָ�����������
    TSimCapSet tDstSCS, tSrcSCS;
    if (m_tConf.m_tStatus.IsVidAdapting())
    {        
        //Ŀǰ�ݲ�����˫�ٻ���ֱ�������ָ�...
        if (IsNeedVidAdapt(tDstSCS, tSrcSCS, m_wVidBasBitrate))
        {
            ChangeAdapt(ADAPT_TYPE_VID, m_wVidBasBitrate, &tDstSCS, &tSrcSCS);
        }        
    }
    if (m_tConf.m_tStatus.IsBrAdapting())
    {
        if (IsNeedBrAdapt(tDstSCS, tSrcSCS, m_wBasBitrate))
        {
            ChangeAdapt(ADAPT_TYPE_BR, m_wBasBitrate, &tDstSCS, &tSrcSCS);
        }        
    }

    //zbq[12/11/2008] �������䣺ͣ˫�����������������������
    StopAllHdBasSwitch(TRUE);
    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_DOUBLEVID, FALSE);

    if (m_tConf.m_tStatus.IsHdVidAdapting())
    {
        RefreshBasParam4AllMt(TRUE);
    }

	//zbq[09/12/2008] ʱ�����
	m_tDoubleStreamSrc.SetNull();

    //�����˫��ʱ���ն˷����ˣ���Ӧ�������ն˵ķ������ʻָ�
	TLogicalChannel tLogicalChannel;
    if (!m_tVidBrdSrc.IsNull() && TYPE_MT == m_tVidBrdSrc.GetType())
    {
        if (m_ptMtTable->GetMtLogicChnnl(m_tVidBrdSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
        {            
            tLogicalChannel.SetFlowControl(m_ptMtTable->GetSndBandWidth(m_tVidBrdSrc.GetMtId()));
            cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(m_tVidBrdSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
        }
    }    
    
	//����˫���ն����ڷ��͵�һ·�������ָ�����
	if(IsDStreamMtSendingVideo(tOldDStreamSrc))
	{
		if (m_ptMtTable->GetMtLogicChnnl(tOldDStreamSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
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
            tOtherMt = tStatus.GetSelectMt(MODE_VIDEO);
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

    RefreshRcvGrp();

#ifdef _SATELITE_
	bySrcChn = m_tPlayEqp == m_tDoubleStreamSrc ? m_byPlayChnnl : 0;
	g_cMpManager.StopSatConfCast(tOldDStreamSrc, CAST_FST, MODE_SECVIDEO, bySrcChn);
#endif

#ifdef _SATELITE_
	RefreshConfState();
#endif

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
		m_byLastDsSrcMtId = m_tDoubleStreamSrc.GetMtId(); 
		StopDoubleStream(TRUE, FALSE);
	}

    // zbq [09/04/2007] ¼���˫�����󣬴˴������ܽ�����
    if ( TYPE_MCUPERI == tMt.GetType() && EQP_TYPE_RECORDER == tMt.GetEqpType())
    {
    }
    else
    {
        g_cMpManager.SetSwitchBridge(tMt, 0, MODE_SECVIDEO);
    }

	TLogicalChannel tH239LogicChnnl;
    TCapSupport tCapSupport;
	CServMsg cSendMsg;
	//�������ն˵�H.239ͨ��
	for(u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
	{
        if (byLoop == tMt.GetMtId() && TYPE_MCUPERI != tMt.GetType())
        {
            continue;
        }

        if (!m_tConfAllMtInfo.MtJoinedConf(byLoop))
        {
            continue;
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

		// xliang [12/19/2008]  �����ϴη���˫����MT,��ʱ����˫��ͨ����
		//(���յ�close LC notif֮���ȥ�򿪣���֤֮ǰ˫��ͨ��ȷʵ�ѹر�)
		if( byLoop == m_byLastDsSrcMtId )
		{
			m_tLogicChnnl = tLogicChnnl; // xliang [12/19/2008] ��¼logical channel��Ϊ��˫����������ʱ����
			continue;
		}
        //��ͨ�ն˴�˫��
        if(!m_ptMtTable->GetMtCapSupport( tOtherMt.GetMtId(), &tCapSupport ))
        {
            continue;
        }
		// xliang [11/14/2008]  ���polycom˫�������⴦��
		//polycom��˫��ͨ�����ն˺���ʱ��ȷ��������˴򿪲���;
		//�˴��Է�����Polycom��MT��ֱ��flowctrl�������. ��Polycom��MCU�������������ƣ��������Ƴɹ�����flowctrl
		if (MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byLoop) &&
            (MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) || 
			MT_TYPE_MMCU == m_ptMtTable->GetMtType(byLoop)))
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
				CallLog("[StartDoubleStream] MCU_POLY_GETH239TOKEN_REQ to PolyMCU.%d\n", byLoop);
			}
			else
			{
				ConfLog( FALSE, "[StartDoubleStream] No send MCU_POLY_GETH239TOKEN_REQ to polyMCU due to Forward chnnl closed, try to reopen it, start DS once more, please\n" );
			}
		}
		else
		{
			McuMtOpenDoubleStreamChnnl(tOtherMt, tLogicChnnl, tCapSupport);  
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

        // ֪ͨ����ն�˫��״̬�ı� [02/05/2007-zbq]
        TMtStatus tMtStatus;
        m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus);
        
        CServMsg cServMsg;
        cServMsg.SetSrcMtId( tMt.GetMtId() );
        cServMsg.SetEventId( MT_MCU_MTSTATUS_NOTIF );
        cServMsg.SetMsgBody( (u8*)&tMtStatus, sizeof(TMtStatus) );
        g_cMcuVcApp.SendMsgToConf( m_tConf.GetConfId(), MT_MCU_MTSTATUS_NOTIF,
                            cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
    }

   
    
	//��������, ��ǿ�Ƶ�����һ·����Դ�ķ��������۰�,û�������������FlowControl����
    if (!m_tVidBrdSrc.IsNull() && TYPE_MT == m_tVidBrdSrc.GetType())
    {
        CServMsg cServMsg;
        TLogicalChannel tLogicalChannel;
        if (TRUE == m_ptMtTable->GetMtLogicChnnl(m_tVidBrdSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
        {
            //�����˫��ʱ���ն˷����ˣ���Ӧ�������ն˵ķ������ʼ��룬��������ͨ����Ϣ��
            tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth(m_tVidBrdSrc.GetMtId())));
            cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(m_tVidBrdSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
        }
    }

	RefreshRcvGrp();

    //˫��¼��
    AdjustRecordSrcStream( MODE_SECVIDEO );

    //ǿ�Ƶ������������� -- modify bas 2
    TSimCapSet tDstSimCapSet, tSrcSimCapSet;
    u16 wAdaptBitRate;
    if (m_tConf.m_tStatus.IsVidAdapting())
    {        
        if (IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
        {
            ChangeAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
        }        
    }
    if (m_tConf.m_tStatus.IsBrAdapting())
    {
        if (IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, &m_tVidBrdSrc))
        {
            ChangeAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
        }        
    }
    
    if (m_cMtRcvGrp.IsNeedAdp(FALSE))
    {
        if (!m_tConf.m_tStatus.IsHDDoubleVidAdapting())
        {
            StartHDDSBrdAdapt();
        }
        else
        {
			// fxh[20090213]bas��������˫��Դ�л�ʱ���ж��������������Ƿ����޸�
			TDStreamCap tNewDSCap;
			GetDSBrdSrcSim(tNewDSCap);
			if (tOldDSCap.GetMediaType() != tNewDSCap.GetMediaType())
			{
				u8 byDSBasNum = 0;

                //������Դ��������������
                TBasChn atBasChn[MAXNUM_CONF_MVCHN];
                m_cBasMgr.GetChnGrp(byDSBasNum, atBasChn, CHN_ADPMODE_DSBRD);

				for(u8 byIdx = 0; byIdx < byDSBasNum; byIdx ++)
				{
                    if (!atBasChn[byIdx].IsNull())
                    {
                        ChangeHDAdapt(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId());
                    }
                }
			}
			// ���������������
            RefreshBasParam4AllMt(TRUE, TRUE);
            StartAllHdBasSwitch(TRUE);
        }
    }

	// ˢ���������
    if ( m_tConf.m_tStatus.IsHdVidAdapting() )
    {
        RefreshBasParam4AllMt(TRUE);
    }
    
    //�鲥�����鲥����
    if (m_tConf.GetConfAttrb().IsMulticastMode())
    {
        g_cMpManager.StartMulticast(m_tDoubleStreamSrc, 0, MODE_SECVIDEO);
    }
#ifndef _SATELITE_
    //�����鲥
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        g_cMpManager.StartDistrConfCast(m_tDoubleStreamSrc, MODE_SECVIDEO);      
    }
#endif

#ifdef _SATELITE_
	u8 bySrcChn = m_tPlayEqp == m_tDoubleStreamSrc ? m_byPlayChnnl : 0;
	g_cMpManager.StartSatConfCast(m_tDoubleStreamSrc, CAST_FST, MODE_SECVIDEO, bySrcChn);
#endif

    // guzh [10/30/2007] 
    if (m_tDoubleStreamSrc.GetType() == TYPE_MT)
    {
        //�µ�˫����������ʱ��ǿ�ƹؼ�֡
        SetTimer(MCUVC_SECVIDEO_FASTUPDATE_TIMER, 1200, 100*m_tDoubleStreamSrc.GetMtId()+1);	       
    }

    // guzh [10/30/2007] ע�����Ϣ�Ĵ�������������˫��Դ���͵���������������һ��Ҫ���ö�ʱ��
    //�ȴ�1s��˫�������ն˷���flowcontrol����˫������
    SetTimer(MCUVC_SENDFLOWCONTROL_TIMER, 1200);

#ifdef _SATELITE_
	RefreshConfState();
#endif

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
void CMcuVcInst::ProcSendFlowctrlToDSMtTimeout(const CMessage * pcMsg )
{
    if (m_tDoubleStreamSrc.IsNull())
    {
        ConfLog(FALSE, "[ProcSendFlowctrlToDSMtTimeout] no double stream source mt!\n");
        return;
    }

    CServMsg cServMsg0, cServMsg1;
    TLogicalChannel tLogicalChannel;
    if (m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
    {
        tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth(m_tDoubleStreamSrc.GetMtId())));
        tLogicalChannel.SetCurrFlowControl(0xffff);//���ϴα����flowcontrol�ı䣬��ֹ������
        cServMsg0.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));        
    }

	TMtStatus tStatus;
	BOOL32 bIsOpenLogChnn = TRUE;
    u16 wMtDialBitrate;     
    u16 wMinDialBitrate = m_ptMtTable->GetRcvBandWidth(m_tDoubleStreamSrc.GetMtId());
    u8 byLoop = 1;
    for (; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {         
        if (byLoop == m_tDoubleStreamSrc.GetMtId() || !m_tConfAllMtInfo.MtJoinedConf(byLoop))
        {
            continue;
        }

        //zbq[07/28/2009] ˫��flowctrl���������������ն˲���Ӧflowctrl��HD-AdpParam��Ȼ����
        if (IsHDConf(m_tConf) &&
            m_cMtRcvGrp.IsMtNeedAdp(byLoop, FALSE))
        {
            continue;
        }

        wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(byLoop);
		
        //�ж��ն��Ƿ������Ƶ�߼�ͨ��, ��ֹ������ʱ���0
		bIsOpenLogChnn = m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tLogicalChannel, TRUE);
		
        //zbq[09/25/2008] ������������0�ĵ�һ·ͨ��Ӱ�쵽˫������
        if (0 != wMtDialBitrate &&
			wMinDialBitrate > wMtDialBitrate && bIsOpenLogChnn )
        {
            wMinDialBitrate = wMtDialBitrate;
        }
    }
	
    if (m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE))
    {
        tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(wMinDialBitrate, FALSE));
        tLogicalChannel.SetCurrFlowControl(0xffff);//���ϴα����flowcontrol�ı䣬��ֹ������
        cServMsg1.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));        
    }
		
	BOOL32 bOtherMtSrc = IsDStreamMtSendingVideo(m_tDoubleStreamSrc);
    //�ȷ�С��flowcontrol
    if (GetDoubleStreamVideoBitrate(m_tConf.GetBitRate()) < m_tConf.GetBitRate()/2)
    {
		if (bOtherMtSrc)
		{
			SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg0);     
		}
        SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg1);        
    }
    else
    {
        SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg1);    
        if (bOtherMtSrc)
		{
			SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg0);             
		}		
    }
    
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
        if ( !tOtherMt.IsNull() &&
             !(tOtherMt == m_tDoubleStreamSrc) )
        {
            // �����ն˵�ǰ�ķ������ʡ�˫�����շ�����ȷ��flowcontrol��ֵ
            m_ptMtTable->GetMtLogicChnnl( tOtherMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, TRUE);
            wBitrate = min ( m_ptMtTable->GetSndBandWidth(tOtherMt.GetMtId()), m_ptMtTable->GetRcvBandWidth(byLoop) );
            wBitrate = GetDoubleStreamVideoBitrate(wBitrate);
            wBitrate = min( wBitrate, tLogicalChannel.GetFlowControl() );
            tLogicalChannel.SetFlowControl(wBitrate);
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
		McuMtOpenDoubleStreamChnnl(tMt, tLogicalChannel, tCapSupport);
	}

	return;
}

/*=============================================================================
�� �� ���� GetDoubleStreamVideoBitrate
��    �ܣ� ��ȡ˫����Ƶ���ʣ�Ĭ�ϵ�һ·
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� u16  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/23  4.0			������                  ����
=============================================================================*/
u16  CMcuVcInst::GetDoubleStreamVideoBitrate(u16 wDialBitrate, BOOL32 bFst /*= TRUE*/)
{ 
    u8 byDStreamScale = m_tConf.GetDStreamScale();    
    
    u16 wSndBitrate = byDStreamScale*wDialBitrate/100; 
    return bFst ? (wDialBitrate - wSndBitrate) : wSndBitrate;
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
                                            const TLogicalChannel &tLogicChnnl, 
                                            const TCapSupport &tCapSupport)
{
	u32 dwRcvIp = 0;
    u16 wRcvPort = 0;
    TLogicalChannel tDStreamChnnl;
    CServMsg cMsg;

	// xliang [11/14/2008]  ���polycom˫�������⴦��
	if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) )
	{
		//Polycom��˫�����������ģ�ͨ����Ϣ����У�飬ֱ�Ӵ�
		if (tCapSupport.IsDStreamSupportH239() &&
			m_tConf.GetCapSupport().GetDStreamCapSet().GetMediaType() == tCapSupport.GetDStreamMediaType())
		{
			if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
			{
				dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
				if( dwRcvIp == 0 )
				{
					ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) mp not connected(Polycom)!\n",
						tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );		    
				}
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
					wChanBitrate = min(wChanBitrate, 
						GetDoubleStreamVideoBitrate(m_ptMtTable->GetDialBitrate(tMt.GetMtId()), FALSE) );
				}
			}
			tDStreamChnnl.SetFlowControl(wChanBitrate);
			
			tDStreamChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
			tDStreamChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 5 );        
			tDStreamChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
			tDStreamChnnl.SetActivePayload( GetActivePayload( m_tConf,tDStreamChnnl.GetChannelType() ) );        
			cMsg.SetMsgBody( ( u8 * )&tDStreamChnnl, sizeof( tDStreamChnnl ) );
			
			u8 byVideoFormat = m_tConf.GetVideoFormat(tDStreamChnnl.GetChannelType(), LOGCHL_SECVIDEO);
			cMsg.CatMsgBody( (u8*)&byVideoFormat, sizeof(u8) );
			
			SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cMsg );
			
			//FIXME: TEST
			CallLog("[McuMtOpenDoubleStreamChnnl] Mt.%d 's CurReqBR.%d, GetDSVBR.%d(polycom)\n",
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
			CallLog( "Open Second Video LogicChannel Request send to Mt%d, H239.%d, BR.%d, type: %s\n", 
				tMt.GetMtId(), tDStreamChnnl.IsSupportH239(),
				tDStreamChnnl.GetMediaType(), GetMediaStr( tDStreamChnnl.GetChannelType() ) );
		}
		else
		{
			CallLog("[McuMtOpenDoubleStreamChnnl] IsH239.%d, CapDSType.%d. ConfDSType.%d\n",
				tCapSupport.IsDStreamSupportH239(),
				tCapSupport.GetDStreamMediaType(),
				m_tConf.GetCapSupport().GetDStreamCapSet().GetMediaType());
		}
		return;
	}

    TLogicalChannel tLogicChnAdj = tLogicChnnl;  //  pengjie[8/11/2009]

	//zbq[09/02/2008] ˫��ͨ���ֱ���ƥ��У��
	u8 bySrcRes = tLogicChnnl.GetVideoFormat();
	u8 byDstRes = tCapSupport.GetDStreamCapSet().GetResolution();
	if (!IsDSResMatched(bySrcRes, byDstRes))
	{
		CallLog("[McuMtOpenDoubleStreamChnnl] Mt.%d's DS res dismatched<src.%d, dst.%d>, ignore it\n", 
			tMt.GetMtId(), bySrcRes, byDstRes);
		return;
	}
    
    BOOL32 bNonMVAdp = TRUE;

    //zbq[04/15/2009] ������飬������֧�����䣬˫������С�ڻ���֡�ʻ�ֱ��ʵ�H264ͨ��
    if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType()
        &&
        (m_tConf.GetConfAttrbEx().IsResEx1080() ||
         m_tConf.GetConfAttrbEx().IsResEx720()  ||
         m_tConf.GetConfAttrbEx().IsResEx4Cif() ||
         m_tConf.GetConfAttrbEx().IsResExCif()  ||
         (0 != m_tConf.GetSecVideoMediaType() &&
          MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType())) )
    {
        bNonMVAdp = FALSE;
    }

    BOOL32 bDstResMatch = IsResGE(tCapSupport.GetDStreamResolution(), tLogicChnnl.GetVideoFormat());
    BOOL32 bDstFpsMatch = tCapSupport.GetDStreamUsrDefFPS() >= tLogicChnnl.GetChanVidFPS();

    if (tLogicChnnl.GetChannelType() == MEDIA_TYPE_H264 &&
        tCapSupport.GetDStreamMediaType() == MEDIA_TYPE_H264)
    {
        if (bNonMVAdp)
        {
            if (!bDstFpsMatch || !bDstResMatch)
            {
                s8 achCapRes[32] = {0};
                strcpy(achCapRes, GetResStr(tCapSupport.GetDStreamResolution()));

                CallLog( "Mt%d second video media cap<Res:%s, Fps.%d> isn't match to conf<Res:%s, Fps.%d>, \
no open video logicChannel request send to it!\n", 
                    tMt.GetMtId(),
                    achCapRes, tCapSupport.GetDStreamUsrDefFPS(),
                    GetResStr(tLogicChnnl.GetVideoFormat()), tLogicChnnl.GetChanVidFPS());
                return;
            }
        }
        else
        {
            //zbq[05/08/2009] ���Զ˵�������˫��ͨ�� αװ
            if (!bDstFpsMatch)
            {
                tLogicChnAdj.SetChanVidFPS(tCapSupport.GetDStreamUsrDefFPS());
            }
            if (!bDstResMatch)
            {
                tLogicChnAdj.SetVideoFormat(tCapSupport.GetDStreamResolution());
            }
        }
    }

    //zbq[12/27/2008] ���ڶ�˫��������˫��ͨ�� αװ
    if (tLogicChnAdj.GetChannelType() != tCapSupport.GetDStreamMediaType() &&
        (tLogicChnAdj.GetChannelType() == m_tConf.GetCapSupport().GetDStreamMediaType() ||
         tLogicChnAdj.GetChannelType() == m_tConf.GetCapSupportEx().GetSecDSType()))
    {
        CallLog("Mt.%d's dschan been adjusted from.%d to %d, conf<%d, %d>\n",
            tMt.GetMtId(), tLogicChnAdj.GetChannelType(), tCapSupport.GetDStreamMediaType(),
            m_tConf.GetCapSupport().GetDStreamMediaType(), m_tConf.GetCapSupportEx().GetSecDSType());

        tLogicChnAdj.SetActivePayload(GetActivePayload(m_tConf, tCapSupport.GetDStreamMediaType()));
        tLogicChnAdj.SetChannelType(tCapSupport.GetDStreamMediaType());
        if (MEDIA_TYPE_H264 == tCapSupport.GetDStreamMediaType())
        {
            tLogicChnAdj.SetChanVidFPS(tCapSupport.GetDStreamUsrDefFPS());
        }
        else
        {
            tLogicChnAdj.SetChanVidFPS(tCapSupport.GetDStreamFrmRate());
        }
        tLogicChnAdj.SetVideoFormat(tCapSupport.GetDStreamResolution());
        tLogicChnAdj.SetSupportH239(tCapSupport.IsDStreamSupportH239());
    }
         

    if( tLogicChnAdj.IsSupportH239() == tCapSupport.IsDStreamSupportH239() &&
        tLogicChnAdj.GetChannelType() == tCapSupport.GetDStreamMediaType() )
    {
        if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
        {
            dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
            if( dwRcvIp == 0 )
            {
                ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) mp not connected!\n",
                    tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );		    
            }
            wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
            m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
        }
        
        if( tLogicChnAdj.IsSupportH239())
        {
            tDStreamChnnl.SetSupportH239( tCapSupport.IsDStreamSupportH239() );
            NotifyH239TokenOwnerInfo( &tMt );
        }
        tDStreamChnnl.SetMediaType( MODE_SECVIDEO );
        tDStreamChnnl.SetChannelType( tLogicChnAdj.GetChannelType() );
        tDStreamChnnl.SetChanVidFPS( tLogicChnAdj.GetChanVidFPS() );
        tDStreamChnnl.SetVideoFormat( tLogicChnAdj.GetVideoFormat() );

        //zbq[01/31/2008] FIXME: �����˫��ͨ���������������
        //tDStreamChnnl.SetFlowControl(  );

        // guzh [2008/03/07] ��˿���E1 ���ٺ�Polycom���������̶���ͨ�����ʵ��ϸ�У��
        // ��һ���ֿ�����MtAdp��
        u16 wChanBitrate = m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(), FALSE );

        if ( !( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) && m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId())) )
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
					wChanBitrate = min(wChanBitrate, 
						GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE) );
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
					wChanBitrate = min(wChanBitrate, 
						GetDoubleStreamVideoBitrate(m_ptMtTable->GetDialBitrate(tMt.GetMtId()), FALSE) );
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
				wChanBitrate = min(wChanBitrate, 
					GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId()), FALSE) );
			}
		}
        tDStreamChnnl.SetFlowControl(wChanBitrate);

        tDStreamChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
        tDStreamChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 5 );        
        tDStreamChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
        tDStreamChnnl.SetActivePayload( GetActivePayload(m_tConf, tDStreamChnnl.GetChannelType() ) );        
        cMsg.SetMsgBody( ( u8 * )&tDStreamChnnl, sizeof( tDStreamChnnl ) );
        
		//zbq[09/02/2008] Format���ն������򿪣�����ֻ����������
        u8 byVideoFormat = tDStreamChnnl.GetVideoFormat();
		if (byVideoFormat == 0)
		{
			byVideoFormat = m_tConf.GetVideoFormat(tDStreamChnnl.GetChannelType(), LOGCHL_SECVIDEO);
		}
		tDStreamChnnl.SetVideoFormat(byVideoFormat);
		
		cMsg.SetMsgBody( ( u8 * )&tDStreamChnnl, sizeof( tDStreamChnnl ) );
        cMsg.CatMsgBody( (u8*)&byVideoFormat, sizeof(u8) );

		cMsg.SetDstMtId( tMt.GetMtId() );
        
        SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cMsg );

        //FIXME: TEST
        CallLog("[McuMtOpenDoubleStreamChnnl] Mt.%d 's CurReqBR.%d, GetDSVBR.%d, Format.%d\n",
                 tMt.GetMtId(),
				 m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE),
				 GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())),
				 byVideoFormat);

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
        CallLog( "Open Second Video LogicChannel Request send to Mt%d, DStream.%d, type: %s\n", 
                tMt.GetMtId(), tDStreamChnnl.IsSupportH239(), GetMediaStr( tDStreamChnnl.GetChannelType() ) );
    }
    else
    {
        //��ӡ��Ϣ
        CallLog( "Mt%d <%d-H239.%d> not support any conf second video media type<%d:%d-H239.%d>, \
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
		//�������
		StopSwitchToSubMt( tDstMt.GetMtId(), MODE_VIDEO );

		m_ptMtTable->SetMtVideo2Recv( tDstMt.GetMtId(), TRUE );

        //zbq [08/23/2007] ¼�����˫��������Ҫ���ݷ���ͨ������
        u8 bySrcChn = m_tDoubleStreamSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
        g_cMpManager.StartSwitchToSubMt( m_tDoubleStreamSrc, bySrcChn, tDstMt, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE );

		//�����ն�״̬
		m_ptMtTable->SetMtSrc( tDstMt.GetMtId(), ( const TMt * )&m_tDoubleStreamSrc, MODE_VIDEO );

		CallLog( "StartSwitchDStreamToFirstLChannel Mt%d\n", tDstMt.GetMtId() );
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
#ifndef _SATELITE_
	if( m_tConf.GetCapSupport().IsDStreamSupportH239() )
	{
		return;
	}
#endif
	
	TLogicalChannel tLogicChnnl;
	if( MT_MANU_SONY == m_ptMtTable->GetManuId(tDstMt.GetMtId()) &&
        !m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicChnnl, TRUE ) && 
		m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_VIDEO, &tLogicChnnl, TRUE ) && 
		tLogicChnnl.GetChannelType() == m_tConf.GetCapSupport().GetDStreamMediaType() )
	{
		//�ָ�Ϊ����ƵԴ
		RestoreRcvMediaBrdSrc( tDstMt.GetMtId(), MODE_VIDEO, TRUE );
		CallLog( "StopSwitchDStreamToFirstLChannel Mt%d\n", tDstMt.GetMtId() );
	}

	
#ifdef _SATELITE_
	//���Ǳ����ն� �����������л�����������Դ
	if (!m_tVidBrdSrc.IsNull())
	{
		u8 bySrcChn = m_tVidBrdSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
		g_cMpManager.StartSatConfCast(m_tVidBrdSrc, CAST_SEC, MODE_VIDEO, bySrcChn);
	}

	BOOL32 bHd = ::topoGetMtInfo(LOCAL_MCUID, tDstMt.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
	if (!bHd && !m_tVidBrdSrc.IsNull())
	{
		StartSwitchToSubMtFromAdp(tDstMt.GetMtId(), FALSE);
	}
#endif

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
		if( NULL != ptMt )
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
			CServMsg cServMsg;
			TH239TokenInfo tH239Info;
			tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
			tH239Info.SetSymmetryBreaking( 0 );
			cServMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
			cServMsg.SetMsgBody((u8 *)&m_tH239TokenOwnerInfo.m_tH239TokenMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId(), MCU_MT_RELEASEH239TOKEN_CMD, cServMsg);

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
			if( byLoop == m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId() )
            {
                continue;
			}
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
		if( ptMt->GetMtId() != m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId() )
		{
			tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
			cServMsg.SetEventId(MCU_MT_OWNH239TOKEN_NOTIF);
			cServMsg.SetMsgBody((u8 *)ptMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( ptMt->GetMtId(), MCU_MT_OWNH239TOKEN_NOTIF, cServMsg);
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

	TMtStatus tMtStatus;
	TMt  tDstMt;
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
				if( (!tSelMt.IsLocal() && tSelMt.GetMcuId() == tSrcMt.GetMtId()) ||
					tMtStatus.GetSelectMt(MODE_AUDIO) == tSrcMt ) // ������Ƶ����
				{
					if( bStartSwitch )
					{
						StartSwitchToSubMt( tSrcMt, 0,  byMtIdxLp, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE );
					}
					else
					{
						StopSwitchToSubMt( tDstMt.GetMtId(), MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);
					}
				}
			}
		}
	}

	// ����VCS MCS��ǰѡ���ն�Ϊ����������ʱ��Ҫ������Ӧ�Ľ��𽻻�
	TMt tMCAudSrc;
	tMCAudSrc.SetNull();
	for (u8 byMCIdx = 1; byMCIdx <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byMCIdx++)
	{
		for (u8 byChnl = 0; byChnl < MAXNUM_MC_CHANNL; byChnl++)
		{
			if (g_cMcuVcApp.GetMcSrc(byMCIdx, &tMCAudSrc, byChnl, MODE_AUDIO) &&
				((!tMCAudSrc.IsLocal() && tMCAudSrc.GetMcuId() == tSrcMt.GetMtId()) ||
				  tMCAudSrc == tSrcMt))
			{
				if (bStartSwitch)
				{
				    g_cMpManager.StartSwitchToMc(tMCAudSrc, 0, byMCIdx, byChnl, MODE_AUDIO);
				}
				else
				{
					g_cMpManager.StopSwitchToMc(m_byConfIdx, byMCIdx, byChnl, MODE_AUDIO);
				}
			}
		}
		
	}

	return;
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
    /*
	if( FALSE == HasJoinedSpeaker() )
	{
        return;
	}
    */

    // xsl [9/20/2006] ��Ҫ���ǹ㲥ԴΪ�����������
	TMt tAudSrcMt = m_tAudBrdSrc;

	//�ն˾�����ֹͣ�ⲿ����ն˽�����Ƶ
	if( TRUE == bDecodeAud && TRUE == bOpenFlag )
	{
        //��ȡ��ѡ��
		StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE, FALSE );
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
            if (tAudSrcMt.GetType() == TYPE_MCUPERI && tAudSrcMt.GetEqpType() == EQP_TYPE_MIXER)
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
                StartSwitchToSubMt( tAudSrcMt, 0,  byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE );
            }            
		}
		else
		{	
            //�ָ���Ƶѡ��
			StartSwitchToSubMt( tSelectMt, 0,  byDstMtId, MODE_AUDIO, SWITCH_MODE_SELECT, FALSE );
		}
	}

    /*
    //  xsl [2/24/2006] ��ʱ����ԭ���Ĵ����߼���������ʱ�ٷſ�
	if( tAudSrcMt.GetMtId() != byDstMtId )
	{
		return;
	}
    */

	//�ն�������ֹͣ���ն����ⲿ������Ƶ
	if( FALSE == bDecodeAud && TRUE == bOpenFlag )
	{
        if (!tAudSrcMt.IsNull() && tAudSrcMt.GetType() == TYPE_MCUPERI)
        {
            // xsl [9/20/2006] ֹͣ���������Ľ���
            if (tAudSrcMt.GetEqpType() == EQP_TYPE_MIXER && m_ptMtTable->IsMtInMixGrp(byDstMtId))
            {
                StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
							        (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(byDstMtId)), 
                                     FALSE, MODE_AUDIO );
            }
        }
        else
        {
            //�������ն��Ƿ�����
		    if( tAudSrcMt.GetMtId() == byDstMtId )
		    {
			    for( u8 byMtLoop = 1; byMtLoop <= MAXNUM_CONF_MT; byMtLoop++ )
			    {
				    if( m_tConfAllMtInfo.MtJoinedConf( byMtLoop ) )
				    {	
					    StopSwitchToSubMt( byMtLoop, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE );
				    }
			    }
		    }
		    else
		    {
                //������ն˱������ն�ѡ��
			    ProcMtSelectdByOtherMt(byDstMtId, FALSE);
		    }
        }
		
	}

	//ȡ���ն��������ָ����ն����ⲿ������Ƶ
	if( FALSE == bDecodeAud && FALSE == bOpenFlag )
	{
        if (!tAudSrcMt.IsNull() && tAudSrcMt.GetType() == TYPE_MCUPERI)
        {
            // xsl [9/20/2006] �ָ����������Ľ���
            if (tAudSrcMt.GetEqpType() == EQP_TYPE_MIXER && m_ptMtTable->IsMtInMixGrp(byDstMtId))
            {
                TMt tSrcMt = m_ptMtTable->GetMt(byDstMtId);
                StartSwitchToPeriEqp(tSrcMt, 0, m_tMixEqp.GetEqpId(), 
								     (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(byDstMtId)),
								     MODE_AUDIO, SWITCH_MODE_SELECT);
            }
        }
        else
        {
            //�������ն��Ƿ�����
		    if( tAudSrcMt.GetType() == TYPE_MT && tAudSrcMt.GetMtId() == byDstMtId )
		    {
			    StartSwitchToAllSubMtJoinedConf( tAudSrcMt, 0 );
		    }
		    else
		    {
                //������ն˱������ն�ѡ��
			    ProcMtSelectdByOtherMt(byDstMtId, TRUE);
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
    �� �� ֵ�� BOOL32
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
    if (g_cMcuVcApp.IsSelInDoubleMediaConf())
    {
        return TRUE;
    }

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
            if (!IsHDConf(m_tConf) || !g_cMcuVcApp.IsSelAccord2Adp())
            {
                bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_AUDIO : MODE_NONE;

                ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap audio<%d> video<%d> vs DstCap audio<%d> video<%d>\n",
                    tSimCapSrc.GetAudioMediaType(), tSimCapSrc.GetVideoMediaType(),
                    tSimCapDst.GetAudioMediaType(), tSimCapDst.GetVideoMediaType() );
            }
            else
            {
                ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match loss, need adp: \n\t\tSrcCap audio<%d> video<%d> vs DstCap audio<%d> video<%d>\n",
                    tSimCapSrc.GetAudioMediaType(), tSimCapSrc.GetVideoMediaType(),
                    tSimCapDst.GetAudioMediaType(), tSimCapDst.GetVideoMediaType() );
                
                bAccord2Adp = TRUE;
            }
        }
        if ( MODE_NONE == bySwitchMode )
        {           
            ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap audio<%d> video<%d> vs DstCap audio<%d> video<%d>\n",
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
            if (!IsHDConf(m_tConf) || !g_cMcuVcApp.IsSelAccord2Adp())
            {
                bySwitchMode = MODE_NONE;
                ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap video<%d> vs DstCap video<%d>\n",
                     tSimCapSrc.GetVideoMediaType(), tSimCapDst.GetVideoMediaType() );
                return FALSE;
            }
            else
            {
                ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match loss, need adp: \n\t\tSrcCap video<%d> vs DstCap video<%d>\n",
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
            ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap audio<%d> vs DstCap audio<%d>\n",
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
            if (!IsHDConf(m_tConf) || !g_cMcuVcApp.IsSelAccord2Adp())
            {
                bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_AUDIO : MODE_NONE;
                ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap VidRes<%d> vs DstCap VidRes<%d>\n",
                    tSimCapSrc.GetVideoResolution(), tSimCapDst.GetVideoResolution() );
            }
            else
            {
                ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match loss, need adp: \n\t\tSrcCap VidRes<%d> vs DstCap VidRes<%d>\n",
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
        ConfLog( FALSE, "[AdjustKedaMcuAndTaideMuteInfo] ptMt NULL ! \n");
        return;
    }
    
    //�ϱ�MCS
    TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus(ptMt->GetMtId(), &tMtStatus);
    if( TRUE == bDecodeAud ) 
    {
        tMtStatus.SetDecoderMute( bOpenFlag != 0 ); //�����Ƿ���	        
    }
    else
    {
        tMtStatus.SetCaptureMute( bOpenFlag != 0 );	//�ɼ��Ƿ���(�Ƿ�����)
    }
    m_ptMtTable->SetMtStatus(ptMt->GetMtId(), &tMtStatus);

    MtStatusChange( ptMt->GetMtId(), TRUE );
    
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
		ConfLog( FALSE, "[PolycomSecChnnl] byPolycomMtId.%d ignore it\n", byPolycomMtId );
		return;
	}
	
	TCapSupport tPolyMcuCap;
	BOOL32 bRet = m_ptMtTable->GetMtCapSupport(byPolycomMtId, &tPolyMcuCap);
	if ( !bRet )
	{
		ConfLog( FALSE, "[PolycomSecChnnl] get PolyMt.%d's cap failed\n", byPolycomMtId );
		return;
	}
	TLogicalChannel tLogicChan;
	TMt tPolyMt = m_ptMtTable->GetMt(byPolycomMtId);
	McuMtOpenDoubleStreamChnnl(tPolyMt, tLogicChan, tPolyMcuCap);
	
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
		ConfLog(FALSE, "[SwitchNewVmpToSingleMt] illegal tMt!\n");
		return;
	}

	if (CONF_VMPMODE_NONE == m_tConf.m_tStatus.GetVMPMode())
	{
		ConfLog(FALSE, "[SwitchNewVmpToSingleMt] No VMP is used!\n");
		return;
	}
	
	u8 bySrcChnnl = 0;
	u8 byMtMediaType = 0;
	u8 byMtRes = 0;
	u8 byVmpRes = 0;
	m_cMtRcvGrp.GetMtMediaRes(tMt.GetMtId(), byMtMediaType, byMtRes);
	bySrcChnnl = GetVmpOutChnnlByRes(byMtRes, m_tVmpEqp.GetEqpId(), byMtMediaType);

	if( bySrcChnnl == (u8)~0 )
	{
		ConfLog(FALSE, "[SwitchNewVmpToSingleMt] Mt.%u cannot receive vmp broardcast \
		because unmatched format or resolution!\n");
	}
	else
	{
		g_cMpManager.SetSwitchBridge(m_tVmpEqp, bySrcChnnl, MODE_VIDEO);	//����
		StartSwitchToSubMt(m_tVmpEqp, bySrcChnnl, tMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode ��Ĭ��ֵ����SWITCH_MODE_SELECT
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
	CallLog("[ProcMtMcuOpenLogicChnnlRsp] Mt.%u cannot receive vmp broardcast \
		because unmatched format or resolution!\n");
	}
	else
	{
		g_cMpManager.SetSwitchBridge(m_tVidBrdSrc, bySrcChnnl, MODE_VIDEO);	//����
		StartSwitchToSubMt(m_tVidBrdSrc, bySrcChnnl, tMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode ��Ĭ��ֵ����SWITCH_MODE_SELECT
	}
*/

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
	memcpy(achConfName, cServMsg.GetMsgBody(), min(MAXLEN_CONFNAME, cServMsg.GetMsgBodyLen()));

	cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
	cServMsg.CatMsgBody((u8*)achConfName, strlen(achConfName));
	cServMsg.SetEventId(MCU_VCS_RELEASEMT_REQ);
	SendMsgToAllMcs(MCU_VCS_RELEASEMT_REQ, cServMsg);
	
}


//END OF FILE
