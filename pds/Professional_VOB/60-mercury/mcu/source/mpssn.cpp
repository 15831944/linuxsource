/*****************************************************************************
   ģ����      : MP�Ựģ��
   �ļ���      : mpssn.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: MP�Ựģ��ʵ����ʵ��
   ����        : ������
   �汾        : V0.1  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���         �޸�����
   2003/07/10  0.1         ������          ����
******************************************************************************/

#include "evmcu.h"
#include "evmp.h"
#include "mcuvc.h"
#include "mpssn.h"
#include "mcuver.h"
//#include "mcuerrcode.h"

CMpSsnApp g_cMpSsnApp;

//����
CMpSsnInst::CMpSsnInst()
{
    m_byMpId = 0;
}

//����
CMpSsnInst::~CMpSsnInst()
{

}

/*====================================================================
    ������      : SendMsgToMp
    ����        ������Ϣ��MP
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����wEvent ��ϢID
				  pbyMsg ��Ϣ��
                  wLen   ��Ϣ����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMpSsnInst::SendMsgToMp( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	post(m_dwMpIId,wEvent,pbyMsg, wLen, m_dwMpNode);

	return;
}

/*====================================================================
    ������      : SendMsgToMcu
    ����        ������Ϣ��Mcu
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����wEvent ��ϢID
				  pbyMsg ��Ϣ��
                  wLen   ��Ϣ����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMpSsnInst::SendMsgToMcu( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	g_cMcuVcApp.SendMsgToDaemonConf( wEvent, pbyMsg, wLen );
	
	return;
}

/*====================================================================
    ������      : InstanceEntry
    ����        ����ͨʵ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMpSsnInst::InstanceEntry( CMessage * const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	switch( pcMsg->event ) 
	{
	  //ת��MCU����MP����Ϣ
	  case MCU_MP_ADDSWITCH_REQ:
	  case MCU_MP_REMOVESWITCH_REQ:
	  case MCU_MP_GETSWITCHSTATUS_REQ:
	  case MCU_MP_RELEASECONF_NOTIFY:
	  case MCU_MP_ADDMULTITOONESWITCH_REQ:		//MCU����MP�����Ӷ�㵽һ�㽻������
	  case MCU_MP_REMOVEMULTITOONESWITCH_REQ:	//MCU����MP���Ƴ���㵽һ�㽻������
	  case MCU_MP_STOPMULTITOONESWITCH_REQ:		//MCU����MP��ֹͣ��㵽һ�㽻������
	  case MCU_MP_ADDRECVONLYSWITCH_REQ:		//MCU����MP������ֻ���ս�������
	  case MCU_MP_REMOVERECVONLYSWITCH_REQ:	    //MCU����MP���Ƴ�ֻ���ս�������
	  case MCU_MP_SETRECVSWITCHSSRC_REQ:	    //MCU����MP�ı�����ս���ssrc����
      case MCU_MP_CONFUNIFORMMODE_NOTIFY:       //��һ����
      case MCU_MP_ADDBRDSRCSWITCH_REQ:          // MCU����MP�Ĳ𽨹㲥���������� 
      case MCU_MP_ADDBRDDSTSWITCH_REQ:
      case MCU_MP_REMOVEBRDSRCSWITCH_REQ:
	  case MCU_MP_GETDSINFO_REQ:
      case MCU_MP_PINHOLE_NOTIFY:
      case MCU_MP_STOPPINHOLE_NOTIFY:
	  case MCU_MP_ADDSENDSELFMUTEPACK_CMD:
	  case MCU_MP_DELSENDSELFMUTEPACK_CMD:
          SendMsgToMp(pcMsg->event,pcMsg->content,pcMsg->length);
		  break;

	  //����MPע����Ϣd
	  case MP_MCU_REG_REQ:
		  ProcMpRegisterReq(pcMsg);
		  break;
	  case MP_MCU_GETMSSTATUS_REQ:
      case MCU_MSSTATE_EXCHANGE_NTF:
		  ProcMpGetMsStatusReq(pcMsg);
		  break;
	  //����Mp����
	  case OSP_DISCONNECT:
		  ProcMpDisconnect(pcMsg);
		  break;

	  //ת��MP����MCU����Ϣ
	  case MP_MCU_ADDSWITCH_ACK:	  
	  case MP_MCU_REMOVESWITCH_ACK:
	  
	  case MP_MCU_GETSWITCHSTATUS_ACK:
	  case MP_MCU_ADDMULTITOONESWITCH_ACK:		    //MP����MCU�����Ӷ�㵽һ�㽻������	  
	  case MP_MCU_REMOVEMULTITOONESWITCH_ACK:		//MP����MCU���Ƴ���㵽һ�㽻������
	  
	  case MP_MCU_STOPMULTITOONESWITCH_ACK:		    //MP����MCU��ֹͣ��㵽һ�㽻������
	  case MP_MCU_ADDRECVONLYSWITCH_ACK:		        //MP����MCU������ֻ���ս�������	  
	  case MP_MCU_REMOVERECVONLYSWITCH_ACK:	        //MP����MCU���Ƴ�ֻ���ս�������	  
	  case MP_MCU_SETRECVSWITCHSSRC_ACK:	        //MP����MCU�ı�����ս���ssrc����
	  
      case MP_MCU_ADDBRDSRCSWITCH_ACK:                  //MP���ղ𽨹㲥����������
      case MP_MCU_ADDBRDDSTSWITCH_ACK:
      case MP_MCU_REMOVEBRDSRCSWITCH_ACK:
      
      case MP_MCU_ADDSWITCH_NACK:
      case MP_MCU_ADDMULTITOONESWITCH_NACK:		    //MP�ܾ�MCU�����Ӷ�㵽һ�㽻������

      case MP_MCU_ADDBRDSRCSWITCH_NACK:
      case MP_MCU_ADDBRDDSTSWITCH_NACK:

    case MP_MCU_BRDSRCSWITCHCHANGE_NTF:
        
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[MpSsn]Msg %d(%s) passed for conf.%d\n",
            pcMsg->event, OspEventDesc(pcMsg->event), cServMsg.GetConfIdx());
        
		  g_cMcuVcApp.SendMsgToConf( cServMsg.GetConfIdx(), pcMsg->event, 
									 pcMsg->content, pcMsg->length );
          break;

	  //case MP_MCU_FLUXOVERRUN_NOTIFY:				//zgc [2007/01/11] MP֪ͨMCU����ת����������
      case MP_MCU_FLUXSTATUS_NOTIFY:
	  case MP_MCU_GETDSINFO_ACK:
	  case MP_MCU_GETDSINFO_NACK:
		  SendMsgToMcu(pcMsg->event, pcMsg->content, pcMsg->length);
		  break;

          // ������ʱ������
      case MP_MCU_GETSWITCHSTATUS_NACK:
      case MP_MCU_REMOVESWITCH_NACK:
      case MP_MCU_REMOVEMULTITOONESWITCH_NACK:	//MP�ܾ�MCU���Ƴ���㵽һ�㽻������
      case MP_MCU_STOPMULTITOONESWITCH_NACK:		//MP�ܾ�MCU��ֹͣ��㵽һ�㽻������
      case MP_MCU_ADDRECVONLYSWITCH_NACK:		    //MP�ܾ�MCU������ֻ���ս�������
      case MP_MCU_REMOVERECVONLYSWITCH_NACK:	    //MP�ܾ�MCU���Ƴ�ֻ���ս�������
      case MP_MCU_SETRECVSWITCHSSRC_NACK:			//MP�ܾ�MCU�ı�����ս���ssrc����

      case MP_MCU_REMOVEBRDSRCSWITCH_NACK:
		  break;
	  default:
		  break;
	} 

	return;
}

/*=============================================================================
    �� �� ���� DaemonInstanceEntry
    ��    �ܣ� ʵ����Ϣ����Daemon��ں���������override
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
               CApp* pcApp
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/12  4.0			����                  ����
=============================================================================*/
void CMpSsnInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp )
{
	if (NULL == pcMsg)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "MpSsn[DaemonInstanceEntry]: The received msg's pointer is NULL!");
		return;
	}

	switch (pcMsg->event)
	{
	case MCU_APPTASKTEST_REQ:			//GUARD Probe Message
		DaemonProcAppTaskRequest( pcMsg );
		break;
	default:
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "MpSsn[DaemonInstanceEntry]: Wrong message %u(%s) received! AppId=%u\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ),pcApp!=NULL?pcApp->appId:0 );
		break;
	}
	
	return;
}

/*=============================================================================
    �� �� ���� DaemonProcAppTaskRequest
    ��    �ܣ� GUARDģ��̽����Ϣ������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const CMessage * pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/12  4.0			����                  ����
=============================================================================*/
void CMpSsnInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );

	return;
}

/*====================================================================
    ������      : ProcMpRegisterReq
    ����        ������Mp��ע����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMpSsnInst::ProcMpRegisterReq( CMessage * const pcMsg)
{
    CServMsg cServMsg( pcMsg->content ,pcMsg->length );
	TMp tMp = *(TMp*)cServMsg.GetMsgBody();
	// ���Ӱ汾У��,zgc,2007-09-28
	// �Ƶ�TMp�ṹ��, zgc, 2007-10-10
	//	u16 wVersion = *(u16*)(cServMsg.GetMsgBody() + sizeof(TMp));
	//	wVersion = ntohs(wVersion);
	
    u16 wRecvStartPort = g_cMcuAgent.GetRecvStartPort();
	u8 byIsRestrictFluxOfMcuMp = g_cMcuVcApp.GetPerfLimit().IsLimitMp() ? 1 : 0;
	u32 dwSysSSrc = htonl(g_cMSSsnApp.GetMSSsrc());
	
	switch( CurState() )
	{
	case STATE_IDLE:
		//zbq[12/11/2007]״̬���ϸ�ת
		//case STATE_NORMAL:
		{    
			//����Mp��Node��IId
			m_dwMpNode = pcMsg->srcnode;
			m_dwMpIId = pcMsg->srcid;
			m_byMpId = tMp.GetMpId();
			m_byMpAttachMode = tMp.GetAttachMode();
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)		 
			SetMpIp( tMp.GetIpAddr());
#endif
			// ��Ϣ�峤��У��, zgc, 2007-10-10
			if (cServMsg.GetMsgBodyLen() != sizeof(TMp))
			{
				LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "MpSsn: Mp(0x%x) register invalid msg len:%d(should be %d).\n", 
					tMp.GetIpAddr() ,cServMsg.GetMsgBodyLen(), sizeof(TMp));
				SendMsgToMp(pcMsg->event+2,pcMsg->content,pcMsg->length);
				::OspDisconnectTcpNode( pcMsg->srcnode );
				return;
			}
			
			//��Mp��Id�����ж�
			if( m_byMpId != (u8)GetInsID() )
			{
				//NACK
				LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR,"Mp%d(0x%x) register to instance %d, Nack!\n", m_byMpId, tMp.GetIpAddr() , GetInsID());
				SendMsgToMp(pcMsg->event+2,pcMsg->content,pcMsg->length);
				if (m_byMpAttachMode != 1)
				{
					::OspDisconnectTcpNode( pcMsg->srcnode );
				}			
				return;
			}
			
			//��֤,�Ƿ��Ǳ�MCU�ĵ���
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)// [2/25/2010 xliang] 8000E ϵ�в���������У�飬��Ϊû�е���������
#ifndef WIN32
			if( tMp.GetIpAddr() != ntohl( g_cMcuAgent.GetBrdIpAddr( m_byMpId ) ) )
			{
				//NACK
				LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR,"other mcu board mp[Ip= %0x, Id= %d] register, nack!\n", tMp.GetIpAddr(), m_byMpId);
				SendMsgToMp(pcMsg->event+2,pcMsg->content,pcMsg->length);
				if (m_byMpAttachMode != 1)
				{
					::OspDisconnectTcpNode( pcMsg->srcnode );
				}		   
				return;
			}
#endif
#endif
			// �汾��֤ʧ�ܣ��ܾ�ע��, zgc, 2007-09-28
			if ( tMp.GetVersion() != DEVVER_MP )
			{
				cServMsg.SetErrorCode(ERR_MCU_VER_UNMATCH);
				post(pcMsg->srcid, pcMsg->event + 2,
					cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
				
				LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "MpSsn: Mp.%d(0x%x) version %d differs from MCU %d, NACK!\n",                       
					GetInsID(),
					tMp.GetIpAddr(),
					tMp.GetVersion(),
					DEVVER_MP );
				
				
				// �����ã�����
				if (m_byMpAttachMode != 1)
				{
					::OspDisconnectTcpNode(pcMsg->srcnode);
				}
				return;
			}
			
			LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[MpSsn]: Mp %d register to mcu success!\n", tMp.GetMpId() );
			
			//֪ͨMcu
			SendMsgToMcu(pcMsg->event,pcMsg->content,pcMsg->length);
			
			//ע���������ʵ��
			::OspNodeDiscCBRegQ( pcMsg->srcnode, GetAppID(), GetInsID() );
			
			// guzh [6/12/2007] ���ӻỰֵ֪ͨ
			cServMsg.SetMsgBody( (u8*)&dwSysSSrc, sizeof(dwSysSSrc) );
			cServMsg.CatMsgBody( (u8*)&wRecvStartPort, sizeof(u16) );
			
			//zgc,2007/01/11,����Ƿ�����MPת��������־
			cServMsg.CatMsgBody( (u8*)&byIsRestrictFluxOfMcuMp, sizeof(u8) );
			
			// guzh [1/17/2007] ����MP����ȷ�����������Ƶ�����
			u16 wMaxCap = 0;
#ifdef _MINIMCU_
			// ����8000B��Ҫ���� MPC/MDSC/HDSC
			if ( m_byMpAttachMode == 1 )
			{
				wMaxCap = g_cMcuVcApp.GetPerfLimit().m_wMpcMaxMpAbility;
			}
			else
			{
				// Ϊ�˷�ֹ�û����ô���ֱ�Ӳ����ڲ�Ĭ��ֵ
				// u8 byType = BrdExtModuleIdentify();
				u8 byType = g_cMcuAgent.GetRunBrdTypeByIdx( m_byMpId );
				if ( byType == 0xFF )
				{
					wMaxCap = 0;
				}
				else if ( byType == BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/ )
				{
					wMaxCap = MAXLIMIT_CRI_MP_HDSC;
				}
				else
				{
					wMaxCap = MAXLIMIT_CRI_MP_MDSC;
				}
			}
#else

			u8 byBrdId = g_cMcuAgent.GetBrdIdbyIpAddr(tMp.GetIpAddr());
			TBoardInfo tBrdInfo;
			g_cMcuAgent.GetBrdCfgById(byBrdId,&tBrdInfo);

			if ( m_byMpAttachMode == 1 )
			{
				wMaxCap = g_cMcuVcApp.GetPerfLimit().m_wMpcMaxMpAbility;
			}
			else
			{
				//zhouyiliang 20110113 is2.2������
				wMaxCap = g_cMcuVcApp.GetPerfLimit().m_wCriMaxMpAbility;
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)//8000Gת���嶼������,��Ĭ��ֵ
				u8 byType = g_cMcuAgent.GetRunBrdTypeByIdx( m_byMpId );
				if( BRD_TYPE_IS22 == byType )
				{
					wMaxCap = MAXLIMIT_CRI_MP_EX;
				}
				else if (BRD_TYPE_CRI2 == byType) 
				{
					//ǰ����
					if( tBrdInfo.GetPortChoice() == 0)
					{
						wMaxCap = MAXLIMIT_CRI_MP_EX;
					}
				}		
#endif
			}
#endif
			wMaxCap = htons(wMaxCap);
			cServMsg.CatMsgBody( (u8*)&wMaxCap, sizeof(u16) );

            //��ʱ����
            u32 dwPinHoleInterval = g_cMcuVcApp.GetPinHoleInterval();
            dwPinHoleInterval = htonl(dwPinHoleInterval);
            cServMsg.CatMsgBody( (u8*)&dwPinHoleInterval, sizeof(u32) );

			u8 bySendMutePackNum = g_cMcuVcApp.GetSendMutePackNum();
            cServMsg.CatMsgBody( (u8*)&bySendMutePackNum, sizeof(u8) );

			u8 bySendMutePackInterval = g_cMcuVcApp.GetSendMutePackInterval();
            cServMsg.CatMsgBody( (u8*)&bySendMutePackInterval, sizeof(u8) );
			
			//ACK
			SendMsgToMp( pcMsg->event+1,cServMsg.GetServMsg(),cServMsg.GetServMsgLen() );
			
			NEXTSTATE( STATE_NORMAL );
			break;
     }
	 default:
         LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "EqpSsn: Wrong message %u(%s) received in current state %u!\n",
			 pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());         
		 break;	   
	}
}

/*====================================================================
    ������      : ProcMpDisconnect
    ����        ��Mp��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMpSsnInst::ProcMpDisconnect(CMessage * const pcMsg)
{
	LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[MpSsn]: MP disconnect messege %u(%s) received!,Dri id is:%d.\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byMpId );

	if( *( u32 * )pcMsg->content == m_dwMpNode )	//��ʵ����Ӧ���Ӷ�
	{
        if (INVALID_NODE != m_dwMpNode)
        {
            OspDisconnectTcpNode(m_dwMpNode);
        }
        
		m_byMpId = 0;
		m_dwMpNode = INVALID_NODE;
		SetMpIp(0);	// [12/17/2009 xliang] 

		TMp tMp;
        tMp.SetMpId((u8)GetInsID());

		//[2011/09/02/zhangli]mpIp�·������飬������ݴ���Ϣ���RTCP����
		//��data��MP��Ϣ��remove��������ȡ����ip��Ϣ��������Ҫ����ȥ
		u32 dwMpIp = g_cMcuVcApp.GetMpIpAddr(tMp.GetMpId());
		tMp.SetIpAddr(dwMpIp);

		CServMsg cServMsg;
		cServMsg.SetMsgBody((u8 *)&tMp,sizeof(tMp));

		SendMsgToMcu(MCU_MP_DISCONNECTED_NOTIFY,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());

		NEXTSTATE( STATE_IDLE );
	}

	return;
}

/*=============================================================================
  �� �� ���� ProcMpGetMsStatusReq
  ��    �ܣ� ȡ��������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMpSsnInst::ProcMpGetMsStatusReq(CMessage* const pcMsg)
{
    // guzh [4/11/2007] ���û��ͨ��ע��ͷ��ͻ�ȡ���󣬺��п�������������ǰ�����ӣ������
    if ( CurState() == STATE_IDLE )
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[ProcMpGetMsStatusReq]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        
        return;
    }
    
    if (MCU_MSSTATE_EXCHANGE_NTF == pcMsg->event)
    {
        // ���ͬ��ʧ��,�Ͽ���Ӧ������
        u8 byIsSwitchOk = *pcMsg->content;
        if (0 == byIsSwitchOk)
        {
            OspDisconnectTcpNode( m_dwMpNode );
            return;
        }        
    }

	if( MCU_MSSTATE_ACTIVE == g_cMSSsnApp.GetCurMSState() ) // ��������ʱ�������Լ���ʱ
    {
        TMcuMsStatus tMsStatus;
        tMsStatus.SetMsSwitchOK(g_cMSSsnApp.IsMsSwitchOK());
        
        CServMsg cServMsg;
        cServMsg.SetMsgBody((u8*)&tMsStatus, sizeof(tMsStatus));
        SendMsgToMp(MCU_MP_GETMSSTATUS_ACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

        //LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMpGetMsStatusReq] IsMsSwitchOK :%d.\n", tMsStatus.IsMsSwitchOK());
    }
    return;
}
//CMpConfig
/*--------------------------------------------------------------------------------*/

//����
CMpConfig::CMpConfig()
{

}

//����
CMpConfig::~CMpConfig()
{

}

/*====================================================================
    ������      : SendMsgToMpSsn
    ����        ������Ϣ��Mp�Ự
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byMpId Mp���
	              wEvent ��ϢID
				  pbyMsg ��Ϣ��
                  wLen   ��Ϣ����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMpConfig::SendMsgToMpSsn( u8 byMpId, u16 wEvent,  u8 * const pbyMsg, u16 wLen)
{
	//�����������������ⲿ��AppͶ����Ϣ��������
	if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
	{
		return;
	}
    
    ::OspPost( MAKEIID( AID_MCU_MPSSN, byMpId), wEvent, pbyMsg, wLen );

	return;
}


/*====================================================================
    ������      : BroadcastToAllMpSsn
    ����        ������Ϣ�����������ӵ�Mp�Ự
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����wEvent ��ϢID
				  pbyMsg ��Ϣ��
                  wLen   ��Ϣ����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMpConfig::BroadcastToAllMpSsn(u16 wEvent, u8 * const pbyMsg, u16 wLen)
{
	//�����������������ⲿ��AppͶ����Ϣ��������
	if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
	{
		return;
	}

    ::OspPost( MAKEIID( AID_MCU_MPSSN, CInstance::EACH ), wEvent, pbyMsg, wLen );

	return;
}


//END OF FILE
