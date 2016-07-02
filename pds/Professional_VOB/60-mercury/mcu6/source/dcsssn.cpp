/*****************************************************************************
   ģ����      : mcu
   �ļ���      : dcsssn.cpp
   ����ļ�    : dcsssn.h
   �ļ�ʵ�ֹ���: MCU��T120����ҵ����
   ����        : �ű���
   �汾        : V4.0  Copyright(C) 2005-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/11/28  4.0		   �ű���      ����
******************************************************************************/

#include "evmcudcs.h"
#include "evmcu.h"
#include "evmcueqp.h"
#include "mcuvc.h"
#include "dcsssn.h"

/*lint -save -esym(530, tArgptr, argptr)*/
// (Warning -- argument no. 2 should be a pointer)
/*lint -save -esym(560, printf)*/
/*lint -save -e641)*/
// (Warning -- Warning -- Repeated include file
/*lint -save -e537)*/
// (Warning -- Symbol 'xxx' not accessed
/*lint -save -e551)*/
// (Warning -- Symbol 'xxx' not accessed
/*lint -save -e552)*/
// (Warning -- Header file repeatedly included but does not have a standard include guard
/*lint -save -e451)*/
// (Warning -- Last value assigned to variable xxx not used
/*lint -save -e438)*/
// (Warning -- Symbol 'xxx' (line n) not subsequently referenced)
/*lint -save -e529)*/
// (Warning -- Symbol 'xxx' (line n) not subsequently referenced)
/*lint -save -esym(1502, g_cMcuAgent)*/

CDcsSsnApp g_cDcsSsnApp;

//////////////////////////////////////////////////////////////////////////
//Construction / Destruction
//////////////////////////////////////////////////////////////////////////
CDcsSsnInst::CDcsSsnInst() : m_dwDcsNode(0),
							 m_dwDcsIId(0),
							 m_dwMcuIId(0),
							 m_dwDcsIp(0),
							 m_wDcsPort(0)
{	
}

CDcsSsnInst::~CDcsSsnInst()
{
    // destructor ���ܵ���
	// ClearInst();
}

/*=============================================================================
  �� �� ���� ClearInst
  ��    �ܣ� (1) ���ʵ��״̬
			 (2) ɾ��DCS��ַ�б��ж�Ӧ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/13    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ClearInst()
{
	NEXTSTATE( STATE_T120_IDLE );
	g_cDcsSsnApp.m_adwDcsIp[GetInsID()-1] = 0;
	m_dwDcsNode = 0;
	m_dwDcsIId  = 0;
	m_dwMcuIId  = 0;
	m_dwDcsIp   = 0;
	m_wDcsPort	= 0;
}

/*=============================================================================
  �� �� ���� DaemonInstanceEntry
  ��    �ܣ� ������DCS��Ϣ�ķַ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
             CApp * pcApp
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/13    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::DaemonInstanceEntry( CMessage *const pcMsg, CApp *pcApp )
{
	//��Ϣ���
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "Daemon: the pcMsg pointer should not be Null, ignore it !\n");
		return;
	}
	else
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "Daemon: message %u<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
	}
	
	switch( pcMsg->event )
	{
	case DCS_MCU_REG_REQ:
		DaemonProcDcsMcuRegReq( pcMsg, pcApp );
		break;
    case MCU_APPTASKTEST_REQ:
        DaemonProcAppTaskRequest(pcMsg);
        break;
	default:
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "Daemon: unexcept message %d<%s> received, ignore it !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
    
    return;
}

/*=============================================================================
  �� �� ���� InstanceEntry
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/13    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::InstanceEntry( CMessage *const pcMsg )
{
	//��Ϣ���
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "Inst: the pcMsg pointer should not be Null, ignore it !\n");
		return;
	}
	else
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "Inst: message %u<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
	
	switch( pcMsg->event )
	{
	case DCS_MCU_REG_REQ:
		ProcDcsMcuRegisterReq( pcMsg );
		break;

	case MCU_DCSSSN_CREATECONF_REQ:
	case MCU_DCSSSN_ADDMT_REQ:
	case MCU_DCSSSN_DELMT_REQ:
	case MCU_DCSSSN_RELEASECONF_REQ:
		ProcMcuDcsMsg( pcMsg );
		break;

	case DCS_MCU_CREATECONF_ACK:
	case DCS_MCU_CREATECONF_NACK:
	case DCS_MCU_CONFCREATED_NOTIF:
	case DCS_MCU_ADDMT_ACK:
	case DCS_MCU_ADDMT_NACK:
	case DCS_MCU_DELMT_ACK:
	case DCS_MCU_DELMT_NACK:
	case DCS_MCU_MTJOINED_NOTIF:
	case DCS_MCU_MTLEFT_NOTIF:		
	case DCS_MCU_RELEASECONF_ACK:
	case DCS_MCU_RELEASECONF_NACK:
	case DCS_MCU_CONFRELEASED_NOTIF:
		ProcDcsMcuMsg( pcMsg );
		break;
		
	case OSP_DISCONNECT:
		ProcDcsDisconnect( pcMsg );
		break;

    case MCU_MSSTATE_EXCHANGE_NTF:
        ProcMSStateExchageNotify( pcMsg );
        break;
        
    case DCS_MCU_GETMSSTATUS_REQ:
        ProcDcsMcuGetMsStatusReq( pcMsg );
        break;

	default:
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "Inst: unexpected msg %u<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
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
    03/05/29    1.0         
====================================================================*/
void CDcsSsnInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );
}

/*=============================================================================
  �� �� ���� DaemonProcDcsMcuRegReq
  ��    �ܣ� Ԥ����DCS��ע����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
             CApp *pcApp
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/13    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::DaemonProcDcsMcuRegReq( CMessage *const pcMsg, CApp *pcApp )
{
 	CDcsMcuRegReqPdu  cRegPdu;
	CMcuDcsRegNackPdu cRegNackPdu;
	CNetDataCoder	  cRegCoder;
	u8 achRegNackData[sizeof(CMcuDcsRegNackPdu)];

	//���ע��DCS����
	cRegCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
	cRegPdu.PduDecode( cRegCoder );
	
	//��ȡ��ǰDCS��Ϣ
	m_dwDcsIId  = pcMsg->srcid;
	m_dwDcsNode = pcMsg->srcnode;
	m_dwDcsIp	= cRegPdu.m_cDcsInfor.m_dwDcsIp;

	//δ����, NACK
	if ( FALSE == g_cMcuVcApp.IsPeriDcsConfiged( m_dwDcsIp ) )
	{
		u16 wErrorType = CErrorInfor::e_UnConfig;
		McuDcsRegNack( cRegNackPdu, cRegCoder, achRegNackData, wErrorType, pcMsg->event + 2 );			
		
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "Reg: dcs 0x%x have not been configed, ignore it !\n", m_dwDcsIp );
		return;
	}

	//�Ƿ��Ѿ�ע��
	for( s32 nIndex = 0; nIndex < MAXNUM_MCU_DCS; nIndex ++  )
	{
		//�Ѿ�ע��, NACK
		if( m_dwDcsIp == g_cDcsSsnApp.m_adwDcsIp[nIndex] )
		{
			u16 wErrorType = CErrorInfor::e_ConfAlreadyExist;
			McuDcsRegNack( cRegNackPdu, cRegCoder, achRegNackData, wErrorType, pcMsg->event + 2 );			

			LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "Reg: dcs 0x%x have already registered, ignore it !\n", m_dwDcsIp );
			return;
		}
	}
	
	//�Ƿ��пյ�ʵ��
	for( s32 nInst = 1; nInst < MAXNUM_MCU_DCS + 1; nInst ++ )
	{
		//����п�ʵ��
		if( pcApp->GetInstance(nInst)->CurState() == STATE_T120_IDLE )
		{
			//ת�����ÿ�ʵ������
			SendMsgToInst( nInst, pcMsg->event, pcMsg->content, pcMsg->length );
			
			//����ע�����Ϣ���ÿ�ʵ��
			CDcsSsnInst* cDcsSsnInst;
			cDcsSsnInst = (CDcsSsnInst*)pcApp->GetInstance(nInst);
			cDcsSsnInst->m_dwDcsNode = pcMsg->srcnode;
			cDcsSsnInst->m_dwDcsIId  = pcMsg->srcid;
			
			//���浱ǰDCS��ַ
			g_cDcsSsnApp.m_adwDcsIp[nInst-1] = m_dwDcsIp;
			return;
		}	
	}

	//û���ҵ�����ʵ��, NACK	
	u16 wErrorType = CErrorInfor::e_DcsFull;
	McuDcsRegNack( cRegNackPdu, cRegCoder, achRegNackData, wErrorType, pcMsg->event + 2 );
	
	LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "reg: there is no space for new DCS ! \n");
	return;
}

/*=============================================================================
  �� �� ���� ProcDcsMcuGetMsStatusReq
  ��    �ܣ� DCSȡ����״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/06/14    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuGetMsStatusReq( CMessage *const pcMsg )
{
    // guzh [4/11/2007] ���û��ͨ��ע��ͷ��ͻ�ȡ���󣬺��п�������������ǰ�����ӣ������
    if ( CurState() == STATE_T120_IDLE )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_DCS, "[ProcDcsMcuGetMsStatusReq]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        
        return;
    }

    CDcsMcuGetmsstatusReqPdu	cGetMSStatusPdu;
    CNetDataCoder				cMSStatusCoder;
    
    u8 byConfIdx = 0;	//����������
    
    //��Ϣ����
    switch( pcMsg->event )
    {
    case DCS_MCU_GETMSSTATUS_REQ:
        {            
            //������Ϣ
            cMSStatusCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
            cGetMSStatusPdu.PduDecode( cMSStatusCoder );          

            //ֱ��ȡ����״̬
            if( MCU_MSSTATE_ACTIVE == g_cMSSsnApp.GetCurMSState() )
            {
                //д�����ݵ�Ԫ
                CMcuDcsGetmsstatusAckPdu cGetMSStatusAckPdu;
                CNetDataCoder            cMSStatusAckCoder;
                u8 achMSStatusData[sizeof(CMcuDcsGetmsstatusAckPdu)];
                
                cGetMSStatusAckPdu.m_wConfId = byConfIdx;
                
                //д�뵱ǰ������״̬
                cGetMSStatusAckPdu.m_cMcuMsstate.m_bMcuMsstateInfo = g_cMSSsnApp.IsMsSwitchOK();
                
                //���벢����
                cMSStatusAckCoder.SetNetDataBuffer( achMSStatusData, sizeof(achMSStatusData) );
                cGetMSStatusAckPdu.PduEncode( cMSStatusAckCoder );
                SendMsgToDcs( MCU_DCS_GETMSSTATUS_ACK, achMSStatusData, (u16)cMSStatusAckCoder.GetCurrentLength() ); 
            }            
        }
        break;
    default:
        LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "unexpected msg %d<%s> received in get ms status req !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
        break;
    }

    return;
}

/*=============================================================================
  �� �� ���� ProcDcsMcuRegisterReq
  ��    �ܣ� У��ע����Ϣ,����Ӧ��Ӧ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/13    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuRegisterReq( CMessage *const pcMsg )
{
	CDcsMcuRegReqPdu	cRegPdu;
	CMcuDcsRegAckPdu	cRegAckPdu;
	CMcuDcsRegNackPdu	cRegNackPdu;
	CNetDataCoder		cRegCoder;

	u8 achRegAckData[sizeof(CMcuDcsRegAckPdu)];
	u8 achRegNackData[sizeof(CMcuDcsRegNackPdu)];

	cRegCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
	cRegPdu.PduDecode( cRegCoder );	

	//��ȡ��ǰDCS��IP��Port
	m_dwDcsIp  = cRegPdu.m_cDcsInfor.m_dwDcsIp;
	m_wDcsPort = cRegPdu.m_cDcsInfor.m_wDcsPort;
	
	//�ж���Ϣ����Դ
	if( MAKEIID(AID_MCU_DCSSSN, CInstance::DAEMON) != pcMsg->srcid )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "Inst: register req should be send by dcsssn Daemon, ignore it !\n");
		u16 wErrorType = CErrorInfor::e_NotDaemon;
		McuDcsRegNack( cRegNackPdu, cRegCoder, achRegNackData, wErrorType, pcMsg->event + 2 );
		return;
	}
	
	//����ACK
	McuDcsRegAck( cRegAckPdu, cRegCoder, achRegAckData, pcMsg->event + 1 );
	
	//����״̬
	NEXTSTATE( STATE_T120_NORMAL );
	
	//ע�����
	::OspNodeDiscCBRegQ( m_dwDcsNode, GetAppID(), GetInsID() );

	//DCS��Ϣ
	TDcsInfo tDcsInfo;
	tDcsInfo.m_byDcsId	= (u8)GetInsID();
	tDcsInfo.m_dwDcsIp	= cRegPdu.m_cDcsInfor.m_dwDcsIp;
	tDcsInfo.m_wDcsPort	= cRegPdu.m_cDcsInfor.m_wDcsPort;

	//����Ϣ��MCU֪ͨDCSע��ɹ�
	CServMsg cServMsg;
	cServMsg.SetEventId( MCU_DCSCONNCETED_NOTIF );
	cServMsg.SetMsgBody( (u8*)&tDcsInfo, sizeof(tDcsInfo) );
	g_cDcsSsnApp.SendMsgToMcuDaemon( cServMsg );

    return;
}

/*=============================================================================
  �� �� ���� ProcDcsDisconnect
  ��    �ܣ� ������Ϣ����:����ձ�ʵ��, ֪ͨMcuVcDaemon.
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/13    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcDcsDisconnect( CMessage *const pcMsg )
{
    LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "disconnect msg %u<%s> received in inst.%d !\n", pcMsg->event, OspEventDesc(pcMsg->length), GetInsID() );

	//״̬���
	switch( CurState() )
	{
	case STATE_T120_NORMAL:
		{
            if( *( u32 * )pcMsg->content == m_dwDcsNode )
            {               
                if (INVALID_NODE != m_dwDcsNode)
                {
                    OspDisconnectTcpNode(m_dwDcsNode);
                }            
                
                //ȡ��ǰʵ��ע���DCS����Ϣ
                TDcsInfo tDcsInfo;
                tDcsInfo.m_byDcsId	= (u8)GetInsID();
                tDcsInfo.m_dwDcsIp	= m_dwDcsIp;
                tDcsInfo.m_wDcsPort	= m_wDcsPort;
                
                //����Ϣ֪ͨMcuVcDaemon			
                CServMsg cServMsg;
                cServMsg.SetEventId( MCU_DCSDISCONNECTED_NOTIF );
                cServMsg.SetMsgBody( (u8*)&tDcsInfo, sizeof(tDcsInfo) );
                g_cDcsSsnApp.SendMsgToMcuDaemon( cServMsg );
                
                //���ʵ��״̬
                ClearInst();
            }			
		}
		break;
		
	default:
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected state<%d> received osp disconnected msg !\n", CurState() );
		break;
	}

    return;
}

/*=============================================================================
  �� �� ���� ProcMSStateExchageNotify
  ��    �ܣ� ����MCU��DCS��������֪ͨ��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/11/27    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcMSStateExchageNotify( CMessage *const pcMsg )
{
    //״̬���
    switch( CurState() )
    {
    case STATE_T120_NORMAL:
        {
            // ���ͬ��ʧ��,�Ͽ���Ӧ������
            u8 byIsSwitchOk = *pcMsg->content;
            if (0 == byIsSwitchOk)
            {
                OspDisconnectTcpNode( m_dwDcsNode );
                return;
            }            
        }
        break;
    default:
        LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected state<%d> received StateExchageNotify !\n", CurState() );
        break;
    }
    return;
}

/*=============================================================================
  �� �� ���� ProcMcuDcsMsg
  ��    �ܣ� ����MCU��DCS����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/13    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcMcuDcsMsg( CMessage *const pcMsg )
{
	//��Ϣ���
	if ( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "mcu to dcs  null msgreceive failed !\n");
		return;
	}
	//״̬���
	if ( STATE_T120_NORMAL != CurState() )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "mcu to dcs msg %d<%s> received in wrong state !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		return;
	}
	//��Ϣ����
	switch( pcMsg->event )
	{
	case MCU_DCSSSN_CREATECONF_REQ:
		ProcMcuDcsCreateConfReq( pcMsg );
		break;
	case MCU_DCSSSN_RELEASECONF_REQ:
		ProcMcuDcsReleaseConfReq( pcMsg );
		break;
	case MCU_DCSSSN_DELMT_REQ:
		ProcMcuDcsDelMtReq( pcMsg );
		break;
	case MCU_DCSSSN_ADDMT_REQ:
		ProcMcuDcsAddMtReq( pcMsg );
		break;
	default:
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected msg %d<%s>received in mcu to dcs msg !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
}

/*=============================================================================
  �� �� ���� ProcDcsMcuMsg
  ��    �ܣ� ����DCS��MCU����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/13    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuMsg( CMessage *const pcMsg )
{
	//��Ϣ���
	if ( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs to mcu null msg receive failed !\n");
		return;
	}
	//״̬���
	if ( STATE_T120_NORMAL != CurState() )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs to mcu msg msg %d<%s> received in wrong state !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		return;
	}
	//��Ϣ����
	switch( pcMsg->event )
	{
	case DCS_MCU_CREATECONF_ACK:
	case DCS_MCU_CREATECONF_NACK:
	case DCS_MCU_CONFCREATED_NOTIF:
		ProcDcsMcuCreateConfRsp( pcMsg );
		break;
	case DCS_MCU_RELEASECONF_ACK:
	case DCS_MCU_RELEASECONF_NACK:
	case DCS_MCU_CONFRELEASED_NOTIF:
		ProcDcsMcuReleaseConfRsp( pcMsg );
		break;
	case DCS_MCU_ADDMT_ACK:
	case DCS_MCU_ADDMT_NACK:
		ProcDcsMcuAddMtRsp( pcMsg );
		break;
	case DCS_MCU_DELMT_ACK:
	case DCS_MCU_DELMT_NACK:
		ProcDcsMcuDelMtRsp( pcMsg );
		break;
	case DCS_MCU_MTJOINED_NOTIF:
		ProcDcsMcuMtOnlineNotify( pcMsg );
		break;
	case DCS_MCU_MTLEFT_NOTIF:
		ProcDcsMcuMtOfflineNotify( pcMsg );
		break;
	default:
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected msg %d<%s> received in inst.%d \n", pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
		break;
	}

    return;
}

/*=============================================================================
  �� �� ���� ProcMcuDcsCreateConfReq
  ��    �ܣ� ����MCU����DCSSSN�Ĵ�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/27    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcMcuDcsCreateConfReq( CMessage *const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	//��Ϣ��У��
	if ( !MSGBODY_LEN_GE( cServMsg, sizeof(TDcsConfInfo) ) ) 
		return;
	
	//ȡ��Ϣ��
	TDcsConfInfo tDcsConfInfo = *(TDcsConfInfo*)( cServMsg.GetMsgBody() );
	
	//����д��DCS���ݵ�Ԫ
	CMcuDcsCreateConfReqPdu cCreateConfPdu;
	CNetDataCoder			cCreateConfCoder;
	u8	achCreateConfData[sizeof(CMcuDcsCreateConfReqPdu)];
	
	//����ID
	if ( 0 == cServMsg.GetConfIdx() )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "confidx should not be 0 in mcu to dcs create conf !\n ");
		return;
	}
	cCreateConfPdu.m_wConfId = cServMsg.GetConfIdx();

	//��������
	if ( NULL == tDcsConfInfo.GetConfName() ) 
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "conf name should not be NULL in mcu to dcs create conf !\n");
		return;
	}
	LPCSTR lpszConfName = NULL;
	lpszConfName = tDcsConfInfo.GetConfName();
	strncpy( cCreateConfPdu.m_cConfBasicInfor.m_achConfName, lpszConfName, sizeof(cCreateConfPdu.m_cConfBasicInfor.m_achConfName) );

	//��������

	//ȡ����
	LPCSTR lpszDataConfPwd = tDcsConfInfo.GetConfPwd();

	//���ܿ�
	if ( NULL == lpszDataConfPwd ) 
	{
		cCreateConfPdu.m_cConfBasicInfor.m_bHasPassword = FALSE;
	}
	//������
	else
	{
		cCreateConfPdu.m_cConfBasicInfor.m_bHasPassword = TRUE;
		strncpy( cCreateConfPdu.m_cConfBasicInfor.m_achConfPassword, lpszDataConfPwd, sizeof(u8) * MAXLEN_PWD );
	}
	
	//��������
	u16 wBandwidth = tDcsConfInfo.GetBandwidth();
	if ( 0 != wBandwidth )
	{
		cCreateConfPdu.m_cConfShortInfor.IncludeOption( CConfShortInfor::e_BandWidth );
		cCreateConfPdu.m_cConfShortInfor.m_dwBandWidth = wBandwidth;
	}
	else
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "data conf bandwidth should not be 0 ! \n" );
		return;
	}

	//�Ƿ�����ն���������,  Ĭ��ΪFALSE
	if ( TRUE == tDcsConfInfo.IsSupportJoinMt() )
	{
		cCreateConfPdu.m_cConfShortInfor.IncludeOption( CConfShortInfor::e_SupportJoin );
		cCreateConfPdu.m_cConfShortInfor.m_bSupportJoinedMt = TRUE;
	}

	//���벢���͸����ݵ�Ԫ
	cCreateConfCoder.SetNetDataBuffer( achCreateConfData, sizeof(achCreateConfData) );
	cCreateConfPdu.PduEncode( cCreateConfCoder );
	SendMsgToDcs( MCU_DCS_CREATECONF_REQ, achCreateConfData, (u16)cCreateConfCoder.GetCurrentLength() );

    return;
}

/*=============================================================================
  �� �� ���� ProcDcsMcuCreateConfRsp
  ��    �ܣ� ����DCS����MCU�Ĵ����Ӧ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/28    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuCreateConfRsp( CMessage *const pcMsg )
{
	CDcsMcuCreateConfAckPdu		cConfAckPdu;
	CDcsMcuCreateConfNackPdu	cConfNackPdu;
	CDcsMcuCreateConfNotifyPdu  cConfNotifyPdu;
	CNetDataCoder				cConfRspCoder;

	u8 byConfIdx = 0;	//����������
	
	//��Ϣ����
	switch( pcMsg->event )
	{
	case DCS_MCU_CREATECONF_ACK:
		{
			//������Ϣ
			cConfRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cConfAckPdu.PduDecode( cConfRspCoder );
			
			//У��confidx			
			byConfIdx = (u8)cConfAckPdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong confidx received in create conf ack !\n");
				return;
			}
			g_cDcsSsnApp.SendMsgToMcuConf( byConfIdx, DCSSSN_MCU_CREATECONF_ACK );
		}
		break;

	case DCS_MCU_CREATECONF_NACK:
		{
			//������Ϣ
			cConfRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cConfNackPdu.PduDecode( cConfRspCoder );

			//У��confidx
			byConfIdx = (u8)cConfNackPdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong confidx received in create conf nack !\n");
				return;
			}
			//��������
			u8 byErrorId = cConfNackPdu.m_cErrorInfor.m_emErrorType;
			if ( CErrorInfor::e_Succuss == byErrorId )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong error id received in create conf nack !\n" );
				return;
			}
			g_cDcsSsnApp.SendMsgToMcuConf( byConfIdx, DCSSSN_MCU_CREATECONF_NACK, &byErrorId, sizeof(u8) );
		}
		break;

	case DCS_MCU_CONFCREATED_NOTIF:
		{
			//������Ϣ
			cConfRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cConfNotifyPdu.PduDecode( cConfRspCoder );

			//У��confidx
			byConfIdx = (u8)cConfNotifyPdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong confidx received in create conf notify !\n");
				return;
			}
			
			//������ݻ�����Ϣ
			TDcsConfInfo tDcsConfInfo;
			memset( &tDcsConfInfo, 0, sizeof(tDcsConfInfo) );

			//������
			s8 *pszConfName = cConfNotifyPdu.m_cConfBasicInfor.m_achConfName;
			if ( NULL == pszConfName )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "conf name shoule not be null in create conf notify !\n");
				return;
			}
			tDcsConfInfo.SetConfName( pszConfName );

			//��������
			if ( TRUE == cConfNotifyPdu.m_cConfBasicInfor.m_bHasPassword ) 
			{
				LPCSTR lpszConfPwd = cConfNotifyPdu.m_cConfBasicInfor.m_achConfPassword;
				if ( NULL != lpszConfPwd )
				{
					tDcsConfInfo.SetConfPwd( lpszConfPwd );
				}
				else
				{
					LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "get conf encrypt key in create conf notify failed !\n");
					return;
				}
			}	
			
			//��������
			if ( 0 != cConfNotifyPdu.m_cConfShortInfor.m_dwBandWidth )
			{
				u16 wBandwidth = (u16)cConfNotifyPdu.m_cConfShortInfor.m_dwBandWidth;
				tDcsConfInfo.SetBandwidth( wBandwidth );
			}
			else
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "get bandwidth in create conf notify failed !\n");
				return;
			}
			
			//�Ƿ�����ն˺���
			BOOL32 bSupport = cConfNotifyPdu.m_cConfShortInfor.m_bSupportJoinedMt;
			tDcsConfInfo.SetSupportJoinMt( bSupport );
			
			//����Ϣ��McuVc
			CServMsg cServMsg;
			cServMsg.SetConfIdx( byConfIdx );
			cServMsg.SetEventId( DCSSSN_MCU_CONFCREATED_NOTIF );
			cServMsg.SetMsgBody( (u8*)&tDcsConfInfo, sizeof(tDcsConfInfo) );
			
			g_cDcsSsnApp.SendMsgToMcuConf( cServMsg );

		}
		break;

	default:
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "unexpected msg %d<%s> received in create conf rsp !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
}

/*=============================================================================
  �� �� ���� ProcMcuDcsReleaseConfReq
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/28    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcMcuDcsReleaseConfReq( CMessage *const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	CMcuDcsReleaseConfReqPdu cReleaseConfReqPdu;
	CNetDataCoder			 cReleaseConfCoder;
	u8	achReleaseConfData[sizeof(CMcuDcsReleaseConfReqPdu)];
	
	//д�����ݵ�Ԫ
	cReleaseConfReqPdu.m_wConfId = cServMsg.GetConfIdx();
	
	//���벢����
	cReleaseConfCoder.SetNetDataBuffer( achReleaseConfData, sizeof(achReleaseConfData) );
	cReleaseConfReqPdu.PduEncode( cReleaseConfCoder );
	SendMsgToDcs( MCU_DCS_RELEASECONF_REQ, achReleaseConfData, (u16)cReleaseConfCoder.GetCurrentLength() );

    return;
}

/*=============================================================================
  �� �� ���� ProcDcsMcuReleaseConfRsp
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/28    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuReleaseConfRsp( CMessage *const pcMsg )
{
	CDcsMcuReleaseConfAckPdu	cReleaseAckPdu;
	CDcsMcuReleaseConfNackPdu	cReleaseNackPdu;
	CDcsMcuReleaseConfNackPdu	cReleaseNotifyPdu;
	CNetDataCoder				cReleaseRspCoder;

	u8 byConfIdx = 0;	//����������
	
	//��Ϣ����
	switch( pcMsg->event )
	{
	case DCS_MCU_RELEASECONF_ACK:
		{
			cReleaseRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cReleaseAckPdu.PduDecode( cReleaseRspCoder );

			byConfIdx = (u8)cReleaseAckPdu.m_wConfId;
			
			//У��confidx
			if ( 0 == byConfIdx )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong confidx received in release conf ack !\n");
				return;
			}
			CServMsg cServMsg;
			cServMsg.SetEventId( DCSSSN_MCU_RELEASECONF_ACK );
			cServMsg.SetMsgBody( &byConfIdx, sizeof(u8) );
			g_cDcsSsnApp.SendMsgToMcuDaemon( cServMsg );
		}
		break;

	case DCS_MCU_RELEASECONF_NACK:
		{
			cReleaseRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cReleaseNackPdu.PduDecode( cReleaseRspCoder );

			byConfIdx = (u8)cReleaseNackPdu.m_wConfId;

			//У��confidx
			if ( 0 == byConfIdx )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong confidx received in release conf nack !\n");
				return;
			}		
			//��������
			u8 byErrorId = cReleaseNackPdu.m_cErrorInfor.m_emErrorType;
			if ( CErrorInfor::e_Succuss == byErrorId )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong error id received in release conf nack !\n");
				return;
			}
			CServMsg cServMsg;
			cServMsg.SetEventId( DCSSSN_MCU_RELEASECONF_NACK );
			cServMsg.SetMsgBody( &byConfIdx, sizeof(u8) );
			cServMsg.CatMsgBody( &byErrorId, sizeof(u8) );
			g_cDcsSsnApp.SendMsgToMcuDaemon( cServMsg );
		}
		break;

	case DCS_MCU_CONFRELEASED_NOTIF:
		{
			cReleaseRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cReleaseNotifyPdu.PduDecode( cReleaseRspCoder );

			byConfIdx = (u8)cReleaseNotifyPdu.m_wConfId;
			
			//У��confidx
			if ( 0 == byConfIdx )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong confidx received in release conf notify !\n");
				return;
			}
			CServMsg cServMsg;
			cServMsg.SetEventId( DCSSSN_MCU_CONFRELEASED_NOTIF );
			cServMsg.SetMsgBody( &byConfIdx, sizeof(u8) );
			g_cDcsSsnApp.SendMsgToMcuDaemon( cServMsg );
		}
		break;
		
	default:
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "unexpected msg %d<%s> received in release conf rsp !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
}

/*=============================================================================
  �� �� ���� ProcMcuDcsAddMtReq
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/28    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcMcuDcsAddMtReq( CMessage *const pcMsg )
{
	//ȡ��Ϣ��
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TDcsMtInfo tDcsMtInfo = *(TDcsMtInfo*)cServMsg.GetMsgBody();
	
	//��Ϣ��У��
	if ( !MSGBODY_LEN_GE( cServMsg, sizeof(TDcsMtInfo) ) ) 
		return;

	//д�����ݵ�Ԫ
	CMcuDcsAddMtReqPdu cAddMtPdu;
	CNetDataCoder      cAddMtCoder;
	u8 achAddMtData[sizeof(CMcuDcsAddMtReqPdu)];

	//��������
	u8 byConfIdx = cServMsg.GetConfIdx();
	if ( 0 == byConfIdx )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "get wrong confidx in add mt req !\n");
		return;
	}
	cAddMtPdu.m_wConfId = byConfIdx;

	//�ն�ID
	u8 byMtId = tDcsMtInfo.m_byMtId;
	if ( 0 == byMtId )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "get wrong mt id in add mt req !\n");
		return;
	}
	cAddMtPdu.m_wMtId = byMtId;

	//�ж����ӵ��ն˵�����
	//�Զ�����, �ȴ��¼�����
	if ( TDcsMtInfo::emBelowJoin == tDcsMtInfo.m_emJoinedType )
	{
		//�ն�IP��Ч
		if ( 0 == tDcsMtInfo.m_dwMtIp )
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "get wrong mt ip in add mt req !\n");
			return;
		}
		cAddMtPdu.m_cMtShortInfor.m_emPositionType = CMtShortInfor::e_Below_Joined;
		//�ն˵�ַ
		cAddMtPdu.m_cMtShortInfor.IncludeOption( CMtShortInfor::e_NodeAddress );		
		cAddMtPdu.m_cMtShortInfor.m_dwMtIp	= tDcsMtInfo.m_dwMtIp;
		cAddMtPdu.m_cMtShortInfor.m_wMtPort = 0; 
		//�ȴ���ʱ
		cAddMtPdu.m_cMtShortInfor.IncludeOption( CMtShortInfor::e_WaitJoinTimeout );
		cAddMtPdu.m_cMtShortInfor.m_wTimeOut = 5; //�ȴ��¼�����ĳ�ʱֵ(s)
	}
	//��������, �����¼�
	else if ( TDcsMtInfo::emInviteBelow == tDcsMtInfo.m_emJoinedType )
	{
		//�ն�IP�Ͷ˿���Ч
		if ( 0 == tDcsMtInfo.m_dwMtIp || 0 == tDcsMtInfo.m_wPort )
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "get wrong mt ip or port in add mt req !\n");
			return;
		}
		cAddMtPdu.m_cMtShortInfor.m_emPositionType = CMtShortInfor::e_Below_Invite;
		//�ն˵�ַ
		cAddMtPdu.m_cMtShortInfor.IncludeOption( CMtShortInfor::e_NodeAddress );
		cAddMtPdu.m_cMtShortInfor.m_dwMtIp  = tDcsMtInfo.m_dwMtIp;
		cAddMtPdu.m_cMtShortInfor.m_wMtPort = tDcsMtInfo.m_wPort;
	}
	//�Զ�����, �ϼ�MCU���뱾�˼���
	else if ( TDcsMtInfo::emAboveInvite == tDcsMtInfo.m_emJoinedType )
	{
		//�ն�IP��Ч
		if ( 0 == tDcsMtInfo.m_dwMtIp )
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "get wrong mt ip in add mt req !\n");
			return;
		}
		cAddMtPdu.m_cMtShortInfor.m_emPositionType = CMtShortInfor::e_Above_Invited;
		//�ն˵�ַ
		cAddMtPdu.m_cMtShortInfor.IncludeOption( CMtShortInfor::e_NodeAddress );
		cAddMtPdu.m_cMtShortInfor.m_dwMtIp  = tDcsMtInfo.m_dwMtIp;
		//�ȴ���ʱ
		cAddMtPdu.m_cMtShortInfor.IncludeOption( CMtShortInfor::e_WaitJoinTimeout );
		cAddMtPdu.m_cMtShortInfor.m_wTimeOut = 5; //�ȴ��ϼ�����ĳ�ʱֵ(s)
	}
	else
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "unexpected mt Join Type received in add mt req, ignore it !\n");
		return;
	}
	
	//���벢����
	cAddMtCoder.SetNetDataBuffer( achAddMtData, sizeof(achAddMtData) );
	cAddMtPdu.PduEncode( cAddMtCoder );
	SendMsgToDcs( MCU_DCS_ADDMT_REQ, achAddMtData, (u16)cAddMtCoder.GetCurrentLength() );	

    return;
}

/*=============================================================================
  �� �� ���� ProcDcsMcuAddMtRsp
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/28    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuAddMtRsp( CMessage *const pcMsg )
{
	CDcsMcuAddMtAckPdu		cAddMtAckPdu;
	CDcsMcuAddMtNackPdu		cAddMtNackPdu;
	CNetDataCoder			cAddMtRspCoder;
	
	u8 byConfIdx = 0;	//����������
	
	//��Ϣ����
	switch( pcMsg->event )
	{
	case DCS_MCU_ADDMT_ACK:
		{
			//������Ϣ
			cAddMtRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cAddMtAckPdu.PduDecode( cAddMtRspCoder );

			//У��confidx
			byConfIdx = (u8)cAddMtAckPdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong confidx: %d get in add mt ack !\n", byConfIdx );
				return;
			}
			//�ն�ID
			if ( 0 == cAddMtAckPdu.m_wMtId )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong mt id: %d get in add mt ack !\n", cAddMtAckPdu.m_wMtId );
				return;
			}
			u8 byMtId = (u8)cAddMtAckPdu.m_wMtId; 
			//����
			g_cDcsSsnApp.SendMsgToMcuConf( byConfIdx, DCSSSN_MCU_ADDMT_ACK, &byMtId, sizeof(u8) );
		}
		break;

	case DCS_MCU_ADDMT_NACK:
		{
			//������Ϣ
			cAddMtRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cAddMtNackPdu.PduDecode( cAddMtRspCoder );
			
			//У��confidx
			byConfIdx = (u8)cAddMtNackPdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong confidx: %d get in add mt Nack !\n", byConfIdx );
				return;
			}
			//�ն�ID
			if ( 0 == cAddMtNackPdu.m_wMtId )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong mt id: %d get in add mt nack !\n", cAddMtNackPdu.m_wMtId );
				return;
			}
			u8 byMtId = (u8)cAddMtNackPdu.m_wMtId;

			//��������
			u8 byErrorId = cAddMtNackPdu.m_cErrorInfor.m_emErrorType;
			if ( CErrorInfor::e_Succuss == byErrorId )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong error id received in add mt Nack !\n");
				return;
			}
			//����
			CServMsg cServMsg;
			cServMsg.SetConfIdx( byConfIdx );
			cServMsg.SetEventId( DCSSSN_MCU_ADDMT_NACK );
			cServMsg.SetMsgBody( &byMtId, sizeof(u8) );
			cServMsg.CatMsgBody( &byErrorId, sizeof(u8) );
			g_cDcsSsnApp.SendMsgToMcuConf( cServMsg );
		}
		break;

	default:
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "unexpected msg %d<%s> received in add mt rsp !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
}

/*=============================================================================
  �� �� ���� ProcMcuDcsDelMtReq
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/28    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcMcuDcsDelMtReq( CMessage *const pcMsg )
{
	//ȡ��Ϣ��
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byMtId = *cServMsg.GetMsgBody();
	
	//��Ϣ��У��
	if ( !MSGBODY_LEN_GE( cServMsg, sizeof(u8) ) ) 
		return;

	//д�����ݵ�Ԫ
	CMcuDcsDelMtReqPdu cDelMtPdu;
	CNetDataCoder      cDelMtCoder;
	u8 achDelMtData[sizeof(CMcuDcsDelMtReqPdu)];
	
	//��������
	u8 byConfIdx = cServMsg.GetConfIdx();
	if ( 0 == byConfIdx )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "get wrong confidx in del mt req !\n");
		return;
	}
	cDelMtPdu.m_wConfId = byConfIdx;
	
	//�ն�ID
	if ( 0 == byMtId )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "get wrong mt id in del mt req !\n");
		return;
	}
	cDelMtPdu.m_wMtId = byMtId;

	//���벢����
	cDelMtCoder.SetNetDataBuffer( achDelMtData, sizeof(achDelMtData) );
	cDelMtPdu.PduEncode( cDelMtCoder );
	SendMsgToDcs( MCU_DCS_DELMT_REQ, achDelMtData, (u16)cDelMtCoder.GetCurrentLength() );
	
    return;
}

/*=============================================================================
  �� �� ���� ProcDcsMcuDelMtRsp
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/28    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuDelMtRsp( CMessage *const pcMsg )
{
	CDcsMcuDelMtAckPdu		cDelMtAckPdu;
	CDcsMcuDelMtNackPdu		cDelMtNackPdu;
	CNetDataCoder			cDelMtRspCoder;
	
	u8 byConfIdx = 0;	//����������
	
	//��Ϣ����
	switch( pcMsg->event )
	{
	case DCS_MCU_DELMT_ACK:
		{
			//������Ϣ
			cDelMtRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cDelMtAckPdu.PduDecode( cDelMtRspCoder );
			
			//У��confidx
			byConfIdx = (u8)cDelMtAckPdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong confidx: %d get in del mt ack !\n", byConfIdx );
				return;
			}
			//�ն�ID
			if ( 0 == cDelMtAckPdu.m_wMtId )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong mt id: %d get in del mt ack !\n", cDelMtAckPdu.m_wMtId );
				return;
			}
			u8 byMtId = (u8)cDelMtAckPdu.m_wMtId;
			
			//����
			g_cDcsSsnApp.SendMsgToMcuConf( byConfIdx, DCSSSN_MCU_DELMT_ACK, &byMtId, sizeof(u8) );
			
		}
		break;
		
	case DCS_MCU_DELMT_NACK:
		{
			//������Ϣ
			cDelMtRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cDelMtNackPdu.PduDecode( cDelMtRspCoder );
			
			//У��confidx
			byConfIdx = (u8)cDelMtNackPdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong confidx get in del mt Nack !\n");
				return;
			}
			//�ն�ID
			if ( 0 == cDelMtNackPdu.m_wMtId )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong mt id: %d get in del mt Nack !\n", cDelMtNackPdu.m_wMtId );
				return;
			}
			u8 byMtId = (u8)cDelMtNackPdu.m_wMtId;

			//��������
			u8 byErrorId = cDelMtNackPdu.m_cErrorInfor.m_emErrorType;
			if ( CErrorInfor::e_Succuss == byErrorId )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong error id received in del mt Nack !\n");
				return;
			}
			//����
			CServMsg cServMsg;
			cServMsg.SetConfIdx( byConfIdx );
			cServMsg.SetEventId( DCSSSN_MCU_DELMT_NACK );
			cServMsg.SetMsgBody( &byMtId, sizeof(u8) );
			cServMsg.CatMsgBody( &byErrorId, sizeof(u8) );
			g_cDcsSsnApp.SendMsgToMcuConf( cServMsg );
		}
		break;
		
	default:
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "unexpected msg %d<%s> received in del mt rsp !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
}

/*=============================================================================
  �� �� ���� ProcDcsMcuMtOnlineNotify
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/28    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuMtOnlineNotify( CMessage *const pcMsg )
{
	CDcsMcuMtMtOnlineNotifyPdu	cMtOnlinePdu;
	CNetDataCoder				cMtOnlineCoder;
	
	u8 byConfIdx = 0;	//����������
	
	//��Ϣ����
	switch( pcMsg->event )
	{
	case DCS_MCU_MTJOINED_NOTIF:
		{
			//������Ϣ
			cMtOnlineCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cMtOnlinePdu.PduDecode( cMtOnlineCoder );
			
			//У��confidx
			byConfIdx = (u8)cMtOnlinePdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong confidx get in mt online notify !\n");
				return;
			}
			//��������ն���Ϣ
			TDcsMtInfo tDcsMtInfo;
			memset( &tDcsMtInfo, 0, sizeof(tDcsMtInfo) );

			//�ն�ID
			if ( 0 == cMtOnlinePdu.m_wMtId )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong mt id get in online notify !\n");
				return;
			}
			tDcsMtInfo.m_byMtId = (u8)cMtOnlinePdu.m_wMtId;

			//�����ն˵�����
			if ( CMtShortInfor::e_Mcu == cMtOnlinePdu.m_cMtShortInfor.m_emDeviceType )
			{
				tDcsMtInfo.m_byDeviceType = TDcsMtInfo::emT120Mcu;
			}
			else if ( CMtShortInfor::e_Mt == cMtOnlinePdu.m_cMtShortInfor.m_emDeviceType ) 
			{
				tDcsMtInfo.m_byDeviceType = TDcsMtInfo::emT120Mt;
			}
			else
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "mt %d wrong device type get in online notify !\n", cMtOnlinePdu.m_wMtId );
				return;
			}	
			//����McuVc
			CServMsg cServMsg;
			cServMsg.SetConfIdx( byConfIdx );
			cServMsg.SetEventId( DCSSSN_MCU_MTJOINED_NOTIF );
			cServMsg.SetMsgBody( (u8*)&tDcsMtInfo, sizeof(tDcsMtInfo) );
			g_cDcsSsnApp.SendMsgToMcuConf( cServMsg );
		}
		break;
		
	default:
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "unexpected msg %d<%s> received in mt online notify !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
}

/*=============================================================================
  �� �� ���� ProcDcsMcuMtOfflineNotify
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage *const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/28    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuMtOfflineNotify( CMessage *const pcMsg )
{
	CDcsMcuMtMtOfflineNotifyPdu	cMtOfflinePdu;
	CNetDataCoder				cMtOfflineCoder;
	
	u8 byConfIdx = 0;	//����������
	
	//��Ϣ����
	switch( pcMsg->event )
	{
	case DCS_MCU_MTLEFT_NOTIF:
		{
			//������Ϣ
			cMtOfflineCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cMtOfflinePdu.PduDecode( cMtOfflineCoder );
			
			//У��confidx
			byConfIdx = (u8)cMtOfflinePdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong confidx %d get in mt offline notify !\n", byConfIdx );
				return;
			}
			//�ն�ID
			if ( 0 == cMtOfflinePdu.m_wMtId )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "wrong mt id %d get in online notify !\n", cMtOfflinePdu.m_wMtId );
				return;
			}
			u8 byMtId = (u8)cMtOfflinePdu.m_wMtId;			
	
			//����McuVc
			g_cDcsSsnApp.SendMsgToMcuConf( byConfIdx, DCSSSN_MCU_MTLEFT_NOTIF, &byMtId, sizeof(u8) );
		}
		break;
		
	default:
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "unexpected msg %d<%s> received in mt offline notify !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
}

/*=============================================================================
  �� �� ���� McuDcsRegNack
  ��    �ܣ� ע��ܾ�Ӧ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/26    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::McuDcsRegNack( CMcuDcsRegNackPdu	cRegNackPdu, 
								 CNetDataCoder		cRegCoder, 
								 u8		achRegNackData[sizeof(CMcuDcsRegNackPdu)], 
								 u16	wErrorType, 
								 u16	wEvent	)
{
	cRegNackPdu.m_cErrorInfor.m_emErrorType = (CErrorInfor::EErrorType)wErrorType;
	cRegCoder.SetNetDataBuffer( achRegNackData, sizeof(u8) * sizeof(CMcuDcsRegNackPdu) );
	cRegNackPdu.PduEncode( cRegCoder );
	SendMsgToDcs( wEvent, achRegNackData, (u16)cRegCoder.GetCurrentLength() );
    return;
}

/*=============================================================================
  �� �� ���� McuDcsRegNack
  ��    �ܣ� ע�����Ӧ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/26    4.0			�ű���                  ����
=============================================================================*/
void CDcsSsnInst::McuDcsRegAck( CMcuDcsRegAckPdu cRegAckPdu, 
								CNetDataCoder	 cRegCoder, 
								u8	 achRegAckData[sizeof(CMcuDcsRegAckPdu)],
								u16	 wEvent )
{
	cRegCoder.SetNetDataBuffer( achRegAckData, sizeof(u8) * sizeof(CMcuDcsRegAckPdu));
	cRegAckPdu.PduEncode( cRegCoder );
	SendMsgToDcs( wEvent, achRegAckData, (u16)cRegCoder.GetCurrentLength() );
    return;
}


CDcsConfig::CDcsConfig()
{
	for(s32 nLoop = 0; nLoop < MAXNUM_MCU_DCS; ++nLoop)
	{
		m_adwDcsIp[nLoop] = 0;
	}
}

CDcsConfig::~CDcsConfig()
{
	
}

/*=============================================================================
  �� �� ���� SendMsgToDcs
  ��    �ܣ� ����Ϣ��DCS
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u16 wEvent
             u8 *const pbyMsg
             u16 wLen
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/14    4.0			�ű���                  ����
=============================================================================*/
BOOL32 CDcsSsnInst::SendMsgToDcs( u16 wEvent, u8 *const pbyMsg, u16 wLen )
{
	if( OspIsValidTcpNode( m_dwDcsNode ) )
	{
		u32 dwRet = OspPost( m_dwDcsIId, wEvent, pbyMsg, wLen, m_dwDcsNode, MAKEIID( GetAppID(), GetInsID()) );
		if( OSP_OK == dwRet )
		{
			return TRUE;
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "Send msg %d<%s> to Dcs failed !\n", wEvent, OspEventDesc(wEvent) );
		}
	}
	LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "Send: dcs node %d invalid, check it !\n", m_dwDcsNode);
	return FALSE;
}

/*=============================================================================
  �� �� ���� SendMsgToInst
  ��    �ܣ� DCS�Ự�ַ���Ϣ����ͨʵ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  s32 nInst
             u16 wEvent
             u8 *const pbyMsg
             u16 wLen
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/13    4.0			�ű���                  ����
=============================================================================*/
BOOL32 CDcsSsnInst::SendMsgToInst( s32 nInst, u16 wEvent, u8 *const pbyMsg, u16 wLen )
{
	u32 dwRet = OspPost( MAKEIID(AID_MCU_DCSSSN, nInst), wEvent, pbyMsg, wLen, 0, MAKEIID(AID_MCU_DCSSSN, CInstance::DAEMON) );
	if ( OSP_OK == dwRet )
	{
		return TRUE;
	}
	LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "send msg: %d<%s> to dcsssn inst failed !\n", wEvent, OspEventDesc(wEvent) );
	return FALSE;
}


/*=============================================================================
  �� �� ���� SendMsgToMcuConf
  ��    �ܣ� ����Ϣ��MCU����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/13    4.0			�ű���                  ����
=============================================================================*/
BOOL32 CDcsConfig::SendMsgToMcuConf( u8 byConfIdx, u16 wEvent, u8 *const pbyMsg, u16 wLen )
{
	if( 0 != byConfIdx )
	{	
		CServMsg cServMsg;
		cServMsg.SetConfIdx( byConfIdx );
		cServMsg.SetEventId( wEvent );
		cServMsg.SetMsgBody( pbyMsg, wLen );

		OspPost( MAKEIID( AID_MCU_VC, g_cMcuVcApp.GetConfMapInsId(byConfIdx) ), 
					wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		return TRUE ;
	}
	LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "send msg: %d<%s> to mcu conf failed !\n", wEvent, OspEventDesc(wEvent) );
	return FALSE;
}

/*=============================================================================
  �� �� ���� SendMsgToMcuConf
  ��    �ܣ� ����Ϣ��MCU����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CServMsg &cServMsg
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/28    4.0			�ű���                  ����
=============================================================================*/
BOOL32 CDcsConfig::SendMsgToMcuConf( CServMsg &cServMsg )
{
	u32 dwRet = OspPost( MAKEIID( AID_MCU_VC, g_cMcuVcApp.GetConfMapInsId(cServMsg.GetConfIdx())), 
					     cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	if ( 0 == dwRet )
	{
		return TRUE;
	}
	LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "send msg: %d<%s> to mcu conf failed !\n", cServMsg.GetEventId(), OspEventDesc(cServMsg.GetEventId()) );
	return FALSE;
}

/*=============================================================================
  �� �� ���� SendMsgToMcuDaemon
  ��    �ܣ� ����Ϣ��MCU daemonʵ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u16 wEvent
             u8 *const pbyMsg
             u16 wLen
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/13    4.0			�ű���                  ����
=============================================================================*/
BOOL32 CDcsConfig::SendMsgToMcuDaemon( CServMsg &cServMsg )
{
	u32 dwRet = OspPost( MAKEIID(AID_MCU_VC, CInstance::DAEMON), cServMsg.GetEventId(), 
						 cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	if ( OSP_OK == dwRet )
	{
		return TRUE;
	}
	LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "Send msg: %d<%s> to mcuvc daemon failed !\n", cServMsg.GetEventId(), OspEventDesc(cServMsg.GetEventId()) );
	return FALSE;
}

/*=============================================================================
  �� �� ���� BroadcastToAllDcsSsn
  ��    �ܣ� ����Ϣ�����е�DCS�Ựʵ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wEvent
             u8 *const pbyMsg
             u16 wLen
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/11/27    4.0			�ű���                  ����
=============================================================================*/
void CDcsConfig::BroadcastToAllDcsSsn( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
    //�����������������ⲿ��AppͶ����Ϣ��������
    if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
    {
        return;
    }
    
    ::OspPost(MAKEIID( AID_MCU_DCSSSN, CInstance::EACH ), wEvent, pbyMsg, wLen);
    
    return;
}

/*=============================================================================
  �� �� ���� MSGBODY_LEN_GE
  ��    �ܣ� У����Ϣ��ĳ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CServMsg &cServMsg
             u16 wLen
  �� �� ֵ�� inline BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/27    4.0			�ű���                  ����
=============================================================================*/
BOOL32 MSGBODY_LEN_GE( CServMsg &cServMsg, u32 wLen )
{
	if(cServMsg.GetMsgBodyLen() < wLen)
	{
		const s8* pszMsgStr = OspEventDesc( cServMsg.GetEventId() );
		
		LogPrint(LOG_LVL_DETAIL, MID_MCU_DCS, "Error body len %u for msg %u(%s) detected:  "
				"Should >= %u. Ignore it.\n", cServMsg.GetMsgBodyLen(), 
				cServMsg.GetEventId(),	pszMsgStr ? pszMsgStr : "null",	wLen);									
		return FALSE;
	}	
	return TRUE;
}

/*====================================================================
    ������      ��InstanceDump
    ����        �����ش�ӡ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 param, ��ӡ״̬����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    06/05/19    4.0         �ű���        ����
====================================================================*/
void CDcsSsnInst::InstanceDump( u32 param )
{
	s8 achType[32];
	memset( achType, 0, sizeof(achType) );
    strncpy( achType, "DCS", sizeof(achType) );
	StaticLog("%3u %5u %5u %6s   %s %d\n", GetInsID(), GetInsID(), m_dwDcsNode, achType, g_cMcuVcApp.GetDcsAlias((u8)GetInsID()),param );
    return;
}


// END OF FILE

