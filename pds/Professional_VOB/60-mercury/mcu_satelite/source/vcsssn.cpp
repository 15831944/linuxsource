/*****************************************************************************
   ģ����      : mcu
   �ļ���      : vcsssn.cpp
   ����ļ�    : vcsssn.h
   �ļ�ʵ�ֹ���: MCU��VCS�Ự��
   ����        : ���㻪
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2008/11/20              ���㻪      ����
******************************************************************************/



#include "evmcu.h"
#include "evmcumcs.h"
#include "evmcuvcs.h"
#include "evmcutest.h"
#include "mcuvc.h"
#include "mcuerrcode.h"
#include "umconst.h"
#include "nplusmanager.h"
#include "mcucfg.h"
#include "vcsssn.h"

#if defined(_VXWORKS_)
#include "brddrvlib.h"
#elif defined(_LINUX_)
#include "boardwrapper.h"
#endif

CVcsSsnApp	g_cVcsSsnApp;	                   // VCS�ỰӦ��ʵ��

CUsrManage  g_cVCSUsrManage((u8*)DIR_DATA, (u8*)MCU_VCSUSER_FILENAME);   //VCS���û��������


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVcsSsnInst::CVcsSsnInst( void )
{

}

CVcsSsnInst::~CVcsSsnInst( void )
{

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
    2008/11/20              ���㻪        ����
====================================================================*/
void CVcsSsnInst::InstanceEntry( CMessage * const pcMsg )
{
	// ��������Ϊ��Ч״̬��ʵ���������κ���Ϣ����
	if (CurState() == STATE_INVALID)
	{
		return;
	}

	if (NULL == pcMsg)
	{
		OspPrintf(TRUE, FALSE, "[CVcsSsnInst%u]: The received msg's pointer in the msg entry is NULL!", GetInsID());
		return;
	}

	if (MCU_MCS_MTSTATUS_NOTIF != pcMsg->event)
	{
		VcsLog("[CVcsSsnInst%u]: Event %u(%s) received!\n", GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));
	}

	switch (pcMsg->event)
	{
	// ��ʼ��VCS��ͨʵ����״̬
	case OSP_POWERON:
		{
			// ����VCS����32��ʵ����ֻ�к�16��ʵ����Ч��֧��16��VCSͬʱ����
			if (GetInsID() <= MAXNUM_MCU_MC)
			{
				NEXTSTATE(STATE_INVALID) ;
			}
		}
    	break;

	case OSP_DISCONNECT:				       //����̨����֪ͨ		
		QuitCurVCConf();
		CMcsSsnInst::InstanceEntry( pcMsg );   //������ɶ�Ӧ�������� 
		break;                       

    // VCS�û��������,����ԭ�е�MCS��Ϣ����ʹ�ò�ͬ���û���Ϣ
    case MCS_MCU_ADDUSER_REQ:           //VCS����MCU����û�
    case MCS_MCU_DELUSER_REQ:           //VCS����MCUɾ���û�
    case MCS_MCU_GETUSERLIST_REQ:       //VCS����MCU�õ��û��б�
    case MCS_MCU_CHANGEUSER_REQ:        //VCS����MCU�޸��û���Ϣ

    case MCS_MCU_ADDUSERGRP_REQ:        //VCS����MCU����û���
    case MCS_MCU_CHANGEUSERGRP_REQ:     //VCS����MCU�޸��û���
    case MCS_MCU_DELUSERGRP_REQ:        //VCS����MCUɾ���û���
    case MCS_MCU_GETUSERGRP_REQ:        //VCS����MCU��ȡ�û�����Ϣ

        ProcVCSUserManagerMsg( pcMsg );
        break;

	// VCS��������Զ���
	case VCS_MCU_GETSOFTNAME_REQ:
	case VCS_MCU_CHGSOFTNAME_REQ:
		ProcVCSSoftNameMsg( pcMsg );
		break;
		
	case VCS_MCU_SETUNPROCFGINFO_REQ:
	case VCS_MCU_GETUNPROCFGINFO_REQ:
		ProcVCSMcuUnProcCfgMsg( pcMsg );
		break;

	// ģ����ز���
	case MCS_MCU_MODIFYTEMPLATE_REQ:        //VCS�޸�ģ��
	case MCS_MCU_DELTEMPLATE_REQ:           //VCSɾ��ģ��
		ProcVCSConfTemMsg( pcMsg );
		break;              

	case VCS_MCU_OPRCONF_REQ:           //VCS�������ĳ����ϯ
	case VCS_MCU_QUITCONF_REQ:          //VCS�����˳�ĳ����ϯ
		ProcVCSConfOprMsg( pcMsg );
		break;

	case VCS_MCU_CREATECONF_BYTEMPLATE_REQ: //VCSͨ������ģ�崴������
		ProcVCSMcuCreateConfMsg( pcMsg );
		break;

	
	case VCS_MCU_VCMT_REQ:                  //VCS��������	
	case VCS_MCU_VCMODE_REQ:                //���鵱ǰ�ĵ���ģʽ����    
	case VCS_MCU_CHGVCMODE_REQ:             //�л�����ģʽ
    case VCS_MCU_MUTE_REQ:                  //VCS����������
	case VCS_MCU_GROUPCALLMT_REQ:           //VCS�������
	case VCS_MCU_GROUPDROPMT_REQ:           //VCSֹͣ�������
	case VCS_MCU_STARTCHAIRMANPOLL_REQ:     //VCS��������ϯ��ѯ  
	case VCS_MCU_STOPCHAIRMANPOLL_REQ:      //VCS����ֹͣ��ϯ��ѯ
	case VCS_MCU_ADDMT_REQ:                 //VCS��ʱ�����ն�
	case VCS_MCU_DELMT_REQ:                 //VCSɾ����ʱ�����ն�

	case VCS_MCU_RELEASEMT_ACK:             //VCSͬ���ͷ��ն�
	case VCS_MCU_RELEASEMT_NACK:            //VCS�ܾ��ͷ��ն�
		ProcVCSMcuVCMsg( pcMsg );
		break;

	//Ӧ����Ϣ
	case MCU_VCS_OPRCONF_ACK:
	case MCU_VCS_OPRCONF_NACK:
	case MCU_VCS_QUITCONF_ACK:
	case MCU_VCS_QUITCONF_NACK:		

	case MCU_VCS_CREATECONF_BYTEMPLATE_ACK:
	case MCU_VCS_CREATECONF_BYTEMPLATE_NACK:
		
	case MCU_VCS_VCMT_ACK:
	case MCU_VCS_VCMT_NACK:
	case MCU_VCS_VCMT_NOTIF:

	case MCU_VCS_VCMODE_ACK:
	case MCU_VCS_VCMODE_NACK:

    case MCU_VCS_CHGVCMODE_ACK:
	case MCU_VCS_CHGVCMODE_NACK:
	case MCU_VCS_CHGVCMODE_NOTIF:

    case MCU_VCS_MUTE_ACK:
	case MCU_VCS_MUTE_NACK:

    case MCU_VCS_GROUPCALLMT_ACK:
	case MCU_VCS_GROUPCALLMT_NACK:
	case MCU_VCS_GROUPCALLMT_NOTIF:

	case MCU_VCS_GROUPDROPMT_ACK:
	case MCU_VCS_GROUPDROPMT_NACK:

    case MCU_VCS_STARTCHAIRMANPOLL_ACK:
	case MCU_VCS_STARTCHAIRMANPOLL_NACK:
	case MCU_VCS_STOPCHAIRMANPOLL_ACK:
	case MCU_VCS_STOPCHAIRMANPOLL_NACK:

	case MCU_VCS_ADDMT_ACK:
	case MCU_VCS_ADDMT_NACK:
	case MCU_VCS_DELMT_ACK:
	case MCU_VCS_DELMT_NACK:
		
    case MCU_VCS_CONFSTATUS_NOTIF:

	case MCU_VCS_GETSOFTNAME_ACK:
	case MCU_VCS_GETSOFTNAME_NACK:
    case MCU_VCS_CHGSOFTNAME_ACK:
	case MCU_VCS_CHGSOFTNAME_NACK:
	case VCS_MCU_SOFTNAME_NOTIF:

    case MCU_VCS_SETUNPROCFGINFO_NOTIF:
		
    case MCU_VCS_RELEASEMT_REQ:
	case MCU_VCS_RELEASEMT_NOTIF:
		ProcMcuVcsMsg( pcMsg );
		break;


	default:
		CMcsSsnInst::InstanceEntry( pcMsg );
		

	}
}
/*====================================================================
    ������      ��ProcVCSSoftNameMsg
    ����        ������VCS��������޸���Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2008/11/20              ���㻪        ����
====================================================================*/
void CVcsSsnInst::ProcVCSSoftNameMsg( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	switch( CurState() )
	{
	case STATE_NORMAL:
		switch(pcMsg->event)
		{
		case VCS_MCU_GETSOFTNAME_REQ:
		case VCS_MCU_CHGSOFTNAME_REQ:
			::OspPost(MAKEIID( AID_MCU_CONFIG, 1 ), cServMsg.GetEventId(), cServMsg.GetServMsg(), 
					  cServMsg.GetServMsgLen());
			break;
		default:
		OspPrintf(TRUE, FALSE, "[ProcVCSSoftNameMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
}

}
/*====================================================================
    ������      ��ProcUserManagerMsg
    ����        �������û�������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2008/11/20              ���㻪        ����
====================================================================*/
void CVcsSsnInst::ProcVCSUserManagerMsg( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	CVCSUsrInfo *ptVCSUserInfo  = NULL;
    TUsrGrpInfo tUsrGrpInfo;
    
    // ��ǰVCS���û�������
    u8 byMcsGrpId = CMcsSsn::GetUserGroup( GetInsID() );

    // ��������������
    u8 byUsrGrpId;
    
    // guzh [12/14/2006] ����Ǳ����л���Ĺ�����ʽ,û���޸�Ȩ��
//     if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
//     {
//         switch (pcMsg->event)
//         {
//         case MCS_MCU_ADDUSER_REQ:
//         case MCS_MCU_DELUSER_REQ:
//         case MCS_MCU_CHANGEUSER_REQ:
//         case MCS_MCU_ADDUSERGRP_REQ:
//         case MCS_MCU_CHANGEUSERGRP_REQ:
//         case MCS_MCU_DELUSERGRP_REQ:
// 			cServMsg.SetErrorCode(ERR_MCU_NPLUS_EDITUSER);
// 			SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
// 			return;
//         default:
//             break;
//         }
//     }

	switch (pcMsg->event)
	{
	case MCS_MCU_ADDUSER_REQ:       //VCS����MCU����û�
        {
			// �����û�����Ӳ�ȡԭMCS��Ϣ������Ϣ�岻ͬ���������Ϣ���ݽ��м��飬������
			if (cServMsg.GetMsgBodyLen() < sizeof(CVCSUsrInfo))
			{
				OspPrintf(TRUE, FALSE,"[ProcVCSUserManagerMsg] error:MCS_MCU_ADDUSER_REQ with wrong message body!\n");
				return;
			}
			
		    ptVCSUserInfo  = (CVCSUsrInfo*)cServMsg.GetMsgBody();

		    cServMsg.SetMsgBody((u8*)ptVCSUserInfo->GetName(), MAX_CHARLENGTH);
            byUsrGrpId = ptVCSUserInfo->GetUsrGrpId();

            if ( byMcsGrpId != USRGRPID_SADMIN && 
                 byMcsGrpId != byUsrGrpId )
            {
                // ���ܲ�����������
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
			    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			    return;
            }

            BOOL32 bRet = FALSE;
            u8 byMaxNum = 0;
            u8 byNum = 0;

            bRet = g_cMcuVcApp.GetVCSUsrGrpUserCount(byUsrGrpId, byMaxNum, byNum);
            if (byNum >= byMaxNum)
            {
                // ��ǰ���û���
			    cServMsg.SetErrorCode(UM_NOT_ENOUGHMEM);
			    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			    return;
            }

			// ����û���Ϣ
		    if (g_cVCSUsrManage.AddUser(ptVCSUserInfo))
		    {       
			    SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

			    cServMsg.SetMsgBody((u8*)ptVCSUserInfo, sizeof(CVCSUsrInfo));

                // ֪ͨ�����û���
				g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn( byUsrGrpId, pcMsg->event + 3, 
											         cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		    }
		    else
		    {
			    cServMsg.SetErrorCode(g_cVCSUsrManage.GetLastError());
			    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				return;
		    }

			// ����û������������Ϣ
			TUserTaskInfo tUserTaskInfo;
			tUserTaskInfo.SetUserTaskInfo(ptVCSUserInfo->GetName(), ptVCSUserInfo->GetTaskNum(), ptVCSUserInfo->GetTask());
		    g_cMcuVcApp.AddVCSUserTaskInfo(tUserTaskInfo); 

        }
		break;

	case MCS_MCU_DELUSER_REQ:       //VCS����MCUɾ���û�
		{
			if (cServMsg.GetMsgBodyLen() < sizeof(CVCSUsrInfo))
			{
				OspPrintf(TRUE, FALSE, "[ProcVCSUserManagerMsg] error:MCS_MCU_DELUSER_REQ with wrong message body!\n");
				return;
			}
			ptVCSUserInfo  = (CVCSUsrInfo*)cServMsg.GetMsgBody();
			byUsrGrpId = ptVCSUserInfo->GetUsrGrpId();
			
			if ( byMcsGrpId != USRGRPID_SADMIN && 
				 byMcsGrpId != byUsrGrpId )
			{
				// ���ܲ�����������
				cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
				SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				return;
			}

			if (0 == strncmp(m_achUser, ptVCSUserInfo->GetName(), (MAXLEN_PWD-1)))
			{
				// ����ɾ���Լ�
				cServMsg.SetErrorCode(UM_ACTOR_ONLY);
				SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				return;
			}

			cServMsg.SetMsgBody((u8*)ptVCSUserInfo->GetName(), MAX_CHARLENGTH);
			if (g_cVCSUsrManage.DelUser(ptVCSUserInfo))
			{
				SendMsgToMcs( pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

				// ֪ͨ�����û���
				g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn( byUsrGrpId, pcMsg->event + 3, 
													  cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			}
			else
			{
				cServMsg.SetErrorCode( g_cVCSUsrManage.GetLastError() );
				SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				return;
			}

			// ɾ���û���Ӧ��������Ϣ
			g_cMcuVcApp.DelVCSUserTaskInfo(ptVCSUserInfo->GetName());
		}

		break;

	case MCS_MCU_CHANGEUSER_REQ:  //VCS����MCU�޸��û�
		{
			if (cServMsg.GetMsgBodyLen() < sizeof(CVCSUsrInfo))
			{
				OspPrintf(TRUE, FALSE, "[ProcVCSUserManagerMsg] error:MCS_MCU_CHANGEUSER_REQ with wrong message body!\n");
				return;
			}

			ptVCSUserInfo  = (CVCSUsrInfo*)cServMsg.GetMsgBody();
			byUsrGrpId = ptVCSUserInfo->GetUsrGrpId();

			cServMsg.SetMsgBody((u8*)ptVCSUserInfo->GetName(), MAX_CHARLENGTH);

			if ( byMcsGrpId != USRGRPID_SADMIN && 
				 byMcsGrpId != byUsrGrpId )
			{
				// ���ܲ�����������
				cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
				SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());		
				return;
			}
        
			if (g_cVCSUsrManage.ModifyInfo(ptVCSUserInfo))
			{
				SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

				cServMsg.SetMsgBody((u8*)ptVCSUserInfo, sizeof(CVCSUsrInfo));
				g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn( byUsrGrpId, pcMsg->event + 3, 
													  cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			}
			else
			{
				cServMsg.SetErrorCode( g_cVCSUsrManage.GetLastError() );
				SendMsgToMcs( pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			}

			// �޸��û������������Ϣ
			TUserTaskInfo tUserTaskInfo;
			tUserTaskInfo.SetUserTaskInfo(ptVCSUserInfo->GetName(), ptVCSUserInfo->GetTaskNum(), ptVCSUserInfo->GetTask());
			g_cMcuVcApp.ChgVCSUserTaskInfo(tUserTaskInfo); 
		}
			
			break;
		
	case MCS_MCU_GETUSERLIST_REQ:   //VCS����MCU�õ���ǰ�û������е��û��б�
		{
			CLongMessage cLongMsg;
			u8 abyExUsrBuf[sizeof(CExUsrInfo) * USERNUM_PERPKT];
			u8 abyVCSUserBuf[sizeof(CVCSUsrInfo) * USERNUM_PERPKT];

			u8 byUsrItr = 0; // �����û���ָ��
			u8 byUserNumInPack = 0;

            BOOL32 bRet = FALSE;
            u8 byUsrMaxNum = 0;
            u8 byUsrNum = 0;

            bRet = g_cMcuVcApp.GetVCSUsrGrpUserCount(byMcsGrpId, byUsrMaxNum, byUsrNum);
            
            // ������Ҫ�����ܰ���
            cLongMsg.m_uTotal = byUsrNum / USERNUM_PERPKT;
            if ( byUsrNum % USERNUM_PERPKT != 0 )
            {
                cLongMsg.m_uTotal++;
            }
            cLongMsg.m_uCurr = 1;

			if ( !bRet || byUsrNum == 0)
			{
                if (!bRet)
                {
                    // ��ȡʧ��,Nack
                    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());  // Nack
                }
                else
                {
                    // ����Ϊ0,Ack
                    SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()); // Ack
                }
				
				OspPrintf(TRUE, FALSE, "[ProcVCSUserManagerMsg] No users in current group\n");
			}
			else
			{
				SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()); // Ack

                // Notify
				while ( cLongMsg.m_uCurr <= cLongMsg.m_uTotal ) 
				{
					byUserNumInPack = 0;
					if ( !g_cMcuVcApp.GetVCSCurUserList(byMcsGrpId, abyExUsrBuf, byUsrItr, byUserNumInPack) )
					{
						OspPrintf(TRUE, FALSE, "[ProcVCSUserManagerMsg] GetVCSCurUserList Failed at index: %d\n", byUsrItr);
                        return;
					}
					cLongMsg.m_uNumUsr = byUserNumInPack;

					PackUserTaskInfo(byUserNumInPack, abyExUsrBuf, abyVCSUserBuf);

					cServMsg.SetMsgBody((u8*)&cLongMsg, sizeof(cLongMsg));
					cServMsg.CatMsgBody(abyVCSUserBuf, byUserNumInPack * sizeof(CVCSUsrInfo));

					SendMsgToMcs( pcMsg->event + 3, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

                    cLongMsg.m_uCurr ++;
				}
			}
		}
		break;

	// �û�����Ϣ���ֲ��䣬�����浽��ͬ����
    case MCS_MCU_ADDUSERGRP_REQ:    //VCS����û���
        // ֻ�г�������Ա��Ȩ��
        {
            tUsrGrpInfo = *(TUsrGrpInfo*)cServMsg.GetMsgBody();

            if ( byMcsGrpId != USRGRPID_SADMIN  )
            {
                // ���ܲ���
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
                cServMsg.SetMsgBody( (u8*)tUsrGrpInfo.GetUsrGrpName(), MAX_CHARLENGTH );
			    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                return;
            }
            else
            {
                u16 wErrorCode = 0;
                byUsrGrpId = g_cMcuVcApp.AddVCSUserGroup(tUsrGrpInfo, wErrorCode);
                if ( byUsrGrpId == USRGRPID_INVALID )
                {
			        cServMsg.SetErrorCode( wErrorCode );
                    cServMsg.SetMsgBody( (u8*)tUsrGrpInfo.GetUsrGrpName(), MAX_CHARLENGTH );
			        SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                }
                else
                {
                    tUsrGrpInfo.SetUsrGrpId(byUsrGrpId);
                    cServMsg.SetMsgBody( (u8*)&tUsrGrpInfo, sizeof(TUsrGrpInfo) );

                    SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	

                    cServMsg.SetEventId( MCU_MCS_USERGRP_NOTIF );
			        g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn( byUsrGrpId, MCU_MCS_USERGRP_NOTIF, 
											              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                }
            }
        }
        break;
        
    case MCS_MCU_CHANGEUSERGRP_REQ:    //VCS�޸��û���
        // ֻҪ��������Ա��Ȩ��
        {
            tUsrGrpInfo = *(TUsrGrpInfo*)cServMsg.GetMsgBody();
            u8 byTargetId = tUsrGrpInfo.GetUsrGrpId();
            if ( byMcsGrpId != USRGRPID_SADMIN  )
            {
                // ���ܲ���
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);                
                cServMsg.SetMsgBody( (u8*)&byTargetId, sizeof(u8) );
			    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                return;
            }
            else
            {
                u16 wErrorCode = 0;
                if ( !g_cMcuVcApp.ChgVCSUserGroup(tUsrGrpInfo, wErrorCode) )
                {
			        cServMsg.SetErrorCode( wErrorCode );
                    cServMsg.SetMsgBody( (u8*)&byTargetId, sizeof(u8) );
			        SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                }
                else
                {
                    SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	

                    cServMsg.SetEventId( MCU_MCS_USERGRP_NOTIF );

			        g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn( byMcsGrpId, MCU_MCS_USERGRP_NOTIF, 
											              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                }
            }
        }
        break;

    case MCS_MCU_DELUSERGRP_REQ:    //VCSɾ���û��鼰�����Դ
        // ֻҪ��������Ա��Ȩ��
        {
            u8 byDelGrpId = *cServMsg.GetMsgBody();
            if ( byMcsGrpId != USRGRPID_SADMIN )
            {
                // ���ܲ���
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
			    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());		
                return;
            }
            else
            {
                u16 wErrorCode = 0;
                if ( !g_cMcuVcApp.DelVCSUserGroup(byDelGrpId, wErrorCode) )
                {
			        cServMsg.SetErrorCode( wErrorCode );
			        SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                }
                else
                {
                    SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	

                    // Nofity
                    cServMsg.SetEventId( MCU_MCS_DELUSERGRP_NOTIF );
                    
			        g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn( byMcsGrpId, MCU_MCS_DELUSERGRP_NOTIF, 
											              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                }
            }
        }
        break;
        
    case MCS_MCU_GETUSERGRP_REQ:    //VCS�����û�����Ϣ
        // ֻҪ��������Ա��Ȩ��
        {
            if ( byMcsGrpId != USRGRPID_SADMIN )
            {
                // ���ܲ���
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
                cServMsg.SetMsgBody();
			    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());		
            }
            else
            {
                u8 byNum = 0;
                TUsrGrpInfo *ptInfo = NULL;
                g_cMcuVcApp.GetVCSUserGroupInfo( byNum, &ptInfo );
                
                cServMsg.SetMsgBody( &byNum, sizeof(u8) );
                cServMsg.CatMsgBody( (u8*)ptInfo, sizeof(TUsrGrpInfo) * byNum );

                SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
            }
        }
        break;

    default:
        break;
	}

	return;
}
/*====================================================================
    ������      ��ProcVCSMcuUnProcCfgMsg
    ����        ������VCS����MCU���账����Ϣ�����ü���ȡ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2008/11/20              ���㻪        ����
====================================================================*/
void CVcsSsnInst::ProcVCSMcuUnProcCfgMsg( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8* pbyMsgBody = cServMsg.GetMsgBody();

	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUUNPROCCFGFILENAME);

	switch(pcMsg->event)
	{
	case VCS_MCU_SETUNPROCFGINFO_REQ:
		{
			u32 dwCfgLen = cServMsg.GetMsgBodyLen();
			dwCfgLen = (dwCfgLen > MAXLEN_MCUUNPROCCFG_INI ? MAXLEN_MCUUNPROCCFG_INI : dwCfgLen);

			FILE* hFile = NULL;
			if ((hFile = fopen(achFullName, "wb")) != NULL)
			{
			    SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				fwrite(pbyMsgBody, sizeof(u8), dwCfgLen, hFile);
				fclose(hFile);
			}
			else
			{
				VcsLog("[ProcVCSMcuUnProcCfgMsg] fail to open mcuunproccfg.ini\n");
				SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				return;
			}
			CMcsSsn::BroadcastToAllMcsSsn(pcMsg->event + 3, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

		}

		break;
	case VCS_MCU_GETUNPROCFGINFO_REQ:
		{
			FILE* hFile = NULL;
			if ((hFile = fopen(achFullName, "rb")) != NULL)
			{
				u8 achCfgInfo[MAXLEN_MCUUNPROCCFG_INI];
				memset(achCfgInfo, 0, sizeof(achCfgInfo));						
				u32 dwCfgLen = fread(achCfgInfo, sizeof(u8), sizeof(achCfgInfo), hFile);
				fclose(hFile);
				cServMsg.SetMsgBody(achCfgInfo, (u16)dwCfgLen);
				SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			}
			else
			{
				VcsLog("[ProcVCSMcuUnProcCfgMsg] fail to open mcuunproccfg.ini\n");
				SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				return;
			}

		}
		break;
	default:
	    break;
	}


}
/*====================================================================
    ������      ��PackUserTaskInfo
    ����        �����û�������Ϣ������û���Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[IN]byUserNumInPack��������û���
	              [IN]pbyExUserBuf���������û�������Ϣ��������ָ��
	              [OUT]pbyVCSUserBuf�������û�������Ϣ��������ָ��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2008/11/20              ���㻪        ����

====================================================================*/
void CVcsSsnInst::PackUserTaskInfo(u8 byUserNumInPack, u8 *pbyExUserBuf, u8 *pbyVCSUserBuf)
{
	TUserTaskInfo tUserTaskInfo;
	CVCSUsrInfo   cVCSUserInfo;
	CExUsrInfo*   pcVCSUserInfo;
	for (u16 wIndex = 0; wIndex < byUserNumInPack; wIndex++)
	{
		cVCSUserInfo.SetNull();
		tUserTaskInfo.SetNull();

		pcVCSUserInfo = (CExUsrInfo*)&cVCSUserInfo;
		memcpy(pcVCSUserInfo, pbyExUserBuf, sizeof(CExUsrInfo));		
		if (g_cMcuVcApp.GetSpecVCSUserTaskInfo(((CExUsrInfo*)pbyExUserBuf)->GetName(), tUserTaskInfo))
		{
			cVCSUserInfo.AssignTask(tUserTaskInfo.GetUserTaskNum(), tUserTaskInfo.GetUserTaskInfo());
		}
		memcpy(pbyVCSUserBuf, &cVCSUserInfo, sizeof(cVCSUserInfo));

		pbyExUserBuf  += sizeof(CExUsrInfo);
		pbyVCSUserBuf += sizeof(CVCSUsrInfo); 
	}

}
/*====================================================================
    ������      ��ProcVCSConfTemMsg
    ����        ������VCS�Ļ���ģ����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2008/12/18              ���㻪        ����
====================================================================*/
void CVcsSsnInst::ProcVCSConfTemMsg( CMessage * const pcMsg )
{
	CServMsg cServMsg(pcMsg->content,pcMsg->length);

	switch( CurState() )
	{
	case STATE_NORMAL:
		switch(pcMsg->event)
		{
		case MCS_MCU_MODIFYTEMPLATE_REQ:
		case MCS_MCU_DELTEMPLATE_REQ:
			{
				// VCS�������ڿ���ĵ���ϯ�������޸�/ɾ��
				CConfId cConfId = cServMsg.GetConfId();
			    u8 byConfIdx = g_cMcuVcApp.GetConfIdx(cServMsg.GetConfId());

                TTemplateInfo tOldTemInfo;
                if(!g_cMcuVcApp.GetTemplate(byConfIdx, tOldTemInfo))
                {
                    cServMsg.SetErrorCode(ERR_MCU_TEMPLATE_NOTEXIST);
                    VcsLog( "[ProcVCSConfTemMsg] get template confIdx<%d> failed\n", byConfIdx);
                    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                    return;
                }

				// ������ǰ�����Ƿ�����ɸô��޸�ģ����������Ļ���
				if(g_cMcuVcApp.IsConfE164Repeat(tOldTemInfo.m_tConfInfo.GetConfE164(), FALSE ))
				{
					cServMsg.SetErrorCode(ERR_MCU_TEMPLATE_INVC);
					VcsLog("[ProcVCSConfTemMsg] template is in vc\n");
					SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
					return;
				}

				if (MCS_MCU_DELTEMPLATE_REQ == pcMsg->event)
				{
					// VCS����ϯɾ�����Ӧɾ�����û����õĸõ���ϯ����
					CConfId cConfId = cServMsg.GetConfId();
					g_cMcuVcApp.DelVCSSpecTaskInfo(cConfId);
				}
			}
			break;
		default:
		    break;
		}
		// ���⴦����ɽ������ദ��
		CMcsSsnInst::InstanceEntry( pcMsg );		
		break;

	default:
		OspPrintf(TRUE, FALSE, "[ProcVCSConfTemMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}	

}

/*====================================================================
    ������      ��ProcVCSMcuCreateConfMsg
    ����        ������VCS�Ĵ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2008/12/04              ���㻪        ����

====================================================================*/
void CVcsSsnInst::ProcVCSMcuCreateConfMsg( const CMessage * pcMsg )
{
	CServMsg	cServMsg(pcMsg->content,pcMsg->length);
    
    
//     //  n+1����ģʽ��������ģ��
//     if (pcMsg->event == MCS_MCU_CREATECONF_BYTEMPLATE_REQ ||
//         pcMsg->event == MCS_MCU_CREATESCHCONF_BYTEMPLATE_REQ ||
//         pcMsg->event == MCS_MCU_CREATETEMPLATE_REQ ||
//         pcMsg->event == MCS_MCU_SAVECONFTOTEMPLATE_REQ)
//     {
//         if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_IDLE ||
//             g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_SWITCH)
//         {
//             OspPrintf(TRUE, FALSE, "[ProcMcsMcuDaemonConfMsg] mcs can't create template in n+1 mode!\n");
//             cServMsg.SetErrorCode(ERR_MCU_NPLUS_CREATETEMPLATE);
//             SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
//             return;
//         }
//     }    

	switch( CurState() )
	{
	case STATE_NORMAL:
	    cServMsg.SetMcuId( LOCAL_MCUID );
		cServMsg.SetSrcSsnId( ( u8 )GetInsID() );
		cServMsg.SetSrcMtId( 0 );
		cServMsg.SetEventId( pcMsg->event );
		g_cMcuVcApp.SendMsgToDaemonConf( cServMsg.GetEventId(), cServMsg.GetServMsg(), 
			                             cServMsg.GetServMsgLen() );
		break;

	default:
		OspPrintf(TRUE, FALSE, "[ProcVCSMcuCreateConfMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
}
/*====================================================================
    ������      ��ProcVCSMcuVCMsg
    ����        ������VCS�ĵ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2008/12/08              ���㻪        ����

====================================================================*/
void CVcsSsnInst::ProcVCSMcuVCMsg( const CMessage * pcMsg )
{
	VcsLog("[CVcsSsnInst%d][ProcVCSMcuVCMsg]: Console%d receive command message %d(%s)!\n",
			GetInsID(), GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));  

    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
    switch (CurState())
    {
    case STATE_NORMAL:
        cServMsg.SetMcuId(LOCAL_MCUID);
        cServMsg.SetSrcSsnId((u8)GetInsID());
        cServMsg.SetSrcMtId(0);
        cServMsg.SetEventId(pcMsg->event);
        
        g_cMcuVcApp.SendMsgToConf(cServMsg.GetConfId(), pcMsg->event,
                                  cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		
        break;

    default:
		OspPrintf(TRUE, FALSE, "[ProcMcsMcuMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        break;
    }
}

/*====================================================================
    ������      ��ProcMcuVcsMsg
    ����        ������VCS����Ӧ����Ϣͳһ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2008/12/08              ���㻪        ����
====================================================================*/
void CVcsSsnInst::ProcMcuVcsMsg( const CMessage * pcMsg )
{
	VcsLog("[CVcsSsnInst%d][ProcMcuVcsMsg]: Command reply Message %u(%s) to Console%d received!\n",
			GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event), GetInsID() );


	switch( CurState() )
	{
	case STATE_NORMAL:
        SendMsgToMcs( pcMsg->event, pcMsg->content, pcMsg->length );
		break;

	default:
		OspPrintf(TRUE, FALSE, "[ProcMcuVcsMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
}
/*====================================================================
    ������      ��ProcVCSConfOprMsg
    ����        ������VCS��ʼ����ϯ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2008/12/12              ���㻪        ����
====================================================================*/
void CVcsSsnInst::ProcVCSConfOprMsg( const CMessage * pcMsg )
{
	VcsLog("[ProcVCSConfOprMsg]: Console.%u receive command message %u(%s)!\n", 
            GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));

	CServMsg cServMsg(pcMsg->content, pcMsg->length);


	CConfId cConfId = cServMsg.GetConfId();

	switch( CurState() )
	{
	case STATE_NORMAL:
        switch( pcMsg->event)
        {
        case VCS_MCU_OPRCONF_REQ:
			{
				// ���������ϼ�����ϯ�������¼�����ϯ����������VCS����
				CMcuVcInst* pVCInst = NULL;
				for (u8 byConfIdx= MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
				{
					pVCInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
					if (pVCInst != NULL && pVCInst->m_tConf.GetConfId() == cConfId &&
						CONF_CREATE_MT == pVCInst->m_byCreateBy)
					{
						cServMsg.SetErrorCode(ERR_MCU_VCS_MMCUINVC);
						SendMsgToMcs(cServMsg.GetEventId() + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
						return;						
					}

				}
				TUserTaskInfo tTaskInfo;
				if(g_cMcuVcApp.IsYourTask(m_achUser, cConfId))
				{
					SendMsgToMcs(cServMsg.GetEventId() + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
					g_cVcsSsnApp.SetCurConfID(GetInsID(), cConfId);
				}
				else
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_NOTALLOWOPR);
					SendMsgToMcs(cServMsg.GetEventId() + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
					return;
				}
			}
        	break;
        case VCS_MCU_QUITCONF_REQ:
			{
				if (g_cVcsSsnApp.GetCurConfID(GetInsID()) == cConfId)
				{
					SendMsgToMcs(cServMsg.GetEventId() + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
					QuitCurVCConf();
				}
				else
				{
					VcsLog("[ProcVCSConfOprMsg] VCS_MCU_QUITCONF_REQ to quit conf which is not current operated conf\n");
					SendMsgToMcs(cServMsg.GetEventId() + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
					return;
				}
			}

            break;
        default:
            break;
        }
		break;

	default:
		OspPrintf(TRUE, FALSE, "[ProcVCSConfOprMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
}
/*====================================================================
    ������      ��QuitCurVCConf
    ����        ���˳���ǰ����ϯ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2008/12/12              ���㻪        ����
====================================================================*/
void CVcsSsnInst::QuitCurVCConf()
{
	CConfId cConfID = g_cVcsSsnApp.GetCurConfID(GetInsID());
	if (cConfID.IsNull())
	{
		return;
	}
	
	CConfId cNull;
	g_cVcsSsnApp.SetCurConfID(GetInsID(), cNull);
	if (!g_cVcsSsnApp.ChkVCConfStatus(cConfID))
	{
		// û���κε���Ա�����û��飬���Զ������û���
		CServMsg cServMsg;
        cServMsg.SetMcuId(LOCAL_MCUID);
        cServMsg.SetSrcSsnId(0);       // ����Ҫ��ӦACK��Ϣ������̨
        cServMsg.SetSrcMtId(0);
		cServMsg.SetConfId(cConfID);
        cServMsg.SetEventId(MCS_MCU_RELEASECONF_REQ);
        
        g_cMcuVcApp.SendMsgToConf(cConfID, MCS_MCU_RELEASECONF_REQ,
                                  cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
	}

	
}


/*====================================================================
    ������      ��ChkVCConfStatus
    ����        �����ָ���Ļ����Ƿ��Դ��ڱ�����״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2008/12/15              ���㻪        ����
====================================================================*/
BOOL CVcsSsn::ChkVCConfStatus( CConfId cConfId )
{
	for (u8 byInstId = MAXNUM_MCU_MC + 1; byInstId <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byInstId++)
	{
		if (cConfId == m_acCurConfID[byInstId - 1])
		{
			return TRUE;
		}
		
	}

	return FALSE;
}
/*====================================================================
                         CVcsSsn 
====================================================================*/
CVcsSsn::CVcsSsn( void )
{
}

CVcsSsn::~CVcsSsn( void )
{
}
/*====================================================================
    ������      ��SetCurConfID
    ����        ������ָ��ʵ�������Ļ���ID��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2008/12/15              ���㻪        ����
====================================================================*/
void CVcsSsn::SetCurConfID( u16 byInsId, CConfId& cConfID )
{
	if (byInsId < 1)
	{
		return;
	}
	m_acCurConfID[byInsId - 1] = cConfID;
}

/*====================================================================
    ������      ��GetCurConfID
    ����        ����ȡָ��ʵ�������Ļ���ID��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2008/12/15              ���㻪        ����
====================================================================*/
CConfId CVcsSsn::GetCurConfID( u16 byInsId )
{
	return m_acCurConfID[byInsId - 1];
}

/*====================================================================
    ������      ��Broadcast2SpecGrpVcsSsn
    ����        ������Ϣ��ͬ�û����VCS��Ӧ�ĻỰʵ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byGrdId, ���
				  u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ�룬ȱʡΪNULL
				  u16 wLen, ��Ϣ���ȣ�ȱʡΪ0
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/21                ���㻪        ����
====================================================================*/
void CVcsSsn::Broadcast2SpecGrpVcsSsn( u8 byGrdId, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//�����������������ⲿ��AppͶ����Ϣ��������
	if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
	{
		return;
	}

    u8 bySsnGrpId = 0;
	for (u8 byInstID = MAXNUM_MCU_MC + 1; byInstID <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byInstID++)
	{
        bySsnGrpId = CMcsSsn::GetUserGroup( byInstID );
        if ( bySsnGrpId != USRGRPID_INVALID )
        {
            if ( bySsnGrpId == byGrdId ||
                 bySsnGrpId == USRGRPID_SADMIN )
            {
				// ������ض�����߳����û���
				::OspPost(MAKEIID( AID_MCU_VCSSN, byInstID ), wEvent, pbyMsg, wLen);
                VcsLog( "[Broadcast2SpecGrpVcsSsn]: Message %u(%s) In InsID.%u is send for Spec Group Id(%d) <--> Ssn Group Id(%d)\n",
                        wEvent, ::OspEventDesc( wEvent ), byInstID, 
                        byGrdId, bySsnGrpId);
            }
            else
            {
            
                VcsLog( "[Broadcast2SpecGrpVcsSsn]: Message %u(%s) In InsID.%u is Dropped for Spec Group Id(%d) <--> Ssn Group Id(%d)\n",
                        wEvent, ::OspEventDesc( wEvent ), byInstID, 
                        byGrdId, bySsnGrpId);
            }
        }
	}

	return;
}

/*====================================================================
    ������      ��GetVCSSSsnData
    ����        ����ȡ��Ҫͬ����VCSSSn��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/21                ���㻪        ����
====================================================================*/
BOOL32 CVcsSsn::GetVCSSSsnData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen)
{
	if (NULL == pbyBuf)
	{
		OspPrintf(TRUE, FALSE, "[GetVCSSSsnData] NULL == pbyBuf\n");
		return FALSE;
	}

	dwOutBufLen = sizeof(m_acCurConfID);
	if (dwInBufLen < dwOutBufLen)
	{
		OspPrintf(TRUE, FALSE, "[GetVCSSSsnData] dwInBufLen(%d) < dwOutBufLen(%d)\n", 
			      dwInBufLen, dwOutBufLen);
		return FALSE;
	}

	memcpy(pbyBuf, &m_acCurConfID, dwOutBufLen);
	return TRUE;
}

/*====================================================================
    ������      ��SetVCSSSsnData
    ����        ��������Ҫͬ����VCSSSn��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	08/11/21                ���㻪        ����
====================================================================*/
BOOL32 CVcsSsn::SetVCSSSsnData( u8 *pbyBuf, u32 dwBufLen )
{
	if (NULL == pbyBuf)
	{
		OspPrintf(TRUE, FALSE, "[SetVCSSSsnData] NULL == pbyBuf\n");
		return FALSE;
	}

	if (dwBufLen != sizeof(m_acCurConfID))
	{
		OspPrintf(TRUE, FALSE, "[SetVCSSSsnData] input lenth(%d) is not correct\n", dwBufLen);
		return FALSE;
	}

	memcpy(&m_acCurConfID, pbyBuf, dwBufLen);
	return TRUE;
}




















































